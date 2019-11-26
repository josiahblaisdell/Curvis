#pragma once
#include "HatchWidget.h"

HatchWidget::HatchWidget(QWidget *parent)
	: GLSLWidget(parent)
{
	_dataShader = NULL;
	_m_camera_position = { 0.0f, 0.0f, -7.0f };
	_m_camera_up = QVector3D(0.0f, 1.0f, 0.0f);
	connect(_dataShader, SIGNAL(GLSLShader::fragShaderCompileSuccess), this, SLOT(DataViewerWidget::fragSuccess));
	connect(_dataShader, SIGNAL(GLSLShader::vertShaderCompileSuccess), this, SLOT(DataViewerWidget::vertSuccess));
	verts_n = 0;
	norms_n = 0;
	colors_n = 0;
	indices_n = 0;
	isTriMesh = false;
	isReady = false;

}

void HatchWidget::fragSuccess() { _fragSuccess = true; }
void HatchWidget::vertSuccess() { _vertSuccess = true; }
bool HatchWidget::getCompileStatus(GLenum shadertype)
{
	switch (shadertype)
	{
	case GL_VERTEX_SHADER: return _vertSuccess; break;
	case GL_FRAGMENT_SHADER: return _fragSuccess; break;
	default:
		return false; break;
	}
}
void HatchWidget::cleanup()
{
	if (_program == nullptr)
		return;
	makeCurrent();
	delete _program;
	_program = 0;
	doneCurrent();
}

glm::vec3 HatchWidget::CalcFaceNormal(SimpleTriFace face) {
	PolyMesh::Point u = trimesh.point(face.v2) - trimesh.point(face.v1);
	PolyMesh::Point v = trimesh.point(face.v3) - trimesh.point(face.v1);
	PolyMesh::Point pN = OpenMesh::cross(u, v);
	return glm::vec3(pN[0], pN[1], pN[2]);
}

void HatchWidget::OnUpdate() {
	_f->glClearColor(1, 1, 1, _isTransparent ? 0 : 1);
	_f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_f->glEnable(GL_DEPTH_TEST);
	_context->makeCurrent(_surface);
	QMatrix3x3 NormalMatrix = (m_world*m_scale).normalMatrix();
	if (isReady) {
		_dataShader->Use();
		_dataShader->CheckGlErrors("Update() (Use Shader)");
		_ef->glBindVertexArray(m_VAO);
		_dataShader->CheckGlErrors("Update() (Use Vertex Array Obj)");
		_dataShader->SetUniform("uProjection", m_proj);
		_dataShader->SetUniform("uModelMatrix", m_world*m_scale);
		_dataShader->SetUniform("uViewMatrix", m_camera);
		_dataShader->SetUniform("uNormalMatrix", NormalMatrix.data());
		_dataShader->CheckGlErrors("Update() (Setting Uniforms)");
		if (isTriMesh) {
			context()->extraFunctions()->glDrawElements(GL_TRIANGLES, indices_n, GL_UNSIGNED_INT, (GLvoid*)0);
			_dataShader->CheckGlErrors("Update() (glDrawElements())");
		}
		//else context()->extraFunctions()->glDrawElements(GL_QUADS, isize, GL_UNSIGNED_INT, (GLvoid*)0);
		
	}
}

void HatchWidget::GetPolyMesh() {
	verts_n = 0;
	norms_n = 0;
	colors_n = 0;
	indices_n = 0;
	vertices.clear();
	normals.clear();
	colors.clear();
	indices.clear();
	//get the face & vertex normals
	polymesh.update_normals();
	PolyMesh::FaceIter f_it, f_end(polymesh.faces_end());
	PolyMesh::FaceVertexIter fv_it, fv_end;
	PolyMesh::VertexIter v_it, v_end(polymesh.vertices_end());
	//create the vertices array
	for (v_it = polymesh.vertices_begin(); v_it != v_end; v_it++) {
		float x = polymesh.point(v_it.handle())[0];
		float y = polymesh.point(v_it.handle())[1];
		float z = polymesh.point(v_it.handle())[2];
		vertices.push_back(glm::vec4(x, y, z, 1.f));
		verts_n++;
	}
	//create the normals array (per vertex normals)
	for (v_it = polymesh.vertices_begin(); v_it != v_end; v_it++) {
		OpenMesh::Vec3f normal = polymesh.normal(*v_it);
			//polymesh.calc_vertex_normal(v_it.handle());
		GLfloat x = normal[0];
		GLfloat y = normal[1];
		GLfloat z = normal[2];
		normals.push_back(glm::vec3(x, y, z));
		norms_n++;
	}
	//indices for vertices are given by iterating over faces.
	for (f_it = polymesh.faces_begin(); f_it != f_end; f_it++) {
		fv_end = polymesh.fv_end(*f_it);
		for (fv_it = polymesh.fv_begin(*f_it); fv_it != fv_end; fv_it++) {
			GLuint idx = fv_it->idx();
			indices.push_back(idx);
			indices_n++;
		}
	}
	//create colors for vertices
	for (v_it = polymesh.vertices_begin(); v_it != v_end; v_it++) {
		GLfloat r = 1.f;
		GLfloat g = 0.f;
		GLfloat b = 0.f;
		GLfloat a = 1.f;
		colors.push_back(glm::vec4(r, g, b, a));
		colors_n++;
	}
	static_assert(sizeof(glm::vec3) == sizeof(GLfloat) * 3, "Platform doesn't support vec3 this directly.");
	static_assert(sizeof(glm::vec4) == sizeof(GLfloat) * 4, "Platform doesn't support vec4 this directly.");
	vsize = sizeof(GLfloat) * 4 * verts_n;
	nsize = sizeof(GLfloat) * 3 * norms_n;
	csize = sizeof(GLfloat) * 4 * colors_n;
	isize = sizeof(GLuint)*indices_n;
	polymesh.release_face_normals();
	polymesh.release_vertex_normals();
}

void HatchWidget::GetTriMesh() {
	verts_n = 0;
	norms_n = 0;
	colors_n = 0;
	indices_n = 0;
	vertices.clear();
	normals.clear();
	colors.clear();
	indices.clear();
	CurvComputer c;
	c.Execute(&trimesh);
	//get the face & vertex normals
	if (trimesh.has_vertex_normals()) {
		int x = 0;
	}
	//trimesh.update_normals();


	TriMesh::FaceIter f_it, f_end(trimesh.faces_end());
	TriMesh::FaceVertexIter fv_it, fv_end;
	TriMesh::VertexIter v_it, v_end(trimesh.vertices_end());
	//create the vertices array
	for (v_it = trimesh.vertices_begin(); v_it != v_end; ++v_it) {
		GLfloat x = trimesh.point(v_it.handle())[0];
		GLfloat y = trimesh.point(v_it.handle())[1];
		GLfloat z = trimesh.point(v_it.handle())[2];
		vertices.push_back(glm::vec4(x, y, z, 1.f));
		verts_n++;
	}
	//create the normals array
	trimesh.GenVertexNormalBuffer(&normals, &norms_n);
	//indices for vertices are given by iterating over faces.
	for (f_it = trimesh.faces_begin(); f_it != f_end; ++f_it) {
		TriMesh::FaceVertexCCWIter fv_ccw, fv_ccwend = trimesh.fv_ccwend(*f_it);
		for (fv_ccw = trimesh.fv_ccwbegin(*f_it); fv_ccw != fv_ccwend; ++fv_ccw) {
			GLuint idx = trimesh.vertex_handle(fv_ccw->idx()).idx();
			indices.push_back(idx);
			indices_n++;
		}
	}
	//create colors for vertices
	for (v_it = trimesh.vertices_begin(); v_it != v_end; ++v_it) {
		GLfloat r = 1.f;
		GLfloat g = 0.f;
		GLfloat b = 0.f;
		GLfloat a = 1.f;
		colors.push_back(glm::vec4(r, g, b, a));
		colors_n++;
	}
	static_assert(sizeof(glm::vec3) == sizeof(GLfloat) * 3, "Platform doesn't support vec3 this directly.");
	static_assert(sizeof(glm::vec4) == sizeof(GLfloat) * 4, "Platform doesn't support vec4 this directly.");
	vsize = sizeof(GLfloat) * 4 * verts_n;
	nsize = sizeof(GLfloat) * 3 * norms_n;
	csize = sizeof(GLfloat) * 4 * colors_n;
	isize = sizeof(GLuint)*indices_n;

}

//default shape is a cube.
void HatchWidget::OnInit() {
	PolyMesh::VertexHandle vhandle[8];
	std::vector<SimplePolyFace>  face_vhandles;
	vhandle[0] = polymesh.add_vertex(PolyMesh::Point(-1, -1, 1));
	vhandle[1] = polymesh.add_vertex(PolyMesh::Point(1, -1, 1));
	vhandle[2] = polymesh.add_vertex(PolyMesh::Point(1, 1, 1));
	vhandle[3] = polymesh.add_vertex(PolyMesh::Point(-1, 1, 1));
	vhandle[4] = polymesh.add_vertex(PolyMesh::Point(-1, -1, -1));
	vhandle[5] = polymesh.add_vertex(PolyMesh::Point(1, -1, -1));
	vhandle[6] = polymesh.add_vertex(PolyMesh::Point(1, 1, -1));
	vhandle[7] = polymesh.add_vertex(PolyMesh::Point(-1, 1, -1));

	face_vhandles.clear();
	//each element of face_vhandles is an array of vertex handles for a face.
	SimplePolyFace face1(vhandle[0], vhandle[1], vhandle[2], vhandle[3]);
	face_vhandles.push_back(face1);
	polymesh.add_face(vhandle[0], vhandle[1], vhandle[2], vhandle[3]);
	SimplePolyFace face2( vhandle[7], vhandle[6], vhandle[5], vhandle[4] );
	face_vhandles.push_back(face2);
	polymesh.add_face(vhandle[7], vhandle[6], vhandle[5], vhandle[4]);
	SimplePolyFace face3(vhandle[1], vhandle[0], vhandle[4], vhandle[5]);
	face_vhandles.push_back(face3);
	polymesh.add_face(vhandle[1], vhandle[0], vhandle[4], vhandle[5]);
	SimplePolyFace face4(vhandle[2], vhandle[1], vhandle[5], vhandle[6]);
	face_vhandles.push_back(face4);
	polymesh.add_face(vhandle[2], vhandle[1], vhandle[5], vhandle[6]);
	SimplePolyFace face5(vhandle[3], vhandle[2], vhandle[6], vhandle[7] );
	face_vhandles.push_back(face5);
	polymesh.add_face(vhandle[3], vhandle[2], vhandle[6], vhandle[7]);
	SimplePolyFace face6(vhandle[0], vhandle[3], vhandle[7], vhandle[4] );
	face_vhandles.push_back(face6);
	polymesh.add_face(vhandle[0], vhandle[3], vhandle[7], vhandle[4]);
	//get vertices, normals, and indices
	//GetPolyMesh();
	//initializeOpenGLFunctions();
	context()->functions()->initializeOpenGLFunctions();
	_context = context();
	_f = _context->functions();
	_ef = _context->extraFunctions();
	_surface = _context->surface();
	//SetupVertexArrayObject();
	// Camera never changes in this example.
	m_camera.setToIdentity();
	m_camera.translate(_m_camera_position.x(), _m_camera_position.y(), _m_camera_position.z());
	
}

bool HatchWidget::SetupVertexArrayObject() {
	_context->makeCurrent(_surface);
	//_dataShader->Use();
	//if (vaoObject == nullptr) {

		//connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLSLWidget::cleanup);
		//initializeOpenGLFunctions();
		//vaoObject = new QOpenGLVertexArrayObject();
		//eboObject = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
		//vboObject = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		//cboObject = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		//nboObject = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

		//Create the vertex array object to store state of vbo.
		//vaoObject->create();
		//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Create VAO)");
		//Create the vertex buffer
		//vboObject->create();
		//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Create VBO)");
		//Create the normal buffer
		//nboObject->create();
		//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Create NBO)");
		//Create the color buffer
		//cboObject->create();
		//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Create CBO)");
		//create the index buffer
		//eboObject->create();
		//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Create EBO)");
	//}

	//bind the vertex array
	//-----------------------------------------------------
	//vaoObject->bind();
	_ef->glGenVertexArrays(1, &m_VAO);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Create VAO)");
	_ef->glBindVertexArray(m_VAO);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (VAO Bind 1)");

	_ef->glEnableVertexAttribArray(0);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib array 0)");


	_ef->glEnableVertexAttribArray(1);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib array 1)");


	_ef->glEnableVertexAttribArray(2);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib array 2)");

	//-----------------------------------------------------

	//bind the vertex buffer to the current vertex array
	//-----------------------------------------------------
	//vboObject->bind();
	_ef->glGenBuffers(1, &m_VBO);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Create VBO)");
	_ef->glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (VBO Bind)");

	//vboObject->allocate(&vertices[0], vsize);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (VBO Allocate)");
	_ef->glBufferData(GL_ARRAY_BUFFER, vsize + nsize + csize, (GLvoid*)0, GL_STATIC_DRAW);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Create Buffer Data)");
	_ef->glBufferSubData(GL_ARRAY_BUFFER, 0, vsize, &vertices[0]);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Write Vertices)");
	_ef->glBufferSubData(GL_ARRAY_BUFFER, vsize, nsize, &normals[0]);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Write Normals)");
	_ef->glBufferSubData(GL_ARRAY_BUFFER, vsize + nsize, csize, &colors[0]);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Write Colors)");
	

	//vboObject->setUsagePattern(QOpenGLBuffer::StaticDraw);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (VBO UsagePattern)");
	//_ef->glBindVertexArray(m_VAO);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (VAO Bind 2)");
	_ef->glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib pointer 0)");
	_ef->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)vsize);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib pointer 1)");
	_ef->glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(vsize + nsize));
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib pointer 2)");
	//context()->functions()->glEnableVertexAttribArray(0);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib array 0)");
	//
	//context()->functions()->glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vertex attr pointer)");
	//-----------------------------------------------------

	//bind the normal buffer to the current vertex array
	//-----------------------------------------------------
	//nboObject->bind();
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (NBO Bind)");
	//
	//nboObject->allocate(&normals[0], nsize);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (VBO Allocate)");
	//
	//nboObject->setUsagePattern(QOpenGLBuffer::StaticDraw);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (VBO UsagePattern)");
	//
	//context()->functions()->glEnableVertexAttribArray(1);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable nbo attrib array 1)");
	//
	//context()->functions()->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)vsize);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable normals attr pointer)");
	//-----------------------------------------------------

	//bind the color buffer to the current vertex array
	//-----------------------------------------------------
	//cboObject->bind();
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (CBO Bind)");
	//
	//cboObject->allocate(&colors[0], csize);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (CBO Allocate)");
	//
	//nboObject->setUsagePattern(QOpenGLBuffer::StaticDraw);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (CBO UsagePattern)");
	//
	//context()->functions()->glEnableVertexAttribArray(2);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable cbo attrib array 3)");
	//
	//context()->functions()->glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(nsize + vsize));
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vertex colors pointer)");
	//-----------------------------------------------------

	//bind the indices buffer to the current vertex array
	//-----------------------------------------------------
	//qDebug() << eboObject->bind();
	_ef->glGenBuffers(1, &m_EBO);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Create EBO)");
	_ef->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Bind EBO)");
	_ef->glBufferData(GL_ELEMENT_ARRAY_BUFFER, isize, &indices[0], GL_STATIC_DRAW);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Write EBO)");
	//eboObject->allocate(&indices[0], isize);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Allocate & initialize EBO)");
	//eboObject->setUsagePattern(QOpenGLBuffer::StaticDraw);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Usage Pattern EBO)");

	//_ef->glUseProgram(0);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Unbind VAO)");
	//_ef->glBindVertexArray(0);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Unbind VAO)");

	//_ef->glBindVertexArray(m_VAO);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Test Bind)");
	//_ef->glBindVertexArray(0);
	//_dataShader->CheckGlErrors("SetupVertexArrayObject() (Test UnBind)");

	return true;
}

void HatchWidget::updateMesh() {
	//get vertices, normals, and indices
	if(isTriMesh) GetTriMesh();
	//else GetPolyMesh();
	//setup vao, vbo and ebo
	SetupVertexArrayObject();

	// Set camera to a standard position
	m_camera.setToIdentity();
	m_camera.translate(_m_camera_position.x(), _m_camera_position.y(), _m_camera_position.z());
	
}


HatchWidget::~HatchWidget(){}

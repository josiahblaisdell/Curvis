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
	vaoObject = nullptr;
	eboObject = nullptr;
	vboObject = nullptr;
	isTriMesh = false;

}

void HatchWidget::fragSuccess() { _fragSuccess = true; }
void HatchWidget::vertSuccess() { _vertSuccess = true; }
QOpenGLContext * HatchWidget::getContext(){ return _context; }
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
	vboObject->destroy();
	eboObject->destroy();
	vaoObject->destroy();
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

void HatchWidget::setupVertexAttribs()
{
	qDebug() << vboObject->bind();
	context()->functions()->glEnableVertexAttribArray(0);
	context()->functions()->glEnableVertexAttribArray(1);
	context()->functions()->glEnableVertexAttribArray(2);
	context()->functions()->glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	//offset for normals is 4*pts.size() (size of vertices array)
	context()->functions()->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(sizeof(GLfloat)*4* verts_n));
	//offset for colors is 4*pts.size() (size of vertices array) + 3*pts.size() (size of normals array)
	context()->functions()->glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(sizeof(GLfloat) *(4*verts_n + 3*norms_n)));
	vboObject->release();
}

void HatchWidget::OnUpdate() {
	context()->functions()->glClearColor(1, 1, 1, _isTransparent ? 0 : 1);
	context()->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	context()->functions()->glEnable(GL_DEPTH_TEST);
	if (_dataShader != NULL) {
		vaoObject->bind();
		_dataShader->Use();
		_dataShader->SetUniform("uProjection", m_proj);
		_dataShader->SetUniform("uModelMatrix", m_world*m_scale);
		_dataShader->SetUniform("uViewMatrix", m_camera);
		//_dataShader->SetUniform("uEyePos", glm::vec3(0.0,0.0,0.0));
		if (isTriMesh) {
			context()->extraFunctions()->glDrawElements(GL_TRIANGLES, isize-1, GL_UNSIGNED_INT, (GLvoid*)0);
		}
		//else context()->extraFunctions()->glDrawElements(GL_QUADS, isize, GL_UNSIGNED_INT, (GLvoid*)0);
		_dataShader->CheckGlErrors("Update()");
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
	//get the face & vertex normals
	trimesh.request_face_normals();
	trimesh.update_face_normals();
	trimesh.request_vertex_normals();
	trimesh.update_vertex_normals();
	//trimesh.request_halfedge_normals();
	//trimesh.request_vertex_normals();
	if (trimesh.has_vertex_normals()) {
		int x = 0;
	}
	//trimesh.update_normals();


	TriMesh::FaceIter f_it, f_end(trimesh.faces_end());
	TriMesh::FaceVertexIter fv_it, fv_end;
	TriMesh::VertexIter v_it, v_end(trimesh.vertices_end());
	//for (f_it = trimesh.faces_begin(); f_it != f_end; ++f_it) {
	//	fv_it = trimesh.fv_begin(*f_it);
	//	fv_end = trimesh.fv_end(*f_it);
	//	TriMesh::VertexHandle handles[3];
	//	int k = 0;
	//	for (fv_it = trimesh.fv_begin(*f_it); fv_it != fv_end; ++fv_it) {
	//		handles[k] = trimesh.vertex_handle(fv_it->idx());
	//		k++;
	//	}

	//	SimpleTriFace f(handles, trimesh.face_handle(f_it->idx()));
	//	glm::vec3 normal = CalcFaceNormal(f);
	//	OpenMesh::Vec3f n(normal.x, normal.y, normal.z);
	//	trimesh.set_normal(*f_it, n);
	//}
	//create the vertices array
	for (v_it = trimesh.vertices_begin(); v_it != v_end; ++v_it) {
		GLfloat x = trimesh.point(v_it.handle())[0];
		GLfloat y = trimesh.point(v_it.handle())[1];
		GLfloat z = trimesh.point(v_it.handle())[2];
		vertices.push_back(glm::vec4(x, y, z, 1.f));
		TriMesh::Normal n(0, 0, 0);
		n = trimesh.normal(v_it.handle());
		n = n.normalize();
		normals.push_back(glm::vec3(n[0], n[1], n[2]));
		verts_n++;
		norms_n++;
	}
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
	GetPolyMesh();
	_context = context();
	SetupVertexArrayObject();
	// Camera never changes in this example.
	m_camera.setToIdentity();
	m_camera.translate(_m_camera_position.x(), _m_camera_position.y(), _m_camera_position.z());
	
}

bool HatchWidget::SetupVertexArrayObject() {

	if (vaoObject == nullptr) {

		connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLSLWidget::cleanup);
		initializeOpenGLFunctions();
		vaoObject = new QOpenGLVertexArrayObject();
		eboObject = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
		vboObject = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

		//Create the vertex array object to store state of vbo.
		vaoObject->create();
		//Create the vertex buffer
		vboObject->create();
		//create the index buffer
		eboObject->create();
	}
	//bind the vertex array
	vaoObject->bind();

	//bind the vertex buffer to the current vertex array

	vboObject->bind();
	vboObject->allocate(0, sizeof(GLfloat)*(vsize + nsize + csize));
	vboObject->setUsagePattern(QOpenGLBuffer::StaticDraw);

	//write vertex data to graphics card
	vboObject->write(0, vertices.data(), vsize);
	//write normals data to graphics cards
	vboObject->write(vsize, normals.data(), nsize);
	//write colors data to graphics card.
	vboObject->write(vsize + nsize, colors.data(), csize);


	qDebug() << eboObject->bind();
	eboObject->allocate(indices.data(), isize);
	eboObject->setUsagePattern(QOpenGLBuffer::StaticDraw);

	qDebug() << vboObject->bind();
	context()->functions()->glEnableVertexAttribArray(0);
	context()->functions()->glEnableVertexAttribArray(1);
	context()->functions()->glEnableVertexAttribArray(2);
	context()->functions()->glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	//offset for normals is 4*pts.size() (size of vertices array)
	context()->functions()->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(vsize));
	//offset for colors is 4*pts.size() (size of vertices array) + 3*pts.size() (size of normals array)
	context()->functions()->glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(vsize + nsize));
	vboObject->release();
	eboObject->release();
	vaoObject->release();
	return true;
}

void HatchWidget::updateMesh() {
	//get vertices, normals, and indices
	if(isTriMesh) GetTriMesh();
	else GetPolyMesh();
	//setup vao, vbo and ebo
	SetupVertexArrayObject();

	// Set camera to a standard position
	m_camera.setToIdentity();
	m_camera.translate(_m_camera_position.x(), _m_camera_position.y(), _m_camera_position.z());
	
}


HatchWidget::~HatchWidget(){}

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
	PolyMesh::Point u = curvtrimesh.point(face.v2) - curvtrimesh.point(face.v1);
	PolyMesh::Point v = curvtrimesh.point(face.v3) - curvtrimesh.point(face.v1);
	PolyMesh::Point pN = OpenMesh::cross(u, v);
	return glm::vec3(pN[0], pN[1], pN[2]);
}

void HatchWidget::OnUpdate() {
	_f->glClearColor(1, 1, 1, _isTransparent ? 0 : 1);
	_f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_f->glEnable(GL_DEPTH_TEST);
	//in order for qt to access the vao, you must set the current surface to the surface used when the vao was created.
	_context->makeCurrent(_surface);
	QMatrix3x3 NormalMatrix = (m_camera*m_world*m_scale).normalMatrix();
	if (isReady) {
		_dataShader->Use();
		_dataShader->CheckGlErrors("Update() (Use Shader)");
		_ef->glBindVertexArray(m_VAO);
		_dataShader->CheckGlErrors("Update() (Use Vertex Array Obj)");
		_dataShader->SetUniform("uProjection", m_proj);
		_dataShader->SetUniform("uModelMatrix", m_world*m_scale);
		_dataShader->SetUniform("uViewMatrix", m_camera);
		_dataShader->SetUniform("uNormalMatrix", NormalMatrix);
		_dataShader->CheckGlErrors("Update() (Setting Uniforms)");
		if (isTriMesh) {
			context()->extraFunctions()->glDrawElements(GL_TRIANGLES, indices_n, GL_UNSIGNED_INT, (GLvoid*)0);
			_dataShader->CheckGlErrors("Update() (glDrawElements())");
		}
	}
}

bool HatchWidget::SetupCurvTriMesh() {
	curvtrimesh.GetCurvTriMesh(verts_n, norms_n, colors_n, indices_n,
		vertices, normals, colors, indices,
		minorcurv, majorcurv, meancurv, gausscurv);

	static_assert(sizeof(glm::vec3) == sizeof(GLfloat) * 3, "Platform doesn't support vec3 this directly.");
	static_assert(sizeof(glm::vec4) == sizeof(GLfloat) * 4, "Platform doesn't support vec4 this directly.");
	vsize = sizeof(GLfloat) * 4 * verts_n;
	nsize = sizeof(GLfloat) * 3 * norms_n;
	colsize = sizeof(GLfloat) * 4 * colors_n;
	mincsize = sizeof(GLfloat) * 3 * norms_n;
	majcsize = sizeof(GLfloat) * 3 * norms_n;
	meancsize = sizeof(GLfloat) * 3 * norms_n;
	gausscsize = sizeof(GLfloat) * 1 * norms_n;
	isize = sizeof(GLuint)*indices_n;
	return true;

}

//default shape is a cube.
void HatchWidget::OnInit() {
	context()->functions()->initializeOpenGLFunctions();
	_context = context();
	_f = _context->functions();
	_ef = _context->extraFunctions();
	_surface = _context->surface();
	// Camera never changes in this example.
	m_camera.setToIdentity();
	m_camera.translate(_m_camera_position.x(), _m_camera_position.y(), _m_camera_position.z());
}

bool HatchWidget::SetupVertexArrayObject() {
	_context->makeCurrent(_surface);
	//Create the vertex array & enable attributes for color, normal and vertex
	//The vertex shader should have the following layout:
	//layout(location = 0) in vec4 inPosition;
	//layout(location = 1) in vec3 inNormal;
	//layout(location = 2) in vec4 inColor;
	//layout(location = 3) in vec3 inMinorCurvature;
	//layout(location = 4) in vec3 inMajorCurvature;
	//layout(location = 5) in vec3 inMeanCurvature;
	//layout(location = 6) in float inGaussCurvature;
	//-------------------------------------------------------------------------------------------
	_ef->glGenVertexArrays(1, &m_VAO);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Create VAO)");
	_ef->glBindVertexArray(m_VAO);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (VAO Bind 1)");
	_ef->glEnableVertexAttribArray(0);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib array position)");
	_ef->glEnableVertexAttribArray(1);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib array normal)");
	_ef->glEnableVertexAttribArray(2);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib array color)");
	_ef->glEnableVertexAttribArray(3);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib array minor curvature)");
	_ef->glEnableVertexAttribArray(4);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib array major curvature)");
	_ef->glEnableVertexAttribArray(5);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib array mean curvature)");
	_ef->glEnableVertexAttribArray(6);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib array gauss curvature)");
	//-------------------------------------------------------------------------------------------

	//Create & Setup the Vertex Buffer that stores normals, colors and vertices
	//-----------------------------------------------------
	_ef->glGenBuffers(1, &m_VBO);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Create VBO)");
	_ef->glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (VBO Bind)");
	//-----------------------------------------------------

	//write data to GPU
	//Creating enough space for vertices, normals, colors, minorcurvature, majorcurvature, and mean curvature.
	//-------------------------------------------------------------------------------------------
	_ef->glBufferData(GL_ARRAY_BUFFER, vsize + nsize + colsize + mincsize + majcsize + meancsize + gausscsize, (GLvoid*)0, GL_STATIC_DRAW);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Create Buffer Data)");
	_ef->glBufferSubData(GL_ARRAY_BUFFER, 0, vsize, &vertices[0]);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Write Vertices)");
	_ef->glBufferSubData(GL_ARRAY_BUFFER, vsize, nsize, &normals[0]);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Write Normals)");
	_ef->glBufferSubData(GL_ARRAY_BUFFER, vsize + nsize, colsize, &colors[0]);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Write Colors)");
	_ef->glBufferSubData(GL_ARRAY_BUFFER, vsize + nsize + colsize, nsize, &minorcurv[0]);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Write Minor Curvature)");
	_ef->glBufferSubData(GL_ARRAY_BUFFER, vsize + nsize + colsize + mincsize, nsize, &majorcurv[0]);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Write Major Curvature)");
	_ef->glBufferSubData(GL_ARRAY_BUFFER, vsize + nsize + colsize + mincsize + majcsize, nsize, &meancurv[0]);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Write Mean Curvature)");
	_ef->glBufferSubData(GL_ARRAY_BUFFER, vsize + nsize + colsize + mincsize + majcsize + meancsize, gausscsize, &gausscurv[0]);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Write Gauss Curvature)");
	//-------------------------------------------------------------------------------------------

	//Configure where in memory the attributes are located (last item is offset, second to last is stride). Here is how this is organized in memory:
	//          VERTICES                                 NORMALS                             COLORS						           MINOR CURVATURE			        MAJOR CURVATURE			         MEAN CURVATURE		  GAUSS CURVATURE
	//[v11v12v13v14v21v22v23v24..vi1vi2vi3vi4][n11n12n13n21n22n23...ni1ni2ni3][c11c12c13c14c21c22c23c24...ci1ci2ci3ci4][c11c12c13c21c22c23...ci1ci2ci3] [c11c12c13c21c22c23...ci1ci2ci3] [c11c12c13c21c22c23...ci1ci2ci3]  [c1c2c3c...ci]
	//-------------------------------------------------------------------------------------------
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (VAO Bind 2)");
	_ef->glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib pointer pos)");
	_ef->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)vsize);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib pointer norm)");
	_ef->glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(vsize + nsize));
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib pointer color)");
	_ef->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(vsize + nsize + colsize));
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib pointer minor curvature)");
	_ef->glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(vsize + nsize + colsize + mincsize));
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib pointer major curvature)");
	_ef->glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(vsize + 3*nsize + colsize + mincsize + majcsize));
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib pointer mean curvature)");
	_ef->glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(GLfloat), (GLvoid*)(vsize + 4 * nsize + colsize + mincsize + majcsize + meancsize));
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Enable vbo attrib pointer gauss curvature)");
	//-------------------------------------------------------------------------------------------

	//Create & Setup the Vertex Buffer that stores normals, colors and vertices
	//-------------------------------------------------------------------------------------------
	_ef->glGenBuffers(1, &m_EBO);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Create EBO)");
	_ef->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Bind EBO)");
	_ef->glBufferData(GL_ELEMENT_ARRAY_BUFFER, isize, &indices[0], GL_STATIC_DRAW);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Write EBO)");
	//-------------------------------------------------------------------------------------------

	//unbind the vertex array
	//-------------------------------------------------------------------------------------------
	_ef->glBindVertexArray(0);
	_dataShader->CheckGlErrors("SetupVertexArrayObject() (Unbind VAO)");
	//-------------------------------------------------------------------------------------------

	return true;
}

void HatchWidget::updateMesh() {
	//get vertices, normals, and indices
	SetupCurvTriMesh();
	//setup vao, vbo, ebo, write to gpu
	SetupVertexArrayObject();

	// Set camera to a standard position
	m_camera.setToIdentity();
	m_camera.translate(_m_camera_position.x(), _m_camera_position.y(), _m_camera_position.z());
	
}

void HatchWidget::updateslot() {
	//OnUpdate();
}
HatchWidget::~HatchWidget(){}

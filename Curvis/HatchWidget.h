#pragma once
#include <glew.h>
#include <gl\GL.h>
#include <qopenglfunctions.h>
#include <QOpenGLShader>
#include <qoffscreensurface.h>
#include "GLSLShader.h"
#include "VisObject.h"
#include "Logger.h"
#include "GLSLWidget.h"

struct SimplePolyFace {
	SimplePolyFace(PolyMesh::VertexHandle p1, PolyMesh::VertexHandle p2, PolyMesh::VertexHandle p3, PolyMesh::VertexHandle p4) { v1 = p1; v2 = p2; v3 = p3; v4 = p4; }
	SimplePolyFace(PolyMesh::VertexHandle vs[4]) { v1 = vs[0]; v2 = vs[1]; v3 = vs[2]; v4 = vs[3]; }
	PolyMesh::VertexHandle v1;
	PolyMesh::VertexHandle v2;
	PolyMesh::VertexHandle v3;
	PolyMesh::VertexHandle v4;
};
struct SimpleTriFace {
	SimpleTriFace(TriMesh::VertexHandle p1, TriMesh::VertexHandle p2, TriMesh::VertexHandle p3, TriMesh::FaceHandle face) { v1 = p1; v2 = p2; v3 = p3; f = face; }
	SimpleTriFace(TriMesh::VertexHandle vs[3], TriMesh::FaceHandle face) { v1 = vs[0]; v2 = vs[1]; v3 = vs[2]; f = face; }
	TriMesh::VertexHandle v1;
	TriMesh::VertexHandle v2;
	TriMesh::VertexHandle v3;
	TriMesh::FaceHandle f;
};

class HatchWidget : public GLSLWidget, public VisObject
{
	Q_OBJECT


public:
	HatchWidget(QWidget *parent);
	~HatchWidget();
	QOpenGLContext* getContext();
	GLSLShader *_dataShader;
	void update();
	bool getCompileStatus(GLenum shadertype);
	//is the mesh made up of polygons or triangles...
	bool isTriMesh;
	void updateMesh();
protected:
	void SetupShaderProgram();
	//overide function
	virtual void OnInit() override;
	virtual void OnUpdate() override;
	void GetPolyMesh();
	void GetTriMesh();
	bool SetupVertexArrayObject();
	virtual void cleanup() override;
	glm::vec3 CalcFaceNormal(SimpleTriFace f);
	virtual void setupVertexAttribs() override;
	QOpenGLContext *_context;
	QSurfaceFormat _format;
	QOpenGLVertexArrayObject*	vaoObject;
	QOpenGLBuffer*				vboObject;
	QOpenGLBuffer*				eboObject;

	std::vector<glm::vec4> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec4> colors;
	std::vector<GLuint> indices;

	int _facecount;
	glm::mat4 m_proj_view;
	GLuint m_proj_view_id;
	unsigned long long verts_n;
	unsigned long long norms_n;
	unsigned long long colors_n;
	unsigned long long indices_n;
	//store size of GLfloat*num_elements_in_vector*num_vectors
	GLuint vsize;
	GLuint nsize;
	GLuint csize;
	GLuint isize;


private:
	//OpenGL/Shader variables:
	bool _fragSuccess;
	bool _vertSuccess;
public slots:
	void fragSuccess();
	void vertSuccess();
};

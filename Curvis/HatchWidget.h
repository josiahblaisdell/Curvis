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
#include "CurvComputer.h"
#include <opencv.hpp>

#define FBO_SIZE 3

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

	//This is our shader 
	GLSLShader* _shadingShader;
	GLSLShader* _directionShader;
	GLSLShader* _silhouetteShader;
	GLSLShader* _screenShader;
	void update();
	bool getCompileStatus(GLenum shadertype);
	//is the mesh made up of polygons or triangles...
	bool isTriMesh;
	//True if we can draw the image (ie: vertex array buffer is done being set up)
	bool isReady;
	//Call SetupVertexArrayBuffer() and GetTri/PolyMesh
	void updateMesh();
	//For sharing context between shader and the window.
	QOpenGLContext *_context;
	//For setting the current active surface.
	QSurface* _surface;

	//Create a texture of noise that looks like this: https://www.vectorstock.com/royalty-free-vector/tv-noise-texture-vector-4608313
	bool CreateNoiseTexture();
	inline float NormalDistribution(float x0, float y0, float x1, float y1, float r);

	std::vector<std::vector<int>> phase;
	unsigned int NPN;
	unsigned int animationSpeed;
	unsigned int NMESH;
	float DM;

protected:
	//Initialize the scene with a cube
	virtual void OnInit() override;
	bool SetupFrameBufferObjects();
	//Draw the scene (glDrawElements(...))
	virtual void OnUpdate() override;

	//Initialize the VAO with the vertices, normals, colors and index buffer. 
	bool SetupVertexArrayObject();

	//Get the vertices, normals, colors, indices, curvatures for the mesh
	bool SetupCurvTriMesh();

	//destructor code
	virtual void cleanup() override;

	//Method to try and calculate face normals manually
	glm::vec3 CalcFaceNormal(SimpleTriFace f);

	//Vertex array Object ID
	GLuint m_VAO;
	//Vertex Buffer Object ID
	GLuint m_VBO;
	//Index buffer object ID
	GLuint m_EBO;
	//Array of vertices in mesh.
	std::vector<glm::vec4> vertices;
	//Array of normals in mesh
	std::vector<glm::vec3> normals;
	//Array of colors in mesh.
	std::vector<glm::vec4> colors;
	//Array of indices into vertices and colors to create the mesh.
	std::vector<GLuint> indices;
	//Array of minorcurvature for vertices in mesh
	std::vector<glm::vec3> minorcurv;
	//Array of major curvature for vertices in mesh
	std::vector<glm::vec3> majorcurv;
	//Array of mean curvature for vertices in mesh
	std::vector<glm::vec3> meancurv;
	//Array of mean curvature for vertices in mesh
	std::vector<float> gausscurv;



	//Framebuffer 0
	GLuint m_FBO[FBO_SIZE];
	//Texture 0
	GLuint m_FBOTex[FBO_SIZE];
	GLuint m_noiseTex;
	//Renderbuffer 0
	GLuint m_RBO[FBO_SIZE];
	//Vertex array Object ID
	GLuint m_quad_VAO;
	//Vertex Buffer Object ID
	GLuint m_quad_VBO;
	//Index buffer object ID
	GLuint m_quad_EBO;
	//vertices for quad
	std::vector<glm::vec4> quad_verts;
	std::vector<glm::vec2> quad_texcoords;
	std::vector<GLuint> quad_indices;

	glm::mat4 m_proj_view;
	GLuint m_proj_view_id;
	//number of vertices
	unsigned long long verts_n;
	//number of normals
	unsigned long long norms_n;
	//number of Colors
	unsigned long long colors_n;
	//number of indices
	unsigned long long indices_n;
	//store size in bytes of GLfloat*num_elements_in_vector*num_vectors
	GLuint vsize;
	//store size in bytes of GLfloat*num_elements_in_vector*num_vectors
	GLuint nsize;
	//store size of in bytes GLfloat*num_elements_in_vector*num_vectors
	GLuint colsize;
	//store size of in bytes GLfloat*num_elements_in_vector*num_vectors
	GLuint mincsize;
	//store size of in bytes GLfloat*num_elements_in_vector*num_vectors
	GLuint majcsize;
	//store size of in bytes GLfloat*num_elements_in_vector*num_vectors
	GLuint meancsize;
	//store size of in bytes GLfloat*num_elements_in_vector*num_vectors
	GLuint gausscsize;
	//store size in bytes of GLfloat*num_elements_in_vector*num_vectors
	GLuint isize;


private:
	//OpenGL/Shader variables:
	bool _fragSuccess;
	bool _vertSuccess;
	QOpenGLFunctions* _f;
	QOpenGLExtraFunctions *_ef;
public slots:
	void fragSuccess();
	void vertSuccess();
	void updateslot();
};

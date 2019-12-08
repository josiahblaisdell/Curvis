#pragma once
#include "VisObject.h"
#include <glm\glm.hpp>
#include "OpenGLFiles\glew\include\GL\glew.h"
#include <gl\GL.h>
#include <string>
#include <stdio.h>
#include <vector>
#include <tuple>
#include <qopenglfunctions.h>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShader>
#include <qoffscreensurface.h>
#include <qobject.h>
#include "Logger.h"
class GLSLShader : public QObject, public VisObject
{
	Q_OBJECT
		typedef std::tuple<bool, GLenum, GLuint, std::string> shadertuple;
public:
	//checks to make sure the shader compiled successfully
	bool isValid();
	//Set int attributes for variables in the vertex shader.
	void SetAttribute(char* name, int val);
	//Set float attributes for variables in the vertex shader.
	void SetAttribute(char* name, float val);
	//Set vec3(float,float,float) attributes for variables in the vertex shader.
	void SetAttribute(char* name, float x, float y, float z);
	//Set float array attributes for variables in the vertex shader.
	void SetAttribute(char* name, float* vals);
	//Set int attributes for variables in the vertex shader.
	void SetAttribute(char* name, glm::vec3&);
	void SetGStap(bool);
	void SetInputTopology(GLenum topology);
	void SetOutputTopology(GLenum topology);
	//Set int uniform variables for any shader.
	void SetUniform(char* name, int val);
	//Set float uniform variables for any shader.
	void SetUniform(char* name, float val);
	//Set (x,y,z) uniform variables for any shader.
	void SetUniform(char* name, float x, float y, float z);
	//Set float array uniform variables for any shader.
	void SetUniform(char* name, float* vals);
	//Set uniform vec3 variables
	void SetUniform(char* name, glm::vec3& v3);
	//Set uniform mat4 variables
	void SetUniform(char* name, glm::mat4& m4);
	//Set uniform mat3 variables
	void SetUniform(char* name, glm::mat3& m4);
	//Set uniform mat4 variables
	void SetUniform(char* name, QMatrix4x4& m4);
	void SetUniform(char * name, QMatrix3x3 & m4);
	void SetUniform(char* name, glm::vec4& v);
	void SetUniform(char * name, glm::vec2 & v);

	void SetUniform1i(char* name, int index);
	//Set verbose logging
	void SetVerbose(bool enable);
	void Use();
	void UseFixedFunction();
	GLuint getProgramID();
	GLSLShader(std::vector<std::string> filePaths, std::vector<std::string>*  glslUniforms, std::vector<std::string>* glslAttributes, QOpenGLContext *c, QSurface* s);
	bool setupUniforms();
	bool setupAttributes();
	~GLSLShader();
	void CheckGlErrors(std::string caller);
	bool LinkProgram();
signals:
	void fragShaderCompileSuccess();
	void geomShaderCompileSuccess();
	void vertShaderCompileSuccess();
	void tcsShaderCompileSuccess();
	void tesShaderCompileSuccess();

protected:
	//shaders stores all the shaders associated with this instance of GLShader.
	//tuple gives (compile status, shader type, shader id, shader path)
	//example: (true,GL_VERTEX_SHADER,1,test.vert) true means compile was successful.
	std::vector< shadertuple  > shaders;

	//context that is used by this shader.
	QOpenGLContext* _context;
	//if the constructor is not passed a context create my own (to test compilation presumably)
	QSurface *_surface;
	//if the constructor is not passed a context create my own QOpenGLFunctions instance (to test compilation presumably)
	QOpenGLFunctions *_qOpenGLFunctions;
	//this has some functions like glparameteri that are needed for tessellation shaders.
	QOpenGLExtraFunctions *_qOpenGLExtraFunctions;
	//this is the id of the linked shader program
	GLuint _program;
	//creates a new shader using the file paths you pass to the constructor.
	bool Create();
	//true when all compilation is done and successful.
	bool _ready;
	//returns the file extension of the filename passed in argument
	std::string GetFileExtension(std::string FileName);
	//returns a string of characters corresponding to the shader (or file) stored at fp
	std::string readfile(FILE &fp);
	//calls read file on the shader you pass uses shadertype for logging. Returns "" if reading fails.
	std::string readShader(GLenum ShaderType, std::string file);
	//array storing each of the variables and their corresponding ids
	std::vector<std::pair<GLuint, std::string>> _glslUniforms;
	std::vector<std::pair<GLuint, std::string>> _glslAttributes;
private:
	void emitShaderCompileSuccess(GLenum t);
};


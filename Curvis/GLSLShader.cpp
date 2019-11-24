#include "GLSLShader.h"
#define _USE_MATH_DEFINES
//found on the interwebs...
static const char * const glGetTypeString(GLenum type)
{
	switch (type)
	{
	case GL_BYTE: return "GLbyte";
	case GL_UNSIGNED_BYTE: return "GLubyte";
	case GL_SHORT: return "GLshort";
	case GL_UNSIGNED_SHORT: return "GLushort";
	case GL_INT: return "GLint";
	case GL_UNSIGNED_INT: return "GLuint";
	case GL_FLOAT: return "GLfloat";
	case GL_2_BYTES: return "GL_2_BYTES";
	case GL_3_BYTES: return "GL_3_BYTES";
	case GL_4_BYTES: return "GL_4_BYTES";
	case GL_DOUBLE: return "GLdouble";
	case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
	case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
	case GL_GEOMETRY_SHADER: return "GL_GEOMETRY_SHADER";
	case GL_TESS_CONTROL_SHADER: return "GL_TESS_CONTROL_SHADER";
	case GL_TESS_EVALUATION_SHADER: return "GL_TESS_EVALUATION_SHADER";
	case GL_COMPUTE_SHADER: return "GL_COMPUTE_SHADER";
	default: return "not a GLenum type";
	}
}

void GLSLShader::Use()
{
	_qOpenGLFunctions->glUseProgram(_program);
}

GLuint GLSLShader::getProgramID()
{
	return _program;
}

GLSLShader::GLSLShader(std::vector<std::string> filePaths, std::vector<std::string>*  glslUniforms, std::vector<std::string>* glslAttributes, QOpenGLContext *c)
{
	//current opengl _context has not been set up yet.
	if (c == NULL) {
		_context = new QOpenGLContext();
		_context->create();
		_surface = new QOffscreenSurface();
		_surface->create();
		_context->makeCurrent(_surface);
		_qOpenGLFunctions = new QOpenGLFunctions(c);
		_qOpenGLFunctions->initializeOpenGLFunctions();
		_qOpenGLExtraFunctions = new QOpenGLExtraFunctions(c);
	}
	//current opengl _context has been set up and was passed into constructor.
	else {
		_surface = NULL;
		_context = c;
		_qOpenGLFunctions = c->functions();
		_qOpenGLExtraFunctions = c->extraFunctions();

	}
	int i = 0;
	if (glslUniforms != NULL) for each(auto uniform in *glslUniforms) {
		_glslUniforms.push_back(std::pair<GLuint, std::string>(i, uniform));
		i++;
	}
	i = 0;
	if (glslAttributes != NULL) for each(auto attribute in *glslAttributes) {
		_glslAttributes.push_back(std::pair<GLuint, std::string>(i, attribute));
		i++;
	}
	_ready = false;

	getLog()->Write(DEBUG, "GLSLShader()", "Log file opened successfully.");

	for each(std::string shader in filePaths) {
		std::string *ext = new std::string(GetFileExtension(shader).c_str());
		getLog()->Write(DEBUG, "GLSLShader()", "Adding " + *ext + " shader stored at: " + shader);
		if (strcmp(ext->c_str(), ".vert") == 0 || strcmp(ext->c_str(), ".vs") == 0) {
			shadertuple vs(false, GL_VERTEX_SHADER, 0, shader);
			shaders.push_back(vs);
		}
		else if (strcmp(ext->c_str(), ".tcs") == 0) {
			shadertuple vs(false, GL_TESS_CONTROL_SHADER, 0, shader);
			shaders.push_back(vs);
		}
		else if (strcmp(ext->c_str(), ".tes") == 0) {
			shadertuple vs(false, GL_TESS_EVALUATION_SHADER, 0, shader);
			shaders.push_back(vs);
		}
		else if (strcmp(ext->c_str(), ".geom") == 0 || strcmp(ext->c_str(), ".gs") == 0) {
			shadertuple vs(false, GL_GEOMETRY_SHADER, 0, shader);
			shaders.push_back(vs);
		}
		else if (strcmp(ext->c_str(), ".frag") == 0 || strcmp(ext->c_str(), ".fs") == 0) {
			shadertuple vs(false, GL_FRAGMENT_SHADER, 0, shader);
			shaders.push_back(vs);
		}
		else if (strcmp(ext->c_str(), ".comp") == 0) {
			shadertuple vs(false, GL_COMPUTE_SHADER, 0, shader);
			shaders.push_back(vs);
		}
		else {
			getLog()->Write(DEBUG, "GLSLShader()", "Invalid shader file name.");
		}
		delete ext;
	}
	getLog()->Write(DEBUG, "GLSLShader()", "Finished adding shaders to shader array.");
	Create();
	CheckGlErrors("Constructor()");
	if (_ready) setupUniforms();
}

bool GLSLShader::setupUniforms() {
	getLog()->Write(DEBUG, "setupUniforms()", "Getting uniform locations.");
	if (!_glslUniforms.empty()) {
		for (size_t i = 0; i < _glslUniforms.size(); i++)
		{
			GLint uniformid = _qOpenGLFunctions->glGetUniformLocation(_program, _glslUniforms[i].second.c_str());
			if (uniformid < 0) {
				getLog()->Write(DEBUG, "setupUniforms()", "Unable to find uniform variable: " + _glslUniforms[i].second);
			}
			_glslUniforms[i].first = uniformid;
		}
	}
	CheckGlErrors("SetupUniforms()");
	getLog()->Write(DEBUG, "setupUniforms()", "Finished getting uniform locations.");
	return true;

}

bool GLSLShader::setupAttributes() {
	getLog()->Write(DEBUG, "setupAttributes()", "Binding attribute locations.");

	if (!_glslAttributes.empty()) {
		GLuint i = 0;
		for each (auto variable in _glslAttributes) {
			_qOpenGLFunctions->glBindAttribLocation(_program, i, std::get<1>(variable).c_str());
			std::get<0>(variable) = i;
			i++;
		}
	}
	getLog()->Write(DEBUG, "setupAttributes()", "Finished binding attribute locations.");
	return true;
}

void GLSLShader::CheckGlErrors(std::string caller) {
	unsigned int glerr = _qOpenGLFunctions->glGetError();
	if (glerr == GL_NO_ERROR) return;
	getLog()->Write(DEBUG, "CheckGlErrors()", "GL Error discovered from caller: " + caller);

	switch (glerr) {
	case GL_INVALID_ENUM:
		getLog()->Write(DEBUG, "CheckGlErrors()", "Invalid enum.");
		break;
	case GL_INVALID_VALUE:
		getLog()->Write(DEBUG, "CheckGlErrors()", "Invalid value.");
		break;
	case GL_INVALID_OPERATION:
		getLog()->Write(DEBUG, "CheckGlErrors()", "Invalid operation.");
		break;
	case GL_STACK_OVERFLOW:
		getLog()->Write(DEBUG, "CheckGlErrors()", "Stack overflow.");
		break;
	case GL_STACK_UNDERFLOW:
		getLog()->Write(DEBUG, "CheckGlErrors()", "Stack underflow.");
		break;
	case GL_OUT_OF_MEMORY:
		getLog()->Write(DEBUG, "CheckGlErrors()", "Out of memory.");
		break;
	default:
		char c[10];
		itoa(glerr, c, 10);
		getLog()->Write(DEBUG, "CheckGlErrors()", "Unknown OpenGL error : " + std::string(c));
	}
}

bool GLSLShader::LinkProgram() {
	getLog()->Write(DEBUG, "LinkProgram()", "Linking the shaders that were compiled. ");
	int status;
	//	glewInit();
	//glProgramParameteriEXT(_program, GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
	//_qOpenGLExtraFunctions->glProgramParameteri(_program, GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
	//CheckGlErrors("GL_GEOMETRY_INPUT_TYPE_EXT");
	//glProgramParameteriEXT(_program, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
	//_qOpenGLExtraFunctions->glProgramParameteri(_program, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
	//CheckGlErrors("GL_GEOMETRY_OUTPUT_TYPE_EXT");
	//glProgramParameteriEXT(_program, GL_GEOMETRY_VERTICES_OUT_EXT, 200);
	//_qOpenGLExtraFunctions->glProgramParameteri(_program, GL_GEOMETRY_VERTICES_OUT_EXT, 200);
	//CheckGlErrors("GL_GEOMETRY_VERTICES_OUT_EXT");
	setupAttributes();
	_qOpenGLFunctions->glLinkProgram(_program);
	CheckGlErrors("LinkProgram()");
	_qOpenGLFunctions->glGetProgramiv(_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		char* infoLog;
		int infoLogLen;
		char infoLogLenChar[10];
		int linkStatus;

		_qOpenGLFunctions->glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &infoLogLen);
		itoa(infoLogLen, infoLogLenChar, 10);
		getLog()->Write(DEBUG, "Create()", "Failed to link _program -- info log length: " + std::string(infoLogLenChar));
		if (infoLogLen > 0) {
			infoLog = new char[infoLogLen + 1];
			_qOpenGLFunctions->glGetProgramInfoLog(_program, infoLogLen, NULL, infoLog);
			infoLog[infoLogLen] = '\0';
			getLog()->Write(DEBUG, "Create()", std::string(infoLog));
			delete[] infoLog;
		}
		_qOpenGLFunctions->glDeleteProgram(_program);
		return false;
	}
	CheckGlErrors("LinkProgram() (post-linking)");
	getLog()->Write(DEBUG, "LinkProgram()", "Linking the shaders was successful. ");
	return true;
}

bool GLSLShader::Create()
{
	getLog()->Write(DEBUG, "Create()", "Creating Shaders...");
	_program = _qOpenGLFunctions->glCreateProgram();
	for each(shadertuple shader in shaders) {
		int status;
		int logLength;
		bool compiled = std::get<0>(shader);
		GLenum shadertype = std::get<1>(shader);
		GLuint shaderid = std::get<2>(shader);
		std::string fp = std::get<3>(shader);
		getLog()->Write(DEBUG, "Create()", "Creating " + std::string(glGetTypeString(shadertype)) + " shader.");
		shaderid = _qOpenGLFunctions->glCreateShader(shadertype);
		//set shader id to the value OpenGL returned.
		std::get<1>(shader) = shaderid;
		//read the code in the file path associated with this shader.
		std::string *code = new std::string(readShader(shadertype, fp));
		//store in const char* for glShaderSource()...
		const char *c_str_code = code->c_str();
		_qOpenGLFunctions->glShaderSource(shaderid, 1, &c_str_code, NULL);
		_qOpenGLFunctions->glCompileShader(shaderid);
		std::stringstream ss;
		ss << std::string(glGetTypeString(shadertype)) + " Shader";
		//check if this shader compiled successfully.
		CheckGlErrors(ss.str());
		_qOpenGLFunctions->glGetShaderiv(shaderid, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			getLog()->Write(DEBUG, "Create()", "Failed to create " + std::string(glGetTypeString(shadertype)) + " shader.");
			//set compile status to "false" indicating compile failed.
			std::get<0>(shader) = false;
			ss.str("");
			ss << std::string(glGetTypeString(shadertype)) + " shader compilation failed for shader: " + fp;
			getLog()->Write(DEBUG, "Create()", ss.str());
			_qOpenGLFunctions->glGetShaderiv(shaderid, GL_INFO_LOG_LENGTH, &logLength);
			char* message = new char[logLength];
			_qOpenGLFunctions->glGetShaderInfoLog(shaderid, logLength, NULL, message);
			getLog()->Write(DEBUG, "Create()", std::string(message));
			delete[] message;
			return false;
		}
		//set compiled to success!
		std::get<0>(shader) = true;
		emitShaderCompileSuccess(shadertype);
		_qOpenGLFunctions->glAttachShader(_program, shaderid);
		CheckGlErrors("Create() (post-attaching)");
		delete code;
	}
	_ready = LinkProgram();
	return _ready;
}

std::string GLSLShader::GetFileExtension(std::string FileName)
{
	if (FileName.find_last_of(".") != std::string::npos)
		return FileName.substr(FileName.find_last_of("."));
	return "";
}

//Reads the file passed to it.
std::string GLSLShader::readfile(FILE &fp) {
	if (&fp == NULL) {
		getLog()->Write(DEBUG, "readShader()", "Invalid filepath in readShader()");
		return "";
	}
	//SEEK_END may be on past the end of the file -- device dependent
	fseek(&fp, 0, SEEK_END);
	int numbytes = ftell(&fp);
	char* buffer = new char[numbytes + 1];
	rewind(&fp);
	fread(buffer, 1, numbytes, &fp);
	fclose(&fp);
	char cur = ' ';
	int i = numbytes;
	//rewind to the end of the shader code where there should be a } character.
	while (buffer[i] != '}') {
		buffer[i] = '\0';
		i--;
	}
	return std::string(buffer);
}

std::string GLSLShader::readShader(GLenum ShaderType, std::string file) {
	std::string shadertype(glGetTypeString(ShaderType));
	getLog()->Write(DEBUG, "readShader()", "Loading " + shadertype);
	FILE *fp = fopen(file.c_str(), "r");
	std::string code = readfile(*fp);
	if (code != "") {
		getLog()->Write(DEBUG, "readShader()", "Successfully loaded: " + shadertype);
		return code;
	}
	getLog()->Write(DEBUG, "readShader()", "There was a problem loading: " + shadertype);
	return "";
}

void GLSLShader::emitShaderCompileSuccess(GLenum t)
{
	switch (t)
	{
	case GL_VERTEX_SHADER: emit vertShaderCompileSuccess(); break;
	case GL_FRAGMENT_SHADER: emit fragShaderCompileSuccess(); break;
	case GL_GEOMETRY_SHADER: emit geomShaderCompileSuccess(); break;
	case GL_TESS_CONTROL_SHADER: emit tesShaderCompileSuccess(); break;
	case GL_TESS_EVALUATION_SHADER: emit tcsShaderCompileSuccess(); break;
	default: break;
	}
}


GLSLShader::~GLSLShader()
{
	if (_surface != NULL) {
		_surface->destroy();
		delete _surface;
	}
	_qOpenGLFunctions->glDeleteProgram(_program);
	_glslUniforms.clear();
}

bool GLSLShader::isValid()
{
	int status;
	_qOpenGLFunctions->glValidateProgram(_program);
	_qOpenGLFunctions->glGetProgramiv(_program, GL_VALIDATE_STATUS, &status);
	return (status == GL_FALSE);
}

void GLSLShader::SetAttribute(char * name, float val)
{
	GLint location = _qOpenGLFunctions->glGetUniformLocation(_program, name);
	if (location < 0)
		getLog()->Write(DEBUG, "SetAtrribute(float)", "Cannot find Uniform variable " + std::string(name));
	else {
		const GLint gVal = val;
		char c[10];
		itoa(val, c, 10);
		getLog()->Write(DEBUG, "SetAttribute(float)", "Setting " + std::string(name) + " to " + std::string(c));
		_qOpenGLFunctions->glUniform1i(location, val);
	}
}
void GLSLShader::SetUniform(char * name, float val)
{
	GLint location = _qOpenGLFunctions->glGetUniformLocation(_program, name);
	if (location < 0)
		getLog()->Write(DEBUG, "SetUniform(float)", "Cannot find Uniform variable " + std::string(name));
	else {
		const GLint gVal = val;
		char c[10];
		itoa(val, c, 10);
		getLog()->Write(DEBUG, "SetUniform(float)", "Setting " + std::string(name) + " to " + std::string(c));
		_qOpenGLFunctions->glUniform1f(location, val);
	}
	CheckGlErrors("SetUniform(float)");
}

void GLSLShader::SetUniform(char * name, int val)
{
	GLint location = _qOpenGLFunctions->glGetUniformLocation(_program, name);

	if (location < 0)
		getLog()->Write(DEBUG, "SetUniform(int)", "Cannot find Uniform variable " + std::string(name));
	else {
		const GLint gVal = val;
		char c[10];
		itoa(val, c, 10);
		//getLog()->Write(DEBUG, "SetUniform(int)", "Setting " + std::string(name) + " to " + std::string(c));
		_qOpenGLFunctions->glUniform1i(location, val);
	}
	CheckGlErrors("SetUniform(QMatrix4x4)");
}
void GLSLShader::SetUniform(char * name, QMatrix4x4 & m4)
{
	CheckGlErrors("SetUniform(QMatrix4x4)");
	GLint location = _qOpenGLFunctions->glGetUniformLocation(_program, name);

	if (location < 0)
		getLog()->Write(DEBUG, "SetUniform(QMatrix4x4)", "Cannot find Uniform variable " + std::string(name));
	else {
		getLog()->Write(DEBUG, "SetUniform(QMatrix4x4)", "Setting " + std::string(name) + " to Matrix4x4 value.");
		_qOpenGLFunctions->glUniformMatrix4fv(location, 1, false, m4.data());

	}
	CheckGlErrors("SetUniform(QMatrix4x4)");
}
void GLSLShader::SetUniform(char * name, QMatrix3x3 & m3)
{
	GLint location = _qOpenGLFunctions->glGetUniformLocation(_program, name);

	if (location < 0)
		getLog()->Write(DEBUG, "SetUniform(QMatrix3x3)", "Cannot find Uniform variable " + std::string(name));
	else {
		getLog()->Write(DEBUG, "SetUniform(QMatrix3x3)", "Setting " + std::string(name) + " to Matrix3x3 value.");
		_qOpenGLFunctions->glUniformMatrix3fv(location, 1, false, m3.data());
	}
	CheckGlErrors("SetUniform(QMatrix3x3)");
}
void GLSLShader::SetUniform(char * name, glm::vec4 & v)
{
	GLint location = _qOpenGLFunctions->glGetUniformLocation(_program, name);

	if (location < 0)
		getLog()->Write(DEBUG, "SetUniform(vec4)", "Cannot find Uniform variable " + std::string(name));
	else {
		getLog()->Write(DEBUG, "SetUniform(vec4)", "Setting " + std::string(name) + " to vec4 value.");
		GLfloat glv[4] = { v.x, v.y, v.z,v.w };
		_qOpenGLFunctions->glUniform3fv(location, 1, glv);
	}
	CheckGlErrors("SetUniform(Vec4)");
}

void GLSLShader::SetUniform(char * name, glm::vec3 & v)
{
	GLint location = _qOpenGLFunctions->glGetUniformLocation(_program, name);

	if (location < 0)
		getLog()->Write(DEBUG, "SetUniform(vec3)", "Cannot find Uniform variable " + std::string(name));
	else {
		getLog()->Write(DEBUG, "SetUniform(QMatrix3x3)", "Setting " + std::string(name) + " to vec3 value.");
		GLfloat glv[3] = { v.x, v.y, v.z };
		_qOpenGLFunctions->glUniform3fv(location, 1, glv);
	}
	CheckGlErrors("SetUniform(vec3)");
}
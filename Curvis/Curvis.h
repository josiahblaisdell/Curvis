#pragma once

#include <QtWidgets/QMainWindow>
#include <QWidget>
#include <qfile.h>
#include <qfiledialog.h>
#include <stdio.h>
#include "VisObject.h"
#include "GLSLShader.h"
#include <tuple>
#include "Logger.h"
#include "glew.h"
#include "ui_Curvis.h"
#include <qtimer.h>
typedef std::tuple<bool, GLenum, GLuint, std::string> ShaderTuple;
class Curvis : public QMainWindow, VisObject
{
	Q_OBJECT

public:
	Curvis(QWidget *parent = Q_NULLPTR);

private:
	Ui::CurvisClass ui;
	std::vector<QString> vertexShaderPaths;
	std::vector<QString> fragShaderPaths;
	std::vector<GLuint> vertShaderIDs;
	std::vector<GLuint> fragShaderIDs;
	std::vector<ShaderTuple> shaders;
	std::vector<std::string> _glslUniforms;
	std::vector<std::string> _glslAttributes;
	QString plyPath;
	QStringList shaderCompileLog;

private slots:
	void on_btn_loadply_clicked();
};

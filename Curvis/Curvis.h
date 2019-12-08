#pragma once

#include <QtWidgets/QMainWindow>
#include <QWidget>
#include <qfile.h>
#include <qfiledialog.h>
#include <stdio.h>
#include "Quaternion.h"
#include "VisObject.h"
#include "GLSLShader.h"
#include <tuple>
#include "Logger.h"
#include "glew.h"
#include "ui_Curvis.h"
#include <qtimer.h>
#include <qopenglfunctions.h>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShader>
#include <qoffscreensurface.h>
#include <qopenglvertexarrayobject.h>
#include <qopenglbuffer.h>

typedef std::tuple<bool, GLenum, GLuint, std::string> ShaderTuple;
class Curvis : public QMainWindow, VisObject
{
	Q_OBJECT

public:
	Curvis(QWidget *parent = Q_NULLPTR);

private:
	Ui::CurvisClass ui;
	std::vector<std::string> shaderpaths;
	std::vector<std::string> _glslUniforms;
	std::vector<std::string> _glslAttributes;
	QString plyPath;
	QStringList shaderCompileLog;

private slots:
	void on_btn_loadply_clicked();
};

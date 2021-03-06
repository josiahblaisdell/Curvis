#pragma once
#include "Curvis.h"

Curvis::Curvis(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	initVisObject();
	qDebug() << connect(ui.plypath, SIGNAL(textChanged()), ui.hatchWidget, SLOT(updateslot()));
}

void Curvis::on_btn_loadply_clicked() {
	shaderpaths.clear();
	_glslAttributes.clear();
	_glslUniforms.clear();
	shaderpaths.push_back(".\\Shaders\\framebuffer0.vert");
	shaderpaths.push_back(".\\Shaders\\framebuffer0.frag");
	_glslAttributes.push_back("inPos");
	_glslAttributes.push_back("inTexCoords");
	_glslUniforms.push_back("img_size");
	_glslUniforms.push_back("uPattern");
	_glslUniforms.push_back("uShading");
	_glslUniforms.push_back("uField");
	ui.hatchWidget->_screenShader = new GLSLShader(shaderpaths, &_glslUniforms, &_glslAttributes, ui.hatchWidget->_context, ui.hatchWidget->_surface);

	shaderpaths.clear();
	_glslAttributes.clear();
	_glslUniforms.clear();
	shaderpaths.push_back(".\\Shaders\\cube_vertex_shader.vert");
	shaderpaths.push_back(".\\Shaders\\shading.frag");
	_glslAttributes.push_back("inPosition");
	_glslAttributes.push_back("inNormal");
	_glslAttributes.push_back("inColor");
	_glslAttributes.push_back("inMinorCurvature");
	_glslAttributes.push_back("inMajorCurvature");
	_glslAttributes.push_back("inMeanCurvature");
	_glslUniforms.push_back("uModelMatrix");
	_glslUniforms.push_back("uViewMatrix");
	_glslUniforms.push_back("uProjection");
	_glslUniforms.push_back("uNormalMatrix");
	ui.hatchWidget->_shadingShader = new GLSLShader(shaderpaths, &_glslUniforms, &_glslAttributes, ui.hatchWidget->_context,ui.hatchWidget->_surface);

	shaderpaths.clear();
	_glslAttributes.clear();
	_glslUniforms.clear();
	shaderpaths.push_back(".\\Shaders\\cube_vertex_shader.vert");
	shaderpaths.push_back(".\\Shaders\\direction_encoding.frag");
	_glslAttributes.push_back("inPosition");
	_glslAttributes.push_back("inNormal");
	_glslAttributes.push_back("inColor");
	_glslAttributes.push_back("inMinorCurvature");
	_glslAttributes.push_back("inMajorCurvature");
	_glslAttributes.push_back("inMeanCurvature");
	_glslUniforms.push_back("uModelMatrix");
	_glslUniforms.push_back("uViewMatrix");
	_glslUniforms.push_back("uProjection");
	_glslUniforms.push_back("uNormalMatrix");
	ui.hatchWidget->_directionShader = new GLSLShader(shaderpaths, &_glslUniforms, &_glslAttributes, ui.hatchWidget->_context, ui.hatchWidget->_surface);

	shaderpaths.clear();
	_glslAttributes.clear();
	_glslUniforms.clear();
	shaderpaths.push_back(".\\Shaders\\cube_vertex_shader.vert");
	shaderpaths.push_back(".\\Shaders\\silhouette.frag");
	_glslAttributes.push_back("inPosition");
	_glslAttributes.push_back("inNormal");
	_glslAttributes.push_back("inColor");
	_glslAttributes.push_back("inMinorCurvature");
	_glslAttributes.push_back("inMajorCurvature");
	_glslAttributes.push_back("inMeanCurvature");
	_glslUniforms.push_back("uModelMatrix");
	_glslUniforms.push_back("uViewMatrix");
	_glslUniforms.push_back("uProjection");
	_glslUniforms.push_back("uNormalMatrix");
	ui.hatchWidget->_silhouetteShader = new GLSLShader(shaderpaths, &_glslUniforms, &_glslAttributes, ui.hatchWidget->_context, ui.hatchWidget->_surface);

	QFile filepath = QFileDialog::getOpenFileName(this, tr("Open .vert File"), ".\\sampledata", tr("ply Files (*.ply)"));
	plyPath = QString(filepath.fileName());
	if (plyPath.size() == 0) return;
	getLog()->Write(DEBUG, "on_btn_loadply_clicked()", "Loading PLY at file: " + filepath.fileName().toStdString());
	ui.plypath->setText(filepath.fileName());
	if (!filepath.exists()) return;
	OpenMesh::IO::Options ropt;
	CurvTriMesh mesh;
	OpenMesh::IO::read_mesh(mesh, plyPath.toStdString(),ropt);
	mesh.Boot();
	curvtrimesh = mesh;
	ui.hatchWidget->isTriMesh = true;
	ui.hatchWidget->updateMesh();
	ui.hatchWidget->isReady = true;
	update();
}

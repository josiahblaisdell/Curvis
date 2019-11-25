#include "Curvis.h"

Curvis::Curvis(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	initVisObject();
	int x = 0;
	std::vector<std::string> shaderpaths;
}

void Curvis::on_btn_loadply_clicked() {
	ui.hatchWidget->isTriMesh = true;
	ui.btn_loadply->setEnabled(false);
	QFile filepath = QFileDialog::getOpenFileName(this, tr("Open .vert File"), ".\\sampledata", tr("ply Files (*.ply)"));
	plyPath = QString(filepath.fileName());
	if (plyPath.size() == 0) return;
	getLog()->Write(DEBUG, "on_btn_loadply_clicked()", "Loading PLY at file: " + filepath.fileName().toStdString());
	ui.plypath->setText(filepath.fileName());
	if (!filepath.exists()) return;
	//trimesh.request_face_normals();
	//trimesh.request_halfedge_normals();
	//trimesh.request_vertex_normals();
	OpenMesh::IO::Options ropt;
	OpenMesh::IO::read_mesh(trimesh, plyPath.toStdString(),ropt);
	ui.hatchWidget->updateMesh();
	////ADD SHADERS HERE
	//std::vector<std::string> shaderpaths;
	//shaderpaths.push_back(".\\Shaders\\cube_vertex_shader.vert");
	//shaderpaths.push_back(".\\Shaders\\cube_frag_shader.frag");
	//_glslAttributes.push_back("inPosition");
	//_glslAttributes.push_back("inNormal");
	//_glslAttributes.push_back("inColor");
	//_glslUniforms.push_back("uModelMatrix");
	//_glslUniforms.push_back("uViewMatrix");
	//_glslUniforms.push_back("uProjection");
	////_glslUniforms.push_back("uEyePos");
	//ui.hatchWidget->_dataShader = new GLSLShader(shaderpaths, &_glslUniforms, &_glslAttributes, ui.hatchWidget->getContext());
	update();
}

void Curvis::on_runButton_clicked() {
	//ADD SHADERS HERE
	std::vector<std::string> shaderpaths;
	shaderpaths.push_back(".\\Shaders\\cube_vertex_shader.vert");
	shaderpaths.push_back(".\\Shaders\\cube_frag_shader.frag");
	_glslAttributes.push_back("inPosition");
	_glslAttributes.push_back("inNormal");
	_glslAttributes.push_back("inColor");
	_glslUniforms.push_back("uModelMatrix");
	_glslUniforms.push_back("uViewMatrix");
	_glslUniforms.push_back("uProjection");
	ui.hatchWidget->_dataShader = new GLSLShader(shaderpaths, &_glslUniforms, &_glslAttributes, ui.hatchWidget->getContext());
	update();
}
#include "Curvis.h"

Curvis::Curvis(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	initVisObject();
	int x = 0;
	//qDebug() << connect(ui.btn_loadply, SIGNAL(clicked()), this, SLOT(on_btn_loadply_clicked()));
}

void Curvis::on_btn_loadply_clicked() {
	ui.btn_loadply->setEnabled(false);
	
	QFile filepath = QFileDialog::getOpenFileName(this, tr("Open .vert File"), ".\\sampledata", tr("ply Files (*.ply)"));
	plyPath = QString(filepath.fileName());
	if (plyPath.size() == 0) return;
	getLog()->Write(DEBUG, "on_btn_loadply_clicked()", "Loading PLY at file: " + filepath.fileName().toStdString());
	OpenMesh::IO::read_mesh(poly, plyPath.toStdString());
	ui.plypath->setText(filepath.fileName());
	QTimer::singleShot(1000, [&] { ui.btn_loadply->setEnabled(true); });
	update();
}
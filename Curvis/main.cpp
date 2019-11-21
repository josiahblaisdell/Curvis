#include "Curvis.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Curvis w;
	w.show();
	return a.exec();
}

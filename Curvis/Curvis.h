#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Curvis.h"

class Curvis : public QMainWindow
{
	Q_OBJECT

public:
	Curvis(QWidget *parent = Q_NULLPTR);

private:
	Ui::CurvisClass ui;
};

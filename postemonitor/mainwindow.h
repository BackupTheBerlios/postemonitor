#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QtGui>
#include <QtCore>

class myMainWindow : public QDialog, private Ui::myMainWindow
{
	Q_OBJECT

public:
	myMainWindow(QWidget *parent = 0);
};

#endif

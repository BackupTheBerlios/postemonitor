#ifndef RACCOMANDATA_H
#define RACCOMANDATA_H

#include "ui_raccomandatamonitor.h"

#include <QtGui>
#include <QtCore>
#include <QString>
#include <QLineEdit>
#include <QDate>
#include <QPixmap>
#include <QHttp>
#include <QFile>

//class raccomandataMonitor : public QFrame, private Ui::Form
class raccomandataMonitor : public QFrame, private Ui::raccomandataMonitor
{
	Q_OBJECT


public:
	raccomandataMonitor(QWidget *parent = 0);
private:
	QHttp *sock;
	QFile *buff;

private slots:
	void on_bottone_clicked();
	void on_sock_requestStarted(int);
	void on_sock_requestFinished(int,bool);
	void on_numerow_textChanged(const QString);
};

#endif

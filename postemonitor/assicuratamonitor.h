#ifndef ASSICURATA_H
#define ASSICURATA_H

#include "ui_assicuratamonitor.h"

#include <QtGui>
#include <QtCore>
#include <QString>
#include <QLineEdit>
#include <QDate>
#include <QPixmap>
#include <QHttp>
#include <QFile>

//class assicurataMonitor : public QFrame, private Ui::Form
class assicurataMonitor : public QFrame, private Ui::assicurataMonitor
{
	Q_OBJECT


public:
	assicurataMonitor(QWidget *parent = 0);
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

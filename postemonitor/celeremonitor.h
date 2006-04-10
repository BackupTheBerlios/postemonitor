#ifndef CELEREMONITOR_H
#define CELEREMONITOR_H

#include "ui_CelereMonitorWG.h"

#include <QtNetwork>
#include <QtGui>
#include <QtCore>

class QHttp;
class QLineEdit;
class QLabel;
class QPushButton;
class QMainWindow;
class QProgressBar;
class QFile;
class QWidget;
class QRegExp;
class QValidator;

class CelereMonitor : public QFrame, private Ui::UICelere
{
	Q_OBJECT
	public:
	CelereMonitor(QWidget *parent=0);

	private slots:
	
	void on_lineEditCode_textChanged(const QString & text);
	void on_pushButtonSend_clicked();
	void on_pushButtonCancel_clicked();
	void done(bool err);
	void on_myHttpClient_requestStarted(int id);
	void on_myHttpClient_requestFinished(int id,bool err);
	
	signals:
	void dataQueryProgress(int prog,int tot);///progresso della fase di invio del pacchetto
	void dataResultProgress(int prog, int tot);/// progresso della fase di ricezione dei dati
	void connectionStateChanged(int cs);///stato attuale della connessione [connesso,inConnessione,in chiusura,chiuso,abortito]
	void connectionClosed();
	
	private:
	void update_listView();
	void writeBufferToDisk(const QByteArray &buff);
	
	QHttp *myHttpClient;
	QFile *mio;
	QBuffer *dataResponse;
	QString host;
	int idClosing;
	int idConnecting;
	int idRequest;
	};
#endif

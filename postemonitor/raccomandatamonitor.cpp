#include "raccomandatamonitor.h"
#include "raccass_engine.h"

raccomandataMonitor::raccomandataMonitor(QWidget* parent):QFrame(parent)
{
	setupUi(this);
	sock=new QHttp();

	QRegExp rx("\\d{12}");
	QValidator *validator = new QRegExpValidator(rx, this);
	numerow->setValidator(validator);

	connect(sock, SIGNAL(requestFinished(int,bool)),
			this, SLOT(on_sock_requestFinished(int,bool)));
	connect(sock, SIGNAL(requestStarted(int)),
			this, SLOT(on_sock_requestStarted(int)));
}

void raccomandataMonitor::on_bottone_clicked()
{
	buff = new QFile("temp.pm");
	if(!buff->open(QIODevice::ReadWrite))
	return;
	sock->setHost("www.poste.it");
	QString url="/online/dovequando/dovequandoraccomandata.php?mpcode=";
	url+=numerow->text();
	url+="&mpdate=";
	url+= dataw->date().toString("dd/MM/yyyy");
	sock->get(url,buff);
	bottone->setEnabled(false);
	//sock->close();
}

void raccomandataMonitor::on_sock_requestStarted(int numeroreq)
{
	risultato->setText(tr("Attendere. Richiesta in corso."));
}

void raccomandataMonitor::on_sock_requestFinished(int numeroreq, bool err)
{
	string output;
	string numeros;
	string datas;

	numeros=numerow->text().toStdString();

	datas=dataw->date().toString("dd/MM/yyyy").toStdString();

	if(numeroreq%2==0){
		//buff->flush();
		buff->close();
		ifstream input("temp.pm",ios::in);
		parse(input,output,numeros,datas);
		risultato->setText(QString::fromStdString(output));
		input.close();
	bottone->setEnabled(true);
	}
	buff->remove();

}

void raccomandataMonitor::on_numerow_textChanged(const QString newtesto)
{
	if(numerow->hasAcceptableInput())bottone->setEnabled(true);
	else bottone->setEnabled(false);
}

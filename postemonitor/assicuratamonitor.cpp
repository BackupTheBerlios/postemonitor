#include "assicuratamonitor.h"
#include "raccass_engine.h"

assicurataMonitor::assicurataMonitor(QWidget* parent):QFrame(parent)
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

void assicurataMonitor::on_bottone_clicked()
{
	buff = new QFile("temp.pm");
	if(!buff->open(QIODevice::ReadWrite))
	return;
	sock->setHost("www.poste.it");
	QString url="/online/dovequando/dovequandoassicurata.php?mpcode=";
	url+=numerow->text();
	url+="&mpdate=";
	url+= dataw->date().toString("dd/MM/yyyy");
	sock->get(url,buff);
	bottone->setEnabled(false);
	//sock->close();
}


void assicurataMonitor::on_sock_requestStarted(int numeroreq)
{
	risultato->setText(tr("Attendere. Richiesta in corso."));
}

void assicurataMonitor::on_sock_requestFinished(int numeroreq, bool err)
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

void assicurataMonitor::on_numerow_textChanged(const QString newtesto)
{
	if(numerow->hasAcceptableInput())bottone->setEnabled(true);
	else bottone->setEnabled(false);
}

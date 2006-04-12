#include "celeremonitor.h"
#include "parser.h"
#include <iostream>
#include <string>
CelereMonitor::CelereMonitor(QWidget *parent) : QFrame(parent)
{
	setupUi(this);
	
	//comodità di debugging
	host = "javapc.poste.it";
	//end comodità
	
	//Impostazione delle restrizioni di input sulla lineEditCodice
	QRegExp rx( "[0-9a-zA-Z]+" );
	QValidator* validator = new QRegExpValidator( rx, this );
	lineEditCode->setValidator(validator);
	//Fine restrizione
	
	myHttpClient = new QHttp;
	
	
	connect(myHttpClient,SIGNAL(done(bool)),this, SLOT(done(bool)));
	connect(myHttpClient,SIGNAL(dataSendProgress(int,int)),this,SIGNAL(dataQueryProgress(int,int)));
	connect(myHttpClient,SIGNAL(dataReadProgress(int,int)),this,SIGNAL(dataResultProgress(int,int)));
	connect(myHttpClient,SIGNAL(requestFinished(int,bool)),this,SLOT(on_myHttpClient_requestFinished(int,bool)));
	connect(myHttpClient,SIGNAL(requestStarted(int)),this,SLOT(on_myHttpClient_requestStarted(int)));
	connect(myHttpClient,SIGNAL(stateChanged(int)),this,SLOT(on_myhttpclient_stateChanged(int)));
}

void CelereMonitor::on_lineEditCode_textChanged(const QString & text)
{
	//piccola routine che converte il testo che viene scritto automaticamente in maiuscolo
	lineEditCode->setText(text.toUpper());
	//verfico le condizioni per l'invio della richiesta
	if (lineEditCode->hasAcceptableInput() && (lineEditCode->text()).length() == 7 && myHttpClient->state() == QHttp::Unconnected )
		pushButtonSend->setEnabled(true);//Abilito il bottone di invio se l'input è valido
	else pushButtonSend->setEnabled(false);//disabilito il bottone di invio se l'input non è valido
}

//funzione di lettura una volta completata la richiesta
void CelereMonitor::done(bool err)
{
	if(err){
		//Un errore è stato riscontrato
		//visualizzo il messaggio di errore
		(void) QMessageBox::warning(this,tr("Errore"),tr("Impossibile effetuare la richiesta"));
		
		//annullo ogni richiesta dell'oggetto myHttpClient
		myHttpClient->abort();
		
		//reimposto i bottoni per una nuova richiesta
		pushButtonSend->setEnabled(true);
		pushButtonCancel->setEnabled(false);
		
		//esco dallo SLOT per evitare che venga eseguito altro
		return;
	}
	pushButtonCancel->setEnabled(false);
	pushButtonSend->setEnabled(true);
	this->writeBufferToDisk(dataResponse->data());
	update_listView();//visualizzo i risultati nel QTreeWidget come nella vecchia listView :D
	//QMessageBox::information(this,tr("Letto:"),tr("%1").arg(dataResponse->data().data()));
	delete dataResponse;
	dataResponse =0;
	if(myHttpClient->hasPendingRequests() == true)
		QMessageBox::information(this,"Attenzione","Richieste ancora in attesa!!");
	myHttpClient->abort();
	
	emit dataQueryProgress(0,1000);
}

void CelereMonitor::on_pushButtonSend_clicked()
{
	listViewResponse->clear();//riazzero gli eventuali risultati presenti
	dataResponse = new QBuffer;
	if(!dataResponse->open(QIODevice::ReadWrite))//apertura del buffer su cui viene scritto tutto
	{
		QMessageBox::warning(this,tr("Errore"),tr("Impossibile accedere al buffer"));
		return;
	}
	
	//inizio il pacchetto per la richiesta
	QHttpRequestHeader header("POST", "/trackingPacchi");
    header.setValue("Host", host);
	header.addValue("keep-alive","30");
    header.setContentType("application/x-www-form-urlencoded");
    
	QString searchString = "Action=ActionTracking&Method=Tracking&SessionId=-1&ldv="+lineEditCode->text()+"\n";
	//fine del pacchetto
	
	//inizio la connessione
    idConnecting = myHttpClient->setHost(host);
	
	
    idRequest = myHttpClient->request(header,searchString.toUtf8(), dataResponse);
	
	
	//una volta lanciate le richieste lancio la chiusura del QHttp
	//idClosing = myHttpClient->close();
	//imposto i pulsanti di conseguenza
	pushButtonCancel->setEnabled(true);
	pushButtonSend->setEnabled(false);
}
void CelereMonitor::on_pushButtonCancel_clicked() //annullamento di una richiesta
{
	myHttpClient->abort();
	pushButtonCancel->setEnabled(false);
	pushButtonSend->setEnabled(true);
	emit dataQueryProgress(0,1000);
}

//all'inizio di una richiesta viene lanciata questa funzione
void CelereMonitor::on_myHttpClient_requestStarted(int id)
{
	
	if(id == idClosing)
	{
		//QMessageBox::information(this,tr("Esecuzione"),tr("Richiesta Chiusura"));
	}
	if(id == idRequest)
	{
		//QMessageBox::information(this,tr("Esecuzione"),tr("Richiesta in corso"));
		
	}
	if( id == idConnecting)
	{
		//QMessageBox::information(this,tr("Esecuzione"),tr("Richiesta Connessione"));
	}	
}

//funzione al termine di ogni richiesta
void CelereMonitor::on_myHttpClient_requestFinished(int id,bool err)
{
	if(!err)
	{
		if(id == idClosing)
		{
			//QMessageBox::information(this,tr("Finita"),tr("Richiesta Chiusura"));
			pushButtonSend->setEnabled(true);
			pushButtonCancel->setEnabled(false);
		}
		if(id == idRequest)
		{
			//QMessageBox::information(this,tr("Finita"),tr("Richiesta Finita"));
		}
		if( id == idConnecting)
		{
			//QMessageBox::information(this,tr("Finita"),tr("Richiesta Connessione"));
		}	
	}
	/*else
	{
		QMessageBox::warning(this,tr("Errore in richiesta!"),tr("Errore:%1").arg(myHttpClient->errorString()));
	}
	*/
}
void CelereMonitor::writeBufferToDisk(const QByteArray &buff)
{
	QFile file("temp.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
	file.write(buff);
	file.close();
}


void CelereMonitor::update_listView()
{
	std::ifstream input("temp.txt",std::ios::in);
	std::string output;
	mioParser::parse(input,output,lineEditCode->text().toStdString(),3);
	QString finalOut;
	finalOut += QString::fromStdString(output);
	
	input.close();

	if(finalOut.contains("Attenzione!",	Qt::CaseInsensitive)>0)
	{
		QMessageBox::information(this,tr("Attenzione"),tr("Informazione non ancora disponibile o codice inserito errato.<br>Controllare l'esattezza dei dati ed eventualmente riprovare pi&ugrave; tardi."));
		//.arg(finalOut));
		//this->groupBoxResponse->setEnabled(false);
	}
	else if(finalOut.contains("momento",	Qt::CaseInsensitive)>0)
		QMessageBox::information(this,tr("Attenzione"),tr("Il servizio non &egrave; al momento disponibile.<BR>Riprovare nuovamente pi&ugrave; tardi."));
	else
	{
		//this->groupBoxResponse->setEnabled(true);
		QStringList rows = finalOut.split("\n");
		rows.pop_back();
		rows.pop_front();
		
		for (QStringList::Iterator it = rows.begin(); it != rows.end(); ++it )
		{
			QStringList cols = (*it).split("\t");
			new QTreeWidgetItem(listViewResponse,cols);
    		}
		
	}
	QFile fi("temp.txt");
	if(fi.exists()) fi.remove();
}	

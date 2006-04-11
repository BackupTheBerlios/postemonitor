#include "celereonemonitor.h"
#include "parser.h"

CelereOneMonitor::CelereOneMonitor(QWidget *parent) : QFrame(parent)
{
	setupUi(this);
	
	//comodità di debugging
	host = "javapc.poste.it";
	//end comodità
	
	//Impostazione delle restrizioni di input sulla lineEditCodice
	QRegExp rx( "[0-9a-zA-Z]+" );
	QValidator* validator = new QRegExpValidator( rx, this );
	lineEditCode->setValidator(validator);
	lineEditCode->setMaxLength(13);
	//Fine restrizione
	
	//impostazione variabili interne
	dataResponse = 0;
	//fine var interne
	
	
	myHttpClient = new QHttp;
	
	
	connect(myHttpClient,SIGNAL(done(bool)),this, SLOT(done(bool)));
	connect(myHttpClient,SIGNAL(dataSendProgress(int,int)),this,SIGNAL(dataQueryProgress(int,int)));
	connect(myHttpClient,SIGNAL(dataReadProgress(int,int)),this,SIGNAL(dataResultProgress(int,int)));
	connect(myHttpClient,SIGNAL(requestFinished(int,bool)),this,SLOT(on_myHttpClient_requestFinished(int,bool)));
	connect(myHttpClient,SIGNAL(requestStarted(int)),this,SLOT(on_myHttpClient_requestStarted(int)));
	connect(myHttpClient,SIGNAL(stateChanged(int)),this,SLOT(on_myhttpclient_stateChanged(int)));
}

void CelereOneMonitor::on_lineEditCode_textChanged(const QString & text)
{
	//piccola routine che converte il testo che viene scritto automaticamente in maiuscolo
	lineEditCode->setText(text.toUpper());
	//verfico le condizioni per l'invio della richiesta
	if (lineEditCode->hasAcceptableInput() && (lineEditCode->text()).length() == 13 && myHttpClient->state() == QHttp::Unconnected)
		pushButtonSend->setEnabled(true);//Abilito il bottone di invio se l'input è valido
	else pushButtonSend->setEnabled(false);//disabilito il bottone di invio se l'input non è valido
}

//funzione di lettura una volta completata la richiesta
void CelereOneMonitor::done(bool err)
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
	dataResponseDev->close();
	
	update_listView();//visualizzo i risultati nel QTreeWidget come nella vecchia listView :D
	
	delete dataResponseDev;
	delete dataResponse;
	dataResponseDev = 0;
	dataResponse = 0;
	if(myHttpClient->hasPendingRequests() == true)
		QMessageBox::information(this,"Attenzione","Richieste ancora in attesa!!");
	myHttpClient->abort();
	
	emit dataQueryProgress(0,1000);
}

void CelereOneMonitor::on_pushButtonSend_clicked()
{
	listViewResponse->clear();//riazzero gli eventuali risultati presenti
	dataResponse = new QByteArray;
	dataResponseDev = new QBuffer(dataResponse);
	if(!dataResponseDev->open(QIODevice::ReadWrite))//apertura del buffer su cui viene scritto tutto
	{
		QMessageBox::warning(this,tr("Errore"),tr("Impossibile accedere al buffer"));
		return;
	}
	
	//inizio il pacchetto per la richiesta
	QHttpRequestHeader header("POST", "/trackingPacchi");
    header.setValue("Host", host);
	header.addValue("keep-alive","30");
	header.addValue("connection","keep-alive");
	header.addValue("Referer","http://www.poste.it/online/dovequando/formpaccocelere1.html");
    header.setContentType("application/x-www-form-urlencoded");
    
	QString searchString = "Action=ActionCore&Method=Tracking&SessionId=-1&PRODOTTO=PACCO&ID_LDV="+lineEditCode->text()+"\n";
	//fine del pacchetto
	
	//inizio la connessione
    idConnecting = myHttpClient->setHost(host);
	
	
    idRequest = myHttpClient->request(header,searchString.toUtf8(), dataResponseDev);
	
	
	//una volta lanciate le richieste lancio la chiusura del QHttp
	//idClosing = myHttpClient->close();
	//imposto i pulsanti di conseguenza
	pushButtonCancel->setEnabled(true);
	pushButtonSend->setEnabled(false);
}
void CelereOneMonitor::on_pushButtonCancel_clicked() //annullamento di una richiesta
{
	myHttpClient->abort();
	pushButtonCancel->setEnabled(false);
	pushButtonSend->setEnabled(true);
	emit dataQueryProgress(0,1000);
}

//all'inizio di una richiesta viene lanciata questa funzione
void CelereOneMonitor::on_myHttpClient_requestStarted(int id)
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
void CelereOneMonitor::on_myHttpClient_requestFinished(int id,bool err)
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


void CelereOneMonitor::update_listView()
{
	//eseguo il parsing del buffer
	purge_the_buffer();
	
	if(dataResponse->length() != 0)
	{
		QString finalOut(dataResponse->data());
	
		QStringList rows = finalOut.split("\n");
		rows.pop_front(); // rimuovo la prima riga :D
		for (QStringList::Iterator it = rows.begin(); it != rows.end(); ++it )
		{
				QStringList cols = (*it).split(";");
				new QTreeWidgetItem(listViewResponse,cols);
		}
	}
	delete dataResponse;
	dataResponse = 0;
}	
void CelereOneMonitor::purge_the_buffer()
{	
	parse(dataResponse);
}
void CelereOneMonitor::parse(QByteArray *buffer)
{	
	if(buffer->indexOf("Attenzione!",0) != -1)
	{
		buffer->clear();
		QMessageBox::information(this,tr("Attenzione"),tr("Informazione non ancora disponibile!"));
		return;
	}
	
	buffer->replace("\t","");
	//rimuovo i dati inutili all'inizio del buffer
	int start = buffer->indexOf("Situazione della spedizione:&nbsp;",0);

	buffer->remove(0,start);
	
	
	//rimuovo i dati inutili
	int end   = buffer->indexOf("</table>",start);
	end += 16;
	
	buffer->remove(end,buffer->size());
	buffer->replace("<tr bgcolor=\"#e8f404\">","");
	buffer->replace("<tr bgcolor=\"#292873\">","");
	//buffer->replace("<td rowspan=\"4\" align=\"right\" bgcolor=\"#e8f404\">","");
	buffer->replace("<td rowspan=\"4\" align=\"right\" bgcolor=\"#e8f404\"></td>","");
	buffer->replace("<td rowspan=\"4\" bgcolor=\"#e8f404\" align=\"right\"></td>","");
	buffer->replace("<font size=\"1\">","");
	buffer->replace("<font size=1>","");
	buffer->replace("<font color=\"#ffffff\" size=\"2\">","");
	buffer->replace("<font size=2 color=\"#ffffff\">","");
	buffer->replace("</font>","");
	buffer->replace("</tr>","\n");
	buffer->replace("<td>","");
	buffer->replace("</td>",";");
	buffer->replace("<b>","");
	buffer->replace("</b>","");
	buffer->replace("<i>","");
	buffer->replace("</i>","");
	
	start = buffer->indexOf("Data",0);
	buffer->remove(0,start);

	end = buffer->indexOf("</table>");
	buffer->remove(end,buffer->length());
	
	//mi appoggio ad una qstring e una qstring list per rimuovere le occorrenze di linee vuote
	QString quasifinito(buffer->data());
	
	buffer->clear();
	
	QStringList list = quasifinito.split("\n");
	QStringList::const_iterator i;
	for(i = list.begin();i!=list.end();++i)
	{
		if((*i).length()>2) buffer->append(*i+"\n");
	}
}

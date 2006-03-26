#include <cassert>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include "raccass_engine.h"
using namespace std;

//Funzione per la sostituzione
string& replaceAll(string& context, const string& from, const string& to) {
size_t lookHere = 0;
size_t foundHere;
while ((foundHere = context.find(from, lookHere))!= string::npos) {
	context.replace(foundHere, from.size(), to);
	lookHere = foundHere + to.size();
}
return context;
}

//Funzione per la rimozione dei tag HTML
string& stripHTMLTags(string& s) {
static bool inTag = false;
bool done = false;

replaceAll(s, "\t", "");

while (!done) {
	//replaceAll(s, "</td> ", " ");
	//replaceAll(s, "</td></tr>", "\n");
	if (inTag) {
		size_t rightPos = s.find('>');
		if (rightPos != string::npos) {
			inTag = false;
			s.erase(0, rightPos + 1);
		}
		else {
		done = true;
		s.erase();
		}
	}
	else {
	size_t leftPos = s.find('<');
		if (leftPos != string::npos) {
		size_t rightPos = s.find('>');
			if (rightPos == string::npos) {
			inTag = done = true;
			s.erase(leftPos);
			}
			else
			s.erase(leftPos, rightPos - leftPos + 1);
			}
		else
		done = true;
		}
	}
replaceAll(s, "&lt;", "<");
replaceAll(s, "&gt;", ">");
replaceAll(s, "&amp;", "&");
replaceAll(s, "&nbsp;", " ");
return s;
}

//Funzione di parsing principale
string& parse(ifstream& input, string& output, string codice, string data) {
ofstream tempout("temp",ios::out);
ifstream tempin("temp",ios::in);

string s;

string infounavailable="<B>Attenzione!</B><BR>";
infounavailable+="Dati non disponibili.<BR>";

string infotempunavailable="<B>Attenzione!</B><BR>";
infotempunavailable+="Dati temporaneamente non disponibili.<BR>";
infotempunavailable+="Controllare l'esattezza dei dati ed eventualmente ripetere l'interrogazione tra 4 ore.";

string svctempunavailable="<B>Solo dalle 7 alle 23.</B>";

string svcunavailable="<B>Attenzione!</B><BR>";
svcunavailable+="Il servizio non &egrave; al momento disponibile.<BR>";
svcunavailable+="Riprovare nuovamente pi&ugrave; tardi.";

string codenotreg="<B>Attenzione!</B><BR>";
codenotreg+="Il codice non &egrave; stato ancora registrato.";

string situa="Situazione della spedizione:";

//Rimuovo la parte iniziale
while(s!="<!--- start TABELLA GENERICA --->"&&s!="<!--- start ERRORE --->")
{
	getline(input,s);
}

//Rimuovo i tag HTML
while(getline(input, s))
{
	if (s=="<!--- end TABELLA GENERICA --->") break;
	if (s=="<!--- end ERRORE --->") break;
	if (!stripHTMLTags(s).empty()) tempout << s << endl;
}

//Rimuovo delle stringhe note
while(getline(tempin, s))
{
	//replaceAll(s, "\t", " ");
	//if(s==" ")s="\n";
	replaceAll(s, situa, "");
	replaceAll(s, codice, "");
	replaceAll(s, data, "");
	replaceAll(s, "Data impostata nella ricerca:", "");
	//replaceAll(s, "\t\t", "");
	//replaceAll(s,"\n","<br>");
	replaceAll(s,"Accettato","<br><br>Accettato");
	output += s;
}

//Controllo se le informazioni sono disponibili

//Rilevo l'errore "Failure of server APACHE bridge:"
if(output[0]=='W') output=svcunavailable;

//Servizio disponibile solo dalle 7 alle 23
//if(output[0]=='W') output=svctempunavailable;

//Informazioni temporaneamente non disponibili o codice errato
else if(output[0]=='D') output=infotempunavailable;

//Informazioni non disponibili o codice errato
else if(output[0]=='A') output=infounavailable;

//Codice non ancora registrato
else if(output[0]=='I'&&output[1]=='l') output=codenotreg;

tempout.close();
tempin.close();
remove("temp");
return output;
}


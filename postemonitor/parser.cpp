#include "parser.h"

using namespace std;
namespace mioParser{
#define CEL1_STR "Notizie in tempo reale della tua spedizione con Paccocelere1"
#define CEL3_STR "Notizie in tempo reale della tua spedizione con Paccocelere3"

//Funzione per la sostituzione
string& replaceAll(string& context, const string& from, const string& to) 
{
size_t lookHere = 0;
size_t foundHere;
while ((foundHere = context.find(from, lookHere))!= string::npos) {
	context.replace(foundHere, from.size(), to);
	lookHere = foundHere + to.size();
}
return context;
}

//Funzione per la rimozione dei tag HTML
string& stripHTMLTags(string& s)
{
static bool inTag = false;
bool done = false;

replaceAll(s, "\t", "");
replaceAll(s, "<title>Poste Italiane</title>", "");
while (!done) {
	replaceAll(s, "</td><td>", "\t");
	replaceAll(s, "</td></tr>", "\n");
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
void parse(ifstream& input, string& output, string codice,int version)
{
ofstream tempout("temp",ios::out);
ifstream tempin("temp",ios::in);

string s;
string situa = "Situazione della spedizione: ";
situa +=codice;

char* temp = new char;
while(*temp!='<'){
	*temp = (char)input.get();
}
input.unget();

while(getline(input, s))
	if (!stripHTMLTags(s).empty()) tempout << s << endl;

while(getline(tempin, s))
{
	if(s==" ")s="\n";
	if(version == 1) replaceAll(s,CEL1_STR, "");
	else replaceAll(s,CEL3_STR, "");
	replaceAll(s," -->","");
	replaceAll(s, situa, "");
	replaceAll(s, "    ", "");
	replaceAll(s, "  Esegui nuovo monitoraggio  ", "\n");
	output += s;
}

//Controllo se le informazioni sono disponibili
if(output[0]=='W')//Rilevo l'errore "Failure of server APACHE bridge:"
{
	output="<B>Attenzione!</B><BR>";
	output+="Il servizio non &egrave; al momento disponibile.<BR>";
	output+="Riprovare nuovamente pi&ugrave; tardi.";
}
/*else// if(output[0]!='D')//Informazioni non disponibili o codice errato
{
	output="<B>Attenzione!</B><BR>";
	output+="Informazione non ancora disponibile o codice inserito errato.<BR>";
	output+="Controllare l'esattezza dei dati ed eventualmente contattare l'ufficio postale";
	output+="di accettazione o il numero telefonico 803.160 (gratuito da telefono fisso).";
}
*/
tempout.close();
tempin.close();
remove("temp");
}
};

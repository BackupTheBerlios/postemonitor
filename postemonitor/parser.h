#include <cassert>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
namespace mioParser{
	string& replaceAll(string& context, const string& from, const string& to); 
	string& stripHTMLTags(string& s);
	void parse(ifstream& input, string& output, string codice,int version);
};
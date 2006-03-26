#include <cassert>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

string& replaceAll(string& context, const string& from, const string& to);

string& stripHTMLTags(string& s);

string& parse(ifstream& input, string& output, string codice, string data);

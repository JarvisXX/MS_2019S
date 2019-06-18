#ifndef API
#define API

#include<vector>
#include<string>

using namespace std;

void tokenize(const string& line, vector<std::string>& argv, const string& charset=string(" \t\r\n"));

#endif

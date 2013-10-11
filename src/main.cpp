#include "vraptor.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;
using namespace VRaptor;
/*
string readFile(const string& fname){
	ifstream f(fname.c_str());
	stringstream buf;
	while(f.good()){
		string line;
		getline(f,line);
		buf<<line<<endl;
	}
	f.close();
	return buf.str();
}

int main(int argc,char **argv){
	string fname(argv[1]);
	string s = readFile(fname);
	VModule *m = VModule::readFromString(s);
	cout<<"Read file"<<endl;
}
*/
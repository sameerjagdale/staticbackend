//============================================================================
// Name        : staticbackend.cpp
// Author      : Sameer Jagdale
//============================================================================

#include "vraptor.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <codegen.hpp>
#include<prettyPrinter.hpp>
using namespace std;
using namespace VRaptor;

string readFile(const string& fname) {
	ifstream f(fname.c_str());
	stringstream buf;
	while (f.good()) {
		string line;
		getline(f, line);
		buf << line << endl;
	}
	f.close();
	return buf.str();
}

void writeFile(string fileName, vector<string> output) {
	ofstream f;

	f.open((fileName).c_str(), ios::out);
	for (int i = 0; i < output.size(); i++) {
		f << output[i];

	}

	f.close();
}

int main(int argc, char **argv) {
	if (argc < 3) {
		cout
				<< "usage ./staticbackend [IR file]  [output file ] enable_openmp(0/1) enable_sse(0/1)";
		exit(0);
	}

	string fname(argv[1]);
	string s = readFile(fname);
	bool enableOpenMP = false, enableSse = false;
	string fileName(argv[2]);
	if (argc >= 4) {

		string openmp(argv[3]);

		if (openmp.compare("1") == 0) {

			enableOpenMP = true;
		}
	}
	if (argc >= 5) {
		string sse(argv[4]);
		if (sse.compare("1") == 0) {

			enableSse = true;
		}
	}
	VModule *m = VModule::readFromString(s);
	VCompiler vc;
	vc.setOpenMpFlag(enableOpenMP);
	vc.setSseFlag(enableSse);
	Context cntxt = vc.moduleCodeGen(m);
	PrettyPrinter pp;
	vector<string> output = pp.prettyPrint(cntxt.getAllStmt());
	for (int i = 0; i < output.size(); i++) {
		if (output[i].compare("}\n\n") == 0) {

		}
		cout << output[i];
	}

	writeFile(fileName, output);
}

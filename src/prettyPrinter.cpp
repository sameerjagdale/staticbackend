/*
 * prettyPrinter.cpp

 *
 *  Created on: 2013-03-19
 *      Author: sjagda
 */

#include<prettyPrinter.hpp>
#include<vector>
#include<string>
#include<sstream>
using std::vector;
using std::string;
using std::ostringstream;

vector<string> PrettyPrinter::prettyPrint(vector<string> srcStmt) {
	vector<string> output;
	int tab = 0;
	for (int i = 0; i < srcStmt.size(); i++) {
		string str;
		ostringstream convert;
		if (srcStmt[i].find("}") != string::npos) {
			tab--;
			convert << tab;

		}

		str = srcStmt[i];
		for (int j = 0; j < tab; j++) {
			str = "\t" + str;
		}
		if (srcStmt[i].find("{") != string::npos) {

			tab++;
			convert << tab;
		}
		output.push_back(str);
	}
	return output;
}

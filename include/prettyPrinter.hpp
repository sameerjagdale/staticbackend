/*
 * prettyPrinter.hpp
 *
 *  Created on: 2013-03-19
 *      Author: sjagda
 */

#ifndef PRETTYPRINTER_HPP_
#define PRETTYPRINTER_HPP_

#include<vector>
#include<string>
using std::vector;
using std::string;

class PrettyPrinter {
public:
	vector<string> prettyPrint(vector<string> srcStmt);
};

#endif /* PRETTYPRINTER_HPP_ */

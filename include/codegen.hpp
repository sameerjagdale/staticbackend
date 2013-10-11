/*
 * codegen.hpp
 *
 *  Created on: 2013-03-15
 *      Author: sjagda
 */

#ifndef CODEGEN_HPP_
#define CODEGEN_HPP_

#include "vraptor.hpp"
#include <vector>
#include<iostream>
#include<string>
#include<sstream>
#include<stdlib.h>
#include<memory>
#include <vectorAnalysis.hpp>

#define DIM_OFFSET 32
#define SIZE_OFFSET 20
#define DATA_OFFSET 16
using std::vector;
using std::string;

namespace VRaptor {
using std::tr1::shared_ptr;
typedef std::tr1::shared_ptr<BreakStmt> BreakStmtPtr;
typedef shared_ptr<ContinueStmt> ContinueStmtPtr;
typedef shared_ptr<RefOpStmt> RefOpStmtPtr;
typedef shared_ptr<AndExpr> AndExprPtr;
typedef shared_ptr<OrExpr> OrExprPtr;
typedef shared_ptr<NotExpr> NotExprPtr;
typedef shared_ptr<FuncallExpr> FuncallExprPtr;
typedef shared_ptr<LibCallExpr> LibCallExprPtr;
typedef shared_ptr<ArrayType> ArrayTypePtr;

class Context {

	vector<string> list;
	bool arrayFlag;

public:
	Context();
	void addStmt(const string stmt);
	vector<string> getAllStmt();
	void setArrayFlag();
	void resetArrayFlag();
	bool isArray();
};

class VCompiler {
private:
	Context stmtTypeCodeGen(StmtPtr stmt, SymTable *symTable);
	Context exprTypeCodeGen(ExpressionPtr expr, SymTable *symTable);
	Context vTypeCodeGen(VTypePtr returnType, SymTable *symTable);
	Context scalarTypeCodeGen(ScalarTypePtr vtype);
	Context arrayTypeCodeGen(ArrayTypePtr type, SymTable *symTable);
	bool enableOpenMP;
	bool enableSse;
public:
	bool getSseFlag();
	void setSseFlag(bool);
	bool getOpenmpFlag();
	void setOpenMpFlag(bool);
	Context funcCodeGen(VFunction *func);
	Context stmtCodeGen(StmtPtr stmt, SymTable *symTable);
	Context forStmtCodeGen(ForStmtPtr stmt, SymTable *symTable);
	Context assignStmtCodeGen(AssignStmtPtr stmt, SymTable *symTable);
	Context nameExprCodeGen(NameExprPtr expr, SymTable *symTable);
	Context plusExprCodeGen(PlusExprPtr expr, SymTable *symTable);
	Context domainExprCodeGen(DomainExprPtr expr, SymTable *symTable);
	Context constExprCodeGen(ConstExprPtr expr, SymTable *symTable);
	Context stmtListCodeGen(StmtListPtr stmt, SymTable *symTable);
	Context indexExprCodeGen(IndexExprPtr expr, SymTable *symTable);
	Context minusExprCodeGen(MinusExprPtr expr, SymTable *symTable);
	Context mMultExprCodeGen(MmultExprPtr expr, SymTable *symTable);
	Context returnStmtCodeGen(ReturnStmtPtr stmt, SymTable *symTable);

	Context moduleCodeGen(VModule *vm);
	Context breakStmtCodeGen(BreakStmtPtr stmt);
	Context continueStmtCodeGen(ContinueStmtPtr stmt);
	Context ifStmtCodeGen(IfStmtPtr stmt, SymTable * symTable);
	Context whileStmtCodeGen(WhileStmtPtr stmt, SymTable *symTable);
	Context refOpStmtCodeGen(RefOpStmtPtr stmt, SymTable *symTable);
	Context pForStmtCodeGen(PforStmtPtr stmt, SymTable *symTable);

	Context binaryExprCodeGen(BinaryExprPtr expr, SymTable *symTable);
	Context multExprCodeGen(MultExprPtr expr, SymTable *symTable);
	Context divExprCodeGen(DivExprPtr expr, SymTable *symTable);
	Context geqExprCodeGen(GeqExprPtr expr, SymTable *symTable);
	Context gtExprCodeGen(GtExprPtr expr, SymTable *symTable);
	Context ltExprCodeGen(LtExprPtr expr, SymTable *symTable);
	Context leqExprCodeGen(LeqExprPtr expr, SymTable *symTable);
	Context andExprCodeGen(AndExprPtr expr, SymTable *symTable);
	Context orExprCodeGen(OrExprPtr expr, SymTable *symTable);
	Context notExprCodeGen(NotExprPtr expr, SymTable *symTable); //to be written
	Context funCallExprCodeGen(FuncallExprPtr expr, SymTable *symTable);
	Context libCallExprCodeGen(LibCallExprPtr expr, SymTable *symtable);
	Context negateExprCodeGen(NegateExprPtr expr, SymTable *symTable);
	//Context vectoriseStmt(AssignStmtPtr stmt, string size, SymTable *symTable);
};

}
#endif /* CODEGEN_HPP_ */
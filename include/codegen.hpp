/*
 * codegen.hpp
 *
 *  Created on: 2013-03-15
 *      Author: sjagda
 */

#ifndef CODEGEN_HPP_
#define CODEGEN_HPP_
#endif /* CODEGEN_HPP_ */
#include "vraptor.hpp"
#include <vector>
#include<iostream>
#include<string>
#include<sstream>
#include<stdlib.h>
#include <vectorAnalysis.hpp>
//using namespace VRaptor;
#define DIM_OFFSET 10
#define SIZE_OFFSET 20
#define DATA_OFFSET 30
using std::vector;
using std::string;
namespace VRaptor {
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
	Context stmtTypeCodeGen(Statement *stmt, SymTable *symTable);
	Context exprTypeCodeGen(Expression* expr, SymTable *symTable);
	Context vTypeCodeGen(VType*returnType, SymTable *symTable);
	Context scalarTypeCodeGen(ScalarType *vtype);
	Context arrayTypeCodeGen(ArrayType* type, SymTable *symTable);
	bool enableOpenMP;
	bool enableSse;
public:
	bool getSseFlag();
	void setSseFlag(bool);
	bool getOpenmpFlag();
	void setOpenMpFlag(bool);
	Context funcCodeGen(VFunction *func);
	Context stmtCodeGen(Statement *stmt, SymTable *symTable);
	Context forStmtCodeGen(ForStmt *stmt, SymTable *symTable);
	Context assignStmtCodeGen(AssignStmt *stmt, SymTable *symTable);
	Context nameExprCodeGen(NameExpr *expr, SymTable *symTable);
	Context plusExprCodeGen(PlusExpr* expr, SymTable *symTable);
	Context domainExprCodeGen(DomainExpr *expr, SymTable *symTable);
	Context constExprCodeGen(ConstExpr *expr, SymTable *symTable);
	Context stmtListCodeGen(StmtList *stmt, SymTable *symTable);
	Context indexExprCodeGen(IndexExpr *expr, SymTable *symTable);
	Context minusExprCodeGen(MinusExpr *expr, SymTable *symTable);
	Context mMultExprCodeGen(MmultExpr* expr, SymTable *symTable);
	Context returnStmtCodeGen(ReturnStmt *stmt, SymTable *symTable);

	Context moduleCodeGen(VModule *vm);
	Context breakStmtCodeGen(BreakStmt *stmt);
	Context continueStmtCodeGen(ContinueStmt *stmt);
	Context ifStmtCodeGen(IfStmt *stmt, SymTable * symTable);
	Context whileStmtCodeGen(WhileStmt *stmt, SymTable *symTable);
	Context refOpStmtCodeGen(RefOpStmt stmt, SymTable *symTable);
	Context pForStmtCodeGen(PforStmt *stmt, SymTable *symTable);

	Context binaryExprCodeGen(BinaryExpr *expr, SymTable *symTable);
	Context multExprCodeGen(MultExpr*expr, SymTable *symTable);
	Context divExprCodeGen(DivExpr *expr, SymTable *symTable);
	Context geqExprCodeGen(GeqExpr* expr, SymTable *symTable);
	Context gtExprCodeGen(GtExpr* expr, SymTable *symTable);
	Context ltExprCodeGen(LtExpr*expr, SymTable *symTable);
	Context leqExprCodeGen(LeqExpr *expr, SymTable *symTable);
	Context andExprCodeGen(AndExpr *expr, SymTable *symTable);
	Context orExprCodeGen(OrExpr *expr, SymTable *symTable);
	Context notExprCodeGen(NotExpr *expr, SymTable *symTable); //to be written
	Context funCallExprCodeGen(FuncallExpr *expr, SymTable *symTable);
	Context libCallExprCodeGen(LibCallExpr *expr, SymTable *symtable);
	Context negateExprCodeGen(NegateExpr *expr, SymTable *symTable);
	Context vectoriseStmt(AssignStmt* stmt, int size, SymTable *symTable);
};

}

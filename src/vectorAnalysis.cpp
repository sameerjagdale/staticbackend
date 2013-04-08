#include "vectorAnalysis.hpp"

bitset<MAX> VectorAnalysis::analyse(StmtList *stmt) {
	bitset<MAX> stmtNum;
	VarMap map;
	cout << "entered analyse" << endl;
	for (int i = 0; i < stmt->getNumChildren(); i++) {
		bool val = stmtVectorAnalysis(stmt->getChild(i).get(),map);
		stmtNum.set(i, val);

	}
	return stmtNum;
}
bool VectorAnalysis::stmtVectorAnalysis(Statement *stmt,VarMap map) {
	cout << "entered statement vector " << endl;
	switch (stmt->getStmtType()) {
	case Statement::STMT_ASSIGN:
		return assignStmtVectorAnalysis((AssignStmt*) stmt);
	default:
		cout << "not assignment statement" << endl;
		return false;
	}
}
bool VectorAnalysis::assignStmtVectorAnalysis(AssignStmt *stmt) {
	cout << "entered assignment statement" << endl;
	if (stmt->getLhs().size() > 1) {
		return false;
	}

	bool leftVal = exprVectorAnalysis(stmt->getLhs()[0].get());

	bool rightVal = exprVectorAnalysis(stmt->getRhs().get());
	return leftVal && rightVal;
}

bool VectorAnalysis::exprVectorAnalysis(Expression* expr) {
	cout << "entered expression vector" << endl;
	if (expr->isBinaryExpr()) {
		return binaryExprVectorAnalysis((BinaryExpr*) expr);
	} else if (expr->getExprType() == Expression::NAME_EXPR) {
		return nameExprVectorAnalysis((NameExpr*) expr);
	} else if (expr->getExprType() == Expression::INDEX_EXPR) {
		return indexExprVectorAnalysis((IndexExpr*) expr);
	}

	return false;
}

bool VectorAnalysis::binaryExprVectorAnalysis(BinaryExpr *expr) {
	cout << "entered binary expression" << endl;
	bool leftVal = exprVectorAnalysis(expr->getLhs().get());
	bool rightVal = exprVectorAnalysis(expr->getRhs().get());
	return leftVal && rightVal;
}

bool VectorAnalysis::nameExprVectorAnalysis(NameExpr* expr) {
	cout << "entered name expression" << endl;
	return false;
}

bool VectorAnalysis::indexExprVectorAnalysis(IndexExpr*expr) {
	cout << "entered index expression" << endl;

	return false;
}

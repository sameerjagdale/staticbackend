#include "vectorAnalysis.hpp"
#include <vraptor.hpp>

void VectorAnalysis::analyse(StmtList *stmt) {
	for (int i = 0; i < stmt->getNumChildren(); i++) {
		if (invalidLoop) {
			break;
		}
		switch (stmt->getChild(i).get()->getStmtType()) {
		case Statement::STMT_ASSIGN:
			assignStmtVectorAnalysis((AssignStmt*) stmt->getChild(i).get(), i);
			break;
		default:
			stmtSet.reset();
			invalidLoop = true;
			break;
		}
	}
}

bool VectorAnalysis::assignStmtVectorAnalysis(AssignStmt*stmt, int stmtNo) {
	if (stmt->getLhs().size() > 1) {
		return false;
	}
	if (stmt->getLhs()[0].get()->getExprType() != Expression::INDEX_EXPR) {
		return false;
	}
	if (stmt->getRhs().get()->getExprType() != Expression::INDEX_EXPR) {
		return false;
	}
	return indexExprVectorAnalysis((IndexExpr*) stmt->getLhs()[0].get(), stmtNo)
			&& indexExprVectorAnalysis((IndexExpr*) stmt->getRhs().get(),
					stmtNo);
}

bool VectorAnalysis::indexExprVectorAnalysis(IndexExpr*expr, int stmtNo) {
	int indxId;
	if (expr->getIndices().size() > 1) {
		return false;
	}
	std::unordered_map<std::string, double>::const_iterator val = varMap.find(
			expr->getArrayId());
	if (val == varMap.end) {
		Expression *indxExpr = expr->getIndices()[0].get();
		if (indxExpr->getExprType() != Expression::NAME_EXPR) {
			return false;
		} else {
			NameExpr *name = (NameExpr*) indxExpr;
			indxId = name->getId();

		}
		std::pair<int, int> m(expr->getArrayId(), indxId);
		varMap.insert(m);
		vector<int> stmtVector;
		stmtVector.push_back(stmtNo);
		stmtMap.insert(
				std::pair<int, vector<int>>(expr->getArrayId(), stmtVector));
		return true;
	}
}

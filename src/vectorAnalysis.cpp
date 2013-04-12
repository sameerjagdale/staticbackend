#include "vectorAnalysis.hpp"
#include <vraptor.hpp>

void VectorAnalysis::analyse(StmtList *stmt) {
	bool val;
	for (int i = 0; i < stmt->getNumChildren(); i++) {
		if (invalidLoop) {
			break;
		}
		switch (stmt->getChild(i).get()->getStmtType()) {
		case Statement::STMT_ASSIGN:
			 val= assignStmtVectorAnalysis((AssignStmt*) stmt->getChild(i).get(), i);
			stmtSet.set(i,val);
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
	std::unordered_map<int,bool>::const_iterator arrayVal=arrayMap.find(expr->getArrayId());
	if(arrayVal!=arrayMap.end()){
		if(arrayVal->second){
			return false;
		}
	}
	std::unordered_map<int,int>::const_iterator val = varMap.find(
			expr->getArrayId());
	if (val == varMap.end()) {
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
	else{
		Expression *indxExpr=expr->getIndices()[0].get();
		if(indxExpr->getExprType()!=Expression::NAME_EXPR){
			return false;
		}		
		else{
			NameExpr *name=(NameExpr*)indxExpr;
			indxId=name->getId();
		}	
		int id =val->second;
		if(id==indxId){
			std::unordered_map<int,vector<int>>::const_iterator stmtIter=stmtMap.find(expr->getArrayId());
		
			vector<int> stmtVec=stmtIter->second;
			stmtVec.push_back(stmtNo);
			stmtMap.insert(std::pair<int,vector<int>>(expr->getArrayId(),stmtVec));
			return true;
		}		
		else{
			return false;
		}
				 
	}
}
bool VectorAnalysis::canVectorise(int stmtNo){
	return stmtSet[stmtNo];
}

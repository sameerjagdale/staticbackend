#include<vraptor.hpp>
#include<iostream>
#include<vector>
#include<bitset>
#include<unordered_map>
#define MAX 50
namespace std {
using namespace __gnu_cxx;
}
using namespace std;
using namespace VRaptor;
typedef unordered_map<string, string> VarMap;
class VectorAnalysis {
public:
	bitset<MAX> analyse(StmtList *stmt);
	bool stmtVectorAnalysis(Statement* stmt);
	bool assignStmtVectorAnalysis(AssignStmt* stmt, VarMap map);
	bool exprVectorAnalysis(Expression* expr, VarMap map);
	bool nameExprVectorAnalysis(NameExpr* expr, VarMap map);
	bool binaryExprVectorAnalysis(BinaryExpr* expr, VarMap map);
	bool indexExprVectorAnalysis(IndexExpr*expr, VarMap map);
};

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
typedef struct {
	vector<int> stmtNos;
	Expression* indx;
	bool invalid;
} StmtStruct;
typedef unordered_map<int, int> VarMap; // array id to index id
typedef unordered_map<int, vector<int>> StmtMap; // array id to stmt vector

class VectorAnalysis {
private:
	VarMap varMap;
	StmtMap stmtMap;
	bitset<MAX> stmtSet;
	bool invalidLoop;
public:
	VectorAnalysis() {
		invalidLoop = false;
	}
	void analyse(StmtList *stmt);
	bool assignStmtVectorAnalysis(AssignStmt*,int stmtNo);
	bool binaryExprVectorAnalysis(BinaryExpr*,int stmtNo);
	bool indexExprVectorAnalysis(IndexExpr*,int stmtNo);
	bool canVectorise(int stmtNo);
};

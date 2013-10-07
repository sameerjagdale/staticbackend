/**Copyright 2012, Rahul Garg and McGill University
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#ifndef VRAPTOR_H
#include <vector>
#include <string>
#define VR_MINIMAL
#ifndef VR_MINIMAL
#include "llvm/DerivedTypes.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/PassManager.h"
#endif

#include "vtypes.hpp"
#include <tr1/memory>
using std::tr1::shared_ptr;
using std::tr1::static_pointer_cast;

namespace VRaptor {

class Node {
protected:
	VTypePtr m_vtype;
public:
	enum NodeType {
		STMT_TYPE, EXPR_TYPE, FUNC_TYPE
	};
	virtual NodeType getNodeType() const=0;
	VTypePtr getType() const {
		return m_vtype;
	}
	void setType(VTypePtr v) {
		m_vtype = v;
	}
	virtual ~Node() {
	}
};

class Expression;
typedef shared_ptr<Expression> ExpressionPtr;
typedef std::vector<ExpressionPtr> ExpressionPtrVector;

class Expression: public Node {
public:
	enum ExprType {
		CONST_EXPR,
		NAME_EXPR,
		PLUS_EXPR,
		MINUS_EXPR,
		MULT_EXPR,
		DIV_EXPR,
		INDEX_EXPR,
		NEGATE_EXPR,
		MMULT_EXPR,
		TRANS_EXPR,
		GT_EXPR,
		GEQ_EXPR,
		LT_EXPR,
		LEQ_EXPR,
		AND_EXPR,
		OR_EXPR,
		NOT_EXPR,
		FUNCALL_EXPR,
		DOMAIN_EXPR,
		DIM_EXPR,
		DIMVEC_EXPR,
		LIBCALL_EXPR,
		ALLOC_EXPR,
		IMAG_EXPR,
		REAL_EXPR,
		REDUCE_EXPR,
		CAST_EXPR
	};
	NodeType getNodeType() const {
		return EXPR_TYPE;
	}
	virtual void getChildren(ExpressionPtrVector& children) const=0;
	virtual void getChildrenDeep(ExpressionPtrVector& children) const;
	virtual ExprType getExprType() const=0;
	virtual ~Expression() {
	}
	static ExpressionPtr readFromXml(Poco::XML::Node* elem);
	virtual bool isBinaryExpr() const {
		return false;
	}
	virtual ExpressionPtr clone()=0;
};

class ConstExprBuilder;

class ConstExpr: public Expression {
	void *m_val;
public:
	ConstExpr(void *val) :
			m_val(val) {
	}
	ExprType getExprType() const {
		return CONST_EXPR;
	}
	int getIntVal() {
		int *ptr = (int*) m_val;
		return *ptr;
	}
	long int getLongVal() {
		long int *ptr = (long int*) m_val;
		return *ptr;
	}
	float getFloatVal() {
		float *ptr = (float*) m_val;
		return *ptr;
	}
	double getDoubleVal() {
		double *ptr = (double*) m_val;
		return *ptr;
	}
	ExpressionPtr clone();
	void getChildren(std::vector<ExpressionPtr> &children) const {
		return;
	}
};
typedef shared_ptr<ConstExpr> ConstExprPtr;

class ConstExprBuilder {
public:
	static ConstExprPtr getIntConstExpr(int val);
	static ConstExprPtr getFloatConstExpr(float val);
	static ConstExprPtr getDoubleConstExpr(double val);
	static ConstExprPtr getLongConstExpr(long int val);
};

class NameExpr: public Expression {
	int m_id;
public:
	NameExpr(int id) :
			m_id(id) {
	}
	ExprType getExprType() const {
		return NAME_EXPR;
	}
	int getId() {
		return m_id;
	}
	ExpressionPtr clone();
	void getChildren(std::vector<ExpressionPtr> &children) const {
		return;
	}
};
typedef shared_ptr<NameExpr> NameExprPtr;

class BinaryExpr: public Expression {
protected:
	ExpressionPtr m_rhs;
	ExpressionPtr m_lhs;
public:
	bool isBinaryExpr() const {
		return true;
	}
	void getChildren(std::vector<ExpressionPtr>& children) const;
	ExpressionPtr getRhs() {
		return m_rhs;
	}
	ExpressionPtr getLhs() {
		return m_lhs;
	}
	virtual void setRhs(ExpressionPtr e) {
		m_rhs = e;
	}
	virtual void setLhs(ExpressionPtr e) {
		m_lhs = e;
	}
	virtual ~BinaryExpr() {
	}
};
typedef shared_ptr<BinaryExpr> BinaryExprPtr;

class PlusExpr: public BinaryExpr {
public:
	ExprType getExprType() const {
		return PLUS_EXPR;
	}
	PlusExpr(ExpressionPtr l, ExpressionPtr r) {
		m_lhs = l;
		m_rhs = r;
	}
	ExpressionPtr clone();
};
typedef shared_ptr<PlusExpr> PlusExprPtr;

class MinusExpr: public BinaryExpr {
public:
	ExprType getExprType() const {
		return MINUS_EXPR;
	}
	MinusExpr(ExpressionPtr l, ExpressionPtr r) {
		m_lhs = l;
		m_rhs = r;
	}
	ExpressionPtr clone();
};
typedef shared_ptr<MinusExpr> MinusExprPtr;

class MultExpr: public BinaryExpr {
public:
	ExprType getExprType() const {
		return MULT_EXPR;
	}
	MultExpr(ExpressionPtr l, ExpressionPtr r) {
		m_lhs = l;
		m_rhs = r;
	}
	ExpressionPtr clone();
};
typedef shared_ptr<MultExpr> MultExprPtr;

class MmultExpr: public BinaryExpr {
public:
	ExprType getExprType() const {
		return MMULT_EXPR;
	}
	MmultExpr(ExpressionPtr l, ExpressionPtr r) {
		m_lhs = l;
		m_rhs = r;
	}
	ExpressionPtr clone();
};
typedef shared_ptr<MmultExpr> MmultExprPtr;

class DivExpr: public BinaryExpr {
public:
	ExprType getExprType() const {
		return DIV_EXPR;
	}
	DivExpr(ExpressionPtr l, ExpressionPtr r) {
		m_lhs = l;
		m_rhs = r;
	}
	ExpressionPtr clone();
};
typedef shared_ptr<DivExpr> DivExprPtr;

class GtExpr: public BinaryExpr {
public:
	ExprType getExprType() const {
		return GT_EXPR;
	}
	GtExpr(ExpressionPtr l, ExpressionPtr r) {
		m_lhs = l;
		m_rhs = r;
	}
	ExpressionPtr clone();
};
typedef shared_ptr<GtExpr> GtExprPtr;

class GeqExpr: public BinaryExpr {
public:
	ExprType getExprType() const {
		return GEQ_EXPR;
	}
	GeqExpr(ExpressionPtr l, ExpressionPtr r) {
		m_lhs = l;
		m_rhs = r;
	}
	ExpressionPtr clone();
};
typedef shared_ptr<GeqExpr> GeqExprPtr;

class LtExpr: public BinaryExpr {
public:
	ExprType getExprType() const {
		return LT_EXPR;
	}
	LtExpr(ExpressionPtr l, ExpressionPtr r) {
		m_lhs = l;
		m_rhs = r;
		m_vtype.reset(new ScalarType(ScalarType::SCALAR_BOOL, 1));
	}
	ExpressionPtr clone();
};
typedef shared_ptr<LtExpr> LtExprPtr;

class LeqExpr: public BinaryExpr {
public:
	ExprType getExprType() const {
		return LEQ_EXPR;
	}
	LeqExpr(ExpressionPtr l, ExpressionPtr r) {
		m_lhs = l;
		m_rhs = r;
	}
	ExpressionPtr clone();
};
typedef shared_ptr<LeqExpr> LeqExprPtr;

class AndExpr: public BinaryExpr {
public:
	ExprType getExprType() const {
		return AND_EXPR;
	}
	AndExpr(ExpressionPtr l, ExpressionPtr r) {
		m_lhs = l;
		m_rhs = r;
	}
	ExpressionPtr clone();
};

class OrExpr: public BinaryExpr {
public:
	ExprType getExprType() const {
		return OR_EXPR;
	}
	OrExpr(ExpressionPtr l, ExpressionPtr r) {
		m_lhs = l;
		m_rhs = r;
	}
	ExpressionPtr clone();
};

class UnaryExpr: public Expression {
protected:
	ExpressionPtr m_expr;
public:
	ExpressionPtr getBaseExpr() {
		return m_expr;
	}
	void getChildren(std::vector<ExpressionPtr> &children) const {
		children.push_back(m_expr);
	}
	void setBaseExpr(ExpressionPtr expr) {
		m_expr = expr;
	}
};

class NotExpr: public UnaryExpr {
public:
	NotExpr(ExpressionPtr expr);
	ExprType getExprType() const {
		return NOT_EXPR;
	}
};

class NegateExpr: public UnaryExpr {
public:
	NegateExpr(ExpressionPtr expr);
	ExprType getExprType() const {
		return NEGATE_EXPR;
	}
	ExpressionPtr clone();
};
typedef shared_ptr<NegateExpr> NegateExprPtr;

class FuncallExpr: public Expression {
	ExpressionPtrVector m_args;
	NameExprPtr m_name;
public:
	FuncallExpr(NameExprPtr name, ExpressionPtrVector args) :
			m_name(name), m_args(args) {
	}
	ExprType getExprType() const {
		return FUNCALL_EXPR;
	}
	ExpressionPtr clone();
	void getChildren(ExpressionPtrVector &children) const;
	NameExprPtr getFuncName() {
		return m_name;
	}
	ExpressionPtrVector getArgs() {
		return m_args;
	}
};

class IndexExpr: public Expression {
	NameExprPtr m_name;
	ExpressionPtrVector m_indices;
	bool m_isBoundsChecked;
	bool m_isNegativeMode;
	bool m_isFlattened;
	bool m_isSliceCopy;
public:
	IndexExpr(NameExprPtr name, ExpressionPtrVector indices,
			bool isBoundsChecked, bool isNegativeMode, bool isFlattened,
			bool copySlice) :
			m_name(name), m_indices(indices), m_isBoundsChecked(
					isBoundsChecked), m_isNegativeMode(isNegativeMode), m_isFlattened(
					isFlattened), m_isSliceCopy(copySlice) {
	}
	IndexExpr(NameExprPtr name, ExpressionPtr expr1, ExpressionPtr expr2,
			bool isBoundsChecked, bool isNegativeMode, bool isFlattened);
	IndexExpr(NameExprPtr name, ExpressionPtr expr, bool isBoundsChecked,
			bool isNegativeMode, bool isFlattened);
	ExprType getExprType() const {
		return INDEX_EXPR;
	}
	void getChildren(std::vector<ExpressionPtr>& children) const;
	bool isBoundsChecked() const {
		return m_isBoundsChecked;
	}
	bool isNegativeMode() const {
		return m_isNegativeMode;
	}
	bool isFlattened() const {
		return m_isFlattened;
	}
	bool isSliceCopy() const {
		return m_isSliceCopy;
	}
	int getArrayId() const {
		return m_name->getId();
	}
	NameExprPtr getNameExpr() {
		return m_name;
	}
	ExpressionPtrVector getIndices() const {
		return m_indices;
	}
	void setIndices(const ExpressionPtrVector& vec) {
		m_indices = vec;
	}
	ExpressionPtr clone();
};
typedef shared_ptr<IndexExpr> IndexExprPtr;

class DomainExpr: public Expression {
	int m_ndims;
	ExpressionPtrVector m_exprs;
public:
	DomainExpr(int ndims);
	ExprType getExprType() const {
		return DOMAIN_EXPR;
	}
	int getNdims() const {
		return m_ndims;
	}
	void setStartExpr(int i, ExpressionPtr e) {
		m_exprs[i * 3] = e;
	}
	void setStepExpr(int i, ExpressionPtr e) {
		m_exprs[i * 3 + 1] = e;
	}
	void setStopExpr(int i, ExpressionPtr e) {
		m_exprs[i * 3 + 2] = e;
	}
	ExpressionPtr getStartExpr(int i) {
		return m_exprs[i * 3];
	}
	ExpressionPtr getStepExpr(int i) {
		return m_exprs[i * 3 + 1];
	}
	ExpressionPtr getStopExpr(int i) {
		return m_exprs[i * 3 + 2];
	}
	ExpressionPtr clone();
	void getChildren(ExpressionPtrVector &children) const;
};
typedef shared_ptr<DomainExpr> DomainExprPtr;

class LibCallExpr: public UnaryExpr {
public:
	enum Code {
		LIB_SQRT,
		LIB_LOG2,
		LIB_LOG10,
		LIB_LOGE,
		LIB_EXPE,
		LIB_EXP10,
		LIB_SIN,
		LIB_COS,
		LIB_TAN,
		LIB_ASIN,
		LIB_ACOS,
		LIB_ATAN
	};
	LibCallExpr(Code c, ExpressionPtr base) :
			m_code(c) {
		m_expr = base;
	}
	ExpressionPtr clone();
	ExprType getExprType() const {
		return LIBCALL_EXPR;
	}
	LibCallExpr::Code getCode() {
		return m_code;
	}

private:
	Code m_code;
};

class ReduceExpr: public Expression {
	ExpressionPtr m_array;
	ExpressionPtr m_axis;
public:
	enum Op {
		REDUCE_PROD, REDUCE_SUM
	};
	ReduceExpr(ExpressionPtr array, ExpressionPtr axis) :
			m_array(array), m_axis(axis) {
	}
	ExpressionPtr clone();
	ExprType getExprType() const {
		return REDUCE_EXPR;
	}
	void getChildren(std::vector<ExpressionPtr> &children) const;
};

class DimExpr: public Expression {
	ExpressionPtr m_expr;
	NameExprPtr m_array;
public:
	DimExpr(NameExprPtr array, ExpressionPtr dim) :
			m_array(array), m_expr(dim) {
	}
	void getChildren(std::vector<ExpressionPtr> &children) const;
	ExpressionPtr clone();
	ExprType getExprType() const {
		return DIM_EXPR;
	}
};

class DimvecExpr: public Expression {
	NameExprPtr m_array;
public:
	DimvecExpr(NameExprPtr array) :
			m_array(array) {
	}
	ExpressionPtr clone();
	void getChildren(std::vector<ExpressionPtr> &children) const;
	ExprType getExprType() const {
		return DIMVEC_EXPR;
	}
};

class TransExpr: public UnaryExpr {
	bool m_isComplexConj;
public:
	TransExpr(ExpressionPtr array, bool isComplexConugate = false) :
			m_isComplexConj(isComplexConugate) {
		m_expr = array;
	}
	ExpressionPtr clone();
	ExprType getExprType() const {
		return TRANS_EXPR;
	}
};

class AllocExpr: public Expression {
public:
	enum AllocTag {
		ALLOC_ONES, ALLOC_ZEROS
	};
private:
	ExpressionPtrVector m_exprs;
	AllocTag m_tag;
public:
	AllocExpr(const ExpressionPtrVector& vec, AllocTag tag) :
			m_exprs(vec), m_tag(tag) {
	}
	ExprType getExprType() const {
		return ALLOC_EXPR;
	}
	AllocTag getTag() const {
		return m_tag;
	}
	ExpressionPtr getDim(int i) const;
	ExpressionPtr clone();
	void getChildren(ExpressionPtrVector &children) const;
};

class ImagExpr: public UnaryExpr {
public:
	ImagExpr(ExpressionPtr base) {
		m_expr = base;
	}
	ExprType getExprType() const {
		return IMAG_EXPR;
	}
};

class RealExpr: public UnaryExpr {
public:
	RealExpr(ExpressionPtr base) {
		m_expr = base;
	}
	ExprType getExprType() const {
		return REAL_EXPR;
	}
};

class CastExpr: public UnaryExpr {
public:
	CastExpr(ExpressionPtr expr) {
		m_expr = expr;
	}
	ExprType getExprType() const {
		return CAST_EXPR;
	}
};

class Statement;
typedef shared_ptr<Statement> StmtPtr;
typedef std::vector<StmtPtr> StmtPtrVector;

class Statement: public Node {
protected:
	bool m_onGpu;
public:
	enum StmtType {
		STMT_ASSIGN,
		STMT_IF,
		STMT_WHILE,
		STMT_FOR,
		STMT_PFOR,
		STMT_LIST,
		STMT_BREAK,
		STMT_CONTINUE,
		STMT_BOUNDCHECK,
		STMT_RETURN,
		STMT_REFINCR,
		STMT_REFDECR,
		STMT_LIBCALL
	};
	NodeType getNodeType() const {
		return STMT_TYPE;
	}
	virtual StmtType getStmtType() const=0;
	bool isOnGpu() {
		return m_onGpu;
	}
	static StmtPtr readFromXml(Poco::XML::Node* n);
	virtual bool isLoop() const {
		return false;
	}
	virtual ~Statement() {
	}
	//virtual void insertBefore(StmtPtr toInsert){}
	void setGpu(bool mode) {
		m_onGpu = mode;
	}
	//virtual StmtPtr clone()=0;
};

class StmtList: public Statement {
	StmtPtrVector m_list;
public:
	StmtList(const StmtPtrVector& list) :
			m_list(list) {
		m_onGpu = false;
	}
	StmtType getStmtType() const {
		return STMT_LIST;
	}
	StmtPtr getChild(int i) const {
		return m_list[i];
	}
	size_t getNumChildren() const {
		return m_list.size();
	}
	static StmtPtr readFromXml(Poco::XML::Node* n);
	void insertAt(int index, StmtPtr stmt);
	//StmtPtr clone();
};
typedef shared_ptr<StmtList> StmtListPtr;

class BoundCheckStmt: public Statement {
private:
	int m_arrayid;
	int m_dim;
	ExpressionPtr m_expr;
public:
	BoundCheckStmt(int arrayid, int dim, ExpressionPtr expr) :
			m_arrayid(arrayid), m_dim(dim), m_expr(expr) {
	}
	//StmtPtr clone(){return new BoundCheckStmt(m_arrayid,m_dim,m_expr->clone());}
	int getArrayId() {
		return m_arrayid;
	}
	int getDim() {
		return m_dim;
	}
	ExpressionPtr getExpression() {
		return m_expr;
	}
	StmtType getStmtType() const {
		return STMT_BOUNDCHECK;
	}
};

class AssignStmt: public Statement {
	std::vector<ExpressionPtr> m_lhs;
	ExpressionPtr m_rhs;
public:
	AssignStmt(std::vector<ExpressionPtr> lhs, ExpressionPtr rhs) :
			m_lhs(lhs), m_rhs(rhs) {
	}
	std::vector<shared_ptr<Node> > getChildren() const;
	StmtType getStmtType() const {
		return STMT_ASSIGN;
	}
	static StmtPtr readFromXml(Poco::XML::Node* n);
	ExpressionPtr getRhs() {
		return m_rhs;
	}
	std::vector<ExpressionPtr> getLhs() {
		return m_lhs;
	}
	//StmtPtr clone();
	void setRhs(ExpressionPtr e) {
		m_rhs = e;
	}
	void setLhs(std::vector<ExpressionPtr> v) {
		m_lhs = v;
	}
};
typedef shared_ptr<AssignStmt> AssignStmtPtr;

class IfStmt: public Statement {
	ExpressionPtr m_cond;
	StmtPtr m_ifbranch;
	StmtPtr m_elsebranch;
	bool m_hasElse;
public:
	IfStmt(ExpressionPtr cond, StmtPtr ifbranch, StmtPtr elsebranch);
	IfStmt(ExpressionPtr cond, StmtPtr ifbranch);
	std::vector<Node*> getChildre();
	StmtType getStmtType() const {
		return STMT_IF;
	}
	StmtPtr getIfBranch() const {
		return m_ifbranch;
	}
	StmtPtr getElseBranch() const {
		return m_elsebranch;
	}
	ExpressionPtr getCond() const {
		return m_cond;
	}
	bool hasElseBranch() const {
		return m_hasElse;
	}
	static StmtPtr readFromXml(Poco::XML::Node* n);
	//StmtPtr clone();
};
typedef shared_ptr<IfStmt> IfStmtPtr;

class ForStmt: public Statement {
protected:
	std::vector<int> m_vars;
	ExpressionPtr m_domain;
	StmtPtr m_stmt;
public:
	ForStmt(std::vector<int> vars, ExpressionPtr domain, StmtPtr stmt);
	virtual StmtType getStmtType() const {
		return STMT_FOR;
	}
	virtual bool isLoop() const {
		return true;
	}
	static StmtPtr readFromXml(Poco::XML::Node* n);
	std::vector<int> getIterVars() {
		return m_vars;
	}
	StmtPtr getBody() const {
		return m_stmt;
	}
	ExpressionPtr getDomain() const {
		return m_domain;
	}
	//StmtPtr clone();
};
typedef shared_ptr<ForStmt> ForStmtPtr;

class PforStmt: public Statement {
	std::vector<int> m_privates;
	std::vector<int> m_vars;
	ExpressionPtr m_domain;
	StmtPtr m_stmt;
public:
	PforStmt(std::vector<int> vars, std::vector<int> privates,
			ExpressionPtr domain, StmtPtr stmt);
	StmtType getStmtType() const {
		return STMT_PFOR;
	}
	bool isLoop() const {
		return true;
	}
	std::vector<int> getIterVars() const {
		return m_vars;
	}
	ExpressionPtr getDomain() const {
		return m_domain;
	}
	StmtPtr getBody() {
		return m_stmt;
	}
	static StmtPtr readFromXml(Poco::XML::Node* n);
	std::vector<int> getPrivateVars() const {
		return m_privates;
	}
	//StmtPtr clone();
};
typedef shared_ptr<PforStmt> PforStmtPtr;

class WhileStmt: public Statement {
	ExpressionPtr cond;
	StmtPtr body;
public:
	StmtType getStmtType() const {
		return STMT_WHILE;
	}
	bool isLoop() const {
		return true;
	}
	StmtPtr getBody() const {
		return body;
	}
	ExpressionPtr getCond() const {
		return cond;
	}
	static StmtPtr readFromXml(Poco::XML::Node* n);
	WhileStmt(ExpressionPtr c, StmtPtr b) :
			cond(c), body(b) {
	}
	//StmtPtr clone();
};
typedef shared_ptr<WhileStmt> WhileStmtPtr;

class BreakStmt: public Statement {
public:
	StmtType getStmtType() const {
		return STMT_BREAK;
	}
};

class ContinueStmt: public Statement {
public:
	StmtType getStmtType() const {
		return STMT_CONTINUE;
	}
};

class RefOpStmt: public Statement {
	NameExprPtr m_var;
	bool m_isIncr;
public:
	RefOpStmt(NameExprPtr expr, bool isIncr) :
			m_var(expr), m_isIncr(isIncr) {
	}
	StmtType getStmtType() const {
		if (m_isIncr)
			return STMT_REFINCR;
		else
			return STMT_REFDECR;
	}
	bool isIncr() const {
		return m_isIncr;
	}
	NameExprPtr getName() const {
		return m_var;
	}
};

class ReturnStmt: public Statement {
	std::vector<int> m_rids;
public:
	ReturnStmt(int i);
	ReturnStmt(std::vector<int> ids) :
			m_rids(ids) {
	}
	StmtType getStmtType() const {
		return STMT_RETURN;
	}
	static StmtPtr readFromXml(Poco::XML::Node* n);
	std::vector<int> getRids() {
		return m_rids;
	}
	StmtPtr clone();
};
typedef shared_ptr<ReturnStmt> ReturnStmtPtr;

class LibCallStmt: public Statement {
	ExpressionPtr m_lhs;
	ExpressionPtr m_rhs;
	bool m_isAlloc;
	bool m_checkDims;
public:
	LibCallStmt(ExpressionPtr lhs, ExpressionPtr rhs, bool isAlloc = true,
			bool checkDims = false) :
			m_lhs(lhs), m_rhs(rhs), m_isAlloc(isAlloc), m_checkDims(checkDims) {
	}
	ExpressionPtr getRhs() const {
		return m_rhs;
	}
	ExpressionPtr getLhs() const {
		return m_lhs;
	}
	bool isAlloc() const {
		return m_isAlloc;
	}
	bool isCheckDims() const {
		return m_checkDims;
	}
	static StmtPtr readFromXml(Poco::XML::Node *n);
	StmtPtr clone();
	StmtType getStmtType() const {
		return STMT_LIBCALL;
	}
};
typedef shared_ptr<LibCallStmt> LibCallStmtPtr;

class ArrayShape {
	int ndims;
	std::vector<size_t> m_sizes;
	std::vector<int> m_strides;
	std::vector<bool> m_sizeKnown;
	std::vector<bool> m_strideKnown;
public:
	bool isAllSizesKnown() const;
	std::vector<size_t> getAllSizes() const;
	bool isSizeKnown(int i) const;
	size_t getSize(int i) const;
	void setSizes(std::vector<size_t> sizes);
	void setSize(int dim, size_t size);

	bool isStrideKnown(int i) const;
	int getStride(int i) const;
	bool isAllStridesKnown() const;
	std::vector<int> getAllStrides() const;
	void setStrides(std::vector<int> strides);
	void setStride(int dim, int stride);
};

class SymTable {
	std::map<int, std::string> m_namemap;
	std::map<int, VTypePtr> m_typemap;
public:
	std::string getName(int id) {
		return m_namemap[id];
	}
	VTypePtr getType(int id);
	void addEntry(int id, const std::string& name, VTypePtr type);
	void addEntry(int id, const std::string& name);
	static SymTable* readFromXml(Poco::XML::Node* n);
	int getSize() {
		return m_namemap.size();
	}
	std::vector<int> getAllIds();
};

class VModule;
//class VCompiler;

class VFunction: public Node {
public:
	struct Arg {
		int m_id;
		bool m_restrict;
	};
	typedef int (*fntype)(void **, int *);
private:
	StmtListPtr m_body;
	SymTable *m_symtable;
	std::string m_name;
	std::vector<Arg> m_args;
	VModule *m_module;
	fntype m_compiledfn;
	bool m_doCleanup;
public:
	VFunction(std::string name, StmtListPtr body, SymTable *table,
			std::vector<Arg> args, shared_ptr<FuncType> fntype);
	void setModule(VModule *mod) {
		m_module = mod;
	}
	NodeType getNodeType() {
		return FUNC_TYPE;
	}
	static VFunction *readFromXml(Poco::XML::Node* elem);
	NodeType getNodeType() const {
		return FUNC_TYPE;
	}
	std::string writeToXml();
	std::string getName() const {
		return m_name;
	}
	void compile();
	void compileWithoutLower();
	VModule *getModule() const {
		return m_module;
	}
	SymTable *getSymTable() {
		return m_symtable;
	}
	StmtListPtr getBody() const {
		return m_body;
	}
	int createTempVar(VTypePtr vtype);
	int call(void** args, int* errnode);
	void disableCleanup() {
		m_doCleanup = false;
	}
	bool requiresCleanup() {
		return m_doCleanup;
	}
	friend class VCompiler;
};

class VContext;

class VModule {
	std::vector<VFunction*> m_funcs;
#ifndef VR_MINIMAL
	VContext* m_ctx;
	VCompiler *m_compiler;
#endif
	std::string m_name;
public:
	~VModule();
#ifndef VR_MINIMAL
	void setContext(VContext *ctx) {m_ctx = ctx;}
	VContext *getContext() {return m_ctx;}
#endif
	std::string getName() const {
		return m_name;
	}
	static VModule* readFromString(std::string s);
	VFunction *getFunction(std::string s);
	void registerRuntimeFuncs();
	void compile();
	friend class VCompiler;
	friend class VectorAnalysis;
#ifndef VR_MINIMAL
	VCompiler *getCompiler();
	friend class VCompiler;
#endif
};

#ifndef VR_MINIMAL
class VContext {
	llvm::LLVMContext *m_context;
	bool m_enableGPU;
	bool m_enableParfor;
	std::vector<VModule*> m_modules;

	//All of these sizes are in bytes
	int m_stridePtrOffset;
	int m_strideIntSize;
	int m_dimsPtrOffset;
	int m_dimsIntSize;
	int m_dataPtrOffset;
public:
	VContext();
	~VContext();
	void addModule(VModule* m);
	void removeModule(VModule *m);
	void enableGPU() {m_enableGPU = true;}
	void disableGPU() {m_enableGPU = false;}
	void enableParfor() {m_enableParfor = true;}
	void disableParfor() {m_enableParfor = false;}
	void setStridePtrOffset(int val) {m_stridePtrOffset = val;}
	int getStridePtrOffset() const {return m_stridePtrOffset;}
	void setDataPtrOffset(int val) {m_dataPtrOffset = val;}
	int getDataPtrOffset() const {return m_dataPtrOffset;}
	void setDimsPtrOffset(int val) {m_dimsPtrOffset = val;}
	int getDimsPtrOffset() const {return m_dimsPtrOffset;}
	void setDimsIntSize(int val) {m_dimsIntSize = val;}
	int getDimsIntSize() const {return m_dimsIntSize;}
	void setStrideIntSize(int val) {m_strideIntSize = val;}
	int getStrideIntSize() const {return m_strideIntSize;}

	llvm::LLVMContext *getLLVMContext() {return m_context;}
};
#endif
}
;

#define VRAPTOR_H
#endif

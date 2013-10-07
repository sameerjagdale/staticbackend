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

#include "vraptor.hpp"
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <utility>
#include <vector>

#ifndef VR_MINIMAL
#include "vruntime.hpp"
#include "vcompile.hpp"
#endif
using namespace VRaptor;
using std::vector;
using std::cout;
using std::endl;

ForStmt::ForStmt(std::vector<int> vars, ExpressionPtr domain, StmtPtr stmt) :
		m_vars(vars), m_domain(domain), m_stmt(stmt) {
	if (m_stmt->getStmtType() != Statement::STMT_LIST) {
		StmtPtrVector list;
		list.push_back(m_stmt);
		m_stmt.reset(new StmtList(list));
	}
}

PforStmt::PforStmt(std::vector<int> vars, std::vector<int> privates,
		ExpressionPtr domain, StmtPtr stmt) :
		m_vars(vars), m_privates(privates), m_domain(domain), m_stmt(stmt) {
	if (m_stmt->getStmtType() != Statement::STMT_LIST) {
		StmtPtrVector list;
		list.push_back(m_stmt);
		m_stmt.reset(new StmtList(list));
	}
}

IfStmt::IfStmt(ExpressionPtr cond, StmtPtr ifbranch, StmtPtr elsebranch) :
		m_cond(cond), m_ifbranch(ifbranch), m_elsebranch(elsebranch) {
//	cout << "IfElse(): Created if/else with condition type "
//			<< cond->getExprType() << endl;
	if (elsebranch != NULL) {
		m_hasElse = true;
	} else {
		m_hasElse = false;
	}
}

IfStmt::IfStmt(ExpressionPtr cond, StmtPtr ifbranch) :
		m_cond(cond), m_ifbranch(ifbranch), m_hasElse(false) {

}
/*
 StmtPtr ForStmt::clone(){
 ForStmtPtr fstmt(new ForStmt(m_vars,m_domain->clone(),m_stmt->clone()));
 return fstmt;
 }


 StmtPtr IfStmt::clone(){
 StmtPtr newptr;
 if(m_hasElse){
 newptr.reset(new IfStmt(m_cond->clone(),m_ifbranch->clone(),m_elsebranch->clone()));
 }else{
 newptr.reset(new IfStmt(m_cond->clone(),m_ifbranch->clone()));
 }
 return newptr;
 }


 StmtPtr PforStmt::clone(){
 PforStmtPtr pstmt(new PforStmt(m_vars,m_privates,m_domain->clone(),m_stmt->clone()));
 return pstmt;
 }

 StmtPtr StmtList::clone(){
 StmtPtrVector stmts;
 for(int i=0;i<m_list.size();i++) stmts.push_back(m_list[i]->clone());
 StmtListPtr slist(new StmtList(m_list));
 slist->setGpu(m_onGpu);
 return slist;
 }

 StmtPtr WhileStmt::clone(){
 WhileStmtPtr wstmt(new WhileStmt(this->cond->clone(),this->body->clone()));
 return wstmt;
 }


 StmtPtr AssignStmt::clone(){
 ExpressionPtrVector lhs;
 for(int i=0;i<m_lhs.size();i++) lhs.push_back(m_lhs[i]->clone());
 AssignStmtPtr astmt(new AssignStmt(lhs,m_rhs->clone()));
 return astmt;
 }
 */

ExpressionPtr IndexExpr::clone() {
	ExpressionPtrVector indices;
	for (int i = 0; i < m_indices.size(); i++)
		indices.push_back(m_indices[i]->clone());
	IndexExprPtr iexpr(
			new IndexExpr(static_pointer_cast<NameExpr>(m_name->clone()),
					indices, m_isBoundsChecked, m_isNegativeMode, m_isFlattened,
					m_isSliceCopy));
	iexpr->setType(m_vtype);
	return iexpr;
}

ExpressionPtr PlusExpr::clone() {
	PlusExprPtr pexpr(new PlusExpr(m_lhs->clone(), m_rhs->clone()));
	pexpr->setType(m_vtype);
	return pexpr;
}

ExpressionPtr MinusExpr::clone() {
	MinusExprPtr pexpr(new MinusExpr(m_lhs->clone(), m_rhs->clone()));
	pexpr->setType(m_vtype);
	return pexpr;
}

ExpressionPtr MultExpr::clone() {
	MultExprPtr pexpr(new MultExpr(m_lhs->clone(), m_rhs->clone()));
	pexpr->setType(m_vtype);
	return pexpr;
}

ExpressionPtr DivExpr::clone() {
	DivExprPtr pexpr(new DivExpr(m_lhs->clone(), m_rhs->clone()));
	pexpr->setType(m_vtype);
	return pexpr;
}

ExpressionPtr LtExpr::clone() {
	LtExprPtr pexpr(new LtExpr(m_lhs->clone(), m_rhs->clone()));
	pexpr->setType(m_vtype);
	return pexpr;
}

ExpressionPtr LeqExpr::clone() {
	LeqExprPtr lexpr(new LeqExpr(m_lhs->clone(), m_rhs->clone()));
	lexpr->setType(m_vtype);
	return lexpr;
}

ExpressionPtr GtExpr::clone() {
	GtExprPtr pexpr(new GtExpr(m_lhs->clone(), m_rhs->clone()));
	pexpr->setType(m_vtype);
	return pexpr;
}

ExpressionPtr GeqExpr::clone() {
	ExpressionPtr pexpr(new GeqExpr(m_lhs->clone(), m_rhs->clone()));
	pexpr->setType(m_vtype);
	return pexpr;
}

ExpressionPtr MmultExpr::clone() {
	ExpressionPtr pexpr(new MmultExpr(m_lhs->clone(), m_rhs->clone()));
	pexpr->setType(m_vtype);
	return pexpr;
}

ExpressionPtr DomainExpr::clone() {
	DomainExprPtr dexpr(new DomainExpr(m_ndims));
	for (int i = 0; i < m_ndims; i++) {
		dexpr->setStartExpr(i, getStartExpr(i)->clone());
		dexpr->setStepExpr(i, getStepExpr(i)->clone());
		dexpr->setStopExpr(i, getStopExpr(i)->clone());
	}
	dexpr->setType(m_vtype);
	return dexpr;
}

ExpressionPtr ConstExpr::clone() {
	ConstExprPtr cexpr(new ConstExpr(m_val));
	cexpr->setType(m_vtype);
	return cexpr;
}

ExpressionPtr FuncallExpr::clone() {
	vector<ExpressionPtr> args;
	for (int i = 0; i < m_args.size(); i++)
		args.push_back(m_args[i]->clone());
	shared_ptr<FuncallExpr> fexpr(
			new FuncallExpr(static_pointer_cast<NameExpr>(m_name->clone()),
					args));
	fexpr->setType(m_vtype);
	return fexpr;
}

ExpressionPtr NameExpr::clone() {
	NameExprPtr nexpr(new NameExpr(m_id));
	nexpr->setType(m_vtype);
	return nexpr;
}

ExpressionPtr DimExpr::clone() {
	shared_ptr<DimExpr> dexpr(
			new DimExpr(static_pointer_cast<NameExpr>(m_array->clone()),
					m_expr->clone()));
	dexpr->setType(m_vtype);
	return dexpr;
}

ExpressionPtr DimvecExpr::clone() {
	shared_ptr<DimvecExpr> dexpr(
			new DimvecExpr(static_pointer_cast<NameExpr>(m_array->clone())));
	dexpr->setType(m_vtype);
	return dexpr;
}

ExpressionPtr AllocExpr::clone() {
	ExpressionPtr aexpr(new AllocExpr(m_exprs, m_tag));
	aexpr->setType(m_vtype);
	return aexpr;
}

ExpressionPtr LibCallExpr::clone() {
	shared_ptr<LibCallExpr> lexpr(new LibCallExpr(m_code, m_expr));
	lexpr->setType(m_vtype);
	return lexpr;
}

ExpressionPtr NegateExpr::clone() {
	NegateExprPtr nexpr(new NegateExpr(m_expr));
	nexpr->setType(m_vtype);
	return nexpr;
}

ExpressionPtr TransExpr::clone() {
	ExpressionPtr texpr(new TransExpr(m_expr, m_isComplexConj));
	texpr->setType(m_vtype);
	return texpr;
}

void StmtList::insertAt(int index, StmtPtr stmt) {
	StmtPtrVector newlist(m_list.size() + 1);
	for (int i = 0; i < index; i++)
		newlist[i] = m_list[i];
	newlist[index] = stmt;
	for (int i = index; i < m_list.size(); i++)
		newlist[i + 1] = m_list[i];
	m_list = newlist;
}

VFunction::VFunction(std::string name, StmtListPtr body, SymTable *table,
		std::vector<Arg> args, shared_ptr<FuncType> fntype) :
		m_name(name), m_symtable(table), m_body(body), m_args(args), m_doCleanup(
				true) {
	this->m_vtype = fntype;
}

NegateExpr::NegateExpr(ExpressionPtr expr) {
	m_expr = expr;
	setType(expr->getType());
}

int VFunction::createTempVar(shared_ptr<VType> vtype) {
	vector<int> ids = m_symtable->getAllIds();
	int maxid = 0;
	for (int i = 0; i < ids.size(); i++) {
		if (ids[i] > maxid)
			maxid = ids[i];
	}
	maxid++;
	std::stringstream namestream;
	namestream << "rahul_temp" << maxid;
	std::string name = namestream.str();
	cout << "createTempVar: Created temporary variable " << name << endl;
	m_symtable->addEntry(maxid, name, vtype);
	return maxid;
}

std::vector<shared_ptr<Node> > AssignStmt::getChildren() const {
	vector<shared_ptr<Node> > children;
	children.insert(children.end(), m_lhs.begin(), m_lhs.end());
	children.push_back(m_rhs);
	return children;
}

void BinaryExpr::getChildren(vector<ExpressionPtr>& children) const {
	children.push_back(m_lhs);
	children.push_back(m_rhs);
}

/*ConstExpr<int> *ConstExprBuilder::getIntConstExpr(int val){
 ConstExpr<int> *c = new ConstExpr<int>(val);
 c->m_vtype = Int32Type::getInstance();
 return c;
 }

 ConstExpr<float> *ConstExprBuilder::getFloatConstExpr(float val){
 ConstExpr<float> *c = new ConstExpr<float>(val);
 c->m_vtype = Float32Type::getInstance();
 return c;
 }

 ConstExpr<double> *ConstExprBuilder::getDoubleConstExpr(double val){
 ConstExpr<double> *c = new ConstExpr<double>(val);
 c->m_vtype = Float64Type::getInstance();
 return c;
 }*/

ConstExprPtr ConstExprBuilder::getIntConstExpr(int val) {
	int *ptr = new int;
	*ptr = val;
	ConstExprPtr c(new ConstExpr(ptr));
	ScalarTypePtr newptr(new ScalarType(ScalarType::SCALAR_INT, 32));
	c->setType(newptr);
	return c;
}

ConstExprPtr ConstExprBuilder::getLongConstExpr(long int val) {
	long int *ptr = new long int;
	*ptr = val;
	ConstExprPtr c(new ConstExpr(ptr));
	ScalarTypePtr newptr(new ScalarType(ScalarType::SCALAR_INT, 64));
	c->setType(newptr);
	return c;
}

ConstExprPtr ConstExprBuilder::getFloatConstExpr(float val) {
	float *ptr = new float;
	*ptr = val;
	ConstExprPtr c(new ConstExpr(ptr));
	ScalarTypePtr newptr(new ScalarType(ScalarType::SCALAR_FLOAT, 32));
	c->setType(newptr);
	return c;
}

DomainExpr::DomainExpr(int ndims) :
		m_ndims(ndims) {
	m_exprs.resize(3 * ndims);
}

ConstExprPtr ConstExprBuilder::getDoubleConstExpr(double val) {
	double *ptr = new double;
	*ptr = val;
	ConstExprPtr c(new ConstExpr(ptr));
	ScalarTypePtr newptr(new ScalarType(ScalarType::SCALAR_FLOAT, 64));
	c->setType(newptr);
	return c;
}

void AllocExpr::getChildren(ExpressionPtrVector &children) const {
	children.insert(children.end(), m_exprs.begin(), m_exprs.end());
}

void IndexExpr::getChildren(vector<ExpressionPtr>& children) const {
	children.push_back(m_name);
	children.insert(children.end(), m_indices.begin(), m_indices.end());
}

void DomainExpr::getChildren(std::vector<ExpressionPtr> &children) const {
	for (int i = 0; i < 3 * m_ndims; i++) {
		children.push_back(m_exprs[i]);
	}
}

void FuncallExpr::getChildren(ExpressionPtrVector &children) const {
	children.insert(children.end(), m_args.begin(), m_args.end());
}

void DimExpr::getChildren(std::vector<ExpressionPtr> &children) const {
	children.push_back(m_array);
	children.push_back(m_expr);
}

void DimvecExpr::getChildren(std::vector<ExpressionPtr> &children) const {
	children.push_back(m_array);
}

void Expression::getChildrenDeep(vector<ExpressionPtr>& children) const {
	vector<ExpressionPtr> shallow;
	getChildren(shallow);
	for (size_t i = 0; i < shallow.size(); i++) {
		children.push_back(shallow[i]);
	}
	for (size_t i = 0; i < shallow.size(); i++) {
		shallow[i]->getChildrenDeep(children);
	}
}

void SymTable::addEntry(int id, const std::string& name) {
	m_namemap[id] = name;
}

void SymTable::addEntry(int id, const std::string& name,
		shared_ptr<VType> vtype) {
	addEntry(id, name);
	m_typemap[id] = vtype;
}

std::vector<int> SymTable::getAllIds() {
	std::vector<int> ids;
	for (std::map<int, std::string>::const_iterator it = m_namemap.begin();
			it != m_namemap.end(); it++) {
		int id = (*it).first;
		ids.push_back(id);
	}
	return ids;
}

shared_ptr<VType> SymTable::getType(int id) {
	if (m_typemap.find(id) == m_typemap.end())
		std::cout << "Warning: Did not find ID " << id << " in typemap"
				<< std::endl;
	return m_typemap[id];
}

std::string VFunction::writeToXml() {
	std::string s;
	return s;
}

VFunction *VModule::getFunction(std::string s) {
	for (int i = 0; i < m_funcs.size(); i++) {
		VFunction *temp = m_funcs[i];
		std::cout << temp->getName() << std::endl;
		if (s.compare(temp->getName()) == 0)
			return temp;
	}
	return NULL;
}
#ifndef VR_MINIMAL
void VContext::addModule(VModule *m) {
	//first check if it is already present
	for(size_t i=0;i<m_modules.size();i++) {
		if(m_modules[i]==m) return;
	}
	m_modules.push_back(m);
	m->setContext(this);
}

void VContext::removeModule(VModule *m) {
	size_t index = 0;
	bool found = false;
	for(size_t i=0;i<m_modules.size();i++) {
		if(m==m_modules[i]) {
			index = i;
			found = true;
		}
	}
	if(!found) return;
	vector<VModule*> modules;
	for(size_t i=0;i<m_modules.size();i++) {
		if(i!=index) modules.push_back(m_modules[i]);
	}
	m_modules = modules;
	m->setContext(NULL);
}

//The default are suitable for NumPy on a x86-64 machine, tested on Ubuntu 12.04
VContext::VContext():m_enableGPU(false),m_enableParfor(true),m_context(new llvm::LLVMContext),m_modules(),
m_stridePtrOffset(40),m_dataPtrOffset(16),m_strideIntSize(8),m_dimsIntSize(8),m_dimsPtrOffset(32) {
	GRuntime::initialize();
	GRuntime::setContext(this);
}

VContext::~VContext() {
	std::cout<<"deleting context"<<std::endl;
	for(size_t i=0;i<m_modules.size();i++) {
		VModule *tmp = m_modules[i];
		tmp->setContext(NULL);
		delete tmp;
	}
	delete m_context;
	GRuntime::shutdown();
	std::cout<<"Shutdown GRuntime"<<std::endl;
}
#endif

VModule::~VModule() {
	std::cout << "Deleting module" << std::endl;
#ifndef VR_MINIMAL
	if(m_ctx!=NULL) m_ctx->removeModule(this);
	delete m_compiler;
#endif
	std::cout << "deleted module" << std::endl;
}

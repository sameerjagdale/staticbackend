/*
 * codegen.cpp
 *
 *  Created on: 2013-03-15
 *      Author: Sameer Jagdale
 *
 */
///Contains Methods which generate code C++ from  the VRIR
#include <codegen.hpp>

using namespace VRaptor;
using namespace std;

Context::Context() {
	arrayFlag = false;
}

bool Context::isArray() {
	return arrayFlag;
}

void Context::resetArrayFlag() {
	arrayFlag = false;
}

void Context::setArrayFlag() {
	arrayFlag = true;
}
vector<string> Context::getAllStmt() {
	return list;
}

void Context::addStmt(const string stmt) {
	list.push_back(stmt);
}

Context VCompiler::scalarTypeCodeGen(ScalarType *vtype) {
	Context cntxt;

	switch (vtype->getScalarTag()) {
	case ScalarType::SCALAR_INT:
		cntxt.addStmt("int");
		break;
	case ScalarType::SCALAR_FLOAT:

		cntxt.addStmt("double");
		break;
	case ScalarType::SCALAR_BOOL:
		cntxt.addStmt("bool");
		break;
	default:
		cntxt.addStmt("void");
	}
	return cntxt;
}
Context VCompiler::arrayTypeCodeGen(ArrayType* type, SymTable *symTable) {
	Context cntxt;

	/*Context tempCntxt = scalarTypeCodeGen(
	 (ScalarType*) type->getElementType().get());
	 if (tempCntxt.getAllStmt().size() > 0) {

	 cntxt.addStmt(tempCntxt.getAllStmt()[0] + "*");
	 }*/
	cntxt.addStmt("void*");
	return cntxt;
}
Context VCompiler::moduleCodeGen(VModule *vm) {
	Context cntxt;
	cntxt.addStmt("#include\"math.h\" \n");
	cntxt.addStmt("#include\"matrixOps.hpp\"\n");
	cntxt.addStmt("#include\"sse.hpp\"\n");
	vector<VFunction*> funcList = vm->m_funcs;
	for (int i = 0; i < funcList.size(); i++) {

		Context tempCntxt = funcCodeGen(funcList[i]);
		for (int j = 0; j < tempCntxt.getAllStmt().size(); j++) {
			cntxt.addStmt(tempCntxt.getAllStmt()[j]);
		}
	}

	return cntxt;
}
Context VCompiler::vTypeCodeGen(VType* vType, SymTable *symTable) {
	Context cntxt;

	switch (vType->getBasicType()) {
	case VType::SCALAR_TYPE:
		cntxt = scalarTypeCodeGen((ScalarType*) vType);
		break;
	case VType::DOMAIN_TYPE:
		cout << "cannot generate code for domain type" << endl;
		break;
	case VType::ARRAY_TYPE:

		cntxt = arrayTypeCodeGen((ArrayType*) vType, symTable);

		break;
	case VType::UNIT_TYPE:
		cntxt.addStmt("void");

		break;
	default:
		cntxt.addStmt("void");
		break;
	}
	//return "";
	return cntxt;
}

Context VCompiler::funcCodeGen(VFunction *func) {
	Context cntxt;
	Context argCntxt;
	FuncType *funcType = (FuncType*) func->getType().get();
	VType* returnType = funcType->getReturnType(0).get();
	string ret;
	Context retCntxt = vTypeCodeGen(returnType, func->getSymTable());
	//function signature
	if (retCntxt.getAllStmt().size() > 0) {
		ret = retCntxt.getAllStmt()[0];
	} else {
		ret = "void";
	}

	string str(ret + " " + func->getName() + "(");
	int id;
	string argName, argType;
	if (func->m_args.size() > 0) {
		id = func->m_args[0].m_id;
		argName = func->getSymTable()->getName(id);

		argCntxt = vTypeCodeGen(func->getSymTable()->getType(id).get(),
				func->getSymTable());

		if (argCntxt.getAllStmt().size() <= 0) {

			cout << "error while fetching arguments " << endl;
			return cntxt;
		}
		argType = argCntxt.getAllStmt()[0];
		str += argType + " " + argName;
		//if argument is an arraycntxt.addStmt(tempCntxt.getAllStmt()[0] + "*");

	}

	for (int i = 1; i < func->m_args.size(); i++) {
		id = func->m_args[i].m_id;
		argName = func->getSymTable()->getName(id);
		argCntxt = vTypeCodeGen(func->getSymTable()->getType(id).get(),
				func->getSymTable());
		argType = argCntxt.getAllStmt()[0];
		str += "," + argType + " " + argName;

	}
	str += ")\n{\n";
	cntxt.addStmt(str);
	//variable declarations
	SymTable *symTable = func->getSymTable();
	vector<int> idVec = symTable->getAllIds();
	for (int i = 0; i < idVec.size(); i++) {
		int j;
		for (j = 0; j < func->m_args.size(); j++) {
			if (idVec[i] == func->m_args[j].m_id) {
				break;
			}

		}
		if (j == func->m_args.size()) {
			VType* type = symTable->getType(i).get();
			Context varCntxt = vTypeCodeGen(type, func->getSymTable());
			string varType = varCntxt.getAllStmt()[0];
			string varName = symTable->getName(i);
			cntxt.addStmt(varType + " " + varName + ";\n");
		}
	}
	//array data
	ostringstream convert;

	for (int i = 0; i < idVec.size(); i++) {
		VType *vtype = symTable->getType(idVec[i]).get();
		if (vtype->getBasicType() == VType::ARRAY_TYPE) {
			ArrayType *array = (ArrayType*) vtype;
			Context tempCntxt = scalarTypeCodeGen(
					array->getElementType().get());

			string name = symTable->getName(idVec[i]);
			convert << DATA_OFFSET;

			cntxt.addStmt(
					tempCntxt.getAllStmt()[0] + " *" + name + "_data= (*"
							+ tempCntxt.getAllStmt()[0] + "**) ((char*)" + name
							+ "+" + convert.str() + ")) ;\n");
			convert.str("");
			convert << DIM_OFFSET;
			cntxt.addStmt(
					"long *" + symTable->getName(idVec[i])
							+ "_dim = (*(long**) ((char*)" + name + "+"
							+ convert.str() + ")) ;\n");
			convert.str("");
		}
	}
	//function body
	StmtListPtr listPtr = func->getBody();
	StmtList *stmtList = listPtr.get();

	for (int i = 0; i < stmtList->getNumChildren(); i++) {

		StmtPtr stmtPtr = stmtList->getChild(i);
		Statement *stmt = stmtPtr.get();
		Context tempCntxt = stmtCodeGen(stmt, func->getSymTable());
		vector<string> tempVector = tempCntxt.getAllStmt();
		for (int j = 0; j < tempVector.size(); j++) {
			cntxt.addStmt(tempVector[j]);
		}
	}

	cntxt.addStmt("}\n\n");

	return cntxt;
}

Context VCompiler::stmtCodeGen(Statement *stmt, SymTable *symTable) {
	Context cntxt;
	cntxt = stmtTypeCodeGen(stmt, symTable);

	return cntxt;
}

Context VCompiler::pForStmtCodeGen(PforStmt *stmt, SymTable *symTable) {
	Context cntxt;
	string ompStr;

	cout << getOpenmpFlag() << endl;
	if (getOpenmpFlag()) {

		vector<int> privateVec = stmt->getPrivateVars();

		ompStr = "#pragma omp parallel for";
		if (privateVec.size() > 0) {

			string priVar = symTable->getName(privateVec[0]);
			ompStr += " private(" + priVar;
			if (symTable->getType(privateVec[0]).get()->getBasicType()
					== VType::ARRAY_TYPE) {
				ompStr += "_data";
			}

			for (int i = 1; i < privateVec.size(); i++) {
				cout << "ompStr   " << ompStr << endl;
				ompStr += "," + symTable->getName(privateVec[i]);
				if (symTable->getType(privateVec[i]).get()->getBasicType()
						== VType::ARRAY_TYPE) {
					ompStr += "_data";
				}
			}
			ompStr += ")";
		}
		ompStr += "\n";
		cntxt.addStmt(ompStr);
	}
	StmtPtr sPtr = stmt->getBody();
	Statement * bodyStmt = sPtr.get();
	ExpressionPtr domainPtr = stmt->getDomain();
	Context domainCntxt = exprTypeCodeGen(domainPtr.get(), symTable);

	string initStmt, compStmt, iterStmt;
	vector<string> domainVec = domainCntxt.getAllStmt();
	vector<int> iterVar = stmt->getIterVars();
	string var;
	int count = iterVar.size();

	for (int i = 0; i < iterVar.size(); i++) {
		var = symTable->getName(iterVar[i]);
		initStmt = var + "=" + domainVec[i];
		compStmt = var + "<" + domainVec[i + count];
		iterStmt = var + "=" + var + "+" + domainVec[i + 2 * count];
		cntxt.addStmt(
				"for(" + initStmt + ";" + compStmt + ";" + iterStmt + ")\n");
		cntxt.addStmt("{\n");

	}
	Context bodyCntxt = stmtTypeCodeGen(bodyStmt, symTable);

	vector<string> bodyVec = bodyCntxt.getAllStmt();
	for (int i = 0; i < bodyVec.size(); i++) {
		cntxt.addStmt(bodyVec[i]);

	}
	for (int i = 0; i < iterVar.size(); i++) {
		cntxt.addStmt("}\n");
	}
	return cntxt;
}
Context VCompiler::returnStmtCodeGen(ReturnStmt *stmt, SymTable *symTable) {
	Context cntxt;

	vector<int> ridVec = stmt->getRids();
	string retStr = "return ";
	if (ridVec.size() > 0) {
		retStr += symTable->getName(ridVec[0]);
		/*for (int i = 1; i < ridVec.size(); i++) {
		 retStr += "," + ridVec[i];
		 }*/
		if (ridVec.size() > 1) {
			cout << "compiler does not handle multiple returns" << endl;
		}
	}
	cntxt.addStmt(retStr + ";\n");
	return cntxt;
}
Context VCompiler::breakStmtCodeGen(BreakStmt *stmt) {
	Context cntxt;
	cntxt.addStmt("break;\n");
	return cntxt;
}
Context VCompiler::continueStmtCodeGen(ContinueStmt *stmt) {
	Context cntxt;
	cntxt.addStmt("continue;\n");
	return cntxt;
}

Context VCompiler::ifStmtCodeGen(IfStmt *stmt, SymTable *symTable) {
	Context cntxt;
	// condition
	Expression *expr = stmt->getCond().get();
	Context condCntxt = exprTypeCodeGen(expr, symTable);
	cntxt.addStmt("if(" + condCntxt.getAllStmt()[0] + ")\n");
	cntxt.addStmt("{\n");
	// if block
	Statement *ifBlockStmt = stmt->getIfBranch().get();
	Context ifCntxt = stmtTypeCodeGen(ifBlockStmt, symTable);
	for (int i = 0; i < ifCntxt.getAllStmt().size(); i++) {
		cntxt.addStmt(ifCntxt.getAllStmt()[i]);
	}
	cntxt.addStmt("}\n");
	//else block
	if (stmt->hasElseBranch()) {
		cntxt.addStmt("else\n");
		cntxt.addStmt("{\n");

		Statement *elseBlockStmt = stmt->getElseBranch().get();
		Context elseCntxt = stmtTypeCodeGen(elseBlockStmt, symTable);
		for (int i = 0; i < elseCntxt.getAllStmt().size(); i++) {
			cntxt.addStmt(elseCntxt.getAllStmt()[i]);
		}
		cntxt.addStmt("}\n");
	}
	return cntxt;
}
Context VCompiler::whileStmtCodeGen(WhileStmt *stmt, SymTable *symTable) {
	Context cntxt;
	//condition
	Expression *condExpr = stmt->getCond().get();
	Context condCntxt = exprTypeCodeGen(condExpr, symTable);
	cntxt.addStmt("while(" + condCntxt.getAllStmt()[0] + ")\n");
	cntxt.addStmt("{\n");
	//body
	Statement* bodyStmt = stmt->getBody().get();
	Context bodyCntxt = stmtTypeCodeGen(bodyStmt, symTable);
	for (int i = 0; i < bodyCntxt.getAllStmt().size(); i++) {
		cntxt.addStmt(bodyCntxt.getAllStmt()[i]);
	}
	cntxt.addStmt("}\n");
	return cntxt;
}
Context VCompiler::stmtTypeCodeGen(Statement *stmt, SymTable *symTable) {
	Context cntxt;

	switch (stmt->getStmtType()) {
	case 0: //Assignment Statement
		cntxt = assignStmtCodeGen((AssignStmt*) stmt, symTable);
		break;
	case 1: // IF statement
		cntxt = ifStmtCodeGen((IfStmt*) stmt, symTable);
		break;
	case 2: // while statement
		cntxt = whileStmtCodeGen((WhileStmt*) stmt, symTable);
		break;
	case 3: // for statement
		cntxt = forStmtCodeGen((ForStmt*) stmt, symTable);
		break;
	case 4: //parallel for
		cntxt = pForStmtCodeGen((PforStmt*) stmt, symTable);
		break;
	case 5: // list statement

		cntxt = stmtListCodeGen((StmtList*) stmt, symTable);
		break;
	case 6: //break statement
		cntxt = breakStmtCodeGen((BreakStmt*) stmt);
		break;
	case 7: // continue statement
		cntxt = continueStmtCodeGen((ContinueStmt*) stmt);
		break;
	case 8: // bound check statement
		break;
	case 9: //return
		cntxt = returnStmtCodeGen((ReturnStmt*) stmt, symTable);
		break;
	case 10: // statement refincr

		break;
	case 11: // refDecr
		break;
	case 12: // libcall
		break;
	default:
		break;
	}
	return cntxt;
}
Context VCompiler::mMultExprCodeGen(MmultExpr* expr, SymTable *symTable) {
	Context cntxt;

	if (expr->getLhs().get()->getExprType() != Expression::NAME_EXPR) {
		cout << "LHS not name expression. Cannot generate code" << endl;
		return cntxt;
	}
	if (expr->getRhs().get()->getExprType() != Expression::NAME_EXPR) {
		cout << "RHS not name expression. Cannot generate code" << endl;
		return cntxt;
	}
	Context lcntxt = exprTypeCodeGen(expr->getLhs().get(), symTable);
	Context rcntxt = exprTypeCodeGen(expr->getRhs().get(), symTable);
	string lstr = lcntxt.getAllStmt()[0];
	string rstr = rcntxt.getAllStmt()[0];
	NameExpr *lexpr = (NameExpr*) expr->getLhs().get();
	VType *ltype = symTable->getType(lexpr->getId()).get();
	if (ltype->getBasicType() != VType::ARRAY_TYPE) {
		cout << "variable is not an array. cannot generate multiplication code"
				<< endl;
		return cntxt;
	}
	ArrayType *larray = (ArrayType*) ltype;
	Context ltypeCntxt = scalarTypeCodeGen(larray->getElementType().get());
	NameExpr *rexpr = (NameExpr*) expr->getRhs().get();
	VType *rtype = symTable->getType(rexpr->getId()).get();
	if (rtype->getBasicType() != VType::ARRAY_TYPE) {
		cout << "variable is not an array. cannot generate multiplication code"
				<< endl;
		return cntxt;
	}
	ArrayType *rarray = (ArrayType*) rtype;
	Context rtypeCntxt = scalarTypeCodeGen(rarray->getElementType().get());
	string ltypeStr;
	if (ltypeCntxt.getAllStmt().size() > 0) {
		ltypeStr = ltypeCntxt.getAllStmt()[0];
	}
	string rtypeStr;
	if (rtypeCntxt.getAllStmt().size() > 0) {
		rtypeStr = ltypeCntxt.getAllStmt()[0];
	}
	if (rtypeStr.compare(ltypeStr) != 0) {
		cout << "arrays not of the same type. expression cannot be compiled"
				<< endl;
		return cntxt;
	}
	string lname = symTable->getName(lexpr->getId());
	string rname = symTable->getName(rexpr->getId());
	cntxt.addStmt(
			"matrixMult_" + rtypeStr + "(" + lname + +"_data" + "," + rname
					+ +"_data" + "," + lname + "_dim[0]" + lname + "_dim[1]"
					+ rname + "_dim[1]" + ")");
	return cntxt;
}
Context VCompiler::exprTypeCodeGen(Expression* expr, SymTable *symTable) {
	Context cntxt;

	switch (expr->getExprType()) {
	case 0: // CONST_EXPR
		cntxt = constExprCodeGen((ConstExpr*) expr, symTable);
		break;
	case 1: //Name expression
		cntxt = nameExprCodeGen((NameExpr*) expr, symTable);
		break;
	case 2: //Plus expression
		//cout << "plus expression reached" << endl;
		cntxt = plusExprCodeGen((PlusExpr*) expr, symTable);
		break;
	case 3: //minus expression
		cntxt = minusExprCodeGen((MinusExpr*) expr, symTable);
		break;
	case 4: //mult expr
		cntxt = multExprCodeGen((MultExpr*) expr, symTable);
		break;
	case 5: // division expr
		cntxt = divExprCodeGen((DivExpr*) expr, symTable);
		break;
	case 6: //index expr
		cntxt = indexExprCodeGen((IndexExpr*) expr, symTable);
		break;
	case 7: //Negate expr
		cntxt = negateExprCodeGen((NegateExpr*) expr, symTable);
		break;
	case 8: // matrix multiplication
		cntxt = mMultExprCodeGen((MmultExpr*) expr, symTable);
		break;
	case 9: //Transpose
		break;
	case 10:  // greater than
		cntxt = gtExprCodeGen((GtExpr*) expr, symTable);
		break;
	case 11: //greater than equal to
		cntxt = geqExprCodeGen((GeqExpr*) expr, symTable);
		break;
	case 12: // less than
		cntxt = ltExprCodeGen((LtExpr*) expr, symTable);
		break;
	case 13: // less than equal to
		cntxt = leqExprCodeGen((LeqExpr*) expr, symTable);
		break;
	case 14: // and expression
		cntxt = andExprCodeGen((AndExpr*) expr, symTable);
		break;
	case 15: // or expression
		cntxt = orExprCodeGen((OrExpr*) expr, symTable);
		break;
	case 16: // not expression
		cntxt = notExprCodeGen((NotExpr*) expr, symTable);
		break;
	case 17: // function call expression

		break;
	case 18: // domain expression
		cntxt = domainExprCodeGen((DomainExpr*) expr, symTable);
		break;
	case 19:  //dim expression
		break;
	case 20: // dim vector
		break;
	case 21: // library call expression
		cntxt = libCallExprCodeGen((LibCallExpr*) expr, symTable);
		break;
	case 22: // alloc expression
		break;
	case 23: // imaginary expression
		break;
	case 24: // real expressions
		break;
	case 25: // reduce expressions
		break;
	case 26: // cast expression
		break;
	default:
		break;

	}
	return cntxt;
}
Context VCompiler::libCallExprCodeGen(LibCallExpr *expr, SymTable *symTable) {
	Context cntxt;
	string funcName;
	switch (expr->getCode()) {
	case LibCallExpr::LIB_SQRT:
		funcName = "sqrt(";
		break;
	case LibCallExpr::LIB_LOG2:
		funcName = "log2(";
		break;
	case LibCallExpr::LIB_LOG10:
		funcName = "log10(";
		break;
	case LibCallExpr::LIB_LOGE:
		funcName = "log(";
		break;
	case LibCallExpr::LIB_EXPE:
		funcName = "exp(";
		break;
	case LibCallExpr::LIB_EXP10:
		funcName = "exp10(";
		break;
	case LibCallExpr::LIB_SIN:
		funcName = "sin(";
		break;
	case LibCallExpr::LIB_COS:
		funcName = "cos(";
		break;
	case LibCallExpr::LIB_TAN:
		funcName = "tan(";
		break;
	case LibCallExpr::LIB_ASIN:
		funcName = "asin(";
		break;
	case LibCallExpr::LIB_ACOS:
		funcName = "acos(";
		break;
	case LibCallExpr::LIB_ATAN:
		funcName = "atan(";
		break;
	default:
		cout << "error in library call expression \n function not found"
				<< endl;
		break;

	}
	ExpressionPtr basePtr = expr->getBaseExpr();
	Context baseCntxt = exprTypeCodeGen(basePtr.get(), symTable);
	funcName += baseCntxt.getAllStmt()[0] + ")";
	return cntxt;
}
Context VCompiler::funCallExprCodeGen(FuncallExpr *expr, SymTable *symTable) {
	Context cntxt;

	NameExprPtr namePtr = expr->getFuncName();
	Context nameCntxt = exprTypeCodeGen(namePtr.get(), symTable);
	string name = nameCntxt.getAllStmt()[0];
	name = name + "(";
	ExpressionPtrVector vecPtr = expr->getArgs();
	Context tempCntxt = exprTypeCodeGen(vecPtr[0].get(), symTable);
	if (tempCntxt.getAllStmt().size() > 0) {
		name += tempCntxt.getAllStmt()[0];
	}
	for (int i = 1; vecPtr.size(); i++) {
		tempCntxt = exprTypeCodeGen(vecPtr[i].get(), symTable);
		if (tempCntxt.getAllStmt().size() > 0) {
			name += ("," + tempCntxt.getAllStmt()[0]);
		}
	}
	name += ")";
	cntxt.addStmt(name);
	return cntxt;
}
Context VCompiler::notExprCodeGen(NotExpr *expr, SymTable *symTable) {
	Context cntxt;
	Context baseCntxt = exprTypeCodeGen(expr->getBaseExpr().get(), symTable);
	string baseStr = baseCntxt.getAllStmt()[0];
	cntxt.addStmt("(!" + baseStr + "");
	return cntxt;
}
Context VCompiler::negateExprCodeGen(NegateExpr *expr, SymTable *symTable) {
	Context cntxt;
	Context baseCntxt = exprTypeCodeGen(expr->getBaseExpr().get(), symTable);
	string baseStr = baseCntxt.getAllStmt()[0];
	cntxt.addStmt("(-" + baseStr + "");
	return cntxt;
}
Context VCompiler::constExprCodeGen(ConstExpr *expr, SymTable *symTable) {
	Context cntxt;
	ostringstream convert;
	ScalarType* sc = (ScalarType*) expr->getType().get();
	convert.str("");
	int intVal;
	double floatVal;
	switch (sc->getScalarTag()) {
	case ScalarType::SCALAR_INT:
		intVal = expr->getIntVal();
		convert << intVal;
		break;
	case ScalarType::SCALAR_FLOAT:

		floatVal = expr->getDoubleVal();

		convert << floatVal;
		break;
	}

	cntxt.addStmt(convert.str());
	return cntxt;
}
Context VCompiler::domainExprCodeGen(DomainExpr *expr, SymTable *symTable) {
	Context cntxt, strtCntxt, endCntxt, stepCntxt;
	vector<string> vec;
	for (int j = 0; j < expr->getNdims(); j++) {
		Context tempCntxt;
		ExpressionPtr startPtr = expr->getStartExpr(j);
		Expression *startExpr = startPtr.get();

		tempCntxt = exprTypeCodeGen(startExpr, symTable);
		vec = tempCntxt.getAllStmt();

		for (int i = 0; i < vec.size(); i++) {
			strtCntxt.addStmt(vec[i]);
		}

		ExpressionPtr endPtr = expr->getStopExpr(j);
		Expression* endExpr = endPtr.get();

		tempCntxt = exprTypeCodeGen(endExpr, symTable);
		vec = tempCntxt.getAllStmt();
		for (int i = 0; i < vec.size(); i++) {
			endCntxt.addStmt(vec[i]);
		}

		ExpressionPtr stepPtr = expr->getStepExpr(j);
		Expression * stepExpr = stepPtr.get();

		tempCntxt = exprTypeCodeGen(stepExpr, symTable);
		vec = tempCntxt.getAllStmt();
		for (int i = 0; i < vec.size(); i++) {
			stepCntxt.addStmt(vec[i]);
		}
	}
	vec = strtCntxt.getAllStmt();

	for (int i = 0; i < vec.size(); i++) {
		cntxt.addStmt(vec[i]);
	}

	vec = endCntxt.getAllStmt();
	for (int i = 0; i < vec.size(); i++) {
		cntxt.addStmt(vec[i]);
	}

	vec = stepCntxt.getAllStmt();
	for (int i = 0; i < vec.size(); i++) {
		cntxt.addStmt(vec[i]);
	}
	return cntxt;
}
Context VCompiler::nameExprCodeGen(NameExpr *expr, SymTable *symTable) {
	Context cntxt;

//	VTypePtr vptr = expr->getType();
//	VType *vtype = vptr.get();

	string name = symTable->getName(expr->getId());
	VType* vtype = symTable->getType(expr->getId()).get();

	cntxt.addStmt(name);
	return cntxt;
}
Context VCompiler::binaryExprCodeGen(BinaryExpr*expr, SymTable *symTable) {
	Context cntxt;
	ExpressionPtr lPtr = expr->getLhs();
	ExpressionPtr rPtr = expr->getRhs();

	Context lCntxt = exprTypeCodeGen(lPtr.get(), symTable);

	Context rCntxt = exprTypeCodeGen(rPtr.get(), symTable);
	vector<string> lVec = lCntxt.getAllStmt();
	vector<string> rVec = rCntxt.getAllStmt();
	string lStr, rStr;
	if (lVec.size() > 0) {
		lStr = lVec[0];
		for (int i = 1; i < lVec.size(); i++) {
			lStr += " " + lVec[i];
		}

	}
	if (rVec.size()) {
		rStr = rVec[0];
		for (int i = 1; i < rVec.size(); i++) {
			rStr += " " + rVec[i];
		}

	}

	lStr = "(" + lStr;
	rStr += ")";
	cntxt.addStmt(lStr);
	cntxt.addStmt(rStr);
	return cntxt;
}
Context VCompiler::leqExprCodeGen(LeqExpr*expr, SymTable *symTable) {
	Context cntxt;
	Context tempCntxt = binaryExprCodeGen(expr, symTable);
	string lStr = tempCntxt.getAllStmt()[0];
	string rStr = tempCntxt.getAllStmt()[1];
	cntxt.addStmt(lStr + " <= " + rStr);
	return cntxt;
}
Context VCompiler::andExprCodeGen(AndExpr*expr, SymTable *symTable) {
	Context cntxt;
	Context tempCntxt = binaryExprCodeGen(expr, symTable);
	string lStr = tempCntxt.getAllStmt()[0];
	string rStr = tempCntxt.getAllStmt()[1];
	cntxt.addStmt(lStr + " && " + rStr);
	return cntxt;
}
Context VCompiler::orExprCodeGen(OrExpr *expr, SymTable *symTable) {
	Context cntxt;
	Context tempCntxt = binaryExprCodeGen(expr, symTable);
	string lStr = tempCntxt.getAllStmt()[0];
	string rStr = tempCntxt.getAllStmt()[1];
	cntxt.addStmt(lStr + " || " + rStr);
	return cntxt;
}
Context VCompiler::minusExprCodeGen(MinusExpr *expr, SymTable *symTable) {
	Context cntxt;
	Context tempCntxt = binaryExprCodeGen(expr, symTable);
	string lStr = tempCntxt.getAllStmt()[0];
	string rStr = tempCntxt.getAllStmt()[1];
	cntxt.addStmt(lStr + " - " + rStr);
	return cntxt;
}
Context VCompiler::ltExprCodeGen(LtExpr*expr, SymTable *symTable) {
	Context cntxt;
	Context tempCntxt = binaryExprCodeGen(expr, symTable);
	string lStr = tempCntxt.getAllStmt()[0];
	string rStr = tempCntxt.getAllStmt()[1];
	cntxt.addStmt(lStr + " < " + rStr);
	return cntxt;
}
Context VCompiler::gtExprCodeGen(GtExpr* expr, SymTable *symTable) {
	Context cntxt;
	Context tempCntxt = binaryExprCodeGen(expr, symTable);
	string lStr = tempCntxt.getAllStmt()[0];
	string rStr = tempCntxt.getAllStmt()[1];
	cntxt.addStmt(lStr + " > " + rStr);
	return cntxt;
}
Context VCompiler::geqExprCodeGen(GeqExpr* expr, SymTable *symTable) {
	Context cntxt;
	Context tempCntxt = binaryExprCodeGen(expr, symTable);
	string lStr = tempCntxt.getAllStmt()[0];
	string rStr = tempCntxt.getAllStmt()[1];
	cntxt.addStmt(lStr + " >= " + rStr);
	return cntxt;
}

Context VCompiler::multExprCodeGen(MultExpr*expr, SymTable *symTable) {
	Context cntxt;
	Context tempCntxt = binaryExprCodeGen(expr, symTable);
	string lStr = tempCntxt.getAllStmt()[0];
	string rStr = tempCntxt.getAllStmt()[1];
	cntxt.addStmt(lStr + " * " + rStr);
	return cntxt;
}
Context VCompiler::divExprCodeGen(DivExpr *expr, SymTable *symTable) {
	Context cntxt;
	Context tempCntxt = binaryExprCodeGen(expr, symTable);
	string lStr = tempCntxt.getAllStmt()[0];
	string rStr = tempCntxt.getAllStmt()[1];
	cntxt.addStmt(lStr + " / " + rStr);
	return cntxt;
}
Context VCompiler::plusExprCodeGen(PlusExpr* expr, SymTable *symTable) {
	Context cntxt;

	if (expr->getLhs().get()->getExprType() == Expression::NAME_EXPR
			&& expr->getRhs().get()->getExprType() == Expression::NAME_EXPR) {
		NameExpr * lnameExpr = (NameExpr*) expr->getLhs().get();
		NameExpr * rnameExpr = (NameExpr*) expr->getRhs().get();
		if (symTable->getType(rnameExpr->getId()).get()->getBasicType()
				== VType::ARRAY_TYPE
				&& symTable->getType(lnameExpr->getId()).get()->getBasicType()
						== VType::ARRAY_TYPE) {
			string lnameStr = symTable->getName(lnameExpr->getId());
			string rnameStr = symTable->getName(rnameExpr->getId());
			ArrayType *larray = (ArrayType*) symTable->getType(
					lnameExpr->getId()).get();
			ArrayType *rarray = (ArrayType*) symTable->getType(
					rnameExpr->getId()).get();
			Context ltypeCntxt = scalarTypeCodeGen(
					larray->getElementType().get());
			Context rtypeCntxt = scalarTypeCodeGen(
					rarray->getElementType().get());
			string ltypeStr = ltypeCntxt.getAllStmt()[0];
			if (ltypeStr.compare(rtypeCntxt.getAllStmt()[0]) != 0) {
				cout << "types not same. cannot generate codes to add arrays"
						<< endl;
			}
			string lname = symTable->getName(lnameExpr->getId());
			string rname = symTable->getName(rnameExpr->getId());
			cntxt.addStmt(
					"matrixSum_" + ltypeStr + "(" + lname + "_data," + rname
							+ "_data," + lname + "_dim[0]" + lname
							+ "_dim[1])");
			return cntxt;
		}
	}

	Context tempCntxt = binaryExprCodeGen(expr, symTable);
	string lStr = tempCntxt.getAllStmt()[0];
	string rStr = tempCntxt.getAllStmt()[1];
	cntxt.addStmt(lStr + " + " + rStr);

	return cntxt;
}
Context VCompiler::indexExprCodeGen(IndexExpr *expr, SymTable *symTable) {
	Context cntxt;

	Expression *nameExpr = expr->getNameExpr().get();

	Context nameCntxt = exprTypeCodeGen(nameExpr, symTable);
	string name, nameData;
//if (nameCntxt.getAllStmt().size() > 0) {
	name = nameCntxt.getAllStmt()[0];
//	}
	nameData = name + "_data";
	nameData = "(*(" + nameData;

	ExpressionPtrVector indxPtrVec = expr->getIndices();
	for (int i = 0; i < indxPtrVec.size(); i++) {
		Expression *indxExpr = indxPtrVec[i].get();
		Context indxCntxt = exprTypeCodeGen(indxExpr, symTable);
		if (indxCntxt.getAllStmt().size() > 0) {
			string indxStr = indxCntxt.getAllStmt()[0];

			for (int j = i + 1; j < indxPtrVec.size(); j++) {
				string jstr;
				ostringstream convert;
				convert << j;
				jstr = convert.str();
				indxStr += "*" + name + "_dim[" + jstr + "]";

			}
			nameData += "+" + indxStr;

			//name += ("[" + indxCntxt.getAllStmt()[0] + "]");
		}
	}
	nameData += "))";
	cntxt.addStmt(nameData);

	return cntxt;

}
Context VCompiler::refOpStmtCodeGen(RefOpStmt stmt, SymTable *symTable) {
	Context cntxt;

	return cntxt;
}
Context VCompiler::forStmtCodeGen(ForStmt *stmt, SymTable *symTable) {
	Context cntxt;
	bool vectorise = true;
	VectorAnalysis analysis;

	StmtPtr sPtr = stmt->getBody();

	StmtList * bodyStmt = (StmtList*) sPtr.get();

	ExpressionPtr domainPtr = stmt->getDomain();

	Context domainCntxt = exprTypeCodeGen(domainPtr.get(), symTable);

	string initStmt, compStmt, iterStmt;
	vector<string> domainVec = domainCntxt.getAllStmt();
	vector<int> iterVar = stmt->getIterVars();
	if (iterVar.size() > 1) {
		vectorise = false;
	}
	if (getSseFlag() && vectorise) {

		analysis.analyse((StmtList*) stmt->getBody().get());
	}
	string var = symTable->getName(iterVar[0]);

//vectorisation
	if (getSseFlag() && vectorise) {
		bool flag = true;

		string size = domainVec[iterVar.size()];
		for (int i = 0; i < bodyStmt->getNumChildren(); i++) {
			if (analysis.canVectorise(i)) {

				Context tempCntxt = vectoriseStmt(
						(AssignStmt*) bodyStmt->getChild(i).get(), size,
						symTable);

				for (int i = 0; i < tempCntxt.getAllStmt().size(); i++) {
					cntxt.addStmt(tempCntxt.getAllStmt()[i]);
				}

			} else {
				flag = false;
			}
		}
		if (flag) {
			return cntxt;
		}
	}
	int count = iterVar.size();

	for (int i = 0; i < iterVar.size(); i++) {
		var = symTable->getName(iterVar[i]);
		initStmt = var + "=" + domainVec[i];
		compStmt = var + "<" + domainVec[i + count];
		iterStmt = var + "=" + var + "+" + domainVec[i + 2 * count];
		cntxt.addStmt(
				"for(" + initStmt + ";" + compStmt + ";" + iterStmt + ")\n");
		cntxt.addStmt("{\n");

	}

	Context bodyCntxt = stmtTypeCodeGen(bodyStmt, symTable);
	vector<string> bodyVec = bodyCntxt.getAllStmt();
	for (int i = 0; i < bodyVec.size(); i++) {
		cntxt.addStmt(bodyVec[i]);

	}
	for (int i = 0; i < iterVar.size(); i++) {
		cntxt.addStmt("}\n");
	}
	return cntxt;
}
Context VCompiler::assignStmtCodeGen(AssignStmt *stmt, SymTable *symTable) {
	Context cntxt;

	vector<ExpressionPtr> lExprVec = stmt->getLhs();
	string lStr;

	for (int i = 0; i < lExprVec.size(); i++) {
		ExpressionPtr lExprPtr = lExprVec[i];
		Expression* lExpr = lExprPtr.get();

		Context lCntxt = exprTypeCodeGen(lExpr, symTable);
		vector<string> lVec = lCntxt.getAllStmt();
		for (int j = 0; j < lVec.size(); j++) {
			lStr += lVec[i];
		}
	}
	ExpressionPtr rExprPtr = stmt->getRhs();
	Expression* rExpr = rExprPtr.get();
	Context rCntxt = exprTypeCodeGen(rExpr, symTable);
	vector<string> rExprVec = rCntxt.getAllStmt();
	string rStr;
	for (int i = 0; i < rExprVec.size(); i++) {
		rStr += rExprVec[i];
	}

	cntxt.addStmt(lStr + " = " + rStr + ";\n");

	return cntxt;
}
Context VCompiler::stmtListCodeGen(StmtList *stmt, SymTable *symTable) {
	Context cntxt;

	for (int i = 0; i < stmt->getNumChildren(); i++) {

		Statement *childStmt = stmt->getChild(i).get();
		Context tempCntxt = stmtTypeCodeGen(childStmt, symTable);
		for (int i = 0; i < tempCntxt.getAllStmt().size(); i++) {
			cntxt.addStmt(tempCntxt.getAllStmt()[i]);
		}
	}
	return cntxt;
}

Context VCompiler::vectoriseStmt(AssignStmt *stmt, string size,
		SymTable *symTable) {
	Context cntxt;
	IndexExpr *lhs = (IndexExpr*) stmt->getLhs()[0].get();
	string l = symTable->getName(lhs->getArrayId());
	BinaryExpr* rhsExpr = (BinaryExpr*) stmt->getRhs().get();
	IndexExpr *lExpr = (IndexExpr*) rhsExpr->getLhs().get();
	IndexExpr *rExpr = (IndexExpr*) rhsExpr->getRhs().get();
	string rl = symTable->getName(lExpr->getArrayId());
	string rr = symTable->getName(rExpr->getArrayId());
	//ostringstream ss;
	//ss << size;

	switch (rhsExpr->getExprType()) {
	case Expression::PLUS_EXPR:
		cntxt.addStmt(
				"sse_add(" + rl + "_data," + rr + "_data," + l + "_data," + size
						+ ");\n");
		break;
	case Expression::MINUS_EXPR:
		cntxt.addStmt(
				"sse_sub(" + rl + "," + rr + "," + l + "," + size + ");\n");
		break;
	case Expression::MULT_EXPR:
		cntxt.addStmt(
				"sse_mul(" + rl + "," + rr + "," + l + "," + size + ");\n");
		break;
	case Expression::DIV_EXPR:
		cntxt.addStmt(
				"sse_div(" + rl + "," + rr + "," + l + "," + size + ");\n");
		break;
	}
	return cntxt;
}
void VCompiler::setOpenMpFlag(bool val) {
	enableOpenMP = val;
}
void VCompiler::setSseFlag(bool val) {
	enableSse = val;
}

bool VCompiler::getOpenmpFlag() {
	return enableOpenMP;
}

bool VCompiler::getSseFlag() {
	return enableSse;
}

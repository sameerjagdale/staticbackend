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
#include <vector>
#include <string>
#include <cstdlib>
#include <utility>
#include <iostream>
#include <sstream>
#include <Poco/DOM/Node.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/SAX/InputSource.h>
#include <Poco/DOM/DOMParser.h>
#include <Poco/AutoPtr.h>
#include <Poco/DOM/Document.h>

using namespace VRaptor;
using std::vector;
using std::pair;
using Poco::XML::NamedNodeMap;
using Poco::XML::NodeList;
using Poco::XML::XMLString;
typedef Poco::XML::Node PocoNode;
using Poco::AutoPtr;

PocoNode *getFirstChildElement(PocoNode *n) {
	if (!(n->hasChildNodes()))
		return NULL;
	PocoNode *child = n->firstChild();
	while (child->nodeType() != PocoNode::ELEMENT_NODE) {
		child = child->nextSibling();
	}
}

vector<PocoNode*> getChildrenByName(PocoNode *n, const XMLString &name) {
	vector<PocoNode*> vec;
	if (n == NULL)
		return vec;
	AutoPtr<NodeList> list = n->childNodes();
	for (int i = 0; i < list->length(); i++) {
		PocoNode *item = list->item(i);
		if (item->nodeType() == PocoNode::ELEMENT_NODE) {
			//std::cout<<"getChildrenByName: Child name "<<item->nodeName()<<std::endl;
			if (item->nodeName().compare(name) == 0) {
				vec.push_back(item);
			}
		}
	}
	return vec;
}

shared_ptr<FuncType> FuncType::readFromXml(Poco::XML::Node* node) {
	vector<VTypePtr> intypes;
	vector<VTypePtr> outtypes;
	vector<PocoNode*> innodes = getChildrenByName(
			node->getNodeByPath("intypes"), "vtype");

	for (int i = 0; i < innodes.size(); i++) {
		PocoNode *vnode = innodes[i];
		VTypePtr v = VType::readFromXml(vnode);
		intypes.push_back(v);
	}
	vector<PocoNode*> outnodes = getChildrenByName(
			node->getNodeByPath("outtypes"), "vtype");

	for (int i = 0; i < outnodes.size(); i++) {
		PocoNode *vnode = outnodes[i];
		VTypePtr v = VType::readFromXml(vnode);
		outtypes.push_back(v);
	}

	shared_ptr<FuncType> fntype(new FuncType(intypes, outtypes));
	return fntype;
}

VTypePtr VType::readFromXml(Poco::XML::Node *node) {
	AutoPtr<NamedNodeMap> attribs = node->attributes();
	XMLString name = attribs->getNamedItem("name")->nodeValue();
	if (name.compare("int32") == 0) {
		VTypePtr vtype(new ScalarType(ScalarType::SCALAR_INT, 32));
		return vtype;
	} else if (name.compare("int64") == 0) {
		VTypePtr vtype(new ScalarType(ScalarType::SCALAR_INT, 64));
		return vtype;
	} else if (name.compare("float32") == 0) {
		VTypePtr vtype(new ScalarType(ScalarType::SCALAR_FLOAT, 32));
		return vtype;
	} else if (name.compare("float64") == 0) {
		VTypePtr vtype(new ScalarType(ScalarType::SCALAR_FLOAT, 64));
		return vtype;
	} else if (name.compare("bool") == 0) {
		VTypePtr vtype(new ScalarType(ScalarType::SCALAR_BOOL, 1));
		return vtype;
	} else if (name.compare("unit") == 0) {
		return UnitType::getInstance();
	} else if (name.compare("func") == 0) {
		return FuncType::readFromXml(node);
	} else if (name.compare("array") == 0) {
		//cout<<"Reading array!"<<endl;
		int ndims = atoi(
				attribs->getNamedItem("ndims")->getNodeValue().c_str());
		VTypePtr stype = VType::readFromXml(node->getNodeByPath("vtype"));
		XMLString layout = attribs->getNamedItem("layout")->getNodeValue();
		ArrayType::ArrayLayout l;
		if (layout.compare("rowmajor") == 0) {
			l = ArrayType::ROW_MAJOR;
		} else if (layout.compare("colmajor") == 0) {
			l = ArrayType::COL_MAJOR;
		} else {
			l = ArrayType::STRIDES;
		}
		ScalarTypePtr scalarType = static_pointer_cast<ScalarType>(stype);
		shared_ptr<ArrayType> atype(new ArrayType(scalarType, ndims, l));
		return atype;
	} else if (name.compare("domain") == 0) {
		int ndims = atoi(
				attribs->getNamedItem("ndims")->getNodeValue().c_str());
		shared_ptr<DomainType> dtype(new DomainType(ndims));
		return dtype;
	} else if (name.compare("void") == 0) {
		return UnitType::getInstance();
	}

}

static pair<ExpressionPtr, ExpressionPtr> readPair(Poco::XML::Node * node) {
	ExpressionPtr e1 = Expression::readFromXml(node->getNodeByPath("lhs/expr"));
	ExpressionPtr e2 = Expression::readFromXml(node->getNodeByPath("rhs/expr"));
	return pair<ExpressionPtr, ExpressionPtr>(e1, e2);
}

shared_ptr<Expression> Expression::readFromXml(Poco::XML::Node * node) {
	AutoPtr<NamedNodeMap> attribs = node->attributes();
	XMLString name = attribs->getNamedItem("name")->getNodeValue();
	ExpressionPtr e;
	//std::cout<<"Reading expression "<<name<<std::endl;
	if (name.compare("plus") == 0) {
		pair<ExpressionPtr, ExpressionPtr> p = readPair(node);
		e.reset(new PlusExpr(p.first, p.second));
	} else if (name.compare("minus") == 0) {
		pair<ExpressionPtr, ExpressionPtr> p = readPair(node);
		e.reset(new MinusExpr(p.first, p.second));
	} else if (name.compare("mult") == 0) {
		pair<ExpressionPtr, ExpressionPtr> p = readPair(node);
		e.reset(new MultExpr(p.first, p.second));
	} else if (name.compare("div") == 0) {
		pair<ExpressionPtr, ExpressionPtr> p = readPair(node);
		e.reset(new DivExpr(p.first, p.second));
	} else if (name.compare("lt") == 0) {
		pair<ExpressionPtr, ExpressionPtr> p = readPair(node);
		e.reset(new LtExpr(p.first, p.second));
	} else if (name.compare("leq") == 0) {
		pair<ExpressionPtr, ExpressionPtr> p = readPair(node);
		e.reset(new LeqExpr(p.first, p.second));
	} else if (name.compare("gt") == 0) {
		pair<ExpressionPtr, ExpressionPtr> p = readPair(node);
		e.reset(new GtExpr(p.first, p.second));
	} else if (name.compare("geq") == 0) {
		pair<ExpressionPtr, ExpressionPtr> p = readPair(node);
		e.reset(new GeqExpr(p.first, p.second));
	} else if (name.compare("mmult") == 0) {
		pair<ExpressionPtr, ExpressionPtr> p = readPair(node);
		e.reset(new MmultExpr(p.first, p.second));
	} else if (name.compare("trans") == 0) {
		ExpressionPtr child = Expression::readFromXml(
				node->getNodeByPath("expr"));
		bool isConj = atoi(
				attribs->getNamedItem("conjugate")->getNodeValue().c_str()) > 0;
		e.reset(new TransExpr(child, isConj));
	} else if (name.compare("index") == 0) {
		Poco::XML::Node * anamenode = node->getNodeByPath("aname/expr");
		shared_ptr<NameExpr> aname = static_pointer_cast<NameExpr>(
				Expression::readFromXml(anamenode));
		ExpressionPtrVector indices;
		vector<PocoNode*> indexnodes = getChildrenByName(
				node->getNodeByPath("indices"), "expr");
		for (int i = 0; i < indexnodes.size(); i++) {
			PocoNode *indexnode = indexnodes[i];
			ExpressionPtr index = Expression::readFromXml(indexnode);
			indices.push_back(index);
		}
		bool isBoundsChecked = atoi(
				attribs->getNamedItem("boundscheck")->getNodeValue().c_str())
				> 0;

		bool isFlattened = false;
		PocoNode *isFlattenedNode = attribs->getNamedItem("flattened");
		if (isFlattenedNode != NULL) {
			isFlattened = atoi(isFlattenedNode->getNodeValue().c_str()) > 0;
		}

		PocoNode* isNegativeNode = attribs->getNamedItem("negative");
		bool isNegative = false;
		if (isNegativeNode != NULL) {
			isNegative = atoi(isNegativeNode->getNodeValue().c_str()) > 0;
		}
		PocoNode *isCopySliceNode = attribs->getNamedItem("copyslice");
		bool isCopySlice =
				(isCopySliceNode == NULL) ?
						false :
						(atoi(isCopySliceNode->getNodeValue().c_str()) > 0);
		e.reset(
				new IndexExpr(aname, indices, isBoundsChecked, isNegative,
						isFlattened, isCopySlice));
	} else if (name.compare("name") == 0) {
		int id = atoi(attribs->getNamedItem("id")->getNodeValue().c_str());
		e.reset(new NameExpr(id));
	} else if (name.compare("funcall") == 0) {
		Poco::XML::Node * fnnamenode = node->getNodeByPath("fnname/expr");
		shared_ptr<NameExpr> fnname = static_pointer_cast<NameExpr>(
				Expression::readFromXml(fnnamenode));
		vector<PocoNode*> argnodes = getChildrenByName(
				node->getNodeByPath("args"), "expr");
		ExpressionPtrVector args;
		for (int i = 0; i < argnodes.size(); i++) {
			PocoNode *argnode = argnodes[i];
			ExpressionPtr arg = Expression::readFromXml(argnode);
			args.push_back(arg);
		}
		e.reset(new FuncallExpr(fnname, args));
	} else if (name.compare("domain") == 0) {
		vector<PocoNode*> rangeNodes = getChildrenByName(node, "range");
		const int ndims = rangeNodes.size();
		//std::cout<<"Reading domain. Found "<<ndims<<" dimensions"<<std::endl;
		DomainExpr *dexpr = new DomainExpr(ndims);
		for (int i = 0; i < ndims; i++) {
			PocoNode *range = rangeNodes[i];
			PocoNode *startnode = range->getNodeByPath("start/expr");
			if (startnode != NULL) {
				//std::cout<<"Start node is not empty"<<std::endl;
				dexpr->setStartExpr(i, Expression::readFromXml(startnode));
			} else {
				//std::cout<<"Start node is empty. Setting start node to constant"<<std::endl;
				dexpr->setStartExpr(i, ConstExprBuilder::getIntConstExpr(0));
			}
			PocoNode *stopnode = range->getNodeByPath("stop/expr");
			if (stopnode != NULL)
				dexpr->setStopExpr(i, Expression::readFromXml(stopnode));
			Poco::XML::Node *stepnode = range->getNodeByPath("step/expr");
			if (stepnode != NULL) {
				dexpr->setStepExpr(i, Expression::readFromXml(stepnode));
			} else {
				dexpr->setStepExpr(i, ConstExprBuilder::getIntConstExpr(1));
			}
		}
		e.reset(dexpr);
	} else if (name.compare("const") == 0) {
		Poco::XML::Node * typenode = node->getNodeByPath("vtype");
		VTypePtr vtype = VType::readFromXml(typenode);
		XMLString val = attribs->getNamedItem("value")->getNodeValue();
		if (vtype->getBasicType() == VType::SCALAR_TYPE) {
			ScalarTypePtr stype = static_pointer_cast<ScalarType>(vtype);
			ScalarType::ScalarTag tag = stype->getScalarTag();
			Poco::UInt8 bitness = stype->getBitness();
			if (tag == ScalarType::SCALAR_INT && bitness == 32) {
				int ival = atoi(val.c_str());
				return ConstExprBuilder::getIntConstExpr(ival);
			} else if (tag == ScalarType::SCALAR_FLOAT && bitness == 32) {
				float fval = atof(val.c_str());
				return ConstExprBuilder::getFloatConstExpr(fval);
			} else if (tag == ScalarType::SCALAR_FLOAT && bitness == 64) {
				double dval = atof(val.c_str());
				return ConstExprBuilder::getDoubleConstExpr(dval);
			}
		}
	} else if (name.compare("libcall") == 0) {
		XMLString codeStr = attribs->getNamedItem("libfunc")->getNodeValue();
		LibCallExpr::Code c;
		if (codeStr.compare("sqrt") == 0) {
			c = LibCallExpr::LIB_SQRT;
		} else if (codeStr.compare("log2") == 0) {
			c = LibCallExpr::LIB_LOG2;
		} else if (codeStr.compare("log10") == 0) {
			c = LibCallExpr::LIB_LOG10;
		} else if (codeStr.compare("loge") == 0) {
			c = LibCallExpr::LIB_LOGE;
		} else if (codeStr.compare("expe") == 0) {
			c = LibCallExpr::LIB_EXPE;
		} else if (codeStr.compare("exp10") == 0) {
			c = LibCallExpr::LIB_EXP10;
		} else if (codeStr.compare("sin") == 0) {
			c = LibCallExpr::LIB_SIN;
		} else if (codeStr.compare("asin") == 0) {
			c = LibCallExpr::LIB_ASIN;
		} else if (codeStr.compare("cos") == 0) {
			c = LibCallExpr::LIB_COS;
		} else if (codeStr.compare("acos") == 0) {
			c = LibCallExpr::LIB_ACOS;
		} else if (codeStr.compare("tan") == 0) {
			c = LibCallExpr::LIB_TAN;
		} else if (codeStr.compare("atan") == 0) {
			c = LibCallExpr::LIB_ATAN;
		}
		XMLString exprStr = "expr";
		Poco::XML::Node * child = node->getNodeByPath(exprStr);
		ExpressionPtr arg = Expression::readFromXml(child);
		e.reset(new LibCallExpr(c, arg));
	} else if (name.compare("alloc") == 0) {
		XMLString tagStr = attribs->getNamedItem("alloctype")->getNodeValue();
		AllocExpr::AllocTag tag;
		if (tagStr.compare("ones") == 0) {
			tag = AllocExpr::ALLOC_ONES;
		} else if (tagStr.compare("zeros") == 0) {
			tag = AllocExpr::ALLOC_ZEROS;
		}
		vector<PocoNode*> children = getChildrenByName(node, "expr");
		ExpressionPtrVector vec;
		for (int i = 0; i < children.size(); i++)
			vec.push_back(Expression::readFromXml(children[i]));
		e.reset(new AllocExpr(vec, tag));
	}
	if (e != NULL) {
		Poco::XML::Node * typenode = node->getNodeByPath("vtype");
		if (typenode != NULL) {
			VTypePtr vtype = VType::readFromXml(typenode);
			e->setType(vtype);
		}
	} else {
		std::cout << "Failed to read expression " << name << " from XML"
				<< std::endl;
	}
	return e;
}

StmtPtr ForStmt::readFromXml(Poco::XML::Node * n) {
	vector<int> itervars;
	vector<PocoNode*> symnodes = getChildrenByName(n->getNodeByPath("itervars"),
			"sym");
	for (int i = 0; i < symnodes.size(); i++) {
		PocoNode *symnode = symnodes[i];
		AutoPtr<NamedNodeMap> attribs = symnode->attributes();
		int id = atoi(attribs->getNamedItem("id")->getNodeValue().c_str());
		//std::cout<<"reading id "<<id<<std::endl;
		itervars.push_back(id);
	}
	//std::cout<<"itervars size "<<itervars.size()<<std::endl;
	Poco::XML::Node * domainnode = n->getNodeByPath("domain/expr");
	ExpressionPtr domain = Expression::readFromXml(domainnode);
	Poco::XML::Node * bodynode = n->getNodeByPath("body/stmt");
	StmtPtr body = Statement::readFromXml(bodynode);
	StmtPtr fstmt(new ForStmt(itervars, domain, body));
	return fstmt;
}

StmtPtr PforStmt::readFromXml(Poco::XML::Node * n) {
	vector<int> itervars;
	vector<PocoNode*> symnodes = getChildrenByName(n->getNodeByPath("itervars"),
			"sym");
	for (int i = 0; i < symnodes.size(); i++) {
		PocoNode *symnode = symnodes[i];
		AutoPtr<NamedNodeMap> attribs = symnode->attributes();
		int id = atoi(attribs->getNamedItem("id")->getNodeValue().c_str());
		//  std::cout<<"reading id "<<id<<std::endl;
		itervars.push_back(id);
	}
	//std::cout<<"itervars size"<<itervars.size()<<std::endl;
	vector<int> privates;
	vector<PocoNode*> privnodes = getChildrenByName(
			n->getNodeByPath("privates"), "sym");
	for (int i = 0; i < privnodes.size(); i++) {
		PocoNode *symnode = privnodes[i];
		AutoPtr<NamedNodeMap> attribs = symnode->attributes();
		int id = atoi(attribs->getNamedItem("id")->getNodeValue().c_str());
		// std::cout<<"reading id "<<id<<std::endl;
		privates.push_back(id);
	}
	Poco::XML::Node * domainnode = n->getNodeByPath("domain/expr");
	ExpressionPtr domain = Expression::readFromXml(domainnode);
	Poco::XML::Node * bodynode = n->getNodeByPath("body/stmt");
	StmtPtr body = Statement::readFromXml(bodynode);
	StmtPtr pstmt(new PforStmt(itervars, privates, domain, body));
	return pstmt;
}

StmtPtr AssignStmt::readFromXml(Poco::XML::Node * n) {
	//std::cout<<"Reading assignment stmt from XML"<<std::endl;
	ExpressionPtrVector lhs;
	vector<PocoNode*> lhsnodes = getChildrenByName(n, "lhs");
	for (int i = 0; i < lhsnodes.size(); i++) {
		Poco::XML::Node * exprnode = lhsnodes[i]->getNodeByPath("expr");
		ExpressionPtr e = Expression::readFromXml(exprnode);
		lhs.push_back(e);
	}
	Poco::XML::Node * rhsnode = n->getNodeByPath("rhs/expr");
	ExpressionPtr rhs = Expression::readFromXml(rhsnode);
	StmtPtr s(new AssignStmt(lhs, rhs));
	s->setType(UnitType::getInstance());
	return s;
}

StmtPtr ReturnStmt::readFromXml(Poco::XML::Node * n) {
	vector<PocoNode*> children = getChildrenByName(n, "expr");
	vector<int> ids;
	for (int i = 0; i < children.size(); i++) {
		NameExprPtr ne = static_pointer_cast<NameExpr>(
				NameExpr::readFromXml(children[i]));
		ids.push_back(ne->getId());
	}
	StmtPtr rstmt(new ReturnStmt(ids));
	return rstmt;
}

StmtPtr IfStmt::readFromXml(Poco::XML::Node * n) {
	ExpressionPtr condexpr = Expression::readFromXml(n->getNodeByPath("expr"));
	StmtPtr ifstmt = Statement::readFromXml(n->getNodeByPath("if/stmt"));
	Poco::XML::Node * elsenode = n->getNodeByPath("else/stmt");
	StmtPtr ptr;
	if (elsenode != NULL) {
		StmtPtr elsestmt = Statement::readFromXml(elsenode);
		ptr.reset(new IfStmt(condexpr, ifstmt, elsestmt));
	} else {
		ptr.reset(new IfStmt(condexpr, ifstmt));
	}
	return ptr;
}

StmtPtr LibCallStmt::readFromXml(Poco::XML::Node *n) {
	AutoPtr<NamedNodeMap> attribs = n->attributes();
	ExpressionPtr lhsExpr = Expression::readFromXml(
			n->getNodeByPath("lhs/expr"));
	ExpressionPtr rhsExpr = Expression::readFromXml(
			n->getNodeByPath("rhs/expr"));
	bool isAlloc = atoi(attribs->getNamedItem("alloc")->getNodeValue().c_str())
			> 0;
	PocoNode *checkDimsAttrib = attribs->getNamedItem("checkDims");
	bool checkDims = false;
	if (checkDimsAttrib != NULL) {
		checkDims = atoi(checkDimsAttrib->getNodeValue().c_str()) > 0;
	}
	StmtPtr ptr(new LibCallStmt(lhsExpr, rhsExpr, isAlloc, checkDims));
	return ptr;
}

StmtPtr Statement::readFromXml(Poco::XML::Node * n) {
	AutoPtr<NamedNodeMap> attribs = n->attributes();
	XMLString name = attribs->getNamedItem("name")->getNodeValue();
	//std::cout<<"Statement::readFromXml "<<name<<std::endl;
	if (name.compare("stmtlist") == 0) {
		return StmtList::readFromXml(n);
	} else if (name.compare("assignstmt") == 0) {
		return AssignStmt::readFromXml(n);
	} else if (name.compare("forstmt") == 0) {
		return ForStmt::readFromXml(n);
	} else if (name.compare("pforstmt") == 0) {
		return PforStmt::readFromXml(n);
	} else if (name.compare("returnstmt") == 0) {
		return ReturnStmt::readFromXml(n);
	} else if (name.compare("whilestmt") == 0) {
		//std::cout<<"Statement::readFromXml: Reading whilestmt"<<std::endl;
		return WhileStmt::readFromXml(n);
	} else if (name.compare("libcallstmt") == 0) {
		return LibCallStmt::readFromXml(n);
	} else if (name.compare("ifstmt") == 0) {
		return IfStmt::readFromXml(n);
	} else {
		std::cout << "Unimplemented statement type " << name << std::endl;
		StmtPtr temp;
		return temp;
	}
}

StmtPtr WhileStmt::readFromXml(Poco::XML::Node * n) {
	ExpressionPtr cond = Expression::readFromXml(n->getNodeByPath("expr"));
	StmtPtr body = Statement::readFromXml(n->getNodeByPath("stmt"));
	StmtPtr wstmt(new WhileStmt(cond, body));
	return wstmt;
}

StmtPtr StmtList::readFromXml(Poco::XML::Node * n) {
	using std::cout;
	using std::endl;
	//std::cout<<"StmtList::readFromXml"<<std::endl;
	StmtPtrVector slist;
	vector<PocoNode*> children = getChildrenByName(n, "stmt");
	//std::cout<<"Number of children: "<<children.size()<<std::endl;
	for (int i = 0; i < children.size(); i++) {
		//std::cout<<"Processing child "<<i<<std::endl;
		PocoNode *child = children[i];
		StmtPtr stmt = Statement::readFromXml(child);
		//cout<<"Read child "<<i<<std::endl;
		slist.push_back(stmt);
	}
	shared_ptr<StmtList> s(new StmtList(slist));
	AutoPtr<NamedNodeMap> attribs = n->attributes();
	PocoNode* onGpuAttr = attribs->getNamedItem("gpu");
	if (onGpuAttr != NULL && atoi(onGpuAttr->getNodeValue().c_str()) > 0) {
		s->m_onGpu = true;
	} else {
		s->m_onGpu = false;
	}
	s->setType(UnitType::getInstance());
	//std::cout<<"StmtList::readFromXml Fin"<<std::endl;
	return s;
}

SymTable *SymTable::readFromXml(Poco::XML::Node * n) {
	SymTable *s = new SymTable;
	vector<PocoNode*> symnodes = getChildrenByName(n, "sym");
	for (int i = 0; i < symnodes.size(); i++) {
		PocoNode *symnode = symnodes[i];
		AutoPtr<NamedNodeMap> attribs = symnode->attributes();
		int id = atoi(attribs->getNamedItem("id")->getNodeValue().c_str());
		XMLString name = attribs->getNamedItem("name")->getNodeValue();
		Poco::XML::Node * vtypenode = symnode->getNodeByPath("vtype");
		VTypePtr type = VType::readFromXml(vtypenode);
		s->addEntry(id, name, type);
	}
	return s;
}

VFunction *VFunction::readFromXml(Poco::XML::Node * elem) {
	AutoPtr<NamedNodeMap> attribs = elem->attributes();
	XMLString name = attribs->getNamedItem("id")->getNodeValue();
	//  std::cout<<"Reading function "<<name <<std::endl;
	Poco::XML::Node * arglist = elem->getNodeByPath("arglist");
	vector<VFunction::Arg> args;
	vector<PocoNode*> argnodes = getChildrenByName(arglist, "arg");
	//std::cout<<"Vfunction::readFromXml: arglist size "<<argnodes.size()<<std::endl;
	for (int i = 0; i < argnodes.size(); i++) {
		Poco::XML::Node * xmlarg = argnodes[i];
		AutoPtr<NamedNodeMap> argAttribs = xmlarg->attributes();
		VFunction::Arg arg;
		arg.m_id = atoi(argAttribs->getNamedItem("id")->getNodeValue().c_str());
		arg.m_restrict = atoi(
				argAttribs->getNamedItem("restrict")->getNodeValue().c_str())
				> 0;
		args.push_back(arg);
	}
	Poco::XML::Node * bodynode = elem->getNodeByPath("body/stmt");
	StmtListPtr sl = static_pointer_cast<StmtList>(
			Statement::readFromXml(bodynode));
	SymTable *table = SymTable::readFromXml(elem->getNodeByPath("symtable"));
	shared_ptr<FuncType> fntype = FuncType::readFromXml(
			elem->getNodeByPath("vtype"));
	VFunction *f = new VFunction(name, sl, table, args, fntype);
	return f;
}

VModule *VModule::readFromString(std::string s) {
	//std::cout<<s<<std::endl;
	std::stringstream ss(s);
	Poco::XML::InputSource src(ss);
	Poco::XML::DOMParser parser;
	Poco::AutoPtr<Poco::XML::Document> doc = parser.parse(&src);
	NamedNodeMap *attribs = doc->attributes();
	//XMLString modnmae = attribs->getNamedItem("name")->getNodeValue();
	VModule *m = new VModule;
	vector<PocoNode*> fnNodes = getChildrenByName(doc->firstChild(),
			"function");
	vector<VFunction*> fns;
	for (int i = 0; i < fnNodes.size(); i++) {
		VFunction *fn = VFunction::readFromXml(fnNodes[i]);
		fn->setModule(m);
		fns.push_back(fn);
	}
	m->m_funcs = fns;
#ifndef VR_MINIMAL
	m->m_compiler = NULL;
	llvm::InitializeNativeTarget();
#endif
	return m;
}

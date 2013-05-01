#Copyright [2013] Rahul Garg and McGill University
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

import ast
import inspect
import sys

implFunMap = {'sqrt':'sqrt','log':'ln','log2':'log2','log10':'log10','exp':'exp','sin':'sin','cos':'cos','tan':'tan','atan':'atan','asin':'asin','acos':'acos'}

class XmlNode(object):
    def __init__(self,name):
        self.name = name
        self.attribs = {}
        self.children = []
        self.offset = 0
    def addAttribute(self,aname,avalue):
        self.attribs[aname] = avalue
    def addChild(self,child):
        self.children.append(child)
    def toXML(self):
        s = ''
        for i in range(self.offset):
            s += ' '
        s = s + "<"+self.name
        for k in self.attribs:
            s = s+' '+k+'="'+str(self.attribs[k])+'"'
        #if 'name' in self.attribs:
            #print(self.attribs['name'])
        s=s+'>\n'
        for child in self.children:
            child.offset = self.offset + 2
            s = s+child.toXML()
        for i in range(self.offset):
            s += ' '
        s=s+'</'+self.name+'>\n'
        return s

class VType(object):
    pass

class VoidType(VType):
    def __eq__(self,other):
        if type(self)==type(other):
            return True
        else:
            return False
    def __str__(self):
        return 'void'

class ScalarType(VType):
    def __eq__(self,other):
        if type(self)==type(other):
            return True
        else:
            return False

class Int32(ScalarType):
    def __str__(self):
        return 'int32'


class Int64(ScalarType):
    def __str__(self):
        return 'int64'

class Float32(ScalarType):
    def __str__(self):
        return 'float32'

class Float64(ScalarType):
    def __str__(self):
        return 'float64'
    
class BoolType(ScalarType):
    def __str__(self):
        return 'bool'

class ArrayType(VType):
    def __init__(self,etype,ndims):
        self.etype = etype
        self.ndims = ndims
    def __str__(self):
        return 'array('+str(self.etype)+','+str(self.ndims)+')'
    def __eq__(self,other):
        if self.etype==other.etype and self.ndims==other.ndims:
            return True
        return False

class FuncType(VType):
    def __init__(self,intypes,rtypes):
        self.rtypes = rtypes
        self.intypes = intypes
        
class DomainType(VType):
    def __init__(self,ndims):
        self.ndims = ndims


def vtypeFromString(typestr):
    if typestr=='int32':
        return Int32()
    elif typestr=='int64':
        return Int64()
    elif typestr=='float32':
        return Float32()
    elif typestr=='float64':
        return Float64()
    elif typestr=='bool':
        return BoolType()
    elif typestr.startswith('array'):
        sep1 = typestr.find('(')
        sep2 = typestr.find(',')
        sep3 = typestr.find(')')
        ndims = int(typestr[sep2+1:sep3])
        etype = vtypeFromString(typestr[sep1+1:sep2])
        return ArrayType(etype,ndims)
    elif typestr=='void':
        return VoidType()

def vtypeToXml(vtype):
    x = XmlNode("vtype")
    if isinstance(vtype,ScalarType):
        x.addAttribute('name',str(vtype))
    elif isinstance(vtype,VoidType):
        x.addAttribute('name',str(vtype))
    elif isinstance(vtype,ArrayType):
        x.addAttribute("name",'array')
        x.addAttribute("ndims",vtype.ndims)
        x.addAttribute("layout",'strides')
        elemtype = vtypeToXml(vtype.etype)
        x.addChild(elemtype)
    elif isinstance(vtype,FuncType):
        typenode = XmlNode('vtype')
        typenode.addAttribute('name','func')
        intypenode = XmlNode('intypes')
        for t in vtype.intypes:
            intypenode.addChild(vtypeToXml(t))
        outtypenode = XmlNode('outtypes')
        for t in vtype.rtypes:
            outtypenode.addChild(vtypeToXml(t))
        typenode.addChild(intypenode)
        typenode.addChild(outtypenode)
        return typenode
    elif isinstance(vtype,DomainType):
        typenode = XmlNode('vtype')
        typenode.addAttribute('name','domain')
        typenode.addAttribute('ndims',vtype.ndims)
        return typenode
    else:
        pass
    return x

        
class FnCompiler(object):
    def __init__(self,fn):
        self.fn = fn
        argspec = inspect.getfullargspec(fn)
        arglist = argspec.args
        annots = argspec.annotations
        intypes = []
        outtypes = []
        for arg in arglist:
            intypes.append(vtypeFromString(annots[arg]))
        otypeannot = annots['return']
        if isinstance(otypeannot,list):
            for otype in otypeannot:
                outtypes.append(vtypeFromString(otype))
        else:
            outtypes.append(vtypeFromString(otypeannot))
        #print(intypes,outtypes)
        self.fntype = FuncType(intypes,outtypes)
        self.idToStrMap = {}
        self.strToIdMap = {}
        self.typeMap = {}
        self.exprTypeMap = {}
        self.fnast = ast.parse(inspect.getsource(fn))
        self.curid = 0
        self.tempid = 0
    def genNewSym(self,symtype):
        while True:
            tempname = 'rahul'+str(self.tempid)
            if tempname in self.strToIdMap:
                self.tempid += 1
            else:
                id = self.insertNewName(tempname)
                self.typeMap[id] = symtype
                return id
    def enableGpuMode(self):
        pass
    def disableGpuMode(self):
        pass
    def insertNewName(self,name):
        id = self.curid
        self.idToStrMap[id] = name
        self.strToIdMap[name] = self.curid
        self.curid = id + 1
        return id
    def verifyOrInsertType(self,tid,vtype):
        if tid in self.typeMap:
            if not self.typeMap[tid]==vtype:
                print('Error in type verification')
        else:
            self.typeMap[tid] = vtype
    def getOrInsertId(self,name):
        if name in self.strToIdMap:
            return self.strToIdMap[name]
        else:
            return self.insertNewName(name)
    def initArgs(self):
        fnnode = self.fnast.body[0]
        i = 0
        for arg in ast.iter_child_nodes(fnnode.args):
            #print(arg._fields)
            argid = self.insertNewName(arg.arg)
            self.typeMap[argid] = self.fntype.intypes[i]
            i = i+1
    def toXMLNode(self):
        inferTypes(self)
        xmlnode = XmlNode("function")
        xmlnode.addAttribute("id",self.fn.__name__)
        fnnode = self.fnast.body[0]
        xmlnode.addChild(vtypeToXml(self.fntype))
        arglist = XmlNode("arglist")
        for arg in ast.iter_child_nodes(fnnode.args):
            argxmlnode = XmlNode('arg')
            argxmlnode.addAttribute('id',self.strToIdMap[arg.arg])
            argxmlnode.addAttribute('restrict','0')
            arglist.addChild(argxmlnode)
        xmlnode.addChild(arglist)
        bodyxml = genFuncCode(self)
        symtablenode = XmlNode('symtable')
        for id in self.idToStrMap:
            name = self.idToStrMap[id]
            symnode = XmlNode('sym')
            symnode.addAttribute('id',id)
            symnode.addAttribute('name',name)
            if id in self.typeMap:
                vtype = self.typeMap[id]
                symnode.addChild(vtypeToXml(vtype))
            symtablenode.addChild(symnode)
        xmlnode.addChild(symtablenode)
        xmlnode.addChild(bodyxml)
        return xmlnode

def inferTypes(fnc):
    fnc.initArgs()
    stmts = fnc.fnast.body[0].body
    for stmt in stmts:
        inferStmtType(fnc,stmt)
    return

def getNodeList(n):
    if isinstance(n,ast.AST):
        return [n]
    return n

def binOpType(ltype,rtype):
    #TODO: Handle all cases
    if ltype==rtype:
        return ltype
    if isinstance(ltype,ScalarType) and isinstance(rtype,ScalarType):
        if isinstance(ltype,Float64) or isinstance(rtype,Float64):
            return Float64()
        if isinstance(ltype,BoolType):
            return rtype
        if instance(rtype,BoolType):
            return ltype
    if isinstance(ltype,ArrayType) and isinstance(rtype,ArrayType):
        basetype = binOpType(ltype.etype,rtype.etype)
        ndims = ltype.ndims
        if rtype.ndims>ltype.ndims:
            ndims = rtype.ndims
        return ArrayType(basetype,ndims)
    raise Exception('BinOp type not inferred')

def arrayIndexType(atype,indices):
    ndims = atype.ndims
    allScalarInts = True
    for child in indices:
        isInt = isinstance(child,Int32) or isinstance(child,Int64)
        if not isInt:
            allScalarInts = False
    if allScalarInts:
        newdims = ndims - len(indices)
        if newdims==0:
            return atype.etype
        else:
            return ArrayType(atype.etype,newdims)

def opToName(op):
    if isinstance(op,ast.Add):
        return 'plus'
    elif isinstance(op,ast.Sub):
        return 'minus'
    elif isinstance(op,ast.Mult):
        return 'mult'
    elif isinstance(op,ast.Div):
        return 'div'
    

def genExprCode(fnc,expr,simplify=True):
    retlist = []
    xmlnode = XmlNode('expr')
    if isinstance(expr,ast.Name):
        xmlnode.addAttribute('name','name')
        xmlnode.addAttribute('id',fnc.strToIdMap[expr.id])
    elif isinstance(expr,ast.Compare):
        op = expr.ops[0]
        opname = ''
        if isinstance(op,ast.Lt):
            opname = 'lt'
        elif isinstance(op,ast.LtE):
            opname = 'leq'
        elif isinstance(op,ast.Gt):
            opname = 'gt'
        elif isinstance(op,ast.GtE):
            opname = 'geq'
        xmlnode.addAttribute('name',opname)
        lhsnode = XmlNode('lhs')
        rhsnode = XmlNode('rhs')
        lhslist = genExprCode(fnc,expr.left)
        rhslist = genExprCode(fnc,expr.comparators[0])
        for j in range(len(rhslist)-1):
            retlist.append(rhslist[i])
        for i in range(len(lhslist)-1):
            retlist.append(lhslist[i])     
        lhsnode.addChild(lhslist[-1])
        rhsnode.addChild(rhslist[-1])
        xmlnode.addChild(lhsnode)
        xmlnode.addChild(rhsnode)        
    elif isinstance(expr,ast.BinOp):
        rhsnode = XmlNode('rhs')
        rhslist = genExprCode(fnc,expr.right)
        for i in range(len(rhslist)-1):
            retlist.append(rhslist)
        rhsnode.addChild(rhslist[-1])
        lhsnode = XmlNode('lhs')
        lhslist = genExprCode(fnc,expr.left)
        lhsnode.addChild(lhslist[-1])
        opnode = XmlNode('expr')
        opnode.addAttribute('name',opToName(expr.op))
        opnode.addChild(lhsnode)
        opnode.addChild(rhsnode)
        if (not isinstance(fnc.exprTypeMap[expr.left],ScalarType)) or (not isinstance(fnc.exprTypeMap[expr.right],ScalarType)):
            opnode.addAttribute('broadcast','1')
            if not simplify:
                xmlnode = opnode
            else:
                tempvarexpr = XmlNode('expr')
                tempvarexpr.addAttribute('name','name')
                exprtype = fnc.exprTypeMap[expr]
                tempvarexpr.addAttribute('id',fnc.genNewSym(exprtype))
                tempvarexpr.addChild(vtypeToXml(exprtype))
                xmlnode = tempvarexpr
                astmtnode = XmlNode('stmt')
                astmtnode.addAttribute('name','libcallstmt')
                temprhs = XmlNode('rhs')
                temprhs.addChild(opnode)
                astmtnode.addChild(temprhs)
                templhs = XmlNode('lhs')
                templhs.addChild(tempvarexpr)
                astmtnode.addChild(templhs)
                astmtnode.addAttribute('alloc','1')
                retlist.append(astmtnode)
        else:
            xmlnode = opnode
    elif isinstance(expr,ast.Subscript):
        xmlnode.addAttribute('name','index')
        xmlnode.addAttribute('negative','1')
        xmlnode.addAttribute('boundscheck','1')
        namenode = XmlNode('aname')
        valuelist = genExprCode(fnc,expr.value)
        for i in range(len(valuelist)-1):
            retlist.append(valuelist[i])
        namenode.addChild(valuelist[-1])
        xmlnode.addChild(namenode)
        indexnode = XmlNode('indices')
        xmlnode.addChild(indexnode)
        indices = expr.slice
        if isinstance(indices,ast.Index):
            indextup = indices.value
            if isinstance(indextup,ast.Tuple):
                indextup = indextup.elts
            else:
                indextup = [indextup]
            for child in indextup:
                indexlist = genExprCode(fnc,child)
                for i in range(len(indexlist)-1):
                    retlist.append(indexlist[i])
                indexnode.addChild(indexlist[-1])
        elif isinstance(indices,ast.ExtSlice):
            print('Slices are unimplemented')
    elif isinstance(expr,ast.Call):
        if expr.func.id =='range':
            xmlnode.addAttribute('name','domain')
            rangenode = XmlNode('range')
            stopnode = XmlNode('stop')
            stopexprlist = genExprCode(fnc,expr.args[-1])
            for i in range(len(stopexprlist)-1):
                retlist.append(stopexprlist[i])
            stopexprnode = stopexprlist[-1]
            stopnode.addChild(stopexprnode)
            rangenode.addChild(stopnode)
            if len(expr.args)>1:
                startnode = XmlNode('start')
                startexprlist = genExprCode(fnc,expr.args[0])
                for i in range(len(startexprlist)-1):
                    retlist.append(startexprlist[i])
                startexprnode = startexprlist[-1]
                startnode.addChild(startexprnode)
                rangenode.addChild(startnode)
            xmlnode.addChild(rangenode)
        elif expr.func.id=='PAR':
            argrets = genExprCode(fnc,expr.args[0])
            for i in range(len(argrets)-1):
                retlist.append(argrets[i])
            xmlnode = argrets[-1]
        elif expr.func.id=='Domain':
            xmlnode.addAttribute('name','domain')
            for arg in expr.args:
                argtype = fnc.exprTypeMap[arg]
                if(isinstance(argtype,Int32)):
                    argrets = genExprCode(fnc,arg)
                    for i in range(len(argrets)-1):
                        retlist.append(argrets[i])
                    rangenode = XmlNode('range')
                    stopnode = XmlNode('stop')
                    rangenode.addChild(stopnode)
                    stopnode.addChild(argrets[-1])
                    xmlnode.addChild(rangenode)
                elif isinstance(argtype,DomainType):
                    print('arg is ',arg)
                    argrets = genExprCode(fnc,arg)
                    for i in range(len(argrets)-1):
                        retlist.append(argrets[i])
                    argxml = argrets[-1]
                    #print(argxml.toXML())
                    for child in argxml.children:
                        if child.name=='range':
                            xmlnode.addChild(child)
                    sys.stdout.flush()
                    
                else:
                    raise Exception('Unimplemented domain type')
        elif expr.func.id in implFunMap:
            opnode = XmlNode('expr')
            opnode.addAttribute('name','libcall')
            opnode.addChild(genExprCode(fnc,expr.args[0]))
            opnode.addChild(vtypeToXml(fnc.exprTypeMap[expr]))
            tempvarexpr = XmlNode('expr')
            tempvarexpr.addAttribute('name','name')
            exprtype = fnc.exprTypeMap[expr]
            tempvarexpr.addAttribute('id',fnc.genNewSym(exprtype))
            tempvarexpr.addChild(vtypeToXml(exprtype))
            xmlnode = tempvarexpr
            astmtnode = XmlNode('stmt')
            astmtnode.addAttribute('name','libcallstmt')
            temprhs = XmlNode('rhs')
            temprhs.addChild(opnode)
            astmtnode.addChild(temprhs)
            templhs = XmlNode('lhs')
            templhs.addChild(tempvarexpr)
            astmtnode.addChild(temprhs)
            astmtnode.addAttribute('alloc','1')
            retlist.append(astmtnode)
        elif expr.func.id=='dot':
            raise Exception('dot not yet implemented')
        elif expr.func.id=='transpose':
            raise Exception('transpose not yet implemented')
        elif expr.func.id=='sum':
            raise Excpetion('sum not yet implemented')
        elif expr.func.id=='prod':
            raise Exception('prod not yet implemented')
    else:
        xmlnode.addAttribute('name','const')
        xmlnode.addAttribute('value',expr.n)
    if expr in fnc.exprTypeMap:
        xmlnode.addChild(vtypeToXml(fnc.exprTypeMap[expr]))
    else:
        raise Exception('Expression VType not found')
    retlist.append(xmlnode)
    return retlist

def genStmtCode(fnc,stmt):
    retlist = []
    xmlnode = XmlNode('stmt')
    if isinstance(stmt,ast.Assign):
        xmlnode.addAttribute('name','assignstmt')
        rhsnode = XmlNode('rhs')
        rhslist = genExprCode(fnc,stmt.value)
        for i in range(len(rhslist)-1):
            retlist.append(rhslist[i])
        rhsnode.addChild(rhslist[-1])
        xmlnode.addChild(rhsnode)
        lhsnode = XmlNode('lhs')
        target = stmt.targets[0]
        lhslist = genExprCode(fnc,target)
        for i in range(len(lhslist)-1):
            retlist.append(lhslist[i])
        lhsnode.addChild(lhslist[-1])
        xmlnode.addChild(lhsnode)
    elif isinstance(stmt,ast.While):
        xmlnode.addAttribute('name','whilestmt')
        condlist = genExprCode(fnc,stmt.test)
        for i in range(len(condlist)-1):
            retlist.append(condlist)
        xmlnode.addChild(condlist[-1])
        bodynode = XmlNode('stmt')
        bodynode.addAttribute('name','stmtlist')
        for child in getNodeList(stmt.body):
            childlist = genStmtCode(fnc,child)
            for i in range(len(childlist)-1):
                retlist.append(childlist[i])
            bodynode.addChild(childlist[-1])
        xmlnode.addChild(bodynode)
    elif isinstance(stmt,ast.If):
        xmlnode.addAttribute('name','ifstmt')
        condlist = genExprCode(fnc,stmt.test)
        for i in range(len(condlist)-1):
            retlist.append(condlist[i])
        xmlnode.addChild(condlist[-1])
        ifbodynode = XmlNode('if')
        ifbodystmtlist = XmlNode('stmt')
        ifbodystmtlist.addAttribute('name','stmtlist')
        ifbodynode.addChild(ifbodystmtlist)
        xmlnode.addChild(ifbodynode)
        #print('body',stmt.body,'orelse',stmt.orelse)
        for child in stmt.body:
            childlist = genStmtCode(fnc,child)
            for childStmt in childlist:
                ifbodystmtlist.addChild(childStmt)
        elsebodynode = XmlNode('else')
        elsebodystmtlist = XmlNode('stmt')
        elsebodystmtlist.addAttribute('name','stmtlist')
        elsebodynode.addChild(elsebodystmtlist)
        for child in stmt.orelse:
            childlist = genStmtCode(fnc,child)
            for  childStmt in childlist:
                elsebodystmtlist.addChild(childStmt)
        xmlnode.addChild(elsebodynode)
    elif isinstance(stmt,ast.For):
        isParallel = False
        if isinstance(stmt.iter,ast.Call):
            if stmt.iter.func.id=='PAR':
                xmlnode.addAttribute('name','pforstmt')
                if len(stmt.iter.args)>1:
                    privatesCall = stmt.iter.args[1]
                    privatesNode = XmlNode('privates')
                    for privateVar in privatesCall.args:
                        varNode = XmlNode('sym')
                        varNode.addAttribute('id',fnc.strToIdMap[privateVar.s])
                        privatesNode.addChild(varNode)
                    xmlnode.addChild(privatesNode)
                        #print(privateVar,fnc.strToIdMap[privateVar.s])
                isParallel = True
        if not isParallel:
            xmlnode.addAttribute('name','forstmt')
        itervars = XmlNode('itervars')
        target = stmt.target
        if isinstance(target,ast.Name):
            target = [target,]
        if isinstance(target,ast.Tuple):
            target = target.elts
        for v in target:
            name = v.id
            nameid = fnc.strToIdMap[name]
            symnode = XmlNode('sym')
            symnode.addAttribute('id',nameid)
            itervars.addChild(symnode)
        xmlnode.addChild(itervars)
        domainnode = XmlNode('domain')
        domainlist = genExprCode(fnc,stmt.iter)
        for i in range(len(domainlist)-1):
            retlist.append(domainlist[i])        
        domainnode.addChild(domainlist[-1])
        xmlnode.addChild(domainnode)
        bodynode = XmlNode('body')
        xmlnode.addChild(bodynode)
        bodystmtlist = XmlNode('stmt')
        bodystmtlist.addAttribute('name','stmtlist')
        bodynode.addChild(bodystmtlist)
        for child in getNodeList(stmt.body):
            childlist = genStmtCode(fnc,child)
            for childnode in childlist:
                bodystmtlist.addChild(childnode)
    elif isinstance(stmt,ast.Return):
        xmlnode.addAttribute('name','returnstmt')
        if stmt.value is not None:
            rvallist = genExprCode(fnc,stmt.value)
            for i in range(len(rvallist)-1):
                retlist.append(rvallist[i])
            xmlnode.addChild(rvallist[-1])
    retlist.append(xmlnode)
    return retlist

def genFuncCode(fnc):
    stmts = fnc.fnast.body[0].body
    bodynode = XmlNode('body')
    xmlnode = XmlNode('stmt')
    xmlnode.addAttribute('name','stmtlist')
    #xmlnode.addAttribute('gpu','0')
    inGpuRegion = False
    gpunode = None
    for stmt in stmts:
        #print 'statement found',type(stmt),isinstance(stmt,ast.Expr)
        if isinstance(stmt,ast.Expr) and isinstance(stmt.value,ast.Call):
            func = stmt.value
            if func.func.id=='gpu_begin':
                print('beginning gpu')
                inGpuRegion = True
                gpunode = XmlNode('stmt')
                gpunode.addAttribute('name','stmtlist')
                gpunode.addAttribute('gpu','1')
            elif func.func.id=='gpu_end':
                print('ending gpu')
                inGpuRegion = False
                xmlnode.addChild(gpunode)
        else:   
            stmtxmllist = genStmtCode(fnc,stmt)
            for stmtxml in stmtxmllist:
                if inGpuRegion:
                    gpunode.addChild(stmtxml)
                else:
                    xmlnode.addChild(stmtxml)
    bodynode.addChild(xmlnode)
    return bodynode       
            

def inferExprType(fnc,expr):
    exprtype = None
    if isinstance(expr,ast.Name):
        nameid = fnc.strToIdMap[expr.id]
        exprtype = fnc.typeMap[nameid]
    elif isinstance(expr,ast.Compare):
        #print('inferring Compare operator',expr)
        inferExprType(fnc,expr.left)
        inferExprType(fnc,expr.comparators[0])
        exprtype = BoolType()
    elif isinstance(expr,ast.BinOp):
        rtype = inferExprType(fnc,expr.right)
        ltype = inferExprType(fnc,expr.left)
        exprtype = binOpType(ltype,rtype)
    elif isinstance(expr,ast.Subscript):
        basetype = inferExprType(fnc,expr.value)
        indices = expr.slice
        indextypes = []
        if isinstance(indices,ast.Index):
            indextup = indices.value
            if isinstance(indextup,ast.Tuple):
                indextup = indextup.elts
            else:
                indextup = [indextup]
            for child in indextup:
                itype = inferExprType(fnc,child)
                indextypes.append(itype)
        elif isinstance(indices,ast.ExtSlice):
            raise Exception('Slices are unimplemented')
        exprtype = arrayIndexType(basetype,indextypes)
    elif isinstance(expr,ast.Call):
        if expr.func.id=='privates':
            return None
        for arg in expr.args:
            inferExprType(fnc,arg)
        if expr.func.id=='range':
            if len(expr.args)>3 or len(expr.args)==0:
                raise Exception('invalid number of arguments to range')
            for arg in expr.args:
                argtype = fnc.exprTypeMap[arg]
                if not (isinstance(argtype,Int32) or isinstance(argtype,Int64)):
                    raise Exception('Invalid arguments to range')
            exprtype = DomainType(1)
        elif expr.func.id=='Domain':
            exprtype = DomainType(len(expr.args))
        elif expr.func.id=='PAR':
            exprtype = fnc.exprTypeMap[expr.args[0]]
        elif expr.func.id in implFunMap:
            exprtype = fnc.exprTypeMap[expr.args[0]]
        elif expr.func.id=='dot':
            assert len(expr.args) is 2
            ltype = inferExprType(fnc,expr.args[0])
            rtype = inferExprType(fnc,expr.args[1])
            assert isinstance(ltype,ArrayType) and isinstance(ltype,ArrayType)
            assert ltype.dims==rtype.ndims and ltype.etype==rtype.etype
            return ltype
        elif expr.func.id=='transpose':
            atype = fnc.exprTypeMap[expr.args[0]]
            if (not isinstance(atype,ArrayType)) or (isinstance(atype,ArrayType) and not atype.ndims==2):
                raise Exception('Type mismatch')
            else:
                exprtype = atype
        elif expr.func.id=='sum':
            raise Exception('sum not yet implemented')
        elif expr.func.id=='prod':
            raise Exception('prod not yet implemented')
    else:
        #must be constant
        #print(expr)
        val = expr.n
        if isinstance(val,int):
            exprtype = Int32()
        else:
            exprtype = Float64()
    fnc.exprTypeMap[expr] = exprtype
    return exprtype

def printIfElse(stmt):
    print('Start if/else')
    print('test',stmt.test)
    print('body',stmt.body)
    for child in getNodeList(stmt.orelse):
        if isinstance(child,ast.If):
            printIfElse(child)
        else:
            print('orelse',child)
    print('End if/else')

def inferStmtType(fnc,stmt):
    #print stmt._fields
    if isinstance(stmt,ast.Assign):
        rhstype = inferExprType(fnc,stmt.value)
        target = stmt.targets[0]
        if isinstance(target,ast.Name):
            name = target.id
            targetid = fnc.getOrInsertId(name)
            fnc.verifyOrInsertType(targetid,rhstype)
            fnc.exprTypeMap[target] = rhstype
        else:
            lhstype = inferExprType(fnc,target)
            if not rhstype==lhstype:
                raise Exception('Type mismatch')
    elif isinstance(stmt,ast.Return):
        if stmt.value is not None:
            inferExprType(fnc,stmt.value)
    elif isinstance(stmt,ast.If):
        inferExprType(fnc,stmt.test)
        for child in getNodeList(stmt.body):
            inferStmtType(fnc,child)
        if stmt.orelse is not None:
            for child in getNodeList(stmt.orelse):
                inferStmtType(fnc,child)
    elif isinstance(stmt,ast.For):
        target = stmt.target
        if isinstance(target,ast.Name):
            target = [target,]
        if isinstance(target,ast.Tuple):
            target = target.elts
        for v in target:
            name = v.id
            nameid = fnc.getOrInsertId(name)
            fnc.verifyOrInsertType(nameid,Int32())
        for child in getNodeList(stmt.body):
            inferStmtType(fnc,child)
        inferExprType(fnc,stmt.iter)
    elif isinstance(stmt,ast.While):
        condtype = inferExprType(fnc,stmt.test)
        for child in getNodeList(stmt.body):
            inferStmtType(fnc,child)


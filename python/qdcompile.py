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
import sys
import inspect
from qdxml import *

def gpu_begin():
    pass

def gpu_end():
    pass

def privates(*x):
	pass 

def PAR(x,privateVars=None):
    return x

def domainGen(domain):
    if len(domain.args)==1:
        m = len(domain.args[0])
        for i in range(m):
            yield i
    if len(domain.args)==2:
        print('case 2')
        m = (domain.args[0])
        n = (domain.args[1])
        for i in range(m):
            for j in range(n):
                yield i,j
    

class Domain:
    def __init__(self,*args):
        self.args = args
    def __iter__(self):
        return domainGen(self)
def vtypeToCtype(vt):
    if isinstance(vt,Int32):
        return 'int'
    elif isinstance(vt,Int64):
        return 'long int'
    elif isinstance(vt,Float32):
        return 'float'
    elif isinstance(vt,Float64):
        return 'double'
    elif isinstance(vt,VoidType):
        return 'void'
    else:
        return 'void*'

def vtypeToFmt(vt):
    if isinstance(vt,Int32):
        return 'i'
    elif isinstance(vt,Int64):
        return 'l'
    elif isinstance(vt,Float32):
        return 'f'
    elif isinstance(vt,Float64):
        return 'd'
    else:
        return 'o'

def genWrapFunc(modname,fnc):
    fn = fnc.fn
    fnname = fn.__name__
    code = 'extern '
    nargs = (len(fnc.fntype.intypes))
    rtype = vtypeToCtype(fnc.fntype.rtypes[0])
    isVoid = isinstance(fnc.fntype.rtypes[0],VoidType)
    code += rtype + ' '+fnname + '('
    for i in range(nargs):
        intype = vtypeToCtype(fnc.fntype.intypes[i])
        code += intype
        if i<(nargs-1):
            code+=','
    code += ');\n'
    wrapname = modname+'_'+fnname+'_wrap'
    code += 'static PyObject * '+wrapname
    code += '(PyObject *self,PyObject *args){\n'
    fmtstring = ''
    for i in range(nargs):
        intype = fnc.fntype.intypes[i]
        code += vtypeToCtype(intype)+' arg'+str(i)+';\n'
        fmtstring += vtypeToFmt(intype)
    if not isVoid:
        code += rtype + ' rval;\n'
    code += 'if(!PyArg_ParseTuple(args,"'+fmtstring+'",'
    for i in range(nargs):
        code += '&arg'+str(i)
        if i<(nargs-1):
            code+=','
    code+=')) return NULL;\n'
    if not isVoid:
        code += 'rval = '
    code += fnname+'('
    for i in range(nargs):
        code += 'arg'+str(i)
        if i<(nargs-1):
            code += ','
    code += ');\n'
    if isVoid:
        code += 'Py_INCREF(Py_None);\n'
        code += 'return Py_None;\n'
    else:
        code += 'return Py_BuildValue("'
        code += vtypeToFmt(fnc.fntype.rtypes[0])
        code += '",rval);\n'
    code += '}\n'
    return code

def compile(modname,funs):
    modXmlNode = XmlNode('module')
    modXmlNode.addAttribute('name',modname)
    ccode = '#include <Python.h>\n'
    methtable = "static PyMethodDef "+modname+"Methods[] = {\n"
    for fun in funs:
        fnc = FnCompiler(fun)
        fnXmlNode = fnc.toXMLNode()
        modXmlNode.addChild(fnXmlNode)
        ccode = ccode + genWrapFunc(modname,fnc)
        fnname = fun.__name__
        fnwrapname = modname+'_'+fnname+'_wrap'
        methtable += '{"'+fnname+'",'+fnwrapname+',METH_VARARGS,"Autogen fn"},\n'
    methtable += '{NULL,NULL,0,NULL}\n };\n'
    ccode += methtable
    ccode += 'static struct PyModuleDef '+modname+'module = {\n'
    ccode += 'PyModuleDef_HEAD_INIT,\n'
    ccode += 'NULL,\n'
    ccode += '-1,\n'
    ccode += modname+'Methods\n };\n'
    ccode += 'PyMODINIT_FUNC\n'
    ccode += 'PyInit_'+modname+'(void){\n'
    ccode += '  return PyModule_Create(&'+modname+'module);\n'
    ccode += '}\n'
    modXml = modXmlNode.toXML()
    modXmlFile = open(modname+'.xml','w')
    modXmlFile.write(modXml)
    modXmlFile.close()
    cfile = open(modname+'_wrap.c','w')
    cfile.write(ccode)
    cfile.close()
    print(ccode)

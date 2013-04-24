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

def PAR(x):
    return x

class Domain:
    def __init__(self,*args):
        self.args = args
    def __iter__(self):
        return self
    def next(self):
        if len(args)==1:
            m = len(args[0])
            for i in range(m):
                yield i
        if len(args)==2:
            m = len(args[0])
            n = len(args[1])
            for i in range(m):
                for j in range(n):
                    yield i,j


def compile(modname,funs):
    modXmlNode = XmlNode('module')
    modXmlNode.addAttribute('name',modname)
    for fun in funs:
        fnc = FnCompiler(fun)
        fnXmlNode = fnc.toXMLNode()
        modXmlNode.addChild(fnXmlNode)
    modXml = modXmlNode.toXML()
    print(modXml)

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

import qdcompile
from qdcompile import PAR,ArrayType,Int32,Float64,VoidType

def fn1(a: 'int32',b: 'int32') -> 'int32':
    c = a+b
    return c

def fn2(a: 'array(float64,1)' ,b: 'array(float64,1)' ,n: 'int32') -> 'void':
    for i in range(n):
        a[i] = b[i]
    return

def fn3(a: 'array(float64,1)' ,b: 'array(float64,1)' ,n: 'int32') -> 'void':
    for i in PAR(range(n)):
        a[i] = b[i]
    return

qdcompile.compile('qdtest2',[fn1,fn2])

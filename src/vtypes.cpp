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

#include "vtypes.hpp"

using namespace VRaptor;

int FuncType::getNumReturns(){
    shared_ptr<VType> firstReturn = m_outtypes[0];
    if(firstReturn->getBasicType()==VType::UNIT_TYPE) return 0;
    else return m_outtypes.size();
}

ScalarTypePtr ScalarType::getInt32Instance(VComplexType v, bool isSigned){
    ScalarTypePtr stype(new ScalarType(ScalarType::SCALAR_INT,32,v,isSigned));
    return stype;
}

shared_ptr<UnitType> UnitType::m_instance;

shared_ptr<UnitType> UnitType::getInstance(){
    if(!m_instance) m_instance.reset(new UnitType);
    return m_instance;
}


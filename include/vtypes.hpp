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

#ifndef VTYPES_HPP_
#define VTYPES_HPP_
#include <vector>
#include <Poco/DOM/Node.h>
#include <Poco/Types.h>

#include <tr1/memory>
using std::tr1::shared_ptr;

namespace VRaptor{

class VType;
typedef shared_ptr<VType> VTypePtr;

class VType{
public:
	enum VBasicType{SCALAR_TYPE, DOMAIN_TYPE, ARRAY_TYPE, UNIT_TYPE, FUNC_TYPE};
	virtual VBasicType getBasicType() const=0;
    virtual ~VType(){}
    static VTypePtr readFromXml(Poco::XML::Node* node);
};

class NType:public VType{

};

class ScalarType;
typedef shared_ptr<ScalarType> ScalarTypePtr;

class ScalarType:public NType{
public:
    enum VComplexType{REAL, COMPLEX, MAYCOMPLEX};
    enum ScalarTag {SCALAR_INT, SCALAR_FLOAT, SCALAR_BOOL};

    ScalarType(ScalarTag tag,Poco::UInt8 bitness, VComplexType v=REAL,bool isSigned = true):m_stag(tag),m_ctype(v),m_bitness(bitness),m_isSigned(isSigned){}
	VBasicType getBasicType() const{return SCALAR_TYPE;}
    VComplexType getComplexStatus() const{return m_ctype;}
    ScalarTag getScalarTag() const{return m_stag;}
    Poco::UInt8 getBitness() const{return m_bitness;}
    static ScalarTypePtr getInt32Instance(VComplexType v=REAL,bool isSigned=true);
    static ScalarTypePtr getInt64Instance(VComplexType v=REAL,bool isSigned=true);
    static ScalarTypePtr getFloat32Instance(VComplexType v=REAL);
    static ScalarTypePtr getFloat64Instance(VComplexType v=REAL);
private:
     VComplexType m_ctype;
     ScalarTag m_stag;
     Poco::UInt8 m_bitness;
     bool m_isSigned;
};


class ArrayType:public NType{
public:
    enum ArrayLayout {ROW_MAJOR, COL_MAJOR, STRIDES};
private:
    ScalarTypePtr m_elem;
	ArrayLayout m_layout;
	int m_ndims;
public:
    ArrayType(ScalarTypePtr elem,int ndims,ArrayLayout layout):m_elem(elem),m_ndims(ndims),m_layout(layout){}
	bool isArrayType() const{return true;}
    ScalarTypePtr getElementType() const{return m_elem;}
	int getNdims() const{return m_ndims;}
	ArrayLayout getLayout() const{return m_layout;}
	VBasicType getBasicType() const{return VType::ARRAY_TYPE;}
};

class UnitType:public VType{
    static shared_ptr<UnitType> m_instance;
	UnitType(){}
	UnitType(const UnitType& u);
public:
	VBasicType getBasicType() const{return UNIT_TYPE;}
    static shared_ptr<UnitType> getInstance();
};

class DomainType:public VType{
	int m_ndims;
public:
	int getNdims() const{return m_ndims;}
	bool isOneDim() const{return m_ndims==1;}
	bool isMultiDim() const{return m_ndims>1;}
	VBasicType getBasicType() const{return DOMAIN_TYPE;}
	DomainType(int ndims):m_ndims(ndims){}
};

class FuncType:public VType{
    std::vector< VTypePtr  > m_intypes;
    std::vector< VTypePtr  > m_outtypes;
public:
    FuncType(std::vector< VTypePtr  > intypes,std::vector< VTypePtr  > outtypes):m_intypes(intypes),m_outtypes(outtypes){}
    static shared_ptr<FuncType> readFromXml(Poco::XML::Node* node);
	VBasicType getBasicType() const{return FUNC_TYPE;}
    VTypePtr  getArgType(int i){return m_intypes[i];}
    VTypePtr  getReturnType(int i){return m_outtypes[i];}
	int getNumArgs(){return m_intypes.size();}
    int getNumReturns();
};
typedef shared_ptr<FuncType> FuncTypePtr;

}


#endif /* VTYPES_HPP_ */

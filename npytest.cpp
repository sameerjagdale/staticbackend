extern "C"{
#include <Python.h>
#include "ndarrayobject.h"
};

#include <cstddef>
#include <iostream>
using namespace std;
int main(){
	cout<<"Offset of data pointer "<<offsetof(PyArrayObject,data)<<endl;
	cout<<"Offset of dimensions pointer "<<offsetof(PyArrayObject,dimensions)<<endl;
	cout<<"Dimensions are stored as "<<sizeof(npy_intp)<<" byte integers"<<endl;
	cout<<"Offset of strides pointer "<<offsetof(PyArrayObject,strides)<<endl;
}


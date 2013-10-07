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

#ifndef __POCOUTILS_H
#define __POCOUTILS_H
#include <Poco/HashSet.h>
#include <Poco/HashMap.h>
#include <Poco/Types.h>
#include <Poco/SharedPtr.h>

template <typename T>
class VRHashSet:public Poco::HashSet<T>{
public:
    void unite(const VRHashSet<T>& other){
        for(typename Poco::HashSet<T>::ConstIterator it=other.begin(); it!= other.end(); it++){
            this->insert(*it);
        }
    }

    void subtract(const VRHashSet<T>& other){
        for(typename Poco::HashSet<T>::ConstIterator it=other.begin(); it!= other.end(); it++){
            this->erase(*it);
        }
    }
    bool contains(const T& value){
        if(find(value)==this->end()) return false;
        return true;
    }

    typedef typename Poco::HashSet<T>::ConstIterator const_iterator;

};

template <typename T>
class VRHashPtr: public Poco::Hash<T*>{
    Poco::Hash<Poco::Int64> hasher;
public:
    std::size_t operator()(const T*& value){
        Poco::Int64 ptrval = static_cast<Poco::UInt64>(value);
        return hasher(ptrval);
    }
};


template <typename T>
struct SharedArray{
private:
    class VRArrayReleasePolicy{
    public:
        static void release(T *ptr){
            delete[] ptr;
        }
    };
public:
    typedef Poco::SharedPtr<T,Poco::ReferenceCounter,VRArrayReleasePolicy> type;
};

#endif


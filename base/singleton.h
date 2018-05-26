/* Licensed to the Apache Software Foundation (ASF) under one or more
* contributor license agreements.  See the NOTICE file distributed with
* this work for additional information regarding copyright ownership.
* The ASF licenses this file to You under the Apache License, Version 2.0
* (the "License"); you may not use this file except in compliance with
* the License.  You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


/*************************************************************************
 * File: singleton.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_SINGLETON_H
#define	BASE_SINGLETON_H

#include "base.h"
#include <memory>

NS(NBPy)

class UnCopyable
{
protected:
    UnCopyable(){};
    ~UnCopyable(){};
private:
    /**   */
    UnCopyable(const UnCopyable& src);
    UnCopyable& operator= ( const UnCopyable& src );
};

template<class T>
class Singleton : private UnCopyable
{
public:
    static T* getInstance()
    {
        if( _instance.get() == NULL ){
            _instance.reset( new T() );
        }
        return _instance.get();
    }
protected:
    Singleton(){};
    virtual ~Singleton(){};

protected:
    friend class std::auto_ptr<T>;
    static std::auto_ptr<T>  _instance;
};

template<class T>
std::auto_ptr<T> Singleton<T>::_instance;

NSE

/********************************************
* Macro define
********************************************/
#define DECLARE_SINGLETON(type) \
	friend class std::auto_ptr<type>;	   \
	friend class Singleton<type>;	  \
	public:                      \
        static type* GetInstance(){	  \
            return Singleton<type>::getInstance();}  \
	protected:                	\
        type(){ Initialize(); }         \
	public:                      \
        virtual ~type() { Destroy(); }

#endif // BASE_SINGLETON_H

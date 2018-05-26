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
 * File: pyextend.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef CONNSVR_PYEXTEND_H
#define CONNSVR_PYEXTEND_H

#include <stacklessengine.h>

NS(NBPy)

    class CPyConnsvr : public CPyClassBase
    {
    PYCLASS_DEF_INTERFACE(CPyConnsvr)
    public:
        CPyConnsvr(IScriptEngine *e){}
        ~CPyConnsvr(){}
        PYCLASS_METHOD_DEF(CPyConnsvr,GetPIDFilePath);
        PYCLASS_METHOD_DEF(CPyConnsvr,Inform);
        PYCLASS_METHOD_DEF(CPyConnsvr,Send);
    private:
    };

    extern void InitExtendModule();
NSE

#endif //CONNSVR_PYEXTEND_H

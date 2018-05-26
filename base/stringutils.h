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
 * File: stringutils.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_STRINGUTILS_H
#define BASE_STRINGUTILS_H

#include "singleton.h"
#include <sstream>

NS(NBPy)

    class CStringUtils
    {
        DECLARE_SINGLETON(CStringUtils)
    public:
        void Initialize(){};
        void Destroy(){};

        template<typename T>
        T ConvertFromStr(std::string s, T def)
        {
            T t = def;
            m_ss.clear();
            m_ss.str("");
            m_ss << s;
            m_ss >> t;
            return t;
        };

        template<typename T>
        std::string &ConvertToStr(T s)
        {
            m_ss.clear();
            m_ss.str("");
            m_ss << s;
            m_ss >> m_t;
            return m_t;
        };

        char *Trim(char *, unsigned int);

    private:
        std::stringstream m_ss;
        std::string m_t;
    };

NSE

#endif //BASE_STRINGUTILS_H

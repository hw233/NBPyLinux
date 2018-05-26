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
 * File: configfile.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_CONFIGFILE_H
#define BASE_CONFIGFILE_H

#include "imcodepackager.h"

NS(NBPy)

    class CConfigFile
    {
    public:
        CConfigFile();
        ~CConfigFile();
        void Close();

        bool LoadFromFile(const char *);
        bool EnterSection(const char *);
        long GetIntValue(const char *, long = 0);
        const char *GetStrValue(const char *, const char * = "");
        size_t EnterListMode(const char *);
        long GetListIntValue(size_t , long = 0);
        const char *GetListStrValue(size_t, const char * = "");
    private:
        const char *cacheString(const char *);
    private:
        CDictPackager m_root;
        CDictPackager m_section;
        CListPackager m_sectionlist;
        bool m_islistmode;
        int m_stringcacheIndex;
        std::map<int, std::string> m_stringcache;
    };

NSE

#endif //BASE_CONFIGFILE_H

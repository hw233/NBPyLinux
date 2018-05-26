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
 * File: configfile.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "configfile.h"
#include "codec_json.h"

NS(NBPy)

    CConfigFile::CConfigFile()
            :m_stringcacheIndex(0)
             ,m_islistmode(false)
    {}

    CConfigFile::~CConfigFile(){
        Close();
    }

    void CConfigFile::Close() {
        m_sectionlist.Clear();
        m_section.Clear();
        m_root.Clear();
    }

    bool CConfigFile::LoadFromFile(const char *path)
    {
        CBuffer buffer;
        char szFileBuf[512];
        FILE *f = fopen(path, "r");
        if(!f)
            return false;

        size_t r = 0;
        do
        {
            r = fread(szFileBuf, 1, 512, f);
            if(r > 0)
                buffer.Push(szFileBuf, r);
        }while(r);
        fclose(f);

        m_root.Clear();
        CJsonCodec::GetInstance()->GetEncoder()->Encode(&m_root, &buffer);
        return true;
    }

    bool CConfigFile::EnterSection(const char *szSectionName)
    {
        m_section.Clear();
        CIMDictCode code;
        size_t s = 0;
        if(!m_root.Get<STR, CIMDictCode *>(szSectionName, &code, s))
            return false;
        m_section.Load(&code);
        return true;
    }

    long CConfigFile::GetIntValue(const char *szKey, long def)
    {
        return m_section.Get<STR,long>(szKey, def);
    }

    const char *CConfigFile::GetStrValue(const char *szKey, const char *def)
    {
        size_t l = 512;
        char szTmp[l];
        memset(szTmp,0,l);
        m_section.Get<STR,STR>(szKey, szTmp, l);
        if(l == 0)
        {
            strcpy(szTmp, def);
        }
        return cacheString(szTmp);
    }

    size_t CConfigFile::EnterListMode(const char *szSectionName)
    {
        CIMListCode code;
        size_t s = 0;
        m_section.Get<STR, CIMListCode *>(szSectionName, &code, s);
        m_sectionlist.Load(&code);
        return m_sectionlist.GetSize();
    }

    long CConfigFile::GetListIntValue(size_t nIndex, long def)
    {
        return m_sectionlist.At<long>(nIndex, def);
    }

    const char *CConfigFile::GetListStrValue(size_t nIndex, const char *def)
    {
        size_t l = 512;
        char szTmp[l];
        memset(szTmp,0,l);
        m_sectionlist.At<STR>(nIndex, szTmp, l);
        if(l == 0)
        {
            strcpy(szTmp, def);
        }
        return cacheString(szTmp);
    }

    const char *CConfigFile::cacheString(const char *s)
    {
        std::string str = s;
        m_stringcache[m_stringcacheIndex] = str;
        std::map<int, std::string>::iterator it = m_stringcache.find(m_stringcacheIndex);
        m_stringcacheIndex ++;
        return it->second.c_str();
    }
NSE
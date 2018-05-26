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
 * File: performance.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "performance.h"
#include "datetime.h"
#include "codec_json.h"

NS(NBPy)

    void CPerformance::Initialize() {
        memset(m_szFName, 0, 32);
        memset(m_szDisName, 0, 255);
        memset(m_szFPath, 0, 255);
    }

    void CPerformance::Destroy() {
        m_collectors.clear();
    }

    void CPerformance::SetName(const char *fname, const char *disname, unsigned long interval) {
        strcpy(m_szDisName, disname);
        strcpy(m_szFName, fname);
        sprintf(m_szFPath, "./performances/%s.pfcs", fname);
        m_interval = interval;
        m_st = 0;
    }

    void CPerformance::AddCollector(IPerformanceCollector *p) {
        m_collectors.push_back(p);
    }

    void CPerformance::Collect() {
        if(!*m_szFPath)
            return;

        if(CDateTime::timestamp() - m_st < m_interval)
            return;

        m_st = CDateTime::timestamp();

        CPerformancePackager packager;
        packager.AddForString("", m_szDisName);

        for (std::vector<IPerformanceCollector *>::iterator p = m_collectors.begin();
				p != m_collectors.end();
				p ++) {
				(*p)->Collect(packager);
        }

        CBuffer bufOut;
        CJsonCodec::GetInstance()->GetDecoder()->Decode(&packager.getPackager(), &bufOut);

        FILE *pf = fopen(m_szFPath, "w");
        if(pf)
        {
            fwrite(bufOut.Begin(), bufOut.GetDataSize(), 1, pf);
            fflush(pf);
            fclose(pf);
        }
    }

	void CPerformancePackager::AddForInt64(const char *name, uint64_t value, PerformanceMode mode){
		CIMDictCode code;
		CDictPackager dict;
		dict.Set<STR, STR>("name",  name);
		dict.Set<STR, int64_t>("data", (uint64_t)value);
		dict.Set<STR, int>("mode", (int)mode);
		dict.Save(&code);
		m_packager.Append<CIMDictCode *>(&code);
	}

	void CPerformancePackager::AddForInt(const char *name, unsigned long value, PerformanceMode mode){
		CIMDictCode code;
		CDictPackager dict;
		dict.Set<STR, STR>("name",  name);
		dict.Set<STR, long>("data", (long)value);
		dict.Set<STR, int>("mode", (int)mode);
		dict.Save(&code);
		m_packager.Append<CIMDictCode *>(&code);
	}

	void CPerformancePackager::AddForString(const char *name, const char *value){
		CIMDictCode code;
		CDictPackager dict;
		dict.Set<STR, STR>("name",  name);
		dict.Set<STR, STR>("data", value);
		dict.Set<STR, int>("mode", (int)PFM_STRING);
		dict.Save(&code);
		m_packager.Append<CIMDictCode *>(&code);
	}

	void CPerformancePackager::AddForList(const char *name, CListPackager &packager, PerformanceMode mode){
		if(mode != PFM_REPORT && mode != PFM_SUBLIST)
			return;

		CIMDictCode code;
		CIMListCode codeList;
		packager.Save(&codeList);
		CDictPackager dict;
		dict.Set<STR, STR>("name",  name);
		dict.Set<STR, CIMListCode *>("data", &codeList);
		dict.Set<STR, int>("mode", (int)mode);
		dict.Save(&code);
		m_packager.Append<CIMDictCode *>(&code);
	}
NSE


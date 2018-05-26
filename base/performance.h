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
 * File: performance.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_PERFORMANCE_H
#define BASE_PERFORMANCE_H

#include "base.h"
#include "singleton.h"
#include "imcodepackager.h"

NS(NBPy)

    enum PerformanceMode{
			PFM_STRING 		= -1,//: 文本直显
			PFM_NUMBER 		= 0, //: 数字直显
			PFM_PERCENT_NUM = 1, //: 百分比数字显示
			PFM_PERCENT_PIC = 2, //: 百分比图形显示
			PFM_PROG_NUM 	= 3, //: 数字进度显示
			PFM_BYTE 		= 4, //: 字节数值显示(源单位byte,自动转换单位，单个单位显示)
			PFM_MUTILBYTE 	= 5, //: 字节数值显示(源单位byte,自动转换单位，多个单位显示)
			PFM_BPS 		= 6, //: 网络速度数值显示(源单位bits/s 自动转换单位)
			PFM_REPORT 		= 7, //: 趋势曲线图
			PFM_SUBLIST		= 8, //: 子列表
    };

    class CPerformancePackager{
	public:
		void AddForInt64(const char *name, uint64_t value, PerformanceMode mode = PFM_NUMBER);
		void AddForInt(const char *name, unsigned long value, PerformanceMode mode = PFM_NUMBER);
		void AddForString(const char *name, const char *value);
		void AddForList(const char *name, CListPackager &packager, PerformanceMode mode = PFM_REPORT);

		CListPackager &getPackager(){ return m_packager; };
	private:
		CListPackager m_packager;
    };

    class IPerformanceCollector {
    public:
        virtual void Collect(CPerformancePackager &) = 0;

    };

    class CPerformance
    {
        DECLARE_SINGLETON(CPerformance)

    public:
        void Initialize();
        void Destroy();
        void SetName(const char *fname, const char *disname, unsigned long interval = 1000);

        void AddCollector(IPerformanceCollector *p);
        void Collect();

    private:
        char m_szDisName[255];
        char m_szFName[32];
        char m_szFPath[255];
        unsigned long m_st;
        unsigned long m_interval;
        std::vector<IPerformanceCollector *> m_collectors;
    };

NSE

#endif //BASE_PERFORMANCE_H

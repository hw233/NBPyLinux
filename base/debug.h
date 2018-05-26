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
 * File: debug.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_DEBUG_H
#define BASE_DEBUG_H

#include "output.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

#define BL_DEBUG_INFO_LIMIT 1024

#if((BL_DEBUG_LEVEL & BL_DEBUG_LEVEL_STATUS) == BL_DEBUG_LEVEL_STATUS)
#define DEBUG_STATUS(f, ...)\
	DEBUG_INFO(NULL, 0, f, ##__VA_ARGS__)
#else
    #define DEBUG_STATUS(s, f, ...)
#endif

#if((BL_DEBUG_LEVEL & BL_DEBUG_LEVEL_WARN) == BL_DEBUG_LEVEL_WARN)
#define DEBUG_WARN(f, ...)\
	DEBUG_INFO("WARN", 0, f, ##__VA_ARGS__)
#else
    #define DEBUG_WARN(f, ...)
#endif

#if((BL_DEBUG_LEVEL & BL_DEBUG_LEVEL_ERROR) == BL_DEBUG_LEVEL_ERROR)
#define DEBUG_ERROR(f, ...)\
	DEBUG_INFO("ERROR", 0, f, ##__VA_ARGS__)
#else
	#define DEBUG_ERROR(f, ...)
#endif

NS(NBPy)

inline void DEBUG_INFO(const char *szFlag, int nFd, const char *f, ...)
{
	char szTmp[BL_DEBUG_INFO_LIMIT];
	char szTmpOut[BL_DEBUG_INFO_LIMIT * 2];
	va_list vl;
	va_start(vl, f);
	vsnprintf(szTmp, BL_DEBUG_INFO_LIMIT - 1, f, vl);
	va_end(vl);

	time_t t = time(NULL);
	tm *tf = localtime(&t);
	if(nFd > 0)
	{
		sprintf(szTmpOut, "[%.4d/%.2d/%.2d %.2d:%.2d:%.2d][%s] (%d) %s\n", tf->tm_year + 1900, tf->tm_mon + 1, tf->tm_mday, tf->tm_hour, tf->tm_min, tf->tm_sec, szFlag, nFd, szTmp);
	}else if(szFlag)
	{
		sprintf(szTmpOut, "[%.4d/%.2d/%.2d %.2d:%.2d:%.2d][%s] %s\n", tf->tm_year + 1900, tf->tm_mon + 1, tf->tm_mday, tf->tm_hour, tf->tm_min, tf->tm_sec, szFlag, szTmp);
	}else
		sprintf(szTmpOut, "[%.4d/%.2d/%.2d %.2d:%.2d:%.2d] %s\n", tf->tm_year + 1900, tf->tm_mon + 1, tf->tm_mday, tf->tm_hour, tf->tm_min, tf->tm_sec, szTmp);
	IOutput *io = COutputSetting::GetInstance()->get_default();
	io->write(szTmpOut, strlen(szTmpOut));
}

inline void print_stacktrace()
{
    int size = 16;
    void * array[16];
    int stack_num = backtrace(array, size);
    char ** stacktrace = backtrace_symbols(array, stack_num);
    DEBUG_STATUS(">>>>>> __STACKTRACE__");
    for (int i = 0; i < stack_num; ++i)
    {
        DEBUG_STATUS(">>> %s", stacktrace[i]);
    }
    DEBUG_STATUS("<<<<<< __STACKTRACE__");
    free(stacktrace);
}

NSE


#endif // BASELIB_DEBUG_H_INCLUDED

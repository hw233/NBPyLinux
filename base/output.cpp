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
 * File: output.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "output.h"

NS(NBPy)

    int CConsoleOutput::write(const char *data, unsigned int size)
    {
        return printf("%s",data);
    }

    const char * CFileOutput::get_file_path(const char *fname, const char *prefix)
    {
        time_t t = time(NULL);
        tm *tf = localtime(&t);
        static char szPath[255];
        static char szPath2[255];
        sprintf(szPath, "%s%s-%.4d-%.2d-%.2d.log", fname, prefix, tf->tm_year + 1900, tf->tm_mon + 1, tf->tm_mday);

        bool has_file = access(szPath, R_OK) == 0;
        bool can_write = access(szPath, W_OK) == 0;

        if(has_file && !can_write)
        {
            sprintf(szPath2, "%s-%d", szPath, getpid());
            return szPath2;
        }else
        {
            if (has_file)
            {
                int cnt = 1;
                sprintf(szPath2, "%s.%d", szPath, cnt);
                while(0 == access(szPath2, R_OK))
                {
                    cnt ++;
                    sprintf(szPath2, "%s.%d", szPath, cnt);
                }
                rename(szPath, szPath2);
                remove(szPath);
            }
        }
        return szPath;
    }

    bool CFileOutput::open(const char *fname, const char *prefix)
    {
        if(m_fd)
            fclose(m_fd);

        time_t t = time(NULL);
        tm *tf = localtime(&t);
        memcpy(&m_tm, tf, sizeof(tm));
        m_fname = fname;
        m_prefix = prefix;
        m_fd = fopen(CFileOutput::get_file_path(fname, prefix), "a");
        return m_fd != NULL;
    }

    int CFileOutput::write(const char *data, unsigned int size)
    {
        time_t t = time(NULL);
        tm *tf = localtime(&t);
        if (tf->tm_year != m_tm.tm_year || tf->tm_mon != m_tm.tm_mon || tf->tm_mday != m_tm.tm_mday)
        {
            m_fd = fopen(CFileOutput::get_file_path(m_fname.c_str(), m_prefix.c_str()), "a");
            memcpy(&m_tm, tf, sizeof(tm));
        }
        if(!m_fd)
            return 0;
        fwrite(data, sizeof(char), size, m_fd);
        fflush(m_fd);
        return 0;
    }

NSE

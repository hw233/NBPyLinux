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
 * File: deamonutils.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_DEAMONUTILS_H
#define BASE_DEAMONUTILS_H

#include "base.h"

NS(NBPy)

    typedef struct __deamon_info {
        int         err;
        pid_t       pid;
    } deamon_info, *p_deamon_info;

    class CDeamonUtils
    {
    public:
        static int Begin();
        static void Complete(int fd, int err = EXIT_SUCCESS);
    };

    class CPIDFileUtils
    {
    public:
        static bool Generate(const char* szFilename);
        static void Remove(const char* szFilename);
    };

NSE

#endif //BASE_DEAMONUTILS_H

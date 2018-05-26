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
 * File: stringutils.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "stringutils.h"

NS(NBPy)
    char *CStringUtils::Trim(char *str, unsigned int size)
    {
        unsigned int n = 0;
        char *dst = NULL;
        do
        {
            if(isprint(*str) && !isspace(*str))
            {
                //if(!dst)
                {
                    dst = str;
                    break;
                }
            }
            str++;
            n++;
        }
        while(n < size);

        if(!dst)
            dst = str;

        char *str_end = str + size - 1;
        do
        {
            if(*str_end != 0 && !isspace(*str_end))
            {
                break;
            }
            else
            {
                *str_end = 0;
            }
            str_end--;
        }
        while(str_end >= dst);
        return dst;
    }
NSE
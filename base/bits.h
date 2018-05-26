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
 * File: bits.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/
#ifndef BASE_BITS_H
#define BASE_BITS_H

#include "base.h"

NS(NBPy)

	static const int magic_pos[32] =
	{
	  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
	  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
	};

    class BitUtils
    {
    public:

        static inline int bit_pos(unsigned int v)
        {
            return magic_pos[((unsigned int)((v & -v) * 0x077CB531U)) >> 27];
        };

        static inline bool is_power(unsigned int v)
        {
            return !((v - 1) & v);
        };

        static inline int sqrt_for_2(unsigned int v)
        {
            return is_power(v) ? bit_pos(v) : -1;
        };

        static inline int align(unsigned int v, unsigned int b)
        {
            return (((v)+ ((b) - 1)) &~((b) - 1));
        };
    };

NSE

#endif // BASE_BITS_H

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
 * File: codec.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_CODEC_H
#define BASE_CODEC_H

#include "imcode.h"

NS(NBPy)

    class IEncoder
    {
    public:
        virtual ~IEncoder() {};
        /// target data >> intermediacode for packager
        virtual bool Encode(IIMCodePackager *, void *) = 0;
        /// target data >> intermediacode
        virtual bool Encode(IIMCode *, void *) = 0;
    };

    class IDecoder
    {
    public:
        virtual ~IDecoder() {};
        /// target data << intermediacode for packager
        virtual bool Decode(IIMCodePackager *, void *) = 0;
        /// target data << intermediacode
        virtual bool Decode(IIMCode *, void *) = 0;
    };

    typedef IEncoder *LPIEncoder;
    typedef IDecoder *LPIDecoder;

    class ICodec
    {
    public:
        virtual LPIEncoder GetEncoder() = 0;
        virtual LPIDecoder GetDecoder() = 0;
    };

NSE

#endif //BASE_CODEC_H

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
 * File: codec_json.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_CODEC_JSON_H
#define BASE_CODEC_JSON_H

#include "codec.h"
#include "singleton.h"

NS(NBPy)

    class CJsonEncoder: public IEncoder{
    public:
        CJsonEncoder();
        ~CJsonEncoder();


        /// IBuffer * >> intermediacode for packager
        bool Encode(IIMCodePackager *packager, void *json);
        /// IBuffer * >> intermediacode
        bool Encode(IIMCode *code, void *json);
    };

    class CJsonDecoder: public IDecoder{
    public:
        CJsonDecoder();
        ~CJsonDecoder();

        /// IBuffer * << intermediacode for packager
        bool Decode(IIMCodePackager *packager, void *json);
        /// IBuffer * << intermediacode
        bool Decode(IIMCode *code, void *json);

    private:
        bool DecodeMapData(IIMCodePackager *packager, void *writer);
        bool DecodeSequenceData(IIMCodePackager *packager, void *writer);
    };

    class CJsonCodec: public ICodec{
        DECLARE_SINGLETON(CJsonCodec)
    public:
        void Initialize();
        void Destroy();
        LPIEncoder GetEncoder();
        LPIDecoder GetDecoder();
    private:
        CJsonEncoder m_encoder;
        CJsonDecoder m_decoder;
    };

NSE

#endif //BASE_CODEC_JSON_H

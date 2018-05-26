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
 * File: codec_py.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_CODEC_PY_H
#define BASE_CODEC_PY_H

#include <Python.h>
#include "codec.h"
#include "singleton.h"

NS(NBPy)

    class CPythonEncoder: public IEncoder{
    public:
        CPythonEncoder();
        ~CPythonEncoder();


        /// PythonObject * >> intermediacode for packager
        bool Encode(IIMCodePackager *packager, void *pyData);
        /// PythonObject * >> intermediacode
        bool Encode(IIMCode *code, void *pyData);

    private:
        bool EncodeMapData(IIMCodePackager *packager, PyObject *pyObject);
        bool EncodeSequenceData(IIMCodePackager *packager, PyObject *pyObject);

    };

    class CPythonDecoder: public IDecoder{
    public:
        CPythonDecoder();
        ~CPythonDecoder();

        /// PythonObject * << intermediacode for packager
        bool Decode(IIMCodePackager *packager, void *pyData);
        /// PythonObject * << intermediacode
        bool Decode(IIMCode *code, void *pyData);

    private:
        bool DecodeMapData(IIMCodePackager *packager, PyObject *pyObject);
        bool DecodeSequenceData(IIMCodePackager *packager, PyObject *pyObject);
    };

    class CPythonCodec: public ICodec{
        DECLARE_SINGLETON(CPythonCodec)
    public:
        void Initialize();
        void Destroy();
        LPIEncoder GetEncoder();
        LPIDecoder GetDecoder();
    private:
        CPythonEncoder m_encoder;
        CPythonDecoder m_decoder;
    };

NSE

#endif //BASE_CODEC_PY_H

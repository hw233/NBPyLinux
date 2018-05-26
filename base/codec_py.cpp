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
 * File: codec_py.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "codec_py.h"
#include "imcodepackager.h"

NS(NBPy)


    void CPythonCodec::Destroy() {

    }

    void CPythonCodec::Initialize() {

    }

    LPIEncoder CPythonCodec::GetEncoder() {
        return &m_encoder;
    }

    LPIDecoder CPythonCodec::GetDecoder() {
        return &m_decoder;
    }

    CPythonEncoder::CPythonEncoder() {}
    CPythonEncoder::~CPythonEncoder() {}

    bool CPythonEncoder::Encode(IIMCodePackager *packager, void *pyData) {
        if(!pyData)
        {
            assert(false && "CPythonEncoder::Encode pyData is NULL");
            return false;
        }

        PyObject *pyObject = (PyObject *)pyData;
        if(PyDict_Check(pyObject))
            return EncodeMapData(packager, pyObject);

        if(PySequence_Check(pyObject))
            return EncodeSequenceData(packager, pyObject);

        return false;
    }

    bool CPythonEncoder::Encode(IIMCode *code, void *pyData) {
        if(!code)
            return false;

        PyObject *pyObject = (PyObject *)pyData;
        if(PyDict_Check(pyObject) && code->GetType() == DTP_MAP)
        {
            CDictPackager packager(code);
            if(Encode(&packager, pyObject))
            {
                packager.Save();
                return true;
            }
            return false;
        }

        if(PySequence_Check(pyObject) && code->GetType() == DTP_LIST)
        {
            CListPackager packager(code);
            if(Encode(&packager, pyObject))
            {
                packager.Save();
                return true;
            }
            return false;
        }

        return false;
    }

    bool CPythonEncoder::EncodeMapData(IIMCodePackager *packager, PyObject *pyObject) {
        if(!PyDict_Check(pyObject))
        {
            assert(false && "CPythonEncoder::EncodeMapData >> pyObject isn't a Python Dict Object");
            return false;
        }

        CDictPackager *pDict = dynamic_cast<CDictPackager *>(packager);
        if(!pDict)
        {
            assert(false && "CPythonEncoder::EncodeMapData >> packager isn't CDictPackager");
            return false;
        }
        pDict->Clear();

        PyObject *key, *val;
        Py_ssize_t s = 0;

        int intKey = 0;
        const char *strKey = NULL;

        while(PyDict_Next(pyObject, &s, &key, &val) != 0) {

            //此处key,val是借来的引用，无需负责decref
            intKey = 0;
            strKey = NULL;

            if(PyInt_Check(key))
            {
                intKey = (int)PyInt_AsLong(key);
            }
            else if(PyString_Check(key))
            {
                strKey = PyString_AsString(key);
                char *c = NULL;
                Py_ssize_t si = 0;
                PyString_AsStringAndSize(key, &c, &si);
            }
            else{
                continue;
            }

            if(val == Py_None)
            {
            	continue;
            }

            if(PyBool_Check(val))
            {
                bool bv = (Py_True == val);
                strKey ? pDict->Set<STR, bool>(strKey, bv) : pDict->Set<int, bool>(intKey, bv);

            }else if(PyInt_Check(val))
            {
                long nv = PyInt_AsLong(val);
                strKey ? pDict->Set<STR, long>(strKey, nv) : pDict->Set<int, long>(intKey, nv);
            }
            else if(PyLong_Check(val))
            {
                long long llv = 0;
                try
                {
                    llv = PyLong_AsLong(val);
                }catch(...)
                {
                    llv = PyLong_AsLongLong(val);
                }
                if(llv >= INT_MIN && llv <= INT_MAX)
                {
                    long nv = llv + 0;
                    strKey ? pDict->Set<STR, long>(strKey, nv) : pDict->Set<int, long>(intKey, nv);
                }else
                {
                    int64_t nv = llv + 0;
                    strKey ? pDict->Set<STR, int64_t>(strKey, nv) : pDict->Set<int, int64_t>(intKey, nv);
                }
            }
            else if(PyFloat_Check(val))
            {
                float dv = PyFloat_AsDouble(val);
                strKey ? pDict->Set<STR, float>(strKey, dv) : pDict->Set<int, float>(intKey, dv);
            }
            else if(PyString_Check(val))
            {
                char *c = NULL;
                Py_ssize_t si = 0;
                PyString_AsStringAndSize(val, &c, &si);
                strKey ? pDict->Set<STR, STR>(strKey, c, si) : pDict->Set<int, STR>(intKey, c, si);
            }
            else if (PyDict_Check(val))
            {
                CIMDictCode code;
                CDictPackager sp(&code);
                if(EncodeMapData(&sp, val))
                {
                    sp.Save();
                    strKey ? pDict->Set<STR, CIMDictCode *>(strKey, &code) : pDict->Set<int, CIMDictCode *>(intKey, &code);
                }
            }
            else if (PySequence_Check(val))
            {
                CIMListCode code;
                CListPackager sp(&code);
                if(EncodeSequenceData(&sp, val))
                {
                    sp.Save();
                    strKey ? pDict->Set<STR, CIMListCode *>(strKey, &code) : pDict->Set<int, CIMListCode *>(intKey, &code);
                }
            }else{
            	//DEBUG_STATUS("no match 203");
            }
        }

		//DEBUG_STATUS("CPythonEncoder::EncodeMapData len: %ld", pDict->GetSize());

        return true;
    }

    bool CPythonEncoder::EncodeSequenceData(IIMCodePackager *packager, PyObject *pyObject) {
        if(!PySequence_Check(pyObject))
        {
            assert(false && "CPythonEncoder::EncodeSequenceData >> pyObject isn't a Python Sequence Object");
            return false;
        }

        CListPackager *pList = dynamic_cast<CListPackager *>(packager);
        if(!pList)
        {
            assert(false && "CPythonEncoder::EncodeSequenceData >> packager isn't CListPackager");
            return false;
        }
        pList->Clear();

        Py_ssize_t se = PySequence_Fast_GET_SIZE(pyObject);
        PyObject *v = NULL;
        for (int i = 0; i < se; i++)
        {
            //此处是借来的引用，无需负责decref
            //!而PySequence_GetItem是新引用，需要负责decref!!!!
            v = PySequence_Fast_GET_ITEM(pyObject, i);

            if(v == Py_None)
            {
            	continue;
            }

            if(PyInt_Check(v))
            {
                long nv = PyInt_AsLong(v);
                pList->Append<long>(nv);
            }
            else if(PyLong_Check(v))
            {
                long long llv = 0;
                try
                {
                    llv = PyLong_AsLong(v);
                }catch(...)
                {
                    llv = PyLong_AsLongLong(v);
                }
                if(llv >= INT_MIN && llv <= INT_MAX)
                {
                    long nv = llv + 0;
                    pList->Append<long>(nv);
                }else
                {
                    int64_t nv = llv + 0;
                    pList->Append<int64_t>(nv);
                }
            }
            else if(PyBool_Check(v))
            {
                bool bv = Py_True == v;
                pList->Append<bool>(bv);
            }
            else if(PyFloat_Check(v))
            {
                double dv = PyFloat_AsDouble(v);
                pList->Append<double>(dv);
            }
            else if(PyString_Check(v))
            {
                char *c = NULL;
                Py_ssize_t si = 0;
                PyString_AsStringAndSize(v, &c, &si);
                pList->Append<STR>(c, si);
            }
            else if (PyDict_Check(v))
            {
                CIMDictCode code;
                CDictPackager sp(&code);
                if(EncodeMapData(&sp, v))
                {
                    sp.Save();
                    pList->Append<CIMDictCode *>(&code);
                }
            }
            else if (PySequence_Check(v))
            {
                CIMListCode code;
                CListPackager sp(&code);
                if(EncodeSequenceData(&sp, v))
                {
                    sp.Save();
                    pList->Append<CIMListCode *>(&code);
                }
            }

            //Py_XDECREF(v);
        }

        return true;
    }

    CPythonDecoder::CPythonDecoder() {}
    CPythonDecoder::~CPythonDecoder() {}

    bool CPythonDecoder::Decode(IIMCodePackager *packager, void *pyData) {

        if(!pyData)
        {
            assert(false && "CPythonDecoder::Decode pyData is NULL");
            return false;
        }

        PyObject *pyObject = (PyObject *)pyData;

        if(packager->GetType() == DTP_MAP)
            return DecodeMapData(packager, pyObject);

        if(packager->GetType() == DTP_LIST)
            return DecodeSequenceData(packager, pyObject);

        return false;
    }

    bool CPythonDecoder::Decode(IIMCode *code, void *pyData) {
        if(!code)
            return false;

        PyObject *pyObject = (PyObject *)pyData;

        if(PyDict_Check(pyObject) && code->GetType() == DTP_MAP)
        {
            CDictPackager packager(code);
            return Decode(&packager, pyData);
        }

        if(PySequence_Check(pyObject) && code->GetType() == DTP_LIST)
        {
            CListPackager packager(code);
            return Decode(&packager, pyData);
        }

        return false;
    }

    bool CPythonDecoder::DecodeMapData(IIMCodePackager *packager, PyObject *pyObject) {
        if(packager->GetType() != DTP_MAP)
        {
            assert(false && "CPythonDecoder::DecodeMapData packager isn't a CDictPackager");
            return false;
        }
        if(!PyDict_Check(pyObject))
        {
            assert(false && "CPythonDecoder::DecodeMapData pyObject isn't a Python dict object");
            return false;
        }

        CDictPackager *pDict = dynamic_cast<CDictPackager *>(packager);
        if(!pDict)
        {
            assert(false && "CPythonEncoder::DecodeMapData >> packager isn't CDictPackager");
            return false;
        }


        for(IMKVITEMS::iterator kv = pDict->GetItems().begin(); kv != pDict->GetItems().end(); kv ++) {
            PyObject *key = NULL;
            PyObject *val = NULL;

            switch (kv->first.tp) {
                case DTP_STR: {
                    key = PyString_FromString(kv->first.str_key);
                }
                    break;
                case DTP_INT: {
                    key = PyInt_FromLong(kv->first.i_key);
                }
                    break;
                default: {
                    continue;
                }
            }

            switch (kv->second->tp) {
                case DTP_NULL: {
                    val = Py_None;
                    Py_INCREF(val);
                }
                    break;
                case DTP_STR: {
                    val = Py_BuildValue("s#", (STR) kv->second->data, kv->second->length);
                }
                    break;
                case DTP_CHAR: {
                    val = Py_BuildValue("c", *(char *) kv->second->data);
                }
                    break;
                case DTP_SHORT: {
                    val = Py_BuildValue("h", *(short *) kv->second->data);
                }
                    break;
                case DTP_INT: {
                    val = Py_BuildValue("i", *(int *) kv->second->data);
                }
                    break;
                case DTP_LONG: {
                    val = Py_BuildValue("l", *(long *) kv->second->data);
                }
                    break;
                case DTP_BOOL: {
                    val = (*(bool *) kv->second->data) ? Py_True : Py_False;
                    Py_INCREF(val);
                }
                    break;
                case DTP_FLOAT: {
                    val = Py_BuildValue("f", *(float *) kv->second->data);
                }
                    break;
                case DTP_INT64: {
                    val = Py_BuildValue("q", *(int64_t *) kv->second->data);
                }
                    break;
                case DTP_DOUBLE: {
                    val = Py_BuildValue("f", *(double *) kv->second->data);
                }
                    break;
                case DTP_MEMORY: {
                    val = Py_BuildValue("s#", (STR) kv->second->data, kv->second->length);
                }
                    break;
                case DTP_PYSTR: {
                    val = Py_BuildValue("s#", (STR) kv->second->data, kv->second->length);
                }
                    break;

                case DTP_MAP: {
                    CIMDictCode code;
                    code.Load(kv->second->data,kv->second->length);
                    CDictPackager sp(&code);
                    val = PyDict_New();
                    if( DecodeMapData(&sp, val) ){
                    }
                }
                    break;
                case DTP_LIST: {
                    CIMListCode code;
                    code.Load(kv->second->data,kv->second->length);
                    CListPackager sp(&code);
                    val = PyList_New(0);
                    if( DecodeSequenceData(&sp, val) ){
                    }
                }
                    break;
                default: {
                    Py_XDECREF(key);
                    continue;
                }
            }

            PyDict_SetItem(pyObject, key, val);

            //此处key,val的引用已经被偷窃，所以自身应该负责decref
            Py_XDECREF(key);
            Py_XDECREF(val);
        }

        return true;
    }

    bool CPythonDecoder::DecodeSequenceData(IIMCodePackager *packager, PyObject *pyObject) {
        if(packager->GetType() != DTP_LIST)
        {
            assert(false && "CPythonDecoder::DecodeSequenceData packager isn't a CListPackager");
            return false;
        }
        if(!PySequence_Check(pyObject))
        {
            assert(false && "CPythonDecoder::DecodeSequenceData pyObject isn't a Python sequence object");
            return false;
        }

        CListPackager *pList = dynamic_cast<CListPackager *>(packager);
        if(!pList)
        {
            assert(false && "CPythonEncoder::DecodeSequenceData >> packager isn't CListPackager");
            return false;
        }

        for(IMLISTITEMS::iterator lv = pList->GetItems().begin(); lv != pList->GetItems().end(); lv ++){
            PyObject *val = NULL;

			LPITEM_DEF v = *lv;

            switch (v->tp) {
                case DTP_NULL: {
                    val = Py_None;
                    Py_INCREF(val);
                }
                    break;
                case DTP_STR: {
                    val = Py_BuildValue("s#", (STR) v->data, v->length);
                }
                    break;
                case DTP_CHAR: {
                    val = Py_BuildValue("c", *(char *) v->data);
                }
                    break;
                case DTP_SHORT: {
                    val = Py_BuildValue("h", *(short *) v->data);
                }
                    break;
                case DTP_INT: {
                    val = Py_BuildValue("i", *(int *) v->data);
                }
                    break;
                case DTP_LONG: {
                    val = Py_BuildValue("l", *(long *) v->data);
                }
                    break;
                case DTP_BOOL: {
                    val = (*(bool *) v->data) ? Py_True : Py_False;
                    Py_INCREF(val);
                }
                    break;
                case DTP_FLOAT: {
                    val = Py_BuildValue("f", *(float *) v->data);
                }
                    break;
                case DTP_INT64: {
                    val = Py_BuildValue("q", *(int64_t *) v->data);
                }
                    break;
                case DTP_DOUBLE: {
                    val = Py_BuildValue("f", *(double *) v->data);
                }
                    break;
                case DTP_MEMORY: {
                    val = Py_BuildValue("s#", (STR) v->data, v->length);
                }
                    break;
                case DTP_PYSTR: {
                    val = Py_BuildValue("s#", (STR) v->data, v->length);
                }
                    break;
                case DTP_MAP: {
                    CIMDictCode code;
                    code.Load(v->data,v->length);
                    CDictPackager sp(&code);
                    val = PyDict_New();
                    if( DecodeMapData(&sp, val) ){
                    }
                }
                    break;
                case DTP_LIST: {
                    CIMListCode code;
                    code.Load(v->data,v->length);
                    CListPackager sp(&code);
                    val = PyList_New(0);
                    if( DecodeSequenceData(&sp, val) ){
                    }
                }
                    break;
                default: {
                    continue;
                }
            }

            PyList_Append(pyObject, val);
            //此处val的引用已经被偷窃，所以自身应该负责decref
            Py_XDECREF(val);
        }

        return true;
    }

NSE

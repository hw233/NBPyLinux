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
 * File: codec_json.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "pool.h"
#include "codec_json.h"
#include "imcodepackager.h"
#include <rapidjson/reader.h>
#include <rapidjson/writer.h>
#include <iostream>

using namespace rapidjson;
using namespace std;


NS(NBPy)

    struct PackagerLinker {
        PackagerLinker *parent;
        IIMCodePackager *packager;
        std::string currentKey;
    };

    struct JsonReadHandler : public BaseReaderHandler<UTF8<>, JsonReadHandler> {

        JsonReadHandler(IIMCodePackager *packager){
            m_packager = packager;
            m_linker = NULL;
        }

        bool Null() { return true; }
        bool StartObject() {
            PackagerLinker *linker = (PackagerLinker *)CSimplePool::GetInstance()->Alloc(sizeof(PackagerLinker));
            linker = new(linker) PackagerLinker();
            linker->parent = m_linker;
            if(m_linker)
            {
                CDictPackager *p = (CDictPackager *)CSimplePool::GetInstance()->Alloc(sizeof(CDictPackager));
                p = new(p) CDictPackager();
                linker->packager = p;
            } else
                linker->packager = m_packager;

            m_linker = linker;
            return true;
        }
        bool Key(const char* str, SizeType length, bool copy) {
            m_linker->currentKey = str;
            //DEBUG_STATUS("KEY >> %s", str);
            return true;
        }
        bool EndObject(SizeType memberCount) {

            if(!m_linker)
                return false;
            if(m_linker->packager->GetType() != DTP_MAP)
                return false;

            CIMDictCode code;
            m_linker->packager->Save(&code);
            if(m_linker->packager != m_packager)
            {
                ((CDictPackager *)m_linker->packager)->~CDictPackager();
                CSimplePool::GetInstance()->Free(m_linker->packager);
            }
            void *pr = m_linker;
            m_linker = m_linker->parent;

            ((PackagerLinker *)pr)->~PackagerLinker();
            CSimplePool::GetInstance()->Free(pr);

            if(!m_linker)
                return true;

            if(m_linker->packager->GetType() == DTP_MAP)
            {
                CDictPackager *pp = (CDictPackager *)m_linker->packager;
                pp->Set<STR, CIMDictCode *>(m_linker->currentKey.c_str(),&code);
            }
            if(m_linker->packager->GetType() == DTP_LIST)
            {
                CListPackager *pp = (CListPackager *)m_linker->packager;
                pp->Append<CIMDictCode *>(&code);
            }

            return true;
        }
        bool StartArray() {
            if(!m_linker)
            {
                assert(false && "JsonReadHandler::StartArray >> Datasource's root must be is a map-object!");
                return false;
            }
            PackagerLinker *linker = (PackagerLinker *)CSimplePool::GetInstance()->Alloc(sizeof(PackagerLinker));
            linker = new(linker) PackagerLinker();
            linker->parent = m_linker;
            if(m_linker)
            {
                CListPackager *p = (CListPackager *)CSimplePool::GetInstance()->Alloc(sizeof(CListPackager));
                p = new(p) CListPackager();
                linker->packager = p;
            } else
                linker->packager = m_packager;

            m_linker = linker;
            return true;
        }
        bool EndArray(SizeType elementCount) {

            if(!m_linker)
                return false;
            if(m_linker->packager->GetType() != DTP_LIST)
                return false;

            CIMListCode code;
            m_linker->packager->Save(&code);
            if(m_linker->packager != m_packager)
            {
                ((CListPackager *)m_linker->packager)->~CListPackager();
                CSimplePool::GetInstance()->Free(m_linker->packager);
            }
            void *pr = m_linker;
            m_linker = m_linker->parent;

            ((PackagerLinker *)pr)->~PackagerLinker();
            CSimplePool::GetInstance()->Free(pr);

            if(!m_linker)
                return true;

            if(m_linker->packager->GetType() == DTP_MAP)
            {
                CDictPackager *pp = (CDictPackager *)m_linker->packager;
                pp->Set<STR, CIMListCode *>(m_linker->currentKey.c_str(),&code);
            }
            if(m_linker->packager->GetType() == DTP_LIST)
            {
                CListPackager *pp = (CListPackager *)m_linker->packager;
                pp->Append<CIMListCode *>(&code);
            }

            return true;
        }

        bool Bool(bool b) {
            if(m_linker->packager->GetType() == DTP_MAP)
            {
                CDictPackager *pp = (CDictPackager *)m_linker->packager;
                pp->Set<STR, bool>(m_linker->currentKey.c_str(),b);
            }
            if(m_linker->packager->GetType() == DTP_LIST)
            {
                CListPackager *pp = (CListPackager *)m_linker->packager;
                pp->Append<bool>(b);
            }
            return true;
        }
        bool Int(int i) {
            if(m_linker->packager->GetType() == DTP_MAP)
            {
                CDictPackager *pp = (CDictPackager *)m_linker->packager;
                pp->Set<STR, int>(m_linker->currentKey.c_str(),i);
            }
            if(m_linker->packager->GetType() == DTP_LIST)
            {
                CListPackager *pp = (CListPackager *)m_linker->packager;
                pp->Append<int>(i);
            }
            return true;
        }
        bool Uint(unsigned u) {
            if(m_linker->packager->GetType() == DTP_MAP)
            {
                CDictPackager *pp = (CDictPackager *)m_linker->packager;
                pp->Set<STR, long>(m_linker->currentKey.c_str(),u);
            }
            if(m_linker->packager->GetType() == DTP_LIST)
            {
                CListPackager *pp = (CListPackager *)m_linker->packager;
                pp->Append<long>(u);
            }
            return true;
        }
        bool Int64(int64_t i) {
            if(m_linker->packager->GetType() == DTP_MAP)
            {
                CDictPackager *pp = (CDictPackager *)m_linker->packager;
                pp->Set<STR, int64_t>(m_linker->currentKey.c_str(),i);
            }
            if(m_linker->packager->GetType() == DTP_LIST)
            {
                CListPackager *pp = (CListPackager *)m_linker->packager;
                pp->Append<int64_t>(i);
            }
            return true;
        }
        bool Uint64(uint64_t u) {
            if(m_linker->packager->GetType() == DTP_MAP)
            {
                CDictPackager *pp = (CDictPackager *)m_linker->packager;
                pp->Set<STR, int64_t>(m_linker->currentKey.c_str(),u);
            }
            if(m_linker->packager->GetType() == DTP_LIST)
            {
                CListPackager *pp = (CListPackager *)m_linker->packager;
                pp->Append<int64_t>(u);
            }
            return true;
        }
        bool Double(double d) {
            if(m_linker->packager->GetType() == DTP_MAP)
            {
                CDictPackager *pp = (CDictPackager *)m_linker->packager;
                pp->Set<STR, float>(m_linker->currentKey.c_str(),d);
            }
            if(m_linker->packager->GetType() == DTP_LIST)
            {
                CListPackager *pp = (CListPackager *)m_linker->packager;
                pp->Append<float>(d);
            }
            return true;
        }

        bool String(const char* str, SizeType length, bool copy) {
            //DEBUG_STATUS("String >> %s", str);
            if(m_linker->packager->GetType() == DTP_MAP)
            {
                CDictPackager *pp = (CDictPackager *)m_linker->packager;
                pp->Set<STR, STR>(m_linker->currentKey.c_str(),str, length);
            }
            if(m_linker->packager->GetType() == DTP_LIST)
            {
                CListPackager *pp = (CListPackager *)m_linker->packager;
                pp->Append<STR>(str, length);
            }
            return true;
        }

    private:
        IIMCodePackager *m_packager;
        PackagerLinker *m_linker;
    };

    void CJsonCodec::Destroy() {

    }

    void CJsonCodec::Initialize() {

    }

    LPIEncoder CJsonCodec::GetEncoder() {
        return &m_encoder;
    }

    LPIDecoder CJsonCodec::GetDecoder() {
        return &m_decoder;
    }

    CJsonEncoder::CJsonEncoder() {

    }

    CJsonEncoder::~CJsonEncoder() {}

    bool CJsonEncoder::Encode(IIMCodePackager *packager, void *json) {
        if(!json)
        {
            assert(false && "CJsonEncoder::Encode json is NULL");
            return false;
        }

        IBuffer *jsonBuffer = (IBuffer *)json;

        JsonReadHandler handler(packager);
        Reader reader;
        StringStream ss((const char *)jsonBuffer->Begin());
        reader.Parse<kParseCommentsFlag|kParseTrailingCommasFlag>(ss, handler);
        //DEBUG_STATUS("ERRCODE: %d, OFFSET: %d", reader.GetParseErrorCode(), reader.GetErrorOffset());
        return true;
    }

    bool CJsonEncoder::Encode(IIMCode *code, void *json) {
        if(!code)
            return false;

        CDictPackager packager(code);
        if(Encode(&packager, json))
        {
            packager.Save();
            return true;
        }

        return false;
    }

    CJsonDecoder::CJsonDecoder() {}
    CJsonDecoder::~CJsonDecoder() {}

    bool CJsonDecoder::Decode(IIMCodePackager *packager, void *json)
    {
        if(!json)
        {
            assert(false && "CJsonEncoder::Decode json is NULL");
            return false;
        }

        IBuffer *jsonBuffer = (IBuffer *)json;

        StringBuffer sb;
        Writer<StringBuffer> writer(sb);

        if(packager->GetType() == DTP_MAP)
        {
            DecodeMapData(packager, &writer);
        }

        if(packager->GetType() == DTP_LIST)
        {
            DecodeSequenceData(packager, &writer);
        }

        jsonBuffer->Push(sb.GetString(), sb.GetLength());

        return true;
    }

    bool CJsonDecoder::Decode(IIMCode *code, void *json)
    {
        if(!code)
            return false;

        if(code->GetType() == DTP_MAP)
        {
            CDictPackager packager(code);
            if(Decode(&packager, json))
            {
                return true;
            }
        }else{
            CListPackager packager(code);
            if(Decode(&packager, json))
            {
                return true;
            }
        }

        return false;
    }

    bool CJsonDecoder::DecodeMapData(IIMCodePackager *packager, void *writer){
        Writer<StringBuffer> *pWriter = (Writer<StringBuffer> *)writer;
        CDictPackager *pDict = (CDictPackager *)packager;

        char szTmpKey[255];
        pWriter->StartObject();

        for(IMKVITEMS::iterator kv = pDict->GetItems().begin(); kv != pDict->GetItems().end(); kv ++) {
            memset(szTmpKey,0,255);
            if(kv->first.tp == DTP_INT)
            {
                sprintf(szTmpKey, "%d", kv->first.i_key);
            } else
                strcpy(szTmpKey, kv->first.str_key);

            pWriter->Key(szTmpKey);

            switch (kv->second->tp)
            {
                case DTP_NULL:
                {
                    pWriter->Null();
                }break;
                case DTP_MAP:
                {
                    CIMDictCode code;
                    code.Load(kv->second->data, kv->second->length);
                    CDictPackager pp(&code);
                    DecodeMapData(&pp, pWriter);
                    continue;
                }
                case DTP_LIST:
                {
                    CIMListCode code;
                    code.Load(kv->second->data, kv->second->length);
                    CListPackager pp(&code);
                    DecodeSequenceData(&pp, pWriter);
                    continue;
                }
                case DTP_CHAR:
                {
                    int v = *((char *)kv->second->data);
                    pWriter->Int(v);
                }break;
                case DTP_SHORT:
                {
                    int v = *((short *)kv->second->data);
                    pWriter->Int(v);
                }break;
                case DTP_INT:
                {
                    int v = *((int *)kv->second->data);
                    pWriter->Int(v);
                }break;
                case DTP_LONG:
                {
                    long v = *((long *)kv->second->data);
                    pWriter->Int(v);
                }break;
                case DTP_BOOL:
                {
                    bool v = *((bool *)kv->second->data);
                    pWriter->Bool(v);
                }break;
                case DTP_INT64:
                {
                    int64_t v = *((int64_t *)kv->second->data);
                    pWriter->Int64(v);
                }break;
                case DTP_FLOAT:
                {
                    float v = *((float *)kv->second->data);
                    pWriter->Double(v);
                }break;
                case DTP_DOUBLE:
                {
                    double v = *((double *)kv->second->data);
                    pWriter->Double(v);
                }break;
                case DTP_STR:
                case DTP_PYSTR:
                {
                    pWriter->String((const char *)kv->second->data, kv->second->length, true);
                }break;
                default:
                {
                    pWriter->String("[MEMORY]");
                }
            }
        }

        pWriter->EndObject();

        return true;
    }

    bool CJsonDecoder::DecodeSequenceData(IIMCodePackager *packager, void *writer){
        Writer<StringBuffer> *pWriter = (Writer<StringBuffer> *)writer;
        CListPackager *pList = (CListPackager *)packager;

        pWriter->StartArray();

        for(IMLISTITEMS::iterator lv = pList->GetItems().begin(); lv != pList->GetItems().end(); lv ++){
        	LPITEM_DEF kv = *lv;
            switch (kv->tp)
            {
                case DTP_NULL:
                {
                    pWriter->Null();
                }break;
                case DTP_MAP:
                {
                    CIMDictCode code;
                    code.Load(kv->data, kv->length);
                    CDictPackager pp(&code);
                    DecodeMapData(&pp, pWriter);
                    continue;
                }
                case DTP_LIST:
                {
                    CIMListCode code;
                    code.Load(kv->data, kv->length);
                    CListPackager pp(&code);
                    DecodeSequenceData(&pp, pWriter);
                    continue;
                }
                case DTP_CHAR:
                {
                    int v = *((char *)kv->data);
                    pWriter->Int(v);
                }break;
                case DTP_SHORT:
                {
                    int v = *((short *)kv->data);
                    pWriter->Int(v);
                }break;
                case DTP_INT:
                {
                    int v = *((int *)kv->data);
                    pWriter->Int(v);
                }break;
                case DTP_LONG:
                {
                    long v = *((long *)kv->data);
                    pWriter->Int(v);
                }break;
                case DTP_BOOL:
                {
                    bool v = *((bool *)kv->data);
                    pWriter->Bool(v);
                }break;
                case DTP_INT64:
                {
                    int64_t v = *((int64_t *)kv->data);
                    pWriter->Int64(v);
                }break;
                case DTP_FLOAT:
                {
                    float v = *((float *)kv->data);
                    pWriter->Double(v);
                }break;
                case DTP_DOUBLE:
                {
                    double v = *((double *)kv->data);
                    pWriter->Double(v);
                }break;
                case DTP_STR:
                case DTP_PYSTR:
                {
                    pWriter->String((const char *)kv->data, kv->length, true);
                }break;
                default:
                {
                    pWriter->String("[MEMORY]");
                }
            }
        }

        pWriter->EndArray();

        return true;
    }
NSE

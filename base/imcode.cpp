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
 * File: imcode.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "imcode.h"
#include "memory.h"
#include "debug.h"

NS(NBPy)

    CIMCode::CIMCode(DATA_TP tp) {
        m_invalid = true;
        if(tp != DTP_MAP && tp != DTP_LIST)
        {
            return;
        }
        m_invalid = false;
        m_tp = tp;
        Reset();
    }

    bool CIMCode::IsInvalid() {
        return m_invalid;
    }

    void CIMCode::SetSize(unsigned int size) {
        LPDATA_HEADER p = (LPDATA_HEADER)m_buffer.Begin();
        p->size = size;
    }

    void CIMCode::IncSize() {
        LPDATA_HEADER p = (LPDATA_HEADER)m_buffer.Begin();
        p->size += 1;
    }

    void CIMCode::AddItem(LPITEM_DEF item) {
        DATA_HEADER header;
        header.tp = (char)item->tp;
        header.size = item->length;

        LPDATA_HEADER pHeader = (LPDATA_HEADER)m_buffer.Push(&header, sizeof(header));
        switch(item->tp)
        {
            case DTP_BOOL:{
                pHeader->size = sizeof(bool);
                m_buffer.Push(item->data, sizeof(bool));
            }break;
            case DTP_CHAR:{
                pHeader->size = sizeof(char);
                m_buffer.Push(item->data, sizeof(char));
            }break;
            case DTP_DOUBLE:{
                pHeader->size = sizeof(double);
                m_buffer.Push(item->data, sizeof(double));
            }break;
            case DTP_FLOAT:{
                pHeader->size = sizeof(float);
                m_buffer.Push(item->data, sizeof(float));
            }break;
            case DTP_INT:{
                pHeader->size = sizeof(int);
                m_buffer.Push(item->data, sizeof(int));
            }break;
            case DTP_INT64:{
                pHeader->size = sizeof(int64_t);
                m_buffer.Push(item->data, sizeof(int64_t));
            }break;
            case DTP_LONG:{
                pHeader->size = sizeof(long);
                m_buffer.Push(item->data, sizeof(long));
            }break;
            case DTP_MEMORY:{
                m_buffer.Push(item->data, item->length);
            }break;
            case DTP_NULL:{
            }break;
            case DTP_PBUFFER:{
            }break;
            case DTP_PYSTR:{
                m_buffer.Push(item->data, item->length);
            }break;
            case DTP_STR:{
                m_buffer.Push(item->data, item->length);
            }break;
            case DTP_SHORT:{
                pHeader->size = sizeof(short);
                m_buffer.Push(item->data, sizeof(short));
            }break;
            case DTP_BUFFER:{
                IBuffer *buffer = dynamic_cast<IBuffer *>((CBuffer *)item->data);
                if(buffer)
                {
                    pHeader->size = (unsigned int)buffer->GetDataSize();
                    m_buffer.Push(buffer->Begin(), buffer->GetDataSize());
                }
            }break;
            case DTP_MAP:
            case DTP_LIST:{
                pHeader->size = item->length;
                m_buffer.Push(item->data, item->length);
            }break;
            default:{}
        }

        IncSize();
    }

    void CIMCode::_ADDKVItem(int keyTp, void *pKey, size_t keySize, LPITEM_DEF item){
        KEYVALUE_DATA_HEADER header;
        header.key.tp = (char)keyTp;
        header.key.size = (unsigned int)keySize;
        LPKEYVALUE_DATA_HEADER pHeader = (LPKEYVALUE_DATA_HEADER)m_buffer.Push(&header, sizeof(header));

        pHeader->value.tp = (char)item->tp;
        pHeader->value.size = item->length;

        switch(item->tp)
        {
            case DTP_BOOL:{
                pHeader->value.size = sizeof(bool);
            }break;
            case DTP_CHAR:{
                pHeader->value.size = sizeof(char);
            }break;
            case DTP_DOUBLE:{
                pHeader->value.size = sizeof(double);
            }break;
            case DTP_FLOAT:{
                pHeader->value.size = sizeof(float);
            }break;
            case DTP_INT:{
                pHeader->value.size = sizeof(int);
            }break;
            case DTP_INT64:{
                pHeader->value.size = sizeof(int64_t);
            }break;
            case DTP_LONG:{
                pHeader->value.size = sizeof(long);
            }break;
            case DTP_SHORT:{
                pHeader->value.size = sizeof(short);
            }break;
            case DTP_BUFFER:{
                IBuffer *buffer = dynamic_cast<IBuffer *>((CBuffer *)item->data);
                if(buffer)
                {
                    pHeader->value.size = (unsigned int)buffer->GetDataSize();
                }
            }break;
            case DTP_MAP:
            case DTP_LIST:{
                pHeader->value.size = item->length;
            }break;
            default:{}
        }

        m_buffer.Push(pKey, keySize);

        switch(item->tp)
        {
            case DTP_BOOL:{
                m_buffer.Push(item->data, sizeof(bool));
            }break;
            case DTP_CHAR:{
                m_buffer.Push(item->data, sizeof(char));
            }break;
            case DTP_DOUBLE:{
                m_buffer.Push(item->data, sizeof(double));
            }break;
            case DTP_FLOAT:{
                m_buffer.Push(item->data, sizeof(float));
            }break;
            case DTP_INT:{
                m_buffer.Push(item->data, sizeof(int));
            }break;
            case DTP_INT64:{
                m_buffer.Push(item->data, sizeof(int64_t));
            }break;
            case DTP_LONG:{
                m_buffer.Push(item->data, sizeof(long));
            }break;
            case DTP_MEMORY:{
                m_buffer.Push(item->data, item->length);
            }break;
            case DTP_NULL:{
            	DEBUG_STATUS("is NULL!!!!!!");
            }break;
            case DTP_PBUFFER:{
            }break;
            case DTP_PYSTR:{
                m_buffer.Push(item->data, item->length);
            }break;
            case DTP_STR:{
                m_buffer.Push(item->data, item->length);
            }break;
            case DTP_SHORT:{
                m_buffer.Push(item->data, sizeof(short));
            }break;
            case DTP_BUFFER:{
                IBuffer *buffer = dynamic_cast<IBuffer *>((CBuffer *)item->data);
                if(buffer)
                {
                    m_buffer.Push(buffer->Begin(), buffer->GetDataSize());
                }
            }break;
            case DTP_MAP:
            case DTP_LIST:{
                m_buffer.Push(item->data, item->length);
            }break;
            default:{}
        }

        IncSize();
    }

    void CIMCode::AddItem(int key, LPITEM_DEF item) {
        _ADDKVItem(DTP_INT, &key, sizeof(int), item);
    }

    void CIMCode::AddItem(const char *key, LPITEM_DEF item) {
        _ADDKVItem(DTP_STR, (void *)key, strlen(key), item);
    }

    size_t CIMCode::Dump(IBuffer *pBuffer) {
        pBuffer->Push(m_buffer.Begin(), m_buffer.GetDataSize());
        return m_buffer.GetDataSize();
    }

    void CIMCode::Load(IBuffer *pBuffer) {
        m_buffer.Reset();
        m_buffer.Push(pBuffer->Begin(), pBuffer->GetDataSize());
    }

    void CIMCode::Load(const void *p, size_t s) {
        m_buffer.Reset();
        m_buffer.Push(p, s);
    }

    size_t CIMCode::GetLength() {
        return m_buffer.GetDataSize();
    }

    DATA_TP CIMCode::GetType() {
        return m_tp;
    }

    void CIMCode::Reset() {
        DATA_HEADER header;
        header.tp = (char)m_tp;
        header.size = 0;
        m_buffer.Reset();
        m_buffer.Push(&header, sizeof(header));
    }

    IBuffer *CIMCode::GetRawBuffer() {
        return &m_buffer;
    }
NSE

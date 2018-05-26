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
 * File: imcodepackager.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "imcodepackager.h"
#include "memory.h"
#include "pool.h"

NS(NBPy)


    CDictPackager::CDictPackager(){
        m_src = NULL;
    }

    CDictPackager::CDictPackager(IIMCode *src) {
        Load(src);
    }

    void CDictPackager::Load(IIMCode *src) {
        CIMCode *pCode = dynamic_cast<CIMCode *>(src);
        m_src = pCode;
        CBuffer buffer;
        pCode->Dump(&buffer);
		Load(&buffer);
    }

    void CDictPackager::Load(IBuffer *buffer) {
        Clear();

        size_t ss = buffer->GetDataSize();
        LPDATA_HEADER header = (LPDATA_HEADER)buffer->Pop(sizeof(DATA_HEADER));
        if(!header || header->tp != DTP_MAP)
        {
            return;
        }


        for(int i = 0; i < header->size; i ++){
            if(buffer->GetDataSize() < sizeof(KEYVALUE_DATA_HEADER))
            {
                break;
            }
            LPKEYVALUE_DATA_HEADER itemheader = (LPKEYVALUE_DATA_HEADER)buffer->Pop(sizeof(KEYVALUE_DATA_HEADER));

            HASH_KEY key;
            key.tp = itemheader->key.tp;
            const void *pKeyData = buffer->Pop(itemheader->key.size);
            if(key.tp == DTP_STR)
            {
                memset(key.str_key,0,255);
                memcpy(key.str_key,pKeyData,itemheader->key.size);
            }else{
                key.i_key = *((int *)pKeyData);
            }

			if(itemheader->value.tp == DTP_NULL)
			{
				ss = ss - buffer->GetDataSize() - sizeof(KEYVALUE_DATA_HEADER) - itemheader->key.size;
				//DEBUG_STATUS("key >> %d << %d", key.i_key, ss);
				continue;
			}

            size_t datasize = itemheader->value.size;
            //DEBUG_STATUS("datasize >> %ld", datasize);
            if(itemheader->value.tp != DTP_MAP && itemheader->value.tp != DTP_LIST && datasize > 10240){
				DEBUG_STATUS("!!!!!!!!!!!极度可疑的字符串长度: %ld for %d", datasize, key.i_key);
				assert(false && "ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
            }

            LPITEM_DEF item = (LPITEM_DEF)CSimplePool::GetInstance()->Alloc(sizeof(ITEM_DEF) + datasize);
            item->tp = itemheader->value.tp;
            item->length = (unsigned int)datasize;
            item->data = CMemory<char>::MovePtr(item, sizeof(ITEM_DEF));
            const void *pp = buffer->Pop(datasize);
            if(!pp){
				DEBUG_STATUS("!!!!!!!!!!!!!!!!!!!!!!!!!!!CDictPackager::Load pp is NULL >> datasize %ld , buf size: %ld", datasize, buffer->GetDataSize());
				DEBUG_STATUS("!!!!!!!!!!!key : %d, val-tp: %d", key.i_key, itemheader->value.tp);
				assert(false && "ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
				Clear();
				return;
            }
            memcpy(item->data, pp, datasize);

            m_items[key] = item;
        };
    }

    CDictPackager::~CDictPackager(){
        Clear();
    }

    void CDictPackager::Clear() {
        for(IMKVITEMS::iterator kv = m_items.begin(); kv != m_items.end(); kv ++)
        {
            CSimplePool::GetInstance()->Free(kv->second);
        }
        m_items.clear();
    }

    void CDictPackager::Save(IIMCode *target) {
        CIMCode *t = dynamic_cast<CIMCode *>(m_src);
        if(target)
            t = dynamic_cast<CIMCode *>(target);

        if(!t)
            return;

        t->Reset();

        for(IMKVITEMS::iterator kv = m_items.begin(); kv != m_items.end(); kv ++) {
            if(kv->first.tp == DTP_STR)
            {
                t->AddItem((const char *)kv->first.str_key,kv->second);
            }else
                t->AddItem(kv->first.i_key,kv->second);
        }
    }

    void CDictPackager::SetValue(DATA_TP keyTp, void *pKey, DATA_TP valTp, void *pVal, size_t valSize, size_t tpSize) {
        if(keyTp != DTP_STR && keyTp != DTP_INT)
        {
            assert(false && "CDictPackager::SetValue TK error");
            return;
        }
        if(valTp == DTP_NULL)
        {
            return;
        }

        HASH_KEY _key;
        _key.tp = keyTp;
        if(_key.tp == DTP_STR)
        {
            memset(_key.str_key, 0, 255);
            strcpy(_key.str_key, *((const char **)pKey));
        } else
            _key.i_key = *((int *)pKey);

        void *pOld = NULL;
        IMKVITEMS::iterator kv = m_items.find(_key);
        if(kv != m_items.end())
            pOld = kv->second;

        size_t vsize = valSize;
        CBuffer tmpBuffer;
        switch (valTp)
        {
            case DTP_STR:
            {
                if(vsize == 0) {
                    const char *s = *((const char **) pVal);
                    vsize = strlen(s);
                }
            }
            case DTP_PYSTR:
            case DTP_MEMORY:
            {
                tmpBuffer.Push(*((void **)pVal), vsize);
            }break;
            case DTP_MAP:
            case DTP_LIST:
            {
                CIMCode *code = dynamic_cast<CIMCode *>(*((IIMCode **)pVal));
                if(code)
                    code->Dump(&tmpBuffer);

                vsize = tmpBuffer.GetDataSize();
            }break;
            default:
            {
                vsize = tpSize;
                tmpBuffer.Push(pVal, vsize);
            }
        }


        LPITEM_DEF item = (LPITEM_DEF)CSimplePool::GetInstance()->Alloc(sizeof(ITEM_DEF) + vsize);
        item->tp = valTp;
        item->length = (unsigned int)vsize;
        item->data = CMemory<char>::MovePtr(item, sizeof(ITEM_DEF));

        memcpy(item->data, tmpBuffer.Begin(), tmpBuffer.GetDataSize());


        m_items[_key] = item;

        if(pOld) {
            CSimplePool::GetInstance()->Free(pOld);
        }
    }

    bool CDictPackager::GetValue(DATA_TP keyTp, void *pKey, DATA_TP valTp, void *pVal, size_t &valSize) {
        if(keyTp != DTP_STR && keyTp != DTP_INT)
        {
            assert(false && "CDictPackager::GetValue TK error");
            return false;
        }
        if(valTp == DTP_NULL)
        {
            return false;
        }

        HASH_KEY _key;
        _key.tp = keyTp;
        if(_key.tp == DTP_STR)
        {
            memset(_key.str_key, 0, 255);
            strcpy(_key.str_key,*((const char **)pKey));
        } else
            _key.i_key = *((int *)pKey);

        valSize = 0;

        IMKVITEMS::iterator kv = m_items.find(_key);
        if(kv == m_items.end())
            return false;

        if(valTp != kv->second->tp)
        {
        	if( (valTp == DTP_INT || valTp == DTP_LONG) && (kv->second->tp == DTP_INT || kv->second->tp == DTP_LONG) )
        	{}else
        	{
				DEBUG_STATUS("CDictPackager::GetValue value-TYPE cannot match!value-TYPE is %d FOR KEY: %d", kv->second->tp, *((int *)pKey));
				assert(false && "FUCK!!");
				return false;
        	}
        }

        switch (kv->second->tp)
        {
            case DTP_STR:
            case DTP_PYSTR:
            {
                void *p = *((void **)pVal);
                memcpy(p, kv->second->data, kv->second->length);
            }break;
            case DTP_MEMORY:
            {
                void *p = *((void **)pVal);
                memcpy(p, kv->second->data, kv->second->length);
            }break;
            case DTP_MAP:
            case DTP_LIST:
            {
                CIMCode *code = dynamic_cast<CIMCode *>(*((IIMCode **)pVal));
                if(code)
                {
                    code->Load(kv->second->data,kv->second->length);
                }
            }break;
            case DTP_INT:
            {
                if(valTp == DTP_INT){
					memcpy(pVal, kv->second->data, kv->second->length);
                }else{
					int *iv = (int *)kv->second->data;
					long *lv = (long *)pVal;
					*lv = *iv - 0;
                }

            }break;
            case DTP_LONG:
            {
                if(valTp == DTP_LONG){
					memcpy(pVal, kv->second->data, kv->second->length);
                }else{
					long *iv = (long *)kv->second->data;
					int *lv = (int *)pVal;
					*lv = *iv - 0;
                }
            }break;
            default:
            {
                memcpy(pVal, kv->second->data, kv->second->length);
            }
        }

        valSize = kv->second->length;
        return true;
    }

    void CDictPackager::DelValue(DATA_TP keyTp, void *pKey) {
        if(keyTp != DTP_STR && keyTp != DTP_INT)
        {
            assert(false && "CDictPackager::Remove TK error");
            return;
        }
        HASH_KEY _key;
        _key.tp = keyTp;
        if(_key.tp == DTP_STR)
        {
            memset(_key.str_key,0,255);
            strcpy(_key.str_key,*((const char **)pKey));
        } else
            _key.i_key = *((int *)pKey);

        IMKVITEMS::iterator kv = m_items.find(_key);
        if(kv == m_items.end())
            return;

        if(kv->second)
            CSimplePool::GetInstance()->Free(kv->second);
        m_items.erase(_key);
    }

    bool CDictPackager::HasValue(DATA_TP keyTp, void *pKey) {
        if(keyTp != DTP_STR && keyTp != DTP_INT)
        {
            assert(false && "CDictPackager::HasKey TK error");
            return false;
        }
        HASH_KEY _key;
        _key.tp = keyTp;
        if(_key.tp == DTP_STR)
        {
            memset(_key.str_key,0,255);
            strcpy(_key.str_key,*((const char **)pKey));
        } else
            _key.i_key = *((int *)pKey);

        IMKVITEMS::iterator kv = m_items.find(_key);
        return (kv != m_items.end());
    }

    CListPackager::CListPackager(){
        m_src = NULL;
    }

    CListPackager::CListPackager(IIMCode *src) {
        Load(src);
    }

    void CListPackager::Load(IIMCode *src) {
        CIMCode *pCode = dynamic_cast<CIMCode *>(src);
        m_src = pCode;
        CBuffer buffer;
        pCode->Dump(&buffer);
        Load(&buffer);
    }

    void CListPackager::Load(IBuffer *buffer) {
        Clear();

        LPDATA_HEADER header = (LPDATA_HEADER)buffer->Pop(sizeof(DATA_HEADER));
        if(!header || header->tp != DTP_LIST)
            return;

        for(int i = 0; i < header->size; i ++){
            if(buffer->GetDataSize() < sizeof(DATA_HEADER))
                break;
            LPDATA_HEADER itemheader = (LPDATA_HEADER)buffer->Pop(sizeof(DATA_HEADER));
			if(itemheader->tp == DTP_NULL)
				break;

            size_t datasize = itemheader->size;
            if(itemheader->tp != DTP_MAP && itemheader->tp != DTP_LIST && datasize > 10240){
				DEBUG_ERROR("!!!!!!!!!!!极度可疑的数据长度: %ld for %d", datasize, i);
				assert(false && "ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
            }


            LPITEM_DEF item = (LPITEM_DEF)CSimplePool::GetInstance()->Alloc(sizeof(ITEM_DEF) + datasize);
            item->tp = itemheader->tp;
            item->length = (unsigned int)datasize;
            item->data = CMemory<char>::MovePtr(item, sizeof(ITEM_DEF));
            const void *pp = buffer->Pop(datasize);
            if(!pp){
				DEBUG_ERROR("!!!!!!!!!!!!!!!!!!!!!!!!!!!CDictPackager::Load pp is NULL >> datasize %ld , buf size: %ld", datasize, buffer->GetDataSize());
				DEBUG_ERROR("!!!!!!!!!!!index : %d, val-tp: %d", i, itemheader->tp);
				Clear();
				return;
            }
            memcpy(item->data, pp, datasize);

            m_items.push_back(item);
        };
    }

    CListPackager::~CListPackager() {
        Clear();
    }

    void CListPackager::Clear() {
        for(IMLISTITEMS::iterator lv = m_items.begin(); lv != m_items.end(); lv ++)
        {
            CSimplePool::GetInstance()->Free(*lv);
        }
        m_items.clear();
    }

    void CListPackager::Save(IIMCode *target) {
        CIMCode *t = dynamic_cast<CIMCode *>(m_src);
        if(target)
            t = dynamic_cast<CIMCode *>(target);

        if(!t)
            return;

        t->Reset();

        for(IMLISTITEMS::iterator lv = m_items.begin(); lv != m_items.end(); lv ++)
        {
            t->AddItem(*lv);
        }
    }

    void CListPackager::Remove(size_t index) {
        if(index >= m_items.size())
            return;

        IMLISTITEMS::iterator it = m_items.begin();
        it += index;
        LPITEM_DEF item = *it;
        m_items.erase(it);
        CSimplePool::GetInstance()->Free(item);
    }

    void CListPackager::AppendValue(DATA_TP valTp, void *pVal, size_t valSize, size_t tpSize) {
        if(valTp == DTP_NULL)
        {
            return;
        }
        size_t vsize = valSize;
        CBuffer tmpBuffer;
        switch (valTp)
        {
            case DTP_STR:
            {
                if(vsize == 0) {
                    const char *s = *((const char **) pVal);
                    vsize = strlen(s);
                }
            }
            case DTP_PYSTR:
            case DTP_MEMORY:
            {
                tmpBuffer.Push(*((void **)pVal), vsize);
            }break;
            case DTP_MAP:
            case DTP_LIST:
            {
                CIMCode *code = dynamic_cast<CIMCode *>(*((IIMCode **)pVal));
                if(code)
                    code->Dump(&tmpBuffer);

                vsize = tmpBuffer.GetDataSize();
            }break;
            default:
            {
                vsize = tpSize;
                tmpBuffer.Push(pVal, vsize);
            }
        }

        LPITEM_DEF item = (LPITEM_DEF)CSimplePool::GetInstance()->Alloc(sizeof(ITEM_DEF) + vsize);
        item->tp = valTp;
        item->length = (unsigned int)vsize;
        item->data = CMemory<char>::MovePtr(item, sizeof(ITEM_DEF));
        memcpy(item->data, tmpBuffer.Begin(), tmpBuffer.GetDataSize());

        m_items.push_back(item);
    }

    void CListPackager::GetValue(size_t index, DATA_TP valTp, void *pVal, size_t &valSize) {

		if(valTp == DTP_NULL)
        {
            return;
        }

        valSize = 0;

        if(index >= m_items.size())
            return;

        LPITEM_DEF kv = m_items.at(index);

        if(valTp != kv->tp)
        {
        	if( (valTp == DTP_INT || valTp == DTP_LONG) && (kv->tp == DTP_INT || kv->tp == DTP_LONG) )
        	{}else
        	{
				DEBUG_STATUS("CListPackager::GetValue value-TYPE cannot match!value-TYPE is %d", kv->tp);
				assert(false && "FUCK!!!!!!!!");
				return;
        	}
        }

        switch (kv->tp)
        {
            case DTP_STR:
            case DTP_PYSTR:
            {
                void *p = *((void **)pVal);
                memcpy(p, kv->data, kv->length);
            }break;
            case DTP_MEMORY:
            {
                void *p = *((void **)pVal);
                memcpy(p, kv->data, kv->length);
            }break;
            case DTP_MAP:
            case DTP_LIST:
            {
                CIMCode *code = dynamic_cast<CIMCode *>(*((IIMCode **)pVal));
                if(code)
                {
                    code->Load(kv->data,kv->length);
                }
            }break;
            case DTP_INT:
            {
                if(valTp == DTP_INT){
					memcpy(pVal, kv->data, kv->length);
                }else{
					int *iv = (int *)kv->data;
					long *lv = (long *)pVal;
					*lv = *iv - 0;
                }

            }break;
            case DTP_LONG:
            {
                if(valTp == DTP_LONG){
					memcpy(pVal, kv->data, kv->length);
                }else{
					long *iv = (long *)kv->data;
					int *lv = (int *)pVal;
					*lv = *iv - 0;
                }
            }break;
            default:
            {
                memcpy(pVal, kv->data, kv->length);
            }
        }

        valSize = kv->length;
    }

    void CDictPackager::PrintData(std::string &strDst, int seq) {
    }

    void CListPackager::PrintData(std::string &strDst, int seq) {
    }

NSE

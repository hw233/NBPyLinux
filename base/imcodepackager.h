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
 * File: imcodepackager.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_IMCODEPACKAGER_H
#define BASE_IMCODEPACKAGER_H

#include "imcode.h"
#include "debug.h"
#include <assert.h>

NS(NBPy)

	typedef NBPy_UNORDERED_MAP<HASH_KEY, LPITEM_DEF, HASH> IMKVITEMS;
	typedef std::vector<LPITEM_DEF> IMLISTITEMS;

    class CDictPackager;
    class CListPackager;

    template <typename T>
    DATA_TP GetDataMapType()
    {
        if(typeid(T) == typeid(char)) return DTP_CHAR;
        if(typeid(T) == typeid(short)) return DTP_SHORT;
        if(typeid(T) == typeid(int)) return DTP_INT;
        if(typeid(T) == typeid(long)) return DTP_LONG;
        if(typeid(T) == typeid(int64_t)) return DTP_INT64;
        if(typeid(T) == typeid(float)) return DTP_FLOAT;
        if(typeid(T) == typeid(bool)) return DTP_BOOL;
        if(typeid(T) == typeid(double)) return DTP_DOUBLE;
        if(typeid(T) == typeid(STR)) return DTP_STR;
        if(typeid(T) == typeid(CIMDictCode *)) return DTP_MAP;
        if(typeid(T) == typeid(CIMListCode *)) return DTP_LIST;
        if(typeid(T) == typeid(LSTR)) return DTP_STR;
        if(typeid(T) == typeid(void *)) return DTP_MEMORY;
        return DTP_NULL;
    };

    class CDictPackager: public IIMCodePackager{
    public:
        CDictPackager();
        CDictPackager(IIMCode *src);
        ~CDictPackager();

        void Load(IIMCode *src);
        void Load(IBuffer *srcBuffer);
        void Save(IIMCode *target = NULL);

        template <typename TK, typename VK>
        void Set(TK key, VK val, size_t size = 0)
        {
            DATA_TP ktp = GetDataMapType<TK>();
            DATA_TP tp = GetDataMapType<VK>();
            if(ktp == DTP_NULL || tp == DTP_NULL)
            {
            	//DEBUG_STATUS("key %d type is error", key);
            	return;
            }
            SetValue(ktp, &key, tp, &val, size, sizeof(VK));
        };

        template <typename TK>
        void Remove(TK key){
            DATA_TP ktp = GetDataMapType<TK>();
            DelValue(ktp, &key);
        }

        template <typename TK, typename VK>
        bool Get(TK key, VK pVal, size_t &size){
            DATA_TP ktp = GetDataMapType<TK>();
            DATA_TP tp = GetDataMapType<VK>();
            if(tp != DTP_STR && tp != DTP_PYSTR && tp != DTP_MEMORY && tp != DTP_MAP && tp != DTP_LIST)
            {
                assert(false && "CDictPackager::Get1 VK error!must in (char *,void *,CIMDictCode *,CIMListCode *)");
                return false;
            }
            return GetValue(ktp, &key, tp, &pVal, size);
        };

        template <typename TK, typename VK>
        VK Get(TK key, VK def = 0){
            DATA_TP ktp = GetDataMapType<TK>();
            DATA_TP tp = GetDataMapType<VK>();
            if(tp != DTP_STR && tp != DTP_PYSTR && tp != DTP_MEMORY && tp != DTP_MAP && tp != DTP_LIST)
            {
            }else{
                assert(false && "CDictPackager::Get2 VK error!must in (char,short,int,long,int64_t,bool,float,double)");
                return def;
            }
            VK v = def;
            size_t s = sizeof(VK);
            bool f = GetValue(ktp, &key, tp, &v, s);
            if(!f)
                v = def;
            return v;
        };

        template <typename TK>
        bool HasKey(TK key){
            DATA_TP ktp = GetDataMapType<TK>();
            return HasValue(ktp, &key);
        };

        void Clear();
        void Alone(){ m_src = NULL; }

        size_t GetSize(){ return m_items.size(); }
        DATA_TP GetType(){ return DTP_MAP; };

        IMKVITEMS& GetItems(){ return m_items; };

        void PrintData(std::string &strDst, int seq = 0);

    private:
        bool GetValue(DATA_TP keyTp, void *pKey, DATA_TP valTp, void *pVal, size_t& valSize);
        void SetValue(DATA_TP keyTp, void *pKey, DATA_TP valTp, void *pVal, size_t valSize, size_t tpSize);
        void DelValue(DATA_TP keyTp, void *pKey);
        bool HasValue(DATA_TP keyTp, void *pKey);

    private:
        IIMCode *m_src;
        IMKVITEMS m_items;
    };

    class CListPackager: public IIMCodePackager {
    public:
        CListPackager();
        CListPackager(IIMCode *src);
        ~CListPackager();

        void Load(IIMCode *src);
        void Load(IBuffer *srcBuffer);
        void Save(IIMCode *target = NULL);

        template <typename VK>
        void Append(VK val, size_t size = 0)
        {
            DATA_TP tp = GetDataMapType<VK>();
            AppendValue(tp,&val,size,sizeof(VK));
        }

        template <typename VK>
        VK At(size_t index, VK v, size_t &size)
        {
            DATA_TP tp = GetDataMapType<VK>();
            if(tp != DTP_STR && tp != DTP_PYSTR && tp != DTP_MEMORY && tp != DTP_MAP && tp != DTP_LIST)
            {
                assert(false && "CListPackager::At1 VK error!must in (char *,void *,CIMDictCode *,CIMListCode *)");
                return NULL;
            }
            if(m_items.size() <= index)
                return NULL;

            GetValue(index, tp, &v, size);
            return v;
        }

        template <typename VK>
        VK At(size_t index, VK def = 0)
        {
            DATA_TP tp = GetDataMapType<VK>();
            if(tp != DTP_STR && tp != DTP_PYSTR && tp != DTP_MEMORY && tp != DTP_MAP && tp != DTP_LIST)
            {
            }else{
                assert(false && "CListPackager::At2 VK error!must in (char,short,int,long,int64_t,bool,float,double)");
                return def;
            }
            if(m_items.size() <= index)
                return def;

            VK v = def;
            size_t s = 0;
            GetValue(index, tp, &v, s);
            return v;
        }

        void Remove(size_t index);

        void Clear();
        void Alone(){ m_src = NULL; }

        size_t GetSize(){ return m_items.size(); }
        DATA_TP GetType(){ return DTP_LIST; };

        IMLISTITEMS& GetItems(){ return m_items; };

        void PrintData(std::string &strDst, int seq = 0);
    private:
        void GetValue(size_t index, DATA_TP valTp, void *pVal, size_t& valSize);
        void AppendValue(DATA_TP valTp, void *pVal, size_t valSize, size_t tpSize);

    private:
        IIMCode *m_src;
        IMLISTITEMS m_items;
    };

NSE

#endif //BASE_IMCODEPACKAGER_H

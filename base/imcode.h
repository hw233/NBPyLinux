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

#ifndef BASE_IMCODE_H
#define BASE_IMCODE_H

#include "buffer.h"
#include <string.h>
#include <typeinfo>

NS(NBPy)

    typedef const char *STR;
    typedef char *LSTR;
    inline size_t __stl_hash_string(const char* __s)
    {
        unsigned long __h = 0;
        for ( ; *__s; ++__s)
            __h = 5*__h + tolower(*__s);

        return size_t(__h);
    }

    enum DATA_TP
    {
        DTP_NULL	= 0,
        DTP_INT		= 1,
        DTP_INT64	= 2,
        DTP_STR		= 3,
        DTP_FLOAT	= 4,
        DTP_BOOL	= 5,
        DTP_DOUBLE	= 6,
        DTP_MAP		= 7,
        DTP_LIST	= 8,
        DTP_BUFFER	= 9,
        DTP_PBUFFER	= 10,
        DTP_MEMORY	= 11,
        DTP_PYSTR	= 12,
        DTP_LONG	= 13,
        DTP_SHORT	= 14,
        DTP_CHAR    = 15
    };

#pragma pack(1)

    typedef struct __DATA_HEADER
    {
        char tp;
        unsigned int size;

    } DATA_HEADER, *LPDATA_HEADER;

    typedef struct __KEYVALUE_DATA_HEADER
    {
        DATA_HEADER key;
        DATA_HEADER value;
    } KEYVALUE_DATA_HEADER, *LPKEYVALUE_DATA_HEADER;

    typedef struct __DATA_NODE
    {
        DATA_HEADER header;
        const void *data;
        int buffer_offset;

    } DATA_NODE, *LPDATA_NODE;

    typedef struct __KEYVALUE_DATA_NODE
    {
        KEYVALUE_DATA_HEADER header;
        const void *data;
        int buffer_offset;

    } KEYVALUE_DATA_NODE, *LPKEYVALUE_DATA_NODE;

    typedef struct __HASH_KEY
    {
        char tp;
        int i_key;
        char str_key[255];

        bool operator==(const __HASH_KEY& __x) const
        {
            if (__x.tp != tp)
                return false;
            switch (tp)
            {
                case DTP_STR:
                    return __stl_hash_string(__x.str_key) == __stl_hash_string(str_key);
                default:
                    return __x.i_key == i_key;
            }
        }
    } HASH_KEY, *LPHASH_KEY;

    struct HASH
    {
        size_t operator()(const HASH_KEY& key) const
        {
            switch (key.tp)
            {
                case DTP_STR:
                    return __stl_hash_string(key.str_key);
                default:
                    return static_cast<size_t>(key.i_key);
            }
        }
    };

    typedef struct __ITEM_DEF
    {
        int tp;
        void *data;
        unsigned int length;

    } ITEM_DEF, *LPITEM_DEF;

#pragma pack()


    class IIMCode {
    public:
        virtual void Load(IBuffer *) = 0;
        virtual void Load(const void *, size_t) = 0;
        virtual size_t Dump(IBuffer *) = 0;
        virtual size_t GetLength() = 0;
        virtual DATA_TP GetType() = 0;
        virtual IBuffer *GetRawBuffer() = 0;
        virtual void Reset() = 0;
    };

    class IIMCodePackager {
    public:
        virtual void Load(IIMCode *src) = 0;
        virtual void Save(IIMCode *target) = 0;
        virtual DATA_TP GetType() = 0;
    };

    class CIMCode: public IIMCode {
    public:
        CIMCode(DATA_TP tp);
        void AddItem(LPITEM_DEF item);
        void AddItem(int key, LPITEM_DEF item);
        void AddItem(const char *key, LPITEM_DEF item);
        bool IsInvalid();
        void Load(IBuffer *pBuffer);
        void Load(const void *p, size_t s);
        size_t Dump(IBuffer *pBuffer);
        void Reset();

        size_t GetLength();
        DATA_TP GetType();
        IBuffer *GetRawBuffer();

    private:
        void _ADDKVItem(int keyTp, void *pKey, size_t keySize, LPITEM_DEF item);
        void SetSize(unsigned int size);
        void IncSize();

    private:
        CBuffer m_buffer;
        bool m_invalid;
        DATA_TP m_tp;
    };

    class CIMDictCode: public CIMCode {
    public:
        CIMDictCode():CIMCode(DTP_MAP){}
    };

    class CIMListCode: public CIMCode {
    public:
        CIMListCode():CIMCode(DTP_LIST){}
    };
NSE


#endif //BASE_IMCODE_H

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
 * File: pool.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_POOL_H
#define BASE_POOL_H

#include "base.h"
#include "singleton.h"
#include "performance.h"

#define CHUNK_FLAG_FREE 0
#define CHUNK_FLAG_USED 1

#define CHUNK_CHECKSUM 0xE38E38E

#define GET_CHUNK_FROM_AVAILABLE_ADDR(p) ((pchunk)(((char *)p) - sizeof(chunk)))
#define GET_CHUNK_AVAILABLE_ADDR(ck) (((char *)ck) + sizeof(chunk))
#define GET_CHUNK_AVAILABLE_SIZE(ck) ( (2<<((int)ck->index)) )
#define GET_EXCHUNK_AVAILABLE_SIZE(ck) ( ((int)ck->index) )
#define GET_CHUNK_SIZE(index) ((2<<((int)index)) + sizeof(chunk))
#define GET_EXCHUNK_SIZE(size) (size + sizeof(chunk))

NS(NBPy)

#pragma pack(1)
    // chunk|memory|chunk|memory...
    // memory's addr = chunk's addr + sizeof(chunk)
    typedef struct __chunk
    {
        unsigned flag 		: 1;
        unsigned checksum 	: 31;
        unsigned index 		: 32;// chunk's index, if extend, index is the chunk real-size
        __chunk *next;		// next chunk
        __chunk **pprev;		// `next`'s pointer for prev-chunk
    }chunk, *pchunk;

    typedef struct __pool
    {
        int si;				// chunks index start
        int ei;				// chunks index end
        unsigned int min;	// min chunk size
        unsigned int max;	// max chunk size
        unsigned int checksum;
        unsigned long free;
        unsigned long used;
        pchunk chunks[32];
        pchunk extended;
    } pool, *ppool;
#pragma pack()


    namespace memory_pool
    {
        int find_chunk_index(ppool p, unsigned int size);

        void *alloc_from_system(ppool p, unsigned int size);

        pchunk create_chunk(ppool p, int index);

        void *pop_available_chunk(ppool p, int index, unsigned int size);

        extern ppool create(unsigned int min = 0x10, unsigned int max = 0x80000000);
        extern void destroy(ppool p);
        extern void *alloc(ppool p, unsigned int &size, bool &from_system);
        extern unsigned int release(ppool p, void *ptr, bool &to_system);
        extern void *expand(ppool p, void *data, unsigned int src_size, unsigned int size);
        extern void clear(ppool p);
        extern const char *print_memory_table(ppool p);

		extern unsigned long shrink(ppool p);
        //extern void pop_chunk(pchunk ck, unsigned int index);
        //extern void insert_chunk(pchunk ck, unsigned int index);
    };

    class CSimplePool: public IPerformanceCollector
    {
        DECLARE_SINGLETON(CSimplePool)
    public:
        void Initialize();
        void Destroy();

        void *Alloc(size_t);
        void Free(void *);
        void *Resize(void *, size_t, size_t);

        unsigned long Shrink();

        short GetUsePercent();

        void Lookup();

        void Collect(CPerformancePackager &pack);
    private:
        ppool m_pool;
        unsigned long m_alloc_from_system;
        unsigned long m_free_to_system;
        unsigned long m_alloc_from_pool;
        unsigned long m_free_to_pool;
        unsigned long m_alloctimes_from_pool;
        unsigned long m_freetimes_to_pool;
        unsigned long m_pool_used_low_st;
    };

NSE

#endif //BASE_POOL_H

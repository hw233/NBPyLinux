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
 * File: pool.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "pool.h"
#include "bits.h"
#include "datetime.h"
#include "debug.h"
#include <iostream>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

NS(NBPy)

inline void pop_chunk(ppool p, pchunk ck, unsigned int index)
{
    if(ck->next)
    {
        ck->next->pprev = NULL;
    }
    p->chunks[index] = ck->next;
    ck->flag = CHUNK_FLAG_USED;
}

inline void insert_chunk(ppool p, pchunk ck, unsigned int index)
{
    ck->next = p->chunks[index];
    if(ck->next)
        ck->next->pprev = &ck;
    p->chunks[index] = ck;
    ck->pprev = NULL;
    ck->flag = CHUNK_FLAG_FREE;
}

inline void set_free_chunk(ppool p, pchunk ck, unsigned int index)
{
    ck->checksum = p->checksum;
    ck->flag = CHUNK_FLAG_FREE;
    ck->index = index;
}

inline void set_used_chunk(ppool p, pchunk ck, unsigned int index)
{
    ck->checksum = p->checksum;
    ck->flag = CHUNK_FLAG_USED;
    ck->index = index;
}

ppool memory_pool::create(unsigned int min, unsigned int max)
{
    int si = BitUtils::sqrt_for_2(min);
    int ei = BitUtils::sqrt_for_2(max);
    if(si < 0 || ei < 0)
        return NULL;
    ppool ptr = reinterpret_cast<ppool>(::malloc(sizeof(pool)));
    ::memset(ptr, 0, sizeof(pool));
    ptr->min 	= min;
    ptr->max 	= max;
    ptr->si		= si;
    ptr->ei		= ei;
    ptr->checksum = (unsigned int)CDateTime::timestamp() % 0x7FFFFFFF;

    return ptr;
};

void memory_pool::destroy(ppool p)
{
    if(p)
        ::free(p);
};

int memory_pool::find_chunk_index(ppool p, unsigned int size)
{
    int n = p->si;
    while(static_cast<unsigned int>(2 << n) < size && n <= p->ei)
    {
        n ++;
    }
    return n <= p->ei ? n : -1;
};

void *memory_pool::alloc_from_system(ppool p, unsigned int size)
{
    if(size >= INT_MAX)
    {
        DEBUG_ERROR("Alloc memory size not in range, alloc size: %d", size);
        return NULL;
    }
    pchunk ck = reinterpret_cast<pchunk>(::malloc(GET_EXCHUNK_SIZE(size)));
    if(!ck)	return NULL;
    set_used_chunk(p, ck, size);
    return GET_CHUNK_AVAILABLE_ADDR(ck);
}

pchunk memory_pool::create_chunk(ppool p, int index)
{
    pchunk ck = reinterpret_cast<pchunk>(::malloc(GET_CHUNK_SIZE(index)));
    if(!ck)
    {
        return NULL;
    }
    set_used_chunk(p, ck, index);
    p->free += GET_CHUNK_AVAILABLE_SIZE(ck);
    return ck;
}

void *memory_pool::pop_available_chunk(ppool p, int index, unsigned int size)
{
    pchunk ck = p->chunks[index];
    if(ck)
    {
        pop_chunk(p, ck, index);
    }else
        ck = create_chunk(p, index);
    if(!ck)
    {
        return NULL;
    }
    return GET_CHUNK_AVAILABLE_ADDR(ck);
}

void *memory_pool::alloc(ppool p, unsigned int &size, bool &from_system)
{
    if(size >= INT_MAX)
    {
    	print_stacktrace();
        DEBUG_STATUS("Alloc memory size not in range, alloc size: %ld", size);
        size = 0;
        return NULL;
    }
    int index = find_chunk_index(p, size);
    from_system = true;
    if(index < p->si || index > p->ei){
    	print_stacktrace();
        DEBUG_STATUS("Alloc memory from system, alloc size: %ld", size);
		assert(false && "memory_pool::alloc from system!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        return alloc_from_system(p, size);
    }

    from_system = false;
    void *ptr = pop_available_chunk(p, index, size);
    if(!ptr)
    {
        from_system = true;
        return alloc_from_system(p, size);
        //DEBUG_STATUS("memory_pool::pop_available_chunk fail. index %d, size %d", index, size);
    }
    //printf("alloc addr: %p\n", ptr);
    pchunk ck = GET_CHUNK_FROM_AVAILABLE_ADDR(ptr);
    size = GET_CHUNK_AVAILABLE_SIZE(ck);

    p->used += size;
    return ptr;
};

unsigned int memory_pool::release(ppool p, void *ptr, bool &to_system)
{
    if(!ptr)
        return 0;
    pchunk ck = GET_CHUNK_FROM_AVAILABLE_ADDR(ptr);
    if(ck->checksum != p->checksum)
    {
        //no pool memory free!!!!
        print_stacktrace();
        DEBUG_ERROR("no pool memory free!!!! checksum: %d, ptr's checksum: %d, ptr: %p, chunk addr: %p", CHUNK_CHECKSUM, ck->checksum, ptr, ck);
        //::free(ptr);
        return 0;
    }
    if(ck->flag == CHUNK_FLAG_FREE)
    {
        DEBUG_ERROR("Retry free pool memory, chunk addr: %p", ck);
        //print_stacktrace();
        return 0;
    }

    unsigned int size = GET_CHUNK_AVAILABLE_SIZE(ck);

    ck->flag = CHUNK_FLAG_FREE;
    int t_index = ck->index;
    to_system = false;
    if(t_index < p->si || t_index > p->ei)
    {
        size = GET_EXCHUNK_AVAILABLE_SIZE(ck);
        to_system = true;
        ::free(ck);
        return size;
    }

    insert_chunk(p, ck, t_index);

    if(p->used > 0)
        p->used -= size;

    return size;
}

void *memory_pool::expand(ppool p, void *data, unsigned int src_size, unsigned int size)
{
    int index = find_chunk_index(p, size);
    pchunk ck = GET_CHUNK_FROM_AVAILABLE_ADDR(data);
    if(ck->index == (unsigned int)index)
        return data;

    void *ptr = (index < p->si || index > p->ei) ? alloc_from_system(p, size) : pop_available_chunk(p, index, size);
    if(!ptr)
    {
        ptr = alloc_from_system(p, size);
        DEBUG_STATUS("memory_pool::pop_available_chunk fail. index %d, size %d", index, size);
    }

    memcpy(ptr, data, src_size);
    //ool b = false;
    //release(p, data, b);
    return ptr;
}

void memory_pool::clear(ppool p)
{
    for(int i = 0; i < 32; i ++)
    {
        pchunk ck = p->chunks[i];
        if(!ck)	continue;
        while(ck)
        {
            pchunk _ck = ck;
            ck = ck->next;
            pop_chunk(p, _ck, i);
            ::free(_ck);
        }
    }
}

unsigned long memory_pool::shrink(ppool p){
	unsigned long l = 0;
    for(int i = 0; i < 32; i ++)
    {
        pchunk ck = p->chunks[i];
        if(!ck)	continue;
        while(ck)
        {
            pchunk _ck = ck;
            ck = ck->next;
            if(_ck->flag == CHUNK_FLAG_FREE)
            {
            	l += GET_CHUNK_AVAILABLE_SIZE(_ck);
				pop_chunk(p, _ck, i);
				::free(_ck);
            }
        }
    }
    p->free -= l;
    return l;
}

const char *memory_pool::print_memory_table(ppool p)
{
    static std::string strOut = "";
    strOut = "";
    char szTmp[255];
    char szTmp2[255];
    const char *szFmt = "{\"name\":\"CHUNK-%d\", \"data\":%d, \"mode\":%d}%s";
    const char *szFmt2 = "[\"%p\", %d]%s";
    const char *szexFmt = "{\"name\":\"EXCHUNK-%d\", \"data\":%d, \"mode\":0}%s";

    std::string strSub = "";
    for(int i = p->si; i <= p->ei; i ++)
    {
        strSub = "";
        pchunk ck = p->chunks[i];
        if(!ck)	continue;
        int c = 0;
        while(ck)
        {
            sprintf(szTmp2, szFmt2, reinterpret_cast<unsigned long>(GET_CHUNK_AVAILABLE_ADDR(ck)),GET_CHUNK_AVAILABLE_SIZE(ck), ck->next ? "," : "");
            strSub += szTmp2;

            ck = ck->next;
            c ++;
        }

        //DEBUG_STATUS(strSub.c_str());

        sprintf(szTmp, szFmt, i, c, 0, ",");
        strOut += szTmp;
    }
    //DEBUG_STATUS(">>>>>>>>> memory_table_extend >>>>>>>>");
    pchunk ck = p->chunks[0];
    int c = 0;
    while(ck)
    {
        //DEBUG_STATUS(">>>>>> chunk: %p, size: %d",GET_CHUNK_AVAILABLE_ADDR(ck), GET_CHUNK_AVAILABLE_SIZE(ck));
        sprintf(szTmp, szexFmt, GET_CHUNK_AVAILABLE_ADDR(ck), GET_CHUNK_AVAILABLE_SIZE(ck), ",");
        strOut += szTmp;
        ck = ck->next;
        c ++;
    }
    //DEBUG_STATUS(">>> chunk count: %", c);
    //DEBUG_STATUS(">>>>>>>>>>>>>>>>>>> memory_table_end >>>>>>>>>>>>>>>>>");

    return strOut.c_str();
}

void CSimplePool::Initialize()
{
	m_pool_used_low_st 	= 1;
    m_alloc_from_system	= 0;
    m_free_to_system	= 0;
    m_alloc_from_pool	= 0;
    m_free_to_pool		= 0;
    m_alloctimes_from_pool 	= 0;
    m_freetimes_to_pool 	= 0;
    m_pool = memory_pool::create(0x10, 0x2000000);
    CPerformance::GetInstance()->AddCollector(this);
}

void CSimplePool::Destroy()
{
    memory_pool::destroy(m_pool);
    m_alloc_from_system	= 0;
    m_free_to_system	= 0;
    m_alloc_from_pool	= 0;
    m_free_to_pool		= 0;
}

void *CSimplePool::Alloc(size_t s)
{
    unsigned int rs = s;
    bool bsystem = false;
    void *p = memory_pool::alloc(m_pool, rs, bsystem);
    if(bsystem)
    {
        m_alloc_from_system += GET_EXCHUNK_SIZE(rs);
    }else
    {
        m_alloctimes_from_pool ++;
        m_alloc_from_pool += rs;
    }
    return p;
}

void CSimplePool::Free(void *p)
{
    bool bsystem = false;
    unsigned int s = memory_pool::release(m_pool, p, bsystem);
    if(bsystem)
    {
        m_free_to_system += GET_EXCHUNK_SIZE(s);
    }else
    {
        m_freetimes_to_pool ++;
        m_free_to_pool += s;
    }
}

void *CSimplePool::Resize(void *p, size_t src_size, size_t new_size)
{
    return memory_pool::expand(m_pool, p, src_size, new_size);
}

unsigned long CSimplePool::Shrink(){
	return memory_pool::shrink(m_pool);
}

short CSimplePool::GetUsePercent(){
    unsigned long cs = m_pool->free;
    return cs > 0 ? (short)(m_pool->used * 100.0f / cs) : 0;
}

void CSimplePool::Lookup(){
	short pp = GetUsePercent();
	if(pp < 10){
		if(m_pool_used_low_st == 0)
		{
			m_pool_used_low_st = CDateTime::timestamp();
		}else if(m_pool_used_low_st > 1){
			if(CDateTime::timestamp() - m_pool_used_low_st > NBPy_POOL_SHRINK_TIME_MS)
			{
				unsigned long l = Shrink();
				DEBUG_STATUS(">>> meory pool shrink %ld size", l);
				m_pool_used_low_st = 1;
			}
		}
	}else{
		m_pool_used_low_st = 0;
	}
}

void CSimplePool::Collect(CPerformancePackager &pack)
{
    pack.AddForInt("内存使用百分比", GetUsePercent(), PFM_PERCENT_PIC);
    pack.AddForInt("内存池当前容量", m_pool->used + m_pool->free, PFM_BYTE);
    pack.AddForInt("从系统内存分配",  m_alloc_from_system, PFM_NUMBER);
    pack.AddForInt("释放回系统内存",  m_free_to_system, PFM_NUMBER);
    //pack.AddForInt("从内存池中分配",  m_alloc_from_pool, PFM_NUMBER);
    //pack.AddForInt("释放回内存池中",  m_free_to_pool, PFM_NUMBER);
    pack.AddForInt("内存池分配次数",  m_alloctimes_from_pool, PFM_NUMBER);
    pack.AddForInt("内存池释放次数",  m_freetimes_to_pool, PFM_NUMBER);
}

NSE

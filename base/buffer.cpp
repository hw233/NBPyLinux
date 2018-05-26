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
 * File: buffer.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "buffer.h"
#include "pool.h"
#include "debug.h"

NS(NBPy)

    CBuffer::CBuffer()
		:m_current(NULL)
		,m_begin(NULL)
		,m_end(NULL)
		,m_buffer(NULL)
		,m_size(0){
        m_size = 0;
        m_defaultsize = DEFAULT_BUFFER_SIZE;
        this->Reset();
    }

    CBuffer::CBuffer(size_t size) {
        m_current = m_begin = m_end = m_buffer = NULL;
        m_size = 0;
        m_defaultsize = size;
        this->Reset();
    }

    CBuffer::CBuffer(const CBuffer &buffer) {
        m_current = m_begin = m_end = m_buffer = NULL;
        m_size = 0;
        this->Resize(buffer.GetSize());
        void *pData = buffer.Begin();
        this->Push(pData, buffer.GetDataSize());
    }

    CBuffer::~CBuffer() {
        if(m_buffer)
            CSimplePool::GetInstance()->Free(m_buffer);
        m_current = m_begin = m_end = m_buffer = NULL;
        m_size = 0;
    }

    size_t CBuffer::GetSize() const {
        return m_end - m_begin;
    }

    size_t CBuffer::GetDataSize() const {
        return m_current - m_begin;
    }

    void *CBuffer::Begin() const {
        return m_begin;
    }

    void *CBuffer::End() const {
        return m_current;
    }

    const void *CBuffer::Push(const void *p, size_t size) {
        if(this->GetSize() - this->GetDataSize() < size)
        {
            this->AddSize(size);
        }
        memcpy(m_current, p, size);
        void *pt = m_current;
        m_current += size;
        return pt;
    }

    const void *CBuffer::Pop(size_t size) {
        if(m_begin + size > m_current)
        {
            return NULL;
        }
        void *p = m_begin;
        m_begin += size;
        return p;
    }

    void CBuffer::Reset() {
        if(m_size != m_defaultsize) {
            if(m_buffer)
                CSimplePool::GetInstance()->Free(m_buffer);
            m_buffer = (char *) CSimplePool::GetInstance()->Alloc(m_defaultsize);

        } else{
        }
        m_current = m_begin = m_buffer;
        m_end = m_begin + m_defaultsize;
        m_size = m_defaultsize;
    }

    void CBuffer::Seek(size_t seek) {
        char *newcurr = m_current + seek;
        if(newcurr < m_begin || newcurr > m_end)
            return;
        m_current = newcurr;
    }

    void CBuffer::Resize(size_t size) {
        if(size <= this->GetSize())
            return;

        if(m_size >= size) {
            size_t ds = this->GetDataSize();
            void *pTmp = NULL;
            if(ds > 0) {
                pTmp = CSimplePool::GetInstance()->Alloc(ds);
                memcpy(pTmp, m_begin, ds);
            }
            m_current = m_begin = m_buffer;
            m_end = m_begin + m_size;
            if(ds > 0) {
                memcpy(m_begin, pTmp, ds);
                m_current += ds;
                CSimplePool::GetInstance()->Free(pTmp);
            }
            return;
        }else
        {
            this->AddSize(size - m_size);
        }
    }

    void CBuffer::AddSize(size_t size) {
        size_t ds = this->GetDataSize();
        void *pTmp = NULL;
        if(ds > 0) {
            pTmp = CSimplePool::GetInstance()->Alloc(ds);
            memcpy(pTmp, m_begin, ds);
        }

        if(m_buffer)
        {
            CSimplePool::GetInstance()->Free(m_buffer);
        }

        m_size += size;
        m_buffer = (char *)CSimplePool::GetInstance()->Alloc(m_size);
        if(ds > 0)
        {
            memcpy(m_buffer, pTmp, ds);
            CSimplePool::GetInstance()->Free(pTmp);
        }
        m_begin = m_buffer;
        m_end = m_begin + m_size;
        m_current = m_begin + ds;
    }

    void CBuffer::Clear() {
        memset(m_buffer, 0, m_size);
        m_current = m_begin = m_buffer;
    }

NSE

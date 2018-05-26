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
 * File: buffer.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_BUFFER_H
#define BASE_BUFFER_H

#include "base.h"

#define DEFAULT_BUFFER_SIZE 512

NS(NBPy)

    class IBuffer {
    public:
        virtual const void *Push(const void *, size_t) = 0;
        virtual const void *Pop(size_t) = 0;
        virtual void Seek(size_t) = 0;
        virtual void Reset() = 0;
        virtual void *Begin() const = 0;
        virtual void *End() const = 0;
        virtual void Resize(size_t) = 0;
        virtual void AddSize(size_t) = 0;
        virtual size_t GetSize() const = 0;
        virtual size_t GetDataSize() const = 0;
        virtual void Clear() = 0;
    };

    class CBuffer: public IBuffer{
    public:
        CBuffer();
        CBuffer(size_t size);
        CBuffer(const CBuffer& buffer);
        ~CBuffer();
        const void *Push(const void *p, size_t size);
        const void *Pop(size_t size);
        void Seek(size_t seek);
        void Reset();
        void *Begin() const;
        void *End() const;
        void Resize(size_t size);
        void AddSize(size_t size);
        size_t GetSize() const;
        size_t GetDataSize() const;
        void Clear();
    private:
        char *m_buffer;
        char *m_begin;
        char *m_end;
        char *m_current;
        size_t m_size;
        size_t m_defaultsize;
    };

    template<size_t DefaultSize>
    class CAutoBuffer: public CBuffer {
    public:
        CAutoBuffer():CBuffer(DefaultSize){
        }

        CAutoBuffer(const CAutoBuffer& buffer):CBuffer(DefaultSize){
            this->Push(buffer.Begin(), buffer.GetDataSize());
        }
    };

    class CThreadBuffer: public IBuffer {
    public:
        CThreadBuffer(){};
        ~CThreadBuffer(){};

        const void *Push(const void *p, size_t size) {
            return m_buffer.Push(p, size);
        }
        const void *Pop(size_t size) {
            return m_buffer.Pop(size);
        }
        void Seek(size_t seek) {
            m_buffer.Seek(seek);
        }
        void Reset(){
            m_buffer.Reset();
        }
        void *Begin() const{
            return m_buffer.Begin();
        }
        void *End() const{
            return m_buffer.End();
        }
        void Resize(size_t size){
            m_buffer.Resize(size);
        }
        void AddSize(size_t size){
            m_buffer.AddSize(size);
        }
        size_t GetSize() const{
            return m_buffer.GetSize();
        }
        size_t GetDataSize() const{
            return m_buffer.GetDataSize();
        }
    private:
        CBuffer m_buffer;
    };
NSE

#endif //BASE_BUFFER_H

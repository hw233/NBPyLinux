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
 * File: queue.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_QUEUE_H
#define BASE_QUEUE_H

#include "base.h"
#include <queue>

NS(NBPy)

    template <typename T>
    class CShareQueue
    {
    public:
        void push(T ev)
        {
#ifdef NBPy_THREAD_MODE
            std::lock_guard<std::recursive_mutex> lck(m_mutex);
#endif
            m_queueEvs.push(ev);
        };

        T pop()
        {
#ifdef NBPy_THREAD_MODE
            if(!m_mutex.try_lock())
                return NULL;
#endif

            if (m_queueEvs.size() == 0)
            {
#ifdef NBPy_THREAD_MODE
                m_mutex.unlock();
#endif
                return NULL;
            }
            T t = m_queueEvs.front();
            m_queueEvs.pop();
#ifdef NBPy_THREAD_MODE
            m_mutex.unlock();
#endif
            return t;
        };
    private:
        std::queue<T> m_queueEvs;
#ifdef NBPy_THREAD_MODE
        std::recursive_mutex m_mutex;
#endif
    };

    template <typename T>
    class CSharePtrQueue
    {
    public:
        void push(T ev)
        {
#ifdef NBPy_THREAD_MODE
            std::lock_guard<std::recursive_mutex> lck(m_mutex);
#endif
            m_queueEvs.push(ev);
        };

        bool pop(T *v)
        {
#ifdef NBPy_THREAD_MODE
            if(!m_mutex.try_lock())
                return false;
#endif
            if (m_queueEvs.size() == 0)
            {
#ifdef NBPy_THREAD_MODE
                m_mutex.unlock();
#endif
                return false;
            }
            T t = m_queueEvs.front();
            memcpy(v, &t, sizeof(T));
            m_queueEvs.pop();
#ifdef NBPy_THREAD_MODE
            m_mutex.unlock();
#endif
            return true;
        };
    private:
        std::queue<T> m_queueEvs;
#ifdef NBPy_THREAD_MODE
        std::recursive_mutex m_mutex;
#endif
    };

NSE

#endif //BASE_QUEUE_H

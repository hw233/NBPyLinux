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
 * File: peer.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_PEER_H
#define BASE_PEER_H

#include "network.h"

#define THREAD_ALLOCATOR_DEFINE(T)\
private:\
static CObjectAllocator<T> m_allocator;\
public:\
static IAllocator<T> *GetAllocator(){ return &m_allocator; };\
void Release(){ GetAllocator()->free(this); }\



#define THREAD_ALLOCATOR_IMPL(T)\
CObjectAllocator<T> T::m_allocator;

NS(NBPy)

    class CPeer : public IPeer
    {
    public:
        CPeer();
        CPeer(int sck);
        virtual void Init(int sck = NBPy_INVALID_SOCKET);

        virtual ~CPeer();

        //!!from IEventProcessor
        virtual void OnEventRead(LPEVENT);
        virtual void OnEventWrite(LPEVENT);
        virtual void OnEventError(LPEVENT);
        virtual void OnEvent(LPEVENT);

        virtual void Disconnect(bool unlink = false);
        virtual bool Connect(const char *, unsigned int);

        virtual unsigned int OnWriteReady(unsigned int r){ return r; };
        virtual bool OnReadReady(LPPACKET);
        virtual void OnError();
        virtual bool OnConnect(){ return false; };

        virtual bool OnClose(){ return false; };
        virtual bool OnTerminate(){ return false; };

        virtual int GetIOHandle();
        virtual PEERID GetId();

        virtual int Send(char *, size_t);

        virtual void SetDataType(char c){ m_data_tp = c; };
        virtual void SetSocketType(char c){ m_socket_tp = c; };

        virtual void Close() {
            if(m_raw_sck == NBPy_INVALID_SOCKET)
                return;
			CEventDispatcher::GetInstance()->UnBind(m_raw_sck);
            CPeerFactory::GetInstance()->Release(this);
            OnError();
        }

#if(BL_COMPRESS_ENABLED == 1)
        bool isCompressSupported(){ return m_compress_supported; };
#else
        bool isCompressSupported(){ return false; };
#endif
        virtual void setCompressSupported(bool b) { m_compress_supported = b; };

        virtual void SetScriptEngine(IScriptEngine *engine) { m_engine = engine; };

        virtual void SetSID(unsigned int sid){ m_sid = sid; };
        virtual unsigned int GetSID(){return m_sid;};
    private:
        virtual void ParseData();

    protected:
		unsigned int m_sid;
        int m_raw_sck;
        CBuffer m_recvbuffer;
        CBuffer m_sendbuffer;

        CBuffer m_parsebuffer;
        CIMDictCode m_parsecode;

        char m_data_tp;
        char m_socket_tp;
        int m_flag;
        bool m_compress_supported;

        IScriptEngine *m_engine;
    };


NSE

#endif //BASE_PEER_H

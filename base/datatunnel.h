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
 * File: datatunnel.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_DATATUNNEL_H
#define BASE_DATATUNNEL_H

#include "network.h"
#include <sys/un.h>

NS(NBPy)
	class CDataTunnel;

	class CDataTunnelNode: public IEventProcessor{
	public:
		CDataTunnelNode();
		~CDataTunnelNode();

		void Init(CDataTunnel *parent, const char *strDst, pid_t pid);

        void OnEventRead(LPEVENT);
        void OnEventWrite(LPEVENT);
        void OnEventError(LPEVENT){};
        void OnEvent(LPEVENT){};

        bool Send(const char *data, size_t l);
        bool Recv(const char *data, size_t l);

        void SetScriptEngine(IScriptEngine *e){ m_engine = e; }

	private:
		int m_raw_sck;
		char szSrcAddr[255];
		char szDstAddr[255];
        IScriptEngine *m_engine;
        CBuffer m_snd;
        CBuffer m_recv;
		CDataTunnel *pParent;
		pid_t m_dstpid;
	};

    class CDataTunnel: public IEventProcessor{
    public:
        CDataTunnel();
        ~CDataTunnel();

        void SetRcvSize(size_t s){
			m_rcvsize = s;
		};

		size_t GetRcvSize(){ return m_rcvsize; }

        void Bind(const char *);
        void UnBind();

        //!!from IEventProcessor
        void OnEventRead(LPEVENT);
        void OnEventWrite(LPEVENT){};
        void OnEventError(LPEVENT){};
        void OnEvent(LPEVENT){};

        virtual bool OnPacketIn(LPPACKET, const char *){ return false; };
        bool Send(void *, size_t, const char *, pid_t);

        void SetScriptEngine(IScriptEngine *engine) { m_engine = engine; };

		CDataTunnelNode *getSendNode(pid_t pid, STR addr);
		CDataTunnelNode *getRecvNode(STR addr);

		void removeSendNode(STR addr);
		void removeRecvNode(STR addr);

		void OnPacketReady(LPPACKET p, const char *sz);

		uint64_t getBytesForSent(){ return m_bytesForSent; };
		uint64_t getBytesForRecv(){ return m_bytesForRecv; };

        void Lookup();

	private:
        void TryRecvAllData();

    private:
		CBuffer m_rcvbuf;
        int m_raw_sck;
        IScriptEngine *m_engine;
        size_t m_rcvsize;
		char m_buf[512 * 1024];

        std::map<std::string, CDataTunnelNode *> m_snds;
        std::map<std::string, CDataTunnelNode *> m_recvs;

        CObjectAllocator<CDataTunnelNode> m_allocator;

        uint64_t m_bytesForSent;
        uint64_t m_bytesForRecv;
    };

NSE

#endif //BASE_DATATUNNEL_H

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
 * File: datatunnel.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "stacklessengine.h"
#include "datatunnel.h"
#include "datetime.h"

NS(NBPy)

	CDataTunnelNode::CDataTunnelNode()
		:m_raw_sck(NBPy_INVALID_SOCKET)
        ,m_engine(NULL)
        ,pParent(NULL)
	{

	}

	CDataTunnelNode::~CDataTunnelNode(){
		if(m_raw_sck == NBPy_INVALID_SOCKET)
			return;
		//DEBUG_STATUS("发送管道销毁", szSrcAddr);
		//接收模式的节点不会初始化socket，所以解链绑定地址只会针对发送模式节点
		CEventDispatcher::GetInstance()->UnBind(m_raw_sck);
		::close(m_raw_sck);
		//::unlink(szSrcAddr);
		m_raw_sck = NBPy_INVALID_SOCKET;
	}

	void CDataTunnelNode::Init(CDataTunnel *parent, const char *strDst, pid_t pid){
		strcpy(szDstAddr, strDst);
		pParent = parent;
		m_dstpid = pid;
	}

	void CDataTunnelNode::OnEventRead(LPEVENT e){
	}

	void CDataTunnelNode::OnEventWrite(LPEVENT e){
		if(m_raw_sck == NBPy_INVALID_SOCKET)
		{
			m_raw_sck = CNetworkUtils::GetInstance()->CreateSocket(eAT_UNIX_UDP);
			if (m_raw_sck == NBPy_INVALID_SOCKET)
			{
				DEBUG_ERROR("Create s2s send socket fail!");
				return;
			}
			CNetworkUtils::GetInstance()->SetSocketOpt(eAT_UNIX_UDP, m_raw_sck, 1);

/*
			sprintf(szSrcAddr, NBPy_DATATUNNEL_ADDRPREFIX2, getpid(), m_dstpid);
			socklen_t addrsize = 0;
			sockaddr* paddr = CNetworkUtils::GetInstance()->BuildSockAddr(eAT_UNIX_UDP, szSrcAddr, 0, &addrsize);


			int nRet = ::bind(m_raw_sck, paddr, addrsize);
			if (nRet == -1)
			{
				if( errno == EADDRINUSE )
				{
					::unlink(szSrcAddr);
					nRet = ::bind(m_raw_sck, paddr, addrsize);
					if( errno == EADDRINUSE )
					{
						DEBUG_ERROR("严重错误!!! %s 被占用无法绑定", szSrcAddr);
						::close(m_raw_sck);
						m_raw_sck = NBPy_INVALID_SOCKET;
						return;
					}
				}
				else
				{
					DEBUG_ERROR("严重错误!!! %s 无法绑定错误号%d", szSrcAddr, errno);
					::unlink(szSrcAddr);
					::close(m_raw_sck);
					m_raw_sck = NBPy_INVALID_SOCKET;
					return;
				}
			}
			DEBUG_STATUS("发送源 %s 绑定成功", szSrcAddr);
*/
			int sndlen = pParent->GetRcvSize();
			setsockopt(m_raw_sck, SOL_SOCKET, SO_SNDBUF, (char *)&sndlen, sizeof(sndlen));

			CEventDispatcher::GetInstance()->Bind(m_raw_sck, this);
		}
		//DEBUG_STATUS("CDataTunnelNode::OnEventWrite Send %d", total_s);
	}

	bool CDataTunnelNode::Send(const char *data, size_t l){
		if(m_snd.GetDataSize() > 0)
			return false;

		//尝试调用一下发送，以初始化发送socket和加入IO处理
		OnEventWrite(NULL);
		if(m_raw_sck == NBPy_INVALID_SOCKET)
			return false;

		//m_snd.Push(data, l);

		unsigned long st = CDateTime::timestamp();

		socklen_t addrsize = 0;
		sockaddr *paddr = CNetworkUtils::GetInstance()->BuildSockAddr(eAT_UNIX_UDP, szDstAddr, 0, &addrsize);

		ssize_t sent = 0;

		sent = ::sendto(m_raw_sck, data, l, MSG_DONTROUTE | MSG_DONTWAIT, paddr, addrsize);
		if ( l == sent )
		{
			//m_snd.Reset();
			//DEBUG_STATUS("CDataTunnelNode::OnEventWrite Send OK %d", sent);
			return true;
		}
		else if( 0 == errno || EAGAIN == errno || EWOULDBLOCK == errno || EINTR == errno )
		{
			//m_snd.Reset();
			if(!sent){
				DEBUG_STATUS("CDataTunnelNode::OnEventWrite Send ERROR 1");
				return false;
			}
			DEBUG_STATUS("CDataTunnelNode::OnEventWrite Send BUSY %d time: %ld", errno, (CDateTime::timestamp() - st));
			return false;
		}
		else
		{
			//m_snd.Reset();
			DEBUG_STATUS("CDataTunnelNode ERR: %d", errno);
			//pParent->removeSendNode(szDstAddr);
			return false;
		}
	}

	bool CDataTunnelNode::Recv(const char *data, size_t l){
		return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	CDataTunnelNode *CDataTunnel::getSendNode(pid_t pid, STR addr){
		CDataTunnelNode *pNode = NULL;
		std::map<std::string,CDataTunnelNode *>::iterator it = m_snds.find(addr);
		if(it == m_snds.end())
		{
			pNode = m_allocator.allocate();
			pNode = new(pNode) CDataTunnelNode();
			pNode->Init(this, addr, pid);
			m_snds[addr] = pNode;
			return pNode;
		}
		return it->second;
	}

	CDataTunnelNode *CDataTunnel::getRecvNode(STR addr){
		CDataTunnelNode *pNode = NULL;
		std::map<std::string,CDataTunnelNode *>::iterator it = m_recvs.find(addr);
		if(it == m_recvs.end())
		{
			pNode = m_allocator.allocate();
			pNode = new(pNode) CDataTunnelNode();
			pNode->Init(this, addr, 0);
			m_recvs[addr] = pNode;
			return pNode;
		}
		return it->second;
	}

	void CDataTunnel::removeSendNode(STR addr){
		std::map<std::string,CDataTunnelNode *>::iterator it = m_snds.find(addr);
		if(it == m_snds.end())
			return;
		it->second->~CDataTunnelNode();
		m_allocator.free(it->second);
		m_snds.erase(addr);
	}

	void CDataTunnel::removeRecvNode(STR addr){
		std::map<std::string,CDataTunnelNode *>::iterator it = m_recvs.find(addr);
		if(it == m_recvs.end())
			return;
		it->second->~CDataTunnelNode();
		m_allocator.free(it->second);
		m_recvs.erase(addr);
	}

    CDataTunnel::CDataTunnel()
            :m_raw_sck(NBPy_INVALID_SOCKET)
            ,m_engine(NULL)
            ,m_rcvsize(NBPy_UNIXSCOKET_RECV_SIZE)
            ,m_bytesForSent(0)
            ,m_bytesForRecv(0){

    }

    CDataTunnel::~CDataTunnel() {
        UnBind();

        std::map<std::string, CDataTunnelNode *>::iterator it = m_snds.begin();
        while(it != m_snds.end()){
			if(it->second)
			{
				it->second->~CDataTunnelNode();
				m_allocator.free(it->second);
			}
			it ++;
        }
        m_snds.clear();

        it = m_recvs.begin();
        while(it != m_recvs.end()){
			if(it->second)
			{
				it->second->~CDataTunnelNode();
				m_allocator.free(it->second);
			}
			it ++;
        }
        m_recvs.clear();
    }

    void CDataTunnel::Bind(const char *addr) {
        m_raw_sck = CSocket::ListenAtUnix(eAT_UNIX_UDP, addr);
        CEventDispatcher::GetInstance()->Bind(m_raw_sck, this);
    }

    void CDataTunnel::UnBind() {
        if(m_raw_sck == NBPy_INVALID_SOCKET)
            return;
        CEventDispatcher::GetInstance()->UnBind(m_raw_sck);
        CSocket::Close(eAT_UNIX_UDP, m_raw_sck, true);
    }

    void CDataTunnel::OnEventRead(LPEVENT e) {
    	TryRecvAllData();
    }

    void CDataTunnel::OnPacketReady(LPPACKET p, const char *sz){
		if(!OnPacketIn(p, sz) && m_engine){
			m_engine->OnPacketIn(m_raw_sck, p);
		}
    }

	void CDataTunnel::Lookup(){
        size_t hs = sizeof(PACKET_FIXED_HEADER);

        while(m_rcvbuf.GetDataSize() >= hs){
            LPPACKET_FIXED_HEADER header = (LPPACKET_FIXED_HEADER)m_rcvbuf.Begin();
            if(header->length > m_rcvbuf.GetDataSize())
            {
                //data is uncomplete!
                break;
            }

            LPPACKET p = (LPPACKET)header;
            if(!p->IsValid())
            {
                break;
            }

            OnPacketReady(p, "");
            m_rcvbuf.Pop(p->header.length);
        }
    }

	void CDataTunnel::TryRecvAllData(){
        struct sockaddr_un src_addr;
        memset(&src_addr, 0, sizeof(src_addr));
        socklen_t l = sizeof(src_addr);
        int re = 0;
        size_t total = 0;
        /// UDP EPOLLIN
        while( (re = ::recvfrom(m_raw_sck, m_buf, m_rcvsize, MSG_DONTWAIT, (sockaddr *)&src_addr, &l)) > 0 )
        {
        	m_rcvbuf.Push(m_buf, re);
            total += re;
			m_bytesForRecv += re;
        }
        if ( 0 < re || 0 == errno || EAGAIN == errno || EWOULDBLOCK == errno || EINTR == errno )
        {
        }
        else
        {
            DEBUG_STATUS("CDataTunnel::TryRecvAllData >> ERR %d", errno);
			return;
        }
	}

    bool CDataTunnel::Send(void *data, size_t datalen, const char *szDstAddr, pid_t dstpid) {
    	if(datalen > m_rcvsize){
    		DEBUG_STATUS("S2S数据太长，通过route投递. %ld > %ld", datalen, m_rcvsize);
    		return false;
    	}
    	CDataTunnelNode *node = getSendNode(dstpid, szDstAddr);
    	if(!node)
			return false;

		bool b = node->Send((const char *)data, datalen);
		if(b)
		{
			m_bytesForSent += datalen;
		}
		return b;
    }
NSE

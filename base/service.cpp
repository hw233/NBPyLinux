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
 * File: service.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "stacklessengine.h"
#include "service.h"
#include <sys/un.h>

NS(NBPy)

    CService::CService()
    :m_engine(NULL)
    ,m_raw_sck(NBPy_INVALID_SOCKET)
    ,m_raw_sckv6(NBPy_INVALID_SOCKET)
    ,m_uClientLimit(0){

    }

    CService::~CService() {

    }

    bool CService::Create(const char *szAddrv4, const char *szAddrv6, unsigned int uPort, unsigned int uClientLimit) {
        m_uClientLimit = uClientLimit;

        m_raw_sck = CSocket::Listen(szAddrv4, uPort);
        if (m_raw_sck == NBPy_INVALID_SOCKET)
        {
            return false;
        }

        if(szAddrv6)
        {
            m_raw_sckv6 = CSocket::Listen(szAddrv6, uPort);

            if (m_raw_sckv6 == NBPy_INVALID_SOCKET)
            {
                CSocket::Close(eAT_TCP, m_raw_sck);
                return false;
            }
            CEventDispatcher::GetInstance()->Bind(m_raw_sckv6, this);
        }

        CEventDispatcher::GetInstance()->Bind(m_raw_sck, this);

        return m_raw_sck != NBPy_INVALID_SOCKET;
    }

    void CService::Close() {
        if(m_raw_sck != NBPy_INVALID_SOCKET){
			CEventDispatcher::GetInstance()->UnBind(m_raw_sck);
            CSocket::Close(eAT_TCP, m_raw_sck);
        }
        if(m_raw_sckv6 != NBPy_INVALID_SOCKET){
			CEventDispatcher::GetInstance()->UnBind(m_raw_sckv6);
			CSocket::Close(eAT_TCP, m_raw_sckv6);
        }
        m_raw_sck = NBPy_INVALID_SOCKET;
        m_raw_sckv6 = NBPy_INVALID_SOCKET;
    }

    void CService::OnEventRead(LPEVENT e) {
        //e->data = (e->fd == m_raw_sck) ? 4 : 6;
        while(true){
			int s = NBPy_INVALID_SOCKET;
			sockaddr_in iaddr;
			sockaddr *paddr = (sockaddr *)&iaddr;
			socklen_t ts = sizeof(sockaddr_in);
			s = CSocket::Accept(m_raw_sck, paddr, ts);
			if(s == NBPy_INVALID_SOCKET)
				s = CSocket::Accept(m_raw_sckv6, paddr, ts);

			if(s == NBPy_INVALID_SOCKET)
				break;

			//e->fd = s;
			ACCEPT_PEER_INFO info;
			info.fd = s;

			CNetworkUtils::GetInstance()->FillAcceptPeerInfo(&info);

			IPeer *pPeer = NULL;

			OnConnected(&info, pPeer);

			//DEBUG_STATUS("New Connection");
			if(pPeer)
			{
				if(!pPeer->OnConnect() && m_engine){
					m_engine->OnConnect(info.fd, pPeer->GetSID(), info.r_addr, info.r_port);
				}
			}
        }
    }

    void CService::OnEventError(LPEVENT e) {
        //...
    }

    void CService::OnEvent(LPEVENT e) {
        int event_sets = e->event_sets;
        if (NBPy_VALIDATE_MASK(event_sets, ETP_ERROR))
        {
            OnError(e->fd);
            return;
        }
        if (NBPy_VALIDATE_MASK(event_sets, ETP_READ_READY))
        {
        	if(e->fd != NBPy_INVALID_SOCKET)
        	{
        	}
        }
    }
NSE

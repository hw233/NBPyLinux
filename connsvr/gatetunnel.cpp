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
 * File: gatetunnel.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "gatetunnel.h"
#include "clientpeer.h"
#include <stacklessengine.h>

NS(NBPy)
    void CGateTunnel::Initialize() {
        m_engine = NULL;
        sprintf(szAddr, NBPy_FDTUNNEL_ADDRPREFIX, getpid());
        Bind(szAddr);
    }

    void CGateTunnel::Destroy() {
        UnBind();
    }

    bool CGateTunnel::OnFDIn(int *fds, size_t l) {
        ACCEPT_PEER_INFO info;
        for(int i = 0; i < l; i ++)
        {
            memset(&info, 0, sizeof(info));
            info.fd = fds[i];
            CNetworkUtils::GetInstance()->FillAcceptPeerInfo(&info);

			DEBUG_STATUS(">>> Accept client %s:%d.", info.r_addr, info.r_port);
			CClientPeer *pClient = (CClientPeer *)CPeerFactory::GetInstance()->Create<CClientPeer>(info.fd);
			pClient->SetScriptEngine(CStacklessEngine::GetInstance());

            if(!pClient->OnConnect() && m_engine){
                m_engine->OnConnect(info.fd, pClient->GetSID(), info.r_addr, info.r_port);
            }
        }
        return true;
    }
NSE

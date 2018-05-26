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
 * File: slaveservice.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "slaveservice.h"
#include "slavepeer.h"
#include <protocol.h>

NS(NBPy)

    void CSlaveService::Initialize() {
        CIMDictCode code;
        CDictPackager p2(&code);
        p2.Set<int,int>(PK_SCHEME, PS_R2S);
        p2.Set<int,int>(PK_TYPE, PT_SLAVE_REG);
        p2.Save();
        PACKET sp;
        memset(&sp, 0, sizeof(sp));
        sp.SetAsIMCode();
        sp.Pack(&m_regActBuff
                ,(char *)code.GetRawBuffer()->Begin()
                ,code.GetRawBuffer()->GetDataSize());
    }

    void CSlaveService::Destroy() {
        Close();
    }

    void CSlaveService::OnConnected(LPACCEPT_PEER_INFO info, IPeer *&pClient) {
        DEBUG_STATUS(">>> Slave %s:%d connected.", info->r_addr, info->r_port);
        pClient = CPeerFactory::GetInstance()->Create<CSlavePeer>(info->fd);
    }

    void CSlaveService::SendRegAct(IPeer *p){
        p->Send((char *)m_regActBuff.Begin(), m_regActBuff.GetDataSize());
    }

    void CSlaveService::SetPidPeer(pid_t pid, IPeer *p) {
        m_pidPeers[pid] = p;
    }

    IPeer* CSlaveService::GetPeerForPid(pid_t pid) {
        std::map<pid_t, IPeer *>::iterator it = m_pidPeers.find(pid);
        if(it == m_pidPeers.end())
            return NULL;
        return it->second;
    }

    void CSlaveService::RemovePidPeer(pid_t pid) {
        m_pidPeers.erase(pid);
    }

    void CSlaveService::SetTagPeer(int tag, IPeer *p) {
        TAGPEERS::iterator it = m_tagPeers.find(tag);
        if(it == m_tagPeers.end())
        {
            m_tagPeers[tag].clear();
            it = m_tagPeers.find(tag);
        }
        std::map<IPeer *, int> &km = it->second;
        km[p] = tag;
    }

    void CSlaveService::GetPeersForTag(int tag, IPeer **dst, size_t ts) {
        TAGPEERS::iterator it = m_tagPeers.find(tag);
        if(it == m_tagPeers.end())
            return;
        std::map<IPeer *, int> &km = it->second;
        int i = 0;
        for(std::map<IPeer *, int>::iterator kv=km.begin();kv != km.end(); kv ++){
            if(i >= ts)
                break;
            dst[i] = kv->first;
            i ++;
        }
    }

    void CSlaveService::RemoveTagPeer(int tag, IPeer *p) {
        TAGPEERS::iterator it = m_tagPeers.find(tag);
        if(it == m_tagPeers.end())
            return;
        std::map<IPeer *, int> &km = it->second;
        km.erase(p);
    }

NSE

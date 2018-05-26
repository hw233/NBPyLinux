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
 * File: slavepeer.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "slavepeer.h"
#include "slaveservice.h"
#include <protocol.h>

NS(NBPy)

THREAD_ALLOCATOR_IMPL(CSlavePeer);

    CSlavePeer::CSlavePeer()
        :m_pid(0)
        ,m_tp(ST_ROUTER){
        memset(m_tags,0, sizeof(int) * 64);
    }

    CSlavePeer::~CSlavePeer() {
        //DEBUG_STATUS("~CSlavePeer %p", this);
    }

    bool CSlavePeer::OnClose(){
        CSlaveService::GetInstance()->RemovePidPeer(m_pid);

        for(int i = 0; i < 64; i ++)
        {
            if(m_tags[i] == 0)
                break;
            CSlaveService::GetInstance()->RemoveTagPeer(m_tags[i], this);
        }

        OnUnRegister();
        return true;
    }

    bool CSlavePeer::Terminate(){
        //Release();
        return false;
    }

    bool CSlavePeer::OnReadReady(LPPACKET p) {
        CPacketDecoder decoder(p);
        CDictPackager *packager = dynamic_cast<CDictPackager *>(decoder.GetCodePackager());
        if(!packager)
            return false;

        int scheme = packager->Get<int,int>(PK_SCHEME);
        int tp = packager->Get<int,int>(PK_TYPE);

        if (scheme == PS_S2R)
        {
            switch (tp)
            {
                case PT_DELIVER:
                {
                    //代为投递
                    OnDeliver(packager);
                }
                    break;
                case PT_SLAVE_REG:
                {
                    //节点声明
                    OnRegister(packager);
                }
                    break;
                default:{}
            }
            return true;
        }
        return false;
    }

    void CSlavePeer::OnRegister(CDictPackager *packager){

        m_tp    = packager->Get<int,int>(PK_SRV_TYPE, ST_ROUTER);
        m_pid   = packager->Get<int,int>(PK_PID);
        CIMListCode codeTags;
        size_t st = 0;
        packager->Get<int, CIMListCode *>(PK_TAGS, &codeTags, st);
        CListPackager listPackager(&codeTags);

        // 绑定PID和PEER
        CSlaveService::GetInstance()->SetPidPeer(m_pid, this);

        // 绑定TAG和PEER
        st = MIN(listPackager.GetSize(), 64);
        for (int i = 0; i < st; ++i) {
            m_tags[i] = listPackager.At<int>(i);
            CSlaveService::GetInstance()->SetTagPeer(m_tags[i], this);
        }

        // 回发响应
        CSlaveService::GetInstance()->SendRegAct(this);

        DEBUG_STATUS(">>> Slave %d register as [%s].", m_pid, m_tp == ST_CONN ? "connsvr" : "logicsvr");
    }

    void CSlavePeer::OnUnRegister() {

        DEBUG_STATUS(">>> Slave %d unregister.", m_pid);
    }

    void CSlavePeer::OnDeliver(CDictPackager *packeger)
    {
        DEBUG_STATUS(">>> Has deliver-packet.");
        CIMDictCode codeWrap;
        int tag = packeger->Get<int, int>(PK_TAG);


        packeger->Set<int, int>(PK_SCHEME, PS_R2S);

        if(tag <= 0){
            CIMListCode codeTo;
            size_t count = 0;
            packeger->Get<int, CIMListCode *>(PK_TO, &codeTo, count);
            CListPackager listPackager(&codeTo);
            packeger->Remove<int>(PK_TO);
            packeger->Save(&codeWrap);

            for(int i = 0; i < listPackager.GetSize(); i ++)
            {
                pid_t pid = listPackager.At<int>(i);
                if(pid <= 0 || pid == getpid())
                    continue;
                CSlavePeer *peer = dynamic_cast<CSlavePeer *>(CSlaveService::GetInstance()->GetPeerForPid(pid));
                if(peer && peer != this)
                {
					DEBUG_STATUS(">>> Deliver packet from %d to pid %d", m_pid, pid);
                    peer->Deliver(&codeWrap);
                }
            }
        }else{
			DEBUG_STATUS(">>> Deliver packet from %d to tag %d", m_pid, tag);
            packeger->Remove<int>(PK_TAG);
            packeger->Save(&codeWrap);

            IPeer *peers[64];
            memset(peers, 0, sizeof(IPeer *) * 64);
            CSlaveService::GetInstance()->GetPeersForTag(tag, peers, 64);
            for (int i = 0; i < 64; ++i) {
                if(!peers[i])
                    break;
                CSlavePeer *peer = dynamic_cast<CSlavePeer *>(peers[i]);
                if(!peer || peer == this)
                    continue;
                peer->Deliver(&codeWrap);
            }
        }
    }


    void CSlavePeer::Deliver(IIMCode *code) {
        PACKET p;
        memset(&p, 0, sizeof(p));
        p.SetAsIMCode();
        p.Pack(&m_sendbuffer, (char *)code->GetRawBuffer()->Begin(), code->GetRawBuffer()->GetDataSize());
        Send(NULL, 0);
    }

NSE

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
 * File: slaveservice.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef ROUTER_SLAVESERVICE_H
#define ROUTER_SLAVESERVICE_H

#include <service.h>
#include <singleton.h>

NS(NBPy)

    typedef std::map<int, std::map<IPeer *, int> > TAGPEERS;

    class CSlaveService: public CService{
        DECLARE_SINGLETON(CSlaveService)
    public:
        void Initialize();
        void Destroy();

        void OnConnected(LPACCEPT_PEER_INFO info, IPeer *&pClient);

        void SendRegAct(IPeer *p);

        void SetPidPeer(pid_t pid, IPeer *p);
        IPeer *GetPeerForPid(pid_t pid);
        void RemovePidPeer(pid_t pid);

        void SetTagPeer(int tag, IPeer *p);
        void GetPeersForTag(int tag, IPeer **dst, size_t ts);
        void RemoveTagPeer(int tag, IPeer *p);
    private:
        CBuffer m_regActBuff;
        std::map<pid_t, IPeer *> m_pidPeers;
        TAGPEERS m_tagPeers;
    };

NSE

#endif //ROUTER_SLAVESERVICE_H

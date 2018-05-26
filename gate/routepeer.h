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
 * File: routepeer.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef CONNSVR_ROUTEPEER_H
#define CONNSVR_ROUTEPEER_H

#include <peer.h>
#include <singleton.h>

NS(NBPy)

    class CRoutePeer: public CPeer{
        DECLARE_SINGLETON(CRoutePeer)
    public:
        void Initialize();
        void Destroy();

        bool OnClose();
        bool OnReadReady(LPPACKET p);

        void Register();
        void OnDeliver(CDictPackager *p, bool rewrap = true);
        void Deliver(int tag, IIMCode *code);

    private:
        CBuffer m_bufSend;
        CBuffer m_bufSendJson;
        CBuffer m_bufDecodeJson;
        PACKET m_pckSend;
    };

NSE

#endif //CONNSVR_ROUTEPEER_H

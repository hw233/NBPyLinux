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
 * File: clientservice.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include <stacklessengine.h>
#include "clientservice.h"
#include "clientpeer.h"
#include "setting.h"
#include <protocol.h>

NS(NBPy)

    void CClientService::Initialize() {

				//回应
				CIMDictCode codeLogin;
				CDictPackager packLogin;
				packLogin.Set<int,int>(PK_SCHEME, PS_S2C);
				packLogin.Set<int,int>(PK_TYPE, PT_RESULT);
				packLogin.Set<int,int>(PK_ERROR, CSetting::GetInstance()->getLoginQueueErrCode());
				packLogin.Set<int,int>(PK_REQUEST, CSetting::GetInstance()->getLoginMsgType());
				packLogin.Set<int,int>(PK_TAG, CSetting::GetInstance()->getLoginMsgTag());
				packLogin.Save(&codeLogin);

				static PACKET pckSend;
				buff_login_queue.Reset();
				memset(&pckSend, 0, sizeof(pckSend));

				pckSend.SetAsIMCode();
				pckSend.Pack(&buff_login_queue
						,(char *)codeLogin.GetRawBuffer()->Begin()
						,codeLogin.GetRawBuffer()->GetDataSize());
    }

    void CClientService::Destroy() {
        Close();
    }

    void CClientService::OnConnected(LPACCEPT_PEER_INFO info, IPeer *&pClient) {
        DEBUG_STATUS(">>> Accept client %s:%d.", info->r_addr, info->r_port);
        pClient = CPeerFactory::GetInstance()->Create<CClientPeer>(info->fd);
        pClient->SetScriptEngine(CStacklessEngine::GetInstance());
        //pClient->SetSID(getSID(info->fd));
    }


NSE

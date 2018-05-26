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
 * File: clientpeer.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "clientpeer.h"
#include "routepeer.h"
#include "setting.h"
#include "s2stunnel.h"
#include <protocol.h>
#include <datetime.h>
#include <stacklessengine.h>

NS(NBPy)

    THREAD_ALLOCATOR_IMPL(CClientPeer);

    CClientPeer::CClientPeer(){
        m_data_tp = -1;
        m_total = 0;
        m_st = m_ss = CDateTime::timestamp();
    }

    CClientPeer::~CClientPeer() {
        DEBUG_STATUS(">>> Client %p disconnected.", this);
    }

    bool CClientPeer::OnClose(){
    	print_stacktrace();
        DEBUG_STATUS(">>> Client %p OnClose.", this);
        return false;
    }

    bool CClientPeer::OnTerminate(){
        //Release();
        return false;
    }

    bool CClientPeer::OnReadReady(LPPACKET p) {

    	if(CDateTime::timestamp() - m_st > 1000)
    	{
			m_total = 1;
    		m_st = CDateTime::timestamp();
    	}else{
			m_total ++;
    	}

    	size_t fre = CSetting::GetInstance()->GetFrequency();
    	if(m_total > fre)
    	{
    		DEBUG_STATUS("client %d on frequency-control", m_raw_sck);
    		Close();
    		return true;
    	}

		m_data_tp = ePEERDTP_PACKET;

        CPacketDecoder decoder(p);
        CDictPackager *packager = dynamic_cast<CDictPackager *>(decoder.GetCodePackager());
        if(!packager)
            return true;

        int scheme = packager->Get<int,int>(PK_SCHEME);
        int tp = packager->Get<int,int>(PK_TYPE);

        //增加消息节指示来源FD
        packager->Set<int,long>(PK_CID, GetSID());
        packager->Set<int,int>(PK_PID, getpid());

        CIMDictCode code;
        packager->Save(&code);

		static CBuffer bufSend;
		static PACKET pckSend;

		bufSend.Reset();
		memset(&pckSend, 0, sizeof(pckSend));
		pckSend.SetAsIMCode();
		pckSend.Pack(&bufSend
				,(char *)code.GetRawBuffer()->Begin()
				,code.GetRawBuffer()->GetDataSize());

		LPPACKET et = (LPPACKET)bufSend.Begin();

		CStacklessEngine::GetInstance()->OnPacketIn(0, et);
        return true;
    }

    void CClientPeer::MutilSend(IBuffer *im){
    	if(m_sendbuffer.GetDataSize() > 0xA00000){
			DEBUG_STATUS("The client has 10m data in send buffer, force close it.");
			this->Close();
			return;
    	}else
        {
			DEBUG_STATUS(">>> Send im-data %d to client.", im->GetDataSize());
            Send((char *)im->Begin(), im->GetDataSize());
        }
    }

	bool CClientPeer::Lookup(){
		return (CDateTime::timestamp() - m_ss) < 30000;
	}
NSE

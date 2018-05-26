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
#include "clientservice.h"
#include <protocol.h>
#include <datetime.h>

NS(NBPy)

    THREAD_ALLOCATOR_IMPL(CClientPeer);

    CClientPeer::CClientPeer(){
        m_data_tp = -1;
        m_total = 0;
        m_st = CDateTime::timestamp();
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

		//static size_t l = 0;
		//l += 1;
		//DEBUG_STATUS("CClientPeer::OnReadReady");

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

        if(m_data_tp < 0)
        {
            m_data_tp = ePEERDTP_PACKET;
            if(p->IsJson())
                m_data_tp = ePEERDTP_JSON;
        }

        CPacketDecoder decoder(p);
        CDictPackager *packager = dynamic_cast<CDictPackager *>(decoder.GetCodePackager());
        if(!packager)
            return true;

        int scheme = packager->Get<int,int>(PK_SCHEME);
        int tp = packager->Get<int,int>(PK_TYPE);
        int tag = packager->Get<int,int>(PK_TAG);

        if(scheme == PS_C2S && tp == PT_HEART){
            //回应心跳
            CIMDictCode codeData;
            CIMDictCode codeHeart;
            CDictPackager packHeartData;
            CDictPackager packHeart;
            packHeartData.Set<int,long>(PK_TT, CDateTime::timestamp());
            packHeartData.Save(&codeData);
            packHeart.Set<int,int>(PK_SCHEME, PS_S2C);
            packHeart.Set<int,int>(PK_TYPE, PT_RESULT);
            packHeart.Set<int,int>(PK_ERROR, 0);
            packHeart.Set<int,int>(PK_REQUEST, PT_HEART);
            packHeart.Set<int,int>(PK_TAG, TAG_CONN);
            packHeart.Set<int,CIMDictCode *>(PK_BODY, &codeData);
            packHeart.Save(&codeHeart);

			static CBuffer bufSend;
			static PACKET pckSend;
			bufSend.Reset();
			memset(&pckSend, 0, sizeof(pckSend));

			pckSend.SetAsIMCode();
			pckSend.Pack(&bufSend
					,(char *)codeHeart.GetRawBuffer()->Begin()
					,codeHeart.GetRawBuffer()->GetDataSize());

			this->MutilSend(&bufSend, NULL);
			DEBUG_STATUS("CClientPeer::OnReadReady LoginLimit");
            return true;
        }

		//DEBUG_STATUS("CClientPeer::OnReadReady Normal");

        //其他消息全部转发
        TAGPIDS &tps = CSetting::GetInstance()->GetTagPids();
        TAGPIDS::iterator it = tps.find(tag);
        if(it == tps.end())
            return true;

        //增加消息节指示来源FD
        packager->Set<int,long>(PK_CID, GetSID());
        packager->Set<int,int>(PK_PID, getpid());

        CIMDictCode code;
        packager->Save(&code);

        if(scheme == PS_C2S
			&& tp == CSetting::GetInstance()->getLoginMsgType()
			&& tag == CSetting::GetInstance()->getLoginMsgTag()
		){
        	if(!CSetting::GetInstance()->incLoginQueue())
			{
				this->MutilSend(CClientService::GetInstance()->getLoginQueueBuffer(), NULL);
				return true;
			}
        }


        bool sc = false;

        std::map<pid_t, int> &km = it->second;

        pid_t failpids[64];
        memset(failpids, 0, sizeof(pid_t) * 64);
        int i = 0;
        for(std::map<pid_t, int>::iterator kv=km.begin(); kv != km.end(); kv ++)
        {
            if(!CS2STunnel::GetInstance()->SendIMCode(kv->first, &code))
            {
                failpids[i] = kv->first;
                i ++;
                //sc = false;
            }
            else{
            	sc = true;
            }
        }

        if(!sc)
        {
        	DEBUG_STATUS("Deliver");
            CRoutePeer::GetInstance()->Deliver(tag, &code);
        }
        return true;
    }

    void CClientPeer::MutilSend(IBuffer *im, IBuffer *json){
    	if(m_sendbuffer.GetDataSize() > 0xA00000){
			DEBUG_STATUS("The client has 10m data in send buffer, force close it.");
			this->Close();
			return;
    	}
        if(CSetting::GetInstance()->IsJsonSupport() && m_data_tp == ePEERDTP_JSON && json != NULL)
        {
			DEBUG_STATUS(">>> Send json-data %d to client.", json->GetDataSize());
            Send((char *)json->Begin(), json->GetDataSize());
        }else if(im != NULL)
        {
			DEBUG_STATUS(">>> Send im-data %d to client.", im->GetDataSize());
            Send((char *)im->Begin(), im->GetDataSize());
        }
    }

NSE

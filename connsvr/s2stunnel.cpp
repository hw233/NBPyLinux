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
 * File: s2stunnel.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "s2stunnel.h"
#include "routepeer.h"
#include "setting.h"
#include <protocol.h>

NS(NBPy)
    void CS2STunnel::Initialize() {
        sprintf(szAddr, NBPy_DATATUNNEL_ADDRPREFIX, getpid());
        Bind(szAddr);
    }

    void CS2STunnel::Destroy() {
        UnBind();
    }

    void CS2STunnel::Inform(pid_t pid, bool ret){
        //if(m_InformData.GetDataSize() == 0)
        	DEBUG_STATUS("CS2STunnel::Inform to %d", pid);

        	m_InformData.Reset();
            CIMDictCode code;
            CDictPackager packager(&code);
            packager.Set<int,int>(PK_SCHEME, PS_S2S);
            packager.Set<int,int>(PK_TYPE, PT_SLAVE_INFORM);
            packager.Set<int,bool>(PK_REQUEST, ret);
            packager.Set<int,int>(PK_PID, (int)getpid());

            CIMListCode codeTags;
            CListPackager packagerTags(&codeTags);
            packagerTags.Append<int>(TAG_CONN);
            packagerTags.Save();
            packager.Set<int,CIMListCode *>(PK_TAGS, &codeTags);

            packager.Save();

            PACKET sp;
            sp.SetAsIMCode();
            sp.Pack(&m_InformData
                    ,(char *)code.GetRawBuffer()->Begin()
                    ,code.GetRawBuffer()->GetDataSize());

        char szDstAddr[255];
        sprintf(szDstAddr, NBPy_DATATUNNEL_ADDRPREFIX, pid);
        if(!Send((char *)m_InformData.Begin(), m_InformData.GetDataSize(), szDstAddr, pid)){
        	DEBUG_STATUS("S2STunnel is busy. send inform by router to %d", pid);
            CRoutePeer::GetInstance()->DeliverToPid(pid, &code);
        }
    }

    void CS2STunnel::Exit(){
    	CBuffer exitData;

		CIMDictCode code;
		CDictPackager packager(&code);
		packager.Set<int,int>(PK_SCHEME, PS_S2S);
		packager.Set<int,int>(PK_TYPE, PT_SLAVE_EXIT);
		packager.Set<int,int>(PK_PID, (int)getpid());

		CIMListCode codeTags;
		CListPackager packagerTags(&codeTags);
		packagerTags.Append<int>(TAG_CONN);
		packagerTags.Save();

		packager.Set<int,CIMListCode *>(PK_TAGS, &codeTags);

		packager.Save();

		PACKET sp;
		sp.SetAsIMCode();
		sp.Pack(&exitData
				,(char *)code.GetRawBuffer()->Begin()
				,code.GetRawBuffer()->GetDataSize());


		std::map<pid_t,int>::iterator it = m_pids.begin();
		while(it != m_pids.end())
		{
			int pid = it->first;
			char szDstAddr[255];
			sprintf(szDstAddr, NBPy_DATATUNNEL_ADDRPREFIX, pid);
			if(!Send((char *)exitData.Begin(), exitData.GetDataSize(), szDstAddr, pid)){
				CRoutePeer::GetInstance()->DeliverToPid(pid, &code);
			}
			it ++;
		}
    }

    bool CS2STunnel::SendIMCode(pid_t pid, IIMCode *code){
        m_sendbuffer.Reset();
        char szDstAddr[255];
        sprintf(szDstAddr, NBPy_DATATUNNEL_ADDRPREFIX, pid);
		//DEBUG_STATUS(">>> CS2STunnel::SendIMCode %s", szDstAddr);
        PACKET sp;
        sp.SetAsIMCode();
        sp.Pack(&m_sendbuffer
                ,(char *)code->GetRawBuffer()->Begin()
                ,code->GetRawBuffer()->GetDataSize());
        return Send((char *)m_sendbuffer.Begin(), m_sendbuffer.GetDataSize(), szDstAddr, pid);
    }

    bool CS2STunnel::OnPacketIn(LPPACKET p, const char *s) {
        CPacketDecoder decoder(p);
        CDictPackager *packager = dynamic_cast<CDictPackager *>(decoder.GetCodePackager());
        if(!packager)
            return true;
        int scheme = packager->Get<int,int>(PK_SCHEME);
        int tp = packager->Get<int,int>(PK_TYPE);
        if(scheme == PS_S2S)
        {
            switch(tp){
                case PT_SLAVE_INFORM:{
                    OnInform(packager);
                    return true;
                }
                case PT_SLAVE_EXIT:{
                    OnExit(packager);
                    return true;
                }
                default:{}
            }
        }else if(scheme == PS_S2C){
            CRoutePeer::GetInstance()->OnDeliver(packager, false);
            return true;
        }
        return false;
    }

    void CS2STunnel::OnInform(CDictPackager *packager){
        int pid = packager->Get<int,int>(PK_PID);
        bool bNeedReturn = packager->Get<int,bool>(PK_REQUEST, false);
        CIMListCode codeTags;
        size_t count = 0;
        packager->Get<int,CIMListCode *>(PK_TAGS, &codeTags, count);
        CListPackager listPackager(&codeTags);
        for(int i = 0; i < listPackager.GetSize(); i ++)
        {
            int tag = listPackager.At<int>(i);
            if(tag > 0)
                CSetting::GetInstance()->AddTagPid(tag, pid);
        }

		m_pids[pid] = 1;
        //return inform to src process
        if(bNeedReturn)
			Inform(pid, false);
    }

    void CS2STunnel::OnExit(CDictPackager *packager){
        int pid = packager->Get<int,int>(PK_PID);
        CIMListCode codeTags;
        size_t count = 0;
        packager->Get<int,CIMListCode *>(PK_TAGS, &codeTags, count);
        CListPackager listPackager(&codeTags);
        for(int i = 0; i < listPackager.GetSize(); i ++)
        {
            int tag = listPackager.At<int>(i);
            if(tag > 0)
                CSetting::GetInstance()->RemoveTagPid(tag, pid);
        }

        m_pids.erase(pid);
    }

	void CSetting::Collect(CPerformancePackager &pack)
	{
		pack.AddForInt("ClientPeer-Count", CPeerFactory::GetInstance()->GetPeers().size(), PFM_NUMBER);
		pack.AddForInt64("UNIX-SENT", (uint64_t)CS2STunnel::GetInstance()->getBytesForSent(), PFM_NUMBER);
		pack.AddForInt64("UNIX-RECV", (uint64_t)CS2STunnel::GetInstance()->getBytesForRecv(), PFM_NUMBER);
	}
NSE

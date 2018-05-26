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

#include <stacklessengine.h>
#include "s2stunnel.h"
#include "setting.h"
#include <protocol.h>
#include "routepeer.h"

NS(NBPy)

    void CS2STunnel::Initialize() {
    	m_total = 0;
        sprintf(szAddr, NBPy_DATATUNNEL_ADDRPREFIX, getpid());
        Bind(szAddr);
    }

    void CS2STunnel::Destroy() {
        UnBind();
    }

    void CS2STunnel::Inform(pid_t pid){
        if(m_InformData.GetDataSize() == 0)
        {
            CIMDictCode code;
            CDictPackager packager(&code);
            packager.Set<int,int>(PK_SCHEME, PS_S2S);
            packager.Set<int,int>(PK_TYPE, PT_SLAVE_INFORM);
            packager.Set<int,int>(PK_PID, (int)getpid());

            CIMListCode codeTags;
            CListPackager packagerTags(&codeTags);
            for(int i = 0; i < 64; i ++)
            {
                int tag = CRoutePeer::GetInstance()->GetTag(i);
                if(tag == 0)
                    break;
                packagerTags.Append<int>(tag);
            }
            packagerTags.Save();

            packager.Set<int,CIMListCode *>(PK_TAGS, &codeTags);

            packager.Save();

            PACKET sp;
            sp.SetAsIMCode();
            sp.Pack(&m_InformData
                    ,(char *)code.GetRawBuffer()->Begin()
                    ,code.GetRawBuffer()->GetDataSize());
        }
        char szDstAddr[255];
        sprintf(szDstAddr, NBPy_DATATUNNEL_ADDRPREFIX, pid);
        Send((char *)m_InformData.Begin(), m_InformData.GetDataSize(), szDstAddr, pid);
    }

    bool CS2STunnel::SendIMCode(pid_t pid, IIMCode *code){
        m_sendbuffer.Reset();
        char szDstAddr[255];
        sprintf(szDstAddr, NBPy_DATATUNNEL_ADDRPREFIX, pid);
        PACKET sp;
        sp.SetAsIMCode();
        sp.Pack(&m_sendbuffer
                ,(char *)code->GetRawBuffer()->Begin()
                ,code->GetRawBuffer()->GetDataSize());
        return Send((char *)m_sendbuffer.Begin(), m_sendbuffer.GetDataSize(), szDstAddr, pid);
    }

    bool CS2STunnel::OnPacketIn(LPPACKET p, const char *s) {
    	m_total ++;
    	//DEBUG_STATUS("s2spacket >> %ld", m_total);
        return false;
    }


	void CSetting::Collect(CPerformancePackager &pack)
	{
		pack.AddForInt("LOGICTIME-AGV", m_total_logic_agv, PFM_NUMBER);
		pack.AddForInt64("UNIX-SENT", (uint64_t)CS2STunnel::GetInstance()->getBytesForSent(), PFM_NUMBER);
		pack.AddForInt64("UNIX-RECV", (uint64_t)CS2STunnel::GetInstance()->getBytesForRecv(), PFM_NUMBER);
	}
NSE

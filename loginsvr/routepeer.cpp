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
 * File: routepeer.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "routepeer.h"
#include <protocol.h>

NS(NBPy)

    void CRoutePeer::Initialize(){
        memset(m_tags,0, sizeof(int) * 64);
    }

    void CRoutePeer::Destroy(){

    }

    bool CRoutePeer::OnClose(){
        DEBUG_STATUS(">>> The route has been disconnected!!!");
        return false;
    }

    void CRoutePeer::AddTag(int tag) {
        for (int i = 0; i < 64; ++i) {
            if(m_tags[i] > 0)
                continue;
            m_tags[i] = tag;
            break;
        }
    }

    int CRoutePeer::GetTag(size_t index) {
        if(index < 64)
            return m_tags[index];
        return 0;
    }

    bool CRoutePeer::OnReadReady(LPPACKET p){
        CPacketDecoder decoder(p);
        CDictPackager *packager = dynamic_cast<CDictPackager *>(decoder.GetCodePackager());
        if(!packager)
            return false;

        int scheme = packager->Get<int,int>(PK_SCHEME);
        int tp = packager->Get<int,int>(PK_TYPE);

        if (scheme == PS_R2S)
        {
            switch (tp)
            {
                case PT_SLAVE_REG:
                {
                    DEBUG_STATUS(">>> Successfully registered to route.");
                    return false;
                }
            }
        }
        return false;
    }

    void CRoutePeer::Register() {
        CIMDictCode code;
        CDictPackager packager(&code);
        packager.Set<int,int>(PK_SCHEME, PS_S2R);
        packager.Set<int,int>(PK_SRV_TYPE, ST_LOGIC);
        packager.Set<int,int>(PK_TYPE, PT_SLAVE_REG);
        packager.Set<int,int>(PK_PID, (int)getpid());

        CIMListCode codeTags;
        CListPackager packagerTags(&codeTags);
        for(int i = 0; i < 64; i ++)
        {
            if(m_tags[i] == 0)
                break;
            packagerTags.Append<int>(m_tags[i]);
        }
        packagerTags.Save();

        packager.Set<int,CIMListCode *>(PK_TAGS, &codeTags);
        packager.Save();

        PACKET sp;
        memset(&sp, 0, sizeof(sp));
        sp.SetAsIMCode();
        sp.Pack(&m_sendbuffer
                ,(char *)code.GetRawBuffer()->Begin()
                ,code.GetRawBuffer()->GetDataSize());

        Send(NULL, 0);
    }

    void CRoutePeer::Deliver(IIMCode *code){
        PACKET sp;
        memset(&sp, 0, sizeof(sp));
        sp.SetAsIMCode();
        sp.Pack(&m_sendbuffer
                ,(char *)code->GetRawBuffer()->Begin()
                ,code->GetRawBuffer()->GetDataSize());

        Send(NULL, 0);
    }

NSE

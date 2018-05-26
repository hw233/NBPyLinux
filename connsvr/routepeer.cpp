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
#include "setting.h"
#include "clientpeer.h"
#include <protocol.h>
#include <codec_json.h>

NS(NBPy)

    void CRoutePeer::Initialize(){
    }

    void CRoutePeer::Destroy(){
    }

    bool CRoutePeer::OnClose(){
        DEBUG_STATUS(">>> The route has been disconnected!!!");
        return false;
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
                case PT_DELIVER:
                {
                    //代为投递
                    OnDeliver(packager);
                    return true;
                }
                case PT_SLAVE_REG:
                {
                    DEBUG_STATUS(">>> Successfully registered to route.");
                    return false;
                }
                default:{}
            }
        }
        return false;
    }

    void CRoutePeer::Register() {
        CIMDictCode code;
        CDictPackager packager(&code);
        packager.Set<int,int>(PK_SCHEME, PS_S2R);
        packager.Set<int,int>(PK_SRV_TYPE, ST_CONN);
        packager.Set<int,int>(PK_TYPE, PT_SLAVE_REG);
        packager.Set<int,int>(PK_PID, (int)getpid());

        CIMListCode codeTags;
        CListPackager packagerTags(&codeTags);
        packagerTags.Append<int>(TAG_CONN);
        packagerTags.Save();

        packager.Set<int,CIMListCode *>(PK_TAGS, &codeTags);
        packager.Save();

        CBuffer bufSend;
        PACKET sp;
        memset(&sp, 0, sizeof(sp));
        sp.SetAsIMCode();
        sp.Pack(&bufSend
                ,(char *)code.GetRawBuffer()->Begin()
                ,code.GetRawBuffer()->GetDataSize());

        Send((char *)bufSend.Begin(), bufSend.GetDataSize());
    }

    void CRoutePeer::OnDeliver(CDictPackager *p, bool rewrap){

        DEBUG_STATUS(">>> Has deliver-packet.");
        //解开包装，发现真实目标和原始数据
        size_t count = 0;
        CIMDictCode codeRaw;
        if(rewrap)
        {
			DEBUG_STATUS(">>> unwrap packet.");
        	CIMDictCode codewrap;
            p->Get<int, CIMDictCode *>(PK_BODY, &codewrap, count);
			p->Load(&codewrap);
        }

        CIMListCode codeTo;
        p->Get<int, CIMListCode *>(PK_TO, &codeTo, count);
		p->Get<int, CIMDictCode *>(PK_BODY, &codeRaw, count);


        CDictPackager packager(&codeRaw);

        int tp = packager.Get<int, int>(PK_TYPE);
        int req = packager.Get<int, int>(PK_REQUEST);
        int tag = packager.Get<int, int>(PK_TAG);
        if(
			(tp == CSetting::GetInstance()->getLoginMsgType() || req == CSetting::GetInstance()->getLoginMsgType())
			&& tag == CSetting::GetInstance()->getLoginMsgTag()
		){
			CSetting::GetInstance()->decLoginQueue();
		}

        //包装成中间代码流
        m_bufSend.Reset();
        memset(&m_pckSend, 0, sizeof(m_pckSend));
        m_pckSend.SetAsIMCode();
        m_pckSend.Pack(&m_bufSend
                ,(char *)codeRaw.GetRawBuffer()->Begin()
                ,codeRaw.GetRawBuffer()->GetDataSize());

        //如果开启了json客户端支持，那么再包装一个json流
        if(CSetting::GetInstance()->IsJsonSupport()){
            m_bufSendJson.Reset();
            memset(&m_pckSend, 0, sizeof(m_pckSend));
            m_pckSend.SetAsJson();

            m_bufDecodeJson.Reset();
            CJsonCodec::GetInstance()->GetDecoder()->Decode(&packager, &m_bufDecodeJson);

            m_pckSend.Pack(&m_bufSendJson
                    ,(char *)m_bufDecodeJson.Begin()
                    ,m_bufDecodeJson.GetDataSize());
        }

        //遍历目标列表，分别发往每个目标
        CListPackager pp(&codeTo);
        //没有指定目标的直接发送到所有客户端
        if(pp.GetSize() == 0)
        {
			DEBUG_STATUS(">>> Send all-world message.");
            NBPy_UNORDERED_MAP<int, IPeer *> &peers = CPeerFactory::GetInstance()->GetPeers();
            for(NBPy_UNORDERED_MAP<int, IPeer *>::iterator kv = peers.begin();
					kv != peers.end();
					kv ++){
                CClientPeer *peer = NULL;
                try{
                	peer = dynamic_cast<CClientPeer *>(kv->second);
                }catch(...){}
                if(peer)
                {
                    peer->MutilSend(&m_bufSend, &m_bufSendJson);
                }
            }
        }else{
            for(int i = 0; i < pp.GetSize(); i ++)
            {
                unsigned int fd = pp.At<long>(i);
				DEBUG_STATUS(">>> Send deliver-message to %d.", fd);
				if(fd == 0)
				{
					CBuffer bufOut;
					CJsonCodec::GetInstance()->GetDecoder()->Decode(p, &bufOut);
					DEBUG_STATUS("FUCK PACKET!!!!!!!!!!");
					DEBUG_STATUS((const char *)bufOut.Begin());
				}
                CClientPeer *peer = NULL;
                try{
                	peer = dynamic_cast<CClientPeer *>(CPeerFactory::GetInstance()->FindPeerBySID(fd));
                }catch(...){}
                if(peer)
                {
                    peer->MutilSend(&m_bufSend, &m_bufSendJson);
                }
            }
        }
    }

    void CRoutePeer::Deliver(int tag, IIMCode *code) {
        CIMDictCode codeWrap;
        CDictPackager packager(&codeWrap);
        packager.Set<int,int>(PK_SCHEME, PS_S2R);
        packager.Set<int,int>(PK_TYPE, PT_DELIVER);
        packager.Set<int,int>(PK_TAG, tag);

        size_t l = 0;
        packager.Set<int,CIMDictCode *>(PK_BODY, (CIMDictCode *)code, l);
		packager.Save();

        PACKET sp;
        memset(&sp, 0, sizeof(sp));
        sp.SetAsIMCode();
        sp.Pack(&m_sendbuffer
                ,(char *)codeWrap.GetRawBuffer()->Begin()
                ,codeWrap.GetRawBuffer()->GetDataSize());

        Send(NULL, 0);
    }

    void CRoutePeer::DeliverToPid(int pid, IIMCode *code) {
        CIMDictCode codeWrap;
        CIMListCode codePids;
        CDictPackager packager(&codeWrap);
        CListPackager packagerPids(&codePids);
        packagerPids.Append<int>(pid);
        packagerPids.Save();

        packager.Set<int,int>(PK_SCHEME, PS_S2R);
        packager.Set<int,int>(PK_TYPE, PT_DELIVER);
        packager.Set<int,CIMListCode *>(PK_TO, &codePids);

        size_t l = 0;
        packager.Set<int,CIMDictCode *>(PK_BODY, (CIMDictCode *)code, l);
		packager.Save();

        PACKET sp;
        memset(&sp, 0, sizeof(sp));
        sp.SetAsIMCode();
        sp.Pack(&m_sendbuffer
                ,(char *)codeWrap.GetRawBuffer()->Begin()
                ,codeWrap.GetRawBuffer()->GetDataSize());

        Send(NULL, 0);
    }
NSE

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
 * File: peer.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "stacklessengine.h"
#include "peer.h"
#include "codec_json.h"
#include "pool.h"

NS(NBPy)

    void CPeer::Disconnect(bool unlink) {

        if(m_raw_sck == NBPy_INVALID_SOCKET)
            return;
        int s = m_raw_sck;
        m_raw_sck = NBPy_INVALID_SOCKET;
        if(!OnClose() && m_engine){
            m_engine->OnClose(s, GetSID());
        }
        CSocket::Close(m_socket_tp, s, unlink);
        OnTerminate();
    }

    CPeer::~CPeer() {
    }


    CPeer::CPeer()
            :m_engine(NULL)
            ,m_raw_sck(NBPy_INVALID_SOCKET)
            ,m_socket_tp(eAT_TCP)
            ,m_compress_supported(false){
        Init();
    }

    CPeer::CPeer(int sck)
            :m_engine(NULL)
            ,m_raw_sck(NBPy_INVALID_SOCKET)
            ,m_socket_tp(eAT_TCP)
            ,m_compress_supported(false) {
        Init(sck);
    }

    void CPeer::Init(int sck) {
        m_raw_sck = sck;
        if(m_raw_sck != NBPy_INVALID_SOCKET)
            CEventDispatcher::GetInstance()->Bind(m_raw_sck, this);
    }

    bool CPeer::Connect(const char *szAddr, unsigned int uPort)
    {
        if(m_socket_tp == eAT_UNIX_TCP || m_socket_tp == eAT_UNIX_UDP)
        {
            m_raw_sck = CSocket::ConnectToUnix(m_socket_tp, szAddr);
        }else
            m_raw_sck = CSocket::Connect(szAddr, uPort);

        if (m_raw_sck == NBPy_INVALID_SOCKET)
        {
            return false;
        }
        if(!OnConnect() && m_engine){
            m_engine->OnConnect(m_raw_sck, GetSID(), szAddr, uPort);
        }
        CEventDispatcher::GetInstance()->Bind(m_raw_sck, this);

        return true;
    }


    void CPeer::OnEvent(LPEVENT ev)
    {
		//DEBUG_STATUS(">>>>>>>>>>> CPeer::OnEvent");
        int event_sets = ev->event_sets;
        if (NBPy_VALIDATE_MASK(event_sets, ETP_ERROR))
        {
            return;
        }
        if (NBPy_VALIDATE_MASK(event_sets, ETP_READ_READY))
        {
            ParseData();
        }
    }

    int CPeer::Send(char *s, size_t l) {

        //DEBUG_STATUS("CPeer::Send crc %d", crc(s, l));
        if(s)
            m_sendbuffer.Push(s, l);

        //尝试主动发送一下
        OnEventWrite(NULL);

    	//DEBUG_STATUS("CPeer::Send");
        return l;
    }

    PEERID CPeer::GetId() {
        return m_raw_sck;
    }

    int CPeer::GetIOHandle() {
        return m_raw_sck;
    }

    void CPeer::OnEventError(LPEVENT) {

    }

    void CPeer::OnEventRead(LPEVENT e) {
		//DEBUG_STATUS(">>>>>>>>>>> CPeer::OnEventRead");
        if(e->data > 0)
        {
            ssize_t tr = 0;
            size_t l = e->data;
            do {
                char *ss = (char *) CSimplePool::GetInstance()->Alloc(l);
                ssize_t r = CSocket::Recv(m_raw_sck, ss, l);
                if (r > 0)
                {
                    m_recvbuffer.Push(ss, (size_t) r);
                }else if(r < 0){
                    CSimplePool::GetInstance()->Free(ss);
                    Close();
                    return;
                }
                CSimplePool::GetInstance()->Free(ss);
                tr += r;
                if (tr == e->data)
                    break;
                l = e->data - tr;
            }while(l > 0);
        } else {
            char szTmp[NBPy_IO_RECV_BUFFSIZE];
            ssize_t cr = 0;
            do{
                cr = CSocket::Recv(m_raw_sck, szTmp, NBPy_IO_RECV_BUFFSIZE);
                if (cr > 0)
                {
                    m_recvbuffer.Push(szTmp, cr);
                }else if(cr < 0){
                    Close();
                    return;
                }
            }while(cr > 0);
        }
    }

    void CPeer::OnEventWrite(LPEVENT e) {
		//DEBUG_STATUS(">>>>>>>>>>> CPeer::OnEventWrite");
        if(m_sendbuffer.GetDataSize() == 0)
            return;

        if(e && e->data > 0)
        {
            CBuffer b;
            size_t ss = MIN(e->data, m_sendbuffer.GetDataSize());
            b.Push(m_sendbuffer.Begin(), ss);
            ssize_t r = CSocket::Send(m_raw_sck, b.Begin(), ss);
            if(r > 0)
            {
                m_sendbuffer.Pop(r);
            }
            if(r < 0){
                Close();
                return;
            }
        } else {
            CBuffer b;
            ssize_t r = 0;
            do {
                b.Reset();
                size_t ss = MIN(NBPy_IO_RECV_BUFFSIZE, m_sendbuffer.GetDataSize());
                b.Push(m_sendbuffer.Begin(), ss);
                r = CSocket::Send(m_raw_sck, b.Begin(), ss);
                if(r > 0)
                {
					//DEBUG_STATUS("CPeer::Send >> CSocket::Send >> %d", r);
                    m_sendbuffer.Pop(r);
                }
                if(r < 0){
                    Close();
                    return;
                }
            }while(r > 0 && m_sendbuffer.GetDataSize() > 0);
        }
    }

    bool CPeer::OnReadReady(LPPACKET p) {
        return false;
    }

    void CPeer::ParseData() {
		//DEBUG_STATUS(">>>>>>>>>>> CPeer::OnEventWrite");
        size_t hs = sizeof(PACKET_FIXED_HEADER);

        while(m_recvbuffer.GetDataSize() >= hs){
            LPPACKET_FIXED_HEADER header = (LPPACKET_FIXED_HEADER)m_recvbuffer.Begin();
            if(header->length > m_recvbuffer.GetDataSize())
            {
                //data is uncomplete!
                break;
            }

            LPPACKET p = (LPPACKET)header;
            if(!p->IsValid())
            {
				DEBUG_STATUS("PACKET crc error %u <> %u", crc(p->GetData(), p->header.zclength), p->header.crc);
				FILE *F = fopen("./invalidpack", "w");
				fwrite(p->GetData(), sizeof(char), p->header.zclength, F);
				fclose(F);
                Close();
                return;
            }

            //DEBUG_STATUS("PACKET crc %d", crc(p, p->header.length));

            if(!OnReadReady(p) && m_engine){
                m_engine->OnPacketIn(m_raw_sck, p);
            }
            m_recvbuffer.Pop(p->header.length);
        }
    }

    void CPeer::OnError() {
        Disconnect(m_socket_tp == eAT_UNIX_TCP || m_socket_tp == eAT_UNIX_UDP);
    }
NSE

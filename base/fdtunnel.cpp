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
 * File: fdtunnel.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "fdtunnel.h"

NS(NBPy)

    CFDTunnel::CFDTunnel()
            :m_raw_sck(NBPy_INVALID_SOCKET) {

    }

    CFDTunnel::~CFDTunnel() {
        UnBind();
    }

    void CFDTunnel::Bind(const char *addr) {
        m_raw_sck = CSocket::ListenAtUnix(eAT_UNIX_UDP, addr);
        CEventDispatcher::GetInstance()->Bind(m_raw_sck, this);
    }

    void CFDTunnel::UnBind() {
        if(m_raw_sck == NBPy_INVALID_SOCKET)
            return;
        CEventDispatcher::GetInstance()->UnBind(m_raw_sck);
        CSocket::Close(eAT_UNIX_UDP, m_raw_sck, true);
    }

    void CFDTunnel::OnEventRead(LPEVENT e) {
        int fds[255];
        ssize_t r = CSocket::RecvFds(m_raw_sck,fds,255);
        if(r < 0)
            return;
        OnFDIn(fds, r);
    }

    bool CFDTunnel::Send(int s, const char *szDstAddr) {
        return CSocket::SendFd(m_raw_sck, s, szDstAddr);
    }
NSE
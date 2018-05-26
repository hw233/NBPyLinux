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
 * File: network.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "network.h"
#include "codec_json.h"
#include "datetime.h"
#include "pool.h"
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/un.h>

NS(NBPy)


    bool PACKET::IsValid()
    {
        //return (header.length_masked == NBPy_MASK_PACKET(this));
        return header.crc == crc(GetData(), header.zclength);
    }

    void PACKET::Pack(IBuffer* p_outbuf, char *buf, size_t size, bool compress)
    {
        this->header.base.verifiymask = (unsigned short)CNetworkUtils::GetInstance()->GetPacketMask();
#if(BL_COMPRESS_ENABLED == 1)
        if(compress && size >= NBPy_COMPRESS_MIN_LENGTH)
        {
            this->header.base.flag |= ePF_COMPRESS;
        }
#endif
        this->header.zclength = size;
        this->header.length = size + sizeof(PACKET);
#if(BL_COMPRESS_ENABLED == 1)
        if(NBPy_VALIDATE_MASK(this->header.base.flag, ePF_COMPRESS))
        {
            uLongf ziped_size = ::compressBound(size);
            p_outbuf->Resize(sizeof(PACKET) + ziped_size);
            LPPACKET pPacket = (LPPACKET)p_outbuf->Begin();
            if( ::compress((Bytef *)pPacket->GetData(), &ziped_size, (Bytef *)buf, size) == Z_OK )
            {
                memcpy(pPacket, this, sizeof(PACKET));
                pPacket->header.length = ziped_size + sizeof(PACKET);
                pPacket->MakeValid();
                p_outbuf->Seek(pPacket->header.length);
                return;
            }
            else
            {
                this->header.base.flag ^= ePF_COMPRESS;
                p_outbuf->Reset();
                return;
            }
        }
#endif
        //this->MakeValid();
        this->header.crc = crc(buf, size);
        p_outbuf->Push(this, sizeof(PACKET));
        p_outbuf->Push(buf, size);
    }

    void PACKET::UnPack(IBuffer* p_outbuf)
    {
#if(BL_COMPRESS_ENABLED == 1)
        if(BL_VALIDATE_MASK(this->header.base.flag, ePF_COMPRESS))
    {
        uLongf ziped_size = this->header.zclength;
        if(ziped_size == 0)
            return;
        p_outbuf->Expand(ziped_size + sizeof(PACKET));
        LPPACKET pPacket = (LPPACKET)p_outbuf->GetCurrPtr();
        int r = Z_OK;
        if( (r = ::uncompress((Bytef *)pPacket->GetData(), &ziped_size, (Bytef *)this->GetData(), this->header.length - sizeof(PACKET))) == Z_OK )
        {
            memcpy(pPacket, this, sizeof(PACKET));
            pPacket->header.length = ziped_size + sizeof(PACKET);
            pPacket->header.base.flag ^= ePF_COMPRESS;
            pPacket->header.length_masked = MASK_PACKET(pPacket);
            p_outbuf->SetOffset(pPacket->header.length);
        }
        else
        {
            p_outbuf->Reset();
        }
        return;
    }
#endif
        p_outbuf->Push(this->GetData(), this->header.zclength);
    }

    CPacketDecoder::CPacketDecoder(LPPACKET p)
    : m_data_inited(false) {
        m_packet = p;
    }

    IIMCodePackager* CPacketDecoder::GetCodePackager() {
        if(!m_data_inited)
        {
            if(!m_packet->IsValid())
                return NULL;

            CBuffer buf;
            m_packet->UnPack(&buf);

            if(m_packet->IsJson())
            {
                CJsonCodec::GetInstance()->GetEncoder()->Encode(&m_mapdata, &buf);
            }else if(m_packet->IsIMCode())
            {
                //CIMDictCode code;
                //code.Load(&buf);
                m_mapdata.Load(&buf);
            }else
            {
                return NULL;
            }
            m_data_inited = true;
        }
        return &m_mapdata;
    }

    CPacketDecoder::~CPacketDecoder() {
        m_mapdata.Clear();
    }

    void CPeerFactory::Initialize() {
    	m_SIDPREFIX = 1;
    }

    void CPeerFactory::Destroy() {
        for(NBPy_UNORDERED_MAP<int, IPeer *>::iterator kv = m_peers.begin();
			kv != m_peers.end();
			kv ++
			){
            kv->second->OnError();
        }
        m_peers.clear();
    }

    bool CPeerFactory::RegisterPeer(IPeer *pPeer)
    {
        if(!pPeer)
            return false;
        m_peers[pPeer->GetId()] = pPeer;
        m_sidpeers[pPeer->GetSID()] = pPeer;
        return true;
    }

    void CPeerFactory::UnRegisterPeer(IPeer *pPeer)
    {
        m_peers.erase(pPeer->GetId());
        m_sidpeers.erase(pPeer->GetSID());
    }

    IPeer *CPeerFactory::FindPeer(int s)
    {
        NBPy_UNORDERED_MAP<int, IPeer *>::iterator it = m_peers.find(s);
        if(it == m_peers.end())
            return NULL;
        return it->second;
    }

    IPeer *CPeerFactory::FindPeerBySID(unsigned int sid)
    {
        NBPy_UNORDERED_MAP<unsigned int, IPeer *>::iterator it = m_sidpeers.find(sid);
        if(it == m_sidpeers.end())
            return NULL;
        return it->second;
    }

    void CNetworkUtils::Initialize() {
        m_nPacket_Mask = 0;
    }

    void CNetworkUtils::Destroy() {
    }

    unsigned int CNetworkUtils::GetAliveSocketCount() {
        return m_nSocketCount;
    }

    int CNetworkUtils::CreateSocket(char adapter, int domain) {
        int _domain = domain;
        int tp = SOCK_STREAM;

        if(adapter == eAT_UNIX_TCP || adapter == eAT_UNIX_UDP)
            _domain = AF_UNIX;
        if(adapter == eAT_UDP || adapter == eAT_UNIX_UDP)
        {
            tp = SOCK_DGRAM;
        }

        int s = socket( _domain, tp, 0 );
        if ( s != NBPy_INVALID_SOCKET )
        {
            m_nSocketCount ++;
        }
        return s;
    }

    void CNetworkUtils::CloseSocket(int s) {
        if ( s != NBPy_INVALID_SOCKET )
        {
            m_nSocketCount --;
        }
        close(s);
    }

    void CNetworkUtils::SetSocketOpt(char adapter, int s, int linger_flag) {
        //设置socket的各类特性，在某些特殊环境里需要
        int opts = fcntl(s, F_GETFL);
        if(!NBPy_VALIDATE_MASK(opts, O_NONBLOCK))
        {
			opts |= O_NONBLOCK;
			fcntl(s, F_SETFL, opts);
        }

        if(adapter != eAT_TCP && adapter != eAT_UNIX_TCP){
			return;
        }

        struct linger ling;
        ling.l_onoff = linger_flag;
        ling.l_linger = 0;
        //setsockopt( s, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling));

        int bNoDelay = 1;
        setsockopt( s, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(int));
    }

    int CNetworkUtils::BuildSockAddr(addrinfo *dst, char adapter, const char *szAddr, unsigned int uPort) {
        struct addrinfo hints;
        bzero(&hints, sizeof(hints));
        hints.ai_flags = AI_PASSIVE;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_IP;
        int ret = 0;

        char szPort[255];
        sprintf(szPort, "%d", uPort);

        ret = getaddrinfo(szAddr, szPort, &hints, &dst);

        //DEBUG_STATUS("getaddrinfo %d for %s:%d", ret, szAddr, uPort);
        return ret;
    }

    sockaddr *CNetworkUtils::BuildSockAddr(char adapter, const char *szAddr, unsigned int uPort, socklen_t *addrlen )
    {
        if(adapter == eAT_UNIX_TCP || adapter == eAT_UNIX_UDP)
        {
            static sockaddr_un un;
            memset(&un, 0, sizeof(un));
            un.sun_family = AF_UNIX;
            strcpy(un.sun_path, szAddr);

            *addrlen = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);
            return (sockaddr *)&un;
        }else
        {
            static sockaddr_in sockAddr;
            memset(&sockAddr,0,sizeof(sockaddr_in));
            sockAddr.sin_family = AF_INET;
            sockAddr.sin_addr.s_addr = inet_addr(szAddr);
            sockAddr.sin_port = htons(uPort);
            *addrlen = sizeof(sockaddr_in);
            return (sockaddr *)&sockAddr;
        }
        return NULL;
    }

    unsigned int CNetworkUtils::GetPacketMask() {
        return m_nPacket_Mask;
    }

    void CNetworkUtils::SetPacketMask(unsigned int mask) {
        m_nPacket_Mask = mask;
    }

    bool CNetworkUtils::IsTGWMode() {
        return m_bTGW;
    }

    void CNetworkUtils::SetTGWMode(bool b) {
        m_bTGW = b;
    }

    size_t CSocket::RecvTotal = 0;
    size_t CSocket::SendTotal = 0;
    size_t CSocket::RecvFromTotal = 0;
    size_t CSocket::SendToTotal = 0;

    void CSocket::Close(char socket_tp, int s, bool bunlink)
    {
        if ( bunlink && (socket_tp == eAT_UNIX_TCP || socket_tp == eAT_UNIX_UDP))
        {
            sockaddr_un uSelfAddr, uRemoteAddr;
            sockaddr *pSelfAddr = (sockaddr *)&uSelfAddr;
            sockaddr *pRemoteAddr = (sockaddr *)&uRemoteAddr;
            GetSockAddr(socket_tp, s, pSelfAddr, pRemoteAddr);
            unlink(uSelfAddr.sun_path);
        }
        CNetworkUtils::GetInstance()->CloseSocket(s);
    }

    void CSocket::GetSockAddr(char socket_tp, LPEVENT ev, sockaddr *pSelfAddr, sockaddr *pRemoteAddr)
    {
        if(!ev)
            return;
        GetSockAddr(socket_tp, ev->fd, pSelfAddr, pRemoteAddr);
    }

    void CSocket::GetSockAddr(char socket_tp, int s, sockaddr *pSelfAddr, sockaddr *pRemoteAddr)
    {
        if (socket_tp == eAT_UNIX_TCP || socket_tp == eAT_UNIX_UDP)
        {
            socklen_t st = sizeof(sockaddr_un);
            getpeername(s, pRemoteAddr, &st);

            st -= offsetof(struct sockaddr_un, sun_path);
            ((sockaddr_un *)pRemoteAddr)->sun_path[st] = 0;

            st = sizeof(sockaddr_un);
            getsockname(s, pSelfAddr, &st);

            st -= offsetof(struct sockaddr_un, sun_path);
            ((sockaddr_un *)pSelfAddr)->sun_path[st] = 0;
        }else
        {
            socklen_t st = sizeof(struct sockaddr_storage);
            getpeername(s, pRemoteAddr, &st);
            st = sizeof(struct sockaddr_storage);
            getsockname(s, pSelfAddr, &st);
        }
    }

    void CNetworkUtils::FillAcceptPeerInfo(LPACCEPT_PEER_INFO info){
        sockaddr_storage iaddr;
        sockaddr_storage iaddr_2;

        CSocket::GetSockAddr(eAT_TCP, info->fd, (sockaddr *)&iaddr, (sockaddr *)&iaddr_2);

        info->af = iaddr_2.ss_family;

        if (iaddr.ss_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *)&iaddr;
            info->l_port = ntohs(s->sin_port);
            inet_ntop(AF_INET, &s->sin_addr, info->l_addr, NBPy_NETWORK_ADDRSIZE);
            s = (struct sockaddr_in *)&iaddr_2;
            info->r_port = ntohs(s->sin_port);
            inet_ntop(AF_INET, &s->sin_addr, info->r_addr, NBPy_NETWORK_ADDRSIZE);
        } else { // AF_INET6
            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&iaddr;
            info->l_port = ntohs(s->sin6_port);
            inet_ntop(AF_INET6, &s->sin6_addr, info->l_addr, NBPy_NETWORK_ADDRSIZE);
            s = (struct sockaddr_in6 *)&iaddr_2;
            info->r_port = ntohs(s->sin6_port);
            inet_ntop(AF_INET6, &s->sin6_addr, info->r_addr, NBPy_NETWORK_ADDRSIZE);
        }
    }


    int CSocket::Accept(int s, sockaddr *addr, socklen_t &len) {
        int fd = ::accept(s, addr, &len);
        if(fd == -1)
        {
        	fd = NBPy_INVALID_SOCKET;
        }else{
			CNetworkUtils::GetInstance()->SetSocketOpt(eAT_TCP, fd, 1);
        }
        return fd;
    }

    int CSocket::Connect(const char *szAddr, unsigned int uPort) {
        char sport[6];
        sprintf(sport, "%d", uPort);

        int listenfd, ret;
        struct addrinfo hints, *res, *ressave;
        bzero(&hints, sizeof(hints));
        hints.ai_flags = AI_PASSIVE;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_IP;

        if (0 != (ret = getaddrinfo(szAddr, sport, &hints, &res)))
        {
            //DEBUG_STATUS("getaddrinfo error: %d", gai_strerror(ret));
            return NBPy_INVALID_SOCKET;
        }

		listenfd = NBPy_INVALID_SOCKET;
        ressave = res;
        while(NULL != res)
        {
            if (-1 == (listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)))
            {
                DEBUG_STATUS("create socket error: %s", strerror(errno));
                res = res->ai_next;
                continue;
            }

            if (-1 == ::connect(listenfd, res->ai_addr, res->ai_addrlen))
            {
                DEBUG_STATUS("connect error: %s, %d", strerror(errno));
                close(listenfd);
                res = res->ai_next;
                continue;
            }

			CNetworkUtils::GetInstance()->SetSocketOpt(eAT_TCP, listenfd, 1);
            break;
        }

        freeaddrinfo(ressave);

        if (NULL == res)
            return NBPy_INVALID_SOCKET;

        return listenfd;
    }

    int CSocket::Listen(const char *host, unsigned int port)
    {
        char sport[6];
        sprintf(sport, "%d", port);

        int listenfd, ret;
        const int on = 1;
        struct addrinfo hints, *res, *ressave;
        bzero(&hints, sizeof(hints));
        hints.ai_flags = AI_PASSIVE;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_IP;

        if (0 != (ret = getaddrinfo(host, sport, &hints, &res)))
        {
            DEBUG_STATUS("getaddrinfo error: %d", gai_strerror(ret));
            return NBPy_INVALID_SOCKET;
        }

		listenfd = NBPy_INVALID_SOCKET;
        ressave = res;
        while(NULL != res)
        {
            if (-1 == (listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)))
            {
                DEBUG_STATUS("create socket error: %s", strerror(errno));
                res = res->ai_next;
                continue;
            }

            if (-1 == setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))
            {
                DEBUG_STATUS("setsockopt SO_REUSEADDR error: %s", strerror(errno));
                close(listenfd);
                res = res->ai_next;
                continue;
            }

			if(res->ai_family == AF_INET6){
				if (setsockopt(listenfd, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)) < 0)
				{
					DEBUG_STATUS("setsockopt IPV6_V6ONLY error: %s", strerror(errno));
					close(listenfd);
					res = res->ai_next;
					continue;
				}
				}

			CNetworkUtils::GetInstance()->SetSocketOpt(eAT_TCP, listenfd, 1);

            if (-1 == ::bind(listenfd, res->ai_addr, res->ai_addrlen))
            {
                DEBUG_STATUS("bind error: %s", strerror(errno));
                close(listenfd);
                res = res->ai_next;
                continue;
            }

            if (-1 == ::listen(listenfd, SOMAXCONN))
            {
                DEBUG_STATUS("listen error: %s, %d", strerror(errno));
                close(listenfd);
                res = res->ai_next;
                continue;
            }

            break;
        }

        freeaddrinfo(ressave);

        if (NULL == res)
            return NBPy_INVALID_SOCKET;

        return listenfd;
    }

    int CSocket::ConnectToUnix(char socket_tp, const char *szAddr) {

        int s = CNetworkUtils::GetInstance()->CreateSocket(socket_tp);
        if (s == NBPy_INVALID_SOCKET)
        {
            DEBUG_ERROR("Create client socket fail!");
            return NBPy_INVALID_SOCKET;
        }

        socklen_t addrsize = 0;
        char sztmpaddr[108];
        sprintf(sztmpaddr, NBPy_DATATUNNEL_ADDRPREFIX, getpid());
        sockaddr *paddr = CNetworkUtils::GetInstance()->BuildSockAddr(socket_tp, sztmpaddr, 0, &addrsize);
        if (::bind(s, paddr, addrsize) != 0) {
            DEBUG_ERROR("Bind local address %s fail!", sztmpaddr);
            return NBPy_INVALID_SOCKET;
        }

        addrsize = 0;
        paddr = CNetworkUtils::GetInstance()->BuildSockAddr(socket_tp, szAddr, 0, &addrsize);
        if (0 != ::connect(s, paddr, addrsize)) {
            DEBUG_ERROR("Connect to %s fail! code: %d", szAddr, errno);
            Close(socket_tp, s, true);
            return NBPy_INVALID_SOCKET;
        }

        CNetworkUtils::GetInstance()->SetSocketOpt(socket_tp, s, 1);
        DEBUG_STATUS("Connect to %s suscess.", szAddr);
        return s;
    }

    int CSocket::ListenAtUnix(char socket_tp, const char *szAddr) {
        unlink(szAddr);
        int s = CNetworkUtils::GetInstance()->CreateSocket(socket_tp);
        if (s == NBPy_INVALID_SOCKET)
        {
            DEBUG_ERROR("Create unix socket fail!");
            return NBPy_INVALID_SOCKET;
        }
        CNetworkUtils::GetInstance()->SetSocketOpt(socket_tp, s, 1);

        socklen_t addrsize = 0;
        sockaddr* paddr = CNetworkUtils::GetInstance()->BuildSockAddr(socket_tp, szAddr, 0, &addrsize);

        int nRet = ::bind(s, paddr, addrsize);
        if (nRet == -1)
        {
            if( errno == EADDRINUSE )
            {
                DEBUG_ERROR("Bind fail, address inuse! %s", szAddr);
                return NBPy_INVALID_SOCKET;
            }
            else
            {
                DEBUG_ERROR("Bind fail! %s", szAddr);
                return NBPy_INVALID_SOCKET;
            }
        }

        if (socket_tp == eAT_UNIX_TCP || socket_tp == eAT_UNIX_UDP)
        {
            DEBUG_STATUS("bind on %s suscess.", szAddr);
        }
        return s;
    }

    ssize_t CSocket::Send(int &s, void *pData, size_t nSize) {
        ssize_t r = ::send(s, (char *)pData, nSize, 0);
        if(r < 0)
        {
            int err = errno;
            //if(err != 0 && err != EAGAIN && err != EWOULDBLOCK )
            if(err != 0 && err != EINTR && err != EWOULDBLOCK && err != EAGAIN)
            {
                return -1;
                DEBUG_ERROR("Send data fail! code: %d", err);
            }
            return 0;
        }
        SendTotal += r;
        return r;
    }

    ssize_t CSocket::Recv(int &s, void *pBuffer, size_t nBufferSize) {
        ssize_t r = ::recv(s, (char *)pBuffer, nBufferSize, 0);
        if(r < 0)
        {
            int err = errno;
            if(err != 0 && err != EINTR && err != EWOULDBLOCK && err != EAGAIN)
            {
                return -1;
                DEBUG_ERROR("Recv data fail! code: %d", err);
            }
            return 0;

        }else if (r == 0){
			//DEBUG_STATUS("target is disconnect %d", s);
        	return -1;
        }
        RecvTotal += r;
        return r;
    }

    ssize_t CSocket::RecvFrom(int &s, void *pBuffer, size_t nBufferSize, char *srcAddr) {
        struct sockaddr_un src_addr;
        memset(&src_addr, 0, sizeof(src_addr));
        socklen_t l = sizeof(src_addr);
        ssize_t r = ::recvfrom(s, (char *)pBuffer, nBufferSize, MSG_DONTWAIT, (sockaddr *)&src_addr, &l);
        if(r < 0)
        {
            int err = errno;
            if(err != 0 && err != EINTR && err != EWOULDBLOCK && err != EAGAIN)
            {
                return -1;
                DEBUG_ERROR("Recv data fail! code: %d", err);
            }
            return 0;

        }else if(r == 0)
        {
            return -1;
        }

        //DEBUG_ERROR("src_addr.sun_path >> %s", src_addr.sun_path);

        strcpy(srcAddr, src_addr.sun_path);

        RecvFromTotal += r;
        return r;
    }

    ssize_t CSocket::SendTo(int &s, void *pData, size_t nSize, const char *srcAddr) {
        struct sockaddr_un src_addr;
        memset(&src_addr, 0, sizeof(src_addr));
        src_addr.sun_family = AF_UNIX;
        strcpy(src_addr.sun_path, srcAddr);
        socklen_t l = sizeof(src_addr);
		size_t sendlen = 0;
		char *sendptr = (char *)pData;

		int sck = CNetworkUtils::GetInstance()->CreateSocket(eAT_UNIX_UDP);
		if(sck == NBPy_INVALID_SOCKET)
			return 0;

		char szAddr[255];
		sprintf(szAddr, NBPy_DATATUNNEL_ADDRPREFIX2, getpid(), CDateTime::timestamp());

		CNetworkUtils::GetInstance()->SetSocketOpt(eAT_UNIX_UDP, sck, 1);
        socklen_t addrsize = 0;
        sockaddr* paddr = CNetworkUtils::GetInstance()->BuildSockAddr(eAT_UNIX_UDP, szAddr, 0, &addrsize);

        int nRet = ::bind(sck, paddr, addrsize);
        if (nRet == -1)
        {
            if( errno == EADDRINUSE )
            {
            	::unlink(szAddr);
				nRet = ::bind(sck, paddr, addrsize);
				if( errno == EADDRINUSE )
				{
					DEBUG_ERROR("严重错误!!! %s 被占用无法绑定", szAddr);
					::close(sck);
					return 0;
				}
            }
            else
            {
				DEBUG_ERROR("严重错误!!! %s 无法绑定错误号%d", szAddr, errno);
            	::unlink(szAddr);
				::close(sck);
                return 0;
            }
        }

		size_t sendtimes = 0;
		while(sendlen < nSize)
		{
			size_t tosend = nSize - sendlen;
			if(sendlen > 0)
				sendptr = CMemory<char>::MovePtr(pData, sendlen);
			ssize_t r = ::sendto(sck, sendptr, tosend, MSG_DONTROUTE, (sockaddr *)&src_addr, l);
			if ( 0 < r )
			{
				sendlen += r;
				SendToTotal += r;
				continue;
			}
			else if( 0 == errno || EAGAIN == errno || EWOULDBLOCK == errno || EINTR == errno )
			{
				if(!r)
				{
					DEBUG_ERROR("严重错误1111!!! sendto %s 失败！！！错误号%d", srcAddr, errno);
					break;
				}
				break;
			}
			else
			{
				DEBUG_ERROR("严重错误2222!!! sendto %s 失败！！！错误号%d", srcAddr, errno);
				break;
			}
		}

		::close(sck);
		::unlink(szAddr);

		//DEBUG_STATUS("IPC-SENDTIEMS: %ld",sendtimes);
        return sendlen;
    }

    bool CSocket::SendFd(int dst, int fd, const char *srcAddr) {

        struct cmsghdr *m_cmptr = (struct cmsghdr*)CSimplePool::GetInstance()->Alloc(sizeof(struct cmsghdr) + sizeof(int));
        if (m_cmptr == NULL)
            return false;

        struct sockaddr_un src_addr;
        memset(&src_addr, 0, sizeof(src_addr));
        src_addr.sun_family = AF_UNIX;
        strcpy(src_addr.sun_path, srcAddr);
        socklen_t l = sizeof(src_addr);

        struct iovec iov;
        struct msghdr msg;
        char buf[2];
        memset(buf, 0, 2);
        iov.iov_base = buf;
        iov.iov_len = 2;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_name = (void *)&src_addr;
        msg.msg_namelen = l;
        m_cmptr->cmsg_level = SOL_SOCKET;
        m_cmptr->cmsg_type = SCM_RIGHTS;
        m_cmptr->cmsg_len = sizeof(struct cmsghdr) + sizeof(int);
        msg.msg_control = m_cmptr;
        msg.msg_controllen = sizeof(struct cmsghdr) + sizeof(int);

        memcpy(CMSG_DATA(m_cmptr),&fd, sizeof(fd));

        bool r = true;
        if (sendmsg(dst, &msg, 0) != 2)
        {
            r = false;
        }else
            ::close(fd);

        CSimplePool::GetInstance()->Free(m_cmptr);
        return r;
    }

    ssize_t CSocket::RecvFds(int dst, int *fds, size_t l) {

        struct cmsghdr *m_cmptr = (struct cmsghdr*)CSimplePool::GetInstance()->Alloc(sizeof(struct cmsghdr) + sizeof(int));
        if (m_cmptr == NULL)
            return 0;

        ssize_t nr = 0;
        int status;
        char *ptr;
        char buf[100];
        struct iovec iov;
        struct msghdr msg;

        status = -1;

        iov.iov_base = buf;
        iov.iov_len = sizeof(buf);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_name = NULL;
        msg.msg_namelen = 0;

        msg.msg_control = m_cmptr;
        msg.msg_controllen = sizeof(struct cmsghdr) + sizeof(int);
        if ((nr = recvmsg(dst, &msg, 0)) <= 0)
        {
            CSimplePool::GetInstance()->Free(m_cmptr);
            if ( !( 0 == errno || EAGAIN == errno || EWOULDBLOCK == errno || EINTR == errno ) )
                return -1;
            return 0;
        }

        ssize_t st = 0;
        for (ptr = buf; ptr < &buf[nr]; )
        {
            if(st >= (ssize_t)l)
            {
                DEBUG_STATUS("RecvFds fds-count too many!!");
                break;
            }

            if (*ptr++ == 0)
            {
                if (ptr != &buf[nr-1])
                {
                    CSimplePool::GetInstance()->Free(m_cmptr);
                    return st;
                }
                status = *ptr & 0xFF;
                int newfd = !status ? *(int*)CMSG_DATA(m_cmptr) : -status;

                fds[st] = newfd;

                st += 1;
                nr -= 2;
            }
        }
        CSimplePool::GetInstance()->Free(m_cmptr);
        return st;
    }
NSE

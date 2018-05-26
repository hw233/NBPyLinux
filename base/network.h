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
 * File: network.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_NETWORK_H
#define BASE_NETWORK_H

#include "singleton.h"
#include "event.h"
#include "buffer.h"
#include "imcodepackager.h"
#include <netdb.h>

#define NBPy_NETWORK_ADDRSIZE   255
#define NBPy_IO_RECV_BUFFSIZE 	512
#define NBPy_MASK_PACKET(p) (((p)->header.length & 0xFFFFFF) | (((p)->header.base.verifiymask & 0x0F) << 28) | (((p)->header.base.flag & 0x0F) << 24))

NS(NBPy)

    typedef int PEERID;

    enum enumAdapterType
    {
        eAT_TCP			= 0
        ,eAT_UDP		= 1	//*
        ,eAT_UNIX_TCP	= 2
        ,eAT_UNIX_UDP	= 3	//*
    };

    enum enumPacketFlag
    {
        ePF_NORMAL			= 0
        ,ePF_COMPRESS		= 0x1
        ,ePF_ENCRYPT		= 0x1 << 1
        ,ePF_ENCODE 		= 0x1 << 2
        ,ePF_COMPRESSSUPPORT= 0x1 << 3
        ,ePF_JSON			= 0x1 << 4
    };

    enum enumPeerDataType
    {
        ePEERDTP_PACKET 	= 0,
        ePEERDTP_JSON 		= 1
    };

#pragma pack(1)
    typedef struct _PACKET_BASE_HEADER
    {
        unsigned short verifiymask;
        unsigned short flag;
    } PACKET_BASE_HEADER, *LPPACKET_BASE_HEADER;

    typedef struct _PACKET_FIXED_HEADER
    {
        _PACKET_BASE_HEADER base;
        unsigned int zclength;
        unsigned int length;
        unsigned int crc;
    } PACKET_FIXED_HEADER, *LPPACKET_FIXED_HEADER;

    typedef struct __PACKET
    {
        PACKET_FIXED_HEADER header;
        __PACKET(){ header.base.flag = 0; }
        bool IsValid();
        //void MakeValid(){ header.length_masked = NBPy_MASK_PACKET(this); };
        void *GetData(){return (char *)this + sizeof(__PACKET);};
        void UnPack(IBuffer* p_outbuf);
        void Pack(IBuffer* p_outbuf, char *buf, size_t size, bool compress = false);

        void SetAsIMCode(){
            header.base.flag |= ePF_JSON;
            header.base.flag ^= ePF_JSON;
            header.base.flag |= ePF_ENCODE;
        };
        void SetAsJson(){
            header.base.flag |= ePF_ENCODE;
            header.base.flag ^= ePF_ENCODE;
            header.base.flag |= ePF_JSON;
        };

        bool IsIMCode(){ return NBPy_VALIDATE_MASK(header.base.flag, ePF_ENCODE); };
        bool IsJson(){ return NBPy_VALIDATE_MASK(header.base.flag, ePF_JSON); };
    } PACKET, *LPPACKET;

#pragma pack()

    class CPacketDecoder
    {
    public:
        CPacketDecoder(LPPACKET);
        ~CPacketDecoder();
        IIMCodePackager *GetCodePackager();
    private:
        CDictPackager m_mapdata;
        LPPACKET m_packet;
        bool m_data_inited;
    };

    class IScriptEngine{
    public:
        virtual bool OnPacketIn(int,LPPACKET pck) = 0;
        virtual bool OnConnect(int s, unsigned int sid, const char *szAddr, unsigned short uPort) = 0;
        virtual bool OnClose(int, unsigned int) = 0;
    };

    class IPeer : public IEventProcessor
    {
    public:
        //!!from IEventProcessor
        virtual void OnEventRead(LPEVENT) {};
        virtual void OnEventWrite(LPEVENT) {};
        virtual void OnEventError(LPEVENT) {};
        virtual void OnEvent(LPEVENT) {};

        //!self
        virtual ~IPeer() {
        };

        virtual void Init(int) = 0;

        virtual void Disconnect(bool unlink = false) = 0;
        virtual bool Connect(const char *, unsigned int) = 0;

        virtual unsigned int OnWriteReady(unsigned int ) = 0;
        virtual bool OnReadReady(LPPACKET) = 0;
        virtual void OnError(){};
        virtual bool OnConnect() = 0;

        virtual int GetIOHandle(){ return NBPy_INVALID_SOCKET; };
        virtual PEERID GetId() = 0;

        virtual int Send(char *, size_t) = 0;

        virtual void SetDataType(char c) = 0;

        virtual bool isCompressSupported() = 0;
        virtual void setCompressSupported(bool) = 0;
        virtual void Release(){};

        virtual void Close() = 0;

        virtual void SetScriptEngine(IScriptEngine *engine) = 0;
        virtual void SetSID(unsigned int sid){};
        virtual unsigned int GetSID(){return GetIOHandle();};

    };

    typedef struct _ACCEPT_PEER_INFO {
        int fd;
        int af;
        char r_addr[NBPy_NETWORK_ADDRSIZE];
        unsigned short r_port;
        char l_addr[NBPy_NETWORK_ADDRSIZE];
        unsigned short l_port;
    } ACCEPT_PEER_INFO, *LPACCEPT_PEER_INFO;

    class IService : public IEventProcessor
    {
    public:
        //!from IEventProcessor
        virtual void OnEventRead(LPEVENT) {};
        virtual void OnEventWrite(LPEVENT) {};
        virtual void OnEventError(LPEVENT) {};
        virtual void OnEvent(LPEVENT) {};

        //!self
        virtual ~IService() {};
        virtual bool Create(const char *, const char *, unsigned int, unsigned int) = 0;
        virtual void Close() = 0;

        virtual void OnConnected(LPACCEPT_PEER_INFO , IPeer *&) = 0;
        virtual void OnWriteReady(int) = 0;
        virtual bool OnReadReady(int) = 0;
        virtual void OnError(int) = 0;
        virtual void SetScriptEngine(IScriptEngine *engine) = 0;
    protected:
    private:
    };

    class ISocket
    {
    public:
        virtual ~ISocket() {};
        virtual void Accept(int, IEventProcessor *) = 0;
        virtual int Connect(const char *, unsigned int) = 0;
        virtual int Listen(unsigned int, const char *) = 0;
        virtual void GetSockAddr(int, sockaddr *, sockaddr *) = 0;
        virtual void GetSockAddr(LPEVENT, sockaddr *, sockaddr *) = 0;
        virtual void Close(int, bool unlink = false) = 0;
        virtual int Send(int &, void *, unsigned int) = 0;
        virtual int Recv(int &, void *, unsigned int) = 0;

        virtual void SetAdapter(char adapter) = 0;
        virtual char GetAdapter() = 0;
    protected:
    private:
    };

    class CPeerFactory{
        DECLARE_SINGLETON(CPeerFactory)
    public:
        void Initialize();
        void Destroy();

        IPeer *FindPeer(int sck);
        IPeer *FindPeerBySID(unsigned int sid);

        template <typename T>
        IPeer *Create(int fd){
            T *p = (T::GetAllocator()) ? T::GetAllocator()->allocate() : new T();
            p = new(p) T();
            IPeer *rp = dynamic_cast<IPeer *>(p);
            rp->Init(fd);
            rp->SetSID(getSID(fd));
            RegisterPeer(p);
            return p;
        }

        void Release(IPeer *p){
            UnRegisterPeer(p);
        };

        NBPy_UNORDERED_MAP<int, IPeer *> &GetPeers(){
            return m_peers;
        }

		unsigned int getSID(int fd){
			unsigned int n = m_SIDPREFIX;
			if(m_SIDPREFIX >= 0xFFFF)
			{
				m_SIDPREFIX = 0;
			}
			m_SIDPREFIX ++;
			return (n << 16) | fd;
		}

    private:
        bool RegisterPeer(IPeer *pPeer);
        void UnRegisterPeer(IPeer *pPeer);
    private:
        NBPy_UNORDERED_MAP<int, IPeer *> m_peers;
        NBPy_UNORDERED_MAP<unsigned int, IPeer *> m_sidpeers;
		unsigned int m_SIDPREFIX;
    };

    class CNetworkUtils
    {
        DECLARE_SINGLETON(CNetworkUtils)

    public:
        void Initialize();
        void Destroy();
        unsigned int GetAliveSocketCount();
        int CreateSocket(char adapter, int domain = AF_INET);
        void CloseSocket(int s);
        void SetSocketOpt( char adapter, int s, int linger_flag = 0 );

        int BuildSockAddr(addrinfo *dst, char adapter, const char *szAddr, unsigned int uPort);
        sockaddr *BuildSockAddr( char adapter, const char *szAddr, unsigned int uPort, socklen_t *addrlen );

        void FillAcceptPeerInfo(LPACCEPT_PEER_INFO info);

        void SetPacketMask(unsigned int mask);
        unsigned int GetPacketMask();
        void SetTGWMode(bool b);
        bool IsTGWMode();
    private:
        volatile unsigned int m_nSocketCount;
        unsigned int m_nPacket_Mask;
        bool m_bTGW;
    };

    class CSocket
    {
    public:
        static int Accept(int s, sockaddr *addr, socklen_t &len);
        static int Connect(const char *, unsigned int);
        //static int Listen(char socket_tp, unsigned int, int* fds, const char * = NULL);
        static int Listen(const char *,unsigned int);

        static int ConnectToUnix(char socket_tp, const char *);
        static int ListenAtUnix(char socket_tp, const char *);

        static void GetSockAddr(char socket_tp, int, sockaddr *, sockaddr *);
        static void GetSockAddr(char socket_tp, LPEVENT, sockaddr *, sockaddr *);
        static void Close(char socket_tp, int, bool unlink = false);
        static ssize_t Send(int &, void *, size_t);
        static ssize_t Recv(int &, void *, size_t);

        static ssize_t SendTo(int &, void *, size_t, const char *);
        static ssize_t RecvFrom(int &, void *, size_t, char *);

        static bool SendFd(int, int, const char *);
        static ssize_t RecvFds(int, int *, size_t);

        static size_t RecvTotal;
        static size_t SendTotal;
        static size_t RecvFromTotal;
        static size_t SendToTotal;
    };

NSE

#endif //BASE_NETWORK_H

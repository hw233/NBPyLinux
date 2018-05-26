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
 * File: event.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "event.h"
#include "datetime.h"
#include "network.h"
#include "pool.h"

NS(NBPy)

    LPEVENT CEventFactory::Create() {
        LPEVENT ev = m_Events.allocate();
        return ev;
    }

    void CEventFactory::Release(LPEVENT ev) {
        if(ev)
            m_Events.free(ev);
    }


    LPEventIO CEventDispatcher::GetIO()
    {
        return m_io;
    }

    void CEventDispatcher::Initialize()
    {
        m_io = NULL;
        m_frame_time = EVENT_FRAME_TIME;
        m_running = false;
		m_last_time = CDateTime::timestamp();
    }

    void CEventDispatcher::Destroy()
    {
        Abort();
    }

    bool CEventDispatcher::Init(LPEventIO io, unsigned int frame_time /* = EVENT_FRAME_TIME */)
    {
        if (!io)
            return false;
        m_io = io;
        m_frame_time = frame_time;
        m_running = true;
        CPerformance::GetInstance()->AddCollector(this);
        return true;
    }

    void CEventDispatcher::Abort()
    {
        m_running = false;
        //if (m_io)
        //   m_io->Stop();
    }

    bool CEventDispatcher::Bind(int fd, LPEventProcessor ep)
    {
        if (!m_io)
            return false;
        return m_io->Bind(fd, ep);
    }

    bool CEventDispatcher::UnBind(int fd) {
        if (!m_io)
            return false;
        m_io->UnBind(fd);
        return true;
    }

    void CEventDispatcher::Lookup()
    {
        if(!m_running || !m_io)
        {
            usleep(1 * 1000);
            return;
        }

		m_last_time = CDateTime::timestamp();

        unsigned long t = CDateTime::timestamp();

        unsigned int uev_count = m_io->Lookup(m_frame_time);

		ITimer::CheckAllTimers();

        if( CDateTime::timestamp() - t < 1 && uev_count == 0)
            usleep(1 * 1000);

		CSimplePool::GetInstance()->Lookup();

    }

    void CEventDispatcher::Post(LPEVENT ev)
    {
        if (m_io)
            m_io->Post(ev);
    }

    void CEventDispatcher::Collect(CPerformancePackager &pack){
    	unsigned long l = CDateTime::timestamp() - m_last_time;
		pack.AddForInt("当前事件帧耗时(毫秒)", l, PFM_NUMBER);
    }

#if (TARGET_PLATFORM == PLATFORM_MACOS)

    void CKQueueEventIO::Initialize() {
        //std::lock_guard<std::recursive_mutex> lck(m_mutex);
        m_handleKQueue = 0;
        Start();
    }

    void CKQueueEventIO::Destroy() {
        //std::lock_guard<std::recursive_mutex> lck(m_mutex);
        ::close(m_handleKQueue);
        m_handleKQueue = -1;
        Stop();
    }

    void CKQueueEventIO::Start() {
        m_running = true;
    }

    void CKQueueEventIO::Stop() {
        m_running = false;
        /*
        if(!m_th.joinable())
        {
        }else
        {
            m_th.join();
        }*/

        LPEVENT pEv = NULL;
        while((pEv = m_evExts.pop()))
        {
            CEventFactory::GetInstance()->Release(pEv);
        }
    }

    bool CKQueueEventIO::Init() {
        return Init(0);
    }

    bool CKQueueEventIO::Init(int handleKQueue) {
        //std::lock_guard<std::recursive_mutex> lck(m_mutex);
        m_handleKQueue = !m_handleKQueue ? kqueue() : m_handleKQueue;

        //m_th = std::thread(&CKQueueEventIO::__Thread, this);

        return m_handleKQueue > 0;
    }

    bool CKQueueEventIO::Bind(int fd, LPEventProcessor ep) {
        //std::lock_guard<std::recursive_mutex> lck(m_mutex);
        struct kevent changes[1];
        EV_SET(&changes[0], fd, EVFILT_READ | EVFILT_WRITE, EV_ADD, 0, 0, ep);
        int ret = kevent(m_handleKQueue, changes, 1, NULL, 0, NULL);
        return ret > 0;
    }

    bool CKQueueEventIO::UnBind(int fd) {
        //std::lock_guard<std::recursive_mutex> lck(m_mutex);
        struct kevent changes[1];
        EV_SET(&changes[0], fd, EVFILT_READ | EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
        int ret = kevent(m_handleKQueue, changes, 1, NULL, 0, NULL);
        return ret > 0;
    }

    int CKQueueEventIO::GetIOHandle() {
        return m_handleKQueue;
    }

    void CKQueueEventIO::Post(LPEVENT ev) {
        //std::lock_guard<std::recursive_mutex> lck(m_mutex);
        m_evExts.push(ev);
    }

    unsigned int CKQueueEventIO::Lookup(unsigned int time_out) {
        if(!m_running)
            return 0;

        LPEVENT pEv = NULL;

        //处理附加消息
        while((pEv = m_evExts.pop()) && m_running)
        {
            IEventProcessor *ep = dynamic_cast<IEventProcessor *>((CEventProcessor *)pEv->op);
            if(ep)
                ep->OnEvent(pEv);
            CEventFactory::GetInstance()->Release(pEv);
        }

        //处理断开连接
        while((pEv = m_evWaitCloses.pop()) && m_running)
        {
            IEventProcessor *ep = dynamic_cast<IEventProcessor *>((CEventProcessor *)pEv->op);
            if(ep)
            {
                IPeer *p = dynamic_cast<IPeer *>(ep);
                if(p)
                    p->Close();
            }
            CEventFactory::GetInstance()->Release(pEv);
        }

        struct timespec ts;
        ts.tv_nsec = 1;
        ts.tv_sec = 0;

        memset(m_events, 0, sizeof(struct kevent) * EVENT_POLL_LIMIT);
        int ret = kevent(m_handleKQueue, NULL, 0, m_events, EVENT_POLL_LIMIT, &ts);

        EVENT e;
        for (int i = 0; i < ret; i++) {
            LPEVENT ev = &e;
            memset(ev, 0, sizeof(EVENT));
            IEventProcessor *ep = (IEventProcessor *) m_events[i].udata;
            if (!ep) {
                //DEBUG_WARN("Event %d found, but the Processor not found!", m_events[i].fflags);
                continue;
            }

            int16_t io_evs = m_events[i].filter;

            ev->event_sets = ETP_UNKNOWN;
            ev->fd = (int) m_events[i].ident;
            ev->op = ep;
            ev->data = (size_t)m_events[i].data;

            if (NBPy_VALIDATE_MASK(io_evs, EVFILT_READ)) {
                //DEBUG_STATUS("EVFILT_READ");
                ev->event_sets |= ETP_READ_READY;
                ep->OnEventRead(ev);
            }
            if (NBPy_VALIDATE_MASK(io_evs, EVFILT_WRITE)) {
                //DEBUG_STATUS("EVFILT_WRITE");
                ev->event_sets |= ETP_WRITE_READY;
                ep->OnEventWrite(ev);
            }

            if (NBPy_VALIDATE_MASK(m_events[i].flags, EV_ERROR)
                || NBPy_VALIDATE_MASK(m_events[i].flags, EV_EOF)) {
                //DEBUG_STATUS("EV_EOF | EV_ERROR");
                ev->event_sets = ETP_ERROR;
                ep->OnEventError(ev);
                LPEVENT fv = CEventFactory::GetInstance()->Create();
                fv->fd = ev->fd;
                fv->op = ev->op;
                m_evWaitCloses.push(fv);
            }

            if(ep && ev->event_sets != ETP_UNKNOWN)
                ep->OnEvent(ev);
            //m_evExts.push(ev);
        }
        return 0;
    }

    void CKQueueEventIO::__Thread(){
        DEBUG_STATUS("Network-Thread Begin.");
        while(m_running) {
            /*
            if(!m_mutex.try_lock())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }*/

            struct timespec ts;
            ts.tv_nsec = 1;
            ts.tv_sec = 0;

            memset(m_events, 0, sizeof(struct kevent) * EVENT_POLL_LIMIT);
            int ret = kevent(m_handleKQueue, NULL, 0, m_events, EVENT_POLL_LIMIT, &ts);

            //DEBUG_STATUS("kevent ret: %d", ret);

            //m_mutex.unlock();

            for (int i = 0; i < ret; i++) {
                LPEVENT ev = CEventFactory::GetInstance()->Create();
                IEventProcessor *ep = (IEventProcessor *) m_events[i].udata;
                if (!ep) {
                    DEBUG_WARN("Event %d found, but the Processor not found!", m_events[i].fflags);
                    continue;
                }

                int16_t io_evs = m_events[i].filter;

                ev->event_sets = ETP_UNKNOWN;
                ev->fd = (int) m_events[i].ident;
                ev->op = ep;
                ev->data = (size_t)m_events[i].data;

                if (NBPy_VALIDATE_MASK(io_evs, EVFILT_READ)) {
                    ev->event_sets |= ETP_READ_READY;
                    ep->OnEventRead(ev);
                }
                if (NBPy_VALIDATE_MASK(io_evs, EVFILT_WRITE)) {
                    ev->event_sets |= ETP_WRITE_READY;
                    ep->OnEventWrite(ev);
                }

                if (ETP_UNKNOWN == ev->event_sets
                    || NBPy_VALIDATE_MASK(m_events[i].flags, EV_DELETE)
                    || NBPy_VALIDATE_MASK(m_events[i].flags, EV_ERROR)) {
                    ev->event_sets |= ETP_ERROR;
                    ep->OnEventError(ev);
                }

                std::lock_guard<std::recursive_mutex> lck(m_mutex);
                m_evExts.push(ev);
            }
        }

        DEBUG_STATUS("Network-Thread End.");
        //CPeerFactory::GetInstance()->Disconnect();
    }

#endif

#if (TARGET_PLATFORM == PLATFORM_LINUX)

    void CEPollEventIO::Initialize() {
        //std::lock_guard<std::recursive_mutex> lck(m_mutex);
        m_handleEPoll = 0;
        Start();
    }

    void CEPollEventIO::Destroy() {
        //std::lock_guard<std::recursive_mutex> lck(m_mutex);
        ::close(m_handleEPoll);
        m_handleEPoll = 0;
        Stop();
    }

    void CEPollEventIO::Start() {
        m_running = true;
    }

    void CEPollEventIO::Stop() {
        m_running = false;

        LPEVENT pEv = NULL;
        while((pEv = m_evExts.pop()))
        {
            CEventFactory::GetInstance()->Release(pEv);
        }
    }

    bool CEPollEventIO::Init() {
        return Init(0);
    }

    bool CEPollEventIO::Init(int handleKQueue) {
        m_handleEPoll = !m_handleEPoll ? epoll_create(65535) : m_handleEPoll;
        return m_handleEPoll > 0;
    }

    bool CEPollEventIO::Bind(int fd, LPEventProcessor ep) {
        epoll_event epollev;
        epollev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLERR | EPOLLHUP;
        epollev.data.ptr = ep;
        return epoll_ctl(m_handleEPoll,EPOLL_CTL_ADD, fd, &epollev) == 0;
    }

    bool CEPollEventIO::UnBind(int fd) {
        epoll_event epollev;
        epollev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLERR | EPOLLHUP;
        epollev.data.ptr = NULL;
        bool b = epoll_ctl(m_handleEPoll,EPOLL_CTL_DEL, fd, &epollev) == 0;
        if(b){
			DEBUG_STATUS("Unbind event %d suscess", fd);
        }else{
			DEBUG_STATUS("Unbind event %d fail", fd);
        }
        return b;
    }

    int CEPollEventIO::GetIOHandle() {
        return m_handleEPoll;
    }

    void CEPollEventIO::Post(LPEVENT ev) {
        m_evExts.push(ev);
    }

    unsigned int CEPollEventIO::Lookup(unsigned int time_out) {
        if(!m_running)
            return 0;

        LPEVENT pEv = NULL;

        //处理附加消息
        while((pEv = m_evExts.pop()) && m_running)
        {
            IEventProcessor *ep = dynamic_cast<IEventProcessor *>((CEventProcessor *)pEv->op);
            if(ep)
                ep->OnEvent(pEv);
            CEventFactory::GetInstance()->Release(pEv);
        }

        //处理断开连接
        while((pEv = m_evWaitCloses.pop()) && m_running)
        {
            IEventProcessor *ep = dynamic_cast<IEventProcessor *>((CEventProcessor *)pEv->op);
            if(ep)
            {
                IPeer *p = dynamic_cast<IPeer *>(ep);
                if(p){
                	UnBind(p->GetIOHandle());
                    p->Close();
                    p->Release();
                }
            }
            CEventFactory::GetInstance()->Release(pEv);
        }

        memset(m_events, 0, sizeof(epoll_event) * EVENT_POLL_LIMIT);
        int ret = epoll_wait(
                m_handleEPoll,
                m_events,
                EVENT_POLL_LIMIT,
                0
        );

        EVENT e;
        for (int i = 0; i < ret; i++) {
            LPEVENT ev = &e;
            memset(ev, 0, sizeof(EVENT));
            IEventProcessor *ep = (IEventProcessor *) m_events[i].data.ptr;
            if (!ep) {
                DEBUG_WARN("Event %d found, but the Processor not found!", m_events[i].events);
                //UnBind(m_events[i].data.fd);
                continue;
            }

            uint32_t io_evs = m_events[i].events;

            ep->PreProcessEvent(io_evs);

            ev->event_sets = ETP_UNKNOWN;
            //ev->fd = m_events[i].data.fd;
            ev->op = ep;
            ev->data = 0;

            if (NBPy_VALIDATE_MASK(io_evs, EPOLLIN)) {
                //DEBUG_STATUS("EVFILT_READ");
                ev->event_sets |= ETP_READ_READY;
                ep->OnEventRead(ev);
            }
            if (NBPy_VALIDATE_MASK(io_evs, EPOLLOUT)) {
                //DEBUG_STATUS("EVFILT_WRITE");
                ev->event_sets |= ETP_WRITE_READY;
                ep->OnEventWrite(ev);
            }

            if (ETP_UNKNOWN == ev->event_sets || NBPy_VALIDATE_MASK(io_evs, EPOLLHUP) || NBPy_VALIDATE_MASK(io_evs, EPOLLRDHUP) || NBPy_VALIDATE_MASK(io_evs, EPOLLERR))
            {
                DEBUG_STATUS("EV_EOF | EV_ERROR");
                ev->event_sets = ETP_ERROR;
                ep->OnEventError(ev);
                LPEVENT fv = CEventFactory::GetInstance()->Create();
                fv->fd = ev->fd;
                fv->op = ev->op;
                m_evWaitCloses.push(fv);
            }

            if(ep && ev->event_sets != ETP_UNKNOWN)
                ep->OnEvent(ev);
            //m_evExts.push(ev);
        }
        return ret;
    }

    void CEPollEventIO::__Thread(){
        DEBUG_STATUS("Network-Thread Begin.");
        while(m_running) {
            break;
        }

        DEBUG_STATUS("Network-Thread End.");
        //CPeerFactory::GetInstance()->Disconnect();
    }

#endif

	std::map<int64_t, ITimer *> ITimer::m_Timers;

	void ITimer::AddTimer(ITimer *pt)
	{
		if(!pt)
			return;
		std::map<int64_t, ITimer *>::iterator it = m_Timers.find(pt->GetPtr()->id);
		if(it != m_Timers.end())
			return;
		m_Timers.insert(std::pair<int64_t, ITimer *>(pt->GetPtr()->id, pt));
	}

	void ITimer::DelTimer(ITimer *pt)
	{
		if(!pt)
			return;
		pt->GetPtr()->is_complete = true;
	}

	void ITimer::DelTimer(int64_t id)
	{
		std::map<int64_t, ITimer *>::iterator it = m_Timers.find(id);
		if(it != m_Timers.end())
		{
			ITimer *pt = (ITimer *)it->second;
			ITimer::DelTimer(pt);
		}
	}

	void ITimer::CheckAllTimers()
	{
		std::map<int64_t, ITimer *>::iterator itPrev = m_Timers.end();
		std::map<int64_t, ITimer *>::iterator it = m_Timers.begin();
		while(it != m_Timers.end())
		{
			ITimer *pt = (ITimer *)it->second;
			if(pt->GetPtr()->is_complete)
			{
				itPrev = it;
				it ++;
				m_Timers.erase(itPrev->first);
				pt->OnDestroy();
				continue;
			}
			if(pt->GetPtr()->can_run())
			{
				//pt->GetPtr()->run();
				pt->OnTimer(pt->GetPtr()->last_run_time);
			}
			it ++;
		}
	}
NSE

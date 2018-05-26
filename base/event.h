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
 * File: event.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_EVENT_H
#define BASE_EVENT_H

#include "singleton.h"
#include "allocator.h"
#include "queue.h"
#include "datetime.h"
#include "performance.h"

#define EVENT_FRAME_TIME 30
#define EVENT_POLL_LIMIT 10000

NS(NBPy)

    enum EventTP
    {
        ETP_UNKNOWN		= 1,
        ETP_READ_READY	= ETP_UNKNOWN << 1,
        ETP_WRITE_READY	= ETP_UNKNOWN << 2,
        ETP_NEW			= ETP_UNKNOWN << 3,
        ETP_ERROR		= ETP_UNKNOWN << 4,
    };

#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
/// !!! 切记千万不要设置ETP_NEW与ETP_READ_READY和ETP_WRITE_READY协同
//////////////////////////////////////////////////////////////////////////
    typedef struct __EVENT
    {
        int event_sets;
        void *op;
        int fd;
        size_t data;
        //void *next;
    } EVENT, *LPEVENT;

#pragma pack()

    class IEventProcessor
    {
    public:
        virtual ~IEventProcessor() {};
        virtual void OnEventRead(LPEVENT) {};
        virtual void OnEventWrite(LPEVENT) {};
        virtual void OnEventError(LPEVENT) {};
        virtual void OnEvent(LPEVENT) {};
        virtual void PreProcessEvent(uint32_t) {};
    protected:
    private:
    };

    typedef IEventProcessor *LPEventProcessor;

    class CEventProcessor: public IEventProcessor{
    public:
        virtual ~CEventProcessor() {};
        virtual void OnEvent(LPEVENT, int) {};
    protected:
    private:
    };

    class IEventIO
    {
    public:
        virtual ~IEventIO() {};
        //////////////////////////////////////////////////////////////////////////
        /// Query io_events
        //////////////////////////////////////////////////////////////////////////
        virtual unsigned int Lookup(unsigned int) = 0;
        //////////////////////////////////////////////////////////////////////////
        /// Bind fd to IO
        //////////////////////////////////////////////////////////////////////////
        virtual bool Bind(int, LPEventProcessor) = 0;
        virtual bool UnBind(int) = 0;
        //////////////////////////////////////////////////////////////////////////
        /// Post a event to the IO-Handle(maybe not be support)
        //////////////////////////////////////////////////////////////////////////
        virtual void Post(LPEVENT) = 0;
        /// Return io handle
        virtual int GetIOHandle() = 0;
        //////////////////////////////////////////////////////////////////////////
        /// Set the IO-Handle
        //////////////////////////////////////////////////////////////////////////
        virtual bool Init(int) = 0;
        virtual bool Init() = 0;

        virtual void Stop() = 0;
        virtual void Start() = 0;
    protected:
    private:
    };
    typedef IEventIO *LPEventIO;

    typedef std::map<int, EVENT> EVENTS;
    typedef std::pair<int, EVENT> EVENTPAIR;

    class CEventFactory{
        DECLARE_SINGLETON(CEventFactory)
    public:
        void Initialize(){};
        void Destroy(){ m_Events.clear(); };
        LPEVENT Create();
        void Release(LPEVENT ev);
    private:
        CThreadAllocator<EVENT> m_Events;
    };

    class CEventDispatcher : public IPerformanceCollector
    {
        DECLARE_SINGLETON(CEventDispatcher)

    public:
        void Initialize();
        void Destroy();

        bool Init(LPEventIO io, unsigned int frame_time = EVENT_FRAME_TIME);
        void Lookup();
        //////////////////////////////////////////////////////////////////////////
        /// Bind fd to IO
        //////////////////////////////////////////////////////////////////////////
        bool Bind(int fd, LPEventProcessor ep);
        bool UnBind(int fd);
        void Abort();
        void Post(LPEVENT ev);
        /// Return io operator
        LPEventIO GetIO();

        void Collect(CPerformancePackager &pack);
    protected:
    private:
        LPEventIO m_io;
        unsigned int m_frame_time;
        bool m_running;
        unsigned int m_last_time;
        unsigned int m_once_time;
    };

#if (TARGET_PLATFORM == PLATFORM_LINUX)

    class CEPollEventIO : public IEventIO
    {
        DECLARE_SINGLETON(CEPollEventIO)
    public:
        void Initialize();
        void Destroy();
        //////////////////////////////////////////////////////////////////////////
        /// Query io_events
        //////////////////////////////////////////////////////////////////////////
        virtual unsigned int Lookup(unsigned int time_out = NBPy_INFINITE);
        //////////////////////////////////////////////////////////////////////////
        /// Bind fd to IO
        //////////////////////////////////////////////////////////////////////////
        virtual bool Bind(int fd, LPEventProcessor ep);
        virtual bool UnBind(int fd);
        //////////////////////////////////////////////////////////////////////////
        /// Set the IO-Handle
        //////////////////////////////////////////////////////////////////////////
        virtual bool Init(int handleEPoll);
        virtual bool Init();
        //////////////////////////////////////////////////////////////////////////
        /// Post event to the IO-Handle
        //////////////////////////////////////////////////////////////////////////
        void Post(LPEVENT ev);
        /// Return io handle
        int GetIOHandle();

        void __Thread();

        void Stop();
        void Start();

    private:
        int m_handleEPoll;
        CShareQueue<LPEVENT> m_evExts;
        CShareQueue<LPEVENT> m_evWaitCloses;
        struct epoll_event m_events[EVENT_POLL_LIMIT];
        bool m_running;
    };

#endif

#if (TARGET_PLATFORM == PLATFORM_MACOS)

    class CKQueueEventIO : public IEventIO
    {
        DECLARE_SINGLETON(CKQueueEventIO)
    public:
        void Initialize();
        void Destroy();
        //////////////////////////////////////////////////////////////////////////
        /// Query io_events
        //////////////////////////////////////////////////////////////////////////
        virtual unsigned int Lookup(unsigned int time_out = NBPy_INFINITE);
        //////////////////////////////////////////////////////////////////////////
        /// Bind fd to IO
        //////////////////////////////////////////////////////////////////////////
        virtual bool Bind(int fd, LPEventProcessor ep);
        virtual bool UnBind(int fd);
        //////////////////////////////////////////////////////////////////////////
        /// Set the IO-Handle
        //////////////////////////////////////////////////////////////////////////
        virtual bool Init(int handleKQueue);
        virtual bool Init();
        //////////////////////////////////////////////////////////////////////////
        /// Post event to the IO-Handle
        //////////////////////////////////////////////////////////////////////////
        void Post(LPEVENT ev);
        /// Return io handle
        int GetIOHandle();

        void __Thread();

        void Stop();
        void Start();

    private:
        int m_handleKQueue;
        CShareQueue<LPEVENT> m_evExts;
        CShareQueue<LPEVENT> m_evWaitCloses;
        struct kevent m_events[EVENT_POLL_LIMIT];
        std::recursive_mutex m_mutex;
        bool m_running;
        std::thread m_th;
    };

#endif


    enum __enumTimerType
    {
        eTimerTP_NONE = 0,
        eTimerTP_LOOP = 1,
        eTimerTP_ONCE = 2
    };

    typedef struct __timer
    {
        int64_t id;
        char tp;
        unsigned long interval;
        unsigned long last_run_time;
        bool is_complete;
        __timer()
        {
            memset(this, 0, sizeof(__timer));
            last_run_time = CDateTime::timestamp();
            is_complete = false;
            interval = 1;
        }

        void run()
        {
            last_run_time = CDateTime::timestamp();
            if(tp == eTimerTP_ONCE)
                is_complete = true;
        }

        bool can_run()
        {
        	if(tp == eTimerTP_NONE)
				return false;
        	unsigned long et = CDateTime::timestamp();
        	unsigned long st = et - last_run_time;
            bool b = st >= interval;
            if(!b)
				return false;

            if(tp == eTimerTP_ONCE){
                is_complete = true;
            }else
            {
				unsigned long ct = st - interval;
				last_run_time = et - ct;
            }
            return true;
        }

    }timer, *timer_ptr;

    class ITimer
    {
    public:
        virtual timer_ptr GetPtr(){ return &__timer; };
        virtual void OnTimer(unsigned long t){  };
        virtual void OnDestroy(){};
        static void CheckAllTimers();
        static void AddTimer(ITimer *);
        static void DelTimer(ITimer *);
        static void DelTimer(int64_t id);
    protected:
        static std::map<int64_t, ITimer *> m_Timers;
        timer __timer;
    };
NSE

#endif //BASE_EVENT_H

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
 * File: stackless.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_STACKLESS_H
#define BASE_STACKLESS_H

#include "macro_py.h"
#include "network.h"
#include "datatunnel.h"
#include "peer.h"
#include "performance.h"

#include <stackless_api.h>
#include <pyport.h>
#include <list>

NS(NBPy)

    enum __enumCallbackType
    {
        eCBTP_NONE 		= 0,
        eCBTP_CONNECT 	= 1,
        eCBTP_CLOSE 	= 2,
        eCBTP_PACKET 	= 3,
        eCBTP_TERMINATE	= 4
    };

    class CStacklessEngine: public IScriptEngine, public IPerformanceCollector{
        DECLARE_SINGLETON(CStacklessEngine)
    public:
        void Initialize();
        void Destroy();

        bool OnPacketIn(int,LPPACKET pck);
        bool OnConnect(int s, unsigned int sid, const char *szAddr, unsigned short uPort);
        bool OnClose(int, unsigned int);
        bool OnTerminate();

        void SetOutput(IOutput *);
        int Run(const char *, ...);
        PyObject *LoadModule(const char *);

        bool BindModule(const char *pszPath, const char *pszModule, bool raw_flag);

        void SetCallback(int, PyObject *);
        void Schedule();


        static PyObject * _SetCallback(PyObject *pyObj, PyObject *pyArgs);

        static PyObject *BeNice(PyObject *pyObj, PyObject *pyArg);
        static void ProcessBeNice();
        static PyChannelObject* pNiceChannel;

        static PyObject *CalcRunCount(PyObject *pyObj, PyObject *pyArg);
        static unsigned long gRunCount;

        void Collect(CPerformancePackager &pack);
        bool InitModule();

	public:
		int push_all_packets();
    private:

		PyObject *m_pyPacketList;

        PyObject *m_pyOnPacketFunc;
        PyObject *m_pyOnConnectFunc;
        PyObject *m_pyOnCloseFunc;
        PyObject *m_pyOnTerminateFunc;

        PyTaskletObject *m_pyOnPacketFuncTask;
        PyTaskletObject *m_pyOnHttpFuncTask;
        PyTaskletObject *m_pyOnConnectFuncTask;
        PyTaskletObject *m_pyOnCloseFuncTask;

        std::list<PyTaskletObject *> m_paused_tasklets;
    };

	class CPyEventProcessor : public IEventProcessor
	{
	public:
		void init(int fd, PyObject *pyFunc, unsigned long Id)
		{
			m_pyFunc = pyFunc;
			Py_XINCREF(m_pyFunc);
			m_id = Id;
			m_fd = fd;
		}

		~CPyEventProcessor()
		{
			Py_XDECREF(m_pyFunc);
		}

		virtual void PreProcessEvent(uint32_t);
	private:
		PyObject *m_pyFunc;
		unsigned long m_id;
		int m_fd;
	};

	class CPyEventOP
	{
		DECLARE_SINGLETON(CPyEventOP)
	public:
		void Initialize();
		void Destroy();

		CPyEventProcessor *Create_Processor( int fd );
		void Destroy_Processor( int fd );
	private:
		CObjectAllocator<CPyEventProcessor> m_processors;
		NBPy_UNORDERED_MAP<int, CPyEventProcessor *> m_processor_alloces;
	};

	class CPyTimer: public ITimer
	{
	public:
		CPyTimer():m_pyOnTimerFunc(NULL),m_pyOnTimerParam(NULL){ __timer.id = reinterpret_cast<int64_t>(this); };
		~CPyTimer(){
			Py_XDECREF(m_pyOnTimerFunc);
			Py_XDECREF(m_pyOnTimerParam);
		}
		virtual void OnTimer(unsigned long);
		void SetOnTimer(unsigned long, char, PyObject *, PyObject *);
		void OnDestroy();
	private:
		PyObject *m_pyOnTimerFunc;
		PyObject *m_pyOnTimerParam;
	public:
		static CObjectAllocator<CPyTimer> g_timer_allocator;
	};
NSE

#endif //BASE_STACKLESS_H

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
 * File: stackless.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "stacklessengine.h"
#include "protocol.h"

NS(NBPy)

    PyChannelObject* CStacklessEngine::pNiceChannel = NULL;
    unsigned long CStacklessEngine::gRunCount = 0;

    void CStacklessEngine::Initialize() {
        m_pyOnPacketFunc        = NULL;
        m_pyOnConnectFunc       = NULL;
        m_pyOnCloseFunc         = NULL;
        m_pyOnTerminateFunc		= NULL;
        m_pyOnPacketFuncTask    = NULL;
        m_pyOnConnectFuncTask   = NULL;
        m_pyOnCloseFuncTask     = NULL;
        m_pyPacketList			= NULL;

        Py_Initialize();
        if(!Py_IsInitialized()){
            return;
        }
		gRunCount = 0;

        CPerformance::GetInstance()->AddCollector(this);
        Run("import sys;reload(sys);sys.setdefaultencoding('utf-8')");

        if(!pNiceChannel)
            pNiceChannel = PyChannel_New(0);

        InitModule();
		m_pyPacketList = PyList_New(0);
    }

    void CStacklessEngine::Destroy() {
		OnTerminate();
    }

    void CStacklessEngine::Collect(CPerformancePackager &pack) {
		pack.AddForInt("当前存活微进程数量", gRunCount, PFM_NUMBER);
    }

    PyObject *CStacklessEngine::BeNice(PyObject *pyObj, PyObject *pyArg)
    {
        assert( pNiceChannel && "NiceChannel is NULL!!!!" );
        /* By using PyChannel_Receive_nr to block the current tasklet on
           the channel, we ensure that soft switching is used (we can
           check this by looking at what is returned and making sure it
           is the unwind token. */
        PyObject * ret = PyChannel_Receive_nr(pNiceChannel);
        //assert(ret == (PyObject *)Py_UnwindToken);
        return ret;
    }

    PyObject *CStacklessEngine::CalcRunCount(PyObject *pyObj, PyObject *pyArgs)
    {
        if( !pyArgs )
            Py_RETURN_FALSE;

        unsigned long tp = 0;
        if( !PyArg_ParseTuple(pyArgs, "L", &tp) )
            Py_RETURN_FALSE;

		CStacklessEngine::gRunCount = tp;

        Py_RETURN_TRUE;
    }

    PyObject *CStacklessEngine::_SetCallback(PyObject *pyObj, PyObject *pyArgs)
    {
        if( !pyArgs )
            Py_RETURN_FALSE;

        PyObject *pObj;
        int tp = 0;
        if( !PyArg_ParseTuple(pyArgs, "iO", &tp, &pObj) )
            Py_RETURN_FALSE;

		if(!PyCallable_Check(pObj))
			Py_RETURN_FALSE;

		CStacklessEngine::GetInstance()->SetCallback(tp, pObj);

		Py_RETURN_TRUE;
    }

    void CStacklessEngine::ProcessBeNice()
    {
        for (int i = pNiceChannel->balance; i < 0; i++)
        {
            PyChannel_Send(pNiceChannel, Py_None);
            if (PyErr_Occurred() != NULL) {
                PyErr_Print();
                PyErr_Clear();
            }
        }
    }

    int CStacklessEngine::Run(const char *f, ...)
    {
        if(!Py_IsInitialized())
            return -1;
        char szTmp[1024];
        va_list vl;
        va_start(vl, f);
        vsnprintf(szTmp, 1024 - 1, f, vl);
        va_end(vl);
        int r = PyRun_SimpleString(szTmp);
        if(PyErr_Occurred())
        {
            PyErr_Print();
            PyErr_Clear();
            return -1;
        }
        return r;
    }

    void CStacklessEngine::SetOutput(IOutput *pOutput)
    {
        CPyOutput *pyOutput = NULL;
        pyOutput = PyObject_New(CPyOutput, CPyClassBase::GetType<CPyOutput>());
        if(!pyOutput)
        {
            DEBUG_WARN(">>> SetOutput: create new CPyOutput fail!");
            return;
        }

        pyOutput = new(pyOutput) CPyOutput(this);
        pyOutput->SetOutput(pOutput);
        Py_XINCREF(pyOutput);
        PySys_SetObject((char *)"stdout",(PyObject *)pyOutput);
        PySys_SetObject((char *)"stderr",(PyObject *)pyOutput);

        DEBUG_STATUS(">>> Python stdout and stderr is redirected.");
    }


    bool CStacklessEngine::BindModule(const char *pszPath, const char *pszModule, bool raw_flag)
    {
        if(!Py_IsInitialized())
        {
            assert("Python is not Initialized!" && false);
            return false;
        }
        PyObject *pModule = LoadModule("sys");
        PyObject *PDict = PyModule_GetDict(pModule);
        PyObject *pFunc = PyDict_GetItemString(PDict, "version");
        char *szVersion = PyString_AsString(pFunc);
        DEBUG_STATUS("Run with %s", szVersion);
        DEBUG_STATUS("Script Path: %s", pszPath);
        Run("[sys.path.insert(0,i) for i in '%s'.split('|')]", pszPath);
        if(raw_flag)
        {
            Run("import %s", pszModule);
        }
        else
        {
            PyObject *pyMod = LoadModule(pszModule);
            if(!pyMod)
            {
                if(PyErr_Occurred())
                {
                    PyErr_Print();
                    PyErr_Clear();
                }
                return false;
            }
        }
        return true;
    }

    PyObject *CStacklessEngine::LoadModule(const char *pszModuleName)
    {
        if(!Py_IsInitialized())
            return NULL;
        PyObject *pName = Py_BuildValue("s", pszModuleName);
        PyObject *pModule = PyImport_Import(pName);
        Py_XDECREF(pName);
        if(PyErr_Occurred())
        {
            PyErr_Print();
            PyErr_Clear();
        }
        return pModule;
    }

    void CStacklessEngine::SetCallback(int tp, PyObject *pFunc)
    {
		//DEBUG_STATUS(">>> Set callback >> %d", tp);
        if(!PyCallable_Check(pFunc))
            return;

		//DEBUG_STATUS(">>> Set callback >> %d -> %p", tp, pFunc);
        switch(tp)
        {
            case eCBTP_CONNECT:
            {
                DEBUG_STATUS(">>> Set callback >> eCBTP_CONNECT");
                Py_XDECREF(m_pyOnConnectFunc);
                m_pyOnConnectFunc = pFunc;
                Py_XINCREF(m_pyOnConnectFunc);
                break;
            }
            case eCBTP_CLOSE:
            {
                DEBUG_STATUS(">>> Set callback >> eCBTP_CLOSE");
                Py_XDECREF(m_pyOnCloseFunc);
                m_pyOnCloseFunc = pFunc;
                Py_XINCREF(m_pyOnCloseFunc);
                break;
            }
            case eCBTP_PACKET:
            {
                DEBUG_STATUS(">>> Set callback >> eCBTP_PACKET");
                Py_XDECREF(m_pyOnPacketFunc);
                m_pyOnPacketFunc = pFunc;
                Py_XINCREF(m_pyOnPacketFunc);
                break;
            }
            case eCBTP_TERMINATE:
            {
                DEBUG_STATUS(">>> Set callback >> eCBTP_TERMINATE");
                Py_XDECREF(m_pyOnTerminateFunc);
                m_pyOnTerminateFunc = pFunc;
                Py_XINCREF(m_pyOnTerminateFunc);
                break;
            }break;
            default:{}
        }
    }

    bool CStacklessEngine::OnPacketIn(int s, LPPACKET pPacket)
    {
        if(!Py_IsInitialized())
            return false;
        if(!m_pyOnConnectFunc || !PyCallable_Check(m_pyOnConnectFunc))
			return false;

        PyObject *pyPacket = PyDict_New();

        CPacketDecoder decoder(pPacket);
        CBuffer buf;

        if(!CPythonCodec::GetInstance()->GetDecoder()->Decode(decoder.GetCodePackager(), pyPacket))
        {
            Py_XDECREF(pyPacket);
            return false;
        }

        if(PyDict_Size(pyPacket) == 0)
        {
            Py_XDECREF(pyPacket);
            DEBUG_WARN("CStacklessEngine::OnPacketIn >> packet is empty!");
            return false;
        }

/*
        if(m_pyOnPacketFunc && PyCallable_Check(m_pyOnPacketFunc))
        {
            m_pyOnPacketFuncTask = PyTasklet_New(NULL, m_pyOnPacketFunc);
            PyObject *pyFd = Py_BuildValue("i", s);
            PyObject *pyParams = PyTuple_Pack(2, pyFd, pyPacket);
            Py_XDECREF(pyFd);
            PyTasklet_Setup(m_pyOnPacketFuncTask, pyParams, NULL);
            Py_XDECREF(pyParams);
            Py_XDECREF(m_pyOnPacketFuncTask);
            if(PyErr_Occurred())
            {
                PyErr_Print();
                PyErr_Clear();
            }
        }else
        {
            Py_XDECREF(pyPacket);
            return false;
        }

*/		if(m_pyPacketList){
			Py_ssize_t now = PyList_GET_SIZE(m_pyPacketList);
			if( now >= 2048 ){
				//push packets
			}
		}

		if(!m_pyPacketList){
			m_pyPacketList = PyList_New(0);
		}

		PyList_Append(m_pyPacketList, pyPacket);

        Py_XDECREF(pyPacket);
        return true;
    }


	int CStacklessEngine::push_all_packets(){

		if( m_pyPacketList )
		{
			Py_ssize_t now = PyList_GET_SIZE(m_pyPacketList);
			if( now > 0 )
			{
				//Generate the tasklet.
				PyTaskletObject *pTask = PyTasklet_New(NULL,m_pyOnPacketFunc);
				PyObject* pTuple = PyList_AsTuple(m_pyPacketList);
				PyTasklet_Setup(pTask,pTuple,NULL);

				Py_DECREF(m_pyPacketList);
				Py_DECREF(pTuple);
				Py_DECREF(pTask);
				m_pyPacketList = 0;
			}
		}
		return 0;
	}

    bool CStacklessEngine::OnConnect(int s, unsigned int sid, const char *szAddr, unsigned short uPort)
    {
        if(!Py_IsInitialized())
            return false;
        if(m_pyOnConnectFunc && PyCallable_Check(m_pyOnConnectFunc))
        {
            m_pyOnConnectFuncTask = PyTasklet_New(NULL, m_pyOnConnectFunc);
            PyObject *pyFd = Py_BuildValue("i", s);
            PyObject *pySID = Py_BuildValue("l", sid);
            PyObject *pyAddr = Py_BuildValue("s", szAddr);
            PyObject *pyPort = Py_BuildValue("i", uPort);
            PyObject *pyParams = PyTuple_Pack(4, pyFd, pySID, pyAddr, pyPort);
            Py_XDECREF(pyFd);
            Py_XDECREF(pySID);
            Py_XDECREF(pyAddr);
            Py_XDECREF(pyPort);
            PyTasklet_Setup(m_pyOnConnectFuncTask, pyParams, NULL);
            Py_XDECREF(pyParams);
            Py_XDECREF(m_pyOnConnectFuncTask);
            if(PyErr_Occurred())
            {
                PyErr_Print();
                PyErr_Clear();
            }
        }else
            return false;
        return true;
    }

    bool CStacklessEngine::OnClose(int s, unsigned int sid)
    {
        if(!Py_IsInitialized())
            return false;
        if(m_pyOnCloseFunc && PyCallable_Check(m_pyOnCloseFunc))
        {
            m_pyOnCloseFuncTask = PyTasklet_New(NULL, m_pyOnCloseFunc);
            PyObject *pyFd = Py_BuildValue("i", s);
            PyObject *pySID = Py_BuildValue("l", sid);
            PyObject *pyParams = PyTuple_Pack(2, pyFd, pySID);
            Py_XDECREF(pyFd);
            Py_XDECREF(pySID);
            PyTasklet_Setup(m_pyOnCloseFuncTask, pyParams, NULL);
            Py_XDECREF(pyParams);
            Py_XDECREF(m_pyOnCloseFuncTask);
            if(PyErr_Occurred())
            {
                PyErr_Print();
                PyErr_Clear();
            }
        } else
            return false;
        return true;
    }

    bool CStacklessEngine::OnTerminate()
    {
        if(!Py_IsInitialized())
            return false;

		bool b = false;
        if(m_pyOnTerminateFunc && PyCallable_Check(m_pyOnTerminateFunc))
        {
            PyTaskletObject *pTask = PyTasklet_New(NULL, m_pyOnTerminateFunc);
            PyObject *pyParams = PyTuple_New(0);
            PyTasklet_Setup(pTask, pyParams, NULL);
            PyTasklet_Run(pTask);
            Py_XDECREF(pyParams);
            Py_XDECREF(pTask);
            if(PyErr_Occurred())
            {
                PyErr_Print();
                PyErr_Clear();
            }
            b = true;
        }

        Py_XDECREF(m_pyOnConnectFunc);
        Py_XDECREF(m_pyOnCloseFunc);
        Py_XDECREF(m_pyOnPacketFunc);
        Py_XDECREF(m_pyOnTerminateFunc);
        Py_XDECREF(pNiceChannel);

        Py_Finalize();

        return b;
    }

    void CPyClassBase::__del__(__CPyClassBase *self)
    {
        self->ob_type->tp_free((PyObject *)self);
    }

    LPCPyClassBase CPyModule::m_pClassBase = NULL;

    CPyModule::CPyModule(const char *module_name, const char *module_doc, IScriptEngine *pMachine)
    {
        if(!CPyModule::m_pClassBase)
        {
            PyTypeObject *pType = CPyClassBase::GetTypeObject<CPyClassBase>();
            if( (pType->tp_flags & Py_TPFLAGS_READY) != Py_TPFLAGS_READY)
            {
                PyType_Ready(pType);
            }
            Py_XINCREF(pType);
            CPyModule::m_pClassBase = PyObject_NEW(CPyClassBase, pType);
            Py_XINCREF(CPyModule::m_pClassBase);
        }
        CPyModule::m_pClassBase->pMachine = pMachine;
        m_name = module_name;
        m_doc = module_doc;
        m_pMethods_ref = NULL;
    };

    CPyModule::~CPyModule()
    {
    }

    void CPyModule::RegisterCallable(const char *name, PyCFunction pyFunc, int flag, const char *doc)
    {
        PyMethodDef pd;
        pd.ml_name = name;
        pd.ml_meth = pyFunc;
        pd.ml_doc = doc;
        pd.ml_flags = flag;
        m_methods.push_back(pd);
    }

    static PyMethodDef G_NONEDEF = {NULL,NULL,0,NULL};
    void CPyModule::Embed(const char *parent, const char *self_name)
    {
        unsigned long nmsize = m_methods.size();
        m_pMethods_ref = NULL;
        if(nmsize > 0)
        {
            PyMethodDef *pMethods = new PyMethodDef[nmsize + 1];
            m_pMethods_ref = pMethods;
            unsigned int n = 0;
            std::vector<PyMethodDef>::iterator it = m_methods.begin();
            while(it != m_methods.end() && n < nmsize)
            {
                memcpy(pMethods, &(*it), sizeof(PyMethodDef));
                n ++;
                it ++;
                pMethods ++;
            }
            memcpy(pMethods, &G_NONEDEF, sizeof(PyMethodDef));
        }

        PyObject* pObjMod = Py_InitModule4(m_name, m_pMethods_ref, m_doc, (PyObject *)CPyModule::m_pClassBase, PYTHON_API_VERSION);
        if( !pObjMod )
            return;

		if(parent){
			PyObject *parentMod = PyImport_ImportModule(parent);
			if(parentMod){
				PyModule_AddObject(parentMod, self_name, pObjMod);
				Py_XINCREF(pObjMod);
			}
		}

        std::map<const char *, int>::iterator itV = m_intValues.begin();
        while(itV != m_intValues.end())
        {
            PyModule_AddIntConstant(pObjMod,itV->first,itV->second);
            itV ++;
        }
        std::map<const char *, PyTypeObject *>::iterator itCls = m_cls.begin();
        while(itCls != m_cls.end())
        {
            PyModule_AddObject(pObjMod,itCls->first,(PyObject *)itCls->second);
            itCls ++;
        }
    }

    void CStacklessEngine::Schedule()
    {
        while(m_paused_tasklets.size() > 0)
        {
            PyTaskletObject *o = m_paused_tasklets.front();
            PyTasklet_Insert(o);
            Py_XDECREF(o);
            m_paused_tasklets.pop_front();
        }

        ProcessBeNice();

        do {
            PyObject *oRun = PyStackless_RunWatchdog(100);
            if(oRun && oRun != Py_None)
            {
                PyTaskletObject *taskORun = (PyTaskletObject *)oRun;
                if(!PyTasklet_Alive(taskORun))
                {
                    if(taskORun->cstate)
                    {
                        PyTasklet_Kill(taskORun);
                    }else
                    {
                        PyTasklet_Remove(taskORun);
                    }
                    Py_XDECREF(taskORun);
                }else
                {
                    if(!PyTasklet_Scheduled(taskORun))
                    {
                        PyTasklet_Remove(taskORun);
                        Py_XINCREF(taskORun);
                        m_paused_tasklets.push_back(taskORun);
                    }
                }
            }
            if(PyErr_Occurred())
            {
                PyErr_Print();
                PyErr_Clear();
            }

        } while(PyStackless_GetRunCount() > 1);

        timespec slp_ts,slp_tr;
        if( m_paused_tasklets.size() < 1 )
        {
            slp_tr.tv_sec = slp_tr.tv_nsec = 0;
            //
            slp_ts.tv_sec  = 0;
            slp_ts.tv_nsec = 100 * 1000;
            nanosleep(&slp_ts,&slp_tr);
        }
    }

    PYCLASS_INIT(CPyClassBase, "ClassBase");

    PYCLASS_INIT_F(CPyOutput, "Output", "MLSE Output Object");

    PYCLASS_BEGIN_METHOD_DEF(CPyOutput)
                    PYCLASS_ADD_METHOD(CPyOutput,write)
    PYCLASS_END_METHOD_DEF(CPyOutput)

    PYCLASS_BEGIN_GETSET_DEF(CPyOutput)
    PYCLASS_END_GETSET_DEF(CPyOutput)

    PYCLASS_IMP_INTERFACE(CPyOutput)

    PYCLASS_METHOD_IMPL(CPyOutput, write)
    {
        if( !pyArgs )
            Py_RETURN_NONE;
        if(!pySelf->pMachine)
            Py_RETURN_NONE;

        IOutput *pOutput = pySelf->GetOutput();
        if(!pOutput)
            Py_RETURN_NONE;

        char*        szBuf = 0;
        Py_ssize_t   len = 0;
        if( !PyArg_ParseTuple(pyArgs,"s#",&szBuf,&len) )
        {
            Py_RETURN_NONE;
        }

        if( !szBuf )
            Py_RETURN_NONE;

        if( len == 0 )
            Py_RETURN_NONE;
        if( len == 1 && szBuf[0] == '\n' )
            Py_RETURN_NONE;

        time_t t = time(NULL);
        tm *tf = localtime(&t);
        static char szTime[32];
        sprintf(szTime, "[%.4d/%.2d/%.2d %.2d:%.2d:%.2d] ", tf->tm_year + 1900, tf->tm_mon + 1, tf->tm_mday, tf->tm_hour, tf->tm_min, tf->tm_sec);

        pOutput->write(szTime,strlen(szTime));
        pOutput->write(szBuf,len);
        pOutput->write("\n",1);
        Py_RETURN_NONE;
    }


	void CPyEventOP::Initialize()
	{
		return;
	}

	void CPyEventOP::Destroy()
	{
		return;
	}

	CPyEventProcessor *CPyEventOP::Create_Processor( int fd )
	{
		CPyEventProcessor *pp = m_processors.allocate();
		m_processor_alloces[fd] = pp;
		return pp;
	}

	void CPyEventOP::Destroy_Processor( int fd )
	{
		NBPy_UNORDERED_MAP<int, CPyEventProcessor *>::iterator it = m_processor_alloces.find(fd);
		if(it != m_processor_alloces.end())
		{
			m_processors.free(it->second);
			m_processor_alloces.erase(it->first);
		}
	}

	PyObject *EventOP_Bind(PyObject *pySelf, PyObject *pyArgs)
	{
		if( !pyArgs )
			Py_RETURN_FALSE;
		PyObject    *pyFD, *pyEvent, *pyFunc, *pyId;
		if( !PyArg_UnpackTuple(pyArgs, "ref", 4, 4, &pyFD, &pyEvent, &pyFunc, &pyId) )
			Py_RETURN_FALSE;

		int fd 		= PyLong_AsLong(pyFD);
		int events 	= PyLong_AsLong(pyEvent);
		unsigned long id = PyLong_AsUnsignedLong(pyId);

#if (TARGET_PLATFORM == PLATFORM_LINUX)
		CPyEventProcessor *pp = CPyEventOP::GetInstance()->Create_Processor(fd);
		pp->init(fd, pyFunc, id);

		epoll_event epollev;
		epollev.events = events | EPOLLET;
		epollev.data.ptr = pp;
		if(epoll_ctl(CEPollEventIO::GetInstance()->GetIOHandle(),EPOLL_CTL_ADD, fd, &epollev) != 0 && errno != EEXIST)
		{
			DEBUG_ERROR("epoll_ctl fail! fd: %d, errno: %d", fd, errno);
			CPyEventOP::GetInstance()->Destroy_Processor(fd);
			Py_RETURN_FALSE;
		}else if (errno == EEXIST){
		}else{
			DEBUG_STATUS("EventOP_Bind suscess! fd: %d", fd);
		}
#endif
		Py_RETURN_TRUE;
	}

	PyObject *EventOP_UnBind(PyObject *pySelf, PyObject *pyArgs)
	{
		if( !pyArgs )
			Py_RETURN_FALSE;
		PyObject    *pyFD;
		if( !PyArg_UnpackTuple(pyArgs, "ref", 1, 1, &pyFD) )
			Py_RETURN_FALSE;

		int fd 		= PyLong_AsLong(pyFD);

#if (TARGET_PLATFORM == PLATFORM_LINUX)
		epoll_event epollev;
		epollev.events = EPOLLIN | EPOLLOUT | EPOLLET;
		epollev.data.ptr = NULL;

		epoll_ctl(CEPollEventIO::GetInstance()->GetIOHandle(),EPOLL_CTL_DEL, fd, &epollev);

		DEBUG_STATUS("EventOP_UnBind suscess! fd: %d", fd);
		CPyEventOP::GetInstance()->Destroy_Processor(fd);
#endif

		Py_RETURN_TRUE;
	}

	void CPyEventProcessor::PreProcessEvent(uint32_t events)
	{
		if(!m_pyFunc)
			return;

		PyTaskletObject *pyOnPacketFuncTask = PyTasklet_New(NULL, m_pyFunc);
		PyObject *pyFd = Py_BuildValue("i", m_fd);
		PyObject *pyEvent = Py_BuildValue("i", events);
		PyObject *pyId = Py_BuildValue("l", m_id);
		PyObject *pyParams = PyTuple_Pack(3, pyFd, pyEvent, pyId);
		PyTasklet_Setup(pyOnPacketFuncTask, pyParams, NULL);
		Py_XDECREF(pyFd);
		Py_XDECREF(pyEvent);
		Py_XDECREF(pyId);
		Py_XDECREF(pyParams);
		Py_XDECREF(pyOnPacketFuncTask);
		if(PyErr_Occurred())
		{
			PyErr_Print();
			PyErr_Clear();
		}
	}

	CObjectAllocator<CPyTimer> CPyTimer::g_timer_allocator;

	void CPyTimer::OnTimer(unsigned long t)
	{
		ITimer::OnTimer(t);
		if(!Py_IsInitialized())
			return;

		if(!m_pyOnTimerFunc)
			return;

		PyTaskletObject *pyOnTimerFuncTask = PyTasklet_New(NULL, m_pyOnTimerFunc);
		PyObject *pyParams = PyList_New(0);
		PyObject *pyDt = Py_BuildValue("L", t);
		PyList_Append(pyParams, pyDt);
		if(m_pyOnTimerParam && m_pyOnTimerParam != Py_None)
		{
			if(!PySequence_Check(m_pyOnTimerParam))
			{
				PyList_Append(pyParams, m_pyOnTimerParam);
			}else{
				PySequence_InPlaceConcat(pyParams, m_pyOnTimerParam);
			}
		}
		PyObject *pyTupleParams = PySequence_Tuple(pyParams);
		Py_XDECREF(pyParams);

		PyTasklet_Setup(pyOnTimerFuncTask, pyTupleParams, NULL);
		Py_XDECREF(pyDt);
		Py_XDECREF(pyTupleParams);
		Py_XDECREF(pyOnTimerFuncTask);
		if(PyErr_Occurred())
		{
			PyErr_Print();
			PyErr_Clear();
		}
	}

	void CPyTimer::SetOnTimer(unsigned long i, char t, PyObject * fn, PyObject *p)
	{
		__timer.interval = i;
		__timer.tp = t;
		Py_XDECREF(m_pyOnTimerFunc);
		Py_XDECREF(m_pyOnTimerParam);
		m_pyOnTimerFunc = fn;
		Py_XINCREF(m_pyOnTimerFunc);
		m_pyOnTimerParam = p;
		Py_XINCREF(m_pyOnTimerParam);
	}

	void CPyTimer::OnDestroy()
	{
		g_timer_allocator.free(this);
	}

	PyObject* CreateTimer(PyObject *self, PyObject* args)
	{
		if( !args )
			Py_RETURN_FALSE;
		unsigned long interval = 1;
		char flag = 0;
		PyObject *pObjFunc;
		PyObject *pObjParam = NULL;

        if( !PyArg_ParseTuple(args, "iOi|O",&interval, &pObjFunc, &flag, &pObjParam) )
        {
			if(PyErr_Occurred())
			{
				PyErr_Print();
				PyErr_Clear();
			}
            Py_RETURN_NONE;
        }

		CPyTimer *pT = CPyTimer::g_timer_allocator.allocate();
		if(pT)
		{
			pT = (CPyTimer *) new(pT)CPyTimer();
			pT->SetOnTimer(interval, flag, pObjFunc, pObjParam);
			ITimer::AddTimer(dynamic_cast<ITimer *>(pT));
			return Py_BuildValue("L", pT->GetPtr()->id);
		}else
		{
			return Py_BuildValue("L", 0);
		}
	}

	PyObject* DestroyTimer(PyObject *self, PyObject* args)
	{
		int id = 0;
		if( !PyArg_ParseTuple(args, "L:ref", &id) )
			Py_RETURN_FALSE;
		ITimer::DelTimer(id);
		Py_RETURN_TRUE;
	}

    bool CStacklessEngine::InitModule()
    {
        CPyModule module("kernel", "Multiple Language Server Environment", this);
        module.RegisterCallable("BeNice", (PyCFunction)BeNice, METH_VARARGS);
        module.RegisterCallable("SetCallback", (PyCFunction)_SetCallback, METH_VARARGS);
        module.RegisterCallable("CalcRunCount", (PyCFunction)CalcRunCount, METH_VARARGS);
        module.RegisterIntValue("CBTP_ON_CONNECT",eCBTP_CONNECT);
        module.RegisterIntValue("CBTP_ON_CLOSE",eCBTP_CLOSE);
        module.RegisterIntValue("CBTP_ON_PACKET",eCBTP_PACKET);
        module.RegisterIntValue("CBTP_ON_TERMINATE",eCBTP_TERMINATE);
        module.Embed();

        CPyModule moduleTimer("kernel.timer", "Timer module", this);
		moduleTimer.RegisterCallable("CreateTimer", (PyCFunction)CreateTimer, METH_VARARGS);
		moduleTimer.RegisterCallable("DestroyTimer", (PyCFunction)DestroyTimer, METH_VARARGS);
		moduleTimer.RegisterIntValue("TIMER_TP_LOOP",eTimerTP_LOOP);
		moduleTimer.RegisterIntValue("TIMER_TP_ONCE",eTimerTP_ONCE);
		moduleTimer.Embed("kernel", "timer");

#if (TARGET_PLATFORM == PLATFORM_LINUX)
		CPyModule moduleEvent("kernel.event", "Event IO module", this);
		moduleEvent.RegisterCallable("Bind", EventOP_Bind, METH_VARARGS);
		moduleEvent.RegisterCallable("UnBind", EventOP_UnBind, METH_VARARGS);
		moduleEvent.RegisterIntValue("EVENT_READ",EPOLLIN);
		moduleEvent.RegisterIntValue("EVENT_WRITE",EPOLLOUT);
		moduleEvent.RegisterIntValue("EVENT_ERROR",EPOLLERR);
		moduleEvent.Embed("kernel", "event");
#endif
        return true;
    }
NSE

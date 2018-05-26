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

#include "testpeer.h"
#include <protocol.h>

NS(NBPy)

    void CTestScriptEngine::Initialize() {
        m_pyOnPacketFunc        = NULL;
        m_pyOnConnectFunc       = NULL;
        m_pyOnCloseFunc         = NULL;

        Py_Initialize();
        if(!Py_IsInitialized()){
            return;
        }
        Run("import sys;reload(sys);sys.setdefaultencoding('utf-8')");

        InitModule();
    }

    void CTestScriptEngine::Destroy() {
        Py_XDECREF(m_pyOnConnectFunc);
        Py_XDECREF(m_pyOnCloseFunc);
        Py_XDECREF(m_pyOnPacketFunc);

        Py_Finalize();
    }

    PyObject *CTestScriptEngine::_SetCallback(PyObject *pyObj, PyObject *pyArgs)
    {
        if( !pyArgs )
            Py_RETURN_FALSE;

        PyObject *pObj;
        int tp = 0;
        if( !PyArg_ParseTuple(pyArgs, "iO", &tp, &pObj) )
            Py_RETURN_FALSE;

		if(!PyCallable_Check(pObj))
			Py_RETURN_FALSE;

		CTestScriptEngine::GetInstance()->SetCallback(tp, pObj);

		Py_RETURN_TRUE;
    }

    PyObject *CTestScriptEngine::Send(PyObject *pyObj, PyObject *pyArgs)
    {
        if( !pyArgs )
            Py_RETURN_FALSE;
		Py_RETURN_TRUE;
    }

    PyObject *CTestScriptEngine::Connect(PyObject *pyObj, PyObject *pyArgs){
        if( !pyArgs )
            Py_RETURN_FALSE;

        PyObject *pAddr;
        int pt = 0;
        if( !PyArg_ParseTuple(pyArgs, "Oi", &pAddr, pt) )
            Py_RETURN_FALSE;

		if(!PyString_Check(pAddr))
			Py_RETURN_FALSE;

		char *szAddr = PyString_AsString(pAddr);

		CTestPeer::GetInstance()->Disconnect();
		CTestPeer::GetInstance()->Connect(szAddr, pt);

		Py_RETURN_TRUE;
    }

    PyObject *CTestScriptEngine::Close(PyObject *pyObj, PyObject *pyArgs){
		CTestPeer::GetInstance()->Disconnect();
		Py_RETURN_TRUE;
    }


    int CTestScriptEngine::Run(const char *f, ...)
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


    bool CTestScriptEngine::BindModule(const char *pszPath, const char *pszModule, bool raw_flag)
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

    PyObject *CTestScriptEngine::LoadModule(const char *pszModuleName)
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

    void CTestScriptEngine::SetCallback(int tp, PyObject *pFunc)
    {
        if(!PyCallable_Check(pFunc))
            return;

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
            default:{}
        }
    }

    bool CTestScriptEngine::OnPacketIn(int s, LPPACKET pPacket)
    {
        if(!Py_IsInitialized())
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
            DEBUG_WARN("CTestScriptEngine::OnPacketIn >> packet is empty!");
            return false;
        }

        if(m_pyOnPacketFunc && PyCallable_Check(m_pyOnPacketFunc))
        {
        	PyObject_CallFunction(m_pyOnPacketFunc, "iO", s, pyPacket);
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

        Py_XDECREF(pyPacket);
        return true;
    }
    bool CTestScriptEngine::OnConnect(int s, const char *szAddr, unsigned short uPort)
    {
        if(!Py_IsInitialized())
            return false;
        if(m_pyOnConnectFunc && PyCallable_Check(m_pyOnConnectFunc))
        {
        	PyObject_CallFunction(m_pyOnConnectFunc, "isi", s, szAddr, uPort);
            if(PyErr_Occurred())
            {
                PyErr_Print();
                PyErr_Clear();
            }
        }else
            return false;
        return true;
    }

    bool CTestScriptEngine::OnClose(int s)
    {
        if(!Py_IsInitialized())
            return false;
        if(m_pyOnCloseFunc && PyCallable_Check(m_pyOnCloseFunc))
        {
        	PyObject_CallFunction(m_pyOnCloseFunc, "i", s);
            if(PyErr_Occurred())
            {
                PyErr_Print();
                PyErr_Clear();
            }
        } else
            return false;
        return true;
    }

    bool CTestScriptEngine::InitModule()
    {
        CPyModule module("kernel", "Multiple Language Server Environment", this);
        module.RegisterCallable("Send", (PyCFunction)Send, METH_VARARGS);
        module.RegisterCallable("Connect", (PyCFunction)Connect, METH_VARARGS);
        module.RegisterCallable("Close", (PyCFunction)Close, METH_VARARGS);
        module.RegisterCallable("SetCallback", (PyCFunction)_SetCallback, METH_VARARGS);
        module.RegisterIntValue("CBTP_ON_CONNECT",eCBTP_CONNECT);
        module.RegisterIntValue("CBTP_ON_CLOSE",eCBTP_CLOSE);
        module.RegisterIntValue("CBTP_ON_PACKET",eCBTP_PACKET);
        module.Embed();

        return true;
    }

    void CTestPeer::Initialize(){
    }

    void CTestPeer::Destroy(){

    }

    bool CTestPeer::OnClose(){
        return false;
    }

    bool CTestPeer::OnReadReady(LPPACKET p){
        return false;
    }

NSE

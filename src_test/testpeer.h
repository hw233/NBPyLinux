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
 * File: routepeer.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef TESTCLIENT_TESTPEER_H
#define TESTCLIENT_TESTPEER_H

#include <macro_py.h>
#include <peer.h>
#include <singleton.h>

#include <pyport.h>

NS(NBPy)

    enum __enumClientCallbackType
    {
        eCBTP_CONNECT 	= 1,
        eCBTP_CLOSE 	= 2,
        eCBTP_PACKET 	= 3,
    };


	class CTestScriptEngine: public IScriptEngine{
		DECLARE_SINGLETON(CTestScriptEngine)
	public:
		void Initialize();
		void Destroy();

        bool OnPacketIn(int s,LPPACKET pck);
        bool OnConnect(int s, const char *szAddr, unsigned short uPort);
        bool OnClose(int s);

        int Run(const char *, ...);
        PyObject *LoadModule(const char *);

        bool BindModule(const char *pszPath, const char *pszModule, bool raw_flag);

        void SetCallback(int, PyObject *);
        bool InitModule();

        static PyObject * _SetCallback(PyObject *pyObj, PyObject *pyArgs);
        static PyObject * Send(PyObject *pyObj, PyObject *pyArgs);
        static PyObject * Connect(PyObject *pyObj, PyObject *pyArgs);
        static PyObject * Close(PyObject *pyObj, PyObject *pyArgs);

    private:
        PyObject *m_pyOnPacketFunc;
        PyObject *m_pyOnConnectFunc;
        PyObject *m_pyOnCloseFunc;
	};

    class CTestPeer: public CPeer{
        DECLARE_SINGLETON(CTestPeer)
    public:
        void Initialize();
        void Destroy();

        bool OnClose();
        bool OnReadReady(LPPACKET p);
    private:
    };

NSE

#endif //TESTCLIENT_TESTPEER_H

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
 * File: pyextend.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "pyextend.h"
#include "s2stunnel.h"
#include "routepeer.h"
#include "clientpeer.h"
#include "setting.h"

NS(NBPy)

    PYCLASS_INIT_F(CPyConnsvr, "current", "Connsvr Object");

    PYCLASS_BEGIN_METHOD_DEF(CPyConnsvr)
                    PYCLASS_ADD_METHOD_EX(CPyConnsvr, Inform, METH_VARARGS)
                    PYCLASS_ADD_METHOD_EX(CPyConnsvr, Send, METH_VARARGS)
                    PYCLASS_ADD_METHOD_EX(CPyConnsvr, SendClient, METH_VARARGS)
                    PYCLASS_ADD_METHOD_EX(CPyConnsvr, Deliver, METH_VARARGS)
                    PYCLASS_ADD_METHOD_EX(CPyConnsvr, Close, METH_VARARGS)
                    PYCLASS_ADD_METHOD_EX(CPyConnsvr, GetPIDFilePath, METH_VARARGS)
    PYCLASS_END_METHOD_DEF(CPyConnsvr)

    PYCLASS_BEGIN_GETSET_DEF(CPyConnsvr)
    PYCLASS_END_GETSET_DEF(CPyConnsvr)

    PYCLASS_IMP_INTERFACE(CPyConnsvr)

    PYCLASS_METHOD_IMPL(CPyConnsvr, GetPIDFilePath)
    {
        return PyString_FromString(CSetting::GetInstance()->GetPIDFilePath().c_str());
    }

    PYCLASS_METHOD_IMPL(CPyConnsvr, Inform)
    {
        if( !pyArgs )
            Py_RETURN_FALSE;

        PyObject *pids = NULL;
        if( !PyArg_ParseTuple(pyArgs, "O", &pids) )
            Py_RETURN_FALSE;

        if(!PySequence_Check(pids))
            Py_RETURN_FALSE;

        Py_ssize_t se = PySequence_Fast_GET_SIZE(pids);
        PyObject *v = NULL;
        for (int i = 0; i < se; i++) {
            //此处是借来的引用，无需负责decref
            //!而PySequence_GetItem是新引用，需要负责decref!!!!
            v = PySequence_Fast_GET_ITEM(pids, i);
            pid_t pid = (pid_t)PyLong_AsLong(v);
            CS2STunnel::GetInstance()->Inform(pid);
        }
        Py_RETURN_TRUE;
    }

    PYCLASS_METHOD_IMPL(CPyConnsvr, SendClient)
    {
        if( !pyArgs )
            Py_RETURN_FALSE;

        PyObject *pObj;
        int fd = 0;
        if( !PyArg_ParseTuple(pyArgs, "iO", &fd, &pObj) )
            Py_RETURN_FALSE;

        if(!PyDict_Check(pObj))
            Py_RETURN_FALSE;

        IPeer *p = CPeerFactory::GetInstance()->FindPeer(fd);
        if(!p)
            Py_RETURN_FALSE;

        CIMDictCode code;
        if(!CPythonCodec::GetInstance()->GetEncoder()->Encode(&code, pObj))
            Py_RETURN_FALSE;

        CBuffer bufSend;
        PACKET sp;
        sp.SetAsIMCode();
        sp.Pack(&bufSend,
                 (char *)code.GetRawBuffer()->Begin(), code.GetRawBuffer()->GetDataSize());

        if(p->Send((char *)bufSend.Begin(), bufSend.GetDataSize()))
            Py_RETURN_TRUE;

        Py_RETURN_FALSE;
    }

    PYCLASS_METHOD_IMPL(CPyConnsvr, Send)
    {
        if( !pyArgs )
            Py_RETURN_FALSE;

        PyObject *pObj;
        int tag = 0;
        if( !PyArg_ParseTuple(pyArgs, "iO", &tag, &pObj) )
            Py_RETURN_FALSE;

        if(!PyDict_Check(pObj))
            Py_RETURN_FALSE;

        CIMDictCode code;
        if(!CPythonCodec::GetInstance()->GetEncoder()->Encode(&code, pObj))
            Py_RETURN_FALSE;

        TAGPIDS &tps = CSetting::GetInstance()->GetTagPids();
        TAGPIDS::iterator it = tps.find(tag);
        if(it == tps.end())
			Py_RETURN_FALSE;

        bool sc = true;

        std::map<pid_t, int> &km = it->second;

        pid_t failpids[64];
        memset(failpids, 0, sizeof(pid_t) * 64);
        int i = 0;
        for(std::map<pid_t, int>::iterator kv=km.begin(); kv != km.end(); kv ++)
        {
            if(!CS2STunnel::GetInstance()->SendIMCode(kv->first, &code))
            {
                failpids[i] = kv->first;
                i ++;
                sc = false;
            }
            else{
            }
        }

        if(!sc)
        {
			/*PyObject *ret = PyList_New(0);
			for(int i = 0; i < 64; i ++)
			{
				if(failpids[i] == 0)
					break;
				PyObject *pyO = Py_BuildValue("i", failpids[i]);
				PyList_Append(ret, pyO);
				Py_XDECREF(pyO);
			}*/
			CRoutePeer::GetInstance()->Deliver(tag, &code);
			Py_RETURN_TRUE;
        }
        Py_RETURN_TRUE;
    }

    PYCLASS_METHOD_IMPL(CPyConnsvr, Deliver)
    {
        if( !pyArgs )
            Py_RETURN_FALSE;

        PyObject *pObj;
        int tag = 0;
        if( !PyArg_ParseTuple(pyArgs, "iO", &tag, &pObj) )
            Py_RETURN_FALSE;

        if(!PyDict_Check(pObj))
            Py_RETURN_FALSE;

        CIMDictCode code;
        if(!CPythonCodec::GetInstance()->GetEncoder()->Encode(&code, pObj))
            Py_RETURN_FALSE;

        CRoutePeer::GetInstance()->Deliver(tag, &code);

        Py_RETURN_TRUE;
    }

    PYCLASS_METHOD_IMPL(CPyConnsvr, Close)
    {
        if( !pyArgs )
            Py_RETURN_FALSE;

        PyObject *pObj;
        int fd = 0;
        if( !PyArg_ParseTuple(pyArgs, "i", &fd) )
            Py_RETURN_FALSE;


		CClientPeer *peer = dynamic_cast<CClientPeer *>(CPeerFactory::GetInstance()->FindPeer(fd));
        if(!peer)
            Py_RETURN_FALSE;

		peer->Close();

        Py_RETURN_TRUE;
    }

    void InitExtendModule(){
        CPyModule module("kernel.processor", "processor module", CStacklessEngine::GetInstance());
        module.RegisterClass<CPyConnsvr>();
        module.Embed("kernel", "processor");
    }
NSE

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
#include "clientpeer.h"
#include "s2stunnel.h"
#include "routepeer.h"
#include "setting.h"

NS(NBPy)

    PYCLASS_INIT_F(CPyLoginsvr, "current", "Loginsvr Object");

    PYCLASS_BEGIN_METHOD_DEF(CPyLoginsvr)
                    PYCLASS_ADD_METHOD_EX(CPyLoginsvr, Inform, METH_VARARGS)
                    PYCLASS_ADD_METHOD_EX(CPyLoginsvr, Send, METH_VARARGS)
                    PYCLASS_ADD_METHOD_EX(CPyLoginsvr, BatchSend, METH_VARARGS)
                    PYCLASS_ADD_METHOD_EX(CPyLoginsvr, SetTags, METH_VARARGS)
                    PYCLASS_ADD_METHOD_EX(CPyLoginsvr, Deliver, METH_VARARGS)
                    PYCLASS_ADD_METHOD_EX(CPyLoginsvr, GetPIDFilePath, METH_VARARGS)
    PYCLASS_END_METHOD_DEF(CPyLoginsvr)

    PYCLASS_BEGIN_GETSET_DEF(CPyLoginsvr)
    PYCLASS_END_GETSET_DEF(CPyLoginsvr)

    PYCLASS_IMP_INTERFACE(CPyLoginsvr)

    PYCLASS_METHOD_IMPL(CPyLoginsvr, GetPIDFilePath)
    {
        return PyString_FromString(CSetting::GetInstance()->GetPIDFilePath().c_str());
    }

    PYCLASS_METHOD_IMPL(CPyLoginsvr, SetTags)
    {
        if( !pyArgs )
            Py_RETURN_FALSE;

        PyObject *tags = NULL;
        if( !PyArg_ParseTuple(pyArgs, "O", &tags) )
            Py_RETURN_FALSE;

        if(!PySequence_Check(tags))
            Py_RETURN_FALSE;

        Py_ssize_t se = PySequence_Fast_GET_SIZE(tags);
        PyObject *v = NULL;
        for (int i = 0; i < se; i++) {
            //此处是借来的引用，无需负责decref
            //!而PySequence_GetItem是新引用，需要负责decref!!!!
            v = PySequence_Fast_GET_ITEM(tags, i);
            int tag = (int)PyLong_AsLong(v);
            CRoutePeer::GetInstance()->AddTag(tag);
        }
        Py_RETURN_TRUE;
    }

    PYCLASS_METHOD_IMPL(CPyLoginsvr, Inform)
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
			//DEBUG_STATUS("inform to %d suscess", pid);
        }

        Py_RETURN_TRUE;
    }

    PYCLASS_METHOD_IMPL(CPyLoginsvr, BatchSend)
    {
    	unsigned long lt = CDateTime::timestamp();
        if( !pyArgs )
			Py_RETURN_NONE;

        PyObject *pObj;
        PyObject *pids;
        if( !PyArg_ParseTuple(pyArgs, "OO", &pids, &pObj) )
			Py_RETURN_NONE;

        if(!PyDict_Check(pObj))
			Py_RETURN_NONE;

		if(!PySequence_Check(pids))
			Py_RETURN_NONE;

        CIMDictCode code;
        if(!CPythonCodec::GetInstance()->GetEncoder()->Encode(&code, pObj))
            Py_RETURN_FALSE;


		PyObject *pyFails = PyList_New(0);
        Py_ssize_t se = PySequence_Fast_GET_SIZE(pids);
        PyObject *v = NULL;
        for (int i = 0; i < se; i++) {
            //此处是借来的引用，无需负责decref
            //!而PySequence_GetItem是新引用，需要负责decref!!!!
            v = PySequence_Fast_GET_ITEM(pids, i);
            int pid = (int)PyLong_AsLong(v);

			if(!CS2STunnel::GetInstance()->SendIMCode(pid, &code)){
				PyList_Append(pyFails, Py_BuildValue("i",pid));
			}
        }

		lt = CDateTime::timestamp() - lt;
		//DEBUG_STATUS("[S2S-BATCH-SEND] %ld ms", lt);

        return pyFails;
    }

    PYCLASS_METHOD_IMPL(CPyLoginsvr, Send)
    {
    	unsigned long lt = CDateTime::timestamp();
        if( !pyArgs )
            Py_RETURN_FALSE;

        PyObject *pObj;
        int pid = 0;
        if( !PyArg_ParseTuple(pyArgs, "iO", &pid, &pObj) )
            Py_RETURN_FALSE;

        if(!PyDict_Check(pObj))
            Py_RETURN_FALSE;

		if(pid == 0 || pid == getpid()){
			//DEBUG_STATUS("SEND TO CLIENT");

			PyObject *pyTo = PyDict_GetItem(pObj, Py_BuildValue("i", 0x11));
			PyObject *pyPack = PyDict_GetItem(pObj, Py_BuildValue("i", 0x14));

			PyDict_DelItem(pObj, Py_BuildValue("i", 0x11));

			CIMDictCode code;
			if(!CPythonCodec::GetInstance()->GetEncoder()->Encode(&code, pyPack))
				Py_RETURN_FALSE;

			static CBuffer bufSend;
			static PACKET pckSend;

			bufSend.Reset();
			memset(&pckSend, 0, sizeof(pckSend));
			pckSend.SetAsIMCode();
			pckSend.Pack(&bufSend
					,(char *)code.GetRawBuffer()->Begin()
					,code.GetRawBuffer()->GetDataSize());

			Py_ssize_t se = PySequence_Fast_GET_SIZE(pyTo);
			PyObject *v = NULL;
			for (int i = 0; i < se; i++)
			{
				//此处是借来的引用，无需负责decref
				//!而PySequence_GetItem是新引用，需要负责decref!!!!
				v = PySequence_Fast_GET_ITEM(pyTo, i);
				if(v == Py_None)
					continue;

				unsigned int fd = (unsigned int)PyLong_AsUnsignedLong(v);

				//DEBUG_STATUS("SEND TO %d", fd);

                CClientPeer *peer = dynamic_cast<CClientPeer *>(CPeerFactory::GetInstance()->FindPeerBySID(fd));
                if(peer)
                {
                    peer->MutilSend(&bufSend);
                }
			}

			Py_RETURN_TRUE;
		}

        CIMDictCode code;
        if(!CPythonCodec::GetInstance()->GetEncoder()->Encode(&code, pObj))
            Py_RETURN_FALSE;

        if(CS2STunnel::GetInstance()->SendIMCode(pid, &code)){
			lt = CDateTime::timestamp() - lt;
			//DEBUG_STATUS("[S2S-SEND] %ld ms", lt);
            Py_RETURN_TRUE;
        }

        Py_RETURN_FALSE;
    }

    PYCLASS_METHOD_IMPL(CPyLoginsvr, Deliver)
    {
    	unsigned long lt = CDateTime::timestamp();
        if( !pyArgs )
            Py_RETURN_FALSE;

        PyObject *pObj;
        if( !PyArg_ParseTuple(pyArgs, "O", &pObj) )
            Py_RETURN_FALSE;

        if(!PyDict_Check(pObj))
            Py_RETURN_FALSE;

        CIMDictCode code;
        if(!CPythonCodec::GetInstance()->GetEncoder()->Encode(&code, pObj))
            Py_RETURN_FALSE;

        CRoutePeer::GetInstance()->Deliver(&code);

		lt = CDateTime::timestamp() - lt;
		DEBUG_STATUS("[DELIVER] %ld ms", lt);
        Py_RETURN_TRUE;
    }

    void InitExtendModule(){
        CPyModule module("kernel.processor", "processor module", CStacklessEngine::GetInstance());
        module.RegisterClass<CPyLoginsvr>();
        module.Embed("kernel", "processor");
    }
NSE

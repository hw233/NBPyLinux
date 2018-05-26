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
#include "setting.h"

NS(NBPy)

    PYCLASS_INIT_F(CPyLogicsvr, "current", "Logicsvr Object");

    PYCLASS_BEGIN_METHOD_DEF(CPyLogicsvr)
                    PYCLASS_ADD_METHOD_EX(CPyLogicsvr, Inform, METH_VARARGS)
                    PYCLASS_ADD_METHOD_EX(CPyLogicsvr, Send, METH_VARARGS)
                    PYCLASS_ADD_METHOD_EX(CPyLogicsvr, BatchSend, METH_VARARGS)
                    PYCLASS_ADD_METHOD_EX(CPyLogicsvr, SetTags, METH_VARARGS)
                    PYCLASS_ADD_METHOD_EX(CPyLogicsvr, Deliver, METH_VARARGS)
                    PYCLASS_ADD_METHOD_EX(CPyLogicsvr, GetPIDFilePath, METH_VARARGS)
    PYCLASS_END_METHOD_DEF(CPyLogicsvr)

    PYCLASS_BEGIN_GETSET_DEF(CPyLogicsvr)
    PYCLASS_END_GETSET_DEF(CPyLogicsvr)

    PYCLASS_IMP_INTERFACE(CPyLogicsvr)

    PYCLASS_METHOD_IMPL(CPyLogicsvr, GetPIDFilePath)
    {
        return PyString_FromString(CSetting::GetInstance()->GetPIDFilePath().c_str());
    }

    PYCLASS_METHOD_IMPL(CPyLogicsvr, SetTags)
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

    PYCLASS_METHOD_IMPL(CPyLogicsvr, Inform)
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

    PYCLASS_METHOD_IMPL(CPyLogicsvr, Send)
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

    PYCLASS_METHOD_IMPL(CPyLogicsvr, BatchSend)
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

    PYCLASS_METHOD_IMPL(CPyLogicsvr, Deliver)
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
        module.RegisterClass<CPyLogicsvr>();
        module.Embed("kernel", "processor");
    }
NSE

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
 * File: macro_py.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_MACRO_PY_H
#define BASE_MACRO_PY_H

#include "codec_py.h"
#include "output.h"
#include "network.h"
//////////////////////////////////////////////////////////////////////////
// PYCLASS HELPER MACROS
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// PYCLASS DEFINE/IMPLEMENT MACROS
//////////////////////////////////////////////////////////////////////////

// PYCLASS METHOD IMPLEMENT MACRO
#define PYCLASS_METHOD_IMPL(OBJ, n)\
	PyObject * OBJ::n##_Callable(OBJ *pySelf, PyObject *pyArgs)

// PYCLASS METHOD DEFINE MACRO
#define PYCLASS_METHOD_DEF(OBJ, n)\
	static 	PyObject * n##_Callable(OBJ *pySelf, PyObject *pyArgs)


// PYCLASS METHOD IMPLEMENT MACRO (Accept KeyWord Args)
#define PYCLASS_METHOD_KW_IMPL(OBJ, n)\
	PyObject * OBJ::n##_Callable(OBJ *pySelf, PyObject *pyArgs, PyObject *pyKW)

// PYCLASS METHOD DEFINE MACRO (Accept KeyWord Args)
#define PYCLASS_METHOD_KW_DEF(OBJ, n)\
	static PyObject * n##_Callable(OBJ *pySelf, PyObject *pyArgs, PyObject *pyKW)

//////////////////////////////////////////////////////////////////////////
// PYCLASS REGISTER MACROS
//////////////////////////////////////////////////////////////////////////
#define PYCLASS_INIT_F(OBJ, n, doc)\
	const char *OBJ::__name__ = n;\
	const char *OBJ::__doc__ = doc;

#define PYCLASS_INIT(OBJ, n)\
	PYCLASS_INIT_F(OBJ, n, NULL)
//////////////////////////////////////////////////////////////////////////
// METHOD DEFINE
#define PYCLASS_BEGIN_METHOD_DEF(OBJ)\
static PyMethodDef OBJ##_cls_methods[] = {

#define PYCLASS_ADD_METHOD_F_EX(OBJ,name,flag,doc)\
{ #name,         (PyCFunction)OBJ::name##_Callable, flag,	doc},

#define PYCLASS_ADD_METHOD_F(OBJ,name,doc)\
PYCLASS_ADD_METHOD_F_EX(OBJ, name, METH_VARARGS, doc)

#define PYCLASS_ADD_METHOD(OBJ, name)\
PYCLASS_ADD_METHOD_F_EX(OBJ, name, METH_VARARGS, NULL)

#define PYCLASS_ADD_METHOD_EX(OBJ, name, flag)\
PYCLASS_ADD_METHOD_F_EX(OBJ, name, flag, NULL)

#define PYCLASS_END_METHOD_DEF(OBJ)\
    {NULL,NULL,0,NULL}\
};\
PyMethodDef *OBJ::__methods__ = OBJ##_cls_methods;

//////////////////////////////////////////////////////////////////////////
// GETSET DEFINE
#define PYCLASS_BEGIN_GETSET_DEF(OBJ)\
static PyGetSetDef OBJ##_cls_getsets[] = {

#define PYCLASS_ADD_GET_F(OBJ, name, doc)\
{#name, (getter)OBJ::getter_##name, NULL, doc,	NULL},

#define PYCLASS_ADD_SET_F(OBJ, name, doc)\
{#name, NULL, (setter)OBJ::setter_##name, doc,	NULL},

#define PYCLASS_ADD_GETSET_F(OBJ, name, doc)\
{#name, (getter)OBJ::getter_##name, (setter)OBJ::setter_##name, doc,	NULL},

#define PYCLASS_ADD_GET(OBJ, name)\
PYCLASS_ADD_GET_F(OBJ, name, NULL)

#define PYCLASS_ADD_SET(OBJ, name)\
PYCLASS_ADD_SET_F(OBJ, name, NULL)

#define PYCLASS_ADD_GETSET(OBJ, name)\
PYCLASS_ADD_GETSET_F(OBJ, name, NULL)

#define PYCLASS_END_GETSET_DEF(OBJ)\
{NULL,NULL,NULL}\
};\
PyGetSetDef *OBJ::__getsets__ = OBJ##_cls_getsets;

#define PYCLASS_GET_IMPL(OBJ, name)\
	PyObject *OBJ::getter_##name(OBJ *pySelf, void *closure)

#define PYCLASS_GET_DEF(OBJ, name)\
	static PYCLASS_GET_IMPL(OBJ, name)

#define PYCLASS_SET_IMPL(OBJ, name)\
	int OBJ::setter_##name(OBJ *pySelf, PyObject *pyValue, void *closure)

#define PYCLASS_SET_DEF(OBJ, name)\
	static PYCLASS_SET_IMPL(OBJ, name)

#define PYCLASS_GETSET_MAPPING_DEF(OBJ, name)\
	PYCLASS_GET_DEF(OBJ, name);\
	PYCLASS_SET_DEF(OBJ, name)

#define PYCLASS_GETSET_MAPPING_IMPL(OBJ, name, ctp, tp, ttp)\
	PYCLASS_GET_IMPL(OBJ, name)\
	{\
		if(!pySelf->__ptr__)\
			Py_RETURN_NONE;\
		return Py##tp##_From##ttp(pySelf->__ptr__->##name);\
	}\
	PYCLASS_SET_IMPL(OBJ, name)\
	{\
		if(!pySelf->__ptr__)\
			return 0;\
		pySelf->__ptr__->##name = (ctp)Py##tp##_As##ttp##(pyValue);\
		return 0;\
	}

#define PYCLASS_GETSET_MAPPING_IMPL_INT(OBJ, name)\
	PYCLASS_GETSET_MAPPING_IMPL(OBJ, name, long, Int, Long)
#define PYCLASS_GETSET_MAPPING_IMPL_UINT(OBJ, name)\
	PYCLASS_GETSET_MAPPING_IMPL(OBJ, name, unsigned long, Long, UnsignedLong)
#define PYCLASS_GETSET_MAPPING_IMPL_FLOAT(OBJ, name)\
	PYCLASS_GETSET_MAPPING_IMPL(OBJ, name, float, Float, Double)
#define PYCLASS_GETSET_MAPPING_IMPL_BOOL(OBJ, name)\
	PYCLASS_GETSET_MAPPING_IMPL(OBJ, name, bool, Bool, Long)
#define PYCLASS_GETSET_MAPPING_IMPL_STRING(OBJ, name)\
	PYCLASS_GETSET_MAPPING_IMPL(OBJ, name, char *, String, String)
//////////////////////////////////////////////////////////////////////////

#define PYCLASS_DEF_INTERFACE(O)\
public:\
	static PyObject *__new__(PyTypeObject *, PyObject *, PyObject *);\
	static int __init__(O *, PyObject *, PyObject *);\
	static void __del__(O *self);\
	static PyMethodDef *__methods__;\
	static PyGetSetDef *__getsets__;\
	static const char *__name__;\
	static const char *__doc__;

#define PYCLASS_IMP_INTERFACE_NEW(O)\
PyObject* O::__new__(PyTypeObject *pytype, PyObject *args, PyObject *kwds)\
{\
	LPCPyClassBase pClassBase = CPyModule::GetPyClassBase();\
	if(!pClassBase)\
	{\
	Py_RETURN_NONE;\
	}\
	O *p = PyObject_NEW(O, pytype);\
	if( !p )\
	{\
	Py_RETURN_NONE;\
	}\
	p = new(p) O(pClassBase->pMachine);\
	p->pMachine = pClassBase->pMachine;\
	PyObject *po = (PyObject*)p;\
	return po;\
}

#define PYCLASS_IMP_INTERFACE_INIT(O)\
int O::__init__(O *pySelf, PyObject *args, PyObject *kwds)\
{\
	return 0;\
}

#define PYCLASS_IMP_INTERFACE_DEL(O)\
void O::__del__(O *self)\
{\
	self->pMachine = NULL;\
	self->ob_type->tp_free(self);\
}

#define PYCLASS_IMP_INTERFACE_NODEL(O)\
PYCLASS_IMP_INTERFACE_NEW(O)\
PYCLASS_IMP_INTERFACE_INIT(O)

#define PYCLASS_IMP_INTERFACE_NOINIT(O)\
PYCLASS_IMP_INTERFACE_NEW(O)\
PYCLASS_IMP_INTERFACE_DEL(O)

#define PYCLASS_IMP_INTERFACE(O)\
PYCLASS_IMP_INTERFACE_NEW(O)\
PYCLASS_IMP_INTERFACE_INIT(O)\
PYCLASS_IMP_INTERFACE_DEL(O)

NS(NBPy)
    typedef struct __CPyClassBase
    {
        PyObject_HEAD
        IScriptEngine *pMachine;
        static void __del__(__CPyClassBase *self);
        static PyMethodDef *__methods__;
        static PyGetSetDef *__getsets__;
        static const char *__name__;
        static const char *__doc__;

        template<typename T>
        static PyTypeObject *GetTypeObject()
        {
            static PyTypeObject typeinfo = {
                    PyObject_HEAD_INIT(&PyType_Type)
                    0,								/*ob_size*/
                    T::__name__,					/*tp_name*/
                    sizeof(T),						/*tp_basicsize*/
                    0,								/*tp_itemsize*/
                    (destructor)T::__del__,			/*tp_dealloc*/
            };
            return &typeinfo;
        }

        template<typename T>
        static PyTypeObject *GetType()
        {
            PyTypeObject *pType = GetTypeObject<T>();
            pType->tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
            pType->tp_doc = T::__doc__;
            pType->tp_methods = T::__methods__;
            pType->tp_getset = T::__getsets__;
            pType->tp_new = T::__new__;
            pType->tp_init = (initproc)T::__init__;
            pType->tp_str = NULL;
            pType->tp_repr = NULL;

            if( (pType->tp_flags & Py_TPFLAGS_READY) != Py_TPFLAGS_READY)
            {
                PyType_Ready(pType);
            }
            Py_XINCREF(pType);
            return pType;
        };
    }CPyClassBase, *LPCPyClassBase;


    class CPyModule
    {
    public:
        CPyModule(){};
        CPyModule(const char *module_name, const char *module_doc = NULL, IScriptEngine *pMachine = NULL);
        ~CPyModule();
        void RegisterIntValue(const char *name, int v){ m_intValues[name] = v; };
        void RegisterCallable(const char *name, PyCFunction pyFunc, int flag, const char *doc = NULL);
		void Embed(const char *parent = NULL, const char *self_name = NULL);
        template <typename T>
        void RegisterClass()
        {
            PyTypeObject *pyType = CPyClassBase::GetType<T>();
            //Py_XINCREF(pyType);
            m_cls[T::__name__] = pyType;
        };
        static LPCPyClassBase GetPyClassBase(){ return m_pClassBase; };
    private:
        const char *m_name;
        const char *m_doc;
        PyMethodDef *m_pMethods_ref;
        std::vector<PyMethodDef> m_methods;
        std::map<const char *, int> m_intValues;
        std::map<const char *, PyTypeObject *> m_cls;
        static LPCPyClassBase m_pClassBase;
    };

    class CPyOutput : public CPyClassBase
    {
    PYCLASS_DEF_INTERFACE(CPyOutput)
    public:
        CPyOutput(IScriptEngine *pM){
            pMachine = pM;
            m_pOutput = NULL;
        }
        IOutput *GetOutput(){ return m_pOutput; };
        void SetOutput(IOutput *p){ m_pOutput = p; };
        PYCLASS_METHOD_DEF(CPyOutput,write);
    private:
        IOutput *m_pOutput;
    };

NSE

#endif //BASE_MACRO_PY_H

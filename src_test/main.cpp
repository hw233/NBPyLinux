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
 * File: main.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/
#include <iostream>
#include <stdio.h>
#include <codec_py.h>
#include <imcodepackager.h>

USN(NBPy)

int main(int argc, char** argv) {

	Py_Initialize();

	PyRun_SimpleString("import os,sys;sys.path.append(os.getcwd());");

	PyObject *pyMod = PyImport_ImportModule("ip");
	PyObject *pyD = PyDict_GetItemString(PyModule_GetDict(pyMod), "d");
	PyObject *pyF = PyDict_GetItemString(PyModule_GetDict(pyMod), "pp");

	//printf("\nsrc-len: %ld\n",PyDict_Size(pyD));

	CIMDictCode code;
	CDictPackager tp;
	CPythonCodec::GetInstance()->GetEncoder()->Encode(&tp, pyD);

	//DEBUG_STATUS("SRC-LEN2: %ld", tp.GetSize());

	tp.Save(&code);

	FILE *f = fopen("./ffpack","w");
	fwrite(code.GetRawBuffer()->Begin(),code.GetRawBuffer()->GetDataSize(), 1, f);
	fclose(f);

	CDictPackager pack(&code);

	//printf("\ndecode-len: %ld\n",pack.GetSize());

	PyObject *pyD2 = PyDict_New();
	CPythonCodec::GetInstance()->GetDecoder()->Decode(&code, pyD2);

	PyObject_CallFunction(pyF,"O", pyD2);

	printf("\nok\n");

	Py_Finalize();
}
/*
#include "testpeer.h"

#include <getopt.h>
#include <signalutils.h>

USN(NBPy);

bool io_stop = false;
bool runat_deamon = false;

void __SIGFN(int sigNo)
{
    switch( sigNo )
    {
        case SIGINT:
        {
            if(runat_deamon)
                break;
        }
        case SIGTERM:
        case SIGSTOP:
        case SIGQUIT:
        {
            DEBUG_STATUS(">>> Signal[%d] received. The process is going to shutdown.",sigNo);
            io_stop = true;
            break;
        }
        default:
        {
            DEBUG_STATUS(">>> Signal[%d] received.",sigNo);
            break;
        }
    }
};

void print_usage()
{
    printf("-------------------------------------------------------------------\n\
    Copyright 2017@ starlove all rights reserved.\n\
       Usage: tester [Option...] \n\
       Options:\n\
       \n\
       \t -h  --help: Display this usage information.\n\
       \t -m  --module: python module name\n\
       \t -p  --packetmask: network packet mask\n\
--------------------------------------------------------------------\n");
};

const struct option __options[] =
        {
                {"help",        0, NULL, 'h'},
                {"module",      1, NULL, 'm'},
                {"packetmask",  1, NULL, 'p'},
                {NULL,          0, NULL, 0}
        };

int main(int argc, char** argv) {

	printf("%ld\n", sizeof(long));

    unsigned int nPackMask = 0;
    int opt = 0;

    std::string szSckPath = "./";
    std::string szScriptMod = "";

    while ( (opt = getopt_long(argc, argv, "hm:p:", __options, NULL)) != -1 )
    {
        switch (opt)
        {
            case 'h':
            {
                print_usage();
                return 0;
            }
            case 'p':
            {
                nPackMask = atoi(optarg);
                break;
            }
            case 'm':
            {
                szScriptMod = optarg;
                break;
            }
            default:{}
        }
    }//end of while

    if(szScriptMod.size() == 0){
        DEBUG_STATUS(">>> Module name not specified");
        print_usage();
        return -1;
    }

    DEBUG_STATUS(">>> The current network packet mask is [0x%x]", nPackMask);

    //绑定系统信号处理函数
    CSignalUtils::handler(SIGTERM,__SIGFN);
    CSignalUtils::handler(SIGKILL,__SIGFN);
    CSignalUtils::handler(SIGSTOP,__SIGFN);
    CSignalUtils::handler(SIGHUP,__SIGFN);
    CSignalUtils::handler(SIGPIPE,__SIGFN);
    CSignalUtils::handler(SIGINT,__SIGFN);
    CSignalUtils::handler(SIGQUIT,__SIGFN);


    //设定网络通信数据包校验掩码
    CNetworkUtils::GetInstance()->SetPacketMask(nPackMask);

    //初始化脚本内核模块绑定
    CTestScriptEngine::GetInstance()->InitModule();
    //绑定指定脚本模块
    CTestScriptEngine::GetInstance()->BindModule(szSckPath.c_str(), szScriptMod.c_str(), false);

#if (TARGET_PLATFORM == PLATFORM_MACOS)
    //FOR MAC
    //初始化事件引擎
    CKQueueEventIO::GetInstance()->Init();
    //绑定事件引擎
    CEventDispatcher::GetInstance()->Init(CKQueueEventIO::GetInstance());
#endif

#if (TARGET_PLATFORM == PLATFORM_LINUX)
    //FOR LINUX
    //初始化事件引擎
    CEPollEventIO::GetInstance()->Init();
    //绑定事件引擎
    CEventDispatcher::GetInstance()->Init(CEPollEventIO::GetInstance());
#endif

    DEBUG_STATUS(">>> Worker initialized!");
    while(!io_stop)
    {
        //事件引擎调度
        CEventDispatcher::GetInstance()->Lookup();
    }

    CTestPeer::GetInstance()->Disconnect();

    //停止事件引擎
    CEventDispatcher::GetInstance()->Abort();
    return 0;
}*/

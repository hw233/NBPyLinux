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

#include "pyextend.h"
#include "routepeer.h"
#include "s2stunnel.h"
#include "setting.h"

#include <configfile.h>
#include <getopt.h>
#include <deamonutils.h>
#include <signalutils.h>
#include <stacklessengine.h>

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
       Usage: logicsvr [Option...] \n\
       Options:\n\
       \n\
       \t -h  --help: Display this usage information.\n\
       \t -d  --deamon: deamon mode\n\
       \t -m  --module: python module name\n\
--------------------------------------------------------------------\n");
};

const struct option __options[] =
        {
                {"help",        0, NULL, 'h'},
                {"module",      1, NULL, 'm'},
                {"deamon",      0, NULL, 'd'},
                {NULL,          0, NULL, 0}
        };

int main(int argc, char** argv) {

    int fork_fd = -1;

    unsigned int nPackMask = 0;
    unsigned short uRoutePort = 0;
    int opt = 0;

    std::string szLogPath = "./";
    std::string szSckLogPath = "./";
    std::string szPidFilePath = "./all.pid";
    std::string szRouteAddr = "127.0.0.1";
    std::string szSckPath = "./script/";
    std::string szScriptMod = "";

    while ( (opt = getopt_long(argc, argv, "hm:d", __options, NULL)) != -1 )
    {
        switch (opt)
        {
            case 'h':
            {
                print_usage();
                return 0;
            }
            case 'd':
            {
                runat_deamon = true;
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

    //读入配置文件，并结合当前运行参数决定最终运行配置
    CConfigFile config;
    if(config.LoadFromFile("./conf/server.conf"))
    {
        if(config.EnterSection("main")) {

            if (!runat_deamon)
                runat_deamon = config.GetIntValue("deamon", 0) != 0;

            szLogPath = config.GetStrValue("logdir", szLogPath.c_str());
            szPidFilePath = config.GetStrValue("pidfile", szPidFilePath.c_str());
            szSckPath = config.GetStrValue("scriptdir", szSckPath.c_str());

            nPackMask = (unsigned int) config.GetIntValue("packet-mask", nPackMask);

            szRouteAddr = config.GetStrValue("routeaddr", szRouteAddr.c_str());
            uRoutePort = (unsigned short) config.GetIntValue("routeport");

            size_t rcvbuf = (size_t) config.GetIntValue("rcvbuf", NBPy_UNIXSCOKET_RECV_SIZE);
            CSetting::GetInstance()->SetIPCRecvBufSize(rcvbuf);
        }
    }
    config.Close();
    if(szScriptMod.size() == 0){
        DEBUG_STATUS(">>> Module name not specified");
        print_usage();
        return -1;
    }

    char performanceFN[255];
    char performanceName[255];
    sprintf(performanceFN, "logicsvr-%s", szScriptMod.c_str());
    sprintf(performanceName, "逻辑运算服务器性能统计(%d)", getpid());
    CPerformance::GetInstance()->SetName(performanceFN, performanceName, 200);

    CSetting::GetInstance()->SetPIDFilePath(szPidFilePath);

    //构建日志文件名前缀
    szLogPath += "logicsvr-";
    szLogPath += szScriptMod;

    DEBUG_STATUS(">>> The current network packet mask is [0x%x]", nPackMask);

    //绑定内核日志输出
    CFileOutput fo;
    if (runat_deamon)
    {
        fork_fd = CDeamonUtils::Begin();
        if(!fo.open(szLogPath.c_str()))
        {
            //内核日志输出目标指定失败，无法继续deamon模式
            CDeamonUtils::Complete(fork_fd, -1006);
            return -1;
        }
        COutputSetting::GetInstance()->set_default(&fo);
    }

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

    //绑定脚本日志输出
    CFileOutput fo_script;
    if (runat_deamon) {
        if (!fo_script.open(szLogPath.c_str(), "script")) {
            CDeamonUtils::Complete(fork_fd, -1007);
            return -1;
        }
        CStacklessEngine::GetInstance()->SetOutput(&fo_script);
    }
    //初始化脚本内核模块绑定
    CStacklessEngine::GetInstance()->InitModule();
    //初始化脚本内核附加模块绑定
    InitExtendModule();
    //绑定指定脚本模块
    CStacklessEngine::GetInstance()->BindModule(szSckPath.c_str(), szScriptMod.c_str(), false);

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

    //尝试连接路由索引服务器
    CRoutePeer::GetInstance()->SetScriptEngine(CStacklessEngine::GetInstance());
    if(!CRoutePeer::GetInstance()->Connect(szRouteAddr.c_str(), uRoutePort)){
        CDeamonUtils::Complete(fork_fd, -1003);
        return -1;
    }

    //初始化进程间数据传输通道
    CS2STunnel::GetInstance()->SetScriptEngine(CStacklessEngine::GetInstance());
    CS2STunnel::GetInstance()->SetRcvSize(CSetting::GetInstance()->GetIPCRecvBufSize());

    //进入deamon模式，写入pid
    if (runat_deamon) {
        CDeamonUtils::Complete(fork_fd, 0);
    }
	CPIDFileUtils::Generate(szPidFilePath.c_str());

    //向路由索引服务器注册自己，并声明自己的职责
    CRoutePeer::GetInstance()->Register();

    DEBUG_STATUS(">>> Service initialized!");
    while(!io_stop)
    {
    	CSetting::GetInstance()->BeginLogicTimeRecord();

    	CS2STunnel::GetInstance()->Lookup();

    	CSetting::GetInstance()->EndLogicTimeRecord("S2STUNNEL");

        //事件引擎调度
        CEventDispatcher::GetInstance()->Lookup();

    	CSetting::GetInstance()->EndLogicTimeRecord("EVENT");

        CStacklessEngine::GetInstance()->push_all_packets();

    	CSetting::GetInstance()->EndLogicTimeRecord("PUSHMESSAGE");
        //stackless的微进程调度
        CStacklessEngine::GetInstance()->Schedule();

    	CSetting::GetInstance()->EndLogicTimeRecord();

        //性能收集器调度
        //CPerformance::GetInstance()->Collect();
    }

    CStacklessEngine::GetInstance()->OnTerminate();

    CRoutePeer::GetInstance()->Disconnect();

    //停止事件引擎
    CEventDispatcher::GetInstance()->Abort();
    DEBUG_STATUS(">>> Service closed!");

    //从pid中除去自己
	CPIDFileUtils::Remove(szPidFilePath.c_str());

//!========================================================
    CPerformance::GetInstance()->Collect();
    return 0;
}

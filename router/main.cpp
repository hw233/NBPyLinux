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

#include "slaveservice.h"

#include <configfile.h>
#include <getopt.h>
#include <deamonutils.h>
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
       Usage: router [Option...] \n\
       Options:\n\
       \n\
       \t -h  --help: Display this usage information.\n\
       \t -d  --deamon: Running on deamon mode\n\
       \t -p  --port: Set port to listen\n\
--------------------------------------------------------------------\n");
};

const struct option __options[] =
        {
                {"help",        0, NULL, 'h'},
                {"port",        1, NULL, 'p'},
                {"deamon",      0, NULL, 'd'},
                {NULL,          0, NULL, 0}
        };

int main(int argc, char** argv) {

    //char performanceFN[255];
    char performanceName[255];
    //sprintf(performanceFN, "router-%d", getpid());
    sprintf(performanceName, "路由索引服务器性能统计(%d)", getpid());
    CPerformance::GetInstance()->SetName("router", performanceName, 200);

    int fork_fd = -1;

    unsigned int nPackMask = 0;
    unsigned short uPort = 0;

    int opt = 0;

    std::string szLogPath = "./";
    std::string szPidFilePath = "./all.pid";
    std::string szRouteAddr = "0.0.0.0";

    while ( (opt = getopt_long(argc, argv, "hp:d", __options, NULL)) != -1 )
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
            case 'p':
            {
                uPort = (unsigned short)atoi(optarg);
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
			if(uPort == 0 || uPort >= 65535)
				uPort = (unsigned short)config.GetIntValue("routeport");

			szRouteAddr     = config.GetStrValue("routeaddr", szRouteAddr.c_str());

			if(!runat_deamon)
				runat_deamon = config.GetIntValue("deamon", 0) != 0;

			szLogPath       = config.GetStrValue("logdir",szLogPath.c_str());
			szPidFilePath   = config.GetStrValue("pidfile",szPidFilePath.c_str());

			nPackMask       = (unsigned int)config.GetIntValue("packet-mask",nPackMask);
        }

    }
    config.Close();

    if(uPort == 0 || uPort >= 65535)
    {
        //端口无效
        printf("Port %d is invalid\n", uPort);
        print_usage();
        CDeamonUtils::Complete(fork_fd, -1001);
        return -1;
    }


    //构建日志文件名前缀
    szLogPath += "router";

    DEBUG_STATUS(">>> The current network packet mask is [0x%x]", nPackMask);
    DEBUG_STATUS(">>> For IPv4 listen on 0.0.0.0:%d ", uPort);

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

    //创建TCP服务
    if(!CSlaveService::GetInstance()->Create(szRouteAddr.c_str(), NULL, uPort))
    {
        CDeamonUtils::Complete(fork_fd, -1004);
        return -1;
    }

    //进入deamon模式，写入pid
    if (runat_deamon) {
        CDeamonUtils::Complete(fork_fd, 0);
        CPIDFileUtils::Generate(szPidFilePath.c_str());
    }

    DEBUG_STATUS(">>> Service initialized!");
    while(!io_stop)
    {
        //事件引擎调度
        CEventDispatcher::GetInstance()->Lookup();
        //性能收集器调度
        CPerformance::GetInstance()->Collect();
    }

	//清除所有客户端连接
	CPeerFactory::GetInstance()->Destroy();
    //停止事件引擎
    CEventDispatcher::GetInstance()->Abort();
    DEBUG_STATUS(">>> Service closed!");

    //从pid中除去自己
    if (runat_deamon) {
        CPIDFileUtils::Remove(szPidFilePath.c_str());
    }

//!========================================================
    CPerformance::GetInstance()->Collect();
    return 0;
}

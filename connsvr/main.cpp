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
#include "clientservice.h"
#include "routepeer.h"
#include "gatetunnel.h"
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
       Usage: connsvr [Option...] \n\
       Options:\n\
       \n\
       \t -h  --help: Display this usage information.\n\
       \t -d  --deamon: deamon mode\n\
       \t -p  --port: Set port to listen\n\
       \t -s  --gatemode: gateway mode\n\
       \t -i  --ipv6: Enable IPv6 support\n\
--------------------------------------------------------------------\n");
};

const struct option __options[] =
        {
                {"help",        0, NULL, 'h'},
                {"port",        1, NULL, 'p'},
                {"gatemode",    0, NULL, 'g'},
                {"ipv6",        0, NULL, 'i'},
                {"deamon",      0, NULL, 'd'},
                {NULL,          0, NULL, 0}
        };

int main(int argc, char** argv) {

    //char performanceFN[255];
    char performanceName[255];
    //sprintf(performanceFN, "connsvr");
    sprintf(performanceName, "连接管理服务器性能统计(%d)", getpid());
    CPerformance::GetInstance()->SetName("connsvr", performanceName, 200);

    int fork_fd = -1;

    unsigned int nPackMask = 0;
    unsigned short uPort = 0;
    unsigned short uRoutePort = 0;
    bool gatemode = false;
    bool ipv6 = false;
    bool json = false;

    int opt = 0;

    std::string szLogPath = "./";
    std::string szSckLogPath = "./";
    std::string szSckPath = "./script/";
    std::string szPidFilePath = "./all.pid";
    std::string szRouteAddr = "127.0.0.1";

    while ( (opt = getopt_long(argc, argv, "hp:gid", __options, NULL)) != -1 )
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
            case 'g':
            {
                gatemode = true;
                break;
            }
            case 'i':
            {
                ipv6 = true;
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
            if (uPort == 0 || uPort >= 65535)
                uPort = (unsigned short) config.GetIntValue("port");

            if (!gatemode)
                gatemode = config.GetIntValue("gatemode", 0) != 0;

            if (!ipv6)
                ipv6 = config.GetIntValue("ipv6", 0) != 0;

            if (!runat_deamon)
                runat_deamon = config.GetIntValue("deamon", 0) != 0;

            json = config.GetIntValue("json", 0) != 0;

            szLogPath = config.GetStrValue("logdir", szLogPath.c_str());
            szPidFilePath = config.GetStrValue("pidfile", szPidFilePath.c_str());
            szSckPath = config.GetStrValue("scriptdir", szSckPath.c_str());

            nPackMask = (unsigned int) config.GetIntValue("packet-mask", nPackMask);

            szRouteAddr = config.GetStrValue("routeaddr", szRouteAddr.c_str());
            uRoutePort = (unsigned short) config.GetIntValue("routeport");

            size_t fre = (size_t) config.GetIntValue("frequency", 0);
            CSetting::GetInstance()->SetFrequency(fre);

            size_t rcvbuf = (size_t) config.GetIntValue("rcvbuf", NBPy_UNIXSCOKET_RECV_SIZE);
            CSetting::GetInstance()->SetIPCRecvBufSize(rcvbuf);

            size_t loginqueueulimit = (size_t) config.GetIntValue("loginqueue-limit", 0);
            CSetting::GetInstance()->SetLoginQueueLimit(loginqueueulimit);

            int loginmsgtp = (int) config.GetIntValue("loginmsg-type", 0);
            CSetting::GetInstance()->SetLoginMsgType(loginmsgtp);
            int loginmsgtag = (int) config.GetIntValue("loginmsg-tag", 0);
            CSetting::GetInstance()->SetLoginMsgTag(loginmsgtag);
            int loginmsgerrcode = (int) config.GetIntValue("login-errcode", 0);
            CSetting::GetInstance()->SetLoginQueueErrCode(loginmsgerrcode);

            DEBUG_STATUS(">>> loginqueue-limit: %d", loginqueueulimit);
            DEBUG_STATUS(">>> loginmsg-type: %d", loginmsgtp);
            DEBUG_STATUS(">>> loginmsg-tag: %d", loginmsgtag);
            DEBUG_STATUS(">>> login-errcode: %d", loginmsgerrcode);

        }
    }
    config.Close();

    CSetting::GetInstance()->SetGatewayMode(gatemode);
    CSetting::GetInstance()->SetJsonSupport(json);
    CSetting::GetInstance()->SetPIDFilePath(szPidFilePath);

    if (!gatemode && (uPort == 0 || uPort >= 65535)){
        printf("Port %d is invalid\n", uPort);
        print_usage();
        CDeamonUtils::Complete(fork_fd, -1001);
        return -1;
    }

    //构建日志文件名前缀
    szLogPath += "connsvr";
    szSckLogPath = szLogPath + "-script";

    DEBUG_STATUS(">>> The current network packet mask is [0x%x]", nPackMask);
    if(!gatemode)
    {
        if(ipv6)
            DEBUG_STATUS(">>> For IPv6 listen on ::%d", uPort);
        DEBUG_STATUS(">>> For IPv4 listen on 0.0.0.0:%d", uPort);
    } else
    {
        DEBUG_STATUS(">>> Working in gateway mode");
    }

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
        if (!fo_script.open(szSckLogPath.c_str())) {
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
    CStacklessEngine::GetInstance()->BindModule(szSckPath.c_str(), "connsvr", false);

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

    //创建TCP服务
    CClientService::GetInstance()->SetScriptEngine(CStacklessEngine::GetInstance());
    if(!CClientService::GetInstance()->Create("0.0.0.0", ipv6 ? "::" : NULL, uPort, 5000))
    {
        CDeamonUtils::Complete(fork_fd, -1004);
        return -1;
    }

    //初始化进程间数据传输通道
    CS2STunnel::GetInstance()->SetScriptEngine(CStacklessEngine::GetInstance());

    CS2STunnel::GetInstance()->SetRcvSize(CSetting::GetInstance()->GetIPCRecvBufSize());

    //初始化进程间句柄传输通道
    CGateTunnel::GetInstance()->SetScriptEngine(CStacklessEngine::GetInstance());

    //进入deamon模式
    if (runat_deamon) {
        CDeamonUtils::Complete(fork_fd, 0);
    }
    //写入pid
	CPIDFileUtils::Generate(szPidFilePath.c_str());

    //向路由索引服务器注册自己，并声明自己的职责
    CRoutePeer::GetInstance()->Register();

    DEBUG_STATUS(">>> Service initialized!");
    while(!io_stop)
    {
    	CS2STunnel::GetInstance()->Lookup();
        //事件引擎调度
        CEventDispatcher::GetInstance()->Lookup();
        //
        CStacklessEngine::GetInstance()->push_all_packets();

        //stackless的微进程调度
        CStacklessEngine::GetInstance()->Schedule();
        //性能收集器调度
        //CPerformance::GetInstance()->Collect();
    }

    CStacklessEngine::GetInstance()->OnTerminate();

	//清除所有客户端连接
	CPeerFactory::GetInstance()->Destroy();

	CS2STunnel::GetInstance()->Exit();

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

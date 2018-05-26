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
 * File: s2stunnel.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef LOGICSVR_S2STUNNEL_H
#define LOGICSVR_S2STUNNEL_H

#include <datatunnel.h>
#include <map>

NS(NBPy)
    class CS2STunnel: public CDataTunnel {
        DECLARE_SINGLETON(CS2STunnel)
    public:
        void Initialize();
        void Destroy();

        bool OnPacketIn(LPPACKET p, const char *s);
        void Inform(pid_t pid);
        void Exit();

        bool SendIMCode(pid_t pid, IIMCode *code);

        void OnInform(CDictPackager *packager);
        void OnExit(CDictPackager *packager);
    private:
        CBuffer m_sendbuffer;
        CBuffer m_InformData;
        char szAddr[255];
        std::map<pid_t,int> m_pids;
        size_t m_total;
    };
NSE

#endif //LOGICSVR_S2STUNNEL_H

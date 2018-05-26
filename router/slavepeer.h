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
 * File: slavepeer.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef ROUTER_SLAVEPEER_H
#define ROUTER_SLAVEPEER_H

#include <peer.h>

NS(NBPy)


    class CSlavePeer: public CPeer{
    THREAD_ALLOCATOR_DEFINE(CSlavePeer)
    public:
        CSlavePeer();
        ~CSlavePeer();

        bool OnClose();
        bool Terminate();
        bool OnReadReady(LPPACKET p);

        void Deliver(IIMCode *code);

    private:
        void OnRegister(CDictPackager *packager);
        void OnUnRegister();

        void OnDeliver(CDictPackager *packeger);

        int m_tags[64];
        pid_t m_pid;
        int m_tp;
    };

NSE

#endif //ROUTER_SLAVEPEER_H

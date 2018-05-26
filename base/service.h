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
 * File: service.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_SERVICE_H
#define BASE_SERVICE_H

#include "peer.h"

NS(NBPy)

    class CService : public IService
    {
    public:
        CService();
        virtual ~CService();
        virtual bool Create(const char *szAddrv4, const char *szAddrv6 = NULL, unsigned int uPort = 0, unsigned int uClientLimit = 1000);
        virtual void Close();
        virtual void OnConnected(LPACCEPT_PEER_INFO info, IPeer *&pClient) {};
        virtual void OnWriteReady(int s) {};
        virtual bool OnReadReady(int){ return false; };
        virtual void OnError(int s) {};

        virtual void OnEventRead(LPEVENT);
        virtual void OnEventWrite(LPEVENT){};
        virtual void OnEventError(LPEVENT);
        virtual void OnEvent(LPEVENT);

        virtual void SetScriptEngine(IScriptEngine *engine) { m_engine = engine; };

    protected:
        int m_raw_sck;
        int m_raw_sckv6;
        IScriptEngine *m_engine;
        unsigned int m_uClientLimit;
    };

NSE

#endif //BASE_SERVICE_H

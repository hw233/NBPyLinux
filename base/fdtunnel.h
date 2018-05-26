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
 * File: fdtunnel.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_FDTUNNEL_H
#define BASE_FDTUNNEL_H

#include "network.h"
#include <sys/un.h>

NS(NBPy)

    class CFDTunnel: public IEventProcessor{
    public:
        CFDTunnel();
        ~CFDTunnel();

        void Bind(const char *);
        void UnBind();

        //!!from IEventProcessor
        void OnEventRead(LPEVENT);
        void OnEventWrite(LPEVENT){};
        void OnEventError(LPEVENT){};
        void OnEvent(LPEVENT){};

        virtual bool OnFDIn(int *, size_t){ return false; };
        bool Send(int, const char *);

    private:
        int m_raw_sck;
    };

NSE

#endif //BASE_FDTUNNEL_H

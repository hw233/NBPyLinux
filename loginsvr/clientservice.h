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
 * File: clientservice.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef CONNSVR_CLIENTSERVICE_H
#define CONNSVR_CLIENTSERVICE_H

#include <service.h>
#include <singleton.h>

NS(NBPy)

    class CClientService: public CService{
        DECLARE_SINGLETON(CClientService)
    public:
        void Initialize();
        void Destroy();

		void LookupClients();

        void OnConnected(LPACCEPT_PEER_INFO info, IPeer *&pClient);

	private:
    };

NSE

#endif //CONNSVR_CLIENTSERVICE_H

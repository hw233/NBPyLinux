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
 * File: signalutils.cpp
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#include "signalutils.h"

NS(NBPy)

    signal_func *CSignalUtils::handler(int signo, signal_func *func)
    {
        struct sigaction act, oact;

        act.sa_handler = func;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
            act.sa_flags |= SA_INTERRUPT;	/* SunOS 4.x */
#endif
        } else {
#ifdef SA_RESTART
            act.sa_flags |= SA_RESTART;	/* SVR4, 4.4BSD */
#endif
        }

        if (sigaction(signo, &act, &oact) < 0)
            return SIG_ERR;

        return oact.sa_handler;
    }

NSE
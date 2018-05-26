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
 * File: protocol.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_PROTOCOL_H
#define BASE_PROTOCOL_H

#include "base.h"

NS(NBPy)

typedef int TAG_TYPE;

enum enumServerType
{
    ST_ROUTER   = 1,
    ST_LOGIC    = 2,
    ST_CONN     = 3,
};

enum enumPacketSchemes
{
    PS_C2S 	= 1,
    PS_S2C 	= 2,
    PS_S2S 	= 3,
    PS_S2R 	= 4,
    PS_R2S 	= 5,
};

enum enumPacketTypes
{
    PT_SLAVE_REG 	= 0x10,
    PT_SLAVE_DEL 	= 0x11,
    PT_HEART		= 0x12,
    PT_SLAVE_INFORM = 0x13,
    PT_SLAVE_EXIT	= 0x14,
    PT_DELIVER      = 0x15,

    PT_RESULT 		= 0xFFFF,
};

enum enumPacketKeys
{
    PK_ERROR    = 0x0A,
    PK_SCHEME	= 0x0B,
    PK_SID		= 0x0C,
    PK_SOCKET	= 0x0D,
    PK_PID 		= 0x0E,
    PK_FD 		= 0x0F,
    PK_TYPE 	= 0x10,
    PK_TO		= 0x11,
    PK_FROM 	= 0x12,
    PK_BODY 	= 0x14,
    PK_TAGS 	= 0x15,
    PK_TAG  	= 0x16,
    PK_SRV_TYPE = 0x17,
    PK_IPC 		= 0x18,
    PK_IPCADDR	= 0x19,
    PK_REQUEST  = 0x20,
    PK_CHID     = 0x21,
    PK_FROM_TAG = 0x22,
    PK_REQID    = 0x23,
    PK_RID      = 0x24,
    PK_CID      = 0x25,
    PK_TT       = 0x26,
    PK_ROLEID   = 0x27,
    PK_TO_PID   = 0x28
};

enum enumTags
{
    TAG_NONE	= 0,
    TAG_ROUTER 	= 0x01,
    TAG_CONN	= 0x02,
    TAG_STATUS	= 0x03,
};


#pragma pack(1)
typedef struct __SID
{
    int cid;
    int sid;
}SID, *LPSID;
#pragma pack()

NSE


#endif // BASE_PROTOCOL_H

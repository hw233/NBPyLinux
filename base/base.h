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
 * File: base.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_BASE_H
#define BASE_BASE_H

//操作系统类型
#define PLATFORM_OTHER      -1
#define PLATFORM_LINUX      0
#define PLATFORM_MACOS      1
#define PLATFORM_WINDOWS    2

//当前操作系统
#ifndef TARGET_PLATFORM
#define TARGET_PLATFORM PLATFORM_LINUX
#endif


#if(TARGET_PLATFORM == PLATFORM_LINUX)
__asm__(".symver memcpy,memcpy@GLIBC_2.14");
#endif


#include <map>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


#if(TARGET_PLATFORM == PLATFORM_LINUX)
#include <sys/epoll.h>
#include <stddef.h>
#endif

#if(TARGET_PLATFORM == PLATFORM_MACOS)
#include <sys/event.h>
#endif

//#define NBPy_THREAD_MODE

#ifndef NBPy_INVALID_SOCKET
#define NBPy_INVALID_SOCKET  (int)(~0)
#endif // NBPy_INVALID_SOCKET

#define NBPy_INFINITE 0xFFFFFFFF

#define NBPy_POOL_SHRINK_TIME_MS 	300000              /// memorypool shrink time

#define NBPy_RECV_BLOCK_SIZE 	    512                 /// IO recv buffer default size
#define NBPy_FRAME_TIME 		    30                  /// IO work frame interval
#define NBPy_UNIXSCOKET_RECV_SIZE 	(10 * 1024)                 /// IO recv buffer default size
#define NBPy_DATATUNNEL_ADDRPREFIX	"./sockets/s2s_%d.sock"
#define NBPy_DATATUNNEL_ADDRPREFIX2	"./sockets/s2s_%d_%ld.sock"
#define NBPy_FDTUNNEL_ADDRPREFIX	"./sockets/fd_%d.sock"

#ifndef MAX
#define MAX(a, b) ((a > b) ? a : b)
#endif

#ifndef MIN
#define MIN(a, b) ((a < b) ? a : b)
#endif

#define NBPy_DEBUG_LEVEL_STATUS       1
#define NBPy_DEBUG_LEVEL_WARN         2
#define NBPy_DEBUG_LEVEL_ERROR        4

///Debug infomation level
#define NBPy_DEBUG_LEVEL              (NBPy_DEBUG_LEVEL_STATUS | NBPy_DEBUG_LEVEL_WARN | NBPy_DEBUG_LEVEL_ERROR)

#define NBPy_VALIDATE_MASK( val, mask ) \
	((val & mask) == mask)

///Using stlport
#include <unordered_map>
#ifdef _STLP_UNORDERED_MAP
#define NBPy_UNORDERED_MAP std::tr1::unordered_map
#else
#define NBPy_UNORDERED_MAP std::unordered_map
#endif

#ifndef NS
#define NS(ns) \
namespace NS_##ns{\

#endif

#ifndef NSE
#define NSE \
}
#endif

#ifndef USN
#define USN(ns) \
using namespace NS_##ns;
#endif

NS(NBPy)

NSE

extern uint32_t crc(const void *,size_t);

#endif //BASE_BASE_H

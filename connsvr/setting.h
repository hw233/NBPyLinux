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
 * File: setting.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef CONNSVR_SETTING_H
#define CONNSVR_SETTING_H

#include <singleton.h>
#include <performance.h>

#define MAX_LOGIN_QUEUE 5

NS(NBPy)

    typedef std::map<int, std::map<pid_t, int> > TAGPIDS;

    class CSetting: public IPerformanceCollector{
        DECLARE_SINGLETON(CSetting)
    public:
        void Initialize(){
            m_isGatewayMode = false;
            m_isJsonSupport = false;
            m_pidfilepath = "";
            m_frequency = 0;
			m_limit_loginqueue = MAX_LOGIN_QUEUE;
			m_current_loginqueue = 0;

			m_login_msg_tp = 0;
			m_loginqueue_errcode = 0;
			m_login_msg_tag = 0;

            CPerformance::GetInstance()->AddCollector(this);
        }

        void Destroy(){

        }

        void SetGatewayMode(bool r){ m_isGatewayMode = r; }
        void SetJsonSupport(bool r){ m_isJsonSupport = r; }
        void SetFrequency(size_t f){ m_frequency = f; }
        void SetIPCRecvBufSize(size_t s){ m_ipc_rcv_size = s; }
        void SetPIDFilePath(std::string &r){ m_pidfilepath = r; }
        bool IsGetwayMode(){ return m_isGatewayMode; }
        bool IsJsonSupport(){ return m_isJsonSupport; }
		size_t GetFrequency(){ return m_frequency; }
		size_t GetIPCRecvBufSize(){ return m_ipc_rcv_size; }

        std::string &GetPIDFilePath(){ return m_pidfilepath; }

        void AddTagPid(int tag, pid_t pid){
            TAGPIDS::iterator tagpid = m_tagpids.find(tag);
            if(tagpid == m_tagpids.end())
            {
                m_tagpids[tag].clear();
                tagpid = m_tagpids.find(tag);
            }
            std::map<pid_t, int> &km = tagpid->second;
            km[pid] = tag;
        }

        TAGPIDS &GetTagPids(){
            return m_tagpids;
        }

        void RemoveTagPid(int tag, pid_t pid){
            TAGPIDS::iterator tagpid = m_tagpids.find(tag);
            if(tagpid == m_tagpids.end())
                return;
            std::map<pid_t, int> &km = tagpid->second;
            km.erase(pid);
        }

		void Collect(CPerformancePackager& );

        void SetLoginQueueLimit(unsigned int limit){
			m_limit_loginqueue = limit;
        }

        bool incLoginQueue(){
			if(m_current_loginqueue >= m_limit_loginqueue){
				return false;
			}
			m_current_loginqueue ++;
			return true;
        };
        void decLoginQueue(){ if(m_current_loginqueue > 0) m_current_loginqueue --; };

        int getLoginMsgType(){ return m_login_msg_tp; }
        int getLoginQueueErrCode(){ return m_loginqueue_errcode; }
        int getLoginMsgTag(){ return m_login_msg_tag; }

        void SetLoginMsgType(int v){
        	m_login_msg_tp = v;
        }
        void SetLoginMsgTag(int v){
        	m_login_msg_tag = v;
        }
        void SetLoginQueueErrCode(int v){
        	m_loginqueue_errcode = v;
        }

    private:
        bool m_isGatewayMode;
        bool m_isJsonSupport;
        size_t m_frequency;
        size_t m_ipc_rcv_size;
        std::string m_pidfilepath;
		unsigned int m_current_loginqueue;
		unsigned int m_limit_loginqueue;

		int m_login_msg_tp;
		int m_loginqueue_errcode;
		int m_login_msg_tag;

        TAGPIDS m_tagpids;
    };
NSE

#endif //CONNSVR_SETTING_H

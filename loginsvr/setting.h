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

#ifndef LOGICSVR_SETTING_H
#define LOGICSVR_SETTING_H

#include <singleton.h>
#include <performance.h>

NS(NBPy)

    typedef std::map<int, std::map<pid_t, int> > TAGPIDS;

    class CSetting : public IPerformanceCollector{
        DECLARE_SINGLETON(CSetting)
    public:
        void Initialize(){
            m_pidfilepath = "";
            m_total_logic_times = 0;
            m_total_logic_agv = 0;
            m_current_logic_time = 0;
            CPerformance::GetInstance()->AddCollector(this);
        }

        void Destroy(){

        }

        void SetFrequency(size_t f){ m_frequency = f; }
		size_t GetFrequency(){ return m_frequency; }

        void SetPIDFilePath(std::string &r){ m_pidfilepath = r; }
        std::string &GetPIDFilePath(){ return m_pidfilepath; }

        bool AddTagPid(int tag, pid_t pid){
            TAGPIDS::iterator tagpid = m_tagpids.find(tag);
            if(tagpid == m_tagpids.end())
            {
                m_tagpids[tag].clear();
                tagpid = m_tagpids.find(tag);
            }
            std::map<pid_t, int> &km = tagpid->second;
            if(km.find(pid) != km.end())
				return false;
            km[pid] = tag;
            return true;
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

        void SetIPCRecvBufSize(size_t s){ m_ipc_rcv_size = s; }
		size_t GetIPCRecvBufSize(){ return m_ipc_rcv_size; }

		void BeginLogicTimeRecord(){
			m_current_logic_time = CDateTime::timestamp();
			DEBUG_STATUS(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>BeginLogicTimeRecord");
		}

		void EndLogicTimeRecord(const char *t = NULL){
			unsigned long st = (CDateTime::timestamp() - m_current_logic_time);
			if(t == NULL){
				m_total_logic_times ++;
				if(st > m_total_logic_agv)
					m_total_logic_agv = st;
				DEBUG_STATUS(">>>>>>>>>[END]>>>>>>>>>>>: %ld / %ld", st, m_total_logic_times);
			}else
				DEBUG_STATUS(">>>>>>>>>%s>>>>>>>>>>>: %ld / %ld", t, st, m_total_logic_times);
		}

		void Collect(CPerformancePackager& );

    private:
        std::string m_pidfilepath;
        size_t m_ipc_rcv_size;

        unsigned long m_total_logic_times;
        unsigned long m_total_logic_agv;

        unsigned long m_current_logic_time;
        size_t m_frequency;

        TAGPIDS m_tagpids;
    };
NSE

#endif //LOGICSVR_SETTING_H

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

NS(NBPy)

    typedef std::map<int, std::map<pid_t, int> > TAGPIDS;

    class CSetting{
        DECLARE_SINGLETON(CSetting)
    public:
        void Initialize(){
            m_isGatewayMode = false;
            m_isJsonSupport = false;
            m_pidfilepath = "";
            m_frequency = 0;
        }

        void Destroy(){

        }

        void SetGatewayMode(bool r){ m_isGatewayMode = r; }
        void SetJsonSupport(bool r){ m_isJsonSupport = r; }
        void SetFrequency(size_t f){ m_frequency = f; }
        void SetPIDFilePath(std::string &r){ m_pidfilepath = r; }
        bool IsGetwayMode(){ return m_isGatewayMode; }
        bool IsJsonSupport(){ return m_isJsonSupport; }
		size_t GetFrequency(){ return m_frequency; }

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

    private:
        bool m_isGatewayMode;
        bool m_isJsonSupport;
        size_t m_frequency;
        std::string m_pidfilepath;

        TAGPIDS m_tagpids;
    };
NSE

#endif //CONNSVR_SETTING_H

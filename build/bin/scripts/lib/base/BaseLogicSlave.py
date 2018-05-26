#coding:utf-8

from Slave                      import Slave ,S2S,C2S,R2S,_s2sHandler_,_c2sHandler_
from protocol                   import *
from SessionID                  import SessionID
from Processor                  import Processor
from BaseLogicServer            import BaseLogicServer
from BaseS2SCaller              import BaseS2SCaller

class BaseSlaveConfig(object):
    def __init__(self):
        self.tags       = None
        self.pack       = None
        self.protocols  = None
        self.need_body  = False
        self.logic_srv  = BaseLogicServer
        self.s2s_caller = BaseS2SCaller
        self.s2s_pts    = {}

class BaseLogicSlave(Slave):
    def __init__(self, slave_config):
        self._tags = [TagType.TAG_STATUS]
        for tag in slave_config.tags:
            self._tags.append(tag)

        super(BaseLogicSlave,self).__init__(*self._tags)


        self.PackClass  = slave_config.pack
        self.__Srv      = slave_config.logic_srv.getInstance()
        self.__Srv._initialize(self, slave_config)

        if slave_config.protocols is None:
            return

        for tag in slave_config.tags:
            curr_protocol = slave_config.protocols.get(tag)
            if curr_protocol is None:
                continue
            pt_keys = curr_protocol.__class__.__dict__.keys()
            for key in pt_keys:
                if (len(key) <= 7):     continue
                if key == '__dict__':   continue

                pt_val = curr_protocol.__getattribute__(key)
                is_s2s_mode = False

                if slave_config.s2s_pts.has_key(pt_val):
                    is_s2s_mode = True
                else:
                    is_s2s_mode = (key[0:7] == 'PT_S2S_')

                if is_s2s_mode:
                    print '[S2S] register key %s = %d'%(key, pt_val)
                    def __func_s2s(self,pack):
                        ret,data = self.__Srv.S2SProcessor(pt_val, pack)
                        return tag,ret,data
                    S2S(pt_val)(__func_s2s)
                    pass
                elif (key[0:3] == 'PT_'):
                    print '[C2S] register key %s = %d'%(key, pt_val)
                    def __func(self, pack):
                        ret, data = self.__Srv.C2SProcessor(pt_val, pack)
                        return ret, data
                    C2S(tag, pt_val)(__func)

    def login(self, sid, attr):
        self.__Srv.roleEnter(sid, attr)

    def update(self, sid, attr):
        self.__Srv.roleUpdate(sid, attr)
    
    def logout(self, sid):
        self.__Srv.roleLeave(sid)


#coding:utf-8
import Singleton
import time
import datetime

from protocol               import *
from Processor              import *
from Slave                  import unmask_s2s_tp

from SessionID              import SessionID

from BaseS2SCaller          import BaseS2SCaller

class BaseLogicServer(Singleton.Singleton):
    def __init__(self):
        self._slave = None

    def _initialize(self, slave, slave_config):
        assert( slave )
        self._slave = slave
        self.__need_body    = slave_config.need_body
        self.Send           = self._slave.Send
        self.Multicast      = self._slave.Multicast
        self.Broadcast      = self._slave.Broadcast
        self.SetSlaveMsg    = self._slave.SetSlaveMsg
        self.GetSlaveMsg    = self._slave.GetSlaveMsg
        self.__s2s_caller   = slave_config.s2s_caller.getInstance()
        #old
        self.__s2s_caller._initialize(self)
        #add by sjx
        #self.__s2s_caller._initialize(slave)
        self.pack           = slave_config.pack
        self.tags           = slave_config.tags
        pass

    @staticmethod
    def getInstance():  return Singleton.getInstance(BaseLogicServer)

    #发送至指定ssid的客户端
    def sendToClientBySSID(self,ssid,body):
        realPack = self.pack(body)
        realPack.TAG = self.tags[0]
        pack = GeneralPack()
        pack.Fill( TO=ssid.Data,SCHEMA=PacketSchema.PS_S2C,BODY=realPack.data )
        self.Send(pack.data,ssid)

    #发送至指定sid的客户端
    def sendToClient(self,sid,body):
        ssid = SessionID(sid)
        self.sendToClientBySSID(ssid,body)

    #角色登入
    def roleEnter(self,ssid, data):
        #处理器分派
        Processor.dispatch_proc('enter',ssid=ssid, attr=data)
        pass

    #角色更新处理
    def roleUpdate(self, ssid, data):
        #处理器分派
        Processor.dispatch_proc('update',ssid=ssid, attr=data)
        pass

    #角色离线
    def roleLeave(self,ssid):
        #处理器分派
        Processor.dispatch_proc('leave',ssid=ssid)
        pass

    def C2SProcessor(self,tp,pack):
        '''处理c2s消息管线'''
        tp = pack.TYPE
        sid,reqid,pbody,device = pack.Fetch('SID','REQID', 'BODY', 'DEVICE')
        if (pbody is None) and (self.__need_body):
            print 'BODY None'
            return ErrorCode.ERR_NORETURN,None

        pbody = self.pack(pbody)

        #get source role
        if sid is None:
            print 'sid None'
            return ErrorCode.ERR_NORETURN,None

        ssid = SessionID(sid)
        if pbody is None:   pbody = {}

        ret, data = Processor.dispatch_func_ret(tp, ssid=ssid, param=pbody, device=device)
        if ret is None: ret = ErrorCode.ERR_NORETURN
        return ret,data

    def S2SProcessor(self,tp,pack):
        '''处理s2s消息管线'''
        stp = unmask_s2s_tp(pack.TYPE)
        ret,data = Processor.dispatch_func_ret(stp, pack=pack, ACTION=HANDLER_TYPE_S2S)
        if ret is None: return ErrorCode.ERR_NO_SERVICE, None
        return ret,data

    #请求同步先前登入的玩家角色
    def requestAsync(self):
        pass

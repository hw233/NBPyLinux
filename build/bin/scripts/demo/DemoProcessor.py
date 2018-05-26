#coding:utf-8
from time                   import time
from datetime               import datetime

from Processor              import *

from DemoProtocol           import *

'''此处使用逻辑处理器类的静态方法wrap指定下面的代码解释过程中将会对指定类名的类的成员方法进行装饰'''
Processor.wrap('DemoProcessor')

'''你的逻辑处理器的实现类必须继承自系统逻辑处理器类'''
class DemoProcessor(Processor):
    def __init__(self, slave_cfg):    
        '''__init__内首先必须调用继承树的上层类的__init__'''
        super(DemoProcessor, self).__init__()
        '''slave_cfg.logic_srv.getInstance()是一个BaseLogicServer的实例，提供了你向客户端发送数据的接口'''
        self.__Srv = slave_cfg.logic_srv.getInstance()
        '''slave_cfg.logic_srv.getInstance()是一个BaseS2SCaller的实例，提供了你向其他逻辑服节点发送数据的接口'''
        self.__s2s = slave_cfg.s2s_caller.getInstance()
        pass

    '''这是逻辑处理器类的一种装饰器，注意它的参数是字符串，只接受enter或leave，分别代表用户通过验证连接上来时或者断开连接离开时的状态通知处理方法'''
    @Processor.register('enter')
    def __enter(self, ssid, attr=None):
        print '%s 进入游戏' % ssid
        pass

    @Processor.register('leave')
    def __leave(self, ssid, attr=None):
        print '%s 离开游戏' % ssid
        pass

    '''这是逻辑处理器类的C2S消息处理装饰器，只接受一个参数，就是消息类型值'''
    @Processor.register(DemoType.PT_DEMO_ECHO)
    def __func_echo(self, ssid, param, **other):
        print 'receive packet >>>', param
        return DemoError.ERR_OK, param
        
    '''这是逻辑处理器类的S2S消息处理装饰器'''
    @Processor.register(DemoType.PT_S2S_DEMO_ECHO, HANDLER_TYPE_S2S)
    def __func_echo2(self, pack, **other):
        print 'receive [s2s] packet >>>', pack
        return DemoError.ERR_OK, {222:'fuck you!'}
        

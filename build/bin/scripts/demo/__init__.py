#coding: utf-8

import DBHelper
DBHelper.DBHelper.getInstance()._initialize()

import kernel.timer

def ontimer(dt, a,b,c):
	print 'ontimer', dt, a, b, c

print 'timer >>', kernel.timer.CreateTimer(3000, ontimer, kernel.timer.TIMER_TP_LOOP, (11,22,'bb'))

#Import from lib
from protocol                   import *
from DemoProtocol               import *   #如果你有自己的Protocol，则在此导入它即可

'''导入创建逻辑服节点需要用到的类'''
from base.BaseLogicSlave        import BaseLogicSlave, BaseSlaveConfig

#Import all processor
'''导入你的所有处理器类'''
from DemoProcessor              import DemoProcessor

'''每一个逻辑服都必须通过下面的代码初始化一个逻辑服节点的配置'''
#Slave configuration
'''新建一个逻辑服节点配置对象'''
slave_config = BaseSlaveConfig()
'''指定该节点响应的tag(列表，可以是多个)'''
slave_config.tags       = [TagType.TAG_DEMO]
'''指定该节点收到的包都应该以哪种包结构封装，如果没指定，则默认为GeneralPack'''
slave_config.pack       = DemoPack
'''绑定预设要由该节点处理的tag和消息类型类'''
slave_config.protocols  = {
    TagType.TAG_DEMO: DemoType
}

#Template data load
#CSV或其他静态配置数据的加载

#Create all processor
'''实例化你的所有处理器类，参数为上面配置的逻辑服节点配置对象的实例'''
DemoProcessor(slave_config)

#Create slave
'''实例化逻辑服节点对象，进入底层主循环处理，参数为上面配置的逻辑服节点配置对象的实例'''
__logic_slave   = BaseLogicSlave(slave_config)
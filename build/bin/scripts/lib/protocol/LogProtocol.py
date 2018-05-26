#coding: utf-8

from protocol.CommonProtocol import PacketType, PacketType, PacketSchema, GeneralPack

class __LogType(object):
    
    PT_S2S_ROLE_LOG_INFO  = 0x40050      #资源记录

    def __setattr__(self,attr,val): 
        raise TypeError("Attribute is readonly")
        pass

LogType = __LogType()


class __LogKey(object):
    
    PK_ROLE_ID              = 0x450         #角色ID
    PK_ROLE_SSID            = 0x451         #角色SSID
    PK_ROLE_TOKEN           = 0x452         #角色TOKEN
    PK_ROLE_NAME            = 0x453         #角色名字   

    PK_LOG_TYPE             = 0x454         #日志类型
    PK_LOG_NAME             = 0x455         #行为名称
    PK_LOG_SYSTEM           = 0x456         #日志对应的系统模块

    PK_LOG_CURGOLD          = 0x457         #当前元宝
    PK_LOG_CURCOIN          = 0x458         #当前铜币
    PK_LOG_CURIRON          = 0x459         #当前铁矿
    PK_LOG_CURFOOD          = 0x460         #当前粮食
    PK_LOG_CURWOOD          = 0x461         #当前木材
    PK_LOG_CURFAME          = 0x462         #当前声望
    PK_LOG_GOLD             = 0x463         #增加或减少的元宝
    PK_LOG_COIN             = 0x464         #增加或减少的铜币
    PK_LOG_IRON             = 0x465         #增加或减少的铁矿
    PK_LOG_FOOD             = 0x466         #增加或减少的粮食
    PK_LOG_WOOD             = 0x467         #增加或减少的木材
    PK_LOG_FAME             = 0x468         #增加或减少的声望
    PK_LOG_TIME             = 0x469         #当前日志时间

    PK_LOG_ADDITEMS         = 0x470         #增加的物品数量
    PK_LOG_DELITEMS         = 0x471         #减少的物品数量
    PK_LOG_INFO             = 0x472         #玩家行为信息
    PK_LOG_ADDHEROS         = 0x473         #增加的武将

    
    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

LogKey = __LogKey()

class __LogName(object):

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

LogName = __LogName()

#Error codes
class __LogError(object):

    __slots__ = ()

    ERR_LOG_OK                                 = 0                     #正常
    ERR_LOG_INVALID_PARAM                      = -1101                 #参数错误

LogError = __LogError()

class LogPack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = LogKey
    def __init__(self, data=None):
        super(LogPack, self).__init__(data)
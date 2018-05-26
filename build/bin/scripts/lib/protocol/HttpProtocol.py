#coding:utf-8
from CommonProtocol import PacketType, PacketType, PacketSchema, GeneralPack

class __HttpType(object):
    PT_HTTP_UPDATE_HEAD                 = 0x60020    #更新头像
    PT_HTTP_UPDATE_ROLE                 = 0x60021    #更新角色其他信息
    PT_HTTP_UPDATE_PAY                  = 0x60022    #更新角色充值
    PT_UPDATE_MAIL_INFO                 = 0x60023    #更新邮件
    PT_HTTP_BAN_CHAT                    = 0x60024    #禁言
    PT_HTTP_BAN_USER                    = 0x60025    #封号
    PT_HTTP_BAN_IMSI                    = 0x60026    #封IMSI
    PT_HTTP_USER_OFF                    = 0x60027    #踢下线
    PT_HTTP_UPDATE_BORN                 = 0x60028    #更新出生州信息
    PT_HTTP_RECHARGE                    = 0x60029    #http充值
    
    def __setattr__(self,attr,val):
        raise TypeError("Attribute is readonly")

HttpType = __HttpType()


class __HttpKey(object):
    PK_HTTP_ID       = 0x630            #httpID
    PK_ROLE_ID       = 0x631            #角色uid
    PK_ROLE_SSID     = 0x632            #角色ssid
    PK_ROLE_TOKEN    = 0x633            #角色TOKEN
    PK_HTTP_TYPE     = 0x634            #类型

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

HttpKey = __HttpKey()


#Error codes
class __HttpError(object):
    __slots__ = ()
    ERR_OK            = 0     #正常
    ERR_OFFLINING     = -1001 #不在线
    ERR_INVALID_PARAM = -1002 #参数错误
    pass

HttpError = __HttpError()


class HttpPack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = HttpKey
    def __init__(self, data=None):
        super(HttpPack, self).__init__(data)

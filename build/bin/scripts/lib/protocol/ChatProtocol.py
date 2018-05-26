#coding:utf-8
from CommonProtocol import PacketType, PacketType, PacketSchema, GeneralPack

class __ChatType(object):
    PT_C2S_CHAT                 = 0x60001     #客户端聊天
    PT_S2S_CHAT                 = 0x60002     #发送的聊天
    PT_S2C_CHAT                 = 0x60003     #系统发送的聊天
    PT_S2S_UPDATE_ALIANCED      = 0x60004     #更新角色同盟信息
    PT_S2S_GAG_CHAT             = 0x60005     #禁言通知
    PT_S2S_NOTICE               = 0x60006     #系统公告跑马灯
    PT_S2S_HOUTAI_NOTICE        = 0x60007     #后台系统发送公告

    def __setattr__(self,attr,val):
        raise TypeError("Attribute is readonly")

ChatType = __ChatType()


class __ChatKey(object):
    PK_ROLE_ID              = 0x600     #角色ID
    PK_ROLE_SSID            = 0x601     #角色SSID
    PK_ROLE_NICKNAME        = 0x602     #昵称
    PK_ROLE_TOKEN           = 0x603     #角色TOKEN
    PK_ROLE_VIP             = 0x604     #VIP等级
    PK_ROLE_HEADPIC         = 0x605     #头像
    PK_CHAT_CHANNEL         = 0x606     #聊天类型(0=世界频道,1=同盟频道,2=系统频道) 
    PK_CHAT_CONTENT         = 0x607     #聊天内容
    PK_CHAT_TIME            = 0x608     #聊天时间
    PK_CHAT_TYPE            = 0x609     #0=文本类型,1=语音类型
    PK_CHAT_NOTICE          = 0x610     #0=不在跑马灯中显示,1=在跑马灯中显示
    PK_ALIANCE_ID           = 0x611     #同盟ID
    PK_CHAT_ISSYSTEM        = 0x612     #是否是系统
    PK_CONTENT_ID           = 0x613     #公告
    PK_ROLE_VIP_ENABLE      = 0x614     #VIP激活状态

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

ChatKey = __ChatKey()

#Error codes
class __ChatError(object):
    __slots__ = ()
    ERR_OK                                     = 0                   #正常
    ERR_GM_COMMONT_NOEXIST                     = -601                #GM命令不存在
    ERR_GM_FORMAT_ILLEGAL                      = -602                #GM命令格式错误
    ERR_CHAT_NOT_ALINACE                       = -603                #角色没有同盟
    ERR_CHAT_IS_GAG                            = -604                #角色在禁言中
    pass

ChatError = __ChatError()


class ChatPack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = ChatKey
    def __init__(self, data=None):
        super(ChatPack, self).__init__(data)

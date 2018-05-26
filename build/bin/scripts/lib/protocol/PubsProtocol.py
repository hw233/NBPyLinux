#coding: utf-8

from protocol.CommonProtocol import PacketType, PacketType, PacketSchema, GeneralPack

class __PubsType(object):

    PT_C2S_PUBS_INFO     = 0x50001 # 获取/刷新 酒馆信息
    PT_C2S_GET_HERO      = 0x50002 # 抽将
    PT_S2C_ZAHOMU        = 0x50003 # 可招募
    PT_C2S_GET_CARD_TIME = 0x50004 # 获取特殊卡包剩余时间
    PT_S2S_OPEN_CARD_PACK= 0x50005 #开启卡包S2S
    PT_S2C_GET_CARD_LEVEL= 0x50006 #招武将的等级

    def __setattr__(self,attr,val):
        raise TypeError("Attribute is readonly")
        pass

PubsType = __PubsType()


class __PubsKey(object):
    PK_ROLE_ID            = 0x501 #角色ID
    PK_ROLE_SSID          = 0x502 #角色SSID
    PK_ROLE_TOKEN         = 0x503 #角色TOKEN
    PK_ROLE_NAME          = 0x504 #角色名字

    PK_PUBS_TYPE          = 0x505 #抽武将类型
    PK_GOLD_TIME          = 0x506 #元宝抽次数
    PK_GOLD_SY            = 0x507 #元宝抽剩余秒数
    PK_WEI_TIME           = 0x508 #魏国武将今天剩余次数
    PK_WU_TIME            = 0x509 #吴国武将今天剩余次数
    PK_SHU_TIME           = 0x510 #蜀国武将今天剩余次数
    PK_PUBS_TIMES         = 0x511 #抽将次数 0 一次 1 多次
    PK_PUBS_HEROLIST      = 0x512 #获取英雄的列表
    PK_PUBS_PACK          = 0x513 #基础信息包
    PK_OPEN_CARD_TYPE     = 0x514 #开启卡包类型
    PK_OPEN_CARD_LV       = 0x515 #开启卡包等级

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

PubsKey = __PubsKey()

#Error codes
class __PubsError(object):

    __slots__ = ()

    ERR_OK              = 0                     #正常
    ERR_INVALID_PARAM   = -5001 #参数错误
    ERR_NO_ENOUGH_GOLD  = -5002 #元宝不足
    ERR_NO_ENOUGH_COIN  = -5003 #铜币不足
    ERR_NO_ENOUGH_FRAME = -5004 #声望不足
    ERR_NO_ENOUGH_TIMES = -5005 #当天次数用完
    ERR_SET_GUO_ERR     = -5006
    ERR_OPEN_CARD_NONE  = -5007 #开启卡包错误
    ERR_CARD_IS_OPEN    = -5008 #该卡包已经开启

PubsError = __PubsError()

class PubsPack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = PubsKey
    def __init__(self, data=None):
        super(PubsPack, self).__init__(data)

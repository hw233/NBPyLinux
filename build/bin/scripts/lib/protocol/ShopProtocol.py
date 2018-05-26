#coding: utf-8

from protocol.CommonProtocol import PacketType, PacketType, PacketSchema, GeneralPack

class __ShopType(object):
    PT_S2C_SEND_SHOP_DATA      = 0x150001    #s2c推送商店信息（已经买过的物品信息）
    PT_C2S_GET_SHOP_DATA       = 0x150002    #c2s获取商店信息（已经买过的物品信息）
    PT_C2S_BUY_SHOP_ITEM       = 0x150003    #c2s购买物品


    def __setattr__(self,attr,val):
        raise TypeError("Attribute is readonly")
        pass

ShopType = __ShopType()


class __ShopKey(object):
    PK_ROLE_ID            = 0x1501 #角色ID
    PK_ROLE_SSID          = 0x1502 #角色SSID
    PK_ROLE_TOKEN         = 0x1503 #角色TOKEN
    PK_ROLE_NAME          = 0x1504 #角色名字
    #
    PK_SHOP_ID            = 0x1505 #商品sql,id
    PK_SHOP_IID           = 0x1506 #商品csv,id
    PK_SHOP_TID           = 0x1507 #商品temp,id
    PK_SHOP_NUM           = 0x1508 #商品数量
    PK_SHOP_STATUS        = 0x1509 #商品状态
    PK_SHOP_CREATTIME     = 0x1510 #购买商品时间戳
    PK_SHOP_DATE          = 0x1511 #购买商品的日期(eg:2016-11-21)
    PK_SHOP_BUY_PACK      = 0x1512

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

ShopKey = __ShopKey()

#Error codes
class __ShopError(object):

    __slots__ = ()

    ERR_OK                     = 0      #正常
    ERR_OVER_LIMIT             = -15001 # 超出每日购买上限
    ERR_ALIENCE_TEC_LV_LOW     = -15002 # 同盟科技等级不足



ShopError = __ShopError()

class ShopPack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = ShopKey
    def __init__(self, data=None):
        super(ShopPack, self).__init__(data)

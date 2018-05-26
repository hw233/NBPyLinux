#coding: utf-8

from protocol.CommonProtocol import PacketType, PacketType, PacketSchema, GeneralPack

class __BagType(object):

    PT_C2S_BAG_LIST         = 0x40001 # 背包所有物品
    PT_C2S_BAG_SELL         = 0x40002 # 出售物品
    PT_C2S_BAG_USED         = 0x40003 # 使用物品
    PT_S2C_BAG_NEW          = 0x40004 # 获得物品
    PT_C2S_BAG_COMPOUND     = 0x40005 # 合成物品
    PT_C2S_BAG_RESOLVE      = 0x40006 # 分解物品
    PT_S2S_BAG_ADD_DEL      = 0x40007 # s2s增加删除物品
    PT_C2S_BAG_OPEN         = 0x40008 # 打开背包
    PT_S2C_BAG_ADD_DEL      = 0x40009 # s2c增加删除物品
    PT_C2S_EQUIP_OPERATE    = 0x40010 # 穿戴装备/脱装备/替换装备
    PT_C2S_STONE_OPERATE    = 0x40011 # 镶嵌宝石/卸载宝石/替换宝石
    PT_S2S_DESCOMPSE_HERO   = 0x40012 # 分解武将
    PT_S2C_UPDATE_ITEM      = 0x40013 # 更新物品
    PT_C2S_GET_HERO_EQUIP   = 0x40014 # 获取武将装备
    PT_S2S_CHECK_SIZE       = 0x40015 # 检查是否足够空间

    def __setattr__(self,attr,val):
        raise TypeError("Attribute is readonly")
        pass

BagType = __BagType()


class __BagKey(object):
    PK_ROLE_ID              = 0x401 #角色ID
    PK_ROLE_SSID            = 0x402 #角色SSID
    PK_ROLE_TOKEN           = 0x403 #角色TOKEN
    PK_ROLE_NAME            = 0x404 #角色名字

    PK_ITEM_ID              = 0x405 #物品ID
    PK_ITEM_PID             = 0x406 #物品模版ID
    PK_ITEM_NUM             = 0x407 #物品数量
    PK_ITEM_TYPE            = 0x408 #物品类型
    PK_ITEM_ADDITEMS        = 0x409 #增加物品字典
    PK_ITEM_DELITEMS        = 0x40a #删除物品字典
    PK_ITEM_ISNEW           = 0x40b #新物品
    PK_BAG_ITEMS            = 0x40c #物品列表
    PK_ITEM_HERO            = 0x420 #所属武将
    PK_ITEM_MOSAIC          = 0x421 #镶嵌的宝石
    PK_ITEM_OPERATE         = 0x422 #当前操作
    PK_ITEM_STATUS          = 0x423 #当前状态0=正常,1=装备
    PK_ITEM_OTHER           = 0x424 #其他通用参数
    PK_ITEM_POS             = 0x425 #宝石镶嵌对应的孔


    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

BagKey = __BagKey()

#Error codes
class __BagError(object):

    __slots__ = ()

    ERR_OK            = 0                     #正常
    ERR_INVALID_PARAM = -4001                #参数错误
    ERR_EMPTY         = -4002                #没有此物品
    ERR_ITEM_DEL      = -4003                #删除物品错误
    ERR_NO_EQUIP      = -4010                #当前物品不是装备
    ERR_EQUIP_NEXIST  = -4011                #装备不存在
    ERR_STONE_NEXIST  = -4012                #宝石不存在
    ERR_NO_MOSAIC     = -4013                #当前无法镶嵌宝石
    ERR_NO_REPLACE    = -4014                #当前无法替换宝石
    ERR_NO_WAR_EQUIP  = -4015                #当前无法穿装备
    ERR_NO_MOSAIC_SAME= -4016                #无法镶嵌同种宝石
    ERR_IS_FULL       = -4017                #背包已满
    ERR_IS_FULL_IN    = -4018                #背包已满


BagError = __BagError()

class BagPack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = BagKey
    def __init__(self, data=None):
        super(BagPack, self).__init__(data)

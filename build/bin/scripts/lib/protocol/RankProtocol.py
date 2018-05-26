#coding: utf-8

from protocol.CommonProtocol import PacketType, PacketType, PacketSchema, GeneralPack

class __RankType(object):

    PT_C2S_GET_RANK = 0x130001   #获取排行榜
    PT_C2S_REFLASH  = 0x130002   #刷新排行榜
    PT_S2C_RANK_INFO = 0x130003


    def __setattr__(self,attr,val):
        raise TypeError("Attribute is readonly")
        pass

RankType = __RankType()


class __RankKey(object):
    PK_ROLE_ID              = 0x1301 #角色ID
    PK_ROLE_SSID            = 0x1302 #角色SSID
    PK_ROLE_TOKEN           = 0x1303 #角色TOKEN
    PK_ROLE_NAME            = 0x1304 #角色名字

    PK_RANK_TYPE            = 0x1305 #排行榜类型
    PK_ALIANCE_ID           = 0x1306 #同盟ID
    PK_ALIANCE_NAME         = 0x1307 #同盟名字
    PK_ALIANCE_FAME         = 0x1308 #盟主名字
    PK_ALIANCE_FLAG         = 0x1309 #同盟旗帜
    PK_ROLE_FAME            = 0x1340 #角色声望
    PK_ROLE_HEAD            = 0x1341 #角色头像
    PK_ROLE_HIS_CON         = 0x1342 #角色同盟总贡献
    PK_ROLE_DAY_CON         = 0x1343 #角色同盟日贡献
    PK_ROLE_WEEK_CON        = 0x1344 #角色同盟周贡献
    PK_TS_LEVEL             = 0x1345 #太守府等级
    PK_HERO_PID             = 0x1346 #英雄模板ID
    PK_HERO_LEVEL           = 0x1347 #英雄等级
    PK_RANK_ID              = 0x1348 #
    PK_RANK_TYPE            = 0x1349 #排行榜类别
    PK_ALIBOSS_ID           = 0x1350 #同盟盟主ID
    PK_ALIBOSS_NAME         = 0x1351 #同盟盟主名字
    PK_RANK_PACK            = 0x1352
    PK_REFLASH_RANK         = 0x1353 #刷新排行榜
    PK_PACK_FINISH          = 0x1354 #分包传输完毕
    PK_JOIN_TIME            = 0x1355
    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

RankKey = __RankKey()

#Error codes
class __RankError(object):

    __slots__ = ()

    ERR_OK              = 0                     #正常
    # ERR_INVALID_PARAM   = -5001 #参数错误
    # ERR_NO_ENOUGH_GOLD  = -5002 #元宝不足
    # ERR_NO_ENOUGH_COIN  = -5003 #铜币不足
    # ERR_NO_ENOUGH_FRAME = -5004 #声望不足
    # ERR_NO_ENOUGH_TIMES = -5005 #当天次数用完
    # ERR_SET_GUO_ERR     = -5006
    # ERR_OPEN_CARD_NONE  = -5007 #开启卡包错误
    # ERR_CARD_IS_OPEN    = -5008 #该卡包已经开启

RankError = __RankError()

class RankPack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = RankKey
    def __init__(self, data=None):
        super(RankPack, self).__init__(data)

#coding: utf-8

from protocol.CommonProtocol import PacketType, PacketType, PacketSchema, GeneralPack

class __TrendType(object):

    PT_C2S_TREND_INFO            = 0x120001 # 各洲记录信息
    PT_C2S_GET_AWARD             = 0x120002 # 领取奖励
    PT_S2C_ALL_TREND_INFO        = 0x120003 # 全服天下大势信息
    PT_S2C_UPDATE_ALL_TREND_INFO = 0x120003 # 更新全服天下大势信息
    PT_S2S_RES_INFO              = 0x120004 # 野外地块占领
    PT_C2S_ALL_TREND_INFO        = 0x120005 # c2s全服天下大势信息
    PT_S2S_STATES_INFO           = 0x120006 # 中立城池占领

    PT_S2S_ALLIANCE_NUM          = 0x120007 # 同盟人数达标的同盟有多少个

    def __setattr__(self,attr,val):
        raise TypeError("Attribute is readonly")
        pass

TrendType = __TrendType()


class __TrendKey(object):
    PK_ROLE_ID              = 0x1201 #角色ID
    PK_ROLE_SSID            = 0x1202 #角色SSID
    PK_ROLE_TOKEN           = 0x1203 #角色TOKEN
    PK_ROLE_NAME            = 0x1204 #角色名字

    PK_STATE_ID        = 0x1205 #洲id
    PK_TREND_ID        = 0x1206 #大势id
    PK_TREND_ISOPEN    = 0x1207 #是否开启
    PK_TREND_STATUS    = 0x1208 #是否达成
    PK_TREND_ISGET     = 0x1209 #是否达成
    PK_TREND_TIME      = 0x120a #结束时间
    PK_TREND_INFO      = 0x120b #基本信息
    PK_RES_LEV         = 0x120c #资源等级
    PK_TREND_FINISH_DT = 0x120d #完成时间
    PK_CITY_ID         = 0x120e #城市id
    PK_LEAGUE_ID       = 0x120f #同盟id
    PK_LEAGUE_NAME     = 0x1211 #同盟名字
    PK_LEAGUE_NUM      = 0x1212 #同盟数量

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

TrendKey = __TrendKey()

#Error codes
class __TrendError(object):

    __slots__ = ()

    ERR_OK            = 0                     #正常
    ERR_INVALID_PARAM = -12001                #参数错误
    ERR_NOT_HAS_AWARD = -12002                #没有奖励
    ERR_BEEN_AWARD    = -12003                #已领取奖励

TrendError = __TrendError()

class TrendPack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = TrendKey
    def __init__(self, data=None):
        super(TrendPack, self).__init__(data)

#coding:utf-8
from    CommonProtocol 	import 	PacketType, PacketType, PacketSchema, GeneralPack

class __BattleType(object):

    PT_C2S_WORLD_RES         = 0x20001 #获得世界资源
    PT_C2S_TOUCH_POINT       = 0x20002 #点击位置
    PT_C2S_EVENT_MOVE        = 0x20003 #迁城
    PT_C2S_EVENT_GATHER      = 0x20004 #采集
    PT_C2S_EVENT_STATION     = 0x20005 #驻扎
    PT_C2S_EVENT_ATTACK      = 0x20006 #攻击
    PT_C2S_EVENT_TRANSPORT   = 0x20007 #运输
    PT_C2S_EVENT_DETECT      = 0x20008 #侦测
    PT_C2S_EVENT_TRADE       = 0x20009 #通商
    PT_S2C_EVENT_CALLBACK    = 0x2000a #回调
    PT_S2C_EVENT_LIST        = 0x2000b #出征队列
    PT_C2S_AEMY_LIST         = 0x2000c #部队列表
    PT_C2S_MARK              = 0x2000d #标记
    PT_C2S_MARK_LIST         = 0x2000e #标记列表
    PT_C2S_EVENT_BACK        = 0x2000f #返回事件
    PT_C2S_EVENT_DETECT_BACK = 0x20010 #返回事件

    PT_C2S_PEEK_EVENT        = 0x20011 #获取一条当前世界事件

    PT_C2S_EVENT_ACCELERATE  = 0x20012 #加速
    PT_C2S_EVENT_INFO        = 0x20013 #资源信息

    PT_S2S_ARMY_LIST         = 0x20014 #部队信息

    PT_C2S_REPORT_LIST       = 0x20015 #战报列表
    PT_C2S_REPORT_INFO       = 0x20016 #战报详情

    PT_S2S_EVENT_ACCELERATE  = 0x20017 #s2s事件加速
    PT_S2S_EVENT_BACK        = 0x20018 #s2s事件加速召回

    PT_C2S_EVENT_BACK_HALFWAY = 0x20019 #半路返回

    PT_C2S_BATTLE_INFO       = 0x2001a #战斗详情

    PT_C2S_REPORT_STATUS     = 0x2001b #战报更新状态
    PT_S2C_REPORT_NEW        = 0x2001c #新战报通知
    PT_C2S_REPORT_ALL        = 0x2001d #所有未读战报数量

    PT_S2S_EVENT_RANDOM_MOVE = 0x2001e #随机迁城


    PT_C2S_TRADE_ARMY_LIST   = 0x2001f #玩家通商队伍列表
    PT_C2S_TRADE_LIST        = 0x20020 #玩家城内通商列表
    PT_C2S_TRADE_REPATRIATE  = 0x20021 #通商遣返
    PT_S2S_TRADE_ARMY_UPDATE = 0x20022 #通商队伍更新

    PT_S2C_EVENT_END         = 0x20023 #事件结束

    PT_S2S_CREATE_NAME       = 0x20024 #s2s创建名字


    PT_S2C_WORLD_NOTIFY      = 0x20025 #事件通知

    PT_C2S_GET_RES_TYPE      = 0x20026 #拿周围的某一个资源， 返回坐标

    PT_S2S_UPDATE_HEAD       = 0x20027 #s2s更新头像
    PT_C2S_GET_STATION_LIST  = 0x20028 #获取驻扎列表
    PT_C2S_STATION_REPATRIATE= 0x20029 #驻扎遣返

    PT_C2S_NEW_CITY          = 0x2002a #新出生点
    PT_S2S_ALLIANCE_INFO     = 0x2002b #更新同盟


    PT_C2S_EVENT_GIVEUP      = 0x2002c #放弃土地
    PT_S2C_WORLD_INFO        = 0x2002d #世界基本信息

    PT_S2S_ROLE_INFO         = 0x2002e #更新角色信息

    PT_S2C_BATTLE_INFO       = 0x2002f #通知前段进入战斗表现

    PT_S2C_PROVINCE_LIST     = 0x20030 #同盟领地
    PT_S2C_PROVINCE_STATUS   = 0x20031 #同盟领地占领
    PT_C2S_PROVINCE_WAR      = 0x20032 #同盟战争

    PT_S2C_EVENT_MOVE        = 0x20033 #s2c 迁城
    PT_S2C_UPDATE_RES        = 0x20034 #s2c 更新单个资源

    PT_S2C_INFLUENCE         = 0x20035 #s2c 个人势力
    PT_S2C_INFLUENCE_UPDATE  = 0x20036 #s2c 更新个人势力

    PT_C2S_FIND_NEARBY       = 0x20037 #查找附近的资源
    PT_S2S_RES_NUM           = 0x20038 #s2s 土地数量
    PT_S2C_SHOW_BORN         = 0x20039 #显示出生选择州
    PT_S2S_UPDATE_BORN       = 0x2003a #更新出生

    PT_S2C_EVENT_INFO        = 0x2003b #更新事件信息

    def __setattr__(self,attr,val):
        raise TypeError("Attribute is readonly")

BattleType = __BattleType()


class __BattleKey(object):

    PK_ROLE_ID            = 0x201 #角色ID
    PK_ROLE_SSID          = 0x202 #角色SSID
    PK_ROLE_TOKEN         = 0x203 #角色TOKEN
    PK_ROLE_POINT         = 0x204 #玩家位置

    PK_BATTLE_RES_ID      = 0x205 #资源ID
    PK_BATTLE_RES_TYPE    = 0x206 #资源类型
    PK_BATTLE_RES_POS     = 0x207 #资源位置
    PK_BATTLE_RES_LEV     = 0x208 #资源等级
    PK_BATTLE_RES_INFO    = 0x209 #资源信息
    PK_BATTLE_RES_DICT    = 0x20a #资源列表

    PK_BATTLE_CONFIRM     = 0x20b #确认

    PK_BATTLE_ARMY_ID     = 0x20c #部队ID
    PK_BATTLE_ARMY_LIST   = 0x20d #部队列表
    PK_BATTLE_ARMY_INFO   = 0x20e #部队信息

    PK_BATTLE_HERO_ID     = 0x20f #英雄ID
    PK_BATTLE_HERO_INFO   = 0x210 #英雄信息

    PK_BATTLE_MARK_LIST   = 0x211 #标记列表

    PK_EVENT_ID           = 0x212 #事件ID
    PK_EVENT_INFO         = 0x213 #事件信息
    PK_EVENT_ORIGIN       = 0x214 #事件原点
    PK_EVENT_TARGET       = 0x215 #事件结束点
    PK_EVENT_LIST         = 0x216 #事件列表
    PK_EVENT_TYPE         = 0x217 #事件类型
    PK_EVENT_CALLBACKTYPE = 0x218 #事件类型



    PK_BATTLE_VAL_1       = 0x219 #值1
    PK_BATTLE_VAL_2       = 0x21a #值2
    PK_BATTLE_VAL_3       = 0x21b #值3
    PK_BATTLE_VAL_4       = 0x21c #值4
    PK_BATTLE_VAL_5       = 0x21d #值5
    PK_BATTLE_VAL_6       = 0x21e #值6
    PK_BATTLE_VAL_7       = 0x21f #值7


    PK_HERO_ID            = 0x220 #英雄ID
    PK_HERO_HID           = 0x221 #英雄模版ID
    PK_HERO_LEVEL         = 0x222 #英雄等级
    PK_HERO_BATT          = 0x223 #英雄兵力
    PK_HERO_HP            = 0x224 #英雄体力
    PK_HERO_HURT          = 0x225 #对别人普通伤害
    PK_HERO_MHURT         = 0x226 #对别人法术伤害
    PK_HERO_HARM          = 0x227 #损伤
    PK_HERO_INJURED       = 0x228 #伤兵
    PK_HERO_BATTLE_INFO   = 0x229 #英雄战斗统计

    PK_BATTLE_INFO        = 0x22a #战前信息
    PK_BATTLE_REPORT      = 0x22b #战报
    PK_BATTLE_RESULT      = 0x22c #战斗统计
    PK_BATTLE_TROPHY      = 0x22d #战利品
    PK_BATTLE_WIN         = 0x22e #胜利
    PK_BATTLE_HURT        = 0x22f #战斗损伤
    PK_BATTLE_REPORT_ID   = 0x230 #战报ID

    PK_ROLE_NAME          = 0x231 #角色名字
    PK_ROLE_EXP           = 0x232 #经验

    PK_HERO_STAR          = 0x233 #英雄星级
    PK_BATTLE_RES_NAME    = 0x234 #资源名字
    PK_BATTLE_STATUS      = 0x235 #状态
    PK_HERO_NAME          = 0x236 #英雄名字

    PK_EVENT_TRID         = 0x237 #对方uid
    PK_BATTLE_RES_PIC     = 0x238 #对方的头像

    PK_HERO_EXP           = 0x239 #英雄经验
    PK_HERO_LEV_INFO      = 0x23a #英雄升级信息

    PK_ALLIANCE_ID        = 0x23b #同盟ID
    PK_ALLIANCE_NAME      = 0x23c #同盟名字
    PK_ALLIANCE_PIC       = 0x23d #同盟Pic

    PK_BATTLE_RES_WARFREE = 0x23e #时间免战
    PK_BATTLE_RES_GIVEUP  = 0x23f #时间放弃

    PK_BATTLE_RES_NUM     = 0x240 #拥有土地数量

    PK_ROLE_FAME          = 0x241 #角色声望

    PK_STATION_LEV        = 0x242 #驻扎等级
    PK_ROLE_LEV           = 0x243 #太守府等级

    PK_PROVINCE_LIST      = 0x244 #中立城池列表

    PK_ALLIANCE_TID       = 0x245 #其它同盟ID
    PK_ALLIANCE_TNAME     = 0x246 #其它同盟名字

    PK_ROLE_VIP           = 0x247 #玩家VIP
    PK_ROLE_VIP_STATUS    = 0x248 #玩家VIP状态


    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

BattleKey = __BattleKey()

#Error codes
class __BattleError(object):
    __slots__ = ()
    ERR_OK                     = 0 #正常
    ERR_OFFLINING              = -2001 #离线
    ERR_INVALID_PARAM          = -2002 #参数错误
    ERR_BATTLE_RES_NONE        = -2003 #空资源
    ERR_BATTLE_RES_TYPE_ERR    = -2004 #错误资源
    ERR_BATTLE_ARMY_HAS_EVEN   = -2005 #部队已经有事件
    ERR_WORLD_MARK_MORE        = -2006 #超过20个标记
    ERR_WORLD_TRADE_FULL       = -2007 #该城商队已满
    ERR_WORLD_HAS_TRADE        = -2008 #已有商队前往该城池
    ERR_WORLD_TRADE_UNUSED     = -2009 #当前无闲置商队可进行通商
    ERR_WORLD_TRADE_CUR        = -2010 #该城池已有你的商队
    ERR_WORLD_TRANSPORT_UNUSED = -2011 #当前无闲置运输队可进行运输
    ERR_WORLD_DETECT_UNUSED    = -2012 #当前无闲置侦查队可进行侦查

    ERR_WORLD_EVENT_NOTFOUND   = -2013 #当前没有更多世界事件

    ERR_ARMY_HAS_EVENT         = -2014 #部队已出征
    ERR_ARMY_NOT_HP            = -2015 #部队没有体力
    ERR_ARMY_IS_HURT           = -2016 #部队重伤中
    ERR_ARMY_IS_CONS           = -2017 #部队征兵中

    ERR_TARGET_CHANGE          = -2018 #目标已变更


    ERR_NOTITY_ATTACK          = -2019 #你受到了%s的袭击
    ERR_NOTITY_MOVE_CHANGE     = -2020 #目标已被占领，无法迁城
    ERR_NOTITY_STATION_CHANGE  = -2021 #目标已被占领，无法驻扎
    ERR_NOTITY_ACCELERATE      = -2022 #%s部队行军时间缩短%s

    ERR_NOTITY_HAS_ATTACK      = -2023 #正在受到别人攻击

    ERR_NEW_ROLE_CITY_FULL     = -2024 #当前洲创建人已满
    ERR_NEW_ROLE_FULL          = -2025 #所有创建人已满

    ERR_WORLD_RES_LIMIT        = -2026 #超过土地上限

    ERR_NOTHAS_ALIANCE         = -2027 #没有同盟
    ERR_MOVE_WARFREE_NOT       = -2028 # 该领地处于免战期，不能迁城到此地
    ERR_MOVE_STATION_NOT       = -2029 # 该领地有驻扎，不能迁城到此地
    pass

BattleError = __BattleError()



class __BattleTip(object):


    TIP_TARGET_CHANGE           = -20001 # 目标已变更，请重新操作
    TIP_STATION_SUCCEED         = -20002 # 你的%s分队在%s驻扎成功
    TIP_STATION_CHANGE_BACK     = -20003 # %s地点发生了变化，你的%s分队未进行驻扎，正在返回
    TIP_STATION_FULL            = -20004 # 该城池驻扎队列已满，无法驻扎
    TIP_STATION_FULL_BACK       = -20005 # %s城池驻扎队列已满，你的%s分队未进行驻扎，正在返回
    TIP_STATION_BACK            = -20006 # 驻扎部队%s分队已返回城内
    TIP_STATION_ATTACK_BACK     = -20007 # 你的%s分队在%s被击败了，正在返回
    TIP_STATION_REPATRIATE_BACK = -20008 # 你的%s分队在%s被遣返了，正在返回
    TIP_ATTACK_CHANGE_BACK      = -20009 # %s地点发生了变化，你的%s分队未进行攻击，正在返回
    TIP_ATTACK_SUCCEED_LOOT     = -20010 # 战斗胜利！你的%s分队掠夺了%s，正在返回
    TIP_ATTACK_FAIL_BACK        = -20011 # 战斗失败！你的%s分队在%s被击败，正在返回
    TIP_ATTACK_SUCCEED_OCCUPY   = -20012 # 战斗胜利！你的%s分队成功占领了%s，正在返回
    TIP_ATTACK_BACK             = -20013 # 攻击部队%s分队已返回城内
    TIP_ATTACK_WARFREE_BACK     = -20014 # %s处于免战期间，你的%s分队无法进行攻击，正在返回
    TIP_ATTACK_SEIZE            = -20015 # 你在%s的领地被%s夺取了！
    TIP_DETECT_EMPTY            = -20016 # 当前无侦查队可用
    TIP_DETECT_CHANGE_BACK      = -20017 # %s地点发生了变化，你的侦查队未进行侦查，正在返回
    TIP_DETECT_WARFREE          = -20018 # %s处于免战期间，你的侦查队无法进行侦查，正在返回
    TIP_DETECT_SUCCEED          = -20019 # 你的侦查队在%s侦查成功，正在返回
    TIP_DETECT_FAIL             = -20020 # 你的侦查队在%s侦查失败，正在返回
    TIP_DETECT_BACK             = -20021 # 侦查队已返回城内
    TIP_TRANSPORT_EMPTY         = -20022 # 运输的资源不能为0
    TIP_TRANSPORT_NOT_ENOUGH    = -20023 # 资源不足，运输失败
    TIP_TRANSPORT_FULL          = -20024 # 最多可以同时派出3支运输队
    TIP_TRANSPORT_BACK          = -20025 # 运输队已返回城内
    TIP_TRANSPORT_SUCCEED       = -20026 # 你的运输队在%s运输成功，正在返回
    TIP_TRANSPORT_FAIL          = -20027 # 你的运输队在%s运输失败，正在返回
    TIP_TRADE_ONE               = -20028 # 同一城池只能派遣一支商队进行通商
    TIP_TRADE_EMPTY             = -20029 # 当前无闲置商队，请升级商会增加商队数量
    TIP_TRADE_ING               = -20030 # 该城池已有你的商队
    TIP_TRADE_FULL              = -20031 # 该城池商队已满，无法通商
    TIP_TRADE_CHANGE_BACK       = -20032 # %s地点发生了变化，你的商队未进行通商，正在返回
    TIP_TRADE_FULL_BACK         = -20033 # %s城池商队已满，你的商队无法进入，正在返回
    TIP_TRADE_SUCCEED           = -20034 # 开始对%s进行通商
    TIP_TRADE_BACK              = -20035 # 商队已返回城内
    TIP_TRADE_REPATRIATE_BACK   = -20036 # 你的商队在%s被遣返了，正在返回
    TIP_TRADE_ATTACK_BACK       = -20037 # 你的商队在%s被掠夺了，正在返回
    TIP_TRANSPORT_RES           = -20038 # %s给你送来了%s
    TIP_ATTACK_NOTHAS_ALIANCE   = -20039 # 未加入同盟的玩家，无法攻击中立城池
    TIP_ATTACK_FULL_BACK        = -20040 # 领地已满，你的%s分队未对%s进行攻击，正在返回
    TIP_MOVE_FULL               = -20041 # 当前所处州内，没有无主之地可迁往
    TIP_RES_GIVEUP              = -20042 # 你放弃了在%s的领地
    TIP_RES_GIVEUP_CANEN        = -20043 # 你终止了放弃该领地
    TIP_RES_NOT_CAN_GIVEUP      = -20044 # 该领地处于免战期，无法放弃
    TIP_STATION_NONE            = -20045 # 该城池未建造鸿胪寺，无法驻扎
    TIP_ATTACK_FAIL_LOOT        = -20046 # 战斗失败！你的城池被%s掠夺了
    TIP_ATTACK_MOP_BACK         = -20047 # 扫荡成功！你的%s分队在%s进行了扫荡，正在返回
    TIP_ATTACK_MOP_FAIL_BACK    = -20048 # 扫荡失败！你的%s分队在%s被击败，正在返回
    TIP_MOVE_NOT_FAME           = -20049 # 声望不够无法迁城
    # 已解除保护状态，小心保护自己！
    # 已解除保护状态，去干一番霸业吧！

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")


BattleTip = __BattleTip()


class BattlePack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = BattleKey
    def __init__(self, data=None):
        super(BattlePack, self).__init__(data)

#coding: utf-8

from protocol.CommonProtocol import PacketType, PacketType, PacketSchema, GeneralPack

class __CampaginType(object):

    PT_S2C_CP_INFO     = 0x100001 #s2c战役信息
    PT_C2S_CRUSADE     = 0x100002 #讨伐
    PT_C2S_VISIT       = 0x100003 #拜访
    PT_C2S_CRUSADE_ADD = 0x100004 #增加讨伐次数



    def __setattr__(self,attr,val):
        raise TypeError("Attribute is readonly")
        pass

CampaginType = __CampaginType()


class __CampaginKey(object):
    PK_ROLE_ID            = 0x1001 #角色ID
    PK_ROLE_SSID          = 0x1002 #角色SSID
    PK_ROLE_TOKEN         = 0x1003 #角色TOKEN
    PK_ROLE_NAME          = 0x1004 #角色名字

    PK_CP_ID     = 0x1005 #关卡ID
    PK_CP_ITEMS  = 0x1006 #物品列表
    PK_CP_NUM    = 0x1007 #可挑战次数
    PK_CP_AID    = 0x1008 #部队ID
    PK_CP_BATTLE = 0x1009 #战斗信息
    PK_CP_GOLD   = 0x100a #需要元宝


    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

CampaginKey = __CampaginKey()

#Error codes
class __CampaginError(object):

    __slots__ = ()

    ERR_OK            = 0                     #正常
    ERR_INVALID_PARAM = -10001 #参数错误

    ERR_NOT_OPEN      = -10002 #关卡尚未开启，无法进行讨伐
    ERR_NOT_NUM       = -10003 #挑战次数已用完，无法进行讨伐
    ERR_NOT_ALLNUM    = -10004 #挑战次数已满，无法购买
    ERR_NOT_VISIT_NUM = -10005 #拜帖不足，无法拜访



#
#     630,1,战役系统,选中未开启关卡，点击讨伐按钮,关卡尚未开启，无法进行讨伐,2
# 631,1,战役系统,当玩家使用完挑战次数后，点击讨伐按钮,挑战次数已用完，无法进行讨伐,2
# 632,1,战役系统,当前挑战次数已满，点击“+”号按钮,挑战次数已满，无法购买,2
# 633,1,战役系统,处于购买挑战次数确认界面（打开后等到刷新挑战次数），元宝足够，挑战次数已满，点击确定按钮回到战役系统界面飘字,挑战次数已满，无法购买,2
# 634,1,战役系统,当玩家有拜帖道具时，点击拜访按钮，拜访成功,获得%s碎片 x1,2
# 635,1,战役系统,当玩家无拜帖时，点击拜访按钮,拜帖不足，无法拜访,2
# 636,1,战役系统,当前章节有关卡处于未通关状态时，点击向右箭头,当前章节尚未通关,2
# 637,1,战役系统,通关最新章节的最后一关，回到战役界面飘字提示,已通关%s,2
# 638,1,战役系统,处于选择派出部队列表，点击体力不足的部队,该部队体力不足，无法出征,2
# 639,1,战役系统,处于选择派出部队列表，点击重伤中的部队,该部队处于重伤中，无法出征,2
# 640,1,战役系统,处于选择派出部队列表，点击行军中的部队,该部队正在行军中，无法出征,2
# 641,1,战役系统,处于选择派出部队列表，点击征兵中的部队,该部队正在征兵中，无法出征,2
#
# 712,2,战役系统,处于战役系统界面，剩余挑战次数未满，点击“+”号按钮,是否花费%s元宝增加一次挑战次数？,2
# 713,2,战役系统,处于购买挑战次数确认界面，元宝不足，点击确定按钮,元宝不足，是否前往充值？,2

CampaginError = __CampaginError()

class CampaginPack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = CampaginKey
    def __init__(self, data=None):
        super(CampaginPack, self).__init__(data)

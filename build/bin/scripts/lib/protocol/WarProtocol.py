#coding: utf-8

from protocol.CommonProtocol import PacketType, PacketType, PacketSchema, GeneralPack

class __WarType(object):

    PT_S2S_BATTLE_REPORT = 0x110001   #战斗战报
    PT_S2C_SHOW_BATTLE   = 0x110002   #显示战斗
    PT_C2S_REPORT_INFO   = 0x110003   #战报详情  
    PT_C2S_REPORT_ALL    = 0x110004   #所有战报  
    PT_C2S_REPORT_STATUS = 0x110005   #战报状态  
    PT_C2S_REPORT_LIST   = 0x110006   #战报列表  
    PT_S2C_REPORT_NEW    = 0x110007   #新的战报
    PT_C2S_BATTLE_INFO   = 0x110008   #战斗详情
    PT_S2S_OTHER_REPORT  = 0x110009   #其他战报
    PT_C2S_ALIANCE_REPORT= 0x110010   #同盟战报
    PT_S2S_UPDATE_ALIANCE= 0x110011   #更新同盟
     
    
    def __setattr__(self,attr,val):
        raise TypeError("Attribute is readonly")
        pass

WarType = __WarType()


class __WarKey(object):
    PK_ROLE_ID              = 0x1101 #角色ID
    PK_ROLE_SSID            = 0x1102 #角色SSID
    PK_ROLE_TOKEN           = 0x1103 #角色TOKEN
    PK_ROLE_NAME            = 0x1104 #角色名字

    PK_WAR_INIT_INFO        = 0x1150  #战斗初始化信息
    PK_WAR_PROCESS_INFO     = 0x1151  #战斗流程
    PK_WAR_ATK_TEAM         = 0x1152  #攻击方
    PK_WAR_DEF_TEAM         = 0x1153  #防御方
    PK_WAR_REWARD           = 0x1154  #战斗奖励
    PK_WAR_END              = 0x1155  #战斗结束

    PK_WAR_IS_ATTACK        = 0x1156  #是否攻击
    PK_WAR_DEFAULT_BUFF     = 0x1157  #默认buff
    PK_WAR_RESULT           = 0x1158  #战斗结果
    PK_WAR_EXP              = 0x1159  #获得经验 
    PK_WAR_COIN             = 0x1160  #获得的铜币
    PK_WAR_ITENS            = 0x1161  #获得的物品
    PK_WAR_INFO             = 0x1162  #最后每个武将的信息  
    PK_WAR_ATTACK_UID       = 0x1163  #是否攻击方

    PK_WAR_REPORT           = 0x1164  #战报
    PK_WAR_ISSHOW           = 0x1165  #是否显示
    PK_WAR_REPORT_TYPE      = 0x1166  #战报类型
    PK_WAR_POS              = 0x1167  #战斗坐标
    PK_WAR_START_TIME       = 0x1168  #战报产生的开始时间
    PK_WAR_DEFE_NAME        = 0x1169  #防守方名字
    PK_WAR_DEFE_UID         = 0x1170  #防守方ID
    PK_WAR_TYPE             = 0x1171  #战报类型
    PK_WAR_END_TIME         = 0x1172  #战报产生的结束时间 
    PK_WAR_REPORT_ID        = 0x1173  #战报ID
    PK_WAR_ATTK_NAME        = 0x1174  #攻击方名字
    PK_WAR_ALIANCE_ID       = 0x1175  #同盟ID
    PK_WAR_AD_NAME          = 0x1176  #攻击方同盟名字
    PK_WAR_FD_NAME          = 0x1177  #防守方同盟名字

    PK_WAR_PARAM            = 0x1105
    PK_VAL_1                = 0x1106
    PK_VAL_2                = 0x1107
    PK_VAL_3                = 0x1108
    PK_VAL_4                = 0x1109
    PK_VAL_5                = 0x110a
    PK_VAL_6                = 0x110b
    PK_VAL_7                = 0x110c
    PK_VAL_8                = 0x110d
    PK_VAL_9                = 0x110e
    PK_VAL_10               = 0x110f
    PK_VAL_11               = 0x1110
    PK_VAL_12               = 0x1111
    PK_VAL_13               = 0x1112
    PK_VAL_14               = 0x1113
    PK_VAL_15               = 0x1114
    PK_VAL_16               = 0x1115
    PK_VAL_17               = 0x1116
    PK_VAL_18               = 0x1118


    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

WarKey = __WarKey()

#Error codes
class __WarError(object):

    __slots__ = ()

    ERR_OK                      = 0                     #正常
    ERR_INVALID_PARAM           = -11001                #参数错误
    ERR_REPORT_NO_EXISTS        = -11002                #战报不存在
    ERR_REPORT_FORMAT           = -11003                #战报格式错误




WarError = __WarError()

class WarPack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = WarKey
    def __init__(self, data=None):
        super(WarPack, self).__init__(data)

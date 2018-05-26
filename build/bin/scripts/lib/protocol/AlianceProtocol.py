#coding:utf-8
from CommonProtocol import PacketType, PacketType, PacketSchema, GeneralPack

class __AlianceType(object):
    PT_C2S_CREATE_ALIANCE                 = 0x90001     #创建同盟
    PT_C2S_SEARCH_ALIANCE                 = 0x90002     #搜索同盟
    PT_C2S_ALIANCE_OPERATE                = 0x90003     #同盟操作 
    PT_C2S_UPDATE_NOTICE                  = 0x90004     #更新同盟公告
    PT_C2S_UPDATE_FLAG                    = 0x90005     #更新旗帜
    PT_C2S_GET_APPLY                      = 0x90006     #获取申请列表
    PT_C2S_APPLY_OPERATE                  = 0x90007     #申请列表操作
    PT_C2S_HELP_ALIANCE                   = 0x90008     #帮助同盟
    PT_S2S_ALIANCE_HELP                   = 0x90010     #帮助建筑
    PT_C2S_ALIANCE_INFO                   = 0x90011     #同盟信息
    PT_C2S_GET_MEMBER_INFO                = 0x90012     #获取成员信息
    PT_C2S_GET_TECHNO_INFO                = 0x90013     #获取同盟科技信息
    PT_C2S_STUDY_TECHNO                   = 0x90014     #学习研究科技       
    PT_C2S_GET_TOP_INFO                   = 0x90015     #同盟排行榜信息
    PT_C2S_INVITE_JOIN                    = 0x90016     #邀请玩家加入
    PT_C2S_GET_HELP_LIST                  = 0x90017     #获取帮助列表
    PT_S2C_ALIANCE_ADVER                  = 0x90018     #同盟广告
    PT_C2S_AGREEN_INVITE_JOIN             = 0x90019     #同意对方邀请  
    PT_C2S_REQUEST_RECOMMEND              = 0x90020     #请求推荐
    PT_S2C_UPDATE_HELP                    = 0x90021     #更新帮助 
    PT_C2S_ROLE_APPLY                     = 0x90022     #角色申请加入同盟 
    PT_S2C_UPDATE_APPLY_LIST              = 0x90023     #更新申请列表
    PT_S2C_UPDATE_TECHNO_INFO             = 0x90024     #更新科技信息
    PT_S2C_UPDATE_MEMBER_INFO             = 0x90025     #更新成员信息
    PT_S2C_INVITE_JOIN                    = 0x90026     #通知邀请玩家加入
    PT_S2C_NOTICE_JOIN                    = 0x90027     #通知加入同盟
    PT_C2S_TECHNO_ADD_SPEED               = 0x90028     #科技加速
    PT_C2S_GET_ALIANCE_LIST               = 0x90029     #获取同盟列表
    PT_S2S_ALIANCE_RANK                   = 0x90030     #获取同盟排名
    PT_S2S_GET_ALIANCE_MEMBER             = 0x90031     #获取同盟成员
    PT_S2S_CHECK_ALIANCE                  = 0x90032     #检查同盟
    PT_S2S_GET_ALIANCE_TECHNO             = 0x90033     #获取同盟科技等级
    PT_S2S_UPDATE_CONTRIBUTE              = 0x90034     #更新同盟贡献值 
    PT_S2S_UPDATE_ROLE_INFO               = 0x90035     #更新角色信息
    def __setattr__(self,attr,val):
        raise TypeError("Attribute is readonly")

AlianceType = __AlianceType()


class __AlianceKey(object):
   
    PK_ROLE_ID              = 0x900     #角色ID
    PK_ROLE_SSID            = 0x901     #角色SSID
    PK_ROLE_TOKEN           = 0x902     #角色TOKEN
    PK_ALIANCE_ID           = 0x903     #同盟ID
    PK_ROLE_HEAD_PIC        = 0x904     #角色头像
    PK_ROLE_NAME            = 0x905     #角色名字 
    PK_ALIANCE_NAME         = 0x906     #同盟名称
    PK_ALIANCE_TYPE         = 0x907     #职位
    PK_ALIANCE_FLAG         = 0x908     #同盟旗帜
    PK_ALIANCE_NOTICE       = 0x909     #同盟公告
    PK_ALIANCE_TIME         = 0x910     #同盟创建时间
    PK_ROLE_CONTRIBUTE      = 0x911     #角色贡献
    PK_ALIANCE_CONTRIBUTE   = 0x912     #同盟贡献
    PK_ALIANCE_LEADER       = 0x913     #盟主的名字
    PK_ROLE_JOIN_TIME       = 0x914     #角色加入时间
    PK_ROLE_APPLY_TIME      = 0x915     #角色申请时间
    PK_ALIANCED_FAME        = 0x916     #同盟声望
    PK_ALIANCED_NUM         = 0x917     #同盟当前人数
    PK_ALIANCED_MAX_NUM     = 0x918     #同盟当前最大人数
    PK_ROLE_APPLY_STATUS    = 0x919     #申请状态0=未申请,1=已申请
    PK_ROLE_FAME            = 0x920     #同盟声望

    PK_BUILD_ID             = 0x921     #建筑ID
    PK_BUILD_TYPE           = 0x922     #建筑类型
    PK_BUILD_LEVEL          = 0x923     #建筑等级
    PK_ROLE_OPERATE         = 0x924     #角色操作
    PK_MEMBER_INFO          = 0x925     #成员信息
    PK_ROLE_SEVEN_CONTRIBUTE= 0x926     #成员7日贡献 
    PK_ROLE_ONLINE          = 0x927     #角色是否在线
    PK_ALIANCE_NEW_APPLY    = 0x928     #是否有新的申请
    PK_ROLE_LOGOUT_TIME     = 0x929     #离线时间

    PK_TECHNO_ID            = 0x930     #同盟科技
    PK_TECHNO_TYPE          = 0x931     #科技类型
    PK_TECHNO_LEVEL         = 0x932     #科技等级
    PK_TECHNO_EXP           = 0x933     #科技经验
    PK_TECHNO_SYTIME        = 0x944     #科技剩余秒数
    PK_HELP_INFO            = 0x945     #帮助信息
    PK_TECHNO_INFO          = 0x946     #科技信息

    PK_HELP_ID              = 0x950     #帮助ID
    PK_HELP_STATUS          = 0x951     #帮助状态
    PK_BUILD_ID             = 0x952     #建筑ID
    PK_BUILD_TYPE           = 0x953     #建筑类型
    PK_BUILD_LEVEL          = 0x954     #建筑等级
    PK_HELP_NUM             = 0x955     #当前帮助次数
    PK_HELP_MAX_NUM         = 0x956     #当前帮助最大次数
    PK_HELP_UID_LIST        = 0x957     #当前帮助列表
    PK_ALIANCE_INFO         = 0x958     #同盟信息
    PK_ALIANCE_OTHER        = 0x958     #同盟其他


    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

AlianceKey = __AlianceKey()



#Error codes
class __AlianceError(object):
    __slots__ = ()
    ERR_OK                                     = 0                   #正常
    ERR_ALIANCE_NO_EXISTS                      = -9001               #同盟不存在
    ERR_NO_HAS_MEMBERS                         = -9002               #成员不在此同盟中
    ERR_LEADER_IS_MAX_NUM                      = -9003               #盟主人数已经最大
    ERR_DLEADER_IS_MAX_NUM                     = -9004               #副盟主人数已经最大
    ERR_OFFICE_IS_MAX_NUM                      = -9005               #官员人数已经最大
    ERR_NO_POWER_OPERATE                       = -9006               #无权限操作
    ERR_POS_MORE_TWO                           = -9007               #位阶必须大于对方2个位阶
    ERR_POS_MORE_POS                           = -9008               #位阶必须大于对方位阶
    ERR_NO_IN_APPLY                            = -9009               #不在申请列表中
    ERR_IS_HAS_ALIANCE_NAME                    = -9010               #有相同名字的同盟
    ERR_EXISTS_APPLY_LIST                      = -9011               #申请列表中已存在申请，无法重复申请
    ERR_ALIANCE_IS_HAS                         = -9012               #对方已经有同盟
    ERR_JOIN_TIME_NO_DAY                       = -9013               #加入时间小于1天
    ERR_HELP_IS_EXISTS                         = -9014               #当前帮助存在 
    ERR_HELP_IS_MAX_NUM                        = -9015               #当前已是最大次数，无法帮助
    ERR_ALREADY_HELP                           = -9016               #已经帮助过,无法帮助
    ERR_HELP_NO_EXISTS                         = -9017               #当前帮助不存在 
    ERR_TECHNO_NO_EXISTS                       = -9018               #科技不存在
    ERR_NO_STUDY_TECHNO                        = -9019               #无法研究                      
    ERR_IS_HAS_ALIANCE                         = -9020               #存在同盟
    ERR_APPLY_MAX_LIMIT                        = -9021               #申请超过最大限制
    ERR_MEMBER_MAX_LIMIT                       = -9022               #同盟人数已达最大限制
    ERR_NO_HELP_SELF                           = -9023               #无法帮助自己
    ERR_CONTRIBUTE_SHORT                       = -9024               #贡献值不足  
    pass

AlianceError = __AlianceError()


class AliancePack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = AlianceKey
    def __init__(self, data=None):
        super(AliancePack, self).__init__(data)

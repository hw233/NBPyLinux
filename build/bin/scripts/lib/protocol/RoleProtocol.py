#coding:utf-8
from CommonProtocol import PacketType, PacketType, PacketSchema, GeneralPack

class __RoleType(object):
    PT_CHECKUKEY                 = 0x10001     #检查
    PT_LOGIN                     = 0x10002     #登录
    PT_REGISTER                  = 0x10003     #注册
    PT_GET_ROLE_INFO             = 0x10004     #获取角色信息
    PT_S2S_UPDATE_ROLE           = 0x10005     #更新角色信息
    PT_CREATE_NAME               = 0x10006     #创建角色名称
    PT_REGISTER_NEWUI            = 0x10007     #新UI注册
    PT_S2C_REPEAT_LOGIN          = 0x10008     #重复登录
    PT_ONEKEY_LOGIN              = 0x10009     #一键登录／输入帐号登录
    PT_UPGRADE_BUILD             = 0x10010     #升级建筑
    PT_BUILD_CD_SPEED            = 0x10011     #建筑CD加速
    PT_BUILD_CD_CHECK            = 0x10012     #建筑CD检查
    PT_GET_BUILD_INFO            = 0x10013     #建筑信息
    PT_ANTI_ADDITIION            = 0x10014     #防沉迷个人信息输入
    PT_S2C_BUILD_PRODUCE         = 0x10015     #建筑产出
    PT_C2S_EXCHANGE_ROLE         = 0x10016     #兑换
    PT_ROLE_UPDATE               = 0x10017     #更新角色
    PT_CHECK_ANTI                = 0x10018     #检查防沉迷
    PT_S2S_HERO_EQUIP_ATTR       = 0x10019     #装备属性更新
    PT_C2S_HERO_UPGRADE          = 0x10020     #武将升级
    PT_C2S_HERO_RESOLVED         = 0x10021     #武将分解
    PT_C2S_ACTIVE_XINGE          = 0x10022     #星格激活
    PT_C2S_HERO_UPGRADE_STAR     = 0x10023     #武将升星
    PT_S2C_HERO_UPDATE           = 0x10024     #武将更新
    PT_C2S_HERO_FETTER           = 0x10025     #武将羁绊
    PT_C2S_HERO_FETTER_SPELL     = 0x10026     #武将解除羁绊
    PT_C2S_HERO_SKILL_UPGRADE    = 0x10027     #武将技能升级
    PT_C2S_HERO_FORCE            = 0x10028     #武将上阵下阵
    PT_C2S_GET_HERO_LIST         = 0x10029     #获取武将列表
    PT_C2S_GET_HERO_INFO         = 0x10030     #获取武将信息
    PT_BUY_QUEUE_NUM             = 0x10031     #购买队列
    PT_BUILD_PRODUCE             = 0x10032     #建筑产出
    PT_C2S_HERO_CONSCRIPT        = 0x10033     #武将征兵
    PT_C2S_CONSCRIPT_OPERATE     = 0x10034     #武将征兵操作(取消/立即完成)
    PT_C2S_HERO_EXCHANGE         = 0x10035     #武将互换
    PT_S2S_UPDATE_HERO           = 0x10036     #武将信息更新
    PT_S2S_GET_FORCE_INFO        = 0x10037     #获取部队信息
    PT_S2S_GET_OTHER_INFO        = 0x10038     #获取其他角色信息
    PT_C2S_HERO_REFINE           = 0x10039     #武将修炼
    PT_C2S_REFINE_OPERATE        = 0x10040     #武将修炼操作
    PT_C2S_GET_HERO_REFINE       = 0x10041     #获取武将修炼信息
    PT_C2S_GET_TAXES_INFO        = 0x10042     #获取征收信息
    PT_C2S_ROLE_TAXES_OPERATE    = 0x10043     #角色征收
    PT_C2S_GET_TECHNO_INFO       = 0x10044     #获取科技信息
    PT_C2S_STUDY_TECHNO          = 0x10045     #学习科技
    PT_S2C_UPDATE_TECHNO         = 0x10046     #更新学习科技
    PT_C2S_NETWORK_RECONN        = 0x10047     #断线重连
    PT_C2S_GET_OTHER_INFO        = 0x10048     #获取其他角色基本信息
    PT_S2S_USE_ITEM              = 0x10049     #使用物品新增buff
    PT_S2S_GET_RES_INFO          = 0x10050     #获取角色资源信息
    PT_S2S_GET_ROLE_DECETE       = 0x10051     #获取角色侦查成功率
    PT_C2S_UPDATE_HEAD_PIC       = 0x10052     #更新角色头像
    PT_C2S_ALIANCE_HELP          = 0x10053     #请求同盟帮助
    PT_S2S_HELP_ALIANCE          = 0x10054     #帮助同盟
    PT_C2S_HERO_COMPOSE          = 0x10055     #武将合成
    PT_S2S_ALIANCE_TECHNO        = 0x10056     #同盟科技信息更新
    PT_S2S_UPDATE_ALIANCED       = 0x10057     #更新同盟
    PT_S2S_BLOCK_PRODUCT         = 0x10058     #更新地块产出
    PT_S2C_OTHER_PRODUCT         = 0x10059     #其他产出
    PT_C2S_GET_GUIDE             = 0x10060     #获取新手教学引导index
    PT_S2S_UPDATE_USE            = 0x10061     #道具使用更新
    PT_S2S_GET_TECHNO            = 0x10062     #获取科技增加百分比
    PT_C2S_FREE_ADD_SPEED        = 0x10063     #免费加速
    PT_S2C_INPUT_ANTI_INFO       = 0x10064     #显示身份证验证框
    PT_S2S_GAG_LOGIN             = 0x10065     #通知封号登陆
    PT_S2S_KICK_OFFLINE          = 0x10066     #通知角色被踢下线
    PT_S2S_GET_BUILD_INFO        = 0x10067     #获取建筑信息
    PT_C2S_REPLACE_ACCOUNT       = 0x10068     #切换账号
    PT_S2S_UPDATE_IDCARD         = 0x10069     #S2S更新IDCARD
    PT_S2S_REFRESH_RECHARGE      = 0x10070     #S2S更新充值信息
    PT_C2S_ROLE_HEART            = 0x10071     #心跳包
    PT_C2S_VIP_ACTIVITE          = 0x10072     #VIP激活 每激活 增加激活天数
    PT_C2S_ADD_VIP_POINT         = 0x10073     #使用道具元宝增加技能点数
    PT_S2C_VIP_INFO              = 0x10074     #更新VIP信息
    PT_S2S_GET_PLUNDER_BFB       = 0x10075     #获取掠夺百分比
    PT_S2S_GET_FORCE_BATT        = 0x10076     #获取部队总兵力
    def __setattr__(self,attr,val):
        raise TypeError("Attribute is readonly")

RoleType = __RoleType()


class __RoleKey(object):
    PK_ROLE_VIP_DAY         = 0x083     #VIP天数
    PK_ROLE_USE_NUM         = 0x084     #使用数量
    PK_VIP_SKILL_ROUND      = 0x085     #VIP技能回合数减少 
    PK_HERO_HUER_BFB        = 0x086     #伤病转化比例
    PK_VIP_ACTIVE_STATUS    = 0x087     #vip激活状态
    PK_GM_REASON            = 0x088     #GM操作原因
    PK_LIMIT_TIME           = 0x089     #限制时间 禁言多长时间,封号多长时间
    PK_HERO_EQUIP_NUM       = 0x090     #武将装备的数量
    PK_RES_DOWNLOAD_URL     = 0x091     #当前服务器资源下载地址
    PK_ROLE_BUFF            = 0x092     #使用后的buffid
    PK_ROLE_GUIDE_STEP      = 0x093     #指引步骤
    PK_HERO_FAME            = 0x094     #武将获得声望
    PK_SKILL_POINT          = 0x095     #科技点数
    PK_ROLE_AID             = 0x096     #同盟ID
    PK_ROLE_WORLD_POS       = 0x097     #野外坐标点
    PK_ROLE_VIP_POINT       = 0x098     #vip点数
    PK_ROLE_VIP_SYTIME      = 0x099     #vip激活剩余天数
    PK_ROLE_ID              = 0x100     #角色ID
    PK_ROLE_SSID            = 0x101     #角色SSID
    PK_ROLE_USERNAME        = 0x102     #帐号
    PK_ROLE_USERPWD         = 0x103     #密码
    PK_ROLE_NICKNAME        = 0x104     #昵称
    PK_ROLE_TOKEN           = 0x105     #角色TOKEN
    PK_ROLE_GOLD            = 0x106     #角色元宝
    PK_ROLE_COIN            = 0x107     #角色铜币
    PK_ROLE_VIP             = 0x108     #角色VIP
    PK_ROLE_SERVER_ID       = 0x109     #角色服务器ID
    PK_ROLE_IS_GM           = 0x110     #是否GM
    PK_ROLE_IS_ONLINE       = 0x111     #是否在线
    PK_ROLE_SEE_UID         = 0x112     #角色可以显示的ID
    PK_ROLE_HONOUR          = 0x113     #角色荣誉
    PK_ROLE_FAME            = 0x114     #角色声望
    PK_ROLE_FOOD            = 0x115     #角色食物
    PK_ROLE_WOOD            = 0x116     #角色木头（林场）
    PK_ROLE_IRON            = 0x117     #角色铁矿
    PK_ROLE_VIP_END_TIME    = 0x118     #VIP到期时间
    PK_ROLE_PLATFORM        = 0x119     #当前角色所在的平台
    PK_ROLE_HEAD_PIC        = 0x120     #头像
    PK_ROLE_ADD_HERO        = 0x121     #添加武将
    PK_ROLE_DEL_ITEM        = 0x122     #删除物品
    PK_ROLE_ADD_ITEM        = 0x123     #添加物品
    PK_LOGS_TYPE            = 0x124     #日志类型
    PK_MINE_TYPE_WOOD       = 0x125     #农田类型
    PK_MINE_TYPE_FOOD       = 0x126     #林场类型
    PK_MINE_TYPE_IRON       = 0x127     #铁矿类型
    PK_LOG_TEXT             = 0x128     #日志详情
    PK_BUILD_ID             = 0x129     #建筑ID
    PK_CD_TIME              = 0x130     #CD倒计时
    PK_BUILD_TYPE           = 0x131     #建筑类型
    PK_BUILD_LEVEL          = 0x132     #建筑等级
    PK_IDCARD_NAME          = 0x133     #身份证姓名
    PK_IDCARD_ID            = 0x134     #身份证号码
    PK_IDCARD_STATUS        = 0x135     #防沉迷状态
    PK_EXCHANGE_MY_TYPE     = 0x136     #兑换类型
    PK_EXCHANGE_SY_TYPE     = 0x137     #兑换类型系统
    PK_EXCHANGE_MY_NUM      = 0x138     #兑换数量
    PK_EXCHANGE_SY_NUM      = 0x139     #兑换数量
    PK_ANTI_STATUS          = 0x140     #兑换数量
    PK_ANTI_SECONDS         = 0x141     #兑换数量
    PK_ANTI_HOURS           = 0x142     #兑换数量
    PK_QUEUE_NUM            = 0x143     #队列数量
    PK_FORCE_NUM            = 0x144     #部队数量
    PK_ROLE_OPERATE         = 0x145     #角色操作类型
    PK_ROLE_INFO            = 0x146     #角色信息
    PK_ROLE_FORCE           = 0x147     #角色所有部队
    PK_ROLE_BUILD           = 0x148     #建筑信息
    PK_ROLE_BIOTECH         = 0x149     #科技信息
    #================================================武将
    PK_HERO_ID              = 0x150     #武将ID
    PK_HERO_HID             = 0x151     #武将模版ID
    PK_HERO_LEVEL           = 0x152     #武将等级
    PK_HERO_STAR            = 0x153     #武将等级
    PK_HERO_EXP             = 0x154     #武将经验
    PK_HERO_HP              = 0x155     #武将体力
    PK_HERO_ATTK            = 0x156     #武将攻击
    PK_HERO_DEFE            = 0x157     #武将防御
    PK_HERO_INTER           = 0x158     #武将谋略
    PK_HERO_PHY             = 0x159     #武将统帅
    PK_HERO_HIT             = 0x160     #武将命中
    PK_HERO_MISS            = 0x161     #武将闪避
    PK_HERO_CIRTICAL        = 0x162     #武将暴击
    PK_HERO_SPEED           = 0x163     #武将速度
    PK_HERO_SKILL           = 0x164     #武将技能
    PK_HERO_SKILL_ID        = 0x165     #技能ID
    PK_HERO_SKILL_LEVEL     = 0x166     #技能等级
    PK_HERO_ATTR            = 0x167     #武将的属性列表
    PK_HERO_HURT_TIME       = 0x168     #武将重伤恢复剩余时间
    PK_HERO_BATT            = 0x169     #武将兵力
    PK_HERO_ALL_BATT        = 0x170     #武将总兵力
    PK_HERO_XINGE           = 0x171     #武将星格
    PK_HERO_TID             = 0x172     #武将目标ID
    PK_HERO_XINGE_IDX       = 0x173     #星格索引
    PK_HERO_FORCE_ID        = 0x174     #部队ID
    PK_HERO_STATUS          = 0x175     #武将状态
    PK_HERO_FETTER          = 0x176     #羁绊的武将ID
    PK_HERO_ISHURT          = 0x177     #是否受伤
    PK_HERO_ISCONS          = 0x178     #是否征兵
    PK_HERO_CONS_TIME       = 0x179     #征兵倒计时
    PK_HERO_CONS_NUM        = 0x180     #征兵数量ridetech
    PK_HERO_ISMARCH         = 0x181     #武将行军中
    PK_HERO_CONS_PACK       = 0x182     #武将征兵列表 {武将ID:征兵数量}
    PK_HERO_BARMS           = 0x183     #武将兵种大类型
    PK_HERO_SARMS           = 0x184     #武将兵种小类型
    PK_HERO_NAME            = 0x185     #武将名字
    PK_HERO_MAIN            = 0x186     #是否主将,0=不是,1=是
    PK_HERO_RANGE           = 0x187     #攻击距离
    PK_HERO_IS_REFINE       = 0x188     #是否修炼0=不是,1=是
    PK_HERO_REFINE_TIME     = 0x189     #修炼剩余时间
    PK_REFINE_TYPE          = 0x190     #修炼类型 1=普通修炼，2=8小时双倍修炼，3=24小时双倍修炼
    PK_REFINE_EXP           = 0x191     #修炼获得经验
    PK_TAXES_NUM            = 0x192     #当前征收次数
    PK_TAXES_TIME           = 0x193     #征收CD倒计时
    PK_TAXES_TOTAL_NUM      = 0x194     #总共征收多少
    PK_TAXES_COMPLATE_TIME  = 0x195     #征收完成时间列表
    PK_TECHNO_TYPE          = 0x196     #科技类型
    PK_TECHNO_LEVEL         = 0x197     #科技等级
    PK_ROLE_LOGIN_TYPE      = 0x198     #登录类型 0=普通登录,1=一键登录
    PK_ROLE_OTHER           = 0x199     #其他通用参数
    PK_ROLE_ALLIES          = 0x200     #同盟
    PK_ROLE_GUIDE_STEP      = 0x201     #新手引导步骤
    PK_CHECK_IDCARD_STATUS  = 0x202     #查询身份证状态
    PK_LOGIN_SERVER_TYPE    = 0x203     #服务器状态

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

RoleKey = __RoleKey()

#建筑类型
class __BuildType(object):
    BUILD_TYPE_TSF      = 1         #太守府
    BUILD_TYPE_JM       = 2         #民居
    BUILD_TYPE_XLT      = 3         #修炼塔
    BUILD_TYPE_SC       = 4         #市场
    BUILD_TYPE_SH       = 5         #商会
    BUILD_TYPE_CK       = 6         #仓库
    BUILD_TYPE_TXY      = 7         #太学院
    BUILD_TYPE_JY       = 8         #军营
    BUILD_TYPE_CQ       = 9         #城墙
    BUILD_TYPE_JC       = 10        #校场
    BUILD_TYPE_HLS      = 11        #鸿胪寺
    BUILD_TYPE_BSGF     = 12        #宝石工坊
    BUILD_TYPE_TJP      = 13        #铁匠铺
    BUILD_TYPE_LC       = 14        #林场
    BUILD_TYPE_LT       = 15        #良田
    BUILD_TYPE_TK       = 16        #铁矿
    BUILD_TYPE_JG       = 17        #酒馆
    BUILD_TYPE_LT       = 18        #擂台
    BUILD_TYPE_CXF      = 19        #丞相府
    BUILD_TYPE_TSM      = 20        #铁神庙
    BUILD_TYPE_TT       = 21        #天坛
    BUILD_TYPE_JY       = 22        #监狱
    BUILD_TYPE_PMC      = 23        #跑马场


BuildType = __BuildType()

#Error codes
class __RoleError(object):
    __slots__ = ()
    ERR_OK                                     = 0                   #正常
    ERR_ROLE_NOT_OFFLINE                       = -1001               #角色不在线
    ERR_CREATE_FAILED                          = -1002               #创建角色失败
    ERR_COIN_DEFIC                             = -1003               #铜币不足
    ERR_GOLD_DEFIC                             = -1004               #元宝不足
    ERR_USER_NO_EXISTS                         = -1005               #帐号已注册
    ERR_USER_NAME_EXISTS                       = -1006               #名字已存在
    ERR_USER_ALAEADY_EXISTS                    = -1007               #帐号已存在
    ERR_USER_NAME_NO_EXISTS                    = -1008               #名字不存在
    ERR_USER_ENBLED_LOGIN                      = -1009               #帐号被禁止登录
    ERR_FAME_DEFIC                             = -1010               #声望不足
    ERR_FOOD_DEFIC                             = -1011               #食物不足
    ERR_WOOD_DEFIC                             = -1012               #木头不足
    ERR_IRON_DEFIC                             = -1013               #铁矿不足
    ERR_HONOUR_DEFIC                           = -1014               #荣誉不足
    ERR_BUILD_HAS_CD                           = -1015               #当前建筑正在队列中
    ERR_CD_QUEUE_REACH                         = -1016               #当前CD对列已满
    ERR_BUILD_NO_EXISTS                        = -1017               #建筑不存在
    ERR_MAX_HIGH_STAR                          = -1018               #当前已是最高星级
    ERR_HERO_XINGE_NO                          = -1019               #武将星格未激活满
    ERR_HERO_IS_NO_EXISTS                      = -1020               #武将不存在
    ERR_HERO_IS_FORCE                          = -1021               #武将出征状态
    ERR_HERO_IS_FETTER                         = -1022               #武将羁绊状态
    ERR_FORCE_IS_NO_EXISTS                     = -1023               #部队不存在
    ERR_FORCE_HERO_ISMAX                       = -1024               #部队人数已达上限
    ERR_HERO_IS_FETTER                         = -1025               #武将羁绊中
    ERR_HERO_SAME_FORCE                        = -1026               #武将在同意部队中
    ERR_HERO_HP_DEFIC                          = -1027               #武将体力不足
    ERR_HERO_REFINEING                         = -1028               #武将修炼中
    ERR_BUILD_LEVEL_ZERO                       = -1029               #当前建筑等级为0
    ERR_HERO_NO_REFINE                         = -1030               #武将没有修炼
    ERR_NO_SAME_FORCE                          = -1031               #不能是相同部队
    ERR_NORMAL_NO_COUNT                        = -1032               #征收普通次数不足
    ERR_TAXES_TIME_NO_END                      = -1033               #征收CD未结束
    ERR_FORCE_NO_COUNT                         = -1034               #征收强制次数不足
    ERR_IS_MAX_LEVEL                           = -1035               #当前已是最大等级
    ERR_BUILD_LEVEL_LACK                       = -1036               #需求的建筑等级不足
    ERR_NO_FRONT_TECHN                         = -1037               #前置科技条件不满足
    ERR_TECHNO_NO_UPGRADE                      = -1038               #无法同时升级科技
    ERR_HERO_HAS_FORCE                         = -1039               #当前武将在部队中
    ERR_HERO_NO_FETTER                         = -1040               #当前武将没有羁绊
    ERR_HERO_MAX_LIMIT                         = -1041               #武将数量已达上限
    ERR_HERO_IS_MARCH                          = -1042               #武将行军状态无法互换
    ERR_FORCE_HERO_ISHAS                       = -1043               #阵形上有同种类型的武将
    ERR_SKILL_POINT_DEFIC                      = -1044               #技能点不足
    ERR_SKILL_NO_ACTIVE                        = -1045               #技能未激活
    ERR_NO_SAME_HERO_CARD                      = -1046               #同名武将卡不足
    ERR_NO_ACCOUNT_REPLACE                     = -1047               #您的账号在其他地方登陆
    ERR_VIP_NO_POINT                           = -1048               #技能点不足
    ERR_LOGIN_FULL                             = -1049               #登录爆满           
    pass

RoleError = __RoleError()


class RolePack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = RoleKey
    def __init__(self, data=None):
        super(RolePack, self).__init__(data)

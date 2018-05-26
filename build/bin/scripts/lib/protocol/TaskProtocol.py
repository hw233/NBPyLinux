#coding: utf-8

from CommonProtocol import PacketType, PacketType, PacketSchema, GeneralPack

class __TaskType(object):
    
    PT_C2S_GET_TASK_LIST        = 0x30001  #任务列表
    PT_S2C_TASK_COMPLATE        = 0x30002  #任务完成
    PT_S2C_TASK_UPDATE          = 0x30003  #任务更新
    PT_C2S_TASK_GET_REWARD      = 0x30004  #任务获取奖励
    PT_S2S_UPDATE_TASK_COND     = 0x30005  #更新某个种类的任务条件
    PT_S2C_TASK_RESET           = 0x30006  #重置日常任务

    PT_C2S_GET_EVENT_LIST       = 0x30006  #事件列表
    PT_S2C_UPDATE_EVENT         = 0x30007  #更新事件
    PT_C2S_EVENT_ADD_SPEED      = 0x30008  #事件加速
    PT_S2C_EVENT_RECALL         = 0x30009  #事件召回
    PT_S2S_CREATE_EVENT         = 0x30010  #创建事件
    PT_S2S_NEW_EVENT            = 0x30011  #有新的事件
    PT_S2S_COMPLATE_EVENT       = 0x30012  #完成某个事件
    PT_C2S_SET_TASK_INFO        = 0x30013  #特殊任务设置及新手步骤设置 

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

TaskType = __TaskType()


class __TaskKey(object):
    
    PK_ROLE_ID          = 0x300          #角色ID
    PK_ROLE_SSID        = 0x301          #角色SSID
    PK_ROLE_TOKEN       = 0x302          #角色TOKEN
    PK_ROLE_NAME        = 0x303          #角色名称
    PK_TASK_ID          = 0x304          #任务ID
    PK_TASK_TYPE        = 0x305          #任务类型（1=主线,2=支线,3=日常）
    PK_TASK_PACK        = 0x306          #列表包
    PK_TASK_PROGRESS    = 0x307          #进度
    PK_TASK_COND        = 0x308          #完成条件
    PK_TASK_STATUS      = 0x309          #任务完成状态（0：未完成，1：已完成，2：已领取奖励）
    PK_TASK_MODEL       = 0x310          #任务小类型
    PK_ROLE_RNAME       = 0x311          #对方的名字
    PK_TASK_LEVEL       = 0x312          #任务目标等级
    PK_TASK_NUM         = 0x313          #任务目标数量
    PK_TASK_NID         = 0x314          #任务目标ID
    
    PK_EVENT_ID         = 0x350          #事件ID
    PK_EVENT_OPER       = 0x351          #事件加速操作,1使用道具加速,2=使用元宝加速
    PK_EVENT_NEW_DEL    = 0x352          #0=移除,1=新增
    PK_EVENT_PACK       = 0x353          #事件包
    PK_FORCE_ID         = 0x354          #部队ID
    PK_EVENT_POS        = 0x355          #坐标
    PK_EVENT_TYPE       = 0x356          #事件类型
    PK_EVENT_H_TYPE     = 0x357          #
    PK_EVENT_DIS        = 0x358          #距离
    PK_EVENT_ENDTIME    = 0x359          #事件结束时间
    PK_EVENT_SYTIME     = 0x360          #事件剩余时间
    PK_EVENT_STATUS     = 0x361          #当前事件状态
    PK_ROLE_TRID        = 0x362          #目标角色ID
    PK_EVENT_HERO_NAME  = 0x363          #当前武将名字
    PK_EVENT_OTHER      = 0x364          #附加参数
    PK_IS_GUIDE         = 0x365          #是否显示新手指引0=显示,1=不显示
    PK_EVENT_NAME       = 0x366          #事件名字 
        
    

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

TaskKey = __TaskKey()

#Error codes
class __TaskError(object):

    __slots__ = ()

    ERR_OK                                 = 0                     #正常
    ERR_INVALID_PARAM                      = -1101                 #参数错误
    ERR_TASK_NO_EXISTS                     = -1102                 #任务不存在
    ERR_HAS_GET_REWARD                     = -1103                 #已经领取
    ERR_REWARD_IS_ILLEGEA                  = -1104                 #取得的任务物品错误
    ERR_EVENT_NO_EXISTS                    = -1105                 #事件不存在
    ERR_TASK_NOT_COMPLATE                  = -1106                 #任务未完成  
    
TaskError = __TaskError()

class __TaskModel(object):
    '''任务类型'''
    __slots__ = ()

    MODEL_JMJ              = 1     #建民居              (无参数)
    MODEL_XLT              = 2     #建修炼塔            (无参数)
    MODEL_JSC              = 3     #建市场              (无参数)
    MODEL_JSH              = 4     #建商会              (无参数)
    MODEL_JCK              = 5     #建仓库              (无参数)
    MODEL_TXY              = 6     #建太学院            (无参数)
    MODEL_JJY              = 7     #建军营              (无参数)
    MODEL_HLS              = 8     #建鸿胪寺            (无参数)
    MODEL_BSF              = 9     #建宝石坊            (无参数)
    MODEL_JJC              = 10    #建校场              (无参数)
    MODEL_ZBF              = 11    #建装备工坊          (无参数)
    MODEL_JLC              = 12    #建林场              (特殊)     当前等级的林场及总数 参数格式 "等级_数量"
    MODEL_JLT              = 13    #建良田              (特殊)     当前等级的良田及总数 参数格式 "等级_数量"
    MODEL_JTK              = 14    #建铁矿              (特殊)     当前等级的铁矿及总数 参数格式 "等级_数量"
    MODEL_MCL              = 15    #木材产量            (有参数)   产量值(在升级后产量提升 通知任务逻辑服)
    MODEL_LCL              = 16    #良田产量            (有参数)   产量值(在升级后产量提升 通知任务逻辑服)
    MODEL_TCL              = 17    #铁矿产量            (有参数)   产量值(在升级后产量提升 通知任务逻辑服)
    MODEL_SJ_TSF           = 18    #升级太守府          (有参数)   传递等级
    MODEL_SJ_MJ            = 19    #升级民居            (有参数)   传递等级
    MODEL_SJ_XLT           = 20    #升级修炼塔          (有参数)   传递等级
    MODEL_SJ_SC            = 21    #升级市场            (有参数)   传递等级
    MODEL_SJ_SH            = 22    #升级商会            (有参数)   传递等级
    MODEL_SJ_CK            = 23    #升级仓库            (有参数)   传递等级
    MODEL_SJ_TXY           = 24    #升级太学院          (有参数)   传递等级
    MODEL_SJ_JY            = 25    #升级军营            (有参数)   传递等级
    MODEL_SJ_HLS           = 26    #升级鸿胪寺          (有参数)   传递等级
    MODEL_SJ_BSF           = 27    #升级宝石坊          (有参数)   传递等级
    MODEL_SJ_LC            = 28    #升级林场            (有参数)   传递等级
    MODEL_SJ_LT            = 29    #升级良田            (有参数)   传递等级
    MODEL_SJ_TK            = 30    #升级铁矿            (有参数)   传递等级
    MODEL_SJ_JC            = 31    #升级校场            (有参数)   传递等级
    MODEL_SJ_CQ            = 32    #升级城墙            (有参数)   传递等级
    MODEL_SJ_ZBF           = 33    #升级装备坊          (有参数)   传递等级
    MODEL_CJ_ZYD           = 34    #采集                (无参数)   
    MODEL_LD_ZYD           = 35    #掠夺资源点          (无参数)
    MODEL_ZC_CC            = 36    #侦查城池            (无参数)
    MODEL_LD_CC            = 37    #掠夺城池            (无参数)
    MODEL_TS               = 38    #通商                (有参数)  传递怪物等级 59 挂钩
    MODEL_KM               = 39    #打怪                (无参数)
    MODEL_WJ_SJ            = 40    #武将升级            (有参数)  传递等级 60 挂钩
    MODEL_WJ_XL            = 41    #武将修炼            (无参数)
    MODEL_JN_SJ            = 42    #技能升级            (有参数)  传递技能等级
    MODEL_KJ_SJ            = 43    #科技升级            (有参数)  传递科技等级  
    MODEL_WJ_BF            = 44    #武将拜访            (无参数)
    MODEL_CQ_WJ            = 45    #抽取武将            (无参数)
    MODEL_HC_ZB            = 46    #合成装备            (无参数)
    MODEL_HC_BS            = 47    #合成宝石            (无参数)
    MODEL_TG_FB            = 48    #通关副本            (无参数)
    MODEL_DH_ZY            = 49    #兑换资源            (无参数)
    MODEL_JJC              = 50    #竞技场              (无参数)
    MODEL_HYS              = 51    #添加好友数量        (有参数)  传递当前好友数量
    MODEL_LD_MC            = 52    #掠夺木材数量        (有参数)  当前掠夺数量
    MODEL_LD_LS            = 53    #掠夺粮食数量        (有参数)  当前掠夺数量
    MODEL_LD_TK            = 54    #掠夺铁矿数量        (有参数)  当前掠夺数量
    MODEL_CJ_MC            = 55    #采集木材数量        (有参数)  当前掠夺数量
    MODEL_CJ_LS            = 56    #采集木材数量        (有参数)  当前掠夺数量
    MODEL_CJ_TK            = 57    #采集木材数量        (有参数)  当前掠夺数量
    MODEL_LJ_ZB            = 58    #累计征兵数量        (有参数)  总的数量
    MODEL_JS_GW            = 39    #击杀野怪等级        (有参数)  传递怪物等级
    MODEL_SJ_WJ            = 40    #升级武将到多少等级  (有参数)  传递参数 等级
    MODEL_HC_BS            = 47    #合成宝石到多少级    (有参数)  传递参数 等级
    MODEL_HC_ZB            = 46    #合成装备到多少级    (有参数)  传递参数 等级
    MODEL_XX_KJ            = 65    #学习固定ID科技      (有参数)  传递科技ID
    MODEL_SJ_KJ            = 43    #升级科技到多少等级  (有参数)  科技等级  
    MODEL_ZB               = 67    #征兵                (无参数)
    MODEL_BD               = 68    #编队                (无参数)
    MODEL_PUB              = 69    #酒馆五连抽          (无参数)
    MODEL_TS_GZ            = 70    #提升官职            (有参数)  当前官职
    MODEL_MR_DL            = 71    #每日登录            (无参数)  
    MODEL_TG_FB            = 72    #通关副本            (有参数)  当前副本ID
    MODEL_ZC               = 73    #征收                (有参数)  征收数       
TaskModel = __TaskModel()

class TaskPack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = TaskKey
    def __init__(self, data=None):
        super(TaskPack, self).__init__(data)
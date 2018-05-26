#coding: utf-8

class LogType(object):
    PLUS_EQUIPMENT       = '强化装备'
    UPGRADE_PLAYER       = '升级球员'
    BAG_ADD_SPACE        = '包裹扩容'
    TREASURE_HUNT        = '寻宝'
    UPGRADE_FORMATION    = '阵型升级'
    BUY_FATIGUE          = '购买体力'
    SYS_ADD_FATIGUE      = '系统增加'
    SHOP_BUY             = '商城购买'
    SIGN                 = '签到'
    TASK                 = '任务'
    COPY                 = '荣誉副本'
    ARENA                = '竞技场'
    HIRE_COASH           = '雇佣教练'
    RECHARGE             = '充值'
    PLAYER_UPGRADE       = '球员升级'
    GM_ADD               = 'GM添加'
    VIP                  = 'VIP奖励'
    CHECK_USE            = '支票使用'
    SALE_ITEM            = '出售物品'
    MAIL_ITEM            = '邮件附件'


#花费流向
import time
from datetime import datetime,timedelta
class costlogs(object):
    def __init__(self, role, logtype='' ,costcp = 0, costzs=0, costhr=0, costfatigue = 0, costexp=0, curexp=0):
        super(costlogs, self).__init__()
        self.uid                = role.uid                                  #角色ID
        self.pid                = 0                                         #日志ID
        self.description        = (logtype == "") and '未知' or logtype     #描述
        self.cost_cp            = int(costcp)                               #花费的钞票(正数,负数)
        self.cost_zs            = int(costzs)                               #花费的钻石(正数,负数)
        self.cost_hr            = int(costhr)                               #花费的荣誉(正数,负数)
        self.cost_exp           = int(costexp)                              #经验(正数,负数)
        self.cost_fa            = int(costfatigue)                          #体力
        self.cur_cp             = int(role.coin)                            #当前钞票
        self.cur_zs             = int(role.gold)                            #当前钻石
        self.cur_hr             = int(role.honour)                          #当前荣誉
        self.cur_exp            = curexp                                    #离升级经验
        self.cur_lv             = int(role.level)                           #当前等级
        self.cur_fa             = int(role.fatigue)                         #当前体力
        self.is_ok              = 1                                         #是否成功（1:是,0:否）
        self.created_at         = time.strftime("%Y-%m-%d %H:%M:%S")        #创建时间

    @property
    def t_keys(self):
        return str(tuple(self.__dict__.keys())).replace('\'','`').replace('(','').replace(')','')

    @property
    def t_values(self):
        tkeys = self.__dict__.keys()
        valuestr = ''
        for k in tkeys:
            value = self.__getattribute__(k)
            if tkeys.index(k) == 0:
                valuestr += "\'%s\'"%str(value)
            else:
                valuestr += ",\'%s\'"%str(value)
        return valuestr

#球员日志
class playerlogs(object):
    def __init__(self, role, player, gold=0, coin=0, description=0):
        super(playerlogs, self).__init__()
        self.uid                = role.uid                                  #角色ID
        self.pname              = player.name                               #球员名字
        self.gs                 = 0                                         #1=正选,2=替补(暂时不做)
        self.pos                = int(player.posid)                         #球员位置
        self.coin               = int(coin)                                 #消耗钞票
        self.gold               = int(gold)                                 #消耗暂时
        self.description        = int(description)                          #获取多少经验
        self.is_ok              = 1                                         #
        self.created_at         = time.strftime("%Y-%m-%d %H:%M:%S")        #创建时间

    @property
    def t_keys(self):
        return str(tuple(self.__dict__.keys())).replace('\'','`').replace('(','').replace(')','')
    @property
    def t_values(self):
        tkeys = self.__dict__.keys()
        valuestr = ''
        for k in tkeys:
            value = self.__getattribute__(k)
            if tkeys.index(k) == 0:
                valuestr += "\'%s\'"%str(value)
            else:
                valuestr += ",\'%s\'"%str(value)
        return valuestr

#球队日志
class teamlogs(object):
    def __init__(self, frole, nrole):
        super(teamlogs, self).__init__()
        self.uid                = frole.uid                                 #角色ID
        self.flevel             = int(frole.level)                          #升级前等级
        self.nlevel             = int(nrole.level)                          #升级后等级
        self.fvit               = int(frole.fatigue)                        #升级前体力
        self.nvit               = int(nrole.fatigue)                        #升级后体力
        self.is_ok              = 1
        self.created_at         = time.strftime("%Y-%m-%d %H:%M:%S")        #创建时间

    @property
    def t_keys(self):
        return str(tuple(self.__dict__.keys())).replace('\'','`').replace('(','').replace(')','')

    @property
    def t_values(self):
        return str(tuple(self.__dict__.values())).replace('(','').replace(')','')



#比赛日志
class matchlogs(object):
    def __init__(self, role, otype=0, mtype='', gname='', score='',items="", is_suc=0, exp=0, honour=0):
        super(matchlogs, self).__init__()
        self.uid                = role.uid                                  #角色ID
        self.otype              = otype                                     #0:进入/1:离开
        self.mtype              = mtype                                     #荣誉之战/竞技场
        self.gname              = gname                                     #挑战对象
        self.score              = score                                     #比分
        self.items              = items                                     #奖励的物品
        self.is_suc             = is_suc                                    #是否胜利
        self.exp                = int(exp)                                  #奖励的经验
        self.hor                = int(honour)                               #奖励的荣誉
        self.is_ok              = otype                                     #
        self.created_at         = time.strftime("%Y-%m-%d %H:%M:%S")        #创建时间

    @property
    def t_keys(self):
        return str(tuple(self.__dict__.keys())).replace('\'','`').replace('(','').replace(')','')

    @property
    def t_values(self):
        tkeys = self.__dict__.keys()
        valuestr = ''
        for k in tkeys:
            value = self.__getattribute__(k)
            if tkeys.index(k) == 0:
                valuestr += "\'%s\'"%str(value)
            else:
                valuestr += ",\'%s\'"%str(value)
        return valuestr


#阵型日志
class formationlogs(object):
    def __init__(self, role, formation, coin=0, exp=0):
        super(formationlogs, self).__init__()
        self.uid                = role.uid                                  #角色ID
        self.fname              = formation.name                            #阵型名字
        self.is_cur             = formation.iscur                           #是否为当前阵型
        self.coin               = int(coin)                                 #消耗铜币
        self.description        = int(exp)                                  #获取经验
        self.created_at         = time.strftime("%Y-%m-%d %H:%M:%S")        #创建时间


    @property
    def t_keys(self):
        return str(tuple(self.__dict__.keys())).replace('\'','`').replace('(','').replace(')','')

    @property
    def t_values(self):
        tkeys = self.__dict__.keys()
        valuestr = ''
        for k in tkeys:
            value = self.__getattribute__(k)
            if tkeys.index(k) == 0:
                valuestr += "\'%s\'"%str(value)
            else:
                valuestr += ",\'%s\'"%str(value)
        return valuestr

#登录日志
class loginlogs(object):
    def __init__(self, row):
        super(loginlogs, self).__init__()
        self.id                 = int(row.get('id',0))                                          #表自增ID
        self.uid                = int(row.get('uid',0))                                         #角色ID
        self.online_time        = int(row.get('online_time',0))                                 #当天在线时间
        self.cl_days            = int(row.get('cl_days',0))                                     #连续登录天数
        self.is_online          = int(row.get('is_online',0))                                   #1:是/0:否在线
        self.created_at         = row.get('created_at') and row.get('created_at').strftime("%Y-%m-%d %H:%M:%S") or time.strftime("%Y-%m-%d %H:%M:%S")      #创建时间
        self.updated_at         = row.get('updated_at') and row.get('updated_at').strftime("%Y-%m-%d %H:%M:%S") or time.strftime("%Y-%m-%d %H:%M:%S")       #更新时间
        pass

    @property
    def t_keys(self):
        tkeys = self.__dict__.keys()
        tkeys.remove('id')
        return str(tuple(tkeys)).replace('\'','`').replace('(','').replace(')','')

    @property
    def t_values(self):
        tkeys = self.__dict__.keys()
        tkeys.remove('id')
        valuestr = ''
        for k in tkeys:
            value = self.__getattribute__(k)
            if tkeys.index(k) == 0:
                valuestr += "\'%s\'"%str(value)
            else:
                valuestr += ",\'%s\'"%str(value)
        return valuestr

    @property
    def u_values(self):
        tkeys = self.__dict__.keys()
        tkeys.remove('id')
        valuestr = ''
        for k in tkeys:
            value = self.__getattribute__(k)
            if tkeys.index(k) == 0:
                valuestr += "`%s`=\'%s\'"%(k,str(value))
            else:
                valuestr += ",`%s`=\'%s\'"%(k,str(value))
        return valuestr

#装备日志
class equiplogs(object):
    def __init__(self, role, equip, playerid, store=0, coin=0, is_ok=0, is_safe=0):
        super(equiplogs, self).__init__()
        self.uid                = role.UID                                      #角色ID
        self.pid                = int(playerid)                                      #穿戴球员ID
        self.iname              = equip.itemtmp.name                            #装备名称
        self.pos                = int(equip.itemtmp.part)                       #装备部位
        self.cost_store         = int(store)                                    #消耗道具
        self.cost_coin          = int(coin)                                     #消耗钞票
        if int(is_ok):
            self.level = int(equip.plus)                                        #升级目标等级
        else:
            if int(is_safe):
                self.level = int(equip.plus)+1
            else:
                self.level = int(equip.plus)+2
        self.rate               = equip.preplusinfo and int(equip.preplusinfo.get('persent', 0)) or 100      #强化成功率
        self.add                = equip.preplusinfo and (equip.itemtmp.text[:6]+' +'+ str(equip.preplusinfo.get('add', 0))) or (equip.itemtmp.text[:6]+' +'+ str(equip.preaddprop))  #强化增加值
        self.is_ok              = int(is_ok)                                    #是否强化成功（1：是，0：否）
        self.created_at         = time.strftime("%Y-%m-%d %H:%M:%S")            #创建时间

    @property
    def t_keys(self):
        return str(tuple(self.__dict__.keys())).replace('\'','`').replace('(','').replace(')','')

    @property
    def t_values(self):
        tkeys = self.__dict__.keys()
        valuestr = ''
        for k in tkeys:
            value = self.__getattribute__(k)
            if tkeys.index(k) == 0:
                valuestr += "\'%s\'"%str(value)
            else:
                valuestr += ",\'%s\'"%str(value)
        return valuestr

#留存日志
class retentionlogs(object):
    def __init__(self, row={}):
        super(retentionlogs, self).__init__()
        self.id                 = row.get('id',0)
        self.date_time          = row.get('date_time') and row.get('date_time').strftime("%Y-%m-%d %H:%M:%S") or ((datetime.now()+ timedelta(days = -1)).strftime("%Y-%m-%d") + ' 00:00:00')              #统计日期
        self.new_num            = row.get('new_num',0)                          #新增人数
        self.one_num            = row.get('one_num',0)                          #一天留存人数
        self.two_num            = row.get('two_num',0)                          #二天留存人数
        self.three_num          = row.get('three_num',0)                        #三天留存人数
        self.four_num           = row.get('four_num',0)                         #四天留存人数
        self.five_num           = row.get('five_num',0)                         #五天留存人数
        self.six_num            = row.get('six_num',0)                          #六天留存人数
        self.fourteen_num       = row.get('fourteen_num',0)                     #十四天留存人数
        self.thirty_num         = row.get('thirty_num',0)                       #三十天留存人数

    @property
    def t_keys(self):
        tkeys = self.__dict__.keys()
        tkeys.remove('id')
        return str(tuple(tkeys)).replace('\'','`').replace('(','').replace(')','')

    @property
    def t_values(self):
        tkeys = self.__dict__.keys()
        tkeys.remove('id')
        valuestr = ''
        for k in tkeys:
            value = self.__getattribute__(k)
            if tkeys.index(k) == 0:
                valuestr += "\'%s\'"%str(value)
            else:
                valuestr += ",\'%s\'"%str(value)
        return valuestr

    @property
    def u_values(self):
        tkeys = self.__dict__.keys()
        tkeys.remove('id')
        valuestr = ''
        for k in tkeys:
            value = self.__getattribute__(k)
            if tkeys.index(k) == 0:
                valuestr += "`%s`=\'%s\'"%(k,str(value))
            else:
                valuestr += ",`%s`=\'%s\'"%(k,str(value))
        return valuestr

    def init_data(self, row):
        tkeys = self.__dict__.keys()
        for k in tkeys:
            value = self.__getattribute__(k)
            if k in ['id','date_time','new_num'] and value:continue
            self.__setattr__(k, row.get(k,0))
        pass

#总数据日志
class recordlogs(object):
    def __init__(self, row={}):
        super(recordlogs, self).__init__()
        self.join_game_count        = 0 #进入游戏数量
        self.create_user            = 0 #当天创建的用户数量
        self.max_online             = 0 #最高在线
        self.mean_online            = 0 #平均在线
        self.max_level              = 0 #最高等级
        self.recharge_user_count    = 0 #充值人数
        self.new_add_recharge       = 0 #新增充值数量
        self.total_recharge_count   = 0 #总共充值次数
        self.recharge_money         = 0 #充值金额
        self.apru                   = 0 #充值apru
        self.consume_gold           = 0 #消费总金券量
        self.created_at             = (datetime.now()+ timedelta(days = -1)).strftime("%Y-%m-%d") + ' 00:00:00'        #创建时间
        if row:
            self.init_data(row)


    @property
    def t_keys(self):
        return str(tuple(self.__dict__.keys())).replace('\'','`').replace('(','').replace(')','')

    @property
    def t_values(self):
        tkeys = self.__dict__.keys()
        valuestr = ''
        for k in tkeys:
            value = self.__getattribute__(k)
            if tkeys.index(k) == 0:
                valuestr += "\'%s\'"%str(value)
            else:
                valuestr += ",\'%s\'"%str(value)
        return valuestr

    def init_data(self, row):
        tkeys = self.__dict__.keys()
        for k in tkeys:
            if k == 'created_at': continue
            self.__setattr__(k, row.get(k,0))
        if self.recharge_user_count:
            self.apru = round(float(self.recharge_money)/float(self.recharge_user_count),4)
        pass

#阵型日志
class tasklogs(object):
    def __init__(self, role, tid, is_ok=0):
        super(tasklogs, self).__init__()
        self.uid                = role.UID                                  #角色ID
        self.tid                = int(tid)                                  #任务ID
        self.is_ok              = int(is_ok)                                #1:是/0:否成功
        self.created_at         = time.strftime("%Y-%m-%d %H:%M:%S")        #创建时间


    @property
    def t_keys(self):
        return str(tuple(self.__dict__.keys())).replace('\'','`').replace('(','').replace(')','')

    @property
    def t_values(self):
        tkeys = self.__dict__.keys()
        valuestr = ''
        for k in tkeys:
            value = self.__getattribute__(k)
            if tkeys.index(k) == 0:
                valuestr += "\'%s\'"%str(value)
            else:
                valuestr += ",\'%s\'"%str(value)
        return valuestr

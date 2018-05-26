#!coding:utf-8
#!/usr/bin/python

import time
import stackless
from datetime             import datetime,timedelta
from protocol.WarProtocol import *

#buff 模板
class BuffTemp(object):
    def __init__(self, row=None):
        super(BuffTemp, self).__init__()
        self.id              = 0               #buff ID
        self.name            = ""              #buff 名字
        self.team            = 0               #buff 组号
        self.level           = 0               #等级
        self.round           = 0               #持续回合数
        self.effect          = 0               #BUFF生效时刻 1=立即生效,2=行动时生效
        self.settlement      = 0               #结算时间 1=按大回合结算,2行动时结算
        self.type1           = 0               #buff类型1
        self.type2           = 0               #buff类型2
        self.type3           = 0               #buff类型3
        self.type4           = 0               #buff类型4
        self.attribute1      = 0               #属性加成1
        self.attribute2      = 0               #属性加成2
        self.attribute3      = 0               #属性加成3
        self.attribute4      = 0               #属性加成4
        self.mutex           = ""              #特殊互斥buff组
        self.dead            = 0               #死亡后buff是否消失,0=不消失,1=消失
        self.dispel          = 0               #是否驱散,1=可以驱散,2=不可驱散
        self.special         = 0               #buff对应的特效ID
        self.mRound          = 0               #当前回合
        self.hero            = None            #释放Buff的武将 
        self.__init_data(row)
        pass

    def __init_data(self, row):
        self.id              = int(row["id"])               
        self.name            = str(row["name"])                 
        self.team            = int(row["team"])               
        self.level           = int(row["level"])               
        self.round           = int(row["round"])                
        self.effect          = int(row["effect"])               
        self.settlement      = int(row["settlement"])                
        self.type1           = int(row["type_1"])               
        self.type2           = int(row["type_2"])              
        self.type3           = int(row["type_3"])              
        self.type4           = int(row["type_4"])               
        self.attribute1      = str(row["attribute_1"])               
        self.attribute2      = str(row["attribute_2"])               
        self.attribute3      = str(row["attribute_3"])               
        self.attribute4      = str(row["attribute_4"])               
        self.mutex           = str(row["mutex"])              
        self.dead            = int(row["dead"])               
        self.dispel          = int(row["dispel"])              
        self.special         = int(row["special"])               
        pass

    #计算固定值
    def get_attr_info(self, bufftype=0):
        v = 0
        for i in xrange(1, 5):
            tp  = self.__getattribute__("type%s"%str(i))
            val = self.__getattribute__("attribute%s"%str(i))
            if tp != bufftype: continue
            v += int(val)
        return v

    #计算属性百分比值
    def get_add_attr_info(self, bufftype=0):
        v = 0
        for i in xrange(1, 5):
            tp  = self.__getattribute__("type%s"%str(i))
            val = self.__getattribute__("attribute%s"%str(i))
            if tp != bufftype: continue
            v += float(val)
        return v

    #获取某个属性百分比
    def get_attr_bfb_info(self, bufftype=0):
        buffinfo = {}
        for i in xrange(1, 5):
            tp  = self.__getattribute__("type%s"%str(i))
            val = self.__getattribute__("attribute%s"%str(i))
            if tp != bufftype: continue
            _k, _v  = val.split(";")
            buffinfo[int(_k)] = float(_v)
        return buffinfo

    @property
    def type(self):
        return self.effect

    @property
    def mutexlist(self):
        #buff互斥列表
        try:
            mutexlist = str(self.mutex).split(";")
            mutexlist = [int(i) for i in mutexlist] 
            return mutexlist
        except Exception,ex:
            return []
    
    @property
    def Attack(self):
        #buff 增加攻击值
        return self.get_attr_info(1)

    @property
    def Defe(self):
        #buff 增加防御值
        return self.get_attr_info(2)


    @property
    def Inter(self):
        #buff 增加谋略值
        return self.get_attr_info(3)

        
    @property
    def Phy(self):
        #buff 增加统帅值
        return self.get_attr_info(4)

    
    @property
    def Critical(self):
        #buff 增暴击帅值
        return self.get_attr_info(5)

    @property
    def Miss(self): 
        #buff 增闪避帅值
        return self.get_attr_info(6)

    @property
    def Speed(self):
        #buff 增速度值
        return self.get_attr_info(7)

    @property
    def Hit(self): 
        #buff 增命中值
        return self.get_attr_info(8)

    @property
    def Batt(self): 
        #buff 增命兵力
        return 0

    #===================================================buff 增加 属性加成
    @property
    def addAttack(self): 
        #buff 增加攻击值百分比
        return self.get_add_attr_info(9)

    @property
    def addDefe(self): 
        #buff 增加防御值百分比
        return self.get_add_attr_info(10)


    @property
    def addInter(self):
        #buff 增加谋略值百分比
        return self.get_add_attr_info(11)

    @property
    def addPhy(self): 
        #buff 增加统帅值百分比
        return self.get_add_attr_info(12)

    @property
    def addCritical(self):
        #buff 增暴击帅值百分比
        return self.get_add_attr_info(13)
        
    @property
    def addMiss(self): 
        #buff 增闪避帅值百分比
        return self.get_add_attr_info(14)

    @property
    def addSpeed(self):
        #buff 增速度值百分比
        return self.get_add_attr_info(15)

    @property
    def addHit(self):
        #buff 增命中值百分比
        return self.get_add_attr_info(16)

    @property
    def SkillHurt(self):
        #技能伤害百分比百分比
        return self.get_add_attr_info(17)

    @property
    def NormalHurt(self):
        #普通伤害百分比百分比
        return self.get_add_attr_info(18)

    @property
    def AllHurt(self):
        #所有伤害百分比百分比
        return self.get_add_attr_info(19)

    @property
    def mSkillHurt(self):
        #所受技能伤害百分比
        return self.get_add_attr_info(20)

    @property
    def mNormalHurt(self):
        #所受普通伤害百分比百分比
        return self.get_add_attr_info(21)

    @property
    def mAllHurt(self):
        #所受所有伤害百分比百分比
        return self.get_add_attr_info(22)
    
    @property
    def nMiss(self):
        #闪避一次攻击
        v = False
        for i in xrange(1, 5):
            tp  = self.__getattribute__("type%s"%str(i))
            if tp == 23: v = True
        return v
    
    @property
    def SuckBlood(self): #吸血百分比
        #普通攻击可以恢复5%的兵力
        return self.get_add_attr_info(24)

    @property
    def nHurtBatt(self):
        #伤害战斗百分比 越战越勇 (2-自身兵力百分比换算成造成伤害倍数)
        v = False
        for i in xrange(1, 5):
            tp  = self.__getattribute__("type%s"%str(i))
            if tp == 25: v = True
        return v

    #获取越战越勇可以造成的伤害倍数 (单个武将)
    def get_batt_hurt(self, sybatt=0):
        return 2 - sybatt

    @property
    def DoubleHit(self):
        #武将一回合内的连击次数
        return self.get_attr_info(26)

    def get_coat_absorb(self):
        #盾的吸收值
        v = 0
        for i in xrange(1, 5):
            tp   = self.__getattribute__("type%s"%str(i))
            info = self.__getattribute__("attribute%s"%str(i))
            if tp != 27: continue
            bufflist   = info.split(";")
            _tp, _bfb  = int(bufflist[0]), float(bufflist[1])
            attr = self.hero.get_attr_info(_tp)
            v += attr * _bfb
        return v
     
    #每回合兵力变化值
    def get_batt_info(self):
        v = 0
        #每回合兵力恢复总值
        for i in xrange(1, 5):
            tp   = self.__getattribute__("type%s"%str(i))
            info = self.__getattribute__("attribute%s"%str(i))
            if tp != 28: continue
            bufflist   = info.split(";")
            _tp, _bfb  = int(bufflist[0]), float(bufflist[1])
            attr = self.hero.get_attr_info(_tp)
            v += attr * _bfb
        return v 
        
    @property
    def IgnoreDefe(self):
        #无视防御
        v = False
        for i in xrange(1, 5):
            tp  = self.__getattribute__("type%s"%str(i))
            if tp == 29: v = True
        return v

    @property
    def SputteHurt(self):
        #溅射伤害百分比
        return self.get_add_attr_info(30)


    def get_attack_batt(self):
        #被攻击兵力变化值
        v = 0
        for i in xrange(1, 5):
            tp   = self.__getattribute__("type%s"%str(i))
            info = self.__getattribute__("attribute%s"%str(i))
            if tp != 31: continue
            bufflist   = info.split(";")
            _tp, _bfb  = int(bufflist[0]), float(bufflist[1])
            attr = self.hero.get_attr_info(_tp)
            v += attr * _bfb
        return v
        
    @property
    def Silent(self):
        #buff 是否沉默
        v = False
        for i in xrange(1, 5):
            tp  = self.__getattribute__("type%s"%str(i))
            if tp == 32: v = True
        return v

    @property
    def Vertigo(self):
        #buff 是否眩晕
        v = False
        for i in xrange(1, 5):
            tp  = self.__getattribute__("type%s"%str(i))
            if tp == 33: v = True
        return v

    
    @property
    def AllowAttack(self):
        #buff 禁止普通攻击
        v = False
        for i in xrange(1, 5):
            tp  = self.__getattribute__("type%s"%str(i))
            if tp == 34: v = True
        return v

    @property
    def AllowSkill(self):
        #buff 禁止全技能
        v = False
        for i in xrange(1, 5):
            tp  = self.__getattribute__("type%s"%str(i))
            if tp == 35: v = True
        return v

    @property
    def Chaos(self):
        #buff 是否混乱 无视敌我进行普通攻击和释放技能
        v = False
        for i in xrange(1, 5):
            tp  = self.__getattribute__("type%s"%str(i))
            if tp == 36: v = True
        return v

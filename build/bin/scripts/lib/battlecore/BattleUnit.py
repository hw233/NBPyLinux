#!coding:utf-8
#!/usr/bin/python

import copy, random, math
import stackless, time, uuid
from protocol.BattleProtocol        import *
from protocol.RoleProtocol          import *
from lib.BaseFunc                   import *
from GameGlobalConfig               import GameGlobalConfig

SKILL_TYPE_ZHIHUI    = 0   #指挥技能
SKILL_TYPE_ZHUDONG   = 1   #主动技能
SKILL_TYPE_NORMAL    = 2   #普通攻击
SKILL_TYPE_ZHUIJI    = 3   #追击

class BattleUnit(object):
    '''战斗单元'''
    def __init__(self, mBattleConfig = None):
        self.iid            = 0        # 武将ID
        self.pos            = 0        # 位置
        self.hid            = 0        # 模版ID
        self.hp             = 0        # 武将体力
        self.level          = 0        # 武将的等级
        self.exp            = 0        # 武将经验
        self.name           = 0        # 武将名字
        self.star           = 0        # 武将星级
        self.iscons         = 0        # 是否征兵中
        self.ishurt         = 0        # 是否受伤
        self.ismarch        = 0        # 是否出征
        self.sarms          = 1        # 兵种类型(大类型)
        self.marms          = 0        # 兵种类型(小类型)
        self.range          = 1        # 攻击距离
        self.hurtbfb        = 0        # 最后的伤兵百分比
        self.vipround       = 0        # vip 减少技能回合数

        self.mAttk          = 0        # 总攻击
        self.mDefe          = 0        # 总防御
        self.mInter         = 0        # 总谋略
        self.mPhy           = 0        # 总统帅
        self.mHit           = 0        # 总命中
        self.mMiss          = 0        # 总闪避
        self.mCritica       = 0        # 总爆击
        self.mSpeed         = 0        # 总速度
        self.mBatt          = 0        # 剩余兵力
        self.mTotBatt       = 0        # 总兵力
        self.mInitBatt      = 0        # 第一次初始兵力

        self.nAttk          = 0        # buff 产生攻击
        self.nDefe          = 0        # buff 产生防御
        self.nInter         = 0        # buff 产生谋略
        self.nPhy           = 0        # buff 产生统帅
        self.nHit           = 0        # buff 产生命中
        self.nMiss          = 0        # buff 产生闪避
        self.nCritica       = 0        # buff 产生爆击
        self.nSpeed         = 0        # buff 产生速度

        self.addAttk        = 1        # buff 产生攻击百分比
        self.addDefe        = 1        # buff 产生防御百分比
        self.addInter       = 1        # buff 产生谋略百分比
        self.addPhy         = 1        # buff 产生统帅百分比
        self.addHit         = 1        # buff 产生命中百分比
        self.addMiss        = 1        # buff 产生闪避百分比
        self.addCritica     = 1        # buff 产生爆击百分比
        self.addSpeed       = 1        # buff 产生速度百分比
        self.addSkillHurt   = 1        # buff 技能伤害百分比
        self.addNormalHurt  = 1        # buff 普工伤害百分比
        self.addAllHurt     = 1        # buff 所有伤害百分比
        self.addmSkillHurt  = 1        # buff 受到技能伤害百分比
        self.addmNormalHurt = 1        # buff 受到普工伤害百分比
        self.addmAllHurt    = 1        # buff 受到所有伤害百分比
        self.addSuckBlood   = 1        # buff 普通攻击吸血百分比
  

        self.oAttk          = 0        # 技能单回合临时增加攻击
        self.oDefe          = 0        # 技能单回合临时增加防御
        self.oInter         = 0        # 技能单回合临时增加谋略
        self.oPhy           = 0        # 技能单回合临时增加统帅
        self.oHit           = 0        # 技能单回合临时增加命中
        self.oMiss          = 0        # 技能单回合临时增加闪避
        self.oCritica       = 0        # 技能单回合临时增加爆击
        self.oSpeed         = 0        # 技能单回合临时增加速度
        self.oBatt          = 0        # 技能单回合临时增加兵力
        self.okilleffect    = 1        # 技能单回合临时斩杀效果
        self.osuckblood     = 0        # 技能单回合临时吸血效果
        self.overfight      = 1        # 技能单回合临时越战越勇效果
        self.SkillAttk      = 0        # 技能产生的攻击替换自身攻击 
        self.SkillHurt      = 0        # 技能产生的伤害,不需要重新取一次普通伤害

        
        self.killbatt       = 0        # 杀死对方兵力
        self.nHurtMuilte    = 1        # 对倍数伤害
        self.mHurt          = 0        # 对别人普通伤害  
        self.bLev           = 0        # 是否升级
        self.isMainHero     = 0        # 是否主将,0=不是,1=是
        self.skillinfo      = {}       # 武将技能信息
        self.mHeroSkill     = []       # 武将技能
        self.mRound         = 1        # 当前武将回合
        self.mBattle        = None     # 当前战斗
        self.buffers        = []       # 当前武将buff列表  
        self.type           = 0        # 0=攻击方,1=防守方
        self.pos            = 0        # 武将站位
        self.rounddata      = {}       # 执行动作每回合数据数据
        self.cleanbuff      = []       # 清除buff

        self.normalHurt     = 0        # 普通杀伤    总和
        self.skillHurt      = 0        # 技能杀伤    总和
        self.skillcount     = 0        # 技能释放次数 总和
        self.helpbatt       = 0        # 救援回复的总兵力
        self.losebatt       = 0        # 损失总兵力
        self.mBattleConfig  = mBattleConfig
        self.ismonster      = 0
        self.__tasklet      = None
        self.__atomic       = None
        self.__lock         = 0
        pass

    def __enter(self):
        self.__tasklet = stackless.getcurrent()
        self.__atomic  = self.__tasklet.set_atomic(True)
        pass

    def __leave(self):
        self.__tasklet.set_atomic(self.__atomic)
        pass

    def sync(func):
        def fun(self,args=None):
            self.__enter()
            res = None
            try:
                res = func(self,args)
            except Exception,ex:
                pass
            self.__leave()
            return res
        return fun

    @property
    def pack(self):
        #初始信息
        return [self.iid, self.hid, self.pos, self.level, int(self.mTotBatt), self.star, self.ismonster, self.isMainHero, self.hp, self.exp]

    @property
    def buffpack(self):
        #BUFF信息
        return [int(self.Vertigo), int(self.Silent), int(self.Chaos), int(self.IgnoreDefe)]

    @property
    def buffinfo(self):
        #BUFF组信息
        bufflist = [buffer.id for buffer in self.buffers]
        return bufflist

    @property
    def mExp(self): #当前经验
        return self.exp
    
    @property
    def movebuff(self):
        bufflist = [buffer.id for buffer in self.buffers if buffer.effect == 2]
        return bufflist

    @property
    def myinfo(self): #每回合自己的信息 当前buff, 目标, 状态, 回复或者损失兵力， 剩余兵力, 无效果
        return [int(self.roundBatt), int(self.mBatt), int(self.battbuff)]
    
    @property
    def endpack(self):
        return [int(self.mBatt), int(self.hurtbatt), int(self.hp), int(self.mExp), int(self.level)]
    

    @property
    def staticpack(self):
        if self.mBatt <=0: self.mBatt = 0
        if self.mBatt >= self.mTotBatt:
            losebatt = 0
        else:            
            losebatt = self.mTotBatt - self.mBatt
        return [int(self.mBatt), int(self.hurtbatt), int(self.hp), int(self.mExp), int(self.level),\
                int(self.normalHurt), int(self.skillHurt), int(self.skillcount), int(self.helpbatt), \
                int(losebatt), int(self.hurtbatt)]
    
    @property
    def hurtbatt(self): #每场战斗
        #最后的伤兵
        #hurtbfb = GameGlobalConfig.getInstance().get_global_info(45)
        dieBatt = self.mTotBatt - self.mBatt #死亡兵力
        if dieBatt <= 0: dieBatt = 0
        mHurtBatt = dieBatt * (float(self.hurtbfb)/100)   
        return int(round(mHurtBatt))

    @property
    def diebatt(self):  #最后总死亡兵力
        dieBatt = self.mInitBatt - int(self.mBatt) #死亡兵力
        if dieBatt <= 0: dieBatt = 0
        return dieBatt

    @property
    def sybatt(self): #最后剩余兵力
        if self.mBatt >= self.mTotBatt: return self.mTotBatt
        return self.mBatt

    #初始化武将信息
    def InitHeroData(self, row=None, pos=0):
        self.pos             = pos
        self.iid             = int(row[RoleKey.PK_HERO_ID])
        self.hid             = int(row[RoleKey.PK_HERO_HID])
        self.name            = str(row[RoleKey.PK_HERO_NAME])
        self.level           = int(row[RoleKey.PK_HERO_LEVEL])
        self.exp             = int(row[RoleKey.PK_HERO_EXP])
        self.hp              = int(row[RoleKey.PK_HERO_HP])
        self.star            = int(row[RoleKey.PK_HERO_STAR])
        self.range           = int(row[RoleKey.PK_HERO_RANGE])
        self.mAttk           = int(row[RoleKey.PK_HERO_ATTK])
        self.mDefe           = int(row[RoleKey.PK_HERO_DEFE])
        self.mInter          = int(row[RoleKey.PK_HERO_INTER])
        self.mPhy            = int(row[RoleKey.PK_HERO_PHY])
        self.mHit            = int(row[RoleKey.PK_HERO_HIT])
        self.mMiss           = int(row[RoleKey.PK_HERO_MISS])
        self.mCritica        = int(row[RoleKey.PK_HERO_CIRTICAL])
        self.mSpeed          = int(row[RoleKey.PK_HERO_SPEED])
        self.mBatt           = int(row[RoleKey.PK_HERO_BATT])
        self.mTotBatt        = self.mBatt
        self.mInitBatt       = self.mBatt
        self.isMainHero      = int(row[RoleKey.PK_HERO_MAIN])
        self.skillinfo       = row[RoleKey.PK_HERO_SKILL]
        self.hurtbfb         = int(row[RoleKey.PK_HERO_HUER_BFB])
        self.vipround        = int(row[RoleKey.PK_VIP_SKILL_ROUND])
        self.__InitHeroSkill()
        pass

    #初始化怪物信息
    def InitMonsterData(self, monsterTemp=None, pos=0, isMainHero=0):
        self.iid             = monsterTemp.id
        self.hid             = monsterTemp.id
        self.name            = monsterTemp.name
        self.level           = monsterTemp.level
        self.hp              = 0
        self.star            = monsterTemp.star
        self.range           = monsterTemp.range
        self.pos             = pos
        self.mAttk           = monsterTemp.atk
        self.mDefe           = monsterTemp.defence
        self.mInter          = monsterTemp.strategy
        self.mPhy            = 0
        self.mHit            = monsterTemp.hit
        self.mMiss           = monsterTemp.miss
        self.mCritica        = monsterTemp.critical
        self.mSpeed          = monsterTemp.speed
        self.mBatt           = monsterTemp.troop
        self.mTotBatt        = self.mBatt
        self.mInitBatt       = self.mBatt
        self.isMainHero      = isMainHero
        self.skillinfo       = monsterTemp.get_heros_skill()
        self.__InitHeroSkill()
        pass


    #初始化技能
    def __InitHeroSkill(self):
        for skillid, level in self.skillinfo.iteritems():
            skillObj = self.mBattleConfig.getSkillTemp(skillid, level)
            if skillObj is None: continue
            self.mHeroSkill.append(skillObj)
        pass
    
    #获取武将技能 1=主动技能, 2=被动技能,3=指挥技能,4=追击技能
    def get_skill_info(self, skilltype=0):
        for skill in self.mHeroSkill:
            if skill is None: continue
            if skill.type == skilltype:
                return skill
        return None

    #获取被动技能(可能有多个)
    def get_passive_skill(self):
        skilllist = []
        for skill in self.mHeroSkill:
            if skill is None: continue
            if skill.type == 2:
                skilllist.append(skill)
        return skilllist

    #============================武将自身属性START===========================================================        
    @property
    def Attk(self):
        #武将自身攻击 + buff产生的攻击 + 单回合武将技能产生的攻击) * buff 百分比
        return (self.mAttk + self.nAttk + self.oAttk) * self.addAttk

    @property
    def Defe(self):
        #武将自身防御 + buff产生的防御 +单回合武将技能产生的防御) * buff 百分比
        return (self.mDefe + self.nDefe + self.oDefe) * self.addDefe

    @property
    def Inter(self):
        #武将自身谋略 + buff产生的谋略 +单回合武将技能产生的谋略) * buff 百分比
        return (self.mInter + self.nInter + self.oInter) * self.addInter

    @property
    def Phy(self):
        #武将自身统帅 + buff产生的统帅 +单回合武将技能产生的统帅) * buff 百分比
        return (self.mPhy + self.nPhy + self.oPhy) * self.addPhy

    @property
    def Hit(self):
        #武将自身命中 + buff产生的命中 +单回合武将技能产生的统帅) * buff 百分比
        return (self.mHit + self.nHit + self.oHit) * self.addHit

    @property
    def Miss(self):
        #武将自身闪避 + buff产生的闪避 +单回合武将技能产生的闪避) * buff 百分比
        return (self.mMiss + self.nMiss  + self.oMiss) * self.addMiss

    @property
    def Critica(self):
        #武将自身暴击 + buff产生的暴击 +单回合武将技能产生的暴击) * buff 百分比
        return (self.mCritica + self.nCritica + self.oCritica) * self.addCritica

    @property
    def Speed(self):
        #武将自身速度 + buff产生的速度 +单回合武将技能产生的速度) * buff 百分比
        return (self.mSpeed + self.nSpeed + self.oSpeed) * self.addSpeed

    @property
    def Batt(self):
        #当前总兵力
        return self.mBatt
    
    
    #============================武将自身属性END===========================================================
    #============================计算普通攻击（伤害, 防御,命中, 暴击） START===============================
    #获取伤害值
    def getHurt(self, hero):
        # 伤害值 = 攻击方总攻击力 * 攻击方兵力 * 参数 /（攻击方兵力 + 防御方兵力）*（1 - 防御方伤害减免）
        # *（1 + 伤害加成 - 伤害减成）
        # 参数为3
        if self.IgnoreDefe:
            defe = 0
        else:
            defe = hero.getHurtDeduction()
        if self.SkillAttk > 0:
            v = (self.SkillAttk * self.Batt * 3.0) / (self.Batt + hero.Batt) * ( 1 - defe)
        else:
            v = (self.Attk * self.Batt * 3.0) / (self.Batt + hero.Batt) * ( 1 - defe)
        #print "=====当前技能攻击:%s 当前攻击:%s, 当前兵力:%s, 当前敌方兵力:%s, 当前防御伤害减免:%s, 最后计算的伤害值:%s"%(self.SkillAttk, self.Attk, self.Batt, hero.Batt, defe, v)
        return math.floor(v)

    #获取技能设定的攻击值进行攻击
    def getSkillHurt(self, hero, attk=0):
        if hero is None: return 0
        if self.IgnoreDefe:
            defe = 0
        else:
            defe = hero.getHurtDeduction()
        v = (attk * self.Batt * 3.0) / (self.Batt + hero.Batt) * ( 1 - defe)
        #print "=====当前技能攻击:%s 当前攻击:%s, 当前兵力:%s, 当前敌方兵力:%s, 当前防御伤害减免:%s, 最后计算的伤害值:%s"%(self.SkillAttk, self.Attk, self.Batt, hero.Batt, defe, v)
        return math.floor(v)
        pass

    #防御伤害减免
    def getHurtDeduction(self):
        #伤害减免 = 总防御力/（总防御力 + 参数1 * 等级 + 参数2）
        #参数1为40，参数2为3500
        return self.Defe / (self.Defe + 40*self.level + 3500)

    #获取是否命中
    def getHit(self, hero):
        # 命中值 = max（20%，攻击方命中值/（攻击方命中值 + 防御方闪避值） * （1 + 命中率加成% - 命中率减成%））
        per = max(0.2, float(self.Hit) / (self.Hit + hero.Miss))
        return random.random() <= per
    
    #暴击率
    def getCritical(self):
        #获取暴击率
        return random.random() <= self.Critica / 10000.0

    #暴击造成的伤害值
    def getCriticalHurt(self):
        return 2
    #============================计算普通攻击（伤害, 防御,命中, 暴击） END==================================

    #============================武将BUFF状态 开始==========================================================
    @property
    def Vertigo(self):
        #是否眩晕
        for buffer in self.buffers:
            if buffer.Vertigo: return True
        return False

    @property
    def Silent(self):
        #是否沉默
        for buffer in self.buffers:
            if buffer.Silent: return True
        return False

    @property
    def AllowSkill(self):
        #是否禁止全技能
        for buffer in self.buffers:
            if buffer.AllowSkill: return True
        return False

    @property
    def AllowAttack(self):
        #禁止普通攻击
        for buffer in self.buffers:
            if buffer.AllowAttack: return True
        return False

    @property
    def Chaos(self):
        #是否混乱
        for buffer in self.buffers:
            if buffer.Chaos: return True
        return False

    @property
    def IgnoreDefe(self):
        #buff 是否 无视防御
        for buffer in self.buffers:
            if buffer.Chaos: return True
        return False

    @property
    def DoubleHit(self):
        #BUFF 获取连接次数
        v = 0
        for buffer in self.buffers:
            v += buffer.DoubleHit
        return (v==0) and 1 or v

    @property
    def isMissOnec(self):
        #buff 是否闪避一次
        for buffer in self.buffers:
            if buffer.nMiss: return True
        return False

    @property
    def buffOverFight(self):
        #buff 是否越战越勇,普通攻击
        for buffer in self.buffers:
            if buffer.nHurtBatt: return 2 - float(self.mBatt)/self.mTotBatt
        return 1
    
    @property
    def shieldSuck(self):
        #buff 盾的吸收值
        hurt = 0
        for buffer in self.buffers:
            hurt += buffer.get_coat_absorb()
        return math.floor(hurt)

    @property
    def roundBatt(self):
        #buff 每回合兵力变化
        if self.mBatt <=0: return 0
        batt = 0
        for buffer in self.buffers:
            batt += buffer.get_batt_info()
        return math.floor(batt)

    @property
    def battbuff(self):
        #buff 每回合兵力变化 对于的buff
        for buffer in self.buffers:
            batt = buffer.get_batt_info()
            if batt != 0: return buffer.id
        return 0 
    
    @property
    def SputteHurt(self):
        #buff 溅射百分比
        bfb = 0
        for buffer in self.buffers:
            bfb += buffer.SputteHurt
        return bfb

    @property
    def SuckBlood(self):
        #buff 吸血百分比
        bfb = 0
        for buffer in self.buffers:
            bfb += buffer.SuckBlood
        return bfb

    @property
    def roundAttck(self):
        #每回合被攻击兵力变化
        if self.mBatt <=0: return 0
        batt = 0
        for buffer in self.buffers:
            batt += buffer.get_attack_batt()
        return int(math.floor(batt))

    #============================武将BUFF状态 结束==========================================================

    #============================武将技能增加 开始==========================================================
    #技能临时增加攻击
    def setSkillAttk(self, v = 0, addbfb=1):
        self.oAttk      = v
        pass

    #技能临时增加防御
    def setSkillDefe(self, v = 0, addbfb=1):
        self.oDefe      = v
        pass 
       
    #技能临时增加谋略
    def setSkillInter(self, v = 0, addbfb=1):
        self.oInter      = v
        pass 
            
    #技能临时增加统帅
    def setSkillPhy(self, v = 0, addbfb=1):
        self.oPhy        = v
        pass

    #技能临时增加命中
    def setSkillHit(self, v = 0, addbfb=1):
        self.oHit        = v
        pass

    #技能临时增加闪避
    def setSkillMiss(self, v = 0, addbfb=1):
        self.oMiss        = v
        pass
    
    #技能临时增加暴击
    def setSkillCritica(self, v = 0, addbfb=1):
        self.oCritica     = v
        pass

    #技能临时增加速度
    def setSkillSpeed(self, v = 0, addbfb=1):
        self.oSpeed        = v
        pass

    #技能临时增加兵力
    def setSkillBatt(self, v = 0, addbfb=1):
        self.oBatt        = v
        pass

    #技能造成的伤害值(当前技能类型总的伤害值)
    def setSkillHurt(self, v=0):
        self.mHurt += v
        pass

    #设置伤害倍数
    def setHurtMultiple(self, v=0):
        self.nHurtMuilte = v
        pass
    
    #技能恢复/减少兵力
    def setResumedBatt(self, batt):
        self.mBatt += batt
        pass

    #清除buff 组号
    def SkillRemoveBuff(self, bufflist):
        pass

    #技能设置buff
    def SkillAddBuff(self, buff):
        pass

    #设置CD减少
    def setHeroRound(self, round=0):
        self.mRound += round
        if self.mRound <=0: self.mRound =0
        pass

    #设置斩杀
    def setKillEffect(self, hurt):
        self.okilleffect = hurt
        pass

    #设置吸血效果
    def setSuckBlood(self, v):
        self.osuckblood = v
        pass

    #设置越战越勇
    def setOverFight(self, v):
        self.overfight = v
        pass

    #设置伤害
    def setHeroHurt(self, hurt):
        self.mBatt -= hurt
        if self.mBatt <=0: self.mBatt = 0
        pass
    
    #设置技能替换的攻击
    def setSkillReplaceAttack(self, v):
        self.SkillAttk      = v        
        self.SkillHurt      = 0
        pass

    #设置技能替换的攻击
    def setSkillReplaceAttack(self, v):
        self.SkillAttk      = v        
        self.SkillHurt      = 0
        pass
    #============================武将技能增加 结束==========================================================

    #战斗开始初始状态
    def HeroResetStatus(self):
        self.killbatt       = 0
        self.normalHurt     = 0        # 普通杀伤    总和
        self.skillHurt      = 0        # 技能杀伤    总和
        self.skillcount     = 0        # 技能释放次数 总和
        self.helpbatt       = 0        # 救援回复的总兵力
        self.losebatt       = 0        # 损失总兵力
        self.CleanBuffProp()
        self.CleanSkillProp()
        self.buffers = []
        pass
    
    #清除BUFF属性
    def CleanBuffProp(self):
        self.nAttk           = 0        # buff 产生攻击
        self.nDefe           = 0        # buff 产生防御
        self.nInter          = 0        # buff 产生谋略
        self.nPhy            = 0        # buff 产生统帅
        self.nHit            = 0        # buff 产生命中
        self.nMiss           = 0        # buff 产生闪避
        self.nCritica        = 0        # buff 产生爆击
        self.nSpeed          = 0        # buff 产生速度

        self.addAttk         = 1        # buff 产生攻击百分比
        self.addDefe         = 1        # buff 产生防御百分比
        self.addInter        = 1        # buff 产生谋略百分比
        self.addPhy          = 1        # buff 产生统帅百分比
        self.addHit          = 1        # buff 产生命中百分比
        self.addMiss         = 1        # buff 产生闪避百分比
        self.addCritica      = 1        # buff 产生爆击百分比
        self.addSpeed        = 1        # buff 产生速度百分比

        self.addSkillHurt    = 1        # buff 技能伤害百分比
        self.addNormalHurt   = 1        # buff 普工伤害百分比
        self.addAllHurt      = 1        # buff 所有伤害百分比
        self.addmSkillHurt   = 1        # buff 受到技能伤害百分比
        self.addmNormalHurt  = 1        # buff 受到普工伤害百分比
        self.addmAllHurt     = 1        # buff 受到所有伤害百分比
        self.addSuckBlood    = 1        # buff 普通攻击吸血百分比
        pass

    #清除技能增加属性
    def CleanSkillProp(self):
        self.oAttk        = 0           # 单回合临时增加攻击
        self.oDefe        = 0           # 单回合临时增加防御
        self.oInter       = 0           # 单回合临时增加谋略
        self.oPhy         = 0           # 单回合临时增加统帅
        self.oHit         = 0           # 单回合临时增加命中
        self.oMiss        = 0           # 单回合临时增加闪避
        self.oCritica     = 0           # 单回合临时增加爆击
        self.oSpeed       = 0           # 单回合临时增加速度
        self.oBatt        = 0           # 单回合临时增加兵力(剩余兵力)
        self.oTotBatt     = 0           # 单回合临时增加兵力
        self.mHurt        = 0
        pass


    #检查buff组号是否有当前同类buff
    def __check_buff(self, buffer):
        for mbuff in self.buffers:
            if buffer.team != mbuff.team: continue
            if buffer.level >= mbuff.level:
                return mbuff                             
        return None

    #对当前武将添加buff, 如果有相同buff 则 替换等级最高的buff
    def addBuffer(self, hero, bufferlist=[]):
        for buffer in bufferlist:
            mbuffer = self.__check_buff(buffer)
            if mbuffer:self.buffers.remove(mbuffer)
            buffer.hero = hero #给buff 绑定释放者
            self.buffers.append(buffer)
        self.__update_buffer_attr()
        pass

    #移除某个buff,只用于技能移除
    def removeBuffer(self, buffer):
        mbuffer = self.__check_buff(buffer)
        if mbuffer:self.buffers.remove(mbuffer)
        self.__update_buffer_attr()
        pass

    #更新buff影响的属性,增加具体值和百分比(立即生效)
    def __update_buffer_attr(self, tp=1):
        self.CleanBuffProp()
        for buff in self.buffers:
            if buff.effect == tp:
                self.nAttk          += buff.Attack
                self.nDefe          += buff.Defe
                self.nInter         += buff.Inter
                self.nPhy           += buff.Phy
                self.nHit           += buff.Hit
                self.nMiss          += buff.Miss
                self.nCritica       += buff.Critical
                self.nSpeed         += buff.Speed
                self.addAttk        *= (1+buff.addAttack)
                self.addDefe        *= (1+buff.addDefe)
                self.addInter       *= (1+buff.addInter)
                self.addPhy         *= (1+buff.addPhy)
                self.addHit         *= (1+buff.addHit)
                self.addMiss        *= (1+buff.addMiss)
                self.addSpeed       *= (1+buff.addSpeed)
                self.addSkillHurt   *= (1+buff.SkillHurt)        
                self.addNormalHurt  *= (1+buff.NormalHurt)       
                self.addAllHurt     *= (1+buff.AllHurt)       
                self.addmSkillHurt  *= (1+buff.mSkillHurt)      
                self.addmNormalHurt *= (1+buff.mNormalHurt)
                self.addmAllHurt    *= (1+buff.mAllHurt)    
                self.addSuckBlood   *= (1+buff.SuckBlood)
        pass
    #========================================================================================================
    #1兵力数量最多敌人X-N人,2兵力数量最少敌人X-N人,3我方兵力最多的友军X-N人,4我方兵力最少的友军X-N人,5敌方随机X-N人,6友方随机X-N人,7自身
    #获取己方武将列表 随机_m,_n,_hero 区别开的武将
    def get_batt_hero(self, _m, _n, target=0, _hero=None):
        endtarger = 0 #0=敌人,1=自己, 2=友军
        heros = []
        if target == 1:     #1兵力数量最多敌人X-N人
            heros = self.mBattle.get_batt_hero(self, 2)
            heros = [hero for hero in heros if hero.mBatt > 0 and hero != self]
            heros.sort(key=lambda hero:hero.mBatt, reverse=True)
        elif target == 2:   #2兵力数量最少敌人X-N人
            heros = self.mBattle.get_batt_hero(self, 2)
            heros = [hero for hero in heros if hero.mBatt > 0 and hero != self]
            heros.sort(key=lambda hero:hero.mBatt, reverse=False)
        elif target == 3:   #3我方兵力最多的友军X-N人
            heros = self.mBattle.get_batt_hero(self, 1)
            heros = [hero for hero in heros if hero.mBatt > 0]
            heros.sort(key=lambda hero:hero.mBatt, reverse=True)
            endtarger = 2
        elif target == 4:   #4我方兵力最少的友军X-N人
            heros = self.mBattle.get_batt_hero(self, 1)
            heros = [hero for hero in heros if hero.mBatt > 0]
            heros.sort(key=lambda hero:hero.mBatt, reverse=False)
            endtarger = 2
        elif target == 5:   #5敌方随机X-N人
            heros = self.mBattle.get_batt_hero(self, 2)
            heros = [hero for hero in heros if hero.mBatt > 0]
        elif target == 6:   #6友方随机X-N人
            heros = self.mBattle.get_batt_hero(self, 1)
            heros = [hero for hero in heros if hero.mBatt > 0]
            endtarger = 2
        elif target == 7:   #7自己
            endtarger = 1
            heros     = [self]
        elif target == 8:   #敌方最近的武将
            heros = self.mBattle.get_batt_hero(self, 2)
            if _hero:
                heros = [hero for hero in heros if hero.mBatt > 0 and hero not in [self,_hero]]
            else:
                heros = [hero for hero in heros if hero.mBatt > 0 and hero != self]
            heros.sort(key=lambda hero:hero.pos, reverse=False)
            if len(heros) > 0: heros = [heros[0]]
        elif target == 9:   #己方最近的武将
            heros = self.mBattle.get_batt_hero(self, 1)
            heros = [hero for hero in heros if hero.mBatt > 0 and hero != self]
            heros.sort(key=lambda hero:hero.pos, reverse=False)
            if len(heros) > 0: heros = [heros[0]]
        elif target == 10:  #混乱状态随机目标武将
            heros = self.mBattle.get_batt_hero(self, 0)
            if _hero:
                heros = [hero for hero in heros if hero.mBatt > 0 and hero not in [self,_hero]]
            else:
                heros = [hero for hero in heros if hero.mBatt > 0 and hero != self]
            heros.sort(key=lambda hero:hero.mBatt, reverse=True)
        if len(heros) == 0 and endtarger == 2: 
            heros.append(self)
        elif len(heros) == 0 and endtarger != 2:
            return heros,endtarger    
        if len(heros) == 1:
            _m = _n = 1
        elif _m > len(heros):
            _m, _n = len(heros), len(heros)
        elif _m < len(heros) and _n >= len(heros):
            _n = len(heros)
        heros = random.sample(heros, random.randint(_m, _n)) #随机取出武将个数
        return heros,endtarger
    #========================================================================================================
    #buff结算
    def BalanceBuff(self, mtype=1):
        effectbuff = []
        for buffer in self.buffers:
            if buffer.settlement == 1 and mtype == 1:
                buffer.round -=1
            elif buffer.settlement == 2 and mtype == 2:
                buffer.round -=1

            if buffer.round  <= 0 and mtype == 1:
                effectbuff.append(buffer)
        #这里多了一步可以减少
        
        for buffer in effectbuff:
            self.buffers.remove(buffer)
            self.cleanbuff.append(buffer.id)
        #print "====================%s 每回合增加或者减少 %s 兵力"%(str(self.name), str(self.roundBatt))
        if mtype == 1 and self.mBatt >0:    #行动前 回复 或者掉血
            self.mBatt    += self.roundBatt                            #没回合恢复兵力或者损失兵力
            if self.roundBatt >0: self.helpbatt += self.roundBatt      #救援兵力
        if self.helpbatt <=0: self.helpbatt = 0
        self.__update_buffer_attr()
        pass

    #每回合重置数据
    def round_clear(self):
        self.rounddata = {}
        self.cleanbuff = []
        pass  
        
    #释放指挥技能
    def __ReleaseCommondSkill(self):
        skill = self.get_skill_info(3)
        if skill is None: return
        if self.mRound != skill.command_cd: return
        skilltypes      = skill.getSkillType()
        #检查是否眩晕和全技能
        if not self.Vertigo or not self.AllowSkill:
            mBattInfo = self.set_skill_attk(skilltypes, 0)
            self.rounddata[SKILL_TYPE_ZHIHUI] = [0, skill.id,mBattInfo]
        else:
            if skill.affected != 0:
                if self.Vertigo:
                    self.rounddata[SKILL_TYPE_ZHIHUI] = [2,skill.id, {}]
                elif self.AllowSkill:
                    self.rounddata[SKILL_TYPE_ZHIHUI] = [4,skill.id,{}]
                else:
                    self.rounddata[SKILL_TYPE_ZHIHUI] = [6,skill.id,{}]
                return
            mBattInfo = self.set_skill_attk(skilltypes, 0)
            self.rounddata[SKILL_TYPE_ZHIHUI] = [0, skill.id, mBattInfo]
        pass
    
    #释放主动技能
    def __ReleaseMainSkill(self):
        #获取主动技能
        skill = self.get_skill_info(1)
        if skill is None: return
        if skill.mRound == skill.skills_cd and self.vipround > 0: #第一次减去vip回合数
            skill.mRound -= self.vipround
        if skill.mRound !=0: 
            skill.mRound -=1
            if skill.mRound <=0: skill.mRound = 0
            return
        skilltypes = skill.getSkillType()
        if not self.Vertigo and not self.AllowSkill and not self.Silent:
            mBattInfo       = self.set_skill_attk(skilltypes, 1)
            skill.mRound    = skill.skills_cd
            status          = self.Chaos and 5 or 0
            self.rounddata[SKILL_TYPE_ZHUDONG] = [status,skill.id,mBattInfo]
        else:
            if skill.affected != 0: 
                if self.Vertigo:
                    self.rounddata[SKILL_TYPE_ZHIHUI] = [2,skill.id,{}]
                elif self.AllowSkill:
                    self.rounddata[SKILL_TYPE_ZHIHUI] = [4,skill.id,{}]
                elif self.Silent:
                    self.rounddata[SKILL_TYPE_ZHIHUI] = [3,skill.id,{}]
                else:
                    self.rounddata[SKILL_TYPE_ZHIHUI] = [6,skill.id,{}]
                return
            mBattInfo    = self.set_skill_attk(skilltypes, 1)
            skill.mRound = skill.skills_cd
            status       = self.Chaos and 5 or 0
            self.rounddata[SKILL_TYPE_ZHUDONG] = [status,skill.id, mBattInfo]
        pass

    #释放被动技能 初始化被动技能,挂上buff
    def ReleasePassiveSkill(self):
        skilllist = self.get_passive_skill()
        for skill in skilllist:
            skilltypes = skill.getSkillType()               #取出被动技能的所有类型
            for i, skilltype in skilltypes.iteritems():
                skilltype.setSkillEffect(self, self)        #获取的武将技能影响
        pass

    #释放追击技能 i=是第几次普通攻击触发追击
    def __ReleasePursuedSkill(self):
        skill    = self.get_skill_info(4)
        if skill is None: return
        if skill.mRound == 0: 
            skilltypes          = skill.getSkillType()
            mBattInfo           = self.set_skill_attk(skilltypes, 2)
            skill.mRound        = skill.skills_cd
            self.rounddata[SKILL_TYPE_NORMAL][2].append(mBattInfo)
        else:
            skill.mRound    -=1
            if skill.mRound <=0: skill.mRound = 0
        pass

    #执行普通攻击
    def __NormalAttack(self):
        #获取普通攻击
        if self.AllowAttack:
            self.rounddata[SKILL_TYPE_NORMAL] = [1, 0, []]
            return
        elif self.Vertigo:
            self.rounddata[SKILL_TYPE_NORMAL] = [2, 0, []]
            return
        
        #普通攻击 连击次数
        for i in xrange(0, self.DoubleHit):
            if i == 0: 
                status       = self.Chaos and 5 or 0
                self.rounddata[SKILL_TYPE_NORMAL] = [status, 0, []]
            mBattInfo       = self.set_normal_attk()
            self.rounddata[SKILL_TYPE_NORMAL][2].append(mBattInfo)
            #释放追击
            self.__ReleasePursuedSkill()
        pass

    #武将动作流程
    #释放状态:0=释放，1= 禁止普攻,2=眩晕,3=沉默,4=禁止全技能 ,5=混乱,6=是否受控
    #回合中数据 结构 [技能类型] = {释放状态:0,,释放后对目标造成的结果:{武将ID:[剩余buff, 新增buff, 移除buff, 友军/敌方, 状态，损失兵力，剩余兵力]}}
    #
    def Action(self):
        self.__update_buffer_attr(2) #行动时buff生效
        #第一回合初始化被动技能
        #if self.mRound == 1:
        #    self.__ReleasePassiveSkill()
        #释放指挥技能
        self.__ReleaseCommondSkill()
        #释放主动技能
        self.__ReleaseMainSkill()
        #释放普通攻击
        self.__NormalAttack()
        #回合数加一,结算buff
        self.BalanceBuff(2)
        self.mRound += 1
        pass

    #设置当前技能目标武将的状态
    def __set_target_status(self, heros):
        herostatus = {}
        for hero in heros:
            if herostatus.has_key(hero.iid): continue
            if not self.getHit(hero):
                herostatus[hero.iid] = 0
            else:
                if self.getCritical():
                    herostatus[hero.iid] = 1
                else:
                    herostatus[hero.iid] = 2
        return herostatus

    #对武将触发的技能攻击
    def set_skill_attk(self, skilltypes, tp=0):
        skilltargets    = {}  #对特定目标实施过的buff
        mHeroHurt       = {}  #当前技能造成对方总伤害
        mBattInfo       = {}  #对每个武将赋予的buff及伤害
        mHerostatus     = {}  #武将对接的状态,0=命中,1=暴击, 2=正常
        mHeroeffect     = {}  #技能对武将产生的效果  
        mTargetInfo     = {}  #对每个武将最后的针对的目标 0=敌人，1=自己
        mEffectHeros    = {}  #影响的所有武将
        mSkillTypes     = []
        endtarget       = 0 
        if tp != 2: self.skillcount += 1
        for i, skilltype in skilltypes.iteritems():
            self.CleanSkillProp()                               #每个类型前清除前类型的附加属性
            heros =  skilltargets.get(skilltype.targetNum, [])
            issetstatus = False                                 #是否重新获取目标状态，不同目标状态不同
            if len(heros) == 0:                                 #之前没有相同目标
                if self.Chaos:
                    heros,endtarget = self.get_batt_hero(skilltype.min, skilltype.max, 10)
                else:
                    heros,endtarget = self.get_batt_hero(skilltype.min, skilltype.max, skilltype.target)
                skilltargets[skilltype.targetNum] = heros
                issetstatus = True
            if len(heros) == 0: print "===================获取不到目标";return {}
            if len(mHerostatus) == 0 or issetstatus == True:
                _herostatus = self.__set_target_status(heros)
                for hid, status in _herostatus.iteritems():
                    if mHerostatus.has_key(hid): continue
                    if endtarget == 2:                                          #如果是友军 只能是命中，否则默认
                        mHerostatus[hid] = 2
                    else:
                        mHerostatus[hid] = status
            mSkillTypes.append(skilltype.type)
            #对目标人数绑定buff,及技能类型
            for hero in heros:
                info = {}
                if mHerostatus.get(hero.iid, 0) == 0 or hero.isMissOnec:             #对方是否闪避
                    hurt = 0
                elif mHerostatus.get(hero.iid, 0) == 1:
                    info = skilltype.setSkillEffect(self, hero)
                    hurt = self.getHurt(hero) * self.getCriticalHurt()          #获取对敌方武将造成的伤害
                else:
                    info = skilltype.setSkillEffect(self, hero)
                    hurt = self.getHurt(hero)
                #排除技能造成的伤害
                if skilltype.type in [1,2,3]: 
                    mHeroHurt[hero.iid]   = mHeroHurt.get(hero.iid, 0) + hurt
                else:
                    hurt = 0
                #技能对武将产生的效果
                if mHeroeffect.has_key(hero.iid) == False:
                    mHeroeffect[hero.iid] = {}
                mHeroeffect[hero.iid][skilltype.type] = info
                mEffectHeros[hero.iid] = hero
        #计算对目标造成的伤害
        for iid, hero in mEffectHeros.iteritems():
            status =  mHerostatus.get(hero.iid, 0) 
            hurt   =  mHeroHurt.get(hero.iid, 0)
            #hurt   =  (hurt + self.mHurt) * self.addSkillHurt * self.addAllHurt * hero.addmSkillHurt * hero.addmAllHurt * self.okilleffect * self.overfight -  hero.shieldSuck
            #print "====================技能攻击==================%s 对 %s 造成 %s, target:%s"%(str(self.name), str(hero.name), hurt, str(endtarget))
            herotarget = 0       #0=敌人,1=自己
            if self == hero:     #如果是自己 
                hurt        = 0
                status      = 2
                herotarget  = 1
            else:               #如果是友方有可能包括自己 
                herotarget  = 0       
                if self.type == hero.type and hero == self:   #如果是自己
                    status      = 2
                    if not self.Chaos: hurt    = 0
                elif self.type == hero.type and hero != self: #如果是友方
                    status      = 2
                    if not self.Chaos: hurt    = 0
                elif self.type != hero.type:                  #如果是敌方
                    if status == 1: status = 2
                    hurt += self.mHurt
                elif self.Chaos and 4 in mSkillTypes:         #混乱状态并且是回血的技能则伤害为0
                    hurt = 0

            if hurt <=0: hurt = 0
            if hero.mBatt <= hurt:
                self.killbatt  += hero.mBatt
            else:
                self.killbatt  += hurt
            hero.setHeroHurt(hurt)
            self.skillHurt += hurt
            info   = mHeroeffect.get(hero.iid, {})
            print "==================技能攻击==================%s 对 %s 造成 %s, 剩余兵力:%s, 增加的buff:%s, 当前状态:%s"%(str(self.name), str(hero.name), hurt, str(hero.mBatt), str(info), str(status))
            mBattInfo[hero.iid] = [copy.copy(hero.buffinfo), herotarget, status, int(hurt), int(hero.mBatt), info, 0]
        return mBattInfo

    #对武将触发普通攻击,普通攻击只取到一个人
    def set_normal_attk(self):
        mHeroHurt = 0   #对目标的伤害
        mBattInfo = {}
        self.CleanSkillProp()
        if self.Chaos: #是否混乱, 
            heros,endtarget = self.get_batt_hero(1, 1, 10)
        else:
            heros,endtarget = self.get_batt_hero(5, 5, 5)
            heros = self.__get_my_targer(heros)
        if len(heros) == 0: return {}
        herostatus = self.__set_target_status(heros)
        hero       = heros[0]        #普通攻击的目标
        if herostatus.get(hero.iid, 0) == 0 or hero.isMissOnec: #对方是否闪避
            mHeroHurt = 0
        elif herostatus.get(hero.iid, 0) == 1:
            mHeroHurt = self.getHurt(hero) * self.getCriticalHurt() #获取对敌方武将造成的伤害
        else:   #添加数据结构
            mHeroHurt = self.getHurt(hero)
        #数据
        if endtarget == 2: endtarget = 0
        status =  herostatus.get(hero.iid, 0)
        hurt   =  mHeroHurt
        hurt   =  (hurt + self.mHurt) * self.addNormalHurt * self.addAllHurt * hero.addmNormalHurt * hero.addmAllHurt * self.buffOverFight -  hero.shieldSuck
        if hurt <=0 or status == 0: hurt = 0
        _hurt,info  = hurt, {}                          #本身受到伤害值
        
        #被攻击方 兵力变化
        if hero.roundAttck > 0:
            info  = {31:hero.roundAttck}
            _hurt -= hero.roundAttck

        #自己是否有吸血效果
        addSuckBlood     = int(hurt * self.SuckBlood)    #吸血效果,按原有伤害计算, 自身获得敌方损失兵力
        if addSuckBlood > 0:
            self.mBatt += addSuckBlood
            if self.mBatt >= self.mTotBatt: self.mBatt = self.mTotBatt
            info[24] = addSuckBlood

        self.normalHurt  += (_hurt < 0) and 0 or _hurt   #普通攻击伤害总和
        if hero.mBatt <= _hurt:
            self.killbatt  += hero.mBatt
        else:
            self.killbatt  += _hurt
        hero.setHeroHurt(_hurt)
        #print "==================普通攻击==================%s 对 %s 造成 %s, 剩余兵力:%s"%(str(self.name), str(hero.name), hurt, str(hero.mBatt))
        mBattInfo[hero.iid] = [copy.copy(hero.buffinfo), endtarget, status, int(hurt), int(hero.mBatt), info, 1]

        #如果有溅射伤害,需要重新取一个敌人
        if self.SputteHurt > 0 and status !=0:
            _hero, _info = self.__set_sputte_hurt(hurt, hero)
            if _hero and len(_info) > 0: mBattInfo[_hero.iid] = _info
        return mBattInfo

    #普通攻击造成一个溅射伤害
    def __set_sputte_hurt(self, hurt, hero):
        if self.Chaos: #是否混乱
            heros,endtarget = self.get_batt_hero(1, 1, 10, hero)
        else:
            heros,endtarget = self.get_batt_hero(1, 1, 8, hero)
        if len(heros) == 0: return None, []
        _hero = heros[0]            #取出一个需要溅射
        hurt  = self.SputteHurt * hurt
        self.normalHurt += hurt     #普通攻击伤害总和
        _hero.setHeroHurt(hurt)
        return _hero, [copy.copy(_hero.buffinfo), 0, 2, int(hurt), int(_hero.mBatt), {}, 2]


    #获取武将的某个属性
    def get_attr_info(self, tp = 1, bfb=1):
        if tp == 1:
            return self.Attk  * bfb
        elif tp == 2:
            return self.Defe  * bfb
        elif tp == 3:
            return self.Inter * bfb
        elif tp == 4:
            return self.Speed * bfb
        elif tp == 5:
            return self.mBatt * bfb       
        elif tp == 6:
            return self.Critica  * bfb       
        elif tp == 7:
            return self.Hit  * bfb        
        elif tp == 8:
            return self.Miss * bfb        
        elif tp == 9:
            return self.Phy  * bfb
        return 0

    #普通攻击获取自己的目标
    def __get_my_targer(self, heros):
        if len(heros) == 0: return []
        attkinfo    = {0:[3, 2, 1, 6, 5, 4, 9, 8, 7], 1:[1, 2, 3, 4, 5, 6, 7, 8,9], 2:[2, 1, 3, 5, 4, 6, 8, 7, 9]}
        _pos        = self.pos % 3
        attksort    = attkinfo.get(_pos, []) #获取攻击顺序
        _ppos       = [hero.pos for hero in heros]
        #print "============当前%s, 当前位置:%s, 当前攻击顺序:%s,敌方武将位置列表:%s"%(str(self.name), str(self.pos), str(attksort), str(_ppos))
        for pos in attksort:
            for hero in heros:
                if hero.pos == pos: return [hero]
        return heros[0:1]




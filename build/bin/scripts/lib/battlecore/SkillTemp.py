#!coding:utf-8
#!/usr/bin/python

import copy, random, math, stackless, time, uuid
from protocol.BattleProtocol  import *
from protocol.RoleProtocol  import *
from lib.BaseFunc         import *

class SkillTemp(object):
    '''技能模板'''
    def __init__(self, conf=None, row=None):
        self.conf     = conf
        self.id         = 0     #ID
        self.name       = ''    #技能名
        self.group      = 0     #技能组
        self.level      = 0     #技能等级
        self.icon       = 0     #技能图标
        self.iconname   = 0     #技能文字
        self.effect     = 0     #技能特效
        self.skills_cd  = 0     #技能CD
        self.command_cd = 0     #指挥CD
        self.type       = 0     #发动类型
        self.nodefense  = 0     #无视防御
        self.crits      = 0     #暴击
        self.hit        = 0     #命中
        self.attribute  = ''    #属性加成
        self.affected   = 0     #控制技能影响
        self.skill_1    = 0     #技能类型1
        self.actual_1   = ''    #技能属性1
        self.target_1   = 0     #目标类型
        self.min_1      = 0     #目标最少人数
        self.max_1      = 0     #目标最多人数
        self.skill_2    = 0     #技能类型2
        self.actual_2   = ''    #技能属性2
        self.target_2   = 0     #目标类型
        self.min_2      = 0     #目标最少人数
        self.max_2      = 0     #目标最多人数
        self.skill_3    = 0     #技能类型3
        self.actual_3   = ''    #技能属性3
        self.target_3   = 0     #目标类型
        self.min_3      = 0     #目标最少人数
        self.max_3      = 0     #目标最多人数
        self.skill_4    = 0     #技能类型4
        self.actual_4   = ''    #技能属性4
        self.target_4   = 0     #目标类型
        self.min_4      = 0     #目标最少人数
        self.max_4      = 0     #目标最多人数
        self.explain    = ''    #技能说明
        self.consume    = 0     #升级消耗技能点
        self.decompose  = 0     #分解返还技能点
        self.mRound     = 0     #当前技能回合数
        if row != None:self.init_row(row)
        pass

    def init_row(self, row):
        self.id         = int(row["id"])                #ID
        self.name       = str(row["name"])              #技能名
        self.group      = int(row["group"])             #技能组
        self.level      = int(row["level"])             #技能等级
        self.icon       = int(row["icon"])              #技能图标
        self.iconname   = int(row["iconname"])          #技能文字
        self.effect     = int(row["effect"])            #技能特效
        self.skills_cd  = int(row["skills_cd"])         #技能CD
        self.command_cd = int(row["command_cd"])        #指挥CD
        self.type       = int(row["type"])              #发动类型
        self.nodefense  = int(row["nodefense"])         #无视防御
        # self.crits      = int(row["crits"])             #暴击
        # self.hit        = int(row["hit"])               #命中
        self.attribute  = str(row["attribute"])         #属性加成
        self.affected   = int(row["affected"])          #控制技能影响
        self.skill_1    = int(row["skill_1"])           #技能类型1
        self.actual_1   = str(row["actual_1"])          #技能属性1
        self.target_1   = int(row["target_1"])          #目标类型
        self.min_1      = int(row["min_1"])             #目标最少人数
        self.max_1      = int(row["max_1"])             #目标最多人数
        self.skill_2    = int(row["skill_2"])           #技能类型2
        self.actual_2   = str(row["actual_2"])          #技能属性2
        self.target_2   = int(row["target_2"])          #目标类型
        self.min_2      = int(row["min_2"])             #目标最少人数
        self.max_2      = int(row["max_2"])             #目标最多人数
        self.skill_3    = int(row["skill_3"])           #技能类型3
        self.actual_3   = str(row["actual_3"])          #技能属性3
        self.target_3   = int(row["target_3"])          #目标类型
        self.min_3      = int(row["min_3"])             #目标最少人数
        self.max_3      = int(row["max_3"])             #目标最多人数
        self.skill_4    = int(row["skill_4"])           #技能类型4
        self.actual_4   = str(row["actual_4"])          #技能属性4
        self.target_4   = int(row["target_4"])          #目标类型
        self.min_4      = int(row["min_4"])             #目标最少人数
        self.max_4      = int(row["max_4"])             #目标最多人数
        self.explain    = str(row["explain"])           #技能说明
        self.consume    = int(row["consume"])           #升级消耗技能点
        self.decompose  = int(row["decompose"])         #分解返还技能点
        self.mRound     = self.skills_cd
        self.AllSkillType = {}                          #技能所包含所有技能类型字典 {1:obj,2:obj,3:obj,...}
        pass

    def getSkillType(self):
        self.AllSkillType = {}
        for i in xrange(1, 4):
            skilltp  = self.__getattribute__("skill_%s"%str(i))
            if skilltp == 0: continue
            skilltypeObj = SkillType(self, i)
            self.AllSkillType[i] = skilltypeObj
        return self.AllSkillType
        pass

    # 属性加成
    # 释放技能时候，加成到武将属性里面的
    # {1:300, 2:200}

    def getSkillAttribute(self, tp):
        attrdict = {}
        try:
            if self.attribute =="0": return 0
            attrstr  = "{%s}"%self.attribute.replace("|", ",").replace(";",":")
            attrdict = eval(attrstr)
        except Exception,ex:
            attrdict = {}
        print attrdict,self.attribute
        return attrdict.get(tp, 0)

    @property
    def Nodefense(self):
        # 是否无视防御
        return bool(self.nodefense)

    @property
    def Affected(self):
        # 是否受控制技能影响
        return bool(self.affected)


class SkillType(object):
    ''' 技能类型对象 '''
    def __init__(self, SkillTemp=None, idx=0):
        skillkey = "%s%s"%(SkillTemp.id, SkillTemp.level)
        self.skillTemp = SkillTemp
        self.HeroObj   = None
        self.skillty   = self.skillTemp.__getattribute__("skill_%s"%str(idx))      #技能类型
        self.actual    = self.skillTemp.__getattribute__("actual_%s"%str(idx))     #技能属性
        self.target    = self.skillTemp.__getattribute__("target_%s"%str(idx))     #目标类型
        self.min       = self.skillTemp.__getattribute__("min_%s"%str(idx))        #目标最少人数
        self.max       = self.skillTemp.__getattribute__("max_%s"%str(idx))        #目标最多人数
        self.idx       = idx
        pass

    @property
    def targetNum(self):
        return "%s%s%s"%(self.target, self.min, self.max)

    @property
    def type(self):
        return self.skillty
    
    def getBuffTemp(self, bid):

        return self.skillTemp.conf.getBuffTemp(bid)

    # 获取技能属性加成
    def getSkillAttribute(self, attr_ty):
        return self.skillTemp.getSkillAttribute(attr_ty)

    # 解析类型2
    # 加成属性，然后进行攻击
    # 3；1.2（武将攻击属性设定为谋略属性的1.2倍，然后进行攻击）
    # 立刻造成伤害（1.2*自身谋略）
    def parse_actual_info_two(self):
        # info = self.actual
        # splitlist   = info.split("|")
        # d = {}
        # for inf in splitlist:
        #     buf = inf.split(";")
        #     d[eval(buf[0])] = eval(buf[1])
        try:
            d = "{%s}"%self.actual.replace("|", ",").replace(";",":")
            d = eval(d)
        except Exception,ex:
            d = {}
        count = 0
        actualDict = d
        for k, v in actualDict.items():
            attr = self.HeroObj.get_attr_info(k) + self.getSkillAttribute(k) # 属性值 = 英雄自身属性 + 技能增加属性
            count += attr*v
        return round(count)

    # 解析类型3
    # 伤害加成
    # 1.5；100；3|2（造成150%普通攻击伤害+100点固定伤害+2倍谋略固定伤害
    # 立刻造成伤害（1.5*普攻伤害+100+自身谋略值*2）
    # ['1.5','100',['3','2']]
    def parse_actual_info_three(self, enemy):
        info         = self.actual
        splitlist    = info.split(";")
        inf          = splitlist[2]
        res          = inf.split("|")
        splitlist[2] = res
        count        = 0
        hero         = self.HeroObj
        # 如果技能增加攻击属性，先增加瞬时攻击力
        if self.getSkillAttribute(1) != 0:
            hero.setSkillAttk(self.getSkillAttribute(1))
        mhurt = hero.getHurt(enemy)
        for i, k in enumerate(splitlist):
            if i == 0:
                v = eval(k)
                count += float(v)*mhurt
            elif i == 1:
                count += float(eval(k))
            elif i == 2:
                attr = self.HeroObj.get_attr_info(eval(k[0])) + self.getSkillAttribute(k[0])# 属性值 = 英雄自身属性 + 技能增加属性
                v = eval(k[1])
                count += attr*v
        return count


    # 解析类型4
    # 回复兵力
    # 3；20；200（（施法者谋略*20+固定值）*释放者剩余兵力百分比）	#
    # 给目标增加兵力（属性类型*20+200）
    def parse_actual_info_four(self):
        info = self.actual
        hero = self.HeroObj
        splitlist   = info.split(";")
        count = 0.0
        attr = self.HeroObj.get_attr_info(eval(splitlist[0]))+self.getSkillAttribute(eval(splitlist[0]))# 属性值 = 英雄自身属性 + 技能增加属性
        # for i, k in enumerate(splitlist):
        v = attr*float(eval(splitlist[1]))+float(eval(splitlist[2]))
        count = v*(hero.mBatt/float(hero.mTotBatt))
        return int(count)

    # 斩杀效果
    def parse_actual_info_six(self, enemy):
        c = 2 - (enemy.mBatt/float(enemy.mTotBatt))
        return float(c)

    # 吸血效果，根据技能伤害回复的兵力倍数，伤害*百分比
    def parse_actual_info_seven(self):
        c = eval(self.actual)
        return float(c)

    # 减CD效果
    def parse_actual_info_ten(self):
        c = eval(self.actual)
        return int(c)

    # 越战越勇
    def parse_actual_info_twelve(self):
        hero = self.HeroObj
        c = 2 - (hero.mBatt/float(hero.mTotBatt))
        return float(c)

    # BUFF效果
    # 1；23|1；24
    # buff获得的几率和buffid,多个BUFF用“|”分开
    def parse_actual_info_thirteen(self):
        info = self.actual
        splitlist   = info.split("|")
        bufflist    = []
        for k in splitlist:
            buffs    = k.split(";")
            m,n      = float(buffs[0]), int(buffs[1])
            a        = random.randint(1,100) # 触发概率
            if a <= m*100:  
                bufftemp = self.getBuffTemp(n)
                bufflist.append(bufftemp)
        return bufflist
        pass

    # 15 追击效果
    # 0.3；3；1.2（触发几率30%，武将攻击属性设定为谋略属性的1.2倍，然后进行攻击）
    # 触发几率，属性类型；倍数
    def parse_actual_info_fifteen(self):
        info = self.actual
        count = 0
        splitlist   = info.split(";")
        pre  = float(eval(splitlist[0]))
        att = int(eval(splitlist[1]))
        attr = self.HeroObj.get_attr_info(att) + self.getSkillAttribute(att) # 属性
        times  = float(eval(splitlist[2])) # 倍数
        a = random.randint(1,100) # 触发概率
        if a <= pre*100:
            count = attr*times
        return count

    # 16 复活效果
    # 0.3
    # 回复最大兵力的百分比
    def parse_actual_info_sixteen(self):
        c = eval(self.actual)
        return float(c)*self.HeroObj.mTotBatt
        pass

    # 指挥技能 [bufftemp,bufftemp,...]
    def parse_actual_info_twentytwo(self):
        info = self.actual
        splitlist   = info.split(";")
        l = []
        for i in splitlist:
            buffid = int(eval(i))
            bufftemp = self.getBuffTemp(buffid)
            l.append(bufftemp)
        return l


    #这里设置技能的类型除了自己就是别人(不区分其他)
    def setSkillEffect(self, hero, enemy=None):
        self.HeroObj = hero
        if self.skillty == 1:  #绝对伤害
            c = round(eval(self.actual))
            hero.setSkillHurt(c)
            return c
        elif self.skillty == 2: #加成属性，然后造成伤害
            c    = self.parse_actual_info_two()
            hurt = hero.getSkillHurt(enemy, c)
            hero.setSkillHurt(hurt)
            return hurt
        elif self.skillty == 3: #伤害加成
            if enemy is None: return 0
            c = self.parse_actual_info_three(enemy)
            hero.setSkillHurt(c)
            return c
        elif self.skillty == 4: #回复兵力
            c = self.parse_actual_info_four()
            if enemy:enemy.setResumedBatt(c)
            return c
        elif self.skillty == 5: #回复兵力
            c = self.actual.split(";")
            if c and enemy: enemy.SkillRemoveBuff(c)
            return c
        elif self.skillty == 6: #斩杀效果
            if enemy is None: return 0
            c = self.parse_actual_info_six(enemy)
            self.HeroObj.setKillEffect(c)
            return c
        elif self.skillty == 7: #吸血效果
            c = self.parse_actual_info_seven()
            if enemy: enemy.setSuckBlood(c)
            return c
        elif self.skillty == 10:  # 减CD效果，减少CD的回合数 (这里是减少技能不是 减少自身回合数)
            c = self.parse_actual_info_ten()
            if enemy: enemy.setHeroRound(c)
            return c
        elif self.skillty == 12: # 越战越勇效果
            c = self.parse_actual_info_twelve()
            if enemy: enemy.setOverFight(c)
            return c
        elif self.skillty == 13: # 获得BUFF效果
            c = self.parse_actual_info_thirteen()
            if enemy: enemy.addBuffer(hero, c)
            return [buffer.id for buffer in c]
        elif self.skillty == 15: # 追击效果
            c = self.parse_actual_info_fifteen()
            if enemy: enemy.setHeroHurt(c)
            return c
        elif self.skillty == 16: # 复活效果
            c = self.parse_actual_info_sixteen()
            if enemy: enemy.setSuckBlood(c)
            return c
        elif self.skillty == 22:
            c = self.parse_actual_info_twentytwo()
            if enemy: enemy.addBuffer(hero, c)
            return [buffer.id for buffer in c]
        return 0 

    '''
    #targettype 0=敌方,1=自己,2=友军
    def setSkillEffect(self, hero=None, enemy=None, targettype=0):
        self.HeroObj = hero
        if self.skillty == 1:
            # 绝对伤害
            c = round(eval(self.actual))
            if targettype == 1:
                self.HeroObj.setSkillHurt(c)
            else:
                if enemy: enemy.setSkillHurt(c)
            return c
        elif self.skillty == 2:
            # 加成属性，然后造成伤害
            c = self.parse_actual_info_two()
            if targettype == 1:
                self.HeroObj.setSkillHurt(c)
            else:
                if enemy: enemy.setSkillHurt(c)
            return c
        elif self.skillty == 3:
            # 伤害加成
            if enemy == None:return 0
            c = self.parse_actual_info_three(enemy)
            self.HeroObj.setSkillHurt(c)
            return c
        elif self.skillty == 4:
            # 回复兵力
            c = self.parse_actual_info_four()
            if targettype == 1:
                self.HeroObj.setResumedBatt(c)
            else:
                if enemy: enemy.setResumedBatt(c)
            return c
        elif self.skillty == 5:
            # 清除BUFF组号
            c = self.actual.split(";")
            if c:
                if targettype == 1:
                    self.HeroObj.SkillRemoveBuff(c)
                else:
                    if enemy: enemy.SkillRemoveBuff(c)
                return c
            else:
                return []
        elif self.skillty == 6:
            # 斩杀效果
            c = self.parse_actual_info_six(enemy)
            self.HeroObj.setKillEffect(c)
            return c
        elif self.skillty == 7:
            # 吸血效果
            print "=====================吸血效果"
            c = self.parse_actual_info_seven()
            if targettype == 1:
                self.HeroObj.setSuckBlood(c)
            else:
                if enemy: enemy.setSuckBlood(c)    
            return c
        elif self.skillty == 10:
            # 减CD效果，减少CD的回合数 (这里是减少技能不是 减少自身回合数)
            c = self.parse_actual_info_ten()
            if targettype == 1:
                self.HeroObj.setHeroRound(c)
            else:
                if enemy: enemy.setHeroRound(c)
            return c
        elif self.skillty == 12:
            # 越战越勇效果
            c = self.parse_actual_info_twelve()
            if targettype == 1:
                self.HeroObj.setOverFight(c)
            else:
                if enemy: enemy.setOverFight(c)
            return c
        elif self.skillty == 13:
            # 获得BUFF效果
            c = self.parse_actual_info_thirteen()
            if targettype == 1:
                self.HeroObj.addBuffer(c)
            else:
                if enemy: enemy.addBuffer(c)
            return [buffer.id for buffer in c]
        elif self.skillty == 15:
            # 追击效果
            c = self.parse_actual_info_fifteen()
            if targettype == 1:
                self.HeroObj.setHeroHurt(c)
            else:
                if enemy: enemy.setHeroHurt(c)
            return c
        elif self.skillty == 16:
            # 复活效果
            print "=====================复活效果"
            c = self.parse_actual_info_sixteen()
            if targettype ==1:
                self.HeroObj.setSuckBlood(c)
            else:
                if enemy: enemy.setSuckBlood(c)
            return c
        elif self.skillty == 22:
            c = self.parse_actual_info_twentytwo()
            if targettype == 1:
                self.HeroObj.addBuffer(c)
            else:
                if enemy: enemy.addBuffer(c)
            return [buffer.id for buffer in c]
        pass
        '''

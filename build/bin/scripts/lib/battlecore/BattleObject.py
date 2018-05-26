# -*- coding: utf-8 -*-
#!/usr/local/bin/python2.5

import copy
import stackless
import traceback
import time, datetime, base64
from lib.protocol import *
from BattleDefine import *
from BattleConfig import BattleConfig
from BattleUnit   import BattleUnit
from BattleNotify import BattleNotify


BATTLE_STATUS_INIT              =   1
BATTLE_STATUS_PROCESS           =   2
BATTLE_STATUS_STANDBY           =   3
BATTLE_STATUS_END               =   4

BATTLE_INIT_INFO                = WarKey.PK_WAR_INIT_INFO
BATTLE_PROCESS_INFO             = WarKey.PK_WAR_PROCESS_INFO
BATTLE_ATK_TEAM                 = WarKey.PK_WAR_ATK_TEAM
BATTLE_DEF_TEAM                 = WarKey.PK_WAR_DEF_TEAM
BATTLE_REWARD                   = WarKey.PK_WAR_REWARD
BATTLE_END                      = WarKey.PK_WAR_END
BATTLE_IS_ATTACK                = WarKey.PK_WAR_IS_ATTACK
BATTLE_DEFAULT_BUFF             = WarKey.PK_WAR_DEFAULT_BUFF
BATTLE_RESULT                   = WarKey.PK_WAR_RESULT
BATTLE_EXP                      = WarKey.PK_WAR_EXP
BATTLE_COIN                     = WarKey.PK_WAR_COIN
BATTLE_ITEMS                    = WarKey.PK_WAR_ITENS
BATTLE_INFO                     = WarKey.PK_WAR_INFO
BATTLE_ATTACK_UID               = WarKey.PK_WAR_ATTACK_UID
BATTLE_DEFE_UID                 = WarKey.PK_WAR_DEFE_UID
BATTLE_POS                      = WarKey.PK_WAR_POS
BATTLE_TYPE                     = WarKey.PK_WAR_TYPE 
BATTLE_START_TIME               = WarKey.PK_WAR_START_TIME
BATTLE_END_TIME                 = WarKey.PK_WAR_END_TIME
BATTLE_DEFE_NAME                = WarKey.PK_WAR_DEFE_NAME
BATTLE_REPORT_TYPE              = WarKey.PK_WAR_REPORT_TYPE
BATTLE_ATTK_NAME                = WarKey.PK_WAR_ATTK_NAME
BATTLE_AD_NAME                  = WarKey.PK_WAR_AD_NAME
BATTLE_FD_NAME                  = WarKey.PK_WAR_FD_NAME

class BattleObject(object):

    def __init__(self, role=None, fid=0, uid=0, tid=0, pos=(0,0), tp=0, name="", isAuto=True, isShowBattle=False, adname="", fdname=""):
        self.role           = role                  #攻击方角色
        self.fid            = fid                   #攻击方部队ID
        self.uid            = uid                   #防守方角色ID 0=怪物
        self.tid            = tid                   #防守方部队ID
        self.isAuto         = isAuto                #自动初始化部队数据
        self.isShowBattle   = isShowBattle          #是否通知前段进入战斗 
        self._round         = 1                     #回合
        self._heros         = None                  #我方(攻击方)
        self._enemys        = None                  #敌方(被攻击方)
        self._result        = 0                     #结果 0=失败,1=未战
        self.ismaster       = (uid > 0) and 1 or 2  #1=玩家,2=怪物
        self.type           = tp                    # 0=迁城,1=采集,2=驻扎,3=攻击,4=运输,5=侦查,6=通商,7=战役
        self.target         = self.ismaster -1      # 0=玩家,1=野怪
        self.pos            = pos                   #坐标
        self.name           = name                  #防守方名字
        self.rname          = role.name             #攻击方名字
        self.adname         = adname                #攻击方同盟名字
        self.fdname         = fdname                #防守方同盟名字
        self.starttime      = int(time.time())
        self.endtime        = int(time.time())
        self._status        = BATTLE_STATUS_INIT
        self.init_pack      = {BATTLE_ATK_TEAM:[],BATTLE_DEF_TEAM:[], BATTLE_ATTACK_UID:role.UID,BATTLE_DEFE_UID:uid, BATTLE_DEFAULT_BUFF:{},BATTLE_TYPE:self.target,\
                               BATTLE_POS:self.pos, BATTLE_START_TIME:self.starttime, BATTLE_DEFE_NAME:self.name, BATTLE_ATTK_NAME:self.rname, BATTLE_REPORT_TYPE:self.type,\
                               BATTLE_AD_NAME:self.adname, BATTLE_FD_NAME:self.fdname}     #攻防部队数据
        self.process        = {}                    #处理的每回合流程
        self.reward         = []                    #最后的奖励
        self.end            = {BATTLE_REWARD:self.reward,BATTLE_RESULT:self._result, BATTLE_END_TIME:self.endtime, BATTLE_ATK_TEAM:{},BATTLE_DEF_TEAM:{}}       #奖励  
        self.report         = {BATTLE_INIT_INFO:self.init_pack, BATTLE_PROCESS_INFO:self.process,BATTLE_END:self.end}
        self.BattleConfig   = BattleConfig.getInstance()
        self.__tasklet      = None
        self.__atomic       = None
        self.__lock         = 0             #0=正常,1=锁住  
        self.__InitBattleData()
        pass

    @property
    def result(self):
        return self._result

    @property
    def aforceinfo(self):
        batts,kills = {}, {}
        for pos, hero in self._heros.iteritems():
            batts[hero.iid] = -hero.diebatt
            kills[hero.iid] = hero.killbatt
        return {"hid":0, "fid":self.fid, "hp":0, "status":-1, "batt":batts, "hurt":kills, "block":1}

    @property
    def dforceinfo(self):
        batts,kills = {}, {}
        for pos, hero in self._enemys.iteritems():
            batts[hero.iid] = -hero.diebatt
            kills[hero.iid] = hero.killbatt
        return {"hid":0, "fid":self.tid, "hp":0, "status":-1, "batt":batts, "hurt":kills, "block":1} 
    
    def _enter(self):
        self.__lock    = 1
        self.__tasklet = stackless.getcurrent()
        self.__atomic  = self.__tasklet.set_atomic(True)
        pass

    def _leave(self):
        self.__tasklet.set_atomic(self.__atomic)
        self.__lock = 0
        pass

    #同步处理
    def sync(func):
        def wrapper(self, args=None):
            self._enter()
            res = None
            try:
                res = func(self, args)
            except Exception,ex:
                traceback.print_exc()
            self._leave()
            return res
        return wrapper


    #格式化部队数据 {占位:战斗单元}
    def __FormatForceData(self, fid, info={}):
        battleunits = {}
        forcedata   = info.get(fid, {})
        for rol, herodict in forcedata.iteritems():
            for i in xrange(1,4):
                heroinfo = herodict.get(i, None)
                if heroinfo is None: continue
                pos = (rol - 1) * 3 + i
                battunit         = BattleUnit(self.BattleConfig)
                battunit.InitHeroData(heroinfo, pos)
                battleunits[pos] = battunit
        return battleunits


    #获取角色部队数据 返回的 {部队ID:{1:{1:部队信息",2:"",3:"",......}}}
    def __getRoleForceInfo(self, uid, fid):
        res, info = BattleNotify.getInstance().s2sForceInfo(self.role, fid, uid)
        if res < 0: return res
        try:
            data = self.__FormatForceData(fid, info)
        except Exception, e:
            print ">>>>>>>>>>>>>=__getRoleForceInfo========err: %s=========="%e
        
        return data

    #获取怪物部队数据
    def __getMonsterForceInfo(self, fid):
        MonsterTeam = self.BattleConfig.getMonsterTeam(fid)
        if MonsterTeam is None: return {}
        res = MonsterTeam.get_battle_unit()
        return res 

    #初始化攻击方和防守方部队数据
    def __InitBattleData(self):
        if not self.isAuto: return
        #攻击方的数据
        self._heros         = self.__getRoleForceInfo(self.role.uid, self.fid)     #我方(攻击方)
        #防守方数据
        if self.uid != 0:
            self._enemys         = self.__getRoleForceInfo(self.uid, self.tid)     #敌方(被攻击方)
        else:
            self._enemys         = self.__getMonsterForceInfo(self.tid)
        self.__InitBattleInfo()
        pass

    #初始化战斗信息 (角色自己处理 格式化 部队数据)
    def InitBattleData(self, AttackDict={}, DefeDict={}, monsterid = 0):
        self._heros         = AttackDict     #我方(攻击方)
        self._enemys        = DefeDict       #敌方(被攻击方)
        if monsterid        != 0:
            mHeroTeam  = self.BattleConfig.getMonsterTeam(monsterid)
            if mHeroTeam is None: return
            self._enemys = mHeroTeam.get_battle_unit()
        self.__InitBattleInfo()
        pass

    #战报添加攻击成员
    def add_attk_member(self, hero):
        #挂上被动技能的buff
        self.init_pack[BATTLE_ATK_TEAM].append(hero.pack)
        self.init_pack[BATTLE_DEFAULT_BUFF][hero.iid] = copy.copy(hero.buffinfo)
        pass
    
    #战报添加防守成员
    def add_defe_member(self, hero):
        #挂上被动技能的buff
        self.init_pack[BATTLE_DEF_TEAM].append(hero.pack)
        self.init_pack[BATTLE_DEFAULT_BUFF][hero.iid] = copy.copy(hero.buffinfo)
        pass

    #添加每回合数据
    def add_round_info(self, round, rounddata):
        if self.process.has_key(round) == False:
            self.process[round] = []
        self.process[round].append(rounddata)
        pass

    #设置奖励
    def set_reward_info(self, gold=0, coin=0, wood=0, food=0, iron=0, exp=0, items={}):
        self.reward = [gold, coin, wood, food, iron, items, exp]
        self.end[BATTLE_REWARD] = self.reward
        pass

    #设置结束后武将信息
    def set_hero_end(self):
        #self.set_reward_info(0, 100, 100, 100, 100)
        self.end[BATTLE_RESULT] = self._result
        for pos, hero in self._heros.iteritems():
            self.end[BATTLE_ATK_TEAM][hero.iid] = hero.staticpack

        for pos, hero in self._enemys.iteritems():
            self.end[BATTLE_DEF_TEAM][hero.iid] = hero.staticpack
            
        #更新角色武将信息
        if self.type == 7: return 0   #战役不消耗兵力
        BattleNotify.getInstance().s2sUpdateHeroInfo(self.role, self.role.uid, **self.aforceinfo)
        if self.uid > 0: #更新其他角色信息
            BattleNotify.getInstance().s2sUpdateHeroInfo(self.role, self.uid, **self.dforceinfo)
        #结束战斗是否通知前段
        #BattleNotify.getInstance().s2sBattleReportInfo(self.role, self.report, self.uid, self.type, int(self.isShowBattle))
        pass

    #设置胜利方
    def set_win_role(self, tp=0):
        self.end[BATTLE_WIN] = tp
        pass            
        
    #初始化我方和敌方数据
    def __InitBattleInfo(self):
        for pos, hero in self._heros.iteritems():
            hero.HeroResetStatus()
            self.add_attk_member(hero)
            hero.mBattle = self
            hero.type    = 0
            hero.ReleasePassiveSkill()
        for pos, hero in self._enemys.iteritems():
            hero.HeroResetStatus()
            self.add_defe_member(hero)
            hero.mBattle = self
            hero.type    = 1
            hero.ReleasePassiveSkill()
        self._status     = BATTLE_STATUS_PROCESS
        pass
    
    #获取攻击方成员 0=取所有武将, 1=取己方武将,2=取敌方武将
    def get_batt_hero(self, hero=None, tp=0):
        if hero is None or tp == 0:         #取所有武将
            aheros = self._heros.values()
            dheros = self._enemys.values()
            return aheros + dheros
        elif hero.type == 0 and tp == 1:
            return self._heros.values()
        elif hero.type == 0 and tp == 2:
            return self._enemys.values()
        elif hero.type == 1 and tp == 1:
            return self._enemys.values() 
        elif hero.type == 1 and tp == 2:
            return self._heros.values()
        pass

    @sync
    def startWar(self, params=None):
        #开始战斗
        _report   = [] #战斗
        _heroinfo = {} #武将每一轮自己的数据
        _aHeros   = self._get_heros(self._heros.items())
        _dHeros   = self._get_heros(self._enemys.items())
        allheros  = self._get_heros(_aHeros+_dHeros)

        if len(_dHeros) == 0: #无防守方,未战
            print "=====================无人应战:%s==========%s"%(str(self.adname), str(self.fdname))
            self._result = 3
            self.set_hero_end()
            return 0
        #开始战斗逻辑
        while self._status == BATTLE_STATUS_PROCESS:
            aTeamUnitNum = len([unit for unit in self._heros.values() if  unit.mBatt>0])
            dTeamUnitNum = len([unit for unit in self._enemys.values() if unit.mBatt>0])
            for pos, hero in allheros:
                if hero.mBatt <=0: continue
                _heroinfo[hero] = copy.copy(hero.myinfo)
                hero.Action()
                aTeamUnitNum = len([unit for unit in self._heros.values() if  unit.mBatt>0])
                dTeamUnitNum = len([unit for unit in self._enemys.values() if unit.mBatt>0])
                if aTeamUnitNum<=0 or dTeamUnitNum<=0:
                    break
            if aTeamUnitNum <= 0 and dTeamUnitNum >0:
                self._result = 0
                self._status = BATTLE_STATUS_END
            elif aTeamUnitNum > 0 and dTeamUnitNum <= 0:
                self._result  = 1
                self._status = BATTLE_STATUS_END
            elif aTeamUnitNum == 0 and dTeamUnitNum == 0:
                self._result = 2
                self._status = BATTLE_STATUS_END
            
            round_data = []
            #清除所有武将的buff
            for pos, hero in allheros:
                if len(hero.rounddata) > 0:
                    round_data.append({hero.iid:[copy.copy(hero.buffinfo), hero.rounddata, copy.copy(hero.cleanbuff), _heroinfo.get(hero, [])]})
                hero.round_clear()
                hero.BalanceBuff(1)    #大回合结算buff
            self.add_round_info(self._round, round_data)
            print "=====================================================当前第%s回合结束"%str(self._round)
            self._round += 1
            if self._status == BATTLE_STATUS_END:
                print "=====================================================结果:%s"%str(self._result) 
                self.set_hero_end()
                break
            #如果大于30回合 攻击方失败
            if self._round >=31:
                self._status = BATTLE_STATUS_END
                self._result = 0
                self.set_hero_end()
                break
        return 0

    #按速度返回武将列表
    def _get_heros(self, heros):
        _heros = sorted(heros, key = lambda hero:hero[1].mSpeed if hero[1].mBatt else -1, reverse=True)
        return _heros 


if __name__ == "__main__":
    import sys
    sys.path.append("../lib")
    sys.path.append("../lib/protocol")
    pass

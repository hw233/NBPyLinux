# -*- coding: utf-8 -*-
#!/usr/local/bin/python2.5


import copy
import stackless
import traceback
import time, datetime, base64
from lib.protocol                import *
from BattleDefine                import *
from BattleConfig                import BattleConfig
from BattleUnit                  import BattleUnit
from lib.battlecore.BattleNotify import BattleNotify


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



#可以联系战斗,防守方不恢复兵力
class BossBattle(object):
    def __init__(self, fid=0, uid=0, tp=0, pos=(0,0), name="", defedict={}, isShowBatt=False):
        super(BossBattle, self).__init__()
        self.defedict       = defedict      #防守方兵力更新
        self.role           = None          #当前正在攻打的玩家
        self.aid            = 0             #攻击方部队ID
        self.fid         	= fid 			#防守方部队ID
        self.uid            = uid           #防守方角色ID 
        self.isShowBatt     = isShowBatt    #是否通知前段显示战斗
        self._round         = 1         	#回合
        self._heros         = None      	#我方(攻击方)
        self._enemys        = None      	#敌方(被攻击方)
        self._result        = 0         	#结果
        self.type           = tp            #0=迁城,1=采集,2=驻扎,3=攻击,4=运输,5=侦查,6=通商,7=战役
        self.target         = 1             #战斗类型 0=玩家,1=其他
        self.isbalance      = False         #是否结算
        self.forcedict      = {}            #攻击方部队信息 {uid:fid:{}}  
        self.pos            = pos
        self.name           = name
        self.starttime      = int(time.time())
        self.endtime        = int(time.time())
        self._status        = BATTLE_STATUS_INIT
        self.init_pack      = {BATTLE_ATK_TEAM:[],BATTLE_DEF_TEAM:[], BATTLE_ATTACK_UID:0,BATTLE_DEFE_UID:self.uid, BATTLE_DEFAULT_BUFF:{},BATTLE_TYPE:self.target,\
                               BATTLE_POS:self.pos, BATTLE_START_TIME:self.starttime, BATTLE_DEFE_NAME:self.name, BATTLE_ATTK_NAME:"", BATTLE_REPORT_TYPE:self.type} #攻防部队数据
        self.process        = {}                                                 #处理的每回合流程
        self.reward         = []                                                 #最后的奖励
        self.end            = {BATTLE_REWARD:self.reward,BATTLE_RESULT:self._result, BATTLE_END_TIME:self.endtime, BATTLE_ATK_TEAM:{},BATTLE_DEF_TEAM:{}}       #奖励
        self.report         = {BATTLE_INIT_INFO:self.init_pack, BATTLE_PROCESS_INFO:self.process,BATTLE_END:self.end}
        self.BattleConfig   = BattleConfig.getInstance()
        self.__tasklet      = None
        self.__atomic       = None
        self.__lock         = 0     #0=正常,1=锁住
        self.__InitBattleData()
        pass

    @property
    def result(self):
        return self._result

    @property
    def isShowBattle(self):
        return self.isShowBatt
    
    @property
    def defeTeam(self):
        return self.init_pack[BATTLE_DEF_TEAM]

    @property
    def aforceinfo(self):
        batts, kills = {}, {}
        for pos, hero in self._heros.iteritems():
            batts[hero.iid] = -hero.diebatt
            kills[hero.iid] = hero.killbatt
        return {"hid":0, "fid":self.aid, "hp":0, "status":-1, "batt":batts, "hurt":kills, "block":1}

    @property
    def dforceinfo(self):
        batts,kills = {}, {}
        for pos, hero in self._enemys.iteritems():
            batts[hero.iid] = -hero.diebatt
            kills[hero.iid] = hero.killbatt
        return {"hid":0, "fid":self.aid, "hp":0, "status":-1, "batt":batts, "hurt":kills, "block":1} 

    @property
    def defeheros(self):
        herodict = {}
        for pos, herounit in self._enemys.iteritems():
            herodict[herounit.hid] = herounit.mBatt
        return herodict

    def _enter(self):
        self.__tasklet = stackless.getcurrent()
        self.__atomic  = self.__tasklet.set_atomic(True)
        pass

    def _leave(self):
        self.__tasklet.set_atomic(self.__atomic)
        pass

    #同步处理
    def sync(func):
        def wrapper(self, role=None, fid=0, parmas={}):
            self._enter()
            res = None
            try:
                res = func(self, role, fid, parmas)
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
        #print "========================获取部队数据===================================="
        res, info = BattleNotify.getInstance().s2sForceInfo(self.role, fid, uid)
        if res < 0: return res
        data = self.__FormatForceData(fid, info)
        return data

    #获取怪物部队数据
    def __getMonsterForceInfo(self, fid):
        MonsterTeam = self.BattleConfig.getMonsterTeam(fid)
        if MonsterTeam is None: return {}
        res = MonsterTeam.get_battle_unit()
        return res

    #初始化攻击方和防守方部队数据
    def __InitBattleData(self):
        #防守方数据
        self._enemys         = self.__getMonsterForceInfo(self.fid)
        #print "========================防守方数据:%s"%str(self._enemys)
        if len(self.defedict) >0:
            for pos, herounit in self._enemys.iteritems():
                herounit.mBatt    = self.defedict.get(herounit.hid, herounit.mBatt)
                herounit.mTotBatt = herounit.mBatt                  
        self.__InitDefeInfo()
        pass

    #更新防守方数据
    def UpdateDefeTeam(self, defeheros={}):
        pass

    #初始化战斗信息 (角色自己处理 格式化 部队数据)
    @sync
    def startBattle(self, role, fid, attkinfo):
        if self.__lock == 1: return
        self.__lock = 1
        try:
            self.aid                          = fid     #攻击方部队ID
            self.role      					  = role
            if len(attkinfo) == 0:
                self._heros         		  = self.__getRoleForceInfo(role.uid, fid)     #(攻击方)
            else:
                self._heros                   = attkinfo
            self.init_pack[BATTLE_ATTACK_UID] = role.uid
            self.init_pack[BATTLE_ATTK_NAME]  = role.name
            self.__InitAttackInfo()
            self._status     				  = BATTLE_STATUS_PROCESS
            self.__StartBattle()
        except Exception,ex:
            print "==============startBattle",ex
            pass
        self.__lock = 0
        pass


    #重置攻击方
    def __ResetAttackInfo(self):
    	self.init_pack[BATTLE_ATK_TEAM] 	= []
    	self.init_pack[BATTLE_DEFAULT_BUFF] = {}
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

    #最后结算攻击方兵力
    def saveAttkForceInfo(self):
        self.forcedict = {}
        batt           = 0
        for pos, hero in self._heros.iteritems():
            mHero                = copy.copy(hero)
            if mHero.mBatt       >= mHero.mTotBatt: #如果无兵力损失则 等于之前兵力
                mHero.mBatt      = mHero.mTotBatt
            else:
                mHero.mTotBatt   = mHero.mBatt
            self.forcedict[pos]  = mHero
            batt += hero.mBatt
        return self.forcedict
    
    #设置结束后武将信息
    def set_hero_end(self):
        self.end[BATTLE_RESULT] = self._result
        for pos, hero in self._heros.iteritems():
            self.end[BATTLE_ATK_TEAM][hero.iid] = hero.staticpack

        for pos, hero in self._enemys.iteritems():
            self.end[BATTLE_DEF_TEAM][hero.iid] = hero.staticpack

        #更新武将部队的数据
        self.saveAttkForceInfo()
        #更新角色武将信息
        #if self.type == 7: return 0   #战役不消耗兵力
        #res = BattleNotify.getInstance().s2sUpdateHeroInfo(self.role, self.role.uid, **self.aforceinfo)
        #print "=========当前战报结果:%s, 当前战报:%s"%(str(self._result), str(self._report))
        #if self.uid > 0: #更新其他角色信息
        #    res = BattleNotify.getInstance().s2sUpdateHeroInfo(self.role, self.uid, **self.dforceinfo)
        return 0

    #更新角色武将信息
    def UpdateRoleHeroInfo(self):
        print "==================================更新角色武将信息"
        res = BattleNotify.getInstance().s2sUpdateHeroInfo(self.role, self.role.uid, **self.aforceinfo)
        #print "=========当前战报结果:%s, 当前战报:%s"%(str(self._result), str(self._report))
        if self.uid > 0: #更新其他角色信息
            res = BattleNotify.getInstance().s2sUpdateHeroInfo(self.role, self.uid, **self.dforceinfo)
        return res

    #设置胜利方
    def set_win_role(self, tp=0):
        self.end[BATTLE_WIN] = tp
        pass

    #初始化我方和敌方数据
    def __InitAttackInfo(self):
        for pos, hero in self._heros.iteritems():
            hero.HeroResetStatus()
            self.add_attk_member(hero)
            hero.mBattle = self
            hero.type    = 0
        pass

    #初始化防守方数据
    def __InitDefeInfo(self):
        for pos, hero in self._enemys.iteritems():
            hero.HeroResetStatus()
            if hero.mBatt == 0: continue
            self.add_defe_member(hero)
            hero.mBattle = self
            hero.type    = 1
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

    def __StartBattle(self):
        #开始战斗
        _report   = [] #战斗
        _heroinfo = {} #武将每一轮自己的数据
        _aHeros   = self._get_heros(self._heros.items())
        _dHeros   = self._get_heros(self._enemys.items())
        allheros  = self._get_heros(_aHeros+_dHeros)
        if len(_dHeros) == 0: #无防守方,未战
            self._result = 3
            self.set_hero_end()
            return 0
        #开始战斗逻辑
        while self._status == BATTLE_STATUS_PROCESS:
            print "==========================================开始战斗==========================="
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
            self._round += 1
            if self._status == BATTLE_STATUS_END:
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


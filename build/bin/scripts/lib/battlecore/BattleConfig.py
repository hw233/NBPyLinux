#coding: utf-8


import Singleton, copy
from CSVParser          import SmartCSVParser
from HeroTemp           import *
from SkillTemp          import *
from BuffTemp           import *
from MonsterTemp        import *
from MonsterTeamTemp    import *

class BattleConfig(Singleton.Singleton):
    def __init__(self):
        self.__monsterDict      = {}    #怪物英雄
        self.__monsterTeam      = {}    #怪物队伍
        self.__skillDict        = {}    #技能模板
        self.__buffDict         = {}    #武将buff   
        self.__heroDict         = {}    #武将技能 
        self.__init_data()
        pass

    @staticmethod
    def getInstance():
        return Singleton.getInstance(BattleConfig)

    #初始化表数据
    def __init_data(self):
        self.__init_monster_hero()
        self.__init_team_monster()
        self.__init_skill()
        self.__init_buff()
        self.__init_hero()
        pass

    def __init_monster_hero(self):
        # 怪物属性数据
        rows = SmartCSVParser("monster.csv").Parse()
        if rows is None:print 'monster config error';return
        try:
            for row in rows:
                monster = MonsterTemp(row)
                self.__monsterDict[monster.id] = monster
        except Exception,ex:
            print ex,'>>>>>>monster>>>>>>',row
        pass


    def __init_team_monster(self):
        # 怪物队伍
        rows = SmartCSVParser("team_monster.csv").Parse()
        if rows is None:print 'team_monster config error';return
        try:
            for row in rows:
                obj = MonsterTeamTemp(self, row)
                self.__monsterTeam[obj.id] = obj
        except Exception,ex:
            print ex,'>>>>>>team_monster>>>>>>',row
        pass

    def __init_skill(self):
        # 技能模板
        rows = SmartCSVParser("skill.csv").Parse()
        if rows is None:print 'skill config error';return
        try:
            for row in rows:
                obj = SkillTemp(self, row)
                if self.__skillDict.has_key(obj.id) == False:
                    self.__skillDict[obj.id] = {}
                self.__skillDict[obj.id][obj.level] = obj
        except Exception,ex:
            print ex,'>>>>>>skill>>>>>>',row
        pass
    
    def __init_hero(self):
        # 武将对应的技能
        rows = SmartCSVParser("hero.csv").Parse()
        if rows is None:print 'hero config error';return
        try:
            for row in rows:
                mHeroTemp = HeroTemp(row)
                self.__heroDict[mHeroTemp.id] = mHeroTemp
        except Exception,ex:
            print ex,'>>>>>>hero>>>>>>',row
        pass

    def __init_buff(self):
        # buff模板
        rows = SmartCSVParser("buff.csv").Parse()
        if rows is None:print 'buff config error';return
     
        for row in rows:
            obj = BuffTemp(row)
            self.__buffDict[obj.id] = obj
        try:
            pass
        except Exception,ex:
            print ex,'>>>>>>buff>>>>>>',row
        pass
    
    #=====================================================================
    #获取怪物
    def getMonsterInfo(self, iid):
        return self.__monsterDict.get(iid, None)

    #获取怪物队伍信息
    def getMonsterTeam(self, iid):
        return self.__monsterTeam.get(iid)

    #获取技能模板
    def getSkillTemp(self, skillid=0, level=0):
        skilltype = self.__skillDict.get(skillid, {}).get(level, None)
        if skilltype is None: return None
        return copy.copy(skilltype)

    #获取buff
    def getBuffTemp(self, bid):
        buffer = self.__buffDict.get(bid, None)
        if buffer is None: return None
        return copy.copy(buffer)

    #获取武将技能
    def getHeroSkillInfo(self, hid, tp=0):
        if tp == 0:
            hero = self.__heroDict.get(hid, None)
        else:
            hero = self.__monsterDict.get(hid, None)
        if hero is not None: return hero.get_heros_skill()
        return {}
    
    #获取武将
    def get_hero_info(self, hid):
        hero = self.__heroDict.get(hid, None)
        return hero
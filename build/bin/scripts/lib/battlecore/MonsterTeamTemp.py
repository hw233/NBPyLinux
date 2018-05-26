#!coding:utf-8
#!/usr/bin/python

from BattleUnit import BattleUnit

class MonsterTeamTemp(object):
    '''怪物部队'''
    def __init__(self, BattleConfig=None, row=None):
        self.id                  =  0                             # 怪物部队编号
        self.main                =  0                             # 主将    
        self.hero_1              =  None                          # 部队武将1
        self.hero_2              =  None                          # 部队武将2
        self.hero_3              =  None                          # 部队武将3
        self.hero_4              =  None                          # 部队武将4
        self.hero_5              =  None                          # 部队武将5
        self.forcedict           = {}                             # {占位:战斗单元}  
        self.BattleConfig        = BattleConfig.getInstance()     # 战斗配置
        self.__init_force(row)
        pass


    #初始化部队
    def __init_force(self, row):
        if row is None: return
        self.id                  =  int(row["id"])             
        self.main                =  int(row["coach"])
        self.hero_1              =  self.BattleConfig.getMonsterInfo(int(row["num1"]))
        self.hero_2              =  self.BattleConfig.getMonsterInfo(int(row["num2"]))
        self.hero_3              =  self.BattleConfig.getMonsterInfo(int(row["num3"]))
        self.hero_4              =  self.BattleConfig.getMonsterInfo(int(row["num4"]))
        self.hero_5              =  self.BattleConfig.getMonsterInfo(int(row["num5"]))
        self.__init_force_pos()
        pass

    @property
    def herolist(self):
        return [self.hero_1,self.hero_2, self.hero_3, self.hero_4, self.hero_5]

    #初始化武将占位
    def __init_force_pos(self):
        _mHeroList = [MonsterTemp for MonsterTemp in self.herolist if MonsterTemp is not None]
        _heroform  = [[],[],[]]
        _mHeroList = sorted(_mHeroList, key=lambda monster:(monster.range, monster.id), reverse=False) #根据攻击距离和ID进行排序
        
        for hero in _mHeroList:
            if hero.range == 1 and len(_heroform[0]) < 3:
                _heroform[0].append(hero)
            elif hero.range == 1 and len(_heroform[0]) >= 3 and len(_heroform[1]) < 3:
                _heroform[1].append(hero)
            elif hero.range == 2 and len(_heroform[1]) < 3:
                _heroform[1].append(hero)
            elif hero.range == 2 and len(_heroform[1]) >= 3 and len(_heroform[2]) < 3:
                _heroform[2].append(hero)
            elif hero.range == 3 and len(_heroform[2]) < 3:
                _heroform[2].append(hero)
            elif hero.range == 3 and len(_heroform[2]) >= 3 and len(_heroform[1]) < 3:
                _heroform[1].append(hero)
        #计算出来后,对阵型武将分配位置
        for poslist in _heroform:
            idx = _heroform.index(poslist) + 1  
           
            _poslist = [ m + (idx - 1) * 3 for m in xrange(1, 4)]
            if len(poslist) == 0: #当前列无武将
                for pos in _poslist:
                    self.forcedict[pos] = None
                
            elif len(poslist) == 1: #当前列有1个武将
                 for pos in _poslist:
                    if _poslist.index(pos) == 1:
                        self.forcedict[pos] = poslist[0]
            elif len(poslist) == 2: #当前列有2个武将
                for pos in _poslist:
                    if _poslist.index(pos) == 0:
                        self.forcedict[pos] = poslist[0]
                    elif _poslist.index(pos) == 2:
                        self.forcedict[pos] = poslist[1]
                    else:
                        self.forcedict[pos] = None
            else:                   #当前列有3个武将
                for pos in _poslist:
                    if _poslist.index(pos) == 0:
                        self.forcedict[pos] = poslist[0]
                    elif _poslist.index(pos) == 1:
                        self.forcedict[pos] = poslist[1]
                    elif _poslist.index(pos) == 2:
                        self.forcedict[pos] = poslist[2]      
        pass

    #获取战斗单元
    def get_battle_unit(self):
        forcedict = {}
        for pos, mosterTemp in self.forcedict.iteritems():
            if not mosterTemp: continue
            mUnit          = BattleUnit(self.BattleConfig)
            if self.main == mosterTemp.id:
                mUnit.InitMonsterData(mosterTemp, pos, 1)
            else:
                mUnit.InitMonsterData(mosterTemp, pos, 0)
            forcedict[pos] = mUnit
        return forcedict

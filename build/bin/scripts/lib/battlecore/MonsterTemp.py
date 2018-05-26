#!coding:utf-8
#!/usr/bin/python

class MonsterTemp(object):
    '''怪物模版'''
    def __init__(self, row=None):
        self.id                  =  0             # 怪物编号
        self.name                =  ""            # 怪物名字
        self.level               =  1             # 怪物等级
        self.star                =  0             # 怪物星级    
        self.type                =  0             # 怪物类型 1、普通 2、精英 3、BOSS 4、世界BOSS 5、特殊
        self.atk                 =  0             # 怪物攻击
        self.defence             =  0             # 怪物防御
        self.strategy            =  0             # 怪物谋略
        self.troop               =  0             # 怪物兵力
        self.speed               =  0             # 怪物速度
        self.sex                 =  0             # 怪物性别
        self.head                =  ""            # 怪物头像
        self.quality             =  0             # 怪物品质
        self.body                =  ""            # 怪物全身像
        self.sarms               =  0             # 怪物兵种类型(大类型)
        self.marms               =  0             # 怪物兵种类型(小类型)
        self.skill1              =  ""            # 怪物技能1
        self.skill2              =  ""            # 怪物技能2
        self.skill3              =  ""            # 怪物技能3
        self.hit                 =  0             # 怪物命中
        self.miss                =  0             # 怪物闪避
        self.critical            =  0             # 怪物暴击
        self.range               =  0             # 怪物攻击距离 
        self.__init_hero(row)
        pass

    
    #初始化角色
    def __init_hero(self, row):
        if row is None: return
        
        self.id                  =  int(row["id"])             
        self.name                =  str(row["name"])            
        self.level               =  int(row["level"])             
        self.type                =  int(row["type"])            
        self.atk                 =  int(row["atk"])             
        self.defence             =  int(row["defence"])                 
        self.strategy            =  int(row["strategy"])             
        self.troop               =  int(row["troop"])             
        self.speed               =  int(row["speed"])            
        self.sex                 =  int(row["sex"])             
        self.head                =  int(row["head"])            
        self.quality             =  int(row["quality"])             
        self.body                =  int(row["body"])            
        self.sarms               =  int(row["arms"])           
        self.marms               =  int(row["bingzhong"])             
        self.skill1              =  str(row["skill1"])             
        self.skill2              =  str(row["skill2"])             
        self.skill3              =  str(row["skill3"])             
        self.hit                 =  int(row["hit"])             
        self.miss                =  int(row["miss"])             
        self.critical            =  int(row["critical"])             
        self.range               =  int(row["range"])             

        pass

    def get_heros_skill(self):
        _skill1     = self.skill1.replace(";", ":")
        _skill2     = self.skill2.replace(";", ":")
        _skill3     = self.skill3.replace(";", ":")
        skillinfo   = "{%s, %s, %s}"%(_skill1, _skill2, _skill3)
        try:
            return eval(skillinfo)
        except Exception,ex:
            return {}


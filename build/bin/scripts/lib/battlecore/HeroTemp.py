#!coding:utf-8
#!/usr/bin/python
'''
三国

HeroTemp
'''

'''
@Created    : date
@author     : axing_
@file info  :
'''

#武将模板
class HeroTemp(object):
    def __init__(self, row):
        super(HeroTemp, self).__init__()
        self.id                  =  0             # 模版ID
        self.name                =  ""            # 名字
        self.sex                 =  0             # 性别
        self.head                =  ""            # 头像
        self.body                =  ""            # 全身像
        self.level               =  0             # 等级
        self.sarms               =  0             # 兵种类型(大类型)
        self.marms               =  0             # 兵种类型(小类型)
        self.quality             =  0             # 品质
        self.range               =  0             # 攻击距离 
        self.country             =  0             # 国家
        self.rank                =  0             # 星级
        self.need                =  ""            # 合成材料
        self.power               =  0             # 攻击
        self.grow_pow            =  0             # 攻击成长(影响力量)
        self.defe                =  0             # 防御
        self.grow_def            =  0             # 防御成长 
        self.intel               =  0             # 谋略  
        self.grow_int            =  0             # 谋略成长(影响智力)
        self.phy                 =  0             # 统帅
        self.grow_phy            =  0             # 统帅成长
        self.hit                 =  0             # 命中
        self.miss                =  0             # 闪避
        self.critical            =  0             # 暴击
        self.speed               =  0             # 速度
        self.skill1              =  0             # 技能1
        self.skill2              =  0             # 技能2
        self.skill3              =  0             # 技能3
        self.friendid1           =  0             # 羁绊1  
        self.friendid2           =  0             # 羁绊2  
        self.friendid3           =  0             # 羁绊3  
        self.friendid4           =  0             # 羁绊4
        self.num_dan             =  0             # 分解获得
        self.text                =  0             # 文字描述
        self.mus_skill2          =  ""            # 技能2触发条件 1等级,2星级
        self.mus_skill3          =  ""            # 技能3触发条件
        self.fetterlist          =  []            # 羁绊的列表
        self.__init_hero(row)

        pass


    #初始化角色
    def __init_hero(self, row):
        self.id                  =  int(row["id"])             # 模版ID
        self.name                =  str(row["name"])           # 名字
        self.sex                 =  int(row["sex"])            # 性别
        self.head                =  str(row["head"])           # 头像
        self.body                =  str(row["body"])           # 全身像
        self.level               =  int(row["lev"])          # 等级
        self.sarms               =  int(row["arms"])           # 兵种类型(大类型)
        self.marms               =  int(row["bingzhong"])      # 兵种类型(小类型)
        self.quality             =  int(row["quality"])        # 品质
        self.range               =  int(row["range"])          # 攻击距离 
        self.country             =  int(row["country"])        # 国家
        self.rank                =  int(row["rank"])           # 星级
        self.need                =  str(row["need"])           # 合成材料
        self.power               =  int(row["power"])          # 攻击
        self.grow_pow            =  int(row["grow_pow"])       # 攻击成长(影响力量)
        self.defe                =  int(row["def"])            # 防御
        self.grow_defe           =  int(row["grow_def"])       # 防御成长
        self.intel               =  int(row["intel"])          # 谋略  
        self.grow_int            =  int(row["grow_int"])       # 谋略成长(影响智力)
        self.phy                 =  int(row["phy"])            # 统帅
        self.grow_phy            =  int(row["grow_phy"])       # 统帅成长
        self.hit                 =  int(row["hit"])            # 命中
        self.miss                =  int(row["miss"])           # 闪避
        self.critical            =  int(row["critical"])       # 暴击
        self.speed               =  int(row["speed"])          # 速度
        self.skill1              =  int(row["skill1"])         # 技能1
        self.skill2              =  int(row["skill2"])         # 技能2
        self.skill3              =  int(row["skill3"])         # 技能3
        self.friendid1           =  int(row["friendid1"])      # 羁绊1  
        self.friendid2           =  int(row["friendid2"])      # 羁绊2  
        self.friendid3           =  int(row["friendid3"])      # 羁绊3  
        self.friendid4           =  int(row["friendid4"])      # 羁绊4
        self.num_dan             =  int(row["num_dan"])        # 分解获得
        self.text                =  str(row["text"])           # 文字描述
        self.mus_skill2          =  str(row["mus_skill2"])     # 技能2触发条件 1等级,2星级
        self.mus_skill3          =  str(row["mus_skill3"])     # 技能3触发条件
        pass

    def get_heros_skill(self):
        return {1:self.skill1,2:self.skill2,3:self.skill3}
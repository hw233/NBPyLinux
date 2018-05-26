#coding: utf-8
'''
三国

RoleConfig
'''
import copy
import Singleton
from CSVParser              import SmartCSVParser

#游戏全局配置参数表1
class GameGlobalConfig(Singleton.Singleton):
    def __init__(self):
        super(GameGlobalConfig, self).__init__()
        self.datainfo = {} #数据
        self.__init_data()
        pass

    @staticmethod
    def getInstance():
        return Singleton.getInstance(GameGlobalConfig)

    
    #初始化表数据
    def __init_data(self):
        rows = SmartCSVParser("parmeter.csv").Parse()
        if rows is None:print '__parmeter config error '; return
        for row in rows:
            try:
                self.datainfo[int(row["id"])] = int(row["parameter"])
            except Exception,ex:
                self.datainfo[int(row["id"])] = str(row["parameter"])
                #print ex,'>>>>>>>>>>>>',row
        pass

    #获取表数据
    def get_global_info(self, id):
        return self.datainfo.get(id, 0)    



# -*- coding: utf-8 -*-
#!/usr/local/bin/python2.5


import Singleton
from datetime              import datetime,timedelta
import time

'''
防沉迷单例类
self.__antiPercent   ：受益比例

'''
RATE = 1

class AntiAddiction(Singleton.Singleton):
    def __init__(self):
        super(AntiAddiction, self).__init__()
        # self.__antiPercent = 1 # 受益比例
        self.timePointDict = {1:3600,2:7200,3:10800,4:14400,5:18000}
        self.timeKeyList   = [1,2,3,4,5]
        self.secondList    = [(3600*RATE),(7200*RATE),(10800*RATE),(14400*RATE),(18000*RATE)]
        self._hour         = 0
        self.lasttime      = 0 # 持续时间
        pass

    @staticmethod
    def getInstance():
        return Singleton.getInstance(AntiAddiction)

    def getAntiPrecentbyHour(self, hour):
        if hour<3:
            antiPrecent = 1
        elif hour>=3 and hour<5:
            antiPrecent = 0.5
        else:
            antiPrecent = 0
        return antiPrecent

    def getHourBySecond(self, second):
        thour = 1
        if second >=0 and second < int(self.timePointDict.get(1))*RATE:
            thour = 1
        elif second >= int(self.timePointDict.get(2))*RATE and second < int(self.timePointDict.get(3))*RATE:
            thour = 2
        elif second >= int(self.timePointDict.get(3))*RATE and second < int(self.timePointDict.get(4))*RATE:
            thour = 3
        elif second >= int(self.timePointDict.get(4))*RATE and second < int(self.timePointDict.get(5))*RATE:
            thour = 4
        elif second >= int(self.timePointDict.get(5))*RATE:
            thour = 5
        return thour

    def getAntiPrecentbySeconds(self, s):
        if s < (10800*RATE) and s > 0:
            antiPrecent = 1
        elif s>=(10800*RATE) and s < (18000*RATE):
            antiPrecent = 0.5
        else:
            antiPrecent = 0
        return antiPrecent

#coding:utf-8
import cPickle
import Singleton
from protocol                   import *
# from protocol.MatchProtocol     import *

class BaseS2SCaller(Singleton.Singleton):
    def __init__(self):
        self.__Srv = None
        pass

    def _initialize(self, srv):
        self.__Srv = srv
        pass

    @staticmethod
    def getInstance():
        return Singleton.getInstance(BaseS2SCaller)

    def __s2s(self, tag, tp, data):
        return self.__Srv.SetSlaveMsg(tp, tag, data, PacketBlockType.PBT_BLOCK)

    def __s2s_nowait(self, tag, tp, data):
        return self.__Srv.SetSlaveMsg(tp, tag, data, PacketBlockType.PBT_NON_BLOCK)


    def killClient(self, ssid):
        pack = GeneralPack()
        pack.TYPE = PacketType.PT_PEER_KILL
        pack.SID  = ssid.Data
        self.__Srv._slave.SetConnMsg(ssid, pack.data)

    def TestGetMsg(self, ssid, mydata):
        packNotify = GeneralPack()
        packNotify.SID      = ssid.Data
        packNotify.ATTR     = mydata
        packNotify.TYPE     = 20001
        return self.__Srv.GetSlaveMsg(packNotify.TYPE, TagType.TAG_DEMO, packNotify.data)


    def notifyRoleEnter(self, ssid, attr):
        packNotify = GeneralPack()
        packNotify.SID      = ssid.Data
        packNotify.ATTR     = attr
        packNotify.TYPE     = NotifyType.NT_CLIENT_COME
        return self.__Srv.SetSlaveMsg(PacketType.PT_NOTIFY, TagType.TAG_STATUS, packNotify.data, PacketBlockType.PBT_NON_BLOCK)

    #

    def notifyRoleLeave(self, ssid, attr={}):
        packNotify = GeneralPack()
        packNotify.SID      = ssid.Data
        packNotify.TYPE     = NotifyType.NT_CLIENT_LOST
        return self.__Srv.SetSlaveMsg(PacketType.PT_NOTIFY, TagType.TAG_STATUS, packNotify.data, PacketBlockType.PBT_NON_BLOCK)

    #add by sjx
    def notifyRoleUpdate(self, ssid, attr):
        packNotify = GeneralPack()
        packNotify.SID      = ssid.Data
        packNotify.ATTR     = attr
        packNotify.TYPE     = NotifyType.NT_CLIENT_UPDATE
        return self.__Srv.SetSlaveMsg(PacketType.PT_NOTIFY, TagType.TAG_STATUS, packNotify.data, PacketBlockType.PBT_NON_BLOCK)
    #
    # #返回值：是否正常完成比赛, 比赛结果(310), 比赛结果数据包(MatchPack实例)
    # def getMatchResult(self, team_a, team_b, home_and_away = False, extdata=None):
    #     pack = MatchPack()
    #     pack.MATCH_TEAM_A = team_a
    #     pack.MATCH_TEAM_B = team_b
    #     pack.MATCH_RULE_HOMEANDAWAY = home_and_away
    #     pack.MATCH_EXTDATA = extdata
    #     ret, data = self.__Srv.GetSlaveMsg(MatchType.PT_S2S_MATCH, TagType.TAG_MATCH, pack.data)
    #     if ret != MatchError.ERR_OK:
    #         return False, None, None
    #     r = MatchPack(data)
    #     sa,sb = r.Fetch('MATCH_ACTION_TEAM_A_SCORE', 'MATCH_ACTION_TEAM_B_SCORE')
    #     win = 3
    #     if sb > sa:
    #         win = 0
    #     elif sb == sa:
    #         win = 1
    #     return True, win, r

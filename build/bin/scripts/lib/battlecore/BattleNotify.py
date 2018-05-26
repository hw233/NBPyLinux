#coding: utf-8


import Singleton
from protocol import *
from SessionID import SessionID
from base.BaseLogicSlave        import BaseLogicSlave, BaseSlaveConfig

class BattleNotify(Singleton.Singleton):
    def __init__(self):
        self.slave_cfg  = BaseSlaveConfig()
        self.__Srv      = self.slave_cfg.logic_srv.getInstance()
        pass

    @staticmethod
    def getInstance():
        return Singleton.getInstance(BattleNotify)

    def _initialize(self, srv):
       self.__Srv = srv
       pass


    ##扣除/添加 铜钱,元宝,物品,经验,体力
    def s2s_update_role_info(self, role, **param):
        rolePack = RolePack()
        rolePack.ROLE_SSID      = role.SSID
        rolePack.ROLE_ID        = role.UID
        rolePack.ROLE_TOKEN     = role.TOKEN
        rolePack.ROLE_GOLD      = param.get("gold", 0)
        rolePack.ROLE_COIN      = param.get("coin", 0)
        rolePack.ROLE_HONOUR    = param.get("honour", 0)
        rolePack.ROLE_FAME      = param.get("fame", 0)
        rolePack.ROLE_WOOD      = param.get("wood", 0)
        rolePack.ROLE_FOOD      = param.get("food", 0)
        rolePack.ROLE_IRON      = param.get("iron", 0)
        rolePack.ROLE_ADD_ITEM  = param.get("additems", {})
        rolePack.ROLE_DEL_ITEM  = param.get("delitems", {})
        rolePack.ROLE_WORLD_POS = param.get("point",[])
        res = self.__Srv.SetSlaveMsg(RoleType.PT_S2S_UPDATE_ROLE, TagType.TAG_ROLE, rolePack.data, PacketBlockType.PBT_BLOCK)
        return res
        # return (0,0)

    #S2S 添加/扣除 物品
    def s2sUpdateItem(self, role, additem={}, delitem={}):
        bagPack = BagPack()
        bagPack.ROLE_SSID          = role.SSID
        bagPack.ROLE_ID            = role.UID
        bagPack.ROLE_TOKEN         = role.TOKEN
        bagPack.ITEM_ADDITEMS      = additem
        bagPack.ITEM_DELITEMS      = delitem
        res = self.__Srv.SetSlaveMsg(BagType.PT_S2S_BAG_ADD_DEL, TagType.TAG_BAG, bagPack.data, PacketBlockType.PBT_BLOCK)
        return res

    #获取部队信息
    def s2sForceInfo(self, role, fid, uid=0):
        rolePack = RolePack()
        rolePack.ROLE_SSID     = role.SSID
        rolePack.ROLE_ID       = uid
        rolePack.ROLE_TOKEN    = role.TOKEN
        rolePack.HERO_FORCE_ID = fid
        res = self.__Srv.SetSlaveMsg(RoleType.PT_S2S_GET_FORCE_INFO, TagType.TAG_ROLE, rolePack.data, PacketBlockType.PBT_BLOCK)
        return res

    #更新部队信息
    def s2sUpdateHeroInfo(self, role, uid, **param):
        rolePack = RolePack()
        rolePack.ROLE_SSID     = role.SSID
        rolePack.ROLE_ID       = uid
        rolePack.ROLE_TOKEN    = role.TOKEN
        rolePack.HERO_ID       = param.get('hid',0)
        rolePack.HERO_FORCE_ID = param.get('fid',0)
        rolePack.HERO_HP       = param.get('hp',0)
        rolePack.HERO_EXP      = param.get('exp',0)
        rolePack.HERO_STATUS   = param.get('status',-1)
        rolePack.HERO_BATT     = param.get('batt',{})
        rolePack.ROLE_OTHER    = param.get('hurt',{})
        rolePack.TYPE          = RoleType.PT_S2S_UPDATE_HERO
        block                  = param.get('block',0)
        ty                     = PacketBlockType.PBT_BLOCK
        if block == 0:
            ty = PacketBlockType.PBT_NON_BLOCK
        res = self.__Srv.SetSlaveMsg(RoleType.PT_S2S_UPDATE_HERO, TagType.TAG_ROLE, rolePack.data, ty)
        return res

    #通知前端进入战斗
    def s2sBattleReportInfo(self, role, info, uid=0, tp=0, isshow=0):
        pack = WarPack()
        pack.ROLE_SSID      = role.SSID
        pack.ROLE_ID        = uid
        pack.ROLE_TOKEN     = role.TOKEN
        pack.WAR_REPORT_TYPE= tp
        pack.WAR_REPORT     = info
        pack.WAR_ISSHOW     = isshow
        res = self.__Srv.SetSlaveMsg(WarType.PT_S2S_BATTLE_REPORT, TagType.TAG_WAR, pack.data, PacketBlockType.PBT_BLOCK)
        return res

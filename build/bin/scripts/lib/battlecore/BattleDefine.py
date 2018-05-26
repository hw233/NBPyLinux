#coding: utf-8

def enum(*sequential, **named):
    enums = dict(zip(sequential, range(len(sequential))), **named)
    return type('Enum', (), enums)

HERO_TYPE = enum(HERO_TYPE_INF=1, HERO_TYPE_ARC=2, HERO_TYPE_CAV=3, HERO_TYPE_CEL=4)

WORLD_RES = enum("RES_EMPTY", "RES_MILL", "RES_FARM", "RES_IRON", "RES_BANK", "RES_MONSTER", "RES_PLAYER")

WORLD_RES_NAME = {
    0: "空地",
    1: "伐木场",
    2: "农田",
    3: "矿场",
    4: "钱庄"
}

MONSTRT_NAME = {0:"野狼",1:"噬人巨熊",2:"吃人猛虎",3:"山贼",4:"黄巾贼寇",5:"割据军阀",6:"禁卫军",7:"天马",8:"仙人",9:"白玉麒麟"}


WORLD_EVENT_TYPE = enum("MOVE", "GATHER", "STATION", "ATTACK", "TRANSPORT", "DETECT", "TRADE", "CAMPAGIN")

WORLD_EVENT_CALLBACK = enum("ATTACK", "ATTACK_BACK", "GATHER", "GATHER_ING", "GATHER_BACK",\
    "STATION", "STATION_ING", "STATION_BACK", "TRADE", "TRADE_ING", "TRADE_BACK", \
    "TRANSPORT","TRANSPORT_BACK", "DETECT","DETECT_BACK")






# 1	无人野外空地	迁城	驻扎	标记/取消标记
# 2	盟友驻扎野外空地	信息	驻扎	标记/取消标记
# 3	非盟友驻扎野外空地	信息	侦查	攻击	标记/取消标记
# 4	自己驻扎的野外空地	返回	驻扎	标记/取消标记
# 5	怪物	怪物信息	进攻	标记/取消标记
# 6	未占领资源点	信息	采集	驻扎	标记/取消标记
# 7	自己占领资源点	信息	返回	驻扎	标记/取消标记
# 8	盟友占领资源点	信息	驻扎	标记/取消标记
# 9	非盟友占领资源点	侦查	信息	攻击	标记/取消标记
# 10	自己主城	信息	进城	标记/取消标记
# 11	盟友城	信息	驻扎	运输	通商	标记/取消标记
# 12	其他城	信息	侦查	攻击	通商	运输	标记/取消标记
# 13	通商中的其他城池	信息	侦查	攻击	运输	返回	标记/取消标记
# 14	自己驻扎和盟友驻扎的野外空地	返回	驻扎	标记/取消标记
# 15	盟友占领，自己驻扎的资源点	信息	驻扎	返回	标记/取消标记
# 16	驻扎的盟友城/通商的盟友城	信息	驻扎	运输	通商	返回	标记/取消标记
# 17	自己出发的部队	信息	返回	加速						弹出信息,返回,加速,
# 18	自己返回的部队	信息	加速							弹出信息,加速
# 19	自己侦查出发的侦查部队	信息	返回	加速						弹出信息,返回,加速,
# 20	自己侦查返回	信息	加速							弹出信息,加速
# 21	自己运输出发部队	信息	返回	加速						弹出信息,返回,加速,
# 22	自己运输返回部队	信息	加速							弹出信息,加速
# 23	自己通商出发部队	信息	返回	加速						弹出信息,返回,加速,
# 24	自己通商返回部队	信息	加速							弹出信息,加速
# 25	盟友出发的部队	信息
# 26	盟友返回的部队	信息
# 27	盟友侦查出发的侦查部队	信息
# 28	盟友侦查返回	信息
# 29	盟友运输出发部队	信息
# 30	盟友运输返回部队	信息
# 31	盟友通商出发部队	信息
# 32	盟友通商返回部队	信息
# 33	非盟友出发的部队	信息
# 34	非盟友返回的部队	信息
# 35	非盟友侦查出发的侦查部队	信息
# 36	非盟友侦查返回	信息
# 37	非盟友运输出发部队	信息
# 38	非盟友运输返回部队	信息
# 39	非盟友通商出发部队	信息
# 40	非盟友通商返回部队	信息
# 41	驻扎中自己的部队	信息	返回
#
#
#
#
#
#
#
#
#
#
# 'BeNice', 'CBTP_ON_CLOSE', 'CBTP_ON_CONNECT',
#  'CBTP_ON_PACKET', 'CreateTimer', 'DestroyTimer',
#
# 'GetVersion', 'TIMER_TP_LOOP', 'TIMER_TP_ONCE', '__doc__', '__name__']

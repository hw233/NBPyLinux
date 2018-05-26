

if __name__ == "__main__":

    import sys
    sys.path.append("../../lib")
    sys.path.append("../../lib/stub")
    from XmlReader import XmlReader
    #XmlReader.PATH = "../../../../../xml/xml/"
    from BattleNotify import gBattleNotify
    from Slave import Slave
    slave=  Slave()
    gBattleNotify.init(slave)
    #from Battle import battle_test
    #battle_test()
    from PVEBattle import pve_test
    pve = pve_test()
    from PVPBattle import pvp_test
    pvp_test()
    from BossBattle import BossBattle
    import timeit
    from TeamBattle import TeamBattle,TeamBattle_TestCase
    a = timeit.Timer(stmt="TeamBattle_TestCase();test_5v5team_battle_testcase()",setup="from TeamBattle import TeamBattle,TeamBattle_TestCase,test_5v5team_battle_testcase")
    print a.timeit(1 )
    
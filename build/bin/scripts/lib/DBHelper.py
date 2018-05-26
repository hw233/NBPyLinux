#!/usr/local/bin/python2.5
import stackless
import Singleton
import Config
import os

DEBUG = False

#if os.environ.get('OS','').lower().find('windows') < 0 and (not DEBUG):
from DBBaseEx import DBBase
#else:
#from DBBase import DBBase

MAX_DB_CONN = 2

class DBHelper(Singleton.Singleton):
    def __init__(self,nconn=MAX_DB_CONN,CHAR_SET='utf8'):
        self._tasklet   = None
        self._atomic    = None
        self._maxconn   = nconn
        self._charset   = CHAR_SET
        #
        self._conn_pool = []
        pass

    def __enter(self):
        self.__tasklet = stackless.getcurrent()
        self.__atomic = self.__tasklet.set_atomic(True)
    def __exit(self):
        self.__tasklet.set_atomic( self.__atomic )


    def _initialize(self):
        cfg = Config.Config('./config/db.cfg')
        cfg.enter('db')
        dbhost,dbuser,dbpwd,dbname = (cfg.host,cfg.user,cfg.PASSWORD,cfg.DATABASE)
        unix_sock = cfg.unix_sock
        params = {
            'host':         dbhost,
            'unix_socket':  unix_sock,
            'user':         dbuser,
            'passwd':       dbpwd,
            'db':           dbname,
            'charset':      self._charset,
            'use_unicode':  False,
            'compress':     False
            }
        if unix_sock:   params.pop('host',None)
        else:           params.pop('unix_socket',None)
        for i in xrange(0,self._maxconn):
            db = DBBase()
            try:
                db.Connect(**params)
                self._conn_pool.append(db)
            except Exception,ex:    print "Exception occured when trying to connect to DataBase!EX:%s" % ex
        print '[DBHelper] connection count: %d'%(len(self._conn_pool))

    @staticmethod
    def getInstance():
        return Singleton.getInstance(DBHelper)
    #
    def GetDefaultDB(self):
        self.__enter()
        if len(self._conn_pool) < 1:
            self._initialize()
        self.__exit()

        if stackless.getcurrent() == stackless.getmain():
            return self._conn_pool[0]
        idx = 2; ret = 1
        #while idx < len( self._conn_pool ):
        #    if self._conn_pool[idx].conn.waiting() < self._conn_pool[ret].conn.waiting():
        #        ret = idx
        #    idx += 1
        db = self._conn_pool.pop(ret)
        self._conn_pool.append(db)
        return db


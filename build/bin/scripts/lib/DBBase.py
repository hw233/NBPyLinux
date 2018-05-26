#-*- coding: utf-8 -*-
import MySQLdb
import kernel.timer
import stackless
from warnings import warn
from _mysql_exceptions import OperationalError

class DBBase(object):
    def __init__(self):
        super(DBBase,self).__init__()
        self.conn = None
        self._cursor = None
        self._trans = None
        self._args = None
        self._kwargs = None
        #
        self._timerListener = kernel.timer.CreateTimer(10000, self.OnTimer, kernel.timer.TIMER_TP_LOOP, None)
        
    def __del__(self):
        if self.conn != None:
            if self._trans: self.RollBack()
            self.conn.close()    
        self.conn,self._trans,self._cursor = None,None,None
        kernel.timer.DestoryTimer(self._timerListener)

    def __enter(self):
        self.__tasklet = stackless.getcurrent()
        self.__atomic = self.__tasklet.set_atomic(True)
    def __exit(self):   self.__tasklet.set_atomic( self.__atomic )
        
      
    def OnTimer(self,args):
        if self.conn is None:
            try:    self.conn = MySQLdb.connect(*self._args,**self._kwargs)
            except Exception,ex:    print ex
        if self.conn is None:   return
        
        try:    self.conn.ping(True)
        except OperationalError,oe: print oe;self.conn = None
        #Try to re-connect.
        if self.conn is None:
            try:    self.conn = MySQLdb.connect(*self._args,**self._kwargs)
            except Exception,ex:    print ex

    def Connect(self, *args, **kwargs):
        self.conn = MySQLdb.connect(*args, **kwargs)
        self._trans = False
        #After mysql-python 1.12, * Turn autocommit off initially
        #But here we enable autocommit again.
        self.conn.autocommit(True)
        self._args,self._kwargs = args,kwargs
        return self.conn

    #MySQL python Version 1.1.2
    # * Remove Connection.begin(); use SQL BEGIN or START TRANSACTION instead self.conn.begin()
    def Begin(self):
        #If already start transaction, donn't restart it. maybe commit the last trans
        if self._trans:warn("A transaction is being ocuupied");return
        self._cursor = self.conn.cursor()
        self._cursor.execute("BEGIN")
        self._trans = True

    def RollBack(self):
        if not self._trans:     return
        self.conn.rollback()
        del self._cursor
        self._trans = False
        
    def Commit(self):
        if not self._trans: return
        self.conn.commit()
        del self._cursor
        self._trans = False

    def Insert(self, sql, para ):
        self.__enter()
        _cursor = self.conn.cursor()
        ret = _cursor.execute(sql, para)
        ret = _cursor.lastrowid
        _cursor.close()
        self.__exit()
        return ret 
        
    #if ReturnDict==True, return a dictonary instead of tuple
    def Query(self, sql, para, ReturnDict=False):
        _cursor = None
        self.__enter()
        if ReturnDict:  _cursor = self.conn.cursor( cursorclass = MySQLdb.cursors.DictCursor )
        else:           _cursor = self.conn.cursor()
        assert(_cursor)
        num = _cursor.execute(sql,para)
        if num < 1: self.__exit();return None
        ret = _cursor.fetchall()
        del _cursor
        self.__exit()
        return ret         
    #para is tuple or None
    def Modify(self, sql, para):
        self.__enter()
        _cursor = self.conn.cursor()
        _cursor.execute(sql, para)
        ret = _cursor.rowcount
        del _cursor
        self.__exit()
        return ret
            
    #para is list of tuples or None
    def ModifyMany(self, sql, para):
        self.__enter()
        _cursor = self.conn.cursor()
        ret = _cursor.executemany(sql, para)
        del _cursor
        self.__exit()
        return ret
            
    def escape_string(self,string):     return MySQLdb.escape_string(string)

class Transaction(object):
    def __init__(self, db):
        super(Transaction,self).__init__(self)
        self.db = db
        self.db.Begin()

    def Commit(self):   self.db.Commit()

    def RollBack(self): self.db.RollBack()

    def __del__(self):
        if self.db._trans:
            self.db.RollBack()


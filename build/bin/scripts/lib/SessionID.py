#coding: utf-8

#import struct

from FakeProperty   import FakeProperty

class SessionID(object):
    def __init__(self, _sid = None):
        self.__cid  = 0
        self.__sid  = 0
        
        self.Data   = _sid
        
    @FakeProperty
    def CID(self):
        return self.__cid
        
    @FakeProperty.set(CID)
    def CID(self, v):
        self.__cid = v
    
    @FakeProperty
    def SID(self):
        return self.__sid
        
    @FakeProperty.set(SID)
    def SID(self, v):
        self.__sid = v
    
    @FakeProperty
    def Data(self):
        #return struct.pack("ii", self.__cid, self.__sid)
        return self.__cid, self.__sid
    
    @FakeProperty.set(Data)
    def Data(self, v):
        #if v is not None:
        #    self.__cid, self.__sid = struct.unpack("ii", v)
        if v:
            self.__cid, self.__sid = v
            
    def __repr__(self):
        return '%d|%d'%(self.__cid, self.__sid)
    
    def __str__(self):
        return '%d|%d'%(self.__cid, self.__sid)
    

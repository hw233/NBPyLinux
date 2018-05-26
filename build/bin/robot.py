#coding:utf-8

import sys
import os

import kernel

import traceback

sys.path.append(os.getcwd() + "/script/lib")

from protocol   import *

def OnPacketIn(p):
    pack = GeneralPack(p)
    schema, tp = pack.Fetch('SCHEMA','TYPE')
    print schema, tp
    pass

def __on_read(s, packet):
    OnPacketIn(packet)
    pass

def __on_connect(s, addr, port):
    print 'client %d [%s:%d] connected' %(s, addr, port)
    pass

def __on_close(s):
    print 'client %d disconnected' %(s)
    pass

def __on_terminate():
    pass
    
kernel.SetCallback(kernel.CBTP_ON_PACKET, __on_read)
kernel.SetCallback(kernel.CBTP_ON_CONNECT, __on_connect)
kernel.SetCallback(kernel.CBTP_ON_CLOSE, __on_close)

kernel.Connect("127.0.0.1", 8777)


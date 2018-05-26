#coding:utf-8

import os

import kernel
import kernel.processor

import stackless
import traceback

from protocol   import *
from SessionID  import SessionID

#---------------------------------
_schemaHandler_ = {}
_c2sHandler_    = {}
_s2sHandler_    = {}
_r2sHandler_    = {}
#---------------------------------


'''
    Decorator for schema handling.
'''
def SchemaHandler(schema):
    def _Handler(fn):
        if _schemaHandler_.get(schema):    print "[WARN] Schema handler for [%s] will be overwrote" % schema
        _schemaHandler_[schema] = fn
        return fn
    return _Handler

'''
    Decorator for C2S messages handling.
    @tag:   The tag of the logic system
    @tp:    The request type from client to logic system.

    A handler of C2S should return:
    @err:   Error code
    @data:  The message to send back to client.
'''
def C2S(tag,tp):
    def _Handler(fn):
        if _c2sHandler_.get(tp):   print "[WARN] c2s handler for [%s] will be overwrote" % str((tag,tp))
        _c2sHandler_[(tag,tp)] = fn
        return fn
    return _Handler
'''
    Decorator for S2S messages handling.
    @tp:    The request type.

    A handler of S2S should return:
    @tag: the tag of the response
    @err: error code
    @data:the message to send back.
'''
def S2S(tp):
    def _Handler(fn):
        if _s2sHandler_.get(tp):   print "[WARN] s2s handler for [%s] will be overwrote" % tp
        _s2sHandler_[tp] = fn
        return fn
    return _Handler
'''
    Decorator for R2S messages handling.

    Note:This is used to deal with server status issues,so , no return value wanted.
'''
def R2S(tp):
    def _Handler(fn):
        if _r2sHandler_.get(tp):   print "[WARN] r2s handler for [%s] will be overwrote" % tp
        _r2sHandler_[tp] = fn
        return fn
    return _Handler

def unmask_s2s_params(tp):
    return tp & 0x00ffffff,(tp & 0xfe000000)>>24,(tp & 0x01000000)>>24

def unmask_s2s_tp(tp):
    return tp & 0x00ffffff

class Slave(object):
    def __init__(self, *tags):
        self.__srv = kernel.processor.current()

        kernel.SetCallback(kernel.CBTP_ON_PACKET, self.__on_ps)
        kernel.SetCallback(kernel.CBTP_ON_CONNECT, self.__on_connect)
        kernel.SetCallback(kernel.CBTP_ON_CLOSE, self.__on_close)
        kernel.SetCallback(kernel.CBTP_ON_TERMINATE, self.__on_terminate)

        self.__tags         = tags

        self.__srv.SetTags(self.__tags)
        #===================================================================
        self.mBusyChannels  = {}
        self.__chid_seed    = 1
        self.__taskletch    = None
        self.__atomicch     = None
        #===================================================================

        self.PackClass      = GeneralPack

        self.__slaves       = {}

        ###keep Main tasklet###
        def __main_func():
            while True:
                kernel.BeNice()
        stackless.tasklet(__main_func)().run()
        pass

    def __enterch(self):
        self.__taskletch = stackless.getcurrent()
        self.__atomicch = self.__taskletch.set_atomic(True)

    def __exitch(self):
        self.__taskletch.set_atomic( self.__atomicch )

    def __getChannel(self,chid):
        self.__enterch()
        ch = self.mBusyChannels.get(chid,None)
        self.__exitch()
        return ch

    def __newChannel(self):
        self.__enterch()
        ch,chid = stackless.channel(),self.__chid_seed
        #
        self.__chid_seed = ( (self.__chid_seed + 1)& 0xFFFFFFFF )
        #
        self.mBusyChannels[chid] = ch
        self.__exitch()
        return chid,ch

    def __freeChannel(self, chid):
        self.__enterch()
        try:    del self.mBusyChannels[chid]
        except KeyError,ke: print "Invalid channle-id %s"%chid; pass
        self.__exitch()
        
    def __on_ps(self, *ps):
        for p in ps:
            stackless.tasklet(self.__on_read)(0, p)
            
        del ps

    def __on_read(self, s, packet):
        print '__on_read'
        sid = packet.get(PacketKey.PK_SID)
        if not sid:
            cid = packet.get(PacketKey.PK_CID)
            pid = packet.get(PacketKey.PK_PID)
            sid = (pid, cid)

        packet[PacketKey.PK_SID] = sid
        ssid = SessionID(sid)
        self.OnPacketIn(ssid, packet)
        pass

    def __on_connect(self, s, sid, addr, port):
        pass

    def __on_close(self, s, sid):
        pass

    def __on_terminate(self):
        pack = GeneralPack()
        pack.Fill( SCHEMA=PacketSchema.PS_S2S,TYPE=PacketType.PT_SLAVE_EXIT, PID=int(os.getpid()), TAGS=self.__tags )
        
        for tag, pids in self.__slaves.items():
            for pid, tag in pids.items():
                self.__srv.Send(pid, pack)
        pass

    def OnPacketIn(self, ssid, packet):
        #if packet.get(PacketKey.PK_SCHEMA, 0) == PacketSchema.PS_C2S:
        #    packet[PacketKey.PK_SCHEMA] = PacketSchema.PS_S2C
        #    self.__srv.Send(packet, tag=TagType.TAG_CONN, pid=ssid.CID)

        pack = self.PackClass(packet)
        print '>>> MessageIn [%s]' % str(pack)
        schema, tp = pack.Fetch('SCHEMA','TYPE')
        if schema is None:  schema = PacketSchema.PS_C2S
        fnSchema = _schemaHandler_.get(schema)
        if not fnSchema:
            print " !!!! Unknown schema:%s" % schema
            return
        fnSchema(self,tp,pack)
        pass

    def Send(self, packet, ssid = None, tag = TagType.TAG_CONN):
        pid = 0
        if ssid:
            packet[PacketKey.PK_SID] = ssid.Data
            pid = ssid.CID
            if pid is None:
                pid = os.getpid()

        fails = []

        if pid > 0:
            if not self.__srv.Send(pid, packet):
                fails.append(pid)
        elif tag > 0:
            pids = self.__slaves.get(tag, {})
            for pid, tag in pids.items():
                if not self.__srv.Send(pid, packet):
                    fails.append((pid))

        if len(fails) == 0:
            return True


        wrappack = self.PackClass({})
        wrappack[PacketKey.PK_SCHEMA]   = PacketSchema.PS_S2R
        wrappack[PacketKey.PK_TYPE]     = PacketType.PT_DELIVER
        wrappack[PacketKey.PK_TO]       = fails
        wrappack[PacketKey.PK_BODY]     = packet

        self.__srv.Deliver(wrappack.data)

        return True
        pass

    def SendBySID(self, packet, sid):
        return self.Send(packet, SessionID(sid))
        pass

    def RetSend(self, csid, realPack):
        _sid, _cid = csid.SID, csid.CID
        pack = GeneralPack()
        pack.Fill( TO=(_sid,),SCHEMA=PacketSchema.PS_S2C,BODY=realPack.data )
        if self.Send(pack.data, csid):   del pack;   return
        #
        print ">>>> send to %s failed" % csid
        pass

    def Inform(self, pids, req=True):
        pack = GeneralPack()
        pack.Fill( SCHEMA=PacketSchema.PS_S2S,TYPE=PacketType.PT_SLAVE_INFORM, PID=int(os.getpid()), TAGS=self.__tags, REQUEST=req )
        for pid in pids:
            self.__srv.Send(pid, pack.data)

    @R2S(PacketType.PT_SLAVE_REG)
    def OnSlaveRegisterSuscess(self, msg):
        print '节点注册成功'
        pids = []
        try:
            f = open(self.__srv.GetPIDFilePath(), 'r')
            if f:
                for line in f:
                    pid = int(line)
                    if pid != os.getpid():
                        pids.append(pid)
                f.close()
        except Exception,ex:
            print '获取其他节点信息失败', ex
            return

        print '获取到其他 %d 个节点信息，发起通告' %(len(pids))
        #self.__srv.Inform(pids)
        self.Inform(pids)
        pass

    @R2S(PacketType.PT_DELIVER)
    def OnDeliver(self, msg):
        print 'Has new Deliver'

        body = msg.Fetch( 'BODY' )
        if not body:
            return

        realPack = self.PackClass(body)
        self.__on_read(0, realPack)
        pass

    @S2S(PacketType.PT_SLAVE_INFORM)
    def OnSlaveInform(self, msg):
        tags, pid, need = msg.Fetch( 'TAGS','PID','REQUEST' )
        print '有新的节点通告 FROM %d For Tag %s' %(pid, tags)

        for tag in tags:
            pids = self.__slaves.get(tag, {})
            pids[pid] = tag
            self.__slaves[tag] = pids
            
        if need:
            self.Inform([pid,], False)

        return None, ErrorCode.ERR_NORETURN, None
        pass

    @S2S(PacketType.PT_SLAVE_EXIT)
    def OnSlaveExit(self, msg):
        print '有节点退出'

        tags, pid = msg.Fetch( 'TAGS','PID' )

        for tag in tags:
            pids = self.__slaves.get(tag, {})
            pids.pop(pid, None)
            self.__slaves[tag] = pids

        return None, ErrorCode.ERR_NORETURN, None
        pass

    '''
        dispatch c2s message handling
    '''
    @SchemaHandler(PacketSchema.PS_C2S)
    def HandleC2S(self,tp,msg):
        if not tp:  return
        tag,reqid,sid = msg.Fetch( 'TAG','REQID','SID' )
        realPack,csid = GeneralPack(),SessionID( sid )
        #print 'type: %d tag: %d, sid: %s'%(tp, tag, str(csid))
        #
        fn = _c2sHandler_.get( (tag,tp) )
        if not fn:
            realPack.Fill( ERROR=ErrorCode.ERR_NO_SERVICE,TYPE=PacketType.PT_RESULT,REQUEST=tp,TAG=tag,REQID=reqid )
            self.RetSend( csid,realPack )
            print '[404] for type: %d tag: %d'%(tp, tag)
            return
        #Call the function
        err,data = ErrorCode.ERR_INTERNAL_ERR,None
        try:    err,data = fn(self,msg)
        except: "<%s>" % fn.__name__,traceback.print_exc()
        #
        if err == ErrorCode.ERR_NORETURN:   return
        realPack.Fill( ERROR=err,BODY=data,TYPE=PacketType.PT_RESULT,REQUEST=tp,TAG=tag,REQID=reqid )
        self.RetSend( csid,realPack )
        del realPack,msg
        pass
    '''
        dispatch s2s message handling
    '''
    @SchemaHandler(PacketSchema.PS_S2S)
    def HandleS2S(self,tp,msg):
        #print 'S2S >>>', tp, msg, _s2sHandler_
        if not tp:  return
        cmd,op,mode =  unmask_s2s_params(tp)
        FN = _s2sHandler_.get(cmd,None)
        reqid,_from_       = msg.Fetch('CHID','FROM')
        tag,err,data = None,ErrorCode.ERR_NO_SERVICE,None
        #Call the function
        if FN:
            try:    tag,err,data = FN(self,msg)
            except:
                traceback.print_exc();
                err = ErrorCode.ERR_INTERNAL_ERR
        #
        if err == ErrorCode.ERR_NORETURN:   return
        if mode == PacketBlockType.PBT_BLOCK:
            pack    = GeneralPack()
            pack.Fill( FROM_TAG=tag,TO=_from_,SCHEMA=PacketSchema.PS_S2S,ERROR=err,BODY=data,REQUEST=cmd,CHID=reqid)
            if   op == PacketType.PT_GET:   pack.TYPE = PacketType.PT_GET_RES
            elif op == PacketType.PT_SET:   pack.TYPE = PacketType.PT_SET_RES

            '''modify for ipc-s2s.first use [IPCSendToModule(sid,tag,pack)] to return the s2s-result,if fail, then return by router
               raw-message [FROM] cannot be None'''
            sid = int( _from_ )

            if not self.__srv.Send(sid, pack):
                wrappack = self.PackClass({})
                wrappack[PacketKey.PK_SCHEMA]   = PacketSchema.PS_S2R
                wrappack[PacketKey.PK_TYPE]     = PacketType.PT_DELIVER
                wrappack[PacketKey.PK_TO]       = [sid,]
                wrappack[PacketKey.PK_BODY]     = pack

                self.__srv.Deliver(wrappack.data)

            del pack,msg
        pass
    '''
        dispatch r2s message handling
    '''

    @SchemaHandler(PacketSchema.PS_R2S)
    def HandleR2S(self,tp,msg):
        if not tp:  return
        FN = _r2sHandler_.get(tp)
        if FN is None:   print "Unhandled r2s msessage:%s for type:%s"%(msg,tp);return
        FN(self,msg)
        pass

    '''
        GET result handle
    '''
    @S2S(PacketType.PT_GET_RES)
    def OnGetResultHandle(self, msg):
        chid,body,err = msg.Fetch('CHID','BODY','ERROR')
        ch = self.__getChannel(chid)
        if ch is None:  print " > Warning:Invalid channel...:%s msg:%s" % (chid,msg)
        else:
            ch.send( (err,body) )
            self.__freeChannel( chid )
        return None, ErrorCode.ERR_NORETURN,None

    '''
        SET result handle
    '''
    @S2S(PacketType.PT_SET_RES)
    def OnSetResultHandle(self, msg):
        chid,body,err = msg.Fetch('CHID','BODY','ERROR')
        ch = self.__getChannel( chid )
        if ch is None:  print " > Warning:Invalid channel...:%s msg:%s" % (chid,msg)
        #Send to channel
        else:
            ch.send( (err,body) )
            self.__freeChannel( chid )
        return None, ErrorCode.ERR_NORETURN,None

    '''
        Multicast
    @msg:   The message to client(dict)
    @to:    The session-id(s) of the target clients.
    '''
    def Multicast(self,msg,to):
        if len(to) < 1: return
        tmp,p = {},GeneralPack()
        for s in to:
            k = (s.RID,s.CID)
            lst = tmp.get( k,None )
            if lst is None:   lst=set(); tmp[k] = lst
            lst.add(s.SID)
        #
        p.Fill( SCHEMA=PacketSchema.PS_S2C,TT=PacketTarget.PTT_MULTICAST,BODY=msg )
        for s,l in tmp.iteritems():
            p.Fill( TO=list(l),RID=s[0],CID=s[1] )
            self.Send(p.data)
    '''
        BroadCast
    @tag : TAG
    @tp  : TYPE
    @body: BODY
    '''
    def Broadcast(self, tag, tp, body,bReturn=False):
        pack = GeneralPack()
        pack.Fill( TAG=tag,SCHEMA=PacketSchema.PS_S2C,TYPE=tp,TT=PacketTarget.PTT_BROADCAST,BODY=body)
        if bReturn:     return pack.data
        self.Send(pack.data)

    ''' Send command to conn-server. (2012-09-21)
        @ssid:  The session-id of the client
        @data:  The message data for conn-server.
    '''
    def SetConnMsg(self,ssid,data):
        _sid,_cid = ssid.SID,ssid.CID
        pack = GeneralPack()
        pack.Fill( TO=(_sid,),TYPE=PacketType.PT_SET,SCHEMA=PacketSchema.PS_S2S,BODY=data )
        #
        self.Send(pack.data, ssid)

    '''
        set message to other slave
        #Changelog:
         2011-10-18 <Royce>: Now this will return (error,data) [tuple] to the caller.
    '''
    def SetSlaveMsg(self, cmd=None, tag=None, body=None, mode = PacketBlockType.PBT_NON_BLOCK):
        msg                    = GeneralPack()
        if mode != PacketBlockType.PBT_NON_BLOCK:
            chid, ch          = self.__newChannel()
            '''modify for ipc-s2s.add [FROM],use [IPCSendToModule(tag,pack)] send the s2s-message,if fail, then send by router'''
            msg.Fill( FROM=os.getpid(),TAG=tag,SCHEMA=PacketSchema.PS_S2S,BODY=body,TYPE=( cmd | ((PacketType.PT_SET | mode)<<24) ),CHID=chid )
            self.Send(msg.data, tag=tag)
            return ch.receive()
        else:
            msg.Fill( TAG=tag,SCHEMA=PacketSchema.PS_S2S,BODY=body,TYPE=( cmd | ((PacketType.PT_SET | mode)<<24) ),CHID=0 )
            self.Send(msg.data, tag=tag)
            print 'send'
            return None,None
    '''
        get message from other slave
        #Changelog:
         2011-10-18 <Royce>: Now this will return (error,data) [tuple] to the caller.
    '''
    def GetSlaveMsg(self, cmd=None, tag=None, body=None):
        msg = GeneralPack()
        chid, channel = self.__newChannel()
        '''modify for ipc-s2s.add [FROM],use [IPCSendToModule(sid,tag,pack)] send the s2s-message,if fail, then send by router'''
        msg.Fill( FROM=os.getpid(),TAG=tag,SCHEMA=PacketSchema.PS_S2S,BODY=body,TYPE=( cmd | ((PacketType.PT_GET | PacketBlockType.PBT_BLOCK)<<24) ),CHID=chid )
        self.Send(msg.data, tag=tag)
        err,data = channel.receive()
        return err,data

    @S2S(PacketType.PT_NOTIFY)
    def OnS2SNotify(self,pack):
        #print '>>> system notify'
        tp  = pack.BODY[PacketKey.PK_TYPE]
        sid = SessionID(pack.BODY[PacketKey.PK_SID])
        data = pack.BODY.get(PacketKey.PK_ATTR)
        if tp == NotifyType.NT_CLIENT_LOST:
            self.logout(sid)
        elif tp == NotifyType.NT_CLIENT_COME:
            self.login(sid, data)
        elif tp == NotifyType.NT_CLIENT_UPDATE:
            self.update(sid, data)
        return None, ErrorCode.ERR_NORETURN, None

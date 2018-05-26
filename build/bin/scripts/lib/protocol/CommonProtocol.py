#coding: utf-8
import sys
sys.path.append('../')
sys.path.append('../lib/')
from FakeProperty import FakeProperty

class __ServerType(object):

    ST_ROUTER   = 1
    ST_LOGIC    = 2
    ST_CONN     = 3

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

ServerType = __ServerType()

class __PacketSchema(object):

    PS_C2S 	= 1
    PS_S2C 	= 2
    PS_S2S 	= 3
    PS_S2R 	= 4
    PS_R2S 	= 5

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

PacketSchema = __PacketSchema()

class __PacketType(object):

    PT_SLAVE_REG    = 0x10
    PT_SLAVE_DEL    = 0x11
    PT_PEER_KILL    = 0x12
    PT_SLAVE_INFORM = 0x13
    PT_SLAVE_EXIT   = 0x14
    PT_DELIVER      = 0x15

    PT_GET          = 0x20
    PT_GET_RES      = 0x21
    PT_SET          = 0x22
    PT_SET_RES      = 0x23

    PT_NOTIFY       = 0xFF

    PT_TEST         = 0xFFF0

    PT_RESULT       = 0xFFFF#65535 result

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

PacketType = __PacketType()


class __PacketKey(object):

    PK_ERROR    = 0x0A
    PK_SCHEMA	= 0x0B
    PK_SID 		= 0x0C
    PK_SOCKET	= 0x0D
    PK_PID 		= 0x0E
    PK_FD 		= 0x0F
    PK_TYPE 	= 0x10
    PK_TO		= 0x11
    PK_FROM 	= 0x12
    PK_BODY 	= 0x14
    PK_TAGS 	= 0x15
    PK_TAG  	= 0x16
    PK_SRV_TYPE = 0x17
    PK_IPC 		= 0x18
    PK_IPCADDR  = 0x19
    PK_REQUEST  = 0x20
    PK_CHID     = 0x21
    PK_FROM_TAG = 0x22
    PK_REQID    = 0x23
    PK_RID      = 0x24
    PK_CID      = 0x25
    PK_TT       = 0x26
    PK_ROLEID   = 0x27
    PK_TO_PID   = 0x28
    PK_DEVICE   = 0x29
    PK_ATTR     = 0x30
    PK_SHOP     = 0x31

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

PacketKey = __PacketKey()

class __DeviceMode(object):

    DM_UNKNOWN      = 0x0
    DM_IOS          = 0x1
    DM_ANDROID      = 0x2

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

DeviceMode = __DeviceMode()

class __TagType(object):
    TAG_NONE            = 0
    TAG_ROUTER          = 0x01
    TAG_CONN            = 0x02
    TAG_STATUS          = 0x03
    TAG_DEMO            = 0x04
    TAG_ECHO            = 0x05
    TAG_ROLE            = 0x06
    TAG_BATTLE          = 0x07
    TAG_TASK            = 0x08
    TAG_BAG             = 0x09
    TAG_PUBS            = 0x10
    TAG_CHAT            = 0x11
    TAG_HTTP            = 0x12
    TAG_FRIEND          = 0x13
    TAG_MAIL            = 0x14
    TAG_ALIANCE         = 0x15
    TAG_CAMPAGIN        = 0x16
    TAG_WAR             = 0x17
    TAG_LOG             = 0x18
    TAG_TREND           = 0x19
    TAG_RANK            = 0x20
    TAG_SHOP            = 0x21
    TAG_ANTI            = 0x22
    TAG_LOGIN           = 0x23
    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

TagType = __TagType()

class __PacketTargetType(object):
    PTT_NORMAL      = 0x0
    PTT_MULTICAST   = 0x1
    PTT_BROADCAST   = 0x2

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

PacketTarget = __PacketTargetType()

class __ErrorCode(object):
    ERR_OK              = 0     #
    ERR_FAIL            = -1    #
    ERR_NO_SERVICE      = 404   #
    ERR_INTERNAL_ERR    = 500   #
    ERR_NORETURN        = 65535 #

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

ErrorCode = __ErrorCode()

class __PacketBlockType(object):
    PBT_BLOCK       = 0x01
    PBT_NON_BLOCK   = 0x00

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

PacketBlockType = __PacketBlockType()

class __TableViewTag(object):
    TAG_FRIEND_TABLE_VIEW    = 0x900
    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

TableViewTag= __TableViewTag()


class __notifyType(object):
    NT_CLIENT_LOST     =   0x10 #(16)Connection lost
    NT_CLIENT_COME     =   0x11 #(17)Connection established
    NT_CONN_SVR_LOST   =   0x20 #(32)Connection server left the server-group
    NT_CONN_SVR_COME   =   0x21 #(33)Connection server joined the server-group
    NT_LOGIC_SVR_LOST  =   0x30 #(48)A logic server left the server-group
    NT_LOGCI_SVR_COME  =   0x31 #(49)A logic server joined the server-group
    NT_ROUTER_SVR_COME =   0x40 #(64)A router server joined the server-group
    NT_ROUTER_SVR_LOST =   0x41 #(65)A router server left the server-group
    NT_CLIENT_UPDATE   =   0x42 #Connection update
    #write to attributes is denied.
    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

NotifyType = __notifyType()


class GeneralPack(dict):
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = PacketKey
    def __init__(self, raw=None):
        super(GeneralPack, self).__init__()
        if raw:
            self.update(raw)

    def Fill(self, **kws):
        ks = self.__class__.KEYS
        for k,v in kws.items():
            key = '%s%s'%(self.__class__.PREFIX_PACKET_KEY, k)
            if hasattr(ks, key):
                self.__setitem__(getattr(ks, key), v)
            elif hasattr(ks, k):
                self.__setitem__(getattr(ks, k), v)
            elif hasattr(PacketKey, key):
                self.__setitem__(getattr(PacketKey, key), v)
            elif hasattr(PacketKey, k):
                self.__setitem__(getattr(PacketKey, k), v)

    def Fetch(self, *keys):
        results = []
        ks = self.__class__.KEYS
        for k in keys:
            key = '%s%s'%(self.__class__.PREFIX_PACKET_KEY, k)
            if hasattr(ks, key):
                results.append(self.get(getattr(ks, key)))
            elif hasattr(ks, k):
                results.append(self.get(getattr(ks, k)))
            elif hasattr(PacketKey, key):
                results.append(self.get(getattr(PacketKey, key)))
            elif hasattr(PacketKey, k):
                results.append(self.get(getattr(PacketKey, k)))
            else:
                results.append(None)
        if len(keys) == 1:
            return results[0]
        return tuple(results)

    @FakeProperty
    def data(self):
        return dict(self)

    def __getattr__(self, attName):
        key = '%s%s'%(self.__class__.PREFIX_PACKET_KEY, attName)
        ks = self.__class__.KEYS
        if hasattr(ks, key):
            return self.get(getattr(ks, key))
        elif hasattr(ks, attName):
            return self.get(getattr(ks, attName))
        elif hasattr(PacketKey, key):
            return self.get(getattr(PacketKey, key))
        elif hasattr(PacketKey, k):
            return self.get(getattr(PacketKey, k))
        else:
            return object.__getattribute__(self, attName)

    def __setattr__(self, attName, v):
        key = '%s%s'%(self.__class__.PREFIX_PACKET_KEY, attName)
        ks = self.__class__.KEYS
        if hasattr(ks, key):
            self.__setitem__(getattr(ks, key), v)
        elif hasattr(ks, attName):
            self.__setitem__(getattr(ks, attName), v)
        elif hasattr(PacketKey, key):
            self.__setitem__(getattr(PacketKey, key), v)
        elif hasattr(PacketKey, k):
            self.__setitem__(getattr(PacketKey, k), v)
        else:
            object.__setattr__(self, attName, v)

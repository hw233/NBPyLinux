#coding: utf-8

from protocol.CommonProtocol import PacketType, PacketType, PacketSchema, GeneralPack

class __AntiType(object):

    PT_C2S_CHECK_ID_CARD = 0x160001


    def __setattr__(self,attr,val):
        raise TypeError("Attribute is readonly")
        pass

AntiType = __AntiType()


class __AntiKey(object):
    PK_ROLE_ID              = 0x1601 #角色ID
    PK_ROLE_SSID            = 0x1602 #角色SSID
    PK_ROLE_TOKEN           = 0x1603 #角色TOKEN
    PK_ROLE_NAME            = 0x1604 #角色名字
    #
    PK_IDCARD_NAME          = 0x1605 #身份证名字
    PK_IDCARD_ID            = 0x1606 #身份证号码


    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

AntiKey = __AntiKey()

#Error codes
class __AntiError(object):

    __slots__ = ()

    ERR_OK                      = 0                     #正常
    ERR_INVALID_PARAM           = -11001                #参数错误
    ERR_REPORT_NO_EXISTS        = -11002                #战报不存在
    ERR_REPORT_FORMAT           = -11003                #战报格式错误




AntiError = __AntiError()

class AntiPack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = AntiKey
    def __init__(self, data=None):
        super(AntiPack, self).__init__(data)

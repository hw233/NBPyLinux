#coding:utf-8
from CommonProtocol import PackKey,PackType,PackSchema
from GeneralPack import GeneralPack

class __PlatformType(object):
    __slots__ = ()

    PT_S2S_CHECK_LOGIN = 0x1001

    def __setattr__(self, n, v):  raise TypeError("Attribute is readonly")

PlatformType = __PlatformType()



class __PlatformKey(object):
    __slots__ = ()

    PK_PLATFORM_CP      = 0x1001
    PK_PLATFORM_CPTOKEN = 0x1002
    PK_PLATFORM_CPUID   = 0x1003

    def __setattr__(self, n, v):  raise TypeError("Attribute is readonly")

PlatformKey = __PlatformKey()

#Error codes
class __PlatformError(object):
    __slots__ = ()
    ERR_OK                                  = 0                   # 正常
    ERR_PLATFORM_UNKNOWN                    = -501               # 未知错误
    ERR_INVALID_PARAM                       = -502               # 参数不正确
    pass


PlatformError = __PlatformError()


class PlatformPack(GeneralPack):
    __slots__ = ()
    def __init__(self, data=None):
        super(PlatformPack, self).__init__(data, PlatformKey)



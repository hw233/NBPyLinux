#coding:utf-8
from protocol.CommonProtocol import PacketKey,PacketType,PacketSchema
from protocol.CommonProtocol import GeneralPack

'''
定义逻辑处理器节点的tag类型
'''
TAG_DEMO = 0x01FFFF

'''
定义消息类型
'''
class __DemoType(object):
    __slots__ = ()
    PT_DEMO_ECHO             = 10001
    PT_S2S_DEMO_ECHO             = 20001
    def __setattr__(self,n,v):  raise TypeError("Attribute is readonly")

DemoType = __DemoType()

'''
定义消息内可用的键
'''
class __DemoKey(object):
    __slots__ = ()
    PK_DEMO_ECHO_TEXT       = 0x100
    def __setattr__(self,n,v):  raise TypeError("Attribute is readonly")

DemoKey = __DemoKey()

'''
自定义错误码(注意别和CommonProtocol.py内的错误码数值冲突)
'''
#Error codes
class __DemoError(object):
    __slots__ = ()
    ERR_OK                                      = 0                   #正常
    ERR_DEMO_UNKNOWN                            = -5001               #未知错误
    ERR_DEMO_TEXT_INVALID                       = -5002               #文本非法
    pass

DemoError = __DemoError()

'''
自定义消息的封装类 DemoPack.

简介:
    让指定的消息可用键中形如 'PK_XXX' 的键对应的值可以通过使用例如 DemoPack.XXX 来访问


示例:
    #-------- 解析 -----------
    #`msg` 是一个从服务器群组投递过来的数据字典.
    ipack = DemoPack(msg)
    print ipack.SOME_KEY

    #-------- 构造 -------
    ipack = DemoPack()
    ipack.KEY1 = 'value1'
    ipack.KEY2  = 'value2'
    ....
    send( ipack.data )
'''
class DemoPack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = DemoKey
    def __init__(self, data=None):
        super(DemoPack, self).__init__(data)



#coding: utf-8

from protocol.CommonProtocol import PacketType, PacketType, PacketSchema, GeneralPack

class __FriendType(object):

    PT_S2C_FRIEND_LIST        = 0x70001 #好友列表
    PT_C2S_FRIEND_APPLY_LIST  = 0x70002 #好友请求列表
    PT_C2S_FRIEND_PASS_REJECT = 0x70003 #通过或拒绝添加好友
    PT_C2S_FRIEND_ADD         = 0x70004 #发送好友请求
    PT_C2S_FRIEND_DEL         = 0x70005 #删除好友
    PT_S2C_ADD_FRIEND         = 0x70006 #成功添加好友，更新好友列表
    PT_S2C_APPLY_FRIEND       = 0x70007 #发送好友请求，更新请求列表
    PT_S2C_DEL_FRIEND         = 0x70008 #成功删除好友，更新好友列表
    PT_S2C_FRIEND_APPLY_LIST  = 0x70009 #s2c好友请求列表
    PT_C2S_FRIEND_LIST        = 0x7000a #好友列表

    def __setattr__(self,attr,val):
        raise TypeError("Attribute is readonly")
        pass

FriendType = __FriendType()


class __FriendKey(object):

    PK_ROLE_ID            = 0x701 #角色ID
    PK_ROLE_SSID          = 0x702 #角色SSID
    PK_ROLE_TOKEN         = 0x703 #角色TOKEN
    PK_ROLE_NAME          = 0x704 #角色名字
    PK_ROLE_PRESTIGE      = 0x705 #声望
    PK_ROLE_LASTLOGIN     = 0x706 #最后登录时间截
    PK_ROLE_HEAD          = 0x707 #角色头像
    PK_FRIEND_PARAM_TYPE  = 0x708 #类型
    PK_FRIEND_UPDATED_AT  = 0x709 #更新时间
    PK_FRIEND_STATUS      = 0x70a #状态



    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

FriendKey = __FriendKey()

#Error codes
class __FriendError(object):

    __slots__ = ()

    ERR_OK                     = 0                    #正常
    ERR_INVALID_PARAM          = -7001                #参数错误
    ERR_FRIEND_LIST_FULL       = -7002                #好友列表已满，无法添加
    ERR_FRIEND_YOUR_FRIEND     = -7003                #该玩家已是你的好友
    ERR_FRIEND_LIST_FULL_OTHER = -7004                #对方好友列表已满，无法添加
    ERR_FRIEND_APPLY_LIST_FULL = -7005                #对方好友请求列表已满，请稍后再试
    ERR_ROLE_NAME_NOT_TRUE     = -7006                #当前所输入的名字不存在
    ERR_FRIEND_ADD_MYSELF      = -7007                #不能向自己发送请求

FriendError = __FriendError()

class FriendPack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = FriendKey
    def __init__(self, data=None):
        super(FriendPack, self).__init__(data)

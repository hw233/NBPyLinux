#coding: utf-8

from protocol.CommonProtocol import PacketType, PacketType, PacketSchema, GeneralPack

class __MailType(object):

    PT_C2S_MAIL_LIST        = 0x80001       # C2S邮件列表(系统邮件，好友邮件)
    PT_S2C_MAIL_LIST        = 0x80002       # S2C邮件列表(S2C返回邮件[{mid:mailpack},...])

    PT_C2S_GET_MAIL_BY_TMID = 0x80003       # 同盟邮件
    PT_DEL_MAIL             = 0x80004       # 删除邮件
    PT_SEND_MAIL            = 0x80005       # 发送邮件
    PT_GET_MAIL_GIFT        = 0x80006       # 领取邮件附件
    PT_READ_MAIL            = 0x80007       # 读取邮件

    # S2S
    PT_S2S_UPDATE_ALIANCED  = 0x80008
    PT_S2S_SEND_MAIL        = 0x80009
    PT_S2S_UPDATE_MAIL      = 0x80010       #通知更新邮件
    PT_S2S_REFLASH_MAIL     = 0x80011       #刷新邮件
    def __setattr__(self,attr,val):
        raise TypeError("Attribute is readonly")
        pass

MailType = __MailType()


class __MailKey(object):

    PK_ROLE_ID            = 0x801 #角色ID
    PK_ROLE_SSID          = 0x802 #角色SSID
    PK_ROLE_TOKEN         = 0x803 #角色TOKEN
    PK_ROLE_NAME          = 0x804 #角色名字
    #
    PK_MAIL_SUID          = 0x805 # 发件人id
    PK_MAIL_SNAME         = 0x806 # 发件人名字
    PK_MAIL_TUID          = 0x807 # 收件人id
    PK_MAIL_TNAME         = 0x808 # 收件人名字
    PK_MAIL_TITLE         = 0x809 # 标题
    PK_MAIL_CONTENT       = 0x810 # 内容
    PK_MAIL_CREATTIME     = 0x811 # 创建时间
    PK_MAIL_STATUS        = 0x812 # 状态
    PK_MAIL_ISGET         = 0x813 # 附件是否被领取
    PK_MAIL_GIFT          = 0x814 # 附件
    PK_MAIL_ID            = 0x815 # 邮件ID
    PK_MAIL_TYPE          = 0x816 # 邮件类型
    PK_MAIL_TMID          = 0x817 # 同盟邮件ID
    PK_ALIANCE_ID         = 0x817 # 同盟ID


    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

MailKey = __MailKey()

#Error codes
class __MailError(object):

    __slots__ = ()

    ERR_OK                     = 0                    #正常
    ERR_MAIL_FULL              = -80001               #邮箱满了


MailError = __MailError()

class MailPack(GeneralPack):
    __slots__ = ()
    PREFIX_PACKET_KEY = 'PK_'
    KEYS = MailKey
    def __init__(self, data=None):
        super(MailPack, self).__init__(data)

#coding:utf-8

'''processor transfer'''
from protocol import *

HANDLER_TYPE_C2S = 0
HANDLER_TYPE_S2S = 1

__pre_processor_handler__ = {}
__processor_handler_c2s__ = {}
__processor_handler_s2s__ = {}
__processor_cls__ = []
__current_clskey__ = None

class Processor(object):
    def __init__(self):
        global __current_clskey__,__pre_processor_handler__,__processor_handler_c2s__,__processor_handler_s2s__
        cn = self.__class__.__name__
        if __pre_processor_handler__.get(cn) is None: return
        dct = __pre_processor_handler__.get(cn)
        for k,v in dct.items():
            if v[1] == HANDLER_TYPE_C2S:
                __processor_handler__ = __processor_handler_c2s__
                #print "[register-c2s-processor] '%s.%s(%s)' -> '%s'"%(cn,v[0].__name__,v[0].func_doc,k)
            elif v[1] == HANDLER_TYPE_S2S:
                __processor_handler__ = __processor_handler_s2s__
                #print "[register-s2s-processor] '%s.%s(%s)' -> '%s'"%(cn,v[0].__name__,v[0].func_doc,k)
            else:
                continue

            if __processor_handler__.get(k) is None: __processor_handler__[k] = []
            __processor_handler__[k].append((self,v[0]))

        del __pre_processor_handler__[cn]
        print '[class-register complete] %s'%(cn)
        pass

    @staticmethod
    def wrap(cls):
        global __current_clskey__
        __current_clskey__ = cls

    @staticmethod
    def register(event, tp = HANDLER_TYPE_C2S):
        def __handler(fn):
            global __current_clskey__,__pre_processor_handler__

            if __current_clskey__ is None:  return fn
            if __pre_processor_handler__.get(__current_clskey__) is None:
                __pre_processor_handler__[__current_clskey__] = {}

            __pre_processor_handler__[__current_clskey__][event] = (fn,tp)
            return fn
        return __handler


    @staticmethod
    def dispatch_proc(event,**dictParams):
        global __processor_handler_c2s__,__processor_handler_s2s__
        tp = dictParams.get('ACTION',HANDLER_TYPE_C2S)
        if tp == HANDLER_TYPE_C2S:
            __processor_handler__ = __processor_handler_c2s__
        elif tp == HANDLER_TYPE_S2S:
            __processor_handler__ = __processor_handler_s2s__
        else:
            #raise ValueError('processor proc not found[%(tp)d,%(event)s]'%(locals()))
            return

        if __processor_handler__.get(event) is None:
            #raise ValueError('processor proc not found[%(tp)d,%(event)s]'%(locals()))
            return
        for fn in __processor_handler__[event]:
            fn[1](fn[0],**dictParams)

        

    @staticmethod
    def dispatch_func(event,**dictParams):
        global __processor_handler_c2s__,__processor_handler_s2s__
        tp = dictParams.get('ACTION',HANDLER_TYPE_C2S)
        ret = ErrorCode.ERR_NORETURN
        if tp == HANDLER_TYPE_C2S:
            __processor_handler__ = __processor_handler_c2s__
        elif tp == HANDLER_TYPE_S2S:
            __processor_handler__ = __processor_handler_s2s__
        else:
            #raise ValueError('processor func not found[%(tp)d,%(event)s]'%(locals()))
            return ret
        if __processor_handler__.get(event) is None:
            #raise ValueError('processor func not found[%(tp)d,%(event)s]'%(locals()))
            return ret

        for fn in __processor_handler__[event]:
            ret = fn[1](fn[0],**dictParams)
        return ret

    @staticmethod
    def dispatch_func_ret(event,**dictParams):
        global __processor_handler_c2s__,__processor_handler_s2s__
        tp = dictParams.get('ACTION',HANDLER_TYPE_C2S)
        ret = ErrorCode.ERR_NORETURN
        if tp == HANDLER_TYPE_C2S:
            __processor_handler__ = __processor_handler_c2s__
        elif tp == HANDLER_TYPE_S2S:
            __processor_handler__ = __processor_handler_s2s__
        else:
            #raise ValueError('processor func not found[%(tp)d,%(event)s]'%(locals()))
            return ret,None
        ehs = __processor_handler__.get(event)
        if ehs is None:
            #raise ValueError('processor func not found[%(tp)d,%(event)s]'%(locals()))
            return ret,None

        for fn in ehs:
            #print 'exec %s(%s) params: %s'%(fn[1].func_doc,fn[1], dictParams)
            return fn[1](fn[0],**dictParams)



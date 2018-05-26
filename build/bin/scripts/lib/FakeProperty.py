#coding: utf-8

class __FakeProperty(object):
    def __init__(self):
        pass

    def __call__(self, pf):
        p = property(fget=pf)
        return p

    def set(self, fn):
        try:
            return fn.setter
        except Exception, ex:
            def __fnproc(pf):
                get_fn = None
                def __set(po, v):
                    pf(po, v)
                return property(fget=fn.fget, fset=__set)
            return __fnproc

FakeProperty = __FakeProperty()

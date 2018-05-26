#coding: utf-8

#import MLSE
import kernel
import time
import stackless

class __TimerMode(object):

    TM_ONCE = 0     #一次
    TM_LOOP = 1     #无限
    TM_SPEC = 2     #指定次数

    def __setattr__(self,attr,val): raise TypeError("Attribute is readonly")

TimerMode = __TimerMode()

class Timer(object):
    '''
    定时器类
    参数说明:
    func > 定时器要执行的方法
    interval > 定时时间间隔
    mode > 执行次数(一次、无限、指定次数。参照TimerMode)
    numberoftimes > 指定次数类型时，设定执行次数
    tag > 定时器标识

    '''
    def __init__(self, func, interval, mode = TimerMode.TM_LOOP, numberoftimes = -1, tag = 'timer%d'%(int(time.time()))):
        print 'Timer `%s` created'%(tag)
        self._tag           = tag
        self._func          = func
        self._status        = True
        self._st            = time.time()
        self._mode          = mode
        self._interval      = interval
        self._numberoftimes = numberoftimes
        pass

    def __del__(self):
        print 'Timer `%s` removed'%(self._tag)
        pass

    def __call__(self, *args, **kwargs):
        def _op():
            times = 0
            while True:
                if not self._status:
                    break     
                if time.time() - self._st >= self._interval:
                    self._st = time.time()
                    self._func(*args, **kwargs)
                    times += 1
                    if self._mode == TimerMode.TM_ONCE:
                        break
                    if self._mode == TimerMode.TM_SPEC:
                        if times >= self._numberoftimes:
                            break
                kernel.BeNice()
                pass
            pass
        stackless.tasklet(_op)().insert()
        pass
    
    #取消定时器
    def CancelTimer(self):
        self._status = False
        pass

    #有参数
    def decorator(interval, mode = TimerMode.TM_LOOP, numberoftimes = -1, tag = 'timer%d'%(int(time.time()))):
        '''
        定时器装饰器，可以直接装饰一个无参数方法使它成为定时器方法
        参数说明:
        func > 定时器要执行的方法
        interval > 定时时间间隔
        mode > 执行次数(一次、无限、指定次数。参照TimerMode)
        numberoftimes > 指定次数类型时，设定执行次数
        tag > 定时器标识

        '''
        def _func(fn, *args, **kwargs):
            Timer(fn, interval, mode, numberoftimes, tag)()
            print args
            return fn
            pass
        return _func


#无参数
def decorator(interval, mode = TimerMode.TM_LOOP, numberoftimes = -1, tag = 'timer%d'%(int(time.time()))):
    '''
    定时器装饰器，可以直接装饰一个无参数方法使它成为定时器方法
    参数说明:
    func > 定时器要执行的方法
    interval > 定时时间间隔
    mode > 执行次数(一次、无限、指定次数。参照TimerMode)
    numberoftimes > 指定次数类型时，设定执行次数
    tag > 定时器标识

    '''
    def _func(fn):
        Timer(fn, interval, mode, numberoftimes, tag)()
        return fn
        pass
    return _func

    
    

#-*- coding: utf-8 -*-
#Author: velen

import random, re


# some_list = [1,2,3,4]
# probabilities = [0.2,0.1,0.6,0.1]
# print random_pick(some_list,probabilities)
def random_pick(some_list, probabilities, odd=1.0):
    # 概率
    x = random.uniform(0,1)
    cumulative_probability = 0.0
    for item, item_probability in zip(some_list, probabilities):
        cumulative_probability += item_probability / odd
        if x < cumulative_probability:break
    return item

def random_pick_dict(some_dict, odd=1.0):
    # 概率
    x = random.uniform(0,1)
    cumulative_probability = 0.0
    for item, item_probability in some_dict.iteritems():
        cumulative_probability += item_probability / odd
        if x < cumulative_probability:break
    return item


# some_list = [1,2,3,4]
# odds=[3,1,4,2]
# print random_pick_odd(some_list,odds)
def random_pick_odd(some_list, odds):
    # 权重
    table = [z for x,y in zip(some_list,odds) for z in [x] * y]
    return random.choice(table)

# map1={'1':'2','3':'4',}
# str='1133'
# print multiple_replace(str,map1)
def multiple_replace(text,adict):
    rx = re.compile('|'.join(map(re.escape,adict)))
    def one_xlat(match):
        return adict[match.group(0)]
    return rx.sub(one_xlat,text) #每遇到一次匹配就会调用回调函数

# txt = "1;3|4;5"
def convert_multiple_dict(txt, d={";":":", "|":','}):
    return eval("{%s}"%multiple_replace(txt, d))

#!/usr/bin/env python
# -*- coding:utf-8 -*-
import csv
from StringIO import StringIO

class CSVParser:
    __default_path = "../data/"
    def __init__(self):
            pass
    @staticmethod
    def Parse(input_file,path=None):
        if path is None:    input_file = CSVParser.__default_path + input_file
        else:           input_file = path + input_file
        return CSVParser._parse_csv(input_file, skip=1)

    @staticmethod
    def _parse_csv(input_file, skip=0,strip=True):
        src = file(input_file, "rb")
        line = src.readline()
        s = csv.Sniffer()
        _dialect = s.sniff( line )
        headers = None
        if s.has_header( line ):
            def _head_strip(h):
                tmp = h.strip()
                if len(tmp) > 2:
                    quote = tmp[0] + tmp[-1]
                    if quote == "\"\"" or quote == "''":    return tmp[1:-1]
                return tmp
            headers = map( _head_strip,line.split(_dialect.delimiter) )
            while True:                
                try:    headers.remove('')
                except Exception,ex:    break
        
        reader = csv.DictReader(src, headers,dialect=_dialect)
        # skip the first line
        for i in xrange(skip - 1):
            try:    reader.next()
            except StopIteration,si:    return []

        result_list = []
        for row in reader:
            if strip:
                for _k, _v in row.iteritems():
                    if _v is str:    row[_k] = _v.strip()
            result_list.append(row)
        return result_list


##  CSV解析器
#   支持将特定格式的字符串自动转换为PYTHON内置类型：
#       '(...)':tuple
#       '[...]':list
#       '{...}':dict
#
class SmartCSVParser:
    __special  = ('()','[]','{}','\"\"','\'\'')
    __default_path = "./scripts/data/"
    def __init__(self,fname):
        fname = SmartCSVParser.__default_path + fname
        self.__file     = file(fname,"rb")

    def __del__(self):  
        if self.__file:
            self.__file.close()
    
    ##
    #originals:需要保存原始数据的列.原始数据以"__"为前缀放入行字典.如:id列被保存原始数据后,行字典会多一项['__id']=xxx的item.
    def Parse(self,headers=None,skip=0,originals=[]):
        self.__file.seek(0)
        line_1 = self.__file.readline()
        s = csv.Sniffer()
        _dialect = s.sniff( line_1 )
        #----- generate headers.
        if not headers:
            if skip != 0:   
                raise("Where is the headers?")
            if s.has_header( line_1 ):
                def _head_strip(h):
                    tmp = h.strip()
                    if len(tmp) > 2:
                        quote = tmp[0] + tmp[-1]
                        if quote == "\"\"" or quote == "''":    return tmp[1:-1]
                    return tmp
                headers = map( _head_strip,line_1.split(_dialect.delimiter) )
                while True:                
                    try:    headers.remove('')
                    except Exception,ex:    break
        else:
            #with given header,we should seek back to the begin of the file.
            self.__file.seek(0)
        reader = csv.DictReader( self.__file,headers,dialect=_dialect )
        result = []
        while skip: reader.next(); skip -= 1

        #-----------------------------
        #   Make result list.
        for row in reader:
            res = {}
            for _k in headers:
                _v = row.get( _k,None )
                if isinstance(_v,str):
                    tmp = _v.strip()
                    if len(tmp) >= 2:
                        sep = tmp[0]+tmp[-1]
                        if sep in self.__special:       
                            try:
                                res[_k] = eval(tmp)
                            except: res[_k] = tmp
                        else:                       
                            try:    
                                if tmp.find('.') >=0:   res[_k] = float(tmp)
                                else:                   res[_k] = long(tmp)
                            except: res[_k] = tmp
                    else:                       
                        if len(tmp) == 0:           res[_k] = None
                        else:                       
                            try:    
                                if tmp.find('.') >= 0:   res[_k] = float(tmp)
                                else:                    res[_k] = long(tmp)
                            except: res[_k] = tmp
                #
                if _k in originals:     res['__%s'%_k] = _v
            result.append(res)
        return result

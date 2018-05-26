#coding: utf-8

import urllib
import urllib2
import socket
#import stackless

socket.setdefaulttimeout(3.0)

def post_json(url, data):
    def _fn(url, data):
        try:
            req = urllib2.Request(url)
            data = str(data)
            data = data.replace("'", '"')
            opener = urllib2.build_opener(urllib2.HTTPCookieProcessor())
            response = opener.open(req, data)
            ret = response.read()
            #print '%s RETURN >>>>'%(url),ret
            return ret
        except Exception, ex:
            #print '%s ERROR >>>>'%(url),ex
            return ""
    r = _fn(url, data)
    return r

def post(url, data):
    def _fn(url, data):
        try:
            req = urllib2.Request(url)
            data = str(data)
            opener = urllib2.build_opener(urllib2.HTTPCookieProcessor())
            response = opener.open(req, data)
            ret = response.read()
            return ret
        except Exception, ex:
            print 'http.post exception: ', ex
            return ""
    r = _fn(url, data)
    return r

'''
def post_for_httplib(url, data):
    try:
        conn = httplib.HTTPConnection("passport_i.25pp.com",8080)
    except Exception,e:
        print e
    headers = {"Host":"passport_i.25pp.com","Content-Length":"32"}
    param = ("07d72e5200cb5c0031d988e618d90d5b")
    conn.request("POST" ,"/index?tunnel-command=2852126756", param, headers)
    response = conn.getresponse()
    data = response.read()
    print(data)
    conn.close()
'''
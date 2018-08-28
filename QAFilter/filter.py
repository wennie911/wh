#coding=utf-8
from ctypes import *

import datetime
import sys
reload(sys)
sys.setdefaultencoding("utf-8")

class Cfilter:

    def __init__(self):
        self.filterlib = None
        self.answerJson = create_string_buffer("",10240)
        

	#初始化数据库路径和动态库路径
    def initFilter(self):
        self.filterlib = cdll.LoadLibrary('./libfilter.so')
        
        self.filterlib.qaf_init()
        

	#测试应答入口，如：get('你好')
    def get(self, data):
        c_s = c_char_p(data)
        self.filterlib.qaf_get_answer(c_s, self.answerJson, 10240)
        #print self.answerJson.value
        if len(self.answerJson.value) > 0:
            return self.answerJson.value

        return ""



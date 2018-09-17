#coding=utf-8
from ctypes import cdll
from ctypes import c_int
from ctypes import c_char_p
from ctypes import create_string_buffer
import datetime
import sys
reload(sys)
sys.setdefaultencoding("utf-8")

class AiBao:

    def __init__(self):
        self.nlulib = None
        self.answer = create_string_buffer("I'm studying !",1024)
        self.o_command = create_string_buffer("",1024)
        self.o_print = create_string_buffer("",10240)

	#初始化数据库路径和动态库路径
    def initnlu(self, path):
        if path[len(path)-1] is not '/':
            path = path + '/'
        libpath = path + 'libnlu.so'
        self.nlulib = cdll.LoadLibrary(libpath)
        self.nlulib.set_log_level(1)
        self.nlulib.segmente.restype = c_char_p
        self.nlulib.segmentefile.restype = c_int
        dbpath = path + 'dataLibs/'
        ret = self.nlulib.nlu_init(dbpath)
        if ret == 1:
            return True

        return False

	#测试应答入口，如：get('你好')
    def get(self, data):
        c_s = c_char_p(data)
        self.nlulib.nlu_push_text(c_s, self.answer, 1024, self.o_command, 1024, self.o_print, 10240)
        if len(self.o_print.value) > 0:
            return self.o_print.value

        return ""

	#检查是否有繁体字，有责转换为简体，bool
    def set_t2s(self, tag):
        self.nlulib.set_t2s(tag)

	#设置只分词，无词性输出
    def set_seg_only(self, tag):
        self.nlulib.set_seg_only(tag)

	#设置只提取中文
    def set_ch_only(self, tag):
        self.nlulib.set_ch_only(tag)

	#设置分割符，词间间隔符
    def set_deli(self, tag):
        self.nlulib.set_deli(tag)

	#单句分词
    def segmente(self, sent):
        c_s = c_char_p(sent)
        return self.nlulib.segmente(c_s)

	#文件分词
    def segmentefile(self, infile, outfile):
        c_infile = c_char_p(infile)
        c_outfile = c_char_p(outfile)
        return self.nlulib.segmentefile(c_infile, c_outfile)


####################################################################  
## file        : makefile for build nlu dir                        ##  
## author      : zhangjian                                         ##  
## date-time   : 2016.11.09                                        ##  
#####################################################################  

CC = gcc

LIBS = -lpthread -ldl

CPP = g++
CPPFLAGS := -std=c++11 -w -g -DWECHAT -fPIC -shared

TARGET = libnlu.so
RM = rm -rf

CPPSRC = $(shell find $(APPDIR) -name '*.cpp')

all:
	$(CPP) $(CPPFLAGS) -L $(PWD)/libs/linux $(CPPSRC) -o $(TARGET) -lsqlite3 -lgum -lcurl $(LIBS)
clean:
	$(RM) $(TARGET)


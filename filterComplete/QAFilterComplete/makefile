CC = gcc

LIBS = -ldl

CPP = g++
CPPFLAGS := -std=c++11 -w -g -fPIC -shared

TARGET = libfilter.so
RM = rm -rf

CPPSRC = $(shell find $(APPDIR) -name '*.cpp')

all:
		$(CPP) $(CPPFLAGS) $(CPPSRC) -o $(TARGET) $(LIBS)
clean:
		$(RM) $(TARGET)
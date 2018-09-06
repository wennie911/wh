CC = gcc
CFLAGS = -lesl -lm -lmsc -lrt -ldl -lpthread -lcurl -lmysqlclient
ESLINCPATH = /usr/local/src/freeswitch/libs/esl/src/include
TTSINCPATH = /usr/local/src/keda_asr_linux/include
ESLLIBPATH = /usr/local/src/freeswitch/libs/esl/.libs/
TTSLIBPATH = /usr/local/src/keda_asr_linux/libs/x64
MYSQLLIBPATH = /usr/lib64/mysql

TARGET = aieslserver
OBJS = ai.o aiesl.o esl_control.o json.o tts.o calllog.o loadconfig.o evaluate.o
RM = rm -f

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) -L $(ESLLIBPATH) -L $(TTSLIBPATH) -L $(MYSQLLIBPATH) $(CFLAGS)

$(OBJS):%o:%c
	$(CC) -c $(CFLAGS) -I $(ESLINCPATH) -I $(TTSINCPATH)  $< -o $@

clean:
	$(RM) $(TARGET) $(OBJS)



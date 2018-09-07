#ifndef _H_CALL_LOG
#define _H_CALL_LOG

#include <time.h>

enum callstatus
{
	CS_START = 1,			//启动originate进程
	CS_ACTIVATE,			//进入inbound侦测到启动
	CS_CONNECT,				//进入outbound
	CS_BRIDGE,				//转呼
	CS_DISCONNECT,			//退出outbound
	CS_UNACTIVATE			//inbound侦测到断掉
};

const char *get_calllog_error();
int login_db();
void logout_db();
int add_record_info(char *jobUUID, char *log_filepathname);
int add_chat_log(char *jobUUID, char *question, char *answer, const char *datetime);
int set_chat_hungup_info(char *jobUUID, int hungup_reason, time_t during_time, char level);
const char *get_calllog_error();
void get_datetime(char *sDateime);
int set_record_level(char *jobUUID, char level);

#endif

#ifndef _H_CALL_LOG
#define _H_CALL_LOG

enum callstatus
{
	CS_START = 1,			//����originate����
	CS_ACTIVATE,			//����inbound��⵽����
	CS_CONNECT,				//����outbound
	CS_BRIDGE,				//ת��
	CS_DISCONNECT,			//�˳�outbound
	CS_UNACTIVATE			//inbound��⵽�ϵ�
};

const char *get_calllog_error();
int login_db();
void logout_db();
int add_record_info(char *jobUUID, char *log_filepathname);
int add_chat_log(char *jobUUID, char *question, char *answer, const char *datetime);
const char *get_calllog_error();
void get_datetime(char *sDateime);

#endif

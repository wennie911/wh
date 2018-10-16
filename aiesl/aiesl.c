#include <stdio.h>
#include <stdlib.h>
#include <esl.h>
#include <pthread.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include "ai.h"
#include "esl_control.h"
#include "calllog.h"
#include "loadconfig.h"
#include "evaluate.h"
#include "nluProcess.h"
#include "webapi.h"

static int flag = 0;
static int chat_count = 0;
static int chat_typed  = 0;
static int chat_end = 0;
int	g_customer_talk_count = 0;
int g_ignore_earlymedia = 0;
const char *CONF_FILE_PATH = "/etc/freeswitch/system.conf";

typedef enum {
	SESSION_INIT,				//初始化
	SESSION_WAIT_ANSWER,		//等待被叫应答
	SESSION_START,				//流程开始
	SESSION_START_READY,		//开场白结束
	SESSION_CONTINUE,			//流程进行中
	SESSION_STOP				//流程结束
} session_status_types_t;

struRetAnswer g_retAnswer;

int g_session_status = SESSION_INIT;
int g_ASR_process = 1;

static void event_callback(esl_handle_t *handle, esl_event_t *event, char *sound_path, char *record_file_pathname, char *record_www_pathname, int *nDisconnet)
{
	char *job_UUID = esl_event_get_header(handle->info_event, "unique-id");

	if( event )
	{
		char *application = esl_event_get_header(event, "Application");
		char *event_name = esl_event_get_header(event, "Event-Name");
		char *event_subclass = esl_event_get_header(event, "Event-Subclass");
		esl_log(ESL_LOG_INFO, "JobID:%s event_callback Application: %s event_name: %s event_subclass: %s event_id: %d \n", job_UUID, application, event_name, event_subclass, event->event_id);


		/*
		if( event->event_id == ESL_EVENT_CHANNEL_HANGUP)
		{
		*nDisconnet = 1;
		goto callback_end;
		}
		*/

		if( event->event_id == ESL_EVENT_CHANNEL_ANSWER &&  SESSION_WAIT_ANSWER == g_session_status )
		{
			g_session_status = SESSION_START;
			DialogStart(handle, sound_path, record_file_pathname, record_www_pathname );
			goto callback_end;
		}
		
		if( event->event_id == ESL_EVENT_PLAYBACK_START )
		{
			esl_log(ESL_LOG_INFO, "ESL_EVENT_PLAYBACK_START sound: %s  \n", esl_event_get_header(event, "palybacksound"));
			if( SESSION_START == g_session_status )
			{
				g_session_status = SESSION_START_READY;
			}
		}
		else if ( event->event_id == ESL_EVENT_PLAYBACK_STOP )
		{
			esl_log(ESL_LOG_INFO, "ESL_EVENT_PLAYBACK_STOP sound: %s  \n", esl_event_get_header(event, "palybacksound"));
			if( SESSION_START_READY == g_session_status )
			{
				g_session_status = SESSION_CONTINUE;
			}
		}
	
		if( event_subclass && !strcasecmp(event_subclass, "CALLTIMEOUT") )
		{
			*nDisconnet = 1;
			goto callback_end;
		}
	}

	if( g_ASR_process )
	{
		char sAsrResult[1024] = {0};
		char sAsrWords[MAXANSWERLEN] = {0};
		if( !GetAsrResult(sAsrResult) )
			goto callback_end;

		char sDatetime[64];
		get_datetime(sDatetime);

		esl_log(ESL_LOG_INFO, "JobID:%s GetAsrResult:%s\n", job_UUID, sAsrResult);
		dojsonlist(sAsrResult, sAsrWords);
		if( strlen(sAsrWords) <= 0 )
			goto callback_end;

		g_customer_talk_count ++;
		if( g_session_status < SESSION_CONTINUE )
		{
			esl_log(ESL_LOG_INFO, "JobID:%s Not reach continue status! Return!\n", job_UUID);		
			add_chat_log(job_UUID, sAsrWords, "", sDatetime );
			goto callback_end;
		}

		int iRet=0;
		char sSoundName[MAXANSWERLEN] = {0};
		memset(&g_retAnswer, 0, sizeof(struRetAnswer));
		iRet = processNLU(sAsrWords, &g_retAnswer);
		if( iRet )
		{
			if( strlen(g_retAnswer.sAudio) )
			{
				match_keyword(g_retAnswer.sQuestion);
				match_keyword_pingyin(g_retAnswer.sQuestionPinYin);

				esl_play_break(handle);
				esl_play_sound(handle, sound_path, g_retAnswer.sAudio);
				add_chat_log(job_UUID, g_retAnswer.sQuestion, g_retAnswer.sAnswer, sDatetime );
			}

			if(g_retAnswer.iIsEnd)
			{
				*nDisconnet=1;
				chat_end = 1;
				goto callback_end;
			}
		}
		
	}

callback_end:
	//esl_log(ESL_LOG_INFO, "event_callback end!Session_status:%d\n", g_session_status);
	return;
}

void give_permission(char * path_or_pathname)
{
	char cmd_chmod[250];
	sprintf(cmd_chmod, "chmod 777 %s", path_or_pathname);
	esl_log(ESL_LOG_INFO, "command! %s\n", cmd_chmod);
	system(cmd_chmod);
}

int  create_log_dic(char *record_path, struct tm *tm_now)
{
	char record_file_path[250];
	sprintf(record_file_path, "%s/record/%04d%02d%02d", record_path,  tm_now->tm_year+1900, tm_now->tm_mon+1, tm_now->tm_mday);
	
	struct stat statbuf;
	if ( 0 != stat(record_file_path, &statbuf))   
	{
		if (ENOENT == errno)  
		{
			esl_log(ESL_LOG_INFO, "create_log_dic path: %s!\n", record_file_path);
			if (-1 == mkdir(record_file_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
			{
				return -1;
			}

			give_permission(record_path);

		}
		else
		{
			return -2;
		}
	}

	return 0;
}

void DialogStart(esl_handle_t *handle, char *sound_path, char *record_file_pathname, char *record_www_pathname )
{
	const char *unique_id = esl_event_get_header(handle->info_event, "unique-id");

	evaluate_start();

	esl_record_start(handle, record_file_pathname);

	if( add_record_info(unique_id, record_www_pathname) )
	{
		esl_log(ESL_LOG_ERROR, "add_record_info error:%s!\n", get_calllog_error());
	}
	update_log_connectd(unique_id);
	
	struRetAnswer retAnswer;
	char sDatetime[64];
	get_datetime(sDatetime);
	memset(&retAnswer, 0x0, sizeof(struRetAnswer));
	
	if(ai_get_answer("开场白", &retAnswer))
	{
		esl_log(ESL_LOG_INFO, "开场白!%s!\n", retAnswer.sAnswer);
		if(strlen(retAnswer.sAudio))
		{
			esl_play_sound(handle, sound_path, retAnswer.sAudio);

			add_chat_log(esl_event_get_header(handle->info_event, "unique-id"), " ", retAnswer.sAnswer, sDatetime);
		}
	}
	else
	{
		esl_log(ESL_LOG_ERROR, "ai_response_text error\n");
	}
}

static void charge_callback(esl_socket_t server_sock, esl_socket_t client_sock, struct sockaddr_in *addr, void *user_data)
{
	esl_handle_t handle = {{0}};
	esl_status_t status;
	int hungup_reason = 2;

	if (fork()) 
	{ 
		close(client_sock); 
		return; 
	}
	
	esl_attach_handle(&handle, client_sock, addr);
	esl_log(ESL_LOG_INFO, "Connected! %d!\n", handle.sock);

	const char *unique_id = esl_event_get_header(handle.info_event, "Unique-ID");
	esl_log(ESL_LOG_INFO, "Session Unique-ID %s!\n", unique_id);

	esl_events(&handle, ESL_EVENT_TYPE_PLAIN, "CHANNEL_CREATE CHANNEL_HANGUP_COMPLETE CHANNEL_HANGUP CHANNEL_EXECUTE_COMPLETE CHANNEL_ANSWER CHANNEL_DESTROY PLAYBACK_START PLAYBACK_STOP CUSTOM ASR CALLTIMEOUT");
	esl_filter(&handle, "Unique-ID", unique_id);

	esl_send_recv(&handle, "linger 50");
	esl_execute(&handle, "answer", NULL, NULL);

	char record_file_path[1024] = {0};
	char record_file_pathname[1024] = {0};
	char record_www_pathname[1024] = {0};
	char sound_path[1024] = {0};
	char record_path[1024] = {0};
	char runtime_path[1024] = {0};
	char audio_root_path[1024] = {0};
	char audio_pathname[1024] = {0};


	//load record and soud profile
	GetProfileString(CONF_FILE_PATH, "fs" ,"record", record_path);
	GetProfileString(CONF_FILE_PATH, "fs" ,"sound", sound_path);
	sprintf(sound_path, "%s/%s", sound_path, esl_get_var(&handle, "AI-Sound"));
	GetProfileString(CONF_FILE_PATH, "fs" ,"runtime", runtime_path);
	GetProfileString(CONF_FILE_PATH, "fs" ,"downrecord", audio_root_path);
	esl_log(ESL_LOG_INFO, "record path: %s\n", record_path);
	esl_log(ESL_LOG_INFO, "sound path: %s\n", sound_path);
	esl_log(ESL_LOG_INFO, "runtime path: %s\n", runtime_path);
	esl_log(ESL_LOG_INFO, "audio root path: %s\n", audio_root_path);

	time_t now;
	struct tm *tm_now;
	time(&now);
	tm_now = localtime(&now);
	create_log_dic(record_path, tm_now);
	sprintf(record_file_path, "%s/record/%04d%02d%02d/", record_path,  tm_now->tm_year+1900, tm_now->tm_mon+1, tm_now->tm_mday);
	sprintf(record_file_pathname, "%s/%s", record_file_path, unique_id);
	sprintf(record_www_pathname, "./record/%04d%02d%02d/%s.wav", tm_now->tm_year+1900, tm_now->tm_mon+1, tm_now->tm_mday, unique_id);
	sprintf(audio_pathname, "%s/%04d%02d%02d/%s.pcm", audio_root_path,  tm_now->tm_year+1900, tm_now->tm_mon+1, tm_now->tm_mday, unique_id);
	esl_log(ESL_LOG_INFO, "record_file_path: %s!\n", record_file_path);
	esl_log(ESL_LOG_INFO, "record_file_pathname: %s!\n", record_file_pathname);
	esl_log(ESL_LOG_INFO, "record_www_pathname: %s!\n", record_www_pathname);
	esl_log(ESL_LOG_INFO, "audio_pathname: %s!\n", audio_pathname);

	//init ai
	esl_log(ESL_LOG_INFO, "ai_init start!\n");
	char ai_core[1024];
	char ai_env[1024];
	char sys_usr[32];
	char ignore_earlymedia[32];
	sprintf(ai_core, "%s/%s/libfilter.so", runtime_path, esl_get_var(&handle, "AI-Core"));
	sprintf(ai_env, "%s/%s/", runtime_path,  esl_get_var(&handle, "AI-Env"));
	sprintf(sys_usr, "%s", esl_get_var(&handle, "Sys-Usr"));
	sprintf(ignore_earlymedia, "%s", esl_get_var(&handle, "Ignore-Earlymedia"));
	esl_log(ESL_LOG_INFO, "JobId:%s AI-Core:%s!\n", unique_id, ai_core);
	esl_log(ESL_LOG_INFO, "JobId:%s AI-Env:%s!\n", unique_id, ai_env);
	esl_log(ESL_LOG_INFO, "JobId:%s Sys-Usr:%s!\n", unique_id, sys_usr);
	esl_log(ESL_LOG_INFO, "JobId:%s Ignore-Earlymedia:%s!\n", unique_id, ignore_earlymedia);
	//if( !ai_initialize("/usr/local/src/ai/libnlu.so", "/usr/local/src/ai/dataLibs/") )
	if( !ai_initialize(ai_core, ai_env) )
	{
		esl_log(ESL_LOG_ERROR, "ai_init error: %s!\n", ai_get_info());
		return;
	}
	esl_log(ESL_LOG_INFO, "ai_init end!Success!\n");

	//connect db
	char db_server_ip[32] = {0};
	GetProfileString(CONF_FILE_PATH, "mysql" ,"ip", db_server_ip);
	int db_server_port = 0; 
	GetProfileInt(CONF_FILE_PATH, "mysql", "port", &db_server_port);  
	char db_server_user[32] = {0};
	GetProfileString(CONF_FILE_PATH, "mysql", "user", db_server_user);  
	char db_server_pass[32] = {0};
	GetProfileString(CONF_FILE_PATH, "mysql", "password", db_server_pass);  
	char db_server_db[32] = {0};
	GetProfileString(CONF_FILE_PATH, "mysql",	"database", db_server_db);
	esl_log(ESL_LOG_INFO, "login db params: %s %d %s %s %s!\n", db_server_ip, db_server_port, db_server_user, db_server_pass, db_server_db);
	if(login_db(db_server_ip, db_server_port, db_server_user, db_server_pass, db_server_db))
	{
		esl_log(ESL_LOG_ERROR, "login db error: %s!\n", get_calllog_error());
		return;
	}
	else
	{
		esl_log(ESL_LOG_INFO, "login db OK!\n");
	}

	if( ASRWebapiProcessInit(unique_id, audio_pathname) )
	{
		goto callback_end;
	}

	if( !strcasecmp(ignore_earlymedia, "true") )
	{
		g_ignore_earlymedia = 1;
		g_session_status = SESSION_START;
		DialogStart(&handle, sound_path, record_file_pathname, record_www_pathname );
	}
	else
	{
		g_session_status = SESSION_WAIT_ANSWER;
	}
	
	int nBridge = 0;
	int nDisconnect = 0;
	char cEvalue = 'C';
	while(1)
	{
		status = esl_recv_timed(&handle, 100);
		if(  status != ESL_SUCCESS)
		{
			event_callback(&handle, NULL, sound_path, record_file_pathname, record_www_pathname, &nDisconnect);
		}
		else
		{
			if( handle.last_event )
			{
				const char *application = esl_event_get_header(handle.last_event, "Application");
				const char *event_type = esl_event_get_header(handle.last_event, "content-type");
				const char *event_name = esl_event_get_header(handle.last_event, "Event-Name");
				const char *event_subclass = esl_event_get_header(handle.last_event, "Event-Subclass");

				esl_log(ESL_LOG_INFO, "JobID:%s last_event Application: %s event_type: %s event_name: %s event_subclass: %s event_id: %d \n", unique_id, application, event_type, event_name, event_subclass, handle.last_event->event_id);

				if (event_type)
				{
					if(!strcasecmp(event_type, "text/event-plain") && handle.last_ievent ) 
					{		
						event_callback(&handle, handle.last_ievent, sound_path, record_file_pathname, record_www_pathname, &nDisconnect);
						if( nDisconnect )
						{
							g_session_status = SESSION_STOP;
							goto callback_end;
						}

					}	
					else if(!strcasecmp(event_type, "text/disconnect-notice"))
					{
						g_session_status = SESSION_STOP;
						goto callback_end;
					}
				}
			}
		}
	}

callback_end:
	cEvalue = evaluate_end();
	esl_log(ESL_LOG_INFO, "EValuate count:%d time:%ld match:%d Level %c!\n", chat_count, GetCostTime(), get_match_key_word(),  cEvalue);

	if( nDisconnect == 1 )
		hungup_reason = 1;
	set_chat_hungup_info(esl_event_get_header(handle.info_event, "unique-id"), hungup_reason, GetCostTime(), cEvalue);

	sleep(5);

	ai_uninit();

	esl_log(ESL_LOG_INFO, "Disconnected! %d\n", handle.sock);

	esl_record_stop(&handle, record_file_pathname);

	esl_stop_ASR(&handle,  unique_id);

	char s_uniqueid[128] = {0};
	strcpy(s_uniqueid, unique_id);
	esl_disconnect(&handle);	

	esl_log(ESL_LOG_INFO, "unique_id: %s!\n", s_uniqueid);

	esl_log(ESL_LOG_INFO, "make record %s:%s!\n", record_file_path, s_uniqueid);
	char *argv[] = {"/usr/local/src/makerecord.sh", record_file_path, s_uniqueid, NULL};
	char *envVec[] = {NULL};  
	if( -1 == execve("/usr/local/src/makerecord.sh", argv, envVec) )
	{
		esl_log(ESL_LOG_INFO, "make record error:%s!\n", strerror(errno));
	}

	exit(0);
}

void signal_handler(int signo)
{
	if (signo == SIGCHLD) 
	{
		pid_t pid;
		while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
			printf("SIGCHLD pid %d\n", pid);
			esl_log(ESL_LOG_INFO, "SIGCHLD pid %d\n", pid);
		}
	}
}

int main(void)
{
	esl_global_set_default_logger(ESL_LOG_LEVEL_INFO);

	signal(SIGCHLD, signal_handler);

	esl_listen_threaded("localhost", 8040, charge_callback, NULL, 100000);

	return 0;
}


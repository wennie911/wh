#include "calllog.h"

#include <mysql/mysql.h>
#include <string.h>
#include <unistd.h>  
#include <fcntl.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <stdarg.h>  
#include <ctype.h>
#include <time.h>
#include <esl.h>

#define KEYVALLEN 256  

MYSQL mysql;
int flag_login_db = 0;

/*
[mysql]ip = 10.50.10.97port = 3306user = rootpassword = Freeswitch,2018database = callingsystem
*/
int login_db(char *db_server_ip, int db_server_port, char *db_server_user, char *db_server_pass, char *db_server_db )
{
  MYSQL_RES *result = NULL;

  mysql_init(&mysql);
  if( !mysql_real_connect(&mysql, db_server_ip, db_server_user, db_server_pass, db_server_db, db_server_port, NULL, 0) )
  {
	 return -1;
  }

  flag_login_db = 1;
  return 0;
}

void logout_db()
{	  
	mysql_close(&mysql);
}

int add_record_info(char *jobUUID, char *log_filepathname)
{
	if( !flag_login_db )
		return -2;

	char sql[1024];	
	sprintf(sql, "UPDATE T_CALLING_LOG SET F_RECORD = \'%s'\ WHERE F_JOB_ID = \'%s\'", log_filepathname, jobUUID);

	if( mysql_real_query(&mysql, sql, strlen(sql)) )
	{
		return -1; 
	}

	return 0;
}

int set_record_level(char *jobUUID, char level)
{
	if( !flag_login_db )
		return -2;

	char sql[1024];	
	sprintf(sql, "UPDATE T_CALLING_LOG SET F_EVALUEATE = \'%c'\ WHERE F_JOB_ID = \'%s\'", level, jobUUID);

	if( mysql_real_query(&mysql, sql, strlen(sql)) )
	{
		return -1; 
	}

	return 0;
}

int add_chat_log(char *jobUUID, char *question, char *answer, const char *datetime)
{
	if( !flag_login_db )
		return -2;

	char sql[1024];
	sprintf(sql, "INSERT INTO T_LOG_INFO (F_JOB_ID, F_QUESTION, F_ANSWER, F_DATETIME) VALUES (\'%s\', \'%s\', \'%s\', \'%s\')", jobUUID, question, answer, datetime);  

	esl_log(ESL_LOG_INFO, "add_chat_log sql:%s\n", sql);

	mysql_query(&mysql, "SET NAMES utf8"); 

	if( mysql_real_query(&mysql, sql, strlen(sql)) )
	{
		return -1; 
	}

	return 0;
}

int update_log_connectd(char *jobUUID)
{
	if( !flag_login_db )
		return -2;

	char sql[1024];	
	sprintf(sql, "UPDATE T_CALLING_LOG SET F_OUTBOUND = 1  WHERE F_JOB_ID = \'%s\'", jobUUID);

	if( mysql_real_query(&mysql, sql, strlen(sql)) )
	{
		return -1; 
	}

	return 0;
}


const char *get_calllog_error()
{
	return mysql_error(&mysql);
}

void get_datetime(char *sDateime)
{
	/*
	char *wday[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	time_t timep;
	struct tm *p;
	time(&timep);
	p = gmtime(&timep);
	*/

	time_t now ;
	struct tm *tm_now ;
	time(&now) ;
	tm_now = localtime(&now) ;

	sprintf(sDateime, "%d-%d-%d %02d:%02d:%02d", (1900+tm_now->tm_year), (1+tm_now->tm_mon), tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
}
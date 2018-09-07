#include <string.h>
#include <stdio.h>
#include <time.h>
#include <esl.h>

#include "nluProcess.h"

char keyword_list[1000][64] = {0};
char keyword_list_pingyin[1000][64] = {0};

struct timeval t_start;
struct timeval t_end;
struct timespec tpstart;
struct timespec tpend;
time_t time_use = 0L;
int  if_match_keyword = 0;
int  if_match_keyword_pingyin = 0;
extern int g_customer_talk_count;
struRetAnswer retAnswer;

int match_keyword(char *words)
{
	if(if_match_keyword)
		return 1;

	if( words == NULL || strlen(words) == 0 )
		return 0;
	int i = 0;
	for( i = 0 ; i < 12; ++i )
	{
		if( NULL != strstr(words, keyword_list[i]) 	)
		{
			if_match_keyword = 1;
			return 1;
		}
	}

	return 0;
}

int match_keyword_pingyin(char *pingyin)
{
	if( if_match_keyword_pingyin )
		return 1;

	if( pingyin == NULL || strlen(pingyin) == 0 )
		return 0;

	int i = 0;
	for( i = 0 ; i < 12; ++i )
	{
		if( NULL != strstr(pingyin, keyword_list_pingyin[i]) 	)
		{
			if_match_keyword_pingyin = 1;
			return 1;
		}
	}

	return 0;
}

void evaluate_start()
{
	gettimeofday(&t_start, NULL);
	clock_gettime(CLOCK_MONOTONIC, &tpstart);

	esl_log(ESL_LOG_INFO, "evaluate_start tv_sec:%d tv_usec:%d!\n", t_start.tv_sec, t_start.tv_usec );
}

char evaluate_end()
{
	gettimeofday(&t_end, NULL);
	clock_gettime(CLOCK_MONOTONIC, &tpend);

	esl_log(ESL_LOG_INFO, "evaluate_end tv_sec:%d tv_usec:%d\n!", t_end.tv_sec, t_end.tv_usec );

	time_use = t_end.tv_sec - t_start.tv_sec;
	//time_use = tpend.tv_sec - tpstart.tv_sec;

	esl_log(ESL_LOG_INFO, "evaluate_end get_match_key_word:%d\n", get_match_key_word());
	
	if( get_match_key_word())

	{
		return 'A';
	}
	else
	
	{
		return getSessionStat(&g_retAnswer);
		/*if( isBusyOfDialouge(&g_sessionSlot) )
		{
			return 'E';
		}	

		if( time_use >= 120 && getValidOfDialouge(&g_sessionSlot) > 2 )
		{
			return 'B';
		}
		
			
			
		if( g_customer_talk_count <= 0 || isRefusedOfDialouge(&g_sessionSlot) )
		
		{
			
			return 'D';
		
		}
		
		if( (getValidOfDialouge(&g_sessionSlot) < 3) &&  !isRefusedOfDialouge(&g_sessionSlot))
		{
			return 'C';
		}*/

	}

	return 'D';
}

time_t GetCostTime()
{
	return time_use;
}

int get_match_key_word()
{
	return (if_match_keyword == 1 || if_match_keyword_pingyin == 1);
}
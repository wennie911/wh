#include <string.h>
#include <stdio.h>
#include <sys/time.h>

#include "nluProcess.h"

char keyword_list[12][32] = {"�", "�Ż�", "���", "����", "���", "��Ϣ", "������", "�����", "����", "����", "���", "�¿�"};
char keyword_list_pingyin[12][32] = {"huo|dong", "you|hui", "e|du", "ban|li", "nian|fei", "li|xi", "shou|xu|fei", "fu|wu|fei", "zi|liao", "liu|chen", "shen|he", "xia|ka"};

struct timeval t_start;
struct timeval t_end;
int time_use = 0L;
int  if_match_keyword = 0;
int  if_match_keyword_pingyin = 0;

extern sessionSlotStru g_sessionSlot;

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
}

char evaluate_end()
{
	gettimeofday(&t_end, NULL);

	time_use = (t_end.tv_sec-t_start.tv_sec)*1000000 + (t_end.tv_usec-t_start.tv_usec);//΢��

	//���йؼ��� �� ��������
	if( (if_match_keyword || if_match_keyword_pingyin || g_sessionSlot.bFlowEndCGFlag || g_sessionSlot.bFlowEndSBFlag ) &&
		!isRefusedOfDialouge(&g_sessionSlot) &&		//û�ܾ�
		!isBusyOfDialouge(&g_sessionSlot))			//û��æ
	{
		return 'A';
	}
	else
	{
		if( time_use >= 120000000 && getValidOfDialouge(&g_sessionSlot) > 2 )
		{
			return 'B';
		}
		
		if( (getValidOfDialouge(&g_sessionSlot) < 3) &&  !isRefusedOfDialouge(&g_sessionSlot))
		{
			return 'C';
		}

		if( (getValidOfDialouge(&g_sessionSlot) < 1) || isRefusedOfDialouge(&g_sessionSlot) )
		{
			return 'D';
		}
	}

	return 'E';
}

float GetCostTime()
{
	return time_use;
}

int get_match_key_word()
{
	return (if_match_keyword == 1 || if_match_keyword_pingyin == 1);
}
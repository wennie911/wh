#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "ai.h"
#include "nluProcess.h"

#include "calllog.h"



//处理nlu 返回nlu的处理结果
int processNLU(char *sAsrResponse, struRetAnswer *retAnswerPtr)
{
	if(strlen(sAsrResponse)==0)
		return 0;


	char words[MAXANSWERLEN*2]={0};
	
	char answer_json[MAXANSWERLEN]={0};

	//将sAsrResponse(json)解析到words
    //dojsonlist(sAsrResponse, words);
	//if(strlen(words)<=0)
	//	return 0;

	//调用nul 获得回答的结构
	return ai_get_answer(sAsrResponse, retAnswerPtr);

}

//A 成功 B 有意愿 C兴趣不大 D明确拒绝 E当前忙碌
char getSessionStat(struRetAnswer *retAnswerPtr)
{
	if(strcmp(retAnswerPtr->sType,"A")==0)
	{
		return 'A';
	}
	if(strcmp(retAnswerPtr->sType,"B")==0)
	{
		return 'D';
	}
	if(strcmp(retAnswerPtr->sType,"I")==0)
	{
		return 'E';
	}
	if(retAnswerPtr->iDialogeCount>3 )
	{
		return 'B';
	}
	if(retAnswerPtr->iDialogeCount<=3 || retAnswerPtr->iCurrentLayer<3)
	{
		return 'C';
	}
	
	if(retAnswerPtr->iCurrentLayer == retAnswerPtr->iTotalLayer)
	{
		return 'B';
	}
	return 'B';
}

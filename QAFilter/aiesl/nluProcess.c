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
	dojsonlist(sAsrResponse, words);
	if(strlen(words)<=0)
		return 0;

	//调用nul 获得回答的结构
	return ai_get_answer(words, retAnswerPtr);

}

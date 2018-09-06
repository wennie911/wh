#include "QAFilterInterface.h"
#include "filterProcess.h"



CFilterProcess g_cFilterProcess;

int qaf_init(const char *sDllDir)
{
	string sDT;
	sDT = sDllDir;
	return g_cFilterProcess.init(sDT);
	
}

//输入question 返回answer和answer的声音文件
int qaf_get_answer(const char *in_sentence, char *out_sentence, int out_sentence_size)
{
	string sInSentence="";
	string sOutSentence="";
	if(strlen(in_sentence)==0)
		return 0;
	sInSentence.append(in_sentence);
	printf("sInsentence=%s", in_sentence);
	if(g_cFilterProcess.processInSentence(sInSentence, sOutSentence))
	{
		memset(out_sentence, 0, out_sentence_size);
		
		strncpy(out_sentence, sOutSentence.c_str(), out_sentence_size);

		//printf("out_sentence=%s\n", out_sentence);
		return 1;
	}
	return 0;
}

//相关的状态接口
	
//有效会话次数
int getValidSession()
{
}
//是否有聊天
int isChatInSession()
{
}
//是否有咨询
int isConsultedInSession()
{
}
//是否挽回过
int isRedeemed()
{
}
//是否拒绝
int isRefused()
{
}
//是否在忙
int isBusy()
{
}
//是否成功
int isSuccesed()
{
}


#include "QAFilterInterface.h"
#include "filterProcess.h"



CFilterProcess g_cFilterProcess;

int qaf_init(const char *sDllDir)
{
	string sDT;
	sDT = sDllDir;
	return g_cFilterProcess.init(sDT);
	
}

//����question ����answer��answer�������ļ�
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

//��ص�״̬�ӿ�
	
//��Ч�Ự����
int getValidSession()
{
}
//�Ƿ�������
int isChatInSession()
{
}
//�Ƿ�����ѯ
int isConsultedInSession()
{
}
//�Ƿ���ع�
int isRedeemed()
{
}
//�Ƿ�ܾ�
int isRefused()
{
}
//�Ƿ���æ
int isBusy()
{
}
//�Ƿ�ɹ�
int isSuccesed()
{
}


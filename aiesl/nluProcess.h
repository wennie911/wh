#ifndef NLUPROCESS_H_  
#define NLUPROCESS_H_  


/*�½�һ���Ự,�����ַ�������״̬
zcq
20180723
*/
#define MAXANSWERLEN 512 //���answer����

typedef struct _struRetAnswer
{
	int iIsEnd;
	int iIsBreak;
	int iCurrentLayer;
	int iTotalLayer;
	int iDialogeCount;
	char sQuestion[MAXANSWERLEN*1];
	char sAnswer[MAXANSWERLEN*4];
	char sMatchedQuestion[MAXANSWERLEN*2];
	char sAudio[MAXANSWERLEN];
	char sType[MAXANSWERLEN];
	char sQuestionPinYin[MAXANSWERLEN*2];
}struRetAnswer;

//����nlu ����nlu�Ĵ�����
int processNLU(char *sAsrResponse, struRetAnswer *retAnswerPtr);
char getSessionStat(struRetAnswer *retAnswerPtr);

#endif
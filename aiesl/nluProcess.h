#ifndef NLUPROCESS_H_  
#define NLUPROCESS_H_  


/*新建一个会话,返回字符串，及状态
zcq
20180723
*/
#define MAXANSWERLEN 512 //最大answer长度

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

//处理nlu 返回nlu的处理结果
int processNLU(char *sAsrResponse, struRetAnswer *retAnswerPtr);
char getSessionStat(struRetAnswer *retAnswerPtr);

#endif
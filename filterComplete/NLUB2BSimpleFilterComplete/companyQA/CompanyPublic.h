#pragma once
#include <string>
#include <map>
#include <vector>

#include "../config/ra_nlu_brain.h"

using namespace std;

typedef	enum _COMPANY_WORD_TYPE
{
	COMPANY_WORD_NOUN =1,
	COMPANY_WORD_VERB ,
	COMPANY_WORD_ADJ ,
	COMPANY_WORD_QUAN ,
	COMPANY_WORD_PRONOUN,
	COMPANY_WORD_PREPOSITION ,
	COMPANY_WORD_ADVERB,
	COMPANY_WORD_CONJUNCTION =9,
	COMPANY_WORD_EN=99
	
}WORD_TYPE;


typedef struct _struSentence
{
	int iId;
	string sSentence;
}struSentence;

typedef struct _struWordList
{
	int iWordType;//���Դ���
	int iWordProperty; //����
	bool bUsedFlag;//��ʹ�ù���
	bool bValidFlag;//��Ч��
	vector<string> vecSynonymWords;
}struWordList;

typedef struct _struMediaUrl
{
	string sAudio;
	string sType;
	string sIsEnd;
	string sIsBreak;
	string sLoop;
	string sAffirm;
	string sQuestionType;
}struMediaUrl;

//һ��������qa
typedef struct _struFullQA
{
	string sQuestion;
	string sAnswer;
	string sMatchedQuestion;
	struMediaUrl media;
}struFullQA;

typedef struct _struLTPWord
{
	int iWordType;//�ʼ��� 0,1,2
	string sWord;
	bool bMatchingFlag;//ƥ��ı�־
}struLTPWordList;

typedef struct _struQuestionList
{  
	int iId; 
	int  iFrequency;
	string sSentence;  
	string sAnswer;
	struMediaUrl mu;
}struQuestionList;  


typedef struct _struKeyWord
{
	string sWord;
	string sPinYin;
	string sFirstPY;//��ĸ�б�
	string sLastPY;//��ĸ�б�
	string sMixtruePY;//����б�
	vector<string> vecPinYin;
	vector<string> vecFirstPY;
	vector<string> vecLastPY;//��ĸ�б�
	vector<string> vecMixture;//��ĸ��ĸ�Ļ���б�
}struKeyWord;

class CCompanyPublic
{
	public:
		CCompanyPublic();
		~CCompanyPublic();
		map<string,string> m_mapCommandCn; //������ĵ��ַ���
		map<string,string> m_mapNormalCn; //������ĵ��ַ���

		string getNewString(const string sEn, int iType);
		string formatNewString(const string originalStr, const string insertStr ); //��Ҫ����Ĵ����̶���ʽ����ԭ��
		string formatNewString(const string originalStr, int iParamCount, const char *str, ...);
		string formatNewStringUseOfInterface(const string originalStr, int iParamCount, const char *str, ...);

		//�Ѿ����б�ƴװ��json
		string makeSenenceJson(vector<struFullQA> &vecQA);
					
		string trimRN(string sInputBase64Str);
		string makeBase64(string sJson);
		string makeLog(string sInput, string sAnswer, vector<struQuestionList> vecSentenceList, string sContentype, int iType, int iSelectIndex);

		//ƴ��ƥ��
		bool matchPinYin(string &sSentence, vector<struKeyWord> &vecKeyWords, string &sMatchWord);
		string getApproximatelyPinYin(string sSrcPY);
		int getPYDistance(vector<string> &vecSrc, vector<string> &vecDes);
		float matchingRateForPinYin(string sInFirstPY, string sFirstPY);
	private:	
		bool initCommandString();
		string getPinYin(string sSentence);
		
};

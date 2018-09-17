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
	int iWordType;//词性大类
	int iWordProperty; //词性
	bool bUsedFlag;//被使用过了
	bool bValidFlag;//有效的
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

//一个完整的qa
typedef struct _struFullQA
{
	string sQuestion;
	string sAnswer;
	string sMatchedQuestion;
	struMediaUrl media;
}struFullQA;

typedef struct _struLTPWord
{
	int iWordType;//词级别 0,1,2
	string sWord;
	bool bMatchingFlag;//匹配的标志
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
	string sFirstPY;//声母列表
	string sLastPY;//韵母列表
	string sMixtruePY;//混合列表
	vector<string> vecPinYin;
	vector<string> vecFirstPY;
	vector<string> vecLastPY;//韵母列表
	vector<string> vecMixture;//声母韵母的混合列表
}struKeyWord;

class CCompanyPublic
{
	public:
		CCompanyPublic();
		~CCompanyPublic();
		map<string,string> m_mapCommandCn; //存放中文的字符串
		map<string,string> m_mapNormalCn; //存放中文的字符串

		string getNewString(const string sEn, int iType);
		string formatNewString(const string originalStr, const string insertStr ); //将要插入的串按固定格式插入原串
		string formatNewString(const string originalStr, int iParamCount, const char *str, ...);
		string formatNewStringUseOfInterface(const string originalStr, int iParamCount, const char *str, ...);

		//把句子列表拼装成json
		string makeSenenceJson(vector<struFullQA> &vecQA);
					
		string trimRN(string sInputBase64Str);
		string makeBase64(string sJson);
		string makeLog(string sInput, string sAnswer, vector<struQuestionList> vecSentenceList, string sContentype, int iType, int iSelectIndex);

		//拼音匹配
		bool matchPinYin(string &sSentence, vector<struKeyWord> &vecKeyWords, string &sMatchWord);
		string getApproximatelyPinYin(string sSrcPY);
		int getPYDistance(vector<string> &vecSrc, vector<string> &vecDes);
		float matchingRateForPinYin(string sInFirstPY, string sFirstPY);
	private:	
		bool initCommandString();
		string getPinYin(string sSentence);
		
};

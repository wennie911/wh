#pragma once

#include "CompanyOnlyQA.h"
#include "CompanyQuestionIdList.h"
#include "CompanyPublic.h"
#include "../game/GamePublic.h"


static vector<struWordList> m_vecWordsListLastQA;//上一次输入被保存的词



class CCompanyQANormal
{
	public:
		CCompanyQANormal();
		~CCompanyQANormal();
		
		void initNA(vector<struWordList> vecWordsList, int iTone, void *vPtr, void *vApplyLocatorPt);
		
		bool process(string & retStr);
		bool getAnswer(string & retStr);

		bool setQuestionFrequency(int iQuestionId);
		string getAnswerByQuestionId(int iQuestionId);
		
		int compareQuestion(string sQuestion);

		bool getGroupParam(int iQuestionId, struMediaUrl &medialUrl);

		void setShortSentenceFlag(bool bShortSentenceFlag){m_bShortSentenceFlag = bShortSentenceFlag;}

		
	private:
		vector<struWordList> m_vecWordsList;//输入的词
		CCompanyOnlyQA m_QADB;
		CCompanyQuestionIdList m_questionIdList;
		map<int,string>  m_questionListAllSelect;
		vector<struSentence> m_questionList;
		CCompanyPublic m_cp;//字符转换、提取
		
		int m_iTone;
		void *m_vPtr;
		void *m_applyLocatorPtr;
		string m_sInSentence;//输入的句子
		bool m_bShortSentenceFlag;//输入短句标识
		int m_iPriority;//当前优先级
		vector<string> m_vecNameWords;//存放人名
		vector<string> m_vecDateWords;//存放时间
		vector<vector<int> > m_vecSentenceIndex;
		CGamePublic m_GamePublic;
		int m_iSentenceType;
		
		
		string getAnswerFromId();
		bool getQuestionId();

		float getMatchCount(string &sentence);
		bool processMultiQuestion(string &sRetStr);
		bool getFirstPriority(vector<int> &vecCurrentPriorityWordsIdx);//得到第一个优先级的词列表索引
		bool getNextPriority(vector<int> &vecCurrentPriorityWordsIdx);//得到下一个优先级的词列表索引
		bool getFirstPriorityLastQA(vector<int> &vecCurrentPriorityWordsIdx);
		bool getNextPriorityLastQA(vector<int> &vecCurrentPriorityWordsIdx);
		void saveCurrentWordList();
		float getMatchCountForLTP(string &sSentence);

		bool getDateWords();//把m_vecWordsList里的:姓名、姓、日期、数量词取出来
		bool getNameWords();

		bool getInSentenceIndex();
		bool removeOtherWords();
		bool splitChNumber(const string sSrcStr, string &sNum, string&sOther);
		int getSentenceBayesType(vector<struWordList> &vecWordList);


};


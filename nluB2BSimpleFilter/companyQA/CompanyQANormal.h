#pragma once

#include "CompanyOnlyQA.h"
#include "CompanyQuestionIdList.h"
#include "CompanyPublic.h"
#include "../game/GamePublic.h"


static vector<struWordList> m_vecWordsListLastQA;//��һ�����뱻����Ĵ�



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
		vector<struWordList> m_vecWordsList;//����Ĵ�
		CCompanyOnlyQA m_QADB;
		CCompanyQuestionIdList m_questionIdList;
		map<int,string>  m_questionListAllSelect;
		vector<struSentence> m_questionList;
		CCompanyPublic m_cp;//�ַ�ת������ȡ
		
		int m_iTone;
		void *m_vPtr;
		void *m_applyLocatorPtr;
		string m_sInSentence;//����ľ���
		bool m_bShortSentenceFlag;//����̾��ʶ
		int m_iPriority;//��ǰ���ȼ�
		vector<string> m_vecNameWords;//�������
		vector<string> m_vecDateWords;//���ʱ��
		vector<vector<int> > m_vecSentenceIndex;
		CGamePublic m_GamePublic;
		int m_iSentenceType;
		
		
		string getAnswerFromId();
		bool getQuestionId();

		float getMatchCount(string &sentence);
		bool processMultiQuestion(string &sRetStr);
		bool getFirstPriority(vector<int> &vecCurrentPriorityWordsIdx);//�õ���һ�����ȼ��Ĵ��б�����
		bool getNextPriority(vector<int> &vecCurrentPriorityWordsIdx);//�õ���һ�����ȼ��Ĵ��б�����
		bool getFirstPriorityLastQA(vector<int> &vecCurrentPriorityWordsIdx);
		bool getNextPriorityLastQA(vector<int> &vecCurrentPriorityWordsIdx);
		void saveCurrentWordList();
		float getMatchCountForLTP(string &sSentence);

		bool getDateWords();//��m_vecWordsList���:�������ա����ڡ�������ȡ����
		bool getNameWords();

		bool getInSentenceIndex();
		bool removeOtherWords();
		bool splitChNumber(const string sSrcStr, string &sNum, string&sOther);
		int getSentenceBayesType(vector<struWordList> &vecWordList);


};


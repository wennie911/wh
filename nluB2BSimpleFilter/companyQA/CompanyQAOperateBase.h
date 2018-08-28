#pragma once

/*
���ݿ�Ӧ�õĻ���
����һЩ�̶��Ĳ���
*/

#include "../datalibs_management/CompanyDB.h"

#include "CompanyPublic.h"


#define COMPANY_COMMAND 1
#define COMPANY_NORMAL 2


class CCompanyQAOperateBase
{
	public:
		CCompanyQAOperateBase();
		~CCompanyQAOperateBase();
	public:
		bool fristQuestionIdList(vector<string> &vecKeyList, vector<int> &vecIdList);
		bool questionIdList(vector<string> &vecKeyList, vector<int> &vecRangeIdList, vector<int> &vecIdList);

		bool getGroupId(int iQuestionId, int &iGroupId);
		bool getGroupParamByQuestionId(int iQuestionId, vector<string> &vecGroupParam);
		bool getAnswerByQuestionId(int iQuestionId, string &sAnswerStr);
		bool getQuestionStrByQuestionId(int iQuestionId, string &sQuestionStr);
		bool getAllQuestionByGroupId(int iQuestionId, vector<struSentence> &vecQuestionList);
		
		bool getCommand(map < string,string > & mapCommandCn);
		bool getNormal(map < string,string > & mapNormalCn);

		bool  getCompanyQuestionFrequency(int iQuestionId, int &iFrequercy);
		bool  setCompanyQuestionFrequency(int iQuestionId);

		bool getMinIdByGroup(vector<int> &vecRetIdList);
		bool getDefaultAnswer(string &sDefaultAnswer, int iType=0);
		bool checkPrimaryWord(string &sInWord);

		bool getAnswerByMatchQuestion(string &sQuestionStr, int &iQuestionId);

				
		//�õ���ǰ�������
		bool getYearWeekByLocalDay(int &iYear, int &iWeek);

		
		//�õ���Ҷ˹���
		bool getBayesTrainedData(map<string,string> &mapBayesData, string &sModule);
		//�õ���Ҫ�Ĵʺ�ƴ��
		bool getPrimaryWord(vector<struKeyWord> &vecKeyWords);
	private:
		CCompanyDB *m_CCompanyDBPtr;
		CCompanyFrequencyDB * m_FrequencyDBPtr;
		
};

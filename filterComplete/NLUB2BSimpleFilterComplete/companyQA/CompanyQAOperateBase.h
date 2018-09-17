#pragma once

/*
数据库应用的基类
用于一些固定的操作
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

				
		//得到当前的年的周
		bool getYearWeekByLocalDay(int &iYear, int &iWeek);

		
		//得到贝叶斯相关
		bool getBayesTrainedData(map<string,string> &mapBayesData, string &sModule);
		//得到主要的词和拼音
		bool getPrimaryWord(vector<struKeyWord> &vecKeyWords);
	private:
		CCompanyDB *m_CCompanyDBPtr;
		CCompanyFrequencyDB * m_FrequencyDBPtr;
		
};

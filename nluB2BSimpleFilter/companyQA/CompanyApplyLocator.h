#pragma once

//zcq 20171115 企业用户qa的入口
#include "CompanyWordsOperate.h"
#include "../dialogue_management/ContentLocatorBase.h"
#include "../baisic_info/InSematicInfos.h"
#include "../baisic_info//CellInfoList.h"
#include "CompanyPublic.h"
#include "CompanyOnlyQA.h"
#include "CompanyQANormal.h"

#include "CompanyBayes.h"

#include "../json/json_reader.h"
#include "../json/json_writer.h"



class CCompanyApplyLocator:public CContentLocatorBase
{
	public:
		CCompanyApplyLocator();
		virtual ~CCompanyApplyLocator();
	public:
		CCompanyBayes *m_BayesPtr;
		
		bool ContentLocate(CINSemanticInfos * pSemantic,CCellInfoList * pCellInfoList);
		bool ContentLocateActivity(CINSemanticInfos * pSemantic,CCellInfoList * pCellInfoList);
		bool ContentLocateCompare(CINSemanticInfos * pSemantic,CCellInfoList * pCellInfoList);
		bool CommandLocate(CINSemanticInfos *inpSemantic, CCellInfoList *outCellList);
		bool HistoryLocate(CINSemanticInfos *inpSemantic, CCellInfoList *outCellList);

		bool setQuestionList(vector<struQuestionList> &vecQuestionList);
			
		int parseJson(string sJson, string &sRet);

		string getAnswerStr(int iSelectedId);
	private:
		CCellInfoList *m_pCellInfoList; //输出
		CINSemanticInfos *m_pSemantic; //输入
		CSegmantation * m_pSemanticPrivate;//私有使用
		
		CCompanyWordsOperate m_wordsOperate;
		vector<struQuestionList> m_vecQuestionList;
		vector<struKeyWord> m_vecKeyWordsList;

		CCompanyOnlyQA m_QADB;
		CCompanyPublic m_public;
		
		

		map<int, void*> m_mapQAIndex;//存放不同处理流程的指针
		int m_iCurrentModule; //当前启用模块 缺省-1;
		int m_icurrentApp;//当前正在使用的APP

		string m_sModule;
		string m_sEvent;
		
		int selectModule();
		string getModuleStr(int iSelectModule);
		void setModule(string sModule, string &sRet);
		int getModuleIdx(string sModule);

		void setAppIdx(string sAppName, string &sRet);
		int getCurrentAppIdx();
		
		bool procKeyWords(string &sRet);//句子里有标识的关键词时，把关键词返回

		
		bool getSpecialAnswer(vector<string> &vecParam, string &sRet);
		
		bool makeSentence(string &sRet);
		void Init();

	
		
};

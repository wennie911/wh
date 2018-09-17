#pragma once

//zcq 20171115 ��ҵ�û�qa�����
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
		CCellInfoList *m_pCellInfoList; //���
		CINSemanticInfos *m_pSemantic; //����
		CSegmantation * m_pSemanticPrivate;//˽��ʹ��
		
		CCompanyWordsOperate m_wordsOperate;
		vector<struQuestionList> m_vecQuestionList;
		vector<struKeyWord> m_vecKeyWordsList;

		CCompanyOnlyQA m_QADB;
		CCompanyPublic m_public;
		
		

		map<int, void*> m_mapQAIndex;//��Ų�ͬ�������̵�ָ��
		int m_iCurrentModule; //��ǰ����ģ�� ȱʡ-1;
		int m_icurrentApp;//��ǰ����ʹ�õ�APP

		string m_sModule;
		string m_sEvent;
		
		int selectModule();
		string getModuleStr(int iSelectModule);
		void setModule(string sModule, string &sRet);
		int getModuleIdx(string sModule);

		void setAppIdx(string sAppName, string &sRet);
		int getCurrentAppIdx();
		
		bool procKeyWords(string &sRet);//�������б�ʶ�Ĺؼ���ʱ���ѹؼ��ʷ���

		
		bool getSpecialAnswer(vector<string> &vecParam, string &sRet);
		
		bool makeSentence(string &sRet);
		void Init();

	
		
};

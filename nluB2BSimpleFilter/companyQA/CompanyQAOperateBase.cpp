#include "CompanyQAOperateBase.h"
#include "../AICC/KGPublic.h"

CCompanyQAOperateBase::CCompanyQAOperateBase()
{
	m_CCompanyDBPtr = CCompanyDB::GetInstance();
	m_FrequencyDBPtr = CCompanyFrequencyDB::GetInstance();
}

CCompanyQAOperateBase::~CCompanyQAOperateBase()
{
}

bool CCompanyQAOperateBase::fristQuestionIdList(vector<string> &vecKeyList, vector<int> &vecIdList)
{
	if(vecKeyList.empty())
		return false;
	if(vecKeyList.size()==1 && vecKeyList.at(0).empty())
		return false;
	return m_CCompanyDBPtr->getQuestionId(vecKeyList, vecIdList);
}

bool CCompanyQAOperateBase::questionIdList(vector<string> &vecKeyList, vector<int> &vecRangeIdList, vector<int> &vecIdList)
{
	if(vecKeyList.empty())
		return false;
	if(vecKeyList.size()==1 && vecKeyList.at(0).empty())
		return false;
	return m_CCompanyDBPtr->getQuestionIdRangeIdList(vecKeyList, vecRangeIdList, vecIdList);
}

bool CCompanyQAOperateBase::getGroupParamByQuestionId(int iQuestionId, vector<string> &vecGroupParam)
{
	//得到group id
	int iGroupId=0;
	vecGroupParam.clear();
	
	if(!m_CCompanyDBPtr->getGroupId(iQuestionId,iGroupId))
	{
		return false;
	}

	if(!m_CCompanyDBPtr->getGroupParam(iGroupId, vecGroupParam))
	{
		return false;
	}
	
	return true;
}

bool CCompanyQAOperateBase::getAnswerByQuestionId(int iQuestionId,  string &sAnswerStr)
{
	//得到group id
	int iGroupId=0;
	sAnswerStr="";
	if(!m_CCompanyDBPtr->getGroupId(iQuestionId,iGroupId))
	{
		return false;
	}

	if(!m_CCompanyDBPtr->getAnswerByGroup(iGroupId, sAnswerStr))
	{
		return false;	
	}
	
	return true;
}

bool CCompanyQAOperateBase::getAnswerByMatchQuestion(string &sQuestionStr, int &iQuestionId)
{
	iQuestionId=0;
	if(!m_CCompanyDBPtr->GetMatchQuestion(iQuestionId, sQuestionStr))
	{
		return false;
	}
	return true;
}


bool CCompanyQAOperateBase::getQuestionStrByQuestionId(int iQuestionId, string &sQuestionStr)
{
	sQuestionStr="";
	if(!m_CCompanyDBPtr->getQuestionStrByQuestionId(iQuestionId, sQuestionStr))
	{
		return false;
	}

	return true;
}

bool CCompanyQAOperateBase::getAllQuestionByGroupId(int iQuestionId, vector<struSentence> &vecQuestionList)
{
	//得到group id
	int iGroupId=0;
	vecQuestionList.clear();
	
	if(!m_CCompanyDBPtr->getGroupId(iQuestionId,iGroupId))
	{
		return false;
	}

	if(!m_CCompanyDBPtr->getAllQuestionByGroupId(iGroupId, vecQuestionList))
	{
		return false;
	}
	return true;
}

bool  CCompanyQAOperateBase::setCompanyQuestionFrequency(int iQuestionId)
{
	//得到group id
	int iGroupId=0;
	
	
	if(!m_CCompanyDBPtr->getGroupId(iQuestionId,iGroupId))
	{
		return false;
	}

	//得到questionid 是否存在
	if(!m_FrequencyDBPtr->getCompanyQuestionIdCount(iQuestionId, iGroupId))
	{
		//不存在,插入数据
		if(!m_FrequencyDBPtr->setCompanyQuestionFrequency(iQuestionId, iGroupId))
		{
			return false;
		}
		
	}

	return m_FrequencyDBPtr->addCompanyQuestionFrequency(iQuestionId, iGroupId);

}

bool  CCompanyQAOperateBase::getCompanyQuestionFrequency(int iQuestionId, int &iFrequercy)
{
	//得到group id
	int iGroupId=0;
	
	
	if(!m_CCompanyDBPtr->getGroupId(iQuestionId,iGroupId))
	{
		return false;
	}

	return m_FrequencyDBPtr->getCompanyQuestionFrequency(iQuestionId, iGroupId, iFrequercy);

}

bool CCompanyQAOperateBase::getDefaultAnswer(string &sDefaultAnswer,int iType)
{
	return m_CCompanyDBPtr->GetDefaultAnswer(sDefaultAnswer, iType);
}

bool CCompanyQAOperateBase::checkPrimaryWord(string &sInWord)
{
	return m_CCompanyDBPtr->checkPrimaryWord(sInWord);
}

bool CCompanyQAOperateBase::getMinIdByGroup(vector<int> &vecRetIdList)
{
	if(vecRetIdList.empty())
		return false;

	map<int,int> mapGroupIdList;

	for(vector<int>::iterator iter=vecRetIdList.begin(); iter!=vecRetIdList.end(); iter++)
	{
		int iGroupId=0;
		bool bFindFlag=false;
		if(!m_CCompanyDBPtr->getGroupId(*iter,iGroupId))
		{
			continue;
		}
		mapGroupIdList.insert(pair<int,int> (*iter, iGroupId));
	}

	//找到最小的组
	int iMix=500;
	for(map<int,int>::iterator iter=mapGroupIdList.begin(); iter!=mapGroupIdList.end(); iter++)
	{
		if(iMix >= iter->second)
		{
			iMix = iter->second;
		}
	}

	//将最小组的id返回
	for(map<int,int>::iterator iter=mapGroupIdList.begin(); iter!=mapGroupIdList.end(); iter++)
	{
		if(iMix == iter->second)
		{
			vecRetIdList.clear();
			vecRetIdList.push_back(iter->first);
			return !vecRetIdList.empty();
		}
	}
	return false;
/*
	//按序得到每个组最长字符的question
	vecRetIdList.clear();
	for(vector<int>::iterator iter = vecGroupIdList.begin(); iter != vecGroupIdList.end(); iter++)
	{
		int iQuestionId=0;
		if(!m_CCompanyDBPtr->getMaxLengthIdByGroup(*iter, iQuestionId))
		{
			continue;
		}
		vecRetIdList.push_back(iQuestionId);
	}
	
	return !vecRetIdList.empty();*/
}


///////////////////////////////////////////////////////////////////////////////

bool CCompanyQAOperateBase::getCommand(map < string,string > & mapCommandCn)
{
	return m_CCompanyDBPtr->GetEnToCnField(mapCommandCn, COMPANY_COMMAND);
}

bool CCompanyQAOperateBase::getNormal(map < string,string > & mapNormalCn)
{
	return m_CCompanyDBPtr->GetEnToCnField(mapNormalCn, COMPANY_NORMAL);
}
///////////////////////////////////////////////////////////////////////////////

//得到当前的年的周
bool CCompanyQAOperateBase::getYearWeekByLocalDay(int &iYear, int &iWeek)
{
	return m_CCompanyDBPtr->getYearWeekByLocalDay(iYear, iWeek);
}

bool CCompanyQAOperateBase::getBayesTrainedData(map<string,string> &mapBayesData, string &sModule)
{
	if(sModule.empty())
		return false;
	return m_CCompanyDBPtr->getBayesTrainedData(mapBayesData, sModule);
}

bool CCompanyQAOperateBase::getPrimaryWord(vector<struKeyWord> &vecKeyWords)
{
	return m_CCompanyDBPtr->getPrimaryWord(vecKeyWords);
}


#include "CompanyQuestionIdList.h"

CCompanyQuestionIdList::CCompanyQuestionIdList()
{
}

CCompanyQuestionIdList:: ~CCompanyQuestionIdList()
{
}

bool CCompanyQuestionIdList::setKeyAndId(vector<string> &vecKeyList, vector<int> &vecResultIdList)
{
	if(vecKeyList.empty())
		return false;
	if(vecResultIdList.empty())
		return false;

	if(vecKeyList.at(0).empty())
		return false;
	
	m_vecKeyList.push_back(vecKeyList);
	m_vecResultIdList.push_back(vecResultIdList);
	return true;
}

vector<int> CCompanyQuestionIdList::getResultIdListLastOne()
{
	vector<int> vecResultListId;
	vecResultListId = m_vecResultIdList.back();
	return vecResultListId;
}

vector<string> CCompanyQuestionIdList::getConditionKeyListLastOne()
{
	vector<string> vecKeyList;
	vecKeyList = m_vecKeyList.back();
	return vecKeyList;
}
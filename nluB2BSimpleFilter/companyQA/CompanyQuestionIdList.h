#pragma once

#include <vector>
#include <string>

using namespace std;
class CCompanyQuestionIdList
{
	public:
		CCompanyQuestionIdList();
		~CCompanyQuestionIdList();
	public:
		//把当前的查询条件和返回结果保存起来
		bool setKeyAndId(vector<string> &vecKeyList, vector<int> &vecResultIdList);
		//得到最后一次存放的返回结果
		vector<int> getResultIdListLastOne();
		//得到最后一次存放的查询条件
		vector<string> getConditionKeyListLastOne();
		void clear(){m_vecKeyList.clear(); m_vecResultIdList.clear();}
	private:
		vector<vector<string> > m_vecKeyList;
		vector<vector<int> > m_vecResultIdList;
};

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
		//�ѵ�ǰ�Ĳ�ѯ�����ͷ��ؽ����������
		bool setKeyAndId(vector<string> &vecKeyList, vector<int> &vecResultIdList);
		//�õ����һ�δ�ŵķ��ؽ��
		vector<int> getResultIdListLastOne();
		//�õ����һ�δ�ŵĲ�ѯ����
		vector<string> getConditionKeyListLastOne();
		void clear(){m_vecKeyList.clear(); m_vecResultIdList.clear();}
	private:
		vector<vector<string> > m_vecKeyList;
		vector<vector<int> > m_vecResultIdList;
};

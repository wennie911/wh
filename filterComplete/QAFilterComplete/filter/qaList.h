#pragma once

/*
zcq
��Ŵ�nlu���ص�qa�б�   
 */
 #include "filterPublic.h"

class CNLUQAList
{
	public:
		CNLUQAList();
		~CNLUQAList();
	public:
		//���ݹ�������©����˵�question �����������qaList
		void addQuestion(struNLUQA &nluqa);
		bool filterLeak(vector<string> &vecLeakList);
		vector<struNLUQA> getAllowQAList();
		vector<struNLUQA> getOtherQAList();
		//�ж�qalist�Ƿ�Ϊ��
		bool isNull();
		void clear();
	private:
		vector<struNLUQA> m_NLUQAList;//������qalist
		vector<struNLUQA> m_allowNLUQAList;//©�������qalist
		vector<struNLUQA> m_otherNLUQAList;//������qalist
		
};


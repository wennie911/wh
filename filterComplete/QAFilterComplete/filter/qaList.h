#pragma once

/*
zcq
存放从nlu返回的qa列表   
 */
 #include "filterPublic.h"

class CNLUQAList
{
	public:
		CNLUQAList();
		~CNLUQAList();
	public:
		//根据过滤器的漏点过滤掉question 返回整理过的qaList
		void addQuestion(struNLUQA &nluqa);
		bool filterLeak(vector<string> &vecLeakList);
		vector<struNLUQA> getAllowQAList();
		vector<struNLUQA> getOtherQAList();
		//判断qalist是否为空
		bool isNull();
		void clear();
	private:
		vector<struNLUQA> m_NLUQAList;//完整的qalist
		vector<struNLUQA> m_allowNLUQAList;//漏点允许的qalist
		vector<struNLUQA> m_otherNLUQAList;//其它的qalist
		
};


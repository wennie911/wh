#include "qaList.h"

CNLUQAList::CNLUQAList()
{
	m_NLUQAList.clear();
}

CNLUQAList::~CNLUQAList()
{
}

void CNLUQAList::addQuestion(struNLUQA &nluqa)
{
	struNLUQA qaT;

	qaT.sAnswer = nluqa.sAnswer;
	qaT.sAudio = nluqa.sAudio;
	qaT.sMatchedQuestion = nluqa.sMatchedQuestion;
	qaT.sQuestion = nluqa.sQuestion;
	qaT.sQuestionPinYin = nluqa.sQuestionPinYin;
	qaT.sType = nluqa.sType;
	qaT.sQuestionType = nluqa.sQuestionType;
	qaT.iAffirm = nluqa.iAffirm;
	qaT.iIsBreak = nluqa.iIsBreak;
	qaT.iIsEnd = nluqa.iIsEnd;
	qaT.iLoop = nluqa.iLoop;
	qaT.iIsDel =1;

	//printf("qaT.answer=%s\n", qaT.sAnswer.c_str());

	m_NLUQAList.push_back(qaT);
}

bool CNLUQAList::filterLeak(vector<string> &vecLeakList)
{
	m_allowNLUQAList.clear();
	m_otherNLUQAList.clear();

	for(vector<struNLUQA>::iterator iterQA=m_NLUQAList.begin(); iterQA!=m_NLUQAList.end(); iterQA++)
	{
		for(vector<string>::iterator iterLeak=vecLeakList.begin(); iterLeak!=vecLeakList.end(); iterLeak++)
		{
			//printf("sType=%s, leak=%s, sQuestionType=%s\n", iterQA->sType.c_str(), (*iterLeak).c_str(), iterQA->sQuestionType.c_str());
			if(iterQA->sType.compare(*iterLeak)==0)
				iterQA->iIsDel = 0;
		}
		
	}

	for(vector<struNLUQA>::iterator iterQA=m_NLUQAList.begin(); iterQA!=m_NLUQAList.end(); iterQA++)
	{
		if(iterQA->iIsDel==0)
			m_allowNLUQAList.push_back(*iterQA);
		else
			m_otherNLUQAList.push_back(*iterQA);
	}
	return !m_allowNLUQAList.empty();
}
vector<struNLUQA> CNLUQAList::getAllowQAList()
{
	return m_allowNLUQAList;
}
vector<struNLUQA> CNLUQAList::getOtherQAList()
{
	return m_otherNLUQAList;
}
//ÅÐ¶ÏqalistÊÇ·ñÎª¿Õ
bool CNLUQAList::isNull()
{
	if(m_NLUQAList.empty())
		return false;
	
	//printf("no null answer=%s\n",m_NLUQAList.at(0).sAnswer.c_str());
	return true;
}

void CNLUQAList::clear()
{
	m_NLUQAList.clear();
	m_allowNLUQAList.clear();
	m_otherNLUQAList.clear();
}
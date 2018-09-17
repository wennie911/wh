#include "LogicLayer.h"

CLogicLayer::CLogicLayer()
{
}

CLogicLayer::~CLogicLayer()
{
}

void CLogicLayer::setLayer(struLayer &layer)
{
	
	m_layer.iInitiative=0;
	m_layer.iLayerId=0;
	m_layer.iUsed=0;
	m_layer.sAnswer="";
	m_layer.sDefDenyQuestion="";
	m_layer.sDefQuestion="";
	m_layer.vecLeakList.clear();
	
	m_layer.iInitiative = layer.iInitiative;
	m_layer.iLayerId = layer.iLayerId;
	m_layer.iUsed = layer.iUsed;
	m_layer.sAnswer = layer.sAnswer;
	m_layer.sDefDenyQuestion = layer.sDefDenyQuestion;
	m_layer.sDefQuestion = layer.sDefQuestion;
	copy(layer.vecLeakList.begin(), layer.vecLeakList.end(), back_inserter(m_layer.vecLeakList));
	//printf("m_layer.vecLeakList.at(0)=%s\n", m_layer.vecLeakList.at(0).c_str());
}


int CLogicLayer::getLayerId()
{
	return m_layer.iLayerId;
}

//设置已经回答的answer
bool CLogicLayer::setAnswer(string &sAnswer)
{
	m_layer.sAnswer=sAnswer;
	return !m_layer.sAnswer.empty();
}

bool CLogicLayer::setAudio(string &sAudio)
{
	m_layer.sAudio=sAudio;
	return !m_layer.sAudio.empty();
}

string CLogicLayer::getAnswer()
{
	return m_layer.sAnswer;
}

string CLogicLayer::getAudio()
{
	return m_layer.sAudio;
}

//判断这个层是否已经用过
bool CLogicLayer::layerIsUsed()
{
	return m_layer.iUsed;
}
void CLogicLayer::setUsed(bool bUsedFlag)
{
	m_layer.iUsed = bUsedFlag;
	printf("layer id = %d, setuesd=%d\n", m_layer.iLayerId, m_layer.iUsed);
}
//漏点的列表
vector<string> CLogicLayer::getLeakList()
{
	//vecLeakList.clear();
	//copy(m_layer.vecLeakList.begin(), m_layer.vecLeakList.end(), back_inserter(vecLeakList));
	return m_layer.vecLeakList;
}
//得到确定的缺省问题
string CLogicLayer::getDefQuestion()
{
	return m_layer.sDefQuestion;
}
//得到拒绝的缺省问题
string CLogicLayer::getDenyQuestion()
{
	return m_layer.sDefDenyQuestion;
}

string CLogicLayer::getDefWhenQuestion()
{
	return m_layer.sDefWhenQuestion;
}

string CLogicLayer::getDefWhereQuestion()
{
	return m_layer.sDefWhereQuestion;
}

string CLogicLayer::getDefWhoQuestion()
{
	return m_layer.sDefWhoQuestion;
}

string CLogicLayer::getDefHowQuestion()
{
	return m_layer.sDefHowQuestion;
}

string CLogicLayer::getDefWhatQuestion()
{
	return m_layer.sDefWhatQuestion;
}

string CLogicLayer::getDefWhichQuestion()
{
	return m_layer.sDefWhichQuestion;
}

string CLogicLayer::getDefWhyQuestion()
{
	return m_layer.sDefWhyQuestion;
}








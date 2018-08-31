#include "filterProcess.h"
#include "../json/json_writer.h"

CFilterProcess::CFilterProcess()
{
	m_iDialogueCount=0;
	m_iCurrentLayerId=0;
}

CFilterProcess::~CFilterProcess()
{
}

bool CFilterProcess::init()
{
	//��ȡnluso�����ļ�
	string sDllPath;
	string sDataPath;
	int iLogLevle=0;
	
	if(!readNLUSoConfig("./nlusoConfig.xml", sDllPath, sDataPath, iLogLevle))
	{
		return false;
	}
	if(sDllPath.empty()||sDataPath.empty()|| iLogLevle==0)
	{
		return false;
	}
	//��nlu
	if(!m_so_nlu.LoadSo(sDllPath, sDataPath, iLogLevle))
		return false;
	//��ȡ�����������ļ�
	if(!readFilterConfig("./filterConfig.xml"))
	{
		return false;
	}
	printf("filter one defquestion = %s\n", m_vecLoginLayer.begin()->getDefQuestion().c_str());

	//��ȡ��������ļ�
	return true;
}

bool CFilterProcess::processInSentence(string &sInSentence, string &sOutSentence )
{
	vector<struNLUQA> vecNLUQAList;
	
	m_nluQaList.clear();

	m_iDialogueCount++;
	//����һ��nlu
	if(!m_so_nlu.getAnswer(sInSentence, vecNLUQAList))
		return false;

	//�����صĽ����������
	for(vector<struNLUQA>::iterator iter=vecNLUQAList.begin(); iter!=vecNLUQAList.end(); iter++)
	{
		m_nluQaList.addQuestion(*iter);
	}

	m_nluQaList.isNull();

	initQAStru(m_retQA);
	
	//��������������
	if(!layerProcess())
		return false;

	Json::Value retAnswer;
	Json::FastWriter writer;
	
	retAnswer["question"] =Json::Value(m_retQA.sQuestion);
	retAnswer["answer"] =Json::Value(m_retQA.sAnswer);
	retAnswer["matchedQuestion"] =Json::Value(m_retQA.sMatchedQuestion);
	retAnswer["audio"] =Json::Value(m_retQA.sAudio);
	retAnswer["questionPinYin"] =Json::Value(m_retQA.sQuestionPinYin);
	retAnswer["type"] =Json::Value(m_retQA.sType);
	retAnswer["isBreak"] =Json::Value(m_retQA.iIsBreak);
	retAnswer["isEnd"] =Json::Value(m_retQA.iIsEnd);
	retAnswer["dialogeCount"] = Json::Value(m_iDialogueCount);
	retAnswer["currentLayer"] = Json::Value(m_iCurrentLayerId);
	retAnswer["totalLayer"] = Json::Value(m_iTotalLayer);
	
	sOutSentence="";
	sOutSentence = writer.write(retAnswer);

    return true;
	
}

bool CFilterProcess::readFilterConfig(string sXMLPath)
{
	//����xml
	TiXmlDocument doc;

	if(!doc.LoadFile(sXMLPath.c_str()))
		return false;
	
	
	//���ڵ�
	TiXmlElement *pRootElement = doc.RootElement();
	if(pRootElement == NULL)
		return false;

	//���root�ڵ� �����Ƕ�ڵ�
	TiXmlElement *pLayer = pRootElement->FirstChildElement();

	while(pLayer!=NULL)
	{
		CLogicLayer ll;
		struLayer layer;
		string sT;
		sT = getXmlAttribute(pLayer,"id");
		layer.iLayerId = atoi(sT.c_str());
		sT = getXmlAttribute(pLayer,"initiative");
		layer.iInitiative = atoi(sT.c_str());
		layer.iUsed = 0;
		sT = getXmlAttribute(pLayer,"defDenyQuestion");
		layer.sDefDenyQuestion = sT;
		sT = getXmlAttribute(pLayer,"defQuestion");
		layer.sDefQuestion = sT;
		sT = getXmlAttribute(pLayer,"leakList");
		
		vector<string> vs;
		splitStr(sT, vs, "|");
		copy(vs.begin(), vs.end(),back_inserter(layer.vecLeakList));
		
		ll.setLayer(layer);
		
		m_vecLoginLayer.push_back(ll);
		pLayer = pLayer->NextSiblingElement();
	}
	m_iTotalLayer = m_vecLoginLayer.size();
	return !m_vecLoginLayer.empty();
}

bool CFilterProcess::readNLUSoConfig(string sXMLPath, string &sDllPath, string &sDataPath, int &iLogLevle)
{
	//����xml
	TiXmlDocument doc;

	if(!doc.LoadFile(sXMLPath.c_str()))
		return false;
	
	
	//���ڵ�
	TiXmlElement *pRootElement = doc.RootElement();
	if(pRootElement == NULL)
		return false;

	//nluso�ӽڵ�
	TiXmlElement *nluso = pRootElement->FirstChildElement();

	string sT;
	sT = getXmlAttribute(nluso,"soPath");
	sDllPath = sT;
	sT = getXmlAttribute(nluso,"dataPath");
	sDataPath = sT;
	sT = getXmlAttribute(nluso,"logLevel");
	iLogLevle = atoi(sT.c_str());

	return true;
}

string CFilterProcess::getXmlAttribute(TiXmlElement *pNode, const string sAttributeName)
{
	string sValue="";
	if(pNode->Attribute(sAttributeName.c_str()))
	{
		sValue = pNode->Attribute(sAttributeName.c_str());
		return sValue;
	}

	printf("read xml fail attribute:[%s]\n",sAttributeName.c_str());
	return sValue;
}

//�����ָ���
int CFilterProcess::splitStr(const string &str, vector<string> &ret, string sep)
{
    if (str.empty())
    {
        return 0;
    }

    string tmp;
    string::size_type pos_begin = str.find_first_not_of(sep);
    string::size_type comma_pos = 0;

	if(str.find(sep)==string::npos)
	{
		ret.push_back(str);
		return ret.size();
	}

    while (pos_begin != string::npos)
    {
        comma_pos = str.find(sep, pos_begin);
        if (comma_pos != string::npos)
        {
            tmp = str.substr(pos_begin, comma_pos - pos_begin);
            pos_begin = comma_pos + sep.length();
        }
        else
        {
            tmp = str.substr(pos_begin);
            pos_begin = comma_pos;
        }

        if (!tmp.empty())
        {
            ret.push_back(tmp);
            tmp.clear();
        }
    }
	
	if(ret.size())
		return ret.size();
    return 0;
}


//�õ������к�����Ŀ���layerָ��
CLogicLayer * CFilterProcess::getLayer()
{
	for(vector<CLogicLayer>::iterator iter=m_vecLoginLayer.begin(); iter!=m_vecLoginLayer.end();iter++)
	{
		if(!iter->layerIsUsed())
			return &(*iter);
	}
	return NULL;
}

//�õ���һ�����ش��answer ��audio
string CFilterProcess::getLastAnswer(int iLayerId)
{
	if(iLayerId<=1)
		return "";
	int iLastLayerId = iLayerId-1;
	for(vector<CLogicLayer>::iterator iter=m_vecLoginLayer.begin(); iter!=m_vecLoginLayer.end();iter++)
	{
		if(iter->getLayerId()==iLastLayerId)
			return iter->getAnswer();
	}
	return "";
}

string CFilterProcess::getLastAudio(int iLayerId)
{
	if(iLayerId<=1)
		return "";
	int iLastLayerId = iLayerId-1;
	for(vector<CLogicLayer>::iterator iter=m_vecLoginLayer.begin(); iter!=m_vecLoginLayer.end();iter++)
	{
		if(iter->getLayerId()==iLastLayerId)
			return iter->getAudio();
	}
	return "";
}

void CFilterProcess::setLayerAnswerAndAudio(int iLayerId, string &sAnswer, string &sAudio)
{
	for(vector<CLogicLayer>::iterator iter=m_vecLoginLayer.begin(); iter!=m_vecLoginLayer.end();iter++)
	{
		if(iter->getLayerId() == iLayerId)
		{
			iter->setAnswer(sAnswer);
			iter->setAudio(sAudio);
			return;
		}
	}
	return;
}


bool CFilterProcess::layerProcess()
{
	CLogicLayer *oneFilterPtr;
	
	//�õ�һ�����˲�
	oneFilterPtr = getLayer();
	if(oneFilterPtr==NULL)
	{
		return false;
	}
	//����ǰ���id��������
	m_iCurrentLayerId = oneFilterPtr->getLayerId();
	printf("layerId=%d\n", m_currentLayerId);
	
	//�õ�������˲�������qaList
	vector<string> vecLeakList = oneFilterPtr->getLeakList();
	m_nluQaList.filterLeak(vecLeakList);

	vector<struNLUQA> vecAllowQAList, vecOtherQAList;
	vecAllowQAList = m_nluQaList.getAllowQAList();
	vecOtherQAList = m_nluQaList.getOtherQAList();

	//������ͨ����question
	if(!vecAllowQAList.empty())
	{
		printf("vecAllowQAList size=%d\n",vecAllowQAList.size());
		//Ŀǰֻȡ��һ��
		vector<struNLUQA>::iterator iterQA = vecAllowQAList.begin();
		if(iterQA->iIsEnd ==1)
		{
			setRetAnswer(*iterQA);
			m_bSessionEnd=true;
			return true;
		}
		//����ش��answer
		
		setRetAnswer(*iterQA);
		setLayerAnswerAndAudio(oneFilterPtr->getLayerId(), iterQA->sAnswer, iterQA->sAudio);
		oneFilterPtr->setUsed(true);
		
		return true;
	}
	else if(!vecOtherQAList.empty())
	{
		printf("vecOtherQAList size=%d\n",vecOtherQAList.size());
		for(vector<struNLUQA>::iterator iterOtherQA = vecOtherQAList.begin(); iterOtherQA != vecOtherQAList.end(); iterOtherQA++)
		{
			//�ж��Ƿ�Ҫ�ظ�
			if(iterOtherQA->sAnswer.compare("@sayagain")==0)
			{
				string sAnswer="";
				string sAudio="";
				
				printf("sysagain\n");
				
				sAnswer = oneFilterPtr->getAnswer();
				if(sAnswer.length()==0)
				{
					//����һ��
					sAnswer = getLastAnswer(oneFilterPtr->getLayerId());
					if(sAnswer.length()==0)
						return false;
					sAudio = getLastAudio(oneFilterPtr->getLayerId());
					setRetAnswer(sAnswer, sAudio, 0, iterOtherQA->sQuestion, iterOtherQA->sMatchedQuestion, 
						iterOtherQA->sQuestionPinYin,iterOtherQA->sType);
					setLayerAnswerAndAudio(oneFilterPtr->getLayerId(), sAnswer, sAudio);
					return true;
				}
				sAudio = oneFilterPtr->getAudio();
				setRetAnswer(sAnswer, sAudio, 0, iterOtherQA->sQuestion, iterOtherQA->sMatchedQuestion, 
					iterOtherQA->sQuestionPinYin, iterOtherQA->sType);
				setLayerAnswerAndAudio(oneFilterPtr->getLayerId(), sAnswer, sAudio);
				return true;
			}
		
			//�ж��Ƿ��п���ѭ����q
			if(iterOtherQA->iLoop==1)
			{
				printf("loop\n");
				setRetAnswer(*iterOtherQA);
				setLayerAnswerAndAudio(oneFilterPtr->getLayerId(), iterOtherQA->sAnswer, iterOtherQA->sAudio);
				return true;
			}
		}
		
		//���qalist�з񶨱�־���жϹ��˲��Ƿ��з�ȱʡ�ش��q
		string sDefDenyQuestion = oneFilterPtr->getDenyQuestion();
		for(vector<struNLUQA>::iterator iterOtherQA = vecOtherQAList.begin(); iterOtherQA != vecOtherQAList.end(); iterOtherQA++)
		{
			if(iterOtherQA->iAffirm==0)
			{
				printf("affirm\n");
				//����һ��nlu ��ȱʡ�����ⷢһ��
				vector<struNLUQA> vecNLUQAListT;
				vector<struNLUQA>::iterator iterQA;
				if(!m_so_nlu.getAnswer(sDefDenyQuestion, vecNLUQAListT))
					return false;
				iterQA=vecNLUQAListT.begin();
					
				setRetAnswer(iterQA->sAnswer, iterQA->sAudio, iterQA->iIsEnd, iterOtherQA->sQuestion, iterOtherQA->sMatchedQuestion, 
					iterOtherQA->sQuestionPinYin,iterOtherQA->sType);
				setLayerAnswerAndAudio(oneFilterPtr->getLayerId(), iterQA->sAnswer, iterQA->sAudio);
				oneFilterPtr->setUsed(true);
				return true;
			}
		}
		
		//���˲��Ƿ��п϶�ȱʡ�ش��q
		vector<struNLUQA>::iterator iterOtherQA = vecOtherQAList.begin();
		string sDefQuestion = oneFilterPtr->getDefQuestion();
		if(sDefQuestion.empty())
			return false;
		//����һ��nlu ��ȱʡ�����ⷢһ��
		vector<struNLUQA> vecNLUQAListT;
		vector<struNLUQA>::iterator iterQA;
		printf("defQuestion=%s\n", sDefQuestion.c_str());
		if(!m_so_nlu.getAnswer(sDefQuestion, vecNLUQAListT))
			return false;
		iterQA=vecNLUQAListT.begin();
		
		setRetAnswer(iterQA->sAnswer, iterQA->sAudio, iterQA->iIsEnd, iterOtherQA->sQuestion, iterOtherQA->sMatchedQuestion, 
			iterOtherQA->sQuestionPinYin,iterOtherQA->sType);
		setLayerAnswerAndAudio(oneFilterPtr->getLayerId(), iterQA->sAnswer, iterQA->sAudio);
		oneFilterPtr->setUsed(true);
		return true;
	}
}

void CFilterProcess::setRetAnswer(struNLUQA & oneQA)
{
	m_retQA.sAnswer = oneQA.sAnswer;
	m_retQA.sAudio = oneQA.sAudio;
	m_retQA.sMatchedQuestion = oneQA.sMatchedQuestion;
	m_retQA.sQuestion = oneQA.sQuestion;
	m_retQA.sQuestionPinYin = oneQA.sQuestionPinYin;
	m_retQA.sType = oneQA.sType;
	m_retQA.iIsBreak = oneQA.iIsBreak;
	m_retQA.iIsEnd = oneQA.iIsEnd;
}

void CFilterProcess::setRetAnswer(string &sAnswer, string &sAudio, int iIsEnd, string &sQuestion, string &sMatchedQuestion, string &sQuestionPinYin, string &sType)
{
	m_retQA.sAnswer = sAnswer;
	m_retQA.sAudio = sAudio;
	m_retQA.sMatchedQuestion = sMatchedQuestion;
	m_retQA.sQuestion = sQuestion;
	m_retQA.sQuestionPinYin = sQuestionPinYin;
	m_retQA.sType = sType;
	m_retQA.iIsBreak = 1;
	m_retQA.iIsEnd = iIsEnd;
}

void CFilterProcess::initQAStru(struNLUQA &oneQA)
{
	oneQA.sAnswer = "";
	oneQA.sAudio = "";
	oneQA.sMatchedQuestion = "";
	oneQA.sQuestion = "";
	oneQA.sQuestionPinYin = "";
	oneQA.sType="";
	oneQA.iIsBreak = 0;
	oneQA.iIsEnd = 0;
}
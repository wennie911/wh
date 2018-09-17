#include "CompanyApplyLocator.h"


#include "../baisic_info/ConceptContent.h"
#include "../baisic_info/CompanyContent.h"


#define QANORMALMODULE 0
#define QAXYXYKMODULE 1

#define NOJSON 0
#define PARSEJSONTRUE 1
#define PARSEJSONFALSE 2

static bool SplitWord(const string &word, WORDPARAM &param)
{
	return CWordFinder::GetInstance()->FindFromUserDic(word, param);
}

CCompanyApplyLocator::CCompanyApplyLocator()
{
	m_nConceptSetType = RA_CONCEPTSET_COMPANY;
	m_iCurrentModule = QANORMALMODULE;
	
	string sRet;
	setModule("normal", sRet);
	
	Init();
}

CCompanyApplyLocator::~CCompanyApplyLocator()
{
	map<int, void * >::iterator iter = m_mapQAIndex.begin();

	while(iter != m_mapQAIndex.end())
	{
	  	switch(iter->first)
	  	{
			case QANORMALMODULE:
				delete (CCompanyQANormal *)iter->second;
				break;
			default:
				;
	  	}

	  	iter->second = NULL;
		iter++;
		 //删除map元素
		 //m_mapGameIndex.erase(iter++); 

	}
}

bool CCompanyApplyLocator::ContentLocateCompare(CINSemanticInfos * pSemantic,CCellInfoList * pCellInfoList)
{
	AITRACE
	string sAnswer="";
	string sRet="";
	string st="";
	struMediaUrl mu;
	CCompanyPublic cp;
	vector<struQuestionList> vecSentenceList;
	m_pSemantic = pSemantic;
	m_pCellInfoList = pCellInfoList;
	string sSentence=pSemantic->m_szInputSentence;
	int iQuestionId=0;

	//加载自定义词库, 用完后要及时释放
	CWordFinder::RegisterThirdDic(&SplitWord);
	
	m_pSemanticPrivate = CSegmantation::GetInstance();
	m_pSemanticPrivate->AnalyzeSentenceSegmentation(sSentence.c_str());
	CWordFinder::RemoveThirdDic(&SplitWord);
	/////////////////////////////////////////
	
	map<int,void* >::iterator iter;
	iter = m_mapQAIndex.find(QANORMALMODULE);

	if(iter == m_mapQAIndex.end())
		return false;
	CCompanyQANormal *pQaNormal = (CCompanyQANormal *)iter->second;
	PrintI("into ContentLocateCompare sentence=%s", sSentence.c_str());
	iQuestionId=pQaNormal->compareQuestion(sSentence);
	PrintI("iQuestionId=%d", iQuestionId);
	string sKeyWords;
	if(iQuestionId<=0)
	{	
		//判断句子里是否有关键词,如果有，用关键词作为question再查找一次
		
		if(procKeyWords(sKeyWords))
		{
			iQuestionId=pQaNormal->compareQuestion(sKeyWords);

			if(iQuestionId>0)//有匹配的关键词
			{
				//跳转
				goto PARAM;
			}
		}

		//用拼音来判断关键词
		vector<struKeyWord> vecKewWordsPY;
		sKeyWords.clear();
		if(m_QADB.getPrimaryWord(vecKewWordsPY))
		{
			if(m_public.matchPinYin(sSentence, vecKewWordsPY, sKeyWords))
			{
				iQuestionId=pQaNormal->compareQuestion(sKeyWords);

				if(iQuestionId>0)//有匹配的关键词
				{
					//跳转
					goto PARAM;
				}
			}
		}
		
		
		return false;
	}

	PARAM: 
		
	sAnswer = pQaNormal->getAnswerByQuestionId(iQuestionId);
	PrintI("into ContentLocateCompare sAnswer=%s", sAnswer.c_str());
	pQaNormal->getGroupParam(iQuestionId, mu);
	
	if(!sAnswer.empty())
	{
		vector<struFullQA> qaList;
		struFullQA qa;
		qa.sQuestion = sSentence;
		qa.sAnswer = sAnswer;
		qa.sMatchedQuestion = sSentence;

		qa.media.sType = mu.sType;
		qa.media.sAudio = mu.sAudio;
		qa.media.sIsEnd = mu.sIsEnd;
		qa.media.sIsBreak = mu.sIsBreak;
		qa.media.sLoop = mu.sLoop;
		qa.media.sAffirm = mu.sAffirm;
		
		qaList.push_back(qa);	
		sRet = cp.makeSenenceJson(qaList);
		//pQaNormal->setLastAnswer(sSentence, sAnswer, mu.sImageUrl, sKeyWords, mu.sAudioUrl);
		PrintI("into ContentLocateCompare ret=%s", sRet.c_str());
	}
		
	return makeSentence(sRet);

}




bool CCompanyApplyLocator::procKeyWords(string &sRet)
{
	for(int i=0; i < m_pSemanticPrivate->m_vWordList.size(); i++)
	{
		CWord *pWord = m_pSemanticPrivate->m_vWordList.at(i);
		PrintI("word=%s, npos=%d",pWord->m_szValue.c_str(), pWord->m_nPos[0] );
		for(int j=0; j < 6; j++)
		{
			if(pWord->m_nPos[j] == RA_WORD_NOUN_SPECIAL)
			{
				sRet = pWord->m_szValue;
				return true;
			}
		}
		
	}
	return false;
}

//处理特殊的回答 医生介绍， 症状选择， 科室选择，打开视频等
bool CCompanyApplyLocator::getSpecialAnswer(vector<string> &vecParam, string &sRet)
{
	if(vecParam.size()<3)
		return false;
	
	//第一个字段是模块
	m_sModule = vecParam.at(0);
	//弟二哥字段是事件
	m_sEvent = vecParam.at(1);

	
	return false;
}

//如果是行业模块是激活状态, 直接处理掉
bool CCompanyApplyLocator::ContentLocateActivity(CINSemanticInfos * pSemantic,CCellInfoList * pCellInfoList)
{
	AITRACE
	/*switch(selectModule())
	{
		case QANORMALMODULE:
			return false;
		case QAHTDISHMODULE:
			
	}*/
	PrintI("Current module = %d", selectModule());
	if(selectModule()>0)
	{
		bool bt=ContentLocate(pSemantic, pCellInfoList);
		PrintI("ContentLocate return %d", bt);
		return bt;
	}
	PrintI("ContentLocateActivity return false");
	return false;
}

bool CCompanyApplyLocator::ContentLocate(CINSemanticInfos * pSemantic,CCellInfoList * pCellInfoList)
{
	AITRACE
	//小于等于四个词的退出
int nCharLen=0;
#ifdef WIN32
	nCharLen = 2;
#else
	nCharLen = 3;
#endif
	string sSentence=pSemantic->m_szInputSentence;
	bool bShortFlag=false;
	vector<CINSemanticInfo*>::const_iterator iter = pSemantic->m_vSemantics.begin();

	int iToneQuery = (*iter)->m_nTone;
	int iToneQueryEx = (*iter)->m_nToneEx;
	
/*	if(sSentence.size()<=nCharLen*1)
	{
		return false;
	}*/
/*	if(iToneQueryEx<=0)
	{
		if(sSentence.size()<=nCharLen*4)
		{
			if(!m_QADB.checkPrimaryWord(sSentence))
			{
				return false;
			}
			bShortFlag=false;
		}
	}
*/
	if(sSentence.size()<=nCharLen*5)
	{
		bShortFlag=true;
	}
	
	vector<struWordList> vecWordsList;
	string sRetStr;
	m_pSemantic = pSemantic;
	m_pCellInfoList = pCellInfoList;

	switch(selectModule())
	{
		case QANORMALMODULE:
		{
			vecWordsList=m_wordsOperate.getWordsList(m_pSemantic);
			map<int,void* >::iterator iter;
			iter = m_mapQAIndex.find(QANORMALMODULE);

			if(iter == m_mapQAIndex.end())
				return false;
			CCompanyQANormal *pQaNormal = (CCompanyQANormal *)iter->second;
			
			pQaNormal->initNA(vecWordsList, iToneQueryEx,(void *)pSemantic, (void *)this);
			pQaNormal->setShortSentenceFlag(bShortFlag);
			if(pQaNormal->getAnswer(sRetStr))
			{
				return makeSentence(sRetStr);
			}
			
			break;
		}
		
		default:
		{
			vecWordsList=m_wordsOperate.getWordsList(m_pSemantic);
			map<int,void* >::iterator iter;
			iter = m_mapQAIndex.find(QANORMALMODULE);

			if(iter == m_mapQAIndex.end())
				return false;
			CCompanyQANormal *pQaNormal = (CCompanyQANormal *)iter->second;
			
			pQaNormal->initNA(vecWordsList, iToneQueryEx,(void *)pSemantic, (void *)this);
			pQaNormal->setShortSentenceFlag(bShortFlag);
			if(pQaNormal->getAnswer(sRetStr))
			{
				return makeSentence(sRetStr);
			}
			
			break;
		}
			
	}
	return false;
}

bool CCompanyApplyLocator::CommandLocate(CINSemanticInfos *inpSemantic, CCellInfoList *outCellList)
{
	return false;
}

bool CCompanyApplyLocator::HistoryLocate(CINSemanticInfos *inpSemantic, CCellInfoList *outCellList)
{
	return false;
}

int CCompanyApplyLocator::selectModule()
{
	//找到当前的机器人(serial)用哪个模块处理
	
	return m_iCurrentModule;
}

string CCompanyApplyLocator::getModuleStr(int iSelectModule)
{
	switch(iSelectModule)
	{
		case QANORMALMODULE:
			return "normal";
		default:
			return "normal";
	}
	return "normal";
}

int CCompanyApplyLocator::getModuleIdx(string sModule)
{
	if(sModule.compare("normol")==0)
	{
		return QANORMALMODULE;
	}
		
	return QANORMALMODULE;
}
void CCompanyApplyLocator::setModule(string sModule, string &sRet)
{
	
	if(sModule.compare("normal")==0)
	{
		m_iCurrentModule = QANORMALMODULE;
		sRet="";
		return ;
	}
		
}

void CCompanyApplyLocator::Init()
{
	m_BayesPtr=NULL;
	
	//加载标准模块
	CCompanyQANormal *pQANormal = new(std::nothrow)CCompanyQANormal();
 	if(pQANormal)
 	{
 		m_mapQAIndex[QANORMALMODULE] = (void *) pQANormal;
 	}

	m_QADB.getPrimaryWord(m_vecKeyWordsList);
}

//转换成结构返回
bool CCompanyApplyLocator::makeSentence(string &sRet)
{
	if(sRet.empty())
		return false;

	CCompanyContent *pNewContent = NULL;
	pNewContent = new(nothrow) CCompanyContent();
	if (pNewContent == NULL)
		return false;

	PrintI("makeSentence %s", sRet.c_str());
	pNewContent->m_nContentType = COMPANY_QA;
	pNewContent->SetStrVariable(T_ANSWER, sRet);

	CCellInfo *pCellInfo = new(std::nothrow) CCellInfo();
	if(pCellInfo == NULL) 
		return false;

	pCellInfo->m_nConceptSetType = RA_CONCEPTSET_COMPANY;
	
	m_pSemantic->SetParseAll();
	pCellInfo->m_pContent = pNewContent;
	
	for(int i=0; i<m_pCellInfoList->m_vInfoList.size(); i++)
	{
		delete m_pCellInfoList->m_vInfoList.at(i);
	}
	m_pCellInfoList->m_vInfoList.clear();
	m_pCellInfoList->m_vInfoList.push_back(pCellInfo);

	return true;
}

bool CCompanyApplyLocator::setQuestionList(vector<struQuestionList> &vecQuestionList)
{
	m_vecQuestionList.clear();
	copy(vecQuestionList.begin(), vecQuestionList.end(), back_inserter(m_vecQuestionList));
	return !vecQuestionList.empty();
}

string CCompanyApplyLocator::getAnswerStr(int iSelectedId)
{
	
	return "";
	
}


// true 成功 false 不处理 2失败
int CCompanyApplyLocator::parseJson(string sJson, string &sRet)
{
	try
	{
		PrintI("input json length(%ld)", sJson.size());
		if(sJson.find("\{")!=string::npos &&
		   sJson.find("\}")!=string::npos && 
		   sJson.find(":")!=string::npos )
		{
			;
		}
		else
			return NOJSON;
		
		
	}
	catch (...)
	{
		PrintI("catch return 2");
		
		return PARSEJSONFALSE;
	}
}


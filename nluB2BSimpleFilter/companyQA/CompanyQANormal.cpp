#include "CompanyQANormal.h"
#include "CompanyWordsOperate.h"
#include "CompanyApplyLocator.h"
#include "../dialogue_management/sort_question.h"
#include "../baisic_info/InSematicInfos.h"
#include "../AICC/KGTimeInterface.h"
#include "../AICC/KgObjStru.h"
#include "../AICC/KGPublic.h"

#define MATCHVALUE 0.6

CCompanyQANormal::CCompanyQANormal()
{
	m_bShortSentenceFlag=false;
}

CCompanyQANormal::~CCompanyQANormal()
{
	;
}

void CCompanyQANormal::initNA(vector<struWordList> vecWordsList, int iTone, void *vPtr, void *vApplyLocatorPt)
{
	m_vecWordsList.clear();
	copy(vecWordsList.begin(), vecWordsList.end(), back_inserter(m_vecWordsList));
	m_iTone = iTone;
	m_vPtr = vPtr;
	
	m_applyLocatorPtr = vApplyLocatorPt;
	m_sInSentence = ((CINSemanticInfos *)vPtr)->m_szInputSentence;
	int nCharLen=0;
}


bool CCompanyQANormal::process(string & retStr)
{
	return false;
}

bool CCompanyQANormal::getAnswer(string & retStr)
{
	int nCharLen;
#ifdef WIN32
	nCharLen = 2;
#else
	nCharLen = 3;
#endif

	//生成一个缺省的返回结构
	struFullQA qaDefault;

	qaDefault.sQuestion = m_sInSentence;
	qaDefault.sMatchedQuestion="";
	qaDefault.sAnswer="";
	qaDefault.media.sAudio="";
	qaDefault.media.sType="Z";//噪音
	qaDefault.media.sIsEnd="0";
	qaDefault.media.sIsBreak="0";
	qaDefault.media.sLoop="0";
	qaDefault.media.sAffirm="1";

	if(m_sInSentence.size()<=nCharLen*1)
	{
		vector<struFullQA> qaList;
		qaList.push_back(qaDefault);
		retStr=m_cp.makeSenenceJson(qaList);
		return !retStr.empty();
	}
	if(m_vecWordsList.empty())
	{
		vector<struFullQA> qaList;
		qaList.push_back(qaDefault);
		retStr=m_cp.makeSenenceJson(qaList);
		return !retStr.empty();
	}
	
	if(getQuestionId())
	{
		retStr=getAnswerFromId();
		if(retStr.empty())
		{
			vector<struFullQA> qaList;

			qaDefault.media.sType="";
			qaList.push_back(qaDefault);
			retStr=m_cp.makeSenenceJson(qaList);

			return !retStr.empty();
		}
		else
			return !retStr.empty();
	}
	
	vector<struFullQA> qaList;	
	qaList.push_back(qaDefault);
	retStr=m_cp.makeSenenceJson(qaList);

	return !retStr.empty();
}


//得到id对应的answer
string CCompanyQANormal::getAnswerFromId()
{
	string sQuestionStr="";
	string sAnswerStr="";
	vector<int> vecIdList;
	vector<string> vecQuestionStr;
	bool bT=false;

	vecIdList= m_questionIdList.getResultIdListLastOne();
		
	if(vecIdList.size()>=10)
	{
		//找一个兜底的answer
		return "";
	}
	
	if(vecIdList.size()==1)//只有一个ID的时候,认为是唯一答案
	{
		if(!m_QADB.getAnswerByQuestionId(vecIdList.at(0), sAnswerStr))
		{
			return "";
		}
				
		//检查问句的匹配度
		if(!m_QADB.getQuestionStrByQuestionId(vecIdList.at(0), sQuestionStr))
		{
			return "";
		}
		float fT=getMatchCount(sQuestionStr);
		if(fT<MATCHVALUE)
		{
			return "";
		}
		
		//得到组的多媒体参数
		struMediaUrl mu;
		vector<struFullQA> qaList;				
		struFullQA qa;
		
		getGroupParam(vecIdList.at(0), mu);
		qa.sQuestion = m_sInSentence;
		qa.sAnswer = sAnswerStr;
		qa.sMatchedQuestion = sQuestionStr;

		qa.media.sType = mu.sType;
		qa.media.sAudio = mu.sAudio;
		qa.media.sIsEnd = mu.sIsEnd;
		qa.media.sIsBreak = mu.sIsBreak;
		qa.media.sLoop =  mu.sLoop;
		qa.media.sAffirm = mu.sAffirm;
		
		qaList.push_back(qa);
		
		return m_cp.makeSenenceJson(qaList);
		
	}

	

	//找到所有的问题,并记录与原句的匹配度
	float fMax=0;
	map<int,float> mapId;
	
	m_questionList.clear();
	
	for(vector<int>::iterator iter=vecIdList.begin(); iter!=vecIdList.end(); iter++)
	{
		struSentence st;
		sQuestionStr="";

		
		if(!m_QADB.getQuestionStrByQuestionId(*iter, sQuestionStr))
			continue;
		st.iId=*iter;
		st.sSentence=sQuestionStr;

		float f=getMatchCount(sQuestionStr);
		if(f<=MATCHVALUE)
			continue;
		mapId.insert(pair<int,float>(*iter,f));
		
		m_questionList.push_back(st);
	}
	if(mapId.size()==0)
		return "";
	//找到最匹配的一个

	//得到最大的一个数
	int iKeyId=0;
	int iId=0;
	
	for(map<int,float>::iterator iter=mapId.begin(); iter!=mapId.end(); iter++)
	{
		if(fMax < iter->second)
		{
			fMax = iter->second;
			iKeyId = iter->first;
		}
	}
	
	//如果最大数的id只有一个则输出
	int iCount=0;
	for(map<int,float>::iterator iter=mapId.begin(); iter!=mapId.end(); iter++)
	{
		if(fMax==iter->second)
		{
			iCount++;
		}
	}

	
	if(iCount==1)
	{
		if( fMax<MATCHVALUE) //考虑2个字匹配一个字的情况
			return "";

		map<int,float>::iterator iterOne = mapId.find(iKeyId);
		if(iterOne == mapId.end())
		{
			return "";

		}
		else
		{
			//得到答案
			if(!m_QADB.getAnswerByQuestionId(iterOne->first, sAnswerStr))
			{
				//把当前q的词集合保存下来
				//saveCurrentWordList();
				return "";
			}
						
			//得到组的多媒体参数
			struMediaUrl mu;
			vector<struFullQA> qaList;				
			struFullQA qa;

			sQuestionStr="";
			
			getGroupParam(vecIdList.at(0), mu);
			m_QADB.getQuestionStrByQuestionId(iterOne->first, sQuestionStr);
			qa.sQuestion = m_sInSentence;
			qa.sAnswer = sAnswerStr;
			qa.sMatchedQuestion = sQuestionStr;

			qa.media.sType = mu.sType;
			qa.media.sAudio = mu.sAudio;
			qa.media.sIsEnd = mu.sIsEnd;
			qa.media.sIsBreak = mu.sIsBreak;
			qa.media.sLoop = mu.sLoop;
			qa.media.sAffirm = mu.sAffirm;
		
			qaList.push_back(qa);
			
			return m_cp.makeSenenceJson(qaList);
		}
	}
    
	if(fMax<MATCHVALUE)
		return "";
	
	m_questionListAllSelect.clear();
	//得到所有的questionId
	vecIdList.clear();

	for(vector<struSentence>::iterator iter=m_questionList.begin();iter != m_questionList.end(); iter++)
	{
		vecIdList.push_back(iter->iId);
	}
	
	if(!m_QADB.getMinIdByGroup(vecIdList))
	{
		return "";
	}
	
	if(vecIdList.size()==1)
	{
		if(!m_QADB.getAnswerByQuestionId(vecIdList.at(0), sAnswerStr))
		{
			return "";
		}
		if(!m_QADB.getQuestionStrByQuestionId(vecIdList.at(0), sQuestionStr))
		{
			return "";
		}
	
		//得到组的多媒体参数
		struMediaUrl mu;
		vector<struFullQA> qaList;				
		struFullQA qa;

		getGroupParam(vecIdList.at(0), mu);
		
		qa.sQuestion = m_sInSentence;
		qa.sAnswer = sAnswerStr;
		qa.sMatchedQuestion = sQuestionStr;

		qa.media.sType = mu.sType;
		qa.media.sAudio = mu.sAudio;
		qa.media.sIsEnd = mu.sIsEnd;
		qa.media.sIsBreak = mu.sIsBreak;
		qa.media.sLoop = mu.sLoop;
		qa.media.sAffirm = mu.sAffirm;
		
		qaList.push_back(qa);
		return m_cp.makeSenenceJson(qaList);
	}

	//有多个,取前三个
	vector<struFullQA> qaList;		
	for(vector<int>::iterator iter=vecIdList.begin(); iter!=vecIdList.end(); iter++)
	{
		sAnswerStr="";
		sQuestionStr="";
		if(!m_QADB.getAnswerByQuestionId(*iter, sAnswerStr))
		{
			continue;
		}
		if(!m_QADB.getQuestionStrByQuestionId(*iter, sQuestionStr))
		{
			return "";
		}

		//得到组的多媒体参数
		struMediaUrl mu;
		struFullQA qa;

		sQuestionStr="";
			
		getGroupParam(*iter, mu);
		
		qa.sQuestion = m_sInSentence;
		qa.sAnswer = sAnswerStr;
		qa.sMatchedQuestion = sQuestionStr;

		qa.media.sType = mu.sType;
		qa.media.sAudio = mu.sAudio;
		qa.media.sIsEnd = mu.sIsEnd;
		qa.media.sIsBreak = mu.sIsBreak;
		qa.media.sLoop = mu.sLoop;
		qa.media.sAffirm = mu.sAffirm;
		
		qaList.push_back(qa);
	}
	
	return m_cp.makeSenenceJson(qaList);

	
}


bool CCompanyQANormal::getFirstPriority(vector<int> &vecCurrentPriorityWordsIdx)
{
	vecCurrentPriorityWordsIdx.clear();	
	
	for(m_iPriority=0; m_iPriority <=99; m_iPriority++)
	{
		/*if(m_iPriority==5)//疑问代词不用
			return false;*/
		for(vector<struWordList>::iterator iter = m_vecWordsList.begin(); iter != m_vecWordsList.end(); iter++)
		{
			if(iter->iWordType==m_iPriority)
			{
				m_iPriority=iter->iWordType;
				int iIdx = iter - m_vecWordsList.begin(); 
				vecCurrentPriorityWordsIdx.push_back(iIdx);
			}
		}

		
		if(!vecCurrentPriorityWordsIdx.empty())
		{
			return true;
		}
	}
	return !vecCurrentPriorityWordsIdx.empty();
}

bool CCompanyQANormal::getNextPriority(vector<int> &vecCurrentPriorityWordsIdx)
{
	vecCurrentPriorityWordsIdx.clear();	
	int iPriorityTemp=m_iPriority;
	for(m_iPriority+=1; m_iPriority <=99; m_iPriority++)
	{
	/*	if(m_iPriority==5)//疑问代词不用
			return false;*/
		for(vector<struWordList>::iterator iter = m_vecWordsList.begin(); iter != m_vecWordsList.end(); iter++)
		{
			if(iter->iWordType==m_iPriority)
			{
				m_iPriority=iter->iWordType;
				int iIdx = iter - m_vecWordsList.begin(); 
				vecCurrentPriorityWordsIdx.push_back(iIdx);
			}
		}
		if(!vecCurrentPriorityWordsIdx.empty())
		{
			return true;
		}
	}
	if(vecCurrentPriorityWordsIdx.empty())
	{
		m_iPriority = iPriorityTemp;
		return false;
	}
	return true;
}

bool CCompanyQANormal::getFirstPriorityLastQA(vector<int> &vecCurrentPriorityWordsIdx)
{
	vecCurrentPriorityWordsIdx.clear();	
	for(m_iPriority=0; m_iPriority <=99; m_iPriority++)
	{
		for(vector<struWordList>::iterator iter = m_vecWordsListLastQA.begin(); iter != m_vecWordsListLastQA.end(); iter++)
		{
			if(iter->iWordType==m_iPriority)
			{
				m_iPriority=iter->iWordType;
				int iIdx = iter - m_vecWordsListLastQA.begin(); 
				vecCurrentPriorityWordsIdx.push_back(iIdx);
			}
		}
		if(!vecCurrentPriorityWordsIdx.empty())
		{
			return true;
		}
	}
	return !vecCurrentPriorityWordsIdx.empty();
}

bool CCompanyQANormal::getNextPriorityLastQA(vector<int> &vecCurrentPriorityWordsIdx)
{
	vecCurrentPriorityWordsIdx.clear();	
	int iPriorityTemp=m_iPriority;
	for(m_iPriority+=1; m_iPriority <=99; m_iPriority++)
	{
		for(vector<struWordList>::iterator iter = m_vecWordsListLastQA.begin(); iter != m_vecWordsListLastQA.end(); iter++)
		{
			if(iter->iWordType==m_iPriority)
			{
				m_iPriority=iter->iWordType;
				int iIdx = iter - m_vecWordsListLastQA.begin(); 
				vecCurrentPriorityWordsIdx.push_back(iIdx);
			}
		}
		if(!vecCurrentPriorityWordsIdx.empty())
		{
			return true;
		}
	}
	if(vecCurrentPriorityWordsIdx.empty())
	{
		m_iPriority = iPriorityTemp;
		return false;
	}
	return true;
}

int CCompanyQANormal::getSentenceBayesType(vector<struWordList> &vecWordList)
{
	if(((CCompanyApplyLocator *) m_applyLocatorPtr)->m_BayesPtr==NULL)
		return 0;
	vector<string> vecBayesWordsList;
	for(vector<struWordList>::iterator iter = vecWordList.begin(); iter != vecWordList.end(); iter++)
	{
		vecBayesWordsList.push_back(iter->vecSynonymWords[0]);
	}

	return ((CCompanyApplyLocator *) m_applyLocatorPtr)->m_BayesPtr->getSentenceType(vecBayesWordsList);
}

/*
1、按优先词在数据表查找，在结果里用其它的词继续查找。
2、如果第一个优先查不到，用第二个查找
3、查询的结果如果是多个，再用上一个QA保存的词继续查找
*/
bool CCompanyQANormal::getQuestionId()
{
	vector<int> vecCurrentPriorityWordsIdx;
	vector<int> vecIdList;
	vector<int> vecIdListTmp;
	vector<int> vecIdFirstList;
	vector<string> vecTemp;
	m_iSentenceType=0;
	m_iSentenceType  = getSentenceBayesType(m_vecWordsList);
	PrintI("---------------------------------m_iSentenceType=%d------------------\n", m_iSentenceType);
	int iSize= m_vecWordsList.size();
	bool bChecked=false;
	m_questionIdList.clear();
	vecTemp.push_back("test");

	//找到第一个优先级的词
	getFirstPriority(vecCurrentPriorityWordsIdx);
	
	//按当前优先级词的顺序，查找数据表
	//用第一个词集合得到所有的q
	
	for(vector<int>::iterator iter=vecCurrentPriorityWordsIdx.begin(); iter!=vecCurrentPriorityWordsIdx.end(); iter++)
	{
		if(m_QADB.fristQuestionIdList(m_vecWordsList.at(*iter).vecSynonymWords, vecIdList))
		{
			m_vecWordsList.at(*iter).bUsedFlag=true;
			m_vecWordsList.at(*iter).bValidFlag=true;
			m_questionIdList.setKeyAndId(m_vecWordsList.at(*iter).vecSynonymWords, vecIdList);
			//copy(vecIdFirstList.begin(), vecIdFirstList.end(), vecIdList.end());
			//vecIdList.insert(vecIdList.end(), vecIdFirstList.begin(), vecIdFirstList.end());
			break;
		}
		else
			m_vecWordsList.at(*iter).bUsedFlag=true;
	}

	
	if(!vecIdList.empty())//找到了
	{
		;
	}
	/*if(!vecIdList.empty())
	{
		m_questionIdList.setKeyAndId(vecTemp, vecIdList);

	}*/
	else  //没有检查到
	{
		vecIdList.clear();
		NEXTSEARCH:
		//继续用后续的词去查找，直到找到q的集合
		//得到下一个优先级的词 查找数据表 没找到继续下个优先级的
		if(getNextPriority(vecCurrentPriorityWordsIdx))
		{
			for(vector<int>::iterator iter=vecCurrentPriorityWordsIdx.begin(); iter!=vecCurrentPriorityWordsIdx.end(); iter++)
			{
				if(m_QADB.fristQuestionIdList(m_vecWordsList.at(*iter).vecSynonymWords, vecIdList))
				{
					m_vecWordsList.at(*iter).bUsedFlag=true;
					m_vecWordsList.at(*iter).bValidFlag=true;
					m_questionIdList.setKeyAndId(m_vecWordsList.at(*iter).vecSynonymWords, vecIdList);
					break;
				}
				else
					m_vecWordsList.at(*iter).bUsedFlag=true;
			}
			if(vecIdList.empty())
			{
				goto NEXTSEARCH;
			}
		}
		
		if(vecIdList.empty())//实在是找不到
		{
			return false;
		}
		
		//将剩下的词在检索的集合里进行筛选
		vecIdList.clear();
		
		for(vector<int>::iterator iter=vecCurrentPriorityWordsIdx.begin(); iter!=vecCurrentPriorityWordsIdx.end(); iter++)
		{
			if(m_vecWordsList.at(*iter).bUsedFlag)
				continue;
			vecIdListTmp.clear();
			vecIdListTmp = m_questionIdList.getResultIdListLastOne();
			if(m_QADB.questionIdList(m_vecWordsList.at(*iter).vecSynonymWords, vecIdListTmp, vecIdList))
			{
				m_vecWordsList.at(*iter).bUsedFlag=true;
				m_vecWordsList.at(*iter).bValidFlag=true;
				m_questionIdList.setKeyAndId(m_vecWordsList.at(*iter).vecSynonymWords, vecIdList);
			}
			else
				m_vecWordsList.at(*iter).bUsedFlag=true;
			
		}
	}
	

	//没有检查到，退出
	vecIdList=m_questionIdList.getResultIdListLastOne();
	if(vecIdList.empty())
	{
		return false;
	}

	//将剩下的词，依次对集合vecIdList进行筛选
	vecIdList.clear();
	
	
	for(vector<struWordList>::iterator iter=m_vecWordsList.begin(); iter != m_vecWordsList.end(); iter++)
	{
		if(iter->bUsedFlag)
			continue;
		vecIdListTmp.clear();
		vecIdListTmp = m_questionIdList.getResultIdListLastOne();
		if(m_QADB.questionIdList(iter->vecSynonymWords, vecIdListTmp, vecIdList))
		{
			iter->bUsedFlag=true;
			iter->bValidFlag=true;
			m_questionIdList.setKeyAndId(iter->vecSynonymWords, vecIdList);
		}
		else
		{
			iter->bUsedFlag=true;
		}
	}
	
	//如果vecIdLis还是有多个值，则用上个Q保存的词继续筛选
	vecIdList=m_questionIdList.getResultIdListLastOne();
	if(vecIdList.size()==1)
		return true;
	PrintI("\nm_vecWordsListLastQA.size=%d\n", m_vecWordsListLastQA.size());
	
	vecCurrentPriorityWordsIdx.clear();
	if(!getFirstPriorityLastQA(vecCurrentPriorityWordsIdx))
		return true; 
	
	vecIdList.clear();
	do
	{
		for(vector<int>::iterator iter=vecCurrentPriorityWordsIdx.begin(); iter!=vecCurrentPriorityWordsIdx.end(); iter++)
		{
			if(m_vecWordsListLastQA.at(*iter).bUsedFlag)
				continue;
			vecIdListTmp.clear();
			vecIdListTmp = m_questionIdList.getResultIdListLastOne();
			if(m_QADB.questionIdList(m_vecWordsListLastQA.at(*iter).vecSynonymWords, vecIdListTmp, vecIdList))
			{
				m_vecWordsListLastQA.at(*iter).bUsedFlag=true;
				m_vecWordsListLastQA.at(*iter).bValidFlag=true;
				m_questionIdList.setKeyAndId(m_vecWordsListLastQA.at(*iter).vecSynonymWords, vecIdList);
				PrintI("word=%s\n", m_vecWordsListLastQA.at(*iter).vecSynonymWords.at(0).c_str());
			}
			else
			{
				m_vecWordsListLastQA.at(*iter).bUsedFlag=true;
			}
		}
	}
	while(getNextPriorityLastQA(vecCurrentPriorityWordsIdx));
	
	vecIdList.clear();
	vecIdList=m_questionIdList.getResultIdListLastOne();
	

	return !vecIdList.empty();
}

//把输入词list的每一个词与句子比较，返回词匹配的总字数与原句的比值,由于句子没有切词，比值定在0.5以上
float CCompanyQANormal::getMatchCount(string &sentence)
{
	float fMatchLength=0.0;
	float fSentenceLength=0;

	//先判断是否由@组成的字符串
	if(sentence.find("@")!=string::npos)
	{
		return getMatchCountForLTP(sentence);
	}
	
	//将输入句子切词
	CSegmantation *pSegmantation = CSegmantation::GetInstance();
	if(!pSegmantation->AnalyzeSentenceSegmentation(sentence.c_str()))
	{
		return 0.0;

	}
/*	
	CSemantic *pSemantic = CSemantic::GetInstance();
	if(!pSemantic->Parser(pSegmantation))
	{
		return 0.0;
	}
	CINSemanticInfos *tSemantic = 0;
	tSemantic = new(nothrow) CINSemanticInfos(pSemantic);
	if (tSemantic == 0)
	{
		return 0.0;
	}
*/
	//pSegmantation->m_vWordList;
	CCompanyWordsOperate wo;
	vector<struWordList> vecWordsListT;
	bool bNounChecked=false;
	vecWordsListT = wo.getWordsList(pSegmantation);

	//得到句子的分类
	if(m_iSentenceType!=0)
	{
		if(m_iSentenceType != getSentenceBayesType(vecWordsListT))
			return 0.0;
	}
		
	for(vector<struWordList>::iterator iter = vecWordsListT.begin(); iter != vecWordsListT.end(); iter++)
	{
		fSentenceLength+= iter->vecSynonymWords.at(0).length();
	}
	
	for(vector<struWordList>::iterator iter=m_vecWordsList.begin(); iter!=m_vecWordsList.end(); iter++)
	{
		if(!iter->bValidFlag)
			continue;
		
		for(vector<string>::iterator iter1=iter->vecSynonymWords.begin(); iter1!=iter->vecSynonymWords.end();iter1++)
		{
			for(vector<struWordList>::iterator iterTempWord = vecWordsListT.begin(); iterTempWord != vecWordsListT.end(); iterTempWord++ )
			{
				if(iterTempWord->bUsedFlag)
					continue;
				if(iterTempWord->vecSynonymWords.at(0).find(*iter1)==0)
				{
					iterTempWord->bUsedFlag = true;
					fMatchLength += iter1->length();
					if(iterTempWord->iWordType ==COMPANY_WORD_NOUN)
						bNounChecked=true;
					
					break;
				}
			}
			
		}

	}
	if(bNounChecked)
	{
		return fMatchLength / fSentenceLength+0.3;
	}
	return fMatchLength / fSentenceLength;
}

//匹配度计算 限于由依存处理的Q
float CCompanyQANormal::getMatchCountForLTP(string &sSentence)
{
	//判断是否由@组成的字符串 '2@公安|0@机关|2@委托|2@有|1@资质'
	if(sSentence.find('@')==string::npos)
		return 0.0;

	//将字符串转化成结构列表
	CKGPublic kgp;
	vector<struLTPWordList> vecLTPWords;
	vector<string> vecSplitStr;
	float fMatchLength=0.0;
	if(kgp.kg_split(sSentence, vecSplitStr, "|")==0)
		return 0.0;

	for(vector<string>::iterator iter=vecSplitStr.begin(); iter!=vecSplitStr.end(); iter++)
	{
		vector<string> vecSplitStr1;
		if(kgp.kg_split(*iter, vecSplitStr1, "@")==0)
			continue;
		if(vecSplitStr1.size()<2)
			continue;
		struLTPWordList ltpWord;
		ltpWord.iWordType = atoi(vecSplitStr1.at(0).c_str());
		ltpWord.sWord = vecSplitStr1.at(1);
		ltpWord.bMatchingFlag=false;
		vecLTPWords.push_back(ltpWord);
	}

	//和原句进行词匹配
	for(vector<struWordList>::iterator iter=m_vecWordsList.begin(); iter!=m_vecWordsList.end(); iter++)
	{
		//if(!iter->bValidFlag)
		//	continue;
		for(vector<struLTPWordList>::iterator iterLtp=vecLTPWords.begin(); iterLtp!=vecLTPWords.end(); iterLtp++)
		{
			if(iterLtp->bMatchingFlag)
				continue;
			
			if(iter->vecSynonymWords.at(0).compare(iterLtp->sWord)==0)
			{
				iterLtp->bMatchingFlag=true;
			}
		}
	}

	//对匹配的结果进行打分
	for(vector<struLTPWordList>::iterator iterLtp=vecLTPWords.begin(); iterLtp!=vecLTPWords.end(); iterLtp++)
	{
		if(iterLtp->bMatchingFlag==false)
		  continue;
		switch(iterLtp->iWordType)
		{
			case 0:
				fMatchLength+=0.3;
				break;
			case 1:
				fMatchLength+=0.2;
				break;
			case 2:
				fMatchLength+=0.1;
				break;
			default:
				fMatchLength+=0.0;
		}
	}

	return fMatchLength;
}

bool CCompanyQANormal::getGroupParam(int iQuestionId, struMediaUrl &medialUrl)
{
	
    vector<string> vecGroupParam;
	
	if(!m_QADB.getGroupParamByQuestionId(iQuestionId, vecGroupParam))
	{
		return false;
	}

	medialUrl.sType=vecGroupParam.at(0);
	medialUrl.sAudio=vecGroupParam.at(1);
	medialUrl.sIsEnd=vecGroupParam.at(2);
	medialUrl.sIsBreak=vecGroupParam.at(3);
	medialUrl.sLoop=vecGroupParam.at(4);
	medialUrl.sAffirm=vecGroupParam.at(5);

	return true;
}

//保存question的热度
bool CCompanyQANormal::setQuestionFrequency(int iQuestionId)
{
	//记录下当前question的热度
	return m_QADB.setCompanyQuestionFrequency(iQuestionId);

}

string CCompanyQANormal::getAnswerByQuestionId(int iQuestionId)
{
	string sAnswer="";
	if(!m_QADB.getAnswerByQuestionId(iQuestionId, sAnswer))
		return "";

	//回答的字符串中是否有''@'' 特殊处理标识
	if(sAnswer.at(0)!='@')
	{
		return sAnswer;
	}
	
	return "";
}


int  CCompanyQANormal::compareQuestion(string sQuestion)
{
	int iQuestionId=0;
	if(!m_QADB.getAnswerByMatchQuestion(sQuestion, iQuestionId))
		return 0;
	return iQuestionId;
}

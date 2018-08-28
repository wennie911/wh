#include "CompanyWordsOperate.h"
#include "../datalibs_management/Synonym.h"


CCompanyWordsOperate::CCompanyWordsOperate()
{
}

CCompanyWordsOperate::~CCompanyWordsOperate()
{
}

vector<struWordList> CCompanyWordsOperate::getWordsList(CINSemanticInfos * pSemantic)
{
	m_pSemantic = pSemantic;

	setPosList();
	
	return m_vecWordsList;
}

vector<struWordList> CCompanyWordsOperate::getWordsList(CSegmantation * pSemantic)
{
	m_pSemanticPrivate = pSemantic;

	setPosListPrivate();
	
	return m_vecWordsList;
}

bool CCompanyWordsOperate::setPosListPrivate()
{
	//CSynonymDB *sy = CSynonymDB::GetInstance();
	m_vecWordsList.clear();
	int nCharLen=0;
#ifdef WIN32
	nCharLen = 2;
#else
	nCharLen = 3;
#endif
	//只保留名词, 动词, 形容词, 数词,英语
	for(int i=0; i < m_pSemanticPrivate->m_vWordList.size(); i++)
	{
		int iLevel=5;
		CWord *pWord = m_pSemanticPrivate->m_vWordList.at(i);
		struWordList wl;
		set<string> vsvalues;
		int iWordProperty = pWord->m_nPos[0];

		wl.bUsedFlag=false;
		wl.bValidFlag=false;
		if(isNounWord(iWordProperty))
		{
			wl.iWordType = COMPANY_WORD_NOUN;
			if(iWordProperty == RA_WORD_J || iWordProperty == RA_WORD_EN)
				wl.iWordProperty = RA_WORD_NOUN;
			else
			    wl.iWordProperty = iWordProperty;
		}
		else if(isVerbWord(iWordProperty))
		{
			wl.iWordType = COMPANY_WORD_VERB;
			wl.iWordProperty = iWordProperty;
		}
		else if(isAdjWord(iWordProperty))
		{
			wl.iWordType = COMPANY_WORD_ADJ;
			wl.iWordProperty = iWordProperty;
		}
		else if(isQuanWord(iWordProperty))
		{
			wl.iWordType = COMPANY_WORD_QUAN;
			wl.iWordProperty = iWordProperty;
			wl.vecSynonymWords.push_back(pWord->m_szValue);
			m_vecWordsList.push_back(wl);
			continue;
		}
		else if(isPronounWord(iWordProperty))
		{
			wl.iWordType = COMPANY_WORD_PRONOUN;
			wl.iWordProperty = iWordProperty;
			wl.vecSynonymWords.push_back(pWord->m_szValue);
			m_vecWordsList.push_back(wl);
			continue;
		}
		else if(isPrepositionWord(iWordProperty))
		{	
			wl.iWordType = COMPANY_WORD_PREPOSITION;
			wl.iWordProperty = iWordProperty;
			wl.vecSynonymWords.push_back(pWord->m_szValue);
			m_vecWordsList.push_back(wl);
			continue;
		}
		else if(isConjunctionWord(iWordProperty))
		{
			wl.iWordType = COMPANY_WORD_CONJUNCTION;
			wl.iWordProperty = iWordProperty;
			wl.vecSynonymWords.push_back(pWord->m_szValue);
			m_vecWordsList.push_back(wl);
			continue;
		}
		else if(isAdverbWord(iWordProperty))
		{
			wl.iWordType = COMPANY_WORD_ADVERB;
			wl.iWordProperty = iWordProperty;
			wl.vecSynonymWords.push_back(pWord->m_szValue);
			m_vecWordsList.push_back(wl);
			continue;
		}
		else if(isEnglishWord(iWordProperty))
		{
			wl.iWordType = COMPANY_WORD_EN;
			wl.iWordProperty = iWordProperty;
			wl.vecSynonymWords.push_back(pWord->m_szValue);
			m_vecWordsList.push_back(wl);
			continue;
		}
		else
		{
			continue;
		}
		
		wl.vecSynonymWords.push_back(pWord->m_szValue);
		m_vecWordsList.push_back(wl);
		   
	}
	return !m_vecWordsList.empty();
}

bool CCompanyWordsOperate::setPosList()
{
	CSynonymDB *sy = CSynonymDB::GetInstance();
	m_vecWordsList.clear();
	int nCharLen=0;
#ifdef WIN32
	nCharLen = 2;
#else
	nCharLen = 3;
#endif
	//只保留名词, 动词, 形容词, 数词,英语
	for(int i=0; i < m_pSemantic->m_vSWords.size(); i++)
	{
		int iLevel=5;
		SWord *pWord = m_pSemantic->m_vSWords.at(i);
		struWordList wl;
		set<string> vsvalues;
		int iWordProperty = pWord->m_nPos[0];

		wl.bUsedFlag=false;
		wl.bValidFlag=false;
		if(isNounWord(iWordProperty))
		{
			wl.iWordType = COMPANY_WORD_NOUN;
			if(iWordProperty == RA_WORD_J || iWordProperty == RA_WORD_EN)
				wl.iWordProperty = RA_WORD_NOUN;
			else
			    wl.iWordProperty = iWordProperty;
		}
		else if(isVerbWord(iWordProperty))
		{
			wl.iWordType = COMPANY_WORD_VERB;
			wl.iWordProperty = iWordProperty;
		}
		else if(isAdjWord(iWordProperty))
		{
			wl.iWordType = COMPANY_WORD_ADJ;
			wl.iWordProperty = iWordProperty;
		}
		else if(isQuanWord(iWordProperty))
		{
			wl.iWordType = COMPANY_WORD_QUAN;
			wl.iWordProperty = iWordProperty;
			wl.vecSynonymWords.push_back(pWord->m_szValue);
			m_vecWordsList.push_back(wl);
			continue;
		}
		else if(isPronounWord(iWordProperty))
		{
			wl.iWordType = COMPANY_WORD_PRONOUN;
			wl.iWordProperty = iWordProperty;
			wl.vecSynonymWords.push_back(pWord->m_szValue);
			m_vecWordsList.push_back(wl);
			continue;
		}
		else if(isPrepositionWord(iWordProperty))
		{	
			wl.iWordType = COMPANY_WORD_PREPOSITION;
			wl.iWordProperty = iWordProperty;
			wl.vecSynonymWords.push_back(pWord->m_szValue);
			m_vecWordsList.push_back(wl);
			continue;
		}
		else if(isConjunctionWord(iWordProperty))
		{
			wl.iWordType = COMPANY_WORD_CONJUNCTION;
			wl.iWordProperty = iWordProperty;
			wl.vecSynonymWords.push_back(pWord->m_szValue);
			m_vecWordsList.push_back(wl);
			continue;
		}
		else if(isAdverbWord(iWordProperty))
		{
			wl.iWordType = COMPANY_WORD_ADVERB;
			wl.iWordProperty = iWordProperty;
			wl.vecSynonymWords.push_back(pWord->m_szValue);
			m_vecWordsList.push_back(wl);
			continue;
		}
		else if(isEnglishWord(iWordProperty))
		{
			wl.iWordType = COMPANY_WORD_EN;
			wl.iWordProperty = iWordProperty;
			wl.vecSynonymWords.push_back(pWord->m_szValue);
			m_vecWordsList.push_back(wl);
			continue;
		}
		else
		{
			continue;
		}
		
		if (pWord->isUsefullWord())
		{
			if (false)//(pWord->m_vszSynonymMarks.size() > 0)
			{
				wl.vecSynonymWords.clear();
				sy->GetSynonymWords(pWord->m_vszSynonymMarks, vsvalues, iLevel);
				if (vsvalues.size() == 0)
				{
					wl.vecSynonymWords.push_back(pWord->m_szValue);
				
				}
				else
				{
					wl.vecSynonymWords.clear();
					set<string>::iterator iter=vsvalues.begin(); 
					//只存放前100个
					int i=0;
					wl.vecSynonymWords.push_back(pWord->m_szValue);
					while(iter!=vsvalues.end())
					{
						if(pWord->m_szValue.compare(*iter)!=0)
						{
							wl.vecSynonymWords.push_back(*iter);
						}
						++iter;
						if(i<100)
						{
							i++;
						}
						else
							break;
					}
				}
					m_vecWordsList.push_back(wl);
			}
			else
			{
				wl.vecSynonymWords.push_back(pWord->m_szValue);
				m_vecWordsList.push_back(wl);
					
			}
		}
		else
		{
			wl.vecSynonymWords.push_back(pWord->m_szValue);
			m_vecWordsList.push_back(wl);
		}

			
			   
	}
	return !m_vecWordsList.empty();
}

//名词
bool CCompanyWordsOperate::isNounWord(int iWordProperty)
{
	if( iWordProperty >= RA_WORD_NOUN_U && iWordProperty <= RA_WORD_NOUN_END)
		return true;
	if( iWordProperty == RA_WORD_J)
		return true;
	if( iWordProperty == RA_WORD_EN)
		return true;
	return false;
}

//动词
bool CCompanyWordsOperate::isVerbWord(int iWordProperty)
{
	if((iWordProperty >= RA_WORD_VERB && iWordProperty <= RA_WORD_VERB_A_END) ||
		(iWordProperty >= RA_WORD_VERB_T && iWordProperty <= RA_WORD_VERB_END) ||
		(iWordProperty >= RA_WORD_VERB_AUX && iWordProperty <= RA_WORD_VERB_AUX_END))
		return true;
	return false;
}

//形容词
bool CCompanyWordsOperate::isAdjWord(int iWordProperty)
{
	if( iWordProperty >= RA_WORD_ADJ && iWordProperty <= RA_WORD_ADJ_END)
		return true;
	return false;
}

//数量词
bool CCompanyWordsOperate::isQuanWord(int iWordProperty)
{
	if( iWordProperty >= RA_WORD_QUAN && iWordProperty <= RA_WORD_QUAN_END)
	{
		if(iWordProperty == RA_WORD_CLASSIFIER_U)
			return false;
		return true;
	}
	return false;
}

//代词 疑问代词
bool CCompanyWordsOperate::isPronounWord(int iWordProperty)
{
	if( iWordProperty >= RA_WORD_PRONOUN_Q && iWordProperty <= RA_WORD_PRONOUN_Q_END)
		return true;
	return false;
}

//连词  把助词也放在这里
bool CCompanyWordsOperate::isConjunctionWord(int iWordProperty)
{
	if( iWordProperty == RA_WORD_CONJUNCTION)
		return true;
	if( iWordProperty >= RA_WORD_PARTICLE && iWordProperty <= RA_WORD_PARTICLE_END)
		return true;
	return false;
}

//介词
bool CCompanyWordsOperate::isPrepositionWord(int iWordProperty)
{
	if( iWordProperty == RA_WORD_PREPOSITION)
		return true;
	return false;
}


//副词
bool CCompanyWordsOperate::isAdverbWord(int iWordProperty)
{
	if( iWordProperty >= RA_WORD_ADVERB && iWordProperty <= RA_WORD_ADVERB_END)
		return true;
	return false;
}

//英文词
bool CCompanyWordsOperate::isEnglishWord(int iWordProperty)
{
	if( iWordProperty == RA_WORD_EN)
		return true;
	return false;
}

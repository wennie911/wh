#pragma once

#include <set>
#include "CompanyPublic.h"
#include "../baisic_info/InSematicInfos.h"

class CCompanyWordsOperate
{
	public:
		CCompanyWordsOperate();
		~CCompanyWordsOperate();
	public:
		vector<struWordList> getWordsList(CINSemanticInfos * pSemantic);
		vector<struWordList> getWordsList(CSegmantation * pSemantic);
	private:
		CINSemanticInfos *m_pSemantic; //输入
		CSegmantation * m_pSemanticPrivate;//私有使用
		vector<struWordList> m_vecWordsList;

		bool setPosList();
		bool setPosListPrivate();
		bool isNounWord(int iWordProperty);
		bool isVerbWord(int iWordProperty);
		bool isAdjWord(int iWordProperty);
		bool isQuanWord(int iWordProperty);
		bool isPronounWord(int iWordProperty);
		bool isConjunctionWord(int iWordProperty);
		bool isPrepositionWord(int iWordProperty);
		bool isAdverbWord(int iWordProperty);
		bool isEnglishWord(int iWordProperty);
};

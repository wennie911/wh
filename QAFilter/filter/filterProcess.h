#ifndef _H_FILTER_PROCESS_
#define _H_FILTER_PROCESS_

/*
zcq
初始化各种参数，nlu动态库
处理过滤器
*/
#include "filterPublic.h"
#include "callNLUSO.h"
#include "LogicLayer.h"
#include "qaList.h"
#include "../commonlib/tinyxml.h"


class CFilterProcess
{
	public:
		CFilterProcess();
		~CFilterProcess();

	public:
		bool init();
		bool processInSentence(string &sInSentence, string &sOutSentence );

	private:
		CCallNlUSo m_so_nlu;
		vector<CLogicLayer> m_vecLoginLayer; 
		CNLUQAList m_nluQaList;
		int m_currentLayerId;
		bool m_bSessionEnd;
		struNLUQA m_retQA;

		//读取过滤器配置xml
		bool readFilterConfig(string sXMLPath);
		bool readNLUSoConfig(string sXMLPath, string &sDllPath, string &sDataPath, int &iLogLevle);
		string getXmlAttribute(TiXmlElement *pNode, const string sAttributeName);

		CLogicLayer * getLayer();
		string getLastAnswer(int iLayerId);
		string getLastAudio(int iLayerId);
		void setLayerAnswerAndAudio(int iLayerId,string &sAnswer, string &sAudio);

		void setRetAnswer(struNLUQA &oneQA);
		void setRetAnswer(string &sAnswer, string &sAudio, int iIsEnd, string &sQuestion, string &sMatchedQuestion, string &sQuestionPinYin, string &sType);

		void initQAStru(struNLUQA &oneQA);

		int splitStr(const string &str, vector<string> &ret, string sep);

		bool layerProcess();
};
#endif

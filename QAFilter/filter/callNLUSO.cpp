#include <dlfcn.h>

#include "callNLUSO.h"
#include "../json/json_reader.h"


typedef int (*nlu_init)(const char *);
typedef int (*nlu_answer)(const char *, char *, const int, char *, const int, char *, const int);
typedef void (*nlu_log_level)(int);

nlu_init func_nlu_init;
nlu_answer func_nlu_answer;
nlu_log_level func_nlu_loglevel;


CCallNlUSo::CCallNlUSo()
{
	m_sDLLPath = "";
	m_sDataPath = "";
	m_iLogLevel = 3;

}

CCallNlUSo::~CCallNlUSo()
{
	CloseSo();
}

bool CCallNlUSo::LoadSo(string &sDllPath, string &sDataPath, int iLogLevel)
{
	m_sDLLPath = sDllPath;
	m_sDataPath = sDataPath;
	m_iLogLevel = iLogLevel;
	
	if(m_sDLLPath.empty() || m_sDataPath.empty() || iLogLevel ==0)
		return false;
	
	m_handle = dlopen(m_sDLLPath.c_str(), RTLD_NOW);
	if(NULL == m_handle)
	{
		printf("Load ai module error! %s!\n", dlerror());
		return false;
	}

	func_nlu_init = (nlu_init)dlsym(m_handle, "nlu_init");
	func_nlu_answer = (nlu_answer)dlsym(m_handle, "nlu_push_text");
	func_nlu_loglevel = (nlu_log_level)dlsym(m_handle, "set_log_level");

	if( (NULL == func_nlu_init) || (NULL == func_nlu_answer )  ||  (NULL == func_nlu_loglevel ))
	{
		printf("Load ai interface error! %s!\n", dlerror());
		return false;
	}

	setLogLevel(m_iLogLevel);
	initNLU(m_sDataPath);
	
	return true;
}

void CCallNlUSo::CloseSo()
{
	dlclose(m_handle);
}

void CCallNlUSo::initNLU(string & sDataPath)
{
	func_nlu_init(sDataPath.c_str());
}

void CCallNlUSo::setLogLevel(int iLevel)
{
	func_nlu_loglevel(iLevel);
}

bool CCallNlUSo::parseRetJson(string sJson, vector<struNLUQA> &nluQAList)
{
	Json::Reader reader;
	Json::Value root;

	if(!reader.parse(sJson, root))
		return false;
	
	nluQAList.clear();
	int iQuestionListSize = root.size();
	
	for(int i=0; i < iQuestionListSize; i++)
	{
		struNLUQA uqa;
		uqa.sQuestion = root[i]["question"].asString();
		uqa.sAnswer = root[i]["answer"].asString();
		uqa.sMatchedQuestion = root[i]["matchedQuestion"].asString();
		uqa.sAudio = root[i]["audio"].asString();
		uqa.sType = root[i]["type"].asString();
		uqa.sQuestionPinYin = root[i]["questionPinYin"].asString();
		uqa.iIsEnd = atoi(root[i]["isEnd"].asString().c_str());
		uqa.iIsBreak = atoi(root[i]["isBreak"].asString().c_str());
		uqa.iLoop = atoi(root[i]["loop"].asString().c_str());
		uqa.iAffirm = atoi(root[i]["affirm"].asString().c_str());
		uqa.iIsDel =0;
		nluQAList.push_back(uqa);
	}

	return !nluQAList.empty();
}

bool CCallNlUSo::getAnswer(string &sInSentence, vector<struNLUQA> &vecNluQAList)
{
	memset(m_sAnswer, 0, BUFSTRINGLEN);
	memset(m_sCommand, 0, BUFSTRINGLEN);
	memset(m_sPrint, 0, BUFSTRINGLEN);
	
	if(!func_nlu_answer(sInSentence.c_str(), m_sAnswer, BUFSTRINGLEN, m_sCommand, BUFSTRINGLEN, m_sPrint, BUFSTRINGLEN))
	{
		return false;
	}
	return parseRetJson(m_sAnswer, vecNluQAList);
}



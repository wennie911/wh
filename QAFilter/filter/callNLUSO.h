#ifndef _H_CALLNLUSO_
#define _H_CALLNLUSO_

/*
zcq
调用nlu.so
读取并设置参数
解析返回的qa数据（json）
*/
#include "filterPublic.h"

#include <iostream>
#include <vector>
#include <string>


using namespace std;

class CCallNlUSo
{
	public:
		CCallNlUSo();
		~CCallNlUSo();
	public:
		//打开nlu.so
		bool LoadSo();
		//返回nlu的调用结构
		bool getAnswer(string &sInSentence, vector<struNLUQA> &vecNluQAList);
	private:
		char m_sAnswer[BUFSTRINGLEN];
		char m_sCommand[BUFSTRINGLEN];
		char m_sPrint[BUFSTRINGLEN];

		string m_sDLLPath;
		string m_sDataPath;
		int m_iLogLevel;

		void *m_handle;
		
		
		
		//关闭nlu.so
		void CloseSo();
		//调用nlu接口
		//初始化nlu
		void initNLU(string &sDataPath);
		//设置日志级别
		void setLogLevel(int iLevel);
		//读取配置文件
		//解析返回的字符串
		bool parseRetJson(string sJson, vector<struNLUQA> &nluQAList);
		
};
#endif

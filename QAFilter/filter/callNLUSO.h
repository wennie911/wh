#ifndef _H_CALLNLUSO_
#define _H_CALLNLUSO_

/*
zcq
����nlu.so
��ȡ�����ò���
�������ص�qa���ݣ�json��
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
		//��nlu.so
		bool LoadSo();
		//����nlu�ĵ��ýṹ
		bool getAnswer(string &sInSentence, vector<struNLUQA> &vecNluQAList);
	private:
		char m_sAnswer[BUFSTRINGLEN];
		char m_sCommand[BUFSTRINGLEN];
		char m_sPrint[BUFSTRINGLEN];

		string m_sDLLPath;
		string m_sDataPath;
		int m_iLogLevel;

		void *m_handle;
		
		
		
		//�ر�nlu.so
		void CloseSo();
		//����nlu�ӿ�
		//��ʼ��nlu
		void initNLU(string &sDataPath);
		//������־����
		void setLogLevel(int iLevel);
		//��ȡ�����ļ�
		//�������ص��ַ���
		bool parseRetJson(string sJson, vector<struNLUQA> &nluQAList);
		
};
#endif

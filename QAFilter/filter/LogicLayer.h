#ifndef _H_LOGICLAYER_
#define _H_LOGICLAYER_
/*
zcq
�߼��� ��������Ͽ��Դ���Ự���߼�

*/
#include <vector>
#include <string>

#include "filterPublic.h"

using namespace std;

class CLogicLayer
{
	public:
		CLogicLayer();
		~CLogicLayer();
	public:
		void setLayer(struLayer &layer);
		int getLayerId();
		//�����Ѿ��ش��answer
		bool setAnswer(string &sAnswer);
		string getAnswer();
		bool setAudio(string &sAudio);
		string getAudio();
		//�ж�������Ƿ��Ѿ��ù�
		bool layerIsUsed();
		void setUsed(bool bUsedFlag);
		//�õ�©����б�
		vector<string> getLeakList();
		//�õ�ȷ����ȱʡ����
		string getDefQuestion();
		//�õ��ܾ���ȱʡ����
		string getDenyQuestion();
		
	private:
		struLayer m_layer;
		
};
#endif

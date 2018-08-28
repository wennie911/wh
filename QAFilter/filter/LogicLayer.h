#ifndef _H_LOGICLAYER_
#define _H_LOGICLAYER_
/*
zcq
逻辑层 在这个层上可以处理会话的逻辑

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
		//设置已经回答的answer
		bool setAnswer(string &sAnswer);
		string getAnswer();
		bool setAudio(string &sAudio);
		string getAudio();
		//判断这个层是否已经用过
		bool layerIsUsed();
		void setUsed(bool bUsedFlag);
		//得到漏点的列表
		vector<string> getLeakList();
		//得到确定的缺省问题
		string getDefQuestion();
		//得到拒绝的缺省问题
		string getDenyQuestion();
		
	private:
		struLayer m_layer;
		
};
#endif

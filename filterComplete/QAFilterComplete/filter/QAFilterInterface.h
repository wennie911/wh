#ifndef _H_QAFILTER_INTERFACE_
#define _H_QAFILTER_INTERFACE_

/*
zcq
这个动态库实现对会话的逻辑处理
并调用NLU的动态库，处理qa的匹配
同时会读取相关配置文件
*/
#include "filterPublic.h"


extern "C" {
	int qaf_init(const char *sDllDir);

	//输入question 返回answer和answer的声音文件 json
	int qaf_get_answer(const char *in_sentence, char *out_sentence, int out_sentence_size);

	//相关的状态接口
	
	//有效会话次数
	int getValidSession();
	//是否有聊天
	int isChatInSession();
	//是否有咨询
	int isConsultedInSession();
	//是否挽回过
	int isRedeemed();
	//是否拒绝
	int isRefused();
	//是否在忙
	int isBusy();
	//是否成功
	int isSuccesed();
	//匹配关键字标志 A B C D
	
}

#endif

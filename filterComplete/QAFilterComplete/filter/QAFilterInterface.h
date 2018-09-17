#ifndef _H_QAFILTER_INTERFACE_
#define _H_QAFILTER_INTERFACE_

/*
zcq
�����̬��ʵ�ֶԻỰ���߼�����
������NLU�Ķ�̬�⣬����qa��ƥ��
ͬʱ���ȡ��������ļ�
*/
#include "filterPublic.h"


extern "C" {
	int qaf_init(const char *sDllDir);

	//����question ����answer��answer�������ļ� json
	int qaf_get_answer(const char *in_sentence, char *out_sentence, int out_sentence_size);

	//��ص�״̬�ӿ�
	
	//��Ч�Ự����
	int getValidSession();
	//�Ƿ�������
	int isChatInSession();
	//�Ƿ�����ѯ
	int isConsultedInSession();
	//�Ƿ���ع�
	int isRedeemed();
	//�Ƿ�ܾ�
	int isRefused();
	//�Ƿ���æ
	int isBusy();
	//�Ƿ�ɹ�
	int isSuccesed();
	//ƥ��ؼ��ֱ�־ A B C D
	
}

#endif

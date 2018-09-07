#include "ai.h"
#include "json.h"
#include "esl_control.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <memory.h>
#include <curl/curl.h>

extern char keyword_list[100][32];
extern char keyword_list_pingyin[100][32];

typedef int (*qaf_init)(const char *);
typedef int (*qaf_get_answer)(const char *, char *, const int);

static void *ai_handle = NULL;
qaf_init func_ai_init = NULL;
qaf_get_answer func_ai_answer = NULL;

static char printbuf[1024];

static pthread_mutex_t ai_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutexattr_t ai_attr;
static const char ai_url[] = {"http://10.50.10.103:8562/nlu"}; 

typedef struct _ai_response
{
	int res_len;
	char res_data[256];
}response;

const char *ai_get_info()
{
	return &printbuf[0];
}

int load_ai_module(const char *module_pathname)
{
	ai_handle = dlopen(module_pathname, RTLD_NOW);
	if( NULL == ai_handle )
	{
		sprintf(printbuf, "Load ai module error! %s!\n", dlerror());
		return 0;
	}

	return 1;
}

int load_ai_interface()
{
	func_ai_init = dlsym(ai_handle, "qaf_init");
	func_ai_answer = dlsym(ai_handle, "qaf_get_answer");
	
	if( (NULL == func_ai_init) || (NULL == func_ai_init ))
	{
		sprintf(printbuf, "Load ai interface error! %s!\n", dlerror());
		return 0;
	}

	return 1;
}

int ai_initialize(const char *module_pathname, const char *work_dir)
{
	if( !load_ai_module(module_pathname) )
		return 0;

	if( !load_ai_interface() )
		return 0;


	if( !func_ai_init(work_dir) )
		return 0;

	//从文件里取需要匹配的关键词
	getKeyWordsFile("./keyword.txt");

	return 1;
}

void ai_uninit()
{
	if(  NULL != ai_handle )
	{
		dlclose(ai_handle);
		ai_handle = NULL;
	}
}

int get_response_info(char *sjson, struRetAnswer *retAnswerPtr)
{
	cJSON *json;
	int nret = 0;
	printf("get_response_info sJson=%s\n",sjson);
	json = cJSON_Parse(sjson);
	if (json)
	{
		cJSON *item=cJSON_GetObjectItem(json, "question"); 
		strcpy(retAnswerPtr->sQuestion, item->valuestring);

		item=cJSON_GetObjectItem(json, "answer"); 
		strcpy(retAnswerPtr->sAnswer, item->valuestring);

		item=cJSON_GetObjectItem(json, "matchedQuestion"); 
		strcpy(retAnswerPtr->sMatchedQuestion, item->valuestring);

		item=cJSON_GetObjectItem(json, "audio"); 
		strcpy(retAnswerPtr->sAudio, item->valuestring);

		item=cJSON_GetObjectItem(json, "questionPinYin"); 
		strcpy(retAnswerPtr->sQuestionPinYin, item->valuestring);

		item=cJSON_GetObjectItem(json, "type"); 
		strcpy(retAnswerPtr->sType, item->valuestring);

		item=cJSON_GetObjectItem(json, "isBreak"); 
		retAnswerPtr->iIsBreak = item->valueint;

		item=cJSON_GetObjectItem(json, "isEnd"); 
		retAnswerPtr->iIsEnd = item->valueint;

		item=cJSON_GetObjectItem(json, "dialogeCount"); 
		retAnswerPtr->iDialogeCount = item->valueint;

		item=cJSON_GetObjectItem(json, "currentLayer"); 
		retAnswerPtr->iCurrentLayer = item->valueint;

		item=cJSON_GetObjectItem(json, "totalLayer"); 
		retAnswerPtr->iTotalLayer = item->valueint;
		printf("sAudio=%s\n", retAnswerPtr->sAudio);
		nret = 1;
	}

	cJSON_Delete(json);

	return nret;
}

//调用动态库接口 将返回的json进行处理
int ai_get_answer(char * sAnswer, struRetAnswer *retAnswerPtr)
{
	if(strlen(sAnswer)<=0)
		return 0;
	char retAnswerJson[MAXANSWERLEN*20]={0};
	if(!func_ai_answer(sAnswer, retAnswerJson, sizeof(retAnswerJson)))
		return 0;

	if(!get_response_info(retAnswerJson, retAnswerPtr))
		return 0;

	if(strlen(retAnswerPtr->sAnswer)<=0)
		return 0;

	return 1;
}

void dojson(cJSON *json, char *text)
{
	cJSON *ws_node = cJSON_GetObjectItem(json, "ws");
	int size = cJSON_GetArraySize(ws_node);
	int i = 0;
	for( i = 0; i < size; ++i )
	{
		cJSON *ws_child_node = cJSON_GetArrayItem(ws_node, i);

		cJSON *cw_node = cJSON_GetObjectItem(ws_child_node, "cw");

		cJSON *cw_child_node = cJSON_GetArrayItem(cw_node, 0);

		cJSON *w_node = cJSON_GetObjectItem(cw_child_node, "w");

		strcat(text, w_node->valuestring);
	}
}

void dojsonlist(const char *text, char *words)
{
	char *out;
	cJSON *json;

	char* delim = "{\"sn\":";		//分隔符字符串
	char *p_reserve1 = NULL, *p_reserve2 = NULL;
	int len = 0;
	char *ajson = NULL;
	char *sub = NULL;
	int left_len = strlen(text);
	char* p = strstr(text, delim);	
	memset(words, '\0', sizeof(words));

	while(	p != NULL )
	{
		p_reserve1 = p;
		sub = text + (p - text) + 6;
		p = strstr(sub, delim);	
		p_reserve2 = p;

		if ( p_reserve2 != NULL && p_reserve1 != p_reserve2 )
		{
			len = p_reserve2 - p_reserve1;
			ajson = (char*)malloc(len + 1);
			memset(ajson, '\0', len + 1);
			text += len;
			left_len -= len;
			strncpy(ajson, p_reserve1, len);

			json = cJSON_Parse(ajson);
			if (!json)
			{
				set_tts_error("Parse json Error before: [%s]\n", cJSON_GetErrorPtr());
			}
			else
			{
				dojson(json, words);
				cJSON_Delete(json);
			}

			free(ajson);
		}
	}

	if( text != NULL )
	{
		ajson = (char*)malloc(left_len + 1);
		memset(ajson, '\0', left_len + 1);
		strncpy(ajson, text, left_len);

		json = cJSON_Parse(text);
		if (!json)
		{
			set_tts_error("Parse json Error before: [%s]\n", cJSON_GetErrorPtr());
		}
		else
		{
			dojson(json, words);
			cJSON_Delete(json);
		}

		free(ajson);
	}
}




int write_callback(void *ptr, size_t size, size_t num, void *usr_data)
{
	response *ai_res = (response*)usr_data;
	memcpy(ai_res->res_data, ptr, size * num);
	ai_res->res_len = num;

	return num;
}


//读取关键字文件
int getKeyWordsFile(char *sFilename)
{
	FILE *fp = fopen(sFilename, "r");
	char sBuf[MAXANSWERLEN];
	int iLen=0;
	int i=0;
	char sBufList[100][32]={0};

	memset(keyword_list,0,sizeof(keyword_list));
	memset(keyword_list_pingyin, 0, sizeof(keyword_list_pingyin));

	if(fp==NULL)
		return 0;

	while(fgets(sBuf, MAXANSWERLEN, fp)!=NULL)
	{
		iLen=strlen(sBuf);
		sBuf[iLen-1]='\0';
		strncpy(sBufList[i], sBuf,32);
		iLen=strlen(sBufList[i]);
		sBufList[i][iLen-1]='\0';
		i++;
	}

	fclose(fp);

	//把读取的内容导入关键字表
	//每行内容：活动@huo|dong
	char delim[]="@";

	for(i=0; i<100; i++)
	{
		char *p = strtok(sBufList[i], delim);
		if(p != NULL)
		{
			strcpy(keyword_list[i], p);
			p=strtok(NULL, delim);
			strcpy(keyword_list_pingyin[i], p);
		}
	}
}

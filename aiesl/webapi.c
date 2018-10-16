#include "webapi.h"
#include "code.h"
#include "./WebRtcMoudle/webrtc_vad.h"

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <curl/curl.h>
#include <esl.h>

#define	CHECK_LENGTH	160
extern const char *CONF_FILE_PATH;
static int nReadLen = CHECK_LENGTH / sizeof(short);			//per read length
static int nInitLen = CHECK_LENGTH * 10;					//init databuffer length

int	 FOS_LEN = 0;				//5
char keda_param[128] = {0};		//"{\"aue\":\"raw\",\"engine_type\":\"sms8k\",\"vad_eos\":\"500\"}";
char keda_api_key[128] = {0};	//"48782b8bc49449f2e4573225aeb7104c";
char keda_appid[32] = {0};		//"5b7fb8d7";
int	  nMaxDataLen = 0;			//1500000;				//max valid data len
int   nVadEos = 50;				// vad eos check times

static char sJobId[128] = {0};
static char sAudiofilePathname[256] = {0};
static char sLibcurlData[1024] = {0};
static size_t nShift = 0;
static VadInst* pVad = NULL;
static FILE* fp = NULL;
static int  nRun = 1;
static pthread_t tid;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int nDataFlag = 0;
static int nFosBlocksLen = 0;

typedef struct _node
{
	struct _node *prevNode;
	struct _node *nextNode;
	char data[CHECK_LENGTH];
};


struct _list{
	struct _node *firstNode;
	struct _node *lastNode;
	int len;
};

static struct _list listFos = {NULL, NULL, 0};

// 头部加入节点
void lpush(struct _list *list, char *data)
{
	struct _node *node = malloc(sizeof(struct _node));
	memset(node, 0x00, sizeof(struct _node));
	memcpy(node->data, data, CHECK_LENGTH);
	node->nextNode = list->firstNode;
	if (list->firstNode != NULL)
	{
		list->firstNode->prevNode = node;
	}
	list->firstNode = node;
	if (list->len == 0)
	{
		list->lastNode = node;
	}
	list->len++;
}


// 尾部加入节点
void rpush(struct _list *list, void *data)
{
	struct _node *node = malloc(sizeof(struct _node));
	memset(node, 0x00, sizeof(struct _node));
	memcpy(node->data, data, CHECK_LENGTH);
	node->prevNode = list->lastNode;
	if (list->lastNode != NULL)
	{
		list->lastNode->nextNode = node;
	}
	list->lastNode = node;
	if (list->len == 0)
	{
		list->firstNode = node;
	}
	list->len++;
}


// 头部弹出节点
void lpop(struct _list *list)
{
	if (list->firstNode == NULL)
	{
		return;
	}

	if (list->firstNode->nextNode != NULL)
	{
		list->firstNode = list->firstNode->nextNode;
		free(list->firstNode->prevNode);
		list->firstNode->prevNode = NULL;
	}
	else
	{
		free(list->firstNode);
		list->firstNode = NULL;
	}

	list->len--;

	if (list->len == 0)
	{
		list->lastNode = NULL;
	}
}

// 尾部弹出节点
void rpop(struct _list *list)
{
	if (list->lastNode == NULL)
	{
		return;
	}

	if (list->lastNode->prevNode != NULL)
	{
		list->lastNode = list->lastNode->prevNode;
		free(list->lastNode->nextNode);
		list->lastNode->nextNode = NULL;
	}
	else
	{
		free(list->lastNode);
		list->lastNode = NULL;
	}

	list->len--;
	if (list->len == 0)
	{
		list->firstNode = NULL;
	}
}

// 获取指定位置上的节点值
void *getVal(struct _list list, int pos)
{
	if (pos < 0)
	{
		return NULL;
	}

	int i;
	struct _node *curNode = list.firstNode;
	for(i = 0; i < pos; i++)
	{
		if(curNode->nextNode != NULL)
		{
			curNode = curNode->nextNode;
		}
		else
		{
			curNode = NULL;
			break;
		}
	}


	if (curNode == NULL){
		return NULL;
	}
	return curNode->data;
}

int GetAsrResult(char *pAsrResult)
{
	int nRet = 0;
	pthread_mutex_lock(&mutex);
	//esl_log(ESL_LOG_INFO, "JobID:%s GetAsrResult:%s nShift:%ld!\n", sJobId, sLibcurlData, nShift);
	if( nShift > 0 )
	{
		strcpy(pAsrResult, sLibcurlData);
		memset(sLibcurlData, 0x00, sizeof(sLibcurlData));
		nRet = (int)nShift ;
		nShift = 0;
	}

	pthread_mutex_unlock(&mutex);

	return nRet;
}

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	pthread_mutex_lock(&mutex);
	size_t res_size =  size * nmemb;
	memcpy(sLibcurlData + (int)nShift, ptr, res_size);
	nShift += res_size;
	esl_log(ESL_LOG_INFO, "JobID:%s write_callback %s size:%ld nmemb:%ld res_size:%ld nShift:%ld!\n", sJobId, sLibcurlData, size, nmemb, res_size, nShift);
	pthread_mutex_unlock(&mutex);
	return size * nmemb;
}

/*80/10ms，160/20ms，240/30ms*/
static void *ASRWebapi_func(void *arg)
{
	short sBufferRead[CHECK_LENGTH] = { 0 };
	int bStart = 0;
	int nEVadTimes = 0;
	char pDataBuffer[nMaxDataLen + CHECK_LENGTH];	//valid data buffer
	char pFosDataBuffer[nMaxDataLen + CHECK_LENGTH + CHECK_LENGTH * FOS_LEN];
	int nBufferLen = nInitLen;				//databuffer length
	int nFosBufferLen = nInitLen + FOS_LEN * CHECK_LENGTH;
	int nDataLen = 0;						//valid data length
	int nReadCount = 0;						//audio file pos
	int nVadCount = 0;

	esl_log(ESL_LOG_INFO, "JobID:%s ASRWebapi_func start fp=%d!\n", sJobId,fp);
	memset(pDataBuffer, 0x00 ,sizeof(pDataBuffer));
	memset(pFosDataBuffer, 0x00, sizeof(pFosDataBuffer));
	while (nRun)
	{
		if( NULL == fp )
		{	
			esl_log(ESL_LOG_INFO, "JobID:%s ASRWebapi_func try open:%s !\n", sJobId, sAudiofilePathname);
			fp = fopen(sAudiofilePathname, "rb");
			if( NULL == fp )
			{
				esl_log(ESL_LOG_INFO, "JobID:%s ASRWebapi_func try open:%s faild!\n", sJobId, sAudiofilePathname);
				usleep(10000);
				continue;
			}
			esl_log(ESL_LOG_INFO, "JobID:%s ASRWebapi_func try open:%s succeed!\n", sJobId, sAudiofilePathname);
			fseek(fp, 0, SEEK_SET); 
		}

		//if data buffer size >= nMaxdatalen(according to asr)
		//or invalid data check times > nVadEos
		//sending data buffer to asr
		if( nDataLen >= nMaxDataLen || (bStart && nEVadTimes >= nVadEos ))
		{
			struct _node *node = listFos.firstNode;
			int nFos = 0;
			while(node)
			{
				memcpy(pFosDataBuffer +  nFos * CHECK_LENGTH, node->data, CHECK_LENGTH );
				node = node->nextNode;
				lpop(&listFos);
				nFos ++;
			}
			memcpy(pFosDataBuffer + nFos * CHECK_LENGTH, pDataBuffer, nDataLen );
			esl_log(ESL_LOG_INFO, "JobID:%s  datalen:%d fosdatalen:%d \n", sJobId, nDataLen, nFos * CHECK_LENGTH + nDataLen);

			char vadFile[250];
			sprintf(vadFile, "%s%d", sAudiofilePathname, nVadCount++);
			FILE* fpr = fopen(vadFile, "wb");
			fwrite(pFosDataBuffer, nFos * CHECK_LENGTH + nDataLen, 1, fpr);
			fclose(fpr);

			sendDataToASR(pFosDataBuffer, nDataLen + nFos * CHECK_LENGTH, keda_param, keda_api_key , keda_appid);
			nDataLen = 0;
			bStart = 0;
			nEVadTimes = 0;
			memset(pDataBuffer, 0x00, nBufferLen);
			memset(pFosDataBuffer, 0x00, nFosBufferLen);

			pthread_mutex_lock(&mutex);
			pthread_cond_signal(&cond);
			pthread_mutex_unlock(&mutex);
		}

		if (nReadLen != fread(sBufferRead, sizeof(short), nReadLen, fp))
		{
			fseek(fp, nReadCount, SEEK_SET);
			usleep(10000);
			continue;
		}
		nReadCount += CHECK_LENGTH;

		int nVad = WebRtcVad_Process(pVad, 8000, sBufferRead, CHECK_LENGTH);
		//esl_log(ESL_LOG_INFO, "JobID:%s ASRWebapi_func start:%d readcount:%d datelen:%d bufferlen:%d vad:%d\n", sJobId, bStart, nReadCount, nDataLen, nBufferLen, nVad);
		if ( 0 ==  nVad)
		{
			if(bStart)
			{
				//counting vad times
				nEVadTimes ++;
				//still copy invalid data into data buffer
				memcpy(pDataBuffer + nDataLen, sBufferRead, CHECK_LENGTH);	
				nDataLen += CHECK_LENGTH;
			}
			//save invalid data before first valid frame 
			else
			{
				if( listFos.len == FOS_LEN )
				{
					lpop(&listFos);
				}
				rpush(&listFos, sBufferRead);
				//esl_log(ESL_LOG_INFO, "JobID:%s ASRWebapi_func save invalid times:%d\n", sJobId, listFos.len);
			}
		}
		//copy valid data into data buffer
		else  
		{
			if( 0 == bStart )
				bStart = 1;

			nEVadTimes = 0;
			memcpy(pDataBuffer + nDataLen, sBufferRead, CHECK_LENGTH);	
			nDataLen += CHECK_LENGTH;
		}
	}

	return 0;
}

int sendDataToASR(char* pAudioFileData, int nAudioFileDataLen, const char *pASRaram, const char *pApikey, const char *pAppid )
{
	char sBase64Param [128];
	char sMd5Str[33] = {0};
	char sVerifyStr[1024] = {0};
	int nRet = 0;
	char x_appid[32] = {0};
	char x_time[32] = {0};
	char x_param[256] = {0};
	char x_checksum[64] = {0};
	char *pBase64VadedBuffer = NULL;
	char *pUrlVadedBuffer = NULL;
	char *pBase64UrlVadedBuffer = NULL;
	
	if( NULL == pAudioFileData )
	{
		nRet = -1;
		goto process_exit;	
	}
	
	struct CURL *curl = curl_easy_init();
	if (NULL == curl)
	{
		esl_log(ESL_LOG_ERROR, "JobID:%s curl_easy_init failed!\n", sJobId);
		nRet = -2;
		goto process_exit;
	}

	pBase64VadedBuffer = (char*)malloc(2*nAudioFileDataLen);
	if( NULL == pBase64VadedBuffer )
	{
		esl_log(ESL_LOG_ERROR, "JobID:%s pBase64VadedBuffer buffer error:%s\n", sJobId, strerror(errno));
		nRet = -3;
		goto process_exit;
	}

	int nBase64VadedBuffer = encode_base64(pAudioFileData, nAudioFileDataLen, pBase64VadedBuffer);
	
	pUrlVadedBuffer  = curl_escape(pBase64VadedBuffer, nBase64VadedBuffer);
	int nUrlVadedBufferLen = strlen(pUrlVadedBuffer) + 7;

	pBase64UrlVadedBuffer = (char*)malloc(nUrlVadedBufferLen);
	if( NULL == pBase64UrlVadedBuffer )
	{
		esl_log(ESL_LOG_ERROR, "JobID:%s Malloc Url buffer error:%s\n", sJobId, strerror(errno));
		nRet = -3;
		goto process_exit;
	}
	sprintf(pBase64UrlVadedBuffer, "audio=%s", pUrlVadedBuffer);

	time_t nTimestamp = time(0);
	int nBase64Param = encode_base64(pASRaram, strlen(pASRaram), sBase64Param);	
	sprintf(sVerifyStr, "%s%ld%s", pApikey, nTimestamp, sBase64Param);
	get_md5(sVerifyStr, sMd5Str);
	
	sprintf(x_appid, "X-Appid:%s", pAppid);
	sprintf(x_time, "X-CurTime:%ld", nTimestamp);
	sprintf(x_param, "X-Param:%s", sBase64Param);
	sprintf(x_checksum, "X-CheckSum:%s", sMd5Str);

	struct curl_slist *http_headers = curl_slist_append(0, x_appid);
	curl_slist_append(http_headers, x_time);
	curl_slist_append(http_headers, x_param);
	curl_slist_append(http_headers, x_checksum);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_headers);

	curl_easy_setopt(curl, CURLOPT_URL, "http://api.xfyun.cn/v1/service/v1/iat");
	//
	curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
	//设置post请求
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pBase64UrlVadedBuffer);
	//设置超时
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1000);
	//指定回调函数
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	//设置回调函数参数
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL); 

	//memset(sLibcurlData, 0x00, sizeof(sLibcurlData));
	//nShift = 0;

	CURLcode res = curl_easy_perform(curl);

	esl_log(ESL_LOG_INFO, "JobID:%s curl_easy_perform:%d end!\n", sJobId, res);
	
process_exit:
	if( pBase64VadedBuffer)
		free(pBase64VadedBuffer);
	if(pUrlVadedBuffer)
		curl_free(pUrlVadedBuffer);
	if( pBase64UrlVadedBuffer)
		free(pBase64UrlVadedBuffer);
	
	return nRet;
}

static void get_asr_config(const char *job_UUID)
{
	esl_log(ESL_LOG_INFO, "JobID:%s get_asr_config:%s start!\n", job_UUID, CONF_FILE_PATH);

	GetProfileString(CONF_FILE_PATH, "asr" ,"param", keda_param);
	esl_log(ESL_LOG_INFO, "JobID:%s ASR param: %s\n", job_UUID, keda_param);

	GetProfileString(CONF_FILE_PATH, "asr" ,"apikey", keda_api_key);
	esl_log(ESL_LOG_INFO, "JobID:%s ASR apikey: %s\n", job_UUID, keda_api_key);

	GetProfileString(CONF_FILE_PATH, "asr" ,"appid", keda_appid);
	esl_log(ESL_LOG_INFO, "JobID:%s ASR appid: %s\n", job_UUID, keda_appid);

	GetProfileInt(CONF_FILE_PATH, "asr" ,"fos", &FOS_LEN);
	esl_log(ESL_LOG_INFO, "JobID:%s ASR fos: %d\n", job_UUID, FOS_LEN);

	GetProfileInt(CONF_FILE_PATH, "asr" ,"eos", &nVadEos);
	esl_log(ESL_LOG_INFO, "JobID:%s ASR eos: %d\n", job_UUID, nVadEos);

	GetProfileInt(CONF_FILE_PATH, "asr" ,"maxlen", &nMaxDataLen);
	esl_log(ESL_LOG_INFO, "JobID:%s ASR maxlen: %d\n", job_UUID, nMaxDataLen);
}

int ASRWebapiProcessInit(const char *job_UUID, const char *sAudioFile)
{
	strcpy(sJobId, job_UUID);
	strcpy(sAudiofilePathname, sAudioFile);

	get_asr_config(job_UUID);

	if (WebRtcVad_Create(&pVad))
	{
		esl_log(ESL_LOG_ERROR, "JobID:%s ASRWebapiProcessInit WebRtcVad_Create failed!\n", job_UUID);
		return -1;
	}

	if (WebRtcVad_Init(pVad))
	{
		esl_log(ESL_LOG_ERROR, "JobID:%s ASRWebapiProcessInit WebRtcVad_Init failed!\n", job_UUID);
		return -1;
	}

	if (WebRtcVad_set_mode(pVad, 3))
	{
		esl_log(ESL_LOG_ERROR, "JobID:%s ASRWebapiProcessInit WebRtcVad_set_mode failed!\n", job_UUID);
		return -1;
	}

	fp = fopen(sAudioFile, "rb");
	if( NULL == fp )
	{
		esl_log(ESL_LOG_ERROR, "JobID:%s ASRWebapiProcessInit open \'%s\' failed!error:%s!\n", job_UUID, sAudioFile, strerror(errno) );
		//return -2;
	}
	else
	{
		esl_log(ESL_LOG_ERROR, "JobID:%s ASRWebapiProcessInit open \'%s\' succeed=%d!\n", job_UUID, sAudioFile, fp );
	}

	if (0 != pthread_create(&tid, NULL, ASRWebapi_func, NULL))
	{
		esl_log(ESL_LOG_ERROR, "JobID:%s ASRWebapiProcessInit pthread_create failed!error:%s!\n", job_UUID, strerror(errno));
		return -2;
	}

	esl_log(ESL_LOG_INFO, "JobID:%s ASRWebapiProcessInit succeed!\n", job_UUID, strerror(errno));

	return 0;
}

void ASRWebapiProcessUninit(const char *job_UUID)
{
	nRun = 0;
	pthread_join(tid, NULL);
	WebRtcVad_Free(pVad);
}

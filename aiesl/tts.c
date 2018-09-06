#include "tts.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <memory.h>
#include <stdarg.h>
#include <string.h>

#include "qtts.h"
#include "msp_cmn.h"
#include "msp_errors.h"

static const char* tts_params = "voice_name = jinger, text_encoding = utf8, sample_rate = 16000, speed = 70, volume = 50, pitch = 70, rdn = 2";
static const char* login_params  = "appid = 5ae2c673, work_dir=/usr/local/src/keda_asr_linux/bin/";//��¼����,appid��msc���,��������Ķ�

static char tts_error[1024];
//static char tts_info[1024];
static pthread_mutex_t tts_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutexattr_t tts_attr;

/* wav��Ƶͷ����ʽ */
typedef struct _wave_pcm_hdr
{
	char    riff[4];                // = "RIFF"
	int		size_8;                 // = FileSize - 8
	char    wave[4];                // = "WAVE"
	char    fmt[4];                 // = "fmt "
	int		fmt_size;				// = ��һ���ṹ��Ĵ�С : 16
	short int       format_tag;     // = PCM : 1
	short int       channels;       // = ͨ���� : 1
	int		samples_per_sec;        // = ������ : 8000 | 6000 | 11025 | 16000
	int		avg_bytes_per_sec;      // = ÿ���ֽ��� : samples_per_sec * bits_per_sample / 8
	short int       block_align;            // = ÿ�������ֽ��� : wBitsPerSample / 8
	short int       bits_per_sample;        // = ����������: 8 | 16
	char            data[4];                // = "data";
	int		data_size;              // = �����ݳ��� : FileSize - 44 
} wave_pcm_hdr;

/* Ĭ��wav��Ƶͷ������ */
wave_pcm_hdr default_wav_hdr = 
{
	{ 'R', 'I', 'F', 'F' },
	0,
	{'W', 'A', 'V', 'E'},
	{'f', 'm', 't', ' '},
	16,
	1,
	1,
	16000,
	32000,
	2,
	16,
	{'d', 'a', 't', 'a'},
	0  
};

int set_tts_error(char *fmt, ...)
{
	memset(tts_error, 0x00, sizeof(tts_error));
	va_list args;
	int n;
	va_start(args, fmt);
	n = vsprintf(tts_error, fmt, args);
	va_end(args);
	return n;
}

const char *get_tts_error()
{
	return &tts_error[0];
}

int tts_init()
{
	int ret = MSP_SUCCESS;

	ret = MSPLogin(NULL, NULL, login_params);//��һ���������û������ڶ������������룬�����������ǵ�¼�������û������������http://www.xfyun.cnע���ȡ
	if (MSP_SUCCESS != ret)
	{
		set_tts_error("MSPLogin failed, error code: %d.\n", ret);
		return 0;
	}

	/*
	pthread_mutexattr_init(&tts_attr); 
	pthread_mutexattr_setpshared(&tts_attr, PTHREAD_PROCESS_PRIVATE);
	pthread_mutexattr_settype(&tts_attr , PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutex_init(&tts_mutex, &attr);
	*/
	return 1;
}

void tts_uninit()
{

}


/* �ı��ϳ� */
int text_to_speech(const char* src_text, const char* des_path)
{
	int          ret          = -1;
	FILE*        fp           = NULL;
	const char*  sessionID    = NULL;
	unsigned int audio_len    = 0;
	wave_pcm_hdr wav_hdr      = default_wav_hdr;
	int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;

	if (NULL == src_text || NULL == des_path)
	{
		return 0;
	}

	fp = fopen(des_path, "wb");
	if (NULL == fp)
	{
		set_tts_error("text_to_speech open file:%s error!\n");
		goto speech_error;
	}

	/* ��ʼ�ϳ� */
	sessionID = QTTSSessionBegin(tts_params, &ret);
	if (MSP_SUCCESS != ret)
	{
		set_tts_error("text_to_speech QTTSSessionBegin error!errorcode=%d!\n", ret);
		goto speech_error;
	}

	ret = QTTSTextPut(sessionID, src_text, (unsigned int)strlen(src_text), NULL);
	if (MSP_SUCCESS != ret)
	{
		set_tts_error("text_to_speech QTTSTextPut error!errorcode=%d!\n", ret);
		QTTSSessionEnd(sessionID, "TextPutError");
		goto speech_error;
	}

	fwrite(&wav_hdr, sizeof(wav_hdr) ,1, fp); //���wav��Ƶͷ��ʹ�ò�����Ϊ16000
	while (1) 
	{
		/* ��ȡ�ϳ���Ƶ */
		const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
		if (MSP_SUCCESS != ret)
			break;

		if (NULL != data)
		{
			fwrite(data, audio_len, 1, fp);
			wav_hdr.data_size += audio_len;		//����data_size��С
		}
		if (MSP_TTS_FLAG_DATA_END == synth_status)
			break;
		usleep(50 * 1000); //��ֹƵ��ռ��CPU
	}

	if (MSP_SUCCESS != ret)
	{
		set_tts_error("text_to_speech QTTSAudioGet error!errorcode=%d!\n", ret);
		QTTSSessionEnd(sessionID, "AudioGetError");
		goto speech_error;
	}

	/* ����wav�ļ�ͷ���ݵĴ�С */
	wav_hdr.size_8 += wav_hdr.data_size + (sizeof(wav_hdr) - 8);

	/* ��������������д���ļ�ͷ��,��Ƶ�ļ�Ϊwav��ʽ */
	fseek(fp, 4, 0);
	fwrite(&wav_hdr.size_8,sizeof(wav_hdr.size_8), 1, fp); //д��size_8��ֵ
	fseek(fp, 40, 0); //���ļ�ָ��ƫ�Ƶ��洢data_sizeֵ��λ��
	fwrite(&wav_hdr.data_size,sizeof(wav_hdr.data_size), 1, fp); //д��data_size��ֵ

	/* �ϳ���� */
	ret = QTTSSessionEnd(sessionID, "Normal");
	
	fclose(fp);
	fp = NULL;		
	return 1;

speech_error:
	if (NULL != fp)
	{
		fclose(fp);
		fp = NULL;		
	}

	return 0;
}

int text_to_speech_sync(const char* src_text, const char* des_path)
{
	pthread_mutex_lock(&tts_mutex);
	int ret = text_to_speech(src_text, des_path);
	pthread_mutex_unlock(&tts_mutex);

	return ret;
}
#include "code.h"
#include "md5.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// 全局常量定义
static const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char padding_char = '=';

/*编码代码
* const unsigned char * sourcedata， 源数组
* char * base64 ，码字保存
*/
int encode_base64(const char * sourcedata, int datalength, char * base64)
{
	int i=0, j=0, nBasese64 = 0;
	unsigned char trans_index=0;	// 索引是8位，但是高两位都为0
	for (; i < datalength; i += 3)
	{
		// 每三个一组，进行编码
		// 要编码的数字的第一个
		trans_index = ((sourcedata[i] >> 2) & 0x3f);
		base64[j++] = base64char[(int)trans_index];
		nBasese64 ++;
		// 第二个
		trans_index = ((sourcedata[i] << 4) & 0x30);
		if (i + 1 < datalength)
		{
			trans_index |= ((sourcedata[i + 1] >> 4) & 0x0f);
			base64[j++] = base64char[(int)trans_index];
			nBasese64 ++;
		}
		else
		{
			base64[j++] = base64char[(int)trans_index];
			base64[j++] = padding_char;
			base64[j++] = padding_char;
			nBasese64 += 3; 
			break;	// 超出总长度，可以直接break
		}
		// 第三个
		trans_index = ((sourcedata[i + 1] << 2) & 0x3c);
		if (i + 2 < datalength)
		{ // 有的话需要编码2个
			trans_index |= ((sourcedata[i + 2] >> 6) & 0x03);
			base64[j++] = base64char[(int)trans_index];
			trans_index = sourcedata[i + 2] & 0x3f;
			base64[j++] = base64char[(int)trans_index];
			nBasese64 += 2; 
		}
		else
		{
			base64[j++] = base64char[(int)trans_index];
			base64[j++] = padding_char;
			nBasese64 += 2; 
			break;
		}
	}

	base64[j] = '\0'; 

	return nBasese64;
}

void get_md5(char *data, char *md5String)
{
	md5_byte_t digest[16];
	md5_state_t md5StateT;
	md5_init(&md5StateT);
	md5_append(&md5StateT, (const md5_byte_t *) data, strlen(data));
	md5_finish(&md5StateT, digest);
	char hexBuffer[3];
	int i = 0;
	for (i = 0; i != 16; ++i)
	{
		if (digest[i] < 16)
			sprintf(hexBuffer, "0%x", digest[i]);
		else
			sprintf(hexBuffer, "%x", digest[i]);
		strcat(md5String, hexBuffer);
	}
}

#include "base64.h"

#include <string>
#include <iostream>

using namespace std;

static const unsigned char map[256] = {  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 253, 255,  
	255, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 253, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,  
	52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,  
	255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,  
	7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  
	19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,  
	255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  
	37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  
	49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
	255, 255, 255, 255 };  

string CBase64::Encode(const unsigned char* Data,int DataByte)
{
	//编码表
	const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//返回值
	string strEncode;
	unsigned char Tmp[4]={0};
	int LineLength=0;
	for(int i=0;i<(int)(DataByte / 3);i++)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		Tmp[3] = *Data++;
		strEncode+= EncodeTable[Tmp[1] >> 2];
		strEncode+= EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		strEncode+= EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		strEncode+= EncodeTable[Tmp[3] & 0x3F];
		if(LineLength+=4,LineLength==76) {strEncode+="\r\n";LineLength=0;}
	}
	//对剩余数据进行编码
	int Mod=DataByte % 3;
	if(Mod==1)
	{
		Tmp[1] = *Data++;
		strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4)];
		strEncode+= "==";
	}
	else if(Mod==2)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		strEncode+= EncodeTable[((Tmp[2] & 0x0F) << 2)];
		strEncode+= "=";
	}

	return strEncode;
}

/*
string CBase64::Decode(const char* Data,int DataByte,int& OutByte)
{
	//解码表
	const char DecodeTable[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		62, // '+'
		0, 0, 0,
		63, // '/'
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
		0, 0, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
		0, 0, 0, 0, 0, 0,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
	};
	//返回值
	string strDecode;
	int nValue;
	int i= 0;
	while (i < DataByte)
	{
		if (*Data != '\r' && *Data!='\n')
		{
			nValue = DecodeTable[*Data++] << 18;
			nValue += DecodeTable[*Data++] << 12;
			strDecode+=(nValue & 0x00FF0000) >> 16;
			OutByte++;
			if (*Data != '=')
			{
				nValue += DecodeTable[*Data++] << 6;
				strDecode+=(nValue & 0x0000FF00) >> 8;
				OutByte++;
				if (*Data != '=')
				{
					nValue += DecodeTable[*Data++];
					strDecode+=nValue & 0x000000FF;
					OutByte++;
				}
			}
			i += 4;
		}
		else// 回车换行,跳过
		{
			Data++;
			i++;
		}
	}
	return strDecode;
}
*/


string CBase64::Decode(const char *in, int DataByte, int& OutByte)
{  
	unsigned long t, x, y, z;  
	unsigned char c;  
	int g = 3;  
	string out;

	for (x = y = z = t = 0; in[x]!=0;) 
	{  
		c = map[in[x++]];  
		if (c == 255) 
			return "";  

		if (c == 253) 
			continue; 

		if (c == 254)
		{ 
			c = 0; 
			g--; 
		}

		t = (t<<6)|c;  
		if (++y == 4) 
		{  
			out.append( 1, (unsigned char)((t>>16)&255));  
			if (g > 1) out.append( 1, (unsigned char)((t>>8)&255));  
			if (g > 2) out.append( 1, (unsigned char)(t&255));  
			y = t = 0;  
		}  
	}  

	return out;
}  

#ifndef _H_BASE64
#define _H_BASE64

#include <string>

using namespace std;

class CBase64
{
public:
    static string Encode(const unsigned char* Data,int DataByte);

	static string Decode(const char* Data,int DataByte,int& OutByte);
};

#endif
#ifndef ___BASE64_H___
#define ___BASE64_H___

#include <string>
using namespace std;

class CBase64
{
public:
	static bool Encode(const unsigned char *pIn, unsigned long uInLen, string& strOut);
	static bool Encode(const unsigned char *pIn, unsigned long uInLen, unsigned char *pOut, unsigned long *uOutLen);
	static bool Decode(const string& strIn, unsigned char *pOut, unsigned long *uOutLen) ;

	static string Encode(string data);	
	static string Decode(string data);
	static int GetDecodeBufferLength(string data);

	static string GBToUTF8(const char*); //��gb2312����תΪUTF-8�ַ�
	static string UTF8ToGB(const char*); //��UTF-8����תΪgb2312�ַ�
};

#endif // ___BASE64_H___

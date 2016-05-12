#ifndef __Encode_Base64__
#define __Encode_Base64__

#include <string> 
using namespace std;


class EncodeBase64{
public:
	static std::string base64_encode(unsigned char const* , unsigned int len);
	static std::string base64_decode(std::string const& s);

};
#endif
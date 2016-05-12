#ifndef INIReader_INIReader_h
#define INIReader_INIReader_h

#include <map>

#include <Windows.h>

using std::map;
using std::wstring;


class INIReader {
public:
	//static INIReader instance;
	//static INIReader &getInstance();	// to be used as singleton
	int isFileExist(wstring &filePath);
    int GetParamValue(const wstring &key, wstring &value);
	bool INIReader::WStringToString(const std::wstring &wstr,std::string &str);
	bool INIReader::StringToWString(const std::string &str,std::wstring &wstr);
	void operator=(INIReader const&);	// avoid assignment


	INIReader(void);	// hide ctor
	INIReader(INIReader const&);	// avoid copy
	

    int readFile(const wstring &filename);
    int retrieveFilePath(wstring &filepath);
    int parseContentLine(wstring &contentLine);
    
    map<wstring, wstring> paramMap;
};


#endif

#include "vxIniFile.h"


///////////////////////////////////////////////////////////////////////////

/**************************************************************************
* FunctionName:
*     MkPathDir
* Description:
*     创建全路径目录（递归方式创建完整目录结构）。
* Parameter:
*     @[in ] szDir: 路径目录。
* ReturnValue:
*     成功，返回 TRUE；失败，返回 FALSE。
*/
BOOL MkPathDir(char * szDir)
{
	int err = -1;

	char cp = '\0';
	char szPath[MAX_PATH];
	char *szPt = szPath;

	if ((szDir == NULL) || (sizeof(szDir) > MAX_PATH))
		return FALSE;

	memset(szPath, 0, MAX_PATH);

	while (*szPt++ = cp = *szDir++)
	{
		if ((cp == '\\') || (cp == '/') || (*szDir == '\0'))
		{
			if (_access(szPath, 0) == 0)
			{
				err = 0;
			}
			else
			{
				err = mkdir(szPath);
				if (err != 0)
					break;
			}
		}
	}

	return (err == 0);
}

/**************************************************************************
* FunctionName:
*     MkFilePathDir
* Description:
*     根据给定的文件全路径名，创建其路径目录。
* Parameter:
*     @[in ] szFilePath: 文件全路径名。
* ReturnValue:
*     成功，返回 TRUE；失败，返回 FALSE。
*/
BOOL MkFilePathDir(char * szFilePath)
{
	string strDir = szFilePath;		
	strDir = strDir.substr(0,strDir.find_last_of("\\") + 1);

	char* c;
	const int len = strDir.length();
	c =new char[len+1];
	strcpy(c,strDir.c_str());
	return MkPathDir(c);
}
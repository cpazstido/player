#include "vxIniFile.h"


///////////////////////////////////////////////////////////////////////////

/**************************************************************************
* FunctionName:
*     MkPathDir
* Description:
*     ����ȫ·��Ŀ¼���ݹ鷽ʽ��������Ŀ¼�ṹ����
* Parameter:
*     @[in ] szDir: ·��Ŀ¼��
* ReturnValue:
*     �ɹ������� TRUE��ʧ�ܣ����� FALSE��
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
*     ���ݸ������ļ�ȫ·������������·��Ŀ¼��
* Parameter:
*     @[in ] szFilePath: �ļ�ȫ·������
* ReturnValue:
*     �ɹ������� TRUE��ʧ�ܣ����� FALSE��
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
#ifndef __VXINIFILE_H__
#define __VXINIFILE_H__

#include <Windows.h>
#include <string>
#include <sstream>
#include <io.h>
#include <direct.h>

using  std::string;
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
BOOL MkPathDir(char * szDir);

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
BOOL MkFilePathDir(char * szFilePath);



///////////////////////////////////////////////////////////////////////////


#endif // __VXINIFILE_H__
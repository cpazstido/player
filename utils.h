#include <Windows.h>
#include <string>
#include <sstream>
#include <io.h>
#include <direct.h>
using namespace std;
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
bool MkPathDir(char * szDir);

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
bool MkFilePathDir(char * szFilePath);

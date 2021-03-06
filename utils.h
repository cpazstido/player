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
*     创建全路径目录（递归方式创建完整目录结构）。
* Parameter:
*     @[in ] szDir: 路径目录。
* ReturnValue:
*     成功，返回 TRUE；失败，返回 FALSE。
*/
bool MkPathDir(char * szDir);

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
bool MkFilePathDir(char * szFilePath);

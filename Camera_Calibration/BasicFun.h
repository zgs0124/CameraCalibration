#pragma once

#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES

//#include "glog/logging.h"
//#include "opencv2\opencv.hpp"
#include <crtdbg.h>
#include <time.h>
#include <sys/timeb.h>
#include <direct.h>
#include <io.h>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <iostream>
#include <mutex>
#include <vector>
#include <thread>
#include <string>
#include <list>
#include <Windows.h>
#include "GetConfig.h"
#include "glog\logging.h"

#ifdef _DEBUG
#pragma comment(lib,"glogd.lib")
#else
#pragma comment(lib,"glog.lib")
#endif // DEBUG

#define WRITE_BUFF		1024*4

extern CGetConfig g_config;

/*获取系统毫秒级时间*/
struct NowDate
{
	char tmp0[16]; //年月日
	char tmp1[16]; //时分秒
	char tmp2[4];  //毫秒
};
NowDate getTime();

void GLOG_Init();

bool WriteFile(std::string m_strWrite, std::string m_strPath);
bool IfDirectoryExists(const char * absolutePath);
bool CreateDir(const char * filename);
bool GetTime(int value, std::string &time);

void GetCurrentPath(std::string currentPath, std::string exeName);

//TCHAR 转 string
std::string TCHAR2String(TCHAR *STR);
//Unicode 下char转CString
CString Char2CString(const char *input);
//CString 转 string
std::string ToString(CString cs);
//string 转 CString
CString ToCString(std::string str);
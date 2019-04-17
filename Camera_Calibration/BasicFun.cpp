#include "stdafx.h"
#include "BasicFun.h"

CGetConfig g_config;

void GLOG_Init()
{
	std::string m_LogPath = "";
	if (g_config.GetLogPath().length() > 0)
	{
		m_LogPath = g_config.GetLogPath();
		CreateDir(m_LogPath.c_str());
	}
	else
	{
		m_LogPath = "C:\\123";
		CreateDir(m_LogPath.c_str());
	}
	std::string m_curPath, m_exeName;
	GetCurrentPath(m_curPath, m_exeName);
	//FLAGS_log_dir = m_LogPath.c_str();
	google::InitGoogleLogging(m_exeName.c_str());
	std::string str = m_LogPath + "\\INFO_";
	google::SetLogDestination(google::GLOG_INFO, str.c_str());
}

bool WriteFile(std::string m_strWrite, std::string m_strPath)
{
	//写文件
	if (0 == m_strPath.length())
	{
		printf("输入的路径错误！\n");
		return false;
	}
	FILE *pFile = NULL;
	fopen_s(&pFile, m_strPath.c_str(), "a+");//打开文件
	if (pFile == NULL)//错误处理
	{
		printf("打开文件错误！\n");
		return false;
	}
	if (fseek(pFile, 0L, SEEK_END) != 0)
	{
		printf("设置文件写入指针错误！\n");
		fclose(pFile);
		return false;
	}
	size_t baoWenLen = m_strWrite.length();
	if (0 == baoWenLen)
	{
		fclose(pFile);
		return false;
	}
	char buf[WRITE_BUFF];
	memcpy(buf, m_strWrite.c_str(), baoWenLen);
	char *bufIndex = buf;
	size_t nToWrite = baoWenLen;
	size_t nWrite = 0;
	//printf("%d ", nToWrite);
	while (nToWrite != 0)
	{
		nWrite = fwrite(bufIndex, sizeof(char), nToWrite, pFile);
		//nWrite = fwrite(bufIndex,)
		if (nWrite == 0)
		{
			fclose(pFile);
			printf("写入文件数据错误！\n");
			fclose(pFile);
			return false;
		}
		else
		{
			nToWrite -= nWrite;
			bufIndex += nWrite;
		}
	}
	fclose(pFile);
	return true;
}
bool IfDirectoryExists(const char * absolutePath)
{
	if (_access(absolutePath, 0) == 0)
	{
		size_t nInd = strlen(absolutePath) - 1;
		while (nInd > 0)
		{
			if (absolutePath[nInd] != '\\' && absolutePath[nInd] != '/')
				break;
			nInd--;
		}
		if (nInd >= _MAX_PATH)
			return false;
		char szDir[_MAX_PATH] = {};
		strncpy_s(szDir, absolutePath, nInd + 1);
		struct stat myStat;
		int nErr = stat(szDir, &myStat);
		if (nErr == 0)
		{
			bool b = ((myStat.st_mode) & S_IFMT) == S_IFDIR;
			return ((myStat.st_mode) & S_IFMT) == S_IFDIR;
		}
	}
	return false;
}
/*
* 递归创建一个文件夹
*/
bool CreateDir(const char * filename)
{
	if (NULL == filename)
		return false;
	if (IfDirectoryExists(filename))
		return true;
	char szDirName[_MAX_PATH] = {};
	strcpy_s(szDirName, filename);
	for (int i = 0; i < strlen(szDirName); i++)
	{
		if ('/' == szDirName[i])
		{
			szDirName[i] = '\\';
		}
	}
	size_t nLen = strlen(szDirName);
	if (szDirName[nLen - 1] == '\\')
	{
		szDirName[nLen - 1] = '\0';
	}
	char * pSeparator = strchr(szDirName, ':');
	if (NULL == pSeparator)
		pSeparator = szDirName;
	else
		pSeparator += 2;

	do
	{
		pSeparator = strchr(pSeparator + 1, '\\');
		if (NULL == pSeparator)
			break;
		pSeparator[0] = '\0';
		if (!IfDirectoryExists(szDirName) && _mkdir(szDirName))
		{
			printf("fail to create directory: %s\n", szDirName);
			return false;
		}
		pSeparator[0] = '\\';
	} while (true);

	if (!IfDirectoryExists(szDirName) && _mkdir(szDirName))
	{
		printf("fail to create directory: %s\n", szDirName);
		return false;
	}
	return true;
}

bool GetTime(int value, std::string &time)
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	char buf[MAX_PATH] = { 0 };
	if (time.length() > 0)
		return false;
	switch (value)
	{
	case 1:
	{
		sprintf_s(buf, MAX_PATH, "%02d_%02d_%02d", st.wYear, st.wMonth, st.wDay);
		time = buf;
		break;
	}
	case 2:
	{
		sprintf_s(buf, MAX_PATH, "%02d_%02d_%02d", st.wHour, st.wMinute, st.wSecond);
		time = buf;
		break;
	}
	case 3:
	{
		sprintf_s(buf, MAX_PATH, "%02d_%02d_%02d_%02d_%02d_%02d_%d", st.wYear, st.wMonth, st.wDay, st.wHour,
			st.wMinute, st.wSecond, st.wMilliseconds);
		time = buf;
		break;
	}
	case 4:
	{
		sprintf_s(buf, MAX_PATH, "%02d%02d%02d_%02d%02d%02d%d", st.wYear, st.wMonth, st.wDay, st.wHour,
			st.wMinute, st.wSecond, st.wMilliseconds);
		time = buf;
		break;
	}
	default:
		break;
	}
	return true;
}


NowDate getTime()
{
	struct tm t;
	time_t now;
	time(&now);
	NowDate date;
	localtime_s(&t, &now);

	strftime(date.tmp0, sizeof(date.tmp0), "%Y%m%d_", &t);
	strftime(date.tmp1, sizeof(date.tmp1), "%H%M%S_", &t);

	struct timeb tb;
	ftime(&tb);
	sprintf_s(date.tmp2, "%d", tb.millitm);

	return date;
}

std::string TCHAR2String(TCHAR *STR)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen*sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);
	return str;
}

//Unicode 下char转CString
CString Char2CString(const char *input)
{
	USES_CONVERSION;
	CString cstr = A2T(input);
	//USES_CONVERSION;
	//CString cstr = A2W(input);
	return cstr;
}

std::string ToString(CString cs) 
{
#ifdef _UNICODE
	//如果是unicode工程
	USES_CONVERSION;
	std::string str(W2A(cs));
	return str;
#else
	//如果是多字节工程 
	std::string str(cs.GetBuffer());
	cs.ReleaseBuffer();
	return str;

#endif // _UNICODE 
}

CString ToCString(std::string str) 
{
#ifdef _UNICODE
	//如果是unicode工程
	USES_CONVERSION; 
	CString ans(str.c_str());
	return ans;
#else
	//如果是多字节工程 
	//string 转 CString
	CString ans;
	ans.Format("%s", str.c_str());
	return ans;
#endif // _UNICODE  
}

void GetCurrentPath(std::string currentPath,std::string exeName)
{
	char szExePath[_MAX_PATH] = {};
	memset(szExePath, 0, MAX_PATH);
	GetModuleFileNameA(NULL, szExePath, MAX_PATH);
	currentPath = szExePath;
	if (currentPath.length() > 0)
	{
		size_t iPos = currentPath.rfind('\\');
		exeName = currentPath.substr(iPos + 1, currentPath.length());
		currentPath = currentPath.substr(0, iPos + 1);
	}
}
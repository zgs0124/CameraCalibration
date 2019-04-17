//The functions in the file are used to transform between variant UDP message and the universal message package sent by socket
#include "stdafx.h"
#include "CommUtil.h"
#include <io.h>
#include <shellapi.h>
#include <time.h>
#include <direct.h>
#include <WinVer.h>
#include <tchar.h>


/*
int CreateMultiDir2(const char* szPath)
{
	char szFilePath[MAX_PATH];
	int k = 0;

	char szBuf[MAX_PATH];
	strcpy_s(szBuf,sizeof(szBuf),szPath);
	int i = strlen(szBuf);
	for(int j = 0; j<i; j++)
	{
		if(szBuf[j] == '\\' || szBuf[j] == '/')
		{
			// 解出一级目录
			k = j;
			strncpy_s(szFilePath,sizeof(szFilePath),szBuf, k);
			szFilePath[k] = '\0';

			if((_access(szFilePath, 0)) == -1)
			{
				// 创建目录
				if(_mkdir(szFilePath) == -1)
					return -1;
			}
		}
	}

	if (k == 0) // 非法目录 
	{
		return -1;
	}

	return 0;
}

void GetAnotherDateFromCur(std::string &sTimeStr, int iDay)
{
    time_t atime;
	time(&atime);
	atime+=iDay*24*3600;
	tm ttm;
	_tzset();
	localtime_s(&ttm, &atime);
	char charLocalTime[32];
	sprintf(charLocalTime, "%04d%02d%02d",
           ttm.tm_year + 1900,ttm.tm_mon + 1, ttm.tm_mday);
	sTimeStr = charLocalTime;
}

void GetLogFolderWithDate(const std::string &sPath, std::string &sPathDate, int iPlusMinus)
{
	std::string sDate;
	GetAnotherDateFromCur(sDate, iPlusMinus);
	sPathDate=sPath;
	sPathDate+="\\";
	sPathDate+=sDate;
}

int RemoveAllFilesInAFolder(const std::string &sFolderPath)
{
	int iRet=0;
	std::string sPath;
    char szPath[256];
	memset(szPath,0,256);

	sPath.append(sFolderPath);
	sPath.append("*.*");
	memcpy(szPath,sPath.c_str(),sPath.length()+1);
	*(szPath+sPath.length()+1)=0;

	SHFILEOPSTRUCT filestruct;
    filestruct.hwnd=NULL;
    filestruct.wFunc=FO_DELETE;
    filestruct.pFrom=szPath;
	filestruct.pTo=NULL;
    filestruct.fFlags=FOF_FILESONLY | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    filestruct.fAnyOperationsAborted=TRUE;
    filestruct.hNameMappings=NULL;
    filestruct.lpszProgressTitle=NULL;

	if (SHFileOperation(&filestruct)!=0)
	{
		iRet=-1;
	}
	return iRet;
}

int RemoveAFolderN(const std::string &sFolderPath)
{
	//CAPPLog::Log(LOG_INFO, "RemoveAFolderN->, Folder:%s.", sFolderPath.c_str());
	int iRet=0;
    WIN32_FIND_DATAA FindFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    if (RemoveAllFilesInAFolder(sFolderPath)!=0)
	{
		return -1;
	}
	std::string sRet,sFile;
	sFile=sFolderPath+"*.*";
    hFind = FindFirstFile(sFile.c_str(), &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        return -1;
    } 
	while (FindNextFile(hFind,&FindFileData)!=0)
	{
		if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			DWORD dwAttrs=FindFileData.dwFileAttributes;
       
            if (dwAttrs & FILE_ATTRIBUTE_READONLY) 
            { 
				dwAttrs&=~FILE_ATTRIBUTE_READONLY;
    			sRet.clear();
    			sRet.append(FindFileData.cFileName);
				sRet=sFolderPath + sRet + "\\";
                if (!SetFileAttributes(sRet.c_str(),dwAttrs))
				{
					return -4;
				}
			}
			sRet.clear();
			sRet.append(FindFileData.cFileName);
			if (sRet!="..")
			{
				sRet=sFolderPath + sRet + "\\";
				iRet=RemoveAFolderN(sRet);
                if (iRet!=0)
				{
					break;
				}
			}
		}
	}
	FindClose(hFind);
	if (!iRet)
	{
    	if (!RemoveDirectory(sFolderPath.c_str()))
	    {
		    iRet=-2;
	    }
	}
	return iRet;
}


bool IsFileExists(const char* szFile)
{
	return ((_access(szFile, 0)) != -1);
}

time_t FileTimeToGMT(LPFILETIME pft)
{
	time_t tt = 0;
	LONGLONG ll = (LONGLONG)(pft->dwLowDateTime);
	ll += (LONGLONG) ((LONGLONG) (pft->dwHighDateTime)) << 32;
	ll -= 116444736000000000;
	tt = ll/10000000;
	return tt;
}

time_t GetFileModifyTime(const char *filename, DWORD *pdwErrCode)
{
	if(pdwErrCode != NULL) *pdwErrCode = 0;
	time_t modTime = 0;
	FILETIME modT;
	modT.dwHighDateTime=0;
	modT.dwLowDateTime=0;
	HANDLE hFile = ::CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, 0) ;
	if(hFile==INVALID_HANDLE_VALUE)
	{
		DWORD err = GetLastError();
		if(pdwErrCode != NULL) *pdwErrCode = err;
		if(err != 2 && err !=3)
		{
			//Zhangxq Aug 12, 2008 for bug 10996
			//If function CreateFile failed, try another way to get last modified time
			WIN32_FILE_ATTRIBUTE_DATA f_data;
			//CAPPLog::Log("GetFileModifyTime-----use GetFileAttributesEx"); 
			if (GetFileAttributesExA(filename, GetFileExInfoStandard, &f_data))
			{
				modT.dwHighDateTime=f_data.ftLastWriteTime.dwHighDateTime;
				modT.dwLowDateTime=f_data.ftLastWriteTime.dwLowDateTime;
				*pdwErrCode = 0 ;
			}
			else
			{
				*pdwErrCode=GetLastError();
				//CAPPLog::Log(LOG_ERR, _T("esutil::GetFileModifyTime-----CreateFile failed(%d):%s"),
				//	*pdwErrCode, filename.c_str());  
			}
		}
	}
	else
	{
		if(GetFileTime(hFile, NULL, NULL, &modT))
		{
			//do nothing
		}
		else
		{
			modT.dwHighDateTime=0;
			modT.dwLowDateTime=0;
			DWORD err = GetLastError();
			if(pdwErrCode != NULL) *pdwErrCode = err;
			//CAPPLog::Log(LOG_ERR, _T("esutil::GetFileModifyTime-----GetFileTime failed(%d):%s"),
			//	err, filename.c_str()); 
		}
		CloseHandle(hFile) ;
	}
	//Zhangxq Dec 18, 2008 for bug 16960
	//In these two cases, we only transfer time here.
	if (modT.dwHighDateTime!=0)
	{
		modTime = FileTimeToGMT(&modT);
	}

	return modTime;
}
*/
BOOL GetWholeProductVersion(char *psFullPathFileName, char *psOVersion, char *psHeaderNumber, BOOL f4BAlign)
{
	BOOL fRet = FALSE;
	strcpy(psOVersion, "");
	do
	{
		char path[2048];
		if (psFullPathFileName!=NULL)
		{
			if((_access(psFullPathFileName, 0)) == -1)
			{
				break;
			}
			strcpy(path, psFullPathFileName);
		}
		else
		{
			if(0 == GetModuleFileNameA(NULL,path, 2048)) break;
		}	
		DWORD dwHnd;
		DWORD dwVerSize = GetFileVersionInfoSizeA(path, &dwHnd);
		if(0 == dwVerSize) break;
		LPVOID pBlock = new byte[dwVerSize*2];
		BOOL flag = GetFileVersionInfoA(path,0, dwVerSize, pBlock);
		if(flag)
		{
			VS_FIXEDFILEINFO* pBuf;
			UINT len;
			//Specifies the root block (language independent
			/*
			flag = VerQueryValue(pBlock, "\\", (void**)(&pBuf), &len);
			if(flag)
			{
				DWORD ms = pBuf->dwProductVersionMS;
				int ms1 = ms >> 16;
				int ms2 = ms & 0x0000ffff;
				DWORD ls = pBuf->dwProductVersionLS;
				int ls1 = ls >> 16;
				int ls2 = ls & 0x0000ffff;
				char ver[32];
				if(f4BAlign) sprintf(ver,  "%04d.%04d.%04d.%04d", ms1, ms2, ls1, ls2);
				else sprintf(ver,  "%d.%d.%d.%d", ms1, ms2, ls1, ls2);
				strcpy(psOVersion, ver);
				fRet = TRUE;
			}
			*/
			std::string sTmp;
			char* pStrBuild = NULL;
			std::string sKeyString="\\StringFileInfo\\";
			sKeyString.append(psHeaderNumber);
			sKeyString.append("\\ProductVersion");
			flag = VerQueryValueA(pBlock, (LPSTR)sKeyString.c_str(), (void**)(&pStrBuild), &len);
			if (flag)
			{
				sTmp=pStrBuild;
				if (!sTmp.empty())
				{
    				//strcat(psOVersion, "-");
    				strcat_s(psOVersion,strlen(pStrBuild),pStrBuild);
				}
			}
			sKeyString="\\StringFileInfo\\";
			sKeyString.append(psHeaderNumber);
			sKeyString.append("\\SpecialBuild");
			flag = VerQueryValueA(pBlock, (LPSTR)sKeyString.c_str(), (void**)(&pStrBuild), &len);
			if (flag)
			{
				//Zhangxq Feb 10, 2009 for bug 18943
				//if private build is long than the normal(normal length is 11), 
				//then erase the redundancy.
				sTmp=pStrBuild;
				if (!sTmp.empty())
				{
    				strcat(psOVersion, ".");
    				strcat(psOVersion, sTmp.c_str());
					fRet=TRUE;
				}
			}
		}
		delete[] pBlock;
	}while(FALSE);
	return fRet;
}

int StartProgram(const std::wstring &exefile, const std::wstring &cmdline, HANDLE &hprocess, DWORD* pPID, bool bWait)
{
	int nRet = 0;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;


	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	hprocess = NULL;
	std::wstring sCurrentDir;
	size_t iPos = exefile.find(L".EXE");
	if (iPos == std::wstring::npos)
	{
		iPos = exefile.find(L".exe");
	}
	if (iPos!=std::wstring::npos)
	{
		sCurrentDir = exefile.substr(0, iPos - 1);
		iPos=sCurrentDir.rfind(L"\\");
		if (iPos!=std::wstring::npos)
		{
			sCurrentDir.erase(iPos+1);
		}
		else
			sCurrentDir.clear();
	}

	LPTSTR lpstr;
	if (sCurrentDir.empty())
	{
		lpstr=NULL;
	}
	else
	{
		lpstr=(LPTSTR)sCurrentDir.data();
	}

	for(int i = 0; i < 5; i++)
	{
		nRet = 0;
		if (!CreateProcess(
			NULL,					// name of executable module
			(LPTSTR)cmdline.c_str(),			// command line string
			NULL,					// process attributes
			NULL,					// thread attributes
			FALSE,					// handle inheritance option
			0,						// creation flags
			NULL,					// new environment block
			lpstr,					// current directory name
			&si,					// startup information
			&pi))					// process information
		{
			// if failed, continue transport the exception
			nRet = GetLastError();
			if(2 == nRet || 3 == nRet) break;
			::Sleep(3000);
		}else
		{
			hprocess = pi.hProcess;
			if(pPID != NULL) *pPID = pi.dwProcessId; 
			CloseHandle(pi.hThread);
			if(bWait)
			{
				::WaitForSingleObject(pi.hProcess, INFINITE);  
			}
			break;
		}
	}
	// succeeding, set exception handled state
	return nRet;
}

int StartProgram(const std::string &cmdline, HANDLE &hprocess, DWORD* pPID, bool bWait)
{
	int nRet = 0;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;


	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	hprocess = NULL;
	std::string sCurrentDir;
	size_t iPos = cmdline.find('.');
	if (iPos != std::string::npos)
	{
		sCurrentDir = cmdline.substr(0, iPos - 1);
		iPos = sCurrentDir.rfind("\\");
		if (iPos >= 0)
		{
			sCurrentDir.erase(iPos + 1);
		}
		else
			sCurrentDir.clear();
	}

	LPTSTR lpstr;
	if (sCurrentDir.empty())
	{
		lpstr = NULL;
	}
	else
	{
		lpstr = (LPTSTR)sCurrentDir.data();
	}

	for (int i = 0; i < 5; i++)
	{
		nRet = 0;
		if (!CreateProcess(
			NULL,					// name of executable module
			(LPTSTR)cmdline.c_str(),			// command line string
			NULL,					// process attributes
			NULL,					// thread attributes
			FALSE,					// handle inheritance option
			0,						// creation flags
			NULL,					// new environment block
			lpstr,					// current directory name
			&si,					// startup information
			&pi))					// process information
		{
			// if failed, continue transport the exception
			nRet = GetLastError();
			if (2 == nRet || 3 == nRet) break;
			::Sleep(3000);
		}
		else
		{
			hprocess = pi.hProcess;
			if (pPID != NULL) *pPID = pi.dwProcessId;
			CloseHandle(pi.hThread);
			if (bWait)
			{
				::WaitForSingleObject(pi.hProcess, INFINITE);
			}
			break;
		}
	}
	// succeeding, set exception handled state
	return nRet;
}


void GetAppPath(TCHAR *pszPath, int iSize)
{
	GetModuleFileName(NULL, pszPath, iSize);
#ifdef  UNICODE                   
	std::wstring sPath;
#else
	std::string sPath;
#endif
	sPath.append(pszPath);
	size_t iPos = sPath.rfind(_T("\\"));
	if (iPos > 0)
	{
		sPath = sPath.substr(0, iPos + 1);
	}
	_tcscpy(pszPath, sPath.c_str());
}


bool IsFileOrDirExists(const wchar_t *pszFile)
{
	return (_waccess(pszFile, 0) == 0);
}

bool IsFileOrDirExists(const char *pszFile)
{
	return (_access(pszFile, 0) == 0);
}

#ifdef  UNICODE                     // r_winnt
const std::wstring PATH_SOLIDUS_CHAR = _T("\\");
#else
const std::string PATH_SOLIDUS_CHAR = "\\";
#endif

BOOL TestandCreateDirectory(const TCHAR *pszFile)
{
	if (!IsFileOrDirExists(pszFile))
	{
#ifdef  UNICODE                     // r_winnt
		std::wstring tmp = pszFile;
		size_t npos=std::wstring::npos; 
#else
		std::string tmp = pszFile;
		size_t npos = std::wstring::npos;
#endif
		int slash = tmp.find_last_of(PATH_SOLIDUS_CHAR);
		if (slash != npos)	tmp.erase(slash);
		if (!IsFileOrDirExists(pszFile))
		{
			if (npos == slash) return FALSE;
			if (!TestandCreateDirectory(tmp.c_str()))
			{
				return FALSE;
			}
		}


		BOOL res = CreateDirectory(pszFile, NULL);
		int  error = GetLastError();
		if (FALSE == res && (error != 183)) //directory exist/
		{
			return FALSE;
		}
		else
			return TRUE;
	}
	return TRUE;
}

/*
**time_t转SYSTEMTIME
*/
SYSTEMTIME Time_tToSystemTime(time_t t)
{
	tm temptm;
	localtime_s(&temptm, &t);

	SYSTEMTIME st = { 1900 + temptm.tm_year,
		1 + temptm.tm_mon,
		temptm.tm_wday,
		temptm.tm_mday,
		temptm.tm_hour,
		temptm.tm_min,
		temptm.tm_sec,
		0 };
	return st;
}
/*
**SYSTEMTIME转time_t
*/
time_t SystemTimeToTime_t(const SYSTEMTIME& st)
{
	tm temptm = { st.wSecond,
		st.wMinute,
		st.wHour,
		st.wDay,
		st.wMonth - 1,
		st.wYear - 1900,
		st.wDayOfWeek,
		0,
		0 };

	return mktime(&temptm);
}

bool TimePlusOrMinus(const std::wstring &sStartTime, int iPlusOrMinus, std::wstring &sEndTime)
{
	bool bRet = false;
	if (sStartTime.length() != 14) return bRet;

	SYSTEMTIME st_time;
	st_time.wYear = _wtoi(sStartTime.substr(0, 4).c_str());
	st_time.wMonth = _wtoi(sStartTime.substr(4, 2).c_str());
	st_time.wDay = _wtoi(sStartTime.substr(6, 2).c_str());
	st_time.wHour = _wtoi(sStartTime.substr(8, 2).c_str());
	st_time.wMinute = _wtoi(sStartTime.substr(10, 2).c_str());
	st_time.wSecond = _wtoi(sStartTime.substr(12, 2).c_str());
	time_t tt_time = SystemTimeToTime_t(st_time);
	tt_time += iPlusOrMinus;
	st_time = Time_tToSystemTime(tt_time);
	wchar_t sz[20];
	swprintf(sz, 20, L"%d%02d%02d%02d%02d%02d", st_time.wYear, st_time.wMonth, st_time.wDay,
		st_time.wHour, st_time.wMinute, st_time.wSecond);
	sEndTime = sz;
	bRet = true;
	return bRet;
}

void SplitStr(const std::string &sSrc, const std::string &sDel, std::vector<std::string> &vect)
{
	std::string sTmp = sSrc;
	vect.clear();
	while (!sTmp.empty())
	{
		size_t iPos = sTmp.find(sDel);
		if (iPos != std::string::npos)
		{
			vect.push_back(sTmp.substr(0, iPos));
			sTmp = sTmp.substr(iPos + sDel.length());
		}
		else
		{
			vect.push_back(sTmp);
			sTmp.clear();
		}
	}
}

void SplitStr(const std::wstring &sSrc, const std::wstring &sDel, std::vector<std::wstring> &vect)
{
	std::wstring sTmp = sSrc;
	vect.clear();
	while (!sTmp.empty())
	{
		size_t iPos = sTmp.find(sDel);
		if (iPos != std::wstring::npos)
		{
			vect.push_back(sTmp.substr(0, iPos));
			sTmp = sTmp.substr(iPos + sDel.length());
		}
		else
		{
			vect.push_back(sTmp);
			sTmp.clear();
		}
	}
}

void ByteArray2HexStr(std::wstring &sDest, const BYTE *arrayByte, int size)
{
	wchar_t sz[10];
	sDest.clear();
	for (int i = 0; i<size; i++)
	{
		swprintf(sz, 10, L"%02X", arrayByte[i]);
		sDest += sz;
	}
}

int HexStr2ByteArray(const std::wstring &strIn, BYTE *arrayByte, int size)
{
	int nCount = 0;
	int iLen = strIn.length() / 2;
	wchar_t *pEnd;
	for (int i = 0; i<iLen && i<size; i++)
	{
		arrayByte[i] = wcstol(strIn.substr(i * 2, 2).c_str(), &pEnd, 16);
		nCount++;
	}
	return nCount;
}

void Int2ByteArray(int val, BYTE *arrayByte, int size)
{
	for (int i = 0; i<4 && i<size; i++)
	{
		arrayByte[size - i - 1] = (BYTE)(((UINT)val) >> (i * 8));
	}
}

int ByteArray2Int(const BYTE *arrayByte, int size)
{
	int val = 0;
	BYTE* pByte = (BYTE*)&val;
	for (int i = 0; i<4 && i<size; i++)
	{
		pByte[size - i - 1] = arrayByte[i];
	}
	return val;
}

void ByteToStr(unsigned char ucValue, wstring &Str)
{
	wchar_t szText[10];
	wsprintf(szText, _T("%d"), ucValue);
	Str = szText;
}

void ByteToStrHex(unsigned char ucValue, wstring &StrHex)
{
	wchar_t szText[10];
	wsprintf(szText, _T("%02X"), ucValue);
	StrHex = szText;
}

bool StrHexToByte(const wstring &StrHex, unsigned char &ucValue)
{
	bool bRet = false;
	unsigned char ucH = 0, ucL = 0;
	if (StrHex.length() != 2) return bRet;
	wstring sNumbers(_T("0123456789")), sHex(_T("0123456789ABCDEFabcdef"));
	if (sHex.find(StrHex.substr(0, 1)) == wstring::npos || sHex.find(StrHex.substr(1, 1)) == wstring::npos) return bRet;
	for (int i = 0; i < 2; i++)
	{
		unsigned char ucTmp = 0;
		wstring sTmp = StrHex.substr(i, 1);
		if (sNumbers.find(sTmp) != wstring::npos)
		{
			ucTmp = _wtoi(sTmp.c_str());
		}
		else if (sTmp == _T("A") || sTmp == _T("a"))
			ucTmp = 10;
		else if (sTmp == _T("B") || sTmp == _T("b"))
			ucTmp = 11;
		else if (sTmp == _T("C") || sTmp == _T("c"))
			ucTmp = 12;
		else if (sTmp == _T("D") || sTmp == _T("d"))
			ucTmp = 13;
		else if (sTmp == _T("E") || sTmp == _T("e"))
			ucTmp = 14;
		else if (sTmp == _T("F") || sTmp == _T("f"))
			ucTmp = 15;
		if (i == 0) ucH = ucTmp;
		else if (i == 1) ucL = ucTmp;
	}
	ucValue = ucH * 16 + ucL;
	bRet = true;
	return bRet;
}

CSimpleStringQueue::CSimpleStringQueue(int iIndex, int iMaxItems)
	:m_iIndex(iIndex), m_iMaxItems(iMaxItems)
{
	if (m_iIndex == 0)
	{
		srand(1000);
		m_iIndex=rand();
	}
	char szMsg[64];
	sprintf(szMsg, "_CSimpleStringQueue_%d", m_iIndex);
	m_hMutexConfig = CreateMutexA(NULL, FALSE, szMsg);
}

CSimpleStringQueue::~CSimpleStringQueue()
{
	if (m_hMutexConfig != NULL)
	{
		CloseHandle(m_hMutexConfig);
		m_hMutexConfig = NULL;
	}
}

int CSimpleStringQueue::PutAnItem(const string &item)
{
	LockMutex lock(m_hMutexConfig, INFINITE);
	if (m_vQueue.size() >= m_iMaxItems)
	{
		return -1;
	}
	bool bFound = false;
	for (int i = 0; i < m_vQueue.size(); i++)
	{
		if (m_vQueue[i] == item)
		{
			bFound = true;
			break;
		}
	}
	if (!bFound) m_vQueue.push_back(item);
	return m_vQueue.size();
}

int CSimpleStringQueue::GetAnItem(string &item, bool bRemove)
{
	LockMutex lock(m_hMutexConfig, INFINITE);
	int iRet = -1;
	if (m_vQueue.size() > 0)
	{
		vector<string>::const_iterator iter = m_vQueue.begin();
		item = *iter;
		if (bRemove) m_vQueue.erase(iter);
		iRet = m_vQueue.size();
	}
	return iRet;
}

void CSimpleStringQueue::Clone(vector<string> &other)
{
	LockMutex lock(m_hMutexConfig, INFINITE);
	other.clear();
	for (int i = 0; i<m_vQueue.size(); i++)
	{
		other.push_back(m_vQueue[i]);
	}
}

bool CSimpleStringQueue::RemoveItem(const string &item)
{
	LockMutex lock(m_hMutexConfig, INFINITE);
	bool bRet = false;

	vector<string>::iterator iter = m_vQueue.begin();
	while (iter != m_vQueue.end())
	{
		if (*iter == item)
		{
			m_vQueue.erase(iter);
			if (!bRet) bRet = true;
			iter = m_vQueue.begin();
		}
		else
			iter++;
	}
	return bRet;
}

void CSimpleStringQueue::RemoveAll(void)
{
	LockMutex lock(m_hMutexConfig, INFINITE);
	m_vQueue.clear();
}

CSimpleWStringQueue::CSimpleWStringQueue(int iIndex, int iMaxItems)
	:m_iIndex(iIndex), m_iMaxItems(iMaxItems)
{
	if (m_iIndex == 0)
	{
		srand(1000);
		m_iIndex = rand();
	}
	wchar_t szMsg[64];
	swprintf(szMsg, 64, L"_CSimpleStringQueue_%d", m_iIndex);
	m_hMutexConfig = CreateMutexW(NULL, FALSE, szMsg);
}

CSimpleWStringQueue::~CSimpleWStringQueue()
{
	if (m_hMutexConfig != NULL)
	{
		CloseHandle(m_hMutexConfig);
		m_hMutexConfig = NULL;
	}
}

int CSimpleWStringQueue::PutAnItem(const wstring &item)
{
	LockMutex lock(m_hMutexConfig, INFINITE);
	if (m_vQueue.size() >= m_iMaxItems)
	{
		return -1;
	}
	bool bFound = false;
	for (int i = 0; i < m_vQueue.size(); i++)
	{
		if (m_vQueue[i] == item)
		{
			bFound = true;
			break;
		}
	}
	if (!bFound) m_vQueue.push_back(item);
	return m_vQueue.size();
}

int CSimpleWStringQueue::GetAnItem(wstring &item, bool bRemove)
{
	LockMutex lock(m_hMutexConfig, INFINITE);
	item.clear();
	int iRet = -1;
	if (m_vQueue.size() > 0)
	{
		vector<wstring>::const_iterator iter = m_vQueue.begin();
		item = *iter;
		if (bRemove) m_vQueue.erase(iter);
		iRet = m_vQueue.size();
	}
	return iRet;
}

void CSimpleWStringQueue::Clone(vector<wstring> &other)
{
	LockMutex lock(m_hMutexConfig, INFINITE);
	other.clear();
	for (int i = 0; i<m_vQueue.size(); i++)
	{
		other.push_back(m_vQueue[i]);
	}
}

bool CSimpleWStringQueue::RemoveItem(const wstring &item)
{
	LockMutex lock(m_hMutexConfig, INFINITE);
	bool bRet = false;

	vector<wstring>::iterator iter = m_vQueue.begin();
	while (iter != m_vQueue.end())
	{
		if (*iter == item)
		{
			m_vQueue.erase(iter);
			if (!bRet) bRet = true;
			iter = m_vQueue.begin();
		}
		else
			iter++;
	}
	return bRet;
}

void CSimpleWStringQueue::RemoveAll(void)
{
	LockMutex lock(m_hMutexConfig, INFINITE);
	m_vQueue.clear();
}
/*
template <class T>
CASimpleQueue<T>::CASimpleQueue(int iIndex, int iMaxItems)
	:m_iIndex(iIndex), m_iMaxItems(iMaxItems)
{
	if (m_iIndex == 0)
	{
		srand(1000);
		m_iIndex = rand();
	}
	wchar_t szMsg[64];
	_swprintf(szMsg, L"_CSimpleStringQueue_%d", m_iIndex);
	m_hMutexConfig = CreateMutexW(NULL, FALSE, szMsg);
}

template <class T>
CASimpleQueue<T>::~CASimpleQueue()
{
	vector<T *>::iterator it = m_vQueue.begin();
	while (it != m_vQueue.end())
	{
		delete *it;
		*it = NULL;
		m_vQueue.erase(it);
		it = m_vQueue.begin();
	}
	if (m_hMutexConfig != NULL)
	{
		CloseHandle(m_hMutexConfig);
		m_hMutexConfig = NULL;
	}
}

template <class T>
int CASimpleQueue<T>::PutAnItem(const T &item)
{
	LockMutex lock(m_hMutexConfig, INFINITE);
	if (m_vQueue.size() >= m_iMaxItems)
	{
		return -1;
	}
	bool bFound = false;
	for (int i = 0; i < m_vQueue.size(); i++)
	{
		if (*(m_vQueue[i]) == item)
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		T *pT = new T(item);
		m_vQueue.push_back(pT);
	}
	return m_vQueue.size();
}

template <class T>
int CASimpleQueue<T>::GetAnItemPtr(T *item, bool bRemove)
{
	LockMutex lock(m_hMutexConfig, INFINITE);
	int iRet = -1;
	if (m_vQueue.size() > 0)
	{
		vector<wstring>::const_iterator iter = m_vQueue.begin();
		item = *iter;
		if (bRemove) m_vQueue.erase(iter);
		iRet = m_vQueue.size();
	}
	return iRet;
}

template <class T>
int CASimpleQueue<T>::GetAnItem(T &item, bool bRemove)
{
	LockMutex lock(m_hMutexConfig, INFINITE);
	int iRet = -1;
	if (m_vQueue.size() > 0)
	{
		vector<wstring>::const_iterator iter = m_vQueue.begin();
		item = *(*iter);
		if (bRemove) m_vQueue.erase(iter);
		iRet = m_vQueue.size();
	}
	return iRet;
}

template <class T>
bool CASimpleQueue<T>::RemoveItem(const T &item)
{
	LockMutex lock(m_hMutexConfig, INFINITE);
	bool bRet = false;

	vector<wstring>::iterator iter = m_vQueue.begin();
	while (iter != m_vQueue.end())
	{
		if (*(*iter) == item)
		{
			m_vQueue.erase(iter);
			if (!bRet) bRet = true;
			iter = m_vQueue.begin();
		}
		else
			iter++;
	}
	return bRet;
}
*/
void GetCurrentTimeStr(std::wstring &sTime, bool bDel)
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	wchar_t sz[64];
	if (bDel)
		swprintf(sz, 64, L"%04d-%02d-%02d %02d:%02d:%02d",
		    systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
	else
		swprintf(sz, 64, L"%04d%02d%02d%02d%02d%02d",
		systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
	sTime = sz;
}

void GetCurrentTimeStr(std::string &sTime, bool bDel)
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	char sz[64];
	if (bDel)
		sprintf(sz, "%04d-%02d-%02d %02d:%02d:%02d",
		systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
	else
		sprintf(sz, "%04d%02d%02d%02d%02d%02d",
		systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
	sTime = sz;
}

/*
下面的多项式值的得到, 是通过对Poly逆向计算得到的:

引用

0000 0100 1100 0001 0001 1101 1011 0111-->
0    4    C    1    1    D    B    7 |= == == == = 04C11DB7
<---------------------------------------- -
1110 1101 1011 1000 1000 0011 0010 0000
E    D    B    8     8   3    2    0 == == == == EDB88320
*/
unsigned int GetCrc32(char* InCharStr, unsigned int len)
{
	//生成Crc32的查询表   
	unsigned int Crc32Table[256];
	int i, j;
	unsigned int Crc;
	for (i = 0; i < 256; i++){
		Crc = i;
		for (j = 0; j < 8; j++){
			if (Crc & 1)
				Crc = (Crc >> 1) ^ 0xEDB88320;
			else
				Crc >>= 1;
		}
		Crc32Table[i] = Crc;
	}

	//开始计算CRC32校验值   
	Crc = 0xffffffff;
	for (int i = 0; i<len; i++){
		Crc = (Crc >> 8) ^ Crc32Table[(Crc & 0xFF) ^ InCharStr[i]];
	}

	Crc ^= 0xffffffff;
	return Crc;
}


unsigned short GetCrc16(char* InCharStr, unsigned int len)
{
	//生成Crc16的查询表   
	unsigned short Crc16Table[256];
	unsigned int i, j;
	unsigned short Crc;
	for (i = 0; i < 256; i++){
		Crc = i;
		for (j = 0; j < 8; j++){
			if (Crc & 0x1)
				Crc = (Crc >> 1) ^ 0xA001;
			else
				Crc >>= 1;
		}
		Crc16Table[i] = Crc;
	}

	//开始计算CRC16校验值   
	Crc = 0x0000;
	for (i = 0; i<len; i++){
		Crc = (Crc >> 8) ^ Crc16Table[(Crc & 0xFF) ^ InCharStr[i]];

	}
	//Crc ^= 0x0000;     
	return Crc;
}

unsigned int GetCrc32_Mpeg(char* InCharStr, unsigned int len)
{
	unsigned int i;
	unsigned int crc = 0xffffffff, j = 0;

	while ((len--) != 0)
	{
		crc ^= (unsigned int)InCharStr[j] << 24;
		j++;
		for (i = 0; i < 8; ++i)
		{
			if ((crc & 0x80000000) != 0)
				crc = (crc << 1) ^ 0x04C11DB7;
			else
				crc <<= 1;
		}
	}
	return crc;
}

unsigned int GetCrc32_Mpeg(const wstring &sInHexStr, wstring &sCrcOutput)
{
	BYTE arrByte[1024];
	int iLen = HexStr2ByteArray(sInHexStr, arrByte, 1024);
	unsigned int iCrc = GetCrc32_Mpeg((char *)arrByte, iLen);
	TCHAR szTmp[10];
	_stprintf(szTmp, _T("%08X"), iCrc);
	sCrcOutput = szTmp;

	return iCrc;
}
unsigned int GetCrc32(const wstring &sInHexStr, wstring &sCrcOutput)
{
	BYTE arrByte[1024];
	int iLen = HexStr2ByteArray(sInHexStr, arrByte, 1024);
	unsigned int iCrc = GetCrc32((char *)arrByte, iLen);
	TCHAR szTmp[10];
	_stprintf(szTmp, _T("%08X"), iCrc);
	sCrcOutput = szTmp;

	return iCrc;
}
unsigned int GetCrc16(const wstring &sInHexStr, wstring &sCrcOutput)
{
	BYTE arrByte[1024];
	int iLen = HexStr2ByteArray(sInHexStr, arrByte, 1024);
	unsigned int iCrc = GetCrc16((char *)arrByte, iLen);
	TCHAR szTmp[10];
	_stprintf(szTmp, _T("%04X"), iCrc);
	sCrcOutput = szTmp;

	return iCrc;
}

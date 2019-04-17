//The functions in the file are used to transform between variant UDP message and the universal message package sent by socket
#ifndef _INC_COMMON_UTILITY___
#define _INC_COMMON_UTILITY___
//#include <windows.h>
#include <string>
#include <vector>

using namespace std;
/*
int CreateMultiDir2(const char* szPath);
void GetAnotherDateFromCur(std::string &sTimeStr, int iDay);
void GetLogFolderWithDate(const std::string &sPath, std::string &sPathDate, int iPlusMinus);
int RemoveAllFilesInAFolder(const std::string &sFolderPath);
int RemoveAFolderN(const std::string &sFolderPath);
bool IsFileExists(const char* szFile);
time_t FileTimeToGMT(LPFILETIME pft);
time_t GetFileModifyTime(const char*filename, DWORD *pdwErrCode);
*/
BOOL GetWholeProductVersion(char *psFullPathFileName, char *psOVersion, char *psHeaderNumber, BOOL f4BAlign);
int StartProgram(const std::wstring &exefile, const std::wstring &cmdline, HANDLE &hprocess, DWORD* pPID = NULL, bool bWait = false);
int StartProgram(const std::string &cmdline, HANDLE &hprocess, DWORD* pPID = NULL, bool bWait = false);
void GetAppPath(TCHAR *pszPath, int iSize);
bool IsFileOrDirExists(const wchar_t *pszFile);
bool IsFileOrDirExists(const char *pszFile);
BOOL TestandCreateDirectory(const TCHAR *pszFile);

SYSTEMTIME Time_tToSystemTime(time_t t);
time_t SystemTimeToTime_t(const SYSTEMTIME& st);
//����ʼʱ��Ļ��������ӻ��߼��������룬�����µ�ʱ��
//sStartTime,��ʼʱ�䣬�����������ʽΪ������ʱ���룬��20171201101112��ʾ2017��12��1��10ʱ11��12��
//iPlusOrMinus�����ӻ��߼��ٵ���������λΪ�룬������ʾ���ӣ��������㣬������ʾ���٣���ǰ����
//sEndTime,����ʱ�䣬�����������ʽΪ������ʱ���룬��20171201101112��ʾ2017��12��1��10ʱ11��12��
//����ֵ��true-����ɹ� false-ʱ���ʽ���󣬼���ʧ��
bool TimePlusOrMinus(const std::wstring &sStartTime, int iPlusOrMinus, std::wstring &sEndTime);
void SplitStr(const std::wstring &sSrc, const std::wstring &sDel, std::vector<std::wstring> &vect);
void SplitStr(const std::string &sSrc, const std::string &sDel, std::vector<std::string> &vect);
void ByteArray2HexStr(std::wstring &sDest, const BYTE *arrayByte, int size);
int HexStr2ByteArray(const std::wstring &strIn, BYTE *arrayByte, int size);
void Int2ByteArray(int val, BYTE *arrayByte, int size);
int ByteArray2Int(const BYTE *arrayByte, int size);
//�ֽ�����ת����ʮ�����ַ���
void ByteToStr(unsigned char ucValue, wstring &Str);
//�ֽ�����ת����ʮ�������ַ���
void ByteToStrHex(unsigned char ucValue, wstring &StrHex);
//ʮ�������ַ���ת�����ֽ�����
bool StrHexToByte(const wstring &StrHex, unsigned char &ucValue);
//ȡ�õ�ǰʱ���ַ��������룬bDel���Ƿ���ʾ�ָ�����true-��
void GetCurrentTimeStr(std::wstring &sTime, bool bDel=false);
void GetCurrentTimeStr(std::string &sTime, bool bDel = false);
/*
*��������GetCrc32
*  ����ԭ�ͣ�unsigned int GetCrc32(char* InStr, unsigned int len)
*      ������InStr-- - ָ����Ҫ����CRC32ֵ���ַ���
*          len-- - ΪInStr�ĳ���
*      ����ֵΪ���������CRC32�����
*
*/
unsigned int GetCrc32(char* InCharStr, unsigned int len);
/*
ʮ�������ַ������룬ʮ������Crc�ַ������������CRC32���
*/
unsigned int GetCrc32(const wstring &sInHexStr, wstring &sCrcOutput);
/*
*  ��������GetCrc16
*  ����ԭ�ͣ�unsigned short GetCrc16(char* InStr, unsigned int len)
*      ������InStr-- - ָ����Ҫ����CRC32ֵ���ַ���
*          len-- - ΪInStr�ĳ���
*      ����ֵΪ���������CRC32�����
*/
unsigned short GetCrc16(char* InCharStr, unsigned int len);
/*
ʮ�������ַ������룬ʮ������Crc�ַ������������CRC32���
*/
unsigned int GetCrc16(const wstring &sInHexStr, wstring &sCrcOutput);

/*
CRC32/MPEG2
*/
unsigned int GetCrc32_Mpeg(char* InCharStr, unsigned int len);
/*
ʮ�������ַ������룬ʮ������Crc�ַ������������CRC32���
*/
unsigned int GetCrc32_Mpeg(const wstring &sInHexStr, wstring &sCrcOutput);

class LockMutex
{
public:
	//##ModelId=45360C25006D
	LockMutex(HANDLE hMutex, DWORD dwTimeoutSecond)
	{
		m_hMutex = NULL;
		m_bLock = false;
		m_bTimeout = false;
		DWORD dwWaitResult = 0;
		if (hMutex != NULL)
		{
			dwWaitResult = WaitForSingleObject(
				hMutex,   // handle to mutex
				(dwTimeoutSecond == INFINITE) ? dwTimeoutSecond = INFINITE : dwTimeoutSecond * 1000);   // 1min time-out interval
		}
		switch (dwWaitResult)
		{
		case WAIT_OBJECT_0:
		case WAIT_ABANDONED:
			m_hMutex = hMutex;
			m_bLock = true;
			break;
		case WAIT_TIMEOUT:
			m_bTimeout = true;
			break;
		}
	}
	//##ModelId=45360C25006F
	~LockMutex()
	{
		if (m_hMutex != NULL)
		{
			ReleaseMutex(m_hMutex);
		}
	}
	bool IsLocked()
	{
		return m_bLock;
	}
	bool IsTimeout()
	{
		return m_bTimeout;
	}
private:
	//##ModelId=45360C250070
	BOOL fOwn;
	//##ModelId=45360C2500AB
	HANDLE m_hMutex;
	bool m_bLock;
	bool m_bTimeout;
};

class LockCritSect
{
public:
	LockCritSect(CRITICAL_SECTION* pCritSect){
		EnterCriticalSection(pCritSect);
		m_pCritSect = pCritSect;
	}
	~LockCritSect() {
		LeaveCriticalSection(m_pCritSect);
	}

private:
	CRITICAL_SECTION* m_pCritSect;
};

class CSimpleStringQueue
{
public:
	CSimpleStringQueue(int iIndex, int iMaxItems);
	~CSimpleStringQueue();
	//return the size of queue.
	int PutAnItem(const string &item);
	//return the size of queue.
	int GetAnItem(string &item, bool bRemove = false);
	bool RemoveItem(const string &item);
	void Clone(vector<string> &other);
	void RemoveAll(void);
private:
	int m_iIndex;
	vector<string> m_vQueue;
	int m_iMaxItems;
	HANDLE m_hMutexConfig;
};

class CSimpleWStringQueue
{
public:
	CSimpleWStringQueue(int iIndex, int iMaxItems);
	~CSimpleWStringQueue();
	//return the size of queue.
	int PutAnItem(const wstring &item);
	//return the size of queue.
	int GetAnItem(wstring &item, bool bRemove = false);
	bool RemoveItem(const wstring &item);
	void Clone(vector<wstring> &other);
	void RemoveAll(void);
private:
	int m_iIndex;
	vector<wstring> m_vQueue;
	int m_iMaxItems;
	HANDLE m_hMutexConfig;
};
/*
template <class T>
class CASimpleQueue
{
public:
	CASimpleQueue(int iIndex, int iMaxItems);
	~CASimpleQueue();
	//return the size of queue.
	int PutAnItem(const T &item);
	//return the size of queue.
	int GetAnItemPtr(T *item, bool bRemove = false);
	//return the size of queue.
	int GetAnItem(T &item, bool bRemove = false);

	bool RemoveItem(const T &item);
private:
	int m_iIndex;
	std::vector<T *> m_vQueue;
	int m_iMaxItems;
	HANDLE m_hMutexConfig;
};
*/

 
#endif

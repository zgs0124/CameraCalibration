#pragma once
#include <string>
#include <wtypes.h>
#include <map>
#include <mutex>


using namespace std;

#define  MEMORYSIZE 10240 //每块内存的大小

struct MemoryAddr{
	std::string StrName;
	LPVOID pBuffer;
	HANDLE m_Handle;
};


class SharedMemory
{
public:
	SharedMemory();
	~SharedMemory(); 
public:
	int m_iMemoeryNum;        //开辟内存数
	std::mutex m_mapLock;
	map<int, MemoryAddr> m_mapMemoryInfo;
public:
	LPVOID Init(std::string MeName);
	//void   IntPutData(std::string MeName, LPVOID Lpvoid, int ibuflen);
	void IntPutData(std::string MeName, char *lpVoid, int ibuflen);
	LPVOID OutPutData(std::string MeName);
};


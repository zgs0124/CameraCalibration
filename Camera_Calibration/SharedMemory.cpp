#include "stdafx.h"
#include "SharedMemory.h"
#include "BasicFun.h"

SharedMemory::SharedMemory()
{
	m_iMemoeryNum = 0;
}

SharedMemory::~SharedMemory()
{
	try
	{
		m_mapLock.lock();
		for (auto it = m_mapMemoryInfo.begin(); it != m_mapMemoryInfo.end(); it++)
		{
			::UnmapViewOfFile(it->second.pBuffer);
			::CloseHandle(it->second.m_Handle);
		}
		m_mapMemoryInfo.clear();
		m_mapLock.unlock();
	}
	catch (...)
	{
	}
}

LPVOID SharedMemory::Init(std::string MeName)
{
	MemoryAddr mMemoryAddr;
	LPVOID pBuffer = NULL;
	// 首先试图打开一个命名的内存映射文件对象 
	HANDLE hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, ToCString(MeName));

	if (NULL == hMap)
	{    // 打开失败，创建之
		hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MEMORYSIZE, ToCString(MeName));
		if (NULL == hMap)
		{
			OutputDebugString(L"CreateFileMapping 函数执行失败！！！");
			return NULL;
		}
		// 映射对象的一个视图，得到指向共享内存的指针，设置里面的数据
		pBuffer = ::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	}
	else
	{    // 打开成功，映射对象的一个视图，得到指向共享内存的指针，显示出里面的数据
		pBuffer = ::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	}

	mMemoryAddr.StrName = MeName.c_str();
	mMemoryAddr.pBuffer = pBuffer;
	mMemoryAddr.m_Handle = hMap;

	m_mapLock.lock();
	m_mapMemoryInfo.insert(pair<int, MemoryAddr>(m_iMemoeryNum, mMemoryAddr));
	m_mapLock.unlock();

	m_iMemoeryNum++;

	return pBuffer;
}

void SharedMemory::IntPutData(std::string MeName, char *lpVoid, int ibuflen)
{
	CString out;
	char tmp[256] = { 0 };
	int i = sizeof(TCHAR);
	m_mapLock.lock();
	for (auto itr = m_mapMemoryInfo.begin(); itr != m_mapMemoryInfo.end(); itr++)
	{
		if (MeName == itr->second.StrName)
		{
			memcpy(itr->second.pBuffer, lpVoid, ibuflen);
		}
	}
	m_mapLock.unlock();
}

LPVOID SharedMemory::OutPutData(std::string MeName)
{
	LPVOID pBuffer = NULL;
	m_mapLock.lock();
	for (auto iter = m_mapMemoryInfo.begin(); iter != m_mapMemoryInfo.end(); iter++)
	{
		if (iter->second.StrName == MeName)
		{
			pBuffer = iter->second.pBuffer;
			break;
		}
	}
	m_mapLock.unlock();
	return pBuffer;
}


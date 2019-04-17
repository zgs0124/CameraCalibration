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
	// ������ͼ��һ���������ڴ�ӳ���ļ����� 
	HANDLE hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, ToCString(MeName));

	if (NULL == hMap)
	{    // ��ʧ�ܣ�����֮
		hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MEMORYSIZE, ToCString(MeName));
		if (NULL == hMap)
		{
			OutputDebugString(L"CreateFileMapping ����ִ��ʧ�ܣ�����");
			return NULL;
		}
		// ӳ������һ����ͼ���õ�ָ�����ڴ��ָ�룬�������������
		pBuffer = ::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	}
	else
	{    // �򿪳ɹ���ӳ������һ����ͼ���õ�ָ�����ڴ��ָ�룬��ʾ�����������
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



// Camera_Calibration.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCamera_CalibrationApp: 
// �йش����ʵ�֣������ Camera_Calibration.cpp
//

class CCamera_CalibrationApp : public CWinApp
{
public:
	CCamera_CalibrationApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCamera_CalibrationApp theApp;
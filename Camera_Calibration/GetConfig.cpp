#include "stdafx.h"
#include "GetConfig.h"
#include "BasicFun.h"


CGetConfig::CGetConfig()
{
}


CGetConfig::~CGetConfig()
{
}

bool CGetConfig::ReadConfig()
{
	char szExePath[_MAX_PATH] = {};
	memset(szExePath, 0, MAX_PATH);
	GetModuleFileNameA(NULL, szExePath, MAX_PATH);
	m_CurrentPath = szExePath;
	if (m_CurrentPath.length() > 0)
	{
		size_t iPos = m_CurrentPath.rfind('\\');
		m_ExeName = m_CurrentPath.substr(iPos + 1, m_CurrentPath.length());
		m_CurrentPath = m_CurrentPath.substr(0, iPos + 1);
	}
	std::string configpath = m_CurrentPath + CONFIG_NAME;

	char tmpc[256] = { 0 };
	memset(tmpc, sizeof(tmpc), 0);
	GetPrivateProfileStringA("CAMERA", "Show_Pic", "0", tmpc, sizeof(tmpc), configpath.c_str());
	if (0 == atoi((char *)tmpc)){
		m_bShowPic = false;
	}else if (1 == atoi((char *)tmpc)){
		m_bShowPic = true;
	}else{
		LOG(INFO) << _T("配置文件中的 Show_Pic 错误！\n");
	}

	memset(tmpc, sizeof(tmpc), 0);
	GetPrivateProfileStringA("CAMERA", "Save_Pic", "0", tmpc, sizeof(tmpc), configpath.c_str());
	if (0 == atoi((char *)tmpc)){
		m_bSavePic = false;
	}
	else if (1 == atoi((char *)tmpc)){
		m_bSavePic = true;
	}
	else{
		LOG(INFO) << _T("配置文件中的 Save_Pic 错误！\n");
	}

	memset(tmpc, sizeof(tmpc), 0);
	GetPrivateProfileStringA("CAMERA", "Save_Pic_Path", "", tmpc, sizeof(tmpc), configpath.c_str());
	m_SavePicPath = tmpc;

	memset(tmpc, sizeof(tmpc), 0);
	GetPrivateProfileStringA("CAMERA", "MAX_Pic", "2000", tmpc, sizeof(tmpc), configpath.c_str());
	m_MaxSave = atoi(tmpc);

	memset(tmpc, sizeof(tmpc), 0);
	GetPrivateProfileStringA("CAMERA", "Log_Path", "", tmpc, sizeof(tmpc), configpath.c_str());
	m_LogPath = tmpc;

	memset(tmpc, sizeof(tmpc), 0);
	GetPrivateProfileStringA("CAMERA", "BiaoDingPath", "", tmpc, sizeof(tmpc), configpath.c_str());
	m_BiaodingPath = tmpc;
	CreateDir(m_BiaodingPath.c_str());

	memset(tmpc, sizeof(tmpc), 0);
	GetPrivateProfileStringA("CAMERA", "Pixel2Size_X", "", tmpc, sizeof(tmpc), configpath.c_str());
	m_Pix2Size_x = (float)atof(tmpc);

	memset(tmpc, sizeof(tmpc), 0);
	GetPrivateProfileStringA("CAMERA", "Pixel2Size_Y", "", tmpc, sizeof(tmpc), configpath.c_str());
	m_Pix2Size_y = (float)atof(tmpc);

	memset(tmpc, sizeof(tmpc), 0);
	GetPrivateProfileStringA("CAMERA", "3D_Path", "", tmpc, sizeof(tmpc), configpath.c_str());
	m_3DPicPath = tmpc;
	CreateDir(m_3DPicPath.c_str());

	memset(tmpc, sizeof(tmpc), 0);
	GetPrivateProfileStringA("CAMERA", "moveValueX", "", tmpc, sizeof(tmpc), configpath.c_str());
	m_PointMove.x = (float)atof(tmpc);

	memset(tmpc, sizeof(tmpc), 0);
	GetPrivateProfileStringA("CAMERA", "moveValueY", "", tmpc, sizeof(tmpc), configpath.c_str());
	m_PointMove.y = (float)atof(tmpc);

	memset(tmpc, sizeof(tmpc), 0);
	GetPrivateProfileStringA("CAMERA", "TransForm", "", tmpc, sizeof(tmpc), configpath.c_str());
	std::string str = tmpc;
	std::string str1;
	m_TransForm.create(3, 3, CV_64FC1);
	size_t iPos = 0;
	for (int i = 0; i < 3; i++)
	{
		double *ptr = m_TransForm.ptr<double>(i, 0);
		for (int j = 0; j < 3; j++)
		{
			iPos = str.find(',');
			str1 = str.substr(0, iPos);
			str = str.substr(iPos + 1, str.length());
			*(m_TransForm.ptr<double>(i, j)) = atof(str1.c_str());
		}
	}
	//double dd[9] = { 0.0 };
	//for (int ix = 0; ix < 3; ix++)
	//{
	//	double *ptr = m_TransForm.ptr<double>(ix, 0);
	//	for (int iy = 0; iy < 3; iy++)
	//	{
	//		dd[ix * 3 + iy] = *ptr;
	//		ptr++;
	//	}
	//}
	return true;
}


bool &CGetConfig::show_pic()
{
	return m_bShowPic;
}

bool &CGetConfig::GetSavePic()
{
	return m_bSavePic;
}
void CGetConfig::SetSavePic(bool ifsave)
{
	m_bSavePic = ifsave;
}

std::string &CGetConfig::GetSavePicPath()
{
	return m_SavePicPath;
}

int &CGetConfig::GetMaxPic()
{
	return m_MaxSave;
}

void CGetConfig::SetLogPath(std::string path)
{
	m_LogPath = path;
}
std::string &CGetConfig::GetLogPath()
{
	return m_LogPath;
}

void CGetConfig::SetBiaoDingPath(std::string path)
{
	m_BiaodingPath = path;
}
std::string &CGetConfig::GetBiaoDingPath()
{
	return m_BiaodingPath;
}

void CGetConfig::SetPix2Size_X(float sz)
{
	m_Pix2Size_x = sz;
}
float &CGetConfig::GetPix2Size_X()
{
	return m_Pix2Size_x;
}
void CGetConfig::SetPix2Size_Y(float sz)
{
	m_Pix2Size_y = sz;
}
float &CGetConfig::GetPix2Size_Y()
{
	return m_Pix2Size_y;
}
void CGetConfig::Set3DPicPath(std::string path)
{
	m_3DPicPath = path;
}
std::string &CGetConfig::Get3DPicPath()
{
	return m_3DPicPath;
}
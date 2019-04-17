#pragma once
#include "Camera.h"


#include <pylon/PylonIncludes.h>
#include <pylon/ConfigurationEventHandler.h>
#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif
#include <pylon/gige/PylonGigEIncludes.h>
#include <pylon/gige/ActionTriggerConfiguration.h>


#define POINT_NUM_X 8		//标定板X轴上的顶点数
#define POINT_NUM_Y	6		//标定板Y轴上的顶点数
#define STANDARD_LEN_X	5	//标定板X轴上顶点之间的距离（单位：毫米）
#define STANDARD_LEN_Y	5	//标定板Y轴上顶点之间的距离（单位：毫米）

using namespace Pylon;

typedef Pylon::CBaslerGigEInstantCamera Camera_GigE;
//typedef Pylon::CBaslerGigEInstantCameraArray Camera_t;
using namespace Basler_GigECameraParams;

static const uint32_t c_maxCamerasToUse = 2;

// Simple helper class to set the HeartbeatTimeout safely.
class CHeartbeatHelper
{
public:
	explicit CHeartbeatHelper(CInstantCamera& camera)
		: m_pHeartbeatTimeout(NULL)
	{
		// m_pHeartbeatTimeout may be NULL
		m_pHeartbeatTimeout = camera.GetTLNodeMap().GetNode("HeartbeatTimeout");
	}

	bool SetValue(int64_t NewValue)
	{
		// Do nothing if no heartbeat feature is available.
		if (!m_pHeartbeatTimeout.IsValid())
			return false;

		// Apply the increment and cut off invalid values if neccessary.
		int64_t correctedValue = NewValue - (NewValue % m_pHeartbeatTimeout->GetInc());

		m_pHeartbeatTimeout->SetValue(correctedValue);
		return true;
	}

	bool SetMax()
	{
		// Do nothing if no heartbeat feature is available.
		if (!m_pHeartbeatTimeout.IsValid())
			return false;

		int64_t maxVal = m_pHeartbeatTimeout->GetMax();
		return SetValue(maxVal);
	}

protected:
	GenApi::CIntegerPtr m_pHeartbeatTimeout; // Pointer to the node, will be NULL if no node exists.
};

class CBaslerGigE :public CCamera
{
private:
	CGrabResultPtr m_ptrGrabResult;
	CBaslerGigEInstantCameraArray m_cameras;
	DeviceInfoList_t m_usableDeviceInfos;
	std::string m_CameraIP;
	std::string m_CameraName;
	std::map<std::string, cv::Mat> m_mSrcImg;
	std::mutex m_mtxImg;
	std::thread m_GrabThread;
public:

private:
	void Grab();
	bool Add_ImgMap();
	void Save_ImgMap();
	void GetSaveImgPath();
public:
	CBaslerGigE();
	CBaslerGigE(const std::string &ip);
	~CBaslerGigE();

	bool ConnectCamera();
	bool DisConnectCamera();
	bool StartGrab();
	bool StopGrab();

	bool Get_Img(cv::Mat &img);
	CBaslerGigE operator+(const CBaslerGigE& basler) const;

	void SetSavePic(bool save);
	void SetAOI(const int width, const int height, int offect_x = 0, int offect_y = 0);
	void SetExpouseTime(double &time);
	double GetExpouseTime();
	void SetFrameRate(const double frame, bool ifauto = false);
	int GetFrameRate();
	//设置等待触发模式
	void WaitForTrigger();


};


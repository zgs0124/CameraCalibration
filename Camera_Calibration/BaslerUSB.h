#pragma once
#include "Camera.h"
#ifndef USE_USB
#define USE_USB
#endif

#include <pylon/PylonIncludes.h>
#include <pylon/ConfigurationEventHandler.h>
using namespace Pylon;
#if defined( USE_USB )
// Settings for using Basler USB cameras.
#include <pylon/usb/BaslerUsbInstantCamera.h>
typedef Pylon::CBaslerUsbInstantCamera Camera_usb;
//typedef CBaslerUsbCameraEventHandler CameraEventHandler_t; // Or use Camera_t::CameraEventHandler_t
using namespace Basler_UsbCameraParams;
#else
#error Camera type is not specified. For example, define USE_USB for using USB cameras.
#endif


// Simple helper class to set the HeartbeatTimeout safely.
class CHeartbeatHelper_USB
{
public:
	explicit CHeartbeatHelper_USB(CInstantCamera& camera)
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

// When using Device Specific Instant Camera classes there are specific Configuration event handler classes available which can be used, for example
// Pylon::CBaslerGigEConfigurationEventHandler or Pylon::CBasler1394ConfigurationEventHandler
//Example of a configuration event handler that handles device removal events.
class CSampleConfigurationEventHandler : public Pylon::CConfigurationEventHandler
{
public:
	// This method is called from a different thread when the camera device removal has been detected.
	void OnCameraDeviceRemoved(CInstantCamera& /*camera*/)
	{
		// Print two new lines, just for improving printed output.
		std::cout << std::endl << std::endl;
		std::cout << "CSampleConfigurationEventHandler::OnCameraDeviceRemoved called." << std::endl;
	}
};


class CBaslerUSB :public CCamera
{
private:
	CDeviceInfo m_info;
	Camera_usb m_camera;
	CGrabResultPtr ptrGrabResult;

	bool m_bStartSave;				//开始保存内存中的图片
	//std::vector<cv::Mat> m_vSavePic;
	std::map<std::string, cv::Mat> m_mSavePic;
	bool m_RunThread;
	std::thread m_ThreadGrab;
	std::thread m_ThreadSavePic;
	int m_iPos;						//图片显示计数器
	double		m_ASpeed_x;			//x方向加速度
	double		m_ASpeed_y;			//y方向加速度
	cv::Point m_Last, m_Next;
public:

private:
	void Grab();
	void GetCameraConfig();
	void GetSaveImgPath();
	void SavePic_Map();
	void OutPutResult();
	void ImgProcess(cv::Mat img);	//图像处理函数
public:
	CBaslerUSB();
	~CBaslerUSB();

	bool ConnectCamera();
	bool DisConnectCamera();
	bool StartGrab();
	bool StopGrab();

	void SetAOI();
	void SetFrameRate(int &rate);
	int GetFrameRate();
	void SetExposureTime(double &time);
	double GetExposureTime();
};

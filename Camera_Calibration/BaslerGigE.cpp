#include "stdafx.h"
#include "BaslerGigE.h"

namespace Pylon
{
	class CInstantCamera;

	class CConfigurationEventPrinter : public CConfigurationEventHandler
	{
	public:
		void OnAttach(CInstantCamera& /*camera*/)
		{
			std::cout << "OnAttach event" << std::endl;
		}

		void OnAttached(CInstantCamera& camera)
		{
			std::cout << "OnAttached event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		}

		void OnOpen(CInstantCamera& camera)
		{
			std::cout << "OnOpen event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		}

		void OnOpened(CInstantCamera& camera)
		{
			std::cout << "OnOpened event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		}

		void OnGrabStart(CInstantCamera& camera)
		{
			std::cout << "OnGrabStart event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		}

		void OnGrabStarted(CInstantCamera& camera)
		{
			std::cout << "OnGrabStarted event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		}

		void OnGrabStop(CInstantCamera& camera)
		{
			std::cout << "OnGrabStop event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		}

		void OnGrabStopped(CInstantCamera& camera)
		{
			std::cout << "OnGrabStopped event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		}

		void OnClose(CInstantCamera& camera)
		{
			std::cout << "OnClose event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		}

		void OnClosed(CInstantCamera& camera)
		{
			std::cout << "OnClosed event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		}

		void OnDestroy(CInstantCamera& camera)
		{
			std::cout << "OnDestroy event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		}

		void OnDestroyed(CInstantCamera& /*camera*/)
		{
			std::cout << "OnDestroyed event" << std::endl;
		}

		void OnDetach(CInstantCamera& camera)
		{
			std::cout << "OnDetach event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		}

		void OnDetached(CInstantCamera& camera)
		{
			std::cout << "OnDetached event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		}

		void OnGrabError(CInstantCamera& camera, const String_t errorMessage)
		{
			std::cout << "OnGrabError event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
			std::cout << "Error Message: " << errorMessage << std::endl;
		}

		void OnCameraDeviceRemoved(CInstantCamera& camera)
		{
			std::cout << "OnCameraDeviceRemoved event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		}
	};
}

CBaslerGigE::CBaslerGigE()
{
	m_bSavePic = false;
	PylonInitialize();
}
CBaslerGigE::CBaslerGigE(const std::string &ip) :m_CameraIP(ip)
{
	m_bSavePic = false;
	std::cout << m_CameraIP << std::endl;
	PylonInitialize();
}

CBaslerGigE::~CBaslerGigE()
{
	StopGrab();
	DisConnectCamera();
	if (vContours.size() > 0)
	{
		vContours.clear();
	}
	if (vHierarchy.size() > 0)
	{
		vHierarchy.clear();
	}
	if (image_points_buf.size() > 0)
	{
		image_points_buf.clear();
	}
	if (image_points_seq.size() > 0)
	{
		image_points_seq.clear();
	}
	//m_cameras.DestroyDevice();
	PylonTerminate();
	google::ShutdownGoogleLogging();
	std::cout << "delete CBasler" << std::endl;
}

bool CBaslerGigE::ConnectCamera()
{
	std::string err;
	std::string strout;
	try
	{
		CTlFactory& tlFactory = CTlFactory::GetInstance();
		IGigETransportLayer *pTL = dynamic_cast<IGigETransportLayer*>(tlFactory.CreateTl(BaslerGigEDeviceClass));
		if (pTL == NULL)
		{
			throw RUNTIME_EXCEPTION("No GigE transport layer available.");
			err = "No GigE transport layer available.";
			Add_list(err);
			return false;
		}
		DeviceInfoList_t allDeviceInfos;
		if (pTL->EnumerateDevices(allDeviceInfos) == 0)
		{
			throw RUNTIME_EXCEPTION("No GigE cameras present.");
			err = "No GigE cameras present.";
			Add_list(err);
			return false;
		}
		// Only use cameras in the same subnet as the first one.

		m_usableDeviceInfos.push_back(allDeviceInfos[0]);
		const String_t subnet(static_cast<const CBaslerGigEDeviceInfo&>(allDeviceInfos[0]).GetSubnetAddress());

		// Start with index 1 as we have already added the first one above.
		// We will also limit the number of cameras to c_maxCamerasToUse.
		//int iNum = allDeviceInfos.size();
		for (size_t i = 1; i < allDeviceInfos.size() && m_usableDeviceInfos.size() < c_maxCamerasToUse; ++i)
		{
			const CBaslerGigEDeviceInfo& gigeinfo = static_cast<const CBaslerGigEDeviceInfo&>(allDeviceInfos[i]);
			if (subnet == gigeinfo.GetSubnetAddress())
			{
				// Add this deviceInfo to the ones we will be using.
				m_usableDeviceInfos.push_back(gigeinfo);
			}
			else
			{
				//std::cerr << "Camera will not be used because it is in a different subnet " << subnet << "!" << std::endl;
				sprintf((char *)err.c_str(), "Camera will not be used because it is in a different subnet %s !", subnet);
				//g_list.push_back(err);
				Add_list(err);
				return false;
			}
		}
		m_cameras.Initialize(m_usableDeviceInfos.size());
		//iNum = m_cameras.GetSize();
		for (int i = 0; i < m_cameras.GetSize(); i++)
		{
			m_cameras[i].Attach(tlFactory.CreateDevice(m_usableDeviceInfos[i]));
			std::string str1 = m_cameras[i].GetDeviceInfo().GetIpAddress();
			if (str1 == m_CameraIP)
			{
				m_cameras[i].Open();
				//m_cameras[i].GetStreamGrabberParams().TransmissionType = TransmissionType_Multicast;
				SetAOI(640, 480);
				CHeartbeatHelper heartbeatHelper(m_cameras[i]);
				heartbeatHelper.SetValue(1000);  // 1000 ms timeout
				m_bConnect = true;

				Add_list("连接相机成功！");
				return true;
			}
			else
				Add_list("连接相机失败！");
			continue;
		}
		return false;
	}
	catch (const GenericException &e)
	{
		// Error handling.
		std::cerr << "An exception occurred." << std::endl << e.GetDescription() << std::endl;
		Add_list("An exception occurred.\n");
		return false;
	}
}

bool CBaslerGigE::DisConnectCamera()
{
	m_bConnect = false;
	m_cameras.Close();
	m_cameras.DestroyDevice();
	return true;
}

bool CBaslerGigE::StartGrab()
{
	if (m_bConnect)
	{
		m_bGrab = true;
		m_GrabThread = std::thread(std::mem_fun(&CBaslerGigE::Grab), this);
		return true;
	}
	else
		return false;
}
void CBaslerGigE::Grab()
{
	char Out[1024] = {};
	int iNum = 0;
	for (int i = 0; i < m_cameras.GetSize(); i++)
	{
		std::string str1 = m_cameras[i].GetDeviceInfo().GetIpAddress();
		if (str1 == m_CameraIP)
		{
			if (m_cameras[i].IsOpen())
			{
				//SetFrameRate(13);
				m_cameras[i].StartGrabbing(GrabStrategy_OneByOne);
				bool first = true;
				int iNum = 0;
				double t_begin, t_end;
				t_begin = clock();
				CImagePersistenceOptions additionalOptions;
				additionalOptions.SetQuality(100);
				while (m_cameras[i].IsGrabbing() && m_bGrab)
				{
					// Wait for an image and then retrieve it. A timeout of 5000 ms is used.
					m_cameras[i].RetrieveResult(INFINITE, m_ptrGrabResult, TimeoutHandling_ThrowException);
					if (m_ptrGrabResult->GrabSucceeded())
					{
						if (first)
						{
							m_iImgWidth = m_ptrGrabResult->GetWidth();
							m_iImgHeight = m_ptrGrabResult->GetHeight();
							m_Img.create(m_iImgHeight, m_iImgWidth, CV_8UC1);
							memset(Out, 0, 1024);
							sprintf(Out, "图像宽高：%d x %d", m_iImgWidth, m_iImgHeight);
							first = false;
						}
						//size_t iSize = m_ptrGrabResult->GetImageSize();
						const uint8_t *pImageBuffer = (uint8_t *)m_ptrGrabResult->GetBuffer();
						memset(m_Img.data, 0, m_iImgWidth * m_iImgHeight);
						memcpy(m_Img.data, pImageBuffer, m_iImgWidth * m_iImgHeight);
						//是否显示图像
						if (m_bShowPic)
						{
							//if (GetFrameRate() > 25)
							//{
							//	iNum++;
							//	if (GetFrameRate() / iNum <= 25)
							//	{
							//		Add_imgGrabList(m_Img);
							//		iNum = 0;
							//	}
							//}else
								Add_imgGrabList(m_Img);
						}
							
						if (m_bTransForm)
						{
							GetSaveImgPath();
							ImgTransForm(m_PicName, m_Img, m_Result, cv::Size(m_iImgWidth, m_iImgHeight));
						}

						if (m_PicSavePath.length() > 0 && m_bSavePic)
						{
							std::string strpath = "";
							SetPicName();
							if (m_PicName.length() > 0)
							{
								strpath = m_PicSavePath + m_PicName;
								Add_list(strpath);
								CImagePersistence::Save(ImageFileFormat_Jpeg, strpath.c_str(), m_ptrGrabResult);
								Add_imgGrabList(m_Img);
								Add_list("保存图片成功！");
								m_bSavePic = false;
								break;
							}
						}
						m_bSavePic = false;
						iNum++;
						t_end = clock();
						if (t_end - t_begin >= 1000)
						{
							////std::cout << "帧率：" << (iNum - iFrameRate) << std::endl;
							memset(Out, 0, 1024);
							sprintf(Out, "%s :帧率： %d", m_CameraIP.c_str(), iNum);
							Add_list(Out);
							t_begin = t_end;
							m_iFrameRate = iNum;
							iNum = 0;
						}
#ifdef PYLON_WIN_BUILD
						//Display the grabbed image.
						//Pylon::DisplayImage(1, m_ptrGrabResult);
#endif
					}
					else
					{
						std::cout << "Error: " << m_ptrGrabResult->GetErrorCode() << " " << m_ptrGrabResult->GetErrorDescription() << std::endl;
					}
				}
			}
		}
	}
}
bool CBaslerGigE::StopGrab()
{
	std::string strout;
	if (m_bGrab){
		m_bGrab = false;
		m_GrabThread.join();
	}
	if (m_cameras.GetSize() > 0)
	{
		for (int i = 0; i < m_cameras.GetSize(); i++)
		{
			std::string str1 = m_cameras[i].GetDeviceInfo().GetIpAddress();
			if (str1 == m_CameraIP)
			{
				m_cameras[i].StopGrabbing();
				strout.append(m_CameraIP);
				strout.append(":相机停止采集！");
				Add_list(strout);
				return true;
			}
			else
				continue;
		}
	}
	else
		return false;
	return true;
}

void CBaslerGigE::SetExpouseTime(double &time)
{
	for (int i = 0; i < m_cameras.GetSize(); i++)
	{
		std::string str1 = m_cameras[i].GetDeviceInfo().GetIpAddress();
		if (str1.compare(m_CameraIP) == 0)
			m_cameras[i].ExposureTimeAbs.SetValue(time);
	}
}


void CBaslerGigE::SetSavePic(bool save)
{
	m_bSavePic = save;
}

double CBaslerGigE::GetExpouseTime()
{
	double time = 0;
	for (int i = 0; i < m_cameras.GetSize(); i++)
	{
		std::string str1 = m_cameras[i].GetDeviceInfo().GetIpAddress();
		if (str1.compare(m_CameraIP) == 0)
			time = m_cameras[i].ExposureTimeAbs.GetValue();
	}
	return time;
}

void CBaslerGigE::SetFrameRate(const double frame,bool ifauto)
{
	for (int i = 0; i < m_cameras.GetSize(); i++)
	{
		std::string str1 = m_cameras[i].GetDeviceInfo().GetIpAddress();
		if (str1.compare(m_CameraIP) == 0 && ifauto)
		{
			m_cameras[i].AcquisitionFrameRateEnable.SetValue(ifauto);
			m_cameras[i].AcquisitionFrameRateAbs.SetValue(frame);
		}
		else
		{
			m_cameras[i].AcquisitionFrameRateEnable.SetValue(ifauto);
		}
	}
}

int CBaslerGigE::GetFrameRate()
{
	//char tmp[MAX_PATH] = { 0 };
	for (int i = 0; i < m_cameras.GetSize(); i++)
	{
		std::string str1 = m_cameras[i].GetDeviceInfo().GetIpAddress();
		if (str1.compare(m_CameraIP) == 0)
			m_iFrameRate = (int)m_cameras[i].AcquisitionFrameRateAbs.GetValue();
	}
	//sprintf_s(tmp, "相机:%s, 帧率: %d", m_CameraIP.c_str(), m_iFrameRate);
	//Add_list(tmp);
	return m_iFrameRate;
}

void CBaslerGigE::SetAOI(const int width, const int height, int offect_x, int offect_y)
{
	for (int i = 0; i < m_cameras.GetSize(); i++)
	{
		std::string str1 = m_cameras[i].GetDeviceInfo().GetIpAddress();
		if (str1.compare(m_CameraIP) == 0){
			m_cameras[i].Width.SetValue(width);
			m_cameras[i].Height.SetValue(height);
			m_cameras[i].OffsetX.SetValue(offect_x);
			m_cameras[i].OffsetY.SetValue(offect_y);
		}
	}
}

void CBaslerGigE::WaitForTrigger()
{
	for (int i = 0; i < m_cameras.GetSize(); i++)
	{
		std::string str1 = m_cameras[i].GetDeviceInfo().GetIpAddress();
		if (str1.compare(m_CameraIP) == 0){
			m_cameras[i].TriggerSelector.SetValue(TriggerSelector_FrameStart);
			m_cameras[i].TriggerMode.SetValue(TriggerMode_On);
			m_cameras[i].TriggerSource.SetValue(TriggerSource_Line1);
			m_cameras[i].TriggerActivation.SetValue(TriggerActivation_RisingEdge);
		}
	}
}

void CBaslerGigE::GetSaveImgPath()
{
	std::string str;
	if (g_config.GetSavePicPath().length() > 0)
	{
		GetTime(1, str);
		m_CurrentPicPath = g_config.GetSavePicPath() + "\\" + str + "\\";
		CreateDir(m_CurrentPicPath.c_str());
		m_Time = getTime();
		char name[128] = { 0 };
		sprintf(name, "%s%s%s", m_Time.tmp0, m_Time.tmp1, m_Time.tmp2);
		m_PicName = m_CurrentPicPath + name;
		m_PicName.append(".jpg");
		//if (ifsave)
		//CImagePersistence::Save(ImageFileFormat_Jpeg, m_PicName.c_str(), ptrGrabResult);
	}
}

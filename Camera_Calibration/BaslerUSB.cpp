#include "stdafx.h"
#include "BaslerUSB.h"


CBaslerUSB::CBaslerUSB()
{
	m_bStartSave = false;
	m_iPos = 0;
}

CBaslerUSB::~CBaslerUSB()
{
	StopGrab();
	DisConnectCamera();

	if (m_mSavePic.size() > 0)
	{
		m_mSavePic.clear();
		ASSERT(m_mSavePic.size() == 0);
	}
}

bool CBaslerUSB::ConnectCamera()
{
	if (!m_bConnect)
	{
		PylonInitialize();
		CTlFactory& tlFactory = CTlFactory::GetInstance();
		m_camera.Attach(tlFactory.CreateDevice(m_info));
		m_camera.Open();
		SetAOI();
		m_bConnect = true;
		return true;
	}
	else
		return false;
}

bool CBaslerUSB::DisConnectCamera()
{
	if (m_bConnect)
	{
		m_camera.Close();
		m_camera.DestroyDevice();
		PylonTerminate();
		m_bConnect = false;
		return true;
	}
	else
		return false;
}

bool CBaslerUSB::StartGrab()
{
	if (m_bConnect && !m_bGrab)
	{
		m_bGrab = true;
		m_ThreadGrab = std::thread(std::mem_fun(&CBaslerUSB::Grab), this);
		m_ThreadSavePic = std::thread(std::mem_fun(&CBaslerUSB::SavePic_Map), this);
		//m_vSavePic.reserve(g_config.GetMaxPic());
		return true;
	}
	else
		return false;
}

std::vector<std::vector<cv::Point>> vContours;
std::vector<cv::Vec4i> vHierarchy;
void CBaslerUSB::Grab()
{
	int iNum = 0;
	int iRate = 0;
	double t_beg, t_end;
	int iTime = 0;
	// Camera.StopGrabbing() is called automatically by the RetrieveResult() method
	// when c_countOfImagesToGrab images have been retrieved.
	t_beg = clock();
	const uint8_t *pImageBuffer;
	CString str;
	char tmp[128] = { 0 };
	m_camera.StartGrabbing(GrabStrategy_OneByOne);
	m_camera.RegisterConfiguration(new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);
	while (m_camera.IsGrabbing() && m_bGrab)
	{
		// Wait for an image and then retrieve it. A timeout of 5000 ms is used.
		//m_camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
		m_camera.RetrieveResult(INFINITE, ptrGrabResult, TimeoutHandling_ThrowException);

		// Image grabbed successfully?
		if (ptrGrabResult->GrabSucceeded())
		{
			if (iNum == 0)
			{
				GetCameraConfig();
				m_Img.create(m_iImgHeight, m_iImgWidth, CV_8UC1);
				iNum++;
			}
			pImageBuffer = (uint8_t *)ptrGrabResult->GetBuffer();
			memset(m_Img.data, 0, m_iImgWidth*m_iImgHeight);
			memcpy(m_Img.data, pImageBuffer, m_iImgWidth*m_iImgHeight);
			//SavePic(g_config.GetSavePic());
			//用缓存来保存图片
			//if (g_config.GetSavePic())
			//{
			//	if (m_mSavePic.size() <= g_config.GetMaxPic())
			//	{
			//		m_mSavePic[m_PicName] = m_Img;
			//	}
			//	else
			//	{
			//		m_bStartSave = true;
			//		g_config.SetSavePic(false);
			//	}
			//}
			//cv::resize(m_Img, m_dstImg, cv::Size(), 0.5, 0.5);
			//ImgProcess(m_Img);
			if (m_bTransForm)
			{
				GetSaveImgPath();
				ImgTransForm(m_PicName, m_Img, m_Result, cv::Size(m_iImgWidth, m_iImgHeight));
			}
			
			t_end = clock();
			iRate++;
			if (t_end - t_beg >= 1000)
			{
				SetFrameRate(iRate);
				iRate = 0;
				t_beg = t_end;
			}
			if (m_iFrameRate > 25)
			{
				m_iPos++;
				if (0 == m_iPos % (m_iFrameRate/25) && g_config.show_pic())
				{
					Add_imgGrabList(m_Img);
					m_iPos = 0;
				}
			}
			else
				Add_imgGrabList(m_Img);
		}
		else
		{
			std::cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << std::endl;
		}
	}
	m_camera.StopGrabbing();
	Add_list("采集线程退出！");
}

bool CBaslerUSB::StopGrab()
{
	if (m_bConnect && m_bGrab)
	{
		m_bGrab = false;
		m_ThreadGrab.join();
		m_ThreadSavePic.join();
		return true;
	}
	else
		return false;
}

void CBaslerUSB::GetSaveImgPath()
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
		m_PicName.append(".bmp");
		//if (ifsave)
		//CImagePersistence::Save(ImageFileFormat_Jpeg, m_PicName.c_str(), ptrGrabResult);
	}
}

void CBaslerUSB::SavePic_Map()
{
	while (m_bGrab)
	{
		if (m_bStartSave)
		{
			Add_list("图片开始保存！");
			for (auto itr = m_mSavePic.begin(); itr != m_mSavePic.end(); itr++)
			{
				cv::imwrite(itr->first.c_str(), itr->second);
			}
			m_mSavePic.clear();
			ASSERT(m_mSavePic.size() == 0);
			m_bStartSave = false;
			Add_list("图片保存完毕！");
		}
		else
			Sleep(100);
	}
}


void CBaslerUSB::GetCameraConfig()
{
	m_iImgWidth = ptrGrabResult->GetWidth();
	m_iImgHeight = ptrGrabResult->GetHeight();
	m_fExposurTime = m_camera.ExposureTime.GetValue();
}

void CBaslerUSB::SetAOI()
{
	
	//设置相机图像读出模式
	//m_camera.SensorReadoutMode.SetValue(SensorReadoutMode_Fast);
	//设置曝光时间
	m_camera.ExposureTime.SetValue(5000.0);
	//设置采集图像大小
	m_camera.BinningHorizontalMode.SetValue(BinningHorizontalMode_Average);
	m_camera.BinningHorizontal.SetValue(1);
	m_camera.BinningVerticalMode.SetValue(BinningVerticalMode_Average);
	m_camera.BinningVertical.SetValue(1);
	
	//设置触发抓拍模式
	//m_camera.TriggerSelector.SetValue(TriggerSelector_FrameStart);
	//m_camera.TriggerMode.SetValue(TriggerMode_On);
	//m_camera.TriggerSource.SetValue(TriggerSource_Line1);
	//m_camera.TriggerActivation.SetValue(TriggerActivation_RisingEdge);
	//m_camera.AcquisitionStatusSelector.SetValue(AcquisitionStatusSelector_FrameTriggerWait);
}
void CBaslerUSB::SetFrameRate(int &rate)
{
	m_iFrameRate = rate;
	m_camera.AcquisitionFrameRate.SetValue(rate);
}
int CBaslerUSB::GetFrameRate()
{
	return m_iFrameRate;
}
void CBaslerUSB::SetExposureTime(double &time)
{
	m_fExposurTime = time;
	m_camera.ExposureTime.SetValue(m_fExposurTime);
}
double CBaslerUSB::GetExposureTime()
{
	return m_camera.ExposureTime.GetValue();
}

void CBaslerUSB::ImgProcess(cv::Mat img)
{
	//cv::resize(img, m_dstImg, cv::Size(), 0.5, 0.5);
	//二值化
	cv::threshold(img, m_GrayImg, 90, 255, cv::THRESH_BINARY);

	//腐蚀、膨胀
	//cv::dilate(m_GrayImg, m_GrayImg, cv::Mat(), cv::Point(), 5);
	//cv::erode(m_GrayImg, m_GrayImg, cv::Mat(), cv::Point(), 5);

	//cv::erode(m_GrayImg, m_GrayImg, cv::Mat(), cv::Point(), 3);
	//cv::dilate(m_GrayImg, m_GrayImg, cv::Mat(), cv::Point(), 3);

	//find contour
	findContours(m_GrayImg, vContours, vHierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	cv::Mat mImgDraw(img.rows, img.cols, CV_8UC3, cv::Scalar(0));
	cv::drawContours(mImgDraw, vContours, -1, cv::Scalar(255), CV_FILLED);


	int nContNums = (int)vContours.size();
	//排除无用的
	std::vector<std::vector<cv::Point>> vContAft;
	vContAft.reserve(nContNums);

	std::vector<cv::Rect> vContRectAft;
	vContRectAft.reserve(nContNums);

	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 2, 8);
	char tmp[128] = { 0 };
	for (int i = 0; i < nContNums; i++)
	{
		cv::Rect contRct = boundingRect(vContours[i]);

		//sprintf_s(tmp, "%d:x = %d,y = %d,w = %d,h = %d", i + 1, contRct.x, contRct.y, contRct.width, contRct.height);
		//cv::putText(mImgDraw, tmp, cv::Point(contRct.x, contRct.y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
		//宽高比
		double dWhRt = contRct.width * 1.0 / contRct.height;
		double dHwRt = contRct.height * 1.0 / contRct.width;

		if (((dWhRt > LSWHRTL && dWhRt < LSWHRTH) || (dHwRt > LSWHRTL && dHwRt < LSWHRTH))
			/*&& contRct.width > LSWL && contRct.height < LSWH && contRct.height > LSHL && contRct.height < LSHH*/)
		{
			vContAft.push_back(vContours[i]);
			vContRectAft.push_back(contRct);
		}
	}
	if (vContRectAft.size() > 0)
	{
		for (auto itr = vContRectAft.begin(); itr != vContRectAft.end(); itr++)
		{
			cv::rectangle(mImgDraw, *itr, cv::Scalar(0, 0, 255), 2);
			cv::circle(mImgDraw, cv::Point((itr->x + (itr->width / 2)), (itr->y + (itr->height / 2))), 2, cv::Scalar(0, 0, 255), 3);
			sprintf_s(tmp, "center:x = %d,y = %d", (itr->x + (itr->width / 2)), (itr->y + (itr->height / 2)));
			cv::putText(mImgDraw, tmp, cv::Point((itr->x + (itr->width / 2)), (itr->y + (itr->height / 2))), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0));
		}
	}

	if (1 == vContRectAft.size())
	{
		auto itr = vContRectAft.begin();
		m_Next.x = itr->x + itr->width / 2;
		m_Next.y = itr->y + itr->height / 2;
		OutPutResult();
	}
}
void CBaslerUSB::OutPutResult()
{
	//if (0 == m_FrameRate)
	//	return;
	if (0 == m_Last.x && 0 == m_Last.y)
	{
		m_Last.x = m_Next.x;
		m_Last.y = m_Next.y;
	}
	else
	{
		m_ASpeed_x = (((m_Last.x - m_Next.x) * EACH_PIX_LENGTH_X) * 0.001) * m_iFrameRate;
		m_ASpeed_y = (((m_Last.y - m_Next.y) * EACH_PIX_LENGTH_Y) * 0.001) * m_iFrameRate;
		if ((m_ASpeed_x >= DBL_EPSILON || m_ASpeed_x <= -DBL_EPSILON) && (m_ASpeed_y >= DBL_EPSILON || m_ASpeed_y <= -DBL_EPSILON))
		{
			char tmp[256] = { 0 };
			sprintf_s(tmp, "x=%.2f;y=%.2f\n", m_ASpeed_x, m_ASpeed_y);
			std::string str1 = m_PicName + " ";
			str1.append(tmp);
			//LOG(INFO) << str1.c_str();
			WriteFile(str1, "D:\\123.txt");
			Add_list(tmp);
		}
		m_Last.x = m_Next.x;
		m_Last.y = m_Next.y;
	}
}
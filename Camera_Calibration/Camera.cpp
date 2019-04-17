#include "stdafx.h"
#include "Camera.h"

CCamera::CCamera()
{
	m_iImgWidth = 0;
	m_iImgHeight = 0;
	m_iFrameRate = 0;
	m_fExposurTime = 0.0;
	m_bShowPic = true;
	m_bConnect = false;
	m_bGrab = false;
	m_bTransForm = false;
	m_bStartSaveTransFormImg = false;
	m_PicSavePath = "";
	m_PicName = "";
}

CCamera::~CCamera()
{
	m_mtxGrabImg.lock();
	if (m_imgGrablist.size() > 0)
	{
		m_imgGrablist.clear();
	}
	m_mtxGrabImg.unlock();
	m_mtxResultImg.lock();
	if (m_imgResultlist.size() > 0)
	{
		m_imgResultlist.clear();
	}
	m_mtxResultImg.unlock();
	m_mtxlist.lock();
	if (m_list.size() > 0)
	{
		m_list.clear();
	}
	m_mtxlist.unlock();
	std::cout << "delete CCamera" << std::endl;
}

bool CCamera::ConnectCamera()
{
	std::cout << "Connect camera failed" << std::endl;
	return false;
}
bool CCamera::DisConnectCamera()
{
	std::cout << "Disconnect camera failed" << std::endl;
	return false;
}
bool CCamera::StartGrab()
{
	std::cout << "Start grab camera failed" << std::endl;
	return false;
}
bool CCamera::StopGrab()
{
	std::cout << "Stop grab camera failed" << std::endl;
	return false;
}

bool CCamera::Add_list(const std::string &str)
{
	if (str.length() > 0)
	{
		m_mtxlist.lock();
		m_list.push_back(str);
		m_mtxlist.unlock();
		return true;
	}
	else
		return false;	
}
void CCamera::Get_list(std::string &str)
{
	str = "";
	m_mtxlist.lock();
	if (m_list.size() > 0)
	{
		auto itr = m_list.begin();
		str = *itr;
		m_list.erase(itr);
		m_mtxlist.unlock();
	}
	else
		m_mtxlist.unlock();
}

bool CCamera::Add_imgGrabList(const cv::Mat &img)
{
	m_mtxGrabImg.lock();
	if (!img.empty())
	{
		m_imgGrablist.push_back(img);
		m_mtxGrabImg.unlock();
		return true;
	}
	else
	{
		m_mtxGrabImg.unlock();
		return false;
	}
}

void CCamera::Get_imgGrabList(cv::Mat &img)
{
	char tmp[MAX_PATH] = { 0 };
	m_mtxGrabImg.lock();
	if (m_imgGrablist.size() > 0)
	{
		//sprintf_s(tmp, "%d", m_imgGrablist.size());
		//Add_list(tmp);
		//auto itr = m_imgGrablist.begin();
		//(*itr).copyTo(img);
		m_imgGrablist.begin()->copyTo(img);
		m_imgGrablist.pop_front();
		m_mtxGrabImg.unlock();
	}
	else
		m_mtxGrabImg.unlock();
}

bool CCamera::Add_imgResultList(const cv::Mat &img)
{
	m_mtxResultImg.lock();
	if (!img.empty())
	{
		m_imgResultlist.push_back(img);
		m_mtxResultImg.unlock();
		return true;
	}
	else
	{
		m_mtxResultImg.unlock();
		return false;
	}
}
void CCamera::Get_imgResultList(cv::Mat &img)
{
	m_mtxResultImg.lock();
	if (m_imgResultlist.size() > 0)
	{
		auto itr = m_imgResultlist.begin();
		(*itr).copyTo(img);
		m_imgResultlist.erase(itr);
		m_mtxResultImg.unlock();
	}
	else
		m_mtxResultImg.unlock();
}
void CCamera::SetPicSavePath(const std::string &path)
{
	if (path.length() <= 0)
		return;
	std::string str;
	size_t iPos = path.rfind('.');
	str = path.substr(iPos + 1, path.length());
	if (0 == str.compare("jpg") || 0 == str.compare("bmp") || 0 == str.compare("png"))
	{
		m_PicSavePath = path;
	}
	else
	{
		m_PicSavePath = path + "\\";
	}
}
std::string CCamera::GetPicSavePath()
{
	std::string path = m_PicSavePath;
	path.append(m_PicName);
	return path;
}

void CCamera::SetPicName()
{
	std::string str = "";
	GetTime(4, str);
	m_PicName.clear();
	m_PicName = str + ".jpg";
}

//图像处理
void CCamera::ImgProcess1(const cv::Mat &img)
{
	//二值化
	cv::threshold(img, m_GrayImg, 60, 255, cv::THRESH_BINARY);
	Add_imgGrabList(m_GrayImg);
	//腐蚀、膨胀
	//cv::dilate(m_GrayImg, m_GrayImg, cv::Mat(), cv::Point(), 5);
	//cv::erode(m_GrayImg, m_GrayImg, cv::Mat(), cv::Point(), 5);
	//cv::erode(m_GrayImg, m_GrayImg, cv::Mat(), cv::Point(), 3);
	//cv::dilate(m_GrayImg, m_GrayImg, cv::Mat(), cv::Point(), 3);
	//find contour
	//std::vector<std::vector<cv::Point>> vContours;
	//std::vector<cv::Vec4i> vHierarchy;
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
		cv::rectangle(mImgDraw, contRct, cv::Scalar(0, 0, 255), 2);
		//sprintf_s(tmp, "%d:x = %d,y = %d,w = %d,h = %d", i + 1, contRct.x, contRct.y, contRct.width, contRct.height);
		//cv::putText(mImgDraw, tmp, cv::Point(contRct.x, contRct.y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
		//宽高比
		//double dWhRt = contRct.width * 1.0 / contRct.height;
		//double dHwRt = contRct.height * 1.0 / contRct.width;
		//if (((dWhRt > LSWHRTL && dWhRt < LSWHRTH)
		//	|| (dHwRt > LSWHRTL && dHwRt < LSWHRTH))
		//	&& contRct.width > LSWL && contRct.height < LSWH
		//	&& contRct.height > LSHL && contRct.height < LSHH)
		//{
		//	vContAft.push_back(vContours[i]);
		//	vContRectAft.push_back(contRct);
		//}
	}
	Add_imgGrabList(mImgDraw);
}

void CCamera::ImgProcess2(const std::string path)
{
	int iNum = 0;
	char tmp[MAX_PATH] = { 0 };
	cv::Mat imageInput;
	if (path.length() > 0)
		imageInput = cv::imread(path);
	if (imageInput.empty())
		return;

	cv::Size board_size;
	if (m_iCornerPointX > 0 && m_iCornerPointY > 0)
		board_size = cv::Size(m_iCornerPointX, m_iCornerPointY);    /* 标定板上每行、列的角点数 */

	/* 提取角点 */
	/*
	findChessboardCorners(imageInput, board_size, image_points_buf);
	if (image_points_buf.size() > 0)
	{
		for (auto itr = image_points_buf.begin(); itr != image_points_buf.end(); itr++)
		{
			sprintf_s(tmp, "%d", iNum++);
			circle(imageInput, cv::Point((int)itr->x, (int)itr->y), 2, cv::Scalar(0, 0, 255));
			cv::putText(imageInput, tmp, cv::Point((int)itr->x, (int)itr->y), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 255), 2);
			LOG(INFO) << "dst: Point:" << iNum << "||| x = " << itr->x << ",y = " << itr->y;
			sprintf_s(tmp, "dst: Point %d:-> %.4f  %.4f\n", iNum, itr->x, itr->y);
			if (m_BiaodingPath.length() >0)
				WriteFile(tmp, m_BiaodingPath.c_str());
		}
		Add_imgGrabList(imageInput);
	}
	else
	{
		Add_list("can not find chessboard corners!");
		//std::cout << "can not find chessboard corners!\n"; //找不到角点  
		return;
	}
	*/

	if (0 == findChessboardCorners(imageInput, board_size, image_points_buf))
	{
		Add_list("can not find chessboard corners!");
		//std::cout << "can not find chessboard corners!\n"; //找不到角点  
		return;
	}
	else
	{
		cv::Mat view_gray;
		cvtColor(imageInput, view_gray, CV_RGB2GRAY);
		
		for (auto it = image_points_buf.begin(); it != image_points_buf.end(); it++)
		{
			//cv::Point pointInterest;
			//pointInterest.x = (int)it->x;
			//pointInterest.y = (int)it->y;
			//circle(imageInput, pointInterest, 2, cv::Scalar(0, 0, 255));
			sprintf_s(tmp, "src: Point %d :x = %.4f,y = %.4f\n", iNum++, it->x, it->y);
			//cv::putText(imageInput, tmp, cv::Point(it->x , it->y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0));
			LOG(INFO) << "src: Point:" << iNum << "x = " << it->x << ",y = " << it->y;
			if (m_BiaodingPath.length() >0)
				WriteFile(tmp, m_BiaodingPath.c_str());
		}
		size_t count = image_points_buf.size();
		//输出检验
		//std::cout << "-->count = " << count << std::endl;

		//亚像素精确化 
		//cv::find4QuadCornerSubpix(view_gray, image_points_buf, cv::Size(11, 11)); //对粗提取的角点进行精确化  
		cornerSubPix(view_gray, image_points_buf, cv::Size(5, 5), cv::Size(-1, -1), cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
		//image_points_seq.push_back(image_points_buf);  //保存亚像素角点  
		//在图像上显示角点位置
		drawChessboardCorners(view_gray, board_size, image_points_buf, false); //用于在图片中标记角点 

		cv::Mat mImgDraw;
		//cv::Mat mImgDraw(imageInput.rows, imageInput.cols,CV_8SC3);
		//cvtColor(imageInput, mImgDraw, CV_GRAY2RGB);
		cv::cvtColor(view_gray, mImgDraw, CV_GRAY2RGB);

		iNum = 1;
		for (auto itr = image_points_buf.begin(); itr != image_points_buf.end(); itr++)
		{
			sprintf_s(tmp, "%d", iNum++);
			cv::putText(mImgDraw, tmp, cv::Point((int)itr->x, (int)itr->y), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 3);
			LOG(INFO) << "dst: Point:" << iNum << "||| x = " << itr->x << ",y = " << itr->y;
			sprintf_s(tmp, "dst: Point %d:-> %.4f  %.4f\n", iNum, itr->x, itr->y);
			if (m_BiaodingPath.length() >0)
				WriteFile(tmp, m_BiaodingPath.c_str());
		}
		Add_imgGrabList(mImgDraw);
		cv::imwrite("D:\\123.bmp", mImgDraw);
	}

	//iNum = 0;
	//double BD_X, BD_Y,x1,x2,y1,y2 = 0;
	//for (auto itr = image_points_buf.begin(); itr != image_points_buf.end(); itr++)
	//{
	//	if (0 == iNum)
	//	{
	//		x1 = itr->x;
	//		y1 = itr->y;
	//		LOG(INFO) << "(1)iNum = " << iNum;
	//	}
	//	else if (0 == (iNum + 1) % POINT_NUM_X)
	//	{
	//		x2 = itr->x;
	//		BD_X = abs((POINT_NUM_X*STANDARD_LEN_X) / (x2 - x1));
	//		LOG(INFO) << "(2)iNum = " << iNum << "标定: " << "BD_X = " << BD_X;
	//	}
	//	else if (1 == (iNum + 1) % POINT_NUM_X && iNum >0)
	//	{
	//		x1 = itr->x;
	//		LOG(INFO) << "(3)iNum = " << iNum;
	//		if (iNum == POINT_NUM_X * (POINT_NUM_Y - 1))
	//		{
	//			y2 = itr->y;
	//			BD_Y = abs((POINT_NUM_Y*STANDARD_LEN_Y) / (y2 - y1));
	//			LOG(INFO) << "(4)iNum = " << iNum << "标定: " << "BD_Y = " << BD_Y;
	//		}
	//	}
	//	iNum++;
	//}
}


void CCamera::CalculationResult(const int beg, const int end)
{
	int iNum = 0;
	double BD_X, BD_Y, x1, x2, y1, y2;
	size_t iSize = image_points_buf.size();
	if (0 == iSize)
		return;
	for (auto itr = image_points_buf.begin(); itr != image_points_buf.end(); itr++)
	{
		if (iNum == beg)
		{
			x1 = itr->x;
			y1 = itr->y;
		}
		if (iNum == end)
		{
			x2 = itr->x;
			y2 = itr->y;
		}
		iNum++;
	}

	BD_X = abs((((beg % m_iCornerPointX) - (end % m_iCornerPointX)) * m_iX) / (x2 - x1));
	BD_Y = abs((((beg / m_iCornerPointX) - (end / m_iCornerPointX)) * m_iY) / (y2 - y1));
	char tmp[MAX_PATH] = { 0 };
	sprintf_s(tmp, "POINT1(%.2f.%.2f)|POINT2(%.2f.%.2f)", x1, y1, x2, y2);
	Add_list(tmp);
	memset(tmp, 0, MAX_PATH);
	sprintf_s(tmp, "->BD_X = %.2f, BD_Y = %.2f", BD_X, BD_Y);
	Add_list(tmp);
}


void CCamera::SetCornerPointNum(const int pos_x, const int pos_y)
{
	//if (pos_x == pos_y)
	//return;
	m_iCornerPointX = pos_x;
	m_iCornerPointY = pos_y;
}
void CCamera::Set2CornerDistance(const int dis_x, const int dis_y)
{
	if (0 <= dis_x && 0 <= dis_y)
	{
		m_iX = dis_x;
		m_iY = dis_y;
	}
}
void CCamera::SetBiaodingPath(const std::string &path)
{
	if (path.length() > 0)
	{
		m_BiaodingPath = path;
	}
}
std::string CCamera::GetBiaodingPath()
{
	return m_BiaodingPath;
}

void CCamera::GetTranspose(const std::string path, cv::Point2f *point_src, cv::Point2f *point_dst, cv::Point2f *move)
{
	cv::Mat src;
	cv::Mat dst;
	char tmp[MAX_PATH] = { 0 };
	if (path.length() > 0)
		src = cv::imread(path.c_str(), 1);
	if (src.empty())
		return;
	memset(tmp, 0, MAX_PATH);
	point_dst[0].x += move->x; point_dst[0].y += move->y;
	point_dst[1].x += move->x; point_dst[1].y += move->y;
	point_dst[2].x += move->x; point_dst[2].y += move->y;
	point_dst[3].x += move->x; point_dst[3].y += move->y;

	m_TransForm = cv::getPerspectiveTransform(point_dst, point_src);
	sprintf_s(tmp, "cols = %d,rows = %d,channel = %d,type = %d\n", m_TransForm.cols, m_TransForm.rows, m_TransForm.channels(), m_TransForm.type());
	//printf("cols = %d,rows = %d,channel = %d,type = %d\n", m_TransForm.cols, m_TransForm.rows, m_TransForm.channels(), m_TransForm.type());
	Add_list(tmp);
	cv::warpPerspective(src, dst, m_TransForm,cv::Size(src.cols,src.rows), cv::INTER_LINEAR | cv::WARP_INVERSE_MAP);
	Add_imgResultList(dst);
}

void CCamera::WriteTranspose(cv::Point2f *point_move)
{
	CvMat mat = m_TransForm;
	std::string str;
	char tmp[MAX_PATH] = { 0 };
	m_BiaodingPath = g_config.GetBiaoDingPath();
	WriteFile("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", m_BiaodingPath.c_str());
	memset(tmp, 0, MAX_PATH);
	//sprintf_s(tmp, "TransForm:\n");
	//WriteFile(tmp, m_BiaodingPath.c_str());
	//memset(tmp, 0, MAX_PATH);
	sprintf_s(tmp, "TransForm:\nmoveValueX = %lf,moveValueY = %lf\n[", point_move->x, point_move->y);
	WriteFile(tmp, m_BiaodingPath.c_str());
	printf("[");
	for (int row = 0; row < mat.rows; row++)
	{
		str.clear();
		double* ptr = (double*)(mat.data.ptr + row * mat.step);//第row行数据的起始指针
		for (int col = 0; col < mat.cols; col++)
		{
			memset(tmp, 0, MAX_PATH);
			//printf("%lf  ", *ptr);
			sprintf_s(tmp, "%.12lf  ", *ptr);
			ptr++;
			str.append(tmp);
		}
		if (row == mat.rows - 1)
		{
			//str.append("]");
			str.replace(str.length()-2,1,"]");
			printf("]");
		}
		Add_list(str);
		str.append("\n");
		WriteFile(str, m_BiaodingPath.c_str());
		std::cout << std::endl;
	}
	WriteFile("===================================================\n", m_BiaodingPath.c_str());
}

int iPos = 0;
std::string path_3D;
void CCamera::ImgTransForm(std::string path, cv::Mat src, cv::Mat dst, cv::Size size)
{
	if (src.empty() || path.length() < 0)
		return;

	cv::warpPerspective(src, dst, g_config.m_TransForm, size, cv::INTER_LINEAR | cv::WARP_INVERSE_MAP);
	if (dst.empty())
		Add_list("矩阵转换错误");
	//cv::threshold(dst, m_GrayImg, 90, 255, cv::THRESH_BINARY);
	//获取亮线的中线
	int iRow = dst.rows;
	int iCols = dst.cols;;
	std::vector<cv::Point> output;
	int iSum = 0;
	int iNum = 0;
	char tmp[16] = { 0 };
	std::string out;
	for (int i = 0; i < iCols; i++)
	{
		iSum = 0;
		iNum = 0;
		uchar *ptr = dst.ptr<uchar>(0,i);
		for (int j = 0; j < iRow; j++)
		{
			if (*ptr > 128)
			{
				*ptr = 255;
				iSum += j;
				iNum++;
			}
			else
				*ptr = 0;
			ptr += iCols;
		}
		if (iSum > 0)
		{
			output.push_back(cv::Point(i, (int)(iSum / iNum)));
			sprintf_s(tmp, "%d ", (int)(iSum / iNum));
			out.append(tmp);
		}
		else
		{
			out.append("0 ");
		}
	}
	out = out.substr(0, out.length() - 1);
	out.append("\n");
	if (iPos < PIC_HEIGHT_3D && iPos > 0)
	{
		WriteFile(out, path_3D);
		iPos++;
	}
	else
	{
		iPos = 0;
		std::string path;
		GetTime(4, path);
		path_3D = g_config.Get3DPicPath() + "\\" + path + ".dat";
		WriteFile(out, path_3D);
		iPos++;
	}
	/*
	//行、列遍历
	for (int i = 0; i < dst.rows; i++)
	{
		uchar *ptr = dst.ptr<uchar>(i);
		for (int j = 0; j < dst.cols; j++)
		{
			if (*ptr > 128)
			{
				*ptr = 255;
			}
			else
				*ptr = 0;
			ptr++;
		}
	}*/
	Add_imgResultList(dst);
	if (m_bStartSaveTransFormImg)
	{
		//cv::imwrite("D:\\123.jpg", src);
		cv::imwrite(path.c_str(), dst);
	}
	//用缓存来保存图片
	//if (m_bStartSaveTransFormImg)
	//{
	//	if (m_mSaveTransForm.size() <= g_config.GetMaxPic())
	//	{
	//		m_mSaveTransForm[path] = dst;
	//	}
	//	else
	//	{
	//		m_bStartSaveTransFormImg = true;
	//	}
	//}
}
void CCamera::SavePicMap()
{
	while (m_bGrab)
	{
		if (m_bStartSaveTransFormImg)
		{
			Add_list("图片开始保存！");
			for (auto itr = m_mSaveTransForm.begin(); itr != m_mSaveTransForm.end(); itr++)
			{
				cv::imwrite(itr->first.c_str(), itr->second);
			}
			m_mSaveTransForm.clear();
			ASSERT(m_mSaveTransForm.size() == 0);
			m_bStartSaveTransFormImg = false;
			Add_list("图片保存完毕！");
		}
		else
			Sleep(100);
	}
}

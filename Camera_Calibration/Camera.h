#pragma once

#include <string>
#include <iostream>
#include <mutex>
#include <list>
#include <map>
#include "BasicFun.h"
#include "opencv2\opencv.hpp"
#ifdef _DEBUG
#pragma comment(lib,"opencv_core2410d.lib")
#pragma comment(lib,"opencv_highgui2410d.lib")
#pragma comment(lib,"opencv_imgproc2410d.lib")
#pragma comment(lib,"opencv_calib3d2410d.lib")
#else
#pragma comment(lib,"opencv_core2410.lib")
#pragma comment(lib,"opencv_highgui2410.lib")
#pragma comment(lib,"opencv_imgproc2410.lib")
#pragma comment(lib,"opencv_calib3d2410.lib")
#endif // DEBUG

#define LSWHRTL 0.5
#define LSWHRTH  1.0
#define LSWL 2
#define LSWH 5
#define LSHL 2
#define LSHH 5

#define EACH_PIX_LENGTH_X 10   //暂定单位为：毫米
#define EACH_PIX_LENGTH_Y 10   //暂定单位为：毫米

#define PIC_HEIGHT_3D 480		//3D图像数据的高度

class CCamera
{
private:
	std::list<std::string> m_list;
	std::mutex m_mtxlist;
	std::list<cv::Mat>		m_imgGrablist;		//用于界面显示图片
	std::list<cv::Mat>		m_imgResultlist;	//用于界面显示图片
	std::mutex m_mtxGrabImg;
	std::mutex m_mtxResultImg;
	//检测相关数据
	int m_iCornerPointX;			//横轴角点数
	int m_iCornerPointY;			//纵轴角点数
	int m_iX;						//横轴方向两个角点间的距离
	int m_iY;						//纵轴方向两个角点间的距离
	cv::Mat m_dstImg;
	std::string m_BiaodingPath;		//标定文件存储路径
	std::map<std::string, cv::Mat> m_mSaveTransForm;		//开辟缓存，进行图片存储
public:
	NowDate m_Time;
	int m_iImgWidth;
	int m_iImgHeight;
	int m_iFrameRate = 0;			//帧率
	double m_fExposurTime = 0;		//曝光时间
	bool m_bShowPic;				//是否显示图片
	bool m_bSavePic;				//是否保存图片
	bool m_bConnect;				//是否连接相机
	bool m_bGrab;					//是否拍摄
	bool m_bTransForm;				//是否开始检测深度信息
	bool m_bStartSaveTransFormImg;	//开始保存矩阵变换后的图像
	cv::Mat m_Img;					//相机采集到的原始图像
	cv::Mat m_Result;				//通过矩阵变换后的图像
	cv::Mat m_TransForm;			//变换矩阵
	cv::Mat m_GrayImg;				//灰度图像
	std::string m_PicName;			//图片名称
	std::string m_PicSavePath;		//图片保存路径
	std::string m_CurrentPicPath;

	std::vector<std::vector<cv::Point>> vContours;
	std::vector<cv::Vec4i> vHierarchy;
	std::vector<cv::Point2f> image_points_buf;  /* 缓存每幅图像上检测到的角点 */
	std::vector<std::vector<cv::Point2f>> image_points_seq; /* 保存检测到的所有角点 */

private:
	void SaveTransFormImg();
	void SavePicMap();
public:
	CCamera();
	~CCamera();
	virtual bool ConnectCamera();
	virtual bool DisConnectCamera();
	virtual bool StartGrab();
	virtual bool StopGrab();

	CCamera operator=(const CCamera &t) const;
	bool Add_list(const std::string &str);
	void Get_list(std::string &str);
	bool Add_imgGrabList(const cv::Mat &img);
	void Get_imgGrabList(cv::Mat &img);
	bool Add_imgResultList(const cv::Mat &img);
	void Get_imgResultList(cv::Mat &img);
	void SetPicSavePath(const std::string &path);
	std::string GetPicSavePath();
	void SetPicName();
	void SetBiaodingPath(const std::string &path);
	std::string GetBiaodingPath();

	//图像处理
	void ImgProcess1(const cv::Mat &img);
	void ImgProcess2(const std::string path);
	void CalculationResult(const int beg, const int end);
	void SetCornerPointNum(const int pos_x, const int pos_y);
	void Set2CornerDistance(const int dis_x, const int dis_y);

	//获取转置矩阵
	//path			输入图像路径
	//point_src		原始图像输入点
	//point_dst		目标图像输入点
	//move			目标图像x,y偏移值
	//transform		输出 3x3矩阵
	void GetTranspose(const std::string path, cv::Point2f *point_src, cv::Point2f *point_dst, cv::Point2f *move);
	void ImgTransForm(std::string path, cv::Mat src, cv::Mat dst, cv::Size size);
	void WriteTranspose(cv::Point2f *point_move);

};


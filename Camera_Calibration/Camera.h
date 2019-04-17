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

#define EACH_PIX_LENGTH_X 10   //�ݶ���λΪ������
#define EACH_PIX_LENGTH_Y 10   //�ݶ���λΪ������

#define PIC_HEIGHT_3D 480		//3Dͼ�����ݵĸ߶�

class CCamera
{
private:
	std::list<std::string> m_list;
	std::mutex m_mtxlist;
	std::list<cv::Mat>		m_imgGrablist;		//���ڽ�����ʾͼƬ
	std::list<cv::Mat>		m_imgResultlist;	//���ڽ�����ʾͼƬ
	std::mutex m_mtxGrabImg;
	std::mutex m_mtxResultImg;
	//����������
	int m_iCornerPointX;			//����ǵ���
	int m_iCornerPointY;			//����ǵ���
	int m_iX;						//���᷽�������ǵ��ľ���
	int m_iY;						//���᷽�������ǵ��ľ���
	cv::Mat m_dstImg;
	std::string m_BiaodingPath;		//�궨�ļ��洢·��
	std::map<std::string, cv::Mat> m_mSaveTransForm;		//���ٻ��棬����ͼƬ�洢
public:
	NowDate m_Time;
	int m_iImgWidth;
	int m_iImgHeight;
	int m_iFrameRate = 0;			//֡��
	double m_fExposurTime = 0;		//�ع�ʱ��
	bool m_bShowPic;				//�Ƿ���ʾͼƬ
	bool m_bSavePic;				//�Ƿ񱣴�ͼƬ
	bool m_bConnect;				//�Ƿ��������
	bool m_bGrab;					//�Ƿ�����
	bool m_bTransForm;				//�Ƿ�ʼ��������Ϣ
	bool m_bStartSaveTransFormImg;	//��ʼ�������任���ͼ��
	cv::Mat m_Img;					//����ɼ�����ԭʼͼ��
	cv::Mat m_Result;				//ͨ������任���ͼ��
	cv::Mat m_TransForm;			//�任����
	cv::Mat m_GrayImg;				//�Ҷ�ͼ��
	std::string m_PicName;			//ͼƬ����
	std::string m_PicSavePath;		//ͼƬ����·��
	std::string m_CurrentPicPath;

	std::vector<std::vector<cv::Point>> vContours;
	std::vector<cv::Vec4i> vHierarchy;
	std::vector<cv::Point2f> image_points_buf;  /* ����ÿ��ͼ���ϼ�⵽�Ľǵ� */
	std::vector<std::vector<cv::Point2f>> image_points_seq; /* �����⵽�����нǵ� */

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

	//ͼ����
	void ImgProcess1(const cv::Mat &img);
	void ImgProcess2(const std::string path);
	void CalculationResult(const int beg, const int end);
	void SetCornerPointNum(const int pos_x, const int pos_y);
	void Set2CornerDistance(const int dis_x, const int dis_y);

	//��ȡת�þ���
	//path			����ͼ��·��
	//point_src		ԭʼͼ�������
	//point_dst		Ŀ��ͼ�������
	//move			Ŀ��ͼ��x,yƫ��ֵ
	//transform		��� 3x3����
	void GetTranspose(const std::string path, cv::Point2f *point_src, cv::Point2f *point_dst, cv::Point2f *move);
	void ImgTransForm(std::string path, cv::Mat src, cv::Mat dst, cv::Size size);
	void WriteTranspose(cv::Point2f *point_move);

};


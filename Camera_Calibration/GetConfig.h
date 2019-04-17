#pragma once
#define CONFIG_NAME		"config.ini"


class CGetConfig
{
public:
	CGetConfig();
	~CGetConfig();
private:
	bool m_bShowPic;				//�Ƿ����ͼ����ʾ
	bool m_bSavePic;				//�Ƿ񱣴�ͼƬ��1������  0�������棩
	std::string m_SavePicPath;		//����ͼƬ·��
	int m_MaxSave;					//ÿ�α���ͼƬ�������
	std::string m_LogPath;			//��־��¼·��
	std::string m_BiaodingPath;		//�궨�������·��
	float m_Pix2Size_x;				//�궨��X��ÿ�����ص����ĳߴ磨��λ��mm��
	float m_Pix2Size_y;				//�궨��Y��ÿ�����ص����ĳߴ磨��λ��mm��
	std::string m_3DPicPath;		//3D�����ļ��洢·��
public:
	std::string m_CurrentPath;		//�������ǰ����·��
	std::string m_ExeName;			//�����������
	cv::Point2f m_PointMove;		//
	cv::Mat m_TransForm;				//�궨����
public:
	bool ReadConfig();
	bool &show_pic();
	bool &GetSavePic();
	void SetSavePic(bool ifsave);
	std::string &GetSavePicPath();
	int &GetMaxPic();
	void SetLogPath(std::string path);
	std::string &GetLogPath();
	void SetBiaoDingPath(std::string path);
	std::string &GetBiaoDingPath();
	void SetPix2Size_X(float sz);
	float &GetPix2Size_X();
	void SetPix2Size_Y(float sz);
	float &GetPix2Size_Y();
	void Set3DPicPath(std::string path);
	std::string &Get3DPicPath();
};


#pragma once
#define CONFIG_NAME		"config.ini"


class CGetConfig
{
public:
	CGetConfig();
	~CGetConfig();
private:
	bool m_bShowPic;				//是否进行图像显示
	bool m_bSavePic;				//是否保存图片（1：保存  0：不保存）
	std::string m_SavePicPath;		//保存图片路径
	int m_MaxSave;					//每次保存图片最大数量
	std::string m_LogPath;			//日志记录路径
	std::string m_BiaodingPath;		//标定结果保存路径
	float m_Pix2Size_x;				//标定后X轴每个像素点代表的尺寸（单位：mm）
	float m_Pix2Size_y;				//标定后Y轴每个像素点代表的尺寸（单位：mm）
	std::string m_3DPicPath;		//3D数据文件存储路径
public:
	std::string m_CurrentPath;		//保存程序当前运行路径
	std::string m_ExeName;			//保存程序名称
	cv::Point2f m_PointMove;		//
	cv::Mat m_TransForm;				//标定矩阵
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


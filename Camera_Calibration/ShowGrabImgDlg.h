#pragma once
#include "afxwin.h"


// CShowGrabImgDlg 对话框

class CShowGrabImgDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowGrabImgDlg)

public:
	CShowGrabImgDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShowGrabImgDlg();

// 对话框数据
	enum { IDD = IDD_SHOW_GRAB_IMG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CStatic m_ShowImg;
	std::string m_ShowImgWindowName;

	int m_iWidthImg;
	int m_iHeightImg;
public:

private:
	virtual BOOL OnInitDialog();
public:
	void SetRect(CRect *rc);
	void ShowImg(cv::Mat *img);
	afx_msg void OnDestroy();
};

#pragma once
#include "afxwin.h"


// CShowGrabImgDlg �Ի���

class CShowGrabImgDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowGrabImgDlg)

public:
	CShowGrabImgDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CShowGrabImgDlg();

// �Ի�������
	enum { IDD = IDD_SHOW_GRAB_IMG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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

// ShowGrabImgDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Camera_Calibration.h"
#include "ShowGrabImgDlg.h"
#include "afxdialogex.h"


// CShowGrabImgDlg �Ի���

IMPLEMENT_DYNAMIC(CShowGrabImgDlg, CDialogEx)

CShowGrabImgDlg::CShowGrabImgDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CShowGrabImgDlg::IDD, pParent)
{

}

CShowGrabImgDlg::~CShowGrabImgDlg()
{
}

void CShowGrabImgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMG_STATIC, m_ShowImg);
}


BEGIN_MESSAGE_MAP(CShowGrabImgDlg, CDialogEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CShowGrabImgDlg ��Ϣ�������


BOOL CShowGrabImgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_ShowImgWindowName = "ShowGrab";

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

void CShowGrabImgDlg::SetRect(CRect *rc)
{
	if (rc->right > 0 && rc->bottom > 0)
	{
		//MoveWindow(rc->left, rc->top, rc->right, rc->bottom);
		m_ShowImg.MoveWindow(rc);
	}
	CRect rect;
	m_ShowImg.GetClientRect(rect);
	
	cv::namedWindow(m_ShowImgWindowName, cv::WINDOW_NORMAL);
	cv::resizeWindow(m_ShowImgWindowName, rect.Width(), rect.Height());
	HWND hWnd = (HWND)cvGetWindowHandle(m_ShowImgWindowName.c_str());
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_IMG_STATIC)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);
}

void CShowGrabImgDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	cv::destroyWindow(m_ShowImgWindowName);
	// TODO:  �ڴ˴������Ϣ����������
}

void CShowGrabImgDlg::ShowImg(cv::Mat *img)
{
	if (!img->empty())
	{
		m_iWidthImg = img->cols;
		m_iHeightImg = img->rows;
		cv::imshow(m_ShowImgWindowName, *img);
	}
}
// ShowResultImgDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Camera_Calibration.h"
#include "ShowResultImgDlg.h"
#include "afxdialogex.h"


// CShowResultImgDlg �Ի���

IMPLEMENT_DYNAMIC(CShowResultImgDlg, CDialogEx)

CShowResultImgDlg::CShowResultImgDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CShowResultImgDlg::IDD, pParent)
{

}

CShowResultImgDlg::~CShowResultImgDlg()
{
}

void CShowResultImgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMG_STATIC, m_ShowResultImg);
}


BEGIN_MESSAGE_MAP(CShowResultImgDlg, CDialogEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CShowResultImgDlg ��Ϣ�������
BOOL CShowResultImgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_ShowResultImgWindowName = "ShowResult";

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

void CShowResultImgDlg::SetRect(CRect *rc)
{
	if (rc->right > 0 && rc->bottom > 0)
	{
		//MoveWindow(rc->left, rc->top, rc->right, rc->bottom);
		m_ShowResultImg.MoveWindow(rc);
	}
	CRect rect;
	m_ShowResultImg.GetClientRect(rect);
	cv::namedWindow(m_ShowResultImgWindowName, cv::WINDOW_NORMAL);
	cv::resizeWindow(m_ShowResultImgWindowName, rect.Width(), rect.Height());
	HWND hWnd = (HWND)cvGetWindowHandle(m_ShowResultImgWindowName.c_str());
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_IMG_STATIC)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);
}

void CShowResultImgDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	cv::destroyWindow(m_ShowResultImgWindowName);
	// TODO:  �ڴ˴������Ϣ����������
}

void CShowResultImgDlg::ShowImg(cv::Mat *img)
{
	if (!img->empty())
	{
		m_iWidthResultImg = img->cols;
		m_iHeightResultImg = img->rows;
		cv::imshow(m_ShowResultImgWindowName, *img);
	}
}


// Camera_CalibrationDlg.h : 头文件
//

#pragma once

#include "afxwin.h"
#include "Camera.h"
#include "BaslerGigE.h"
#include "BaslerUSB.h"
#include "afxcmn.h"
#include "ShowGrabImgDlg.h"
#include "ShowResultImgDlg.h"

#define RECT_W	550
#define RECT_H	350

// CCamera_CalibrationDlg 对话框
class CCamera_CalibrationDlg : public CDialogEx
{
// 构造
public:
	CCamera_CalibrationDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CAMERA_CALIBRATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void InitControl();

private:
	CShowGrabImgDlg		*m_pShowGrabImgDlg;
	CShowResultImgDlg	*m_pShowResultImgDlg;

	CListBox m_List;
	CListCtrl m_PointList;
	CString  m_PicName;

	CBaslerGigE *m_pBaslerGigE;
	CBaslerUSB	*m_pBaslerUSB;
	CCamera		*m_pCamera;
	bool m_bConnect;
	bool m_bGrab;
	cv::Mat m_imgGrab;
	cv::Mat m_imgResult;

	cv::Point2f point_dst[4];
	cv::Point2f point_src[4];
	cv::Point2f point_move;
private:
	void InitPointListCtrl();
public:
	afx_msg void OnBnClickedConnectButton();
	afx_msg void OnBnClickedGrabButton();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnBnClickedGetExposureButton();
	afx_msg void OnBnClickedSetExposureButton();
	afx_msg void OnBnClickedPathButton();
	afx_msg void OnBnClickedSavePicButton();
	afx_msg void OnBnClickedCalibrationButton();
	afx_msg void OnBnClickedInputPicButton();
	afx_msg void OnBnClickedUsbConnectButton();
	afx_msg void OnBnClickedUsbGrabButton();
	afx_msg void OnBnClickedGetUsbExposureButton();
	afx_msg void OnBnClickedSetUsbExposureButton();
	afx_msg void OnBnClickedGetTransposeButton();
	afx_msg void OnBnClickedSaveTransposeButton();
	afx_msg void OnBnClickedShowTransformButton();
	afx_msg void OnBnClickedUsbSaveCheck();
	afx_msg void OnBnClickedClearListButton();
	afx_msg void OnBnClickedGetGigeFrameButton();
	afx_msg void OnBnClickedSetGigeFrameButton();
	afx_msg void OnBnClickedSetGigeAutoframeButton();
};

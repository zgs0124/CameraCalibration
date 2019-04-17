
// Camera_CalibrationDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Camera_Calibration.h"
#include "Camera_CalibrationDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCamera_CalibrationDlg �Ի���



CCamera_CalibrationDlg::CCamera_CalibrationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCamera_CalibrationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCamera_CalibrationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_POINT_LIST, m_PointList);
}

BEGIN_MESSAGE_MAP(CCamera_CalibrationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT_BUTTON, &CCamera_CalibrationDlg::OnBnClickedConnectButton)
	ON_BN_CLICKED(IDC_GRAB_BUTTON, &CCamera_CalibrationDlg::OnBnClickedGrabButton)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_GET_EXPOSURE_BUTTON, &CCamera_CalibrationDlg::OnBnClickedGetExposureButton)
	ON_BN_CLICKED(IDC_SET_EXPOSURE_BUTTON, &CCamera_CalibrationDlg::OnBnClickedSetExposureButton)
	ON_BN_CLICKED(IDC_PATH_BUTTON, &CCamera_CalibrationDlg::OnBnClickedPathButton)
	ON_BN_CLICKED(IDC_SAVE_PIC_BUTTON, &CCamera_CalibrationDlg::OnBnClickedSavePicButton)
	ON_BN_CLICKED(IDC_CALIBRATION_BUTTON, &CCamera_CalibrationDlg::OnBnClickedCalibrationButton)
	ON_BN_CLICKED(IDC_INPUT_PIC_BUTTON, &CCamera_CalibrationDlg::OnBnClickedInputPicButton)
	ON_BN_CLICKED(IDC_USB_CONNECT_BUTTON, &CCamera_CalibrationDlg::OnBnClickedUsbConnectButton)
	ON_BN_CLICKED(IDC_USB_GRAB_BUTTON, &CCamera_CalibrationDlg::OnBnClickedUsbGrabButton)
	ON_BN_CLICKED(IDC_GET_USB_EXPOSURE_BUTTON, &CCamera_CalibrationDlg::OnBnClickedGetUsbExposureButton)
	ON_BN_CLICKED(IDC_SET_USB_EXPOSURE_BUTTON, &CCamera_CalibrationDlg::OnBnClickedSetUsbExposureButton)
	ON_BN_CLICKED(IDC_GET_TRANSPOSE_BUTTON, &CCamera_CalibrationDlg::OnBnClickedGetTransposeButton)
	ON_BN_CLICKED(IDC_SAVE_TRANSPOSE_BUTTON, &CCamera_CalibrationDlg::OnBnClickedSaveTransposeButton)
	ON_BN_CLICKED(IDC_SHOW_TRANSFORM_BUTTON, &CCamera_CalibrationDlg::OnBnClickedShowTransformButton)
	ON_BN_CLICKED(IDC_USB_SAVE_CHECK, &CCamera_CalibrationDlg::OnBnClickedUsbSaveCheck)
	ON_BN_CLICKED(IDC_CLEAR_LIST_BUTTON, &CCamera_CalibrationDlg::OnBnClickedClearListButton)
	ON_BN_CLICKED(IDC_GET_GIGE_FRAME_BUTTON, &CCamera_CalibrationDlg::OnBnClickedGetGigeFrameButton)
	ON_BN_CLICKED(IDC_SET_GIGE_FRAME_BUTTON, &CCamera_CalibrationDlg::OnBnClickedSetGigeFrameButton)
	ON_BN_CLICKED(IDC_SET_GIGE_AUTOFRAME_BUTTON, &CCamera_CalibrationDlg::OnBnClickedSetGigeAutoframeButton)
END_MESSAGE_MAP()


// CCamera_CalibrationDlg ��Ϣ�������

BOOL CCamera_CalibrationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	m_pBaslerGigE = nullptr;
	m_pBaslerUSB = nullptr;
	m_pCamera = nullptr;
	m_pShowGrabImgDlg = nullptr;
	m_pShowResultImgDlg = nullptr;
	m_bConnect = false;
	m_bGrab = false;

	g_config.ReadConfig();
	GLOG_Init();
	InitControl();

	CRect rc_src(0, 0, RECT_W, RECT_H);
	CRect rc_dst(0, 0, 1200, 750);
	CRect rc;
	GetClientRect(&rc);//��ȡ�Ի���ͻ������С
	MoveWindow(rc_dst);
	//ClientToScreen(&rc);//ת��ΪӫĻ����
	CRect rc_1(rc.left + 2, rc.top + 2, rc.left + RECT_W, rc.top + RECT_H);
	CRect rc_2(rc.left + 2, rc.top + 358, rc.left + RECT_W, rc.top + RECT_H);
	m_pShowGrabImgDlg = new CShowGrabImgDlg;
	m_pShowGrabImgDlg->Create(IDD_SHOW_GRAB_IMG_DIALOG);
	m_pShowGrabImgDlg->SetWindowPos(GetOwner(), rc_1.left, rc_1.top, rc_1.right, rc_1.bottom, SWP_SHOWWINDOW);
	m_pShowGrabImgDlg->SetRect(&rc_src);

	m_pShowResultImgDlg = new CShowResultImgDlg;
	m_pShowResultImgDlg->Create(IDD_SHOW_RESULT_IMG_DIALOG);
	m_pShowResultImgDlg->SetWindowPos(GetOwner(), rc_2.left, rc_2.top, rc_2.right, rc_2.bottom, SWP_SHOWWINDOW);
	m_pShowResultImgDlg->SetRect(&rc_src);

	SetTimer(1, 10, nullptr);
	SetTimer(2, 10, nullptr);
	//SetTimer(3, 100, nullptr);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CCamera_CalibrationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCamera_CalibrationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CRect rect;
		CPaintDC dc(this);
		GetClientRect(rect);
		dc.FillSolidRect(rect, RGB(189, 250, 200));
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CCamera_CalibrationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCamera_CalibrationDlg::InitControl()
{
	GetDlgItem(IDC_CAMERA_IPADDRESS)->SetWindowText(_T("192.168.1.12"));
	//ListCtrl ��������
	m_PointList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES);
	//m_PointList.InsertColumn(0, _T(""), LVCFMT_LEFT, 0);
	m_PointList.InsertColumn(0, _T("�����"), LVCFMT_CENTER, 70);
	m_PointList.InsertColumn(1, _T("Xֵ"), LVCFMT_CENTER, 100);
	m_PointList.InsertColumn(2, _T("Yֵ"), LVCFMT_CENTER, 100);

	GetDlgItem(IDC_POINT_X_EDIT)->SetWindowText(_T("0"));
	GetDlgItem(IDC_POINT_Y_EDIT)->SetWindowText(_T("0"));
	GetDlgItem(IDC_POINT_W_EDIT)->SetWindowText(_T("450"));
	GetDlgItem(IDC_POINT_H_EDIT)->SetWindowText(_T("600"));
	GetDlgItem(IDC_MOVE_X_EDIT)->SetWindowText(_T("200"));
	GetDlgItem(IDC_MOVE_Y_EDIT)->SetWindowText(_T("200"));
}

void CCamera_CalibrationDlg::OnBnClickedConnectButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString strIp, strBuntton;
	std::string str,ip;
	GetDlgItem(IDC_CAMERA_IPADDRESS)->GetWindowText(strIp);
	GetDlgItem(IDC_CONNECT_BUTTON)->GetWindowText(strBuntton);
	str = CT2A(strBuntton.GetBuffer());
	ip = CT2A(strIp.GetBuffer());

	if (str.compare("����") == 0 && strIp.GetLength() > 0 && m_pBaslerGigE == nullptr)
	{
		m_pBaslerGigE = new CBaslerGigE(ip);
		if (m_pBaslerGigE->ConnectCamera())
		{
			GetDlgItem(IDC_CONNECT_BUTTON)->SetWindowText(_T("�Ͽ�"));
			((CButton *)GetDlgItem(IDC_GRAB_BUTTON))->EnableWindow(true);
		}
		else
		{
			delete m_pBaslerGigE;
			m_pBaslerGigE = nullptr;
			m_List.InsertString(0, _T("222�������ʧ�ܣ�"));
		}
	}
	else if (m_pBaslerGigE != nullptr && str.compare("�Ͽ�") == 0)
	{
		if (m_pBaslerGigE->m_bGrab)
			OnBnClickedGrabButton();
		m_pBaslerGigE->DisConnectCamera();
		delete m_pBaslerGigE;
		m_pBaslerGigE = nullptr;
		GetDlgItem(IDC_CONNECT_BUTTON)->SetWindowText(_T("����")); 
		((CButton *)GetDlgItem(IDC_GRAB_BUTTON))->EnableWindow(false);
		m_List.InsertString(0, _T("�Ͽ������"));
	}
	else
		m_List.InsertString(0, _T("333�������ʧ�ܣ�"));
}


void CCamera_CalibrationDlg::OnBnClickedGrabButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString strBuntton;
	std::string str, ip;
	GetDlgItem(IDC_GRAB_BUTTON)->GetWindowText(strBuntton);
	str = CT2A(strBuntton.GetBuffer());
	if (str.compare("����") == 0 && m_pBaslerGigE != nullptr)
	{
		if (m_pBaslerGigE->StartGrab())
			GetDlgItem(IDC_GRAB_BUTTON)->SetWindowText(_T("ֹͣ"));
		else
			return;
	}
	else if (str.compare("ֹͣ") == 0)
	{
		m_pBaslerGigE->StopGrab();
		GetDlgItem(IDC_GRAB_BUTTON)->SetWindowText(_T("����"));
	}
	else
		m_List.InsertString(0, _T("�������ʧ�ܣ�"));

}


void CCamera_CalibrationDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch (nIDEvent)
	{
	case 1:
	{
		if (m_pBaslerGigE != nullptr)
		{
			m_pBaslerGigE->Get_imgGrabList(m_imgGrab);
			m_pBaslerGigE->Get_imgResultList(m_imgResult);
			if (/*(!m_imgGrab.empty() || !m_imgResult.empty())&& */m_pShowGrabImgDlg != nullptr)
			{
				m_pShowGrabImgDlg->ShowImg(&m_imgGrab);
				m_pShowResultImgDlg->ShowImg(&m_imgResult);
			}
		}
		if (m_pBaslerUSB != nullptr)
		{
			m_pBaslerUSB->Get_imgGrabList(m_imgGrab);
			m_pBaslerUSB->Get_imgResultList(m_imgResult);
			if (/*(!m_imgGrab.empty() || !m_imgResult.empty())&& */m_pShowGrabImgDlg != nullptr)
			{
				m_pShowGrabImgDlg->ShowImg(&m_imgGrab);
				m_pShowResultImgDlg->ShowImg(&m_imgResult);
			}
		}
		break;
	}
	case 2:
	{
		std::string str = "";
		CString cstr;
		if (m_pBaslerGigE != nullptr)
		{
			m_pBaslerGigE->Get_list(str);
			if (str.length() > 0)
			{
				cstr = ToCString(str);
				m_List.InsertString(0, cstr);
			}
			cstr.Format(_T("%d"), m_pBaslerGigE->m_iImgWidth);
			GetDlgItem(IDC_GIGE_IMG_WIDTH_EDIT)->SetWindowText(cstr);
			cstr.Format(_T("%d"), m_pBaslerGigE->m_iImgHeight);
			GetDlgItem(IDC_GIGE_IMG_HEIGTH_EDIT)->SetWindowText(cstr);
		}
		else if (m_pBaslerUSB != nullptr)
		{
			m_pBaslerUSB->Get_list(str);
			if (str.length() > 0)
			{
				cstr = ToCString(str);
				m_List.InsertString(0, cstr);
			}
			cstr.Format(_T("%ld"), m_pBaslerUSB->GetFrameRate());
			GetDlgItem(IDC_USB_FRAMERATE_EDIT)->SetWindowText(cstr);
			cstr.Format(_T("%d"), m_pBaslerUSB->m_iImgWidth);
			GetDlgItem(IDC_USB_IMG_WIDTH_EDIT)->SetWindowText(cstr);
			cstr.Format(_T("%d"), m_pBaslerUSB->m_iImgHeight);
			GetDlgItem(IDC_USB_IMG_HEIGTH_EDIT)->SetWindowText(cstr);
		}
		break;
	}

	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CCamera_CalibrationDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	KillTimer(1);
	KillTimer(2);
	KillTimer(3);
	if (m_pBaslerGigE != nullptr)
	{
		delete m_pBaslerGigE;
		m_pBaslerGigE = nullptr;
	}
	if (m_pBaslerUSB != nullptr)
	{
		delete m_pBaslerUSB;
		m_pBaslerUSB = nullptr;
	}
	if (m_pCamera != nullptr)
	{
		delete m_pCamera;
		m_pCamera = nullptr;
	}

	if (m_pShowGrabImgDlg != nullptr)
	{
		delete m_pShowGrabImgDlg;
		m_pShowGrabImgDlg = nullptr;
	}
	if (m_pShowResultImgDlg != nullptr)
	{
		delete m_pShowResultImgDlg;
		m_pShowResultImgDlg = nullptr;
	}
	CDialogEx::OnClose();
}

//��ȡ�ع�ʱ��
void CCamera_CalibrationDlg::OnBnClickedGetExposureButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString str;	
	double time;
	if (m_pBaslerGigE != nullptr)
	{
		time = m_pBaslerGigE->GetExpouseTime();
		str.Format(_T("%.2f"), time);
		GetDlgItem(IDC_EXPOSURE_EDIT)->SetWindowText(str);
	}
}

//�����ع�ʱ��
void CCamera_CalibrationDlg::OnBnClickedSetExposureButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItem(IDC_EXPOSURE_EDIT)->GetWindowText(str);
	double time = _ttof(str);
	if (m_pBaslerGigE != nullptr)
		m_pBaslerGigE->SetExpouseTime(time);
}

//ͼƬ����·��
void CCamera_CalibrationDlg::OnBnClickedPathButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	std::string path;
	TCHAR szPath[_MAX_PATH];
	BROWSEINFO bi;
	bi.hwndOwner = GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.lpszTitle = _T("��ѡ�������·��");
	bi.pszDisplayName = szPath;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = NULL;

	LPITEMIDLIST pItemIDList = SHBrowseForFolder(&bi);

	if (pItemIDList)
	{
		if (SHGetPathFromIDList(pItemIDList, szPath))
		{
			path = TCHAR2String(szPath);
			//videoPath = inputImgPath;
		}

		//use IMalloc interface for avoiding memory leak
		IMalloc* pMalloc;
		if (SHGetMalloc(&pMalloc) != NOERROR)
		{
			TRACE(_T("Can't get the IMalloc interface\n"));
		}

		pMalloc->Free(pItemIDList);
		if (pMalloc)
			pMalloc->Release();
		GetDlgItem(IDC_PATH_EDIT)->SetWindowText(szPath);
		UpdateData(FALSE);
	}
}

//�����ͼ
void CCamera_CalibrationDlg::OnBnClickedSavePicButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString cstr;
	if (m_pBaslerGigE != nullptr)
	{
		GetDlgItem(IDC_PATH_EDIT)->GetWindowText(cstr);
		if (cstr.GetLength() > 0)
		{
			m_pBaslerGigE->SetPicSavePath(ToString(cstr));
			m_pBaslerGigE->m_bSavePic = true;
			OnBnClickedGrabButton();
			((CButton *)GetDlgItem(IDC_CALIBRATION_BUTTON))->EnableWindow(true);
			((CComboBox *)GetDlgItem(IDC_CORNERPOINT_X_COMBO))->SetCurSel(7);
			((CComboBox *)GetDlgItem(IDC_CORNERPOINT_Y_COMBO))->SetCurSel(5);
			((CEdit *)GetDlgItem(IDC_X_EDIT))->SetWindowText(_T("60"));
			((CEdit *)GetDlgItem(IDC_Y_EDIT))->SetWindowText(_T("60"));
		}
	}
	else
		m_List.InsertString(0, _T("���������"));
}

//�ǵ���
void CCamera_CalibrationDlg::OnBnClickedCalibrationButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString cstr;
	GetDlgItem(IDC_CORNERPOINT_X_COMBO)->GetWindowText(cstr);
	int iPos_X = (int)_ttof(cstr);
	GetDlgItem(IDC_CORNERPOINT_Y_COMBO)->GetWindowText(cstr);
	int iPos_Y = (int)_ttof(cstr);
	if (m_pBaslerGigE != nullptr)
	{
		m_pBaslerGigE->SetCornerPointNum(iPos_X, iPos_Y);
		//std::string path = m_pBaslerGigE->GetPicSavePath();
		m_pBaslerGigE->ImgProcess2(m_pBaslerGigE->GetPicSavePath());
		InitPointListCtrl();
		//m_List.InsertString(0, _T("�궨ͼƬ�ɹ���"));
	}
	else
		m_List.InsertString(0, _T("�궨ͼƬʧ�ܣ�"));
}

//������ͼƬ
void CCamera_CalibrationDlg::OnBnClickedInputPicButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFilters[] = _T("picture(*.jpg;*,bmp)\0*.jpg;*.bmp\0");
	CFileDialog OpenDlg(TRUE);
	OpenDlg.m_ofn.lpstrTitle = _T("Open File");
	OpenDlg.m_ofn.lpstrFilter = szFilters;

	if (IDOK == OpenDlg.DoModal())
	{
		m_PicName = OpenDlg.GetPathName(); //�ļ�����������FilePathName��
		if (m_pBaslerGigE == nullptr)
		{
			m_pBaslerGigE = new CBaslerGigE;
			m_pBaslerGigE->SetPicSavePath(ToString(m_PicName));
		}
		m_imgGrab = cv::imread(ToString(m_PicName), 1);
		if (m_pShowGrabImgDlg != nullptr)
			m_pShowGrabImgDlg->ShowImg(&m_imgGrab);
		else
			m_List.InsertString(0, _T("��ʼ����ʾ���ڴ���"));
		((CButton *)GetDlgItem(IDC_CALIBRATION_BUTTON))->EnableWindow(true);
		((CComboBox *)GetDlgItem(IDC_CORNERPOINT_X_COMBO))->SetCurSel(7);
		((CComboBox *)GetDlgItem(IDC_CORNERPOINT_Y_COMBO))->SetCurSel(5);
		((CEdit *)GetDlgItem(IDC_X_EDIT))->SetWindowText(_T("60"));
		((CEdit *)GetDlgItem(IDC_Y_EDIT))->SetWindowText(_T("60"));
	}
}


//USB�������
void CCamera_CalibrationDlg::OnBnClickedUsbConnectButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	if (!m_bConnect)
	{
		m_pBaslerUSB = new CBaslerUSB;
		if (!m_pBaslerUSB->ConnectCamera())
		{
			m_List.InsertString(0, _T("USB������Ӳ��ɹ�"));
			delete m_pBaslerUSB;
			m_pBaslerUSB = nullptr;
			return;
		}
		m_bConnect = true;
		GetDlgItem(IDC_USB_CONNECT_BUTTON)->SetWindowText(_T("�Ͽ�"));
		GetDlgItem(IDC_USB_GRAB_BUTTON)->EnableWindow(true);
		m_List.InsertString(0, _T("USB������ӳɹ�"));
	}
	else
	{
		m_pBaslerUSB->DisConnectCamera();
		delete m_pBaslerUSB;
		m_pBaslerUSB = nullptr;
		m_bConnect = false;
		GetDlgItem(IDC_USB_CONNECT_BUTTON)->SetWindowText(_T("����"));
		GetDlgItem(IDC_USB_GRAB_BUTTON)->EnableWindow(false);
		m_List.InsertString(0, _T("USB����Ͽ��ɹ�"));
	}
}

//USB�������
void CCamera_CalibrationDlg::OnBnClickedUsbGrabButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (m_pBaslerUSB != nullptr && !m_bGrab)
	{
		m_bGrab = true;
		m_pBaslerUSB->StartGrab();
		GetDlgItem(IDC_USB_GRAB_BUTTON)->SetWindowText(_T("ֹͣ"));
		m_List.InsertString(0, _T("USB�����ʼ�ɼ�"));
	}
	else
	{
		m_bGrab = false;
		m_pBaslerUSB->StopGrab();
		GetDlgItem(IDC_USB_GRAB_BUTTON)->SetWindowText(_T("����"));
		m_List.InsertString(0, _T("USB���ֹͣ�ɼ�"));
	}
}


void CCamera_CalibrationDlg::OnBnClickedGetUsbExposureButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString cstr;
	if (m_pBaslerUSB != nullptr)
	{
		double time = m_pBaslerUSB->GetExposureTime();
		cstr.Format(_T("%.2f"), time);
		GetDlgItem(IDC_USB_EXPOSURE_EDIT)->SetWindowText(cstr);
	}
}


void CCamera_CalibrationDlg::OnBnClickedSetUsbExposureButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString cstr;
	GetDlgItem(IDC_USB_EXPOSURE_EDIT)->GetWindowText(cstr);
	if (m_pBaslerUSB != nullptr)
	{
		double time = _ttof(cstr);
		m_pBaslerUSB->SetExposureTime(time);
	}
}

void CCamera_CalibrationDlg::InitPointListCtrl()
{
	int iNum = 0;
	CString cstr_num, cstr_x, cstr_y;
	if (m_pBaslerGigE != nullptr)
		m_pCamera = m_pBaslerGigE;
	else if (m_pBaslerUSB != nullptr)
		m_pCamera = m_pBaslerUSB;
	if (m_PointList.GetItemCount() > 0)
	{
		m_PointList.DeleteAllItems();
	}
	size_t count = m_pCamera->image_points_buf.size();
	for (auto itr = m_pCamera->image_points_buf.begin(); itr != m_pCamera->image_points_buf.end(); itr++)
	{
		cstr_num.Format(_T("Point%d"), iNum + 1);
		cstr_x.Format(_T("%lf"), itr->x);
		cstr_y.Format(_T("%lf"), itr->y);
		m_PointList.InsertItem(iNum, _T(""));
		m_PointList.SetItemText(iNum, 0, cstr_num);
		m_PointList.SetItemText(iNum, 1, cstr_x);
		m_PointList.SetItemText(iNum, 2, cstr_y);
		iNum++;
	}
	
	m_pCamera = nullptr;
}

//��ȡת�þ���
void CCamera_CalibrationDlg::OnBnClickedGetTransposeButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString cstr;
	double x, y, w, h, move_x, move_y;
	memset(point_dst, 0, sizeof(point_dst));
	memset(point_src, 0, sizeof(point_src));
	point_move.x = 0;
	point_move.y = 0;
	GetDlgItem(IDC_POINT_X_EDIT)->GetWindowText(cstr);
	x = _ttof(cstr);
	GetDlgItem(IDC_POINT_Y_EDIT)->GetWindowText(cstr);
	y = _ttof(cstr);
	GetDlgItem(IDC_POINT_W_EDIT)->GetWindowText(cstr);
	w = _ttof(cstr);
	GetDlgItem(IDC_POINT_H_EDIT)->GetWindowText(cstr);
	h = _ttof(cstr);
	GetDlgItem(IDC_MOVE_X_EDIT)->GetWindowText(cstr);
	move_x = _ttof(cstr);
	GetDlgItem(IDC_MOVE_Y_EDIT)->GetWindowText(cstr);
	move_y = _ttof(cstr);
	point_dst[0].x = (float)x;			point_dst[0].y = (float)y;
	point_dst[1].x = (float)(x + w);	point_dst[1].y = (float)y;
	point_dst[2].x = (float)x;			point_dst[2].y = (float)(y + h);
	point_dst[3].x = (float)(x + w);	point_dst[3].y = (float)(y + h);
	point_move.x = (float)move_x;
	point_move.y = (float)move_y;

	BOOL state = false;
	int iCheckNum = 0;
	for (int i = 0; i < m_PointList.GetItemCount(); i++)
	{
		state = m_PointList.GetCheck(i);
		if (state)
		{
			cstr = m_PointList.GetItemText(i, 1);
			point_src[iCheckNum].x = (float)_ttof(cstr);
			cstr = m_PointList.GetItemText(i, 2);
			point_src[iCheckNum].y = (float)_ttof(cstr);
			iCheckNum++;
		}
	}
	if (4 != iCheckNum)
	{
		m_List.InsertString(0, _T("��ѡ��4���"));
		return;
	}
	if (m_pBaslerGigE != nullptr)
		m_pCamera = m_pBaslerGigE;
	else if (m_pBaslerUSB != nullptr)
		m_pCamera = m_pBaslerUSB;
	m_pCamera->GetTranspose(m_pCamera->m_PicSavePath + m_pCamera->m_PicName, point_src, point_dst, &point_move);
}

void CCamera_CalibrationDlg::OnBnClickedSaveTransposeButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (m_pBaslerGigE != nullptr)
		m_pCamera = m_pBaslerGigE;
	else if (m_pBaslerUSB != nullptr)
		m_pCamera = m_pBaslerUSB;
	else
		return;
	m_pCamera->WriteTranspose(&point_move);
}

void CCamera_CalibrationDlg::OnBnClickedShowTransformButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (m_pBaslerGigE != nullptr)
		m_pCamera = m_pBaslerGigE;
	else if (m_pBaslerUSB != nullptr)
		m_pCamera = m_pBaslerUSB;
	else
		return;
	if (m_pCamera->m_bTransForm)
	{
		m_pCamera->m_bTransForm = false;
		m_List.InsertString(0, _T("ֹͣ���о�������"));
	}
	else
	{
		m_pCamera->m_bTransForm = true;
		m_List.InsertString(0, _T("��ʼ���о�������"));
	}
}


void CCamera_CalibrationDlg::OnBnClickedUsbSaveCheck()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int check = ((CButton *)(GetDlgItem(IDC_USB_SAVE_CHECK)))->GetCheck();
	if (check == 1)
		m_pBaslerUSB->m_bStartSaveTransFormImg = true;
	else
		m_pBaslerUSB->m_bStartSaveTransFormImg = false;
	
}


void CCamera_CalibrationDlg::OnBnClickedClearListButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	for (int i = 0; i < m_List.GetCount(); i++)
	{
		m_List.DeleteString(i);
	}
}


void CCamera_CalibrationDlg::OnBnClickedGetGigeFrameButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString cstr;
	if (m_pBaslerGigE != nullptr)
	{
		double frame = m_pBaslerGigE->GetFrameRate();
		cstr.Format(_T("%lf"), frame);
		GetDlgItem(IDC_GIGE_FRAMERATE_EDIT)->SetWindowTextW(cstr);
	}
}


void CCamera_CalibrationDlg::OnBnClickedSetGigeFrameButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString cstr;
	GetDlgItem(IDC_GIGE_FRAMERATE_EDIT)->GetWindowTextW(cstr);
	if (m_pBaslerGigE != nullptr)
	{
		double frame = _ttof(cstr);
		m_pBaslerGigE->SetFrameRate(frame, true);
	}
}


void CCamera_CalibrationDlg::OnBnClickedSetGigeAutoframeButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (m_pBaslerGigE != nullptr)
	{
		m_pBaslerGigE->SetFrameRate(0, false);
	}
}

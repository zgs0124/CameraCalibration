#pragma once


// CShowResultImgDlg �Ի���

class CShowResultImgDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowResultImgDlg)

public:
	CShowResultImgDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CShowResultImgDlg();

// �Ի�������
	enum { IDD = IDD_SHOW_RESULT_IMG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CStatic m_ShowResultImg;
	std::string m_ShowResultImgWindowName;

	int m_iWidthResultImg;
	int m_iHeightResultImg;
public:

private:
	virtual BOOL OnInitDialog();
public:
	void SetRect(CRect *rc);
	void ShowImg(cv::Mat *img);
	afx_msg void OnDestroy();
};

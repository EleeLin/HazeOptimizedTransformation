#pragma once


// CZoomDlg �Ի���

class CZoomDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CZoomDlg)

public:
	CZoomDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CZoomDlg();

// �Ի�������
	enum { IDD = IDD_ZOOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	double m_nZw;
	double m_nZh;
};

#pragma once


// CLineStDlg �Ի���

class CLineStDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLineStDlg)

public:
	CLineStDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLineStDlg();

// �Ի�������
	enum { IDD = IDD_LINEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	UINT m_nMin;
	UINT m_nMax;
	double m_fA;
	double m_fB;
};

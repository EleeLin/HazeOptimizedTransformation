#pragma once


// CRawDlg �Ի���

class CRawDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRawDlg)

public:
	CRawDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRawDlg();

// �Ի�������
	enum { IDD = IDD_RAWINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	UINT m_nWidth;
	UINT m_nHeight;
};

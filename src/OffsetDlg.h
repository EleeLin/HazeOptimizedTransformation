#pragma once


// COffsetDlg �Ի���

class COffsetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COffsetDlg)

public:
	COffsetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~COffsetDlg();

// �Ի�������
	enum { IDD = IDD_OFFSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_nX;
	int m_nY;
};

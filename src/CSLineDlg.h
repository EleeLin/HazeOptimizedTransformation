#pragma once


// CCSLineDlg �Ի���

class CCSLineDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCSLineDlg)

public:
	CCSLineDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCSLineDlg();

// �Ի�������
	enum { IDD = IDD_CSLINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};

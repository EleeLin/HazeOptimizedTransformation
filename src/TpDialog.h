#pragma once


// CTpDialog �Ի���

class CTpDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CTpDialog)

public:
	CTpDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTpDialog();

// �Ի�������
	enum { IDD = IDD_TP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	int m [25];    // ģ��༭��
	int m_Zoom;    // ���ű༭��
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};

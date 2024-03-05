#pragma once


// CTpDialog 对话框

class CTpDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CTpDialog)

public:
	CTpDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTpDialog();

// 对话框数据
	enum { IDD = IDD_TP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	int m [25];    // 模板编辑框
	int m_Zoom;    // 缩放编辑框
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};

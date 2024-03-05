#pragma once


// CLineStDlg 对话框

class CLineStDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLineStDlg)

public:
	CLineStDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLineStDlg();

// 对话框数据
	enum { IDD = IDD_LINEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	UINT m_nMin;
	UINT m_nMax;
	double m_fA;
	double m_fB;
};

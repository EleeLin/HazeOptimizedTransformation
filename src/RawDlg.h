#pragma once


// CRawDlg 对话框

class CRawDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRawDlg)

public:
	CRawDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRawDlg();

// 对话框数据
	enum { IDD = IDD_RAWINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	UINT m_nWidth;
	UINT m_nHeight;
};

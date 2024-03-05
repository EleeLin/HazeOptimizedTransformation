#pragma once


// CZoomDlg 对话框

class CZoomDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CZoomDlg)

public:
	CZoomDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CZoomDlg();

// 对话框数据
	enum { IDD = IDD_ZOOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_nZw;
	double m_nZh;
};

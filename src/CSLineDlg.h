#pragma once


// CCSLineDlg 对话框

class CCSLineDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCSLineDlg)

public:
	CCSLineDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCSLineDlg();

// 对话框数据
	enum { IDD = IDD_CSLINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};

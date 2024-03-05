#pragma once


// COffsetDlg 对话框

class COffsetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COffsetDlg)

public:
	COffsetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COffsetDlg();

// 对话框数据
	enum { IDD = IDD_OFFSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_nX;
	int m_nY;
};

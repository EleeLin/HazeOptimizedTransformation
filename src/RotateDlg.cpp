// RotateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IPro.h"
#include "RotateDlg.h"
#include "afxdialogex.h"


// CRotateDlg 对话框

IMPLEMENT_DYNAMIC(CRotateDlg, CDialogEx)

CRotateDlg::CRotateDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRotateDlg::IDD, pParent)
	, m_fAngle(0)
{

}

CRotateDlg::~CRotateDlg()
{
}

void CRotateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ANGLE, m_fAngle);
}


BEGIN_MESSAGE_MAP(CRotateDlg, CDialogEx)
END_MESSAGE_MAP()


// CRotateDlg 消息处理程序

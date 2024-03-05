// LineStDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IPro.h"
#include "LineStDlg.h"
#include "afxdialogex.h"


// CLineStDlg 对话框

IMPLEMENT_DYNAMIC(CLineStDlg, CDialogEx)

CLineStDlg::CLineStDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLineStDlg::IDD, pParent)
	, m_nMin(0)
	, m_nMax(0)
	, m_fA(0)
	, m_fB(0)
{

}

CLineStDlg::~CLineStDlg()
{
}

void CLineStDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MIN, m_nMin);
	DDV_MinMaxUInt(pDX, m_nMin, 0, 255);
	DDX_Text(pDX, IDC_MAX, m_nMax);
	DDV_MinMaxUInt(pDX, m_nMax, 0, 255);
	DDX_Text(pDX, IDC_A, m_fA);
	DDX_Text(pDX, IDC_B, m_fB);
}


BEGIN_MESSAGE_MAP(CLineStDlg, CDialogEx)
END_MESSAGE_MAP()


// CLineStDlg 消息处理程序

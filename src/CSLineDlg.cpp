// CSLineDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IPro.h"
#include "CSLineDlg.h"
#include "afxdialogex.h"


// CCSLineDlg 对话框

IMPLEMENT_DYNAMIC(CCSLineDlg, CDialogEx)

CCSLineDlg::CCSLineDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCSLineDlg::IDD, pParent)
{

}

CCSLineDlg::~CCSLineDlg()
{
}

void CCSLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCSLineDlg, CDialogEx)
END_MESSAGE_MAP()


// CCSLineDlg 消息处理程序

// ZoomDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "IPro.h"
#include "ZoomDlg.h"
#include "afxdialogex.h"


// CZoomDlg �Ի���

IMPLEMENT_DYNAMIC(CZoomDlg, CDialogEx)

CZoomDlg::CZoomDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CZoomDlg::IDD, pParent)
	, m_nZw(0)
	, m_nZh(0)
{

}

CZoomDlg::~CZoomDlg()
{
}

void CZoomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_H, m_nZh);
	DDX_Text(pDX, IDC_W, m_nZw);
}


BEGIN_MESSAGE_MAP(CZoomDlg, CDialogEx)
END_MESSAGE_MAP()


// CZoomDlg ��Ϣ�������

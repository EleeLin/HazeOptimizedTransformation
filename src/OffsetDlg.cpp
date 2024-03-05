// OffsetDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "IPro.h"
#include "OffsetDlg.h"
#include "afxdialogex.h"


// COffsetDlg �Ի���

IMPLEMENT_DYNAMIC(COffsetDlg, CDialogEx)

COffsetDlg::COffsetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COffsetDlg::IDD, pParent)
	, m_nX(0)
	, m_nY(0)
{
}

COffsetDlg::~COffsetDlg()
{
}

void COffsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_POSX, m_nX);
	DDX_Text(pDX, IDC_POSY, m_nY);
}


BEGIN_MESSAGE_MAP(COffsetDlg, CDialogEx)
END_MESSAGE_MAP()


// COffsetDlg ��Ϣ�������

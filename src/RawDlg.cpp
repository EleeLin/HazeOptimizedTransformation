// RawDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "IPro.h"
#include "RawDlg.h"
#include "afxdialogex.h"


// CRawDlg �Ի���

IMPLEMENT_DYNAMIC(CRawDlg, CDialogEx)

CRawDlg::CRawDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRawDlg::IDD, pParent)
	, m_nWidth(0)
	, m_nHeight(0)
{

}

CRawDlg::~CRawDlg()
{
}

void CRawDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITWIDTH, m_nWidth);
	DDX_Text(pDX, IDC_EDITHEIGHT, m_nHeight);
}


BEGIN_MESSAGE_MAP(CRawDlg, CDialogEx)
END_MESSAGE_MAP()


// CRawDlg ��Ϣ�������

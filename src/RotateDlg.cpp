// RotateDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "IPro.h"
#include "RotateDlg.h"
#include "afxdialogex.h"


// CRotateDlg �Ի���

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


// CRotateDlg ��Ϣ�������

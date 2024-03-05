// TpDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "IPro.h"
#include "TpDialog.h"
#include "afxdialogex.h"


// CTpDialog 对话框

IMPLEMENT_DYNAMIC(CTpDialog, CDialogEx)

CTpDialog::CTpDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTpDialog::IDD, pParent)
{
	for (int mat = 0; mat < 25; mat++)
	{
		m [mat] = 0;
	}

	m_Zoom = 1;
}

CTpDialog::~CTpDialog()
{
}

void CTpDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m [0]);
	DDX_Text(pDX, IDC_EDIT2, m [5]);
	DDX_Text(pDX, IDC_EDIT3, m [10]);
	DDX_Text(pDX, IDC_EDIT4, m [15]);
	DDX_Text(pDX, IDC_EDIT5, m [20]);
	DDX_Text(pDX, IDC_EDIT6, m [1]);
	DDX_Text(pDX, IDC_EDIT7, m [6]);
	DDX_Text(pDX, IDC_EDIT8, m [11]);
	DDX_Text(pDX, IDC_EDIT9, m [16]);
	DDX_Text(pDX, IDC_EDIT10, m [21]);
	DDX_Text(pDX, IDC_EDIT11, m [2]);
	DDX_Text(pDX, IDC_EDIT12, m [7]);
	DDX_Text(pDX, IDC_EDIT13, m [12]);
	DDX_Text(pDX, IDC_EDIT14, m [17]);
	DDX_Text(pDX, IDC_EDIT15, m [22]);
	DDX_Text(pDX, IDC_EDIT16, m [3]);
	DDX_Text(pDX, IDC_EDIT17, m [8]);
	DDX_Text(pDX, IDC_EDIT18, m [13]);
	DDX_Text(pDX, IDC_EDIT19, m [18]);
	DDX_Text(pDX, IDC_EDIT20, m [23]);
	DDX_Text(pDX, IDC_EDIT21, m [4]);
	DDX_Text(pDX, IDC_EDIT22, m [9]);
	DDX_Text(pDX, IDC_EDIT23, m [14]);
	DDX_Text(pDX, IDC_EDIT24, m [19]);
	DDX_Text(pDX, IDC_EDIT25, m [24]);
	DDX_Text(pDX, IDC_EDIT26, m_Zoom);
}


BEGIN_MESSAGE_MAP(CTpDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CTpDialog::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTpDialog::OnBnClickedButton2)
END_MESSAGE_MAP()


// CTpDialog 消息处理程序


void CTpDialog::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	m [7] = -1;
	m [11] = -1;
	m [12] = 5;
	m [13] = -1;
	m [17] = -1;
	UpdateData (FALSE);
}


void CTpDialog::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	m [6] = 1;
	m [7] = 1;
	m [8] = 1;
	m [11] = 1;
	m [12] = 1;
	m [13] = 1;
	m [16] = 1;
	m [17] = 1;
	m [18] = 1;
	m_Zoom = 9;
	UpdateData (FALSE);
}

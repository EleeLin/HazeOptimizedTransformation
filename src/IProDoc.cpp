
// IProDoc.cpp : CIProDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "IPro.h"
#endif

#include "IProDoc.h"
#include "Raw.h"
#include "BmpFile.h"
#include "RawDlg.h"
#include "LineStDlg.h"
#include "TpDialog.h"
#include <math.h>
#include "MainFrm.h"
#include "IProView.h"
#include "RotateDlg.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CIProDoc

IMPLEMENT_DYNCREATE(CIProDoc, CDocument)

BEGIN_MESSAGE_MAP(CIProDoc, CDocument)
	ON_COMMAND(ID_32776, &CIProDoc::OnLineStretch)
	ON_COMMAND(ID_32777, &CIProDoc::OnTpHandle)
	ON_COMMAND(ID_32779, &CIProDoc::OnMedianFilter)
END_MESSAGE_MAP()


// CIProDoc ����/����

CIProDoc::CIProDoc()
	: m_bfile(FALSE)
{
	// TODO: �ڴ����һ���Թ������
}

CIProDoc::~CIProDoc()
{
}

BOOL CIProDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CIProDoc ���л�

void CIProDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CIProDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// ������������֧��
void CIProDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:  strSearchContent = _T("point;rectangle;circle;ole object;")��
	SetSearchContent(strSearchContent);
}

void CIProDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CIProDoc ���

#ifdef _DEBUG
void CIProDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIProDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CIProDoc ����
void CIProDoc::OnLineStretch()
{
	// TODO: �ڴ���������������
	CLineStDlg dlg;
	if (dlg.DoModal () == IDOK)
	{
		int min = dlg.m_nMin;
		int max = dlg.m_nMax;
		double a = dlg.m_fA;
		double b = dlg.m_fB;
		for (int i = 0; i < m_fileOut.m_Cols * m_fileOut.m_Rows; i++)
        {
			if (*(m_fileOut.m_pImgDat + i) <= max && *(m_fileOut.m_pImgDat + i) >= min)
			{
				*(m_fileOut.m_pImgDat + i) = *(m_fileOut.m_pImgDat + i) * a + b;
				if (*(m_fileOut.m_pImgDat + i) < 0)
					*(m_fileOut.m_pImgDat + i) = -*(m_fileOut.m_pImgDat + i);
				if (*(m_fileOut.m_pImgDat + i) > 255)
					*(m_fileOut.m_pImgDat + i) = 255;
			}
			UpdateAllViews (NULL);
		}
	}
}




BOOL CIProDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: �ڴ����ר�ô����/����û���
	
	if (m_fileOut.Save2File (lpszPathName) == TRUE)
	{
		m_bfile = TRUE;
//		SetModifiedFlag (FALSE);
		return TRUE;
	}
	return CDocument::OnSaveDocument(lpszPathName);
}


BOOL CIProDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

    // TODO:  �ڴ������ר�õĴ�������
	CString strFilePathName = lpszPathName;
	CString strTemp = strFilePathName.Right (3);
	if (strTemp.Compare ("bmp") == 0)
	{
		m_file.Load4File (strFilePathName);
		m_fileOut = m_file;
		m_bfile = TRUE;
		UpdateAllViews (NULL);
	}
	
	else 
	{
		CRawDlg dialog;
		if (dialog.DoModal () == IDOK)
		{
			CRaw raw;
			if (raw.ReadFromFile (strFilePathName, dialog.m_nHeight, dialog.m_nWidth) == TRUE)
		    {
			    if (AfxMessageBox ("Rawͼ���ѳɹ������ڴ棬�Ƿ�Ҫ����ת��ΪBMP��ʽ", MB_YESNO) == IDYES)
			    {
					m_file.CreateBmp (dialog.m_nWidth, dialog.m_nHeight, 1);
					for (int i = dialog.m_nHeight - 1; i > -1; i--)
					{
						for (int j = 0; j < dialog.m_nWidth; j++)
						{
							*(m_file.m_pImgDat + (dialog.m_nHeight - 1 - i) * dialog.m_nWidth +j) = *(raw.m_pBuff + i * dialog.m_nWidth + j);
						} 
					}

					m_fileOut = m_file;
					CFileDialog dlgWrite (FALSE, "bmp");
					if (dlgWrite.DoModal () == IDOK)
					{
						CString strWrite = dlgWrite.GetPathName ();
						m_file.Save2File (strWrite);
						m_bfile = TRUE;
						UpdateAllViews (NULL);
					}
				}
			}
		}
	}
	
	return TRUE;
}


void CIProDoc::OnTpHandle()
{
	// TODO: �ڴ���������������
	CTpDialog dlg;
	if (dlg.DoModal () == TRUE)
	{
		int i, j, k; 
		int sum;
		int temp [25];
		for (i = 2; i < m_file.m_Rows - 2; i++)
	    {
			for (j = 2; j < m_file.m_Cols - 2; j++)
		    {
			    sum = 0;
				for (int z = 0; z < 25; z++)
				{
					temp [z] = 0;
				}
	
			    temp [0] = m_file.m_pImgDat [(i - 2) * m_file.m_Cols + j - 2] * dlg.m [0];
				temp [1] = m_file.m_pImgDat [(i - 2) * m_file.m_Cols + j - 2 + 1] * dlg.m [1];
				temp [2] = m_file.m_pImgDat [(i - 2) * m_file.m_Cols + j - 2 + 2] * dlg.m [2];
				temp [3] = m_file.m_pImgDat [(i - 2) * m_file.m_Cols + j - 2 + 3] * dlg.m [3];
				temp [4] = m_file.m_pImgDat [(i - 2) * m_file.m_Cols + j - 2 + 4] * dlg.m [4];
				
				temp [5] = m_file.m_pImgDat [(i - 2 + 1) * m_file.m_Cols + j - 2] * dlg.m [5];
				temp [6] = m_file.m_pImgDat [(i - 2 + 1) * m_file.m_Cols + j - 2 + 1] * dlg.m [6];
				temp [7] = m_file.m_pImgDat [(i - 2 + 1) * m_file.m_Cols + j - 2 + 2] * dlg.m [7];
				temp [8] = m_file.m_pImgDat [(i - 2 + 1) * m_file.m_Cols + j - 2 + 3] * dlg.m [8];
				temp [9] = m_file.m_pImgDat [(i - 2 + 1) * m_file.m_Cols + j - 2 + 4] * dlg.m [9];

				temp [10] = m_file.m_pImgDat [(i - 2 + 2) * m_file.m_Cols + j - 2 + 0] * dlg.m [10];
				temp [11] = m_file.m_pImgDat [(i - 2 + 2) * m_file.m_Cols + j - 2 + 1] * dlg.m [11];
				temp [12] = m_file.m_pImgDat [(i - 2 + 2) * m_file.m_Cols + j - 2 + 2] * dlg.m [12];
				temp [13] = m_file.m_pImgDat [(i - 2 + 2) * m_file.m_Cols + j - 2 + 3] * dlg.m [13];
				temp [14] = m_file.m_pImgDat [(i - 2 + 2) * m_file.m_Cols + j - 2 + 4] * dlg.m [14];

				temp [15] = m_file.m_pImgDat [(i - 2 + 3) * m_file.m_Cols + j - 2 + 0] * dlg.m [15];
				temp [16] = m_file.m_pImgDat [(i - 2 + 3) * m_file.m_Cols + j - 2 + 1] * dlg.m [16];
				temp [17] = m_file.m_pImgDat [(i - 2 + 3) * m_file.m_Cols + j - 2 + 2] * dlg.m [17];
				temp [18] = m_file.m_pImgDat [(i - 2 + 3) * m_file.m_Cols + j - 2 + 3] * dlg.m [18];
				temp [19] = m_file.m_pImgDat [(i - 2 + 3) * m_file.m_Cols + j - 2 + 4] * dlg.m [19];

				temp [20] = m_file.m_pImgDat [(i - 2 + 4) * m_file.m_Cols + j - 2 + 0] * dlg.m [20];
				temp [21] = m_file.m_pImgDat [(i - 2 + 4) * m_file.m_Cols + j - 2 + 1] * dlg.m [21];
				temp [22] = m_file.m_pImgDat [(i - 2 + 4) * m_file.m_Cols + j - 2 + 2] * dlg.m [22];
				temp [23] = m_file.m_pImgDat [(i - 2 + 4) * m_file.m_Cols + j - 2 + 3] * dlg.m [23];
				temp [24] = m_file.m_pImgDat [(i - 2 + 4) * m_file.m_Cols + j - 2 + 4] * dlg.m [24];

				for (k = 0; k < 25; k++)
			    {
				    sum = sum + temp [k];
			    }

				sum = sum / dlg.m_Zoom;
				sum = abs (sum);
				m_fileOut.m_pImgDat [i * m_file.m_Cols + j] = sum;
			    if  (m_fileOut.m_pImgDat [i * m_file.m_Cols + j] > 255)
					m_fileOut.m_pImgDat [i * m_file.m_Cols + j] = 255;
				if (m_fileOut.m_pImgDat [i * m_file.m_Cols + j] < 0)
					m_fileOut.m_pImgDat [i * m_file.m_Cols + j] = 0;
		    }
	    }
    }
	UpdateAllViews (NULL);
}

void CIProDoc::OnMedianFilter()
{
	// TODO: �ڴ���������������
}




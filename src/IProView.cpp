
// IProView.cpp : CIProView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "IPro.h"
#endif

#include "IProDoc.h"
#include "IProView.h"
#include "Raw.h"
#include "RawDlg.h"
#include "OffsetDlg.h"
#include "RotateDlg.h"
#include "ZoomDlg.h"
#include <complex>
using namespace std;
#include <math.h>
#include "Fourie.h"
#include "CSLineDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIProView

IMPLEMENT_DYNCREATE(CIProView, CScrollView)

BEGIN_MESSAGE_MAP(CIProView, CScrollView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CIProView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_DROPFILES()
	ON_WM_CREATE()
	ON_COMMAND(ID_32780, &CIProView::OnOffset)
	ON_COMMAND(ID_32783, &CIProView::OnRotate)
	ON_COMMAND(ID_32781, &CIProView::OnZoom)
	ON_COMMAND(ID_32784, &CIProView::OnInverse)
	ON_COMMAND(ID_32785, &CIProView::OnDFT)
	ON_COMMAND(ID_32786, &CIProView::OnBWHigh)
	ON_COMMAND(ID_32787, &CIProView::OnBTLow)
	ON_COMMAND(ID_32779, &CIProView::OnMidFiltering)
	ON_COMMAND(ID_32788, &CIProView::OnIFT)
	ON_COMMAND(ID_32792, &CIProView::OnGCM)
	ON_COMMAND(ID_32793, &CIProView::OnGrayMatching)
	ON_COMMAND(ID_32795, &CIProView::OnStateBinary)
	ON_COMMAND(ID_32796, &CIProView::OnExtractFeature)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_32797, &CIProView::OnWander)
	ON_WM_NCMOUSEMOVE()
	ON_COMMAND(ID_32798, &CIProView::OnTellBinary)
	ON_COMMAND(ID_HOT32799, &CIProView::OnGetCSLine)
	ON_COMMAND(ID_HOT32800, &CIProView::OnCalculateHOT)
	ON_COMMAND(ID_HOT32804, &CIProView::OnDrawHOT)
	ON_COMMAND(ID_HOT32805, &CIProView::OnDrawColor)
END_MESSAGE_MAP()

// CIProView ����/����

CIProView::CIProView()
	: m_nPosX(0)
	, m_nPosY(0)
	, m_bZoom(false)
	, m_bFourie(false)
	, m_nDel(0)
	, m_bWander(FALSE)
	, m_bWanderIn(FALSE)
	, m_nOffsetX(0)
	, m_nOffsetY(0)
	, m_bCSLine(false)
	, m_Start(0)
	, m_End(0)
	, m_bCSExt(false)
	, m_fCSEquation_a(0)
	, m_fCSEquation_b(0)
	, m_pHOT(NULL)
	, m_pHOTColor(NULL)
	, m_fHotMax(0)
	, m_fHotMin(0)
{
	// TODO: �ڴ˴���ӹ������

}

CIProView::~CIProView()
{
	if (m_nDel > 0)
	{
		delete m_fe.m_pCTData;
	    delete m_fe.m_pCFData;
	}

	if (m_pHOT != NULL)
		delete [] m_pHOT;

	if (m_pHOTColor != NULL)
		delete [] m_pHOTColor;
}

BOOL CIProView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	
	return CScrollView::PreCreateWindow(cs);
}

// CIProView ����

void CIProView::OnDraw(CDC* pDC)
{
	CIProDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
	if (pDoc->m_bfile == TRUE && m_bZoom == FALSE)
	{
		pDoc->m_fileOut.Draw2DC (pDC->m_hDC, m_nPosX, m_nPosY);
	}

	if (m_bZoom)
	{
		BITMAPINFO *pBmInf = (BITMAPINFO*)pDoc->m_fileOut.m_pBmpInfo;
		pBmInf->bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
		pBmInf->bmiHeader.biCompression = BI_RGB;
		pBmInf->bmiHeader.biPlanes      = 1;
		pBmInf->bmiHeader.biBitCount    = pDoc->m_fileOut.m_PxlBytes*8;
		pBmInf->bmiHeader.biWidth       = pDoc->m_fileOut.m_Cols;
		pBmInf->bmiHeader.biHeight      = pDoc->m_fileOut.m_Rows; 
				
		BYTE *pBuf = pDoc->m_fileOut.m_pImgDat;
		int rowBytes = ( (pBmInf->bmiHeader.biWidth*(pBmInf->bmiHeader.biBitCount)+31)& ~31 )/8;
		if ( rowBytes!=pDoc->m_fileOut.m_Cols * pDoc->m_fileOut.m_PxlBytes ){
			pBuf = new BYTE[pDoc->m_fileOut.m_Rows*rowBytes];
			for (int i=0;i<pDoc->m_fileOut.m_Rows;i++ ) memcpy( pBuf+i*rowBytes, pDoc->m_fileOut.m_pImgDat+i*pDoc->m_file.m_Cols*pDoc->m_fileOut.m_PxlBytes,pDoc->m_fileOut.m_Cols*pDoc->m_fileOut.m_PxlBytes );					
		}
		int oldMod = ::SetStretchBltMode(pDC->m_hDC, STRETCH_DELETESCANS );
		::StretchDIBits( pDC->m_hDC,
		m_nPosX,m_nPosY,pBmInf->bmiHeader.biWidth * m_nZoomX,pBmInf->bmiHeader.biHeight * m_nZoomY,
				 0,0,pBmInf->bmiHeader.biWidth,pBmInf->bmiHeader.biHeight,
				 pBuf,(CONST BITMAPINFO *)pBmInf,
				 DIB_RGB_COLORS,SRCCOPY );
		::SetStretchBltMode(pDC->m_hDC, oldMod );

		if ( pBuf!=pDoc->m_fileOut.m_pImgDat ) delete []pBuf;
	}
}


// CIProView ��ӡ


void CIProView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CIProView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CIProView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CIProView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}

void CIProView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CIProView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CIProView ���

#ifdef _DEBUG
void CIProView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CIProView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CIProDoc* CIProView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIProDoc)));
	return (CIProDoc*)m_pDocument;
}
#endif //_DEBUG


// CIProView ��Ϣ�������


void CIProView::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CIProDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	char* pFilePathName = new char[2048];
    ::DragQueryFile(hDropInfo, 0, pFilePathName, 2048);  // ��ȡ�Ϸ��ļ��������ļ���
    CString strFilePathName;
	strFilePathName.Format("%s", pFilePathName);
	CString strTemp = strFilePathName.Right (3);
	if (strTemp.Compare ("bmp") == 0)
	{
		pDoc->m_file.Load4File (strFilePathName);
		pDoc->m_fileOut = pDoc->m_file;
		pDoc->m_bfile = TRUE;
		Invalidate ();
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
					pDoc->m_file.CreateBmp (dialog.m_nWidth, dialog.m_nHeight, 1);
					for (int i = dialog.m_nHeight - 1; i > -1; i--)
					{
						for (int j = 0; j < dialog.m_nWidth; j++)
						{
							*(pDoc->m_file.m_pImgDat + (dialog.m_nHeight - 1 - i) * dialog.m_nWidth +j) = *(raw.m_pBuff + i * dialog.m_nWidth + j);
						} 
					}

					pDoc->m_fileOut = pDoc->m_file;
					CFileDialog dlgWrite (FALSE, "bmp");
					if (dlgWrite.DoModal () == IDOK)
					{
						CString strWrite = dlgWrite.GetPathName ();
						pDoc->m_file.Save2File (strWrite);
						pDoc->m_bfile = TRUE;
						Invalidate ();
					}
				}
			}
		}
	}
	

    ::DragFinish(hDropInfo);   // ע����������٣��������ͷ�Windows Ϊ�����ļ��ϷŶ�������ڴ�
    delete[] pFilePathName;
    
	CScrollView::OnDropFiles(hDropInfo);
}


int CIProView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
         return -1;
	// TODO:  �ڴ������ר�õĴ�������

	DragAcceptFiles (TRUE);
	return 0;
}


void CIProView::OnOffset()
{
	// TODO: �ڴ���������������
	COffsetDlg dlg;
	if (dlg.DoModal () == IDOK)
	{
		m_nPosX += dlg.m_nX;
		m_nPosY += dlg.m_nY;
	}
	Invalidate ();
}


void CIProView::OnRotate()
{
	// TODO: �ڴ���������������
	CIProDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CClientDC dc (this);
	CDC bmpDC;
    bmpDC.CreateCompatibleDC(&dc);
    CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, pDoc->m_file.m_Cols, pDoc->m_file.m_Rows);
    bmpDC.SelectObject(&bmp);
	BITMAPINFO *pBmInf = (BITMAPINFO*)pDoc->m_file.m_pBmpInfo;
	pBmInf->bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	pBmInf->bmiHeader.biCompression = BI_RGB;
	pBmInf->bmiHeader.biPlanes      = 1;
	pBmInf->bmiHeader.biBitCount    = pDoc->m_file.m_PxlBytes*8;
	pBmInf->bmiHeader.biWidth       = pDoc->m_file.m_Cols;
	pBmInf->bmiHeader.biHeight      = pDoc->m_file.m_Rows; 
	StretchDIBits(bmpDC.m_hDC,0,0,pDoc->m_file.m_Cols,pDoc->m_file.m_Rows,0,0,pDoc->m_file.m_Cols,pDoc->m_file.m_Rows,pDoc->m_file.m_pImgDat,(CONST BITMAPINFO *)pBmInf,DIB_RGB_COLORS,MERGECOPY); 

    CRotateDlg dlg;
	if (dlg.DoModal () == IDOK)
	{
		double x1,x2,x3; 
        double y1,y2,y3; 
        double maxWidth,maxHeight,minWidth,minHeight; 
        double srcX,srcY; 
        double sinA,cosA; 
	    double DstWidth; 
        double DstHeight; 
		dlg.m_fAngle = dlg.m_fAngle / 180.0 * 3.14159265;
        sinA = sin (dlg.m_fAngle); 
        cosA = cos(dlg.m_fAngle); 
		x1 = pDoc->m_file.m_Cols * cosA;
        y1 = pDoc->m_file.m_Cols * sinA;
        x2 = pDoc->m_file.m_Cols * cosA - pDoc->m_file.m_Rows * sinA;
        y2 = pDoc->m_file.m_Cols * sinA + pDoc->m_file.m_Rows * cosA;
        x3 = - pDoc->m_file.m_Rows * sinA;
        y3 = pDoc->m_file.m_Rows * cosA;
        minWidth = x3> (x1> x2?x2:x1)?(x1> x2?x2:x1):x3; 
        minWidth = minWidth> 0?0:minWidth; 
        minHeight = y3> (y1> y2?y2:y1)?(y1> y2?y2:y1):y3; 
        minHeight = minHeight> 0?0:minHeight; 
        maxWidth = x3> (x1> x2?x1:x2)?x3:(x1> x2?x1:x2); 
        maxWidth = maxWidth> 0?maxWidth:0; 
        maxHeight = y3> (y1> y2?y1:y2)?y3:(y1> y2?y1:y2); 
        maxHeight = maxHeight> 0?maxHeight:0; 
        DstWidth = abs(maxWidth - minWidth)+1; 
        DstHeight =abs(maxHeight - minHeight)+1; 

        HDC dcDst;//��ת����ڴ��豸���� 
        HBITMAP newBitmap; 
        dcDst = CreateCompatibleDC(dc.m_hDC); 
        newBitmap = CreateCompatibleBitmap(dc.m_hDC,(int)DstWidth,(int)DstHeight); 
        SelectObject(dcDst,newBitmap);
        ::FillRect(dcDst,CRect(0,0,DstWidth,DstHeight),CBrush(RGB(255,255,255)));

        int i = 0;
        int j = 0;
		float f1, f2;
		f1 = - 0.5 * (DstWidth - 1) * cosA - 0.5 * (DstHeight - 1) * sinA + 0.5 * (pDoc->m_file.m_Cols - 1);
		f2 = 0.5 * (DstWidth - 1) * sinA - 0.5 * (DstHeight - 1) * cosA + 0.5 * (pDoc->m_file.m_Rows - 1);
        for(i = 0; i < DstHeight; i++) 
        { for(j = 0; j  <  DstWidth; j++) 
          { 
			  srcX = (j + minWidth) * cosA + (i + minHeight) * sinA; 
              srcY = (i + minHeight) * cosA - (j + minWidth) * sinA; 

              if((srcX >= 0) && (srcX <= pDoc->m_file.m_Cols) &&(srcY >=  0) && (srcY <= pDoc->m_file.m_Cols)) 
              { 
				  BitBlt(dcDst, j, i, 1, 1, bmpDC.m_hDC, (int)srcX, (int)srcY, SRCCOPY);
              } 
          } 
         }

        CRect rtHour;
	    CPoint pCenter;
	    pCenter.x = m_nPosX + DstWidth/2;
	    pCenter.y = m_nPosY + DstHeight/2;
        rtHour.left=pCenter.x-DstWidth/2;
        rtHour.right=rtHour.left+DstWidth;
        rtHour.top=pCenter.y-DstHeight/2;
        rtHour.bottom=rtHour.top+DstHeight;
        TransparentBlt(dc.m_hDC,rtHour.left,rtHour.top,rtHour.Width(),rtHour.Height(),dcDst,0,0,rtHour.Width(),rtHour.Height(),RGB(255,255,255));

        bmpDC.DeleteDC();
        bmp.DeleteObject();
        DeleteDC(dcDst); 
        DeleteObject(newBitmap);
}

}


void CIProView::OnZoom()
{
	// TODO: �ڴ���������������
	CIProDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CClientDC dc (this);

	CZoomDlg dlg;
	if (dlg.DoModal () == IDOK)
	{
		m_nZoomX = dlg.m_nZw;
		m_nZoomY  = dlg.m_nZh;
		m_bZoom = TRUE;
		Invalidate ();  
		
/*		int nWidth = pDoc->m_fileOut.m_Cols; 
		int nHeight = pDoc->m_fileOut.m_Rows;
	
		double ZoomH = dlg.m_nZh; 
		double ZoomW = dlg.m_nZw;
		
		int cols = (int) (ZoomW * nWidth + 0.5);
		int rows = (int) (ZoomH * nHeight + 0.5);
		
		CBmpFile temp;
		temp.CreateBmp (cols, rows, pDoc->m_fileOut.m_PxlBytes);
		for (int r = 0; r < rows; r++)
		{ 
			for (int c = 0; c < cols; c++)
		    { 
				int py = (int) (r / ZoomH + 0.5); 
		        int px = (int) (c / ZoomW + 0.5); 
		        if (px >= 0 && px < nWidth && py >= 0 && py < nHeight)  
		        { 
					temp.m_pImgDat [r * cols + c] = pDoc->m_fileOut.m_pImgDat [py * nWidth + px]; 
		        } 
			}   
		}
		
		pDoc->m_fileOut = temp; 
		Invalidate();*/
	}     
}


void CIProView::OnInverse()
{
	// TODO: �ڴ���������������
	CIProDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
              
	CBmpFile t;
	t = pDoc->m_fileOut;
	pDoc->m_fileOut.CreateBmp (t.m_Rows, t.m_Cols, t.m_PxlBytes);
	for (int i = 0; i < t.m_Rows; i++)
		for (int j = 0; j < t.m_Cols; j++)
		{
			pDoc->m_fileOut [j][i] = t [i] [j];
		}

	Invalidate ();
}


void CIProView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	CIProDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ����ר�ô����/����û���
	CSize sizeTotal;
	sizeTotal.cx = pDoc->m_fileOut.m_Cols * 10;
	sizeTotal.cy = pDoc->m_fileOut.m_Rows * 10;
	SetScrollSizes (MM_TEXT, sizeTotal);
}


void CIProView::OnDFT()
{
	// TODO: �ڴ���������������
	CIProDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	m_fe.GetFWH (pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows);
	m_nDel++;

	m_fe.InPutData (m_fe.m_pCTData, pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows, pDoc->m_fileOut.m_pImgDat);
	m_fe.Fourie (m_fe.m_pCTData, pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows, m_fe.m_pCFData);
	m_fe.OutPutDataDFT (m_fe.m_pCFData, pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows, pDoc->m_fileOut.m_pImgDat);

	m_bFourie = TRUE;

	Invalidate ();
}


void CIProView::OnBWHigh()
{
	// TODO: �ڴ���������������
	CIProDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	m_fe.GetFWH (pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows);
	m_nDel++;

	m_fe.ButterWorthHighPass (pDoc->m_fileOut.m_pImgDat, pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows, 1);

	delete m_fe.m_pCFData;
	delete m_fe.m_pCTData;
	m_nDel--;

	Invalidate ();
}


void CIProView::OnBTLow()
{
	// TODO: �ڴ���������������
    CIProDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	m_fe.GetFWH (pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows);
	m_nDel++;

	m_fe.ButterWorthLowPass (pDoc->m_fileOut.m_pImgDat, pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows, 200);
	
	delete m_fe.m_pCFData;
	delete m_fe.m_pCTData;
	m_nDel--;

	Invalidate ();
}


void CIProView::OnMidFiltering()
{
	// TODO: �ڴ���������������
	CIProDoc* pDoc = GetDocument();
    CBmpFile mp;
	mp = pDoc->m_fileOut;
    int rows = pDoc->m_fileOut.m_Rows;
    int cols = pDoc->m_fileOut.m_Cols;
    int a[25];
    int temp;
    for (int r = 2;r<rows-2;r++) 
    {
		for (int c = 2;c<cols-2;c++)
        {
			a[0] = mp [r-2] [c-2];
            a[1] = mp [r-2] [c-1];
            a[2] = mp [r-2] [c];
            a[3] = mp [r-2] [c+1];
            a[4] = mp [r-2] [c+2];
            a[5] = mp [r-1] [c-2];
            a[6] = mp [r-1] [c-1];
            a[7] = mp [r-1] [c];
            a[8] = mp [r-1] [c+1];
            a[9] = mp [r-1] [c+2];
            a[10] = mp [r] [c-2];
            a[11] = mp [r] [c-1];
            a[12] = mp [r] [c];
            a[13] = mp [r] [c+1];
            a[14] = mp [r] [c+2];
            a[15] = mp [r+1] [c-2];
            a[16] = mp [r+1] [c-1];
            a[17] = mp [r+1] [c];
            a[18] = mp [r+1] [c+1];
            a[19] = mp [r+1] [c+2];
            a[20] = mp [r+2] [c-2];
            a[21] = mp [r+2] [c-1];
            a[22] = mp [r+2] [c];
            a[23] = mp [r+2] [c+1];
            a[24] = mp [r+2] [c+2];
            for (int i = 0;i<25;i++) 
            {
				for (int j = i+1;j<25;j++) 
                {
					if(a[i]>a[j])
                    {
						temp = a[i];
                        a[i] = a[j];
                        a[j] = temp;
					}
                }
            }

			pDoc->m_fileOut [r] [c] = BYTE (a [12]);
      }
   }
   Invalidate ();
}


void CIProView::OnIFT()
{
	// TODO: �ڴ���������������
    CIProDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	if (m_bFourie == TRUE)
    {
	    m_fe.IFourie (m_fe.m_pCFData, m_fe.m_pCTData, pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows);
	    m_fe.OutPutDataIFT (m_fe.m_pCTData, pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows, pDoc->m_fileOut.m_pImgDat);
		m_bFourie = FALSE;
		delete m_fe.m_pCFData;
		delete m_fe.m_pCTData;
		m_nDel--;
	}

	else 
	{
		MessageBox ("�Բ�������û����DFT���㣬����ִ�д˲�����");
		return;
	}
	
	Invalidate ();
}


void CIProView::OnGCM()
{
	// TODO: �ڴ���������������
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);

	int nWidth = pDoc->m_fileOut.m_Cols;
	int nHeight = pDoc->m_fileOut.m_Rows;

	CBmpFile bm;
	bm = pDoc->m_fileOut;

	double p0 [16] [16];
	double p45 [16] [16];
	double p90 [16] [16];
	double p135 [16] [16];

	double f1 [4];//�Ƕ��׾�
	double f2 [4];//���Ծ�
	double f3 [4];//���
	double f4 [4];//��
	double f5 [4];//����

	double u1 [4], u2 [4], delta1 [4], delta2 [4];

	double meanf1,meanf2,meanf3,meanf4,meanf5;
	meanf1 = meanf2 = meanf3 = meanf4 = meanf5 = 0;

	long R0, R45, R90, R135;
	DWORD i, j;
	int m, n;
	for(m = 0; m < 16; m++)
	{
		for(n = 0; n < 16; n++)
		{
			p0 [m] [n] = 0;
			p45 [m] [n] = 0;
			p90 [m] [n] = 0;
			p135 [m] [n] = 0;
		}
	}
	for(n = 0; n < 4; n++)
	{
		f1 [n] = 0;
		f2 [n] = 0;
		f3 [n] = 0;
		f4 [n]=0;
		f5 [n]=0;
		u1 [n]=0;
		u2 [n]=0;
		delta1 [n]=0;
		delta2 [n]=0;
	}
	int pixel = 0;
	int nMaxPixel = 0;
	for(i = 0;i < nHeight; i++)
	{
		for(j = 0; j < nWidth; j++)
		{
			if(bm [i] [j] > nMaxPixel)
			{
				nMaxPixel = bm [i] [j];
			}
		}
	}
	
	nMaxPixel = nMaxPixel+1;
	
	//256ѹ��Ϊ16��
	for(i = 0; i < nHeight; i++)
	{
		for(j = 0; j < nWidth; j++)
		{
			pixel = bm [i] [j];
			pixel = (int) (pixel * 16 / nMaxPixel);
			bm [i] [j] = pixel;
		}
	}
	int pixel_0, pixel_45, pixel_90, pixel_135;

	for(i = 0; i < nHeight; i++)
	{
		for(j = 0; j < nWidth; j++)
		{
			pixel= bm [i] [j];
			//0�ȷ���
			if (j < nWidth - 1)
			{
				pixel_0 = bm [i] [j + 1];
				p0 [pixel] [pixel_0]++;
				p0 [pixel_0] [pixel]++;
			}
			//45�ȷ���
			if(i < nHeight - 1 && j < nWidth - 1)
			{	
				pixel_45 = bm [i + 1] [j + 1];
				p45 [pixel] [pixel_45]++;
				p45 [pixel_45] [pixel]++;
			}
			//90��
			if(i < nHeight - 1)
			{
				pixel_90 = bm [i + 1] [j];
				p90 [pixel] [pixel_90]++;
				p90 [pixel_90] [pixel]++;
			}
			//135����
			if(j > 0 && i < nHeight - 1)
			{
				pixel_135 = bm [i + 1] [j - 1];
				p135 [pixel] [pixel_135]++;
				p135 [pixel_135] [pixel]++;
			}
		}	
	}
	R0 = 2 * nHeight * (nWidth - 1);
	R45 = 2 * (nHeight - 1) * (nWidth - 1);
	R90 = 2 * (nHeight - 1) * nWidth;
	R135 = 2 * (nHeight - 1) * (nWidth - 1);
	for (m = 0; m < 16; m++)
	{
		for(n = 0; n < 16; n++)
		{
			p0 [m] [n] = p0 [m] [n] / R0;
			p45 [m] [n] = p45 [m] [n] / R45;
			p90 [m] [n] = p90 [m] [n] / R90;
			p135 [m] [n] = p135 [m] [n] / R135;
			//��Ƕ��׾�
			f1 [0] += p0 [m] [n] * p0 [m] [n];
			f1 [1] += p45 [m] [n] * p45 [m] [n];
			f1 [2] += p90 [m] [n] * p90 [m] [n];
			f1 [3] += p135 [m] [n] * p135 [m] [n];
			//���Ծ���
			f2 [0] += (m - n) * (m - n) * p0 [m] [n];
			f2 [1] += (m - n) * (m - n) * p45 [m] [n];
			f2 [2] += (m - n) * (m - n) * p90 [m] [n];
			f2 [3] += (m - n) * (m - n) * p135 [m] [n];
		}	
	}
	double temp1, temp2, temp3, temp4;
	temp1 = temp2 = temp3 = temp4 = 0;
	for (m = 0; m < 16; m++)
	{
		temp1 = temp2 = temp3 = temp4 = 0;
		for(n = 0; n < 16; n++)
		{
			temp1 += p0 [m] [n];
			temp2 += p45 [m] [n];
			temp3 += p90 [m] [n];
			temp4 += p135 [m] [n];
		}
		u1[0] += temp1 * m;
		u1[1] += temp2 * m;
		u1[2] += temp3 * m;
		u1[3]+=temp4*m;
	}
	for(n = 0; n < 16; n++)
	{
		temp1 = temp2 = temp3 = temp4 = 0;
		for(m = 0; m < 16; m++)
		{
			temp1 += p0[m][n];
			temp2 += p45[m][n];
			temp3 += p90 [m] [n];
			temp4 += p135[m][n];
		}
		u2[0] += temp1*n;
		u2[1] += temp2*n;
		u2[2] += temp3*n;
		u2[3] += temp4*n;
	}
	for (m = 0;m < 16; m++)
	{
		temp1 = temp2 = temp3 = temp4 = 0;
		for (n = 0; n < 16; n++)
		{
			temp1 += p0 [m] [n];
			temp2 += p45 [m] [n];
			temp3 += p90 [m] [n];
			temp4 += p135 [m] [n];
		}
		delta1 [0] += temp1 * (m - u1 [0]) * (m - u1 [0]);
		delta1 [1] += temp2 * (m - u1 [1]) * (m - u1 [1]);
		delta1 [2] += temp3 * (m - u1 [2]) * (m - u1 [2]);
		delta1 [3] += temp4 * (m - u1 [3]) * (m - u1 [3]);
	}
	for (n = 0; n < 16; n++)
	{
		temp1 = temp2 = temp3 = temp4 = 0;
		for (m = 0; m < 16; m++)
		{
			temp1 += p0 [m] [n];
			temp2 += p45 [m] [n];
			temp3 += p90 [m] [n];
			temp4 += p135 [m] [n];
		}
		delta2 [0] += temp1 * (n - u2 [0]) * (n - u2 [0]);
		delta2 [1] += temp2 * (n - u2 [1]) * (n - u2 [1]);
		delta2 [2] += temp3 * (n - u2 [2]) * (n - u2 [2]);
		delta2[3]+=temp4*(n-u2[3])*(n-u2[3]);
	}
	//�����ĸ���������f3
	temp1 = temp2 = temp3 = temp4 = 0;
	for(m = 0;m < 16; m++)
	{
		for(n = 0; n < 16; n++)
		{
			temp1 += m * n * p0 [m] [n];
			temp2 += m * n * p45 [m] [n];
			temp3 += m * n * p90 [m] [n];
			temp4 += m * n * p135 [m] [n];
		}
	}
	f3 [0] = (temp1 - u1 [0] * u2 [0]) / (delta1 [0] * delta2 [0]);
	f3 [1] = (temp2 - u1 [1] * u2 [1]) / (delta1 [1] * delta2 [1]);
	f3 [2] = (temp3 - u1 [2] * u2 [2]) / (delta1 [2] * delta2 [2]);
	f3 [3] = (temp4 - u1 [3] * u2 [3]) / (delta1 [3] * delta2 [3]);
	//������f4������f5
	for (m = 0; m < 16; m++)
	{
		for (n = 0; n < 16; n++)
		{
			if (p0 [m] [n] > 0)
			{
				f4 [0] -= p0 [m] [n] * log (p0 [m] [n]) / log (2.0);
			}
			if (p45 [m] [n] > 0)
			{
				f4[1] -= p45 [m] [n] * log (p45 [m] [n]) / log (2.0);
			}
			if (p90 [m] [n] > 0)
			{
				f4 [2] -= p90 [m] [n] * log (p90 [m] [n]) / log(2.0);
			}
			if (p135 [m] [n] > 0)
			{
				f4[3] -= p135 [m] [n] * log (p135 [m] [n]) / log(2.0);
			}
			f5 [0] += p0 [m] [n] / (1 + (m - n) * (m - n));
			f5 [1] += p45 [m] [n] / (1 + (m - n) * (m - n));
			f5 [2] += p90 [m] [n] / (1 + (m - n) * (m - n));
			f5 [3] += p135 [m] [n] / (1 + (m - n) * (m - n));
		}
	}
	for (m = 0; m < 4; m++)
	{
		meanf1 += f1 [m];
		meanf2 += f2 [m];
		meanf3 += f3 [m];
		meanf4 += f4 [m];
		meanf5 += f5 [m];
	}
	meanf1 = meanf1 / 4.0;
	meanf2 = meanf2 / 4.0;
	meanf3 = meanf3 / 4.0;
	meanf4 = meanf4 / 4.0;
	meanf5 = meanf5 / 4.0;

	CString strInfo;
	strInfo.Format ("���׾�=%.3lf\n���Ծ�=%.3lf\n���=%.3lf\n��=%.3lf\n����=%.3lf\n", meanf1, meanf2, meanf3, meanf4, meanf5);
    ::MessageBox(NULL,strInfo,"�Ҷȹ�������������",MB_OK);
}


void CIProView::OnGrayMatching()
{
	// TODO: �ڴ���������������
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);

	if (pDoc->m_fileOut.m_pImgDat == NULL)
	{
		MessageBox ("����δ����Դͼ����������Դͼ���ٽ���ģ��ƥ�䣡");
		return;
	}

	CFileDialog dlg (TRUE);
	dlg.DoModal ();
	CString sPath;
	sPath = dlg.GetPathName ();
	CBmpFile file;
    file.Load4File (sPath);
	MessageBox ("ģ��ͼ���Ѷ����ڴ棡����ȷ����ť��ʼͼ��ƥ�䣡");

	double dSigmaST, dSigmaS, dSigmaT, R, MaxR;

	long lMaxWidth, lMaxHeight;

	BYTE pixel, templatepixel;
	templatepixel = 0;
    pixel = 0;

	dSigmaT = 0;
	for (int m = 0; m < file.m_Rows; m++)
	{
		for (int n = 0; n < file.m_Cols; n++)
		{
			templatepixel = file [m] [n];
			dSigmaT += (double) templatepixel * templatepixel;
		}
	}

	MaxR = 0.0;
	for (int i = 0; i < pDoc->m_fileOut.m_Rows - file.m_Rows + 1; i++)
	{
		for (int j = 0; j < pDoc->m_fileOut.m_Cols - file.m_Cols + 1; j++)
		{
			dSigmaST = 0;
			dSigmaS = 0;

			for (int m = 0; m < file.m_Rows; m++)
			{
				for (int n = 0; n < file.m_Cols; n++)
				{
					dSigmaS += (double) pDoc->m_fileOut [i + m] [j + n] * pDoc->m_fileOut [i + m] [j + n];
					dSigmaST += (double) pDoc->m_fileOut [i + m] [j + n] * file [m] [n];
				}
			}

				
		    R = dSigmaST / (sqrt (dSigmaS) * sqrt (dSigmaT));

		    if (R > MaxR)
		    {
				MaxR = R;
			    lMaxWidth = j;
			    lMaxHeight = i;
		    }
		}
	}

	for (int m = 0; m < file.m_Rows; m++)
	{
		for (int n = 0; n < file.m_Cols; n++)
		{
			pDoc->m_fileOut [m + lMaxHeight] [n + lMaxWidth]  = file [m] [n];
		}
	}

	Invalidate ();
}


void CIProView::OnStateBinary()
{
	// TODO: �ڴ���������������
   
    // ��ȡ�ĵ�ָ��
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);
	
	// ��ȷ���ȡ��ֵ
	///////////////////////////////////////////////////

	// ������ֵnThreshold
	int nThreshold = 0;
	int nNewThreshold = 0;

	// ���岢��ʼ���Ҷ�ͳ������his [256]
	int his [256];
	memset (his, 0, sizeof (his));

	// lS1,lS2�ֱ������1���ص���������2���ص�����
	int lS1, lS2;

	// lP1,lP2�ֱ��ʾ��1�����غ���2������
	double lP1, lP2;

	// Avervalue1,Avervalue2�ֱ������1�ҶȾ�ֵ����2�ҶȾ�ֵ
	double Avervalue1,Avervalue2;

	// ��������
	int IterationTimes;

	// �Ҷ������Сֵ
	int graymin = 255, graymax = 0;

	// ѭ������i, j, k
	DWORD i, j;
	int k;

	// ͳ��ֱ��ͼ
	for (i = 0; i < pDoc->m_fileOut.m_Rows; i++)
	{
		for (j = 0; j < pDoc->m_fileOut.m_Cols; j++)
		{
			BYTE gray = pDoc->m_fileOut [i] [j];
			his [gray] ++;
			if (gray > graymax)
				graymax = gray;
			if (gray < graymin)
				graymin = gray;
		}
	}

	// ����ֵ��������ֵ
	nNewThreshold = int ((graymax + graymin) / 2);

	// �����������ֵ
	for (IterationTimes = 0; nThreshold != nNewThreshold && IterationTimes < 100; IterationTimes ++)
	{
		nThreshold = nNewThreshold;
		lP1 = 0.0;
		lP2 = 0.0;
		lS1 = 0;
		lS2 = 0;

		// ����1�������غ���������
		for (k = graymin; k <= nThreshold; k++)
		{
			lP1 += (double) k * his [k];
			lS1 += his [k];
		}

		// ������1��ֵ
		Avervalue1 = lP1 / lS1;

		// ����2�������غ���������
		for (k = nThreshold + 1; k <= graymax; k++)
		{
			lP2 += (double) k * his [k];
			lS2 += his [k];
		}

		// ������2�ľ�ֵ
		Avervalue2 = lP2 / lS2;

		// ��ȡ�µ���ֵ
		nNewThreshold = int ((Avervalue1 + Avervalue2) / 2);
	}

	CString strInfo;
	strInfo.Format ("��ֵ = %d\n", nThreshold);
	::MessageBox (NULL, strInfo, "��ֵ���", MB_OK);

	// ��ֵ��ͼ��
	for (i = 0; i < pDoc->m_fileOut.m_Rows; i++)
	{
		for (j = 0; j < pDoc->m_fileOut.m_Cols; j++)
		{
			if (pDoc->m_fileOut [i] [j] < nThreshold)
				pDoc->m_fileOut [i] [j] = 0;
			else 
				pDoc->m_fileOut [i] [j] = 255;
		}
	}

	Invalidate ();
}


void CIProView::OnExtractFeature()
{
	// TODO: �ڴ���������������
	MessageBox ("����ȷ��������ͼ���Ƕ�ֵͼ�񣬷�����������ֵ����");
	
	// ����ĵ�ָ��
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);

	// ͼ��ĸߺͿ�
	UINT nWidth = pDoc->m_fileOut.m_Cols;
	UINT nHeight = pDoc->m_fileOut.m_Rows;
	
	// ���ڼ�¼��Ե���أ������ܳ�
	CBmpFile bmpPer;
	bmpPer.CreateBmp (pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows, pDoc->m_fileOut.m_PxlBytes);    

    // ���ڴ�Ÿ������ص���ͨ���
	CBmpFile bmpTemp;
	bmpTemp.CreateBmp (pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows, pDoc->m_fileOut.m_PxlBytes);

	// �����ж�ɨ���Ƿ�Ӧ�ü�������
	BOOL bMove = FALSE;
	
	// counter���ڼ�¼��ͨ��
	int counter = 0;

	// ѭ���������м����
	int i, j, m, n, t, present;

    const int T = 50;    

	//  �ȶ�ͼ�����Ԥ�����ٿ�ʼ����ͼ��������ز��õ����ǵ���ͨ��ţ��������ĸ���ͨ����
	//////////////////////////////////////////////////////////////////////////////////////

	// ��ͼ���е�һ�к͵�һ������ֵ��Ϊ255
	for (i = 0; i < nWidth; i++)
		pDoc->m_fileOut [nHeight - 1] [i] = 255;
	
	for (j = 0; j < nHeight; j++)
		pDoc->m_fileOut [nHeight - 1 - j] [0] = 255;

	for (j = 1; j < nHeight - 1; j++)
	{
		if (bMove == TRUE)
			break;

		for (i = 1; i < nWidth - 1; i++)
		{
			if (counter > 255)
			{
				AfxMessageBox ("��ͨ������Ŀ̫�࣬�������������");
				bMove = TRUE;
				return;
			}
			
			if(pDoc->m_fileOut [nHeight - j - 1] [i] < T)    // ���ڣ�ij�������ػҶ�==0
			{
				if (pDoc->m_fileOut [nHeight - j] [i + 1] < T)    // ����������=0
				{
					bmpTemp [nHeight - j - 1] [i] = bmpTemp [nHeight - j] [i + 1];    // ���������ر�Ÿ��赱ǰ����
					present = bmpTemp [nHeight - j] [i + 1];    // �ҽ��������ر�Ÿ���present
					
					if (pDoc->m_fileOut [nHeight - j - 1] [i - 1] < T && bmpTemp [nHeight - j - 1] [i - 1] != present)    //��������=0�ұ�Ų���������
					{   
						int temp = bmpTemp [nHeight - j - 1] [i - 1];    // ����һ���ȼ۳�ͻ���������ص���ͨ��Ÿ�ֵ��temp
						
						if(temp < present)    // ��������ر��С����������
						{
							present = temp;    // ����ı�Ÿ���present
							temp = bmpTemp [nHeight - j] [i + 1];    // �����ϱ�Ÿ���temp
						}
						
						counter--;
						
						for (m = 1; m < nHeight; m++)    // ������forѭ����Ŀ���ǽ�֮ǰ������һ����ͨ����ı�Ŷ���Ϊ�����صı�ţ������������ϵı�Ŷ���һ
						{
							for (n = 1; n < nWidth; n++)    // Ŀ����Ϊ�˱��ֱ�Ű���ͨ�ɷִ��ϵ��´����ҵı�Ŵ�С��������
							{
								if(bmpTemp [nHeight - m - 1] [n] == temp)
								{
									bmpTemp [nHeight - m - 1] [n] = present;    // ��֮ǰ���еı��Ϊtemp�����ر��Ϊpresent
								}
								
								else if(bmpTemp [nHeight - m - 1] [n] > temp)
								{
									bmpTemp [nHeight - m - 1] [n] -= 1;    // �����б�Ŵ���temp�����صı�Ŷ���һ
								}
							}
						}
					}//end ��ߺ�����
					
					if(pDoc->m_fileOut [nHeight - j] [i - 1] < T && bmpTemp [nHeight-j] [i - 1] != present)    // ����������=0�ұ�Ų���������
					{
						counter--;
						int temp = bmpTemp [nHeight - j] [i - 1];    // ����һ���ȼ۳�ͻ�����������ص���ͨ��Ÿ�ֵ��temp
						
						if(present < temp)    // ����������ر��С����������
						{
							present = temp;    // �����ϵı�Ÿ���present
							temp = bmpTemp [nHeight - j] [i - 1];    // �����ϱ�Ÿ���temp
						}
					
						for (m = 1; m < nHeight; m++)    // ������forѭ����Ŀ���ǽ�֮ǰ������һ����ͨ����ı�Ŷ���Ϊ�������صı�ţ������������ϵı�Ŷ���һ
						{
							for (n = 1; n < nWidth; n++)    // Ŀ����Ϊ�˱��ֱ�Ű���ͨ�ɷִ��ϵ��´����ҵı�Ŵ�С��������
							{
								if (bmpTemp [nHeight-m - 1] [n] == present)
								{
									bmpTemp [nHeight-m - 1] [n] = temp;    // ��֮ǰ���е�������ͨ���������صı�ű��Ϊ���ϱ��
								}
								
								else if (bmpTemp [nHeight-m - 1] [n] > present)
								{
									bmpTemp [nHeight-m - 1] [n] -= 1;    // ���������ϱ�ŵ��������ر�Ŷ���һ
								}
							}
						}
						
						present = temp;

					}//end ���Ϻ�����
				}
				
				// ���򣬰����������ϣ����ϣ����ҵ�˳�򣬱�Ǵ˵�Ϊ�ĸ��е�һ����
				else if (pDoc->m_fileOut [nHeight-j] [i] < T)    // ��������=0�����ǵ�ǰ���صı��Ϊ�����صı��
				{
					bmpTemp [nHeight - j - 1] [i] = bmpTemp [nHeight - j] [i];
					present = bmpTemp [nHeight - j] [i];
				}
				
				else if (pDoc->m_fileOut [nHeight-j] [i - 1] < T)    // ����������=0�����ǵ�ǰ���صı��Ϊ�������صı��
				{
					bmpTemp [nHeight - j - 1] [i] = bmpTemp [nHeight - j] [i - 1];
					present = bmpTemp [nHeight - j] [i - 1];
				}
				
				else if(pDoc->m_fileOut [nHeight-j - 1] [i - 1] < T)    // ��������=0�����ǵ�ǰ���صı��Ϊ�������صı��
				{
					bmpTemp [nHeight - j - 1] [i] = bmpTemp [nHeight - j - 1] [i - 1];
					present = bmpTemp [nHeight - j - 1] [i - 1];
				}
				
				//���϶������㣬��Ϊһ���µ�����Ŀ�ʼ����ͨ��������һ
				else
				{
					++counter;
					present = counter;
					bmpTemp [nHeight - j - 1] [i] = present;
				}
			}
        }
    }

    int num = counter;

    Shapefeature* m_shapefeature = new Shapefeature [num];    // �ṹ�������ڴ����,���������ڱ���ĳ��ͨ������״������
	
	for (i = 0; i < num; i++)
	{
		m_shapefeature [i].index = i + 1;
		m_shapefeature [i].lefttop.x = nWidth;
		m_shapefeature [i].lefttop.y = nHeight;
		m_shapefeature [i].rightbottom.x = 0;
		m_shapefeature [i].rightbottom.y = 0;
		m_shapefeature [i].area = 0;
		m_shapefeature [i].arealength = 0;
	}

	for (t = 1; t < num + 1; t++)
	{
		for (j = 1; j < nHeight - 1; j++)
		{
			for (i = 1; i < nWidth - 1; i++)
			{
				if (bmpTemp [nHeight - j - 1] [i] == t)
				{
					if (m_shapefeature [t-1].lefttop.x > i)
						m_shapefeature [t-1].lefttop.x = i;
					if (m_shapefeature [t-1].lefttop.y > j)
						m_shapefeature [t-1].lefttop.y = j;
					if (m_shapefeature [t-1].rightbottom.x < i)
						m_shapefeature [t-1].rightbottom.x = i;
					if (m_shapefeature [t-1].rightbottom.y < j)
						m_shapefeature [t-1].rightbottom.y = j;
				}
			}
		}
	}
	
	// �������
	/////////////////////////////////////////////////////////////////////////////
	
	for (t = 0; t < num; t++)
	{
		for(j = 1; j < nHeight - 1; j++)
		{
			for (i = 1; i < nWidth - 1; i++)
			{
				if (bmpTemp [nHeight - j - 1] [i] == t + 1)
				{
					m_shapefeature [t].area++;
				}
			}
		}
	}
	
	// �����ܳ�
	////////////////////////////////////////////////////////////////////////////
	
	for (j = 1; j < nHeight - 1; j++)
	{
		for (i = 1; i < nWidth - 1; i++)
		{
			if (pDoc->m_fileOut [j] [i] - pDoc->m_fileOut [j] [i + 1] == 255)    // ��
				bmpPer [j] [i + 1] = 100;
			else if (pDoc->m_fileOut [j] [i + 1] - pDoc->m_fileOut [j] [i] == 255)
				bmpPer [j] [i] = 100;

			else if (pDoc->m_fileOut [j] [i] - pDoc->m_fileOut [j + 1] [i] == 255)    // ��
				bmpPer [j + 1] [i] = 100;
			else if (pDoc->m_fileOut [j + 1] [i] - pDoc->m_fileOut [j] [i] == 255)
			    bmpPer [j] [i] = 100;

		    else if (pDoc->m_fileOut [j] [i] - pDoc->m_fileOut [j + 1] [i + 1] == 255)    // ����
				bmpPer [j + 1] [i + 1] = 100;
			else if (pDoc->m_fileOut [j + 1] [i + 1] - pDoc->m_fileOut [j] [i] == 255)
				bmpPer [j] [i] = 100;

			else if (pDoc->m_fileOut [j] [i] - pDoc->m_fileOut [j + 1] [i - 1] == 255)    // ����
				bmpPer [j + 1] [i - 1] = 100;
			else if (pDoc->m_fileOut [j + 1] [i - 1] - pDoc->m_fileOut [j] [i] == 255)
				bmpPer [j] [i] = 100;

			else if (pDoc->m_fileOut [j] [i] - pDoc->m_fileOut [j - 1] [i - 1] == 255)    // ����
				bmpPer [j - 1] [i - 1] = 100;
			else if(pDoc->m_fileOut [j - 1] [i - 1] - pDoc->m_fileOut [j] [i] == 255)
				bmpPer [j] [i] = 100;

			else if (pDoc->m_fileOut [j] [i] - pDoc->m_fileOut [j - 1] [i + 1] == 255)    // ����
				bmpPer [j - 1] [i + 1] = 100;
			else if (pDoc->m_fileOut [j - 1] [i + 1] - pDoc->m_fileOut [j] [i] == 255)
				bmpPer [j] [i] = 100;
		}
	}

	for (t = 0; t < num; t++)
	{
		for (i = 1; i < nWidth - 1; i++)
		{
			for (j = 1; j < nHeight - 1; j++)
			{
				if (bmpTemp [j] [i] == t + 1)
				{
					if (bmpPer [j] [i] == 100)
						m_shapefeature [t].arealength++;
			    }
			}
		}
	}

	CString* strOutput =new CString[num];
	CString s;
	
	for (i = 0; i < num; i++)
	{
		s.Format ("  ���� : %d\n\n  ��� : %d\n  �ܳ� : %d\n  Բ�ζ� : %f\n ", m_shapefeature [i].index, m_shapefeature [i].area, m_shapefeature [i].arealength, (double) (m_shapefeature [i].area * 4 * 3.1415926) / (double) (m_shapefeature [i].arealength * m_shapefeature [i].arealength));
		::MessageBox (NULL, s, "����������ȡ���", MB_OK);
	    strOutput [i] += s;
	}

    delete [] strOutput;
	delete [] m_shapefeature;

    return;
}


void CIProView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);
	
	if (m_bWander == TRUE && m_bWanderIn == FALSE)
	{
		if (point.x <= m_nPosX + pDoc->m_fileOut.m_Cols && point.x >= m_nPosX && point.y <= m_nPosY + pDoc->m_fileOut.m_Rows && point.y >= m_nPosY)
		{
			m_bWanderIn = TRUE;
			m_nOffsetX = point.x - m_nPosX;
			m_nOffsetY = point.y - m_nPosY;
		}
	}

	if (m_bCSLine == TRUE && m_bCSExt == FALSE)
	{
		m_Start.x = point.x;
		m_Start.y = point.y;
		m_End.x = point.x;
		m_End.y = point.y;
		m_bCSExt = TRUE;
	}
	
	CScrollView::OnLButtonDown(nFlags, point);
}


void CIProView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);

	if (m_bWander == TRUE && m_bWanderIn == TRUE)
	{
		m_bWanderIn = FALSE;
	}

	if (m_bCSLine == TRUE && m_bCSExt == TRUE)
	{
		CClientDC dc(this);
        dc.SelectStockObject(NULL_BRUSH);
		CPen pen (PS_DASH, 3, RGB (255, 0, 0));
		CPen* pOldPen = dc.SelectObject (&pen);
		dc.Rectangle(m_Start.x, m_Start.y, m_End.x, m_End.y);
		dc.SelectObject (pOldPen);
		m_bCSExt = FALSE;
		CCSLineDlg dlg;
		if (dlg.DoModal () == IDOK)
		{
			m_bCSLine = FALSE;
			int nWidth = fabs (double (m_Start.x - point.x)) + 1;
			int nHeight = fabs (double (m_Start.y - point.y)) + 1;
			int nPixelNum = nWidth * nHeight;
			BYTE* pRedBand = new BYTE [nPixelNum];
			BYTE* pBlueBand = new BYTE [nPixelNum];
			double sum_X, sum_Y, sum_X2, sum_XY;
			sum_X = 0.0;
			sum_Y = 0.0;
			sum_X2 = 0.0;
			sum_XY = 0.0;
			
			if (m_Start.x > point.x && m_Start.y < point.y)
			{
				m_Start.x -= nWidth;
			}

			else if (m_Start.x > point.x && m_Start.y > point.y)
			{
				m_Start.x -= nWidth;
				m_Start.y -= nHeight;
			}

			else if (m_Start.x < point.x && m_Start.y > point.y)
			{
				m_Start.y -= nHeight;
			}

			for (int i = 0; i < nPixelNum; i++)
			{
				int frows = (i + 1) / nWidth + 1;
				int fcols = (i + 1) % nWidth;
				int trows = m_Start.y - m_nPosY + frows - 1;
				int tcols = m_Start.x - m_nPosX + fcols - 1;
				int trowsbmp = pDoc->m_fileOut.m_Rows - trows;
				int tcolsbmp = tcols;
				pRedBand [i] = pDoc->m_fileOut.m_pImgDat [(trowsbmp * pDoc->m_fileOut.m_Cols + tcolsbmp) * 3];
				sum_Y += pRedBand [i];
				pBlueBand [i] = pDoc->m_fileOut.m_pImgDat [(trowsbmp * pDoc->m_fileOut.m_Cols + tcolsbmp) * 3 + 2];
				sum_X += pBlueBand [i];
				sum_X2 += pBlueBand [i] * pBlueBand [i];
				sum_XY += pBlueBand [i] * pRedBand [i];
			}

			double Aver_X = sum_X / nPixelNum;
			double Aver_Y = sum_Y / nPixelNum;
			m_fCSEquation_b = (sum_XY - nPixelNum * Aver_X * Aver_Y) / (sum_X2 - nPixelNum * Aver_X * Aver_X);
			m_fCSEquation_a = Aver_Y - m_fCSEquation_b * Aver_X;
			CString str;
			str.Format ("����ߵķ���Ϊ��Y = %fX + (%f)", m_fCSEquation_b, m_fCSEquation_a);
			MessageBox (str);
			MessageBox ("����߼�����ϣ��뵥�����������ص�HOTֵ���˵������Ӱ���HOT��");
		}
	}

	CScrollView::OnLButtonUp(nFlags, point);
}


void CIProView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_bWander == TRUE && m_bWanderIn == TRUE)
	{
		m_nPosX = point.x - m_nOffsetX;
        m_nPosY = point.y - m_nOffsetY;
	
		Invalidate ();
	}

	if (m_bCSLine == TRUE && m_bCSExt == TRUE)
	{
		CClientDC dc (this);
		dc.SetROP2(R2_NOT);   
        dc.SelectStockObject(NULL_BRUSH);
		CPen pen (PS_DASH, 3, RGB (255, 0, 0));
		CPen* pOldPen = dc.SelectObject (&pen);
		dc.Rectangle (m_Start.x, m_Start.y, m_End.x, m_End.y);
		dc.Rectangle (m_Start.x, m_Start.y, point.x, point.y);
		m_End.x = point.x;
		m_End.y = point.y;
		dc.SelectObject (pOldPen);
	}
	
	CScrollView::OnMouseMove(nFlags, point);
}


void CIProView::OnWander()
{
	// TODO: �ڴ���������������
	m_bWander = TRUE;
	m_bCSLine = FALSE;
}



void CIProView::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_bWander == TRUE && m_bWanderIn == TRUE)
	{
		m_bWanderIn = FALSE;
	}

	CScrollView::OnNcMouseMove(nHitTest, point);
}


void CIProView::OnTellBinary()
{
	// TODO: �ڴ���������������
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);

	// ��ȡͼ���͸�
	int nWidth = pDoc->m_fileOut.m_Cols;
	int nHeight = pDoc->m_fileOut.m_Rows;;

    // ��ֵnThreshold
	int nThreshold;

	// ���岢��ʼ���Ҷ�ͳ������his[256]
	int his [256];
	memset (his, 0, sizeof(his));

    // lS,lS1,LS2 �ֱ����������������1������������2��������
	int lS, lS1, lS2;

	// lP, lP1�ֱ�����ܵ������غ���1������
	double lP, lP1;

	// AverValue1, Avervalue2�ֱ������1�ҶȾ�ֵ����2�ҶȾ�ֵ
	double AverValue1, AverValue2;

	// Disper1, Disper2, ClassinDisper, ClassoutDisper�� max��ʾ��1�����2������ڷ����䷽�max��ʾ��������
	double Disper1, Disper2, ClassinDisper, ClassoutDisper, max;

	// �Ҷ������Сֵ
	int Gmin = 255, Gmax = 0;

    // ѭ������
	int i, j, k;
		
	//ͳ�Ƴ������Ҷ�ֵ��������
	for(i=0;i<nHeight;i++)
	{
		for(j=0;j<nWidth;j++)
		{
			int gray = pDoc->m_fileOut [i] [j];
			his [gray]++;
			
			if(gray > Gmax) 
				Gmax=gray;
		    
			if(gray < Gmin) 
				Gmin=gray;
		}
	}

	// ����ֵ
	lP=0.0;
	lS=0;
	k = 0;
	
	if (Gmin == 0) 
		Gmin++;

	//�����ܵ�������lP����������lS
	for (k = Gmin; k < Gmax; k++)
	{
		lP += (double) k * (double) his [k];
		lS += his [k];
	}

	// ����ֵ�������ֲ�������Ϊ����������׼��
	max=0.0;
	lS1 = 0;
	lS2 = 0;
	lP1 = 0.0;
	Disper1 = 0.0;
	Disper2 = 0.0;
	ClassinDisper = 0.0;
	ClassoutDisper = 0.0;
	double ratio = 0.0;

	// ����ֵ
	for (k = Gmin; k < Gmax; k++)
	{
		lS1 += his [k];

		if(!lS1)
			continue;
		
		// ������2��������
		lS2 = lS - lS1; 		
		if (lS2 == 0)
			break;

		// ������1������
		lP1 += (double) k * his [k];

		// ������1��ֵ
		AverValue1 = lP1 / lS1;

		// ������1�䷽��
		for (int n = Gmin;n<=k;n++)
			Disper1 += ((n - AverValue1) * (n - AverValue1) * his [n]);

		// ������2���ֵ
		AverValue2 = (lP - lP1) / lS2;

		// ������2�䷽��
		for (int m = k+1; m <= Gmax; m++)
			Disper2 += ((m - AverValue2) * (m - AverValue2) * his [m]);

		// �������ڷ���
		ClassinDisper = Disper1 + Disper2;

		// ������䷽��
		ClassoutDisper = (double) lS1 * (double) lS2 * (AverValue1 - AverValue2) * (AverValue1 - AverValue2);

		// ��䷽�������ڷ����ֵ
		if (ClassinDisper != 0) 
			ratio = ClassoutDisper / ClassinDisper;

		// ��ȡ�ָ���ֵnThreshold
        if (ratio > max)
	    {
			max = ratio;
		    nThreshold = k;  
	    } 
	}
   
	CString strInfo;
    strInfo.Format ("��ֵ=%d\n", nThreshold);
	::MessageBox (NULL, strInfo, "��ֵ���", MB_OK);

	// ��ֵ��ͼ��
	for(i = 0; i < nHeight; i++)
    {
		for (j = 0; j < nWidth; j++)
		{
			if (pDoc->m_fileOut [i] [j] < nThreshold) 
				pDoc->m_fileOut [i] [j] = 0;
			
			else 
				pDoc->m_fileOut [i] [j] = 255;
		}
	}

	Invalidate ();
}


void CIProView::OnGetCSLine()
{
	// TODO: �ڴ���������������
	MessageBox ("��ȷ��ͼ���Ƿ���룬���Ѷ�������������ѡͼ���е�������򣡿�ѡ�������벻Ҫʹ���γ���ͼ��Χ�����������쳣��������");
	m_bWander = FALSE;
	m_bCSLine = TRUE;
}


void CIProView::OnCalculateHOT()
{
	// TODO: �ڴ���������������

	// ��ȡ�ĵ�ָ��
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);

	int nPixelNum;    // Ӱ����������
	int nWidth = pDoc->m_fileOut.m_Cols;     // Ӱ����
	int nHeight = pDoc->m_fileOut.m_Rows;    // Ӱ��߶�     
	nPixelNum = nWidth * nHeight;
	m_pHOT = new double [nPixelNum]; 	// Ϊָ�����鶯̬�����ڴ�
	m_pHOTColor = new double [nPixelNum];
	int nRows, nCols;                   // �С���
	nRows = 0;
	nCols = 0;
	double a = 0.0;
	if (m_fCSEquation_b < 0)
		a = 3.14159265 - atan (fabs (m_fCSEquation_b));
	else 
		a = atan (m_fCSEquation_b);
	for (int i = 0; i < nPixelNum; i++)
	{
		nRows = (i + 1) / nWidth;
		nCols = (i + 1) % nWidth - 1;
		BYTE iRed = pDoc->m_fileOut.m_pImgDat [(nRows * pDoc->m_fileOut.m_Cols + nCols) * 3];
		BYTE iBlue = pDoc->m_fileOut.m_pImgDat [(nRows * pDoc->m_fileOut.m_Cols + nCols) * 3 + 2];
		m_pHOT [i] = fabs (iBlue * sin (a) - iRed * cos (a));
		m_pHOTColor [i] = m_pHOT [i];
        if (i == 0)
			m_fHotMin = m_pHOT [i];
		if (m_pHOT [i] > m_fHotMax) 
			m_fHotMax = m_pHOT [i];
		if (m_pHOT [i] < m_fHotMin)
			m_fHotMin = m_pHOT [i];
	}

	MessageBox ("Ӱ��HOTֵ������ϣ��뵥����HOTͼ�񡯲˵���鿴HOTͼ��");
}

void CIProView::OnDrawHOT()
{
	// TODO: �ڴ���������������
	
	// ��ȡ�ĵ�ָ��
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);

	MessageBox ("���ڽ��д˲���ǰ��֤Դͼ��������ͼΪ��ǰ���ͼ��������������ܻ�����ڴ���ʴ���");

	// ��������ʼ��HOTλͼ
	CBmpFile bmp;
	bmp.CreateBmp (pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows, 1);

	// ��0-255������ź��HOTֵ
	BYTE nGray;

	// ��HOTֵ����bmpͼ��
	for (int i = 0; i < pDoc->m_fileOut.m_Cols * pDoc->m_fileOut.m_Rows; i++)
	{
		nGray = (BYTE) (((m_pHOT [i] - m_fHotMin) / (m_fHotMax - m_fHotMin)) * 255.0);
		bmp.m_pImgDat [i] = nGray * 2;
	}

	// ������ͼ���鿴HOTͼ��
	CMainFrame* pFrame = (CMainFrame*) (AfxGetApp ()->m_pMainWnd);

	// ����һ���½��ļ�����Ϣ������һ���µ��ĵ�-��ͼ
	pFrame->SendMessage (WM_COMMAND, ID_FILE_NEW);

	// ��ȡ�½���ͼָ��
	CIProView* pView = (CIProView*) pFrame->MDIGetActive ()->GetActiveView ();

	// ��ȡ�¹������µ��ĵ���ָ��
	CIProDoc* pDocNew = pView->GetDocument ();

	// ���ĵ���ȡHOTͼ��
	pDocNew->m_fileOut = bmp;
	pDocNew->m_bfile = TRUE;

	// ˢ����ʾ
	pDocNew->UpdateAllViews (pView);
}


void CIProView::OnDrawColor()
{
	// TODO: �ڴ���������������
	
	// ��ȡ�ĵ�ָ��
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);

	MessageBox ("���ڽ��д˲���ǰ��֤Դͼ��������ͼΪ��ǰ���ͼ��������������ܻ�����ڴ���ʴ���");

	// ��������ʼ��HOTλͼ
	CBmpFile bmp;
	bmp.CreateBmp (pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows, 3);

	double fRegion = 0.0;
	fRegion = (m_fHotMax - m_fHotMin) / 5;

	// ��HOTֵ����bmpͼ��
	for (int i = 0; i < pDoc->m_fileOut.m_Cols * pDoc->m_fileOut.m_Rows; i++)
	{
		if (m_pHOTColor [i] < m_fHotMin + fRegion)
		{
			bmp.m_pImgDat [i * 3 + 2] = 0;
			bmp.m_pImgDat [i * 3 + 1] = 151;
			bmp.m_pImgDat [i * 3] = 225;
		}

		else if (m_pHOTColor [i] < m_fHotMin + fRegion * 2)
		{
			bmp.m_pImgDat [i * 3 + 2] = 80;
			bmp.m_pImgDat [i * 3 + 1] = 99;
			bmp.m_pImgDat [i * 3] = 173;
		}

		else if (m_pHOTColor [i] < m_fHotMin + fRegion * 3)
		{
			bmp.m_pImgDat [i * 3 + 2] = 161;
			bmp.m_pImgDat [i * 3 + 1] = 0;
			bmp.m_pImgDat [i * 3] = 130;
		}

		else if (m_pHOTColor [i] < m_fHotMin + fRegion * 4)
		{
			bmp.m_pImgDat [i * 3 + 2] = 231;
			bmp.m_pImgDat [i * 3 + 1] = 59;
			bmp.m_pImgDat [i * 3] = 121;
		}

		else 
		{
			bmp.m_pImgDat [i * 3 + 2] = 138;
			bmp.m_pImgDat [i * 3 + 1] = 28;
			bmp.m_pImgDat [i * 3] = 33;
		}
	}

	// ������ͼ���鿴HOTͼ��
	CMainFrame* pFrame = (CMainFrame*) (AfxGetApp ()->m_pMainWnd);

	// ����һ���½��ļ�����Ϣ������һ���µ��ĵ�-��ͼ
	pFrame->SendMessage (WM_COMMAND, ID_FILE_NEW);

	// ��ȡ�½���ͼָ��
	CIProView* pView = (CIProView*) pFrame->MDIGetActive ()->GetActiveView ();

	// ��ȡ�¹������µ��ĵ���ָ��
	CIProDoc* pDocNew = pView->GetDocument ();

	// ���ĵ���ȡHOTͼ��
	pDocNew->m_fileOut = bmp;
	pDocNew->m_bfile = TRUE;

	// ˢ����ʾ
	pDocNew->UpdateAllViews (pView);
}

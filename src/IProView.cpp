
// IProView.cpp : CIProView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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
	// 标准打印命令
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

// CIProView 构造/析构

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
	// TODO: 在此处添加构造代码

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
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	
	return CScrollView::PreCreateWindow(cs);
}

// CIProView 绘制

void CIProView::OnDraw(CDC* pDC)
{
	CIProDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
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


// CIProView 打印


void CIProView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CIProView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CIProView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CIProView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
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


// CIProView 诊断

#ifdef _DEBUG
void CIProView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CIProView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CIProDoc* CIProView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIProDoc)));
	return (CIProDoc*)m_pDocument;
}
#endif //_DEBUG


// CIProView 消息处理程序


void CIProView::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CIProDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	char* pFilePathName = new char[2048];
    ::DragQueryFile(hDropInfo, 0, pFilePathName, 2048);  // 获取拖放文件的完整文件名
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
			    if (AfxMessageBox ("Raw图像已成功读入内存，是否要将其转换为BMP格式", MB_YESNO) == IDYES)
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
	

    ::DragFinish(hDropInfo);   // 注意这个不能少，它用于释放Windows 为处理文件拖放而分配的内存
    delete[] pFilePathName;
    
	CScrollView::OnDropFiles(hDropInfo);
}


int CIProView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
         return -1;
	// TODO:  在此添加您专用的创建代码

	DragAcceptFiles (TRUE);
	return 0;
}


void CIProView::OnOffset()
{
	// TODO: 在此添加命令处理程序代码
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
	// TODO: 在此添加命令处理程序代码
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

        HDC dcDst;//旋转后的内存设备环境 
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
	// TODO: 在此添加命令处理程序代码
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
	// TODO: 在此添加命令处理程序代码
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

	// TODO: 在此添加专用代码和/或调用基类
	CSize sizeTotal;
	sizeTotal.cx = pDoc->m_fileOut.m_Cols * 10;
	sizeTotal.cy = pDoc->m_fileOut.m_Rows * 10;
	SetScrollSizes (MM_TEXT, sizeTotal);
}


void CIProView::OnDFT()
{
	// TODO: 在此添加命令处理程序代码
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
	// TODO: 在此添加命令处理程序代码
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
	// TODO: 在此添加命令处理程序代码
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
	// TODO: 在此添加命令处理程序代码
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
	// TODO: 在此添加命令处理程序代码
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
		MessageBox ("对不起，您还没有做DFT运算，不能执行此操作！");
		return;
	}
	
	Invalidate ();
}


void CIProView::OnGCM()
{
	// TODO: 在此添加命令处理程序代码
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

	double f1 [4];//角二阶矩
	double f2 [4];//惯性矩
	double f3 [4];//相关
	double f4 [4];//熵
	double f5 [4];//逆差矩

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
	
	//256压缩为16级
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
			//0度方向
			if (j < nWidth - 1)
			{
				pixel_0 = bm [i] [j + 1];
				p0 [pixel] [pixel_0]++;
				p0 [pixel_0] [pixel]++;
			}
			//45度方向
			if(i < nHeight - 1 && j < nWidth - 1)
			{	
				pixel_45 = bm [i + 1] [j + 1];
				p45 [pixel] [pixel_45]++;
				p45 [pixel_45] [pixel]++;
			}
			//90度
			if(i < nHeight - 1)
			{
				pixel_90 = bm [i + 1] [j];
				p90 [pixel] [pixel_90]++;
				p90 [pixel_90] [pixel]++;
			}
			//135度向
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
			//求角二阶矩
			f1 [0] += p0 [m] [n] * p0 [m] [n];
			f1 [1] += p45 [m] [n] * p45 [m] [n];
			f1 [2] += p90 [m] [n] * p90 [m] [n];
			f1 [3] += p135 [m] [n] * p135 [m] [n];
			//惯性矩形
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
	//计算四个方向的相关f3
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
	//计算熵f4和逆差矩f5
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
	strInfo.Format ("二阶矩=%.3lf\n惯性矩=%.3lf\n相关=%.3lf\n熵=%.3lf\n逆差矩=%.3lf\n", meanf1, meanf2, meanf3, meanf4, meanf5);
    ::MessageBox(NULL,strInfo,"灰度共生矩阵特征量",MB_OK);
}


void CIProView::OnGrayMatching()
{
	// TODO: 在此添加命令处理程序代码
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);

	if (pDoc->m_fileOut.m_pImgDat == NULL)
	{
		MessageBox ("您还未读入源图像，请先输入源图像再进行模板匹配！");
		return;
	}

	CFileDialog dlg (TRUE);
	dlg.DoModal ();
	CString sPath;
	sPath = dlg.GetPathName ();
	CBmpFile file;
    file.Load4File (sPath);
	MessageBox ("模板图像已读入内存！单击确定按钮开始图像匹配！");

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
	// TODO: 在此添加命令处理程序代码
   
    // 获取文档指针
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);
	
	// 峰谷法获取阈值
	///////////////////////////////////////////////////

	// 定义阈值nThreshold
	int nThreshold = 0;
	int nNewThreshold = 0;

	// 定义并初始化灰度统计数组his [256]
	int his [256];
	memset (his, 0, sizeof (his));

	// lS1,lS2分别代表类1像素的总数、类2像素的总数
	int lS1, lS2;

	// lP1,lP2分别表示类1质量矩和类2质量矩
	double lP1, lP2;

	// Avervalue1,Avervalue2分别代表类1灰度均值和类2灰度均值
	double Avervalue1,Avervalue2;

	// 迭代次数
	int IterationTimes;

	// 灰度最大、最小值
	int graymin = 255, graymax = 0;

	// 循环变量i, j, k
	DWORD i, j;
	int k;

	// 统计直方图
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

	// 给阈值赋迭代初值
	nNewThreshold = int ((graymax + graymin) / 2);

	// 迭代求最佳阈值
	for (IterationTimes = 0; nThreshold != nNewThreshold && IterationTimes < 100; IterationTimes ++)
	{
		nThreshold = nNewThreshold;
		lP1 = 0.0;
		lP2 = 0.0;
		lS1 = 0;
		lS2 = 0;

		// 求类1的质量矩和像素总数
		for (k = graymin; k <= nThreshold; k++)
		{
			lP1 += (double) k * his [k];
			lS1 += his [k];
		}

		// 计算类1均值
		Avervalue1 = lP1 / lS1;

		// 求类2的质量矩和像素总数
		for (k = nThreshold + 1; k <= graymax; k++)
		{
			lP2 += (double) k * his [k];
			lS2 += his [k];
		}

		// 计算类2的均值
		Avervalue2 = lP2 / lS2;

		// 获取新的阈值
		nNewThreshold = int ((Avervalue1 + Avervalue2) / 2);
	}

	CString strInfo;
	strInfo.Format ("阈值 = %d\n", nThreshold);
	::MessageBox (NULL, strInfo, "阈值结果", MB_OK);

	// 二值化图像
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
	// TODO: 在此添加命令处理程序代码
	MessageBox ("请您确定待处理图像是二值图像，否则请先做二值化！");
	
	// 获得文档指针
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);

	// 图像的高和宽
	UINT nWidth = pDoc->m_fileOut.m_Cols;
	UINT nHeight = pDoc->m_fileOut.m_Rows;
	
	// 用于记录边缘像素，计算周长
	CBmpFile bmpPer;
	bmpPer.CreateBmp (pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows, pDoc->m_fileOut.m_PxlBytes);    

    // 用于存放各个像素的连通标号
	CBmpFile bmpTemp;
	bmpTemp.CreateBmp (pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows, pDoc->m_fileOut.m_PxlBytes);

	// 用于判断扫描是否应该继续进行
	BOOL bMove = FALSE;
	
	// counter用于记录连通数
	int counter = 0;

	// 循环变量和中间变量
	int i, j, m, n, t, present;

    const int T = 50;    

	//  先对图像进行预处理，再开始遍历图像各个像素并得到它们的连通标号，即属于哪个连通区域
	//////////////////////////////////////////////////////////////////////////////////////

	// 将图像中第一行和第一列像素值设为255
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
				AfxMessageBox ("连通区域数目太多，请减少样本个数");
				bMove = TRUE;
				return;
			}
			
			if(pDoc->m_fileOut [nHeight - j - 1] [i] < T)    // 若第（ij）个像素灰度==0
			{
				if (pDoc->m_fileOut [nHeight - j] [i + 1] < T)    // 若右上像素=0
				{
					bmpTemp [nHeight - j - 1] [i] = bmpTemp [nHeight - j] [i + 1];    // 将右上像素标号赋予当前像素
					present = bmpTemp [nHeight - j] [i + 1];    // 且将右上像素标号赋给present
					
					if (pDoc->m_fileOut [nHeight - j - 1] [i - 1] < T && bmpTemp [nHeight - j - 1] [i - 1] != present)    //若左像素=0且标号不等于右上
					{   
						int temp = bmpTemp [nHeight - j - 1] [i - 1];    // 即有一个等价冲突，将左像素的连通标号赋值给temp
						
						if(temp < present)    // 如果左像素标号小于右上像素
						{
							present = temp;    // 将左的标号赋给present
							temp = bmpTemp [nHeight - j] [i + 1];    // 将右上标号赋给temp
						}
						
						counter--;
						
						for (m = 1; m < nHeight; m++)    // 这两个for循环的目的是将之前和右上一个连通区域的标号都改为左像素的标号，并将大于右上的标号都减一
						{
							for (n = 1; n < nWidth; n++)    // 目的是为了保持标号按联通成分从上到下从左到右的标号从小到大排序
							{
								if(bmpTemp [nHeight - m - 1] [n] == temp)
								{
									bmpTemp [nHeight - m - 1] [n] = present;    // 将之前所有的标号为temp的像素标记为present
								}
								
								else if(bmpTemp [nHeight - m - 1] [n] > temp)
								{
									bmpTemp [nHeight - m - 1] [n] -= 1;    // 将所有标号大于temp的像素的标号都减一
								}
							}
						}
					}//end 左边和右上
					
					if(pDoc->m_fileOut [nHeight - j] [i - 1] < T && bmpTemp [nHeight-j] [i - 1] != present)    // 若左上像素=0且标号不等于右上
					{
						counter--;
						int temp = bmpTemp [nHeight - j] [i - 1];    // 即有一个等价冲突，将左上像素的连通标号赋值给temp
						
						if(present < temp)    // 如果左上像素标号小于右上像素
						{
							present = temp;    // 将左上的标号赋给present
							temp = bmpTemp [nHeight - j] [i - 1];    // 将右上标号赋给temp
						}
					
						for (m = 1; m < nHeight; m++)    // 这两个for循环的目的是将之前和右上一个连通区域的标号都改为左上像素的标号，并将大于右上的标号都减一
						{
							for (n = 1; n < nWidth; n++)    // 目的是为了保持标号按联通成分从上到下从左到右的标号从小到大排序
							{
								if (bmpTemp [nHeight-m - 1] [n] == present)
								{
									bmpTemp [nHeight-m - 1] [n] = temp;    // 将之前所有的右上连通区域内像素的标号标记为左上标号
								}
								
								else if (bmpTemp [nHeight-m - 1] [n] > present)
								{
									bmpTemp [nHeight-m - 1] [n] -= 1;    // 将大于左上标号的所有像素标号都减一
								}
							}
						}
						
						present = temp;

					}//end 左上和右上
				}
				
				// 否则，按照最左，左上，最上，上右的顺序，标记此点为四个中的一个。
				else if (pDoc->m_fileOut [nHeight-j] [i] < T)    // 若左像素=0，则标记当前像素的标号为左像素的标号
				{
					bmpTemp [nHeight - j - 1] [i] = bmpTemp [nHeight - j] [i];
					present = bmpTemp [nHeight - j] [i];
				}
				
				else if (pDoc->m_fileOut [nHeight-j] [i - 1] < T)    // 若左上像素=0，则标记当前像素的标号为左上像素的标号
				{
					bmpTemp [nHeight - j - 1] [i] = bmpTemp [nHeight - j] [i - 1];
					present = bmpTemp [nHeight - j] [i - 1];
				}
				
				else if(pDoc->m_fileOut [nHeight-j - 1] [i - 1] < T)    // 若上像素=0，则标记当前像素的标号为右上像素的标号
				{
					bmpTemp [nHeight - j - 1] [i] = bmpTemp [nHeight - j - 1] [i - 1];
					present = bmpTemp [nHeight - j - 1] [i - 1];
				}
				
				//以上都不满足，则为一个新的区域的开始，连通区域数加一
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

    Shapefeature* m_shapefeature = new Shapefeature [num];    // 结构体数组内存分配,该数组用于保存某连通区域形状特征量
	
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
	
	// 计算面积
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
	
	// 计算周长
	////////////////////////////////////////////////////////////////////////////
	
	for (j = 1; j < nHeight - 1; j++)
	{
		for (i = 1; i < nWidth - 1; i++)
		{
			if (pDoc->m_fileOut [j] [i] - pDoc->m_fileOut [j] [i + 1] == 255)    // 右
				bmpPer [j] [i + 1] = 100;
			else if (pDoc->m_fileOut [j] [i + 1] - pDoc->m_fileOut [j] [i] == 255)
				bmpPer [j] [i] = 100;

			else if (pDoc->m_fileOut [j] [i] - pDoc->m_fileOut [j + 1] [i] == 255)    // 下
				bmpPer [j + 1] [i] = 100;
			else if (pDoc->m_fileOut [j + 1] [i] - pDoc->m_fileOut [j] [i] == 255)
			    bmpPer [j] [i] = 100;

		    else if (pDoc->m_fileOut [j] [i] - pDoc->m_fileOut [j + 1] [i + 1] == 255)    // 左下
				bmpPer [j + 1] [i + 1] = 100;
			else if (pDoc->m_fileOut [j + 1] [i + 1] - pDoc->m_fileOut [j] [i] == 255)
				bmpPer [j] [i] = 100;

			else if (pDoc->m_fileOut [j] [i] - pDoc->m_fileOut [j + 1] [i - 1] == 255)    // 右下
				bmpPer [j + 1] [i - 1] = 100;
			else if (pDoc->m_fileOut [j + 1] [i - 1] - pDoc->m_fileOut [j] [i] == 255)
				bmpPer [j] [i] = 100;

			else if (pDoc->m_fileOut [j] [i] - pDoc->m_fileOut [j - 1] [i - 1] == 255)    // 左上
				bmpPer [j - 1] [i - 1] = 100;
			else if(pDoc->m_fileOut [j - 1] [i - 1] - pDoc->m_fileOut [j] [i] == 255)
				bmpPer [j] [i] = 100;

			else if (pDoc->m_fileOut [j] [i] - pDoc->m_fileOut [j - 1] [i + 1] == 255)    // 右上
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
		s.Format ("  区域 : %d\n\n  面积 : %d\n  周长 : %d\n  圆形度 : %f\n ", m_shapefeature [i].index, m_shapefeature [i].area, m_shapefeature [i].arealength, (double) (m_shapefeature [i].area * 4 * 3.1415926) / (double) (m_shapefeature [i].arealength * m_shapefeature [i].arealength));
		::MessageBox (NULL, s, "区域特征提取结果", MB_OK);
	    strOutput [i] += s;
	}

    delete [] strOutput;
	delete [] m_shapefeature;

    return;
}


void CIProView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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
			str.Format ("晴空线的方程为：Y = %fX + (%f)", m_fCSEquation_b, m_fCSEquation_a);
			MessageBox (str);
			MessageBox ("晴空线计算完毕，请单击‘计算像素的HOT值’菜单项计算影像的HOT！");
		}
	}

	CScrollView::OnLButtonUp(nFlags, point);
}


void CIProView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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
	// TODO: 在此添加命令处理程序代码
	m_bWander = TRUE;
	m_bCSLine = FALSE;
}



void CIProView::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bWander == TRUE && m_bWanderIn == TRUE)
	{
		m_bWanderIn = FALSE;
	}

	CScrollView::OnNcMouseMove(nHitTest, point);
}


void CIProView::OnTellBinary()
{
	// TODO: 在此添加命令处理程序代码
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);

	// 获取图像宽和高
	int nWidth = pDoc->m_fileOut.m_Cols;
	int nHeight = pDoc->m_fileOut.m_Rows;;

    // 阈值nThreshold
	int nThreshold;

	// 定义并初始化灰度统计数组his[256]
	int his [256];
	memset (his, 0, sizeof(his));

    // lS,lS1,LS2 分别代表像素总数，类1像素总数，类2像素总数
	int lS, lS1, lS2;

	// lP, lP1分别代表总的质量矩和类1质量矩
	double lP, lP1;

	// AverValue1, Avervalue2分别代表类1灰度均值和类2灰度均值
	double AverValue1, AverValue2;

	// Disper1, Disper2, ClassinDisper, ClassoutDisper， max表示类1方差，类2方差，类内方差，类间方差，max表示类间最大差距
	double Disper1, Disper2, ClassinDisper, ClassoutDisper, max;

	// 灰度最大最小值
	int Gmin = 255, Gmax = 0;

    // 循环变量
	int i, j, k;
		
	//统计出各个灰度值得像素数
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

	// 赋初值
	lP=0.0;
	lS=0;
	k = 0;
	
	if (Gmin == 0) 
		Gmin++;

	//计算总的质量矩lP和像素总数lS
	for (k = Gmin; k < Gmax; k++)
	{
		lP += (double) k * (double) his [k];
		lS += his [k];
	}

	// 赋初值和声明局部变量，为后续计算做准备
	max=0.0;
	lS1 = 0;
	lS2 = 0;
	lP1 = 0.0;
	Disper1 = 0.0;
	Disper2 = 0.0;
	ClassinDisper = 0.0;
	ClassoutDisper = 0.0;
	double ratio = 0.0;

	// 求阈值
	for (k = Gmin; k < Gmax; k++)
	{
		lS1 += his [k];

		if(!lS1)
			continue;
		
		// 计算类2像素总数
		lS2 = lS - lS1; 		
		if (lS2 == 0)
			break;

		// 计算类1质量矩
		lP1 += (double) k * his [k];

		// 计算类1均值
		AverValue1 = lP1 / lS1;

		// 计算类1间方差
		for (int n = Gmin;n<=k;n++)
			Disper1 += ((n - AverValue1) * (n - AverValue1) * his [n]);

		// 计算类2间均值
		AverValue2 = (lP - lP1) / lS2;

		// 计算类2间方差
		for (int m = k+1; m <= Gmax; m++)
			Disper2 += ((m - AverValue2) * (m - AverValue2) * his [m]);

		// 计算类内方差
		ClassinDisper = Disper1 + Disper2;

		// 计算类间方差
		ClassoutDisper = (double) lS1 * (double) lS2 * (AverValue1 - AverValue2) * (AverValue1 - AverValue2);

		// 类间方差与类内方差比值
		if (ClassinDisper != 0) 
			ratio = ClassoutDisper / ClassinDisper;

		// 获取分割阈值nThreshold
        if (ratio > max)
	    {
			max = ratio;
		    nThreshold = k;  
	    } 
	}
   
	CString strInfo;
    strInfo.Format ("阈值=%d\n", nThreshold);
	::MessageBox (NULL, strInfo, "阈值结果", MB_OK);

	// 二值化图像
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
	// TODO: 在此添加命令处理程序代码
	MessageBox ("请确认图像是否读入，若已读入请利用鼠标框选图像中的晴空区域！框选过程中请不要使矩形超出图像范围，否则会出现异常或计算错误！");
	m_bWander = FALSE;
	m_bCSLine = TRUE;
}


void CIProView::OnCalculateHOT()
{
	// TODO: 在此添加命令处理程序代码

	// 获取文档指针
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);

	int nPixelNum;    // 影像像素总数
	int nWidth = pDoc->m_fileOut.m_Cols;     // 影像宽度
	int nHeight = pDoc->m_fileOut.m_Rows;    // 影像高度     
	nPixelNum = nWidth * nHeight;
	m_pHOT = new double [nPixelNum]; 	// 为指针数组动态分配内存
	m_pHOTColor = new double [nPixelNum];
	int nRows, nCols;                   // 行、列
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

	MessageBox ("影像HOT值计算完毕，请单击‘HOT图像’菜单项查看HOT图像！");
}

void CIProView::OnDrawHOT()
{
	// TODO: 在此添加命令处理程序代码
	
	// 获取文档指针
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);

	MessageBox ("请在进行此操作前保证源图像所在视图为当前活动视图，否则接下来可能会出现内存访问错误！");

	// 创建并初始化HOT位图
	CBmpFile bmp;
	bmp.CreateBmp (pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows, 1);

	// 按0-255宽度缩放后的HOT值
	BYTE nGray;

	// 将HOT值赋给bmp图像
	for (int i = 0; i < pDoc->m_fileOut.m_Cols * pDoc->m_fileOut.m_Rows; i++)
	{
		nGray = (BYTE) (((m_pHOT [i] - m_fHotMin) / (m_fHotMax - m_fHotMin)) * 255.0);
		bmp.m_pImgDat [i] = nGray * 2;
	}

	// 打开新视图，查看HOT图像
	CMainFrame* pFrame = (CMainFrame*) (AfxGetApp ()->m_pMainWnd);

	// 发送一个新建文件的消息，创建一个新的文档-视图
	pFrame->SendMessage (WM_COMMAND, ID_FILE_NEW);

	// 获取新建视图指针
	CIProView* pView = (CIProView*) pFrame->MDIGetActive ()->GetActiveView ();

	// 获取新关联的新的文档类指针
	CIProDoc* pDocNew = pView->GetDocument ();

	// 新文档获取HOT图像
	pDocNew->m_fileOut = bmp;
	pDocNew->m_bfile = TRUE;

	// 刷新显示
	pDocNew->UpdateAllViews (pView);
}


void CIProView::OnDrawColor()
{
	// TODO: 在此添加命令处理程序代码
	
	// 获取文档指针
	CIProDoc *pDoc = GetDocument();
	ASSERT_VALID (pDoc);

	MessageBox ("请在进行此操作前保证源图像所在视图为当前活动视图，否则接下来可能会出现内存访问错误！");

	// 创建并初始化HOT位图
	CBmpFile bmp;
	bmp.CreateBmp (pDoc->m_fileOut.m_Cols, pDoc->m_fileOut.m_Rows, 3);

	double fRegion = 0.0;
	fRegion = (m_fHotMax - m_fHotMin) / 5;

	// 将HOT值赋给bmp图像
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

	// 打开新视图，查看HOT图像
	CMainFrame* pFrame = (CMainFrame*) (AfxGetApp ()->m_pMainWnd);

	// 发送一个新建文件的消息，创建一个新的文档-视图
	pFrame->SendMessage (WM_COMMAND, ID_FILE_NEW);

	// 获取新建视图指针
	CIProView* pView = (CIProView*) pFrame->MDIGetActive ()->GetActiveView ();

	// 获取新关联的新的文档类指针
	CIProDoc* pDocNew = pView->GetDocument ();

	// 新文档获取HOT图像
	pDocNew->m_fileOut = bmp;
	pDocNew->m_bfile = TRUE;

	// 刷新显示
	pDocNew->UpdateAllViews (pView);
}

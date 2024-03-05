// Raw.cpp: implementation of the CRaw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Raw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaw::CRaw()
{
	//直方图数组清0
	for(int i=0;i<256;i++)
		m_Histogram[i]=0;
	m_pBuffOut = NULL;
}
BOOL CRaw::ReadFromFile(CString strFilename,int nHeight,int nWidth)
{
	CFile file;   
	CFileException ex;   
	CString strError1= "文件打开错误!";   
	CString strError2= "非正确的raw格式文件!";   
	if (!file.Open(strFilename, CFile::modeRead, &ex))
	{   
		ex.ReportError();   
		return FALSE;   
	}   
	m_sizeImage.cy= nHeight;   
	m_sizeImage.cx= nWidth;   
	m_nHeight = nHeight;   
	m_nWidth = nWidth;  
	m_pBuff= new BYTE[nHeight*nWidth];   
	if (file.Read(m_pBuff, nHeight*nWidth*sizeof(BYTE))!=(nHeight*nWidth))
	{   
		AfxMessageBox(strError2, MB_OK|MB_ICONEXCLAMATION);   
		file.Close();   
		return FALSE;   
	}   
	file.Close();   
	return TRUE;   
}
void CRaw::computeHistGray()
{
	//只处理灰度图像
	if(m_pBuff==NULL)
		return;
	//循环变量
	int i,j;
	//中间变量
	int temp=0;
	//每行像素所占字节数
	//统计灰度直方图
	for(i=0;i<m_nHeight;i++){
		for(j=0;j<m_nWidth;j++){
			temp=*(m_pBuff+i*m_nWidth+j);
			m_Histogram[temp]++;
		}
	}
}
CRaw::~CRaw()
{
	if (m_pBuff!=NULL)   
		delete m_pBuff;   
	m_pBuff = NULL;  	
}
void CRaw::Empty()   
{   
	if (m_pBuff!=NULL)   
		delete m_pBuff;   
	m_pBuff = NULL;   
}
BOOL CRaw::Write(LPCTSTR lpszPathName)
{
	//写模式打开文件
	CFile file;
	if (!file.Open(lpszPathName, CFile::modeCreate | CFile::modeReadWrite 
		| CFile::shareExclusive))
		return FALSE;
	int nHeight,nWidth;
	nHeight = m_nHeight;
	nWidth = m_nWidth;
	file.Write(m_pBuffOut,nHeight*nWidth*sizeof(BYTE));
	file.Close();
	return TRUE;	
}
void CRaw::EdgeByAnyMask(int *mask,int maskW,int maskH,int masksize)
{
	//释放m_pBuffOut指向的图像数据空间
	if(m_pBuffOut!=NULL){
		delete []m_pBuffOut;
		m_pBuffOut=NULL;
	}
	//申请输出图像缓冲区
	//每行像素字节数
	m_pBuffOut=new BYTE[m_nWidth*m_nHeight];
	
	//模板卷积函数调用
	TemplateEdge(m_pBuff, m_nWidth, m_nHeight,mask, maskW, maskH, masksize,m_pBuffOut);	
}
void CRaw::TemplateEdge(BYTE* imgIn, int width,int height,int *mask,int maskW,int maskH,int masksize,BYTE* imgOut)
{
	//循环变量，图像的坐标
	int i,j;
	//循环变量，用于模板卷积运算
	int  c, r;
	//中间变量
	int sum;
	//模板卷积运算，边缘像素不处理
	for (i=0;i<height;i++)
	{
		for (j=0;j<width;j++)
		{
			*(imgOut+i*width+j) = 255;
		}
	}
	for(i=maskH/2;i<height-maskH/2; i++)
	{
		for(j=maskW/2; j<width-maskW/2; j++)
		{
			//卷积求和
			sum=0;
			for(r=-maskH/2;r<=maskH/2;r++)
			{
				for(c=-maskW/2;c<=maskW/2;c++)
				{
					sum += (*(mask+(r+maskH/2)*maskW+(c+maskW/2)))*(*(imgIn+(i+r)*width+(j+c)));
				}
			}
			//取绝对值
			sum = sum/masksize;
			sum=abs(sum);
			if(sum>255)
				sum=255;
			else if(sum<0)
				sum=0;
			*(imgOut+i*width+j)=sum;
		}
	}
}

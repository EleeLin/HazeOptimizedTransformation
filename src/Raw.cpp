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
	//ֱ��ͼ������0
	for(int i=0;i<256;i++)
		m_Histogram[i]=0;
	m_pBuffOut = NULL;
}
BOOL CRaw::ReadFromFile(CString strFilename,int nHeight,int nWidth)
{
	CFile file;   
	CFileException ex;   
	CString strError1= "�ļ��򿪴���!";   
	CString strError2= "����ȷ��raw��ʽ�ļ�!";   
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
	//ֻ����Ҷ�ͼ��
	if(m_pBuff==NULL)
		return;
	//ѭ������
	int i,j;
	//�м����
	int temp=0;
	//ÿ��������ռ�ֽ���
	//ͳ�ƻҶ�ֱ��ͼ
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
	//дģʽ���ļ�
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
	//�ͷ�m_pBuffOutָ���ͼ�����ݿռ�
	if(m_pBuffOut!=NULL){
		delete []m_pBuffOut;
		m_pBuffOut=NULL;
	}
	//�������ͼ�񻺳���
	//ÿ�������ֽ���
	m_pBuffOut=new BYTE[m_nWidth*m_nHeight];
	
	//ģ������������
	TemplateEdge(m_pBuff, m_nWidth, m_nHeight,mask, maskW, maskH, masksize,m_pBuffOut);	
}
void CRaw::TemplateEdge(BYTE* imgIn, int width,int height,int *mask,int maskW,int maskH,int masksize,BYTE* imgOut)
{
	//ѭ��������ͼ�������
	int i,j;
	//ѭ������������ģ��������
	int  c, r;
	//�м����
	int sum;
	//ģ�������㣬��Ե���ز�����
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
			//������
			sum=0;
			for(r=-maskH/2;r<=maskH/2;r++)
			{
				for(c=-maskW/2;c<=maskW/2;c++)
				{
					sum += (*(mask+(r+maskH/2)*maskW+(c+maskW/2)))*(*(imgIn+(i+r)*width+(j+c)));
				}
			}
			//ȡ����ֵ
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

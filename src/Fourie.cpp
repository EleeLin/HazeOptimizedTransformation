#include "stdafx.h"
#include "Fourie.h"
#include <math.h>
#include <complex>
using namespace std;


CFourie::CFourie(void)
{
	m_nTransHeight = 0;
	m_nTransWidth = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_pCTData = NULL;
    m_pCFData = NULL;
}

void CFourie::GetFWH (int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	double dTmpOne,dTmpTwo;

	dTmpOne=log(m_nWidth)/log(2);     //������и���Ҷ�任�Ŀ�ȣ�2���������ݣ�
    dTmpTwo=ceil(dTmpOne);
    dTmpTwo=pow(2,dTmpTwo);
    m_nTransWidth=(int)dTmpTwo;

	dTmpOne=log(m_nHeight)/log(2);    //������и���Ҷ�任�ĸ߶ȣ�2���������ݣ�
    dTmpTwo=ceil(dTmpOne);
    dTmpTwo=pow(2,dTmpTwo);
    m_nTransHeight=(int)dTmpTwo;

	m_pCTData = NULL;
    m_pCFData = NULL;

	m_pCTData = new complex <double> [m_nTransWidth * m_nTransHeight];
	m_pCFData = new complex <double> [m_nTransWidth * m_nTransHeight];
}

CFourie::~CFourie(void)
{
}

void CFourie::Fourie (complex <double>* pCTData, int nWidth, int nHeight, complex <double>* pCFData)
{
/*	int x, y;
	int u, v;
	double PI = 3.1415926535;

	for(y = 0; y < m_nTransHeight; y++)
	{
		for (x = 0; x < m_nTransWidth; x++)
		{
			pCFData [y * m_nTransWidth + x] = complex <double> (0,0);
		}
	}

	for (u = 0; u < m_nTransHeight; u++)
	{
		for (v = 0; v < m_nTransWidth; v++)
		{
			for (x = 0; x < m_nTransHeight; x++)
			{
				for (y = 0; y < m_nTransWidth; y++)
				{
					pCFData [m_nTransHeight * u + v] += pCTData [m_nTransHeight * x + y] * complex <double> (cos (2.0 * PI * ((double)u * (double)x / (double)m_nTransWidth + (double)v * (double)y / (double)m_nTransHeight)),- sin (2.0 * PI * ((double)u * (double)x / (double)m_nTransWidth + (double)v * (double)y / (double)m_nTransHeight)));
				}
			}
		}
	}

	for (y = 0; y < m_nTransHeight; y++)
	{
		for (x = 0; x < m_nTransWidth; x++)
		{
			pCFData [y * m_nTransHeight + x] /= m_nTransHeight;
		}
	}*/
	
	int x,y;                        //ѭ�����Ʊ���
 
    int nXLev,nYLev;                //x,y�����У������ϵĵ�������
    nXLev=(int)(log(m_nTransWidth)/log(2)+0.5);      //����x,y�����У������ϵĵ�������
    nYLev=(int)(log(m_nTransHeight)/log(2)+0.5);
    for (y=0;y<m_nTransHeight;y++)
    {
		FastFourie(&pCTData[m_nTransWidth*y],&pCFData[m_nTransWidth*y],nXLev);    //x������п��ٸ���Ҷ�任
    }
    
    //pCFData��Ŀǰ�洢��pCTData�����б任�Ľ��
    //Ϊ��ֱ������FFT_1D,��Ҫ��pCFData�Ķ�ά����ת�ã���һ������FFT_1D����
    //����Ҷ�б任��ʵ�����൱�ڶ��н��и���Ҷ�任��
    for (y=0;y<m_nTransHeight;y++)
    {
		for (x=0;x<m_nTransWidth;x++)
        {
			pCTData[m_nTransHeight*x+y]=pCFData[m_nTransWidth*y+x];
        }
    }

    for(x=0;x<m_nTransWidth;x++)
    {
		//��x������п��ٸ���Ҷ�任��ʵ�����൱�ڶ�ԭ����ͼ�����ݽ����з���ĸ���Ҷ�任
        FastFourie(&pCTData[x*m_nTransHeight],&pCFData[x*m_nTransHeight],nYLev);
    }
 
	//pCFData��Ŀǰ�洢��pCTData�����б任�Ľ������Ϊ�˷����з���ĸ���Ҷ�任�����������ת�ã����ڰѽ��ת�û���
    for (y=0;y<m_nTransHeight;y++)
    {
		for (x=0;x<m_nTransWidth;x++)
        {
			pCTData[m_nTransWidth*y+x]=pCFData[m_nTransHeight*x+y];
        }
    }
   
	memcpy(pCTData,pCFData,sizeof(complex<double>)*m_nTransHeight*m_nTransWidth);
 }
 

void CFourie::FastFourie (complex <double>* pCTData, complex <double>* pCFData, int nLevel)
{
	//ѭ�����Ʊ���
    int i,j,k;
    int nCount=0; 
    double PI=3.1415926;

    //����Ҷ�任����
    nCount=(int)pow(2,nLevel);
    
	//���㸵��Ҷ�任����
    int nBtFlyLen;

    //ĳһ������
    nBtFlyLen=0;

    //�任ϵ���ĽǶ�=2*PI*i/nCount
    double dAngle;
    complex<double>*pCW;


    //�����ڴ棬�洢����Ҷ�任��Ҫ��ϵ����
    pCW=new complex<double>[nCount/2];

    //���㸵��Ҷ�任��ϵ��
    for (i=0;i<nCount/2;i++)
    {
		dAngle=-2*PI*i/nCount;
        pCW[i]=complex<double>(cos(dAngle),sin(dAngle));
    }

    //�任��Ҫ�Ĺ����ռ�
    complex<double> *pCWork1,*pCWork2;

    //���乤���ռ�
    pCWork1=new complex<double>[nCount];
    pCWork2=new complex<double>[nCount];

    //��ʱ����
    complex<double> *pCTemp;

    //��ʼ����д������
    memcpy(pCWork1,pCTData,sizeof(complex<double>)*nCount);

    //��ʱ����
    int nInter;
    nInter=0;


    //�����㷨���п��ٸ���Ҷ�任
    for (k=0;k<nLevel;k++)
    {
		for (j=0;j<(int)pow(2,k);j++)
        {
			nBtFlyLen=(int)pow(2,(nLevel-k));
            
			//�������ţ���Ȩ����
            for (i=0;i<nBtFlyLen/2;i++)
            {
				nInter=j*nBtFlyLen;
                pCWork2[i+nInter]=pCWork1[i+nInter]+pCWork1[i+nInter+nBtFlyLen/2];
                pCWork2[i+nInter+nBtFlyLen/2]=(pCWork1[i+nInter]-pCWork1[i+nInter+nBtFlyLen/2])*pCW[(int)(i*pow(2,k))];
            }
        }

		//����pCWork1��pCWork2������
        pCTemp=pCWork1;
        pCWork1=pCWork2;
        pCWork2=pCTemp;
  }

   //�������� 
   for (j=0;j<nCount;j++)
   {
	   nInter=0;
       for (i=0;i<nLevel;i++)
       {
		   if(j&(1<<i))nInter+=1<<(nLevel-i-1);
       }
       pCFData[j]=pCWork1[nInter];
   }
 
  //�ͷ��ڴ�ռ�
  delete pCW;
  delete pCWork1;
  delete pCWork2;
  pCW=NULL;
  pCWork1=NULL;
  pCWork2=NULL;
}

void CFourie::IFourie (complex <double>* pCFData, complex <double>* pCTData, int nWidth, int nHeight)
{
/*	int x, y;
	int u, v;
	double PI = 3.1415926535;

	for (x = 0; x < m_nTransHeight; x++)
	{
		for (y = 0; y < m_nTransWidth; y++)
		{
			for (u = 0; u < m_nTransHeight; u++)
			{
				for (v = 0; v < m_nTransWidth; v++)
				{
					pCTData [m_nTransHeight * x + y] += pCFData [m_nTransHeight * u + v] * complex <double> (cos (2.0 * PI * ((double)u * (double)x / (double)m_nTransWidth + (double)v * (double)y / (double)m_nTransHeight)), sin (2.0 * PI * ((double)u * (double)x / (double)m_nTransWidth + (double)v * (double)y / (double)m_nTransHeight)));
				}
			}
		}
	}

	for (y = 0; y < m_nTransHeight; y++)
	{
		for (x = 0; x < m_nTransWidth; x++)
		{
			pCTData [y * m_nTransHeight + x] /= m_nTransWidth;
		}
	}*/
	//ѭ�����Ʊ���
    int x;
    int y;
 
   //���乤����Ҫ���ڴ�ռ�
   complex<double>*pCWork=new complex<double>[m_nTransWidth*m_nTransHeight];
    
   //��ʱ����
   complex<double>*pCTmp;
   
   //Ϊ�����ø���Ҷ���任�����԰Ѹ���ҶƵ�������ȡ����
   //Ȼ��ֱ���������任�����������Ǹ���Ҷ���任����Ĺ���
   for (y=0;y<m_nTransHeight;y++)
   {
	   for (x=0;x<m_nTransWidth;x++)
       {
		   pCTmp=&pCFData[m_nTransWidth*y+x];
           pCWork[m_nTransWidth*y+x]=complex<double>(pCTmp->real(),-pCTmp->imag());
       }
    }
 
   //���ø���Ҷ���任
   Fourie(pCWork,nWidth,nHeight,pCTData);
 
   //��ʱ���Ĺ��������ս��
   //���ݸ���Ҷ�任ԭ�����������ķ�����õĽ����ʵ�ʵ�ʱ������
   //���һ��ϵ��
   for (y=0;y<m_nTransHeight;y++)
   {
	   for(x=0;x<m_nTransWidth;x++)
       {
		   pCTmp=&pCTData[m_nTransWidth*y+x];
           pCTData[m_nTransWidth*y+x]=complex<double>(pCTmp->real()/(m_nTransWidth*m_nTransHeight),-pCTmp->imag()/(m_nTransWidth*m_nTransHeight));
       }
   }
   
   delete pCWork;
   pCWork=NULL;
}

void CFourie::ButterWorthLowPass (LPBYTE lpImage, int nWidth, int nHeight, int nRadius)
{
/*	int x, y;
	double H;
	for (y = 0; y < m_nTransHeight; y++)
	{
		for (x = 0; x < m_nTransWidth; x++)
		{
			H = (double) (y * y + x * x);
			H = H / (nRadius * nRadius);
			H = 1 / (1 + H);
			pCFData [y * m_nTransWidth + x] = complex <double> (H * (pCFData [y * m_nTransWidth + x].real ()), H * (pCFData [y * m_nTransWidth + x].imag ()));
		}
    }*/
	// ����ѭ������
	int x, y;

	// ButterWorth �˲�ϵ��
	double H;

	double dReal;
    double dImag;

	// ͼ������ֵ
	unsigned char unchValue;

	// ָ��ʱ�����ݵ�ָ��
	complex <double> *pCTData;

	// ָ��Ƶ�����ݵ�ָ��
	complex <double> *pCFData;

	// �����ڴ�
	pCTData = new complex <double> [m_nTransWidth * m_nTransHeight];
	pCFData = new complex <double> [m_nTransWidth * m_nTransHeight];

	// ��ʼ��
	// ͼ�����ݵĸߺͿ�һ����2���������ݣ�����pCTData
	// ��һ����������Ҫ��0
	for(y = 0; y < m_nTransHeight; y++)
	{
		for (x = 0; x < m_nTransWidth; x++)
		{
			pCTData [y * m_nTransWidth + x] = complex <double> (0,0);
		}
	}

	// ��ͼ�����ݴ���pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			unchValue = lpImage [y * nWidth + x];
			pCTData [y * m_nTransWidth + x] = complex <double> (unchValue, 0);
		}
	}

	// ����Ҷ���任
	Fourie (pCTData, nWidth, nHeight, pCFData);

	// ���濪ʼʵʩButterWorth��ͨ�˲�
	for (y = 0; y < m_nTransHeight; y++)
	{
		for (x = 0; x < m_nTransWidth; x++)
		{
			H = (double) (y * y + x * x);
			H = H / (nRadius * nRadius);
			H = 1 / (1 + H);
			pCFData [y * m_nTransWidth + x] = complex <double> (H * (pCFData [y * m_nTransWidth + x].real ()), H * (pCFData [y * m_nTransWidth + x].imag ()));
		}
    }
	
	// �Ծ���ButterWorth��ͨ�˲���ͼ����з��任
	IFourie (pCFData, pCTData, nWidth, nHeight);

	// �ѷ��任�����ݴ���lpImage
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			dReal = pCTData [y * m_nTransWidth + x].real ();
			dImag = pCTData [y * m_nTransHeight + x].imag ();
			unchValue = (unsigned char) max (0, min (255, sqrt (dReal * dReal + dImag * dImag)));
			lpImage [y * nWidth + x] = unchValue;
		}
	}

	// �ͷ��ڴ�
	delete pCTData;
	delete pCFData;
	pCTData = NULL;
	pCTData = NULL;
}

void CFourie::ButterWorthHighPass (LPBYTE lpImage, int nWidth, int nHeight, int nRadius)
{
/*	int x, y;
	double H;
	for (y = 0; y < m_nTransHeight; y++)
	{
		for (x = 0; x < m_nTransWidth; x++)
		{
			H = (double) (y * y + x * x);
			H = (nRadius * nRadius) / H;
			H = 1 / (1 + H);
			pCFData [y * m_nTransWidth + x] = complex <double> (H * (pCFData [y * m_nTransWidth + x].real ()), H * (pCFData [y * m_nTransWidth + x].imag ()));
		}
    }*/
	// ����ѭ������
	int x, y;

	// ButterWorth �˲�ϵ��
	double H;

	double dReal;
    double dImag;

	// ͼ������ֵ
	unsigned char unchValue;

	// ָ��ʱ�����ݵ�ָ��
	complex <double> *pCTData;

	// ָ��Ƶ�����ݵ�ָ��
	complex <double> *pCFData;

	// �����ڴ�
	pCTData = new complex <double> [m_nTransWidth * m_nTransHeight];
	pCFData = new complex <double> [m_nTransWidth * m_nTransHeight];

	// ��ʼ��
	// ͼ�����ݵĸߺͿ�һ����2���������ݣ�����pCTData
	// ��һ����������Ҫ��0
	for(y = 0; y < m_nTransHeight; y++)
	{
		for (x = 0; x < m_nTransWidth; x++)
		{
			pCTData [y * m_nTransWidth + x] = complex <double> (0,0);
		}
	}

	// ��ͼ�����ݴ���pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			unchValue = lpImage [y * nWidth + x];
			pCTData [y * m_nTransWidth + x] = complex <double> (unchValue, 0);
		}
	}

	// ����Ҷ���任
	Fourie (pCTData, nWidth, nHeight, pCFData);

	// ���濪ʼʵʩButterWorth��ͨ�˲�
	for (y = 0; y < m_nTransHeight; y++)
	{
		for (x = 0; x < m_nTransWidth; x++)
		{
			H = (double) (y * y + x * x);
			H = (nRadius * nRadius) / H;
			H = 1 / (1 + H);
			pCFData [y * m_nTransWidth + x] = complex <double> (H * (pCFData [y * m_nTransWidth + x].real ()), H * (pCFData [y * m_nTransWidth + x].imag ()));
		}
    }
	
	// �Ծ���ButterWorth��ͨ�˲���ͼ����з��任
	IFourie (pCFData, pCTData, nWidth, nHeight);

	// �ѷ��任�����ݴ���lpImage
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
     		dReal = pCTData [y * m_nTransWidth + x].real ();
			dImag = pCTData [y * m_nTransHeight + x].imag ();
			unchValue = (unsigned char) max (0, min (255, sqrt (dReal * dReal + dImag * dImag) + 100));
			lpImage [y * nWidth + x] = unchValue;
		}
	}

	// �ͷ��ڴ�
	delete pCTData;
	delete pCFData;
	pCTData = NULL;
	pCTData = NULL;
}

void CFourie::InPutData (complex <double> *pCTData, int nWidth, int nHeight, LPBYTE lpImage)
{
	// ѭ������
	int x, y;	

	// ͼ������ֵ
	unsigned char unchValue;

	// ��ʼ��
	// ͼ�����ݵĸߺͿ�һ����2���������ݣ�����pCTData
	// ��һ����������Ҫ��0
	for(y = 0; y < m_nTransHeight; y++)
	{
		for (x = 0; x < m_nTransWidth; x++)
		{
			pCTData [y * m_nTransWidth + x] = complex <double> (0,0);
		}
	}

	// ��ͼ�����ݴ���pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			unchValue = lpImage [y * nWidth + x];
			pCTData [y * m_nTransWidth + x] = complex <double> (unchValue, 0);
			pCTData [y * m_nTransWidth + x] *= pow (-1, x + y);
		}
	}
}

void CFourie::OutPutDataDFT (complex <double> *pCTData, int nWidth, int nHeight, LPBYTE lpImage)
{
	// ѭ������
	int x, y;
	
	// ͼ������ֵ
	unsigned char unchValue;

	double dReal;
	double dImag;

	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			dReal = pCTData [y * m_nTransWidth + x].real ();
			dImag = pCTData [y * m_nTransHeight + x].imag ();
     		unchValue = (unsigned char) max (0, min (255, sqrt (dReal * dReal + dImag * dImag)/ 500 /*+ 100*/));
			lpImage [y  * nWidth + x] = unchValue;
		}
	}
}

void CFourie::OutPutDataIFT (complex <double> *pCTData, int nWidth, int nHeight, LPBYTE lpImage)
{
	// ѭ������
	int x, y;
	
	// ͼ������ֵ
	unsigned char unchValue;

	double dReal;
	double dImag;

	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			dReal = pCTData [y * m_nTransWidth + x].real ();
			dImag = pCTData [y * m_nTransHeight + x].imag ();
     		unchValue = (unsigned char) max (0, min (255, sqrt (dReal * dReal + dImag * dImag)));
			lpImage [y  * nWidth + x] = unchValue;
		}
	}
}
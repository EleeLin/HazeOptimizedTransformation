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

	dTmpOne=log(m_nWidth)/log(2);     //计算进行傅里叶变换的宽度（2的整数次幂）
    dTmpTwo=ceil(dTmpOne);
    dTmpTwo=pow(2,dTmpTwo);
    m_nTransWidth=(int)dTmpTwo;

	dTmpOne=log(m_nHeight)/log(2);    //计算进行傅里叶变换的高度（2的整数次幂）
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
	
	int x,y;                        //循环控制变量
 
    int nXLev,nYLev;                //x,y（行列）方向上的迭代次数
    nXLev=(int)(log(m_nTransWidth)/log(2)+0.5);      //计算x,y（行列）方向上的迭代次数
    nYLev=(int)(log(m_nTransHeight)/log(2)+0.5);
    for (y=0;y<m_nTransHeight;y++)
    {
		FastFourie(&pCTData[m_nTransWidth*y],&pCFData[m_nTransWidth*y],nXLev);    //x方向进行快速傅里叶变换
    }
    
    //pCFData中目前存储了pCTData经过行变换的结果
    //为了直接利用FFT_1D,需要把pCFData的二维数据转置，再一次利用FFT_1D进行
    //傅里叶行变换（实际上相当于对列进行傅里叶变换）
    for (y=0;y<m_nTransHeight;y++)
    {
		for (x=0;x<m_nTransWidth;x++)
        {
			pCTData[m_nTransHeight*x+y]=pCFData[m_nTransWidth*y+x];
        }
    }

    for(x=0;x<m_nTransWidth;x++)
    {
		//对x方向进行快速傅里叶变换，实际上相当于对原来的图像数据进行列方向的傅里叶变换
        FastFourie(&pCTData[x*m_nTransHeight],&pCFData[x*m_nTransHeight],nYLev);
    }
 
	//pCFData中目前存储了pCTData经过行变换的结果，但为了方便列方向的傅里叶变换，对其进行了转置，现在把结果转置回来
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
	//循环控制变量
    int i,j,k;
    int nCount=0; 
    double PI=3.1415926;

    //傅里叶变换点数
    nCount=(int)pow(2,nLevel);
    
	//计算傅里叶变换点数
    int nBtFlyLen;

    //某一级长度
    nBtFlyLen=0;

    //变换系数的角度=2*PI*i/nCount
    double dAngle;
    complex<double>*pCW;


    //分配内存，存储傅里叶变换需要的系数表
    pCW=new complex<double>[nCount/2];

    //计算傅里叶变换的系数
    for (i=0;i<nCount/2;i++)
    {
		dAngle=-2*PI*i/nCount;
        pCW[i]=complex<double>(cos(dAngle),sin(dAngle));
    }

    //变换需要的工作空间
    complex<double> *pCWork1,*pCWork2;

    //分配工作空间
    pCWork1=new complex<double>[nCount];
    pCWork2=new complex<double>[nCount];

    //临时变量
    complex<double> *pCTemp;

    //初始化，写入数据
    memcpy(pCWork1,pCTData,sizeof(complex<double>)*nCount);

    //临时变量
    int nInter;
    nInter=0;


    //蝶形算法进行快速傅里叶变换
    for (k=0;k<nLevel;k++)
    {
		for (j=0;j<(int)pow(2,k);j++)
        {
			nBtFlyLen=(int)pow(2,(nLevel-k));
            
			//倒序重排，加权计算
            for (i=0;i<nBtFlyLen/2;i++)
            {
				nInter=j*nBtFlyLen;
                pCWork2[i+nInter]=pCWork1[i+nInter]+pCWork1[i+nInter+nBtFlyLen/2];
                pCWork2[i+nInter+nBtFlyLen/2]=(pCWork1[i+nInter]-pCWork1[i+nInter+nBtFlyLen/2])*pCW[(int)(i*pow(2,k))];
            }
        }

		//交换pCWork1和pCWork2的数据
        pCTemp=pCWork1;
        pCWork1=pCWork2;
        pCWork2=pCTemp;
  }

   //重新排序 
   for (j=0;j<nCount;j++)
   {
	   nInter=0;
       for (i=0;i<nLevel;i++)
       {
		   if(j&(1<<i))nInter+=1<<(nLevel-i-1);
       }
       pCFData[j]=pCWork1[nInter];
   }
 
  //释放内存空间
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
	//循环控制变量
    int x;
    int y;
 
   //分配工作需要的内存空间
   complex<double>*pCWork=new complex<double>[m_nTransWidth*m_nTransHeight];
    
   //临时变量
   complex<double>*pCTmp;
   
   //为了利用傅里叶正变换，可以把傅里叶频域的数据取共轭
   //然后直接利用正变换，输出结果就是傅里叶反变换结果的共轭
   for (y=0;y<m_nTransHeight;y++)
   {
	   for (x=0;x<m_nTransWidth;x++)
       {
		   pCTmp=&pCFData[m_nTransWidth*y+x];
           pCWork[m_nTransWidth*y+x]=complex<double>(pCTmp->real(),-pCTmp->imag());
       }
    }
 
   //调用傅里叶正变换
   Fourie(pCWork,nWidth,nHeight,pCTData);
 
   //求时域点的共轭，求得最终结果
   //根据傅里叶变换原理，利用这样的方法求得的结果和实际的时域数据
   //相差一个系数
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
	// 控制循环变量
	int x, y;

	// ButterWorth 滤波系数
	double H;

	double dReal;
    double dImag;

	// 图像像素值
	unsigned char unchValue;

	// 指向时域数据的指针
	complex <double> *pCTData;

	// 指向频域数据的指针
	complex <double> *pCFData;

	// 分配内存
	pCTData = new complex <double> [m_nTransWidth * m_nTransHeight];
	pCFData = new complex <double> [m_nTransWidth * m_nTransHeight];

	// 初始化
	// 图像数据的高和宽不一定是2的整数次幂，所以pCTData
	// 有一部分数据需要补0
	for(y = 0; y < m_nTransHeight; y++)
	{
		for (x = 0; x < m_nTransWidth; x++)
		{
			pCTData [y * m_nTransWidth + x] = complex <double> (0,0);
		}
	}

	// 把图像数据传给pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			unchValue = lpImage [y * nWidth + x];
			pCTData [y * m_nTransWidth + x] = complex <double> (unchValue, 0);
		}
	}

	// 傅里叶正变换
	Fourie (pCTData, nWidth, nHeight, pCFData);

	// 下面开始实施ButterWorth高通滤波
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
	
	// 对经过ButterWorth高通滤波的图像进行反变换
	IFourie (pCFData, pCTData, nWidth, nHeight);

	// 把反变换的数据传给lpImage
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

	// 释放内存
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
	// 控制循环变量
	int x, y;

	// ButterWorth 滤波系数
	double H;

	double dReal;
    double dImag;

	// 图像像素值
	unsigned char unchValue;

	// 指向时域数据的指针
	complex <double> *pCTData;

	// 指向频域数据的指针
	complex <double> *pCFData;

	// 分配内存
	pCTData = new complex <double> [m_nTransWidth * m_nTransHeight];
	pCFData = new complex <double> [m_nTransWidth * m_nTransHeight];

	// 初始化
	// 图像数据的高和宽不一定是2的整数次幂，所以pCTData
	// 有一部分数据需要补0
	for(y = 0; y < m_nTransHeight; y++)
	{
		for (x = 0; x < m_nTransWidth; x++)
		{
			pCTData [y * m_nTransWidth + x] = complex <double> (0,0);
		}
	}

	// 把图像数据传给pCTData
	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			unchValue = lpImage [y * nWidth + x];
			pCTData [y * m_nTransWidth + x] = complex <double> (unchValue, 0);
		}
	}

	// 傅里叶正变换
	Fourie (pCTData, nWidth, nHeight, pCFData);

	// 下面开始实施ButterWorth高通滤波
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
	
	// 对经过ButterWorth高通滤波的图像进行反变换
	IFourie (pCFData, pCTData, nWidth, nHeight);

	// 把反变换的数据传给lpImage
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

	// 释放内存
	delete pCTData;
	delete pCFData;
	pCTData = NULL;
	pCTData = NULL;
}

void CFourie::InPutData (complex <double> *pCTData, int nWidth, int nHeight, LPBYTE lpImage)
{
	// 循环变量
	int x, y;	

	// 图像像素值
	unsigned char unchValue;

	// 初始化
	// 图像数据的高和宽不一定是2的整数次幂，所以pCTData
	// 有一部分数据需要补0
	for(y = 0; y < m_nTransHeight; y++)
	{
		for (x = 0; x < m_nTransWidth; x++)
		{
			pCTData [y * m_nTransWidth + x] = complex <double> (0,0);
		}
	}

	// 把图像数据传给pCTData
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
	// 循环变量
	int x, y;
	
	// 图像像素值
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
	// 循环变量
	int x, y;
	
	// 图像像素值
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
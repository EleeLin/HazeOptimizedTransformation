#pragma once
#include <complex>
using namespace std;

class CFourie
{
public:
	CFourie(void);
	~CFourie(void);
	void Fourie (complex <double>* pCTData, int nWidth, int nHeight, complex <double>* pCFData);
	void FastFourie (complex <double>* pCTData, complex <double>* pCFData, int nLevel);
	void IFourie (complex <double>* pCFData, complex <double>* pCTData, int nWidth, int nHeight);
	void ButterWorthLowPass (LPBYTE lpImage, int nWidth, int nHeight, int nRadius);
	void ButterWorthHighPass (LPBYTE lpImage, int nWidth, int nHeight, int nRadius);
	void InPutData (complex <double> *pCTData, int nWidth, int nHeight, LPBYTE lpImage);
	void OutPutDataDFT (complex <double> *pCTData, int nWidth, int nHeight, LPBYTE lpImage);
	void OutPutDataIFT (complex <double> *pCTData, int nWidth, int nHeight, LPBYTE lpImage);
	void GetFWH (int, int);

public:
	int m_nTransWidth;
	int m_nTransHeight;
	int m_nWidth;
	int m_nHeight;
	complex <double>*m_pCTData;
	complex <double>*m_pCFData;
};


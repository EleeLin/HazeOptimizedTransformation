// Raw.h: interface for the CRaw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAW_H__73FCCD7E_64AB_45B8_A314_D46496E466B5__INCLUDED_)
#define AFX_RAW_H__73FCCD7E_64AB_45B8_A314_D46496E466B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRaw  
{
public:
	CRaw();
	virtual ~CRaw();
	BYTE* m_pBuff;//���Raw��ʽ�������ļ�
	BYTE* m_pBuffOut;//���Raw��ʽ����������ļ�
	int m_Histogram[256];//���Raw��ʽ�ĻҶ�ֱ��ͼ
	BOOL ReadFromFile(CString strFilename,int nHeight,int nWidth);//���ļ��ж�ȡRawͼ��strFilename:Դ�ļ�������·�����ļ���
	void computeHistGray();//ͳ�ƻҶ�ֱ��ͼ
	BOOL Write(LPCTSTR lpszPathName);
	void EdgeByAnyMask(int *mask,int maskW,int maskH,int masksize);
	void Empty();
	int m_nWidth;//Raw��ʽ�Ŀ��
	int m_nHeight;//Raw��ʽ�ĸ߶�
	CSize m_sizeImage;//Raw��ʽ�Ĵ�С
private:
	void TemplateEdge(BYTE* imgIn, int width,int height,int *mask,int maskW,int maskH,int masksize,BYTE* imgOut);
	
};

#endif // !defined(AFX_RAW_H__73FCCD7E_64AB_45B8_A314_D46496E466B5__INCLUDED_)

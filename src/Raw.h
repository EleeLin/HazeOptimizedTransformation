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
	BYTE* m_pBuff;//存放Raw格式的数据文件
	BYTE* m_pBuffOut;//存放Raw格式的数据输出文件
	int m_Histogram[256];//存放Raw格式的灰度直方图
	BOOL ReadFromFile(CString strFilename,int nHeight,int nWidth);//从文件中读取Raw图像strFilename:源文件的完整路径和文件名
	void computeHistGray();//统计灰度直方图
	BOOL Write(LPCTSTR lpszPathName);
	void EdgeByAnyMask(int *mask,int maskW,int maskH,int masksize);
	void Empty();
	int m_nWidth;//Raw格式的宽度
	int m_nHeight;//Raw格式的高度
	CSize m_sizeImage;//Raw格式的大小
private:
	void TemplateEdge(BYTE* imgIn, int width,int height,int *mask,int maskW,int maskH,int masksize,BYTE* imgOut);
	
};

#endif // !defined(AFX_RAW_H__73FCCD7E_64AB_45B8_A314_D46496E466B5__INCLUDED_)

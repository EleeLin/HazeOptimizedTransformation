
// IProView.h : CIProView 类的接口
//

#pragma once
#include "Fourie.h"
#include "atltypes.h"
#include "bmpfile.h"

//灰度图像形状特征提取中的结构体
///////////////////////////////////////////////////////////
struct Point
{
	int x;
	int y;
};

struct Shapefeature
{
	int index;//该连通区域的标号
	struct Point lefttop;
	struct Point rightbottom;
	int area;
	int arealength;
};

class CIProView : public CScrollView
{
protected: // 仅从序列化创建
	CIProView();
	DECLARE_DYNCREATE(CIProView)

// 特性
public:
	CIProDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CIProView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int m_nPosX;
	int m_nPosY;
	afx_msg void OnOffset();
	afx_msg void OnRotate();
	afx_msg void OnZoom();
	afx_msg void OnInverse();
	virtual void OnInitialUpdate();
	bool m_bZoom;
	bool m_bFourie;
	double m_nZoomX;
	double m_nZoomY;
	CFourie m_fe;
	UINT m_nDel;
	afx_msg void OnDFT();
	afx_msg void OnBWHigh();
	afx_msg void OnBTLow();
	afx_msg void OnMidFiltering();
	afx_msg void OnIFT();
	afx_msg void OnGCM();
	afx_msg void OnGrayMatching();
	afx_msg void OnStateBinary();
	afx_msg void OnExtractFeature();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnWander();
	BOOL m_bWander;
	BOOL m_bWanderIn;
	int m_nOffsetX;
	int m_nOffsetY;
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnTellBinary();
	afx_msg void OnGetCSLine();
	bool m_bCSLine;
	CPoint m_Start;
	CPoint m_End;
	bool m_bCSExt;
	double m_fCSEquation_a;
	double m_fCSEquation_b;
	CBmpFile m_BmpHOT;
	afx_msg void OnCalculateHOT();
	double* m_pHOT;
	double* m_pHOTColor;
	double m_fHotMax;
	double m_fHotMin;
	afx_msg void OnDrawHOT();
	afx_msg void OnDrawColor();
};

#ifndef _DEBUG  // IProView.cpp 中的调试版本
inline CIProDoc* CIProView::GetDocument() const
   { return reinterpret_cast<CIProDoc*>(m_pDocument); }
#endif


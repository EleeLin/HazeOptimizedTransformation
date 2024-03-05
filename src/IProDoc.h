
// IProDoc.h : CIProDoc ��Ľӿ�
//


#pragma once
#include "bmpfile.h"


class CIProDoc : public CDocument
{
protected: // �������л�����
	CIProDoc();
	DECLARE_DYNCREATE(CIProDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CIProDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	CBmpFile m_file;
	CBmpFile m_fileOut;
	bool m_bfile;
	afx_msg void OnLineStretch();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg void OnTpHandle();
	afx_msg void OnMedianFilter();
};

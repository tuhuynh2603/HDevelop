#pragma once
#include "resource.h"
#include "EvImageDoc.h"

// CChildFormView form view
class CEvImageDoc;
class CChildFormView : public CFormView
{
	DECLARE_DYNCREATE(CChildFormView)

protected:
	CChildFormView();           // protected constructor used by dynamic creation
	virtual ~CChildFormView();

public:
	enum { IDD = IDD_CHILDFORMVIEW };
	static CChildFormView *CreateOne( CWnd *pParent );
	CEvImageDoc* GetDocument();
	CEvImageDoc* m_pDocument;

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
    virtual void OnInitialUpdate();
    afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);


};



#if !defined(AFX_MAGNIFIERWND_H__8F1C44EC_422C_4C83_B839_DF8E31796877__INCLUDED_)
#define AFX_MAGNIFIERWND_H__8F1C44EC_422C_4C83_B839_DF8E31796877__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MagnifierWnd.h : header file
//
#include "AppDoc.h"
/////////////////////////////////////////////////////////////////////////////
// CMagnifierWnd window

class CMagnifierWnd : public CWnd
{
// Construction
public:
	CMagnifierWnd();
	BOOL ShowMagnifier();
	CString path;
	void ZoomIn();
	void ZoomOut();
	void Clear();
	HBITMAP m_Bmap;
	HDC m_hdc;
	int Size;
	int m_nZoomValue;
	int SavedDC;
	CPen m_hPen;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMagnifierWnd)
	public:
	virtual BOOL Create(CAppDoc *pDoc,int size, CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMagnifierWnd();
	CAppDoc* m_pDoc;
	// Generated message map functions
protected:
	//{{AFX_MSG(CMagnifierWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAGNIFIERWND_H__8F1C44EC_422C_4C83_B839_DF8E31796877__INCLUDED_)

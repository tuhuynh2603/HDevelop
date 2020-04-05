/// AppView.h : interface of the CAppView class
/// Udupa; April'2012

#pragma once

#include "EvImageView.h"
//#include "MagnifierWnd.h"

#define ZOOMOUT		0
#define ZOOMIN		1
#define ZOOMFIT		2

class CAppView : public CEvImageView
{
protected: // create from serialization only
	CAppView();
	DECLARE_DYNCREATE(CAppView)

// Attributes
public:
	CAppDoc* GetDocument() const;
	//CMagnifierWnd m_MagnifierWnd;
	//CImageBuffer ImgTemp;
	CRect m_rcSize;
	//BOOL m_bPixelRuler;
// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CAppView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual void UpdateStatusBar(CString* pstrMsg);

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnViewBinarize();
	afx_msg void OnUpdateViewBinarize(CCmdUI *pCmdUI);
	afx_msg void OnViewZoom();
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnViewZoomFit();
	afx_msg void OnViewZoom100();
	afx_msg void OnUpdateViewZoom(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewZoomIn(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewZoomOut(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewZoomFit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewZoom100(CCmdUI *pCmdUI);
	//afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnShowMaginifier();
	afx_msg void OnUpdateShowMaginifier(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in AppView.cpp
inline CAppDoc* CAppView::GetDocument() const
   { return reinterpret_cast<CAppDoc*>(m_pDocument); }
#endif


/// RibbonControl.h : interface of the CRibbonControl
/// Udupa; April'2012

#pragma once

#include "afxribbonbar.h"

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

class DECLSPEC_WINDOW_IF CRibbonControl : public CMFCRibbonBar
{
public:
	CRibbonControl();
	~CRibbonControl();

	virtual BOOL IsReplaceFrameCaption();
	virtual void OnShow(BOOL bShow);
	virtual void OnPaneContextMenu(CWnd* pParentFrame, CPoint point) override;
	virtual BOOL OnShowRibbonQATMenu(CWnd* pWnd, int x, int y, CMFCRibbonBaseElement* pHit);
	virtual BOOL LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID);
	virtual BOOL SaveState(LPCTSTR lpszProfileName, int nIndex, UINT uiID);
	void Maximize(BOOL bMaximize);
	BOOL IsMaximized();
	void Initialize();
	virtual void RecalcLayout();

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


/// EvTabView.h : interface of the CEvTabView
/// Udupa; April'2012

#pragma once

#include <afxtempl.h>
#include "EvTabPage.h"
#include "EvTabDoc.h"
#include "EvSdiChildWnd.h"

//#include "DirectoryExplorer.h"

class CEvImageDoc;

#ifdef _DEBUG
#pragma pack (push, 1)

typedef struct
{
	WORD dlgVer;
	WORD signature;
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	WORD cDlgItems;
	short x;
	short y;
	short cx;
	short cy;
} DLGTEMPLATEEX;

#pragma pack (pop)
#endif // _DEBUG


// EvTabView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEvTabView view

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

class DECLSPEC_WINDOW_IF CEvTabView : public CCtrlView
{
protected:
//	CEvTabView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CEvTabView)

public:
    CImageList m_imageList;
    CArray<HICON, HICON> m_arrImage;
	CEvTabView() : CCtrlView(_T("SysTabControl32"), AFX_WS_DEFAULT_VIEW) {}
	//CEvTabView() : CCtrlView(_T("CMFCTabCtrl"), AFX_WS_DEFAULT_VIEW) {}
	CTabCtrl& GetTabCtrl() const { return *(CTabCtrl*) this; }

	int AddPage(LPCTSTR pszTitle, int nPageID,	LPCTSTR pszTemplateName);
	int AddPage(LPCTSTR pszTitle, int nPageID, int nTemplateID);
	BOOL RemovePage(int nIndex);
	int GetPageCount();
	BOOL GetPageTitle(int nIndex, CString& strTitle);
	BOOL SetPageTitle(int nIndex, LPCTSTR pszTitle);
	int GetPageID(int nIndex);
	int SetPageID(int nIndex, int nPageID);
	BOOL ActivatePage(int nIndex);
	int GetActivePage();
	CWnd* GetPage(int nIndex);
	int GetPageIndex(int nPageID);
	CEvImageDoc* GetActiveDoc();

	CEvSdiChildWnd* m_SdiChildWnd;
	CEvImageDoc** m_pDocs;

//	CDirectoryExplorer *m_nDirectoryExplorer;
	
	CBrush m_BrushActive;
	CBrush m_BrushPassive;
	CBrush* m_pBrush;
	COLORREF m_Color;
	CFont m_font;
	CFont m_fontId;
	CFont m_fontResult;
	int m_nTotalDocs;

	CSliderCtrl m_sliderBinarize;
	CStatic m_staticBinarize;
	CStatic m_staticTrackId;
	CStatic m_staticTrackStatus;
	
	COLORREF m_colorActive; 
	COLORREF m_colorPassive; 
	COLORREF m_colorResult; 
	COLORREF m_colorPass; 
	COLORREF m_colorFail; 

	CEvTabDoc* GetDocument();
	void SetCameraId(CString* camId);
	BOOL m_bShowCamId;

#ifdef _DEBUG
	BOOL CheckDialogTemplate(LPCTSTR pszTemplateName);
#endif // _DEBUG
	void ResizeDialog(int nIndex, int cx, int cy);

protected:
	CTypedPtrArray<CObArray, CEvTabPage*> m_pPages;
	CArray<HWND, HWND> m_hFocusWnd;
	CArray<int, int> m_nPageIDs;

	CString m_strCamId, m_strDocStatus;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEvTabView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void SetTitle() {};
	virtual void SetDocStatusText() { };

	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnInitPage(int nIndex, int nPageID);
	virtual void OnActivatePage(int nIndex, int nPageID);
	virtual void OnDeactivatePage(int nIndex, int nPageID);
	virtual void OnDestroyPage(int nIndex, int nPageID);
	//}}AFX_VIRTUAL
	
	
// Implementation
protected:
	virtual ~CEvTabView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CEvTabView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSelChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnSetBinaryMode(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetDocStatusText(WPARAM wParam, LPARAM lParam);
	afx_msg void OnInspStatusClick();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#ifndef _DEBUG  // debug version in MyTabView.cpp
inline CEvTabDoc* CEvTabView::GetDocument()
   { return (CEvTabDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

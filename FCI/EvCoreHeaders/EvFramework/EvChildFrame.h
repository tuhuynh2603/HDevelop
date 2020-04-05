/// EvChildFrame.h : interface of the CEvChildFrame class
/// Udupa; April'2012

#pragma once

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

class DECLSPEC_WINDOW_IF CEvChildFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CEvChildFrame)
public:
	CEvChildFrame();

// Attributes
public:
	CMFCToolBar m_wndToolBar;

// Operations
public:
	void EnableMaximizeBox();

// Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
     virtual BOOL CanShowOnTaskBarTabs();

	 virtual void SaveBarState(LPCTSTR lpszProfileName);
	 virtual void LoadBarState(LPCTSTR lpszProfileName);
// Implementation
public:
	virtual ~CEvChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnSizeParent(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();

// Generated message map functions
protected:
	///virtual int CreateFrame()=0;
	DECLARE_MESSAGE_MAP()
};

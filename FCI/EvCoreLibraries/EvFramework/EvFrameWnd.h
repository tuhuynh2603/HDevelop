/// EvFrameWnd.h : interface of the CEvFrameWnd class
/// Udupa; April'2012

#pragma once
// EvFrameWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEvFrameWnd frame

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

class DECLSPEC_WINDOW_IF CEvFrameWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CEvFrameWnd)
protected:
	CEvFrameWnd();           // protected constructor used by dynamic creation

// Attributes
public:


// Operations
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEvFrameWnd)
	//}}AFX_VIRTUAL
	virtual BOOL LoadEvFrame(UINT nIDResource,
				DWORD dwDefaultStyle, CRect rect,
				CWnd* pParentWnd, DWORD dwDefaultStyleEx, 
				CCreateContext* pContext);

// Implementation
protected:
	virtual ~CEvFrameWnd();

	// Generated message map functions
	//{{AFX_MSG(CEvFrameWnd)
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

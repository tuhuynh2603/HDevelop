/// EvSdiChildWnd.h : interface of the CEvSdiChildWnd class
/// Udupa; April'2012

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CEvSdiChildWnd window

#include "EvSdiTemplate.h"

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

class DECLSPEC_WINDOW_IF CEvSdiChildWnd : public CWnd
{
// Construction
public:
	CEvSdiChildWnd();

// Attributes
public:
	CWnd *m_pParent;

	CEvSdiTemplate* m_pDocTemplate;

// Operations
public:
	BOOL Create(CWnd *pParent, DWORD ctrlId,
		UINT nIDResource,
		CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass,
		CRuntimeClass* pViewClass,
		DWORD Style, DWORD StyleEx);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEvSdiChildWnd)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEvSdiChildWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEvSdiChildWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

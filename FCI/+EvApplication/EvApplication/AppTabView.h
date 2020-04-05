/// AppTabView.h : header file
/// Udupa; April'2012

#pragma once

#include "EvTabView.h"

/////////////////////////////////////////////////////////////////////////////
// CAppTabView view

class CAppTabView : public CEvTabView
{
protected:
	DECLARE_DYNCREATE(CAppTabView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppTabView)
	virtual void OnInitialUpdate();
	virtual void SetTitle();
	virtual void SetDocStatusText();
	
protected:
	//}}AFX_VIRTUAL
	
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CAppTabView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

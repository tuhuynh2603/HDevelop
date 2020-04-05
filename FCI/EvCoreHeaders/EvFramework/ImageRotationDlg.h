#pragma once

#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CImageRotationDlg dialog

class CImageRotationDlg : public CDialog
{
// Construction
public:
	CImageRotationDlg(CWnd* pParent = NULL);   // standard constructor
	CView *m_pView;

// Dialog Data
	//{{AFX_DATA(CImageRotationDlg)
	enum { IDD = IDD_IMG_ROT_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageRotationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	double GetAngle();
protected:
	double m_dAngleDeg;			// Amount of rotation of image

	// Generated message map functions
	//{{AFX_MSG(CImageRotationDlg)
	afx_msg void OnRotCCW();
	afx_msg void OnRotCW();
	afx_msg void OnRotCcw2();
	afx_msg void OnRotCw2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

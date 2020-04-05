#pragma once

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CChangePasswordDlg dialog

class CChangePasswordDlg : public CDialog
{
// Construction
public:
	CChangePasswordDlg(CWnd* pParent = NULL);   // standard constructor
	~CChangePasswordDlg();

// Dialog Data
	//{{AFX_DATA(CChangePasswordDlg)
	enum { IDD = IDD_CHANGE_PASSWORD_DLG };

	CString	m_strNewPassword;
	CString	m_strConfirmNewPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangePasswordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBrush m_brush;
	char m_title[1024];	
	// Generated message map functions
	//{{AFX_MSG(CChangePasswordDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

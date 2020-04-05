#pragma once

//#include "StaticGradient.h"
/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog

class CLoginDlg : public CDialog
{
// Construction
public:
	CLoginDlg(CWnd* pParent = NULL);   // standard constructor
	~CLoginDlg();
	CString m_strUserLevel;

// Dialog Data
	//{{AFX_DATA(CLoginDlg)
	enum { IDD = IDD_LOGIN_DIALOG };
	//CComboBox	m_ctrlLevel;
	CComboBox	m_ctrlLevel;
	CString	m_strPassword;
	CString	m_strUsername;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoginDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBrush m_brush;
	// Generated message map functions
	//{{AFX_MSG(CLoginDlg)
	afx_msg void OnSelchangeAccessLevel();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void FillComboBoxList();
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

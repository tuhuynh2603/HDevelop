#pragma once

/////////////////////////////////////////////////////////////////////////////
// CModifyAccounts dialog

class CApplication;

class CModifyAccounts : public CDialog
{
// Construction
public:
	CModifyAccounts(CWnd* pParent = NULL);   // standard constructor
	~CModifyAccounts();
	CApplication* m_pApp;

// Dialog Data
	//{{AFX_DATA(CModifyAccounts)
	enum { IDD = IDD_MODIFY_ACCOUNT_DIALOG };
	int		m_nLogoutTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModifyAccounts)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    CBrush m_brush;
	// Generated message map functions
	//{{AFX_MSG(CModifyAccounts)
	afx_msg void OnCreateNewUser();
	afx_msg void OnDeleteUser();
	afx_msg void OnSetfocusLogoutTime();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

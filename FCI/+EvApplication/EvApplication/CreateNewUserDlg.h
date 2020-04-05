#pragma once

/////////////////////////////////////////////////////////////////////////////
// CCreateNewUserDlg dialog

class CCreateNewUserDlg : public CDialog
{
// Construction
public:
	CCreateNewUserDlg(CWnd* pParent = NULL);   // standard constructor
	~CCreateNewUserDlg();
	CString m_strAccessLevel;
	UserAccountInfo* m_pUser;
	bool bUserCreation;

// Dialog Data
	//{{AFX_DATA(CCreateNewUserDlg)
	enum { IDD = IDD_NEW_LOGIN_DIALOG };
	CComboBox m_ctrlAccessLevel;
	CString	m_strUsername;
	CString	m_strPassword;
	CString	m_strConfirmPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateNewUserDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBrush m_brush;
	// Generated message map functions
	//{{AFX_MSG(CCreateNewUserDlg)
	afx_msg void OnSelchangeAccLevel();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSetfocusEditUser();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void FillComboBoxList();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CDeleteUserDlg dialog

class CDeleteUserDlg : public CDialog
{
// Construction
public: 
	CDeleteUserDlg(CWnd* pParent = NULL);   // standard constructor
	~CDeleteUserDlg();
	CApplication* m_pApp;

// Dialog Data
	//{{AFX_DATA(CDeleteUserDlg)
	enum { IDD = IDD_DELETE_USER_DIALOG };
	CComboBox m_ctrlUserName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteUserDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBrush m_brush;
	// Generated message map functions
	//{{AFX_MSG(CDeleteUserDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelete();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void FillComboBoxList();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

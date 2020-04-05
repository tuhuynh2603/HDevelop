#pragma once

/////////////////////////////////////////////////////////////////////////////
// CNewRecipeDialog dialog

class CNewRecipeDialog : public CDialogEx
{
// Construction
public:
	CNewRecipeDialog(CWnd* pParent = NULL);   // standard constructor
	~CNewRecipeDialog();

// Dialog Data
	//{{AFX_DATA(CNewRecipeDialog)
	enum { IDD = IDD_NEW_CONFIG_FILE_DLG };
	CString	m_strNewFileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewRecipeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBrush m_brush;
	// Generated message map functions
	//{{AFX_MSG(CNewRecipeDialog)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

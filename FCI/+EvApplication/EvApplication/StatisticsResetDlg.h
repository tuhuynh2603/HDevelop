#pragma once

/////////////////////////////////////////////////////////////////////////////
// CStatisticsResetDlg dialog

class CStatisticsResetDlg : public CDialog
{
// Construction
public:
	CStatisticsResetDlg(CWnd* pParent = NULL);   // standard constructor
	~CStatisticsResetDlg();

// Dialog Data
	//{{AFX_DATA(CStatisticsResetDlg)
	enum { IDD = IDD_STATISTICS_RESET_DLG };
	CComboBox	m_comboSelectResetTime2;
	CComboBox    m_comboSelectResetTime1;
	CComboBox	m_comboSelectResetNumber;
	//}}AFX_DATA

public:
	CApplication* m_pApp;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatisticsResetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBrush m_brush;
	// Generated message map functions
	//{{AFX_MSG(CStatisticsResetDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeSelectResetNumber();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

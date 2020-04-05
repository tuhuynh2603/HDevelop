#pragma once

#include "LineChartCtrl.h"
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CYieldTrendDlg dialog

class CYieldTrendDlg : public CDialog
{
// Construction
public:
	CYieldTrendDlg(CWnd* pParent = NULL);   // standard constructor
	CYieldTrendDlg(CAppDoc* pDoc);   // standard constructor
	
	CApplication* m_pApp;
	CAppDoc* m_pDoc2;
	~CYieldTrendDlg();

	CLineChartCtrl m_wndLineChart;

// Dialog Data
	//{{AFX_DATA(CYieldTrendDlg)
	enum { IDD = IDD_YIELD_TREND_DLG };
	double	m_dYieldTrk1;
	double	m_dYieldTrk2;
	double	m_dYieldTrk3;
	double	m_dYieldTrk4;
	double	m_dYieldTrk5;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CYieldTrendDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CBrush m_brush;
	// Generated message map functions
	//{{AFX_MSG(CYieldTrendDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

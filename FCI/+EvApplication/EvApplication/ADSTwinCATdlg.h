#if !defined(AFX_ADSTWINCATDLG_H__C5B9ED24_FF78_4B36_AAAF_76E44FDF086A__INCLUDED_)
#define AFX_ADSTWINCATDLG_H__C5B9ED24_FF78_4B36_AAAF_76E44FDF086A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ADSTwinCATdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CADSTwinCATdlg dialog

class CADSTwinCATdlg : public CDialog
{
// Construction
public:
	CADSTwinCATdlg(CWnd* pParent = NULL);   // standard constructor


// Dialog Data
	//{{AFX_DATA(CADSTwinCATdlg)
	enum { IDD = IDD_TWINCAT_ADS_DIALOG };
	int		m_nNetID0;
	int		m_nNetID1;
	int		m_nNetID2;
	int		m_nNetID3;
	int		m_nNetID4;
	int		m_nNetID5;
	int		m_nADSPortNr;
	int		m_nCycleTime;
	int		m_nMaxDelay;
	int		m_nTimeout;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CADSTwinCATdlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CADSTwinCATdlg)
	afx_msg void OnCheckPortnr();
	afx_msg void OnSelchangeComboPortnr();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADSTWINCATDLG_H__C5B9ED24_FF78_4B36_AAAF_76E44FDF086A__INCLUDED_)

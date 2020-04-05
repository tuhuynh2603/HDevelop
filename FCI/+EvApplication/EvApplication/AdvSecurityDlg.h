#if !defined(AFX_ADVSECURITYDLG_H__D436C0D2_3A64_495A_A3FD_02ADF6DF1E78__INCLUDED_)
#define AFX_ADVSECURITYDLG_H__D436C0D2_3A64_495A_A3FD_02ADF6DF1E78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvSecurityDlg.h : header file
//
#include "PropertyListCtrl.h"
#include "MyData.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CAdvSecurityDlg dialog

class CAdvSecurityDlg : public CDialog
{
// Construction
public:
	bool SetSecurityAccess();
	bool GetSecurityAccess();
	CString GetAccsLevelStatus(DWORD dwValue,CString strParam,int nOldValue);
	CAdvSecurityDlg(CWnd* pParent = NULL);   // standard constructor
	~CAdvSecurityDlg();
	CMyData m_data;
// Dialog Data
	//{{AFX_DATA(CAdvSecurityDlg)
	enum { IDD = IDD_ADVANCE_SECURITY_DLG };
	CPropertyListCtrl	m_propertyControl;
	CString	m_strTechPassword;
	CString	m_strEngPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvSecurityDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBrush m_brush;
	int m_nOlddwvalue[70];
	// Generated message map functions
	//{{AFX_MSG(CAdvSecurityDlg)
	virtual void OnOK();
	afx_msg void OnDefault();
	virtual BOOL OnInitDialog();
	afx_msg void OnReadfile();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CMyStaticPropertyItemManger m_propertyManager;
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSECURITYDLG_H__D436C0D2_3A64_495A_A3FD_02ADF6DF1E78__INCLUDED_)

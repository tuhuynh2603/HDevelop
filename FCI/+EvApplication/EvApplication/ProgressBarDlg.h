#pragma once

#include <afxwin.h>
#include <afxcmn.h>
#include "Resource.h"
// CProgressBarDlg dialog

typedef BOOL(WINAPI *SLWA)(HWND, COLORREF, BYTE, DWORD);

class CProgressBarDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressBarDlg)

public:
	CProgressBarDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CProgressBarDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROGRESS_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	CBrush m_brush;

	virtual void OnOK();
	virtual void OnCancel();


public:
	CString m_strProgress;
	CProgressCtrl m_ProgressBar;
	unsigned int ComputerTime;
	DWORD m_nTimer;
	int m_nCurrentPos;

	void SetStatus(CString strStatus, int nPos);

	// Generated message map functions
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()	
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);


};

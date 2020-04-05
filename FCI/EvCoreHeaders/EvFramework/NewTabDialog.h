/// NewTabDialog.h : interface of the CNewTabDialog class
/// Udupa; April'2012

#pragma once
#include "afxbutton.h"

// CNewTabDialog dialog

class CNewTabDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CNewTabDialog)

public:
	CNewTabDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewTabDialog();

// Dialog Data
	enum { IDD = IDD_NEWTABS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CBrush m_brush;
	COLORREF m_backColor;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedMessageloggerMfcbutton();
	afx_msg void OnBnClickedImagehistoryMfcbutton();
	afx_msg void OnBnClickedMulticastMfcbutton();
	virtual void OnCancel();
	afx_msg void OnClose();
	CMFCButton m_buttonImageHistory;
	CMFCButton m_buttonMessageLog;
	CMFCButton m_buttonCpuLoad;
	CMFCButton m_buttonImageBrowser;
	CMFCButton m_buttonValidationDoc;
	CMFCButton m_buttonStatistics;
	CMFCButton m_buttonWatchdog;
	CMFCButton m_buttonMultiCast;
};

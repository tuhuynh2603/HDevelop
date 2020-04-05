#pragma once
#include <map> 
#define MAX_CLIENT_MONITORING 5

class CApplication;

// CCentralizedMonitorDlg dialog

class CCentralizedMonitorDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCentralizedMonitorDlg)

public:
	CCentralizedMonitorDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CCentralizedMonitorDlg();

	//0: Black, 1: Yellow	, 2: Green, 3: Red
	unsigned int m_nIconStatus[MAX_CLIENT_MONITORING];
	CRgn m_prgnClientSegment[MAX_CLIENT_MONITORING];
	CBrush m_brushGreen;
	CBrush m_brushRed;
	CBrush m_brushBlack;
	CBrush m_brushYellow;

	std::map<CString, BOOL> m_ClientAcceptedMap;
	std::map<CString, CString> m_ClientAcceptedMachineId;

	CApplication* m_pApp;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CENTRALIZED_MONITOR_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonDisconnectClient0();
	afx_msg void OnBnClickedButtonDisconnectClient1();
	afx_msg void OnBnClickedButtonDisconnectClient2();
	afx_msg void OnBnClickedButtonDisconnectClient3();
	afx_msg void OnBnClickedButtonDisconnectClient4();
};

#pragma once


// CErrorDlg dialog

class CErrorDlg : public CDialog
{
	DECLARE_DYNAMIC(CErrorDlg)

	CFont Font1;
	CFont Font2;
	CFont FontTitle;

	COLORREF clGoodStatus;
	COLORREF clBadStatus;

	bool bTransportErrorStatus;
	bool bErrorInSystem;
	bool bTransportShow;
	bool bTimerRun;
	BOOL bDoUpdate;

public:
	void PostModuleStatus(bool bError, CString strMsg);
	void PostModuleStatus(bool bError, CString strTitle, CString strMsg);
	void ResetState(void);


public:
	CErrorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CErrorDlg();
	virtual BOOL OnInitDialog();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ERRORDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	//	afx_msg void OnBnClickedErrordlgContractButton();
	//	afx_msg void OnBnClickedErrordlgExpandButton();
private:

	CBitmapButton m_ctrlTransportRetryButton;
	CBitmapButton m_ctrlTransportStopButton;
	CBitmapButton m_ctrlReScanButton;


	CString m_strTransportErrorMsg;
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnCancel();
	afx_msg void OnOK();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	//afx_msg void OnBnClickedErrordlgTransportRetryButton();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	/*afx_msg void OnStnClickedErrordlgTransportmsgLbl();
	afx_msg void OnBnClickedErrordlgTransportStopButton();
	afx_msg void OnBnClickedErrordlgTransportRescanButton();*/
};

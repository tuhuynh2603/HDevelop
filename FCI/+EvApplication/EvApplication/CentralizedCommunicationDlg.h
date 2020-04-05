#pragma once
#include "resource.h"

// CentralizedCommunicationDlg dialog

class CentralizedCommunicationDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CentralizedCommunicationDlg)

public:
	CentralizedCommunicationDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CentralizedCommunicationDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CENTRALIZED_COMM_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strMessage;
	CString m_strImagePath;
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedButtonSendImage();
	afx_msg void OnBnClickedButtonSendAllImage();
	afx_msg void OnBnClickedButtonSendMappingResult();
	afx_msg void OnBnClickedButtonSend2();
};

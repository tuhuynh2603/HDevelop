#pragma once
#include "resource.h"

// CCentralizedFTPSettings dialog

class CCentralizedFTPSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CCentralizedFTPSettings)

public:
	CCentralizedFTPSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCentralizedFTPSettings();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CENTRALIZED_COMMUNICATION_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strSystemName;
	CString m_strSharedDirectory;
	CString m_strUserName;
	CString m_strPassword;
	afx_msg void OnBnClickedButtonConnect();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	void Connect();
};

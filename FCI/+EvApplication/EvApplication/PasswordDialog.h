#pragma once


// CPasswordDialog dialog

class CPasswordDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CPasswordDialog)

public:
	CPasswordDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPasswordDialog();

// Dialog Data
	enum { IDD = IDD_PASSWORD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strPassword;
};

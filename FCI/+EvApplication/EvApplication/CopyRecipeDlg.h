#pragma once


// CCopyRecipeDlg dialog

class CCopyRecipeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCopyRecipeDlg)

public:
	CCopyRecipeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCopyRecipeDlg();

// Dialog Data
	enum { IDD = IDD_PARM_COPYRECIPE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strNewRecipeName;
	afx_msg void OnBnClickedOk();
};

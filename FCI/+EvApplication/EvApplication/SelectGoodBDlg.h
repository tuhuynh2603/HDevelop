#pragma once
#include "resource.h"
#include "IniFile.h"

// CSelectGoodBDlg dialog

class CSelectGoodBDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectGoodBDlg)

public:
	CSelectGoodBDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectGoodBDlg();

	CListBox m_ctrlDefectsAvailable;
	CListBox m_ctrlGoodB;
	CIniFile iniFile;

	CString m_strFileDir;
	CString m_strKeyName;
// Dialog Data
	enum { IDD = IDD_SELECT_GOODB_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void GetGoodBList();
	void GetDefectsList();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedRightButton();
	afx_msg void OnBnClickedLeftButton();
	virtual BOOL OnInitDialog();
};

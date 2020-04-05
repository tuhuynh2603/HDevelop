#include "resource.h"

#pragma once
// CScanBarcode dialog

class CApplication;
class CScanBarcode : public CDialogEx
{
	DECLARE_DYNAMIC(CScanBarcode)

public:	
	CScanBarcode(CWnd* pParent = NULL);   // standard constructor
	virtual ~CScanBarcode();

// Dialog Data
	enum { IDD = IDD_SCAN_BARCODE_DIALOG };

	CApplication* m_pApp;
	CString m_strBarcode;
	CEdit m_editBox;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void DisableQuitButton(BOOL bDisable);
	BOOL FindRecipe();
	afx_msg void OnEnChangeEditBarcode();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedQuit();
};

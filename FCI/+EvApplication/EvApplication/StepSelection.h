#pragma once
#include "resource.h"

// CStepSelection dialog

class CStepSelection : public CDialogEx
{
	DECLARE_DYNAMIC(CStepSelection)

public:
	CStepSelection(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStepSelection();

// Dialog Data
	enum { IDD = IDD_STEP_SELECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnInitMenu(CMenu* pMenu);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};

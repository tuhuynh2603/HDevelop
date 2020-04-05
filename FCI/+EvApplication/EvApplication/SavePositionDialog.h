#pragma once

#include "resource.h"


// CSavePositionDialog dialog

class CSavePositionDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CSavePositionDialog)

public:
	CSavePositionDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSavePositionDialog();

// Dialog Data
	enum { IDD = IDD_SAVEPOS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nSaveOption;
	afx_msg void OnBnClickedOk();
	CString m_strNewRecipe;
};

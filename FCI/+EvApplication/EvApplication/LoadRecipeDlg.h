#pragma once
#include "afxwin.h"

class CApplication;

// CLoadRecipeDlg dialog

class CLoadRecipeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoadRecipeDlg)

public:
	CLoadRecipeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoadRecipeDlg();

// Dialog Data
	enum { IDD = IDD_PARM_LOADRECIPE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CApplication* m_pApp;
	void PopulateRecipeList();

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CListBox m_listRecipe;
	afx_msg void OnLbnSelchangeRecipeList();
};

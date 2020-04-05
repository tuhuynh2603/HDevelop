#pragma once
#include "afxwin.h"

class CApplication;

// DeleteRecipeDlg dialog

class CDeleteRecipeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDeleteRecipeDlg)

public:
	CDeleteRecipeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDeleteRecipeDlg();

// Dialog Data
	enum { IDD = IDD_PARM_DELETERECIPE_DIALOG };

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

/// EvTabPage.cpp : implementation of the CEvTabPage class
/// Udupa; April'2012

#include "stdafx.h"
#include "EvTabPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEvTabPage dialog


CEvTabPage::CEvTabPage(CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
#ifndef _AFX_NO_OCC_SUPPORT
	AfxEnableControlContainer ();
#endif // !_AFX_NO_OCC_SUPPORT
	//{{AFX_DATA_INIT(CEvTabPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CEvTabPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEvTabPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEvTabPage, CDialog)
	//{{AFX_MSG_MAP(CEvTabPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEvTabPage message handlers

BOOL CEvTabPage::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
#ifndef _AFX_NO_OCC_SUPPORT
	if (nCode == CN_EVENT)
		return GetParent()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
#endif // !_AFX_NO_OCC_SUPPORT

	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CEvTabPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return (BOOL) GetParent()->SendMessage(WM_COMMAND, wParam, lParam);
//	return CDialog::OnCommand(wParam, lParam);
}

BOOL CEvTabPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return (BOOL) GetParent()->SendMessage(WM_NOTIFY, wParam, lParam);
//	return CDialog::OnNotify(wParam, lParam, pResult);
}

BOOL CEvTabPage::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	return GetParent()->PreTranslateMessage(pMsg);
//	return CDialog::PreTranslateMessage(pMsg);
}

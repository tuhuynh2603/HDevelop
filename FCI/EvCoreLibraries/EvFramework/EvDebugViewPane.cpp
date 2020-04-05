/// EvDebugViewPane.cpp : implementation of the CEvDebugViewPane class
/// Udupa; August'2012

#include "StdAfx.h"
#include "EvDebugViewPane.h"
#include "Resource.h"
#include "EvFrameWnd.h"
#include "EvMainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEvDebugViewPane

CEvDebugViewPane::CEvDebugViewPane()
{
	m_pMessageDialog = NULL;
	bIsClosed = FALSE;
}

CEvDebugViewPane::~CEvDebugViewPane()
{
	
}

void CEvDebugViewPane::OnClose() 
{
	CloseChildren();
	bIsClosed = TRUE;
	CDockablePane::OnClose();
}

void CEvDebugViewPane::CloseChildren()
{
	if(m_pMessageDialog != NULL) {
		if(IsWindow(m_pMessageDialog->m_hWnd))
			m_pMessageDialog->DestroyWindow();
		delete m_pMessageDialog;
		m_pMessageDialog = NULL;
	}
}


BEGIN_MESSAGE_MAP(CEvDebugViewPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


_declspec(dllimport) CDialog* ShowMessageDialogNonModal(CWnd* pParent);
int CEvDebugViewPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pMessageDialog = ShowMessageDialogNonModal(this);
	if(m_pMessageDialog == NULL)
		return -1;

	return 0;
}


void CEvDebugViewPane::AdjustLayout()
{
	if((m_pMessageDialog != NULL) && ::IsWindow(m_pMessageDialog->m_hWnd)) {
		CRect rect;
		GetWindowRect(&rect);

		int cx = rect.Width();
		int cy = rect.Height();

		m_pMessageDialog->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);
		m_pMessageDialog->Invalidate();
	}
}

void CEvDebugViewPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	//m_wndStatsDlg.SetSize(cx,cy);
	m_pMessageDialog->SetWindowPos(NULL,0,0,cx,cy,SWP_NOACTIVATE|SWP_NOZORDER);
	m_pMessageDialog->Invalidate();
}

void CEvDebugViewPane::UpdateFonts()
{
}


void CEvDebugViewPane::OnPressCloseButton()
{
	CDockablePane::OnPressCloseButton();
	bIsClosed = TRUE;
}
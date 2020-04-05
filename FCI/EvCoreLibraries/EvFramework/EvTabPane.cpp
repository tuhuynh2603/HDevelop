/// EvTabPane.cpp : implementation of the CEvTabPane class
/// Udupa; August'2012

#include "StdAfx.h"
#include "EvTabPane.h"
#include "MsgCode.h"


void CEvTabPane::OnPressCloseButton()
{
	CDockablePane::OnPressCloseButton();
	((CMDIFrameWndEx*) AfxGetMainWnd())->SendMessage(WM_PANE_CLOSED, (WPARAM) this, NULL);

	((CMDIFrameWndEx*) AfxGetMainWnd())->RecalcLayout();
}

void CEvTabPane::OnAfterDockFromMiniFrame()
{
	((CMDIFrameWndEx*) AfxGetMainWnd())->RecalcLayout();
}

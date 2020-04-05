/// RibbonControl.cpp : implementation of the CRibbonControl class
/// Udupa; April'2012

#include "StdAfx.h"
#include "RibbonControl.h"


CRibbonControl::CRibbonControl()// : CMFCRibbonBar(FALSE)
{
//	m_QAToolbar.SetVisible(FALSE);
}


CRibbonControl::~CRibbonControl()
{
}

void CRibbonControl::Initialize()
{
	m_QAToolbar.SetRect(CRect(0,0,0,0));
	m_QAToolbar.Redraw();
}

BOOL CRibbonControl::IsReplaceFrameCaption()
{
	return FALSE;
}

void CRibbonControl::OnShow(BOOL bShow)
{
}

void CRibbonControl::Maximize(BOOL bMaximize)
{
	if(bMaximize) {
		if(m_dwHideFlags & AFX_RIBBONBAR_HIDE_ELEMENTS)
			ToggleMimimizeState();
	}
	else {
		if((m_dwHideFlags & AFX_RIBBONBAR_HIDE_ELEMENTS) == 0)
			ToggleMimimizeState();
	}
}

BOOL CRibbonControl::IsMaximized()
{
	return ((m_dwHideFlags & AFX_RIBBONBAR_HIDE_ELEMENTS)==0);
}

void CRibbonControl::RecalcLayout()
{
	CMFCRibbonBar::RecalcLayout();
}

void CRibbonControl::OnPaneContextMenu(CWnd* pParentFrame, CPoint point)
{
}

BOOL CRibbonControl::OnShowRibbonQATMenu(CWnd* pWnd, int x, int y, CMFCRibbonBaseElement* pHit)
{
	return FALSE;
}

BOOL CRibbonControl::LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	return TRUE;
}

BOOL CRibbonControl::SaveState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	return TRUE;
}

void CMFCRibbonBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//CPane::OnLButtonDblClk(nFlags, point);
}
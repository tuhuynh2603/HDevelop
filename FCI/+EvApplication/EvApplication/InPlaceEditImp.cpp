// InPlaceEditImp.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Štefan Belopotoèan, StefanBelopotocan@cmpnetmail.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InPlaceEditImp.h"
#include "ApplicationDef.h"

//#include "UserMessageID.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CInPlaceEditImp

IMPLEMENT_DYNAMIC(CInPlaceEditImp, CEdit)

CInPlaceEditImp* CInPlaceEditImp::CreateInPlaceEdit(const CString& strInitText, CWnd* pWndParent, CRect& rect, DWORD exDwStyle)
{
	CInPlaceEditImp* pEdit = new CInPlaceEditImp(strInitText);
	rect.DeflateRect(0, 1);
	pEdit->Create(WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|ES_LEFT | exDwStyle, rect, pWndParent, IDC_IPEDIT);

	return pEdit;
}

void CInPlaceEditImp::MoveControl(CRect& rect)
{
	rect.DeflateRect(0, 1);
	MoveWindow(rect, FALSE);
}

BEGIN_MESSAGE_MAP(CInPlaceEditImp, CEdit)
	//{{AFX_MSG_MAP(CInPlaceEditImp)
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEditImp message handlers

int CInPlaceEditImp::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CEdit::OnCreate(lpCreateStruct) == -1) 
		return -1;

	CFont* pFont = GetParent()->GetFont();
	SetFont(pFont);

	SetWindowText(m_strInitText);

	return 0;
}

void CInPlaceEditImp::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar == VK_ESCAPE || nChar == VK_RETURN)
		GetParent()->SetFocus();
	else
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CInPlaceEditImp::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	GetOwner()->SendMessage(WM_USER_SET_DATA);
}
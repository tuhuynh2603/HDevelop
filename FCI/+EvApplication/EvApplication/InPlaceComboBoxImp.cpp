// InPlaceComboBoxImp.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Štefan Belopotoèan, StefanBelopotocan@cmpnetmail.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InPlaceComboBoxImp.h"
#include "ApplicationDef.h"

//#include "UserMessageID.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInPlaceCBEditImp

BEGIN_MESSAGE_MAP(CInPlaceCBEditImp, CEdit)
	//{{AFX_MSG_MAP(CInPlaceCBEditImp)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceCBEditImp message handlers

void CInPlaceCBEditImp::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar == VK_UP || nChar == VK_DOWN)
		GetOwner()->SendMessage(WM_USER_ON_NEW_SELECTION, nChar);
	else
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
// CInPlaceCBListBoxImp

CInPlaceCBListBoxImp::CInPlaceCBListBoxImp()
{
}

CInPlaceCBListBoxImp::~CInPlaceCBListBoxImp()
{
}


BEGIN_MESSAGE_MAP(CInPlaceCBListBoxImp, CListBox)
	//{{AFX_MSG_MAP(CInPlaceCBListBoxImp)
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceCBListBoxImp message handlers

void CInPlaceCBListBoxImp::OnKillFocus(CWnd* pNewWnd) 
{
	CListBox::OnKillFocus(pNewWnd);

	GetOwner()->SendMessage(WM_USER_ON_SELECTION_CANCEL);
}

void CInPlaceCBListBoxImp::ProcessSelected()
{
	int nSelectedItem = GetCurSel();

	GetOwner()->SendMessage(WM_USER_ON_SELECTION_OK, nSelectedItem, GetItemData(nSelectedItem));
}

void CInPlaceCBListBoxImp::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CListBox::OnLButtonUp(nFlags, point);

	ProcessSelected();
}

BOOL CInPlaceCBListBoxImp::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN)
		{
			ProcessSelected();
			return TRUE;
		}
	}

	return CListBox::PreTranslateMessage(pMsg);
}


#define _IDB_ARROW_DOWN	32738

/////////////////////////////////////////////////////////////////////////////
// CInPlaceComboBoxImp

IMPLEMENT_DYNAMIC(CInPlaceComboBoxImp, CWnd)

CInPlaceComboBoxImp::CInPlaceComboBoxImp()
{
	m_nButtonDx = ::GetSystemMetrics(SM_CXHSCROLL);
	m_nCurrentSelection = -1;
}

CInPlaceComboBoxImp* CInPlaceComboBoxImp::CreateInPlaceComboBox(CWnd* pWndParent, CRect& rect)
{	
	CInPlaceComboBoxImp* pInPlaceComboBox = (CInPlaceComboBoxImp*)new CInPlaceComboBoxImp;
	
	pInPlaceComboBox->Create(NULL, "", WS_VISIBLE|WS_CHILD, rect, pWndParent, IDC_IPEDIT);

	return pInPlaceComboBox;
}

BEGIN_MESSAGE_MAP(CInPlaceComboBoxImp, CWnd)
	//{{AFX_MSG_MAP(CInPlaceComboBoxImp)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_USER_ON_SELECTION_OK, OnSelectionOk)
	ON_MESSAGE(WM_USER_ON_SELECTION_CANCEL, OnSelectionCancel)
	ON_MESSAGE(WM_USER_ON_NEW_SELECTION, OnNewSelection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceComboBoxImp message handlers

int CInPlaceComboBoxImp::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_nTextHeight = lpCreateStruct->cy + 1;
	
	CRect rect;
	GetClientRect(rect);
	rect.DeflateRect(0, 1);
	rect.right -= m_nButtonDx;

	CFont* pFont = GetParent()->GetFont();

	// Vstupný riadok
	m_wndEdit.Create(WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, rect, this, IDC_IPEDIT + 1);
	m_wndEdit.SetOwner(this);
	m_wndEdit.SetFont(pFont);

	// Zoznam položiek
	rect.right += m_nButtonDx - 1;
	rect.top = rect.bottom + 2;
	rect.bottom += 100;

	ClientToScreen(rect);
	GetParent()->ScreenToClient(rect);

	m_wndList.Create(WS_BORDER|WS_CHILD|WS_VSCROLL, rect, GetParent(), IDC_IPEDIT + 2);
	m_wndList.SetOwner(this);
	m_wndList.SetFont(pFont);
	
	return 0;
}

void CInPlaceComboBoxImp::MoveControl(CRect& rect)
{
	CRect prevRect;
	GetClientRect(prevRect);

	CWnd* pWndParent = GetParent();

	// Prekresli predošlú pozíciu
	ClientToScreen(prevRect);
	pWndParent->ScreenToClient(prevRect);
	pWndParent->InvalidateRect(prevRect);

	// Nová pozícia
	MoveWindow(rect, FALSE);
	m_wndList.SetWindowPos(NULL, rect.left, rect.bottom + 1, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
}

#define DEFAULT_IPLISTBOX_HEIGHT 16 * 8

void CInPlaceComboBoxImp::ResetListBoxHeight()
{
	CRect rect;

	GetClientRect(rect);
	rect.right -= 1;

	int nItems = m_wndList.GetCount();
	int nListBoxHeight = nItems > 0 ? nItems * m_nTextHeight : DEFAULT_IPLISTBOX_HEIGHT;

	if(nListBoxHeight > DEFAULT_IPLISTBOX_HEIGHT)
		nListBoxHeight = DEFAULT_IPLISTBOX_HEIGHT;

	m_wndList.SetWindowPos(NULL, 0, 0, rect.Width(), nListBoxHeight, SWP_NOZORDER|SWP_NOMOVE);
}

void CInPlaceComboBoxImp::OnPaint() 
{
	CPaintDC dc(this);
	
	// Nakresli tlaèítko
	CRect rect;

	GetClientRect(rect);
	rect.left = rect.right - m_nButtonDx;

#if 1
	dc.DrawFrameControl(rect, DFC_SCROLL, m_wndList.IsWindowVisible() ? 
		DFCS_SCROLLDOWN|DFCS_PUSHED : DFCS_SCROLLDOWN);
#else
	dc.DrawFrameControl(rect, DFC_SCROLL, m_wndList.IsWindowVisible() ? 
		DFCS_SCROLLDOWN|DFCS_PUSHED|DFCS_FLAT : DFCS_SCROLLDOWN|DFCS_FLAT);
#endif
}

BOOL CInPlaceComboBoxImp::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;	
}

void CInPlaceComboBoxImp::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDown(nFlags, point);

	CRect rect;
	GetClientRect(rect);

	CRect rectButton(rect);
	rectButton.left = rectButton.right - m_nButtonDx;

	if(rectButton.PtInRect(point))
	{
		int nDoAction = m_wndList.IsWindowVisible() ? SW_HIDE : SW_SHOW;

		m_wndList.ShowWindow(nDoAction);
		InvalidateRect(rectButton, FALSE);

		if(nDoAction == SW_SHOW)
			m_wndList.SetFocus();
	}
}

void CInPlaceComboBoxImp::HideListBox()
{
	m_wndList.ShowWindow(SW_HIDE);

	CRect rectButton;

	GetClientRect(rectButton);
	rectButton.left = rectButton.right - m_nButtonDx;

	InvalidateRect(rectButton, FALSE);
}

LRESULT CInPlaceComboBoxImp::OnSelectionOk(WPARAM wParam, LPARAM /*lParam*/)
{
	HideListBox();

	m_wndEdit.SetFocus();
	SetCurSelToEdit(m_nCurrentSelection = int(wParam));

	GetOwner()->SendMessage(WM_USER_SET_DATA);

	return TRUE;
}

LRESULT CInPlaceComboBoxImp::OnSelectionCancel(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	HideListBox();

	return TRUE;
}

LRESULT CInPlaceComboBoxImp::OnNewSelection(WPARAM wParam, LPARAM /*lParam*/)
{
	int nItems = m_wndList.GetCount();

	if(nItems > 0)
	{
		if(wParam == VK_UP)
		{
			if(m_nCurrentSelection > 0)
				SetCurSel(m_nCurrentSelection - 1);
		}
		else
		{
			if(m_nCurrentSelection < nItems - 1)
				SetCurSel(m_nCurrentSelection + 1);
		}
	}

	return TRUE;
}

void CInPlaceComboBoxImp::SetCurSelToEdit(int nSelect)
{
	CString strText;

	if(nSelect != -1)
		m_wndList.GetText(nSelect, strText);
		
	m_wndEdit.SetWindowText(strText);
	m_wndEdit.SetSel(0, -1); 
}

int CInPlaceComboBoxImp::SetCurSel(int nSelect)
{
	if(nSelect >= m_wndList.GetCount())
		return CB_ERR;

	int nRet = m_wndList.SetCurSel(nSelect);

	if(nRet != -1)
	{
		SetCurSelToEdit(nSelect);
		m_nCurrentSelection = nSelect;
	}

	return nRet;
}

CString CInPlaceComboBoxImp::GetTextData() const
{
	CString strText;

	if(m_nCurrentSelection != -1)
		m_wndList.GetText(m_nCurrentSelection, strText);

	return strText;
}

int CInPlaceComboBoxImp::AddString(LPCTSTR pStrText, DWORD nData)
{
	int nIndex = m_wndList.AddString(pStrText);

	return m_wndList.SetItemData(nIndex, nData);
}

void CInPlaceComboBoxImp::ResetContent()
{
	m_wndList.ResetContent();

	m_nCurrentSelection = -1;
}
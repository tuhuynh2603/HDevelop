// PropertyListCtrl.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Štefan Belopotoèan, StefanBelopotocan@cmpnetmail.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PropertyListCtrl.h"
#include "ApplicationDef.h"

#include "PropertyItem.h"

//#include "UserMessageID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrl

CPropertyItem* CPropertyListCtrl::GetPropertyItem(int nIndex)
{
	return nIndex < GetCount() ? (CPropertyItem*)GetItemData(nIndex) : NULL;
}

void CPropertyListCtrl::SetPropertyItemManger(CPropertyItemManger* pPropertyItemManger)
{
	m_pPropertyItemManger = pPropertyItemManger;
	m_pPropertyItemManger->InitControl(this);
}

#define PROPERTY_LEFT_BORDER 20

void CPropertyListCtrl::GetItemValueRect(CRect& rect)
{
	rect.left += PROPERTY_LEFT_BORDER;
	rect.left += rect.Width() / 2;

	rect.DeflateRect(3, 0, 0, 1);
}

void CPropertyListCtrl::ResetContent()
{
	delete m_pWndInPlaceControl;
	m_pWndInPlaceControl = NULL;
	m_nSelectedItem = -1;

	CListBox::ResetContent();
}

BEGIN_MESSAGE_MAP(CPropertyListCtrl, CListBox)
	//{{AFX_MSG_MAP(CPropertyListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelChange)
	ON_MESSAGE(WM_USER_SET_DATA, OnSetData)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrl message handlers

void CPropertyListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
#if 1
	if(lpDrawItemStruct->itemAction & ODA_FOCUS)
		return;
#endif

	CPropertyItem* pPropertyItem = (CPropertyItem*)lpDrawItemStruct->itemData;
	ASSERT(pPropertyItem != NULL);

	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	// Rámèek
	CRect rect;
	GetClientRect(rect);

	int nLeftBorder = rect.left + PROPERTY_LEFT_BORDER;

	// + / -
	bool bTabItem = ToBool(pPropertyItem->IsKindOf(RUNTIME_CLASS(CPropertyItemCategory)));

	if(bTabItem)
	{
		CRect rcSign(lpDrawItemStruct->rcItem);
		rcSign.right = nLeftBorder;

		//rcSign.DeflateRect(4, 4);

		rcSign.right += 1;
		rcSign.bottom += 1;

		dc.FillRect(rcSign, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
		dc.FrameRect(rcSign, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
		
		CPoint ptCenter(rcSign.CenterPoint());

		// minus		
		dc.MoveTo(ptCenter.x - 4, ptCenter.y);
		dc.LineTo(ptCenter.x + 5, ptCenter.y);

		if(!static_cast<CPropertyItemCategory*>(pPropertyItem)->GetChildrenVisible())
		{
			// plus
			dc.MoveTo(ptCenter.x, ptCenter.y - 4);
			dc.LineTo(ptCenter.x, ptCenter.y + 5);
		}
	}

	// Vertikála v ¾avo
	CPen pen(PS_SOLID, 1, RGB(198, 198, 198));
	CPen* pOldPen = dc.SelectObject(&pen);

	dc.MoveTo(nLeftBorder, rect.top);
	dc.LineTo(nLeftBorder, rect.bottom);

	// Spodná èiara
	rect = lpDrawItemStruct->rcItem;
	rect.left += PROPERTY_LEFT_BORDER;

	int nBottom = rect.bottom - 1;

	dc.MoveTo(nLeftBorder, nBottom);
	dc.LineTo(rect.right, nBottom);

	// Odde¾ovaè ståpcov
	nLeftBorder += rect.Width() / 2;

	dc.MoveTo(nLeftBorder, rect.top);
	dc.LineTo(nLeftBorder, nBottom);

	// Text položky
	rect.left += 1;
	rect.bottom -= 1;
	rect.right = nLeftBorder;

	COLORREF crOldTextColor = dc.GetTextColor();
	COLORREF crOldBkColor = dc.GetBkColor();
	COLORREF crBkColor = crOldBkColor;

	if((lpDrawItemStruct->itemAction | ODA_SELECT) && (lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		crBkColor = ::GetSysColor(COLOR_HIGHLIGHT);
		dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		dc.SetBkColor(crBkColor);
	}
	
	dc.FillSolidRect(&rect, crBkColor);

	rect.left += 5;
	rect.right -= 5;

	CFont* pOldFont = NULL;
	CFont fontLabel;

	if(bTabItem)
	{
		LOGFONT logFont;
		CFont* pFont = GetFont();
		pFont->GetLogFont(&logFont);

		logFont.lfWeight = FW_BOLD;
		fontLabel.CreateFontIndirect(&logFont);

		pOldFont = dc.SelectObject(&fontLabel);
	}

	// Popis položky
	dc.DrawText(pPropertyItem->GetName(), &rect, DT_SINGLELINE|DT_VCENTER);

	dc.SetTextColor(crOldTextColor);
	dc.SetBkColor(crOldBkColor);
	dc.SelectObject(pOldPen);

	if(pOldFont != NULL) {
		dc.SelectObject(pOldFont);
		fontLabel.DeleteObject();
	}

	// Hodnota položky
	rect = lpDrawItemStruct->rcItem;
	GetItemValueRect(rect);

	pPropertyItem->DrawValue(&dc, rect);

	dc.Detach();
}

void CPropertyListCtrl::DoCollapse(CPropertyItemCategory* pPropertyItemTab, int nItem)
{
	ASSERT(pPropertyItemTab->GetChildrenVisible());

	nItem++;

	for(int nNumber = pPropertyItemTab->GetNumberEnabledItems(); nNumber > 0; nNumber--)
		DeleteString(nItem);

	pPropertyItemTab->SetChildrenVisible(false);
}

void CPropertyListCtrl::DoExpand(CPropertyItemCategory* pPropertyItemTab, int& nItem)
{
	POSITION pos = pPropertyItemTab->GetHeadPosition();

	while(pos != NULL)
	{
		CPropertyItem* pPropertyItem = pPropertyItemTab->GetNext(pos);

		if(pPropertyItem->GetEnabled())
			InsertPropertyItem(pPropertyItem, ++nItem);
	}

	pPropertyItemTab->SetChildrenVisible();
}

void CPropertyListCtrl::DoCollapseExpand(int nItem, CPropertyItem* pPropertyItem)
{	
	if(pPropertyItem == NULL)
		pPropertyItem = (CPropertyItem*)GetItemData(nItem);

	ASSERT(pPropertyItem != NULL);

	if(pPropertyItem->IsKindOf(RUNTIME_CLASS(CPropertyItemCategory)))
	{
		if(SetCurrentData())
		{
			delete m_pWndInPlaceControl;
			m_pWndInPlaceControl = NULL;
		}

		CPropertyItemCategory* pPropertyItemTab = static_cast<CPropertyItemCategory*>(pPropertyItem);

		if(pPropertyItemTab->GetChildrenVisible())
			DoCollapse(pPropertyItemTab, nItem);
		else
			DoExpand(pPropertyItemTab, nItem);
	}
}

void CPropertyListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CListBox::OnLButtonDown(nFlags, point);
	SCROLLINFO scrollInfo;
	memset(&scrollInfo, 0, sizeof(SCROLLINFO));
	scrollInfo.cbSize = sizeof(SCROLLINFO);
	scrollInfo.fMask = SIF_ALL;
	GetScrollInfo(SB_VERT, &scrollInfo, SIF_ALL);

	if(point.x <= PROPERTY_LEFT_BORDER)
	{
		DoCollapseExpand(GetCurSel());
	}
}

void CPropertyListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CListBox::OnLButtonDblClk(nFlags, point);

	DoCollapseExpand(GetCurSel());
}

void CPropertyListCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CListBox::OnChar(nChar, nRepCnt, nFlags);

	int nItem = GetCurSel();
	CPropertyItem* pPropertyItem = (CPropertyItem*)GetItemData(nItem);
	ASSERT(pPropertyItem != NULL);

	if(pPropertyItem->IsKindOf(RUNTIME_CLASS(CPropertyItemCategory)))
	{
		bool bChildrenVisible = static_cast<CPropertyItemCategory*>(pPropertyItem)->GetChildrenVisible();

		switch(nChar)
		{
			case '+':
				if(!bChildrenVisible)
					DoExpand((CPropertyItemCategory*)pPropertyItem, nItem);
				break;
			case '-':
				if(bChildrenVisible)
					DoCollapse((CPropertyItemCategory*)pPropertyItem, nItem);
				break;
			// ??? VK_RETURN tu neprichádza ???
			/*case VK_RETURN:
				DoCollapseExpand(nItem, pPropertyItem);
				break;*/
			default:
				;	
		}
	}
	/*else if(nChar == VK_RETURN)
		TRACE("\nEditácia");*/
}

bool CPropertyListCtrl::SetCurrentData()
{
	if(m_pWndInPlaceControl != NULL)
	{
		ASSERT(m_nSelectedItem != -1);

		CPropertyItem* pPropertyItemEdited = (CPropertyItem*)GetItemData(m_nSelectedItem);
		ASSERT(pPropertyItemEdited != NULL);

		pPropertyItemEdited->SetData(m_pWndInPlaceControl);
		m_pPropertyItemManger->OnDataChanged(pPropertyItemEdited, this, m_nSelectedItem);

		return true;
	}

	return false;
}

LRESULT CPropertyListCtrl::OnSetData(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	SetCurrentData();

	return TRUE;
}

void CPropertyListCtrl::OnSelChange()
{
	int nItem = GetCurSel();

	if(m_nSelectedItem != nItem)
	{		
		CPropertyItem* pPropertyItem = (CPropertyItem*)GetItemData(nItem);
		ASSERT(pPropertyItem != NULL);

		CRect rect,rectNew;

		GetItemRect(nItem, rect);
		GetItemValueRect(rect);
		//rect.Height=rect.Height+4;
		rectNew=CRect(rect);
		rectNew.bottom=rectNew.bottom+4;
		pPropertyItem->CreateInPlaceControl(this, rectNew, m_pWndInPlaceControl);

		m_nSelectedItem = nItem;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemManger

CPropertyItemCategory* CPropertyItemManger::AddNewTab(LPCTSTR pStrTabName)
{
	CPropertyItemCategory* pNewTab = new CPropertyItemCategory(pStrTabName);

	m_listPropertyItemTabs.AddTail(pNewTab);

	return pNewTab;
}

void CPropertyItemManger::InitControl(CPropertyListCtrl* pWndPropertyListCtrl)
{
	int nItem = 0;
	POSITION pos = m_listPropertyItemTabs.GetHeadPosition();
	
	while(pos != NULL)
	{
		CPropertyItemCategory* pPropertyItemTab = m_listPropertyItemTabs.GetNext(pos);

		if(pPropertyItemTab->GetEnabled())
		{
			pWndPropertyListCtrl->InsertPropertyItem(pPropertyItemTab, nItem);
			pWndPropertyListCtrl->DoExpand(pPropertyItemTab, nItem);

			nItem++;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemManagerAdaptable

CPropertyItemCategory* CPropertyItemManagerAdaptable::GetCategoryTab(int nIndex)
{
	POSITION pos = m_listPropertyItemTabs.FindIndex(nIndex);
	ASSERT(pos != NULL);

	return m_listPropertyItemTabs.GetAt(pos);
}

void CPropertyItemManagerAdaptable::CheckState(CPropertyListCtrl* pWndPropertyListCtrl, int nFromControlIndex, BYTE nFromPropertyItemID)
{
	bool bDoInsert = false;

	POSITION pos = m_listPropertyItemTabs.GetHeadPosition();
	
	while(pos != NULL)
	{
		CPropertyItemCategory* pPropertyItemTab = m_listPropertyItemTabs.GetNext(pos);
		POSITION posTab = pPropertyItemTab->GetHeadPosition();

		if(bDoInsert)
		{
			if(pPropertyItemTab->GetEnabled())
			{
				pWndPropertyListCtrl->InsertPropertyItem(pPropertyItemTab, ++nFromControlIndex);
				pPropertyItemTab->SetChildrenVisible();
			}
			else
				// Táto záložka nieje prístupná
				continue;
		}
		
		while(posTab != NULL)
		{
			CPropertyItem* pPropertyItem = pPropertyItemTab->GetNext(posTab);

			if(bDoInsert)
			{
				if(pPropertyItem->GetEnabled())
					pWndPropertyListCtrl->InsertPropertyItem(pPropertyItem, ++nFromControlIndex);
			}
			else if(nFromPropertyItemID == pPropertyItem->GetPropertyID())
			{
				// Našiel si položku od ktorej by mali nasledova zmeny
				int nItems = pWndPropertyListCtrl->GetCount();

				if(nItems != nFromControlIndex + 1)
				{
					// Zmaž všetky položky 
					int i = nItems - 1;
					nItems -= (nFromControlIndex + 1);
					
					while(nItems-- > 0)
					{
						pWndPropertyListCtrl->DeleteString(i--);
					}
				}

				bDoInsert = true;
			}
		}
	}
}

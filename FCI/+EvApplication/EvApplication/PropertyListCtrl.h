#if !defined(AFX_PROPERTYLISTCTRL1_H__466FB4EF_6072_11D3_A7E3_006008C8B630__INCLUDED_)
#define AFX_PROPERTYLISTCTRL1_H__466FB4EF_6072_11D3_A7E3_006008C8B630__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyListCtrl.h : header file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Štefan Belopotoèan, StefanBelopotocan@cmpnetmail.com
//
/////////////////////////////////////////////////////////////////////////////

#include "PropertyItem.h"
#include "DebugLog.h"

class CPropertyItemManger;

/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrl

class CPropertyListCtrl : public CListBox
{
	CPropertyListCtrl(const CPropertyListCtrl& d);
	CPropertyListCtrl& operator=(const CPropertyListCtrl& d);

public:
	CPropertyListCtrl();

	// Attributes
	void SetPropertyItemManger(CPropertyItemManger* pPropertyItemManger);
	CPropertyItem* GetPropertyItem(int nIndex);

	// Operations
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	void InsertPropertyItem(CPropertyItem* pPropertyItem, int nIndex = 0);
	void ResetContent();

protected:
	void DoCollapse(CPropertyItemCategory* pPropertyItemTab, int nItem);
	void DoExpand(CPropertyItemCategory* pPropertyItemTab, int& nItem);

	void DoCollapseExpand(int nItem, CPropertyItem* pPropertyItem = NULL);

	void GetItemValueRect(CRect& rect);
	bool SetCurrentData();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyListCtrl)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CPropertyListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyListCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSelChange();
	afx_msg LRESULT OnSetData(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Data
private:
	int m_nSelectedItem;
	CWnd* m_pWndInPlaceControl;

	CPropertyItemManger* m_pPropertyItemManger;

	friend class CPropertyItemManger;
};

inline CPropertyListCtrl::CPropertyListCtrl()
{
	m_nSelectedItem = -1;
	m_pWndInPlaceControl = NULL;
	m_pPropertyItemManger = NULL;
}

inline CPropertyListCtrl::~CPropertyListCtrl()
{
	delete m_pWndInPlaceControl;
}

inline BOOL CPropertyListCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	dwStyle |= LBS_OWNERDRAWFIXED;
	//rect.bottom=rect.bottom+4;
	OutputDebugLogTo(0, TRUE, "Rect Height",rect.bottom);
	CRect rectNew=CRect(rect);
	rectNew.bottom=rectNew.bottom+4;
	OutputDebugLogTo(0, TRUE, "RectNew Height",rectNew.bottom);
	return CListBox::Create(dwStyle, rectNew, pParentWnd, nID);
}

inline void CPropertyListCtrl::InsertPropertyItem(CPropertyItem* pPropertyItem, int nIndex)
{
	ASSERT(pPropertyItem != NULL);
	InsertString(nIndex, (LPCTSTR)pPropertyItem);
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemManger

class CPropertyItemManger
{
	CPropertyItemManger(const CPropertyItemManger& d);
	CPropertyItemManger& operator=(const CPropertyItemManger& d);

public:
	CPropertyItemManger();
	~CPropertyItemManger();

	// Operations
	CPropertyItemCategory* AddNewTab(LPCTSTR pStrTabName);
	void InitControl(CPropertyListCtrl* pWndPropertyListCtrl);

	virtual void OnDataChanged(CPropertyItem* pPropertyItem, CPropertyListCtrl* pWndPropertyListCtrl, int nIndex);

	// Data
protected:
	CMyTypedPtrList<CObList, CPropertyItemCategory*> m_listPropertyItemTabs;
};

inline CPropertyItemManger::CPropertyItemManger()
{
}

inline CPropertyItemManger::~CPropertyItemManger()
{
}

inline void CPropertyItemManger::OnDataChanged(CPropertyItem* /*pPropertyItem*/, CPropertyListCtrl* /*pWndPropertyListCtrl*/, int /*nIndex*/)
{
}

/////////////////////////////////////////////////////////////////////////////

#define BEGIN_PROPERTY_TAB(tabName, enabled) \
{ \
	CPropertyItemCategory* pNewTab = AddNewTab(tabName); \
	pNewTab->SetEnabled(enabled); \
	CPropertyItem* pPropertyItem;
#define PROPERTY_ITEM(id, class_name, itemName, enabled) \
	pPropertyItem = new class_name(itemName); \
	pPropertyItem->SetEnabled(enabled); \
	pPropertyItem->SetPropertyID(id);\
	pNewTab->AddPropertyItem(pPropertyItem);
#define END_PROPERTY_TAB() \
}

#define BEGIN_ITERATE_PROPERTY_ITEMS() \
	POSITION pos = m_listPropertyItemTabs.GetHeadPosition(); \
	while(pos != NULL) \
	{ \
		CPropertyItemCategory* pPropertyItemTab = m_listPropertyItemTabs.GetNext(pos); \
		POSITION posItem = pPropertyItemTab->GetHeadPosition(); \
		while(posItem != NULL) \
		{ \
			CPropertyItem* pPropertyItem = pPropertyItemTab->GetNext(posItem); \
			switch(pPropertyItem->GetPropertyID()) \
			{
#define SET_ITEM_STRING(id, string) \
				case id: \
					static_cast<CPropertyItemString*>(pPropertyItem)->SetData(string); \
					break;
#define GET_ITEM_STRING(id, string) \
				case id: \
					string = static_cast<CPropertyItemString*>(pPropertyItem)->GetData(); \
					break;
#define SET_ITEM_LIST(id, item) \
				case id: \
				static_cast<CPropertyItemList*>(pPropertyItem)->SetData(item); \
					break;
#define GET_ITEM_LIST(id, item) \
				case id: \
					static_cast<CPropertyItemList*>(pPropertyItem)->GetData(item); \
					break;
#define END_ITERATE_PROPERTY_ITEMS() \
				default: ; \
			} \
		} \
	}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemManagerAdaptable

class CPropertyItemManagerAdaptable : public CPropertyItemManger
{
	CPropertyItemManagerAdaptable(const CPropertyItemManagerAdaptable& d);
	CPropertyItemManagerAdaptable& operator=(const CPropertyItemManagerAdaptable& d);

public:
	CPropertyItemManagerAdaptable();
	~CPropertyItemManagerAdaptable();

	// Attributes
	CPropertyItemCategory* GetCategoryTab(int nIndex);

	// Operations
	void CheckState(CPropertyListCtrl* pWndPropertyListCtrl, int nFromControlIndex, BYTE nFromPropertyItemID);
};

inline CPropertyItemManagerAdaptable::CPropertyItemManagerAdaptable()
{
}

inline CPropertyItemManagerAdaptable::~CPropertyItemManagerAdaptable()
{
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYLISTCTRL1_H__466FB4EF_6072_11D3_A7E3_006008C8B630__INCLUDED_)

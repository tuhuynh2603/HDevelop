#if !defined(_INPLACECOMBOBOXIMP_H)
#define _INPLACECOMBOBOXIMP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InPlaceComboBoxImp.h : header file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Štefan Belopotoèan, StefanBelopotocan@cmpnetmail.com
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CInPlaceCBEditImp

class CInPlaceCBEditImp : public CEdit
{
	CInPlaceCBEditImp(const CInPlaceCBEditImp& d);
	CInPlaceCBEditImp& operator=(const CInPlaceCBEditImp& d);

public:
	CInPlaceCBEditImp();
	virtual ~CInPlaceCBEditImp();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceCBEditImp)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceCBEditImp)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

inline CInPlaceCBEditImp::CInPlaceCBEditImp()
{
}

inline CInPlaceCBEditImp::~CInPlaceCBEditImp()
{
}

/////////////////////////////////////////////////////////////////////////////
// CInPlaceCBListBoxImp

class CInPlaceCBListBoxImp : public CListBox
{
	CInPlaceCBListBoxImp(const CInPlaceCBListBoxImp& d);
	CInPlaceCBListBoxImp& operator=(const CInPlaceCBListBoxImp& d);

public:
	CInPlaceCBListBoxImp();
	virtual ~CInPlaceCBListBoxImp();

	// Operations
protected:
	void ProcessSelected();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceCBListBoxImp)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceCBListBoxImp)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CInPlaceComboBoxImp

class CInPlaceComboBoxImp : public CWnd
{
	CInPlaceComboBoxImp(const CInPlaceComboBoxImp& d);
	CInPlaceComboBoxImp operator=(const CInPlaceComboBoxImp& d);

protected:
	DECLARE_DYNAMIC(CInPlaceComboBoxImp)

public:
	CInPlaceComboBoxImp();
	virtual ~CInPlaceComboBoxImp();

	// Attributes
public:
	int GetCurrentSelection() const;
	DWORD GetDWordData() const;
	CString GetTextData() const;

	// Operations
public:
	static CInPlaceComboBoxImp* CreateInPlaceComboBox(CWnd* pWndParent, CRect& rect);

	int SetCurSel(int nSelect);
	int AddString(LPCTSTR pStrText, DWORD nData = 0);

	void ResetContent();
	void ResetListBoxHeight();

	void MoveControl(CRect& rect);

private:
	void SetCurSelToEdit(int nSelect);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceComboBoxImp)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceComboBoxImp)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnSelectionOk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelectionCancel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNewSelection(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	void HideListBox();
	
	// Data
private:
	int m_nButtonDx;
	int m_nTextHeight;

	int m_nCurrentSelection;

	CInPlaceCBEditImp m_wndEdit;
	CInPlaceCBListBoxImp m_wndList;
};

inline CInPlaceComboBoxImp::~CInPlaceComboBoxImp()
{
}

inline int CInPlaceComboBoxImp::GetCurrentSelection() const
{
	return m_nCurrentSelection;
}

inline DWORD CInPlaceComboBoxImp::GetDWordData() const
{
	return (DWORD) (m_nCurrentSelection != -1 ? m_wndList.GetItemData(m_nCurrentSelection) : 0);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_INPLACECOMBOBOXIMP_H)

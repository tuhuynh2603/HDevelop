#if !defined(_INPLACEEDITIMP_H)
#define _INPLACEEDITIMP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InPlaceEditImp.h : header file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Štefan Belopotoèan, StefanBelopotocan@cmpnetmail.com
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEditImp

class CInPlaceEditImp : public CEdit
{
	CInPlaceEditImp(const CInPlaceEditImp& d);
	CInPlaceEditImp& operator=(const CInPlaceEditImp& d);

protected:
	DECLARE_DYNAMIC(CInPlaceEditImp)

public:
	CInPlaceEditImp(const CString& srtInitText);
	virtual ~CInPlaceEditImp();

	// Operations
public:
	static CInPlaceEditImp* CreateInPlaceEdit(const CString& strInitText, CWnd* pWndParent, CRect& rect, DWORD exDwStyle = 0);

	void MoveControl(CRect& rect);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceEditImp)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceEditImp)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Data
private:
	CString m_strInitText;
};

inline CInPlaceEditImp::CInPlaceEditImp(const CString& strInitText)
	: m_strInitText(strInitText)
{
}

inline CInPlaceEditImp::~CInPlaceEditImp()
{
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_INPLACEEDITIMP_H)

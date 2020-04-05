// LineChartCtrl.h : header file
//

#pragma once

#include <afxtempl.h>

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

class CLineChartItem:public CObject
{
public:
	CLineChartItem() {;}

	COLORREF m_colorLine;
	UINT     m_nLower;		// lower bounds
	UINT     m_nUpper;		// upper bounds
	
	UINT     m_nPos;		// current position within bounds
	UINT     m_nOldPos;		// last position within bounds
	BOOL     m_bShow;
};

typedef CTypedPtrArray <CObArray, CLineChartItem*> CItemArray;

/////////////////////////////////////////////////////////////////////////////
// CLineChartCtrl window

class DECLSPEC_WINDOW_IF CLineChartCtrl : public CWnd
{
// Construction
public:
	static BOOL RegisterWndClass(HINSTANCE hInstance);

	CLineChartCtrl();
	UINT m_nVertical;
	BOOL m_bIsDialogBar;

// Attributes
public:
	CItemArray m_items;
	UINT SetPos(int nIndex, UINT nPos);
	void InvalidateCtrl();
	void DrawSpike();
	void DrawSpikeGlobal();

	// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLineChartCtrl)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	void Go();
	BOOL Add(COLORREF color, UINT Upper, UINT Lower );
	BOOL Remove(int nIndex);
	BOOL Modify(int nIndex, COLORREF color, UINT Upper, UINT Lower);
	BOOL SetGlobalBounds(UINT Upper, UINT Lower);
	BOOL ShowLine(int nIndex, BOOL bShow=TRUE);
	virtual ~CLineChartCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLineChartCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CDC      m_MemDC;
	CBitmap  m_Bitmap;
	UINT m_nGlobalUpper;
	//UINT m_nGlobalUpperCount;
	//UINT m_nGlobalLower;
};

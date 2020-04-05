/// PanWindow.h : interface of the CPanWindow class
/// Udupa

#pragma once

#include "resource.h"
class CEvImageView;

class CPanWindow : public CDialog
{
// Construction
public:
	CPanWindow(CWnd* pParent = NULL);   // standard constructor
	~CPanWindow();

	CEvImageView* m_pParent;
// Dialog Data
	//{{AFX_DATA(CPanWindow)
	enum { IDD = IDD_PAN_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPanWindow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
protected:
	bool m_bIsPanning;
	double m_dScaleShift;
	CRect m_rectFocus;
	CBrush m_brush;

	// Generated message map functions
	//{{AFX_MSG(CPanWindow)
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

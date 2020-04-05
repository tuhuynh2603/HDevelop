#pragma once

/////////////////////////////////////////////////////////////////////////////
// CMagnifierWnd window

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

class DECLSPEC_WINDOW_IF CMagnifierWnd : public CWnd
{
// Construction
public:
	CMagnifierWnd();
	void ZoomIn();
	void ZoomOut();

	int m_nZoomValue;
	CPen m_hPen;
	CImageBuffer m_bufMagnifier;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMagnifierWnd)
	public:
	virtual BOOL Create(int size, CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMagnifierWnd();

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	// Generated message map functions
protected:
	//{{AFX_MSG(CMagnifierWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

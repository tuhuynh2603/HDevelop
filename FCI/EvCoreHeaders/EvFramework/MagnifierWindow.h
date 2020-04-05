#pragma once

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

class DECLSPEC_WINDOW_IF CMagnifierWindow : public CWnd
{
// Construction
public:
	CMagnifierWindow();
	~CMagnifierWindow();
	void ZoomIn();
	void ZoomOut();
	void AllocateBuffer(BOOL bColor);
	void SetImage(CImageBuffer* pSourceBuf, CRect* pRect);

	int m_nZoomValue;
	CPen m_hPen;
	CPen m_hOutlinePen;
	CImageBuffer m_bufMagnifier;

// Attributes
protected:
	CRect m_rectSource;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMagnifierWindow)
	public:
	virtual BOOL Create(int size, CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void CopyFromColorTable(CImageBuffer* pImgDisplaySource, CDisplayAdapter *pDisplayAdapterDest);

	// Generated message map functions
protected:
	//{{AFX_MSG(CMagnifierWindow)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

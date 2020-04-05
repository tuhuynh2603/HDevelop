#pragma once

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

#include "ChildView.h"

// EvImageZoomView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEvImageZoomView view

class DECLSPEC_WINDOW_IF CEvImageZoomView : public CChildView
{
protected:
	CEvImageZoomView();           // protected constructor used by dynamic creation
	DECLARE_DYNAMIC(CEvImageZoomView)

// Attributes
public:
    enum {SEC_ZOOMOFF, SEC_ZOOMPANOVER, SEC_ZOOMNORMAL, SEC_ZOOMFIT};

// Operations
public:
	void ClientToDevice(CPoint &point);
	void ViewLPtoDP(LPPOINT lpPoints, int nCount = 1);
	void ViewDPtoLP(LPPOINT lpPoints, int nCount = 1);
	virtual void ZoomPercent(int nPercent = 25, CPoint *point = NULL);
	virtual void ZoomFit();
	virtual void ZoomOriginal(CPoint *point = NULL);
	virtual void ZoomOut(CPoint *point = NULL, float delta = 1.25);
	virtual void ZoomIn(CPoint *point = NULL, float delta = 1.25);
	virtual void ZoomIn(CRect &rect);
	void SetZoomMode(UINT zoomMode);
	void SetZoomMinMaxPercent(int nMin, int nMax);
	void SetZoomMinMax(float fMin, float fMax);
	virtual void SetZoomLevel(float fNewScale, CPoint* ptCentre=NULL);
    float GetZoomLevel() {return m_fZoomScale;};
	UINT GetZoomMode() {return m_zoomMode;};
	CPoint GetLogicalCenterPoint(void);
	void CenterOnLogicalPoint(CPoint ptCenter);
	void SetScrollSizes(int nMapMode,
			  SIZE sizeTotal, const SIZE& sizePage = sizeDefault,
			  const SIZE& sizeLine = sizeDefault);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEvImageZoomView)
	public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnDraw(CDC* pDC) = 0;      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateViewport(CPoint * ptNewCenter);
	void ReCalcBars(void);
	void PersistRatio(const CSize &orig, CSize &dest, CPoint &remainder);
	virtual void ZoomOutOfRange(BOOL bUnderFlow, float fAttempt);
	virtual void ZoomLevelChanged(void) { };
	virtual ~CEvImageZoomView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CEvImageZoomView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	UINT m_zoomMode;	      // The current zoom mode: normal/off/zoom to fit
    CSize m_szOrigTotalDev;	  // Origional total size in device units
    CSize m_szOrigPageDev;    // Origional page scroll size in device units
    CSize m_szOrigLineDev;    // Origional line scroll size in device units
    float m_fZoomScale;       // Current ratio between device/logical units
    float m_fZoomMin;
    float m_fZoomMax;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

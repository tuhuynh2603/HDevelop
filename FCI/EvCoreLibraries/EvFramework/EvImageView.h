#pragma once

#include "EvImageZoomView.h"  
#include "MsgCode.h"
#include "resource.h"
#include "ImageDisplayAdapter.h"
#include "EvImageDoc.h"
#include "DisplayOverlay.h"
#include "MagnifierWindow.h"

class CThresholdDialog;

typedef struct {
	CRect rect;							// Rectangle Position
	LOGPEN PenInfo;						// Rectangle Style
	int nTrackerType;
} TRACK_INFO;

typedef struct {
	CRect rect;							// Rectangle Position
	LOGPEN PenInfo;						// Rectangle Style
} CIRCLE_INFO;

typedef struct {
	int x, y;
	CString str;
	int C_Interval, L_Interval, m_String_Size;
	LOGPEN BkColor, TextColor;			// String Style
}STR_INFO;

class CEvImageView;

class CVisRectTracker : public CRectTracker
{
public:
	CVisRectTracker(CEvImageView* pView = NULL);
	virtual ~CVisRectTracker();
	CEvImageView* m_pView;

protected:
	CSize		 m_sizeImage;
	virtual void OnChangedRect(const CRect& rectOld);
};

class CPanWindow;

#ifdef _WINDOW_IF
#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

class DECLSPEC_WINDOW_IF CEvImageView : public CEvImageZoomView
{
protected: // create from serialization only
	CEvImageView();
	DECLARE_DYNCREATE(CEvImageView)
	CPanWindow* m_pPanDlg;
	BOOL m_bFramed;
	BOOL m_bPanning;
	BOOL m_bPixelRuler;
	CPoint m_ptPanStart;
	CPoint m_ptStartScrollPos;

	// Attributes
public:
	CVisRectTracker m_Tracker;				// Tracker for user to resize Rectangles
	CThresholdDialog *m_pdlgThres;				// Binary Threshold Adjust Dlg Box

	CArray<CUR_INFO, CUR_INFO> m_DrwCurAry;	// Array to store the cursors to draw
	CArray<RECT_INFO, RECT_INFO> m_DrwRectAry;	// Array to store the Rectangles to draw
	CArray<CIRCLE_INFO, CIRCLE_INFO> m_DrwCircleAry;	// Array to store the Circle to draw
	CArray<LINE_INFO, LINE_INFO> m_DrwLineAry;	// Array to store the Lines to draw
	CArray<TRACK_INFO, TRACK_INFO> m_RectTrackAry;		// Array to store the Track Rectangles to draw
	CArray<STR_INFO, STR_INFO> m_DrwStringAry;//Array to store Strings to be drawn on screen
	CArray<STR_ATTR, STR_ATTR> m_DrwStrAry;	//Array to store Strings to be drawn on screen(New)
											// Operations
public:
	CEvImageDoc* GetDocument();
	CPoint m_ptMagnifier;
	CMagnifierWindow m_MagnifierWindow;
	CPoint m_ptMagnifierOffset;
	void RefreshMagnifier();
	BOOL m_bMagnifierView;
	HCURSOR m_hMagnifierCursor;
	HCURSOR m_hArrowCursor;

	CPoint m_ptPixelRulerStart;
	CPoint m_ptPixelRulerEnd;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEvImageView)
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct
									//}}AFX_VIRTUAL

									// Implementation
public:
	CSize GetImageSize();
	void GetDeviceRect(CRect &rectImage);
	void GetImageRect(CRect &rectDevice);
	BOOL m_bStretch;
	void UpdateTrackRectPos(CRect &rect, BOOL bConvert = TRUE);
	virtual void UpdateStatusBar(CString* strMessage, int nPane = 0);
	virtual ~CEvImageView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual void FitToWindow();

protected:
	void DrawCursorNow(CDC* pDC, CUR_INFO *pCurInfo);	// Draw Cursor
	void DrawCircleNow(CDC* pDC, CIRCLE_INFO *pCircleInfo);	// Draw Rectangle
	void DrawStringNow(CDC* pDC, STR_INFO *pStrInfo, int);   //Draw String

															 // Generated message map functions
protected:
	//{{AFX_MSG(CEvImageView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	//}}AFX_MSG
public:
	bool teachRect;
	void DrawTeachRegion(CRect &rect, bool flag);
	afx_msg LRESULT SetGrayPallette(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ZoomIn(WPARAM wParam = 0, LPARAM lParam = 0);			// Zoom In the Image
	afx_msg LRESULT ZoomOut(WPARAM wParam = 0, LPARAM lParam = 0);			// Zoom Out the Image
	afx_msg LRESULT ZoomFit(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg LRESULT ZoomInRect(CRect rect);			// Zoom Out the Image
	afx_msg LRESULT ViewDrwLine(WPARAM wParam = 0, LPARAM lParam = 0);		// Add a line to m_DrwLineAry
	afx_msg LRESULT ToggleMagnifier(WPARAM wParam = 0, LPARAM lParam = 0);

protected:
	afx_msg LRESULT BinaryMode(WPARAM wParam = 0, LPARAM lParam = 0);		// Switch Image from Gray to Binary
	afx_msg LRESULT OnSetThreshold(WPARAM wParam = 0, LPARAM lParam = 0);	// Change Threshold Value

	afx_msg LRESULT ViewDrwDel(WPARAM wParam = 0, LPARAM lParam = 0);		// Delete All Graphics
	afx_msg LRESULT ViewDrwUpd(WPARAM wParam = 0, LPARAM lParam = 0);		// Redraw the View now
	afx_msg LRESULT ViewDrwCur(WPARAM wParam = 0, LPARAM lParam = 0);		// Add a cursor to m_DrwCurAry
	afx_msg LRESULT ViewCurDel(WPARAM wParam = 0, LPARAM lParam = 0);		// Del a cursor fm m_DrwCurAry
	afx_msg LRESULT ViewLineDel(WPARAM wParam = 0, LPARAM lParam = 0);		// Del a line fm m_DrwLineAry
	afx_msg LRESULT ViewDrwRect(WPARAM wParam = 0, LPARAM lParam = 0);		// Add a Rect to m_DrwRectAry
	afx_msg LRESULT ViewDrwCircle(WPARAM wParam = 0, LPARAM lParam = 0);		// Add a Circle to m_DrwRectAry
	afx_msg LRESULT ViewDrwTrackRect(WPARAM wParam = 0, LPARAM lParam = 0);	// Add a Track Rect to m_RectTrackAry
	afx_msg LRESULT ViewDrwTrackCross(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg LRESULT ViewDrwString(WPARAM wParam = 0, LPARAM lParam = 0);	// Add a String to m_DrwStringAry
	afx_msg LRESULT ViewDrwStr(WPARAM wParam = 0, LPARAM lParam = 0);		// (New)Add a String to m_DrwStringAry
	afx_msg LRESULT ViewDeleteString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ViewDelRect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ViewDelCircle(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ViewSetScrollSize(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT ViewUpdImg(WPARAM wParam = 0,		// Pointer to ImageID
		LPARAM lParam = 0);	// Not Used
	afx_msg LRESULT ViewRestorePrevImg(WPARAM wParam = 0,
		LPARAM lParam = 0);
	void GetMagnifierDrwPos(CPoint ptMouse, CPoint *ptDraw);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in VisionView.cpp
inline CEvImageDoc* CEvImageView::GetDocument()
{
	return (CEvImageDoc*)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

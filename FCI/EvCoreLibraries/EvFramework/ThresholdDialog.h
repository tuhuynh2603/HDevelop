#pragma once
#include "resource.h"
enum _RangeMessages {
	RS_LEFTCHANGED,
	RS_RIGHTCHANGED,
	RS_BOTHCHANGED,
};

class CThresholdDialog : public CWnd
{
// Construction
public:
	CThresholdDialog(CWnd* pParent = NULL);   // standard constructor
	~CThresholdDialog();

	void CreateNew(CView* pView);

	void IncThresholdValue(int nStep=0);
	void DecThresholdValue(int nStep=0);

	struct ThresholdValue {
		int nLow;
		int nHigh;
	}m_nThresholdValue;
	

	CBrush m_BrushActive;

	// Dialog Data
	//{{AFX_DATA(CThresholdDialog)
	enum { IDD = IDD_THRESHOLD_DLG };
	CScrollBar m_sliderBinarize;
	CEdit m_editBinarizeLow;
	CEdit m_editBinarizeHigh;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThresholdDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL RegisterWindowClass();
	CView* m_pView;
	// Generated message map functions
	//{{AFX_MSG(CThresholdDialog)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_nArrowWidth;
	int m_nRangeWidth;
	int m_nRangeHeight;
	double m_dMin, m_dMax;           // Outer Edges of the Control
	double m_dLeft, m_dRight;        // Position of Arrows
	BOOL m_bTracking;           // Follow Mouse Input if T
	enum _TrackMode {
		TRACK_LEFT,             // Left Arrow is being slided
		TRACK_RIGHT,            // Right Arrow is being slided
		TRACK_MIDDLE,           // Middle Area is being slided
	} m_TrackMode;
	CRect m_RectLeft, m_RectRight; // Rectangles of the Left and Right Arrow
	CPoint m_ClickOffset;
public:
	void OnPaintSlider2Thump(CDC &dc);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	LRESULT OnRangeChange(WPARAM wParam, LPARAM /* lParam */);
	void OnEnChangeEditLow();
	void OnEnChangeEditHigh();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

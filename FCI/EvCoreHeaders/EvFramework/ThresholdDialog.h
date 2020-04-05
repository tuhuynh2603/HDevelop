#pragma once
#include "resource.h"

class CThresholdDialog : public CWnd
{
// Construction
public:
	CThresholdDialog(CWnd* pParent = NULL);   // standard constructor
	~CThresholdDialog();

	void CreateNew(CView* pView);

	void IncThresholdValue(int nStep=0);
	void DecThresholdValue(int nStep=0);

	int m_nThresholdValue;

	CBrush m_BrushActive;

	// Dialog Data
	//{{AFX_DATA(CThresholdDialog)
	enum { IDD = IDD_THRESHOLD_DLG };
	CScrollBar m_sliderBinarize;
	CStatic m_staticBinarize;
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
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

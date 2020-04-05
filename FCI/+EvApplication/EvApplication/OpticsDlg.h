#pragma once

#include "resource.h"
#include "LineChartCtrl.h"

struct OPTICS_PARM_DLG {
	int nIndex;
	double dMean;
	double dSNR;
	double dFocus;
	BYTE nCalcMode;
};

class COpticsDlg : public CDialogBar
{
// Construction
public:
	LRESULT SetParameters(WPARAM wParam, LPARAM lParam);
	COpticsDlg(CWnd* pParent = NULL);   // standard constructor
	~COpticsDlg();

	BOOL Create( CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle,
		UINT nID, BOOL = TRUE);
	BOOL Create( CWnd* pParentWnd, LPCTSTR lpszTemplateName,
		UINT nStyle, UINT nID, BOOL = TRUE);
	
	LRESULT RefreshGraph(WPARAM wParam, LPARAM lParam);
	void SetStats(int nFrames, int nFocusImages, int nTopOffset,int nBottomOffset);
	void GetStats(int& nFrames, int& nFocusImages, int& nTopOffset,int& nBottomOffset);
	void OnResizeFrames(int& nFrames, int& nFocusImages, int& nTopOffset,int& nBottomOffset);
	void DisplayMessage(bool m_nFlag);
//protected:

	double m_dSNRAccumulator[8];
	double m_dFocusAccumulator[8];
	double m_dMeanAccumulator[8];
	int m_nAccCount[8];
	int m_nFullWindowHeight;
	COLORREF m_clrROI[8];

	void RepositionWindow(int nFrames);
	void EnableEditControls();

// Dialog Data
	//{{AFX_DATA(COpticsDlg)
	enum { IDD = IDD_OPTICS_DLGBAR };
	CComboBox	m_ctrlFrameCount;
	int		m_nAvSize;
	int m_nBottomOffset;
	int m_nTopOffset;
	int m_nCircular;
	

	int m_nCurrentTrack;
	int m_nCurrentDoc;
	double m_dAutoFocusStepSizeMM;
	int m_nAutoFocusStepNo;
	BOOL m_bAutoFocusing;

	//}}AFX_DATA
	bool m_bCheckROI;
	double	m_dOpticsMean[8];
	double	m_dOpticsSNR[8];
	double	m_dOpticsFocus[8];

	CSize m_sizeDocked;
	CSize m_sizeFloating;
	BOOL m_bChangeDockedSize;   // Indicates whether to keep
	// a default size for docking
	
	CLineChartCtrl m_wndLineChart;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpticsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual CSize CalcDynamicLayout( int nLength, DWORD dwMode );

// Implementation
protected:
	CBrush m_brush1;
	CBrush m_brush2;
	BOOL OnInitDialogBar();

	// Generated message map functions
	//{{AFX_MSG(COpticsDlg)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRadio2();
	afx_msg void OnRadio1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAutoFocusing();
	afx_msg void OnUpdateButtonAutoFocusing(CCmdUI *pCmdUI);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

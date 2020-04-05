#pragma once
#include "resource.h"
#include "LineChartCtrl.h"
#include "AppDoc.h"

struct OPTICS_PARM {
	int nIndex;
	double dMean;
	double dSNR;
	double dFocus;
	BYTE nCalcMode;
};

// COpticsDialog dialog

class COpticsDialog : public CDialogEx
{
	DECLARE_DYNAMIC(COpticsDialog)

public:
	COpticsDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~COpticsDialog();
	CAppDoc* m_pDoc;

// Dialog Data
	enum { IDD = IDD_OPTICS_DIALOG };

	LRESULT SetParameters(WPARAM wParam, LPARAM lParam);

	LRESULT RefreshGraph(WPARAM wParam, LPARAM lParam);
	void SetStats(int nFrames, int nFocusImages, int nTopOffset,int nBottomOffset);
	void GetStats(int& nFrames, int& nFocusImages, int& nTopOffset,int& nBottomOffset);
	void OnResizeFrames(int& nFrames, int& nFocusImages, int& nTopOffset,int& nBottomOffset);
	void DisplayMessage(bool m_nFlag);

	double m_dSNRAccumulator[8];
	double m_dFocusAccumulator[8];
	double m_dMeanAccumulator[8];
	int m_nAccCount[8];
	int m_nFullWindowHeight;
	COLORREF m_clrROI[8];

	void RepositionWindow(int nFrames);
	void EnableEditControls();

	CComboBox	m_ctrlFrameCount;
	int		m_nAvSize;
	int m_nBottomOffset;
	int m_nTopOffset;
	int m_nCircular;
	
	//}}AFX_DATA
	bool m_bCheckROI;
	double	m_dOpticsMean[8];
	double	m_dOpticsSNR[8];
	double	m_dOpticsFocus[8];

	CSize m_sizeDocked;
	CSize m_sizeFloating;
	BOOL m_bChangeDockedSize;   // Indicates whether to keep
	//CLineChartCtrl m_wndLineChart;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CBrush m_brush1;
	CBrush m_brush2;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRadio2();
	afx_msg void OnRadio1();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeFramecountCombo();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

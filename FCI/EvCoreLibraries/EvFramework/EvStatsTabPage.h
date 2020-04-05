/// EvStatsTabPage.h : interface of the CEvStatsTabPage
/// Udupa; Nov'2012

#pragma once

#include "resource.h"
#include "atltime.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "Controls\\BarChart.h"
// EvTabPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEvStatsTabPage dialog

struct CChartInfo
{
	CString strTitleText;
	CString strListText;

	char strChartTypes[3][20];
	char strTypeAlias[20];
	COLORREF colorBar[3];
	BOOL bTrackWise;
	int nEntryCount;

	void Initialize(char* pstrTitleText, char* strCharType1, COLORREF colorBar1, BOOL bTrackWise=TRUE, char* pstrListText=NULL, 
		char* strCharType2=NULL, COLORREF colorBar2=0, char* strCharType3=NULL, COLORREF colorBar3=0);
};

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

class DECLSPEC_WINDOW_IF CEvStatsTabPage : public CDialog
{
// Construction
public:
	CEvStatsTabPage(CWnd* pParent = NULL);   // standard constructor
	~CEvStatsTabPage();

	CBrush m_brush;
	CFont m_font;

// Dialog Data
	//{{AFX_DATA(CEvStatsTabPage)
	enum { IDD = IDD_STATSCHILD_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEvStatsTabPage)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTvnSelchangedStatsDatatree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeStatsStartDatetimepicker(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeStatsEndDatetimepicker(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CEvStatsTabPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	HTREEITEM m_itemRoot;
	int m_nFailTypes;
	CChartInfo m_ChartInfo[11];

public:
	int m_nImageIndex;
	int m_nCamera;
	CTime m_StartDate;
	CTime m_EndDate;
	int m_nChartType;

	CString* m_pstrLotFolder;

	void SetStatsType(int nType);
	void AdjustLayout();
	void PopulateDataTree();
	void DataTreeChanged(LPNMTREEVIEW pNMTreeView);
	void Plot();


	CTreeCtrl m_treeData;
	CEdit m_editData;
	CBarChart m_chartControl;
	CListBox m_listChartType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

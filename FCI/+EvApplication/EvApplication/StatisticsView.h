#if !defined(AFX_STATISTICSVIEW_H__8A4D0602_18EB_408E_AF4C_403FCA56C51B__INCLUDED_)
#define AFX_STATISTICSVIEW_H__8A4D0602_18EB_408E_AF4C_403FCA56C51B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StatisticsView.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CStatisticsView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CStatisticsView : public CDialogEx
{
public:
	CStatisticsView(CWnd* pParent = NULL);           // protected constructor used by dynamic creation
protected:
	DECLARE_DYNCREATE(CStatisticsView)

	void Initialize(BOOL bRefresh = FALSE);
	void  AddItem(CListCtrl* pCtrl, CString strItem, int nIndex);
	void AddColumn(CListCtrl* pCtrl, CString strHeader, int nIndex, int nWidth);
	void UpdateContents(BOOL bRefresh = TRUE);
	void UpdateTotalContents();
	void RefreshDefectsList();
	void InitializeLotStats();

	void UpdateCentralizedSummary(int nStationId);
	void UpdateCentralizedMachineState(int nStationId);
	void UpdateCentralizedMachineState(int nStationId, COLORREF colorState);
	void UpdateCentralizedMachineID(int nStationId);
	void UpdateCentralizedContents();

	//void RecordLotStats(BOOL bForced=FALSE);
	//void UpdateLotHourlyInfo();
	//void UpdateLotDailyInfo();
	void UpdateLotInfo();
	void UpdateTotalLotInfo();
	void UpdateLotInfoMID(int nDie);

	void CreateHourlyFolder(SYSTEMTIME sysTime);
	void CheckAutoLogOff();
	void LoadLotInfo();
	void LoadTotalLotInfo();
	void UpdateStatsInfo();
	BOOL m_bInitialized;
	CString m_strHourlyFolder;
	CString m_strLotFolder;
	CFont m_fontList;
	COLORREF m_colorInfo1[100][20];
	COLORREF m_colorInfo2[70][20];
	COLORREF m_colorInfo3[70][20];
	COLORREF m_colorInfo4[500][20];

	COLORREF m_colorInfo5[70][20];

	int m_nTotalStaticDefectsCount;
	//CBrush m_brush;
	//CFont m_font;
	

// Form Data
public:
	//{{AFX_DATA(CStatisticsView)
	enum { IDD = IDD_STATISTICS_FORMVIEW };
	/*CListCtrl	m_ctrlSummaryList;
	CListCtrl	m_ctrlDefectsList;*/
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	BOOL m_bWriteFirstLot;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatisticsView)
	public:
	virtual void OnInitialUpdate();
	virtual ~CStatisticsView();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	double dAverageDiesTimeMs;
	int nPPMDiesNo;
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CStatisticsView)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnCustomdrawList1(NMHDR*, LRESULT*);
    afx_msg void OnCustomdrawList2(NMHDR*, LRESULT*);
	afx_msg void OnCustomdrawList3(NMHDR*, LRESULT*);
	afx_msg void OnCustomdrawList4(NMHDR*, LRESULT*);
	afx_msg void OnCustomdrawList5(NMHDR*, LRESULT*);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CBitmapButton m_ctrlMachineStopButton;

public:
	afx_msg void OnLvnItemchangedSummaryList(NMHDR *pNMHDR, LRESULT *pResult);
	CRITICAL_SECTION m_csLogFile;
	int UpdateAuditTrail(CString strEvent, BOOL bSave);
	//BOOL m_bAuditTrailValidated;
	/*CTabCtrlSSL m_TabControl;

	CSummarList m_TabDefectList;
	CSummarList m_TabSummaryList;*/

	afx_msg void OnBnClickedButtonSummaryList();
	afx_msg void OnBnClickedButtonDefectList();
	CListCtrl m_DefectList;
	CListCtrl m_SummaryList;
	CListCtrl m_ListStatsInfo;
	afx_msg void OnBnClickedButtonStatsInfo();

	CComboBox m_comboDeviceList;
	afx_msg void OnCbnSelchangeComboDeviceList();
	CListCtrl m_listTotalInfo;
	afx_msg void OnBnClickedButtonTotalInfo();
	CComboBox m_comboPositionList;
	afx_msg void OnCbnSelchangeComboPositionList();
	void UpdateUPH(double dStripTime, int nDieNo);
	CRITICAL_SECTION m_CriticalSectionUpdate;
	afx_msg void OnBnClickedOnlineResetstatscounter();
	afx_msg void OnBnClickedButtonStopMachine();
	
	CListCtrl m_listCentralizedInfo;


};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif; // !defined(AFX_STATISTICSVIEW_H__8A4D0602_18EB_408E_AF4C_403FCA56C51B__INCLUDED_);
#include "afxwin.h"
#include "afxcmn.h"



class CDialogPaneStatsview : public CDockablePane
{
	DECLARE_DYNAMIC(CDialogPaneStatsview)
	DECLARE_MESSAGE_MAP()
public:
	CDialogPaneStatsview();
	BOOL bIsClosed;
	BOOL bRefreshPane;
	virtual ~CDialogPaneStatsview();
	CStatisticsView m_wndStatsDlg;
	virtual void OnPressCloseButton();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lp);
	afx_msg void OnSize(UINT nType,int cx,int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
};
/// EvMainFrame.h : interface of the CEvMainFrame class
/// Udupa; April'2012

#pragma once

#include "RibbonControl.h"
#include "EvPropertiesWnd.h"
#include "EvTeachPropertiesWnd.h"
#include "EvDefectPropertiesWnd.h"
#include "CalendarBar.h"
#include "EvHistoryImagePane.h"
#include "OutputWnd.h"

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

class CEvTrackHandler;
class CEvDebugViewPane;

enum CategoryType
{
	CategoryType_MainView,
	CategoryType_Statistics,
	CategoryType_MessageLogger,
	CategoryType_ImageHistory,
	CategoryType_Multicast,
	CategoryType_CpuLoad,
	CategoryType_None
};

class CFolderDialog : public CFileDialog
{
public:
	explicit CFolderDialog(CString strTitle, CString strInitialFolder) : CFileDialog(TRUE, NULL, NULL, NULL)
	{
		//m_ofn.lpstrTitle = strTitle.GetBuffer(0);
		m_ofn.lpstrInitialDir = strInitialFolder;
		m_bPickFoldersMode = TRUE;
	}
};

class CEvRibbonCategory
{
public:
	int nIndex;
	int nCategoryType;
	CMFCRibbonCategory* pCategory;

	CEvRibbonCategory(CMFCRibbonCategory* pCat, int nIdx, int nCatType)
	{
		pCategory = pCat;
		nIndex = nIdx;
		nCategoryType = nCatType;
	}
};

class COutlookBar : public CMFCOutlookBar
{
	virtual BOOL AllowShowOnPaneMenu() const { return TRUE; }
	virtual void GetPaneName(CString& strName) const { strName = "Outlook Bar"; }
};

BOOL DECLSPEC_WINDOW_IF IsSingleInstance(CString strId);
CString DECLSPEC_WINDOW_IF GetCurrentVersion(CString strApp);

class DECLSPEC_WINDOW_IF CEvMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CEvMainFrame)
public:
	CEvMainFrame();
	void LoadDefaults(CString strRegPath);
	CArray<CEvRibbonCategory*, CEvRibbonCategory*> m_arrRibbonCategories;
	int m_nActiveCategory;

// Attributes
public:
	CRibbonControl     m_wndRibbonBar;
	CMFCRibbonStatusBar  m_wndStatusBar;
	BOOL m_bFullScreen;
	void InitializeProperties(CEvImageDoc* pDoc, int nFOV);
	void ShowProperties(CEvImageDoc* pDoc, int nFOV);
	void InitializeTeachProperties(CEvImageDoc* pDoc, int nFOV);
	void ShowTeachProperties(CEvImageDoc* pDoc, int nFOV);
	void InitializeDefectProperties();
	void ShowDefectProperties();
	void HideParmPanes();
	void ViewFullScreen(BOOL bFullScreen, BOOL bOnlineMode=FALSE);
//	int AddNewTab();
	void PushHistoryBuffer(int nCamera, CImageBuffer* pBuffer);

	CEvTrackHandler** m_pTrackHandlers;
	int m_nTracksCount;
	CString EV_REGPATH;
	void Initialize(CEvTrackHandler** pTracks, int nTracks);
	CString m_strLotFolder;

	BOOL bIsPropertiesVisiblePane;
	BOOL m_bIsMaximized;
	CEvPropertiesWnd* m_pwndProperties;
	COutputWnd *m_wndOutputPane;
///	CEvDebugViewPane* m_pDebugViewPane;
	/*CEvPropertiesWnd* m_pwndTeachProperties;*/
	CEvTeachPropertiesWnd* m_pwndTeachProperties;
	BOOL bIsPropertiesVisiblePaneTeach;
	BOOL bIsOutputPaneVisible;
	CEvDefectPropertiesWnd* m_pwndDefectProperties;
	BOOL bIsPropertiesVisiblePaneDefect;

protected:
	
	CArray<CEvTabPane*, CEvTabPane*> m_arrTabWnds;
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	CMFCRibbonStatusBarPane* m_StatusPanes[4];


// Operations
public:


// Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEvMainFrame)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEvMainFrame();
	void Maximize();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void GetCanvasRect(CRect& rect);

	int ShowFailedImage(int nCamera, int nImageIndex);
	BOOL m_bRemoteClosed;

protected:  // control bar embedded members
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	///CImageHistoryWnd  m_wndOutput;
	COutlookBar       m_wndNavigationBar;
	CMFCShellTreeCtrl m_wndTree;
	CCalendarBar      m_wndCalendar;
	CMFCCaptionBar    m_wndCaptionBar;

	void CreateStatsPane();
	void ShowTaskBar(BOOL bShow);

// Generated message map functions
protected:
	//{{AFX_MSG(CEvMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg LRESULT OnUpdateStatusBar(WPARAM wParam=0, LPARAM lParam=0);	// Display a Message on the StatusBar
	afx_msg void OnWindowManager();
	afx_msg void OnViewCaptionBar();
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAddNewTab();
	afx_msg void OnUpdateAddNewTab(CCmdUI* pCmdUI);

	void CreateNewTab(CEvTabPane* pWnd);
	BOOL ActivateExisting(int type);

	void SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, CMFCShellTreeCtrl& tree, CCalendarBar& calendar, int nInitialWidth);
	BOOL CreateCaptionBar();

	int FindFocusedOutlookWnd(CMFCOutlookBarTabCtrl** ppOutlookWnd);

	CMFCOutlookBarTabCtrl* FindOutlookParent(CWnd* pWnd);
	CMFCOutlookBarTabCtrl* m_pCurrOutlookWnd;
	CMFCOutlookBarPane*    m_pCurrOutlookPage;

	afx_msg void OnOrbButton();
	afx_msg void OnOrbFullScreen();
	afx_msg void OnUpdateOrbFullScreen(CCmdUI* pCmdUI);
	

	BOOL CreateDockingWindows();

	virtual void InitializeApp() = 0;
	virtual int CreateRibbon() = 0;
	virtual void ResizeChildren();
	virtual BOOL OnCloseMiniFrame(CPaneFrameWnd* pWnd);
	virtual void SetApplicationLook(int nIndex);
	virtual void ShowMainView(BOOL bShow) = 0;
	virtual CDockablePane* GetDebugPane()=0;
	virtual void ToggleMulticast() = 0;
	virtual BOOL IsNewTabAvailable() = 0;
	virtual void  OnUpdateRibbonTab() = 0;
	BOOL m_bEnableMulticast;

public:
	/*afx_msg void OnShowOutputLog();
	afx_msg void OnUpdateShowOutputLog(CCmdUI *pCmdUI);*/
	afx_msg void OnNewwindowImageHistory();
	afx_msg void OnUpdateNewwindowImageHistory(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNewWindowMulticast(CCmdUI *pCmdUI);
	afx_msg void OnNewwindowCpuLoad();
	afx_msg void OnUpdateNewwindowCpuLoad(CCmdUI *pCmdUI);
	afx_msg void OnSelchangeSourceImageCombo();
	afx_msg void OnSelchangeHistoryDepthCombo();
	afx_msg void OnCheckMulticastEnable();
	afx_msg void OnShowOutputLog();
	afx_msg void OnUpdateShowOutputLog(CCmdUI *pCmdUI);


	void InitOutputLog();

	void ShowHideDebugPane(BOOL bShow);
	void OnNewwindowMulticast(BOOL bActivate=TRUE);
	LRESULT OnRibbonCategoryChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnPaneClosed(WPARAM wParam, LPARAM lParam);
	LRESULT OnMessagePushHistoryBuffer(WPARAM wParam, LPARAM lParam);
	LRESULT OnVisionPaneClosed(WPARAM wParam, LPARAM lParam);
	LRESULT OnRemoveRibbonCategory(WPARAM wParam, LPARAM lParam);

	void AddLogEventPane(CString strInfo, int nEventLog);
	void ShowHidePanes(BOOL bShow);
	virtual void ShowHideStatPane(BOOL bShow)=0;
	virtual void RecalcLayout(BOOL bNotify = TRUE);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

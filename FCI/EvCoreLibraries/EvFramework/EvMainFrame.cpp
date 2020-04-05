/// EvMainFrm.cpp : implementation of the CEvMainFrame class
/// Udupa; April'2012

#include "stdafx.h"
#include "MsgCode.h"
#include "EvImageDoc.h"
#include "Resource.h"
#include "EvMainFrame.h"
#include "NewTabDialog.h"
#include "EvChildFrame.h"
#include "EvDebugViewPane.h"
#include "EVStatsPane.h"
#include "EvTrackHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEvMainFrame

IMPLEMENT_DYNAMIC(CEvMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CEvMainFrame, CMDIFrameWndEx)
	//{{AFX_MSG_MAP(CEvMainFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_TOOLS_OPTIONS, OnOptions)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPD_STATUSBAR, OnUpdateStatusBar)
	ON_COMMAND(ID_STATUSBAR_NEWTAB, OnAddNewTab)
	ON_UPDATE_COMMAND_UI(ID_STATUSBAR_NEWTAB, OnUpdateAddNewTab)
///	ON_COMMAND(ID_NEWWINDOW_MESSAGELOGGER, OnNewwindowMessagelogger)
///	ON_UPDATE_COMMAND_UI(ID_NEWWINDOW_MESSAGELOGGER, OnUpdateNewwindowMessagelogger)
	ON_COMMAND(ID_NEWWINDOW_IMAGEHISTORY, OnNewwindowImageHistory)
	ON_UPDATE_COMMAND_UI(ID_NEWWINDOW_IMAGEHISTORY, OnUpdateNewwindowImageHistory)
	ON_COMMAND(ID_OUTPUT_LOG, OnShowOutputLog)
	ON_COMMAND(ID_NEWWINDOW_CPULOAD, OnNewwindowCpuLoad)
	ON_UPDATE_COMMAND_UI(ID_NEWWINDOW_CPULOAD, OnUpdateNewwindowCpuLoad)
	ON_REGISTERED_MESSAGE(AFX_WM_ON_CHANGE_RIBBON_CATEGORY, OnRibbonCategoryChanged)
	ON_COMMAND(ID_NEWWINDOW_IMAGEHISTORY+1, OnSelchangeSourceImageCombo)
	ON_UPDATE_COMMAND_UI(ID_NEWWINDOW_IMAGEHISTORY+1, OnUpdateNewwindowImageHistory)
	ON_COMMAND(ID_NEWWINDOW_IMAGEHISTORY+2, OnSelchangeHistoryDepthCombo)
	ON_UPDATE_COMMAND_UI(ID_NEWWINDOW_IMAGEHISTORY+2, OnUpdateNewwindowImageHistory)
	ON_COMMAND(ID_NEWWINDOW_MULTICAST+1, OnCheckMulticastEnable)
	ON_UPDATE_COMMAND_UI(ID_NEWWINDOW_MULTICAST+1, OnUpdateNewWindowMulticast)
	ON_MESSAGE(WM_PANE_CLOSED, OnPaneClosed)
//	ON_MESSAGE(WM_PANE_NEW_GROUP, MoveToNewTab)
	ON_MESSAGE(WM_VISION_PANE_CLOSED, OnVisionPaneClosed)
	ON_MESSAGE(WM_PUSH_IMAGEBUF, OnMessagePushHistoryBuffer)

	ON_MESSAGE(WM_RIBBON_REMOVECATEGORY, OnRemoveRibbonCategory)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEvMainFrame construction/destruction

CEvMainFrame::CEvMainFrame()
{
	m_bHiColorIcons = TRUE;
	m_pTrackHandlers = NULL;
	m_nActiveCategory = 0;

	m_bEnableMulticast = FALSE;
	m_bRemoteClosed = FALSE;
///	m_pDebugViewPane = NULL;
	bIsPropertiesVisiblePane = FALSE;
	bIsPropertiesVisiblePaneTeach = FALSE;
	bIsPropertiesVisiblePaneDefect = FALSE;
	bIsOutputPaneVisible = FALSE;

	m_wndOutputPane = NULL;
}

CEvMainFrame::~CEvMainFrame()
{
	
	if(m_pwndDefectProperties)
		delete m_pwndDefectProperties;

	if(m_pwndTeachProperties)
		delete m_pwndTeachProperties;

	if(m_pwndProperties)
		delete m_pwndProperties;
///	delete m_pDebugViewPane;
	/*int nRibbonCount = (int) m_arrRibbonCategories.GetCount();
	for(int i=0; i<nRibbonCount; i++)
		delete m_arrRibbonCategories[i];*/
	for(int i=0; i<m_arrTabWnds.GetCount(); i++) {
		if(m_arrTabWnds[i])
			delete m_arrTabWnds[i];
	}

	if(m_pTrackHandlers)
		delete[] m_pTrackHandlers;
}

int CEvMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_bIsMaximized = FALSE;
	CreateRibbon();
	m_wndRibbonBar.SetExclusiveRowMode(FALSE);
	m_wndRibbonBar.EnablePrintPreview(FALSE);

	///CMDIFrameWndEx::EnableDocking(CBRS_ALIGN_ANY);

	//CMDITabInfo mdiTabParams;
	//mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE;
	//mdiTabParams.m_bActiveTabCloseButton = FALSE;
	//mdiTabParams.m_bTabIcons = FALSE;
	//mdiTabParams.m_bAutoColor = TRUE;
	//mdiTabParams.m_bDocumentMenu = TRUE;
	//EnableMDITabbedGroups(TRUE, mdiTabParams);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; 
	// set to FALSE to place close button at right of tab area
	mdiTabParams.m_bActiveTabCloseButton = FALSE;
	// set to TRUE to enable document icons on MDI taba
	mdiTabParams.m_bTabIcons = FALSE;    
	// set to FALSE to disable auto-coloring of MDI tabs
	mdiTabParams.m_bAutoColor = FALSE; 
	// set to TRUE to enable the document menu at the right edge of the tab area
	mdiTabParams.m_bDocumentMenu = TRUE; 
	//set to TRUE to enable the user to change the tabs positions by dragging the tabs
	mdiTabParams.m_bEnableTabSwap = TRUE;
	// set to TRUE to give each tab window has a flat frame
	mdiTabParams.m_bFlatFrame = FALSE;
	// set to TRUE to enable each tab window to display the Close button on the right edge of the tab. 
	mdiTabParams.m_bTabCloseButton = FALSE;
	// set to TRUE to enable the tabs to display tooltips.
	mdiTabParams.m_bTabCustomTooltips = TRUE;
	// Specifies that the tabs labels are located at the top of the page
	mdiTabParams.m_tabLocation = CMFCTabCtrl::LOCATION_TOP;
	EnableMDITabbedGroups(TRUE, mdiTabParams);



	if(!m_wndStatusBar.Create(this)) {
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_StatusPanes[0] = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, "Pixel", TRUE, NULL, "Position: [0000 0000]  Size: [0000 0000]                    ");
	m_wndStatusBar.AddElement(m_StatusPanes[0], "Pixel Info");
	m_wndStatusBar.AddSeparator();
	m_StatusPanes[1] = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, "FOV", FALSE, NULL, "FOV: 9999, 9999");
	m_wndStatusBar.AddElement(m_StatusPanes[1], "Zoom_FOV");
	m_wndStatusBar.AddSeparator();
	m_StatusPanes[2] = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE3, "", TRUE, NULL, "Stored Image Folder: C:\\Demo\\Inspection\\Camera1");
	m_wndStatusBar.AddElement(m_StatusPanes[2], "Process");
	m_wndStatusBar.AddSeparator();
	m_StatusPanes[3] = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE4, "", TRUE, NULL,
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	m_wndStatusBar.AddElement(m_StatusPanes[3], "Status");


	

	
	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	/*CString filename = "Software\\MonsterVision\\InterojoWetLens\\Preferences\\Persistence\\DockingManager";
*/
	
	/*CDockState state;*/
	/*state.LoadState(_T("Software\\MonsterVision\\InterojoWetLens\\Preferences\\Persistence\\DockingManager"));
	SetDockState(state);*/
	//CDockingManager* pDock = GetDockingManager();
	//pDock->DockPane(
	/*EnableMDITabs(FALSE,
		TRUE,
		CMFCBaseTabCtrl::LOCATION_TOP,
		FALSE,
		CMFCTabCtrl::STYLE_3D_SCROLLED,
		FALSE,
		FALSE);*/
	
	// Navigation pane will be created at left, so temporary disable docking at the left side:
	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);

	//// Create and setup "Outlook" navigation bar:
	//if (!CreateOutlookBar(m_wndNavigationBar, ID_VIEW_NAVIGATION, m_wndTree, m_wndCalendar, 250))
	//{
	//	TRACE0("Failed to create navigation pane\n");
	//	return -1;      // fail to create
	//}

	// Create a caption bar:
	//if (!CreateCaptionBar())
	//{
	//	TRACE0("Failed to create caption bar\n");
	//	return -1;      // fail to create
	//}

	//// Outlook bar is created and docking on the left side should be allowed.
	//EnableDocking(CBRS_ALIGN_LEFT);
	//EnableAutoHidePanes(CBRS_ALIGN_RIGHT);


	

	///m_arrRibbonCategories.Add(new CEvRibbonCategory(m_wndRibbonBar.GetCategory(0), 0, CategoryType_MainView));

	///CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory("Statistics", 0, IDB_WRITELARGE);
	///pCategory->SetKeys("S");
	///m_arrRibbonCategories.Add(new CEvRibbonCategory(pCategory, 1, CategoryType_Statistics));

	///CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory("+", 0, IDB_WRITELARGE);
	///pCategory->SetKeys("N");
	///m_arrRibbonCategories.Add(new CEvRibbonCategory(pCategory, 2, CategoryType_None));

	m_wndRibbonBar.Initialize();
	///m_wndRibbonBar.IsReplaceFrameCaption();
	//m_wndRibbonBar.SetQuickAccessDefaultState(

	if(!CreateDockingWindows()) {
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	///m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	///DockPane(&m_wndOutput);
	m_pwndProperties->EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_TOP | CBRS_ALIGN_RIGHT | CBRS_ALIGN_BOTTOM);
	DockPane(m_pwndProperties);		
		
	m_pwndTeachProperties->EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_TOP | CBRS_ALIGN_RIGHT | CBRS_ALIGN_BOTTOM);
    DockPane(m_pwndTeachProperties);

	m_pwndDefectProperties->EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_TOP | CBRS_ALIGN_RIGHT | CBRS_ALIGN_BOTTOM);
    DockPane(m_pwndDefectProperties);
	//FloatControlBar(
	//m_pwndProperties->hideUndockPane();
	//m_wndProperties.Dock(
	//GetDockingManager()->HideAutoHidePanes();


	// Enable enhanced windows management dialog
	//EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// Switch the order of document name and application name on the window title bar. This
	// improves the usability of the taskbar because the document name is visible with the thumbnail.
	//////ModifyStyle(0, FWS_PREFIXTITLE);
	//ModifyStyle(WS_CAPTION, 0);

	/*EnableFullScreenMode(ID_FULLSCREEN);
	EnableFullScreenMainMenu(FALSE);
	ShowTaskBar(FALSE);*/

///	m_wndRibbonBar.Initialize();
	if(m_hMenu != NULL) {
		::DestroyMenu(m_hMenu);
		m_hMenu = NULL;
	}


	


	//InitializeApp();



	return 0;
}

void CEvMainFrame::LoadDefaults(CString strRegPath)
{
	EV_REGPATH = strRegPath;
	m_bEnableMulticast = GetRegistryEntry(EV_REGPATH + "Preferences\\Persistence", "MultiCast", FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CEvMainFrame diagnostics

#ifdef _DEBUG
void CEvMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CEvMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEvMainFrame message handlers


LRESULT CEvMainFrame::OnUpdateStatusBar(WPARAM wParam, LPARAM lParam)
{
	int nPane = (int) wParam;
	if(nPane > 3)
		return -1;

	CString* pstrMsg = (CString*) lParam;
	CString strOld = m_StatusPanes[nPane]->GetText();
	if(pstrMsg->CompareNoCase(strOld)==0)
		return 0;

	m_StatusPanes[nPane]->SetText(pstrMsg->GetBuffer(0));
	m_StatusPanes[nPane]->Redraw();
	pstrMsg->ReleaseBuffer();

	return 0;
}

///Udupa; Aug'2012
HANDLE ghMutex = NULL;
BOOL IsSingleInstance(CString strId)
{
	SetLastError(0);
	ghMutex = CreateMutex(NULL, FALSE, strId);
	if((ghMutex == NULL) || (GetLastError() == ERROR_ALREADY_EXISTS)) {
		if(ghMutex != NULL) {
			ReleaseMutex(ghMutex);
			CloseHandle(ghMutex);
			ghMutex = NULL;
		}
		return FALSE;
	}

	return TRUE;
}

void CEvMainFrame::OnClose() 
{
	//SetCursor(LoadCursor(NULL, IDC_WAIT));
	if (m_pwndDefectProperties->IsWindowVisible())
	{
		m_pwndDefectProperties->OnClose();
	}
	if (m_pwndTeachProperties->IsWindowVisible())
	{
		m_pwndTeachProperties->OnClose();
	}
	if (m_pwndProperties->IsWindowVisible())
	{
		m_pwndProperties->OnClose();
	}
	if (m_wndOutputPane->IsWindowVisible())
	{
		m_wndOutputPane->OnClose();
	}
	for(int i=0; i<m_arrTabWnds.GetCount(); i++) {
		if(m_arrTabWnds[i]) {
			m_arrTabWnds[i]->ShowPane(FALSE, FALSE, FALSE);
			//GetDockingManager()->RemovePaneFromDockManager(m_arrTabWnds[i], TRUE, TRUE);
			m_arrTabWnds[i]->SendMessage(WM_CLOSE);
		}
	}

	CMDIFrameWndEx::OnClose();
}


void CEvMainFrame::ShowTaskBar(BOOL bShow)
{
	::ShowWindow(::FindWindow("Shell_TrayWnd", NULL), bShow ? SW_SHOW : SW_HIDE);
	::ShowWindow(::FindWindowEx(NULL, NULL, MAKEINTATOM(0xC017), NULL), bShow ? SW_SHOW : SW_HIDE);
	CRect rect;
	HWND taskBar = ::FindWindow("Shell_traywnd", NULL);
	if (taskBar) {
		::GetWindowRect(taskBar, &rect);
	}
	int nHeight = rect.Height();
	if (bShow) {
		SetWindowPos(NULL, 0, 0,
			GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)- rect.Height(), SWP_SHOWWINDOW);
	}
	else {
		SetWindowPos(NULL, 0, 0,
			GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
	}
	//HWND hTaskBarWnd = ::FindWindow("Shell_TrayWnd", NULL);
	//if (NULL == hTaskBarWnd) {
	//	AfxMessageBox("Cannot find task bar window");
	//	return;
	//}

	//::SetWindowPos(hTaskBarWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | bShow ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);
}

void CEvMainFrame::InitializeProperties(CEvImageDoc* pDoc, int nFOV)
{
	m_pwndProperties->InitPropList(pDoc, nFOV);
	/*if(!m_pwndProperties->IsPaneVisible())
		bIsPropertiesVisiblePane = TRUE;*/
}

void CEvMainFrame::ShowProperties(CEvImageDoc* pDoc, int nFOV)
{
	m_pwndProperties->RefreshPropList(pDoc, nFOV);
	//DockPane(m_pwndProperties);

	CRect rectClient;
	GetDockingManager()->GetClientAreaBounds(rectClient);
	rectClient.left = rectClient.right - 350;
	//m_pwndProperties->FloatPane(rectClient, DM_UNKNOWN, false);
	m_pwndProperties->ShowPane(TRUE, FALSE, TRUE);
	bIsPropertiesVisiblePane = TRUE;
}

void CEvMainFrame::InitializeTeachProperties(CEvImageDoc* pDoc, int nFOV)
{
	m_pwndTeachProperties->InitPropList(pDoc, nFOV);
	/*if(!m_pwndProperties->IsPaneVisible())
		bIsPropertiesVisiblePaneTeach = TRUE;*/
}

void CEvMainFrame::ShowTeachProperties(CEvImageDoc* pDoc, int nFOV)
{
	m_pwndTeachProperties->RefreshPropList(pDoc, nFOV);
	//DockPane(m_pwndProperties);

	CRect rectClient;
	GetDockingManager()->GetClientAreaBounds(rectClient);
	rectClient.left = rectClient.right - 350;
	//m_pwndProperties->FloatPane(rectClient, DM_UNKNOWN, false);
	m_pwndTeachProperties->ShowPane(TRUE, FALSE, TRUE);
	bIsPropertiesVisiblePaneTeach = TRUE;
}

void CEvMainFrame::InitializeDefectProperties()
{
	m_pwndDefectProperties->InitPropList();
	/*if(!m_pwndProperties->IsPaneVisible())
		bIsPropertiesVisiblePaneDefect = TRUE;*/
}

void CEvMainFrame::ShowDefectProperties()
{
	CRect rectClient;
	GetDockingManager()->GetClientAreaBounds(rectClient);
	rectClient.left = rectClient.right - 350;
	//m_pwndProperties->FloatPane(rectClient, DM_UNKNOWN, false);
	//m_pwndDefectProperties->ShowPane(TRUE, FALSE, TRUE);
	bIsPropertiesVisiblePaneDefect = TRUE;
}


void CEvMainFrame::HideParmPanes()
{
///	OnNewwindowMessagelogger();
	m_pwndProperties->ShowPane(FALSE, FALSE, FALSE);
	m_pwndTeachProperties->ShowPane(FALSE, FALSE, FALSE);
	m_pwndDefectProperties->ShowPane(FALSE, FALSE, FALSE);
	RecalcLayout();
}

BOOL CEvMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CMDIFrameWndEx::PreCreateWindow(cs))
		return FALSE;
	
		//cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

		//cs.style &= (0xFFFFFFFF ^ WS_SIZEBOX);
		//cs.style |= WS_BORDER;
		//cs.style &= (0xFFFFFFFF ^ WS_MAXIMIZEBOX);
		////end my edits

	
	cs.style = WS_CLIPCHILDREN | FWS_ADDTOTITLE | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX;// | WS_MAXIMIZEBOX;// | WS_DLGFRAME; // ;
	cs.style |= (WS_SYSMENU | WS_MINIMIZEBOX);
	cs.dwExStyle = WS_EX_WINDOWEDGE;

	cs.cx = GetSystemMetrics(SM_CXSCREEN);
	cs.cy = GetSystemMetrics(SM_CYSCREEN);

	return TRUE;
}

void CEvMainFrame::Maximize()
{
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	::SetWindowPos(m_hWnd, HWND_TOP, -10, -50, cx+10, cy+50, SWP_SHOWWINDOW);
}

BOOL CEvMainFrame::OnCloseMiniFrame(CPaneFrameWnd* pWnd)
{
	CWnd* pPane = pWnd->GetPane();
	int nResult = 0;
	if(pPane)
		nResult = (int) OnPaneClosed((WPARAM) pPane, NULL);
	//	return FALSE;

	//pDocClass->IsDerivedFrom(RUNTIME_CLASS(CDocument)));

	if(nResult == 0)
		return CMDIFrameWndEx::OnCloseMiniFrame(pWnd);

	return FALSE;
}

BOOL CEvMainFrame::CreateDockingWindows()
{
	DWORD dwNoCloseBarStyle = AFX_DEFAULT_DOCKING_PANE_STYLE & ~AFX_CBRS_AUTOHIDE;
	///m_pwndProperties->Initialize();
	if (!m_pwndProperties->Create("Vision Parameters", this, CRect(0, 0, 350, 900), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | CBRS_RIGHT | CBRS_FLOAT_MULTI, /*AFX_CBRS_OUTLOOK_TABS*/AFX_CBRS_REGULAR_TABS, dwNoCloseBarStyle))//CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}
	if (!m_pwndTeachProperties->Create("Teach Parameters", this, CRect(0, 0, 350, 900), TRUE, ID_VIEW_TEACHPROPERTIESWND, WS_CHILD | CBRS_RIGHT | CBRS_FLOAT_MULTI,  /*AFX_CBRS_OUTLOOK_TABS*/AFX_CBRS_REGULAR_TABS, dwNoCloseBarStyle))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}
	if (!m_pwndDefectProperties->Create("Defect Information", this, CRect(0, 0, 350, 900), TRUE, ID_VIEW_DEFECTPROPERTIESWND, WS_CHILD | CBRS_RIGHT | CBRS_FLOAT_MULTI,  /*AFX_CBRS_OUTLOOK_TABS*/AFX_CBRS_REGULAR_TABS, dwNoCloseBarStyle))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}
	
	SetDockingWindowIcons(m_bHiColorIcons);
	return TRUE;
}

void CEvMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	//HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	//m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_pwndProperties->SetIcon(hPropertiesBarIcon, FALSE);

	HICON hPropertiesTeachBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_pwndTeachProperties->SetIcon(hPropertiesBarIcon, FALSE);

	HICON hPropertiesDefectBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_pwndDefectProperties->SetIcon(hPropertiesBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

BOOL CEvMainFrame::CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, CMFCShellTreeCtrl& tree, CCalendarBar& calendar, int nInitialWidth)
{
	bar.SetMode2003();

	if (!bar.Create("Shortcuts", this, CRect(0, 0, nInitialWidth, 32000), uiID, WS_CHILD | WS_VISIBLE | CBRS_LEFT))
	{
		return FALSE; // fail to create
	}

	CMFCOutlookBarTabCtrl* pOutlookBar = (CMFCOutlookBarTabCtrl*)bar.GetUnderlyingWindow();

	if (pOutlookBar == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	pOutlookBar->EnableInPlaceEdit(TRUE);

	static UINT uiPageID = 1;

	// can float, can autohide, can resize, CAN NOT CLOSE
	DWORD dwStyle = AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE;

	CRect rectDummy(0, 0, 0, 0);
	const DWORD dwTreeStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	tree.Create(dwTreeStyle, rectDummy, &bar, 1200);
	pOutlookBar->AddControl(&tree, "Folders", 2, TRUE, dwStyle);

	calendar.Create(rectDummy, &bar, 1201);
	pOutlookBar->AddControl(&calendar, "Calendar", 3, TRUE, dwStyle);

	bar.SetPaneStyle(bar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	CWinAppEx* pApp = (CWinAppEx*) AfxGetApp();
	pOutlookBar->SetImageList(m_bHiColorIcons ? IDB_PAGES_HC : IDB_PAGES, 24);
	pOutlookBar->SetToolbarImageList(m_bHiColorIcons ? IDB_PAGES_SMALL_HC : IDB_PAGES_SMALL, 16);
	pOutlookBar->RecalcLayout();

	BOOL bAnimation = pApp->GetInt(_T("OutlookAnimation"), TRUE);
	CMFCOutlookBarTabCtrl::EnableAnimation(bAnimation);

	bar.SetButtonsFont(&afxGlobalData.fontBold);

	return TRUE;
}

BOOL CEvMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("Failed to create caption bar\n");
		return FALSE;
	}

	m_wndCaptionBar.SetButton("Options...", ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	m_wndCaptionBar.SetButtonToolTip("Click here to see more options");

	m_wndCaptionBar.SetText("This is a caption bar where a message can be presented to the user.", CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	m_wndCaptionBar.SetImageToolTip("Important", "Here is an informational message for the user.");

	return TRUE;
}

// CEvMainFrame diagnostics

// CEvMainFrame message handlers

void CEvMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CEvMainFrame::SetApplicationLook(int nIndex)
{
	CWinAppEx* pApp = (CWinAppEx*) AfxGetApp();
	switch (nIndex)
	{
	case 0:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case 1:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case 2:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case 3:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case 4:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case 5:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case 6:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (nIndex)
		{
		case 7:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case 8:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case 9:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case 10:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	pApp->WriteInt(_T("ApplicationLook"), m_nAppLook);
}

void CEvMainFrame::OnViewCaptionBar()
{
	if(IsWindow(m_wndCaptionBar.m_hWnd)) {
		m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
		RecalcLayout(FALSE);
	}
}

void CEvMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	if(IsWindow(m_wndCaptionBar.m_hWnd))
		pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

void CEvMainFrame::OnOptions()
{
	CMFCRibbonCustomizeDialog *pOptionsDlg = new CMFCRibbonCustomizeDialog(this, &m_wndRibbonBar);
	ASSERT(pOptionsDlg != NULL);

	pOptionsDlg->DoModal();
	delete pOptionsDlg;
}

void CEvMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
	for(int i=0; i<m_arrTabWnds.GetCount(); i++) {
		if(m_arrTabWnds[i])
			m_arrTabWnds[i]->UpdateFonts();
	}
}

void CEvMainFrame::ViewFullScreen(BOOL bFullScreen, BOOL bOnlineMode) 
{
	ShowTaskBar(!bFullScreen);
	if(bFullScreen) {
		if(bOnlineMode) {
			m_wndStatusBar.ShowWindow(SW_SHOW);
			m_wndRibbonBar.Maximize(FALSE);
		}
		m_pwndProperties->ShowPane(FALSE, FALSE, FALSE);
		m_pwndTeachProperties->ShowPane(FALSE, FALSE, FALSE);
		//m_wndOutputPane.ShowPane(FALSE, FALSE, FALSE);
///		m_pDebugViewPane->ShowPane(FALSE, FALSE, FALSE);
		ModifyStyle(WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX/* | WS_MAXIMIZEBOX*/, 0);
		//Maximize();
		ShowWindow(SW_MAXIMIZE);
		UpdateWindow();
		CRect rect;
		GetWindowRect(&rect);
		
		/*if(rect.Height() < GetSystemMetrics(SM_CYSCREEN))
			ShowWindow(SW_RESTORE);*/
	}
	else {
		if(bOnlineMode) {
			m_wndStatusBar.ShowWindow(SW_SHOW);
			m_wndRibbonBar.ForceRecalcLayout();
			m_wndRibbonBar.Maximize(TRUE);
			m_wndRibbonBar.SetActiveCategory(m_wndRibbonBar.GetCategory(m_nActiveCategory >= 0 ? m_nActiveCategory : 0));
		}
		ModifyStyle(0, WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX/* | WS_MAXIMIZEBOX*/);
		//ShowHidePanes(TRUE);
		ShowWindow(SW_SHOWMAXIMIZED);
	}

	m_bFullScreen = bFullScreen;
	//ShowFullScreen();

	RecalcLayout(TRUE);
	SetFocus();
}

void CEvMainFrame::OnOrbButton()
{
}

void CEvMainFrame::OnOrbFullScreen()
{
	m_bFullScreen = !m_bFullScreen;
	ViewFullScreen(m_bFullScreen);
}

void CEvMainFrame::OnUpdateOrbFullScreen(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_bFullScreen);
}

void CEvMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIFrameWndEx::OnSize(nType, cx, cy);
}

void CEvMainFrame::GetCanvasRect(CRect& rect)
{
	GetDockingManager()->GetClientAreaBounds(rect);
}

void CEvMainFrame::ResizeChildren()
{
	int nCount = (int) m_arrTabWnds.GetCount();
	CDockablePane* pPane;
	CRect rectClient;
	GetDockingManager()->GetClientAreaBounds(rectClient);
	for(int i=0; i<nCount; i++) {
		pPane = m_arrTabWnds[i];
		if(pPane && IsWindow(pPane->m_hWnd) && pPane->IsFloating())
			pPane->GetParentMiniFrame()->SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOZORDER);
	}
}

void CEvMainFrame::RecalcLayout(BOOL bNotify)
{
	//ResizeChildren();

	CMDIFrameWndEx::RecalcLayout(bNotify);
}

void CEvMainFrame::OnAddNewTab()
{
}

void CEvMainFrame::OnUpdateAddNewTab(CCmdUI* pCmdUI)
{
}

void CEvMainFrame::CreateNewTab(CEvTabPane* pWnd)
{
	m_arrTabWnds.Add(pWnd);
}

BOOL CEvMainFrame::ActivateExisting(int type)
{
	int nCount = m_wndRibbonBar.GetCategoryCount();
	CEvRibbonCategory* pCategory;
	for(int i=1; i<nCount; i++) {
		pCategory = m_arrRibbonCategories[i];
		if(pCategory->nCategoryType == type) {
			//m_wndRibbonBar.SetActiveCategory(pCategory->pCategory, TRUE);
			return TRUE;
		}
	}

	return FALSE;
}

void CEvMainFrame::CreateStatsPane()
{
	CEVStatsPane* pWnd = new CEVStatsPane(this);
	CRect rectClient;
	GetDockingManager()->GetClientAreaBounds(rectClient);

	if(pWnd->Create("Statistics", this, rectClient, TRUE, ID_NEWWINDOW_FAILEDIMAGESVIEW, 
		WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLYBY))
	{
		pWnd->SetControlBarStyle(AFX_CBRS_FLOAT | AFX_CBRS_CLOSE | AFX_CBRS_AUTOHIDE);
		pWnd->EnableDocking(CBRS_ALIGN_ANY);
		pWnd->FloatPane(rectClient, DM_UNKNOWN, false);

		CreateNewTab(pWnd);
	}
}

//void CEvMainFrame::OnNewwindowMessagelogger()
//{
//	/*if(ActivateExisting(CategoryType_MessageLogger))
//		return;*/
//
//	CRect rect;
//	GetDebugPane()->GetWindowRect(&rect);
//	DWORD dwNoCloseBarStyle = AFX_DEFAULT_DOCKING_PANE_STYLE & ~AFX_CBRS_AUTOHIDE;
//	int cx = (int)(rect.Width());
//	int cy = (int)(rect.Height());
//
//	if(!m_pDebugViewPane){
//		m_pDebugViewPane = new CEvDebugViewPane();
//	/*	CRect rectClient;
//		GetDockingManager()->GetClientAreaBounds(rectClient);*/
//
//		if(!m_pDebugViewPane->Create("Message Log", this, CRect(0,0,600,400), TRUE, ID_MESSAGELOGGERWNDS, 
//			WS_CHILD | CBRS_RIGHT |CBRS_FLOAT_MULTI,AFX_CBRS_REGULAR_TABS,dwNoCloseBarStyle))
//		{
//			/*if(!ActivateExisting(CategoryType_MessageLogger)) {
//				HWND hWnd = ::FindWindow("#32770", "EV Debug Viewer");
//				if(hWnd != NULL)
//					::SetForegroundWindow(hWnd);
//			}*/
//			return;
//		}
//		//m_pDebugViewPane->SetControlBarStyle(AFX_CBRS_FLOAT | AFX_CBRS_CLOSE | AFX_CBRS_AUTOHIDE);
//		m_pDebugViewPane->EnableDocking(CBRS_ALIGN_RIGHT|CBRS_ALIGN_BOTTOM);
//		//m_pDebugViewPane->AttachToTabWnd(GetDebugPane(),DM_SHOW,1);
//		//pWnd->FloatPane(CRect(0, 0, rectClient.Width()*0.15, rectClient.Height()*0.15), DM_UNKNOWN, false);
//		m_pDebugViewPane->DockToWindow(GetDebugPane(),CBRS_RIGHT,CRect(cx/2,cy/2,cx,cy));
//		//DockPane((CBasePane*)m_pDebugViewPane,AFX_IDW_DOCKBAR_BOTTOM,CRect(cx,cy,cx,cy));
//	}
//	
//	if(m_pDebugViewPane->m_hWnd && m_pDebugViewPane->IsWindowVisible()) {
//		if(m_pDebugViewPane->IsDocked()){
//			m_pDebugViewPane->UndockPane();
//			m_pDebugViewPane->DockToWindow(GetDebugPane(),CBRS_RIGHT,CRect(cx/2,cy/2,cx,cy));
//			//DockPane((CBasePane*)m_pDebugViewPane,AFX_IDW_DOCKBAR_BOTTOM,CRect(cx,cy,cx,cy));
//		}
//		if(m_pDebugViewPane->IsFloating()){
//			//m_dlgPaneStats->UndockPane();
//			m_pDebugViewPane->DockToWindow(GetDebugPane(),CBRS_RIGHT,CRect(cx/2,cy/2,cx,cy));
//		}
//		m_pDebugViewPane->ShowPane(TRUE, FALSE, TRUE);
//	}
////	m_pDebugViewPane->ShowPane(TRUE,FALSE,TRUE);
//	//DockPane(m_pDebugViewPane,AFX_IDW_DOCKBAR_BOTTOM);
//	/*CreateNewTab(pWnd);
//
//	int nRibbonCount = (int) m_arrRibbonCategories.GetCount();
//	CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory("Message Log", 0, IDB_WRITELARGE, 
//		CSize(16,16), CSize(32,32), nRibbonCount-1);
//
//	m_arrRibbonCategories.InsertAt(nRibbonCount-1, new CEvRibbonCategory(pCategory, nRibbonCount-1, CategoryType_MessageLogger), 1);
//	nRibbonCount = (int) m_arrRibbonCategories.GetCount();
//	m_arrRibbonCategories[nRibbonCount-1]->nIndex = nRibbonCount-1;
//	m_wndRibbonBar.SetActiveCategory(pCategory, TRUE);*/
//	RecalcLayout();
//}

//void CEvMainFrame::OnUpdateNewwindowMessagelogger(CCmdUI *pCmdUI)
//{
//	pCmdUI->Enable(m_pDebugViewPane->bIsClosed && !m_bIsMaximized);
//}

void CEvMainFrame::OnNewwindowMulticast(BOOL bActivate)
{
	if(ActivateExisting(CategoryType_Multicast))
		return;

	CreateNewTab(NULL);
	int nRibbonCount = (int) m_arrRibbonCategories.GetCount();
	CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory("Multicast", 0, IDB_RIBBONMULTICAST_PNG, 
		CSize(16,16), CSize(32,32), nRibbonCount-1);

	CMFCRibbonPanel* pPanel = pCategory->AddPanel("Multicast");

	pPanel->Add(new CMFCRibbonLabel(""));
	CMFCRibbonButton* pButton = new CMFCRibbonButton(ID_NEWWINDOW_MULTICAST+1, "Multicast Enabled", -1, 0);
	pPanel->Add(pButton);
	pPanel->Add(new CMFCRibbonLabel(""));

	m_arrRibbonCategories.InsertAt(nRibbonCount-1, new CEvRibbonCategory(pCategory, nRibbonCount-1, CategoryType_Multicast), 1);
	nRibbonCount = (int) m_arrRibbonCategories.GetCount();
	m_arrRibbonCategories[nRibbonCount-1]->nIndex = nRibbonCount-1;
	if(bActivate)
		m_wndRibbonBar.SetActiveCategory(pCategory, TRUE);

	m_bEnableMulticast = TRUE;
	ToggleMulticast();

	m_wndRibbonBar.RecalcLayout();
}

void CEvMainFrame::OnCheckMulticastEnable()
{
	int nIndex = m_wndRibbonBar.GetCategoryIndex(m_wndRibbonBar.GetActiveCategory());
	CEvRibbonCategory* pCategory = m_arrRibbonCategories[nIndex];

	//CMFCRibbonCheckBox* pCheck = (CMFCRibbonCheckBox*) m_wndRibbonBar.FindByID(ID_NEWWINDOW_MULTICAST+1);
	CMFCRibbonButton* pButton = (CMFCRibbonButton*) m_wndRibbonBar.FindByID(ID_NEWWINDOW_MULTICAST+1);
	if(pButton) {
		m_bEnableMulticast = !m_bEnableMulticast;
		ToggleMulticast();

		if(!m_bEnableMulticast)
			PostMessage(WM_RIBBON_REMOVECATEGORY, m_wndRibbonBar.GetCategoryIndex(m_wndRibbonBar.GetActiveCategory()));
	}
}

LRESULT CEvMainFrame::OnRemoveRibbonCategory(WPARAM wParam, LPARAM lParam)
{
	int nIndex = (int) wParam;

	m_nActiveCategory = -1;
	m_wndRibbonBar.RemoveCategory(nIndex);
	int nTabIndex = nIndex - 1;
	if((m_arrTabWnds.GetCount() > nTabIndex) && m_arrTabWnds[nTabIndex])
		delete m_arrTabWnds[nTabIndex];
	m_arrTabWnds.RemoveAt(nTabIndex);
	m_arrRibbonCategories.RemoveAt(nIndex);

	for(int i=nIndex; i<m_arrRibbonCategories.GetCount(); i++)
		m_arrRibbonCategories[i]->nIndex = i;
	m_wndRibbonBar.SetActiveCategory(m_arrRibbonCategories[nIndex==(m_arrRibbonCategories.GetCount()-1) ? nIndex-1 : nIndex]->pCategory, TRUE);
	
	m_wndRibbonBar.RecalcLayout();
	//OnRibbonCategoryChanged(0, 0);


	return 0;
}

void CEvMainFrame::OnUpdateNewWindowMulticast(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_bEnableMulticast);
}

void CEvMainFrame::OnNewwindowImageHistory()
{
	CEvTabPane* pWnd = new CEvHistoryImagePane();
	CRect rectClient;
	GetDockingManager()->GetClientAreaBounds(rectClient);
		
	int nCount = 1;
	/*int nRibbonCount = (int) m_arrRibbonCategories.GetCount();
	for(int i=0; i<nRibbonCount; i++) {
		if(m_arrRibbonCategories[i]->nCategoryType == CategoryType_ImageHistory)
			nCount++;
	}*/
	CString str;
	str.Format("Image History %d", nCount);

	if(!pWnd->Create(str, this, rectClient, TRUE, ID_IMAGEHISTORYWNDS, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLYBY))
	{
		TRACE0("Failed to create Image History window\n");
		return; // failed to create
	}

	///AFX_CBRS_FLOAT | AFX_CBRS_CLOSE | AFX_CBRS_RESIZE | AFX_CBRS_AUTOHIDE
	pWnd->SetControlBarStyle(AFX_CBRS_FLOAT | AFX_CBRS_CLOSE | AFX_CBRS_AUTOHIDE);
	pWnd->EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT | CBRS_ALIGN_BOTTOM);
	pWnd->FloatPane(rectClient, DM_UNKNOWN, false);
	pWnd->ShowPane(TRUE,FALSE,TRUE);
	//DockPane(pWnd);
	//HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_OUTPUT_WND_HC), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	//pWnd->SetIcon(hOutputBarIcon, FALSE);

	//CreateNewTab(pWnd);

	//nRibbonCount = (int) m_arrRibbonCategories.GetCount();
	//CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(str, 0, IDB_WRITELARGE, 
	//	CSize(16,16), CSize(32,32), nRibbonCount-1);

	//CMFCRibbonPanel* pPanel = pCategory->AddPanel(str);

	//pPanel->Add(new CMFCRibbonLabel(""));
	//CMFCRibbonComboBox* pCombo = new CMFCRibbonComboBox(ID_NEWWINDOW_IMAGEHISTORY+1, FALSE, 60, "Source Image ");
	//for(int i=0; i<5; i++) {
	//	str.Format("Camera %d", i+1);
	//	pCombo->AddItem(str);
	//}
	//pCombo->SelectItem("Camera 1");
	//pPanel->Add(pCombo);

	//pPanel->Add(new CMFCRibbonLabel(""));
	//pCombo = new CMFCRibbonComboBox(ID_NEWWINDOW_IMAGEHISTORY+2, FALSE, 20, "History Depth ");
	//for(int i=0; i<10; i++) {
	//	str.Format("%d", i+1);
	//	pCombo->AddItem(str);
	//}
	//pCombo->SelectItem("6");
	//pCombo->SetDropDownHeight(300);
	//pPanel->Add(pCombo);

	/*m_arrRibbonCategories.InsertAt(nRibbonCount-1, new CEvRibbonCategory(pCategory, nRibbonCount-1, CategoryType_ImageHistory), 1);
	nRibbonCount = (int) m_arrRibbonCategories.GetCount();
	m_arrRibbonCategories[nRibbonCount-1]->nIndex = nRibbonCount-1;
	m_wndRibbonBar.SetActiveCategory(pCategory, TRUE);

	OnSelchangeSourceImageCombo();*/
	m_wndRibbonBar.RecalcLayout();
}

void CEvMainFrame::OnSelchangeSourceImageCombo()
{
	int nIndex = m_wndRibbonBar.GetCategoryIndex(m_wndRibbonBar.GetActiveCategory());
	CEvRibbonCategory* pCategory = m_arrRibbonCategories[nIndex];

	CEvHistoryImagePane* pPane = (CEvHistoryImagePane*) m_arrTabWnds[pCategory->nIndex-1];
	if(!pPane)
		return;

	CMFCRibbonComboBox* pCombo = (CMFCRibbonComboBox*) m_wndRibbonBar.FindByID(ID_NEWWINDOW_IMAGEHISTORY+1);
	if(pCombo)
		pPane->m_nSourceImage = pCombo->GetCurSel();
	CString str;
	str.Format("Camera %d History", pPane->m_nSourceImage+1);
	pCategory->pCategory->SetName(str);
	pPane->SetWindowTextA(str);
	m_wndRibbonBar.Invalidate();
	
	pPane->SetTitles();
}

void CEvMainFrame::OnSelchangeHistoryDepthCombo()
{
	int nIndex = m_wndRibbonBar.GetCategoryIndex(m_wndRibbonBar.GetActiveCategory());
	CEvRibbonCategory* pCategory = m_arrRibbonCategories[nIndex];

	CEvHistoryImagePane* pPane = (CEvHistoryImagePane*) m_arrTabWnds[pCategory->nIndex-1];
	if(!pPane)
		return;

	CMFCRibbonComboBox* pCombo = (CMFCRibbonComboBox*) m_wndRibbonBar.FindByID(ID_NEWWINDOW_IMAGEHISTORY+2);
	pPane->SetHistoryDepth(pCombo ? pCombo->GetCurSel()+1 : 6);
}

int CEvMainFrame::ShowFailedImage(int nCamera, int nImageIndex)
{
	CFailImageStack* pFailure;
	if(!m_pTrackHandlers[nCamera]->GetFailedImage(nImageIndex, pFailure)) {
		int nIndex = m_wndRibbonBar.GetCategoryIndex(m_wndRibbonBar.GetActiveCategory());
		CEvRibbonCategory* pCategory = m_arrRibbonCategories[nIndex];

		CEVStatsPane* pPane = (CEVStatsPane*) m_arrTabWnds[pCategory->nIndex-1];
		return pPane->OnSelection(nCamera, nImageIndex+1, pFailure);
	}

	return -1;
}

LRESULT CEvMainFrame::OnMessagePushHistoryBuffer(WPARAM wParam, LPARAM lParam)
{
	PushHistoryBuffer((int) wParam, (CImageBuffer*) lParam);

	return 0;
}

void CEvMainFrame::PushHistoryBuffer(int nCamera, CImageBuffer* pBuffer)
{
	int nCount = m_wndRibbonBar.GetCategoryCount();
	CEvRibbonCategory* pCategory;
	for(int i=1; i<nCount; i++) {
		pCategory = m_arrRibbonCategories[i];
		if(pCategory->nCategoryType == CategoryType_ImageHistory) {
			CEvHistoryImagePane* pPane = (CEvHistoryImagePane*) m_arrTabWnds[i-1];
			if(nCamera == pPane->m_nSourceImage)
				pPane->PushImage(pBuffer);
		}
	}
}

void CEvMainFrame::OnUpdateNewwindowImageHistory(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CEvMainFrame::OnNewwindowCpuLoad()
{

}

void CEvMainFrame::OnUpdateNewwindowCpuLoad(CCmdUI *pCmdUI)
{

}

LRESULT CEvMainFrame::OnRibbonCategoryChanged(WPARAM wParam, LPARAM lParam)
{

	int nIndex = m_wndRibbonBar.GetCategoryIndex(m_wndRibbonBar.GetActiveCategory());
	if (m_bIsMaximized) {
		OnUpdateRibbonTab();
	}
	//if(m_nActiveCategory == nIndex)
	//	return 0;

	//CEvRibbonCategory* pCategory = m_arrRibbonCategories[nIndex];


	//if(pCategory->nCategoryType == CategoryType_None) {
	//	if(m_wndRibbonBar.IsMaximized())
	//		m_wndRibbonBar.SetActiveCategory(m_wndRibbonBar.GetCategory(m_nActiveCategory >= 0 ? m_nActiveCategory : 0));
	//	else {
	//		keybd_event(VK_MENU, 0, 0, 0);
	//		keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
	//	}

	//	if(IsNewTabAvailable()) {
	//		CNewTabDialog dlg;
	//		dlg.DoModal();
	//	}
	//}
	//else {
	//	CDockablePane* pPane;
	//	if(pCategory->nCategoryType == CategoryType_MainView) {
	//		ShowMainView(TRUE);
	//		for(int i=0; i<m_arrTabWnds.GetCount(); i++) {
	//			pPane = m_arrTabWnds[i];
	//			if(pPane && pPane->IsFloating())
	//				pPane->ShowPane(FALSE, FALSE, FALSE);
	//		}
	//	}
	//	else {
	//		for(int i=0; i<m_arrTabWnds.GetCount(); i++) {
	//			pPane = m_arrTabWnds[i];
	//			if(pPane && pPane->IsFloating())
	//				pPane->ShowPane(nIndex==(i+1) ? TRUE : FALSE, FALSE, FALSE);
	//		}
	//		ShowMainView(FALSE);

	//		if(pCategory->nCategoryType == CategoryType_ImageHistory) {
	//			CEvHistoryImagePane* pHistPane = (CEvHistoryImagePane*) m_arrTabWnds[pCategory->nIndex-1];
	//			if(pHistPane) {
	//				CMFCRibbonComboBox* pCombo = (CMFCRibbonComboBox*) m_wndRibbonBar.FindByID(ID_NEWWINDOW_IMAGEHISTORY+1);
	//				pCombo->SelectItem(pHistPane->m_nSourceImage);
	//				pCombo = (CMFCRibbonComboBox*) m_wndRibbonBar.FindByID(ID_NEWWINDOW_IMAGEHISTORY+2);
	//				pCombo->SelectItem(pHistPane->m_nDepth-1);
	//			}
	//		}
	//		else if(pCategory->nCategoryType == CategoryType_CpuLoad) {
	//		}
	//		else if(pCategory->nCategoryType == CategoryType_MessageLogger) {
	//		}
	//	}
	//	m_nActiveCategory = nIndex;

	//	//m_wndRibbonBar.Maximize(pCategory->nCategoryType != CategoryType_Statistics);
	//}
	//if(!m_wndRibbonBar.IsMaximized() && (pCategory->nCategoryType!=CategoryType_Multicast)) {
	//	keybd_event(VK_MENU, 0, 0, 0);
	//	keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
	//	keybd_event(VK_MENU, 0, 0, 0);
	//	keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
	//}

	return 0;
}

LRESULT CEvMainFrame::OnPaneClosed(WPARAM wParam, LPARAM lParam)
{
	CDockablePane* pPane = (CDockablePane*) wParam;
	for(int i=0; i<m_arrTabWnds.GetCount(); i++) {
		if(m_arrTabWnds[i] == pPane) {
			int nIndex = m_wndRibbonBar.GetCategoryIndex(m_wndRibbonBar.GetActiveCategory());
			CEvRibbonCategory* pCategory = m_arrRibbonCategories[nIndex];
			if((pCategory->nCategoryType == CategoryType_MainView) || (pCategory->nCategoryType == CategoryType_Statistics))
				return -1;

			PostMessage(WM_RIBBON_REMOVECATEGORY, m_wndRibbonBar.GetCategoryIndex(m_wndRibbonBar.GetActiveCategory()));

			pPane->SendMessage(WM_CLOSE);
			break;
		}
	}

	return 0;
}


LRESULT CEvMainFrame::OnVisionPaneClosed(WPARAM wParam, LPARAM lParam)
{
	if((int)lParam == 0)
		bIsPropertiesVisiblePane = FALSE;
	else if((int)lParam == 1)
		bIsPropertiesVisiblePaneTeach = FALSE;
	else if((int)lParam == 2)
		bIsPropertiesVisiblePaneDefect = FALSE;

	return 0;
}



void CEvMainFrame::Initialize(CEvTrackHandler** pTracks, int nTracks)
{
	m_pTrackHandlers = pTracks;
	m_nTracksCount = nTracks;
}


void CEvMainFrame::ShowHidePanes(BOOL bShow)
{
	//m_pDebugViewPane->ShowPane(bShow,FALSE,TRUE);
//	ShowHideDebugPane(bShow);
	ShowHideStatPane(bShow);
}


void CEvMainFrame::ShowHideDebugPane(BOOL bShow)
{
	/*if(m_pDebugViewPane->m_hWnd && m_pDebugViewPane->IsWindowVisible() && !m_pDebugViewPane->bIsClosed)
		m_pDebugViewPane->ShowPane(bShow,FALSE,TRUE);*/
}
///Udupa; June'2013
CString GetCurrentVersion(CString strApp)
{
	DWORD dwVerHnd = 0;
	char szFullPath[MAX_PATH];
	GetModuleFileName(NULL, szFullPath, sizeof(szFullPath));
	DWORD dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);

	if(dwVerInfoSize) {
		HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		LPSTR lpstrVffInfo = (LPSTR) GlobalLock(hMem);
		GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpstrVffInfo);

		char strGetName[256];
		char strResult[256] = {0};

		lstrcpy(strGetName, "\\VarFileInfo\\Translation");
		UINT uVersionLen = 0;
		LPSTR lpVersion = NULL;
		BOOL bRetCode = VerQueryValue((LPVOID)lpstrVffInfo,
			(LPSTR) strGetName,
			(void **) &lpVersion,
			(UINT *) &uVersionLen);
		if(bRetCode && uVersionLen && lpVersion) {
			sprintf_s(strResult, "%04x%04x", (WORD)(*((DWORD *)lpVersion)),
				(WORD)(*((DWORD *)lpVersion)>>16));
		}
		else
			lstrcpy(strResult, "041904b0");

		sprintf_s(strGetName, "\\StringFileInfo\\%s\\", strResult);
		lstrcat(strGetName, "FileVersion");
		uVersionLen = 0;
		lpVersion = NULL;
		bRetCode = VerQueryValue((LPVOID)lpstrVffInfo,
			(LPSTR) strGetName,
			(void **) &lpVersion,
			(UINT *) &uVersionLen);

		if(bRetCode && uVersionLen && lpVersion)
			lstrcpy(strResult, lpVersion);
		else
			lstrcpy(strResult, "");
			
			
		CString str = strResult;
		int nTokenPos = 0;
		CString strToken = str.Tokenize(".", nTokenPos);
		int nNum[4] = {0, 0, 0, 0};
		for(int nIndex=0; nIndex<4; nIndex++) {
			if(strToken.IsEmpty())
				break;
			sscanf_s(strToken, "%d", &nNum[nIndex]);
			strToken = str.Tokenize(".", nTokenPos);
		}
		str.Format("%s-Version %d.%02d.%02d.%02d", strApp, nNum[0], nNum[1], nNum[2], nNum[3]);
		return str;
	}

	return strApp;
}

void CEvMainFrame::AddLogEventPane(CString strInfo,int nEventLog) 
{
	m_wndOutputPane->AddToEventLogPane(strInfo,nEventLog);
}

void CEvMainFrame::InitOutputLog()
{
	CRect rect;
	GetDebugPane()->GetWindowRect(&rect);
	DWORD dwNoCloseBarStyle = AFX_DEFAULT_DOCKING_PANE_STYLE & ~AFX_CBRS_AUTOHIDE;
	int cx = (int)(rect.Width());
	int cy = (int)(rect.Height());

	if (!m_wndOutputPane) {
		m_wndOutputPane = new COutputWnd();
		if (!m_wndOutputPane->Create("Log Information", this, CRect(0, 0, 800, 500), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | CBRS_RIGHT | CBRS_FLOAT_MULTI,  /*AFX_CBRS_OUTLOOK_TABS*/AFX_CBRS_REGULAR_TABS, dwNoCloseBarStyle))
		{
			TRACE0("Failed to create Output window\n");
			return; // failed to create
		}
		m_wndOutputPane->EnableDocking(CBRS_ALIGN_RIGHT | CBRS_ALIGN_BOTTOM);
		//m_pDebugViewPane->AttachToTabWnd(GetDebugPane(),DM_SHOW,1);
		//pWnd->FloatPane(CRect(0, 0, rectClient.Width()*0.15, rectClient.Height()*0.15), DM_UNKNOWN, false);
		m_wndOutputPane->DockToWindow(GetDebugPane(), CBRS_RIGHT, CRect(cx / 2, cy / 2, cx, cy));
		//m_wndOutputPane->DockPane((CBasePane*)m_wndOutputPane, CRect(cx, cy, cx, cy), DM_SHOW);
	}

	if (m_wndOutputPane->m_hWnd) {
		if (m_wndOutputPane->IsDocked()) {
			m_wndOutputPane->UndockPane();
			m_wndOutputPane->DockToWindow(GetDebugPane(), CBRS_RIGHT, CRect(cx / 2, cy / 2, cx, cy));
			//DockPane((CBasePane*)m_pDebugViewPane,AFX_IDW_DOCKBAR_BOTTOM,CRect(cx,cy,cx,cy));
		}
		if (m_wndOutputPane->IsFloating()) {
			//m_dlgPaneStats->UndockPane();
			m_wndOutputPane->DockToWindow(GetDebugPane(), CBRS_RIGHT, CRect(cx / 2, cy / 2, cx, cy));
		}
		m_wndOutputPane->ShowPane(FALSE, FALSE, TRUE);
		m_wndOutputPane->m_wndOutputEvent.Invalidate();
		m_wndOutputPane->m_wndCommEvent.Invalidate();
		m_wndOutputPane->m_wndDataLog.Invalidate();
		m_wndOutputPane->bIsClosed = FALSE;
	}
	RecalcLayout();
}

void CEvMainFrame::OnShowOutputLog()
{
	//CRect rectClient;
	//GetDockingManager()->GetClientAreaBounds(rectClient);
	//rectClient.left = rectClient.right - 350;
	////m_pwndProperties->FloatPane(rectClient, DM_UNKNOWN, false);
	//m_wndOutputPane.ShowPane(TRUE, FALSE, TRUE);
	//bIsOutputPaneVisible = TRUE;
	//m_wndOutputPane.bIsClosed = FALSE;
	//RecalcLayout();


		CRect rect;
		GetDebugPane()->GetWindowRect(&rect);
		DWORD dwNoCloseBarStyle = AFX_DEFAULT_DOCKING_PANE_STYLE & ~AFX_CBRS_AUTOHIDE;
		int cx = (int)(rect.Width());
		int cy = (int)(rect.Height());
	
		if(!m_wndOutputPane){
			m_wndOutputPane = new COutputWnd();
			if (!m_wndOutputPane->Create("Log Information", this, CRect(0, 0, 800, 500), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | CBRS_RIGHT | CBRS_FLOAT_MULTI,  /*AFX_CBRS_OUTLOOK_TABS*/AFX_CBRS_REGULAR_TABS, dwNoCloseBarStyle))
			{
				TRACE0("Failed to create Output window\n");
				return; // failed to create
			}
			m_wndOutputPane->EnableDocking(CBRS_ALIGN_RIGHT|CBRS_ALIGN_BOTTOM);
			//m_pDebugViewPane->AttachToTabWnd(GetDebugPane(),DM_SHOW,1);
			//pWnd->FloatPane(CRect(0, 0, rectClient.Width()*0.15, rectClient.Height()*0.15), DM_UNKNOWN, false);
			m_wndOutputPane->DockToWindow(GetDebugPane(),CBRS_RIGHT,CRect(cx/2,cy/2,cx,cy));
			//m_wndOutputPane->DockPane((CBasePane*)m_wndOutputPane, CRect(cx, cy, cx, cy), DM_SHOW);
		}
		
		if(m_wndOutputPane->m_hWnd) {
			if(m_wndOutputPane->IsDocked()){
				m_wndOutputPane->UndockPane();
				m_wndOutputPane->DockToWindow(GetDebugPane(),CBRS_RIGHT,CRect(cx/2,cy/2,cx,cy));
				//DockPane((CBasePane*)m_pDebugViewPane,AFX_IDW_DOCKBAR_BOTTOM,CRect(cx,cy,cx,cy));
			}
			if(m_wndOutputPane->IsFloating()){
				//m_dlgPaneStats->UndockPane();
				m_wndOutputPane->DockToWindow(GetDebugPane(),CBRS_RIGHT,CRect(cx/2,cy/2,cx,cy));
			}
			m_wndOutputPane->ShowPane(TRUE, FALSE, TRUE);
			m_wndOutputPane->m_wndOutputEvent.Invalidate();
			m_wndOutputPane->m_wndCommEvent.Invalidate();
			m_wndOutputPane->m_wndDataLog.Invalidate();
			m_wndOutputPane->bIsClosed = FALSE;
		}
		RecalcLayout();
}



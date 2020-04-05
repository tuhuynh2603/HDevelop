/// MainFrm.cpp : implementation of the CMainFrame class
/// Udupa; April'2012

#include "stdafx.h"
#include "Application.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include "MsgCode.h"

#include "MainFrame.h"
#include "EvChildFrame.h"
#include "StatisticsView.h"
#include "ImageSelectionDlg.h"
#include "EvDebugViewPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CEvMainFrame)

BEGIN_MESSAGE_MAP(CMainFrame, CEvMainFrame)
	ON_WM_CLOSE()
	ON_WM_WINDOWPOSCHANGING()
	ON_COMMAND(ID_WINDOW_MANAGER,OnWindowManager)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CEvMainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CEvMainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(IDC_ORB_BUTTON, &CEvMainFrame::OnOrbButton)
	ON_COMMAND(ID_FULLSCREEN, &CEvMainFrame::OnOrbFullScreen)
	ON_COMMAND(ID_OUTPUT_LOG, &CEvMainFrame::OnShowOutputLog)
	ON_UPDATE_COMMAND_UI(ID_OUTPUT_LOG,OnUpdateShowOutputLog)
	ON_COMMAND(ID_SHOW_IMAGES,ShowPreviousImages)
	ON_UPDATE_COMMAND_UI(ID_SHOW_IMAGES,OnUpdateShowPreviousImages)
	ON_UPDATE_COMMAND_UI(ID_FULLSCREEN,OnUpdateOrbFullScreen)
	ON_COMMAND(ID_SHOW_STATISTICS,ShowStatsPane)
	ON_UPDATE_COMMAND_UI(ID_SHOW_STATISTICS,OnUpdateShowStatsPane)
	
	ON_MESSAGE(WM_RESIZE_DIE_MAPPING, OnResizeDieMapping)

	ON_MESSAGE(WM_LOAD_RECIPE_SERVER, OnLoadServerRecipe)
	ON_MESSAGE(WM_LOAD_RECIPE, OnLoadRecipe)
	ON_MESSAGE(WM_LOAD_SINGLE_FOV_RECIPE, OnLoadSingleFovRecipe)
	ON_MESSAGE(WM_HIDE_PANES, OnHidePanes)
	ON_MESSAGE(WM_SEND_LIGHTVALUE, OnSendLightValue)
	ON_MESSAGE(WM_SHOW_MOTORINITIALIZING, OnShowMotorInitializing)
	ON_MESSAGE(WM_SET_CUR_POS_PARAM_DISP, OnSetCurPosParamDisplay)
	ON_MESSAGE(WM_SET_ONLINE_STATUS, OnSetOnlineStatus)
	ON_MESSAGE(WM_SET_FULLSCREEN, OnViewFullScreen)
	ON_MESSAGE(WM_ADD_EVENT_LOG, OnAddEventLogPane)
	ON_MESSAGE(WM_SET_RECIPE_NAME, OnUpdateRecipeName)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	///m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLACK);
	m_nAppLook = ID_VIEW_APPLOOK_OFF_2007_BLACK;

	m_bFullScreen = FALSE;
	m_dlgPane = NULL;
	m_dlgPaneStats = NULL;
	
}


CMainFrame::~CMainFrame()
{
	if (m_dlgPaneStats != NULL) {
		delete m_dlgPaneStats;
		m_dlgPaneStats = NULL;
	}

	if (m_dlgPane != NULL) {
		delete m_dlgPane;
		m_dlgPane = NULL;
	}
}

void CMainFrame::InitializeApp()
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	pApp->ConfigureLightControllers();
}


int CMainFrame::CreateRibbon()
{
	
	CApplication* pApp = (CApplication*) AfxGetApp();
	// set the visual manager and style based on persisted value
	OnApplicationLook(m_nAppLook);



	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	

	m_pwndProperties = new CAppPropWnd();

	m_pwndTeachProperties = new CAppTeachPropWnd();

	m_pwndDefectProperties = new CAppDefectPropWnd();
	
	CFrameWnd* pWnd = (CFrameWnd*)GetActiveFrame();
	pWnd->EnableDocking(CBRS_ALIGN_ANY);
	if (!m_wndOpticsPanel.Create(this, IDD_OPTICS_DLGBAR, CBRS_ALIGN_TOP | CBRS_FLYBY | SWP_NOZORDER, IDD_OPTICS_DLGBAR)) {
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}
	m_wndOpticsPanel.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndOpticsPanel);
	CString msg;
	//	msg.LoadString(IDS_STRING61448);
	m_wndOpticsPanel.SetWindowText("Focusing Tool");
	m_wndOpticsPanel.ShowWindow(SW_HIDE);
//	OnNewwindowMessagelogger();
//	pWndDebugViewer = new 

	
	return 0;
}

void CMainFrame::ShowHideStatPane(BOOL bShow)
{
	m_bIsMaximized = !bShow;
	if(!m_dlgPane->bIsClosed)
		m_dlgPane->ShowPane(bShow,FALSE,TRUE);
	if(!m_dlgPaneStats->bIsClosed)
		m_dlgPaneStats->ShowPane(bShow,FALSE,TRUE);
	/*if (!m_wndOutputPane->bIsClosed)
		m_wndOutputPane->ShowPane(bShow, FALSE, TRUE);*/
	//ShowHideDebugPane(bShow);
}

void CMainFrame::ShowMainView(BOOL bShow)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	CEvChildFrame** pFrames = (CEvChildFrame**) pApp->m_pFrames;
	UINT nShow = bShow ? SW_SHOW : SW_HIDE;
	for(int i=pApp->m_nTotalViews; i>=0; i--) {
		if((pFrames[i] != NULL) && ::IsWindow(pFrames[i]->m_hWnd)) {
			pFrames[i]->ShowWindow(nShow);
		}
	}
	if(!bShow) {
		OnUpdateStatusBar((WPARAM) 0, (LPARAM) &CString(""));
		OnUpdateStatusBar((WPARAM) 1, (LPARAM) &CString(""));
		OnUpdateStatusBar((WPARAM) 2, (LPARAM) &CString(""));
		OnUpdateStatusBar((WPARAM) 3, (LPARAM) &CString(""));
		SetWindowText("");
	}
}

CDockablePane* CMainFrame::GetDebugPane()
{
	return m_dlgPaneStats;
}

void CMainFrame::OnClose()
{
	
	if(!m_bRemoteClosed) {
		CApplication* pApp = (CApplication*) AfxGetApp();
		if(pApp->m_bOnline)
			return;

		if (pApp->m_bLotOpened) {
			if (AfxMessageBox("Lot is Opened. Do you want to Save Statistics Information", MB_ICONQUESTION | MB_OKCANCEL) == IDOK) {
				m_dlgPaneStats->m_wndStatsDlg.UpdateLotInfo();
				m_dlgPaneStats->m_wndStatsDlg.UpdateTotalLotInfo();
				pApp->AddLogEvent("FCI_4X Vision Statistics Saved");
			}
			
		}

		if(AfxMessageBox("Confirm to close FCI_4X Vision application", MB_ICONQUESTION | MB_OKCANCEL) == IDOK) {
			pApp->AddLogEvent("FCI_4X Vision Application closed");
		}
		else
			return;
	}
	if (m_dlgPaneStats->IsWindowVisible())
	{
		m_dlgPaneStats->OnPressCloseButton();
	}
	if (m_dlgPane->IsWindowVisible())
	{
		m_dlgPane->OnPressCloseButton();
	}
	ShowTaskBar(TRUE);
	CEvMainFrame::OnClose();
}

int gcx=0, gcy=0;
void CMainFrame::ResizeChildren()
{
	CEvMainFrame::ResizeChildren();
	return;

	CApplication* pApp = (CApplication*) AfxGetApp();
	if(!pApp->m_bLoadComplete)
		return;

	CRect rectClient;
	GetDockingManager()->GetClientAreaBounds(rectClient);

	if(pApp->m_nTotalViews == 1) {
		int cx = (int)(rectClient.Width()/5.0);
		int cy = rectClient.Height();
		if((gcx != cx) || (gcy != cy)) {
			CEvChildFrame** pFrames = (CEvChildFrame**) pApp->m_pFrames;
			int arrx[2] = { 0, cx*4 };
			int arry[2] = { 0,  0 };
			int arrcx[2] = { cx*4, cx };
			int arrcy[2] = { cy,   cy };
			/*if(!pApp->m_pStatsView)
				arrcx[0] = rectClient.Width();

			if(!pApp->m_pStatsView)
				arrcx[0] += cx;*/
			for(int i=pApp->m_nTotalViews; i>=0; i--) {
				if((pFrames[i] != NULL) && ::IsWindow(pFrames[i]->m_hWnd)) {
					if(pFrames[i]->IsZoomed() /*&& pApp->m_pStatsView*/)
						pFrames[i]->MDIRestore();
					pFrames[i]->SetWindowPos(NULL, arrx[i], arry[i], arrcx[i], arrcy[i], SWP_NOZORDER | SWP_NOACTIVATE);
				}
			}
			gcx = cx;
			gcy = cy;
		}
	}
	else if(pApp->m_nTotalViews==2) {
		int cx = rectClient.Width() / 2;
		int cy = rectClient.Height() * 0.80;
		if((gcx != cx) || (gcy != cy)) {
			CEvChildFrame** pFrames = (CEvChildFrame**) pApp->m_pFrames;
			int arrx[3] = { 0, cx,  0 };
			int arry[3] = { 0,  0,  cy, };
			int arrcx[3] = { cx, cx, cx*2 };
			int arrcy[3] = { cy, cy, rectClient.Height() - cy};
			/*if(!pApp->m_pStatsView) {
				arrcy[0] = rectClient.Height();
				arrcy[1] = arrcy[0];
			}*/

			for(int i=pApp->m_nTotalViews; i>=0; i--) {
				if((pFrames[i] != NULL) && ::IsWindow(pFrames[i]->m_hWnd)) {
					if(pFrames[i]->IsZoomed())
						pFrames[i]->MDIRestore();
					pFrames[i]->SetWindowPos(NULL, arrx[i], arry[i], arrcx[i], arrcy[i], SWP_NOZORDER | SWP_NOACTIVATE);
				}
			}
			gcx = cx;
			gcy = cy;
		}
	}
	//else if(pApp->m_nTotalViews==3) {
	//	int cx = rectClient.Width() / 2;
	//	int cy = rectClient.Height() / 2;
	//	if((gcx != cx) || (gcy != cy)) {
	//		CEvChildFrame** pFrames = (CEvChildFrame**) pApp->m_pFrames;
	//		int arrx[4] = { 0, cx,  0, cx };
	//		int arry[4] = { 0,  0,  cy, cy };
	//		int arrcx[4] = { cx, cx, cx, cx };
	//		int arrcy[4] = { cy, cy, cy, cy };
	//		if(!pApp->m_pStatsView)
	//			arrcy[1] = cy*2;

	//		for(int i=pApp->m_nTotalViews; i>=0; i--) {
	//			if((pFrames[i] != NULL) && ::IsWindow(pFrames[i]->m_hWnd)) {
	//				if(pFrames[i]->IsZoomed())
	//					pFrames[i]->MDIRestore();
	//				pFrames[i]->SetWindowPos(NULL, arrx[i], arry[i], arrcx[i], arrcy[i], SWP_NOZORDER | SWP_NOACTIVATE);
	//			}
	//		}
	//		gcx = cx;
	//		gcy = cy;
	//	}
	//}
	else if(pApp->m_nTotalViews==3) {
		int cx = rectClient.Width() / 3;
		int cy = rectClient.Height() / 1.7;
		if((gcx != cx) || (gcy != cy)) {
			CEvChildFrame** pFrames = (CEvChildFrame**) pApp->m_pFrames;
			int arrx[4] = { 0, cx,  cx<<1, 0 };
			int arry[4] = { 0,  0,  0, cy };
			int arrcx[4] = { cx, cx, cx, /*rectClient.Width()*/cx};
			int arrcy[4] = { cy, cy, cy, /*rectClient.Height()*0.51*/(rectClient.Height()-cy)};
			/*if(!pApp->m_pStatsView)
				arrcy[1] = cy*2;*/

			for(int i=pApp->m_nTotalViews; i>=0; i--) {
				if((pFrames[i] != NULL) && ::IsWindow(pFrames[i]->m_hWnd)) {
					if(pFrames[i]->IsZoomed())
						pFrames[i]->MDIRestore();
					pFrames[i]->SetWindowPos(NULL, arrx[i], arry[i], arrcx[i], arrcy[i], SWP_NOZORDER | SWP_NOACTIVATE);
				}
			}
			gcx = cx;
			gcy = cy;
		}
	}
	else if(pApp->m_nTotalViews == 4) {
		int cx = rectClient.Width() / 5;
		int cy = rectClient.Height() / 2;
		if((gcx != cx) || (gcy != cy)) {
			CEvChildFrame** pFrames = (CEvChildFrame**) pApp->m_pFrames;
			int arrx[5] = { 0, 2*cx,  0, 2*cx, 4*cx};
			int arry[5] = { 0,  0, cy, cy, 0 };
			int arrcx[5] = { 2*cx, 2*cx, 2*cx, 2*cx, cx};
			int arrcy[5] = { cy, cy, cy, cy, 2*cy};
			for(int i=pApp->m_nTotalViews; i>=0; i--) {
				if((pFrames[i] != NULL) && ::IsWindow(pFrames[i]->m_hWnd)) {
					if(pFrames[i]->IsZoomed())
						pFrames[i]->MDIRestore();
					pFrames[i]->SetWindowPos(NULL, arrx[i], arry[i], arrcx[i], arrcy[i], SWP_NOZORDER | SWP_NOACTIVATE);
				}
			}
			gcx = cx;
			gcy = cy;
		}
	}
	else if(pApp->m_nTotalViews == 5) {
		int cx = (int)(rectClient.Width()/3.0);
		int cy = rectClient.Height() / 2;
		if((gcx != cx) || (gcy != cy)) {
			CEvChildFrame** pFrames = (CEvChildFrame**) pApp->m_pFrames;
			int arrx[6] = { 0, cx,  0, cx, cx<<1, cx<<1 };
			int arry[6] = { 0,  0, cy, cy,    cy,     0 };
			int arrcx[6] = { cx, cx, cx, cx, cx, cx };
			int arrcy[6] = { cy, cy, cy, cy, cy, cy };
			for(int i=pApp->m_nTotalViews; i>=0; i--) {
				if((pFrames[i] != NULL) && ::IsWindow(pFrames[i]->m_hWnd)) {
					if(pFrames[i]->IsZoomed())
						pFrames[i]->MDIRestore();
					pFrames[i]->SetWindowPos(NULL, arrx[i], arry[i], arrcx[i], arrcy[i], SWP_NOZORDER | SWP_NOACTIVATE);
				}
			}
			gcx = cx;
			gcy = cy;
		}
	}
	else if(pApp->m_nTotalViews == 10) {
		int cx = (int)(rectClient.Width()/4.0)-1;
		int cy = rectClient.Height() / 3;
		int cx2 = cx*2 + 4;
		int cy2 = cy*2;
		int cx3 = cx*3 + 4;
		if((gcx != cx) || (gcy != cy)) {
			CEvChildFrame** pFrames = (CEvChildFrame**) pApp->m_pFrames;
			//int arrx[11] = { 0,  0, cx,   0,  cx, cx3, cx2, cx3, cx2, cx3, cx };
			//int arry[11] = { 0, cy, cy, cy2, cy2,   0,  cy,  cy, cy2, cy2, 0 };
			int arrx[11] = { 0, cx,  0, cx,  0,  cx2, cx3, cx2, cx3, cx3,  cx };
			int arry[11] = { 0,  0, cy, cy, cy2,   0,   0,  cy,  cy, cy2, cy2 };
			//int arrx[11] = { 0, cx, cx2, cx3,  0, cx3,   0,  cx, cx2, cx3, cx };
			//int arry[11] = { 0,  0,   0,   0, cy,  cy, cy2, cy2, cy2, cy2, cy };

			int arrcx[11] = { cx, cx, cx, cx, cx, cx, cx, cx, cx, cx, cx2 };
			int arrcy[11] = { cy, cy, cy, cy, cy, cy, cy, cy, cy, cy, cy };
			for(int i=pApp->m_nTotalViews; i>=0; i--) {
				if((pFrames[i] != NULL) && ::IsWindow(pFrames[i]->m_hWnd)) {
					if(pFrames[i]->IsZoomed())
						pFrames[i]->MDIRestore();
					pFrames[i]->SetWindowPos(NULL, arrx[i], arry[i], arrcx[i], arrcy[i], SWP_NOZORDER | SWP_NOACTIVATE);
				}
			}
			gcx = cx;
			gcy = cy;
		}
	}

	int nImagesViewWidth, nImageViewHeight;
	nImagesViewWidth = (int) (rectClient.Width()/3);
	nImageViewHeight = (int) (rectClient.Height()/1.7);

	pApp->m_nImagesViewCX = nImagesViewWidth*2;//(int) rectClient.Width() * 0.4;
	pApp->m_nImagesViewCY = rectClient.Height() - nImageViewHeight;//(int) rectClient.Height() * 0.75;
	if(pApp->m_bImagesViewInitialised) {
		CRect rect1;
		//pApp->m_pStatsView->GetWindowRect(&rect1);
//		pApp->m_pStatsView->ScreenToClient(&rect1);
		//pApp->m_pImageSelectionDlg->SetWindowPos(NULL,rect1.top*1.33,rect1.right * 0.34/*pApp->m_nImagesViewCY*0.75*/,rect1.right *0.549, rect1.bottom*0.329, SWP_NOZORDER);
//		pApp->m_pImageSelectionDlg->SetWindowPos(NULL, rect1.right, rect1.top, pApp->m_nImagesViewCX, pApp->m_nImagesViewCY, SWP_NOZORDER);
		//pApp->m_pImageSelectionDlg->SetSize();
	}

}

void CMainFrame::OnApplicationLook(UINT id)
{
	//m_nAppLook = id;
	SetApplicationLook(8);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::RecalculateLayout()
{
	RecalcLayout(TRUE);
}

void CMainFrame::ToggleMulticast()
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	char str[16];
	memset(str, 0, 16);
	for(int i=0; i<pApp->m_nTotalTracks; i++) {
		if(m_bEnableMulticast)
			sprintf_s(str, "239.0.0.%d", i+1);
		EnableMulticast(&pApp->m_pTracks[i].m_FGResource, str);
	}
	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "MultiCast", m_bEnableMulticast);
}

BOOL CMainFrame::IsNewTabAvailable()
{
	CApplication* pApp = (CApplication*) AfxGetApp();

	return (pApp->CheckPassword()==0);
}

LRESULT CMainFrame::OnLoadServerRecipe(WPARAM wParam, LPARAM lParam)
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	pApp->LoadServerRecipes();

	return 0;
}

LRESULT CMainFrame::OnResizeDieMapping(WPARAM wParam, LPARAM lParam)
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	
	int nDeviceConfigSize = sizeof(CentralizedDeviceConfigInfo);
	uint8_t* pDeviceConfigBuf = new uint8_t[nDeviceConfigSize];
	std::memcpy(pDeviceConfigBuf, (uint8_t*)wParam, nDeviceConfigSize);
	CentralizedDeviceConfigInfo *pCSDeviceConfig = reinterpret_cast<CentralizedDeviceConfigInfo*>(pDeviceConfigBuf);
	pApp->motionParameters.SetMappingParameters(pCSDeviceConfig->nDevicePerX, pCSDeviceConfig->nDevicePerY, 
									pCSDeviceConfig->nFovPerX, pCSDeviceConfig->nFovPerY, pCSDeviceConfig->bHorizontalDevice);

	pApp->motionParameters.CreateScanPositions();
	pApp->ResizeDieSelectionDlg();
	pApp->ClearResults();

	return 0;
}

LRESULT CMainFrame::OnLoadRecipe(WPARAM wParam, LPARAM lParam)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	pApp->LoadRecipes();

	return 0;
}

LRESULT CMainFrame::OnLoadSingleFovRecipe(WPARAM wParam, LPARAM lParam)
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	pApp->LoadRecipes((int)wParam, (int)lParam);

	return 0;
}

LRESULT CMainFrame::OnSendLightValue(WPARAM wParam, LPARAM lParam)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	int nError = pApp->m_LightModule.OutPortWriteString((int) wParam, 0, (CString*) lParam, 0);

	return nError;
}


void CMainFrame::ShowPreviousImages()
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	CRect rect;
	m_dlgPaneStats->GetWindowRect(&rect);
	int cx = (int)(rect.Width());
	int cy = (int)(rect.Height());
	DWORD dwNoCloseBarStyle = AFX_DEFAULT_DOCKING_PANE_STYLE & ~AFX_CBRS_AUTOHIDE;

	

	if(!m_dlgPane){
		m_dlgPane = new CDialogPane;
		UINT style = WS_CHILD | CBRS_RIGHT |CBRS_FLOAT_MULTI;
		if (!m_dlgPane->Create("Die View", this, CRect(0,0,600,400), TRUE,
			IDD_IMAGESELECTION_DLG, style,AFX_CBRS_REGULAR_TABS,dwNoCloseBarStyle))
		{
			TRACE0("Failed to create dialog pane\n");
			delete m_dlgPane;
			return ; 
		}

		pApp->m_pImageSelectionDlg = m_dlgPane;
	
		m_dlgPane->EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT|CBRS_ALIGN_BOTTOM);
		m_dlgPane->DockToWindow(m_dlgPaneStats,CBRS_RIGHT,CRect(cx*0.25,cy*0.25,cx,cy));
		//DockPane((CBasePane*)m_dlgPaneStats,AFX_IDW_DOCKBAR_BOTTOM,CRect(cx*2,cy,cx,cy));
	}
	if(m_dlgPane->IsDocked()){
		m_dlgPane->UndockPane();
		m_dlgPane->DockToWindow(m_dlgPaneStats,CBRS_RIGHT,CRect(cx*0.25, cy*0.25,cx,cy));
		
		//DockPane((CBasePane*)m_dlgPaneStats,AFX_IDW_DOCKBAR_BOTTOM,CRect(cx*2,cy,cx,cy));
	}
	if(m_dlgPane->IsFloating()){
		//m_dlgPaneStats->UndockPane();
		m_dlgPane->DockToWindow(m_dlgPaneStats,CBRS_RIGHT,CRect(cx*0.25, cy*0.25,cx,cy));
	}
	if(!m_dlgPane->bHideEnable)
		m_dlgPane->ShowPane(TRUE,FALSE,TRUE);
	else
		m_dlgPane->ShowPane(FALSE,FALSE,TRUE);
	m_dlgPane->m_wndDlg.Invalidate();
	m_dlgPane->bIsClosed = FALSE;

	if (m_wndOutputPane && m_wndOutputPane->IsDocked()) {
		m_wndOutputPane->ShowPane(FALSE, FALSE, FALSE);
		m_wndOutputPane->UndockPane();
	}

	/*if (m_wndOutputPane && !m_wndOutputPane->bIsClosed) {
		OnShowOutputLog();
		m_wndOutputPane->m_nMaxLines = pApp->m_nMaxLineOutputLog+1;
	}*/
		
	//OnNewwindowImageHistory();
	RecalcLayout();

}

void CMainFrame::OnUpdateShowPreviousImages(CCmdUI *pCmdUI)
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	pCmdUI->Enable(/*m_dlgPane->bIsClosed  && */!m_bIsMaximized && pApp->m_UserAccInfo.m_nCurrentAccessLevel >= pApp->m_AdvSecurityDlg.m_data.m_nShowDieCfg);
}


void CMainFrame::OnUpdateShowStatsPane(CCmdUI *pCmdUI)
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	pCmdUI->Enable(/*m_dlgPaneStats->bIsClosed  && */!m_bIsMaximized && pApp->m_UserAccInfo.m_nCurrentAccessLevel >= pApp->m_AdvSecurityDlg.m_data.m_nShowStats);
}

void CMainFrame::ShowStatsPane()
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	DWORD dwNoCloseBarStyle = AFX_DEFAULT_DOCKING_PANE_STYLE & ~AFX_CBRS_AUTOHIDE;
	if(!m_dlgPaneStats){
		m_dlgPaneStats = new CDialogPaneStatsview;
			UINT style = WS_CHILD | CBRS_RIGHT |CBRS_FLOAT_MULTI;
		if (!m_dlgPaneStats->Create("Statistics", this,CRect(0,0,600,400), TRUE,
			IDD_STATISTICS_FORMVIEW, style,AFX_CBRS_REGULAR_TABS,dwNoCloseBarStyle))
		{
			TRACE0("Failed to create dialog pane\n");
			delete m_dlgPane;
			return; 
		}
		
		m_dlgPaneStats->EnableDocking(CBRS_ALIGN_ANY);
		DockPane((CBasePane*)m_dlgPaneStats,AFX_IDW_DOCKBAR_BOTTOM, CRect(0, 0, 600, 400));
		CDockablePane* pTabbedBar = NULL;
	}
	if (m_dlgPaneStats->bRefreshPane) {
		m_dlgPaneStats->m_wndStatsDlg.OnInitialUpdate();
		m_dlgPaneStats->m_wndStatsDlg.Initialize(TRUE);
		m_dlgPaneStats->m_wndStatsDlg.InitializeLotStats();
		m_dlgPaneStats->m_wndStatsDlg.UpdateContents();
		m_dlgPaneStats->m_wndStatsDlg.UpdateStatsInfo();

		if (!pApp->m_bIsMasterApp) {
			m_dlgPaneStats->m_wndStatsDlg.UpdateCentralizedContents();
		}

		//m_dlgPaneStats->m_wndStatsDlg.Initialize(TRUE);
		m_dlgPaneStats->bRefreshPane = FALSE;
	}
	else {
		if (m_dlgPane && m_dlgPane->IsDocked()) {
			m_dlgPane->ShowPane(FALSE, FALSE, FALSE);
			m_dlgPane->UndockPane();
		}
		if (m_dlgPaneStats->IsDocked()) {
			m_dlgPaneStats->ShowPane(FALSE, FALSE, FALSE);
			m_dlgPaneStats->UndockPane();
			DockPane((CBasePane*)m_dlgPaneStats, AFX_IDW_DOCKBAR_BOTTOM, CRect(0, 0, 600, 400));
		}
		if (m_dlgPaneStats->IsFloating()) {
			//m_dlgPaneStats->UndockPane();
			DockPane((CBasePane*)m_dlgPaneStats, AFX_IDW_DOCKBAR_BOTTOM, CRect(0, 0, 600, 400));
		}
		m_dlgPaneStats->ShowPane(TRUE, FALSE, TRUE);
		m_dlgPaneStats->bIsClosed = FALSE;
		//OnNewwindowImageHistory();
		if (m_dlgPane && !m_dlgPane->bIsClosed)
			ShowPreviousImages();
		RecalcLayout();
	}
	if (m_wndOutputPane) {
		m_wndOutputPane->m_nMaxLines = pApp->m_nMaxLineOutputLog + 1;
	}
}


LRESULT CMainFrame::OnShowMotorInitializing(WPARAM wParam, LPARAM lParam)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	pApp->ShowMotionInitializing();

	return 0;
}


void CMainFrame::OnMaximizeDoc(BOOL bTrue)
{
	ShowHidePanes(bTrue);

}

LRESULT CMainFrame::OnHidePanes(WPARAM wParam, LPARAM lParam)
{
	ShowHidePanes(wParam);
	
	return 0;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	//int nShiftKey = GetKeyState(VK_SHIFT) & 0x8000;

	if(pMsg->message == WM_KEYDOWN) {
		if(/*nShiftKey &&*/(pMsg->wParam == VK_LEFT) || (pMsg->wParam == VK_RIGHT) ||
		  (pMsg->wParam == VK_UP) || (pMsg->wParam == VK_DOWN)) {
				if(pApp->NavigateDieImages((int)pMsg->wParam))
					return TRUE;
		}
		else if(pMsg->wParam == VK_RETURN ||  pMsg->wParam == VK_ESCAPE) {
			if(pMsg->wParam == VK_ESCAPE && pApp->m_bPixelRuler) {
				pApp->OnPixelRuler();
			}
			else {
				pApp->StepDebugNavigate((int)pMsg->wParam);
			}
			return TRUE;
		}
		
	}
	return FALSE;
}

LRESULT CMainFrame::OnSetCurPosParamDisplay(WPARAM wParam, LPARAM lParam)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	pApp->SetCurPosParamDisplay((int) wParam, (CString*)lParam);

	return 0;
}


LRESULT CMainFrame::OnSetOnlineStatus(WPARAM wParam, LPARAM lParam)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	pApp->SetOnlineStatus();

	return 0;
}


LRESULT CMainFrame::OnViewFullScreen(WPARAM wParam, LPARAM lParam)
{
	ViewFullScreen(wParam,lParam);
	return 0;
}


void CMainFrame::DisplayOpticsPanel(BOOL bVisible, int nHeight)
{
	if (bVisible) {
		DockControlBar(&m_wndOpticsPanel);
		m_wndOpticsPanel.ShowWindow(SW_SHOW);

		CMDIChildWnd* pWnd = (CMDIChildWnd *)GetActiveFrame();
		CRect rect1, rect2, rect3;
		SystemParametersInfo(SPI_GETWORKAREA, NULL, &rect1, 0);
		m_wndOpticsPanel.GetWindowRect(&rect2);
		//	m_wndToolBar.GetWindowRect(&rect3);

		FloatControlBar(&m_wndOpticsPanel, CPoint(rect1.Width() - (rect2.Width()<10 ? 240 : rect2.Width() + 2), rect3.top), CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	}
	else {
		DockControlBar(&m_wndOpticsPanel);
		m_wndOpticsPanel.ShowWindow(SW_HIDE);
	}
}



LRESULT CMainFrame::OnAddEventLogPane(WPARAM wParam, LPARAM lParam)
{
	CString*  strInfo = (CString *)wParam;
	int nEventLog = lParam;
	AddLogEventPane(*strInfo,nEventLog);
	
	return 0;
}

void CMainFrame::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CApplication* pApp = (CApplication*)AfxGetApp();

	//int nCurrentLevel = pApp->m_UserAccInfo.m_nCurrentAccessLevel;
	//if((nCurrentLevel >= pApp->m_AdvSecurityDlg.m_data.m_nTaskBar))
	//lpwndpos->flags = lpwndpos->flags | SWP_NOMOVE;
	//else
	lpwndpos->flags = lpwndpos->flags |SWP_NOSIZE;
	CEvMainFrame::OnWindowPosChanging(lpwndpos);
}

void CMainFrame::OnUpdateShowOutputLog(CCmdUI *pCmdUI)
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	pCmdUI->Enable(pApp->m_UserAccInfo.m_nCurrentAccessLevel >= pApp->m_AdvSecurityDlg.m_data.m_nOutPutLog);
}


void CMainFrame::OnUpdateRibbonTab()
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	for (int i = 0; i < pApp->m_nTotalViews; i++) {
		if (pApp->m_pFrames[i]->IsZoomed()) {
			pApp->m_pFrames[i]->MDIRestore();
			pApp->m_pFrames[i]->MDIMaximize();
		}
	}
}

LRESULT CMainFrame::OnUpdateRecipeName(WPARAM wParam, LPARAM lParam)
{
	CMFCRibbonBaseElement* pElement = m_wndRibbonBar.FindByID(ID_RECIPE_BUTTON, TRUE, TRUE);
	CString*  strInfo = (CString *)wParam;
	pElement->SetText(*strInfo);
	CMFCRibbonBaseElement* pElement1 = m_wndRibbonBar.FindByID(ID_PRODUCT_CODE_BUTTON, TRUE, TRUE);
	strInfo = (CString *)lParam;
	pElement1->SetText(*strInfo);
	m_wndRibbonBar.ForceRecalcLayout();
	return 0;
}


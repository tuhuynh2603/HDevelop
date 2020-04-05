/// EvChildFrame.cpp : implementation of the CEvChildFrame class
/// Udupa; April'2012

#include "stdafx.h"
#include "EvChildFrame.h"
#include "EvTabPane.h"
#include "MsgCode.h"
#include "EvMainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEvChildFrame

IMPLEMENT_DYNCREATE(CEvChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CEvChildFrame, CMDIChildWndEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ACTIVATE()
	ON_WM_SETFOCUS()
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CEvChildFrame construction/destruction
LRESULT CEvChildFrame::OnSizeParent(WPARAM wParam, LPARAM lParam)
{
	AFX_SIZEPARENTPARAMS* lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;
	
	CRect rectClient(lpLayout->rect);

	if (lpLayout->hDWP != NULL)
		AfxRepositionWindow(lpLayout, m_hWnd, &rectClient);

	return 0;
}

CEvChildFrame::CEvChildFrame()
{
}

CEvChildFrame::~CEvChildFrame()
{
	//SaveBarState("Software\\MonsterVision\\InterojoWetLens\\Preferences\\Persistence\\ChildFrame");
}


BOOL CEvChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	if(!CMDIChildWndEx::PreCreateWindow(cs))
		return FALSE;

	//cs.style = /*WS_CHILD | FWS_ADDTOTITLE;// | WS_SYSMENU;// | WS_CLIPCHILDREN;// | WS_CAPTION;


	//cs.style &= (~WS_SIZEBOX & ~WS_MINIMIZEBOX & ~WS_CAPTION);
	///cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	//cs.dwExStyle |= WS_EX_TOOLWINDOW | WS_EX_MDICHILD | WS_EX_NOACTIVATE;
	//cs.dwExStyle = WS_EX_MDICHILD | WS_EX_TOOLWINDOW;

	//cs.style &=~ WS_SIZEBOX;
	//cs.style &=~ WS_MINIMIZEBOX;
	//cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	//cs.style &= ~WS_CAPTION;
	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
        | FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;


	cs.cy = ::GetSystemMetrics(SM_CYSCREEN) / 3; 
    cs.cx = ::GetSystemMetrics(SM_CXSCREEN) / 3; 
    cs.y = ((cs.cy * 3) - cs.cy) / 2; 
    cs.x = ((cs.cx * 3) - cs.cx) / 2;
    // Trick#2: Add this line for remove the ugly client edge of the child frame.
    cs.dwExStyle &= (~WS_EX_CLIENTEDGE);
	return TRUE;
}

BOOL CEvChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	///CMenu* pMenu = GetSystemMenu(FALSE);
	//pMenu->RemoveMenu(SC_MOVE, MF_BYCOMMAND); // removing this items will prevent the window from moving !!!
	//pMenu->RemoveMenu(SC_CLOSE, MF_BYCOMMAND); // removing this items will prevent the window from moving !!!

	return CMDIChildWndEx::OnCreateClient(lpcs, pContext);
}

void CEvChildFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWndEx::OnSize(nType, cx, cy);
	CMDIChildWndEx::AdjustDockingLayout();
	/*if(cx/2 >= cy) {
		((CMDIFrameWndEx*) AfxGetMainWnd())->SendMessage(WM_PANE_CLOSED, (WPARAM) this, (LPARAM)TRUE);
	}
	else{
		((CMDIFrameWndEx*) AfxGetMainWnd())->SendMessage(WM_PANE_CLOSED, (WPARAM) this, (LPARAM)FALSE);
	}*/
	/*CChildView* pView = (CChildView*) GetActiveView();
	if(pView && IsWindow(pView->m_hWnd))
		pView->FitToWindow();*/
}

void CEvChildFrame::EnableMaximizeBox()
{
	if(!(GetStyle() & WS_MAXIMIZEBOX)) {
		ModifyStyle(WS_CAPTION, WS_MAXIMIZEBOX);
	}
}

// CEvChildFrame diagnostics

#ifdef _DEBUG
void CEvChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CEvChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CEvChildFrame message handlers
#define ID_STATUSBAR_ZOOM_SLIDER 999

int CEvChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;


//	this->EnableDocking(CBRS_ALIGN_RIGHT);
	//static int x=0;
	//m_nTrackId = x++;
	///CreateFrame():
	//pApp->m_pFrames[m_nTrackId] = this;

	//if(x <= pApp->m_nTotalTracks) {
		/*m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EvChildFrame);
		m_wndToolBar.LoadToolBar(IDR_EvChildFrame, 0, 0, TRUE );
		m_wndToolBar.SetPaneStyle( CBRS_TOOLTIPS | CBRS_FLYBY| CBRS_TOP); 
		m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
		EnableDocking(CBRS_ALIGN_ANY);
		DockPane(&m_wndToolBar);*/

		//CMFCRibbonSlider* pSlider = new CMFCRibbonSlider(ID_STATUSBAR_ZOOM_SLIDER);
		//pSlider->SetZoomButtons();
		//pSlider->SetRange(0, 200);
		//pSlider->SetPos(100);
/*
		CMFCToolBarButton* pButton = new CMFCToolBarButton(ID_STATUSBAR_ZOOM_SLIDER, 0, "Zoom");
		pButton->SetStyle(
		m_wndToolBar.InsertButton(
		*/

		///m_wndToolBar.AddPane(AddExtendedElement(pSlider, _T("Zoom Slider"));
	//}

//	if(m_nTrackId == pApp->m_nTotalTracks)
	//	ModifyStyle(0, WS_SYSMENU);
	
	//LoadBarState("Software\\MonsterVision\\InterojoWetLens\\Preferences\\Persistence\\ChildFrame");
	CMDIChildWndEx::EnableDocking(CBRS_ALIGN_ANY);
	return 0;
}

void CEvChildFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
}

void CEvChildFrame::OnSetFocus(CWnd* pOldWnd)
{
}

BOOL CEvChildFrame::OnEraseBkgnd(CDC* pDC) 
{
	//if(pApp->m_pStatsView && !pApp->m_pStatsView->m_bInitialized) {
	//	CRect rect;
	//	GetClientRect(&rect);
	//	HBRUSH hBrush = ::CreateSolidBrush(RGB(0,0,0));
	//	::FillRect(pDC->GetSafeHdc(), rect, hBrush);
	//	::DeleteObject(hBrush);
	//}

	//return TRUE;
	
	return CMDIChildWndEx::OnEraseBkgnd(pDC);
}

void CEvChildFrame::OnClose() 
{
	//SaveBarState("Software\\MonsterVision\\InterojoWetLens\\Preferences\\Persistence\\ChildFrame");
	//CMDIChildWnd::OnClose();
}


BOOL CEvChildFrame::CanShowOnTaskBarTabs()
{
    return FALSE;
}



void CEvChildFrame::SaveBarState(LPCTSTR lpszProfileName) 
{
  const_cast<CEvChildFrame*>(this)->GetDockingManager()->SaveState(lpszProfileName);

    //// Trick#3: we need to call serialization method of CMFCToolBar panes that may be docked to the child frame.
    //CObList list;
    //const_cast<CEvChildFrame*>(this)->GetDockingManager()->GetPaneList(list, FALSE, NULL, FALSE);
    //if (list.GetCount() > 0) {
    //    POSITION pos = list.GetTailPosition();
    //    while (pos != NULL) {
    //        CMFCToolBar* pToolBar = DYNAMIC_DOWNCAST(CMFCToolBar, list.GetPrev(pos));
    //        if (pToolBar != nullptr) {
    //            pToolBar->SaveState(lpszProfileName);
    //        }
    //    }
    //}
}

void CEvChildFrame::LoadBarState(LPCTSTR lpszProfileName)
{
    // Trick#3: we need to call serialization method of CMFCToolBar panes that may be docked to the child frame.
    /*CObList list;
    GetDockingManager()->GetPaneList(list, FALSE, NULL, FALSE);
    if (list.GetCount() > 0) {
        POSITION pos = list.GetTailPosition();
        while (pos != NULL) {
            CMFCToolBar* pToolBar = DYNAMIC_DOWNCAST(CMFCToolBar, list.GetPrev(pos));
            if (pToolBar != nullptr) {
                pToolBar->LoadState(lpszProfileName);
            }
        }
    }*/

    GetDockingManager()->LoadState(lpszProfileName);
    GetDockingManager()->SetDockState();
    GetDockingManager()->ShowDelayShowMiniFrames(TRUE);

    //// Trick#4: MFC BUGFIX: force assigning the child frame docking manager to all miniframes (for details look at http://stackoverflow.com/q/39253843/987850).
    //for (POSITION pos = GetDockingManager()->GetMiniFrames().GetHeadPosition(); pos != NULL;)
    //{
    //    CWnd* pWndNext = (CWnd*)GetDockingManager()->GetMiniFrames().GetNext(pos);
    //    if (pWndNext != nullptr && pWndNext->IsKindOf(RUNTIME_CLASS(CPaneFrameWnd))) {
    //        STATIC_DOWNCAST(CPaneFrameWnd, pWndNext)->SetDockingManager(GetDockingManager());
    //    }
    //}
}

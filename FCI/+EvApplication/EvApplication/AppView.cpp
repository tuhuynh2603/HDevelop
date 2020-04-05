/// AppView.cpp : implementation of the CAppView class
/// Udupa; April'2012

#include "stdafx.h"
#include "Application.h"
#include "AppDoc.h"
#include "AppView.h"
#include "TrackHandler.h"
#include "ImageSelectionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAppView

IMPLEMENT_DYNCREATE(CAppView, CEvImageView)

BEGIN_MESSAGE_MAP(CAppView, CEvImageView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_COMMAND(ID_VIEW_BINARIZE, OnViewBinarize)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BINARIZE, OnUpdateViewBinarize)
	ON_COMMAND(ID_VIEW_ZOOM, OnViewZoom)
	ON_COMMAND(ID_VIEW_ZOOM_IN, OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOM_OUT, OnViewZoomOut)
	ON_COMMAND(ID_VIEW_ZOOM_FIT, OnViewZoomFit)
	ON_COMMAND(ID_VIEW_ZOOM_100, OnViewZoom100)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM, OnUpdateViewZoom)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_IN, OnUpdateViewZoomIn)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_OUT, OnUpdateViewZoomOut)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_FIT, OnUpdateViewZoomFit)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_100, OnUpdateViewZoom100)
	ON_COMMAND(ID_TOOLS_SHOW_MAGNIFIER, OnShowMaginifier)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SHOW_MAGNIFIER, OnUpdateShowMaginifier)
END_MESSAGE_MAP()

// CAppView construction/destruction

CAppView::CAppView()
{
	//m_rcSize = CRect(0,0,250,250);
	//ImgTemp.Allocate(CSize(m_rcSize.Width(),m_rcSize.Height()));

	m_bPixelRuler = FALSE;
}

CAppView::~CAppView()
{
}

BOOL CAppView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CEvImageView::PreCreateWindow(cs);
}

// CAppView drawing

void CAppView::OnDraw(CDC* pDC)
{
	CAppDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CEvImageView::OnDraw(pDC);
	pDoc->SetOpticsCharacteristics();
	//pDoc->SetLightCalibration();
}

void CAppView::OnRButtonUp(UINT nFlags, CPoint point)
{
	CEvImageView::OnRButtonUp(nFlags, point);

	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CAppView::OnContextMenu(CWnd* pWnd, CPoint point)
{
#ifndef SHARED_HANDLERS
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CAppView diagnostics

#ifdef _DEBUG
void CAppView::AssertValid() const
{
	CEvImageView::AssertValid();
}

void CAppView::Dump(CDumpContext& dc) const
{
	CEvImageView::Dump(dc);
}

CAppDoc* CAppView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAppDoc)));
	return (CAppDoc*)m_pDocument;
}
#endif //_DEBUG


void CAppView::UpdateStatusBar(CString* pstrMsg)
{
	CEvImageView::UpdateStatusBar(pstrMsg);
}
// CAppView message handlers


void CAppView::OnViewBinarize()
{
	CAppDoc* pDoc = GetDocument();
	if(GetKeyState(VK_SHIFT) & 0x8000) {
		CApplication* pApp = (CApplication*) AfxGetApp();
		pApp->BinarizeAllTracks(!pDoc->m_bBinaryMode);
	}
	else {
		SendMessage(WM_BINARY_MODE, (WPARAM)!pDoc->m_bBinaryMode, (LPARAM)&pDoc->m_nThreshold);
		/*if (pDoc->m_bBinaryMode)
			BinaryMode((WPARAM)FALSE,(LPARAM)pDoc->m_nThreshold);
		else
			BinaryMode((WPARAM)TRUE,(LPARAM)pDoc->m_nThreshold);*/
	}
}


void CAppView::OnUpdateViewBinarize(CCmdUI *pCmdUI)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	CAppDoc* pDoc = GetDocument();
	pCmdUI->Enable(!pApp->m_bOnline && !pDoc->m_pTrack->m_bContInsp && !pDoc->m_bInspSavedImage
		&& pApp->m_UserAccInfo.m_nCurrentAccessLevel >= pApp->m_AdvSecurityDlg.m_data.m_nBinarize);//&& !pDoc->m_bInspecting

	pCmdUI->SetCheck(GetDocument()->m_bBinaryMode);
}

void CAppView::OnViewZoom()
{
	//ZoomFit();
	CApplication* pApp = (CApplication*) AfxGetApp();
	if(pApp->m_bZoomAll){
		for(int i=0; i<pApp->m_nTotalTracks; i++)
			for(int j=0;j<pApp->m_pTracks[i].m_nTotalDocs;j++)
				pApp->m_pTracks[i].m_pDoc[j]->m_ImageViewInfo.pImageView->ZoomFit();
	}
	else{
		CAppDoc* pDoc = GetDocument();
		pDoc->m_ImageViewInfo.pImageView->ZoomFit();
	}
}

void CAppView::OnUpdateViewZoom(CCmdUI *pCmdUI)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	pCmdUI->Enable(!pApp->m_bOnline);
}

void CAppView::OnViewZoomIn()
{
	
	//ZoomIn();
	CApplication* pApp = (CApplication*) AfxGetApp();
	if(pApp->m_bZoomAll){
		for(int i=0; i<pApp->m_nTotalTracks; i++)
			for(int j=0;j<pApp->m_pTracks[i].m_nTotalDocs;j++)
				pApp->m_pTracks[i].m_pDoc[j]->m_ImageViewInfo.pImageView->ZoomIn();
	}
	else{
		CAppDoc* pDoc = GetDocument();
		pDoc->m_ImageViewInfo.pImageView->ZoomIn();
	}
}

void CAppView::OnUpdateViewZoomIn(CCmdUI *pCmdUI)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	pCmdUI->Enable(!pApp->m_bOnline && pApp->m_UserAccInfo.m_nCurrentAccessLevel >= pApp->m_AdvSecurityDlg.m_data.m_nZoomIn);
}

void CAppView::OnViewZoomOut()
{
	//ZoomOut();
	CAppDoc* pDoc = GetDocument();
	CApplication* pApp = (CApplication*) AfxGetApp();
	if(pApp->m_bZoomAll){
		for(int i=0; i<pApp->m_nTotalTracks; i++)
			for(int j=0;j<pApp->m_pTracks[i].m_nTotalDocs;j++)
				pApp->m_pTracks[i].m_pDoc[j]->m_ImageViewInfo.pImageView->ZoomOut();
	}
	else{
		CAppDoc* pDoc = GetDocument();
		pDoc->m_ImageViewInfo.pImageView->ZoomOut();
	}
}

void CAppView::OnUpdateViewZoomOut(CCmdUI *pCmdUI)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	pCmdUI->Enable(!pApp->m_bOnline && pApp->m_UserAccInfo.m_nCurrentAccessLevel >= pApp->m_AdvSecurityDlg.m_data.m_nZoomOut);
}

void CAppView::OnViewZoomFit()
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	if(pApp->m_bZoomAll){
		for(int i=0; i<pApp->m_nTotalTracks; i++)
			for(int j=0;j<pApp->m_pTracks[i].m_nTotalDocs;j++)
				pApp->m_pTracks[i].m_pDoc[j]->m_ImageViewInfo.pImageView->ZoomFit();
	}
	else{
		CAppDoc* pDoc = GetDocument();
		pDoc->m_ImageViewInfo.pImageView->ZoomFit();
	}
}

void CAppView::OnUpdateViewZoomFit(CCmdUI *pCmdUI)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	pCmdUI->Enable(!pApp->m_bOnline && pApp->m_UserAccInfo.m_nCurrentAccessLevel >= pApp->m_AdvSecurityDlg.m_data.m_nZoomFit);
}

void CAppView::OnViewZoom100()
{
	
	CApplication* pApp = (CApplication*) AfxGetApp();
	if(pApp->m_bZoomAll){
		for(int i=0; i<pApp->m_nTotalTracks; i++)
			for(int j=0;j<pApp->m_pTracks[i].m_nTotalDocs;j++)
				pApp->m_pTracks[i].m_pDoc[j]->m_ImageViewInfo.pImageView->ZoomOriginal();
	}
	else{
		CAppDoc* pDoc = GetDocument();
		pDoc->m_ImageViewInfo.pImageView->ZoomOriginal();
	}
}

void CAppView::OnUpdateViewZoom100(CCmdUI *pCmdUI)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	pCmdUI->Enable(!pApp->m_bOnline && pApp->m_UserAccInfo.m_nCurrentAccessLevel >= pApp->m_AdvSecurityDlg.m_data.m_nZoom100);
}


void CAppView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CEvImageView::OnMouseMove(nFlags, point);
	CApplication* pApp = (CApplication*) AfxGetApp();

	CPoint ptTemp,ptDraw;
	CSize size;
	LOGPEN BlueInfo;
	BlueInfo.lopnColor = RGB(0,255,0);
	BlueInfo.lopnStyle = PS_SOLID;
	BlueInfo.lopnWidth.x = 6;
	if(pApp->m_bPixelRuler && m_bPixelRuler) {

		CAppDoc* pDoc = GetDocument();

		LOGPEN GreenPen;
		LOGPEN BluePen;
		CPoint ptDraw = m_ptPixelRulerStart;
		GreenPen.lopnColor = RGB(0,255,0);
		GreenPen.lopnStyle = PS_SOLID;
		GreenPen.lopnWidth.x = 1;
		BluePen.lopnColor = RGB(0,0,255);
		BluePen.lopnStyle = PS_SOLID;
		BluePen.lopnWidth.x = 1;


		DrwDel(&pDoc->m_ImageViewInfo);
		
		DrwLine(&pDoc->m_ImageViewInfo,&m_ptPixelRulerStart,&m_ptPixelRulerEnd,&GreenPen);
		OutputDebugLogTo(5,TRUE,"x1 = %d, y1 = %d", m_ptPixelRulerStart.x, m_ptPixelRulerStart.y);
		OutputDebugLogTo(5,TRUE,"x = %d, y = %d",m_ptPixelRulerEnd.x,m_ptPixelRulerEnd.y);
		DrwUpdate(&pDoc->m_ImageViewInfo);

		int nTrack = pDoc->m_pTrack->m_nTrack;
		double dResolution = 1;
		double dResMean = (pApp->m_pTracks[nTrack].m_dResolutionX +
			pApp->m_pTracks[nTrack].m_dResolutionY) / 2.0;

		//double dResMean = (pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[0].m_Inspection.m_CalibrationData.dResolutionAlongXInMicronPerPixel +
		//					pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[0].m_Inspection.m_CalibrationData.dResolutionAlongYInMicronPerPixel) / 2.0;
		dResolution = dResMean;
		pApp->m_pPixelRulerDlg->UpdateDatas(m_ptPixelRulerStart, m_ptPixelRulerEnd, dResolution,
			pApp->m_pTracks[nTrack].m_dResolutionX, pApp->m_pTracks[nTrack].m_dResolutionY);
	}
	if(m_bPanning) {
		
		CEvImageView* pView;
		CAppDoc* pDoc = GetDocument();

		pView = pDoc->m_ImageViewInfo.pImageView;
		if(pView != this)
			pView->ScrollToPosition(GetScrollPosition());
			
	}
}

BOOL CAppView::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	CEvImageView::OnMouseWheel(nFlags, zDelta, point);
	CApplication* pApp = (CApplication*) AfxGetApp();
	CEvImageView* pView;

	if(pApp->m_bZoomAll) {
		for(int i=0; i<pApp->m_nTotalTracks; i++){
			for(int j=0;j<pApp->m_pTracks[i].m_nTotalDocs;j++) {
				pView = pApp->m_pTracks[i].m_pDoc[j]->m_ImageViewInfo.pImageView;
				if(pView != this)
					pView->SetZoomLevel(m_fZoomScale, &GetLogicalCenterPoint());
			}
		}
	}
	return TRUE;
}

void CAppView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CApplication* pApp = (CApplication*)AfxGetApp();

	CEvImageView::OnLButtonDblClk(nFlags, point);
	//OnViewZoomFit();
	BOOL bChanged = FALSE;
	int nPrevHeight = 0;
	int nPrevWidth = 0;
	CRect rect1;

	for(int i=0; i</*pApp->m_nTotalTracks*/pApp->m_nTotalViews; i++)
	{
		pApp->m_pFrames[i]->GetWindowRect(&rect1);
		if(i>0)
		{
			if(rect1.Height() != nPrevHeight)
				bChanged = TRUE;
			if (rect1.Width() != nPrevWidth)
				bChanged = TRUE;
		}
		nPrevHeight = rect1.Height();
		nPrevWidth = rect1.Width();
	}
	//pApp->m_pImageSelectionDlg->ShowWindow(!bChanged);
//	OnViewZoomFit();

	/*if (pApp->m_pInfoMsgDlg) {
		pApp->m_pInfoMsgDlg->ShowWindow(pApp->m_bTeaching);
	}*/
}

void CAppView::OnLButtonDown(UINT nFlags, CPoint point)
{
	
	
	CApplication* pApp = (CApplication*) AfxGetApp();
	CAppDoc* pDoc = GetDocument();
	if(pApp->m_bPixelRuler){
		for (int nTrack = 0; nTrack < pApp->m_nTotalTracks; nTrack++) {
			for (int nDoc = 0; nDoc < pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
				DrwDel(&pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
				DrwUpdate(&pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
			}
		}
		m_bPixelRuler = TRUE;
	}


	pApp->m_nNavigateStaticImgTrack = pDoc->m_pTrack->m_nTrack;
	CEvImageView::OnLButtonDown(nFlags, point);
}


void CAppView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CEvImageView::OnLButtonUp(nFlags, point);

	if(m_bPixelRuler) {
		 m_bPixelRuler = FALSE;
	}
	
}


void CAppView::OnUpdateShowMaginifier(CCmdUI *pCmdUI)
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	BOOL bTrackLiveImg, bDocInsp;
	for (int nTrack = 0; nTrack<pApp->m_nTotalTracks; nTrack++) {
		bTrackLiveImg &= pApp->m_pTracks[nTrack].m_bLiveImage;
		bDocInsp &= pApp->m_pTracks[nTrack].m_pDoc[0]->m_bInspecting;


	}
	BOOL bStepInspectionEnabled = FALSE;
	BOOL bStoredInspectionEnabled = FALSE;
	for (int nTrack = 0; nTrack < pApp->m_nTotalTracks; nTrack++) {
		for (int nDoc = 0; nDoc < pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
			bStepInspectionEnabled |= pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_bInspSavedImage;
			bStoredInspectionEnabled |= pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_bStoredImages;
		}
	}

	pCmdUI->Enable(!pApp->m_bOnline &&
		!bStepInspectionEnabled &&
		!pApp->m_bTeaching &&
		!pApp->m_bTrackInspSeq &&
		!pApp->m_bInspecting &&
		!bTrackLiveImg &&
		!bDocInsp &&
		!bStoredInspectionEnabled &&
		(pApp->m_UserAccInfo.m_nCurrentAccessLevel >= pApp->m_AdvSecurityDlg.m_data.m_nShowMaginifier)); //&& !pDoc->m_bInspecting

	pCmdUI->SetCheck(m_bMagnifierView);
}

void CAppView::OnShowMaginifier()
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	if (!pApp->m_bTeaching) {
		ToggleMagnifier();
		CAppDoc* pDoc = GetDocument();
		DrwDel(&pDoc->m_ImageViewInfo);
		DrwUpdate(&pDoc->m_ImageViewInfo);
	}
}

void CAppView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	if (!pApp->m_bTeaching) {
		CAppDoc* pDoc = GetDocument();
		ToggleMagnifier();
		DrwDel(&pDoc->m_ImageViewInfo);
		DrwUpdate(&pDoc->m_ImageViewInfo);
		CEvImageZoomView::OnRButtonDblClk(nFlags, point);
	}
}
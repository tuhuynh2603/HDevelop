/// AppTabView.cpp : implementation of the CAppTabView class
/// Udupa; April'2012

#include "stdafx.h"
#include "Application.h"
#include "AppDoc.h"
#include "AppView.h"
#include "EvFrameWnd.h"
#include "AppTabView.h"
#include "resource.h"
#include "EvChildFrame.h"
#include "TrackHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAppTabView

IMPLEMENT_DYNCREATE(CAppTabView, CEvTabView)

BEGIN_MESSAGE_MAP(CAppTabView, CEvTabView)
	//{{AFX_MSG_MAP(CAppTabView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAppTabView message handlers

void CAppTabView::OnInitialUpdate() 
{
	CEvTabView::OnInitialUpdate();

	for(int i=0; i<m_nTotalDocs; i++) {
		m_SdiChildWnd[i].Create(GetPage(i), IDC_CHILD_CONTROL,
			IDR_MAINFRAME,
			RUNTIME_CLASS(CAppDoc),
			RUNTIME_CLASS(CEvFrameWnd),       // main SDI frame window
			RUNTIME_CLASS(CAppView),
			WS_CHILD | WS_VISIBLE, 0L);

		POSITION pos = m_SdiChildWnd[i].m_pDocTemplate->GetFirstDocPosition();
		m_pDocs[i] = (CAppDoc*) m_SdiChildWnd[i].m_pDocTemplate->GetNextDoc(pos);
	}
}

void CAppTabView::SetTitle()
{
	CTrackHandler* pTrack = ((CAppDoc*) m_pDocs[0])->m_pTrack;

	CAppDoc* pDoc = (CAppDoc*) m_pDocs[0];
	CString str, strCam;
	CApplication* pApp = pTrack->m_pApp;
	int nCamNo = pApp->m_nFirstTrackNo + pDoc->m_nDocIdx;
	if(pTrack->m_nTrack==0){
		pApp->nTop = pApp->nTop+1;
		strCam.Format("%s %s", pApp->m_bIsMasterApp ? "" : "Centralized", pApp->m_strTopCameraName.GetAt(nCamNo));
	}
	else if(pTrack->m_nTrack==1){
		pApp->nBottom = pApp->nBottom+1;
	   strCam.Format("%s %s", pApp->m_bIsMasterApp ? "" : "Centralized",pApp->m_strBottomCameraName.GetAt(nCamNo));
	}
	else{
		pApp->nSide = pApp->nSide+1;
	   strCam.Format("%s %s", pApp->m_bIsMasterApp ? "" : "Centralized",pApp->m_strSideCameraName.GetAt(nCamNo));
	}
//	m_staticTrackId.SetWindowTextA(strCam);
	GetDocument()->SetNewTitle(strCam);

	//if(pTrack->m_bCamAvail) 
	//	str.Format("Cam: %s", pTrack->m_strCameraFile);
	//else{
		if(pTrack->m_nTrack==0){
			str = "Top Camera";
		}
		else if(pTrack->m_nTrack==1){
			str = "Bottom Camera";
		}
		else{
			str = "Side camera";
		}
	//}
	SetCameraId(&str);
}

void CAppTabView::SetDocStatusText()
{
	CAppDoc* pDoc = (CAppDoc*) m_pDocs[0];

	CString str;
	if(!pDoc->m_strLoadImage.IsEmpty()) {
		if(!m_strDocStatus.IsEmpty())
			str.Format("%s  [%s]", pDoc->m_strLoadImage, m_strDocStatus);
		else
			str = pDoc->m_strLoadImage;
	}
	else {
		if(!m_strDocStatus.IsEmpty())
			str = m_strDocStatus;
		else {
			SetCameraId(&m_strDocStatus);
			return;
		}
	}

	m_staticTrackStatus.SetWindowTextA(str);
}

int CAppTabView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_nTotalDocs = 1;//((CApplication*)AfxGetApp())->m_pTracks[0].m_nTotalDocs;
	for(int i=0; i<m_nTotalDocs; i++) {
		m_arrImage.Add(AfxGetApp()->LoadIcon(IDI_DOC));
		m_imageList.Add(m_arrImage[i]);
	}

	if(CEvTabView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CAppTabView::OnSize(UINT nType, int cx, int cy) 
{
	CCtrlView::OnSize(nType, cx, cy);

	ResizeDialog(GetTabCtrl().GetCurSel(), cx, cy);
	
	if(m_pDocs != NULL) {
		for(int i=0; i<m_nTotalDocs; i++)
			if(m_pDocs[i] != NULL)
				((CAppDoc*) m_pDocs[i])->ZoomView(ZOOMFIT);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Overridables

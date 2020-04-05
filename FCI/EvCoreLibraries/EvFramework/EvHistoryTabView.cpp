/// EvHistoryTabView.cpp : implementation of the CEvHistoryTabView class
/// Udupa; April'2012

#include "stdafx.h"
#include "EvImageDoc.h"
#include "EvImageView.h"
#include "EvFrameWnd.h"
#include "EvHistoryTabView.h"
#include "resource.h"
#include "EvChildFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEvHistoryTabView

IMPLEMENT_DYNCREATE(CEvHistoryTabView, CEvTabView)

BEGIN_MESSAGE_MAP(CEvHistoryTabView, CEvTabView)
	//{{AFX_MSG_MAP(CEvHistoryTabView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEvHistoryTabView message handlers

void CEvHistoryTabView::OnInitialUpdate() 
{
	CEvTabView::OnInitialUpdate();

	for(int i=0; i<m_nTotalDocs; i++) {
		m_SdiChildWnd[i].Create(GetPage(i), IDC_CHILD_CONTROL,
			128,
			RUNTIME_CLASS(CEvImageDoc),
			RUNTIME_CLASS(CEvFrameWnd),       // main SDI frame window
			RUNTIME_CLASS(CEvImageView),
			WS_CHILD | WS_VISIBLE, 0L);

		POSITION pos = m_SdiChildWnd[i].m_pDocTemplate->GetFirstDocPosition();
		m_pDocs[i] = (CEvImageDoc*) m_SdiChildWnd[i].m_pDocTemplate->GetNextDoc(pos);
	}
}

void CEvHistoryTabView::SetTitle(int nIndex, int nTrack)
{
	CString str;
	if(nIndex>=0) {
		CString str1;
		str1.Format("#%d", nIndex);
		str.Format("Cam%d %s", nTrack + 1, 
			nIndex==1 ? "(Latest)" : str1);
	}
	else
		str.Format("Camera %d (Image# %d)", nTrack + 1, nIndex * -1);
	m_staticTrackId.SetWindowTextA(str.GetBuffer(0));
	str.ReleaseBuffer();
	///GetDocument()->SetNewTitle(str);
}

int CEvHistoryTabView::UpdateImage(CImageBuffer* pBuffer)
{
	m_pBuffer = pBuffer;
	if(m_pDocs[0])
		m_pDocs[0]->InitViewBuffer(pBuffer);

	return 0;
}

int CEvHistoryTabView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_nTotalDocs = 1;

	for(int i=0; i<m_nTotalDocs; i++) {
		m_arrImage.Add(AfxGetApp()->LoadIcon(IDI_DOC));
		m_imageList.Add(m_arrImage[i]);
	}

	if(CEvTabView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CEvHistoryTabView::OnSize(UINT nType, int cx, int cy) 
{
	CCtrlView::OnSize(nType, cx, cy);

	ResizeDialog(GetTabCtrl().GetCurSel(), cx, cy);
	
	if(m_pDocs != NULL) {
		for(int i=0; i<m_nTotalDocs; i++)
			if(m_pDocs[i] != NULL)
				m_pDocs[i]->ZoomFit();
	}
}

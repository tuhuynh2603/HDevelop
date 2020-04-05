/// EvHistoryImagePane.cpp : implementation of the CEvHistoryImagePane class
/// Udupa; April'2012

#include "stdafx.h"

#include "HistoryImagePane.h"
#include "Resource.h"
#include "EvFrameWnd.h"
#include "HistoryTabView.h"
#include "EvImageDoc.h"
#include "ExternalFunctions.h"
#include "MsgCode.h"
#include "EvChildFrame.h"
#include "EvMainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEvHistoryImagePane

CHistoryImagePane::CHistoryImagePane()
{
	m_nSourceImage = 0;
	m_nDepth = 15;

	m_nLastIndex = 14;
}

CHistoryImagePane::~CHistoryImagePane()
{
}

void CHistoryImagePane::OnClose() 
{
	CloseChildren();

	CDockablePane::OnClose();
}

void CEvHistoryImagePane::CloseChildren()
{
	int nCount = (int) m_arrImageWnds.GetCount();
	for(int i=nCount-1; i>=0; i--)
		Clean(i);
}

void CHistoryImagePane::Clean(int nIndex)
{
	CEvSdiChildWnd* pWnd = m_arrImageWnds[nIndex];
	if(IsWindow(pWnd->m_hWnd)) {
		POSITION pos = pWnd->m_pDocTemplate->GetFirstDocPosition();
		CEvTabDoc* pDoc = (CEvTabDoc*) pWnd->m_pDocTemplate->GetNextDoc(pos);
		pDoc->OnCloseDocument();
	}

	if(IsWindow(m_arrImageDialogs[nIndex]->m_hWnd))
		m_arrImageDialogs[nIndex]->DestroyWindow();
	delete m_arrImageDialogs[nIndex];
	m_arrImageDialogs.RemoveAt(nIndex);

	delete pWnd;
	m_arrImageWnds.RemoveAt(nIndex);

	if(nIndex < m_arrImageDocs.GetCount())
		m_arrImageDocs.RemoveAt(nIndex);
}

BEGIN_MESSAGE_MAP(CHistoryImagePane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

int CHistoryImagePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	//CRect rectDummy;
	//rectDummy.SetRectEmpty();

	///for(int i=0; i<m_nDepth; i++)
	///	AddImageView();

	//const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	//if (!m_wndOutputBuild.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
	//	!m_wndOutputDebug.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
	//	!m_wndOutputFind.Create(dwStyle, rectDummy, &m_wndTabs, 4))
	//{
	//	TRACE0("Failed to create output windows\n");
	//	return -1;      // fail to create
	//}

	UpdateFonts();

	SetHistoryDepth(m_nDepth);

	//// Attach list windows to tab:
	//m_wndTabs.AddTab(&m_wndOutputBuild, "Build", (UINT)0);
	//m_wndTabs.AddTab(&m_wndOutputDebug, "Debug", (UINT)1);
	//m_wndTabs.AddTab(&m_wndOutputFind, "Find", (UINT)2);

	return 0;
}

void CHistoryImagePane::AddImageView()
{
	CEvTabPage* pPage = new CEvTabPage();
	pPage->Create(MAKEINTRESOURCE(IDD_CHILD_DIALOG), this);
	pPage->ShowWindow(SW_SHOW);

	CEvSdiChildWnd* pWnd = new CEvSdiChildWnd();
	pWnd->Create(pPage, IDC_CHILD_CONTROL,
		128,//IDR_HISTORYTAB,
		RUNTIME_CLASS(CEvTabDoc),
		RUNTIME_CLASS(CEvFrameWnd),       // main SDI frame window
		RUNTIME_CLASS(CEvHistoryTabView),
		WS_CHILD | WS_BORDER | WS_VISIBLE, 0L);

	m_arrImageDialogs.Add(pPage);
	m_arrImageWnds.Add(pWnd);
	if(IsWindow(pWnd->m_hWnd)) {
		POSITION pos = pWnd->m_pDocTemplate->GetFirstDocPosition();
		CDocument* pDoc = pWnd->m_pDocTemplate->GetNextDoc(pos);
		pos = pDoc->GetFirstViewPosition();
		CEvTabView* pView = (CEvTabView*) pDoc->GetNextView(pos);
		if(pView) {
			CEvSdiTemplate* pDocTemplate = pView->m_SdiChildWnd[0].m_pDocTemplate;

			if(pDocTemplate) {
				pos = pDocTemplate->GetFirstDocPosition();
				m_arrImageDocs.Add((CEvImageDoc*) pDocTemplate->GetNextDoc(pos));
			}
		}
	}
}

void CHistoryImagePane::SetHistoryDepth(int nDepth)
{
	//int nCount = (int) m_arrImageWnds.GetCount();
	//for(int i=nCount-1; i>=nDepth; i--)
		//Clean(i);

	//nCount = (int) m_arrImageWnds.GetCount();
	for(int i=0; i<nDepth; i++){
		Clean(i);
		AddImageView();
	}

	m_nDepth = nDepth;
	AdjustLayout();
	
	SetTitles();
}

void CHistoryImagePane::SetTitles()
{
	CEvSdiChildWnd* pWnd;
	for(int i=0; i<m_nDepth; i++) {
		pWnd = m_arrImageWnds[i];
		if(IsWindow(pWnd->m_hWnd)) {
			POSITION pos = pWnd->m_pDocTemplate->GetFirstDocPosition();
			CDocument* pDoc = pWnd->m_pDocTemplate->GetNextDoc(pos);
			pos = pDoc->GetFirstViewPosition();
			CEvHistoryTabView* pView = (CEvHistoryTabView*) pDoc->GetNextView(pos);
			if(pView)
				pView->SetTitle(i+1, m_nSourceImage);
		}
	}
}

void CHistoryImagePane::PushImage(CImageBuffer* pBuffer)
{
	m_nLastIndex++;
	if(m_nLastIndex >= m_nDepth)
		m_nLastIndex = 0;
	CImageBuffer* pDestBuffer = &m_Buffers[m_nLastIndex];
	if(!pDestBuffer->IsAllocated() || (pDestBuffer->GetImgSize() != pBuffer->GetImgSize()))
		pDestBuffer->Allocate(pBuffer->GetImgSize(), true);
	CRect rect = CRect(CPoint(0,0), pBuffer->GetImgSize());
	ImgCopy(pBuffer, &rect, pDestBuffer, &rect);

	int nIndex = (m_nLastIndex + 1) % m_nDepth;
	BOOL bNewSize;
	for(int i=m_nDepth-1; i>=0; i--, nIndex++) {
		nIndex %= m_nDepth;
		if(m_Buffers[nIndex].IsAllocated() && (i < m_arrImageDocs.GetCount())) {
			bNewSize = (m_arrImageDocs[i]->m_pbufImage==NULL);
			m_arrImageDocs[i]->InitViewBuffer(&m_Buffers[nIndex]);
			if(bNewSize)
				m_arrImageDocs[i]->SetDocSize(rect.Size());
			m_arrImageDocs[i]->ZoomFit();
		}
	}
}

void CHistoryImagePane::AdjustLayout()
{
	CRect rect;
	GetWindowRect(&rect);

	int cx = rect.Width();
	int cy = rect.Height();
	int arrcx[15], arrcy[15];
	if(m_nDepth==10) {
		cx /= 5;
		cy /= 2;
		int arrcx1[10] = { 0, cx,  cx<<1, cx*3, cx*4,  0, cx,  cx<<1, cx*3, cx*4 };
		int arrcy1[10] = { 0,  0,      0,    0,    0, cy, cy,     cy,   cy,   cy };
		memcpy(arrcx, arrcx1, sizeof(int)*m_nDepth);
		memcpy(arrcy, arrcy1, sizeof(int)*m_nDepth);
	}
	else if(m_nDepth==9) {
		cx /= 5;
		cy /= 2;
		int arrcx1[9] = { 0, cx,  cx<<1, cx*3, cx*4,  0, cx,  cx<<1, cx*3 };
		int arrcy1[9] = { 0,  0,      0,    0,    0, cy, cy,     cy,   cy };
		memcpy(arrcx, arrcx1, sizeof(int)*m_nDepth);
		memcpy(arrcy, arrcy1, sizeof(int)*m_nDepth);
	}
	else if(m_nDepth==8) {
		cx /= 4;
		cy /= 2;
		int arrcx1[8] = { 0, cx,  cx<<1, cx*3,  0, cx, cx<<1,  cx*3 };
		int arrcy1[8] = { 0,  0,      0,    0, cy, cy,    cy,    cy };
		memcpy(arrcx, arrcx1, sizeof(int)*m_nDepth);
		memcpy(arrcy, arrcy1, sizeof(int)*m_nDepth);
	}
	else if(m_nDepth==7) {
		cx /= 4;
		cy /= 2;
		int arrcx1[7] = { 0, cx,  cx<<1, cx*3,  0, cx, cx<<1 };
		int arrcy1[7] = { 0,  0,      0,    0, cy, cy,    cy };
		memcpy(arrcx, arrcx1, sizeof(int)*m_nDepth);
		memcpy(arrcy, arrcy1, sizeof(int)*m_nDepth);
	}
	else if(m_nDepth==6) {
		cx /= 3;
		cy /= 2;
		int arrcx1[6] = { 0, cx,  cx<<1,  0, cx, cx<<1 };
		int arrcy1[6] = { 0,  0,      0, cy, cy,    cy };
		memcpy(arrcx, arrcx1, sizeof(int)*m_nDepth);
		memcpy(arrcy, arrcy1, sizeof(int)*m_nDepth);
	}
	else if(m_nDepth==5) {
		cx /= 3;
		cy /= 2;
		int arrcx1[5] = { 0, cx,  cx<<1,  0, cx };
		int arrcy1[5] = { 0,  0,      0, cy, cy };
		memcpy(arrcx, arrcx1, sizeof(int)*m_nDepth);
		memcpy(arrcy, arrcy1, sizeof(int)*m_nDepth);
	}
	else if(m_nDepth==4) {
		cx /= 2;
		cy /= 2;
		int arrcx1[4] = { 0, cx,  0, cx };
		int arrcy1[4] = { 0,  0, cy, cy };
		memcpy(arrcx, arrcx1, sizeof(int)*m_nDepth);
		memcpy(arrcy, arrcy1, sizeof(int)*m_nDepth);
	}
	else if(m_nDepth==3) {
		cx /= 3;
		int arrcx1[3] = { 0, cx,  cx<<1 };
		int arrcy1[3] = { 0,  0,      0 };
		memcpy(arrcx, arrcx1, sizeof(int)*m_nDepth);
		memcpy(arrcy, arrcy1, sizeof(int)*m_nDepth);
	}
	else if(m_nDepth==2) {
		cx /= 2;
		int arrcx1[2] = { 0, cx };
		int arrcy1[2] = { 0,  0 };
		memcpy(arrcx, arrcx1, sizeof(int)*m_nDepth);
		memcpy(arrcy, arrcy1, sizeof(int)*m_nDepth);
	}
	else if(m_nDepth==1) {
		int arrcx1[1] = { 0 };
		int arrcy1[1] = { 0 };
		memcpy(arrcx, arrcx1, sizeof(int)*m_nDepth);
		memcpy(arrcy, arrcy1, sizeof(int)*m_nDepth);
	}
	CEvSdiChildWnd* pWnd;
	for(int i=0; i<m_nDepth; i++) {
		pWnd = m_arrImageWnds[i];
		if((pWnd != NULL) && ::IsWindow(pWnd->m_hWnd)) {
			int nWidth;
			if((m_nDepth > 3) && (i > m_nDepth/2)) {
				nWidth = rect.Width() / (m_nDepth/2);
				if(i > (m_nDepth/2+1))
					arrcx[i] = arrcx[i-1] + nWidth;
			}
			else
				nWidth = cx;
			m_arrImageDialogs[i]->SetWindowPos(NULL, arrcx[i], arrcy[i], nWidth, cy, SWP_NOZORDER);
			m_arrImageDialogs[i]->GetDlgItem(IDC_CHILD_CONTROL)->SetWindowPos(NULL, arrcx[i], arrcy[i], nWidth, cy, SWP_NOMOVE | SWP_NOZORDER);
			pWnd->SetWindowPos(NULL, arrcx[i], arrcy[i], nWidth, cy, SWP_NOMOVE | SWP_NOZORDER);
			pWnd->m_pDocTemplate->m_pFrame->SetWindowPos(NULL, arrcx[i], arrcy[i], nWidth, cy, SWP_NOMOVE | SWP_NOZORDER);
		}
	}
	//m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void CHistoryImagePane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	AdjustLayout();
}

void CHistoryImagePane::AdjustHorzScroll(CListBox& wndListBox)
{
	//CClientDC dc(this);
	//CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	//int cxExtentMax = 0;

	//for (int i = 0; i < wndListBox.GetCount(); i ++)
	//{
	//	CString strItem;
	//	wndListBox.GetText(i, strItem);

	//	cxExtentMax = max(cxExtentMax, dc.GetTextExtent(strItem).cx);
	//}

	//wndListBox.SetHorizontalExtent(cxExtentMax);
	//dc.SelectObject(pOldFont);
}

void CHistoryImagePane::UpdateFonts()
{
	//m_wndOutputBuild.SetFont(&afxGlobalData.fontRegular);
	//m_wndOutputDebug.SetFont(&afxGlobalData.fontRegular);
	//m_wndOutputFind.SetFont(&afxGlobalData.fontRegular);
}

/////////////////////////////////////////////////////////////////////////////
// CEvHistoryImageView

CHistoryImageView::CHistoryImageView()
{
}

CHistoryImageView::~CHistoryImageView()
{
}

BEGIN_MESSAGE_MAP(CHistoryImageView, CEvTabView)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CEvHistoryImageView message handlers

void CHistoryImageView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void CHistoryImageView::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != NULL && pParentBar != NULL) {
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();
	}
}

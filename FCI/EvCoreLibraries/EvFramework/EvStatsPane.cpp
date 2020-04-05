/// EVStatsPane.cpp : implementation of the CEVStatsPane class
/// Udupa; November'2012

#include "StdAfx.h"
#include "EvStatsTabPage.h"
#include "EvImageDoc.h"
#include "EvChildFrame.h"
#include "EvMainFrame.h"
#include "EvFrameWnd.h"
#include "EvHistoryTabView.h"
#include "EVStatsPane.h"
#include "Resource.h"
#include "ExternalFunctions.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEVStatsPane

CEVStatsPane::CEVStatsPane(CEvMainFrame* pMainFrame)
{
	m_pImageWnd = NULL;
	m_pImageDialog = NULL;
	m_pImageDoc = NULL;

	m_pMainFrame = pMainFrame;
}


CEVStatsPane::~CEVStatsPane()
{
}

void CEVStatsPane::OnClose() 
{
	CloseChildren();

	CDockablePane::OnClose();
}

void CEVStatsPane::CloseChildren()
{
	if(m_pImageWnd && IsWindow(m_pImageWnd->m_hWnd)) {
		POSITION pos = m_pImageWnd->m_pDocTemplate->GetFirstDocPosition();
		CEvTabDoc* pDoc = (CEvTabDoc*) m_pImageWnd->m_pDocTemplate->GetNextDoc(pos);
		pDoc->OnCloseDocument();
	}

	if(m_pImageDialog) {
		if(IsWindow(m_pImageDialog->m_hWnd))
			m_pImageDialog->DestroyWindow();
		delete m_pImageDialog;
		m_pImageDialog = NULL;
	
		delete m_pImageWnd;
		m_pImageWnd = NULL;
	}
}

void CEVStatsPane::DoDataExchange(CDataExchange* pDX)
{
	CDockablePane::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEvStatsTabPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEVStatsPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_COMMAND_RANGE(IDC_STATS_IMAGES_RADIO, IDC_STATS_TABLE_RADIO, OnRadioStatsType)
	ON_COMMAND_RANGE(IDC_STATSIMAGE_CAM1_RADIO, IDC_STATSIMAGE_CAM5_RADIO, OnRadioStatsCam)
	ON_BN_CLICKED(IDC_STATS_IMAGEFIRST_BUTTON, OnBnClickedStatsImageFirstButton)
	ON_BN_CLICKED(IDC_STATS_IMAGEPREV_BUTTON, OnBnClickedStatsImagePrevButton)
	ON_BN_CLICKED(IDC_STATS_IMAGENEXT_BUTTON, OnBnClickedStatsImageNextButton)
	ON_BN_CLICKED(IDC_STATS_IMAGELAST_BUTTON, OnBnClickedStatsImageLastButton)
	ON_BN_CLICKED(IDC_STATS_DATAREFRESH_BUTTON, &CEVStatsPane::OnBnClickedStatsDatarefreshButton)
	ON_LBN_SELCHANGE(IDC_STATS_CHARTTYPE_LIST, &CEVStatsPane::OnLbnSelchangeStatsCharttypeList)
END_MESSAGE_MAP()

int CEVStatsPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;


	EnableGripper(FALSE);

	AddImageView();
	OnRadioStatsType(IDC_STATS_IMAGES_RADIO);
	((CButton*) m_pImageDialog->GetDlgItem(IDC_STATS_IMAGES_RADIO))->SetCheck(1);
	//if(m_pImageDoc == NULL)
	//	return -1;

	return 0;
}


void CEVStatsPane::AdjustLayout()
{
	if((m_pImageWnd != NULL) && ::IsWindow(m_pImageWnd->m_hWnd)) {
		CRect rect;
		GetWindowRect(&rect);
		int cx = rect.Width();
		int cy = rect.Height();

		m_pImageDialog->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);

		int nOffset = 230;
		int nWidth = cx-nOffset;
		m_pImageDialog->GetDlgItem(IDC_STATSCHILD_CONTROL)->SetWindowPos(NULL, nOffset, 0, nWidth, cy, SWP_NOZORDER);
		//m_pImageWnd->SetWindowPos(NULL, cx, cy, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
		m_pImageWnd->m_pDocTemplate->m_pFrame->SetWindowPos(NULL, nOffset, 0, nWidth, cy, SWP_NOZORDER);

		m_pImageDialog->GetDlgItem(IDC_STATS_BARCHART_STATIC)->SetWindowPos(NULL, nOffset, 0, nWidth, cy, SWP_NOREDRAW);

		m_pImageDialog->GetDlgItem(IDC_STATS_DATA_EDIT)->SetWindowPos(NULL, nOffset, 0, nWidth, cy, SWP_NOZORDER);

		m_pImageDialog->AdjustLayout();
	}
}

void CEVStatsPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	AdjustLayout();
}

void CEVStatsPane::UpdateFonts()
{
}

void CEVStatsPane::AddImageView()
{
	CEvStatsTabPage* pPage = new CEvStatsTabPage();
	pPage->Create(MAKEINTRESOURCE(IDD_STATSCHILD_DIALOG), this);
	pPage->ShowWindow(SW_SHOW);

	CEvSdiChildWnd* pWnd = new CEvSdiChildWnd();
	pWnd->Create(pPage, IDC_STATSCHILD_CONTROL,
		128,//IDR_HISTORYTAB,
		RUNTIME_CLASS(CEvTabDoc),
		RUNTIME_CLASS(CEvFrameWnd),
		RUNTIME_CLASS(CEvHistoryTabView),
		WS_CHILD | WS_BORDER | WS_VISIBLE, 0L);

	pPage->m_pstrLotFolder = &m_pMainFrame->m_strLotFolder;
	m_pImageDialog = pPage;
	m_pImageWnd = pWnd;
	if(IsWindow(pWnd->m_hWnd)) {
		POSITION pos = pWnd->m_pDocTemplate->GetFirstDocPosition();
		CDocument* pDoc = pWnd->m_pDocTemplate->GetNextDoc(pos);
		pos = pDoc->GetFirstViewPosition();
		CEvTabView* pView = (CEvTabView*) pDoc->GetNextView(pos);
		if(pView) {
			CEvSdiTemplate* pDocTemplate = pView->m_SdiChildWnd[0].m_pDocTemplate;

			if(pDocTemplate) {
				pos = pDocTemplate->GetFirstDocPosition();
				m_pImageDoc = (CEvImageDoc*) pDocTemplate->GetNextDoc(pos);
///				m_arrImageDocs.Add((CEvImageDoc*) pDocTemplate->GetNextDoc(pos));
			}
		}
		m_pImageDialog->PopulateDataTree();
		m_pImageDialog->Plot();
	}
}

void CEVStatsPane::OnRadioStatsType(UINT id)
{
	int nType = id - IDC_STATS_IMAGES_RADIO;
	m_pImageDialog->SetStatsType(nType);
	m_pImageWnd->m_pDocTemplate->m_pFrame->ShowWindow(nType==0 ? SW_SHOW : SW_HIDE);
}

int CEVStatsPane::OnSelection(int nTrack, int nIndex, CFailImageStack* pFailure)
{
	if(!pFailure->bufFailImage.IsAllocated())
		return -1;

	if(!m_Buffer.IsAllocated() || (m_Buffer.GetImgSize() != pFailure->bufFailImage.GetImgSize()))
		m_Buffer.Allocate(pFailure->bufFailImage.GetImgSize(), true);
	CRect rect = CRect(CPoint(0,0), pFailure->bufFailImage.GetImgSize());
	ImgCopy(&pFailure->bufFailImage, &rect, &m_Buffer, &rect);

	BOOL bNewSize = (m_pImageDoc->m_pbufImage==NULL);
	m_pImageDoc->InitViewBuffer(&m_Buffer);
	if(bNewSize)
		m_pImageDoc->SetDocSize(rect.Size());

	CString strMessage1, strMessage2;
	if(!pFailure->strFailImage.IsEmpty()) {
		strMessage1.Format("%s  @%s", pFailure->strFailImage, pFailure->strFailTime);
		strMessage2.Format("[Tool: %d  Mold: %d  Gripper: %d]", pFailure->nToolNo, pFailure->nMoldNo, pFailure->nGripperNo);
	}
	else {
		strMessage1 = "";
		strMessage2 = "";
	}

	LOGPEN pen;
	pen.lopnColor = RGB(200,50,0);
	pen.lopnStyle = PS_SOLID;
	pen.lopnWidth.x = 1;
	DrwDel(&m_pImageDoc->m_ImageViewInfo);
	DrwStr(&m_pImageDoc->m_ImageViewInfo, &strMessage1, &CPoint(50,50), &pen, 80);
	pen.lopnColor = RGB(200,100,0);
	DrwStr(&m_pImageDoc->m_ImageViewInfo, &strMessage2, &CPoint(50,160), &pen, 60);


	if(IsWindow(m_pImageWnd->m_hWnd)) {
		POSITION pos = m_pImageWnd->m_pDocTemplate->GetFirstDocPosition();
		CDocument* pDoc = m_pImageWnd->m_pDocTemplate->GetNextDoc(pos);
		pos = pDoc->GetFirstViewPosition();
		CEvHistoryTabView* pView = (CEvHistoryTabView*) pDoc->GetNextView(pos);
		if(pView)
			pView->SetTitle(-nIndex, nTrack);
	}
	m_pImageDoc->ZoomFit();

	return 0;
}

void CEVStatsPane::OnRadioStatsCam(UINT id)
{
	m_pImageDialog->UpdateData(TRUE);
	m_pMainFrame->ShowFailedImage(m_pImageDialog->m_nCamera, m_pImageDialog->m_nImageIndex-1);
}

void CEVStatsPane::OnBnClickedStatsImageFirstButton()
{
	int nIndex = 0;
	if(!m_pMainFrame->ShowFailedImage(m_pImageDialog->m_nCamera, nIndex)) {
		m_pImageDialog->m_nImageIndex = nIndex + 1;
		m_pImageDialog->UpdateData(FALSE);
	}
}

void CEVStatsPane::OnBnClickedStatsImagePrevButton()
{
	m_pImageDialog->UpdateData(TRUE);
	int nIndex = m_pImageDialog->m_nImageIndex-2;
	if(nIndex < 0)
		nIndex = 24;
	if(!m_pMainFrame->ShowFailedImage(m_pImageDialog->m_nCamera, nIndex)) {
		m_pImageDialog->m_nImageIndex = nIndex + 1;
		m_pImageDialog->UpdateData(FALSE);
	}
}

void CEVStatsPane::OnBnClickedStatsImageNextButton()
{
	m_pImageDialog->UpdateData(TRUE);
	int nIndex = m_pImageDialog->m_nImageIndex;
	if(nIndex > 24)
		nIndex = 0;
	if(!m_pMainFrame->ShowFailedImage(m_pImageDialog->m_nCamera, nIndex)) {
		m_pImageDialog->m_nImageIndex = nIndex + 1;
		m_pImageDialog->UpdateData(FALSE);
	}
}

void CEVStatsPane::OnBnClickedStatsImageLastButton()
{
	int nIndex = 24;
	m_pMainFrame->ShowFailedImage(m_pImageDialog->m_nCamera, nIndex);
	if(!m_pMainFrame->ShowFailedImage(m_pImageDialog->m_nCamera, nIndex)) {
		m_pImageDialog->m_nImageIndex = nIndex + 1;
		m_pImageDialog->UpdateData(FALSE);
	}
}

void CEVStatsPane::OnBnClickedStatsDatarefreshButton()
{
	m_pImageDialog->PopulateDataTree();
}


void CEVStatsPane::OnLbnSelchangeStatsCharttypeList()
{
	m_pImageDialog->Plot();
}

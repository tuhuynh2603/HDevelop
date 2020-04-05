/// EvFrameWnd.cpp : implementation file of CEvFrameWnd class
/// Udupa; April'2012

#include "stdafx.h"
#include "EvFrameWnd.h"
#include "EvSdiTemplate.h"
//#include "StatisticsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEvFrameWnd

IMPLEMENT_DYNCREATE(CEvFrameWnd, CFrameWnd)

CEvFrameWnd::CEvFrameWnd()
{
}

CEvFrameWnd::~CEvFrameWnd()
{
}


BEGIN_MESSAGE_MAP(CEvFrameWnd, CFrameWnd)
	//{{AFX_MSG_MAP(CEvFrameWnd)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEvFrameWnd message handlers
BOOL CEvFrameWnd::LoadEvFrame(UINT nIDResource,
				DWORD dwDefaultStyle, CRect rect,
				CWnd* pParentWnd, DWORD dwDefaultStyleEx, 
				CCreateContext* pContext)
{
	// only do this once
//	ASSERT_VALID_IDR(nIDResource);
	ASSERT(m_nIDHelp == 0 || m_nIDHelp == nIDResource);

	m_nIDHelp = nIDResource;    // ID for help context (+HID_BASE_RESOURCE)

	CString strFullString;
	if (strFullString.LoadString(nIDResource))
		AfxExtractSubString(m_strTitle, strFullString, 0);    // first sub-string

//	VERIFY(AfxDeferRegisterClass(AFX_WNDFRAMEORVIEW_REG));

	// attempt to create the window
	LPCTSTR lpszClass = GetIconWndClass(dwDefaultStyle, nIDResource);
	LPCTSTR lpszTitle = m_strTitle;

	if (!Create(lpszClass, lpszTitle, dwDefaultStyle, rect,
	  pParentWnd, MAKEINTRESOURCE(nIDResource), dwDefaultStyleEx, pContext))
	{
		return FALSE;   // will self destruct on failure normally
	}

	// save the default menu handle
	ASSERT(m_hWnd != NULL);
	m_hMenuDefault = ::GetMenu(m_hWnd);

	// load accelerator resource
	LoadAccelTable(MAKEINTRESOURCE(nIDResource));
	EnableScrollBar(SB_BOTH, ESB_DISABLE_BOTH);

//	if (pContext == NULL)   // send initial update
//		SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);

	return TRUE;
}


void CEvFrameWnd::OnFileOpen() 
{
	CString newName;
	CEvSdiTemplate* pTemplate = (CEvSdiTemplate*) GetActiveDocument()->GetDocTemplate();

	if (!pTemplate->DoPromptFileName(newName, AFX_IDS_OPENFILE,
	  OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, TRUE))
		return; // open cancelled

	pTemplate->OpenDocumentFile(newName);
}

void CEvFrameWnd::OnFileNew() 
{
	CEvSdiTemplate* pTemplate = (CEvSdiTemplate*) GetActiveDocument()->GetDocTemplate();
	pTemplate->OpenDocumentFile(NULL);
}

BOOL CEvFrameWnd::OnEraseBkgnd(CDC* pDC) 
{
	//if(pApp->m_pStatsView) {// && pApp->m_pStatsView->m_bInitialized) {
	//	CRect rect;
	//	GetClientRect(&rect);
	//	HBRUSH hBrush = ::CreateSolidBrush(RGB(0,0,0));
	//	::FillRect(pDC->GetSafeHdc(), rect, hBrush);
	//	::DeleteObject(hBrush);
	//}

	return TRUE;
	
	//return CFrameWnd::OnEraseBkgnd(pDC);
}

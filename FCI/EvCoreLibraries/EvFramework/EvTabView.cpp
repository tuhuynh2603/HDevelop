/// EvTabView.cpp : implementation of the CEvTabView class
/// Udupa; April'2012
/// Udupa; Nov'2012; Added Inspection-time/Camera-Id unified label

#include "stdafx.h"
#include "EvImageDoc.h"
#include "EvFrameWnd.h"
#include "EvTabView.h"
#include "resource.h"
#include "EvMainFrame.h"
#include "MsgCode.h"
//#include "DirectoryExplorer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEvTabView

IMPLEMENT_DYNCREATE(CEvTabView, CCtrlView)

CEvTabView::~CEvTabView()
{
	delete[] m_SdiChildWnd;
	m_font.DeleteObject();
	m_fontResult.DeleteObject();
	m_fontId.DeleteObject();
	m_BrushActive.DeleteObject();
	m_BrushPassive.DeleteObject();
	delete[] m_pDocs;
//	if(m_nDirectoryExplorer)
//		delete m_nDirectoryExplorer;
}


BEGIN_MESSAGE_MAP(CEvTabView, CCtrlView)
	//{{AFX_MSG_MAP(CEvTabView)
	///ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()  
	ON_WM_KILLFOCUS()
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnSelChanging)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelChange)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SET_INSPSTATUS, OnSetDocStatusText)
	ON_MESSAGE(WM_SET_BINARYMODE, OnSetBinaryMode)
	ON_BN_CLICKED(IDC_CHILD_RESULT_STATIC, OnInspStatusClick)

	ON_WM_LBUTTONDOWN()

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEvTabView drawing

void CEvTabView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}

/////////////////////////////////////////////////////////////////////////////
// CEvTabView diagnostics

#ifdef _DEBUG
void CEvTabView::AssertValid() const
{
	CCtrlView::AssertValid();
}

void CEvTabView::Dump(CDumpContext& dc) const
{
	CCtrlView::Dump(dc);
}

CEvTabDoc* CEvTabView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEvTabDoc)));
	return (CEvTabDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEvTabView message handlers

void CEvTabView::OnInitialUpdate() 
{
	CCtrlView::OnInitialUpdate();

	GetTabCtrl().SetMinTabWidth(0);


	m_strDocStatus = "";
	m_bShowCamId = TRUE;
}

BOOL CEvTabView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = TCS_HOTTRACK | TCS_FOCUSNEVER | WS_VISIBLE | WS_CHILD | TCS_BUTTONS;
	//cs.style &= (~WS_BORDER & ~WS_THICKFRAME & ~DS_MODALFRAME);
	// | TCS_OWNERDRAWFIXED;// 
	///::InitCommonControls();

	// create image list 
    if(!m_imageList.Create(GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),ILC_COLOR8|ILC_MASK,4,4))
	{
		TRACE(_T("CEvTabView::OnCreate: Create() image list failed\n"));
		return -1;
	}
    

	return CCtrlView::PreCreateWindow(cs);
}

void CEvTabView::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
}

void CEvTabView::OnSelChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
	//
	// Notify derived classes that the selection is changing.
	//
	int nIndex = GetTabCtrl().GetCurSel();
	if(nIndex == -1)
		return;

	OnDeactivatePage(nIndex, m_nPageIDs[nIndex]);

	//
	// Save the input focus and hide the old page.
	//
	CEvTabPage* pDialog = m_pPages[nIndex];

	if(pDialog != NULL) {
		m_hFocusWnd[nIndex] = ::GetFocus();
		pDialog->ShowWindow(SW_HIDE);
	}
	*pResult = 0;
}

void CEvTabView::OnSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	int nIndex = GetTabCtrl().GetCurSel();
	if(nIndex == -1)
		return;

	//
	// Show the new page.
	//
	CEvTabPage* pDialog = m_pPages[nIndex];

	if(pDialog != NULL) {
		::SetFocus(m_hFocusWnd[nIndex]);
		CRect rect;
		GetClientRect(&rect);
		ResizeDialog(nIndex, rect.Width(), rect.Height());
		pDialog->ShowWindow(SW_SHOW);
	}

	//
	// Notify derived classes that the selection has changed.
	//
	OnActivatePage(nIndex, m_nPageIDs[nIndex]);
	if(pResult)
		*pResult = 0;
}


int CEvTabView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CCtrlView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_font.CreateFont(15,0,0,0,550,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"Segoe UI");
	m_fontResult.CreateFont(15,0,0,0,1000,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"Segoe UI");
	m_fontId.CreateFont(18,0,0,0,1000,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"Segoe UI");
	///GetTabCtrl().SendMessage(WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT));

	//m_colorActive = RGB(130, 160, 210);
	//m_colorActive = RGB(150, 180, 210);

	m_colorActive = RGB(158, 160, 160);

	m_colorPassive = RGB(158, 160, 160);
	m_colorPass = RGB(50, 230, 50);
	m_colorFail = RGB(230, 50, 50);
	m_colorResult = m_colorFail;
	
	m_BrushActive.CreateSolidBrush(m_colorActive);
	m_BrushPassive.CreateSolidBrush(m_colorPassive);
	m_pBrush = &m_BrushPassive;
	m_Color = m_colorActive;


	// set image list that will be in future populated with MDIChild
	// window icons
    GetTabCtrl().SetImageList(&m_imageList);
	GetTabCtrl().SetFont(&m_font);
	//GetTabCtrl().SetMinTabWidth(100);

	m_SdiChildWnd = new CEvSdiChildWnd[m_nTotalDocs];
	m_pDocs = new CEvImageDoc*[m_nTotalDocs];

	for(int i=0; i<m_nTotalDocs; i++) {
		AddPage("", IDD_CHILD_DIALOG, IDD_CHILD_DIALOG);
		//str.Format("%d", i+1);
		m_pDocs[i] = NULL;
	}

	//m_sliderBinarize.Create(WS_CHILD | WS_VISIBLE, CRect(170, 0, 350, 23), this, IDC_CHILD_BINARIZE_SLIDER);
	//m_sliderBinarize.SetRange(0, 255);
	//m_sliderBinarize.SetPos(128);
	//m_staticBinarize.Create("128", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, CRect(352, 0, 400, 23), this, IDC_CHILD_BINARIZE_STATIC);
	//m_staticBinarize.SetFont(&m_font);

	m_staticTrackStatus.Create("", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_RIGHT | SS_NOTIFY, CRect(170, 0, 350, 20), this, IDC_CHILD_RESULT_STATIC);
	m_staticTrackStatus.SetFont(&m_fontResult);

	//m_staticTrackId.Create("Camera", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_RIGHT, CRect(0, 0, 100, 10), this, IDC_CHILD_ID_STATIC);
	m_staticTrackId.Create("", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_RIGHT, CRect(0, 0, 100, 10), this, IDC_CHILD_ID_STATIC);
	m_staticTrackId.SetFont(&m_fontId);

//	if(!m_nDirectoryExplorer)
//		delete m_nDirectoryExplorer;
//	m_nDirectoryExplorer = new CDirectoryExplorer(this);
//	if(!m_nDirectoryExplorer->Create(IDD_DIRECTORY_EXPLORER, this)) {
//		delete m_nDirectoryExplorer;
//		m_nDirectoryExplorer = NULL;
//		return -1;
//	}
//
//	m_nDirectoryExplorer->ShowWindow(SW_HIDE);
	
	SetFocus();

	return 0;
}

void CEvTabView::OnDestroy() 
{
	int nCount = (int) m_pPages.GetSize();

	// delete image list
	VERIFY(m_imageList.DeleteImageList());

	// Destroy dialogs and delete CEvTabPage objects.
	if(nCount > 0) {
		for(int i=nCount - 1; i>=0; i--) {
			OnDestroyPage(i, m_nPageIDs[i]);
			CEvTabPage* pDialog = m_pPages[i];
			if(pDialog != NULL) {
				pDialog->DestroyWindow();
				delete pDialog;
			}
		}
	}

	// Clean up the internal arrays.
	m_pPages.RemoveAll();
	m_hFocusWnd.RemoveAll();
	m_nPageIDs.RemoveAll();

	CCtrlView::OnDestroy();
}

void CEvTabView::OnSetFocus(CWnd* pOldWnd) 
{
	///int nPgID = GetTabCtrl().GetCurSel();

	//CClientDC dc(this);
	//CRect rect;
	//GetTabCtrl().GetItemRect(0, &rect);
	//int nTabHeight = rect.Height() + 2;

 //   GetClientRect(&rect);
	//rect.DeflateRect(0, 0, 0, rect.Height()-nTabHeight);
 //   CBrush brush((COLORREF) ::GetSysColor(COLOR_APPWORKSPACE));
 //   dc.FillRect(rect, &brush);
	
	CEvMainFrame* pFrame = (CEvMainFrame*) AfxGetMainWnd();
	if(/*pFrame->m_pwndProperties->IsPaneVisible() &&*/pFrame->IsWindowVisible() && (pFrame->m_pwndProperties->IsDocked() || pFrame->m_pwndProperties->IsFloating())) {
		pFrame->m_pwndProperties->SetCurView(*m_pDocs);
	}
	if(/*pFrame->m_pwndTeachProperties->IsPaneVisible() &&*/pFrame->IsWindowVisible() && (pFrame->m_pwndTeachProperties->IsDocked() || pFrame->m_pwndTeachProperties->IsFloating())) {
		pFrame->m_pwndTeachProperties->SetCurView(*m_pDocs);
	}
	CCtrlView::OnSetFocus(pOldWnd);
}

void CEvTabView::OnKillFocus(CWnd* pNewWnd) 
{
	CCtrlView::OnKillFocus(pNewWnd);

	// Save the HWND of the control that holds the input focus.
	int nIndex = GetTabCtrl().GetCurSel();
	if(nIndex != -1 && nIndex < m_hFocusWnd.GetCount())
		m_hFocusWnd[nIndex] = ::GetFocus();		
}

BOOL CEvTabView::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	GetTabCtrl().GetItemRect(0, &rect);
	int nTabHeight = rect.Height() + 2;

    GetClientRect(&rect);
	rect.DeflateRect(0, 0, 0, rect.Height()-nTabHeight);

	BOOL bFocus = (GetParentFrame() == ((CEvMainFrame*) AfxGetMainWnd())->GetActiveFrame());
	m_pBrush = bFocus ? &m_BrushActive : &m_BrushPassive;
	m_Color = bFocus ? m_colorActive : m_colorPassive;
    pDC->FillRect(rect, m_pBrush);
	
	//return TRUE;
	
	return CCtrlView::OnEraseBkgnd(pDC);
}


/////////////////////////////////////////////////////////////////////////////
// Public interface

int CEvTabView::AddPage(LPCTSTR pszTitle, int nPageID, LPCTSTR pszTemplateName)
{
	//
	// Verify that the dialog template is compatible with CEvTabView
	// (debug builds only). If your app asserts here, make sure the dialog
	// resource you're adding to the view is a borderless child window and
	// is not marked visible.
	//
#ifdef _DEBUG
	if(pszTemplateName != NULL) {
		BOOL bResult = CheckDialogTemplate(pszTemplateName);
		ASSERT(bResult);
	}
#endif // _DEBUG

	// set item text to window text
	//
	// Add a page to the tab control.
	//
	TC_ITEM item;
	item.mask = TCIF_TEXT|TCIF_IMAGE;;
	item.pszText = (LPTSTR) pszTitle;
	int nIndex = GetTabCtrl().GetItemCount();
	item.iImage = nIndex;
	///item.
	
	if(GetTabCtrl().InsertItem(nIndex, &item) == -1)
		return -1;

	//
	// Add controls to the page.
	//
	if(pszTemplateName == NULL) {
		//
		// Not much to do if pszTemplateName is NULL.
		//
		int nArrayIndex = (int) m_pPages.Add(NULL);
		ASSERT(nIndex == nArrayIndex);

		nArrayIndex = (int) m_nPageIDs.Add(nPageID);
		ASSERT(nIndex == nArrayIndex);

		OnInitPage(nIndex, nPageID);
		m_hFocusWnd.Add(NULL);
	}
	else {
		//
		// Create a modeless dialog box.
		//
		CEvTabPage* pDialog = new CEvTabPage;

		if(pDialog == NULL) {
			GetTabCtrl().DeleteItem(nIndex);
			return -1;
		}

		if(!pDialog->Create(pszTemplateName, this)) {
//		if(!pDialog->Create("FOO", "CDialog", WS_CHILD, CRect(0,0,1,1), this, 1234, NULL)) {
			GetTabCtrl().DeleteItem(nIndex);
			pDialog->DestroyWindow();
			delete pDialog;
			return -1;
		}
		//
		// Record the address of the dialog object and the page ID.
		//
		int nArrayIndex = (int) m_pPages.Add(pDialog);
		ASSERT(nIndex == nArrayIndex);

		nArrayIndex = (int) m_nPageIDs.Add(nPageID);
		ASSERT(nIndex == nArrayIndex);

		//
		// Size and position the dialog box within the view.
		//
		pDialog->SetParent(this); // Just to be sure

		CRect rect;
		GetClientRect(&rect);

		if(rect.Width() > 0 && rect.Height() > 0)
			ResizeDialog(nIndex, rect.Width(), rect.Height());

		//
		// Initialize the page.
		//
		if(OnInitPage(nIndex, nPageID)) {
			//
			// Make the first control in the dialog is the one that
			// receives the input focus when the page is displayed.
			//
			HWND hwndFocus = pDialog->GetTopWindow()->m_hWnd;
			nArrayIndex = (int) m_hFocusWnd.Add(hwndFocus);
			ASSERT(nIndex == nArrayIndex);
		}
		else {
			//
			// Make the control that currently has the input focus is the one
			// that receives the input focus when the page is displayed.
			//
			m_hFocusWnd.Add(::GetFocus());
		}

		//
		// If this is the first page added to the view, make it visible.
		//
		if(nIndex == 0)
			pDialog->ShowWindow(SW_SHOW);
	}
	return nIndex;
}

int CEvTabView::AddPage(LPCTSTR pszTitle, int nPageID, int nTemplateID)
{
	return AddPage(pszTitle, nPageID, MAKEINTRESOURCE(nTemplateID));
}

BOOL CEvTabView::RemovePage(int nIndex)
{
	if(nIndex >= GetTabCtrl().GetItemCount())
		return FALSE;

	//
	// Notify derived classes that the page is being destroyed.
	//
	OnDestroyPage(nIndex, m_nPageIDs[nIndex]);

	//
	// Switch pages if the page being deleted is the current page and it's
	// not the only remaining page.
	//
	int nCount = GetTabCtrl().GetItemCount();
	if(nCount > 1 && nIndex == GetTabCtrl().GetCurSel()) {
		int nPage = nIndex + 1;
		if(nPage >= nCount)
			nPage = nCount - 2;
		ActivatePage(nPage);
	}

	//
	// Remove the page from the tab control.
	//
	GetTabCtrl().DeleteItem(nIndex);

	//
	// Destroy the dialog (if any) that represents the page.
	//
	CEvTabPage* pDialog = m_pPages[nIndex];
	if(pDialog != NULL) {
		pDialog->DestroyWindow();	
		delete pDialog;
	}

	//
	// Clean up, repaint, and return.
	//
	m_pPages.RemoveAt(nIndex);
	m_hFocusWnd.RemoveAt(nIndex);
	m_nPageIDs.RemoveAt(nIndex);
	Invalidate();
	return TRUE;
}

int CEvTabView::GetPageCount()
{
	return GetTabCtrl().GetItemCount();
}

BOOL CEvTabView::GetPageTitle(int nIndex, CString &strTitle)
{
	if(nIndex >= GetTabCtrl().GetItemCount())
		return FALSE;

	TCHAR szTitle[1024];

	TC_ITEM item;
	item.mask = TCIF_TEXT;
	item.pszText = szTitle;
	item.cchTextMax = sizeof szTitle / sizeof(TCHAR);

	if(!GetTabCtrl().GetItem(nIndex, &item))
		return FALSE;

	strTitle = item.pszText;
	return TRUE;
}

BOOL CEvTabView::SetPageTitle(int nIndex, LPCTSTR pszTitle)
{
	if(nIndex >= GetTabCtrl().GetItemCount())
		return FALSE;

	TC_ITEM item;
	item.mask = TCIF_TEXT;
	item.pszText = (LPTSTR) pszTitle;
	
	BOOL bResult = GetTabCtrl().SetItem(nIndex, &item);
	if(bResult)
		Invalidate();
	return bResult;
}

int CEvTabView::GetPageID(int nIndex)
{
	if(nIndex >= GetTabCtrl().GetItemCount())
		return -1;

	return m_nPageIDs[nIndex];
}

int CEvTabView::SetPageID(int nIndex, int nPageID)
{
	if(nIndex >= GetTabCtrl().GetItemCount())
		return -1;

	int nOldPageID = m_nPageIDs[nIndex];
	m_nPageIDs[nIndex] = nPageID;
	return nOldPageID;
}

BOOL CEvTabView::ActivatePage(int nIndex)
{
	if(nIndex >= GetTabCtrl().GetItemCount())
		return FALSE;

	//
	// Do nothing if the specified page is already active.
	//
	if(nIndex == GetTabCtrl().GetCurSel())
		return TRUE;

	//
	// Deactivate the current page.
	//
	int nOldIndex = GetTabCtrl().GetCurSel();

	if(nIndex != -1) {
		CEvTabPage* pDialog = m_pPages[nOldIndex];
		if(pDialog != NULL) {
			m_hFocusWnd[nOldIndex] = ::GetFocus();
			pDialog->ShowWindow(SW_HIDE);
		}
	}

	//
	// Activate the new one.
	//
	GetTabCtrl().SetCurSel(nIndex);
	CEvTabPage* pDialog = m_pPages[nIndex];

	if(pDialog != NULL) {
		::SetFocus(m_hFocusWnd[nIndex]);
		CRect rect;
		GetClientRect(&rect);
		ResizeDialog(nIndex, rect.Width(), rect.Height());
		pDialog->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

int CEvTabView::GetActivePage()
{
	return GetTabCtrl().GetCurSel();
}

CEvImageDoc* CEvTabView::GetActiveDoc()
{
	return m_pDocs[GetActivePage()];
}

CWnd* CEvTabView::GetPage(int nIndex)
{
	if(nIndex >= GetTabCtrl().GetItemCount())
		return NULL;

	return (CWnd*) m_pPages[nIndex];
}

int CEvTabView::GetPageIndex(int nPageID)
{
	int nCount = GetTabCtrl().GetItemCount();
	if(nCount == 0)
		return -1;

	for(int i=0; i<nCount; i++) {
		if(m_nPageIDs[i] == nPageID)
			return i;
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// Private helper functions

#ifdef _DEBUG
BOOL CEvTabView::CheckDialogTemplate(LPCTSTR pszTemplateName)
{
	//
	// Verify that the dialog resource exists.
	//
	ASSERT(pszTemplateName != NULL);
	HINSTANCE hInstance = AfxFindResourceHandle(pszTemplateName, RT_DIALOG);
	HRSRC hResource = ::FindResource(hInstance, pszTemplateName, RT_DIALOG);

	if(hResource == NULL)
		return FALSE; // Resource doesn't exist

	HGLOBAL hTemplate = LoadResource(hInstance, hResource);
	ASSERT(hTemplate != NULL);

	//
	// Get the dialog's style bits.
	//
	DLGTEMPLATEEX* pTemplate = (DLGTEMPLATEEX*) LockResource(hTemplate);

	DWORD dwStyle;
	if(pTemplate->signature == 0xFFFF)
		dwStyle = pTemplate->style;
	else
		dwStyle = ((DLGTEMPLATE*) pTemplate)->style;

	UnlockResource(hTemplate);
	FreeResource(hTemplate);

	//
	// Verify that the dialog is an invisible child window.
	//
	if(dwStyle & WS_VISIBLE)
		return FALSE; // WS_VISIBLE flag is set

	if(!(dwStyle & WS_CHILD))
		return FALSE; // WS_CHILD flag isn't set

	//
	// Verify that the dialog has no border and no title bar.
	//
	if(dwStyle & (WS_BORDER | WS_THICKFRAME | DS_MODALFRAME))
		return FALSE; // One or more border flags are set

	if(dwStyle & WS_CAPTION)
		return FALSE; // WS_CAPTION flag is set

	return TRUE;
}
#endif // _DEBUG

void CEvTabView::ResizeDialog(int nIndex, int cx, int cy)
{
	//return 
	if(nIndex > -1 && nIndex<m_nTotalDocs) {
		CEvTabPage* pDialog = m_pPages[nIndex];

		if(pDialog != NULL) {
			CRect rect;
			GetTabCtrl().GetItemRect(nIndex, &rect);

			int x, y, nWidth, nHeight;
			DWORD dwStyle = GetTabCtrl().GetStyle();

			if(dwStyle & TCS_VERTICAL) { // Vertical tabs
				int nTabWidth =
					rect.Width() * GetTabCtrl().GetRowCount();
				x = (dwStyle & TCS_RIGHT) ? 4 : nTabWidth + 4;
				y = 4;
				nWidth = cx - nTabWidth - 8;
				nHeight = cy - 8;
			}
			else { // Horizontal tabs
				int nTabHeight = rect.Height() * GetTabCtrl().GetRowCount();
				x = -2;//4;
				y = (dwStyle & TCS_BOTTOM) ? 4 : nTabHeight;
				nWidth = cx + 4;// - 8;
				nHeight = cy - nTabHeight + 2;
			}
			pDialog->SetWindowPos(NULL, x, y, nWidth, nHeight, SWP_NOZORDER);

			//switch(m_pApp->m_bMaximizedMode){
			//case SIZE_MAXIMIZED:
			//	m_nDirectoryExplorer->SetWindowPos(NULL, cx-220, 120,220 , nHeight, SWP_NOZORDER);
			//	m_nDirectoryExplorer->m_pThumbNailImageDlg->m_nTabIndex=m_nTabIndex;
			//	m_nDirectoryExplorer->m_pThumbNailImageDlg->SetWindowPos(NULL, 3, nHeight-32,cx-220-3 , 152, SWP_NOZORDER);
			//	m_nDirectoryExplorer->m_pThumbNailImageDlg->ResizeWindowControls();
			//	m_nDirectoryExplorer->ShowWindow(SW_SHOW);
			//	break;	
			//case SIZE_RESTORED: 
			//	m_nDirectoryExplorer->ShowWindow(SW_HIDE);
			//	m_nDirectoryExplorer->m_pThumbNailImageDlg->ShowWindow(SW_HIDE);
			//	break;
			//}

			int nTabHeight = rect.Height();
			int nIdOffset = 200;
			CEvImageDoc* pDoc = GetActiveDoc();
			int nStatusWidth = cx - nIdOffset - ((pDoc && pDoc->m_bBinaryMode) ? 210 : 50);//150;
			m_staticTrackStatus.SetWindowPos(NULL, nWidth-nIdOffset-nStatusWidth, 0, nStatusWidth, nTabHeight, SWP_NOZORDER | SWP_NOREDRAW);
			m_staticTrackStatus.RedrawWindow();
			m_staticTrackId.SetWindowPos(NULL, nWidth-nIdOffset, 0, nIdOffset-10, nTabHeight, SWP_NOZORDER);

			//pDialog->GetDlgItem(IDC_EvSdiChildWndCTRL1)->SetWindowPos(NULL, 0, 0, cx, nHeight, SWP_NOZORDER);
			for(int i=0; i<m_nTotalDocs; i++) {
				if(m_SdiChildWnd[i].m_hWnd)
					m_SdiChildWnd[i].m_pDocTemplate->m_pFrame->SetWindowPos(NULL, 0, 0, nWidth, nHeight, SWP_NOZORDER);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Overridables

BOOL CEvTabView::OnInitPage(int nIndex, int nPageID)
{
	//
	// Override in derived class to initialize pages.
	//
	return TRUE;
}

void CEvTabView::OnActivatePage(int nIndex, int nPageID)
{
	//
	// Override in derived class to respond to page activations.
	//
}

void CEvTabView::OnDeactivatePage(int nIndex, int nPageID)
{
	//
	// Override in derived class to respond to page deactivations.
	//
}

void CEvTabView::OnDestroyPage(int nIndex, int nPageID)
{
	//
	// Override in derived class to free resources.
	//
}

BOOL CEvTabView::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN && (pMsg->wParam==VK_PRIOR || pMsg->wParam==VK_NEXT)) {
		UCHAR nIndex = (GetActivePage()+(pMsg->wParam==VK_PRIOR?-1:1))%GetPageCount();
		ActivatePage(nIndex==0xff?GetPageCount()-1:nIndex);
		return TRUE;
	}
	return CCtrlView::PreTranslateMessage(pMsg);
}

BOOL CEvTabView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	CEvSdiTemplate* pDocTemplate = m_SdiChildWnd[GetActivePage()].m_pDocTemplate;
	if(pDocTemplate && pDocTemplate->m_pFrame->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	return CCtrlView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

HBRUSH CEvTabView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CCtrlView::OnCtlColor(pDC, pWnd, nCtlColor);
	int nId = pWnd->GetDlgCtrlID();

	if(nId==IDC_CHILD_ID_STATIC) {
        //pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0,0,0));
        pDC->SetBkColor(m_Color);
		return *m_pBrush;
	}
	else if(nId==IDC_CHILD_RESULT_STATIC) {
		//pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(GetRValue(m_Color)-100,GetGValue(m_Color)-100,GetBValue(m_Color)-100)); //RGB(80,80,130)
		//pDC->SetTextColor(m_colorResult);
		pDC->SetBkColor(m_Color);
		return *m_pBrush;
	}

	return hbr;
}

void CEvTabView::OnInspStatusClick()
{
	//if(m_bShowCamId && m_strDocStatus.IsEmpty())
	//	return;

	m_bShowCamId = !m_bShowCamId;

	if(m_bShowCamId)
		m_staticTrackStatus.SetWindowTextA(m_strCamId);
	else
		SetDocStatusText();
}

void CEvTabView::SetCameraId(CString* camId)
{
	if(!camId)
		return;

	if(!camId->IsEmpty())
		m_strCamId = *camId;
	m_staticTrackStatus.SetWindowTextA(m_strCamId);
	m_bShowCamId = TRUE;
}

LRESULT CEvTabView::OnSetDocStatusText(WPARAM wParam, LPARAM lParam)
{
	if(wParam)
		m_strDocStatus = *((CString*) wParam);
	else
		m_strDocStatus.Empty();

	SetDocStatusText();

	m_bShowCamId = FALSE;

	return 0;
}

LRESULT CEvTabView::OnSetBinaryMode(WPARAM wParam, LPARAM lParam)
{
	OnSelChange(NULL, NULL);

	return 0;
}

//void CEvTabView::OnLButtonDown(UINT nFlags, CPoint point) 
//{
//	CEvMainFrame* pFrame = (CEvMainFrame*) AfxGetMainWnd();
//	//if(pFrame->m_pwndProperties && pFrame->m_pwndProperties->IsVisible() && (pFrame->m_pwndProperties->IsDocked() || pFrame->m_pwndProperties->IsFloating())) {
//	if(pFrame->m_pwndProperties->IsPaneVisible()) {
//		//CEvImageDoc* pDoc = GetActiveDoc();
//		//if(pDoc) {
//		int nPage = GetTabCtrl().GetCurSel();
//			OutputDebugLogTo(0,TRUE,"CEvTabView::OnSetFocus Doc");
//			//pFrame->m_pwndProperties->SetCurView(pDoc);
//		//}
//	}
//	CCtrlView::OnLButtonDown(nFlags, point);
//}

#include "stdafx.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "EvMainFrame.h"
#include "strsafe.h"
#include "MsgCode.h"

#define MAX_LINE 5

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd()
{
	bIsClosed = FALSE;
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_3D_SCROLLED, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	// Create output panes:
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | LBS_MULTIPLESEL;

	if (!m_wndOutputEvent.Create(dwStyle, rectDummy, &m_wndTabs, 0) ||
		!m_wndCommEvent.Create(dwStyle, rectDummy, &m_wndTabs, 1) ||
		!m_wndDataLog.Create(dwStyle, rectDummy, &m_wndTabs, 2))
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}

	UpdateFonts();

	CString strTabName;
	BOOL bNameValid;
	
	// Attach list windows to tab:
	bNameValid = strTabName.LoadString(IDS_EVENT_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputEvent, strTabName, (UINT)0);
	bNameValid = strTabName.LoadString(IDS_COMM_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndCommEvent, strTabName, (UINT)1);
	bNameValid = strTabName.LoadString(IDS_DATA_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndDataLog, strTabName, (UINT)2);
	/*bNameValid = strTabName.LoadString(IDS_MOTOIN_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputMotion, strTabName, (UINT)3);*/

	// Fill output tabs with some dummy text (nothing magic here)
	//FillBuildWindow();
	//FillDebugWindow();
	//FillFindWindow();

	return 0;
}

void COutputWnd::OnClose()
{
	bIsClosed = TRUE;
	CDockablePane::OnClose();
}

void COutputWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	return;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndTabs.Invalidate();
	m_wndOutputEvent.Invalidate();
	m_wndCommEvent.Invalidate();
	m_wndDataLog.Invalidate();
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax*2);
	dc.SelectObject(pOldFont);
}


void COutputWnd::UpdateFonts()
{
	m_wndOutputEvent.SetFont(&afxGlobalData.fontMarlett);
	//m_wndOutputLight.SetFont(&afxGlobalData.fontMarlett);
	//m_wndOutputMotion.SetFont(&afxGlobalData.fontMarlett);
	m_wndCommEvent.SetFont(&afxGlobalData.fontMarlett);
	m_wndDataLog.SetFont(&afxGlobalData.fontMarlett);
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList()
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_POPUP_SAVETOFILE, OnSaveToFile)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList message handlers

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{

	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	/*if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}*/
	//PopulateBookmarkMenu(pMenu);

	//RECT rect;
	//m_wndMenuBar.GetItemRect(4, &rect);
	//ClientToScreen(&rect); // set the bookmark menu's location

	pSumMenu->TrackPopupMenu(TPM_RIGHTBUTTON, point.x, point.y, this, NULL);
	SetFocus();
}

void COutputList::OnEditCopy()
{
	//MessageBox(_T("Copy output"));
	// TODO: Add your command handler code here
	int nCount = GetSelCount();
	CArray<int, int> sels;
	if (nCount <= 0)
		return; // nothing to copy

	sels.SetSize(nCount);
	GetSelItems(nCount, sels.GetData());

	CString s;

	// Extract the text
	for (int i = 0; i < nCount; i++)
	{ /* copy items */
		CString t;
		GetText(sels[i], t);
		t += _T("\r\n");
		s += t;
	} /* copy items */

	HGLOBAL g = ::GlobalAlloc(GMEM_MOVEABLE, (s.GetLength() + 1) * sizeof(TCHAR));
	if (g == NULL)
	{ /* alloc failed */
		ASSERT(FALSE);  // failed to allocate memory
		return;
	} /* alloc failed */

	LPTSTR p = (LPTSTR)::GlobalLock(g);
	if (p == NULL)
	{ /* lock failed */
		ASSERT(FALSE);
		return;
	} /* lock failed */

	StringCchCopy(p, s.GetLength() + 1, (LPCTSTR)s);

	::GlobalUnlock(g);

	if (!OpenClipboard())
	{ /* clipboard open failed */
		ASSERT(FALSE);
		GlobalFree(g);
		return;
	} /* clipboard open failed */

	if (!EmptyClipboard())
	{ /* empty failed */
		ASSERT(FALSE);
		GlobalFree(g);
		return;
	} /* empty failed */

#ifdef _UNICODE
#define CF_ CF_UNICODETEXT 
#else
#define CF_ CF_TEXT
#endif
	if (::SetClipboardData(CF_, g) == NULL)
	{ /* SetClipboardData failed */
		ASSERT(FALSE); //
		::CloseClipboard();
		::GlobalFree(g);
		return;
	} /* SetClipboardData failed */
	::CloseClipboard();
}

void COutputList::OnEditClear()
{
	//MessageBox(_T("Clear output"));
	ResetContent();
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != NULL && pParentBar != NULL)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}


void COutputList::OnSaveToFile()
{
	CFileDialog   dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Log File (*.log)|*.log|All Files (*.*)|*.*||");

	CString strFileName;

	if (dlg.DoModal() == IDOK) {
		strFileName = dlg.GetPathName();
		CString strExt = dlg.GetFileExt();
		if (strExt.IsEmpty()) {
			strExt = "log";
			strFileName += "." + strExt;
		}
		else
			strExt.MakeLower();

		//--. Create file
		FILE * fileData;

		fileData = fopen(strFileName, "w");

		if (fileData == NULL) return;

		//--. Write data
		int nStringNo = GetCount();
		CString strItemData;

		for (int i = 0; i<nStringNo; i++) {

			GetText(i, strItemData);

			fprintf(fileData, "%s\n", strItemData);
		}

		//--. Close file
		fclose(fileData);
	}
}


int COutputWnd::AddToEventLogPane(CString strMsgLog,int nEventLog)
{
	
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	CString strTemp;
	strTemp.Format("%04d/%02d/%02d %02d:%02d:%02d.%03d %s", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds, strMsgLog);

	int nCount;
	switch (nEventLog) {
	case 0:m_wndOutputEvent.InsertString(0, strTemp);
		AdjustHorzScroll(m_wndOutputEvent);
		nCount = m_wndOutputEvent.GetCount();
		if (nCount == m_nMaxLines)
			m_wndOutputEvent.DeleteString(nCount - 1);
		break;
	case 1:m_wndCommEvent.InsertString(0, strTemp);
		AdjustHorzScroll(m_wndCommEvent);
		nCount = m_wndCommEvent.GetCount();
		if (nCount == m_nMaxLines)
			m_wndCommEvent.DeleteString(nCount - 1);
		break;
	case 2:m_wndDataLog.InsertString(0, strTemp);
		AdjustHorzScroll(m_wndDataLog);
		nCount = m_wndDataLog.GetCount();
		if (nCount == m_nMaxLines)
			m_wndDataLog.DeleteString(nCount - 1);
		break;
	/*case 3:m_wndOutputLight.InsertString(0, strTemp);
		AdjustHorzScroll(m_wndOutputLight);
		break;*/
	}
	return 0;
}

void COutputWnd::OnPressCloseButton()
{
	CDockablePane::OnPressCloseButton();
	((CEvMainFrame*)AfxGetMainWnd())->bIsOutputPaneVisible = FALSE;
	bIsClosed = FALSE;
}
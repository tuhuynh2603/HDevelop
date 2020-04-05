// MagnifierWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "AppDoc.h"
#include "MagnifierWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMagnifierWnd

CMagnifierWnd::CMagnifierWnd()
{
	SavedDC = 0;
}

CMagnifierWnd::~CMagnifierWnd()
{
	if (m_hdc)
    {
        ::RestoreDC(m_hdc, SavedDC);
        ::DeleteDC(m_hdc);
    }
	if (m_Bmap)
        ::DeleteObject(m_Bmap);
    if (IsWindow(m_hWnd))
        DestroyWindow();
	m_hPen.DeleteObject();
}


BEGIN_MESSAGE_MAP(CMagnifierWnd, CWnd)
	//{{AFX_MSG_MAP(CMagnifierWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMagnifierWnd message handlers

BOOL CMagnifierWnd::Create(CAppDoc *pDoc,int size, CWnd* pParentWnd) 
{
	
	ASSERT (IsWindow(pParentWnd->m_hWnd));
    ASSERT (size > 9);
    
    Size = size;
    if (Size < 10)
        Size = 10;
    path = "MirrorImage.bmp";
	m_nZoomValue = 0;
	m_pDoc = pDoc;
	return CWnd::CreateEx(0,
        AfxRegisterWndClass(NULL, SetCursor(LoadCursor(NULL,IDC_SIZEALL)), (HBRUSH)GetStockObject(NULL_BRUSH), NULL),
        _T(""), WS_POPUP,// | WS_BORDER,
        CRect (0, 0, Size, Size),
        pParentWnd, NULL, NULL);
}
BOOL CMagnifierWnd::ShowMagnifier()
{
    ASSERT (IsWindow(m_hWnd));  // first call CMagnifierWindow::Create()

    if (!IsWindow(m_hWnd))
        return FALSE;

    SetFocus();
    
    CPoint pt;
    GetCursorPos(&pt);
    SetWindowPos(&wndTopMost, pt.x + Size / 2, pt.y + Size / 2, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
	
	CRect rc;
	GetClientRect(&rc);
	CRgn rgn;
	rgn.CreateRoundRectRgn(rc.left,rc.top,rc.right,rc.bottom,700,700);
	SetWindowRgn(rgn,TRUE);
	if (m_hdc)
    {
        ::RestoreDC(m_hdc, SavedDC);
        ::DeleteDC(m_hdc);
    }
	if (m_Bmap)
        ::DeleteObject(m_Bmap);
	m_hPen.DeleteObject();
	m_hPen.CreatePen(PS_SOLID, 5, RGB(0, 0, 255));
    return TRUE;
}
void CMagnifierWnd::Clear()
{
	if (m_hdc)
    {
        ::RestoreDC(m_hdc, SavedDC);
        ::DeleteDC(m_hdc);
    }
	if (m_Bmap)
        ::DeleteObject(m_Bmap);
    if (IsWindow(m_hWnd))
        DestroyWindow();
	m_hPen.DeleteObject();
}
void CMagnifierWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect(&rect);
	if(m_Bmap)
        ::DeleteObject(m_Bmap);
	m_Bmap=(HBITMAP)::LoadImage(NULL,path,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if(m_hdc)
		::DeleteObject(m_hdc);
	m_hdc=::CreateCompatibleDC(dc.m_hDC);
	::SelectObject(m_hdc,m_Bmap);
	SavedDC = ::SaveDC(m_hdc);
	//BitBlt(dc.m_hDC,0,0,rect.right,rect.bottom,m_hdc,0,0,SRCCOPY);
	dc.SetStretchBltMode(COLORONCOLOR);
	VERIFY (::StretchBlt(dc.m_hDC, 0, 0,
            rect.right,
            rect.bottom,
            m_hdc,
            m_nZoomValue,
            m_nZoomValue,
            250 - m_nZoomValue * 2, 250 - m_nZoomValue * 2, SRCCOPY));
	dc.SelectObject(m_hPen);
	dc.Arc(1,1,248,248,0,0,0,0);
	GetParent()->RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}
BOOL CMagnifierWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(zDelta > 0)  //+ve Move Mouse wheel in forward
		ZoomIn();
	else
		ZoomOut();
	Invalidate();
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
void CMagnifierWnd::ZoomIn()
{
	m_nZoomValue +=10;
	if(m_nZoomValue > 120)
		m_nZoomValue = 120;
	Invalidate();
}
void CMagnifierWnd::ZoomOut()
{
	m_nZoomValue -=10;
	if(m_nZoomValue < 0)
		m_nZoomValue = 0;
	Invalidate();
}
void CMagnifierWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	switch(nChar)
	{
		case VK_UP:
			ZoomIn();
			break;
		case VK_DOWN:
			ZoomOut();
			break;
		case VK_ESCAPE:
			Clear();
			DestroyWindow();
			pApp->m_bModifierView = FALSE;
			DrwDel(&m_pDoc->m_ImageViewInfo);
			DrwUpdate(&m_pDoc->m_ImageViewInfo);
			break;
		default:
			CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
			return;
	}
}

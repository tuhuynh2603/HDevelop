/// MagnifierWnd.cpp : implementation of the CMagnifierWnd class
/// Udupa; April'2012

#include "stdafx.h"
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
	m_bufMagnifier.Allocate(CSize(250, 250), true);
}

CMagnifierWnd::~CMagnifierWnd()
{
	m_hPen.DeleteObject();

	if(IsWindow(m_hWnd))
        DestroyWindow();
}

BEGIN_MESSAGE_MAP(CMagnifierWnd, CWnd)
	//{{AFX_MSG_MAP(CMagnifierWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMagnifierWnd message handlers

BOOL CMagnifierWnd::Create(int size, CWnd* pParentWnd) 
{
	ASSERT(IsWindow(pParentWnd->m_hWnd));
    ASSERT(size > 9);
    
    if(size < 10)
        size = 10;
	m_nZoomValue = 80;

	BOOL bCreated = CWnd::CreateEx(0,
        AfxRegisterWndClass(NULL, SetCursor(LoadCursor(NULL,IDC_SIZEALL)), (HBRUSH)GetStockObject(NULL_BRUSH), NULL),
        _T(""), WS_POPUP,// | WS_BORDER,
        CRect (0, 0, size, size),
        pParentWnd, NULL, NULL);


	if(bCreated) {
		CPoint pt;
		GetCursorPos(&pt);
		SetWindowPos(&wndTopMost, pt.x + size / 2, pt.y + size / 2, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

		CRect rc;
		GetClientRect(&rc);
		CRgn rgn;
		rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 700, 700);
		SetWindowRgn(rgn, TRUE);
		rgn.DeleteObject();

		m_hPen.CreatePen(PS_SOLID, 3, RGB(50, 50, 150));
	}

	return bCreated;
}

void CMagnifierWnd::OnPaint() 
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);

	int width = 250 - m_nZoomValue * 2;
	::StretchDIBits(dc.m_hDC,
					0,0,
					rect.right,
					rect.bottom,
					m_nZoomValue,
					m_nZoomValue,
					width, 
					width,
					(void*) m_bufMagnifier.GetImgPtr(),
					m_bufMagnifier.GetAdapter()->GetBitmapInfo(),
					DIB_RGB_COLORS, SRCCOPY);

	HGDIOBJ oldBrush = dc.SelectObject((HBRUSH)GetStockObject(NULL_BRUSH));
	CPen* oldPen = dc.SelectObject(&m_hPen);
	dc.Ellipse(0, 0, 249, 249);

	dc.SelectObject(oldBrush);
	dc.SelectObject(oldPen);

	GetWindowRect(&rect);
	GetParent()->RedrawWindow(&rect, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}

BOOL CMagnifierWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;	
}

BOOL CMagnifierWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(zDelta > 0)
		ZoomIn();
	else
		ZoomOut();

	return TRUE;
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
	if(m_nZoomValue < 20)
		m_nZoomValue = 20;

	Invalidate();
}

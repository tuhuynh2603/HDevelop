#include "stdafx.h"
#include "MagnifierWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMagnifierWindow

CMagnifierWindow::CMagnifierWindow()
{
}

void CMagnifierWindow::AllocateBuffer(BOOL bColor)
{
	if(!m_bufMagnifier.IsAllocated())
		m_bufMagnifier.Allocate(CSize(250, 250), true, bColor);
}

void CMagnifierWindow::SetImage(CImageBuffer* pSourceBuf, CRect* pRect)
{
	if(pRect == NULL)
		pRect = &m_rectSource;
	else
		m_rectSource = *pRect;

	CRect rcPartMagnifier;
	rcPartMagnifier = CRect(CPoint(0,0),pRect->Size());
	if(pRect->Width() < 250 || pRect->Height() < 250) {
		if(pRect->left == 0)
			rcPartMagnifier = rcPartMagnifier + CPoint(250 - pRect->right,0);
		if((pRect->top == 0))
			rcPartMagnifier = rcPartMagnifier + CPoint(0,250 - pRect->bottom);
	}
	ImgCopy(pSourceBuf, pRect, &m_bufMagnifier, &rcPartMagnifier);	
	CopyFromColorTable(pSourceBuf,m_bufMagnifier.GetAdapter());

}

CMagnifierWindow::~CMagnifierWindow()
{
	if(m_hPen.m_hObject != NULL)
		m_hPen.DeleteObject();

	if(m_hOutlinePen.m_hObject != NULL)
		m_hOutlinePen.DeleteObject();

	if(IsWindow(m_hWnd))
        DestroyWindow();
}

BEGIN_MESSAGE_MAP(CMagnifierWindow, CWnd)
	//{{AFX_MSG_MAP(CMagnifierWindow)
	ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMagnifierWindow message handlers

BOOL CMagnifierWindow::Create(int size, CWnd* pParentWnd) 
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

		m_hPen.CreatePen(PS_SOLID, 5, RGB(0, 0, 255));
		m_hOutlinePen.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
	}

	return bCreated;
}

void CMagnifierWindow::OnPaint() 
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

BOOL CMagnifierWindow::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;	
}

BOOL CMagnifierWindow::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(zDelta > 0)
		ZoomIn();
	else
		ZoomOut();

	return TRUE;
}

void CMagnifierWindow::ZoomIn()
{
	m_nZoomValue +=10;
	if(m_nZoomValue > 120)
		m_nZoomValue = 120;

	Invalidate();
}

void CMagnifierWindow::ZoomOut()
{
	m_nZoomValue -=10;
	if(m_nZoomValue < 20)
		m_nZoomValue = 20;

	Invalidate();
}


void CMagnifierWindow::CopyFromColorTable(CImageBuffer* pImgDisplaySource, CDisplayAdapter *pDisplayAdapterDest)
{
	RGBQUAD* sourceColors = pImgDisplaySource->GetAdapter()->GetColorTable();
	RGBQUAD* destColors = pDisplayAdapterDest->GetColorTable();
	memcpy(destColors, sourceColors, 256 * sizeof(RGBQUAD));
	for (int i = 0; i<256; i++) {
		destColors[i].rgbRed = sourceColors[i].rgbBlue;
		destColors[i].rgbGreen = sourceColors[i].rgbGreen;
		destColors[i].rgbBlue = sourceColors[i].rgbRed;
		destColors[i].rgbReserved = 0;
	}
}
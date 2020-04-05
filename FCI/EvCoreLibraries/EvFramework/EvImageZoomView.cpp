// ZoomView.cpp : implementation file
//

#include "stdafx.h"
#include "EvImageZoomView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEvImageZoomView

IMPLEMENT_DYNAMIC(CEvImageZoomView, CScrollView)

CEvImageZoomView::CEvImageZoomView():CChildView()
{
    m_zoomMode    = CEvImageZoomView::SEC_ZOOMOFF;
    m_fZoomScale  = 1.0f;
    m_fZoomMin  = -1.0f;
    m_fZoomMax = -1.0f;
    ZoomLevelChanged();
    m_bCenter = TRUE;   
}

CEvImageZoomView::~CEvImageZoomView()
{
}


BEGIN_MESSAGE_MAP(CEvImageZoomView, CScrollView)
	//{{AFX_MSG_MAP(CEvImageZoomView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
// CEvImageZoomView diagnostics

#ifdef _DEBUG
void CEvImageZoomView::AssertValid() const
{
    if (m_fZoomMin != -1 && m_fZoomMax !=-1) {
		ASSERT (m_fZoomMin <= m_fZoomScale);
		ASSERT (m_fZoomScale <= m_fZoomMax);
	}

	CView::AssertValid();
}

void CEvImageZoomView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);

    dc << "zoom scale = " << m_fZoomScale << "\n";
    dc << "zoom min = " << m_fZoomMin << "\n";
    dc << "zoom max = " << m_fZoomMax << "\n";
    dc << "zoom mode = " << m_zoomMode << "\n";
    dc << "original size = " << m_szOrigTotalDev << "\n";
    dc << "original page size = " << m_szOrigPageDev << "\n";
    dc << "line size = " << m_szOrigLineDev << "\n";
    dc << "mapping mode = " << m_nMapMode << "\n";
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEvImageZoomView message handlers


void CEvImageZoomView::ZoomOutOfRange(BOOL bUnderFlow, float fAttempt)
{

}

void CEvImageZoomView::SetZoomLevel(float fNewScale, CPoint* ptCentre)
{
    if (m_fZoomMax == -1 && m_fZoomMin == -1){
		m_fZoomScale = fNewScale;
		UpdateViewport(ptCentre);
		return;
    }

    if (fNewScale <=m_fZoomMax && fNewScale >=m_fZoomMin){
		m_fZoomScale = fNewScale;
		UpdateViewport(ptCentre);
		return;
    }
    
    //Problems here.
    if (fNewScale < m_fZoomMin)
		ZoomOutOfRange(TRUE,fNewScale); //Underflow
    else
		ZoomOutOfRange(FALSE,fNewScale); //Overflow
}

void CEvImageZoomView::SetScrollSizes(int nMapMode,
				  SIZE sizeTotal,
				  const SIZE& sizePage /* = sizeDefault */, // in logical units
				  const SIZE& sizeLine /* = sizeDefault */)	// in logical units
{
    ASSERT_VALID(this);
    
    nMapMode; //UNUSED
    // Set up the defaults
    if (sizeTotal.cx == 0 && sizeTotal.cy == 0){
	sizeTotal.cx = 1;
	sizeTotal.cy = 1;
    }
    //SetWindowExt doesn't like 0,0 will bomb out.
    ASSERT(sizeTotal.cx > 0 && sizeTotal.cy > 0);
    
    m_nMapMode    = MM_ANISOTROPIC; // Need for arbitrary scaling
    m_totalLog    = sizeTotal;
    
    // Setup default Viewport extent to be conversion of Window extent
    // into device units.
    
    //BLOCK for DC
    {
	CWindowDC dc(NULL);
	dc.SetMapMode(m_nMapMode);
	
	// total size
	m_totalDev = m_totalLog;
	dc.LPtoDP((LPPOINT)&m_totalDev);
    } // Release DC here
    
    m_szOrigTotalDev = m_totalDev;
    m_szOrigPageDev  = sizePage;
    m_szOrigLineDev  = sizeLine;
    ReCalcBars();
    
    ZoomLevelChanged(); //Notify app that there's a new zoom level 1.0f.   
}

void CEvImageZoomView::CenterOnLogicalPoint(CPoint ptCenter)
{
    ViewLPtoDP(&ptCenter);
    ClientToDevice(ptCenter);
    CScrollView::CenterOnPoint(ptCenter);
}

CPoint CEvImageZoomView::GetLogicalCenterPoint()
{
    CPoint pt;
    CRect rect;

    // Get the center of screen
    GetClientRect(&rect);
    pt.x = (rect.Width()  / 2);
    pt.y = (rect.Height() / 2);
    
    // Convert the point to logical coordinates
    ViewDPtoLP(&pt);
    return pt;
}

void CEvImageZoomView::SetZoomMinMax(float fMin, float fMax)
{
    ASSERT_VALID(this);
    ASSERT(fMin < fMax);
    
    ASSERT(m_fZoomScale <= fMax);
    ASSERT(m_fZoomScale >= fMin);
    
    m_fZoomMin = fMin;
    m_fZoomMax = fMax;
}

void CEvImageZoomView::SetZoomMinMaxPercent(int nMin, int nMax)
{
    ASSERT_VALID(this);
    ASSERT(nMin < nMax);
    
    SetZoomMinMax(nMin/100.0f,nMax/100.0f);
}

void CEvImageZoomView::SetZoomMode(UINT zoomMode)
{
    ASSERT_VALID(this);
    ASSERT(m_nMapMode == MM_ANISOTROPIC);
    if (zoomMode != m_zoomMode)
	m_zoomMode = zoomMode;
    return;
}

void CEvImageZoomView::ZoomIn(CRect &rect)
{
    ASSERT_VALID(this);
    ASSERT(m_nMapMode == MM_ANISOTROPIC);
    if (m_zoomMode == CEvImageZoomView::SEC_ZOOMNORMAL){ //Only if we're in normal zoom mode.
		// Make sure that the rect is normalized
		CRect zoomRect = rect;
		zoomRect.NormalizeRect();
		
		// Get the center of rect
		CPoint ptCenter;
		ptCenter.x = ((zoomRect.left + zoomRect.right)  / 2);
		ptCenter.y = ((zoomRect.top  + zoomRect.bottom) / 2);
		
		CRect clientRect;
		GetClientRect(&clientRect);
		
		// Calculate the new zoom scale based on input rect
		// and the current client rect.
		
		float fHorizScale =
			(float) (clientRect.right  + 1) / (float) zoomRect.Width();
		float fVertScale  =
			(float) (clientRect.bottom + 1) / (float) zoomRect.Height();
		
		// Keep the scale Isotropic
		SetZoomLevel(__min(fHorizScale, fVertScale));

		UpdateViewport(&ptCenter);
    } 
	return;
}

void CEvImageZoomView::ZoomIn(CPoint *point, float delta)
{
    ASSERT_VALID(this);
    ASSERT(m_nMapMode == MM_ANISOTROPIC);

    if (m_zoomMode == CEvImageZoomView::SEC_ZOOMNORMAL) {
		// Increase the zoom scale.
		SetZoomLevel(m_fZoomScale * delta);
		
		UpdateViewport(point);
    } //Not in zoom mode, simply ignore it.
    
    return;
}

void CEvImageZoomView::ZoomOut(CPoint *point, float delta)
{
    ASSERT_VALID(this);
    ASSERT(m_nMapMode == MM_ANISOTROPIC);

    if (m_zoomMode == CEvImageZoomView::SEC_ZOOMNORMAL){
		
		// Decrease the zoom scale.
		SetZoomLevel(m_fZoomScale / delta);
		
		UpdateViewport(point);
    }// Not in zoom mode, ignore.
    
    return;
	
}

void CEvImageZoomView::ZoomOriginal(CPoint *point)
{
    ASSERT_VALID(this);
    ASSERT(m_nMapMode == MM_ANISOTROPIC);
	
    // Reset the zoom scale to 1.0
    if (m_zoomMode == CEvImageZoomView::SEC_ZOOMNORMAL){
		m_fZoomScale = 1.0f;
		
		UpdateViewport(point);
    } // We're not in zoom mode, ignore.
    return;
	
}

void CEvImageZoomView::ZoomFit()
{
	//ShowScrollBar(SB_BOTH, FALSE);
    ASSERT_VALID(this);
    ASSERT(m_nMapMode == MM_ANISOTROPIC);
    
    CRect  rc;
    CPoint pt;
    CSize  sizeSb;
    
    if (m_zoomMode == CEvImageZoomView::SEC_ZOOMNORMAL/*SEC_ZOOMFIT*/) {
		GetTrueClientSize(m_totalDev, sizeSb);    
		PersistRatio(m_totalLog, m_totalDev, pt);
		
		SetZoomLevel((float) m_totalDev.cx / m_szOrigTotalDev.cx);
		
		UpdateBars(); //Scrollbars should go away, view fills client
		Invalidate(TRUE);
		
		ZoomLevelChanged();
    } //Not in zoom fit mode, ignore.
	
	//ShowScrollBar(SB_BOTH, TRUE);
    return;
}

void CEvImageZoomView::ZoomPercent(int nPercent, CPoint *point)
{
    ASSERT_VALID(this);
    ASSERT(m_nMapMode == MM_ANISOTROPIC);
    ASSERT(nPercent != 0);
    
    if (m_zoomMode == CEvImageZoomView::SEC_ZOOMNORMAL){
		//Calculate new zoom scale based on %
		m_fZoomScale = nPercent/100.0f;
		
		UpdateViewport(point);
    }
    return;
}

void CEvImageZoomView::ViewDPtoLP(LPPOINT lpPoints, int nCount)
{
    ASSERT(m_nMapMode > 0); // must be set
    CWindowDC dc(this);
    OnPrepareDC(&dc);
    dc.DPtoLP(lpPoints, nCount);
}

void CEvImageZoomView::ViewLPtoDP(LPPOINT lpPoints, int nCount)
{
    ASSERT(m_nMapMode > 0); // must be set
    CWindowDC dc(this);
    OnPrepareDC(&dc);
    dc.LPtoDP(lpPoints, nCount);
}

void CEvImageZoomView::ClientToDevice(CPoint &point)
{
    CPoint scrollPt = GetDeviceScrollPosition();
    point.x += scrollPt.x;
    point.y += scrollPt.y;
}

void CEvImageZoomView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
#ifdef _DEBUG
    if (m_nMapMode != MM_ANISOTROPIC) {
		TRACE0("Error: must call SetScrollSizes() before painting view\n");
		ASSERT(FALSE);
		return;
    }
#endif //_DEBUG
    
    ASSERT_VALID(pDC);
    ASSERT(m_totalLog.cx >= 0 && m_totalLog.cy >= 0);
    ASSERT(m_totalDev.cx >= 0 && m_totalDev.cx >= 0);
    
    // Set the Mapping mode, and the window and viewport extents
    // Note: This will always be ANISOTROPIC for CEvImageZoomView
    pDC->SetMapMode(m_nMapMode);
    pDC->SetWindowExt(m_totalLog);  //Set up the logical window
	
    //Now figure out the origin for the viewport, depends on
    //This code is from CSCrollView
    CPoint ptVpOrg(0, 0);
    
    if (!pDC->IsPrinting()) {
		pDC->SetViewportExt(m_totalDev); // in device coordinates
		
		// by default shift viewport origin in negative direction of scroll
		ASSERT(pDC->GetWindowOrg() == CPoint(0,0));
		
		if (m_zoomMode == CEvImageZoomView::SEC_ZOOMPANOVER) { //Special mode for pan overviewing
			ptVpOrg.x = 0; //m_totalDev.cx /2;
			ptVpOrg.y = 0; //m_totalDev.cy /2;
		} else 
			ptVpOrg = -GetDeviceScrollPosition();
		
		// Center full fit
		if (m_bCenter) {
			CRect rect;
			GetClientRect(&rect);
			
			// if client area is larger than total device size,
			// override scroll positions to place origin such that
			// output is centered in the window
			if (m_totalDev.cx < rect.Width())
				ptVpOrg.x = (rect.Width() - m_totalDev.cx) / 2;
			if (m_totalDev.cy < rect.Height())
				ptVpOrg.y = (rect.Height() - m_totalDev.cy) / 2;
		}
    } 
	else {
		// Special case for printing
		CSize  printSize;
		printSize.cx = pDC->GetDeviceCaps(HORZRES);
		printSize.cy = pDC->GetDeviceCaps(VERTRES);
		// Maintain the origional ratio, setup origin shift
		PersistRatio(m_totalLog, printSize, ptVpOrg);
		// Zoom completely out
		pDC->SetViewportExt(printSize);
    }
    
    // Set the new viewport origin, call CView for printing behavior
    pDC->SetViewportOrg(ptVpOrg);
    CView::OnPrepareDC(pDC, pInfo);
}

BOOL CEvImageZoomView::OnPreparePrinting(CPrintInfo* pInfo) 
{
    return DoPreparePrinting(pInfo);
}

void CEvImageZoomView::PersistRatio(const CSize &orig, CSize &dest, CPoint &remainder)
{
    float ratio1 = (float) orig.cx / orig.cy;
    float ratio2 = (float) dest.cx / dest.cy;

    int   newSize;
    
    // Do nothing if they are the same
    if (ratio1 > ratio2) {
		// Shrink hieght
		newSize = (int)(dest.cx / ratio1);
		remainder.x = 0;
		remainder.y = dest.cy - newSize;
		dest.cy = newSize;
    } else if (ratio2 > ratio1) {
		// Shrink width
		newSize = (int)(dest.cy * ratio1);
		remainder.x = dest.cx - newSize;
		remainder.y = 0;
		dest.cx = newSize;
    }
}

void CEvImageZoomView::ReCalcBars()
{
    ASSERT_VALID(this);

    {  
		CWindowDC dc(NULL);
		dc.SetMapMode(m_nMapMode);
		
		// Calculate new device units for scrollbar
		// Start with origional logical units from SetScrollPos
		m_pageDev = m_szOrigPageDev;
		dc.LPtoDP((LPPOINT)&m_pageDev);
		m_lineDev = m_szOrigLineDev;
		dc.LPtoDP((LPPOINT)&m_lineDev);
	} // Free DC
	
	// Make sure of the range
	if (m_pageDev.cy < 0)  m_pageDev.cy = -m_pageDev.cy;
	if (m_lineDev.cy < 0)  m_lineDev.cy = -m_lineDev.cy;
	
	// If none specified - use one tenth, Just like CScrollView
	
	//ASSERT(m_totalDev.cx >= 0 && m_totalDev.cy >= 0);
	if (m_totalDev.cx < 0 || m_totalDev.cy < 0){
		TRACE0("CEvImageZoomView: Zoom level too low, divide by zero possible\n");
		ASSERT(FALSE);
	}
	
	if (m_pageDev.cx == 0) m_pageDev.cx = m_totalDev.cx / 10;
	if (m_pageDev.cy == 0) m_pageDev.cy = m_totalDev.cy / 10;
	if (m_lineDev.cx == 0) m_lineDev.cx = m_pageDev.cx  / 10;
	if (m_lineDev.cy == 0) m_lineDev.cy = m_pageDev.cy  / 10;
	
	// Now update the scrollbars
	if (m_hWnd != NULL) {
		UpdateBars();
		Invalidate(TRUE); // Zoom scale changed, redraw all
	}
}

void CEvImageZoomView::UpdateViewport(CPoint *ptNewCenter)
{
    //Ok for ptNewCenter = NULL
    ASSERT_VALID(this);
    CPoint ptCenter;
    
    if (!ptNewCenter)
		ptCenter = GetLogicalCenterPoint();
    else 
		ptCenter = *ptNewCenter;
    
    // Modify the Viewport extent
    m_totalDev.cx = (int) ((float) m_szOrigTotalDev.cx * m_fZoomScale);
    m_totalDev.cy = (int) ((float) m_szOrigTotalDev.cy * m_fZoomScale);
    ReCalcBars();
    
    // Set the current center point.
    CenterOnLogicalPoint(ptCenter);
    
    // Notify the class that a new zoom scale was done
    ZoomLevelChanged();
    return;
}

void CEvImageZoomView::OnSize(UINT nType, int cx, int cy) 
{
    ASSERT_VALID(this);
    
    if (GetZoomMode() == CEvImageZoomView::SEC_ZOOMFIT)
	ZoomFit();

    CScrollView::OnSize(nType, cx, cy);
}

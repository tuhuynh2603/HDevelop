// VisionView.cpp : implementation of the CEvImageView class
// Udupa; Dec'12/Jan'13; Added Mouse based panning/zooming, ShowOverlay toggle

#include "stdafx.h"
#include "EvImageDoc.h"
#include "EvImageView.h"
#include "EvMainFrame.h"
#include "ThresholdDialog.h"
#include "PanWindow.h"
#include "SupportFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEvImageView

IMPLEMENT_DYNCREATE(CEvImageView, CEvImageZoomView)

BEGIN_MESSAGE_MAP(CEvImageView, CEvImageZoomView)
	//{{AFX_MSG_MAP(CEvImageView)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_NCPAINT()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCMOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP

	//User Defined Messages
	ON_MESSAGE(WM_SET_THRESHOLD, OnSetThreshold)
	ON_MESSAGE(WM_BINARY_MODE, BinaryMode)



	ON_MESSAGE(WM_SET_GRAYPALLETTE, SetGrayPallette)
	ON_MESSAGE(WM_ZOOM_IN, ZoomIn)
	ON_MESSAGE(WM_ZOOM_OUT, ZoomOut)
	ON_MESSAGE(WM_ZOOM_FIT, ZoomFit)

	ON_MESSAGE(WM_DRW_DEL, ViewDrwDel)
	ON_MESSAGE(WM_DRW_UPD, ViewDrwUpd)
	ON_MESSAGE(WM_DRW_CUR, ViewDrwCur)
	ON_MESSAGE(WM_DRW_LINE, ViewDrwLine)
	ON_MESSAGE(WM_DRW_RECT, ViewDrwRect)
	ON_MESSAGE(WM_DRW_CIRCLE, ViewDrwCircle)
	ON_MESSAGE(WM_DRW_TRACK_RECT, ViewDrwTrackRect)
	ON_MESSAGE(WM_DRW_TRACK_CROSS, ViewDrwTrackCross)
	ON_MESSAGE(WM_DRW_STR, ViewDrwString)
	ON_MESSAGE(WM_DRW_STRING, ViewDrwStr)
	ON_MESSAGE(WM_STR_DEL, ViewDeleteString)

	ON_MESSAGE(WM_UPDATE_IMG, ViewUpdImg)
	ON_MESSAGE(WM_RESTORE_IMG, ViewRestorePrevImg)

	ON_MESSAGE(WM_CUR_DEL, ViewCurDel)
	ON_MESSAGE(WM_LINE_DEL, ViewLineDel)
	
	ON_MESSAGE(WM_RECT_DEL, ViewDelRect)
	ON_MESSAGE(WM_CIRCLE_DEL, ViewDelCircle)

	ON_MESSAGE(WM_SETSCROLLSIZE, ViewSetScrollSize)

	ON_WM_MOUSEWHEEL()

	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

void CVisRectTracker::OnChangedRect(const CRect& rectOld)
{
	int nWidth,nHeight;
	CRect rcNew;

	ASSERT(m_pView);

	if (m_sizeImage.cx == 0)
		m_sizeImage = m_pView->GetImageSize();

	m_rect.NormalizeRect();

	rcNew = rectOld;
	m_pView->GetImageRect(rcNew);

	nWidth = rcNew.Width();
	nHeight = rcNew.Height();

	rcNew = m_rect;
	m_pView->GetImageRect(rcNew);

	if(rcNew.top <0)
		rcNew = CRect(CPoint(rcNew.left,0),CSize(nWidth,nHeight));
	if (rcNew.left<0)
		rcNew = CRect(CPoint(0,rcNew.top),CSize(nWidth,nHeight));
	if(rcNew.bottom > m_sizeImage.cy-1)
		rcNew = CRect(CPoint(rcNew.left,m_sizeImage.cy-1-nHeight),CSize(nWidth,nHeight));
	if (rcNew.right > m_sizeImage.cx-1)
		rcNew = CRect(CPoint(m_sizeImage.cx-1-nWidth,rcNew.top),CSize(nWidth,nHeight));

	m_pView->GetDeviceRect(rcNew);
	m_rect = rcNew;
	m_pView->UpdateTrackRectPos(m_rect);
}


/////////////////////////////////////////////////////////////////////////////
// CEvImageView construction/destruction

CEvImageView::CEvImageView()
{
	m_pdlgThres = NULL;

	m_bStretch = FALSE;
	m_bCenter = FALSE;

	// Set Style of Tracker Rect
	m_Tracker.m_nStyle|=CRectTracker::resizeInside;
	//m_Tracker.m_nStyle|=CRectTracker::solidLine;
	m_Tracker.m_nHandleSize=5;

	m_pPanDlg = NULL;
	m_bFramed = TRUE;
	teachRect=false;
	m_bPanning = FALSE;
	m_bMagnifierView = FALSE;
}

CEvImageView::~CEvImageView()
{
	if(m_pPanDlg)
		delete m_pPanDlg;
	if(m_pdlgThres)
		delete m_pdlgThres;
}

BOOL CEvImageView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CEvImageZoomView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CEvImageView drawing

void CEvImageView::OnDraw(CDC* pDC)
{
	CPen *pOldPen, GreenPen, TrackPen, DummyPen;//,ProcessPen;
	CBrush *pOldBrush;
	int i;
	TRACK_INFO TrackInfo;

	DummyPen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	GreenPen.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	TrackPen.CreatePen(PS_SOLID, 2, RGB(255, 200, 0));
	//ProcessPen.CreatePen(PS_SOLID, 1, RGB(0, 200, 200));

	pOldPen   = pDC->SelectObject(&DummyPen);
	pOldBrush = (CBrush *)pDC->SelectStockObject(HOLLOW_BRUSH);

	CEvImageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDoc->m_pbufImage) {
		if ((pDoc->m_pbufImage)->GetAdapter()->GetSafeHandle()) {
			if (m_bStretch) {
				CRect rect;
				GetClientRect(rect);
				(pDoc->m_pbufImage->GetAdapter())->Stretch(pDC, CPoint(0,0), rect.Size());
			}
			else
				(pDoc->m_pbufImage->GetAdapter())->Draw(pDC, CPoint(0, 0)); 
		}
	}
	else {
		///TRACE("Buffer not allocated *********\n");
	}

	if(pDoc->m_bShowOverlay) {
		for(i=0;i<m_DrwCurAry.GetSize();i++)
			DrawCursorNow(pDC,&m_DrwCurAry.GetAt(i));
		for(i=0;i<m_DrwCircleAry.GetSize();i++)
			DrawCircleNow(pDC,&m_DrwCircleAry.GetAt(i));
		for(i=0;i<m_DrwLineAry.GetSize();i++)
			DrawLineNow(pDC,&m_DrwLineAry.GetAt(i));
		for(i=0;i<m_DrwRectAry.GetSize();i++)
			DrawRectNow(pDC,&m_DrwRectAry.GetAt(i));
		for(i=0;i<m_DrwStringAry.GetSize();i++)
			DrawStringNow(pDC,&m_DrwStringAry.GetAt(i),i);
		for(i=0;i<m_DrwStrAry.GetSize();i++)
			DrawStrNow(pDC,&m_DrwStrAry.GetAt(i));
	}
	if (pDoc->m_bTrackRectangle) {
		CPen* pCurrentPen = pDC->SelectObject(&GreenPen);
		pCurrentPen->DeleteObject();

		//Check if need to drawing the polyline
		CArray<int> polylineId;
		for (i = 0; i < m_RectTrackAry.GetSize(); i++) {
			TrackInfo = m_RectTrackAry.ElementAt(i);
			//Tracker Type: 0-Rectangle, 1-Polygon, 2-Eclipse, 3-Polyline
			if (TrackInfo.nTrackerType == 1 || TrackInfo.nTrackerType >= 3)
				polylineId.Add(i);
		}

		for (i=0;i<m_RectTrackAry.GetSize();i++) {
			TrackInfo = m_RectTrackAry.ElementAt(i);
			if (i==pDoc->m_nTrackerIdx) {
				pDC->SelectObject(&TrackPen);
				if(TrackInfo.nTrackerType==1 || TrackInfo.nTrackerType >= 3)
					pDC->Ellipse(TrackInfo.rect);
				else if(TrackInfo.nTrackerType==2) {
					CRect* pRect = &TrackInfo.rect;
					pDC->MoveTo(CPoint(pRect->CenterPoint().x, pRect->top));
					pDC->LineTo(CPoint(pRect->CenterPoint().x, pRect->bottom));
					pDC->MoveTo(CPoint(pRect->left, pRect->CenterPoint().y));
					pDC->LineTo(CPoint(pRect->right, pRect->CenterPoint().y));
				}
				else if (TrackInfo.nTrackerType == 0)
					pDC->Rectangle(TrackInfo.rect);

				CRect rectTest = TrackInfo.rect;
				CRect rectTemp = m_Tracker.m_rect;
				pDC->LPtoDP(&rectTest);
				m_Tracker.m_rect = rectTest;
				m_Tracker.Draw(pDC);
				m_Tracker.m_rect = rectTemp;


				pDC->SelectObject(&GreenPen);
			}
			else {
				if (TrackInfo.nTrackerType == 1 || TrackInfo.nTrackerType >= 3) {
					pDC->SelectObject(&TrackPen);

					pDC->Ellipse(TrackInfo.rect);
					pDC->SelectObject(&GreenPen);
				}
				else if(TrackInfo.nTrackerType==2) {
					CRect* pRect = &TrackInfo.rect;
					pDC->MoveTo(CPoint(pRect->CenterPoint().x, pRect->top));
					pDC->LineTo(CPoint(pRect->CenterPoint().x, pRect->bottom));
					pDC->MoveTo(CPoint(pRect->left, pRect->CenterPoint().y));
					pDC->LineTo(CPoint(pRect->right, pRect->CenterPoint().y));
				}
				else if (TrackInfo.nTrackerType == 0)
					pDC->Rectangle(TrackInfo.rect);
			}
		}

		//Drawing the line between polyline point 
		for (i = 0; i < polylineId.GetSize() - 1; i++) {
			TrackInfo = m_RectTrackAry.ElementAt(polylineId[i]);
			CRect* pRect = &TrackInfo.rect;
			TRACK_INFO TrackInfoDest = m_RectTrackAry.ElementAt(polylineId[i + 1]);
			CRect* pRectDest = &TrackInfoDest.rect;

			pDC->MoveTo(CPoint(pRect->CenterPoint().x, pRect->CenterPoint().y));
			pDC->LineTo(CPoint(pRectDest->CenterPoint().x, pRectDest->CenterPoint().y));

			CRect rectTest = TrackInfo.rect;
			CRect rectTemp = m_Tracker.m_rect;
			pDC->LPtoDP(&rectTest);
			m_Tracker.m_rect = rectTest;
			m_Tracker.Draw(pDC);
			m_Tracker.m_rect = rectTemp;
		}

		if (polylineId.GetSize() > 2) {
			int startId = polylineId.GetSize() - 1;
			int endId = 0;
			TrackInfo = m_RectTrackAry.ElementAt(polylineId[startId]);
			CRect* pRect = &TrackInfo.rect;
			TRACK_INFO TrackInfoDest = m_RectTrackAry.ElementAt(polylineId[endId]);
			CRect* pRectDest = &TrackInfoDest.rect;
			if (TrackInfo.nTrackerType == 1) {
				pDC->MoveTo(CPoint(pRect->CenterPoint().x, pRect->CenterPoint().y));
				pDC->LineTo(CPoint(pRectDest->CenterPoint().x, pRectDest->CenterPoint().y));

				CRect rectTest = TrackInfo.rect;
				CRect rectTemp = m_Tracker.m_rect;
				pDC->LPtoDP(&rectTest);
				m_Tracker.m_rect = rectTest;
				m_Tracker.Draw(pDC);
				m_Tracker.m_rect = rectTemp;
			}
		}
	}
	CPen* pCurrentPen = pDC->SelectObject(pOldPen);
	pCurrentPen->DeleteObject();
	GreenPen.DeleteObject();
	TrackPen.DeleteObject();
	pDC->SelectObject(pOldBrush);
}

void CEvImageView::OnInitialUpdate()
{
	CEvImageZoomView::OnInitialUpdate();

	CSize sizeTotal;
	float	fZoom;

	CEvImageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->m_ImageViewInfo.pImageView = this;

	// TODO: calculate the total size of this view
	TRACE("Initialization of the VisionView class\n");

	fZoom = GetZoomLevel();
	sizeTotal = pDoc->GetDocSize();
	SetScrollSizes(MM_TEXT, sizeTotal,CSize(20,20),CSize(5,5));
	SetZoomMinMax(0.03125, 16);
	SetZoomMode(SEC_ZOOMNORMAL);
	SetZoomLevel(fZoom);

	m_Tracker.m_pView = this;

	if(m_pPanDlg)
		delete m_pPanDlg;
	m_pPanDlg = new CPanWindow(this);
	if(!m_pPanDlg->Create(IDD_PAN_DLG, this)) {
		delete m_pPanDlg;
		m_pPanDlg = NULL;
		return;
	}
	m_pPanDlg->ShowWindow(SW_HIDE);

	SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
// CEvImageView diagnostics

#ifdef _DEBUG
void CEvImageView::AssertValid() const
{
	CEvImageZoomView::AssertValid();
}

void CEvImageView::Dump(CDumpContext& dc) const
{
	CEvImageZoomView::Dump(dc);
}

CEvImageDoc* CEvImageView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEvImageDoc)));
	return (CEvImageDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEvImageView message handlers

void CEvImageView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CEvImageDoc *pDoc = GetDocument();
	CPoint pt = point;
	CRect rc;
//	int i;
	CSize ImageSize ;
	CRect rcBinSrc;
	CRect rcIntersect;
	CPoint ptBlobPoint;

	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&pt);
	m_ptPixelRulerStart = CPoint(pt.x,pt.y);
	BOOL bTracker = FALSE;

	//For drawing polyline
	CArray<int> polylineId;
	for (int i = 0; i < m_RectTrackAry.GetSize(); i++) {
		TRACK_INFO TrackInfo = m_RectTrackAry.ElementAt(i);
		//Tracker Type: 0-Rectangle, 1-Polygon, 2-Eclipse, 3-Polyline
		if (TrackInfo.nTrackerType == 1 || TrackInfo.nTrackerType >= 3)
			polylineId.Add(i);
	}
	for (int i = 0; i < polylineId.GetSize(); i++) {
		TRACK_INFO TrackInfo = m_RectTrackAry.ElementAt(polylineId[i]);
		rc = TrackInfo.rect;
		dc.LPtoDP(&rc);

		if (rc.PtInRect(point) && pDoc->m_nTrackerIdx != i) {
			pDoc->m_nTrackerIdx = i;
			m_Tracker.m_rect = rc;
			RedrawWindow();
			break;
		}
	}

	if (pDoc->m_bTrackRectangle) {
		if (m_Tracker.HitTest(point)>=0 && !pDoc->m_bLockMoveResizeRect) {
			// Start Tracking Rect
			if (m_Tracker.Track(this, point)) {
				rc = m_Tracker.m_rect;
				dc.DPtoLP(&rc);
				m_RectTrackAry.ElementAt(pDoc->m_nTrackerIdx).rect = rc;
				RedrawWindow();
			}
			bTracker = TRUE;
		}
	}

	/// Udupa; Sep'2012; Mouse based panning
	if(!bTracker && !m_bPixelRuler) {
		BOOL bHorizontal, bVertical;
		CheckScrollBars(bHorizontal, bVertical);
		if(bHorizontal || bVertical) {
			m_bPanning = TRUE;
			SetCapture();
			m_ptPanStart = point;
			m_ptStartScrollPos = GetScrollPosition();
		}
	}

	CEvImageZoomView::OnLButtonDown(nFlags, point);
}


/// Udupa; Sep'2012; Mouse based panning
void CEvImageView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bPanning) {
		ReleaseCapture();
		m_bPanning = FALSE;
	}

	CEvImageZoomView::OnLButtonUp(nFlags, point);
}

void CEvImageView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CEvImageDoc *pDoc = GetDocument();
	CClientDC dc(this);
	OnPrepareDC(&dc);	
	CRect rc;
	int i;
	
	if (!pDoc->m_bLockSelectRect && pDoc->m_bTrackRectangle) {
		for (i=0;i<m_RectTrackAry.GetSize();i++) {
			rc = m_RectTrackAry.ElementAt(i).rect;
			dc.LPtoDP(&rc);
			if (rc.PtInRect(point) && pDoc->m_nTrackerIdx != i) {
				pDoc->m_nTrackerIdx = i;
				m_Tracker.m_rect = rc;
				RedrawWindow();
				break;
			}
		}
	}
	
	CEvImageZoomView::OnRButtonDown(nFlags, point);
}


void CEvImageView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CEvImageZoomView::OnLButtonDblClk(nFlags, point);

	CMDIChildWndEx* pWnd = (CMDIChildWndEx*) GetParentFrame()->GetParentFrame();
	CEvMainFrame* pFrame = (CEvMainFrame*) AfxGetMainWnd();
	if(pWnd->IsZoomed()){
		pWnd->MDIRestore();
		pFrame->ShowHidePanes(TRUE);
		pFrame->m_bIsMaximized = FALSE;
	}
	else{
		pFrame->ShowHidePanes(FALSE);
		pFrame->m_bIsMaximized = TRUE;
		pWnd->MDIMaximize();
	}
}

BOOL CEvImageView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CRect rc;
	CEvImageDoc *pDoc = GetDocument();

	if (pWnd==this) {
		if(pDoc->m_bTrackRectangle && !pDoc->m_bLockMoveResizeRect) {
			CClientDC dc(this);
			OnPrepareDC(&dc);
			rc = m_RectTrackAry.ElementAt(pDoc->m_nTrackerIdx).rect;
			dc.LPtoDP(&rc);
			m_Tracker.m_rect = rc;
			if (m_Tracker.SetCursor(this,nHitTest)) {
				return TRUE;
			}
		}
	}
	return CEvImageZoomView::OnSetCursor(pWnd, nHitTest, message);
}

void CEvImageView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CClientDC dc(this);
	OnPrepareDC(&dc);

	LOGPEN BlueInfo;
	BlueInfo.lopnColor = RGB(0, 255, 0);
	BlueInfo.lopnStyle = PS_SOLID;
	BlueInfo.lopnWidth.x = 6;

	CPoint pt = point;
	dc.DPtoLP(&pt);
	m_ptPixelRulerEnd = CPoint(pt.x,pt.y);
	CEvImageDoc *pDoc = GetDocument();

	if(pDoc->m_pbufImage) {
		
		CString strMsg;
		BYTE nValue;
		if(pDoc->m_pbufImage->IsColor()) {
			BYTE nR, nG, nB;
			if(pDoc->m_pbufImage->GetPixelRGB(pt.x, pt.y, nR, nG, nB)==0)
				strMsg.Format("Position: [%04d, %04d]  Gray[%03d]  RGB[%03d:%03d:%03d]", pt.x, pt.y, (nR+nG+nB)/3, nR, nG, nB);
			else
				strMsg = "";
		}
		else if(pDoc->m_pbufImage->GetPixelVal(pt.x, pt.y, nValue)==0)
			strMsg.Format("Position: [%04d, %04d]  Gray[%03d]", pt.x, pt.y, nValue);
		else
			strMsg = "";
		UpdateStatusBar(&strMsg);

		CSize szImage = pDoc->m_ImageViewInfo.sizeImage;
		strMsg.Format("FOV: %d, %d", szImage.cx, szImage.cy);
		UpdateStatusBar(&strMsg, 1);
	}
	
	if(m_bPanning) {
		CPoint ptDelta = point - m_ptPanStart;
		ptDelta = m_ptStartScrollPos - ptDelta;
		ScrollToPosition(ptDelta);
	}
	if (m_bMagnifierView) {
		CClientDC dc(this);
		OnPrepareDC(&dc);
		CPoint ptDraw = CPoint(pt.x, pt.y);
		
		CPoint2D pt2DTemp;
		pt2DTemp = CPoint2D(ptDraw.x, ptDraw.y);
		DrwDel(&pDoc->m_ImageViewInfo);
		DrwEllipse(&pDoc->m_ImageViewInfo, &pt2DTemp, 20, 20, &BlueInfo);
		//DrwTrackCross(&pDoc->m_ImageViewInfo, &CRect(pt.x - 25, pt.y - 25, pt.x + 25, pt.y + 25));
		CRect rectImage = CRect(ptDraw.x - 125, ptDraw.y - 125, ptDraw.x + 125, ptDraw.y + 125);
		CheckRect(&rectImage, &pDoc->m_pbufImage->GetImgSize());

		if (rectImage.IsRectEmpty()) {
			if (IsWindow(m_MagnifierWindow.m_hWnd) && m_MagnifierWindow.IsWindowVisible()) {
				ToggleMagnifier();
				m_bMagnifierView = TRUE;
			}
			return;
		}

		if (!IsWindow(m_MagnifierWindow.m_hWnd) || !m_MagnifierWindow.IsWindowVisible()) {
			m_bMagnifierView = FALSE;
			ToggleMagnifier();
			DrwDel(&pDoc->m_ImageViewInfo);
		}

		if (rectImage.Width() > 0 && rectImage.Height() > 0) {
			if (rectImage.Width() < 250 || rectImage.Height() < 250)
				m_MagnifierWindow.m_bufMagnifier.ClrImg();
			m_MagnifierWindow.SetImage(pDoc->m_pbufImage, &rectImage);
			if (IsWindow(m_MagnifierWindow.m_hWnd)) {
				GetMagnifierDrwPos(point, &ptDraw);
				m_MagnifierWindow.MoveWindow(ptDraw.x, ptDraw.y, 250, 250, TRUE);
				m_MagnifierWindow.RedrawWindow();
			}
		}
		DrwUpdate(&pDoc->m_ImageViewInfo);
	}
	CEvImageZoomView::OnMouseMove(nFlags, point);
}

/// Udupa; Sep'2012; Mouse based zooming
BOOL CEvImageView::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);

	CPoint pt = point;
	ScreenToClient(&pt);
	dc.DPtoLP(&pt);

	if (m_bMagnifierView && IsWindow(m_MagnifierWindow.m_hWnd))
		return m_MagnifierWindow.OnMouseWheel(nFlags, zDelta, pt);

	if(zDelta > 0)
		CEvImageZoomView::ZoomIn(&pt, 2.0);
	else
		CEvImageZoomView::ZoomOut(&pt, 2.0);

	OnPrepareDC(&dc);
	dc.LPtoDP(&pt);
	ClientToScreen(&pt);
	SetCursorPos(pt.x, pt.y);

	return TRUE;
}

void CEvImageView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CEvImageDoc *pDoc = GetDocument();
	CRect rc,rcOld;
	CString strMsg;
	BOOL bRedraw=FALSE;

		//switch(nChar) {
		//	case VK_OEM_PLUS:
		//		ZoomIn();
		//		return;
		//	case VK_OEM_MINUS:
		//		ZoomOut();
		//		return;
		//	case VK_ESCAPE:
		//		ZoomFit();
		//		return;
		//}

	// For Threshold Adjustment
	if (m_pdlgThres) {
		switch(nChar) {
			case VK_UP :
				if (GetKeyState(VK_SHIFT)<0)
					m_pdlgThres->IncThresholdValue(1);	//Inc Big Step
				else
					m_pdlgThres->IncThresholdValue();	// Inc Small Step
				break;

			case VK_DOWN :
				if (GetKeyState(VK_SHIFT)<0)
					m_pdlgThres->DecThresholdValue(1);	// Dec Big Step
				else
					m_pdlgThres->DecThresholdValue();	// Dec Small Step
				break;
		}
	}
	else if (m_bMagnifierView) {
		switch (nChar) {
		case VK_UP:
			m_MagnifierWindow.ZoomIn();
			break;

		case VK_DOWN:
			m_MagnifierWindow.ZoomOut();
			break;
		}
		return;
	}
	// For Track Rect Adjustment
	else if (pDoc->m_bTrackRectangle && !pDoc->m_bLockSelectRect && !pDoc->m_bLockMoveResizeRect) {
		rcOld = rc = m_RectTrackAry.ElementAt(pDoc->m_nTrackerIdx).rect;
		switch(nChar) {
			case VK_UP :
				// Resize Fast
				if (GetKeyState(VK_CONTROL)<0 && GetKeyState(VK_SHIFT)<0) {
					//OutputDebugLogTo(0,FALSE,"UP Resize Fast");
					if (rc.Height() > 12) {
						rc.DeflateRect(0,0,0,10);
						bRedraw = TRUE;
					}
					else if (rc.Height() > 2) {
						rc.DeflateRect(0,0,0,rc.Height()-2);
						bRedraw = TRUE;
					}
					m_RectTrackAry[pDoc->m_nTrackerIdx].rect = rc;
				}
				// Resize Slow
				else if (GetKeyState(VK_CONTROL)<0 && GetKeyState(VK_SHIFT)>=0) {
					//OutputDebugLogTo(0,FALSE,"UP Resize Slow");
					if (rc.Height() > 2) {
						rc.DeflateRect(0,0,0,1);
						bRedraw = TRUE;
					}
					m_RectTrackAry[pDoc->m_nTrackerIdx].rect = rc;
				}
				// Move Fast
				else if (GetKeyState(VK_CONTROL)>=0 && GetKeyState(VK_SHIFT)<0) {
					//OutputDebugLogTo(0,FALSE,"UP Move Fast");
					if (rc.top >=10) {
						rc += CPoint(0,-10);
						bRedraw = TRUE;
					}
					else {
						rc += CPoint(0,-rc.top);
						bRedraw = TRUE;
					}
					m_RectTrackAry[pDoc->m_nTrackerIdx].rect = rc;
				}
				// Move Slow
				else {
					//OutputDebugLogTo(0,FALSE,"UP Move Slow");
					if (rc.top >=1) {
						rc += CPoint(0,-1);
						bRedraw = TRUE;
					}
					m_RectTrackAry[pDoc->m_nTrackerIdx].rect = rc;
				}
				break;

			case VK_DOWN :
				// Resize Fast
				if (GetKeyState(VK_CONTROL)<0 && GetKeyState(VK_SHIFT)<0) {
					//OutputDebugLogTo(0,FALSE,"DOWN Resize Fast");
					if (rc.bottom <= pDoc->m_ImageViewInfo.sizeImage.cy-1-10) {
						rc.InflateRect(0,0,0,10);
						bRedraw = TRUE;
					}
					else if (rc.bottom < pDoc->m_ImageViewInfo.sizeImage.cy-1) {
						rc.InflateRect(0,0,0,pDoc->m_ImageViewInfo.sizeImage.cy-1-rc.bottom);
						bRedraw = TRUE;
					}
					m_RectTrackAry[pDoc->m_nTrackerIdx].rect = rc;
				}
				// Resize Slow
				else if (GetKeyState(VK_CONTROL)<0 && GetKeyState(VK_SHIFT)>=0) {
					//OutputDebugLogTo(0,FALSE,"DOWN Resize Slow");
					if (rc.bottom < pDoc->m_ImageViewInfo.sizeImage.cy-1) {
						rc.InflateRect(0,0,0,1);
						bRedraw = TRUE;
					}
					m_RectTrackAry[pDoc->m_nTrackerIdx].rect = rc;
				}
				// Move Fast
				else if (GetKeyState(VK_CONTROL)>=0 && GetKeyState(VK_SHIFT)<0) {
					//OutputDebugLogTo(0,FALSE,"DOWN Move Fast");
					if (rc.bottom <= pDoc->m_ImageViewInfo.sizeImage.cy-1-10) {
						rc += CPoint(0,10);
						bRedraw = TRUE;
					}
					else {
						rc += CPoint(0,pDoc->m_ImageViewInfo.sizeImage.cy-1-rc.bottom);
						bRedraw = TRUE;
					}
					m_RectTrackAry[pDoc->m_nTrackerIdx].rect = rc;
				}
				// Move Slow
				else {
					//OutputDebugLogTo(0,FALSE,"DOWN Move Slow");
					if (rc.bottom <= pDoc->m_ImageViewInfo.sizeImage.cy-1) {
						rc += CPoint(0,1);
						bRedraw = TRUE;
					}
					else {
						rc += CPoint(0,pDoc->m_ImageViewInfo.sizeImage.cy-rc.bottom);
						bRedraw = TRUE;
					}
					m_RectTrackAry[pDoc->m_nTrackerIdx].rect = rc;
				}
				break;

			case VK_LEFT :
				// Resize Fast
				if (GetKeyState(VK_CONTROL)<0 && GetKeyState(VK_SHIFT)<0) {
					//OutputDebugLogTo(0,FALSE,"LEFT Resize Fast");
					if (rc.Width() > 12) {
						rc.DeflateRect(0,0,10,0);
						bRedraw = TRUE;
					}
					else if (rc.Width() > 2) {
						rc.DeflateRect(0,0,rc.Width()-2,0);
						bRedraw = TRUE;
					}
					m_RectTrackAry[pDoc->m_nTrackerIdx].rect =  rc;
				}
				// Resize Slow
				else if (GetKeyState(VK_CONTROL)<0 && GetKeyState(VK_SHIFT)>=0) {
					//OutputDebugLogTo(0,FALSE,"LFET Resize Slow");
					if (rc.Width() > 2) {
						rc.DeflateRect(0,0,1,0);
						bRedraw = TRUE;
					}
					m_RectTrackAry[pDoc->m_nTrackerIdx].rect = rc;
				}
				// Move Fast
				else if (GetKeyState(VK_CONTROL)>=0 && GetKeyState(VK_SHIFT)<0) {
					//OutputDebugLogTo(0,FALSE,"LEFT Move Fast");
					if (rc.left >=10) {
						rc += CPoint(-10,0);
						bRedraw = TRUE;
					}
					else {
						rc += CPoint(-rc.left,0);
						bRedraw = TRUE;
					}
					m_RectTrackAry[pDoc->m_nTrackerIdx].rect = rc;
				}
				// Move Slow
				else {
					//OutputDebugLogTo(0,FALSE,"LEFT Move Slow");
					if (rc.left >=1) {
						rc += CPoint(-1,0);
						bRedraw = TRUE;
					}
					m_RectTrackAry[pDoc->m_nTrackerIdx].rect = rc;
				}
				break;

			case VK_RIGHT :
				// Resize Fast
				if (GetKeyState(VK_CONTROL)<0 && GetKeyState(VK_SHIFT)<0) {
					//OutputDebugLogTo(0,FALSE,"RIGHT Resize Fast");
					if (rc.right <= pDoc->m_ImageViewInfo.sizeImage.cx-1-10) {
						rc.InflateRect(0,0,10,0);
						bRedraw = TRUE;
					}
					else if (rc.right < pDoc->m_ImageViewInfo.sizeImage.cx-1) {
						rc.InflateRect(0,0,pDoc->m_ImageViewInfo.sizeImage.cx-1-rc.right,0);
						bRedraw = TRUE;
					}
					m_RectTrackAry[pDoc->m_nTrackerIdx].rect = rc;
				}
				// Resize Slow
				else if (GetKeyState(VK_CONTROL)<0 && GetKeyState(VK_SHIFT)>=0) {
					//OutputDebugLogTo(0,FALSE,"RIGHT Resize Slow");
					if (rc.right < pDoc->m_ImageViewInfo.sizeImage.cx-1) {
						rc.InflateRect(0,0,1,0);
						bRedraw = TRUE;
					}
					m_RectTrackAry[pDoc->m_nTrackerIdx].rect = rc;
				}
				// Move Fast
				else if (GetKeyState(VK_CONTROL)>=0 && GetKeyState(VK_SHIFT)<0) {
					//OutputDebugLogTo(0,FALSE,"DOWN Move Fast");
					if (rc.right <= pDoc->m_ImageViewInfo.sizeImage.cx-1-10) {
						rc += CPoint(10,0);
						bRedraw = TRUE;
					}
					else {
						rc += CPoint(pDoc->m_ImageViewInfo.sizeImage.cx-1-rc.right,0);
						bRedraw = TRUE;
					}
					m_RectTrackAry[pDoc->m_nTrackerIdx].rect = rc;
				}
				// Move Slow
				else {
					//OutputDebugLogTo(0,FALSE,"RIGHT Move Slow");
					if (rc.right <= pDoc->m_ImageViewInfo.sizeImage.cx-1) {
						rc += CPoint(1,0);
						bRedraw = TRUE;
					}
					m_RectTrackAry[pDoc->m_nTrackerIdx].rect = rc;
				}
				break;
		}

	}

	CEvImageZoomView::OnKeyDown(nChar, nRepCnt, nFlags);
	if (bRedraw) {
		UpdateTrackRectPos(rc,FALSE);
		RedrawWindow();
	}
}

LRESULT CEvImageView::BinaryMode(WPARAM wParam, LPARAM lParam)
{
	BOOL bMode;
	bMode = (BOOL)wParam;
	struct Threshold {
		int nLow;
		int nHigh;
	}*pThreshold;
	pThreshold = (Threshold*)lParam;

	if (bMode&&pThreshold == NULL)
		return 0;

	CEvImageDoc* pDoc = GetDocument();
	if(!pDoc->m_pbufImage)
		return 0;


	if (bMode) {
		pDoc->m_bBinaryMode = TRUE;
		pDoc->m_nThreshold.nLow = pThreshold->nLow;
		pDoc->m_nThreshold.nHigh = pThreshold->nHigh;
	}
	else
		pDoc->m_bBinaryMode = FALSE;

	
	pDoc->BinaryMode(pDoc->m_bBinaryMode);
	if (pDoc->m_bBinaryMode) {
		if (!m_pdlgThres) {
			m_pdlgThres = new CThresholdDialog();
			m_pdlgThres->m_nThresholdValue.nLow = pDoc->m_nThreshold.nLow;
			m_pdlgThres->m_nThresholdValue.nHigh = pDoc->m_nThreshold.nHigh;
			m_pdlgThres->CreateNew(this);
			m_pdlgThres->ShowWindow(SW_SHOW);
		}
		pDoc->m_pbufImage->GetAdapter()->ChangeToBinaryPalette((WPARAM)&pDoc->m_nThreshold);
	}
	else {
		if(m_pdlgThres) {
			m_pdlgThres->DestroyWindow();
			delete m_pdlgThres;
			m_pdlgThres = NULL;
		}
		pDoc->m_pbufImage->GetAdapter()->ChangeToGrayPalette();
	}
	GetParent()->GetParent()->GetParent()->SendMessage(WM_SET_BINARYMODE);
	
	RedrawWindow();
	//SetActiveWindow();
	//SetFocus();	
	return 0;
}

LRESULT CEvImageView::SetGrayPallette(WPARAM wParam, LPARAM lParam)
{
	CEvImageDoc* pDoc = GetDocument();
	if(!pDoc || !pDoc->m_pbufImage || !pDoc->m_pbufImage->GetAdapter())
		return 0;

	if(pDoc->m_bBinaryMode)
		pDoc->m_pbufImage->GetAdapter()->ChangeToBinaryPalette((WPARAM)&pDoc->m_nThreshold);
	else
		pDoc->m_pbufImage->GetAdapter()->ChangeToGrayPalette();

	return 0;
}

LRESULT CEvImageView::ZoomIn(WPARAM wParam, LPARAM lParam) 
{
	CEvImageZoomView::ZoomIn(NULL,2.0);
	return 0;
}

LRESULT CEvImageView::ZoomInRect(CRect rect)
{
//	CRect rczoom;
//	rczoom = CRect(800,100,1000,500);
	CEvImageZoomView::ZoomIn(rect);
	return 0;
}

LRESULT CEvImageView::ZoomOut(WPARAM wParam, LPARAM lParam) 
{
	CEvImageZoomView::ZoomOut(NULL, 2.0);
	return 0;
}

LRESULT CEvImageView::ZoomFit(WPARAM wParam, LPARAM lParam) 
{
	CEvImageZoomView::ZoomFit();
	return 0;
}

LRESULT CEvImageView::OnSetThreshold(WPARAM wParam, LPARAM lParam)
{
	CEvImageDoc *pDoc = GetDocument();
	struct ThresholdValue {
		int nLow;
		int nHigh;
	};

	if(!pDoc->m_pbufImage)
		return 0;
	ThresholdValue* pThresholdValue = (ThresholdValue*)wParam;
	pDoc->m_nThreshold.nLow = pThresholdValue->nLow;
	pDoc->m_nThreshold.nHigh = pThresholdValue->nHigh;
	pDoc->m_pbufImage->GetAdapter()->ChangeToBinaryPalette((WPARAM)&pDoc->m_nThreshold);
	RedrawWindow();
	return 0;
}

LRESULT CEvImageView::ViewDrwDel(WPARAM wParam, LPARAM lParam)
{
	CEvImageDoc *pDoc = GetDocument();

	m_DrwCurAry.RemoveAll();
	m_DrwRectAry.RemoveAll();
	m_DrwCircleAry.RemoveAll();
	m_DrwLineAry.RemoveAll();
	m_RectTrackAry.RemoveAll();
	m_DrwStringAry.RemoveAll();
	m_DrwStrAry.RemoveAll();
	if (pDoc) {
		pDoc->m_nTrackerIdx = 0;
		pDoc->m_bTrackRectangle = FALSE;
		pDoc->m_bLockAddDelRect = FALSE;
		pDoc->m_bLockMoveResizeRect = FALSE;
		pDoc->m_bLockSelectRect = FALSE;
	}

	return 0;
}

LRESULT CEvImageView::ViewDrwUpd(WPARAM wParam, LPARAM lParam)
{
	RedrawWindow();
	return 0;
}

LRESULT CEvImageView::ViewUpdImg(WPARAM wParam, LPARAM lParam)
{
	CEvImageDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CPerformanceTimer DispTimer1,DispTimer2;
	double ElapsedTime;
	BYTE *pDst;

	CImageBuffer *Img = (CImageBuffer*)wParam;
	
	ElapsedTime=0;
	pDst = 0;

	
	if (pDoc->m_lDebugFlag&DEBUG_TIME_EXT)
		DispTimer1.Reset();

	if (pDoc->m_lDebugFlag&DEBUG_TIME_EXT) {
		ElapsedTime  = 	DispTimer1.GetElapsedMSec();
		OutputDebugLog("Read Time:=%3.4fms",ElapsedTime);
		DispTimer2.Reset();
	}

	pDoc->InitViewBuffer(Img);
	//Update View
	RedrawWindow();
	
	if (pDoc->m_lDebugFlag&DEBUG_TIME_EXT) {
		ElapsedTime = DispTimer2.GetElapsedMSec();
		OutputDebugLog("Refresh Time:=%3.4fms",ElapsedTime);
		ElapsedTime = DispTimer1.GetElapsedMSec();
		OutputDebugLog("Total Display Time:=%3.4fms",ElapsedTime);
	}
	return 0;
}

LRESULT CEvImageView::ViewRestorePrevImg(WPARAM wParam, LPARAM lParam)
{
	CEvImageDoc *pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->RestorePreviousViewBuffer();

	return 0;
}


LRESULT CEvImageView::ViewDrwCur(WPARAM wParam, LPARAM lParam)
{
	CUR_INFO *pCI;

	pCI = (CUR_INFO*)wParam;
	m_DrwCurAry.Add(*pCI);
	return 0;
}

LRESULT CEvImageView::ViewCurDel(WPARAM wParam, LPARAM lParam)
{
	m_DrwCurAry.RemoveAll();
	return 0;
}

LRESULT CEvImageView::ViewDrwLine(WPARAM wParam, LPARAM lParam)
{
	LINE_INFO *pLI;

	pLI = (LINE_INFO*)wParam;
	m_DrwLineAry.Add(*pLI);
	return 0;
}


LRESULT CEvImageView::ViewLineDel(WPARAM wParam, LPARAM lParam)
{
	int nNoOfLines = *((int*) wParam);
	if(nNoOfLines==-1)
		m_DrwLineAry.RemoveAll();
	else {
		int nCount = (int) m_DrwLineAry.GetSize();
		nNoOfLines = __max(0, nNoOfLines);
		nNoOfLines = __min(nNoOfLines, nCount);
		for(int i=0; i<nNoOfLines; i++)
			m_DrwLineAry.RemoveAt(nCount-1-i);
	}
	return 0;
}

LRESULT CEvImageView::ViewDrwString(WPARAM wParam, LPARAM lParam)
{
	STR_INFO *pSI;

	pSI = (STR_INFO*)wParam;
	m_DrwStringAry.Add(*pSI);
	return 0;
}

LRESULT CEvImageView::ViewDrwStr(WPARAM wParam, LPARAM lParam)
{
	STR_ATTR *pSA;

	pSA = (STR_ATTR*)wParam;
	m_DrwStrAry.Add(*pSA);
	return 0;
}

LRESULT CEvImageView::ViewDeleteString(WPARAM wParam, LPARAM lParam)
{
	m_DrwStrAry.RemoveAll();
	return 0;
}

LRESULT CEvImageView::ViewDelRect(WPARAM wParam, LPARAM lParam)
{
	m_DrwRectAry.RemoveAll();
	return 0;
}

LRESULT CEvImageView::ViewDelCircle(WPARAM wParam, LPARAM lParam)
{
	m_DrwCircleAry.RemoveAll();
	return 0;
}

LRESULT CEvImageView::ViewDrwRect(WPARAM wParam, LPARAM lParam)
{
	RECT_INFO *pRI;

	pRI = (RECT_INFO*)wParam;
	m_DrwRectAry.Add(*pRI);

	return 0;
}

LRESULT CEvImageView::ViewDrwCircle(WPARAM wParam, LPARAM lParam)
{
	CIRCLE_INFO *pRI;

	pRI = (CIRCLE_INFO*)wParam;
	m_DrwCircleAry.Add(*pRI);

	return 0;
}

LRESULT CEvImageView::ViewDrwTrackRect(WPARAM wParam, LPARAM lParam)
{
	CRect *pRect;
	CEvImageDoc *pDoc = GetDocument();


	TRACK_INFO Tracker;
	


	pRect = (CRect*)wParam;

	Tracker.rect = pRect;			
	Tracker.nTrackerType = (int) lParam;

	m_RectTrackAry.Add(Tracker);

	pDoc->m_bTrackRectangle = TRUE;
	pDoc->m_nTrackerIdx = (int) (m_RectTrackAry.GetSize() - 1);

	return 0;
}

LRESULT CEvImageView::ViewDrwTrackCross(WPARAM wParam, LPARAM lParam)
{
	CRect *pRect;
	CEvImageDoc *pDoc = GetDocument();
	TRACK_INFO Tracker;

	pRect = (CRect*)wParam;

	Tracker.rect = pRect;			
	Tracker.nTrackerType = 2;
	m_Tracker.m_nHandleSize = 0;

	m_RectTrackAry.Add(Tracker);

	pDoc->m_bTrackRectangle = TRUE;
	pDoc->m_nTrackerIdx = (int) (m_RectTrackAry.GetSize() - 1);

	return 0;
}

void CEvImageView::DrawCursorNow(CDC* pDC,CUR_INFO *pCurInfo)
{
	CPoint pt1,pt2;
	// Check to see if the current Pen has the same setting as pPenInfo
	LOGPEN CurrentPenInfo;
	CPen *pCurrentPen, NewPen;
	pCurrentPen = pDC->GetCurrentPen();
	pCurrentPen->GetLogPen(&CurrentPenInfo);
	if (CurrentPenInfo.lopnColor != pCurInfo->PenInfo.lopnColor ||
		CurrentPenInfo.lopnStyle != pCurInfo->PenInfo.lopnStyle ||
		CurrentPenInfo.lopnWidth.x != pCurInfo->PenInfo.lopnWidth.x) {

		NewPen.CreatePen(pCurInfo->PenInfo.lopnStyle,
						 pCurInfo->PenInfo.lopnWidth.x,
						 pCurInfo->PenInfo.lopnColor);
		pCurrentPen = pDC->SelectObject(&NewPen);
		pCurrentPen->DeleteObject();
	}

	pt1.x = pCurInfo->ptCursor.x - pCurInfo->nExt;
	pt1.y = pCurInfo->ptCursor.y - pCurInfo->nExt;
	pt2.x = pCurInfo->ptCursor.x + pCurInfo->nExt;
	pt2.y = pCurInfo->ptCursor.y + pCurInfo->nExt;
	pDC->MoveTo(pt1);
	pDC->LineTo(pt2);
	
	pt1.x = pCurInfo->ptCursor.x + pCurInfo->nExt;
	pt1.y = pCurInfo->ptCursor.y - pCurInfo->nExt;
	pt2.x = pCurInfo->ptCursor.x - pCurInfo->nExt;
	pt2.y = pCurInfo->ptCursor.y + pCurInfo->nExt;
	pDC->MoveTo(pt1);
	pDC->LineTo(pt2);

}

void CEvImageView::DrawCircleNow(CDC* pDC,CIRCLE_INFO *pRectInfo)
{
	// Check to see if the current Pen has the same setting as pPenInfo
	LOGPEN CurrentPenInfo;
	CPen *pCurrentPen, NewPen;
	pCurrentPen = pDC->GetCurrentPen();
	pCurrentPen->GetLogPen(&CurrentPenInfo);
	if (CurrentPenInfo.lopnColor != pRectInfo->PenInfo.lopnColor ||
		CurrentPenInfo.lopnStyle != pRectInfo->PenInfo.lopnStyle ||
		CurrentPenInfo.lopnWidth.x != pRectInfo->PenInfo.lopnWidth.x) {

		NewPen.CreatePen(pRectInfo->PenInfo.lopnStyle,
						 pRectInfo->PenInfo.lopnWidth.x,
						 pRectInfo->PenInfo.lopnColor);
		pCurrentPen = pDC->SelectObject(&NewPen);
		pCurrentPen->DeleteObject();
	}

	pDC->Ellipse(pRectInfo->rect);

}

void CEvImageView::DrawStringNow(CDC* pDC,STR_INFO *pStringInfo,int index)
{


	long ImageWidth, ImageHeight;
	int Xmax, Ymax;
	CString text[6];
	CDC *hDC=NULL;

	CEvImageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	Xmax=ImageWidth=pDoc->m_ImageViewInfo.sizeImage.cx;
	Ymax=ImageHeight=pDoc->m_ImageViewInfo.sizeImage.cy-72;
	 
		pDC->SetBkMode(OPAQUE);
		pDC->SetBkColor(pStringInfo->BkColor.lopnColor);//RGB(0,0,0));//blacke
		pDC->SetTextColor(pStringInfo->TextColor.lopnColor);//RGB(255, 255, 255)); //White
		pDC->TextOut(pStringInfo->x,pStringInfo->y, pStringInfo->str,pStringInfo->str.GetLength());

}

void CEvImageView::UpdateStatusBar(CString* pstrMessage, int nPane)
{
	AfxGetMainWnd()->SendMessage(WM_UPD_STATUSBAR, (WPARAM) nPane, (LPARAM) pstrMessage);
}

void CEvImageView::UpdateTrackRectPos(CRect &rect,BOOL bConvert)
{
	CRect rc;
	CString strMsg;
	CEvImageDoc* pDoc = GetDocument();

	TRACK_INFO TrackInfo;

	rc = rect;
	if (bConvert) {
		CClientDC dc(this);
		OnPrepareDC(&dc);
		dc.DPtoLP(&rc);
	}
	m_RectTrackAry.ElementAt(pDoc->m_nTrackerIdx).rect = rc;

	RedrawWindow();
	strMsg.Format(" Position: [%04d %04d]  Size: [%04d %04d]",rc.left,
												 rc.top,
												 rc.Width(),
												 rc.Height());
	UpdateStatusBar(&strMsg);
}
void CEvImageView::GetImageRect(CRect &rectDevice)
{
	CClientDC   dc(this);	
				
	OnPrepareDC(&dc);
	dc.DPtoLP(&rectDevice);
}

void CEvImageView::GetDeviceRect(CRect &rectImage)
{
	CClientDC   dc(this);	
				
	OnPrepareDC(&dc);
	dc.LPtoDP(&rectImage);
}

CSize CEvImageView::GetImageSize()
{
	CEvImageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	return pDoc->GetDocSize();
}

BOOL CEvImageView::OnEraseBkgnd(CDC* pDC) 
{
	CEvImageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// return CEvImageZoomView::OnEraseBkgnd(pDC);

    if (m_bStretch)
		return TRUE;

    CRect ClientRect, ImageRect;
	float	fLevel;

    GetClientRect(ClientRect);

	fLevel = GetZoomLevel();
	if(pDoc->m_pbufImage && pDoc->m_pbufImage->GetAdapter()->GetSafeHandle())
		ImageRect.SetRect(0,0, (int)(pDoc->m_pbufImage->GetImgSize().cx*fLevel), (int)(pDoc->m_pbufImage->GetImgSize().cy*fLevel));
	else
		ImageRect.SetRectEmpty();

    //HBRUSH hBrush = ::CreateSolidBrush(RGB(255,255,255));
    HBRUSH hBrush = ::CreateSolidBrush(RGB(50,50,50));

    ::FillRect(pDC->GetSafeHdc(), 
               CRect(ImageRect.right,0, ClientRect.right, ImageRect.bottom), hBrush);
    ::FillRect(pDC->GetSafeHdc(), 
               CRect(0, ImageRect.bottom, ClientRect.right, ClientRect.bottom), hBrush);

    ::DeleteObject(hBrush);

    return TRUE;
}

void CEvImageView::OnNcPaint()
{
	CEvImageZoomView::OnNcPaint();

	CDC* pDC = GetWindowDC();

	CBitmap bmp;
	if(bmp.LoadBitmap(IDB_PAN_BITMAP)) {
		CDC dcMemory;
		dcMemory.CreateCompatibleDC(pDC);
		CBitmap* pOldBitmap = dcMemory.SelectObject(&bmp);

		BITMAP bmpInfo;
		bmp.GetBitmap(&bmpInfo);

		CRect rect;
		GetClientRect(&rect);
		if(m_bFramed)
			rect.OffsetRect(3, 3);

		BOOL bSuccess = pDC->StretchBlt(rect.right, rect.bottom, 
			GetSystemMetrics(SM_CXVSCROLL)+1, GetSystemMetrics(SM_CYHSCROLL)+1, &dcMemory, 
			0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY);
		dcMemory.SelectObject(pOldBitmap);
		dcMemory.DeleteDC();
	}
	ReleaseDC(pDC);
}

void CEvImageView::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	CEvImageZoomView::OnNcMouseMove(nHitTest, point);

	CRect rect;
	GetClientRect(&rect);
	rect = CRect(rect.BottomRight(), CSize(GetSystemMetrics(SM_CXVSCROLL), 
									GetSystemMetrics(SM_CYHSCROLL)));
	ScreenToClient(&point);
	if(rect.PtInRect(point))
		SetCursor(LoadCursor(NULL, IDC_SIZEALL));
}

void CEvImageView::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	CRect rect;
	GetClientRect(&rect);
	rect = CRect(rect.BottomRight(), CSize(GetSystemMetrics(SM_CXVSCROLL), 
									GetSystemMetrics(SM_CYHSCROLL)));
	ScreenToClient(&point);
	if(rect.PtInRect(point)) {
		SetCursor(LoadCursor(NULL, IDC_SIZEALL));
		m_pPanDlg->ShowWindow(SW_SHOW);
		m_pPanDlg->OnLButtonDown(0, point);
		return;
	}

	CEvImageZoomView::OnNcLButtonDown(nHitTest, point);
}


CVisRectTracker::CVisRectTracker(CEvImageView* pView)
{
	m_pView = pView;
	m_sizeImage.cx = 0;
	m_sizeImage.cy = 0;
}

CVisRectTracker::~CVisRectTracker()
{

}
LRESULT CEvImageView::ViewSetScrollSize(WPARAM wParam, LPARAM lParam)
{
	SetScrollSizes(MM_TEXT, CSize((int) wParam, (int) lParam), CSize(20,20), CSize(5,5));

	return 0;
}

void CEvImageView::FitToWindow()
{
	ZoomFit();
}


void CEvImageView::RefreshMagnifier()
{
	if (m_bMagnifierView && IsWindow(m_MagnifierWindow.m_hWnd) && m_MagnifierWindow.IsWindowVisible()) {
		CEvImageDoc* pDoc = GetDocument();
		m_MagnifierWindow.SetImage(pDoc->m_pbufImage, NULL);
		m_MagnifierWindow.RedrawWindow();
	}
	
}

LRESULT CEvImageView::ToggleMagnifier(WPARAM wParam, LPARAM lParam)
{
	if (m_bMagnifierView) {
		if (IsWindow(m_MagnifierWindow.m_hWnd))
			m_MagnifierWindow.ShowWindow(SW_HIDE);
		//SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	else {
		m_MagnifierWindow.m_nZoomValue = (int)((250.0 - 32.0 / m_fZoomScale) / 2.0);
		if (m_MagnifierWindow.m_nZoomValue < 20)
			m_MagnifierWindow.m_nZoomValue = 20;
		if (m_MagnifierWindow.m_nZoomValue > 120)
			m_MagnifierWindow.m_nZoomValue = 120;
		if (!IsWindow(m_MagnifierWindow.m_hWnd))
			m_MagnifierWindow.Create(250, this);
		//SetCursor(m_hMagnifierCursor);
		m_MagnifierWindow.ShowWindow(SW_SHOW);

		TRACKMOUSEEVENT mouseEvent;
		mouseEvent.cbSize = sizeof(mouseEvent);
		mouseEvent.hwndTrack = m_hWnd;
		mouseEvent.dwFlags = TME_LEAVE;
		TrackMouseEvent(&mouseEvent);
	}
	SetFocus();
	m_bMagnifierView = !m_bMagnifierView;

	return 0;
}

void CEvImageView::GetMagnifierDrwPos(CPoint ptMouse, CPoint *ptDraw)
{
	CRect rcClient, rcWnd;
	GetClientRect(&rcClient);
	GetWindowRect(&rcWnd);

	if (ptMouse.x > (rcClient.Width() * 2 / 3))
		m_ptMagnifierOffset.x = -250;
	else if (ptMouse.x < (rcClient.Width() / 3))
		m_ptMagnifierOffset.x = 10;

	if (ptMouse.y >(rcClient.Height() * 2 / 3))
		m_ptMagnifierOffset.y = -250;
	else if (ptMouse.y < (rcClient.Height() / 3))
		m_ptMagnifierOffset.y = 10;

	ptDraw->x = ptMouse.x + m_ptMagnifierOffset.x + rcWnd.left - rcClient.left;
	ptDraw->y = ptMouse.y + m_ptMagnifierOffset.y + rcWnd.top - rcClient.top;
}

LRESULT CEvImageView::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	CEvImageDoc *pDoc = GetDocument();
	if (m_bMagnifierView) {
		ToggleMagnifier();
		m_bMagnifierView = TRUE;
		DrwDel(&pDoc->m_ImageViewInfo);
		DrwUpdate(&pDoc->m_ImageViewInfo);
	}

	return 0;
}

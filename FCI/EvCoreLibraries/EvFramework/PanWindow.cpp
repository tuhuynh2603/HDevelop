/// PanWindow.cpp : implementation of the CPanWindow class
/// Udupa

#include "stdafx.h"
#include "PanWindow.h"
#include "EvImageView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPanWindow::CPanWindow(CWnd* pParent /*=NULL*/)
	: CDialog(CPanWindow::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPanWindow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pParent = (CEvImageView*) pParent;
	m_bIsPanning = false;
	m_brush.CreateStockObject(NULL_BRUSH);
}

CPanWindow::~CPanWindow()
{
	m_brush.DeleteObject();
}

void CPanWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPanWindow)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPanWindow, CDialog)
	//{{AFX_MSG_MAP(CPanWindow)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPanWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if(m_bIsPanning) {
		CSize sizeCanvas = m_pParent->GetDocument()->GetDocSize();

		CImageBuffer* pBuf = m_pParent->GetDocument()->m_pbufImage;
		if (pBuf) {
			if(pBuf->GetAdapter()->GetSafeHandle()) {
				CRect rect;
				GetClientRect(rect);
				pBuf->GetAdapter()->Stretch(&dc, CPoint(0,0), rect.Size());
			}
		}
		CPen pen(PS_SOLID, 2, RGB(255,0,255));
		CPen* pOldPen = dc.SelectObject(&pen);
		CBrush* pOldBrush = dc.SelectObject(&m_brush);
		dc.Rectangle(&m_rectFocus);
		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);
	}
}

void CPanWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	m_bIsPanning = true;

	CRect rectParent;
	CSize sizeCanvas;

	m_pParent->GetClientRect(&rectParent);
	sizeCanvas = m_pParent->GetDocument()->GetDocSize();
	double newWidth = __max(double(rectParent.Width())/5, 120);
	double newHeight = newWidth * double(sizeCanvas.cy) / double(sizeCanvas.cx);
	SetWindowPos(NULL, 0, 0, int(newWidth), int(newHeight), SWP_HIDEWINDOW|SWP_NOMOVE);

	CRect rectPan;
	GetWindowRect(&rectPan);
	CPoint ptPanDlgPos = rectParent.BottomRight();
	m_pParent->ClientToScreen(&ptPanDlgPos);
	ptPanDlgPos.x -= (rectPan.Width() - GetSystemMetrics(SM_CXVSCROLL) - 2);
	ptPanDlgPos.y -= (rectPan.Height() - GetSystemMetrics(SM_CYHSCROLL) - 2);
	SetWindowPos(NULL, ptPanDlgPos.x, ptPanDlgPos.y, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);

	GetClientRect(&rectPan);
	m_dScaleShift = double(sizeCanvas.cx) / rectPan.Width() * m_pParent->GetZoomLevel();

	int nDisplacementX = (int)(m_pParent->GetScrollPosition().x / m_dScaleShift);
	int nDisplacementY = (int)(m_pParent->GetScrollPosition().y / m_dScaleShift);
	SetCursorPos(ptPanDlgPos.x + GetSystemMetrics(SM_CXFIXEDFRAME) + nDisplacementX,
					ptPanDlgPos.y + GetSystemMetrics(SM_CYFIXEDFRAME) + nDisplacementY);

	m_rectFocus = CRect(CPoint(nDisplacementX, nDisplacementY), 
		CSize(int(rectParent.Width() / m_dScaleShift),
		int(rectParent.Height() / m_dScaleShift)));
	
	Invalidate();

	CDialog::OnLButtonDown(nFlags, point);
}

void CPanWindow::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bIsPanning) {
		m_bIsPanning = FALSE;
		ReleaseCapture();
		ShowWindow(SW_HIDE);

		CRect rectParent;
		m_pParent->GetClientRect(&rectParent);
		CPoint pt;
		pt.x = rectParent.Width() + GetSystemMetrics(SM_CXVSCROLL)/2;
		pt.y = rectParent.Height() + GetSystemMetrics(SM_CYHSCROLL)/2;
		m_pParent->ClientToScreen(&pt);
		SetCursorPos(pt.x, pt.y);
	}
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CPanWindow::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_bIsPanning) {
		CRect rectClient;
		GetClientRect(&rectClient);
		CPoint ptCursorPos;
		
		if(point.x <= 0)
			ptCursorPos.x = 0;
		else if(point.x >= (rectClient.right - m_rectFocus.Width()))
			ptCursorPos.x = rectClient.right - m_rectFocus.Width();
		else
			ptCursorPos.x = point.x + 1;
		
		if(point.y <= 0)
			ptCursorPos.y = 0;
		else if(point.y >= (rectClient.bottom - m_rectFocus.Height()))
			ptCursorPos.y = rectClient.bottom - m_rectFocus.Height();
		else
			ptCursorPos.y = point.y + 1;

		m_pParent->ScrollToPosition(CPoint(int(ptCursorPos.x * m_dScaleShift), 
								int(ptCursorPos.y * m_dScaleShift)));
		
		m_rectFocus += (ptCursorPos - m_rectFocus.TopLeft());
		Invalidate();
	}

	CDialog::OnMouseMove(nFlags, point);
}

HBRUSH CPanWindow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	return m_brush;
}

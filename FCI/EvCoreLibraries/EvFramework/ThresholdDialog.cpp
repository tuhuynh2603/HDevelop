// ThresholdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MsgCode.h"
#include "ThresholdDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThresholdDialog dialog

const UINT RANGE_CHANGED =
::RegisterWindowMessage(_T("RANGE_CHANGED{2E7439A4-12DB-42a2-BB95-3462B3EC39CE}CRangeSlider"));

CThresholdDialog::CThresholdDialog(CWnd* pParent /*=NULL*/)
	//: CWnd(CThresholdDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CThresholdDialog)
	//}}AFX_DATA_INIT


	//m_BrushActive.CreateSolidBrush(RGB(130, 160, 210));
	m_BrushActive.CreateSolidBrush(RGB(0, 0, 0));
	m_dMin = 0;
	m_dMax = 255;
	m_bTracking = FALSE;
}

CThresholdDialog::~CThresholdDialog()
{
	m_BrushActive.DeleteObject();
}

void CThresholdDialog::CreateNew(CView* pView)
{
	m_pView = pView;
	Create(NULL, "", WS_CHILD | WS_VISIBLE, CRect(0, 0, 210, 25), 
		pView->GetParent()->GetParent()->GetParent(), IDD_THRESHOLD_DLG);
	



	char str[4];
	sprintf_s(str, "%d", m_nThresholdValue.nLow);
	m_editBinarizeLow.Create(WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE, CRect(0, 2, 30, 23), this, IDC_THRESHOLD_VALUE);
	m_editBinarizeLow.SetWindowTextA(str);
	m_dLeft = m_nThresholdValue.nLow;

	sprintf(str, "%d", m_nThresholdValue.nHigh);
	m_editBinarizeHigh.Create(WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE, CRect(180, 2, 210, 23), this, IDC_THRESHOLD_VALUE_HIGH);
	m_editBinarizeHigh.SetWindowTextA(str);
	m_dRight = m_nThresholdValue.nHigh;

	//m_editBinarizeLow.SetFont(&m_font);
}

void CThresholdDialog::DoDataExchange(CDataExchange* pDX)
{
	CWnd::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CThresholdDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CThresholdDialog, CWnd)
	//{{AFX_MSG_MAP(CThresholdDialog)
//	ON_WM_HSCROLL()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
//	ON_WM_PAINT()
//ON_WM_PAINT()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_REGISTERED_MESSAGE(RANGE_CHANGED, OnRangeChange)
	ON_EN_CHANGE(IDC_THRESHOLD_VALUE, OnEnChangeEditLow)
	ON_EN_CHANGE(IDC_THRESHOLD_VALUE_HIGH, OnEnChangeEditHigh)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CThresholdDialog message handlers

BOOL CThresholdDialog::OnInitDialog() 
{
	//CWnd::OnInitDialog();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CThresholdDialog::IncThresholdValue(int nStep)
{
	int nPos = GetScrollPos(SB_HORZ);
	CScrollBar* pScrollBar = GetScrollBarCtrl(SB_HORZ);	
	if (nStep==0)
		OnHScroll(SB_LINERIGHT,nPos,pScrollBar);
	else
		OnHScroll(SB_PAGERIGHT,nPos,pScrollBar);
}

void CThresholdDialog::DecThresholdValue(int nStep)
{
	int nPos = GetScrollPos(SB_HORZ);
	CScrollBar* pScrollBar = GetScrollBarCtrl(SB_HORZ);	
	if (nStep==0)
		OnHScroll(SB_LINELEFT,nPos,pScrollBar);
	else
		OnHScroll(SB_PAGELEFT,nPos,pScrollBar);

}

HBRUSH CThresholdDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

    pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));
	//LOGBRUSH LogBrush;
	//m_BrushActive.GetLogBrush(&LogBrush);
	//pDC->SetBkColor(LogBrush.lbColor);

	return m_BrushActive;
}


void CThresholdDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CWnd::OnPaint() for painting messages
	OnPaintSlider2Thump(dc);
}
void CThresholdDialog::OnPaintSlider2Thump(CDC &dc) {
	CRect ClientRect;
	GetClientRect(&ClientRect);

	m_nRangeHeight = ClientRect.Height();
	m_nArrowWidth = m_nRangeHeight * 3 / 4;

	m_nRangeWidth = ClientRect.Width() - 2 * m_nArrowWidth - 60;

	CRect ClipBox;

	dc.GetClipBox(&ClipBox);

	// Erase Background.
	CGdiObject *pBr = dc.SelectStockObject(GRAY_BRUSH);
	CGdiObject *pPen = dc.SelectStockObject(BLACK_PEN);
	dc.Rectangle(&ClipBox);

	int nPositionLow = static_cast<int>((m_dLeft - m_dMin) / (m_dMax - m_dMin) * m_nRangeWidth + 0.5)+30;
	int nPositionHigh = static_cast<int>((m_dRight - m_dMin) / (m_dMax - m_dMin) * m_nRangeWidth + 0.5)+30;
	dc.SelectStockObject(BLACK_PEN);
	CBrush ButtonFace;
	ButtonFace.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
	dc.SelectObject(ButtonFace);

	m_RectLeft = CRect(CPoint(nPositionLow, 0), CSize(m_nArrowWidth, m_nRangeHeight));
	{
		CRect & rect = m_RectLeft;
		dc.Rectangle(&rect);
		rect.DeflateRect(1, 1, 2, 2);
		// Make appearance 3D
		if (!m_bTracking || m_TrackMode != TRACK_LEFT) {
			CPen pen1;
			pen1.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNHIGHLIGHT));
			dc.SelectObject(pen1);
			dc.MoveTo(rect.left, rect.bottom);
			dc.LineTo(rect.left, rect.top);
			dc.LineTo(rect.right, rect.top);
			CPen pen2;
			pen2.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW));
			dc.SelectObject(pen2);
			dc.LineTo(rect.right, rect.bottom);
			dc.LineTo(rect.left, rect.bottom);
		}

		// Draw arrow
		dc.SelectStockObject(BLACK_PEN);
		dc.SelectStockObject(BLACK_BRUSH);
		CPoint poly[] = {
			CPoint(nPositionLow + m_nArrowWidth / 3, m_nRangeHeight / 3),
			CPoint(nPositionLow + m_nArrowWidth / 3, m_nRangeHeight * 2 / 3),
			CPoint(nPositionLow + m_nArrowWidth * 2 / 3, m_nRangeHeight / 2),
		};
		dc.Polygon(poly, 3);
	}
	m_RectRight = CRect(CPoint(m_nArrowWidth + nPositionHigh, 0), CSize(m_nArrowWidth, m_nRangeHeight));
	{
		CRect & rect = m_RectRight;
		dc.SelectObject(ButtonFace);
		dc.Rectangle(&rect);
		// Make appearance 3D
		if (!m_bTracking || m_TrackMode != TRACK_RIGHT) {
			rect.DeflateRect(1, 1, 2, 2);
			CPen pen1;
			pen1.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNHIGHLIGHT));
			dc.SelectObject(pen1);
			dc.MoveTo(rect.left, rect.bottom);
			dc.LineTo(rect.left, rect.top);
			dc.LineTo(rect.right, rect.top);
			CPen pen2;
			pen2.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW));
			dc.SelectObject(pen2);
			dc.LineTo(rect.right, rect.bottom);
			dc.LineTo(rect.left, rect.bottom);
		}

		// Draw Arrow
		dc.SelectStockObject(BLACK_PEN);
		dc.SelectStockObject(BLACK_BRUSH);
		CPoint poly[] = {
			CPoint(m_nArrowWidth + nPositionHigh + m_nArrowWidth * 2 / 3,    m_nRangeHeight / 3),
			CPoint(m_nArrowWidth + nPositionHigh + m_nArrowWidth * 2 / 3,    m_nRangeHeight * 2 / 3),
			CPoint(m_nArrowWidth + nPositionHigh + m_nArrowWidth / 3,        m_nRangeHeight / 2),
		};
		dc.Polygon(poly, 3);
	}
	dc.SelectStockObject(WHITE_BRUSH);
	// Draw Area in between.
	CRect rect = CRect(CPoint(m_nArrowWidth + nPositionLow + 1, 1), CPoint(m_nArrowWidth + nPositionHigh, m_nRangeHeight - 1));

	dc.SelectStockObject(NULL_PEN);
	dc.SelectStockObject(WHITE_BRUSH);
	dc.Rectangle(&rect);
}


void CThresholdDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bTracking) {
		int x = point.x - m_ClickOffset.x;

		CRect rect;
		WPARAM changed = 0;
		switch (m_TrackMode) {
		case TRACK_LEFT: {
			double dOldLeft = m_dLeft;
			m_dLeft = static_cast<double>(x - m_nArrowWidth / 2) / m_nRangeWidth * (m_dMax - m_dMin) + m_dMin - 60;
			if (m_dLeft >= m_dRight)
				m_dLeft = m_dRight;
			if (m_dLeft <= m_dMin)
				m_dLeft = m_dMin;

			if (dOldLeft != m_dLeft)
				changed = RS_LEFTCHANGED;
			rect = m_RectLeft;
		} break;
		case TRACK_RIGHT: {
			double dOldRight = m_dRight;
			m_dRight = static_cast<double>(x - m_nArrowWidth * 3 / 2) / m_nRangeWidth * (m_dMax - m_dMin) + m_dMin - 60;
			if (m_dRight <= m_dLeft)
				m_dRight = m_dLeft;
			if (m_dRight >= m_dMax)
				m_dRight = m_dMax;
			if (dOldRight != m_dRight)
				changed = RS_RIGHTCHANGED;
			rect = m_RectRight;
		} break;
		case TRACK_MIDDLE:
		{
			double delta = m_dRight - m_dLeft;
			ASSERT(delta >= 0.0);
			m_dLeft = static_cast<double>(x - m_nArrowWidth) / m_nRangeWidth * (m_dMax - m_dMin) + m_dMin - delta / 2.0;
			m_dRight = static_cast<double>(x - m_nArrowWidth) / m_nRangeWidth * (m_dMax - m_dMin) + m_dMin + delta / 2.0;
			if (m_dLeft <= m_dMin) {
				m_dLeft = m_dMin;
				m_dRight = m_dLeft + delta;
			}
			if (m_dRight >= m_dMax) {
				m_dRight = m_dMax;
				m_dLeft = m_dRight - delta;
			}
			changed = RS_BOTHCHANGED;
		}
		break;
		}
		SendMessage(RANGE_CHANGED, changed, static_cast<LPARAM>(0));

		Invalidate();
	}
	CWnd::OnMouseMove(nFlags, point);
}


void CThresholdDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	TRACE("Down Point %d, %d\n", point.x, point.y);
	SetFocus();
	Invalidate();

	if (!m_bTracking) {
		// Hit Testing into Rects.
		// Left, Middle or Right?
		CRect rect;
		GetClientRect(&rect);
		CRect middleRect = CRect(m_RectLeft.right + 1, 0, m_RectRight.left - 1, rect.bottom);

			middleRect = CRect(0, m_RectLeft.bottom + 1, m_RectLeft.right, m_RectRight.top - 1);
		if (m_RectLeft.PtInRect(point)) {
			m_bTracking = TRUE;
			m_TrackMode = TRACK_LEFT;
			m_ClickOffset = point - m_RectLeft.CenterPoint();
		}
		else if (m_RectRight.PtInRect(point)) {
			m_bTracking = TRUE;
			m_TrackMode = TRACK_RIGHT;
			m_ClickOffset = point - m_RectRight.CenterPoint();
		}
		else if (middleRect.PtInRect(point)) {
			m_bTracking = TRUE;
			m_TrackMode = TRACK_MIDDLE;
			m_ClickOffset = point - middleRect.CenterPoint();
		}
		if (m_bTracking)
			SetCapture();
	}
	char str[4];
	sprintf_s(str, "%d", m_nThresholdValue.nLow);
	m_editBinarizeLow.SetWindowTextA(str);
	sprintf_s(str, "%d", m_nThresholdValue.nHigh);
	m_editBinarizeHigh.SetWindowTextA(str);
	CWnd::OnLButtonDown(nFlags, point);
}


void CThresholdDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bTracking) {
		m_bTracking = FALSE;
		::ReleaseCapture();
		Invalidate();
	}
	CWnd::OnLButtonUp(nFlags, point);
}

LRESULT CThresholdDialog::OnRangeChange(WPARAM wParam, LPARAM /* lParam */) {
	UpdateData(FALSE);

	char str[4];
	m_nThresholdValue.nLow = (int)m_dLeft;
	m_nThresholdValue.nHigh = (int)m_dRight;
	sprintf_s(str, "%d", m_nThresholdValue.nLow);
	m_editBinarizeLow.SetWindowTextA(str);

	sprintf_s(str, "%d", m_nThresholdValue.nHigh);
	m_editBinarizeHigh.SetWindowTextA(str);

	m_pView->SendMessage(WM_SET_THRESHOLD, (WPARAM)&m_nThresholdValue);

	return static_cast<LRESULT>(0);
}

void CThresholdDialog::OnEnChangeEditLow() {
	UpdateData(TRUE);
	CString strTemp = _T("");
	m_editBinarizeLow.GetWindowTextA(strTemp);
	int nTemp = atoi(strTemp);
	if (nTemp > m_nThresholdValue.nHigh)
	{
		m_nThresholdValue.nLow = m_nThresholdValue.nHigh;
		char str[4];
		sprintf_s(str, "%d", m_nThresholdValue.nHigh);
		m_editBinarizeLow.SetWindowTextA(str);
	}

	else if (nTemp < 0) {
		m_nThresholdValue.nLow = 0;
		char str[4];
		sprintf_s(str, "%d", 0);
		m_editBinarizeLow.SetWindowTextA(str);
	}	
	else
		m_nThresholdValue.nLow = nTemp;
	m_dLeft = m_nThresholdValue.nLow;
	OnPaint();
	m_pView->SendMessage(WM_SET_THRESHOLD, (WPARAM)&m_nThresholdValue);
	Invalidate();
	UpdateData(FALSE);
}

void CThresholdDialog::OnEnChangeEditHigh() {
	UpdateData(TRUE);
	CString strTemp = _T("");
	m_editBinarizeHigh.GetWindowTextA(strTemp);
	int nTemp = atoi(strTemp);
	if (nTemp < m_nThresholdValue.nLow)
	{
		m_nThresholdValue.nHigh = m_nThresholdValue.nLow;

	}

	else if (nTemp > 255) {
		m_nThresholdValue.nHigh =255;
		char str[4];
		sprintf_s(str, "%d", 255);
		m_editBinarizeHigh.SetWindowTextA(str);
	}
	else
		m_nThresholdValue.nHigh = nTemp;
	m_dRight = m_nThresholdValue.nHigh;
	OnPaint();
	m_pView->SendMessage(WM_SET_THRESHOLD, (WPARAM)&m_nThresholdValue);
	Invalidate();
	UpdateData(FALSE);

}
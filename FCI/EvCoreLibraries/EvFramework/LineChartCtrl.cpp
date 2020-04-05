// LineChartCtrl.cpp : implementation file
//


#include "stdafx.h"
#include "LineChartCtrl.h"
#include "MathDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLineChartCtrl

CLineChartCtrl::CLineChartCtrl()
{
	CLineChartCtrl::RegisterWndClass(AfxGetInstanceHandle());
	m_nGlobalUpper = INT_MAX;
	m_bIsDialogBar = FALSE;
	//m_nGlobalLower = 0;
	//m_nGlobalUpperCount = 0;
}

CLineChartCtrl::~CLineChartCtrl()
{
	int nCount = (int) m_items.GetSize();
	
	for (int i = 0; i < nCount; i++)
		delete m_items.GetAt(i);
	m_items.RemoveAll();
}


BEGIN_MESSAGE_MAP(CLineChartCtrl, CWnd)
	//{{AFX_MSG_MAP(CLineChartCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CLineChartCtrl::RegisterWndClass(HINSTANCE hInstance)
{
	WNDCLASS wc;
	wc.lpszClassName = "LINE_CHART_CTRL"; // matches class name in client
	wc.hInstance = hInstance;
	wc.lpfnWndProc = ::DefWindowProc;
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = 0;
	wc.lpszMenuName = NULL;
	wc.hbrBackground = (HBRUSH) ::GetStockObject(LTGRAY_BRUSH);
	wc.style = CS_GLOBALCLASS; // To be modified
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	return (::RegisterClass(&wc) != 0);
}

void CLineChartCtrl::InvalidateCtrl()
{
	CClientDC dc(this);
	CRect rcClient;
	GetClientRect(rcClient);

	if (m_MemDC.GetSafeHdc() == NULL)
	{
		m_MemDC.CreateCompatibleDC(&dc);
		m_Bitmap.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());
		m_MemDC.SelectObject(m_Bitmap);
		
		// draw scale
		m_MemDC.SetBkColor(RGB(0,0,0));
		CBrush bkBrush(HS_CROSS,RGB(0,80,0));
		m_MemDC.FillRect(rcClient,&bkBrush);
	}

	InvalidateRect(rcClient, FALSE);
}

UINT CLineChartCtrl::SetPos(int nIndex, UINT nPos)
{
	if (nIndex >= m_items.GetSize())
		return 0;

	CLineChartItem* pItem = m_items.GetAt(nIndex);

//	if (nPos > pItem->m_nUpper)
//		nPos = pItem->m_nUpper;
//	if (nPos < pItem->m_nLower)
//		nPos = pItem->m_nLower;

	pItem->m_nOldPos = pItem->m_nPos;
	pItem->m_nPos = nPos;

	return pItem->m_nOldPos;
}

void CLineChartCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rcClient;
	GetClientRect(rcClient);

	// draw scale
	if (m_MemDC.GetSafeHdc() != NULL)
		dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &m_MemDC, 0, 0, SRCCOPY);
}

void CLineChartCtrl::DrawSpike()
{
	CRect rcClient;
	GetClientRect(rcClient);

	if (m_MemDC.GetSafeHdc() != NULL)
	{
		m_MemDC.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &m_MemDC, 4, 0, SRCCOPY);

		// draw scale
		CRect rcRight = rcClient;
		rcRight.left = rcRight.right - 4;
		m_MemDC.SetBkColor(RGB(0,0,0));

		CBrush bkBrush(HS_HORIZONTAL,RGB(0,80,0));  
		m_MemDC.FillRect(rcRight,&bkBrush);

		static BOOL bDrawVerticle = FALSE;
		bDrawVerticle = !bDrawVerticle;

		// Draw Verticle lines only every two steps
		if (bDrawVerticle)
		{
			CPen pen(PS_SOLID, 1, RGB(0,80,0));
			CPen* pOldPen = m_MemDC.SelectObject(&pen);
			m_MemDC.MoveTo(CPoint(rcClient.right-2, rcClient.top));
			m_MemDC.LineTo(CPoint(rcClient.right-2, rcClient.bottom));
			m_MemDC.SelectObject(pOldPen);
		}

		int nCount = (int) m_items.GetSize();
		CLineChartItem* pItem;
		CPoint ptOld, ptNew;
		for (int i=0; i<nCount; i++)
		{
			pItem = m_items.GetAt(i);
			if(!pItem->m_bShow)
				continue;

			UINT  nRange = pItem->m_nUpper - pItem->m_nLower;
			ptOld.x = rcRight.left-1; // Minus one to make sure to draw inside the area
			ptNew.x = rcRight.right-1;
			ptOld.y = (int)((((float)(nRange - pItem->m_nOldPos))/(float)nRange)
				* (float)rcRight.Height());
			ptNew.y = (int)((((float)(nRange - pItem->m_nPos))/(float)nRange)
				* (float)rcRight.Height());

			CPen pen(PS_SOLID, 2, pItem->m_colorLine);
			CPen* pOldPen = m_MemDC.SelectObject(&pen);
			m_MemDC.MoveTo(ptOld);
			m_MemDC.LineTo(ptNew);
			m_MemDC.SelectObject(pOldPen);
		}
	}
}

void CLineChartCtrl::DrawSpikeGlobal()
{
	CRect rcClient;
	GetClientRect(rcClient);

	if (m_MemDC.GetSafeHdc() != NULL)
	{
		m_MemDC.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &m_MemDC, 4, 0, SRCCOPY);

		// draw scale
		CRect rcRight = rcClient;
		rcRight.left = rcRight.right - 4;
		m_MemDC.SetBkColor(RGB(0,0,0));

		CBrush bkBrush(HS_HORIZONTAL,RGB(0,80,0));  
		m_MemDC.FillRect(rcRight,&bkBrush);

		static BOOL bDrawVerticle = FALSE;
		bDrawVerticle = !bDrawVerticle;

		// Draw Verticle lines only every two steps
		if (bDrawVerticle)
		{
			CPen pen(PS_SOLID, 1, RGB(0,80,0));
			CPen* pOldPen = m_MemDC.SelectObject(&pen);
			m_MemDC.MoveTo(CPoint(rcClient.right-2, rcClient.top));
			m_MemDC.LineTo(CPoint(rcClient.right-2, rcClient.bottom));
			m_MemDC.SelectObject(pOldPen);
		}

		int nCount = (int) m_items.GetSize();
		CLineChartItem* pItem;
		CPoint ptOld, ptNew;

		UINT nRange = Round(m_nGlobalUpper*1.5);// - m_nGlobalLower;
		double dFactor = float(rcRight.Height()) / nRange;
		for (int i=0; i<nCount; i++)
		{
			pItem = m_items.GetAt(i);
			if(!pItem->m_bShow)
				continue;

			ptOld.x = rcRight.left-8; // Minus one to make sure to draw inside the area
			ptNew.x = rcRight.right-8;
			ptOld.y = int((nRange - pItem->m_nOldPos) * dFactor);
			ptNew.y = int((nRange - pItem->m_nPos) * dFactor);
			//ptOld.y = rcRight.Height() - int(float(pItem->m_nOldPos) * float(rcRight.Height()) / nRange);
			//ptNew.y = rcRight.Height() - int(float(pItem->m_nPos) * float(rcRight.Height()) / nRange);

			CPen pen(PS_SOLID, 2, pItem->m_colorLine);
			CPen* pOldPen = m_MemDC.SelectObject(&pen);
			m_MemDC.MoveTo(ptOld);
			m_MemDC.LineTo(ptNew);
			m_MemDC.SelectObject(pOldPen);
		}
	}

	Invalidate(FALSE);
}

BOOL CLineChartCtrl::Add(COLORREF color, UINT Upper, UINT Lower)
{
	CLineChartItem* pItem = new CLineChartItem;
	pItem->m_colorLine = color;
	pItem->m_nLower = Lower;
	pItem->m_nUpper = Upper;
	pItem->m_nPos = 0;
	pItem->m_nOldPos = 0;
	pItem->m_bShow = TRUE;

	try 
	{
		m_items.Add(pItem);

		InvalidateCtrl();
		return TRUE;
	}
	catch (CMemoryException* e)
	{
		if (pItem !=NULL) 
			delete pItem;
		e->Delete();
		return FALSE;
	}
}

BOOL CLineChartCtrl::Remove(int nIndex)
{
	CLineChartItem* pItem = m_items[nIndex];
	m_items.RemoveAt(nIndex);

	return TRUE;
}

BOOL CLineChartCtrl::Modify(int nIndex, COLORREF color, UINT Upper, UINT Lower)
{
	CLineChartItem* pItem = m_items[nIndex];

	pItem->m_colorLine = color;
	pItem->m_nPos = Round(pItem->m_nPos * (double(Lower) / pItem->m_nLower));
	pItem->m_nOldPos = Round(pItem->m_nOldPos * (double(Lower) / pItem->m_nLower));
	pItem->m_nLower = Lower;
	pItem->m_nUpper = Upper;

	return TRUE;
}

BOOL CLineChartCtrl::SetGlobalBounds(UINT Upper, UINT Lower)
{
	//CLineChartItem* pItem;

	if((Upper > UINT(Round(m_nGlobalUpper*0.5)) && Upper < UINT(Round(m_nGlobalUpper*1.5))))
		return TRUE;
	
	/*if(Upper < m_nGlobalUpper) {
		m_nGlobalUpperCount++;
		if(m_nGlobalUpperCount<10)
			return TRUE;
	}
	m_nGlobalUpperCount = 0;*/

	//for(int nIndex=0; nIndex<m_items.GetSize(); nIndex++) {
	//	pItem = m_items[nIndex];
	//	pItem->m_nLower = Lower;
	//	pItem->m_nUpper = nRaisedUpper;
	//}
	m_nGlobalUpper = Upper;
	//m_nGlobalLower = Lower;

	return TRUE;
}

BOOL CLineChartCtrl::ShowLine(int nIndex, BOOL bShow)
{
	CLineChartItem* pItem = m_items[nIndex];
	pItem->m_bShow = bShow;

	return TRUE;
}

void CLineChartCtrl::Go()
{
	DrawSpike();

	Invalidate(FALSE);
}

void CLineChartCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_bIsDialogBar)
		((CDialogBar*)GetParent())->CalcDynamicLayout(0, LM_HORZDOCK);
}

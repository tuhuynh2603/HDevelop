/// DisplayOverlay.cpp : implementation of generic Overlay functions
/// Udupa; March'2013

#include "StdAfx.h"
#include "DisplayOverlay.h"


void DrawLineNow(CDC* pDC, LINE_INFO *pLineInfo)
{
	LOGPEN CurrentPenInfo;
	CPen *pCurrentPen, NewPen;
	pCurrentPen = pDC->GetCurrentPen();
	pCurrentPen->GetLogPen(&CurrentPenInfo);
	if(CurrentPenInfo.lopnColor != pLineInfo->PenInfo.lopnColor ||
		CurrentPenInfo.lopnStyle != pLineInfo->PenInfo.lopnStyle ||
		CurrentPenInfo.lopnWidth.x != pLineInfo->PenInfo.lopnWidth.x) {

		NewPen.CreatePen(pLineInfo->PenInfo.lopnStyle,
						 pLineInfo->PenInfo.lopnWidth.x,
						 pLineInfo->PenInfo.lopnColor);
		pCurrentPen = pDC->SelectObject(&NewPen);
		pCurrentPen->DeleteObject();
	}


	pDC->MoveTo(pLineInfo->pt1);
	pDC->LineTo(pLineInfo->pt2);
}

void DrawRectNow(CDC* pDC, RECT_INFO *pRectInfo)
{
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

	pDC->Rectangle(pRectInfo->rect);

}


BOOL DrawStrNow(CDC* pDC, STR_ATTR *pStringattr)
{
	CFont* pOldFont = NULL;
	TRY	{
		if(pDC->GetBkMode() != TRANSPARENT)
			pDC->SetBkMode(TRANSPARENT);
		if(pDC->GetTextAlign() != TA_LEFT)
			pDC->SetTextAlign(TA_LEFT);
		if(pDC->GetTextColor() != pStringattr->Color)
			pDC->SetTextColor(pStringattr->Color);

		CFont fontText;
		fontText.CreateFont(-pStringattr->FontHeight,0,0,0,800,FALSE,FALSE,0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS, "Segoe UI");

		pOldFont = (CFont*) pDC->SelectObject(&fontText);

		//CSize sizeText = pDC->GetTextExtent(pStringattr->strMsg);
		pDC->TextOut(pStringattr->pt.x, pStringattr->pt.y, pStringattr->strMsg);

		if (pOldFont != NULL)
			pDC->SelectObject(pOldFont);
		fontText.DeleteObject();
	}
	CATCH_ALL(e) {
		if (pOldFont != NULL)
			pDC->SelectObject(pOldFont);

		return FALSE;
	}
	END_CATCH_ALL

	return TRUE;
}

void DrawOverlayText(CDC* pDC, CSize& szMax, CString& strStatus, CPoint& ptPos, LOGPEN& penText, int nFontHeight)
{
	STR_ATTR strAttr;
	strAttr.Color = penText.lopnColor;
	strAttr.FontHeight = nFontHeight;
	strAttr.strMsg = strStatus;
	strAttr.pt.x = ptPos.x;
	strAttr.pt.y = ptPos.y;

	DrawStrNow(pDC, &strAttr);
}

void DrawOverlayLine(CDC* pDC, CSize& szMax, CPoint2D& pt2D1, CPoint2D& pt2D2, LOGPEN& penInfo)
{
	LINE_INFO LineInfo;
	LineInfo.PenInfo.lopnColor = penInfo.lopnColor;
	LineInfo.PenInfo.lopnStyle = penInfo.lopnStyle;
	LineInfo.PenInfo.lopnWidth = penInfo.lopnWidth;
	LineInfo.pt1 = CPoint(Round(pt2D1.x),Round(pt2D1.y));
	LineInfo.pt2 = CPoint(Round(pt2D2.x),Round(pt2D2.y));
	CheckPoint(&(LineInfo.pt1), &szMax);
	CheckPoint(&(LineInfo.pt2), &szMax);

	DrawLineNow(pDC, &LineInfo);
}

void DrawOverlayRect(CDC* pDC, CSize& szMax, CRect& rect, LOGPEN& penInfo)
{
	if(CheckRect(&rect, &szMax))
		return;

	RECT_INFO RectInfo;
	RectInfo.PenInfo.lopnColor = penInfo.lopnColor;
	RectInfo.PenInfo.lopnStyle = penInfo.lopnStyle;
	RectInfo.PenInfo.lopnWidth = penInfo.lopnWidth;
	RectInfo.rect = *rect;

	DrawRectNow(pDC, &RectInfo);
}
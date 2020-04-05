/// DisplayOverlay.cpp : implementation of generic Overlay functions
/// Udupa; March'2013

#pragma once

#include "ImageStruct.h"

void DrawLineNow(CDC* pDC, LINE_INFO *pLineInfo);	// Draw Line
void DrawRectNow(CDC* pDC, RECT_INFO *pRectInfo);	// Draw Rectangle
BOOL DrawStrNow(CDC* pDC, STR_ATTR *pStrAttr); // Draw String New function

void DrawOverlayLine(CDC* pDC, CSize& szMax, CPoint2D& pt2D1, CPoint2D& pt2D2, LOGPEN& penInfo);
void DrawOverlayRect(CDC* pDC, CSize& szMax, CRect& rect, LOGPEN& penInfo);
void DrawOverlayText(CDC* pDC, CSize& szMax, CString& strStatus, CPoint& ptPos, LOGPEN& penText, int nFontHeight);

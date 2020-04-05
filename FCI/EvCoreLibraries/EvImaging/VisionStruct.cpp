#include "StdAfx.h"
#include "VisionStruct.h"
#include "ExternalFunctions.h"
#include "DebugLog.h"
#include "DisplayOverlay.h"


CVisionResult::CVisionResult()
{
	nResult = 0;
	nCursors = 0;
	nDefectRects = 0;
	nDefectRegions = 0;

	pCursors = NULL;
	pDefectRects = NULL;
	pDefectRegions = NULL;
	pData = NULL;

	bCreated = FALSE;
}

CVisionResult::~CVisionResult()
{
	if(bCreated) {
		if(pData)
			free(pData);
	}
	else {
		if(pCursors)
			free(pCursors);
		if(pDefectRects)
			free(pDefectRects);
		if(pDefectRegions)
			free(pDefectRegions);
	}
}

int CVisionResult::GetSize()
{
	return sizeof(int)*4;
}

int CVisionResult::Create(BYTE* pSource, int nResultArg)
{
	int nHeaderSize = CVisionResult::GetSize();

	bCreated = (pSource == NULL);
	if(!bCreated) {
		memcpy(this, pSource, nHeaderSize);
		pData = pSource;
	}

	int nSize = (nHeaderSize + 
			sizeof(CPoint2D) * nCursors + 
			sizeof(CRect) * nDefectRects + 
			sizeof(CBlockArc) * nDefectRegions);

	BYTE* pDataTemp;
	if(bCreated) {
		nResult = nResultArg;
		pData = (BYTE*) malloc(nSize);
		memcpy(pData, this, nHeaderSize);
		pDataTemp = pData + nHeaderSize;
		
		if(nCursors) {
			pCursors = (CPoint2D*) pDataTemp;
			pDataTemp += sizeof(CPoint2D) * nCursors;
		}
		if(nDefectRects) {
			pDefectRects = (CRect*) pDataTemp;
			pDataTemp += sizeof(CRect) * nDefectRects;
		}
		if(nDefectRegions)
			pDefectRegions = (CBlockArc*) pDataTemp;
	}
	else {
		pDataTemp = pData + nHeaderSize;
		if(nCursors > 0) {
			int nLength = sizeof(CPoint2D) * nCursors;
			pCursors = (CPoint2D*) malloc(nLength);
			memcpy(pCursors, pDataTemp, nLength);
			pDataTemp += nLength;
		}

		if(nDefectRects > 0) {
			int nLength = sizeof(CRect) * nDefectRects;
			pDefectRects = (CRect*) malloc(nLength);
			memcpy(pDefectRects, pDataTemp, nLength);
			pDataTemp += nLength;
		}

		if(nDefectRegions > 0) {
			int nLength = sizeof(CBlockArc) * nDefectRegions;
			pDefectRegions = (CBlockArc*) malloc(nLength);
			memcpy(pDefectRegions, pDataTemp, nLength);
		}
	}


	return nSize;
}

CBlockArc::CBlockArc()
{
	dRadiusOuter = 0;
	dRadiusInner = 0;
	dAngleStart = 0;
	dAngleEnd = 0;
	pt2DCenter = CPoint2D(0,0);
	nPerimeter = 0;
	nStartIndex = 0;
	nEndIndex = 0;
	nFailType = -1;
	nOffset = 0;
}

CBlockArc::~CBlockArc()
{
}

CBlockArc & CBlockArc::operator =(CBlockArc BlockArc)
{
	dRadiusOuter = BlockArc.dRadiusOuter;
	dRadiusInner = BlockArc.dRadiusInner;
	dAngleStart = BlockArc.dAngleStart;
	dAngleEnd = BlockArc.dAngleEnd;
	pt2DCenter = BlockArc.pt2DCenter;
	nPerimeter = BlockArc.nPerimeter;
	nStartIndex = BlockArc.nStartIndex;
	nEndIndex = BlockArc.nEndIndex;
	nRectInnerPos = BlockArc.nRectInnerPos;
	nRectOuterPos = BlockArc.nRectOuterPos;
	nFailType = BlockArc.nFailType;
	nOffset = BlockArc.nOffset;
	return *this;
}


int CBlockArc::Draw(IMAGEVIEW_INFO *pView,
					LOGPEN* pPen)
{
	int i;
	int nDrawPt;
	double dAngleStep;
	double dAngle;
	double dSinAngle;
	double dCosAngle;
	CPoint2D pt2DOuter0;
	CPoint2D pt2DOuter1;
	CPoint2D pt2DInner0;
	CPoint2D pt2DInner1;

try {
	dAngleStep = PI/180;
	nDrawPt = Round((dAngleEnd - dAngleStart)/dAngleStep) + 1;
	dSinAngle = sin(dAngleStart);
	dCosAngle = cos(dAngleStart);
	pt2DOuter0 = pt2DCenter + CPoint2D(dRadiusOuter * dCosAngle, dRadiusOuter * dSinAngle + nOffset);
	pt2DInner0 = pt2DCenter + CPoint2D(dRadiusInner * dCosAngle, dRadiusInner * dSinAngle + nOffset);
	DrwLine2D(pView, &pt2DOuter0, &pt2DInner0, pPen);
	for(i=1;i<nDrawPt;i++) {
		dAngle = dAngleStart + i * dAngleStep;
		dSinAngle = sin(dAngle);
		dCosAngle = cos(dAngle);
		pt2DOuter1 = pt2DCenter + CPoint2D(dRadiusOuter * dCosAngle, dRadiusOuter * dSinAngle + nOffset);
		pt2DInner1 = pt2DCenter + CPoint2D(dRadiusInner * dCosAngle, dRadiusInner * dSinAngle + nOffset);
		DrwLine2D(pView, &pt2DOuter0, &pt2DOuter1, pPen);
		DrwLine2D(pView, &pt2DInner0, &pt2DInner1, pPen);
		pt2DOuter0 = pt2DOuter1;
		pt2DInner0 = pt2DInner1;
	}
	DrwLine2D(pView, &pt2DOuter0, &pt2DInner0, pPen);
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CBlockArc::Draw");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CBlockArc::Draw(CDC* pDC, CSize& sizeMax, LOGPEN& pen)
{
	int i;
	int nDrawPt;
	double dAngleStep;
	double dAngle;
	double dSinAngle;
	double dCosAngle;
	CPoint2D pt2DOuter0;
	CPoint2D pt2DOuter1;
	CPoint2D pt2DInner0;
	CPoint2D pt2DInner1;

try
{
	dAngleStep = PI/180;
	nDrawPt = Round((dAngleEnd - dAngleStart)/dAngleStep) + 1;
	dSinAngle = sin(dAngleStart);
	dCosAngle = cos(dAngleStart);
	pt2DOuter0 = pt2DCenter + CPoint2D(dRadiusOuter * dCosAngle, dRadiusOuter * dSinAngle + nOffset);
	pt2DInner0 = pt2DCenter + CPoint2D(dRadiusInner * dCosAngle, dRadiusInner * dSinAngle + nOffset);

	DrawOverlayLine(pDC, sizeMax, pt2DOuter0, pt2DInner0, pen);
	for(i=1;i<nDrawPt;i++) {
		dAngle = dAngleStart + i * dAngleStep;
		dSinAngle = sin(dAngle);
		dCosAngle = cos(dAngle);
		pt2DOuter1 = pt2DCenter + CPoint2D(dRadiusOuter * dCosAngle, dRadiusOuter * dSinAngle + nOffset);
		pt2DInner1 = pt2DCenter + CPoint2D(dRadiusInner * dCosAngle, dRadiusInner * dSinAngle + nOffset);
		DrawOverlayLine(pDC, sizeMax, pt2DOuter0, pt2DOuter1, pen);
		DrawOverlayLine(pDC, sizeMax, pt2DInner0, pt2DInner1, pen);
		pt2DOuter0 = pt2DOuter1;
		pt2DInner0 = pt2DInner1;
	}
	DrawOverlayLine(pDC, sizeMax, pt2DOuter0, pt2DInner0, pen);
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CBlockArc::DrawDC");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

/// Drawing defect regions over the image
/// Udupa; March'2013
int DrawDefectRegions(CDC* pDC, CSize& sizeMax, int nResult, CRectArray& rects, CRegionArray& regions)
{
	LOGPEN pen;
	pen.lopnColor = RGB(0,0,255);
	pen.lopnStyle = PS_SOLID;
	pen.lopnWidth.x = 3;
	
	CPen NewPen;
	NewPen.CreatePen(pen.lopnStyle, pen.lopnWidth.x, pen.lopnColor);
	CPen* pOldPen = pDC->GetCurrentPen();
	pDC->SelectObject(&NewPen);

	CBrush* pOldBrush = (CBrush *) pDC->SelectStockObject(HOLLOW_BRUSH);

	for(int i=0; i<rects.GetSize(); i++)
		DrawOverlayRect(pDC, sizeMax, rects[i], pen);
	for(int i=0; i<regions.GetSize(); i++)
		regions[i].Draw(pDC, sizeMax, pen);

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	NewPen.DeleteObject();

	return 0;
}

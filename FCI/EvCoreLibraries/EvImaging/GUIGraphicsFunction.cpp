#include "stdafx.h"
#include "GUIGraphicsFunction.h"
#include "ExternalFunctions.h"
#include "ImageFunction.h"
#include "Utilities.h"

#include "BitmapFunctions.h"

int DrwCur2D(IMAGEVIEW_INFO *pView,
			 CPoint2D *pt2D,
		     LOGPEN *pPenInfo,
		     int nExt)
{
	CPoint pt = CPoint(Round(pt2D->x), Round(pt2D->y));
	DrwCur(pView,&pt,pPenInfo,nExt);
	return 0;
}

int DrwRectangle2D(IMAGEVIEW_INFO *pView,
				   CRectangle2D *prect2D,
				   LOGPEN *pPenInfo)
{
	CPoint ptTopLft;
	CPoint ptBotLft;
	CPoint ptTopRht;
	CPoint ptBotRht;

	ptTopLft = CPoint(Round(prect2D->GetDevTopLeft().x), Round(prect2D->GetDevTopLeft().y));
	ptBotLft = CPoint(Round(prect2D->GetDevBottomLeft().x), Round(prect2D->GetDevBottomLeft().y));
	ptTopRht = CPoint(Round(prect2D->GetDevTopRight().x), Round(prect2D->GetDevTopRight().y));
	ptBotRht = CPoint(Round(prect2D->GetDevBottomRight().x), Round(prect2D->GetDevBottomRight().y));

	DrwLine(pView,&ptTopLft,&ptTopRht,pPenInfo);
	DrwLine(pView,&ptTopRht,&ptBotRht,pPenInfo);
	DrwLine(pView,&ptBotLft,&ptBotRht,pPenInfo);
	DrwLine(pView,&ptTopLft,&ptBotLft,pPenInfo);

	return 0;
}

int DrwEllipse(IMAGEVIEW_INFO *pView,
			   CPoint2D *ppt2DCtr,
			   double dRadiusX,
			   double dRadiusY,
			   LOGPEN *pPenInfo)
{
	int i;
	double x,y;
	CPoint pt1;
	CPoint pt2;
	double dAngleStepX;
	double dAngleStepY;
	int nAngleStep;
	x = dRadiusX + ppt2DCtr->x;
	y = ppt2DCtr->y;
	dAngleStepX = 180/(PI * dRadiusX);
	if(dAngleStepX < 1)
		dAngleStepX = 1;
	dAngleStepY = 180/(PI * dRadiusY);
	if(dAngleStepY < 1)
		dAngleStepY = 1;
	if(dAngleStepX < dAngleStepY)
		nAngleStep = Round(dAngleStepX);
	else
		nAngleStep = Round(dAngleStepY);
	if(nAngleStep > 40)
		nAngleStep = 40;
	pt1 = CPoint(Round(x), Round(y));
	for(i=1; i<360; i+=nAngleStep) {
		x = dRadiusX * cos(i*PI/180) + ppt2DCtr->x;
		y = dRadiusY * sin(i*PI/180) + ppt2DCtr->y;
		pt2 = CPoint(Round(x), Round(y));
		DrwLine(pView,&pt1,&pt2,pPenInfo);
		pt1 = pt2;
	}
	return 0;
}

int DrwEllipse2D(IMAGEVIEW_INFO *pView,
				 CRectangle2D *prect2D,
				 LOGPEN *pPenInfo)
{
	int i;
	CPoint pt1;
	CPoint pt2;
	double x, y;
	double dRotX, dRotY;
	double dCenterX, dCenterY;
	double dHalfWidth, dHalfHeight;
	double dSinRotAngle, dCosRotAngle, dRotAngle;
	dHalfWidth = prect2D->GetWidth()/2;
	dHalfHeight = prect2D->GetHeight()/2;
	dRotAngle = -prect2D->GetAngle();
	dSinRotAngle = sin(dRotAngle);
	dCosRotAngle = cos(dRotAngle);
	dCenterX = prect2D->GetCenterPoint().x;
	dCenterY = prect2D->GetCenterPoint().y;
	x = dHalfWidth * cos((double) 0);
	y = dHalfHeight * sin((double) 0);
	dRotX = x * dCosRotAngle + y * dSinRotAngle + dCenterX;
	dRotY =-x * dSinRotAngle + y * dCosRotAngle + dCenterY;
	pt1 = CPoint(Round(dRotX), Round(dRotY));
	for(i=1; i<360; i++) {
		x = dHalfWidth * cos(i*PI/180);
		y = dHalfHeight * sin(i*PI/180);
		dRotX = x * dCosRotAngle + y * dSinRotAngle + dCenterX;
		dRotY =-x * dSinRotAngle + y * dCosRotAngle + dCenterY;
		pt2 = CPoint(Round(dRotX), Round(dRotY));
		DrwLine(pView,&pt1,&pt2,pPenInfo);
		pt1 = pt2;
	}
	return 0;
}

int DrwLine2D(IMAGEVIEW_INFO *pView,
			  CPoint2D *pt2D1,
			  CPoint2D *pt2D2,
			  LOGPEN *pPenInfo)
{
	DrwLine(pView, &CPoint(Round(pt2D1->x),Round(pt2D1->y)),
				  &CPoint(Round(pt2D2->x),Round(pt2D2->y)),pPenInfo);
	return 0;
}

int DrwContour(IMAGEVIEW_INFO *pView, 
			   CPoint2D *pt2DContour, 
			   int nContour, 
			   LOGPEN *pPenInfo)
{
	int i;
	if(nContour>1) {
		for(i=1;i<nContour;i++)
			DrwLine2D(pView, &pt2DContour[i-1], &pt2DContour[i], pPenInfo);
		DrwLine2D(pView, &pt2DContour[0], &pt2DContour[nContour-1], pPenInfo);
	}
	return 0;
}
int DrwArrow(IMAGEVIEW_INFO *pView,
			 int nSide,
			 CPoint* pptPos,
			 double dOffset,
			 int nArrowWidth,
			 int nArrowLength,
			 LOGPEN *pPenInfo)
{
	CPoint ptStart;
	CPoint ptEnd;
	CPoint ptArrow1;
	CPoint ptArrow2;
	int nArrowHalfWidth;
	nArrowHalfWidth = nArrowWidth/2;
	switch (nSide) {
		case LEFT:
			ptStart = CPoint(Round(pptPos->x - dOffset), pptPos->y);
			ptEnd = CPoint(ptStart.x - nArrowLength, ptStart.y);
			ptArrow1 = CPoint(ptStart.x - nArrowHalfWidth, ptStart.y - nArrowHalfWidth);
			ptArrow2 = CPoint(ptStart.x - nArrowHalfWidth, ptStart.y + nArrowHalfWidth);
			break;
		case TOP:
			ptStart = CPoint(pptPos->x, Round(pptPos->y - dOffset));
			ptEnd = CPoint(ptStart.x, ptStart.y - nArrowLength);
			ptArrow1 = CPoint(ptStart.x - nArrowHalfWidth, ptStart.y - nArrowHalfWidth);
			ptArrow2 = CPoint(ptStart.x + nArrowHalfWidth, ptStart.y - nArrowHalfWidth);
			break;
		case RIGHT:
			ptStart = CPoint(Round(pptPos->x + dOffset), pptPos->y);
			ptEnd = CPoint(ptStart.x + nArrowLength, ptStart.y);
			ptArrow1 = CPoint(ptStart.x + nArrowHalfWidth, ptStart.y - nArrowHalfWidth);
			ptArrow2 = CPoint(ptStart.x + nArrowHalfWidth, ptStart.y + nArrowHalfWidth);
			break;
		case BOTTOM:
			ptStart = CPoint(pptPos->x, Round(pptPos->y + dOffset));
			ptEnd = CPoint(ptStart.x, ptStart.y + nArrowLength);
			ptArrow1 = CPoint(ptStart.x - nArrowHalfWidth, ptStart.y + nArrowHalfWidth);
			ptArrow2 = CPoint(ptStart.x + nArrowHalfWidth, ptStart.y + nArrowHalfWidth);
			break;
	}
	CheckPoint(&ptStart, &pView->sizeImage);
	CheckPoint(&ptEnd, &pView->sizeImage);
	CheckPoint(&ptArrow1, &pView->sizeImage);
	CheckPoint(&ptArrow2, &pView->sizeImage);

	DrwLine(pView, &ptStart, &ptEnd,pPenInfo);
	DrwLine(pView, &ptStart, &ptArrow1,pPenInfo);
	DrwLine(pView, &ptStart, &ptArrow2,pPenInfo);
	return 0;
}

int DrwDebugImage(IMAGEVIEW_INFO* pView,
				  CImageBuffer* pImgBufDispFOV,
				  CImageBuffer* pImgBufProc,
				  CRect* prCImageBufferProcRelative,
				  CPoint* pptImgBufProcTopLftInFOV,
				  CString* pstrDebug,
				  BOOL bColorDisplay,
				  BOOL bCleanupDisplay,
				  BOOL bWaitForResponse)
{
	CImageBuffer pImgBufDispFOVBackup;
	CRect rCImageBufferDebugRelative;
	CRect rcDisplay;
	CRect rcFOV;
	CPoint ptTopLft;
	int nError;
	int nResponse = 0;

	if(!pImgBufDispFOV->GetAdapter()) {
		ASSERT(!"pImgBufDispFOV is not a Display Buffer");
		return -1;
	}

	if (!pImgBufDispFOV) {
		ASSERT(!"pImgBufDispFOV is a bad Pointer");
		return -1;
	}
	if (!pImgBufDispFOV->IsAllocated()) {
		ASSERT(!"pImgBufDispFOV is not Allocated");
		return -1;
	}
	if (!pImgBufProc) {
		ASSERT(!"pImgBufProc is a bad Pointer");
		return -1;
	}
	if (!pImgBufProc->IsAllocated()) {
		ASSERT(!"pImgBufProc is not Allocated");
		return -1;
	}
	if (ValidateRect(prCImageBufferProcRelative,&(pImgBufProc->GetImgSize()))) {
		ASSERT(!"prCImageBufferProcRelative is Invalid");
		return -1;
	}

	ptTopLft = CPoint(prCImageBufferProcRelative->left + pptImgBufProcTopLftInFOV->x, prCImageBufferProcRelative->top + pptImgBufProcTopLftInFOV->y);
	rcDisplay = CRect(ptTopLft, prCImageBufferProcRelative->Size());
	rcFOV = CRect(CPoint(0,0), pImgBufDispFOV->GetImgSize());
	if(!RectInRect(rcFOV, rcDisplay)) {
		ASSERT(!"rcDisplay is Invalid");
		return -1;
	}
	if (ValidateRect(&rcDisplay,&(pImgBufDispFOV->GetImgSize()))) {
		ASSERT(!"Size of prCImageBufferProc is Invalid");
		return -1;
	}

	if (ValidateRect(&rcDisplay,&(pView->sizeImage))) {
		ASSERT(!"Size of prCImageBufferProc is Invalid");
		return -1;
	}

	if(bWaitForResponse) {
		pImgBufDispFOVBackup.Allocate(AbsSize(rcFOV.Size()), false, pImgBufDispFOV->IsColor());
		nError = ImgCopy(pImgBufDispFOV,&rcFOV,&pImgBufDispFOVBackup,&rcFOV);
		if(nError) {
			OutputDebugLog("ImgCopy Failed in DrwDebugImageWT");
			return -1;
		}
	}

	nError = ImgCopy(pImgBufProc,prCImageBufferProcRelative,pImgBufDispFOV,&rcDisplay);
	if(nError) {
		OutputDebugLog("ImgCopy Failed in DrwDebugImageWT");
		return -1;
	}
	if(bColorDisplay)
		pImgBufDispFOV->GetAdapter()->ChangeToColorPalette();

	DrwUpdate(pView);
	UpdateStatusBar(pstrDebug,3);
	if(bWaitForResponse)
		nResponse = WaitResponse(pView);
	if(bCleanupDisplay)
		DrwDel(pView);
	if(bColorDisplay)
		pImgBufDispFOV->GetAdapter()->ChangeToGrayPalette();

	if(bWaitForResponse) {
		nError = ImgCopy(&pImgBufDispFOVBackup,&rcFOV,pImgBufDispFOV,&rcFOV);
		//DrwUpdate(pView);
	}
	if(nError) {
		OutputDebugLog("ImgCopy Failed in DrwDebugImageWT");
		return -1;
	}
	if(nResponse)
		return -1;

	return 0;
}

int DrwDebugImage(IMAGEVIEW_INFO* pView,
				  CImageBuffer* pImgBufDispFOV,
				  CImageBuffer* pImgBufProc,
				  CRect* prCImageBufferProcRelative,
				  CImageBuffer* pImgBufMask,
				  CRect* prCImageBufferMaskRelative,
				  CPoint* pptImgBufProcTopLftInFOV,
				  CString* pstrDebug)
{
	CImageBuffer ImgMaskDebugDisplay;
	CRect rcDispInFOV;
	CRect rcDispRelative;
	int nResponse;
	int nError;

	rcDispInFOV = CRect(*pptImgBufProcTopLftInFOV, prCImageBufferMaskRelative->Size());
	rcDispRelative = CRect(CPoint(0,0), prCImageBufferMaskRelative->Size());
	ImgMaskDebugDisplay.Allocate(AbsSize(rcDispRelative.Size()));
	nError = ImgDebugMasking(pImgBufDispFOV,
							 &rcDispInFOV,
							 pImgBufProc,
							 prCImageBufferProcRelative,
							 pImgBufMask,
							 prCImageBufferMaskRelative,
							 &ImgMaskDebugDisplay,
							 &rcDispRelative);
	if(nError != 0)
		goto ErrorAbort;
	nResponse = DrwDebugImage(pView, pImgBufDispFOV, &ImgMaskDebugDisplay, &rcDispRelative, &rcDispInFOV.TopLeft(), pstrDebug);
	if(nResponse)
		goto ErrorAbort;			
	return 0;
ErrorAbort:
	return -1;
}

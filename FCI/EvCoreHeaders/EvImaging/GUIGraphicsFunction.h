#pragma once

#include "ImageStruct.h"

int DrwCur2D(IMAGEVIEW_INFO *pImageViewInfo, CPoint2D *pt2D, LOGPEN *pPenInfo, int nExt);
int DrwRectangle2D(IMAGEVIEW_INFO *pImageViewInfo, CRectangle2D *prect2D, LOGPEN *pPenInfo);
int DrwEllipse2D(IMAGEVIEW_INFO *pImageViewInfo, CRectangle2D *prect2D, LOGPEN *pPenInfo);
int DrwEllipse(IMAGEVIEW_INFO *pImageViewInfo, CPoint2D *ppt2DCtr, double dRadiusX, double dRadiusY, LOGPEN *pPenInfo);
int DrwLine2D(IMAGEVIEW_INFO *pImageViewInfo, CPoint2D *pt2D1, CPoint2D *pt2D2, LOGPEN *pPenInfo);
int DrwDebugImage(IMAGEVIEW_INFO* pView,
				  CImageBuffer* pImgBufDispFOV,
				  CImageBuffer* pImgBufProc,
				  CRect* prCImageBufferProcRelative,
				  CPoint* pptProcTopLftInFOV,
				  CString* pstrDebug,
				  BOOL bColorDisplay = false,
				  BOOL bCleanupDisplay = TRUE,
				  BOOL bWaitForResponse = TRUE);

int DrwDebugImage(IMAGEVIEW_INFO* pView,
				  CImageBuffer* pImgBufDispFOV,
				  CImageBuffer* pImgBufProc,
				  CRect* prCImageBufferProcRelative,
				  CImageBuffer* pImgBufMask,
				  CRect* prCImageBufferMaskRelative,
				  CPoint* pptImgBufProcTopLftInFOV,
				  CString* pstrDebug);

int DrwArrow(IMAGEVIEW_INFO *pView,
			 int nSide,
			 CPoint* pptPos,
			 double dOffset,
			 int nArrowWidth,
			 int nArrowLength,
			 LOGPEN *pPenInfo);

int DrwContour(IMAGEVIEW_INFO *pView, CPoint2D *pt2DContour, int nContour, LOGPEN *pPenInfo);

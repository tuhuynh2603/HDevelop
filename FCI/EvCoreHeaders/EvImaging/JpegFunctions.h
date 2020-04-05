#pragma once

#include "ImageBuffer.h"

int LoadJPGFile(CString strFileName, CImageBuffer& bufImage);
int SaveJPGFile(CString strFileName, CImageBuffer& bufImage, BYTE nQuality);
int SaveDefectsJPGFile(CString strFileName, 
					   CImageBuffer& bufImage, 
					   CRect* prcDefects, 
					   int nDefects, 
					   int nLineWidth,
					   BYTE nQuality);
int SaveDefectsJPGFile(CString strFileName, 
					   CImageBuffer& bufImage, 
					   CRect* prcDefects,
					   int nDefects,
					   int* pnArrowSide,
					   int nArrowOffset,
					   CImageBuffer* pImgArrow, 
					   BYTE nQuality);
int SaveDaiIchiDefectsJPGFile(CString strFileName, 
					   CImageBuffer& bufImage, 
					   CRect* prcDefects,
					   int nDefects,
					   int nInnerRectOffset,
					   int nMinInnerSize,
					   int nOuterInnerDis,
					    int nLineWidth,
					   BYTE nQuality);
extern int CreateDefectsLocationWindow(CSize& sizeImage,
								 CRect* prcDefects, 
								 int nDefects,
								 CRect* prcLocationDefects,
								 int nLocationDefects,
								 int nInnerRectOffset,
								 int nMinInnerSize,
								 int nOuterInnerDis);


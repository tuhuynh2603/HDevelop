#pragma once

#include "ImageBuffer.h"
#include <vector>

int LoadGrayScaleBMPFile(CString strFileName, CImageBuffer& bufImage, BOOL bSameBitCount=FALSE);
int SaveGrayScaleBMPFile(CString strFileName, CImageBuffer& bufImage);
int SaveGrayScaleBMPFileROI(CString strFileName, CRect* prcROI, CImageBuffer& bufImage);

int LoadDefectInfoBMPEmbeddedFile(CString strFileName, std::vector<CRect>& prcDefects, std::vector<CPoint>& pptDefectCenters);
int LoadGrayScaleBMPEmbeddedFile(CString strFileName, CImageBuffer& bufImage, std::vector<CRect>& prcDefects, std::vector<CPoint>& pptDefectCenters, BOOL bSameBitCount = FALSE);
int SaveDefectsEmbeddedBMPFile(CString strFileName, CImageBuffer& bufImage, std::vector<CRect> prcDefects, std::vector<CPoint> pptDefectCenters);

int SaveDefectsBMPFile(CString strFileName, 
					   CImageBuffer& bufImage, 
					   CRect* prcDefects, 
					   int nDefects,
					   int nLineWidth);
int SaveDefectsBMPFile(CString strFileName, 
					   CImageBuffer& bufImage, 
					   CRect* prcDefects,
					   int nDefects,
					   int nArrowOffset,
					   CImageBuffer* pImgArrow);

int ImgAddArrow(CImageBuffer* pImgMask,
				CRect* prcMask,
				CImageBuffer* pImgMask1,
				CRect* prcMask1,
				CPoint* pptOffsetOfMask1);	
int GetArrowDir(CRect* prcFndBlob,
					  int nFndBlobNo,
					  double dOffset,
					  int nArrowWidth,
					  int nArrowLength,
					  int* pnSide);
int CreateDefectsLocationWindow(CSize& sizeImage,
								 CRect* prcDefects, 
								 int nDefects,
								 CRect* prcLocationDefects,
								 int nLocationDefects,
								 int nInnerRectOffset,
								 int nMinInnerSize,
								 int nOuterInnerDis);

int SaveDaiIchiDefectsBMPFile(CString strFileName, 
						    CImageBuffer& bufImage, 
						    CRect* prcDefects, 
						    int nDefects,
							int nInnerRectOffset,
							int nMinInnerSize,
							int nOuterInnerDis,
							int nLineWidth);

union UnsignedShortByteUnion {
	unsigned short int asUnsignedShort;
	unsigned char asBytes[2];
};
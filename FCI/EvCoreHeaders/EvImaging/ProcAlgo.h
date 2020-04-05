#pragma once

#include "stdafx.h"
#include "ImageStruct.h"
#include "ProcAlgoStruct.h"

#include "MathDef.h"

int Create1DBinomialMask(int nMaskSize, 
						 double *ptrMask, 
						 BOOL bNormalize);

int Create1DGaussianMask(int nMaskSize,		// Size of Mask(if it is 3, then actual size is 3+1+3=7)
						 double dSigma,		// Gaussian Sigma
						 double *ptrMask);	// Pointer to Mask

int ApplyGaussianFilter(long *plProjVal,	// Projection Values for applying Mask Filter
						int nProjSize,		// Size of Projection
						double *ptrMask,	// Pointer to Mask
						int nMaskSize);		// Mask Size(if it is 3, then actual size is 3+1+3=7)

int	GetEdge1D(long *plProjVal,
			  int nWidth,		// Proj Size
			  int nSize,		// Group Factor
			  int nRef,			// Ref Value
			  int nThreshold,
			  int nGradient,
			  int nMethod,
			  int nDirection,
			  double *pdFndPos,
			  int *pnFndGrad,
			  long lDebugFlag);

int GetEdgeEx(long *pImageLine,
			  int nLength,
			  ADV_GRD_EDGE_FIND_PARM *pEdgeInfo,
			  double pdEdgePos[],
			  int pnEdgePol[]);

int GetEdgeExNew(long *pImageLine,
				 int nLength,
				 double *ptrMask,
				 ADV_GRD_EDGE_FIND_PARM *pEdgeInfo,
				 double pdEdgePos[],
				 int pnEdgePol[],
				 int nOffset=0);

int GetEdgeExNew2(BYTE *pImageLine,
				 int nLength,
				 double *ptrMask,
				 ADV_GRD_EDGE_FIND_PARM *pEdgeInfo,
				 double pdEdgePos[],
				 int pnEdgePol[],
				 int nOffset=0);

class CRange
{
public:
	CRange();
	~CRange();
	double dStart;
	double dEnd;
};

class CRingRange
{
public:
	CRingRange();
	~CRingRange();
	void Clean();
	int FindRange(CPoint2D* ppt2DCtr,
				  double dInnerRadius,
				  double dOuterRadius);

	int FindRange(CPoint2D* ppt2DCtrInner,
				  double dInnerRadius,
				  CPoint2D* ppt2DCtrOuter,
				  double dOuterRadius);

	int BinariseGradImgAndAddress(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 CSize* pcsGradStep,
			 BYTE nThresVal,
			 long* plBlobPixelAddress,
			 long* plBlobPixelNo);

	int FindRingMask(CImageBuffer *pImgMask,
			 CRect *prcMask,
			 CPoint* pptOffset);

public:
	int* pnPosY;
	CSize* pcsLeftRange;
	CSize* pcsRightRange;
	int nRangeNo;
};


class CProjection {
public :
	PROJ_RSLT m_ProjRslt;
	PROJ_SLANT_RSLT m_ProjSlantRslt;

	CProjection();
	~CProjection();
	int GetProj(IMAGEVIEW_INFO *pView,
				CImageBuffer *pImgProj,
				PROJ_PARM *pParm,
				long lDebugFlag); 

	int GetSlantProj(IMAGEVIEW_INFO *pView,
					 CImageBuffer *pImgProj,
					 PROJ_SLANT_PARM *pParm,
					 long lDebugFlag); 
	
	int GetSlantMaxProj(IMAGEVIEW_INFO *pView,
					  CImageBuffer *pImgProj,
					  PROJ_SLANT_PARM *pParm,
					  long lDebugFlag);

	int GetSlantMinProj(IMAGEVIEW_INFO *pView,
					  CImageBuffer *pImgProj,
					  PROJ_SLANT_PARM *pParm,
					  long lDebugFlag);

	void CopyProjRslt(PROJ_RSLT* pProjRsltSrc,
					  PROJ_RSLT* pProjRsltDst);
private :
	int GetMaxMinValue();
	int DrawProj(IMAGEVIEW_INFO *pView,
				 PROJ_PARM *pParm,
				 PROJ_RSLT *pRslt);
	int DrawSlantProj(IMAGEVIEW_INFO *pView,
					  PROJ_SLANT_PARM *pParm,
					  PROJ_RSLT *pRslt);

};

class CGridEdge {
public :
	GRD_EDGE_FIND_RSLT m_EdgeFindRslt;
	
	CGridEdge();

	~CGridEdge();

	int GetGridEdge(IMAGEVIEW_INFO *pView,
					CImageBuffer *pImg,
					GRD_EDGE_FIND_PARM *pParm,
				    long lDebugFlag);

	int GetLine(GRD_EDGE_FIND_PARM *pParm,
				GRD_EDGE_FIND_RSLT *pRslt,
				CLine2D *pLine);

private :
	int DrawGridEdge(IMAGEVIEW_INFO *pView,
					 GRD_EDGE_FIND_PARM *pParm,
					 GRD_EDGE_FIND_RSLT *pRslt);

};

class CAdvGridEdge {
public :
	ADV_GRD_EDGE_FIND_RSLT m_EdgeFindRslt;
	
	CAdvGridEdge();
	~CAdvGridEdge();
	int GetAdvGridEdge(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImg,
				       ADV_GRD_EDGE_FIND_PARM *pEdgeParm,
					   long lDebugFlag);

	int GetAdvGridEdgeII(IMAGEVIEW_INFO *pView,
						 CImageBuffer *pImg,
						 ADV_GRD_EDGE_FIND_PARM *pEdgeParm,
						 long lDebugFlag);

	int GetAdvGridEdgeIII(IMAGEVIEW_INFO *pView,
						  CImageBuffer *pImg,
						  ADV_GRD_EDGE_FIND_PARM *pEdgeParm,
						  CPoint ptStart,
						  CPoint ptEnd,
						  int nSrchWidth,
						  long lDebugFlag);

	int GetAdvGridEdgeIV(IMAGEVIEW_INFO *pView,
						 CImageBuffer *pImg,
				         ADV_GRD_EDGE_FIND_PARM *pEdgeParm,
						 CPoint ptStart,
					     CPoint ptEnd,
					     int nSrchWidth,
					     long lDebugFlag);


	int GetHoughLine(ADV_GRD_EDGE_FIND_PARM *pEdgeParm,
					 LINE_FIND_PARM *pLineParm,
					 ADV_GRD_EDGE_FIND_RSLT *pRslt,
					 CLine2D *pLine);

	int GetHoughLineII(ADV_GRD_EDGE_FIND_PARM *pEdgeParm,
					   LINE_FIND_PARM *pLineParm,
					   ADV_GRD_EDGE_FIND_RSLT *pRslt,
					   CLine2D *pLine,
					   int nPtSelCnt=3);

	int DrawAdvGridEdge(IMAGEVIEW_INFO *pView,
						ADV_GRD_EDGE_FIND_PARM *pParm,
						ADV_GRD_EDGE_FIND_RSLT *pRslt);

	int DrawAdvGridEdgeIII(IMAGEVIEW_INFO *pView,
						   ADV_GRD_EDGE_FIND_PARM *pParm,
						   CPoint ptStart,
						   CPoint ptEnd,
						   int nSrchWidth,
						   ADV_GRD_EDGE_FIND_RSLT *pRslt);

private :

};

class CAdvBlobFind
{
public :
	ADV_BLOB_FIND_RSLT m_AdvBlobFindRslt;

	CAdvBlobFind();
	~CAdvBlobFind();
	void Clean();

	int ConnectionRoughCheck(IMAGEVIEW_INFO *pView,
							ADV_BLOB_FIND_PARM* pAdvBlobRoughCheckParm, 
							CRect* prcRoughCheck,
							int* pnRoughCheck,
							CSize* pcsConnection,
							long lProcMode,
							long lDebugFlag);
	
	int BlackHoleContour(CImageBuffer* pImg,
					   CRect* prcBlob,
					   CPoint* pptROITopLeft,
					   CPoint* pptBlobStartPos,
					   CPoint* pptHoleContour,
					   int* pnptHoleContour,
					   CRect* prcHoleFound,
					   BYTE byteProcFill,
					   BYTE byteFill,
					   BOOL* pbHole);	

	int GetAdvBlobHole(IMAGEVIEW_INFO *pView,
					  CImageBuffer *pImgSrc,
					  ADV_BLOB_FIND_PARM *pParm,
					  long lProcMode,
					  long lDebugFlag);

	int GetBlobHoleInfo(ADV_BLOB_INFO *pAdvBlobInfo,
					  CImageBuffer* pImgBufProc,
					  CPoint* pptROITopLeft,
					  CRect* prcOrg,
					  ADV_BLOB_FIND_PARM *pParm,
					  long lProcMode,
					  long lDebugFlag);

	int GetMaxBlobIndex(long lSortingMode, int* pMaxSortingIndex);
	int BlobSorting(long lSortingMode, BOOL bAscend, int* pSortingIndex);

	int GetAdvBlob(IMAGEVIEW_INFO *pView,
				   ADV_BLOB_FIND_PARM *pParm,
				   long lDebugFlag);

	int GetAdvBlobCombinedInfo(CRect* prcBlobCombined,
							   ADV_BLOB_INFO* pAdvBlobCombinedInfo);

	void DrawBlobContour(IMAGEVIEW_INFO* pView,
						 int nSamplingX,
						 int nSamplingY,
						 CPoint* pptOffset); 

	int GetAdvBlob3(IMAGEVIEW_INFO *pView,					// Vision System Info
					CImageBuffer *pImgSrc,					// Pointer to a Binary Image
					ADV_BLOB_FIND_PARM *pParm,			// Tolerance Parameters of Blob Finding
					long lProcMode,						// Blob Detection Options: 0 for X, Y Size and Area Check, Other Values refer to Definition		
					long lDebugFlag);



// GetAdvBlob4 is under development, please do not use
	int GetAdvBlob4(IMAGEVIEW_INFO *pView,
				  CImageBuffer *pImgSrc,
				  long* plBlobPixelAddress,
				  long lBlobPixelNo,
				  ADV_BLOB_FIND_PARM *pParm,
				  long lProcMode,
				  long lDebugFlag);


	int GetAdvBlobFiltering(IMAGEVIEW_INFO *pView,			// Vision System Info
					CImageBuffer *pImgSrc,					// Pointer to a Binary Source Image
					CImageBuffer *pImgDst,					// Pointer to a Binary Destination Image
					ADV_BLOB_FIND_PARM *pParm,			// Tolerance Parameters of Blob Finding
					long lProcMode,						// Blob Detection Options: 0 for X, Y Size and Area Check, Other Values refer to Definition		
					long lDebugFlag);

	int GetAdvBlobFiltering4(IMAGEVIEW_INFO *pView,			// Vision System Info
					CImageBuffer *pImgSrc,					// Pointer to a Binary Source Image
					CImageBuffer *pImgDst,					// Pointer to a Binary Destination Image
					long* plBlobPixelAddress,
					long lBlobPixelNo,
					ADV_BLOB_FIND_PARM *pParm,			// Tolerance Parameters of Blob Finding
					long lProcMode,						// Blob Detection Options: 0 for X, Y Size and Area Check, Other Values refer to Definition		
					long lDebugFlag);

	int GetAdvBlobFiltering5(IMAGEVIEW_INFO *pView,			// Vision System Info
					CImageBuffer *pImgGray,	   
					CRect* prcImgGray,
					CImageBuffer *pImgSrc,					// Pointer to a Binary Source Image
					CImageBuffer *pImgDst,					// Pointer to a Binary Destination Image
					long* plBlobPixelAddress,
					long lBlobPixelNo,
					ADV_BLOB_FIND_PARM *pParm,			// Tolerance Parameters of Blob Finding
					long lProcMode,						// Blob Detection Options: 0 for X, Y Size and Area Check, Other Values refer to Definition		
					long lDebugFlag);

	int WhiteBlobContour(CImageBuffer *pImg,					// Pointer to the Binary Image
					CPoint* pptROITopLeft,				// TopLeft Point of Processing ROI
					CPoint *pptBlobStartPos,			// The Starting White Point
					ADV_BLOB_INFO *FoundBlobInfo,		// Parameters of Found Blob
					long lProcMode);					// Blob Detection Options: 0 for X, Y Size and Area Check, Other Values refer to Definition		

	int WhiteBlobContour4(CImageBuffer *pImg,					// Pointer to the Binary Image
					CPoint* pptROITopLeft,				// TopLeft Point of Processing ROI
					CPoint *pptBlobStartPos,			// The Starting White Point
					ADV_BLOB_INFO *FoundBlobInfo,		// Parameters of Found Blob
					long lProcMode);					// Blob Detection Options: 0 for X, Y Size and Area Check, Other Values refer to Definition		

	int BlackBlobContour(CImageBuffer *pImg,					// Pointer to the Binary Image
					CPoint* pptROITopLeft,				// TopLeft Point of Processing ROI
					CPoint *pptBlobStartPos,			// The Starting White Point
					ADV_BLOB_INFO *FoundBlobInfo,		// Parameters of Found Blob
					long lProcMode);					// Blob Detection Options: 0 for X, Y Size and Area Check, Other Values refer to Definition		

	int BlackBlobContour4(CImageBuffer *pImg,
					   CPoint* pptROITopLeft,
					   CPoint *pptBlobStartPos,
					   ADV_BLOB_INFO *FoundBlobInfo,
					   long lProcMode);	

	int WhiteHoleContour(CImageBuffer* pImg,
					   CRect* prcBlob,
					   CPoint* pptROITopLeft,
					   CPoint* pptBlobStartPos,
					   CPoint* pptHoleContour,
					   int* pnptHoleContour,
					   CRect* prcHoleFound,
					   BYTE byteProcFill,
					   BYTE byteFill,
					   BOOL* pbHole);	

	BOOL FoundBlobCheck(ADV_BLOB_FIND_PARM *pParm,		// Tolerance Parameters of Blob Finding
					ADV_BLOB_INFO *FoundBlobRslt,		// Parameters of Found Blob
					long lProcMode);					// Blob Detection Options: 0 for X, Y Size and Area Check, Other Values refer to Definition	

	int FoundBlobRoughCheck(ADV_BLOB_FIND_PARM *pParm,
					ADV_BLOB_INFO *FoundBlobRslt,
					long lProcMode);	
    
	int CheckBlobByMinRectSize(IMAGEVIEW_INFO* pView,
						  CImageBuffer *pImgBin,
						  CRect *prcBin,
						  int nBlobType,
						  CSize *pszBlobHoleMin,									
						  CSize *pszBlobMin,
						  CPoint* pptFound,
						  BOOL* pbFound,
						  long lDebugFlag);

	int CheckBlobByMinSqr(CImageBuffer* pImgSrc,
						  CRect* prcSrc,
						  CSize* pcsMinSqr,
						  int nHoleArea,
						  int nBlobType,
						  CArray<CRect,CRect>* pAryRcFound,
			     		  BOOL* pbDefectFound);

 	int CheckBlobByMinSqr(CImageBuffer* pImgSrc,
							  CRect* prcSrc,
							  CSize* pcsMinSqr,
							  int nHoleArea,
							  int nBlobType,
							  CPoint* pptFound,
			     			  BOOL* pbDefectFound);
	int CheckBlobByRectPercentage(CImageBuffer* pImgSrc,
							  CRect* prcSrc,
							  CSize* pcsMinSqr,
							  int nHoleArea,
							  int nBlobType,
							  CPoint* pptFound,
			     			  BOOL* pbDefectFound);
	int CheckBlobByRectPercentage(CImageBuffer* pImgSrc,
							  CRect* prcSrc,
							  CSize* pcsMinSqr,
							  int nHoleArea,
							  int nBlobType,
							  CArray<CRect,CRect>* pAryRcFound,
			     			  BOOL* pbDefectFound);

private :
	CPoint* ptAryFndBlobContour;	// Found Blob Contour
	int nFndBlobContour;
	long m_lProcMode;
	int nBlobMemLoc;
	int pnDir[8];

	BOOL bBlobFill;
	long *plModifiedAddr;
	long lModifiedAddrNo;

	long *plNonFilledAddr;
	long lNonFilledNo;
	long *plFillSeedsAddr1;
	long lFillSeedsNo1;
	long *plFillSeedsAddr2;
	long lFillSeedsNo2;
	int nDir0, nDir1, nDir2, nDir3;
	int nDir4, nDir5, nDir6, nDir7;
	
	CPoint ptDstSrcOffset;

	int GetBlobInfo(ADV_BLOB_INFO *pAdvBlobInfo,
				CImageBuffer* pImgBufProc,
				CRect* prcOrg,
				ADV_BLOB_FIND_PARM *pParm,
				long lProcMode,
				long lDebugFlag);


	int GetBlobInfo4(ADV_BLOB_INFO *pAdvBlobInfo,
				  CImageBuffer* pImgBufProc,
				  CRect* prcOrg,
				  ADV_BLOB_FIND_PARM *pParm,
				  long lProcMode,
				  long lDebugFlag);

	int GetBlobFilteringInfo(ADV_BLOB_INFO *pAdvBlobInfo,
							 CImageBuffer* pImgBufProc,
							 CImageBuffer* pImgBufDst,	
							 CRect* prcOrg,
							 ADV_BLOB_FIND_PARM *pParm,
							 long lProcMode,
							 long lDebugFlag);

	int GetBlobFilteringInfo4(ADV_BLOB_INFO *pAdvBlobInfo,
							 CImageBuffer* pImgBufProc,
							 CImageBuffer* pImgBufDst,	
							 ADV_BLOB_FIND_PARM *pParm,
							 long lProcMode,
							 long lDebugFlag);


	int GetBlobFilteringInfo5(ADV_BLOB_INFO *pAdvBlobInfo,
							 CPoint* pptProcWndTopLeft,
							 CImageBuffer *pImgGray,	   
							 CRect* prcImgGray,
							 CImageBuffer* pImgBufProc,
							 CImageBuffer* pImgBufDst,	
							 ADV_BLOB_FIND_PARM *pParm,
							 long lProcMode,
							 long lDebugFlag);

	BOOL ContourCompleteCheck(int nBlobType, 
							  BYTE byteBotLft, 
							  BYTE byteBot, 
							  BYTE byteBotRht, 
							  BYTE byteRht,
							  CPoint* pptBlobStart);

	int DrawAdvBlob(IMAGEVIEW_INFO *pView,
					ADV_BLOB_FIND_PARM *pParm,
					ADV_BLOB_FIND_RSLT *pRslt);
};



class CHistogram {
public :
	HIST_FIND_RSLT m_HistogramRslt;
	
	CHistogram();
	~CHistogram();
	int GetHistogram(IMAGEVIEW_INFO *pView,
					 CImageBuffer *pImg,
				     HIST_FIND_PARM *pParm,
				     long lDebugFlag);
	int GetThreshold(int nMinThres, int nMaxThres, long *plVal);
	int GetPercentageThreshold(double dBlackPercentage, 
							   double dWhitePercentage, 
							   long *plVal, 
							   int &nThreshold);

	int GetWhitePercentageThreshold(double dWhitePercentage, 
									long *plVal, 
									int &nThreshold);
	int GetWhitePercentageCutoffThreshold(double dWhitePercentage, 
											long *plVal, 
											int &nThreshold);
	int GetBlackPercentageThreshold(double dBlackPercentage, 
									long *plVal, 
									int &nThreshold);
	int GetBlackPercentageCutoffThreshold(double dBlackPercentage, 
										long *plVal, 
										int &nThreshold);

	int GetAvgIntensity(double dPercentageStart,
						double dPercentageEnd,
						long *plVal, 
						int* pnAvgIntensity);

	int GetAvgIntensity(double dPercentageStart,
						double dPercentageEnd,
						long *plVal, 
						double* pdAvgIntensity);
private :
	int DrawHistogram(IMAGEVIEW_INFO *pView,
					HIST_FIND_PARM *pParm,
					HIST_FIND_RSLT *pRslt);

};


class CMask {
public :
	CMask();
	~CMask();
	int InitRect2DMask(CImageBuffer *pImgMask,
					   CRect* prcMask,
					   CRectangle2D *prc2D,
					   BYTE byteLabel);

	int ApplyGrayMaskFreeHand(CImageBuffer *pImgMask,
							CRect *prcImgMask,	
							CPoint2D *ppt2DMsk,	
							int npt2DMskNo,		
							BYTE byteLabel);	

	int ApplyGrayMaskFreeHand(CImageBuffer *pImgMask,
						CRect *prcImgMask,			
						CPoint2D *ppt2DMsk,			
						int npt2DMskNo,			
						CPoint* pptOffset,
						BYTE byteLabel);
				
	int ApplyGrayMaskFreeHand(CImageBuffer *pImgMask,
							CRect *prcImgMask,	
							CPoint *pptMsk,	
							int nptMskNo,	
							CPoint* pptOffset,
							BYTE byteLabel);	

	int ApplyGrayMaskFreeHand(CImageBuffer *pImgMask,
							CRect *prcImgMask,	
							CArray<CPoint, CPoint>* pAryPtMsk,	
							CPoint* pptOffset,
							BYTE byteLabel);	

	int ApplyGrayMaskFreeHand(CImageBuffer *pImgMask,
							CRect *prcImgMask,	
							CArray<CPoint2D, CPoint2D>* pAryPt2DMsk,	
							CPoint* pptOffset,
							BYTE byteLabel);	

	int AddBandBinariseMask(CImageBuffer *pImgSrc,
						CRect *prcImgSrc,
						CImageBuffer *pMaskSrc,
						CRect *prcMaskSrc,
						CImageBuffer *pImgDst,
						CRect *prcDst,
						int nThresMin,
						int nThresMax,
						int nInvert);

	int EdgeMask(CImageBuffer* pImgMsk,
			   CRect *prcImgMask,
			   CPoint2D *ppt2DConvexMsk, 			// Convex Points in Order
			   int npt2DConvexMskNo,				// Number of Convex Points
			   BYTE byteLabel);				// The Label Value to the Linked Line  

private :
	CRect rcMskBoundingRelative;
	CRect rcMskBounding;					// Bounding Rectangle of the Convex Region
	CImageBuffer ImgMskTemp;						// Image Buffer to the Convex region
	CRect rcOverlap;						// The Overlap Rectangle between Process ROI and the Bounding Rect of the Convex region
	long* plContourAddress;
	long lContourNo;
	
	int FillMask(CImageBuffer *pImg,
				BYTE byteLabel);
	int LinkPt(CRect *prcImgMask,
			   CPoint2D *ppt2DConvexMsk, 			// Convex Points in Order
			   int npt2DConvexMskNo,				// Number of Convex Points
			   BYTE byteLabel);				// The Label Value to the Linked Line  
	int MergeMask(CImageBuffer *pImgMask,
				CRect *prcImgMask,	
				CImageBuffer *pImgMaskTemp,
				BYTE byteLabel);	
	int CheckFastFillStatus(CPoint2D *ppt2DMsk,
						int npt2DMskNo,
						BOOL* pbFillX,
						BOOL* pbFillY,
						CRect* prcBounding);
	int GetFillPairtPts(CRect* prcImgMask,
						CPoint2D* ppt2DConvexMsk, 
						int npt2DConvexMskNo,
						CRect* prcPtsBounding,
						BOOL bFillX, 
						BOOL bFillY,
						int* pnStartPos,
						int* pnEndPos,
						int* pnStartIndex,
						int* pnEndIndex);

	int FillMask(CImageBuffer *pImgMask,
				 CRect *prcImgMask,
				 BOOL bFillX, 
				 BOOL bFillY,
				 int* pnStartPos,
				 int* pnEndPos,
				 int nStartIndex,
				 int nEndIndex,
				 BYTE byteLabel);


};

class CPkgIntensityAvg {
public :
	CPkgIntensityAvg();
	~CPkgIntensityAvg();

	int PkgIntensityAvg(CImageBuffer *pPkgImg,		// Pointer to Source Image
						CRect *prcPkg,			// ROI Rectangle of the Source Image
						BYTE byteMinPkg,		// Package Min Intensity  
						BYTE byteMaxPkg,		// Package Max Intensity
						CImageBuffer *pMsk,			// Pointer to Mask Image
						CRect *prcMsk,			// ROI Rectangle of the Mask Image
						CImageBuffer *pPkgImgAvg,	// Pointer to Image of Average Intensity on the Package
						CRect *prcPkgAvg,		// ROI Rectangle of the Package Image
						int nSamplingStepX = 0,	// X Sampling: (<=0 Auto Setting) 
						int nSamplingStepY = 0, // Y Sampling: (<=0 Auto Setting)
						int nAvgROIWidth = 0,
						int nAvgROIHeight = 0);

	int PkgIntensityAvg(CImageBuffer *pPkgImg,		// Pointer to Source Image
						CRect *prcPkg,			// ROI Rectangle of the Source Image
						CImageBuffer *pMsk,			// Pointer to Mask Image
						CRect *prcMsk,			// ROI Rectangle of the Mask Image
						CImageBuffer *pPkgImgAvg,	// Pointer to Image of Average Intensity on the Package
						CRect *prcPkgAvg,		// ROI Rectangle of the Package Image
						int nSamplingStepX = 0,	// X Sampling: (<=0 Auto Setting) 
						int nSamplingStepY = 0, // Y Sampling: (<=0 Auto Setting)
						int nAvgROIWidth = 0,
						int nAvgROIHeight = 0);

private:
	CImageBuffer ImgPkgSampling;				// Image Buffer to Save the Sampling of the Source Image
	CRect rcPkgSamplingImg;				// ROI Rectangle the Sampling of the Source Image

	CImageBuffer ImgMskSampling;				// Image Buffer to Save the Sampling of the Mask Image
	CRect rcMskSamplingImg;				// ROI Rectangle the Sampling of the Mask Image

	CImageBuffer ImgAvgSampling;				// Image Buffer to Save the Sampling of the Average Intensity Image
	CRect rcAvgSamplingImg;				// Image Buffer to Save the Sampling of the Average Intensity Image

	CSize csSampling;					// CSize of the ROI after Sampling
	CSize csAvgSampling;				// CSize of the Average ROI after Re-Sampling
	int nNonMaskedPercentage;			// The Percentage Tolerance of Not-Masked points in a Segmented Area (nMaskStepX*nMaskStepY)
	int nSamplingX;						// The Sampling Step for X Direction 	
	int nSamplingY;						// The Sampling Step for Y Direction 	
	int nMaskStepX;						// The X Size of Segment Area for Average Intensity Calculation 	
	int nMaskStepY;						// The Y Size of Segment Area for Average Intensity Calculation 

	int nInterpolationX;				// Interpolation Step for X Direction
	int nInterpolationY;				// Interpolation Step for Y Direction
	int nInterpolationPosX;				// Number of Points in Interpolation Step for X Direction
	int nInterpolationPosY;				// Number of Points in Interpolation Step for Y Direction
	int	nNonMaskedSegmentNo;			// The Tolerance Number of Points that have not been Masked
	BYTE byteAvg;						// Package Average Intensity
	void PkgLocalIntensityAve(CImageBuffer *pSamplingMean,CRect *prcSamplingMean,CImageBuffer *pImgSampling,CRect *prcImgSampling,CImageBuffer *pMskSampling,CRect *prcMskSampling);
	void PkgLocalIntensityAve3(CImageBuffer *pSamplingAvg,		// Pointer to Image of Sampling Average
							CRect *prcSamplingAvg,		// ROI of Image of Sampling Average
							CImageBuffer *pImgSampling,		// Pointer to Sampling Source Image
							CRect *prcImgSampling,		// ROI of Sampling Source Image
							CImageBuffer *pMskSampling,		// Pointer to Sampling Mask Image
							CRect *prcMskSampling);		// ROI of Sampling Mask Image

	void Fast1DInterpolation(BYTE *pbyte,int nSize);
	void Fast1DInterpolation(BYTE *pbyte,long lStartIndex,int nSize);
	void Fast2DInterpolation(BYTE *pbyte,CSize csRect);

	void Fast2DInterpolation(BYTE *pbyte, CImageBuffer *pPkgImgAvg,CRect *prcPkgAvg,long lIndexStart);
	void PkgIntensityInterpolation(CImageBuffer *pPkgImgAvg,CRect *prcPkgAvg);

	int GetInterpolation(CArray<CPoint, CPoint>* pAryPtPresentByRow,
						CArray<CPoint, CPoint>* pAryPtPresentByCol,
						int nCol,
						int nRow,
						BYTE* pbyteVal);

	int Sampling(CImageBuffer *pImgSrc,
				CRect *prcSrc,
				CImageBuffer *pImgSamplingSrc,
				CRect *prcSamplingSrc,
				BYTE byteMinPkg,
				BYTE byteMaxPkg,
				CImageBuffer *pImgMsk,
				CRect *prcMsk,
				CImageBuffer *pImgSamplingMsk,
				CRect *prcSamplingMsk,
				CSize *pcsSampling);
};


class CAdvBinarise
{
public :
	CAdvBinarise();
	~CAdvBinarise();

	int Binarise(CImageBuffer *pImgSrc,			// Pointer to Source Image
			   CRect *prcSrc,				// ROI of Source Image
			   int nBlobType,
			   CImageBuffer *pImgMsk, 			// Pointer to Mask Image
			   CRect *prcMsk, 				// ROI of Mask Image
			   BYTE byteThresVal, 
			   CImageBuffer *pImgDst,  			// Pointer to Destination Image
			   CRect *prcDst); 				// ROI of Destination Image

	int Binarise(CImageBuffer *pImgSrc,			// Pointer to Source Image
			   CRect *prcSrc,				// ROI of Source Image
			   int nBlobType,
			   CImageBuffer *pImgMsk, 			// Pointer to Mask Image
			   CRect *prcMsk, 				// ROI of Mask Image
			   BYTE byteThresVal, 
			   CImageBuffer *pImgDst,  			// Pointer to Destination Image
			   CRect *prcDst,				// ROI of Destination Image
			   long* plBlobPixelAddress,
			   long* plBlobPixelNo);


	int Binarise(CImageBuffer *pImgSrc, 
				 CRect *prcSrc, 
				 int nBlobType,
				 CImageBuffer *pImgMsk, 
				 CRect *prcMsk, 
				 CImageBuffer *pImgThreshold, 
				 CRect *prcThreshold, 
				 CImageBuffer *pImgDst, 
				 CRect *prcDst);

	int Binarise(CImageBuffer *pImgSrc,		
			   CRect *prcSrc,	
			   int nBlobType,
			   CImageBuffer *pImgMsk,
			   CRect *prcMsk, 	
			   CImageBuffer *pImgMean,
			   CRect *prcMean,
			   BYTE byteContrast,
			   CImageBuffer *pImgDst,  
			   CRect *prcDst);

	int Binarise(CImageBuffer *pImgSrc,		
			   CRect *prcSrc,	
			   int nBlobType,
			   CImageBuffer *pImgMsk,
			   CRect *prcMsk, 	
			   CImageBuffer *pImgMean,
			   CRect *prcMean,
			   BYTE byteContrast,
			   CImageBuffer *pImgDst,  
			   CRect *prcDst,
			   long* plBlobPixelAddress,
			   long* plBlobPixelNo); 

	int BinariseExt(CImageBuffer *pImgSrc, 
					CRect *prcSrc, 
					int nBlobType,
					CImageBuffer *pImgMsk, 
					CRect *prcMsk, 
					CImageBuffer *pImgThreshold, 
					CRect *prcThreshold, 
					CImageBuffer *pImgDst, 
					CRect *prcDst,
					int* pnAccumulation);

	int GetAccPixelCount(CImageBuffer *pImgSrc,	
				CRect *prcSrc,	
				int nBlobType,
				CImageBuffer *pImgMsk,
				CRect *prcMsk, 
				CImageBuffer *pImgThreshold,
				CRect *prcThreshold, 
				long* plAccPixelCount);


	int GetAccPixelCount(CImageBuffer *pImgSrc,		
					   CRect *prcSrc,	
					   int nBlobType,
					   CImageBuffer *pImgMsk,
					   CRect *prcMsk, 	
					   CImageBuffer *pImgMean,
					   CRect *prcMean,
					   BYTE byteContrast,
					   long* plAccPixelCount); 

	int PkgThreshold(CImageBuffer *pImgSrc,
				 CRect *prectSrc,
				 CImageBuffer *pImgDst,
				 CRect *prectDst,
				 int nContrast,
				 int nInvert);

	int FindBandThreshold(CImageBuffer *pImgSrc, 
				 CRect *prcSrc, 
				 CImageBuffer *pImgMsk, 
				 CRect *prcMsk, 
				 int nDefectsPercentage,
				 BYTE &byteMinThreshold,
				 BYTE &byteMaxThreshold);

	int FindBandThreshold(CImageBuffer *pImgSrc,		// Pointer to Source Image	
						  CRect *prcSrc,				// ROI of Source Image
						  CImageBuffer *pImgMsk,			// Pointer to Mask Image
						  CRect *prcMsk,				// ROI of Mask Image
						  CSize* pcsSampling,
						  int nWhitePercentage,	// The Percentage of White Defect and Black Defect
						  int nBlackPercentage,	// The Percentage of White Defect and Black Defect
						  BYTE &byteBlackCutoffThreshold,	// Threshold to be Used to Mask Black Defect
						  BYTE &byteWhiteCutoffThreshold);	// Threshold to be Used to Mask White Defect


	int FindBandThreshold(CImageBuffer *pImgSrc,		// Pointer to Source Image	
						 CRect *prcSrc,				// ROI of Source Image
						 BYTE byteMinSrc,
						 BYTE byteMaxSrc,
						 CImageBuffer *pImgMsk,			// Pointer to Mask Image
						 CRect *prcMsk,				// ROI of Mask Image
						 CSize* pcsSampling,
						 int nWhitePercentage,	// The Percentage of White Defect and Black Defect
						 int nBlackPercentage,	// The Percentage of White Defect and Black Defect
						 BYTE &byteBlackCutoffThreshold,	// Threshold to be Used to Mask Black Defect
						 BYTE &byteWhiteCutoffThreshold);	// Threshold to be Used to Mask White Defect

	int AdvHist(CImageBuffer *pImgSrc,
				CRect *prcSrc, 
				CImageBuffer *pImgMsk, 
				CRect *prcMsk, 
				long *plAry);

	int AdvHist(CImageBuffer *pImgSrc,		// Pointer to Source Image
				CRect *prcSrc,			// ROI of the Source Image
				CSize* pcsSamplingSrc,
				long *plAry);			// Histogram Data

	int AdvHist(CImageBuffer *pImgSrc,		// Pointer to Source Image
			    CRect *prcSrc,			// ROI of the Source Image
				CImageBuffer *pImgMsk,		// Pointer to Mask Image
				CRect *prcMsk,			// ROI of the Mask Image
				CSize* pcsSampling,
				long *plAry);			// Histogram Data


	int AdvHist(CImageBuffer *pImgSrc,		// Pointer to Source Image
			    CRect *prcSrc,			// ROI of the Source Image
				BYTE byteMinSrc,
				BYTE byteMaxSrc,
				CImageBuffer *pImgMsk,		// Pointer to Mask Image
				CRect *prcMsk,			// ROI of the Mask Image
				CSize* pcsSampling,
				long *plAry);			// Histogram Data

	int AdvHist(CImageBuffer *pImgSrc,		// Pointer to Source Image
			  CRect *prcSrc,			// ROI of the Source Image
			  CImageBuffer *pImgMsk,			// Pointer to Mask Image
			  CRect *prcMsk,			// ROI of the Mask Image
			  int nMaskColor,
			  long *plAry);				// Histogram Data

	int AdvHist(CImageBuffer* pImgBufSrc,
				 CRectangle2D* prc2DSrc,
				 CSize* pcsSampling,
				 long* plHist);

	int AdvHist(CImageBuffer* pImgBufSrc,
				 CRectangle2D* prc2DSrc,
				 CImageBuffer* pImgMask,
				 CPoint* pptMaskTopLft,
				 CSize* pcsSampling,
				 long* plHist);

	int AdvHist(CImageBuffer *pImgSrc,		// Pointer to Source Image
			  CRect *prcSrc,			// ROI of the Source Image
			  CSize* pcsSamplingSrc,
			  CImageBuffer *pImgMsk,			// Pointer to Mask Image
			  CRect *prcMsk,			// ROI of the Mask Image
			  CSize* pcsSamplingMsk,
			  long *plAry);				// Histogram Data


	int GetThreshold(CImageBuffer* pImgBufSrc,
					 CRectangle2D* prc2D,
					 CSize* pcsSampling,
					 double dBlackPercentage,
					 double dWhitePercentage,
					 BYTE* pbyteBlackThresholdCutoff,
					 BYTE* pbyteThresholdBlackAvg,
					 BYTE* pbyteWhiteThresholdCutoff,
					 BYTE* pbyteThresholdWhiteAvg);

	int GetThreshold(CImageBuffer* pImgBufSrc,
					 CRectangle2D* prc2D,
					 CImageBuffer* pImgMask,
					 CPoint* pptMaskTopLft,
					 CSize* pcsSampling,
					 double dBlackPercentage,
					 double dWhitePercentage,
					 BYTE* pbyteBlackThresholdCutoff,
					 BYTE* pbyteThresholdBlackAvg,
					 BYTE* pbyteWhiteThresholdCutoff,
					 BYTE* pbyteThresholdWhiteAvg);
};


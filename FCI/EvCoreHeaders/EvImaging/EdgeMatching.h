#ifndef _EDGE_MATCHING
#define _EDGE_MATCHING

//#include "ImageBuffer.h"
#include "Imaging.h"
#include "Morphology.h"

extern BOOL bCodeDebug;

int GetBlobEdge(CAdvBlobFind* pBlobFinder, 
				CRect* prcCheck, 
				int nBlobIndex, 
				int nSide,
				CArray<CPoint, CPoint>* pAryptEdge);

int BinariseLevelGradImg(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
 			 int nColor,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 CRect* prcGradStep,
			 BYTE nThresVal);

int BinariseLevelGradImg(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			CImageBuffer *pImgDst,
			CRect *prcDst,
			CRect* prcGradStep,
			BYTE nThresVal,
 			int nMaxWhite);


int BinariseGradImg(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
			 CImageBuffer *pImgMask,
			 CRect *prcMask,
 			 int nColor,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 CSize* pcsGradStep,
			 BYTE nThresVal);

int BinariseGradLineImg(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
  			 int nColor,
			 CImageBuffer *pImgMask,
			 CRect *prcMask,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 CSize* pcsGradStep,
			 BYTE nThresVal);
int BinariseGradLineImg(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
  			 int nColor,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 CRect* prcGradStep,
			 BYTE nThresVal);

int ImgSetPixelVal(CImageBuffer* pImg,
			   CRect* prc,
			   CPoint* ppt,
			   int nNo,
			   CPoint* pptOffset,
			   BYTE byteVal);

int ImgLine(CImageBuffer* pImgBufBin,
			CRect* prcBin,
			CPoint* pptStart,
			CPoint* pptEnd,
			BYTE byteVale);



int ProjMaxXVal(CImageBuffer *pImgSrc,
				  CRect *prcSrc,	
				  CImageBuffer *pImgMsk,	
				  CRect *prcMsk,	
				  int *pnAry);		

int BinariseGradImg(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
 			 int nColor,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 CSize* pcsGradStep,
			 BYTE nThresVal);

int BinariseGradLineImg(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
  			 int nColor,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 CSize* pcsGradStep,
			 BYTE nThresVal);


int ProjAvgXVal(CImageBuffer *pImgSrc,
				  CRect *prcSrc,	
				  CImageBuffer *pImgMsk,	
				  CRect *prcMsk,	
				  int *pnAry);		


int ImgGradY(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			int nStepY,
			CImageBuffer *pImgDst,
			CRect *prcDst);

int ImgGradYForLineEdgeType(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			CImageBuffer *pImgMask,
			CRect *prcMask,
			int nStepY,
			CImageBuffer *pImgDst,
			CRect *prcDst);


int ImgGradYForLineEdgeType(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			int nBlobColor,
			int nStepY,
			CImageBuffer *pImgDst,
			CRect *prcDst);

int CheckLoopTimeOut(CPerformanceTimer* pTimerInspTimeOutExt,
					 int nLoopIndex,
					 int nCheckPeriod,
					 double dInspTimeOut);

int ImgSegmentCopy(CImageBuffer* pImgSrc,
				CRect* prcPeriod,
				CRect* prcSegment,
				CImageBuffer* pImgSegment,
				CRect* prcSegmentRelative);

int GetHoughTransformLine(CArray<CPoint2D,CPoint2D> * pAryPt2DSrc,
				double dNominalAngleRad,
				double dTolAngleRad,
				double dAngleStep,
				double dOnLineTol,
				double* pdOnLinePercentage,
				CLine2D* pln2DHT);

int GetHoughTransformLine(CArray<CPoint2D,CPoint2D> * pAryPt2DSrc,
				double dNominalAngleRad,
				double dTolAngleRad,
				double dAngleStep,
				double dOnLineTol,
				int* pnOnLinePercentage,
				CLine2D* pln2DHT);


int GetHoughTransformLine(CArray<CPoint,CPoint> * pAryPtSrc,
				double dNominalAngleRad,
				double dTolAngleRad,
				double dAngleStep,
				double dOnLineTol,
				double* pdOnLinePercentage,
				CLine2D* pln2DHT);

int ImgFiltering(IMAGEVIEW_INFO *pView,
			 CImageBuffer *pImgBin,
			 CRect* prcUnwrap,
			 int nBlobColor,
			 int nMinArea,
			 int nMinX,
			 int nMinY,
			 CImageBuffer* pImgBinFiltering);

int ImgFiltering(IMAGEVIEW_INFO *pView,
			 CImageBuffer *pImgBin,
			 CRect* prcUnwrap,
			 int nBlobColor,
			 CSize* pcsArea,
			 CSize* pcsX,
			 CSize* pcsY,
			 CImageBuffer* pImgBinFiltering);

int ImgFiltering(IMAGEVIEW_INFO *pView,
			 CImageBuffer *pImgBin,
			 CRect* prcUnwrap,
			 int nBlobColor,
			 int nMinArea,
			 int nMinX,
			 int nMinY,
			 double nMinLength,
			 double nMinElongation,
			 CImageBuffer* pImgBinFiltering);

int ImgDIV(CImageBuffer *pImg, CRect *prectROI, BYTE byteVal);

int ImgDIV(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			int nDivVal,
			CImageBuffer *pImgDst,
			CRect *prcDst);

int ImgDebugADD(CImageBuffer* pImgSrc0,
				CRect* prcSrc0,
				BYTE byteVale0,
				CImageBuffer* pImgSrc1,
				CRect* prcSrc1,
				BYTE byteVale1,
				CImageBuffer* pImgDst,
				CRect* prcDst);

int GetHorLineCenterPoints(CImageBuffer *pImg,
						  CRect *prc,
						  CPoint2D* ppt2D,
						  int* pnNo);

int GetVerLineCenterPoints(CImageBuffer *pImg,
						  CRect *prc,
						  CPoint2D* ppt2D,
						  int* pnNo);

int GetHorLineCenterPoints(CImageBuffer *pImg,
						  CRect *prc,
						  CRect* prcROI,
						  CPoint2D** pppt2D,
						  int* pnNo);

int GetVerLineCenterPoints2(CImageBuffer *pImg,
						  CRect *prc,
						  CPoint2D** pppt2D,
						  int* pnNo);

int ImgSetPixelValAry(CImageBuffer* pImgSrc,
					CRect* prcSrc,
					CImageBuffer* pImgDst,
					CRect* prcDst,
					CPoint2D* ppt2DAry, 
					int nPtAryNo,
					BYTE byteVale);

int ImgWrapCopy(CImageBuffer *pImgSrc,
				CRect* prcFOVSrc,
				CRect* prcSrc,
				CImageBuffer* pImgDst,
				CRect* prcDst);

int ImgWrapCopy(CImageBuffer *pImgSrc,
				CRect* prcSrc,
				CImageBuffer* pImgDst,
				CRect* prcFOVDst,
				CRect* prcDst);

int GetSeedsFillImage(CImageBuffer *pImgBinSrc,
					 CRect* prcBinSrc,
					 CImageBuffer *pImgBinSeed,
					 CRect* prcBinSeed,
					 CPoint* pptBinSeedTopLftInrcBinSrc,
					 int nBlobType,
					 CImageBuffer *pImgBinDst,
					 CRect* prcBinDstRelative,
					 CPoint* pptBinDstTopLftInrcBinSrc);

int GetSeedsFillImage(CImageBuffer *pImgBinSrc,
					 CRect* prcBinSrc,
					 CPoint* pptSeed,
					 int nSeed,
					 int nBlobType,
					 CImageBuffer *pImgBinDst,
					 CRect* prcBinDstRelative,
					 CPoint* pptBinDstTopLftInrcBinSrc);

int GetSeedsFillImage(CImageBuffer *pImgBinSrc,
				 CRect* prcBinSrc,
				 CPoint* pptSeed,
				 int nSeed,
				 BYTE byteFilledVal);

int GetSeedsFillImageHole(CImageBuffer *pImgBinSrc,
					 CRect* prcBinSrc,
					 CPoint* pptSeed,
					 int nSeed,
					 int nBlobType,
					 CImageBuffer *pImgBinDst,
					 CRect* prcBinDstRelative,
					 CPoint* pptBinDstTopLftInrcBinSrc);



int GetSeedsFillImage(CImageBuffer *pImgBinSrc,
					 CRect* prcBinSrc,
					 long* plSeedAddress,
					 int nSeedAddress,
					 int nBlobType,
					 CImageBuffer *pImgBinDst,
					 CRect* prcBinDstRelative,
					 CPoint* pptBinDstTopLftInrcBinSrc);

int GetMaxOverlapSeedBlob(IMAGEVIEW_INFO *pView,
						 CImageBuffer *pImgBinSrc,
						 CRect* prcBinSrc,
						 CImageBuffer *pImgBinSeeds,
						 CRect* prcBinSeeds,
						 long lDebugFlag);

int GetMaxSingleBlobSeedFillImage(IMAGEVIEW_INFO *pView,
								 CImageBuffer *pImgBinSrc,
								 CRect* prcBinSrc,
								 CImageBuffer *pImgBinSeeds,
								 CRect* prcBinSeeds,
								 CPoint ptBinSeedsTopLftInrcBinSrc,
								 int nBlobType,
								 CImageBuffer *pImgBinDst,
								 CRect* prcBinDstRelative,
								 CPoint* pptBinDstTopLftInrcBinSrc,
								 long lDebugFlag);

int GetMorphologyImg(CImageBuffer* pImgSrc,
				   CRect* prcSrc,
				   CSize* pcsKernel,
				   int nMorphologyType,
				   CImageBuffer* pImgMorphology,
				   CRect* prcMorphology);

int GetContrastImg(CImageBuffer* pImgSrc,
				   CRect* prcSrc,
				   CSize* pcsKernel,
				   int nMorphologyType,
				   CImageBuffer* pImgContrast,
				   CRect* prcContrast);

int GetContrastImg(CImageBuffer* pImgSrc,
				   CRect* prcSrc,
				   CSize* pcsKernel,
				   int nMorphologyType,
				   CImageBuffer* pImgMsk,
				   CImageBuffer* pImgContrast,
				   CRect* prcContrast);

int GetContrastImgBySampling(CImageBuffer* pImgSrc,
				   CRect* prcSrc,
				   CSize* pcsKernel,
				   int nMorphologyType,
				   CImageBuffer* pImgMsk,
				   CImageBuffer* pImgContrast,
				   CRect* prcContrast,
				   CSize* pcsSampling);

int BOOLAryDilation1D(BOOL* pbVal,
					int nValNo,
					BOOL bBlobType,
					BOOL* pbDilationVal,
					int nDilationValNo,
					int nDilationSize);

class CSlidingAverage
{
public:
	CSlidingAverage();
	~CSlidingAverage();
	void Clean();
	int ImgSlidingAverage(CImageBuffer *pImgSrc,
				CRect* prcSrc,
				CSize* pcsProj,
				CImageBuffer* pImgDst,
				CRect* prcDst);

	int ImgSlidingAverage(CImageBuffer *pImgSrc,
				CRect* prcSrc,
				CImageBuffer* pImgMask,
				CRect* prcMask,
				CSize* pcsProj,
				CImageBuffer* pImgDst,
				CRect* prcDst);
private:
	int GetXGrayAcclWithMask(CImageBuffer* pImgSrc, 
				CRect* prcSrc, 
				CImageBuffer* pImgMask, 
				CRect* prcMask, 
				long* plXAry,
				long* plWeightAry);

	int GetYGrayAcclWithMask(CImageBuffer* pImgSrc, 
				CRect* prcSrc, 
				CImageBuffer* pImgMask, 
				CRect* prcMask, 
				long* plYAry,
				long* plWeightAry);

	int ImgSlidingAverageForEdge(CImageBuffer *pImgSrc,
				CRect* prcSrc,
				CSize* pcsProj,
				CImageBuffer* pImgDst,
				CRect* prcDst);
	int ImgSlidingAverageForEdge(CImageBuffer *pImgSrc,
					CRect* prcSrc,
					CSize* pcsProj,
					int nSide,
					CImageBuffer* pImgDst,
					CRect* prcDst);

	int ImgSlidingAverageForEdge(CImageBuffer *pImgSrc,
				CRect* prcSrc,
				CImageBuffer* pImgMask,
				CRect* prcMask,
				CSize* pcsProj,
				CImageBuffer* pImgDst,
				CRect* prcDst);
	int ImgSlidingAverageForEdge(CImageBuffer *pImgSrc,
				CRect* prcSrc,
				CImageBuffer* pImgMask,
				CRect* prcMask,
				CSize* pcsProj,
				int nSide,
				CImageBuffer* pImgDst,
				CRect* prcDst);



};

class CBlob1D
{
public:
	CBlob1D();
	~CBlob1D();
	void Clean();

	int Inspect(BOOL* pbVal,
				int nValNo,
				BOOL bBlobVal,
				int nMinBlobLength,
				int nMaxBlobLength);
	int Inspect(CArray<BOOL, BOOL>* pArybVal,
				BOOL bBlobVal,
				int nMinBlobLength,
				int nMaxBlobLength);
	int Dilation(CArray<BOOL, BOOL>* pArybVal,
				int nDilationSize);
	int Dilation(BOOL* pbVal,
				int nValNo,
				int nDilationSize);
	int Erosion(CArray<BOOL, BOOL>* pArybVal,
				int nDilationSize);
	int Erosion(BOOL* pbVal,
				int nValNo,
				int nErosionSize);
	int AND(BOOL* pbValSrc0,
			BOOL* pbValSrc1,
			BOOL* pbValDst,
			int nValNo);
	int GetBlobRangebySeed(BOOL* pbVal,
				int nValNo,
				int nPos,
				CPoint* pptRange);
public:
	CArray<CPoint,CPoint> AryPtBlobRange;
};

class CPointArrayMatching
{
public:
	CPointArrayMatching();
	~CPointArrayMatching();
	void Clean();
	int Inspect(CArray<CPoint2D, CPoint2D>* pAryTempl,
				CArray<CPoint2D, CPoint2D>* pAryInsp,
				CArray<CPoint, CPoint>* pAryMatchingIndex,
				CArray<CPoint2D, CPoint2D>* pAryTemplMappingToInsp,
				int nMinMatchingPercentageTol,
				CSize* pcsMatchingTol,
				CSize* pcsShiftTol,
				CPoint2D* ppt2DFoundBestShift);

	int Inspect(CArray<CPoint2D, CPoint2D>* pAryTempl,
				CArray<CPoint2D, CPoint2D>* pAryInsp,
				CArray<CPoint, CPoint>* pAryMatchingIndex,
				CArray<CPoint2D, CPoint2D>* pAryTemplMappingToInsp,
				CSize* pcsMatchingTol,
				CSize* pcsShiftTol,
				CPoint2D* ppt2DFoundBestShift);


public:
	CLine2D ln2DZoomX;
	CLine2D ln2DZoomY;

private:

	int InspectByRegion(CArray<CPoint2D, CPoint2D>* pAryTempl,
				CArray<CPoint2D, CPoint2D>* pAryInsp,
				CArray<CPoint, CPoint>* pAryMatchingIndex,
				CSize* pcsMatchingTol,
				CSize* pcsShiftTol,
				CPoint2D* ppt2DFoundBestShift,
				CArray<CPoint2D, CPoint2D>* pAryTemplNotMatched,
				CArray<CPoint2D, CPoint2D>* pAryInspNotMatched);


};
class CSinCosTabAry
{
public:
	CSinCosTabAry();
	~CSinCosTabAry();
	void Clean();
	int Create(double dAngleDegStep = 1.0);
	CArray<CPoint2D,CPoint2D> SinCosTabAry;
	CArray<CPoint2D,CPoint2D> SinCosTab360Ary;
	BOOL bSinCosTabCreated;
};
class CImageUnwrapInfo
{
public:
	CImageUnwrapInfo();
	~CImageUnwrapInfo();
	void Clean();
	int ImgBinUnwrap(IMAGEVIEW_INFO* pView, 
			 CImageBuffer* pImgSrc,
			 CPoint2D* ppt2DCenter,
			 int nInnerRadius,
			 int nOuterRadius,
			 int nBlobColor,
			 CSize* pcsSampling,
			 long lDebugFlag,
			 int nProcMode = 0);

//	int ImgWrap(CImageBuffer* pImgBufUnwrap,
//			    CImageBuffer* pImgBufWrap);

	int ImgWrap(CImageBuffer* pImgBufUnwrap,
			    CImageBuffer* pImgBufWrap);

	int WrapPosToUnwrapPos(CPoint* pptWrap,
						 CPoint* pptUnWrap);

	int UnwrapPosToWrapPos(CPoint* pptUnWrap,
						 CPoint* pptWrap);
	int UnwrapPosToWrapPos(CPoint2D* ppt2DUnWrap,
						 CPoint2D* ppt2DWrap);
	int UnwrapRectToWrapRect(CRect* prcUnwrap, CRect* prcWrap);

public:
	CImageBuffer ImgUnwrap;
	CRect rcUnwrap;
	CPoint** ppptMappingTab;
	int nUnwrapWidth;
	int nUnwrapHeight;
	double dAngleStep;
	double dAngleStepSampling;
	CPoint2D pt2DCenterOrg;
	int nInnerRadiusOrg;
	int nOuterRadiusOrg;
	CSize csSampling;
private:
	int ImgBinUnwrapWithoutInterpolation(IMAGEVIEW_INFO* pView, 
			 CImageBuffer* pImgSrc,
			 CPoint2D* ppt2DCenter,
			 int nInnerRadius,
			 int nOuterRadius,
			 int nBlobColor,
			 CSize* pcsSampling,
			 long lDebugFlag);
	int ImgBinUnwrapWithInterpolation(IMAGEVIEW_INFO* pView, 
			 CImageBuffer* pImgSrc,
			 CPoint2D* ppt2DCenter,
			 int nInnerRadius,
			 int nOuterRadius,
			 int nBlobColor,
			 CSize* pcsSampling,
			 long lDebugFlag);
};


class CImageEdgeEnhance
{
public:
	CImageEdgeEnhance();
	~CImageEdgeEnhance();
	int Inspect(IMAGEVIEW_INFO *pView,
				 CImageBuffer *pImgBufSrc,
				 CRect* prcSrc,
				 CImageBuffer *pImgBufDst,
				 CRect* prcDst,
				 long lDebugFlag);
	int m_nContrastMaxTol;
	int m_nContrastMinTol;
	int m_nMagnifyTimes;
private:
	int nMaxThreshold;
	int nMinThreshold;
	int ImgMagnify(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			CImageBuffer *pImgDst,
			CRect *prcDst,
			int nTimes,
			int nMaxThreshold);

};

class CCircleInfo
{
public:
	CCircleInfo();
	~CCircleInfo();
	void Clean();

	int OffsetPos(CPoint* pptOffset);

	int Inspect(CPoint* pptContour,
				int nPtContourNo,
				int nProcMode = -1,
				int nAngleMin = 45,
				int nAngleMax = 180);

	int Inspect(CPoint* pptContour,
				int nPtContourNo,
				CPoint* pptEstimatedCenter,
				double dEstimatedRadius,
				int nProcMode,
				int nAngleMin = 45,
				int nAngleMax = 180);

	int SetInvalidEdge(double* pdRadius360,
						double dRadiusDifTol);
//////////////////////////////////
// Not recommended to use the below function
	int Inspect2(CPoint* pptContour,
				int nPtContourNo,
				CPoint* pptEstimatedCenter,
				double dEstimatedRadius,
				int nProcMode,
				int nAngleMin,
				int nAngleMax);
///////////////////////////////////////////////
	int PointSortingByAngle(CPoint* pptContour,
				int nPtContourNo,
				CPoint2D* ppt2DCtr,
				int nProcMode,
				CArray<CPoint,CPoint>* pAryPtContourSortingByAngle,
				int nAngleDif);
	int PointSortingByAngle(CPoint* pptContour,
				int nPtContourNo,
				CPoint2D* ppt2DCtr,
				int nProcMode,
				int nAngleDif);

	int InspectNoSampling(CPoint* pptContour,
				int nPtContourNo,
				int nProcMode,
				CArray<CPoint,CPoint>* pAryPtContourSortingByAngle,
				int nAngleDif = 45);


	int GetSegmentPtsInRing(double dRadiusDifPercentage,
						CPoint2D* ppt2DMask);

	int GetRadiusvar(CPoint* pptContour,
				int nPtContourNo,
				double dStartPercentage,
				double dEndPercentage,
				double* pdRadiusMaxVar);

	int ModifySegmentPt(int nRadiusDifTol);

	int GetAvgRadiusPercentage(double dMinRadius,
					 double dMaxRadius,
					 double* pdRadiusPercentage);

	int GetMaxBlobRadiusPercentage(double dMinRadius,
					 double dMaxRadius,
					 double* pdRadiusPercentage);

	int GetPercentageByLineFitting(double dRadiusDifTol,
						   int nClosingSize,
						   int* pnConnectedPercentage,
						   int* pnPercentageAll);

	int Deflate(double dDeflateRadius,
				CArray<CPoint, CPoint>* pAryPtDeflatePos);

	int Inflate(double dInflateRadius,
				CArray<CPoint, CPoint>* pAryPtInflatePos);

	int Draw(IMAGEVIEW_INFO* pView, CPoint ptOffset, BOOL bSegments, LOGPEN* pPenInfo, int nSampling = 1);

	int Merge(CCircleInfo* pCircleInfo,
			  int nMaxOverlapAngleDeg,
			  double dRadiusDifTol,
			  BOOL* pbMergedOK);

	int GetPercentage(double dRadiusDifTol,
				   int* pnPercentage);
	CCircleInfo & operator =(CCircleInfo CircleInfo);

	int GetAvgRadius(int nStartPercentage,
					 int nEndPercentage,
					 double* pdAvgRadius);

	int GetAvgRadius(CPoint2D* ppt2DCtr,
					 int nStartPercentage,
					 int nEndPercentage,
					 double* pdAvgRadius);

	int SegmentEdge(CPoint* pptContour,
				int nPtContourNo,
				CPoint2D* ppt2DRoughCenter);

	int GetPercentageByValidEdge(double dRadiusDifTol,
						   int* pnPercentage);


public:
	CPoint ptSegmentEdge[360];
	BOOL pbSegmentEdge[360];
	double dSegmentRadius[360];
	int FindCenterByHorAndVerLine(CPoint* pptContour,
				int nPtContourNo);
	CPoint2D pt2DCenter;
	double dRadius;
	int* pnRadiusSqr;
	int nRadiusNo;
	CRect rcBounding;
	int nCircleMissingPercentage;
	CPoint ptFOVRef;
	int nEdgeProcMode;
private:
	int nEstimatedRadius;
	int nPercentageMergeOrg;
	int nAngleDifInsp;
	CRect rcBoundingRefCenter;
	CPoint2D pt2DCenterAry[4];
	int nCenterAryNo;
	int EdgeInterpolationForInvalidPoint();
	int SegmentEdgeNoSampling(CPoint* pptContour,
			int nPtContourNo,
			CPoint2D* ppt2DRoughCenter,
			CArray<CPoint,CPoint>* pAryPtContourSortingByAngle);
	int SegmentEdgeNoSampling(CPoint* pptContour,
			int nPtContourNo,
			CPoint2D* ppt2DRoughCenter);
	int EdgeInterpolation();
	int FindCircleRadius(CPoint2D* ppt2DCenter,
					  int* pnMaxHist);
	int FindCircleCenter();
	int FindCircleCenter2(int nAngleDif);
	int FindCircleCenter3(int nAngleDif);
	int FindCircleCenter4(int nAngleMin, int nAngleMax);

	int SegmentEdgeForMerge(CCircleInfo* pCircleInfo,
							int* pnFndOverlapDeg);
	int Backup(CCircleInfo* pCircleInfo);
	int Restore(CCircleInfo* pCircleInfo);

	int GetValidRadiusNo(double dRadiusDifTol,
						 int* pnValidRadiusNo);

	int CheckPtConnection(CPoint* pptContour,
					int nPtContourNo,
					BOOL* pbContourConnected);

	int InitResult();

};

class CScanColCenter
{
public:
	CScanColCenter();
	~CScanColCenter();
	void Clean();
	int InspectWrap(CImageBuffer *pImg,
					CRect *prc,
					CRect* prcROI,
					BOOL bEdgeColor = WHITE);

	int FindCtrByMinDistance(CPoint2D* ppt2DFittingCtr,
					 int* pnSelectedIndex,
					 BOOL* pbSelectCtr);

	int FindCtrByMaxDistance(CPoint2D* ppt2DFittingCtr,
					 int* pnSelectedIndex,
					 BOOL* pbSelectCtr);

public:

	int GetColNumber();
	int GetCenter(int nCol, 
				  int nIndex,
				  CPoint2D* ppt2DCenter);

	int GetTopPoint(int nCol, 
				  int nIndex,
				  CPoint* pptTop);

	int GetBotPoint(int nCol, 
				  int nIndex,
				  CPoint* pptBot);

	int GetEdgeNumber(int nCol, int* pnColEdgeNo);
	
	int GetBotMostPoint(int nCol, 
				  CPoint* pptBotMost);
	
	int GetTopMostPoint(int nCol, 
				  CPoint* pptTopMost);

private:
	CArray<CPoint,CPoint> PtEdgeTopAry;
	CArray<CPoint,CPoint> PtEdgeBotAry;
	CArray<CPoint2D,CPoint2D> Pt2DEdgeCtrAry;
	CArray<CPoint,CPoint> PtEdgeColIndexAry;
	int nColNo;
	int GetHorLineCenterPoints(CImageBuffer* pImg,
						  CRect* prc,
						  CRect* prcROI,
						  BOOL bEdgeColor=WHITE);

};

class CScanRowCenter
{
public:
	CScanRowCenter();
	~CScanRowCenter();
	void Clean();
	int InspectWrap(CImageBuffer *pImg,
					CRect *prc,
					CRect* prcROI,
					BOOL bEdgeColor = WHITE);

	int Inspect(CImageBuffer *pImg,
				CRect *prc);

	int FindCtrByMinDistance(CPoint2D* ppt2DFittingCtr,
					 int* pnSelectedIndex,
					 BOOL* pbSelectCtr);

	int FindCtrByMaxDistance(CPoint2D* ppt2DFittingCtr,
					 int* pnSelectedIndex,
					 BOOL* pbSelectCtr);

	int FindRightByMinDistance(CPoint* pptFitting,
						 int* pnSelectedIndex,
						 BOOL* pbSelectIndex);

	int FindLeftByMinDistance(CPoint* pptFitting,
						 int* pnSelectedIndex,
						 BOOL* pbSelectIndex);

public:

	int GetRowNumber();
	int GetCenter(int nRow, 
				  int nIndex,
				  CPoint2D* ppt2DCenter);

	int GetLeftPoint(int nRow, 
				  int nIndex,
				  CPoint* pptTop);

	int GetRightPoint(int nRow, 
				  int nIndex,
				  CPoint* pptBot);

	int GetEdgeNumber(int nRow, int* pnColEdgeNo);
	
	int GetRightMostPoint(int nRow, 
				  CPoint* pptBotMost);
	
	int GetLeftMostPoint(int nRow, 
				  CPoint* pptTopMost);

private:
	CArray<CPoint,CPoint> PtEdgeTopAry;
	CArray<CPoint,CPoint> PtEdgeBotAry;
	CArray<CPoint2D,CPoint2D> Pt2DEdgeCtrAry;
	CArray<CPoint,CPoint> PtEdgeRowIndexAry;
	int nRowNo;
	int GetVerLineCenterPoints(CImageBuffer* pImg,
						  CRect* prc,
						  CRect* prcROI,
						  BOOL bEdgeColor=WHITE);

};

/*
class CScanRowCenter
{
public:
	CScanRowCenter();
	~CScanRowCenter();
	void Clean();
	int Inspect(CImageBuffer *pImg,
				CRect *prc);

public:
	int GetCenter(int nRow, 
				  int nIndex,
				  CPoint2D* ppt2DCenter);
	int GetEdgeNumber(int nRow, int* pnColEdgeNo);
private:
	CPoint2D** pppt2DEdgeCtr;
	int* pnEdgeCtrRowNo;
	int nRowNo;
};
*/

/*class CInterpolationBy2Pts
{
public:
	CInterpolationBy2Pts();
	~CInterpolationBy2Pts();
	void Clean();
	int SetPoint2DAry(CPoint2D* pPt2D,
					  int nPt2DNo);
	int SetPointAry(CPoint* pPt,
					  int nPtNo);
	int GetY(double dx, 
			 double* pdInterpolationy);

	int GetYByMinDisPts(double dx, 
			 double* pdInterpolationy);

public:
	int nNo;
	CPoint2D* ppt2DSorting;
private:
	int* pnMappingIndex;
	int nMappingNo;
	double* pdx;
	double* pdy;
	int GetIndex(double dx,
			 int* pnIndex);
};
*/


class CPOLYGON_LINE
{
public:
	CPOLYGON_LINE();
	~CPOLYGON_LINE();
	CPOLYGON_LINE & operator =(CPOLYGON_LINE PolygonLine);
	CPoint pt0;
	CPoint pt1;
};

class CAngleLookupTable
{
public:
	CAngleLookupTable();
	~CAngleLookupTable();
	void Clean();
	int GetAngle(CPoint* pptOrg,
			   CPoint* pptDst,
			   double* pdAngle);
private:
	double dTanTab[10000];
	double dAngleStep;
};

class CPOLYGON_POINT_INFO
{
public:
	CPOLYGON_POINT_INFO();
	~CPOLYGON_POINT_INFO();
	CPoint ptEdge;
	double dDis;
	CPOLYGON_POINT_INFO & operator =(CPOLYGON_POINT_INFO PolygonPointInfo);
};

class CPOLYGON_LINE_INFO
{
public:
	CPOLYGON_LINE_INFO();
	~CPOLYGON_LINE_INFO();
	void Clean();
	CPOLYGON_POINT_INFO* pPolygonPointInfo;
	int nPolygonPointInfo;
	int GetData(CPoint* pptEdge,
				int nEdgeNo,
				int nStartIndex,
				int nEndIndex,
				CPoint* pptReference);
	int GetPolygonPt2(int nPreAngle,
					 int nMaxPolygonLength,
					 CPoint* pptPolygon);
	int GetPolygonPt(int nMaxPolygonLength,
					 CPoint* pptPolygon);
	int nFoundAngle;
private:
	int nTanTab[1001];
	double dAngleStep;
	int GetAngle(CPoint* pptOrg,
			   CPoint* pptDst,
			   int* pnAngle);
};



class CPOLYGON
{
public:
	CPOLYGON();
	~CPOLYGON();
	void Clean();
	CArray <CPoint,CPoint> ptPolygonArray;
	int GetPolygonLine(CPoint* pptEdge,
					  int nEdgeNo,
					  int nMaxPolygonLength);

	CPOLYGON_LINE* pPolygonLine;
	int nPolygonLine;
private:
	CPOLYGON_LINE_INFO PolygonLineinfo[4];
	CPoint ptReference[4];
	CPoint ptLftMost;
	CPoint ptTopMost;
	CPoint ptRhtMost;
	CPoint ptBotMost;
	CPoint ptTopLft;
	CPoint ptTopRht;
	CPoint ptBotLft;
	CPoint ptBotRht;
	int GetPolygonInfo(CPoint* pptEdge,
					   int nEdgeNo);

};

class CSingleBlobInfo
{
public:
	CSingleBlobInfo();
	~CSingleBlobInfo();
	void Clean();
	BOOL bSelected;
	int nBlobIndex;
	int nStartAngleDeg[2];
	int nEndAngleDeg[2];
	int nAngleRange;
	CPoint ptEdge[360];
	BOOL bEdgePt[360];
	int Inspect(CAdvBlobFind* pBlobFinder,
				int nSingleBlobIndex,
				CPoint2D* ppt2DLensRoughCenter,
				int nLensNominalRadius);
	int InterpolationContour();
};

/*
class CLensBlobLabeling
{
public:
	CLensBlobLabeling();
	~CLensBlobLabeling();
	void Clean();
	void CleanLabel();
	int GetBlobLabelImg(IMAGEVIEW_INFO *pView,
				CImageBuffer* pImgGradBinFiltering,
				CRect* prcSearchRelative,
				long lDebugFlag);
	
	int DrawLensEdge(IMAGEVIEW_INFO *pView,
				LOGPEN* pPen,
				CPoint ptOffset,
				long lDebugFlag);
	int DrawLensEdge2(IMAGEVIEW_INFO *pView,
				LOGPEN* pPen,
				CPoint ptOffset,
				int nStartingPoint,
				int nTotalPoint);
	
	int GetLensEdgePoints(IMAGEVIEW_INFO *pView,
				CPoint ptOffset,
				CPoint *pptEdge);

public:
	CAdvBlobFind BlobFinder;
	CImageBuffer ImgLabel;
	CRect rcLabel;
	int* pnLabelToBlobIndex;
	int nLabelNo;
	int* pnBlobIndexToLabel;
	int nBlobNo;
	CAdvBlobFind* pSegmentBlobFinder;
	CArray<CPoint, CPoint> SegmentIndexAndLabelAry;
	int CheckLensSegment(CPoint2D* ppt2DLensRoughCenter,
						 int nLensNominalRadius);

	int CheckLensSegment2(CPoint2D* ppt2DLensRoughCenter,
						 int nBlobIndex,
						 int nLensNominalRadius);

	CRect rcLensBounding;
	CPoint ptCenter;
	CSingleBlobInfo* pSingleBlobInfo;
	int nSingleBlobInfoNo;
	int nMatchingPercentage;

	BOOL bBroken;
	int nRadius[36];
	int nRadiusMaxVar;
	int nNominalRadius;
private:
	BOOL bAngleFilled[360];
	CPoint ptEdge[360];
	int GetMappingIndex();

	int GetSegmentRadius();
	int BlobDetection(IMAGEVIEW_INFO *pView,
					CImageBuffer* pImgGradBinFiltering,
					CRect* prcSearchRelative,
					long lDebugFlag);

};
*/


class CLensBlobLabeling
{
public:
	CLensBlobLabeling();
	~CLensBlobLabeling();
	void Clean();
	void CleanLabel();
	int BlobDetection(IMAGEVIEW_INFO *pView,
				CImageBuffer* pImgGradBinFiltering,
				CRect* prcSearchRelative,
				long lDebugFlag);

	int GetBlobLabelImg(IMAGEVIEW_INFO *pView,
				CImageBuffer* pImgGradBinFiltering,
				CRect* prcSearchRelative,
				long lDebugFlag);
	
	int DrawLensEdge(IMAGEVIEW_INFO *pView,
				LOGPEN* pPen,
				CPoint ptOffset,
				long lDebugFlag);
	int DrawLensEdge2(IMAGEVIEW_INFO *pView,
				LOGPEN* pPen,
				CPoint ptOffset,
				int nStartingPoint,
				int nTotalPoint);

	int GetLabelingImg(CImageBuffer* pImgLabel,
					CRect* prcLabel,
					CImageBuffer* pImgLabelBin,
					CRect* prcBin,
					int nLabelIndex);

public:
	CImageBuffer ImgLabel;
	CRect rcLabel;
	int* pnLabelToBlobIndex;
	int nLabelNo;
	int* pnBlobIndexToLabel;
	int nBlobNo;
	CAdvBlobFind* pSegmentBlobFinder;
	CRect* prcSegmentBlobFinder;
	CArray<CPoint, CPoint> SegmentIndexAndLabelAry;
	int CheckLensSegment(CPoint2D* ppt2DLensRoughCenter,
						 int nLensNominalRadius);
	CRect rcLensBounding;
	CPoint ptCenter;
	CSingleBlobInfo* pSingleBlobInfo;
	int nSingleBlobInfoNo;
	int nMatchingPercentage;

	BOOL bBroken;
	int nRadius[36];
	int nRadiusMaxVar;
	int nNominalRadius;

	BOOL bBlobWrapConnection;
	int nMostLeftBlobIndex;
	int nMostRightBlobIndex;
	int nShareLabel;
private:
	CAdvBlobFind BlobFinder;
	BOOL bAngleFilled[360];
	CPoint ptEdge[360];
	int GetMappingIndex();

	int GetSegmentRadius();
};

class CPoint2DAndInt
{
public:
	CPoint2D pt2DPos;
	int nVal;
};

class CCurvePointStatistics
{
public:
	CCurvePointStatistics();
	~CCurvePointStatistics();
	void Clean();
	int Inspect(CPoint2D* ppt2D,
				int* pnPercentage,
				int nPercentageTol,
				double dPosTol,
				int nPtNo);

	int Inspect1(CPoint2D* ppt2D,
				int* pnPercentage,
				int nPercentageTol,
				CPoint2D* ppt2DDefectAvg,
				double dPosTol,
				int nPtNo);

public:
	CPoint2D* ppt2DHough;
	int nPtNoHough;
private:
	BOOL* pbHough;
	int CalculateInterpolation(CPoint2D* ppt2D,
					int* pnPercentage,
					int nPercentageTol,
					double dPosTol,
					int nPtNo);

	int EdgePtAndBadPtInterpolation(CPoint2D* ppt2D,
					int* pnPercentage,
					int nPercentageTol,
					double dPosTol,
					int nPtNo);

	int InterpolationBy3Pts(CPoint2DAndInt* pPoint2DAndInt0,
					CPoint2DAndInt* pPoint2DAndInt1,
					CPoint2DAndInt* pPoint2DAndInt2,
					CPoint2DAndInt* pPoint2DAndIntInsp);

	int EdgePtAndBadPtInterpolation();
};

class CCurveFitting
{
public:
	CCurveFitting();
	~CCurveFitting();
	void Clean();
	int GetCurveFitting(CPoint2D* ppt2DEdge,
						int nPt2DEdgeNo,
						BOOL bHor,
						int nSegmentLength,
						int nSegmentPercentage);


	int CheckChipoff(CImageBuffer* pImgBin,
						CRect* prcBin,
						CPoint2D* ppt2DEdgeAll,
						int nPt2DEdgeAllNo,
						CPoint2D* ppt2DEdgeDefect,
						int nPt2DEdgeDefectNo,
						int nOffset,
						double dChipoffTol,
						BOOL* pbFoundChipoff);

	int CheckChipoff(CPoint2D* ppt2DEdgeAll,
						int nPt2DEdgeAllNo,
						CPoint2D* ppt2DEdgeDefect,
						int nPt2DEdgeDefectNo,
						int nOffset,
						double dChipoffTol,
						BOOL* pbFoundChipoff);

	int GetCurveFitting1(CPoint2D* ppt2DEdge,
						int nPt2DEdgeNo,
						CPoint2D* ppt2DDefectAvg,
						BOOL bHor,
						int nSegmentLength,
						int nSegmentPercentage);

	int GetCurveFitting2(CPoint2D* ppt2DEdge,
						int nPt2DEdgeNo,
						CPoint2D* ppt2DDefectAvg,
						BOOL bHor,
						int nSegmentLength,
						int nSegmentPercentage,
						int nSegmentPercentageTol = 40);

	int GetLineSegmentFitting(CPoint2D* ppt2DEdge,
						int nPt2DEdgeNo,
						BOOL bHor,
						int nSegmentLength,
						int nSegmentPercentage);

	int GetY(double dx, double* pdy);
	int GetDistance(CPoint2D* ppt2D, double* pdDis);
	int GetMappingDebugImage(CImageBuffer* pImgSrc,
						CRect* prcSrc,
						CImageBuffer* pImgDst,
						CRect* prcDst);

	int CheckFittingPercentage(CPoint2D* ppt2DEdge,
						int nPt2DEdgeNo,
						double dOffsetY,
						int* pnFittingPercentage);

	int CheckChipoffPercentage(CPoint2D* ppt2DEdge,
						int nPt2DEdgeNo,
						double dOffsetY,
						double* pdChipoffPercentage);

public:
	double dFOVZoomRatio;
	BOOL bLineSegmentMethod;
	int* pnPercentageByLength;
	int* pnPercentageByAllPoints;
	int nSegmentNo;
	CRect* prcSegment;
	CLine2D* pln2DSegment;
	int m_nSegmentLength;
	CInterpolationBy2Pts InterpolationBy2PtsSegment;
	CInterpolationBy3Pts InterpolationBy3PtsSegment;
	CRect rcROI;
	CSize csRangeX;
private:
	int nGoodLineSegmentNo;
	CCurvePointStatistics CurvePointStatistics;
	int m_nSegmentPercentage;
	CPoint2D** ppt2DEdgeSorting;
	int* pnPt2DInSegment;
	CPoint2D* ppt2DSegmentCtr;
	int nSegmentCtrNo;
	int SortingData(CPoint2D* ppt2DEdge,
					int nPt2DEdgeNo,
					BOOL bHor);
	int SegmentHoughTransform();
	int SegmentHoughTransform1(CPoint2D* ppt2DDefectAvg);
	int SegmentHoughTransform2(CPoint2D* ppt2DDefectAvg, int nSegmentPercentageTol);
	int GetFittingParm();
	int CreateInterpolationBy3Pts();
	int CreateInterpolationBy3Pts2();
	int CreateInterpolationBy2Pts();

};


class CPointMatching
{
public:
	CPointMatching();
	~CPointMatching();
	void CleanAll();
	void CleanTeach();
	void CleanInspection();
	int SetTeachPointAry(CPoint* pptTeachAry1,
						 int nPtArySize1);
	int Inspection(IMAGEVIEW_INFO *pView,
				   CImageBuffer *pImgBufSrc,
				   CRect* prcInsp,
				   int nBlobType,
				   CPoint* pptCircleShift,
				   CSize* pcsShift,
				   long lDebugFlag);

	int InspectionByContrast(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufSrc,
					   CRect* prcInsp,
					   CPoint* pptBestMatching,
					   CPoint* pptShift0,
					   CPoint* pptShift1,
					   int nContrast,
					   int nBlobType,
					   CSize* pcsShift,
					   long lDebugFlag);

	int InspectionByContrastRough(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufSrc,
					   CRect* prcInsp,
					   CPoint* pptBestMatching,
					   CPoint* pptShift0,
					   CPoint* pptShift1,
					   int nContrast,
					   int nBlobType,
					   CSize* pcsShift,
					   long lDebugFlag);

	int InspectionLabelByBinaryRough(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufSrc,
					   CRect* prcInsp,
					   CPoint* pptBestMatching,
					   int nBlobType,
					   CSize* pcsShift,
					   int nSegmentIndex,
					   CLensBlobLabeling* pLensBlobLabeling,
					   long lDebugFlag);

	int InspectionByBinaryRough(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufSrc,
					   CRect* prcInsp,
					   CPoint* pptBestMatching,
					   int nBlobType,
					   CSize* pcsShift,
					   long lDebugFlag);

	int InspectionBinaryAccumulation(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufSrc,
					   CRect* prcInsp,
					   CPoint* pptBestMatching,
					   int nBlobType,
					   CSize* pcsShift,
					   long lDebugFlag);

	int GetPtNumberByContrast(IMAGEVIEW_INFO *pView,
					CImageBuffer *pImgBufSrc,
					CRect* prcInsp,
					CPoint* pptBestMatching,
					CPoint* pptShift,
					int nContrast,
					int nBlobType,
					int* pnPtNumberByContrast,
					long lDebugFlag);

	int GetPtNumberByContrast(IMAGEVIEW_INFO *pView,
							CImageBuffer *pImgBufSrc,
							CRect* prcInsp,
							CPoint* pptBestMatching,
							CPoint* pptShift0,
							CPoint* pptShift1,
							int nContrast,
							int nBlobType,
							int* pnPtNumberByContrast,
							long lDebugFlag);
public:
	CImgIntBuf ImgIntBuf;
	long lBestMatchingSum;
	CPoint ptBestMatching;
	CPoint* pptTeachAry;
	int nptTeachNo;
	CPoint ptTeachCtr;
	CRect rcTeach;
	CLine2D ln2DTeach;
	CPoint ptOffset0;
	CPoint ptOffset1;
	int nPtNumberByContrast;
	int nPtPercentageByContrast;
private:
	int CreateAddress(int nPitch);
	int nPrePitch;
	long* plAddress;
	long** pplSum;
	CSize csRange;
};


class CEdgeMatching
{
public:
	CEdgeMatching();
	~CEdgeMatching();
	void CleanTeach();
	void CleanInspect();
	int Teach(CPoint* pptTeach,
			  int nPtTeachNo,
			  int nPtArySize);
	int Inspection(IMAGEVIEW_INFO *pView,
			   CImageBuffer *pImgBufSrc,
			   CRect* prcInsp,
			   int nBlobType,
			   CPoint* pptCircleShift,
			   CSize* pcsShift,
			   int nCircleWidth,
			   int nContrast,
			   long lDebugFlag);
	int InspectionRough(IMAGEVIEW_INFO *pView,
			   CImageBuffer *pImgBufSrc,
			   CRect* prcInsp,
			   int nBlobType,
			   CPoint* pptCircleShift,
			   CSize* pcsShift,
			   int nCircleWidth,
			   int nContrast,
			   long lDebugFlag);
	int InspectionRough2(IMAGEVIEW_INFO *pView,
			   CImageBuffer *pImgBufSrc,
			   CRect* prcInsp,
			   int nBlobType,
			   CPoint* pptCircleShift,
			   CSize* pcsShift,
			   int nCircleWidth,
			   int nContrast,
			   int nFoundMatchingScoreMax,
			   long lDebugFlag);

	int InspectionLabelBinaryRough(IMAGEVIEW_INFO *pView,
			   CImageBuffer *pImgBufSrc,
			   CRect* prcInsp,
			   int nBlobType,
			   CPoint* pptCircleShift,
			   CSize* pcsShift,
			   int nFoundMatchingScoreMax,
			   CLensBlobLabeling* pLensBlobLabeling,
			   long lDebugFlag);

	int InspectionBinaryRough(IMAGEVIEW_INFO *pView,
			   CImageBuffer *pImgBufSrc,
			   CRect* prcInsp,
			   int nBlobType,
			   CPoint* pptCircleShift,
			   CSize* pcsShift,
			   int nFoundMatchingScoreMax,
			   long lDebugFlag);

	int InspectionBinaryAccumulation(IMAGEVIEW_INFO *pView,
			   CImageBuffer *pImgBufSrc,
			   CRect* prcInsp,
			   int nBlobType,
			   CPoint* pptCircleShift,
			   CSize* pcsShift,
			   long lDebugFlag);
public:
	BOOL* pbFoundMatching;
	BOOL* pbInspect;
	CPointMatching* pPointMatching;
	int nPointMatchingNo;
	int nSumPtNumberByContrast;
	int nSumPtNumber;
private:
	int nCircleWidthTeach;
};

class CCircleFitting
{
public:
	CCircleFitting();
	~CCircleFitting();
	void Clean();
	int Inspect(CPoint* pptCircle,
				int nPtCircle,
				int nNominalRadius,
				CSize* pcsCenterShift,
				CRect* prcFOV);
public:
	CPoint2D pt2DBestCtr;
	double dBestRadius;
	double dRadiusVar;

	double dMaxRadius95Percentage;
	double dMinRadius5Percentage;
private:
	CRect rcCtrRange;
	int nDisTol;
	CSize csCenterRange;
	int nNominalRadiusVal;
	
private:
	int FindDistanceTol(CPoint* pptCircle,
						int nPtCircle);
	int FindCircleCtr(CPoint* pptCircle,
					int nPtCircle,
					CSize* pcsCenterRange);
	int CircleBestCenter(CImageBuffer* pImgSrc,
						 CRect* prcSrc,
						 CSize* pcsProj,
						 int nBlobType,
						 CPoint2D* ppt2DBestCtr);
	int CircleBestRadius(CPoint* pptEdge, 
					  int nPtEdgeNo,
					  CSize* pcsCenterRange);

};

class CData1DEdgeDetect {
public :
// Public Members
	EDGE_FIND_RESULT m_EdgeFindRslt;					// Edge Find Result
	
// Public Methods
	CData1DEdgeDetect();
	~CData1DEdgeDetect();
	void Clean();

	int GetEdge(int* pnDat,
			 int nDatNo,
			 int nEdgeType,
			 int nContrast,
			 int nGradient = ANY);

	int GetEdge(long* plDat,
			 int nDatNo,
			 int nEdgeType,
			 int nContrast,
			 int nGradient = ANY);

	int GetEdge(double* pdDat,
			 int nDatNo,
			 int nEdgeType,
			 int nContrast,
			 int nGradient = ANY);

private :
// Private Members
	EDGE_GENERAL_PARM EdgeGeneralParm;
	CProjection m_Projection;							// Edge Projection Information
	int m_nEdgeType;
	long m_lMaxGradVal,									// Maximum found Gradient
		 m_lMinGradVal;									// Minimum found Gradient

	int DimensionMeasurement();

	int CreateProjRslt(PROJ_RSLT* pProjRsltSrc,
					  double* pdDat,
					  int nDatNo);

	int	FndNEGEdgeLeft(EDGE_CONTRAST_INFO* pEdgeContrastInfo, // Edge Contrast Info
					   long* plFirstGradVal,			// First Gradient
					   int nStartPos,					// Start Searching Point
					   int nFirstId,					// Start Point of First Gradient
					   long lMinGrad);					// Negative Edge Threshold 

	int	FndNEGEdgeRight(EDGE_CONTRAST_INFO* pEdgeContrastInfo, // Edge Contrast Info
					   long* plFirstGradVal,			// First Gradient
					   int nStartPos,					// Start Searching Point
					   int nLastId,						// Last Point of First Gradient
					   long lMinGrad);					// Negative Edge Threshold 

	int	FndPOSEdgeLeft(EDGE_CONTRAST_INFO* pEdgeContrastInfo, // Edge Contrast Info
					   long* plFirstGradVal,			// First Gradient
					   int nStartPos,					// Start Searching Point
					   int nFirstId,					// Start Point of First Gradient
					   long lMaxGrad);					// Positive Edge Threshold 

	int	FndPOSEdgeRight(EDGE_CONTRAST_INFO* pEdgeContrastInfo, // Edge Contrast Info
					   long* plFirstGradVal,			// First Gradient
					   int nStartPos,					// Start Searching Point
					   int nLastId,						// Last Point of First Gradient
					   long lMaxGrad);					// Positive Edge Threshold 

	int GetGradientEdge(PROJ_RSLT* pProjRslt,			// Edge Projection Result 
						EDGE_CONTRAST_INFO* pEdgeContrastInfo,	// Edge Contrast Info 
						int nFndContrastEdge,			// Number of Edge Contrast Info 
						long* plSecondGradVal,			// the Second Gradient 
						EDGE_GENERAL_PARM *pParm);		// Edge Find parameter

	int	ScanEdgeContrast(EDGE_GENERAL_PARM *pParm,
						 PROJ_RSLT* pProjRslt,			// Edge Projection Result 
						 long* plFirstGradVal,			// First Gradient
						 long* plSecondGradVal,			// Second Gradient
						 long lPOSContrast,				// Positive Contrast
						 long lNEGContrast,				// Negative Contrast
						 EDGE_CONTRAST_INFO* pEdgeContrastInfo,	// Edge Contrast Info
						 int &nFndEdge);				// Number of Edge Found
};

#endif
#pragma once

#include "stdafx.h"
#include "ImageStruct.h"
#include "VisionStruct.h"
#include "ImageBuffer.h"
#include "ProcAlgoStruct.h"
#include "MathDef.h"


void LogImage(int nTab, CImageBuffer& ImgBuf, LPCTSTR pMessageText, ...);
//void LogImage(int nTab, CImageBuffer* pImgBuf, LPCTSTR pMessageText, ...);

	int AllocImageBuffer(CImageBuffer *ImgBuf,			// Pointer to Image to be Allocated
					CSize ImgSize);				// CSize of Image to be Allocated

	int FreeImgBuf(CImageBuffer *ImgBuf);			// Pointer to Image to be Freed

int ValidateImageAndRect(CImageBuffer *pImg,
						 CRect* prc);

class CTeachShiftParm
{
public:	
	CTeachShiftParm();
	~CTeachShiftParm();
	CPoint ptPkgShift;
	CSize szMinDisp;
	CRect rcDefault;
	CSize szFOV;
};
	int Rot90Deg(CImageBuffer *pImgSrc,
			 CRect *prcSrc);

	int GetSum(CImageBuffer *pImg, 
			   CRect *prectROI, 
			   long *plSum);

	long GetHistSum(long* plHist);

	int GetMax(CImageBuffer *pImg, 
			   CRect *prectROI, 
			   BYTE* pbyteMax, 
			   CPoint2D* ppt2DMax);

	int ImgClear(CImageBuffer *pImg, 
				 CRect *prectROI, 
				 BYTE byteVal);

	int GetImgXLine(CImageBuffer *pImg, 
					CPoint* pptStart, 
					int nSize, 
					BYTE *pbyteX);

	int GetImgYLine(CImageBuffer *pImg, 
					CPoint* pptStart, 
					int nSize, 
					BYTE *pbyteY);

	int ClearImageROIEdge(CImageBuffer *pImgSrc,
					   CRect* prcROISrc,
					   BYTE byteLable);


	int GetXGrayAccl(CImageBuffer *pImg,					// Pointer to Image
					 CRect *prectROI,				// ROI Rectangle
					 long *plXAry);					// Pointer to Array for Storing Result Value
	int GetXGrayAcclASM(CImageBuffer *pImg,					// Pointer to Image
					 CRect *prectROI,				// ROI Rectangle
					 long *plXAry);					// Pointer to Array for Storing Result Value

	int GetYGrayAccl(CImageBuffer *pImg,					// Pointer to Image
					 CRect *prectROI,				// ROI Rectangle
					 long *plYAry);					// Pointer to Array for Storing Result Value
	int GetYGrayAcclASM(CImageBuffer *pImg,					// Pointer to Image
					 CRect *prectROI,				// ROI Rectangle
					 long *plYAry);					// Pointer to Array for Storing Result Value

//	int GetXYBinAccl(CImageBuffer *pImg,					// Pointer to Image
//					 CRect *prectROI,				// ROI Rectangle
//					 int *pnXAry,					// Pointer to Array for Storing X Result Value
//					 int *pnYAry);					// Pointer to Array for Storing Y Result Value

	int GetXYBinAccl(IMAGEVIEW_INFO *pView,			// Vision System Info
					 CRect *prectROI,				// ROI Rectangle
					 int *pnXAry,					// Pointer to Array for Storing X Result Value
					 int *pnYAry);					// Pointer to Array for Storing Y Result Value
	int GetXYBinAcclASM(IMAGEVIEW_INFO *pView,			// Vision System Info
					 CRect *prectROI,				// ROI Rectangle
					 int *pnXAry,					// Pointer to Array for Storing X Result Value
					 int *pnYAry);					// Pointer to Array for Storing Y Result Value
	int GetHist(CImageBuffer *pImg,						// Pointer to Image
				CRect *prectROI,					// ROI Rectangle
				long *plAry);						// Pointer to Array for Storing Result Value
	int GetHistASM(CImageBuffer *pImg,						// Pointer to Image
				CRect *prectROI,					// ROI Rectangle
				long *plAry);						// Pointer to Array for Storing Result Value

	int Binarise(CImageBuffer *pImgSrc,
				 CRect *prectSrc,
				 CImageBuffer *pImgDst,
				 CRect *prectDst,
				 BYTE nThresVal);

	int ImgTimes(CImageBuffer *pImgSrc,
				 CRect *prcSrc,
				 CImageBuffer *pImgDst,
				 CRect *prcDst,
				 int nTimes);

	int Binarise(CImgIntBuf *pImgSrc,
				 CRect *prectSrc,
				 CImageBuffer *pImgDst,
				 CRect *prectDst,
				 int nThresVal);


	int BinariseExt1(CImageBuffer *pImgSrc,
				 CRect *prectSrc,
				 CImageBuffer *pImgDst,
				 CRect *prectDst,
				 BYTE nThresVal,
				 int nInvert);

	int BinariseASM(CImageBuffer *pImgSrc,
				 CRect *prectSrc,
				 CImageBuffer *pImgDst,
				 CRect *prectDst,
				 BYTE nThresVal);


	int BinariseExt(CImageBuffer *pImgSrc,
				    CRect *prectSrc,
				    CImageBuffer *pImgDst,
				    CRect *prectDst,
				    int nThresMin,
				    int nThresMax,
					int nInvert);

	int ImgSum(CImageBuffer *pImgBufSrc,
			   CRect* prcSrc,
			   BYTE byteThreshold,
			   long* plIntensitySum,
			   long* plIntensityNo,
			   BOOL bBLACK);

	int ImgMin(CImageBuffer *pImgSrc0,
			CRect *prcSrc0,
			CImageBuffer *pImgSrc1,
			CRect *prcSrc1,
			CImageBuffer *pImgDst,
			CRect *prcDst);

	int ImgMax(CImageBuffer *pImgSrc0,
			CRect *prcSrc0,
			CImageBuffer *pImgSrc1,
			CRect *prcSrc1,
			CImageBuffer *pImgDst,
			CRect *prcDst);


	int ImgADD(CImageBuffer *pImgSrc0,				// Pointer to the first source image
				CRect *prectSrc0,			// ROI of the first source image
				CImageBuffer *pImgSrc1,			// Pointer to the second source image
				CRect *prectSrc1,			// ROI of the second source image
				CImageBuffer *pImgDst,			// Pointer to the destination image
				CRect *prectDst);			// ROI of the destination image

	int ImgADD(CImageBuffer *pImgSrc0,
			CRect *prcSrc0,
			int nVal,
			CImageBuffer *pImgDst,
			CRect *prcDst);

	int ImgAND(CImageBuffer *pImgSrc0,				// Pointer to the first source image
					CRect *prectSrc0,			// ROI of the first source image
					CImageBuffer *pImgSrc1,			// Pointer to the second source image
					CRect *prectSrc1,			// ROI of the second source image
					CImageBuffer *pImgDst,			// Pointer to the destination image
					CRect *prectDst);			// ROI of the destination image

	int ImgOR(CImageBuffer *pImgSrc0,
				CRect *prcSrc0,
				CImageBuffer *pImgSrc1,
				CRect *prcSrc1,
				CImageBuffer *pImgDst,
				CRect *prcDst);
	int ImgXOR(CImageBuffer *pImgSrc0,
				CRect *prcSrc0,
				CImageBuffer *pImgSrc1,
				CRect *prcSrc1,
				CImageBuffer *pImgDst,
				CRect *prcDst);

	int ImgSampling(CImageBuffer *pImgSrc,
				CRect *prcSrc,
				CImageBuffer *pImgDst,
				CPoint* pptTopLftDst,
				CSize *pcsSampling);

	int ImgSampling(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			CImageBuffer *pImgDst,
			CPoint* pptTopLftDst,
			CSize *pcsSampling,
			int nProcMode);//Min:-1; Avg:0 Max:1

	int ImgSampling(CImageBuffer *pImgSrc,			// Pointer to the source image
				CRect *prectSrc,				// ROI of the source image
				CImageBuffer *pImgDst,				// Pointer to the destination image
				CRect *prectDst,				// ROI of the destination image
				CSize *pcsSampling);			// Sampling steps for X and Y direction

	int ImgAvgSampling(CImageBuffer *pImgSrc,		// Pointer to the source image
				CRect *prectSrc,				// ROI of the source image
				CImageBuffer *pImgDst,				// Pointer to the destination image
				CRect *prectDst,				// ROI of the destination image
				CSize *pcsSampling);			// Sampling steps for X and Y direction

	int ZoomImg(CImageBuffer *pImgSrc,
				CRect *prcSrc,
				CImageBuffer *pImgDst,
				CPoint *pptTopLftDst,
				CSize* pcsZoom);

	int ResizeImageWithInterpolation_Fast(CImageBuffer *pImgSrc,
									CRect *prcSrc,
									CImageBuffer *pImgDst,
									CPoint *pptTopLftDst,
									CSize2D* pcsZoom);

	int ResizeImageWithInterpolation(CImageBuffer *pImgSrc,
									CRect *prcSrc,
									CImageBuffer *pImgDst,
									CPoint *pptTopLftDst,
									CSize2D* pcsZoom);
	int ResizeImage(CImageBuffer* pImgSrc, CImageBuffer* pImgDst, BOOL bAccurate = TRUE);


	int ImgInvert(CImageBuffer *pImgSrc,
				CRect *prcSrc,
				CImageBuffer *pImgDst,
				CRect *prcDst);

	int ImgCopy(CImageBuffer *pImgSrc,
			    CRect *prectSrc,
				CImageBuffer *pImgDst,
				CRect *prectDst);

	int ImgCopyASM(CImageBuffer *pImgSrc,
			    CRect *prectSrc,
				CImageBuffer *pImgDst,
				CRect *prectDst);

	int ImgConvertLUT(CImageBuffer *pImgSrc,
			          CRect *prectSrc,
				      CImageBuffer *pImgDst,
				      CRect *prectDst,
					  int *pLUT);

	int GetImgVal0Pos(CImageBuffer *pImg,
				  CRect *prc,
				  CPoint* ppt,
				  BOOL &bFound);

	int GetImgVal255Pos(CImageBuffer *pImg,
				  CRect *prc,
				  CPoint* ppt,
				  BOOL &bFound);

	int ImgRotate(CImageBuffer *pImgSrc,
				 CRect *prcSrc,
				 CImageBuffer *pImgDst,
				 CRect *prcDst,
				 CPoint2D* ppt2DRotCenterSrc,
				 double dAngleSrc,
				 BOOL bAccurate);

	int ImgSubtract(CImageBuffer *pImgSrc0,
					CRect *prectSrc0,
					CImageBuffer *pImgSrc1,
					CRect *prectSrc1,
					CImageBuffer *pImgDst,
					CRect *prectDst);
	int ImgSubtractAbs(CImageBuffer *pImgSrc0,
					CRect *prcSrc0,
					CImageBuffer *pImgSrc1,
					CRect *prcSrc1,
					CImageBuffer *pImgDst,
					CRect *prcDst);

	int ImgSubtractASM(CImageBuffer *pImgSrc0,
					CRect *prectSrc0,
					CImageBuffer *pImgSrc1,
					CRect *prectSrc1,
					CImageBuffer *pImgDst,
					CRect *prectDst);

	BOOL Dilation1D(BYTE *pbyteSrc,						// Pointer to the Source Data
					int nSizeSrc,						// Size of the Source Data
					int nBlobType,
					BYTE *pbyteDst, 					// Pointer to the Destination Data
					int nSizeDst, 						// Size of the Destination Data
					double *pdMask, 					// Pointer to the Mask Data
					int nMask, 							// Size of the Mask Data
					long lProcMode);					// 0 for Binary Dilation, 1 for Gray Scale Dilation

	BOOL Erosion1D(BYTE *pbyteSrc, 						// Pointer to the Source Data
				   int nSizeSrc, 						// Size of the Source Data
				   int nBlobType,
				   BYTE *pbyteDst,		  				// Pointer to the Destination Data
				   int nSizeDst,  						// Size of the Destination Data
				   double *pdMask,  					// Pointer to the Mask Data
				   int nMask, 							// Size of the Mask Data
				   long lProcMode);						// 0 for Binary Dilation, 1 for Gray Scale Dilation


	BOOL ImgDilation(CImageBuffer *pImgSrc,					// Pointer to Source Image
					 CRect *prcSrc,						// ROI of the Source Image
					 int nBlobType,
					 CImageBuffer *pImgDst,					// Pointer to Destination Image
					 CRect *prcDst,						// ROI of the Destination Image
					 double *pdMask,					// Mask for Dilation
					 CSize *pszMask,					// Size of the Mask
					 long lProcMode);					// 0 for 2-D Binary Dilation Using Two 1-D Dilation,
														// 1 Reserved for Development
	int ImageZooming(CImageBuffer *pImgSrc,					// Pointer to Source Image
					 CRect *prcSrc,						// ROI of the Source Image
					 CImageBuffer *pImgDst,					// Pointer to Destination Image
					 CRect *prcDst);						// ROI of the Destination Image

	int ImgBinDilation(CImageBuffer *pImgSrc,
					 CRect *prcSrc,
					 int nBlobType,
					 CImageBuffer *pImgDst,
					 CRect *prcDst,
					 CSize *pszMask);

	int ImgBinDilation(CImageBuffer *pImgSrc,
					 CRect *prcSrc,
					 int nBlobType,
					 CImageBuffer *pImgDst,
					 CRect *prcDst,
					 CRect *prcDilation);

	BOOL ImgErosion(CImageBuffer *pImgSrc,	 				// Pointer to Source Image
					CRect *prcSrc, 						// ROI of the Source Image
					int nBlobType,
					CImageBuffer *pImgDst, 					// Pointer to Destination Image
					CRect *prcDst, 						// ROI of the Destination Image
					double *pdMask, 					// Mask for Dilation
					CSize *pszMask, 					// Size of the Mask
					long lProcMode);					// 0 for 2-D Binary Dilation Using Two 1-D Dilation,
														// 1 Reserved for Development
	int MaskOverlap(CImageBuffer* pImgMask,
					CRect* prcMask,
					CImageBuffer* pImgMask1,
					CRect* prcMask1,
					CPoint* pptOffsetOfMask1);	

	int MaskOverlapOR(CImageBuffer* pImgMask,
					CRect* prcMask,
					CImageBuffer* pImgMask1,
					CRect* prcMask1,
					CPoint* pptOffsetOfMask1);	

	int ImgDebugMasking(CImageBuffer *pImgNOTMasked,
					CRect *prcNOTMasked,
					CImageBuffer *pImgMasked,
					CRect *prcMasked,
					CImageBuffer *pImgMask,
					CRect *prcMask,
					CImageBuffer *pImgMerge,
					CRect *prcMerge);


	CRect GetTeachRect(CRect* prcPreTeach,
				   CTeachShiftParm* pTeachShiftParm);

	CRect GetTeachRect(CRect* prcPreTeach,
					CPoint* ppt2DPkgShift,
					CSize* pszMinDisp,
					CRect* prcDefault,
					CSize* pszFOV);


	int GetLinePtsInROI(CPoint2D* ppt2DOrg,
					   CPoint2D* ppt2D,
					   CPoint2D* ppt2DInROI,
					   CRect* prcROI);

	int ImgDrawLine(CImageBuffer* pImgBufBin,
				 CRect* prcBin,
				 CPoint2D* ppt2DStart,
				 CPoint2D* ppt2DEnd,
				 BYTE byteVale,
				 int nWidth);

	int ImgSlidingSum(CImageBuffer *pImgSrc,
				CRect* prcSrc,
				CSize* pcsProj,
				CImageBuffer* pImgIntProj,
				CRect* prcProj);

	int ImgSlidingSum(CImageBuffer *pImgSrc,
				CRect* prcSrc,
				CSize* pcsProj,
				CImgIntBuf* pImgIntProj,
				CRect* prcProj);

	int OpticsCharacteristics(CImageBuffer* pBuffer, CRect* prectROI, double& dMean, double& dNoise, double& dFocus);

	BOOL ZOOM1D(BYTE *pbyteSrc,						// Pointer to the Source Data
				int nSizeSrc,						// Size of the Source Data
				BYTE *pbyteDst, 					// Pointer to the Destination Data
				int nSizeDst 						// Size of the Destination Data
				);					

	BOOL ZOOM1D(BYTE *pbyteSrc,	
			int nSizeSrc,
			int* pnPosInSrc,
			int* pnPosInterpolTimes10,
			BYTE *pbyteDst,
			int nSizeDst);

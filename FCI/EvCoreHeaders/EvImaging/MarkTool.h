#ifndef _MARKTOOL
#define _MARKTOOL

#include "stdafx.h"
#include "ImageStruct.h"
#include "ImageBuffer.h"
#include "MarkToolStruct.h"


int TmplAdjThres(IMAGEVIEW_INFO *pView,// Vision System Info
				 CImageBuffer *pImg,		// Source Image Buffer
				 long lAreaCnt,		// Teach Template White Pixel Count
				 int nIniThres,		// Teach Template Threshold
				 int nContrastTol,	// Amount of Theshold Compensation Allowed
				 int nMarkColor,	// Color of Marking; WHITE or BLACK
				 CRect rectROI,		// Area of Interest
				 int *pnAdjThres,	// Return Threshold After Adjustment
				 long lDebugFlag);	// Debugging Options


int MinusImg(IMAGEVIEW_INFO *pView,	// Vision System Info
			 CImageBuffer *pInspImg,		// Ptr to Inspected Image
			 CRect *prcInsp,		// ROI of Inspected Image
			 int nThres,			// Adjusted Threshold
			 CImageBuffer *pRefImg,		// Ptr to Reference Image
			 CRect *prcRef,			// ROI of Reference Image
			 CImageBuffer *pDstImg,		// Ptr to Destination Image After Doing the Subtraction
			 long lDebugFlag);		// Debugging Options


/*int FindExcMisAreaBlob(IMAGEVIEW_INFO *pView,		// Vision System Info
					   ImgID *pImg,				// Ptr to Image To Do Miss/Excess Check
					   int nMarkColor,			// Color of Mark
					   double dMissArea,		// Missing Area Size
					   double dExcessArea,		// Excess Area Size
					   double *pdFndMissArea,	// Found Missing Area Size
					   double *pdFndExcessArea,	// Found Excess Area Size
					   CPoint *pptFndMissArea,	// Position of Missing Area Location
					   CPoint *pptFndExcessArea,// Position of Excess Area Location
					   CRect *prectROI,			// ROI
					   long lDebugFlag);		// Debugging Options
*/

int FindExcMisSqBlb(IMAGEVIEW_INFO *pView,			// Vision System Info
					CImageBuffer *pImg,				// Ptr to Image To Do Miss/Excess Check
					int nMarkColor,				// Color of Mark
					double dMissArea,			// Missing Area Size
					double dExcessArea,			// Excess Area Size
					double *pdFndMissArea,		// Found Missing Area Size
					double *pdFndExcessArea,	// Found Excess Area Size
					CPoint *pptFndMissArea,		// Position of Missing Area Location
					CPoint *pptFndExcessArea,	// Position of Excess Area Location
					CRect *prectROI,			// ROI
					long lDebugFlag);			// Debugging Options

int CheckBlob(IMAGEVIEW_INFO *pView,		// Vision System Info
			  CImageBuffer *pImg,
			  CRect *prect,				// Ptr to ROI
			  double dDefectSize,		// Maximum Allowable Defect Size
			  double *pdFndBlobSize,	// Found Defect Size
			  CPoint *pptBlobErr,		// Found Defect Position
			  long lDebugFlag);			// Debugging Options


double CheckArea(IMAGEVIEW_INFO *pView,	// Vision System Info
				 CImageBuffer *pImg,
				 CRect *prect,			// Ptr to Original Rect
				 int nX1,				// Start X Pos of ROI
				 int nY1,				// Start Y Pos of ROI
				 int nXLen,				// X Size of ROI
				 int nYLen,				// Y Size of ROI
				 double dMember,		// Fuzzy Logic member
				 int lDebugFlag);		// Debugging Options

int MaskTmpl(IMAGEVIEW_INFO *pView,		// Vision System Info
			 CImageBuffer *pImg,
			 CRect *prect,				// Ptr to Rect ROI
			 int nThres,				// Threshold Value
			 int nFillVal,				// Fill Color
			 int nMarkColor);			// Type of Marking(WHITE or BLACK)

int	MaskRect(IMAGEVIEW_INFO *pView,		//	Vision System Info
			 CRect *prect,				// Ptr to Rect ROI
			 int nFillVal);				// Fill Color

int ScanForMarkTmpl(IMAGEVIEW_INFO *pView,	// Vision System Info
					CImageBuffer *pImg,		// Source Image Buffer
					CRect *prect,		// Pointer to Rect ROI
					int nMarkColor,		// Type of Marking(WHITE or BLACK)
					int nMinSize,		// Minimum Mark Size(Blob Size)
					int nThres,			// Teach Threshold
					int nWinExt,		// Template Extension Size
					int nMaxRect,		// Maximum Allowable Rects
					int *nFndBlob,		// Pointer to Found Template(Blobs) Count
					CRect *rectFnd);	// Pointer to Found Template(Blobs) Rect Pos	

CRect GetTrimRect(CRect rectInflated,	// Original Rect After Inflation
				  CRect rectOrg,		// Original Rect Before Inflation
				  CRect rectCut,		// Intruded Rect
				  CRect rectIntersect);	// Area of intersection between Original and Intruded Rect

int GetAngleBetween2Rects(IMAGEVIEW_INFO *pView,	// Vision System Info
						  CRect *rcFirst,		// Rect 1
						  CRect *rcLast,		// Rect 2
						  double *pdAngle);		// Pointer to Angle between the 2 Rects

int GetPixCnt(IMAGEVIEW_INFO *pView,		// Vision System Info
			  CImageBuffer *pImg,
			  CRect *rect,				// Rect ROI
			  int nMarkColor,			// Type of Marking(WHITE or BLACK)
			  int nThres,				// Binarizing Threshold for Gettting Pixel Count
			  long *lPixCount);			// Pointer to the Got Pixel Count 

int GenerateRefImg(IMAGEVIEW_INFO *pView,	// Vision System Info
				   CImageBuffer *pSrcImg,		// Original Teach Image(Gray)
				   CImageBuffer *pDstImg,		// Binarize Gray Image
				   CRect *prectROI,		// Rect ROI
				   int nThres);			// Binarizing Threshold

class CGrayScaleCorr {

public :
	// Variables

	// Functions
	CGrayScaleCorr();
	~CGrayScaleCorr();

	int CreateHostTeachMarkTemplate(
		 CImageBuffer* pImgTempl,					// Pointer to Template Image
		 CRect* prcTempl,						// ROI of Template Image
		 CORR_TEMPLATE *pHostMarkTmpl,			// Template Info
		 int nTemplateSamplingAcptScore);		// The Minimum Accept Score for Template Sampling

	int Correlate(
		CImageBuffer* pImgInsp, 						// Pointer to Inspection Image
		CRect *prcInsp, 						// ROI of Inspection Image
		CORR_TEMPLATE *pMarkTemplate,			// Template Info
		int nAcptScore,							// Accept Score
		int nRejectScore,						// Reject Score
		int* pnFoundScore,						// Found Matching Score
		CPoint* pptFoundPos);					// Found Matching Position


	int GrayTemplateMatching(
		CImageBuffer *pImgTempl,						// Pointer to the template image
		CRect *prcImgTempl,						// ROI of the template image
		CImageBuffer *pImgInsp,						// Pointer to the inspection image
		CRect *prcImgInsp,						// ROI of the inspection image
		int &nMaxCorr);							// Correlation Score




private :

	int nMinCorrSamplingScore;					// Minimum Accept Score for Template Sampling
	int nSkipScore;								// Score for Skipping Further Process
	int *pnScoreX;								// Correlation Scores for X Projection
	int *pnScoreY;								// Correlation Scores for Y Projection
	int *pnScoreXY;								// Sum of the X Projection Correlation Scores and Y Projection Correlation Score
	long *plXAry2;								// X Projection of Inspection Images Whose Projection Length is equal to the Height of Template
	long *plYAry2;								// Y Projection of Inspection Images Whose Projection Length is equal to the Width of Template
	int nMinCorrTol;							// Parameter for Determining the Possible Check Position

	CPoint ptCorrPos[100];						// Correlation Position
	int nCorNo;									// Number of Correlation Position
	CPoint ptSamplingCorrPos[100];				// Correlation Position for Sampling Image
	int nCorNoSampling;							// Number of Correlation Position for Sampling Image
	CORR_TEMPLATE MarkInsp;						// Inspection Mark Info
	CSize csSampling;							// Size of Sampling Steps for X and Y Direction
	int ImgOffsetSampling(
		CImageBuffer *pImgSrc,						// Pointer to the source image
		CRect *prcSrc,							// ROI of the source image
		CImageBuffer *pImgDst,						// Pointer to the destination image
		CRect *prcDst,							// ROI of the destination image
		CSize *pcsSampling,						// Sampling steps for X and Y direction
		CSize *pcsOffset);						// Sampling starting position			

	void GetImgProj(
		CORR_PROJ_PATTERN *pMarkTemplateProj,	// Template Projection Info
		CORR_PROJ_PATTERN *pMarkInspProj);		// Inspection-Mark Projection Info

	int ProjXFirstCorr2D(
		CORR_PROJ_PATTERN *pMarkTemplateProj,	// Template Projection Info
		CORR_PROJ_PATTERN *pMarkInspProj,		// Inspection-Mark Projection Info
		CPoint *pptCorrPos,						// Correlation Positions
		int &nCorNo);							// Number of Correlation Positions

	int ProjYFirstCorr2D(
		CORR_PROJ_PATTERN *pMarkTemplateProj,	// Template Projection Info
		CORR_PROJ_PATTERN *pMarkInspProj,		// Inspection-Mark Projection Info
		CPoint *pptCorrPos,						// Correlation Positions
		int &nCorNo);							// Number of Correlation Positions

	int ProjXCorrPt(
		CORR_PROJ_PATTERN *pMarkTemplateProj,	// Template Projection Info
		CORR_PROJ_PATTERN *pMarkInspProj,		// Inspection-Mark Projection Info
		CPoint ptCorr);							// Correlation Position

	int ProjYCorrPt(
		CORR_PROJ_PATTERN *pMarkTemplateProj,	// Template Projection Info
		CORR_PROJ_PATTERN *pMarkInspProj,		// Inspection-Mark Projection Info
		CPoint ptCorr);							// Correlation Position

	int MatchingPosDetermination(
		CORR_TEMPLATE *pMarkTemplate,			// Template Info
		CORR_TEMPLATE *pMarkInsp,				// Inspection-Mark Info
		CPoint *pptCorrPos,						// Correlation Positions
		int &nCorNo,							// Number of Correlation Positions
		int nMinCorrTol);						// Correlation Tolerance for Choosing Correlation Positions

	int SortingMaxProjCorrPos(
		int *pnScoreX,							// Correlation Scores for X Projection
		int *pnScoreY,							// Correlation Scores for Y Projection
		int *pnScoreXY,							// Sum of the X Projection Correlation Scores and Y Projection Correlation Score
		CSize csCorr,							// Size of Template Shift
		CPoint *pptCorrPos,						// Positions for Mark Correlation 	
		int &nCorNo);							// Number of Positions for Mark Correlation

	void GetSamplingStep(
		CORR_TEMPLATE *pMarkTemplate);			// Template Info 

	int GetSamplingMinCorrSocre(
		CORR_TEMPLATE *pMarkTemplate,			// Template Info
		int nSamplingStepX,						// Sampling Step Size for X direction
		int nSamplingStepY,						// Sampling Step Size for Y direction
		int nMinCorrSamplingScore,				// Minimum Accept Score for Template Sampling
		int &nMinCorr);							// Found Minimum Score for Template Sampling

	void GetProjectionData(
		 CORR_TEMPLATE *pMarkTemplate);			// Template Info 

	void ProjCorData(
		 CORR_PROJ_DATA *pProjData);			// Projection Info 

	void LocateMarkMem(
		 CORR_TEMPLATE *pMarkTemplate);			// Template Info 

	void GetCor2DData(
		 CORR_TEMPLATE *pMarkTemplate);			// Template Info 

	void GetProjDirection(
		 CORR_TEMPLATE *pMarkTemplate);			// Template Info 

};
#endif
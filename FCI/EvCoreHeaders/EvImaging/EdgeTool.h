#ifndef _EDGETOOL
#define _EDGETOOL

#include "stdafx.h"
#include "ImageStruct.h"
#include "MathDef.h"
#include "EdgeToolStruct.h"
#include "ProcAlgo.h"

class CEdgeDetect {
public :
// Public Members
	EDGE_FIND_RESULT m_EdgeFindRslt;					// Edge Find Result
	EDGE_SLANT_SCAN_RESULT m_EdgeSlantScanRslt;			// Edge Slant Scan Result
	
// Public Methods
	CEdgeDetect();
	~CEdgeDetect();
	int GetEdge(IMAGEVIEW_INFO *pView,						// Vision System Info
				CImageBuffer *pImg,							// Pointer to Image
				EDGE_FIND_PARM *pParm,					// Edge Find Parameter
				EDGE_FILTER_PARM *pFilterParm,			// Edge Filter Parameter
				long lProcMode,							// Edge Find Mode
				long lEdgeCheckMode,					// Edge Check Mode
				long lDebugFlag);						// Debugging Options

	int GetSlantScanEdge(IMAGEVIEW_INFO *pView,					// Vision System Info
						 CImageBuffer *pImg,							// Pointer to Image
						 EDGE_SLANT_SCAN_PARM *pParm,			// Edge Slant Scan Parameter
						 EDGE_FILTER_PARM *pFilterParm,			// Edge Filter Parameter
						 long lProcMode,						// Edge Find Mode
						 long lEdgeCheckMode,					// Edge Check Mode
						 long lDebugFlag,
						 CString* pstrDebug=NULL);						// Debugging Options

	int GetSlantScanEdge(IMAGEVIEW_INFO *pView,
						 CImageBuffer *pImg,
						 EDGE_SLANT_SCAN_PARM *pParm,
						 EDGE_FILTER_PARM *pFilterParm,
						 CProjection* pProjection,
						 long lProcMode,
						 long lEdgeCheckMode,
						 long lDebugFlag);

private :
// Private Members
	EDGE_GENERAL_PARM EdgeGeneralParm;
	CProjection m_Projection;							// Edge Projection Information

	CArray <double,double> dAryFndPOSEdge;
	CArray <double,double> dAryFndNEGEdge;

	long m_lMaxGradVal,									// Maximum found Gradient
		 m_lMinGradVal;									// Minimum found Gradient

// Private Methods
	int ApplyFiltering(PROJ_RSLT* pProjRslt,			// Edge Projection Result 
					   EDGE_FILTER_PARM *pFilterParm);	// Edge Filter Parameter

	int ApplyUserDefFiltering(PROJ_RSLT* pProjRslt, 	// Edge Projection Result 
							  FILTER_1D* UserFilter);	// User Defined Edge Filter

	int ApplyMinFiltering(PROJ_RSLT* pProjRslt, 		// Edge Projection Result 
						  FILTER_1D* MinFilter);		// Min Filter

	int MinInSlideWindow(PROJ_RSLT* pProjRslt, 			// Edge Projection Result 
						 int nCenterID,					// Center Position of a Filtering Region
						 int nFilterRadius,				// Radius of the Filtering Region
						 long &lMin);					// Min in the Filtering Region

	int ApplyMaxFiltering(PROJ_RSLT* pProjRslt, 		// Edge Projection Result 
						  FILTER_1D* MaxFilter);		// Min Filter

	int MaxInSlideWindow(PROJ_RSLT* pProjRslt, 			// Edge Projection Result 
						 int nCenterID,					// Center Position of a Filtering Region
						 int nFilterRadius,				// Radius of the Filtering Region
						 long &lMax);					// Max in the Filtering Region

	int GetGradientEdge(PROJ_RSLT* pProjRslt,			// Edge Projection Result 
						EDGE_CONTRAST_INFO* pEdgeContrastInfo,	// Edge Contrast Info 
						int nFndContrastEdge,			// Number of Edge Contrast Info 
						long* plSecondGradVal,			// the Second Gradient 
						EDGE_GENERAL_PARM *pParm);		// Edge Find parameter

	int GetGravityEdge(PROJ_RSLT* pProjRslt,			// Edge Projection Result 
					   EDGE_CONTRAST_INFO* pEdgeContrastInfo,	// Edge Contrast Info 
					   int nFndContrastEdge,			// Number of Edge Contrast Info
					   long* plFirstGradVal,			// the First Gradient 
					   EDGE_GENERAL_PARM *pParm);			// Edge Find parameter

	int GetOneGravityEdge(PROJ_RSLT* pProjRslt,			// Edge Projection Result 
						  EDGE_CONTRAST_INFO* pEdgeContrastInfo,	// Edge Contrast Info 
						  long* plFirstGradVal,			// the First Gradient 
						  EDGE_GENERAL_PARM *pParm,		// Edge Find parameter
						  double& dExactPos);			// Edge Position Found

	int GetScanningGradientEdge(PROJ_RSLT* pProjRslt, 	// Edge Projection Result 
								EDGE_FIND_PARM *pParm);	// Edge Find parameter

	int GetScanningGravityEdge(PROJ_RSLT* pProjRslt, 	// Edge Projection Result 
							   EDGE_FIND_PARM *pParm);	// Edge Find parameter

	int GetLevelEdge(PROJ_RSLT* pProjRslt,				// Edge Projection Result 
					 EDGE_GENERAL_PARM *pParm);			// Edge Find parameter

	int	GetLevelEdgeMinAndMax(PROJ_RSLT* pProjRslt,		// Edge Projection Result 
							  int nContrast,			// Edge Contrast
							  int nWhiteCount,			// White Pixel Count
							  int nBlackCount,			// Black Pixel Count
							  long& lMin,				// Min Value
							  long& lMax);				// Max Value

	int EdgeCheck(PROJ_RSLT* pProjRslt,					// Edge Projection Result 
				  double dEdge,							// Edge Position
				  BOOL bPositive,						// Positive or Negative Edge
				  EDGE_GENERAL_PARM *pParm,				// Edge Find Parameter
				  long lEdgeCheckMode);					// Edge Check Mode

	int CheckContrastAndOtherEdges(PROJ_RSLT* pProjRslt,// Edge Projection Result 
								int nContrastTol,		// Edge Contrast
								int nEdgePos,			// Positive or Negative Edge
								int nBlackEdgeStart,	// Black Edge Starting Point
								int nBlackEdgeEnd,		// Black Edge Ending Point
								int nWhiteEdgeStart,	// White Edge Starting Point
								int nWhiteEdgeEnd);		// White Edge Ending Point

	int	ScanEdgeContrast(EDGE_GENERAL_PARM *pParm,
						 PROJ_RSLT* pProjRslt,			// Edge Projection Result 
						 long* plFirstGradVal,			// First Gradient
						 long* plSecondGradVal,			// Second Gradient
						 long lPOSContrast,				// Positive Contrast
						 long lNEGContrast,				// Negative Contrast
						 EDGE_CONTRAST_INFO* pEdgeContrastInfo,	// Edge Contrast Info
						 int &nFndEdge);				// Number of Edge Found

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

	int NormalizeData(PROJ_RSLT* pProjRslt,				// Edge Projection Result 
					  int nNormalize);					// Data for Normalization

};

class CGridEdgeDetect {
public :
// Public Members
	MULTI_EDGE_SCAN_RSLT m_MultiEdgeScanRslt;			// Edge Find Result
	
// Public Methods
	CGridEdgeDetect();
	~CGridEdgeDetect();

	int GetEdge(IMAGEVIEW_INFO *pView,						// Vision System Info
				CImageBuffer *pImg,							// Pointer to Image
				MULTI_EDGE_SCAN_PARM *pParm,			// Multi-Edge Scanning Parameter
				EDGE_FILTER_PARM *pFilterParm,			// Edge Filter Parameter
				long lProcMode,							// Edge Find Mode
				long lEdgeCheckMode,					// Edge Check Mode
				long lDebugFlag);						// Debugging Options

	int GetSlantScanEdge(IMAGEVIEW_INFO *pView,					// Vision System Info
						 CImageBuffer *pImg,							// Pointer to Image
						 MULTI_EDGE_SLANT_SCAN_PARM *pParm,		// Multi-Edge Slant Scanning Parameter
						 EDGE_FILTER_PARM *pFilterParm,			// Edge Filter Parameter
						 long lProcMode,						// Edge Find Mode
						 long lEdgeCheckMode,					// Edge Check Mode
						 long lDebugFlag,
						 CString* pstrDebug=NULL);						// Debugging Options
};

#endif
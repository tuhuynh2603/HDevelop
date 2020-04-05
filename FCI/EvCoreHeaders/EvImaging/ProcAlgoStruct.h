#pragma once

#include "ImageDef.h"

#include "MathDef.h"

///Udupa; June'2014
template<typename TYPE> void createArray(TYPE*& pArray, int nSize, bool bOptimize=true, bool bInitialize=false);
template<typename TYPE> void deleteArray(TYPE*& pArray, bool bOptimize=true);
template<typename TYPE> void copyArray(TYPE* pDst, TYPE* pSrc, int nSize);

class CCudaInterface;
CCudaInterface*& CudaIf();
BOOL IsOptimized();
BOOL OptimizeCpu();
BOOL OptimizeGpu();



class PROJ_PARM
{
public:
	PROJ_PARM();
	~PROJ_PARM();

	CRect rectROI;						// ROI Rectangle
	int nOrientation;					// Row or Column Projection
}; 

class PROJ_RSLT
{
public:
	PROJ_RSLT();
	~PROJ_RSLT();
	void Clean();
	int lMaxValue;						// Max Projected Value
	int lMinValue;						// Min Projected Value
	int nFirstId;						// The first valid position
	int nLastId;						// The last valid position
	double dPOSEdgeShift;				// Shift of Positive Edge
	double dNEGEdgeShift;				// Shift of Negative Edge
	long* plProjVal;					// Array of Projected Values
	int nProjSize;						// Projection Size
	long lMagnify;						// Magnification of Projection
};


class PROJ_SLANT_PARM
{
public:
	PROJ_SLANT_PARM();
	~PROJ_SLANT_PARM();

	CLine2D ln2DScan;					// Scanning Base Line 
	CPoint2D pt2DScanStart;				// Scanning Point
	CPoint2D pt2DScanEnd;				// Scanning End Point
	CPoint2D pt2DProj;					// Projection Point
	BOOL bAccurate;						// Accuracy: 0: low accuracy but fast; 1: high accuracy but slow
}; 


class PROJ_SLANT_RSLT
{
public:
	PROJ_SLANT_RSLT();
	~PROJ_SLANT_RSLT();
	PROJ_RSLT ProjRslt;					// Projection result
	int nAbsScanWidth;
	int nAbsScanHeight;
	double dScanWidthStepX;
	double dScanWidthStepY;
	double dScanHeightStepX;
	double dScanHeightStepY;
};


typedef struct
{
	CRect rectROI;						// ROI Rectangle
	int nOrientation;					// Row or Column Edge Finding
	int nGradient;						// POS or NEG Edge Gradient wrt to FORWARD Direction Only
	int nMethod;						// THRESHOLD or LEVEL Option
	int nThreshold;						// Gradient Threshold
	int nSkipFactor;					// Skip Size between each edge finder(pix)
	int nGroupFactor;					// Group Size for each edge finder (pix)
	int nDirection;						// FORWARD(Use 1st Edge) or BACKWARD(Use Last Edge) Direction
} GRD_EDGE_FIND_PARM;

typedef struct
{
	double dFndEdgePos[MAX_LINE_COUNT][MAX_GRD_EDGE];		// Found Edge Positions
	int nFndEdgeGrad[MAX_LINE_COUNT][MAX_GRD_EDGE];		// Found Edges Gradient Type
	int nLineCount;									// No of Scan Lines
	BOOL bRsltValid;								// Is Result Valid
} GRD_EDGE_FIND_RSLT;



class ADV_GRD_EDGE_FIND_PARM
{
public:
	ADV_GRD_EDGE_FIND_PARM();
	~ADV_GRD_EDGE_FIND_PARM();

	CRect rectROI;						// ROI Rectangle
	int nOrientation;					// Row or Column Edge Finding
	int nGradient;						// POS or NEG Edge Gradient
	int nThreshold;						// Gradient Threshold
	int nGridSpacing;					// Spacing between each line scanner edge finder(pix)
	int nDirection;						// FORWARD or BACKAWARD Direction of Search
	int nEdgeFilterSize;				// Edge Filter Size
	int nEdgeWidth;						// Edge Width
	int nMaxStepSize;					// Maximum gap between edge samples taken for Hough transform  
	int nSmoothingMethod;				// Smoothing before edge detection  
}; 




typedef struct {
	double dFndEdgePos[MAX_LINE_COUNT][MAX_GRD_EDGE];		// Found Edge Positions
	int nFndEdgeGrad[MAX_LINE_COUNT][MAX_GRD_EDGE];		// Found Edges Gradient Type
	int nLineCount;									// No of Scan Lines
	BOOL bRsltValid;								// Is Result Valid
	double dR;										// Pendicular Dist of Line to Origin(0,0)
	double dTheta;									// Angle of Line wrt X-Axis
	double dRange;									// Max-Min of Found Edge wrt to EdgeFinding Axis
} ADV_GRD_EDGE_FIND_RSLT;

typedef struct {
	double	dAngleStep;
	double	dAngleDeviation;
	double	dNominalAngle;
	double	dRhoBinSize;
	int		nMinimumVotes;
} LINE_FIND_PARM;


#define FIND_BLOB_RECT_AND_AREA		0			// Option of Find Blob Rect and Area
#define FIND_BLOB_RECT				1			// Option of Find Blob Rect
#define FIND_BLOB_AREA				2			// Option of Find Blob Area
#define FIND_BLOB_CENTER			4			// Option of Find Blob Center	
#define FIND_BLOB_LENGTH			8			// Option of Find Blob Length
#define FIND_BLOB_ELONGATION		16			// Option of Find Blob Elongation
#define FIND_BLOB_CONTOUR			32			// Option of Find Blob Contour
#define FIND_BLOB_HOLE				64			// Option of Find Blob Hole
#define FIND_BLOB_MAX_PROJ_LENGTH	128


#define SORTING_BLOB_BY_AREA		0		
#define SORTING_BLOB_BY_WIDTH		1		
#define SORTING_BLOB_BY_HEIGHT		2		
#define SORTING_BLOB_BY_LENGTH		3		
#define SORTING_BLOB_BY_ELONGATION	4		



typedef struct {
	CRect rectROI;						// ROI Rectangle
	int nOrientation;					// Row or Column Blob Finding
	int nBlobType;						// BLACK or WHITE Blob
	int nMinBlobSize;					// Min Blob Size to be considered a blob
	int nEdgeLevel;						// 0%-100% Search Level
	int nContrast;						// MaxProjVal/PixelCount - MinProjVal/PixelCount must be greater than
										// nContrast to assume that there is some whiteness and blackness present
										// If nContrast==0 then don't check contrast
} BLOB_FIND_PARM;

typedef struct {
	CRect rcFndBlobPos[MAX_BLOB];		// Found Blob Rect
	int nFndBlobs;						// No of Blobs Found
} BLOB_FIND_RSLT;

typedef struct {
	CRect rcROISrc;						// ROI Source Rectangle
	CRect rcROIDst;						// ROI Destination Rectangle
	int nBlobType;						// BLACK or WHITE Blob
	int nMinXBlobSize;					// Min X Blob Size to be considered a blob
	int nMaxXBlobSize;					// Max X Blob Size to be considered a blob
	int nMinYBlobSize;					// Min Y Blob Size to be considered a blob
	int nMaxYBlobSize;					// Max Y Blob Size to be considered a blob
	int nMinBlobArea;					// Min Blob Area
	int nMaxBlobArea;					// Max Blob Area
	int nThreshold;						// 0-255 Threshold
	double dMinBlobLength;					// Min Blob Length
	double dMaxBlobLength;					// Max Blob Length
	double dMinBlobElongation;				// Min Blob Elongation
	double dMaxBlobElongation;				// Max Blob Elongation
} ADV_BLOB_FIND_PARM;


class ADV_BLOB_INFO
{
public:
	ADV_BLOB_INFO();
	~ADV_BLOB_INFO();
	void Clean();
	ADV_BLOB_INFO & operator =(ADV_BLOB_INFO* pAdvBlobInfo);
	CPoint2D pt2DFndBlobCenter;				// Found Blob Center
	CRect rcFndBlobPos;						// Found Blob Rect
	int nFndBlobArea;						// Individual Area of Each Found Blob
	CPoint* pptBlobContour;					// Blob Contour Points
	int nBlobContourSize;					// Number of Blob Contour Points
	CRect* prcHole;
	CPoint** ppptBlobHoleContour;
	int* pnptBlobHoleContourSize;
	int nBlobHole;
	double dFndBlobLength;					// Found Blob Length
	double dFndBlobElongation;				// Found Blob Elongation
	int nBlobAvgIntensity;
};

class ADV_BLOB_FIND_RSLT
{
public:
	ADV_BLOB_FIND_RSLT();
	~ADV_BLOB_FIND_RSLT();
	void Clean();
	ADV_BLOB_INFO* pAdvBlobInfo;
	ADV_BLOB_FIND_RSLT & operator =(ADV_BLOB_FIND_RSLT* pAdvBlobFindRslt);
	int nFndBlobs;						// No of Blobs Found
};


typedef struct
{
	CRect rectROI;						// ROI Rectangle
} HIST_FIND_PARM;

typedef struct {
	long lHistVal[256];						// Histogram Value
	long lAcclHistVal[256];					// Accumulative Histogram Value
	int nMostCommonGray;					// Most Common Gray Value
	int nLowGray;							// Highest Gray Value
	int nHighGray;							// Lowest Gray Value
	int nMedian;							// Median Gray Value
} HIST_FIND_RSLT;

class CPerformanceTimer;
void PrintTime(ADV_BLOB_FIND_PARM *pParm, CString str, CPerformanceTimer* pTimer);

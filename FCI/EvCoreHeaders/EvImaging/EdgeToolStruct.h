#ifndef _EDGE_TOOL_STRUCT
#define _EDGE_TOOL_STRUCT

#include "ImageStruct.h"


#define FILTER_0						0			// Filter Index 0
#define FILTER_1						1			// Filter Index 1
#define FILTER_2						2			// Filter Index 2
#define FILTER_3						3			// Filter Index 3
#define FILTER_4						4			// Filter Index 4
#define FILTER_5						5			// Filter Index 5

#define BINOMIAL_FILTER					0			// Binomial Filter 
#define GAUSSIAN_FILTER					1			// Gaussian Filter
#define MAX_FILTER						2			// Max. Filter (Gray Scale Dilation)
#define MIN_FILTER						3			// Min. Filter (Gray Erosion Dilation)
#define MEAN_FILTER						4			// Mean Filter
#define MEDIAN_FILTER					5			// Median Filter

#define EDGE_NO_CHECK					0			// No Edge Contrast Check
#define EDGE_MEAN_CHECK					1			// Edge Contrast Check Using the Difference between Mean of White Pixel Count and Mean of Black Pixel Count
#define EDGE_NEIGHBOR_CHECK				2			// Edge Contrast Check and One Edge Check in the Region of White and Black Pixel Count

#define GRAVITY_EDGE					0			// Gravity Edge
#define GRADIENT_EDGE					1			// Gradient Edge
#define LEVEL_EDGE						2			// Level Edge

#define LINE_EDGE_TYPE					0			// Line Edge TYPE
#define LEVEL_EDGE_TYPE					1			// Level Edge TYPE

#define Filter_Sum_Max					1000

class FILTER_1D
{
public:
	// Functions
	FILTER_1D();
	~FILTER_1D();
	void Clean();
	FILTER_1D & operator = (const FILTER_1D &Filter1D); 

	int CreateBinomialFilter(int nFilterRadius);
	int CreateGaussianFilter(int nFilterRadius, double dSigma);
	int CreateMaxFilter(int nFilterRadius);
	int CreateMinFilter(int nFilterRadius);
	int CreateMeanFilter(int nFilterRadius);
	int CreateMedianFilter(int nFilterRadius);

	// Variables
	int *m_pnFilter;					// Pointer to Filter Data
	int m_nFilterSize;					// Size of Filter Data
	int m_nFilterRadius;				// Radius of Filter
	int m_nFilterSum;					// Sum of Filter
	double m_dPOSEdgeShift;				// Positive Edge Shift after Applying Filter
	double m_dNEGEdgeShift;				// Negative Edge Shift after Applying Filter
	int m_nFilterMode;					// Filter Mode
private:
	void FindFilterSum();
};

class EDGE_FILTER_PARM
{
public:
	// Functions
	EDGE_FILTER_PARM();
	~EDGE_FILTER_PARM();
	void InitUserDefFilterPam();

	// Variables
	FILTER_1D Filter_0;							// User Defined Filter 0
	FILTER_1D Filter_1;							// User Defined Filter 1
	FILTER_1D Filter_2;							// User Defined Filter 2
	FILTER_1D Filter_3;							// User Defined Filter 3
	FILTER_1D Filter_4;							// User Defined Filter 4
	FILTER_1D Filter_5;							// User Defined Filter 5

	long m_lApplyFilterSequence[10];			// Sequence of Applying Filters		
	int m_nTotalApplyFilter;					// Total of Applying Filters

	FILTER_1D Filter[6];						// Save User Defined Filters in an Array
};


class EDGE_FIND_PARM
{
public:
	EDGE_FIND_PARM();
	~EDGE_FIND_PARM();
	// Variables
	CRect rectROI;						// ROI Rectangle
	int nOrientation;					// Scanning Direction
	int nGradient;						// POS or NEG or ANY Edge Gradient
	int nDirection;						// FORWARD or BACKWARD Search
	int nWhiteCount;					// Min White Pixel Count on Edge Cross Over
	int nBlackCount;					// Min Black Pixel Count on Edge Cross Over
	int nEdgeLevel;						// 33%-80% Search Level
	int nContrast;						// Edge Contrast
	int nTerminationContrast;			// Not in use: Reserved for Development
}; 


class EDGE_SLANT_SCAN_PARM
{
public:
	EDGE_SLANT_SCAN_PARM();
	~EDGE_SLANT_SCAN_PARM();
	// Variables
	CLine2D ln2DScan;					// Scanning Base Line
	CPoint2D pt2DScanStart;				// Scanning Point
	CPoint2D pt2DScanEnd;				// Scanning End Point
	CPoint2D pt2DProj;					// Projection Point
	BOOL bAccurate;						// Accuracy: 0: Low Accuracy but Fast ; 1: Hight Accuracy but Slow
	int nGradient;						// POS or NEG or ANY Edge Gradient
	int nDirection;						// FORWARD or BACKWARD Search
	int nWhiteCount;					// Min White Pixel Count on Edge Cross Over
	int nBlackCount;					// Min Black Pixel Count on Edge Cross Over
	int nEdgeLevel;						// 33%-80% Search Level
	int nContrast;						// Edge Contrast
	int nTerminationContrast;			// Not in use: Reserved for Development
}; 

class EDGE_GENERAL_PARM
{
public:
	int nStartPos;						// Start Position
	int nGradient;						// POS or NEG or ANY Edge Gradient
	int nDirection;						// FORWARD or BACKWARD Search
	int nWhiteCount;					// Min White Pixel Count on Edge Cross Over
	int nBlackCount;					// Min Black Pixel Count on Edge Cross Over
	int nEdgeLevel;						// 33%-80% Search Level
	int nContrast;						// Edge Contrast
	int nTerminationContrast;			// Not in use: Reserved for Development
}; 


class EDGE_FIND_RSLT
{
public:
	EDGE_FIND_RSLT();
	~EDGE_FIND_RSLT();
	double dFndEdgePos[MAX_EDGE];		// Found Edge Positions
	double dFndGradVal[MAX_EDGE];		// Gradient Value at Found Edge Position
	int nFndEdgeGrad[MAX_EDGE];			// Gradient Type at Found Edge Position 
	int nFndEdges;						// Number of Edges Found
};

class EDGE_FIND_RESULT
{
public:
	EDGE_FIND_RESULT();
	~EDGE_FIND_RESULT();
	double* pdFndPOSEdge;			// Positive Edge Position Found
	int nFndPOSEdge;				// Number of Positive Edge Found
	double* pdFndNEGEdge;			// Negative Edge Position Found
	int nFndNEGEdge;				// Number of Negative Edge Found
};


class EDGE_SLANT_SCAN_RESULT
{
public:
	EDGE_SLANT_SCAN_RESULT();
	~EDGE_SLANT_SCAN_RESULT();
	CPoint2D* ppt2DFndPOSEdge;			// Positive Edge Position Found
	int nFndPOSEdge;				// Number of Positive Edge Found
	CPoint2D* ppt2DFndNEGEdge;			// Negative Edge Position Found
	int nFndNEGEdge;				// Number of Negative Edge Found
};

class MULTI_EDGE_SCAN_PARM
{
public:
	MULTI_EDGE_SCAN_PARM();
	~MULTI_EDGE_SCAN_PARM();
	// Variables
	CRect rcScanROI;					// ROI Rectangle
	int nSkipFactor;					// Skip Size between each edge finder(pix)
	int nGroupFactor;					// Group Size for each edge finder (pix)
	EDGE_FIND_PARM EdgeFindParm;		// Edge find parameter for each scanning line
}; 

class MULTI_EDGE_SLANT_SCAN_PARM
{
public:
	MULTI_EDGE_SLANT_SCAN_PARM();
	~MULTI_EDGE_SLANT_SCAN_PARM();
	// Variables
	CPoint2D pt2DLastScanLineStart;				// Starting Point of Last Slant Scanning 
	int nSkipFactor;							// Skip Size between each edge finder(pix)
	EDGE_SLANT_SCAN_PARM EdgeSlantScanParm;		// Edge find parameter for each scanning line
}; 

class MULTI_EDGE_SCAN_RSLT
{
public:
	MULTI_EDGE_SCAN_RSLT();
	~MULTI_EDGE_SCAN_RSLT();
	CPoint2D* pt2dFndPOSEdge;			// Positive Edge Position Found
	int nFndPOSEdge;					// Number of Positive Edge Found
	CPoint2D* pt2dFndNEGEdge;			// Negative Edge Position Found
	int nFndNEGEdge;					// Number of Negative Edge Found
};


class EDGE_SHAPE_INFO
{
public:
	// Functions
	EDGE_SHAPE_INFO();
	~EDGE_SHAPE_INFO();
	bool bFndLftPos;					// Left Edge Found or Not
	bool bFndRhtPos;					// Right Edge Found or Not
	int nFndLftPos;						// Left Edge Position
	int nFndRhtPos;						// Right Edge Position
	long lGrad;							// Threshold of First Gradient
};

class EDGE_CONTRAST_INFO
{
public:
	// Functions
	EDGE_CONTRAST_INFO();
	~EDGE_CONTRAST_INFO();
	EDGE_CONTRAST_INFO & operator = (const EDGE_CONTRAST_INFO &EdgeContrastInfo); 
	EDGE_SHAPE_INFO HalfGradEdgeShapeInfo;		// Edge Shape Information by Setting Threshold at Half of Edge Gradient 
	bool bHalfGradEdgeShape;					// Half-Edge-Gradient Edge Shape Information Found or Not
	EDGE_SHAPE_INFO BaseGradEdgeShapeInfo;		// Edge Shape Information by Setting Threshold at 1/3 of Edge Gradient 
	bool bBaseGradEdgeShape;					// 1/3-Edge-Gradient Edge Shape Information Found or Not
	long lPOSContrast;							// Positive Edge Contrast
	long lNEGContrast;							// Negative Edge Contrast
	long lMaxContrast;							// Positive Edge Contrast Found
	long lMinContrast;							// Negative Edge Contrast Found
	bool bPOSEdge;								// Positive or Negative Edge Found 
	int nZeroCrossingLftPos;					// Left Point of zero-crossing 
	int nZeroCrossingRhtPos;					// Right Point of zero-crossing
};

#endif
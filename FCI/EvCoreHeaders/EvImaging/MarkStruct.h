#ifndef _MARKSTRUCT
#define _MARKSTRUCT

#include "Imaging.h"


#define TMPL_PASS				1
#define TMPL_FAIL_CORR			2
#define TMPL_FAIL_EXCESS_MARK	3
#define TMPL_FAIL_MISSING_MARK	4
#define TMPL_FAIL_NOTFOUND		5

#define BLOB_AREA				1
#define SQUARE_AREA				2

#define MAX_TCH_WIN				4
#define MAX_SYMBOLSET			5

#define MAX_MRK_TEMPLATE		300
#define MAX_MARKSET				3
#define MAX_TMPL_PER_MRKSET		50

//////////////////////////////////////////////////////////////////////////////
// Mark Parameters
//////////////////////////////////////////////////////////////////////////////

// Template Inspection Parameters
typedef struct {
	BOOL bGrossCheck;
	int nAcceptScore;
	int nRejectScore;
	double dMinExcessPixel;			// Min Excess Pixel Size	
	double dMinMissingPixel;			// Min Missing Pixel Size
	int nMethod;					// 1:Blob Area, 2:Square Area
}TMPL_INSP_PARM;

// Single Mark Set Inspection Parameters
typedef struct {
	TMPL_INSP_PARM TmplPar;			// Parameters for Other Templates
	TMPL_INSP_PARM LogoPar;			// Parameters for 1st Template
	BOOL bLogoSeparateParm;			// Seperate Parameters for First Template
	BOOL bTchWinUserDefine;			// Teach Window User Define
	int nMarkRotation;				// Max allowable rotation of mark
	int nTchRectCnt;				// Total teach rectangles
	int nMarkColor;					// White or black marking
	int nMinCharSize;				// Min Character Size used for blob segmentation
	int nTmplContrastTol;			// Template Contrast Tol
	int nXFirstTmplShiftTol;		// X Shift Tol for First Template
	int nYFirstTmplShiftTol;		// Y Shift Tol for First Template
	int nXOtherTmplShiftTol;		// X Shift Tol for Other Templates
	int nYOtherTmplShiftTol;		// Y Shift Tol for Other Templates
	BOOL bMaskTemplate;				// Mask Template After Inspection
	int nTmplXSkpFact;				// Template X Skip Factor
	int nTmplYSkpFact;				// Template Y Skip Factor
	int nFastTmplXSkpFact;			// Fast Template X Skip Factor
	int nFastTmplYSkpFact;			// Fast Template Y Skip Factor
	BOOL bEnableFastFirst;			// Enable Fast Search for 1st Template
	BOOL bEnableFastLast;			// Enable Fast Search for Last Template
	int m_nMarkMaskDilationSize;
}SYM_INSP_PARM_SET;


// Overall Mark Inspection Parameters
typedef struct {
	BOOL bEnableMarkInspect;		// Enable/Disable Mark Inspection
	int nTmplSet;					// Number of different Templates Set for Mark Inspection
	int nTotalSymSet;				// Number of different Symbol Set within a Template Set
	SYM_INSP_PARM_SET SymInspParmSet[MAX_SYMBOLSET];
}MARK_INSP_PARM;


//////////////////////////////////////////////////////////////////////////////
// Mark Teach
//////////////////////////////////////////////////////////////////////////////

// Single Templates Teach Info
typedef struct {
	CRect rectTeachWin;				// Template Position and Size
	long lPixelCount;				// Number of White/Black pixel count depending on White or Black Mark
//ZWS
	CORR_TEMPLATE HostMarkTmpl;
	CImageBuffer ImgMarkMask;
}TMPL_TCH_INFO;


// Single Symbol Set Teach Info
typedef struct {
	TMPL_TCH_INFO TchTmplInfo[MAX_TMPL_PER_MRKSET];
	int	TchRoughTmplIdx[MAX_TMPL_PER_MRKSET];
	int nTotalTaughtTmpl;			// Total No. of Taught Templates
	int nTchThres;					// Teach Threshold
	double dTchAngle;				// Angle of Marking
	CRect rectTchWin[MAX_TCH_WIN];	// Teach Window Rect for Marking
	int nFirstTemplate;				// Template ID in TchTmplInfo[] selected as 1st Template
	int nLastTemplate;				// Template ID in TchTmplInfo[] selected as Last Template
	int nLeftMostTemplate;
	int nRightMostTemplate;
}SYM_TCH_INFO_SET;


// Single Mark Set Teach Info
typedef struct {
	CImageBuffer *pCImageBufferRefImg;
	CImageBuffer *pCImageBufferRefTmpl;
	int nTaughtSymSet;				// No. of Symbol sets that has been taught
	SYM_TCH_INFO_SET SymTchInfoSet[MAX_SYMBOLSET];
}MARK_TCH_INFO_SET;


// Overall Mark Teaching Information
typedef struct {
	int nTaughtMarkSet;				// No. of Mark Template sets that has been taught
	MARK_TCH_INFO_SET MarkTchInfoSet[MAX_MARKSET];
	CPoint2D pt2DTchPkgCenter;
	double dTchPkgAngle;
}MARK_TCH_INFO;

//////////////////////////////////////////////////////////////////////////////
// Mark Inspection Result
//////////////////////////////////////////////////////////////////////////////

// Inspection Result of each Template
typedef struct {
	int nResult;					// 1:Pass,2:Fail Corr,3:Fail Compare,
									// 4:Not Found
	int nFndScore;					// Found Matching Score
	CPoint ptFndTmplPos;			// Found Template Position
	BOOL bHasCompared;				// Has Template Been Complared
	int nTmplInspThres;				// Template Inspect Threshold after
									// Compensation
	double dExcessBlobSize;			// Found Excess Blob Size 
	CPoint ptFndExcessBlobPos;		// Found Excess Blob Pos
	double dMissingBlobSize;		// Found Missing Blob Size
	CPoint ptFndMissingBlobPos;		// Found Missing Blob Position
}TMPL_INSP_RSLT;


// Single Mark Set Inspection Result
typedef struct {
	TMPL_INSP_RSLT TmplInspRslt[MAX_TMPL_PER_MRKSET];
	int nGlobalInspThres;
	double dAngleOffset;
}SYM_INSP_RSLT_SET;


// Single Symbol Set Inspection Result
typedef struct {
	int nInspTmplSet;
	SYM_INSP_RSLT_SET SymInspRsltSet[MAX_SYMBOLSET];
}MARK_INSP_RSLT_SET;


// Overall Mark Inspection Results
typedef struct {
	int nInspMarkSet;
	MARK_INSP_RSLT_SET MarkInspRsltSet[MAX_MARKSET];
}MARK_INSP_RESULT;

#endif
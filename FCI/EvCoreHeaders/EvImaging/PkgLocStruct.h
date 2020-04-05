#ifndef _PKG_LOC_STRUCT
#define _PKG_LOC_STRUCT

#include "stdafx.h"
#include "MarkStruct.h"

#include "Imaging.h"

#define INNER	0
#define OUTER	1
#define MVS_NORMAL	2

#define ROUGHCORR	0
#define ROUGHSCAN	1

typedef struct {
	CPoint2D pt2DFirst;
	CPoint2D pt2DLast;
	CPoint2D pt2DPkgCtr;
	double dRatio;
	double dDist;
	double dDir;
	double dAngle;
} PKG_ROUGH_LOC_TEACH_INFO;
//////////////////////////////////////////////////////////////////////////////
// Package Location Parameters
//////////////////////////////////////////////////////////////////////////////
typedef struct {					// Search Parameters
	BOOL bEnablePkgLoc;				// Enable Package Location
	BOOL bEnableRoughPkgLoc;		// Enable the Rough Package Loc using Correlation
	int nRoughPkgLocMethod;			// Enable Rough Correlation Method or Scanning Method
	double dMaxTiltAngle;			// Max Device Tilt Angle (HT Angle Deviation)
	int	nXPkgShiftTol;				// X Package Shift Tolerance
	int	nYPkgShiftTol;				// Y Package Shift Tolerance
	CRect rectTchWin;				// Device Teach Window Pos and Size
	CRectangle2D rc2DTchWin;
	int nTchPkgThres;				// Pkg Teach Threshold
	double dAngleStep;				// Hough Transform Angle Step
	int nRoughLocateTopEdgeLevel;	// Rough Edge Location Top Edge Level
	int nRoughLocateLeftEdgeLevel;	// Rough Edge Location Top Edge Level
	// New Stuff
	CImageBuffer *pCImageBufferRefImg;		// Pointer to Host Reference Template Buffer
	BOOL bTeachOkay;				// Check if Package Location is Taught
	int nCorrSensitivity;			// Correlation Match Sensitivity 
	int nTmplXSkpFact;				// Template X Skip Factor
	int nTmplYSkpFact;				// Template Y Skip Factor
	CRect rectTopLftTch;			// Top Left Teach Position
	CRect rectTopLftTmpl;			// Top Left Template Position
	CRect rectBotRhtTch;			// Bottom Right Teach Position
	CRect rectBotRhtTmpl;			// Bottom Right Template Position
	double dTaughtDeviceAngle;		// Taught Device Angle
	int nGridEdgeSearchLength;		// Search Length for GetAdvGridEdgeIII()
	int nTopLinePt;					// Numbers of Top Edge Points to used for HT
	int nBotLinePt;					// Numbers of Bottom Edge Points to used for HT
	int nLftLinePt;					// Numbers of Left Edge Points to used for HT
	int nRhtLinePt;					// Numbers of Right Edge Points to used for HT
	TMPL_TCH_INFO	PkgTopLftTchTmplInfo;	// Teach Template Information of Top-Left Corner of Package;
	TMPL_TCH_INFO	PkgBotRhtTchTmplInfo;	// Teach Template Information of Bottom-Right Corner of Package;

	BOOL bEnableFindTop;
	BOOL bEnableFindBottom;
	BOOL bEnableFindLeft;
	BOOL bEnableFindRight;

	BOOL bEnableFixedHeight;
	BOOL bEnableFixedWidth;
	double dPkgHeight;
	double dPkgWidth;
	PKG_ROUGH_LOC_TEACH_INFO PkgRoughLocTeachInfo;

	BYTE byteBinTchThres;

} PKG_LOC_PARM;


class PKG_LOC_EDGE_PARM
{
public:
	PKG_LOC_EDGE_PARM();
	~PKG_LOC_EDGE_PARM();
//	MULTI_EDGE_SCAN_PARM MultiEdgeParm; // Multi Edge Paramaters
	MULTI_EDGE_SLANT_SCAN_PARM MultiEdgeParm; // Multi Edge Paramaters
	int nOnLineVarRatio;				// The OnLine Variance Ratio
	int nEdgeSelection;					// Inner Edge / Outer Edge
	int nFilterType[6];
	EDGE_FILTER_PARM EdgeFilter;		// Edge Filter Parameter 
};

class PKG_ADV_LOC_PARM
{
public:
	PKG_ADV_LOC_PARM();
	~PKG_ADV_LOC_PARM();
	int nXShiftTol;						// X-direction Shift Tolerace
	int nYShiftTol;						// Y-direction Shift Tolerance
	PKG_LOC_EDGE_PARM TopParm;			// Top Edge & Filter Parameters
	PKG_LOC_EDGE_PARM BotParm;			// Bot Edge & Filter Parameters
	PKG_LOC_EDGE_PARM LftParm;			// Lft Edge & Filter Parameters
	PKG_LOC_EDGE_PARM RhtParm;			// Rht Edge & Filter Parameters
	long lProcMode;						// The Process Mode to Select Edge detection
	double dSigma;						// Sigma for the Gaussian Filter
	double dOnLineTol;					// On Line Tolerence
};

//////////////////////////////////////////////////////////////////////////////
// Package Location Teach
//////////////////////////////////////////////////////////////////////////////
typedef struct {					// Package Teach Window Position
	CPoint ptPkgTchCenter;			// Pkg Teach Center Position
	CRect rectTchWin;				// Bottom Side Search Window
	int nTchPkgThres;				// Pkg Teach Threshold
	CPoint ptFndTopLft;				// Fnd TopLeft Position
	CPoint ptFndTopRht;				// Fnd TopRight Position
	CPoint ptFndBotLft;				// Fnd BottomLeft Position
	CPoint ptFndBotRht;				// Fnd BottomRight Position
	double dFndPkgAngle;			// Fnd Package Angle
} PKG_LOC_TCH_PARM;


//////////////////////////////////////////////////////////////////////////////
// Package Location Result
//////////////////////////////////////////////////////////////////////////////
class PKG_LOC_RESULT
{					// Package Location Results
public:
	PKG_LOC_RESULT();
	~PKG_LOC_RESULT();
	void Rotate(double dAngle, CPoint2D* ppt2DRotCtr);
	int GetRotROIRefPkg(CRectangle2D* prc2DPkgTeach,
						CRect* prcTeachROI,
						CRectangle2D* prc2DInspROI);
	PKG_LOC_RESULT(CPoint2D* ppt2DTopLft,
				   CPoint2D* ppt2DTopRht,
				   CPoint2D* ppt2DBotRht,
				   CPoint2D* ppt2DBotLft);
	CPoint2D ptPkgFoundCenter;		// Pkg Found Center Position
	double dPkgFoundAngle;			// Pkg Found Angle
	double dFndXOffset;				// X Difference between Teach and Found Position
	double dFndYOffset;				// Y Difference between Teach and Found Position

	CRectangle2D rc2DPkg;

	CLine2D LineTop,				// Top Side Line
			LineBottom,				// Bottom Side Line
			LineLeft,				// Left Side Line
			LineRight;				// Right Side Line

	CLine2D lineVert,				// This Line is used for Vertical Tipt2Tip Measurement
			lineHorz;				// This Line is used for Horizontal Tipt2Tip Measurement
									
	CPoint2D ptTopLft,				// Pkg TopLeft Hand Corner Coordinates
			 ptTopRht,				// Pkg TopRight Hand Corner Coordinates
			 ptBotLft,				// Pkg BottomLeft Hand Corner Coordinates
			 ptBotRht;				// Pkg BottomRight Hand corner Coordinates
	
	CPoint2D ptTopCntr,				// Pkg TopLine Center Coordinates
			 ptBotCntr,				// Pkg BottomLine Center Coordinates
			 ptLftCntr,				// Pkg LeftLine Center Coordinates
			 ptRhtCntr;				// Pkg RightLine Center Coordinates

};

	
class CMarkInspParm 
{
public:
	CMarkInspParm();
	~CMarkInspParm();
public:
	BOOL	bEnableMarkInsp;
	int		nAcceptScore;
	int		nRejectScore;
	double	dAngleStep;
	int		nSymbolContrast;
	int		nMarkRotation;
	double	dOtherExcessArea;
	double	dOtherMissingArea;
	int		nMarkMatchPixInMisMatchSqr;
	int		nFirstTempShiftXTol;
	int		nFirstTempShiftYTol;
	int		nOtherTempShiftXTol;
	int		nOtherTempShiftYTol;
	int		nMinCharSize;
	int		nNoOfTemplates;
	int		nPkgLocDilationX;
	int		nPkgLocDilationY;
	int		nSymbolHeight;
	int		nSymbolPitch;
	double	dMaxTiltAngle;
	int		nShitXTolerance;
	int		nShitYTolerance;
};
		
#endif;
#ifndef _MARK_TOOL_STRUCT
#define _MARK_TOOL_STRUCT

#include "ImageStruct.h"

class CORR_PROJ_DATA
{
public:
	// Functions
	CORR_PROJ_DATA();
	~CORR_PROJ_DATA();
	void Clean();

	// Variables
	int nSize;						// Size of Proj Data
	long *plProj;					// Pointer to PRoj Data
	double dSumProj;				// Sum of Proj Data
	double dSumSqrProj;				// Sum of Proj Data Square
	double dNormalProj;				// Normalise Corr Score
};

class CORR_PROJ_PATTERN
{
public:
	// Functions
	CORR_PROJ_PATTERN();
	~CORR_PROJ_PATTERN();
	void Clean();

	// Variables
	CRect rcOrg;					// ROI of Inspection Image in FOV
	CSize csShift;					// Size of Searching Region

	CImageBuffer ImgBuf;					// Pointer to Template Image
	CRect rCImageBuffer;					// ROI of Inspection Image ( TopLeft Point is set by (0,0))
	double dSum;					// 2D Sum 
	double dSumSqr;					// 2D Sum of Square
	double dNormal;					// 2D Normalise Parameter 
	CORR_PROJ_DATA ProjX;			// 1D X Direction Info
	CORR_PROJ_DATA ProjY;			// 1D Y Direction Info
};

class CORR_TEMPLATE
{
public:
	// Functions
	CORR_TEMPLATE();
	~CORR_TEMPLATE();
	void Clean();

	// Variables
	int nStepX;						// Sampling Step Size for X direction
	int nStepY;						// Sampling Step Size for Y direction
	int nProjDir;					// 0: Do X First, 1: Do Y First
	CORR_PROJ_PATTERN NonSampling;	//  Non-Sampling Corr Info
	CORR_PROJ_PATTERN Sampling;		// Sampling Corr Info
};

#endif
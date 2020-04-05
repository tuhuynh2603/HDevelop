#include "stdAfx.h"
#include "MarkToolStruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CORR_PROJ_DATA::CORR_PROJ_DATA()
{
	nSize = 0;
	plProj = 0;
	dSumProj = 0;
	dSumSqrProj = 0;
	dNormalProj = 0;
}

CORR_PROJ_DATA::~CORR_PROJ_DATA()
{
	Clean();
}

void CORR_PROJ_DATA::Clean()
{
	nSize = 0;
	dSumProj = 0;
	dSumSqrProj = 0;
	dNormalProj = 0;

	if(plProj) {
		delete [] plProj;
		plProj = 0;
	}
}

CORR_PROJ_PATTERN::CORR_PROJ_PATTERN()
{
	rcOrg.SetRect(0,0,0,0);
	csShift = CSize(0,0);
	dSum = 0;
	dSumSqr = 0;
	dNormal = 0;
}

CORR_PROJ_PATTERN::~CORR_PROJ_PATTERN()
{
	Clean();
}

void CORR_PROJ_PATTERN::Clean()
{
	rcOrg.SetRect(0,0,0,0);
	dSum = 0;
	dSumSqr = 0;
	dNormal = 0;
	
	if (ImgBuf.IsAllocated())
		ImgBuf.Free();

	ProjX.Clean();
	ProjY.Clean();
}

CORR_TEMPLATE::CORR_TEMPLATE()
{
	nStepX = 0;
	nStepY = 0;
	nProjDir = 0;
}

CORR_TEMPLATE::~CORR_TEMPLATE()
{
	Clean();
}

void CORR_TEMPLATE::Clean()
{
	nStepX = 0;
	nStepY = 0;
	nProjDir = 0;

	NonSampling.Clean(); 
	Sampling.Clean();
}















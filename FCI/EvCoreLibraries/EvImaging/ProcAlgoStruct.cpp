#include "stdAfx.h"
#include "ProcAlgoStruct.h"
#include "CudaInterface.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////
///Centralized array allocation/deallocation
///Udupa; June'2014

template<typename TYPE> void createArray(TYPE*& pArray, int nSize, bool bOptimize, bool bInitialize)
{
	if(nSize < 0)
		nSize = -nSize;

	if(OptimizeCpu() && bOptimize) {
		int nAllocSize = sizeof(TYPE) * nSize;
		pArray = (TYPE*) malloc(nAllocSize);
		if(bInitialize)
			memset(pArray, 0, nAllocSize);
	}
	else
		pArray = new TYPE[nSize];
}
template void createArray<CPoint2D>(CPoint2D*& pArray, int nSize, bool bOptimize, bool bInitialize);
template void createArray<CPoint>(CPoint*& pArray, int nSize, bool bOptimize, bool bInitialize);
template void createArray<CPoint*>(CPoint**& pArray, int nSize, bool bOptimize, bool bInitialize);
template void createArray<int>(int*& pArray, int nSize, bool bOptimize, bool bInitialize);
template void createArray<long>(long*& pArray, int nSize, bool bOptimize, bool bInitialize);
template void createArray<double>(double*& pArray, int nSize, bool bOptimize, bool bInitialize);
template void createArray<BYTE>(BYTE*& pArray, int nSize, bool bOptimize, bool bInitialize);


template<typename TYPE> void deleteArray(TYPE*& pArray, bool bOptimize)
{
	if(pArray) {
		if(OptimizeCpu() && bOptimize)
			free(pArray);
		else
			delete[] pArray;
		pArray = NULL;
	}
}
template void deleteArray<CPoint2D>(CPoint2D*& pArray, bool bOptimize);
template void deleteArray<CPoint>(CPoint*& pArray, bool bOptimize);
template void deleteArray<CPoint*>(CPoint**& pArray, bool bOptimize);
template void deleteArray<int>(int*& pArray, bool bOptimize);
template void deleteArray<long>(long*& pArray, bool bOptimize);
template void deleteArray<double>(double*& pArray, bool bOptimize);
template void deleteArray<BYTE>(BYTE*& pArray, bool bOptimize);


/// Shallow copy
template<typename TYPE> void copyArray(TYPE* pDst, TYPE* pSrc, int nSize)
{
	int nAllocSize = sizeof(TYPE) * nSize;
	memcpy(pDst, pSrc, nAllocSize);
}
template void copyArray<int>(int* pSrc, int* pDst, int nSize);
template void copyArray<long>(long* pSrc, long* pDst, int nSize);
template void copyArray<double>(double* pSrc, double* pDst, int nSize);
template void copyArray<BYTE>(BYTE* pSrc, BYTE* pDst, int nSize);


///////////////////////////////////////////////////////////

CCudaInterface*& CudaIf()
{
	static CCudaInterface* gpCudaIf = NULL;
	return gpCudaIf;
}

BOOL IsOptimized()
{
	CCudaInterface* pCudaIf = CudaIf();
	return (pCudaIf && (pCudaIf->m_bOptimizeGpu || pCudaIf->m_bOptimizeCpu));
}

BOOL OptimizeGpu()
{
	CCudaInterface* pCudaIf = CudaIf();
	return pCudaIf && pCudaIf->m_bOptimizeGpu;
}

BOOL OptimizeCpu()
{
	CCudaInterface* pCudaIf = CudaIf();
	return pCudaIf && pCudaIf->m_bOptimizeCpu;
}



PROJ_PARM::PROJ_PARM()
{
	rectROI = CRect(0,0,0,0);
	nOrientation = ROW;
}

PROJ_PARM::~PROJ_PARM()
{
}

PROJ_SLANT_PARM::PROJ_SLANT_PARM()
{
	pt2DScanStart = CPoint2D(0,0);
	pt2DScanEnd = CPoint2D(0,0);
	pt2DProj = CPoint2D(0,0);
	bAccurate = 0; 
}

PROJ_SLANT_PARM::~PROJ_SLANT_PARM()
{
}

PROJ_RSLT::PROJ_RSLT()
{
	lMaxValue = 0;
	lMinValue = 0;
	nProjSize = 0;
	nFirstId = 0;
	nLastId = 0;
	dPOSEdgeShift = 0;
	dNEGEdgeShift = 0;
	lMagnify = 1;
	plProjVal = NULL;
}

void PROJ_RSLT::Clean()
{
	if(plProjVal)
		delete [] plProjVal;
	lMaxValue = 0;
	lMinValue = 0;
	nProjSize = 0;
	nFirstId = 0;
	nLastId = 0;
	dPOSEdgeShift = 0;
	dNEGEdgeShift = 0;
	lMagnify = 1;
	plProjVal = NULL;
}


PROJ_RSLT::~PROJ_RSLT()
{
	Clean();
}

PROJ_SLANT_RSLT::PROJ_SLANT_RSLT()
{
	nAbsScanWidth = 0;
	nAbsScanHeight = 0;
	dScanWidthStepX = 0;
	dScanWidthStepY = 0;
	dScanHeightStepX = 0;
	dScanHeightStepY = 0;
}

PROJ_SLANT_RSLT::~PROJ_SLANT_RSLT()
{
}


ADV_GRD_EDGE_FIND_PARM::ADV_GRD_EDGE_FIND_PARM()
{
	rectROI = CRect(0,0,0,0);			
	nOrientation = ROW;		
	nGradient = POS;			
	nThreshold = 20;			
	nGridSpacing = 1;		
	nDirection = FORWARD;			
	nEdgeFilterSize = 3;	
	nEdgeWidth = 1;			
	nMaxStepSize = 3;		
	nSmoothingMethod = BINOMIAL_SMOOTHING;	
}

ADV_GRD_EDGE_FIND_PARM::~ADV_GRD_EDGE_FIND_PARM()
{
}


ADV_BLOB_INFO::ADV_BLOB_INFO()
{
	pt2DFndBlobCenter = CPoint2D(0,0);
	rcFndBlobPos = CRect(0,0,0,0);
	nFndBlobArea = 0;
	pptBlobContour = NULL;
	nBlobContourSize = 0;
	dFndBlobLength = 0;
	dFndBlobElongation = 0;
	nBlobAvgIntensity = -1;
	prcHole = NULL;
	ppptBlobHoleContour = NULL;
	nBlobHole = 0;
	pnptBlobHoleContourSize = NULL;
}

void ADV_BLOB_INFO::Clean()
{
	int i;
	deleteArray(pptBlobContour);
	deleteArray(prcHole);
	if(ppptBlobHoleContour != NULL) {
		for(i=0;i<nBlobHole;i++)
			deleteArray(ppptBlobHoleContour[i]);
		deleteArray(ppptBlobHoleContour);
	}
	deleteArray(pnptBlobHoleContourSize, false);

	pt2DFndBlobCenter = CPoint2D(0,0);
	rcFndBlobPos = CRect(0,0,0,0);
	nFndBlobArea = 0;
	nBlobContourSize = 0;
	dFndBlobLength = 0;
	dFndBlobElongation = 0;
	nBlobHole = 0;
}
ADV_BLOB_INFO::~ADV_BLOB_INFO()
{
	Clean();
}

ADV_BLOB_INFO & ADV_BLOB_INFO::operator =(ADV_BLOB_INFO* pAdvBlobInfo) 
{
	int i, j;
	Clean();
	pt2DFndBlobCenter = pAdvBlobInfo->pt2DFndBlobCenter;
	rcFndBlobPos = pAdvBlobInfo->rcFndBlobPos;
	nFndBlobArea = pAdvBlobInfo->nFndBlobArea;
	nBlobContourSize = pAdvBlobInfo->nBlobContourSize;
	if(nBlobContourSize >0) {
		createArray(pptBlobContour, nBlobContourSize);
		if(OptimizeCpu())
			copyArray(pptBlobContour, pAdvBlobInfo->pptBlobContour, nBlobContourSize);
		else
			for(i = 0; i < nBlobContourSize; i++)
				pptBlobContour[i] = pAdvBlobInfo->pptBlobContour[i];
	}
	else
		pptBlobContour = NULL;
	dFndBlobLength = pAdvBlobInfo->dFndBlobLength;
	dFndBlobElongation = pAdvBlobInfo->dFndBlobElongation;
	nBlobHole = pAdvBlobInfo->nBlobHole;
	if(nBlobHole > 0) {
		createArray(ppptBlobHoleContour, nBlobHole);
		createArray(pnptBlobHoleContourSize, nBlobHole, false);
		createArray(prcHole, nBlobHole);
		for(i=0;i<nBlobHole;i++) {
			pnptBlobHoleContourSize[i] = pAdvBlobInfo->pnptBlobHoleContourSize[i];
			prcHole[i] = pAdvBlobInfo->prcHole[i];
			if(pnptBlobHoleContourSize[i] > 0) {
				createArray(ppptBlobHoleContour[i], pnptBlobHoleContourSize[i]);
				if(OptimizeCpu())
					copyArray(ppptBlobHoleContour[i], pAdvBlobInfo->ppptBlobHoleContour[i], pnptBlobHoleContourSize[i]);
				else
					for(j=0;j<pnptBlobHoleContourSize[i];j++)
						ppptBlobHoleContour[i][j] = pAdvBlobInfo->ppptBlobHoleContour[i][j];
			}
			else 
				pnptBlobHoleContourSize[i] = NULL;
		}
	}
	else {
		ppptBlobHoleContour = NULL;
		pnptBlobHoleContourSize = NULL;
		prcHole = NULL;
	}
	return *this;
}

ADV_BLOB_FIND_RSLT::ADV_BLOB_FIND_RSLT()
{
	pAdvBlobInfo = NULL;
	nFndBlobs = 0;
}

void ADV_BLOB_FIND_RSLT::Clean()
{
	int i;
	if(pAdvBlobInfo) {
		if(nFndBlobs <= NORMAL_BLOB_SIZE) {
			for(i = 0; i < NORMAL_BLOB_SIZE; i++) 
				pAdvBlobInfo[i].Clean();
		}
		else {
			for(i = 0; i < nFndBlobs; i++) 
				pAdvBlobInfo[i].Clean();
		}
		deleteArray(pAdvBlobInfo, false);
	}
	nFndBlobs = 0;
}

ADV_BLOB_FIND_RSLT::~ADV_BLOB_FIND_RSLT()
{
	Clean();
}


ADV_BLOB_FIND_RSLT & ADV_BLOB_FIND_RSLT::operator =(ADV_BLOB_FIND_RSLT* pAdvBlobFindRslt) 
{
	int i;
	Clean();
	nFndBlobs = pAdvBlobFindRslt->nFndBlobs;
	if(nFndBlobs>0) {
		if(nFndBlobs<=NORMAL_BLOB_SIZE)
			createArray(pAdvBlobInfo, NORMAL_BLOB_SIZE, false);
		else
			createArray(pAdvBlobInfo, nFndBlobs, false);
		if(OptimizeCpu())
			copyArray(pAdvBlobInfo, pAdvBlobFindRslt->pAdvBlobInfo, nFndBlobs);
		else
			for(i = 0; i < nFndBlobs; i++)
				pAdvBlobInfo[i] = pAdvBlobFindRslt->pAdvBlobInfo[i];
	}
	return *this;
}
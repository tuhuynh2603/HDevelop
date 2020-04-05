#include "stdafx.h"
#include "EdgeTool.h"
#include "ImageFunction.h"
#include "Utilities.h"
#include "TrigTable.h"
#include <math.h>
#include "ExternalFunctions.h"
#include "BitmapFunctions.h"
#include "MathDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CEdgeDetect::CEdgeDetect()
{
}

CEdgeDetect::~CEdgeDetect()
{
// dAryFndPOSEdge and dAryFndNEGEdge are not used in the new version of edge detection.
//	dAryFndPOSEdge.RemoveAll();
//	dAryFndNEGEdge.RemoveAll();
}

int CEdgeDetect::GetEdge(IMAGEVIEW_INFO *pView,
						 CImageBuffer *pImg,
						 EDGE_FIND_PARM *pParm,
						 EDGE_FILTER_PARM *pFilterParm,
						 long lProcMode,
						 long lEdgeCheckMode,
						 long lDebugFlag)
{
	// Get Projection
	PROJ_PARM ProjParm;		
	int i;
	int nProjSize,
		nError,
		nWidth,
		nStartPos;
	PROJ_RSLT ProjRsltOrg;
	BOOL bEdgePositive;
	BOOL bEdgeCheck;
	int nFndEdge;
	EDGE_CONTRAST_INFO* pEdgeContrastInfo;
	int nFndContrastEdge;
	long lProcContrastPOS, lProcContrastNEG;
	long* plFirstGradVal;
	long* plSecondGradVal;
	int nScanCentral;	
	
	if(pFilterParm->m_nTotalApplyFilter > 5) {
		ASSERT(!"m_nTotalApplyFilter is more than 5");
		return -1;
	}
	if(pParm->nEdgeLevel < 33) {
		ASSERT(!"pParm->nEdgeLevel is less than 33");
		return -1;
	}
	m_EdgeFindRslt.nFndPOSEdge = 0;
	if(m_EdgeFindRslt.pdFndPOSEdge) {
		delete [] m_EdgeFindRslt.pdFndPOSEdge;
		m_EdgeFindRslt.pdFndPOSEdge = NULL;
	}
	m_EdgeFindRslt.nFndNEGEdge = 0;
	if(m_EdgeFindRslt.pdFndNEGEdge) {
		delete [] m_EdgeFindRslt.pdFndNEGEdge;
		m_EdgeFindRslt.pdFndNEGEdge = NULL;
	}

	if(CheckRect(&pParm->rectROI,&pView->sizeImage))
		return -1;
	if (pParm->nOrientation==ROW) {
		nWidth = pParm->rectROI.Width();
		nStartPos = pParm->rectROI.left;
		ProjParm.nOrientation = COLUMN;
		nScanCentral = (pParm->rectROI.top + pParm->rectROI.bottom)/2;
	}
	else {
		nWidth = pParm->rectROI.Height();
		nStartPos = pParm->rectROI.top;
		ProjParm.nOrientation = ROW;
		nScanCentral = (pParm->rectROI.left + pParm->rectROI.right)/2;
	}
	EdgeGeneralParm.nStartPos = nStartPos;
	EdgeGeneralParm.nGradient = pParm->nGradient;
	EdgeGeneralParm.nDirection = pParm->nDirection;
	EdgeGeneralParm.nWhiteCount = pParm->nWhiteCount;
	EdgeGeneralParm.nBlackCount = pParm->nBlackCount;
	EdgeGeneralParm.nEdgeLevel = pParm->nEdgeLevel;
	EdgeGeneralParm.nContrast = pParm->nContrast;
	EdgeGeneralParm.nTerminationContrast = pParm->nTerminationContrast;

	ProjParm.rectROI = pParm->rectROI;
	nError = m_Projection.GetProj(pView,pImg,&ProjParm,lDebugFlag);
	if (nError)
		return -1;
	m_Projection.CopyProjRslt(&m_Projection.m_ProjRslt,&ProjRsltOrg);
	nProjSize = m_Projection.m_ProjRslt.nProjSize;
	if (nProjSize<=4) 
		return -1;
	pEdgeContrastInfo = new EDGE_CONTRAST_INFO[abs(m_Projection.m_ProjRslt.nProjSize)];
	plFirstGradVal = new long[abs(m_Projection.m_ProjRslt.nProjSize)];
	plSecondGradVal = new long[abs(m_Projection.m_ProjRslt.nProjSize)];
	ApplyFiltering(&m_Projection.m_ProjRslt,pFilterParm);
	lProcContrastPOS = m_Projection.m_ProjRslt.lMagnify * pParm->nContrast;
	lProcContrastNEG = -lProcContrastPOS;
	ScanEdgeContrast(&EdgeGeneralParm,
					 &m_Projection.m_ProjRslt,
					 plFirstGradVal,
					 plSecondGradVal,
					 lProcContrastPOS,
					 lProcContrastNEG,
					 pEdgeContrastInfo,
					 nFndContrastEdge);

	m_EdgeFindRslt.pdFndPOSEdge = new double [abs(nFndContrastEdge)];
	m_EdgeFindRslt.pdFndNEGEdge = new double [abs(nFndContrastEdge)];
	switch(lProcMode) {
		case GRADIENT_EDGE:
			GetGradientEdge(&m_Projection.m_ProjRslt,
							pEdgeContrastInfo,
							nFndContrastEdge,
							plSecondGradVal,
							&EdgeGeneralParm);
			break;
		case GRAVITY_EDGE: 		
			GetGravityEdge(&m_Projection.m_ProjRslt,
						   pEdgeContrastInfo,
						   nFndContrastEdge,
						   plFirstGradVal,
						   &EdgeGeneralParm);

			break;
		case LEVEL_EDGE: 
			GetLevelEdge(&m_Projection.m_ProjRslt, &EdgeGeneralParm);
			break;
	}
	delete [] pEdgeContrastInfo;
	delete [] plFirstGradVal;
	delete [] plSecondGradVal;
	if (pParm->nGradient==POS||pParm->nGradient==ANY) {
		bEdgePositive = TRUE;
		nFndEdge = 0;
		for(i = 0; i < m_EdgeFindRslt.nFndPOSEdge; i++) {
			bEdgeCheck = EdgeCheck(&ProjRsltOrg,
								   m_EdgeFindRslt.pdFndPOSEdge[i],
								   bEdgePositive,
								   &EdgeGeneralParm,
								   lEdgeCheckMode);
			if(bEdgeCheck == 0)
				m_EdgeFindRslt.pdFndPOSEdge[nFndEdge++] = m_EdgeFindRslt.pdFndPOSEdge[i];
		}
		m_EdgeFindRslt.nFndPOSEdge = nFndEdge;
		if((pParm->nDirection == BACKWARD) &&(m_EdgeFindRslt.nFndPOSEdge > 0)) {
			double* dPOSEdgeBak;
			dPOSEdgeBak = new double[abs(m_EdgeFindRslt.nFndPOSEdge)];
			for( i = 0; i < m_EdgeFindRslt.nFndPOSEdge; i++)
				dPOSEdgeBak[i] = m_EdgeFindRslt.pdFndPOSEdge[i];
			for( i = 0; i < m_EdgeFindRslt.nFndPOSEdge; i++)
				m_EdgeFindRslt.pdFndPOSEdge[i] = dPOSEdgeBak[m_EdgeFindRslt.nFndPOSEdge - 1 - i];
			delete [] dPOSEdgeBak;
		}
		if(lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_EDGE) {
			LOGPEN RedPenInfo;
			LOGPEN BluePenInfo;
			RedPenInfo.lopnColor = RGB(255,0,0);
			RedPenInfo.lopnStyle = PS_SOLID;
			RedPenInfo.lopnWidth.x = 1;
			BluePenInfo.lopnColor = RGB(0,0,255);
			BluePenInfo.lopnStyle = PS_SOLID;
			BluePenInfo.lopnWidth.x = 1;
			for(i = 0; i < nFndEdge; i++) {
				if(i==0) {
					if (pParm->nOrientation==ROW) 
						DrwCur(pView,&CPoint((int)m_EdgeFindRslt.pdFndPOSEdge[i],nScanCentral),&RedPenInfo,4);
					else
						DrwCur(pView,&CPoint(nScanCentral,(int)m_EdgeFindRslt.pdFndPOSEdge[i]),&RedPenInfo,4);
				}
				else {
					if (pParm->nOrientation==ROW) 
						DrwCur(pView,&CPoint((int)m_EdgeFindRslt.pdFndPOSEdge[i],nScanCentral),&BluePenInfo,4);
					else
						DrwCur(pView,&CPoint(nScanCentral,(int)m_EdgeFindRslt.pdFndPOSEdge[i]),&BluePenInfo,4);
				}
			}
		}		
	}
	if (pParm->nGradient==NEG||pParm->nGradient==ANY) {
		bEdgePositive = FALSE;
		nFndEdge = 0;
		for(i = 0; i < m_EdgeFindRslt.nFndNEGEdge; i++) {
			bEdgeCheck = EdgeCheck(&ProjRsltOrg,
								   m_EdgeFindRslt.pdFndNEGEdge[i],
								   bEdgePositive,
								   &EdgeGeneralParm,
								   lEdgeCheckMode);
			if(bEdgeCheck == 0)
				m_EdgeFindRslt.pdFndNEGEdge[nFndEdge++] = m_EdgeFindRslt.pdFndNEGEdge[i];
		}
		m_EdgeFindRslt.nFndNEGEdge = nFndEdge;
		if((pParm->nDirection == BACKWARD) &&(m_EdgeFindRslt.nFndNEGEdge > 0)){
			double* dNEGEdgeBak;
			dNEGEdgeBak = new double[abs(m_EdgeFindRslt.nFndNEGEdge)];
			for( i = 0; i < m_EdgeFindRslt.nFndNEGEdge; i++)
				dNEGEdgeBak[i] = m_EdgeFindRslt.pdFndNEGEdge[i];
			for( i = 0; i < m_EdgeFindRslt.nFndNEGEdge; i++)
				m_EdgeFindRslt.pdFndNEGEdge[i] = dNEGEdgeBak[m_EdgeFindRslt.nFndNEGEdge - 1 - i];
			delete [] dNEGEdgeBak;
		}
		if(lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_EDGE) {
			LOGPEN RedPenInfo;
			LOGPEN BluePenInfo;
			RedPenInfo.lopnColor = RGB(255,0,0);
			RedPenInfo.lopnStyle = PS_SOLID;
			RedPenInfo.lopnWidth.x = 1;
			BluePenInfo.lopnColor = RGB(0,0,255);
			BluePenInfo.lopnStyle = PS_SOLID;
			BluePenInfo.lopnWidth.x = 1;
			for(i = 0; i < nFndEdge; i++) {
				if(i==0) {
					if (pParm->nOrientation==ROW) 
						DrwCur(pView,&CPoint((int)m_EdgeFindRslt.pdFndNEGEdge[i],nScanCentral),&RedPenInfo,4);
					else
						DrwCur(pView,&CPoint(nScanCentral,(int)m_EdgeFindRslt.pdFndNEGEdge[i]),&RedPenInfo,4);
				}
				else {
					if (pParm->nOrientation==ROW) 
						DrwCur(pView,&CPoint((int)m_EdgeFindRslt.pdFndNEGEdge[i],nScanCentral),&BluePenInfo,4);
					else
						DrwCur(pView,&CPoint(nScanCentral,(int)m_EdgeFindRslt.pdFndNEGEdge[i]),&BluePenInfo,4);
				}
			}
		}		
	}
	return 0;
}


int CEdgeDetect::GetSlantScanEdge(IMAGEVIEW_INFO *pView,
								  CImageBuffer *pImg,
								  EDGE_SLANT_SCAN_PARM *pParm,
								  EDGE_FILTER_PARM *pFilterParm,
								  long lProcMode,
								  long lEdgeCheckMode,
								  long lDebugFlag,
								  CString* pstrDebug)
{
	// Get Projection
	PROJ_SLANT_PARM ProjParm;		
	int i;
	int nProjSize,
		nError,
		nStartPos;
	PROJ_RSLT ProjRsltOrg;
	BOOL bEdgePositive;
	BOOL bEdgeCheck;
	int nFndEdge;
	EDGE_CONTRAST_INFO* pEdgeContrastInfo;
	int nFndContrastEdge;
	long lProcContrastPOS, lProcContrastNEG;
	CLine2D ln2DParallel;
	CLine2D ln2DPerpendicular;
	CPoint2D pt2DStartAvg;
	long* plFirstGradVal;
	long* plSecondGradVal;

	m_EdgeFindRslt.nFndPOSEdge = 0;
	if(m_EdgeFindRslt.pdFndPOSEdge) {
		delete [] m_EdgeFindRslt.pdFndPOSEdge;
		m_EdgeFindRslt.pdFndPOSEdge = NULL;
	}
	m_EdgeFindRslt.nFndNEGEdge = 0;
	if(m_EdgeFindRslt.pdFndNEGEdge) {
		delete [] m_EdgeFindRslt.pdFndNEGEdge;
		m_EdgeFindRslt.pdFndNEGEdge = NULL;
	}

	m_EdgeSlantScanRslt.nFndPOSEdge = 0;
	if(m_EdgeSlantScanRslt.ppt2DFndPOSEdge) {
		delete [] m_EdgeSlantScanRslt.ppt2DFndPOSEdge;
		m_EdgeSlantScanRslt.ppt2DFndPOSEdge = NULL;
	}
	m_EdgeSlantScanRslt.nFndNEGEdge = 0;
	if(m_EdgeSlantScanRslt.ppt2DFndNEGEdge) {
		delete [] m_EdgeSlantScanRslt.ppt2DFndNEGEdge;
		m_EdgeSlantScanRslt.ppt2DFndNEGEdge = NULL;
	}

	nStartPos = 0;
	ProjParm.ln2DScan = pParm->ln2DScan;
	ProjParm.pt2DScanStart = pParm->pt2DScanStart;
	ProjParm.pt2DScanEnd = pParm->pt2DScanEnd;
	ProjParm.pt2DProj = pParm->pt2DProj;
	ProjParm.bAccurate = pParm->bAccurate;

	EdgeGeneralParm.nStartPos = nStartPos;
	EdgeGeneralParm.nGradient = pParm->nGradient;
	EdgeGeneralParm.nDirection = pParm->nDirection;
	EdgeGeneralParm.nWhiteCount = pParm->nWhiteCount;
	EdgeGeneralParm.nBlackCount = pParm->nBlackCount;
	EdgeGeneralParm.nEdgeLevel = pParm->nEdgeLevel;
	EdgeGeneralParm.nContrast = pParm->nContrast;
	EdgeGeneralParm.nTerminationContrast = pParm->nTerminationContrast;
	nError = m_Projection.GetSlantProj(pView,pImg,&ProjParm,lDebugFlag);
	if (nError)
		return -1;
	m_Projection.CopyProjRslt(&m_Projection.m_ProjSlantRslt.ProjRslt,&ProjRsltOrg);
	nProjSize = m_Projection.m_ProjSlantRslt.ProjRslt.nProjSize;
	if (nProjSize<=4) 
		return -1;
	pEdgeContrastInfo = new EDGE_CONTRAST_INFO[abs(nProjSize)];
	plFirstGradVal = new long[abs(nProjSize)];
	plSecondGradVal = new long[abs(nProjSize)];
	ApplyFiltering(&m_Projection.m_ProjSlantRslt.ProjRslt,pFilterParm);
	lProcContrastPOS = m_Projection.m_ProjSlantRslt.ProjRslt.lMagnify * pParm->nContrast;
	lProcContrastNEG = -lProcContrastPOS;
	ScanEdgeContrast(&EdgeGeneralParm,
					 &m_Projection.m_ProjSlantRslt.ProjRslt,
					 plFirstGradVal,
					 plSecondGradVal,
					 lProcContrastPOS,
					 lProcContrastNEG,
					 pEdgeContrastInfo,
					 nFndContrastEdge);
	m_EdgeFindRslt.pdFndPOSEdge = new double [abs(nFndContrastEdge)];
	m_EdgeFindRslt.pdFndNEGEdge = new double [abs(nFndContrastEdge)];
	m_EdgeSlantScanRslt.ppt2DFndPOSEdge = new CPoint2D[abs(nFndContrastEdge)];
	m_EdgeSlantScanRslt.ppt2DFndNEGEdge = new CPoint2D[abs(nFndContrastEdge)];
	switch(lProcMode) {
		case GRADIENT_EDGE:
			GetGradientEdge(&m_Projection.m_ProjSlantRslt.ProjRslt,
							pEdgeContrastInfo,
							nFndContrastEdge,
							plSecondGradVal,
							&EdgeGeneralParm);
			break;
		case GRAVITY_EDGE: 		
			GetGravityEdge(&m_Projection.m_ProjSlantRslt.ProjRslt,
						   pEdgeContrastInfo,
						   nFndContrastEdge,
						   plFirstGradVal,
						   &EdgeGeneralParm);
			break;
		case LEVEL_EDGE: 
			GetLevelEdge(&m_Projection.m_ProjSlantRslt.ProjRslt, &EdgeGeneralParm);
			break;
	}
	delete [] pEdgeContrastInfo;
	delete [] plFirstGradVal;
	delete [] plSecondGradVal;
	if(m_Projection.m_ProjSlantRslt.nAbsScanHeight > 1) {
		pt2DStartAvg.x = pParm->pt2DScanStart.x + 
			(m_Projection.m_ProjSlantRslt.nAbsScanHeight - 1) * m_Projection.m_ProjSlantRslt.dScanHeightStepX / 2.0;
		pt2DStartAvg.y = pParm->pt2DScanStart.y + 
			(m_Projection.m_ProjSlantRslt.nAbsScanHeight - 1) * m_Projection.m_ProjSlantRslt.dScanHeightStepY / 2.0;
	}
	else 
		pt2DStartAvg = pParm->pt2DScanStart;
	if (pParm->nGradient==POS||pParm->nGradient==ANY) {
		bEdgePositive = TRUE;
		nFndEdge = 0;
		for(i = 0; i < m_EdgeFindRslt.nFndPOSEdge; i++) {
			bEdgeCheck = EdgeCheck(&ProjRsltOrg,
								   m_EdgeFindRslt.pdFndPOSEdge[i],
								   bEdgePositive,
								   &EdgeGeneralParm,
								   lEdgeCheckMode);
			if(bEdgeCheck == 0) {
				m_EdgeSlantScanRslt.ppt2DFndPOSEdge[nFndEdge].x = pt2DStartAvg.x + 
					m_Projection.m_ProjSlantRslt.dScanWidthStepX * m_EdgeFindRslt.pdFndPOSEdge[i];
				m_EdgeSlantScanRslt.ppt2DFndPOSEdge[nFndEdge].y = pt2DStartAvg.y + 
					m_Projection.m_ProjSlantRslt.dScanWidthStepY * m_EdgeFindRslt.pdFndPOSEdge[i];
				nFndEdge++;
			}
		}
		m_EdgeSlantScanRslt.nFndPOSEdge = nFndEdge;
		if((pParm->nDirection == BACKWARD) && (m_EdgeSlantScanRslt.nFndPOSEdge > 0)){
			CPoint2D* ppt2DPOSEdgeBak;
			ppt2DPOSEdgeBak = new CPoint2D[abs(m_EdgeSlantScanRslt.nFndPOSEdge)];
			for( i = 0; i < m_EdgeSlantScanRslt.nFndPOSEdge; i++)
				ppt2DPOSEdgeBak[i] = m_EdgeSlantScanRslt.ppt2DFndPOSEdge[i];
			for( i = 0; i < m_EdgeSlantScanRslt.nFndPOSEdge; i++)
				m_EdgeSlantScanRslt.ppt2DFndPOSEdge[i] = ppt2DPOSEdgeBak[m_EdgeSlantScanRslt.nFndPOSEdge - 1 - i];
			delete [] ppt2DPOSEdgeBak;
		}
		if(lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_EDGE) {
			LOGPEN RedPenInfo;
			LOGPEN BluePenInfo;
			RedPenInfo.lopnColor = RGB(255,0,0);
			RedPenInfo.lopnStyle = PS_SOLID;
			RedPenInfo.lopnWidth.x = 1;
			BluePenInfo.lopnColor = RGB(0,0,255);
			BluePenInfo.lopnStyle = PS_SOLID;
			BluePenInfo.lopnWidth.x = 1;
			if(nFndEdge > 0) {
				DrwCur(pView,&CPoint(Round(m_EdgeSlantScanRslt.ppt2DFndPOSEdge[0].x), Round(m_EdgeSlantScanRslt.ppt2DFndPOSEdge[0].y)),&RedPenInfo,4);
				for(i = 1; i < nFndEdge; i++) 
					DrwCur(pView,&CPoint(Round(m_EdgeSlantScanRslt.ppt2DFndPOSEdge[i].x), Round(m_EdgeSlantScanRslt.ppt2DFndPOSEdge[i].y)),&BluePenInfo,4);
			}
			if(pstrDebug != NULL) 
				UpdateStatusBar(pstrDebug, 3);
		}
	}
	if (pParm->nGradient==NEG||pParm->nGradient==ANY) {
		bEdgePositive = FALSE;
		nFndEdge = 0;
		for(i = 0; i < m_EdgeFindRslt.nFndNEGEdge; i++) {
			bEdgeCheck = EdgeCheck(&ProjRsltOrg,
								   m_EdgeFindRslt.pdFndNEGEdge[i],
								   bEdgePositive,
								   &EdgeGeneralParm,
								   lEdgeCheckMode);
			if(bEdgeCheck == 0) {
				m_EdgeSlantScanRslt.ppt2DFndNEGEdge[nFndEdge].x = pt2DStartAvg.x + 
					m_Projection.m_ProjSlantRslt.dScanWidthStepX * m_EdgeFindRslt.pdFndNEGEdge[i];
				m_EdgeSlantScanRslt.ppt2DFndNEGEdge[nFndEdge].y = pt2DStartAvg.y + 
					m_Projection.m_ProjSlantRslt.dScanWidthStepY * m_EdgeFindRslt.pdFndNEGEdge[i];
				nFndEdge++;
			}
		}
		m_EdgeSlantScanRslt.nFndNEGEdge = nFndEdge;
		if((pParm->nDirection == BACKWARD) && (m_EdgeSlantScanRslt.nFndNEGEdge > 0)) {
			CPoint2D* ppt2DNEGEdgeBak;
			ppt2DNEGEdgeBak = new CPoint2D[abs(m_EdgeSlantScanRslt.nFndNEGEdge)];
			for( i = 0; i < m_EdgeSlantScanRslt.nFndNEGEdge; i++)
				ppt2DNEGEdgeBak[i] = m_EdgeSlantScanRslt.ppt2DFndNEGEdge[i];
			for( i = 0; i < m_EdgeSlantScanRslt.nFndNEGEdge; i++)
				m_EdgeSlantScanRslt.ppt2DFndNEGEdge[i] = ppt2DNEGEdgeBak[m_EdgeSlantScanRslt.nFndNEGEdge - 1 - i];
			delete [] ppt2DNEGEdgeBak;
		}
		if(lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_EDGE) {
			LOGPEN RedPenInfo;
			LOGPEN BluePenInfo;
			RedPenInfo.lopnColor = RGB(255,0,0);
			RedPenInfo.lopnStyle = PS_SOLID;
			RedPenInfo.lopnWidth.x = 1;
			BluePenInfo.lopnColor = RGB(0,0,255);
			BluePenInfo.lopnStyle = PS_SOLID;
			BluePenInfo.lopnWidth.x = 1;
			if(nFndEdge > 0) {
				DrwCur(pView,&CPoint(Round(m_EdgeSlantScanRslt.ppt2DFndNEGEdge[0].x), Round(m_EdgeSlantScanRslt.ppt2DFndNEGEdge[0].y)),&RedPenInfo,4);
				for(i = 1; i < nFndEdge; i++) 
					DrwCur(pView,&CPoint(Round(m_EdgeSlantScanRslt.ppt2DFndNEGEdge[i].x), Round(m_EdgeSlantScanRslt.ppt2DFndNEGEdge[i].y)),&BluePenInfo,4);
			}
			if(pstrDebug != NULL) 
				UpdateStatusBar(pstrDebug, 3);
		}		
	}
	m_EdgeFindRslt.nFndPOSEdge = 0;
	m_EdgeFindRslt.nFndNEGEdge = 0;
	delete [] m_EdgeFindRslt.pdFndPOSEdge;
	m_EdgeFindRslt.pdFndPOSEdge = NULL;
	delete [] m_EdgeFindRslt.pdFndNEGEdge;
	m_EdgeFindRslt.pdFndNEGEdge = NULL;
	return 0;
}


int CEdgeDetect::GetSlantScanEdge(IMAGEVIEW_INFO *pView,
								  CImageBuffer *pImg,
								  EDGE_SLANT_SCAN_PARM *pParm,
								  EDGE_FILTER_PARM *pFilterParm,
								  CProjection* pProjection,
								  long lProcMode,
								  long lEdgeCheckMode,
								  long lDebugFlag)
{
	  PROJ_SLANT_PARM ProjParm;		
	  int i;
	  int nProjSize,
		  nError,
		  nStartPos;
	  PROJ_RSLT ProjRsltOrg;
	  BOOL bEdgePositive;
	  BOOL bEdgeCheck;
	  int nFndEdge;
	  EDGE_CONTRAST_INFO* pEdgeContrastInfo;
	  int nFndContrastEdge;
	  long lProcContrastPOS, lProcContrastNEG;
	  CLine2D ln2DParallel;
	  CLine2D ln2DPerpendicular;
	  CPoint2D pt2DStartAvg;
	  long* plFirstGradVal;
	  long* plSecondGradVal;
	  CPoint2D* ppt2DPOSEdgeBak;
	  CPoint2D* ppt2DNEGEdgeBak;
	  
	  m_EdgeFindRslt.nFndPOSEdge = 0;
	  if(m_EdgeFindRslt.pdFndPOSEdge) {
		  delete [] m_EdgeFindRslt.pdFndPOSEdge;
		  m_EdgeFindRslt.pdFndPOSEdge = NULL;
	  }
	  m_EdgeFindRslt.nFndNEGEdge = 0;
	  if(m_EdgeFindRslt.pdFndNEGEdge) {
		  delete [] m_EdgeFindRslt.pdFndNEGEdge;
		  m_EdgeFindRslt.pdFndNEGEdge = NULL;
	  }
	  
	  m_EdgeSlantScanRslt.nFndPOSEdge = 0;
	  if(m_EdgeSlantScanRslt.ppt2DFndPOSEdge) {
		  delete [] m_EdgeSlantScanRslt.ppt2DFndPOSEdge;
		  m_EdgeSlantScanRslt.ppt2DFndPOSEdge = NULL;
	  }
	  m_EdgeSlantScanRslt.nFndNEGEdge = 0;
	  if(m_EdgeSlantScanRslt.ppt2DFndNEGEdge) {
		  delete [] m_EdgeSlantScanRslt.ppt2DFndNEGEdge;
		  m_EdgeSlantScanRslt.ppt2DFndNEGEdge = NULL;
	  }
	  
	  pEdgeContrastInfo = NULL;
	  plFirstGradVal = NULL;
	  plSecondGradVal = NULL;
	  ppt2DPOSEdgeBak = NULL;
	  ppt2DNEGEdgeBak = NULL;

	  nStartPos = 0;
	  ProjParm.ln2DScan = pParm->ln2DScan;
	  ProjParm.pt2DScanStart = pParm->pt2DScanStart;
	  ProjParm.pt2DScanEnd = pParm->pt2DScanEnd;
	  ProjParm.pt2DProj = pParm->pt2DProj;
	  ProjParm.bAccurate = pParm->bAccurate;
	  
	  EdgeGeneralParm.nStartPos = nStartPos;
	  EdgeGeneralParm.nGradient = pParm->nGradient;
	  EdgeGeneralParm.nDirection = pParm->nDirection;
	  EdgeGeneralParm.nWhiteCount = pParm->nWhiteCount;
	  EdgeGeneralParm.nBlackCount = pParm->nBlackCount;
	  EdgeGeneralParm.nEdgeLevel = pParm->nEdgeLevel;
	  EdgeGeneralParm.nContrast = pParm->nContrast;
	  EdgeGeneralParm.nTerminationContrast = pParm->nTerminationContrast;
	  
	  pProjection->CopyProjRslt(&pProjection->m_ProjSlantRslt.ProjRslt,&ProjRsltOrg);
	  nProjSize = pProjection->m_ProjSlantRslt.ProjRslt.nProjSize;
	  
	  if (nProjSize<=4) 
		  return -1;
	  pEdgeContrastInfo = new EDGE_CONTRAST_INFO[abs(nProjSize)];
	  plFirstGradVal = new long[abs(nProjSize)];
	  plSecondGradVal = new long[abs(nProjSize)];
	  ApplyFiltering(&pProjection->m_ProjSlantRslt.ProjRslt,pFilterParm);
	  lProcContrastPOS = pProjection->m_ProjSlantRslt.ProjRslt.lMagnify * pParm->nContrast;
	  lProcContrastNEG = -lProcContrastPOS;
	  nError = ScanEdgeContrast(&EdgeGeneralParm,
							  &pProjection->m_ProjSlantRslt.ProjRslt,
							  plFirstGradVal,
							  plSecondGradVal,
							  lProcContrastPOS,
							  lProcContrastNEG,
							  pEdgeContrastInfo,
							  nFndContrastEdge);
	  if(nError)
		  goto ErrorAbort;
	  m_EdgeFindRslt.pdFndPOSEdge = new double [abs(nFndContrastEdge)];
	  m_EdgeFindRslt.pdFndNEGEdge = new double [abs(nFndContrastEdge)];
	  m_EdgeSlantScanRslt.ppt2DFndPOSEdge = new CPoint2D[abs(nFndContrastEdge)];
	  m_EdgeSlantScanRslt.ppt2DFndNEGEdge = new CPoint2D[abs(nFndContrastEdge)];
	  switch(lProcMode) {
	  case GRADIENT_EDGE:
		  nError = GetGradientEdge(&pProjection->m_ProjSlantRslt.ProjRslt,
								  pEdgeContrastInfo,
								  nFndContrastEdge,
								  plSecondGradVal,
								  &EdgeGeneralParm);
		  if(nError)
			  goto ErrorAbort;
		  break;
	  case GRAVITY_EDGE: 		
		  nError = GetGravityEdge(&pProjection->m_ProjSlantRslt.ProjRslt,
								  pEdgeContrastInfo,
								  nFndContrastEdge,
								  plFirstGradVal,
								  &EdgeGeneralParm);
		  if(nError)
			  goto ErrorAbort;
		  break;
	  case LEVEL_EDGE: 
		  nError = GetLevelEdge(&pProjection->m_ProjSlantRslt.ProjRslt, &EdgeGeneralParm);
		  if(nError)
			  goto ErrorAbort;
		  break;
	  }
	  if(pProjection->m_ProjSlantRslt.nAbsScanHeight > 1) {
		  pt2DStartAvg.x = pParm->pt2DScanStart.x + 
			  (pProjection->m_ProjSlantRslt.nAbsScanHeight - 1) * pProjection->m_ProjSlantRslt.dScanHeightStepX / 2.0;
		  pt2DStartAvg.y = pParm->pt2DScanStart.y + 
			  (pProjection->m_ProjSlantRslt.nAbsScanHeight - 1) * pProjection->m_ProjSlantRslt.dScanHeightStepY / 2.0;
	  }
	  else 
		  pt2DStartAvg = pParm->pt2DScanStart;
	  if (pParm->nGradient==POS||pParm->nGradient==ANY) {
		  bEdgePositive = TRUE;
		  nFndEdge = 0;
		  for(i = 0; i < m_EdgeFindRslt.nFndPOSEdge; i++) {
			  bEdgeCheck = EdgeCheck(&ProjRsltOrg,
				  m_EdgeFindRslt.pdFndPOSEdge[i],
				  bEdgePositive,
				  &EdgeGeneralParm,
				  lEdgeCheckMode);
			  if(bEdgeCheck == 0) {
				  m_EdgeSlantScanRslt.ppt2DFndPOSEdge[nFndEdge].x = pt2DStartAvg.x + 
					  pProjection->m_ProjSlantRslt.dScanWidthStepX * m_EdgeFindRslt.pdFndPOSEdge[i];
				  m_EdgeSlantScanRslt.ppt2DFndPOSEdge[nFndEdge].y = pt2DStartAvg.y + 
					  pProjection->m_ProjSlantRslt.dScanWidthStepY * m_EdgeFindRslt.pdFndPOSEdge[i];
				  nFndEdge++;
			  }
		  }
		  m_EdgeSlantScanRslt.nFndPOSEdge = nFndEdge;
		  if((pParm->nDirection == BACKWARD) && (m_EdgeSlantScanRslt.nFndPOSEdge > 0)){
			  ppt2DPOSEdgeBak = new CPoint2D[abs(m_EdgeSlantScanRslt.nFndPOSEdge)];
			  for( i = 0; i < m_EdgeSlantScanRslt.nFndPOSEdge; i++)
				  ppt2DPOSEdgeBak[i] = m_EdgeSlantScanRslt.ppt2DFndPOSEdge[i];
			  for( i = 0; i < m_EdgeSlantScanRslt.nFndPOSEdge; i++)
				  m_EdgeSlantScanRslt.ppt2DFndPOSEdge[i] = ppt2DPOSEdgeBak[m_EdgeSlantScanRslt.nFndPOSEdge - 1 - i];
		  }
		  if(lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_EDGE) {
			  LOGPEN RedPenInfo;
			  LOGPEN BluePenInfo;
			  RedPenInfo.lopnColor = RGB(255,0,0);
			  RedPenInfo.lopnStyle = PS_SOLID;
			  RedPenInfo.lopnWidth.x = 1;
			  BluePenInfo.lopnColor = RGB(0,0,255);
			  BluePenInfo.lopnStyle = PS_SOLID;
			  BluePenInfo.lopnWidth.x = 1;
			  if(nFndEdge > 0) {
				  DrwCur(pView,&CPoint(Round(m_EdgeSlantScanRslt.ppt2DFndPOSEdge[0].x), Round(m_EdgeSlantScanRslt.ppt2DFndPOSEdge[0].y)),&RedPenInfo,4);
				  for(i = 1; i < nFndEdge; i++) 
					  DrwCur(pView,&CPoint(Round(m_EdgeSlantScanRslt.ppt2DFndPOSEdge[i].x), Round(m_EdgeSlantScanRslt.ppt2DFndPOSEdge[i].y)),&BluePenInfo,4);
			  }
		  }
	  }
	  if (pParm->nGradient==NEG||pParm->nGradient==ANY) {
		  bEdgePositive = FALSE;
		  nFndEdge = 0;
		  for(i = 0; i < m_EdgeFindRslt.nFndNEGEdge; i++) {
			  bEdgeCheck = EdgeCheck(&ProjRsltOrg,
				  m_EdgeFindRslt.pdFndNEGEdge[i],
				  bEdgePositive,
				  &EdgeGeneralParm,
				  lEdgeCheckMode);
			  if(bEdgeCheck == 0) 
			  {
				  m_EdgeSlantScanRslt.ppt2DFndNEGEdge[nFndEdge].x = pt2DStartAvg.x + 
					  pProjection->m_ProjSlantRslt.dScanWidthStepX * m_EdgeFindRslt.pdFndNEGEdge[i];
				  m_EdgeSlantScanRslt.ppt2DFndNEGEdge[nFndEdge].y = pt2DStartAvg.y + 
					  pProjection->m_ProjSlantRslt.dScanWidthStepY * m_EdgeFindRslt.pdFndNEGEdge[i];
				  nFndEdge++;
			  }
		  }
		  m_EdgeSlantScanRslt.nFndNEGEdge = nFndEdge;
		  if((pParm->nDirection == BACKWARD) && (m_EdgeSlantScanRslt.nFndNEGEdge > 0)) {
			  ppt2DNEGEdgeBak = new CPoint2D[abs(m_EdgeSlantScanRslt.nFndNEGEdge)];
			  for( i = 0; i < m_EdgeSlantScanRslt.nFndNEGEdge; i++)
				  ppt2DNEGEdgeBak[i] = m_EdgeSlantScanRslt.ppt2DFndNEGEdge[i];
			  for( i = 0; i < m_EdgeSlantScanRslt.nFndNEGEdge; i++)
				  m_EdgeSlantScanRslt.ppt2DFndNEGEdge[i] = ppt2DNEGEdgeBak[m_EdgeSlantScanRslt.nFndNEGEdge - 1 - i];
		  }
		  if(lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_EDGE) {
			  LOGPEN RedPenInfo;
			  LOGPEN BluePenInfo;
			  RedPenInfo.lopnColor = RGB(255,0,0);
			  RedPenInfo.lopnStyle = PS_SOLID;
			  RedPenInfo.lopnWidth.x = 1;
			  BluePenInfo.lopnColor = RGB(0,0,255);
			  BluePenInfo.lopnStyle = PS_SOLID;
			  BluePenInfo.lopnWidth.x = 1;
			  if(nFndEdge > 0) {
				  DrwCur(pView,&CPoint(Round(m_EdgeSlantScanRslt.ppt2DFndNEGEdge[0].x), Round(m_EdgeSlantScanRslt.ppt2DFndNEGEdge[0].y)),&RedPenInfo,4);
				  for(i = 1; i < nFndEdge; i++) 
					  DrwCur(pView,&CPoint(Round(m_EdgeSlantScanRslt.ppt2DFndNEGEdge[i].x), Round(m_EdgeSlantScanRslt.ppt2DFndNEGEdge[i].y)),&BluePenInfo,4);
			  }
		  }		
	  }
	  m_EdgeFindRslt.nFndPOSEdge = 0;
	  m_EdgeFindRslt.nFndNEGEdge = 0;
	  if(m_EdgeFindRslt.pdFndPOSEdge != NULL)
		  delete [] m_EdgeFindRslt.pdFndPOSEdge;
	  m_EdgeFindRslt.pdFndPOSEdge = NULL;
	  if(m_EdgeFindRslt.pdFndNEGEdge != NULL)
		  delete [] m_EdgeFindRslt.pdFndNEGEdge;
	  m_EdgeFindRslt.pdFndNEGEdge = NULL;
	  if(pEdgeContrastInfo != NULL)
		  delete [] pEdgeContrastInfo;
	  if(plFirstGradVal != NULL)
		  delete [] plFirstGradVal;
	  if(plSecondGradVal != NULL)
		  delete [] plSecondGradVal;
	  if(ppt2DPOSEdgeBak != NULL)
		  delete [] ppt2DPOSEdgeBak;
	  if(ppt2DNEGEdgeBak != NULL)
		  delete [] ppt2DNEGEdgeBak;
	  return 0;
ErrorAbort:
	  m_EdgeFindRslt.nFndPOSEdge = 0;
	  m_EdgeFindRslt.nFndNEGEdge = 0;
	  if(m_EdgeFindRslt.pdFndPOSEdge != NULL)
		  delete [] m_EdgeFindRslt.pdFndPOSEdge;
	  m_EdgeFindRslt.pdFndPOSEdge = NULL;
	  if(m_EdgeFindRslt.pdFndNEGEdge != NULL)
		  delete [] m_EdgeFindRslt.pdFndNEGEdge;
	  m_EdgeFindRslt.pdFndNEGEdge = NULL;
	  if(pEdgeContrastInfo != NULL)
		  delete [] pEdgeContrastInfo;
	  if(plFirstGradVal != NULL)
		  delete [] plFirstGradVal;
	  if(plSecondGradVal != NULL)
		  delete [] plSecondGradVal;
	  if(ppt2DPOSEdgeBak != NULL)
		  delete [] ppt2DPOSEdgeBak;
	  if(ppt2DNEGEdgeBak != NULL)
		  delete [] ppt2DNEGEdgeBak;
	  return -1;
}

int CEdgeDetect::EdgeCheck(PROJ_RSLT* pProjRslt,
						   double dEdge,
						   BOOL bPositive,
						   EDGE_GENERAL_PARM *pParm,
						   long lEdgeCheckMode)
{
	int i;
	int nFirstIdx,
		nLastIdx,
		nEdgePos;
	long lProcContrast;
	int nBlackEdgeStart;
	int nBlackEdgeEnd;
	int nWhiteEdgeStart;
	int nWhiteEdgeEnd;
	long lBlackMean;
	long lWhiteMean;
	int nTransition;
	int nError;

	if(lEdgeCheckMode == EDGE_NO_CHECK) 
		return 0;
	if((pParm->nBlackCount <= 0) || (pParm->nWhiteCount <= 0)) 
		return 0;
	nTransition = 1;
	lProcContrast = pProjRslt->lMagnify * pParm->nContrast;

	nEdgePos = (int)(dEdge + 0.5) - pParm->nStartPos;

	if (((pParm->nDirection==FORWARD)&&bPositive) ||((pParm->nDirection==BACKWARD)&&(!bPositive))) {
		nBlackEdgeStart = nEdgePos - nTransition - pParm->nBlackCount;
		nBlackEdgeEnd = nEdgePos - nTransition;
		nWhiteEdgeStart = nEdgePos + nTransition;
		nWhiteEdgeEnd = nEdgePos + nTransition + pParm->nWhiteCount;
	}
	else {
		nWhiteEdgeStart = nEdgePos - nTransition - pParm->nBlackCount;
		nWhiteEdgeEnd = nEdgePos - nTransition;
		nBlackEdgeStart = nEdgePos + nTransition;
		nBlackEdgeEnd = nEdgePos + nTransition + pParm->nWhiteCount;
	}
	nFirstIdx = pProjRslt->nFirstId;
	nLastIdx = pProjRslt->nLastId;
	if((nBlackEdgeStart < nFirstIdx) || (nWhiteEdgeStart < nFirstIdx)) {
		return -1;
	}
	if((nBlackEdgeEnd >= nLastIdx) || (nWhiteEdgeEnd >= nLastIdx)) {
		return -1;
	}
	if(lEdgeCheckMode == EDGE_MEAN_CHECK) {
		lBlackMean = 0;
		for(i = nBlackEdgeStart; i <= nBlackEdgeEnd; i++) 
			lBlackMean += pProjRslt->plProjVal[i];
		if((nBlackEdgeEnd - nBlackEdgeStart + 1) == 0)
			return -1;
		lBlackMean = lBlackMean/(nBlackEdgeEnd - nBlackEdgeStart + 1);
		lWhiteMean = 0;
		for(i = nWhiteEdgeStart; i <= nWhiteEdgeEnd; i++) 
			lWhiteMean += pProjRslt->plProjVal[i];
		if((nWhiteEdgeEnd - nWhiteEdgeStart + 1) == 0)
			return -1;
		lWhiteMean = lWhiteMean/(nWhiteEdgeEnd - nWhiteEdgeStart + 1);
		if((lWhiteMean - lBlackMean) < lProcContrast)
			return -1;
	}
	if(lEdgeCheckMode == EDGE_NEIGHBOR_CHECK) {
		nError = CheckContrastAndOtherEdges(pProjRslt,
											lProcContrast,
											nEdgePos,
											nBlackEdgeStart,
											nBlackEdgeEnd,
											nWhiteEdgeStart,
											nWhiteEdgeEnd);
		return nError;
	}
	return 0;
}


int CEdgeDetect::CheckContrastAndOtherEdges(PROJ_RSLT* pProjRslt,
											int nContrastTol,
											int nEdgePos,
											int nBlackEdgeStart,
											int nBlackEdgeEnd,
											int nWhiteEdgeStart,
											int nWhiteEdgeEnd)
{
	int i;
	long lGradMaxTol;
	long lGradTol;
	long lGradMinTol;
	long lGradBaseTol;
	long lGradBlack;
	int nBlackBasePos;
	int nWhiteBasePos;
	long lGradWhite;
	long lGrad1;
	long lGrad2;
	bool bFndBlackBase;
	bool bFndWhiteBase;
	int nStep;
	int nContrast;

	lGrad1 = abs(pProjRslt->plProjVal[nEdgePos]-pProjRslt->plProjVal[nEdgePos-1]);
	lGrad2 = abs(pProjRslt->plProjVal[nEdgePos]-pProjRslt->plProjVal[nEdgePos+1]);
	if(lGrad1 > lGrad2)
		lGradTol = lGrad1;
	else 
		lGradTol = lGrad2;
	lGradBaseTol = lGradTol / 3; 
	lGradMinTol = lGradTol / 2;
	lGradMaxTol = lGradTol * 2;
	if(nBlackEdgeEnd > nBlackEdgeStart) 
		nStep = 1;
	else
		nStep = -1;
	bFndBlackBase = FALSE;
	for(i = nBlackEdgeStart; i != nBlackEdgeEnd; i+=nStep) {
		lGradBlack = pProjRslt->plProjVal[i]-pProjRslt->plProjVal[i+nStep];
		if(lGradBlack > lGradMaxTol)
			return -1;
		if(lGradBlack < lGradBaseTol) {
			bFndBlackBase = TRUE;
			nBlackBasePos = i + nStep;
			break;
		}
	}
	if(bFndBlackBase) {
		for(i = nBlackBasePos; i != nBlackEdgeEnd; i+=nStep) {
			lGradBlack = abs(pProjRslt->plProjVal[i]-pProjRslt->plProjVal[i+nStep]);
			if(lGradBlack > lGradMinTol)
				return -1;
		}
	}
	if(nWhiteEdgeEnd > nWhiteEdgeStart) 
		nStep = 1;
	else
		nStep = -1;
	bFndWhiteBase = FALSE;
	for(i = nWhiteEdgeStart; i != nWhiteEdgeEnd; i+=nStep) {
		lGradWhite = pProjRslt->plProjVal[i+nStep] - pProjRslt->plProjVal[i];
		if(lGradWhite > lGradMaxTol)
			return -1;
		if(lGradWhite < lGradBaseTol) {
			bFndWhiteBase = TRUE;
			nWhiteBasePos = i + nStep;
			break;
		}
	}
	if(bFndWhiteBase) {
		for(i = nWhiteBasePos; i != nWhiteEdgeEnd; i+=nStep) {
			lGradWhite = abs(pProjRslt->plProjVal[i]-pProjRslt->plProjVal[i+nStep]);
			if(lGradWhite > lGradMinTol)
				return -1;
		}
	}
	if(bFndBlackBase && bFndWhiteBase) {
		nContrast = pProjRslt->plProjVal[nWhiteBasePos] - pProjRslt->plProjVal[nBlackBasePos];
		if(nContrast < nContrastTol)
			return -1;
	}
	return 0;
}

int CEdgeDetect::GetGradientEdge(PROJ_RSLT* pProjRslt,
								 EDGE_CONTRAST_INFO* pEdgeContrastInfo,
								 int nFndContrastEdge,
								 long* plSecondGradVal,
								 EDGE_GENERAL_PARM *pParm)
{
	int i;
	int nStartPos;
	double dExactPos;
	nStartPos = pParm->nStartPos;
	m_EdgeFindRslt.nFndPOSEdge = 0;
	m_EdgeFindRslt.nFndNEGEdge = 0;
	for(i = 0; i < nFndContrastEdge; i++) {
		if(plSecondGradVal[pEdgeContrastInfo[i].nZeroCrossingLftPos] != plSecondGradVal[pEdgeContrastInfo[i].nZeroCrossingRhtPos])
			dExactPos= pEdgeContrastInfo[i].nZeroCrossingLftPos + (double)plSecondGradVal[pEdgeContrastInfo[i].nZeroCrossingLftPos]/
				(double)(plSecondGradVal[pEdgeContrastInfo[i].nZeroCrossingLftPos] - plSecondGradVal[pEdgeContrastInfo[i].nZeroCrossingRhtPos]);
		else 
			dExactPos= (pEdgeContrastInfo[i].nZeroCrossingLftPos + pEdgeContrastInfo[i].nZeroCrossingRhtPos) / 2.0;
		if(((pParm->nDirection==FORWARD)&&(pEdgeContrastInfo[i].bPOSEdge)) ||
			((pParm->nDirection==BACKWARD)&&(!pEdgeContrastInfo[i].bPOSEdge))) {
			if(pParm->nDirection==FORWARD)
				m_EdgeFindRslt.pdFndPOSEdge[m_EdgeFindRslt.nFndPOSEdge++] = dExactPos + nStartPos - pProjRslt->dPOSEdgeShift;
			else
				m_EdgeFindRslt.pdFndPOSEdge[m_EdgeFindRslt.nFndPOSEdge++] = dExactPos + nStartPos + pProjRslt->dPOSEdgeShift;
		}
		else {
			if(pParm->nDirection==FORWARD)
				m_EdgeFindRslt.pdFndNEGEdge[m_EdgeFindRslt.nFndNEGEdge++] = dExactPos + nStartPos - pProjRslt->dNEGEdgeShift;
			else
				m_EdgeFindRslt.pdFndNEGEdge[m_EdgeFindRslt.nFndNEGEdge++] = dExactPos + nStartPos + pProjRslt->dNEGEdgeShift;
		}
	}
	return 0;
}

int CEdgeDetect::GetGravityEdge(PROJ_RSLT* pProjRslt,
								EDGE_CONTRAST_INFO* pEdgeContrastInfo,
								int nFndContrastEdge,
								long* plFirstGradVal,
								EDGE_GENERAL_PARM *pParm)
{
	int i;
	int nStartPos;
	int nError;
	double dExactPos;

	nStartPos = pParm->nStartPos;
	m_EdgeFindRslt.nFndPOSEdge = 0;
	m_EdgeFindRslt.nFndNEGEdge = 0;
	for(i = 0; i < nFndContrastEdge; i++) {
		nError = GetOneGravityEdge(pProjRslt,
								   &pEdgeContrastInfo[i],
								   plFirstGradVal,
								   pParm,
								   dExactPos);
		if(nError == -1)
			return -1;
		if(((pParm->nDirection==FORWARD)&&(pEdgeContrastInfo[i].bPOSEdge)) ||
			((pParm->nDirection==BACKWARD)&&(!pEdgeContrastInfo[i].bPOSEdge))) {
			if(pParm->nDirection==FORWARD)
				m_EdgeFindRslt.pdFndPOSEdge[m_EdgeFindRslt.nFndPOSEdge++] = dExactPos + nStartPos - pProjRslt->dPOSEdgeShift;
			else
				m_EdgeFindRslt.pdFndPOSEdge[m_EdgeFindRslt.nFndPOSEdge++] = dExactPos + nStartPos + pProjRslt->dPOSEdgeShift;
		}
		else {
			if(pParm->nDirection==FORWARD)
				m_EdgeFindRslt.pdFndNEGEdge[m_EdgeFindRslt.nFndNEGEdge++] = dExactPos + nStartPos - pProjRslt->dNEGEdgeShift;
			else
				m_EdgeFindRslt.pdFndNEGEdge[m_EdgeFindRslt.nFndNEGEdge++] = dExactPos + nStartPos + pProjRslt->dNEGEdgeShift;
		}
	}

	return 0;
}

int CEdgeDetect::GetOneGravityEdge(PROJ_RSLT* pProjRslt,
								   EDGE_CONTRAST_INFO* pEdgeContrastInfo,
								   long* plFirstGradVal,
								   EDGE_GENERAL_PARM *pParm,
								   double& dExactPos)

{
	int nFirstIdx,
		nLastIdx,
		i,
		j;
	BOOL bEdgePos,
		 bEdgeNeg,
		 bEdgePresent;
	double dX2Before, dX2Current;
	double dX3Before, dX3Current;
	long lThresPOS,
		 lThresNEG;
	double* dXPos;
	double* dYPos;
	double dx,dy,b,k,dWeight,dMoment;
	int nCntPt;
	double dXFirst,
		   dXLast;
	bool bPOSEdgeFound;
	bool bNEGEdgeFound;
	bool bForwardPOSEdge;
	bool bForwardNEGEdge;

	bForwardPOSEdge = (pEdgeContrastInfo->bPOSEdge)&&
		(((pParm->nDirection==FORWARD)&&(pParm->nGradient==POS)) ||
		((pParm->nDirection==BACKWARD)&&(pParm->nGradient==NEG)) ||
		(pParm->nGradient==ANY));
	bForwardNEGEdge = (!pEdgeContrastInfo->bPOSEdge)&&
		(((pParm->nDirection==FORWARD)&&(pParm->nGradient==NEG)) ||
		((pParm->nDirection==BACKWARD)&&(pParm->nGradient==POS)) ||
		(pParm->nGradient==ANY));
	if((((pParm->nDirection==FORWARD)&&(pEdgeContrastInfo->bPOSEdge)) ||
		((pParm->nDirection==BACKWARD)&&(!pEdgeContrastInfo->bPOSEdge))) &&
		(pParm->nGradient==NEG))
		return -1;
	if((((pParm->nDirection==FORWARD)&&(!pEdgeContrastInfo->bPOSEdge)) ||
		((pParm->nDirection==BACKWARD)&&(pEdgeContrastInfo->bPOSEdge))) &&
		(pParm->nGradient==POS))
		return -1;
	if(pParm->nEdgeLevel < 33) {
		return -1;
	}
	if(pEdgeContrastInfo->bBaseGradEdgeShape) {
		nFirstIdx = pEdgeContrastInfo->BaseGradEdgeShapeInfo.nFndLftPos;
		nLastIdx = pEdgeContrastInfo->BaseGradEdgeShapeInfo.nFndRhtPos;
		if(nFirstIdx > (nLastIdx - 1))
				return -1;
	}
	else {
		if(pParm->nEdgeLevel < 50)
			return -1;
		nFirstIdx = pEdgeContrastInfo->HalfGradEdgeShapeInfo.nFndLftPos;
		nLastIdx = pEdgeContrastInfo->HalfGradEdgeShapeInfo.nFndRhtPos;
		if(nFirstIdx > (nLastIdx - 1))
			return -1;
	}
	lThresPOS = (long)(pEdgeContrastInfo->lMaxContrast * (pParm->nEdgeLevel / 100.0));
	lThresNEG = (long)(pEdgeContrastInfo->lMinContrast * (pParm->nEdgeLevel / 100.0));
	bEdgePos=bEdgeNeg=bEdgePresent=FALSE;
	bPOSEdgeFound = FALSE;
	bNEGEdgeFound = FALSE;
	for (i = nFirstIdx; i <= nLastIdx; i++) {
		if(!bEdgePos) {
			if(bForwardPOSEdge) {
				if ((plFirstGradVal[i] <= lThresPOS) && (plFirstGradVal[i+1] > lThresPOS)) {
					bEdgePos = TRUE;
					dXFirst = InterpolX(CPoint2D(i, plFirstGradVal[i]),
										CPoint2D(i+1, plFirstGradVal[i+1]),
										lThresPOS);
				}
			}
			if(bForwardNEGEdge) {
				if ((plFirstGradVal[i] >= lThresNEG) && (plFirstGradVal[i+1] < lThresNEG)) {
					bEdgePos = TRUE;
					dXFirst = InterpolX(CPoint2D(i, plFirstGradVal[i]),
										CPoint2D(i+1, plFirstGradVal[i+1]),
										lThresNEG);
				}
			}
		}
		else {
			if(bForwardPOSEdge) {
				if ((plFirstGradVal[i] >= lThresPOS) && (plFirstGradVal[i+1] < lThresPOS)) {
					dXLast = InterpolX(CPoint2D(i, plFirstGradVal[i]),
										CPoint2D(i+1, plFirstGradVal[i+1]),
										lThresPOS);
					bPOSEdgeFound = TRUE;
				}
			}
			if(bForwardNEGEdge) {
				if ((plFirstGradVal[i] <= lThresNEG) && (plFirstGradVal[i+1] > lThresNEG)) {
					dXLast = InterpolX(CPoint2D(i, plFirstGradVal[i]),
										CPoint2D(i+1, plFirstGradVal[i+1]),
										lThresNEG);
					bNEGEdgeFound = TRUE;
				}
			}
		}
		if(bPOSEdgeFound||bNEGEdgeFound) 
			break;
	}
	if(bPOSEdgeFound||bNEGEdgeFound) {
		dXPos = new double[abs(pProjRslt->nProjSize)];
		dYPos = new double[abs(pProjRslt->nProjSize)];
		dXPos[0] = dXFirst;
		dYPos[0] = 0;
		nCntPt = 1;
		for(j = (int)dXFirst + 1; j <= (int) dXLast; j++) {
			dXPos[nCntPt] = j;
			dYPos[nCntPt] = plFirstGradVal[j] - lThresPOS;
			nCntPt++;
		}
		dXPos[nCntPt] = dXLast;
		dYPos[nCntPt] = 0;
		nCntPt++;
		dWeight = dMoment = 0;
		// Compute Moments And Weight
		dX2Before = dXPos[0] * dXPos[0];
		dX3Before = dX2Before * dXPos[0];
		for (j=1;j<nCntPt;j++) {
			dx = dXPos[j] - dXPos[j-1];
			dy = dYPos[j] - dYPos[j-1];
			if(dx == 0) {
				delete [] dXPos;
				delete [] dYPos;
				return -1;
			}
			k = dy/dx;
			b = dYPos[j] - k*dXPos[j];
			dX2Current = dXPos[j] * dXPos[j];
			dX3Current = dX2Current * dXPos[j];
			dWeight = dWeight + k * (dX2Current - dX2Before)/2 + b * (dXPos[j] - dXPos[j-1]);	
			dMoment = dMoment + k * (dX3Current - dX3Before)/3 + b * (dX2Current - dX2Before)/2;
			dX2Before = dX2Current;
			dX3Before = dX3Current;
		}
		if (bPOSEdgeFound&&bForwardPOSEdge&&dWeight) {
			if(dWeight == 0) {
				delete [] dXPos;
				delete [] dYPos;
				return -1;
			}
			dExactPos = dMoment/dWeight + 0.5;
		}
		if (bNEGEdgeFound&&bForwardNEGEdge&&dWeight) {
			if(dWeight == 0) {
				delete [] dXPos;
				delete [] dYPos;
				return -1;
			}
			dExactPos = dMoment/dWeight + 0.5;
		}
		delete [] dXPos;
		delete [] dYPos;
		return 0;
	}
	else 
		return -1;
}

int	CEdgeDetect::ScanEdgeContrast(EDGE_GENERAL_PARM *pParm,
								  PROJ_RSLT* pProjRslt,
								  long* plFirstGradVal,
								  long* plSecondGradVal,
								  long lPOSContrast,
								  long lNEGContrast,
								  EDGE_CONTRAST_INFO* pEdgeContrastInfo,
								  int &nFndContrastEdge)
{
	int i, j;
	int plContrast;
	long lMaxGrad;
	long lMinGrad;
	long lHalfGrad;
	long lBaseGrad;
	int nFirstId;
	int nLastId;
	long lMaxIntensity;
	long lMinIntensity;

	long lPOSContrastDivBy5;
	long lNEGContrastDivBy5;
	lPOSContrastDivBy5 = lPOSContrast/5;
	lNEGContrastDivBy5 = lNEGContrast/5;
	EDGE_CONTRAST_INFO EdgeContrastInfo;

	nFirstId = pProjRslt->nFirstId;
	nLastId = pProjRslt->nLastId;

	for (i=nFirstId; i < (nLastId - 1); i++) 
		plFirstGradVal[i]=pProjRslt->plProjVal[i+1] - pProjRslt->plProjVal[i];
	for (i=nFirstId+1; i < (nLastId - 1);i++) 
		plSecondGradVal[i]=plFirstGradVal[i]-plFirstGradVal[i-1];

	nFndContrastEdge = 0;
	if(((pParm->nDirection==FORWARD)&&(pParm->nGradient == POS)) ||
		((pParm->nDirection==BACKWARD)&&(pParm->nGradient == NEG)) ||
		(pParm->nGradient == ANY)) {
		for (i = nFirstId + 1; i < (nLastId - 2); i++) {
			if((plSecondGradVal[i] >= 0) && (plSecondGradVal[i+1] <= 0)) {
				EdgeContrastInfo.bPOSEdge = true;
				EdgeContrastInfo.nZeroCrossingLftPos = i;
				EdgeContrastInfo.nZeroCrossingRhtPos = i+1;
				if(plFirstGradVal[i] > plFirstGradVal[i+1]) 
					lMaxGrad = plFirstGradVal[i];
				else
					lMaxGrad = plFirstGradVal[i+1];
				if(lMaxGrad > lPOSContrastDivBy5) 
				{
					lHalfGrad = lMaxGrad / 2;
					lBaseGrad = lMaxGrad / 3;
					
					EdgeContrastInfo.lPOSContrast = lPOSContrast;
					EdgeContrastInfo.lNEGContrast = lNEGContrast;
					EdgeContrastInfo.lMaxContrast = lMaxGrad;
					EdgeContrastInfo.lMinContrast = 0;
					EdgeContrastInfo.HalfGradEdgeShapeInfo.lGrad = lHalfGrad;
					EdgeContrastInfo.BaseGradEdgeShapeInfo.lGrad = lBaseGrad;
					FndPOSEdgeLeft(&EdgeContrastInfo,plFirstGradVal,i,nFirstId,lMaxGrad);
					if(EdgeContrastInfo.HalfGradEdgeShapeInfo.bFndLftPos || 
						EdgeContrastInfo.BaseGradEdgeShapeInfo.bFndLftPos) 
						FndPOSEdgeRight(&EdgeContrastInfo,plFirstGradVal,i+1,nLastId,lMaxGrad);
					EdgeContrastInfo.bHalfGradEdgeShape = FALSE;
					EdgeContrastInfo.bBaseGradEdgeShape = FALSE;
					if(EdgeContrastInfo.HalfGradEdgeShapeInfo.bFndLftPos &&
						EdgeContrastInfo.HalfGradEdgeShapeInfo.bFndRhtPos) {
						lMaxIntensity = pProjRslt->plProjVal[EdgeContrastInfo.HalfGradEdgeShapeInfo.nFndLftPos];
						lMinIntensity = lMaxIntensity;
						for(j = EdgeContrastInfo.HalfGradEdgeShapeInfo.nFndLftPos + 1; j <= EdgeContrastInfo.HalfGradEdgeShapeInfo.nFndRhtPos; j++) {
							if(lMaxIntensity < pProjRslt->plProjVal[j])
								lMaxIntensity = pProjRslt->plProjVal[j];
							if(lMinIntensity > pProjRslt->plProjVal[j])
								lMinIntensity = pProjRslt->plProjVal[j];
						}
						plContrast = lMaxIntensity - lMinIntensity;
						if(plContrast > lPOSContrast)  
							EdgeContrastInfo.bHalfGradEdgeShape = true;
					}
					if(EdgeContrastInfo.BaseGradEdgeShapeInfo.bFndLftPos &&
						EdgeContrastInfo.BaseGradEdgeShapeInfo.bFndRhtPos) {
						lMaxIntensity = pProjRslt->plProjVal[EdgeContrastInfo.BaseGradEdgeShapeInfo.nFndLftPos];
						lMinIntensity = lMaxIntensity;
						for(j = EdgeContrastInfo.BaseGradEdgeShapeInfo.nFndLftPos + 1; j <= EdgeContrastInfo.BaseGradEdgeShapeInfo.nFndRhtPos; j++) {
							if(lMaxIntensity < pProjRslt->plProjVal[j])
								lMaxIntensity = pProjRslt->plProjVal[j];
							if(lMinIntensity > pProjRslt->plProjVal[j])
								lMinIntensity = pProjRslt->plProjVal[j];
						}
						plContrast = lMaxIntensity - lMinIntensity;
						if(plContrast > lPOSContrast) 
							EdgeContrastInfo.bBaseGradEdgeShape = true;
					}
					if((EdgeContrastInfo.bHalfGradEdgeShape) || (EdgeContrastInfo.bBaseGradEdgeShape))
						pEdgeContrastInfo[nFndContrastEdge++] = EdgeContrastInfo;
				}
			}
		}
	}
	if(((pParm->nDirection==FORWARD)&&(pParm->nGradient == NEG)) ||
		((pParm->nDirection==BACKWARD)&&(pParm->nGradient == POS)) ||
		(pParm->nGradient == ANY)) {
		for (i = nFirstId + 1; i < (nLastId - 2); i++) {
			if((plSecondGradVal[i] <= 0) && (plSecondGradVal[i+1] >= 0)) {
				EdgeContrastInfo.bPOSEdge = false;
				EdgeContrastInfo.nZeroCrossingLftPos = i;
				EdgeContrastInfo.nZeroCrossingRhtPos = i+1;
				if(plFirstGradVal[i] > plFirstGradVal[i+1]) 
					lMinGrad = plFirstGradVal[i+1];
				else
					lMinGrad = plFirstGradVal[i];
				if(lMinGrad < lNEGContrastDivBy5)
				{
					lHalfGrad = lMinGrad / 2;
					lBaseGrad = lMinGrad / 3;
					EdgeContrastInfo.lPOSContrast = lPOSContrast;
					EdgeContrastInfo.lNEGContrast = lNEGContrast;
					EdgeContrastInfo.lMaxContrast = 0;
					EdgeContrastInfo.lMinContrast = lMinGrad;
					EdgeContrastInfo.HalfGradEdgeShapeInfo.lGrad = lHalfGrad;
					EdgeContrastInfo.BaseGradEdgeShapeInfo.lGrad = lBaseGrad;
					FndNEGEdgeLeft(&EdgeContrastInfo,plFirstGradVal,i,nFirstId,lMinGrad);
					if(EdgeContrastInfo.HalfGradEdgeShapeInfo.bFndLftPos || 
						EdgeContrastInfo.BaseGradEdgeShapeInfo.bFndLftPos) 
						FndNEGEdgeRight(&EdgeContrastInfo,plFirstGradVal,i+1,nLastId,lMinGrad);
					EdgeContrastInfo.bHalfGradEdgeShape = FALSE;
					EdgeContrastInfo.bBaseGradEdgeShape = FALSE;
					if(EdgeContrastInfo.HalfGradEdgeShapeInfo.bFndLftPos &&
						EdgeContrastInfo.HalfGradEdgeShapeInfo.bFndRhtPos) {
						lMaxIntensity = pProjRslt->plProjVal[EdgeContrastInfo.HalfGradEdgeShapeInfo.nFndLftPos];
						lMinIntensity = lMaxIntensity;
						for(j = EdgeContrastInfo.HalfGradEdgeShapeInfo.nFndLftPos + 1; j <= EdgeContrastInfo.HalfGradEdgeShapeInfo.nFndRhtPos; j++) {
							if(lMaxIntensity < pProjRslt->plProjVal[j])
								lMaxIntensity = pProjRslt->plProjVal[j];
							if(lMinIntensity > pProjRslt->plProjVal[j])
								lMinIntensity = pProjRslt->plProjVal[j];
						}
						plContrast = lMinIntensity - lMaxIntensity;
						if(plContrast < lNEGContrast)
							EdgeContrastInfo.bHalfGradEdgeShape = true;
					}
					if(EdgeContrastInfo.BaseGradEdgeShapeInfo.bFndLftPos &&
						EdgeContrastInfo.BaseGradEdgeShapeInfo.bFndRhtPos) {
						lMaxIntensity = pProjRslt->plProjVal[EdgeContrastInfo.BaseGradEdgeShapeInfo.nFndLftPos];
						lMinIntensity = lMaxIntensity;
						for(j = EdgeContrastInfo.BaseGradEdgeShapeInfo.nFndLftPos + 1; j <= EdgeContrastInfo.BaseGradEdgeShapeInfo.nFndRhtPos; j++) {
							if(lMaxIntensity < pProjRslt->plProjVal[j])
								lMaxIntensity = pProjRslt->plProjVal[j];
							if(lMinIntensity > pProjRslt->plProjVal[j])
								lMinIntensity = pProjRslt->plProjVal[j];
						}
						plContrast = lMinIntensity - lMaxIntensity;
						if(plContrast < lNEGContrast) 
							EdgeContrastInfo.bBaseGradEdgeShape = true;
					}
					if((EdgeContrastInfo.bHalfGradEdgeShape) || (EdgeContrastInfo.bBaseGradEdgeShape))
						pEdgeContrastInfo[nFndContrastEdge++] = EdgeContrastInfo;
				}
			}
		}
	}
	return 0;
}

int	CEdgeDetect::FndNEGEdgeLeft(EDGE_CONTRAST_INFO* pEdgeContrastInfo,
								long* plFirstGradVal,
								int nStartPos,
								int nFirstId,
								long lMinGrad)
{
	int j;
	pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndLftPos = FALSE;
	pEdgeContrastInfo->BaseGradEdgeShapeInfo.bFndLftPos = FALSE;
	for(j = nStartPos; j >= nFirstId; j--) {
		if((!pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndLftPos) &&
			(plFirstGradVal[j] > pEdgeContrastInfo->HalfGradEdgeShapeInfo.lGrad)) {
			pEdgeContrastInfo->HalfGradEdgeShapeInfo.nFndLftPos = j;
			pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndLftPos = TRUE;
		}
		if(plFirstGradVal[j] > pEdgeContrastInfo->BaseGradEdgeShapeInfo.lGrad) {
			pEdgeContrastInfo->BaseGradEdgeShapeInfo.nFndLftPos = j;
			pEdgeContrastInfo->BaseGradEdgeShapeInfo.bFndLftPos = TRUE;
			break;
		}
		if(plFirstGradVal[j] < lMinGrad)
			break;
	}
	return 0;
}

int	CEdgeDetect::FndNEGEdgeRight(EDGE_CONTRAST_INFO* pEdgeContrastInfo,
								long* plFirstGradVal,
								int nStartPos,
								int nLastId,
								long lMinGrad)
{
	int j;
	pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndRhtPos = FALSE;
	pEdgeContrastInfo->BaseGradEdgeShapeInfo.bFndRhtPos = FALSE;
	for(j = nStartPos; j < nLastId; j++) {
		if((!pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndRhtPos) && 
			(plFirstGradVal[j] > pEdgeContrastInfo->HalfGradEdgeShapeInfo.lGrad)) {
			pEdgeContrastInfo->HalfGradEdgeShapeInfo.nFndRhtPos = j;
			pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndRhtPos = TRUE;
		}
		if(plFirstGradVal[j] > pEdgeContrastInfo->BaseGradEdgeShapeInfo.lGrad) {
			pEdgeContrastInfo->BaseGradEdgeShapeInfo.nFndRhtPos = j;
			pEdgeContrastInfo->BaseGradEdgeShapeInfo.bFndRhtPos = TRUE;
			break;
		}
		if(plFirstGradVal[j] < lMinGrad)
			break;
	}
	return 0;
}

int	CEdgeDetect::FndPOSEdgeLeft(EDGE_CONTRAST_INFO* pEdgeContrastInfo,
								long* plFirstGradVal,
								int nStartPos,
								int nFirstId,
								long lMaxGrad)
{
	int j;
	pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndLftPos = FALSE;
	pEdgeContrastInfo->BaseGradEdgeShapeInfo.bFndLftPos = FALSE;
	for(j = nStartPos; j >= nFirstId; j--) {
		if((!pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndLftPos) &&
			(plFirstGradVal[j] < pEdgeContrastInfo->HalfGradEdgeShapeInfo.lGrad)) {
			pEdgeContrastInfo->HalfGradEdgeShapeInfo.nFndLftPos = j;
			pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndLftPos = TRUE;
		}
		if(plFirstGradVal[j] < pEdgeContrastInfo->BaseGradEdgeShapeInfo.lGrad) {
			pEdgeContrastInfo->BaseGradEdgeShapeInfo.nFndLftPos = j;
			pEdgeContrastInfo->BaseGradEdgeShapeInfo.bFndLftPos = TRUE;
			break;
		}
		if(plFirstGradVal[j] > lMaxGrad)
			break;
	}
	return 0;
}

int	CEdgeDetect::FndPOSEdgeRight(EDGE_CONTRAST_INFO* pEdgeContrastInfo,
								long* plFirstGradVal,
								int nStartPos,
								int nLastId,
								long lMaxGrad)
{
	int j;
	pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndRhtPos = FALSE;
	pEdgeContrastInfo->BaseGradEdgeShapeInfo.bFndRhtPos = FALSE;
	for(j = nStartPos; j < nLastId; j++) {
		if((!pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndRhtPos) &&
			(plFirstGradVal[j] < pEdgeContrastInfo->HalfGradEdgeShapeInfo.lGrad)) {
			pEdgeContrastInfo->HalfGradEdgeShapeInfo.nFndRhtPos = j;
			pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndRhtPos = TRUE;
		}
		if(plFirstGradVal[j] < pEdgeContrastInfo->BaseGradEdgeShapeInfo.lGrad) {
			pEdgeContrastInfo->BaseGradEdgeShapeInfo.nFndRhtPos = j;
			pEdgeContrastInfo->BaseGradEdgeShapeInfo.bFndRhtPos = TRUE;
			break;
		}
		if(plFirstGradVal[j] > lMaxGrad)
			break;
	}
	return 0;
}

int CEdgeDetect::GetLevelEdge(PROJ_RSLT* pProjRslt,
							  EDGE_GENERAL_PARM *pParm)
{
	PROJ_PARM ProjParm;		
	int i,nStartPos;
	int nError;
	long lRange;			
	long lSrchLevel;		
	double dExactPos;
	int nFirstIdx,
		nLastIdx,
		nStep;

	long lMax,lMin;

	BOOL bEdgePos,
		 bEdgeNeg;

	nStartPos = pParm->nStartPos;
	if (pParm->nDirection==FORWARD) {
		nFirstIdx = pProjRslt->nFirstId;
		nStep=1;
		nLastIdx = pProjRslt->nLastId;
		if(nFirstIdx > nLastIdx)
			return -1;
	}
	else {
		nFirstIdx=pProjRslt->nLastId-1;
		nStep=-1;
		nLastIdx=pProjRslt->nFirstId;
		if(nFirstIdx < nLastIdx)
			return -1;
	}
	nError = GetLevelEdgeMinAndMax(pProjRslt,pParm->nContrast,pParm->nWhiteCount,pParm->nBlackCount,lMin,lMax);
	if(nError == -1) {
		return -1;
	}
	lRange = lMax - lMin;
	lSrchLevel = lMin + (pParm->nEdgeLevel * lRange) / 100;
	bEdgePos=FALSE;
	bEdgeNeg=FALSE;
	m_EdgeFindRslt.nFndPOSEdge = 0;
	m_EdgeFindRslt.nFndNEGEdge = 0;
	for (i=nFirstIdx+nStep; i!=nLastIdx-nStep; i+=nStep) {
		if ((pProjRslt->plProjVal[i] > lSrchLevel) &&
			(pProjRslt->plProjVal[i-nStep] <= lSrchLevel)) {
			if(nStep > 0) 
				bEdgePos=TRUE;
			else 
				bEdgeNeg=TRUE;
		}
		if ((pProjRslt->plProjVal[i] < lSrchLevel) &&
			(pProjRslt->plProjVal[i-nStep] >= lSrchLevel)) {
			if(nStep > 0) 
				bEdgeNeg=TRUE;
			else 
				bEdgePos=TRUE;
		}
		//Save found Edges
		if (bEdgePos && (pParm->nGradient==POS||pParm->nGradient==ANY)) {
			dExactPos = InterpolX(CPoint2D(i-nStep,pProjRslt->plProjVal[i-nStep]),
								  CPoint2D(i,pProjRslt->plProjVal[i]),
								  lSrchLevel);
			m_EdgeFindRslt.pdFndPOSEdge[m_EdgeFindRslt.nFndPOSEdge++] = dExactPos + nStartPos - pProjRslt->dPOSEdgeShift;
			bEdgePos=FALSE;
		}
		if (bEdgeNeg && (pParm->nGradient==NEG||pParm->nGradient==ANY)) {
			dExactPos = InterpolX(CPoint2D(i-nStep,pProjRslt->plProjVal[i-nStep]),
								  CPoint2D(i,pProjRslt->plProjVal[i]),
								  lSrchLevel);
			m_EdgeFindRslt.pdFndNEGEdge[m_EdgeFindRslt.nFndNEGEdge++] = dExactPos + nStartPos - pProjRslt->dNEGEdgeShift;
			bEdgeNeg=FALSE;
		}
	}
	return 0;
}

int	CEdgeDetect::GetLevelEdgeMinAndMax(PROJ_RSLT* pProjRslt,
									   int nContrast,
									   int nWhiteCount,
									   int nBlackCount,
									   long& lMin,
									   long& lMax)
{
	int i;
	int nDataNo;
	long *plData;
	int nHistNo;
	double dConvSize;
	long lContrast;
	bool bMinFound, bMaxFound;
	CAdvHistogram AdvHistogram;
	ADV_CONV_HIST_1D_RSLT AdvConvHist1DRslt;

	plData = new long[abs(pProjRslt->nProjSize)];

	lContrast = pProjRslt->lMagnify * nContrast;
	dConvSize = lContrast / 4;
	nDataNo = 0;
	for(i = pProjRslt->nFirstId; i < pProjRslt->nLastId; i++) {
		plData[nDataNo++] = pProjRslt->plProjVal[i];
	}
/*
	AdvHistogram.GetAccHist(plData,
							nDataNo,
							plHistCoordinate, 
							pnHistAcc,
							nHistNo);
	AdvHistogram.GetConvHist(plHistCoordinate, 
							 pnHistAcc,
							 nHistNo,
							 dConvSize,
							 pnHistConv,
							 nMax,
							 lMaxPos);
*/
	AdvHistogram.GetConvHist(plData,
							 nDataNo,
							 (int)dConvSize,
							 &AdvConvHist1DRslt);
    lMin = 0;
    lMax = 0;
	bMinFound = FALSE;
	bMaxFound = FALSE;
	nHistNo = AdvConvHist1DRslt.nHistNo;
	if(nHistNo > 0) {
		for(i = 0; i< nHistNo; i++) {
			if(AdvConvHist1DRslt.pnHist[i] > nBlackCount) {
				lMin = (AdvConvHist1DRslt.plPosStart[i] + AdvConvHist1DRslt.plPosEnd[i])/2;
				bMinFound = TRUE;
				break;
			}
		}
		for(i = nHistNo-1; i>=0 ; i--) {
			if(AdvConvHist1DRslt.pnHist[i] > nWhiteCount) {
				lMax = (AdvConvHist1DRslt.plPosStart[i] + AdvConvHist1DRslt.plPosEnd[i])/2;
				bMaxFound = TRUE;
				break;
			}
		}
	}
	delete [] plData;
	if((lMax-lMin) < lContrast) return -1;
	if(bMinFound == FALSE) return -1;
	if(bMaxFound == FALSE) return -1;
	return 0;
}

int CEdgeDetect::ApplyFiltering(PROJ_RSLT* pProjRslt,
								EDGE_FILTER_PARM *pFilterParm) 
{
	int i;
	int nFilterId;
	int nFilterMode;
	int nError;
	double dTotalFilterSum;
	long lTotalFilterSumTol;

	if(pProjRslt->lMaxValue == 0) {
		return -1;
	}
	lTotalFilterSumTol = LONG_MAX / pProjRslt->lMaxValue;
	if(pFilterParm->m_nTotalApplyFilter > 5) {
		return -1;
	}
	pFilterParm->InitUserDefFilterPam();
	dTotalFilterSum = 1;
	for(i = 0; i < pFilterParm->m_nTotalApplyFilter; i++) {
		nFilterId = pFilterParm->m_lApplyFilterSequence[i];
		if((nFilterId >= 0) && (nFilterId < 6)) {
			nFilterMode = pFilterParm->Filter[nFilterId].m_nFilterMode;
			switch(nFilterMode) {
				case BINOMIAL_FILTER:
				case GAUSSIAN_FILTER:
					dTotalFilterSum *= pFilterParm->Filter[nFilterId].m_nFilterSum;
					if(dTotalFilterSum > lTotalFilterSumTol) {
						dTotalFilterSum /= pFilterParm->Filter[nFilterId].m_nFilterSum;
						NormalizeData(pProjRslt, 
									  pFilterParm->Filter[nFilterId].m_nFilterSum);
					}
					nError = ApplyUserDefFiltering(pProjRslt, 
												   &pFilterParm->Filter[nFilterId]);
					if(nError)
						goto ErrorAbort;
					break;
				case MAX_FILTER: 		
					ApplyMaxFiltering(pProjRslt, 
									  &pFilterParm->Filter[nFilterId]);
					break;
				case MIN_FILTER: 
					ApplyMinFiltering(pProjRslt, 
									  &pFilterParm->Filter[nFilterId]);
					break;
				case MEAN_FILTER: 		
					break;
				case MEDIAN_FILTER: 		
					break;
			}
			if(nError == -1)
				return -1;
		}
	}
	return 0;
ErrorAbort:
	return -1;
}

int CEdgeDetect::NormalizeData(PROJ_RSLT* pProjRslt,  
							   int nNormalize)
{
	int i;
	if(nNormalize <= 0) {
		ASSERT(!"nNormalize is less than or equal to 0");
		return -1;
	}
	for(i = pProjRslt->nFirstId; i < pProjRslt->nLastId; i++) 
		pProjRslt->plProjVal[i] /= nNormalize;

	pProjRslt->lMagnify /= nNormalize;
	return 0;
}

int CEdgeDetect::ApplyUserDefFiltering(PROJ_RSLT* pProjRslt, 
									   FILTER_1D* UserFilter) {
	int i,j,k;
	long *plProjDst;
	long lSum;
	int* ptrFilter;
	int nFilterRadius;
	long lProjMin;
	long lProjMax;

	nFilterRadius = UserFilter->m_nFilterRadius;
	if(nFilterRadius < 0) {
		ASSERT(!"UserFilter->m_nFilterRadius is less than 0");
		return -1;
	}
	if(nFilterRadius > 0) {
		pProjRslt->nFirstId += UserFilter->m_nFilterRadius;
		pProjRslt->nLastId  -= UserFilter->m_nFilterRadius;
		if(pProjRslt->nFirstId > pProjRslt->nLastId) 
			return -1;
		plProjDst = new long[abs(pProjRslt->nProjSize)];
		ptrFilter = new int[abs(UserFilter->m_nFilterSize)];
		for(i=0;i<UserFilter->m_nFilterSize;i++)
			ptrFilter[i] = UserFilter->m_pnFilter[i];
		for(i = pProjRslt->nFirstId; i < pProjRslt->nLastId; i++) {
			lSum = 0;
			for (j=-nFilterRadius,k=0;j<=nFilterRadius;j++,k++) {
				lSum += pProjRslt->plProjVal[i+j] * ptrFilter[k]; 
			}
			plProjDst[i] = lSum;
		}

		lProjMin = plProjDst[pProjRslt->nFirstId];
		lProjMax = plProjDst[pProjRslt->nFirstId];
		for(i = pProjRslt->nFirstId; i < pProjRslt->nLastId; i++) {
			pProjRslt->plProjVal[i] = plProjDst[i];
			if(lProjMin > plProjDst[i])
				lProjMin = plProjDst[i];
			if(lProjMax < plProjDst[i])
				lProjMax = plProjDst[i];
		}
		pProjRslt->lMaxValue = lProjMax;
		pProjRslt->lMinValue = lProjMin;
		pProjRslt->dNEGEdgeShift += UserFilter->m_dNEGEdgeShift;
		pProjRslt->dPOSEdgeShift += UserFilter->m_dPOSEdgeShift;
		pProjRslt->lMagnify *= UserFilter->m_nFilterSum;
		delete [] ptrFilter;
		delete [] plProjDst;
	}
	return 0;
}

int CEdgeDetect::ApplyMinFiltering(PROJ_RSLT* pProjRslt,
								   FILTER_1D* MinFilter) {
	int i;
	long *plProjDst;
	long lMin;
	int nFilterRadius;
	int nLostPosInSlideWindow;
	int nAddedPosInSlideWindow;
	long lProjMin;
	long lProjMax;

	nFilterRadius = MinFilter->m_nFilterRadius;
	if(nFilterRadius < 0) {
		ASSERT(!"MinFilter->m_nFilterRadius is less than 0");
		return -1;
	}
	if(nFilterRadius > 0) {
		plProjDst = new long[abs(pProjRslt->nProjSize)];
		MinInSlideWindow(pProjRslt,
						 pProjRslt->nFirstId + nFilterRadius,
						 nFilterRadius,
						 lMin);
		if((pProjRslt->nFirstId + MinFilter->m_nFilterRadius) > (pProjRslt->nLastId  - MinFilter->m_nFilterRadius)) {
			pProjRslt->nFirstId += MinFilter->m_nFilterRadius;
			pProjRslt->nLastId  -= MinFilter->m_nFilterRadius;
			return -1;
		}
		plProjDst[pProjRslt->nFirstId + nFilterRadius] = lMin;
		nLostPosInSlideWindow = pProjRslt->nFirstId;
		nAddedPosInSlideWindow =  pProjRslt->nFirstId + nFilterRadius * 2 + 1;
		for(i=pProjRslt->nFirstId + nFilterRadius + 1; i < (pProjRslt->nLastId - nFilterRadius); i++) {
			if(pProjRslt->plProjVal[nLostPosInSlideWindow] > lMin) {
				if(pProjRslt->plProjVal[nAddedPosInSlideWindow] < lMin) 
					lMin = pProjRslt->plProjVal[nAddedPosInSlideWindow];
			}
			else {
				if(pProjRslt->plProjVal[nAddedPosInSlideWindow] < lMin) 
					lMin = pProjRslt->plProjVal[nAddedPosInSlideWindow];
				else
					MinInSlideWindow(pProjRslt,
									 i,
									 nFilterRadius,
									 lMin);
			}
			plProjDst[i] = lMin;
			nLostPosInSlideWindow++;
			nAddedPosInSlideWindow++;
		}
		pProjRslt->nFirstId += MinFilter->m_nFilterRadius;
		pProjRslt->nLastId  -= MinFilter->m_nFilterRadius;
		lProjMin = plProjDst[pProjRslt->nFirstId];
		lProjMax = plProjDst[pProjRslt->nFirstId];
		for(i=pProjRslt->nFirstId;i<pProjRslt->nLastId;i++) {
			pProjRslt->plProjVal[i] = plProjDst[i];
			if(lProjMin > plProjDst[i])
				lProjMin = plProjDst[i];
			if(lProjMax < plProjDst[i])
				lProjMax = plProjDst[i];
		}
		pProjRslt->lMaxValue = lProjMax;
		pProjRslt->lMinValue = lProjMin;
		pProjRslt->dNEGEdgeShift += MinFilter->m_dNEGEdgeShift;
		pProjRslt->dPOSEdgeShift += MinFilter->m_dPOSEdgeShift;
		delete [] plProjDst;
	}

	return 0;
}

int CEdgeDetect::MinInSlideWindow(PROJ_RSLT* pProjRslt,
								   int nCenterID,
								   int nFilterRadius,
								   long &lMin)
{
	int i;
	int nFirstID;
	int nLastID;
	nFirstID = nCenterID - nFilterRadius;
	if(nFirstID < pProjRslt->nFirstId)
		return -1;
	nLastID = nCenterID + nFilterRadius;
	if(nLastID > pProjRslt->nLastId)
		return -1;
	lMin = pProjRslt->plProjVal[nFirstID];
	for(i = nFirstID + 1; i < nLastID; i++)
	{
		if(lMin > pProjRslt->plProjVal[i])
			lMin = pProjRslt->plProjVal[i];
	}
	return 0;
}

int CEdgeDetect::ApplyMaxFiltering(PROJ_RSLT* pProjRslt,
								   FILTER_1D* MaxFilter) {
	int i;
	long *plProjDst;
	long lMax;
	int nFilterRadius;
	int nLostPosInSlideWindow;
	int nAddedPosInSlideWindow;
	long lProjMin;
	long lProjMax;


	nFilterRadius = MaxFilter->m_nFilterRadius;
	if(nFilterRadius < 0) {
		ASSERT(!"MaxFilter->m_nFilterRadius is less than 0");
		return -1;
	}
	if(nFilterRadius > 0) {
		plProjDst = new long[abs(pProjRslt->nProjSize)];
		MaxInSlideWindow(pProjRslt,
								pProjRslt->nFirstId + nFilterRadius,
								nFilterRadius,
								lMax);
		if((pProjRslt->nFirstId + MaxFilter->m_nFilterRadius) > (pProjRslt->nLastId  - MaxFilter->m_nFilterRadius)) {
			pProjRslt->nFirstId += MaxFilter->m_nFilterRadius;
			pProjRslt->nLastId  -= MaxFilter->m_nFilterRadius;
			return -1;
		}
		plProjDst[pProjRslt->nFirstId+nFilterRadius] = lMax;
		nLostPosInSlideWindow = pProjRslt->nFirstId;
		nAddedPosInSlideWindow =  pProjRslt->nFirstId + nFilterRadius * 2 + 1;
		for(i=pProjRslt->nFirstId + nFilterRadius + 1; i < (pProjRslt->nLastId - nFilterRadius); i++) {
			if(pProjRslt->plProjVal[nLostPosInSlideWindow] < lMax) {
				if(pProjRslt->plProjVal[nAddedPosInSlideWindow] > lMax) 
					lMax = pProjRslt->plProjVal[nAddedPosInSlideWindow];
			}
			else {
				if(pProjRslt->plProjVal[nAddedPosInSlideWindow] > lMax) 
					lMax = pProjRslt->plProjVal[nAddedPosInSlideWindow];
				else
					MaxInSlideWindow(pProjRslt,
									 i,
									 nFilterRadius,
									 lMax);
			}
			plProjDst[i] = lMax;
			nLostPosInSlideWindow++;
			nAddedPosInSlideWindow++;
		}
		pProjRslt->nFirstId += MaxFilter->m_nFilterRadius;
		pProjRslt->nLastId  -= MaxFilter->m_nFilterRadius;
		lProjMin = plProjDst[pProjRslt->nFirstId];
		lProjMax = plProjDst[pProjRslt->nFirstId];
		for(i=pProjRslt->nFirstId;i<pProjRslt->nLastId;i++) {
			pProjRslt->plProjVal[i] = plProjDst[i];
			if(lProjMin > plProjDst[i])
				lProjMin = plProjDst[i];
			if(lProjMax < plProjDst[i])
				lProjMax = plProjDst[i];
		}
		pProjRslt->lMaxValue = lProjMax;
		pProjRslt->lMinValue = lProjMin;
		pProjRslt->dNEGEdgeShift += MaxFilter->m_dNEGEdgeShift;
		pProjRslt->dPOSEdgeShift += MaxFilter->m_dPOSEdgeShift;
		delete [] plProjDst;
	}
	return 0;
}

int CEdgeDetect::MaxInSlideWindow(PROJ_RSLT* pProjRslt,
								   int nCenterID,
								   int nFilterRadius,
								   long &lMax)
{
	int i;
	int nFirstID;
	int nLastID;

	
	nFirstID = nCenterID - nFilterRadius;
	if(nFirstID < pProjRslt->nFirstId)
		return -1;
	nLastID = nCenterID + nFilterRadius;
	if(nLastID > pProjRslt->nLastId)
		return -1;
	lMax = pProjRslt->plProjVal[nFirstID];
	for(i = nFirstID + 1; i < nLastID; i++)
	{
		if(lMax < pProjRslt->plProjVal[i])
			lMax = pProjRslt->plProjVal[i];
	}
	return 0;
}

int CEdgeDetect::GetScanningGradientEdge(PROJ_RSLT* pProjRslt,
										 EDGE_FIND_PARM *pParm)
{
	int i;
	int nFirstIdx,
		nLastIdx,
		nStep,
		nFndGrad,
		nStartPos;
	long m_lGradVal[MAX_PROJ],		// Stores the grad of each projection
		 m_lDoubleGradVal[MAX_PROJ];	// Stores the grad of each Grad Value 

	double dExactPos;
	long lProcContrastPOS, lProcContrastNEG;

	BOOL bEdgePos,
		 bEdgeNeg;

	dAryFndPOSEdge.RemoveAll();
	dAryFndNEGEdge.RemoveAll();

	if (pParm->nOrientation==ROW) 
		nStartPos = pParm->rectROI.left;
	else 
		nStartPos = pParm->rectROI.top;
	if (pParm->nDirection==FORWARD) {
		nFirstIdx = pProjRslt->nFirstId;
		nStep=1;
		nLastIdx = pProjRslt->nLastId;
		if((nFirstIdx+nStep) > (nLastIdx-nStep))
			return -1;
	}
	else {
		nFirstIdx=pProjRslt->nLastId-1;
		nStep=-1;
		nLastIdx=pProjRslt->nFirstId;
		if((nFirstIdx+nStep) < (nLastIdx-nStep))
			return -1;
	}
		
	// Gradient reaches only until 'LastIdx-Step'
	for (i=nFirstIdx; i!=nLastIdx; i+=nStep) 
		m_lGradVal[i]=pProjRslt->plProjVal[i+nStep] - pProjRslt->plProjVal[i];
	// Calculation of double gradient
	for (i=nFirstIdx+nStep; i!=nLastIdx;i+=nStep) 
		m_lDoubleGradVal[i]=m_lGradVal[i]-m_lGradVal[i-nStep];

	lProcContrastPOS = pProjRslt->lMagnify * pParm->nContrast;
	lProcContrastNEG = -lProcContrastPOS;

	//Find edges
	for (i=nFirstIdx+nStep; i!=nLastIdx-nStep; i+=nStep) {
		bEdgePos=FALSE;
		bEdgeNeg=FALSE;
		if ((m_lGradVal[i] > lProcContrastPOS) &&
			(m_lDoubleGradVal[i] >= 0) &&
			(m_lDoubleGradVal[i+nStep] <= 0)) {
			bEdgePos=TRUE;
			nFndGrad=POS;
		}
		if ((m_lGradVal[i] < lProcContrastNEG) &&
			(m_lDoubleGradVal[i] <= 0) &&
			(m_lDoubleGradVal[i+nStep] >= 0)) {
			bEdgeNeg=TRUE;
			nFndGrad=NEG;
		}
		//Save found Edges
		if (bEdgePos && (pParm->nGradient==POS||pParm->nGradient==ANY)) {
			if((fabs((double) m_lDoubleGradVal[i])+fabs((double) m_lDoubleGradVal[i+nStep])) == 0) {
				dExactPos=i+nStep/2.0;
				dAryFndPOSEdge.Add(dExactPos + nStartPos - pProjRslt->dPOSEdgeShift);
			}
			else {
				if((fabs((double) m_lDoubleGradVal[i])+fabs((double) m_lDoubleGradVal[i+nStep])) == 0) {
					dExactPos=i+nStep/2.0;
					dAryFndPOSEdge.Add(dExactPos + nStartPos - pProjRslt->dPOSEdgeShift);
				}
				else {
					dExactPos=i+nStep*fabs((double) m_lDoubleGradVal[i])/(fabs((double) m_lDoubleGradVal[i])+fabs((double) m_lDoubleGradVal[i+nStep]));
					dAryFndPOSEdge.Add(dExactPos + nStartPos - pProjRslt->dPOSEdgeShift);
				}
			}
		}
		if (bEdgeNeg && (pParm->nGradient==NEG||pParm->nGradient==ANY)) {
			if((fabs((double) m_lDoubleGradVal[i])+fabs((double) m_lDoubleGradVal[i+nStep])) == 0) {
				dExactPos=i+nStep/2.0;
				dAryFndNEGEdge.Add(dExactPos + nStartPos - pProjRslt->dNEGEdgeShift);
			}
			else {
				dExactPos=i+nStep*fabs((double) m_lDoubleGradVal[i])/(fabs((double) m_lDoubleGradVal[i])+fabs((double) m_lDoubleGradVal[i+nStep]));
				dAryFndNEGEdge.Add(dExactPos + nStartPos - pProjRslt->dNEGEdgeShift);
			}
		}
	}
	return 0;
}

int CEdgeDetect::GetScanningGravityEdge(PROJ_RSLT* pProjRslt,
										EDGE_FIND_PARM *pParm)
{
	int nFirstIdx,
		nLastIdx,
		nStep,
		i,
		j,
		nStartPos,
		nStart,
		nEnd;
	BOOL bEdgePos,
		 bEdgeNeg,
		 bEdgePresent;
	double dEdge;
	double dX2Before, dX2Current;
	double dX3Before, dX3Current;
	long lThresPOS,
		 lThresNEG;
	double* dXPos;
	double* dYPos;
	double dx,dy,b,k,dWeight,dMoment;
	int nCntPt;
	double dXFirst,
		   dXLast;
	bool bPOSANY;
	bool bNEGANY;
	bool bPOSEdgeFound;
	bool bNEGEdgeFound;
	long lProcContrast;
	long m_lGradVal[MAX_PROJ];		// Stores the grad of each projection

	dAryFndPOSEdge.RemoveAll();
	dAryFndNEGEdge.RemoveAll();
	if (pParm->nOrientation==ROW) 
		nStartPos = pParm->rectROI.left;
	else 
		nStartPos = pParm->rectROI.top;
	if (pParm->nDirection==FORWARD) {
		nFirstIdx = pProjRslt->nFirstId;
		nStep=1;
		nLastIdx = pProjRslt->nLastId - nStep;
		if(nFirstIdx > (nLastIdx - nStep))
			return -1;
	}
	else {
		nFirstIdx=pProjRslt->nLastId-1;
		nStep=-1;
		nLastIdx=pProjRslt->nFirstId + nStep;
		if((nFirstIdx+nStep) < (nLastIdx + nStep))
			return -1;
	}
	// Gradient reaches only until 'LastIdx-Step'
	for (i=nFirstIdx; i!=nLastIdx; i+=nStep) {
		m_lGradVal[i]= (pProjRslt->plProjVal[i+nStep]-
							 pProjRslt->plProjVal[i]);
	}
	// Get Max and Min Gradient Value
	m_lMaxGradVal=m_lMinGradVal=m_lGradVal[nFirstIdx];
	for (i=nFirstIdx+nStep; i!=nLastIdx; i+=nStep)
		if (m_lGradVal[i] < m_lMinGradVal)
			m_lMinGradVal=m_lGradVal[i];
		else if (m_lGradVal[i] > m_lMaxGradVal)
			m_lMaxGradVal=m_lGradVal[i];
	//Get Threshold Level
	lProcContrast = pProjRslt->lMagnify * pParm->nContrast;
	lThresPOS = (pParm->nEdgeLevel * m_lMaxGradVal) / 100;
	if(lThresPOS < lProcContrast)
		lThresPOS = lProcContrast;
	lThresNEG = (pParm->nEdgeLevel * m_lMinGradVal) / 100;
	if(lThresNEG > (-lProcContrast))
		lThresNEG = -lProcContrast;
	nStart=nEnd=-1;
	bEdgePos=bEdgeNeg=bEdgePresent=FALSE;
	bPOSANY = pParm->nGradient==POS||pParm->nGradient==ANY;
	bNEGANY = pParm->nGradient==NEG||pParm->nGradient==ANY;
	bPOSEdgeFound = FALSE;
	bNEGEdgeFound = FALSE;
	dXPos = new double[abs(pProjRslt->nProjSize)];
	dYPos = new double[abs(pProjRslt->nProjSize)];
	for (i=nFirstIdx;i!=nLastIdx;i+=nStep) {
		if(!bEdgePos) {
			if(bPOSANY) {
				if ((m_lGradVal[i] <= lThresPOS) && (m_lGradVal[i+nStep] > lThresPOS)) {
					bEdgePos = TRUE;
					dXFirst = InterpolX(CPoint2D(i, m_lGradVal[i]),
										CPoint2D(i+nStep, m_lGradVal[i+nStep]),
										lThresPOS);
				}
			}
			if(bNEGANY) {
				if ((m_lGradVal[i] >= lThresNEG) && (m_lGradVal[i+nStep] < lThresNEG)) {
					bEdgePos = TRUE;
					dXFirst = InterpolX(CPoint2D(i, m_lGradVal[i]),
										CPoint2D(i+nStep, m_lGradVal[i+nStep]),
										lThresNEG);
				}
			}
		}
		else {
			if(bPOSANY) {
				if ((m_lGradVal[i] >= lThresPOS) && (m_lGradVal[i+nStep] < lThresPOS)) {
					dXLast = InterpolX(CPoint2D(i, m_lGradVal[i]),
										CPoint2D(i+nStep, m_lGradVal[i+nStep]),
										lThresPOS);
					bPOSEdgeFound = TRUE;
				}
			}
			if(bNEGANY) {
				if ((m_lGradVal[i] <= lThresNEG) && (m_lGradVal[i+nStep] > lThresNEG)) {
					dXLast = InterpolX(CPoint2D(i, m_lGradVal[i]),
										CPoint2D(i+nStep, m_lGradVal[i+nStep]),
										lThresNEG);
					bNEGEdgeFound = TRUE;
				}
			}
			if(bPOSEdgeFound||bNEGEdgeFound) {
				dXPos[0] = dXFirst;
				dYPos[0] = 0;
				nCntPt = 1;
				for(j = (int)dXFirst + 1; j <= (int) dXLast; j++) {
					dXPos[nCntPt] = j;
					dYPos[nCntPt] = m_lGradVal[j] - lThresPOS;
					nCntPt++;
				}
				dXPos[nCntPt] = dXLast;
				dYPos[nCntPt] = 0;
				nCntPt++;
				dWeight = dMoment = 0;
				// Compute Moments And Weight
				dX2Before = dXPos[0] * dXPos[0];
				dX3Before = dX2Before * dXPos[0];
				for (j=1;j<nCntPt;j++) {
					dx = dXPos[j] - dXPos[j-1];
					dy = dYPos[j] - dYPos[j-1];
					k = dy/dx;
					b = dYPos[j] - k*dXPos[j];
					dX2Current = dXPos[j] * dXPos[j];
					dX3Current = dX2Current * dXPos[j];
					dWeight = dWeight + k * (dX2Current - dX2Before)/2 + b * (dXPos[j] - dXPos[j-1]);	
					dMoment = dMoment + k * (dX3Current - dX3Before)/3 + b * (dX2Current - dX2Before)/2;
					dX2Before = dX2Current;
					dX3Before = dX3Current;
				}
				if(dWeight == 0) {
					delete [] dXPos;
					delete [] dYPos;
					return -1;
				}
				if (bPOSEdgeFound&&bPOSANY&&dWeight) {
					bPOSEdgeFound = FALSE;
					if(dWeight != 0) {
						dEdge = dMoment/dWeight + 0.5 + nStartPos;
						if (pParm->nDirection==FORWARD)
							dAryFndPOSEdge.Add(dMoment/dWeight + 0.5 + nStartPos);
						else
							dAryFndPOSEdge.Add(dMoment/dWeight - 0.5 + nStartPos);
					}
				}
				if (bNEGEdgeFound&&bNEGANY&&dWeight) {
					bNEGEdgeFound = FALSE;
					if(dWeight != 0) {
						dEdge = dMoment/dWeight + 0.5 + nStartPos;
						if (pParm->nDirection==FORWARD)
							dAryFndNEGEdge.Add(dMoment/dWeight + 0.5 + nStartPos);
						else
							dAryFndNEGEdge.Add(dMoment/dWeight - 0.5 + nStartPos);
					}
				}
				bEdgePos = FALSE;
			}
		}
	}
	delete [] dXPos;
	delete [] dYPos;
	return 0;
}


CGridEdgeDetect::CGridEdgeDetect()
{
}

CGridEdgeDetect::~CGridEdgeDetect()
{
}

int CGridEdgeDetect::GetEdge(IMAGEVIEW_INFO *pView,
							 CImageBuffer *pImg,
							 MULTI_EDGE_SCAN_PARM *pParm,
							 EDGE_FILTER_PARM *pFilterParm,
							 long lProcMode,
							 long lEdgeCheckMode,
							 long lDebugFlag)
{
	int nError,i;
	CRect rect;
	int nSX,nSY;
	CPoint ptEstEdge;
	int nXStart,nXEnd,nYStart,nYEnd;
	CEdgeDetect EdgeDetect;
	CPoint2DArray pt2dAryPOSEdge;
	CPoint2DArray pt2dAryNEGEdge;

	if(m_MultiEdgeScanRslt.pt2dFndPOSEdge)
		delete [] m_MultiEdgeScanRslt.pt2dFndPOSEdge;
	if(m_MultiEdgeScanRslt.pt2dFndNEGEdge)
		delete [] m_MultiEdgeScanRslt.pt2dFndNEGEdge;
	m_MultiEdgeScanRslt.nFndPOSEdge = 0;
	m_MultiEdgeScanRslt.nFndNEGEdge = 0;
	if (pParm->EdgeFindParm.nOrientation == ROW) {
		nXStart = pParm->rcScanROI.left;
		nXEnd = pParm->rcScanROI.right;
		nYStart = pParm->rcScanROI.top;
		nYEnd = pParm->rcScanROI.bottom;
		for (nSY=nYStart; nSY<nYEnd; nSY+=pParm->nSkipFactor) {
			pParm->EdgeFindParm.rectROI = CRect(nXStart,nSY,nXEnd,nSY + pParm->nGroupFactor);
			nError = EdgeDetect.GetEdge(pView,
									   pImg,
									   &pParm->EdgeFindParm,
									   pFilterParm,
									   lProcMode,
									   lEdgeCheckMode,
									   lDebugFlag);
			if(nError)
				goto ErrorAbort;

			if(pParm->EdgeFindParm.nGradient==POS||pParm->EdgeFindParm.nGradient==ANY) {
				for(i = 0; i< EdgeDetect.m_EdgeFindRslt.nFndPOSEdge; i++) {
					pt2dAryPOSEdge.Add(CPoint2D(EdgeDetect.m_EdgeFindRslt.pdFndPOSEdge[i], nSY + (pParm->nSkipFactor - 1) /2.0));
				}
			}
			if(pParm->EdgeFindParm.nGradient==NEG||pParm->EdgeFindParm.nGradient==ANY) {
				for(i = 0; i< EdgeDetect.m_EdgeFindRslt.nFndNEGEdge; i++) {
					pt2dAryNEGEdge.Add(CPoint2D(EdgeDetect.m_EdgeFindRslt.pdFndNEGEdge[i], nSY + (pParm->nSkipFactor - 1) /2.0));
				}
			}

		}
	}
	else {
		nXStart = pParm->rcScanROI.left;
		nXEnd = pParm->rcScanROI.right;
		nYStart = pParm->rcScanROI.top;
		nYEnd = pParm->rcScanROI.bottom;
		for (nSX=nXStart; nSX<nXEnd; nSX+=pParm->nSkipFactor) {
			pParm->EdgeFindParm.rectROI = CRect(nSX,nYStart,nSX+pParm->nGroupFactor,nYEnd);
			nError = EdgeDetect.GetEdge(pView,
									   pImg,
									   &pParm->EdgeFindParm,
									   pFilterParm,
									   lProcMode,
									   lEdgeCheckMode,
									   lDebugFlag);
			if(nError)
				goto ErrorAbort;
			if(pParm->EdgeFindParm.nGradient==POS||pParm->EdgeFindParm.nGradient==ANY) {
				for(i = 0; i< EdgeDetect.m_EdgeFindRslt.nFndPOSEdge; i++) {
					pt2dAryPOSEdge.Add(CPoint2D(nSX + (pParm->nSkipFactor - 1) /2.0,EdgeDetect.m_EdgeFindRslt.pdFndPOSEdge[i]));
				}
			}
			if(pParm->EdgeFindParm.nGradient==NEG||pParm->EdgeFindParm.nGradient==ANY) {
				for(i = 0; i< EdgeDetect.m_EdgeFindRslt.nFndNEGEdge; i++) {
					pt2dAryNEGEdge.Add(CPoint2D(nSX + (pParm->nSkipFactor - 1) /2.0,EdgeDetect.m_EdgeFindRslt.pdFndNEGEdge[i]));
				}
			}
		}
	}
	m_MultiEdgeScanRslt.nFndPOSEdge = (int) pt2dAryPOSEdge.GetSize();
	m_MultiEdgeScanRslt.pt2dFndPOSEdge = new CPoint2D[abs(m_MultiEdgeScanRslt.nFndPOSEdge)];
	m_MultiEdgeScanRslt.nFndNEGEdge = (int) pt2dAryNEGEdge.GetSize();
	m_MultiEdgeScanRslt.pt2dFndNEGEdge = new CPoint2D[abs(m_MultiEdgeScanRslt.nFndNEGEdge)];
	for(i = 0; i < m_MultiEdgeScanRslt.nFndPOSEdge; i++) 
		m_MultiEdgeScanRslt.pt2dFndPOSEdge[i] = pt2dAryPOSEdge[i];
	for(i = 0; i < m_MultiEdgeScanRslt.nFndNEGEdge; i++) 
		m_MultiEdgeScanRslt.pt2dFndNEGEdge[i] = pt2dAryNEGEdge[i];
	return 0;
ErrorAbort:
	return -1;
}

int CGridEdgeDetect::GetSlantScanEdge(IMAGEVIEW_INFO *pView,
									  CImageBuffer *pImg,
									  MULTI_EDGE_SLANT_SCAN_PARM *pParm,
									  EDGE_FILTER_PARM *pFilterParm,
									  long lProcMode,
									  long lEdgeCheckMode,
									  long lDebugFlag,
									  CString* pstrDebug)
{
	int nError;
	int i, j;
	CRect rect;
	CPoint ptEstEdge;
	CEdgeDetect EdgeDetect;
	CPoint2DArray pt2dAryPOSEdge;
	CPoint2DArray pt2dAryNEGEdge;
	CLine2D ln2DPerpendicular;
	CLine2D ln2DScanBase;
	double dScanHeight;
	CPoint2D pt2DScanStep;
	int nScanLine;

	if(m_MultiEdgeScanRslt.pt2dFndPOSEdge)
		delete [] m_MultiEdgeScanRslt.pt2dFndPOSEdge;
	m_MultiEdgeScanRslt.pt2dFndPOSEdge = NULL;
	if(m_MultiEdgeScanRslt.pt2dFndNEGEdge)
		delete [] m_MultiEdgeScanRslt.pt2dFndNEGEdge;
	m_MultiEdgeScanRslt.nFndPOSEdge = 0;
	m_MultiEdgeScanRslt.nFndNEGEdge = 0;
	m_MultiEdgeScanRslt.pt2dFndNEGEdge = NULL;

	if(pParm->nSkipFactor <= 0) {
		ASSERT(!"pParm->nSkipFactor is invalid");
		return -1;
	}

	ln2DScanBase = pParm->EdgeSlantScanParm.ln2DScan;
	ln2DScanBase.GetPerpendicularLine(&pParm->EdgeSlantScanParm.pt2DScanStart,&ln2DPerpendicular);
	ln2DScanBase.GetDistance(&pParm->pt2DLastScanLineStart,&dScanHeight);
	nScanLine = (int)((dScanHeight + 0.5) / pParm->nSkipFactor);
	if(nScanLine == 0)
		nScanLine = 1;
	pt2DScanStep = CPoint2D((pParm->pt2DLastScanLineStart.x - pParm->EdgeSlantScanParm.pt2DScanStart.x) / nScanLine,
		(pParm->pt2DLastScanLineStart.y - pParm->EdgeSlantScanParm.pt2DScanStart.y) / nScanLine);
	for (j=0; j< nScanLine; j++) {
		if(j!=0) {
			pParm->EdgeSlantScanParm.pt2DScanStart += pt2DScanStep;
			pParm->EdgeSlantScanParm.pt2DScanEnd += pt2DScanStep;
			pParm->EdgeSlantScanParm.pt2DProj += pt2DScanStep;
			ln2DScanBase.GetParallelLine(&pParm->EdgeSlantScanParm.pt2DScanStart,
				&pParm->EdgeSlantScanParm.ln2DScan);
		}
		nError = EdgeDetect.GetSlantScanEdge(pView,
								   pImg,
								   &pParm->EdgeSlantScanParm,
								   pFilterParm,
								   lProcMode,
								   lEdgeCheckMode,
								   lDebugFlag,
								   pstrDebug);
		if(nError)
			goto ErrorAbort;
		if(pParm->EdgeSlantScanParm.nGradient==POS||pParm->EdgeSlantScanParm.nGradient==ANY) {
			for(i = 0; i< EdgeDetect.m_EdgeSlantScanRslt.nFndPOSEdge; i++) {
				pt2dAryPOSEdge.Add(EdgeDetect.m_EdgeSlantScanRslt.ppt2DFndPOSEdge[i]);
			}
		}
		if(pParm->EdgeSlantScanParm.nGradient==NEG||pParm->EdgeSlantScanParm.nGradient==ANY) {
			for(i = 0; i< EdgeDetect.m_EdgeSlantScanRslt.nFndNEGEdge; i++) {
				pt2dAryNEGEdge.Add(EdgeDetect.m_EdgeSlantScanRslt.ppt2DFndNEGEdge[i]);
			}
		}
	}
	m_MultiEdgeScanRslt.nFndPOSEdge = (int) pt2dAryPOSEdge.GetSize();
	m_MultiEdgeScanRslt.pt2dFndPOSEdge = new CPoint2D[abs(m_MultiEdgeScanRslt.nFndPOSEdge)];
	m_MultiEdgeScanRslt.nFndNEGEdge = (int) pt2dAryNEGEdge.GetSize();
	m_MultiEdgeScanRslt.pt2dFndNEGEdge = new CPoint2D[abs(m_MultiEdgeScanRslt.nFndNEGEdge)];
	for(i = 0; i < m_MultiEdgeScanRslt.nFndPOSEdge; i++) 
		m_MultiEdgeScanRslt.pt2dFndPOSEdge[i] = pt2dAryPOSEdge[i];
	for(i = 0; i < m_MultiEdgeScanRslt.nFndNEGEdge; i++) 
		m_MultiEdgeScanRslt.pt2dFndNEGEdge[i] = pt2dAryNEGEdge[i];

	return 0;
ErrorAbort:
	return -1;
}

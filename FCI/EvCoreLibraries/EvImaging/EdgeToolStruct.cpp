#include "stdAfx.h"
#include "EdgeToolStruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

FILTER_1D::FILTER_1D()
{
	m_pnFilter = 0;
	m_nFilterSize = 0;
	m_nFilterRadius = 0;
	m_nFilterSum = 0;
	m_dPOSEdgeShift = 0;
	m_dNEGEdgeShift = 0;
}

FILTER_1D::~FILTER_1D()
{
	Clean();
}

void FILTER_1D::Clean()
{
	m_nFilterSize = 0;
	m_nFilterRadius = 0;
	m_nFilterSum = 0;
	m_dPOSEdgeShift = 0;
	m_dNEGEdgeShift = 0;
	if(m_pnFilter) {
		delete [] m_pnFilter;
		m_pnFilter = 0;
	}
}


FILTER_1D & FILTER_1D::operator = (const FILTER_1D &Filter1D) 
{
	int i;
	Clean();
	m_nFilterRadius = Filter1D.m_nFilterRadius;
	m_nFilterSize = Filter1D.m_nFilterSize;
	m_nFilterSum = Filter1D.m_nFilterSum;
	m_dPOSEdgeShift = Filter1D.m_dPOSEdgeShift;
	m_dNEGEdgeShift = Filter1D.m_dNEGEdgeShift;
	m_nFilterMode =  Filter1D.m_nFilterMode;
	m_pnFilter = new int[abs(m_nFilterSize)];
	for(i = 0; i < m_nFilterSize; i++) {
		m_pnFilter[i] = Filter1D.m_pnFilter[i];
	}
	return *this;
}

void FILTER_1D::FindFilterSum()
{
	int i;
	m_nFilterSum = 0;
	for(i = 0; i < m_nFilterSize; i++) {
		m_nFilterSum += m_pnFilter[i];
	}
}

int FILTER_1D::CreateBinomialFilter(int nFilterRadius)
{
	int i;
	int k1, k2;
	double dSum;
	double dMin;
	double *dFilter;
	int nStartIndex, nEndIndex;
	double dFactor;

	Clean();
	if(nFilterRadius < 0) {
		ASSERT(!"nFilterRadius is Negative");
		return -1;
	}
	m_nFilterMode = BINOMIAL_FILTER;
	m_dPOSEdgeShift = 0;
	m_dNEGEdgeShift = 0;
	m_nFilterRadius = nFilterRadius;
	m_nFilterSize = 2 * nFilterRadius + 1;
	dFilter = new double[abs(m_nFilterSize)];
	m_pnFilter = new int[abs(m_nFilterSize)];

	dFilter[0] = 1;
	dMin = dFilter[0];
	dSum = dFilter[0];
	for (i=1, k1=(int)(2*nFilterRadius), k2=1; i<(int)(2*nFilterRadius+1); i++,k1--,k2++) {
		dFilter[i] = (dFilter[i-1] * k1) / k2;
		if(dMin > dFilter[i])
			dMin = dFilter[i];
		dSum += dFilter[i];
	}
	if((dSum > Filter_Sum_Max) || (dMin < 1))
		dFactor = Filter_Sum_Max / dSum;
	else
		dFactor = 1;

	for (i=0; i < m_nFilterSize; i++) {
		m_pnFilter[i] = (int)(dFilter[i] * dFactor);
	}
	nStartIndex = 0;
	for (i=0; i < m_nFilterSize; i++) {
		if(m_pnFilter[i] > 0) {
			nStartIndex = i;
			break;
		}
	}
	nEndIndex = m_nFilterSize;
	for (i=m_nFilterSize - 1; i >= 0; i--) {
		if(m_pnFilter[i] > 0) {
			nEndIndex = i + 1;
			break;
		}
	}
	m_nFilterRadius = nFilterRadius - nStartIndex;
	m_nFilterSize = 2 * nFilterRadius + 1;
	for (i=nStartIndex; i < nEndIndex; i++) {
		m_pnFilter[i-nStartIndex] = m_pnFilter[i];
	}
	FindFilterSum();
	delete [] dFilter;
	return 0;
}

int FILTER_1D::CreateGaussianFilter(int nFilterRadius, 
								  double dSigma) 
{ 

	int i,j;
	double dSum;
	double dMin;
	double *dFilter;
	double dFactor;
	int nStartIndex, nEndIndex;

	Clean();
	if(nFilterRadius < 0) {
		ASSERT(!"nFilterRadius is Negative");
		return -1;
	}
	if(dSigma <= 0) {
		ASSERT(!"dSigma is Invalid");
		return -1;
	}
	m_nFilterMode = GAUSSIAN_FILTER;
	m_dPOSEdgeShift = 0;
	m_dNEGEdgeShift = 0;
	m_nFilterRadius = nFilterRadius;
	m_nFilterSize = 2 * nFilterRadius + 1;
	dFilter = new double[abs(m_nFilterSize)];
	m_pnFilter = new int[abs(m_nFilterSize)];

	dMin = Filter_Sum_Max;
	dSum = 0;
	for (i=-nFilterRadius,j=0;i<=nFilterRadius;j++,i++) {
		dFilter[j] = (1.0/(sqrt(2*PI)*dSigma)*exp(-((i*i)/(2*dSigma*dSigma))));
		if(dMin > dFilter[j])
			dMin = dFilter[j];
		dSum += dFilter[j];
	}
	if(dSum > 0) {
		if((dSum > Filter_Sum_Max) || (dMin < 1))
			dFactor = Filter_Sum_Max / dSum;
		else
			dFactor = 1;
		for (i=0; i < m_nFilterSize; i++) {
			m_pnFilter[i] = (int)(dFilter[i] * dFactor);
		}
		nStartIndex = 0;
		for (i=0; i < m_nFilterSize; i++) {
			if(m_pnFilter[i] > 0) {
				nStartIndex = i;
				break;
			}
		}
		nEndIndex = m_nFilterSize;
		for (i=m_nFilterSize - 1; i >= 0; i--) {
			if(m_pnFilter[i] > 0) {
				nEndIndex = i + 1;
				break;
			}
		}
		m_nFilterRadius = nFilterRadius - nStartIndex;
		m_nFilterSize = 2 * nFilterRadius + 1;
		for (i=nStartIndex; i < nEndIndex; i++) {
			m_pnFilter[i-nStartIndex] = m_pnFilter[i];
		}
	}
	else
		m_nFilterSize = 0;
	FindFilterSum();
	delete [] dFilter;
	return 0;
}


int FILTER_1D::CreateMaxFilter(int nFilterRadius)
{ 
	Clean();
	if(nFilterRadius < 0) {
		ASSERT(!"nFilterRadius is Negative");
		return -1;
	}
	m_nFilterMode = MAX_FILTER;
	m_dPOSEdgeShift = -nFilterRadius;
	m_dNEGEdgeShift = nFilterRadius;
	m_nFilterRadius = nFilterRadius;
	m_nFilterSize = 2 * nFilterRadius + 1;
	m_pnFilter = new int[abs(m_nFilterSize)];
	memset(m_pnFilter, 0, sizeof(int) * m_nFilterSize);
	if(m_nFilterSize > 0)
		m_pnFilter[0] = 1;
	m_nFilterSum = 1;
	return 0;
}

int FILTER_1D::CreateMinFilter(int nFilterRadius)
{ 
	Clean();
	if(nFilterRadius < 0) {
		ASSERT(!"nFilterRadius is Negative");
		return -1;
	}
	m_nFilterMode = MIN_FILTER;
	m_dPOSEdgeShift = nFilterRadius;
	m_dNEGEdgeShift = -nFilterRadius;
	m_nFilterRadius = nFilterRadius;
	m_nFilterSize = 2 * nFilterRadius + 1;
	m_pnFilter = new int[abs(m_nFilterSize)];
	memset(m_pnFilter, 0, sizeof(int) * m_nFilterSize);
	if(m_nFilterSize > 0)
		m_pnFilter[0] = 1;
	m_nFilterSum = 1;
	return 0;
}

int FILTER_1D::CreateMeanFilter(int nFilterRadius)
{ 
	Clean();
	if(nFilterRadius < 0) {
		ASSERT(!"nFilterRadius is Negative");
		return -1;
	}
	m_nFilterMode = MEAN_FILTER;
	m_dPOSEdgeShift = 0;
	m_dNEGEdgeShift = 0;
	m_nFilterRadius = nFilterRadius;
	m_nFilterSize = 2 * nFilterRadius + 1;
	m_pnFilter = new int[abs(m_nFilterSize)];
	memset(m_pnFilter, 1, sizeof(int) * m_nFilterSize);
	FindFilterSum();
	return 0;
}

int FILTER_1D::CreateMedianFilter(int nFilterRadius)
{ 
	Clean();
	if(nFilterRadius < 0) {
		ASSERT(!"nFilterRadius is Negative");
		return -1;
	}
	m_nFilterMode = MEDIAN_FILTER;
	m_dPOSEdgeShift = 0;
	m_dNEGEdgeShift = 0;
	m_nFilterRadius = nFilterRadius;
	m_nFilterSize = 2 * nFilterRadius + 1;
	m_pnFilter = new int[abs(m_nFilterSize)];
	memset(m_pnFilter, 0, sizeof(int) * m_nFilterSize);
	if(m_nFilterSize > 0)
		m_pnFilter[0] = 1;
	m_nFilterSum = 1;
	return 0;
}


EDGE_FILTER_PARM::EDGE_FILTER_PARM()
{
	m_nTotalApplyFilter = 0;
}

void EDGE_FILTER_PARM::InitUserDefFilterPam()
{
	Filter[0] = Filter_0;
	Filter[1] = Filter_1;
	Filter[2] = Filter_2;
	Filter[3] = Filter_3;
	Filter[4] = Filter_4;
	Filter[5] = Filter_5;
}


EDGE_FILTER_PARM::~EDGE_FILTER_PARM()
{
}

EDGE_FIND_PARM::EDGE_FIND_PARM()
{
	rectROI = CRect(0,0,0,0);	
	nOrientation = ROW;
	nGradient = POS;	
	nDirection = FORWARD;	
	nWhiteCount = 3;
	nBlackCount = 3;
	nEdgeLevel = 50;	
	nContrast = 20;	
	nTerminationContrast = 255;
}

EDGE_FIND_PARM::~EDGE_FIND_PARM()
{
}



EDGE_SLANT_SCAN_PARM::EDGE_SLANT_SCAN_PARM()
{
	pt2DScanStart = CPoint2D(0,0);
	pt2DScanEnd = CPoint2D(0,0);
	pt2DProj = CPoint2D(0,0);
	bAccurate = 0; 
	nGradient = POS;	
	nWhiteCount = 3;
	nBlackCount = 3;
	nEdgeLevel = 50;	
	nContrast = 20;	
	nTerminationContrast = 255;
}

EDGE_SLANT_SCAN_PARM::~EDGE_SLANT_SCAN_PARM()
{
}

EDGE_FIND_RSLT::EDGE_FIND_RSLT()
{
	nFndEdges = 0;	
}

EDGE_FIND_RSLT::~EDGE_FIND_RSLT()
{
}

EDGE_FIND_RESULT::EDGE_FIND_RESULT()
{
	pdFndPOSEdge = NULL;
	nFndPOSEdge = 0;
	pdFndNEGEdge = NULL;
	nFndNEGEdge = 0;
}

EDGE_FIND_RESULT::~EDGE_FIND_RESULT()
{
	if(pdFndPOSEdge)
		delete [] pdFndPOSEdge;
	if(pdFndNEGEdge)
		delete [] pdFndNEGEdge;
	nFndPOSEdge = 0;
	nFndNEGEdge = 0;
}



EDGE_SLANT_SCAN_RESULT::EDGE_SLANT_SCAN_RESULT()
{
	ppt2DFndPOSEdge = NULL;
	nFndPOSEdge = 0;
	ppt2DFndNEGEdge = NULL;
	nFndNEGEdge = 0;
}

EDGE_SLANT_SCAN_RESULT::~EDGE_SLANT_SCAN_RESULT()
{
	if(ppt2DFndPOSEdge)
		delete [] ppt2DFndPOSEdge;
	if(ppt2DFndNEGEdge)
		delete [] ppt2DFndNEGEdge;
	nFndPOSEdge = 0;
	nFndNEGEdge = 0;
}

MULTI_EDGE_SCAN_PARM::MULTI_EDGE_SCAN_PARM()
{
	rcScanROI = CRect(0,0,0,0);
	nSkipFactor = 2;
	nGroupFactor = 1;
}

MULTI_EDGE_SCAN_PARM::~MULTI_EDGE_SCAN_PARM()
{
}

MULTI_EDGE_SLANT_SCAN_PARM::MULTI_EDGE_SLANT_SCAN_PARM()
{
	pt2DLastScanLineStart = CPoint2D(0,0);
	nSkipFactor = 1;
}
MULTI_EDGE_SLANT_SCAN_PARM::~MULTI_EDGE_SLANT_SCAN_PARM()
{
}

MULTI_EDGE_SCAN_RSLT::MULTI_EDGE_SCAN_RSLT()
{
	nFndPOSEdge = 0;
	nFndNEGEdge = 0;
	pt2dFndPOSEdge = NULL;
	pt2dFndNEGEdge = NULL;
}

MULTI_EDGE_SCAN_RSLT::~MULTI_EDGE_SCAN_RSLT()
{
	if(pt2dFndPOSEdge)
		delete [] pt2dFndPOSEdge;
	if(pt2dFndNEGEdge)
		delete [] pt2dFndNEGEdge;
	nFndPOSEdge = 0;
	nFndNEGEdge = 0;
}


EDGE_SHAPE_INFO::EDGE_SHAPE_INFO()
{
	bFndLftPos = false;	
	bFndRhtPos = false;	
}

EDGE_SHAPE_INFO::~EDGE_SHAPE_INFO()
{
}


EDGE_CONTRAST_INFO::EDGE_CONTRAST_INFO()
{
	bHalfGradEdgeShape = false;	
	bBaseGradEdgeShape = false;	
}

EDGE_CONTRAST_INFO::~EDGE_CONTRAST_INFO()
{
}

EDGE_CONTRAST_INFO & EDGE_CONTRAST_INFO::operator = (const EDGE_CONTRAST_INFO &EdgeContrastInfo) 
{
	HalfGradEdgeShapeInfo.bFndLftPos = EdgeContrastInfo.HalfGradEdgeShapeInfo.bFndLftPos;
	HalfGradEdgeShapeInfo.bFndRhtPos = EdgeContrastInfo.HalfGradEdgeShapeInfo.bFndRhtPos;
	HalfGradEdgeShapeInfo.nFndLftPos = EdgeContrastInfo.HalfGradEdgeShapeInfo.nFndLftPos;
	HalfGradEdgeShapeInfo.nFndRhtPos = EdgeContrastInfo.HalfGradEdgeShapeInfo.nFndRhtPos;
	HalfGradEdgeShapeInfo.lGrad = EdgeContrastInfo.HalfGradEdgeShapeInfo.lGrad;
	BaseGradEdgeShapeInfo.bFndLftPos = EdgeContrastInfo.BaseGradEdgeShapeInfo.bFndLftPos;
	BaseGradEdgeShapeInfo.bFndRhtPos = EdgeContrastInfo.BaseGradEdgeShapeInfo.bFndRhtPos;
	BaseGradEdgeShapeInfo.nFndLftPos = EdgeContrastInfo.BaseGradEdgeShapeInfo.nFndLftPos;
	BaseGradEdgeShapeInfo.nFndRhtPos = EdgeContrastInfo.BaseGradEdgeShapeInfo.nFndRhtPos;
	BaseGradEdgeShapeInfo.lGrad = EdgeContrastInfo.BaseGradEdgeShapeInfo.lGrad;
	bHalfGradEdgeShape = EdgeContrastInfo.bHalfGradEdgeShape;
	bBaseGradEdgeShape = EdgeContrastInfo.bBaseGradEdgeShape;
	lMaxContrast = EdgeContrastInfo.lMaxContrast;
	lMinContrast = EdgeContrastInfo.lMinContrast;
	lPOSContrast = EdgeContrastInfo.lPOSContrast;
	lNEGContrast = EdgeContrastInfo.lNEGContrast;
	bPOSEdge = EdgeContrastInfo.bPOSEdge; 
	nZeroCrossingLftPos = EdgeContrastInfo.nZeroCrossingLftPos;
	nZeroCrossingRhtPos = EdgeContrastInfo.nZeroCrossingRhtPos;
	return *this;
}

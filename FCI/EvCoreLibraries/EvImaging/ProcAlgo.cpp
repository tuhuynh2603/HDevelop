#include "stdafx.h"
#include "ProcAlgo.h"
#include "ImageFunction.h"
#include "TrigTable.h"
#include "BitmapFunctions.h"
#include "Utilities.h"
#include "ExternalFunctions.h"
#include <math.h>

#include "MathDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int Create1DBinomialMask(int nMaskSize, double *ptrMask, BOOL bNormalize)
{
	int i;
	double dSum;
	double dK1,dK2;

	//////////////////////////////////////
	BOOL bDebugThis;
	bDebugThis=FALSE;
	//////////////////////////////////////

	ptrMask[0] = 1.0;
	dSum = 1.0;
	for (i=1, dK1=(double)(2*nMaskSize), dK2=1.0; i<2*nMaskSize+1; i++,dK1-=1.0,dK2+=1.0) {
		ptrMask[i] = ptrMask[i-1]*dK1/dK2;
		dSum += ptrMask[i];
	}
	if(dSum == 0)
		return -1;
	if (bNormalize) {
		for (i=0;i<2*nMaskSize+1;i++) {
			ptrMask[i] /= dSum;
		}
	}
	if (bDebugThis) {
		for (i=0;i<2*nMaskSize+1;i++) {
			OutputDebugLog("Binomial ptrMask[%d]=%3.4f",i,ptrMask[i]);
		}
	}

	return 0;
}

int ApplyMask(long *plProjVal, int nProjSize, double *ptrMask,int nMaskSize) {


    long *plProjDst;
	int i,j,k;
	double dSum;

	int nFilterRadius;

	plProjDst = new long[abs(nProjSize)];

	nFilterRadius = nMaskSize;

	for(i=nFilterRadius;i<nProjSize-nFilterRadius;i++) {
		dSum = 0.0;
		for (j=-nFilterRadius,k=0;j<=nFilterRadius;j++,k++) {
			dSum += plProjVal[i+j] * ptrMask[k]; 
		}
		plProjDst[i] = (long) (dSum);
	}
	for(i=nFilterRadius;i<nProjSize-nFilterRadius;i++) 
		plProjVal[i] = plProjDst[i];

	delete [] plProjDst;
	return 0;

/*
	int i,j,k;
	double dSum;
	long lOldVal;
	long lTempBuf[21];		// Area to Store Computed ProjVal. Will use val inside here to 
							// update plProjVal after finish using plProjVal for Gaussian computation

	int nWriteIdx,nReadIdx; // Indexer for Mask
	int nWriteProjIdx;		// Indexer for Writing to ProjVal

	//////////////////////////////////////
	// Debugging Use
	CString strTmp,strTmp1;
	int nTmp;
	BOOL bDebugThis = FALSE;
	//////////////////////////////////////

	if (!ptrMask)
		return -1;
	if (nMaskSize>10) {
		AfxMessageBox("MaskSize Limit is 10");
		return -1;
	}

	nWriteIdx = 0;
	nReadIdx = 0;
	nWriteProjIdx = 0+nMaskSize;
	for(i=0+nMaskSize;i<nProjSize-nMaskSize;i++) {
		if (bDebugThis) {
			OutputDebugLog("");
			strTmp.Empty();
			strTmp1.Empty();
			for (nTmp=0;nTmp<nMaskSize*2+1;nTmp++) {
				strTmp.Format("[%d]:%ld",i-nMaskSize+nTmp,plProjVal[i-nMaskSize+nTmp]);
				strTmp1 = strTmp1 + "," + strTmp;
			}
			OutputDebugLog("Mask List=%s",strTmp1);
			OutputDebugLog("Working on ProjVal[%d], Orig Val=%ld",i,plProjVal[i]);
		}

		dSum = 0.0;
		for (j=0-nMaskSize,k=0;j<=nMaskSize;j++,k++) {
			if (bDebugThis) {
				OutputDebugLog("Compute Mask[%d]=%3.4f with ProjVal[%d]=%ld  Result=%ld",
								k,
								ptrMask[k],
								i+j,
								plProjVal[i+j],
								(long)(plProjVal[i+j]*ptrMask[k]));
			}
			dSum += (double)(plProjVal[i+j])*ptrMask[k]; 
		}

		lTempBuf[nWriteIdx] = (long)dSum;
		if (bDebugThis) {
			OutputDebugLog("Computed New ProjVal[%d] inside lTempBuf[%d] is Val=%ld",
							i,
							nWriteIdx,
							(long)dSum);
		}
		if (i-nMaskSize>=nMaskSize) {
			lOldVal = plProjVal[nWriteProjIdx];
			plProjVal[nWriteProjIdx] = lTempBuf[nReadIdx];
			if (bDebugThis) {
				OutputDebugLog("Committing lTempBuf[%d]=%ld into ProjVal[%d]:OldVal=%ld",
								nReadIdx,
								lTempBuf[nReadIdx],
								nWriteProjIdx,
								lOldVal);
			}
			nWriteProjIdx++;
			nReadIdx++;
			if (nReadIdx>nMaskSize)
				nReadIdx=0;
		}
		nWriteIdx++;
		if (nWriteIdx>nMaskSize)
			nWriteIdx=0;
	}
	return 0;
*/
}

int Create1DGaussianMask(int nMaskSize, double dSigma, double *ptrMask) {

	int i,j;
	double dDat;
	
	//////////////////////////////////////
	// Debugging Use
	BOOL bDebugThis = FALSE;
	//////////////////////////////////////

	for (i=-nMaskSize,j=0;i<=nMaskSize;j++,i++) {
		dDat = (sqrt(2*PI)*dSigma)*exp(-((i*i)/(2*dSigma*dSigma)));
		if(dDat == 0)
			return -1;
		ptrMask[j] = (1.0/dDat);
		if (bDebugThis)
			OutputDebugLog("ptrMask[%d]=%3.4f",i,ptrMask[j]);
	}
	return 0;
}

void binom(int n, double dValues[], BOOL bNormalize=TRUE)
{
	int nCount, i;
	double k1, k2, dn, dSum;

	nCount = n+1;
	dValues[0] = 1.0;
	dSum = 1.0;
	dn = (double) n; 
	for (i=1, k1=dn, k2=1.0; i<nCount; i++,k1-=1.0,k2+=1.0) {
		dValues[i] = dValues[i-1]*k1/k2;
		dSum += dValues[i];
	}
	if (bNormalize) {
		for (i=0; i<nCount; i++)
			dValues[i] /= dSum;
	}
}

void conv(double* pdVec1, double* pdVec2, int n1, int n2, double* pdResult)
{
	int nn, nOffset, i, j, nb, ns, nst,jj;
	double* pd1;
	double* pd2;

	if (n1 > n2) {
		nb=n1;
		ns=n2;
		pd1=pdVec1;
		pd2=pdVec2;
	}
	else {
		nb = n2;
		ns = n1;
		pd1 = pdVec2;
		pd2 = pdVec1;
	}

	nn=n1+n2-1;
	nOffset = n2-1;
	for (i=0; i<nn; i++) {
		pdResult[i]=0.0;
		nst = max(0, i-ns+1);
		for (j=nst; j<nb; j++) {
			jj = i-j;
			if ((jj >= 0) && (jj < n2)) {
				pdResult[i] += pd1[j]*pd2[jj];
			}
			else
				break;
		}
	}
	return;
}

int	GetEdge1D(long *plProjVal,
			  int nWidth,		// Proj Size
			  int nSize,		// Group Factor
			  int nRef,			// Ref Value
			  int nThreshold,
			  int nGradient,
			  int nMethod,
			  int nDirection,
			  double *pdFndPos,
			  int *pnFndGrad,
			  long lDebugFlag)
{

	int i,j;
	double dGradWeight[MAX_PROJ];
	double dXPos[100],dYPos[100];
	double dMaxGrad,dMinGrad;
	double dXFirst,
		   dXLast;
	int nCntPt;

	BOOL bEdgePos,bEdgeNeg,bEdgePresent;
	double dThresPOS,dThresNEG;
	int nStart,nEnd;
	int nFirstIdx,
		nLastIdx,
		nStep;
	int nFndEdges;
	CString strMsg;

	double dx,dy,b,k,dWeight,dMoment;
	dMaxGrad = 0;
	dMinGrad = 0;
	dThresPOS = 0;
	dThresNEG = 0;


	if (nDirection==FORWARD) {
		nFirstIdx=0;
		nStep=1;
		nLastIdx=nWidth-1;
	}
	else {
		nFirstIdx=nWidth-1;
		nStep=-1;
		nLastIdx=0;
	}

	pdFndPos[0]=nFndEdges=0;
	//Get 1st Derivative and Cal Max&Min Gradient
	for (i=nFirstIdx;i!=nLastIdx;i+=nStep) {
		dGradWeight[i] = (double)(plProjVal[i+nStep] - plProjVal[i])/(double)nSize;
		if (i==nFirstIdx) {
			dMaxGrad = dGradWeight[i];
			dMinGrad = dGradWeight[i];
		}
		else {
			if (dGradWeight[i]>dMaxGrad)
				dMaxGrad = dGradWeight[i];
			else if (dGradWeight[i]<dMinGrad)
				dMinGrad = dGradWeight[i];
		}
	}

	nStart=nEnd=-1;
	bEdgePos=bEdgeNeg=bEdgePresent=FALSE;
	dWeight = dMoment = 0;
	dXFirst=-1.0,dXLast=-1.0;

	if (nMethod==LEVEL) {
		//Get Threshold Level
		dThresPOS = ((double)nThreshold/100.0)*dMaxGrad;
		dThresNEG = ((double)nThreshold/100.0)*dMinGrad;
	}
	else if (nMethod==THRESHOLD) {
		//Get Threshold Level
		dThresPOS = (double)nThreshold;
		dThresNEG = -(double)nThreshold;
	}

	if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
		OutputDebugLog("+Thres=%3.2f, -Thres=%3.2f",dThresPOS,dThresNEG);
	//	AfxMessageBox("Check GradWeight & Thres inside GetEdge1D()");
	}

	for (i=nFirstIdx;i!=nLastIdx;i+=nStep) {
		//Set Marker for start of Positive Edge
		if (dGradWeight[i]>dThresPOS && !bEdgePos && !bEdgeNeg && !bEdgePresent && (nGradient==POS||nGradient==ANY)) {
			nStart = i;
			bEdgePos = TRUE;

			if (i!=nFirstIdx) {
				dXFirst = InterpolX(CPoint2D((double)(i-nStep),dGradWeight[i-nStep]),
									CPoint2D((double)(i),dGradWeight[i]),
									dThresPOS);

				// Skip point that fall on Threshold line
				if (dXFirst==(double)(i-nStep))
					dXFirst=-1.0;
			}
		}
		//Set Marker for end of Positive Edge
		else if (dGradWeight[i]<dThresPOS && bEdgePos && !bEdgeNeg && !bEdgePresent && (nGradient==POS||nGradient==ANY)) {
			nEnd = i;
			bEdgePresent = TRUE;

			if (i!=nLastIdx) {
				dXLast = InterpolX(CPoint2D((double)(i),dGradWeight[i]),
									CPoint2D((double)(i-nStep),dGradWeight[i-nStep]),
									dThresPOS);

				// Skip point that fall on Threshold line
				if (dXLast==(double)(i-nStep))
					dXLast=-1.0;
			}
		}

		//Set Marker for start of Negative Edge
		else if (dGradWeight[i]<dThresNEG && !bEdgePos && !bEdgeNeg && !bEdgePresent && (nGradient==NEG||nGradient==ANY)) {
			nStart = i;
			bEdgeNeg = TRUE;

			if (i!=nFirstIdx) {
				dXFirst = InterpolX(CPoint2D((double)(i-nStep),dGradWeight[i-nStep]),
									CPoint2D((double)(i),dGradWeight[i]),
									dThresNEG);
				// Skip point that fall on Threshold line
				if (dXFirst==(double)(i-nStep))
					dXFirst=-1.0;
			}
		}
		//Set Marker for end of Negative Edge
		else if (dGradWeight[i]>dThresNEG && bEdgeNeg && !bEdgePos && !bEdgePresent && (nGradient==NEG||nGradient==ANY)) {
			nEnd = i;
			bEdgePresent = TRUE;
			if (i!=nLastIdx) {
				dXLast = InterpolX(CPoint2D((double)(i),dGradWeight[i]),
									CPoint2D((double)(i-nStep),dGradWeight[i-nStep]),
									dThresNEG);
				// Skip point that fall on Threshold line
				if (dXLast==(double)(i-nStep))
					dXLast=-1.0;
			}
		}

		// Find Edge
		if (bEdgePresent) {
			if (fabs((double) (nEnd-nStart))>=98) {
				AfxMessageBox("Array Size Exceeded in GetEdge1D()");
				return -9;
			}
			if (dXFirst!= -1.0) {
				dXPos[0] = dXFirst;
				if (bEdgePos)
					dYPos[0] = dThresPOS;
				else
					dYPos[0] = dThresNEG;
				nCntPt = 1;
			}
			else
				nCntPt = 0;

			for(j=nStart;j!=nEnd;j+=nStep) {
				dXPos[nCntPt] = j;
				dYPos[nCntPt] = dGradWeight[j];
				nCntPt++;
			}

			if (dXLast!= -1.0) {
				dXPos[nCntPt] = dXLast;
				if (bEdgePos)
					dYPos[nCntPt] = dThresPOS;
				else
					dYPos[nCntPt] = dThresNEG;
				nCntPt++;
			}

			dWeight = dMoment = 0;
			// Compute Moments And Weight
			for (j=1;j<nCntPt;j++) {
				dx = (double)dXPos[j] - dXPos[j-1];
				dy = dYPos[j] - dYPos[j-1];
				k = dy/dx;
				b = dYPos[j] - k*(double)dXPos[j];

				dWeight = dWeight +
						  k*dXPos[j]*dXPos[j]/2 +
						  b*dXPos[j] -
						  k*dXPos[j-1]*dXPos[j-1]/2 -
						  b*dXPos[j-1];	

				dMoment = dMoment + 
						  k*dXPos[j]*dXPos[j]*dXPos[j]/3 +
						  b*dXPos[j]*dXPos[j]/2 -
						  k*dXPos[j-1]*dXPos[j-1]*dXPos[j-1]/3 -
						  b*dXPos[j-1]*dXPos[j-1]/2;
			}
			if (dWeight) {
				if (nDirection==FORWARD)
					pdFndPos[nFndEdges+1] = dMoment/dWeight + 0.5/*Offset*/ + nRef;
				else
					pdFndPos[nFndEdges+1] = dMoment/dWeight - 0.5/*Offset*/ + nRef;
				if (bEdgePos)
					pnFndGrad[nFndEdges+1] = POS;
				else
					pnFndGrad[nFndEdges+1] = NEG;
				nFndEdges++;
				if (nFndEdges>=MAX_GRD_EDGE) {
					AfxMessageBox("nFndEdges>MAX_GRD_EGE in GetEdge1D()");
					return -9;
				}
			}
			nStart=nEnd=-1;
			bEdgePos=bEdgeNeg=bEdgePresent=FALSE;
			dWeight = dMoment = 0;
			dXFirst=-1.0,dXLast=-1.0;
		}
	}
	pdFndPos[0]=nFndEdges;
	pnFndGrad[0]=nFndEdges;
	return 0;
}

int GetEdgeEx(long *pImageLine,
			  int nLength,
			  ADV_GRD_EDGE_FIND_PARM *pEdgeInfo,
			  double pdEdgePos[],
			  int pnEdgePol[])
{

	double*	pdDer1;
	double* pdDer2;
	double* pdImage;
	double	dExactPos;
	double	dThreshold;
	int		nPolarity;
	BOOL	bPEdgePresent,
			bNEdgePresent,
			bEdgePresent;
	double	dAvgGrad;
	double	dDel;
	double  dMaxStepSize;
	int		count,
			nStartIdx,
			nEndIdx,
			i,j, k, n1, n2, m;


	nStartIdx = 0;
	nEndIdx = nLength - 1;
	pdImage = new double[abs(nLength)];
	for (i=0; i<nLength; i++)
		pdImage[i] = (double)pImageLine[i];

	if (pEdgeInfo->nSmoothingMethod == BINOMIAL_SMOOTHING) {
		int nn, nm, nDel;
		double* pdValues;
		double* pdResult;

		nm = pEdgeInfo->nEdgeFilterSize;
		nn = nm - 1;
		if (nn > nLength) {
			delete [] pdImage;
			return -1;
		}
		pdValues = new double[abs(nm)];
		pdResult = new double[abs(nm+nLength-1)];
		binom(nn, pdValues);
		conv(pdImage, pdValues, nLength, nm, pdResult);
		nStartIdx = nn;
		nEndIdx = nStartIdx + nLength - nm;
		nDel = nn / 2;
		for (i= nDel, j = nStartIdx ; i < nLength - nDel; i++, j++) {
			pdImage[i] =  pdResult[j];
		}
		nStartIdx = nDel;
		nEndIdx = nLength - nDel - 1;

		delete [] pdValues;
		delete [] pdResult;
	}

	pdDer1 = new double[abs(nLength)];
	pdDer2 = new double[abs(nLength)];

	dThreshold = pEdgeInfo->nThreshold;
	dMaxStepSize = pEdgeInfo->nMaxStepSize;
	nPolarity = pEdgeInfo->nGradient;

	for (i=nStartIdx; i<(nEndIdx); i++) {
		pdDer1[i]=(pdImage[i+1]-pdImage[i]);
//		OutputDebugLog("pdDer1[%d] = %f", i, pdDer1[i]);
	}

	for (i=nStartIdx+1;i<(nEndIdx);i++) {
		pdDer2[i]=pdDer1[i]-pdDer1[i-1];
//		OutputDebugLog("pdDer2[%d] = %f", i, pdDer2[i]);
	}

	count=0;
	dAvgGrad = dThreshold / ((double) pEdgeInfo->nEdgeWidth);
	BOOL bSlopeOK = FALSE;

	for (i=nStartIdx+1;i<(nEndIdx-1);i++) {
		bEdgePresent=FALSE;
		bPEdgePresent=FALSE;
		bNEdgePresent=FALSE;
		bSlopeOK = FALSE;
		k=(pEdgeInfo->nEdgeWidth) / 2;
		if ((pdDer1[i] > dAvgGrad) && (pdDer2[i] >= 0) && (pdDer2[i+1] < 0) 
			&& ((nPolarity == POS) || (nPolarity == ANY)) ) {
//			OutputDebugLog("Positive edges");
			n1 = i-k;
			n2 = i+k;
			if (k==0)
				n2 = i+1;
			if ((n1 > nStartIdx) && (n2 < nEndIdx)) {
				dDel = pdImage[n2] - pdImage[n1]; 
				if ((dDel > dThreshold) && (dDel < dMaxStepSize)) {
					bSlopeOK = TRUE;
					for (m=n1; ((m<n2) && bSlopeOK); m++) {
						if (pdDer1[m] < 0)
							bSlopeOK = FALSE;
					}
				}
				else if (!(pEdgeInfo->nEdgeWidth % 2)) {
					if ((n2+1) < nEndIdx) {
						n1++;
						n2++;
						dDel = pdImage[n2] - pdImage[n1];
						if ((dDel > dThreshold) && (dDel < dMaxStepSize)) {
							bSlopeOK = TRUE;
							for (m=n1; ((m<n2) && bSlopeOK); m++) {
								if (pdDer1[m] < 0)
									bSlopeOK = FALSE;
							}
						}
					}
				}
				if (bSlopeOK)
					bPEdgePresent = TRUE;
			}
		}
		else if ((pdDer1[i] < (-1.0)*dAvgGrad) && (pdDer2[i] <= 0) && (pdDer2[i+1] > 0)
			&& ((nPolarity == NEG) || (nPolarity == ANY)) ) {
//			OutputDebugLog("Negative edges");
			n1 = i-k;
			n2 = i+k;
			if (k==0)
				n2 = i+1;
			if ((n1 > nStartIdx) && (n2 < nEndIdx)) {
				dDel = pdImage[n2] - pdImage[n1];
				if ((dDel < (-1.0)*dThreshold) && (dDel > (-1.0)*dMaxStepSize)) {
					bSlopeOK = TRUE;
					for (m=n1; ((m<n2) && bSlopeOK); m++) {
						if (pdDer1[m] > 0)
							bSlopeOK = FALSE;
					}
				}
				else if (!(pEdgeInfo->nEdgeWidth % 2)) {
					if ((n1+1) < nEndIdx) {
						n1++;
						n2++;
						dDel = pdImage[n2] - pdImage[n1];
						if ( (dDel < (-1.0)*dThreshold) && (dDel > (-1.0)*dMaxStepSize) ) {
							bSlopeOK = TRUE;
							for (m=n1; ((m<n2) && bSlopeOK); m++) {
								if (pdDer1[m] > 0)
									bSlopeOK = FALSE;
							}
						}
					}
				}
				if (bSlopeOK)
					bNEdgePresent = TRUE;
			}
		}
		if ((nPolarity == POS) && (bPEdgePresent))
			bEdgePresent = TRUE;
		else if ((nPolarity == NEG) && (bNEdgePresent))
			bEdgePresent = TRUE;
		else if ((nPolarity == ANY) && (bPEdgePresent || bNEdgePresent))
			bEdgePresent = TRUE;
		if ((bEdgePresent) && (count < MAX_GRD_EDGE)) {
		    dExactPos=((double) i)+fabs(pdDer2[i])/(fabs(pdDer2[i]) + fabs(pdDer2[i+1]));
//			OutputDebugLog("Edge position = %f", dExactPos);
			pdEdgePos[count+1]=dExactPos;
			if (bPEdgePresent)
				pnEdgePol[count+1] = POS;
			else
				pnEdgePol[count+1] = NEG;
			count++;
		}
	}
	pdEdgePos[0]=count;

	delete [] pdImage;
	delete [] pdDer1;
	delete [] pdDer2;

	return 0;
}

int GetEdgeExNew(long *pImageLine,
				 int nLength,
				 double *ptrMask,
				 ADV_GRD_EDGE_FIND_PARM *pEdgeInfo,
				 double pdEdgePos[],
				 int pnEdgePol[],
				 int nOffset)
{

	double pdDer1[MAX_PROJ];
	double pdDer2[MAX_PROJ];

	double	dExactPos;
	double	dThreshold;
	int		nPolarity;
	BOOL	bPEdgePresent,
			bNEdgePresent,
			bEdgePresent;
	double	dAvgGrad;
	double	dDel;
	double  dMaxStepSize;
	int		count,
			nStartIdx,
			nEndIdx,
			i, k, n1, n2, m;


	nStartIdx = 0;
	nEndIdx = nLength - 1;

	if (pEdgeInfo->nSmoothingMethod == BINOMIAL_SMOOTHING) {
		ApplyMask(pImageLine,nLength,ptrMask,(pEdgeInfo->nEdgeFilterSize-1)/2);
//
		nStartIdx = (pEdgeInfo->nEdgeFilterSize-1)/2;
		nEndIdx = nLength - 1 - nStartIdx;
	}

	dThreshold = pEdgeInfo->nThreshold;
	dMaxStepSize = pEdgeInfo->nMaxStepSize;
	nPolarity = pEdgeInfo->nGradient;

	for (i=nStartIdx; i<nEndIdx; i++) {
		pdDer1[i]=(double)(pImageLine[i+1]-pImageLine[i]);
	}

	for (i=nStartIdx+1;i<(nEndIdx);i++) {
		pdDer2[i]=pdDer1[i]-pdDer1[i-1];
	}

	count=0;
	dAvgGrad = dThreshold / ((double) pEdgeInfo->nEdgeWidth);
	BOOL bSlopeOK = FALSE;

	for (i=nStartIdx+1;i<(nEndIdx-1);i++) {
		bEdgePresent=FALSE;
		bPEdgePresent=FALSE;
		bNEdgePresent=FALSE;
		bSlopeOK = FALSE;
		k=(pEdgeInfo->nEdgeWidth) / 2;
		if ((pdDer1[i] > dAvgGrad) && (pdDer2[i] >= 0) && (pdDer2[i+1] < 0) 
			&& ((nPolarity == POS) || (nPolarity == ANY)) ) {
			n1 = i-k;
			n2 = i+k;
			if (k==0)
				n2 = i+1;
			if ((n1 > nStartIdx) && (n2 < nEndIdx)) {
				dDel = (double)(pImageLine[n2] - pImageLine[n1]); 
				if ((dDel > dThreshold) && (dDel < dMaxStepSize)) {
					bSlopeOK = TRUE;
					for (m=n1; ((m<n2) && bSlopeOK); m++) {
						if (pdDer1[m] < 0)
							bSlopeOK = FALSE;
					}
				}
				else if (!(pEdgeInfo->nEdgeWidth % 2)) {
					if ((n2+1) < nEndIdx) {
						n1++;
						n2++;
						dDel = (double)(pImageLine[n2] - pImageLine[n1]);
						if ((dDel > dThreshold) && (dDel < dMaxStepSize)) {
							bSlopeOK = TRUE;
							for (m=n1; ((m<n2) && bSlopeOK); m++) {
								if (pdDer1[m] < 0)
									bSlopeOK = FALSE;
							}
						}
					}
				}
				if (bSlopeOK)
					bPEdgePresent = TRUE;
			}
		}
		else if ((pdDer1[i] < (-1.0)*dAvgGrad) && (pdDer2[i] <= 0) && (pdDer2[i+1] > 0)
			&& ((nPolarity == NEG) || (nPolarity == ANY)) ) {
			n1 = i-k;
			n2 = i+k;
			if (k==0)
				n2 = i+1;
			if ((n1 > nStartIdx) && (n2 < nEndIdx)) {
				dDel = (double)(pImageLine[n2] - pImageLine[n1]);
				if ((dDel < (-1.0)*dThreshold) && (dDel > (-1.0)*dMaxStepSize)) {
					bSlopeOK = TRUE;
					for (m=n1; ((m<n2) && bSlopeOK); m++) {
						if (pdDer1[m] > 0)
							bSlopeOK = FALSE;
					}
				}
				else if (!(pEdgeInfo->nEdgeWidth % 2)) {
					if ((n1+1) < nEndIdx) {
						n1++;
						n2++;
						dDel = (double)(pImageLine[n2] - pImageLine[n1]);
						if ( (dDel < (-1.0)*dThreshold) && (dDel > (-1.0)*dMaxStepSize) ) {
							bSlopeOK = TRUE;
							for (m=n1; ((m<n2) && bSlopeOK); m++) {
								if (pdDer1[m] > 0)
									bSlopeOK = FALSE;
							}
						}
					}
				}
				if (bSlopeOK)
					bNEdgePresent = TRUE;
			}
		}
		if ((nPolarity == POS) && (bPEdgePresent))
			bEdgePresent = TRUE;
		else if ((nPolarity == NEG) && (bNEdgePresent))
			bEdgePresent = TRUE;
		else if ((nPolarity == ANY) && (bPEdgePresent || bNEdgePresent))
			bEdgePresent = TRUE;
		if ((bEdgePresent) && (count < MAX_GRD_EDGE)) {
		    dExactPos=((double)(i+nOffset))+fabs(pdDer2[i])/(fabs(pdDer2[i]) + fabs(pdDer2[i+1]));
			pdEdgePos[count+1]=dExactPos;
			if (bPEdgePresent)
				pnEdgePol[count+1] = POS;
			else
				pnEdgePol[count+1] = NEG;
			count++;
		}
	}
	pdEdgePos[0]=count;
	return 0;
}

int GetEdgeExNew2(BYTE *pImageLine,
				 int nLength,
				 double *ptrMask,
				 ADV_GRD_EDGE_FIND_PARM *pEdgeInfo,
				 double pdEdgePos[],
				 int pnEdgePol[],
				 int nOffset)
{

	int pnDer1[MAX_PROJ];
	int pnDer2[MAX_PROJ];

	double	dExactPos;
	int	nThresholdPos;
	int	nThresholdNeg;
	int		nPolarity;
	BOOL	bPEdgePresent,
			bNEdgePresent,
			bEdgePresent;
	int	nAvgGradPos;
	int	nAvgGradNeg;
	int	nDel;
	int nMaxStepSizePos;
	int nMaxStepSizeNeg;
	int		count,
			nStartIdx,
			nEndIdx,
			i, k, n1, n2, m;


	nStartIdx = 0;
	nEndIdx = nLength - 1;
/*
	if (pEdgeInfo->nSmoothingMethod == BINOMIAL_SMOOTHING) {
		ApplyMask(pImageLine,nLength,ptrMask,(pEdgeInfo->nEdgeFilterSize-1)/2);
	}
*/
/*
int x[200];
for(i=0;i<nLength;i++) {
	x[i] = pImageLine[i];
}
*/
	nThresholdPos = pEdgeInfo->nThreshold;
	nThresholdNeg = -nThresholdPos;

	nMaxStepSizePos = pEdgeInfo->nMaxStepSize;
	nMaxStepSizeNeg = -nMaxStepSizePos;
	nPolarity = pEdgeInfo->nGradient;

	for (i=nStartIdx; i<nEndIdx; i++) {
		pnDer1[i]= pImageLine[i+1]-pImageLine[i];
	}

	for (i=nStartIdx+1;i<(nEndIdx);i++) {
		pnDer2[i]=pnDer1[i]-pnDer1[i-1];
	}

	count=0;
	nAvgGradPos = nThresholdPos / pEdgeInfo->nEdgeWidth;
	nAvgGradNeg = -nAvgGradPos;

	BOOL bSlopeOK = FALSE;

	for (i=nStartIdx+1;i<(nEndIdx-1);i++) {
		bEdgePresent=FALSE;
		bPEdgePresent=FALSE;
		bNEdgePresent=FALSE;
		bSlopeOK = FALSE;
		k=(pEdgeInfo->nEdgeWidth) / 2;
		if ((pnDer1[i] > nAvgGradPos) && (pnDer2[i] >= 0) && (pnDer2[i+1] < 0) 
			&& ((nPolarity == POS) || (nPolarity == ANY)) ) {
			n1 = i-k;
			n2 = i+k;
			if (k==0)
				n2 = i+1;
			if ((n1 > nStartIdx) && (n2 < nEndIdx)) {
				nDel = pImageLine[n2] - pImageLine[n1]; 
				if ((nDel > nThresholdPos) && (nDel < nMaxStepSizePos)) {
					bSlopeOK = TRUE;
					for (m=n1; ((m<n2) && bSlopeOK); m++) {
						if (pnDer1[m] < 0)
							bSlopeOK = FALSE;
					}
				}
				else if (!(pEdgeInfo->nEdgeWidth % 2)) {
					if ((n2+1) < nEndIdx) {
						n1++;
						n2++;
						nDel = pImageLine[n2] - pImageLine[n1];
						if ((nDel > nThresholdPos) && (nDel < nMaxStepSizePos)) {
							bSlopeOK = TRUE;
							for (m=n1; ((m<n2) && bSlopeOK); m++) {
								if (pnDer1[m] < 0)
									bSlopeOK = FALSE;
							}
						}
					}
				}
				if (bSlopeOK)
					bPEdgePresent = TRUE;
			}
		}
		else if ((pnDer1[i] < nAvgGradNeg) && (pnDer2[i] <= 0) && (pnDer2[i+1] > 0)
			&& ((nPolarity == NEG) || (nPolarity == ANY)) ) {
			n1 = i-k;
			n2 = i+k;
			if (k==0)
				n2 = i+1;
			if ((n1 > nStartIdx) && (n2 < nEndIdx)) {
				nDel = pImageLine[n2] - pImageLine[n1];
				if ((nDel < nThresholdNeg) && (nDel > nMaxStepSizeNeg)) {
					bSlopeOK = TRUE;
					for (m=n1; ((m<n2) && bSlopeOK); m++) {
						if (pnDer1[m] > 0)
							bSlopeOK = FALSE;
					}
				}
				else if (!(pEdgeInfo->nEdgeWidth % 2)) {
					if ((n1+1) < nEndIdx) {
						n1++;
						n2++;
						nDel = pImageLine[n2] - pImageLine[n1];
						if ( (nDel < nThresholdNeg) && (nDel > nMaxStepSizeNeg) ) {
							bSlopeOK = TRUE;
							for (m=n1; ((m<n2) && bSlopeOK); m++) {
								if (pnDer1[m] > 0)
									bSlopeOK = FALSE;
							}
						}
					}
				}
				if (bSlopeOK)
					bNEdgePresent = TRUE;
			}
		}
		if ((nPolarity == POS) && (bPEdgePresent))
			bEdgePresent = TRUE;
		else if ((nPolarity == NEG) && (bNEdgePresent))
			bEdgePresent = TRUE;
		else if ((nPolarity == ANY) && (bPEdgePresent || bNEdgePresent))
			bEdgePresent = TRUE;
		if ((bEdgePresent) && (count < MAX_GRD_EDGE)) {
		    dExactPos=((double)(i+nOffset))+abs(pnDer2[i])/(abs(pnDer2[i]) + abs(pnDer2[i+1]));
			pdEdgePos[count+1]=dExactPos;
			if (bPEdgePresent)
				pnEdgePol[count+1] = POS;
			else
				pnEdgePol[count+1] = NEG;
			count++;
		}
	}
	pdEdgePos[0]=count;

	return 0;
}

int HoughTransform(CRect rectRgn, double* pdX, double* pdY, int nCount, 
				   LINE_FIND_PARM *pLineParm, double& dTheta, double& dR,
				   double& dRangeMax)
{
	static int	  pAccumulator[HOUGHSPACE_THETA_DIMENSION][HOUGHSPACE_RHO_DIMENSION];
	static double dPosMin[HOUGHSPACE_THETA_DIMENSION][HOUGHSPACE_RHO_DIMENSION];
	static double dPosMax[HOUGHSPACE_THETA_DIMENSION][HOUGHSPACE_RHO_DIMENSION];


	int		nCenterX, i, j, nRMax, nTMax, nR, nMax;
	double	dRange, dAngleMin, dAngleMax; 
	bool	bSigned;
	int		nRows, nColumns, nAngleMin, nAngleMax, nAngleStep, nAngle;

	dRange = 0;

	// Compute Hough Space
	nCenterX = (int)sqrt((double) (rectRgn.Width()  * rectRgn.Width() + 
		                 rectRgn.Height() * rectRgn.Height())) / 2;

	dAngleMin = pLineParm->dNominalAngle - pLineParm->dAngleDeviation;
	dAngleMax = pLineParm->dNominalAngle + pLineParm->dAngleDeviation;
	nAngleStep = GetAngleStep(pLineParm->dAngleStep);
	if (dAngleMin < 0) {
		nAngleMin = GetSignedAngleIndex(dAngleMin);
		nAngleMax = GetSignedAngleIndex(dAngleMax);
		bSigned = true;
	}
	else {
		nAngleMin = GetAngleIndex(dAngleMin);
		nAngleMax = GetAngleIndex(dAngleMax);
		bSigned = false;
	}

	nColumns = 2 * nCenterX + 1;
	nRows = (nAngleMax - nAngleMin)/nAngleStep + 1;

	if(nRows > HOUGHSPACE_THETA_DIMENSION) {
		AfxMessageBox("Theta bin in Hough space exceeds HOUGHSPACE_THETA_DIMENSION in HoughTransform()");
		return 0;
	}
	if(nColumns > HOUGHSPACE_RHO_DIMENSION) {
		AfxMessageBox("Rho bin in Hough space exceeds HOUGHSPACE_RHO_DIMENSION in HoughTransform()");
		return 0;
	}

	// Initialize Hough space
	for (i = 0; i < nRows; i++) {
		for (j = 0; j < nColumns; j++) {
			dPosMin[i][j] = 4000.0;
			dPosMax[i][j] = 0.0;
			pAccumulator[i][j] = 0;
		}
	}

	nRMax = 0;
	nTMax = 0;
	BOOL bVert;

	// * NON-STANDARD HOUGH TRANSFORM ASSUMPTION * 
	// If norminal orientation is less than 45 degrees then horizontal co-ordinates 
	// of edge points in pDX are equally spaced.  Likewise for vertical co-ordinates 
	// in pDY is orientation is more than 45 degrees. 
	if(pLineParm->dNominalAngle<45)
		bVert = FALSE;
	else
		bVert = TRUE;

	// Accumulate votes in Hough space
	for(i = 0; i < nCount; i++) {
		nAngle = nAngleMin;
		for(j = 0; j < nRows; j++) {
			if (bSigned) {
				dR = (pdX[i] * cscos[nAngle] + pdY[i] * cssin[nAngle]) / 
					pLineParm->dRhoBinSize;
			}
			else {
				dR = (pdX[i] * ccos[nAngle] + pdY[i] * csin[nAngle]) / 
					pLineParm->dRhoBinSize;
			}

			nR = (int)floor(dR + 0.5);

			if(nR < nColumns) {
				(pAccumulator[j][nR])++;
				
				// Store spatial extents of each point in Hough space 
				if(bVert) {
					if(pdX[i] < dPosMin[j][nR])
						dPosMin[j][nR] = pdX[i];
					else if(pdX[i] > dPosMax[j][nR])
						dPosMax[j][nR] = pdX[i];
				}
				else {
					if(pdY[i] < dPosMin[j][nR])
						dPosMin[j][nR] = pdY[i];
					else if(pdY[i] > dPosMax[j][nR])
						dPosMax[j][nR] = pdY[i];
				}
			}
			nAngle += nAngleStep;
		}
	}
	// Find the maximum in the Hough space
	nMax = 0;
	for(i = 0; i < nRows; i++) {
		for(j = 0; j < nColumns; j++) {
			if(pAccumulator[i][j] > nMax) {
				nTMax  = i;
				nRMax  = j;
				nMax   = pAccumulator[i][j];
				dRange = dPosMax[i][j] - dPosMin[i][j];
			}
			else if(pAccumulator[i][j] == nMax) {
				// If tie in votes count then choose  
				// one with greater spatial range  

				if((dPosMax[i][j] - dPosMin[i][j]) > dRange) {
					nTMax  = i;
					nRMax  = j;
					nMax   = pAccumulator[i][j];
					dRange = dPosMax[i][j] - dPosMin[i][j];
				}
			}
		}
	}
	// Compute parameters of detected line
	dR        = nRMax * pLineParm->dRhoBinSize;
	if (bSigned)
		dTheta = GetSignedAngle(nAngleMin+nTMax*nAngleStep);
	else 
		dTheta = GetAngle(nAngleMin+nTMax*nAngleStep);
	dRangeMax = dRange;

	return nMax;
}

int HoughTransformNew(CRect rectRgn, int* pnX, int *pnY, int nCount, 
				   LINE_FIND_PARM *pLineParm, double& dTheta, double& dR,
				   double& dRangeMax)
{
	static int	  pAccumulator[HOUGHSPACE_THETA_DIMENSION][HOUGHSPACE_RHO_DIMENSION];
	static int nPosMin[HOUGHSPACE_THETA_DIMENSION][HOUGHSPACE_RHO_DIMENSION];
	static int nPosMax[HOUGHSPACE_THETA_DIMENSION][HOUGHSPACE_RHO_DIMENSION];


	int		nCenterX, i, j, nRMax, nTMax, nR, nMax;
	double	dAngleMin, dAngleMax; 
	int		nRange;
	bool	bSigned;
	int		nRows, nColumns, nAngleMin, nAngleMax, nAngleStep, nAngle;

	nRange = 0;

	// Compute Hough Space
	nCenterX = (int)sqrt((double) (rectRgn.Width()  * rectRgn.Width() + 
		                 rectRgn.Height() * rectRgn.Height())) / 2;

	dAngleMin = pLineParm->dNominalAngle - pLineParm->dAngleDeviation;
	dAngleMax = pLineParm->dNominalAngle + pLineParm->dAngleDeviation;
	nAngleStep = GetAngleStep(pLineParm->dAngleStep);
	if (dAngleMin < 0) {
		nAngleMin = GetSignedAngleIndex(dAngleMin);
		nAngleMax = GetSignedAngleIndex(dAngleMax);
		bSigned = true;
	}
	else {
		nAngleMin = GetAngleIndex(dAngleMin);
		nAngleMax = GetAngleIndex(dAngleMax);
		bSigned = false;
	}

	nColumns = 2 * nCenterX + 1;
	nRows = (nAngleMax - nAngleMin)/nAngleStep + 1;

	if(nRows > HOUGHSPACE_THETA_DIMENSION) {
		AfxMessageBox("Theta bin in Hough space exceeds HOUGHSPACE_THETA_DIMENSION in HoughTransform()");
		return 0;
	}
	if(nColumns > HOUGHSPACE_RHO_DIMENSION) {
		AfxMessageBox("Rho bin in Hough space exceeds HOUGHSPACE_RHO_DIMENSION in HoughTransform()");
		return 0;
	}

	// Initialize Hough space
	for (i = 0; i < nRows; i++) {
		for (j = 0; j < nColumns; j++) {
			nPosMin[i][j] = 4000;
			nPosMax[i][j] = 0;
			pAccumulator[i][j] = 0;
		}
	}

	nRMax = 0;
	nTMax = 0;
	BOOL bVert;

	// * NON-STANDARD HOUGH TRANSFORM ASSUMPTION * 
	// If norminal orientation is less than 45 degrees then horizontal co-ordinates 
	// of edge points in pDX are equally spaced.  Likewise for vertical co-ordinates 
	// in pDY is orientation is more than 45 degrees. 
	if(pLineParm->dNominalAngle<45)
		bVert = FALSE;
	else
		bVert = TRUE;

	// Accumulate votes in Hough space
	for(i = 0; i < nCount; i++) {
		nAngle = nAngleMin;
		for(j = 0; j < nRows; j++) {
			if (bSigned) {
				dR = (pnX[i] * cscos[nAngle] + pnY[i] * cssin[nAngle]);
			}
			else {
				dR = (pnX[i] * ccos[nAngle] + pnY[i] * csin[nAngle]);
			}

			nR = (int)floor(dR + 0.5);

			if(nR < nColumns) {
				(pAccumulator[j][nR])++;
				
				// Store spatial extents of each point in Hough space 
				if(bVert) {
					if(pnX[i] < nPosMin[j][nR])
						nPosMin[j][nR] = pnX[i];
					else if(pnX[i] > nPosMax[j][nR])
						nPosMax[j][nR] = pnX[i];
				}
				else {
					if(pnY[i] < nPosMin[j][nR])
						nPosMin[j][nR] = pnY[i];
					else if(pnY[i] > nPosMax[j][nR])
						nPosMax[j][nR] = pnY[i];
				}
			}
			nAngle += nAngleStep;
		}
	}
	// Find the maximum in the Hough space
	nMax = 0;
	for(i = 0; i < nRows; i++) {
		for(j = 0; j < nColumns; j++) {
			if(pAccumulator[i][j] > nMax) {
				nTMax  = i;
				nRMax  = j;
				nMax   = pAccumulator[i][j];
				nRange = nPosMax[i][j] - nPosMin[i][j];
			}
			else if(pAccumulator[i][j] == nMax) {
				// If tie in votes count then choose  
				// one with greater spatial range  

				if((nPosMax[i][j] - nPosMin[i][j]) > nRange) {
					nTMax  = i;
					nRMax  = j;
					nMax   = pAccumulator[i][j];
					nRange = nPosMax[i][j] - nPosMin[i][j];
				}
			}
		}
	}
	// Compute parameters of detected line
	dR        = nRMax;
	if (bSigned)
		dTheta = GetSignedAngle(nAngleMin+nTMax*nAngleStep);
	else 
		dTheta = GetAngle(nAngleMin+nTMax*nAngleStep);
	dRangeMax = nRange;

	return nMax;
}



CRange::CRange()
{
	dStart = 0;
	dEnd = 0;
}

CRange::~CRange()
{
}


CRingRange::CRingRange()
{
	pnPosY = NULL;
	pcsLeftRange = NULL;
	pcsRightRange = NULL;
	nRangeNo = 0;
}

CRingRange::~CRingRange()
{
	Clean();
}

void CRingRange::Clean()
{
	if(pnPosY != NULL)
		delete [] pnPosY;
	if(pcsLeftRange != NULL)
		delete [] pcsLeftRange;
	if(pcsRightRange != NULL)
		delete [] pcsRightRange;
	pnPosY = NULL;
	pcsLeftRange = NULL;
	pcsRightRange = NULL;
	nRangeNo = 0;
}

int CRingRange::FindRange(CPoint2D* ppt2DCtr,
				  double dInnerRadius,
				  double dOuterRadius)
{
	int i;
	int nStartY;
	int nEndY;
	double dDisX;
	double dDisY;
	double dDisX0;
	double dDisX1;
	int nNo;
	int nInnerRadius;
	int nOuterRadius;
	int nRangeMemNo;
	Clean();
try
{

	nInnerRadius = Round(dInnerRadius);
	nOuterRadius = Round(dOuterRadius);
	if((nInnerRadius < 0) || (nOuterRadius < 0))
		goto ErrorAbort;
	if((dOuterRadius - dInnerRadius) < 1)
		goto ErrorAbort;
	nRangeNo = Round(dOuterRadius * 2 + 1);
	nRangeMemNo = nRangeNo + 10;
	pnPosY = new int[nRangeMemNo];
	pcsLeftRange = new CSize[nRangeMemNo];
	pcsRightRange = new CSize[nRangeMemNo];
	for(i=0;i<nRangeNo;i++)
	{
		pnPosY[i] = 0;
		pcsLeftRange[i] = CSize(0,0);
		pcsRightRange[i] = CSize(0,0);
	}
	nStartY = Round(ppt2DCtr->y - dOuterRadius);
	nEndY = Round(ppt2DCtr->y - dInnerRadius);
	nNo = 0;
	for(i=nStartY;i<nEndY;i++)
	{
		dDisY = ppt2DCtr->y - i;
		if(dDisY < dOuterRadius)
		{
			pnPosY[nNo] = i;
			dDisX = sqrt(dOuterRadius * dOuterRadius - dDisY * dDisY);
			pcsLeftRange[nNo] = CSize(Round(ppt2DCtr->x - dDisX), Round(ppt2DCtr->x));
			pcsRightRange[nNo] = CSize(Round(ppt2DCtr->x), Round(ppt2DCtr->x + dDisX));
		}
		nNo++;
	}
	nStartY = Round(ppt2DCtr->y - dInnerRadius);
	nEndY = Round(ppt2DCtr->y + dInnerRadius);
	for(i=nStartY;i<nEndY;i++)
	{
		dDisY = ppt2DCtr->y - i;
		if(fabs(dDisY) < dInnerRadius)
		{
			pnPosY[nNo] = i;
			dDisX0 = sqrt(dOuterRadius * dOuterRadius - dDisY * dDisY);
			dDisX1 = sqrt(dInnerRadius * dInnerRadius - dDisY * dDisY);
			pcsLeftRange[nNo] = CSize(Round(ppt2DCtr->x - dDisX0), Round(ppt2DCtr->x - dDisX1));
			pcsRightRange[nNo] = CSize(Round(ppt2DCtr->x + dDisX1), Round(ppt2DCtr->x + dDisX0));
		}
		else
		{
			pnPosY[nNo] = i;
			dDisX = sqrt(dOuterRadius * dOuterRadius - dDisY * dDisY);
			pcsLeftRange[nNo] = CSize(Round(ppt2DCtr->x - dDisX), Round(ppt2DCtr->x));
			pcsRightRange[nNo] = CSize(Round(ppt2DCtr->x), Round(ppt2DCtr->x + dDisX));
		}
		nNo++;
	}
	nStartY = Round(ppt2DCtr->y + dInnerRadius);
	nEndY = Round(ppt2DCtr->y + dOuterRadius);
	for(i=nStartY;i<nEndY;i++)
	{
		dDisY = i - ppt2DCtr->y;
		if(dDisY < dOuterRadius)
		{
			pnPosY[nNo] = i;
			dDisX = sqrt(dOuterRadius * dOuterRadius - dDisY * dDisY);
			pcsLeftRange[nNo] = CSize(Round(ppt2DCtr->x - dDisX), Round(ppt2DCtr->x));
			pcsRightRange[nNo] = CSize(Round(ppt2DCtr->x), Round(ppt2DCtr->x + dDisX));
		}
		nNo++;
	}
	nRangeNo = nNo;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CRingRange::FindRange");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CRingRange::FindRange(CPoint2D* ppt2DCtrInner,
				  double dInnerRadius,
				  CPoint2D* ppt2DCtrOuter,
				  double dOuterRadius)
{
	int i;
	int nStartY;
	int nEndY;
	double dDisOuterX;
	double dDisOuterY;
	double dDisInnerY;
	double dDisX0;
	double dDisX1;
	int nNo;
	int nInnerRadius;
	int nOuterRadius;
	double dDisX;
	double dDisY;
	double dCtrDis;
	int nRangeMemNo;
	Clean();
try
{
	dCtrDis = ppt2DCtrInner->GetDistance(*ppt2DCtrOuter);
	nInnerRadius = Round(dInnerRadius);
	nOuterRadius = Round(dOuterRadius);
	if((nInnerRadius < 0) || (nOuterRadius < 0))
		goto ErrorAbort;
	if((dOuterRadius - dInnerRadius) < 1)
		goto ErrorAbort;
	nRangeNo = Round((dInnerRadius + dOuterRadius) * 2 + dCtrDis + 1);
	nRangeMemNo = nRangeNo + 10;
	pnPosY = new int[nRangeMemNo];
	pcsLeftRange = new CSize[nRangeMemNo];
	pcsRightRange = new CSize[nRangeMemNo];
	for(i=0;i<nRangeNo;i++)
	{
		pnPosY[i] = 0;
		pcsLeftRange[i] = CSize(0,0);
		pcsRightRange[i] = CSize(0,0);
	}
	nStartY = Round(ppt2DCtrOuter->y - dOuterRadius);
	nEndY = Round(ppt2DCtrInner->y - dInnerRadius);
	nNo = 0;
	for(i=nStartY;i<nEndY;i++)
	{
		dDisOuterY = ppt2DCtrOuter->y - i;
		if(dDisOuterY < dOuterRadius)
		{
			pnPosY[nNo] = i;
			dDisOuterX = sqrt(dOuterRadius * dOuterRadius - dDisOuterY * dDisOuterY);
			pcsLeftRange[nNo] = CSize(Round(ppt2DCtrOuter->x - dDisOuterX), Round(ppt2DCtrOuter->x));
			pcsRightRange[nNo] = CSize(Round(ppt2DCtrOuter->x), Round(ppt2DCtrOuter->x + dDisOuterX));
		}
		nNo++;
	}
	nStartY = Round(ppt2DCtrInner->y - dInnerRadius);
	nEndY = Round(ppt2DCtrInner->y + dInnerRadius);
	for(i=nStartY;i<nEndY;i++)
	{
		dDisOuterY = ppt2DCtrOuter->y - i;
		dDisInnerY = ppt2DCtrInner->y - i;
		if((fabs(dDisInnerY) < dInnerRadius) && (fabs(dDisOuterY) < dOuterRadius))
		{
			pnPosY[nNo] = i;
			dDisX0 = sqrt(dOuterRadius * dOuterRadius - dDisOuterY * dDisOuterY);
			dDisX1 = sqrt(dInnerRadius * dInnerRadius - dDisInnerY * dDisInnerY);
			pcsLeftRange[nNo] = CSize(Round(ppt2DCtrOuter->x - dDisX0), Round(ppt2DCtrInner->x - dDisX1));
			pcsRightRange[nNo] = CSize(Round(ppt2DCtrInner->x + dDisX1), Round(ppt2DCtrOuter->x + dDisX0));
		}
		else
		{
			if(fabs(dDisOuterY) < dOuterRadius)
			{
				pnPosY[nNo] = i;
				dDisX = sqrt(dOuterRadius * dOuterRadius - dDisOuterY * dDisOuterY);
				pcsLeftRange[nNo] = CSize(Round(ppt2DCtrOuter->x - dDisX), Round(ppt2DCtrOuter->x));
				pcsRightRange[nNo] = CSize(Round(ppt2DCtrOuter->x), Round(ppt2DCtrOuter->x + dDisX));
			}
		}
		nNo++;
	}
	nStartY = Round(ppt2DCtrInner->y + dInnerRadius);
	nEndY = Round(ppt2DCtrOuter->y + dOuterRadius);
	for(i=nStartY;i<nEndY;i++)
	{
		dDisY = i - ppt2DCtrOuter->y;
		if(dDisY < dOuterRadius)
		{
			pnPosY[nNo] = i;
			dDisX = sqrt(dOuterRadius * dOuterRadius - dDisY * dDisY);
			pcsLeftRange[nNo] = CSize(Round(ppt2DCtrOuter->x - dDisX), Round(ppt2DCtrOuter->x));
			pcsRightRange[nNo] = CSize(Round(ppt2DCtrOuter->x), Round(ppt2DCtrOuter->x + dDisX));
		}
		nNo++;
	}
	nRangeNo = nNo;

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CRingRange::FindRange");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CRingRange::FindRingMask(CImageBuffer *pImgMask,
			 CRect *prcMask,
			 CPoint* pptOffset)
{
	int i,j;
	int nImgPitch;
	BYTE *pImgMaskPtr;
	long lIndex;
	int y;
	int nMinX, nMinY;
	int nMaxX, nMaxY;

try
{
	pImgMask->ClrImg(0);
	if (!pImgMask) {
		ASSERT(!"pImgMask is a bad Pointer");
		return -1;
	}
	if (!pImgMask->IsAllocated()) {
		ASSERT(!"pImgMask is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMask,&(pImgMask->GetImgSize()))) {
		ASSERT(!"prcMask is Invalid");
		return -1;
	}
	nMinX = prcMask->left;
	nMinY = prcMask->top;
	nMaxX = prcMask->right;
	nMaxY = prcMask->bottom;
	nImgPitch = pImgMask->GetPitch();
	pImgMaskPtr = pImgMask->GetImgPtr();
	for(i=0;i<nRangeNo;i++) {
		y = pnPosY[i] - pptOffset->y;
		if((y < nMinY) || (y >= nMaxY))
			continue;
		lIndex = y * nImgPitch;
		for(j=__max(nMinX, pcsLeftRange[i].cx - pptOffset->x);j<__min(nMaxX, pcsLeftRange[i].cy - pptOffset->x);j++) {
			pImgMaskPtr[lIndex + j] = 255;
		}
		for(j=__max(nMinX, pcsRightRange[i].cx - pptOffset->x);j<__min(nMaxX, pcsRightRange[i].cy - pptOffset->x);j++) {
			pImgMaskPtr[lIndex + j] = 255;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CRingRange::FindRingMask");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CRingRange::BinariseGradImgAndAddress(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 CSize* pcsGradStep,
			 BYTE nThresVal,
			 long* plBlobPixelAddress,
			 long* plBlobPixelNo)
{
	int i,j;
	int x, y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pImgSrcPtr;
	BYTE *pImgDstPtr;
	long lIndexSrc;
	long lIndexSrcRow;
	long lIndexDstRow;
	long lStepOffsetX;
	long lStepOffsetY;
	long lBlobPixelNo;
	CRect rcOffset;
	int nMinX, nMinY;
	int nMaxX, nMaxY;

try
{
	*plBlobPixelNo = 0;
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}
	if (prcSrc->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc Size different from prcDst Size");
		return -1;
	}

	if (nThresVal<0 && nThresVal>=256) {
		ASSERT(!"nThresVal Out Of Range");
		return -1;
	}
	if (pcsGradStep->cx<=0 || pcsGradStep->cy<=0) {
		ASSERT(!"pcsGradStep Out Of Range");
		return -1;
	}


	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	lStepOffsetX = pcsGradStep->cx;
	lStepOffsetY = pcsGradStep->cy * nImgPitchSrc;
	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();

	pImgDst->ClrImg(0);
	pImgSrcPtr = pImgSrc->GetImgPtr();
	pImgDstPtr = pImgDst->GetImgPtr();
	lBlobPixelNo = 0;


	nMinX = prcSrc->left+pcsGradStep->cx;
	nMinY = prcSrc->top+pcsGradStep->cy;
	nMaxX = prcSrc->right-pcsGradStep->cx;
	nMaxY = prcSrc->bottom-pcsGradStep->cy;

	for(i=0;i<nRangeNo;i++) {
		y = pnPosY[i];
		if((y < nMinY) || (y >= nMaxY))
			continue;
		lIndexSrcRow = y * nImgPitchSrc;
		lIndexDstRow = (y - prcSrc->top + prcDst->top) * nImgPitchDst;
		for(j=__max(nMinX, pcsLeftRange[i].cx);j<__min(nMaxX, pcsLeftRange[i].cy);j++) {
			lIndexSrc = lIndexSrcRow+j;
			if((((pImgSrcPtr[lIndexSrc] - pImgSrcPtr[lIndexSrc-lStepOffsetX]) > nThresVal) && ((pImgSrcPtr[lIndexSrc] - pImgSrcPtr[lIndexSrc+lStepOffsetX]) > nThresVal)) ||
			   (((pImgSrcPtr[lIndexSrc] - pImgSrcPtr[lIndexSrc-lStepOffsetY]) > nThresVal) && ((pImgSrcPtr[lIndexSrc] - pImgSrcPtr[lIndexSrc+lStepOffsetY]) > nThresVal))) {
				x = lIndexDstRow + j - prcSrc->left + prcDst->left;
				pImgDstPtr[x] = 255;
				plBlobPixelAddress[lBlobPixelNo++] = x;
			}
		}
		for(j=__max(nMinX, pcsRightRange[i].cx);j<__min(nMaxX, pcsRightRange[i].cy);j++) {
			lIndexSrc = lIndexSrcRow+j;
			if((((pImgSrcPtr[lIndexSrc] - pImgSrcPtr[lIndexSrc-lStepOffsetX]) > nThresVal) && ((pImgSrcPtr[lIndexSrc] - pImgSrcPtr[lIndexSrc+lStepOffsetX]) > nThresVal)) ||
			   (((pImgSrcPtr[lIndexSrc] - pImgSrcPtr[lIndexSrc-lStepOffsetY]) > nThresVal) && ((pImgSrcPtr[lIndexSrc] - pImgSrcPtr[lIndexSrc+lStepOffsetY]) > nThresVal))) {
				x = lIndexDstRow + j - prcSrc->left + prcDst->left;
				pImgDstPtr[x] = 255;
				plBlobPixelAddress[lBlobPixelNo++] = x;
			}
		}
	}
	*plBlobPixelNo = lBlobPixelNo;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CRingRange::BinariseGradImgAndAddress");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

CProjection::CProjection()
{
	m_ProjRslt.lMaxValue = -1;
	m_ProjRslt.lMinValue = -1;
	m_ProjRslt.nProjSize = 0;
}

CProjection::~CProjection()
{
}

int CProjection::GetProj(IMAGEVIEW_INFO *pView,
						 CImageBuffer *pImgProj,
						 PROJ_PARM *pParm,
						 long lDebugFlag)
{
	int nError;

	m_ProjRslt.Clean();
	m_ProjSlantRslt.ProjRslt.Clean();
	if(CheckRect(&pParm->rectROI,&pView->sizeImage))
		return -1;
	if (pParm->nOrientation==ROW) {
		m_ProjRslt.nProjSize = pParm->rectROI.Height();
		m_ProjRslt.plProjVal = new long[abs(m_ProjRslt.nProjSize)];
		nError = GetYGrayAccl(pImgProj,&pParm->rectROI,&m_ProjRslt.plProjVal[0]);
		m_ProjRslt.lMagnify = pParm->rectROI.Width();
		if(nError)
			return -1;
	}
	else {
		m_ProjRslt.nProjSize = pParm->rectROI.Width();
		m_ProjRslt.plProjVal = new long[abs(m_ProjRslt.nProjSize)];
		nError = GetXGrayAccl(pImgProj,&pParm->rectROI,&m_ProjRslt.plProjVal[0]);
		m_ProjRslt.lMagnify = pParm->rectROI.Height();
		if(nError)
			return -1;
	}
	GetMaxMinValue();
	m_ProjRslt.nFirstId = 0;
	m_ProjRslt.nLastId = m_ProjRslt.nProjSize;
	m_ProjRslt.dPOSEdgeShift = 0;
	m_ProjRslt.dNEGEdgeShift = 0;

	if(lDebugFlag&DEBUG_PRINT_EXT && (lDebugFlag&DEBUG_EDGE||lDebugFlag&DEBUG_BLOB))
		OutputDebugLog("GetProj()->GetMaxMinValue()->Max:%ld, Min:%ld",m_ProjRslt.lMaxValue,m_ProjRslt.lMinValue);

	if(lDebugFlag&DEBUG_PRINT_EXT && (lDebugFlag&DEBUG_EDGE||lDebugFlag&DEBUG_BLOB)) {
		for (int i=0;i<m_ProjRslt.nProjSize;i++)
			OutputDebugLog("GetProj()->pProjValue[%d]::%ld",i,m_ProjRslt.plProjVal[i]);
	}

	if(lDebugFlag&DEBUG_DRAW && (lDebugFlag&DEBUG_EDGE||lDebugFlag&DEBUG_BLOB))
		DrawProj(pView,pParm,&m_ProjRslt);

	return 0;
}

int CProjection::GetSlantProj(IMAGEVIEW_INFO *pView,
							  CImageBuffer *pImgProj,
							  PROJ_SLANT_PARM *pParm,
							  long lDebugFlag)
{
	int i,j;
	double dStartX,dStartY;
	double x,y;
	CPoint2D pt2DCorner[4];
	BYTE* pImgPtr;
	BYTE byteVal0, byteVal1, byteVal2, byteVal3;
	BYTE byteWeightX,byteWeightY;
	BYTE byteWeightX1,byteWeightY1;
	long lPitch;
	long lIndex;
	double dProjDis;
	double dAbsScanWidth;
	CPoint2D pt2DIntercept;
	CSize ImgSize;
	CLine2D ln2DPerpendicular;

	m_ProjRslt.Clean();
	m_ProjSlantRslt.ProjRslt.Clean();
	if(pParm->ln2DScan.IsInValid()) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	ImgSize = pImgProj->GetImgSize();
	dAbsScanWidth = pParm->pt2DScanStart.GetDistance(pParm->pt2DScanEnd);
	m_ProjSlantRslt.nAbsScanWidth = (int)(dAbsScanWidth + 0.5);
	if(m_ProjSlantRslt.nAbsScanWidth == 0) {
		m_ProjSlantRslt.ProjRslt.nProjSize = 0;
		return 0;
	}
	if(dAbsScanWidth == 0) {
		m_ProjSlantRslt.ProjRslt.nProjSize = 0;
		return 0;
	}
	m_ProjSlantRslt.dScanWidthStepX = (pParm->pt2DScanEnd.x - pParm->pt2DScanStart.x) / dAbsScanWidth;
	m_ProjSlantRslt.dScanWidthStepY = (pParm->pt2DScanEnd.y - pParm->pt2DScanStart.y) / dAbsScanWidth;

	pParm->ln2DScan.GetPerpendicularLine(&pParm->pt2DProj,&ln2DPerpendicular);
	pParm->ln2DScan.GetIntercept(&ln2DPerpendicular,&pt2DIntercept);
	dProjDis = pt2DIntercept.GetDistance(pParm->pt2DProj);
	m_ProjSlantRslt.nAbsScanHeight = (int)(fabs(dProjDis) + 1.5);
	if(m_ProjSlantRslt.nAbsScanHeight == 1) {
		m_ProjSlantRslt.dScanHeightStepX = 0;
		m_ProjSlantRslt.dScanHeightStepY = 0;
	}
	else {
		m_ProjSlantRslt.dScanHeightStepX = (pParm->pt2DProj.x - pt2DIntercept.x) / dProjDis;
		m_ProjSlantRslt.dScanHeightStepY = (pParm->pt2DProj.y - pt2DIntercept.y) / dProjDis;
	}
	pt2DCorner[0] = pParm->pt2DScanStart;
	pt2DCorner[1] = pParm->pt2DScanStart + CPoint2D(m_ProjSlantRslt.nAbsScanWidth * m_ProjSlantRslt.dScanWidthStepX,
					m_ProjSlantRslt.nAbsScanWidth * m_ProjSlantRslt.dScanWidthStepY);
	pt2DCorner[2] = pParm->pt2DScanStart + CPoint2D(m_ProjSlantRslt.nAbsScanHeight * m_ProjSlantRslt.dScanHeightStepX,
					m_ProjSlantRslt.nAbsScanHeight * m_ProjSlantRslt.dScanHeightStepY);
	pt2DCorner[3] = pt2DCorner[1] + pt2DCorner[2] - pt2DCorner[0];
	for(i = 0; i < 4; i++) 
		if(ValidatePoint(&pt2DCorner[i],&ImgSize)) {
			return -1;
		}

	m_ProjSlantRslt.ProjRslt.nProjSize = m_ProjSlantRslt.nAbsScanWidth;
	m_ProjSlantRslt.ProjRslt.plProjVal = new long[abs(m_ProjSlantRslt.ProjRslt.nProjSize)];
	pImgPtr = pImgProj->GetImgPtr();
	lPitch = pImgProj->GetPitch();

	dStartX = pParm->pt2DScanStart.x; 			
	dStartY = pParm->pt2DScanStart.y; 
	if(pParm->bAccurate == 0) {
		m_ProjSlantRslt.ProjRslt.lMagnify = m_ProjSlantRslt.nAbsScanHeight;
		if(m_ProjSlantRslt.nAbsScanHeight == 1) {
			for(i = 0; i < m_ProjSlantRslt.nAbsScanWidth; i++) {
				lIndex = Round(dStartY)* lPitch + Round(dStartX);	
				m_ProjSlantRslt.ProjRslt.plProjVal[i] = pImgPtr[lIndex];
				dStartX += m_ProjSlantRslt.dScanWidthStepX;
				dStartY += m_ProjSlantRslt.dScanWidthStepY;
			}
		}
		else {
			long* plTab;
			plTab = new long[abs(m_ProjSlantRslt.nAbsScanHeight)];
			double dOffsetX, dOffsetY;
			memset(m_ProjSlantRslt.ProjRslt.plProjVal,0,m_ProjSlantRslt.nAbsScanWidth * sizeof(long));
			dOffsetX = 0;
			dOffsetY = 0;
			for(j = 0; j < m_ProjSlantRslt.nAbsScanHeight; j++) {
				plTab[j] = Round(dOffsetY) * lPitch + Round(dOffsetX);
				dOffsetX += m_ProjSlantRslt.dScanHeightStepX;
				dOffsetY += m_ProjSlantRslt.dScanHeightStepY;
			}
			for(i = 0; i < m_ProjSlantRslt.nAbsScanWidth; i++) {
				lIndex = Round(dStartY)* lPitch + Round(dStartX);	
				for(j = 0; j < m_ProjSlantRslt.nAbsScanHeight; j++) {
					m_ProjSlantRslt.ProjRslt.plProjVal[i] += pImgPtr[lIndex + plTab[j]];
				}
				dStartX += m_ProjSlantRslt.dScanWidthStepX;
				dStartY += m_ProjSlantRslt.dScanWidthStepY;
			}
			delete [] plTab;
		}
	}
	else {
		m_ProjSlantRslt.ProjRslt.lMagnify = m_ProjSlantRslt.nAbsScanHeight * 10000;
		if(m_ProjSlantRslt.nAbsScanHeight == 1) {
			for(i = 0; i < m_ProjSlantRslt.nAbsScanWidth; i++) {
				lIndex = (int)(dStartY) * lPitch + (int) (dStartX);	
				byteVal0 = pImgPtr[lIndex];
				byteVal1 = pImgPtr[lIndex + 1];
				byteVal2 = pImgPtr[lIndex + lPitch];
				byteVal3 = pImgPtr[lIndex + lPitch + 1];
				byteWeightX1 = (BYTE)((dStartX - (int)dStartX) * 100);
				byteWeightY1 = (BYTE)((dStartY - (int)dStartY) * 100);
				byteWeightX = 100 - byteWeightX1;
				byteWeightY = 100 - byteWeightY1;
				m_ProjSlantRslt.ProjRslt.plProjVal[i] = (byteWeightX * byteVal0 + byteWeightX1 * byteVal1) * byteWeightY +
										(byteWeightX * byteVal2 + byteWeightX1 * byteVal3) *byteWeightY1;
				dStartX += m_ProjSlantRslt.dScanWidthStepX;
				dStartY += m_ProjSlantRslt.dScanWidthStepY;
			}
		}
		else {
			memset(m_ProjSlantRslt.ProjRslt.plProjVal,0,m_ProjSlantRslt.nAbsScanWidth * sizeof(long));
			for(i = 0; i < m_ProjSlantRslt.nAbsScanWidth; i++) {
				x = dStartX;
				y = dStartY;
				for(j = 0; j < m_ProjSlantRslt.nAbsScanHeight; j++) {
					lIndex = ((int)y) * lPitch + (int)(x);	
					byteVal0 = pImgPtr[lIndex];
					byteVal1 = pImgPtr[lIndex + 1];
					byteVal2 = pImgPtr[lIndex + lPitch];
					byteVal3 = pImgPtr[lIndex + lPitch + 1];
					byteWeightX1 = (BYTE)((x - (int)x) * 100);
					byteWeightY1 = (BYTE)((y - (int)y) * 100);
					byteWeightX = 100 - byteWeightX1;
					byteWeightY = 100 - byteWeightY1;
					m_ProjSlantRslt.ProjRslt.plProjVal[i] += (byteWeightX * byteVal0 + byteWeightX1 * byteVal1) * byteWeightY +
											(byteWeightX * byteVal2 + byteWeightX1 * byteVal3) * byteWeightY1;
					x += m_ProjSlantRslt.dScanHeightStepX;
					y += m_ProjSlantRslt.dScanHeightStepY;
				}
				dStartX += m_ProjSlantRslt.dScanWidthStepX;
				dStartY += m_ProjSlantRslt.dScanWidthStepY;
			}
		}
	}

	m_ProjSlantRslt.ProjRslt.lMaxValue = m_ProjSlantRslt.ProjRslt.plProjVal[0];
	m_ProjSlantRslt.ProjRslt.lMinValue = m_ProjSlantRslt.ProjRslt.plProjVal[0];
	for (i=1;i<m_ProjSlantRslt.ProjRslt.nProjSize;i++) {
		if (m_ProjSlantRslt.ProjRslt.lMinValue > m_ProjSlantRslt.ProjRslt.plProjVal[i])
			m_ProjSlantRslt.ProjRslt.lMinValue = m_ProjSlantRslt.ProjRslt.plProjVal[i];
		if (m_ProjSlantRslt.ProjRslt.lMaxValue < m_ProjSlantRslt.ProjRslt.plProjVal[i])
			m_ProjSlantRslt.ProjRslt.lMaxValue = m_ProjSlantRslt.ProjRslt.plProjVal[i];
	}
	m_ProjSlantRslt.ProjRslt.nProjSize = m_ProjSlantRslt.nAbsScanWidth;
	m_ProjSlantRslt.ProjRslt.nFirstId = 0;
	m_ProjSlantRslt.ProjRslt.nLastId = m_ProjSlantRslt.ProjRslt.nProjSize;
	m_ProjSlantRslt.ProjRslt.dPOSEdgeShift = 0;
	m_ProjSlantRslt.ProjRslt.dNEGEdgeShift = 0;
	if(lDebugFlag&DEBUG_DRAW && (lDebugFlag&DEBUG_EDGE||lDebugFlag&DEBUG_BLOB)) {
		if(DrawSlantProj(pView,pParm,&m_ProjSlantRslt.ProjRslt))
			goto ErrorAbort;
	}
	return 0;
ErrorAbort:
	return -1;
}

int CProjection::GetSlantMaxProj(IMAGEVIEW_INFO *pView,
							  CImageBuffer *pImgProj,
							  PROJ_SLANT_PARM *pParm,
							  long lDebugFlag)
{
	int i,j;
	double dStartX,dStartY;
	CPoint2D pt2DCorner[4];
	BYTE* pImgPtr;
	long lPitch;
	long lIndex;
	double dProjDis;
	double dAbsScanWidth;
	CPoint2D pt2DIntercept;
	CSize ImgSize;
	CLine2D ln2DPerpendicular;

	m_ProjRslt.Clean();
	m_ProjSlantRslt.ProjRslt.Clean();
	if(pParm->ln2DScan.IsInValid()) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	ImgSize = pImgProj->GetImgSize();
	dAbsScanWidth = pParm->pt2DScanStart.GetDistance(pParm->pt2DScanEnd);
	m_ProjSlantRslt.nAbsScanWidth = (int)(dAbsScanWidth + 0.5);
	if(m_ProjSlantRslt.nAbsScanWidth == 0) {
		m_ProjSlantRslt.ProjRslt.nProjSize = 0;
		return 0;
	}
	if(dAbsScanWidth == 0) {
		m_ProjSlantRslt.ProjRslt.nProjSize = 0;
		return 0;
	}
	m_ProjSlantRslt.dScanWidthStepX = (pParm->pt2DScanEnd.x - pParm->pt2DScanStart.x) / dAbsScanWidth;
	m_ProjSlantRslt.dScanWidthStepY = (pParm->pt2DScanEnd.y - pParm->pt2DScanStart.y) / dAbsScanWidth;

	pParm->ln2DScan.GetPerpendicularLine(&pParm->pt2DProj,&ln2DPerpendicular);
	pParm->ln2DScan.GetIntercept(&ln2DPerpendicular,&pt2DIntercept);
	dProjDis = pt2DIntercept.GetDistance(pParm->pt2DProj);
	m_ProjSlantRslt.nAbsScanHeight = (int)(fabs(dProjDis) + 1.5);
	if(m_ProjSlantRslt.nAbsScanHeight == 1) {
		m_ProjSlantRslt.dScanHeightStepX = 0;
		m_ProjSlantRslt.dScanHeightStepY = 0;
	}
	else {
		m_ProjSlantRslt.dScanHeightStepX = (pParm->pt2DProj.x - pt2DIntercept.x) / dProjDis;
		m_ProjSlantRslt.dScanHeightStepY = (pParm->pt2DProj.y - pt2DIntercept.y) / dProjDis;
	}
	pt2DCorner[0] = pParm->pt2DScanStart;
	pt2DCorner[1] = pParm->pt2DScanStart + CPoint2D(m_ProjSlantRslt.nAbsScanWidth * m_ProjSlantRslt.dScanWidthStepX,
					m_ProjSlantRslt.nAbsScanWidth * m_ProjSlantRslt.dScanWidthStepY);
	pt2DCorner[2] = pParm->pt2DScanStart + CPoint2D(m_ProjSlantRslt.nAbsScanHeight * m_ProjSlantRslt.dScanHeightStepX,
					m_ProjSlantRslt.nAbsScanHeight * m_ProjSlantRslt.dScanHeightStepY);
	pt2DCorner[3] = pt2DCorner[1] + pt2DCorner[2] - pt2DCorner[0];
	for(i = 0; i < 4; i++) 
		if(ValidatePoint(&pt2DCorner[i],&ImgSize)) {
			return -1;
		}

	m_ProjSlantRslt.ProjRslt.nProjSize = m_ProjSlantRslt.nAbsScanWidth;
	m_ProjSlantRslt.ProjRslt.plProjVal = new long[abs(m_ProjSlantRslt.ProjRslt.nProjSize)];
	pImgPtr = pImgProj->GetImgPtr();
	lPitch = pImgProj->GetPitch();

	dStartX = pParm->pt2DScanStart.x; 			
	dStartY = pParm->pt2DScanStart.y; 
	m_ProjSlantRslt.ProjRslt.lMagnify = 1;
	if(m_ProjSlantRslt.nAbsScanHeight == 1) {
		for(i = 0; i < m_ProjSlantRslt.nAbsScanWidth; i++) {
			lIndex = Round(dStartY)* lPitch + Round(dStartX);	
			m_ProjSlantRslt.ProjRslt.plProjVal[i] = pImgPtr[lIndex];
			dStartX += m_ProjSlantRslt.dScanWidthStepX;
			dStartY += m_ProjSlantRslt.dScanWidthStepY;
		}
	}
	else {
		long* plTab;
		plTab = new long[abs(m_ProjSlantRslt.nAbsScanHeight)];
		double dOffsetX, dOffsetY;
		memset(m_ProjSlantRslt.ProjRslt.plProjVal,0,m_ProjSlantRslt.nAbsScanWidth * sizeof(long));
		dOffsetX = 0;
		dOffsetY = 0;
		for(j = 0; j < m_ProjSlantRslt.nAbsScanHeight; j++) {
			plTab[j] = Round(dOffsetY) * lPitch + Round(dOffsetX);
			dOffsetX += m_ProjSlantRslt.dScanHeightStepX;
			dOffsetY += m_ProjSlantRslt.dScanHeightStepY;
		}
		for(i = 0; i < m_ProjSlantRslt.nAbsScanWidth; i++) {
			lIndex = Round(dStartY)* lPitch + Round(dStartX);	
			for(j = 0; j < m_ProjSlantRslt.nAbsScanHeight; j++) {
				if(m_ProjSlantRslt.ProjRslt.plProjVal[i] < pImgPtr[lIndex + plTab[j]])
					m_ProjSlantRslt.ProjRslt.plProjVal[i] = pImgPtr[lIndex + plTab[j]];
			}
			dStartX += m_ProjSlantRslt.dScanWidthStepX;
			dStartY += m_ProjSlantRslt.dScanWidthStepY;
		}
		delete [] plTab;
	}

	m_ProjSlantRslt.ProjRslt.lMaxValue = m_ProjSlantRslt.ProjRslt.plProjVal[0];
	m_ProjSlantRslt.ProjRslt.lMinValue = m_ProjSlantRslt.ProjRslt.plProjVal[0];
	for (i=1;i<m_ProjSlantRslt.ProjRslt.nProjSize;i++) {
		if (m_ProjSlantRslt.ProjRslt.lMinValue > m_ProjSlantRslt.ProjRslt.plProjVal[i])
			m_ProjSlantRslt.ProjRslt.lMinValue = m_ProjSlantRslt.ProjRslt.plProjVal[i];
		if (m_ProjSlantRslt.ProjRslt.lMaxValue < m_ProjSlantRslt.ProjRslt.plProjVal[i])
			m_ProjSlantRslt.ProjRslt.lMaxValue = m_ProjSlantRslt.ProjRslt.plProjVal[i];
	}
	m_ProjSlantRslt.ProjRslt.nProjSize = m_ProjSlantRslt.nAbsScanWidth;
	m_ProjSlantRslt.ProjRslt.nFirstId = 0;
	m_ProjSlantRslt.ProjRslt.nLastId = m_ProjSlantRslt.ProjRslt.nProjSize;
	m_ProjSlantRslt.ProjRslt.dPOSEdgeShift = 0;
	m_ProjSlantRslt.ProjRslt.dNEGEdgeShift = 0;
	if(lDebugFlag&DEBUG_DRAW && (lDebugFlag&DEBUG_EDGE||lDebugFlag&DEBUG_BLOB)) {
		if(DrawSlantProj(pView,pParm,&m_ProjSlantRslt.ProjRslt))
			goto ErrorAbort;
	}
	return 0;
ErrorAbort:
	return -1;
}


int CProjection::GetSlantMinProj(IMAGEVIEW_INFO *pView,
							  CImageBuffer *pImgProj,
							  PROJ_SLANT_PARM *pParm,
							  long lDebugFlag)
{
	int i,j;
	double dStartX,dStartY;
	CPoint2D pt2DCorner[4];
	BYTE* pImgPtr;
	long lPitch;
	long lIndex;
	double dProjDis;
	double dAbsScanWidth;
	CPoint2D pt2DIntercept;
	CSize ImgSize;
	CLine2D ln2DPerpendicular;

	m_ProjRslt.Clean();
	m_ProjSlantRslt.ProjRslt.Clean();
	if(pParm->ln2DScan.IsInValid()) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	ImgSize = pImgProj->GetImgSize();
	dAbsScanWidth = pParm->pt2DScanStart.GetDistance(pParm->pt2DScanEnd);
	m_ProjSlantRslt.nAbsScanWidth = (int)(dAbsScanWidth + 0.5);
	if(m_ProjSlantRslt.nAbsScanWidth == 0) {
		m_ProjSlantRslt.ProjRslt.nProjSize = 0;
		return 0;
	}
	if(dAbsScanWidth == 0) {
		m_ProjSlantRslt.ProjRslt.nProjSize = 0;
		return 0;
	}
	m_ProjSlantRslt.dScanWidthStepX = (pParm->pt2DScanEnd.x - pParm->pt2DScanStart.x) / dAbsScanWidth;
	m_ProjSlantRslt.dScanWidthStepY = (pParm->pt2DScanEnd.y - pParm->pt2DScanStart.y) / dAbsScanWidth;

	pParm->ln2DScan.GetPerpendicularLine(&pParm->pt2DProj,&ln2DPerpendicular);
	pParm->ln2DScan.GetIntercept(&ln2DPerpendicular,&pt2DIntercept);
	dProjDis = pt2DIntercept.GetDistance(pParm->pt2DProj);
	m_ProjSlantRslt.nAbsScanHeight = (int)(fabs(dProjDis) + 1.5);
	if(m_ProjSlantRslt.nAbsScanHeight == 1) {
		m_ProjSlantRslt.dScanHeightStepX = 0;
		m_ProjSlantRslt.dScanHeightStepY = 0;
	}
	else {
		m_ProjSlantRslt.dScanHeightStepX = (pParm->pt2DProj.x - pt2DIntercept.x) / dProjDis;
		m_ProjSlantRslt.dScanHeightStepY = (pParm->pt2DProj.y - pt2DIntercept.y) / dProjDis;
	}
	pt2DCorner[0] = pParm->pt2DScanStart;
	pt2DCorner[1] = pParm->pt2DScanStart + CPoint2D(m_ProjSlantRslt.nAbsScanWidth * m_ProjSlantRslt.dScanWidthStepX,
					m_ProjSlantRslt.nAbsScanWidth * m_ProjSlantRslt.dScanWidthStepY);
	pt2DCorner[2] = pParm->pt2DScanStart + CPoint2D(m_ProjSlantRslt.nAbsScanHeight * m_ProjSlantRslt.dScanHeightStepX,
					m_ProjSlantRslt.nAbsScanHeight * m_ProjSlantRslt.dScanHeightStepY);
	pt2DCorner[3] = pt2DCorner[1] + pt2DCorner[2] - pt2DCorner[0];
	for(i = 0; i < 4; i++) 
		if(ValidatePoint(&pt2DCorner[i],&ImgSize)) {
			return -1;
		}

	m_ProjSlantRslt.ProjRslt.nProjSize = m_ProjSlantRslt.nAbsScanWidth;
	m_ProjSlantRslt.ProjRslt.plProjVal = new long[abs(m_ProjSlantRslt.ProjRslt.nProjSize)];
	pImgPtr = pImgProj->GetImgPtr();
	lPitch = pImgProj->GetPitch();

	dStartX = pParm->pt2DScanStart.x; 			
	dStartY = pParm->pt2DScanStart.y; 
	m_ProjSlantRslt.ProjRslt.lMagnify = 1;
	if(m_ProjSlantRslt.nAbsScanHeight == 1) {
		for(i = 0; i < m_ProjSlantRslt.nAbsScanWidth; i++) {
			lIndex = Round(dStartY)* lPitch + Round(dStartX);	
			m_ProjSlantRslt.ProjRslt.plProjVal[i] = pImgPtr[lIndex];
			dStartX += m_ProjSlantRslt.dScanWidthStepX;
			dStartY += m_ProjSlantRslt.dScanWidthStepY;
		}
	}
	else {
		long* plTab;
		plTab = new long[abs(m_ProjSlantRslt.nAbsScanHeight)];
		double dOffsetX, dOffsetY;
		for(j = 0; j < m_ProjSlantRslt.nAbsScanWidth; j++) 
			m_ProjSlantRslt.ProjRslt.plProjVal[j] = 255;
		dOffsetX = 0;
		dOffsetY = 0;
		for(j = 0; j < m_ProjSlantRslt.nAbsScanHeight; j++) {
			plTab[j] = Round(dOffsetY) * lPitch + Round(dOffsetX);
			dOffsetX += m_ProjSlantRslt.dScanHeightStepX;
			dOffsetY += m_ProjSlantRslt.dScanHeightStepY;
		}
		for(i = 0; i < m_ProjSlantRslt.nAbsScanWidth; i++) {
			lIndex = Round(dStartY)* lPitch + Round(dStartX);	
			for(j = 0; j < m_ProjSlantRslt.nAbsScanHeight; j++) {
				if(m_ProjSlantRslt.ProjRslt.plProjVal[i] > pImgPtr[lIndex + plTab[j]])
					m_ProjSlantRslt.ProjRslt.plProjVal[i] = pImgPtr[lIndex + plTab[j]];
			}
			dStartX += m_ProjSlantRslt.dScanWidthStepX;
			dStartY += m_ProjSlantRslt.dScanWidthStepY;
		}
		delete [] plTab;
	}

	m_ProjSlantRslt.ProjRslt.lMaxValue = m_ProjSlantRslt.ProjRslt.plProjVal[0];
	m_ProjSlantRslt.ProjRslt.lMinValue = m_ProjSlantRslt.ProjRslt.plProjVal[0];
	for (i=1;i<m_ProjSlantRslt.ProjRslt.nProjSize;i++) {
		if (m_ProjSlantRslt.ProjRslt.lMinValue > m_ProjSlantRslt.ProjRslt.plProjVal[i])
			m_ProjSlantRslt.ProjRslt.lMinValue = m_ProjSlantRslt.ProjRslt.plProjVal[i];
		if (m_ProjSlantRslt.ProjRslt.lMaxValue < m_ProjSlantRslt.ProjRslt.plProjVal[i])
			m_ProjSlantRslt.ProjRslt.lMaxValue = m_ProjSlantRslt.ProjRslt.plProjVal[i];
	}
	m_ProjSlantRslt.ProjRslt.nProjSize = m_ProjSlantRslt.nAbsScanWidth;
	m_ProjSlantRslt.ProjRslt.nFirstId = 0;
	m_ProjSlantRslt.ProjRslt.nLastId = m_ProjSlantRslt.ProjRslt.nProjSize;
	m_ProjSlantRslt.ProjRslt.dPOSEdgeShift = 0;
	m_ProjSlantRslt.ProjRslt.dNEGEdgeShift = 0;
	if(lDebugFlag&DEBUG_DRAW && (lDebugFlag&DEBUG_EDGE||lDebugFlag&DEBUG_BLOB)) {
		if(DrawSlantProj(pView,pParm,&m_ProjSlantRslt.ProjRslt))
			goto ErrorAbort;
	}
	return 0;
ErrorAbort:
	return -1;
}

int CProjection::GetMaxMinValue()
{
	int i;

	m_ProjRslt.lMaxValue = m_ProjRslt.plProjVal[0];
	m_ProjRslt.lMinValue = m_ProjRslt.plProjVal[0];

	for (i=1;i<m_ProjRslt.nProjSize;i++) {
		if (m_ProjRslt.lMinValue > m_ProjRslt.plProjVal[i])
			m_ProjRslt.lMinValue = m_ProjRslt.plProjVal[i];

		if (m_ProjRslt.lMaxValue < m_ProjRslt.plProjVal[i])
			m_ProjRslt.lMaxValue = m_ProjRslt.plProjVal[i];
	}
	return 0;
}

int CProjection::DrawProj(IMAGEVIEW_INFO *pView,
								PROJ_PARM *pParm,
								PROJ_RSLT *pRslt)
{
	long lRange;
	int nPos;
	int i,k;
	int nProjWinHt;
	CPoint pt1,pt2;
	int nResponse;


	lRange = pRslt->lMaxValue - pRslt->lMinValue;
	if(lRange <= 0)
		return -1;

	LOGPEN PenInfo;
	PenInfo.lopnColor = RGB(0,255,0);
	PenInfo.lopnStyle = PS_SOLID;
	PenInfo.lopnWidth.x = 1;

	DrwRect(pView,&pParm->rectROI,&PenInfo);

	if (pParm->nOrientation==COLUMN) {
		nProjWinHt = pParm->rectROI.Height();
		for (k=0,i=pParm->rectROI.left;i<pParm->rectROI.right,k<m_ProjRslt.nProjSize;i++,k++) {
			nPos = (int)(((double)(pRslt->plProjVal[k]-pRslt->lMinValue)/(double)lRange)*(double)nProjWinHt);
			pt1=CPoint(i,pParm->rectROI.bottom-nPos);
			pt2=CPoint(i,pParm->rectROI.bottom);
			DrwLine(pView,&pt1,&pt2,&PenInfo);
		}
	}
	else {
		nProjWinHt = pParm->rectROI.Width();
		for (k=0,i=pParm->rectROI.top;i<pParm->rectROI.bottom,k<m_ProjRslt.nProjSize;i++,k++) {
			nPos = (int)(((double)(pRslt->plProjVal[k]-pRslt->lMinValue)/(double)lRange)*(double)nProjWinHt);
			pt1=CPoint(pParm->rectROI.left+nPos,i);
			pt2=CPoint(pParm->rectROI.left,i);
			DrwLine(pView,&pt1,&pt2,&PenInfo);
		}
	}
	DrwUpdate(pView);
	nResponse = WaitResponse(pView);
	DrwDel(pView);
	if(nResponse) 
		return -1;
	return 0;
}

int CProjection::DrawSlantProj(IMAGEVIEW_INFO *pView,
								PROJ_SLANT_PARM *pParm,
								PROJ_RSLT *pRslt)
{
	long lRange;
	int nPos, nPos0, nPos1, nPos2;
	int i;
	double dProjWinHt;
	CPoint pt1,pt2;
	CLine2D ln2DPerpendicularStart;
	CLine2D ln2DPerpendicularEnd;
	CLine2D ln2DProj;
	CPoint2D pt2DProjStart;
	CPoint2D pt2DProjEnd;
	CPoint ptScanPt0, ptScanPt1;
	CPoint ptProjPt0, ptProjPt1;
	CSize2D cs2DScanStep;
	CSize2D cs2DProjStep;
	CPoint2D pt2D1, pt2D2;
	CPoint2D pt2DInt1, pt2DInt2;
	CPoint2D pt2DProj1,pt2DProj2;
	double dDis;
	int nResponse;

	if(pParm->pt2DScanEnd == pParm->pt2DScanStart) {
		return -1;
	}
	pParm->ln2DScan.GetParallelLine(&pParm->pt2DProj,&ln2DProj);
	pParm->ln2DScan.GetPerpendicularLine(&pParm->pt2DScanStart,&ln2DPerpendicularStart);
	pParm->ln2DScan.GetPerpendicularLine(&pParm->pt2DScanEnd,&ln2DPerpendicularEnd);
	ln2DPerpendicularStart.GetIntercept(&ln2DProj, &pt2DProjStart);
	ln2DPerpendicularEnd.GetIntercept(&ln2DProj, &pt2DProjEnd);
	
	cs2DScanStep.cx = (pParm->pt2DScanEnd.x - pParm->pt2DScanStart.x)/ pParm->pt2DScanEnd.GetDistance(pParm->pt2DScanStart);
	cs2DScanStep.cy = (pParm->pt2DScanEnd.y - pParm->pt2DScanStart.y)/ pParm->pt2DScanEnd.GetDistance(pParm->pt2DScanStart);
	cs2DProjStep.cx = (pt2DProjStart.x - pParm->pt2DScanStart.x)/ pt2DProjStart.GetDistance(pParm->pt2DScanStart);
	cs2DProjStep.cy = (pt2DProjStart.y - pParm->pt2DScanStart.y)/ pt2DProjStart.GetDistance(pParm->pt2DScanStart);

	ptScanPt0 = CPoint(Round(pParm->pt2DScanStart.x),Round(pParm->pt2DScanStart.y));
	ptScanPt1 = CPoint(Round(pParm->pt2DScanEnd.x),Round(pParm->pt2DScanEnd.y));
	ptProjPt0 = CPoint(Round(pt2DProjStart.x),Round(pt2DProjStart.y));
	ptProjPt1 = CPoint(Round(pt2DProjEnd.x),Round(pt2DProjEnd.y));

	lRange = pRslt->lMaxValue - pRslt->lMinValue;
	if(lRange <= 0)
		return -1;

	LOGPEN PenInfo;
	PenInfo.lopnColor = RGB(0,255,0);
	PenInfo.lopnStyle = PS_SOLID;
	PenInfo.lopnWidth.x = 1;

	DrwLine(pView,&ptScanPt0,&ptScanPt1,&PenInfo);
	DrwLine(pView,&ptScanPt0,&ptProjPt0,&PenInfo);
	DrwLine(pView,&ptProjPt0,&ptProjPt1,&PenInfo);
	DrwLine(pView,&ptProjPt1,&ptScanPt1,&PenInfo);

	if(ptProjPt0 == ptScanPt0) {
		return 0;
	}


	dProjWinHt = sqrt((double) ((ptScanPt0.x - ptProjPt0.x) * (ptScanPt0.x - ptProjPt0.x) + 
				(ptScanPt0.y - ptProjPt0.y) * (ptScanPt0.y - ptProjPt0.y)));

	nPos = (int)(((double)(pRslt->plProjVal[pRslt->nFirstId]-pRslt->lMinValue)/(double)lRange)*dProjWinHt);
	pt2D1 = CPoint2D(pParm->pt2DScanStart.x + pRslt->nFirstId * cs2DScanStep.cx, 
		pParm->pt2DScanStart.y + pRslt->nFirstId * cs2DScanStep.cy);
	pt2D2 = CPoint2D(pt2D1.x + nPos * cs2DProjStep.cx, pt2D1.y + nPos * cs2DProjStep.cy);
	pt1=CPoint((int)pt2D1.x, (int)pt2D1.y);
	pt2=CPoint((int)pt2D2.x, (int)pt2D2.y);
	DrwLine(pView,&pt1,&pt2,&PenInfo);
	
	for (i=pRslt->nFirstId + 1;i < (pRslt->nLastId - 1) ;i++) {
		nPos0 = (int)(((double)(pRslt->plProjVal[i - 1]-pRslt->lMinValue)/(double)lRange)*dProjWinHt);
		nPos1 = (int)(((double)(pRslt->plProjVal[i]-pRslt->lMinValue)/(double)lRange)*dProjWinHt);
		nPos2 = (int)(((double)(pRslt->plProjVal[i + 1]-pRslt->lMinValue)/(double)lRange)*dProjWinHt);

		pt2D1 = CPoint2D(pParm->pt2DScanStart.x + i * cs2DScanStep.cx, pParm->pt2DScanStart.y + i * cs2DScanStep.cy);
		pt1=CPoint((int)pt2D1.x, (int)pt2D1.y);
		pt2DInt1 = pt1;
		pParm->ln2DScan.GetProjection(&pt2DInt1, &pt2DProj1);
		dDis = pt2D1.GetDistance(pt2DProj1);
		nPos = (int)(dDis * nPos0 + (1 - dDis) * nPos1);
		pt2D2 = CPoint2D(pt2D1.x + nPos * cs2DProjStep.cx, pt2D1.y + nPos * cs2DProjStep.cy);
		pt2=CPoint((int)pt2D2.x, (int)pt2D2.y);
		DrwLine(pView,&pt1,&pt2,&PenInfo);

		pt1=CPoint((int)(pt2D1.x + 1), (int)(pt2D1.y + 1));
		pt2DInt1 = pt1;
		pParm->ln2DScan.GetProjection(&pt2DInt1, &pt2DProj1);
		dDis = pt2D1.GetDistance(pt2DProj1);
		nPos = (int)(dDis * nPos2 + (1 - dDis) * nPos1);
		pt2D2 = CPoint2D(pt2D1.x + nPos * cs2DProjStep.cx, pt2D1.y + nPos * cs2DProjStep.cy);
		pt2=CPoint((int)pt2D2.x, (int)pt2D2.y);
		DrwLine(pView,&pt1,&pt2,&PenInfo);
	}

	nPos = (int)(((double)(pRslt->plProjVal[(pRslt->nLastId - 1)]-pRslt->lMinValue)/(double)lRange)*dProjWinHt);
	pt2D1 = CPoint2D(pParm->pt2DScanStart.x + (pRslt->nLastId - 1) * cs2DScanStep.cx, 
		pParm->pt2DScanStart.y + (pRslt->nLastId - 1) * cs2DScanStep.cy);
	pt2D2 = CPoint2D(pt2D1.x + nPos * cs2DProjStep.cx, pt2D1.y + nPos * cs2DProjStep.cy);
	pt1=CPoint((int)pt2D1.x, (int)pt2D1.y);
	pt2=CPoint((int)pt2D2.x, (int)pt2D2.y);
	DrwLine(pView,&pt1,&pt2,&PenInfo);
	DrwUpdate(pView);
	nResponse = WaitResponse(pView);
	DrwDel(pView);
	if(nResponse) 
		return -1;
	return 0;
}

void CProjection::CopyProjRslt(PROJ_RSLT* pProjRsltSrc,
							  PROJ_RSLT* pProjRsltDst)
{
	int i;
	pProjRsltDst->Clean();
	pProjRsltDst->nProjSize = pProjRsltSrc->nProjSize;
	pProjRsltDst->lMaxValue = pProjRsltSrc->lMaxValue;
	pProjRsltDst->lMinValue = pProjRsltSrc->lMinValue;
	pProjRsltDst->nFirstId = pProjRsltSrc->nFirstId;
	pProjRsltDst->nLastId = pProjRsltSrc->nLastId;
	pProjRsltDst->dPOSEdgeShift = pProjRsltSrc->dPOSEdgeShift;
	pProjRsltDst->dNEGEdgeShift = pProjRsltSrc->dNEGEdgeShift;
	pProjRsltDst->lMagnify = pProjRsltSrc->lMagnify;
	pProjRsltDst->plProjVal = new long [abs(pProjRsltDst->nProjSize)];
	for(i = 0; i < pProjRsltDst->nProjSize ; i++)
		pProjRsltDst->plProjVal[i] = pProjRsltSrc->plProjVal[i];
}



CAdvBlobFind::CAdvBlobFind()
{
	plModifiedAddr = NULL;
	ptAryFndBlobContour = NULL;
}

CAdvBlobFind::~CAdvBlobFind()
{
	if(plModifiedAddr != NULL)
		delete [] plModifiedAddr;
	Clean();
}

void CAdvBlobFind::Clean()
{
	deleteArray(ptAryFndBlobContour);
	ptAryFndBlobContour = NULL;
}

int CAdvBlobFind::CheckBlobByMinRectSize(IMAGEVIEW_INFO* pView,
									  CImageBuffer *pImgBin,
									  CRect *prcBin,
									  int nBlobType,
									  CSize *pszBlobHoleMin,									
									  CSize *pszBlobMin,
									  CPoint* pptFound,
									  BOOL* pbFound,
									  long lDebugFlag)
{
	CRect rcDefectZoom;
	CImageBuffer ImgDefectZoom, ImgDefectZoomErosion;
	double dMask[10];
	CSize csErosion;
	CSize csDilation;
	CSize csDefectCheck;
	int nError;
	csErosion = CSize((pszBlobMin->cx + pszBlobHoleMin->cx)*2, 
					  (pszBlobMin->cy + pszBlobHoleMin->cy)*2);
	csDefectCheck = CSize((prcBin->Width()+pszBlobHoleMin->cx)*4 + 5, (prcBin->Height()+pszBlobHoleMin->cy)*4 + 5);
	rcDefectZoom = CRect(CPoint(0,0), csDefectCheck);
	ImgDefectZoom.Allocate(AbsSize(rcDefectZoom.Size()));
	ImgDefectZoomErosion.Allocate(AbsSize(rcDefectZoom.Size()));
	if(nBlobType == WHITE) 
		ImgDefectZoom.ClrImg(0);
	else
		ImgDefectZoom.ClrImg(255);
	nError = ZoomImg(pImgBin,
					prcBin,
					&ImgDefectZoom,
					&CPoint(pszBlobHoleMin->cx*2 + 2, pszBlobHoleMin->cy*2 + 2),
					&CSize(4,4));
	if(nError)
		goto ErrorAbort;
	csDilation = CSize(pszBlobHoleMin->cx * 2 + 1,
					   pszBlobHoleMin->cy * 2 + 1);
	nError = ImgDilation(&ImgDefectZoom, 
					   &rcDefectZoom, 
					   nBlobType, 
					   &ImgDefectZoomErosion,
					   &rcDefectZoom, 
					   &dMask[0], 
					   &csDilation,
					   0);
	if(nError)
		goto ErrorAbort;
	nError = ImgErosion(&ImgDefectZoomErosion, 
					   &rcDefectZoom, 
					   nBlobType, 
					   &ImgDefectZoom,
					   &rcDefectZoom, 
					   &dMask[0], 
					   &csErosion,
					   0);
	if(nError)
		goto ErrorAbort;

	if(nBlobType == WHITE) 
		nError = GetImgVal255Pos(&ImgDefectZoom, &rcDefectZoom, pptFound, *pbFound);
	else
		nError = GetImgVal0Pos(&ImgDefectZoom, &rcDefectZoom, pptFound, *pbFound);
	if(nError)
		goto ErrorAbort;
	*pptFound = prcBin->TopLeft() + CPoint(pptFound->x/4, pptFound->y/4);
	return 0;
ErrorAbort:
	return -1;
}

int CAdvBlobFind::CheckBlobByMinSqr(CImageBuffer* pImgSrc,
								  CRect* prcSrc,
								  CSize* pcsMinSqr,
								  int nHoleArea,
								  int nBlobType,
								  CArray<CRect,CRect>* pAryRcFound,
			     				  BOOL* pbDefectFound)
{
	CImageBuffer ImgSrcInflate;
	CRect rcSrcRelative;
	CRect rcSrcInflate;
	CRect rcSrcInflateRelative;
	int nError;

	CRect rcProj;
	pAryRcFound->RemoveAll();
	rcProj  =CRect(prcSrc->left,prcSrc->top,prcSrc->right-pcsMinSqr->cx+1,prcSrc->bottom-pcsMinSqr->cy+1);
	if((rcProj.Width() <= 0) || (rcProj.Height() <= 0)) {
		rcSrcRelative = CRect(CPoint(0,0), prcSrc->Size());
		rcSrcInflateRelative = rcSrcRelative;
		rcSrcInflateRelative.InflateRect(0,0,pcsMinSqr->cx,pcsMinSqr->cy);
		ImgSrcInflate.Allocate(AbsSize(rcSrcInflateRelative.Size()));
		if(nBlobType == WHITE)
			ImgSrcInflate.ClrImg(0);
		else
			ImgSrcInflate.ClrImg(255);
		nError = ImgCopy(pImgSrc,
						 prcSrc,
						 &ImgSrcInflate,
						 &rcSrcRelative);
		nError = CheckBlobByRectPercentage(&ImgSrcInflate,
										  &rcSrcInflateRelative,
										  pcsMinSqr,
										  nHoleArea,
										  nBlobType,
										  pAryRcFound,
			     						  pbDefectFound);
	}
	else {
		nError = CheckBlobByRectPercentage(pImgSrc,
										  prcSrc,
										  pcsMinSqr,
										  nHoleArea,
										  nBlobType,
										  pAryRcFound,
			     						  pbDefectFound);
	}
	return 0;

}

int CAdvBlobFind::CheckBlobByRectPercentage(CImageBuffer* pImgSrc,
										  CRect* prcSrc,
										  CSize* pcsMinSqr,
										  int nHoleArea,
										  int nBlobType,
										  CArray<CRect,CRect>* pAryRcFound,
			     						  BOOL* pbDefectFound)

{
	int i, j;
	int nImgPitchSrc;
	int nWidthSrc, nHeightSrc;
	BYTE *pImgSrcPtr;
	int *pProjVPtr;
	CRect rcFirstProj;
	long lImgSrcLostRowIndex;
	long lImgSrcAddRowIndex;
	long lProjVRowIndex;
	CRect rcProjV;
	long *plXAry;
	long *plYAry;
	long lProjRowIndex;
	int nError;
	int nProjHeight,nProjWidth,nProjPitch;
	int nProjVWidth,nProjVPitch,nProjVHeight;
	int* pnProj;
	int *pIntPtr,*pRowptr,*pColptr;
	long lRejectVal;
	CRect* prcProj;
	CRect rcProj;
	CSize* pcsProj;
	CRect rcSrcInflate;
	CImageBuffer ImgDefect;
	CPoint ptFound;
	ADV_BLOB_FIND_PARM AdvBlobParm;
	CAdvBlobFind BlobFinder;
	int nPkgDiagonalDist;
	int nMaxArea;
	long lProcMode;
	IMAGEVIEW_INFO VSITemp;
	CRect rcMemLoc;
	CRect rcDefect;
	CRect rcProc;
	long* plBlobPixelAddress;
	long lBlobPixelNo;
	int nDefectImgPitch;
	int nx, ny;

	*pbDefectFound = FALSE;
	plXAry = NULL;
	plYAry = NULL;
	pnProj = NULL;
	pProjVPtr = NULL;
	plBlobPixelAddress = NULL;
	rcMemLoc = CRect(CPoint(0,0), CSize(prcSrc->right+2, prcSrc->bottom+2));
	rcProc = *prcSrc;
	ImgDefect.Allocate(AbsSize(rcMemLoc.Size()));
	ImgDefect.ClrImg(0);
	nDefectImgPitch = ImgDefect.GetPitch();
	plBlobPixelAddress = new long[abs(rcMemLoc.Width() * rcMemLoc.Height())];
	pcsProj = pcsMinSqr;
	rcProj  =CRect(prcSrc->left,prcSrc->top,prcSrc->right-pcsProj->cx+1,prcSrc->bottom-pcsProj->cy+1);
	prcProj = &rcProj;
	if((prcProj->Width() <= 0) || (prcProj->Height() <= 0))
		goto PASS;
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	if((pcsProj->cx<=0) || (pcsProj->cy<=0)) {
		return -1;
	}

	*pbDefectFound = FALSE;
	nProjHeight = prcProj->Height(); 
	nProjWidth = prcProj->Width();
	nProjWidth = nProjWidth % 4;
	if (nProjWidth > 0)
		nProjWidth = 4 - nProjWidth;
	nProjPitch = prcProj->Width() + nProjWidth; 
	pnProj = new int[abs(nProjHeight*nProjPitch)];
	memset((void*)pnProj,0,nProjHeight*nProjPitch*sizeof(int));
	nProjWidth = prcProj->Width();	
	rcProjV = CRect(CPoint(0,0), CSize(prcSrc->Width(), prcSrc->Height() - pcsProj->cy + 1));

	nProjVHeight = rcProjV.Height(); 
	nProjVWidth = rcProjV.Width();
	nProjVWidth = nProjVWidth % 4;
	if (nProjVWidth > 0)
		nProjVWidth = 4 - nProjVWidth;
	nProjVPitch = rcProjV.Width() + nProjVWidth; 
	pProjVPtr = new int[abs(nProjVHeight*nProjVPitch)];
	memset((void*)pProjVPtr,0,nProjVHeight*nProjVPitch*sizeof(int));

	nImgPitchSrc = pImgSrc->GetPitch();
	nWidthSrc = rcProjV.Width();
	nHeightSrc = rcProjV.Height();
	pImgSrcPtr = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;

	plXAry = new long[abs(prcSrc->Width())];
	rcFirstProj = CRect(prcSrc->TopLeft(), CSize(prcSrc->Width(), pcsProj->cy));
	nError = GetXGrayAccl(pImgSrc, &rcFirstProj, &plXAry[0]);
	for(i=0;i<nWidthSrc;i++)
		pProjVPtr[i] = plXAry[i];

	lImgSrcLostRowIndex = 0;
	lImgSrcAddRowIndex = nImgPitchSrc * (pcsProj->cy);
	lProjVRowIndex = nProjVPitch;
	for(i=1;i<nHeightSrc;i++) {
		for(j=0;j<nWidthSrc;j++) {
			pProjVPtr[lProjVRowIndex + j] = pProjVPtr[lProjVRowIndex - nProjVPitch + j] + 
					pImgSrcPtr[lImgSrcAddRowIndex + j] - pImgSrcPtr[lImgSrcLostRowIndex+j];	
		}
		lImgSrcLostRowIndex += nImgPitchSrc;
		lImgSrcAddRowIndex += nImgPitchSrc;
		lProjVRowIndex += nProjVPitch;
	}

	nWidthSrc = prcProj->Width();
	nHeightSrc = prcProj->Height();
	plYAry = new long[abs(nHeightSrc)];
	lProjRowIndex = 0;
	lProjVRowIndex = 0;
	for(i=0;i<nHeightSrc;i++) {
		plYAry[i] = 0;
		for(j=0;j<pcsProj->cx;j++) {
			plYAry[i] += pProjVPtr[lProjVRowIndex + j];
		}
		pnProj[lProjRowIndex] = plYAry[i];
		lProjVRowIndex += nProjVPitch;
		lProjRowIndex += nProjPitch;
	}
	lProjRowIndex = 0;
	lProjVRowIndex = 0;
	for(i=0;i<nHeightSrc;i++) {
		for(j=1;j<nWidthSrc;j++) {
			pnProj[lProjRowIndex+j] = pnProj[lProjRowIndex+j-1] + 
				pProjVPtr[lProjVRowIndex + j -1+ pcsProj->cx] - pProjVPtr[lProjVRowIndex + j -1];
		}
		lProjRowIndex += nProjPitch;
		lProjVRowIndex += nProjVPitch;
	}
	pColptr = pRowptr = pIntPtr = pnProj;
	lBlobPixelNo = 0;
	if(nBlobType==BLACK){
		lRejectVal = 255 * nHoleArea;
		for(i = 0,pRowptr =pIntPtr;i<nProjHeight ;i++,pRowptr+=nProjPitch){
			for(j = 0, pColptr=pRowptr;j<nProjWidth;j++,pColptr++){
				if(*pColptr<=lRejectVal){
					*pbDefectFound = TRUE;
					nx = j+prcSrc->left + 1;
					ny = i+prcSrc->top + 1;
					ImgDefect.SetPixelVal(nx, ny, 255);
					plBlobPixelAddress[lBlobPixelNo++] = ny * nDefectImgPitch + nx;
				}
			}
		}
	}
	else{
		lRejectVal = (pcsProj->cx * pcsProj->cy - nHoleArea) * 255;
		if(lRejectVal > 0) {
			for(i = 0,pRowptr =pIntPtr;i<nProjHeight ;i++,pRowptr+=nProjPitch){
				for(j = 0, pColptr=pRowptr;j<nProjWidth;j++,pColptr++){
					if(*pColptr>=lRejectVal){
						ptFound = CPoint(j+prcSrc->left, i+prcSrc->top);
						nx = j+prcSrc->left + 1;
						ny = i+prcSrc->top + 1;
						ImgDefect.SetPixelVal(nx, ny, 255);
						plBlobPixelAddress[lBlobPixelNo++] = ny * nDefectImgPitch + nx;
						*pbDefectFound = TRUE;
					}
				}
			}
		}
	}
	nPkgDiagonalDist = prcSrc->Width() + prcSrc->Height();
	nMaxArea = prcSrc->Width() * prcSrc->Height();
	AdvBlobParm.nBlobType = WHITE;
	AdvBlobParm.nMinBlobArea = 1;
	AdvBlobParm.nMaxBlobArea = nMaxArea;
	AdvBlobParm.nMaxYBlobSize = nPkgDiagonalDist;
	AdvBlobParm.nMinYBlobSize = 1;
	AdvBlobParm.nMaxXBlobSize = nPkgDiagonalDist;
	AdvBlobParm.nMinXBlobSize = 1;
	AdvBlobParm.rcROISrc = rcProc;
	AdvBlobParm.rcROIDst = rcProc;
	lProcMode = 0;
	if(lBlobPixelNo > 0) {
		nError = BlobFinder.GetAdvBlob4(&VSITemp,
						&ImgDefect,
						&plBlobPixelAddress[0],
						lBlobPixelNo,
						&AdvBlobParm,
						lProcMode,
						0);
		for(i=0;i<BlobFinder.m_AdvBlobFindRslt.nFndBlobs;i++) {
			rcDefect = BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].rcFndBlobPos;
			rcDefect = CRect(rcDefect.left - 1,
							 rcDefect.top - 1,
							 rcDefect.right - 1 + pcsMinSqr->cx,
							 rcDefect.bottom - 1+ pcsMinSqr->cy);
			pAryRcFound->Add(rcDefect);
		}
	}
PASS:
	if(plXAry != NULL)
		delete [] plXAry;
	if(plYAry != NULL)
		delete [] plYAry;
	if(pnProj!=NULL)
		delete [] pnProj;
	if(pProjVPtr!=NULL)
		delete [] pProjVPtr;
	if(plBlobPixelAddress != NULL)
		delete [] plBlobPixelAddress;
	return 0;
}

int CAdvBlobFind::CheckBlobByMinSqr(CImageBuffer* pImgSrc,
								  CRect* prcSrc,
								  CSize* pcsMinSqr,
								  int nHoleArea,
								  int nBlobType,
								  CPoint* pptFound,
			     				  BOOL* pbDefectFound)
{
	CImageBuffer ImgSrcInflate;
	CRect rcSrcRelative;
	CRect rcSrcInflate;
	CRect rcSrcInflateRelative;
	int nError;

	CRect rcProj;
	rcProj  =CRect(prcSrc->left,prcSrc->top,prcSrc->right-pcsMinSqr->cx+1,prcSrc->bottom-pcsMinSqr->cy+1);
	if((rcProj.Width() <= 0) || (rcProj.Height() <= 0)) {
		rcSrcRelative = CRect(CPoint(0,0), prcSrc->Size());
		rcSrcInflateRelative = rcSrcRelative;
		rcSrcInflateRelative.InflateRect(0,0,pcsMinSqr->cx,pcsMinSqr->cy);
		ImgSrcInflate.Allocate(AbsSize(rcSrcInflateRelative.Size()));
		if(nBlobType == WHITE)
			ImgSrcInflate.ClrImg(0);
		else
			ImgSrcInflate.ClrImg(255);
		nError = ImgCopy(pImgSrc,
						 prcSrc,
						 &ImgSrcInflate,
						 &rcSrcRelative);
		nError = CheckBlobByRectPercentage(&ImgSrcInflate,
										  &rcSrcInflateRelative,
										  pcsMinSqr,
										  nHoleArea,
										  nBlobType,
										  pptFound,
			     						  pbDefectFound);
	}
	else {
		nError = CheckBlobByRectPercentage(pImgSrc,
										  prcSrc,
										  pcsMinSqr,
										  nHoleArea,
										  nBlobType,
										  pptFound,
			     						  pbDefectFound);
	}
	return 0;

}

int CAdvBlobFind::CheckBlobByRectPercentage(CImageBuffer* pImgSrc,
										  CRect* prcSrc,
										  CSize* pcsMinSqr,
										  int nHoleArea,
										  int nBlobType,
										  CPoint* pptFound,
			     						  BOOL* pbDefectFound)

{
	int i, j;
	int nImgPitchSrc;
	int nWidthSrc, nHeightSrc;
	BYTE *pImgSrcPtr;
	int *pProjVPtr;
	CRect rcFirstProj;
	long lImgSrcLostRowIndex;
	long lImgSrcAddRowIndex;
	long lProjVRowIndex;
	CRect rcProjV;
	long *plXAry;
	long *plYAry;
	long lProjRowIndex;
	int nError;
	int nProjHeight,nProjWidth,nProjPitch;
	int nProjVWidth,nProjVPitch,nProjVHeight;
	int* pnProj;
	int *pIntPtr,*pRowptr,*pColptr;
	long lRejectVal;
	CRect* prcProj;
	CRect rcProj;
	CSize* pcsProj;
	CRect rcSrcInflate;

	*pbDefectFound = FALSE;
	plXAry = NULL;
	plYAry = NULL;
	pnProj = NULL;
	pProjVPtr = NULL;
	pcsProj = pcsMinSqr;
	rcProj  =CRect(prcSrc->left,prcSrc->top,prcSrc->right-pcsProj->cx+1,prcSrc->bottom-pcsProj->cy+1);
	prcProj = &rcProj;
	if((prcProj->Width() <= 0) || (prcProj->Height() <= 0))
		goto PASS;
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	if((pcsProj->cx<=0) || (pcsProj->cy<=0)) {
		return -1;
	}

	*pbDefectFound = FALSE;
	nProjHeight = prcProj->Height(); 
	nProjWidth = prcProj->Width();
	nProjWidth = nProjWidth % 4;
	if (nProjWidth > 0)
		nProjWidth = 4 - nProjWidth;
	nProjPitch = prcProj->Width() + nProjWidth; 
	pnProj = new int[abs(nProjHeight*nProjPitch)];
	memset((void*)pnProj,0,nProjHeight*nProjPitch*sizeof(int));
	nProjWidth = prcProj->Width();	
	rcProjV = CRect(CPoint(0,0), CSize(prcSrc->Width(), prcSrc->Height() - pcsProj->cy + 1));

	nProjVHeight = rcProjV.Height(); 
	nProjVWidth = rcProjV.Width();
	nProjVWidth = nProjVWidth % 4;
	if (nProjVWidth > 0)
		nProjVWidth = 4 - nProjVWidth;
	nProjVPitch = rcProjV.Width() + nProjVWidth; 
	pProjVPtr = new int[abs(nProjVHeight*nProjVPitch)];
	memset((void*)pProjVPtr,0,nProjVHeight*nProjVPitch*sizeof(int));

	nImgPitchSrc = pImgSrc->GetPitch();
	nWidthSrc = rcProjV.Width();
	nHeightSrc = rcProjV.Height();
	pImgSrcPtr = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;

	plXAry = new long[abs(prcSrc->Width())];
	rcFirstProj = CRect(prcSrc->TopLeft(), CSize(prcSrc->Width(), pcsProj->cy));
	nError = GetXGrayAccl(pImgSrc, &rcFirstProj, &plXAry[0]);
	for(i=0;i<nWidthSrc;i++)
		pProjVPtr[i] = plXAry[i];

	lImgSrcLostRowIndex = 0;
	lImgSrcAddRowIndex = nImgPitchSrc * (pcsProj->cy);
	lProjVRowIndex = nProjVPitch;
	for(i=1;i<nHeightSrc;i++) {
		for(j=0;j<nWidthSrc;j++) {
			pProjVPtr[lProjVRowIndex + j] = pProjVPtr[lProjVRowIndex - nProjVPitch + j] + 
					pImgSrcPtr[lImgSrcAddRowIndex + j] - pImgSrcPtr[lImgSrcLostRowIndex+j];	
		}
		lImgSrcLostRowIndex += nImgPitchSrc;
		lImgSrcAddRowIndex += nImgPitchSrc;
		lProjVRowIndex += nProjVPitch;
	}

	nWidthSrc = prcProj->Width();
	nHeightSrc = prcProj->Height();
	plYAry = new long[abs(nHeightSrc)];
	lProjRowIndex = 0;
	lProjVRowIndex = 0;
	for(i=0;i<nHeightSrc;i++) {
		plYAry[i] = 0;
		for(j=0;j<pcsProj->cx;j++) {
			plYAry[i] += pProjVPtr[lProjVRowIndex + j];
		}
		pnProj[lProjRowIndex] = plYAry[i];
		lProjVRowIndex += nProjVPitch;
		lProjRowIndex += nProjPitch;
	}
	lProjRowIndex = 0;
	lProjVRowIndex = 0;
	for(i=0;i<nHeightSrc;i++) {
		for(j=1;j<nWidthSrc;j++) {
			pnProj[lProjRowIndex+j] = pnProj[lProjRowIndex+j-1] + 
				pProjVPtr[lProjVRowIndex + j -1+ pcsProj->cx] - pProjVPtr[lProjVRowIndex + j -1];
		}
		lProjRowIndex += nProjPitch;
		lProjVRowIndex += nProjVPitch;
	}
	pColptr = pRowptr = pIntPtr = pnProj;
	if(nBlobType==BLACK){
		lRejectVal = 255 * nHoleArea;
		for(i = 0,pRowptr =pIntPtr;i<nProjHeight ;i++,pRowptr+=nProjPitch){
			for(j = 0, pColptr=pRowptr;j<nProjWidth;j++,pColptr++){
				if(*pColptr<=lRejectVal){
					*pbDefectFound = TRUE;
					*pptFound = CPoint(j+prcSrc->left, i+prcSrc->top);
					goto PASS;
				}
			}
		}
	}
	else{
		lRejectVal = (pcsProj->cx * pcsProj->cy - nHoleArea) * 255;
		if(lRejectVal > 0) {
			for(i = 0,pRowptr =pIntPtr;i<nProjHeight ;i++,pRowptr+=nProjPitch){
				for(j = 0, pColptr=pRowptr;j<nProjWidth;j++,pColptr++){
					if(*pColptr>=lRejectVal){
						*pptFound = CPoint(j+prcSrc->left, i+prcSrc->top);
						*pbDefectFound = TRUE;
						goto PASS;
					}
				}
			}
		}
	}
PASS:
	if(plXAry != NULL)
		delete [] plXAry;
	if(plYAry != NULL)
		delete [] plYAry;
	if(pnProj!=NULL)
		delete [] pnProj;
	if(pProjVPtr!=NULL)
		delete [] pProjVPtr;
	return 0;
}


int CAdvBlobFind::GetAdvBlobCombinedInfo(CRect* prcBlobCombined,
										 ADV_BLOB_INFO* pAdvBlobCombinedInfo)
{
	int i; 
	double dWidth;
	bool bFirst;
	pAdvBlobCombinedInfo->pt2DFndBlobCenter = CPoint2D(0,0);
	pAdvBlobCombinedInfo->rcFndBlobPos = CRect(0,0,0,0);
	pAdvBlobCombinedInfo->nFndBlobArea = 0;
	pAdvBlobCombinedInfo->pptBlobContour = NULL;
	pAdvBlobCombinedInfo->nBlobContourSize = 0;
	pAdvBlobCombinedInfo->dFndBlobLength = 0;
	pAdvBlobCombinedInfo->dFndBlobElongation = 0;
	bFirst = true;
	for(i = 0; i < m_AdvBlobFindRslt.nFndBlobs; i++) {
		if(RectInRect(*prcBlobCombined, m_AdvBlobFindRslt.pAdvBlobInfo[i].rcFndBlobPos)) {
			if(bFirst) {
				pAdvBlobCombinedInfo->nFndBlobArea = m_AdvBlobFindRslt.pAdvBlobInfo[i].nFndBlobArea;
				pAdvBlobCombinedInfo->rcFndBlobPos = m_AdvBlobFindRslt.pAdvBlobInfo[i].rcFndBlobPos;
				pAdvBlobCombinedInfo->dFndBlobLength = m_AdvBlobFindRslt.pAdvBlobInfo[i].dFndBlobLength;
				bFirst = false;
			}
			else {
				pAdvBlobCombinedInfo->nFndBlobArea += m_AdvBlobFindRslt.pAdvBlobInfo[i].nFndBlobArea;
				pAdvBlobCombinedInfo->rcFndBlobPos.UnionRect(pAdvBlobCombinedInfo->rcFndBlobPos, m_AdvBlobFindRslt.pAdvBlobInfo[i].rcFndBlobPos);
				pAdvBlobCombinedInfo->dFndBlobLength += m_AdvBlobFindRslt.pAdvBlobInfo[i].dFndBlobLength;
			}
		}
	}
	if(pAdvBlobCombinedInfo->dFndBlobLength > 0) {
		dWidth = pAdvBlobCombinedInfo->nFndBlobArea / pAdvBlobCombinedInfo->dFndBlobLength;
		if(dWidth < 1)
			dWidth = 1;
		pAdvBlobCombinedInfo->dFndBlobElongation = pAdvBlobCombinedInfo->dFndBlobLength / dWidth;
	}
	return 0;
}

void CAdvBlobFind::DrawBlobContour(IMAGEVIEW_INFO* pView,
								 int nSamplingX,
								 int nSamplingY,
								 CPoint* pptOffset) 
{
	int i, j;
	LOGPEN RedPenInfo;
	LOGPEN GreenPenInfo;
	CPoint ptContour1, ptContour2;
	RedPenInfo.lopnColor = RGB(255,0,0);
	RedPenInfo.lopnStyle = PS_SOLID;
	RedPenInfo.lopnWidth.x = 1;
	GreenPenInfo.lopnColor = RGB(0,255,0);
	GreenPenInfo.lopnStyle = PS_SOLID;
	GreenPenInfo.lopnWidth.x = 1;
	for(i = 0; i < m_AdvBlobFindRslt.nFndBlobs; i++) {
		if(m_AdvBlobFindRslt.pAdvBlobInfo[i].nBlobContourSize > 0) {
			ptContour1 = m_AdvBlobFindRslt.pAdvBlobInfo[i].pptBlobContour[0];
			ptContour1 = CPoint(ptContour1.x * nSamplingX, ptContour1.y * nSamplingY) + *pptOffset;
			DrwCur(pView,&ptContour1,&RedPenInfo,2);
			for(j = 1; j < m_AdvBlobFindRslt.pAdvBlobInfo[i].nBlobContourSize; j++) {
				ptContour2 = m_AdvBlobFindRslt.pAdvBlobInfo[i].pptBlobContour[j];
				ptContour2 = CPoint(ptContour2.x * nSamplingX, ptContour2.y * nSamplingY) + *pptOffset;
				DrwLine(pView,&ptContour1, &ptContour2,&GreenPenInfo);
				ptContour1 = ptContour2;
				DrwCur(pView,&ptContour2,&RedPenInfo,2);
			}
		}
	}
	DrwUpdate(pView);
}


// GetAdvBlob4 is under development, please do not use
int CAdvBlobFind::GetAdvBlob4(IMAGEVIEW_INFO *pView,
							  CImageBuffer *pImgSrc,
							  long* plBlobPixelAddress,
							  long lBlobPixelNo,
							  ADV_BLOB_FIND_PARM *pParm,
							  long lProcMode,
							  long lDebugFlag)
{
	long li;
	int x, y;
	BYTE *pImgProc, *pImgProcTopLine, *pImgProcBotLine, *pImgProcLeftLine, *pImgProcRightLine;
	BYTE *pImgProcRow, *pImgProcCol;
	BYTE *pImgProcPtr;
	CRect rcBlob;
	int nPitchSrc, nPitchProc;
	CPoint ptBlobStartPos;
	int nImgProcWidth, nImgProcHeight;
	CRect rcSrc;
	CPoint ptROITopLeft;
	ADV_BLOB_FIND_RSLT AdvBlobFindRsltBak;
	ADV_BLOB_FIND_RSLT* pAryAdvBlobBlock;
	int nAryAdvBlobBlock;
	int nImgProcHeightSub1, nImgProcWidthSub1;

	int nAryAdvBlobInfoRow;
	int nAryAdvBlobInfoCol;
	long lMemLocBlobNo;
	ADV_BLOB_INFO AdvBlobInfoMore;
	int nBlobFound;
	int nBlobCheck;
	bool bAbort;
	CRect rcImgSrc;
	long lBlobPixelIndex;

	Clean();
	if(lProcMode & FIND_BLOB_CONTOUR) 
		ptAryFndBlobContour = new CPoint[abs(pImgSrc->GetImgSize().cx * pImgSrc->GetImgSize().cy)];
	if(m_AdvBlobFindRslt.pAdvBlobInfo)
		m_AdvBlobFindRslt.Clean();

	rcImgSrc = pParm->rcROISrc;
	rcSrc = pParm->rcROISrc;
	ptROITopLeft = rcSrc.TopLeft();
	nPitchSrc = pImgSrc->GetPitch();
	nImgProcWidth = rcSrc.Width();
	nImgProcHeight = rcSrc.Height();

	bAbort = false;
	m_AdvBlobFindRslt.nFndBlobs = 0;
	m_lProcMode = lProcMode;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(&pParm->rcROISrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"pParm->rcROISrc is Invalid");
		return -1;
	}
	
	if((pParm->rcROISrc.Width() == 0) || (pParm->rcROISrc.Height()==0))
		return 0;

	plNonFilledAddr = new long[abs(nImgProcWidth * nImgProcHeight)];
	plFillSeedsAddr1 = new long[abs(nImgProcWidth * nImgProcHeight)];
	plFillSeedsAddr2 = new long[abs(nImgProcWidth * nImgProcHeight)];
	nAryAdvBlobBlock = MAX_BLOB / NORMAL_BLOB_SIZE + 1;
	pAryAdvBlobBlock = new ADV_BLOB_FIND_RSLT[abs(nAryAdvBlobBlock)];

	m_AdvBlobFindRslt.pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];

	nAryAdvBlobInfoRow = 0;
	nAryAdvBlobInfoCol = 0;
	lMemLocBlobNo = NORMAL_BLOB_SIZE;

	nPitchProc = pImgSrc->GetPitch();
	pImgProcPtr = pImgSrc->GetImgPtr();
	pImgProc = pImgSrc->GetImgPtr() + rcImgSrc.top * nPitchProc + rcImgSrc.left;

	nDir0 = 1;
	nDir1 = -(nPitchProc - 1);
	nDir2 = -nPitchProc;
	nDir3 = -(nPitchProc + 1);
	nDir4 = -1;
	nDir5 = nPitchProc - 1;
	nDir6 = nPitchProc;
	nDir7 = nPitchProc + 1;

	pnDir[0] = 1;
	pnDir[1] = -(nPitchProc - 1);
	pnDir[2] = -nPitchProc;
	pnDir[3] = -(nPitchProc + 1);
	pnDir[4] = -1;
	pnDir[5] = nPitchProc - 1;
	pnDir[6] = nPitchProc;
	pnDir[7] = nPitchProc + 1;
	bBlobFill = FALSE;
	if(pParm->nBlobType == WHITE) {
		pImgProcLeftLine=pImgProc;
		pImgProcRightLine=pImgProc + nImgProcWidth - 1;
		for(y = 0; y < nImgProcHeight; y++, pImgProcLeftLine += nPitchProc, pImgProcRightLine += nPitchProc) {
			*pImgProcLeftLine = 0;
			*pImgProcRightLine = 0;
		}
		pImgProcTopLine = pImgProc;
		pImgProcBotLine = pImgProc + nPitchProc * (nImgProcHeight - 1);
		for(x = 0; x < nImgProcWidth; x++) {
			*pImgProcTopLine++ = 0;
			*pImgProcBotLine++ = 0;
		}
		pImgProcRow = pImgProcCol = pImgProc + nPitchProc;
		nImgProcHeightSub1 = nImgProcHeight - 1;
		nImgProcWidthSub1 = nImgProcWidth - 1;
		for(li = 0; li < lBlobPixelNo; li++) {
			if(pImgProcPtr[plBlobPixelAddress[li]] == 255) {
				lBlobPixelIndex = plBlobPixelAddress[li];
				x = lBlobPixelIndex % nPitchSrc;
				y = lBlobPixelIndex / nPitchSrc;
				ptBlobStartPos = CPoint(x, y);
				if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE)
					nBlobFound = WhiteBlobContour4(pImgSrc,
										&ptROITopLeft,
										&ptBlobStartPos, 
										&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
										lProcMode);
				else {
					AdvBlobInfoMore.Clean();
					nBlobFound = WhiteBlobContour4(pImgSrc,
										&ptROITopLeft,
										&ptBlobStartPos, 
										&AdvBlobInfoMore,
										lProcMode);
				}
				if(nBlobFound == 0){
					if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE) {
						nBlobCheck = GetBlobInfo4(&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
									pImgSrc,
									&rcSrc,
									pParm,
									lProcMode,
									lDebugFlag);
						if(nBlobCheck == 0) {
							if(lProcMode & FIND_BLOB_CONTOUR) {
								if(m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour)
									delete m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].nBlobContourSize = nFndBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour = new CPoint[abs(nFndBlobContour)];
								for(int kk = 0; kk < nFndBlobContour; kk++)
									m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour[kk] = ptAryFndBlobContour[kk];
							}
							m_AdvBlobFindRslt.nFndBlobs++;
						}
						else
							m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].Clean();
					}
					else {
						nBlobCheck = GetBlobInfo4(&AdvBlobInfoMore,
									pImgSrc,
									&rcSrc,
									pParm,
									lProcMode,
									lDebugFlag);
						if(nBlobCheck == 0) {
							m_AdvBlobFindRslt.nFndBlobs++;
							if(lMemLocBlobNo >= m_AdvBlobFindRslt.nFndBlobs) {
								pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[nAryAdvBlobInfoCol] = &AdvBlobInfoMore;
								nAryAdvBlobInfoCol++;
							}
							else {
								if(m_AdvBlobFindRslt.nFndBlobs >= MAX_BLOB) {
									if ((DEBUG_DRAW&lDebugFlag) || (DEBUG_STEP_MODE&lDebugFlag)) 
										OutputDebugLog("Too many blobs=%d",MAX_BLOB);
									bAbort = true;
									goto Abort;
								}
								pAryAdvBlobBlock[nAryAdvBlobInfoRow].pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
								lMemLocBlobNo += NORMAL_BLOB_SIZE;
								nAryAdvBlobInfoRow++;
								pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[0] = &AdvBlobInfoMore;
								nAryAdvBlobInfoCol = 1;
							}
						}
						else
							AdvBlobInfoMore.Clean();
					}
				}
			}
		} 
	}
	else {
		pImgProcLeftLine=pImgProc;
		pImgProcRightLine=pImgProc + nImgProcWidth - 1;
		for(y = 0; y < nImgProcHeight; y++, pImgProcLeftLine += nPitchProc, pImgProcRightLine += nPitchProc) {
			*pImgProcLeftLine = 255;
			*pImgProcRightLine = 255;
		}
		pImgProcTopLine = pImgProc;
		pImgProcBotLine = pImgProc + nPitchProc * (nImgProcHeight - 1);
		for(x = 0; x < nImgProcWidth; x++) {
			*pImgProcTopLine++ = 255;
			*pImgProcBotLine++ = 255;
		}
		pImgProcRow = pImgProcCol = pImgProc + nPitchProc;
		nImgProcHeightSub1 = nImgProcHeight - 1;
		nImgProcWidthSub1 = nImgProcWidth - 1;
		for(li = 0; li < lBlobPixelNo; li++) {
			if(pImgProcPtr[plBlobPixelAddress[li]] == 0) {
				lBlobPixelIndex = plBlobPixelAddress[li];
				x = lBlobPixelIndex % nPitchSrc;
				y = lBlobPixelIndex / nPitchSrc;
				ptBlobStartPos = CPoint(x, y);
				if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE)
					nBlobFound = BlackBlobContour4(pImgSrc,
										&ptROITopLeft,
										&ptBlobStartPos, 
										&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
										lProcMode);
				else {
					AdvBlobInfoMore.Clean();
					nBlobFound = BlackBlobContour4(pImgSrc,
										&ptROITopLeft,
										&ptBlobStartPos, 
										&AdvBlobInfoMore,
										lProcMode);
				}
				if(nBlobFound == 0){
					if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE) {
						nBlobCheck = GetBlobInfo4(&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
									pImgSrc,
									&rcSrc,
									pParm,
									lProcMode,
									lDebugFlag);
						if(nBlobCheck == 0)
							m_AdvBlobFindRslt.nFndBlobs++;
						else
							m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].Clean();
					}
					else {
						nBlobCheck = GetBlobInfo4(&AdvBlobInfoMore,
									pImgSrc,
									&rcSrc,
									pParm,
									lProcMode,
									lDebugFlag);
						if(nBlobCheck == 0) {
							m_AdvBlobFindRslt.nFndBlobs++;
							if(lMemLocBlobNo >= m_AdvBlobFindRslt.nFndBlobs) {
								pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[nAryAdvBlobInfoCol] = &AdvBlobInfoMore;
								nAryAdvBlobInfoCol++;
							}
							else {
								if(m_AdvBlobFindRslt.nFndBlobs >= MAX_BLOB) {
									if ((DEBUG_DRAW&lDebugFlag) || (DEBUG_STEP_MODE&lDebugFlag)) 
										OutputDebugLog("Too many blobs=%d",MAX_BLOB);
									bAbort = true;
									goto Abort;
								}
								pAryAdvBlobBlock[nAryAdvBlobInfoRow].pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
								lMemLocBlobNo += NORMAL_BLOB_SIZE;
								nAryAdvBlobInfoRow++;
								pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[0] = &AdvBlobInfoMore;
								nAryAdvBlobInfoCol = 1;
							}
						}
						else
							AdvBlobInfoMore.Clean();
					}
				}
			}
		}
	}
// Restore Image 
	pImgProc = pImgSrc->GetImgPtr();// + rcImgSrc.top * nPitchProc + rcImgSrc.left;
	if(pParm->nBlobType == WHITE) {
		for(li=0; li<lBlobPixelNo;li++)
			pImgProc[plBlobPixelAddress[li]] = 255;
	}
	else {
		for(li=0; li<lBlobPixelNo;li++)
			pImgProc[plBlobPixelAddress[li]] = 0;
	}
// end of Restoring Image 

Abort:


	if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_BLOB)
		DrawAdvBlob(pView,pParm,&m_AdvBlobFindRslt);
	if(nAryAdvBlobInfoRow) {
		int k;
		int nFoundBlob;
		nFoundBlob = m_AdvBlobFindRslt.nFndBlobs;
		AdvBlobFindRsltBak.pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
		for(x = 0; x < NORMAL_BLOB_SIZE; x++)
			AdvBlobFindRsltBak.pAdvBlobInfo[x] = &m_AdvBlobFindRslt.pAdvBlobInfo[x];
		m_AdvBlobFindRslt.nFndBlobs = NORMAL_BLOB_SIZE;
		m_AdvBlobFindRslt.Clean();
		m_AdvBlobFindRslt.nFndBlobs = nFoundBlob;
		m_AdvBlobFindRslt.pAdvBlobInfo = new ADV_BLOB_INFO[abs(m_AdvBlobFindRslt.nFndBlobs)];
		k = 0;
		for(x = 0; x < NORMAL_BLOB_SIZE; x++)
			m_AdvBlobFindRslt.pAdvBlobInfo[k++] = &AdvBlobFindRsltBak.pAdvBlobInfo[x];

		for(y = 0; y < nAryAdvBlobInfoRow; y++) {
			for(x = 0; x < NORMAL_BLOB_SIZE; x++) {
				if(k < m_AdvBlobFindRslt.nFndBlobs)
					m_AdvBlobFindRslt.pAdvBlobInfo[k++] = &pAryAdvBlobBlock[y].pAdvBlobInfo[x];
				else
					break;
			}
			pAryAdvBlobBlock[y].nFndBlobs = NORMAL_BLOB_SIZE;
			pAryAdvBlobBlock[y].Clean();
			if(k >= m_AdvBlobFindRslt.nFndBlobs)
				break;
		}
	}
	delete [] pAryAdvBlobBlock;
	delete [] plNonFilledAddr;
	delete [] plFillSeedsAddr1;
	delete [] plFillSeedsAddr2;


	if(bAbort)
		return -1;
	else
		return 0;
}

int CAdvBlobFind::GetBlobInfo4(ADV_BLOB_INFO *pAdvBlobInfo,
							  CImageBuffer* pImgBufProc,
							  CRect* prcOrg,
							  ADV_BLOB_FIND_PARM *pParm,
							  long lProcMode,
							  long lDebugFlag)
{
	int m, n, k;
	long lm;
	long lIndex;
	CRect rcBlob;
	int nBlobFound;
	long lBlobArea;
	long lBlobFillRowIndex, lBlobFillIndex;
	BYTE* pImgProc;
	CPoint ptBlobStartPos;
	int nPitchProc;
	double dXMoment, dYMoment;
	int nBlobRoughCheck;
	pImgProc = pImgBufProc->GetImgPtr();
	nPitchProc = pImgBufProc->GetPitch();
	rcBlob = pAdvBlobInfo->rcFndBlobPos;
	lBlobFillRowIndex = rcBlob.top * nPitchProc;
	lNonFilledNo = 0;

	nBlobRoughCheck = FoundBlobRoughCheck(pParm, pAdvBlobInfo, lProcMode);
	if((nBlobRoughCheck == 0)&& ((lProcMode & FIND_BLOB_LENGTH) || (lProcMode & FIND_BLOB_ELONGATION))) {
		double dlength[6];
		int i;
		CPoint ptLftMost, ptTopMost, ptRhtMost, ptBotMost;
		lIndex = rcBlob.top * nPitchProc + rcBlob.left;
		for(i = rcBlob.top; i < rcBlob.bottom; i++) {
			if(pImgProc[lIndex] == 200) {
				ptLftMost = CPoint(rcBlob.left, i);
				break;
			}
			lIndex += nPitchProc;
		}
		lIndex = rcBlob.top * nPitchProc;
		for(i = rcBlob.left; i < rcBlob.right; i++) {
			if(pImgProc[lIndex + i] == 200) {
				ptTopMost = CPoint(i, rcBlob.top);
				break;
			}
		}
		lIndex = rcBlob.top * nPitchProc + rcBlob.right - 1;
		for(i = rcBlob.top; i < rcBlob.bottom; i++) {
			if(pImgProc[lIndex] == 200) {
				ptRhtMost = CPoint(rcBlob.right, i);
				break;
			}
			lIndex += nPitchProc;
		}
		lIndex = (rcBlob.bottom - 1) * nPitchProc;
		for(i = rcBlob.left; i < rcBlob.right; i++) {
			if(pImgProc[lIndex + i] == 200) {
				ptBotMost = CPoint(i, rcBlob.bottom);
				break;
			}
		}
		dlength[0] = sqrt((double) ((ptTopMost.x - ptLftMost.x) * (ptTopMost.x - ptLftMost.x) + (ptTopMost.y - ptLftMost.y) * (ptTopMost.y - ptLftMost.y)));
		dlength[1] = sqrt((double) ((ptTopMost.x - ptBotMost.x) * (ptTopMost.x - ptBotMost.x) + (ptTopMost.y - ptBotMost.y) * (ptTopMost.y - ptBotMost.y)));
		dlength[2] = sqrt((double) ((ptTopMost.x - ptRhtMost.x) * (ptTopMost.x - ptRhtMost.x) + (ptTopMost.y - ptRhtMost.y) * (ptTopMost.y - ptRhtMost.y)));
		dlength[3] = sqrt((double) ((ptLftMost.x - ptBotMost.x) * (ptLftMost.x - ptBotMost.x) + (ptLftMost.y - ptBotMost.y) * (ptLftMost.y - ptBotMost.y)));
		dlength[4] = sqrt((double) ((ptLftMost.x - ptRhtMost.x) * (ptLftMost.x - ptRhtMost.x) + (ptLftMost.y - ptRhtMost.y) * (ptLftMost.y - ptRhtMost.y)));
		dlength[5] = sqrt((double) ((ptBotMost.x - ptRhtMost.x) * (ptBotMost.x - ptRhtMost.x) + (ptBotMost.y - ptRhtMost.y) * (ptBotMost.y - ptRhtMost.y)));
		pAdvBlobInfo->dFndBlobLength = 0;
		for(i = 0; i < 6; i++) {
			if(dlength[i] > pAdvBlobInfo->dFndBlobLength) pAdvBlobInfo->dFndBlobLength = dlength[i];
		}
	}
	if(pParm->nBlobType == WHITE) {
		for(m = rcBlob.top; m < rcBlob.bottom; m++) {
			for(n = rcBlob.left;n < rcBlob.right; n++) {
				lBlobFillIndex = lBlobFillRowIndex + n;
				if(pImgProc[lBlobFillIndex] == 255) {
					if((pImgProc[lBlobFillIndex -1] == 200) || (pImgProc[lBlobFillIndex -nPitchProc] == 200)) 
						pImgProc[lBlobFillIndex] = 200;
					else
						plNonFilledAddr[lNonFilledNo++] = lBlobFillIndex;
				}
			}
			lBlobFillRowIndex += nPitchProc;
		}

		lFillSeedsNo1 = 0;
		for(lm = 0; lm < lNonFilledNo; lm++) {
			lIndex = plNonFilledAddr[lm];
			for(k = 0; k < 8; k++) {
				if(pImgProc[lIndex + pnDir[k]] == 200) {
					pImgProc[lIndex] = 200;
					plFillSeedsAddr1[lFillSeedsNo1++] = lIndex;
					break;
				}
			}
		}
		while(lFillSeedsNo1 > 0) {
			lFillSeedsNo2 = 0;
			for(lm = 0; lm < lFillSeedsNo1; lm++) {
				lIndex = plFillSeedsAddr1[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 255) {
						plFillSeedsAddr2[lFillSeedsNo2++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
			if(lFillSeedsNo2 == 0)
				break;
			lFillSeedsNo1 = 0;
			for(lm = 0; lm < lFillSeedsNo2; lm++) {
				lIndex = plFillSeedsAddr2[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 255) {
						plFillSeedsAddr1[lFillSeedsNo1++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
		}
		if(nBlobRoughCheck == -1) {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) 
						pImgProc[lBlobFillRowIndex + n] = 0;
				}
				lBlobFillRowIndex += nPitchProc;
			}
			return -1;
		}
		lBlobArea = 0;
		if(lProcMode & FIND_BLOB_CENTER) { 
			dXMoment = 0;
			dYMoment = 0;
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						dXMoment += n;
						dYMoment += m;
						lBlobArea++;
						pImgProc[lBlobFillRowIndex + n]=0;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
			pAdvBlobInfo->pt2DFndBlobCenter = CPoint2D( dXMoment / lBlobArea + prcOrg->left, dYMoment / lBlobArea + prcOrg->top);
		}
		else {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						lBlobArea++;
						pImgProc[lBlobFillRowIndex + n]=0;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
	}
	else {
		for(m = rcBlob.top; m < rcBlob.bottom; m++) {
			for(n = rcBlob.left;n < rcBlob.right; n++) {
				lBlobFillIndex = lBlobFillRowIndex + n;
				if(pImgProc[lBlobFillIndex] == 0) {
					if((pImgProc[lBlobFillIndex -1] == 200) || (pImgProc[lBlobFillIndex -nPitchProc] == 200)) 
						pImgProc[lBlobFillIndex] = 200;
					else
						plNonFilledAddr[lNonFilledNo++] = lBlobFillIndex;
				}
			}
			lBlobFillRowIndex += nPitchProc;
		}

		lFillSeedsNo1 = 0;
		for(lm = 0; lm < lNonFilledNo; lm++) {
			lIndex = plNonFilledAddr[lm];
			for(k = 0; k < 8; k++) {
				if(pImgProc[lIndex + pnDir[k]] == 200) {
					pImgProc[lIndex] = 200;
					plFillSeedsAddr1[lFillSeedsNo1++] = lIndex;
					break;
				}
			}
		}
		while(lFillSeedsNo1 > 0) {
			lFillSeedsNo2 = 0;
			for(lm = 0; lm < lFillSeedsNo1; lm++) {
				lIndex = plFillSeedsAddr1[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 0) {
						plFillSeedsAddr2[lFillSeedsNo2++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
			if(lFillSeedsNo2 == 0)
				break;
			lFillSeedsNo1 = 0;
			for(lm = 0; lm < lFillSeedsNo2; lm++) {
				lIndex = plFillSeedsAddr2[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 0) {
						plFillSeedsAddr1[lFillSeedsNo1++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
		}
		if(nBlobRoughCheck == -1) {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) 
						pImgProc[lBlobFillRowIndex + n] = 255;
				}
				lBlobFillRowIndex += nPitchProc;
			}
			return -1;
		}
		lBlobArea = 0;
		if(lProcMode & FIND_BLOB_CENTER) { 
			dXMoment = 0;
			dYMoment = 0;
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						dXMoment += n;
						dYMoment += m;
						lBlobArea++;
						pImgProc[lBlobFillRowIndex + n]=255;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
			pAdvBlobInfo->pt2DFndBlobCenter = CPoint2D( dXMoment / lBlobArea + prcOrg->left, dYMoment / lBlobArea + prcOrg->top);
		}
		else {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						lBlobArea++;
						pImgProc[lBlobFillRowIndex + n]=255;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
	}
	pAdvBlobInfo->nFndBlobArea = lBlobArea;
	if(lProcMode & FIND_BLOB_ELONGATION) {
		double dWidth;
		if(pAdvBlobInfo->dFndBlobLength) {
			dWidth = pAdvBlobInfo->nFndBlobArea / pAdvBlobInfo->dFndBlobLength;
			if(dWidth < 1) dWidth = 1;
			pAdvBlobInfo->dFndBlobElongation = pAdvBlobInfo->dFndBlobLength / dWidth;
		}
		else pAdvBlobInfo->dFndBlobElongation = 0;
	}
//	pAdvBlobInfo->rcFndBlobPos += CPoint(prcOrg->left, prcOrg->top);
	nBlobFound = FoundBlobCheck(pParm, pAdvBlobInfo, lProcMode);
	return nBlobFound;
}

int CAdvBlobFind::GetMaxBlobIndex(long lSortingMode, int* pMaxSortingIndex)
{
	int* ppSortingIndex;
	ppSortingIndex = NULL;
	*pMaxSortingIndex = -1;
	if(m_AdvBlobFindRslt.nFndBlobs <= 0)
		goto PASS;
	ppSortingIndex = new int[m_AdvBlobFindRslt.nFndBlobs];
	BlobSorting(lSortingMode, FALSE, ppSortingIndex);
	*pMaxSortingIndex = ppSortingIndex[0];
PASS:
	if(ppSortingIndex != NULL)
		delete [] ppSortingIndex;
	return 0;
}

int CAdvBlobFind::BlobSorting(long lSortingMode, BOOL bAscend, int* pSortingIndex)
{
	int i;
	double* pdBlobSortingVal;
	pdBlobSortingVal = NULL;
	if(m_AdvBlobFindRslt.nFndBlobs < 0)
		goto ErrorAbort;
	if(m_AdvBlobFindRslt.nFndBlobs == 1) {
		pSortingIndex[0] = 0;
		goto PASS;
	}
	pdBlobSortingVal = new double[abs(m_AdvBlobFindRslt.nFndBlobs)];
	switch (lSortingMode) {
		case SORTING_BLOB_BY_AREA :
			for(i=0;i<m_AdvBlobFindRslt.nFndBlobs;i++) 
				pdBlobSortingVal[i] = m_AdvBlobFindRslt.pAdvBlobInfo[i].nFndBlobArea;
			break;
		case SORTING_BLOB_BY_WIDTH :
			for(i=0;i<m_AdvBlobFindRslt.nFndBlobs;i++) 
				pdBlobSortingVal[i] = m_AdvBlobFindRslt.pAdvBlobInfo[i].rcFndBlobPos.Width();
			break;
		case SORTING_BLOB_BY_HEIGHT :
			for(i=0;i<m_AdvBlobFindRslt.nFndBlobs;i++) 
				pdBlobSortingVal[i] = m_AdvBlobFindRslt.pAdvBlobInfo[i].rcFndBlobPos.Height();
			break;
		case SORTING_BLOB_BY_LENGTH :
			if((m_lProcMode & FIND_BLOB_LENGTH) == 0)
				goto ErrorAbort;
			for(i=0;i<m_AdvBlobFindRslt.nFndBlobs;i++) 
				pdBlobSortingVal[i] = m_AdvBlobFindRslt.pAdvBlobInfo[i].dFndBlobLength;
			break;
		case SORTING_BLOB_BY_ELONGATION :
			if((m_lProcMode & FIND_BLOB_ELONGATION) == 0)
				goto ErrorAbort;
			for(i=0;i<m_AdvBlobFindRslt.nFndBlobs;i++) 
				pdBlobSortingVal[i] = m_AdvBlobFindRslt.pAdvBlobInfo[i].dFndBlobElongation;
			break;
	}
	Sorting(&pdBlobSortingVal[0], m_AdvBlobFindRslt.nFndBlobs, &pSortingIndex[0], bAscend); 
PASS:
	if(pdBlobSortingVal != NULL)
		delete [] pdBlobSortingVal;
	return 0;
ErrorAbort:
	if(pdBlobSortingVal != NULL)
		delete [] pdBlobSortingVal;
	return -1;
}



int CAdvBlobFind::ConnectionRoughCheck(IMAGEVIEW_INFO *pView,
									ADV_BLOB_FIND_PARM* pAdvBlobRoughCheckParm, 
									CRect* prcRoughCheck,
									int* pnRoughCheck,
									CSize* pcsConnection,
									long lProcMode,
									long lDebugFlag)
{
	CAdvBlobFind AdvBlobFind;
	CRect* prcRect;
	int i, j;
	int nIndex;
	ADV_BLOB_INFO AdvBlobInfoMerged;
	CRectMerge RectMerge;
	int nFndBlobs;
	int nBlobFoundRslt;
	int nRoughCheck;

	prcRect = NULL;
	nBlobFoundRslt = 0;
	nRoughCheck = 0;
	if(m_AdvBlobFindRslt.nFndBlobs < 1)
		goto PASS;
	nFndBlobs = m_AdvBlobFindRslt.nFndBlobs;
	prcRect = new CRect[abs(nFndBlobs)];
	for(i = 0; i < nFndBlobs; i++) {	
		prcRect[i] = m_AdvBlobFindRslt.pAdvBlobInfo[i].rcFndBlobPos;
		prcRect[i].InflateRect(pcsConnection->cx, 
							   pcsConnection->cy,
							   pcsConnection->cx, 
							   pcsConnection->cy);
		CheckRect(&prcRect[i], &pView->sizeImage);
	}
	RectMerge.Merge(&prcRect[0], nFndBlobs, &pView->sizeImage);
	if((lProcMode & FIND_BLOB_LENGTH) || (lProcMode & FIND_BLOB_ELONGATION)) {
		for(i=0; i<RectMerge.nFoundMerged;i++) {
			AdvBlobInfoMerged.rcFndBlobPos = RectMerge.prcFoundMerged[i];
			AdvBlobInfoMerged.nFndBlobArea = 0;
			AdvBlobInfoMerged.dFndBlobLength = 0;
			for(j=0;j<RectMerge.pMergedRect[i].nMergedLabel;j++) {
				nIndex = RectMerge.pMergedRect[i].pnMergedLabel[j];
				if((nIndex >= 0) && ((nIndex <m_AdvBlobFindRslt.nFndBlobs))) {
					AdvBlobInfoMerged.nFndBlobArea += m_AdvBlobFindRslt.pAdvBlobInfo[nIndex].nFndBlobArea;
					AdvBlobInfoMerged.dFndBlobLength += m_AdvBlobFindRslt.pAdvBlobInfo[nIndex].dFndBlobLength;
				}
			}
			if(AdvBlobInfoMerged.dFndBlobLength > 0) {
				double dWidthMean;
				dWidthMean = ((double)AdvBlobInfoMerged.nFndBlobArea) / AdvBlobInfoMerged.dFndBlobLength;
				if(dWidthMean < 1)
					dWidthMean = 1;

				AdvBlobInfoMerged.dFndBlobElongation = AdvBlobInfoMerged.dFndBlobLength / dWidthMean;	
			}
			nBlobFoundRslt = AdvBlobFind.FoundBlobCheck(pAdvBlobRoughCheckParm, &AdvBlobInfoMerged, lProcMode);
			if(nBlobFoundRslt == 0) {
				prcRoughCheck[nRoughCheck++] = AdvBlobInfoMerged.rcFndBlobPos;
			}
		}
	}
	else {
		for(i=0; i<RectMerge.nFoundMerged;i++) {
			AdvBlobInfoMerged.rcFndBlobPos = RectMerge.prcFoundMerged[i];
			AdvBlobInfoMerged.nFndBlobArea = 0;
			for(j=0;j<RectMerge.pMergedRect[i].nMergedLabel;j++) {
				nIndex = RectMerge.pMergedRect[i].pnMergedLabel[j];
				if((nIndex >= 0) && ((nIndex <m_AdvBlobFindRslt.nFndBlobs))) 
					AdvBlobInfoMerged.nFndBlobArea += m_AdvBlobFindRslt.pAdvBlobInfo[nIndex].nFndBlobArea;
			}
			nBlobFoundRslt = AdvBlobFind.FoundBlobCheck(pAdvBlobRoughCheckParm, &AdvBlobInfoMerged, lProcMode);
			if(nBlobFoundRslt == 0) {
				prcRoughCheck[nRoughCheck++] = AdvBlobInfoMerged.rcFndBlobPos;
			}
		}
	}
PASS:
	*pnRoughCheck = nRoughCheck;
	if(prcRect != NULL)
		delete [] prcRect;
	return 0;
}

int CAdvBlobFind::GetAdvBlob3(IMAGEVIEW_INFO *pView,
							  CImageBuffer *pImgSrc,
							  ADV_BLOB_FIND_PARM *pParm,
							  long lProcMode,
							  long lDebugFlag)
{
	int x, y;
	//BYTE *pImgProcTopLine, *pImgProcBotLine;
	BYTE *pImgProc, *pImgProcLeftLine, *pImgProcRightLine;
	BYTE *pImgProcRow, *pImgProcCol;
	CRect rcBlob;
	int nPitchSrc, nPitchProc;
	CImageBuffer ImgProc;
	CPoint ptBlobStartPos;
	int nImgProcWidth, nImgProcHeight;
	CRect rcSrc;
	CRect rcImgProc;
	CPoint ptROITopLeft;
	ADV_BLOB_FIND_RSLT AdvBlobFindRsltBak;
	ADV_BLOB_FIND_RSLT* pAryAdvBlobBlock;
	int nAryAdvBlobBlock;

	int nAryAdvBlobInfoRow;
	int nAryAdvBlobInfoCol;
	long lMemLocBlobNo;
	ADV_BLOB_INFO AdvBlobInfoMore;
	int nBlobFound;
	int nBlobCheck;
	bool bAbort;


	Clean();
	if(m_AdvBlobFindRslt.pAdvBlobInfo)
		m_AdvBlobFindRslt.Clean();
	bAbort = false;
	m_AdvBlobFindRslt.nFndBlobs = 0;
	m_lProcMode = lProcMode;
	rcSrc = pParm->rcROISrc;
	ptROITopLeft = rcSrc.TopLeft();
	if(lProcMode & FIND_BLOB_CONTOUR) 
		createArray(ptAryFndBlobContour, pImgSrc->GetImgSize().cx * pImgSrc->GetImgSize().cy);

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(&pParm->rcROISrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"pParm->rcROISrc is Invalid");
		return -1;
	}
	if((pParm->rcROISrc.Width() == 0) || (pParm->rcROISrc.Height()==0))
		return 0;

	nAryAdvBlobBlock = MAX_BLOB / NORMAL_BLOB_SIZE + 1;
	pAryAdvBlobBlock = new ADV_BLOB_FIND_RSLT[abs(nAryAdvBlobBlock)];

	m_AdvBlobFindRslt.pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];

	nAryAdvBlobInfoRow = 0;
	nAryAdvBlobInfoCol = 0;
	lMemLocBlobNo = NORMAL_BLOB_SIZE;
	nPitchSrc = pImgSrc->GetPitch();
	nImgProcWidth = rcSrc.Width();
	nImgProcHeight = rcSrc.Height();

	ImgProc.Allocate(AbsSize(rcSrc.Size()));
	nPitchProc = ImgProc.GetPitch();

	pImgProc = ImgProc.GetImgPtr();
	rcImgProc = CRect(CPoint(0,0), pParm->rcROISrc.Size());
	plNonFilledAddr = new long[abs(nImgProcHeight * nImgProcWidth)];
	plFillSeedsAddr1 = new long[abs(nImgProcHeight * nImgProcWidth)];
	plFillSeedsAddr2 = new long[abs(nImgProcHeight * nImgProcWidth)];

	pnDir[0] = 1;
	pnDir[1] = -(nPitchProc - 1);
	pnDir[2] = -nPitchProc;
	pnDir[3] = -(nPitchProc + 1);
	pnDir[4] = -1;
	pnDir[5] = nPitchProc - 1;
	pnDir[6] = nPitchProc;
	pnDir[7] = nPitchProc + 1;

	ImgCopy(pImgSrc, &pParm->rcROISrc, &ImgProc, &rcImgProc);

	if(pParm->nBlobType == WHITE) {
		pImgProcLeftLine=pImgProc;
		pImgProcRightLine=pImgProc + nImgProcWidth - 1;
		for(y = 0; y < nImgProcHeight; y++, pImgProcLeftLine += nPitchProc, pImgProcRightLine += nPitchProc) {
			*pImgProcLeftLine = 0;
			*pImgProcRightLine = 0;
		}
		memset(pImgProc, 0, nImgProcWidth);
		memset(pImgProc + nPitchProc * (nImgProcHeight - 1), 0, nImgProcWidth);
		//pImgProcTopLine = pImgProc;
		//pImgProcBotLine = pImgProc + nPitchProc * (nImgProcHeight - 1);
		//for(x = 0; x < nImgProcWidth; x++) {
		//	*pImgProcTopLine++ = 0;
		//	*pImgProcBotLine++ = 0;
		//}
		pImgProcRow = pImgProcCol = pImgProc;
		for(y = 0; y < nImgProcHeight; y++, pImgProcRow += nPitchProc) {
			for(x = 0, pImgProcCol = pImgProcRow; x < nImgProcWidth; x++, pImgProcCol++) {
				if(*pImgProcCol == 255) {
					ptBlobStartPos = CPoint(x , y);
					if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE)
						nBlobFound = WhiteBlobContour(&ImgProc,
											&ptROITopLeft,
											&ptBlobStartPos, 
											&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
											lProcMode);
					else {
						AdvBlobInfoMore.Clean();
						nBlobFound = WhiteBlobContour(&ImgProc,
											&ptROITopLeft,
											&ptBlobStartPos, 
											&AdvBlobInfoMore,
											lProcMode);
					}
					if(nBlobFound == 0){
						if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE) {
							nBlobCheck = GetBlobInfo(&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
										&ImgProc,
										&rcSrc,
										pParm,
										lProcMode,
										lDebugFlag);
							if(nBlobCheck == 0)
								m_AdvBlobFindRslt.nFndBlobs++;
							else
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].Clean();
						}
						else {
							nBlobCheck = GetBlobInfo(&AdvBlobInfoMore,
										&ImgProc,
										&rcSrc,
										pParm,
										lProcMode,
										lDebugFlag);
							if(nBlobCheck == 0) {
								m_AdvBlobFindRslt.nFndBlobs++;
								if(lMemLocBlobNo >= m_AdvBlobFindRslt.nFndBlobs) {
									pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[nAryAdvBlobInfoCol] = &AdvBlobInfoMore;
									nAryAdvBlobInfoCol++;
								}
								else {
									if(m_AdvBlobFindRslt.nFndBlobs >= MAX_BLOB) {
										if ((DEBUG_DRAW&lDebugFlag) || (DEBUG_STEP_MODE&lDebugFlag)) 
											OutputDebugLog("Too many blobs=%d",MAX_BLOB);
										bAbort = true;
										goto Abort;
									}
									pAryAdvBlobBlock[nAryAdvBlobInfoRow].pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
									lMemLocBlobNo += NORMAL_BLOB_SIZE;
									nAryAdvBlobInfoRow++;
									pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[0] = &AdvBlobInfoMore;
									nAryAdvBlobInfoCol = 1;
								}
							}
							else
								AdvBlobInfoMore.Clean();
						}
					}
				}
			}
		} 
	}
	else {
		pImgProcLeftLine=pImgProc;
		pImgProcRightLine=pImgProc + nImgProcWidth - 1;
		for(y = 0; y < nImgProcHeight; y++, pImgProcLeftLine += nPitchProc, pImgProcRightLine += nPitchProc) {
			*pImgProcLeftLine = 255;
			*pImgProcRightLine = 255;
		}
		memset(pImgProc, 255, nImgProcWidth);
		memset(pImgProc + nPitchProc * (nImgProcHeight - 1), 255, nImgProcWidth);
		//pImgProcTopLine = pImgProc;
		//pImgProcBotLine = pImgProc + nPitchProc * (nImgProcHeight - 1);
		//for(x = 0; x < nImgProcWidth; x++) {
		//	*pImgProcTopLine++ = 255;
		//	*pImgProcBotLine++ = 255;
		//}
		m_AdvBlobFindRslt.nFndBlobs = 0;
		pImgProcRow = pImgProcCol = pImgProc;
		for(y = 0; y < nImgProcHeight; y++, pImgProcRow += nPitchProc) {
			for(x = 0, pImgProcCol = pImgProcRow; x < nImgProcWidth; x++, pImgProcCol++) {
				if(*pImgProcCol == 0) {
					ptBlobStartPos = CPoint(x , y);
					if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE)
						nBlobFound = BlackBlobContour(&ImgProc,
											&ptROITopLeft,
											&ptBlobStartPos, 
											&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
											lProcMode);
					else {
						AdvBlobInfoMore.Clean();
						nBlobFound = BlackBlobContour(&ImgProc,
											&ptROITopLeft,
											&ptBlobStartPos, 
											&AdvBlobInfoMore,
											lProcMode);
					}
					if(nBlobFound == 0){
						if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE) {
							nBlobCheck = GetBlobInfo(&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
										&ImgProc,
										&rcSrc,
										pParm,
										lProcMode,
										lDebugFlag);
							if(nBlobCheck == 0)
								m_AdvBlobFindRslt.nFndBlobs++;
							else
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].Clean();
						}
						else {
							nBlobCheck = GetBlobInfo(&AdvBlobInfoMore,
										&ImgProc,
										&rcSrc,
										pParm,
										lProcMode,
										lDebugFlag);
							if(nBlobCheck == 0) {
								m_AdvBlobFindRslt.nFndBlobs++;
								if(lMemLocBlobNo >= m_AdvBlobFindRslt.nFndBlobs) {
									pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[nAryAdvBlobInfoCol] = &AdvBlobInfoMore;
									nAryAdvBlobInfoCol++;
								}
								else {
									if(m_AdvBlobFindRslt.nFndBlobs >= MAX_BLOB) {
										if ((DEBUG_DRAW&lDebugFlag) || (DEBUG_STEP_MODE&lDebugFlag)) 
											OutputDebugLog("Too many blobs=%d",MAX_BLOB);
										bAbort = true;
										goto Abort;
									}
									pAryAdvBlobBlock[nAryAdvBlobInfoRow].pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
									lMemLocBlobNo += NORMAL_BLOB_SIZE;
									nAryAdvBlobInfoRow++;
									pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[0] = &AdvBlobInfoMore;
									nAryAdvBlobInfoCol = 1;
								}
							}
							else
								AdvBlobInfoMore.Clean();
						}
					}
				}
			}
		} 
	}

Abort:
	if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_BLOB)
		DrawAdvBlob(pView,pParm,&m_AdvBlobFindRslt);
	ImgProc.Free();
	if(nAryAdvBlobInfoRow) {
		int k;
		int nFoundBlob;
		nFoundBlob = m_AdvBlobFindRslt.nFndBlobs;
		AdvBlobFindRsltBak.pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
		for(x = 0; x < NORMAL_BLOB_SIZE; x++)
			AdvBlobFindRsltBak.pAdvBlobInfo[x] = &m_AdvBlobFindRslt.pAdvBlobInfo[x];
		m_AdvBlobFindRslt.nFndBlobs = NORMAL_BLOB_SIZE;
		m_AdvBlobFindRslt.Clean();
		m_AdvBlobFindRslt.nFndBlobs = nFoundBlob;
		m_AdvBlobFindRslt.pAdvBlobInfo = new ADV_BLOB_INFO[abs(m_AdvBlobFindRslt.nFndBlobs)];
		k = 0;
		for(x = 0; x < NORMAL_BLOB_SIZE; x++)
			m_AdvBlobFindRslt.pAdvBlobInfo[k++] = &AdvBlobFindRsltBak.pAdvBlobInfo[x];

		for(y = 0; y < nAryAdvBlobInfoRow; y++) {
			for(x = 0; x < NORMAL_BLOB_SIZE; x++) {
				if(k < m_AdvBlobFindRslt.nFndBlobs)
					m_AdvBlobFindRslt.pAdvBlobInfo[k++] = &pAryAdvBlobBlock[y].pAdvBlobInfo[x];
				else
					break;
			}
			pAryAdvBlobBlock[y].nFndBlobs = NORMAL_BLOB_SIZE;
			pAryAdvBlobBlock[y].Clean();
			if(k >= m_AdvBlobFindRslt.nFndBlobs)
				break;
		}
	}
	delete [] pAryAdvBlobBlock;
	delete [] plNonFilledAddr;
	delete [] plFillSeedsAddr1;
	delete [] plFillSeedsAddr2;

	if(bAbort)
		return -1;
	else
		return 0;
}

int CAdvBlobFind::GetAdvBlobHole(IMAGEVIEW_INFO *pView,
							  CImageBuffer *pImgSrc,
							  ADV_BLOB_FIND_PARM *pParm,
							  long lProcMode,
							  long lDebugFlag)
{
	int x, y;
	//BYTE *pImgProcTopLine, *pImgProcBotLine;
	BYTE *pImgProc, *pImgProcLeftLine, *pImgProcRightLine;
	BYTE *pImgProcRow, *pImgProcCol;
	CRect rcBlob;
	int nPitchSrc, nPitchProc;
	CImageBuffer ImgProc;
	CPoint ptBlobStartPos;
	int nImgProcWidth, nImgProcHeight;
	CRect rcSrc;
	CRect rcImgProc;
	CPoint ptROITopLeft;
	ADV_BLOB_FIND_RSLT AdvBlobFindRsltBak;
	ADV_BLOB_FIND_RSLT* pAryAdvBlobBlock;
	int nAryAdvBlobBlock;

	int nAryAdvBlobInfoRow;
	int nAryAdvBlobInfoCol;
	long lMemLocBlobNo;
	ADV_BLOB_INFO AdvBlobInfoMore;
	int nBlobFound;
	int nBlobCheck;
	bool bAbort;


	Clean();

	pAryAdvBlobBlock = NULL;
	plNonFilledAddr = NULL;
	plFillSeedsAddr1 = NULL;
	plFillSeedsAddr2 = NULL;

	if(lProcMode & FIND_BLOB_CONTOUR) 
		createArray(ptAryFndBlobContour, pImgSrc->GetImgSize().cx * pImgSrc->GetImgSize().cy);
	if(m_AdvBlobFindRslt.pAdvBlobInfo)
		m_AdvBlobFindRslt.Clean();
	bAbort = false;
	m_AdvBlobFindRslt.nFndBlobs = 0;
	m_lProcMode = lProcMode;
	rcSrc = pParm->rcROISrc;
	ptROITopLeft = rcSrc.TopLeft();

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(&pParm->rcROISrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"pParm->rcROISrc is Invalid");
		return -1;
	}
	if((pParm->rcROISrc.Width() == 0) || (pParm->rcROISrc.Height()==0))
		return 0;

	nAryAdvBlobBlock = MAX_BLOB / NORMAL_BLOB_SIZE + 1;
	pAryAdvBlobBlock = new ADV_BLOB_FIND_RSLT[abs(nAryAdvBlobBlock)];

	m_AdvBlobFindRslt.pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];

	nAryAdvBlobInfoRow = 0;
	nAryAdvBlobInfoCol = 0;
	lMemLocBlobNo = NORMAL_BLOB_SIZE;
	nPitchSrc = pImgSrc->GetPitch();
	nImgProcWidth = rcSrc.Width();
	nImgProcHeight = rcSrc.Height();

	ImgProc.Allocate(AbsSize(rcSrc.Size()));
	nPitchProc = ImgProc.GetPitch();

	pImgProc = ImgProc.GetImgPtr();
	rcImgProc = CRect(CPoint(0,0), pParm->rcROISrc.Size());
	plNonFilledAddr = new long[abs(nImgProcHeight * nImgProcWidth)];
	plFillSeedsAddr1 = new long[abs(nImgProcHeight * nImgProcWidth)];
	plFillSeedsAddr2 = new long[abs(nImgProcHeight * nImgProcWidth)];

	pnDir[0] = 1;
	pnDir[1] = -(nPitchProc - 1);
	pnDir[2] = -nPitchProc;
	pnDir[3] = -(nPitchProc + 1);
	pnDir[4] = -1;
	pnDir[5] = nPitchProc - 1;
	pnDir[6] = nPitchProc;
	pnDir[7] = nPitchProc + 1;

	ImgCopy(pImgSrc, &pParm->rcROISrc, &ImgProc, &rcImgProc);

	if(pParm->nBlobType == WHITE) {
		pImgProcLeftLine=pImgProc;
		pImgProcRightLine=pImgProc + nImgProcWidth - 1;
		for(y = 0; y < nImgProcHeight; y++, pImgProcLeftLine += nPitchProc, pImgProcRightLine += nPitchProc) {
			*pImgProcLeftLine = 0;
			*pImgProcRightLine = 0;
		}
		memset(pImgProc, 0, nImgProcWidth);
		memset(pImgProc + nPitchProc * (nImgProcHeight - 1), 0, nImgProcWidth);
		//pImgProcTopLine = pImgProc;
		//pImgProcBotLine = pImgProc + nPitchProc * (nImgProcHeight - 1);
		//for(x = 0; x < nImgProcWidth; x++) {
		//	*pImgProcTopLine++ = 0;
		//	*pImgProcBotLine++ = 0;
		//}
		pImgProcRow = pImgProcCol = pImgProc;
		for(y = 0; y < nImgProcHeight; y++, pImgProcRow += nPitchProc) {
			for(x = 0, pImgProcCol = pImgProcRow; x < nImgProcWidth; x++, pImgProcCol++) {
				if(*pImgProcCol == 255) {
					ptBlobStartPos = CPoint(x , y);
					if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE)
						nBlobFound = WhiteBlobContour(&ImgProc,
											&ptROITopLeft,
											&ptBlobStartPos, 
											&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
											lProcMode);
					else {
						AdvBlobInfoMore.Clean();
						nBlobFound = WhiteBlobContour(&ImgProc,
											&ptROITopLeft,
											&ptBlobStartPos, 
											&AdvBlobInfoMore,
											lProcMode);
					}
					if(nBlobFound == 0){
						if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE) {
							nBlobCheck = GetBlobHoleInfo(&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
										&ImgProc,
										&ptROITopLeft,
										&rcSrc,
										pParm,
										lProcMode,
										lDebugFlag);
							if(nBlobCheck == 0)
								m_AdvBlobFindRslt.nFndBlobs++;
							else
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].Clean();
						}
						else {
							nBlobCheck = GetBlobHoleInfo(&AdvBlobInfoMore,
										&ImgProc,
										&ptROITopLeft,
										&rcSrc,
										pParm,
										lProcMode,
										lDebugFlag);
							if(nBlobCheck == 0) {
								m_AdvBlobFindRslt.nFndBlobs++;
								if(lMemLocBlobNo >= m_AdvBlobFindRslt.nFndBlobs) {
									pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[nAryAdvBlobInfoCol] = &AdvBlobInfoMore;
									nAryAdvBlobInfoCol++;
								}
								else {
									if(m_AdvBlobFindRslt.nFndBlobs >= MAX_BLOB) {
										if ((DEBUG_DRAW&lDebugFlag) || (DEBUG_STEP_MODE&lDebugFlag)) 
											OutputDebugLog("Too many blobs=%d",MAX_BLOB);
										bAbort = true;
										goto Abort;
									}
									pAryAdvBlobBlock[nAryAdvBlobInfoRow].pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
									lMemLocBlobNo += NORMAL_BLOB_SIZE;
									nAryAdvBlobInfoRow++;
									pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[0] = &AdvBlobInfoMore;
									nAryAdvBlobInfoCol = 1;
								}
							}
							else
								AdvBlobInfoMore.Clean();
						}
					}
				}
			}
		} 
	}
	else {
		pImgProcLeftLine=pImgProc;
		pImgProcRightLine=pImgProc + nImgProcWidth - 1;
		for(y = 0; y < nImgProcHeight; y++, pImgProcLeftLine += nPitchProc, pImgProcRightLine += nPitchProc) {
			*pImgProcLeftLine = 255;
			*pImgProcRightLine = 255;
		}
		memset(pImgProc, 255, nImgProcWidth);
		memset(pImgProc + nPitchProc * (nImgProcHeight - 1), 255, nImgProcWidth);
		//pImgProcTopLine = pImgProc;
		//pImgProcBotLine = pImgProc + nPitchProc * (nImgProcHeight - 1);
		//for(x = 0; x < nImgProcWidth; x++) {
		//	*pImgProcTopLine++ = 255;
		//	*pImgProcBotLine++ = 255;
		//}
		m_AdvBlobFindRslt.nFndBlobs = 0;
		pImgProcRow = pImgProcCol = pImgProc;
		for(y = 0; y < nImgProcHeight; y++, pImgProcRow += nPitchProc) {
			for(x = 0, pImgProcCol = pImgProcRow; x < nImgProcWidth; x++, pImgProcCol++) {
				if(*pImgProcCol == 0) {
					ptBlobStartPos = CPoint(x , y);
					if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE)
						nBlobFound = BlackBlobContour(&ImgProc,
											&ptROITopLeft,
											&ptBlobStartPos, 
											&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
											lProcMode);
					else {
						AdvBlobInfoMore.Clean();
						nBlobFound = BlackBlobContour(&ImgProc,
											&ptROITopLeft,
											&ptBlobStartPos, 
											&AdvBlobInfoMore,
											lProcMode);
					}
					if(nBlobFound == 0){
						if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE) {
							nBlobCheck = GetBlobHoleInfo(&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
										&ImgProc,
										&ptROITopLeft,
										&rcSrc,
										pParm,
										lProcMode,
										lDebugFlag);
							if(nBlobCheck == 0)
								m_AdvBlobFindRslt.nFndBlobs++;
							else
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].Clean();
						}
						else {
							nBlobCheck = GetBlobHoleInfo(&AdvBlobInfoMore,
										&ImgProc,
										&ptROITopLeft,
										&rcSrc,
										pParm,
										lProcMode,
										lDebugFlag);
							if(nBlobCheck == 0) {
								m_AdvBlobFindRslt.nFndBlobs++;
								if(lMemLocBlobNo >= m_AdvBlobFindRslt.nFndBlobs) {
									pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[nAryAdvBlobInfoCol] = &AdvBlobInfoMore;
									nAryAdvBlobInfoCol++;
								}
								else {
									if(m_AdvBlobFindRslt.nFndBlobs >= MAX_BLOB) {
										if ((DEBUG_DRAW&lDebugFlag) || (DEBUG_STEP_MODE&lDebugFlag)) 
											OutputDebugLog("Too many blobs=%d",MAX_BLOB);
										bAbort = true;
										goto Abort;
									}
									pAryAdvBlobBlock[nAryAdvBlobInfoRow].pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
									lMemLocBlobNo += NORMAL_BLOB_SIZE;
									nAryAdvBlobInfoRow++;
									pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[0] = &AdvBlobInfoMore;
									nAryAdvBlobInfoCol = 1;
								}
							}
							else
								AdvBlobInfoMore.Clean();
						}
					}
				}
			}
		} 
	}

Abort:
	if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_BLOB)
		DrawAdvBlob(pView,pParm,&m_AdvBlobFindRslt);
	ImgProc.Free();
	if(nAryAdvBlobInfoRow) {
		int k;
		int nFoundBlob;
		nFoundBlob = m_AdvBlobFindRslt.nFndBlobs;
		AdvBlobFindRsltBak.pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
		for(x = 0; x < NORMAL_BLOB_SIZE; x++)
			AdvBlobFindRsltBak.pAdvBlobInfo[x] = &m_AdvBlobFindRslt.pAdvBlobInfo[x];
		m_AdvBlobFindRslt.nFndBlobs = NORMAL_BLOB_SIZE;
		m_AdvBlobFindRslt.Clean();
		m_AdvBlobFindRslt.nFndBlobs = nFoundBlob;
		m_AdvBlobFindRslt.pAdvBlobInfo = new ADV_BLOB_INFO[abs(m_AdvBlobFindRslt.nFndBlobs)];
		k = 0;
		for(x = 0; x < NORMAL_BLOB_SIZE; x++)
			m_AdvBlobFindRslt.pAdvBlobInfo[k++] = &AdvBlobFindRsltBak.pAdvBlobInfo[x];

		for(y = 0; y < nAryAdvBlobInfoRow; y++) {
			for(x = 0; x < NORMAL_BLOB_SIZE; x++) {
				if(k < m_AdvBlobFindRslt.nFndBlobs)
					m_AdvBlobFindRslt.pAdvBlobInfo[k++] = &pAryAdvBlobBlock[y].pAdvBlobInfo[x];
				else
					break;
			}
			pAryAdvBlobBlock[y].nFndBlobs = NORMAL_BLOB_SIZE;
			pAryAdvBlobBlock[y].Clean();
			if(k >= m_AdvBlobFindRslt.nFndBlobs)
				break;
		}
	}
	if(pAryAdvBlobBlock != NULL)
		delete [] pAryAdvBlobBlock;
	if(plNonFilledAddr != NULL)
		delete [] plNonFilledAddr;
	if(plFillSeedsAddr1 != NULL)
		delete [] plFillSeedsAddr1;
	if(plFillSeedsAddr2 != NULL)
	delete [] plFillSeedsAddr2;

	if(bAbort)
		return -1;
	else
		return 0;
}


int CAdvBlobFind::GetBlobHoleInfo(ADV_BLOB_INFO *pAdvBlobInfo,
							  CImageBuffer* pImgBufProc,
							  CPoint* pptROITopLeft,
							  CRect* prcOrg,
							  ADV_BLOB_FIND_PARM *pParm,
							  long lProcMode,
							  long lDebugFlag)
{
	int m, n, k;
	long lm;
	long lIndex;
	CRect rcBlob;
	BOOL bBlobFound;
	long lBlobArea;
	long lBlobFillRowIndex, lBlobFillIndex;
	BYTE* pImgProc;
	CPoint ptBlobStartPos;
	int nPitchProc;
	CPoint* pptSingleHoleContour;
	int nptSingleHoleContour;
	CPoint* pptHoleContours;
	CPoint* ptHoleContourIndex;
	int nHoleNo;
	CPoint ptHoleStartPos;
	int nError;
	int nStart;
	int nEnd;
	CRect rcHoleFound;
	int nValidHoleNo;
	double dXMoment, dYMoment;
	CArray<BOOL,BOOL> CBOOLAryHole;
	CArray<CRect,CRect> CRectAryHole;
	int nHoleContourMem;
	CRect rcCleanBlob;

	pptSingleHoleContour = NULL;
	pptHoleContours = NULL;
	ptHoleContourIndex = NULL;
	CBOOLAryHole.RemoveAll();
	CRectAryHole.RemoveAll();
	pImgProc = pImgBufProc->GetImgPtr();
	nPitchProc = pImgBufProc->GetPitch();
	rcBlob = pAdvBlobInfo->rcFndBlobPos;
	rcCleanBlob = rcBlob;
	lBlobFillRowIndex = rcBlob.top * nPitchProc;
	lNonFilledNo = 0;
	if(pParm->nBlobType == WHITE) {
		for(m = rcBlob.top; m < rcBlob.bottom; m++) {
			for(n = rcBlob.left;n < rcBlob.right; n++) {
				lBlobFillIndex = lBlobFillRowIndex + n;
				if(pImgProc[lBlobFillIndex] == 255) {
					if((pImgProc[lBlobFillIndex -1] == 200) || (pImgProc[lBlobFillIndex -nPitchProc] == 200)) 
						pImgProc[lBlobFillIndex] = 200;
					else
						plNonFilledAddr[lNonFilledNo++] = lBlobFillIndex;
				}
			}
			lBlobFillRowIndex += nPitchProc;
		}

		lFillSeedsNo1 = 0;
		for(lm = 0; lm < lNonFilledNo; lm++) {
			lIndex = plNonFilledAddr[lm];
			for(k = 0; k < 8; k++) {
				if(pImgProc[lIndex + pnDir[k]] == 200) {
					pImgProc[lIndex] = 200;
					plFillSeedsAddr1[lFillSeedsNo1++] = lIndex;
					break;
				}
			}
		}
		while(lFillSeedsNo1 > 0) {
			lFillSeedsNo2 = 0;
			for(lm = 0; lm < lFillSeedsNo1; lm++) {
				lIndex = plFillSeedsAddr1[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 255) {
						plFillSeedsAddr2[lFillSeedsNo2++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
			if(lFillSeedsNo2 == 0)
				break;
			lFillSeedsNo1 = 0;
			for(lm = 0; lm < lFillSeedsNo2; lm++) {
				lIndex = plFillSeedsAddr2[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 255) {
						plFillSeedsAddr1[lFillSeedsNo1++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
		}
		pAdvBlobInfo->nBlobHole = 0;
		pAdvBlobInfo->ppptBlobHoleContour = NULL;
		pAdvBlobInfo->pnptBlobHoleContourSize = NULL;

		if((lProcMode & FIND_BLOB_HOLE) && ((rcBlob.Width() > 2) && (rcBlob.Height() > 2))) {
			BOOL bHole;
			rcCleanBlob.InflateRect(1,1,1,1);
			nHoleNo = 0;
			nValidHoleNo = 0;
			nHoleContourMem = 4 * (rcBlob.Width()+2) * (rcBlob.Height()+2);
			//pptSingleHoleContour = new CPoint[abs(nHoleContourMem)];
			//pptHoleContours = new CPoint[abs(nHoleContourMem)];
			//ptHoleContourIndex = new CPoint[abs(nHoleContourMem)];
			createArray(pptSingleHoleContour, nHoleContourMem);
			createArray(pptHoleContours, nHoleContourMem);
			createArray(ptHoleContourIndex, nHoleContourMem);
			//pptSingleHoleContour = (CPoint*) malloc(sizeof(CPoint) * abs(nHoleContourMem));
			//pptHoleContours = (CPoint*) malloc(sizeof(CPoint) * abs(nHoleContourMem));
			//ptHoleContourIndex = (CPoint*) malloc(sizeof(CPoint) * abs(nHoleContourMem));

			lBlobFillRowIndex = rcBlob.top * nPitchProc;

			for(m = rcBlob.top;m < (rcBlob.bottom-1); m++) {
				for(n = rcBlob.left;n < (rcBlob.right-1); n++) {
					if((pImgProc[lBlobFillRowIndex + n] == 200) && (pImgProc[lBlobFillRowIndex + n + 1] == 0)) {
						ptHoleStartPos = CPoint(n+1,m);
						nError =BlackHoleContour(pImgBufProc,
												 &rcBlob,
												 pptROITopLeft,
												 &ptHoleStartPos,
												 &pptSingleHoleContour[0],
												 &nptSingleHoleContour,
												 &rcHoleFound,
												 150,
												 100,
												 &bHole);

						CRectAryHole.Add(rcHoleFound);
						if((pImgProc[lBlobFillRowIndex + n - nPitchProc] == 0) && 
							(pImgProc[lBlobFillRowIndex + n - nPitchProc + 1] == 0))
							bHole = FALSE;
						if((bHole == FALSE) ||
						   (rcBlob.left >= rcHoleFound.left) ||
						   (rcBlob.top >= rcHoleFound.top) ||
						   (rcBlob.right <= rcHoleFound.right) ||
						   (rcBlob.bottom <= rcHoleFound.bottom)) 
							CBOOLAryHole.Add(FALSE);
						else {
							CBOOLAryHole.Add(TRUE);
							nValidHoleNo++;
						}
						if(nHoleNo == 0) {
							ptHoleContourIndex[nHoleNo].x = 0;
						}
						else {
							ptHoleContourIndex[nHoleNo].x = ptHoleContourIndex[nHoleNo-1].y;
						}
						ptHoleContourIndex[nHoleNo].y = ptHoleContourIndex[nHoleNo].x + nptSingleHoleContour;
						nStart = ptHoleContourIndex[nHoleNo].x;
						for(k=0;k<nptSingleHoleContour;k++) 
							pptHoleContours[nStart+k] = pptSingleHoleContour[k];
						nHoleNo++;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
			if(nValidHoleNo > 0) {
				pAdvBlobInfo->nBlobHole = nValidHoleNo;
				createArray(pAdvBlobInfo->ppptBlobHoleContour, nValidHoleNo);
				createArray(pAdvBlobInfo->pnptBlobHoleContourSize, nValidHoleNo, false);
				createArray(pAdvBlobInfo->prcHole, nValidHoleNo);
				//pAdvBlobInfo->ppptBlobHoleContour = new CPoint*[abs(nValidHoleNo)];
				//pAdvBlobInfo->pnptBlobHoleContourSize = new int[abs(nValidHoleNo)];
				//pAdvBlobInfo->prcHole = new CRect[abs(nValidHoleNo)];
				nValidHoleNo = 0;
				for(m=0;m<nHoleNo;m++) {
					if(CBOOLAryHole[m] == FALSE)
						continue;
					nStart = ptHoleContourIndex[m].x;
					nEnd = ptHoleContourIndex[m].y;
					nptSingleHoleContour = nEnd - nStart;
					if(nptSingleHoleContour > 0) {
						pAdvBlobInfo->pnptBlobHoleContourSize[nValidHoleNo] = nptSingleHoleContour;
						createArray(pAdvBlobInfo->ppptBlobHoleContour[nValidHoleNo], nptSingleHoleContour);
						//pAdvBlobInfo->ppptBlobHoleContour[nValidHoleNo] = new CPoint[abs(nptSingleHoleContour)];
						for(n=0;n<nptSingleHoleContour;n++) {
							pAdvBlobInfo->ppptBlobHoleContour[nValidHoleNo][n] = pptHoleContours[nStart + n];
						}
						pAdvBlobInfo->prcHole[nValidHoleNo] = CRectAryHole[m] + *pptROITopLeft;
						nValidHoleNo++;
					}
				}
			}
			else {
				pAdvBlobInfo->ppptBlobHoleContour = NULL;
				pAdvBlobInfo->pnptBlobHoleContourSize = NULL;
			}

		}

		if(ValidateRect(&rcCleanBlob, &(pImgBufProc->GetImgSize()))) {
			CRect rcpImgBufProc;
			rcpImgBufProc = CRect(CPoint(0,0), pImgBufProc->GetImgSize());
			nError = ClearImageROIEdge(pImgBufProc, &rcpImgBufProc, 0);
			nError = CheckRect(&rcCleanBlob, &(pImgBufProc->GetImgSize()));
		}
		lBlobArea = 0;
		if(lProcMode & FIND_BLOB_CENTER) { 
			dXMoment = 0;
			dYMoment = 0;
			lBlobFillRowIndex = rcCleanBlob.top * nPitchProc;
			for(m = rcCleanBlob.top;m < rcCleanBlob.bottom; m++) {
				for(n = rcCleanBlob.left;n < rcCleanBlob.right; n++) {
					if((pImgProc[lBlobFillRowIndex + n] != 0) && (pImgProc[lBlobFillRowIndex + n] != 255)) {
						if(pImgProc[lBlobFillRowIndex + n] == 200) {
							dXMoment += n;
							dYMoment += m;
							lBlobArea++;
						}
						pImgProc[lBlobFillRowIndex + n]=0;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
			pAdvBlobInfo->pt2DFndBlobCenter = CPoint2D( dXMoment / lBlobArea + prcOrg->left, dYMoment / lBlobArea + prcOrg->top);
		}
		else {
			lBlobFillRowIndex = rcCleanBlob.top * nPitchProc;
			for(m = rcCleanBlob.top;m < rcCleanBlob.bottom; m++) {
				for(n = rcCleanBlob.left;n < rcCleanBlob.right; n++) {
					if((pImgProc[lBlobFillRowIndex + n] != 0) && (pImgProc[lBlobFillRowIndex + n] != 255)) {
						if(pImgProc[lBlobFillRowIndex + n] == 200) 
							lBlobArea++;
						pImgProc[lBlobFillRowIndex + n]=0;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
	}
	else {
		for(m = rcBlob.top; m < rcBlob.bottom; m++) {
			for(n = rcBlob.left;n < rcBlob.right; n++) {
				lBlobFillIndex = lBlobFillRowIndex + n;
				if(pImgProc[lBlobFillIndex] == 0) {
					if((pImgProc[lBlobFillIndex -1] == 200) || (pImgProc[lBlobFillIndex -nPitchProc] == 200)) 
						pImgProc[lBlobFillIndex] = 200;
					else
						plNonFilledAddr[lNonFilledNo++] = lBlobFillIndex;
				}
			}
			lBlobFillRowIndex += nPitchProc;
		}

		lFillSeedsNo1 = 0;
		for(lm = 0; lm < lNonFilledNo; lm++) {
			lIndex = plNonFilledAddr[lm];
			for(k = 0; k < 8; k++) {
				if(pImgProc[lIndex + pnDir[k]] == 200) {
					pImgProc[lIndex] = 200;
					plFillSeedsAddr1[lFillSeedsNo1++] = lIndex;
					break;
				}
			}
		}
		while(lFillSeedsNo1 > 0) {
			lFillSeedsNo2 = 0;
			for(lm = 0; lm < lFillSeedsNo1; lm++) {
				lIndex = plFillSeedsAddr1[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 0) {
						plFillSeedsAddr2[lFillSeedsNo2++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
			if(lFillSeedsNo2 == 0)
				break;
			lFillSeedsNo1 = 0;
			for(lm = 0; lm < lFillSeedsNo2; lm++) {
				lIndex = plFillSeedsAddr2[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 0) {
						plFillSeedsAddr1[lFillSeedsNo1++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
		}
		if((lProcMode & FIND_BLOB_HOLE) && ((rcBlob.Width() > 2) && (rcBlob.Height() > 2))) {
			BOOL bHole;
			rcCleanBlob.InflateRect(1,1,1,1);
			nHoleNo = 0;
			nValidHoleNo = 0;
			nHoleContourMem = 4 * (rcBlob.Width()+2) * (rcBlob.Height()+2);
			//pptSingleHoleContour = new CPoint[abs(nHoleContourMem)];
			//pptHoleContours = new CPoint[abs(nHoleContourMem)];
			//ptHoleContourIndex = new CPoint[abs(nHoleContourMem)];
			createArray(pptSingleHoleContour, nHoleContourMem);
			createArray(pptHoleContours, nHoleContourMem);
			createArray(ptHoleContourIndex, nHoleContourMem);
			//pptSingleHoleContour = (CPoint*) malloc(sizeof(CPoint) * abs(nHoleContourMem));
			//pptHoleContours = (CPoint*) malloc(sizeof(CPoint) * abs(nHoleContourMem));
			//ptHoleContourIndex = (CPoint*) malloc(sizeof(CPoint) * abs(nHoleContourMem));

			lBlobFillRowIndex = rcBlob.top * nPitchProc;

			for(m = rcBlob.top;m < (rcBlob.bottom-1); m++) {
				for(n = rcBlob.left;n < (rcBlob.right-1); n++) {
					if((pImgProc[lBlobFillRowIndex + n] == 200) && (pImgProc[lBlobFillRowIndex + n + 1] == 255)) {
						ptHoleStartPos = CPoint(n+1,m);
						nError =WhiteHoleContour(pImgBufProc,
												 &rcBlob,
												 pptROITopLeft,
												 &ptHoleStartPos,
												 &pptSingleHoleContour[0],
												 &nptSingleHoleContour,
												 &rcHoleFound,
												 210,
												 230,
												 &bHole);
						CRectAryHole.Add(rcHoleFound);
						if((pImgProc[lBlobFillRowIndex + n - nPitchProc] == 255) && 
							(pImgProc[lBlobFillRowIndex + n - nPitchProc + 1] == 255))
							bHole = FALSE;
						if((bHole == FALSE) ||
						   (rcBlob.left >= rcHoleFound.left) ||
						   (rcBlob.top >= rcHoleFound.top) ||
						   (rcBlob.right <= rcHoleFound.right) ||
						   (rcBlob.bottom <= rcHoleFound.bottom)) 
							CBOOLAryHole.Add(FALSE);
						else {
							CBOOLAryHole.Add(TRUE);
							nValidHoleNo++;
						}
						if(nHoleNo == 0) {
							ptHoleContourIndex[nHoleNo].x = 0;
						}
						else {
							ptHoleContourIndex[nHoleNo].x = ptHoleContourIndex[nHoleNo-1].y;
						}
						ptHoleContourIndex[nHoleNo].y = ptHoleContourIndex[nHoleNo].x + nptSingleHoleContour;
						nStart = ptHoleContourIndex[nHoleNo].x;
						for(k=0;k<nptSingleHoleContour;k++) 
							pptHoleContours[nStart+k] = pptSingleHoleContour[k];
						nHoleNo++;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
			if(nValidHoleNo > 0) {
				pAdvBlobInfo->nBlobHole = nValidHoleNo;
				createArray(pAdvBlobInfo->ppptBlobHoleContour, nValidHoleNo);
				createArray(pAdvBlobInfo->pnptBlobHoleContourSize, nValidHoleNo, false);
				createArray(pAdvBlobInfo->prcHole, nValidHoleNo);
				//pAdvBlobInfo->ppptBlobHoleContour = new CPoint*[abs(nValidHoleNo)];
				//pAdvBlobInfo->pnptBlobHoleContourSize = new int[abs(nValidHoleNo)];
				//pAdvBlobInfo->prcHole = new CRect[abs(nValidHoleNo)];
				nValidHoleNo = 0;
				for(m=0;m<nHoleNo;m++) {
					if(CBOOLAryHole[m] == FALSE)
						continue;
					nStart = ptHoleContourIndex[m].x;
					nEnd = ptHoleContourIndex[m].y;
					nptSingleHoleContour = nEnd - nStart;
					if(nptSingleHoleContour > 0) {
						pAdvBlobInfo->pnptBlobHoleContourSize[nValidHoleNo] = nptSingleHoleContour;
						createArray(pAdvBlobInfo->ppptBlobHoleContour[nValidHoleNo], nptSingleHoleContour);
						//pAdvBlobInfo->ppptBlobHoleContour[nValidHoleNo] = new CPoint[abs(nptSingleHoleContour)];
						for(n=0;n<nptSingleHoleContour;n++) {
							pAdvBlobInfo->ppptBlobHoleContour[nValidHoleNo][n] = pptHoleContours[nStart + n];
						}
						pAdvBlobInfo->prcHole[nValidHoleNo] = CRectAryHole[m] + *pptROITopLeft;
						nValidHoleNo++;
					}
				}
			}
			else {
				pAdvBlobInfo->ppptBlobHoleContour = NULL;
				pAdvBlobInfo->pnptBlobHoleContourSize = NULL;
			}

		}
		if(ValidateRect(&rcCleanBlob, &(pImgBufProc->GetImgSize()))) {
			CRect rcpImgBufProc;
			rcpImgBufProc = CRect(CPoint(0,0), pImgBufProc->GetImgSize());
			nError = ClearImageROIEdge(pImgBufProc, &rcpImgBufProc, 0);
			nError = CheckRect(&rcCleanBlob, &(pImgBufProc->GetImgSize()));
		}
		lBlobArea = 0;
		if(lProcMode & FIND_BLOB_CENTER) { 
			dXMoment = 0;
			dYMoment = 0;
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if((pImgProc[lBlobFillRowIndex + n] != 0) && (pImgProc[lBlobFillRowIndex + n] != 255)) {
						if(pImgProc[lBlobFillRowIndex + n] == 200) {
							dXMoment += n;
							dYMoment += m;
							lBlobArea++;
						}
						pImgProc[lBlobFillRowIndex + n]=255;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
			pAdvBlobInfo->pt2DFndBlobCenter = CPoint2D( dXMoment / lBlobArea + prcOrg->left, dYMoment / lBlobArea + prcOrg->top);
		}
		else {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if((pImgProc[lBlobFillRowIndex + n] != 0) && (pImgProc[lBlobFillRowIndex + n] != 255)) {
						if(pImgProc[lBlobFillRowIndex + n] == 200) {
							lBlobArea++;
						}
						pImgProc[lBlobFillRowIndex + n]=255;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
	}
	pAdvBlobInfo->nFndBlobArea = lBlobArea;
	if(lProcMode & FIND_BLOB_ELONGATION) {
		double dWidth;
		if(pAdvBlobInfo->dFndBlobLength) {
			dWidth = pAdvBlobInfo->nFndBlobArea / pAdvBlobInfo->dFndBlobLength;
			if(dWidth < 1) dWidth = 1;
			pAdvBlobInfo->dFndBlobElongation = pAdvBlobInfo->dFndBlobLength / dWidth;
		}
		else pAdvBlobInfo->dFndBlobElongation = 0;
	}
	pAdvBlobInfo->rcFndBlobPos += CPoint(prcOrg->left, prcOrg->top);
	bBlobFound = FoundBlobCheck(pParm, pAdvBlobInfo, lProcMode);
	deleteArray(pptSingleHoleContour);
	deleteArray(pptHoleContours);
	deleteArray(ptHoleContourIndex);
	//if(pptSingleHoleContour != NULL)
	//	free(pptSingleHoleContour);
		//delete [] pptSingleHoleContour;
	//if(pptHoleContours != NULL)
	//	free(pptHoleContours);
		//delete [] pptHoleContours;
	//if(ptHoleContourIndex != NULL)
	//	free(ptHoleContourIndex);
		//delete [] ptHoleContourIndex;
	CBOOLAryHole.RemoveAll();
	CRectAryHole.RemoveAll();
	return bBlobFound;
}


int CAdvBlobFind::GetBlobInfo(ADV_BLOB_INFO *pAdvBlobInfo,
							  CImageBuffer* pImgBufProc,
							  CRect* prcOrg,
							  ADV_BLOB_FIND_PARM *pParm,
							  long lProcMode,
							  long lDebugFlag)
{
	int m, n, k;
	long lm;
	long lIndex;
	CRect rcBlob;
	BOOL bBlobFound;
	long lBlobArea;
	long lBlobFillRowIndex, lBlobFillIndex;
	BYTE* pImgProc;
	CPoint ptBlobStartPos;
	int nPitchProc;
	double dXMoment, dYMoment;

	pImgProc = pImgBufProc->GetImgPtr();
	nPitchProc = pImgBufProc->GetPitch();
	rcBlob = pAdvBlobInfo->rcFndBlobPos;
	lBlobFillRowIndex = rcBlob.top * nPitchProc;
	lNonFilledNo = 0;
	if(pParm->nBlobType == WHITE) {
		for(m = rcBlob.top; m < rcBlob.bottom; m++) {
			for(n = rcBlob.left;n < rcBlob.right; n++) {
				lBlobFillIndex = lBlobFillRowIndex + n;
				if(pImgProc[lBlobFillIndex] == 255) {
					if((pImgProc[lBlobFillIndex -1] == 200) || (pImgProc[lBlobFillIndex -nPitchProc] == 200)) 
						pImgProc[lBlobFillIndex] = 200;
					else
						plNonFilledAddr[lNonFilledNo++] = lBlobFillIndex;
				}
			}
			lBlobFillRowIndex += nPitchProc;
		}

		lFillSeedsNo1 = 0;
		for(lm = 0; lm < lNonFilledNo; lm++) {
			lIndex = plNonFilledAddr[lm];
			for(k = 0; k < 8; k++) {
				if(pImgProc[lIndex + pnDir[k]] == 200) {
					pImgProc[lIndex] = 200;
					plFillSeedsAddr1[lFillSeedsNo1++] = lIndex;
					break;
				}
			}
		}
		while(lFillSeedsNo1 > 0) {
			lFillSeedsNo2 = 0;
			for(lm = 0; lm < lFillSeedsNo1; lm++) {
				lIndex = plFillSeedsAddr1[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 255) {
						plFillSeedsAddr2[lFillSeedsNo2++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
			if(lFillSeedsNo2 == 0)
				break;
			lFillSeedsNo1 = 0;
			for(lm = 0; lm < lFillSeedsNo2; lm++) {
				lIndex = plFillSeedsAddr2[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 255) {
						plFillSeedsAddr1[lFillSeedsNo1++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
		}

		lBlobArea = 0;
		if(lProcMode & FIND_BLOB_CENTER) { 
			dXMoment = 0;
			dYMoment = 0;
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						dXMoment += n;
						dYMoment += m;
						lBlobArea++;
						pImgProc[lBlobFillRowIndex + n]=0;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
			pAdvBlobInfo->pt2DFndBlobCenter = CPoint2D( dXMoment / lBlobArea + prcOrg->left, dYMoment / lBlobArea + prcOrg->top);
		}
		else {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						lBlobArea++;
						pImgProc[lBlobFillRowIndex + n]=0;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
	}
	else {
		for(m = rcBlob.top; m < rcBlob.bottom; m++) {
			for(n = rcBlob.left;n < rcBlob.right; n++) {
				lBlobFillIndex = lBlobFillRowIndex + n;
				if(pImgProc[lBlobFillIndex] == 0) {
					if((pImgProc[lBlobFillIndex -1] == 200) || (pImgProc[lBlobFillIndex -nPitchProc] == 200)) 
						pImgProc[lBlobFillIndex] = 200;
					else
						plNonFilledAddr[lNonFilledNo++] = lBlobFillIndex;
				}
			}
			lBlobFillRowIndex += nPitchProc;
		}

		lFillSeedsNo1 = 0;
		for(lm = 0; lm < lNonFilledNo; lm++) {
			lIndex = plNonFilledAddr[lm];
			for(k = 0; k < 8; k++) {
				if(pImgProc[lIndex + pnDir[k]] == 200) {
					pImgProc[lIndex] = 200;
					plFillSeedsAddr1[lFillSeedsNo1++] = lIndex;
					break;
				}
			}
		}
		while(lFillSeedsNo1 > 0) {
			lFillSeedsNo2 = 0;
			for(lm = 0; lm < lFillSeedsNo1; lm++) {
				lIndex = plFillSeedsAddr1[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 0) {
						plFillSeedsAddr2[lFillSeedsNo2++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
			if(lFillSeedsNo2 == 0)
				break;
			lFillSeedsNo1 = 0;
			for(lm = 0; lm < lFillSeedsNo2; lm++) {
				lIndex = plFillSeedsAddr2[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 0) {
						plFillSeedsAddr1[lFillSeedsNo1++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
		}
		lBlobArea = 0;
		if(lProcMode & FIND_BLOB_CENTER) { 
			dXMoment = 0;
			dYMoment = 0;
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						dXMoment += n;
						dYMoment += m;
						lBlobArea++;
						pImgProc[lBlobFillRowIndex + n]=255;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
			pAdvBlobInfo->pt2DFndBlobCenter = CPoint2D( dXMoment / lBlobArea + prcOrg->left, dYMoment / lBlobArea + prcOrg->top);
		}
		else {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						lBlobArea++;
						pImgProc[lBlobFillRowIndex + n]=255;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
	}
	pAdvBlobInfo->nFndBlobArea = lBlobArea;
	if(lProcMode & FIND_BLOB_ELONGATION) {
		double dWidth;
		if(pAdvBlobInfo->dFndBlobLength) {
			dWidth = pAdvBlobInfo->nFndBlobArea / pAdvBlobInfo->dFndBlobLength;
			if(dWidth < 1) dWidth = 1;
			pAdvBlobInfo->dFndBlobElongation = pAdvBlobInfo->dFndBlobLength / dWidth;
		}
		else pAdvBlobInfo->dFndBlobElongation = 0;
	}
	pAdvBlobInfo->rcFndBlobPos += CPoint(prcOrg->left, prcOrg->top);
	bBlobFound = FoundBlobCheck(pParm, pAdvBlobInfo, lProcMode);
	return bBlobFound;
}

int CAdvBlobFind::GetAdvBlobFiltering(IMAGEVIEW_INFO *pView,
								CImageBuffer *pImgSrc,	
								CImageBuffer *pImgDst,	
								ADV_BLOB_FIND_PARM *pParm,
							    long lProcMode,		
								long lDebugFlag)
{
	int x, y;
	//BYTE *pImgProcTopLine, *pImgProcBotLine;
	BYTE *pImgProc, *pImgProcLeftLine, *pImgProcRightLine;
	BYTE *pImgProcRow, *pImgProcCol;
	CRect rcBlob;
	int nPitchSrc, nPitchProc;
	CImageBuffer ImgProc;
	CPoint ptBlobStartPos;
	int nImgProcWidth, nImgProcHeight;
	CRect rcSrc;
	CRect rcImgProc;
	CPoint ptROITopLeft;
	ADV_BLOB_FIND_RSLT AdvBlobFindRsltBak;
	ADV_BLOB_FIND_RSLT* pAryAdvBlobBlock;
	int nAryAdvBlobBlock;

	int nAryAdvBlobInfoRow;
	int nAryAdvBlobInfoCol;
	long lMemLocBlobNo;
	ADV_BLOB_INFO AdvBlobInfoMore;
	int nBlobFound;
	int nBlobCheck;
	bool bAbort;


	Clean();
	if(lProcMode & FIND_BLOB_CONTOUR) 
		createArray(ptAryFndBlobContour, pImgSrc->GetImgSize().cx * pImgSrc->GetImgSize().cy);
	bAbort = false;
	m_AdvBlobFindRslt.nFndBlobs = 0;
	m_lProcMode = lProcMode;
	rcSrc = pParm->rcROISrc;
	ptROITopLeft = rcSrc.TopLeft();

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(&pParm->rcROISrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"pParm->rcROISrc is Invalid");
		return -1;
	}
	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(&pParm->rcROIDst, &(pImgDst->GetImgSize()))) {
		ASSERT(!"pParm->rcROIDst is Invalid");
		return -1;
	}
	if (pParm->rcROISrc.Size() != pParm->rcROIDst.Size()) {
		ASSERT(!"pParm->rcROISrc Size different from pParm->rcROIDst Size");
		return -1;
	}

	nAryAdvBlobBlock = MAX_BLOB / NORMAL_BLOB_SIZE + 1;
	pAryAdvBlobBlock = new ADV_BLOB_FIND_RSLT[abs(nAryAdvBlobBlock)];

	if(m_AdvBlobFindRslt.pAdvBlobInfo)
		m_AdvBlobFindRslt.Clean();
	m_AdvBlobFindRslt.pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];

	nAryAdvBlobInfoRow = 0;
	nAryAdvBlobInfoCol = 0;
	lMemLocBlobNo = NORMAL_BLOB_SIZE;
	nPitchSrc = pImgSrc->GetPitch();
	nImgProcWidth = rcSrc.Width();
	nImgProcHeight = rcSrc.Height();


	ImgProc.Allocate(AbsSize(rcSrc.Size()));
	nPitchProc = ImgProc.GetPitch();

	pImgProc = ImgProc.GetImgPtr();
	rcImgProc = CRect(CPoint(0,0), pParm->rcROISrc.Size());

	plNonFilledAddr = new long[abs(nImgProcHeight * nImgProcWidth)];
	plFillSeedsAddr1 = new long[abs(nImgProcHeight * nImgProcWidth)];
	plFillSeedsAddr2 = new long[abs(nImgProcHeight * nImgProcWidth)];
	pnDir[0] = 1;
	pnDir[1] = -(nPitchProc - 1);
	pnDir[2] = -nPitchProc;
	pnDir[3] = -(nPitchProc + 1);
	pnDir[4] = -1;
	pnDir[5] = nPitchProc - 1;
	pnDir[6] = nPitchProc;
	pnDir[7] = nPitchProc + 1;

	ImgCopy(pImgSrc, &pParm->rcROISrc, &ImgProc, &rcImgProc);

	if(pParm->nBlobType == WHITE) {
		pImgDst->ClrImg(0);
		pImgProcLeftLine=pImgProc;
		pImgProcRightLine=pImgProc + nImgProcWidth - 1;
		for(y = 0; y < nImgProcHeight; y++, pImgProcLeftLine += nPitchProc, pImgProcRightLine += nPitchProc) {
			*pImgProcLeftLine = 0;
			*pImgProcRightLine = 0;
		}
		memset(pImgProc, 0, nImgProcWidth);
		memset(pImgProc + nPitchProc * (nImgProcHeight - 1), 0, nImgProcWidth);
		//pImgProcTopLine = pImgProc;
		//pImgProcBotLine = pImgProc + nPitchProc * (nImgProcHeight - 1);
		//for(x = 0; x < nImgProcWidth; x++) {
		//	*pImgProcTopLine++ = 0;
		//	*pImgProcBotLine++ = 0;
		//}
		pImgProcRow = pImgProcCol = pImgProc;
		for(y = 0; y < nImgProcHeight; y++, pImgProcRow += nPitchProc) {
			for(x = 0, pImgProcCol = pImgProcRow; x < nImgProcWidth; x++, pImgProcCol++) {
				if(*pImgProcCol == 255) {
					ptBlobStartPos = CPoint(x , y);
					if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE)
						nBlobFound = WhiteBlobContour(&ImgProc,
											&ptROITopLeft,
											&ptBlobStartPos, 
											&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
											lProcMode);
					else {
						AdvBlobInfoMore.Clean();
						nBlobFound = WhiteBlobContour(&ImgProc,
											&ptROITopLeft,
											&ptBlobStartPos, 
											&AdvBlobInfoMore,
											lProcMode);
					}
					if(nBlobFound == 0){
						if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE) {
							nBlobCheck = GetBlobFilteringInfo(&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
															&ImgProc,
															pImgDst,	
															&rcSrc,
															pParm,
															lProcMode,
															lDebugFlag);
							if(nBlobCheck == 0)
								m_AdvBlobFindRslt.nFndBlobs++;
							else
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].Clean();
						}
						else {
							nBlobCheck = GetBlobFilteringInfo(&AdvBlobInfoMore,
															&ImgProc,
															pImgDst,	
															&rcSrc,
															pParm,
															lProcMode,
															lDebugFlag);
							if(nBlobCheck == 0) {
								m_AdvBlobFindRslt.nFndBlobs++;
								if(lMemLocBlobNo >= m_AdvBlobFindRslt.nFndBlobs) {
									pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[nAryAdvBlobInfoCol] = &AdvBlobInfoMore;
									nAryAdvBlobInfoCol++;
								}
								else {
									if(m_AdvBlobFindRslt.nFndBlobs >= MAX_BLOB) {
										if ((DEBUG_DRAW&lDebugFlag) || (DEBUG_STEP_MODE&lDebugFlag)) 
											OutputDebugLog("Too many blobs=%d",MAX_BLOB);
										bAbort = true;
										goto Abort;
									}
									pAryAdvBlobBlock[nAryAdvBlobInfoRow].pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
									lMemLocBlobNo += NORMAL_BLOB_SIZE;
									nAryAdvBlobInfoRow++;
									pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[0] = &AdvBlobInfoMore;
									nAryAdvBlobInfoCol = 1;
								}
							}
							else
								AdvBlobInfoMore.Clean();
						}
					}
				}
			}
		} 
	}
	else {
		pImgDst->ClrImg(255);
		pImgProcLeftLine=pImgProc;
		pImgProcRightLine=pImgProc + nImgProcWidth - 1;
		for(y = 0; y < nImgProcHeight; y++, pImgProcLeftLine += nPitchProc, pImgProcRightLine += nPitchProc) {
			*pImgProcLeftLine = 255;
			*pImgProcRightLine = 255;
		}
		memset(pImgProc, 255, nImgProcWidth);
		memset(pImgProc + nPitchProc * (nImgProcHeight - 1), 255, nImgProcWidth);
		//pImgProcTopLine = pImgProc;
		//pImgProcBotLine = pImgProc + nPitchProc * (nImgProcHeight - 1);
		//for(x = 0; x < nImgProcWidth; x++) {
		//	*pImgProcTopLine++ = 255;
		//	*pImgProcBotLine++ = 255;
		//}
		m_AdvBlobFindRslt.nFndBlobs = 0;
		pImgProcRow = pImgProcCol = pImgProc;
		for(y = 0; y < nImgProcHeight; y++, pImgProcRow += nPitchProc) {
			for(x = 0, pImgProcCol = pImgProcRow; x < nImgProcWidth; x++, pImgProcCol++) {
				if(*pImgProcCol == 0) {
					ptBlobStartPos = CPoint(x , y);
					if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE)
						nBlobFound = BlackBlobContour(&ImgProc,
											&ptROITopLeft,
											&ptBlobStartPos, 
											&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
											lProcMode);
					else {
						AdvBlobInfoMore.Clean();
						nBlobFound = BlackBlobContour(&ImgProc,
											&ptROITopLeft,
											&ptBlobStartPos, 
											&AdvBlobInfoMore,
											lProcMode);
					}
					if(nBlobFound == 0){
						if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE) {
							nBlobCheck = GetBlobFilteringInfo(&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
															&ImgProc,
															pImgDst,	
															&rcSrc,
															pParm,
															lProcMode,
															lDebugFlag);
							if(nBlobCheck == 0)
								m_AdvBlobFindRslt.nFndBlobs++;
						}
						else {
							nBlobCheck = GetBlobFilteringInfo(&AdvBlobInfoMore,
															&ImgProc,
															pImgDst,	
															&rcSrc,
															pParm,
															lProcMode,
															lDebugFlag);
							if(nBlobCheck == 0) {
								m_AdvBlobFindRslt.nFndBlobs++;
								if(lMemLocBlobNo >= m_AdvBlobFindRslt.nFndBlobs) {
									pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[nAryAdvBlobInfoCol] = &AdvBlobInfoMore;
									nAryAdvBlobInfoCol++;
								}
								else {
									if(m_AdvBlobFindRslt.nFndBlobs >= MAX_BLOB) {
										if ((DEBUG_DRAW&lDebugFlag) || (DEBUG_STEP_MODE&lDebugFlag)) 
											OutputDebugLog("Too many blobs=%d",MAX_BLOB);
										bAbort = true;
										goto Abort;
									}
									pAryAdvBlobBlock[nAryAdvBlobInfoRow].pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
									lMemLocBlobNo += NORMAL_BLOB_SIZE;
									nAryAdvBlobInfoRow++;
									pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[0] = &AdvBlobInfoMore;
									nAryAdvBlobInfoCol = 1;
								}
							}
						}
					}
				}
			}
		} 
	}

Abort:
	if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_BLOB)
		DrawAdvBlob(pView,pParm,&m_AdvBlobFindRslt);
	ImgProc.Free();
	if(nAryAdvBlobInfoRow) {
		int k;
		int nFoundBlob;
		nFoundBlob = m_AdvBlobFindRslt.nFndBlobs;
		AdvBlobFindRsltBak.pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
		for(x = 0; x < NORMAL_BLOB_SIZE; x++)
			AdvBlobFindRsltBak.pAdvBlobInfo[x] = &m_AdvBlobFindRslt.pAdvBlobInfo[x];
		m_AdvBlobFindRslt.nFndBlobs = NORMAL_BLOB_SIZE;
		m_AdvBlobFindRslt.Clean();
		m_AdvBlobFindRslt.nFndBlobs = nFoundBlob;
		m_AdvBlobFindRslt.pAdvBlobInfo = new ADV_BLOB_INFO[abs(m_AdvBlobFindRslt.nFndBlobs)];
		k = 0;
		for(x = 0; x < NORMAL_BLOB_SIZE; x++)
			m_AdvBlobFindRslt.pAdvBlobInfo[k++] = &AdvBlobFindRsltBak.pAdvBlobInfo[x];

		for(y = 0; y < nAryAdvBlobInfoRow; y++) {
			for(x = 0; x < NORMAL_BLOB_SIZE; x++) {
				if(k < m_AdvBlobFindRslt.nFndBlobs)
					m_AdvBlobFindRslt.pAdvBlobInfo[k++] = &pAryAdvBlobBlock[y].pAdvBlobInfo[x];
				else
					break;
			}
			pAryAdvBlobBlock[y].nFndBlobs = NORMAL_BLOB_SIZE;
			pAryAdvBlobBlock[y].Clean();
			if(k >= m_AdvBlobFindRslt.nFndBlobs)
				break;
		}
	}
	delete [] pAryAdvBlobBlock;
	delete [] plNonFilledAddr;
	delete [] plFillSeedsAddr1;
	delete [] plFillSeedsAddr2;
	if(bAbort)
		return -1;
	else
		return 0;
}

int CAdvBlobFind::GetBlobFilteringInfo(ADV_BLOB_INFO *pAdvBlobInfo,
									   CImageBuffer* pImgBufProc,
									   CImageBuffer* pImgBufDst,	
									   CRect* prcOrg,
									   ADV_BLOB_FIND_PARM *pParm,
									   long lProcMode,
									   long lDebugFlag)
{
	int m, n, k;
	long lm;
	long lIndex;
	CRect rcBlob;
	BOOL bBlobFound;
	long lBlobArea;
	long lBlobFillRowIndex, lBlobFillIndex;
	long lBlobSaveRowIndex;
	BYTE* pImgProc;
	BYTE* pImgDstPtr;
	CPoint ptBlobStartPos;
	int nPitchProc;
	int nPitchDst;
	double dXMoment, dYMoment;

	pImgProc = pImgBufProc->GetImgPtr();
	nPitchProc = pImgBufProc->GetPitch();
	pImgDstPtr = pImgBufDst->GetImgPtr();
	nPitchDst = pImgBufDst->GetPitch();
	rcBlob = pAdvBlobInfo->rcFndBlobPos;
	lBlobFillRowIndex = rcBlob.top * nPitchProc;
	lNonFilledNo = 0;
	if(pParm->nBlobType == WHITE) {
		for(m = rcBlob.top; m < rcBlob.bottom; m++) {
			for(n = rcBlob.left;n < rcBlob.right; n++) {
				lBlobFillIndex = lBlobFillRowIndex + n;
				if(pImgProc[lBlobFillIndex] == 255) {
					if((pImgProc[lBlobFillIndex -1] == 200) || (pImgProc[lBlobFillIndex -nPitchProc] == 200)) 
						pImgProc[lBlobFillIndex] = 200;
					else
						plNonFilledAddr[lNonFilledNo++] = lBlobFillIndex;
				}
			}
			lBlobFillRowIndex += nPitchProc;
		}
		lFillSeedsNo1 = 0;
		for(lm = 0; lm < lNonFilledNo; lm++) {
			lIndex = plNonFilledAddr[lm];
			for(k = 0; k < 8; k++) {
				if(pImgProc[lIndex + pnDir[k]] == 200) {
					pImgProc[lIndex] = 200;
					plFillSeedsAddr1[lFillSeedsNo1++] = lIndex;
					break;
				}
			}
		}
		while(lFillSeedsNo1 > 0) {
			lFillSeedsNo2 = 0;
			for(lm = 0; lm < lFillSeedsNo1; lm++) {
				lIndex = plFillSeedsAddr1[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 255) {
						plFillSeedsAddr2[lFillSeedsNo2++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
			if(lFillSeedsNo2 == 0)
				break;
			lFillSeedsNo1 = 0;
			for(lm = 0; lm < lFillSeedsNo2; lm++) {
				lIndex = plFillSeedsAddr2[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 255) {
						plFillSeedsAddr1[lFillSeedsNo1++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
		}
		lBlobArea = 0;
		if(lProcMode & FIND_BLOB_CENTER) { 
			dXMoment = 0;
			dYMoment = 0;
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						dXMoment += n;
						dYMoment += m;
						lBlobArea++;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
			pAdvBlobInfo->pt2DFndBlobCenter = CPoint2D( dXMoment / lBlobArea + prcOrg->left, dYMoment / lBlobArea + prcOrg->top);
		}
		else {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) 
						lBlobArea++;
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
	}
	else {
		for(m = rcBlob.top; m < rcBlob.bottom; m++) {
			for(n = rcBlob.left;n < rcBlob.right; n++) {
				lBlobFillIndex = lBlobFillRowIndex + n;
				if(pImgProc[lBlobFillIndex] == 0) {
					if((pImgProc[lBlobFillIndex -1] == 200) || (pImgProc[lBlobFillIndex -nPitchProc] == 200)) 
						pImgProc[lBlobFillIndex] = 200;
					else
						plNonFilledAddr[lNonFilledNo++] = lBlobFillIndex;
				}
			}
			lBlobFillRowIndex += nPitchProc;
		}
		lFillSeedsNo1 = 0;
		for(lm = 0; lm < lNonFilledNo; lm++) {
			lIndex = plNonFilledAddr[lm];
			for(k = 0; k < 8; k++) {
				if(pImgProc[lIndex + pnDir[k]] == 200) {
					pImgProc[lIndex] = 200;
					plFillSeedsAddr1[lFillSeedsNo1++] = lIndex;
					break;
				}
			}
		}
		while(lFillSeedsNo1 > 0) {
			lFillSeedsNo2 = 0;
			for(lm = 0; lm < lFillSeedsNo1; lm++) {
				lIndex = plFillSeedsAddr1[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 0) {
						plFillSeedsAddr2[lFillSeedsNo2++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
			if(lFillSeedsNo2 == 0)
				break;
			lFillSeedsNo1 = 0;
			for(lm = 0; lm < lFillSeedsNo2; lm++) {
				lIndex = plFillSeedsAddr2[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 0) {
						plFillSeedsAddr1[lFillSeedsNo1++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
		}
		lBlobArea = 0;
		if(lProcMode & FIND_BLOB_CENTER) { 
			dXMoment = 0;
			dYMoment = 0;
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						dXMoment += n;
						dYMoment += m;
						lBlobArea++;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
			pAdvBlobInfo->pt2DFndBlobCenter = CPoint2D( dXMoment / lBlobArea + prcOrg->left, dYMoment / lBlobArea + prcOrg->top);
		}
		else {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) 
						lBlobArea++;
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
	}
	pAdvBlobInfo->nFndBlobArea = lBlobArea;

	if(lProcMode & FIND_BLOB_ELONGATION) {
		double dWidth;
		if(pAdvBlobInfo->dFndBlobLength) {
			dWidth = pAdvBlobInfo->nFndBlobArea / pAdvBlobInfo->dFndBlobLength;
			if(dWidth < 1) dWidth = 1;
			pAdvBlobInfo->dFndBlobElongation = pAdvBlobInfo->dFndBlobLength / dWidth;
		}
		else pAdvBlobInfo->dFndBlobElongation = 0;
	}
	pAdvBlobInfo->rcFndBlobPos += CPoint(prcOrg->left, prcOrg->top);
	bBlobFound = FoundBlobCheck(pParm, pAdvBlobInfo, lProcMode);
	if(bBlobFound == 0) {
		if(pParm->nBlobType == WHITE) {
			lBlobSaveRowIndex = nPitchDst * (rcBlob.top + prcOrg->top) + prcOrg->left;
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex+n] == 200) {
						pImgDstPtr[lBlobSaveRowIndex + n] = 255;
						pImgProc[lBlobFillRowIndex + n] = 0;
					}
				}
				lBlobFillRowIndex += nPitchProc;
				lBlobSaveRowIndex += nPitchDst;
			}
		}
		else {
			lBlobSaveRowIndex = nPitchDst * (rcBlob.top + prcOrg->top) + prcOrg->left;
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex+n] == 200) {
						pImgDstPtr[lBlobSaveRowIndex + n] = 0;
						pImgProc[lBlobFillRowIndex + n] = 255;
					}
				}
				lBlobFillRowIndex += nPitchProc;
				lBlobSaveRowIndex += nPitchDst;
			}
		}
	}
	else {
		if(pParm->nBlobType == WHITE) {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex+n] == 200) 
						pImgProc[lBlobFillRowIndex + n] = 0;
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
		else {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex+n] == 200) 
						pImgProc[lBlobFillRowIndex + n] = 255;
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
	}
	return bBlobFound;
}

int CAdvBlobFind::GetAdvBlobFiltering4(IMAGEVIEW_INFO *pView,
								CImageBuffer *pImgSrc,	
								CImageBuffer *pImgDst,	
								long* plBlobPixelAddress,
								long lBlobPixelNo,
								ADV_BLOB_FIND_PARM *pParm,
							    long lProcMode,		
								long lDebugFlag)
{
	long li;
	int x, y;
	BYTE *pImgProc, *pImgProcTopLine, *pImgProcBotLine, *pImgProcLeftLine, *pImgProcRightLine;
	BYTE *pImgProcRow, *pImgProcCol;
	BYTE *pImgProcPtr;
	CRect rcBlob;
	int nPitchSrc, nPitchProc;
	CPoint ptBlobStartPos;
	int nImgProcWidth, nImgProcHeight;
	CRect rcSrc;
	CPoint ptROITopLeft;
	ADV_BLOB_FIND_RSLT AdvBlobFindRsltBak;
	ADV_BLOB_FIND_RSLT* pAryAdvBlobBlock;
	int nAryAdvBlobBlock;
	int nImgProcHeightSub1, nImgProcWidthSub1;

	int nAryAdvBlobInfoRow;
	int nAryAdvBlobInfoCol;
	long lMemLocBlobNo;
	ADV_BLOB_INFO AdvBlobInfoMore;
	int nBlobFound;
	int nBlobCheck;
	bool bAbort;
	CRect rcImgSrc;
	long lBlobPixelIndex;

	Clean();
	if(lProcMode & FIND_BLOB_CONTOUR) 
		ptAryFndBlobContour = new CPoint[abs(pImgSrc->GetImgSize().cx * pImgSrc->GetImgSize().cy)];
	if(m_AdvBlobFindRslt.pAdvBlobInfo)
		m_AdvBlobFindRslt.Clean();

	rcImgSrc = pParm->rcROISrc;
	rcSrc = pParm->rcROISrc;
	ptROITopLeft = rcSrc.TopLeft();
	nPitchSrc = pImgSrc->GetPitch();
	nImgProcWidth = rcSrc.Width();
	nImgProcHeight = rcSrc.Height();

	bAbort = false;
	m_AdvBlobFindRslt.nFndBlobs = 0;
	m_lProcMode = lProcMode;

	ptDstSrcOffset = pParm->rcROIDst.TopLeft() - pParm->rcROISrc.TopLeft();

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(&pParm->rcROISrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"pParm->rcROISrc is Invalid");
		return -1;
	}
	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(&pParm->rcROIDst, &(pImgDst->GetImgSize()))) {
		ASSERT(!"pParm->rcROIDst is Invalid");
		return -1;
	}
	if (pParm->rcROISrc.Size() != pParm->rcROIDst.Size()) {
		ASSERT(!"pParm->rcROISrc Size different from pParm->rcROIDst Size");
		return -1;
	}

	if((pParm->rcROISrc.Width() == 0) || (pParm->rcROISrc.Height()==0))
		return 0;

	plNonFilledAddr = new long[abs(nImgProcWidth * nImgProcHeight)];
	plFillSeedsAddr1 = new long[abs(nImgProcWidth * nImgProcHeight)];
	plFillSeedsAddr2 = new long[abs(nImgProcWidth * nImgProcHeight)];
	nAryAdvBlobBlock = MAX_BLOB / NORMAL_BLOB_SIZE + 1;
	pAryAdvBlobBlock = new ADV_BLOB_FIND_RSLT[abs(nAryAdvBlobBlock)];

	m_AdvBlobFindRslt.pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];

	nAryAdvBlobInfoRow = 0;
	nAryAdvBlobInfoCol = 0;
	lMemLocBlobNo = NORMAL_BLOB_SIZE;

	nPitchProc = pImgSrc->GetPitch();
	pImgProcPtr = pImgSrc->GetImgPtr();
	pImgProc = pImgSrc->GetImgPtr() + rcImgSrc.top * nPitchProc + rcImgSrc.left;

	nDir0 = 1;
	nDir1 = -(nPitchProc - 1);
	nDir2 = -nPitchProc;
	nDir3 = -(nPitchProc + 1);
	nDir4 = -1;
	nDir5 = nPitchProc - 1;
	nDir6 = nPitchProc;
	nDir7 = nPitchProc + 1;

	pnDir[0] = 1;
	pnDir[1] = -(nPitchProc - 1);
	pnDir[2] = -nPitchProc;
	pnDir[3] = -(nPitchProc + 1);
	pnDir[4] = -1;
	pnDir[5] = nPitchProc - 1;
	pnDir[6] = nPitchProc;
	pnDir[7] = nPitchProc + 1;
	bBlobFill = FALSE;
	if(pParm->nBlobType == WHITE) {
		pImgDst->ClrImg(0);
		pImgProcLeftLine=pImgProc;
		pImgProcRightLine=pImgProc + nImgProcWidth - 1;
		for(y = 0; y < nImgProcHeight; y++, pImgProcLeftLine += nPitchProc, pImgProcRightLine += nPitchProc) {
			*pImgProcLeftLine = 0;
			*pImgProcRightLine = 0;
		}
		pImgProcTopLine = pImgProc;
		pImgProcBotLine = pImgProc + nPitchProc * (nImgProcHeight - 1);
		for(x = 0; x < nImgProcWidth; x++) {
			*pImgProcTopLine++ = 0;
			*pImgProcBotLine++ = 0;
		}
		pImgProcRow = pImgProcCol = pImgProc + nPitchProc;
		nImgProcHeightSub1 = nImgProcHeight - 1;
		nImgProcWidthSub1 = nImgProcWidth - 1;
		for(li = 0; li < lBlobPixelNo; li++) {
			if(pImgProcPtr[plBlobPixelAddress[li]] == 255) {
				lBlobPixelIndex = plBlobPixelAddress[li];
				x = lBlobPixelIndex % nPitchSrc;
				y = lBlobPixelIndex / nPitchSrc;
				ptBlobStartPos = CPoint(x, y);
				if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE)
					nBlobFound = WhiteBlobContour4(pImgSrc,
										&ptROITopLeft,
										&ptBlobStartPos, 
										&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
										lProcMode);
				else {
					AdvBlobInfoMore.Clean();
					nBlobFound = WhiteBlobContour4(pImgSrc,
										&ptROITopLeft,
										&ptBlobStartPos, 
										&AdvBlobInfoMore,
										lProcMode);
				}
				if(nBlobFound == 0){
					if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE) {
						nBlobCheck = GetBlobFilteringInfo4(&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
									pImgSrc,
									pImgDst,
									pParm,
									lProcMode,
									lDebugFlag);
						if(nBlobCheck == 0) {
							if(lProcMode & FIND_BLOB_CONTOUR) {
								if(m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour)
									delete m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].nBlobContourSize = nFndBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour = new CPoint[abs(nFndBlobContour)];
								for(int kk = 0; kk < nFndBlobContour; kk++)
									m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour[kk] = ptAryFndBlobContour[kk];
							}
							m_AdvBlobFindRslt.nFndBlobs++;
						}
						else
							m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].Clean();
					}
					else {
						nBlobCheck = GetBlobFilteringInfo4(&AdvBlobInfoMore,
									pImgSrc,
									pImgDst,
									pParm,
									lProcMode,
									lDebugFlag);
						if(nBlobCheck == 0) {
							if(lProcMode & FIND_BLOB_CONTOUR) {
								if(m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour)
									delete m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].nBlobContourSize = nFndBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour = new CPoint[abs(nFndBlobContour)];
								for(int kk = 0; kk < nFndBlobContour; kk++)
									m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour[kk] = ptAryFndBlobContour[kk];
							}
							m_AdvBlobFindRslt.nFndBlobs++;
							if(lMemLocBlobNo >= m_AdvBlobFindRslt.nFndBlobs) {
								pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[nAryAdvBlobInfoCol] = &AdvBlobInfoMore;
								nAryAdvBlobInfoCol++;
							}
							else {
								if(m_AdvBlobFindRslt.nFndBlobs >= MAX_BLOB) {
									if ((DEBUG_DRAW&lDebugFlag) || (DEBUG_STEP_MODE&lDebugFlag)) 
										OutputDebugLog("Too many blobs=%d",MAX_BLOB);
									bAbort = true;
									goto Abort;
								}
								pAryAdvBlobBlock[nAryAdvBlobInfoRow].pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
								lMemLocBlobNo += NORMAL_BLOB_SIZE;
								nAryAdvBlobInfoRow++;
								pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[0] = &AdvBlobInfoMore;
								nAryAdvBlobInfoCol = 1;
							}
						}
						else
							AdvBlobInfoMore.Clean();
					}
				}
			}
		} 
	}
	else {
		pImgDst->ClrImg(255);
		pImgProcLeftLine=pImgProc;
		pImgProcRightLine=pImgProc + nImgProcWidth - 1;
		for(y = 0; y < nImgProcHeight; y++, pImgProcLeftLine += nPitchProc, pImgProcRightLine += nPitchProc) {
			*pImgProcLeftLine = 255;
			*pImgProcRightLine = 255;
		}
		pImgProcTopLine = pImgProc;
		pImgProcBotLine = pImgProc + nPitchProc * (nImgProcHeight - 1);
		for(x = 0; x < nImgProcWidth; x++) {
			*pImgProcTopLine++ = 255;
			*pImgProcBotLine++ = 255;
		}
		pImgProcRow = pImgProcCol = pImgProc + nPitchProc;
		nImgProcHeightSub1 = nImgProcHeight - 1;
		nImgProcWidthSub1 = nImgProcWidth - 1;
		for(li = 0; li < lBlobPixelNo; li++) {
			if(pImgProcPtr[plBlobPixelAddress[li]] == 0) {
				lBlobPixelIndex = plBlobPixelAddress[li];
				x = lBlobPixelIndex % nPitchSrc;
				y = lBlobPixelIndex / nPitchSrc;
				ptBlobStartPos = CPoint(x, y);
				if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE)
					nBlobFound = BlackBlobContour4(pImgSrc,
										&ptROITopLeft,
										&ptBlobStartPos, 
										&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
										lProcMode);
				else {
					AdvBlobInfoMore.Clean();
					nBlobFound = BlackBlobContour4(pImgSrc,
										&ptROITopLeft,
										&ptBlobStartPos, 
										&AdvBlobInfoMore,
										lProcMode);
				}
				if(nBlobFound == 0){
					if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE) {
						nBlobCheck = GetBlobFilteringInfo4(&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
									pImgSrc,
									pImgDst,
									pParm,
									lProcMode,
									lDebugFlag);
						if(nBlobCheck == 0) {
							if(lProcMode & FIND_BLOB_CONTOUR) {
								if(m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour)
									delete m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].nBlobContourSize = nFndBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour = new CPoint[abs(nFndBlobContour)];
								for(int kk = 0; kk < nFndBlobContour; kk++)
									m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour[kk] = ptAryFndBlobContour[kk];
							}
							m_AdvBlobFindRslt.nFndBlobs++;
						}
						else
							m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].Clean();
					}
					else {
						nBlobCheck = GetBlobFilteringInfo4(&AdvBlobInfoMore,
									pImgSrc,
									pImgDst,
									pParm,
									lProcMode,
									lDebugFlag);
						if(nBlobCheck == 0) {
							if(lProcMode & FIND_BLOB_CONTOUR) {
								if(m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour)
									delete m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].nBlobContourSize = nFndBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour = new CPoint[abs(nFndBlobContour)];
								for(int kk = 0; kk < nFndBlobContour; kk++)
									m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour[kk] = ptAryFndBlobContour[kk];
							}
							m_AdvBlobFindRslt.nFndBlobs++;
							if(lMemLocBlobNo >= m_AdvBlobFindRslt.nFndBlobs) {
								pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[nAryAdvBlobInfoCol] = &AdvBlobInfoMore;
								nAryAdvBlobInfoCol++;
							}
							else {
								if(m_AdvBlobFindRslt.nFndBlobs >= MAX_BLOB) {
									if ((DEBUG_DRAW&lDebugFlag) || (DEBUG_STEP_MODE&lDebugFlag)) 
										OutputDebugLog("Too many blobs=%d",MAX_BLOB);
									bAbort = true;
									goto Abort;
								}
								pAryAdvBlobBlock[nAryAdvBlobInfoRow].pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
								lMemLocBlobNo += NORMAL_BLOB_SIZE;
								nAryAdvBlobInfoRow++;
								pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[0] = &AdvBlobInfoMore;
								nAryAdvBlobInfoCol = 1;
							}
						}
						else
							AdvBlobInfoMore.Clean();
					}
				}
			}
		}
	}
// Restore Image 
	pImgProc = pImgSrc->GetImgPtr() + rcImgSrc.top * nPitchProc + rcImgSrc.left;
	if(pParm->nBlobType == WHITE) {
		for(li=0; li<lBlobPixelNo;li++)
			pImgProc[plBlobPixelAddress[li]] = 255;
	}
	else {
		for(li=0; li<lBlobPixelNo;li++)
			pImgProc[plBlobPixelAddress[li]] = 0;
	}
// end of Restoring Image 

Abort:


	if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_BLOB)
		DrawAdvBlob(pView,pParm,&m_AdvBlobFindRslt);
	if(nAryAdvBlobInfoRow) {
		int k;
		int nFoundBlob;
		nFoundBlob = m_AdvBlobFindRslt.nFndBlobs;
		AdvBlobFindRsltBak.pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
		for(x = 0; x < NORMAL_BLOB_SIZE; x++)
			AdvBlobFindRsltBak.pAdvBlobInfo[x] = &m_AdvBlobFindRslt.pAdvBlobInfo[x];
		m_AdvBlobFindRslt.nFndBlobs = NORMAL_BLOB_SIZE;
		m_AdvBlobFindRslt.Clean();
		m_AdvBlobFindRslt.nFndBlobs = nFoundBlob;
		m_AdvBlobFindRslt.pAdvBlobInfo = new ADV_BLOB_INFO[abs(m_AdvBlobFindRslt.nFndBlobs)];
		k = 0;
		for(x = 0; x < NORMAL_BLOB_SIZE; x++)
			m_AdvBlobFindRslt.pAdvBlobInfo[k++] = &AdvBlobFindRsltBak.pAdvBlobInfo[x];

		for(y = 0; y < nAryAdvBlobInfoRow; y++) {
			for(x = 0; x < NORMAL_BLOB_SIZE; x++) {
				if(k < m_AdvBlobFindRslt.nFndBlobs)
					m_AdvBlobFindRslt.pAdvBlobInfo[k++] = &pAryAdvBlobBlock[y].pAdvBlobInfo[x];
				else
					break;
			}
			pAryAdvBlobBlock[y].nFndBlobs = NORMAL_BLOB_SIZE;
			pAryAdvBlobBlock[y].Clean();
			if(k >= m_AdvBlobFindRslt.nFndBlobs)
				break;
		}
	}
	delete [] pAryAdvBlobBlock;
	delete [] plNonFilledAddr;
	delete [] plFillSeedsAddr1;
	delete [] plFillSeedsAddr2;


	if(bAbort)
		return -1;
	else
		return 0;
}

int CAdvBlobFind::GetBlobFilteringInfo4(ADV_BLOB_INFO *pAdvBlobInfo,
									   CImageBuffer* pImgBufProc,
									   CImageBuffer* pImgBufDst,	
									   ADV_BLOB_FIND_PARM *pParm,
									   long lProcMode,
									   long lDebugFlag)
{
	int m, n, k;
	long lm;
	long lIndex;
	CRect rcBlob;
	int nBlobFound;
	long lBlobArea;
	long lBlobFillRowIndex, lBlobFillIndex;
	BYTE* pImgProc;
	CPoint ptBlobStartPos;
	int nPitchProc;
	double dXMoment, dYMoment;
	int nBlobRoughCheck;

	pImgProc = pImgBufProc->GetImgPtr();
	nPitchProc = pImgBufProc->GetPitch();
	rcBlob = pAdvBlobInfo->rcFndBlobPos;
	lBlobFillRowIndex = rcBlob.top * nPitchProc;
	lNonFilledNo = 0;
	nBlobRoughCheck = FoundBlobRoughCheck(pParm, pAdvBlobInfo, lProcMode);
	if((nBlobRoughCheck == 0)&& ((lProcMode & FIND_BLOB_LENGTH) || (lProcMode & FIND_BLOB_ELONGATION))) {
		double dlength[6];
		int i;
		CPoint ptLftMost, ptTopMost, ptRhtMost, ptBotMost;
		lIndex = rcBlob.top * nPitchProc + rcBlob.left;
		for(i = rcBlob.top; i < rcBlob.bottom; i++) {
			if(pImgProc[lIndex] == 200) {
				ptLftMost = CPoint(rcBlob.left, i);
				break;
			}
			lIndex += nPitchProc;
		}
		lIndex = rcBlob.top * nPitchProc;
		for(i = rcBlob.left; i < rcBlob.right; i++) {
			if(pImgProc[lIndex + i] == 200) {
				ptTopMost = CPoint(i, rcBlob.top);
				break;
			}
		}
		lIndex = rcBlob.top * nPitchProc + rcBlob.right - 1;
		for(i = rcBlob.top; i < rcBlob.bottom; i++) {
			if(pImgProc[lIndex] == 200) {
				ptRhtMost = CPoint(rcBlob.right, i);
				break;
			}
			lIndex += nPitchProc;
		}
		lIndex = (rcBlob.bottom - 1) * nPitchProc;
		for(i = rcBlob.left; i < rcBlob.right; i++) {
			if(pImgProc[lIndex + i] == 200) {
				ptBotMost = CPoint(i, rcBlob.bottom);
				break;
			}
		}
		dlength[0] = sqrt((double) ((ptTopMost.x - ptLftMost.x) * (ptTopMost.x - ptLftMost.x) + (ptTopMost.y - ptLftMost.y) * (ptTopMost.y - ptLftMost.y)));
		dlength[1] = sqrt((double) ((ptTopMost.x - ptBotMost.x) * (ptTopMost.x - ptBotMost.x) + (ptTopMost.y - ptBotMost.y) * (ptTopMost.y - ptBotMost.y)));
		dlength[2] = sqrt((double) ((ptTopMost.x - ptRhtMost.x) * (ptTopMost.x - ptRhtMost.x) + (ptTopMost.y - ptRhtMost.y) * (ptTopMost.y - ptRhtMost.y)));
		dlength[3] = sqrt((double) ((ptLftMost.x - ptBotMost.x) * (ptLftMost.x - ptBotMost.x) + (ptLftMost.y - ptBotMost.y) * (ptLftMost.y - ptBotMost.y)));
		dlength[4] = sqrt((double) ((ptLftMost.x - ptRhtMost.x) * (ptLftMost.x - ptRhtMost.x) + (ptLftMost.y - ptRhtMost.y) * (ptLftMost.y - ptRhtMost.y)));
		dlength[5] = sqrt((double) ((ptBotMost.x - ptRhtMost.x) * (ptBotMost.x - ptRhtMost.x) + (ptBotMost.y - ptRhtMost.y) * (ptBotMost.y - ptRhtMost.y)));
		pAdvBlobInfo->dFndBlobLength = 0;
		for(i = 0; i < 6; i++) {
			if(dlength[i] > pAdvBlobInfo->dFndBlobLength) pAdvBlobInfo->dFndBlobLength = dlength[i];
		}
	}
	if(pParm->nBlobType == WHITE) {
		for(m = rcBlob.top; m < rcBlob.bottom; m++) {
			for(n = rcBlob.left;n < rcBlob.right; n++) {
				lBlobFillIndex = lBlobFillRowIndex + n;
				if(pImgProc[lBlobFillIndex] == 255) {
					if((pImgProc[lBlobFillIndex -1] == 200) || (pImgProc[lBlobFillIndex -nPitchProc] == 200)) 
						pImgProc[lBlobFillIndex] = 200;
					else
						plNonFilledAddr[lNonFilledNo++] = lBlobFillIndex;
				}
			}
			lBlobFillRowIndex += nPitchProc;
		}

		lFillSeedsNo1 = 0;
		for(lm = 0; lm < lNonFilledNo; lm++) {
			lIndex = plNonFilledAddr[lm];
			for(k = 0; k < 8; k++) {
				if(pImgProc[lIndex + pnDir[k]] == 200) {
					pImgProc[lIndex] = 200;
					plFillSeedsAddr1[lFillSeedsNo1++] = lIndex;
					break;
				}
			}
		}
		while(lFillSeedsNo1 > 0) {
			lFillSeedsNo2 = 0;
			for(lm = 0; lm < lFillSeedsNo1; lm++) {
				lIndex = plFillSeedsAddr1[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 255) {
						plFillSeedsAddr2[lFillSeedsNo2++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
			if(lFillSeedsNo2 == 0)
				break;
			lFillSeedsNo1 = 0;
			for(lm = 0; lm < lFillSeedsNo2; lm++) {
				lIndex = plFillSeedsAddr2[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 255) {
						plFillSeedsAddr1[lFillSeedsNo1++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
		}
		if(nBlobRoughCheck == -1) {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) 
						pImgProc[lBlobFillRowIndex + n] = 0;
				}
				lBlobFillRowIndex += nPitchProc;
			}
			return -1;
		}
		lBlobArea = 0;
		if(lProcMode & FIND_BLOB_CENTER) { 
			dXMoment = 0;
			dYMoment = 0;
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						dXMoment += n;
						dYMoment += m;
						lBlobArea++;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
			pAdvBlobInfo->pt2DFndBlobCenter = CPoint2D( dXMoment / lBlobArea + ptDstSrcOffset.x + pParm->rcROIDst.left, dYMoment / lBlobArea + ptDstSrcOffset.y + pParm->rcROIDst.top);
		}
		else {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						lBlobArea++;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
	}
	else {
		for(m = rcBlob.top; m < rcBlob.bottom; m++) {
			for(n = rcBlob.left;n < rcBlob.right; n++) {
				lBlobFillIndex = lBlobFillRowIndex + n;
				if(pImgProc[lBlobFillIndex] == 0) {
					if((pImgProc[lBlobFillIndex -1] == 200) || (pImgProc[lBlobFillIndex -nPitchProc] == 200)) 
						pImgProc[lBlobFillIndex] = 200;
					else
						plNonFilledAddr[lNonFilledNo++] = lBlobFillIndex;
				}
			}
			lBlobFillRowIndex += nPitchProc;
		}

		lFillSeedsNo1 = 0;
		for(lm = 0; lm < lNonFilledNo; lm++) {
			lIndex = plNonFilledAddr[lm];
			for(k = 0; k < 8; k++) {
				if(pImgProc[lIndex + pnDir[k]] == 200) {
					pImgProc[lIndex] = 200;
					plFillSeedsAddr1[lFillSeedsNo1++] = lIndex;
					break;
				}
			}
		}
		while(lFillSeedsNo1 > 0) {
			lFillSeedsNo2 = 0;
			for(lm = 0; lm < lFillSeedsNo1; lm++) {
				lIndex = plFillSeedsAddr1[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 0) {
						plFillSeedsAddr2[lFillSeedsNo2++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
			if(lFillSeedsNo2 == 0)
				break;
			lFillSeedsNo1 = 0;
			for(lm = 0; lm < lFillSeedsNo2; lm++) {
				lIndex = plFillSeedsAddr2[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 0) {
						plFillSeedsAddr1[lFillSeedsNo1++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
		}
		if(nBlobRoughCheck == -1) {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) 
						pImgProc[lBlobFillRowIndex + n] = 255;
				}
				lBlobFillRowIndex += nPitchProc;
			}
			return -1;
		}
		lBlobArea = 0;
		if(lProcMode & FIND_BLOB_CENTER) { 
			dXMoment = 0;
			dYMoment = 0;
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						dXMoment += n;
						dYMoment += m;
						lBlobArea++;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
			pAdvBlobInfo->pt2DFndBlobCenter = CPoint2D( dXMoment / lBlobArea + ptDstSrcOffset.x + pParm->rcROIDst.left, dYMoment / lBlobArea + ptDstSrcOffset.y + pParm->rcROIDst.top);
		}
		else {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						lBlobArea++;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
	}
	pAdvBlobInfo->nFndBlobArea = lBlobArea;
	if(lProcMode & FIND_BLOB_ELONGATION) {
		double dWidth;
		if(pAdvBlobInfo->dFndBlobLength) {
			dWidth = pAdvBlobInfo->nFndBlobArea / pAdvBlobInfo->dFndBlobLength;
			if(dWidth < 1) dWidth = 1;
			pAdvBlobInfo->dFndBlobElongation = pAdvBlobInfo->dFndBlobLength / dWidth;
		}
		else pAdvBlobInfo->dFndBlobElongation = 0;
	}
	pAdvBlobInfo->rcFndBlobPos += CPoint(ptDstSrcOffset.x + pParm->rcROIDst.left, ptDstSrcOffset.y + pParm->rcROIDst.top);
	nBlobFound = FoundBlobCheck(pParm, pAdvBlobInfo, lProcMode);
	if(nBlobFound == 0) {
		int nPitchDst;
		long lBlobSaveRowIndex, lBlobFillRowIndex;
		BYTE* pImgDstPtr;
		pImgProc = pImgBufProc->GetImgPtr();
		pImgDstPtr = pImgBufDst->GetImgPtr();
		nPitchDst = pImgBufDst->GetPitch();
		lBlobSaveRowIndex = nPitchDst * (rcBlob.top + ptDstSrcOffset.y) + ptDstSrcOffset.x;
		lBlobFillRowIndex = rcBlob.top * nPitchProc;
		if(pParm->nBlobType == WHITE) {
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex+n] == 200) {
						pImgDstPtr[lBlobSaveRowIndex + n] = 255;
						pImgProc[lBlobFillRowIndex + n] = 0;
					}
				}
				lBlobFillRowIndex += nPitchProc;
				lBlobSaveRowIndex += nPitchDst;
			}
		}
		else {
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex+n] == 200) {
						pImgDstPtr[lBlobSaveRowIndex + n] = 0;
						pImgProc[lBlobFillRowIndex + n] = 255;
					}
				}
				lBlobFillRowIndex += nPitchProc;
				lBlobSaveRowIndex += nPitchDst;
			}
		}
	}
	else {
		if(pParm->nBlobType == WHITE) {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex+n] == 200) 
						pImgProc[lBlobFillRowIndex + n] = 0;
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
		else {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex+n] == 200) 
						pImgProc[lBlobFillRowIndex + n] = 255;
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
	}
	return nBlobFound;
}



int CAdvBlobFind::GetAdvBlobFiltering5(IMAGEVIEW_INFO *pView,
									   CImageBuffer *pImgGray,	   
									   CRect* prcImgGray,
									   CImageBuffer *pImgSrc,	
									   CImageBuffer *pImgDst,	
									   long* plBlobPixelAddress,
									   long lBlobPixelNo,
									   ADV_BLOB_FIND_PARM *pParm,
									   long lProcMode,		
									   long lDebugFlag)
{
	long li;
	int x, y;
	BYTE *pImgProc, *pImgProcTopLine, *pImgProcBotLine, *pImgProcLeftLine, *pImgProcRightLine;
	BYTE *pImgProcRow, *pImgProcCol;
	BYTE *pImgProcPtr;
	CRect rcBlob;
	int nPitchSrc, nPitchProc;
	CPoint ptBlobStartPos;
	int nImgProcWidth, nImgProcHeight;
	CRect rcSrc;
	CPoint ptROITopLeft;
	ADV_BLOB_FIND_RSLT AdvBlobFindRsltBak;
	ADV_BLOB_FIND_RSLT* pAryAdvBlobBlock;
	int nAryAdvBlobBlock;
	int nImgProcHeightSub1, nImgProcWidthSub1;

	int nAryAdvBlobInfoRow;
	int nAryAdvBlobInfoCol;
	long lMemLocBlobNo;
	ADV_BLOB_INFO AdvBlobInfoMore;
	int nBlobFound;
	int nBlobCheck;
	bool bAbort;
	CRect rcImgSrc;
	long lBlobPixelIndex;

	Clean();
	if(lProcMode & FIND_BLOB_CONTOUR) 
		ptAryFndBlobContour = new CPoint[abs(pImgSrc->GetImgSize().cx * pImgSrc->GetImgSize().cy)];

	if(m_AdvBlobFindRslt.pAdvBlobInfo)
		m_AdvBlobFindRslt.Clean();

	rcImgSrc = pParm->rcROISrc;
	rcSrc = pParm->rcROISrc;
	ptROITopLeft = rcSrc.TopLeft();
	nPitchSrc = pImgSrc->GetPitch();
	nImgProcWidth = rcSrc.Width();
	nImgProcHeight = rcSrc.Height();

	bAbort = false;
	m_AdvBlobFindRslt.nFndBlobs = 0;
	m_lProcMode = lProcMode;

	ptDstSrcOffset = pParm->rcROIDst.TopLeft() - pParm->rcROISrc.TopLeft();
	if (!pImgGray) {
		ASSERT(!"pImgGray is a bad Pointer");
		return -1;
	}
	if (!pImgGray->IsAllocated()) {
		ASSERT(!"pImgGray is not Allocated");
		return -1;
	}
	if (ValidateRect(prcImgGray, &(pImgGray->GetImgSize()))) {
		ASSERT(!"prcImgGray is Invalid");
		return -1;
	}
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(&pParm->rcROISrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"pParm->rcROISrc is Invalid");
		return -1;
	}
	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(&pParm->rcROIDst, &(pImgDst->GetImgSize()))) {
		ASSERT(!"pParm->rcROIDst is Invalid");
		return -1;
	}
	if (pParm->rcROISrc.Size() != pParm->rcROIDst.Size()) {
		ASSERT(!"pParm->rcROISrc Size different from pParm->rcROIDst Size");
		return -1;
	}
	if (pParm->rcROISrc.Size() != prcImgGray->Size()) {
		ASSERT(!"pParm->rcROISrc Size different from prcImgGray Size");
		return -1;
	}

	if((pParm->rcROISrc.Width() == 0) || (pParm->rcROISrc.Height()==0))
		return 0;

	plNonFilledAddr = new long[abs(nImgProcWidth * nImgProcHeight)];
	plFillSeedsAddr1 = new long[abs(nImgProcWidth * nImgProcHeight)];
	plFillSeedsAddr2 = new long[abs(nImgProcWidth * nImgProcHeight)];
	nAryAdvBlobBlock = MAX_BLOB / NORMAL_BLOB_SIZE + 1;
	pAryAdvBlobBlock = new ADV_BLOB_FIND_RSLT[abs(nAryAdvBlobBlock)];

	m_AdvBlobFindRslt.pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];

	nAryAdvBlobInfoRow = 0;
	nAryAdvBlobInfoCol = 0;
	lMemLocBlobNo = NORMAL_BLOB_SIZE;

	nPitchProc = pImgSrc->GetPitch();
	pImgProcPtr = pImgSrc->GetImgPtr();
	pImgProc = pImgSrc->GetImgPtr() + rcImgSrc.top * nPitchProc + rcImgSrc.left;

	nDir0 = 1;
	nDir1 = -(nPitchProc - 1);
	nDir2 = -nPitchProc;
	nDir3 = -(nPitchProc + 1);
	nDir4 = -1;
	nDir5 = nPitchProc - 1;
	nDir6 = nPitchProc;
	nDir7 = nPitchProc + 1;

	pnDir[0] = 1;
	pnDir[1] = -(nPitchProc - 1);
	pnDir[2] = -nPitchProc;
	pnDir[3] = -(nPitchProc + 1);
	pnDir[4] = -1;
	pnDir[5] = nPitchProc - 1;
	pnDir[6] = nPitchProc;
	pnDir[7] = nPitchProc + 1;
	bBlobFill = FALSE;
	if(pParm->nBlobType == WHITE) {
		pImgDst->ClrImg(0);
		pImgProcLeftLine=pImgProc;
		pImgProcRightLine=pImgProc + nImgProcWidth - 1;
		for(y = 0; y < nImgProcHeight; y++, pImgProcLeftLine += nPitchProc, pImgProcRightLine += nPitchProc) {
			*pImgProcLeftLine = 0;
			*pImgProcRightLine = 0;
		}
		pImgProcTopLine = pImgProc;
		pImgProcBotLine = pImgProc + nPitchProc * (nImgProcHeight - 1);
		for(x = 0; x < nImgProcWidth; x++) {
			*pImgProcTopLine++ = 0;
			*pImgProcBotLine++ = 0;
		}
		pImgProcRow = pImgProcCol = pImgProc + nPitchProc;
		nImgProcHeightSub1 = nImgProcHeight - 1;
		nImgProcWidthSub1 = nImgProcWidth - 1;
		for(li = 0; li < lBlobPixelNo; li++) {
			if(pImgProcPtr[plBlobPixelAddress[li]] == 255) {
				lBlobPixelIndex = plBlobPixelAddress[li];
				x = lBlobPixelIndex % nPitchSrc;
				y = lBlobPixelIndex / nPitchSrc;
				ptBlobStartPos = CPoint(x, y);
				if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE)
					nBlobFound = WhiteBlobContour4(pImgSrc,
										&ptROITopLeft,
										&ptBlobStartPos, 
										&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
										lProcMode);
				else {
					AdvBlobInfoMore.Clean();
					nBlobFound = WhiteBlobContour4(pImgSrc,
										&ptROITopLeft,
										&ptBlobStartPos, 
										&AdvBlobInfoMore,
										lProcMode);
				}
				if(nBlobFound == 0){
					if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE) {
						nBlobCheck = GetBlobFilteringInfo5(&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
									&ptROITopLeft,
									pImgGray,	   
									prcImgGray,
									pImgSrc,
									pImgDst,
									pParm,
									lProcMode,
									lDebugFlag);
						if(nBlobCheck == 0) {
							if(lProcMode & FIND_BLOB_CONTOUR) {
								if(m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour)
									delete m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].nBlobContourSize = nFndBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour = new CPoint[abs(nFndBlobContour)];
								for(int kk = 0; kk < nFndBlobContour; kk++)
									m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour[kk] = ptAryFndBlobContour[kk];
							}
							m_AdvBlobFindRslt.nFndBlobs++;
						}
						else
							m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].Clean();
					}
					else {
						nBlobCheck = GetBlobFilteringInfo5(&AdvBlobInfoMore,
									&ptROITopLeft,
									pImgGray,	   
									prcImgGray,
									pImgSrc,
									pImgDst,
									pParm,
									lProcMode,
									lDebugFlag);
						if(nBlobCheck == 0) {
							if(lProcMode & FIND_BLOB_CONTOUR) {
								if(m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour)
									delete m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].nBlobContourSize = nFndBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour = new CPoint[abs(nFndBlobContour)];
								for(int kk = 0; kk < nFndBlobContour; kk++)
									m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour[kk] = ptAryFndBlobContour[kk];
							}
							m_AdvBlobFindRslt.nFndBlobs++;
							if(lMemLocBlobNo >= m_AdvBlobFindRslt.nFndBlobs) {
								pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[nAryAdvBlobInfoCol] = &AdvBlobInfoMore;
								nAryAdvBlobInfoCol++;
							}
							else {
								if(m_AdvBlobFindRslt.nFndBlobs >= MAX_BLOB) {
									if ((DEBUG_DRAW&lDebugFlag) || (DEBUG_STEP_MODE&lDebugFlag)) 
										OutputDebugLog("Too many blobs=%d",MAX_BLOB);
									bAbort = true;
									goto Abort;
								}
								pAryAdvBlobBlock[nAryAdvBlobInfoRow].pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
								lMemLocBlobNo += NORMAL_BLOB_SIZE;
								nAryAdvBlobInfoRow++;
								pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[0] = &AdvBlobInfoMore;
								nAryAdvBlobInfoCol = 1;
							}
						}
						else
							AdvBlobInfoMore.Clean();
					}
				}
			}
		} 
	}
	else {
		pImgDst->ClrImg(255);
		pImgProcLeftLine=pImgProc;
		pImgProcRightLine=pImgProc + nImgProcWidth - 1;
		for(y = 0; y < nImgProcHeight; y++, pImgProcLeftLine += nPitchProc, pImgProcRightLine += nPitchProc) {
			*pImgProcLeftLine = 255;
			*pImgProcRightLine = 255;
		}
		pImgProcTopLine = pImgProc;
		pImgProcBotLine = pImgProc + nPitchProc * (nImgProcHeight - 1);
		for(x = 0; x < nImgProcWidth; x++) {
			*pImgProcTopLine++ = 255;
			*pImgProcBotLine++ = 255;
		}
		pImgProcRow = pImgProcCol = pImgProc + nPitchProc;
		nImgProcHeightSub1 = nImgProcHeight - 1;
		nImgProcWidthSub1 = nImgProcWidth - 1;
		for(li = 0; li < lBlobPixelNo; li++) {
			if(pImgProcPtr[plBlobPixelAddress[li]] == 0) {
				lBlobPixelIndex = plBlobPixelAddress[li];
				x = lBlobPixelIndex % nPitchSrc;
				y = lBlobPixelIndex / nPitchSrc;
				ptBlobStartPos = CPoint(x, y);
				if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE)
					nBlobFound = BlackBlobContour4(pImgSrc,
										&ptROITopLeft,
										&ptBlobStartPos, 
										&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
										lProcMode);
				else {
					AdvBlobInfoMore.Clean();
					nBlobFound = BlackBlobContour4(pImgSrc,
										&ptROITopLeft,
										&ptBlobStartPos, 
										&AdvBlobInfoMore,
										lProcMode);
				}
				if(nBlobFound == 0){
					if(m_AdvBlobFindRslt.nFndBlobs < NORMAL_BLOB_SIZE) {
						nBlobCheck = GetBlobFilteringInfo5(&m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs],
									&ptROITopLeft,
									pImgGray,	   
									prcImgGray,
									pImgSrc,
									pImgDst,
									pParm,
									lProcMode,
									lDebugFlag);
						if(nBlobCheck == 0) {
							if(lProcMode & FIND_BLOB_CONTOUR) {
								if(m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour)
									delete m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].nBlobContourSize = nFndBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour = new CPoint[abs(nFndBlobContour)];
								for(int kk = 0; kk < nFndBlobContour; kk++)
									m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour[kk] = ptAryFndBlobContour[kk];
							}
							m_AdvBlobFindRslt.nFndBlobs++;
						}
						else
							m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].Clean();
					}
					else {
						nBlobCheck = GetBlobFilteringInfo5(&AdvBlobInfoMore,
									&ptROITopLeft,
									pImgGray,	   
									prcImgGray,
									pImgSrc,
									pImgDst,
									pParm,
									lProcMode,
									lDebugFlag);
						if(nBlobCheck == 0) {
							if(lProcMode & FIND_BLOB_CONTOUR) {
								if(m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour)
									delete m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].nBlobContourSize = nFndBlobContour;
								m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour = new CPoint[abs(nFndBlobContour)];
								for(int kk = 0; kk < nFndBlobContour; kk++)
									m_AdvBlobFindRslt.pAdvBlobInfo[m_AdvBlobFindRslt.nFndBlobs].pptBlobContour[kk] = ptAryFndBlobContour[kk];
							}
							m_AdvBlobFindRslt.nFndBlobs++;
							if(lMemLocBlobNo >= m_AdvBlobFindRslt.nFndBlobs) {
								pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[nAryAdvBlobInfoCol] = &AdvBlobInfoMore;
								nAryAdvBlobInfoCol++;
							}
							else {
								if(m_AdvBlobFindRslt.nFndBlobs >= MAX_BLOB) {
									if ((DEBUG_DRAW&lDebugFlag) || (DEBUG_STEP_MODE&lDebugFlag)) 
										OutputDebugLog("Too many blobs=%d",MAX_BLOB);
									bAbort = true;
									goto Abort;
								}
								pAryAdvBlobBlock[nAryAdvBlobInfoRow].pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
								lMemLocBlobNo += NORMAL_BLOB_SIZE;
								nAryAdvBlobInfoRow++;
								pAryAdvBlobBlock[nAryAdvBlobInfoRow - 1].pAdvBlobInfo[0] = &AdvBlobInfoMore;
								nAryAdvBlobInfoCol = 1;
							}
						}
						else
							AdvBlobInfoMore.Clean();
					}
				}
			}
		}
	}
// Restore Image 
	pImgProc = pImgSrc->GetImgPtr() + rcImgSrc.top * nPitchProc + rcImgSrc.left;
	if(pParm->nBlobType == WHITE) {
		for(li=0; li<lBlobPixelNo;li++)
			pImgProc[plBlobPixelAddress[li]] = 255;
	}
	else {
		for(li=0; li<lBlobPixelNo;li++)
			pImgProc[plBlobPixelAddress[li]] = 0;
	}
// end of Restoring Image 

Abort:


	if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_BLOB)
		DrawAdvBlob(pView,pParm,&m_AdvBlobFindRslt);
	if(nAryAdvBlobInfoRow) {
		int k;
		int nFoundBlob;
		nFoundBlob = m_AdvBlobFindRslt.nFndBlobs;
		AdvBlobFindRsltBak.pAdvBlobInfo = new ADV_BLOB_INFO[NORMAL_BLOB_SIZE];
		for(x = 0; x < NORMAL_BLOB_SIZE; x++)
			AdvBlobFindRsltBak.pAdvBlobInfo[x] = &m_AdvBlobFindRslt.pAdvBlobInfo[x];
		m_AdvBlobFindRslt.nFndBlobs = NORMAL_BLOB_SIZE;
		m_AdvBlobFindRslt.Clean();
		m_AdvBlobFindRslt.nFndBlobs = nFoundBlob;
		m_AdvBlobFindRslt.pAdvBlobInfo = new ADV_BLOB_INFO[abs(m_AdvBlobFindRslt.nFndBlobs)];
		k = 0;
		for(x = 0; x < NORMAL_BLOB_SIZE; x++)
			m_AdvBlobFindRslt.pAdvBlobInfo[k++] = &AdvBlobFindRsltBak.pAdvBlobInfo[x];

		for(y = 0; y < nAryAdvBlobInfoRow; y++) {
			for(x = 0; x < NORMAL_BLOB_SIZE; x++) {
				if(k < m_AdvBlobFindRslt.nFndBlobs)
					m_AdvBlobFindRslt.pAdvBlobInfo[k++] = &pAryAdvBlobBlock[y].pAdvBlobInfo[x];
				else
					break;
			}
			pAryAdvBlobBlock[y].nFndBlobs = NORMAL_BLOB_SIZE;
			pAryAdvBlobBlock[y].Clean();
			if(k >= m_AdvBlobFindRslt.nFndBlobs)
				break;
		}
	}
	delete [] pAryAdvBlobBlock;
	delete [] plNonFilledAddr;
	delete [] plFillSeedsAddr1;
	delete [] plFillSeedsAddr2;


	if(bAbort)
		return -1;
	else
		return 0;
}

int CAdvBlobFind::GetBlobFilteringInfo5(ADV_BLOB_INFO *pAdvBlobInfo,
										CPoint* pptProcWndTopLeft,
										CImageBuffer *pImgGray,	   
										CRect* prcImgGray,
										CImageBuffer* pImgBufProc,
										CImageBuffer* pImgBufDst,	
										ADV_BLOB_FIND_PARM *pParm,
										long lProcMode,
										long lDebugFlag)
{
	int m, n, k;
	long lm;
	long lIndex;
	CRect rcBlob;
	int nBlobFound;
	long lBlobArea;
	long lBlobFillRowIndex, lBlobFillIndex;
	BYTE* pImgProc;
	CPoint ptBlobStartPos;
	long lImgGrayRowIndex;
	int nPitchGray;
	BYTE* pImgGrayPtr;
	long lGraySum;
	int nPitchProc;
	double dXMoment, dYMoment;
	int nBlobRoughCheck;

	pImgProc = pImgBufProc->GetImgPtr();
	nPitchProc = pImgBufProc->GetPitch();
	rcBlob = pAdvBlobInfo->rcFndBlobPos;
	lBlobFillRowIndex = rcBlob.top * nPitchProc;
	lNonFilledNo = 0;
	nBlobRoughCheck = FoundBlobRoughCheck(pParm, pAdvBlobInfo, lProcMode);
	if((nBlobRoughCheck == 0)&& ((lProcMode & FIND_BLOB_LENGTH) || (lProcMode & FIND_BLOB_ELONGATION))) {
		double dlength[6];
		int i;
		CPoint ptLftMost, ptTopMost, ptRhtMost, ptBotMost;
		lIndex = rcBlob.top * nPitchProc + rcBlob.left;
		for(i = rcBlob.top; i < rcBlob.bottom; i++) {
			if(pImgProc[lIndex] == 200) {
				ptLftMost = CPoint(rcBlob.left, i);
				break;
			}
			lIndex += nPitchProc;
		}
		lIndex = rcBlob.top * nPitchProc;
		for(i = rcBlob.left; i < rcBlob.right; i++) {
			if(pImgProc[lIndex + i] == 200) {
				ptTopMost = CPoint(i, rcBlob.top);
				break;
			}
		}
		lIndex = rcBlob.top * nPitchProc + rcBlob.right - 1;
		for(i = rcBlob.top; i < rcBlob.bottom; i++) {
			if(pImgProc[lIndex] == 200) {
				ptRhtMost = CPoint(rcBlob.right, i);
				break;
			}
			lIndex += nPitchProc;
		}
		lIndex = (rcBlob.bottom - 1) * nPitchProc;
		for(i = rcBlob.left; i < rcBlob.right; i++) {
			if(pImgProc[lIndex + i] == 200) {
				ptBotMost = CPoint(i, rcBlob.bottom);
				break;
			}
		}
		dlength[0] = sqrt((double) ((ptTopMost.x - ptLftMost.x) * (ptTopMost.x - ptLftMost.x) + (ptTopMost.y - ptLftMost.y) * (ptTopMost.y - ptLftMost.y)));
		dlength[1] = sqrt((double) ((ptTopMost.x - ptBotMost.x) * (ptTopMost.x - ptBotMost.x) + (ptTopMost.y - ptBotMost.y) * (ptTopMost.y - ptBotMost.y)));
		dlength[2] = sqrt((double) ((ptTopMost.x - ptRhtMost.x) * (ptTopMost.x - ptRhtMost.x) + (ptTopMost.y - ptRhtMost.y) * (ptTopMost.y - ptRhtMost.y)));
		dlength[3] = sqrt((double) ((ptLftMost.x - ptBotMost.x) * (ptLftMost.x - ptBotMost.x) + (ptLftMost.y - ptBotMost.y) * (ptLftMost.y - ptBotMost.y)));
		dlength[4] = sqrt((double) ((ptLftMost.x - ptRhtMost.x) * (ptLftMost.x - ptRhtMost.x) + (ptLftMost.y - ptRhtMost.y) * (ptLftMost.y - ptRhtMost.y)));
		dlength[5] = sqrt((double) ((ptBotMost.x - ptRhtMost.x) * (ptBotMost.x - ptRhtMost.x) + (ptBotMost.y - ptRhtMost.y) * (ptBotMost.y - ptRhtMost.y)));
		pAdvBlobInfo->dFndBlobLength = 0;
		for(i = 0; i < 6; i++) {
			if(dlength[i] > pAdvBlobInfo->dFndBlobLength) pAdvBlobInfo->dFndBlobLength = dlength[i];
		}
	}
	if(pParm->nBlobType == WHITE) {
		for(m = rcBlob.top; m < rcBlob.bottom; m++) {
			for(n = rcBlob.left;n < rcBlob.right; n++) {
				lBlobFillIndex = lBlobFillRowIndex + n;
				if(pImgProc[lBlobFillIndex] == 255) {
					if((pImgProc[lBlobFillIndex -1] == 200) || (pImgProc[lBlobFillIndex -nPitchProc] == 200)) 
						pImgProc[lBlobFillIndex] = 200;
					else
						plNonFilledAddr[lNonFilledNo++] = lBlobFillIndex;
				}
			}
			lBlobFillRowIndex += nPitchProc;
		}

		lFillSeedsNo1 = 0;
		for(lm = 0; lm < lNonFilledNo; lm++) {
			lIndex = plNonFilledAddr[lm];
			for(k = 0; k < 8; k++) {
				if(pImgProc[lIndex + pnDir[k]] == 200) {
					pImgProc[lIndex] = 200;
					plFillSeedsAddr1[lFillSeedsNo1++] = lIndex;
					break;
				}
			}
		}
		while(lFillSeedsNo1 > 0) {
			lFillSeedsNo2 = 0;
			for(lm = 0; lm < lFillSeedsNo1; lm++) {
				lIndex = plFillSeedsAddr1[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 255) {
						plFillSeedsAddr2[lFillSeedsNo2++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
			if(lFillSeedsNo2 == 0)
				break;
			lFillSeedsNo1 = 0;
			for(lm = 0; lm < lFillSeedsNo2; lm++) {
				lIndex = plFillSeedsAddr2[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 255) {
						plFillSeedsAddr1[lFillSeedsNo1++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
		}
		if(nBlobRoughCheck == -1) {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) 
						pImgProc[lBlobFillRowIndex + n] = 0;
				}
				lBlobFillRowIndex += nPitchProc;
			}
			return -1;
		}
		lBlobArea = 0;
		if(lProcMode & FIND_BLOB_CENTER) { 
			dXMoment = 0;
			dYMoment = 0;
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						dXMoment += n;
						dYMoment += m;
						lBlobArea++;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
			pAdvBlobInfo->pt2DFndBlobCenter = CPoint2D( dXMoment / lBlobArea + ptDstSrcOffset.x + pParm->rcROIDst.left, dYMoment / lBlobArea + ptDstSrcOffset.y + pParm->rcROIDst.top);
		}
		else {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						lBlobArea++;
					}
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
	}
	else {
		for(m = rcBlob.top; m < rcBlob.bottom; m++) {
			for(n = rcBlob.left;n < rcBlob.right; n++) {
				lBlobFillIndex = lBlobFillRowIndex + n;
				if(pImgProc[lBlobFillIndex] == 0) {
					if((pImgProc[lBlobFillIndex -1] == 200) || (pImgProc[lBlobFillIndex -nPitchProc] == 200)) 
						pImgProc[lBlobFillIndex] = 200;
					else
						plNonFilledAddr[lNonFilledNo++] = lBlobFillIndex;
				}
			}
			lBlobFillRowIndex += nPitchProc;
		}

		lFillSeedsNo1 = 0;
		for(lm = 0; lm < lNonFilledNo; lm++) {
			lIndex = plNonFilledAddr[lm];
			for(k = 0; k < 8; k++) {
				if(pImgProc[lIndex + pnDir[k]] == 200) {
					pImgProc[lIndex] = 200;
					plFillSeedsAddr1[lFillSeedsNo1++] = lIndex;
					break;
				}
			}
		}
		while(lFillSeedsNo1 > 0) {
			lFillSeedsNo2 = 0;
			for(lm = 0; lm < lFillSeedsNo1; lm++) {
				lIndex = plFillSeedsAddr1[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 0) {
						plFillSeedsAddr2[lFillSeedsNo2++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
			if(lFillSeedsNo2 == 0)
				break;
			lFillSeedsNo1 = 0;
			for(lm = 0; lm < lFillSeedsNo2; lm++) {
				lIndex = plFillSeedsAddr2[lm];
				for(k = 0; k < 8; k++) {
					if(pImgProc[lIndex + pnDir[k]] == 0) {
						plFillSeedsAddr1[lFillSeedsNo1++] = lIndex + pnDir[k];
						pImgProc[lIndex + pnDir[k]] = 200;
					}
				}
			}
		}
		if(nBlobRoughCheck == -1) {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) 
						pImgProc[lBlobFillRowIndex + n] = 255;
				}
				lBlobFillRowIndex += nPitchProc;
			}
			return -1;
		}
		lBlobArea = 0;
		pImgGrayPtr = pImgGray->GetImgPtr(); 
		nPitchGray = pImgGray->GetPitch();
		if(lProcMode & FIND_BLOB_CENTER) { 
			dXMoment = 0;
			dYMoment = 0;
			lImgGrayRowIndex = (prcImgGray->top - pptProcWndTopLeft->y + rcBlob.top) * nPitchGray + prcImgGray->left - pptProcWndTopLeft->x;
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			lGraySum = 0;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						dXMoment += n;
						dYMoment += m;
						lGraySum += pImgGrayPtr[lImgGrayRowIndex + n];
						lBlobArea++;
					}
				}
				lBlobFillRowIndex += nPitchProc;
				lImgGrayRowIndex += nPitchGray;
			}
			pAdvBlobInfo->pt2DFndBlobCenter = CPoint2D( dXMoment / lBlobArea + ptDstSrcOffset.x + pParm->rcROIDst.left, dYMoment / lBlobArea + ptDstSrcOffset.y + pParm->rcROIDst.top);
		}
		else {
			lImgGrayRowIndex = (prcImgGray->top - pptProcWndTopLeft->y + rcBlob.top) * nPitchGray + prcImgGray->left - pptProcWndTopLeft->x;
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			lGraySum = 0;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex + n] == 200) {
						lGraySum += pImgGrayPtr[lImgGrayRowIndex + n];
						lBlobArea++;
					}
				}
				lBlobFillRowIndex += nPitchProc;
				lImgGrayRowIndex += nPitchGray;
			}
		}
	}
	pAdvBlobInfo->nFndBlobArea = lBlobArea;
	if(lBlobArea > 0)
		pAdvBlobInfo->nBlobAvgIntensity = lGraySum / lBlobArea;
	else
		pAdvBlobInfo->nBlobAvgIntensity = 0;

	if(lProcMode & FIND_BLOB_ELONGATION) {
		double dWidth;
		if(pAdvBlobInfo->dFndBlobLength) {
			dWidth = pAdvBlobInfo->nFndBlobArea / pAdvBlobInfo->dFndBlobLength;
			if(dWidth < 1) dWidth = 1;
			pAdvBlobInfo->dFndBlobElongation = pAdvBlobInfo->dFndBlobLength / dWidth;
		}
		else pAdvBlobInfo->dFndBlobElongation = 0;
	}
	pAdvBlobInfo->rcFndBlobPos += CPoint(ptDstSrcOffset.x + pParm->rcROIDst.left, ptDstSrcOffset.y + pParm->rcROIDst.top);
	nBlobFound = FoundBlobCheck(pParm, pAdvBlobInfo, lProcMode);
	if(nBlobFound == 0) {
		int nPitchDst;
		long lBlobSaveRowIndex, lBlobFillRowIndex;
		BYTE* pImgDstPtr;
		pImgProc = pImgBufProc->GetImgPtr();
		pImgDstPtr = pImgBufDst->GetImgPtr();
		nPitchDst = pImgBufDst->GetPitch();
		lBlobSaveRowIndex = nPitchDst * (rcBlob.top + ptDstSrcOffset.y) + ptDstSrcOffset.x;
		lBlobFillRowIndex = rcBlob.top * nPitchProc;
		if(pParm->nBlobType == WHITE) {
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex+n] == 200) {
						pImgDstPtr[lBlobSaveRowIndex + n] = 255;
						pImgProc[lBlobFillRowIndex + n] = 0;
					}
				}
				lBlobFillRowIndex += nPitchProc;
				lBlobSaveRowIndex += nPitchDst;
			}
		}
		else {
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex+n] == 200) {
						pImgDstPtr[lBlobSaveRowIndex + n] = 0;
						pImgProc[lBlobFillRowIndex + n] = 255;
					}
				}
				lBlobFillRowIndex += nPitchProc;
				lBlobSaveRowIndex += nPitchDst;
			}
		}
	}
	else {
		if(pParm->nBlobType == WHITE) {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex+n] == 200) 
						pImgProc[lBlobFillRowIndex + n] = 0;
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
		else {
			lBlobFillRowIndex = rcBlob.top * nPitchProc;
			for(m = rcBlob.top;m < rcBlob.bottom; m++) {
				for(n = rcBlob.left;n < rcBlob.right; n++) {
					if(pImgProc[lBlobFillRowIndex+n] == 200) 
						pImgProc[lBlobFillRowIndex + n] = 255;
				}
				lBlobFillRowIndex += nPitchProc;
			}
		}
	}
	return nBlobFound;
}

BOOL CAdvBlobFind::FoundBlobCheck(ADV_BLOB_FIND_PARM *pParm,
					ADV_BLOB_INFO *FoundBlobRslt,
					long lProcMode)	
{
	if((pParm->nMinXBlobSize		> FoundBlobRslt->rcFndBlobPos.Width())  || (pParm->nMaxXBlobSize		< FoundBlobRslt->rcFndBlobPos.Width()))		return -1;
	if((pParm->nMinYBlobSize		> FoundBlobRslt->rcFndBlobPos.Height()) || (pParm->nMaxYBlobSize		< FoundBlobRslt->rcFndBlobPos.Height()))	return -1;
	if((pParm->nMinBlobArea			> FoundBlobRslt->nFndBlobArea)			|| (pParm->nMaxBlobArea			< FoundBlobRslt->nFndBlobArea))				return -1;
	if(lProcMode & FIND_BLOB_MAX_PROJ_LENGTH) {
		int nMaxProjLength;
		double dLength;
		double dWidthAvg;
		double dFndBlobElongation;
		nMaxProjLength = __max(FoundBlobRslt->rcFndBlobPos.Width(),FoundBlobRslt->rcFndBlobPos.Height());
		if(((pParm->dMinBlobLength - INT_AND_DBL_EQ_TOL) > nMaxProjLength) || ((pParm->dMaxBlobLength + INT_AND_DBL_EQ_TOL)	< nMaxProjLength))			
			return -1;
		dLength = sqrt((double) (FoundBlobRslt->rcFndBlobPos.Width() * FoundBlobRslt->rcFndBlobPos.Width() + 
				FoundBlobRslt->rcFndBlobPos.Height() * FoundBlobRslt->rcFndBlobPos.Height()));
		if(dLength < 1)
			return -1;
		dWidthAvg = FoundBlobRslt->nFndBlobArea/dLength;
		if(dWidthAvg < 1)
			dWidthAvg = 1;
		dFndBlobElongation = dLength/dWidthAvg;
		if(lProcMode & FIND_BLOB_ELONGATION)
			if(((pParm->dMinBlobElongation - INT_AND_DBL_EQ_TOL)	> dFndBlobElongation)|| ((pParm->dMaxBlobElongation + INT_AND_DBL_EQ_TOL)	< dFndBlobElongation))		return -1;
	}
	if(lProcMode & FIND_BLOB_LENGTH)
		if(((pParm->dMinBlobLength - INT_AND_DBL_EQ_TOL)	> FoundBlobRslt->dFndBlobLength)	|| ((pParm->dMaxBlobLength + INT_AND_DBL_EQ_TOL)		< FoundBlobRslt->dFndBlobLength))			return -1;
	if(lProcMode & FIND_BLOB_ELONGATION)
		if(((pParm->dMinBlobElongation - INT_AND_DBL_EQ_TOL)	> FoundBlobRslt->dFndBlobElongation)|| ((pParm->dMaxBlobElongation + INT_AND_DBL_EQ_TOL)	< FoundBlobRslt->dFndBlobElongation))		return -1;
	return 0;
}

int CAdvBlobFind::FoundBlobRoughCheck(ADV_BLOB_FIND_PARM *pParm,
					ADV_BLOB_INFO *FoundBlobRslt,
					long lProcMode)	
{
	if(lProcMode & FIND_BLOB_RECT) {
		if((pParm->nMinXBlobSize		> FoundBlobRslt->rcFndBlobPos.Width())  || (pParm->nMaxXBlobSize		< FoundBlobRslt->rcFndBlobPos.Width()))		return -1;
		if((pParm->nMinYBlobSize		> FoundBlobRslt->rcFndBlobPos.Height()) || (pParm->nMaxYBlobSize		< FoundBlobRslt->rcFndBlobPos.Height()))	return -1;
		if(lProcMode & FIND_BLOB_AREA) {
			long lEstimatedMaxArea;
			lEstimatedMaxArea = FoundBlobRslt->rcFndBlobPos.Width() * FoundBlobRslt->rcFndBlobPos.Height();
			if(pParm->nMinBlobArea		> lEstimatedMaxArea)	return -1;
		}
		if(lProcMode & FIND_BLOB_LENGTH) {
			if(pParm->dMinBlobLength	> (FoundBlobRslt->rcFndBlobPos.Width() + FoundBlobRslt->rcFndBlobPos.Height()))		return -1;
		}
		if(lProcMode & FIND_BLOB_ELONGATION) {
			if(pParm->dMinBlobElongation	> (FoundBlobRslt->rcFndBlobPos.Width() + FoundBlobRslt->rcFndBlobPos.Height()))		return -1;
		}
		if(lProcMode & FIND_BLOB_MAX_PROJ_LENGTH) {
			int nMaxProjLength;
			double dLength;
			double dWidthAvg;
			double dFndBlobElongation;
			nMaxProjLength = __max(FoundBlobRslt->rcFndBlobPos.Width(),FoundBlobRslt->rcFndBlobPos.Height());
			if(((pParm->dMinBlobLength - INT_AND_DBL_EQ_TOL) > nMaxProjLength) || ((pParm->dMaxBlobLength + INT_AND_DBL_EQ_TOL)	< nMaxProjLength))			
				return -1;
			dLength = sqrt((double) (FoundBlobRslt->rcFndBlobPos.Width() * FoundBlobRslt->rcFndBlobPos.Width() + 
					FoundBlobRslt->rcFndBlobPos.Height() * FoundBlobRslt->rcFndBlobPos.Height()));
			if(dLength < 1)
				return -1;
			dWidthAvg = FoundBlobRslt->nFndBlobArea/dLength;
			if(dWidthAvg < 1)
				dWidthAvg = 1;
			dFndBlobElongation = dLength/dWidthAvg;
			if(lProcMode & FIND_BLOB_ELONGATION)
				if(((pParm->dMinBlobElongation - INT_AND_DBL_EQ_TOL)	> dFndBlobElongation)|| ((pParm->dMaxBlobElongation + INT_AND_DBL_EQ_TOL)	< dFndBlobElongation))		return -1;
		}
	}
	return 0;
}

int CAdvBlobFind::WhiteHoleContour(CImageBuffer* pImg,
								   CRect* prcBlob,
								   CPoint* pptROITopLeft,
								   CPoint* pptBlobStartPos,
								   CPoint* pptHoleContour,
								   int* pnptHoleContour,
								   CRect* prcHoleFound,
								   BYTE byteProcFill,
								   BYTE byteFill,
								   BOOL* pbHole)	

{
	int i;
	int nDir, nCount;
	int nDirOrg, nCountOrg;
	int m;
	long lIndexStart, lIndex;
	int nPitch;
	BYTE *pImgPtr;
	int pnDir[4];
	int nptHoleContour;
	int nPositionX, nPositionY;
	int nMinLeft;
	int nMaxRight;
	int nMinTop;
	int nMaxBottom;
	int nBlobRefLft;
	int nBlobRefTop;
	int nBlobRefRht;
	int nBlobRefBot;
	*pbHole = TRUE;
	nBlobRefLft = prcBlob->left - pptBlobStartPos->x;
	nBlobRefTop = 0;
	nBlobRefRht = prcBlob->right - pptBlobStartPos->x - 1;
	nBlobRefBot = prcBlob->bottom - pptBlobStartPos->y - 1;
	nMinLeft = 0;
	nMaxRight = 0;
	nMinTop = 0;
	nMaxBottom = 0;
	pImgPtr = pImg->GetImgPtr();
	nPitch = pImg->GetPitch();
	pnDir[0] = 1;
	pnDir[1] = -nPitch;
	pnDir[2] = -1;
	pnDir[3] = nPitch;
	nCount = 0;
	BOOL bFirstPoint =  1;             
	lIndex = pptBlobStartPos->y * nPitch + pptBlobStartPos->x;
	lIndexStart = lIndex;
	pImgPtr[lIndex] = byteProcFill;
	nDir = 3;
	nptHoleContour = 0;
    nPositionX = 0;
	nPositionY = 0;
	pptHoleContour[nptHoleContour++] = CPoint(pptBlobStartPos->x, pptBlobStartPos->y);
	nMinLeft = 0;
	nMaxRight = 0;
	nMinTop = 0;
	nMaxBottom = 0;
	do {
		nDirOrg = nDir;
		nCountOrg = nCount;
		for(i=0;i<4;i++) {
			if(pImgPtr[lIndex+pnDir[nDir]] >= byteProcFill) {
				if(pImgPtr[lIndex+pnDir[nDir]] == byteFill) {
					*pbHole = FALSE;
					goto TerminateProc;
				}
				switch (nDir) {
				case 0:
					if(nPositionX == nBlobRefRht) {
						*pbHole = FALSE;
						goto TerminateProc;
					}
					nPositionX++;
					if(nPositionX >= nMaxRight)
						nMaxRight = nPositionX;
					break;
				case 1:
					if(nPositionY == nBlobRefTop) {
						*pbHole = FALSE;
						goto TerminateProc;
					}
					nPositionY--;
					if(nPositionY <= nMinTop) 
						nMinTop = nPositionY;
					break;
				case 2:
					if(nPositionX == nBlobRefLft) {
						*pbHole = FALSE;
						goto TerminateProc;
					}
					nPositionX--;
					if(nPositionX <= nMinLeft) 
						nMinLeft = nPositionX;
					break;
				case 3:
					if(nPositionY == nBlobRefBot) {
						*pbHole = FALSE;
						goto TerminateProc;
					}
					nPositionY++;
					if(nPositionY >= nMaxBottom) 
						nMaxBottom = nPositionY;
					break;
				}
				lIndex += pnDir[nDir];
				pptHoleContour[nptHoleContour++] = CPoint(pptBlobStartPos->x + nPositionX,
															pptBlobStartPos->y + nPositionY);
				pImgPtr[lIndex] = byteProcFill;
				bFirstPoint = 0;             
				nDir--;
				if(nDir<0) nDir += 4;
				break;
			}
			else {
				nDir++;
				if(nDir>3) nDir -= 4;
			}
		}
		if(lIndex == lIndexStart) {
		    bFirstPoint = FALSE;
			if((pptBlobStartPos->x != (prcBlob->right - 1)) && 
				(pptBlobStartPos->x != (prcBlob->bottom - 1))) {
				if((pImgPtr[lIndexStart + 1] == 255) ||
				   (pImgPtr[lIndexStart + nPitch] == 255))
				   bFirstPoint = TRUE;
			}
			else {
				if(pptBlobStartPos->x != (prcBlob->right - 1)) {
					if(pImgPtr[lIndexStart + 1] == 255)
						bFirstPoint = TRUE;
				}
				if(pptBlobStartPos->y != (prcBlob->bottom - 1)) {
					if(pImgPtr[lIndexStart + nPitch] == 255)
						bFirstPoint = TRUE;
				}
			}
		}
	} while(lIndex != lIndexStart || bFirstPoint); 
TerminateProc:
	for(m=0;m<nptHoleContour;m++) {
		lIndex = pptHoleContour[m].y * nPitch + pptHoleContour[m].x;
		pImgPtr[lIndex] = byteFill;
	}
	for(m=0;m<nptHoleContour;m++) {
		pptHoleContour[m] = *pptROITopLeft + pptHoleContour[m];
	}
	*pnptHoleContour = nptHoleContour;
	*prcHoleFound = CRect(nMinLeft + pptBlobStartPos->x, 
						  nMinTop + pptBlobStartPos->y,
						  nMaxRight + pptBlobStartPos->x + 1,
						  nMaxBottom + pptBlobStartPos->y + 1);
	return 0;
}


int CAdvBlobFind::BlackHoleContour(CImageBuffer* pImg,
								   CRect* prcBlob,
								   CPoint* pptROITopLeft,
								   CPoint* pptBlobStartPos,
								   CPoint* pptHoleContour,
								   int* pnptHoleContour,
								   CRect* prcHoleFound,
								   BYTE byteProcFill,
								   BYTE byteFill,
								   BOOL* pbHole)	

{
	int i;
	int nDir, nCount;
	int nDirOrg, nCountOrg;
	int m;
	long lIndexStart, lIndex;
	int nPitch;
	BYTE *pImgPtr;
	int pnDir[4];
	int nptHoleContour;
	int nPositionX, nPositionY;
	int nMinLeft;
	int nMaxRight;
	int nMinTop;
	int nMaxBottom;
	int nBlobRefLft;
	int nBlobRefTop;
	int nBlobRefRht;
	int nBlobRefBot;
	*pbHole = TRUE;
	nBlobRefLft = prcBlob->left - pptBlobStartPos->x;
	nBlobRefTop = 0;
	nBlobRefRht = prcBlob->right - pptBlobStartPos->x - 1;
	nBlobRefBot = prcBlob->bottom - pptBlobStartPos->y - 1;
	nMinLeft = 0;
	nMaxRight = 0;
	nMinTop = 0;
	nMaxBottom = 0;
	pImgPtr = pImg->GetImgPtr();
	nPitch = pImg->GetPitch();
	pnDir[0] = 1;
	pnDir[1] = -nPitch;
	pnDir[2] = -1;
	pnDir[3] = nPitch;
	nCount = 0;
	BOOL bFirstPoint =  1;             
	lIndex = pptBlobStartPos->y * nPitch + pptBlobStartPos->x;
	lIndexStart = lIndex;
	pImgPtr[lIndex] = byteProcFill;
	nDir = 3;
	nptHoleContour = 0;
    nPositionX = 0;
	nPositionY = 0;
	pptHoleContour[nptHoleContour++] = CPoint(pptBlobStartPos->x,
											  pptBlobStartPos->y);
	nMinLeft = 0;
	nMaxRight = 0;
	nMinTop = 0;
	nMaxBottom = 0;
	do {
		nDirOrg = nDir;
		nCountOrg = nCount;
		for(i=0;i<4;i++) {
			if(pImgPtr[lIndex+pnDir[nDir]] <= byteProcFill) {
				if(pImgPtr[lIndex+pnDir[nDir]] == byteFill) {
					*pbHole = FALSE;
					goto TerminateProc;
				}
				switch (nDir) {
				case 0:
					if(nPositionX == nBlobRefRht) {
						*pbHole = FALSE;
						goto TerminateProc;
					}
					nPositionX++;
					if(nPositionX >= nMaxRight)
						nMaxRight = nPositionX;
					break;
				case 1:
					if(nPositionY == nBlobRefTop) {
						*pbHole = FALSE;
						goto TerminateProc;
					}
					nPositionY--;
					if(nPositionY <= nMinTop) 
						nMinTop = nPositionY;
					break;
				case 2:
					if(nPositionX == nBlobRefLft) {
						*pbHole = FALSE;
						goto TerminateProc;
					}
					nPositionX--;
					if(nPositionX <= nMinLeft) 
						nMinLeft = nPositionX;
					break;
				case 3:
					if(nPositionY == nBlobRefBot) {
						*pbHole = FALSE;
						goto TerminateProc;
					}
					nPositionY++;
					if(nPositionY >= nMaxBottom) 
						nMaxBottom = nPositionY;
					break;
				}
				lIndex += pnDir[nDir];
				pptHoleContour[nptHoleContour++] = CPoint(pptBlobStartPos->x + nPositionX,
															pptBlobStartPos->y + nPositionY);
				pImgPtr[lIndex] = byteProcFill;
				bFirstPoint = 0;             
				nDir--;
				if(nDir<0) nDir += 4;
				break;
			}
			else {
				nDir++;
				if(nDir>3) nDir -= 4;
			}
		}
		if(lIndex == lIndexStart) {
		    bFirstPoint = FALSE;
			if((pptBlobStartPos->x != (prcBlob->right - 1)) && 
				(pptBlobStartPos->x != (prcBlob->bottom - 1))) {
				if((pImgPtr[lIndexStart + 1] == 0) ||
				   (pImgPtr[lIndexStart + nPitch] == 0))
				   bFirstPoint = TRUE;
			}
			else {
				if(pptBlobStartPos->x != (prcBlob->right - 1)) {
					if(pImgPtr[lIndexStart + 1] == 0)
						bFirstPoint = TRUE;
				}
				if(pptBlobStartPos->y != (prcBlob->bottom - 1)) {
					if(pImgPtr[lIndexStart + nPitch] == 0)
						bFirstPoint = TRUE;
				}
			}
		}
	} while(lIndex != lIndexStart || bFirstPoint); 
TerminateProc:
	for(m=0;m<nptHoleContour;m++) {
		lIndex = pptHoleContour[m].y * nPitch + pptHoleContour[m].x;
		pImgPtr[lIndex] = byteFill;
	}
	for(m=0;m<nptHoleContour;m++) {
		pptHoleContour[m] = *pptROITopLeft + pptHoleContour[m];
	}
	*pnptHoleContour = nptHoleContour;
	*prcHoleFound = CRect(nMinLeft + pptBlobStartPos->x, 
						  nMinTop + pptBlobStartPos->y,
						  nMaxRight + pptBlobStartPos->x + 1,
						  nMaxBottom + pptBlobStartPos->y + 1);

	return 0;
}


int CAdvBlobFind::WhiteBlobContour(CImageBuffer *pImg,
								   CPoint* pptROITopLeft,
								   CPoint *pptBlobStartPos,
								   ADV_BLOB_INFO *FoundBlobInfo,
								   long lProcMode)	

{
	int nDir, nCount;
	int nMinLeft, nMaxRight, nMinTop, nMaxBottom, nPositionX, nPositionY;
	int nMinLeftBak, nMaxRightBak, nMinTopBak, nMaxBottomBak;
	long lIndexStart, lIndex;
	int nPitch;
	CPoint ptBlobStartPos;
	BYTE *pImgPtr;
	int pnDir[8];
	long lFirstIndexStart;
	BOOL bComplete;
	BOOL bFirstPoint;             
	CPoint ptTopMostBak, ptBotMostBak, ptLftMostBak, ptRhtMostBak;
	CPoint2D pt2DTopMost, pt2DBotMost, pt2DLftMost, pt2DRhtMost;
	int nTopMostX, nBotMostX, nLftMostY, nRhtMostY;
	BOOL bFindBlobContour;
	long lSecondContourPtAddress;
	CPoint ptBlobNotFillPos;

	bFindBlobContour = lProcMode & FIND_BLOB_CONTOUR;
	pImgPtr = pImg->GetImgPtr();
	nPitch = pImg->GetPitch();

	pnDir[0] = 1;
	pnDir[1] = -(nPitch - 1);
	pnDir[2] = -nPitch;
	pnDir[3] = -(nPitch + 1);
	pnDir[4] = -1;
	pnDir[5] = nPitch - 1;
	pnDir[6] = nPitch;
	pnDir[7] = nPitch + 1;

	nMinLeft = 0;
	nMaxRight = 0;
	nMinTop = 0;
	nMaxBottom = 0;
	nMinLeftBak = 0;
	nMaxRightBak = 0;
	nMinTopBak = 0;
	nMaxBottomBak = 0;
    nPositionX = 0;
	nPositionY = 0;

	nFndBlobContour = 0;
	if((lProcMode & FIND_BLOB_LENGTH) || (lProcMode & FIND_BLOB_ELONGATION)) {
		CPoint ptTopMost, ptBotMost, ptLftMost, ptRhtMost;
		double dlength[6];
		int i;
		ptTopMost = CPoint(0,0);
		ptBotMost = CPoint(0,0);
		ptLftMost = CPoint(0,0);
		ptRhtMost = CPoint(0,0);
		ptTopMostBak = CPoint(0,0);
		ptBotMostBak = CPoint(0,0);
		ptLftMostBak = CPoint(0,0);
		ptRhtMostBak = CPoint(0,0);
		nTopMostX = 0;
		nBotMostX = 0;
		nLftMostY = 0;
		nRhtMostY = 0;
		if(bFindBlobContour) 
			ptAryFndBlobContour[nFndBlobContour++] = CPoint(pptBlobStartPos->x + pptROITopLeft->x, pptBlobStartPos->y + pptROITopLeft->y);
		ptBlobStartPos = *pptBlobStartPos;
		bComplete = FALSE;
		lFirstIndexStart = ptBlobStartPos.y * nPitch + ptBlobStartPos.x; 
		lSecondContourPtAddress = -1;
		do {
			nCount = 0;
			nDir = 5;
			bFirstPoint =  1;             
//			lIndexStart = ptBlobStartPos.y * nPitch + ptBlobStartPos.x;
			lIndexStart = lFirstIndexStart;
			pImgPtr[lIndexStart] = 200;
			lIndex = lIndexStart;
			do {
				if(pImgPtr[lIndex+pnDir[nDir]] > 0) {
					switch (nDir) {
						case 0: 
							nPositionX++;
							if(nPositionX >= nMaxRight)	{
								nMaxRight = nPositionX;
								nRhtMostY = nPositionY;
							}
							break;
						case 1: 
							nPositionX++;
							if(nPositionX >= nMaxRight) {
								nMaxRight = nPositionX;
								nRhtMostY = nPositionY;
							}
							nPositionY--;
							if(nPositionY <= nMinTop) {
								nMinTop = nPositionY;
								nTopMostX = nPositionX;
							}
							break; 
						case 2: 
							nPositionY--;
							if(nPositionY <= nMinTop) {
								nMinTop = nPositionY;
								nTopMostX = nPositionX;
							}
							break;
						case 3: 
							nPositionX--;
							if(nPositionX <= nMinLeft) {
								nMinLeft = nPositionX;
								nLftMostY = nPositionY;
							}
							nPositionY--;
							if(nPositionY <= nMinTop) {
								nMinTop = nPositionY; 
								nTopMostX = nPositionX;
							}
							break;
						case 4: 
							nPositionX--;
							if(nPositionX <= nMinLeft) {
								nMinLeft = nPositionX;
								nLftMostY = nPositionY;
							}
							break;
						case 5: 
							nPositionX--;
							if(nPositionX <= nMinLeft) {
								nMinLeft = nPositionX;
								nLftMostY = nPositionY;
							}
							nPositionY++;
							if(nPositionY >= nMaxBottom) {
								nMaxBottom = nPositionY;
								nBotMostX = nPositionX;
							}
							break;
						case 6:
							nPositionY++;
							if(nPositionY >= nMaxBottom) {
								nMaxBottom = nPositionY;
								nBotMostX = nPositionX;
							}
							break;
						case 7:	
							nPositionX++;
							if(nPositionX >= nMaxRight)	{
								nMaxRight = nPositionX;
								nRhtMostY = nPositionY;
							}
							nPositionY++;
							if(nPositionY >= nMaxBottom) {
								nMaxBottom = nPositionY;
								nBotMostX = nPositionX;
							}
							break;
					}
					nCount = 0;
					lIndex += pnDir[nDir];

					if(bFindBlobContour) 
						ptAryFndBlobContour[nFndBlobContour++] = CPoint(pptBlobStartPos->x + nPositionX + pptROITopLeft->x,
																pptBlobStartPos->y + nPositionY + pptROITopLeft->x);

					bFirstPoint = 0;             
					pImgPtr[lIndex] = 200;
					nDir -= 2;
					if(nDir<0) nDir += 8;
				}
				else {
					nDir++;
					if(nDir>7) nDir -= 8;
					nCount++;
				}
				if(nCount>8) {
					pImgPtr[lIndex] = 200;
					break;
				}
			} while(lIndex != lIndexStart || bFirstPoint); 
			bComplete = ContourCompleteCheck(WHITE,
											 pImgPtr[lFirstIndexStart + nPitch - 1],
											 pImgPtr[lFirstIndexStart + nPitch],										 
											 pImgPtr[lFirstIndexStart + nPitch + 1], 
											 pImgPtr[lFirstIndexStart + 1],
											 &ptBlobNotFillPos);
			if(!bComplete) {
				if(pImgPtr[lFirstIndexStart + nPitch - 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch - 1;
				if(pImgPtr[lFirstIndexStart + nPitch] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch;
				if(pImgPtr[lFirstIndexStart + nPitch + 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch + 1;
				if(pImgPtr[lFirstIndexStart + 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + 1;
				pImgPtr[lSecondContourPtAddress] = 0;
				nMinLeftBak = nMinLeft;
				nMaxRightBak = nMaxRight;
				nMinTopBak = nMinTop;
				nMaxBottomBak = nMaxBottom;
				ptTopMost = CPoint(nTopMostX, nMinTop);
				ptBotMost = CPoint(nBotMostX, nMaxBottom);
				ptLftMost = CPoint(nMinLeft, nLftMostY);
				ptRhtMost = CPoint(nMaxRight, nRhtMostY);
				ptTopMostBak = ptTopMost;
				ptBotMostBak = ptBotMost;
				ptLftMostBak = ptLftMost;
				ptRhtMostBak = ptRhtMost;
				nMinLeft = ptBlobStartPos.x - pptBlobStartPos->x;
				nMaxRight = ptBlobStartPos.x - pptBlobStartPos->x;
				nMinTop = ptBlobStartPos.y - pptBlobStartPos->y;
				nMaxBottom = ptBlobStartPos.y - pptBlobStartPos->y;
				nPositionX = nMinLeft;
				nPositionY = nMinTop;
				ptTopMost = CPoint(nPositionX, nPositionY);
				ptBotMost = CPoint(nPositionX, nPositionY);
				ptLftMost = CPoint(nPositionX, nPositionY);
				ptRhtMost = CPoint(nPositionX, nPositionY);
			}
		} while(!bComplete);
		if(lSecondContourPtAddress >= 0)
			pImgPtr[lSecondContourPtAddress] = 200;
		ptTopMost = CPoint(nTopMostX, nMinTop);
		ptBotMost = CPoint(nBotMostX, nMaxBottom);
		ptLftMost = CPoint(nMinLeft, nLftMostY);
		ptRhtMost = CPoint(nMaxRight, nRhtMostY);
		if(ptTopMost.y > ptTopMostBak.y)
			ptTopMost = ptTopMostBak;
		if(ptBotMost.y < ptBotMostBak.y)
			ptBotMost = ptBotMostBak;
		if(ptLftMost.x > ptLftMostBak.x)
			ptLftMost = ptLftMostBak;
		if(ptRhtMost.x < ptRhtMostBak.x)
			ptRhtMost = ptRhtMostBak;

		pt2DTopMost.x = ptTopMost.x;
		pt2DTopMost.y = ptTopMost.y - 0.5;
		pt2DBotMost.x = ptBotMost.x;
		pt2DBotMost.y = ptBotMost.y + 0.5;
		pt2DLftMost.x = ptLftMost.x - 0.5;
		pt2DLftMost.y = ptLftMost.y;
		pt2DRhtMost.x = ptRhtMost.x + 0.5;
		pt2DRhtMost.y = ptRhtMost.y;

		dlength[0] = pt2DTopMost.GetDistance(pt2DLftMost);
		dlength[1] = pt2DTopMost.GetDistance(pt2DBotMost);
		dlength[2] = pt2DTopMost.GetDistance(pt2DRhtMost); 
		dlength[3] = pt2DLftMost.GetDistance(pt2DBotMost);
		dlength[4] = pt2DLftMost.GetDistance(pt2DRhtMost);
		dlength[5] = pt2DBotMost.GetDistance(pt2DRhtMost);
		FoundBlobInfo->dFndBlobLength = 0;
		for(i = 0; i < 6; i++) {
			if(dlength[i] > FoundBlobInfo->dFndBlobLength) FoundBlobInfo->dFndBlobLength = dlength[i];
		}
	}
	else {
		ptBlobStartPos = *pptBlobStartPos;
		bComplete = FALSE;
		lFirstIndexStart = ptBlobStartPos.y * nPitch + ptBlobStartPos.x; 
		lSecondContourPtAddress = -1;
		do {
			nCount = 0;
			nDir = 5;
			bFirstPoint =  1;             
			if(bFindBlobContour) 
				ptAryFndBlobContour[nFndBlobContour++] =CPoint(ptBlobStartPos.x + pptROITopLeft->x, ptBlobStartPos.y + pptROITopLeft->y);
//			lIndexStart = ptBlobStartPos.y * nPitch + ptBlobStartPos.x;
			lIndexStart = lFirstIndexStart;
			pImgPtr[lIndexStart] = 200;
			lIndex = lIndexStart;
			do {
				if(pImgPtr[lIndex+pnDir[nDir]] > 0) {
					switch (nDir) {
						case 0: 
							nPositionX++;
							if(nPositionX >= nMaxRight)	nMaxRight = nPositionX;
							break;
						case 1: 
							nPositionX++;
							if(nPositionX >= nMaxRight) nMaxRight = nPositionX;
							nPositionY--;
							if(nPositionY <= nMinTop) nMinTop = nPositionY;
							break; 
						case 2: 
							nPositionY--;
							if(nPositionY <= nMinTop) nMinTop = nPositionY;
							break;
						case 3: 
							nPositionX--;
							if(nPositionX <= nMinLeft) nMinLeft = nPositionX;
							nPositionY--;
							if(nPositionY <= nMinTop) nMinTop = nPositionY; 
							break;
						case 4: 
							nPositionX--;
							if(nPositionX <= nMinLeft) nMinLeft = nPositionX;
							break;
						case 5: 
							nPositionX--;
							if(nPositionX <= nMinLeft) nMinLeft = nPositionX;
							nPositionY++;
							if(nPositionY >= nMaxBottom) nMaxBottom = nPositionY;
							break;
						case 6:
							nPositionY++;
							if(nPositionY >= nMaxBottom) nMaxBottom = nPositionY;
							break;
						case 7:	
							nPositionX++;
							if(nPositionX >= nMaxRight)	nMaxRight = nPositionX;
							nPositionY++;
							if(nPositionY >= nMaxBottom) nMaxBottom = nPositionY;
							break;
					}
					nCount = 0;
					lIndex += pnDir[nDir];
					if(bFindBlobContour) 
						ptAryFndBlobContour[nFndBlobContour++] = CPoint(pptBlobStartPos->x + nPositionX + pptROITopLeft->x,
									pptBlobStartPos->y + nPositionY + pptROITopLeft->y);

					bFirstPoint = 0;             
					pImgPtr[lIndex] = 200;
					nDir -= 2;
					if(nDir<0) nDir += 8;
				}
				else {
					nDir++;
					if(nDir>7) nDir -= 8;
					nCount++;
				}
				if(nCount>8) {
					pImgPtr[lIndex] = 200;
					break;
				}
			} while(lIndex != lIndexStart || bFirstPoint); 
			bComplete = ContourCompleteCheck(WHITE,
											 pImgPtr[lFirstIndexStart + nPitch - 1],
											 pImgPtr[lFirstIndexStart + nPitch],										 
											 pImgPtr[lFirstIndexStart + nPitch + 1], 
											 pImgPtr[lFirstIndexStart + 1],
											 &ptBlobNotFillPos);
			if(!bComplete) {
				if(pImgPtr[lFirstIndexStart + nPitch - 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch - 1;
				if(pImgPtr[lFirstIndexStart + nPitch] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch;
				if(pImgPtr[lFirstIndexStart + nPitch + 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch + 1;
				if(pImgPtr[lFirstIndexStart + 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + 1;
				pImgPtr[lSecondContourPtAddress] = 0;
				nMinLeftBak = nMinLeft;
				nMaxRightBak = nMaxRight;
				nMinTopBak = nMinTop;
				nMaxBottomBak = nMaxBottom;
				nMinLeft = ptBlobStartPos.x - pptBlobStartPos->x;
				nMaxRight = ptBlobStartPos.x - pptBlobStartPos->x;
				nMinTop = ptBlobStartPos.y - pptBlobStartPos->y;
				nMaxBottom = ptBlobStartPos.y - pptBlobStartPos->y;
				nPositionX = nMinLeft;
				nPositionY = nMinTop;
			}
		} while(!bComplete);
		if(lSecondContourPtAddress >= 0)
			pImgPtr[lSecondContourPtAddress] = 200;
	}
	if(nMinLeft > nMinLeftBak)
		nMinLeft = nMinLeftBak;
	if(nMaxRight < nMaxRightBak)
		nMaxRight = nMaxRightBak;
	if(nMinTop > nMinTopBak)
		nMinTop = nMinTopBak;
	if(nMaxBottom < nMaxBottomBak)
		nMaxBottom = nMaxBottomBak;
	FoundBlobInfo->rcFndBlobPos.left = pptBlobStartPos->x + nMinLeft;
	FoundBlobInfo->rcFndBlobPos.right = pptBlobStartPos->x + nMaxRight + 1;
	FoundBlobInfo->rcFndBlobPos.top = pptBlobStartPos->y + nMinTop;
	FoundBlobInfo->rcFndBlobPos.bottom = pptBlobStartPos->y + nMaxBottom + 1;

	if(bFindBlobContour) {
		deleteArray(FoundBlobInfo->pptBlobContour);
		//if(FoundBlobInfo->pptBlobContour)
		//	delete FoundBlobInfo->pptBlobContour;
		FoundBlobInfo->nBlobContourSize = nFndBlobContour;
		createArray(FoundBlobInfo->pptBlobContour, FoundBlobInfo->nBlobContourSize);
		//FoundBlobInfo->pptBlobContour = new CPoint[abs(FoundBlobInfo->nBlobContourSize)];
		for(int i = 0; i < nFndBlobContour; i++)
			FoundBlobInfo->pptBlobContour[i] = ptAryFndBlobContour[i];
	}

	return 0;
}


int CAdvBlobFind::WhiteBlobContour4(CImageBuffer *pImg,
								   CPoint* pptROITopLeft,
								   CPoint *pptBlobStartPos,
								   ADV_BLOB_INFO *FoundBlobInfo,
								   long lProcMode)	
{
	int i;
	int nDir;
	int nMinLeft, nMaxRight, nMinTop, nMaxBottom, nPositionX, nPositionY;
	int nMinLeftBak, nMaxRightBak, nMinTopBak, nMaxBottomBak;
	long lIndexStart, lIndex;
	int nPitch;
	CPoint ptBlobStartPos;
	BYTE *pImgPtr;
	BOOL bComplete;
	long lImgROIArea;
	CPoint ptTopMostBak, ptBotMostBak, ptLftMostBak, ptRhtMostBak;
	long lFirstIndexStart;
	BOOL bFindBlobContour;
	long lSecondContourPtAddress;
	CPoint ptBlobNotFillPos;

	bFindBlobContour = lProcMode & FIND_BLOB_CONTOUR;
	pImgPtr = pImg->GetImgPtr();
	nPitch = pImg->GetPitch();
	lImgROIArea = pImg->GetImgSize().cx * pImg->GetImgSize().cy;

	nDir = 5;
	nMinLeft = 0;
	nMaxRight = 0;
	nMinTop = 0;
	nMaxBottom = 0;
	nMinLeftBak = 0;
	nMaxRightBak = 0;
	nMinTopBak = 0;
	nMaxBottomBak = 0;
    nPositionX = 0;
	nPositionY = 0;

	nFndBlobContour = 0;
	if(bFindBlobContour) 
		ptAryFndBlobContour[nFndBlobContour++] = CPoint(pptBlobStartPos->x + pptROITopLeft->x, pptBlobStartPos->y + pptROITopLeft->y);
	ptBlobStartPos = *pptBlobStartPos;
	bComplete = FALSE;
	lFirstIndexStart = ptBlobStartPos.y * nPitch + ptBlobStartPos.x; 
	lSecondContourPtAddress = -1;

	if((pImgPtr[lFirstIndexStart+nDir5] == 0) && (pImgPtr[lFirstIndexStart+nDir6] == 0) &&
		(pImgPtr[lFirstIndexStart+nDir7] == 0) && (pImgPtr[lFirstIndexStart+nDir0] == 0)) {
		pImgPtr[lFirstIndexStart] = 200;
		FoundBlobInfo->rcFndBlobPos = CRect(*pptBlobStartPos,CSize(1,1));
		goto Pass;
	}
	else {
		do{
			nDir = 5;
//			lIndexStart = ptBlobStartPos.y * nPitch + ptBlobStartPos.x;
			lIndexStart = lFirstIndexStart;
			pImgPtr[lIndexStart] = 200;
			lIndex = lIndexStart;
			for(i = 5;i <= 8; i++) {
				if(i == 8)
					nDir = 0;
				else
					nDir = i;
				if(pImgPtr[lIndexStart+pnDir[nDir]] == 255) 
					break;
			}
			do {
				if(pImgPtr[lIndex+pnDir[nDir]] > 0) {
					switch (nDir) {
						case 0: 
							nPositionX++;
							if(nPositionX > nMaxRight)	nMaxRight = nPositionX;
							break;
						case 1: 
							nPositionX++;
							if(nPositionX > nMaxRight) nMaxRight = nPositionX;
							nPositionY--;
							if(nPositionY < nMinTop) nMinTop = nPositionY;
							break; 
						case 2: 
							nPositionY--;
							if(nPositionY < nMinTop) nMinTop = nPositionY;
							break;
						case 3: 
							nPositionX--;
							if(nPositionX < nMinLeft) nMinLeft = nPositionX;
							nPositionY--;
							if(nPositionY < nMinTop) nMinTop = nPositionY; 
							break;
						case 4: 
							nPositionX--;
							if(nPositionX < nMinLeft) nMinLeft = nPositionX;
							break;
						case 5: 
							nPositionX--;
							if(nPositionX < nMinLeft) nMinLeft = nPositionX;
							nPositionY++;
							if(nPositionY > nMaxBottom) nMaxBottom = nPositionY;
							break;
						case 6:
							nPositionY++;
							if(nPositionY > nMaxBottom) nMaxBottom = nPositionY;
							break;
						case 7:	
							nPositionX++;
							if(nPositionX > nMaxRight)	nMaxRight = nPositionX;
							nPositionY++;
							if(nPositionY > nMaxBottom) nMaxBottom = nPositionY;
							break;
					}
					lIndex += pnDir[nDir];
					if(bFindBlobContour) 
						ptAryFndBlobContour[nFndBlobContour++] = CPoint(pptBlobStartPos->x + nPositionX + pptROITopLeft->x,
									pptBlobStartPos->y + nPositionY + pptROITopLeft->y);
					pImgPtr[lIndex] = 200;
					nDir -= 2;
					if(nDir<0) nDir += 8;
				}
				else {
					nDir++;
					if(nDir>7) nDir -= 8;
				}
			} while(lIndex != lIndexStart); 
			bComplete = ContourCompleteCheck(WHITE,
											 pImgPtr[lFirstIndexStart + nPitch - 1],
											 pImgPtr[lFirstIndexStart + nPitch],										 
											 pImgPtr[lFirstIndexStart + nPitch + 1], 
											 pImgPtr[lFirstIndexStart + 1],
											 &ptBlobNotFillPos);
			if(!bComplete) {
				if(pImgPtr[lFirstIndexStart + nPitch - 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch - 1;
				if(pImgPtr[lFirstIndexStart + nPitch] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch;
				if(pImgPtr[lFirstIndexStart + nPitch + 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch + 1;
				if(pImgPtr[lFirstIndexStart + 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + 1;
				pImgPtr[lSecondContourPtAddress] = 0;
				nMinLeftBak = nMinLeft;
				nMaxRightBak = nMaxRight;
				nMinTopBak = nMinTop;
				nMaxBottomBak = nMaxBottom;
				nMinLeft = ptBlobStartPos.x - pptBlobStartPos->x;
				nMaxRight = ptBlobStartPos.x - pptBlobStartPos->x;
				nMinTop = ptBlobStartPos.y - pptBlobStartPos->y;
				nMaxBottom = ptBlobStartPos.y - pptBlobStartPos->y;
				nPositionX = nMinLeft;
				nPositionY = nMinTop;
			}
		}while(!bComplete);
	}
	if(lSecondContourPtAddress >= 0)
		pImgPtr[lSecondContourPtAddress] = 200;
	if(nMinLeft > nMinLeftBak)
		nMinLeft = nMinLeftBak;
	if(nMaxRight < nMaxRightBak)
		nMaxRight = nMaxRightBak;
	if(nMinTop > nMinTopBak)
		nMinTop = nMinTopBak;
	if(nMaxBottom < nMaxBottomBak)
		nMaxBottom = nMaxBottomBak;
	FoundBlobInfo->rcFndBlobPos.left = pptBlobStartPos->x + nMinLeft;
	FoundBlobInfo->rcFndBlobPos.right = pptBlobStartPos->x + nMaxRight + 1;
	FoundBlobInfo->rcFndBlobPos.top = pptBlobStartPos->y + nMinTop;
	FoundBlobInfo->rcFndBlobPos.bottom = pptBlobStartPos->y + nMaxBottom + 1;
Pass:
	return 0;
}


BOOL CAdvBlobFind::ContourCompleteCheck(int nBlobType, BYTE byteBotLft, BYTE byteBot, BYTE byteBotRht, BYTE byteRht, CPoint* pptBlobStart)
{
	if(nBlobType == WHITE) {
		if((byteBotLft == 0) || (byteBot > 0))		return TRUE;
		if((byteRht == 0) && (byteBotRht == 0))		return TRUE;
		if((byteRht == 200) || (byteBotRht == 200))	return TRUE;
		if(byteRht == 255) {
			pptBlobStart->x = pptBlobStart->x + 1;
			return FALSE;
		}
		else {
			if(byteBotRht == 255) {
				pptBlobStart->x = pptBlobStart->x + 1;
				pptBlobStart->y = pptBlobStart->y + 1;
				return FALSE;
			}
		}
	}
	else {
		if((byteBotLft == 255) || (byteBot <= 200))		return TRUE;
		if((byteRht == 255) && (byteBotRht == 255))		return TRUE;
		if((byteRht == 200) || (byteBotRht == 200))	return TRUE;
		if(byteRht == 0) {
			pptBlobStart->x = pptBlobStart->x + 1;
			return FALSE;
		}
		else {
			if(byteBotRht == 0) {
				pptBlobStart->x = pptBlobStart->x + 1;
				pptBlobStart->y = pptBlobStart->y + 1;
				return FALSE;
			}
		}
	}
	return TRUE;
}


int CAdvBlobFind::BlackBlobContour(CImageBuffer *pImg,
								   CPoint* pptROITopLeft,
								   CPoint *pptBlobStartPos,
								   ADV_BLOB_INFO *FoundBlobInfo,
								   long lProcMode)	

{
	int nDir, nCount;
	int nMinLeft, nMaxRight, nMinTop, nMaxBottom, nPositionX, nPositionY;
	int nMinLeftBak, nMaxRightBak, nMinTopBak, nMaxBottomBak;
	long lIndexStart, lIndex;
	int nPitch;
	CPoint ptBlobStartPos;
	BYTE *pImgPtr;
	int pnDir[8];
	BOOL bComplete;
	BOOL bFirstPoint;             
	long lImgROIArea;
	CPoint ptTopMostBak, ptBotMostBak, ptLftMostBak, ptRhtMostBak;
	long lFirstIndexStart;
	CPoint2D pt2DTopMost, pt2DBotMost, pt2DLftMost, pt2DRhtMost;
	long lSecondContourPtAddress;
	CPoint ptBlobNotFillPos;


	pImgPtr = pImg->GetImgPtr();
	nPitch = pImg->GetPitch();
	lImgROIArea = pImg->GetImgSize().cx * pImg->GetImgSize().cy;

	pnDir[0] = 1;
	pnDir[1] = -(nPitch - 1);
	pnDir[2] = -nPitch;
	pnDir[3] = -(nPitch + 1);
	pnDir[4] = -1;
	pnDir[5] = nPitch - 1;
	pnDir[6] = nPitch;
	pnDir[7] = nPitch + 1;

	nDir = 5;
	nCount = 0;
	nMinLeft = 0;
	nMaxRight = 0;
	nMinTop = 0;
	nMaxBottom = 0;
	nMinLeftBak = 0;
	nMaxRightBak = 0;
	nMinTopBak = 0;
	nMaxBottomBak = 0;
    nPositionX = 0;
	nPositionY = 0;
	bFirstPoint =  1;             

	nFndBlobContour = 0;
	if((lProcMode & FIND_BLOB_LENGTH) || (lProcMode & FIND_BLOB_ELONGATION)) {
		CPoint ptTopMost, ptBotMost, ptLftMost, ptRhtMost;
		double dlength[6];
		int i;
		ptTopMost = (0,0);
		ptBotMost = (0,0);
		ptLftMost = (0,0);
		ptRhtMost = (0,0);
		ptTopMostBak = CPoint(0,0);
		ptBotMostBak = CPoint(0,0);
		ptLftMostBak = CPoint(0,0);
		ptRhtMostBak = CPoint(0,0);
		if(lProcMode & FIND_BLOB_CONTOUR) 
			ptAryFndBlobContour[nFndBlobContour++] = CPoint(pptBlobStartPos->x + pptROITopLeft->x, pptBlobStartPos->y + pptROITopLeft->y);

		ptBlobStartPos = *pptBlobStartPos;
		bComplete = FALSE;
		lFirstIndexStart = ptBlobStartPos.y * nPitch + ptBlobStartPos.x; 
		lSecondContourPtAddress = -1;
		do{
			nCount = 0;
			nDir = 5;
			bFirstPoint =  1;             
//			lIndexStart = ptBlobStartPos.y * nPitch + ptBlobStartPos.x;
			lIndexStart = lFirstIndexStart;
			pImgPtr[lIndexStart] = 200;
			lIndex = lIndexStart;
			do {
				if(pImgPtr[lIndex+pnDir[nDir]] < 255) {
					switch (nDir) {
						case 0: 
							nPositionX++;
							if(nPositionX >= nMaxRight)	{
								nMaxRight = nPositionX;
								ptRhtMost = CPoint(nPositionX, nPositionY);
							}
							break;
						case 1: 
							nPositionX++;
							if(nPositionX >= nMaxRight) {
								nMaxRight = nPositionX;
								ptRhtMost = CPoint(nPositionX, nPositionY);
							}
							nPositionY--;
							if(nPositionY <= nMinTop) {
								nMinTop = nPositionY;
								ptTopMost = CPoint(nPositionX, nPositionY);
							}
							break; 
						case 2: 
							nPositionY--;
							if(nPositionY <= nMinTop) {
								nMinTop = nPositionY;
								ptTopMost = CPoint(nPositionX, nPositionY);
							}
							break;
						case 3: 
							nPositionX--;
							if(nPositionX <= nMinLeft) {
								nMinLeft = nPositionX;
								ptLftMost = CPoint(nPositionX, nPositionY);
							}
							nPositionY--;
							if(nPositionY <= nMinTop) {
								nMinTop = nPositionY; 
								ptTopMost = CPoint(nPositionX, nPositionY);
							}
							break;
						case 4: 
							nPositionX--;
							if(nPositionX <= nMinLeft) {
								nMinLeft = nPositionX;
								ptLftMost = CPoint(nPositionX, nPositionY);
							}
							break;
						case 5: 
							nPositionX--;
							if(nPositionX <= nMinLeft) {
								nMinLeft = nPositionX;
								ptLftMost = CPoint(nPositionX, nPositionY);
							}
							nPositionY++;
							if(nPositionY >= nMaxBottom) {
								nMaxBottom = nPositionY;
								ptBotMost = CPoint(nPositionX, nPositionY);
							}
							break;
						case 6:
							nPositionY++;
							if(nPositionY >= nMaxBottom) {
								nMaxBottom = nPositionY;
								ptBotMost = CPoint(nPositionX, nPositionY);
							}
							break;
						case 7:	
							nPositionX++;
							if(nPositionX >= nMaxRight)	{
								nMaxRight = nPositionX;
								ptRhtMost = CPoint(nPositionX, nPositionY);
							}
							nPositionY++;
							if(nPositionY >= nMaxBottom) {
								nMaxBottom = nPositionY;
								ptBotMost = CPoint(nPositionX, nPositionY);
							}
							break;
					}
					nCount = 0;
					lIndex += pnDir[nDir];

					if(lProcMode & FIND_BLOB_CONTOUR) 
						ptAryFndBlobContour[nFndBlobContour++] = CPoint(pptBlobStartPos->x + nPositionX + pptROITopLeft->x,
																pptBlobStartPos->y + nPositionY + pptROITopLeft->x);

					bFirstPoint = 0;             
					pImgPtr[lIndex] = 200;
					nDir -= 2;
					if(nDir<0) nDir += 8;
				}
				else {
					nDir++;
					if(nDir>7) nDir -= 8;
					nCount++;
				}
				if(nCount>8) {
					pImgPtr[lIndex] = 200;
					break;
				}
			} while(lIndex != lIndexStart || bFirstPoint); 
			bComplete = ContourCompleteCheck(BLACK,
											 pImgPtr[lFirstIndexStart + nPitch - 1],
											 pImgPtr[lFirstIndexStart + nPitch],										 
											 pImgPtr[lFirstIndexStart + nPitch + 1], 
											 pImgPtr[lFirstIndexStart + 1],
											 &ptBlobNotFillPos);
			if(!bComplete) {
				if(pImgPtr[lFirstIndexStart + nPitch - 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch - 1;
				if(pImgPtr[lFirstIndexStart + nPitch] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch;
				if(pImgPtr[lFirstIndexStart + nPitch + 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch + 1;
				if(pImgPtr[lFirstIndexStart + 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + 1;
				pImgPtr[lSecondContourPtAddress] = 255;
				nMinLeftBak = nMinLeft;
				nMaxRightBak = nMaxRight;
				nMinTopBak = nMinTop;
				nMaxBottomBak = nMaxBottom;
				ptTopMostBak = ptTopMost;
				ptBotMostBak = ptBotMost;
				ptLftMostBak = ptLftMost;
				ptRhtMostBak = ptRhtMost;
				nMinLeft = ptBlobStartPos.x - pptBlobStartPos->x;
				nMaxRight = ptBlobStartPos.x - pptBlobStartPos->x;
				nMinTop = ptBlobStartPos.y - pptBlobStartPos->y;
				nMaxBottom = ptBlobStartPos.y - pptBlobStartPos->y;
				nPositionX = nMinLeft;
				nPositionY = nMinTop;
				ptTopMost = CPoint(nPositionX, nPositionY);
				ptBotMost = CPoint(nPositionX, nPositionY);
				ptLftMost = CPoint(nPositionX, nPositionY);
				ptRhtMost = CPoint(nPositionX, nPositionY);
			}
		}while(!bComplete);
		if(lSecondContourPtAddress >= 0)
			pImgPtr[lSecondContourPtAddress] = 200;
		if(ptTopMost.y > ptTopMostBak.y)
			ptTopMost = ptTopMostBak;
		if(ptBotMost.y < ptBotMostBak.y)
			ptBotMost = ptBotMostBak;
		if(ptLftMost.x > ptLftMostBak.x)
			ptLftMost = ptLftMostBak;
		if(ptRhtMost.x < ptRhtMostBak.x)
			ptRhtMost = ptRhtMostBak;

		pt2DTopMost.x = ptTopMost.x;
		pt2DTopMost.y = ptTopMost.y - 0.5;
		pt2DBotMost.x = ptBotMost.x;
		pt2DBotMost.y = ptBotMost.y + 0.5;
		pt2DLftMost.x = ptLftMost.x - 0.5;
		pt2DLftMost.y = ptLftMost.y;
		pt2DRhtMost.x = ptRhtMost.x + 0.5;
		pt2DRhtMost.y = ptRhtMost.y;

		dlength[0] = pt2DTopMost.GetDistance(pt2DLftMost);
		dlength[1] = pt2DTopMost.GetDistance(pt2DBotMost);
		dlength[2] = pt2DTopMost.GetDistance(pt2DRhtMost); 
		dlength[3] = pt2DLftMost.GetDistance(pt2DBotMost);
		dlength[4] = pt2DLftMost.GetDistance(pt2DRhtMost);
		dlength[5] = pt2DBotMost.GetDistance(pt2DRhtMost);

		FoundBlobInfo->dFndBlobLength = 0;
		for(i = 0; i < 6; i++) {
			if(dlength[i] > FoundBlobInfo->dFndBlobLength) FoundBlobInfo->dFndBlobLength = dlength[i];
		}
	}
	else {
		if(lProcMode & FIND_BLOB_CONTOUR) 
			ptAryFndBlobContour[nFndBlobContour++] = CPoint(pptBlobStartPos->x + pptROITopLeft->x, pptBlobStartPos->y + pptROITopLeft->y);

		ptBlobStartPos = *pptBlobStartPos;
		bComplete = FALSE;
		lFirstIndexStart = ptBlobStartPos.y * nPitch + ptBlobStartPos.x; 
		lSecondContourPtAddress = -1;
		do{
			nCount = 0;
			nDir = 5;
			bFirstPoint =  1;             
//			lIndexStart = ptBlobStartPos.y * nPitch + ptBlobStartPos.x;
			lIndexStart = lFirstIndexStart;
			pImgPtr[lIndexStart] = 200;
			lIndex = lIndexStart;
			do {
				if(pImgPtr[lIndex+pnDir[nDir]] < 255) {
					switch (nDir) {
						case 0: 
							nPositionX++;
							if(nPositionX >= nMaxRight)	nMaxRight = nPositionX;
							break;
						case 1: 
							nPositionX++;
							if(nPositionX >= nMaxRight) nMaxRight = nPositionX;
							nPositionY--;
							if(nPositionY <= nMinTop) nMinTop = nPositionY;
							break; 
						case 2: 
							nPositionY--;
							if(nPositionY <= nMinTop) nMinTop = nPositionY;
							break;
						case 3: 
							nPositionX--;
							if(nPositionX <= nMinLeft) nMinLeft = nPositionX;
							nPositionY--;
							if(nPositionY <= nMinTop) nMinTop = nPositionY; 
							break;
						case 4: 
							nPositionX--;
							if(nPositionX <= nMinLeft) nMinLeft = nPositionX;
							break;
						case 5: 
							nPositionX--;
							if(nPositionX <= nMinLeft) nMinLeft = nPositionX;
							nPositionY++;
							if(nPositionY >= nMaxBottom) nMaxBottom = nPositionY;
							break;
						case 6:
							nPositionY++;
							if(nPositionY >= nMaxBottom) nMaxBottom = nPositionY;
							break;
						case 7:	
							nPositionX++;
							if(nPositionX >= nMaxRight)	nMaxRight = nPositionX;
							nPositionY++;
							if(nPositionY >= nMaxBottom) nMaxBottom = nPositionY;
							break;
					}
					nCount = 0;
					lIndex += pnDir[nDir];
					if(lProcMode & FIND_BLOB_CONTOUR) 
						ptAryFndBlobContour[nFndBlobContour++] = CPoint(pptBlobStartPos->x + nPositionX + pptROITopLeft->x,
									pptBlobStartPos->y + nPositionY + pptROITopLeft->y);

					bFirstPoint = 0;             
					pImgPtr[lIndex] = 200;
					nDir -= 2;
					if(nDir<0) nDir += 8;
				}
				else {
					nDir++;
					if(nDir>7) nDir -= 8;
					nCount++;
				}
				if(nCount>8) {
					pImgPtr[lIndex] = 200;
					break;
				}
			} while(lIndex != lIndexStart || bFirstPoint); 
			bComplete = ContourCompleteCheck(BLACK,
											 pImgPtr[lFirstIndexStart + nPitch - 1],
											 pImgPtr[lFirstIndexStart + nPitch],										 
											 pImgPtr[lFirstIndexStart + nPitch + 1], 
											 pImgPtr[lFirstIndexStart + 1],
											 &ptBlobNotFillPos);
			if(!bComplete) {
				if(pImgPtr[lFirstIndexStart + nPitch - 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch - 1;
				if(pImgPtr[lFirstIndexStart + nPitch] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch;
				if(pImgPtr[lFirstIndexStart + nPitch + 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch + 1;
				if(pImgPtr[lFirstIndexStart + 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + 1;
				pImgPtr[lSecondContourPtAddress] = 255;
				nMinLeftBak = nMinLeft;
				nMaxRightBak = nMaxRight;
				nMinTopBak = nMinTop;
				nMaxBottomBak = nMaxBottom;
				nMinLeft = ptBlobStartPos.x - pptBlobStartPos->x;
				nMaxRight = ptBlobStartPos.x - pptBlobStartPos->x;
				nMinTop = ptBlobStartPos.y - pptBlobStartPos->y;
				nMaxBottom = ptBlobStartPos.y - pptBlobStartPos->y;
				nPositionX = nMinLeft;
				nPositionY = nMinTop;
			}
		}while(!bComplete);
	}
	if(lSecondContourPtAddress >= 0)
		pImgPtr[lSecondContourPtAddress] = 200;
	if(nMinLeft > nMinLeftBak)
		nMinLeft = nMinLeftBak;
	if(nMaxRight < nMaxRightBak)
		nMaxRight = nMaxRightBak;
	if(nMinTop > nMinTopBak)
		nMinTop = nMinTopBak;
	if(nMaxBottom < nMaxBottomBak)
		nMaxBottom = nMaxBottomBak;
	FoundBlobInfo->rcFndBlobPos.left = pptBlobStartPos->x + nMinLeft;
	FoundBlobInfo->rcFndBlobPos.right = pptBlobStartPos->x + nMaxRight + 1;
	FoundBlobInfo->rcFndBlobPos.top = pptBlobStartPos->y + nMinTop;
	FoundBlobInfo->rcFndBlobPos.bottom = pptBlobStartPos->y + nMaxBottom + 1;

	if(lProcMode & FIND_BLOB_CONTOUR) {
		deleteArray(FoundBlobInfo->pptBlobContour);
		//if(FoundBlobInfo->pptBlobContour)
		//	delete FoundBlobInfo->pptBlobContour;
		FoundBlobInfo->nBlobContourSize = nFndBlobContour;
		createArray(FoundBlobInfo->pptBlobContour, FoundBlobInfo->nBlobContourSize);
		//FoundBlobInfo->pptBlobContour = new CPoint[abs(FoundBlobInfo->nBlobContourSize)];
		for(int i = 0; i < FoundBlobInfo->nBlobContourSize; i++)
			FoundBlobInfo->pptBlobContour[i] = ptAryFndBlobContour[i];
	}
	return 0;
}

int CAdvBlobFind::BlackBlobContour4(CImageBuffer *pImg,
								   CPoint* pptROITopLeft,
								   CPoint *pptBlobStartPos,
								   ADV_BLOB_INFO *FoundBlobInfo,
								   long lProcMode)	
{
	int i;
	int nDir;
	int nMinLeft, nMaxRight, nMinTop, nMaxBottom, nPositionX, nPositionY;
	int nMinLeftBak, nMaxRightBak, nMinTopBak, nMaxBottomBak;
	long lIndexStart, lIndex;
	int nPitch;
	CPoint ptBlobStartPos;
	BYTE *pImgPtr;
	BOOL bComplete;
	long lImgROIArea;
	CPoint ptTopMostBak, ptBotMostBak, ptLftMostBak, ptRhtMostBak;
	long lFirstIndexStart;
	BOOL bFindBlobContour;
	long lSecondContourPtAddress;
	CPoint ptBlobNotFillPos;

	bFindBlobContour = lProcMode & FIND_BLOB_CONTOUR;
	pImgPtr = pImg->GetImgPtr();
	nPitch = pImg->GetPitch();
	lImgROIArea = pImg->GetImgSize().cx * pImg->GetImgSize().cy;

	nDir = 5;
	nMinLeft = 0;
	nMaxRight = 0;
	nMinTop = 0;
	nMaxBottom = 0;
	nMinLeftBak = 0;
	nMaxRightBak = 0;
	nMinTopBak = 0;
	nMaxBottomBak = 0;
    nPositionX = 0;
	nPositionY = 0;

	nFndBlobContour = 0;
	if(bFindBlobContour) {
		ptAryFndBlobContour[nFndBlobContour++] = CPoint(pptBlobStartPos->x + pptROITopLeft->x, pptBlobStartPos->y + pptROITopLeft->y);
	}
	ptBlobStartPos = *pptBlobStartPos;
	bComplete = FALSE;
	lFirstIndexStart = ptBlobStartPos.y * nPitch + ptBlobStartPos.x; 
	lSecondContourPtAddress = -1;

	if((pImgPtr[lFirstIndexStart+nDir5] == 255) && (pImgPtr[lFirstIndexStart+nDir6] == 255) &&
		(pImgPtr[lFirstIndexStart+nDir7] == 255) && (pImgPtr[lFirstIndexStart+nDir0] == 255)) {
		pImgPtr[lFirstIndexStart] = 255;
		goto Fail;
	}
	else {
		do{
			nDir = 5;
//			lIndexStart = ptBlobStartPos.y * nPitch + ptBlobStartPos.x;
			lIndexStart = lFirstIndexStart;
			pImgPtr[lIndexStart] = 200;
			lIndex = lIndexStart;
			for(i = 5;i <= 8; i++) {
				if(i == 8)
					nDir = 0;
				else
					nDir = i;
				if(pImgPtr[lIndexStart+pnDir[nDir]] == 0) 
					break;
			}
			do {
				if(pImgPtr[lIndex+pnDir[nDir]] < 255) {
					switch (nDir) {
						case 0: 
							nPositionX++;
							if(nPositionX > nMaxRight)	nMaxRight = nPositionX;
							break;
						case 1: 
							nPositionX++;
							if(nPositionX > nMaxRight) nMaxRight = nPositionX;
							nPositionY--;
							if(nPositionY < nMinTop) nMinTop = nPositionY;
							break; 
						case 2: 
							nPositionY--;
							if(nPositionY < nMinTop) nMinTop = nPositionY;
							break;
						case 3: 
							nPositionX--;
							if(nPositionX < nMinLeft) nMinLeft = nPositionX;
							nPositionY--;
							if(nPositionY < nMinTop) nMinTop = nPositionY; 
							break;
						case 4: 
							nPositionX--;
							if(nPositionX < nMinLeft) nMinLeft = nPositionX;
							break;
						case 5: 
							nPositionX--;
							if(nPositionX < nMinLeft) nMinLeft = nPositionX;
							nPositionY++;
							if(nPositionY > nMaxBottom) nMaxBottom = nPositionY;
							break;
						case 6:
							nPositionY++;
							if(nPositionY > nMaxBottom) nMaxBottom = nPositionY;
							break;
						case 7:	
							nPositionX++;
							if(nPositionX > nMaxRight)	nMaxRight = nPositionX;
							nPositionY++;
							if(nPositionY > nMaxBottom) nMaxBottom = nPositionY;
							break;
					}
					lIndex += pnDir[nDir];
					if(bFindBlobContour) 
						ptAryFndBlobContour[nFndBlobContour++] = CPoint(pptBlobStartPos->x + nPositionX + pptROITopLeft->x,
									pptBlobStartPos->y + nPositionY + pptROITopLeft->y);
					pImgPtr[lIndex] = 200;
					nDir -= 2;
					if(nDir<0) nDir += 8;
				}
				else {
					nDir++;
					if(nDir>7) nDir -= 8;
				}
			} while(lIndex != lIndexStart); 
			bComplete = ContourCompleteCheck(BLACK,
											 pImgPtr[lFirstIndexStart + nPitch - 1],
											 pImgPtr[lFirstIndexStart + nPitch],										 
											 pImgPtr[lFirstIndexStart + nPitch + 1], 
											 pImgPtr[lFirstIndexStart + 1],
											 &ptBlobNotFillPos);
			if(!bComplete) {
				if(pImgPtr[lFirstIndexStart + nPitch - 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch - 1;
				if(pImgPtr[lFirstIndexStart + nPitch] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch;
				if(pImgPtr[lFirstIndexStart + nPitch + 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + nPitch + 1;
				if(pImgPtr[lFirstIndexStart + 1] == 200)
					lSecondContourPtAddress = lFirstIndexStart + 1;
				pImgPtr[lSecondContourPtAddress] = 255;
				nMinLeftBak = nMinLeft;
				nMaxRightBak = nMaxRight;
				nMinTopBak = nMinTop;
				nMaxBottomBak = nMaxBottom;
				nMinLeft = ptBlobStartPos.x - pptBlobStartPos->x;
				nMaxRight = ptBlobStartPos.x - pptBlobStartPos->x;
				nMinTop = ptBlobStartPos.y - pptBlobStartPos->y;
				nMaxBottom = ptBlobStartPos.y - pptBlobStartPos->y;
				nPositionX = nMinLeft;
				nPositionY = nMinTop;
			}
		}while(!bComplete);
	}
	if(lSecondContourPtAddress >= 0)
		pImgPtr[lSecondContourPtAddress] = 200;
	if(nMinLeft > nMinLeftBak)
		nMinLeft = nMinLeftBak;
	if(nMaxRight < nMaxRightBak)
		nMaxRight = nMaxRightBak;
	if(nMinTop > nMinTopBak)
		nMinTop = nMinTopBak;
	if(nMaxBottom < nMaxBottomBak)
		nMaxBottom = nMaxBottomBak;
	FoundBlobInfo->rcFndBlobPos.left = pptBlobStartPos->x + nMinLeft;
	FoundBlobInfo->rcFndBlobPos.right = pptBlobStartPos->x + nMaxRight + 1;
	FoundBlobInfo->rcFndBlobPos.top = pptBlobStartPos->y + nMinTop;
	FoundBlobInfo->rcFndBlobPos.bottom = pptBlobStartPos->y + nMaxBottom + 1;

	if(bFindBlobContour) {
		if(FoundBlobInfo->pptBlobContour)
			delete FoundBlobInfo->pptBlobContour;
		FoundBlobInfo->nBlobContourSize = nFndBlobContour;
		FoundBlobInfo->pptBlobContour = new CPoint[abs(FoundBlobInfo->nBlobContourSize)];
		for(int i = 0; i < FoundBlobInfo->nBlobContourSize; i++)
			FoundBlobInfo->pptBlobContour[i] = ptAryFndBlobContour[i];
	}
	return 0;
Fail:
	return -1;
}

int CAdvBlobFind::DrawAdvBlob(IMAGEVIEW_INFO *pView,
							  ADV_BLOB_FIND_PARM *pParm,
							 ADV_BLOB_FIND_RSLT *pRslt)
{

	int i;
	LOGPEN PenInfo;

	PenInfo.lopnColor = RGB(0,255,0);
	PenInfo.lopnStyle = PS_SOLID;
	PenInfo.lopnWidth.x = 1;

	DrwRect(pView,&pParm->rcROISrc,&PenInfo);
	PenInfo.lopnColor = RGB(0,0,255);
	PenInfo.lopnStyle = PS_SOLID;
	PenInfo.lopnWidth.x = 1;

	if (pRslt->nFndBlobs>0) {
		for (i=0;i<pRslt->nFndBlobs;i++) {
			//Draw Blob
			DrwRect(pView,&pRslt->pAdvBlobInfo[i].rcFndBlobPos,&PenInfo);
		}
	}

	return 0;
}

CGridEdge::CGridEdge()
{
	m_EdgeFindRslt.nLineCount = 0;
	m_EdgeFindRslt.bRsltValid = FALSE;
}

CGridEdge::~CGridEdge()
{
}

int CGridEdge::GetGridEdge(IMAGEVIEW_INFO *pView,
						   CImageBuffer *pImg,
						   GRD_EDGE_FIND_PARM *pParm,
						   long lDebugFlag)
{
	long lProjVal[MAX_PROJ];
	int nError,i,j;
	CRect rect;
	int nSX,nSY;

	if(CheckRect(&pParm->rectROI,&pView->sizeImage))
		return -1;

	m_EdgeFindRslt.bRsltValid = FALSE;
	memset(lProjVal,-1,MAX_PROJ*sizeof(long));

	int nWidth = pParm->rectROI.Width();
	int nHeight = pParm->rectROI.Height();
	if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
		OutputDebugLog("GetGridEdge()-> width(%d),Height(%d)",nWidth,nHeight);
	}

	pParm->nGroupFactor = max(pParm->nGroupFactor,1);
	pParm->nSkipFactor = max(pParm->nSkipFactor,0);

	if (lDebugFlag&DEBUG_PRINT && lDebugFlag&DEBUG_EDGE) {
		OutputDebugLog("GetGridEdge()->rectROI=(%d,%d,%d,%d)",
						pParm->rectROI.left,
						pParm->rectROI.top,
						pParm->rectROI.right,
						pParm->rectROI.bottom);
	}
	if (pParm->nOrientation==ROW) {
		if (nWidth>MAX_PROJ) {
			AfxMessageBox("nWidth>MAX_PROJ in GetGridEdge() in CGridEdge Class");
			return -9;
		}
		rect.left = pParm->rectROI.left;
		rect.right = pParm->rectROI.right;
		for (nSY=pParm->rectROI.top,m_EdgeFindRslt.nLineCount=0;
			 nSY<pParm->rectROI.bottom;
			 nSY+=(pParm->nSkipFactor+pParm->nGroupFactor),
			 m_EdgeFindRslt.nLineCount++) {
			if (m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT) {
				AfxMessageBox("m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT in GetGridEdge() in CGridEdge Class");
				return -9;
			}
		
			rect.top = nSY;
			//Removed -1 from the below statement to prevent rect.Top = rect.Bottom 
			//which will affect XGrayAccl function
			rect.bottom = nSY+pParm->nGroupFactor;
			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				OutputDebugLog("GetGridEdge()->ROW rect=(%d,%d,%d,%d)",
								rect.left,
								rect.top,
								rect.right,
								rect.bottom);
			}
			nError = GetXGrayAccl(pImg, &rect, &lProjVal[0]);
			if (nError)
				return nError;

			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				for (i=0;i<nWidth;i++) {
					OutputDebugLog("GetGridEdge()->lProjVal[%d]=%ld",i,lProjVal[i]);
				}
				AfxMessageBox("Check lProjVal inside GetGridEdge()");
			}

			nError = GetEdge1D(&lProjVal[0],
					  nWidth,
					  pParm->nGroupFactor,
					  rect.left,
					  pParm->nThreshold,
					  pParm->nGradient,
					  pParm->nMethod,
					  pParm->nDirection,
					  m_EdgeFindRslt.dFndEdgePos[m_EdgeFindRslt.nLineCount],
					  m_EdgeFindRslt.nFndEdgeGrad[m_EdgeFindRslt.nLineCount],lDebugFlag);
			if (nError)
				return nError;
			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				for (j=0;j<m_EdgeFindRslt.dFndEdgePos[m_EdgeFindRslt.nLineCount][0];j++) {
					OutputDebugLog("GetGridEdge()->FndEdge::Line[%d] Edge[%d]=%3.2f EdgeType=[%d]",
									m_EdgeFindRslt.nLineCount,
									j+1,
									m_EdgeFindRslt.dFndEdgePos[m_EdgeFindRslt.nLineCount][j+1],
									m_EdgeFindRslt.nFndEdgeGrad[m_EdgeFindRslt.nLineCount][j+1]);
				}
			}

		}
	}
	else {
		if (nHeight > MAX_PROJ) {
			AfxMessageBox("nHeight > MAX_PROJ in GetGridEdge() in CGridEdge Class");
			return -9;
		}
		rect.top = pParm->rectROI.top;
		rect.bottom = pParm->rectROI.bottom;

		for (nSX=pParm->rectROI.left,m_EdgeFindRslt.nLineCount=0;
			 nSX<pParm->rectROI.right;
			 nSX+=(pParm->nSkipFactor+pParm->nGroupFactor),
				 m_EdgeFindRslt.nLineCount++) {
			if (m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT) {
				AfxMessageBox("m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT in GetGridEdge() in CGridEdge Class");
				return -9;
			}

			rect.left = nSX;
			//Removed -1 from the below statement to prevent rect.left = rect.Right 
			//which will affect YGrayAccl function
			rect.right = nSX+pParm->nGroupFactor;

			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				OutputDebugLog("GetGridEdge()->COL rect=(%d,%d,%d,%d)",
								rect.left,
								rect.top,
								rect.right,
								rect.bottom);
			}


			nError = GetYGrayAccl(pImg, &rect, &lProjVal[0]);

			if (nError)
				return nError;

			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				for (i=0;i<nHeight;i++) {
					OutputDebugLog("GetGridEdge()->lProjVal[%d]=%ld",i,lProjVal[i]);
				}
				AfxMessageBox("Check lProjVal inside GetGridEdge()");
			}

			nError = GetEdge1D(&lProjVal[0],
					  nHeight,
					  pParm->nGroupFactor,
					  rect.top,
					  pParm->nThreshold,
					  pParm->nGradient,
					  pParm->nMethod,
					  pParm->nDirection,
					  m_EdgeFindRslt.dFndEdgePos[m_EdgeFindRslt.nLineCount],
					  m_EdgeFindRslt.nFndEdgeGrad[m_EdgeFindRslt.nLineCount],lDebugFlag);
			if (nError)
				return nError;
			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				for (j=0;j<m_EdgeFindRslt.dFndEdgePos[m_EdgeFindRslt.nLineCount][0];j++) {
					OutputDebugLog("GetGridEdge()->FndEdge::Line[%d] Edge[%d]=%3.2f EdgeType=[%d]",
									m_EdgeFindRslt.nLineCount,
									j+1,
									m_EdgeFindRslt.dFndEdgePos[m_EdgeFindRslt.nLineCount][j+1],
									m_EdgeFindRslt.nFndEdgeGrad[m_EdgeFindRslt.nLineCount][j+1]);
				}
			}
		}
	}
	m_EdgeFindRslt.bRsltValid = TRUE;
	if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_EDGE) {
		DrawGridEdge(pView,pParm,&m_EdgeFindRslt);
	}
	return 0;
}

int CGridEdge::GetLine(GRD_EDGE_FIND_PARM *pParm,
					   GRD_EDGE_FIND_RSLT *pRslt,
					   CLine2D *pLine)
{
	int i;
	int nRef;
	int nTotalEdge;
	CPoint2D pt2DAry[MAX_POINT];
//	double dXAry[MAX_POINT];
//	double dYAry[MAX_POINT];
	int nCntPt;
	int nError;

	nCntPt = 0;
	pLine->Reset();
	if (pRslt->bRsltValid) {
		if (pParm->nOrientation==COLUMN) {
			nRef = pParm->rectROI.left;
			for (i=0;i<pRslt->nLineCount;i++) {
				nTotalEdge = (int)pRslt->dFndEdgePos[i][0];
				if ( nTotalEdge>0) {
					pt2DAry[nCntPt].x = (double)nRef + (double)(i*(pParm->nGroupFactor+pParm->nSkipFactor));
//					if (pParm->nDirection==FORWARD)
						pt2DAry[nCntPt].y=pRslt->dFndEdgePos[i][1];
//					else
//						dYAry[nCntPt]=pRslt->dFndEdgePos[i][nTotalEdge];
					nCntPt++;
				}
			}
			nError = pLine->GetBestFittingLine(&pt2DAry[0],	nCntPt);
			if(nError==-1) {
				ASSERT(!"pLine->GetBestFittingLine() Error");
				goto ErrorAbort;
			}
		}
		else {
			nRef = pParm->rectROI.top;
			for (i=0;i<pRslt->nLineCount;i++) {
				nTotalEdge = (int)pRslt->dFndEdgePos[i][0];
				if ( nTotalEdge>0) {
					pt2DAry[nCntPt].y = (double)nRef + (double)(i*(pParm->nGroupFactor+pParm->nSkipFactor));
					if (pParm->nDirection==FORWARD)
						pt2DAry[nCntPt].x=pRslt->dFndEdgePos[i][1];
					else
						pt2DAry[nCntPt].x=pRslt->dFndEdgePos[i][nTotalEdge];
					nCntPt++;
				}
			}
			nError = pLine->GetBestFittingLine(&pt2DAry[0],	nCntPt);
			if(nError==-1) {
				ASSERT(!"pLine->GetBestFittingLine() Error");
				goto ErrorAbort;
			}
		}
	}
	else {
		return -2;
	}

	return 0;
ErrorAbort:
	return -1;
}

int CGridEdge::DrawGridEdge(IMAGEVIEW_INFO *pView,
							 GRD_EDGE_FIND_PARM *pParm,
							 GRD_EDGE_FIND_RSLT *pRslt)
{
	int i,j;
	CPoint pt1,pt2;
	LOGPEN PenInfo;

	PenInfo.lopnColor = RGB(0,255,0);
	PenInfo.lopnStyle = PS_SOLID;
	PenInfo.lopnWidth.x = 1;
	DrwRect(pView,&pParm->rectROI,&PenInfo);

	if (pParm->nOrientation==ROW) {
		for (i=0;i<pRslt->nLineCount;i++) {
			PenInfo.lopnColor = RGB(0,128,128);
			PenInfo.lopnStyle = PS_SOLID;
			PenInfo.lopnWidth.x = 0;

			pt1.x = pParm->rectROI.left;
			pt1.y = pParm->rectROI.top +
					i*(pParm->nGroupFactor + pParm->nSkipFactor);

			pt2.x = pParm->rectROI.right;
			pt2.y = pParm->rectROI.top +
					i*(pParm->nGroupFactor + pParm->nSkipFactor);

			DrwLine(pView,&pt1,&pt2,&PenInfo);

			//Draw 1st Point using a different color
			if ((int)pRslt->dFndEdgePos[i][0]>0) {
				PenInfo.lopnColor = RGB(255,0,0);
				PenInfo.lopnStyle = PS_SOLID;
				PenInfo.lopnWidth.x = 1;
				pt1.x = (int)pRslt->dFndEdgePos[i][1];
				pt1.y = pParm->rectROI.top +
						i*(pParm->nGroupFactor + pParm->nSkipFactor);
				DrwCur(pView,&pt1,&PenInfo,2);
			}

			//Draw other Point using a different color
			PenInfo.lopnColor = RGB(0,255,255);
			PenInfo.lopnStyle = PS_SOLID;
			PenInfo.lopnWidth.x = 1;
			for (j=2;j<=(int)pRslt->dFndEdgePos[i][0];j++) {
				pt1.x = (int)pRslt->dFndEdgePos[i][j];
				pt1.y = pParm->rectROI.top +
						i*(pParm->nGroupFactor + pParm->nSkipFactor);
				DrwCur(pView,&pt1,&PenInfo,2);
			}
		}
	}		
	else {
		for (i=0;i<pRslt->nLineCount;i++) {
			PenInfo.lopnColor = RGB(0,128,128);
			PenInfo.lopnStyle = PS_SOLID;
			PenInfo.lopnWidth.x = 0;

			pt1.x = pParm->rectROI.left +
					i*(pParm->nGroupFactor + pParm->nSkipFactor);
			pt1.y = pParm->rectROI.top;

			pt2.x = pParm->rectROI.left +
					i*(pParm->nGroupFactor + pParm->nSkipFactor);
			pt2.y = pParm->rectROI.bottom;

			DrwLine(pView,&pt1,&pt2,&PenInfo);

			//Draw 1st Point with a different color
			if ((int)pRslt->dFndEdgePos[i][0]>0) {
				PenInfo.lopnColor = RGB(255,0,0);
				PenInfo.lopnStyle = PS_SOLID;
				PenInfo.lopnWidth.x = 1;
				pt1.x = pParm->rectROI.left +
						i*(pParm->nGroupFactor + pParm->nSkipFactor);
				pt1.y = (int)pRslt->dFndEdgePos[i][1];
				DrwCur(pView,&pt1,&PenInfo,2);
			}
				
			PenInfo.lopnColor = RGB(0,255,255);
			PenInfo.lopnStyle = PS_SOLID;
			PenInfo.lopnWidth.x = 1;
			for (j=2;j<=pRslt->dFndEdgePos[i][0];j++) {
				pt1.x = pParm->rectROI.left +
						i*(pParm->nGroupFactor + pParm->nSkipFactor);
				pt1.y = (int)pRslt->dFndEdgePos[i][j];
				DrwCur(pView,&pt1,&PenInfo,2);
			}
		}
	}
	return 0;
}

CAdvGridEdge::CAdvGridEdge()
{
	m_EdgeFindRslt.nLineCount = 0;
	m_EdgeFindRslt.dR = 0;
	m_EdgeFindRslt.dTheta = 0;
	m_EdgeFindRslt.bRsltValid = FALSE;
}

CAdvGridEdge::~CAdvGridEdge()
{
}

int CAdvGridEdge::GetAdvGridEdge(IMAGEVIEW_INFO *pView,
								 CImageBuffer *pImg,
								 ADV_GRD_EDGE_FIND_PARM *pEdgeParm,
								 long lDebugFlag)
{

	long lProjVal[MAX_PROJ];
	int nError,i;
	CRect rect;
	int nSX,nSY;

	if(CheckRect(&pEdgeParm->rectROI,&pView->sizeImage))
		return -1;

	m_EdgeFindRslt.bRsltValid = FALSE;
	memset(lProjVal,-1,MAX_PROJ*sizeof(long));

	int nWidth = pEdgeParm->rectROI.Width();
	int nHeight = pEdgeParm->rectROI.Height();
	if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
		OutputDebugLog("GetAdvGridEdge()-> width(%d),Height(%d)",nWidth,nHeight);
	}

	pEdgeParm->nGridSpacing = max(pEdgeParm->nGridSpacing,1);

	if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
		OutputDebugLog("GetAdvGridEdge()->rectROI=(%d,%d,%d,%d)",
						pEdgeParm->rectROI.left,
						pEdgeParm->rectROI.top,
						pEdgeParm->rectROI.right,
						pEdgeParm->rectROI.bottom);
	}
	if (pEdgeParm->nOrientation==ROW) {
		if (nWidth>MAX_PROJ) {
			AfxMessageBox("nWidth > MAX_PROJ Size in GetAdvGridEdge()");
			return -9;
		}
		rect.left = pEdgeParm->rectROI.left;
		rect.right = pEdgeParm->rectROI.right;
		for (nSY=pEdgeParm->rectROI.top,m_EdgeFindRslt.nLineCount=0;
			 nSY<pEdgeParm->rectROI.bottom;
			 nSY+=pEdgeParm->nGridSpacing,
			 m_EdgeFindRslt.nLineCount++) {
			if (m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT) {
				AfxMessageBox("m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT in GetAdvGridEdge()");
				return -9;
			}
		
			rect.top = nSY;
			rect.bottom = nSY;
			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				OutputDebugLog("GetAdvGridEdge()->ROW rect=(%d,%d,%d,%d)",
								rect.left,
								rect.top,
								rect.right,
								rect.bottom);
			} 
			nError = GetXGrayAccl(pImg, &rect, &lProjVal[0]);
			if (nError)
				return -1;

			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				for (i=0;i<nWidth;i++) {
					OutputDebugLog("GetAdvGridEdge()->lProjVal[%d]=%ld",i,lProjVal[i]);
				}
				AfxMessageBox("Check lProjVal inside GetAdvGridEdge()");
			}
			nError = GetEdgeEx(&lProjVal[0],
							   nWidth,
							   pEdgeParm,
 							   m_EdgeFindRslt.dFndEdgePos[m_EdgeFindRslt.nLineCount],
							   m_EdgeFindRslt.nFndEdgeGrad[m_EdgeFindRslt.nLineCount]);
			
			if (nError)
				return -1;
		}
	}
	else {
		if (nHeight > MAX_PROJ) {
			AfxMessageBox("nHeight > MAX_PROJ Size in GetAdvGridEdge()");
			return -9;
		}
		rect.top = pEdgeParm->rectROI.top;
		rect.bottom = pEdgeParm->rectROI.bottom;

		for (nSX=pEdgeParm->rectROI.left,m_EdgeFindRslt.nLineCount=0;
			 nSX<pEdgeParm->rectROI.right;

			 nSX+=pEdgeParm->nGridSpacing,
			 m_EdgeFindRslt.nLineCount++) {
			if (m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT) {
				AfxMessageBox("m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT in GetAdvGridEdge()");
				return -9;
			}

			rect.left = nSX;
			rect.right = nSX;

			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				OutputDebugLog("GetGridEdge()->COL rect=(%d,%d,%d,%d)",
								rect.left,
								rect.top,
								rect.right,
								rect.bottom);
			}

			nError = GetYGrayAccl(pImg, &rect, &lProjVal[0]);
			if (nError)
				return -1;

			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				for (i=0;i<nHeight;i++) {
					OutputDebugLog("GetGridEdge()->lProjVal[%d]=%ld",i,lProjVal[i]);
				}
				AfxMessageBox("Check lProjVal inside GetAdvGridEdge()");
			}
			nError = GetEdgeEx(&lProjVal[0],
							   nHeight,
							   pEdgeParm,
 							   m_EdgeFindRslt.dFndEdgePos[m_EdgeFindRslt.nLineCount],
							   m_EdgeFindRslt.nFndEdgeGrad[m_EdgeFindRslt.nLineCount]);

			if (nError)
				return -1;

		}
	}

	m_EdgeFindRslt.bRsltValid = TRUE;

	if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_EDGE) {
		DrawAdvGridEdge(pView,pEdgeParm,&m_EdgeFindRslt);
	}

	return 0;
}

int CAdvGridEdge::GetAdvGridEdgeII(IMAGEVIEW_INFO *pView,
								   CImageBuffer *pImg,
								   ADV_GRD_EDGE_FIND_PARM *pEdgeParm,
								   long lDebugFlag)

{
	long lProjVal[MAX_PROJ];
	int nError,i;
	CRect rect;
	int nSX,nSY;
	double *ptrMask;
	int nWidth;
	int nHeight;

	ptrMask = 0;

	///////////////////////////////////////
	CPerformanceTimer Timer;
	BOOL bDebugThis;
	bDebugThis=FALSE;
	///////////////////////////////////////


	nWidth = pEdgeParm->rectROI.Width();
	nHeight = pEdgeParm->rectROI.Height();

	if(CheckRect(&pEdgeParm->rectROI,&pView->sizeImage))
		goto ErrorAbort;

	if (pEdgeParm->nSmoothingMethod == BINOMIAL_SMOOTHING) {
		ptrMask = new double[abs(pEdgeParm->nEdgeFilterSize)];
		Create1DBinomialMask((pEdgeParm->nEdgeFilterSize-1)/2,ptrMask,TRUE);
	}

	m_EdgeFindRslt.bRsltValid = FALSE;
	memset(lProjVal,-1,MAX_PROJ*sizeof(long));

	if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
		OutputDebugLog("GetAdvGridEdgeII()-> width(%d),Height(%d)",nWidth,nHeight);
	}

	pEdgeParm->nGridSpacing = max(pEdgeParm->nGridSpacing,1);

	if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
		OutputDebugLog("GetAdvGridEdgeII()->rectROI=(%d,%d,%d,%d)",
						pEdgeParm->rectROI.left,
						pEdgeParm->rectROI.top,
						pEdgeParm->rectROI.right,
						pEdgeParm->rectROI.bottom);
	}
	if (pEdgeParm->nOrientation==ROW) {
		if (nWidth>MAX_PROJ) {
			AfxMessageBox("nWidth > MAX_PROJ Size in GetAdvGridEdgeII()");
			goto ErrorOverFlow;
		}
		rect.left = pEdgeParm->rectROI.left;
		rect.right = pEdgeParm->rectROI.right;
		for (nSY=pEdgeParm->rectROI.top,m_EdgeFindRslt.nLineCount=0;
			 nSY<pEdgeParm->rectROI.bottom;
			 nSY+=pEdgeParm->nGridSpacing,
			 m_EdgeFindRslt.nLineCount++) {
			if (m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT) {
				AfxMessageBox("m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT in GetAdvGridEdgeII()");
				goto ErrorOverFlow;
			}
		
			rect.top = nSY;
			rect.bottom = nSY+1;
			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				OutputDebugLog("GetAdvGridEdgeII()->ROW rect=(%d,%d,%d,%d)",
								rect.left,
								rect.top,
								rect.right,
								rect.bottom);
			}
			if (bDebugThis)
				Timer.Reset();
			nError = GetXGrayAccl(pImg, &rect, &lProjVal[0]);
			if (nError)
				goto ErrorAbort;
			if (bDebugThis)
				OutputDebugLog("GetXGrayAccl=%3.3f",Timer.GetElapsedMSec());

			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				for (i=0;i<nWidth;i++) {
					OutputDebugLog("GetAdvGridEdgeII()->lProjVal[%d]=%ld",i,lProjVal[i]);
				}
				AfxMessageBox("Check lProjVal inside GetAdvGridEdgeII()");
			}

			if (bDebugThis)
				Timer.Reset();
			nError = GetEdgeExNew(&lProjVal[0],
							   nWidth,
							   ptrMask,
							   pEdgeParm,
 							   m_EdgeFindRslt.dFndEdgePos[m_EdgeFindRslt.nLineCount],
							   m_EdgeFindRslt.nFndEdgeGrad[m_EdgeFindRslt.nLineCount]);
			
			if (nError)
				goto ErrorAbort;
			if (bDebugThis)
				OutputDebugLog("GetEdgeExNew=%3.3f",Timer.GetElapsedMSec());
		}
	}
	else {
		if (nHeight > MAX_PROJ) {
			AfxMessageBox("nHeight > MAX_PROJ Size in GetAdvGridEdgeII()");
			goto ErrorOverFlow;
		}
		rect.top = pEdgeParm->rectROI.top;
		rect.bottom = pEdgeParm->rectROI.bottom;

		for (nSX=pEdgeParm->rectROI.left,m_EdgeFindRslt.nLineCount=0;
			 nSX<pEdgeParm->rectROI.right;
			 nSX+=pEdgeParm->nGridSpacing,
			 m_EdgeFindRslt.nLineCount++) {
			if (m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT) {
				AfxMessageBox("m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT in GetAdvGridEdgeII()");
				goto ErrorOverFlow;
			}

			rect.left = nSX;
			rect.right = nSX+1;

			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				OutputDebugLog("GetAdvGridEdgeII()->COL rect=(%d,%d,%d,%d)",
								rect.left,
								rect.top,
								rect.right,
								rect.bottom);
			}

			nError = GetYGrayAccl(pImg, &rect, &lProjVal[0]);
			if (nError)
				goto ErrorAbort;

			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				for (i=0;i<nHeight;i++) {
					OutputDebugLog("GetAdvGridEdgeII()->lProjVal[%d]=%ld",i,lProjVal[i]);
				}
				AfxMessageBox("Check lProjVal inside GetAdvGridEdgeII()");
			}
			nError = GetEdgeExNew(&lProjVal[0],
							   nHeight,
							   ptrMask,
							   pEdgeParm,
 							   m_EdgeFindRslt.dFndEdgePos[m_EdgeFindRslt.nLineCount],
							   m_EdgeFindRslt.nFndEdgeGrad[m_EdgeFindRslt.nLineCount]);

			if (nError)
				goto ErrorAbort;
		}
	}

	m_EdgeFindRslt.bRsltValid = TRUE;

	if (ptrMask) {
		delete [] ptrMask;
	}

	if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_EDGE) {
		DrawAdvGridEdge(pView,pEdgeParm,&m_EdgeFindRslt);
	}

	return 0;

ErrorOverFlow:
	if (ptrMask) {
		delete [] ptrMask;
	}
	return -9;
	
ErrorAbort:
	if (ptrMask) {
		delete [] ptrMask;
	}
	return -1;

}

int CAdvGridEdge::GetAdvGridEdgeIII(IMAGEVIEW_INFO *pView,
									CImageBuffer *pImg,
								    ADV_GRD_EDGE_FIND_PARM *pEdgeParm,
								    CPoint ptStart,
								    CPoint ptEnd,
								    int nSrchWidth,
								    long lDebugFlag)
{
	long lProjVal[MAX_PROJ];
	int nError,i;
	CRect rect;
	int nSX,nSY;
	double *ptrMask;
	int nWidth;
	int nHeight;
	CPoint ptEstEdge;
	float fRatio;
	int nDiff;

	int nXStart,nXEnd,nYStart,nYEnd;

	nXStart = min(ptStart.x,ptEnd.x);
	nXEnd   = max(ptStart.x,ptEnd.x);
	nYStart = min(ptStart.y,ptEnd.y);
	nYEnd   = max(ptStart.y,ptEnd.y);

	ptrMask = 0;

	///////////////////////////////////////
	CPerformanceTimer Timer;
	BOOL bDebugThis;
	bDebugThis=FALSE;
	///////////////////////////////////////


	nWidth = nSrchWidth;
	nHeight = nSrchWidth;

	if(CheckRect(&pEdgeParm->rectROI,&pView->sizeImage))
		goto ErrorAbort;

	if (pEdgeParm->nSmoothingMethod == BINOMIAL_SMOOTHING) {
		ptrMask = new double[abs(pEdgeParm->nEdgeFilterSize)];
		Create1DBinomialMask((pEdgeParm->nEdgeFilterSize-1)/2,ptrMask,TRUE);
	}

	m_EdgeFindRslt.bRsltValid = FALSE;
	memset(lProjVal,-1,MAX_PROJ*sizeof(long));

	if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
		OutputDebugLog("GetAdvGridEdgeIII()-> width(%d),Height(%d)",nWidth,nHeight);
	}

	pEdgeParm->nGridSpacing = max(pEdgeParm->nGridSpacing,1);

	if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
		OutputDebugLog("GetAdvGridEdgeIII()->rectROI=(%d,%d,%d,%d)",
						pEdgeParm->rectROI.left,
						pEdgeParm->rectROI.top,
						pEdgeParm->rectROI.right,
						pEdgeParm->rectROI.bottom);
	}
	if (pEdgeParm->nOrientation==ROW) {
		if (nWidth>MAX_PROJ) {
			AfxMessageBox("nWidth > MAX_PROJ Size in GetAdvGridEdgeIII()");
			goto ErrorOverFlow;
		}
		ptEstEdge = ptStart;
		for (nSY=nYStart,m_EdgeFindRslt.nLineCount=0;
			 nSY<nYEnd;
			 nSY+=pEdgeParm->nGridSpacing,
			 m_EdgeFindRslt.nLineCount++) {
			if (m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT) {
				AfxMessageBox("m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT in GetAdvGridEdgeIII()");
				goto ErrorOverFlow;
			}
			fRatio = (float)(nSY-nYStart)/(float)(nYEnd-nYStart+1);
			nDiff = (int)(fRatio*(float)(nXEnd-nXStart+1));
			if (ptStart.x < ptEnd.x)
				ptEstEdge.x = nXStart + nDiff;
			else
				ptEstEdge.x = nXEnd - nDiff;
			
			rect.left = ptEstEdge.x - nWidth;
			rect.right = ptEstEdge.x + nWidth-1;
			rect.top = nSY;
			rect.bottom = nSY+1;
			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				OutputDebugLog("GetAdvGridEdgeIII()->ROW rect=(%d,%d,%d,%d)",
								rect.left,
								rect.top,
								rect.right,
								rect.bottom);
			}
			if (bDebugThis)
				Timer.Reset();
			nError = GetXGrayAccl(pImg, &rect, &lProjVal[0]);
			if (nError)
				goto ErrorAbort;
			if (bDebugThis)
				OutputDebugLog("GetXGrayAccl=%3.3f",Timer.GetElapsedMSec());

			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				for (i=0;i<nWidth;i++) {
					OutputDebugLog("GetAdvGridEdgeIII()->lProjVal[%d]=%ld",i,lProjVal[i]);
				}
				AfxMessageBox("Check lProjVal inside GetAdvGridEdgeIII()");
			}

			if (bDebugThis)
				Timer.Reset();
			nError = GetEdgeExNew(&lProjVal[0],
							   nWidth*2,
							   ptrMask,
							   pEdgeParm,
 							   m_EdgeFindRslt.dFndEdgePos[m_EdgeFindRslt.nLineCount],
							   m_EdgeFindRslt.nFndEdgeGrad[m_EdgeFindRslt.nLineCount],
							   (ptEstEdge.x - nWidth)-pEdgeParm->rectROI.left);
			
			if (nError)
				goto ErrorAbort;
			if (bDebugThis)
				OutputDebugLog("GetEdgeExNew=%3.3f",Timer.GetElapsedMSec());
		}
	}
	else {
		if (nHeight > MAX_PROJ) {
			AfxMessageBox("nHeight > MAX_PROJ Size in GetAdvGridEdgeIII()");
			goto ErrorOverFlow;
		}
		ptEstEdge = ptStart;
		for (nSX=nXStart,m_EdgeFindRslt.nLineCount=0;
			 nSX<nXEnd;
			 nSX+=pEdgeParm->nGridSpacing,
			 m_EdgeFindRslt.nLineCount++) {
			if (m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT) {
				AfxMessageBox("m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT in GetAdvGridEdgeIII()");
				goto ErrorOverFlow;
			}

			fRatio = (float)(nSX-nXStart)/(float)(nXEnd-nXStart+1);
			nDiff = (int)(fRatio*(float)(nYEnd-nYStart+1));
			if (ptStart.y < ptEnd.y)
				ptEstEdge.y = nYStart + nDiff;
			else
				ptEstEdge.y = nYEnd - nDiff;
			
			rect.top = ptEstEdge.y - nHeight;
			rect.bottom = ptEstEdge.y + nHeight-1;
			rect.left = nSX;
			rect.right = nSX+1;

			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				OutputDebugLog("GetAdvGridEdgeIII()->COL rect=(%d,%d,%d,%d)",
								rect.left,
								rect.top,
								rect.right,
								rect.bottom);
			}

			nError = GetYGrayAccl(pImg, &rect, &lProjVal[0]);
			if (nError)
				goto ErrorAbort;

			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				for (i=0;i<nHeight;i++) {
					OutputDebugLog("GetAdvGridEdgeIII()->lProjVal[%d]=%ld",i,lProjVal[i]);
				}
				AfxMessageBox("Check lProjVal inside GetAdvGridEdgeIII()");
			}
			nError = GetEdgeExNew(&lProjVal[0],
							   nHeight*2,
							   ptrMask,
							   pEdgeParm,
 							   m_EdgeFindRslt.dFndEdgePos[m_EdgeFindRslt.nLineCount],
							   m_EdgeFindRslt.nFndEdgeGrad[m_EdgeFindRslt.nLineCount],
							   (ptEstEdge.y - nHeight)-pEdgeParm->rectROI.top);

			if (nError)
				goto ErrorAbort;
		}
	}

	m_EdgeFindRslt.bRsltValid = TRUE;

	if (ptrMask) {
		delete [] ptrMask;
	}

	if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_EDGE) {
		DrawAdvGridEdgeIII(pView,pEdgeParm,ptStart,ptEnd,nSrchWidth,&m_EdgeFindRslt);
	}

	return 0;

ErrorOverFlow:
	if (ptrMask) {
		delete [] ptrMask;
	}
	return -9;
	
ErrorAbort:
	if (ptrMask) {
		delete [] ptrMask;
	}
	return -1;

}

int CAdvGridEdge::GetAdvGridEdgeIV(IMAGEVIEW_INFO *pView,
								   CImageBuffer *pImg,
								   ADV_GRD_EDGE_FIND_PARM *pEdgeParm,
								   CPoint ptStart,
								   CPoint ptEnd,
								   int nSrchWidth,
								   long lDebugFlag)
{

	BYTE byteProjVal[MAX_PROJ];
	CPoint ptImgLnStart;
	int nImgLnSize;

	int nError,i;
	CRect rect;
	int nSX,nSY;
	double *ptrMask;
	int nWidth;
	int nHeight;
	CPoint ptEstEdge;
	float fRatio;
	int nDiff;

	int nXStart,nXEnd,nYStart,nYEnd;

	nXStart = min(ptStart.x,ptEnd.x);
	nXEnd   = max(ptStart.x,ptEnd.x);
	nYStart = min(ptStart.y,ptEnd.y);
	nYEnd   = max(ptStart.y,ptEnd.y);

	ptrMask = 0;

	///////////////////////////////////////
	CPerformanceTimer Timer;
	BOOL bDebugThis;
	bDebugThis=FALSE;
	///////////////////////////////////////


	nWidth = nSrchWidth;
	nHeight = nSrchWidth;

	if(CheckRect(&pEdgeParm->rectROI,&pView->sizeImage))
		goto ErrorAbort;

	if (pEdgeParm->nSmoothingMethod == BINOMIAL_SMOOTHING) {
		ptrMask = new double[abs(pEdgeParm->nEdgeFilterSize)];
		Create1DBinomialMask((pEdgeParm->nEdgeFilterSize-1)/2,ptrMask,TRUE);
	}

	m_EdgeFindRslt.bRsltValid = FALSE;

	if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
		OutputDebugLog("GetAdvGridEdgeIII()-> width(%d),Height(%d)",nWidth,nHeight);
	}

	pEdgeParm->nGridSpacing = max(pEdgeParm->nGridSpacing,1);

	if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
		OutputDebugLog("GetAdvGridEdgeIII()->rectROI=(%d,%d,%d,%d)",
						pEdgeParm->rectROI.left,
						pEdgeParm->rectROI.top,
						pEdgeParm->rectROI.right,
						pEdgeParm->rectROI.bottom);
	}
	if (pEdgeParm->nOrientation==ROW) {
		if (nWidth>MAX_PROJ) {
			AfxMessageBox("nWidth > MAX_PROJ Size in GetAdvGridEdgeIII()");
			goto ErrorOverFlow;
		}
		ptEstEdge = ptStart;
		for (nSY=nYStart,m_EdgeFindRslt.nLineCount=0;
			 nSY<nYEnd;
			 nSY+=pEdgeParm->nGridSpacing,
			 m_EdgeFindRslt.nLineCount++) {
			if (m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT) {
				AfxMessageBox("m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT in GetAdvGridEdgeIII()");
				goto ErrorOverFlow;
			}
			fRatio = (float)(nSY-nYStart)/(float)(nYEnd-nYStart+1);
			nDiff = (int)(fRatio*(float)(nXEnd-nXStart+1));
			if (ptStart.x < ptEnd.x)
				ptEstEdge.x = nXStart + nDiff;
			else
				ptEstEdge.x = nXEnd - nDiff;

			ptImgLnStart = CPoint(ptEstEdge.x - nWidth - nHeight, nSY);
			nImgLnSize = 2 * nWidth;

			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				OutputDebugLog("GetAdvGridEdgeIII()->ROW ptImgLnStart=(%d,%d)   nImgLnSize =%d",
								ptImgLnStart.x,
								ptImgLnStart.y,
								nImgLnSize);
			}
			if (bDebugThis)
				Timer.Reset();
			
			nError = GetImgXLine(pImg, &ptImgLnStart, nImgLnSize, &byteProjVal[0]);

			if (nError)
				goto ErrorAbort;
			if (bDebugThis)
				OutputDebugLog("GetXGrayAccl=%3.3f",Timer.GetElapsedMSec());

			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				for (i=0;i<nWidth;i++) {
					OutputDebugLog("GetAdvGridEdgeIII()->byteProjVal[%d]=%ld",i,byteProjVal[i]);
				}
				AfxMessageBox("Check lProjVal inside GetAdvGridEdgeIII()");
			}

			if (bDebugThis)
				Timer.Reset();
			nError = GetEdgeExNew2(&byteProjVal[0],
							   nWidth*2,
							   ptrMask,
							   pEdgeParm,
 							   m_EdgeFindRslt.dFndEdgePos[m_EdgeFindRslt.nLineCount],
							   m_EdgeFindRslt.nFndEdgeGrad[m_EdgeFindRslt.nLineCount],
							   (ptEstEdge.x - nWidth)-pEdgeParm->rectROI.left);
			
			if (nError)
				goto ErrorAbort;
			if (bDebugThis)
				OutputDebugLog("GetEdgeExNew=%3.3f",Timer.GetElapsedMSec());
		}
	}
	else {
		if (nHeight > MAX_PROJ) {
			AfxMessageBox("nHeight > MAX_PROJ Size in GetAdvGridEdgeIII()");
			goto ErrorOverFlow;
		}
		ptEstEdge = ptStart;
		for (nSX=nXStart,m_EdgeFindRslt.nLineCount=0;
			 nSX<nXEnd;
			 nSX+=pEdgeParm->nGridSpacing,
			 m_EdgeFindRslt.nLineCount++) {
			if (m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT) {
				AfxMessageBox("m_EdgeFindRslt.nLineCount>MAX_LINE_COUNT in GetAdvGridEdgeIII()");
				goto ErrorOverFlow;
			}

			fRatio = (float)(nSX-nXStart)/(float)(nXEnd-nXStart+1);
			nDiff = (int)(fRatio*(float)(nYEnd-nYStart+1));
			if (ptStart.y < ptEnd.y)
				ptEstEdge.y = nYStart + nDiff;
			else
				ptEstEdge.y = nYEnd - nDiff;
			
			ptImgLnStart = CPoint(nSX, ptEstEdge.y - nHeight);
			nImgLnSize = 2 * nHeight;

			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				OutputDebugLog("GetAdvGridEdgeIII()->ROW ptImgLnStart=(%d,%d)   nImgLnSize =%d",
								ptImgLnStart.x,
								ptImgLnStart.y,
								nImgLnSize);
			}

			nError = GetImgYLine(pImg, &ptImgLnStart, nImgLnSize, &byteProjVal[0]);

			if (nError)
				goto ErrorAbort;

			if (lDebugFlag&DEBUG_PRINT_EXT && lDebugFlag&DEBUG_EDGE) {
				for (i=0;i<nHeight;i++) {
					OutputDebugLog("GetAdvGridEdgeIII()->byteProjVal[%d]=%ld",i,byteProjVal[i]);
				}
				AfxMessageBox("Check lProjVal inside GetAdvGridEdgeIII()");
			}
			nError = GetEdgeExNew2(&byteProjVal[0],
							   nHeight*2,
							   ptrMask,
							   pEdgeParm,
 							   m_EdgeFindRslt.dFndEdgePos[m_EdgeFindRslt.nLineCount],
							   m_EdgeFindRslt.nFndEdgeGrad[m_EdgeFindRslt.nLineCount],
							   (ptEstEdge.y - nHeight)-pEdgeParm->rectROI.top);

			if (nError)
				goto ErrorAbort;
		}
	}

	m_EdgeFindRslt.bRsltValid = TRUE;

	if (ptrMask) {
		delete [] ptrMask;
	}

	if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_EDGE) {
		DrawAdvGridEdgeIII(pView,pEdgeParm,ptStart,ptEnd,nSrchWidth,&m_EdgeFindRslt);
	}

	return 0;

ErrorOverFlow:
	if (ptrMask) {
		delete [] ptrMask;
	}
	return -9;
	
ErrorAbort:
	if (ptrMask) {
		delete [] ptrMask;
	}
	return -1;


}

int CAdvGridEdge::GetHoughLine(ADV_GRD_EDGE_FIND_PARM *pEdgeParm,
							   LINE_FIND_PARM *pLineParm,
							   ADV_GRD_EDGE_FIND_RSLT *pRslt,
							   CLine2D *pLine)
{
    static double pdX[MAX_POINTS_ACCEPTED_BY_HOUGHTRANSFORM];
    static double pdY[MAX_POINTS_ACCEPTED_BY_HOUGHTRANSFORM];
	double        dTheta, dR, *pFixed, *pVariable;
//	double		  pdRefinedX[100], pdRefinedY[100];
	CPoint2D	  pt2DRefined[100];
	double		  dTempX, dTempY, dTempDistance, dMinDistance;
	int           nCount, i, j, nn, nm, nSelectedEdgePos;
	BOOL		  bHori;
	int			  nError;

	////////////////////////////////////////////
	BOOL bDebugThis;
	bDebugThis=FALSE;
	////////////////////////////////////////////

	nCount = 0;
	pRslt->dR = -1.0;
	nSelectedEdgePos = 3;

	if (pEdgeParm->nOrientation == COLUMN) {
		pFixed    = pdX;
		pVariable = pdY;
		bHori = TRUE;
	}				
	else {
		pFixed    = pdY;
		pVariable = pdX;
		bHori = FALSE;
	}

	if(nSelectedEdgePos * pRslt->nLineCount > MAX_POINTS_ACCEPTED_BY_HOUGHTRANSFORM) {
		nSelectedEdgePos = 1;

		if(pRslt->nLineCount > MAX_POINTS_ACCEPTED_BY_HOUGHTRANSFORM) {
			AfxMessageBox(" pRslt->nLineCount exceeds MAX_POINTS_ACCEPTED_BY_HOUGHTRANSFORM in CGridEdge::GetHoughLine()");
			return -1;
		}
	}

	for(i = 0; i < pRslt->nLineCount; i++) {
		nn = (int)pRslt->dFndEdgePos[i][0];
		for(j = 0; (j < nn) && (j < nSelectedEdgePos); j++) {
			pFixed[nCount] = i * pEdgeParm->nGridSpacing;

			if(pEdgeParm->nDirection==FORWARD) 
				pVariable[nCount] = pRslt->dFndEdgePos[i][j + 1];
			else	
				pVariable[nCount] = pRslt->dFndEdgePos[i][nn - j];

			nCount++;
		}
	}

	nm = HoughTransform(pEdgeParm->rectROI,pdX,pdY,nCount,pLineParm,dTheta,dR,pRslt->dRange);

	if (bDebugThis)
		OutputDebugLog("Raw Votes=%d",nm);
	nCount = -1;
	bool bStart;
	double dCosTheta,dSinTheta;

	dCosTheta = cos(dTheta*3.141592654/180.0);
	dSinTheta = sin(dTheta*3.141592654/180.0);

	for(i = 0; i < pRslt->nLineCount; i++) {
		nn = (int)pRslt->dFndEdgePos[i][0];
		dMinDistance = 2.0;
		bStart = true;
		for(j = 0; (j < nn) && (j < nSelectedEdgePos); j++) {
			if (pEdgeParm->nOrientation == COLUMN) {			
				dTempX = i * pEdgeParm->nGridSpacing;
			}
			else {
				dTempY = i * pEdgeParm->nGridSpacing;
			}
			if(pEdgeParm->nDirection==FORWARD) {
				if (pEdgeParm->nOrientation == COLUMN) {			
					dTempY = pRslt->dFndEdgePos[i][j + 1];
				}
				else {
					dTempX = pRslt->dFndEdgePos[i][j + 1];;
				}
			}
			else {
				if (pEdgeParm->nOrientation == COLUMN) {			
					dTempY = pRslt->dFndEdgePos[i][nn - j];
				}
				else {
					dTempX = pRslt->dFndEdgePos[i][nn - j];
				}
			}
			dTempDistance = fabs(dR - dTempX*dCosTheta - dTempY*dSinTheta);
			if (dTempDistance < dMinDistance) {
				if (bStart) {
					nCount++;
					bStart = false;
				}
//				pdRefinedX[nCount] = dTempX;
//				pdRefinedY[nCount] = dTempY;
				pt2DRefined[nCount] = CPoint2D(dTempX, dTempY);
				dMinDistance = dTempDistance;
			}
		}
	}
	nCount++;
	if (nCount > 10) {
//		pLine->GetBestFittingLine(pdRefinedX, pdRefinedY, nCount, bHori);
		nError = pLine->GetBestFittingLine(&pt2DRefined[0],	nCount);
		if(nError==-1) {
			ASSERT(!"pLine->GetBestFittingLine() Error");
			goto ErrorAbort;
		}

//		pLine->Position.x += pEdgeParm->rectROI.left;
//		pLine->Position.y += pEdgeParm->rectROI.top;
		pLine->SetPosition(pLine->GetPosition().x + pEdgeParm->rectROI.left,
				pLine->GetPosition().y + pEdgeParm->rectROI.top);
		if (bDebugThis)
			OutputDebugLog("direction.x = %f, direction.y = %f", pLine->GetDirection().x, pLine->GetDirection().y);
	}
	else {
		return -1;
	}
	if (bDebugThis)
		OutputDebugLog("Refined Votes=%d",nCount);

	return 0;
ErrorAbort:
	return -1;
}

int CAdvGridEdge::GetHoughLineII(ADV_GRD_EDGE_FIND_PARM *pEdgeParm,
							   LINE_FIND_PARM *pLineParm,
							   ADV_GRD_EDGE_FIND_RSLT *pRslt,
							   CLine2D *pLine,
							   int nPtSelCnt)
{
    static int pnX[MAX_POINTS_ACCEPTED_BY_HOUGHTRANSFORM];
    static int pnY[MAX_POINTS_ACCEPTED_BY_HOUGHTRANSFORM];
	double     dR, dTheta;
	int *pFixed, *pVariable;
//	double		  pdRefinedX[100], pdRefinedY[100];
	CPoint2D	  pt2DRefined[100];
	double		  dTempX, dTempY, dTempDistance, dMinDistance;
	int           nCount, i, j, nn, nm, nSelectedEdgePos;
	BOOL		  bHori;
	int nMaxPt;
	int			  nError;

	////////////////////////////////////////////
	BOOL bDebugThis;
	bDebugThis=FALSE;
	////////////////////////////////////////////

	nCount = 0;
	pRslt->dR = -1.0;
	nSelectedEdgePos = nPtSelCnt;


	if (pEdgeParm->nOrientation == COLUMN) {
		pFixed    = pnX;
		pVariable = pnY;
		bHori = TRUE;
	}				
	else {
		pFixed    = pnY;
		pVariable = pnX;
		bHori = FALSE;
	}

	nMaxPt = MAX_POINTS_ACCEPTED_BY_HOUGHTRANSFORM/pRslt->nLineCount;
	if (nSelectedEdgePos > nMaxPt)
		nSelectedEdgePos = nMaxPt;


	if(nSelectedEdgePos * pRslt->nLineCount > MAX_POINTS_ACCEPTED_BY_HOUGHTRANSFORM) {
		nSelectedEdgePos = 1;
		OutputDebugLog("HTII Still Exceed !!!, Pls Check !!!");

		if(pRslt->nLineCount > MAX_POINTS_ACCEPTED_BY_HOUGHTRANSFORM) {
			AfxMessageBox(" pRslt->nLineCount exceeds MAX_POINTS_ACCEPTED_BY_HOUGHTRANSFORM in CGridEdge::GetHoughLine()");
			return -1;
		}
	}

	for(i = 0; i < pRslt->nLineCount; i++) {
		nn = (int)pRslt->dFndEdgePos[i][0];
		for(j = 0; (j < nn) && (j < nSelectedEdgePos); j++) {
			pFixed[nCount] = i * pEdgeParm->nGridSpacing;

			if(pEdgeParm->nDirection==FORWARD) 
				pVariable[nCount] = (int)pRslt->dFndEdgePos[i][j + 1];
			else	
				pVariable[nCount] = (int)pRslt->dFndEdgePos[i][nn - j];

			nCount++;
		}
	}

	nm = HoughTransformNew(pEdgeParm->rectROI,
						   pnX,
						   pnY,
						   nCount,
						   pLineParm,
						   dTheta,
						   dR,
						   pRslt->dRange);

	if (bDebugThis)
		OutputDebugLog("Raw Votes=%d",nm);
	nCount = -1;
	bool bStart;
	double dCosTheta,dSinTheta;

	dCosTheta = cos(dTheta*3.141592654/180.0);
	dSinTheta = sin(dTheta*3.141592654/180.0);

	for(i = 0; i < pRslt->nLineCount; i++) {
		nn = (int)pRslt->dFndEdgePos[i][0];
		dMinDistance = 5.0;
		bStart = true;
		for(j = 0; (j < nn) && (j < nSelectedEdgePos); j++) {
			if (pEdgeParm->nOrientation == COLUMN) {			
				dTempX = i * pEdgeParm->nGridSpacing;
			}
			else {
				dTempY = i * pEdgeParm->nGridSpacing;
			}
			if(pEdgeParm->nDirection==FORWARD) {
				if (pEdgeParm->nOrientation == COLUMN) {			
					dTempY = pRslt->dFndEdgePos[i][j + 1];
				}
				else {
					dTempX = pRslt->dFndEdgePos[i][j + 1];;
				}
			}
			else {
				if (pEdgeParm->nOrientation == COLUMN) {			
					dTempY = pRslt->dFndEdgePos[i][nn - j];
				}
				else {
					dTempX = pRslt->dFndEdgePos[i][nn - j];
				}
			}
			dTempDistance = fabs(dR - dTempX*dCosTheta - dTempY*dSinTheta);
			if (dTempDistance < dMinDistance) {
				if (bStart) {
					nCount++;
					bStart = false;
				}
//				pdRefinedX[nCount] = dTempX;
//				pdRefinedY[nCount] = dTempY;
				pt2DRefined[nCount] = CPoint2D(dTempX, dTempY);
				dMinDistance = dTempDistance;
			}
		}
	}
	nCount++;
	if (nCount > 10) {
//		pLine->GetBestFittingLine(pdRefinedX, pdRefinedY, nCount, bHori);
		nError = pLine->GetBestFittingLine(&pt2DRefined[0],	nCount);
		if(nError==-1) {
			ASSERT(!"pLine->GetBestFittingLine() Error");
			goto ErrorAbort;
		}
//		pLine->Position.x += pEdgeParm->rectROI.left;
//		pLine->Position.y += pEdgeParm->rectROI.top;
		pLine->SetPosition(pLine->GetPosition().x + pEdgeParm->rectROI.left,
				pLine->GetPosition().y + pEdgeParm->rectROI.top);
		if (bDebugThis)
			OutputDebugLog("direction.x = %f, direction.y = %f", pLine->GetDirection().x, pLine->GetDirection().y);
	}
	else {
		return -1;
	}
	if (bDebugThis)
		OutputDebugLog("Refined Votes=%d",nCount);

	return 0;
ErrorAbort:
	return -1;
}

int CAdvGridEdge::DrawAdvGridEdge(IMAGEVIEW_INFO *pView,
							 ADV_GRD_EDGE_FIND_PARM *pParm,
							 ADV_GRD_EDGE_FIND_RSLT *pRslt)
{
	int i,j;
	CPoint pt1,pt2;
	LOGPEN PenInfo;

	PenInfo.lopnColor = RGB(0,255,0);
	PenInfo.lopnStyle = PS_SOLID;
	PenInfo.lopnWidth.x = 1;
	DrwRect(pView,&pParm->rectROI,&PenInfo);

	if (pParm->nOrientation==ROW) {
		for (i=0;i<pRslt->nLineCount;i++) {
			PenInfo.lopnColor = RGB(0,128,128);
			PenInfo.lopnStyle = PS_SOLID;
			PenInfo.lopnWidth.x = 0;

			pt1.x = pParm->rectROI.left;
			pt1.y = pParm->rectROI.top +
					i*pParm->nGridSpacing;

			pt2.x = pParm->rectROI.right;
			pt2.y = pParm->rectROI.top +
					i*pParm->nGridSpacing;

			DrwLine(pView,&pt1,&pt2,&PenInfo);

			PenInfo.lopnColor = RGB(255,0,0);
			PenInfo.lopnStyle = PS_SOLID;
			PenInfo.lopnWidth.x = 1;
			for (j=1;j<=(int)pRslt->dFndEdgePos[i][0];j++) {
				pt1.x = pParm->rectROI.left +
						(int)pRslt->dFndEdgePos[i][j];
				pt1.y = pParm->rectROI.top +
						i*pParm->nGridSpacing;
				DrwCur(pView,&pt1,&PenInfo,2);
			}
		}
	}		
	else {
		for (i=0;i<pRslt->nLineCount;i++) {
			PenInfo.lopnColor = RGB(0,128,128);
			PenInfo.lopnStyle = PS_SOLID;
			PenInfo.lopnWidth.x = 0;

			pt1.x = pParm->rectROI.left +
					i*pParm->nGridSpacing;
			pt1.y = pParm->rectROI.top;

			pt2.x = pParm->rectROI.left +
					i*pParm->nGridSpacing;
			pt2.y = pParm->rectROI.bottom;

			DrwLine(pView,&pt1,&pt2,&PenInfo);

			PenInfo.lopnColor = RGB(255,0,0);
			PenInfo.lopnStyle = PS_SOLID;
			PenInfo.lopnWidth.x = 1;
			for (j=1;j<=pRslt->dFndEdgePos[i][0];j++) {
				pt1.x = pParm->rectROI.left +
						i*pParm->nGridSpacing;
				pt1.y = pParm->rectROI.top +
						(int)pRslt->dFndEdgePos[i][j];
				DrwCur(pView,&pt1,&PenInfo,2);
			}
		}
	}
	return 0;
}

int CAdvGridEdge::DrawAdvGridEdgeIII(IMAGEVIEW_INFO *pView,
							 ADV_GRD_EDGE_FIND_PARM *pParm,
							 CPoint ptStart,
							 CPoint ptEnd,
							 int nSrchWidth,
							 ADV_GRD_EDGE_FIND_RSLT *pRslt)
{
	int i,j;
	CPoint pt1,pt2;
	LOGPEN PenInfo;
	CPoint ptEstEdge;
	int nDiff;
	float fRatio;
	int nXStart,nXEnd,nYStart,nYEnd;

	nXStart = min(ptStart.x,ptEnd.x);
	nXEnd   = max(ptStart.x,ptEnd.x);
	nYStart = min(ptStart.y,ptEnd.y);
	nYEnd   = max(ptStart.y,ptEnd.y);

	PenInfo.lopnColor = RGB(0,255,0);
	PenInfo.lopnStyle = PS_SOLID;
	PenInfo.lopnWidth.x = 1;
	DrwRect(pView,&pParm->rectROI,&PenInfo);

	if (pParm->nOrientation==ROW) {
		for (i=0;i<pRslt->nLineCount;i++) {
			PenInfo.lopnColor = RGB(0,128,128);
			PenInfo.lopnStyle = PS_SOLID;
			PenInfo.lopnWidth.x = 0;

			fRatio = (float)(i*pParm->nGridSpacing)/(float)(nYEnd-nYStart+1);
			nDiff = (int)(fRatio * (float)(nXEnd-nXStart+1));
			if (ptStart.x < ptEnd.x) {
				ptEstEdge.x = nXStart + nDiff;
			}
			else {
				ptEstEdge.x = nXEnd - nDiff;
			}
			
			pt1.x = ptEstEdge.x - nSrchWidth;
			pt1.y = pParm->rectROI.top +
					i*pParm->nGridSpacing;

			pt2.x = ptEstEdge.x + nSrchWidth;
			pt2.y = pParm->rectROI.top +
					i*pParm->nGridSpacing;

			DrwLine(pView,&pt1,&pt2,&PenInfo);

			PenInfo.lopnColor = RGB(255,0,0);
			PenInfo.lopnStyle = PS_SOLID;
			PenInfo.lopnWidth.x = 1;
			for (j=1;j<=(int)pRslt->dFndEdgePos[i][0];j++) {
				pt1.x = pParm->rectROI.left +
						(int)pRslt->dFndEdgePos[i][j];
				pt1.y = pParm->rectROI.top +
						i*pParm->nGridSpacing;
				DrwCur(pView,&pt1,&PenInfo,2);
			}
		}
	}		
	else {
		for (i=0;i<pRslt->nLineCount;i++) {
			PenInfo.lopnColor = RGB(0,128,128);
			PenInfo.lopnStyle = PS_SOLID;
			PenInfo.lopnWidth.x = 0;

			fRatio = (float)(i*pParm->nGridSpacing)/(float)(nXEnd-nXStart+1);
			nDiff = (int)(fRatio * (float)(nYEnd-nYStart+1));
			if (ptStart.y < ptEnd.y)
				ptEstEdge.y = nYStart + nDiff;
			else
				ptEstEdge.y = nYEnd - nDiff;

			pt1.x = pParm->rectROI.left +
					i*pParm->nGridSpacing;
			pt1.y = ptEstEdge.y - nSrchWidth;

			pt2.x = pParm->rectROI.left +
					i*pParm->nGridSpacing;
			pt2.y = ptEstEdge.y + nSrchWidth;

			DrwLine(pView,&pt1,&pt2,&PenInfo);

			PenInfo.lopnColor = RGB(255,0,0);
			PenInfo.lopnStyle = PS_SOLID;
			PenInfo.lopnWidth.x = 1;
			for (j=1;j<=pRslt->dFndEdgePos[i][0];j++) {
				pt1.x = pParm->rectROI.left +
						i*pParm->nGridSpacing;
				pt1.y = pParm->rectROI.top +
						(int)pRslt->dFndEdgePos[i][j];
				DrwCur(pView,&pt1,&PenInfo,2);
			}
		}
	}
	return 0;
}

CHistogram::CHistogram()
{

}

CHistogram::~CHistogram()
{

}

int CHistogram::GetHistogram(IMAGEVIEW_INFO *pView,
							 CImageBuffer *pImg,
							 HIST_FIND_PARM *pParm,
							 long lDebugFlag)
{
	int nError,i;
	int nMostCommon;
	long lHalf;

	if(CheckRect(&pParm->rectROI,&pView->sizeImage))
		return -1;

	memset(m_HistogramRslt.lHistVal,0,256*sizeof(long));

	//Don't have to do initialisation because as it compute the AccHistVal it will init there
	//memset(m_HistogramRslt.lAcclHistVal,0,256*sizeof(long));

	// Get Histogram
	nError = GetHist(pImg,&pParm->rectROI,&m_HistogramRslt.lHistVal[0]);
	if (nError)
		return -1;
	
	m_HistogramRslt.nLowGray = 255;
	m_HistogramRslt.nHighGray = 0;
	nMostCommon=0;
	for (i=0;i<256;i++) {
		// Compute Accumulation
		if (i==0)
			m_HistogramRslt.lAcclHistVal[i] = m_HistogramRslt.lHistVal[i];
		else
			m_HistogramRslt.lAcclHistVal[i] = m_HistogramRslt.lAcclHistVal[i-1] +
											 m_HistogramRslt.lHistVal[i];

		// Compute Max and Min gray Value
		if(m_HistogramRslt.nLowGray == 255) {
			if (m_HistogramRslt.lHistVal[i] > 0) {
				m_HistogramRslt.nLowGray = i;
			}
		}
		if (m_HistogramRslt.lHistVal[i] > 0) {
			m_HistogramRslt.nHighGray = i;
		}
/*
		if(m_HistogramRslt.nLowGray == 255 && m_HistogramRslt.nHighGray == 0) {
			if (m_HistogramRslt.lHistVal[i] > 0)
				m_HistogramRslt.nLowGray = i;
		}
		else if (m_HistogramRslt.nHighGray == 0) {
			if (m_HistogramRslt.lHistVal[i] == 0)
				m_HistogramRslt.nHighGray = i-1;
		}
*/
		// Compute Most Common Gray Pixel Value
		if(m_HistogramRslt.lHistVal[i] > m_HistogramRslt.lHistVal[nMostCommon])
			nMostCommon = i;
		if (lDebugFlag&DEBUG_HIST && lDebugFlag&DEBUG_PRINT_EXT) {
			OutputDebugLog("HistVal[%d]=%ld  AccHistVal[%d]=%ld",
							i,
							m_HistogramRslt.lHistVal[i],
							i,
							m_HistogramRslt.lAcclHistVal[i]);
		}
	}
	m_HistogramRslt.nMostCommonGray = nMostCommon;

	//  Compute Median
	m_HistogramRslt.nMedian = 255;
	lHalf=m_HistogramRslt.lAcclHistVal[255]/2;
	for (i=0;i<256;i++) {
		if (m_HistogramRslt.lAcclHistVal[i] >= lHalf) {
			m_HistogramRslt.nMedian = i; 
			break;
		}
	}
	if (lDebugFlag&DEBUG_HIST && lDebugFlag&DEBUG_PRINT_EXT) {
		OutputDebugLog("LGray=[%d],HGray=[%d],Median=[%d],MostCommonGray=[%d]",
						m_HistogramRslt.nLowGray,
						m_HistogramRslt.nHighGray,
						m_HistogramRslt.nMedian,
						m_HistogramRslt.nMostCommonGray);
	}

	if (lDebugFlag&DEBUG_HIST && lDebugFlag&DEBUG_DRAW)
		DrawHistogram(pView,pParm,&m_HistogramRslt);

	return 0;
}


int CHistogram::GetThreshold(int nMinThres, int nMaxThres, long *plVal)
{
	int				size = 256;
	int				i, k, st;			// loop counter
	int				max_k=0;			// optimum threshold level
	float			N;					// total number of pixels in active region
	float			p_i[256];           // probability distribution
	float			omega_k;            // probability for class occurance
	float			mu_k, mu_t;         // class mean level
	float			sigma2b;            // between-class variance 
	float			sigma2t;            // total variance of level
	float			max_sigma2b = 0.0;  // optimum between-class variance
	float			temp, temp2;        // temporary variables
	
	/* caculate variances and mean levels */
	size = nMaxThres + 1;
	st = nMinThres;
	mu_t = sigma2t = N = 0.0;
	for (i = st; i < size; i++)
		N += plVal[i];
	for (i = st; i < size; i++)
		p_i[i] = plVal[i] / N;
	for (i = st; i < size; i++)
		mu_t += (i * p_i[i]);
	for (i = st; i < size; i++) {
		temp = i - mu_t;
		sigma2t += temp * temp * p_i[i];
	}
	
	/* compute optimal threshold level */
	for (k = st; k<size; k++) {
		omega_k = mu_k = 0.0;
		for (i = st; i<k; i++) {
			omega_k += p_i[i];
			mu_k += (p_i[i]*i);
		}
		temp = (float) (omega_k * (1.0 - omega_k));
		if ( fabs(temp) < 1.0e-10)
			sigma2b = 0.0;
		else 
		{
			temp2 = mu_t * omega_k - mu_k;
			sigma2b = temp2 * temp2 / temp;
		}
		if (sigma2b > max_sigma2b) 
		{
			max_sigma2b = sigma2b;
			max_k = k;
		}
	}
	
    if (fabs(sigma2t) < 1.0e-10) {
		//OutputDebugLog("Thresholding Level = %d \n Ease of Thresholding=0.0\n",max_k);
		AfxMessageBox("Thresholding Level may be unusable\n");
    } else {
		//OutputDebugLog("Thresholding Level = %d \nEase of Thresholding = %f\n", max_k, max_sigma2b/sigma2t);
	}
	
    return max_k;
}

int CHistogram::GetPercentageThreshold(double dBlackPercentage, 
									   double dWhitePercentage, 
									   long *plVal, 
									   int &nThreshold)
{
	long lSum;
	long lCount;
	int nBlackMean;
	int nWhiteMean;
	long lBlackCount;
	long lWhiteCount;
	long lTotalCount;
	int i;
	lTotalCount = 0;
	for(i = 0; i < 256; i++) 
		lTotalCount += plVal[i];
	lBlackCount = Round(lTotalCount * dBlackPercentage / 100);
	lWhiteCount = Round(lTotalCount * dWhitePercentage / 100);
	lSum = 0;
	lCount = 0;
	for(i = 0; i < 256; i++) {
		lSum += plVal[i] * i;
		lCount += plVal[i];
		if(lCount > lBlackCount) 
			break;
	}
	if(lCount > 0)
		nBlackMean = lSum/lCount;
	else 
		nBlackMean = 0;
	lSum = 0;
	lCount = 0;
	for(i = 255; i != 0; i--) {
		lSum += plVal[i] * i;
		lCount += plVal[i];
		if(lCount > lWhiteCount) 
			break;
	}
	if(lCount > 0)
		nWhiteMean = lSum/lCount;
	else 
		nWhiteMean = 0;
	nThreshold = (nBlackMean + nWhiteMean) / 2;
	return 0;
}


int CHistogram::GetWhitePercentageThreshold(double dWhitePercentage, 
											long *plVal, 
											int &nThreshold)
{
	long lSum;
	long lCount;
	long lWhiteCount;
	long lTotalCount;
	int i;
	lTotalCount = 0;
	for(i = 0; i < 256; i++) 
		lTotalCount += plVal[i];
	lWhiteCount = Round(lTotalCount * dWhitePercentage / 100);
	lSum = 0;
	lCount = 0;
	for(i = 255; i != 0; i--) {
		lSum += plVal[i] * i;
		lCount += plVal[i];
		if(lCount > lWhiteCount) 
			break;
	}
	if(lCount > 0)
		nThreshold = lSum/lCount - 1;
	else 
		nThreshold = i - 1;
	if(nThreshold < 0)
		nThreshold = 0;
	return 0;
}

int CHistogram::GetAvgIntensity(double dPercentageStart,
								double dPercentageEnd,
								long *plVal, 
								int* pnAvgIntensity)
{
	int nAvgIntensity;
	double dAvgIntensity;
	int nError;

	nError = GetAvgIntensity(dPercentageStart,
					dPercentageEnd,
					plVal, 
					&dAvgIntensity);
	if(nError)
		goto ErrorAbort;
	nAvgIntensity = Round(dAvgIntensity);
	if(nAvgIntensity < 0)
		nAvgIntensity = 0;
	if(nAvgIntensity > 255)
		nAvgIntensity = 255;
	*pnAvgIntensity = nAvgIntensity;
	return 0;
ErrorAbort:
	return -1;
}

int CHistogram::GetAvgIntensity(double dPercentageStart,
								double dPercentageEnd,
								long *plVal, 
								double* pdAvgIntensity)
{
	long lSum;
	long lCount;
	long lCount1;
	long lCount2;
	long lTotalCount;
	int nCutoff1, nCutoff2;
	double dThreshold;
	int nCountForAvg[256];
	int nCountBefore;
	int i;

	*pdAvgIntensity = 0;
	if((dPercentageStart<(-INT_AND_DBL_EQ_TOL)) || (dPercentageStart>(100+INT_AND_DBL_EQ_TOL)) ||
		(dPercentageEnd<(-INT_AND_DBL_EQ_TOL)) || (dPercentageEnd>(100+INT_AND_DBL_EQ_TOL)) ||
		(dPercentageStart>dPercentageEnd))
		goto ErrorAbort;

	if(dPercentageStart < 0)
		dPercentageStart = 0;
	if(dPercentageStart > 100)
		dPercentageStart = 100;
	if(dPercentageEnd < 0)
		dPercentageEnd = 0;
	if(dPercentageEnd > 100)
		dPercentageEnd = 100;

	lTotalCount = 0;
	for(i = 0; i < 256; i++) { 
		lTotalCount += plVal[i];
		nCountForAvg[i] = 0;
	}
	lCount1 = Round(lTotalCount * dPercentageStart / 100);
	lCount2 = Round(lTotalCount * dPercentageEnd / 100);

	lCount = 0;
	for(i = 0; i < 256; i++) {
		lCount += plVal[i];
		if(lCount > lCount1) {
			if(lCount > lCount2) {
				*pdAvgIntensity = i;
				goto PASS;
			}
			nCountForAvg[i] = lCount - lCount1;
			break;
		}
	}
	nCutoff1 = i;
	if(nCutoff1 > 255)
		nCutoff1 = 255;
	for(i = nCutoff1 + 1; i < 256; i++) {
		nCountBefore = lCount;
		lCount += plVal[i];
		if(lCount > lCount2) {
			nCountForAvg[i] = lCount2 - nCountBefore;
			break;
		}
		nCountForAvg[i] = plVal[i];
	}
	nCutoff2 = i;
	if(nCutoff2 > 255)
		nCutoff2 = 255;
	
	dThreshold = (nCutoff1 + nCutoff2)/2.0;
	if(nCutoff2 > nCutoff1) {
		lSum = 0;
		lCount = 0;
		for(i=nCutoff1;i<=nCutoff2;i++) {
			lSum += i*nCountForAvg[i];
			lCount += nCountForAvg[i];
		}
		if(lCount > 0)
			dThreshold = double(lSum)/lCount;
	}
	*pdAvgIntensity = dThreshold;
PASS:
	return 0;
ErrorAbort:
	return -1;
}

int CHistogram::GetWhitePercentageCutoffThreshold(double dWhitePercentage, 
											long *plVal, 
											int &nThreshold)
{
	long lCount;
	long lWhiteCount;
	long lTotalCount;
	int i;
	lTotalCount = 0;
	for(i = 0; i < 256; i++) 
		lTotalCount += plVal[i];
	lWhiteCount = Round(lTotalCount * dWhitePercentage / 100);
	lCount = 0;
	for(i = 255; i != 0; i--) {
		lCount += plVal[i];
		if(lCount > lWhiteCount) 
			break;
	}
	nThreshold = i;
	return 0;
}

int CHistogram::GetBlackPercentageCutoffThreshold(double dBlackPercentage, 
											long *plVal, 
											int &nThreshold)
{
	long lSum;
	long lCount;
	long lBlackCount;
	long lTotalCount;
	int i;
	lTotalCount = 0;
	for(i = 0; i < 256; i++) 
		lTotalCount += plVal[i];
	lBlackCount = Round(lTotalCount * dBlackPercentage / 100);

	lSum = 0;
	lCount = 0;
	for(i = 0; i < 256; i++) {
		lSum += plVal[i] * i;
		lCount += plVal[i];
		if(lCount > lBlackCount) 
			break;
	}
	nThreshold = i;
	return 0;
}

int CHistogram::GetBlackPercentageThreshold(double dBlackPercentage, 
											long *plVal, 
											int &nThreshold)
{
	long lSum;
	long lCount;
	long lBlackCount;
	long lTotalCount;
	int i;
	lTotalCount = 0;
	for(i = 0; i < 256; i++) 
		lTotalCount += plVal[i];
	lBlackCount = Round(lTotalCount * dBlackPercentage / 100);

	lSum = 0;
	lCount = 0;
	for(i = 0; i < 256; i++) {
		lSum += plVal[i] * i;
		lCount += plVal[i];
		if(lCount > lBlackCount) 
			break;
	}
	if(lCount > 0)
		nThreshold = lSum/lCount + 1;
	else 
		nThreshold = i + 1;
	if(nThreshold >= 255)
		nThreshold = 255;
	return 0;
}

int CHistogram::DrawHistogram(IMAGEVIEW_INFO *pView,
							  HIST_FIND_PARM *pParm,
							  HIST_FIND_RSLT *pRslt)
{
	int i;
	CPoint pt1,pt2;
	CRect rectHist;
	LOGPEN PenInfo;
	long lMaxAccl;

	rectHist = CRect(CPoint(pView->sizeImage.cx/2-128,pView->sizeImage.cy/2-128),
					 CSize(256,256));

	lMaxAccl = pRslt->lHistVal[pRslt->nMostCommonGray];

	PenInfo.lopnColor = RGB(0,255,255);
	PenInfo.lopnStyle = PS_SOLID;
	PenInfo.lopnWidth.x = 1;
	DrwRect(pView,&pParm->rectROI,&PenInfo);

	PenInfo.lopnColor = RGB(0,255,0);
	PenInfo.lopnStyle = PS_SOLID;
	PenInfo.lopnWidth.x = 1;
	DrwRect(pView,&rectHist,&PenInfo);

	for (i=0;i<256;i++) {
		if (i==pRslt->nMedian) {
			PenInfo.lopnColor = RGB(255,0,0);
			PenInfo.lopnStyle = PS_SOLID;
			PenInfo.lopnWidth.x = 1;
		}
		else if (i==pRslt->nMostCommonGray) {
			PenInfo.lopnColor = RGB(0,0,255);
			PenInfo.lopnStyle = PS_SOLID;
			PenInfo.lopnWidth.x = 1;
		}

		pt1.x = pt2.x = rectHist.left + (int)((double)i/256.0*(double)rectHist.Width());
		pt1.y = rectHist.bottom - (int)((double)pRslt->lHistVal[i]/(double)lMaxAccl*(double)rectHist.Height());
		pt2.y = rectHist.bottom;
		DrwLine(pView,&pt1,&pt2,&PenInfo);

		if (i==pRslt->nMedian || i==pRslt->nMostCommonGray) {
			PenInfo.lopnColor = RGB(0,255,0);
			PenInfo.lopnStyle = PS_SOLID;
			PenInfo.lopnWidth.x = 1;
		}
	}
	return 0;
}

CMask::CMask()
{
	lContourNo = 0;
	plContourAddress = NULL;
}

CMask::~CMask()
{
	if(plContourAddress != NULL)
		delete [] plContourAddress;
	plContourAddress = NULL;
}


int CMask::InitRect2DMask(CImageBuffer *pImgMask,
						  CRect* prcMask,
						  CRectangle2D *prc2D,
						  BYTE byteLabel)
{
	CPoint2D pt2DMask[3];
	CRect rcPVI;
	CPoint2D pt2DCenter;
	int npt2DMskNo;
	CPoint ptShift;
	CRectangle2D rc2DInsp;
	CRect rcBounding;
	int nError;
	CPoint2D pt2DTL,
		     pt2DTR,
		     pt2DBL,
		     pt2DBR,
		     pt2DOld,
			 pt2DRef;

	CPoint2D pt2DLMost,
		     pt2DRMost,
		     pt2DTMost,
		     pt2DBMost;

	pImgMask->ClrImg(byteLabel);
	rc2DInsp = CRectangle2D(*prc2D);
	rcBounding = rc2DInsp.GetBoundingRect();
	pt2DTL = rc2DInsp.GetDevTopLeft();
	pt2DTR = rc2DInsp.GetDevTopRight();
	pt2DBL = rc2DInsp.GetDevBottomLeft();
	pt2DBR = rc2DInsp.GetDevBottomRight();
	pt2DLMost = rc2DInsp.GetDevLftMost();
	if(pt2DLMost.x > rcBounding.left)
		pt2DLMost.x = rcBounding.left;
	pt2DTMost = rc2DInsp.GetDevTopMost();
	if(pt2DTMost.y > rcBounding.top)
		pt2DTMost.y = rcBounding.top;
	pt2DRMost = rc2DInsp.GetDevRhtMost();
	if(pt2DRMost.x < (rcBounding.right - 1))
		pt2DRMost.x = rcBounding.right - 1;
	pt2DBMost = rc2DInsp.GetDevBotMost();
	if(pt2DBMost.y < (rcBounding.bottom - 1))
		pt2DBMost.y = rcBounding.bottom - 1;
	// Image Preprocessing START - Masking, BandThreshold, PackageMean
	if(prc2D->GetAngle() != 0) {
		npt2DMskNo = 3;
		pt2DMask[0] = CPoint2D(0,0);
		pt2DMask[1] = pt2DTMost;
		pt2DMask[2] = pt2DLMost;
		nError = ApplyGrayMaskFreeHand(pImgMask,prcMask,&pt2DMask[0], npt2DMskNo, 0);
		if(nError)
			goto ErrorAbort;
		pt2DMask[0] = CPoint2D(rcBounding.right - 1,0);
		pt2DMask[1] = pt2DRMost;
		pt2DMask[2] = pt2DTMost;
		nError = ApplyGrayMaskFreeHand(pImgMask,prcMask,&pt2DMask[0], npt2DMskNo, 0);
		if(nError)
			goto ErrorAbort;
		pt2DMask[0] = CPoint2D(0,rcBounding.bottom - 1);
		pt2DMask[1] = pt2DBMost;
		pt2DMask[2] = pt2DLMost;
		nError = ApplyGrayMaskFreeHand(pImgMask,prcMask,&pt2DMask[0], npt2DMskNo, 0);
		if(nError)
			goto ErrorAbort;
		pt2DMask[0] = CPoint2D(rcBounding.right - 1,rcBounding.bottom - 1);
		pt2DMask[1] = pt2DRMost;
		pt2DMask[2] = pt2DBMost;
		nError = ApplyGrayMaskFreeHand(pImgMask,prcMask,&pt2DMask[0], npt2DMskNo, 0);
		if(nError)
			goto ErrorAbort;
		
	}
	return 0;
ErrorAbort:
	return -1;
}


int CMask::AddBandBinariseMask(CImageBuffer *pImgSrc,
						   CRect *prcImgSrc,
						   CImageBuffer *pMaskSrc,
						   CRect *prcMaskSrc,
						   CImageBuffer *pMaskDst,
						   CRect *prcMaskDst,
						   int nThresMin,
						   int nThresMax,
						   int nInvert)
{
	int x,y;
	int nImgPitchSrc, nMaskPitchSrc, nMaskPitchDst;
	int nWidthSrc,nHeightSrc;
	BYTE *pRowImgSrc, *pColImgSrc, *pRowMaskSrc, *pColMaskSrc, *pRowMaskDst, *pColMaskDst;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcImgSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcImgSrc is Invalid");
		return -1;
	}
	if (!pMaskSrc) {
		ASSERT(!"pMaskSrc is a bad Pointer");
		return -1;
	}
	if (!pMaskSrc->IsAllocated()) {
		ASSERT(!"pMaskDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMaskSrc,&(pMaskSrc->GetImgSize()))) {
		ASSERT(!"prcMaskSrc is Invalid");
		return -1;
	}
	if (!pMaskDst) {
		ASSERT(!"pMaskDst is a bad Pointer");
		return -1;
	}
	if (!pMaskDst->IsAllocated()) {
		ASSERT(!"pMaskDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMaskDst,&(pMaskDst->GetImgSize()))) {
		ASSERT(!"prcMaskDst is Invalid");
		return -1;
	}
	if (prcImgSrc->Size()!=prcMaskSrc->Size()) {
		ASSERT(!"prcImgSrc Size different from prcMaskSrc Size");
		return -1;
	}
	if (prcImgSrc->Size()!=prcMaskDst->Size()) {
		ASSERT(!"prcImgSrc Size different from prcMaskDst Size");
		return -1;
	}
	if (nThresMin<0 && nThresMin>=256) {
		ASSERT(!"nThresMin Out Of Range");
		return -1;
	}
	if (nThresMax<0 && nThresMax>=256) {
		ASSERT(!"nThresMax Out Of Range");
		return -1;
	}
	if (nThresMin>nThresMax) {
		ASSERT(!"nThresMin > nThresMax");
		return -1;
	}
	nImgPitchSrc = pImgSrc->GetPitch();
	nMaskPitchSrc = pMaskSrc->GetPitch();
	nMaskPitchDst = pMaskDst->GetPitch();
	nWidthSrc = prcImgSrc->Width();
	nHeightSrc = prcImgSrc->Height();
	pRowImgSrc = pColImgSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcImgSrc->top + prcImgSrc->left;
	pRowMaskSrc = pColMaskSrc = pMaskSrc->GetImgPtr() + nMaskPitchSrc * prcMaskSrc->top + prcMaskSrc->left;
	pRowMaskDst = pColMaskDst = pMaskDst->GetImgPtr() + nMaskPitchDst * prcMaskDst->top + prcMaskDst->left;


	if (nInvert==0) {
		for (y=0; y<nHeightSrc; y++,pRowImgSrc+=nImgPitchSrc,pRowMaskSrc+=nMaskPitchSrc,pRowMaskDst+=nMaskPitchDst) {
			for (x=0,pColImgSrc=pRowImgSrc,pColMaskSrc=pRowMaskSrc,pColMaskDst=pRowMaskDst; x<nWidthSrc; x++,pColImgSrc++,pColMaskSrc++,pColMaskDst++) {
				if ((*pColMaskSrc>0)&&(*pColImgSrc>=nThresMin) && (*pColImgSrc<=nThresMax))
					*pColMaskDst = 255;
				else
					*pColMaskDst = 0;
			}
		}
	}
	else {
		for (y=0; y<nHeightSrc; y++,pRowImgSrc+=nImgPitchSrc,pRowMaskSrc+=nMaskPitchSrc,pRowMaskDst+=nMaskPitchDst) {
			for (x=0,pColImgSrc=pRowImgSrc,pColMaskSrc=pRowMaskSrc,pColMaskDst=pRowMaskDst; x<nWidthSrc; x++,pColImgSrc++,pColMaskSrc++,pColMaskDst++) {
				if ((*pColMaskSrc==0)&&(*pColImgSrc>=nThresMin) && (*pColImgSrc<=nThresMax))
					*pColMaskDst = 0;
				else
					*pColMaskDst = 255;
			}
		}
	}
	return 0;
}

int CMask::EdgeMask(CImageBuffer* pImgMsk,
			   CRect *prcImgMask,
			   CPoint2D *ppt2DConvexMsk, 			// Convex Points in Order
			   int npt2DConvexMskNo,				// Number of Convex Points
			   BYTE byteLabel)				// The Label Value to the Linked Line  
{
	long lIndex = 0;
	int i;
	double dStepX, dStepY;
	int nLineLength, nMaskWidth;
	double x, y;
	BYTE *pImgPtr;
	int nPitch;
	int nFillRectTopY, nFillRectLeftX;
	CPoint ptStart, ptEnd;
	CLine2D line;
	CPoint* pptMsk;
	int npt2DMskNo;
	long lAddressTemp;
	int nWidth;
	int nHeight;

	nWidth = prcImgMask->Width();
	nHeight = prcImgMask->Height();
	npt2DMskNo = npt2DConvexMskNo + 1;
	createArray(pptMsk, npt2DMskNo);
	//pptMsk = new CPoint[abs(npt2DMskNo)];
	for(i=0; i<npt2DConvexMskNo; i++) {
		pptMsk[i].x = Round(ppt2DConvexMsk[i].x);
		pptMsk[i].y = Round(ppt2DConvexMsk[i].y);
	}
	pptMsk[npt2DMskNo - 1] = pptMsk[0];

	ptStart = pptMsk[0];
	rcMskBounding = CRect(ptStart.x, ptStart.y, ptStart.x, ptStart.y);
	for(i = 1; i < npt2DMskNo; i++)	{
		if(rcMskBounding.left > pptMsk[i].x) rcMskBounding.left = pptMsk[i].x;
		if(rcMskBounding.top > pptMsk[i].y) rcMskBounding.top = pptMsk[i].y;
		if(rcMskBounding.right < pptMsk[i].x) rcMskBounding.right = pptMsk[i].x;
		if(rcMskBounding.bottom < pptMsk[i].y) rcMskBounding.bottom = pptMsk[i].y;
	}
	rcMskBounding.InflateRect(3,3,3,3);
	if(RectOverlapRect(*prcImgMask, rcMskBounding) == 0)
		goto PASS;
	rcMskBoundingRelative = CRect(CPoint(0,0), rcMskBounding.Size());
	if((rcMskBoundingRelative.Width() < 0) || 
	  (rcMskBoundingRelative.Width() > 6000) ||
	  (rcMskBoundingRelative.Height() < 0) ||
	  (rcMskBoundingRelative.Height() > 6000))
		goto ErrorAbort;


	nFillRectTopY = 0;
	nFillRectLeftX = 0;
	pImgPtr = pImgMsk->GetImgPtr();
	nPitch = pImgMsk->GetPitch();

	nMaskWidth = rcMskBoundingRelative.Width();

	lContourNo = 0;
	for(i = 1; i < npt2DMskNo; i++) {
		ptStart = CPoint( pptMsk[i-1].x - nFillRectLeftX, pptMsk[i - 1].y - nFillRectTopY);
		ptEnd = CPoint( pptMsk[i].x - nFillRectLeftX, pptMsk[i].y - nFillRectTopY);
		nLineLength = (int) sqrt((double) ((ptStart.x - ptEnd.x) * (ptStart.x - ptEnd.x) + (ptStart.y - ptEnd.y) * (ptStart.y - ptEnd.y)));
		if(nLineLength)	{
			dStepX = double(ptEnd.x - ptStart.x) / nLineLength;
			dStepY = double(ptEnd.y - ptStart.y) / nLineLength;
			y = ptStart.y;
			x = ptStart.x;
			if(fabs(dStepY) > fabs(dStepX))	{
				do	{
					lIndex = int (y) * nPitch;
					lAddressTemp = lIndex + int(x);
					if((x >= 0) && (x >= nWidth) &&
						(y < 0) && (y >= nHeight))
					{
						if(pImgPtr[lAddressTemp] != byteLabel) 
							pImgPtr[lAddressTemp] = byteLabel;
					}
					y += dStepY;
					x += dStepX;
				}while((int)y != ptEnd.y); 
				lIndex=int(y) * nPitch;
				lAddressTemp = lIndex + int(x);
				if((x >= 0) && (x >= nWidth) &&
					(y < 0) && (y >= nHeight))
				{
					if(pImgPtr[lAddressTemp] != byteLabel) 
						pImgPtr[lAddressTemp] = byteLabel;
				}
			}
			else {
				do	{
					lIndex = int(y) * nPitch;
					lAddressTemp = lIndex + int(x);
					if((x >= 0) && (x >= nWidth) &&
						(y < 0) && (y >= nHeight))
					{
						if(pImgPtr[lAddressTemp] != byteLabel) 
							pImgPtr[lAddressTemp] = byteLabel;
					}
					y += dStepY;
					x += dStepX;
				}while((int)x != ptEnd.x);
				lIndex = int(y) * nPitch;
				lAddressTemp = lIndex + int(x);
				if((x >= 0) && (x >= nWidth) &&
					(y < 0) && (y >= nHeight))
				{
					if(pImgPtr[lAddressTemp] != byteLabel) 
						pImgPtr[lAddressTemp] = byteLabel;
				}
			}
		}
	}

	deleteArray(pptMsk);
	//delete [] pptMsk;
	return 0;
ErrorAbort:
	deleteArray(pptMsk);
	//delete [] pptMsk;
	return -1;
PASS:
	deleteArray(pptMsk);
	//delete [] pptMsk;
	return 1;

}

int CMask::LinkPt(CRect *prcImgMask,
				  CPoint2D *ppt2DConvexMsk, 			// Convex Points in Order
				  int npt2DConvexMskNo, 				// Number of Convex Points
				  BYTE byteLabel)				// The Label Value to the Linked Line  
{
	long lIndex = 0;
	int i;
	double dStepX, dStepY;
	int nLineLength, nMaskWidth;
	double x, y;
	int nError;
	BYTE *pImgPtr;
	int nPitch;
	int nFillRectTopY, nFillRectLeftX;
	CPoint ptStart, ptEnd;
	CLine2D line;
	CPoint* pptMsk;
	int npt2DMskNo;
	long lAddressTemp;

	npt2DMskNo = npt2DConvexMskNo + 1;
	createArray(pptMsk, npt2DMskNo);
	//pptMsk = new CPoint[abs(npt2DMskNo)];
	for(i=0; i<npt2DConvexMskNo; i++) {
		pptMsk[i].x = Round(ppt2DConvexMsk[i].x);
		pptMsk[i].y = Round(ppt2DConvexMsk[i].y);
	}
	pptMsk[npt2DMskNo - 1] = pptMsk[0];

	ptStart = pptMsk[0];
	rcMskBounding = CRect(ptStart.x, ptStart.y, ptStart.x, ptStart.y);
	for(i = 1; i < npt2DMskNo; i++)	{
		if(rcMskBounding.left > pptMsk[i].x) rcMskBounding.left = pptMsk[i].x;
		if(rcMskBounding.top > pptMsk[i].y) rcMskBounding.top = pptMsk[i].y;
		if(rcMskBounding.right < pptMsk[i].x) rcMskBounding.right = pptMsk[i].x;
		if(rcMskBounding.bottom < pptMsk[i].y) rcMskBounding.bottom = pptMsk[i].y;
	}
	rcMskBounding.InflateRect(3,3,3,3);
	if(RectOverlapRect(*prcImgMask, rcMskBounding) == 0)
		goto PASS;
	rcMskBoundingRelative = CRect(CPoint(0,0), rcMskBounding.Size());
	if((rcMskBoundingRelative.Width() < 0) || 
	  (rcMskBoundingRelative.Width() > 6000) ||
	  (rcMskBoundingRelative.Height() < 0) ||
	  (rcMskBoundingRelative.Height() > 6000))
		goto ErrorAbort;

	if(plContourAddress != NULL)
		delete [] plContourAddress;
	plContourAddress = new long[abs(rcMskBoundingRelative.Width() * rcMskBoundingRelative.Height())];

	nFillRectTopY = rcMskBounding.top;
	nFillRectLeftX = rcMskBounding.left;
	ImgMskTemp.Allocate(AbsSize(CSize(rcMskBoundingRelative.Width(), rcMskBoundingRelative.Height())));
	ImgMskTemp.ClrImg(255);
	pImgPtr = ImgMskTemp.GetImgPtr();
	nPitch = ImgMskTemp.GetPitch();

	nMaskWidth = rcMskBoundingRelative.Width();

	lContourNo = 0;
	for(i = 1; i < npt2DMskNo; i++) {
		ptStart = CPoint( pptMsk[i-1].x - nFillRectLeftX, pptMsk[i - 1].y - nFillRectTopY);
		ptEnd = CPoint( pptMsk[i].x - nFillRectLeftX, pptMsk[i].y - nFillRectTopY);
		nLineLength = (int) sqrt((double) ((ptStart.x - ptEnd.x) * (ptStart.x - ptEnd.x) + (ptStart.y - ptEnd.y) * (ptStart.y - ptEnd.y)));
		if(nLineLength)	{
			dStepX = double(ptEnd.x - ptStart.x) / nLineLength;
			dStepY = double(ptEnd.y - ptStart.y) / nLineLength;
			y = ptStart.y;
			x = ptStart.x;
			if(fabs(dStepY) > fabs(dStepX))	{
				do	{
					lIndex = int (y) * nPitch;
					lAddressTemp = lIndex + int(x);
					if(pImgPtr[lAddressTemp] != 200) {
						plContourAddress[lContourNo++] = lAddressTemp;
						pImgPtr[lAddressTemp] = 200;
					}
					y += dStepY;
					x += dStepX;
				}while((int)y != ptEnd.y); 
				lIndex=int(y) * nPitch;
				lAddressTemp = lIndex + int(x);
				if(pImgPtr[lAddressTemp] != 200) {
					plContourAddress[lContourNo++] = lAddressTemp;
					pImgPtr[lAddressTemp] = 200;
				}
			}
			else {
				do	{
					lIndex = int(y) * nPitch;
					lAddressTemp = lIndex + int(x);
					if(pImgPtr[lAddressTemp] != 200) {
						plContourAddress[lContourNo++] = lAddressTemp;
						pImgPtr[lAddressTemp] = 200;
					}
					y += dStepY;
					x += dStepX;
				}while((int)x != ptEnd.x);
				lIndex = int(y) * nPitch;
					lAddressTemp = lIndex + int(x);
					if(pImgPtr[lAddressTemp] != 200) {
						plContourAddress[lContourNo++] = lAddressTemp;
						pImgPtr[lAddressTemp] = 200;
					}
			}
		}
	}

	nError = ClearImageROIEdge(&ImgMskTemp,
							   &rcMskBoundingRelative,
							   255);
	if(nError)
		goto ErrorAbort;
	deleteArray(pptMsk);
	//delete [] pptMsk;
	return 0;
ErrorAbort:
	deleteArray(pptMsk);
	//delete [] pptMsk;
	return -1;
PASS:
	deleteArray(pptMsk);
	//delete [] pptMsk;
	return 1;
}

int CMask::FillMask(CImageBuffer *pImg, 			// Pointer to Image to Be Filled
					BYTE byteLabel)			// The Label Value of the Contour
{

	int i, j, k;
	int nWidth, nHeight, nPitch;
	int nDir[4];
	long *plSeedsOld, *plSeedsNew;
	int lSeedsOldNo, lSeedsNewNo;
	BYTE *pImgPtr;
	CPoint ptDir[4];
	long lIndex, lIndexTemp, lTotalNo;

	nWidth = pImg->GetImgSize().cx;
	nHeight = pImg->GetImgSize().cy;
	nPitch = pImg->GetPitch();
	pImgPtr = pImg->GetImgPtr();
	lTotalNo = nHeight * nPitch;
	plSeedsOld = new long[abs(lTotalNo)];
	plSeedsNew = new long[abs(lTotalNo)];

	nDir[0] = 1;
	nDir[1] = -nPitch;
	nDir[2] = -1;
	nDir[3] = nPitch;
	ptDir[0] = CPoint(1, 0);
	ptDir[1] = CPoint(0, -1);
	ptDir[2] = CPoint(-1, 0);
	ptDir[3] = CPoint(0, 1);
	lIndex = 0;
	for(i = 0; i < nHeight; i++) {
		for(j = 0; j < nWidth; j++) {
			if(pImgPtr[lIndex + j] == 200) break;
			pImgPtr[lIndex+j] = 0;
		}
		for(j = nWidth - 1; j >= 0; j--) {
			if(pImgPtr[lIndex + j] == 200) break;
			pImgPtr[lIndex + j] = 0;
		}
		lIndex += nPitch;
	}
	lSeedsOldNo = 0;
	lSeedsNewNo = 0;
	for(i = 0; i < lContourNo; i++) {
		lIndexTemp = plContourAddress[i] - 1;
		if(pImgPtr[lIndexTemp] == 255) {
			for(k = 0;k < 4; k++) {
				if(pImgPtr[lIndexTemp + nDir[k]] == 0) {
					plSeedsOld[lSeedsOldNo++] = lIndexTemp;
					break;
				}
			}
		}
		lIndexTemp = plContourAddress[i] + 1;
		if(pImgPtr[lIndexTemp] == 255) {
			for(k = 0; k < 4; k++) {
				if(pImgPtr[lIndexTemp + nDir[k]] == 0) {
					plSeedsOld[lSeedsOldNo++] = lIndexTemp;
					break;
				}
			}
		}
	}
	for(i = 0; i < lSeedsOldNo; i++) {
		pImgPtr[plSeedsOld[i]] = 0;
	}
	do {
		lSeedsNewNo = 0;
		for(i = 0; i < lSeedsOldNo; i++) {
			lIndex = plSeedsOld[i];
			for(k = 0; k < 4; k++)	{
				if(pImgPtr[lIndex + nDir[k]] == 255)	{
					plSeedsNew[lSeedsNewNo++] = lIndex + nDir[k];
					pImgPtr[lIndex + nDir[k]] = 0;
				}
			}
		}
		lSeedsOldNo = lSeedsNewNo;
		for(i = 0; i < lSeedsOldNo; i++) plSeedsOld[i] = plSeedsNew[i];
	}while(lSeedsOldNo > 0);
	delete [] plSeedsOld;
	delete [] plSeedsNew;
	return 0;
}

int CMask::MergeMask(CImageBuffer *pMask,		// Pointer to Mask Image
					 CRect *prcMask,		// ROI Rectangle
					 CImageBuffer *pMaskTemp, 	// Pointer to the Image of the Filled Conex Region
					 BYTE byteLabel)		// The Label Value to Fill in the Convex region
{
	int i, j;
	long lIndex, lIndexTemp;
	BYTE *pImgMaskPtr, *pImgMaskTempPtr;
	int nImgMaskPitch, nImgMaskTempPitch;
	nImgMaskPitch = pMask->GetPitch();
	nImgMaskTempPitch = pMaskTemp->GetPitch();
	pImgMaskPtr = pMask->GetImgPtr();
	pImgMaskTempPtr = pMaskTemp->GetImgPtr();
	rcOverlap = rcMskBounding;
	CRect rcOverlapRelative;
	if(rcOverlap.left < prcMask->left) rcOverlap.left = prcMask->left;
	if(rcOverlap.top < prcMask->top) rcOverlap.top = prcMask->top;
	if(rcOverlap.right > prcMask->right) rcOverlap.right = prcMask->right;
	if(rcOverlap.bottom > prcMask->bottom) rcOverlap.bottom = prcMask->bottom;
	lIndex = rcOverlap.top * nImgMaskPitch;
	lIndexTemp = (rcOverlap.top - rcMskBounding.top) * nImgMaskTempPitch;

	rcOverlapRelative = CRect(rcOverlap.left - rcMskBounding.left,
							  rcOverlap.top - rcMskBounding.top,
							  rcOverlap.right - rcMskBounding.left,
							  rcOverlap.bottom - rcMskBounding.top);

	if(!ValidateRect(&rcOverlapRelative, &rcMskBoundingRelative.Size())) {
		for(i = rcOverlap.top; i <rcOverlap.bottom; i++) {
			for(j = rcOverlap.left; j < rcOverlap.right; j++) {
				if(pImgMaskTempPtr[lIndexTemp + j - rcMskBounding.left] != 0) 
					pImgMaskPtr[lIndex + j] = byteLabel;
			}
			lIndex += nImgMaskPitch;
			lIndexTemp += nImgMaskTempPitch;
		}
	}
	return 0;
}

int CMask::CheckFastFillStatus(CPoint2D *ppt2DMsk,
								int npt2DMskNo,
								BOOL* pbFillX,
								BOOL* pbFillY,
								CRect* prcBounding)
{
	int i, m, n;
	CPoint ptTopMost;
	int nIndexTopMost;
	CPoint ptBotMost;
	int nIndexBotMost;
	CPoint ptLftMost;
	int nIndexLftMost;
	CPoint ptRhtMost;
	int nIndexRhtMost;
	int* pnX;
	int* pnY;
	int nMinIndex;
	int nMaxIndex;
	int nSign;

	int nptMskNo;
	
	*pbFillX = FALSE;
	*pbFillY = FALSE;
	pnX = NULL;
	pnY = NULL;
	nptMskNo = npt2DMskNo;
	if(nptMskNo <= 0)
		return -1;

	*pbFillX = TRUE;
	*pbFillY = TRUE;
	pnX = new int[abs(nptMskNo)];
	pnY = new int[abs(nptMskNo)];
	pnX[0] = Round(ppt2DMsk[0].x);
	pnY[0] = Round(ppt2DMsk[0].y);
	ptTopMost = CPoint(pnX[0], pnY[0]);
	nIndexTopMost = 0;
	ptBotMost = ptTopMost;
	nIndexBotMost = 0;
	ptLftMost = ptTopMost;
	nIndexLftMost = 0;
	ptRhtMost = ptTopMost;
	nIndexRhtMost = 0;
	for(i=1;i<nptMskNo;i++) {
		pnX[i] = Round(ppt2DMsk[i].x);
		pnY[i] = Round(ppt2DMsk[i].y);
		if(ptTopMost.y > pnY[i]) {
			ptTopMost = CPoint(pnX[i], pnY[i]);
			nIndexTopMost = i;
		}
		if(ptBotMost.y < pnY[i]) {
			ptBotMost = CPoint(pnX[i], pnY[i]);
			nIndexBotMost = i;
		}
		if(ptLftMost.x > pnX[i]) {
			ptLftMost = CPoint(pnX[i], pnY[i]);
			nIndexLftMost = i;
		}
		if(ptRhtMost.x < pnX[i]) {
			ptRhtMost = CPoint(pnX[i], pnY[i]);
			nIndexRhtMost = i;
		}
	}
	*prcBounding = CRect(ptLftMost.x, ptTopMost.y, ptRhtMost.x + 1, ptBotMost.y + 1);
	if(nIndexTopMost < nIndexBotMost) {
		nMinIndex = nIndexTopMost;
		nMaxIndex = nIndexBotMost;
	}
	else {
		nMinIndex = nIndexBotMost;
		nMaxIndex = nIndexTopMost;
	}
	if(pnY[nMinIndex] < pnY[nMaxIndex]) 
		nSign = 1;
	else
		nSign = -1;
	for(i=nMinIndex;i<nMaxIndex;i++) {
		if(((pnY[i+1] - pnY[i])*nSign) < 0) {
			*pbFillX = FALSE;
			break;
		}
	}
	if(*pbFillX) {
		for(i=nMaxIndex;i<(nMinIndex+nptMskNo);i++) {
			m = i;
			if(m>=nptMskNo)
				m -= nptMskNo;
			n = i + 1;
			if(n>=nptMskNo)
				n -= nptMskNo;
			if(((pnY[m] - pnY[n]) * nSign) < 0) {
				*pbFillX = FALSE;
				break;
			}
		}
	}

	if(nIndexLftMost < nIndexRhtMost) {
		nMinIndex = nIndexLftMost;
		nMaxIndex = nIndexRhtMost;
	}
	else {
		nMinIndex = nIndexRhtMost;
		nMaxIndex = nIndexLftMost;
	}
	if(pnX[nMinIndex] < pnX[nMaxIndex]) 
		nSign = 1;
	else
		nSign = -1;
	for(i=nMinIndex;i<nMaxIndex;i++) {
		if(((pnX[i+1] - pnX[i])*nSign) < 0) {
			*pbFillY = FALSE;
			break;
		}
	}
	if(*pbFillY) {
		for(i=nMaxIndex;i<(nMinIndex+nptMskNo);i++) {
			m = i;
			if(m>=nptMskNo)
				m -= nptMskNo;
			n = i+1;
			if(n>=nptMskNo)
				n -= nptMskNo;
			if(((pnX[m] - pnX[n]) * nSign) < 0) {
				*pbFillY = FALSE;
				break;
			}
		}
	}
	
	if(pnX != NULL)
		delete [] pnX;
	if(pnY != NULL)
		delete [] pnY;
	return 0;
}

int CMask::GetFillPairtPts(CRect* prcImgMask,
							CPoint2D* ppt2DConvexMsk, 
							int npt2DConvexMskNo,
							CRect* prcPtsBounding,
							BOOL bFillX, 
							BOOL bFillY,
							int* pnStartPos,
							int* pnEndPos,
							int* pnStartIndex,
							int* pnEndIndex)
{
	int i;
	double dStepX, dStepY;
	int nLineLength;
	double x, y;
	int nx, ny;
	int nRectWidth;
	int nRectHeight;
	CPoint ptStart, ptEnd;
	CLine2D line;
	CPoint* pptMsk;
	int npt2DMskNo;
	int nStartIndex;
	int nEndIndex;
	int nMskROITop;
	int nMskROIBot;
	int nMskROILft;
	int nMskROIRht;
	int nxl, nxr;
	int nyt, nyb;

	nMskROITop = prcImgMask->top;
	nMskROIBot = prcImgMask->bottom;
	nMskROILft = prcImgMask->left;
	nMskROIRht = prcImgMask->right;

	npt2DMskNo = npt2DConvexMskNo + 1;
	createArray(pptMsk, npt2DMskNo);
	//pptMsk = new CPoint[abs(npt2DMskNo)];
	for(i=0; i<npt2DConvexMskNo; i++) {
		pptMsk[i].x = Round(ppt2DConvexMsk[i].x);
		pptMsk[i].y = Round(ppt2DConvexMsk[i].y);
	}
	pptMsk[npt2DMskNo - 1] = pptMsk[0];
	nRectHeight = prcImgMask->Height();
	nRectWidth =  prcImgMask->Width();
	if(bFillX) {
		if(prcPtsBounding->top > prcImgMask->top)
			nStartIndex = prcPtsBounding->top;
		else
			nStartIndex = prcImgMask->top;
		if(prcPtsBounding->bottom < prcImgMask->bottom)
			nEndIndex = prcPtsBounding->bottom;
		else
			nEndIndex = prcImgMask->bottom;
		for(i=nStartIndex;i<nEndIndex;i++) {
			pnStartPos[i] = nRectWidth;
			pnEndPos[i] = -1;
		}
		for(i = 1; i < npt2DMskNo; i++) {
			ptStart = pptMsk[i-1];
			ptEnd = pptMsk[i];
			nLineLength = (int) sqrt((double) ((ptStart.x - ptEnd.x) * (ptStart.x - ptEnd.x) + (ptStart.y - ptEnd.y) * (ptStart.y - ptEnd.y)));
			if(nLineLength)	{
				dStepX = double(ptEnd.x - ptStart.x) / nLineLength;
				dStepY = double(ptEnd.y - ptStart.y) / nLineLength;
				y = ptStart.y;
				x = ptStart.x;
				if(fabs(dStepY) > fabs(dStepX))	{
					do	{
						ny = int(y);
						nx = int(x);
						if((ny >= nStartIndex) && (ny < nEndIndex)) {
							nxl = nx;
							if(nxl < nMskROILft)
								nxl = nMskROILft;
							nxr = nx + 1;
							if(nxr >= nMskROIRht)
								nxr = nMskROIRht;
							if(pnStartPos[ny] > nxl)
								pnStartPos[ny] = nxl;
							if(pnEndPos[ny] < nxr)
								pnEndPos[ny] = nxr;
						}
						y += dStepY;
						x += dStepX;
					}while((int)y != ptEnd.y); 
					ny = int(y);
					nx = int(x);
					if((ny >= nStartIndex) && (ny < nEndIndex)) {
						nxl = nx;
						if(nxl < nMskROILft)
							nxl = nMskROILft;
						nxr = nx + 1;
						if(nxr >= nMskROIRht)
							nxr = nMskROIRht;
						if(pnStartPos[ny] > nxl)
							pnStartPos[ny] = nxl;
						if(pnEndPos[ny] < nxr)
							pnEndPos[ny] = nxr;
					}
				}
				else {
					do	{
						ny = int(y);
						nx = int(x);
						if((ny >= nStartIndex) && (ny < nEndIndex)) {
							nxl = nx;
							if(nxl < nMskROILft)
								nxl = nMskROILft;
							nxr = nx + 1;
							if(nxr >= nMskROIRht)
								nxr = nMskROIRht;
							if(pnStartPos[ny] > nxl)
								pnStartPos[ny] = nxl;
							if(pnEndPos[ny] < nxr)
								pnEndPos[ny] = nxr;
						}
						y += dStepY;
						x += dStepX;
					}while((int)x != ptEnd.x);
					ny = int(y);
					nx = int(x);
					if((ny >= nStartIndex) && (ny < nEndIndex)) {
						nxl = nx;
						if(nxl < nMskROILft)
							nxl = nMskROILft;
						nxr = nx + 1;
						if(nxr >= nMskROIRht)
							nxr = nMskROIRht;
						if(pnStartPos[ny] > nxl)
							pnStartPos[ny] = nxl;
						if(pnEndPos[ny] < nxr)
							pnEndPos[ny] = nxr;
					}
				}
			}
		}
		if(nEndIndex > nRectHeight)
			nEndIndex = nRectHeight; 
	}
	if(bFillY) {
		if(prcPtsBounding->left > prcImgMask->left)
			nStartIndex = prcPtsBounding->left;
		else
			nStartIndex = prcImgMask->left;
		if(prcPtsBounding->right < prcImgMask->right)
			nEndIndex = prcPtsBounding->right;
		else
			nEndIndex = prcImgMask->right;
		for(i=nStartIndex;i<nEndIndex;i++) {
			pnStartPos[i] = nRectHeight;
			pnEndPos[i] = -1;
		}
		for(i = 1; i < npt2DMskNo; i++) {
			ptStart = pptMsk[i-1];
			ptEnd = pptMsk[i];
			nLineLength = (int) sqrt((double) ((ptStart.x - ptEnd.x) * (ptStart.x - ptEnd.x) + (ptStart.y - ptEnd.y) * (ptStart.y - ptEnd.y)));
			if(nLineLength)	{
				dStepX = double(ptEnd.x - ptStart.x) / nLineLength;
				dStepY = double(ptEnd.y - ptStart.y) / nLineLength;
				y = ptStart.y;
				x = ptStart.x;
				if(fabs(dStepY) > fabs(dStepX))	{
					do	{
						ny = int(y);
						nx = int(x);
						if((nx >= nStartIndex) && (nx < nEndIndex)) {
							nyt = ny;
							if(nyt < nMskROITop)
								nyt = nMskROITop;
							nyb = ny + 1;
							if(nyb >= nMskROIBot)
								nyb = nMskROIBot;
							if(pnStartPos[nx] > nyt)
								pnStartPos[nx] = nyt;
							if(pnEndPos[nx] < nyb)
								pnEndPos[nx] = nyb;
						}
						y += dStepY;
						x += dStepX;
					}while((int)y != ptEnd.y); 
					ny = int(y);
					nx = int(x);
					if((nx >= nStartIndex) && (nx < nEndIndex)) {
						nyt = ny;
						if(nyt < nMskROITop)
							nyt = nMskROITop;
						nyb = ny + 1;
						if(nyb >= nMskROIBot)
							nyb = nMskROIBot;
						if(pnStartPos[nx] > nyt)
							pnStartPos[nx] = nyt;
						if(pnEndPos[nx] < nyb)
							pnEndPos[nx] = nyb;
					}
				}
				else {
					do	{
						ny = int(y);
						nx = int(x);
						if((nx >= nStartIndex) && (nx < nEndIndex)) {
							nyt = ny;
							if(nyt < nMskROITop)
								nyt = nMskROITop;
							nyb = ny + 1;
							if(nyb >= nMskROIBot)
								nyb = nMskROIBot;
							if(pnStartPos[nx] > nyt)
								pnStartPos[nx] = nyt;
							if(pnEndPos[nx] < nyb)
								pnEndPos[nx] = nyb;
						}
						y += dStepY;
						x += dStepX;
					}while((int)x != ptEnd.x);
					ny = int(y);
					nx = int(x);
					if((nx >= nStartIndex) && (nx < nEndIndex)) {
						nyt = ny;
						if(nyt < nMskROITop)
							nyt = nMskROITop;
						nyb = ny + 1;
						if(nyb >= nMskROIBot)
							nyb = nMskROIBot;
						if(pnStartPos[nx] > nyt)
							pnStartPos[nx] = nyt;
						if(pnEndPos[nx] < nyb)
							pnEndPos[nx] = nyb;
					}
				}
			}
		}
		if(nEndIndex > nRectWidth)
			nEndIndex = nRectWidth; 
	}
	if(nStartIndex < 0)
		nStartIndex = 0;
	*pnStartIndex = nStartIndex;
	*pnEndIndex = nEndIndex;
	deleteArray(pptMsk);
	//delete [] pptMsk;
	return 0;
}

int CMask::FillMask(CImageBuffer *pImgMask,
					 CRect *prcImgMask,
					 BOOL bFillX, 
					 BOOL bFillY,
					 int* pnStartPos,
					 int* pnEndPos,
					 int nStartIndex,
					 int nEndIndex,
					 BYTE byteLabel)
{
	int x,y;
	int nImageWidth,nImageHeight,nImagePitch;
	int nProjWidth,nProjHeight;
	BYTE* pByteMaskPtr;
	long lIndex;

	if (!pImgMask) {
		ASSERT(!"pImgMask is a bad Pointer");
		return -1;
	}
	if (!pImgMask->IsAllocated()) {
		ASSERT(!"pImgMask is not Allocated");
		return -1;
	}
	if (ValidateRect(prcImgMask,&(pImgMask->GetImgSize()))) {
		ASSERT(!"prcImgMask is Invalid");
		return -1;
	}
	nImageWidth=pImgMask->GetImgSize().cx;
	nImageHeight=pImgMask->GetImgSize().cy;
	nImagePitch=pImgMask->GetPitch();
	nProjWidth=prcImgMask->Width();
	nProjHeight=prcImgMask->Height();
	pByteMaskPtr = pImgMask->GetImgPtr();
	if(bFillX) {
		lIndex = nImagePitch*nStartIndex;
		for (y=nStartIndex;y<nEndIndex;y++) {
			for (x=pnStartPos[y];x<pnEndPos[y];x++) 
				pByteMaskPtr[lIndex + x] = byteLabel;
			lIndex += nImagePitch;
		}
	}
	if(bFillY) {
		for (x=nStartIndex;x<nEndIndex;x++) {
			lIndex = nImagePitch*pnStartPos[x] + x;
			for (y=pnStartPos[x];y<pnEndPos[x];y++) {
				pByteMaskPtr[lIndex] = byteLabel;
				lIndex += nImagePitch;
			}
		}
	}
	return 0;
}

int CMask::ApplyGrayMaskFreeHand(CImageBuffer *pImgMask,		// Pointer to Mask Image
						CRect *prcImgMask,				// ROI Rectangle
						CArray<CPoint, CPoint>* pAryPtMsk,
						CPoint* pptOffset,
						BYTE byteLabel)					// The Label Value to Fill in the Convex region
{
	int i;
	CPoint2D *ppt2DMsk;
	CPoint ptMsk;
	int npt2DMskNo;
	int nError;

	npt2DMskNo = (int) pAryPtMsk->GetSize();
	ppt2DMsk = NULL;
	if(npt2DMskNo <=0)
		goto PASS;
	
	createArray(ppt2DMsk, npt2DMskNo);
	///ppt2DMsk = new CPoint2D[abs(npt2DMskNo)];

	for(i=0;i<npt2DMskNo;i++) {
		ptMsk = pAryPtMsk->GetAt(i);
		ppt2DMsk[i] = CPoint2D(ptMsk - *pptOffset);
	}
	nError = ApplyGrayMaskFreeHand(pImgMask,
						prcImgMask,			
						ppt2DMsk,		
						npt2DMskNo,			
						byteLabel);			
	if(nError)
		goto ErrorAbort;
PASS:
	deleteArray(ppt2DMsk);
	//if(ppt2DMsk != NULL)
	//	delete [] ppt2DMsk;
	return 0;
ErrorAbort:
	deleteArray(ppt2DMsk);
	//if(ppt2DMsk != NULL)
	//	delete [] ppt2DMsk;
	return -1;
}

int CMask::ApplyGrayMaskFreeHand(CImageBuffer *pImgMask,		// Pointer to Mask Image
						CRect *prcImgMask,				// ROI Rectangle
						CPoint *pptMsk,				// Convex Points in Order
						int nptMskNo,					// Number of Convex Points
						CPoint* pptOffset,
						BYTE byteLabel)					// The Label Value to Fill in the Convex region
{
	int i;
	CPoint2D *ppt2DMsk;
	int npt2DMskNo;
	int nError;

	ppt2DMsk = NULL;
	if(nptMskNo <=0)
		goto PASS;
	npt2DMskNo = nptMskNo;

	createArray(ppt2DMsk, npt2DMskNo);
	///ppt2DMsk = new CPoint2D[abs(npt2DMskNo)];

	for(i=0;i<npt2DMskNo;i++)
		ppt2DMsk[i] = CPoint2D(pptMsk[i] - *pptOffset);
	nError = ApplyGrayMaskFreeHand(pImgMask,
						prcImgMask,			
						ppt2DMsk,		
						npt2DMskNo,			
						byteLabel);			
	if(nError)
		goto ErrorAbort;
PASS:
	deleteArray(ppt2DMsk);
	//if(ppt2DMsk != NULL)
	//	delete [] ppt2DMsk;
	return 0;
ErrorAbort:
	deleteArray(ppt2DMsk);
	//if(ppt2DMsk != NULL)
	//	delete [] ppt2DMsk;
	return -1;
}

int CMask::ApplyGrayMaskFreeHand(CImageBuffer *pImgMask,
							CRect *prcImgMask,	
							CArray<CPoint2D, CPoint2D>* pAryPt2DMsk,	
							CPoint* pptOffset,
							BYTE byteLabel)
{
	int i;
	CPoint2D *ppt2DMsk;
	int npt2DMskNo;
	int nError;

	ppt2DMsk = NULL;
	npt2DMskNo = (int) pAryPt2DMsk->GetSize();
	if(npt2DMskNo <= 0)
		goto ErrorAbort;
	ppt2DMsk = new CPoint2D[npt2DMskNo];
	for(i=0;i<npt2DMskNo;i++)
	{
		ppt2DMsk[i] = pAryPt2DMsk->GetAt(i);
	}
	nError = ApplyGrayMaskFreeHand(pImgMask,
						prcImgMask,	
						ppt2DMsk,	
						npt2DMskNo,		
						pptOffset,
						byteLabel);		
	if(ppt2DMsk != NULL)
		delete [] ppt2DMsk;
	return 0;
ErrorAbort:
	if(ppt2DMsk != NULL)
		delete [] ppt2DMsk;
	return -1;
}


int CMask::ApplyGrayMaskFreeHand(CImageBuffer *pImgMask,		// Pointer to Mask Image
						CRect *prcImgMask,				// ROI Rectangle
						CPoint2D *ppt2DMsk,				// Convex Points in Order
						int npt2DMskNo,					// Number of Convex Points
						CPoint* pptOffset,
						BYTE byteLabel)					// The Label Value to Fill in the Convex region
{
	int i;
	CPoint2D *ppt2DMskTemp;
	int npt2DMskNoTemp;
	int nError;

	ppt2DMskTemp = NULL;
	if(npt2DMskNo <=0)
		goto PASS;
	npt2DMskNoTemp = npt2DMskNo;

	createArray(ppt2DMskTemp, npt2DMskNoTemp);
	///ppt2DMskTemp = new CPoint2D[abs(npt2DMskNoTemp)];

	for(i=0;i<npt2DMskNoTemp;i++)
		ppt2DMskTemp[i] = CPoint2D(ppt2DMsk[i] - *pptOffset);
	nError = ApplyGrayMaskFreeHand(pImgMask,
						prcImgMask,			
						ppt2DMskTemp,		
						npt2DMskNoTemp,			
						byteLabel);			
	if(nError)
		goto ErrorAbort;
PASS:
	deleteArray(ppt2DMskTemp);
	//if(ppt2DMskTemp != NULL)
	//	delete [] ppt2DMskTemp;
	return 0;
ErrorAbort:
	deleteArray(ppt2DMskTemp);
	//if(ppt2DMskTemp != NULL)
	//	delete [] ppt2DMskTemp;
	return -1;
}

int CMask::ApplyGrayMaskFreeHand(CImageBuffer *pImgMask,		// Pointer to Mask Image
						CRect *prcImgMask,				// ROI Rectangle
						CPoint2D *ppt2DMsk,				// Convex Points in Order
						int npt2DMskNo,					// Number of Convex Points
						BYTE byteLabel)					// The Label Value to Fill in the Convex region
{
	int nError;
	BOOL bFillX, bFillY;
	CRect rcBounding;
	int* pnStartPos;
	int* pnEndPos;
	int nStartIndex;
	int nEndIndex;
	pnStartPos = NULL;
	pnEndPos = NULL;
	if (!pImgMask) {
		ASSERT(!"pImgMask is a bad Pointer");
		return -1;
	}
	if (!pImgMask->IsAllocated()) {
		ASSERT(!"pImgMask is not Allocated");
		return -1;
	}
	if (ValidateRect(prcImgMask,&(pImgMask->GetImgSize()))) {
		ASSERT(!"prcImgMask is Invalid");
		return -1;
	}
	if(plContourAddress != NULL)
		delete [] plContourAddress;
	plContourAddress = NULL;
	lContourNo = 0;
	if(npt2DMskNo <= 0)
		goto ErrorAbort;
	if(npt2DMskNo < 3) {
		if(npt2DMskNo == 1) {
			pImgMask->SetPixelVal(Round(ppt2DMsk[0].x), Round(ppt2DMsk[0].y), byteLabel);
			goto PASS;
		}
		if(npt2DMskNo == 2) {
			int i;
			double dDis;
			int nDis;
			CPoint ptEdge;
			dDis = ppt2DMsk[0].GetDistance(ppt2DMsk[1]);
			nDis = Round(dDis);
			pImgMask->SetPixelVal(Round(ppt2DMsk[0].x), Round(ppt2DMsk[0].y), byteLabel);
			pImgMask->SetPixelVal(Round(ppt2DMsk[1].x), Round(ppt2DMsk[1].y), byteLabel);
			if(dDis > 1) {
				for(i=0;i<nDis;i++) {
					ptEdge.x = Round(ppt2DMsk[0].x + (ppt2DMsk[1].x - ppt2DMsk[0].x) * i/dDis);
					ptEdge.y = Round(ppt2DMsk[0].y + (ppt2DMsk[1].y - ppt2DMsk[0].y) * i/dDis);
					pImgMask->SetPixelVal(ptEdge.x, ptEdge.y, byteLabel);
				}
			}
			goto PASS;
		}
	}
	bFillX = FALSE;
	bFillY = FALSE;
	nError = CheckFastFillStatus(&ppt2DMsk[0],
								npt2DMskNo,
								&bFillX,
								&bFillY,
								&rcBounding);
	if(bFillX || bFillY) {
		if(bFillX && bFillY) {
			if(rcBounding.Width() > rcBounding.Height())
				bFillY = FALSE;
			else 
				bFillX = FALSE;
		}
		if(bFillX) {
			pnStartPos = new int[abs(prcImgMask->bottom)];
			pnEndPos = new int[abs(prcImgMask->bottom)];
		}
		if(bFillY) {
			pnStartPos = new int[abs(prcImgMask->right)];
			pnEndPos = new int[abs(prcImgMask->right)];
		}
		GetFillPairtPts(prcImgMask,
						ppt2DMsk, 
						npt2DMskNo,
						&rcBounding,
						bFillX, 
						bFillY,
						&pnStartPos[0],
						&pnEndPos[0],
						&nStartIndex,
						&nEndIndex);
		nError = FillMask(pImgMask,
						  prcImgMask,
						  bFillX, 
						  bFillY,
						  &pnStartPos[0],
						  &pnEndPos[0],
						  nStartIndex,
						  nEndIndex,
						  byteLabel);
	}
	else {
		nError = LinkPt(prcImgMask, ppt2DMsk, npt2DMskNo, 255);
		if(nError == -1)
			goto ErrorAbort;
		if(nError == 1)
			goto PASS;
		nError = FillMask(&ImgMskTemp, 255);
		if(nError == -1)
			goto ErrorAbort;
		nError = MergeMask(pImgMask, prcImgMask, &ImgMskTemp, byteLabel);
		if(nError == -1)
			goto ErrorAbort;
	}
PASS:
	if(plContourAddress != NULL)
		delete [] plContourAddress;
	plContourAddress = NULL;
	if(pnStartPos != NULL)
		delete [] pnStartPos;
	if(pnEndPos != NULL)
		delete [] pnEndPos;
	return 0;
ErrorAbort:
	if(plContourAddress != NULL)
		delete [] plContourAddress;
	plContourAddress = NULL;
	if(pnStartPos != NULL)
		delete [] pnStartPos;
	if(pnEndPos != NULL)
		delete [] pnEndPos;
	return -1;
}


CPkgIntensityAvg::CPkgIntensityAvg()
{
	nNonMaskedPercentage = 60;
	nSamplingX = 4;
	nSamplingY = 4;

	nMaskStepX = 4;
	nMaskStepY = 4;

	nInterpolationX = nSamplingX * nMaskStepX;
	nInterpolationY = nSamplingY * nMaskStepY;

	nInterpolationPosX = nInterpolationX + 1;
	nInterpolationPosY = nInterpolationY + 1;

	nNonMaskedSegmentNo = nMaskStepX * nMaskStepY * nNonMaskedPercentage / 100;
}
CPkgIntensityAvg::~CPkgIntensityAvg() {
}

int CPkgIntensityAvg::PkgIntensityAvg(CImageBuffer *pPkgImg,		// Pointer to Source Image
									CRect *prcPkg,			// ROI Rectangle of the Source Image
									BYTE byteMinPkg,
									BYTE byteMaxPkg,
									CImageBuffer *pMsk,			// Pointer to Mask Image
									CRect *prcMsk,			// ROI Rectangle of the Mask Image
									CImageBuffer *pPkgImgAvg,	// Pointer to Image of Average Intensity on the Package
									CRect *prcPkgAvg,		// ROI Rectangle of the Package Image
									int nSamplingStepX,
									int nSamplingStepY,
									int nAvgROIWidth,
									int nAvgROIHeight)
{
	int nPkgWidth, nPkgHeight;
	if (!pPkgImg) {
		ASSERT(!"pPkgImg is a bad Pointer");
		return -1;
	}
	if (!pPkgImg->IsAllocated()) {
		ASSERT(!"pPkgImg is not Allocated");
		return -1;
	}
	if (ValidateRect(prcPkg,&(pPkgImg->GetImgSize()))) {
		ASSERT(!"prcPkg is Invalid");
		return -1;
	}
	if (!pMsk) {
		ASSERT(!"pMsk is a bad Pointer");
		return -1;
	}
	if (!pMsk->IsAllocated()) {
		ASSERT(!"pMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk,&(pMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}
	if (prcPkg->Size()!=prcMsk->Size()) {
		ASSERT(!"prcPkg Size different from prcMsk Size");
		return -1;
	}
	nPkgWidth = prcPkg->Width();
	nPkgHeight = prcPkg->Height();
	// Bug Fix: nSampling X & Y were added to 1, but removed as the ImgSampling function couldn't take it.
	if((nSamplingStepX <= 0) || (nAvgROIWidth <= 0)) {
		if(nPkgWidth > 100) {
			nSamplingX = 4;
			nMaskStepX = 4;
		}
		else {
			if(nPkgWidth > 50) {
				nSamplingX = 2;
				nMaskStepX = 2;
			}
			else {
				nSamplingX = 1;
				nMaskStepX = 2;
			}
		}
	}
	else {
		nSamplingX = nSamplingStepX;
		nMaskStepX = nAvgROIWidth;
	}
	if((nSamplingStepY <= 0) || (nAvgROIHeight < 0)) {
		if(nPkgHeight > 100) {
			nSamplingY = 4;
			nMaskStepY = 4;
		}
		else {
			if(nPkgHeight > 50) {
				nSamplingY = 2;
				nMaskStepY = 2;
			}
			else {
				nSamplingY = 1;
				nMaskStepY = 2;
			}
		}
	}
	else {
		nSamplingY = nSamplingStepY;
		nMaskStepY = nAvgROIHeight;
	}
	csSampling = CSize((prcPkg->Width() - 1) / nSamplingX, (prcPkg->Height() - 1) / nSamplingY);
	nInterpolationX = nSamplingX * nMaskStepX;
	nInterpolationY = nSamplingY * nMaskStepY;
	nInterpolationPosX = nInterpolationX + 1;
	nInterpolationPosY = nInterpolationY + 1;
	nNonMaskedSegmentNo = nMaskStepX * nMaskStepY * nNonMaskedPercentage / 100;


	rcPkgSamplingImg = CRect(CPoint(0, 0), csSampling);
	rcMskSamplingImg = rcPkgSamplingImg;
	ImgPkgSampling.Allocate(AbsSize(csSampling));
	ImgMskSampling.Allocate(AbsSize(csSampling));
	Sampling(pPkgImg,
			prcPkg,
			&ImgPkgSampling,
			&rcPkgSamplingImg,
			byteMinPkg,
			byteMaxPkg,
			pMsk,
			prcMsk,
			&ImgMskSampling,
			&rcMskSamplingImg,
			&CSize(nSamplingX, nSamplingY));

	csAvgSampling = CSize(csSampling.cx / nMaskStepX,csSampling.cy / nMaskStepY);
	rcAvgSamplingImg = CRect(CPoint(0, 0), csAvgSampling);
	ImgAvgSampling.Allocate(AbsSize(csAvgSampling));
	PkgLocalIntensityAve3(&ImgAvgSampling, &rcAvgSamplingImg, &ImgPkgSampling, &rcPkgSamplingImg, &ImgMskSampling,&rcMskSamplingImg);
	PkgIntensityInterpolation(pPkgImgAvg, prcPkgAvg);
	return 0;
}

int CPkgIntensityAvg::PkgIntensityAvg(CImageBuffer *pPkgImg,		// Pointer to Source Image
									CRect *prcPkg,			// ROI Rectangle of the Source Image
									CImageBuffer *pMsk,			// Pointer to Mask Image
									CRect *prcMsk,			// ROI Rectangle of the Mask Image
									CImageBuffer *pPkgImgAvg,	// Pointer to Image of Average Intensity on the Package
									CRect *prcPkgAvg,		// ROI Rectangle of the Package Image
									int nSamplingStepX,
									int nSamplingStepY,
									int nAvgROIWidth,
									int nAvgROIHeight)
{
	// Bug Fix: nSampling X & Y were added to 1, but removed as the ImgSampling function couldn't take it.
	int nPkgWidth, nPkgHeight;
	if (!pPkgImg) {
		ASSERT(!"pPkgImg is a bad Pointer");
		return -1;
	}
	if (!pPkgImg->IsAllocated()) {
		ASSERT(!"pPkgImg is not Allocated");
		return -1;
	}
	if (ValidateRect(prcPkg,&(pPkgImg->GetImgSize()))) {
		ASSERT(!"prcPkg is Invalid");
		return -1;
	}
	if (!pMsk) {
		ASSERT(!"pMsk is a bad Pointer");
		return -1;
	}
	if (!pMsk->IsAllocated()) {
		ASSERT(!"pMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk,&(pMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}
	if (prcPkg->Size()!=prcMsk->Size()) {
		ASSERT(!"prcPkg Size different from prcMsk Size");
		return -1;
	}
	nPkgWidth = prcPkg->Width();
	nPkgHeight = prcPkg->Height();
	// Bug Fix: nSampling X & Y were added to 1, but removed as the ImgSampling function couldn't take it.
	if((nSamplingStepX <= 0) || (nAvgROIWidth <= 0)) {
		if(nPkgWidth > 100) {
			nSamplingX = 4;
			nMaskStepX = 4;
		}
		else {
			if(nPkgWidth > 50) {
				nSamplingX = 2;
				nMaskStepX = 2;
			}
			else {
				nSamplingX = 1;
				nMaskStepX = 2;
			}
		}
	}
	else {
		nSamplingX = nSamplingStepX;
		nMaskStepX = nAvgROIWidth;
	}
	if((nSamplingStepY <= 0) || (nAvgROIHeight < 0)) {
		if(nPkgHeight > 100) {
			nSamplingY = 4;
			nMaskStepY = 4;
		}
		else {
			if(nPkgHeight > 50) {
				nSamplingY = 2;
				nMaskStepY = 2;
			}
			else {
				nSamplingY = 1;
				nMaskStepY = 2;
			}
		}
	}
	else {
		nSamplingY = nSamplingStepY;
		nMaskStepY = nAvgROIHeight;
	}


	csSampling = CSize((prcPkg->Width() - 1) / nSamplingX, (prcPkg->Height() - 1) / nSamplingY);
	nInterpolationX = nSamplingX * nMaskStepX;
	nInterpolationY = nSamplingY * nMaskStepY;
	nInterpolationPosX = nInterpolationX + 1;
	nInterpolationPosY = nInterpolationY + 1;
	nNonMaskedSegmentNo = nMaskStepX * nMaskStepY * nNonMaskedPercentage / 100;

	csSampling = CSize((prcPkg->Width() - 1) / nSamplingX, (prcPkg->Height() - 1) / nSamplingY);
	rcPkgSamplingImg = CRect(CPoint(0, 0), csSampling);
	rcMskSamplingImg = rcPkgSamplingImg;
	ImgPkgSampling.Allocate(AbsSize(csSampling));
	ImgSampling(pPkgImg, prcPkg, &ImgPkgSampling, &rcPkgSamplingImg, &CSize(nSamplingX, nSamplingY));
	ImgMskSampling.Allocate(AbsSize(csSampling));
	ImgSampling(pMsk, prcMsk, &ImgMskSampling, &rcMskSamplingImg, &CSize(nSamplingX, nSamplingY));
	csAvgSampling = CSize(csSampling.cx / nMaskStepX,csSampling.cy / nMaskStepY);

	if((csAvgSampling.cx < 3) || (csAvgSampling.cy < 3))
	{
		long lHist[256];
		CAdvBinarise AdvBinarise;
		CHistogram Histogram;
		double dPercentageStart;
		double dPercentageEnd;
		int nAvgIntensity;
		int nError;
		pPkgImgAvg->Allocate(prcPkgAvg->Size());
		nError = AdvBinarise.AdvHist(pPkgImg,
						  prcPkg,
						  pMsk,
						  prcMsk,
						  255,
						  &lHist[0]);
		dPercentageStart = 30;
		dPercentageEnd = 70;
		nError = Histogram.GetAvgIntensity(dPercentageStart,
								dPercentageEnd,
								&lHist[0], 
								&nAvgIntensity);
		pPkgImgAvg->ClrImg(nAvgIntensity);

	}
	else
	{
		rcAvgSamplingImg = CRect(CPoint(0, 0), csAvgSampling);
		ImgAvgSampling.Allocate(AbsSize(csAvgSampling));
		PkgLocalIntensityAve(&ImgAvgSampling, &rcAvgSamplingImg, &ImgPkgSampling, &rcPkgSamplingImg, &ImgMskSampling,&rcMskSamplingImg);
		PkgIntensityInterpolation(pPkgImgAvg, prcPkgAvg);
	}
	return 0;
}

int CPkgIntensityAvg::Sampling(CImageBuffer *pImgSrc,
							   CRect *prcSrc,
							   CImageBuffer *pImgSamplingSrc,
							   CRect *prcSamplingSrc,
							   BYTE byteMinPkg,
							   BYTE byteMaxPkg,
							   CImageBuffer *pImgMsk,
							   CRect *prcMsk,
							   CImageBuffer *pImgSamplingMsk,
							   CRect *prcSamplingMsk,
							   CSize *pcsSampling)
{
	int x, y;
	int nWidthSamplingSrc, nHeightSamplingSrc;
	int nSamplingY,nSamplingX;
	BYTE *pRowSrc, *pColSrc;
	BYTE *pRowSrcSampling, *pColSrcSampling;
	BYTE *pRowMsk, *pColMsk;
	BYTE *pRowMskSampling, *pColMskSampling;
	CRect rcTol;
	int nImgPitchSrc, nImgPitchSrcSampling;
	int nImgPitchMsk, nImgPitchMskSampling;
	long lSkipPitchSrc, lSkipPitchSrcSampling;
	long lSkipPitchMsk, lSkipPitchMskSampling;

	nWidthSamplingSrc = prcSamplingSrc->Width();
	nHeightSamplingSrc = prcSamplingSrc->Height();
	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchSrcSampling = pImgSamplingSrc->GetPitch();
	nImgPitchMsk = pImgMsk->GetPitch();
	nImgPitchMskSampling = pImgSamplingMsk->GetPitch();
	lSkipPitchSrc=nImgPitchSrc*pcsSampling->cy;
	lSkipPitchSrcSampling=nImgPitchSrcSampling*pcsSampling->cy;
	lSkipPitchMsk=nImgPitchMsk*pcsSampling->cy;
	lSkipPitchMskSampling=nImgPitchMskSampling*pcsSampling->cy;
	nSamplingX=pcsSampling->cx;
	nSamplingY=pcsSampling->cy;
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowSrcSampling = pColSrcSampling = pImgSamplingSrc->GetImgPtr() + nImgPitchSrcSampling * prcSamplingSrc->top + prcSamplingSrc->left;
	pRowMsk = pColMsk = pImgMsk->GetImgPtr() + nImgPitchMsk * prcMsk->top + prcMsk->left;
	pRowMskSampling = pColMskSampling = pImgSamplingMsk->GetImgPtr() + nImgPitchMskSampling * prcSamplingMsk->top + prcSamplingMsk->left;

	for (y=0; y< nHeightSamplingSrc; y++,pRowSrc+=lSkipPitchSrc,pRowSrcSampling+=nImgPitchSrcSampling,pRowMsk+=lSkipPitchMsk,pRowMskSampling+=nImgPitchMskSampling) {
		for (x=0,pColSrc=pRowSrc,pColSrcSampling=pRowSrcSampling,pColMsk=pRowMsk, pColMskSampling=pRowMskSampling; x< nWidthSamplingSrc; x++,pColSrc+=nSamplingX,pColSrcSampling++,pColMsk+=nSamplingX,pColMskSampling++) {
			*pColSrcSampling = *pColSrc;
			if((*pColSrc >= byteMinPkg) && (*pColSrc <= byteMaxPkg))
				*pColMskSampling = *pColMsk;
			else
				*pColMskSampling = 0;
		}
	}
	return 0;
}

void CPkgIntensityAvg::PkgIntensityInterpolation(CImageBuffer *pPkgImgAvg,				// Pointer to Image of Average Intensity on the Package
												 CRect *prcPkgAvg)					// ROI Rectangle of the Package Image
{
	int i,j,k;
	int nSamplingAvgPitch;
	int nAvgPitch;
	long lSamplingAvgIndex;
	long lIndexTopCalaulated;
	long lIndexBottomCalaulated;
	int nBottomCalaulated;
	long lIndexStart;
	int nStartLeftX, nEndLeftX;
	int nStartRightX, nEndRightX;
	BYTE byteData[4];
	BYTE *pSamplingAvg;
	BYTE *pPkgImgAvgBuf;

	nStartLeftX = prcPkgAvg->left;
	nEndLeftX = (int)(prcPkgAvg->left + 0.5 * nInterpolationX);
	nStartRightX = (int)(prcPkgAvg->left + (ImgAvgSampling.GetImgSize().cx - 2 + 0.5) * nInterpolationX);
	nEndRightX = prcPkgAvg->right;
	pPkgImgAvgBuf = pPkgImgAvg->GetImgPtr();
	pPkgImgAvg->ClrImg(0);
	nAvgPitch = pPkgImgAvg->GetPitch();
	nSamplingAvgPitch = ImgAvgSampling.GetPitch();
	pSamplingAvg = ImgAvgSampling.GetImgPtr();
	lSamplingAvgIndex = 0;
	for(i = 0; i < (ImgAvgSampling.GetImgSize().cy - 2); i++) {
		for(j = 0; j < (ImgAvgSampling.GetImgSize().cx - 2); j++) {
			lIndexStart = (int)(((i + 0.5) * nInterpolationY + prcPkgAvg->top) * nAvgPitch + (j + 0.5) * nInterpolationX + prcPkgAvg->left);
			byteData[0] = pSamplingAvg[lSamplingAvgIndex + j];
			byteData[1] = pSamplingAvg[lSamplingAvgIndex + j + 1];
			byteData[2] = pSamplingAvg[lSamplingAvgIndex + nSamplingAvgPitch + j];
			byteData[3] = pSamplingAvg[lSamplingAvgIndex + nSamplingAvgPitch + j + 1];
			Fast2DInterpolation(&byteData[0], pPkgImgAvg, prcPkgAvg,lIndexStart);

		}
		lIndexStart = (int)(((i + 0.5) * nInterpolationY + prcPkgAvg->top) * nAvgPitch);
		for(k = 0; k <= nInterpolationY; k++) {
			for(j = nStartLeftX; j < nEndLeftX; j++) {
				pPkgImgAvgBuf[lIndexStart + j] = pPkgImgAvgBuf[lIndexStart + nEndLeftX];
			}
			for(j = nStartRightX; j < nEndRightX; j++) {
				pPkgImgAvgBuf[lIndexStart + j] = pPkgImgAvgBuf[lIndexStart + nStartRightX];
			}
			lIndexStart += nAvgPitch;
		}
		lSamplingAvgIndex += nSamplingAvgPitch;
	}
	lIndexTopCalaulated=(int)((0.5 * nInterpolationY + prcPkgAvg->top) * nAvgPitch);
	lIndexStart = prcPkgAvg->top * nAvgPitch;
	for(k = 0; k < (nInterpolationY / 2); k++) {
		for(j = nStartLeftX; j < nEndRightX; j++) {
			pPkgImgAvgBuf[lIndexStart + j] = pPkgImgAvgBuf[lIndexTopCalaulated + j];
		}
		lIndexStart += nAvgPitch;
	}
	nBottomCalaulated = (int)((ImgAvgSampling.GetImgSize().cy - 1.5) * nInterpolationY + prcPkgAvg->top);
	lIndexBottomCalaulated = nBottomCalaulated * nAvgPitch;
	lIndexStart = lIndexBottomCalaulated;
	for(k = nBottomCalaulated;k < prcPkgAvg->bottom; k++) {
		for(j = nStartLeftX; j < nEndRightX; j++) {
			pPkgImgAvgBuf[lIndexStart + j] = pPkgImgAvgBuf[lIndexBottomCalaulated + j];
		}
		lIndexStart += nAvgPitch;
	}
}

void CPkgIntensityAvg::Fast2DInterpolation(BYTE *pbyte,						// Four Corner Values: TopLeft, TopRight, BottomLeft, BottomRight 
										   CImageBuffer *pPkgImgAvg,				// Pointer to ImageBuf where the Interpolation Values are Saved
										   CRect *prcPkgAvg,				// ROI
										   long lIndexStart)				// The Start Index Position that is Used to Save the Interpolation Values
{
	int i;
	BYTE *pYFirstCol, *pYLastCol;
	BYTE *pPkgImgAvgBuf;
	long lIndex;
	int nPitch;
	
	pPkgImgAvgBuf = pPkgImgAvg->GetImgPtr();
	nPitch = pPkgImgAvg->GetPitch();

	pYFirstCol = new BYTE[abs(nInterpolationPosY)];
	pYLastCol = new BYTE[abs(nInterpolationPosY)];

	pYFirstCol[0] = pbyte[0];
	pYFirstCol[nInterpolationPosY - 1] = pbyte[2];
	Fast1DInterpolation(pYFirstCol, nInterpolationPosY);

	pYLastCol[0] = pbyte[1];
	pYLastCol[nInterpolationPosY-1] = pbyte[3];
	Fast1DInterpolation(pYLastCol, nInterpolationPosY);

	lIndex = lIndexStart;
	for(i = 0; i < nInterpolationPosY; i++) {
		pPkgImgAvgBuf[lIndex] = pYFirstCol[i];
		pPkgImgAvgBuf[lIndex + nInterpolationPosX - 1] = pYLastCol[i];
		Fast1DInterpolation(pPkgImgAvgBuf, lIndex, nInterpolationPosX);
		lIndex += nPitch;
	}

	delete [] pYFirstCol;
	delete [] pYLastCol;
}


void CPkgIntensityAvg::PkgLocalIntensityAve(CImageBuffer *pSamplingAvg,		// Pointer to Image of Sampling Average
											CRect *prcSamplingAvg,		// ROI of Image of Sampling Average
											CImageBuffer *pImgSampling,		// Pointer to Sampling Source Image
											CRect *prcImgSampling,		// ROI of Sampling Source Image
											CImageBuffer *pMskSampling,		// Pointer to Sampling Mask Image
											CRect *prcMskSampling)		// ROI of Sampling Mask Image
{
	int i,j,m,n;
	long lSum;
	int nMissingMeanPos[1000];
	int nMissingNo;
	int nPresentMeanX[1000], nPresentMeanY[1000];
	int nPresentNo;
	int nBeforePos;
	int nNextPos;

	int nSumNo;
	int nSegmentX, nSegmentY;
	int nImgPitch, nMskPitch, nMeanPitch;
	long lImgRowOrg, lImgRow;
	long lMskRowOrg, lMskRow;
	long lMeanRowOrg, lMeanRow;
	BYTE *pImgBuf, *pMskBuf, *pMeanBuf;
	long lMeanNo;

	nSegmentX = pImgSampling->GetImgSize().cx / nMaskStepX;
	nSegmentY = pImgSampling->GetImgSize().cy / nMaskStepY;
	*prcSamplingAvg = CRect(CPoint(0, 0), CSize(nSegmentX, nSegmentY));

	pImgBuf = pImgSampling->GetImgPtr();
	nImgPitch = pImgSampling->GetPitch();

	pMskBuf = pMskSampling->GetImgPtr();
	nMskPitch = pMskSampling->GetPitch();

	pMeanBuf = pSamplingAvg->GetImgPtr();
	nMeanPitch = pSamplingAvg->GetPitch();

	lMeanNo = nMeanPitch * nSegmentY;

	lImgRowOrg = prcImgSampling->top * nImgPitch + prcImgSampling->left;
	lMskRowOrg = prcMskSampling->top * nMskPitch + prcMskSampling->left;
	lMeanRowOrg = prcSamplingAvg->top * nMeanPitch + prcSamplingAvg->left;

	lMeanRow = lMeanRowOrg;
	for(i = 0; i < nSegmentY; i++) {
		nPresentNo = 0;
		nMissingNo = 0;
		for(j = 0; j < nSegmentX; j++) {
			nSumNo = 0;
			lSum = 0;
			for(m = 0; m < nMaskStepY; m++) {
				lMskRow = lMskRowOrg + (i * nMaskStepY + m) * nMskPitch + j * nMaskStepX;
				lImgRow = lImgRowOrg + (i * nMaskStepY + m) * nImgPitch + j * nMaskStepX;
				for(n = 0; n < nMaskStepX; n++) {
					if(pMskBuf[lMskRow + n]) {
						lSum += pImgBuf[lImgRow + n];
						nSumNo++;
					}
				}
			}
			if(nSumNo > nNonMaskedSegmentNo) {
				pMeanBuf[lMeanRow + j] = (BYTE) (lSum / nSumNo);
				nPresentMeanX[nPresentNo] = j;
				nPresentMeanY[nPresentNo] = pMeanBuf[lMeanRow + j];
				nPresentNo++;
			}
			else {
				pMeanBuf[lMeanRow + j] = 0;
				nMissingMeanPos[nMissingNo] = j;
				nMissingNo++;
			}
		}

		if((nMissingNo) && (nPresentNo)) {
			for(m = 0; m < nMissingNo; m++) {
				nBeforePos = 0;
				nNextPos = 0;
				if(nMissingMeanPos[m] > nPresentMeanX[nPresentNo - 1]) 
						pMeanBuf[lMeanRow + nMissingMeanPos[m]] = nPresentMeanY[nPresentNo - 1];
				else {
					if(nMissingMeanPos[m] > nPresentMeanX[0]) {
						for(n = 1; n < nPresentNo; n++) {
							if((nMissingMeanPos[m] > nPresentMeanX[n - 1]) && (nMissingMeanPos[m] < nPresentMeanX[n])) {
								nBeforePos = n - 1;
								nNextPos = n;
								break;
							}
						}
						pMeanBuf[lMeanRow + nMissingMeanPos[m]] = nPresentMeanY[nBeforePos] + ((nMissingMeanPos[m] - nPresentMeanX[nBeforePos]) *
							(nPresentMeanY[nNextPos] - nPresentMeanY[nBeforePos])) / (nPresentMeanX[nNextPos] - nPresentMeanX[nBeforePos]);
					}
					else
						pMeanBuf[lMeanRow+nMissingMeanPos[m]] = nPresentMeanY[0];
				}
			}
		}
		lMeanRow += nMeanPitch;
	}
	lSum = 0;
	nSumNo = 0;
	for(i = 0; i< lMeanNo; i++) {
		if(pMeanBuf[i] > 0) {
			lSum += pMeanBuf[i];
			nSumNo++;
		}
	}
	if(nSumNo)
		byteAvg = (BYTE) (lSum / nSumNo);
	else
		byteAvg = 0;
	for(i = 0; i < lMeanNo; i++) {
		if(pMeanBuf[i] == 0) {
			pMeanBuf[i] = byteAvg;
		}
	}
}

void CPkgIntensityAvg::PkgLocalIntensityAve3(CImageBuffer *pSamplingAvg,		// Pointer to Image of Sampling Average
											CRect *prcSamplingAvg,		// ROI of Image of Sampling Average
											CImageBuffer *pImgSampling,		// Pointer to Sampling Source Image
											CRect *prcImgSampling,		// ROI of Sampling Source Image
											CImageBuffer *pMskSampling,		// Pointer to Sampling Mask Image
											CRect *prcMskSampling)		// ROI of Sampling Mask Image
{
	int i,j,m,n;
	long lSum;

	int nSumNo;
	int nSegmentX, nSegmentY;
	int nImgPitch, nMskPitch, nMeanPitch;
	long lImgRowOrg, lImgRow;
	long lMskRowOrg, lMskRow;
	long lMeanRowOrg, lMeanRow;
	BYTE *pImgBuf, *pMskBuf, *pMeanBuf;
	long lHist[256];
	long lMeanNo;
CHistogram Histogram;
double dPercentageStart;
double dPercentageEnd;
int nAvgIntensity;
int nWhiteIntensity;
int nBlackIntensity;
CImageBuffer ImgDif;
BYTE* pImgDifPtr;
CImageBuffer ImgMask;
BYTE* pImgMaskPtr;
double dAvgDif;
CAdvBinarise AdvBinarise;
CArray<CPoint, CPoint>* pAryPtMissingByRow;
CArray<CPoint, CPoint>* pAryPtPresentByRow;
CArray<CPoint, CPoint>* pAryPtMissingByCol;
CArray<CPoint, CPoint>* pAryPtPresentByCol;
BYTE byteVal;
int nError;
	
dPercentageStart = 20;
dPercentageEnd = 80;

pAryPtMissingByRow = NULL;
pAryPtPresentByRow = NULL;
pAryPtMissingByCol = NULL;
pAryPtPresentByCol = NULL;

	nSegmentX = pImgSampling->GetImgSize().cx / nMaskStepX;
	nSegmentY = pImgSampling->GetImgSize().cy / nMaskStepY;
	*prcSamplingAvg = CRect(CPoint(0, 0), CSize(nSegmentX, nSegmentY));

	ImgDif.Allocate(AbsSize(prcSamplingAvg->Size()));
	ImgDif.ClrImg(0);
	pImgDifPtr = ImgDif.GetImgPtr();
	ImgMask.Allocate(AbsSize(prcSamplingAvg->Size()));
	ImgMask.ClrImg(0);
	pImgMaskPtr = ImgMask.GetImgPtr();

	pImgBuf = pImgSampling->GetImgPtr();
	nImgPitch = pImgSampling->GetPitch();

	pMskBuf = pMskSampling->GetImgPtr();
	nMskPitch = pMskSampling->GetPitch();

	pMeanBuf = pSamplingAvg->GetImgPtr();
	nMeanPitch = pSamplingAvg->GetPitch();

	lMeanNo = nMeanPitch * nSegmentY;

	lImgRowOrg = prcImgSampling->top * nImgPitch + prcImgSampling->left;
	lMskRowOrg = prcMskSampling->top * nMskPitch + prcMskSampling->left;
	lMeanRowOrg = prcSamplingAvg->top * nMeanPitch + prcSamplingAvg->left;

	lMeanRow = lMeanRowOrg;
	for(i = 0; i < nSegmentY; i++) {
		for(j = 0; j < nSegmentX; j++) {
			nSumNo = 0;
			lSum = 0;
			for(m=0;m<256;m++)
				lHist[m] = 0;
			for(m = 0; m < nMaskStepY; m++) {
				lMskRow = lMskRowOrg + (i * nMaskStepY + m) * nMskPitch + j * nMaskStepX;
				lImgRow = lImgRowOrg + (i * nMaskStepY + m) * nImgPitch + j * nMaskStepX;
				for(n = 0; n < nMaskStepX; n++) {
					if(pMskBuf[lMskRow + n]) {
						lHist[pImgBuf[lImgRow + n]]++; 
						lSum += pImgBuf[lImgRow + n];
						nSumNo++;
					}
				}
			}
			if(nSumNo > nNonMaskedSegmentNo) {
				nError = Histogram.GetAvgIntensity(dPercentageStart,
												dPercentageEnd,
												&lHist[0], 
												&nAvgIntensity);
				nError = Histogram.GetAvgIntensity(dPercentageStart,
												50,
												&lHist[0], 
												&nBlackIntensity);
				nError = Histogram.GetAvgIntensity(50,
												dPercentageEnd,
												&lHist[0], 
												&nWhiteIntensity);
				pMeanBuf[lMeanRow + j] = nAvgIntensity;
				pImgDifPtr[lMeanRow + j] = nWhiteIntensity - nBlackIntensity;
				pImgMaskPtr[lMeanRow + j] = 255;
			}
			else {
			}
		}
		lMeanRow += nMeanPitch;
	}
	nError = AdvBinarise.AdvHist(&ImgDif,
						  prcSamplingAvg,
						  &ImgMask,
						  prcSamplingAvg,
						  255,
						  &lHist[0]);
	dPercentageStart = 90;
	dPercentageEnd = 100;
	nError = Histogram.GetAvgIntensity(dPercentageStart,
									dPercentageEnd,
									&lHist[0], 
									&dAvgDif);

	lMeanRow = lMeanRowOrg;
	for(i = 0; i < nSegmentY; i++) {
		for(j = 0; j < nSegmentX; j++) {
			if(pImgDifPtr[lMeanRow + j] > dAvgDif) {
				pImgMaskPtr[lMeanRow + j] = 0;
			}
		}
		lMeanRow += nMeanPitch;
	}
	pAryPtMissingByRow = new CArray<CPoint, CPoint>[abs(nSegmentY)];
	pAryPtPresentByRow = new CArray<CPoint, CPoint>[abs(nSegmentY)];
	pAryPtMissingByCol = new CArray<CPoint, CPoint>[abs(nSegmentX)];
	pAryPtPresentByCol = new CArray<CPoint, CPoint>[abs(nSegmentX)];
	lMeanRow = lMeanRowOrg;
	for(i = 0; i < nSegmentY; i++) {
		for(j = 0; j < nSegmentX; j++) {
			if(pImgMaskPtr[lMeanRow + j] > 0) {
				pAryPtPresentByRow[i].Add(CPoint(j, pMeanBuf[lMeanRow + j]));
				pAryPtPresentByCol[j].Add(CPoint(i, pMeanBuf[lMeanRow + j]));
			}
			else {
				pAryPtMissingByRow[i].Add(CPoint(j, 0));
				pAryPtMissingByCol[j].Add(CPoint(i, 0));
			}
		}
		lMeanRow += nMeanPitch;
	}
	for(i = 0; i < nSegmentY; i++) {
		for(j = 0; j < pAryPtMissingByRow[i].GetSize(); j++) {
			nError = GetInterpolation(&pAryPtPresentByRow[i],
									&pAryPtPresentByCol[pAryPtMissingByRow[i].GetAt(j).x],
									pAryPtMissingByRow[i].GetAt(j).x,
									i,
									&byteVal);
			pMeanBuf[i * nMeanPitch + pAryPtMissingByRow[i].GetAt(j).x] = byteVal;
		}
	}

	if(pAryPtMissingByRow != NULL) {
		for(i=0;i<pAryPtMissingByRow->GetSize();i++)
			pAryPtMissingByRow[i].RemoveAll();
		delete [] pAryPtMissingByRow;
	}
	if(pAryPtPresentByRow != NULL) {
		for(i=0;i<pAryPtPresentByRow->GetSize();i++)
			pAryPtPresentByRow[i].RemoveAll();
		delete [] pAryPtPresentByRow;
	}
	if(pAryPtMissingByCol != NULL) {
		for(i=0;i<pAryPtMissingByCol->GetSize();i++)
			pAryPtMissingByCol[i].RemoveAll();
		delete [] pAryPtMissingByCol;
	}
	if(pAryPtPresentByCol != NULL) {
		for(i=0;i<pAryPtPresentByCol->GetSize();i++)
			pAryPtPresentByCol[i].RemoveAll();
		delete [] pAryPtPresentByCol;
	} 
}

int CPkgIntensityAvg::GetInterpolation(CArray<CPoint, CPoint>* pAryPtPresentByRow,
						CArray<CPoint, CPoint>* pAryPtPresentByCol,
						int nCol,
						int nRow,
						BYTE* pbyteVal)
{
	int i;
	int nPresentColNo;
	int nPresentRowNo;
	
	nPresentColNo = (int) pAryPtPresentByRow->GetSize();
	nPresentRowNo = (int) pAryPtPresentByCol->GetSize();
	if(nPresentColNo > 0) {
		if((nCol > pAryPtPresentByRow->GetAt(0).x) && 
			(nCol < pAryPtPresentByRow->GetAt(nPresentColNo-1).x)) {
			for(i=1;i<nPresentColNo;i++) {
				if((nCol > pAryPtPresentByRow->GetAt(i-1).x) && (nCol < pAryPtPresentByRow->GetAt(i).x)) {
					if(pAryPtPresentByRow->GetAt(i).x != pAryPtPresentByRow->GetAt(i-1).x)
						*pbyteVal = (BYTE)(pAryPtPresentByRow->GetAt(i-1).y + (pAryPtPresentByRow->GetAt(i).y - pAryPtPresentByRow->GetAt(i-1).y) * (nCol - pAryPtPresentByRow->GetAt(i-1).x)/(pAryPtPresentByRow->GetAt(i).x - pAryPtPresentByRow->GetAt(i-1).x));
					goto PASS;
				}
			}
		}
	}
	if(nPresentRowNo > 0) {
		if((nRow > pAryPtPresentByCol->GetAt(0).x) && 
			(nRow < pAryPtPresentByCol->GetAt(nPresentRowNo-1).x)) {
			for(i=1;i<nPresentRowNo;i++) {
				if((nRow > pAryPtPresentByCol->GetAt(i-1).x) && (nRow < pAryPtPresentByCol->GetAt(i).x)) {
					if(pAryPtPresentByCol->GetAt(i).x != pAryPtPresentByCol->GetAt(i-1).x) {
						*pbyteVal = (BYTE)(pAryPtPresentByCol->GetAt(i-1).y + (pAryPtPresentByCol->GetAt(i).y - pAryPtPresentByCol->GetAt(i-1).y) * (nRow - pAryPtPresentByCol->GetAt(i-1).x)/(pAryPtPresentByCol->GetAt(i).x - pAryPtPresentByCol->GetAt(i-1).x));
						goto PASS;
					}
				}
			}
		}
	}
	if((nPresentColNo > 0) && (nPresentRowNo == 0)) {
		if(nCol < pAryPtPresentByRow->GetAt(0).x) 
			*pbyteVal = (BYTE)(pAryPtPresentByRow->GetAt(0).y);
		if(nCol > pAryPtPresentByRow->GetAt(nPresentColNo-1).x) 
			*pbyteVal = (BYTE)(pAryPtPresentByRow->GetAt(nPresentColNo-1).y);
	}
	else {
		if(nPresentRowNo > 0) {
			if(nRow < pAryPtPresentByCol->GetAt(0).x) 
				*pbyteVal = (BYTE)(pAryPtPresentByCol->GetAt(0).y);
			if(nRow > pAryPtPresentByCol->GetAt(nPresentRowNo-1).x) 
				*pbyteVal = (BYTE)(pAryPtPresentByCol->GetAt(nPresentRowNo-1).y);
		}
	}
PASS:
	return 0;
}

void CPkgIntensityAvg::Fast1DInterpolation(BYTE *pbyte,			// Pointer to the Buffer
										   int nSize)			// Size of Buffer
{
	int nStep, i, nDblStep;
	if(pbyte[0] != pbyte[nSize])
	{
		nStep = nSize >> 1;
		nDblStep = nStep << 1;
		do {
			for(i = nStep; i < nSize; i += nDblStep) {
				pbyte[i] = (pbyte[i - nStep] + pbyte[i + nStep]) / 2;
			}
			nStep = nStep >> 1;
			nDblStep = nStep << 1;
		}while(nStep > 0);
	}
	else {
		for(i = 1; i < nSize; i++) pbyte[i] = pbyte[0];
	}
}

void CPkgIntensityAvg::Fast1DInterpolation(BYTE *pbyte,			// Pointer to the Buffer
										   long lStartIndex,	// Position to the Starting Point
										   int nSize)			// Size of Buffer
{
	int nStep, i, nDblStep;
	if(pbyte[lStartIndex] != pbyte[lStartIndex + nSize])
	{
		nStep = nSize >> 1;
		nDblStep = nStep << 1;
		do {
			for(i = lStartIndex + nStep; i < (lStartIndex + nSize);i += nDblStep) {
				pbyte[i] = (pbyte[i - nStep] + pbyte[i + nStep]) / 2;
			}
			nStep = nStep >> 1;
			nDblStep = nStep << 1;
		}while(nStep > 0);
	}
	else {
		for(i = 1; i < nSize; i++) pbyte[lStartIndex+i] = pbyte[lStartIndex];
	}
}

void CPkgIntensityAvg::Fast2DInterpolation(BYTE *pbyte,			// Pointer to Buffer With Known Four Corner Values: TopLeft, TopRight, BottomLeft, BottomRight 
										   CSize csRect)		// Size of the Buffer
{
	int nXSize, nYSize;
	int i;
	BYTE *pYFirstCol, *pYLastCol;
	long lSize;
	long lIndex;
	nXSize = csRect.cx;
	nYSize = csRect.cy;
	lSize = nXSize * nYSize;

	pYFirstCol = new BYTE[abs(nYSize)];
	pYLastCol = new BYTE[abs(nYSize)];

	pYFirstCol[0] = pbyte[0];
	pYFirstCol[nYSize - 1] = pbyte[lSize - nXSize];
	Fast1DInterpolation(pYFirstCol, nYSize);

	pYLastCol[0] = pbyte[nXSize - 1];
	pYLastCol[nYSize - 1] = pbyte[lSize - 1];
	Fast1DInterpolation(pYLastCol, nYSize);

	lIndex = 0;
	for(i = 0; i < nYSize; i++) {
		pbyte[lIndex] = pYFirstCol[i];
		pbyte[lIndex + nXSize - 1] = pYLastCol[i];
		Fast1DInterpolation(pbyte, lIndex, nXSize);
		lIndex += nXSize;
	}

	delete [] pYFirstCol;
	delete [] pYLastCol;
}


CAdvBinarise::CAdvBinarise() {
}

CAdvBinarise::~CAdvBinarise() {
}

int CAdvBinarise::Binarise(CImageBuffer *pImgSrc,			// Pointer to Source Image
						   CRect *prcSrc,				// ROI of Source Image
						   int nBlobType,
						   CImageBuffer *pImgMsk, 			// Pointer to Mask Image
						   CRect *prcMsk, 				// ROI of Mask Image
						   BYTE byteThresVal, 
						   CImageBuffer *pImgDst,  			// Pointer to Destination Image
						   CRect *prcDst) {				// ROI of Destination Image
						   
	int x,y;
	int nImgPitchSrc, nImgPitchDst;
	int nImgPitchMsk;
	int nWidthSrc, nHeightSrc;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	BYTE *pRowMsk, *pColMsk;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst, &(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if (prcSrc->Size() != prcMsk->Size()) {
		ASSERT(!"prectSrc Size different from prcMsk Size");
		return -1;
	}

	if (prcSrc->Size() != prcDst->Size()) {
		ASSERT(!"prectSrc Size different from prectDst Size");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();
	nImgPitchMsk = pImgMsk->GetPitch();;

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;
	pRowMsk = pColMsk = pImgMsk->GetImgPtr() + nImgPitchMsk * prcMsk->top + prcMsk->left;

 	if(nBlobType == WHITE) {
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, pRowDst += nImgPitchDst, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, pColDst = pRowDst, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, pColDst++, pColMsk++) {
				if(*pColMsk) {
					if (*pColSrc < byteThresVal)
						*pColDst = 0; 
					else
						*pColDst = 255;
				}
				else
					*pColDst = 0; 
			}
		}
	}
	else {
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, pRowDst += nImgPitchDst, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, pColDst = pRowDst, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, pColDst++, pColMsk++) {
				if(*pColMsk) {
					if (*pColSrc < byteThresVal)
						*pColDst = 0; 
					else
						*pColDst = 255;
				}
				else
					*pColDst = 255; 
			}
		}
	}
	return 0;
}



int CAdvBinarise::Binarise(CImageBuffer *pImgSrc,			// Pointer to Source Image
						   CRect *prcSrc,				// ROI of Source Image
						   int nBlobType,
						   CImageBuffer *pImgMsk, 			// Pointer to Mask Image
						   CRect *prcMsk, 				// ROI of Mask Image
						   BYTE byteThresVal, 
						   CImageBuffer *pImgDst,  			// Pointer to Destination Image
						   CRect *prcDst,				// ROI of Destination Image
						   long* plBlobPixelAddress,
						   long* plBlobPixelNo) {
						   
	int x,y;
	int nImgPitchSrc, nImgPitchDst;
	int nImgPitchMsk;
	int nWidthSrc, nHeightSrc;
	BYTE *pRowSrc, *pColSrc;
	BYTE *pRowMsk, *pColMsk;
	BYTE *pImgDstPtr;
	long lBlobPixelNo;
	long lDstRowIndex;
	long lDstIndex;

	*plBlobPixelNo = 0;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst, &(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if (prcSrc->Size() != prcMsk->Size()) {
		ASSERT(!"prectSrc Size different from prcMsk Size");
		return -1;
	}

	if (prcSrc->Size() != prcDst->Size()) {
		ASSERT(!"prectSrc Size different from prectDst Size");
		return -1;
	}

	pImgDstPtr = pImgDst->GetImgPtr();
	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();
	nImgPitchMsk = pImgMsk->GetPitch();;

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowMsk = pColMsk = pImgMsk->GetImgPtr() + nImgPitchMsk * prcMsk->top + prcMsk->left;
	lDstRowIndex = prcDst->top * nImgPitchDst + prcDst->left;
	lBlobPixelNo = 0;

 	if(nBlobType == WHITE) {
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, lDstRowIndex += nImgPitchDst, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, lDstIndex = lDstRowIndex, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, lDstIndex++, pColMsk++) {
				if(*pColMsk) {
					if (*pColSrc < byteThresVal)
						pImgDstPtr[lDstIndex] = 0; 
					else {
						pImgDstPtr[lDstIndex] = 255; 
						plBlobPixelAddress[lBlobPixelNo++] = lDstIndex;
					}
				}
				else
					pImgDstPtr[lDstIndex] = 0; 
			}
		}
	}
	else {
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, lDstRowIndex += nImgPitchDst, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, lDstIndex = lDstRowIndex, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, lDstIndex++, pColMsk++) {
				if(*pColMsk) {
					if (*pColSrc < byteThresVal) {
						pImgDstPtr[lDstIndex] = 0; 
						plBlobPixelAddress[lBlobPixelNo++] = lDstIndex;
					}
					else
						pImgDstPtr[lDstIndex] = 255; 
				}
				else
					pImgDstPtr[lDstIndex] = 255; 
			}
		}
	}
	*plBlobPixelNo = lBlobPixelNo;
	return 0;
}


int CAdvBinarise::Binarise(CImageBuffer *pImgSrc,			// Pointer to Source Image
						   CRect *prcSrc,				// ROI of Source Image
						   int nBlobType,
						   CImageBuffer *pImgMsk, 			// Pointer to Mask Image
						   CRect *prcMsk, 				// ROI of Mask Image
						   CImageBuffer *pImgThreshold,  	// Pointer to Threshold Image
						   CRect *prcThreshold,  		// ROI of Threshold Image
						   CImageBuffer *pImgDst,  			// Pointer to Destination Image
						   CRect *prcDst) {				// ROI of Destination Image
						   
	int x,y;
	int nImgPitchSrc, nImgPitchDst;
	int nImgPitchThrshold, nImgPitchMsk;
	int nWidthSrc, nHeightSrc;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	BYTE *pRowThreshold, *pColThreshold, *pRowMsk, *pColMsk;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}

	if (!pImgThreshold) {
		ASSERT(!"pImgThreshold is a bad Pointer");
		return -1;
	}
	if (!pImgThreshold->IsAllocated()) {
		ASSERT(!"pImgThreshold is not Allocated");
		return -1;
	}
	if (ValidateRect(prcThreshold, &(pImgThreshold->GetImgSize()))) {
		ASSERT(!"prcThreshold is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst, &(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if (prcSrc->Size() != prcMsk->Size()) {
		ASSERT(!"prectSrc Size different from prcMsk Size");
		return -1;
	}

	if (prcSrc->Size() != prcThreshold->Size()) {
		ASSERT(!"prectSrc Size different from prcThreshold Size");
		return -1;
	}

	if (prcSrc->Size() != prcDst->Size()) {
		ASSERT(!"prectSrc Size different from prectDst Size");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();
	nImgPitchThrshold = pImgThreshold->GetPitch();;
	nImgPitchMsk = pImgMsk->GetPitch();;

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;
	pRowThreshold = pColThreshold = pImgThreshold->GetImgPtr() + nImgPitchThrshold * prcThreshold->top + prcThreshold->left;
	pRowMsk = pColMsk = pImgMsk->GetImgPtr() + nImgPitchMsk * prcMsk->top + prcMsk->left;

 	if(nBlobType == WHITE) {
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, pRowDst += nImgPitchDst, pRowThreshold += nImgPitchThrshold, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, pColDst = pRowDst, pColThreshold = pRowThreshold, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, pColDst++, pColThreshold++, pColMsk++) {
				if(*pColMsk) {
					if (*pColSrc < *pColThreshold)
						*pColDst = 0; 
					else
						*pColDst = 255;
				}
				else
					*pColDst = 0; 
			}
		}
	}
	else {
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, pRowDst += nImgPitchDst, pRowThreshold += nImgPitchThrshold, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, pColDst = pRowDst, pColThreshold = pRowThreshold, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, pColDst++, pColThreshold++, pColMsk++) {
				if(*pColMsk) {
					if (*pColSrc < *pColThreshold)
						*pColDst = 0; 
					else
						*pColDst = 255;
				}
				else
					*pColDst = 255; 
			}
		}
	}
	return 0;
}


int CAdvBinarise::Binarise(CImageBuffer *pImgSrc,		
						   CRect *prcSrc,	
						   int nBlobType,
						   CImageBuffer *pImgMsk,
						   CRect *prcMsk, 	
						   CImageBuffer *pImgMean,
						   CRect *prcMean,
						   BYTE byteContrast,
						   CImageBuffer *pImgDst,  
						   CRect *prcDst) 
{	
						   
	int x,y;
	int nImgPitchSrc, nImgPitchDst;
	int nImgPitchMean, nImgPitchMsk;
	int nWidthSrc, nHeightSrc;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	BYTE *pRowMean, *pColMean, *pRowMsk, *pColMsk;
	BYTE byteThreshold;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}

	if (!pImgMean) {
		ASSERT(!"pImgMean is a bad Pointer");
		return -1;
	}
	if (!pImgMean->IsAllocated()) {
		ASSERT(!"pImgMean is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMean, &(pImgMean->GetImgSize()))) {
		ASSERT(!"prcMean is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst, &(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if (prcSrc->Size() != prcMsk->Size()) {
		ASSERT(!"prectSrc Size different from prcMsk Size");
		return -1;
	}

	if (prcSrc->Size() != prcMean->Size()) {
		ASSERT(!"prectSrc Size different from prcMean Size");
		return -1;
	}

	if (prcSrc->Size() != prcDst->Size()) {
		ASSERT(!"prectSrc Size different from prectDst Size");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();
	nImgPitchMean = pImgMean->GetPitch();;
	nImgPitchMsk = pImgMsk->GetPitch();;

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;
	pRowMean = pColMean = pImgMean->GetImgPtr() + nImgPitchMean * prcMean->top + prcMean->left;
	pRowMsk = pColMsk = pImgMsk->GetImgPtr() + nImgPitchMsk * prcMsk->top + prcMsk->left;

 	if(nBlobType == WHITE) {
		BYTE byte255SubContrast;
		byte255SubContrast = 255 - byteContrast;
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, pRowDst += nImgPitchDst, pRowMean += nImgPitchMean, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, pColDst = pRowDst, pColMean = pRowMean, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, pColDst++, pColMean++, pColMsk++) {
				if(*pColMsk) {
					if(*pColMean < byte255SubContrast) 
						byteThreshold = *pColMean + byteContrast;
					else
						byteThreshold = 255;
					if (*pColSrc >= byteThreshold)
						*pColDst = 255; 
					else
						*pColDst = 0;
				}
				else
					*pColDst = 0; 
			}
		}
	}
	else {
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, pRowDst += nImgPitchDst, pRowMean += nImgPitchMean, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, pColDst = pRowDst, pColMean = pRowMean, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, pColDst++, pColMean++, pColMsk++) {
				if(*pColMsk) {
					if(*pColMean > byteContrast)
						byteThreshold = *pColMean - byteContrast;
					else
						byteThreshold = 0;
					if (*pColSrc <= byteThreshold)
						*pColDst = 0; 
					else
						*pColDst = 255;
				}
				else
					*pColDst = 255; 
			}
		}
	}
	return 0;
}

int CAdvBinarise::Binarise(CImageBuffer *pImgSrc,		
						   CRect *prcSrc,	
						   int nBlobType,
						   CImageBuffer *pImgMsk,
						   CRect *prcMsk, 	
						   CImageBuffer *pImgMean,
						   CRect *prcMean,
						   BYTE byteContrast,
						   CImageBuffer *pImgDst,  
						   CRect *prcDst,
						   long* plBlobPixelAddress,
						   long* plBlobPixelNo) 
{	
						   
	int x,y;
	int nImgPitchSrc, nImgPitchDst;
	int nImgPitchMean, nImgPitchMsk;
	int nWidthSrc, nHeightSrc;
	BYTE *pRowSrc, *pColSrc, *pImgDstPtr;
	BYTE *pRowMean, *pColMean, *pRowMsk, *pColMsk;
	BYTE byteThreshold;
	long lBlobPixelNo;
	long lDstRowIndex;
	long lDstIndex;

	*plBlobPixelNo = 0;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}

	if (!pImgMean) {
		ASSERT(!"pImgMean is a bad Pointer");
		return -1;
	}
	if (!pImgMean->IsAllocated()) {
		ASSERT(!"pImgMean is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMean, &(pImgMean->GetImgSize()))) {
		ASSERT(!"prcMean is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst, &(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if (prcSrc->Size() != prcMsk->Size()) {
		ASSERT(!"prectSrc Size different from prcMsk Size");
		return -1;
	}

	if (prcSrc->Size() != prcMean->Size()) {
		ASSERT(!"prectSrc Size different from prcMean Size");
		return -1;
	}

	if (prcSrc->Size() != prcDst->Size()) {
		ASSERT(!"prectSrc Size different from prectDst Size");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();
	nImgPitchMean = pImgMean->GetPitch();;
	nImgPitchMsk = pImgMsk->GetPitch();;

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowMean = pColMean = pImgMean->GetImgPtr() + nImgPitchMean * prcMean->top + prcMean->left;
	pImgDstPtr = pImgDst->GetImgPtr();
	pRowMsk = pColMsk = pImgMsk->GetImgPtr() + nImgPitchMsk * prcMsk->top + prcMsk->left;
	lDstRowIndex = prcDst->top * nImgPitchDst + prcDst->left;
	lBlobPixelNo = 0;
 	if(nBlobType == WHITE) {
		BYTE byte255SubContrast;
		byte255SubContrast = 255 - byteContrast;
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, lDstRowIndex += nImgPitchDst, pRowMean += nImgPitchMean, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, lDstIndex = lDstRowIndex, pColMean = pRowMean, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, lDstIndex++, pColMean++, pColMsk++) {
				if(*pColMsk) {
					if(*pColMean < byte255SubContrast) 
						byteThreshold = *pColMean + byteContrast;
					else 
						byteThreshold = 255;
					if (*pColSrc >= byteThreshold) {
						pImgDstPtr[lDstIndex] = 255; 
						plBlobPixelAddress[lBlobPixelNo++] = lDstIndex;
					}
					else
						pImgDstPtr[lDstIndex] = 0;
				}
				else
					pImgDstPtr[lDstIndex] = 0; 
			}
		}
	}
	else {
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, lDstRowIndex += nImgPitchDst, pRowMean += nImgPitchMean, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, lDstIndex = lDstRowIndex, pColMean = pRowMean, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, lDstIndex++, pColMean++, pColMsk++) {
				if(*pColMsk) {
					if(*pColMean > byteContrast)
						byteThreshold = *pColMean - byteContrast;
					else
						byteThreshold = 0;
					if (*pColSrc <= byteThreshold) {
						pImgDstPtr[lDstIndex] = 0; 
						plBlobPixelAddress[lBlobPixelNo++] = lDstIndex;
					}
					else
						pImgDstPtr[lDstIndex] = 255;
				}
				else
					pImgDstPtr[lDstIndex] = 255; 
			}
		}
	}
	*plBlobPixelNo = lBlobPixelNo;
	return 0;
}



int CAdvBinarise::BinariseExt(CImageBuffer *pImgSrc,			// Pointer to Source Image
						   CRect *prcSrc,				// ROI of Source Image
						   int nBlobType,
						   CImageBuffer *pImgMsk, 			// Pointer to Mask Image
						   CRect *prcMsk, 				// ROI of Mask Image
						   CImageBuffer *pImgThreshold,  	// Pointer to Threshold Image
						   CRect *prcThreshold,  		// ROI of Threshold Image
						   CImageBuffer *pImgDst,  			// Pointer to Destination Image
						   CRect *prcDst,				// ROI of Destination Image
						   int* pnAccumulation) {
						   
	int x,y;
	int nImgPitchSrc, nImgPitchDst;
	int nImgPitchThrshold, nImgPitchMsk;
	int nWidthSrc, nHeightSrc;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	BYTE *pRowThreshold, *pColThreshold, *pRowMsk, *pColMsk;
	int nAccumulation;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}

	if (!pImgThreshold) {
		ASSERT(!"pImgThreshold is a bad Pointer");
		return -1;
	}
	if (!pImgThreshold->IsAllocated()) {
		ASSERT(!"pImgThreshold is not Allocated");
		return -1;
	}
	if (ValidateRect(prcThreshold, &(pImgThreshold->GetImgSize()))) {
		ASSERT(!"prcThreshold is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst, &(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if (prcSrc->Size() != prcMsk->Size()) {
		ASSERT(!"prectSrc Size different from prcMsk Size");
		return -1;
	}

	if (prcSrc->Size() != prcThreshold->Size()) {
		ASSERT(!"prectSrc Size different from prcThreshold Size");
		return -1;
	}

	if (prcSrc->Size() != prcDst->Size()) {
		ASSERT(!"prectSrc Size different from prectDst Size");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();
	nImgPitchThrshold = pImgThreshold->GetPitch();;
	nImgPitchMsk = pImgMsk->GetPitch();;

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;
	pRowThreshold = pColThreshold = pImgThreshold->GetImgPtr() + nImgPitchThrshold * prcThreshold->top + prcThreshold->left;
	pRowMsk = pColMsk = pImgMsk->GetImgPtr() + nImgPitchMsk * prcMsk->top + prcMsk->left;

	nAccumulation = 0;
 	if(nBlobType == WHITE) {
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, pRowDst += nImgPitchDst, pRowThreshold += nImgPitchThrshold, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, pColDst = pRowDst, pColThreshold = pRowThreshold, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, pColDst++, pColThreshold++, pColMsk++) {
				if(*pColMsk) {
					if (*pColSrc < *pColThreshold)
						*pColDst = 0; 
					else {
						*pColDst = 255;
						nAccumulation++;
					}
				}
				else
					*pColDst = 0; 
			}
		}
	}
	else {
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, pRowDst += nImgPitchDst, pRowThreshold += nImgPitchThrshold, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, pColDst = pRowDst, pColThreshold = pRowThreshold, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, pColDst++, pColThreshold++, pColMsk++) {
				if(*pColMsk) {
					if (*pColSrc < *pColThreshold) {
						*pColDst = 0; 
						nAccumulation++;
					}
					else
						*pColDst = 255;
				}
				else
					*pColDst = 255; 
			}
		}
	}
	*pnAccumulation = nAccumulation;
	return 0;
}


int CAdvBinarise::GetAccPixelCount(CImageBuffer *pImgSrc,		
						   CRect *prcSrc,	
						   int nBlobType,
						   CImageBuffer *pImgMsk,
						   CRect *prcMsk, 	
						   CImageBuffer *pImgMean,
						   CRect *prcMean,
						   BYTE byteContrast,
						   long* plAccPixelCount) 
{	
						   
	int x,y;
	int nImgPitchSrc;
	int nImgPitchMean, nImgPitchMsk;
	int nWidthSrc, nHeightSrc;
	BYTE *pRowSrc, *pColSrc;
	BYTE *pRowMean, *pColMean, *pRowMsk, *pColMsk;
	BYTE byteThreshold;
	long lAccPixelCount;

	*plAccPixelCount = 0;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}

	if (!pImgMean) {
		ASSERT(!"pImgMean is a bad Pointer");
		return -1;
	}
	if (!pImgMean->IsAllocated()) {
		ASSERT(!"pImgMean is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMean, &(pImgMean->GetImgSize()))) {
		ASSERT(!"prcMean is Invalid");
		return -1;
	}

	if (prcSrc->Size() != prcMsk->Size()) {
		ASSERT(!"prectSrc Size different from prcMsk Size");
		return -1;
	}

	if (prcSrc->Size() != prcMean->Size()) {
		ASSERT(!"prectSrc Size different from prcMean Size");
		return -1;
	}


	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchMean = pImgMean->GetPitch();;
	nImgPitchMsk = pImgMsk->GetPitch();;

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowMean = pColMean = pImgMean->GetImgPtr() + nImgPitchMean * prcMean->top + prcMean->left;
	pRowMsk = pColMsk = pImgMsk->GetImgPtr() + nImgPitchMsk * prcMsk->top + prcMsk->left;

	lAccPixelCount = 0;
 	if(nBlobType == WHITE) {
		BYTE byte255SubContrast;
		byte255SubContrast = 255 - byteContrast;
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, pRowMean += nImgPitchMean, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, pColMean = pRowMean, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, pColMean++, pColMsk++) {
				if(*pColMsk) {
					if(*pColMean < byte255SubContrast) 
						byteThreshold = *pColMean + byteContrast;
					else
						byteThreshold = 255;
					if (*pColSrc >= byteThreshold)
						lAccPixelCount++;
				}
			}
		}
	}
	else {
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc,  pRowMean += nImgPitchMean, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, pColMean = pRowMean, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, pColMean++, pColMsk++) {
				if(*pColMsk) {
					if(*pColMean > byteContrast)
						byteThreshold = *pColMean - byteContrast;
					else
						byteThreshold = 0;
					if (*pColSrc <= byteThreshold)
						lAccPixelCount++;
				}
			}
		}
	}
	*plAccPixelCount = lAccPixelCount;
	return 0;
}


int CAdvBinarise::GetAccPixelCount(CImageBuffer *pImgSrc,			// Pointer to Source Image
								   CRect *prcSrc,				// ROI of Source Image
								   int nBlobType,
								   CImageBuffer *pImgMsk, 			// Pointer to Mask Image
								   CRect *prcMsk, 				// ROI of Mask Image
								   CImageBuffer *pImgThreshold,  	// Pointer to Threshold Image
								   CRect *prcThreshold,  		// ROI of Threshold Image
								   long* plAccPixelCount)
{
						   
	int x,y;
	int nImgPitchSrc;
	int nImgPitchThrshold, nImgPitchMsk;
	int nWidthSrc, nHeightSrc;
	BYTE *pRowSrc, *pColSrc;
	BYTE *pRowThreshold, *pColThreshold, *pRowMsk, *pColMsk;
	long lAccPixelCount;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}

	if (!pImgThreshold) {
		ASSERT(!"pImgThreshold is a bad Pointer");
		return -1;
	}
	if (!pImgThreshold->IsAllocated()) {
		ASSERT(!"pImgThreshold is not Allocated");
		return -1;
	}
	if (ValidateRect(prcThreshold, &(pImgThreshold->GetImgSize()))) {
		ASSERT(!"prcThreshold is Invalid");
		return -1;
	}

	if (prcSrc->Size() != prcMsk->Size()) {
		ASSERT(!"prectSrc Size different from prcMsk Size");
		return -1;
	}

	if (prcSrc->Size() != prcThreshold->Size()) {
		ASSERT(!"prectSrc Size different from prcThreshold Size");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchThrshold = pImgThreshold->GetPitch();;
	nImgPitchMsk = pImgMsk->GetPitch();;

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowThreshold = pColThreshold = pImgThreshold->GetImgPtr() + nImgPitchThrshold * prcThreshold->top + prcThreshold->left;
	pRowMsk = pColMsk = pImgMsk->GetImgPtr() + nImgPitchMsk * prcMsk->top + prcMsk->left;

	lAccPixelCount = 0;
 	if(nBlobType == WHITE) {
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, pRowThreshold += nImgPitchThrshold, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, pColThreshold = pRowThreshold, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, pColThreshold++, pColMsk++) {
				if((*pColMsk) && (*pColSrc > *pColThreshold))
					lAccPixelCount++;
			}
		}
	}
	else {
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, pRowThreshold += nImgPitchThrshold, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, pColThreshold = pRowThreshold, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, pColThreshold++, pColMsk++) {
				if((*pColMsk) && (*pColSrc < *pColThreshold))
					lAccPixelCount++;
			}
		}
	}
	*plAccPixelCount = lAccPixelCount;
	return 0;
}

int CAdvBinarise::FindBandThreshold(CImageBuffer *pImgSrc,		// Pointer to Source Image	
								 CRect *prcSrc,				// ROI of Source Image
								 CImageBuffer *pImgMsk,			// Pointer to Mask Image
								 CRect *prcMsk,				// ROI of Mask Image
								 int nDefectsPercentage,	// The Percentage of White Defect and Black Defect
								 BYTE &byteMinThreshold,	// Threshold to be Used to Mask Black Defect
								 BYTE &byteMaxThreshold)	// Threshold to be Used to Mask White Defect
{
	int nSamplingX = 4;
	int nSamplingY = 4;
	long lAry[256];
	CSize csSampling;
	CRect rcSamplingSrc, rcSamplingMsk;
	CImageBuffer ImgSamplingSrc;
	CImageBuffer ImgSamplingMsk;
	CRect prcSamplingMsk;
	int i;
	int nPeakPos;
	long lPeakThreshold;
	long lSmallerThanPeakNo, lBiggerThanPeakNo;
	long lSmallerThanPeakNoTol, lBiggerThanPeakNoTol;
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}
	if (prcSrc->Size() != prcMsk->Size()) {
		ASSERT(!"prectSrc Size different from prcMsk Size");
		return -1;
	}
	// Bug Fix: nSampling X & Y were added to 1 but removed as the ImgSampling function couldn't take it.
	csSampling = CSize((prcSrc->Width() - 1 ) / nSamplingX,(prcSrc->Height() - 1) / nSamplingY);
	rcSamplingSrc = CRect(CPoint(0, 0), csSampling);
	rcSamplingMsk = rcSamplingSrc;
	ImgSamplingSrc.Allocate(AbsSize(csSampling));
	ImgSamplingMsk.Allocate(AbsSize(csSampling));
	ImgSampling(pImgSrc, prcSrc, &ImgSamplingSrc, &rcSamplingSrc, &CSize(nSamplingX, nSamplingY));
	ImgSampling(pImgMsk, prcMsk, &ImgSamplingMsk, &rcSamplingMsk, &CSize(nSamplingX, nSamplingY));
	AdvHist(&ImgSamplingSrc, &rcSamplingSrc, &ImgSamplingMsk, &rcSamplingMsk, &lAry[0]);
	lPeakThreshold = 0;
	nPeakPos = 0;
	for(i = 0; i < 256; i++) {
		if(lAry[i] > lPeakThreshold) {
			lPeakThreshold = lAry[i];
			nPeakPos = i;
		}
	}

	lSmallerThanPeakNo = 0;
	for(i = 0; i <= nPeakPos;i ++) {
		lSmallerThanPeakNo += lAry[i];
	}

	lSmallerThanPeakNoTol = lSmallerThanPeakNo * nDefectsPercentage / 100;
	lSmallerThanPeakNo = 0;
	for(i = 0; i <= nPeakPos; i++) {
		lSmallerThanPeakNo += lAry[i];
		if(lSmallerThanPeakNo > lSmallerThanPeakNoTol) break;
	}
	byteMinThreshold = i;
	
	lBiggerThanPeakNo = 0;
	for(i = nPeakPos; i < 256; i++) {
		lBiggerThanPeakNo += lAry[i];
	}
	lBiggerThanPeakNoTol = lBiggerThanPeakNo * nDefectsPercentage / 100;
	lBiggerThanPeakNo = 0;
	for(i = 255; i >= nPeakPos; i--) {
		lBiggerThanPeakNo += lAry[i];
		if(lBiggerThanPeakNo > lBiggerThanPeakNoTol) break;
	}
	byteMaxThreshold = i;
	ImgSamplingSrc.Free();
	ImgSamplingMsk.Free();
	return 0;
}



int CAdvBinarise::FindBandThreshold(CImageBuffer *pImgSrc,		// Pointer to Source Image	
								 CRect *prcSrc,				// ROI of Source Image
								 CImageBuffer *pImgMsk,			// Pointer to Mask Image
								 CRect *prcMsk,				// ROI of Mask Image
								 CSize* pcsSampling,
								 int nWhitePercentage,	// The Percentage of White Defect and Black Defect
								 int nBlackPercentage,	// The Percentage of White Defect and Black Defect
								 BYTE &byteBlackCutoffThreshold,	// Threshold to be Used to Mask Black Defect
								 BYTE &byteWhiteCutoffThreshold)	// Threshold to be Used to Mask White Defect
{
	int nSamplingX;
	int nSamplingY;
	long lAry[256];
	CSize csSampling;
	CRect rcSamplingSrc, rcSamplingMsk;
	CImageBuffer ImgSamplingSrc;
	CImageBuffer ImgSamplingMsk;
	CRect prcSamplingMsk;
	int i;
	int nPeakPos;
	long lPeakThreshold;
	long lSmallerThanPeakNo, lBiggerThanPeakNo;
	long lSmallerThanPeakNoTol, lBiggerThanPeakNoTol;
	nSamplingX = pcsSampling->cx;
	if(nSamplingX < 1)	nSamplingX = 1;
	if(prcSrc->Width()/nSamplingX < 10) nSamplingX = prcSrc->Width() / 10;
	if(nSamplingX < 1)	nSamplingX = 1;
	nSamplingY = pcsSampling->cy;
	if(nSamplingY < 1)	nSamplingY = 1;
	if(prcSrc->Height()/nSamplingY < 10) nSamplingY = prcSrc->Height() / 10;
	if(nSamplingY < 1) 	nSamplingY = 1;
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}
	if (prcSrc->Size() != prcMsk->Size()) {
		ASSERT(!"prectSrc Size different from prcMsk Size");
		return -1;
	}
	// Bug Fix: nSampling X & Y were added to 1 but removed as the ImgSampling function couldn't take it.
	csSampling = CSize((prcSrc->Width() - 1 ) / nSamplingX,(prcSrc->Height() - 1) / nSamplingY);
	rcSamplingSrc = CRect(CPoint(0, 0), csSampling);
	rcSamplingMsk = rcSamplingSrc;
	ImgSamplingSrc.Allocate(AbsSize(csSampling));
	ImgSamplingMsk.Allocate(AbsSize(csSampling));
	ImgSampling(pImgSrc, prcSrc, &ImgSamplingSrc, &rcSamplingSrc, &CSize(nSamplingX, nSamplingY));
	ImgSampling(pImgMsk, prcMsk, &ImgSamplingMsk, &rcSamplingMsk, &CSize(nSamplingX, nSamplingY));
	AdvHist(&ImgSamplingSrc, &rcSamplingSrc, &ImgSamplingMsk, &rcSamplingMsk, &lAry[0]);
	lPeakThreshold = 0;
	nPeakPos = 0;
	for(i = 0; i < 256; i++) {
		if(lAry[i] > lPeakThreshold) {
			lPeakThreshold = lAry[i];
			nPeakPos = i;
		}
	}

	lSmallerThanPeakNo = 0;
	for(i = 0; i <= nPeakPos;i ++) {
		lSmallerThanPeakNo += lAry[i];
	}

	lSmallerThanPeakNoTol = lSmallerThanPeakNo * nBlackPercentage / 100;
	lSmallerThanPeakNo = 0;
	for(i = 0; i <= nPeakPos; i++) {
		lSmallerThanPeakNo += lAry[i];
		if(lSmallerThanPeakNo > lSmallerThanPeakNoTol) break;
	}
	byteBlackCutoffThreshold = i;
	
	lBiggerThanPeakNo = 0;
	for(i = nPeakPos; i < 256; i++) {
		lBiggerThanPeakNo += lAry[i];
	}
	lBiggerThanPeakNoTol = lBiggerThanPeakNo * nWhitePercentage / 100;
	lBiggerThanPeakNo = 0;
	for(i = 255; i >= nPeakPos; i--) {
		lBiggerThanPeakNo += lAry[i];
		if(lBiggerThanPeakNo > lBiggerThanPeakNoTol) break;
	}
	byteWhiteCutoffThreshold = i;
	ImgSamplingSrc.Free();
	ImgSamplingMsk.Free();
	return 0;
}


int CAdvBinarise::FindBandThreshold(CImageBuffer *pImgSrc,		// Pointer to Source Image	
								 CRect *prcSrc,				// ROI of Source Image
								 BYTE byteMinSrc,
								 BYTE byteMaxSrc,
								 CImageBuffer *pImgMsk,			// Pointer to Mask Image
								 CRect *prcMsk,				// ROI of Mask Image
								 CSize* pcsSampling,
								 int nWhitePercentage,	// The Percentage of White Defect and Black Defect
								 int nBlackPercentage,	// The Percentage of White Defect and Black Defect
								 BYTE &byteBlackCutoffThreshold,	// Threshold to be Used to Mask Black Defect
								 BYTE &byteWhiteCutoffThreshold)	// Threshold to be Used to Mask White Defect
{
	int nSamplingX;
	int nSamplingY;
	long lAry[256];
	CSize csSampling;
	CRect rcSamplingSrc, rcSamplingMsk;
	CRect prcSamplingMsk;
	int i;
	int nPeakPos;
	long lPeakThreshold;
	long lSmallerThanPeakNo, lBiggerThanPeakNo;
	long lSmallerThanPeakNoTol, lBiggerThanPeakNoTol;
	nSamplingX = pcsSampling->cx;
	if(nSamplingX < 1)	nSamplingX = 1;
	if(prcSrc->Width()/nSamplingX < 10) nSamplingX = prcSrc->Width() / 10;
	if(nSamplingX < 1)	nSamplingX = 1;
	nSamplingY = pcsSampling->cy;
	if(nSamplingY < 1)	nSamplingY = 1;
	if(prcSrc->Height()/nSamplingY < 10) nSamplingY = prcSrc->Height() / 10;
	if(nSamplingY < 1) 	nSamplingY = 1;
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}
	if (prcSrc->Size() != prcMsk->Size()) {
		ASSERT(!"prectSrc Size different from prcMsk Size");
		return -1;
	}
	// Bug Fix: nSampling X & Y were added to 1 but removed as the ImgSampling function couldn't take it.

	AdvHist(pImgSrc,
			prcSrc,
			byteMinSrc,
			byteMaxSrc,
			pImgMsk,
			prcMsk,
			pcsSampling,
			&lAry[0]);
	lPeakThreshold = 0;
	nPeakPos = 0;
	for(i = 0; i < 256; i++) {
		if(lAry[i] > lPeakThreshold) {
			lPeakThreshold = lAry[i];
			nPeakPos = i;
		}
	}

	lSmallerThanPeakNo = 0;
	for(i = 0; i <= nPeakPos;i ++) {
		lSmallerThanPeakNo += lAry[i];
	}

	lSmallerThanPeakNoTol = lSmallerThanPeakNo * nBlackPercentage / 100;
	lSmallerThanPeakNo = 0;
	for(i = 0; i <= nPeakPos; i++) {
		lSmallerThanPeakNo += lAry[i];
		if(lSmallerThanPeakNo > lSmallerThanPeakNoTol) break;
	}
	byteBlackCutoffThreshold = i;
	
	lBiggerThanPeakNo = 0;
	for(i = nPeakPos; i < 256; i++) {
		lBiggerThanPeakNo += lAry[i];
	}
	lBiggerThanPeakNoTol = lBiggerThanPeakNo * nWhitePercentage / 100;
	lBiggerThanPeakNo = 0;
	for(i = 255; i >= nPeakPos; i--) {
		lBiggerThanPeakNo += lAry[i];
		if(lBiggerThanPeakNo > lBiggerThanPeakNoTol) break;
	}
	byteWhiteCutoffThreshold = i;
	return 0;
}

int CAdvBinarise::AdvHist(CImageBuffer *pImgSrc,			// Pointer to Source Image
						  CRect *prcSrc,			// ROI of the Source Image
						  CImageBuffer *pImgMsk,			// Pointer to Mask Image
						  CRect *prcMsk,			// ROI of the Mask Image
						  long *plAry)				// Histogram Data
{
	int x, y;
	int nImgPitchMsk, nImgPitchSrc;
	int nWidth, nHeight;

	BYTE *pRowSrc, *pColSrc;
	BYTE *pRowMsk, *pColMsk;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}
	if (prcSrc->Size() != prcMsk->Size()) {
		ASSERT(!"prectSrc Size different from prcMsk Size");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchMsk = pImgMsk->GetPitch();;

	nWidth = prcSrc->Width();
	nHeight = prcSrc->Height();

	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowMsk = pColMsk = pImgMsk->GetImgPtr() + nImgPitchMsk * prcMsk->top + prcMsk->left;

	memset((void *)plAry, 0, 256 * sizeof(long));   //sizeof(long) = 4
	for (y = 0; y < nHeight; y++, pRowSrc += nImgPitchSrc, pRowMsk += nImgPitchMsk) {
		for (x = 0, pColSrc = pRowSrc, pColMsk = pRowMsk; x < nWidth; x++, pColSrc++, pColMsk++) {
			if(*pColMsk)
				(plAry[*pColSrc])++;
		}
	}
	return 0;
}

int CAdvBinarise::AdvHist(CImageBuffer *pImgSrc,			// Pointer to Source Image
						  CRect *prcSrc,			// ROI of the Source Image
						  CImageBuffer *pImgMsk,			// Pointer to Mask Image
						  CRect *prcMsk,			// ROI of the Mask Image
						  int nMaskColor,
						  long *plAry)				// Histogram Data
{
	int x, y;
	int nImgPitchMsk, nImgPitchSrc;
	int nWidth, nHeight;

	BYTE *pRowSrc, *pColSrc;
	BYTE *pRowMsk, *pColMsk;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}
	if (prcSrc->Size() != prcMsk->Size()) {
		ASSERT(!"prectSrc Size different from prcMsk Size");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchMsk = pImgMsk->GetPitch();;

	nWidth = prcSrc->Width();
	nHeight = prcSrc->Height();

	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowMsk = pColMsk = pImgMsk->GetImgPtr() + nImgPitchMsk * prcMsk->top + prcMsk->left;

	memset((void *)plAry, 0, 256 * sizeof(long));   //sizeof(long) = 4
	for (y = 0; y < nHeight; y++, pRowSrc += nImgPitchSrc, pRowMsk += nImgPitchMsk) {
		for (x = 0, pColSrc = pRowSrc, pColMsk = pRowMsk; x < nWidth; x++, pColSrc++, pColMsk++) {
			if(*pColMsk == nMaskColor)
				(plAry[*pColSrc])++;
		}
	}
	return 0;
}


int CAdvBinarise::AdvHist(CImageBuffer *pImgSrc,			// Pointer to Source Image
						  CRect *prcSrc,			// ROI of the Source Image
						  CImageBuffer *pImgSamplingMsk,	// Pointer to Mask Image
						  CRect *prcSamplingMsk,	// ROI of the Mask Image
						  CSize* pcsSampling,
						  long *plAry)				// Histogram Data
{
	int x, y;
	int nImgPitchMsk, nImgPitchSrc;
	int nImgPitchSrcSampling;
	int nSamplingX, nSamplingY;
	int nHeightSampling, nWidthSampling;

	BYTE *pRowSrc, *pColSrc;
	BYTE *pRowSamplingMsk, *pColSamplingMsk;

	nSamplingX = pcsSampling->cx;
	nSamplingY = pcsSampling->cy;
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	if((nSamplingX <= 0) || (nSamplingY <= 0))
	{
		ASSERT(!"nSamplingX or nSamplingY is Invalid");
		return -1;
	}
	nWidthSampling = prcSrc->Width()/nSamplingX;
	nHeightSampling = prcSrc->Height()/nSamplingY;
	if(pImgSamplingMsk != NULL)
	{
		if (!pImgSamplingMsk) {
			ASSERT(!"pImgSamplingMsk is a bad Pointer");
			return -1;
		}
		if (!pImgSamplingMsk->IsAllocated()) {
			ASSERT(!"pImgSamplingMsk is not Allocated");
			return -1;
		}
		if (ValidateRect(prcSamplingMsk, &(pImgSamplingMsk->GetImgSize()))) {
			ASSERT(!"prcMsk is Invalid");
			return -1;
		}

		nImgPitchSrc = pImgSrc->GetPitch();
		nImgPitchSrcSampling = nImgPitchSrc * nSamplingY;

		nImgPitchMsk = pImgSamplingMsk->GetPitch();

		pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
		pRowSamplingMsk = pColSamplingMsk = pImgSamplingMsk->GetImgPtr() + nImgPitchMsk * prcSamplingMsk->top + prcSamplingMsk->left;

		memset((void *)plAry, 0, 256 * sizeof(long));   //sizeof(long) = 4
		for (y = 0; y < nHeightSampling; y ++, pRowSrc += nImgPitchSrcSampling, pRowSamplingMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, pColSamplingMsk = pRowSamplingMsk; x < nWidthSampling; x ++, pColSrc += nSamplingX, pColSamplingMsk ++) {
				if(*pColSamplingMsk)
					(plAry[*pColSrc])++;
			}
		}
	}
	else
	{
		nImgPitchSrc = pImgSrc->GetPitch();
		nImgPitchSrcSampling = nImgPitchSrc * nSamplingY;
		pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
		memset((void *)plAry, 0, 256 * sizeof(long));   //sizeof(long) = 4
		for (y = 0; y < nHeightSampling; y ++, pRowSrc += nImgPitchSrcSampling) {
			for (x = 0, pColSrc = pRowSrc; x < nWidthSampling; x ++, pColSrc += nSamplingX) {
				(plAry[*pColSrc])++;
			}
		}
	}
	return 0;
}

int CAdvBinarise::AdvHist(CImageBuffer *pImgSrc,			// Pointer to Source Image
						  CRect *prcSrc,			// ROI of the Source Image
						  CSize* pcsSamplingSrc,
						  CImageBuffer *pImgMsk,	// Pointer to Mask Image
						  CRect *prcMsk,	// ROI of the Mask Image
						  CSize* pcsSamplingMsk,
						  long *plAry)				// Histogram Data
{
	int x, y;
	int nImgPitchMsk, nImgPitchSrc;
	int nImgPitchSrcSampling;
	int nSamplingSrcX, nSamplingSrcY;
	int nImgPitchMskSampling;
	int nSamplingMskX, nSamplingMskY;
	int nHeightSampling, nWidthSampling;

	BYTE *pRowSrc, *pColSrc;
	BYTE *pRowMsk, *pColMsk;

	nSamplingSrcX = pcsSamplingSrc->cx;
	nSamplingSrcY = pcsSamplingSrc->cy;
	nSamplingMskX = pcsSamplingMsk->cx;
	nSamplingMskY = pcsSamplingMsk->cy;
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}
	if((pcsSamplingMsk->cx <= 0) || (pcsSamplingMsk->cy<=0)) {
		ASSERT(!"pcsSamplingMsk is Invalid");
		return -1;
	}
	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchSrcSampling = nImgPitchSrc * nSamplingSrcY;

	nImgPitchMsk = pImgMsk->GetPitch();
	nImgPitchMskSampling = nImgPitchMsk * nSamplingMskY;
	nWidthSampling = prcMsk->Width()/pcsSamplingMsk->cx;
	nHeightSampling = prcMsk->Height()/pcsSamplingMsk->cy;

	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowMsk = pColMsk = pImgMsk->GetImgPtr() + nImgPitchMsk * prcMsk->top + prcMsk->left;

	memset((void *)plAry, 0, 256 * sizeof(long));   //sizeof(long) = 4
	for (y = 0; y < nHeightSampling; y ++, pRowSrc += nImgPitchSrcSampling, pRowMsk += nImgPitchMskSampling) {
		for (x = 0, pColSrc = pRowSrc, pColMsk = pRowMsk; x < nWidthSampling; x ++, pColSrc += nSamplingSrcX, pColMsk += nSamplingMskX) {
			if(*pColMsk)
				(plAry[*pColSrc])++;
		}
	}
	return 0;
}

int CAdvBinarise::AdvHist(CImageBuffer *pImgSrc,			// Pointer to Source Image
						  CRect *prcSrc,			// ROI of the Source Image
						  CSize* pcsSamplingSrc,
						  long *plAry)				// Histogram Data
{
	int x, y;
	int nImgPitchSrc;
	int nImgPitchSrcSampling;
	int nSamplingSrcX, nSamplingSrcY;
	int nHeightSampling, nWidthSampling;

	BYTE *pRowSrc, *pColSrc;

	nSamplingSrcX = pcsSamplingSrc->cx;
	nSamplingSrcY = pcsSamplingSrc->cy;
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchSrcSampling = nImgPitchSrc * nSamplingSrcY;

	nWidthSampling = prcSrc->Width() / nSamplingSrcX;
	nHeightSampling = prcSrc->Height() / nSamplingSrcY;

	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;

	memset((void *)plAry, 0, 256 * sizeof(long));   //sizeof(long) = 4
	for (y = 0; y < nHeightSampling; y ++, pRowSrc += nImgPitchSrcSampling) {
		for (x = 0, pColSrc = pRowSrc; x < nWidthSampling; x ++, pColSrc += nSamplingSrcX) {
			(plAry[*pColSrc])++;
		}
	}
	return 0;
}

int CAdvBinarise::AdvHist(CImageBuffer *pImgSrc,			// Pointer to Source Image
						  CRect *prcSrc,			// ROI of the Source Image
						  BYTE byteMinSrc,
						  BYTE byteMaxSrc,
						  CImageBuffer *pImgMsk,	// Pointer to Mask Image
						  CRect *prcMsk,	// ROI of the Mask Image
						  CSize* pcsSampling,
						  long *plAry)				// Histogram Data
{
	int x, y;
	int nImgPitchMsk, nImgPitchSrc;
	int nImgPitchSrcSampling;
	int nImgPitchMskSampling;
	int nSamplingX, nSamplingY;
	int nHeight, nWidth;

	BYTE *pRowSrc, *pColSrc;
	BYTE *pRowMsk, *pColMsk;

	nSamplingX = pcsSampling->cx;
	nSamplingY = pcsSampling->cy;
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}
	if (prcSrc->Size()!=prcMsk->Size()) {
		ASSERT(!"prcSrc Size different from prcMsk Size");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchSrcSampling = nImgPitchSrc * nSamplingY;
	nImgPitchMsk = pImgMsk->GetPitch();
	nImgPitchMskSampling = nImgPitchMsk * nSamplingY;

	nHeight = prcSrc->Height();
	nWidth =  prcSrc->Width();

	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowMsk = pColMsk = pImgMsk->GetImgPtr() + nImgPitchMsk * prcMsk->top + prcMsk->left;

	memset((void *)plAry, 0, 256 * sizeof(long));   //sizeof(long) = 4
	if((byteMinSrc == 0) && (byteMaxSrc == 255)) {
		for (y = 0; y < nHeight; y += nSamplingY, pRowSrc += nImgPitchSrcSampling, pRowMsk += nImgPitchMskSampling) {
			for (x = 0, pColSrc = pRowSrc, pColMsk = pRowMsk; x < nWidth; x += nSamplingX, pColSrc += nSamplingX, pColMsk += nSamplingX) {
				if(*pColMsk) 
					(plAry[*pColSrc])++;
			}
		}
	}
	else {
		for (y = 0; y < nHeight; y += nSamplingY, pRowSrc += nImgPitchSrcSampling, pRowMsk += nImgPitchMskSampling) {
			for (x = 0, pColSrc = pRowSrc, pColMsk = pRowMsk; x < nWidth; x += nSamplingX, pColSrc += nSamplingX, pColMsk += nSamplingX) {
				if(*pColMsk) {
					if((*pColSrc >= byteMinSrc) && (*pColSrc <= byteMaxSrc))
						(plAry[*pColSrc])++;
				}
			}
		}
	}
	return 0;
}


int CAdvBinarise::AdvHist(CImageBuffer* pImgBufSrc,
						 CRectangle2D* prc2DSrc,
						 CSize* pcsSampling,
						 long* plHist)
{
	int i, j;
	long* plTab;
	CRect rcBounding;
	int nRect2DWidth;
	int nRect2DHeight;
	int nRect2DWidthSampling;
	int nRect2DHeightSampling;
	double dScanWidthStepX, dScanWidthStepY;
	double dScanHeightStepX, dScanHeightStepY;
	double dOffsetX, dOffsetY;
	long lPitch;
	BYTE* pImgBufSrcPtr;
	CPoint2D pt2DTopLft;
	double dStartX, dStartY;
	long lIndex;

	rcBounding = prc2DSrc->GetBoundingRect();
	if (!pImgBufSrc) {
		ASSERT(!"pImgBufSrc is a bad Pointer");
		return -1;
	}
	if (!pImgBufSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(&rcBounding,&(pImgBufSrc->GetImgSize()))) {
		ASSERT(!"prc2DSrc is Invalid");
		return -1;
	}
	if((pcsSampling->cx < 1) || ((pcsSampling->cy < 1))) {
		ASSERT(!"pcsSampling is Invalid");
		return -1;
	}

	pImgBufSrcPtr = pImgBufSrc->GetImgPtr();
	lPitch = pImgBufSrc->GetPitch();
	nRect2DWidth = (int) prc2DSrc->GetWidth();
	nRect2DHeight = (int) prc2DSrc->GetHeight();

	dScanWidthStepX = cos(prc2DSrc->GetAngle());
	dScanWidthStepY = sin(prc2DSrc->GetAngle());
	if(dScanWidthStepX < 0) {
		dScanWidthStepX = -dScanWidthStepX;
		dScanWidthStepY = -dScanWidthStepY;
	}
	dScanHeightStepX = -dScanWidthStepY;
	dScanHeightStepY = dScanWidthStepX;
	dScanWidthStepX = dScanWidthStepX * pcsSampling->cx;
	dScanWidthStepY = dScanWidthStepY * pcsSampling->cx;
	dScanHeightStepX = dScanHeightStepX * pcsSampling->cy;
	dScanHeightStepY = dScanHeightStepY * pcsSampling->cy;
	nRect2DWidthSampling = nRect2DWidth / pcsSampling->cx;
	nRect2DHeightSampling = nRect2DHeight /  pcsSampling->cy;
	plTab = new long[abs(nRect2DHeight)];
	dOffsetX = 0;
	dOffsetY = 0;
	for(j = 0; j < nRect2DHeightSampling; j++) {
		plTab[j] = Round(dOffsetY) * lPitch + Round(dOffsetX);
		dOffsetX += dScanHeightStepX;
		dOffsetY += dScanHeightStepY;
	}
	for(i=0;i<256;i++)
		plHist[i] = 0;
	pt2DTopLft = prc2DSrc->GetDevTopLeft();
	dStartX = pt2DTopLft.x;
	dStartY = pt2DTopLft.y;
	for(i = 0; i < nRect2DWidthSampling; i++) {
		lIndex = Round(dStartY)* lPitch + Round(dStartX);	
		for(j = 0; j < nRect2DHeightSampling; j++) {
			plHist[pImgBufSrcPtr[lIndex + plTab[j]]]++;
		}
		dStartX += dScanWidthStepX;
		dStartY += dScanWidthStepY;
	}
	delete [] plTab;
	return 0;
}


int CAdvBinarise::AdvHist(CImageBuffer* pImgBufSrc,
						 CRectangle2D* prc2DSrc,
						 CImageBuffer* pImgMask,
						 CPoint* pptMaskTopLft,
						 CSize* pcsSampling,
						 long* plHist)
{
	int i, j;
	long* plTab;
	long* plMaskTab;
	CRect rcBounding;
	int nRect2DWidth;
	int nRect2DHeight;
	int nRect2DWidthSampling;
	int nRect2DHeightSampling;
	double dScanWidthStepX, dScanWidthStepY;
	double dScanHeightStepX, dScanHeightStepY;
	long lPitch;
	long lPitchMask;
	BYTE* pImgBufSrcPtr;
	BYTE* pImgMaskPtr;
	CPoint2D pt2DTopLft;
	double dOffsetX, dOffsetY;
	double dStartX, dStartY;
	int nImgSrcX, nImgSrcY;
	int nMaskX, nMaskY;
	CRect rcMask;
	int nMaskXOffset, nMaskYOffset;
	long lIndex;
	long lMaskIndex;

	rcBounding = prc2DSrc->GetBoundingRect();
	rcMask = CRect(*pptMaskTopLft, rcBounding.Size());
	nMaskXOffset = pptMaskTopLft->x - rcBounding.left;
	nMaskYOffset = pptMaskTopLft->y - rcBounding.top;
	if (!pImgBufSrc) {
		ASSERT(!"pImgBufSrc is a bad Pointer");
		return -1;
	}
	if (!pImgBufSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(&rcBounding,&(pImgBufSrc->GetImgSize()))) {
		ASSERT(!"prc2DSrc is Invalid");
		return -1;
	}
	if (ValidateRect(&rcMask,&(pImgMask->GetImgSize()))) {
		ASSERT(!"rcMask is Invalid");
		return -1;
	}

	if((pcsSampling->cx < 1) || ((pcsSampling->cy < 1))) {
		ASSERT(!"pcsSampling is Invalid");
		return -1;
	}

	pImgBufSrcPtr = pImgBufSrc->GetImgPtr();
	lPitch = pImgBufSrc->GetPitch();
	pImgMaskPtr = pImgMask->GetImgPtr();
	lPitchMask = pImgMask->GetPitch();
	nRect2DWidth = (int) prc2DSrc->GetWidth();
	nRect2DHeight = (int) prc2DSrc->GetHeight();

	dScanWidthStepX = cos(prc2DSrc->GetAngle());
	dScanWidthStepY = sin(prc2DSrc->GetAngle());
	if(dScanWidthStepX < 0) {
		dScanWidthStepX = -dScanWidthStepX;
		dScanWidthStepY = -dScanWidthStepY;
	}
	dScanHeightStepX = -dScanWidthStepY;
	dScanHeightStepY = dScanWidthStepX;
	dScanWidthStepX = dScanWidthStepX * pcsSampling->cx;
	dScanWidthStepY = dScanWidthStepY * pcsSampling->cx;
	dScanHeightStepX = dScanHeightStepX * pcsSampling->cy;
	dScanHeightStepY = dScanHeightStepY * pcsSampling->cy;
	nRect2DWidthSampling = nRect2DWidth / pcsSampling->cx;
	nRect2DHeightSampling = nRect2DHeight /  pcsSampling->cy;
	plTab = new long[abs(nRect2DHeight)];
	plMaskTab = new long[abs(nRect2DHeight)];
	dOffsetX = 0;
	dOffsetY = 0;
	for(j = 0; j < nRect2DHeightSampling; j++) {
		plTab[j] = Round(dOffsetY) * lPitch + Round(dOffsetX);
		plMaskTab[j] = Round(dOffsetY) * lPitchMask + Round(dOffsetX);
		dOffsetX += dScanHeightStepX;
		dOffsetY += dScanHeightStepY;
	}
	for(i=0;i<256;i++)
		plHist[i] = 0;
	pt2DTopLft = prc2DSrc->GetDevTopLeft();
	dStartX = pt2DTopLft.x;
	dStartY = pt2DTopLft.y;
	for(i = 0; i < nRect2DWidthSampling; i++) {
		nImgSrcX = Round(dStartX);
		nImgSrcY = Round(dStartY);
		nMaskX = nImgSrcX + nMaskXOffset;
		nMaskY = nImgSrcY + nMaskYOffset;
		lIndex = nImgSrcY * lPitch + nImgSrcX;	
		lMaskIndex = nMaskY * lPitchMask + nMaskX;
		for(j = 0; j < nRect2DHeightSampling; j++) {
			if(pImgMaskPtr[lMaskIndex + plMaskTab[j]])
				plHist[pImgBufSrcPtr[lIndex + plTab[j]]]++;
		}
		dStartX += dScanWidthStepX;
		dStartY += dScanWidthStepY;
	}
	delete [] plTab;
	delete [] plMaskTab;
	return 0;
}

int CAdvBinarise::GetThreshold(CImageBuffer* pImgBufSrc,
							 CRectangle2D* prc2D,
							 CImageBuffer* pImgMask,
							 CPoint* pptMaskTopLft,
							 CSize* pcsSampling,
							 double dBlackPercentage,
							 double dWhitePercentage,
							 BYTE* pbyteBlackThresholdCutoff,
							 BYTE* pbyteThresholdBlackAvg,
							 BYTE* pbyteWhiteThresholdCutoff,
							 BYTE* pbyteThresholdWhiteAvg)
{
	long lHistAry[256];
	CHistogram Histogram;
	int nThreshold;
	int i;
	long lWeight, lMoment;
	int nError;


	nError = AdvHist(pImgBufSrc,
					prc2D,
					pImgMask,
					pptMaskTopLft,
					pcsSampling,
					&lHistAry[0]);
	if(nError == -1)
		return -1;
	if(Round(dWhitePercentage) != 0) {
		Histogram.GetWhitePercentageCutoffThreshold(dWhitePercentage,
											  &lHistAry[0],
											  nThreshold);
		*pbyteWhiteThresholdCutoff = nThreshold;
		lWeight = 0;
		lMoment = 0;
		for(i=nThreshold; i<= 255; i++) {
			lWeight += lHistAry[i];
			lMoment += lHistAry[i] * i;
		}
		if(lWeight > 0)
			*pbyteThresholdWhiteAvg = (BYTE)(lMoment/lWeight);
	}
	else {
		for(i=255; i>= 0; i--) {
			if(lHistAry[i]>0)
				break;
		}
		*pbyteWhiteThresholdCutoff = i;
		*pbyteThresholdWhiteAvg = i;
	}
	if(Round(dBlackPercentage) != 0) {
		Histogram.GetBlackPercentageCutoffThreshold(dBlackPercentage,
											  &lHistAry[0],
											  nThreshold);
		*pbyteBlackThresholdCutoff = nThreshold;
		lWeight = 0;
		lMoment = 0;
		for(i=0; i<= nThreshold; i++) {
			lWeight += lHistAry[i];
			lMoment += lHistAry[i] * i;
		}
		if(lWeight > 0)
			*pbyteThresholdBlackAvg = (BYTE)(lMoment/lWeight);
	}
	else {
		for(i=0; i<=255; i++) {
			if(lHistAry[i]>0)
				break;
		}
		*pbyteBlackThresholdCutoff = i;
		*pbyteThresholdBlackAvg = i;
	}
	
	return 0;
}

int CAdvBinarise::GetThreshold(CImageBuffer* pImgBufSrc,
							 CRectangle2D* prc2D,
							 CSize* pcsSampling,
							 double dBlackPercentage,
							 double dWhitePercentage,
							 BYTE* pbyteBlackThresholdCutoff,
							 BYTE* pbyteThresholdBlackAvg,
							 BYTE* pbyteWhiteThresholdCutoff,
							 BYTE* pbyteThresholdWhiteAvg)
{
	long lHistAry[256];
	CHistogram Histogram;
	int nThreshold;
	int i;
	long lWeight, lMoment;
	int nError;

	nError = AdvHist(pImgBufSrc,
					prc2D,
					pcsSampling,
					&lHistAry[0]);
	if(nError == -1)
		return -1;
	if(Round(dWhitePercentage) != 0) {
		Histogram.GetWhitePercentageThreshold(dWhitePercentage,
											  &lHistAry[0],
											  nThreshold);
		*pbyteWhiteThresholdCutoff = nThreshold;
		lWeight = 0;
		lMoment = 0;
		for(i=nThreshold; i<= 255; i++) {
			lWeight += lHistAry[i];
			lMoment += lHistAry[i] * i;
		}
		if(lWeight > 0)
			*pbyteThresholdWhiteAvg = (BYTE)(lMoment/lWeight);
	}
	else {
		for(i=255; i>= 0; i--) {
			if(lHistAry[i]>0)
				break;
		}
		*pbyteWhiteThresholdCutoff = i;
		*pbyteThresholdWhiteAvg = i;
	}
	if(Round(dBlackPercentage) != 0) {
		Histogram.GetBlackPercentageThreshold(dBlackPercentage,
											  &lHistAry[0],
											  nThreshold);
		*pbyteBlackThresholdCutoff = nThreshold;
		lWeight = 0;
		lMoment = 0;
		for(i=0; i<= nThreshold; i++) {
			lWeight += lHistAry[i];
			lMoment += lHistAry[i] * i;
		}
		if(lWeight > 0)
			*pbyteThresholdBlackAvg = (BYTE)(lMoment/lWeight);
	}
	else {
		for(i=0; i<=255; i++) {
			if(lHistAry[i]>0)
				break;
		}
		*pbyteBlackThresholdCutoff = i;
		*pbyteThresholdBlackAvg = i;
	}
	
	return 0;
}

int CAdvBinarise::PkgThreshold(CImageBuffer *pImgSrc,		// Pointer to Source Image
							 CRect *prectSrc,			// ROI of Source Image
							 CImageBuffer *pImgDst,			// Pointer to Destination Image
							 CRect *prectDst,			// ROI of Destination Image
							 int nContrast,				// Input Contrast
							 int nInvert)				// 0 : Subtract Contrast From Source Image
														// 1 : Add Source Image and Contrast
{
	int x, y;
	int nThreshold;
	int nImgPitchSrc, nImgPitchDst;
	int nWidthSrc, nHeightSrc;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prectSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prectSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prectDst, &(pImgDst->GetImgSize()))) {
		ASSERT(!"prectDst is Invalid");
		return -1;
	}

	if (prectSrc->Size() != prectDst->Size()) {
		ASSERT(!"prectSrc Size different from prectDst Size");
		return -1;
	}

	if ((nContrast < 0) && (nContrast >= 256)) {
		ASSERT(!"nContrast Out Of Range");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc = prectSrc->Width();
	nHeightSrc = prectSrc->Height();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prectSrc->top + prectSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prectDst->top + prectDst->left;

	if(nInvert)	{
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, pRowDst += nImgPitchDst) {
			for (x = 0, pColSrc = pRowSrc, pColDst = pRowDst; x < nWidthSrc; x++, pColSrc++, pColDst++) {
				nThreshold = *pColSrc + nContrast;
				if (nThreshold > 255)
					*pColDst = 255;
				else
					*pColDst = nThreshold;
			}
		}
	}
	else {
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, pRowDst += nImgPitchDst) {
			for (x = 0, pColSrc = pRowSrc, pColDst = pRowDst; x < nWidthSrc; x++, pColSrc++, pColDst++) {
				nThreshold = *pColSrc - nContrast;
				if (nThreshold < 0)
					*pColDst = 0;
				else
					*pColDst = nThreshold;
			}
		}
	}
	return 0;
}


#include "stdafx.h"
#include "MarkTool.h"
#include "ProcAlgo.h"
#include "ImageFunction.h"
#include "ExternalFunctions.h"

#include "Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int TmplAdjThres(IMAGEVIEW_INFO *pView,
				 CImageBuffer *pImg,
				 long lAreaCnt,
				 int nIniThres,
				 int nContrastTol,
				 int nMarkColor,
				 CRect rectROI,
				 int *pnAdjThres,
				 long lDebugFlag)
{
	long lMinPix,lMaxPix,lGotAreaCnt;
	long lMaxThres,lAtThres;
	CHistogram Histogram;
	HIST_FIND_PARM HistFindParm;
	int nError;
	int nCounter;
	BOOL bFoundThres,bHitLow,bHitHigh;

	lMinPix = (long)(0.95*lAreaCnt); 
	lMaxPix = (long)(1.05*lAreaCnt);

	HistFindParm.rectROI = rectROI;
	nError = Histogram.GetHistogram(pView,pImg,&HistFindParm,0);
	if (nError) {
		OutputDebugLog("TmplAdjThres()->GetHistogram() Process Error");
		return -1;
	}

	if(lDebugFlag & DEBUG_PRINT) {
		if (nMarkColor==WHITE) {
			lMaxThres = Histogram.m_HistogramRslt.lAcclHistVal[255];
			lAtThres = Histogram.m_HistogramRslt.lAcclHistVal[nIniThres];
			lGotAreaCnt = lMaxThres - lAtThres;
		}
		else {
			lGotAreaCnt = Histogram.m_HistogramRslt.lAcclHistVal[nIniThres];
		}
		OutputDebugLog("TmplAdjThres() (%d %d %d %d)",rectROI.left,rectROI.top,rectROI.right,rectROI.bottom);
		OutputDebugLog("Initial threshold = %d Initial AreaSum = %ld Expected AreaSum = %ld",
						nIniThres,lGotAreaCnt,lAreaCnt);
	}

	nCounter = 0;
	bFoundThres = bHitLow = bHitHigh = FALSE;
	while (!bFoundThres && nCounter<=nContrastTol) {
		if (nMarkColor==WHITE) {
			lMaxThres = Histogram.m_HistogramRslt.lAcclHistVal[255];
			lAtThres = Histogram.m_HistogramRslt.lAcclHistVal[nIniThres];
			lGotAreaCnt = lMaxThres - lAtThres;
			if (lGotAreaCnt < lMinPix) {
				nIniThres--;
				bHitLow = TRUE;
			}
			else if (lGotAreaCnt > lMaxPix) {
				nIniThres++;
				bHitHigh = TRUE;
			}
			else 
				bFoundThres=TRUE;
			if (bHitLow && bHitHigh)
				bFoundThres = TRUE;
			nCounter++;
		}
		else {
			lGotAreaCnt = Histogram.m_HistogramRslt.lAcclHistVal[nIniThres];
			if (lGotAreaCnt < lMinPix) {
				nIniThres++;
				bHitLow = TRUE;
			}
			else if (lGotAreaCnt > lMaxPix) {
				nIniThres--;
				bHitHigh = TRUE;
			}
			else 
				bFoundThres=TRUE;
			if (bHitLow && bHitHigh)
				bFoundThres = TRUE;
			nCounter++;
		}
	}
	if(lDebugFlag & DEBUG_PRINT) {
		OutputDebugLog("Adjusted Thres=%d",nIniThres);
	}
	*pnAdjThres = nIniThres;
	return 0;		
}		

int MinusImg(IMAGEVIEW_INFO *pView,
			 CImageBuffer *pInspImg,
			 CRect *prcInsp,
			 int nThres,
			 CImageBuffer *pRefImg,
			 CRect *prcRef,
			 CImageBuffer *pDstImg,
			 long lDebugFlag)
{
	int i,nError;
	int lut[256];
	CImageBuffer ImgTmp;

	CString strMsg;

	CPerformanceTimer Timer;
	double dTime;
//	int nResponse;
	BOOL bDebugThis;

	///////////////////////////////////////////////////////////////////
	bDebugThis=FALSE;	// Set This to True to Debug Internal Timing
	///////////////////////////////////////////////////////////////////

	if (DEBUG_STEP_MODE&lDebugFlag) {
//		DrwImageHost(pView,pInspImg);
//		DrwUpdate(pView);
//		nResponse = WaitResponse(pView);
//		RestorePrevImg(pView);
//		if(nResponse)
//			goto ErrorUserAbort;
	}

	nError = ImgTmp.Allocate(AbsSize(pView->sizeImage),true);
	if (nError==-1) {
		ASSERT(!"Fail To Allocate Image in MinusImg()");
		return nError;
	}

	if (CheckRect(prcInsp,&pView->sizeImage)){
		OutputDebugLog("InspRect SizeError in MinusImg()");
		goto ErrorAbort;
	}
	if (CheckRect(prcRef,&pView->sizeImage)) {
		OutputDebugLog("RefRect SizeError in MinusImg()");
		goto ErrorAbort;
	}

	for(i=0;i<256;i++) {
		if(i<nThres)
			lut[i] = 50;
		else
			lut[i] = 100;
	}
	nError = ImgConvertLUT(pInspImg,prcInsp,&ImgTmp,prcInsp,&lut[0]);
	if(nError==-1) {
		OutputDebugLog("MinusImg() IP_ConvertLUT() Error");
		goto ErrorAbort;
	}

	if (bDebugThis) 
		Timer.Reset();

	nError = ImgSubtract(pRefImg,prcRef,&ImgTmp,prcInsp,pDstImg,prcInsp);
	if(nError==-1) {
		OutputDebugLog("MinusImg() IP_Sub() Error");
		goto ErrorAbort;
	}

	if (bDebugThis) {
		dTime = Timer.GetElapsedMSec();
		OutputDebugLog("Subtract Image Time=%3.2f",dTime);
	}

	// Display Subtracted Image
	if (DEBUG_STEP_MODE&lDebugFlag) {
//		DrwImageHost(pView,pDstImg);
//		DrwUpdate(pView);
//		strMsg.Format("Subtracted Image");
//		UpdateStatusBar(&strMsg,1);
//		nResponse = WaitResponse(pView);
//		RestorePrevImg(pView);
//		if(nResponse)
//			goto ErrorUserAbort;
	}
	FreeImgBuf(&ImgTmp);

	return 0;

ErrorAbort:
	if (ImgTmp.IsAllocated())
		FreeImgBuf(&ImgTmp);
	DrwDel(pView);
	return -1;
/*
ErrorUserAbort:
	if (ImgTmp.IsAllocated())
		FreeImgBuf(&ImgTmp);
	OutputDebugLog("User Abort in MinusImg()");
	DrwDel(pView);
	return -20;
*/
}

/*int FindExcMisAreaBlob(IMAGEVIEW_INFO *pView,
					   ImgID *pImg,
					   int nMarkColor,
					   double dMissArea,
					   double dExcessArea,
					   double *dFndMissArea,
					   double *dFndExcessArea,
					   CPoint *ptFndMissArea,
					   CPoint *ptFndExcessArea,
					   CRect *prectROI,
					   long lDebugFlag)
{

	ADV_BLOB_FIND_PARM AdvBlobParm;
	CAdvBlobFind BlobFind;
	ImgID TmpID;
	CRect rect;
	int nError;
	LOGPEN RedPen;

	RedPen.lopnColor = RGB(255,0,0);
	RedPen.lopnStyle = PS_SOLID;
	RedPen.lopnWidth.x = 1;

	TmpID = pView->SrcImg;
	pView->SrcImg = *pImg;

	// Find Missing Mark
	if (nMarkColor==WHITE)
		AdvBlobParm.nBlobType = WHITE;
	else
		AdvBlobParm.nBlobType = BLACK;
	AdvBlobParm.nMaxBlobArea = max(prectROI->Width(),prectROI->Height());
	AdvBlobParm.nMaxXBlobSize = 0;
	AdvBlobParm.nMaxYBlobSize = 0;
	if (nMarkColor==WHITE)
		AdvBlobParm.nMinBlobArea = (int)dMissArea;
	else
		AdvBlobParm.nMinBlobArea = (int)dExcessArea;
	AdvBlobParm.nMinXBlobSize = 0;
	AdvBlobParm.nMinYBlobSize =0;
	if (nMarkColor==WHITE)
		AdvBlobParm.nThreshold = 175;
	else 
		AdvBlobParm.nThreshold = 75;
	AdvBlobParm.rcROISrc = prectROI;
	
	nError = BlobFind.GetAdvBlob(pView,&AdvBlobParm,lDebugFlag);
	if (nError) {
		if (nMarkColor==WHITE)
			OutputDebugLog("Fail Missing Mark Check in InspExt()");
		else
			OutputDebugLog("Fail Excess Mark Check in InspExt()");
		pView->SrcImg = TmpID;
		return -1;
	}

	if (BlobFind.m_AdvBlobFindRslt.nFndBlobs>1) {
		if(lDebugFlag&DEBUG_PRINT) {
			if (nMarkColor==WHITE) {
				ptFndMissArea->x = BlobFind.m_AdvBlobFindRslt.AdvBlobInfo[1].rcFndBlobPos.CenterPoint().x;
				ptFndMissArea->y = BlobFind.m_AdvBlobFindRslt.AdvBlobInfo[1].rcFndBlobPos.CenterPoint().y;
				*dFndMissArea = (double)BlobFind.m_AdvBlobFindRslt.AdvBlobInfo[1].nFndBlobArea;
				OutputDebugLog("Mismatch in Blob Find(Missing): XLoc=%d,YLoc=%d, Spec=%3.2f Found=%3.2f",
								ptFndMissArea->x,
								ptFndMissArea->y,
								dMissArea,
								*dFndMissArea);
			}
			else {
				ptFndExcessArea->x = BlobFind.m_AdvBlobFindRslt.AdvBlobInfo[1].rcFndBlobPos.CenterPoint().x;
				ptFndExcessArea->y = BlobFind.m_AdvBlobFindRslt.AdvBlobInfo[1].rcFndBlobPos.CenterPoint().y;
				*dFndExcessArea = (double)BlobFind.m_AdvBlobFindRslt.AdvBlobInfo[1].nFndBlobArea;
				OutputDebugLog("Mismatch in Blob Find(Excess): XLoc=%d,YLoc=%d, Spec=%3.2f Found=%3.2f",
								ptFndExcessArea->x,
								ptFndExcessArea->y,
								dExcessArea,
								*dFndExcessArea);
			}

		}
		if (lDebugFlag&DEBUG_DRAW) {
			rect = BlobFind.m_AdvBlobFindRslt.AdvBlobInfo[1].rcFndBlobPos;
			DrwRect(pView, &rect, &RedPen);
		}
		pView->SrcImg = TmpID;
		return 2;
	}

	// Find Excess Mark
	if (nMarkColor==WHITE)
		AdvBlobParm.nBlobType = BLACK;
	else
		AdvBlobParm.nBlobType = WHITE;
	AdvBlobParm.nMaxBlobArea = max(prectROI->Width(),prectROI->Height());
	AdvBlobParm.nMaxXBlobSize = 0;
	AdvBlobParm.nMaxYBlobSize = 0;
	if (nMarkColor==WHITE)
		AdvBlobParm.nMinBlobArea = (int)dExcessArea;
	else
		AdvBlobParm.nMinBlobArea = (int)dMissArea;
	AdvBlobParm.nMinXBlobSize = 0;
	AdvBlobParm.nMinYBlobSize =0;
	if (nMarkColor==WHITE)
		AdvBlobParm.nThreshold = 75;
	else 
		AdvBlobParm.nThreshold = 175;
	AdvBlobParm.rcROISrc = prectROI;
	
	nError = BlobFind.GetAdvBlob(pView,&AdvBlobParm,lDebugFlag);
	if (nError) {
		if (nMarkColor==WHITE)
			OutputDebugLog("Fail Excess Mark Check in InspExt()");
		else
			OutputDebugLog("Fail Missing Mark Check in InspExt()");
		pView->SrcImg = TmpID;
		return -1;
	}

	if (BlobFind.m_AdvBlobFindRslt.nFndBlobs>1) {
		if(lDebugFlag&DEBUG_PRINT) {
			if (nMarkColor==WHITE) {
				ptFndExcessArea->x = BlobFind.m_AdvBlobFindRslt.AdvBlobInfo[1].rcFndBlobPos.CenterPoint().x;
				ptFndExcessArea->y = BlobFind.m_AdvBlobFindRslt.AdvBlobInfo[1].rcFndBlobPos.CenterPoint().y;
				*dFndExcessArea = (double)BlobFind.m_AdvBlobFindRslt.AdvBlobInfo[1].nFndBlobArea;
				OutputDebugLog("Mismatch in Blob Find(Excess): XLoc=%d,YLoc=%d, Spec=%3.2f Found=%3.2f",
								ptFndExcessArea->x,
								ptFndExcessArea->y,
								dExcessArea,
								*dFndExcessArea);
			}
			else {
				ptFndMissArea->x = BlobFind.m_AdvBlobFindRslt.AdvBlobInfo[1].rcFndBlobPos.CenterPoint().x;
				ptFndMissArea->y = BlobFind.m_AdvBlobFindRslt.AdvBlobInfo[1].rcFndBlobPos.CenterPoint().y;
				*dFndMissArea = (double)BlobFind.m_AdvBlobFindRslt.AdvBlobInfo[1].nFndBlobArea;
				OutputDebugLog("Mismatch in Blob Find(Missing): XLoc=%d,YLoc=%d, Spec=%3.2f Found=%3.2f",
								ptFndMissArea->x,
								ptFndMissArea->y,
								dMissArea,
								*dFndMissArea);
			}
		}
		if (lDebugFlag&DEBUG_DRAW) {
			rect = BlobFind.m_AdvBlobFindRslt.AdvBlobInfo[1].rcFndBlobPos;
			DrwRect(pView, &rect, &RedPen);
		}
		pView->SrcImg = TmpID;
		return 1;
	}
	pView->SrcImg = TmpID;
	return 0;
}
*/

int FindExcMisSqBlb(IMAGEVIEW_INFO *pView,
					CImageBuffer *pImg,
					int nMarkColor,
					double dMissArea,
					double dExcessArea,
					double *pdFndMissArea,
					double *pdFndExcessArea,
					CPoint *pptFndMissArea,
					CPoint *pptFndExcessArea,
					CRect *prectROI,
					long lDebugFlag)
{
	CRect rect;
	int nError;
	LOGPEN RedPen;
	int nThres;
	CString strMsg;

	RedPen.lopnColor = RGB(255,0,0);
	RedPen.lopnStyle = PS_SOLID;
	RedPen.lopnWidth.x = 1;

	CImageBuffer ImgTmp1;
	CImageBuffer *pImgHolder;

	nError = ImgTmp1.Allocate(AbsSize(pImg->GetImgSize()));

	if (nError==-1) {
		ASSERT(!"Fail To Allocate Image in FindExcMisSquareBlob()");
		return nError;
	}
	// Find Missing Mark
	if (nMarkColor==WHITE) {
		nThres = 175;
		nError = BinariseExt(pImg,prectROI,&ImgTmp1,prectROI,nThres,255,0);
	}
	else {
		nThres = 75;
		nError = BinariseExt(pImg,prectROI,&ImgTmp1,prectROI,nThres,255,1);
	}
	if (nError)
		goto ErrorAbort;

	*pdFndMissArea = 0.0;
	*pptFndMissArea = CPoint(0,0);
	pImgHolder = pImg;
	pImg= &ImgTmp1;
	nError = CheckBlob(pView,
					   pImg,
					   prectROI,
					   dMissArea,
					   pdFndMissArea,
					   pptFndMissArea,
					   lDebugFlag);
	pImg = pImgHolder;
	if (nError<0) {
		OutputDebugLog("Error in CheckBlob() in FindExcMisSquareBlob()");
		goto ErrorAbort;
	}
	if (nError>0) {
		if(lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("Mismatch in Blob Find(Missing): XLoc=%d,YLoc=%d, Spec=%3.2f Found=%3.2f",
							pptFndMissArea->x,
							pptFndMissArea->y,
							dMissArea,
							*pdFndMissArea);
		}
		if (lDebugFlag&DEBUG_DRAW) {
			DrwCur(pView, pptFndMissArea, &RedPen,5);
		}

		FreeImgBuf(&ImgTmp1);
		return 2;
	}

	// Find Excess Mark
	if (nMarkColor==WHITE) {
		nThres = 75;
		nError = BinariseExt(pImg,prectROI,&ImgTmp1,prectROI,nThres,255,1);
	}
	else {
		nThres = 175;
		nError = BinariseExt(pImg,prectROI,&ImgTmp1,prectROI,nThres,255,0);
	}
	if (nError)
		goto ErrorAbort;

	*pdFndExcessArea = 0.0;
	*pptFndExcessArea = CPoint(0,0);
	pImgHolder = pImg;
	pImg = &ImgTmp1;
	nError = CheckBlob(pView,
					   pImg,
					   prectROI,
					   dExcessArea,
					   pdFndExcessArea,
					   pptFndExcessArea,
					   lDebugFlag);
	pImg = pImgHolder;
	if (nError<0) {
		OutputDebugLog("Error in CheckBlob() in FindExcMisSquareBlob()");
		goto ErrorAbort;
	}
	if (nError>0) {
		if(lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("Mismatch in Blob Find(Excess): XLoc=%d,YLoc=%d, Spec=%3.2f Found=%3.2f",
							pptFndExcessArea->x,
							pptFndExcessArea->y,
							dExcessArea,
							*pdFndExcessArea);
		}
		if (lDebugFlag&DEBUG_DRAW) {
			DrwCur(pView, pptFndExcessArea, &RedPen,5);
		}
		FreeImgBuf(&ImgTmp1);
		return 1;
	}
	FreeImgBuf(&ImgTmp1);
	return 0;

ErrorAbort:
	FreeImgBuf(&ImgTmp1);
	OutputDebugLog("FindExcMisSqBlb() Error");
	DrwDel(pView);
	return -1;

}

int CheckBlob(IMAGEVIEW_INFO *pView, 
			  CImageBuffer *pImg,
			  CRect *prect, 
			  double dDefectSize, 
			  double *pdFndBlobSize, 
			  CPoint *pptBlobErr, 
			  long lDebugFlag)
{
	double dYSpec,dXSpec;
	int nXSpec,nYSpec;
	long lRowDataArray[1500];
	long lColDataArray[1500];
	long *ptrCAry;
	int nWhitePixels;
	double retcode = 0.0;
	int nError;
	CRect rctmp;

	int nXLen,nYLen;
	int nStore,nCount;
	int i,j;
	int nX,nY;
	int nXLoc,nYLoc;

	//lDebugFlag = lDebugFlag | DEBUG_CHECKBLOB | DEBUG_CHECKAREA;

		
	nXLen = prect->Width();
	nYLen = prect->Height();

	nXSpec = nYSpec = (int)floor(dDefectSize);
	dXSpec = dYSpec = dDefectSize - nXSpec;

	if (nXSpec==0)
		nXSpec = nYSpec = 1;

	if (dXSpec==0.0)
		dXSpec = dYSpec = 0.01;

	if(lDebugFlag & DEBUG_CHECKBLOB)
		OutputDebugLog("Defect Specs->intXSpec=%d, doubleXSpec=%3.1f",nXSpec,dXSpec);

	nError = GetXGrayAccl(pImg,prect,&lColDataArray[0]);
	if (nError) {
		return -1;
	}

	nWhitePixels = 0;
	for (i=0;i<prect->Width();i++) {
		nWhitePixels += (int)(lColDataArray[i])/255; // White is 255 and not 1
	}

	if (lDebugFlag & DEBUG_CHECKBLOB)
		OutputDebugLog("CheckBlob() areacnt=%d\n",nWhitePixels); 
	if(nWhitePixels <= (int)((dXSpec-1) * (dYSpec-1))) 
	{
		if (lDebugFlag & DEBUG_CHECKBLOB)
			OutputDebugLog("No Blobs Found"); 
		return 0;
	}

	ptrCAry = &(lColDataArray[0]);
	nXLoc = prect->left;
	nYLoc = prect->top;

	while (nXLen>2) 
	{
		if (lDebugFlag & DEBUG_CHECKBLOB)
			OutputDebugLog("nXLen=%d",nXLen);
		i = 0;
		nCount = 0;
		nStore = nXLen;

		for (i = 0; i< nXLen; i++) {
			if (lDebugFlag & DEBUG_CHECKBLOB)
				OutputDebugLog("i=%d, nXLen=%d, nYSpec=%d, ptrCAry[%d]=%d",i,nXLen,nYSpec,i,ptrCAry[i]);
			if ((int)(ptrCAry[i])/255 >= nYSpec ) { //White is 255 and not 1
				nCount++;
				if (lDebugFlag & DEBUG_CHECKBLOB)
					OutputDebugLog("Col Val > nYSpec, Inc Col nCount=%d",nCount);
				if ( nCount >= nXSpec )	{
					if (lDebugFlag & DEBUG_CHECKBLOB)
						OutputDebugLog("Col nCount(%d) > nXSpec(%d)",nCount,nXSpec); 
					nStore = i - nXSpec + 1;
					if (nStore<0)
						nStore = 0;
					nCount = 0;
					if ((nXLoc+nStore+nXSpec) > prect->right)
						nStore = prect->right - nXSpec - 1; 

					rctmp = CRect(CPoint(nXLoc+nStore,nYLoc),CSize(nXSpec+1,nYLen));
					
					nError = GetYGrayAccl(pImg,prect,&lRowDataArray[0]);
					if (nError) {
						return -1;
					}

					for ( j = 0; j < nYLen; j++ ) {
						if (lDebugFlag & DEBUG_CHECKBLOB)
							OutputDebugLog("j=%d, nYLen=%d, nXSpec=%d, lRowDataArray[%d]=%d",j,nYLen,nXSpec,j,lRowDataArray[j]);
						if ( (int)(lRowDataArray[j])/255 >= nXSpec ) { //White is 255 and not 1
							nCount++;
							if (lDebugFlag & DEBUG_CHECKBLOB)
								OutputDebugLog("Row Val > nXSpec, Inc Row nCount=%d",nCount);
							if ( nCount >= nYSpec )	{
								if (lDebugFlag & DEBUG_CHECKBLOB)
									OutputDebugLog("Row nCount(%d) > nYSpec(%d)",nCount,nYSpec); 
								nCount = 0;
								nX = nXLoc + i - nXSpec + 1;
								nY = nYLoc + j - nYSpec + 1;
								/* Condition values for check area */
								if (nX<0)
									nX=0;
								if ((nX+nXSpec) > prect->right)
									nX = prect->right - nXSpec - 1; 
								if (nY<0)
									nY=0;
								if ((nY+nYSpec) > prect->bottom)
									nY = prect->right - nYSpec - 1; 
                                
								if (lDebugFlag & DEBUG_CHECKBLOB)
									OutputDebugLog("Do CheckArea()");
								retcode = CheckArea(pView,pImg, prect,nX, nY, nXSpec, nYSpec, dXSpec,lDebugFlag);
								if (retcode<0)
									return -1;
								if (retcode>=dXSpec) {
									*pdFndBlobSize = retcode;
									OutputDebugLog("CheckArea Failed !!! Return Code=%2.2f",retcode);
									*pptBlobErr = CPoint(nX,nY);
									return 1;
								}
								j = j - nCount + 2;  /* backup */
							}
						}
						else {
							if (lDebugFlag & DEBUG_CHECKBLOB)
								OutputDebugLog("Row Val < nXSpec, Reset Row nCount");
							nCount = 0;
						}
					}
				}
			}
			else {
				if (lDebugFlag & DEBUG_CHECKBLOB)
					OutputDebugLog("Col Val < nYSpec, Reset Col nCount");
				nCount = 0;
			}
		}

    	if (nStore==0) { 
				nStore = 1; 
		}
		
		/* Move the pointer to the column accumulation array */
		if (lDebugFlag & DEBUG_CHECKBLOB)
			OutputDebugLog("Change Col Start Array to nStore(%d)",nStore);
		ptrCAry = &(lColDataArray[nStore]);
		nXLoc = nXLoc + nStore;
		nXLen = nXLen - nStore;			
	}
	return 0;
}

double CheckArea(IMAGEVIEW_INFO *pView,
				 CImageBuffer *pImg,
				 CRect *prect,
				 int nX1,
				 int nY1,
				 int nXLen,
				 int nYLen,
				 double dMember,
				 int lDebugFlag)
{
	int i;
	int nFound;
	long lXAry[512];
	CRect rc;
	int nXLimit = prect->right;
	int nYLimit = prect->bottom;
	double dDiff,dArea,dRet;
	int nError;

	if (lDebugFlag & DEBUG_CHECKAREA)
		OutputDebugLog("CheckArea()->SX=%d SY=%d XLen=%d YLen=%d F=%3.2f",nX1,nY1,nXLen,nYLen,dMember);
	if (nX1==0)
		nX1 = 1;
	if (nY1==0)
		nY1 = 1;

	if (nX1+nXLen+2>nXLimit)
		nX1 = nXLimit - nXLen - 2;

	if (nY1+nYLen+2>nYLimit)
		nY1 = nYLimit - nYLen - 2;

	rc = CRect(CPoint(nX1-1,nY1-1),CSize(nXLen+2,nXLen+2));

	nError = GetXGrayAccl(pImg,&rc,&lXAry[0]);
	if (nError)
		return -1;

	nFound = 0;
	for (i=0;i<rc.Width();i++) {
		nFound += (int)(lXAry[i])/255;	// White is 255 and not 1
	}


	dDiff = nXLen + nYLen + 1;
	dArea = nXLen * nYLen + dDiff;

	dRet = (double)nFound;
	if (lDebugFlag & DEBUG_CHECKAREA)
		OutputDebugLog("dDiff=%3.2f dArea=%3.2f dRet=%3.2f",dDiff,dArea,dRet);

	if(dRet > dArea) {
		if (lDebugFlag & DEBUG_CHECKAREA)
			OutputDebugLog("dRet>dArea,dRet set to 0.0");
		dRet = 0.0;
	}
	else {
		dRet = fabs(dRet-dArea)/ dDiff;
		if (lDebugFlag & DEBUG_CHECKAREA)
			OutputDebugLog("dRet(%3.2f)= fabs(dRet-dArea)/ dDiff",dRet);
	}
  
	if(dRet > 1.0) {
		if (lDebugFlag & DEBUG_CHECKAREA)
			OutputDebugLog("dRet>1.0,dRet set to 1.0");
		dRet = 1.0;
	}
  
	dRet = 1.0 - dRet;
	if (lDebugFlag & DEBUG_CHECKAREA)
		OutputDebugLog("CheckArea()->[1.0-dRet], ReturnCode=%3.2f",dRet);
	return(dRet);
}

int MaskTmpl(IMAGEVIEW_INFO *pView,
			 CImageBuffer *pImg,
			 CRect *prect,
			 int nThres,
			 int nFillVal,
			 int nMarkColor)
{
	int i,nError;
	int lut[256];
	CRect rcTmp2;
	CImageBuffer ImgTmp;

	CPerformanceTimer Timer;
	double dTime;
	BOOL bDebugThis;
	///////////////////////////////////////////////////////////////////
	bDebugThis=FALSE;	// Set This to True to Debug Internal Timing
	///////////////////////////////////////////////////////////////////


	if (bDebugThis)
		Timer.Reset();

	if (CheckRect(prect,&pView->sizeImage)) {
		OutputDebugLog("TmplRect SizeError in MaskTmpl()");
		goto ErrorAbort;
	}

	if (prect->Width()>512 || prect->Height()>512) {
		AfxMessageBox("Template is too big to Mask Off, Limit is 256x256 Size");
		goto ErrorAbort;
	}

	nError = AllocImageBuffer(&ImgTmp,pImg->GetImgSize());
	if (nError==-1) {
		ASSERT(!"Fail To Allocate Image in MaskTmpl()");
		return nError;
	}
	if (nMarkColor==WHITE) {
		for(i=0;i<256;i++) {
			if(i<nThres-10)
				lut[i] = i;
			else
				lut[i] = nFillVal;
		}
	}
	else {
		for(i=0;i<256;i++) {
			if(i>=nThres+10)
				lut[i] = i;
			else
				lut[i] = nFillVal;
		}
	}

	rcTmp2 = CRect(CPoint(0,0),CSize(prect->Width(),prect->Height()));
	nError = ImgConvertLUT(pImg,prect,&ImgTmp,&rcTmp2,&lut[0]);
	if(nError==-1) {
		OutputDebugLog("MaskTmpl() ImgConvertLUT() Error");
		goto ErrorAbort;
	}

	nError = ImgCopy(&ImgTmp,&rcTmp2,pImg,prect);
	if(nError==-1) {
		OutputDebugLog("MaskTmpl() Fail ImgCopy() Error");
		goto ErrorAbort;
	}
	FreeImgBuf(&ImgTmp);
	if (bDebugThis) {
		dTime = Timer.GetElapsedMSec();
		OutputDebugLog("MaskTmpl() Time=%3.4f",dTime);
	}
	return 0;

ErrorAbort:
	if (ImgTmp.IsAllocated())
		FreeImgBuf(&ImgTmp);
	DrwDel(pView);
	return -1;

}

/*int	MaskRect(IMAGEVIEW_INFO *pView,
			 CRect *prect,
			 int nFillVal)
{
	int nError;
	
	CPerformanceTimer Timer;
	double dTime;
	BOOL bDebugThis;
	///////////////////////////////////////////////////////////////////
	bDebugThis=FALSE;	// Set This to True to Debug Internal Timing
	///////////////////////////////////////////////////////////////////


	if (bDebugThis)
		Timer.Reset();

	if (CheckRect(prect,&pView->sizeImage)) {
		OutputDebugLog("Rect SizeError in MaskRect()");
		goto ErrorAbort;
	}

	pView->pcsIP5000->Lock(500);
	ActiveIP(pView->uIP);
	nError = SetWindow(SRC0_WIN,prect->left,prect->top,prect->right,prect->bottom);
	nError = SetWindow(DST_WIN,prect->left,prect->top,prect->right,prect->bottom);
	nError = IP_Const(pView->SrcImg,nFillVal);
	pView->pcsIP5000->Unlock();
	if(nError==-1) {
		OutputDebugLog("MaskRect()->IP_Const() Error");
		goto ErrorAbort;
	}
	if (bDebugThis) {
		dTime = Timer.GetElapsedMSec();
		OutputDebugLog("MaskRect() Time=%3.4f",dTime);
	}
	return 0;

ErrorAbort:
	pView->pcsIP5000->Lock(500);
	ActiveIP(pView->uIP);
	ReadIPErrorTable(&ErrorTbl);
	pView->pcsIP5000->Unlock();
	OutputDebugLog("MaskRect() ErrorCode = %d ErrorRoutin = %s",ErrorTbl.ErrorCode,ErrorTbl.ErrorRoutin);
	DrwDel(pView);
	return -1;
}
*/

int ScanForMarkTmpl(IMAGEVIEW_INFO *pView,
					CImageBuffer *pImg,
					CRect *prect,
					int nMarkColor,
					int nMinSize,
					int nThres,
					int nWinExt,
					int nMaxRect,
					int *nFndBlob,
					CRect *rectFnd)
{
	CAdvBlobFind AdvBlobFind;
	ADV_BLOB_FIND_PARM AdvBlobParm;
	int i,j,nError;
	CRect rectA,rectB,rectC;
	CArray <CRect,CRect> rectAry;

	LOGPEN RedPen,BluePen;

	RedPen.lopnColor = RGB(255,0,0);
	RedPen.lopnStyle = PS_SOLID;
	RedPen.lopnWidth.x = 1;
	BluePen.lopnColor = RGB(0,0,255);
	BluePen.lopnStyle = PS_SOLID;
	BluePen.lopnWidth.x = 1;


	AdvBlobParm.nBlobType = nMarkColor;
	AdvBlobParm.nMinBlobArea = 4;
	AdvBlobParm.nMaxBlobArea = 1000000;
	AdvBlobParm.nMaxYBlobSize = 1000000;
	AdvBlobParm.nMinYBlobSize = nMinSize;
	AdvBlobParm.nMaxXBlobSize = 1000000;
	AdvBlobParm.nMinXBlobSize = nMinSize;
	AdvBlobParm.nThreshold = nThres;
	AdvBlobParm.rcROISrc = prect;

	CImageBuffer imgBinBuf;
	int nInvert = 0;
	imgBinBuf.Allocate(AbsSize(pImg->GetImgSize()));
	BinariseExt1(pImg,
				 prect,
				 &imgBinBuf,
				 prect,
				 nThres,
				 nInvert);


	nError = AdvBlobFind.GetAdvBlob3(pView,&imgBinBuf,&AdvBlobParm,0,0);
	if (nError) {
		return -1;
	}

	rectAry.RemoveAll();
	// Copy All Found Rectangles to the Dynamic Array
	for (i=0;i<AdvBlobFind.m_AdvBlobFindRslt.nFndBlobs;i++) {
		rectA = AdvBlobFind.m_AdvBlobFindRslt.pAdvBlobInfo[i].rcFndBlobPos;
		rectAry.Add(&rectA);
	}
	// Merge Rects that are within another Rectangle
	for (i=0;i<rectAry.GetSize();i++) {
		rectA = rectAry[i];
		for (j=i+1;j<rectAry.GetSize();j++) {
			rectB = rectAry[j];
			if (RectInRect(rectA,rectB)) {
				rectC.UnionRect(rectA,rectB);
				rectAry[i] = rectC;
				rectAry.RemoveAt(j);
				i=-1;
				j=-1;
				break;
			}
		}
	}
	if (rectAry.GetSize() > nMaxRect) {
		AfxMessageBox("Found Template Exceed Rect Storage Limit"); 
		return -1;
	}

	*nFndBlob = (int) rectAry.GetSize();
	for (i=0;i<rectAry.GetSize();i++) {
		rectA = rectAry[i];
		rectA.InflateRect(nWinExt,nWinExt);
		CheckRect(&rectA,&pView->sizeImage);
		// Check for Overlapping and do Trimming if necessary
		for (j=0;j<rectAry.GetSize();j++) {
			if (i==j) continue;
			rectB = rectAry[j];
			if (rectC.IntersectRect(rectA,rectB))
				rectA = GetTrimRect(rectA,rectAry[i],rectB,rectC);
		}
		rectFnd[i] = rectA;
	}
	return 0;
}

CRect GetTrimRect(CRect rectInflated,
				  CRect rectOrg,
				  CRect rectCut,
				  CRect rectIntersect)
{
	CPoint ptTL,
		   ptTR,
		   ptBL,
		   ptBR;

	BOOL bTLInRect,
		 bTRInRect,
		 bBLInRect,
		 bBRInRect;

	CRect rcTmp1,rcTmp2,rc;


	bTLInRect=bTRInRect=bBLInRect=bBRInRect=FALSE;

	ptTL = CPoint(rectCut.left,rectCut.top);
	ptTR = CPoint(rectCut.right,rectCut.top);
	ptBL = CPoint(rectCut.left,rectCut.bottom);
	ptBR = CPoint(rectCut.right,rectCut.bottom);

	// Ensure that it does not cut into the original Template Rect
	if (rectOrg.PtInRect(ptTL))
		bTLInRect = TRUE;
	if (rectOrg.PtInRect(ptTR))
		bTRInRect = TRUE;
	if (rectOrg.PtInRect(ptBL))
		bBLInRect = TRUE;
	if (rectOrg.PtInRect(ptBR))
		bBRInRect = TRUE;

	if (bTLInRect||bTRInRect||bBLInRect||bBRInRect) {
		rectOrg.InflateRect(2,2);
		return rectOrg;
	}

	bTLInRect=bTRInRect=bBLInRect=bBRInRect=FALSE;

	if (rectInflated.PtInRect(ptTL))
		bTLInRect = TRUE;
	if (rectInflated.PtInRect(ptTR))
		bTRInRect = TRUE;
	if (rectInflated.PtInRect(ptBL))
		bBLInRect = TRUE;
	if (rectInflated.PtInRect(ptBR))
		bBRInRect = TRUE;

	if (bTLInRect&&bTRInRect&&bBLInRect&&bBRInRect) {
		AfxMessageBox("One Tmpl Rect is inside another Tmpl Rect");
		return rectInflated;
	}

	else if ((bTLInRect&&bTRInRect) || (bBLInRect&&bBRInRect)) {
		rcTmp1 = rectCut;
		rcTmp1.InflateRect(rectInflated.Width(),0,rectInflated.Width(),0);
		rcTmp2 = rectInflated;
		rc.SubtractRect(rcTmp2,rcTmp1);
		return rc;
	}
	else if ((bTLInRect&&bBLInRect) || (bTRInRect&&bBRInRect)) {
		rcTmp1 = rectCut;
		rcTmp1.InflateRect(0,rectInflated.Height(),0,rectInflated.Height());
		rcTmp2 = rectInflated;
		rc.SubtractRect(rcTmp2,rcTmp1);
		return rc;
	}
	else if (bTLInRect) {
		if (rectIntersect.Width() > rectIntersect.Height()) {
			rcTmp1 = rectCut;
			rcTmp1.InflateRect(rectInflated.Width(),0,0,0);
			rcTmp2 = rectInflated;
			rc.SubtractRect(rcTmp2,rcTmp1);
		}
		else {
			rcTmp1 = rectCut;
			rcTmp1.InflateRect(0,rectInflated.Height(),0,0);
			rcTmp2 = rectInflated;
			rc.SubtractRect(rcTmp2,rcTmp1);
		}
		return rc;
	}
	else if (bTRInRect) {
		if (rectIntersect.Width() > rectIntersect.Height()) {
			rcTmp1 = rectCut;
			rcTmp1.InflateRect(0,0,rectInflated.Width(),0);
			rcTmp2 = rectInflated;
			rc.SubtractRect(rcTmp2,rcTmp1);
		}
		else {
			rcTmp1 = rectCut;
			rcTmp1.InflateRect(0,rectInflated.Height(),0,0);
			rcTmp2 = rectInflated;
			rc.SubtractRect(rcTmp2,rcTmp1);
		}
		return rc;
	}
	else if (bBLInRect) {
		if (rectIntersect.Width() > rectIntersect.Height()) {
			rcTmp1 = rectCut;
			rcTmp1.InflateRect(rectInflated.Width(),0,0,0);
			rcTmp2 = rectInflated;
			rc.SubtractRect(rcTmp2,rcTmp1);
		}
		else {
			rcTmp1 = rectCut;
			rcTmp1.InflateRect(0,0,0,rectInflated.Height());
			rcTmp2 = rectInflated;
			rc.SubtractRect(rcTmp2,rcTmp1);
		}
		return rc;
	}
	else if (bBRInRect) {
		if (rectIntersect.Width() > rectIntersect.Height()) {
			rcTmp1 = rectCut;
			rcTmp1.InflateRect(0,0,rectInflated.Width(),0);
			rcTmp2 = rectInflated;
			rc.SubtractRect(rcTmp2,rcTmp1);
		}
		else {
			rcTmp1 = rectCut;
			rcTmp1.InflateRect(0,0,0,rectInflated.Height());
			rcTmp2 = rectInflated;
			rc.SubtractRect(rcTmp2,rcTmp1);
		}
		return rc;
	}
	else {
		rcTmp1 = rectCut;
		rcTmp2 = rectInflated;
		rc.SubtractRect(rcTmp2,rcTmp1);
		return rc;
	}
}

int GetAngleBetween2Rects(IMAGEVIEW_INFO *pView,
						  CRect *rcFirst,
						  CRect *rcLast,
						  double *pdAngle)
{
	CPoint pt1,pt2;
	double dXDist;
	double dYDist;

	pt1 = rcFirst->TopLeft();
	pt2 = rcLast->TopLeft();

	dXDist = pt2.x - pt1.x;
	dYDist = pt2.y - pt1.y;

	*pdAngle = atan2(dYDist,dXDist);
	return 0;
}

int GetPixCnt(IMAGEVIEW_INFO *pView,
			  CImageBuffer *pImg,
	 		  CRect *rect,
			  int nMarkColor,
			  int nThres,
			  long *lPixCount)
{
	int nError;
	long lMax;
	long lAtThres;

	CHistogram Histogram;
	HIST_FIND_PARM HistFindParm;

	HistFindParm.rectROI = *rect;
	nError = Histogram.GetHistogram(pView,pImg,&HistFindParm,0);

	if (nError) {
		return -1;
	}

	if (nMarkColor==WHITE) {
		lMax = Histogram.m_HistogramRslt.lAcclHistVal[255];
		lAtThres = Histogram.m_HistogramRslt.lAcclHistVal[nThres];
		*lPixCount = lMax - lAtThres;
	}
	else {
		*lPixCount = Histogram.m_HistogramRslt.lAcclHistVal[nThres];
	}
	return 0;
}

int GenerateRefImg(IMAGEVIEW_INFO *pView,
				   CImageBuffer *pSrcImg,
				   CImageBuffer *pDstImg,
				   CRect *prectROI,
				   int nThres)
{
	int i,nError;
	int lut[256];
	CRect rcTmp2;

	if (CheckRect(prectROI,&pView->sizeImage)) {
		OutputDebugLog("rectROI SizeError in ConvTchImg()");
		return -1;
	}


	for(i=0;i<256;i++) {
		if(i<nThres)
			lut[i] = 150;
		else
			lut[i] = 250;
	}

	rcTmp2 = CRect(CPoint(0,0),CSize(prectROI->Width(),prectROI->Height()));
	nError = ImgConvertLUT(pSrcImg,prectROI,pDstImg,prectROI,&lut[0]);
    if(nError==-1)
		goto ErrorAbort;

	return 0;

ErrorAbort:
	OutputDebugLog("ConvTchImg() Error");
	DrwDel(pView);
	return -1;

}

CGrayScaleCorr::CGrayScaleCorr()
{
	nSkipScore=30;
}

CGrayScaleCorr::~CGrayScaleCorr()
{
}

int CGrayScaleCorr::GrayTemplateMatching(
					CImageBuffer *pImgTempl,
					CRect *prcImgTempl,
					CImageBuffer *pImgInsp,
					CRect *prcImgInsp,
					int &nMaxCorr)
{
	double dSumTempl, dSumInsp;
	double dSqrSumTempl, dSqrSumInsp, dTimesTemplInsp;
	double dNormalTempl, dNormalInsp, dNormal;
	int i, j, m, n;
	long lIndex1,lIndex2;
	int nCorr;
	BYTE *pbyteImgTempl, *pbyteImgInsp;
	long lWidthTimesHeight;
	int nPitchTempl;
	int nPitchInsp;
	int nDx, nDy;


	CheckRect(prcImgInsp, &pImgInsp->GetImgSize());
	pbyteImgTempl = pImgTempl->GetImgPtr();
	nPitchTempl = pImgTempl->GetPitch();

	pbyteImgInsp = pImgInsp->GetImgPtr();
	nPitchInsp = pImgInsp->GetPitch();

	nDx = prcImgTempl->Width() - prcImgInsp->Width();
	nDy = prcImgTempl->Height() - prcImgInsp->Height();

	lWidthTimesHeight = prcImgTempl->Width() * prcImgTempl->Height();

	dSumTempl = 0;
	dSqrSumTempl = 0;
	lIndex1 = prcImgTempl->top * nPitchTempl + prcImgTempl->left;
	for(i = 0; i < prcImgTempl->Height(); i++) {
		for(j = 0; j < prcImgTempl->Width(); j++) {
			dSumTempl += pbyteImgTempl[lIndex1 + j];
			dSqrSumTempl += pbyteImgTempl[lIndex1 + j] * pbyteImgTempl[lIndex1 + j];
		}
		lIndex1 += nPitchTempl;
	}
	dNormalTempl=dSqrSumTempl-1.0*dSumTempl*dSumTempl / lWidthTimesHeight;

	nMaxCorr=0;
	for(i = 0; i <= nDy; i++) {
		for(j = 0; j <= nDx; j++) {
			dSumInsp = 0;
			dSqrSumInsp = 0;
			dTimesTemplInsp = 0;
			lIndex1 = prcImgTempl->top * nPitchTempl + prcImgTempl->left;
			lIndex2 = (prcImgInsp->top + i) * nPitchInsp + prcImgInsp->left + j;
			for(m = 0; m < prcImgTempl->Height(); m++) {
				for(n = 0; n < prcImgTempl->Width(); n++) {
					dSumInsp += pbyteImgInsp[lIndex2+n];
					dSqrSumInsp += pbyteImgInsp[lIndex2+n] * pbyteImgInsp[lIndex2+n];
					dTimesTemplInsp += pbyteImgTempl[lIndex1+n] * pbyteImgInsp[lIndex2+n];
				}
				lIndex1 += nPitchTempl;
				lIndex2 += nPitchInsp;
			}

			dNormalInsp=dSqrSumInsp-1.0*dSumInsp*dSumInsp/lWidthTimesHeight;
			dNormal=sqrt(dNormalTempl*dNormalInsp);
			if(dNormal > 0)
				nCorr=(int)((dTimesTemplInsp-1.0*dSumTempl*dSumInsp/lWidthTimesHeight)*100/dNormal);
			else {
				nCorr = 0;
				return -1;
			}
			if(nCorr>nMaxCorr) nMaxCorr=nCorr;
		}
	}
	return 0;
}

int CGrayScaleCorr::GetSamplingMinCorrSocre(
					CORR_TEMPLATE *pMarkTemplate,
					int nSamplingStepX,
					int nSamplingStepY,
					int nMinCorrSamplingScore,
					int &nMinCorr)
{
	int m, n;
	int nSamplingTotal;
	int xSize, ySize;
	int i, yOffset, xOffset;
	int nCorr[200];	
	CImageBuffer imgData[200];
	CRect rcSampling;
	CRect rcOffsetSampling;
	CImageBuffer imgDataAvg;
	CSize csSampling;

	xSize = pMarkTemplate->NonSampling.rcOrg.Width() / nSamplingStepX;
	ySize = pMarkTemplate->NonSampling.rcOrg.Height() / nSamplingStepY;

	csSampling = CSize(nSamplingStepX, nSamplingStepY);
	rcSampling = CRect(CPoint(0, 0), CSize(xSize, ySize));

	nSamplingTotal = nSamplingStepX * nSamplingStepY;

	for(i = 0;i < nSamplingTotal; i++) 
		imgData[i].Allocate(AbsSize(rcSampling.Size()));
	imgDataAvg.Allocate(AbsSize(rcSampling.Size()));

	nMinCorr=1000;

	for(m=0;m<nSamplingStepY;m++) {
		for(n=0;n<nSamplingStepX;n++) {
			xSize=(pMarkTemplate->NonSampling.rcOrg.Width()-n-nSamplingStepX)/nSamplingStepX;
			ySize=(pMarkTemplate->NonSampling.rcOrg.Height()-m-nSamplingStepY)/nSamplingStepY;
			rcSampling = CRect(CPoint(0, 0), CSize(xSize,ySize));
			for(yOffset=0;yOffset<nSamplingStepY;yOffset++) {
				for(xOffset=0;xOffset<nSamplingStepX;xOffset++) {
					rcOffsetSampling=CRect(n, m, pMarkTemplate->NonSampling.rCImageBuffer.right, pMarkTemplate->NonSampling.rCImageBuffer.bottom);
					ImgOffsetSampling(&pMarkTemplate->NonSampling.ImgBuf,
									&rcOffsetSampling,
									&imgData[yOffset*nSamplingStepX+xOffset],
									&rcSampling,
									&csSampling,
									&CSize(xOffset,yOffset));				
				}
			}
			ImgAvgSampling(&pMarkTemplate->NonSampling.ImgBuf,
							&pMarkTemplate->NonSampling.rCImageBuffer,
							&imgDataAvg,
							&rcSampling,
							&csSampling);				
			for(i=0;i<nSamplingTotal;i++) 
			{
				GrayTemplateMatching(&imgDataAvg,&rcSampling,&imgData[i],&rcSampling,nCorr[i]);
				if(nCorr[i]<nMinCorr) nMinCorr=nCorr[i];
			}
			if(nMinCorr<nMinCorrSamplingScore) break;
		}
	}

	for(i=0;i<nSamplingTotal;i++) imgData[i].Free();
	imgDataAvg.Free();
	return 0;
}

int CGrayScaleCorr::ImgOffsetSampling(
					CImageBuffer *pImgSrc,
					CRect *prcSrc,
					CImageBuffer *pImgDst,
					CRect *prcDst,	
					CSize *pcsSampling,
					CSize *pcsOffset)				
{
	int x, y;
	int nImgPitchSrc, nImgPitchDst;
	int nWidthSrc, nHeightSrc;
	long lSkipPitch,lSkipY,lSkipX;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	CRect rcTol;

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
	
	if ((pcsSampling->cx<1)||((pcsSampling->cy<1))) {
		ASSERT(!"pcsSampling is Invalid");
		return -1;
	}

	rcTol=CRect(prcSrc->left + pcsOffset->cx,
				prcSrc->top + pcsOffset->cy,
				prcSrc->left + pcsOffset->cx + prcDst->Width() * pcsSampling->cx,
				prcSrc->top + pcsOffset->cy + prcDst->Height() * pcsSampling->cy);

	if (ValidateRect(&rcTol,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"The ImgSize of pImgSource is smaller than required Or The ImgSize of pImgDestination is bigger than required");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();

	lSkipPitch=nImgPitchSrc*pcsSampling->cy;
	lSkipX=pcsSampling->cx;
	lSkipY=pcsSampling->cy;

	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * (prcSrc->top + pcsOffset->cy) + prcSrc->left + pcsOffset->cx;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;

	for (y = 0; y< prcDst->Height(); y++, pRowSrc += lSkipPitch, pRowDst += nImgPitchDst) {
		for (x = 0, pColSrc=pRowSrc, pColDst=pRowDst; x < prcDst->Width(); x++,pColSrc += lSkipX, pColDst++) {
			*pColDst = *pColSrc;
		}
	}
	return 0;
}

void CGrayScaleCorr::GetSamplingStep(
					 CORR_TEMPLATE *pMarkTemplate)
{

	int nWidth, nHeight;
	int i,j;
	long MinDataNo, DataNo;
	int nMinCorr;
	nWidth = pMarkTemplate->NonSampling.rcOrg.Width();
	nHeight = pMarkTemplate->NonSampling.rcOrg.Height();
	pMarkTemplate->nStepX = 1;
	pMarkTemplate->nStepY = 1;
	MinDataNo = nWidth * nHeight;
	for(i = 1; i <= 4; i++) {
		for(j = 1; j <= 4; j++) {
			GetSamplingMinCorrSocre(pMarkTemplate,
									i,
									j,
									nMinCorrSamplingScore,
									nMinCorr);
			if(nMinCorr > nMinCorrSamplingScore) {
				DataNo = (nWidth * nHeight) / ( i * j);
				if(DataNo < MinDataNo) {
					MinDataNo = DataNo;
					pMarkTemplate->nStepX = i;
					pMarkTemplate->nStepY = j;
				}
			}
		}
	}
	
	pMarkTemplate->Sampling.rcOrg = CRect(CPoint(pMarkTemplate->NonSampling.rcOrg.TopLeft()),
											CSize(pMarkTemplate->NonSampling.rcOrg.Width() / pMarkTemplate->nStepX,
											pMarkTemplate->NonSampling.rcOrg.Height() / pMarkTemplate->nStepY));
	pMarkTemplate->Sampling.rCImageBuffer = CRect(CPoint(0,0), pMarkTemplate->Sampling.rcOrg.Size());
	pMarkTemplate->Sampling.ProjX.nSize = pMarkTemplate->Sampling.rCImageBuffer.right;
	pMarkTemplate->Sampling.ProjY.nSize = pMarkTemplate->Sampling.rCImageBuffer.bottom;
}

void CGrayScaleCorr::GetImgProj(
					 CORR_PROJ_PATTERN *pMarkTemplateProj,
					 CORR_PROJ_PATTERN *pMarkInspProj)
{
	int x, y;
	int nWidthTempl, nHeightTempl, nPitchTempl;
	int nWidthInsp, nHeightInsp, nPitchInsp;
	BYTE *pRow, *pCol;
	BYTE *pLostRow, *pAddedRow;
	long *plCrntIdx;
	BYTE *pLostCol;
	BYTE *pAddedCol;
	long *plYAryRow2, *plXAryRow2;
	double dSumProjTempl;
	double dNormalProjTempl;
	BYTE *pbyteInsp;
	int nCorrXSize, nCorrYSize;

	nCorrXSize = pMarkInspProj->csShift.cx;
	nCorrYSize = pMarkInspProj->csShift.cy;

	nWidthTempl = pMarkTemplateProj->rCImageBuffer.right;
	nHeightTempl = pMarkTemplateProj->rCImageBuffer.bottom;
	nPitchTempl = pMarkTemplateProj->ImgBuf.GetPitch();
	dSumProjTempl = pMarkTemplateProj->ProjY.dSumProj;
	dNormalProjTempl = pMarkTemplateProj->ProjY.dNormalProj;
	nWidthInsp = pMarkInspProj->rCImageBuffer.right;
	nHeightInsp = pMarkInspProj->rCImageBuffer.bottom;
	nPitchInsp = pMarkInspProj->ImgBuf.GetPitch();
	pbyteInsp = pMarkInspProj->ImgBuf.GetImgPtr();
	memset((void *)plYAry2, 0 , nHeightInsp * nCorrXSize * sizeof(long));
	pRow = pCol = pbyteInsp;
	for(y = 0, plCrntIdx = plYAry2; y < nHeightInsp; y++, pRow += nPitchInsp, plCrntIdx += nCorrXSize) {
		for(x = 0, pCol = pRow; x < nWidthTempl; x++, pCol++) {
			*plCrntIdx += *pCol;
		}
	}
	pLostRow = pbyteInsp;
	pAddedRow = pbyteInsp + nWidthTempl;
	plYAryRow2 = plYAry2;
	for(y = 0, plYAryRow2 += 1;y < nHeightInsp; y++, pLostRow += nPitchInsp, pAddedRow += nPitchInsp, plYAryRow2 += nCorrXSize) {
		for(x = 1, pLostCol = pLostRow, pAddedCol = pAddedRow, plCrntIdx = plYAryRow2; x < nCorrXSize; x++, plCrntIdx++, pLostCol++, pAddedCol++) {
			*plCrntIdx = *(plCrntIdx-1) + *pAddedCol - *pLostCol;
		}
	}

	memset((void *)plXAry2, 0, nWidthInsp * nCorrYSize * sizeof(long));
	pRow = pCol = pbyteInsp;
	for(y = 0; y < nHeightTempl; y++, pRow += nPitchInsp) {
		for(x = 0, pCol = pRow, plCrntIdx = plXAry2; x < nWidthInsp; x++, plCrntIdx++, pCol++) {
			*plCrntIdx += *pCol;
		}
	}
	pLostRow = pbyteInsp;
	pAddedRow = pbyteInsp + nHeightTempl * nPitchInsp;
	plXAryRow2 = plXAry2 + nWidthInsp;
	plCrntIdx = plXAryRow2;
	for(y = 1; y < nCorrYSize; y++, pLostRow += nPitchInsp, pAddedRow += nPitchInsp, plXAryRow2 += nWidthInsp) {
		for(x = 0, pLostCol = pLostRow, pAddedCol = pAddedRow, plCrntIdx = plXAryRow2; x < nWidthInsp; x++, plCrntIdx++, pLostCol++, pAddedCol++) {
			*plCrntIdx = *(plCrntIdx - nWidthInsp) + *pAddedCol - *pLostCol;
		}
	}
}

int CGrayScaleCorr::ProjXFirstCorr2D(
					CORR_PROJ_PATTERN *pMarkTemplateProj,
					CORR_PROJ_PATTERN *pMarkInspProj,
					CPoint *pptCorPos,
					int &nCorPosNo)
{
	int nOffsetX;
	int x,y;
	int nWidthTempl, nHeightTempl;
	int nWidthInsp, nHeightInsp;
	double dSumProjTempl;
	double dNormalProjTempl;
	double dSumProjInsp;
	double dSumSqrProjInsp;
	double dNormalProjInsp;
	double dNormalProj;
	long lTotalSize;
	long* plProjXTempl;
	int nCorrXSize, nCorrYSize;
	int nCorrX, nCorrY;
	long lProj;
	int lXAryRow2;
	int nMaxCorr;
	long lIndexCorr;
	double dTemplTimesInsp;

	nCorrXSize = pMarkInspProj->csShift.cx;
	nCorrYSize = pMarkInspProj->csShift.cy;

	plProjXTempl = pMarkTemplateProj->ProjX.plProj;
	nWidthTempl = pMarkTemplateProj->rCImageBuffer.right;
	nHeightTempl = pMarkTemplateProj->rCImageBuffer.bottom;
	dSumProjTempl = pMarkTemplateProj->ProjX.dSumProj;
	dNormalProjTempl = pMarkTemplateProj->ProjX.dNormalProj;
	nWidthInsp = pMarkInspProj->rCImageBuffer.right;
	nHeightInsp = pMarkInspProj->rCImageBuffer.bottom;
	lTotalSize = nCorrXSize * nCorrYSize;
	pnScoreX = new int[abs(lTotalSize)];
	pnScoreY = new int[abs(lTotalSize)];
	pnScoreXY = new int[abs(lTotalSize)];
	plYAry2 = new long[abs(nHeightInsp * nCorrXSize)];
	plXAry2 = new long[abs(nWidthInsp * nCorrYSize)];

	GetImgProj(pMarkTemplateProj, pMarkInspProj);
	nMaxCorr = 0;
	nCorNo = 1;
	lXAryRow2 = 0;
	lIndexCorr = 0;
	for (y = 0; y < nCorrYSize; y++, lXAryRow2 = lXAryRow2 + nWidthInsp) {
		for(nOffsetX = 0; nOffsetX < nCorrXSize; nOffsetX++) {
			nCorrX = 0;
			nCorrY = 0;
			dSumProjInsp = 0;
			dSumSqrProjInsp = 0;
			dTemplTimesInsp = 0;
			for(x = 0; x < nWidthTempl; x++) {
				lProj=plXAry2[lXAryRow2+nOffsetX+x];
				dSumProjInsp += lProj;
				dSumSqrProjInsp += (double)lProj * lProj;
				dTemplTimesInsp += (double)lProj * plProjXTempl[x];
			}
			dNormalProjInsp = dSumSqrProjInsp - dSumProjInsp * dSumProjInsp / nWidthTempl;
			dNormalProj = sqrt(dNormalProjTempl * dNormalProjInsp);
			nCorrX =(int)((dTemplTimesInsp - dSumProjTempl * dSumProjInsp / nWidthTempl ) * 100 / dNormalProj);
			if(nCorrX > nSkipScore) {
				nCorrY = ProjYCorrPt(pMarkTemplateProj, pMarkInspProj, CPoint(nOffsetX,y));
			}
			pnScoreX[lIndexCorr + nOffsetX] = nCorrX;
			pnScoreY[lIndexCorr + nOffsetX] = nCorrY;
			pnScoreXY[lIndexCorr + nOffsetX] = nCorrX + nCorrY;
		}
		lIndexCorr += nCorrXSize;
	}
	nMaxCorr = SortingMaxProjCorrPos(pnScoreX, 
									 pnScoreY, 
									 pnScoreXY, 
									 pMarkInspProj->csShift, 
									 pptCorPos, 
									 nCorPosNo);
	delete [] plXAry2;
	delete [] plYAry2;
	delete [] pnScoreX;
	delete [] pnScoreY;
	delete [] pnScoreXY;
	return nMaxCorr;
}

int CGrayScaleCorr::ProjYFirstCorr2D(
					CORR_PROJ_PATTERN *pMarkTemplateProj,
					CORR_PROJ_PATTERN *pMarkInspProj,
					CPoint *pptCorPos,
					int &nCorPosNo)
{
	int nOffsetY;
	int x, y;
	int nWidthTempl, nHeightTempl;
	int nWidthInsp, nHeightInsp;
	double dSumProjTempl;
	double dNormalProjTempl;
	double dSumProjInsp;
	double dSumSqrProjInsp;
	double dNormalProjInsp;
	double dNormalProj;
	long lTotalSize;
	long* plProjYTempl;
	int nCorrXSize, nCorrYSize;
	int nCorrX, nCorrY;
	long lProj;
	int lYAryRow2;
	int nMaxCorr;
	long rowIndex;
	long lIndexCorr;
	double dTemplTimesInsp;

	nCorrXSize = pMarkInspProj->csShift.cx;
	nCorrYSize = pMarkInspProj->csShift.cy;
	plProjYTempl = pMarkTemplateProj->ProjY.plProj;
	nWidthTempl = pMarkTemplateProj->rCImageBuffer.right;
	nHeightTempl = pMarkTemplateProj->rCImageBuffer.bottom;
	nWidthInsp = pMarkInspProj->rCImageBuffer.right;
	nHeightInsp = pMarkInspProj->rCImageBuffer.bottom;
	dSumProjTempl = pMarkTemplateProj->ProjY.dSumProj;
	dNormalProjTempl = pMarkTemplateProj->ProjY.dNormalProj;

	lTotalSize = nCorrXSize * nCorrYSize;
	pnScoreX = new int[abs(lTotalSize)];
	pnScoreY = new int[abs(lTotalSize)];
	pnScoreXY = new int[abs(lTotalSize)];
	plYAry2 = new long[abs(nHeightInsp * nCorrXSize)];
	plXAry2 = new long[abs(nWidthInsp * nCorrYSize)];
	GetImgProj(pMarkTemplateProj, pMarkInspProj);
	nMaxCorr = 0;
	nCorNo = 1;
	for(x = 0; x < nCorrXSize; x++)	{
		lYAryRow2 = 0;
		lIndexCorr = 0;
		for(nOffsetY = 0; nOffsetY < nCorrYSize; nOffsetY++, lYAryRow2 += nCorrXSize) {
			nCorrX = 0;
			nCorrY = 0;
			dSumProjInsp = 0;
			dSumSqrProjInsp = 0;
			dTemplTimesInsp = 0;
			rowIndex = lYAryRow2 + x;
			for(y = 0; y < nHeightTempl; y++) {
				lProj = plYAry2[rowIndex];
				dSumProjInsp += lProj;
				dSumSqrProjInsp += (double)lProj * lProj;
				dTemplTimesInsp += (double)lProj * plProjYTempl[y];
				rowIndex += nCorrXSize;
			}
			dNormalProjInsp = dSumSqrProjInsp - dSumProjInsp * dSumProjInsp / nHeightTempl;
			dNormalProj = sqrt(dNormalProjTempl * dNormalProjInsp);
			nCorrY = (int)((dTemplTimesInsp - dSumProjTempl * dSumProjInsp / nHeightTempl) *100 / dNormalProj);
			if(nCorrY > nSkipScore) {
				nCorrX = ProjXCorrPt(pMarkTemplateProj, pMarkInspProj, CPoint(x,nOffsetY));
			}
			pnScoreX[lIndexCorr+x] = nCorrX;
			pnScoreY[lIndexCorr+x] = nCorrY;
			pnScoreXY[lIndexCorr+x] = nCorrX + nCorrY;
			lIndexCorr += nCorrXSize;
		}
	}
	nMaxCorr = SortingMaxProjCorrPos(pnScoreX, 
									 pnScoreY, 
									 pnScoreXY, 
									 pMarkInspProj->csShift, 
									 pptCorPos, 
									 nCorPosNo);
	delete [] plXAry2;
	delete [] plYAry2;
	delete [] pnScoreX;
	delete [] pnScoreY;
	delete [] pnScoreXY;
	return nMaxCorr;
}

int CGrayScaleCorr::SortingMaxProjCorrPos(
					int *pnScoreX,
					int *pnScoreY,
					int *pnScoreXY,
					CSize csCorr,
					CPoint *pptCorrPos,
					int &nCorNo)
{
	int x;
	int nMaxCorr;
	int nXSize, nYSize, nSize;
	int *pCorPosWeight,*pnIndexArray;
	CPoint *pptCorPosTemp;
	int nMaxCorrX,nMaxCorrY;
	int nMaxCorrXTol,nMaxCorrYTol;

	nCorNo = 0;
	nXSize = csCorr.cx;
	nYSize = csCorr.cy;
	nSize = nXSize * nYSize;
	pCorPosWeight = new int[abs(nSize)];
	pnIndexArray = new int[abs(nSize)];
	pptCorPosTemp = new CPoint[abs(nSize)];
	nMaxCorrXTol = 0;
	nMaxCorrYTol = 0;
	nMaxCorrX = 0;
	nMaxCorrY = 0;
	nMaxCorr = 0;
	for(x = 0; x < nSize; x++) {
		if(pnScoreX[x] > nMaxCorrX) nMaxCorrX = pnScoreX[x];
		if(pnScoreY[x] > nMaxCorrY) nMaxCorrY = pnScoreY[x];
	}
	nMaxCorrXTol = (int)(nMaxCorrX * 0.7);
	nMaxCorrYTol = (int)(nMaxCorrY * 0.7);
	if(nMaxCorrXTol > nMinCorrTol) nMaxCorrXTol = nMinCorrTol;
	if(nMaxCorrYTol > nMinCorrTol) nMaxCorrYTol = nMinCorrTol;
	for(x = 0; x < nSize; x++) {
		if((pnScoreX[x] > nMaxCorrXTol) && (pnScoreY[x] > nMaxCorrYTol)) {
			pptCorPosTemp[nCorNo].x = x % nXSize;
			pptCorPosTemp[nCorNo].y = x / nXSize;
			pCorPosWeight[nCorNo] = pnScoreXY[x];
			nCorNo++;
		}
	}

	bool bAscend = FALSE;
	Sorting(pCorPosWeight, nCorNo, pnIndexArray, bAscend);
	if(nCorNo > 100)
		nCorNo = 100;
	for(x = 0; x < nCorNo; x++) {
		pptCorrPos[x] = pptCorPosTemp[pnIndexArray[x]];
	}	

	nMaxCorr = pCorPosWeight[nCorNo - 1] / 2;
	delete [] pCorPosWeight;
	delete [] pnIndexArray;
	delete [] pptCorPosTemp;
	return nMaxCorr;
}

int CGrayScaleCorr::ProjXCorrPt(
					CORR_PROJ_PATTERN *pMarkTemplateProj,
					CORR_PROJ_PATTERN *pMarkInspProj,
					CPoint ptCorr)
{
	int x;
	int nWidthTempl, nHeightTempl;
	int nWidthInsp, nHeightInsp;
	double dSumProjTempl;
	double dNormalProjTempl;
	double dNormalProj;
	long* plProjTempl;
	long* plProjInsp1;
	int nCorr;
	long lProj;
	double dSumProjInsp;
	double dSumSqrProjInsp;
	double dNormalProjInsp;
	double dTemplTimesInsp;
	long lIndex;

	plProjTempl = pMarkTemplateProj->ProjX.plProj;
	dSumProjTempl = pMarkTemplateProj->ProjX.dSumProj;
	dNormalProjTempl = pMarkTemplateProj->ProjX.dNormalProj;

	nWidthTempl = pMarkTemplateProj->rCImageBuffer.right;
	nHeightTempl = pMarkTemplateProj->rCImageBuffer.bottom;
	nWidthInsp = pMarkInspProj->rCImageBuffer.right;
	nHeightInsp = pMarkInspProj->rCImageBuffer.bottom;
	plProjInsp1 = new long[abs(nWidthTempl)];
	lIndex = ptCorr.y * nWidthInsp + ptCorr.x;
	for(x = 0; x < nWidthTempl; x++) {
		plProjInsp1[x] = plXAry2[lIndex + x];
	}
	dSumProjInsp = 0;
	dSumSqrProjInsp = 0;
	dTemplTimesInsp=0;
	for(x = 0; x < nWidthTempl; x++) {
		lProj = plProjInsp1[x];
		dSumProjInsp += lProj;
		dSumSqrProjInsp += (double)lProj * lProj;
		dTemplTimesInsp += (double)lProj * plProjTempl[x];
	}
	dNormalProjInsp = dSumSqrProjInsp - dSumProjInsp * dSumProjInsp / nWidthTempl;
	dNormalProj = sqrt(dNormalProjTempl * dNormalProjInsp);
	nCorr = (int)((dTemplTimesInsp - dSumProjTempl * dSumProjInsp / nWidthTempl) * 100 / dNormalProj);
	delete [] plProjInsp1;
	return nCorr;
}

int CGrayScaleCorr::ProjYCorrPt(
					CORR_PROJ_PATTERN *pMarkTemplateProj,
					CORR_PROJ_PATTERN *pMarkInspProj,
					CPoint ptCorr)
{
	int y;
	int nWidthTempl, nHeightTempl;
	int nWidthInsp, nHeightInsp;
	double dSumProjTempl;
	double dNormalProjTempl;
	double dNormalProj;
	long* plProjTempl;
	long* plProjInsp1;
	int nCorr;
	long lProj;
	double dSumProjInsp;
	double dSumSqrProjInsp;
	double dNormalProjInsp;
	double dTemplTimesInsp;
	long lIndex;
	int nCorrXSize;

	nCorrXSize = pMarkInspProj->csShift.cx;
	plProjTempl = pMarkTemplateProj->ProjY.plProj;
	dSumProjTempl = pMarkTemplateProj->ProjY.dSumProj;
	dNormalProjTempl = pMarkTemplateProj->ProjY.dNormalProj;

	nWidthTempl = pMarkTemplateProj->rCImageBuffer.right;
	nHeightTempl = pMarkTemplateProj->rCImageBuffer.bottom;
	nWidthInsp = pMarkInspProj->rCImageBuffer.right;
	nHeightInsp = pMarkInspProj->rCImageBuffer.bottom;
	plProjInsp1 = new long[abs(nHeightTempl)];

	lIndex = ptCorr.y * nCorrXSize + ptCorr.x;
	for(y = 0; y < nHeightTempl; y++) {
		plProjInsp1[y] = plYAry2[lIndex];
		lIndex += nCorrXSize;
	}
	dSumProjInsp = 0;
	dSumSqrProjInsp = 0;
	dTemplTimesInsp=0;
	for(y = 0; y < nHeightTempl; y++) {
		lProj = plProjInsp1[y];
		dSumProjInsp += lProj;
		dSumSqrProjInsp += (double)lProj * lProj;
		dTemplTimesInsp += (double)lProj * plProjTempl[y];
	}
	dNormalProjInsp = dSumSqrProjInsp - dSumProjInsp * dSumProjInsp / nHeightTempl;
	dNormalProj = sqrt(dNormalProjTempl * dNormalProjInsp);
	nCorr = (int)((dTemplTimesInsp - dSumProjTempl * dSumProjInsp / nHeightTempl) * 100 / dNormalProj);
	delete [] plProjInsp1;
	return nCorr;
}

void CGrayScaleCorr::ProjCorData(
					 CORR_PROJ_DATA *pProjData)
{
	long *plProj;
	int i;
	double dSumProj,dSumSqrProj;
	plProj=pProjData->plProj;
	dSumProj=0;
	dSumSqrProj=0;
	for(i=0;i<pProjData->nSize;i++)
	{
		dSumProj += plProj[i];
		dSumSqrProj += (double)plProj[i]*plProj[i];
	}
	pProjData->dSumProj=dSumProj;
	pProjData->dSumSqrProj=dSumSqrProj;
	pProjData->dNormalProj=dSumSqrProj-dSumProj*dSumProj/pProjData->nSize;
}

void CGrayScaleCorr::GetProjectionData(
					 CORR_TEMPLATE *pMarkTemplate)
{
	GetXGrayAccl(&pMarkTemplate->Sampling.ImgBuf, 
				 &pMarkTemplate->Sampling.rCImageBuffer, 
				 pMarkTemplate->Sampling.ProjX.plProj);
	GetYGrayAccl(&pMarkTemplate->Sampling.ImgBuf, 
				 &pMarkTemplate->Sampling.rCImageBuffer,
				 pMarkTemplate->Sampling.ProjY.plProj);
	ProjCorData(&pMarkTemplate->Sampling.ProjX);
	ProjCorData(&pMarkTemplate->Sampling.ProjY);
	GetXGrayAccl(&pMarkTemplate->NonSampling.ImgBuf, 
				 &pMarkTemplate->NonSampling.rCImageBuffer,
				 pMarkTemplate->NonSampling.ProjX.plProj);
	GetYGrayAccl(&pMarkTemplate->NonSampling.ImgBuf, 
				 &pMarkTemplate->NonSampling.rCImageBuffer,
				 pMarkTemplate->NonSampling.ProjY.plProj);
	ProjCorData(&pMarkTemplate->NonSampling.ProjX);
	ProjCorData(&pMarkTemplate->NonSampling.ProjY);
}

void CGrayScaleCorr::GetProjDirection(
					 CORR_TEMPLATE *pMarkTemplate)
{
	double SigmaX,SigmaY;
	SigmaX=pMarkTemplate->Sampling.ProjX.dSumSqrProj-pMarkTemplate->Sampling.ProjX.dSumProj*pMarkTemplate->Sampling.ProjX.dSumProj/pMarkTemplate->Sampling.ProjX.nSize;
	SigmaY=pMarkTemplate->Sampling.ProjY.dSumSqrProj-pMarkTemplate->Sampling.ProjY.dSumProj*pMarkTemplate->Sampling.ProjY.dSumProj/pMarkTemplate->Sampling.ProjY.nSize;
	if(SigmaX>SigmaY) pMarkTemplate->nProjDir=0;
	else pMarkTemplate->nProjDir=1;
}

void CGrayScaleCorr::GetCor2DData(
					 CORR_TEMPLATE *pMarkTemplate)
{
	int i, j;
	double dSum;
	double dSumSqr;
	long lIndex;
	int nPitch;
	BYTE *pbyteImg;

	pbyteImg = pMarkTemplate->Sampling.ImgBuf.GetImgPtr();
	nPitch = pMarkTemplate->Sampling.ImgBuf.GetPitch();
	dSum=0;
	dSumSqr=0;
	lIndex = pMarkTemplate->Sampling.rCImageBuffer.top * nPitch + pMarkTemplate->Sampling.rCImageBuffer.left;
	for(i = 0; i < pMarkTemplate->Sampling.rCImageBuffer.Height(); i++) {
		for(j = 0; j < pMarkTemplate->Sampling.rCImageBuffer.Width(); j++) {
			dSum += pbyteImg[lIndex + j];
			dSumSqr += pbyteImg[lIndex + j] * pbyteImg[lIndex + j];
		}
		lIndex += nPitch;
	}
	pMarkTemplate->Sampling.dSum=dSum;
	pMarkTemplate->Sampling.dSumSqr=dSumSqr;
	pMarkTemplate->Sampling.dNormal=dSumSqr-1.0*dSum*dSum/(pMarkTemplate->Sampling.rCImageBuffer.Width() * pMarkTemplate->Sampling.rCImageBuffer.Height());

	pbyteImg = pMarkTemplate->NonSampling.ImgBuf.GetImgPtr();
	nPitch = pMarkTemplate->NonSampling.ImgBuf.GetPitch();
	dSum=0;
	dSumSqr=0;
	lIndex = pMarkTemplate->NonSampling.rCImageBuffer.top * nPitch + pMarkTemplate->NonSampling.rCImageBuffer.left;
	for(i = 0; i < pMarkTemplate->NonSampling.rCImageBuffer.Height(); i++) {
		for(j = 0; j < pMarkTemplate->NonSampling.rCImageBuffer.Width(); j++) {
			dSum += pbyteImg[lIndex + j];
			dSumSqr += pbyteImg[lIndex + j] * pbyteImg[lIndex + j];
		}
		lIndex += nPitch;
	}
	pMarkTemplate->NonSampling.dSum=dSum;
	pMarkTemplate->NonSampling.dSumSqr=dSumSqr;
	pMarkTemplate->NonSampling.dNormal=dSumSqr-1.0*dSum*dSum/(pMarkTemplate->NonSampling.rCImageBuffer.Width() * pMarkTemplate->NonSampling.rCImageBuffer.Height());
}

void CGrayScaleCorr::LocateMarkMem(
					 CORR_TEMPLATE *pMarkTemplate)
{
	pMarkTemplate->NonSampling.ImgBuf.Allocate(AbsSize(pMarkTemplate->NonSampling.rCImageBuffer.Size()));
	pMarkTemplate->NonSampling.ProjX.nSize = pMarkTemplate->NonSampling.rCImageBuffer.right;
	pMarkTemplate->NonSampling.ProjX.plProj = new long[abs(pMarkTemplate->NonSampling.ProjX.nSize)];
	pMarkTemplate->NonSampling.ProjY.nSize = pMarkTemplate->NonSampling.rCImageBuffer.bottom;
	pMarkTemplate->NonSampling.ProjY.plProj = new long[abs(pMarkTemplate->NonSampling.ProjY.nSize)];

	pMarkTemplate->Sampling.ImgBuf.Allocate(AbsSize(pMarkTemplate->Sampling.rCImageBuffer.Size()));
	pMarkTemplate->Sampling.ProjX.nSize = pMarkTemplate->Sampling.rCImageBuffer.right;
	pMarkTemplate->Sampling.ProjX.plProj = new long[abs(pMarkTemplate->Sampling.ProjX.nSize)];
	pMarkTemplate->Sampling.ProjY.nSize = pMarkTemplate->Sampling.rCImageBuffer.bottom;
	pMarkTemplate->Sampling.ProjY.plProj = new long[abs(pMarkTemplate->Sampling.ProjY.nSize)];

}

int CGrayScaleCorr::CreateHostTeachMarkTemplate(
					 CImageBuffer* pImgTempl,
					 CRect* prcTempl,
					 CORR_TEMPLATE *pHostMarkTmpl,
					 int nTemplateSamplingAcptScore) 
{
	if (!pImgTempl) {
		ASSERT(!"pImgTempl is a bad Pointer");
		return -1;
	}
	if (!pImgTempl->IsAllocated()) {
		ASSERT(!"pImgTempl is not Allocated");
		return -1;
	}
	if (ValidateRect(prcTempl,&(pImgTempl->GetImgSize()))) {
		// You are getting assertion for this because the template rect has
		// exceeded the image FOV.
		// Possible causes are when you use a high-res camera to do a teach
		// previously and now you have changed to a lower-res camera.
		// Or fail to allocate camera and thus not able to get the correct
		// image resolution so it uses the image default resolution which
		// could be smaller than the camera's resolution
		ASSERT(!"prcTempl is Invalid");
		return -1;
	}

	pHostMarkTmpl->Clean();
	pHostMarkTmpl->NonSampling.rcOrg = prcTempl;
	pHostMarkTmpl->NonSampling.rCImageBuffer = CRect(CPoint(0,0), prcTempl->Size());
	pHostMarkTmpl->Sampling.rCImageBuffer = CRect(CPoint(0,0), prcTempl->Size());
	nMinCorrSamplingScore=nTemplateSamplingAcptScore;
	LocateMarkMem(pHostMarkTmpl);
	ImgCopy(pImgTempl, 
			prcTempl, 
			&pHostMarkTmpl->NonSampling.ImgBuf, 
			&pHostMarkTmpl->NonSampling.rCImageBuffer);
	GetSamplingStep(pHostMarkTmpl);
	ImgAvgSampling(&pHostMarkTmpl->NonSampling.ImgBuf,
					&pHostMarkTmpl->NonSampling.rCImageBuffer,
					&pHostMarkTmpl->Sampling.ImgBuf,
					&pHostMarkTmpl->Sampling.rCImageBuffer,
					&CSize(pHostMarkTmpl->nStepX, pHostMarkTmpl->nStepY));				
	GetProjectionData(pHostMarkTmpl);
	GetProjDirection(pHostMarkTmpl);
	GetCor2DData(pHostMarkTmpl);
	return 0;
}

int CGrayScaleCorr::Correlate(
					CImageBuffer* pImgInsp, 
					CRect *prcInsp, 
					CORR_TEMPLATE *pMarkTemplate,
					int nAcptScore,
					int nRejectScore,
					int* pnFoundScore,
					CPoint* pptFoundPos)
{
	int i;
	BYTE *pbyteMaskCorrPos;
	int nMaskCorrPosSize;
	BOOL bMarkFound;
	CPoint m_pointTemplateShift;
	int nCorr, nMaxCorr;
	long lIndex;
	int nSkip;
	CRect rcMarkSampling;
	CRect rcMark;
	CPoint pCorMaxPos;
	int nImagePitch;

	if (!pImgInsp) {
		ASSERT(!"pImgInsp is a bad Pointer");
		return -1;
	}
	if (!pImgInsp->IsAllocated()) {
		ASSERT(!"pImgInsp is not Allocated");
		return -1;
	}
	if (ValidateRect(prcInsp,&(pImgInsp->GetImgSize()))) {
		ASSERT(!"prcInsp is Invalid");
		return -1;
	}
	if ((pMarkTemplate->NonSampling.rCImageBuffer.Size().cx > prcInsp->Size().cx) ||
		(pMarkTemplate->NonSampling.rCImageBuffer.Size().cy > prcInsp->Size().cy)) {
		ASSERT(!"prcInsp is Invalid");
		return -1;
	}

	nImagePitch=pImgInsp->GetPitch();
	*pnFoundScore = 0;
	*pptFoundPos = CPoint(0,0);

	ptCorrPos[0]=CPoint(0,0);
	nMinCorrTol=nRejectScore;
	MarkInsp.nStepX = pMarkTemplate->nStepX;
	MarkInsp.nStepY = pMarkTemplate->nStepY;
	csSampling = CSize(MarkInsp.nStepX, MarkInsp.nStepY);
	MarkInsp.NonSampling.rcOrg = prcInsp;
	MarkInsp.NonSampling.rCImageBuffer = CRect(CPoint(0,0), prcInsp->Size());
	MarkInsp.Sampling.rCImageBuffer = CRect(CPoint(0,0), CSize(prcInsp->Size().cx / MarkInsp.nStepX, prcInsp->Size().cy / MarkInsp.nStepY));
	LocateMarkMem(&MarkInsp);
	ImgCopy(pImgInsp, 
			prcInsp, 
			&MarkInsp.NonSampling.ImgBuf, 
			&MarkInsp.NonSampling.rCImageBuffer);
	ImgAvgSampling(pImgInsp, 
				   prcInsp,
				   &MarkInsp.Sampling.ImgBuf, 
				   &MarkInsp.Sampling.rCImageBuffer,
				   &csSampling);
	MarkInsp.NonSampling.csShift=CSize(MarkInsp.NonSampling.rCImageBuffer.right - pMarkTemplate->NonSampling.rCImageBuffer.right,
					MarkInsp.NonSampling.rCImageBuffer.bottom - pMarkTemplate->NonSampling.rCImageBuffer.bottom);
	MarkInsp.Sampling.csShift=CSize(MarkInsp.Sampling.rCImageBuffer.right - pMarkTemplate->Sampling.rCImageBuffer.right,
					MarkInsp.Sampling.rCImageBuffer.bottom - pMarkTemplate->Sampling.rCImageBuffer.bottom);

	nMaskCorrPosSize=MarkInsp.Sampling.csShift.cx * MarkInsp.Sampling.csShift.cy;
	pbyteMaskCorrPos=new BYTE[abs(nMaskCorrPosSize)];

	for(i = 0; i < nMaskCorrPosSize; i++) 
		pbyteMaskCorrPos[i] = 1;

	if(pMarkTemplate->nProjDir==0) {
		nMaxCorr = ProjYFirstCorr2D(&pMarkTemplate->Sampling,
									&MarkInsp.Sampling,
									&ptSamplingCorrPos[0],
									nCorNoSampling);
	}
	else {
		nMaxCorr = ProjXFirstCorr2D(&pMarkTemplate->Sampling,
									&MarkInsp.Sampling,
									&ptSamplingCorrPos[0],
									nCorNoSampling);
	}

	nMaxCorr = 0;
	nCorr = 0;
	nSkip=1;
	bMarkFound=0;
	for(i=0;i<nCorNoSampling;i++) {
		lIndex = ptSamplingCorrPos[i].y * MarkInsp.Sampling.csShift.cx + ptSamplingCorrPos[i].x;
		if(pbyteMaskCorrPos[lIndex]) {
			rcMarkSampling = CRect(ptSamplingCorrPos[i], pMarkTemplate->Sampling.rCImageBuffer.Size());
			GrayTemplateMatching(&pMarkTemplate->Sampling.ImgBuf,
								 &pMarkTemplate->Sampling.rCImageBuffer,
								 &MarkInsp.Sampling.ImgBuf,
								 &rcMarkSampling,
								 nCorr);
			if(nCorr>nMaxCorr) {
				nMaxCorr=nCorr;
				pCorMaxPos=ptSamplingCorrPos[i];
			}
			if(nCorr>nAcptScore) break;
		}
	}
	if(nMaxCorr>0) {
		nCorNo = 1;
		ptCorrPos[0] = pCorMaxPos;
		MatchingPosDetermination(pMarkTemplate,&MarkInsp,ptCorrPos,nCorNo,nMinCorrTol);

		rcMark = CRect(ptCorrPos[0], pMarkTemplate->NonSampling.rCImageBuffer.Size());
		GrayTemplateMatching(&pMarkTemplate->NonSampling.ImgBuf,
							 &pMarkTemplate->NonSampling.rCImageBuffer,
							 &MarkInsp.NonSampling.ImgBuf,
							 &rcMark,
							 nCorr);
		*pnFoundScore = nCorr;
		*pptFoundPos = CPoint(MarkInsp.NonSampling.rcOrg.left + rcMark.left,
			MarkInsp.NonSampling.rcOrg.top + rcMark.top);
	}
	delete [] pbyteMaskCorrPos;
	MarkInsp.Clean();
	return 0;
}

int CGrayScaleCorr::MatchingPosDetermination(
					CORR_TEMPLATE *pMarkTemplate,
					CORR_TEMPLATE *pMarkInsp,
					CPoint *pptCorrPos,
					int &nCorNo,
					int nMinCorrTol)
{
	CORR_TEMPLATE MarkInspTemp;
	CPoint ptTopLeft;
	CSize csCorr;
	CRect rcCorrInsp;
	CRect rcCorrTemp;
	int nMaxCorr;
	int i;
	CPoint pCorPosBak;
	CPoint ptCorrPosBak[100];
	pCorPosBak=pptCorrPos[0];


	MarkInspTemp.nStepX = pMarkInsp->nStepX;
	MarkInspTemp.nStepY = pMarkInsp->nStepY;

	ptTopLeft = CPoint(pMarkInsp->NonSampling.rCImageBuffer.left + (pptCorrPos[0].x - 1) * MarkInspTemp.nStepX,
					pMarkInsp->NonSampling.rCImageBuffer.top + (pptCorrPos[0].y - 1) * MarkInspTemp.nStepY);
	if(ptTopLeft.x < 0) ptTopLeft.x = 0;
	if(ptTopLeft.y < 0) ptTopLeft.y = 0;

	csCorr=CSize(pMarkTemplate->NonSampling.rCImageBuffer.Width() + 2 * MarkInspTemp.nStepX,
					pMarkTemplate->NonSampling.rCImageBuffer.Height() + 2 * MarkInspTemp.nStepY);
	if(csCorr.cx > pMarkInsp->NonSampling.rCImageBuffer.Width()) csCorr.cx = pMarkInsp->NonSampling.rCImageBuffer.Width();
	if(csCorr.cy > pMarkInsp->NonSampling.rCImageBuffer.Height()) csCorr.cy = pMarkInsp->NonSampling.rCImageBuffer.Height();

	if((pMarkInsp->NonSampling.rCImageBuffer.right - ptTopLeft.x) < pMarkTemplate->NonSampling.rCImageBuffer.Width()) {
		nCorNo = 0;
		return -1;
	}
	if((pMarkInsp->NonSampling.rCImageBuffer.bottom - ptTopLeft.y) < pMarkTemplate->NonSampling.rCImageBuffer.Height()) {
		nCorNo = 0;
		return -1;
	}
	if((ptTopLeft.x + csCorr.cx) >= pMarkInsp->NonSampling.rCImageBuffer.right) {
		csCorr.cx = pMarkInsp->NonSampling.rCImageBuffer.right - ptTopLeft.x;
	}
	if((ptTopLeft.y + csCorr.cy) >= pMarkInsp->NonSampling.rCImageBuffer.bottom) {
		csCorr.cy = pMarkInsp->NonSampling.rCImageBuffer.bottom - ptTopLeft.y;
	}
	rcCorrInsp = CRect(ptTopLeft, csCorr);

	rcCorrTemp = CRect(CPoint(0,0), csCorr);

	MarkInspTemp.NonSampling.rCImageBuffer = rcCorrTemp;
	MarkInspTemp.Sampling.rCImageBuffer = CRect(rcCorrTemp.TopLeft(), CSize(rcCorrTemp.Width() / MarkInspTemp.nStepX, rcCorrTemp.Height() / MarkInspTemp.nStepY));
	MarkInspTemp.NonSampling.csShift=CSize(MarkInspTemp.NonSampling.rCImageBuffer.right - pMarkTemplate->NonSampling.rCImageBuffer.right,
					MarkInspTemp.NonSampling.rCImageBuffer.bottom - pMarkTemplate->NonSampling.rCImageBuffer.bottom);

	LocateMarkMem(&MarkInspTemp);

	ImgCopy(&pMarkInsp->NonSampling.ImgBuf, 
			&rcCorrInsp, 
			&MarkInspTemp.NonSampling.ImgBuf,
			&rcCorrTemp);

	if(pMarkTemplate->nProjDir==0) {
		nMaxCorr = ProjYFirstCorr2D(&pMarkTemplate->NonSampling,
									&MarkInspTemp.NonSampling, 
									&ptCorrPosBak[0], 
									nCorNo);
	}
	else {
		nMaxCorr = ProjXFirstCorr2D(&pMarkTemplate->NonSampling,
									&MarkInspTemp.NonSampling,
									&ptCorrPosBak[0],
									nCorNo);
	}

	MarkInspTemp.Clean();
	for(i = 0; i < nCorNo; i++)	{
		pptCorrPos[i] = CPoint((pptCorrPos[i].x - 1) * pMarkInsp->nStepX + ptCorrPosBak[i].x,
			(pptCorrPos[i].y - 1) * pMarkInsp->nStepY + ptCorrPosBak[i].y);
	}
	return nMaxCorr;
}

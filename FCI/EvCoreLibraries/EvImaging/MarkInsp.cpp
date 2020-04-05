#include "StdAfx.h"
#include "MarkInsp.h"
#include "AppDef.h"
#include "EdgeMatching.h"
#include "Imaging.h"
#include "ExternalFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CMarkInspect::CMarkInspect()
{
}

CMarkInspect::~CMarkInspect()
{
}

void CMarkInspect::InitMarkInspParm(MARK_INSP_PARM *pMarkInspParm)
{
	int i;

	pMarkInspParm->nTotalSymSet = 1;
	pMarkInspParm->nTmplSet = 1;
	pMarkInspParm->bEnableMarkInspect = TRUE;
	for (i=0;i<MAX_SYMBOLSET;i++) {
		pMarkInspParm->SymInspParmSet[i].bLogoSeparateParm = TRUE;
		pMarkInspParm->SymInspParmSet[i].bTchWinUserDefine = TRUE;
		pMarkInspParm->SymInspParmSet[i].nMarkColor = WHITE;
		pMarkInspParm->SymInspParmSet[i].nMarkRotation = 5;
		pMarkInspParm->SymInspParmSet[i].nMinCharSize = 10;
		pMarkInspParm->SymInspParmSet[i].nTchRectCnt = 1;
		pMarkInspParm->SymInspParmSet[i].nTmplContrastTol = 20;
		pMarkInspParm->SymInspParmSet[i].nXFirstTmplShiftTol = 25;
		pMarkInspParm->SymInspParmSet[i].nYFirstTmplShiftTol = 25;
		pMarkInspParm->SymInspParmSet[i].nXOtherTmplShiftTol = 10;
		pMarkInspParm->SymInspParmSet[i].nYOtherTmplShiftTol = 10;
		pMarkInspParm->SymInspParmSet[i].bMaskTemplate = FALSE;

		pMarkInspParm->SymInspParmSet[i].m_nMarkMaskDilationSize = 1;

		pMarkInspParm->SymInspParmSet[i].nTmplXSkpFact = 3;
		pMarkInspParm->SymInspParmSet[i].nTmplYSkpFact = 3;
		pMarkInspParm->SymInspParmSet[i].nFastTmplXSkpFact = 8;
		pMarkInspParm->SymInspParmSet[i].nFastTmplYSkpFact = 8;
		pMarkInspParm->SymInspParmSet[i].bEnableFastFirst = FALSE;
		pMarkInspParm->SymInspParmSet[i].bEnableFastLast = FALSE;

		pMarkInspParm->SymInspParmSet[i].LogoPar.bGrossCheck = FALSE;
		pMarkInspParm->SymInspParmSet[i].LogoPar.nAcceptScore = 85;
		pMarkInspParm->SymInspParmSet[i].LogoPar.nRejectScore = 40;
		pMarkInspParm->SymInspParmSet[i].LogoPar.dMinMissingPixel = 5;
		pMarkInspParm->SymInspParmSet[i].LogoPar.dMinExcessPixel = 5;
		pMarkInspParm->SymInspParmSet[i].LogoPar.nMethod = SQUARE_AREA;

		pMarkInspParm->SymInspParmSet[i].TmplPar.bGrossCheck = FALSE;
		pMarkInspParm->SymInspParmSet[i].TmplPar.nAcceptScore = 85;
		pMarkInspParm->SymInspParmSet[i].TmplPar.nRejectScore = 40;
		pMarkInspParm->SymInspParmSet[i].TmplPar.dMinMissingPixel = 5;
		pMarkInspParm->SymInspParmSet[i].TmplPar.dMinExcessPixel = 5;
		pMarkInspParm->SymInspParmSet[i].TmplPar.nMethod = SQUARE_AREA;
	}
}

void CMarkInspect::InitMarkTchInfo(MARK_TCH_INFO *pMarkTchInfo)
{
	int i,j,k;

	pMarkTchInfo->nTaughtMarkSet = 0;
	for (i=0;i<MAX_MARKSET;i++) {
		for (j=0;j<MAX_SYMBOLSET;j++) {
			pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].dTchAngle = 0.0;
			pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nTchThres = 128;
			pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nTotalTaughtTmpl = 0;
			pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nFirstTemplate = 0;
			pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nLastTemplate = 0;
			for (k=0;k<MAX_TCH_WIN;k++) 
				pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].rectTchWin[k] = CRect(20,20,100,100);
			for (k=0;k<MAX_TMPL_PER_MRKSET;k++) {
				pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].lPixelCount = 0;
				pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].rectTeachWin = CRect(10,10,20,20);
			}
		}
	}									
}

int CMarkInspect::InspectMark(IMAGEVIEW_INFO *pView,
							  CImageBuffer *pImgBufSrc,
							  CImageBuffer *pImgDispBufSrc,
							  PKG_LOC_RESULT *pPkgLocRslt,
							  MARK_INSP_PARM *pMarkInspParm,
							  MARK_TCH_INFO *pMarkTchInfo,
							  MARK_INSP_RESULT *pMarkInspResult,
							  CSize* pcsZoomFactor,
							  long lDebugFlag)
{
/*	int i,j,nError;
	CString strMsg;
	CPerformanceTimer Timer;
	double dTime;

	m_strErrorMsg.Format("Pass");

	if (!pMarkInspParm->bEnableMarkInspect)
		return INSP_PASS;


	if(lDebugFlag&DEBUG_TIME_EXT)
		Timer.Reset();

	pMarkInspResult->MarkInspRsltSet[0].SymInspRsltSet[0].TmplInspRslt[0].nFndScore = 0;

	if (pMarkTchInfo->nTaughtMarkSet<=0) {
		return MARK_NOT_TAUGHT;
	}

	//Iterate through the Mark Sets
	for(i=0;i<pMarkInspParm->nTmplSet;i++) {
		if (pMarkTchInfo->MarkTchInfoSet[i].nTaughtSymSet==0)
			continue;

		if(!pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefImg) {
			m_strErrorMsg.Format("No Reference Image!!");
			return PROCESS_ERROR;
		}
		if(!pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefTmpl) {
			m_strErrorMsg.Format("No Template Image!!");
			return PROCESS_ERROR;
		}
		
		if(lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("");
			OutputDebugLog("Inspecting Mark Template Set %d",i);
		}
		//Iterate through the Symbol Sets
		for (j=0;j<pMarkInspParm->nTotalSymSet;j++) {
			if (pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nTotalTaughtTmpl==0)
				continue;

			if(lDebugFlag&DEBUG_PRINT) {
				OutputDebugLog("");
				OutputDebugLog("Inspecting Symbol Set %d",j);
			}
			pMarkInspResult->MarkInspRsltSet[0].SymInspRsltSet[0].TmplInspRslt[0].nFndScore = 0;

			CPerformanceTimer mtime;
			mtime.Reset();
			nError = HostCheckSymbolSet(pView,
									pImgBufSrc,
									pImgDispBufSrc,
								    pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefImg,
								    pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefTmpl,
									pPkgLocRslt,
									&(pMarkInspParm->SymInspParmSet[j]),
									&(pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j]),
									&(pMarkInspResult->MarkInspRsltSet[i].SymInspRsltSet[j]),
									pcsZoomFactor,
									lDebugFlag);
			if(lDebugFlag&DEBUG_TIME_EXT) 
				OutputDebugLog("Host Mark Insp Time = %4.3f", mtime.GetElapsedMSec());
			
			if (nError) {
				strMsg.Format("Error::SymSet:%d",j);
				m_strErrorMsg = strMsg+"|"+m_strSymSetErrMsg;
				return nError;
			}
			// Fails SymbolSet Inspection so Skip other SymbolSet and goto next MarkSet
			if (m_nLastTmplErrorID!=-1) {
				m_strErrorMsg.Format("Fail Inspection on MarkSet:%d SymSet:%d TmplID:%d",
		  							  m_nLastMarkSetErrorID,m_nLastSymSetErrorID,m_nLastTmplErrorID);
				break;
			}
		}
		// Pass Inspection so skip the other remaining UnInspected MarkSets
		if (m_nLastTmplErrorID==-1) {
			break;
		}
	}
	if(lDebugFlag&DEBUG_TIME_EXT) {
		dTime = Timer.GetElapsedMSec();
		OutputDebugLog("Mark Inspect Time=%3.2f",dTime);
	}
	if (nError)
		return nError;

	if (m_nLastTmplErrorID==-1) {
		return INSP_PASS;
	}
	else {
		//OutputDebugLog("Mark Error Code=%d",m_nFailErrorCode);
		switch(m_nFailErrorCode) {
			case (TMPL_PASS) :
				m_strErrorMsg = "Pass";
				return INSP_PASS;
			case (TMPL_FAIL_CORR) :
				m_strErrorMsg = "Fail Correlation";
				return INSP_FAIL_MARK_CORR;
			case (TMPL_FAIL_EXCESS_MARK) :
				m_strErrorMsg = "Fail : Excess Mark";
				return INSP_FAIL_MARK_EXCESS;
			case (TMPL_FAIL_MISSING_MARK) :
				m_strErrorMsg = "Fail : Missing Mark";
				return INSP_FAIL_MARK_MISSING;
			case (TMPL_FAIL_NOTFOUND) :
				m_strErrorMsg = "Fail : Mark Template Not found";
				return INSP_FAIL_MARK_TMPL_FIND;
			default :
				return PROCESS_ERROR;
		}
	}
*/
	return 0;
}




int CMarkInspect::InspectRotMark(IMAGEVIEW_INFO *pView,
							  CImageBuffer *pImgBufSrc,
							  CImageBuffer *pImgDispBufSrc,
							  PKG_LOC_RESULT *pPkgLocRslt,
							  PKG_LOC_RESULT *pPkgLocDispRslt,
							  MARK_INSP_PARM *pMarkInspParm,
							  MARK_TCH_INFO *pMarkTchInfo,
							  MARK_TCH_INFO* pMarkRotTchInfo,
							  int* pnSelectedRotTchInfo,
							  double dMarkRotAngleStep,
							  int nMarkRotTchInfoNo,
							  MARK_INSP_RESULT *pMarkInspResult,
							  CSize* pcsZoomFactor,
							  long lDebugFlag)
{
	int i,j,nError;
	CString strMsg;
	CPerformanceTimer Timer;
	double dTime;
	int nMarkAngleIndex;

try
{
	nMarkAngleIndex = Round(nMarkRotTchInfoNo/2 + pPkgLocRslt->dPkgFoundAngle/dMarkRotAngleStep);
	if(nMarkAngleIndex < 0)
		nMarkAngleIndex = 0;
	if(nMarkAngleIndex >= nMarkRotTchInfoNo)
		nMarkAngleIndex = nMarkRotTchInfoNo - 1;

	*pnSelectedRotTchInfo = nMarkAngleIndex;

	m_strErrorMsg.Format("Pass");

	if (!pMarkInspParm->bEnableMarkInspect)
		return INSP_PASS;


	if(lDebugFlag&DEBUG_TIME_EXT)
		Timer.Reset();

	pMarkInspResult->MarkInspRsltSet[0].SymInspRsltSet[0].TmplInspRslt[0].nFndScore = 0;

	if (pMarkTchInfo->nTaughtMarkSet<=0) {
		return MARK_NOT_TAUGHT;
	}

	//Iterate through the Mark Sets
	for(i=0;i<pMarkInspParm->nTmplSet;i++) {
		if (pMarkTchInfo->MarkTchInfoSet[i].nTaughtSymSet==0)
			continue;

		if(!pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefImg) {
			m_strErrorMsg.Format("No Reference Image!!");
			return PROCESS_ERROR;
		}
		if(!pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefTmpl) {
			m_strErrorMsg.Format("No Template Image!!");
			return PROCESS_ERROR;
		}
		
		if(lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("");
			OutputDebugLog("Inspecting Mark Template Set %d",i);
		}
		//Iterate through the Symbol Sets
		for (j=0;j<pMarkInspParm->nTotalSymSet;j++) {
			if (pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nTotalTaughtTmpl==0)
				continue;

			if(lDebugFlag&DEBUG_PRINT) {
				OutputDebugLog("");
				OutputDebugLog("Inspecting Symbol Set %d",j);
			}
			pMarkInspResult->MarkInspRsltSet[0].SymInspRsltSet[0].TmplInspRslt[0].nFndScore = 0;

			CPerformanceTimer mtime;
			mtime.Reset();

			nError = HostCheckRotSymbolSet(pView,
									pImgBufSrc,
									pImgDispBufSrc,
								    pMarkRotTchInfo[nMarkAngleIndex].MarkTchInfoSet[i].pCImageBufferRefImg,
								    pMarkRotTchInfo[nMarkAngleIndex].MarkTchInfoSet[i].pCImageBufferRefTmpl,
									&pMarkTchInfo->pt2DTchPkgCenter,
									pMarkTchInfo->dTchPkgAngle,
									pPkgLocRslt,
									pPkgLocDispRslt,
									&(pMarkInspParm->SymInspParmSet[j]),
									&(pMarkRotTchInfo[nMarkAngleIndex].MarkTchInfoSet[i].SymTchInfoSet[j]),
									&(pMarkInspResult->MarkInspRsltSet[i].SymInspRsltSet[j]),
									pcsZoomFactor,
									lDebugFlag);
/*
			nError = HostCheckSymbolSet(pView,
									pImgBufSrc,
									pImgDispBufSrc,
								    pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefImg,
								    pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefTmpl,
									pPkgLocRslt,
									&(pMarkInspParm->SymInspParmSet[j]),
									&(pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j]),
									&(pMarkInspResult->MarkInspRsltSet[i].SymInspRsltSet[j]),
									pcsZoomFactor,
									lDebugFlag);
*/
			if(lDebugFlag&DEBUG_TIME_EXT) 
				OutputDebugLog("Host Mark Insp Time = %4.3f", mtime.GetElapsedMSec());
			
			if (nError) {
				strMsg.Format("Error::SymSet:%d",j);
				m_strErrorMsg = strMsg+"|"+m_strSymSetErrMsg;
				return nError;
			}
			// Fails SymbolSet Inspection so Skip other SymbolSet and goto next MarkSet
			if (m_nLastTmplErrorID!=-1) {
				m_strErrorMsg.Format("Fail Inspection on MarkSet:%d SymSet:%d TmplID:%d",
		  							  m_nLastMarkSetErrorID,m_nLastSymSetErrorID,m_nLastTmplErrorID);
				break;
			}
		}
		// Pass Inspection so skip the other remaining UnInspected MarkSets
		if (m_nLastTmplErrorID==-1) {
			break;
		}
	}
	if(lDebugFlag&DEBUG_TIME_EXT) {
		dTime = Timer.GetElapsedMSec();
		OutputDebugLog("Mark Inspect Time=%3.2f",dTime);
	}
	if (nError)
		return nError;

	if (m_nLastTmplErrorID==-1) {
		return INSP_PASS;
	}
	else {
		//OutputDebugLog("Mark Error Code=%d",m_nFailErrorCode);
		switch(m_nFailErrorCode) {
			case (TMPL_PASS) :
				m_strErrorMsg = "Pass";
				return INSP_PASS;
			case (TMPL_FAIL_CORR) :
				m_strErrorMsg = "Fail Correlation";
				return INSP_FAIL_MARK_CORR;
			case (TMPL_FAIL_EXCESS_MARK) :
				m_strErrorMsg = "Fail : Excess Mark";
				return INSP_FAIL_MARK_EXCESS;
			case (TMPL_FAIL_MISSING_MARK) :
				m_strErrorMsg = "Fail : Missing Mark";
				return INSP_FAIL_MARK_MISSING;
			case (TMPL_FAIL_NOTFOUND) :
				m_strErrorMsg = "Fail : Mark Template Not found";
				return INSP_FAIL_MARK_TMPL_FIND;
			default :
				return PROCESS_ERROR;
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::InspectRotMark");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMarkInspect::HostCheckRotSymbolSet(IMAGEVIEW_INFO *pView,
									 CImageBuffer *pImgBufSrc,
									 CImageBuffer *pImgDispBufSrc,
								     CImageBuffer *pRefImgBufH,
								     CImageBuffer *pRefTmplBufH,
									 CPoint2D* ppt2DTchPkgCenter,
									 double dTchPkgAngle,
									 PKG_LOC_RESULT *pPkgLocRslt,
									 PKG_LOC_RESULT *pPkgLocDispRslt,
									 SYM_INSP_PARM_SET *pSymParm,
									 SYM_TCH_INFO_SET *pSymTch,
									 SYM_INSP_RSLT_SET *pSymRslt,
									 CSize* pcsZoomFactor,
									 long lDebugFlag)
{
	CPoint ptPkgOffset,
		   ptTmplTopLft,
		   ptPkgAngleOffset;

	CPoint2D pt2DPkgOffset;

	CPoint2D pt2DOld,
			 pt2DNew,
			 pt2DRef;
	
	int i,nError;
	CRect rectTmplDisp;
	CRect rectTmpl,
		  rectSearch;
	BOOL bDebugThis;
	CPerformanceTimer Timer;
	double dTime;
	//int nMedian;
	CRectangle2D rc2DTmpl;
	CPoint2D pt2DTmplCenter;
	CPoint2D pt2DPkgCenter;
	CRectangle2D rc2DMarkRot;
	CPoint ptLastTmplOffset;
	CPoint ptAvgTmplOffset;

	LOGPEN GreenPen;

	CPoint2D pt2DPkgCenterDisp;
	CPoint2D pt2DPkgOffsetDisp;
	double dPkgAngleDisp;
	double dPkgRotAngleDisp;
	CRectangle2D rc2DMarkRotDisp;

try
{
	GreenPen.lopnColor = RGB(0,255,0);
	GreenPen.lopnStyle = PS_SOLID;
	GreenPen.lopnWidth.x = 1;

	m_strSymSetErrMsg.Empty();
	m_nLastTmplErrorID = -1;
	m_nFailErrorCode = 0;


	////////////////////////////////////////////////////////////////////
	if (lDebugFlag&DEBUG_TIME)
		bDebugThis=TRUE;	//Enable This To Debug Template Inspect Timing
	////////////////////////////////////////////////////////////////////


	pt2DPkgCenter = pPkgLocRslt->ptPkgFoundCenter;
	pt2DPkgOffset = pt2DPkgCenter - *ppt2DTchPkgCenter;
	dPkgAngleDisp = pPkgLocDispRslt->dPkgFoundAngle;
	dPkgRotAngleDisp = dPkgAngleDisp - dTchPkgAngle;
	pt2DPkgCenterDisp = pPkgLocDispRslt->ptPkgFoundCenter;

	if (pSymTch->nTotalTaughtTmpl<=0) {
		m_strSymSetErrMsg.Format("Symbol Not Taught!!!");
		return -1;
	}


	// Compute First Template Search Window
	rectTmpl = pSymTch->TchTmplInfo[pSymTch->nFirstTemplate].rectTeachWin;
	rectTmpl = CRect((int) (rectTmpl.left + pt2DPkgOffset.x),
					 (int) (rectTmpl.top + pt2DPkgOffset.y),
					 (int) (rectTmpl.right + pt2DPkgOffset.x),
					 (int) (rectTmpl.bottom + pt2DPkgOffset.y));
	rectTmpl.InflateRect(pSymParm->nXFirstTmplShiftTol/ pcsZoomFactor->cx,pSymParm->nYFirstTmplShiftTol/ pcsZoomFactor->cy);
	CheckRect(&rectTmpl,&pView->sizeImage);
	if (lDebugFlag&DEBUG_PRINT) {
		OutputDebugLog("1st Tmpl SrchWin sx:%d sy:%d ex:%d ey:%d",rectTmpl.left,
																  rectTmpl.top,
																  rectTmpl.right,
																  rectTmpl.bottom);
	}
	if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_STEP_MODE) {
		rectTmplDisp = CRect(rectTmpl.left * pcsZoomFactor->cx,
							rectTmpl.top * pcsZoomFactor->cy,
							rectTmpl.right * pcsZoomFactor->cx,
							rectTmpl.bottom * pcsZoomFactor->cy);
		DrwRect(pView, &rectTmplDisp, &GreenPen);
	}
	if (bDebugThis)
		Timer.Reset();
		// Find First Template Using Logo Parameters


		nError = HostInspRotTmpl(pView,
							  pImgBufSrc,
							  pImgDispBufSrc,
							  pRefImgBufH,
						      pRefTmplBufH,
						      pSymTch->TchRoughTmplIdx[pSymTch->nFirstTemplate],
						      &(pSymParm->LogoPar),
						      &(pSymTch->TchTmplInfo[pSymTch->nFirstTemplate]),
						      &rectTmpl,
						      pSymTch->nTchThres,
						      pSymParm->nTmplContrastTol,
						      pSymParm->nMarkColor,
						      &(pSymRslt->TmplInspRslt[pSymTch->nFirstTemplate]),
							  pcsZoomFactor,
						      lDebugFlag);
		
	if(lDebugFlag&DEBUG_TIME_EXT) {
		dTime = Timer.GetElapsedMSec();
		OutputDebugLog("Host Insp 1st Tmpl Time=%3.2f",dTime);
	}

	if (nError) {
		m_strSymSetErrMsg.Format("Error Processing 1st Template TemplID : %d",pSymTch->nFirstTemplate);
		return PROCESS_ERROR;
	}

	if (DEBUG_STEP_MODE&lDebugFlag) {
		DrwUpdate(pView);
		if(WaitResponse(pView))
			return USER_ABORT;
		DrwDel(pView);
	}

	if (pSymRslt->TmplInspRslt[pSymTch->nFirstTemplate].nResult!=TMPL_PASS) {
		m_nLastTmplErrorID = pSymTch->nFirstTemplate;
		m_nFailErrorCode = pSymRslt->TmplInspRslt[pSymTch->nFirstTemplate].nResult;
		return 0;
	}
	else {
/*
		if (pSymParm->bMaskTemplate) {
			//Mask 1st Template
			rectTmpl = pSymTch->TchTmplInfo[pSymTch->nFirstTemplate].rectTeachWin;
			rectTmpl.OffsetRect(pSymRslt->TmplInspRslt[pSymTch->nFirstTemplate].ptFndTmplPos-pSymTch->TchTmplInfo[pSymTch->nFirstTemplate].rectTeachWin.TopLeft());
			MaskTmpl(pView,
					 pImgBufSrc,
					 &rectTmpl,
					 pSymRslt->TmplInspRslt[pSymTch->nFirstTemplate].nTmplInspThres,
					 nMedian,
					 pSymParm->nMarkColor);
		}
*/
	}
	if (pSymTch->nTotalTaughtTmpl==1) {
		return 0;
	}

	// Compute Last Template Search Window based on Found First Template and Package Location 
	CPoint pt1,pt2,ptFirstTmplOffset,ptMin,ptMax;
	CRect rectMin,rectMax,rectOffset;

	pt1 = pSymRslt->TmplInspRslt[pSymTch->nFirstTemplate].ptFndTmplPos;
	pt2 = pSymTch->TchTmplInfo[pSymTch->nFirstTemplate].rectTeachWin.TopLeft();
	ptFirstTmplOffset =  pt1 - pt2;
	
	rectTmpl = pSymTch->TchTmplInfo[pSymTch->nLastTemplate].rectTeachWin;
	if (lDebugFlag&DEBUG_PRINT) {
		OutputDebugLog("");
		OutputDebugLog("Last Tmpl TchPos sx:%d sy:%d ex:%d ey:%d",rectTmpl.left,
																  rectTmpl.top,
																  rectTmpl.right,
																  rectTmpl.bottom);
	}
	rectTmpl += ptFirstTmplOffset;
	rectTmpl.InflateRect(pSymParm->nXOtherTmplShiftTol/pcsZoomFactor->cx,pSymParm->nYOtherTmplShiftTol/pcsZoomFactor->cy);
	CheckRect(&rectTmpl,&pView->sizeImage);

	if (lDebugFlag&DEBUG_PRINT) {
		OutputDebugLog("Last Tmpl SrchWin sx:%d sy:%d ex:%d ey:%d",rectTmpl.left,
																   rectTmpl.top,
																   rectTmpl.right,
																   rectTmpl.bottom);
	}
	if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_STEP_MODE) {
		rectTmplDisp = CRect(rectTmpl.left * pcsZoomFactor->cx,
							rectTmpl.top * pcsZoomFactor->cy,
							rectTmpl.right * pcsZoomFactor->cx,
							rectTmpl.bottom * pcsZoomFactor->cy);
		DrwRect(pView, &rectTmplDisp, &GreenPen);
	}
	if (bDebugThis)
		Timer.Reset();
	// Find Last Template

		nError = HostInspRotTmpl(pView,
							  pImgBufSrc,
							  pImgDispBufSrc,
					          pRefImgBufH,
							  pRefTmplBufH,
							  pSymTch->TchRoughTmplIdx[pSymTch->nLastTemplate],
							  &(pSymParm->TmplPar),
							  &(pSymTch->TchTmplInfo[pSymTch->nLastTemplate]),
							  &rectTmpl,
							  pSymTch->nTchThres,
							  pSymParm->nTmplContrastTol,
							  pSymParm->nMarkColor,
							  &(pSymRslt->TmplInspRslt[pSymTch->nLastTemplate]),
							  pcsZoomFactor,
							  lDebugFlag);

		if(lDebugFlag&DEBUG_TIME_EXT) {
			dTime = Timer.GetElapsedMSec();
			OutputDebugLog("Host Insp Last Tmpl Time=%3.2f",dTime);
		}

	if (DEBUG_STEP_MODE&lDebugFlag) {
		DrwUpdate(pView);
		if(WaitResponse(pView))
			return USER_ABORT;
		DrwDel(pView);
	}

	if (nError) {
		m_strSymSetErrMsg.Format("Error Processing Last Template TemplID : %d",pSymTch->nLastTemplate);
		return -1;
	}

	if (pSymRslt->TmplInspRslt[pSymTch->nLastTemplate].nResult!=TMPL_PASS) {
		m_nLastTmplErrorID = pSymTch->nLastTemplate;
		m_nFailErrorCode = pSymRslt->TmplInspRslt[pSymTch->nLastTemplate].nResult;
		return 0;
	}
	else {

/*		if (pSymParm->bMaskTemplate) {
			//Mask Last Template
			rectTmpl = pSymTch->TchTmplInfo[pSymTch->nLastTemplate].rectTeachWin;
			rectTmpl.OffsetRect(pSymRslt->TmplInspRslt[pSymTch->nLastTemplate].ptFndTmplPos-pSymTch->TchTmplInfo[pSymTch->nLastTemplate].rectTeachWin.TopLeft());
			MaskTmpl(pView,
					 pImgBufSrc,
					 &rectTmpl,
					 pSymRslt->TmplInspRslt[pSymTch->nLastTemplate].nTmplInspThres,
					 nMedian,
					 pSymParm->nMarkColor);

		}
*/		
	}

	if (pSymTch->nTotalTaughtTmpl==2) {
		return 0;
	}
	pt1 = pSymRslt->TmplInspRslt[pSymTch->nLastTemplate].ptFndTmplPos;
	pt2 = pSymTch->TchTmplInfo[pSymTch->nLastTemplate].rectTeachWin.TopLeft();
	ptLastTmplOffset =  pt1 - pt2;

	ptAvgTmplOffset = CPoint((ptFirstTmplOffset.x + ptLastTmplOffset.x)/2, (ptFirstTmplOffset.y + ptLastTmplOffset.y)/2);
	// Inspecting the rest of the Templates
	CPoint ptTmplOffset;
	double dMarkAngle;
	CPoint ptFnd;

	ptFnd.x = pSymRslt->TmplInspRslt[pSymTch->nLastTemplate].ptFndTmplPos.x - 
			  pSymRslt->TmplInspRslt[pSymTch->nFirstTemplate].ptFndTmplPos.x;
	ptFnd.y = pSymRslt->TmplInspRslt[pSymTch->nLastTemplate].ptFndTmplPos.y - 
		   	  pSymRslt->TmplInspRslt[pSymTch->nFirstTemplate].ptFndTmplPos.y;

	dMarkAngle = atan2((double)ptFnd.y,(double)ptFnd.x);
	dMarkAngle = dMarkAngle - pSymTch->dTchAngle;
	if(dMarkAngle > PI/2)
		dMarkAngle -= PI;
	if(dMarkAngle < -PI/2)
		dMarkAngle += PI;
	pSymRslt->dAngleOffset = dMarkAngle;

	pt2DRef = pSymRslt->TmplInspRslt[pSymTch->nFirstTemplate].ptFndTmplPos;

	for (i=0;i<pSymTch->nTotalTaughtTmpl;i++) {
		if ((i==pSymTch->nFirstTemplate) || (i==pSymTch->nLastTemplate))
			continue;
		
		// Compute Search Window 
		rectTmpl = pSymTch->TchTmplInfo[i].rectTeachWin;
		if (lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("");
			OutputDebugLog("Tmpl[%d] TchPos sx:%d sy:%d ex:%d ey:%d",i,
																	 rectTmpl.left,
																	 rectTmpl.top,
																	 rectTmpl.right,
																	 rectTmpl.bottom);
		}
		rectTmpl += ptAvgTmplOffset;
/*		rc2DMarkRot = CRectangle2D(rectTmpl);
		rc2DMarkRot.RotateAbout(pt2DRef, dMarkAngle);
		rectTmpl = rc2DMarkRot.GetBoundingRect();
*/
		rectTmpl.InflateRect(pSymParm->nXOtherTmplShiftTol/pcsZoomFactor->cx,pSymParm->nYOtherTmplShiftTol/pcsZoomFactor->cy);
		CheckRect(&rectTmpl,&pView->sizeImage);

		if (lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("Tmpl%d SrchWin sx:%d sy:%d ex:%d ey:%d",i,
																	rectTmpl.left,
																	rectTmpl.top,
																	rectTmpl.right,
																	rectTmpl.bottom);
		}
		if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_STEP_MODE) {
			rectTmplDisp = CRect(rectTmpl.left * pcsZoomFactor->cx,
								rectTmpl.top * pcsZoomFactor->cy,
								rectTmpl.right * pcsZoomFactor->cx,
								rectTmpl.bottom * pcsZoomFactor->cy);
			DrwRect(pView, &rectTmplDisp, &GreenPen);
		}
		if (bDebugThis)
			Timer.Reset();
		// Find Template
		nError = HostInspRotTmpl(pView,
							  pImgBufSrc,
							  pImgDispBufSrc,
						      pRefImgBufH,
							  pRefTmplBufH,
							  pSymTch->TchRoughTmplIdx[i],
							  &(pSymParm->TmplPar),
							  &(pSymTch->TchTmplInfo[i]),
							  &rectTmpl,
							  pSymTch->nTchThres,
							  pSymParm->nTmplContrastTol,
							  pSymParm->nMarkColor,
							  &(pSymRslt->TmplInspRslt[i]),
							  pcsZoomFactor,
							  lDebugFlag);

		if(lDebugFlag&DEBUG_TIME_EXT) {
			dTime = Timer.GetElapsedMSec();
			OutputDebugLog("Host Insp Tmpl[%d] Time=%3.2f",i,dTime);
		}

		if (DEBUG_STEP_MODE&lDebugFlag) {
			DrwUpdate(pView);
			if(WaitResponse(pView))
				return USER_ABORT;
			DrwDel(pView);
		}

		if (nError) {
			m_strSymSetErrMsg.Format("Error Processing TemplID : %d",i);
			return -1;
		}
		if (pSymRslt->TmplInspRslt[i].nResult!=TMPL_PASS) {
			m_nLastTmplErrorID = i;
			m_nFailErrorCode = pSymRslt->TmplInspRslt[i].nResult;
			return 0;
		}
		else {
			////Biju
/*			pSymParm->bMaskTemplate = TRUE;
			if (pSymParm->bMaskTemplate) {
				//Mask Other Template
				rectTmpl = pSymTch->TchTmplInfo[i].rectTeachWin;
				rectTmpl.OffsetRect(pSymRslt->TmplInspRslt[i].ptFndTmplPos-pSymTch->TchTmplInfo[i].rectTeachWin.TopLeft());
				MaskTmpl(pView,
						 pImgBufSrc,
						 &rectTmpl,
						 pSymRslt->TmplInspRslt[i].nTmplInspThres,
						 nMedian,
						 pSymParm->nMarkColor);

			}
*/
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::HostCheckRotSymbolSet");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMarkInspect::HostInspTmpl(IMAGEVIEW_INFO *pView,
							   CImageBuffer *pImgBufSrc,
							   CImageBuffer *pImgDispBufSrc,
						       CImageBuffer *pRefImg,
							   CImageBuffer *pRefTmpl,
							   int nIdx,
							   TMPL_INSP_PARM *pTmplInspParm,
							   TMPL_TCH_INFO *pTmplTchInfo,
							   CRect *prectSearch,
							   int nTchThres,
							   int nTmplContrastTol,
							   int nMarkColor,
							   TMPL_INSP_RSLT *pTmplInspRslt,
							   CSize* pcsZoomFactor,
					    	   long lDebugFlag)
{
	int nError;
	CRect rect;
	CRect rcDeb0,rcDeb1;
	int nAcptScore;
	CImageBuffer ImgDebug;
	
	LOGPEN RedPen,
		GreenPen,
		BluePen;
	double dTime;
	CPerformanceTimer Timer;
	int nCormax;
	CRect rectDisp;
	CPoint ptCorrPos;
	CRect rcDispDeb1;


try
{
	RedPen.lopnColor = RGB(255,0,0);
	RedPen.lopnStyle = PS_SOLID;
	RedPen.lopnWidth.x = 1;

	GreenPen.lopnColor = RGB(0,255,0);
	GreenPen.lopnStyle = PS_SOLID;
	GreenPen.lopnWidth.x = 1;

	BluePen.lopnColor = RGB(0,0,255);
	BluePen.lopnStyle = PS_SOLID;
	BluePen.lopnWidth.x = 1;

	// Initialize Insp Rslt Data
	pTmplInspRslt->bHasCompared = FALSE;
	pTmplInspRslt->dExcessBlobSize = 0;
	pTmplInspRslt->dMissingBlobSize = 0;
	pTmplInspRslt->nFndScore = 0;
	pTmplInspRslt->nResult = TMPL_FAIL_NOTFOUND;
	pTmplInspRslt->nTmplInspThres = nTchThres;
	pTmplInspRslt->ptFndExcessBlobPos = CPoint(0,0);
	pTmplInspRslt->ptFndMissingBlobPos = CPoint(0,0);
	pTmplInspRslt->ptFndTmplPos = CPoint(0,0);


	/////////////////////////////////////////////////////////////////
	// Set this Flag to TRUE if you to do debug internal timing
	// for this function
	BOOL bDebugThis=lDebugFlag;
	/////////////////////////////////////////////////////////////////
	if (bDebugThis) {
		Timer.Reset();
	}

	if (pTmplInspParm->bGrossCheck)
		nAcptScore = pTmplInspParm->nRejectScore;
	else
		nAcptScore = pTmplInspParm->nAcceptScore;

	if (DEBUG_STEP_MODE&lDebugFlag) {
		// Display Template at top left hand corner
		CString strDebug;
		rcDeb1 = rcDeb0 = CRect(CPoint(0,0),pImgDispBufSrc->GetImgSize());
		nError = ImgDebug.Allocate(AbsSize(rcDeb1.Size()));
		if (nError == -1) {
			ASSERT(!"Fail To Allocate Image in InspTmpl()");
			goto ErrorAbort;
		}
		nError = ImgCopy(pImgDispBufSrc,&rcDeb0,&ImgDebug,&rcDeb1);

		rcDeb0 = pTmplTchInfo->rectTeachWin;
		rcDeb1 = CRect(0,0,pTmplTchInfo->rectTeachWin.Width(),pTmplTchInfo->rectTeachWin.Height());
		nError = ZoomImg(pRefTmpl,
						 &rcDeb0,
						 &ImgDebug,
						 &CPoint(0,0),
						 pcsZoomFactor);

		rcDispDeb1 = CRect(rcDeb1.left * pcsZoomFactor->cx,
						   rcDeb1.top * pcsZoomFactor->cy,
						   rcDeb1.right * pcsZoomFactor->cx,
						   rcDeb1.bottom * pcsZoomFactor->cy);

		strDebug = "Mark Template";
		nError = DrwDebugImage(pView, pImgDispBufSrc, &ImgDebug, &rcDispDeb1, &CPoint(0,0), &strDebug);
		if(nError != 0)
			goto ErrorAbort;
	}


	nError = m_GrayScaleCorr.Correlate(pImgBufSrc,
									   prectSearch, 
									   &pTmplTchInfo->HostMarkTmpl,
									   nAcptScore,
									   pTmplInspParm->nRejectScore,
									   &nCormax,
									   &ptCorrPos);

	if (nError == -1) {
		OutputDebugLog("m_GrayScaleCorr.Correlate() Error");
		goto ErrorAbort;
	}

	pTmplInspRslt->nFndScore = nCormax;
	pTmplInspRslt->ptFndTmplPos = ptCorrPos;


	if(lDebugFlag&DEBUG_TIME_EXT) {
		dTime = Timer.GetElapsedMSec();
		OutputDebugLog("Correlate () Time=%3.2f",dTime);
	}

	if(lDebugFlag&DEBUG_PRINT) {
		OutputDebugLog("Corr Score=%d",pTmplInspRslt->nFndScore);
	}
	if(pTmplInspRslt->nFndScore == 0) {
		if (lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("Found Score[0] :: TEMPLATE NOT FOUND");
		}
		pTmplInspRslt->nResult = TMPL_FAIL_NOTFOUND;
	}
	else if(pTmplInspRslt->nFndScore < pTmplInspParm->nRejectScore) {
		if (lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("Found Score[%d] < RejectScore[%d] :: FAIL TEMPLATE ",pTmplInspRslt->nFndScore,pTmplInspParm->nRejectScore);
		}
		if (lDebugFlag&DEBUG_DRAW) {
			rect = CRect(pTmplInspRslt->ptFndTmplPos,pTmplTchInfo->rectTeachWin.Size());
			rectDisp = CRect(rect.left * pcsZoomFactor->cx,
							rect.top * pcsZoomFactor->cy,
							rect.right * pcsZoomFactor->cx,
							rect.bottom * pcsZoomFactor->cy);
			DrwRect(pView, &rectDisp, &RedPen);
		}
		pTmplInspRslt->nResult = TMPL_FAIL_CORR;
	}
	else if (pTmplInspRslt->nFndScore >= nAcptScore) {
		if (lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("Found Score[%d] >= AcceptScore[%d] :: PASS ",pTmplInspRslt->nFndScore,nAcptScore);
		}
		if (lDebugFlag&DEBUG_DRAW) {
			rect = CRect(pTmplInspRslt->ptFndTmplPos,pTmplTchInfo->rectTeachWin.Size());
			rectDisp = CRect(rect.left * pcsZoomFactor->cx,
							rect.top * pcsZoomFactor->cy,
							rect.right * pcsZoomFactor->cx,
							rect.bottom * pcsZoomFactor->cy);
			DrwRect(pView, &rectDisp, &GreenPen);
		}
		pTmplInspRslt->nResult = TMPL_PASS;
	}
	else {
		pTmplInspRslt->bHasCompared = TRUE;
		rect = CRect(pTmplInspRslt->ptFndTmplPos,pTmplTchInfo->rectTeachWin.Size());
		if (lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("AcceptScore[%d] > Found Score[%d] > RejectScore[%d] :: MORE PROCESSING",pTmplInspParm->nAcceptScore,pTmplInspRslt->nFndScore,pTmplInspParm->nRejectScore);
		}
		if (lDebugFlag&DEBUG_DRAW) {
			rectDisp = CRect(rect.left * pcsZoomFactor->cx,
							rect.top * pcsZoomFactor->cy,
							rect.right * pcsZoomFactor->cx,
							rect.bottom * pcsZoomFactor->cy);
			DrwRect(pView, &rectDisp, &BluePen);
		}

		if (bDebugThis) {
			Timer.Reset();
		}
		nError = TmplAdjThres(pView,
							  pImgBufSrc,
							  pTmplTchInfo->lPixelCount,
							  nTchThres,
							  nTmplContrastTol,
							  nMarkColor,
							  rect,
							  &pTmplInspRslt->nTmplInspThres,
							  lDebugFlag);
		if(lDebugFlag&DEBUG_TIME_EXT) {
			dTime = Timer.GetElapsedMSec();
			OutputDebugLog("TmplAdjThres() Time=%3.2f",dTime);
		}
		if (nError)
			goto ErrorAbort;

		CImageBuffer ImgTmp1;
		ImgTmp1.Allocate(AbsSize(pView->sizeImage),true);
		if (nError == -1) {
			ASSERT(!"Fail To Allocate Image in InspTmpl()");
			goto ErrorAbort;
		}

		if (bDebugThis) {
			Timer.Reset();
		}
		nError = MinusImg(pView,
						  pImgBufSrc,
						  &rect,
						  pTmplInspRslt->nTmplInspThres,
						  pRefImg,
						  &pTmplTchInfo->rectTeachWin,
						  &ImgTmp1,
						  lDebugFlag);
		if(lDebugFlag&DEBUG_TIME_EXT) {
			dTime = Timer.GetElapsedMSec();
			OutputDebugLog("MinusImg() Time=%3.2f",dTime);
		}
		if (nError)
			goto ErrorAbort;

		if (bDebugThis) {
			Timer.Reset();
		}
		if (pTmplInspParm->nMethod==BLOB_AREA) {
			nError = -1;
			AfxMessageBox("This Option is Still Under Construction");
			if (nError<0) {
				OutputDebugLog("Error in FindExcMisAreaBlob() in InspTmpl()");
				goto ErrorAbort;
			}
		}
		else {
			nError = FindExcMisSqBlb(pView,
									 &ImgTmp1,
									 nMarkColor,
									 pTmplInspParm->dMinMissingPixel,
									 pTmplInspParm->dMinExcessPixel,
									 &pTmplInspRslt->dMissingBlobSize,
									 &pTmplInspRslt->dExcessBlobSize,
									 &pTmplInspRslt->ptFndMissingBlobPos,
									 &pTmplInspRslt->ptFndExcessBlobPos,
									 &rect,
									 lDebugFlag);
			if (nError<0) {
				OutputDebugLog("Error in FindExcMisSquareBlob() in InspTmpl()");
				goto ErrorAbort;
			}
		}
		if(lDebugFlag&DEBUG_TIME_EXT) {
			dTime = Timer.GetElapsedMSec();
			OutputDebugLog("FindMismatch() Time=%3.2f",dTime);
		}
		FreeImgBuf(&ImgTmp1);

		if (nError==1)
			pTmplInspRslt->nResult = TMPL_FAIL_EXCESS_MARK;
		else if (nError==2)
			pTmplInspRslt->nResult = TMPL_FAIL_MISSING_MARK;
		else
			pTmplInspRslt->nResult = TMPL_PASS;

	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::HostInspTmpl");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	if (ImgDebug.IsAllocated())
		FreeImgBuf(&ImgDebug);
	DrwDel(pView);
	return -1;
}


int CMarkInspect::HostInspRotTmpl(IMAGEVIEW_INFO *pView,
							   CImageBuffer *pImgBufSrc,
							   CImageBuffer *pImgDispBufSrc,
						       CImageBuffer *pRefImg,
							   CImageBuffer *pRefTmpl,
							   int nIdx,
							   TMPL_INSP_PARM *pTmplInspParm,
							   TMPL_TCH_INFO *pTmplTchInfo,
							   CRect *prectSearch,
							   int nTchThres,
							   int nTmplContrastTol,
							   int nMarkColor,
							   TMPL_INSP_RSLT *pTmplInspRslt,
							   CSize* pcsZoomFactor,
					    	   long lDebugFlag)
{
	int nError;
	CRect rect;
	CRect rcDeb0,rcDeb1;
	CRect rcDispDeb1;
	int nAcptScore;
	CImageBuffer ImgDebug;

	LOGPEN RedPen,
		   GreenPen,
		   BluePen;
	double dTime;
	CPerformanceTimer Timer;
	int nCormax;
	CRect rectDisp;
	CPoint ptCorrPos;
	CString strDebug;


try
{
	RedPen.lopnColor = RGB(255,0,0);
	RedPen.lopnStyle = PS_SOLID;
	RedPen.lopnWidth.x = 1;

	GreenPen.lopnColor = RGB(0,255,0);
	GreenPen.lopnStyle = PS_SOLID;
	GreenPen.lopnWidth.x = 1;

	BluePen.lopnColor = RGB(0,0,255);
	BluePen.lopnStyle = PS_SOLID;
	BluePen.lopnWidth.x = 1;

	// Initialize Insp Rslt Data
	pTmplInspRslt->bHasCompared = FALSE;
	pTmplInspRslt->dExcessBlobSize = 0;
	pTmplInspRslt->dMissingBlobSize = 0;
	pTmplInspRslt->nFndScore = 0;
	pTmplInspRslt->nResult = TMPL_FAIL_NOTFOUND;
	pTmplInspRslt->nTmplInspThres = nTchThres;
	pTmplInspRslt->ptFndExcessBlobPos = CPoint(0,0);
	pTmplInspRslt->ptFndMissingBlobPos = CPoint(0,0);
	pTmplInspRslt->ptFndTmplPos = CPoint(0,0);


	/////////////////////////////////////////////////////////////////
	// Set this Flag to TRUE if you to do debug internal timing
	// for this function
	BOOL bDebugThis=lDebugFlag;
	/////////////////////////////////////////////////////////////////
	if (bDebugThis) {
		Timer.Reset();
	}

	if (pTmplInspParm->bGrossCheck)
		nAcptScore = pTmplInspParm->nRejectScore;
	else
		nAcptScore = pTmplInspParm->nAcceptScore;

	if (DEBUG_STEP_MODE&lDebugFlag) {
		// Display Template at top left hand corner
		rcDeb1 = rcDeb0 = CRect(CPoint(0,0),pImgDispBufSrc->GetImgSize());

		nError = ImgDebug.Allocate(AbsSize(rcDeb1.Size()));
		if (nError == -1) {
			ASSERT(!"Fail To Allocate Image in InspTmpl()");
			goto ErrorAbort;
		}
		nError = ImgCopy(pImgDispBufSrc,&rcDeb0,&ImgDebug,&rcDeb1);
		if(nError)
		goto ErrorAbort;

		rcDeb0 = pTmplTchInfo->HostMarkTmpl.NonSampling.rCImageBuffer;
		rcDeb1 = CRect(0,0,pTmplTchInfo->rectTeachWin.Width(),pTmplTchInfo->rectTeachWin.Height());
		nError = ZoomImg(&pTmplTchInfo->HostMarkTmpl.NonSampling.ImgBuf,
						 &rcDeb0,
						 &ImgDebug,
						 &CPoint(0,0),
						 pcsZoomFactor);
		if(nError)
		goto ErrorAbort;

		strDebug = "Mark Template";
		rcDispDeb1 = CRect(rcDeb1.left * pcsZoomFactor->cx,
						   rcDeb1.top * pcsZoomFactor->cy,
						   rcDeb1.right * pcsZoomFactor->cx,
						   rcDeb1.bottom * pcsZoomFactor->cy);
		nError = DrwDebugImage(pView, pImgDispBufSrc, &ImgDebug, &rcDispDeb1, &CPoint(0,0), &strDebug);
		if(nError != 0)
			goto ErrorAbort;
	}


	nError = m_GrayScaleCorr.Correlate(pImgBufSrc,
									   prectSearch, 
									   &pTmplTchInfo->HostMarkTmpl,
									   nAcptScore,
									   pTmplInspParm->nRejectScore,
									   &nCormax,
									   &ptCorrPos);

	if (nError == -1) {
		OutputDebugLog("m_GrayScaleCorr.Correlate() Error");
		goto ErrorAbort;
	}

	pTmplInspRslt->nFndScore = nCormax;
	pTmplInspRslt->ptFndTmplPos = ptCorrPos;


	if(lDebugFlag&DEBUG_TIME_EXT) {
		dTime = Timer.GetElapsedMSec();
		OutputDebugLog("Correlate () Time=%3.2f",dTime);
	}

	if(lDebugFlag&DEBUG_PRINT) {
		OutputDebugLog("Corr Score=%d",pTmplInspRslt->nFndScore);
	}
	if(pTmplInspRslt->nFndScore == 0) {
		if (lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("Found Score[0] :: TEMPLATE NOT FOUND");
		}
		if (lDebugFlag&DEBUG_DRAW) {
			rect = *prectSearch;
			DrwRect(pView, &rect, &RedPen);
		}
		pTmplInspRslt->nResult = TMPL_FAIL_NOTFOUND;
	}
	else if(pTmplInspRslt->nFndScore < pTmplInspParm->nRejectScore) {
		if (lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("Found Score[%d] < RejectScore[%d] :: FAIL TEMPLATE ",pTmplInspRslt->nFndScore,pTmplInspParm->nRejectScore);
		}
		if (lDebugFlag&DEBUG_DRAW) {
			rect = CRect(pTmplInspRslt->ptFndTmplPos,pTmplTchInfo->rectTeachWin.Size());
			rectDisp = CRect(rect.left * pcsZoomFactor->cx,
							rect.top * pcsZoomFactor->cy,
							rect.right * pcsZoomFactor->cx,
							rect.bottom * pcsZoomFactor->cy);
			DrwRect(pView, &rectDisp, &RedPen);
		}
		pTmplInspRslt->nResult = TMPL_FAIL_CORR;
	}
	else if (pTmplInspRslt->nFndScore >= nAcptScore) {
		if (lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("Found Score[%d] >= AcceptScore[%d] :: PASS ",pTmplInspRslt->nFndScore,nAcptScore);
		}
		if (lDebugFlag&DEBUG_STEP_MODE) {
			rect = CRect(pTmplInspRslt->ptFndTmplPos,pTmplTchInfo->rectTeachWin.Size());
			rectDisp = CRect(rect.left * pcsZoomFactor->cx,
							rect.top * pcsZoomFactor->cy,
							rect.right * pcsZoomFactor->cx,
							rect.bottom * pcsZoomFactor->cy);
			DrwRect(pView, &rectDisp, &GreenPen);
		}
		pTmplInspRslt->nResult = TMPL_PASS;
	}
/*	else {
		pTmplInspRslt->bHasCompared = TRUE;
		rect = CRect(pTmplInspRslt->ptFndTmplPos,pTmplTchInfo->rectTeachWin.Size());
		if (lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("AcceptScore[%d] > Found Score[%d] > RejectScore[%d] :: MORE PROCESSING",pTmplInspParm->nAcceptScore,pTmplInspRslt->nFndScore,pTmplInspParm->nRejectScore);
		}
		if (lDebugFlag&DEBUG_DRAW) {
			rectDisp = CRect(rect.left * pcsZoomFactor->cx,
							rect.top * pcsZoomFactor->cy,
							rect.right * pcsZoomFactor->cx,
							rect.bottom * pcsZoomFactor->cy);
			DrwRect(pView, &rectDisp, &BluePen);
		}

		if (bDebugThis) {
			Timer.Reset();
		}
		nError = TmplAdjThres(pView,
							  pImgBufSrc,
							  pTmplTchInfo->lPixelCount,
							  nTchThres,
							  nTmplContrastTol,
							  nMarkColor,
							  rect,
							  &pTmplInspRslt->nTmplInspThres,
							  lDebugFlag);
		if(lDebugFlag&DEBUG_TIME_EXT) {
			dTime = Timer.GetElapsedMSec();
			OutputDebugLog("TmplAdjThres() Time=%3.2f",dTime);
		}
		if (nError)
			goto ErrorAbort;

		CImageBuffer ImgTmp1;
		ImgTmp1.Allocate(pView->sizeImage,true);
		if (nError == -1) {
			ASSERT(!"Fail To Allocate Image in InspTmpl()");
			goto ErrorAbort;
		}

		if (bDebugThis) {
			Timer.Reset();
		}
		nError = MinusImg(pView,
						  pImgBufSrc,
						  &rect,
						  pTmplInspRslt->nTmplInspThres,
						  pRefImg,
						  &pTmplTchInfo->rectTeachWin,
						  &ImgTmp1,
						  lDebugFlag);
		if(lDebugFlag&DEBUG_TIME_EXT) {
			dTime = Timer.GetElapsedMSec();
			OutputDebugLog("MinusImg() Time=%3.2f",dTime);
		}
		if (nError)
			goto ErrorAbort;

		if (bDebugThis) {
			Timer.Reset();
		}
		if (pTmplInspParm->nMethod==BLOB_AREA) {
			nError = -1;
			AfxMessageBox("This Option is Still Under Construction");
			if (nError<0) {
				OutputDebugLog("Error in FindExcMisAreaBlob() in InspTmpl()");
				goto ErrorAbort;
			}
		}
		else {
			nError = FindExcMisSqBlb(pView,
									 &ImgTmp1,
									 nMarkColor,
									 pTmplInspParm->dMinMissingPixel,
									 pTmplInspParm->dMinExcessPixel,
									 &pTmplInspRslt->dMissingBlobSize,
									 &pTmplInspRslt->dExcessBlobSize,
									 &pTmplInspRslt->ptFndMissingBlobPos,
									 &pTmplInspRslt->ptFndExcessBlobPos,
									 &rect,
									 lDebugFlag);
			if (nError<0) {
				OutputDebugLog("Error in FindExcMisSquareBlob() in InspTmpl()");
				goto ErrorAbort;
			}
		}
		if(lDebugFlag&DEBUG_TIME_EXT) {
			dTime = Timer.GetElapsedMSec();
			OutputDebugLog("FindMismatch() Time=%3.2f",dTime);
		}
		FreeImgBuf(&ImgTmp1);

		if (nError==1)
			pTmplInspRslt->nResult = TMPL_FAIL_EXCESS_MARK;
		else if (nError==2)
			pTmplInspRslt->nResult = TMPL_FAIL_MISSING_MARK;
		else
			pTmplInspRslt->nResult = TMPL_PASS;

	}
*/

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::HostInspRotTmpl");
	goto ErrorAbort;
}
	
    return 0;
ErrorAbort:
	if (ImgDebug.IsAllocated())
		FreeImgBuf(&ImgDebug);
	DrwDel(pView);
	return -1;
}

int CMarkInspect::ScanForTmpl(IMAGEVIEW_INFO *pView,
							  CImageBuffer *pImgBufSrc,
							  CRect rect,
							  int nMarkColor,
							  int nMinSize,
							  int nThres,
							  int nWinExtX,
							  int nWinExtY,
							  int nMaxRect,
							  int *pnFndBlob,
							  CRect *prectFnd)
{
	return ScanForMarkTmpl(pView,
						   pImgBufSrc,
						   &rect,
						   nMarkColor,
						   nMinSize,
						   nThres,
						   nWinExtX,
						   nWinExtY,
						   nMaxRect,
						   pnFndBlob,
						   prectFnd);

}


int CMarkInspect::ScanForMarkTmpl(IMAGEVIEW_INFO *pView,	// Vision System Info
					CImageBuffer *pImg,		// Source Image Buffer
					CRect *prect,		// Pointer to Rect ROI
					int nMarkColor,		// Type of Marking(WHITE or BLACK)
					int nMinSize,		// Minimum Mark Size(Blob Size)
					int nThres,			// Teach Threshold
					int nWinExtX,		// Template Extension Size
					int nWinExtY,		// Template Extension Size
					int nMaxRect,		// Maximum Allowable Rects
					int *nFndBlob,		// Pointer to Found Template(Blobs) Count
					CRect *rectFnd)	// Pointer to Found Template(Blobs) Rect Pos	
{
	CAdvBlobFind AdvBlobFind;
	ADV_BLOB_FIND_PARM AdvBlobParm;
	int i,j,nError;
	CRect rectA,rectB,rectC;
	CArray <CRect,CRect> rectAry;

	LOGPEN RedPen,BluePen;

	CImageBuffer imgBinBuf;
	int nInvert = 0;

try
{
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
		rectA.InflateRect(nWinExtX,nWinExtY);
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

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::ScanForMarkTmpl");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMarkInspect::GetTeachMarkAngle(IMAGEVIEW_INFO *pView,
							  CRect *rcFirst,
							  CRect *rcLast,
							  double *pdAngle)
{
	return GetAngleBetween2Rects(pView,rcFirst,rcLast,pdAngle);
}

int CMarkInspect::GetTmplTchPixCnt(IMAGEVIEW_INFO *pView,
								   CImageBuffer *pImgBufSrc,
	 							   CRect *prect,
								   int nMarkColor,
								   int nThres,
								   long *plPixCount)

{
	return GetPixCnt(pView,pImgBufSrc,prect,nMarkColor,nThres,plPixCount);
}

int CMarkInspect::ConvTchImg(IMAGEVIEW_INFO *pView,
								 CImageBuffer *pSrcImg,
								 CImageBuffer *pDstImg,
								 CRect *prectROI,
								 int nThres)
{
	return GenerateRefImg(pView,pSrcImg,pDstImg,prectROI,nThres);
}

int CMarkInspect::GetMarkInspParm(CIniFile *ptrIniFile,MARK_INSP_PARM *pMarkInspParm)
{
	int i;
	CString strName;
try
{
	pMarkInspParm->nTotalSymSet = ptrIniFile->GetInteger("MarkInspParm","TotalSymSet",1);
	pMarkInspParm->nTmplSet = ptrIniFile->GetInteger("MarkInspParm","TotalMarkSet",1);
	pMarkInspParm->bEnableMarkInspect = ptrIniFile->GetBoolean("MarkInspParm","EnableMarkInsp",TRUE);

	for (i=0;i<MAX_SYMBOLSET;i++) {
		strName.Format("SymbolSet%d",i);
		pMarkInspParm->SymInspParmSet[i].bLogoSeparateParm = ptrIniFile->GetBoolean(strName,"LogoSeparateParm",TRUE);
		pMarkInspParm->SymInspParmSet[i].bTchWinUserDefine = ptrIniFile->GetBoolean(strName,"TchWinUserDefine",TRUE);
		pMarkInspParm->SymInspParmSet[i].nMarkColor = ptrIniFile->GetInteger(strName,"MarkColor",WHITE);
		pMarkInspParm->SymInspParmSet[i].nMarkRotation = ptrIniFile->GetInteger(strName,"MarkRotatation",5);
		pMarkInspParm->SymInspParmSet[i].nMinCharSize = ptrIniFile->GetInteger(strName,"MinCharSize",10);
		pMarkInspParm->SymInspParmSet[i].nTchRectCnt = ptrIniFile->GetInteger(strName,"TchRectCnt",1);
		pMarkInspParm->SymInspParmSet[i].nTmplContrastTol = ptrIniFile->GetInteger(strName,"TmplContrastTol",10);
		pMarkInspParm->SymInspParmSet[i].nXFirstTmplShiftTol = ptrIniFile->GetInteger(strName,"XFirstTmplShiftTol",25);
		pMarkInspParm->SymInspParmSet[i].nYFirstTmplShiftTol = ptrIniFile->GetInteger(strName,"YFirstTmplShiftTol",25);
		pMarkInspParm->SymInspParmSet[i].nXOtherTmplShiftTol = ptrIniFile->GetInteger(strName,"XOtherTmplShiftTol",10);
		pMarkInspParm->SymInspParmSet[i].nYOtherTmplShiftTol = ptrIniFile->GetInteger(strName,"YOtherTmplShiftTol",10);
		pMarkInspParm->SymInspParmSet[i].bMaskTemplate = ptrIniFile->GetBoolean(strName,"MaskTemplate",FALSE);
		pMarkInspParm->SymInspParmSet[i].m_nMarkMaskDilationSize = ptrIniFile->GetInteger(strName,"MarkMaskDilationSize",1);

		pMarkInspParm->SymInspParmSet[i].nTmplXSkpFact = ptrIniFile->GetInteger(strName,"TmplXSkpFact",3);
		pMarkInspParm->SymInspParmSet[i].nTmplYSkpFact = ptrIniFile->GetInteger(strName,"TmplYSkpFact",3);
		pMarkInspParm->SymInspParmSet[i].nFastTmplXSkpFact = ptrIniFile->GetInteger(strName,"FastTmplXSkpFact",8);
		pMarkInspParm->SymInspParmSet[i].nFastTmplYSkpFact = ptrIniFile->GetInteger(strName,"FastTmplYSkpFact",8);
		pMarkInspParm->SymInspParmSet[i].bEnableFastFirst = ptrIniFile->GetBoolean(strName,"EnableFastFirst",FALSE);
		pMarkInspParm->SymInspParmSet[i].bEnableFastLast = ptrIniFile->GetBoolean(strName,"EnableFastLast",FALSE);

		strName.Format("SymbolSet%dLogoTmplParm",i);
		pMarkInspParm->SymInspParmSet[i].LogoPar.bGrossCheck = ptrIniFile->GetBoolean(strName,"GrossCheck",FALSE);
		pMarkInspParm->SymInspParmSet[i].LogoPar.nAcceptScore = ptrIniFile->GetInteger(strName,"AcceptScore",85);
		pMarkInspParm->SymInspParmSet[i].LogoPar.nRejectScore = ptrIniFile->GetInteger(strName,"RejectScore",20);
		pMarkInspParm->SymInspParmSet[i].LogoPar.dMinMissingPixel = ptrIniFile->GetDouble(strName,"MissingSize",5.0);
		pMarkInspParm->SymInspParmSet[i].LogoPar.dMinExcessPixel = ptrIniFile->GetDouble(strName,"ExcessSize",5.0);
		pMarkInspParm->SymInspParmSet[i].LogoPar.nMethod = ptrIniFile->GetInteger(strName,"Method",SQUARE_AREA);

		strName.Format("SymbolSet%dOtherTmplParm",i);
		pMarkInspParm->SymInspParmSet[i].TmplPar.bGrossCheck = ptrIniFile->GetBoolean(strName,"GrossCheck",FALSE);
		pMarkInspParm->SymInspParmSet[i].TmplPar.nAcceptScore = ptrIniFile->GetInteger(strName,"AcceptScore",85);
		pMarkInspParm->SymInspParmSet[i].TmplPar.nRejectScore = ptrIniFile->GetInteger(strName,"RejectScore",20);
		pMarkInspParm->SymInspParmSet[i].TmplPar.dMinMissingPixel = ptrIniFile->GetDouble(strName,"MissingSize",5.0);
		pMarkInspParm->SymInspParmSet[i].TmplPar.dMinExcessPixel = ptrIniFile->GetDouble(strName,"ExcessSize",5.0);
		pMarkInspParm->SymInspParmSet[i].TmplPar.nMethod = ptrIniFile->GetInteger(strName,"Method",SQUARE_AREA);
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::GetMarkInspParm");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMarkInspect::SetMarkInspParm(CIniFile *ptrIniFile,MARK_INSP_PARM *pMarkInspParm)
{
	int i;
	CString strName;

try
{
	ptrIniFile->SetInteger("MarkInspParm","TotalSymSet",pMarkInspParm->nTotalSymSet);
	ptrIniFile->SetInteger("MarkInspParm","TotalMarkSet",pMarkInspParm->nTmplSet);
	ptrIniFile->SetBoolean("MarkInspParm","EnableMarkInsp",pMarkInspParm->bEnableMarkInspect);

	for (i=0;i<MAX_SYMBOLSET;i++) {
		strName.Format("SymbolSet%d",i);
		ptrIniFile->SetBoolean(strName,"LogoSeparateParm",pMarkInspParm->SymInspParmSet[i].bLogoSeparateParm);
		ptrIniFile->SetBoolean(strName,"TchWinUserDefine",pMarkInspParm->SymInspParmSet[i].bTchWinUserDefine);
		ptrIniFile->SetInteger(strName,"MarkColor",pMarkInspParm->SymInspParmSet[i].nMarkColor);
		ptrIniFile->SetInteger(strName,"MarkRotatation",pMarkInspParm->SymInspParmSet[i].nMarkRotation);
		ptrIniFile->SetInteger(strName,"MinCharSize",pMarkInspParm->SymInspParmSet[i].nMinCharSize);
		ptrIniFile->SetInteger(strName,"TchRectCnt",pMarkInspParm->SymInspParmSet[i].nTchRectCnt);
		ptrIniFile->SetInteger(strName,"TmplContrastTol",pMarkInspParm->SymInspParmSet[i].nTmplContrastTol);
		ptrIniFile->SetInteger(strName,"XFirstTmplShiftTol",pMarkInspParm->SymInspParmSet[i].nXFirstTmplShiftTol);
		ptrIniFile->SetInteger(strName,"YFirstTmplShiftTol",pMarkInspParm->SymInspParmSet[i].nYFirstTmplShiftTol);
		ptrIniFile->SetInteger(strName,"XOtherTmplShiftTol",pMarkInspParm->SymInspParmSet[i].nXOtherTmplShiftTol);
		ptrIniFile->SetInteger(strName,"YOtherTmplShiftTol",pMarkInspParm->SymInspParmSet[i].nYOtherTmplShiftTol);
		ptrIniFile->GetBoolean(strName,"MaskTemplate",pMarkInspParm->SymInspParmSet[i].bMaskTemplate);

		ptrIniFile->SetInteger(strName,"MarkMaskDilationSize",pMarkInspParm->SymInspParmSet[i].m_nMarkMaskDilationSize);


		ptrIniFile->SetInteger(strName,"TmplXSkpFact",pMarkInspParm->SymInspParmSet[i].nTmplXSkpFact);
		ptrIniFile->SetInteger(strName,"TmplYSkpFact",pMarkInspParm->SymInspParmSet[i].nTmplYSkpFact);
		ptrIniFile->SetInteger(strName,"FastTmplXSkpFact",pMarkInspParm->SymInspParmSet[i].nFastTmplXSkpFact);
		ptrIniFile->SetInteger(strName,"FastTmplYSkpFact",pMarkInspParm->SymInspParmSet[i].nFastTmplYSkpFact);
		ptrIniFile->SetBoolean(strName,"EnableFastFirst",pMarkInspParm->SymInspParmSet[i].bEnableFastFirst);
		ptrIniFile->SetBoolean(strName,"EnableFastLast",pMarkInspParm->SymInspParmSet[i].bEnableFastLast);


		strName.Format("SymbolSet%dLogoTmplParm",i);
		ptrIniFile->SetBoolean(strName,"GrossCheck",pMarkInspParm->SymInspParmSet[i].LogoPar.bGrossCheck);
		ptrIniFile->SetInteger(strName,"AcceptScore",pMarkInspParm->SymInspParmSet[i].LogoPar.nAcceptScore);
		ptrIniFile->SetInteger(strName,"RejectScore",pMarkInspParm->SymInspParmSet[i].LogoPar.nRejectScore);
		ptrIniFile->SetDouble(strName,"MissingSize",pMarkInspParm->SymInspParmSet[i].LogoPar.dMinMissingPixel);
		ptrIniFile->SetDouble(strName,"ExcessSize",pMarkInspParm->SymInspParmSet[i].LogoPar.dMinExcessPixel);
		ptrIniFile->SetInteger(strName,"Method",pMarkInspParm->SymInspParmSet[i].LogoPar.nMethod);

		strName.Format("SymbolSet%dOtherTmplParm",i);
		ptrIniFile->SetBoolean(strName,"GrossCheck",pMarkInspParm->SymInspParmSet[i].TmplPar.bGrossCheck);
		ptrIniFile->SetInteger(strName,"AcceptScore",pMarkInspParm->SymInspParmSet[i].TmplPar.nAcceptScore);
		ptrIniFile->SetInteger(strName,"RejectScore",pMarkInspParm->SymInspParmSet[i].TmplPar.nRejectScore);
		ptrIniFile->SetDouble(strName,"MissingSize",pMarkInspParm->SymInspParmSet[i].TmplPar.dMinMissingPixel);
		ptrIniFile->SetDouble(strName,"ExcessSize",pMarkInspParm->SymInspParmSet[i].TmplPar.dMinExcessPixel);
		ptrIniFile->SetInteger(strName,"Method",pMarkInspParm->SymInspParmSet[i].TmplPar.nMethod);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::SetMarkInspParm");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMarkInspect::LoadMarkInspParm(CString strDirectoryName,
							  CString strFileName,
							  MARK_INSP_PARM *pMarkInspParm)
{
	CIniFile iniFile;
	CString strDataFileName;
try
{
	strDataFileName = strDirectoryName + "\\" + strFileName + ".mrk";
	iniFile.SetPath(strDataFileName);

	if (iniFile.ReadFile()) {
		// File Exist
		GetMarkInspParm(&iniFile,pMarkInspParm);
	}
	else {
		// File Not Found
		OutputDebugLog("%s not found. Default values assumed",strDataFileName);
		SetMarkInspParm(&iniFile,pMarkInspParm);
		iniFile.WriteFile();
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::LoadMarkInspParm");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMarkInspect::SaveMarkInspParm(CString strDirectoryName,
							  CString strFileName,
							  MARK_INSP_PARM *pMarkInspParm)
{
	CIniFile iniFile;
	CString strDataFileName;

try
{
	strDataFileName = strDirectoryName + "\\" + strFileName + ".mrk";
	iniFile.SetPath(strDataFileName);
	SetMarkInspParm(&iniFile,pMarkInspParm);
	iniFile.WriteFile();
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::SaveMarkInspParm");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMarkInspect::GetMarkTchInfo(CIniFile *ptrIniFile,MARK_TCH_INFO *pMarkTchInfo)
{
	int i,j,k;
	CString strName;
	CString strVal;

try
{
	pMarkTchInfo->nTaughtMarkSet = ptrIniFile->GetInteger("MarkTchInfo","TaughtMarkSet",0);
	pMarkTchInfo->pt2DTchPkgCenter = ptrIniFile->GetCPoint2D("MarkTchInfo","TeachPkgCtr",CPoint2D(0,0));
	pMarkTchInfo->dTchPkgAngle = ptrIniFile->GetDouble("MarkTchInfo","TeachPkgAngle",0);
	for (i=0;i<MAX_MARKSET;i++) {
		strName.Format("MarkSet%d",i);
		pMarkTchInfo->MarkTchInfoSet[i].nTaughtSymSet = ptrIniFile->GetInteger(strName,"TaughtSymSet",0);
		for (j=0;j<MAX_SYMBOLSET;j++) {
			strName.Format("MarkSet%dSymSet%d",i,j);
			pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].dTchAngle = ptrIniFile->GetDouble(strName,"TchAngle",0.0);
			pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nTchThres = ptrIniFile->GetInteger(strName,"TchThres",128);
			pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nTotalTaughtTmpl = ptrIniFile->GetInteger(strName,"TotalTaughtTmpl",0);
			pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nFirstTemplate = ptrIniFile->GetInteger(strName,"FirstTemplate",0);
			pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nLastTemplate = ptrIniFile->GetInteger(strName,"LastTemplate",0);
			pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nLeftMostTemplate = ptrIniFile->GetInteger(strName,"LeftMostTemplate",0);
			pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nRightMostTemplate = ptrIniFile->GetInteger(strName,"RightMostTemplate",0);
			for (k=0;k<MAX_TCH_WIN;k++) {
				strVal.Format("MrkTchWin%d",k);
				pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].rectTchWin[k] = ptrIniFile->GetCRect(strName,strVal,CRect(20,20,100,100));
			}
			for (k=0;k<MAX_TMPL_PER_MRKSET;k++) {
				strVal.Format("Tmpl%dPixCount",k);
				pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].lPixelCount = ptrIniFile->GetLong(strName,strVal,0);
				strVal.Format("Tmpl%dTchWin",k);
				pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].rectTeachWin = ptrIniFile->GetCRect(strName,strVal,CRect(10,10,20,20));
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::GetMarkTchInfo");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMarkInspect::SetMarkTchInfo(CIniFile *ptrIniFile,MARK_TCH_INFO *pMarkTchInfo)
{
	int i,j,k;
	CString strName;
	CString strVal;
try
{
	ptrIniFile->SetInteger("MarkTchInfo","TaughtMarkSet",pMarkTchInfo->nTaughtMarkSet);
	ptrIniFile->SetCPoint2D("MarkTchInfo","TeachPkgCtr",pMarkTchInfo->pt2DTchPkgCenter);
	ptrIniFile->SetDouble("MarkTchInfo","TeachPkgAngle",pMarkTchInfo->dTchPkgAngle);
	for (i=0;i<MAX_MARKSET;i++) {
		strName.Format("MarkSet%d",i);
		ptrIniFile->SetInteger(strName,"TaughtSymSet",pMarkTchInfo->MarkTchInfoSet[i].nTaughtSymSet);
		for (j=0;j<MAX_SYMBOLSET;j++) {
			strName.Format("MarkSet%dSymSet%d",i,j);
			ptrIniFile->SetDouble(strName,"TchAngle",pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].dTchAngle);
			ptrIniFile->SetInteger(strName,"TchThres",pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nTchThres);
			ptrIniFile->SetInteger(strName,"TotalTaughtTmpl",pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nTotalTaughtTmpl);
			ptrIniFile->SetInteger(strName,"FirstTemplate",pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nFirstTemplate);
			ptrIniFile->SetInteger(strName,"LastTemplate",pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nLastTemplate);
			ptrIniFile->SetInteger(strName,"LeftMostTemplate",pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nLeftMostTemplate);
			ptrIniFile->SetInteger(strName,"RightMostTemplate",pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nRightMostTemplate);
			for (k=0;k<MAX_TCH_WIN;k++) {
				strVal.Format("MrkTchWin%d",k);
				ptrIniFile->SetCRect(strName,strVal,pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].rectTchWin[k]);
			}
			for (k=0;k<MAX_TMPL_PER_MRKSET;k++) {
				strVal.Format("Tmpl%dPixCount",k);
				ptrIniFile->SetLong(strName,strVal,pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].lPixelCount);
				strVal.Format("Tmpl%dTchWin",k);
				ptrIniFile->SetCRect(strName,strVal,pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].rectTeachWin);
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::SetMarkTchInfo");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMarkInspect::LoadMarkTchInfo(CString strDirectoryName,
							  CString strFileName,
							  MARK_TCH_INFO *pMarkTchInfo)
{
	CIniFile iniFile;
	CString strDataFileName;

try
{
	strDataFileName = strDirectoryName + "\\" + strFileName + ".mtch";
	iniFile.SetPath(strDataFileName);

	if (iniFile.ReadFile()) {
		// File Exist
		GetMarkTchInfo(&iniFile,pMarkTchInfo);
	}
	else {
		// File Not Found
		OutputDebugLog("%s not found. Default values assumed",strDataFileName);
		SetMarkTchInfo(&iniFile,pMarkTchInfo);
		iniFile.WriteFile();
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::LoadMarkTchInfo");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMarkInspect::SaveMarkTchInfo(CString strDirectoryName,
							  CString strFileName,
							  MARK_TCH_INFO *pMarkTchInfo)
{
	CIniFile iniFile;
	CString strDataFileName;
try
{
	strDataFileName = strDirectoryName + "\\" + strFileName + ".mtch";
	iniFile.SetPath(strDataFileName);
	SetMarkTchInfo(&iniFile,pMarkTchInfo);
	iniFile.WriteFile();
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::SaveMarkTchInfo");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CMarkInspect::CreateMarkTemplate(IMAGEVIEW_INFO *pView,
							  MARK_INSP_PARM *pMarkInspParm,
							  MARK_TCH_INFO *pMarkTchInfo)
{
	int	nSet, nTemplates;
	int i, j, k;
	CImageBuffer* pImg;
	CImageBuffer* pTmplImg;
	CRect	rectTmpl;
	int nTemplateSamplingAcptScore;
	int nError;

try
{
	nTemplateSamplingAcptScore = 80;

	m_nMarkTemplateSet=pMarkInspParm->nTmplSet;

	for (i=0;i<m_nMarkTemplateSet;i++) {
		if (pMarkTchInfo->MarkTchInfoSet[i].nTaughtSymSet==0)
			continue;

		nSet = pMarkTchInfo->MarkTchInfoSet[i].nTaughtSymSet;
		pImg = pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefImg;
		pTmplImg = pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefTmpl;

		for (j=0; j<nSet; j++) {
			nTemplates = pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nTotalTaughtTmpl;
			for (k=0; k<nTemplates; k++) {
				rectTmpl = pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].rectTeachWin;
				if(pMarkTchInfo->nTaughtMarkSet>0) {
					nError = m_GrayScaleCorr.CreateHostTeachMarkTemplate(pTmplImg,
																&rectTmpl,
																&pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].HostMarkTmpl,
																nTemplateSamplingAcptScore);
					if(nError == -1) {
						OutputDebugLog("CreateOrientationTemplate() Error");
						return -1;
					}
				}
			}
		}	
	}	

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::CreateMarkTemplate");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;

}


int CMarkInspect::CreateRotMarkTemplate(IMAGEVIEW_INFO *pView,
							  MARK_INSP_PARM *pMarkInspParm,
							  MARK_TCH_INFO *pMarkTchInfo,
							  MARK_TCH_INFO* pMarkRotTchInfo,
							  double dMarkZoomRotAngleStep,
							  double dMarkRotAngleStep,
							  int nMarkRotTchInfoNo,
							  CSize* pcsZoomFactor)
{
	int	nSet, nTemplates;
	int i, j, k, m;
	CImageBuffer* pImg;
	CImageBuffer* pTmplImg;

	CImageBuffer ImgTmplZoom;
	CRect rcTmplImgZoom;
	CImageBuffer ImgTmplZoomRot;
	CImageBuffer ImgTmplSamplingRot;
	CRect rcTmplImgSampling;
	CRect rectTmplRelative;
	CImageBuffer ImgTmplBin;


	CRect	rectTmpl;
	int nTemplateSamplingAcptScore;
	int nError;
	CRectangle2D rc2DRotMark;
	double dRotAngle;
	CImageBuffer ImgRot;
	CPoint2D pt2DRotCenter;
	CPoint2D pt2DZoomRotCenter;
	CRect rcMarkTchBounding;
	CRect rcMarkRotBoundingTol;
	int nMarkRotRadius;
	CRectangle2D rc2DRotZoomMark;
	double dMask[10];
	CSize szMask;
	long lProcMode;

try
{
	pImg = NULL;
	nTemplateSamplingAcptScore = 80;
	ImgRot.Allocate(AbsSize(pView->sizeImage));

	m_nMarkTemplateSet=pMarkInspParm->nTmplSet;

	rcMarkTchBounding.SetRectEmpty();
	for (i=0;i<m_nMarkTemplateSet;i++) {
		if (pMarkTchInfo->MarkTchInfoSet[i].nTaughtSymSet==0)
			continue;
		nSet = pMarkTchInfo->MarkTchInfoSet[i].nTaughtSymSet;
		pImg = pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefImg;
		pTmplImg = pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefTmpl;
		for (j=0; j<nSet; j++) {
			nTemplates = pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nTotalTaughtTmpl;
			for (k=0; k<nTemplates; k++) {
				rectTmpl = pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].rectTeachWin;
				rcMarkTchBounding |= rectTmpl;
				if(pMarkTchInfo->nTaughtMarkSet>0) {
					nError = m_GrayScaleCorr.CreateHostTeachMarkTemplate(pTmplImg,
																&rectTmpl,
																&pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].HostMarkTmpl,
																nTemplateSamplingAcptScore);
					if(nError == -1) {
						OutputDebugLog("CreateOrientationTemplate() Error");
						return -1;
					}
				}
			}
		}	
	}	

	nMarkRotRadius = (rcMarkTchBounding.Width() + rcMarkTchBounding.Height())/2;
	rcMarkRotBoundingTol = CRect(rcMarkTchBounding.left - nMarkRotRadius,
								 rcMarkTchBounding.top - nMarkRotRadius,
								 rcMarkTchBounding.right + nMarkRotRadius,
								 rcMarkTchBounding.bottom + nMarkRotRadius);
	
	if (!pImg) {
//		ASSERT(!"pImg is a bad Pointer");
		return -1;
	}
	if (!pImg->IsAllocated()) {
		ASSERT(!"pImg is not Allocated");
		return -1;
	}

	CheckRect(&rcMarkRotBoundingTol, &pImg->GetImgSize());
	for (m = 0; m < nMarkRotTchInfoNo; m++) {
		for (i=0;i<m_nMarkTemplateSet;i++) {
			if (pMarkTchInfo->MarkTchInfoSet[i].nTaughtSymSet==0)
				continue;
			nSet = pMarkTchInfo->MarkTchInfoSet[i].nTaughtSymSet;
			pImg = pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefImg;
			pTmplImg = pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefTmpl;
			rcTmplImgSampling = CRect(CPoint(0,0), CSize(rcMarkRotBoundingTol.right, rcMarkRotBoundingTol.bottom));
			rcTmplImgZoom = CRect(CPoint(0,0), CSize(rcTmplImgSampling.Size().cx * pcsZoomFactor->cx, rcTmplImgSampling.Size().cy * pcsZoomFactor->cy));
			ImgTmplZoom.Allocate(AbsSize(rcTmplImgZoom.Size()));
			ImgTmplZoomRot.Allocate(AbsSize(rcTmplImgZoom.Size()));
			ImgTmplSamplingRot.Allocate(AbsSize(rcTmplImgSampling.Size()));
			nError = ZoomImg(pTmplImg,
							 &rcTmplImgSampling,
							 &ImgTmplZoom,
							 &CPoint(0,0),
							 pcsZoomFactor);
			pt2DRotCenter = pMarkTchInfo->pt2DTchPkgCenter;
			pt2DZoomRotCenter = CPoint2D(pt2DRotCenter.x * pcsZoomFactor->cx, pt2DRotCenter.y * pcsZoomFactor->cy);
			dRotAngle = (m - nMarkRotTchInfoNo/2) * dMarkZoomRotAngleStep;
			ImgRotate(&ImgTmplZoom,
					  &rcTmplImgZoom,
					  &ImgTmplZoomRot,
					  &rcTmplImgZoom,
					  &pt2DZoomRotCenter,
					  dRotAngle,
					  TRUE);
			ImgSampling(&ImgTmplZoomRot,
						&rcTmplImgZoom,	
						&ImgTmplSamplingRot,
						&rcTmplImgSampling,
						pcsZoomFactor);
			for (j=0; j<nSet; j++) {
				nTemplates = pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nTotalTaughtTmpl;
////Biju				nTemplates = 2;
				for (k=0; k<nTemplates; k++) {
					pMarkRotTchInfo[m].MarkTchInfoSet[i].SymTchInfoSet[j].nTotalTaughtTmpl = nTemplates;
					pMarkRotTchInfo[m].MarkTchInfoSet[i].nTaughtSymSet = nSet;

					pMarkRotTchInfo[m].MarkTchInfoSet[i].SymTchInfoSet[j].nFirstTemplate = pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nFirstTemplate;
					pMarkRotTchInfo[m].MarkTchInfoSet[i].SymTchInfoSet[j].nLastTemplate = pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nLastTemplate;

					rectTmpl = pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].rectTeachWin;

					rc2DRotZoomMark = CRectangle2D(CRect(rectTmpl.left * pcsZoomFactor->cx,
													 rectTmpl.top * pcsZoomFactor->cy,
													 rectTmpl.right * pcsZoomFactor->cx,
													 rectTmpl.bottom * pcsZoomFactor->cy));

					rc2DRotZoomMark.RotateAbout(pt2DZoomRotCenter, dRotAngle);
					rectTmpl = rc2DRotZoomMark.GetBoundingRect();
					rectTmpl = CRect(rectTmpl.left / pcsZoomFactor->cx,
									 rectTmpl.top / pcsZoomFactor->cy,
									 rectTmpl.right / pcsZoomFactor->cx,
									 rectTmpl.bottom / pcsZoomFactor->cy);
					pMarkRotTchInfo[m].MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].rectTeachWin = rectTmpl;
					pt2DRotCenter = rectTmpl.CenterPoint();

					rectTmplRelative = CRect(CPoint(0,0),rectTmpl.Size());
					ImgTmplBin.Allocate(AbsSize(rectTmplRelative.Size()));
					pMarkRotTchInfo[m].MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].ImgMarkMask.Allocate(AbsSize(rectTmpl.Size()));
					nError = Binarise(&ImgTmplSamplingRot,
									  &rectTmpl,
									  &ImgTmplBin,
									  &rectTmplRelative,
									  pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nTchThres);

					lProcMode = 0;
					if(pMarkInspParm->SymInspParmSet[i].m_nMarkMaskDilationSize>1)
						szMask = CSize(pMarkInspParm->SymInspParmSet[i].m_nMarkMaskDilationSize,pMarkInspParm->SymInspParmSet[i].m_nMarkMaskDilationSize);
					else
						szMask = CSize(1,1);
					ImgDilation(&ImgTmplBin,
								 &rectTmplRelative,
								 pMarkInspParm->SymInspParmSet[i].nMarkColor,
								 &pMarkRotTchInfo[m].MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].ImgMarkMask,
								 &rectTmplRelative,
								 &dMask[0],
								 &szMask,
								 lProcMode);

					ImgInvert(&pMarkRotTchInfo[m].MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].ImgMarkMask,
							  &rectTmplRelative,
							  &pMarkRotTchInfo[m].MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].ImgMarkMask,
							  &rectTmplRelative);

					if(pMarkTchInfo->nTaughtMarkSet>0) {
						nError = m_GrayScaleCorr.CreateHostTeachMarkTemplate(&ImgTmplSamplingRot,
																	&rectTmpl,
																	&pMarkRotTchInfo[m].MarkTchInfoSet[i].SymTchInfoSet[j].TchTmplInfo[k].HostMarkTmpl,
																	nTemplateSamplingAcptScore);
						if(nError == -1) {
							OutputDebugLog("CreateMarkTemplate() Error");
							return -1;
						}
					}
				}
			}
		}	
	}
	
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::CreateRotMarkTemplate");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}



int CMarkInspect::SaveMarkTemplate(IMAGEVIEW_INFO *pView,
								   CString strDirectoryName,
								   CString strFileName,
								   MARK_TCH_INFO *pMarkTchInfo,
								   int nIdx)
{
	CString strDataFileName;
	CString strImgFileName;
	CImageBuffer* pImage;
	CString strTail;
	int nError;

try
{
	// Save the images now
	pImage = pMarkTchInfo->MarkTchInfoSet[nIdx].pCImageBufferRefImg;
	strTail.Format("MarkBin%02d.bmp", nIdx+1);
	strImgFileName = strDirectoryName + "\\" + strFileName + strTail;
	
	nError = SaveGrayScaleBMPFile(strImgFileName,*pImage);
	
	if (nError==-1)
		goto ErrorAbort;

	strTail.Format("MarkTmpl%02d.bmp", nIdx+1);
	pImage = pMarkTchInfo->MarkTchInfoSet[nIdx].pCImageBufferRefTmpl;
	strImgFileName = strDirectoryName + "\\" + strFileName + strTail;
	
	nError = SaveGrayScaleBMPFile(strImgFileName,*pImage);
	
	if (nError==-1)
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::SaveMarkTemplate");
	goto ErrorAbort;
}
	return 0;

ErrorAbort:
	OutputDebugLog("SaveMarkTemplate() Error");
	DrwDel(pView);
	return -1;

}

int CMarkInspect::LoadMarkTemplates(IMAGEVIEW_INFO *pView,
									CString strDirectoryName,
									CString strFileName,
									MARK_INSP_PARM *pMarkInspParm,
									MARK_TCH_INFO *pMarkTchInfo)
{
	CString strImg1;
	CString strImg2;
	CString strTail;
	int i;
	int nError;

try
{
	pMarkTchInfo->nTaughtMarkSet = 0;

	// Read all the images
	CFile file1, file2;

	for (i=0;i<pMarkInspParm->nTmplSet;i++) {
		strTail.Format("MarkBin%02d.bmp", i+1);
		strImg1 = strDirectoryName + "\\" + strFileName + strTail;
		strTail.Format("MarkTmpl%02d.bmp", i+1);
		strImg2 = strDirectoryName + "\\" + strFileName + strTail;
		if (file1.Open(strImg1, CFile::modeRead) && file2.Open(strImg2, CFile::modeRead)) {
			pMarkTchInfo->nTaughtMarkSet++;
			file1.Close();
			file2.Close();

			nError = LoadGrayScaleBMPFile(strImg1,*pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefImg);
			
			if (nError==-1)
				goto ErrorAbort;
			
			nError = LoadGrayScaleBMPFile(strImg2,*pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefTmpl);
			
			if (nError==-1)
				goto ErrorAbort;

			if (i == MAX_MARKSET)
				break;
		}
	}
	OutputDebugLog("Mark Teach Data Loaded : Mark sets = %d", i);
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::LoadMarkTemplates");
	goto ErrorAbort;
}	
	return 0;

ErrorAbort:
	OutputDebugLog("LoadMarkTemplate() Error");
	DrwDel(pView);
	return -1;

}

// 190704
int CMarkInspect::InspectRotMark(IMAGEVIEW_INFO *pView,
								 CImageBuffer *pImgBufSrc,
								 MARK_INSP_PARM *pMarkInspParm,
								 MARK_TCH_INFO *pMarkTchInfo,
								 MARK_TCH_INFO* pMarkRotTchInfo,
								 int* pnSelectedRotTchInfo,
								 int nAngleDeg,
								 int nMarkRotTchInfoNo,
								 MARK_INSP_RESULT *pMarkInspResult,
								 long lDebugFlag)
{
	int i,j,nError;
	CString strMsg;
	CPerformanceTimer Timer;
	double dTime;
	int nMarkAngleIndex;

try
{
	nMarkAngleIndex = Round(nMarkRotTchInfoNo/2 + nAngleDeg);
	if(nMarkAngleIndex < 0)
		nMarkAngleIndex = 0;
	if(nMarkAngleIndex >= nMarkRotTchInfoNo)
		nMarkAngleIndex = nMarkRotTchInfoNo - 1;

	*pnSelectedRotTchInfo = nMarkAngleIndex;

	m_strErrorMsg.Format("Pass");

	if (!pMarkInspParm->bEnableMarkInspect)
		return INSP_PASS;


	if(lDebugFlag&DEBUG_TIME_EXT)
		Timer.Reset();

	pMarkInspResult->MarkInspRsltSet[0].SymInspRsltSet[0].TmplInspRslt[0].nFndScore = 0;

	if (pMarkTchInfo->nTaughtMarkSet<=0) {
		return MARK_NOT_TAUGHT;
	}

	//Iterate through the Mark Sets
	for(i=0;i<pMarkInspParm->nTmplSet;i++) {
		if (pMarkTchInfo->MarkTchInfoSet[i].nTaughtSymSet==0)
			continue;

		if(!pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefImg) {
			m_strErrorMsg.Format("No Reference Image!!");
			return PROCESS_ERROR;
		}
		if(!pMarkTchInfo->MarkTchInfoSet[i].pCImageBufferRefTmpl) {
			m_strErrorMsg.Format("No Template Image!!");
			return PROCESS_ERROR;
		}
		
		if(lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("");
			OutputDebugLog("Inspecting Mark Template Set %d",i);
		}
		//Iterate through the Symbol Sets
		for (j=0;j<pMarkInspParm->nTotalSymSet;j++) {
			if (pMarkTchInfo->MarkTchInfoSet[i].SymTchInfoSet[j].nTotalTaughtTmpl==0)
				continue;

			if(lDebugFlag&DEBUG_PRINT) {
				OutputDebugLog("");
				OutputDebugLog("Inspecting Symbol Set %d",j);
			}
			pMarkInspResult->MarkInspRsltSet[0].SymInspRsltSet[0].TmplInspRslt[0].nFndScore = 0;

			CPerformanceTimer mtime;
			mtime.Reset();

			nError = HostCheckRotSymbolSet(pView,
									pImgBufSrc,
								    pMarkRotTchInfo[nMarkAngleIndex].MarkTchInfoSet[i].pCImageBufferRefImg,
								    pMarkRotTchInfo[nMarkAngleIndex].MarkTchInfoSet[i].pCImageBufferRefTmpl,
									&pMarkTchInfo->pt2DTchPkgCenter,
									pMarkTchInfo->dTchPkgAngle,
									&(pMarkInspParm->SymInspParmSet[j]),
									&(pMarkRotTchInfo[nMarkAngleIndex].MarkTchInfoSet[i].SymTchInfoSet[j]),
									&(pMarkInspResult->MarkInspRsltSet[i].SymInspRsltSet[j]),
									lDebugFlag);

			if(lDebugFlag&DEBUG_TIME_EXT) 
				OutputDebugLog("Host Mark Insp Time = %4.3f", mtime.GetElapsedMSec());
			
			if (nError) {
				strMsg.Format("Error::SymSet:%d",j);
				m_strErrorMsg = strMsg+"|"+m_strSymSetErrMsg;
				return nError;
			}
			// Fails SymbolSet Inspection so Skip other SymbolSet and goto next MarkSet
			if (m_nLastTmplErrorID!=-1) {
				m_strErrorMsg.Format("Fail Inspection on MarkSet:%d SymSet:%d TmplID:%d",
		  							  m_nLastMarkSetErrorID,m_nLastSymSetErrorID,m_nLastTmplErrorID);
				break;
			}
		}
		// Pass Inspection so skip the other remaining UnInspected MarkSets
		if (m_nLastTmplErrorID==-1) {
			break;
		}
	}
	if(lDebugFlag&DEBUG_TIME_EXT) {
		dTime = Timer.GetElapsedMSec();
		OutputDebugLog("Mark Inspect Time=%3.2f",dTime);
	}
	if (nError)
		return nError;

	if (m_nLastTmplErrorID==-1) {
		return INSP_PASS;
	}
	else {
		//OutputDebugLog("Mark Error Code=%d",m_nFailErrorCode);
		switch(m_nFailErrorCode) {
			case (TMPL_PASS) :
				m_strErrorMsg = "Pass";
				return INSP_PASS;
			case (TMPL_FAIL_CORR) :
				m_strErrorMsg = "Fail Correlation";
				return INSP_FAIL_MARK_CORR;
			case (TMPL_FAIL_EXCESS_MARK) :
				m_strErrorMsg = "Fail : Excess Mark";
				return INSP_FAIL_MARK_EXCESS;
			case (TMPL_FAIL_MISSING_MARK) :
				m_strErrorMsg = "Fail : Missing Mark";
				return INSP_FAIL_MARK_MISSING;
			case (TMPL_FAIL_NOTFOUND) :
				m_strErrorMsg = "Fail : Mark Template Not found";
				return INSP_FAIL_MARK_TMPL_FIND;
			default :
				return PROCESS_ERROR;
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::InspectRotMark");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
     return -1;
}

int CMarkInspect::HostCheckRotSymbolSet(IMAGEVIEW_INFO *pView,
										CImageBuffer *pImgBufSrc,
										CImageBuffer *pRefImgBufH,
										CImageBuffer *pRefTmplBufH,
										CPoint2D* ppt2DTchPkgCenter,
										double dTchPkgAngle,
										SYM_INSP_PARM_SET *pSymParm,
										SYM_TCH_INFO_SET *pSymTch,
										SYM_INSP_RSLT_SET *pSymRslt,
										long lDebugFlag)
{
	CPoint ptPkgOffset,
		   ptTmplTopLft,
		   ptPkgAngleOffset;

	CPoint2D pt2DPkgOffset;

	CPoint2D pt2DOld,
			 pt2DNew,
			 pt2DRef;
	
	int i,nError;
	CRect rectTmplDisp;
	CRect rectTmpl,
		  rectSearch;
	BOOL bDebugThis;
	CPerformanceTimer Timer;
	double dTime;
	//int nMedian;
	CRectangle2D rc2DTmpl;
	CPoint2D pt2DTmplCenter;
	CPoint2D pt2DPkgCenter;
	CRectangle2D rc2DMarkRot;
	CPoint ptLastTmplOffset;
	CPoint ptAvgTmplOffset;

	LOGPEN GreenPen;

	CPoint2D pt2DPkgCenterDisp;
	CPoint2D pt2DPkgOffsetDisp;
	//double dPkgAngleDisp;
	//double dPkgRotAngleDisp;
	CRectangle2D rc2DMarkRotDisp;

try
{
	GreenPen.lopnColor = RGB(0,255,0);
	GreenPen.lopnStyle = PS_SOLID;
	GreenPen.lopnWidth.x = 1;

	m_strSymSetErrMsg.Empty();
	m_nLastTmplErrorID = -1;
	m_nFailErrorCode = 0;


	////////////////////////////////////////////////////////////////////
	if (lDebugFlag&DEBUG_TIME)
		bDebugThis=TRUE;	//Enable This To Debug Template Inspect Timing
	////////////////////////////////////////////////////////////////////


	// Check here
/*	pt2DPkgCenter = pPkgLocRslt->ptPkgFoundCenter;
	pt2DPkgOffset = pt2DPkgCenter - *ppt2DTchPkgCenter;
	dPkgAngleDisp = pPkgLocDispRslt->dPkgFoundAngle;
	dPkgRotAngleDisp = dPkgAngleDisp - dTchPkgAngle;
	pt2DPkgCenterDisp = pPkgLocDispRslt->ptPkgFoundCenter;
*/	//////////

	if (pSymTch->nTotalTaughtTmpl<=0) {
		m_strSymSetErrMsg.Format("Symbol Not Taught!!!");
		return -1;
	}


	// Compute First Template Search Window
	rectTmpl = pSymTch->TchTmplInfo[pSymTch->nFirstTemplate].rectTeachWin;
	rectTmpl = CRect((int) (rectTmpl.left + pt2DPkgOffset.x),
					 (int) (rectTmpl.top + pt2DPkgOffset.y),
					 (int) (rectTmpl.right + pt2DPkgOffset.x),
					 (int) (rectTmpl.bottom + pt2DPkgOffset.y));
	rectTmpl.InflateRect(pSymParm->nXFirstTmplShiftTol,pSymParm->nYFirstTmplShiftTol);
	CheckRect(&rectTmpl,&pView->sizeImage);
	
	if (lDebugFlag&DEBUG_PRINT) {
		OutputDebugLog("1st Tmpl SrchWin sx:%d sy:%d ex:%d ey:%d",rectTmpl.left,
																  rectTmpl.top,
																  rectTmpl.right,
																  rectTmpl.bottom);
	}
	if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_STEP_MODE) {
		DrwRect(pView, &rectTmpl, &GreenPen);
	}
	if (bDebugThis)
		Timer.Reset();
		// Find First Template Using Logo Parameters

		nError = HostInspRotTmpl(pView,
							  pImgBufSrc,
							  pRefImgBufH,
						      pRefTmplBufH,
						      pSymTch->TchRoughTmplIdx[pSymTch->nFirstTemplate],
						      &(pSymParm->LogoPar),
						      &(pSymTch->TchTmplInfo[pSymTch->nFirstTemplate]),
						      &rectTmpl,
						      pSymTch->nTchThres,
						      pSymParm->nTmplContrastTol,
						      pSymParm->nMarkColor,
						      &(pSymRslt->TmplInspRslt[pSymTch->nFirstTemplate]),
							  lDebugFlag);
		
	if(lDebugFlag&DEBUG_TIME_EXT) {
		dTime = Timer.GetElapsedMSec();
		OutputDebugLog("Host Insp 1st Tmpl Time=%3.2f",dTime);
	}

	if (nError) {
		m_strSymSetErrMsg.Format("Error Processing 1st Template TemplID : %d",pSymTch->nFirstTemplate);
		return PROCESS_ERROR;
	}

	if (DEBUG_STEP_MODE&lDebugFlag) {
		DrwUpdate(pView);
		if(WaitResponse(pView))
			return USER_ABORT;
		DrwDel(pView);
	}

	if (pSymRslt->TmplInspRslt[pSymTch->nFirstTemplate].nResult!=TMPL_PASS) {
		m_nLastTmplErrorID = pSymTch->nFirstTemplate;
		m_nFailErrorCode = pSymRslt->TmplInspRslt[pSymTch->nFirstTemplate].nResult;
		return m_nFailErrorCode;
	}
	else {
		
		
	}
	if (pSymTch->nTotalTaughtTmpl==1) {
		return 0;
	}

	// Compute Last Template Search Window based on Found First Template and Package Location 
	CPoint pt1,pt2,ptFirstTmplOffset,ptMin,ptMax;
	CRect rectMin,rectMax,rectOffset;

	pt1 = pSymRslt->TmplInspRslt[pSymTch->nFirstTemplate].ptFndTmplPos;
	pt2 = pSymTch->TchTmplInfo[pSymTch->nFirstTemplate].rectTeachWin.TopLeft();
	ptFirstTmplOffset =  pt1 - pt2;
	
	rectTmpl = pSymTch->TchTmplInfo[pSymTch->nLastTemplate].rectTeachWin;
	if (lDebugFlag&DEBUG_PRINT) {
		OutputDebugLog("");
		OutputDebugLog("Last Tmpl TchPos sx:%d sy:%d ex:%d ey:%d",rectTmpl.left,
																  rectTmpl.top,
																  rectTmpl.right,
																  rectTmpl.bottom);
	}
	rectTmpl += ptFirstTmplOffset;
	rectTmpl.InflateRect(pSymParm->nXOtherTmplShiftTol,pSymParm->nYOtherTmplShiftTol);
	CheckRect(&rectTmpl,&pView->sizeImage);

	if (lDebugFlag&DEBUG_PRINT) {
		OutputDebugLog("Last Tmpl SrchWin sx:%d sy:%d ex:%d ey:%d",rectTmpl.left,
																   rectTmpl.top,
																   rectTmpl.right,
																   rectTmpl.bottom);
	}
	if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_STEP_MODE) {
		DrwRect(pView, &rectTmpl, &GreenPen);
	}
	if (bDebugThis)
		Timer.Reset();
	// Find Last Template
		nError = HostInspRotTmpl(pView,
							  pImgBufSrc,
							  pRefImgBufH,
							  pRefTmplBufH,
							  pSymTch->TchRoughTmplIdx[pSymTch->nLastTemplate],
							  &(pSymParm->TmplPar),
							  &(pSymTch->TchTmplInfo[pSymTch->nLastTemplate]),
							  &rectTmpl,
							  pSymTch->nTchThres,
							  pSymParm->nTmplContrastTol,
							  pSymParm->nMarkColor,
							  &(pSymRslt->TmplInspRslt[pSymTch->nLastTemplate]),
							  lDebugFlag);

		if(lDebugFlag&DEBUG_TIME_EXT) {
			dTime = Timer.GetElapsedMSec();
			OutputDebugLog("Host Insp Last Tmpl Time=%3.2f",dTime);
		}

	if (DEBUG_STEP_MODE&lDebugFlag) {
		DrwUpdate(pView);
		if(WaitResponse(pView))
			return USER_ABORT;
		DrwDel(pView);
	}

	if (nError) {
		m_strSymSetErrMsg.Format("Error Processing Last Template TemplID : %d",pSymTch->nLastTemplate);
		return -1;
	}

	if (pSymRslt->TmplInspRslt[pSymTch->nLastTemplate].nResult!=TMPL_PASS) {
		m_nLastTmplErrorID = pSymTch->nLastTemplate;
		m_nFailErrorCode = pSymRslt->TmplInspRslt[pSymTch->nLastTemplate].nResult;
		return 0;
	}
	else {
		
	
	}

	if (pSymTch->nTotalTaughtTmpl==2) {
		return 0;
	}

	pt1 = pSymRslt->TmplInspRslt[pSymTch->nLastTemplate].ptFndTmplPos;
	pt2 = pSymTch->TchTmplInfo[pSymTch->nLastTemplate].rectTeachWin.TopLeft();
	ptLastTmplOffset =  pt1 - pt2;

	ptAvgTmplOffset = CPoint((ptFirstTmplOffset.x + ptLastTmplOffset.x)/2, (ptFirstTmplOffset.y + ptLastTmplOffset.y)/2);
	// Inspecting the rest of the Templates
	CPoint ptTmplOffset;
	double dMarkAngle;
	CPoint ptFnd;

	ptFnd.x = pSymRslt->TmplInspRslt[pSymTch->nLastTemplate].ptFndTmplPos.x - 
			  pSymRslt->TmplInspRslt[pSymTch->nFirstTemplate].ptFndTmplPos.x;
	ptFnd.y = pSymRslt->TmplInspRslt[pSymTch->nLastTemplate].ptFndTmplPos.y - 
		   	  pSymRslt->TmplInspRslt[pSymTch->nFirstTemplate].ptFndTmplPos.y;

	dMarkAngle = atan2((double)ptFnd.y,(double)ptFnd.x);
	dMarkAngle = dMarkAngle - pSymTch->dTchAngle;
	if(dMarkAngle > PI/2)
		dMarkAngle -= PI;
	if(dMarkAngle < -PI/2)
		dMarkAngle += PI;
	pSymRslt->dAngleOffset = dMarkAngle;

	pt2DRef = pSymRslt->TmplInspRslt[pSymTch->nFirstTemplate].ptFndTmplPos;

	for (i=0;i<pSymTch->nTotalTaughtTmpl;i++) {
		if ((i==pSymTch->nFirstTemplate) || (i==pSymTch->nLastTemplate))
			continue;
		
		// Compute Search Window 
		rectTmpl = pSymTch->TchTmplInfo[i].rectTeachWin;
		if (lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("");
			OutputDebugLog("Tmpl[%d] TchPos sx:%d sy:%d ex:%d ey:%d",i,
																	 rectTmpl.left,
																	 rectTmpl.top,
																	 rectTmpl.right,
																	 rectTmpl.bottom);
		}
		rectTmpl += ptAvgTmplOffset;

		rectTmpl.InflateRect(pSymParm->nXOtherTmplShiftTol, pSymParm->nYOtherTmplShiftTol);
		CheckRect(&rectTmpl,&pView->sizeImage);

		if (lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("Tmpl%d SrchWin sx:%d sy:%d ex:%d ey:%d",i,
																	rectTmpl.left,
																	rectTmpl.top,
																	rectTmpl.right,
																	rectTmpl.bottom);
		}
		if (lDebugFlag&DEBUG_DRAW && lDebugFlag&DEBUG_STEP_MODE) {
			DrwRect(pView, &rectTmpl, &GreenPen);
		}
		if (bDebugThis)
			Timer.Reset();
		// Find Template
		nError = HostInspRotTmpl(pView,
							  pImgBufSrc,
							  pRefImgBufH,
							  pRefTmplBufH,
							  pSymTch->TchRoughTmplIdx[i],
							  &(pSymParm->TmplPar),
							  &(pSymTch->TchTmplInfo[i]),
							  &rectTmpl,
							  pSymTch->nTchThres,
							  pSymParm->nTmplContrastTol,
							  pSymParm->nMarkColor,
							  &(pSymRslt->TmplInspRslt[i]),
							  lDebugFlag);

		if(lDebugFlag&DEBUG_TIME_EXT) {
			dTime = Timer.GetElapsedMSec();
			OutputDebugLog("Host Insp Tmpl[%d] Time=%3.2f",i,dTime);
		}

		if (DEBUG_STEP_MODE&lDebugFlag) {
			DrwUpdate(pView);
			if(WaitResponse(pView))
				return USER_ABORT;
			DrwDel(pView);
		}

		if (nError) {
			m_strSymSetErrMsg.Format("Error Processing TemplID : %d",i);
			return -1;
		}
		if (pSymRslt->TmplInspRslt[i].nResult!=TMPL_PASS) {
			m_nLastTmplErrorID = i;
			m_nFailErrorCode = pSymRslt->TmplInspRslt[i].nResult;
			return 0;
		}
		else {
			////Biju
/*			pSymParm->bMaskTemplate = TRUE;
			if (pSymParm->bMaskTemplate) {
				//Mask Other Template
				rectTmpl = pSymTch->TchTmplInfo[i].rectTeachWin;
				rectTmpl.OffsetRect(pSymRslt->TmplInspRslt[i].ptFndTmplPos-pSymTch->TchTmplInfo[i].rectTeachWin.TopLeft());
				MaskTmpl(pView,
						 pImgBufSrc,
						 &rectTmpl,
						 pSymRslt->TmplInspRslt[i].nTmplInspThres,
						 nMedian,
						 pSymParm->nMarkColor);

			}
*/
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::HostCheckRotSymbolSet");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CMarkInspect::HostInspRotTmpl(IMAGEVIEW_INFO *pView,
							   CImageBuffer *pImgBufSrc,
						       CImageBuffer *pRefImg,
							   CImageBuffer *pRefTmpl,
							   int nIdx,
							   TMPL_INSP_PARM *pTmplInspParm,
							   TMPL_TCH_INFO *pTmplTchInfo,
							   CRect *prectSearch,
							   int nTchThres,
							   int nTmplContrastTol,
							   int nMarkColor,
							   TMPL_INSP_RSLT *pTmplInspRslt,
					    	   long lDebugFlag)
{
	int nError;
	CRect rect;
	CRect rcDeb0,rcDeb1;
	CRect rcDispDeb1;
	int nAcptScore;
	CImageBuffer ImgDebug;

	LOGPEN RedPen,
		   GreenPen,
		   BluePen;
	double dTime;
	CPerformanceTimer Timer;
	int nCormax;
	CRect rectDisp;
	CPoint ptCorrPos;
	CString strDebug;

	LOGPEN TextColorGreen,TextColorRed;
	CString strMsg;
	CPoint strPos;

try
{
	strPos.x = 300;
	strPos.y = 20;

	TextColorGreen.lopnColor = RGB(0,255,0);
	TextColorGreen.lopnStyle = PS_SOLID;
	TextColorGreen.lopnWidth.x = 10;
		
	TextColorRed.lopnColor = RGB(255,0,0);
	TextColorRed.lopnStyle = PS_SOLID;
	TextColorRed.lopnWidth.x = 10;

	RedPen.lopnColor = RGB(255,0,0);
	RedPen.lopnStyle = PS_SOLID;
	RedPen.lopnWidth.x = 1;

	GreenPen.lopnColor = RGB(0,255,0);
	GreenPen.lopnStyle = PS_SOLID;
	GreenPen.lopnWidth.x = 1;

	BluePen.lopnColor = RGB(0,0,255);
	BluePen.lopnStyle = PS_SOLID;
	BluePen.lopnWidth.x = 1;

	// Initialize Insp Rslt Data
	pTmplInspRslt->bHasCompared = FALSE;
	pTmplInspRslt->dExcessBlobSize = 0;
	pTmplInspRslt->dMissingBlobSize = 0;
	pTmplInspRslt->nFndScore = 0;
	pTmplInspRslt->nResult = TMPL_FAIL_NOTFOUND;
	pTmplInspRslt->nTmplInspThres = nTchThres;
	pTmplInspRslt->ptFndExcessBlobPos = CPoint(0,0);
	pTmplInspRslt->ptFndMissingBlobPos = CPoint(0,0);
	pTmplInspRslt->ptFndTmplPos = CPoint(0,0);


	/////////////////////////////////////////////////////////////////
	// Set this Flag to TRUE if you to do debug internal timing
	// for this function
	BOOL bDebugThis=lDebugFlag;
	/////////////////////////////////////////////////////////////////
	if (bDebugThis) {
		Timer.Reset();
	}

	if (pTmplInspParm->bGrossCheck)
		nAcptScore = pTmplInspParm->nRejectScore;
	else
		nAcptScore = pTmplInspParm->nAcceptScore;

	if (DEBUG_STEP_MODE&lDebugFlag) {
		// Display Template at top left hand corner
		rcDeb1 = rcDeb0 = CRect(CPoint(0,0),pImgBufSrc->GetImgSize());

		nError = ImgDebug.Allocate(AbsSize(rcDeb1.Size()));
		if (nError == -1) {
			ASSERT(!"Fail To Allocate Image in InspTmpl()");
			goto ErrorAbort;
		}
		nError = ImgCopy(pImgBufSrc,&rcDeb0,&ImgDebug,&rcDeb1);

		rcDeb0 = pTmplTchInfo->HostMarkTmpl.NonSampling.rCImageBuffer;
		rcDeb1 = CRect(0,0,pTmplTchInfo->rectTeachWin.Width(),pTmplTchInfo->rectTeachWin.Height());
		nError = ImgCopy(&pTmplTchInfo->HostMarkTmpl.NonSampling.ImgBuf,
						 &rcDeb0,
						 &ImgDebug,
						 &rcDeb1);

		strDebug = "Mark Template";
	
		nError = DrwDebugImage(pView, pImgBufSrc, &ImgDebug, &rcDeb1, &CPoint(0,0), &strDebug);
		if(nError != 0)
			goto ErrorAbort;
	}
 

	nError = m_GrayScaleCorr.Correlate(pImgBufSrc,
									   prectSearch, 
									   &pTmplTchInfo->HostMarkTmpl,
									   nAcptScore,
									   pTmplInspParm->nRejectScore,
									   &nCormax,
									   &ptCorrPos);

	if (nError == -1) {
		OutputDebugLog("m_GrayScaleCorr.Correlate() Error");
		goto ErrorAbort;
	}

	pTmplInspRslt->nFndScore = nCormax;
	pTmplInspRslt->ptFndTmplPos = ptCorrPos;


	if(lDebugFlag&DEBUG_TIME_EXT) {
		dTime = Timer.GetElapsedMSec();
		OutputDebugLog("Correlate () Time=%3.2f",dTime);
	}

	if(lDebugFlag&DEBUG_PRINT) {
		OutputDebugLog("Corr Score=%d",pTmplInspRslt->nFndScore);
	}
	if(pTmplInspRslt->nFndScore == 0) {
		if (lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("Found Score[0] :: TEMPLATE NOT FOUND");
		}
		if (lDebugFlag&DEBUG_DRAW) {
			rect = *prectSearch;
			DrwRect(pView, &rect, &RedPen);
		}
		pTmplInspRslt->nResult = TMPL_FAIL_NOTFOUND;
	}
	else if(pTmplInspRslt->nFndScore < pTmplInspParm->nRejectScore) {
		if (lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("Found Score[%d] < RejectScore[%d] :: FAIL TEMPLATE ",pTmplInspRslt->nFndScore,pTmplInspParm->nRejectScore);
		}
		if (lDebugFlag&DEBUG_DRAW) {
			rect = CRect(pTmplInspRslt->ptFndTmplPos,pTmplTchInfo->rectTeachWin.Size());
			DrwRect(pView, &rect, &RedPen);
		}
		pTmplInspRslt->nResult = TMPL_FAIL_CORR;
	}
	else if (pTmplInspRslt->nFndScore >= nAcptScore) {
		if (lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("Found Score[%d] >= AcceptScore[%d] :: PASS ",pTmplInspRslt->nFndScore,nAcptScore);
		}
		if (lDebugFlag&DEBUG_STEP_MODE) {
			rect = CRect(pTmplInspRslt->ptFndTmplPos,pTmplTchInfo->rectTeachWin.Size());
			DrwRect(pView, &rect, &GreenPen);
		}
		pTmplInspRslt->nResult = TMPL_PASS;
	}

	if (DEBUG_STEP_MODE&lDebugFlag) {
		strMsg.Format("Found Score = [%d] ; Accept Score = [%d]",pTmplInspRslt->nFndScore, nAcptScore);
		DrwStr(pView, &strMsg, &strPos, &TextColorGreen, 20);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMarkInspect::HostInspRotTmpl");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	if (ImgDebug.IsAllocated())
		FreeImgBuf(&ImgDebug);
	DrwDel(pView);
	return -1;
}
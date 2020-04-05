#include "stdAfx.h"
#include "Calibrate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCalibrate::CCalibrate()
{
}

CCalibrate::~CCalibrate()
{
}

int CCalibrate::TapeCalibrate(IMAGEVIEW_INFO *pView,
							  CImageBuffer *pImgBufSrc,
							  long lDebugFlag)
{

	int i;
	int	nError;
	int	nSamples;

	double dDist;
	double dSum;
	double dSumSquare;
	double dAngle;

	CLine2D Side0Line;	// Top Line
	CLine2D Side1Line;	// Bottom Line
	CLine2D Side2Line;	// Left Line
	CLine2D Side3Line;	// Right Line
	
	CGridEdge Side0Edge;
	CGridEdge Side1Edge;

	GRD_EDGE_FIND_PARM EdgeParm;

	m_strErrorMsg.Format("Pass");

	CRect *CalBlockRect = &(pView->PixCalInfo.rectTeachCalBlock);
	// Common Values
	EdgeParm.nGroupFactor = 1;
	EdgeParm.nSkipFactor = 3;
	EdgeParm.nMethod = LEVEL;
	EdgeParm.nThreshold = 50;
	
	// Compute TopBottom Calibration
	dSum = 0;
	dSumSquare = 0;
	nSamples = 0;
	dAngle = 0;
	// Find Top Side Edges of Cal Block
	EdgeParm.nDirection = FORWARD;
	EdgeParm.nGradient = POS;
	EdgeParm.nOrientation = COLUMN;
	EdgeParm.rectROI = CRect(CPoint(CalBlockRect->left + CalBlockRect->Width()/4,CalBlockRect->top - 15),
							 CSize(CalBlockRect->Width()/2,30));

	nError = Side0Edge.GetGridEdge(pView,pImgBufSrc,&EdgeParm,lDebugFlag);
	if (nError) {
		m_strErrorMsg.Format("Failed Top Side Calibration");
		return -1;
	}
	nError = Side0Edge.GetLine(&EdgeParm,&Side0Edge.m_EdgeFindRslt,&Side0Line);
	if (nError) {
		m_strErrorMsg.Format("Failed Top Side Calibration");
		return -1;
	}
	if (DEBUG_STEP_MODE&lDebugFlag) {
		DrwUpdate(pView);
		if(WaitResponse(pView)) {
			m_strErrorMsg.Format("Failed Top Side Calibration");
			return -1;
		}
		DrwDel(pView);
	}


	// Find Bottom Side Edges of Cal Block
	EdgeParm.nDirection = BACKWARD;
	EdgeParm.nGradient = POS;
	EdgeParm.nOrientation = COLUMN;
	EdgeParm.rectROI = CRect(CPoint(CalBlockRect->left + CalBlockRect->Width()/4,CalBlockRect->bottom - 15),
							 CSize(CalBlockRect->Width()/2,30));

	nError = Side1Edge.GetGridEdge(pView,pImgBufSrc,&EdgeParm,lDebugFlag);
	if (nError) {
		m_strErrorMsg.Format("Failed Bottom Side Calibration");
		return -2;
	}
	nError = Side1Edge.GetLine(&EdgeParm,&Side1Edge.m_EdgeFindRslt,&Side1Line);
	if (nError) {
		m_strErrorMsg.Format("Failed Bottom Side Calibration");
		return -2;
	}
	if (DEBUG_STEP_MODE&lDebugFlag) {
		DrwUpdate(pView);
		if(WaitResponse(pView)) {
			m_strErrorMsg.Format("Failed Bottom Side Calibration");
			return -2;
		}
		DrwDel(pView);
	}

	if (Side0Edge.m_EdgeFindRslt.nLineCount!=Side1Edge.m_EdgeFindRslt.nLineCount) {
		m_strErrorMsg.Format("Top Scan Line Count != Bottom Scan Line Count");
		return -5;
	}
	// Compute Distance
	for(i=0;i<Side0Edge.m_EdgeFindRslt.nLineCount;i++) {
		if (Side0Edge.m_EdgeFindRslt.dFndEdgePos[i][0] > 0 &&
			Side1Edge.m_EdgeFindRslt.dFndEdgePos[i][0]>0) {
			dDist = Side1Edge.m_EdgeFindRslt.dFndEdgePos[i][1] - Side0Edge.m_EdgeFindRslt.dFndEdgePos[i][1];
			dSum += dDist;
			dSumSquare += dDist * dDist;
			nSamples++;
		}
	}
	if (nSamples>0) {
		dSum/=nSamples;
		dSumSquare/=nSamples;
		if (nSamples>1) {
			dAngle=(atan2(Side0Line.GetDirection().y,Side0Line.GetDirection().x)+atan2(Side1Line.GetDirection().y,Side1Line.GetDirection().x))/2.0;
			dSum=dSum*cos(dAngle);
			dSum=fabs(dSum);
		}
		pView->PixCalInfo.dYPixSize = pView->PixCalInfo.dYCalBlockSize/dSum;
		if(lDebugFlag&DEBUG_PRINT)
			OutputDebugLog("Vertical Angle=%3.3f PixSize=%3.3f",dAngle*180/PI,pView->PixCalInfo.dYPixSize);
	}

	// Compute LeftRight Calibration
	dSum = 0;
	dSumSquare = 0;
	nSamples = 0;
	dAngle = 0;

	// Find Left Side Edges of Cal Block
	EdgeParm.nDirection = FORWARD;
	EdgeParm.nGradient = POS;
	EdgeParm.nOrientation = ROW;
	EdgeParm.rectROI = CRect(CPoint(CalBlockRect->left - 15,CalBlockRect->top + CalBlockRect->Width()/4),
							 CSize(30,CalBlockRect->Height()/2));

	nError = Side0Edge.GetGridEdge(pView,pImgBufSrc,&EdgeParm,lDebugFlag);
	if (nError) {
		m_strErrorMsg.Format("Failed Left Side Calibration");
		return -3;
	}
	nError = Side0Edge.GetLine(&EdgeParm,&Side0Edge.m_EdgeFindRslt,&Side2Line);
	if (nError) {
		m_strErrorMsg.Format("Failed Left Side Calibration");
		return -3;
	}

	if (DEBUG_STEP_MODE&lDebugFlag) {
		DrwUpdate(pView);
		if(WaitResponse(pView)) {
			m_strErrorMsg.Format("Failed Left Side Calibration");
			return -3;
		}
		DrwDel(pView);
	}
	// Find Right Side Edges of Cal Block
	EdgeParm.nDirection = BACKWARD;
	EdgeParm.nGradient = POS;
	EdgeParm.nOrientation = ROW;
	EdgeParm.rectROI = CRect(CPoint(CalBlockRect->right - 15,CalBlockRect->top + CalBlockRect->Width()/4),
							 CSize(30,CalBlockRect->Height()/2));

	nError = Side1Edge.GetGridEdge(pView,pImgBufSrc,&EdgeParm,lDebugFlag);
	if (nError) {
		m_strErrorMsg.Format("Failed Right Side Calibration");
		return -4;
	}
	nError = Side1Edge.GetLine(&EdgeParm,&Side1Edge.m_EdgeFindRslt,&Side3Line);
	if (nError) {
		m_strErrorMsg.Format("Failed Right Side Calibration");
		return -4;
	}

	if (DEBUG_STEP_MODE&lDebugFlag) {
		DrwUpdate(pView);
		if(WaitResponse(pView)) {
			m_strErrorMsg.Format("Failed Right Side Calibration");
			return -4;
		}
		DrwDel(pView);
	}

	if (Side0Edge.m_EdgeFindRslt.nLineCount!=Side1Edge.m_EdgeFindRslt.nLineCount) {
		m_strErrorMsg.Format("Left Scan Line Count != Right Scan Line Count");
		return -5;
	}

	// Compute Distance
	for(i=0;i<Side0Edge.m_EdgeFindRslt.nLineCount;i++) {
		if (Side0Edge.m_EdgeFindRslt.dFndEdgePos[i][0] > 0 &&
			Side1Edge.m_EdgeFindRslt.dFndEdgePos[i][0]>0) {
			dDist = Side1Edge.m_EdgeFindRslt.dFndEdgePos[i][1] - Side0Edge.m_EdgeFindRslt.dFndEdgePos[i][1];
			dSum += dDist;
			dSumSquare += dDist * dDist;
			nSamples++;
		}
	}
	if (nSamples>0) {
		dSum/=nSamples;
		dSumSquare/=nSamples;
		if (nSamples>1) {

			///////////////////////////////////////////////////////////////////
			//SBT 6/8/01 To take care of -ve & +ve angle returned by atan2
			double dAngleLeft, dAngleRight;
			double dSinAngle;
			dAngleLeft = atan2(Side2Line.GetDirection().y,Side2Line.GetDirection().x);
			dAngleRight = atan2(Side3Line.GetDirection().y,Side3Line.GetDirection().x);
			dSinAngle = (fabs(sin(dAngleLeft)) + fabs(sin(dAngleRight)))/2;
			dSum=dSum*dSinAngle;
			///////////////////////////////////////////////////////////////////

			/*
			dAngle=(atan2(Side2Line.Direction.y,Side2Line.Direction.x)+atan2(Side3Line.Direction.y,Side3Line.Direction.x))/2.0;
			dSum=dSum*sin(dAngle);
			dSum=fabs(dSum);
			*/
		}
		pView->PixCalInfo.dXPixSize = pView->PixCalInfo.dXCalBlockSize/dSum;
		if(lDebugFlag&DEBUG_PRINT)
			OutputDebugLog("Horizontal Angle=%3.3f PixSize=%3.3f",dAngle*180/PI,pView->PixCalInfo.dXPixSize);
	}

	if (lDebugFlag&DEBUG_DRAW) {
		DrawTapeCalibrate(pView,
						  &Side0Line,
						  &Side1Line,
						  &Side2Line,
						  &Side3Line);
	}

	return 0;
}


int CCalibrate::DeviceCalibrate(IMAGEVIEW_INFO *pView,
							  CImageBuffer *pImgBufSrc,
							  long lDebugFlag)
{

	m_strErrorMsg.Format("Pass");

	CRect CalBlockRect = &(pView->PixCalInfo.rectTeachCalBlock);
	if((CalBlockRect.Width() > 0) && (CalBlockRect.Height()>0)) {
		pView->PixCalInfo.dXPixSize = pView->PixCalInfo.dXCalBlockSize/CalBlockRect.Width();
		pView->PixCalInfo.dYPixSize = pView->PixCalInfo.dYCalBlockSize/CalBlockRect.Height();
		if(lDebugFlag&DEBUG_PRINT) {
			OutputDebugLog("Horizontal PixSize=%3.3f",pView->PixCalInfo.dXPixSize);
			OutputDebugLog("Vertical PixSize=%3.3f",pView->PixCalInfo.dYPixSize);
		}
		return 0;
	}
	else
		return -1;
}

int CCalibrate::DrawTapeCalibrate(IMAGEVIEW_INFO *pView,
								  CLine2D *TopLine,
								  CLine2D *BotLine,
								  CLine2D *LftLine,
								  CLine2D *RhtLine)
{
	CPoint2D ptTopLft,
			 ptTopRht,
			 ptBotLft,
			 ptBotRht;

	LOGPEN PenInfo;

	CPoint point1,
		   point2;
	int nError;

	PenInfo.lopnColor = RGB(0,255,0);
	PenInfo.lopnStyle = PS_SOLID;
	PenInfo.lopnWidth.x = 1;


//	ptTopLft = LftLine->GetIntersection(*TopLine);
	nError = LftLine->GetIntercept(TopLine, &ptTopLft);
	if(nError==-1) {
		ASSERT(!"LftLine->GetIntercept() Error");
		goto ErrorAbort;
	}

//	ptTopRht = RhtLine->GetIntersection(*TopLine);
	nError = RhtLine->GetIntercept(TopLine, &ptTopRht);
	if(nError==-1) {
		ASSERT(!"RhtLine->GetIntercept() Error");
		goto ErrorAbort;
	}

//	ptBotLft = LftLine->GetIntersection(*BotLine);
	nError = LftLine->GetIntercept(BotLine, &ptBotLft);
	if(nError==-1) {
		ASSERT(!"LftLine->GetIntercept() Error");
		goto ErrorAbort;
	}
//	ptBotRht = RhtLine->GetIntersection(*BotLine);
	nError = RhtLine->GetIntercept(BotLine, &ptBotRht);
	if(nError==-1) {
		ASSERT(!"RhtLine->GetIntercept() Error");
		goto ErrorAbort;
	}

	point1.x = (int)ptTopLft.x;
	point1.y = (int)ptTopLft.y;
	point2.x = (int)ptTopRht.x;
	point2.y = (int)ptTopRht.y;
	DrwLine(pView,&point1,&point2,&PenInfo);

	point1.x = (int)ptTopLft.x;
	point1.y = (int)ptTopLft.y;
	point2.x = (int)ptBotLft.x;
	point2.y = (int)ptBotLft.y;
	DrwLine(pView,&point1,&point2,&PenInfo);
	
	point1.x = (int)ptTopRht.x;
	point1.y = (int)ptTopRht.y;
	point2.x = (int)ptBotRht.x;
	point2.y = (int)ptBotRht.y;
	DrwLine(pView,&point1,&point2,&PenInfo);

	point1.x = (int)ptBotLft.x;
	point1.y = (int)ptBotLft.y;
	point2.x = (int)ptBotRht.x;
	point2.y = (int)ptBotRht.y;
	DrwLine(pView,&point1,&point2,&PenInfo);

	return 0;
ErrorAbort:
	return -1;
}


int CCalibrate::LoadCalData(CString strDirectoryName,
							CString strFileName,
							PIX_CAL_INFO *pPixCalInfo)
{
	CIniFile iniFile;
	CString strDataFileName;

	////strDataFileName = strDirectoryName + "\\" + strFileName + ".cld";
	strDataFileName = strDirectoryName + "\\CalibrationData\\" + strFileName + ".cld";
	iniFile.SetPath(strDataFileName);

	if (iniFile.ReadFile()) {
		// File Exist
		GetCalData(&iniFile,pPixCalInfo);
	}
	else {
		// File Not Found
		OutputDebugLog("%s not found. Default values assumed",strDataFileName);
		SetCalData(&iniFile,pPixCalInfo);
		iniFile.WriteFile();
	}
	return 0;
}

int CCalibrate::SaveCalData(CString strDirectoryName,
							 CString strFileName,
							 PIX_CAL_INFO *pPixCalInfo)
{
	CIniFile iniFile;
	CString strDataFileName;

	///strDataFileName = strDirectoryName + "\\" + strFileName + ".cld";
	strDataFileName = strDirectoryName + "\\CalibrationData\\" + strFileName + ".cld";

	iniFile.SetPath(strDataFileName);
	SetCalData(&iniFile,pPixCalInfo);
	iniFile.WriteFile();
	return 0;
}

int CCalibrate::GetCalData(CIniFile *ptrIniFile,PIX_CAL_INFO *pPixCalInfo)
{
	pPixCalInfo->dXCalBlockSize = ptrIniFile->GetDouble("Calibration Block Size","XSize",100.0);
	pPixCalInfo->dYCalBlockSize = ptrIniFile->GetDouble("Calibration Block Size","YSize",100.0);


	pPixCalInfo->dXCalDotPitch = ptrIniFile->GetDouble("Calibration Dot Pattern Pitch","XPitch",100.0);
	pPixCalInfo->dYCalDotPitch = ptrIniFile->GetDouble("Calibration Dot Pattern Pitch","YPitch",100.0);

	pPixCalInfo->dXPixSize = ptrIniFile->GetDouble("Pixel Size","XSize",0.0302);
	pPixCalInfo->dYPixSize = ptrIniFile->GetDouble("Pixel Size","YSize",0.0302);

	pPixCalInfo->nCalUnitType = ptrIniFile->GetInteger("Unit Type","Unit",MM);

	pPixCalInfo->rectTeachCalBlock = ptrIniFile->GetCRect("Teach Rectangle","CalRect",CRect(10,10,100,100));

	return 0;	
}

int CCalibrate::SetCalData(CIniFile *ptrIniFile,PIX_CAL_INFO *pPixCalInfo)
{
	ptrIniFile->GetDouble("Calibration Block Size","XSize",pPixCalInfo->dXCalBlockSize);
	ptrIniFile->GetDouble("Calibration Block Size","YSize",pPixCalInfo->dYCalBlockSize);

	ptrIniFile->GetDouble("Calibration Dot Pattern Pitch","XPitch",pPixCalInfo->dXCalDotPitch);
	ptrIniFile->GetDouble("Calibration Dot Pattern Pitch","YPitch",pPixCalInfo->dYCalDotPitch);

	ptrIniFile->GetDouble("Pixel Size","XSize",pPixCalInfo->dXPixSize);
	ptrIniFile->GetDouble("Pixel Size","YSize",pPixCalInfo->dYPixSize);

	ptrIniFile->GetInteger("Unit Type","Unit",pPixCalInfo->nCalUnitType);

	ptrIniFile->GetCRect("Teach Rectangle","CalRect",pPixCalInfo->rectTeachCalBlock);

	return 0;
}


/// CInspectionCore: Vision Inspection base class
/// Udupa; Feb'2015

#include "StdAfx.h"
#include "InspectionCore.h"
#include "FCI.cpp"
#include "AppDef.h"
#include "PerformanceTimer.h"
#include "DebugLog.h"
#include "MathAlgo.h"


CTeachInspectData::CTeachInspectData()
{
}

CPVIDefect::CPVIDefect()
{
	m_nID = 0;
	m_nErrorCodeID = 0;
	strDefectName = "";

	bEnable = FALSE;
	nDefectCharacteristics = 0;////0-Black; 1-White;
	m_nSize = 3000;
	nContrast = 80;
	nMinLength = 275;
	nMinSquareSize = 110; 
	nMinCount = 1;
};

CPVIInspectionSetup::~CPVIInspectionSetup()
{
	if(m_pPviDefect!=NULL)
		delete[] m_pPviDefect;
}

CPVIInspectionSetup::CPVIInspectionSetup()
{
	bEnable = FALSE;
	//bNonUniformSurface = TRUE;
	//nDeviceMaskThickness = 10;
	nDeviceMaskNumber = 0;
	nDeviceAreaType = RECTANGLE;
	nDefectCount = 0;
	dDeviceEdgeWidth = 0;
	dDeviceEdgeOffset = 0;

	bEdgeEnable = FALSE;
	nMinEdgeContrast = 60;
	nMaxEdgeContrast = 255;
	dEdgeRegionOpen = 28;
	dEdgeWidth = 0;
	dEdgeOffset = 0;

	bPVIRecheckEnable = FALSE;
	nMinGVDiff = 10;
	nIntensity = 40;

	bPVISlotConnectionCheckEnable = FALSE;
	nNumOfSlot = 0;
	bPVISlotConnectionCheckTopDie = FALSE;
	bPVISlotConnectionCheckBottomDie = FALSE;
	bPVISlotConnectionCheckLeftDie = FALSE;
	bPVISlotConnectionCheckRightDie = FALSE;

	m_pPviDefect = new CPVIDefect[PVI_DEFECT_COUNT];
	for(int i=0;i<10;i++)
		nDeviceMaskArea[i]=RECTANGLE;
	m_rectPviArea = CRect(0,0,0,0);//CRect(100,100,300,300);
	for(int j=0; j<10; j++)
		m_rectPviMaskRegions[j]= CRect(0,0,0,0);//CRect(100,100,300,300);
	for (int j = 0; j<10; j++)
		m_rectPviSlot[j] = CRect(0, 0, 0, 0);

	nPolygonPointNumber = 3;
	for (int j = 0; j < 20; j++)
		m_polygonPviArea[j] = CPoint(700 + 50 * j, 700);
}

CCoverLayer::CCoverLayer() : CParameterCore("Cover Layer", "CoverLayer", -COVER_LAYER_INSP)
{
	bEnable = FALSE;

	for(int i=0; i<5; i++)
	{
		nContrast[i] = 128;
		nCoverLayerLength[i] = 5500;
		nCoverLayerHeight[i] = 165;
		dGapWidthTolerance[i] = 25;
		dGapHeightTolerance[i] = 25;
		dMaskSize[i] = 25;
	}
}

CCoverLayerAlignment::CCoverLayerAlignment() : CParameterCore("Cover Layer Alignment", "CoverLayerAlignment", -COVER_LAYER_ALIGNMENT)
{
	bEnablePointer = FALSE;

	for (int i = 0; i<6; i++)
	{
		nCoverLayerDirection[i] = VERTICAL;
		dToleranceDist[i] = 110;
	}
}

CEncap::CEncap() : CParameterCore("Encap", "Encap", -ENCAP_INSUFFICIENT)
{
	bEnable = FALSE;
	bEnableEncapSurface = FALSE;

	dErosionOffset = 18;
	dMaxLayOnAllowedPercentage = 50;

	nMinContrastCrack = 50;
	nMinSizeCrack = 20;
	nMinLengthCrack = 200;
	nMinSquareSizeCrack = 10;

	nMinContrastBH = 50;
	nMinSizeBH = 20;
	dMinCircularityBH = 0.5;
	nMinLengthBH = 10;
	nMinSquareSizeBH = 12;
	nMinGrayMeanBH = 100;

	nMinContrastCT = 50;
	nMinSizeCT = 20;
	dMaxCircularityCT = 0.5;
	nMinLengthCT = 10;
	nMinSquareSizeCT = 12;

	//nImageIndex = 2;
	//nMinContrast = 50;
	//nMaxContrast = 255;
}

CSlot::CSlot() : CParameterCore("Slot", "Slot", -SLOT)
{
	 bEnable = FALSE;
	 for (int k = 0; k < 5; k++) {
		 bEnableCheckSurface[k] = FALSE;
		 nEdgeOffset[k] = 0;
		 nMinMeanGVDiffSlot[k] = 50;
		 nMinContrast[k] = 80;
		 nMinSize[k] = 200;
		 nMinLength[k]= 100;
		 nMinSquareSize[k] = 5;
		 nMinCount[k] = 1;
	 }


	//nImageIndex = 2;
	//nMinContrast = 50;
	//nMaxContrast = 255;
}

//CDieInsp::CDieInsp() : CParameterCore("Die", "Die", -DIE_INSP)
//{
//	bEnable = TRUE;
//}

CTiltInsp::CTiltInsp() : CParameterCore("Tilt", "Tile", -TILT_INSP)
{
	bEnable = FALSE;
	//nImageIndex = 3;
	nContrast = 0;
	nToleranceSide = 100;
	nAngleOfSideView = 40;
	nDistanceForTiltInsp = 20;

	//nRegionChar = 1;	//// 0 - Dark; 1- Light;
	//nRegionHeight = 80;
	//nMeanGreyValue = 200;
}

CDieEdge::CDieEdge() : CParameterCore("Die Edge", "DieEdge", -DIE_EDGE)
{
	bEnable = FALSE;
	nDefectCharacteristics = 0;////0-Black; 1-White;
	m_nSize = 3000;
	nContrast = 80;
	nMinLength = 275;
	nMinSquareSize = 110;
	nMinCount = 1;
	dOffset = 0;
	dWidth = 0;
}
CEncapMagnus::CEncapMagnus() : CParameterCore("Encap Magnus", "EncapMagnus", -ENCAP_MAGNUS)
{
	bEnable = FALSE;
	Thresh[0]=40;
	Thresh[1] = 220;
	Crop_Expand[0]=220;
	Crop_Expand[1] = 220;
	Dilate_Kernel[0] = 7;
	Dilate_Kernel[1] = 1;
	Opening_Kernel[0]=1;
	Opening_Kernel[1]=7;
	Crop_Smooth=151;
	Area_Object=999999;
}

CTrainingData::CTrainingData()
{
	
	//nLensRadius               = 878;//1250;
	//nCuvetteRadiusRa          = 1350;
	//nCuvetteRadiusRb          = 1350; 
	//nCriticalOpticsZoneRadius = 150;

// Device Location //
	bEnableAutoFocusSequence = FALSE;
	bDeviceLocationEnable = FALSE;//TRUE;
	nNoOfLocationTeachRegion = 1;
	nMinDeviceContrast = 50;
	dDeviceLocWidth = 5500;
	dDeviceLocHeight = 2750;
	dMaskSize = 360;
	bSobelAmp = FALSE;
	m_rectDeviceLoc = CRect(0, 0, 0, 0);
	m_rectAutoFocusROI = CRect(0, 0, 0, 0);

	nDeviceReferencePointType = TOP_LEFT;

	bDeviceEdgeDetectionEnable = FALSE;
	nDeviceHorizontalContrast = 40;
	nDeviceHorizontalDirection = TOP_TO_BOTTOM;
	nDeviceVerticalContrast = 40;
	nDeviceVerticalDirection = LEFT_TO_RIGHT;
	m_rectDeviceHorizontalLineLoc = CRect(0, 0, 0, 0);
	m_rectDeviceVerticalLineLoc = CRect(0, 0, 0, 0);

	bUseSmoothingForEdgeDetection = FALSE;

// Dattam/Pattern Search //
	bDattamSearchEnable = FALSE;//TRUE;
	dDattamSearchAreaX = 100.0;
	dDattamSearchAreaY = 100.0;
	nDattamSearchMaxRotAngle = 10;
	nDattamSearchNoOfDattams = 2;
	dDattamSearchMinAcceptanceScore = 60;// 0.6;
	for(int nRectDattam=0; nRectDattam<4; nRectDattam++)
		m_rectDattamSearch[nRectDattam] = CRect(0,0,0,0);

// Region Search //
	/*bRegionSearchEnable = TRUE;
	nRegionSearchNoOfRegions = 2;
	for(int nRectRgn=0; nRectRgn<10; nRectRgn++)
		m_rectRegionSearch[nRectRgn] = CRect(0,0,0,0);*/

// Encap region //
	bEnableEncap = FALSE;
	nEncapMinContrast = 50;
	nEncapMaxContrast = 255;
	for (int k = 0; k < 3; k++)
	{ 
		m_rectEncap[k] = CRect(0, 0, 0, 0);
		m_nRectWidth[k] = 0;
		m_nRectHeight[k] = 0;
	}
		
	nEncapTracingMaskNumber = 0;
	for (int k = 0; k < 10; k++) {
		nEncapTracingMaskType[k] = RECTANGLE;
		nEncapTracingPolygonPointNumber[k] = 3;
		for (int j = 0; j < 20; j++) {
			m_EncapTracingPolygonPointArea[k][j] = CPoint(700 + 50 * j, 700);
		}
	}
	for (int k = 0; k < 10; k++) {
		m_rectMaskEncapTracing[k] = CRect(0, 0, 0, 0);
	}

	nEncapSurfaceMaskNumber = 0;
	for (int k = 0; k < 10; k++) {
		m_rectMaskEncapSurface[k] = CRect(0, 0, 0, 0);
	}

// cover layer //
	bEnableCoverLayer = FALSE;
	
	//bEnableCLDevLoc = TRUE;
	/*nCLAboveCloseDistance = 180;
	nCLAboveFarDistance = 600;
	nCLBelowCloseDistance = 180;
	nCLBelowFarDistance = 600;
	nCLSideCloseDistance = 220;
	nCLSideFarDistance = 450;*/

	nCLDatum = 2;
	nCLInspRegion = 3;
	bEnableCLDatum = FALSE;
	nCLMaxRotationAngle = 10;
	nCLPatternSearchX = 550;
	nCLPatternSearchY = 280;
	nCLMinAcceptanceScore =	65;//0.65
	/*nCLDatumAboveCloseDistance = 180;
	nCLDatumAboveFarDistance = 600;
	nCLDatumBelowCloseDistance = 180;
	nCLDatumBelowFarDistance = 600;
	nCLDatumSideCloseDistance = 220;
	nCLDatumSideFarDistance = 450;*/
	for (int i = 0; i < 2; i++)
		m_rectCLDatum[i] = CRect(0, 0, 0, 0);
	for (int j = 0; j < 5; j++)
		m_rectCoverLayer[j] = CRect(0, 0, 0, 0);

// Cover Layer Alignment //
	bEnableCLAlignmentPointer = FALSE;
	nPointer = 1;
	nCLAP_MaxRotationAngle = 10;
	nCLAP_PatternSearchX = 165;
	nCLAP_PatternSearchY = 165;
	nCLAP_MinAcceptanceScore = 65;
	bEnableManualPointer = FALSE;
	for (int k = 0; k < 6; k++)
		m_rectPointer[k] = CRect(0, 0, 0, 0);
//Slot
	bEnableSlot = FALSE;
	nNumOfSlot = 3;
	nMinIntensitySlot = 85;
	nContrastEdgeSlot = 10;
	for (int k = 0; k < 5; k++) {
		m_rectSlot[k] = CRect(0, 0, 0, 0);
		m_rectSlotLimit[k] = CRect(0, 0, 0, 0);

		for (int j = 0; j < 20; j++) {
			m_SlotPolygonPoint[k][j] = CPoint(700 + 50 * j, 700);
			m_SlotLimitPolygonPoint[k][j] = CPoint(700 + 50 * j, 700);
		}
	}
	nSlotAreaType = RECTANGLE;
	nSlotPolygonPointCount = 4;

	nSlotLimitAreaType = RECTANGLE;
	nSlotLimitPolygonPointCount = 4;

	nSlotMaskNumber = 0;
	for (int k = 0; k < 10; k++) {
		nSlotMaskType[k] = RECTANGLE;
		nSlotMaskPolygonPointCount[k] = 3;
		for (int j = 0; j < 20; j++) {
			m_SlotMaskPolygonPoint[k][j] = CPoint(700 + 50 * j, 700);
		}
	}
	for (int k = 0; k < 10; k++) {
		m_rectMaskSlot[k] = CRect(0, 0, 0, 0);
	}
	
	//////Bottom encap magnus 
	bEnable_EncapManus = FALSE;
	nThreshMin_EncapManus = 140;
	nThreshMax_EncapManus = 255;
	nThreshMin_Black_EncapManus=0;
	nThreshMax_Black_EncapManus=35;
	nThreshMin_White_EncapManus=220;
	nThreshMax_White_EncapManus=255;

	for (int i = 0; i <= 1; i++)
	{
		nCrop_ExpandLeft_magnus[i] = 200;
		nCrop_ExpandRight_magnus[i] = 200;
		nCrop_ExpandHeight_magnus[i] = 200;
		nCrop_Smooth_EncapManus[i] = 30;

		nCrop_RemoveBLLeft_magnus[i] = 200;
		nCrop_RemoveBLRight_magnus[i] = 200;
		nCrop_RemoveBLHeight_magnus[i] = 30;
	}

	hRect_DeviceLocationEncap_magnus = CRect(0,0,0,0);
	hRect_EncapLocation_magnus = CRect(0, 0, 0, 0);
	hRect_CropSmoothEncap_magnus = CRect(0, 0, 0, 0);	
	hRect_CropNoSmoothEncap_magnus = CRect(0, 0, 0, 0);
	hRect_CropRemoveBlackLine_magnus = CRect(0, 0, 0, 0);


	nDilateX_EncapManus = 1;
	nDilateY_EncapManus = 10;

	nOpeningX_EncapManus = 10;
	nOpeningY_EncapManus = 1;
	nValue_OpeningCircle_magnus = 10;
	nValue_OpeningCircleCrop_magnus = 10;

	nValue_Smooth_EncapMagnus = 75;
	nArea_Object_EncapManus = 999999;
	nHeight_Object_magnus = 1300;
	nWidth_Object_magnus = 1100;
	////
// Tilt Teach //
	m_rectTilt = CRect(0, 0, 0, 0);

	hvec_TupleVectorLocation.RemoveAll();
	for(int i=0; i<32; i++) {
		if(i<31)
			hvec_TupleVectorLocation.Add(HTuple());
		hvec_TupleVectorPVI[i].RemoveAll();
		for(int j=0; j<10; j++)
			darrayPVI[i][j].RemoveAll();
	}
	hvec_TupleVectorEncap.RemoveAll();
	hvec_TupleVectorCoverLayer.RemoveAll();
	for (int i = 0; i < 10; i++)
		hvec_TupleVectorCoverLayer.Add(HTuple());
	for (int k = 0; k < 15; k++) {
		darrayEncap[k].RemoveAll();
		darrayCoverLayer[k].RemoveAll();
	}
	hvec_TupleVectorCoverLayerPointer.RemoveAll();

	hTupleProjectionParam = HTuple();
	hTupleTeachDoc = HTuple();
	hTupleEdgeTeachTuple = HTuple();
	hTupleDatumTeachTuple = HTuple();
	hTuplePointerModelIDs = HTuple();

	bCentralizedVerificationMode = FALSE;
	for(int i = 0;i<5;i++)
		nNoOfPviArea=1;

	try {
		GenEmptyObj(&hObjectDeviceLocation);
		GenEmptyObj(&hObjectPointerLocation);
	}
	catch (HOperatorException ex) {
		CString str;
		str.Format("[Inspection::Initialize] %s", ex.ErrorMessage().Text());
		LogMessage(9, str);
	}






	catch (...) {}
}


CCalibrationData::CCalibrationData()
{
	rectCalibSideTrack = CRect( 100, 100, 300, 300);
	rectDotCalib = CRect( 100, 100, 300, 300);
	
	nCalibTolerence = 5;
	nTargetGreyDeviation = 10;
	nTargetGreyMeanValue = 230;

	nTargetGreyMeanValueSide = 150;
	nCalibTolerenceSide = 5;

	nInputAlongX = 500;
	nInputAlongY = 500;
	dResolutionAlongXInMicronPerPixel = 0.0;
	dResolutionAlongYInMicronPerPixel = 0.0;
	
	nInputMinRadiusDotTargetPixel = 10;

	hResolutionYInput = HTuple();

	bEnableLightCorrection = FALSE;
	hLightCoefficient = HTuple();
	hLightRows = HTuple();
	hLightCols = HTuple();

	try {
		GenEmptyObj(&hCalibMaxImage);
		GenEmptyObj(&hCalibSumImage);
		GenEmptyObj(&hCalibAvgImage);
	}
	catch (HOperatorException ex) {
		CString str;
		str.Format("[Inspection::Initialize] %s", ex.ErrorMessage().Text());
		LogMessage(9, str);
	}
	catch (...) {}
}

CInspectinonData::CInspectinonData()
{
	hTiltHeight = HTuple();

	hStoreTeachModelRowCentre = HTuple();
	hStoreTeachModelColCentre = HTuple();
	hStoreMatchModelCentreRow = HTuple();
	hStoreMatchModelCentreCol = HTuple();
}

CInspectionCore::CInspectionCore()
{
	m_bIsPatternLens = FALSE;
	for(int i=0; i<ERROR_CODES; i++)
		m_InspParms[i] = NULL;
	//m_InspParms[m_LensLocationParam.nIndex]			= &m_LensLocationParam;
	//m_InspParms[m_ForeignParam.nIndex]				= &m_ForeignParam;
	
	m_dPixelSize = 8.33;
	m_bParallelism = FALSE;
	
	m_pShare = NULL;
}


CInspectionCore::~CInspectionCore()
{
	/*for(int i=0; i<THREAD_COUNT; i++)
		CloseHandle(handlesDone[i]);*/
	for (int i = 0; i<2; i++)
		 m_arrayOverlay[i].RemoveAll();
	
	 m_arrayOverlayInspection.RemoveAll();
	 m_arrayOverlayTeach.RemoveAll();
	 m_arrayOverlayDotCalibration.RemoveAll();
	 m_arrayOverlayPVIInsp.RemoveAll();
}

int CInspectionCore::Teach(HImage hImages,
						  int nTeachStep,
						  BOOL *bEnable,
					      BOOL bAutoTeach,
						  int nCurTrack,		
					      int nDoc,
						  int nFOV,
						  int nTotalFOV,
						  CTrainingData* pTrainingData,
						  CCalibrationData* pCalibData,
						  BOOL bEncapLoc)
{
	COLORREF colorRed = RGB(255, 0, 0);
	COLORREF colorGreen = RGB(0, 255, 0);
	COLORREF colorBlue = RGB(0, 0, 255);
	COLORREF colorYellow = RGB(255, 255, 0);
	COLORREF colorOrange = RGB(255, 165, 0);
	COLORREF colorBlack = RGB(0, 0, 0);
	int nStepDebug = 0;


	if(!nTeachStep) {
		m_arrayOverlayTeach.RemoveAll();

		HTuple hDoc = 0;
		HTuple hUniqueNoPatternsRows, hUniqueNoPatternsCols;
		HTuple hLocationTeachRows, hLocationTeachCols;
		HTuple hRefRotationAngles, hRefPointRows, hRefPointCols, hModelIDs;
		HTuple hGotRotationAngleDev = 0;
		HTuple hGotShiftAlongRow = 0;
		HTuple hGotShiftAlongCol = 0;

		HTuple hIsDatum, hMatchModelScore, hDatumRefRotationAngle, hDatumRefPointRow, hDatumRefPointCol, hDebugMessageOut;
		HImage hDebugImageOut;
		HRegion hMatchModelRegion, hDebugRegionOut;

		// Unique Pattern
		HTuple hDatumSearchEnable, hNoOfUniqueRegions, hMaxRotationAngle, hPatternSearchAreaAlongX, hPatternSearchAreaAlongY, hMinAcceptanceScore;
		hDatumSearchEnable = m_TrainingData.bDattamSearchEnable;
		hNoOfUniqueRegions = m_TrainingData.nDattamSearchNoOfDattams;
		hMaxRotationAngle = m_TrainingData.nDattamSearchMaxRotAngle;
		hPatternSearchAreaAlongX = m_TrainingData.dDattamSearchAreaX / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
		hPatternSearchAreaAlongY = m_TrainingData.dDattamSearchAreaY / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
		hMinAcceptanceScore = (m_TrainingData.dDattamSearchMinAcceptanceScore / 100.0);
		if (hDatumSearchEnable == 1) {
			hDoc = 1;
			for (int i = 0; i<m_TrainingData.nDattamSearchNoOfDattams; i++) {
				TupleConcat(hUniqueNoPatternsRows, m_TrainingData.m_rectDattamSearch[i].top, &hUniqueNoPatternsRows);
				TupleConcat(hUniqueNoPatternsRows, m_TrainingData.m_rectDattamSearch[i].bottom, &hUniqueNoPatternsRows);
				TupleConcat(hUniqueNoPatternsCols, m_TrainingData.m_rectDattamSearch[i].left, &hUniqueNoPatternsCols);
				TupleConcat(hUniqueNoPatternsCols, m_TrainingData.m_rectDattamSearch[i].right, &hUniqueNoPatternsCols);
			}

			_FCI_TeachUniquePattern(hImages,
				hNoOfUniqueRegions, hMaxRotationAngle,
				hUniqueNoPatternsRows, hUniqueNoPatternsCols,
				&hModelIDs);

			_FCI_Inspect_UniquePattern(hImages,
				&hMatchModelRegion, &hDebugImageOut, &hDebugRegionOut,
				nStepDebug, hPatternSearchAreaAlongX, hPatternSearchAreaAlongY, hMaxRotationAngle,
				hMinAcceptanceScore, hModelIDs, hUniqueNoPatternsRows, hUniqueNoPatternsCols,
				&hIsDatum, &hMatchModelScore, &hDatumRefRotationAngle, &hDatumRefPointRow, &hDatumRefPointCol, &hDebugMessageOut);
		}

		HRegion hDeviceLocation;
		HTuple hDeviceLocationEnable, hNoOfLocationRegions, hDeviceLocWidth, hDeviceLocHeight, hDeviceMaskSize, hMinDeviceContrast, hSobelAmp, hReferencePointType;
		HTuple hDebugMsgOut, hIsPass, hEdgeRefRotationAngle, hEdgeRefPointRow, hEdgeRefPointCol;

		HTuple hDeviceEdgeDetectionEnable, hDeviceHorizontalContrast, hDeviceHorizontalDirection, hDeviceVerticalContrast, hDeviceVerticalDirection;
		HTuple hDeviceEdgeLocationTeachRows, hDeviceEdgeLocationTeachCols;
		HTuple hDeviceEdgeDetectionSmoothingImage;


		hDeviceLocationEnable = m_TrainingData.bDeviceLocationEnable;
		hNoOfLocationRegions = m_TrainingData.nNoOfLocationTeachRegion;
		hDeviceLocWidth = m_TrainingData.dDeviceLocWidth / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
		hDeviceLocHeight = m_TrainingData.dDeviceLocHeight / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
		hDeviceMaskSize = m_TrainingData.dMaskSize / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
		hMinDeviceContrast = m_TrainingData.nMinDeviceContrast;
		hSobelAmp = m_TrainingData.bSobelAmp;
		hReferencePointType = m_TrainingData.nDeviceReferencePointType;

		hDeviceEdgeDetectionEnable = m_TrainingData.bDeviceEdgeDetectionEnable;
		hDeviceHorizontalContrast = m_TrainingData.nDeviceHorizontalContrast;
		hDeviceHorizontalDirection = m_TrainingData.nDeviceHorizontalDirection;
		hDeviceVerticalContrast = m_TrainingData.nDeviceVerticalContrast;
		hDeviceVerticalDirection = m_TrainingData.nDeviceVerticalDirection;

		hDeviceEdgeDetectionSmoothingImage = m_TrainingData.bUseSmoothingForEdgeDetection;

		HTuple hFOV, hFOVs, hTrack;
		hFOV = nFOV;
		hFOVs = nTotalFOV;
		hTrack = nCurTrack + 1;

		GenEmptyRegion(&hDeviceLocation);

		if(hDeviceLocationEnable == 1) {
			hDoc = 1;
			for(int i=0; i<m_TrainingData.nNoOfLocationTeachRegion; i++) {
				TupleConcat(hLocationTeachRows, m_TrainingData.m_rectDeviceLoc.top, &hLocationTeachRows);
				TupleConcat(hLocationTeachRows, m_TrainingData.m_rectDeviceLoc.bottom, &hLocationTeachRows);
				TupleConcat(hLocationTeachCols, m_TrainingData.m_rectDeviceLoc.left, &hLocationTeachCols);
				TupleConcat(hLocationTeachCols, m_TrainingData.m_rectDeviceLoc.right, &hLocationTeachCols);
			}
			//Use edge detection to detect the device edges instead of blob detection Algo
			if (hDeviceEdgeDetectionEnable == 1) {

				TupleConcat(hDeviceEdgeLocationTeachRows, m_TrainingData.m_rectDeviceHorizontalLineLoc.top, &hDeviceEdgeLocationTeachRows);
				TupleConcat(hDeviceEdgeLocationTeachRows, m_TrainingData.m_rectDeviceHorizontalLineLoc.bottom, &hDeviceEdgeLocationTeachRows);
				TupleConcat(hDeviceEdgeLocationTeachRows, m_TrainingData.m_rectDeviceVerticalLineLoc.top, &hDeviceEdgeLocationTeachRows);
				TupleConcat(hDeviceEdgeLocationTeachRows, m_TrainingData.m_rectDeviceVerticalLineLoc.bottom, &hDeviceEdgeLocationTeachRows);

				TupleConcat(hDeviceEdgeLocationTeachCols, m_TrainingData.m_rectDeviceHorizontalLineLoc.left, &hDeviceEdgeLocationTeachCols);
				TupleConcat(hDeviceEdgeLocationTeachCols, m_TrainingData.m_rectDeviceHorizontalLineLoc.right, &hDeviceEdgeLocationTeachCols);
				TupleConcat(hDeviceEdgeLocationTeachCols, m_TrainingData.m_rectDeviceVerticalLineLoc.left, &hDeviceEdgeLocationTeachCols);
				TupleConcat(hDeviceEdgeLocationTeachCols, m_TrainingData.m_rectDeviceVerticalLineLoc.right, &hDeviceEdgeLocationTeachCols);


				_FCI_Inspect_Cross_Point(hImages, &hDebugImageOut, &hDebugRegionOut, hDeviceHorizontalContrast, hDeviceHorizontalDirection,
					hDeviceVerticalContrast, hDeviceVerticalDirection, hDeviceEdgeLocationTeachRows, hDeviceEdgeLocationTeachCols, hMaxRotationAngle, hDeviceEdgeDetectionSmoothingImage,
					1, &hEdgeRefRotationAngle, &hEdgeRefPointRow, &hEdgeRefPointCol, &hDebugMessageOut, &hIsPass);
			}
			else {
				_FCI_Inspect_EdgeLocation(hImages,
					&hDeviceLocation,
					hSobelAmp, hNoOfLocationRegions, hDeviceLocWidth, hDeviceLocHeight,
					hMinDeviceContrast, hLocationTeachRows, hLocationTeachCols, hDeviceMaskSize, hFOV, hFOVs, hTrack, hReferencePointType,
					&hEdgeRefRotationAngle, &hEdgeRefPointRow, &hEdgeRefPointCol);
			}
			
		}
		else {
			hDoc = 1;
			hEdgeRefPointRow = hEdgeRefPointCol = hEdgeRefRotationAngle = 0;
		}
		//Show the top reference point inspected
		HObject hCross, hCrossRegion, hDebugCrossRegion;
		HTuple hCrossRow, hCrossCol;
		GenCrossContourXld(&hCross, hEdgeRefPointRow, hEdgeRefPointCol, 200, 0);
		GetContourXld(hCross, &hCrossRow, &hCrossCol);
		GenRegionPolygon(&hCrossRegion, hCrossRow, hCrossCol);

		m_arrayOverlayTeach.Add(hCrossRegion, colorYellow, "Top Reference Point Found By Inspect Edge Device Location");

		if (nDoc == 0 && nFOV == 1 && m_TrainingData.bEnableAutoFocusSequence) {
			HTuple hRow1, hRow2, hCol1, hCol2;
			hRow1 = m_TrainingData.m_rectAutoFocusROI.top;
			hCol1 = m_TrainingData.m_rectAutoFocusROI.left;
			hRow2 = m_TrainingData.m_rectAutoFocusROI.bottom;
			hCol2 = m_TrainingData.m_rectAutoFocusROI.right;
			HTuple hWidth = hCol2 - hCol1;
			HTuple hHeight = hRow2 - hRow1;
			HTuple hDiameter = hWidth < hHeight ? hWidth : hHeight;
			HObject hAutoFocusRoi;
			GenRectangle1(&hAutoFocusRoi, hRow1, hCol1, hRow2, hCol2);
			m_arrayOverlayTeach.Add(hAutoFocusRoi, colorRed);

			HObject hLine;

			GenRegionLine(&hLine, hRow1, hCol1 + hWidth / 2, hRow2, hCol1 + hWidth / 2);
			m_arrayOverlayTeach.Add(hLine, colorRed);
			GenRegionLine(&hLine, hRow1 + hHeight / 2, hCol1, hRow1 + hHeight / 2, hCol2);
			m_arrayOverlayTeach.Add(hLine, colorRed);

			GenCircle(&hAutoFocusRoi, (hRow1 + hRow2) / 2, (hCol1 + hCol2) / 2, hDiameter /2);
			m_arrayOverlayTeach.Add(hAutoFocusRoi, colorRed);
		}

		HTuple hRefRotationAngle, hRefPointRow, hRefPointCol;
		HTuple hGotRotationAngle = 0;
		if (hDeviceLocationEnable == 1 || hDatumSearchEnable == 1)
		{
			if (hDatumSearchEnable == 1)
			{
				hRefRotationAngle = hDatumRefRotationAngle;
				hRefPointRow = hDatumRefPointRow;
				hRefPointCol = hDatumRefPointCol;
			}
			else
			{
				hRefRotationAngle = hEdgeRefRotationAngle;
				hRefPointRow = hEdgeRefPointRow;
				hRefPointCol = hEdgeRefPointCol;
			}

			HTuple hTopRefGotRotationAngle, hTopRefGotShiftAlongRow, hTopRefGotShiftAlongCol;

			_FCI_Inspect_DeviceLocation(hImages,
				&hDeviceLocation, &hMatchModelRegion,
				&hDebugImageOut, &hDebugRegionOut,
				hDatumSearchEnable, hDeviceLocationEnable, nStepDebug,
				hSobelAmp, hNoOfLocationRegions,
				hDeviceLocWidth, hDeviceLocHeight, hMinDeviceContrast,
				hNoOfUniqueRegions, hMaxRotationAngle,
				hPatternSearchAreaAlongX, hPatternSearchAreaAlongY, hMinAcceptanceScore,
				hModelIDs, hRefRotationAngle,
				hRefPointRow, hRefPointCol,
				hEdgeRefRotationAngle, hEdgeRefPointRow, hEdgeRefPointCol,
				hUniqueNoPatternsRows, hUniqueNoPatternsCols,
				hLocationTeachRows, hLocationTeachCols,
				hDeviceMaskSize, hFOV, hFOVs, hTrack, hReferencePointType,
				hDeviceEdgeDetectionEnable,
				hDeviceHorizontalContrast, hDeviceHorizontalDirection,
				hDeviceVerticalContrast, hDeviceVerticalDirection,
				hDeviceEdgeLocationTeachRows, hDeviceEdgeLocationTeachCols,
				hDeviceEdgeDetectionSmoothingImage,
				&hIsPass, &hIsDatum, &hGotRotationAngle, &hGotShiftAlongRow, &hGotShiftAlongCol,
				&hMatchModelScore,
				&hTopRefGotRotationAngle, &hTopRefGotShiftAlongRow, &hTopRefGotShiftAlongCol,
				&hDebugMsgOut);

			if (0 != hIsPass)
				m_arrayOverlayTeach.Add(hDeviceLocation, colorGreen);
			if (0 != hIsDatum)
				m_arrayOverlayTeach.Add(hMatchModelRegion, colorGreen);
		}
		ConcatObj(pTrainingData->hObjectDeviceLocation, hDeviceLocation, &pTrainingData->hObjectDeviceLocation);
		TupleConcat(pTrainingData->hTupleProjectionParam, (hGotRotationAngle.TupleConcat(hGotShiftAlongRow)).TupleConcat(hGotShiftAlongCol), &pTrainingData->hTupleProjectionParam);
		TupleConcat(pTrainingData->hTupleTeachDoc, hDoc, &pTrainingData->hTupleTeachDoc);
		//TupleConcat(pTrainingData->hTupleTeachDoc, 1, &pTrainingData->hTupleTeachDoc);
		//TupleConcat(pTrainingData->hTupleDatumTeachTuple, hDatumSearchEnable, &pTrainingData->hTupleDatumTeachTuple);
		//if (hDatumSearchEnable == 1)
		//	TupleConcat(pTrainingData->hTupleEdgeTeachTuple, hIsDatum/*hDeviceLocationEnable*/, &pTrainingData->hTupleEdgeTeachTuple);
		//else
		//	TupleConcat(pTrainingData->hTupleEdgeTeachTuple, hIsPass/*hDeviceLocationEnable*/, &pTrainingData->hTupleEdgeTeachTuple);

		int nIndex = 0;
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceLocationEnable);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hNoOfLocationRegions);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceLocWidth);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceLocHeight);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceMaskSize);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hMinDeviceContrast);

		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDatumSearchEnable);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hNoOfUniqueRegions);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hMaxRotationAngle);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hPatternSearchAreaAlongX);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hPatternSearchAreaAlongY);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hMinAcceptanceScore);

		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hUniqueNoPatternsRows);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hUniqueNoPatternsCols);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hLocationTeachRows);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hLocationTeachCols);

		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hModelIDs);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hRefRotationAngle);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hRefPointRow);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hRefPointCol);

		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hEdgeRefRotationAngle);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hEdgeRefPointRow);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hEdgeRefPointCol);

		//For Device Edge Detection option
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceEdgeDetectionEnable);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceHorizontalContrast);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceHorizontalDirection);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceVerticalContrast);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceVerticalDirection);

		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceEdgeLocationTeachRows);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceEdgeLocationTeachCols);

		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceEdgeDetectionSmoothingImage);

	}

	//////////////////////////////////////
	////////Encap Bottom Magnus///////////
	//////////////////////////////////////
	else if (nTeachStep == 14)
	{

		// get Two Point Encap Location////////

		int nThresh_Min_magnus = m_TrainingData.nThreshMin_EncapManus;
		int nThresh_Max_magnus = m_TrainingData.nThreshMax_EncapManus;
		int nArea_Object_magnus = m_TrainingData.nArea_Object_EncapManus;
		int nHeight_Object_magnus = m_TrainingData.nHeight_Object_magnus;
		int nWidth_Object_magnus = m_TrainingData.nWidth_Object_magnus;
		HRegion hThreshold_Region_magnus;
		HObject hDeviceLocation_magnus, hSelect_DeviceLocation_magnus;
		HTuple hDefectInfo_magnus;

		Threshold(hImages, &hThreshold_Region_magnus, nThresh_Min_magnus, nThresh_Max_magnus);
		Connection(hThreshold_Region_magnus, &hDeviceLocation_magnus);
	//	SelectShape(hDeviceLocation_magnus, &hSelect_DeviceLocation_magnus, "area", "and", nArea_Object_magnus, 9999999);
		SelectShape(hDeviceLocation_magnus, &hSelect_DeviceLocation_magnus, "height", "and", nHeight_Object_magnus, nHeight_Object_magnus + 400);
		SelectShape(hSelect_DeviceLocation_magnus, &hSelect_DeviceLocation_magnus, "width", "and", nWidth_Object_magnus, nWidth_Object_magnus + 400);

		HRegion hSelect_DeviceLocation_obj_magnus;
		SelectObj(hSelect_DeviceLocation_magnus, &hSelect_DeviceLocation_obj_magnus, 1);

		HRegion hBoundary_DeviceLocation_magnus;
		Boundary(hSelect_DeviceLocation_magnus, &hBoundary_DeviceLocation_magnus, "inner");
		HTuple hRows_Device_magnus, hCols_Device_magnus;
		GetRegionPoints(hBoundary_DeviceLocation_magnus, &hRows_Device_magnus, &hCols_Device_magnus);

		HTuple  nbottom_DeviceLocationEncap_magnus,
				ntop_DeviceLocationEncap_magnus,
				nright_DeviceLocationEncap_magnus,
				nleft_DeviceLocationEncap_magnus;
		TupleMax(hRows_Device_magnus, &nbottom_DeviceLocationEncap_magnus);
		TupleMin(hRows_Device_magnus, &ntop_DeviceLocationEncap_magnus);
		TupleMax(hCols_Device_magnus, &nright_DeviceLocationEncap_magnus);
		TupleMin(hCols_Device_magnus, &nleft_DeviceLocationEncap_magnus);

		HTuple nImage_Width, nImage_Height;
		GetImageSize(hImages, &nImage_Width, &nImage_Height);
		m_TrainingData.hRect_DeviceLocationEncap_magnus= CRect( nleft_DeviceLocationEncap_magnus,
																ntop_DeviceLocationEncap_magnus,
																nright_DeviceLocationEncap_magnus,
																nbottom_DeviceLocationEncap_magnus);

		m_TrainingData.hRect_CropRemoveBlackLine_magnus = m_TrainingData.hRect_DeviceLocationEncap_magnus;

		//int nEncapLocation_Height = m_TrainingData.hRect_EncapLocation_magnus.Height();
		//int nEncapLocation_Width = m_TrainingData.hRect_EncapLocation_magnus.Width();

		


		//////////// Get rectangle encap bottom//////////////////
		if (nFOV==1)// top
		{
			if ((int)nleft_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandLeft_magnus[0] > 0)
				m_TrainingData.hRect_EncapLocation_magnus.left = (int)nleft_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandLeft_magnus[0];
			else m_TrainingData.hRect_EncapLocation_magnus.left = 0;
			if ((int)nright_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandRight_magnus[0] < nImage_Width)
				m_TrainingData.hRect_EncapLocation_magnus.right = (int)nright_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandRight_magnus[0];
			else m_TrainingData.hRect_EncapLocation_magnus.right = (int)nImage_Width;

			m_TrainingData.hRect_EncapLocation_magnus.bottom = (int)ntop_DeviceLocationEncap_magnus;
			if (ntop_DeviceLocationEncap_magnus - m_TrainingData.nCrop_ExpandHeight_magnus[0] > 0
				&& ntop_DeviceLocationEncap_magnus - m_TrainingData.nCrop_ExpandHeight_magnus[0] < (int)nImage_Height)
			m_TrainingData.hRect_EncapLocation_magnus.top = (int)ntop_DeviceLocationEncap_magnus - m_TrainingData.nCrop_ExpandHeight_magnus[0];
			else m_TrainingData.hRect_EncapLocation_magnus.top = 0;

			m_TrainingData.hRect_CropSmoothEncap_magnus = m_TrainingData.hRect_EncapLocation_magnus;
			m_TrainingData.hRect_CropSmoothEncap_magnus.top = m_TrainingData.hRect_EncapLocation_magnus.bottom - m_TrainingData.nCrop_Smooth_EncapManus[0];
			
			m_TrainingData.hRect_CropRemoveBlackLine_magnus.top = m_TrainingData.hRect_EncapLocation_magnus.top;
			m_TrainingData.hRect_CropRemoveBlackLine_magnus.bottom = m_TrainingData.hRect_EncapLocation_magnus.top + m_TrainingData.nCrop_RemoveBLHeight_magnus[0];
			m_TrainingData.hRect_CropRemoveBlackLine_magnus.left = (int)nleft_DeviceLocationEncap_magnus + m_TrainingData.nCrop_RemoveBLLeft_magnus[0];
			m_TrainingData.hRect_CropRemoveBlackLine_magnus.right = (int)nright_DeviceLocationEncap_magnus + m_TrainingData.nCrop_RemoveBLRight_magnus[0];
		}
		else if (nFOV ==4)// bottom
		{
			if ((int)nleft_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandLeft_magnus[1] > 0)
				m_TrainingData.hRect_EncapLocation_magnus.left = (int)nleft_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandLeft_magnus[1];
			else m_TrainingData.hRect_EncapLocation_magnus.left = 0;
			if ((int)nright_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandRight_magnus[1] < nImage_Width)
				m_TrainingData.hRect_EncapLocation_magnus.right = (int)nright_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandRight_magnus[1];
			else m_TrainingData.hRect_EncapLocation_magnus.right = (int)nImage_Width;

			m_TrainingData.hRect_EncapLocation_magnus.top = (int)nbottom_DeviceLocationEncap_magnus;

			if ((int)ntop_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandHeight_magnus[1] <  nImage_Height && 
				(int)ntop_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandHeight_magnus[1]>0)
			m_TrainingData.hRect_EncapLocation_magnus.bottom = (int)nbottom_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandHeight_magnus[1];
			else m_TrainingData.hRect_EncapLocation_magnus.bottom = (int)nImage_Height;
			m_TrainingData.hRect_CropSmoothEncap_magnus = m_TrainingData.hRect_EncapLocation_magnus;
			m_TrainingData.hRect_CropSmoothEncap_magnus.bottom = m_TrainingData.hRect_EncapLocation_magnus.top + m_TrainingData.nCrop_Smooth_EncapManus[1];

			m_TrainingData.hRect_CropRemoveBlackLine_magnus.bottom = m_TrainingData.hRect_EncapLocation_magnus.bottom;
			m_TrainingData.hRect_CropRemoveBlackLine_magnus.top = m_TrainingData.hRect_EncapLocation_magnus.bottom - m_TrainingData.nCrop_RemoveBLHeight_magnus[1];
			m_TrainingData.hRect_CropRemoveBlackLine_magnus.left = (int)nleft_DeviceLocationEncap_magnus + m_TrainingData.nCrop_RemoveBLLeft_magnus[1];
			m_TrainingData.hRect_CropRemoveBlackLine_magnus.right = (int)nright_DeviceLocationEncap_magnus + m_TrainingData.nCrop_RemoveBLRight_magnus[1];
		}
	
	}
	else if (nTeachStep == 15)
	{
			m_TrainingData.hRect_CropNoSmoothEncap_magnus = m_TrainingData.hRect_EncapLocation_magnus;

			if (nFOV == 1)// top
			{
				m_TrainingData.nCrop_Smooth_EncapManus[0] = m_TrainingData.hRect_CropSmoothEncap_magnus.Height();
				m_TrainingData.nCrop_RemoveBLHeight_magnus[0] = m_TrainingData.hRect_CropRemoveBlackLine_magnus.Height();
				if (m_TrainingData.nCrop_Smooth_EncapManus[0] <= m_TrainingData.hRect_EncapLocation_magnus.Height())
				{

					m_TrainingData.hRect_CropNoSmoothEncap_magnus.bottom = m_TrainingData.hRect_EncapLocation_magnus.bottom - m_TrainingData.nCrop_Smooth_EncapManus[0];
					m_TrainingData.hRect_CropSmoothEncap_magnus.top = m_TrainingData.hRect_EncapLocation_magnus.bottom - m_TrainingData.nCrop_Smooth_EncapManus[0];
					//////////Save 


				}
				else
				{
					m_TrainingData.hRect_CropNoSmoothEncap_magnus.bottom = m_TrainingData.hRect_CropNoSmoothEncap_magnus.top ;
					m_TrainingData.hRect_CropSmoothEncap_magnus = m_TrainingData.hRect_EncapLocation_magnus;

				}

				

				m_TrainingData.nCrop_ExpandLeft_magnus[0] = m_TrainingData.hRect_EncapLocation_magnus.left - m_TrainingData.hRect_DeviceLocationEncap_magnus.left;
				m_TrainingData.nCrop_ExpandRight_magnus[0] = m_TrainingData.hRect_EncapLocation_magnus.right - m_TrainingData.hRect_DeviceLocationEncap_magnus.right;
				m_TrainingData.nCrop_ExpandHeight_magnus[0] = m_TrainingData.hRect_EncapLocation_magnus.Height();
				m_TrainingData.nCrop_Smooth_EncapManus[0] = m_TrainingData.hRect_CropSmoothEncap_magnus.Height();
			
				m_TrainingData.nCrop_RemoveBLHeight_magnus[0] = m_TrainingData.hRect_CropRemoveBlackLine_magnus.Height();
				m_TrainingData.nCrop_RemoveBLLeft_magnus[0] = m_TrainingData.hRect_CropRemoveBlackLine_magnus.left - m_TrainingData.hRect_DeviceLocationEncap_magnus.left;
				m_TrainingData.nCrop_RemoveBLRight_magnus[0] = m_TrainingData.hRect_CropRemoveBlackLine_magnus.right - m_TrainingData.hRect_DeviceLocationEncap_magnus.right;

			}
			else if (nFOV == 4)// bottom
			{

				m_TrainingData.nCrop_Smooth_EncapManus[1] = m_TrainingData.hRect_CropSmoothEncap_magnus.Height();
				//m_TrainingData.hRect_CropSmoothEncap_magnus = m_TrainingData.hRect_EncapLocation_magnus;
				if (m_TrainingData.nCrop_Smooth_EncapManus[1] <= m_TrainingData.hRect_EncapLocation_magnus.Height())
				{
					m_TrainingData.hRect_CropNoSmoothEncap_magnus.top = m_TrainingData.hRect_EncapLocation_magnus.top + m_TrainingData.nCrop_Smooth_EncapManus[1];
					m_TrainingData.hRect_CropSmoothEncap_magnus.bottom = m_TrainingData.hRect_EncapLocation_magnus.top + m_TrainingData.nCrop_Smooth_EncapManus[1];
					//////////Save 
				}
				else
				{
					m_TrainingData.hRect_CropNoSmoothEncap_magnus.top = m_TrainingData.hRect_CropNoSmoothEncap_magnus.top;
					m_TrainingData.hRect_CropSmoothEncap_magnus = m_TrainingData.hRect_EncapLocation_magnus;

				}
				m_TrainingData.nCrop_ExpandLeft_magnus[1] = m_TrainingData.hRect_EncapLocation_magnus.left - m_TrainingData.hRect_DeviceLocationEncap_magnus.left;
				m_TrainingData.nCrop_ExpandRight_magnus[1] = m_TrainingData.hRect_EncapLocation_magnus.right - m_TrainingData.hRect_DeviceLocationEncap_magnus.right;
				m_TrainingData.nCrop_ExpandHeight_magnus[1] = m_TrainingData.hRect_EncapLocation_magnus.Height();
				m_TrainingData.nCrop_Smooth_EncapManus[1] = m_TrainingData.hRect_CropSmoothEncap_magnus.Height();

				m_TrainingData.nCrop_RemoveBLHeight_magnus[1] = m_TrainingData.hRect_CropRemoveBlackLine_magnus.Height();
				m_TrainingData.nCrop_RemoveBLLeft_magnus[1] = m_TrainingData.hRect_CropRemoveBlackLine_magnus.left - m_TrainingData.hRect_DeviceLocationEncap_magnus.left;
				m_TrainingData.nCrop_RemoveBLRight_magnus[1] = m_TrainingData.hRect_CropRemoveBlackLine_magnus.right - m_TrainingData.hRect_DeviceLocationEncap_magnus.right;
			}
		


	//	CRect hRect_EncapLocation_magnus = m_TrainingData.hRect_EncapLocation_magnus;
		HRegion hRectangle_DeviceLocation_magnus;
		GenRectangle1(&hRectangle_DeviceLocation_magnus,
			m_TrainingData.hRect_EncapLocation_magnus.top,
			m_TrainingData.hRect_EncapLocation_magnus.left,
			m_TrainingData.hRect_EncapLocation_magnus.bottom,
			m_TrainingData.hRect_EncapLocation_magnus.right);
		HImage hCrop_image_magnus;
		ReduceDomain(hImages, hRectangle_DeviceLocation_magnus, &hCrop_image_magnus);

		/////////// Threshold white and black region///////////////
		
		HRegion hBlack_Region_magnus, hWhite_Region_magnus;
		int nThreshMin_Black_magnus = m_TrainingData.nThreshMin_Black_EncapManus;
		int nThreshMax_Black_magnus = m_TrainingData.nThreshMax_Black_EncapManus;
		int nThreshMin_White_magnus = m_TrainingData.nThreshMin_White_EncapManus;
		int nThreshMax_White_magnus = m_TrainingData.nThreshMax_White_EncapManus;
		Threshold(hCrop_image_magnus, &hBlack_Region_magnus, nThreshMin_Black_magnus, nThreshMax_Black_magnus);
		Threshold(hCrop_image_magnus, &hWhite_Region_magnus, nThreshMin_White_magnus, nThreshMax_White_magnus);

		/////////// Dilate White Region to remove intersection line between black and white region ///////////

		HRegion hDilation_WhiteRegion_magnus;
		int nX_Dilation_White_magnus = m_TrainingData.nDilateX_EncapManus;
		int nY_Dilation_White_magnus = m_TrainingData.nDilateY_EncapManus;
		DilationRectangle1(hWhite_Region_magnus, &hDilation_WhiteRegion_magnus, nX_Dilation_White_magnus, nY_Dilation_White_magnus);

		/////////// Union White and Black  and Remove BlackLine region//////////////

		HRegion hUnionRegion_WhiteBlack_magnus;
		Union2(hBlack_Region_magnus, hDilation_WhiteRegion_magnus, &hUnionRegion_WhiteBlack_magnus);

		HRegion hRect_CropRemoveBlackLine_magnus;
		HRegion hUnionRegion_WhiteBlackBL_magnus;
		GenRectangle1(&hRect_CropRemoveBlackLine_magnus,
			m_TrainingData.hRect_CropRemoveBlackLine_magnus.top,
			m_TrainingData.hRect_CropRemoveBlackLine_magnus.left,
			m_TrainingData.hRect_CropRemoveBlackLine_magnus.bottom,
			m_TrainingData.hRect_CropRemoveBlackLine_magnus.right);

		Union2(hUnionRegion_WhiteBlack_magnus, hRect_CropRemoveBlackLine_magnus, &hUnionRegion_WhiteBlackBL_magnus);

		//int nShapeAreaRemove_magnus = m_TrainingData.nArea_Object_EncapManus;
		//HRegion hSelectRegion_BW_magnus;
		//SelectShape(hUnionRegion_WhiteBlackBL_magnus, &hSelectRegion_BW_magnus, "width", "and", 400, 99999);
		//HRegion hFillupRegion_BW_magnus;
		//FillUp(hUnionRegion_WhiteBlackBL_magnus, &hFillupRegion_BW_magnus);
		//

		////////// Remove White and black region /////////////
		HRegion hDiffRegion_magnus;
		Difference(hRectangle_DeviceLocation_magnus, hUnionRegion_WhiteBlackBL_magnus, &hDiffRegion_magnus);
		HRegion hFillupRegion_diff_magnus;
		FillUp(hDiffRegion_magnus, &hFillupRegion_diff_magnus);
		HRegion hRegionOpening_fill_magnus;

		//////////////Remove top black line ( filter)

		int nX_Opening_fill_magnus = m_TrainingData.nOpeningX_EncapManus;
		int nY_Opening_fill_magnus = m_TrainingData.nOpeningY_EncapManus;
		OpeningRectangle1(hFillupRegion_diff_magnus, &hRegionOpening_fill_magnus, nX_Opening_fill_magnus, nY_Opening_fill_magnus);

		///////////////////////////////// Select shape////////

		HRegion hConnection_opening_magnus, hRegionOpening_Select_magnus, hRegionOpening_Circle_magnus;
		int nValueOpening_Circle_magnus = m_TrainingData.nValue_OpeningCircle_magnus;
		Connection(hRegionOpening_fill_magnus, &hConnection_opening_magnus);
		SelectShape(hConnection_opening_magnus, &hRegionOpening_Select_magnus, "width", "and", 400, 9999);
		OpeningCircle(hRegionOpening_Select_magnus, &hRegionOpening_Circle_magnus, nValueOpening_Circle_magnus);

	//////////////////// gen Crop smooth contour to intersection/////////////
	HXLD hContour_Encap_magnus, hCropContour_After_magnus;
	HXLD hCropContour_BeforeSmooth_magnus;

	/// Opening to smooth corner before use smoothContour
	HRegion hRegionOpening_CircleCrop_magnus;
	OpeningCircle(hRegionOpening_Circle_magnus, &hRegionOpening_CircleCrop_magnus, m_TrainingData.nValue_OpeningCircleCrop_magnus);		
	// Crop contour to smooth
	int nValueSmooth_Crop_magnus = m_TrainingData.nValue_Smooth_EncapMagnus;
	GenContourRegionXld(hRegionOpening_CircleCrop_magnus, &hContour_Encap_magnus, "center");
	//CRect hRect_CropSmoothEncap_magnus = m_TrainingData.hRect_CropSmoothEncap_magnus;
	CropContoursXld(hContour_Encap_magnus, &hCropContour_BeforeSmooth_magnus,
		m_TrainingData.hRect_CropSmoothEncap_magnus.top,
		m_TrainingData.hRect_CropSmoothEncap_magnus.left,
		m_TrainingData.hRect_CropSmoothEncap_magnus.bottom,
		m_TrainingData.hRect_CropSmoothEncap_magnus.right, "true");

	SmoothContoursXld(hContour_Encap_magnus, &hCropContour_After_magnus, 2 * nValueSmooth_Crop_magnus + 1);
	/////// Gen region smooth
	HXLD hRegionContour_Smooth_magnus;
	GenRegionContourXld(hCropContour_After_magnus, &hRegionContour_Smooth_magnus, "filled");
	HRegion hRegionInter_CropSmooth_magnus;
	Intersection(hRegionContour_Smooth_magnus, hRegionOpening_Circle_magnus, &hRegionInter_CropSmooth_magnus);

	//////////////////////// Get region no smooth  //////////
	
	HRegion hRegionRectangle_CropNoSmooth_magnus;
//	CRect hRect_CropNoSmoothEncap_magnus = m_TrainingData.hRect_CropNoSmoothEncap_magnus;
	GenRectangle1(&hRegionRectangle_CropNoSmooth_magnus,
		m_TrainingData.hRect_CropNoSmoothEncap_magnus.top,
		m_TrainingData.hRect_CropNoSmoothEncap_magnus.left,
		m_TrainingData.hRect_CropNoSmoothEncap_magnus.bottom,
		m_TrainingData.hRect_CropNoSmoothEncap_magnus.right);
	HRegion hRegionInter_Nosmooth_magnus, hRegionUnion_2RegionSmooth_magnus, hRegionBoundary_2Region_Smooth_magnus;
	Intersection(hRegionOpening_Circle_magnus, hRegionRectangle_CropNoSmooth_magnus, &hRegionInter_Nosmooth_magnus);
	
	//////////// Union Smooth and NoSmooth Region

	Union2(hRegionInter_Nosmooth_magnus, hRegionInter_CropSmooth_magnus, &hRegionUnion_2RegionSmooth_magnus);
	OpeningCircle(hRegionUnion_2RegionSmooth_magnus, &hRegionUnion_2RegionSmooth_magnus, nValueOpening_Circle_magnus);
	Boundary(hRegionUnion_2RegionSmooth_magnus, &hRegionBoundary_2Region_Smooth_magnus, "inner");
	m_arrayOverlayTeach.Add(hRegionUnion_2RegionSmooth_magnus, colorYellow);
//	m_arrayOverlayTeach.Add(hRegionRectangle_CropNoSmooth_magnus, colorYellow);



	//m_arrayOverlayTeach.Add(hRegionInter_CropSmooth_magnus, colorGreen);
}
	////////////////////////////////////
	/////// end Encap Bottom Magnus/////////
	////////////////////////////////////

	else if (nTeachStep == 13) {
		m_arrayOverlayTeach.RemoveAll();

		HRegion hDeviceLocationSelected;
		HTuple hIsSelected, hInspecRotationAngle, hInspectShiftAlongRow, hInspectShiftAlongCol;
		HTuple hCount, hArea, hDeviceCenterRow, hDeviceCenterCol;

		_FCI_Extract_LocationParameterData(pTrainingData->hTupleProjectionParam, pTrainingData->hTupleTeachDoc,
			pTrainingData->hTupleEdgeTeachTuple, pTrainingData->hTupleDatumTeachTuple, nDoc,
			&hIsSelected, &hInspecRotationAngle, &hInspectShiftAlongRow, &hInspectShiftAlongCol);


		if (0 != (hIsSelected == 0))
		{
			if (nCurTrack == 2)
				return 0;// -DEVICE_LOCATION;
			else
				return -DEVICE_LOCATION;
		}

		HTuple hConcatArea, hRow2, hColumn2, hMaxArea;
		AreaCenter(pTrainingData->hObjectDeviceLocation, &hConcatArea, &hRow2, &hColumn2);
		if (hConcatArea.TupleLength() > 0)
			TupleMax(hConcatArea, &hMaxArea);

		if (0 != (hMaxArea == 0))
		{
			if (nCurTrack == 2)
				return 0;// -DEVICE_LOCATION;
			else
				return -DEVICE_LOCATION;
		}

		_FCI_Select_DeviceLocation(pTrainingData->hObjectDeviceLocation,
								&hDeviceLocationSelected,
								nDoc, &hDeviceCenterRow, &hDeviceCenterCol);

		
		CountObj(hDeviceLocationSelected, &hCount);
		if (hCount < 1)
			return 0;

		//AreaCenter(hDeviceLocationSelected, &hArea, &hDeviceCenterRow, &hDeviceCenterCol);
		

		//// ---- Region ---- ////
		HTuple hPVIArea, hRegionCenterRow, hRegionCenterColumn;

		hPVIArea = m_TrainingData.nNoOfPviArea;
		HTuple hPVIAngle, hPVI2DCenter, hPVISize, hPVIPolygonPoints;
		hPVIAngle = hPVI2DCenter = hPVISize = HTuple();

		//// ---- Region Mask ---- ////
		HTuple hMaskNumber;
		hMaskNumber = 0;

		HTuple hSlotNumber;
		hSlotNumber = 0;

		HTuple hMask2DCenterPoints, hMaskAngleConcat, hMaskSize;
		hMask2DCenterPoints = hMaskAngleConcat = hMaskSize = HTuple();
		HTuple hSlot2DCenterPoints, hSlotAngleConcat, hSlotSize;
		hSlot2DCenterPoints = hSlotAngleConcat = hSlotSize = HTuple();

		m_TrainingData.darrayPVI[0][0].Add(hPVIArea.D());
		m_TrainingData.hvec_TupleVectorPVI[0].InsertAt(0, hPVIArea);
		///////
		/////
		//////
		//////



		//m_EncapMagnusPam.Crop_Smooth = 1;




		//////
		//////
		//////
		//////
		HRegion hFitPVIRegion;
		HRegion hPVIRegion;
		for (int nArea = 0; nArea < hPVIArea; nArea++) 
		{
			hPVIPolygonPoints = HTuple();
			if (hPVIArea != 0) {
				if (m_pPviInspectionSetup[nArea].bEnable || m_pPviInspectionSetup[nArea].bEdgeEnable) {

					HTuple hRow1, hCol1, hRow2, hCol2;
					hRow1 = m_pPviInspectionSetup[nArea].m_rectPviArea.top;
					hCol1 = m_pPviInspectionSetup[nArea].m_rectPviArea.left;
					hRow2 = m_pPviInspectionSetup[nArea].m_rectPviArea.bottom;
					hCol2 = m_pPviInspectionSetup[nArea].m_rectPviArea.right;




					GetRegionCenter(&hPVIRegion, hRow1, hCol1, hRow2, hCol2, &hRegionCenterRow, &hRegionCenterColumn);

					HTuple hPVITopRow, hPVITopCol;
					TopReferencePoints(hImages, hPVIRegion, nFOV, nTotalFOV, TOP_LEFT,
						&hPVITopRow, &hPVITopCol, &hPVIAngle, &hPVI2DCenter, &hPVISize);


					//// ---- Mask Region ---- //// 

					hMaskNumber = m_pPviInspectionSetup[nArea].nDeviceMaskNumber;

					hMask2DCenterPoints = hMaskAngleConcat = hMaskSize = HTuple();

					hSlotNumber = m_pPviInspectionSetup[nArea].nNumOfSlot;

					hSlot2DCenterPoints = hSlotAngleConcat = hSlotSize = HTuple();

					HRegion hConcatMaskRgn;
					GenEmptyObj(&hConcatMaskRgn);

					HRegion hConcatSlotRgn;
					GenEmptyObj(&hConcatSlotRgn);

					for (int nMask = 0; nMask < hMaskNumber; nMask++) {
						HTuple hMaskCenterRow, hMaskCenterColumn;
						HRegion hMaskRegion;

						hRow1 = m_pPviInspectionSetup[nArea].m_rectPviMaskRegions[nMask].top;
						hCol1 = m_pPviInspectionSetup[nArea].m_rectPviMaskRegions[nMask].left;
						hRow2 = m_pPviInspectionSetup[nArea].m_rectPviMaskRegions[nMask].bottom;
						hCol2 = m_pPviInspectionSetup[nArea].m_rectPviMaskRegions[nMask].right;

						GetRegionCenter(&hMaskRegion, hRow1, hCol1, hRow2, hCol2, &hMaskCenterRow, &hMaskCenterColumn);

						HTuple hMaskTopRow, hMaskTopCol, hMaskAngle, hMask2DCenter, hMaskSizeEach;
						TopReferencePoints(hImages, hMaskRegion, nFOV, nTotalFOV, TOP_LEFT,
							&hMaskTopRow, &hMaskTopCol, &hMaskAngle, &hMask2DCenter, &hMaskSizeEach);

						TupleConcat(hMask2DCenterPoints, hMask2DCenter, &hMask2DCenterPoints);
						TupleConcat(hMaskAngleConcat, hMaskAngle, &hMaskAngleConcat);
						TupleConcat(hMaskSize, hMaskSizeEach, &hMaskSize);

						HRegion hRectTemp;
						GenRectangle2(&hRectTemp, hMask2DCenter[0], hMask2DCenter[1], hMaskAngle, hMaskSizeEach[0], hMaskSizeEach[1]);
						ConcatObj(hConcatMaskRgn, hRectTemp, &hConcatMaskRgn);
					}
					for (int nSlot = 0; nSlot < hSlotNumber; nSlot++) {
						HTuple hSlotCenterRow, hSlotCenterColumn;
						HRegion hSlotRegion;

						hRow1 = m_pPviInspectionSetup[nArea].m_rectPviSlot[nSlot].top;
						hCol1 = m_pPviInspectionSetup[nArea].m_rectPviSlot[nSlot].left;
						hRow2 = m_pPviInspectionSetup[nArea].m_rectPviSlot[nSlot].bottom;
						hCol2 = m_pPviInspectionSetup[nArea].m_rectPviSlot[nSlot].right;

						GetRegionCenter(&hSlotRegion, hRow1, hCol1, hRow2, hCol2, &hSlotCenterRow, &hSlotCenterColumn);

						HTuple hSlotTopRow, hSlotTopCol, hSlotAngle, hSlot2DCenter, hSlotSizeEach;
						TopReferencePoints(hImages, hSlotRegion, nFOV, nTotalFOV, TOP_LEFT,
							&hSlotTopRow, &hSlotTopCol, &hSlotAngle, &hSlot2DCenter, &hSlotSizeEach);

						TupleConcat(hSlot2DCenterPoints, hSlot2DCenter, &hSlot2DCenterPoints);
						TupleConcat(hSlotAngleConcat, hSlotAngle, &hSlotAngleConcat);
						TupleConcat(hSlotSize, hSlotSizeEach, &hSlotSize);

						HRegion hRectTemp;
						GenRectangle2(&hRectTemp, hSlot2DCenter[0], hSlot2DCenter[1], hSlotAngle, hSlotSizeEach[0], hSlotSizeEach[1]);
						ConcatObj(hConcatSlotRgn, hRectTemp, &hConcatSlotRgn);
					}
					HRegion hFitPVIMaskRegion, hDebugRegionOut;
					HRegion hFitPVISlotRegion;

					Union1(hConcatMaskRgn, &hFitPVIMaskRegion);
					Union1(hConcatSlotRgn, &hFitPVISlotRegion);

					if (m_pPviInspectionSetup[nArea].nDeviceAreaType == RECTANGLE)
						GenRectangle2(&hFitPVIRegion, hPVI2DCenter[0], hPVI2DCenter[1], hPVIAngle, hPVISize[0], hPVISize[1]);
					else if (m_pPviInspectionSetup[nArea].nDeviceAreaType == ECLIPSE)
						GenEllipse(&hFitPVIRegion, m_pPviInspectionSetup[nArea].m_rectPviArea.CenterPoint().y, m_pPviInspectionSetup[nArea].m_rectPviArea.CenterPoint().x, 0,
							m_pPviInspectionSetup[nArea].m_rectPviArea.Width() / 2, m_pPviInspectionSetup[nArea].m_rectPviArea.Height() / 2);
					else if (m_pPviInspectionSetup[nArea].nDeviceAreaType == POLYGON) {
						HTuple hPoint;
						HTuple hRows, hCols;
						for (int nPointId = 0; nPointId < m_pPviInspectionSetup[nArea].nPolygonPointNumber; nPointId++) {
							TupleConcat(hRows, m_pPviInspectionSetup[nArea].m_polygonPviArea[nPointId].y, &hRows);
							TupleConcat(hCols, m_pPviInspectionSetup[nArea].m_polygonPviArea[nPointId].x, &hCols);

							hPoint.Clear();
							hPoint.Append(m_pPviInspectionSetup[nArea].m_polygonPviArea[nPointId].y);
							hPoint.Append(m_pPviInspectionSetup[nArea].m_polygonPviArea[nPointId].x);

							TupleConcat(hPVIPolygonPoints, hPoint, &hPVIPolygonPoints);
						}
						GenRegionPolygonFilled(&hFitPVIRegion, hRows, hCols);
					}

					m_arrayOverlayTeach.Add(hFitPVIRegion, colorYellow);
					m_arrayOverlayTeach.Add(hFitPVIMaskRegion, colorRed);
					m_arrayOverlayTeach.Add(hFitPVISlotRegion, colorOrange);
				}

				int nIndex = 1;

				for (int i = 0; i < hPVIAngle.Length(); i++)
					m_TrainingData.darrayPVI[nIndex][nArea].Add(hPVIAngle[i].D());
				m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hPVIAngle);

				for (int i = 0; i < hPVI2DCenter.Length(); i++)
					m_TrainingData.darrayPVI[nIndex][nArea].Add(hPVI2DCenter[i].D());
				m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hPVI2DCenter);

				for (int i = 0; i < hPVISize.Length(); i++)
					m_TrainingData.darrayPVI[nIndex][nArea].Add(hPVISize[i].D());
				m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hPVISize);

				for (int i = 0; i < hMaskNumber.Length(); i++)
					m_TrainingData.darrayPVI[nIndex][nArea].Add(hMaskNumber[i].D());
				m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hMaskNumber);

				for (int i = 0; i < hMask2DCenterPoints.Length(); i++)
					m_TrainingData.darrayPVI[nIndex][nArea].Add(hMask2DCenterPoints[i].D());
				m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hMask2DCenterPoints);

				for (int i = 0; i < hMaskAngleConcat.Length(); i++)
					m_TrainingData.darrayPVI[nIndex][nArea].Add(hMaskAngleConcat[i].D());
				m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hMaskAngleConcat);

				for (int i = 0; i < hMaskSize.Length(); i++)
					m_TrainingData.darrayPVI[nIndex][nArea].Add(hMaskSize[i].D());
				m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hMaskSize);
				////////////////////////
				for (int i = 0; i < hSlotNumber.Length(); i++)
					m_TrainingData.darrayPVI[nIndex][nArea].Add(hSlotNumber[i].D());
				m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hSlotNumber);

				for (int i = 0; i < hSlot2DCenterPoints.Length(); i++)
					m_TrainingData.darrayPVI[nIndex][nArea].Add(hSlot2DCenterPoints[i].D());
				m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hSlot2DCenterPoints);

				for (int i = 0; i < hSlotAngleConcat.Length(); i++)
					m_TrainingData.darrayPVI[nIndex][nArea].Add(hSlotAngleConcat[i].D());
				m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hSlotAngleConcat);

				for (int i = 0; i < hSlotSize.Length(); i++)
					m_TrainingData.darrayPVI[nIndex][nArea].Add(hSlotSize[i].D());
				m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hSlotSize);
				
				//For PVI Polygon
				for (int i = 0; i < hPVIPolygonPoints.Length(); i++)
					m_TrainingData.darrayPVI[nIndex][nArea].Add(hPVIPolygonPoints[i].D());
				m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hPVIPolygonPoints);
			}
		}
	}
	else if (nTeachStep == 2) {
		//// ---- encap teach part ---- ////
		m_arrayOverlayTeach.RemoveAll();

		HRegion hDeviceLocationSelected;
		HTuple hIsSelected, hInspectRotationAngle, hInspectShiftAlongRow, hInspectShiftAlongCol;
		HTuple hCount, hArea, hDeviceCenterRow, hDeviceCenterCol;

		_FCI_Extract_LocationParameterData(pTrainingData->hTupleProjectionParam, pTrainingData->hTupleTeachDoc,
			pTrainingData->hTupleEdgeTeachTuple, pTrainingData->hTupleDatumTeachTuple, nDoc,
			&hIsSelected, &hInspectRotationAngle, &hInspectShiftAlongRow, &hInspectShiftAlongCol);

		_FCI_Select_DeviceLocation(pTrainingData->hObjectDeviceLocation,
			&hDeviceLocationSelected,
			nDoc, &hDeviceCenterRow, &hDeviceCenterCol);

		
		CountObj(hDeviceLocationSelected, &hCount);
		if (hCount < 1)
			return 0;

		//AreaCenter(hDeviceLocationSelected, &hArea, &hDeviceCenterRow, &hDeviceCenterCol);

		HTuple hMinEncapContrast, hMaxEncapContrast;

		//// enacp teach region display ////
		if (m_TrainingData.bEnableEncap) {
			HTuple hEncapRectRows, hEncapRectCols;
			HTuple hEncapRectCenterRow, hEncapRectCenterCol;
			HTuple hEncapRecttoEdge_X_Shift, hEncapRecttoEdge_Y_Shift;
			HTuple hEncapTeachDoc;

			hEncapRectRows = hEncapRectCols = HTuple();
			hEncapTeachDoc = nDoc;

			HTuple hEncapRow1, hEncapCol1, hEncapRow2, hEncapCol2;
			hEncapRow1 = m_TrainingData.m_rectEncap[0].top;
			hEncapCol1 = m_TrainingData.m_rectEncap[0].left;
			hEncapRow2 = m_TrainingData.m_rectEncap[0].bottom;
			hEncapCol2 = m_TrainingData.m_rectEncap[0].right;

			HRegion hEncapTeachRegion, hProjectedRegion;

			GenRectangle1(&hEncapTeachRegion, hEncapRow1, hEncapCol1, hEncapRow2, hEncapCol2);

			////Encap Tracing Mask
			HRegion hConcatEncapMaskRgn;
			HTuple hEncapPolygonPoints;
			GenEmptyObj(&hConcatEncapMaskRgn);
			for (int nMask = 0; nMask < m_TrainingData.nEncapTracingMaskNumber; nMask++) {
				HRegion hEncapTracingMask;
				hEncapPolygonPoints = HTuple();
				if (m_TrainingData.nEncapTracingMaskType[nMask] == RECTANGLE) {
					hEncapRow1 = m_TrainingData.m_rectMaskEncapTracing[nMask].top;
					hEncapCol1 = m_TrainingData.m_rectMaskEncapTracing[nMask].left;
					hEncapRow2 = m_TrainingData.m_rectMaskEncapTracing[nMask].bottom;
					hEncapCol2 = m_TrainingData.m_rectMaskEncapTracing[nMask].right;
					GenRectangle1(&hEncapTracingMask, hEncapRow1, hEncapCol1, hEncapRow2, hEncapCol2);
				}
				else if (m_TrainingData.nEncapTracingMaskType[nMask] == ECLIPSE) {
					GenEllipse(&hEncapTracingMask, m_TrainingData.m_rectMaskEncapTracing[nMask].CenterPoint().y, m_TrainingData.m_rectMaskEncapTracing[nMask].CenterPoint().x, 0,
						m_TrainingData.m_rectMaskEncapTracing[nMask].Width() / 2, m_TrainingData.m_rectMaskEncapTracing[nMask].Height() / 2);
				}
				else if (m_TrainingData.nEncapTracingMaskType[nMask] == POLYGON) {
					HTuple hPoint;
					HTuple hRows, hCols;
					for (int nPointId = 0; nPointId < m_TrainingData.nEncapTracingPolygonPointNumber[nMask]; nPointId++) {
						TupleConcat(hRows, m_TrainingData.m_EncapTracingPolygonPointArea[nMask][nPointId].y, &hRows);
						TupleConcat(hCols, m_TrainingData.m_EncapTracingPolygonPointArea[nMask][nPointId].x, &hCols);
						
						hPoint.Clear();
						hPoint.Append(m_TrainingData.m_EncapTracingPolygonPointArea[nMask][nPointId].y);
						hPoint.Append(m_TrainingData.m_EncapTracingPolygonPointArea[nMask][nPointId].x);

						//TupleConcat(hEncapPolygonPoints, hPoint, &hEncapPolygonPoints);
					}
					GenRegionPolygonFilled(&hEncapTracingMask, hRows, hCols);
				}
				ConcatObj(hConcatEncapMaskRgn, hEncapTracingMask, &hConcatEncapMaskRgn);
			}
			
			if (m_TrainingData.nEncapTracingMaskNumber > 0) {
				HRegion hFitEncapTracingMaskRegion;
				GenEmptyObj(&hFitEncapTracingMaskRegion);
				Union1(hConcatEncapMaskRgn, &hFitEncapTracingMaskRegion);
				Difference(hEncapTeachRegion, hFitEncapTracingMaskRegion, &hEncapTeachRegion);

				m_arrayOverlayTeach.Add(hFitEncapTracingMaskRegion, colorRed);
			}
			

			HImage hDebugImageOutEncap;
			HRegion hDebugRegionOutEncap;
			HTuple hDebugMsgOutEncap;
			
			_FCI_Encap_RegionProjection(hImages,hEncapTeachRegion,
				&hProjectedRegion, &hDebugImageOutEncap,&hDebugRegionOutEncap,nStepDebug,
				hDeviceCenterRow, hDeviceCenterCol, hInspectRotationAngle, hInspectShiftAlongRow, hInspectShiftAlongCol,&hDebugMsgOutEncap);

			HTuple hWireColor;

			hMinEncapContrast = m_TrainingData.nEncapMinContrast;
			hMaxEncapContrast = m_TrainingData.nEncapMaxContrast;
			hWireColor = nCurTrack;

			HImage hDebugImg;
			HRegion hEncapLocation, hDebugRgn;
			HTuple hIsEncap, hDebugMsg;

			_FCI_Inspect_EncapLocation(hImages, hDeviceLocationSelected, hProjectedRegion,
				&hEncapLocation,
				&hDebugImg, &hDebugRgn,
				nStepDebug, hWireColor, hMinEncapContrast, hMaxEncapContrast,
				&hIsEncap, &hDebugMsg);

			m_arrayOverlayTeach.Add(hEncapTeachRegion, colorYellow);
			if (hIsEncap == TRUE)
				m_arrayOverlayTeach.Add(hEncapLocation, colorOrange);
			//Encap Tracing Mask
			//HRegion hConcatEncapMaskRgn;
			//HTuple hEncapPolygonPoints;
			//GenEmptyObj(&hConcatEncapMaskRgn);
			//for (int nMask = 0; nMask < m_TrainingData.nEncapTracingMaskNumber; nMask++) {
			//	HRegion hEncapTracingMask;
			//	hEncapPolygonPoints = HTuple();
			//	if (m_TrainingData.nEncapTracingMaskType[nMask] == RECTANGLE) {
			//		hEncapRow1 = m_TrainingData.m_rectMaskEncapTracing[nMask].top;
			//		hEncapCol1 = m_TrainingData.m_rectMaskEncapTracing[nMask].left;
			//		hEncapRow2 = m_TrainingData.m_rectMaskEncapTracing[nMask].bottom;
			//		hEncapCol2 = m_TrainingData.m_rectMaskEncapTracing[nMask].right;
			//		GenRectangle1(&hEncapTracingMask, hEncapRow1, hEncapCol1, hEncapRow2, hEncapCol2);
			//	}
			//	else if (m_TrainingData.nEncapTracingMaskType[nMask] == ECLIPSE) {
			//		GenEllipse(&hEncapTracingMask, m_TrainingData.m_rectMaskEncapTracing[nMask].CenterPoint().y, m_TrainingData.m_rectMaskEncapTracing[nMask].CenterPoint().x, 0,
			//			m_TrainingData.m_rectMaskEncapTracing[nMask].Width() / 2, m_TrainingData.m_rectMaskEncapTracing[nMask].Height() / 2);
			//	}
			//	else if (m_TrainingData.nEncapTracingMaskType[nMask] == POLYGON) {
			//		HTuple hPoint;
			//		HTuple hRows, hCols;
			//		for (int nPointId = 0; nPointId < m_TrainingData.nEncapTracingPolygonPointNumber[nMask]; nPointId++) {
			//			TupleConcat(hRows, m_TrainingData.m_EncapTracingPolygonPointArea[nMask][nPointId].y, &hRows);
			//			TupleConcat(hCols, m_TrainingData.m_EncapTracingPolygonPointArea[nMask][nPointId].x, &hCols);

			//			hPoint.Clear();
			//			hPoint.Append(m_TrainingData.m_EncapTracingPolygonPointArea[nMask][nPointId].y);
			//			hPoint.Append(m_TrainingData.m_EncapTracingPolygonPointArea[nMask][nPointId].x);

			//			//TupleConcat(hEncapPolygonPoints, hPoint, &hEncapPolygonPoints);
			//		}
			//		GenRegionPolygonFilled(&hEncapTracingMask, hRows, hCols);
			//	}
			//	ConcatObj(hConcatEncapMaskRgn, hEncapTracingMask, &hConcatEncapMaskRgn);
			//}

			//if (m_TrainingData.nEncapTracingMaskNumber > 0) {
			//	HRegion hFitEncapTracingMaskRegion;
			//	GenEmptyObj(&hFitEncapTracingMaskRegion);
			//	Union1(hConcatEncapMaskRgn, &hFitEncapTracingMaskRegion);
			//	Difference(hEncapTeachRegion, hFitEncapTracingMaskRegion, &hEncapTeachRegion);
			//	m_arrayOverlayTeach.Add(hFitEncapTracingMaskRegion, colorRed);
			//}
		}


		/*int nIndexEncap = 0;
		for (int i = 0; i < hMinEncapContrast.Length(); i++)
			m_TrainingData.darrayEncap[nIndexEncap].Add(hMinEncapContrast[i].D());
		m_TrainingData.hvec_TupleVectorEncap.InsertAt(nIndexEncap++, hMinEncapContrast);

		for (int i = 0; i < hMaxEncapContrast.Length(); i++)
			m_TrainingData.darrayEncap[nIndexEncap].Add(hMaxEncapContrast[i].D());
		m_TrainingData.hvec_TupleVectorEncap.InsertAt(nIndexEncap++, hMaxEncapContrast);*/

	}
	else if (nTeachStep == 3) {

		if(bEnable[2]) {
	
			HTuple hEncapRow1, hEncapCol1, hEncapRow2, hEncapCol2;

			HRegion hEncapRgn;
			HTuple hWireColor;
			hWireColor = nCurTrack;

			HImage hDebugImg;
			HRegion hEncapLocation, hDebugRgn;
			HTuple hIsEncap, hDebugMsg;

			for (int z = 1; z < 3; z++) {
				hEncapRow1 = m_TrainingData.m_rectEncap[z].top;
				hEncapCol1 = m_TrainingData.m_rectEncap[z].left;
				hEncapRow2 = m_TrainingData.m_rectEncap[z].bottom;
				hEncapCol2 = m_TrainingData.m_rectEncap[z].right;
				GenRectangle1(&hEncapRgn, hEncapRow1, hEncapCol1, hEncapRow2, hEncapCol2);
				m_arrayOverlayTeach.Add(hEncapRgn, colorBlue);
			}

			//Encap Tracing Mask
			HRegion hConcatEncapMaskRgn;
			GenEmptyObj(&hConcatEncapMaskRgn);
			for (int nMask = 0; nMask < m_TrainingData.nEncapSurfaceMaskNumber; nMask++) {
				HRegion hEncapSurfaceMask;
				hEncapRow1 = m_TrainingData.m_rectMaskEncapSurface[nMask].top;
				hEncapCol1 = m_TrainingData.m_rectMaskEncapSurface[nMask].left;
				hEncapRow2 = m_TrainingData.m_rectMaskEncapSurface[nMask].bottom;
				hEncapCol2 = m_TrainingData.m_rectMaskEncapSurface[nMask].right;
				GenRectangle1(&hEncapSurfaceMask, hEncapRow1, hEncapCol1, hEncapRow2, hEncapCol2);
				ConcatObj(hConcatEncapMaskRgn, hEncapSurfaceMask, &hConcatEncapMaskRgn);
			}
			
			if (m_TrainingData.nEncapSurfaceMaskNumber > 0) {
				m_arrayOverlayTeach.Add(hConcatEncapMaskRgn, colorRed);
			}
		}

		int nIndexEncap = 2;
		//for (int i = 0; i < hEncapInspCenterRow.Length(); i++)
		//	m_TrainingData.darrayEncap[nIndexEncap].Add(hEncapInspCenterRow[i].D());
		//m_TrainingData.hvec_TupleVectorEncap.InsertAt(nIndexEncap++, hEncapInspCenterRow);

	}
	else if (nTeachStep == 4) {
		HTuple hCLTeachDoc;
		HTuple hMaxRotAngle, PatternSearchAreaX, PatternSearchAreaY, hMinAccScore;
		HTuple hCLDatumRow, hCLDatumCol, hCLModelID, hMatchAngle, hDatumMatchCR, hDatumMatchCC;

		hCLTeachDoc = HTuple();// -1;
		hMaxRotAngle = PatternSearchAreaX = PatternSearchAreaY = hMinAccScore = HTuple();
		hCLDatumRow = hCLDatumCol = hCLModelID = HTuple();

		if (bEnable[3]) {
			hCLTeachDoc = nDoc;
	
			HTuple hRow, hCol = HTuple();
			HTuple hRow1, hRow2, hCol1, hCol2;
			HRegion hRect, hProjectedRect;
			HTuple hIsDatumFound, hMatchModelScore, hDebugMessageOut;

			if (m_TrainingData.bEnableCLDatum) {

				hMaxRotAngle = m_TrainingData.nCLMaxRotationAngle;
				PatternSearchAreaX = m_TrainingData.nCLPatternSearchX / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
				PatternSearchAreaY = m_TrainingData.nCLPatternSearchY / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
				hMinAccScore = m_TrainingData.nCLMinAcceptanceScore/100.0;

				for (int i = 0; i<m_TrainingData.nCLDatum; i++) {
					TupleConcat(hRow, m_TrainingData.m_rectDeviceLoc.top, &hRow);
					TupleConcat(hRow, m_TrainingData.m_rectDeviceLoc.bottom, &hRow);
					TupleConcat(hCol, m_TrainingData.m_rectDeviceLoc.left, &hCol);
					TupleConcat(hCol, m_TrainingData.m_rectDeviceLoc.right, &hCol);
				}

				HTuple hNumDatum = m_TrainingData.nCLDatum;
				HTuple hMaxRotationAngle = m_TrainingData.nCLMaxRotationAngle;
				HTuple hMinSearchWidth = m_TrainingData.nCLPatternSearchX / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
				HTuple hMinSearchHeight = m_TrainingData.nCLPatternSearchY / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
				HTuple hMinAcceptanceScore = m_TrainingData.nCLMinAcceptanceScore;

				_FCI_TeachUniquePattern(hImages, hNumDatum, hMaxRotationAngle, hRow, hCol,
					&hCLModelID);

				//HTuple
				HRegion hModelRegion, hDebugImage, hDebugRegion;

				_FCI_Inspect_UniquePattern(hImages,
					&hModelRegion, &hDebugImage, &hDebugRegion, nStepDebug,
					hMinSearchWidth, hMinSearchHeight, hMaxRotationAngle,
					hMinAcceptanceScore, hCLModelID, hRow, hCol,
					&hIsDatumFound, &hMatchModelScore, &hMatchAngle,
					&hDatumMatchCR, &hDatumMatchCC, &hDebugMessageOut);
			}

			for (int i = 0; i<m_TrainingData.nCLInspRegion; i++) {
				HRegion hCLDefectRegion;
				HTuple hContrast = m_CoverLayerParm.nContrast[i];
				HTuple hMinLength = m_CoverLayerParm.nCoverLayerLength[i] / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
				HTuple hMinHeight = m_CoverLayerParm.nCoverLayerHeight[i] / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
				HTuple hMaxGap = m_CoverLayerParm.dMaskSize[i] / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
				HTuple hGapWidthTolerance = m_CoverLayerParm.dGapWidthTolerance[i] / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
				HTuple hGapHeightTolerance = m_CoverLayerParm.dGapHeightTolerance[i] / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
				HTuple hIsCoverLayer;
				HImage hDebugImageOutCL;
				HRegion hDebugRegionOutCL;
				HTuple hDebugMsgOutCL;

				GenRectangle1(&hRect, m_TrainingData.m_rectCoverLayer[i].top,
					m_TrainingData.m_rectCoverLayer[i].left,
					m_TrainingData.m_rectCoverLayer[i].bottom,
					m_TrainingData.m_rectCoverLayer[i].right);

				_FCI_Inspect_CoverLayer(hImages, hRect,
					&hCLDefectRegion, &hDebugImageOutCL, &hDebugRegionOutCL, nStepDebug,
					hContrast, hMaxGap, hGapWidthTolerance, hGapHeightTolerance, hMinLength, hMinHeight, nFOV, nTotalFOV,
					&hIsCoverLayer,&hDebugMsgOutCL);

				m_arrayOverlayTeach.Add(hCLDefectRegion, colorGreen);
			}

		}

		int nIndexCL = 0;

		m_TrainingData.hvec_TupleVectorCoverLayer.InsertAt(nIndexCL++, hCLModelID);
		m_TrainingData.hvec_TupleVectorCoverLayer.InsertAt(nIndexCL++, hMatchAngle);
		m_TrainingData.hvec_TupleVectorCoverLayer.InsertAt(nIndexCL++, hDatumMatchCR);
		m_TrainingData.hvec_TupleVectorCoverLayer.InsertAt(nIndexCL++, hDatumMatchCC);

	}
	else if (nTeachStep == 5) {
		HRegion hDeviceLocationSelected, hRect;
		HTuple hIsSelect, hAngleOfDeviation4EachIntensity;
		HTuple hDeviceCenterRow, hDeviceCenterCol;
		_FCI_Select_DeviceLocation(pTrainingData->hObjectDeviceLocation,
			&hDeviceLocationSelected,
			nDoc, &hDeviceCenterRow, &hDeviceCenterCol);

		if (hIsSelect == FALSE)
			return 0;

		//HTuple hXShift, hYShift;
		/*_FCI_GetRegionToDeviceRelativeDistance(hDeviceLocationSelected,
			m_TrainingData.m_rectTilt.top,
			m_TrainingData.m_rectTilt.left,
			m_TrainingData.m_rectTilt.bottom,
			m_TrainingData.m_rectTilt.right,
			m_TrainingData.m_rectTilt.CenterPoint().x,
			m_TrainingData.m_rectTilt.CenterPoint().y,
			&hXShift, &hYShift);*/

		/*m_TrainingData.m_nXShift = hXShift;
		m_TrainingData.m_nYShift = hYShift;*/
		GenRectangle1(&hRect, m_TrainingData.m_rectTilt.top,
			m_TrainingData.m_rectTilt.left,
			m_TrainingData.m_rectTilt.bottom,
			m_TrainingData.m_rectTilt.right);
		
		m_arrayOverlayTeach.Add(hRect, colorYellow);
	}
	else if (nTeachStep == 6) {
		HTuple hEnablePointer, hNoOfPointers, hMaxRotationAngle, hPatternSearchAreaAlongX, hPatternSearchAreaAlongY, hMinAcceptanceScore, hEnableManualPointer;
		HTuple hModelIDs;
		HTuple hPointerRows = HTuple();
		HTuple hPointerCols = HTuple();
		
		hEnablePointer = m_TrainingData.bEnableCLAlignmentPointer;
		hNoOfPointers = m_TrainingData.nPointer;
		hMaxRotationAngle = 10;// m_TrainingData.nCLAP_MaxRotationAngle;
		hPatternSearchAreaAlongX = 10;//m_TrainingData.nCLAP_PatternSearchX / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
		hPatternSearchAreaAlongY = 10;// m_TrainingData.nCLAP_PatternSearchY / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
		hMinAcceptanceScore = (m_TrainingData.nCLAP_MinAcceptanceScore / 100.0);
		hEnableManualPointer = m_TrainingData.bEnableManualPointer;

		int nIndexCLAP = 0;

		m_TrainingData.hvec_TupleVectorCoverLayerPointer.InsertAt(nIndexCLAP++, hEnablePointer);
		m_TrainingData.hvec_TupleVectorCoverLayerPointer.InsertAt(nIndexCLAP++, hNoOfPointers);
		m_TrainingData.hvec_TupleVectorCoverLayerPointer.InsertAt(nIndexCLAP++, hMaxRotationAngle);
		m_TrainingData.hvec_TupleVectorCoverLayerPointer.InsertAt(nIndexCLAP++, hPatternSearchAreaAlongX);
		m_TrainingData.hvec_TupleVectorCoverLayerPointer.InsertAt(nIndexCLAP++, hPatternSearchAreaAlongY);
		m_TrainingData.hvec_TupleVectorCoverLayerPointer.InsertAt(nIndexCLAP++, hMinAcceptanceScore);
		m_TrainingData.hvec_TupleVectorCoverLayerPointer.InsertAt(nIndexCLAP++, hEnableManualPointer);

		for (int i = 0; i<hNoOfPointers; i++) {
			TupleConcat(hPointerRows, m_TrainingData.m_rectPointer[i].top, &hPointerRows);
			TupleConcat(hPointerRows, m_TrainingData.m_rectPointer[i].bottom, &hPointerRows);
			TupleConcat(hPointerCols, m_TrainingData.m_rectPointer[i].left, &hPointerCols);
			TupleConcat(hPointerCols, m_TrainingData.m_rectPointer[i].right, &hPointerCols);

			_FCI_TeachUniquePattern(hImages,
				1, hMaxRotationAngle,
				hPointerRows, hPointerCols,
				&hModelIDs);

			//m_TrainingData.hvec_TupleVectorCoverLayerPointer.InsertAt(nIndexCLAP++, hModelIDs);
			TupleConcat(m_TrainingData.hTuplePointerModelIDs, hModelIDs, &m_TrainingData.hTuplePointerModelIDs);

			HTuple hIsDatumFound, hDummy, hDebugMessageOut, hv_MatchRow, hv_MatchColumn, hv_MatchAngle, hv_MatchScore;
			HRegion hPointerRegion, hDebugRegionOut;
			HImage hDebugImageOut;

			_FCI_Inspect_UniquePattern(hImages,
				&hPointerRegion, &hDebugImageOut, &hDebugRegionOut,
				nStepDebug, hPatternSearchAreaAlongX, hPatternSearchAreaAlongY, hMaxRotationAngle,
				hMinAcceptanceScore, hModelIDs, hPointerRows, hPointerCols,
				&hIsDatumFound, &hv_MatchScore, &hv_MatchAngle, &hv_MatchRow, &hv_MatchColumn, &hDebugMessageOut);

			hPointerRows = HTuple();
			hPointerCols = HTuple();

			m_arrayOverlayTeach.Add(hPointerRegion, colorGreen);
		}
	}
	else if (nTeachStep >= 7 && nTeachStep <= 11) {
		int nSlotIndex = nTeachStep - 7;

		HTuple hMinIntensity, hContrastEdgeSlot, IsPass, hDebugMessageOut;
		HRegion hSlotROI, hSlotLocation, hDebugImageOut, hDebugRegionOut;
		GenEmptyObj(&hSlotROI);

		if (m_TrainingData.nSlotAreaType == RECTANGLE) {
			HTuple hSlotRow1, hSlotCol1, hSlotRow2, hSlotCol2;
			hSlotRow1 = m_TrainingData.m_rectSlot[nSlotIndex].top;
			hSlotCol1 = m_TrainingData.m_rectSlot[nSlotIndex].left;
			hSlotRow2 = m_TrainingData.m_rectSlot[nSlotIndex].bottom;
			hSlotCol2 = m_TrainingData.m_rectSlot[nSlotIndex].right;
			GenRectangle1(&hSlotROI, hSlotRow1, hSlotCol1, hSlotRow2, hSlotCol2);
		}
		//Polygon Type
		else {
			HTuple hRows, hCols;
			for (int nPointId = 0; nPointId < m_TrainingData.nSlotPolygonPointCount; nPointId++) {
				TupleConcat(hRows, m_TrainingData.m_SlotPolygonPoint[nSlotIndex][nPointId].y, &hRows);
				TupleConcat(hCols, m_TrainingData.m_SlotPolygonPoint[nSlotIndex][nPointId].x, &hCols);
			}
			GenRegionPolygonFilled(&hSlotROI, hRows, hCols);
		}
		
		hMinIntensity = m_TrainingData.nMinIntensitySlot;
		hContrastEdgeSlot = m_TrainingData.nContrastEdgeSlot;

		_FCI_Inspect_SlotLocation(hImages, hSlotROI,
			&hSlotLocation, &hDebugImageOut, &hDebugRegionOut,
			0, hMinIntensity, hContrastEdgeSlot,
			&IsPass, &hDebugMessageOut);
		if (IsPass == 1) {
			m_arrayOverlayTeach.Add(hSlotLocation, colorOrange);
		}

		
		HRegion hSlotLimitLocation;
		GenEmptyObj(&hSlotLimitLocation);

		if (m_TrainingData.nSlotLimitAreaType == RECTANGLE) {
			HTuple hSlotLimitRow1, hSlotLimitCol1, hSlotLimitRow2, hSlotLimitCol2;
			hSlotLimitRow1 = m_TrainingData.m_rectSlotLimit[nSlotIndex].top;
			hSlotLimitCol1 = m_TrainingData.m_rectSlotLimit[nSlotIndex].left;
			hSlotLimitRow2 = m_TrainingData.m_rectSlotLimit[nSlotIndex].bottom;
			hSlotLimitCol2 = m_TrainingData.m_rectSlotLimit[nSlotIndex].right;
			GenRectangle1(&hSlotLimitLocation, hSlotLimitRow1, hSlotLimitCol1, hSlotLimitRow2, hSlotLimitCol2);
		}
		//Polygon Type
		else {
			HTuple hRows, hCols;
			for (int nPointId = 0; nPointId < m_TrainingData.nSlotLimitPolygonPointCount; nPointId++) {
				TupleConcat(hRows, m_TrainingData.m_SlotLimitPolygonPoint[nSlotIndex][nPointId].y, &hRows);
				TupleConcat(hCols, m_TrainingData.m_SlotLimitPolygonPoint[nSlotIndex][nPointId].x, &hCols);
			}
			GenRegionPolygonFilled(&hSlotLimitLocation, hRows, hCols);
		}
		
		m_arrayOverlayTeach.Add(hSlotLimitLocation, colorBlue);
	}
	else if (nTeachStep == 12) {
		HTuple hSlotRow1, hSlotCol1, hSlotRow2, hSlotCol2;
		HRegion hConcatSlotMaskRgn;
		HTuple hSlotPolygonPoints;
		GenEmptyObj(&hConcatSlotMaskRgn);
		for (int nMask = 0; nMask < m_TrainingData.nSlotMaskNumber; nMask++) {
			HRegion hSlotMask;
			hSlotPolygonPoints = HTuple();
			if (m_TrainingData.nSlotMaskType[nMask] == RECTANGLE) {
				hSlotRow1 = m_TrainingData.m_rectMaskSlot[nMask].top;
				hSlotCol1 = m_TrainingData.m_rectMaskSlot[nMask].left;
				hSlotRow2 = m_TrainingData.m_rectMaskSlot[nMask].bottom;
				hSlotCol2 = m_TrainingData.m_rectMaskSlot[nMask].right;
				GenRectangle1(&hSlotMask, hSlotRow1, hSlotCol1, hSlotRow2, hSlotCol2);
			}
			else if (m_TrainingData.nSlotMaskType[nMask] == ECLIPSE) {
				GenEllipse(&hSlotMask, m_TrainingData.m_rectMaskSlot[nMask].CenterPoint().y, m_TrainingData.m_rectMaskSlot[nMask].CenterPoint().x, 0,
					m_TrainingData.m_rectMaskSlot[nMask].Width() / 2, m_TrainingData.m_rectMaskSlot[nMask].Height() / 2);
			}
			else if (m_TrainingData.nSlotMaskType[nMask] == POLYGON) {
				HTuple hPoint;
				HTuple hRows, hCols;
				for (int nPointId = 0; nPointId < m_TrainingData.nSlotMaskPolygonPointCount[nMask]; nPointId++) {
					TupleConcat(hRows, m_TrainingData.m_SlotMaskPolygonPoint[nMask][nPointId].y, &hRows);
					TupleConcat(hCols, m_TrainingData.m_SlotMaskPolygonPoint[nMask][nPointId].x, &hCols);

					hPoint.Clear();
					hPoint.Append(m_TrainingData.m_SlotMaskPolygonPoint[nMask][nPointId].y);
					hPoint.Append(m_TrainingData.m_SlotMaskPolygonPoint[nMask][nPointId].x);

					//TupleConcat(hSlotPolygonPoints, hPoint, &hSlotPolygonPoints);
				}
				GenRegionPolygonFilled(&hSlotMask, hRows, hCols);
			}
			ConcatObj(hConcatSlotMaskRgn, hSlotMask, &hConcatSlotMaskRgn);
		}
		m_arrayOverlayTeach.Add(hConcatSlotMaskRgn, colorYellow);
	}
	//ReleaseMemory();
	return 0;
}

int CInspectionCore::AutoTeach(HImage hImages,
						  BOOL bRegionTeach,
						  int nCurTrack,
					      int nDoc,
						  int nFOV,
						  int nTotalFOV,
						  CCalibrationData* pCalibData)
{
	int nStepDebug = 0;
	
	if(!bRegionTeach) {

		HTuple hUniqueNoPatternsRows, hUniqueNoPatternsCols;
		HTuple hLocationTeachRows, hLocationTeachCols = HTuple ();
		HTuple hModelIDs;
		HTuple hGotRotationAngleDev, hGotShiftAlongRow, hGotShiftAlongCol;

		HTuple hIsDatum, hMatchModelScore, hDatumRefRotationAngle, hDatumRefPointRow, hDatumRefPointCol, hDebugMessageOut;
		HImage hDebugImageOut;
		HRegion hMatchModelRegion, hDebugRegionOut;

		// Unique Pattern
		HTuple hDatumSearchEnable, hNoOfUniqueRegions, hMaxRotationAngle, hPatternSearchAreaAlongX, hPatternSearchAreaAlongY, hMinAcceptanceScore;
		hDatumSearchEnable = m_TrainingData.bDattamSearchEnable;
		hNoOfUniqueRegions = m_TrainingData.nDattamSearchNoOfDattams;
		hMaxRotationAngle = m_TrainingData.nDattamSearchMaxRotAngle;
		hPatternSearchAreaAlongX = m_TrainingData.dDattamSearchAreaX / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
		hPatternSearchAreaAlongY = m_TrainingData.dDattamSearchAreaY / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
		hMinAcceptanceScore = (m_TrainingData.dDattamSearchMinAcceptanceScore / 100.0);
		if (hDatumSearchEnable == 1) {
			for (int i = 0; i<m_TrainingData.nDattamSearchNoOfDattams; i++) {
				TupleConcat(hUniqueNoPatternsRows, m_TrainingData.m_rectDattamSearch[i].top, &hUniqueNoPatternsRows);
				TupleConcat(hUniqueNoPatternsRows, m_TrainingData.m_rectDattamSearch[i].bottom, &hUniqueNoPatternsRows);
				TupleConcat(hUniqueNoPatternsCols, m_TrainingData.m_rectDattamSearch[i].left, &hUniqueNoPatternsCols);
				TupleConcat(hUniqueNoPatternsCols, m_TrainingData.m_rectDattamSearch[i].right, &hUniqueNoPatternsCols);
			}

			_FCI_TeachUniquePattern(hImages,
				hNoOfUniqueRegions, hMaxRotationAngle,
				hUniqueNoPatternsRows, hUniqueNoPatternsCols,
				&hModelIDs);

			_FCI_Inspect_UniquePattern(hImages,
				&hMatchModelRegion, &hDebugImageOut, &hDebugRegionOut,
				nStepDebug, hPatternSearchAreaAlongX, hPatternSearchAreaAlongY, hMaxRotationAngle,
				hMinAcceptanceScore, hModelIDs, hUniqueNoPatternsRows, hUniqueNoPatternsCols,
				&hIsDatum, &hMatchModelScore, &hDatumRefRotationAngle, &hDatumRefPointRow, &hDatumRefPointCol, &hDebugMessageOut);
		}

		HRegion hDeviceLocation;
		HTuple hDeviceLocationEnable, hNoOfLocationRegions, hDeviceLocWidth, hDeviceLocHeight, hDeviceMaskSize, hMinDeviceContrast, hSobelAmp, hReferencePointType;
		HTuple hDebugMsgOut, hIsPass, hEdgeRefRotationAngle, hEdgeRefPointRow, hEdgeRefPointCol;

		HTuple hDeviceEdgeDetectionEnable, hDeviceHorizontalContrast, hDeviceHorizontalDirection, hDeviceVerticalContrast, hDeviceVerticalDirection;
		HTuple hDeviceEdgeLocationTeachRows, hDeviceEdgeLocationTeachCols;
		HTuple hDeviceEdgeDetectionSmoothingImage;

		hDeviceLocationEnable = m_TrainingData.bDeviceLocationEnable;
		hNoOfLocationRegions = m_TrainingData.nNoOfLocationTeachRegion;
		hDeviceLocWidth = m_TrainingData.dDeviceLocWidth / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
		hDeviceLocHeight = m_TrainingData.dDeviceLocHeight / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
		hDeviceMaskSize = m_TrainingData.dMaskSize / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
		hMinDeviceContrast = m_TrainingData.nMinDeviceContrast;
		hSobelAmp = m_TrainingData.bSobelAmp;
		hReferencePointType = m_TrainingData.nDeviceReferencePointType;

		hDeviceEdgeDetectionEnable = m_TrainingData.bDeviceEdgeDetectionEnable;
		hDeviceHorizontalContrast = m_TrainingData.nDeviceHorizontalContrast;
		hDeviceHorizontalDirection = m_TrainingData.nDeviceHorizontalDirection;
		hDeviceVerticalContrast = m_TrainingData.nDeviceVerticalContrast;
		hDeviceVerticalDirection = m_TrainingData.nDeviceVerticalDirection;

		hDeviceEdgeDetectionSmoothingImage = m_TrainingData.bUseSmoothingForEdgeDetection;

		HTuple hFOV, hFOVs, hTrack;
		hFOV = nFOV;
		hFOVs = nTotalFOV;
		hTrack = nCurTrack + 1;

		if (hDeviceLocationEnable == 1) {
			for (int i = 0; i<m_TrainingData.nNoOfLocationTeachRegion; i++) {
				TupleConcat(hLocationTeachRows, m_TrainingData.m_rectDeviceLoc.top, &hLocationTeachRows);
				TupleConcat(hLocationTeachRows, m_TrainingData.m_rectDeviceLoc.bottom, &hLocationTeachRows);
				TupleConcat(hLocationTeachCols, m_TrainingData.m_rectDeviceLoc.left, &hLocationTeachCols);
				TupleConcat(hLocationTeachCols, m_TrainingData.m_rectDeviceLoc.right, &hLocationTeachCols);
			}
			//Use edge detection to detect the device edges instead of blob detection Algo
			if (hDeviceEdgeDetectionEnable == 1) {

				TupleConcat(hDeviceEdgeLocationTeachRows, m_TrainingData.m_rectDeviceHorizontalLineLoc.top, &hDeviceEdgeLocationTeachRows);
				TupleConcat(hDeviceEdgeLocationTeachRows, m_TrainingData.m_rectDeviceHorizontalLineLoc.bottom, &hDeviceEdgeLocationTeachRows);
				TupleConcat(hDeviceEdgeLocationTeachRows, m_TrainingData.m_rectDeviceVerticalLineLoc.top, &hDeviceEdgeLocationTeachRows);
				TupleConcat(hDeviceEdgeLocationTeachRows, m_TrainingData.m_rectDeviceVerticalLineLoc.bottom, &hDeviceEdgeLocationTeachRows);

				TupleConcat(hDeviceEdgeLocationTeachCols, m_TrainingData.m_rectDeviceHorizontalLineLoc.left, &hDeviceEdgeLocationTeachCols);
				TupleConcat(hDeviceEdgeLocationTeachCols, m_TrainingData.m_rectDeviceHorizontalLineLoc.right, &hDeviceEdgeLocationTeachCols);
				TupleConcat(hDeviceEdgeLocationTeachCols, m_TrainingData.m_rectDeviceVerticalLineLoc.left, &hDeviceEdgeLocationTeachCols);
				TupleConcat(hDeviceEdgeLocationTeachCols, m_TrainingData.m_rectDeviceVerticalLineLoc.right, &hDeviceEdgeLocationTeachCols);


				_FCI_Inspect_Cross_Point(hImages, &hDebugImageOut, &hDebugRegionOut, hDeviceHorizontalContrast, hDeviceHorizontalDirection,
					hDeviceVerticalContrast, hDeviceVerticalDirection, hDeviceEdgeLocationTeachRows, hDeviceEdgeLocationTeachCols, hMaxRotationAngle, hDeviceEdgeDetectionSmoothingImage,
					0, &hEdgeRefRotationAngle, &hEdgeRefPointRow, &hEdgeRefPointCol, &hDebugMessageOut, &hIsPass);
			}
			else {
				_FCI_Inspect_EdgeLocation(hImages,
					&hDeviceLocation,
					hSobelAmp, hNoOfLocationRegions, hDeviceLocWidth, hDeviceLocHeight,
					hMinDeviceContrast, hLocationTeachRows, hLocationTeachCols, hDeviceMaskSize, hFOV, hFOVs, hTrack, hReferencePointType,
					&hEdgeRefRotationAngle, &hEdgeRefPointRow, &hEdgeRefPointCol);
			}
		}
		else {
			/*for (int i = 0; i < m_TrainingData.nNoOfLocationTeachRegion; i++) {
				TupleConcat(hLocationTeachRows, m_TrainingData.m_rectDeviceLoc.top, &hLocationTeachRows);
				TupleConcat(hLocationTeachRows, m_TrainingData.m_rectDeviceLoc.bottom, &hLocationTeachRows);
				TupleConcat(hLocationTeachCols, m_TrainingData.m_rectDeviceLoc.left, &hLocationTeachCols);
				TupleConcat(hLocationTeachCols, m_TrainingData.m_rectDeviceLoc.right, &hLocationTeachCols);
			}
			GenRectangle1(&hDeviceLocation, HTuple(hLocationTeachRows[0]), HTuple(hLocationTeachCols[0]),
				HTuple(hLocationTeachRows[1]), HTuple(hLocationTeachCols[1]));

			HTuple hv_Area, hv_Dummy, hv_Dummx, hv_DeviceAngle, hv_Device2DCenter, hv_DeviceSize;
			AreaCenter(hDeviceLocation, &hv_Area, &hv_Dummy, &hv_Dummx);
			if (0 != (HTuple(hv_Area > 0).TupleAnd((hv_Area.TupleLength()) > 0))) {
				EllipticAxis(hDeviceLocation, &hv_Dummy, &hv_Dummx, &hEdgeRefRotationAngle);

				TopReferencePoints(hImages, hDeviceLocation, hFOV, hFOVs, &hEdgeRefPointRow,
					&hEdgeRefPointCol, &hv_DeviceAngle, &hv_Device2DCenter, &hv_DeviceSize);
			}*/

			hEdgeRefPointRow = hEdgeRefPointCol = hEdgeRefRotationAngle;
		}
		//else {
		//	hLocationTeachRows = hLocationTeachCols = HTuple();//0;
		//}
		HTuple hRefRotationAngle, hRefPointRow, hRefPointCol, hGotRotationAngle;
		if (hDatumSearchEnable == 1 || hDeviceLocationEnable == 1)
		{
			if (hDatumSearchEnable == 1)
			{
				hRefRotationAngle = hDatumRefRotationAngle;
				hRefPointRow = hDatumRefPointRow;
				hRefPointCol = hDatumRefPointCol;
			}
			else
			{
				hRefRotationAngle = hEdgeRefRotationAngle;
				hRefPointRow = hEdgeRefPointRow;
				hRefPointCol = hEdgeRefPointCol;
			}

			HTuple hTopRefGotRotationAngle, hTopRefGotShiftAlongRow, hTopRefGotShiftAlongCol;

			_FCI_Inspect_DeviceLocation(hImages,
				&hDeviceLocation, &hMatchModelRegion,
				&hDebugImageOut, &hDebugRegionOut, hDatumSearchEnable, hDeviceLocationEnable, nStepDebug,
				hSobelAmp, hNoOfLocationRegions,
				hDeviceLocWidth, hDeviceLocHeight, hMinDeviceContrast,
				hNoOfUniqueRegions, hMaxRotationAngle,
				hPatternSearchAreaAlongX, hPatternSearchAreaAlongY, hMinAcceptanceScore,
				hModelIDs, hRefRotationAngle,
				hRefPointRow, hRefPointCol,
				hEdgeRefRotationAngle, hEdgeRefPointRow, hEdgeRefPointCol,
				hUniqueNoPatternsRows, hUniqueNoPatternsCols,
				hLocationTeachRows, hLocationTeachCols,
				hDeviceMaskSize, hFOV, hFOVs, hTrack, hReferencePointType,
				hDeviceEdgeDetectionEnable,
				hDeviceHorizontalContrast, hDeviceHorizontalDirection,
				hDeviceVerticalContrast, hDeviceVerticalDirection,
				hDeviceEdgeLocationTeachRows, hDeviceEdgeLocationTeachCols,
				hDeviceEdgeDetectionSmoothingImage,
				&hIsPass, &hIsDatum, &hGotRotationAngle, &hGotShiftAlongRow, &hGotShiftAlongCol,
				&hMatchModelScore,
				&hTopRefGotRotationAngle, &hTopRefGotShiftAlongRow, &hTopRefGotShiftAlongCol,
				&hDebugMsgOut);
		}


		int nIndex = 0;
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceLocationEnable);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hNoOfLocationRegions);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceLocWidth);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceLocHeight);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceMaskSize);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hMinDeviceContrast);

		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDatumSearchEnable);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hNoOfUniqueRegions);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hMaxRotationAngle);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hPatternSearchAreaAlongX);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hPatternSearchAreaAlongY);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hMinAcceptanceScore);

		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hUniqueNoPatternsRows);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hUniqueNoPatternsCols);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hLocationTeachRows);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hLocationTeachCols);

		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hModelIDs);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hRefRotationAngle);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hRefPointRow);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hRefPointCol);

		//For Device Reference Point
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hEdgeRefRotationAngle);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hEdgeRefPointRow);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hEdgeRefPointCol);

		//For Device Edge Detection option
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceEdgeDetectionEnable);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceHorizontalContrast);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceHorizontalDirection);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceVerticalContrast);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceVerticalDirection);

		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceEdgeLocationTeachRows);
		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceEdgeLocationTeachCols);

		m_TrainingData.hvec_TupleVectorLocation.SetAt(nIndex++, hDeviceEdgeDetectionSmoothingImage);
	}
	else {
		HTuple hPVIArea;
		//OutputDebugLogTo(9, TRUE, "Auto Teach Start");//// for testing

		int nIndex = 0;
		for(int i=0; i<m_TrainingData.darrayPVI[nIndex][0].GetCount(); i++)
			TupleConcat(hPVIArea, m_TrainingData.darrayPVI[nIndex][0][i], &hPVIArea);
		m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(0, hPVIArea);

		for (int nArea = 0; nArea < hPVIArea; nArea++) {
			//OutputDebugLogTo(9, TRUE, "Auto Teach PVIArea %.2f", hPVIArea.D());//// for testing
			HTuple hPVIAngle, hPVI2DCenter, hPVISize;
			HTuple hMaskNumber, hMask2DCenterPoints, hMaskAngleConcat, hMaskSize, hPolygonPoints;
			HTuple hSlotNumber, hSlot2DCenterPoints, hSlotAngleConcat, hSlotSize;
			nIndex = 1;
			for (int i = 0; i < m_TrainingData.darrayPVI[nIndex][nArea].GetCount(); i++)
				TupleConcat(hPVIAngle, m_TrainingData.darrayPVI[nIndex][nArea][i], &hPVIAngle);
			m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hPVIAngle);

			for (int i = 0; i < m_TrainingData.darrayPVI[nIndex][nArea].GetCount(); i++)
				TupleConcat(hPVI2DCenter, m_TrainingData.darrayPVI[nIndex][nArea][i], &hPVI2DCenter);
			m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hPVI2DCenter);

			for (int i = 0; i < m_TrainingData.darrayPVI[nIndex][nArea].GetCount(); i++)
				TupleConcat(hPVISize, m_TrainingData.darrayPVI[nIndex][nArea][i], &hPVISize);
			m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hPVISize);

			for (int i = 0; i < m_TrainingData.darrayPVI[nIndex][nArea].GetCount(); i++)
				TupleConcat(hMaskNumber, m_TrainingData.darrayPVI[nIndex][nArea][i], &hMaskNumber);
			m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hMaskNumber);

			for (int i = 0; i < m_TrainingData.darrayPVI[nIndex][nArea].GetCount(); i++)
				TupleConcat(hMask2DCenterPoints, m_TrainingData.darrayPVI[nIndex][nArea][i], &hMask2DCenterPoints);
			m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hMask2DCenterPoints);

			for (int i = 0; i < m_TrainingData.darrayPVI[nIndex][nArea].GetCount(); i++)
				TupleConcat(hMaskAngleConcat, m_TrainingData.darrayPVI[nIndex][nArea][i], &hMaskAngleConcat);
			m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hMaskAngleConcat);

			for (int i = 0; i < m_TrainingData.darrayPVI[nIndex][nArea].GetCount(); i++)
				TupleConcat(hMaskSize, m_TrainingData.darrayPVI[nIndex][nArea][i], &hMaskSize);
			m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hMaskSize);

			for (int i = 0; i < m_TrainingData.darrayPVI[nIndex][nArea].GetCount(); i++)
				TupleConcat(hSlotNumber, m_TrainingData.darrayPVI[nIndex][nArea][i], &hSlotNumber);
			m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hSlotNumber);

			for (int i = 0; i < m_TrainingData.darrayPVI[nIndex][nArea].GetCount(); i++)
				TupleConcat(hSlot2DCenterPoints, m_TrainingData.darrayPVI[nIndex][nArea][i], &hSlot2DCenterPoints);
			m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hSlot2DCenterPoints);

			for (int i = 0; i < m_TrainingData.darrayPVI[nIndex][nArea].GetCount(); i++)
				TupleConcat(hSlotAngleConcat, m_TrainingData.darrayPVI[nIndex][nArea][i], &hSlotAngleConcat);
 			m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hSlotAngleConcat);

			for (int i = 0; i < m_TrainingData.darrayPVI[nIndex][nArea].GetCount(); i++)
				TupleConcat(hSlotSize, m_TrainingData.darrayPVI[nIndex][nArea][i], &hSlotSize);
			m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hSlotSize);

			// For PVI Polygon
			for (int i = 0; i < m_TrainingData.darrayPVI[nIndex][nArea].GetCount(); i++)
				TupleConcat(hPolygonPoints, m_TrainingData.darrayPVI[nIndex][nArea][i], &hPolygonPoints);
			m_TrainingData.hvec_TupleVectorPVI[nIndex++].InsertAt(nArea, hPolygonPoints);
		}
			
		//// ---- encap auto teach part ---- ////
		//HTuple hMinEncapContrast, hMaxEncapContrast;

		/*int nIndexEncap = 0;
		for (int i = 0; i < m_TrainingData.darrayEncap[nIndexEncap].GetCount(); i++)
			TupleConcat(hMinEncapContrast, m_TrainingData.darrayEncap[nIndexEncap][i], &hMinEncapContrast);
		m_TrainingData.hvec_TupleVectorEncap.InsertAt(nIndexEncap++, hMinEncapContrast);

		for (int i = 0; i < m_TrainingData.darrayEncap[nIndexEncap].GetCount(); i++)
			TupleConcat(hMaxEncapContrast, m_TrainingData.darrayEncap[nIndexEncap][i], &hMaxEncapContrast);
		m_TrainingData.hvec_TupleVectorEncap.InsertAt(nIndexEncap++, hMaxEncapContrast);*/


		//// cover layer part ////
		HTuple hMaxRotAngle, PatternSearchAreaX, PatternSearchAreaY, hMinAccScore;
		HTuple hCLDatumRow, hCLDatumCol;

		HTuple hRow, hCol = HTuple();
		HTuple hRow1, hRow2, hCol1, hCol2;
		HRegion hRect, hProjectedRect;
		HTuple hCLModelID, hIsDatumFound, hMatchModelScore, hMatchAngle, hDatumMatchCR, hDatumMatchCC, hDebugMessageOut;

		if (m_TrainingData.bEnableCLDatum) {

			hMaxRotAngle = m_TrainingData.nCLMaxRotationAngle;
			PatternSearchAreaX = m_TrainingData.nCLPatternSearchX / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
			PatternSearchAreaY = m_TrainingData.nCLPatternSearchY / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
			hMinAccScore = m_TrainingData.nCLMinAcceptanceScore / 100.0;

			for (int i = 0; i<m_TrainingData.nCLDatum; i++) {
				TupleConcat(hRow, m_TrainingData.m_rectDeviceLoc.top, &hRow);
				TupleConcat(hRow, m_TrainingData.m_rectDeviceLoc.bottom, &hRow);
				TupleConcat(hCol, m_TrainingData.m_rectDeviceLoc.left, &hCol);
				TupleConcat(hCol, m_TrainingData.m_rectDeviceLoc.right, &hCol);
			}

			HTuple hNumDatum = m_TrainingData.nCLDatum;
			HTuple hMaxRotationAngle = m_TrainingData.nCLMaxRotationAngle;
			HTuple hMinSearchWidth = m_TrainingData.nCLPatternSearchX / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
			HTuple hMinSearchHeight = m_TrainingData.nCLPatternSearchY / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
			HTuple hMinAcceptanceScore = m_TrainingData.nCLMinAcceptanceScore;

			_FCI_TeachUniquePattern(hImages, hNumDatum, hMaxRotationAngle, hRow, hCol,
				&hCLModelID);

			//HTuple
			HRegion hModelRegion, hDebugImage, hDebugRegion;

			_FCI_Inspect_UniquePattern(hImages,
				&hModelRegion, &hDebugImage, &hDebugRegion, nStepDebug,
				hMinSearchWidth, hMinSearchHeight, hMaxRotationAngle,
				hMinAcceptanceScore, hCLModelID, hRow, hCol,
				&hIsDatumFound, &hMatchModelScore, &hMatchAngle,
				&hDatumMatchCR, &hDatumMatchCC, &hDebugMessageOut);
		}

		int nIndexCL = 0;

		m_TrainingData.hvec_TupleVectorCoverLayer.InsertAt(nIndexCL++, hCLModelID);
		m_TrainingData.hvec_TupleVectorCoverLayer.InsertAt(nIndexCL++, hMatchAngle);
		m_TrainingData.hvec_TupleVectorCoverLayer.InsertAt(nIndexCL++, hDatumMatchCR);
		m_TrainingData.hvec_TupleVectorCoverLayer.InsertAt(nIndexCL++, hDatumMatchCC);

		HTuple hEnablePointer, hNoOfPointers, hMaxRotationAngle, hPatternSearchAreaAlongX, hPatternSearchAreaAlongY, hMinAcceptanceScore, hEnableManualPointer;
		HTuple hModelIDs;
		HTuple hPointerRows = HTuple();
		HTuple hPointerCols = HTuple();

		if (m_TrainingData.bEnableCLAlignmentPointer)
		{
			hEnablePointer = m_TrainingData.bEnableCLAlignmentPointer;
			hNoOfPointers = m_TrainingData.nPointer;
			hMaxRotationAngle = m_TrainingData.nCLAP_MaxRotationAngle;
			hPatternSearchAreaAlongX = m_TrainingData.nCLAP_PatternSearchX / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
			hPatternSearchAreaAlongY = m_TrainingData.nCLAP_PatternSearchY / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
			hMinAcceptanceScore = (m_TrainingData.nCLAP_MinAcceptanceScore / 100.0);
			hEnableManualPointer = m_TrainingData.bEnableManualPointer;

			int nIndexCLAP = 0;

			m_TrainingData.hvec_TupleVectorCoverLayerPointer.InsertAt(nIndexCLAP++, hEnablePointer);
			m_TrainingData.hvec_TupleVectorCoverLayerPointer.InsertAt(nIndexCLAP++, hNoOfPointers);
			m_TrainingData.hvec_TupleVectorCoverLayerPointer.InsertAt(nIndexCLAP++, hMaxRotationAngle);
			m_TrainingData.hvec_TupleVectorCoverLayerPointer.InsertAt(nIndexCLAP++, hPatternSearchAreaAlongX);
			m_TrainingData.hvec_TupleVectorCoverLayerPointer.InsertAt(nIndexCLAP++, hPatternSearchAreaAlongY);
			m_TrainingData.hvec_TupleVectorCoverLayerPointer.InsertAt(nIndexCLAP++, hMinAcceptanceScore);
			m_TrainingData.hvec_TupleVectorCoverLayerPointer.InsertAt(nIndexCLAP++, hEnableManualPointer);

			for (int i = 0; i < hNoOfPointers; i++) {
				TupleConcat(hPointerRows, m_TrainingData.m_rectPointer[i].top, &hPointerRows);
				TupleConcat(hPointerRows, m_TrainingData.m_rectPointer[i].bottom, &hPointerRows);
				TupleConcat(hPointerCols, m_TrainingData.m_rectPointer[i].left, &hPointerCols);
				TupleConcat(hPointerCols, m_TrainingData.m_rectPointer[i].right, &hPointerCols);

				_FCI_TeachUniquePattern(hImages,
					1, hMaxRotationAngle,
					hPointerRows, hPointerCols,
					&hModelIDs);

				TupleConcat(m_TrainingData.hTuplePointerModelIDs, hModelIDs, &m_TrainingData.hTuplePointerModelIDs);
				//m_TrainingData.hvec_TupleVectorCoverLayerPointer.InsertAt(nIndexCLAP++, hModelIDs);

				hPointerRows = HTuple();
				hPointerCols = HTuple();
			}
		}
	}
	return 0;
}

int CInspectionCore::ClearTeachParam(CTrainingData* pTrainingData)
{
	if(pTrainingData != NULL) {
		GenEmptyObj(&pTrainingData->hObjectDeviceLocation);
		pTrainingData->hTupleProjectionParam.Clear();
		pTrainingData->hTupleProjectionParam = HTuple();
		pTrainingData->hTupleTeachDoc.Clear();
		pTrainingData->hTupleTeachDoc = HTuple();
		pTrainingData->hTupleEdgeTeachTuple.Clear();
		pTrainingData->hTupleEdgeTeachTuple = HTuple();
		pTrainingData->hTupleDatumTeachTuple.Clear();
		pTrainingData->hTupleDatumTeachTuple = HTuple();

	}
	else {
		for(int i=0; i<32; i++) {
			m_TrainingData.hvec_TupleVectorPVI[i].RemoveAll();
			//m_TrainingData.hvec_TupleVectorPVI[i].Add(HTuple());
			for(int j=0; j<10; j++)
				m_TrainingData.darrayPVI[i][j].RemoveAll();
		}
		m_TrainingData.hvec_TupleVectorEncap.RemoveAll();
		m_TrainingData.hvec_TupleVectorCoverLayer.RemoveAll();
		for (int k = 0; k < 15; k++) {
			m_TrainingData.darrayEncap[k].RemoveAll();
			m_TrainingData.darrayCoverLayer[k].RemoveAll();
		}
		//OutputDebugLogTo(9, TRUE, "ClearTeachParam");//// for testing
	}
	return 0;
}

int CInspectionCore::ClearInspParam(CInspectinonData* pInspData, BOOL bInspClearAll)
{
	if (!bInspClearAll) {
		pInspData->hTiltHeight.Clear();
		pInspData->hTiltHeight = HTuple();
		//OutputDebugLogTo(9, TRUE, "ClearInspParam");//// for testing
	}
	else {

		pInspData->hStoreTeachModelRowCentre.Clear();
		pInspData->hStoreTeachModelRowCentre = HTuple();
		pInspData->hStoreTeachModelColCentre.Clear();
		pInspData->hStoreTeachModelColCentre = HTuple();
		pInspData->hStoreMatchModelCentreRow.Clear();
		pInspData->hStoreMatchModelCentreRow = HTuple();
		pInspData->hStoreMatchModelCentreCol.Clear();
		pInspData->hStoreMatchModelCentreCol = HTuple();
		//OutputDebugLogTo(9, TRUE, "ClearInspParam All");//// for testing
	}

	return 0;
}

int CInspectionCore::ClearLightCorrectionParam(CCalibrationData* pCalibData)
{
	pCalibData->hLightCoefficient.Clear();
	return 0;
}

int CInspectionCore::ClearCalibParam(CCalibrationData* pCalibData, BOOL bClearAvg, BOOL bClearInsp)
{
	if (!bClearInsp) {
		GenEmptyObj(&pCalibData->hCalibSumImage);
		if (bClearAvg)
			GenEmptyObj(&pCalibData->hCalibAvgImage);
	}
	else if (bClearInsp) {
		pCalibData->hResolutionYInput.Clear();
		pCalibData->hResolutionYInput = HTuple();
		//OutputDebugLogTo(9, TRUE, "Clear Calib Parm");//// for testing
	}

	return 0;
}


int CInspectionCore::GetCalibDataForInsp(CCalibrationData* pCalibData)
{
	HTuple hResolutionInputY = m_CalibrationData.dResolutionAlongYInMicronPerPixel;
	TupleConcat(pCalibData->hResolutionYInput, hResolutionInputY, &pCalibData->hResolutionYInput);

	//int nCount = pCalibData->hResolutionYInput.Length();
	//OutputDebugLogTo(9, TRUE, "GetCalibDataForInsp Count %d", nCount);//// for testing

	return 0;
}

int CInspectionCore::Initialize(BOOL bParallel)
{
	m_bParallelism = bParallel;

	m_bKillInspectThreads = FALSE;
	m_bInitiateClearCache = FALSE;
	m_bClearThreadCache = FALSE;

	m_bKillInspectThreads = FALSE;

	for(int i=0; i<THREAD_COUNT; i++) {
		m_ThreadParms[i].m_hRun = CreateEventA(NULL, FALSE, FALSE, NULL); 
		m_ThreadParms[i].m_hDone = CreateEventA(NULL, FALSE, FALSE, NULL); 
		m_ThreadParms[i].bPass = FALSE;
	}
	
	m_nThreadIndex = 0;

	return 0;
}

int CInspectionCore::Close()
{
	m_bKillInspectThreads = TRUE;
	for(int i=0; i<THREAD_COUNT; i++)
		SetEvent(m_ThreadParms[i].m_hRun);

	return 0;
}

int CInspectionCore::InitializeInspection()
{
	//for(int index = 0; index < 5; index++)
	//{
	//	for(int nDoc = 0; nDoc < 5; nDoc++){
	//		m_arrayOverlayInspection[index][nDoc].RemoveAll();
	//	}
	//	//m_strInspectionResult[index].RemoveAll();
	//}
	for (int i = 0; i<2; i++)
		m_arrayOverlay[i].RemoveAll();
	m_arrayOverlayInspection.RemoveAll();

	for(int i=0; i<ERROR_CODES; i++)
		if(m_InspParms[i]){
			m_InspParms[i]->dInspTime  = 0;
			m_InspParms[i]->nError = 0;
		}

	
	for(int i=0; i<THREAD_COUNT; i++) {
		ResetEvent(m_ThreadParms[i].m_hDone);
		m_hDoneEvents[i] = NULL;
	}

	m_pShare = new SHARED_PARM();

	return 0;
}

void CInspectionCore::ReleaseMemory()
{
	int nPendingHandles = 0;
	
	for(int i=0; i<THREAD_COUNT; i++) {
		if(m_hDoneEvents[i])
			handlesDone[nPendingHandles++] = m_hDoneEvents[i];
	}

	if(nPendingHandles)
		WaitForMultipleObjects(nPendingHandles, handlesDone, TRUE, INFINITE);

	if(m_pShare) {
		delete m_pShare;
		m_pShare = NULL;
	}

	if(m_bInitiateClearCache)
		ClearCache();
	//HTuple hSystem;
	//GetSystem("temporary_mem_cache", &hSystem);
	//if(hSystem == "false")
	//	SetSystem("temporary_mem_cache", "true");
	//else
	//	SetSystem("temporary_mem_cache", "false");

	//SetSystem("global_mem_cache", "cleanup");
	//SetSystem("temporary_mem_cache", "true");

}

void CInspectionCore::ClearCache()
{
	CPerformanceTimer timer;
	SetSystem("temporary_mem_cache", "false");
	SetSystem("temporary_mem_cache", "true");

	m_bClearThreadCache = TRUE;
	
	for(int i=0; i<THREAD_COUNT; i++) {
		SetEvent(m_ThreadParms[i].m_hRun);
		handlesDone[i] = m_ThreadParms[i].m_hDone;
	}
	WaitForMultipleObjects(THREAD_COUNT, handlesDone, TRUE, INFINITE);
	m_bInitiateClearCache = FALSE;
	m_bClearThreadCache = FALSE;
	LogMessage(0, "Clear Time: %.1f", timer.GetElapsedMSec());

	//SetSystem("global_mem_cache", "cleanup");
	/**/
}



UINT InspectionThread(LPVOID pParam)
{
	try {
		CInspectionCore* pInsp = (CInspectionCore*) pParam;
		int nThreadIndex = pInsp->m_nThreadIndex++;
		THREAD_PARM* pThreadParm = (THREAD_PARM*) &pInsp->m_ThreadParms[nThreadIndex];
		SHARED_PARM *pShare = pInsp->m_pShare;
		CPerformanceTimer timer;

		while(1) {
			WaitForSingleObject(pThreadParm->m_hRun, INFINITE);
			if(pInsp->m_bKillInspectThreads)
				return 0;
			ResetEvent(pThreadParm->m_hDone);

			if(pInsp->m_bClearThreadCache) {
				SetSystem("temporary_mem_cache", "false");
				SetSystem("temporary_mem_cache", "true");
			}
			else {
				pInsp->Inspect(nThreadIndex, pThreadParm->bPass);
			}

			SetEvent(pThreadParm->m_hDone);
		}
	}
	catch(...) {
		OutputDebugLogTo(9,TRUE,"InspectionThread");
		return -1;
	}

	return 0;
}

int CInspectionCore::LightCoefficientInspect(HImage hLightImage, HTuple* hCoefficient, HTuple* hLightImageRows, HTuple* hLightImageCols)
{
	_FCI_Light_Coefficient_Inspect(hLightImage, hCoefficient, hLightImageRows, hLightImageCols);
	return 0;
}

int CInspectionCore::LightCorrection(HImage hImage, HTuple hCoefficient, HTuple hLightImageRows, HTuple hLightImageCols, HImage* hCorrectedImage)
{
	_FCI_Light_Correction(hImage, hCorrectedImage, hCoefficient, hLightImageRows, hLightImageCols);
	return 0;
}

int CInspectionCore::Inspect(HImage hImage,
							 HImage hTeachImage,
							 int lDebugFlag,
							 BOOL bRegionInsp,
							 int nCurTrack,
							 int nCurDoc,
							 int nTotalDoc,
							 int nCurFOV,
							 int nTotalFOV,
							 CTrainingData* pTrainingData,
							 CInspectinonData* pInspData,
							 CCalibrationData* pCalibData,
							BOOL bDirForTiltInsp,
							CDeepLearningInterface * pDeepLearningModule,
							CStringArray& strArrayInspValues)
{
	CPerformanceTimer timer;
	COLORREF colorRed = RGB(255, 0, 0);
	COLORREF colorGreen = RGB(0, 255, 0);
	COLORREF colorYellow = RGB(255, 255, 0);
	COLORREF colorBlue = RGB(0, 0, 255);
	COLORREF colorOrange = RGB(255, 165, 0);
	COLORREF colorCyan = RGB(0, 255, 255);
	HImage hDebugImage;
	HRegion hDebugRegion;
	HTuple hDebugMsg;

	if (!bRegionInsp)
		InitializeInspection();


	HRegion hDummyRegion;
	HTuple hDummyArea, hDummyRow, hDummyColumn;
	Threshold(hImage,&hDummyRegion, 1, 255);
	AreaCenter(hDummyRegion, &hDummyArea, &hDummyRow, &hDummyColumn);
	if (hDummyArea == 0)
	{
		HRegion hEmptyRegion;
		GenEmptyRegion(&hEmptyRegion);
		ConcatObj(pTrainingData->hObjectDeviceLocation, hEmptyRegion, &pTrainingData->hObjectDeviceLocation);
		TupleConcat(pTrainingData->hTupleProjectionParam, (HTuple(0).TupleConcat(0)).TupleConcat(0), &pTrainingData->hTupleProjectionParam);
		TupleConcat(pTrainingData->hTupleTeachDoc, 0, &pTrainingData->hTupleTeachDoc);
		TupleConcat(pTrainingData->hTupleDatumTeachTuple, 0, &pTrainingData->hTupleDatumTeachTuple);
		TupleConcat(pTrainingData->hTupleEdgeTeachTuple, 0, &pTrainingData->hTupleEdgeTeachTuple);
 	    return -NO_IMAGE;
	}

	

	timer.Reset();
	int nStepDebug = 0;
	if(lDebugFlag & DEBUG_STEP_MODE)
		nStepDebug = 1;

	CString strOutPutLog;

	switch (nCurTrack) {
	case 0: strOutPutLog.Format("Top%d", nCurDoc + 1);
		break;
	case 1: strOutPutLog.Format("Bottom%d", nCurDoc + 1);
		break;
	case 2: strOutPutLog.Format("Side%d", nCurDoc + 1);
		break;
	}

	if(!bRegionInsp) {

		HTuple hEnablePointer;			
		//hEnablePointer = m_TrainingData.hvec_TupleVectorCoverLayerPointer.GetAt(nIndexCLAP++);

		if ((m_TrainingData.bEnableCLAlignmentPointer == TRUE) && (m_TrainingData.bEnableManualPointer == FALSE))
		{
			HTuple  hNoOfPointers, hMaxRotationAngle, hPatternSearchAreaAlongX, hPatternSearchAreaAlongY, hMinAcceptanceScore, hEnableManualPointer;
			HTuple hModelID, hIsDatumFound, hDebugMessageOut, hv_MatchRow, hv_MatchColumn, hv_MatchAngle, hv_MatchScore;
			HTuple hPointerRows = HTuple();
			HTuple hPointerCols = HTuple();
			HImage hDebugImageOut;
			HRegion hDebugRegionOut, hPointerRegion;

			int nIndexCLAP = 1;
			hNoOfPointers = m_TrainingData.hvec_TupleVectorCoverLayerPointer.GetAt(nIndexCLAP++);
			hMaxRotationAngle = m_TrainingData.hvec_TupleVectorCoverLayerPointer.GetAt(nIndexCLAP++);
			hPatternSearchAreaAlongX = m_TrainingData.hvec_TupleVectorCoverLayerPointer.GetAt(nIndexCLAP++);
			hPatternSearchAreaAlongY = m_TrainingData.hvec_TupleVectorCoverLayerPointer.GetAt(nIndexCLAP++);
			hMinAcceptanceScore = m_TrainingData.hvec_TupleVectorCoverLayerPointer.GetAt(nIndexCLAP++);
			hEnableManualPointer = m_TrainingData.hvec_TupleVectorCoverLayerPointer.GetAt(nIndexCLAP++);
			pTrainingData->nPointer = hNoOfPointers;

			for (int nPointer = 0; nPointer < hNoOfPointers; nPointer++)
			{
				TupleConcat(hPointerRows, m_TrainingData.m_rectPointer[nPointer].top, &hPointerRows);
				TupleConcat(hPointerRows, m_TrainingData.m_rectPointer[nPointer].bottom, &hPointerRows);
				TupleConcat(hPointerCols, m_TrainingData.m_rectPointer[nPointer].left, &hPointerCols);
				TupleConcat(hPointerCols, m_TrainingData.m_rectPointer[nPointer].right, &hPointerCols);

				hModelID = m_TrainingData.hTuplePointerModelIDs[nPointer];
				//hModelID = m_TrainingData.hvec_TupleVectorCoverLayerPointer.GetAt(nIndexCLAP++);

				_FCI_Inspect_UniquePattern(hImage,
					&hPointerRegion, &hDebugImageOut, &hDebugRegionOut,
					nStepDebug, hPatternSearchAreaAlongX, hPatternSearchAreaAlongY, hMaxRotationAngle,
					hMinAcceptanceScore, hModelID, hPointerRows, hPointerCols,
					&hIsDatumFound, &hv_MatchScore, &hv_MatchAngle, &hv_MatchRow, &hv_MatchColumn, &hDebugMessageOut);

				CString str;
				double dMatchModelScore = (hv_MatchScore.D() * 100);
				str.Format("Position[%d] %s CVL Pointer[%d] Match Score [%3.2f %%]", nCurFOV, strOutPutLog, nPointer+1, dMatchModelScore);
				strArrayInspValues.Add(str);

				if (nStepDebug) {
					StepDebug(hDebugImageOut, hDebugRegionOut, colorGreen, hDebugMessageOut, bRegionInsp);
				}

				if (hIsDatumFound == 1)
					ConcatObj(pTrainingData->hObjectPointerLocation, hPointerRegion, &pTrainingData->hObjectPointerLocation);
				else
					return -COVER_LAYER_ALIGNMENT;

				hPointerRows = HTuple();
				hPointerCols = HTuple();

			}
		}

		HTuple hSobelAmp, hDeviceLocationEnable, hNoOfLocationRegions, hDeviceLocWidth, hDeviceLocHeight, hDeviceMaskSize, hReferencePointType;
		HTuple hMinDeviceContrast, hDatumSearchEnable, hNoOfUniqueRegions, hMaxRotationAngle;
		HTuple hPatternSearchAreaAlongX, hPatternSearchAreaAlongY, hMinAcceptanceScore;
		HTuple hModelID, hRefRotationAngle, hUniqueNoPatternsRows, hUniqueNoPatternsCols;
		HTuple hLocationTeachRows, hLocationTeachCols;
		HTuple hRefPointRow, hRefPointCol;
		HTuple hMatchModelCenterRow, hMatchModelCenterCol;

		HTuple hEdgeRefRotationAngle, hEdgeRefPointRow, hEdgeRefPointCol;
		HTuple hDeviceEdgeDetectionEnable, hDeviceHorizontalContrast, hDeviceHorizontalDirection, hDeviceVerticalContrast, hDeviceVerticalDirection;
		HTuple hDeviceEdgeLocationTeachRows, hDeviceEdgeLocationTeachCols;
		HTuple hDeviceEdgeDetectionSmoothingImage;

		int nIndex = 0;
		hDeviceLocationEnable = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hNoOfLocationRegions = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hDeviceLocWidth = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hDeviceLocHeight = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hDeviceMaskSize = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hMinDeviceContrast = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		//hSobelAmp = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hSobelAmp = m_TrainingData.bSobelAmp;
		hReferencePointType = m_TrainingData.nDeviceReferencePointType;

		hDatumSearchEnable = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hNoOfUniqueRegions = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hMaxRotationAngle = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hPatternSearchAreaAlongX = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hPatternSearchAreaAlongY = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hMinAcceptanceScore = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);

		hUniqueNoPatternsRows = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hUniqueNoPatternsCols = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hLocationTeachRows = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hLocationTeachCols = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);

		hModelID = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hRefRotationAngle = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);	
		hRefPointRow = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hRefPointCol = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);

		hEdgeRefRotationAngle = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hEdgeRefPointRow = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hEdgeRefPointCol = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);

		//For Device Edge Detection option
		hDeviceEdgeDetectionEnable = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hDeviceHorizontalContrast = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hDeviceHorizontalDirection = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hDeviceVerticalContrast = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hDeviceVerticalDirection = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);

		hDeviceEdgeLocationTeachRows = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
		hDeviceEdgeLocationTeachCols = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);

		hDeviceEdgeDetectionSmoothingImage = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);

		if (hDeviceLocationEnable == 0 && hDatumSearchEnable == 0)
		{
			HRegion hEmptyRegion;
			GenEmptyRegion(&hEmptyRegion);
			ConcatObj(pTrainingData->hObjectDeviceLocation, hEmptyRegion, &pTrainingData->hObjectDeviceLocation);
			TupleConcat(pTrainingData->hTupleProjectionParam, (HTuple(0).TupleConcat(0)).TupleConcat(0), &pTrainingData->hTupleProjectionParam);
			TupleConcat(pTrainingData->hTupleTeachDoc, 0, &pTrainingData->hTupleTeachDoc);
			TupleConcat(pTrainingData->hTupleDatumTeachTuple, 0, &pTrainingData->hTupleDatumTeachTuple);
			TupleConcat(pTrainingData->hTupleEdgeTeachTuple, 0, &pTrainingData->hTupleEdgeTeachTuple);
			return 0;
		}

		HRegion hDieLocation, hFindModelRegion;
		HTuple hIsPass, hIsFindDatam, hMatchModelScore, hAngleOfDeviation;

		GenEmptyRegion(&hDieLocation);
		hAngleOfDeviation = 0;//HTuple();

		HTuple hTopRefGotRotationAngle, hTopRefGotShiftAlongRow, hTopRefGotShiftAlongCol;
		

		_FCI_Inspect_DeviceLocation(hImage,
			&hDieLocation, &hFindModelRegion,
			&hDebugImage, &hDebugRegion, hDatumSearchEnable, hDeviceLocationEnable, nStepDebug,
			hSobelAmp, hNoOfLocationRegions,
			hDeviceLocWidth, hDeviceLocHeight, hMinDeviceContrast,
			hNoOfUniqueRegions, hMaxRotationAngle,
			hPatternSearchAreaAlongX, hPatternSearchAreaAlongY, hMinAcceptanceScore,
			hModelID, hRefRotationAngle,
			hRefPointRow, hRefPointCol,
			hEdgeRefRotationAngle, hEdgeRefPointRow, hEdgeRefPointCol,
			hUniqueNoPatternsRows, hUniqueNoPatternsCols,
			hLocationTeachRows, hLocationTeachCols, hDeviceMaskSize, nCurFOV, nTotalFOV, nCurTrack + 1, hReferencePointType,
			hDeviceEdgeDetectionEnable,
			hDeviceHorizontalContrast, hDeviceHorizontalDirection,
			hDeviceVerticalContrast, hDeviceVerticalDirection,
			hDeviceEdgeLocationTeachRows, hDeviceEdgeLocationTeachCols,
			hDeviceEdgeDetectionSmoothingImage,
			&hIsPass, &hIsFindDatam,
			&hAngleOfDeviation, &hMatchModelCenterRow, &hMatchModelCenterCol,
			&hMatchModelScore,
			&hTopRefGotRotationAngle, &hTopRefGotShiftAlongRow, &hTopRefGotShiftAlongCol,
			&hDebugMsg);

		CString str;
		double dMatchModelScore = (hMatchModelScore.D() * 100);
		str.Format("Position[%d] %s Datum Location Score [%3.2f %%]", nCurFOV, strOutPutLog, dMatchModelScore);
		strArrayInspValues.Add(str);

		ConcatObj(pTrainingData->hObjectDeviceLocation, hDieLocation, &pTrainingData->hObjectDeviceLocation);
		TupleConcat(pTrainingData->hTupleProjectionParam, (hAngleOfDeviation.TupleConcat(hMatchModelCenterRow)).TupleConcat(hMatchModelCenterCol), &pTrainingData->hTupleProjectionParam);
		TupleConcat(pTrainingData->hTupleTeachDoc, 1, &pTrainingData->hTupleTeachDoc);
		TupleConcat(pTrainingData->hTupleDatumTeachTuple, hDatumSearchEnable, &pTrainingData->hTupleDatumTeachTuple);
		if(hDatumSearchEnable == 1)
			TupleConcat(pTrainingData->hTupleEdgeTeachTuple, hIsFindDatam/*hDeviceLocationEnable*/, &pTrainingData->hTupleEdgeTeachTuple);
		else
			TupleConcat(pTrainingData->hTupleEdgeTeachTuple, hIsPass/*hDeviceLocationEnable*/, &pTrainingData->hTupleEdgeTeachTuple);

		if (hIsPass == TRUE && hDeviceLocationEnable == TRUE) {
			//m_arrayOverlayInspection.Add(hDieLocation, colorGreen);
			//Show the top reference point inspected
			HObject hCross, hCrossRegion, hDebugCrossRegion;
			HTuple hCrossRow, hCrossCol;
			GenCrossContourXld(&hCross, (hEdgeRefPointRow - hTopRefGotShiftAlongRow), (hEdgeRefPointCol - hTopRefGotShiftAlongCol), 200, 0);
			GetContourXld(hCross, &hCrossRow, &hCrossCol);
			GenRegionPolygon(&hCrossRegion, hCrossRow, hCrossCol);

			m_arrayOverlayInspection.Add(hCrossRegion, colorYellow, "Top Reference Point Found By Inspect Edge Device Location");
		}
			
		if (hIsFindDatam == TRUE)
			m_arrayOverlayInspection.Add(hFindModelRegion, colorGreen);

		if (nStepDebug) {
			StepDebug(hDebugImage,hDebugRegion, colorGreen, hDebugMsg, bRegionInsp);
		}
	}
	else if(bRegionInsp) 
{
//	if 
//#pragma region find location params	
//
//
//		//// Find location params
//		HRegion hDeviceLocationSelected;
//		HTuple hIsSelected, hInspectRotationAngle, hInspectShiftAlongRow, hInspectShiftAlongCol;
//		HTuple hCount, hArea, hDeviceCenterRow, hDeviceCenterCol;
//
//		_FCI_Extract_LocationParameterData(pTrainingData->hTupleProjectionParam, pTrainingData->hTupleTeachDoc,
//			pTrainingData->hTupleEdgeTeachTuple, pTrainingData->hTupleDatumTeachTuple, nCurDoc,
//			&hIsSelected, &hInspectRotationAngle, &hInspectShiftAlongRow, &hInspectShiftAlongCol);
//
//		if (0 != (hIsSelected == 0))
//		{
//			if(nCurTrack == 2)
//				return 0;// -DEVICE_LOCATION;
//			else
//				return -DEVICE_LOCATION;
//		}
//		
//		HTuple hConcatArea, hRow2, hColumn2, hMaxArea;
//		AreaCenter(pTrainingData->hObjectDeviceLocation, &hConcatArea, &hRow2, &hColumn2);
//		if (hConcatArea.TupleLength() > 0)
//			TupleMax(hConcatArea, &hMaxArea);
//		
//		if (0 != (hMaxArea == 0))
//		{
//			if (nCurTrack == 2)
//				return 0;// -DEVICE_LOCATION;
//			else
//				return -DEVICE_LOCATION;
//		}
//	
//		CountObj(pTrainingData->hObjectDeviceLocation, &hCount);
//		_FCI_Select_DeviceLocation(pTrainingData->hObjectDeviceLocation,
//			&hDeviceLocationSelected,
//			nCurDoc, &hDeviceCenterRow, &hDeviceCenterCol);
//
//		
//		CountObj(hDeviceLocationSelected, &hCount);
//		if (hCount < 1)
//			return 0;
//
//		//AreaCenter(hDeviceLocationSelected, &hArea, &hDeviceCenterRow, &hDeviceCenterCol);
//#pragma endregion find location params	
//		/*if(hArea == 0)
//			return -DEVICE_LOCATION;*/
//
//		//// Tilt Insp (FOV - first & last; Doc - top(3rd intensity) & side(1st intensity)) ////
//		if (m_TiltInspParm.bEnable && nCurTrack != 1 && (nCurFOV == 1 || nCurFOV == nTotalFOV)) {
//			HRegion hTiltRgn, hInspTiltRgnPortion;
//			HTuple hContrast, hDistanceForTiltInsp, hTiltInspImgIndex;
//			HTuple hTiltHeight;
//			HTuple hTiltRgnChar, hTiltRgnHeight, hMeanGreyValue, hTiltRectRow, hTiltRectCol;
//			HTuple hDebugMsgOutTilt;
//			HImage hDebugImageOutTilt;
//			HRegion hDebugRegionOutTilt;
//
//			hTiltRgnChar = hTiltRgnHeight = hMeanGreyValue = hTiltRectRow = hTiltRectCol = HTuple();
//
//			//hTiltInspImgIndex = m_TiltInspParm.nImageIndex - 1;
//			//if (nCurDoc == hTiltInspImgIndex) {
//				hContrast = m_TiltInspParm.nContrast;
//				hDistanceForTiltInsp = m_TiltInspParm.nDistanceForTiltInsp / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//
//				//hTiltRgnChar = m_TiltInspParm.nRegionChar;
//				//hTiltRgnHeight = m_TiltInspParm.nRegionHeight;
//				//hMeanGreyValue = m_TiltInspParm.nMeanGreyValue;
//
//				HTuple hRow1, hRow2, hCol1, hCol2, hXShift, hYShift;
//				hRow1 = m_TrainingData.m_rectTilt.top;
//				hRow2 = m_TrainingData.m_rectTilt.bottom;
//				hCol1 = m_TrainingData.m_rectTilt.left;
//				hCol2 = m_TrainingData.m_rectTilt.right;
//				/*hXShift = m_TrainingData.m_nXShift;
//				hYShift = m_TrainingData.m_nYShift;*/
//
//				TupleConcat(hTiltRectRow, hRow1.TupleConcat(hRow2), &hTiltRectRow);
//				TupleConcat(hTiltRectCol, hCol1.TupleConcat(hCol2), &hTiltRectCol);
//
//				_FCI_Finding_TiltInspectionParameter(hImage, hDeviceLocationSelected,
//													&hTiltRgn, &hInspTiltRgnPortion,&hDebugImageOutTilt,&hDebugRegionOutTilt,nStepDebug,
//													hTiltRectRow, hTiltRectCol, hInspectRotationAngle, hInspectShiftAlongRow, hInspectShiftAlongCol,
//													hContrast, hDistanceForTiltInsp,
//													nCurFOV, nTotalFOV, nCurTrack + 1,
//													&hTiltHeight,&hDebugMsgOutTilt);
//
//				TupleConcat(pInspData->hTiltHeight, hTiltHeight, &pInspData->hTiltHeight);
//
//				m_arrayOverlayInspection.Add(hTiltRgn, colorGreen);
//
//				if (nStepDebug) {
//					StepDebug(hDebugImageOutTilt, hDebugRegionOutTilt, colorCyan, hDebugMsgOutTilt, bRegionInsp);
//				}
//				int nCount = pInspData->hTiltHeight.Length();
//				OutputDebugLogTo(9, TRUE, "Tilt Insp:: Tilt Height Count %d", nCount);//// for testing
//			//}
//
//			int nCurrentFOV = bDirForTiltInsp ? nTotalFOV : 1;
//			if (nCurTrack == 2 && nCurDoc == 0 && nCurFOV == nCurrentFOV) {	//// Side Camera; First Intensity; Last/first FOV ////
//				HTuple hTiltTolerance, hAngleOfSideView, hPass, hValue;
//				HTuple hResolutionYTop, hResolutionYSide;
//				HTuple hTopHeight1, hTopHeight2;
//				HTuple hSideHeight1, hSideHeight2;
//
//				hTiltTolerance = m_TiltInspParm.nToleranceSide;
//				hAngleOfSideView = m_TiltInspParm.nAngleOfSideView;
//
//				if (pInspData->hTiltHeight[0].Length() > 0 || pInspData->hTiltHeight[2].Length() > 0)
//					hTopHeight1 = bDirForTiltInsp ? pInspData->hTiltHeight[0] : pInspData->hTiltHeight[2];
//				if (pInspData->hTiltHeight[1].Length() > 0 || pInspData->hTiltHeight[3].Length() > 0)
//					hSideHeight1 = bDirForTiltInsp ? pInspData->hTiltHeight[1] : pInspData->hTiltHeight[3];
//				if (pInspData->hTiltHeight[0].Length() > 0 || pInspData->hTiltHeight[2].Length() > 0)
//					hTopHeight2= bDirForTiltInsp ? pInspData->hTiltHeight[2] : pInspData->hTiltHeight[0];
//				if (pInspData->hTiltHeight[1].Length() > 0 || pInspData->hTiltHeight[3].Length() > 0)
//					hSideHeight2 = bDirForTiltInsp ? pInspData->hTiltHeight[3] : pInspData->hTiltHeight[1];
//
//				if (pCalibData->hResolutionYInput[0].Length() > 0) {
//					hResolutionYTop = pCalibData->hResolutionYInput[0];
//					double dValue = hResolutionYTop.D();
//					OutputDebugLogTo(8, TRUE, "%s Resolution Top Y [%.4f]", strOutPutLog, dValue);
//					//// added to display in Output Log - data tab ////
//					CString str;
//					str.Format("%s Top Y [%.4f]", strOutPutLog, dValue);
//					strArrayInspValues.Add(str);
//				}
//
//				if (pCalibData->hResolutionYInput[1].Length() > 0) {
//					hResolutionYSide = pCalibData->hResolutionYInput[1];
//					double dValue = hResolutionYSide.D();
//					OutputDebugLogTo(8, TRUE, "%s Resolution Side Y [%.4f]", strOutPutLog, dValue);
//					//// added to display in Output Log - data tab ////
//					CString str;
//					str.Format("%s Resolution Side Y [%.4f]", strOutPutLog, dValue);
//					strArrayInspValues.Add(str);
//				}
//
//				if (pInspData->hTiltHeight[0].Length() > 0 || pInspData->hTiltHeight[2].Length() > 0)
//				{
//					//hTopHeight1 = bDirForTiltInsp ? pInspData->hTiltHeight[0] : pInspData->hTiltHeight[2];
//					HTuple hValue = hTopHeight1*hResolutionYTop;
//					double dValue = hValue.D();
//					OutputDebugLogTo(8, TRUE, "Fov[%d] %s Top Height %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hTopHeight1.D(), dValue);
//					//// added to display in Output Log - data tab ////
//					CString str;
//					str.Format("Fov[%d] %s Top Height %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hTopHeight1.D(), dValue);
//					strArrayInspValues.Add(str);
//				}
//				if (pInspData->hTiltHeight[1].Length() > 0 || pInspData->hTiltHeight[3].Length() > 0)
//				{
//					//hSideHeight1 = bDirForTiltInsp ? pInspData->hTiltHeight[1] : pInspData->hTiltHeight[3];
//					HTuple hValue = hSideHeight1*hResolutionYSide;
//					double dValue = hValue.D();
//					OutputDebugLogTo(8, TRUE, "Fov[%d] %s Side Height %.4f pixel [%.4f um]", nCurFOV,strOutPutLog, hSideHeight1.D(), dValue);
//					//// added to display in Output Log - data tab ////
//					CString str;
//					str.Format("Fov[%d] %s Side Height %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hTopHeight1.D(), dValue);
//					strArrayInspValues.Add(str);
//				}
//				if (pInspData->hTiltHeight[0].Length() > 0 || pInspData->hTiltHeight[2].Length() > 0)
//				{
//					//hTopHeight2 = bDirForTiltInsp ? pInspData->hTiltHeight[2] : pInspData->hTiltHeight[0];
//					HTuple hValue = hTopHeight2*hResolutionYTop;
//					double dValue = hValue.D();
//					OutputDebugLogTo(8, TRUE, "Fov[%d] %s Top Height %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hTopHeight2.D(), dValue);
//					//// added to display in Output Log - data tab ////
//					CString str;
//					str.Format("Fov[%d] %s Top Height %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hTopHeight2.D(), dValue);
//					strArrayInspValues.Add(str);
//				}
//				if (pInspData->hTiltHeight[1].Length() > 0 || pInspData->hTiltHeight[3].Length() > 0)
//				{
//					//hSideHeight2 = bDirForTiltInsp ? pInspData->hTiltHeight[3] : pInspData->hTiltHeight[1];
//					HTuple hValue = hSideHeight2*hResolutionYSide;
//					double dValue = hValue.D();
//					OutputDebugLogTo(8, TRUE, "Fov[%d] %s Side Height %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hSideHeight2.D(), dValue);
//					//// added to display in Output Log - data tab ////
//					CString str;
//					str.Format("Fov[%d] %s Side Height %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hSideHeight2.D(), dValue);
//					strArrayInspValues.Add(str);
//				}
//
//				_FCI_Tilt_Inspection(hTopHeight1, hTopHeight2, hResolutionYTop,
//									hSideHeight1, hSideHeight2, hResolutionYSide,
//									hTiltTolerance, hAngleOfSideView,
//									&hPass, &hValue);
//				
//				ClearInspParam(pInspData, TRUE); // JY TESTING
//				
//				if (hValue.Length() > 0) {
//					double dValue = hValue.D();
//					OutputDebugLogTo(8, TRUE, "%s Tilt Inspection hValue [%.4f]", strOutPutLog,dValue);
//					//// added to display in Output Log - data tab ////
//					CString str;
//					str.Format("%s Tilt Inspection hValue [%.4f]", strOutPutLog,dValue);
//					strArrayInspValues.Add(str);
//				}
//
//				if (hPass == FALSE)
//					return -TILT_INSP;
//			}
//		}
//
//		//// Encap Insp ////
//		//HTuple hImgIndex = m_EncapParm.nImageIndex - 1;
//		HTuple hImgIndex = -1;
//		//if (m_TrainingData.hvec_TupleVectorEncap.GetSize() > 0)
//		//	if (m_TrainingData.hvec_TupleVectorEncap[14].Length() > 0)
//		//		hImgIndex = ((m_TrainingData.hvec_TupleVectorEncap[/*nIndexEncap*/14][0]) /*- 1*/);
//		if (m_EncapParm.bEnable) {
//
//			if(!m_TrainingData.bEnableEncap)
//				return -ENCAP_INSUFFICIENT;
//
//			int nEncapIndex = 0;
//			/*HTuple hMinEncapContrast = m_TrainingData.hvec_TupleVectorEncap.GetAt(nEncapIndex++);
//			HTuple hMaxEncapContrast = m_TrainingData.hvec_TupleVectorEncap.GetAt(nEncapIndex++);*/
//			HTuple hMinEncapContrast = m_TrainingData.nEncapMinContrast;
//			HTuple hMaxEncapContrast = m_TrainingData.nEncapMaxContrast;
//
//			HTuple hWireColor;
//			HImage hDebugImg;
//			HRegion hEncapLocation, hDebugRgn;
//			HTuple hIsEncap, IsPass, hIsPassForInsufficient, hIsPassForExcess, hDebugMsg;
//
//			hWireColor = nCurTrack;
//
//			HTuple hEncapRow1, hEncapCol1, hEncapRow2, hEncapCol2;
//			hEncapRow1 = m_TrainingData.m_rectEncap[0].top;
//			hEncapCol1 = m_TrainingData.m_rectEncap[0].left;
//			hEncapRow2 = m_TrainingData.m_rectEncap[0].bottom;
//			hEncapCol2 = m_TrainingData.m_rectEncap[0].right;
//
//			HRegion hEncapTeachRegion, hProjectedRegion;
//			HTuple hDebugMsgOutEncap;
//			HImage hDebugImageOutEncap;
//			HRegion hDebugRegionOutEncap;
//			HTuple hMessage;
//	
//			GenRectangle1(&hEncapTeachRegion, hEncapRow1, hEncapCol1, hEncapRow2, hEncapCol2);
//
//			//Encap Tracing Mask
//			HRegion hConcatEncapMaskRgn;
//			GenEmptyObj(&hConcatEncapMaskRgn);
//			/*for (int nMask = 0; nMask < m_TrainingData.nEncapTracingMaskNumber; nMask++) {
//				HRegion hEncapTracingMask;
//				hEncapRow1 = m_TrainingData.m_rectMaskEncapTracing[nMask].top;
//				hEncapCol1 = m_TrainingData.m_rectMaskEncapTracing[nMask].left;
//				hEncapRow2 = m_TrainingData.m_rectMaskEncapTracing[nMask].bottom;
//				hEncapCol2 = m_TrainingData.m_rectMaskEncapTracing[nMask].right;
//				GenRectangle1(&hEncapTracingMask, hEncapRow1, hEncapCol1, hEncapRow2, hEncapCol2);
//				ConcatObj(hConcatEncapMaskRgn, hEncapTracingMask, &hConcatEncapMaskRgn);
//			}*/
//			//HRegion hConcatEncapMaskRgn;
//			HTuple hEncapPolygonPoints;
//			GenEmptyObj(&hConcatEncapMaskRgn);
//			for (int nMask = 0; nMask < m_TrainingData.nEncapTracingMaskNumber; nMask++) {
//				HRegion hEncapTracingMask;
//				hEncapPolygonPoints = HTuple();
//				if (m_TrainingData.nEncapTracingMaskType[nMask] == RECTANGLE) {
//					hEncapRow1 = m_TrainingData.m_rectMaskEncapTracing[nMask].top;
//					hEncapCol1 = m_TrainingData.m_rectMaskEncapTracing[nMask].left;
//					hEncapRow2 = m_TrainingData.m_rectMaskEncapTracing[nMask].bottom;
//					hEncapCol2 = m_TrainingData.m_rectMaskEncapTracing[nMask].right;
//					GenRectangle1(&hEncapTracingMask, hEncapRow1, hEncapCol1, hEncapRow2, hEncapCol2);
//				}
//				else if (m_TrainingData.nEncapTracingMaskType[nMask] == ECLIPSE) {
//					GenEllipse(&hEncapTracingMask, m_TrainingData.m_rectMaskEncapTracing[nMask].CenterPoint().y, m_TrainingData.m_rectMaskEncapTracing[nMask].CenterPoint().x, 0,
//						m_TrainingData.m_rectMaskEncapTracing[nMask].Width() / 2, m_TrainingData.m_rectMaskEncapTracing[nMask].Height() / 2);
//				}
//				else if (m_TrainingData.nEncapTracingMaskType[nMask] == POLYGON) {
//					HTuple hPoint;
//					HTuple hRows, hCols;
//					for (int nPointId = 0; nPointId < m_TrainingData.nEncapTracingPolygonPointNumber[nMask]; nPointId++) {
//						TupleConcat(hRows, m_TrainingData.m_EncapTracingPolygonPointArea[nMask][nPointId].y, &hRows);
//						TupleConcat(hCols, m_TrainingData.m_EncapTracingPolygonPointArea[nMask][nPointId].x, &hCols);
//
//						hPoint.Clear();
//						hPoint.Append(m_TrainingData.m_EncapTracingPolygonPointArea[nMask][nPointId].y);
//						hPoint.Append(m_TrainingData.m_EncapTracingPolygonPointArea[nMask][nPointId].x);
//
//						//TupleConcat(hEncapPolygonPoints, hPoint, &hEncapPolygonPoints);
//					}
//					GenRegionPolygonFilled(&hEncapTracingMask, hRows, hCols);
//				}
//				ConcatObj(hConcatEncapMaskRgn, hEncapTracingMask, &hConcatEncapMaskRgn);
//			}
//
//			if (m_TrainingData.nEncapTracingMaskNumber > 0) {
//				HRegion hFitEncapTracingMaskRegion;
//				GenEmptyObj(&hFitEncapTracingMaskRegion);
//				Union1(hConcatEncapMaskRgn, &hFitEncapTracingMaskRegion);
//				Difference(hEncapTeachRegion, hFitEncapTracingMaskRegion, &hEncapTeachRegion);
//
//				//m_arrayOverlayTeach.Add(hFitEncapTracingMaskRegion, colorRed);
//			}
//
//			_FCI_Encap_RegionProjection(hImage,hEncapTeachRegion,
//				&hProjectedRegion,&hDebugImageOutEncap,&hDebugRegionOutEncap,nStepDebug,
//				hDeviceCenterRow, hDeviceCenterCol, hInspectRotationAngle, 
//				hInspectShiftAlongRow, hInspectShiftAlongCol,&hDebugMsgOutEncap);
//			
//			if (nStepDebug) {
//				StepDebug(hDebugImageOutEncap, hDebugRegionOutEncap, colorCyan, hDebugMsgOutEncap, bRegionInsp);
//			}
//			//m_arrayOverlayInspection.Add(hProjectedRegion, colorOrange);
//
//			_FCI_Inspect_EncapLocation(hImage, hDeviceLocationSelected, hProjectedRegion,
//										&hEncapLocation,
//										&hDebugImg, &hDebugRgn,
//										nStepDebug, hWireColor, hMinEncapContrast, hMaxEncapContrast,
//										&hIsEncap, &hDebugMsg);
//
//			if (nStepDebug) {
//				StepDebug(hDebugImg, hDebugRgn, colorCyan, hDebugMsg, bRegionInsp);
//			}
//			HTuple hErosionOffsetBG = m_EncapParm.dErosionOffset / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//			ErosionCircle(hEncapLocation, &hEncapLocation, hErosionOffsetBG);
//			ReduceDomain(hImage, hEncapLocation, &m_DefectData[nCurDoc].EncapProcessedBackGround);
//			HRegion hRegionBG;
//			Threshold(m_DefectData[nCurDoc].EncapProcessedBackGround, &hRegionBG, 2, 255);
//			HTuple hRow1, hColumn1, hRow2, hColumn2;
//			SmallestRectangle1(hRegionBG, &hRow1, &hColumn1, &hRow2, &hColumn2);
//			CropRectangle1(m_DefectData[nCurDoc].EncapProcessedBackGround, &m_DefectData[nCurDoc].EncapProcessedBackGround, hRow1-50, hColumn1-50, hRow2+50, hColumn2+50);
//			if (hIsEncap == TRUE) {
//				m_arrayOverlayInspection.Add(hEncapLocation, colorOrange);
//
//				//Deep Learning Mode: 0: Classification, 1: Recognition
//				if (m_EncapParm.bEnableEncapSurface)
//				{
//					HTuple hDebugMsgOutEncapDefect;
//					HImage hDebugImageOutEncapDefect;
//					HRegion hDebugRegionOutEncapDefect;
//					HRegion hEncapDefectRegion;
//					HTuple hErosionOffset, hMaxLayOnAllowedPercentage;
//					hErosionOffset = m_EncapParm.dErosionOffset / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//					hMaxLayOnAllowedPercentage = m_EncapParm.dMaxLayOnAllowedPercentage;
//
//					//Encap Surface Mask
//					HRegion hConcatEncapSurfaceMaskRgn;
//					GenEmptyObj(&hConcatEncapSurfaceMaskRgn);
//					for (int nMask = 0; nMask < m_TrainingData.nEncapSurfaceMaskNumber; nMask++) {
//						HRegion hEncapSurfaceMask;
//						hEncapRow1 = m_TrainingData.m_rectMaskEncapSurface[nMask].top;
//						hEncapCol1 = m_TrainingData.m_rectMaskEncapSurface[nMask].left;
//						hEncapRow2 = m_TrainingData.m_rectMaskEncapSurface[nMask].bottom;
//						hEncapCol2 = m_TrainingData.m_rectMaskEncapSurface[nMask].right;
//						GenRectangle1(&hEncapSurfaceMask, hEncapRow1, hEncapCol1, hEncapRow2, hEncapCol2);
//						ConcatObj(hConcatEncapSurfaceMaskRgn, hEncapSurfaceMask, &hConcatEncapSurfaceMaskRgn);
//					}
//
//					if (m_TrainingData.nEncapSurfaceMaskNumber > 0) {
//						HRegion hFitEncapSurfaceMaskRegion;
//						GenEmptyObj(&hFitEncapSurfaceMaskRegion);
//						Union1(hConcatEncapSurfaceMaskRgn, &hFitEncapSurfaceMaskRegion);
//
//						_FCI_Encap_RegionProjection(hImage, hFitEncapSurfaceMaskRegion,
//							&hFitEncapSurfaceMaskRegion, &hDebugImageOutEncap, &hDebugRegionOutEncap, 0,
//							hDeviceCenterRow, hDeviceCenterCol, hInspectRotationAngle,
//							hInspectShiftAlongRow, hInspectShiftAlongCol, &hDebugMsgOutEncap);
//						Difference(hEncapLocation, hFitEncapSurfaceMaskRegion, &hEncapLocation);
//						m_arrayOverlayInspection.Add(hFitEncapSurfaceMaskRegion, colorYellow);
//					}
//
//
//					if ( pTrainingData->bEnableDLModelInspect &&  pDeepLearningModule->m_nDeepLearningMode == 1) {
//
//						HTuple hDebugMsgOutEncapRP;
//						HImage hDebugImageOutEncapRP;
//						HRegion hEncapInspLocation;
//						HRegion hDebugRegionOutEncapRP;
//						HRegion hRect, hProjectedEncapRegion;
//						HTuple hRectRow, hRectCol;
//						GenEmptyObj(&hEncapInspLocation);
//						for (int i = 1; i < 3; i++) {
//							TupleConcat(hRectRow, m_TrainingData.m_rectEncap[i].top, &hRectRow);
//							TupleConcat(hRectRow, m_TrainingData.m_rectEncap[i].bottom, &hRectRow);
//							TupleConcat(hRectCol, m_TrainingData.m_rectEncap[i].left, &hRectCol);
//							TupleConcat(hRectCol, m_TrainingData.m_rectEncap[i].right, &hRectCol);
//							GenRectangle1(&hRect, m_TrainingData.m_rectEncap[i].top, m_TrainingData.m_rectEncap[i].left,
//								m_TrainingData.m_rectEncap[i].bottom, m_TrainingData.m_rectEncap[i].right);
//							ConcatObj(hEncapInspLocation, hRect, &hEncapInspLocation);
//						}
//
//						_FCI_Encap_InnOuter_RgnProjection(hImage, hEncapInspLocation,
//							&hProjectedEncapRegion, &hDebugImageOutEncapRP, &hDebugRegionOutEncapRP, nStepDebug,
//							hDeviceCenterRow, hDeviceCenterCol, hInspectRotationAngle, hInspectShiftAlongRow,
//							hInspectShiftAlongCol, &hDebugMsgOutEncapRP);
//
//
//						m_arrayOverlayInspection.Add(hProjectedEncapRegion, colorCyan);
//
//						if (nStepDebug) {
//							StepDebug(hDebugImageOutEncapRP, hDebugRegionOutEncapRP, colorCyan, hDebugMsgOutEncapRP, bRegionInsp);
//						}
//
//						_FCI_Inspect_EncapDefects(hImage, hEncapLocation, hProjectedEncapRegion,
//							&IsPass, &hIsPassForInsufficient, &hIsPassForExcess);
//
//
//						if (IsPass == FALSE) {
//							HTuple hCenterX, hCenterY, hTop, hLeft, hBottom, hRight, hDefectInfo;
//							Connection(hEncapLocation, &hEncapLocation);
//							RegionFeatures(hEncapLocation, (((HTuple("column").Append("row")).Append("column1")).Append("row1").Append("column2").Append("row2"))
//								, &hDefectInfo);
//
//							CleanDefectData(nCurDoc);
//							for (int nDefectIdx = 0; nDefectIdx < hDefectInfo.Length() / 6; nDefectIdx++) {
//								m_DefectData[nCurDoc].arrayDefectCenters.push_back(CPoint(hDefectInfo[6 * nDefectIdx].D(), hDefectInfo[6 * nDefectIdx + 1].D()));
//								m_DefectData[nCurDoc].arrayDefectRects.push_back(CRect(hDefectInfo[6 * nDefectIdx + 2].D(), hDefectInfo[6 * nDefectIdx + 3].D(),
//									hDefectInfo[6 * nDefectIdx + 4].D(), hDefectInfo[6 * nDefectIdx + 5].D()));
//							}
//
//							if (pTrainingData->bCentralizedVerificationMode) {
//								//Get region points
//								HObject hRegionBorder, hRegionUnion;
//								Union1(hEncapLocation, &hRegionUnion);
//								Boundary(hRegionUnion, &hRegionBorder, "outer");
//								GetRegionContour(hRegionBorder, &hCenterY, &hCenterX);
//								
//								if (hCenterX.Length() > 0) {
//									m_DefectData[nCurDoc].arrayDefectX.resize(1);
//									m_DefectData[nCurDoc].arrayDefectY.resize(1);
//									for (int nPoint = 0; nPoint < hCenterX.Length(); nPoint++) {
//										m_DefectData[nCurDoc].arrayDefectX[0].push_back(hCenterX[nPoint].I());
//										m_DefectData[nCurDoc].arrayDefectY[0].push_back(hCenterY[nPoint].I());
//									}
//								}
//
//								//Inner and outer Encap Rectangle
//								Connection(hProjectedEncapRegion, &hProjectedEncapRegion);
//								RegionFeatures(hProjectedEncapRegion, (((HTuple("column").Append("row")).Append("column1")).Append("row1").Append("column2").Append("row2"))
//									, &hDefectInfo);
//								for (int nDefectIdx = 0; nDefectIdx < hDefectInfo.Length() / 6; nDefectIdx++) {
//									m_DefectData[nCurDoc].arrayDefectCenters.push_back(CPoint(hDefectInfo[6 * nDefectIdx].D(), hDefectInfo[6 * nDefectIdx + 1].D()));
//									m_DefectData[nCurDoc].arrayDefectRects.push_back(CRect(hDefectInfo[6 * nDefectIdx + 2].D(), hDefectInfo[6 * nDefectIdx + 3].D(),
//										hDefectInfo[6 * nDefectIdx + 4].D(), hDefectInfo[6 * nDefectIdx + 5].D()));
//								}
//
//								HTuple hDefectCount;
//								CountObj(hProjectedEncapRegion, &hDefectCount);
//								int nEncapLocCount = hDefectCount.I();
//								if (nEncapLocCount > 0) {
//									m_DefectData[nCurDoc].arrayDefectX.resize(nEncapLocCount + 1);
//									m_DefectData[nCurDoc].arrayDefectY.resize(nEncapLocCount + 1);
//									for (int nIndex = 1; nIndex < m_DefectData[nCurDoc].arrayDefectX.size(); nIndex++) {
//										m_DefectData[nCurDoc].arrayDefectX[nIndex].push_back(-1);
//										m_DefectData[nCurDoc].arrayDefectY[nIndex].push_back(-1);
//									}
//								}
//							}
//							
//
//							if (hIsPassForInsufficient == FALSE)
//								return -ENCAP_INSUFFICIENT;
//							else if (hIsPassForExcess == FALSE)
//								return -ENCAP_EXCESS;
//						}
//
//						HTuple hEncapLocationRow1, hEncapLocationCol1, hEncapLocationRow2, hEncapLocationCol2;
//						SmallestRectangle1(hEncapLocation, &hEncapLocationRow1, &hEncapLocationCol1, &hEncapLocationRow2, &hEncapLocationCol2);
//						CleanDefectData(nCurDoc);
//
//						m_DefectData[nCurDoc].arrayDefectCenters.push_back(CPoint((hEncapLocationCol1.D() + hEncapLocationCol2.D()) / 2,
//																					(hEncapLocationRow1.D() + hEncapLocationRow2.D()) / 2));
//						
//						m_DefectData[nCurDoc].arrayDefectRects.push_back(CRect(hEncapLocationCol1.D(), hEncapLocationRow1.D(),
//																		hEncapLocationCol2.D(), hEncapLocationRow2.D()));
//						return -ENCAP_CONTAMINATION;//Return Fake Error Code
//					}
//
//					//----- Starting to inspect Encap Surface
//
//					HImage hEncapInspectedImage;
//					_FCI_Encap_Extraction(hImage, hEncapLocation, &hEncapInspectedImage, &hDebugImageOutEncapDefect, &hDebugRegionOutEncapDefect, nStepDebug, \
//											hErosionOffset, &hDebugMsgOutEncapDefect);
//					if (nStepDebug) {
//						StepDebug(hDebugImageOutEncapDefect, hDebugRegionOutEncapDefect, colorCyan, hDebugMsgOutEncapDefect, bRegionInsp);
//					}
//
//					// -- Encap Crack
//					HTuple  hDebugMsgOutEncapCrack;
//					HImage hDebugImageOutEncapCrack;
//					HRegion hDebugRegionOutEncapCrack;
//					HRegion hCrackDefectRegion;
//					HTuple hMinContrastCrack, hMinSizeCrack, hMinLengthCrack, hMinSquareSizeCrack, hIsPass;
//					HTuple  hAllECrackDefectMinSize, hAllECrackDefectMinLength, hAllECrackDefectMinSqSize;
//
//					hMinContrastCrack = m_EncapParm.nMinContrastCrack;
//					hMinSizeCrack = m_EncapParm.nMinSizeCrack / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//					hMinLengthCrack = m_EncapParm.nMinLengthCrack / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//					hMinSquareSizeCrack = m_EncapParm.nMinSquareSizeCrack / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//					_FCI_Inspect_Encap_Crack(hEncapInspectedImage,
//						&hCrackDefectRegion, &hDebugImageOutEncapCrack, &hDebugRegionOutEncapCrack, nStepDebug,
//						hMinContrastCrack, hMinSizeCrack, hMinLengthCrack, hMinSquareSizeCrack,
//						&hIsPass, &hDebugMsgOutEncapCrack, &hAllECrackDefectMinSize, &hAllECrackDefectMinLength, &hAllECrackDefectMinSqSize);
//
//
//					if (nStepDebug) {
//						StepDebug(hDebugImageOutEncapCrack, hDebugRegionOutEncapCrack, colorCyan, hDebugMsgOutEncapCrack, bRegionInsp);
//					}
//					if (hIsPass == FALSE){
//						_FCI_Encap_Border_LayOn_Recheck(hEncapInspectedImage, hEncapLocation, hCrackDefectRegion, &hCrackDefectRegion, &hDebugImageOutEncapCrack, &hDebugRegionOutEncapCrack,
//							nStepDebug, hErosionOffset, hMaxLayOnAllowedPercentage, &hDebugMsgOutEncapCrack, &hIsPass);
//
//						if (nStepDebug) {
//							StepDebug(hDebugImageOutEncapCrack, hDebugRegionOutEncapCrack, colorCyan, hDebugMsgOutEncapCrack, bRegionInsp);
//						}
//
//						if (hIsPass == FALSE) {
//							m_arrayOverlayInspection.Add(hCrackDefectRegion, colorRed);
//							CString str;
//							HTuple hResolution = ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//							HTuple hValue = hAllECrackDefectMinSize * hResolution;
//							double dValue = hValue.D();
//							str.Format("Fov[%d] %s EnCap Crack Min Size %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hAllECrackDefectMinSize.D(), dValue);
//							strArrayInspValues.Add(str);
//
//							hValue = hAllECrackDefectMinLength * hResolution;
//							dValue = hValue.D();
//							str.Format("Fov[%d] %s EnCap Crack Min Length %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hAllECrackDefectMinLength.D(), dValue);
//							strArrayInspValues.Add(str);
//
//							hValue = hAllECrackDefectMinSqSize * hResolution;
//							dValue = hValue.D();
//							str.Format("Fov[%d] %s EnCap Crack Min Square Size %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hAllECrackDefectMinSqSize.D(), dValue);
//							strArrayInspValues.Add(str);
//
//							//Store the defect information for Deep Learning Saving
//							HTuple hCenterX, hCenterY, hTop, hLeft, hBottom, hRight, hDefectInfo;
//							Connection(hCrackDefectRegion, &hCrackDefectRegion);
//							RegionFeatures(hCrackDefectRegion, (((HTuple("column").Append("row")).Append("column1")).Append("row1").Append("column2").Append("row2"))
//								, &hDefectInfo);
//
//							CleanDefectData(nCurDoc);
//
//							for (int nDefectIdx = 0; nDefectIdx < hDefectInfo.Length() / 6; nDefectIdx++) {
//								m_DefectData[nCurDoc].arrayDefectCenters.push_back(CPoint(hDefectInfo[6 * nDefectIdx].D(), hDefectInfo[6 * nDefectIdx + 1].D()));
//								m_DefectData[nCurDoc].arrayDefectRects.push_back(CRect(hDefectInfo[6 * nDefectIdx + 2].D(), hDefectInfo[6 * nDefectIdx + 3].D(),
//									hDefectInfo[6 * nDefectIdx + 4].D(), hDefectInfo[6 * nDefectIdx + 5].D()));
//							}
//
//
//							if (pTrainingData->bCentralizedVerificationMode) {
//								//Get region points
//								HObject hRegionBorder;
//								HTuple hDefectCount;
//								CountObj(hCrackDefectRegion, &hDefectCount);
//								int nDefectCount = hDefectCount.I();
//								m_DefectData[nCurDoc].arrayDefectX.resize(nDefectCount);
//								m_DefectData[nCurDoc].arrayDefectY.resize(nDefectCount);
//								for (int nDefectIdx = 0; nDefectIdx < nDefectCount; nDefectIdx++) {
//									HObject hCurrentDefect;
//									SelectObj(hCrackDefectRegion, &hCurrentDefect, HTuple(nDefectIdx + 1));
//									Boundary(hCurrentDefect, &hRegionBorder, "outer");
//									GetRegionContour(hRegionBorder, &hCenterY, &hCenterX);
//
//									if (hCenterX.Length() > 0) {
//										for (int nPoint = 0; nPoint < hCenterX.Length(); nPoint++) {
//											m_DefectData[nCurDoc].arrayDefectX[nDefectIdx].push_back(hCenterX[nPoint].I());
//											m_DefectData[nCurDoc].arrayDefectY[nDefectIdx].push_back(hCenterY[nPoint].I());
//										}
//									}
//								}
//							}
//
//							return -ENCAP_CRACK;
//						}
//					}
//
//					// -- Encap Blow Hole
//					HTuple hDebugMsgOutEncapBH;
//					HImage hDebugImageOutEncapBH;
//					HRegion hDebugRegionOutEncapBH;
//					HRegion hBHDefectRegion;
//					HTuple hMinCircularityBH, hMinLengthBH, hMinSquareSizeBH, hMinGrayMeanBH, hMinContrastBH, hMinSizeBH;
//					HTuple hAllEBHDefectMinSize, hAllEBHDefectMinLength, hAllEBHDefectMinSqSize;
//
//					hMinContrastBH = m_EncapParm.nMinContrastBH;
//					hMinSizeBH = m_EncapParm.nMinSizeBH / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//					hMinCircularityBH = m_EncapParm.dMinCircularityBH;
//					hMinLengthBH = m_EncapParm.nMinLengthBH / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//					hMinSquareSizeBH = m_EncapParm.nMinSquareSizeBH / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//					hMinGrayMeanBH = m_EncapParm.nMinGrayMeanBH;
//					_FCI_Inspect_BlowHole(hEncapInspectedImage,
//						&hBHDefectRegion, &hDebugImageOutEncapBH, &hDebugRegionOutEncapBH, nStepDebug,
//						hMinContrastBH, hMinSizeBH, hMinCircularityBH, hMinLengthBH, hMinSquareSizeBH, hMinGrayMeanBH,
//						&hIsPass, &hDebugMsgOutEncapBH, &hAllEBHDefectMinSize, &hAllEBHDefectMinLength, &hAllEBHDefectMinSqSize);
//
//
//					if (nStepDebug) {
//						StepDebug(hDebugImageOutEncapBH, hDebugRegionOutEncapBH, colorCyan, hDebugMsgOutEncapBH, bRegionInsp);
//					}
//
//					if (hIsPass == FALSE){
//						_FCI_Encap_Border_LayOn_Recheck(hEncapInspectedImage, hEncapLocation, hBHDefectRegion, &hBHDefectRegion, &hDebugImageOutEncapBH, &hDebugRegionOutEncapBH,
//							nStepDebug, hErosionOffset, hMaxLayOnAllowedPercentage, &hDebugMsgOutEncapBH, &hIsPass);
//
//						if (nStepDebug) {
//							StepDebug(hDebugImageOutEncapBH, hDebugRegionOutEncapBH, colorCyan, hDebugMsgOutEncapBH, bRegionInsp);
//						}
//
//						if (hIsPass == FALSE) {
//							m_arrayOverlayInspection.Add(hBHDefectRegion, colorRed);
//							CString str;
//							HTuple hResolution = ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//							HTuple hValue = hAllEBHDefectMinSize * hResolution;
//							double dValue = hValue.D();
//							str.Format("Fov[%d] %s EnCap BlowHole Min Size %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hAllEBHDefectMinSize.D(), dValue);
//							strArrayInspValues.Add(str);
//
//							hValue = hAllEBHDefectMinLength * hResolution;
//							dValue = hValue.D();
//							str.Format("Fov[%d] %s EnCap BlowHole Min Length %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hAllEBHDefectMinLength.D(), dValue);
//							strArrayInspValues.Add(str);
//
//							hValue = hAllEBHDefectMinSqSize * hResolution;
//							dValue = hValue.D();
//							str.Format("Fov[%d] %s EnCap BlowHole Min Square Size %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hAllEBHDefectMinSqSize.D(), dValue);
//							strArrayInspValues.Add(str);
//
//							//Store the defect information for Deep Learning Saving
//							HTuple hCenterX, hCenterY, hTop, hLeft, hBottom, hRight, hDefectInfo;
//							Connection(hBHDefectRegion, &hBHDefectRegion);
//							RegionFeatures(hBHDefectRegion, (((HTuple("column").Append("row")).Append("column1")).Append("row1").Append("column2").Append("row2"))
//								, &hDefectInfo);
//
//							CleanDefectData(nCurDoc);
//
//							for (int nDefectIdx = 0; nDefectIdx < hDefectInfo.Length() / 6; nDefectIdx++) {
//								m_DefectData[nCurDoc].arrayDefectCenters.push_back(CPoint(hDefectInfo[6 * nDefectIdx].D(), hDefectInfo[6 * nDefectIdx + 1].D()));
//								m_DefectData[nCurDoc].arrayDefectRects.push_back(CRect(hDefectInfo[6 * nDefectIdx + 2].D(), hDefectInfo[6 * nDefectIdx + 3].D(),
//									hDefectInfo[6 * nDefectIdx + 4].D(), hDefectInfo[6 * nDefectIdx + 5].D()));
//							}
//
//							if (pTrainingData->bCentralizedVerificationMode) {
//								//Get region points
//								HObject hRegionBorder;
//								HTuple hDefectCount;
//								CountObj(hBHDefectRegion, &hDefectCount);
//								int nDefectCount = hDefectCount.I();
//								m_DefectData[nCurDoc].arrayDefectX.resize(nDefectCount);
//								m_DefectData[nCurDoc].arrayDefectY.resize(nDefectCount);
//								for (int nDefectIdx = 0; nDefectIdx < nDefectCount; nDefectIdx++) {
//									HObject hCurrentDefect;
//									SelectObj(hBHDefectRegion, &hCurrentDefect, HTuple(nDefectIdx + 1));
//									Boundary(hCurrentDefect, &hRegionBorder, "outer");
//									GetRegionContour(hRegionBorder, &hCenterY, &hCenterX);
//
//									if (hCenterX.Length() > 0) {
//										for (int nPoint = 0; nPoint < hCenterX.Length(); nPoint++) {
//											m_DefectData[nCurDoc].arrayDefectX[nDefectIdx].push_back(hCenterX[nPoint].I());
//											m_DefectData[nCurDoc].arrayDefectY[nDefectIdx].push_back(hCenterY[nPoint].I());
//										}
//									}
//								}
//							}
//
//							return -ENCAP_BLOWHOLE;
//						}
//						
//					}
//
//					HTuple hDebugMsgOutEncapCTM;
//					HImage hDebugImageOutEncapCTM;
//					HRegion hDebugRegionOutEncapCTM;
//					HRegion hCTDefectRegion;
//					HTuple hMaxCircularityCT, hMinLengthCT, hMinSquareSizeCT, hMinContrastCT, hMinSizeCT;
//					HTuple hAllECTDefectMinSize, hAllECTDefectMinLength, hAllECTDefectMinSqSize;
//
//					hMinContrastCT = m_EncapParm.nMinContrastCT;
//					hMinSizeCT = m_EncapParm.nMinSizeCT / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//					hMaxCircularityCT = m_EncapParm.dMaxCircularityCT;
//					hMinLengthCT = m_EncapParm.nMinLengthCT / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//					hMinSquareSizeCT = m_EncapParm.nMinSquareSizeCT / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//
//					_FCI_Inspect_Contamination(hEncapInspectedImage,
//						&hCTDefectRegion, &hDebugImageOutEncapCTM, &hDebugRegionOutEncapCTM, nStepDebug,
//						hMinContrastCT, hMinSizeCT, hMinLengthCT, hMinSquareSizeCT, hMaxCircularityCT,
//						&hIsPass, &hDebugMsgOutEncapCTM, &hAllECTDefectMinSize, &hAllECTDefectMinLength, &hAllECTDefectMinSqSize);
//
//					if (nStepDebug) {
//						StepDebug(hDebugImageOutEncapCTM, hDebugRegionOutEncapCTM, colorCyan, hDebugMsgOutEncapCTM, bRegionInsp);
//					}
//
//					if (hIsPass == FALSE){
//						_FCI_Encap_Border_LayOn_Recheck(hEncapInspectedImage, hEncapLocation, hCTDefectRegion, &hCTDefectRegion, &hDebugImageOutEncapCTM, &hDebugRegionOutEncapCTM,
//							nStepDebug, hErosionOffset, hMaxLayOnAllowedPercentage, &hDebugMsgOutEncapCTM, &hIsPass);
//
//						if (nStepDebug) {
//							StepDebug(hDebugImageOutEncapCTM, hDebugRegionOutEncapCTM, colorCyan, hDebugMsgOutEncapCTM, bRegionInsp);
//						}
//
//						if (hIsPass == FALSE) {
//							m_arrayOverlayInspection.Add(hCTDefectRegion, colorRed);
//							CString str;
//							HTuple hResolution = ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//							HTuple hValue = hAllECTDefectMinSize * hResolution;
//							double dValue = hValue.D();
//							str.Format("Fov[%d] %s EnCap Contamination Min Size %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hAllECTDefectMinSize.D(), dValue);
//							strArrayInspValues.Add(str);
//
//							hValue = hAllECTDefectMinLength * hResolution;
//							dValue = hValue.D();
//							str.Format("Fov[%d] %s EnCap Contamination Min Length %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hAllECTDefectMinLength.D(), dValue);
//							strArrayInspValues.Add(str);
//
//							hValue = hAllECTDefectMinSqSize * hResolution;
//							dValue = hValue.D();
//							str.Format("Fov[%d] %s EnCap Contamination Min Square Size %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hAllECTDefectMinSqSize.D(), dValue);
//							strArrayInspValues.Add(str);
//
//							HTuple hCenterX, hCenterY, hTop, hLeft, hBottom, hRight, hDefectInfo;
//							Connection(hCTDefectRegion, &hCTDefectRegion);
//							RegionFeatures(hCTDefectRegion, (((HTuple("column").Append("row")).Append("column1")).Append("row1").Append("column2").Append("row2"))
//								, &hDefectInfo);
//
//							CleanDefectData(nCurDoc);
//
//							for (int nDefectIdx = 0; nDefectIdx < hDefectInfo.Length() / 6; nDefectIdx++) {
//								m_DefectData[nCurDoc].arrayDefectCenters.push_back(CPoint(hDefectInfo[6 * nDefectIdx].D(), hDefectInfo[6 * nDefectIdx + 1].D()));
//								m_DefectData[nCurDoc].arrayDefectRects.push_back(CRect(hDefectInfo[6 * nDefectIdx + 2].D(), hDefectInfo[6 * nDefectIdx + 3].D(),
//									hDefectInfo[6 * nDefectIdx + 4].D(), hDefectInfo[6 * nDefectIdx + 5].D()));
//							}
//
//							if (pTrainingData->bCentralizedVerificationMode) {
//								//Get region points
//								HObject hRegionBorder;
//								HTuple hDefectCount;
//								CountObj(hCTDefectRegion, &hDefectCount);
//								int nDefectCount = hDefectCount.I();
//								m_DefectData[nCurDoc].arrayDefectX.resize(nDefectCount);
//								m_DefectData[nCurDoc].arrayDefectY.resize(nDefectCount);
//								for (int nDefectIdx = 0; nDefectIdx < nDefectCount; nDefectIdx++) {
//									HObject hCurrentDefect;
//									SelectObj(hCTDefectRegion, &hCurrentDefect, HTuple(nDefectIdx + 1));
//									Boundary(hCurrentDefect, &hRegionBorder, "outer");
//									GetRegionContour(hRegionBorder, &hCenterY, &hCenterX);
//
//									if (hCenterX.Length() > 0) {
//										for (int nPoint = 0; nPoint < hCenterX.Length(); nPoint++) {
//											m_DefectData[nCurDoc].arrayDefectX[nDefectIdx].push_back(hCenterX[nPoint].I());
//											m_DefectData[nCurDoc].arrayDefectY[nDefectIdx].push_back(hCenterY[nPoint].I());
//										}
//									}
//								}
//							}
//
//							return -ENCAP_CONTAMINATION;
//						}
//					}
//				}
//
//				HTuple hDebugMsgOutEncapRP;
//				HImage hDebugImageOutEncapRP;
//				HRegion hEncapInspLocation;
//				HRegion hDebugRegionOutEncapRP;
//				HRegion hRect, hProjectedEncapRegion;
//				HTuple hRectRow, hRectCol;
//				GenEmptyObj(&hEncapInspLocation);
//				for (int i = 1; i<3; i++) {
//					TupleConcat(hRectRow, m_TrainingData.m_rectEncap[i].top, &hRectRow);
//					TupleConcat(hRectRow, m_TrainingData.m_rectEncap[i].bottom, &hRectRow);
//					TupleConcat(hRectCol, m_TrainingData.m_rectEncap[i].left, &hRectCol);
//					TupleConcat(hRectCol, m_TrainingData.m_rectEncap[i].right, &hRectCol);
//					GenRectangle1(&hRect, m_TrainingData.m_rectEncap[i].top, m_TrainingData.m_rectEncap[i].left,
//						m_TrainingData.m_rectEncap[i].bottom, m_TrainingData.m_rectEncap[i].right);
//					ConcatObj(hEncapInspLocation, hRect, &hEncapInspLocation);
//				}	
//	
//				_FCI_Encap_InnOuter_RgnProjection(hImage,hEncapInspLocation,
//					&hProjectedEncapRegion,&hDebugImageOutEncapRP,&hDebugRegionOutEncapRP,nStepDebug,
//					hDeviceCenterRow, hDeviceCenterCol, hInspectRotationAngle, hInspectShiftAlongRow, 
//					hInspectShiftAlongCol,&hDebugMsgOutEncapRP);
//				
//				
//				m_arrayOverlayInspection.Add(hProjectedEncapRegion, colorCyan);
//
//				if (nStepDebug) {
//					StepDebug(hDebugImageOutEncapRP, hDebugRegionOutEncapRP, colorCyan, hDebugMsgOutEncapRP, bRegionInsp);
//				}
//
//				_FCI_Inspect_EncapDefects(hImage, hEncapLocation, hProjectedEncapRegion,
//										&IsPass, &hIsPassForInsufficient, &hIsPassForExcess);
//
//				
//				if (IsPass == FALSE) {
//					HTuple hCenterX, hCenterY, hTop, hLeft, hBottom, hRight, hDefectInfo;
//					Connection(hEncapLocation, &hEncapLocation);
//					RegionFeatures(hEncapLocation, (((HTuple("column").Append("row")).Append("column1")).Append("row1").Append("column2").Append("row2"))
//						, &hDefectInfo);
//
//					CleanDefectData(nCurDoc);
//
//					for (int nDefectIdx = 0; nDefectIdx < hDefectInfo.Length() / 6; nDefectIdx++) {
//						m_DefectData[nCurDoc].arrayDefectCenters.push_back(CPoint(hDefectInfo[6 * nDefectIdx].D(), hDefectInfo[6 * nDefectIdx + 1].D()));
//						m_DefectData[nCurDoc].arrayDefectRects.push_back(CRect(hDefectInfo[6 * nDefectIdx + 2].D(), hDefectInfo[6 * nDefectIdx + 3].D(),
//							hDefectInfo[6 * nDefectIdx + 4].D(), hDefectInfo[6 * nDefectIdx + 5].D()));
//					}
//
//					if (pTrainingData->bCentralizedVerificationMode) {
//						//Get region points
//						HObject hRegionBorder, hRegionUnion;
//						Union1(hEncapLocation, &hRegionUnion);
//						Boundary(hRegionUnion, &hRegionBorder, "outer");
//						GetRegionContour(hRegionBorder, &hCenterY, &hCenterX);
//
//						if (hCenterX.Length() > 0) {
//							m_DefectData[nCurDoc].arrayDefectX.resize(1);
//							m_DefectData[nCurDoc].arrayDefectY.resize(1);
//							for (int nPoint = 0; nPoint < hCenterX.Length(); nPoint++) {
//								m_DefectData[nCurDoc].arrayDefectX[0].push_back(hCenterX[nPoint].I());
//								m_DefectData[nCurDoc].arrayDefectY[0].push_back(hCenterY[nPoint].I());
//							}
//						}
//						//Inner and outer Encap
//						Connection(hProjectedEncapRegion, &hProjectedEncapRegion);
//						RegionFeatures(hProjectedEncapRegion, (((HTuple("column").Append("row")).Append("column1")).Append("row1").Append("column2").Append("row2"))
//							, &hDefectInfo);
//						for (int nDefectIdx = 0; nDefectIdx < hDefectInfo.Length() / 6; nDefectIdx++) {
//							m_DefectData[nCurDoc].arrayDefectCenters.push_back(CPoint(hDefectInfo[6 * nDefectIdx].D(), hDefectInfo[6 * nDefectIdx + 1].D()));
//							m_DefectData[nCurDoc].arrayDefectRects.push_back(CRect(hDefectInfo[6 * nDefectIdx + 2].D(), hDefectInfo[6 * nDefectIdx + 3].D(),
//								hDefectInfo[6 * nDefectIdx + 4].D(), hDefectInfo[6 * nDefectIdx + 5].D()));
//						}
//
//						HTuple hDefectCount;
//						CountObj(hProjectedEncapRegion, &hDefectCount);
//						int nEncapLocCount = hDefectCount.I();
//						if (nEncapLocCount > 0) {
//							m_DefectData[nCurDoc].arrayDefectX.resize(nEncapLocCount+1);
//							m_DefectData[nCurDoc].arrayDefectY.resize(nEncapLocCount+1);
//							for (int nIndex = 1; nIndex < m_DefectData[nCurDoc].arrayDefectX.size(); nIndex++) {
//								m_DefectData[nCurDoc].arrayDefectX[nIndex].push_back(-1);
//								m_DefectData[nCurDoc].arrayDefectY[nIndex].push_back(-1);
//							}
//						}
//					}
//
//					if (hIsPassForInsufficient == FALSE)
//						return -ENCAP_INSUFFICIENT;
//					else if (hIsPassForExcess == FALSE)
//						return -ENCAP_EXCESS;
//				}
//			}
//		}
//
//		//// Cover layer (Doc - 4th intensity) ////
//		//HTuple hCoverLayerImgIndex = -1;
//		//if (m_TrainingData.hvec_TupleVectorCoverLayer.GetSize() > 0)
//		//	if (m_TrainingData.hvec_TupleVectorCoverLayer[0].Length() > 0)
//		//		hCoverLayerImgIndex = ((m_TrainingData.hvec_TupleVectorCoverLayer[/*nIndexEncap*/0][0]) /*- 1*/);
//		if (m_CoverLayerParm.bEnable ) {
//			if(!m_TrainingData.bEnableCoverLayer)
//				return -COVER_LAYER_INSP;
//			
//			HTuple hMaxRotAngle, PatternSearchAreaX, PatternSearchAreaY, hMinAccScore, hNoOfRegion;
//			HTuple hCLDatumRow, hCLDatumCol, hCLModelID, hRefAngle, hRefPointRow, hRefPointCol;
//			HTuple hRow, hCol, hRectRow, hRectCol;
//
//			int nIndexCL = 0;
//			hNoOfRegion = m_TrainingData.nCLDatum;
//			hMaxRotAngle = m_TrainingData.nCLMaxRotationAngle;
//			PatternSearchAreaX = m_TrainingData.nCLPatternSearchX;
//			PatternSearchAreaY = m_TrainingData.nCLPatternSearchY;
//			hMinAccScore = m_TrainingData.nCLMinAcceptanceScore;
//			hCLModelID = m_TrainingData.hvec_TupleVectorCoverLayer[nIndexCL++];
//			hRefAngle = m_TrainingData.hvec_TupleVectorCoverLayer[nIndexCL++];
//			hRefPointRow = m_TrainingData.hvec_TupleVectorCoverLayer[nIndexCL++];
//			hRefPointCol = m_TrainingData.hvec_TupleVectorCoverLayer[nIndexCL++];
//
//			HTuple hDebugMsgOutEncapCLRP;
//			HImage hDebugImageOutEncapCLRP;
//			HRegion hDebugRegionOutEncapCLRP;
//			HRegion hCLRegion, hInspProjectedRegionCL, hCVLRegion, hRect;
//			GenEmptyObj(&hInspProjectedRegionCL);
//
//			for (int i = 0; i<m_TrainingData.nCLInspRegion; i++) {
//				TupleConcat(hRectRow, m_TrainingData.m_rectCoverLayer[i].top, &hRectRow);
//				TupleConcat(hRectRow, m_TrainingData.m_rectCoverLayer[i].bottom, &hRectRow);
//				TupleConcat(hRectCol, m_TrainingData.m_rectCoverLayer[i].left, &hRectCol);
//				TupleConcat(hRectCol, m_TrainingData.m_rectCoverLayer[i].right, &hRectCol);
//				GenRectangle1(&hRect, m_TrainingData.m_rectCoverLayer[i].top, m_TrainingData.m_rectCoverLayer[i].left,
//					m_TrainingData.m_rectCoverLayer[i].bottom, m_TrainingData.m_rectCoverLayer[i].right);
//				ConcatObj(hInspProjectedRegionCL, hRect, &hInspProjectedRegionCL);
//			}
//
//			HTuple hIsCVLDatumFound = 0;
//			if(m_TrainingData.bEnableCLDatum)
//			{
//				HTuple hMatchModelScore, hMatchAngle, hDatumMatchCR, hDatumMatchCC, hDebugMessageOut;
//				HRegion hModelRegion;
//
//				for (int i = 0; i<m_TrainingData.nCLDatum; i++) {
//					TupleConcat(hRow, m_TrainingData.m_rectCLDatum[i].top, &hRow);
//					TupleConcat(hRow, m_TrainingData.m_rectCLDatum[i].bottom, &hRow);
//					TupleConcat(hCol, m_TrainingData.m_rectCLDatum[i].left, &hCol);
//					TupleConcat(hCol, m_TrainingData.m_rectCLDatum[i].right, &hCol);
//				}
//
//				_FCI_Inspect_UniquePattern(hImage,
//					&hModelRegion, &hDebugImage, &hDebugRegion, nStepDebug,
//					PatternSearchAreaX, PatternSearchAreaY, hMaxRotAngle,
//					hMinAccScore, hCLModelID, hRow, hCol,
//					&hIsCVLDatumFound, &hMatchModelScore, &hMatchAngle,
//					&hDatumMatchCR, &hDatumMatchCC, &hDebugMessageOut);
//
//				if (nStepDebug) {
//					StepDebug(hDebugImage, hDebugRegion, colorCyan, hDebugMessageOut, bRegionInsp);
//				}
//
//				if (hIsCVLDatumFound == TRUE)
//				{
//					_FCI_ConnectedRgn_RegionProjection(hImage,
//						&hCVLRegion, &hInspProjectedRegionCL,&hDebugImageOutEncapCLRP,&hDebugRegionOutEncapCLRP,nStepDebug,
//						hNoOfRegion, hRectRow, hRectCol, hRefAngle,hRefPointRow,hRefPointCol, hMatchAngle, hDatumMatchCR,
//						hDatumMatchCC,&hDebugMsgOutEncapCLRP);
//				}
//			}
//
//			//If can not found the datum or CVL datum is disabled --> following the shifted was found by Device Locatio Step
//			if (hIsCVLDatumFound == FALSE) {
//				_FCI_Encap_RegionProjection(hImage, hInspProjectedRegionCL,
//					&hInspProjectedRegionCL, &hDebugImageOutEncapCLRP, &hDebugRegionOutEncapCLRP, 0,
//					hDeviceCenterRow, hDeviceCenterCol, hInspectRotationAngle,
//					hInspectShiftAlongRow, hInspectShiftAlongCol, &hDebugMsgOutEncapCLRP);
//			}
//
//			HRegion hMissingCVLRegion;
//			GenEmptyObj(&hMissingCVLRegion);
//
//			HTuple hCoverLayerPresent;
//			GenEmptyObj(&hCLRegion);
//			for (int i = 0; i < m_TrainingData.nCLInspRegion; i++) {
//				HRegion hSelectedRegion, hCLDefectRegion;
//				SelectObj(hInspProjectedRegionCL, &hSelectedRegion, i+1);
//
//				HTuple hDebugMsgOutEncapCLDefect;
//				HImage hDebugImageOutEncapCLDefect;
//				HRegion hDebugRegionOutEncapCLDefect;
//				HTuple hContrast = m_CoverLayerParm.nContrast[i];
//				HTuple hMinLength = m_CoverLayerParm.nCoverLayerLength[i] / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//				HTuple hMinHeight = m_CoverLayerParm.nCoverLayerHeight[i] / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//				HTuple hMaxGap = m_CoverLayerParm.dMaskSize[i] / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//				HTuple hGapWidthTolerance = m_CoverLayerParm.dGapWidthTolerance[i] / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//				HTuple hGapHeightTolerance = m_CoverLayerParm.dGapHeightTolerance[i] / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//				HTuple hIsCoverLayer;
//
//				_FCI_Inspect_CoverLayer(hImage, hSelectedRegion,
//					&hCLDefectRegion,&hDebugImageOutEncapCLDefect,&hDebugRegionOutEncapCLDefect,nStepDebug,
//					hContrast, hMaxGap, hGapWidthTolerance, hGapHeightTolerance, hMinLength, hMinHeight, nCurFOV, nTotalFOV,
//					&hCoverLayerPresent,&hDebugMsgOutEncapCLDefect);
//
//				if (nStepDebug) {
//					StepDebug(hDebugImageOutEncapCLDefect, hDebugRegionOutEncapCLDefect, colorCyan, hDebugMsgOutEncapCLDefect, bRegionInsp);
//				}
//
//				if (hCoverLayerPresent == TRUE)
//					ConcatObj(hCLRegion, hCLDefectRegion, &hCLRegion);
//				else
//					ConcatObj(hMissingCVLRegion, hSelectedRegion, &hMissingCVLRegion);
//			}
//
//			HTuple hNumOfCL;
//			CountObj(hCLRegion, &hNumOfCL);
//
//			m_arrayOverlayInspection.Add(hCLRegion, colorCyan);
//			if (hNumOfCL < m_TrainingData.nCLInspRegion){
//				//Store the defect information for Deep Learning Saving
//				HTuple hCenterX, hCenterY, hTop, hLeft, hBottom, hRight, hDefectInfo;
//				Connection(hMissingCVLRegion, &hMissingCVLRegion);
//				RegionFeatures(hMissingCVLRegion, (((HTuple("column").Append("row")).Append("column1")).Append("row1").Append("column2").Append("row2"))
//					, &hDefectInfo);
//
//				CleanDefectData(nCurDoc);
//
//				for (int nDefectIdx = 0; nDefectIdx < hDefectInfo.Length() / 6; nDefectIdx++) {
//					m_DefectData[nCurDoc].arrayDefectCenters.push_back(CPoint(hDefectInfo[6 * nDefectIdx].D(), hDefectInfo[6 * nDefectIdx + 1].D()));
//					m_DefectData[nCurDoc].arrayDefectRects.push_back(CRect(hDefectInfo[6 * nDefectIdx + 2].D(), hDefectInfo[6 * nDefectIdx + 3].D(),
//						hDefectInfo[6 * nDefectIdx + 4].D(), hDefectInfo[6 * nDefectIdx + 5].D()));
//				
//					if (pTrainingData->bCentralizedVerificationMode) {
//						//Create Fake Point --> Rectangle Information is enough --> No need get the region points
//						m_DefectData[nCurDoc].arrayDefectX.push_back(std::vector<int>(1, 0));
//						m_DefectData[nCurDoc].arrayDefectY.push_back(std::vector<int>(1, 0));
//					}
//				}
//
//				return -COVER_LAYER_INSP;
//			}
//
//			if (m_CoverLayerAlignmentParm.bEnablePointer)
//			{
//				HTuple  hNoOfPointers;
//				HTuple hPointerRows = HTuple();
//				HTuple hPointerCols = HTuple();
//
//				hNoOfPointers = m_TrainingData.hvec_TupleVectorCoverLayerPointer.GetAt(1);
//
//				HTuple hDebugMsgOutPointerRP;
//				HImage hDebugImageOutPointerRP;
//				HRegion hDebugRegionOutPointerRP;
//				HRegion hPointerRegion, hRectPointer, hProjectedPointerRegion;
//				HTuple hRectRow, hRectCol;
//					
//				if (m_TrainingData.bEnableManualPointer == TRUE)
//				{
//					GenEmptyObj(&pTrainingData->hObjectPointerLocation);
//					for (int nPointer = 0; nPointer < hNoOfPointers; nPointer++)
//					{
//						GenEmptyObj(&hRectPointer);
//						GenEmptyObj(&hPointerRegion);
//						TupleConcat(hPointerRows, m_TrainingData.m_rectPointer[nPointer].top, &hPointerRows);
//						TupleConcat(hPointerRows, m_TrainingData.m_rectPointer[nPointer].bottom, &hPointerRows);
//						TupleConcat(hPointerCols, m_TrainingData.m_rectPointer[nPointer].left, &hPointerCols);
//						TupleConcat(hPointerCols, m_TrainingData.m_rectPointer[nPointer].right, &hPointerCols);
//						GenRectangle1(&hRectPointer, m_TrainingData.m_rectPointer[nPointer].top, m_TrainingData.m_rectPointer[nPointer].left,
//							m_TrainingData.m_rectPointer[nPointer].bottom, m_TrainingData.m_rectPointer[nPointer].right);
//						_FCI_CVLA_ManualPointer_RgnProjection(hImage, hRectPointer,
//							&hPointerRegion, &hDebugImageOutPointerRP, &hDebugRegionOutPointerRP, nStepDebug,
//							hDeviceCenterRow, hDeviceCenterCol, hInspectRotationAngle, hInspectShiftAlongRow, hInspectShiftAlongCol, hNoOfPointers,
//							&hDebugMsgOutPointerRP);
//						m_arrayOverlayInspection.Add(hPointerRegion, colorBlue);
//						ConcatObj(pTrainingData->hObjectPointerLocation, hPointerRegion, &pTrainingData->hObjectPointerLocation);
//						if (nStepDebug) {
//							StepDebug(hDebugImageOutPointerRP, hDebugRegionOutPointerRP, colorCyan, hDebugMsgOutPointerRP, bRegionInsp);
//						}
//					}
//				}
//				for (int nPointer = 0; nPointer < hNoOfPointers; nPointer++)
//				{
//					HTuple hNoOfPointers, hMaxRotationAngle, hPatternSearchAreaAlongX, hPatternSearchAreaAlongY, hMinAcceptanceScore, hEnableManualPointer;
//					HTuple hIsPass, hPercentageOut;
//					HRegion hDebugRegionOut, hPointerRegion;
//					HRegion hSelectedPointer, hSelectedCoverLayerSkeleton, hSelectedCoverLayerInsufficient;
//					hNoOfPointers = m_TrainingData.hvec_TupleVectorCoverLayerPointer.GetAt(1);
//					HTuple hTolerance = (m_CoverLayerAlignmentParm.dToleranceDist[nPointer] / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2)) * 2;
//					HTuple hIsHorizontal = m_CoverLayerAlignmentParm.nCoverLayerDirection[nPointer];
//					int nIndexCLAP = 0;
//					if (m_TrainingData.bEnableManualPointer == TRUE)
//					{
//						_FCI_Inspect_CoverLayerAlignmentUsingManualPointer(hImage, pTrainingData->hObjectPointerLocation, hCLRegion,
//							&hSelectedPointer, &hSelectedCoverLayerSkeleton,
//							 hTolerance, hIsHorizontal, nPointer + 1,
//							&hIsPass, &hPercentageOut);
//					}
//					else
//					{
//						_FCI_Inspect_CoverLayerAlignmentUsingPointer(hImage, pTrainingData->hObjectPointerLocation, hCLRegion,
//							&hSelectedPointer, &hSelectedCoverLayerSkeleton,
//							hTolerance, hIsHorizontal, nPointer + 1,
//							&hIsPass, &hPercentageOut);			
//					}
//					if (hIsPass == 0) {
//						m_arrayOverlayInspection.Add(hSelectedCoverLayerInsufficient, colorRed);
//						return -COVER_LAYER_ALIGNMENT;
//					}
//					else
//					{
//						m_arrayOverlayInspection.Add(hSelectedPointer, colorYellow);
//						m_arrayOverlayInspection.Add(hSelectedCoverLayerSkeleton, colorYellow);
//					}
//				}
//			}
//		}
//		if (m_SlotParm.bEnable) {
//			CleanDefectData(nCurDoc);
//
//			int nIsPassAll[5] = { 1,1,1,1,1 };
//			HTuple hDebugMsgOutSlot;
//			HImage hDebugImageOutSlot;
//			HRegion hDebugRegionOutSlot, hConcatSlotMaskRgnProjected;
//			HTuple hMessage;
//			HRegion hConcatSlotMaskRgn;
//			HTuple hSlotPolygonPoints, hSlotRow1, hSlotCol1, hSlotRow2, hSlotCol2;
//			GenEmptyObj(&hConcatSlotMaskRgnProjected);
//			GenEmptyObj(&hConcatSlotMaskRgn);
//			for (int nMask = 0; nMask < m_TrainingData.nSlotMaskNumber; nMask++) {
//				HRegion hSlotMask;
//				hSlotPolygonPoints = HTuple();
//				if (m_TrainingData.nSlotMaskType[nMask] == RECTANGLE) {
//					hSlotRow1 = m_TrainingData.m_rectMaskSlot[nMask].top;
//					hSlotCol1 = m_TrainingData.m_rectMaskSlot[nMask].left;
//					hSlotRow2 = m_TrainingData.m_rectMaskSlot[nMask].bottom;
//					hSlotCol2 = m_TrainingData.m_rectMaskSlot[nMask].right;
//					GenRectangle1(&hSlotMask, hSlotRow1, hSlotCol1, hSlotRow2, hSlotCol2);
//				}
//				else if (m_TrainingData.nSlotMaskType[nMask] == ECLIPSE) {
//					GenEllipse(&hSlotMask, m_TrainingData.m_rectMaskSlot[nMask].CenterPoint().y, m_TrainingData.m_rectMaskSlot[nMask].CenterPoint().x, 0,
//						m_TrainingData.m_rectMaskSlot[nMask].Width() / 2, m_TrainingData.m_rectMaskSlot[nMask].Height() / 2);
//				}
//				else if (m_TrainingData.nSlotMaskType[nMask] == POLYGON) {
//					HTuple hPoint;
//					HTuple hRows, hCols;
//					for (int nPointId = 0; nPointId < m_TrainingData.nSlotMaskPolygonPointCount[nMask]; nPointId++) {
//						TupleConcat(hRows, m_TrainingData.m_SlotMaskPolygonPoint[nMask][nPointId].y, &hRows);
//						TupleConcat(hCols, m_TrainingData.m_SlotMaskPolygonPoint[nMask][nPointId].x, &hCols);
//
//						hPoint.Clear();
//						hPoint.Append(m_TrainingData.m_SlotMaskPolygonPoint[nMask][nPointId].y);
//						hPoint.Append(m_TrainingData.m_SlotMaskPolygonPoint[nMask][nPointId].x);
//
//						//TupleConcat(hSlotPolygonPoints, hPoint, &hSlotPolygonPoints);
//					}
//					GenRegionPolygonFilled(&hSlotMask, hRows, hCols);
//				}
//				Union2(hConcatSlotMaskRgn, hSlotMask, &hConcatSlotMaskRgn);
//				HTuple hArea, hDummy;
//				AreaCenter(hConcatSlotMaskRgn, &hArea, &hDummy, &hDummy);
//				if(hArea > 0)
//					_FCI_Encap_RegionProjection(hImage, hConcatSlotMaskRgn,
//						&hConcatSlotMaskRgnProjected, &hDebugImageOutSlot, &hDebugRegionOutSlot, 0,
//						hDeviceCenterRow, hDeviceCenterCol, hInspectRotationAngle,
//						hInspectShiftAlongRow, hInspectShiftAlongCol, &hDebugMsgOutSlot);
//			}
//			m_arrayOverlayInspection.Add(hConcatSlotMaskRgnProjected, colorYellow);
//
//			for (int k = 0; k < m_TrainingData.nNumOfSlot; k++) {
//				HTuple hSlotRow1, hSlotCol1, hSlotRow2, hSlotCol2, hMinIntensity, hContrastEdgeSlot, hIsPass;
//				HRegion hSlotTeachRegion, hSlotTeachProjected, hSlotLocation;
//				if (m_TrainingData.nSlotAreaType == RECTANGLE) {
//					hSlotRow1 = m_TrainingData.m_rectSlot[k].top;
//					hSlotCol1 = m_TrainingData.m_rectSlot[k].left;
//					hSlotRow2 = m_TrainingData.m_rectSlot[k].bottom;
//					hSlotCol2 = m_TrainingData.m_rectSlot[k].right;
//					GenRectangle1(&hSlotTeachRegion, hSlotRow1, hSlotCol1, hSlotRow2, hSlotCol2);
//				}
//				else {
//					HTuple hRows, hCols;
//					for (int nPointId = 0; nPointId < m_TrainingData.nSlotPolygonPointCount; nPointId++) {
//						TupleConcat(hRows, m_TrainingData.m_SlotPolygonPoint[k][nPointId].y, &hRows);
//						TupleConcat(hCols, m_TrainingData.m_SlotPolygonPoint[k][nPointId].x, &hCols);
//					}
//					GenRegionPolygonFilled(&hSlotTeachRegion, hRows, hCols);
//				}
//				
//				_FCI_Encap_RegionProjection(hImage, hSlotTeachRegion,
//					&hSlotTeachProjected, &hDebugImageOutSlot, &hDebugRegionOutSlot, 0,
//					hDeviceCenterRow, hDeviceCenterCol, hInspectRotationAngle,
//					hInspectShiftAlongRow, hInspectShiftAlongCol, &hDebugMsgOutSlot);
//
//				//m_arrayOverlayInspection.Add(hSlotTeachProjected, colorYellow);
//
//				hMinIntensity = m_TrainingData.nMinIntensitySlot;
//				hContrastEdgeSlot = m_TrainingData.nContrastEdgeSlot;
//				_FCI_Inspect_SlotLocation(hImage, hSlotTeachProjected, 
//					&hSlotLocation, &hDebugImageOutSlot, &hDebugRegionOutSlot,
//					nStepDebug, hMinIntensity, hContrastEdgeSlot,
//					&hIsPass, &hDebugMsgOutSlot);
//				if (nStepDebug) {
//					StepDebug(hDebugImageOutSlot, hDebugRegionOutSlot, colorCyan, hDebugMsgOutSlot, bRegionInsp);
//				}
//				if (hIsPass == 1) {
//					m_arrayOverlayInspection.Add(hSlotLocation, colorOrange);
//					if (m_SlotParm.bEnableCheckSurface[k]) {
//						HTuple hDummy, hColumnSlotLocation, hRowSlotLocation;
//						AreaCenter(hSlotLocation, &hDummy, &hRowSlotLocation, &hColumnSlotLocation);
//						HTuple hSlotLimitRow1, hSlotLimitCol1, hSlotLimitRow2, hSlotLimitCol2;
//						HRegion hSlotLimitTeachRegion, hSlotLimitTeachProjected;
//
//						if (m_TrainingData.nSlotLimitAreaType == RECTANGLE) {
//							hSlotLimitRow1 = m_TrainingData.m_rectSlotLimit[k].top;
//							hSlotLimitCol1 = m_TrainingData.m_rectSlotLimit[k].left;
//							hSlotLimitRow2 = m_TrainingData.m_rectSlotLimit[k].bottom;
//							hSlotLimitCol2 = m_TrainingData.m_rectSlotLimit[k].right;
//							GenRectangle1(&hSlotLimitTeachRegion, hSlotLimitRow1, hSlotLimitCol1, hSlotLimitRow2, hSlotLimitCol2);
//						}
//						//POLYGON Type
//						else {
//							HTuple hRows, hCols;
//							for (int nPointId = 0; nPointId < m_TrainingData.nSlotLimitPolygonPointCount; nPointId++) {
//								TupleConcat(hRows, m_TrainingData.m_SlotLimitPolygonPoint[k][nPointId].y, &hRows);
//								TupleConcat(hCols, m_TrainingData.m_SlotLimitPolygonPoint[k][nPointId].x, &hCols);
//							}
//							GenRegionPolygonFilled(&hSlotLimitTeachRegion, hRows, hCols);
//						}
//
//						HTuple hColumnSlotLimitLocation, hShiftColumnSlotLimit, hRowSlotLimitLocation, hShiftRowSlotLimit;
//						AreaCenter(hSlotLimitTeachRegion, &hDummy, &hRowSlotLimitLocation, &hColumnSlotLimitLocation);
//						hShiftRowSlotLimit = hRowSlotLimitLocation - hRowSlotLocation;
//						hShiftColumnSlotLimit = hColumnSlotLimitLocation - hColumnSlotLocation;
//
//						/*HTuple hSlotSize, hSlotWidth, hSlotHeight;
//						RegionFeatures(hSlotLimitTeachRegion, (HTuple("width").Append("height")), &hSlotSize);
//						hSlotWidth = hSlotSize[0];
//						hSlotHeight = hSlotSize[1];
//						if (hSlotHeight > hSlotWidth) {
//							_FCI_Encap_RegionProjection(hImage, hSlotLimitTeachRegion,
//								&hSlotLimitTeachProjected, &hDebugImageOutSlot, &hDebugRegionOutSlot, 0,
//								hDeviceCenterRow, hDeviceCenterCol, hInspectRotationAngle,
//								hInspectShiftAlongRow, hShiftColumnSlotLimit, &hDebugMsgOutSlot);
//						}
//						else {
//							_FCI_Encap_RegionProjection(hImage, hSlotLimitTeachRegion,
//								&hSlotLimitTeachProjected, &hDebugImageOutSlot, &hDebugRegionOutSlot, 0,
//								hDeviceCenterRow, hDeviceCenterCol, hInspectRotationAngle,
//								hShiftRowSlotLimit, hInspectShiftAlongCol, &hDebugMsgOutSlot);
//						}*/
//
//						_FCI_Encap_RegionProjection(hImage, hSlotLimitTeachRegion,
//							&hSlotLimitTeachProjected, &hDebugImageOutSlot, &hDebugRegionOutSlot, 0,
//							hDeviceCenterRow, hDeviceCenterCol, hInspectRotationAngle,
//							hInspectShiftAlongRow, hInspectShiftAlongCol, &hDebugMsgOutSlot);
//
//						//m_arrayOverlayInspection.Add(hSlotLimitTeachProjected, colorCyan);
//
//						HTuple hEdgeOffset, hMinGVDiff, hMinContrast, hMinSizeDefect, hMinLengthDefect, hMinSquareSizeDefect, hMinCount;
//						HRegion hSlotDefect;
//						HTuple hAllDefectMinSize, hAllDefectMinLength, hAllDefectMinSqSize;
//						hEdgeOffset = m_SlotParm.nEdgeOffset[k] / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//						hMinGVDiff = m_SlotParm.nMinMeanGVDiffSlot[k];
//						hMinContrast = m_SlotParm.nMinContrast[k];
//						hMinSizeDefect = m_SlotParm.nMinSize[k] / (pCalibData->dResolutionAlongXInMicronPerPixel*pCalibData->dResolutionAlongYInMicronPerPixel);
//						hMinLengthDefect = m_SlotParm.nMinLength[k] / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//						hMinSquareSizeDefect = m_SlotParm.nMinSquareSize[k] / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//						hMinCount = m_SlotParm.nMinCount[k];
//						HRegion hSlotLocationMasking;
//						Difference(hSlotLocation, hConcatSlotMaskRgnProjected, &hSlotLocationMasking);
//
//						_FCI_Inspect_SlotDefect(hImage, hSlotLocation, hSlotLimitTeachProjected, hSlotLocationMasking, &hSlotDefect, &hDebugImageOutSlot, &hDebugRegionOutSlot,
//							nStepDebug, hEdgeOffset, hMinGVDiff, hMinContrast, hMinSizeDefect, hMinLengthDefect, hMinSquareSizeDefect, hMinCount,
//							&hIsPass, &hDebugMsgOutSlot, &hAllDefectMinSize, &hAllDefectMinLength, &hAllDefectMinSqSize);
//						if (nStepDebug) {
//							StepDebug(hDebugImageOutSlot, hDebugRegionOutSlot, colorCyan, hDebugMsgOutSlot, bRegionInsp);
//						}
//						if (hIsPass == FALSE) {
//							m_arrayOverlayInspection.Add(hSlotDefect, colorRed);
//							CString str;
//							HTuple hResolution = ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//							HTuple hValue = hAllDefectMinSize * (pCalibData->dResolutionAlongXInMicronPerPixel*pCalibData->dResolutionAlongYInMicronPerPixel);
//							double dValue = hValue.D();
//							str.Format("Fov[%d] %s Slot Defect Min Size %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hAllDefectMinSize.D(), dValue);
//							strArrayInspValues.Add(str);
//
//							hValue = hAllDefectMinLength * hResolution;
//							dValue = hValue.D();
//							str.Format("Fov[%d] %s Slot Defect Min Length %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hAllDefectMinLength.D(), dValue);
//							strArrayInspValues.Add(str);
//
//							hValue = hAllDefectMinSqSize * hResolution;
//							dValue = hValue.D();
//							str.Format("Fov[%d] %s Slot Defect Min Square Size %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, hAllDefectMinSqSize.D(), dValue);
//							strArrayInspValues.Add(str);
//
//							HTuple hCenterX, hCenterY, hTop, hLeft, hBottom, hRight, hDefectInfo;
//							Connection(hSlotDefect, &hSlotDefect);
//							RegionFeatures(hSlotDefect, (((HTuple("column").Append("row")).Append("column1")).Append("row1").Append("column2").Append("row2"))
//								, &hDefectInfo);
//
//							for (int nDefectIdx = 0; nDefectIdx < hDefectInfo.Length() / 6; nDefectIdx++) {
//								m_DefectData[nCurDoc].arrayDefectCenters.push_back(CPoint(hDefectInfo[6 * nDefectIdx].D(), hDefectInfo[6 * nDefectIdx + 1].D()));
//								m_DefectData[nCurDoc].arrayDefectRects.push_back(CRect(hDefectInfo[6 * nDefectIdx + 2].D(), hDefectInfo[6 * nDefectIdx + 3].D(),
//									hDefectInfo[6 * nDefectIdx + 4].D(), hDefectInfo[6 * nDefectIdx + 5].D()));
//							}
//							if (pTrainingData->bCentralizedVerificationMode) {
//								//Get region points
//								HObject hRegionBorder;
//								HTuple hDefectCount;
//								CountObj(hSlotDefect, &hDefectCount);
//								int nDefectCount = hDefectCount.I();
//								for (int nDefectIdx = 0; nDefectIdx < nDefectCount; nDefectIdx++) {
//									HObject hCurrentDefect;
//									SelectObj(hSlotDefect, &hCurrentDefect, HTuple(nDefectIdx + 1));
//									Boundary(hCurrentDefect, &hRegionBorder, "outer");
//									GetRegionContour(hRegionBorder, &hCenterY, &hCenterX);
//
//									if (hCenterX.Length() > 0) {
//										std::vector<int> arrayX;
//										std::vector<int> arrayY;
//										for (int nPoint = 0; nPoint < hCenterX.Length(); nPoint++) {
//											arrayX.push_back(hCenterX[nPoint].I());
//											arrayY.push_back(hCenterY[nPoint].I());
//										}
//										m_DefectData[nCurDoc].arrayDefectX.push_back(arrayX);
//										m_DefectData[nCurDoc].arrayDefectY.push_back(arrayY);
//									}
//									else {
//										m_DefectData[nCurDoc].arrayDefectX.push_back(std::vector<int>(1, 0));
//										m_DefectData[nCurDoc].arrayDefectY.push_back(std::vector<int>(1, 0));
//									}
//								}
//							}
//
//
//							nIsPassAll[k] = 0;
//						}
//					}
//				}
//				else {
//					m_arrayOverlayInspection.Add(hSlotTeachProjected, colorRed);
//
//					HTuple hCenterX, hCenterY, hTop, hLeft, hBottom, hRight, hDefectInfo;
//					Connection(hSlotTeachProjected, &hSlotTeachProjected);
//					RegionFeatures(hSlotTeachProjected, (((HTuple("column").Append("row")).Append("column1")).Append("row1").Append("column2").Append("row2"))
//						, &hDefectInfo);
//
//					for (int nDefectIdx = 0; nDefectIdx < hDefectInfo.Length() / 6; nDefectIdx++) {
//						m_DefectData[nCurDoc].arrayDefectCenters.push_back(CPoint(hDefectInfo[6 * nDefectIdx].D(), hDefectInfo[6 * nDefectIdx + 1].D()));
//						m_DefectData[nCurDoc].arrayDefectRects.push_back(CRect(hDefectInfo[6 * nDefectIdx + 2].D(), hDefectInfo[6 * nDefectIdx + 3].D(),
//							hDefectInfo[6 * nDefectIdx + 4].D(), hDefectInfo[6 * nDefectIdx + 5].D()));
//					
//						if (pTrainingData->bCentralizedVerificationMode) {
//							//Create Fake Point --> Rectangle Information is enough --> No need get the region points
//							m_DefectData[nCurDoc].arrayDefectX.push_back(std::vector<int>(1, 0));
//							m_DefectData[nCurDoc].arrayDefectY.push_back(std::vector<int>(1, 0));
//						}
//					}
//
//					nIsPassAll[k] = 0;
//					continue;
//				}
//			}
//
//			for (int k = 0; k < 5; k++) {
//				if (nIsPassAll[k] == 0) {
//					return -SLOT;
//				}
//			}
//		}
//		//// Device Edge Insp ////
//		if (m_DieEdgeParm.bEnable)
//		{
//			HTuple hDeviceLocationEnable = m_TrainingData.hvec_TupleVectorLocation.GetAt(0);
//			if (hDeviceLocationEnable == 1)
//			{
//				HImage hImageForPVI_Inspection, hImageRotate, hImageRotateShift;
//				HImage hDebugImageOutPviRegion;
//				HRegion hDebugRegionOutPviRegion;
//				HRegion hRegionForPVI_Inspection, hRegionAffineTrans, hRegionAffineRotateShift;
//				HTuple hDebugMsgOutPviRegion;
//			
//				HTuple hLocationSelectedRow, hLocationSelectedCol, hLocationSelectedPhi, hLocationSelectedLen1, hLocationSelectedLen2;
//				SmallestRectangle2(hDeviceLocationSelected, &hLocationSelectedRow, &hLocationSelectedCol, &hLocationSelectedPhi, &hLocationSelectedLen1, &hLocationSelectedLen2);
//
//				Projection_Function(hImage, hDeviceLocationSelected, &hRegionAffineRotateShift, &hImageRotateShift,
//					&hDebugImageOutPviRegion, &hDebugRegionOutPviRegion, nStepDebug,
//					hInspectRotationAngle, hLocationSelectedRow, hLocationSelectedCol, hInspectShiftAlongRow,
//					hInspectShiftAlongCol, 0, 0, &hDebugMsgOutPviRegion);
//
//				
//				HImage hImageAbsDiff, hImageSub;
//				AbsDiffImage(hTeachImage, hImageRotateShift, &hImageAbsDiff, 1);
//				HRegion hFitMaskRegion, hPviTeachRegion;
//				GenEmptyRegion(&hFitMaskRegion);
//				GenEmptyRegion(&hPviTeachRegion);
//
//				Projection_Back_Function(hImage,hRegionForPVI_Inspection, hImageAbsDiff, hFitMaskRegion, hPviTeachRegion, &hRegionForPVI_Inspection, &hImageSub,
//					&hDebugImageOutPviRegion, &hDebugRegionOutPviRegion, nStepDebug,
//					-hInspectShiftAlongRow, -hInspectShiftAlongCol, 0, 0, -hInspectRotationAngle, hLocationSelectedRow, hLocationSelectedCol,
//					0, &hDebugMsgOutPviRegion);
//
//				//// Rotate Teach Region
//				HTuple hSobelAmp, hDeviceLocationEnable, hNoOfLocationRegions, hDeviceLocWidth, hDeviceLocHeight, hDeviceMaskSize;
//				HTuple hMinDeviceContrast, hLocationTeachRows, hLocationTeachCols, hDummy, hReferencePointType;
//
//				int nIndex = 0;
//				hDeviceLocationEnable = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hNoOfLocationRegions = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDeviceLocWidth = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDeviceLocHeight = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDeviceMaskSize = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hMinDeviceContrast = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hSobelAmp = m_TrainingData.bSobelAmp;
//				hReferencePointType = m_TrainingData.nDeviceReferencePointType;
//
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hLocationTeachRows = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hLocationTeachCols = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//
//				//For Device Edge Detection option
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//				hDummy = m_TrainingData.hvec_TupleVectorLocation.GetAt(nIndex++);
//
//				HRegion hTeachLocationRegion;
//				_FCI_Inspect_EdgeLocation(hTeachImage,
//					&hTeachLocationRegion, hSobelAmp, hNoOfLocationRegions, hDeviceLocWidth, hDeviceLocHeight, hMinDeviceContrast,
//					hLocationTeachRows, hLocationTeachCols, hDeviceMaskSize, nCurFOV, nTotalFOV, nCurTrack + 1, hReferencePointType, 
//					&hDummy, &hDummy, &hDummy);
//
//				HTuple hTeachLocationRow, hTeachLocationCol, hTeachLocationPhi, hTeachLocationLen1, hTeachLocationLen2;
//				SmallestRectangle2(hTeachLocationRegion, &hTeachLocationRow, &hTeachLocationCol, &hTeachLocationPhi, &hTeachLocationLen1, &hTeachLocationLen2);
//
//				Projection_Function(hImage, hTeachLocationRegion, &hRegionAffineRotateShift, &hImageRotateShift,
//					&hDebugImageOutPviRegion, &hDebugRegionOutPviRegion, nStepDebug,
//					-hInspectRotationAngle, hTeachLocationRow, hTeachLocationCol, -hInspectShiftAlongRow,
//					-hInspectShiftAlongCol, 0, 0, &hDebugMsgOutPviRegion);
//
//				////
//
//				//// Set Offset & width
//				HTuple hOffset = m_DieEdgeParm.dOffset > 0 ? m_DieEdgeParm.dOffset / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2) : m_DieEdgeParm.dOffset;
//				HTuple hWidth = m_DieEdgeParm.dWidth > 0 ? m_DieEdgeParm.dWidth / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2) : m_DieEdgeParm.dWidth;
//				HRegion hDieEdgeInspRegion = hDeviceLocationSelected;
//				HImage hImageSubEdge;
//
//
//				HImage hDebugImageForPVI_Inspection;
//				HRegion hDebugRegionForPVI_Inspection;
//				HTuple hDebugMessageOut;
//
//				_FCI_OffsetRegions(hRegionAffineRotateShift, hImageSub,
//					&hDieEdgeInspRegion, &hImageSubEdge,
//					&hDebugImageForPVI_Inspection, &hDebugRegionForPVI_Inspection,
//					nStepDebug, hOffset, hWidth,0,TRUE,
//					&hDebugMessageOut);
//
//				if (nStepDebug) 
//					StepDebug(hDebugImageForPVI_Inspection, hDebugRegionForPVI_Inspection, colorCyan, hDebugMessageOut, bRegionInsp);
//
//				if (hOffset > 0) {
//					m_arrayOverlayInspection.Add(hDieEdgeInspRegion, colorCyan);
//					if (hWidth > 0) {
//						HRegion hFillUp, hHole;
//						FillUp(hDieEdgeInspRegion, &hFillUp);
//						Difference(hFillUp, hRegionForPVI_Inspection, &hHole);
//						m_arrayOverlayInspection.Add(hHole, colorCyan);
//					}
//				}
//
//				HRegion hHoleRegion;
//				if (hWidth > 0)
//				{
//					HRegion hFillUp;
//					FillUp(hDieEdgeInspRegion, &hFillUp);
//					Difference(hFillUp, hDieEdgeInspRegion, &hHoleRegion);
//					m_arrayOverlayInspection.Add(hHoleRegion, colorCyan);
//				}
//
//				if (nStepDebug) {
//					HTuple hMsg = "Device Edge Insp Region";
//					StepDebug(hImage, hDieEdgeInspRegion, colorCyan, hMsg, bRegionInsp);
//					HTuple hNumHole = 0;
//					if (hWidth > 0)
//					{
//						HRegion hFillUp;
//						FillUp(hDieEdgeInspRegion, &hFillUp);
//						Difference(hFillUp, hDieEdgeInspRegion, &hHoleRegion);
//						StepDebug(hImageSubEdge, hHoleRegion, colorCyan, hMsg, bRegionInsp);
//					}
//				}
//
//				HTuple hDefectCharecterstics, hContrast, hMinSize, hMinLength, hMinSquareSize, hMinCount;
//
//				hDefectCharecterstics = m_DieEdgeParm.nDefectCharacteristics;
//				hContrast = m_DieEdgeParm.nContrast;
//				hMinSize = m_DieEdgeParm.m_nSize / (pCalibData->dResolutionAlongXInMicronPerPixel*pCalibData->dResolutionAlongYInMicronPerPixel);
//				hMinLength = m_DieEdgeParm.nMinLength / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//				hMinSquareSize = m_DieEdgeParm.nMinSquareSize / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//				hMinCount = m_DieEdgeParm.nMinCount;
//
//				HTuple hPass;
//				HRegion hEdgeDefectRegion;
//
//				_FCI_PVI_Inspection(hImageSubEdge, hDieEdgeInspRegion, hImage,
//					&hEdgeDefectRegion,
//					&hDebugImage, &hDebugRegion,
//					hDefectCharecterstics, hContrast,
//					hMinLength, hMinSize, hMinSquareSize, hMinCount,
//					nStepDebug, nCurDoc,0,0,TRUE,
//					&hPass, &hDebugMsg);
//
//				if (nStepDebug) {
//					StepDebug(hDebugImage, hDebugRegion, colorRed, hDebugMsg, bRegionInsp);
//				}
//
//				if (hPass == FALSE) {
//					m_arrayOverlayInspection.Add(hEdgeDefectRegion, colorRed);
//					return -DIE_EDGE;
//				}
//
//				
//			}
//		}
//
//		//// Region Insp ////
//
//		HTuple hPVIArea, hPVIAngle, hPVI2DCenter, hPVISize = 0; 
//		if(m_TrainingData.hvec_TupleVectorPVI[0].GetSize() > 0)
//			hPVIArea = m_TrainingData.hvec_TupleVectorPVI[0][0];
//
//		if(hPVIArea != 0 && hPVIArea.Length() > 0) {
//			BOOL bPVIEnable = FALSE;
//			for (int i = 0; i < hPVIArea; i++) {
//				if (m_pPviInspectionSetup[i].bEnable)
//					bPVIEnable = TRUE;
//			}
//			if (bPVIEnable)
//			{ 
//
//				m_arrayOverlayPVIInsp.RemoveAll();
//
//				HImage hImageForPVI_Inspection, hImageRotate, hImageRotateShift;
//				HRegion hRegionForPVI_Inspection, hRegionAffineTrans, hRegionAffineRotateShift;
//				HTuple hDebugMsgOutPviRegion;
//				HImage hDebugImageOutPviRegion;
//				HRegion hDebugRegionOutPviRegion;
//				HTuple hMaskNumber;
//				HTuple hMask2DCenterPoints, hMaskAngleConcat, hMaskSize, hPolygonPoints;
//				HTuple hSlotNumber;
//				HTuple hSlot2DCenterPoints, hSlotAngleConcat, hSlotSize;
//				HTuple hTeachModelRowCentre, hTeachModelColCentre;
//				HTuple hMatchModelCentreRow, hMatchModelCentreCol;
//
//				HTuple hLocationSelectedRow, hLocationSelectedCol, hLocationSelectedPhi, hLocationSelectedLen1, hLocationSelectedLen2;
//				SmallestRectangle2(hDeviceLocationSelected, &hLocationSelectedRow, &hLocationSelectedCol, &hLocationSelectedPhi, &hLocationSelectedLen1, &hLocationSelectedLen2);
//				HTuple hDebugMsgOutPviRegion_T;
//				HImage hDebugImageOutPviRegion_T;
//				HRegion hDebugRegionOutPviRegion_T;
//
//				Projection_Function(hImage, hDeviceLocationSelected, &hRegionAffineRotateShift, &hImageRotateShift,
//					&hDebugImageOutPviRegion_T, &hDebugRegionOutPviRegion_T, nStepDebug,
//					hInspectRotationAngle, hLocationSelectedRow, hLocationSelectedCol, hInspectShiftAlongRow, 
//					hInspectShiftAlongCol, 0, 0, &hDebugMsgOutPviRegion_T);
//
//				HImage hImageAbsDiff;
//				AbsDiffImage(hTeachImage, hImageRotateShift, &hImageAbsDiff, 1);
//
//				for (int nArea = 0; nArea < hPVIArea; nArea++) {
//
//					if (!m_pPviInspectionSetup[nArea].bEnable && !m_pPviInspectionSetup[nArea].bEdgeEnable)
//						continue;
//
//					int nIndex = 1;
//					hPVIAngle = m_TrainingData.hvec_TupleVectorPVI[nIndex++][nArea];
//					hPVI2DCenter = m_TrainingData.hvec_TupleVectorPVI[nIndex++][nArea];
//					hPVISize = m_TrainingData.hvec_TupleVectorPVI[nIndex++][nArea];
//
//					hMaskNumber = m_TrainingData.hvec_TupleVectorPVI[nIndex++][nArea];
//					hMask2DCenterPoints = m_TrainingData.hvec_TupleVectorPVI[nIndex++][nArea];
//					hMaskAngleConcat = m_TrainingData.hvec_TupleVectorPVI[nIndex++][nArea];
//					hMaskSize = m_TrainingData.hvec_TupleVectorPVI[nIndex++][nArea];
//
//					hSlotNumber = m_TrainingData.hvec_TupleVectorPVI[nIndex++][nArea];
//					hSlot2DCenterPoints = m_TrainingData.hvec_TupleVectorPVI[nIndex++][nArea];
//					hSlotAngleConcat = m_TrainingData.hvec_TupleVectorPVI[nIndex++][nArea];
//					hSlotSize = m_TrainingData.hvec_TupleVectorPVI[nIndex++][nArea];
//
//					hPolygonPoints = m_TrainingData.hvec_TupleVectorPVI[nIndex++][nArea];
//
//					HRegion hConcatMask, hRectMask;
//					HRegion hPviTeachRegion;
//					GenEmptyObj(&hPviTeachRegion);
//
//					if (m_pPviInspectionSetup[nArea].nDeviceAreaType == RECTANGLE)
//						GenRectangle2(&hRegionForPVI_Inspection, hPVI2DCenter[0], hPVI2DCenter[1], hPVIAngle, hPVISize[0], hPVISize[1]);
//					else if (m_pPviInspectionSetup[nArea].nDeviceAreaType == ECLIPSE)
//						GenEllipse(&hRegionForPVI_Inspection, hPVI2DCenter[0], hPVI2DCenter[1], 0,
//							m_pPviInspectionSetup[nArea].m_rectPviArea.Width() / 2, m_pPviInspectionSetup[nArea].m_rectPviArea.Height() / 2);
//					else if (m_pPviInspectionSetup[nArea].nDeviceAreaType == POLYGON) {
//						HTuple hRows, hCols;
//						for (int nPointId = 0; nPointId < m_pPviInspectionSetup[nArea].nPolygonPointNumber; nPointId++) {
//							TupleConcat(hRows, hPolygonPoints[2 * nPointId], &hRows);
//							TupleConcat(hCols, hPolygonPoints[2 * (nPointId + 1) - 1], &hCols);
//						}
//						GenRegionPolygonFilled(&hRegionForPVI_Inspection, hRows, hCols);
//					}
//
//					
//					ConcatObj(hPviTeachRegion, hRegionForPVI_Inspection, &hPviTeachRegion);
//					GenEmptyObj(&hConcatMask);
//
//					for (int nMaskIndex = 0; nMaskIndex < hMaskNumber; nMaskIndex++)
//					{
//						GenRectangle2(&hRectMask, hMask2DCenterPoints[2 * nMaskIndex], hMask2DCenterPoints[2 * (nMaskIndex + 1) - 1],
//							hMaskAngleConcat[nMaskIndex], hMaskSize[2 * nMaskIndex], hMaskSize[2 * (nMaskIndex + 1) - 1]);
//						ConcatObj(hConcatMask, hRectMask, &hConcatMask);
//					}
//
//					
//
//					//	m_arrayOverlayInspection.Add(hConcatMask, colorYellow);//TEsting 
//					HRegion hFitMaskRegion;
//					GenEmptyObj(&hFitMaskRegion);
//					if (hMaskNumber > 0)
//					{
//						Union1(hConcatMask, &hFitMaskRegion);
//						Difference(hRegionForPVI_Inspection, hFitMaskRegion, &hRegionForPVI_Inspection);
//					}
//					
//					HImage hImageRotateShiftBack, hImageSub;
//					HRegion hRegionAffineRotateShiftBack;
//
//					HTuple hDebugMsgOutPviRegionArea;
//					HImage hDebugImageOutPviRegionArea;
//					HRegion hDebugRegionOutPviRegionArea;
//
//					/*Projection_Back_Function(hRegionForPVI_Inspection, hImageAbsDiff, &hRegionForPVI_Inspection, &hImageSub,
//						&hDebugImageOutPviRegionArea, &hDebugRegionOutPviRegionArea, nStepDebug,
//						-hInspectShiftAlongRow, -hInspectShiftAlongCol, 0, 0, -hInspectRotationAngle, hLocationSelectedRow, hLocationSelectedCol,
//						nArea, &hDebugMsgOutPviRegionArea);*/
//
//					Projection_Back_Function(hImage,hRegionForPVI_Inspection, hImageAbsDiff, hFitMaskRegion, hPviTeachRegion, &hRegionForPVI_Inspection, &hImageSub,
//						&hDebugImageOutPviRegionArea, &hDebugRegionOutPviRegionArea, nStepDebug,
//						-hInspectShiftAlongRow, -hInspectShiftAlongCol, 0, 0, -hInspectRotationAngle, hLocationSelectedRow, hLocationSelectedCol,
//						nArea, &hDebugMsgOutPviRegionArea);
//					HTuple hNumHole = 0;
//					HRegion hMaskRegion;
//					HRegion hSlotRegion;
//					RegionFeatures(hRegionForPVI_Inspection, "holes_num", &hNumHole);
//					if (hNumHole > 0)
//					{
//						HRegion hFillUp;
//						FillUp(hRegionForPVI_Inspection, &hFillUp);
//						Difference(hFillUp, hRegionForPVI_Inspection, &hMaskRegion);
//						m_arrayOverlayInspection.Add(hMaskRegion, colorYellow);
//						Difference(hFillUp, hRegionForPVI_Inspection, &hSlotRegion);
//						m_arrayOverlayInspection.Add(hSlotRegion, colorYellow);
//					}
//					m_arrayOverlayInspection.Add(hRegionForPVI_Inspection, colorOrange);
//
//					if (nStepDebug) {
//						StepDebug(hDebugImageOutPviRegionArea, hDebugRegionOutPviRegionArea, colorCyan, hDebugMsgOutPviRegionArea, bRegionInsp);
//					}
//					else {
//						if (m_pPviInspectionSetup[nArea].bEnable || m_pPviInspectionSetup[nArea].bEdgeEnable) {
//							HRegion hPVIDefectRegionDummy;
//							PVIDefectsOverlay(hImageSub, hPVIDefectRegionDummy, "", colorRed);
//						}
//					}
//
//					//// PVI Insp ////
//					if (m_pPviInspectionSetup[nArea].bEnable) {
//						ReduceDomain(hImageSub, hRegionForPVI_Inspection, &hImageForPVI_Inspection);
//					}
//
//					if (m_pPviInspectionSetup[nArea].bEdgeEnable)  {
//						//// Edge insp ///
//						HTuple hEdgeWidth = m_pPviInspectionSetup[nArea].dEdgeWidth > 0 ? m_pPviInspectionSetup[nArea].dEdgeWidth / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2) : m_pPviInspectionSetup[nArea].dEdgeWidth;
//						HTuple hEdgeOffset = m_pPviInspectionSetup[nArea].dEdgeOffset > 0 ? m_pPviInspectionSetup[nArea].dEdgeOffset / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2) : m_pPviInspectionSetup[nArea].dEdgeOffset;
//
//						HTuple hMinEdgeContrast, hMaxEdgeContrast, hEdgeRgnOpen;
//						HTuple hIsEdgePVIPass, hDebugMessageOut;
//
//						HImage hDebugImageOut;
//						HRegion hDebugRegionOut;
//
//						hMinEdgeContrast = m_pPviInspectionSetup[nArea].nMinEdgeContrast;
//						hMaxEdgeContrast = m_pPviInspectionSetup[nArea].nMaxEdgeContrast;
//						hEdgeRgnOpen = m_pPviInspectionSetup[nArea].dEdgeRegionOpen / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//						if (hEdgeRgnOpen < 1)
//							hEdgeRgnOpen = 1;
//
//						_FCI_ExtractExactEdge(hImage, hRegionForPVI_Inspection,
//							&hRegionForPVI_Inspection, &hDebugImageOut, &hDebugRegionOut, 
//							nStepDebug, hMinEdgeContrast, hMaxEdgeContrast, hEdgeRgnOpen,
//							nArea,&hIsEdgePVIPass, &hDebugMessageOut);
//
//						if (nStepDebug) {
//							StepDebug(hDebugImageOut, hDebugRegionOut, colorCyan, hDebugMessageOut, bRegionInsp);
//						}
//
//						HImage hDebugImageForPVI_Inspection;
//						HRegion hDebugRegionForPVI_Inspection;
//
//						_FCI_OffsetRegions(hRegionForPVI_Inspection, hImageSub,
//							&hRegionForPVI_Inspection, &hImageForPVI_Inspection,
//							&hDebugImageForPVI_Inspection, &hDebugRegionForPVI_Inspection,
//							nStepDebug, hEdgeOffset, hEdgeWidth, 
//							nArea,FALSE,&hDebugMessageOut);
//
//						if (nStepDebug) {
//							StepDebug(hDebugImageForPVI_Inspection, hDebugRegionForPVI_Inspection, colorCyan, hDebugMessageOut, bRegionInsp);
//						}
//
//						if (hEdgeOffset > 0) {
//							m_arrayOverlayInspection.Add(hRegionForPVI_Inspection, colorCyan);
//							if (hEdgeWidth > 0) {
//								HRegion hFillUp, hHole;
//								FillUp(hRegionForPVI_Inspection, &hFillUp);
//								Difference(hFillUp, hRegionForPVI_Inspection, &hHole);
//								m_arrayOverlayInspection.Add(hHole, colorCyan);
//							}
//						}
//					}
//
//					BOOL bPVIPassAll = TRUE;
//					for (int nDefCount = 0; nDefCount < m_pPviInspectionSetup[nArea].nDefectCount; nDefCount++) {
//						if (m_pPviInspectionSetup[nArea].m_pPviDefect[nDefCount].bEnable) {
//							HTuple hDefectCharecterstics, hContrast, hMinSize, hMinLength, hMinSquareSize, hMinCount;
//
//							hDefectCharecterstics = m_pPviInspectionSetup[nArea].m_pPviDefect[nDefCount].nDefectCharacteristics;
//							hContrast = m_pPviInspectionSetup[nArea].m_pPviDefect[nDefCount].nContrast;
//							hMinSize = m_pPviInspectionSetup[nArea].m_pPviDefect[nDefCount].m_nSize / (pCalibData->dResolutionAlongXInMicronPerPixel*pCalibData->dResolutionAlongYInMicronPerPixel);
//							hMinLength = m_pPviInspectionSetup[nArea].m_pPviDefect[nDefCount].nMinLength / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//							hMinSquareSize = m_pPviInspectionSetup[nArea].m_pPviDefect[nDefCount].nMinSquareSize / ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//							hMinCount = m_pPviInspectionSetup[nArea].m_pPviDefect[nDefCount].nMinCount;
//
//							HTuple hPass;
//							HRegion hPVIDefectRegion, hActualPVIDefectRegion;
//
//							_FCI_PVI_Inspection(hImageForPVI_Inspection, hRegionForPVI_Inspection, hImage,
//								&hPVIDefectRegion,
//								&hDebugImage, &hDebugRegion,
//								hDefectCharecterstics, hContrast,
//								hMinLength, hMinSize, hMinSquareSize, hMinCount,
//								nStepDebug, nCurDoc,
//								nDefCount+1,nArea,FALSE,&hPass, &hDebugMsg);
//							if (nStepDebug) {
//								StepDebug(hDebugImage, hDebugRegion, colorRed, hDebugMsg, bRegionInsp);
//							}
//							
//							if (hPass == FALSE)
//							{
//								//m_arrayOverlayInspection.Add(hPVIDefectRegion, colorRed);
//								//return -m_pPviInspectionSetup[nArea].m_pPviDefect[nDefCount].m_nID;
//								HTuple hAllPVIDefectMinSize, hAllPVIDefectMinLength, hAllPVIDefectMinSqSize;
//
//								HTuple hMinMeanGVDiff = m_pPviInspectionSetup[nArea].nMinGVDiff;
//								HTuple hIntensityRecheck = m_pPviInspectionSetup[nArea].nIntensity;
//								BOOL bRecheckEn = m_pPviInspectionSetup[nArea].bPVIRecheckEnable;
//
//								PVI_Inspection_ReCheck(hImage, hTeachImage, hPVIDefectRegion, hRegionForPVI_Inspection,
//									&hActualPVIDefectRegion, &hDebugImage, &hDebugRegion,
//									nStepDebug, hLocationSelectedRow, hLocationSelectedCol, hInspectRotationAngle,
//									hInspectShiftAlongRow, hInspectShiftAlongCol, hDefectCharecterstics, nDefCount + 1, nArea,
//									hMinMeanGVDiff, hIntensityRecheck, bRecheckEn, FALSE,
//									&hPass, &hAllPVIDefectMinSize, &hAllPVIDefectMinLength, &hAllPVIDefectMinSqSize, &hDebugMsg);
//								
//								if (nStepDebug) {
//									StepDebug(hDebugImage, hDebugRegion, colorRed, hDebugMsg, bRegionInsp);
//								}
//
//								//PVI Slot Connection Re-check
//								if (m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckEnable && hPass == FALSE) {
//									HTuple hConnectionCheckTopSide = m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckTopDie;
//									HTuple hConnectionCheckBottomSide = m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckBottomDie;
//									HTuple hConnectionCheckLeftSide = m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckLeftDie;
//									HTuple hConnectionCheckRightSide = m_pPviInspectionSetup[nArea].bPVISlotConnectionCheckRightDie;
//
//									HRegion hConcatSlot, hRectSlot;
//									GenEmptyObj(&hConcatSlot);
//									for (int nSlotIndex = 0; nSlotIndex < m_pPviInspectionSetup[nArea].nNumOfSlot; nSlotIndex++)
//									{
//										GenRectangle2(&hRectSlot, hSlot2DCenterPoints[2 * nSlotIndex], hSlot2DCenterPoints[2 * (nSlotIndex + 1) - 1],
//											hSlotAngleConcat[nSlotIndex], hSlotSize[2 * nSlotIndex], hSlotSize[2 * (nSlotIndex + 1) - 1]);
//										ConcatObj(hConcatSlot, hRectSlot, &hConcatSlot);
//									}
//									_FCI_PVI_CheckingSlot(hRegionForPVI_Inspection, hActualPVIDefectRegion, hConcatSlot, hImage, &hActualPVIDefectRegion,
//										&hDebugImage, &hDebugRegion, nStepDebug, hDeviceCenterRow, hDeviceCenterCol, hInspectRotationAngle,
//										hInspectShiftAlongRow, hInspectShiftAlongCol, hConnectionCheckTopSide, hConnectionCheckBottomSide, hConnectionCheckLeftSide,
//										hConnectionCheckRightSide, &hPass, &hDebugMsg);
//
//									if (m_pPviInspectionSetup[nArea].nNumOfSlot) {
//										//***Project On Inspect Image: Method 1
//										HTuple  hv_HomMat2DIdentity, hv_HomMat2DRotate, hv_HomMat2DTranslate;
//										HomMat2dIdentity(&hv_HomMat2DIdentity);
//										HomMat2dRotate(hv_HomMat2DIdentity, -hInspectRotationAngle, hDeviceCenterRow, hDeviceCenterCol,
//											&hv_HomMat2DRotate);
//										HomMat2dTranslate(hv_HomMat2DRotate, -hInspectShiftAlongRow, -hInspectShiftAlongCol, &hv_HomMat2DTranslate);
//										AffineTransRegion(hConcatSlot, &hConcatSlot, hv_HomMat2DTranslate,
//											"nearest_neighbor");
//										m_arrayOverlayInspection.Add(hConcatSlot, colorYellow);
//									}
//									
//								}
//
//								HTuple hCount = 0;
//								Connection(hActualPVIDefectRegion, &hActualPVIDefectRegion);
//								CountObj(hActualPVIDefectRegion, &hCount);
//								if (0 != (hCount >= hMinCount)) {
//									m_arrayOverlayInspection.Add(hActualPVIDefectRegion, colorRed);
//									CString str;
//									HTuple hResolution = ((pCalibData->dResolutionAlongXInMicronPerPixel + pCalibData->dResolutionAlongYInMicronPerPixel) / 2);
//									HTuple hValue = hAllPVIDefectMinSize*hResolution;
//									double dValue = hValue.D();
//									str.Format("Fov[%d] %s Area[%d] Pvi Defect[%s] Min Size %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, nArea, m_pPviInspectionSetup[nArea].m_pPviDefect[nDefCount].strDefectName, hAllPVIDefectMinSize.D(), dValue);
//									strArrayInspValues.Add(str);
//
//									hValue = hAllPVIDefectMinLength*hResolution;
//									dValue = hValue.D();
//									str.Format("Fov[%d] %s Area[%d] Pvi Defect[%s] Min Length %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, nArea, m_pPviInspectionSetup[nArea].m_pPviDefect[nDefCount].strDefectName, hAllPVIDefectMinLength.D(), dValue);
//									strArrayInspValues.Add(str);
//
//									hValue = hAllPVIDefectMinSqSize*hResolution;
//									dValue = hValue.D();
//									str.Format("Fov[%d] %s Area[%d] Pvi Defect[%s] Min Square Size %.4f pixel [%.4f um]", nCurFOV, strOutPutLog, nArea, m_pPviInspectionSetup[nArea].m_pPviDefect[nDefCount].strDefectName, hAllPVIDefectMinSqSize.D(), dValue);
//									strArrayInspValues.Add(str);
//
//									//Store the defect information for Deep Learning Saving
//									HTuple hCenterX, hCenterY, hTop, hLeft, hBottom, hRight, hDefectInfo;
//									Connection(hActualPVIDefectRegion, &hActualPVIDefectRegion);
//									RegionFeatures(hActualPVIDefectRegion, (((HTuple("column").Append("row")).Append("column1")).Append("row1").Append("column2").Append("row2"))
//										,&hDefectInfo);
//
//									CleanDefectData(nCurDoc);
//
//									for (int nDefectIdx = 0; nDefectIdx < hDefectInfo.Length()/6; nDefectIdx++) {
//										m_DefectData[nCurDoc].arrayDefectCenters.push_back(CPoint(hDefectInfo[6*nDefectIdx].D(), hDefectInfo[6 * nDefectIdx + 1].D()));
//										m_DefectData[nCurDoc].arrayDefectRects.push_back(CRect(hDefectInfo[6 * nDefectIdx + 2].D(), hDefectInfo[6 * nDefectIdx +3].D(),
//																		hDefectInfo[6 * nDefectIdx + 4].D(), hDefectInfo[6 * nDefectIdx + 5].D()));
//									}
//
//									if (pTrainingData->bCentralizedVerificationMode) {
//										//Get region points
//										HObject hRegionBorder;
//										HTuple hDefectCount;
//										CountObj(hActualPVIDefectRegion, &hDefectCount);
//										int nDefectCount = hDefectCount.I();
//										m_DefectData[nCurDoc].arrayDefectX.resize(nDefectCount);
//										m_DefectData[nCurDoc].arrayDefectY.resize(nDefectCount);
//										for (int nDefectIdx = 0; nDefectIdx < nDefectCount; nDefectIdx++) {
//											HObject hCurrentDefect;
//											SelectObj(hActualPVIDefectRegion, &hCurrentDefect, HTuple(nDefectIdx + 1));
//											Boundary(hCurrentDefect, &hRegionBorder, "outer");
//											GetRegionContour(hRegionBorder, &hCenterY, &hCenterX);
//
//											if (hCenterX.Length() > 0) {
//												for (int nPoint = 0; nPoint < hCenterX.Length(); nPoint++) {
//													m_DefectData[nCurDoc].arrayDefectX[nDefectIdx].push_back(hCenterX[nPoint].I());
//													m_DefectData[nCurDoc].arrayDefectY[nDefectIdx].push_back(hCenterY[nPoint].I());
//												}
//											}
//										}
//									}
//
//									return -m_pPviInspectionSetup[nArea].m_pPviDefect[nDefCount].m_nErrorCodeID;//Check with Nguyen lol
//								}
//							}
//						}
//					}
//					//if (/*hPass == FALSE*/bPVIPassAll == FALSE) {
//					//	//m_arrayOverlayInspection.Add(hPVIDefectRegion, colorRed);
//					//	return -PVI_INSP;
//				}	//}
//			}
//		}

		if (m_EncapMagnusPam.bEnable && (nCurFOV == 1 || nCurFOV==4) && nCurDoc==1)
		{ 


			// get Two Point Encap Location////////

			int nThresh_Min_magnus = m_TrainingData.nThreshMin_EncapManus;
			int nThresh_Max_magnus = m_TrainingData.nThreshMax_EncapManus;
			int nArea_Object_magnus = m_TrainingData.nArea_Object_EncapManus;
			int nHeight_Object_magnus = m_TrainingData.nHeight_Object_magnus;
			int nWidth_Object_magnus = m_TrainingData.nWidth_Object_magnus;
			HRegion hThreshold_Region_magnus;
			HObject hDeviceLocation_magnus, hSelect_DeviceLocation_magnus;
			HTuple hDefectInfo_magnus;

			Threshold(hImage, &hThreshold_Region_magnus, nThresh_Min_magnus, nThresh_Max_magnus);
			Connection(hThreshold_Region_magnus, &hDeviceLocation_magnus);
			//SelectShape(hDeviceLocation_magnus, &hSelect_DeviceLocation_magnus, "area", "and", nArea_Object_magnus, 9999999);
			SelectShape(hDeviceLocation_magnus, &hSelect_DeviceLocation_magnus, "height", "and", nHeight_Object_magnus, nHeight_Object_magnus+400);
			SelectShape(hSelect_DeviceLocation_magnus, &hSelect_DeviceLocation_magnus, "width", "and", nWidth_Object_magnus, nWidth_Object_magnus+400);

			HRegion hSelect_DeviceLocation_obj_magnus;
			SelectObj(hSelect_DeviceLocation_magnus, &hSelect_DeviceLocation_obj_magnus, 1);

			HRegion hBoundary_DeviceLocation_magnus;
			Boundary(hSelect_DeviceLocation_magnus, &hBoundary_DeviceLocation_magnus, "inner");
			HTuple hRows_Device_magnus, hCols_Device_magnus;
			GetRegionPoints(hBoundary_DeviceLocation_magnus, &hRows_Device_magnus, &hCols_Device_magnus);

			HTuple  nbottom_DeviceLocationEncap_magnus,
				ntop_DeviceLocationEncap_magnus,
				nright_DeviceLocationEncap_magnus,
				nleft_DeviceLocationEncap_magnus;
			TupleMax(hRows_Device_magnus, &nbottom_DeviceLocationEncap_magnus);
			TupleMin(hRows_Device_magnus, &ntop_DeviceLocationEncap_magnus);
			TupleMax(hCols_Device_magnus, &nright_DeviceLocationEncap_magnus);
			TupleMin(hCols_Device_magnus, &nleft_DeviceLocationEncap_magnus);

			HTuple nImage_Width, nImage_Height;
			GetImageSize(hImage, &nImage_Width, &nImage_Height);

			//////////// Get rectangle encap bottom//////////////////
			CRect hRect_EncapLocation_magnus(100, 100, 400, 400);
			CRect hRect_CropSmoothEncap_magnus(100, 100, 400, 400);
			CRect hRect_CropNoSmoothEncap_magnus(100, 100, 400, 400);
			CRect hRect_CropRemoveBlackLine_magnus(100, 100, 400, 400);


			if (nCurFOV == 1)// top
			{
				if ((int)nleft_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandLeft_magnus[0] > 0)
					hRect_EncapLocation_magnus.left = (int)nleft_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandLeft_magnus[0];
				else hRect_EncapLocation_magnus.left = 0;
				
				if ((int)nright_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandRight_magnus[0] < nImage_Width)
					hRect_EncapLocation_magnus.right = (int)nright_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandRight_magnus[0];
				else hRect_EncapLocation_magnus.right = (int)nImage_Width;

				hRect_EncapLocation_magnus.bottom = (int)ntop_DeviceLocationEncap_magnus;

				if (ntop_DeviceLocationEncap_magnus - m_TrainingData.nCrop_ExpandHeight_magnus[0] > 0
					&& ntop_DeviceLocationEncap_magnus - m_TrainingData.nCrop_ExpandHeight_magnus[0] < (int)nImage_Height)
					hRect_EncapLocation_magnus.top = (int)ntop_DeviceLocationEncap_magnus - m_TrainingData.nCrop_ExpandHeight_magnus[0];
				else hRect_EncapLocation_magnus.top = 0;

				hRect_CropSmoothEncap_magnus = hRect_EncapLocation_magnus;
				hRect_CropNoSmoothEncap_magnus = hRect_EncapLocation_magnus;
				hRect_CropSmoothEncap_magnus.top = hRect_EncapLocation_magnus.bottom - m_TrainingData.nCrop_Smooth_EncapManus[0];
				hRect_CropNoSmoothEncap_magnus.bottom = hRect_EncapLocation_magnus.bottom - m_TrainingData.nCrop_Smooth_EncapManus[0];
			
				hRect_CropRemoveBlackLine_magnus.top = hRect_EncapLocation_magnus.top;
				hRect_CropRemoveBlackLine_magnus.bottom = hRect_EncapLocation_magnus.top + m_TrainingData.nCrop_RemoveBLHeight_magnus[0];
				hRect_CropRemoveBlackLine_magnus.left = (int)nleft_DeviceLocationEncap_magnus + m_TrainingData.nCrop_RemoveBLLeft_magnus[0];
				hRect_CropRemoveBlackLine_magnus.right = (int)nright_DeviceLocationEncap_magnus + m_TrainingData.nCrop_RemoveBLRight_magnus[0];
			
			}
			else if (nCurFOV == 4)// bottom
			{
				if ((int)nleft_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandLeft_magnus[1] > 0)
					hRect_EncapLocation_magnus.left = (int)nleft_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandLeft_magnus[1];
				else hRect_EncapLocation_magnus.left = 0;
				
				if ((int)nright_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandRight_magnus[1] < nImage_Width)
					hRect_EncapLocation_magnus.right = (int)nright_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandRight_magnus[1];
				else hRect_EncapLocation_magnus.right = (int)nImage_Width;

				hRect_EncapLocation_magnus.top = (int)nbottom_DeviceLocationEncap_magnus;

				if ((int)ntop_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandHeight_magnus[1] < nImage_Height &&
					(int)ntop_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandHeight_magnus[1]>0)
					hRect_EncapLocation_magnus.bottom = (int)nbottom_DeviceLocationEncap_magnus + m_TrainingData.nCrop_ExpandHeight_magnus[1];
				else hRect_EncapLocation_magnus.bottom = (int)nImage_Height;

				hRect_CropSmoothEncap_magnus = hRect_EncapLocation_magnus;
				hRect_CropNoSmoothEncap_magnus = hRect_EncapLocation_magnus;
				hRect_CropSmoothEncap_magnus.bottom = hRect_EncapLocation_magnus.top + m_TrainingData.nCrop_Smooth_EncapManus[1];
				hRect_CropNoSmoothEncap_magnus.top =  hRect_EncapLocation_magnus.top + m_TrainingData.nCrop_Smooth_EncapManus[1];
				
				hRect_CropRemoveBlackLine_magnus.bottom = hRect_EncapLocation_magnus.bottom;
				hRect_CropRemoveBlackLine_magnus.top = hRect_EncapLocation_magnus.bottom - m_TrainingData.nCrop_RemoveBLHeight_magnus[1];
				hRect_CropRemoveBlackLine_magnus.left = (int)nleft_DeviceLocationEncap_magnus + m_TrainingData.nCrop_RemoveBLLeft_magnus[1];
				hRect_CropRemoveBlackLine_magnus.right = (int)nright_DeviceLocationEncap_magnus + m_TrainingData.nCrop_RemoveBLRight_magnus[1];
			
			}
			HRegion hRectangle_DeviceLocation_magnus;
			GenRectangle1(&hRectangle_DeviceLocation_magnus,
				hRect_EncapLocation_magnus.top,
				hRect_EncapLocation_magnus.left,
				hRect_EncapLocation_magnus.bottom,
				hRect_EncapLocation_magnus.right);
			HImage hCrop_image_magnus;
			ReduceDomain(hImage, hRectangle_DeviceLocation_magnus, &hCrop_image_magnus);

			/////////// Threshold white and black region///////////////

			HRegion hBlack_Region_magnus, hWhite_Region_magnus;
			int nThreshMin_Black_magnus = m_TrainingData.nThreshMin_Black_EncapManus;
			int nThreshMax_Black_magnus = m_TrainingData.nThreshMax_Black_EncapManus;
			int nThreshMin_White_magnus = m_TrainingData.nThreshMin_White_EncapManus;
			int nThreshMax_White_magnus = m_TrainingData.nThreshMax_White_EncapManus;
			Threshold(hCrop_image_magnus, &hBlack_Region_magnus, nThreshMin_Black_magnus, nThreshMax_Black_magnus);
			Threshold(hCrop_image_magnus, &hWhite_Region_magnus, nThreshMin_White_magnus, nThreshMax_White_magnus);

			/////////// Dilate White Region to remove intersection line between black and white region ///////////

			HRegion hDilation_WhiteRegion_magnus;
			int nX_Dilation_White_magnus = m_TrainingData.nDilateX_EncapManus;
			int nY_Dilation_White_magnus = m_TrainingData.nDilateY_EncapManus;
			DilationRectangle1(hWhite_Region_magnus, &hDilation_WhiteRegion_magnus, nX_Dilation_White_magnus, nY_Dilation_White_magnus);

			/////////// Union White and Black region//////////////
			
			HRegion hRegion_CropRemoveBlackLine_magnus;
			GenRectangle1(&hRegion_CropRemoveBlackLine_magnus,
				hRect_CropRemoveBlackLine_magnus.top,
				hRect_CropRemoveBlackLine_magnus.left,
				hRect_CropRemoveBlackLine_magnus.bottom,
				hRect_CropRemoveBlackLine_magnus.right);
			HRegion hUnionRegion_WhiteBlack_magnus, hUnionRegion_WhiteBlackBL_magnus;
			Union2(hBlack_Region_magnus, hDilation_WhiteRegion_magnus, &hUnionRegion_WhiteBlack_magnus);
			Union2(hUnionRegion_WhiteBlack_magnus, hRegion_CropRemoveBlackLine_magnus, &hUnionRegion_WhiteBlackBL_magnus);

			//int nShapeAreaRemove_magnus = m_TrainingData.nArea_Object_EncapManus;
			//HRegion hSelectRegion_BW_magnus;
			//SelectShape(hUnionRegion_WhiteBlack_magnus, &hSelectRegion_BW_magnus, "width", "and", 400, 99999);
			//HRegion hFillupRegion_BW_magnus;
			//FillUp(hUnionRegion_WhiteBlack_magnus, &hFillupRegion_BW_magnus);
			//

			////////// Remove White and black region /////////////
			HRegion hDiffRegion_magnus;
			Difference(hRectangle_DeviceLocation_magnus, hUnionRegion_WhiteBlackBL_magnus, &hDiffRegion_magnus);
			HRegion hFillupRegion_diff_magnus;
			FillUp(hDiffRegion_magnus, &hFillupRegion_diff_magnus);
			HRegion hRegionOpening_fill_magnus;

			//////////////Remove top black line ( filter)

			int nX_Opening_fill_magnus = m_TrainingData.nOpeningX_EncapManus;
			int nY_Opening_fill_magnus = m_TrainingData.nOpeningY_EncapManus;
			OpeningRectangle1(hFillupRegion_diff_magnus, &hRegionOpening_fill_magnus, nX_Opening_fill_magnus, nY_Opening_fill_magnus);

			///////////////////////////////// Select shape////////

			HRegion hConnection_opening_magnus, hRegionOpening_Select_magnus, hRegionOpening_Circle_magnus;
			int nValueOpening_Circle_magnus = m_TrainingData.nValue_OpeningCircle_magnus;
			Connection(hRegionOpening_fill_magnus, &hConnection_opening_magnus);
			SelectShape(hConnection_opening_magnus, &hRegionOpening_Select_magnus, "width", "and", 200, 9999);
			OpeningCircle(hRegionOpening_Select_magnus, &hRegionOpening_Circle_magnus, nValueOpening_Circle_magnus);

			//////////////////// gen Crop smooth contour to intersection/////////////
			HXLD hContour_Encap_magnus, hCropContour_After_magnus;
			HXLD hCropContour_BeforeSmooth_magnus;

			/// Opening circle before smooth
			HRegion hRegionOpening_CircleCrop_magnus;
			OpeningCircle(hRegionOpening_Circle_magnus, &hRegionOpening_CircleCrop_magnus, m_TrainingData.nValue_OpeningCircleCrop_magnus);
			// Crop contour to smooth
			int nValueSmooth_Crop_magnus = m_TrainingData.nValue_Smooth_EncapMagnus;
			GenContourRegionXld(hRegionOpening_CircleCrop_magnus, &hContour_Encap_magnus, "center");
			// Smooth
			//int nValueSmooth_Crop_magnus = m_TrainingData.nValue_Smooth_EncapMagnus;
			//GenContourRegionXld(hRegionOpening_Circle_magnus, &hContour_Encap_magnus, "center");
			//CRect hRect_CropSmoothEncap_magnus = m_TrainingData.hRect_CropSmoothEncap_magnus;
			CropContoursXld(hContour_Encap_magnus, &hCropContour_BeforeSmooth_magnus,
				hRect_CropSmoothEncap_magnus.top,
				hRect_CropSmoothEncap_magnus.left,
				hRect_CropSmoothEncap_magnus.bottom,
				hRect_CropSmoothEncap_magnus.right, "true");

			SmoothContoursXld(hContour_Encap_magnus, &hCropContour_After_magnus, 2 * nValueSmooth_Crop_magnus + 1);
			/////// Gen region smooth
			HXLD hRegionContour_Smooth_magnus;
			GenRegionContourXld(hCropContour_After_magnus, &hRegionContour_Smooth_magnus, "filled");
			HRegion hRegionInter_CropSmooth_magnus;
			Intersection(hRegionContour_Smooth_magnus, hRegionOpening_Circle_magnus, &hRegionInter_CropSmooth_magnus);

			//////////////////////// Get region no smooth  //////////

			HRegion hRegionRectangle_CropNoSmooth_magnus;
			//	CRect hRect_CropNoSmoothEncap_magnus = m_TrainingData.hRect_CropNoSmoothEncap_magnus;
			GenRectangle1(&hRegionRectangle_CropNoSmooth_magnus,
				hRect_CropNoSmoothEncap_magnus.top,
				hRect_CropNoSmoothEncap_magnus.left,
				hRect_CropNoSmoothEncap_magnus.bottom,
				hRect_CropNoSmoothEncap_magnus.right);
			HRegion hRegionInter_Nosmooth_magnus, hRegionUnion_2RegionSmooth_magnus, hRegionBoundary_2Region_Smooth_magnus;
			Intersection(hRegionOpening_Circle_magnus, hRegionRectangle_CropNoSmooth_magnus, &hRegionInter_Nosmooth_magnus);

			//////////// Union Smooth and NoSmooth Region

			Union2(hRegionInter_Nosmooth_magnus, hRegionInter_CropSmooth_magnus, &hRegionUnion_2RegionSmooth_magnus);
			OpeningCircle(hRegionUnion_2RegionSmooth_magnus, &hRegionUnion_2RegionSmooth_magnus, nValueOpening_Circle_magnus);
			Boundary(hRegionUnion_2RegionSmooth_magnus, &hRegionBoundary_2Region_Smooth_magnus, "inner");
			m_arrayOverlayInspection.Add(hRegionUnion_2RegionSmooth_magnus, colorYellow);
		
		}
	}

	return GOOD_UNIT;
	
}

int CInspectionCore::PVIDefectsOverlay(HImage hPVISubImg, HRegion hPVIRgn, CString strPVIDefectName, COLORREF PVIcolor)
{
//	m_arrayOverlayPVIInsp.RemoveAll();
	m_arrayOverlayPVIInsp.Add(hPVISubImg, hPVIRgn, PVIcolor, strPVIDefectName);

	return 0;
}


void CInspectionCore::StepDebug(HImage hImage, HRegion hDebugRgn, UINT color, HTuple hDebugMsg,int nRegion)
{
	CString strTemp, strDebugMsgTemp = "";
	int nPos;
	int nCOunt = hDebugRgn.CountObj();
	for(int i=0; i<hDebugRgn.CountObj(); i++) {
		strTemp = hDebugMsg[i].S();
		nPos = strTemp.Find(':')+1;
		strDebugMsgTemp = strTemp.Mid(nPos);
		strTemp = strTemp.GetAt(0);

		//if(strTemp == '1'
		m_arrayOverlay[nRegion].Add(hImage.SelectObj(i+1),hDebugRgn.SelectObj(i+1), color, strDebugMsgTemp);
		/*else if(strTemp == '2')
			m_arrayOverlay[DARK_FIELD].Add(hDebugRgn.SelectObj(i+1), color, strDebugMsgTemp);
		else if(strTemp == '3')
			m_arrayOverlay[SINGLE_SPOT].Add(hDebugRgn.SelectObj(i+1), color, strDebugMsgTemp);*/
	}
}


void CInspectionCore::ZoomImage(HImage hImgSrc, HImage hImgDst, int nWidth, int nHeight,int nDoc)
{
	HRegion hRgn;
	hRgn.GenEmptyObj();

	ZoomImageSize(hImgSrc, &hImgDst, nWidth, nHeight, "nearest_neighbor");

	// m_arrayOverlay[nDoc].hImage = hImgDst;
	/*m_arrayOverlay[nDoc].RemoveAll();
	m_arrayOverlay[nDoc].Add(hImgDst, hRgn, RGB(255,0,0), "");*/
}


int CInspectionCore::Inspect(int nThreadIndex, HTuple& bPass)
{
	THREAD_PARM* pThreadParm = &m_ThreadParms[nThreadIndex];

	CString strInspResult;
	CPerformanceTimer timer;
	COLORREF colorRed = RGB(255, 0, 0);
	
	
	
	//bPass = TRUE;
	//try {
	//			if(nThreadIndex==THREAD_MULTIPLE_LENS) {
	//				//***------------ Mulitple Lens Check ----------------****
	//				if(m_MultipleLensParm.bEnable){
	//					timer.Reset();
	//					HRegion hDebugRegionOutLocation;
	//					HTuple hDebugMessageOutLocation;
	//					HTuple hMinNoOfIntersection = m_MultipleLensParm.nMinNoOfIntersection;
	//					HObject hMultipleLensRegion;
	//					HTuple hAllMinNoOfIntersection;
	//					IR_MultipleLens(m_pShare->hLensEdge, &hMultipleLensRegion, &hDebugRegionOutLocation, 
	//									  hMinNoOfIntersection, m_pShare->nStepDebug, &pThreadParm->bPass, &hDebugMessageOutLocation,&hAllMinNoOfIntersection);

	//					if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_MULTIPLE_LENS])
	//						StepDebug(hDebugRegionOutLocation, colorRed, hDebugMessageOutLocation,THREAD_MULTIPLE_LENS);
	//					
	//					if (!pThreadParm->bPass)
	//					{
	//						bPass = pThreadParm->bPass;
	//						strInspResult.Format("[Multiple Lens] Min No. of Intersection: %0.0f",hAllMinNoOfIntersection.D());
	//						m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);
	//						if(!m_pShare->nStepDebug)
	//							m_arrayOverlay[BRIGHT_FIELD].Add(m_pShare->hLensEdge, m_MultipleLensParm.colorDefect);
	//						return m_MultipleLensParm.nErrorCode;
	//					}

	//					OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear) Multiple Lens is %3.2f", timer.GetElapsedMSec());
	//				}
	//			}
	//			else if(nThreadIndex==THREAD_LENS_CRACK){
	//				//***------------ Lens Crack Check ----------------****//Edge Crack
	//				if(m_LensCrackParm.bEnable)
	//				{
	//					timer.Reset();
	//					HRegion hCrackDefectRegion;
	//					HTuple hMinDefectArea, hMinDefectHeight, hMaxDefectXY;
	//					HTuple hEdgeContrast = m_LensCrackParm.nEdgeContrast;
	//					HTuple hMinCrackArea = m_LensCrackParm.nMinCrackArea;
	//					HTuple hMinCrackHeight = m_LensCrackParm.nMinCrackHeight;
	//					HTuple hMaxXYRatio = m_LensCrackParm.dMaxXYRatio;
	//					HObject hDebugRegionOutLensCrack;
	//					HTuple hDebugMessageOutLensCrack;

	//					IR_LensCrack(m_pShare->hImageLensEdgeRegionBF,m_pShare->hImageLensEdgeRegionDF,&hCrackDefectRegion,&hDebugRegionOutLensCrack,
	//						hEdgeContrast, hMinCrackArea,m_pShare->hRowPolar, m_pShare->hColPolar, m_pShare->hRadiusPolar, hMinCrackHeight, hMaxXYRatio,
	//						m_pShare->hWidth, m_pShare->hHeight,m_pShare->nStepDebug,m_pShare->hStartAnglePolar,m_pShare->hEndAnglePolar,m_pShare->hRadiusStartPolar,
	//						m_pShare->hRadiusEndPolar,m_pShare->hWidthPolar,m_pShare->hHeightPolar,&hMinDefectArea, &hMinDefectHeight,
	//						&hMaxDefectXY, &bPass,&hDebugMessageOutLensCrack);

	//					if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_LENS_CRACK])
	//						StepDebug(hDebugRegionOutLensCrack, colorRed, hDebugMessageOutLensCrack,THREAD_LENS_CRACK);

	//					if(!bPass)
	//					{
	//						strInspResult.Format("[Crack] Min Area: %0.0f (%0.00fum2)", hMinDefectArea.D(), hMinDefectArea.D()*m_dPixelSize*m_dPixelSize);
	//						m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);
	//						strInspResult.Format("[Crack] Min Height: %0.0f (%0.00fum)", hMinDefectHeight.D(), hMinDefectHeight.D()*m_dPixelSize);
	//						m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);
	//						strInspResult.Format("[Crack] Max XY Ratio: %0.0f", hMaxDefectXY.D());
	//						m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);
	//						if(!m_pShare->nStepDebug)
	//							m_arrayOverlay[BRIGHT_FIELD].Add(hCrackDefectRegion,  m_LensCrackParm.colorDefect);
	//						if(!m_pShare->bInspectAll)
	//							return m_LensCrackParm.nErrorCode;
	//						else
	//							m_LensCrackParm.nError = m_LensCrackParm.nErrorCode;
	//					}
	//					OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear) LensCrack is %3.2f", timer.GetElapsedMSec());
	//				}

	//				//-----Ends Lens Crack Check-------
	//			}
	//			else if(nThreadIndex==THREAD_TORN_EDGE){
	//					
	//				if(m_TornEdgeParam.bEnable)
	//					{
	//						if(m_LensLocationParam.bIsEType) {
	//							timer.Reset();
	//							HTuple hDefectMean, hDefectDev, hDefectGapWidth;
	//							HTuple hMinEdgeThresholdDF = m_TornEdgeParam.nMinEdgeThresholdDF;
	//							HTuple hMinGapWidth = m_TornEdgeParam.nMinGapWidth;
	//							HTuple hMaxMean = m_TornEdgeParam.nMaxMean;
	//							HTuple hMaxDeviation = m_TornEdgeParam.nMaxDeviation;

	//							HTuple hDefectHeight;
	//							HTuple hMaxEdgeThresholdBF = m_TornEdgeParam.nMaxEdgeThresholdBF;
	//							HTuple hMinTornLength= m_TornEdgeParam.nMinTornLength;

	//							HRegion hTornDefectRegion;

	//							IR_EdgeTorn_E(m_pShare->hImageLensEdgeRegionBF, &hTornDefectRegion, hMaxEdgeThresholdBF, m_pShare->hRowPolar, m_pShare->hColPolar,
	//								m_pShare->hStartAnglePolar, m_pShare->hEndAnglePolar, m_pShare->hRadiusStartPolar, m_pShare->hRadiusEndPolar, m_pShare->hWidthPolar, m_pShare->hHeightPolar,
	//								hMinTornLength, m_pShare->hWidth, m_pShare->hHeight, &hDefectHeight, &bPass);

	//							if(!bPass)
	//							{
	//								strInspResult.Format("[Torn] Height: %0.0f (%0.00fum)", hDefectHeight.D(), hDefectHeight.D()*m_dPixelSize);
	//								m_strInspectionResult[DARK_FIELD].Add(strInspResult);
	//								m_arrayOverlay[DARK_FIELD].Add(hTornDefectRegion, m_TornEdgeParam.colorDefect);
	//								if(!m_pShare->bInspectAll)
	//									return m_TornEdgeParam.nErrorCode;
	//								else
	//									m_TornEdgeParam.nError = m_TornEdgeParam.nErrorCode;
	//							}
	//							OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear) Torn Edge(Thick Type) is %3.2f", timer.GetElapsedMSec());
	//						}
	//						else
	//						{
	//							timer.Reset();
	//							HTuple hTornEdgeMinThicknessBF, hSegmentCheckSize, hTornEdgeMinDeviationBF;
	//							HTuple hTornEdgeMinThicknessDF, hTornEdgeMinDeviationDF;

	//							hTornEdgeMinThicknessBF = m_TornEdgeParam.nTornEdgeMinThicknessBF;
	//							hSegmentCheckSize = m_TornEdgeParam.nSegmentCheckSize;
	//							hTornEdgeMinDeviationBF = m_TornEdgeParam.dTornEdgeMinDeviationBF;
	//							HTuple hAllTornEdgeMinDeviationBF,hAllTornEdgeMinDeviationDF;

	//							hTornEdgeMinThicknessDF = m_TornEdgeParam.nTornEdgeMinThicknessDF;
	//							hTornEdgeMinDeviationDF = m_TornEdgeParam.dTornEdgeMinDeviationDF;

	//							HRegion hTornDefectRegion, hDebugRegionOutEdgeTorn,hTornDefectRegionBF,hTornDefectRegionDF;
	//							HTuple hDebugMessageOutEdgeTorn;
	//							HTuple hTotalTorn;

	//							IR_EdgeTorn(m_pShare->hDarkRegionisPTInBF, m_pShare->hImageLensEdgeRegionBF, m_pShare->hLensEdgeRegionInBF, m_pShare->hLensEdgeRegion, 
	//								&hTornDefectRegionBF, &hDebugRegionOutEdgeTorn, hTornEdgeMinThicknessBF, 
	//								hSegmentCheckSize, hTornEdgeMinDeviationBF, m_pShare->hRowPolar, m_pShare->hColPolar, m_pShare->hStartAnglePolar, 
	//								m_pShare->hEndAnglePolar, m_pShare->hRadiusStartPolar, m_pShare->hRadiusEndPolar, m_pShare->hWidthPolar, m_pShare->hHeightPolar, 
	//								m_pShare->hWidth, m_pShare->hHeight,m_pShare->nStepDebug, &bPass,&hDebugMessageOutEdgeTorn,&hAllTornEdgeMinDeviationBF);

	//							if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_TORN_EDGE])
	//								StepDebug(hDebugRegionOutEdgeTorn, colorRed, hDebugMessageOutEdgeTorn,THREAD_TORN_EDGE);

	//							IR_EdgeTorn(m_pShare->hBrightRegionInDF, m_pShare->hImageLensEdgeRegionBF, m_pShare->hLensEdgeRegionInBF, 
	//									m_pShare->hLensEdgeRegion, &hTornDefectRegionDF, &hDebugRegionOutEdgeTorn, hTornEdgeMinThicknessDF, 
	//									hSegmentCheckSize, hTornEdgeMinDeviationDF, m_pShare->hRowPolar, m_pShare->hColPolar, 
	//									m_pShare->hStartAnglePolar, m_pShare->hEndAnglePolar, m_pShare->hRadiusStartPolar, m_pShare->hRadiusEndPolar, 
	//									m_pShare->hWidthPolar, m_pShare->hHeightPolar, m_pShare->hWidth, m_pShare->hHeight,m_pShare->nStepDebug, &bPass,&hDebugMessageOutEdgeTorn,&hAllTornEdgeMinDeviationDF);

	//							 Union2(hTornDefectRegionBF, hTornDefectRegionDF, &hTornDefectRegion);
	//							 CountObj(hTornDefectRegion, &hTotalTorn);

	//							/* if (0 != (hTotalTorn>0))
	//								{
	//									bPass = 0;
	//								}*/

	//							if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_TORN_EDGE])
	//								StepDebug(hDebugRegionOutEdgeTorn, colorRed, hDebugMessageOutEdgeTorn,THREAD_TORN_EDGE);


	//							if (!bPass){

	//								strInspResult.Format("[Torn Bright Field] Edge Min Deviation: %0.0f", hAllTornEdgeMinDeviationBF.D());
	//								m_strInspectionResult[DARK_FIELD].Add(strInspResult);
	//								strInspResult.Format("[Torn Dark Field] Edge Min Deviation: %0.0f", hAllTornEdgeMinDeviationDF.D());
	//								m_strInspectionResult[DARK_FIELD].Add(strInspResult);

	//								if(!m_pShare->nStepDebug)
	//									m_arrayOverlay[DARK_FIELD].Add(hTornDefectRegion, m_TornEdgeParam.colorDefect);
	//								if(!m_pShare->bInspectAll)
	//									return m_TornEdgeParam.nErrorCode;
	//								else
	//									m_TornEdgeParam.nError = m_TornEdgeParam.nErrorCode;
	//							}
	//							OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear) Torn Edge is %3.2f", timer.GetElapsedMSec());
	//						}
	//					}
	//				}
	//				else if(nThreadIndex==THREAD_EDGE_ROUGHNESS){
	//					if(m_EdgeRoughnessParam.bEnable) {
	//						if(m_LensLocationParam.bIsEType) {
	//								timer.Reset();
	//								HTuple hMaxDefectLength;
	//								HRegion hEdgeDefectBF, hEdgeDefectDF;
	//								HTuple hInnerInspRegionOffsetBF = m_EdgeRoughnessParam.dInnerInspRegionOffsetBF;
	//								HTuple hInnerInspRegionDistanceBF = m_EdgeRoughnessParam.dInnerInspRegionDistanceBF;
	//								HTuple hInnerEdgeBranchWidthBF = m_EdgeRoughnessParam.nInnerEdgeBranchWidthBF;
	//								HTuple hInnerEdgeBranchContrastBF = m_EdgeRoughnessParam.nInnerEdgeBranchContrastBF;
	//								HTuple hInnerEdgeBracnhMinLengthBF = m_EdgeRoughnessParam.nInnerEdgeBracnhMinLengthBF;

	//								HTuple hOuterInspRegionOffsetBF = m_EdgeRoughnessParam.dOuterInspRegionOffsetBF;
	//								HTuple hOuterInspRegionDistanceBF = m_EdgeRoughnessParam.dOuterInspRegionDistanceBF;
	//								HTuple hOuterEdgeBranchWidthBF = m_EdgeRoughnessParam.nOuterEdgeBranchWidthBF;
	//								HTuple hOuterEdgeBranchContrastBF = m_EdgeRoughnessParam.nOuterEdgeBranchContrastBF;
	//								HTuple hOuterEdgeBracnhMinLengthBF = m_EdgeRoughnessParam.nOuterEdgeBracnhMinLengthBF;

	//								IR_EdgeRoughness_E_BF (m_pShare->hLensEdgeTrueRegionBF, m_pShare->hImageBF, &hEdgeDefectBF, \
	//									hInnerInspRegionOffsetBF, hInnerInspRegionDistanceBF, hInnerEdgeBranchWidthBF, hInnerEdgeBranchContrastBF, hInnerEdgeBracnhMinLengthBF, \
	//									hOuterInspRegionOffsetBF, hOuterInspRegionDistanceBF, hOuterEdgeBranchWidthBF, hOuterEdgeBranchContrastBF, hOuterEdgeBracnhMinLengthBF, \
	//									&hMaxDefectLength, &bPass);
	//
	//								if(!bPass){	
	//									strInspResult.Format("[Rough] Length: %0.0f (%0.00fum)", hMaxDefectLength.D(), hMaxDefectLength.D()*m_dPixelSize);
	//									m_strInspectionResult[DARK_FIELD].Add(strInspResult);
	//									if(!m_pShare->nStepDebug)
	//										m_arrayOverlay[DARK_FIELD].Add(hEdgeDefectBF, m_EdgeRoughnessParam.colorDefect);	
	//									if(!m_pShare->bInspectAll)		
	//										return m_EdgeRoughnessParam.nErrorCode;	
	//									else			
	//										m_EdgeRoughnessParam.nError = m_EdgeRoughnessParam.nErrorCode;
	//								}
	//							
	//								HTuple hInnerInspRegionOffsetDF = m_EdgeRoughnessParam.dInnerInspRegionOffsetDF;
	//								HTuple hInnerInspRegionDistanceDF = m_EdgeRoughnessParam.dInnerInspRegionDistanceDF;
	//								HTuple hInnerEdgeBranchMinThreshold = m_EdgeRoughnessParam.nInnerEdgeBranchMinThresholdDF;
	//								HTuple hInnerEdgeBracnhMinLength = m_EdgeRoughnessParam.nInnerEdgeBracnhMinLengthDF;

	//								HTuple hOuterInspRegionOffsetDF = m_EdgeRoughnessParam.dOuterInspRegionOffsetDF;
	//								HTuple hOuterInspRegionDistanceDF = m_EdgeRoughnessParam.dOuterInspRegionDistanceDF;
	//								HTuple hOuterEdgeBranchMinThreshold = m_EdgeRoughnessParam.nOuterEdgeBranchMinThresholdDF;
	//								HTuple hOuterEdgeBracnhMinLength = m_EdgeRoughnessParam.nOuterEdgeBracnhMinLengthDF;
	//								HTuple hMinEdgeBranchHeight = m_EdgeRoughnessParam.nMinEdgeBranchHeight;

	//								IR_EdgeRoughness_E_DF (m_pShare->hLensEdgeTrueRegionDF, m_pShare->hImageDF, &hEdgeDefectDF,\
	//									hInnerInspRegionOffsetDF, hInnerInspRegionDistanceDF, hInnerEdgeBranchMinThreshold, hInnerEdgeBracnhMinLength,\
	//									hOuterInspRegionOffsetDF, hOuterInspRegionDistanceDF, hOuterEdgeBranchMinThreshold, hOuterEdgeBracnhMinLength,\
	//									&hMaxDefectLength, &bPass);
	//								if(!bPass){	
	//									IR_RecheckEdgeRoughness_E_DF(hEdgeDefectDF, &hEdgeDefectDF, hMinEdgeBranchHeight, m_pShare->hRowPolar, m_pShare->hColPolar, m_pShare->hRadiusPolar, \
	//										m_pShare->hWidth, m_pShare->hHeight, &hMaxDefectLength, &bPass);
	//									if(!bPass){	
	//										strInspResult.Format("[Rough] Length: %0.0f (%0.00fum)", hMaxDefectLength.D(), hMaxDefectLength.D()*m_dPixelSize);
	//										m_strInspectionResult[DARK_FIELD].Add(strInspResult);
	//										if(!m_pShare->nStepDebug)
	//											m_arrayOverlay[DARK_FIELD].Add(hEdgeDefectDF, m_EdgeRoughnessParam.colorDefect);	
	//										if(!m_pShare->bInspectAll)		
	//											return m_EdgeRoughnessParam.nErrorCode;	
	//										else			
	//											m_EdgeRoughnessParam.nError = m_EdgeRoughnessParam.nErrorCode;
	//									}
	//									else
	//									{
	//										strInspResult.Format("[Rough] Recheck Pass");
	//										m_strInspectionResult[DARK_FIELD].Add(strInspResult);
	//									}
	//								}
	//								OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear) Edge Roughness(Thick Type) is %3.2f", timer.GetElapsedMSec());
	//						}
	//						else
	//						{
	//							timer.Reset();
	//							HTuple hMinRoughEdgeLength = m_EdgeRoughnessParam.nMinRoughEdgeLength;

	//							//** Advance Parameter ***
	//							HTuple hAddMedianOffset = m_EdgeRoughnessParam.nAddMedianOffset;  //Not to give to vision parameter

	//							HRegion hRoughEdgeDefectRegion, hDebugRegionOutRoughness;
	//							HTuple hDebugMessageOutRoughness,hAllMinRoughEdgeLength;
	//
	//							IR_EdgeRoughness(m_pShare->hLensInnerRegion, m_pShare->hImageLensEdgeRegionDF, m_pShare->hLensEdgeRegionInBF,m_pShare->hAllObjectForRoughness,
	//								m_pShare->hBrightRegionInDFRoughness,m_pShare->hAllObjectForRoughnessPolar,&hRoughEdgeDefectRegion,&hDebugRegionOutRoughness,
	//								hMinRoughEdgeLength,hAddMedianOffset,m_pShare->nStepDebug, &bPass,&hDebugMessageOutRoughness,&hAllMinRoughEdgeLength);
	//
	//							if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_EDGE_ROUGHNESS])		
	//								StepDebug(hDebugRegionOutRoughness, colorRed, hDebugMessageOutRoughness,THREAD_EDGE_ROUGHNESS);
	//
	//							if(!bPass){	
	//								IR_RecheckEdgeRoughness(hRoughEdgeDefectRegion, m_pShare->hImageBF, &hRoughEdgeDefectRegion, 
	//									&bPass);
	//								if(!bPass){	
	//									strInspResult.Format("[Edge Roughness]  Min Length: %0.0f", hAllMinRoughEdgeLength.D());
	//									m_strInspectionResult[DARK_FIELD].Add(strInspResult);
	//									if(!m_pShare->nStepDebug)
	//										m_arrayOverlay[DARK_FIELD].Add(hRoughEdgeDefectRegion, m_EdgeRoughnessParam.colorDefect);	
	//									if(!m_pShare->bInspectAll)		
	//										return m_EdgeRoughnessParam.nErrorCode;	
	//									else			
	//										m_EdgeRoughnessParam.nError = m_EdgeRoughnessParam.nErrorCode;
	//								}
	//							}	
	//							OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear) Edge RoughNess is %3.2f", timer.GetElapsedMSec());
	//						}
	//					}
	//				}
	//				else if(nThreadIndex==THREAD_EDGE_FIBER){
	//					if(m_EdgeFiberParm.bEnable && !m_LensLocationParam.bIsEType) {
	//						timer.Reset();
	//						HTuple hMinEdgeFiberHeight = m_EdgeFiberParm.nMinEdgeFiberHeight;
	//						HTuple hMinEdgeFiberContrastDF = m_EdgeFiberParm.nMinEdgeFiberContrastDF;
	//						HTuple hMaxEdgeFiberContrastDF = m_EdgeFiberParm.nMaxEdgeFiberContrastDF;
	//						HObject hFiberUnion;//h
	//						HRegion hDebugRegionOutFiber;
	//						HTuple hDebugMessageOutFiber;

	//						Edge_Fiber(m_pShare->hImageLensEdgePT_DF, &hFiberUnion,&hDebugRegionOutFiber, hMinEdgeFiberHeight, 
	//							hMinEdgeFiberContrastDF, hMaxEdgeFiberContrastDF, m_pShare->hRowPolar, m_pShare->hColPolar, 
	//							m_pShare->hStartAnglePolar, m_pShare->hEndAnglePolar, m_pShare->hRadiusStartPolar,m_pShare->hRadiusEndPolar, 
	//							m_pShare->hWidthPolar, m_pShare->hHeightPolar, m_pShare->hWidth, m_pShare->hHeight,m_pShare->nStepDebug,&bPass,&hDebugMessageOutFiber);

	//						if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_EDGE_FIBER])		
	//								StepDebug(hDebugRegionOutFiber, colorRed, hDebugMessageOutFiber,THREAD_EDGE_FIBER);

	//						if (!bPass)
	//						{
	//						//	m_strInspectionResult[DARK_FIELD].Add(strInspResult);
	//							if(!m_pShare->nStepDebug)
	//								m_arrayOverlay[DARK_FIELD].Add(hFiberUnion, m_EdgeFiberParm.colorDefect);	
	//							if(!m_pShare->bInspectAll)		
	//								return m_EdgeFiberParm.nErrorCode;	
	//							else			
	//								m_EdgeFiberParm.nError = m_EdgeFiberParm.nErrorCode;
	//						}
	//						OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear)Edge Fiber is %3.2f", timer.GetElapsedMSec());
	//					}
	//				}

	//				else if(nThreadIndex==THREAD_EDGE_TEAR){
	//					if(m_EdgeTearParm.bEnable && !m_LensLocationParam.bIsEType){
	//						timer.Reset();
	//						HTuple hTearEdgeMinDeviation = m_EdgeTearParm.dTearEdgeMinDeviation;
	//						HTuple hTearMinHeight = m_EdgeTearParm.nTearMinHeight;
	//						HRegion hTearDefectRegionDF,hRegionIntersection,hConnectedRegions,hUnionTear;
	//						HRegion hTearDefectRegionBF,hTearDefectRegion;

	//						HObject hDebugTearDefectRegionDF;
	//						HTuple hDebugTearDefectMessageOutDF;

	//						HObject hDebugTearDefectRegionBF;
	//						HTuple hDebugTearDefectMessageOutBF;

	//						HTuple hAllTearEdgeMinDeviation,hInterCount,hTotalTear;

	//						 IR_EdgeTear(m_pShare->hBrightRegionInDF, m_pShare->hLensEdge, &hTearDefectRegionDF, &hDebugTearDefectRegionDF, 
	//								m_pShare->nStepDebug, m_pShare->hRowPolar, m_pShare->hColPolar, m_pShare->hStartAnglePolar, m_pShare->hEndAnglePolar, 
	//								m_pShare->hRadiusStartPolar, m_pShare->hRadiusEndPolar, m_pShare->hWidthPolar, m_pShare->hHeightPolar, m_pShare->hWidth, 
	//								m_pShare->hHeight, hTearMinHeight, hTearEdgeMinDeviation, &hDebugTearDefectMessageOutDF, 
	//								&bPass, &hAllTearEdgeMinDeviation);


	//						if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_EDGE_TEAR])
	//							StepDebug(hDebugTearDefectRegionDF, colorRed, hDebugTearDefectMessageOutDF,THREAD_EDGE_TEAR);

	//						   IR_EdgeTear(m_pShare->hDarkRegionisPTInBF, m_pShare->hLensEdgeTrueRegionBF, &hTearDefectRegionBF, 
	//									&hDebugTearDefectRegionBF, m_pShare->nStepDebug, m_pShare->hRowPolar, m_pShare->hColPolar, m_pShare->hStartAnglePolar, 
	//									m_pShare->hEndAnglePolar, m_pShare->hRadiusStartPolar, m_pShare->hRadiusEndPolar, m_pShare->hWidthPolar, 
	//									m_pShare->hHeightPolar, m_pShare->hWidth, m_pShare->hHeight, hTearMinHeight, hTearEdgeMinDeviation, 
	//									&hDebugTearDefectMessageOutBF, &bPass, &hAllTearEdgeMinDeviation);

	//						   if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_EDGE_TEAR])
	//							StepDebug(hDebugTearDefectRegionBF, colorRed, hDebugTearDefectMessageOutBF,THREAD_EDGE_TEAR);

	//						   Intersection(hTearDefectRegionBF, hTearDefectRegionDF, &hRegionIntersection);
	//							CountObj(hRegionIntersection, &hInterCount);

	//							if (0 != (hInterCount>0))
	//							{
	//							  Union2(hTearDefectRegionDF, hTearDefectRegionBF, &hTearDefectRegion);
	//							  Connection(hTearDefectRegion, &hConnectedRegions);
	//							  Union1(hConnectedRegions, &hUnionTear);
	//							  Connection(hUnionTear, &hTearDefectRegion);
	//							}
	//							else
	//							{

	//							  Union2(hTearDefectRegionDF, hTearDefectRegionBF, &hTearDefectRegion);

	//							}

	//							CountObj(hTearDefectRegion, &hTotalTear);

	//							if (0 != (hTotalTear>0))
	//							{
	//							  bPass = 0;
	//							}
	//							if (!bPass)
	//							{
	//								HTuple  hRow1, hColumn1, hPhi1, hLength11,hLength21;
	//								 SmallestRectangle2(hTearDefectRegion, &hRow1, &hColumn1, &hPhi1, &hLength11,&hLength21);
	//							     GenRectangle2(&hTearDefectRegion, hRow1, hColumn1, hPhi1, hLength11,hLength21);

	//								strInspResult.Format("[Edge Tear]  Min Deviation: %0.0f", hAllTearEdgeMinDeviation.D());
	//								m_strInspectionResult[DARK_FIELD].Add(strInspResult);
	//								if(!m_pShare->nStepDebug)
	//									m_arrayOverlay[DARK_FIELD].Add(hTearDefectRegion, m_EdgeTearParm.colorDefect);
	//								if(!m_pShare->bInspectAll)
	//									return m_EdgeTearParm.nErrorCode;
	//								else
	//									m_EdgeTearParm.nError = m_EdgeTearParm.nErrorCode;
	//			
	//							}
	//						OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear) Edge Tear is %3.2f", timer.GetElapsedMSec());
	//					}
	//				}
	//				else if(nThreadIndex==THREAD_FOREIGN_PARTICLE){
	//					 //***--------------Foreign Particle Detection------------*******
	//					if(m_ForeignParam.bEnable) {
	//						timer.Reset();
	//						 HRegion hForeignParticleRegion;
	//						HTuple hMaxParticleThresholdBF, hLaplaceContrast, hMinParticleArea, hMaxParticleStructFactor, hMinDefectArea, hMaxDefectSF;

	//						hLaplaceContrast = m_ForeignParam.nLaplaceContrast;
	//						hMinParticleArea = m_ForeignParam.nMinParticleArea;
	//						hMaxParticleStructFactor = m_ForeignParam.dMaxParticleStructFactor;
	//						//hMaxParticleThresholdBF = m_ForeignParam.nMaxParticleThresholdBF;

	//						HObject hDebugRegionForeignParticle;
	//						HTuple hDebugForeignParticleMessage;

	//						IR_ForeignParticle(m_pShare->hImageLensInnerRegionDF,m_pShare->hImageLensInnerRegionBF,&hForeignParticleRegion,&hDebugRegionForeignParticle, 
	//							hLaplaceContrast, hMinParticleArea, hMaxParticleStructFactor,m_pShare->nStepDebug, 
	//							&hMinDefectArea, &hMaxDefectSF, &bPass,&hDebugForeignParticleMessage);

	//						if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_FOREIGN_PARTICLE])
	//							StepDebug(hDebugRegionForeignParticle, colorRed, hDebugForeignParticleMessage,THREAD_FOREIGN_PARTICLE);

	//						if(!bPass){
	//								if(!m_pShare->nStepDebug){
	//									strInspResult.Format("[Foreign] Min Area: %0.0f (%0.00fum2)", hMinDefectArea.D(), hMinDefectArea.D()*m_dPixelSize*m_dPixelSize);
	//									m_strInspectionResult[SINGLE_SPOT].Add(strInspResult);
	//									strInspResult.Format("[Foreign] Max Struct Factor: %0.2f", hMaxDefectSF.D());
	//									m_strInspectionResult[SINGLE_SPOT].Add(strInspResult);
	//									m_arrayOverlay[SINGLE_SPOT].Add(hForeignParticleRegion, m_ForeignParam.colorDefect);
	//								}
	//								if(!m_pShare->bInspectAll)
	//									return m_ForeignParam.nErrorCode;
	//								else
	//									m_ForeignParam.nError = m_ForeignParam.nErrorCode;

	//						}
	//						OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear) Foregin Particle is %3.2f", timer.GetElapsedMSec());
	//					}//End Foregin Particle

	//				}
	//				else if(nThreadIndex==THREAD_SURFACE_SCRATCH){
	//					if(m_SurfaceScratchParm.bEnable)
	//					{
	//						timer.Reset();
	//						HTuple hMinDefectLength, hMaxDefectCircularity;
	//						HTuple hScratchContourLengthSS = m_SurfaceScratchParm.nScratchContourLengthSS;
	//						HTuple hScratchMaxWidth = m_SurfaceScratchParm.nScratchMaxWidth;
	//						HTuple hScratchContrast = m_SurfaceScratchParm.nScratchContrast;
	//						HTuple hMinScratchLengthOZ = m_SurfaceScratchParm.dMinScratchLengthOZ;
	//						HTuple hMaxCirularityOZ = m_SurfaceScratchParm.dMaxCirularityOZ;
	//						//HTuple hScratchInspRegionErosion = m_SurfaceScratchParm.dScratchInspRegionErosion;
	//						HRegion hSurfaceScratchDefectOZRegion;
	//						HRegion hOpticalZoneRegion;
	//						HRegion hRegionDifference;

	//						HObject hDebugRegionSurfaceScratch;
	//						HTuple hDebugForeignSurfaceScratch;

	//						IR_GetScratchInspRegion(m_pShare->hPatternLayerRegion, m_pShare->hLensTrueInnerRegionSS,m_pShare->hPatternRegionForSurfaceInspSS,&hOpticalZoneRegion, 
	//						 m_pShare->bIsPattern);
	//		
	//						if (0 != m_pShare->bIsLocationMark)
	//						{
	//						  Difference(m_pShare->hLensInnerRegion, m_pShare->hLocationMarkRegion, &hRegionDifference);
	//						  ReduceDomain(m_pShare->hImageLensInnerRegionSS, hRegionDifference, &m_pShare->hImageLensInnerRegionSS);
	//						}

	//						IR_SurfaceScratchOZ(m_pShare->hImageLensInnerRegionSS, hOpticalZoneRegion, &hSurfaceScratchDefectOZRegion,&hDebugRegionSurfaceScratch,hScratchContourLengthSS,
	//							hScratchMaxWidth,hScratchContrast, hMinScratchLengthOZ, hMaxCirularityOZ,m_pShare->nStepDebug,
	//							&hMinDefectLength, &hMaxDefectCircularity, &bPass,&hDebugForeignSurfaceScratch);

	//						if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_SURFACE_SCRATCH])
	//							StepDebug(hDebugRegionSurfaceScratch, colorRed, hDebugForeignSurfaceScratch,THREAD_SURFACE_SCRATCH);

	//						if(!bPass)
	//						{
	//							strInspResult.Format("[Surface Scratch] Min Length: %0.0f (%0.00fum)", hMinDefectLength.D(), hMinDefectLength.D()*m_dPixelSize);
	//							m_strInspectionResult[SINGLE_SPOT].Add(strInspResult);
	//							strInspResult.Format("[Surface Scratch] Max Circularity: %0.2f", hMaxDefectCircularity.D());
	//							m_strInspectionResult[SINGLE_SPOT].Add(strInspResult);	

	//							HObject hRegionUnion,hConnectedRegions;

	//							Union1(hSurfaceScratchDefectOZRegion, &hRegionUnion);
	//							HTuple hRow1, hColumn1, hPhi1, hLength1,hLength2;
	//							Connection(hRegionUnion, &hConnectedRegions);
	// 							SmallestRectangle2(hConnectedRegions, &hRow1, &hColumn1, &hPhi1, &hLength1,&hLength2);
	//							GenRectangle2(&hSurfaceScratchDefectOZRegion, hRow1, hColumn1, hPhi1, hLength1,hLength2);

	//							if(!bPass)
	//							{
	//								if(!m_pShare->nStepDebug)
	//									m_arrayOverlay[SINGLE_SPOT].Add(hSurfaceScratchDefectOZRegion, m_SurfaceScratchParm.colorDefect);
	//								if(!m_pShare->bInspectAll)
	//									return m_SurfaceScratchParm.nErrorCode;
	//								else
	//									m_SurfaceScratchParm.nError = m_SurfaceScratchParm.nErrorCode;
	//							}
	//						}

	//					}
	//					OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear) SurFace Scratch is %3.2f", timer.GetElapsedMSec());
	//				}
	//				else if(nThreadIndex==THREAD_MOLD_FLASH){
	//					if(m_MoldFlashParm.bEnable){
	//						timer.Reset();
	//						HTuple hMinMoldFlashDia = m_MoldFlashParm.nMinMoldFlashDia;
	//						HTuple hMinMoldFlashContrast = m_MoldFlashParm.nMinMoldFlashContrast;
	//						HTuple hMinIntensityNonMoldFlashBF = m_MoldFlashParm.nMinIntensityNonMoldFlashBF;

	//						HObject hMoldFlashOverlay;
	//						HRegion hDebugRegionOutMoldFlash;
	//						HTuple hDebugMessageOutMoldFlash;
	//						HTuple hAllMinMoldFlashDia;
	//						MoldFlash(m_pShare->hImageLensInnerRegionSS,m_pShare->hImageLensInnerRegionBF,&hMoldFlashOverlay,&hDebugRegionOutMoldFlash,
	//							hMinMoldFlashContrast, hMinMoldFlashDia,hMinIntensityNonMoldFlashBF,
	//							m_pShare->nStepDebug, &bPass,&hAllMinMoldFlashDia, &hDebugMessageOutMoldFlash);

	//						if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_MOLD_FLASH])		
	//							StepDebug(hDebugRegionOutMoldFlash, colorRed, hDebugMessageOutMoldFlash,THREAD_MOLD_FLASH);

	//						if (!bPass)
	//						{
	//							if(!m_pShare->nStepDebug){
	//								strInspResult.Format("[Mold Flash] Min Diameter: %0.0f", hAllMinMoldFlashDia.D());
	//								m_strInspectionResult[SINGLE_SPOT].Add(strInspResult);
	//				
	//								m_arrayOverlay[SINGLE_SPOT].Add(hMoldFlashOverlay, m_MoldFlashParm.colorDefect);
	//							}
	//							if(!m_pShare->bInspectAll)
	//								return m_MoldFlashParm.nErrorCode;
	//							else
	//								m_MoldFlashParm.nError = m_MoldFlashParm.nErrorCode;
	//						}
	//						OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear) Mold Flash is %3.2f", timer.GetElapsedMSec());
	//					}
	//				}
	//				else if(nThreadIndex==THREAD_SURFACE_DEFECT){
	//					if(m_SurfaceDefectParm.bEnable)
	//					{
	//						timer.Reset();
	//						HTuple hMinSurfaceDefectContrast = m_SurfaceDefectParm.nMinSurfaceDefectContrast;
	//						HTuple hMinSurfaceDefectLength = m_SurfaceDefectParm.nMinSurfaceDefectLength;

	//						HObject hSurfaceDefect;
	//						HRegion hDebugRegionOutSurfaceDefect;
	//						HTuple hDebugMessageOutSurfaceDefect;
	//						HTuple hAllMinSurfaceDefectLength;

	//						SurfaceDefect(m_pShare->hImageLensInnerRegionSS, &hSurfaceDefect, 
	//							&hDebugRegionOutSurfaceDefect, hMinSurfaceDefectContrast, hMinSurfaceDefectLength, 
	//							m_pShare->hWidth,m_pShare->hHeight, m_pShare->nStepDebug,m_pShare->hRowPolar,m_pShare->hColPolar,m_pShare->hStartAnglePolar,
	//							m_pShare->hEndAnglePolar,m_pShare->hRadiusStartPolar,m_pShare->hRadiusEndPolar,m_pShare->hWidthPolar,
	//							m_pShare->hHeightPolar,&bPass,&hAllMinSurfaceDefectLength,&hDebugMessageOutSurfaceDefect);

	//						if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_SURFACE_DEFECT])		
	//								StepDebug(hDebugRegionOutSurfaceDefect, colorRed, hDebugMessageOutSurfaceDefect,THREAD_SURFACE_DEFECT);
	//						if (!bPass)
	//						{
	//							if(!m_pShare->nStepDebug){
	//								strInspResult.Format("[Surface Defect] Min Length: %0.0f", hAllMinSurfaceDefectLength.D());
	//								m_strInspectionResult[SINGLE_SPOT].Add(strInspResult);
	//								m_arrayOverlay[SINGLE_SPOT].Add(hSurfaceDefect, m_SurfaceDefectParm.colorDefect);
	//							}
	//							if(!m_pShare->bInspectAll)
	//								return m_SurfaceDefectParm.nErrorCode;
	//							else
	//								m_SurfaceDefectParm.nError = m_SurfaceDefectParm.nErrorCode;
	//						}
	//						OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear) Surface Defect is %3.2f", timer.GetElapsedMSec());
	//					}
	//				}
	//				else if(nThreadIndex==THREAD_BUBBLE){
	//					if(m_BubbleDefectParam.bEnable) {
	//						timer.Reset();
	//						HRegion hBubbleRegions;
	//						HTuple hMinBubbleCircularity, hMaxBubbleCircularity;

	//						hMinBubbleCircularity = m_BubbleDefectParam.dMinBubbleCircularity;
	//						//hMaxBubbleCircularity = m_BubbleDefectParam.dMaxBubbleCircularity;

	//						HTuple hMaxLensBubbleContrastSS = m_BubbleDefectParam.nMaxLensBubbleContrastSS;
	//						HTuple hMinLensBubbleAreaSS = m_BubbleDefectParam.nMinLensBubbleAreaSS;

	//						HTuple hAllMinLensBubbleAreaSS,hAllMinBubbleCircularity;

	//						HRegion hDebugRegionOutInBubble;
	//						HTuple  hDebugMessageOutInBubble;
	//						IR_BubbleDetection(m_pShare->hImageLensInnerRegionSS,m_pShare->hImageLensInnerRegionBF, &hBubbleRegions,&hDebugRegionOutInBubble, 
	//							hMinBubbleCircularity,hMaxLensBubbleContrastSS,hMinLensBubbleAreaSS,
	//							m_pShare->nStepDebug, &bPass,&hAllMinLensBubbleAreaSS,&hAllMinBubbleCircularity,&hDebugMessageOutInBubble);

	//						if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_BUBBLE])		
	//							StepDebug(hDebugRegionOutInBubble, colorRed, hDebugMessageOutInBubble,THREAD_BUBBLE);

	//						if (!bPass){
	//							if(!m_pShare->nStepDebug){
	//								strInspResult.Format("[Bubble Defect] Min Area: %0.0f ", hAllMinLensBubbleAreaSS.D());
	//								m_strInspectionResult[SINGLE_SPOT].Add(strInspResult);
	//								strInspResult.Format("[Bubble Defect] Min Circularity: %0.0f", hAllMinBubbleCircularity.D());
	//								m_strInspectionResult[SINGLE_SPOT].Add(strInspResult);
	//								m_arrayOverlay[SINGLE_SPOT].Add(hBubbleRegions,  m_BubbleDefectParam.colorDefect);
	//							}
	//							if(!m_pShare->bInspectAll)
	//								return m_BubbleDefectParam.nErrorCode;
	//							else
	//								m_BubbleDefectParam.nError = m_BubbleDefectParam.nErrorCode;
	//						}
	//						OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear) Bubble Defect is %3.2f", timer.GetElapsedMSec());
	//					}
	//				}
	//				else if(nThreadIndex==THREAD_DARK_SPOT_CLEAR){
	//					 if(m_DarkSpotClearParm.bEnable){
	//						 timer.Reset();
	//						 HTuple hDarkSpotDFConMin = m_DarkSpotClearParm.nDarkSpotDFConMin;
	//						 HTuple hDarkSpotDFRealConMin = m_DarkSpotClearParm.nDarkSpotDFRealConMin;
	//						 HTuple hDarkSpotDFRealConMax = m_DarkSpotClearParm.nDarkSpotDFRealConMax;
	//						 HTuple hDarkSpotMinArea = m_DarkSpotClearParm.nDarkSpotMinArea;
	//						 HTuple hMaxDarkSpotConBF = m_DarkSpotClearParm.nMaxDarkSpotConSS;
	//						 HTuple hMinAreaSelectDarkSpotBF = m_DarkSpotClearParm.nMinAreaSelectDarkSpotSS;
	//						 HTuple hMinDiaSelectDarkSpotBF = m_DarkSpotClearParm.nMinDiaSelectDarkSpotSS;

	//						 HObject hDarkSpot,hDarkSpotOverlay;
	//						 HRegion hDebugRegionOutDarkSpotClear;
	//						 HTuple hDebugMessageOutDarkSpotClear;
	//						 HTuple hAllDarkSpotMinAreaDF,hAllMinDiaSelectDarkSpotBF,hALLMinAreaSelectDarkSpotBF;
	//						IR_DarkSpot(m_pShare->hImageLensInnerRegionSS, m_pShare->hImageLensInnerRegionDF, 
	//							&hDarkSpot, &hDarkSpotOverlay, &hDebugRegionOutDarkSpotClear, hDarkSpotDFConMin, 
	//							hDarkSpotDFRealConMin, hDarkSpotDFRealConMax, hDarkSpotMinArea, hMaxDarkSpotConBF, 
	//							hMinAreaSelectDarkSpotBF, hMinDiaSelectDarkSpotBF, m_pShare->nStepDebug, &bPass, 
	//							&hDebugMessageOutDarkSpotClear,&hAllDarkSpotMinAreaDF,&hAllMinDiaSelectDarkSpotBF,&hALLMinAreaSelectDarkSpotBF);

	//						if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_DARK_SPOT_CLEAR])		
	//								StepDebug(hDebugRegionOutDarkSpotClear, colorRed, hDebugMessageOutDarkSpotClear,THREAD_DARK_SPOT_CLEAR);
	//						if (!bPass)
	//						{
	//							 if(!m_pShare->nStepDebug){
	//								strInspResult.Format("[Dark Spot] Min Area(Dark Field): %0.0f", hAllDarkSpotMinAreaDF.D());
	//								m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);
	//								strInspResult.Format("[Dark Spot] Min Diameter(Bright Field): %0.0f", hAllMinDiaSelectDarkSpotBF.D());
	//								m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);
	//								strInspResult.Format("[Dark Spot] Min Area (Bright Field): %0.0f", hALLMinAreaSelectDarkSpotBF.D());
	//								m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);
	//								m_arrayOverlay[BRIGHT_FIELD].Add(hDarkSpotOverlay, m_DarkSpotClearParm.colorDefect);	
	//							 }
	//							if(!m_pShare->bInspectAll)		
	//								return m_DarkSpotClearParm.nErrorCode;	
	//							else			
	//								m_DarkSpotClearParm.nError = m_DarkSpotClearParm.nErrorCode;
	//						}
	//						OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear) Dark Spot Clear is %3.2f", timer.GetElapsedMSec());
	//					 }
	//				}
	//				else if(nThreadIndex==THREAD_HOLE_DEFECT){
	//					//***------------ Hole Defect ----------------****
	//					if(m_HolesDefectParm.bEnable){
	//						 timer.Reset();
	//						 HTuple hMaxHoleEdgeContrastBF = m_HolesDefectParm.nMaxHoleEdgeContrastBF;
	//						 HTuple hMinHoleAreaBF = m_HolesDefectParm.nMinHoleAreaBF;
	//						 HTuple hMinHoleIntensityBF = m_HolesDefectParm.nMinHoleIntensityBF;

	//							HTuple hAllMinHoleAreaBF,hAllMinHoleIntensityBF;
	//							HRegion hHoleRegions,hDebugRegionOutHoleInspection;
	//							HTuple hDebugMessageOutHoleInspection;

	//							IR_HoleInspection(m_pShare->hImageLensInnerRegionBF, &hHoleRegions, &hDebugRegionOutHoleInspection, 
	//								hMaxHoleEdgeContrastBF, hMinHoleAreaBF, hMinHoleIntensityBF,m_pShare->nStepDebug, 
	//								&bPass, &hDebugMessageOutHoleInspection, &hAllMinHoleAreaBF, &hAllMinHoleIntensityBF);

	//							if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_HOLE_DEFECT])		
	//								StepDebug(hDebugRegionOutHoleInspection, colorRed, hDebugMessageOutHoleInspection,THREAD_HOLE_DEFECT);

	//							if (!bPass)
	//							{
	//								 if(!m_pShare->nStepDebug){
	//									strInspResult.Format("[Hole Defect] Min Hole Area(Bright Field): %0.0f", hAllMinHoleAreaBF.D());
	//									m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);
	//									strInspResult.Format("[Hole Defect] Min Hole Intensity(Bright Field): %0.0f", hAllMinHoleIntensityBF.D());
	//									m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);
	//						
	//									m_arrayOverlay[BRIGHT_FIELD].Add(hHoleRegions,  m_HolesDefectParm.colorDefect);	
	//								 }
	//								if(!m_pShare->bInspectAll)		
	//									return m_HolesDefectParm.nErrorCode;	
	//								else			
	//									m_HolesDefectParm.nError = m_HolesDefectParm.nErrorCode;
	//							}
	//					
	//						OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear) Hole Defect is %3.2f", timer.GetElapsedMSec());
	//					}
	//					//End Print Shift
	//				}
	//				else if(nThreadIndex==THREAD_FIBER){
	//					//***------------ Hole Defect ----------------****
	//					if(m_FiberParm.bEnable){
	//						 timer.Reset();
	//						 HTuple hMinFiberIntensityDF = m_FiberParm.nMinFiberIntensityDF;
	//						 HTuple	hMinFiberLengthDF = m_FiberParm.nMinFiberLengthDF;
	//						 HTuple	hMaxFiberIntensityBF = m_FiberParm.nMaxFiberIntensityBF;
	//						 HTuple	hMinFiberLengthBF = m_FiberParm.nMinFiberLengthBF; 
	//								  

	//							HTuple hMinDefectLength;
	//							HRegion hFiberRegion,hDebugRegionOutFiber;
	//							HTuple hDebugMessageOutFiber;

	//							IR_Fiber(m_pShare->hImageLensTrueInnerRegionDF, &hFiberRegion, &hDebugRegionOutFiber, 
	//									  hMinFiberIntensityDF, hMinFiberLengthDF, m_pShare->nStepDebug, &bPass, &hMinDefectLength, 
	//									  &hDebugMessageOutFiber);

	//							if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_FIBER])		
	//								StepDebug(hDebugRegionOutFiber, colorRed, hDebugMessageOutFiber,THREAD_FIBER);

	//							if (!bPass)
	//							{
	//								if (!m_pShare->bIsPattern)
	//								{
	//									HRegion hDebugRegionOutRecheckFiberClear;
	//									HTuple hDebugMessageOutRecheckFiberClear;
	//								    IR_RecheckFiberClear(hFiberRegion, m_pShare->hImageLensInnerRegionBF, &hFiberRegion, 
	//									  &hDebugRegionOutRecheckFiberClear, hMaxFiberIntensityBF, hMinFiberLengthBF, 
	//									  m_pShare->nStepDebug, &bPass, &hDebugMessageOutRecheckFiberClear);

	//									if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_FIBER])		
	//										StepDebug(hDebugRegionOutRecheckFiberClear, colorRed, hDebugMessageOutRecheckFiberClear,THREAD_FIBER);
	//								}
	//								if (!bPass)
	//								{
	//								 if(!m_pShare->nStepDebug){
	//									strInspResult.Format("[Fiber] Min Defect Length: %0.0f", hMinDefectLength.D());
	//									m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);							
	//									m_arrayOverlay[BRIGHT_FIELD].Add(hFiberRegion,  m_FiberParm.colorDefect);	
	//								 }
	//								if(!m_pShare->bInspectAll)		
	//									return m_FiberParm.nErrorCode;	
	//								else			
	//									m_FiberParm.nError = m_FiberParm.nErrorCode;
	//								}
	//							}
	//					
	//						OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear) Fiber Defect is %3.2f", timer.GetElapsedMSec());
	//					}
	//					//End Print Shift
	//				}
	//				else if(nThreadIndex==THREAD_EDGE_DEFECT){
	//					//***------------ Print Shift ----------------****
	//					if(m_EdgeDefectParm.bEnable){
	//						 timer.Reset();
	//						 HTuple hMaxSurfaceDefectEdgeContrastSS = m_EdgeDefectParm.nMaxSurfaceDefectEdgeContrastSS;
	//						 HTuple hSurfaceDefectEdgeThickness = m_EdgeDefectParm.nSurfaceDefectEdgeThickness;

	//							//** Advance Parameter
	//						 HTuple hAddMedianOffsetSurfaceDefect = m_EdgeDefectParm.nAddMedianOffsetSurfaceDefect;

	//							HRegion hEdgeDefectSingleSpot,hDebugRegionOutEdgeDefectSingleSpot;
	//							HTuple hDebugMessageOutEdgeDefectSingleSpot;

	//							EdgeDefectSingleSpot( m_pShare->hImageCuvetteRegionSS,  m_pShare->hLensEdgeRegion, &hDebugRegionOutEdgeDefectSingleSpot, 
	//								&hEdgeDefectSingleSpot, hMaxSurfaceDefectEdgeContrastSS, m_pShare->nStepDebug, 
	//								m_pShare->hRowPolar,m_pShare->hColPolar,m_pShare->hStartAnglePolar,m_pShare->hEndAnglePolar,m_pShare->hRadiusStartPolar, 
	//								m_pShare->hRadiusEndPolar,m_pShare->hWidthPolar,m_pShare->hHeightPolar,  m_pShare->hWidth,m_pShare->hHeight, hSurfaceDefectEdgeThickness, 
	//								hAddMedianOffsetSurfaceDefect, &hDebugMessageOutEdgeDefectSingleSpot, 
	//								&bPass);


	//							if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_EDGE_DEFECT])		
	//								StepDebug(hDebugRegionOutEdgeDefectSingleSpot, colorRed, hDebugMessageOutEdgeDefectSingleSpot,THREAD_EDGE_DEFECT);

	//							if (!bPass)
	//							{
	//								 if(!m_pShare->nStepDebug){
	//								
	//									m_arrayOverlay[SINGLE_SPOT].Add(hEdgeDefectSingleSpot,  m_EdgeDefectParm.colorDefect);	
	//								 }
	//								if(!m_pShare->bInspectAll)		
	//									return m_EdgeDefectParm.nErrorCode;	
	//								else			
	//									m_EdgeDefectParm.nError = m_EdgeDefectParm.nErrorCode;
	//							}
	//					
	//						OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern/Clear) Edge Defect is %3.2f", timer.GetElapsedMSec());
	//					}
	//					//End Print Shift
	//				}
	//				else if(nThreadIndex==THREAD_PRINT_SHIFT && m_pShare->bIsPattern){
	//					//***------------ Print Shift ----------------****
	//					if(m_PrintShiftParm.bEnable){
	//						 timer.Reset();
	//						HTuple hMinShift;
	//						HRegion hDebugRegionOutPrintShift,hPrintShiftRegion;
	//						HTuple hDebugMessageOutPrintShift;
	//						HTuple hMinEdgeToPatternDistanceOut;
	//						HTuple hMinEdgeToPatternDistance = m_PrintShiftParm.nMinEdgeToPatternDistance;
	//					

	//						IR_PrintShiftInsp(m_pShare->hLensInnerRegion, m_pShare->hPrintWithOpticalRegion,&hPrintShiftRegion,&hDebugRegionOutPrintShift,
	//							hMinEdgeToPatternDistance,m_pShare->nStepDebug,&bPass,&hDebugMessageOutPrintShift, &hMinEdgeToPatternDistanceOut);

	//						if(m_pShare->nStepDebug && m_bStepDebugSelection[THREAD_PRINT_SHIFT])
	//							StepDebug(hDebugRegionOutPrintShift, colorRed, hDebugMessageOutPrintShift,THREAD_PRINT_SHIFT);

	//						if (!bPass) {
	//							strInspResult.Format("[Shift] Min Distance: %0.0f", hMinEdgeToPatternDistanceOut.D());
	//							m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);					
	//							if(!m_pShare->nStepDebug)
	//								m_arrayOverlay[BRIGHT_FIELD].Add(hPrintShiftRegion, m_PrintShiftParm.colorDefect);
	//							if(!m_pShare->bInspectAll)
	//								return m_PrintShiftParm.nErrorCode;
	//							else
	//								m_PrintShiftParm.nError = m_PrintShiftParm.nErrorCode;
	//						}
	//						OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern) Print Shift is %3.2f", timer.GetElapsedMSec());
	//					}
	//					//End Print Shift
	//				}
	//				else if(nThreadIndex==THREAD_MISSING_PRINT && m_pShare->bIsPattern){
	//					if(m_MissingPrintParm.bEnable)
	//					{	
	//						 timer.Reset();
	//						HTuple hDarkRadiusTolerance = HTuple();
	//						HTuple hBrightRadiusTolerance = HTuple();
	//						HTuple hDarkWidthHeightTolerance = HTuple();
	//						HTuple hBrightWidthHeightTolerance = HTuple();

	//						HTuple hBrightDefectInnerRadius,hBrightDefectInnerHeight,hBrightDefectInnerWidth;
	//						HTuple hBrightLayer,bBrightPass,hDarkDefectInnerRadius;
	//						HTuple hDarkDefectInnerHeight,hDarkDefectInnerWidth,hMinDefectArea; 
	//						HTuple hDarkLayer,bDarkPass;
	//		
	//						HTuple hEnableBrightPatternInsp = m_MissingPrintParm.bEnableBrightPatternInsp;
	//						HTuple hEnableDarkPatternInsp = m_MissingPrintParm.bEnableDarkPatternInsp;
	//					
	//						HRegion hPatternDefectRegion,hDebugRegionMissingPrint;;
	//						HTuple hDebugMessageMissingPrintOut;
	//						for(int i=0; i<m_LensLocationParam.nNumberOfLayer; i++)
	//						{
	//							HTuple hTempDarkRadiusTolerance = m_MissingPrintParm.nDarkRadiusTolerance[i];
	//							HTuple hTempBrightRadiusTolerance = m_MissingPrintParm.nBrightRadiusTolerance[i];
	//						
	//							TupleConcat(hDarkRadiusTolerance, hTempDarkRadiusTolerance, &hDarkRadiusTolerance);
	//							TupleConcat(hBrightRadiusTolerance, hTempBrightRadiusTolerance, &hBrightRadiusTolerance);
	//						
	//						}

	//						IR_MissingPrint(m_pShare->hImageBF,m_pShare->hPatternLayerRegion,m_pShare->hImageLensInnerRegionBF,m_pShare->hLensInnerRegion,
	//							m_pShare->hOpticalRegion, &hPatternDefectRegion,&hDebugRegionMissingPrint,m_pShare->hNumberOfLayer, m_hDarkRegionOpeningOnLayer, 
	//							m_hDarkLayerMaxRadiusSaved, hDarkRadiusTolerance,m_hDarkLayerMaxWidthSaved,m_hDarkLayerMaxHeightSaved, m_hBrightRegionOpeningOnLayer, 
	//							m_hBrightLayerMaxRadiusSaved, hBrightRadiusTolerance, 
	//							m_hBrightLayerMaxWidthSaved,m_hBrightLayerMaxHeightSaved,m_pShare->hRowPolar,
	//							m_pShare->hColPolar, m_pShare->hRadiusPolar, m_pShare->hWidth, m_pShare->hHeight, hEnableBrightPatternInsp, hEnableDarkPatternInsp, 
	//							m_hRatioHeight1,m_hRatioHeight2,m_hRatioHeight3,m_hRatioHeight4,m_pShare->nStepDebug,
	//							&hBrightDefectInnerRadius,&hDarkDefectInnerRadius,&bPass,&hDebugMessageMissingPrintOut);

	//							if (!bPass) {
	//								
	//									strInspResult.Format("[Bright Pattern] Radius: %0.2f (%0.00fum)",hBrightDefectInnerRadius.D(), hBrightDefectInnerRadius.D()*m_dPixelSize);
	//									m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);	
	//								/*	strInspResult.Format("[Bright Pattern %d] Height: %0.2f (%0.00fum)", hBrightLayer.D(), hBrightDefectInnerHeight.D(), hBrightDefectInnerHeight.D()*m_dPixelSize);
	//									m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);
	//									strInspResult.Format("[Bright Pattern %d] Width: %0.2f (%0.00fum)", hBrightLayer.D(), hBrightDefectInnerWidth.D(), hBrightDefectInnerWidth.D()*m_dPixelSize);*/
	//							
	//									strInspResult.Format("[Dark Pattern] Radius: %0.2f (%0.00fum)",hDarkDefectInnerRadius.D(), hDarkDefectInnerRadius.D()*m_dPixelSize);
	//									m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);	
	//									/*strInspResult.Format("[Dark Pattern %d] Radius: %0.2f (%0.00fum)", hDarkLayer.D(), hDarkDefectInnerHeight.D(), hDarkDefectInnerHeight.D()*m_dPixelSize);
	//									m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);
	//									strInspResult.Format("[Dark Pattern %d] Radius: %0.2f (%0.00fum)", hDarkLayer.D(), hDarkDefectInnerWidth.D(), hDarkDefectInnerWidth.D()*m_dPixelSize);
	//									m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);*/
	//								
	//								/*strInspResult.Format("[Pattern] Min Area: %0.0f (%0.00fum2)", hMinDefectArea.D(), hMinDefectArea.D()*m_dPixelSize*m_dPixelSize);
	//								m_strInspectionResult[BRIGHT_FIELD].Add(strInspResult);	*/

	//								if(!m_pShare->nStepDebug)
	//									m_arrayOverlay[BRIGHT_FIELD].Add(hPatternDefectRegion,  m_MissingPrintParm.colorDefect);
	//								if(!m_pShare->bInspectAll)
	//									return m_MissingPrintParm.nErrorCode;
	//								else
	//									m_MissingPrintParm.nError = m_MissingPrintParm.nErrorCode;
	//							}
	//							OutputDebugLogTo(8,TRUE,"The Inspection times(Pattern) Missing Print is %3.2f", timer.GetElapsedMSec());
	//					}

	//				}

	//			}
	//		catch(HOperatorException ex) {
	//			bPass = FALSE;
	//			LogMessage(9, "[InspectionCore] %s", ex.ErrorMessage().Text());
	//			OutputDebugLogTo(8,TRUE,"The Inspection Process Error occurs at %d", nThreadIndex);
	//			return PROCESS_ERROR;
	//		}
	//		catch(...) {
	//			bPass = FALSE;
	//			LogMessage(9, "[InspectionCore->Inspect] %d", nThreadIndex);
	//			OutputDebugLogTo(8,TRUE,"The Inspection Process Error occurs at %d", nThreadIndex);
	//			return PROCESS_ERROR;
	//		}
	///*if(pThreadParm->pParm)
	//	pThreadParm->pParm->dInspTime += timer.GetElapsedMSec();*/

	return 0;
}


//int CInspectionCore::LightCalibrationMode(HImage hImage,int nTrack,int nGreyValue,
//										int* nCalibRange,double* dCalibDeviation,CRect rectSideTrack)
//{
//	HTuple hCalibRange, hCalibDeviation;
//	HTuple hTargetGrayMeanValue, hTolerance, hTargetGrayDeviation;
//	hTargetGrayDeviation = m_CalibrationData.nTargetGreyDeviation;
//
//
//	//if(nTrack==2) {
//	//	hTargetGrayMeanValue = m_CalibrationData.nTargetGreyMeanValueSide;
//	//	hTolerance = m_CalibrationData.nCalibTolerenceSide;
//
//	//	_FCI_SideCameraCalibration(hImage,
//	//								rectSideTrack.top, rectSideTrack.left, rectSideTrack.bottom, rectSideTrack.right,
//	//								hTargetGrayMeanValue, hTolerance,
//	//								&hCalibRange, &hCalibDeviation);
//
//	//}
//	//else{
//	//	HTuple hDifference, hMaxDeviationValue;
//
//	//	hTargetGrayMeanValue = m_CalibrationData.nTargetGreyMeanValue;
//	//	hTolerance = m_CalibrationData.nCalibTolerence;
//	//	hTargetGrayDeviation = m_CalibrationData.nTargetGreyDeviation;
//
//	//	_FCI_SetOptimumPulseWidth(hImage, hTargetGrayMeanValue, hTolerance, hTargetGrayDeviation,
//	//						&hCalibRange, &hDifference, &hMaxDeviationValue);
//
//	//	HImage hTargetImage, hResultantImage, hSubImage;
//	//	
//	//	//_FCI_LightCalibration(hImages[0], hImages[1], hImages[2], hImages[3],
//	//	//					&hTargetImage, &hResultantImage, &hSubImage, hTargetGrayMeanValue);
//
//	//}
//
//	*nCalibRange = hCalibRange.I();
//	*dCalibDeviation = hCalibDeviation.D();
//	OutputDebugLogTo(9,TRUE,"nCalibRange = %d , dCalibDeviation = %.2f",hCalibRange.I(),hCalibDeviation.D());
//
//	return 0;
//}

int CInspectionCore::LightCalibrationOffline(CString strCalibImagesPath, HImage* hMaxImageCalib, CString & strErrorMsg)
{
	strErrorMsg = "";
	HTuple hImageFiles;
	list_image_files(HString(strCalibImagesPath), "bmp", "recursive", &hImageFiles);

	if (hImageFiles.Length() == 0) {
		strErrorMsg = "No image found in folder";
		return -1;
	}

	//1. Get Maximum Image
	if (hImageFiles.Length() > 0) {
		ReadImage(hMaxImageCalib, hImageFiles[0]);
	}

	for (int i = 1; i < hImageFiles.Length(); i++) {
		HImage hCurrentImage;
		ReadImage(&hCurrentImage, hImageFiles[i]);
		MaxImage(hCurrentImage, *hMaxImageCalib, hMaxImageCalib);
	}

	//2. Validate Max Image
	HTuple hWidth, hHeight;
	GetImageSize(*hMaxImageCalib, &hWidth, &hHeight);
	HObject hRect;
	GenRectangle1(&hRect, 0, 0, hHeight, hWidth);
	HTuple hImageRows, hImageCols;
	GetRegionPoints(hRect, &hImageRows, &hImageCols);

	HTuple hGrayval;
	GetGrayval(*hMaxImageCalib, hImageRows, hImageCols, &hGrayval);

	HTuple hBadBlackPixel, hBadBlackPixelCount;
	TupleEqualElem(hGrayval, 0, &hBadBlackPixel);
	TupleSum(hBadBlackPixel, &hBadBlackPixelCount);
	if (hBadBlackPixelCount > 0) {
		CString strMsg;
		strMsg.Format("Image contains bad black pixel(=0) do u want to use this image for light correction? (%d count)/n", hBadBlackPixelCount.I());
		strErrorMsg = strErrorMsg + strMsg;
	}

	HTuple hBadWhitePixel, hBadWhitePixelCount;
	TupleEqualElem(hGrayval, 255, &hBadWhitePixel);
	TupleSum(hBadWhitePixel, &hBadWhitePixelCount);
	if (hBadWhitePixelCount > 0) {
		CString strMsg;
		strMsg.Format("Image contains bad white pixel(=255) do u want to use this image for light correction? (%d count)", hBadWhitePixelCount.I());
		strErrorMsg = strErrorMsg + strMsg;
	}

	return 0;
}

int CInspectionCore::LightCalibrationMode(HImage hImage, HImage* hImageCalibResult,
										  BOOL bFirst, BOOL bRefImg, BOOL bAvgImg, BOOL bLightCorrection,
										  CCalibrationData* pCalibData, CString & strErrorMsg)
{
	if (bRefImg) {
		if (bFirst) {
			GenImageProto(hImage, &pCalibData->hCalibSumImage, 0);
			AddImage(hImage, pCalibData->hCalibSumImage, &pCalibData->hCalibSumImage, 1, 0);
			ConvertImageType(pCalibData->hCalibSumImage, &pCalibData->hCalibSumImage, "uint2");
		}
		else {
			AddImage(hImage, pCalibData->hCalibSumImage, &pCalibData->hCalibSumImage, 1, 0);
		}
	}
	if (bAvgImg) {
		HImage hAvgImage;
		
		_Average_Reference_Image(pCalibData->hCalibSumImage, &hAvgImage);
		ConcatObj(pCalibData->hCalibAvgImage, hAvgImage, &pCalibData->hCalibAvgImage);
	}
	if (bLightCorrection) {
		//1. Loop thorugh list image and max image
		if (pCalibData->hCalibAvgImage.CountObj() > 0) {
			*hImageCalibResult = pCalibData->hCalibAvgImage.SelectObj(1);
		}
		for (int i = 2; i <= pCalibData->hCalibAvgImage.CountObj(); i++) {
			HImage hCurrentImage = pCalibData->hCalibAvgImage.SelectObj(i);
			MaxImage(hCurrentImage, *hImageCalibResult, hImageCalibResult);
		}

		//2. Validate Max Image
		HTuple hWidth, hHeight;
		GetImageSize(*hImageCalibResult, &hWidth, &hHeight);
		HObject hRect;
		GenRectangle1(&hRect, 0, 0, hHeight, hWidth);
		HTuple hImageRows, hImageCols;
		GetRegionPoints(hRect, &hImageRows, &hImageCols);

		HTuple hGrayval;
		GetGrayval(*hImageCalibResult, hImageRows, hImageCols, &hGrayval);

		HTuple hBadBlackPixel, hBadBlackPixelCount;
		TupleEqualElem(hGrayval, 0, &hBadBlackPixel);
		TupleSum(hBadBlackPixel, &hBadBlackPixelCount);
		if (hBadBlackPixelCount > 0) {
			CString strMsg;
			strMsg.Format("Image contains bad black pixel(=0) do u want to use this image for light correction? (%d count)/n", hBadBlackPixelCount.I());
			strErrorMsg = strErrorMsg + strMsg;
		}

		HTuple hBadWhitePixel, hBadWhitePixelCount;
		TupleEqualElem(hGrayval, 255, &hBadWhitePixel);
		TupleSum(hBadWhitePixel, &hBadWhitePixelCount);
		if (hBadWhitePixelCount > 0) {
			CString strMsg;
			strMsg.Format("Image contains bad white pixel(=255) do u want to use this image for light correction? (%d count)", hBadWhitePixelCount.I());
			strErrorMsg = strErrorMsg + strMsg;
		}

		/*HImage hResultantImage, hTargetUniformImage, hLightCorrectionImage, hPosContrastImg, hNegContrastImg;
		HTuple hTargetGrayValue = nGreyValue;

		HImage hImg1, hImg2, hImg3, hImg4;
		hImg1 = pCalibData->hCalibAvgImage.SelectObj(1);
		hImg2 = pCalibData->hCalibAvgImage.SelectObj(2);
		hImg3 = pCalibData->hCalibAvgImage.SelectObj(3);
		hImg4 = pCalibData->hCalibAvgImage.SelectObj(4);

		_FCI_LightCorrection(hImg1, hImg2, hImg3, hImg4,
							&hResultantImage, &hTargetUniformImage, &hLightCorrectionImage,
							&hPosContrastImg, &hNegContrastImg,
							hTargetGrayValue);
		
		CopyImage(hPosContrastImg, hImageCalibPos);
		CopyImage(hNegContrastImg, hImageCalibNeg);
		CopyImage(hResultantImage, hImageCalibResult);*/
	}

	return 0;
}

int CInspectionCore::DotCalibrationMode(HImage hImage,CRect rectDotCalib,HTuple &hDotWidths,HTuple &hDotHeights,
										HTuple &hDotRows,HTuple &hDotColumns)
{
	COLORREF colorGreen = RGB(0, 255, 0);

	HRegion hDotRegions;
	HTuple hResolutionAlongX, hResolutionAlongY;
	HTuple hInputAlongX, hInputAlongY, hInputMinRadiusDotTarget;

	hInputAlongX = m_CalibrationData.nInputAlongX;
	hInputAlongY = m_CalibrationData.nInputAlongY;
	hInputMinRadiusDotTarget = m_CalibrationData.nInputMinRadiusDotTargetPixel;

	_FCI_DotCalibration(hImage, &hDotRegions,
						rectDotCalib.top, rectDotCalib.left, 
						rectDotCalib.bottom, rectDotCalib.right,
						hInputAlongX, hInputAlongY, hInputMinRadiusDotTarget,
						&hDotWidths, &hDotHeights, 
						&hDotRows, &hDotColumns,
						&hResolutionAlongX, &hResolutionAlongY);

	m_arrayOverlayDotCalibration.Add(hDotRegions,colorGreen);
	if(hResolutionAlongX.Length()>0)
		m_CalibrationData.dResolutionAlongXInMicronPerPixel = hResolutionAlongX.D();
	if (hResolutionAlongY.Length()>0)
		m_CalibrationData.dResolutionAlongYInMicronPerPixel = hResolutionAlongY.D();

	return 0;
}

void CInspectionCore::CleanDefectData(int nDoc)
{
	m_DefectData[nDoc].arrayDefectCenters.clear();
	m_DefectData[nDoc].arrayDefectRects.clear();
	m_DefectData[nDoc].arrayDefectX.clear();
	m_DefectData[nDoc].arrayDefectY.clear();
}

void CInspectionCore::CleanCentralizedDefectData(int nStationId, int nDie, int nDoc)
{
	m_CentralizedDefectData[nStationId][nDie][nDoc].arrayDefectCenters.clear();
	m_CentralizedDefectData[nStationId][nDie][nDoc].arrayDefectRects.clear();
	m_CentralizedDefectData[nStationId][nDie][nDoc].arrayDefectX.clear();
	m_CentralizedDefectData[nStationId][nDie][nDoc].arrayDefectY.clear();
}
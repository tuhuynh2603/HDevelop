/// CInspectionCore: Vision Inspection base class
/// Udupa; Feb'2015

#pragma once

#include "HalconCpp.h"
#include "HDevThread.h"
#include "ParameterCore.h"
#include "Parameters.h"
#include <vector>
#include "CDeepLearningInterface.h"

#define PVI_DEFECT_COUNT 11

using namespace HalconCpp;


#ifdef _LS3_IF
	#define	DECLSPEC_LS3_IF _declspec(dllexport)
#else
	#define DECLSPEC_LS3_IF _declspec(dllimport)
#endif

#define DEBUG_STEP_MODE 0x00000200

enum {
	GOOD_UNIT,
	DEVICE_LOCATION = 3,
	DATUM_LOCATION,
	TILT_INSP,
	COVER_LAYER_INSP,
	COVER_LAYER_ALIGNMENT,
	ENCAP_INSUFFICIENT,
	ENCAP_EXCESS,
	ENCAP_CRACK,
	ENCAP_BLOWHOLE,
	ENCAP_CONTAMINATION,
	SLOT,
	DIE_EDGE,
	ENCAP_MAGNUS,
	PVI_INSP,
	USER_MODIFIED, //Is False Acceptance
	USER_MODIFIED_FALSE_REJECTION,
	NO_IMAGE,
	ERROR_CODES,
};


enum {
	THREAD_MULTIPLE_LENS,//0
	THREAD_COUNT
};


enum Areatype{
		RECTANGLE,
		POLYGON,
		ECLIPSE
};

enum ReferencePointType {
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	ENCAP_EDGE_LEFT,
	ENCAP_EDGE_RIGHT,
	CENTER,
	///
	/// Add new inspection codes here
	///
	REF_POINT_TYPE_COUNT
};

enum DirectionType {
	LEFT_TO_RIGHT,
	TOP_TO_BOTTOM,
	RIGHT_TO_LEFT,
	BOTTOM_TO_TOP,
	NONE,
	DIRECTION_TYPE_COUNT
};

enum LineDirectionType {
	VERTICAL,
	HORIZONTAL,
	LINE_DIRECTION_TYPE_COUNT
};

class DECLSPEC_LS3_IF CPVIDefect
{
public: 
	CPVIDefect();

	int m_nID;
	int m_nErrorCodeID;
	CString strDefectName;

	BOOL bEnable;
	int nDefectCharacteristics;
	int m_nSize;
	int nContrast;
	int nMinLength;
	int nMinSquareSize;
	int nMinCount;
};

class DECLSPEC_LS3_IF CPVIInspectionSetup
{
public:
	CPVIInspectionSetup();
	~CPVIInspectionSetup();

	BOOL		bEnable;
	//BOOL		bNonUniformSurface;
	//int			nDeviceMaskThickness;
	//Areatype	nAreatype;
	Areatype	nDeviceAreaType;
	int			nDeviceMaskNumber;
	Areatype	nDeviceMaskArea[10];
	int			nDefectCount;
	double dDeviceEdgeWidth;
	double dDeviceEdgeOffset;

	//For Edge
	BOOL bEdgeEnable;
	int nMinEdgeContrast;
	int nMaxEdgeContrast;
	double dEdgeRegionOpen;
	double dEdgeWidth;
	double dEdgeOffset;

	BOOL bPVIRecheckEnable;
	int nMinGVDiff;
	int nIntensity;

	BOOL bPVISlotConnectionCheckEnable;
	int nNumOfSlot;
	BOOL bPVISlotConnectionCheckTopDie;
	BOOL bPVISlotConnectionCheckBottomDie;
	BOOL bPVISlotConnectionCheckLeftDie;
	BOOL bPVISlotConnectionCheckRightDie;

	CRect m_rectPviArea;
	CRect m_rectPviMaskRegions[10];

	CRect m_rectPviSlot[10];
	CPVIDefect	*m_pPviDefect;

	//For PVI Polygon
	CPoint m_polygonPviArea[20];
	int nPolygonPointNumber;
};

class DECLSPEC_LS3_IF CCoverLayer : public CParameterCore
{
public:
	CCoverLayer();

	BOOL bEnable;

	int nContrast[5];
	int nCoverLayerLength[5];
	int nCoverLayerHeight[5];
	double dGapWidthTolerance[5];
	double dGapHeightTolerance[5];
	double dMaskSize[5];
};

class DECLSPEC_LS3_IF CCoverLayerAlignment : public CParameterCore
{
public:
	CCoverLayerAlignment();

	BOOL bEnablePointer;

	LineDirectionType nCoverLayerDirection[6];
	double dToleranceDist[6];
};

class DECLSPEC_LS3_IF CEncap : public CParameterCore
{
public:
	CEncap();

	BOOL bEnable;
	BOOL bEnableEncapSurface;

	double dErosionOffset;
	double dMaxLayOnAllowedPercentage;//Check Maximum Percentage of length of the defect lay on the encap border allowed

	//Encap Crack
	int nMinContrastCrack;
	int nMinSizeCrack;
	int nMinLengthCrack;
	int nMinSquareSizeCrack;

	//Encap Blow Hole
	int nMinContrastBH;
	int nMinSizeBH;
	double dMinCircularityBH;
	int nMinLengthBH;
	int nMinSquareSizeBH;
	int nMinGrayMeanBH;

	//Encap Contamination
	int nMinContrastCT;
	int nMinSizeCT;
	double dMaxCircularityCT;
	int nMinLengthCT;
	int nMinSquareSizeCT;

	//int nImageIndex;
	//int nMinContrast;
	//int nMaxContrast;
};


class DECLSPEC_LS3_IF CSlot : public CParameterCore
{
public:
	CSlot();

		BOOL bEnable;
		BOOL bEnableCheckSurface[5];
		int nEdgeOffset[5];
		int nMinMeanGVDiffSlot [5];
		int nMinContrast[5];
		int nMinSize[5];
		int nMinLength[5];
		int nMinSquareSize[5];
		int nMinCount[5];

	//int nImageIndex;
	//int nMinContrast;
	//int nMaxContrast;
};

//class DECLSPEC_LS3_IF CDieInsp : public CParameterCore
//{
//public:
//	CDieInsp();
//
//	BOOL bEnable;
//};

class DECLSPEC_LS3_IF CTiltInsp : public CParameterCore
{
public:
	CTiltInsp();

	BOOL bEnable;
//	int nImageIndex;
	int nContrast;
	int nToleranceSide;
	int nAngleOfSideView;
	int nDistanceForTiltInsp;

	//int nRegionChar;
	//int nRegionHeight;
	//int nMeanGreyValue;
};

class DECLSPEC_LS3_IF CDieEdge : public CParameterCore
{
public:
	CDieEdge();

	BOOL bEnable;
	int nDefectCharacteristics;
	int m_nSize;
	int nContrast;
	int nMinLength;
	int nMinSquareSize;
	int nMinCount;
	double dOffset;
	double dWidth;
};

class DECLSPEC_LS3_IF CEncapMagnus : public CParameterCore
{
public:
	CEncapMagnus();


	int Thresh[2];
	int Crop_Expand[2];
	int Dilate_Kernel[2];
	int Opening_Kernel[2];
	int Crop_Smooth;
	int Area_Object;
	BOOL bEnable;



};


class DECLSPEC_LS3_IF CTeachInspectData
{
public:
	CTeachInspectData();
};


class DECLSPEC_LS3_IF CCalibrationData
{
public:
	CCalibrationData();

	CRect rectCalibSideTrack;
	CRect rectDotCalib;

	int nCalibTolerence;
	int nTargetGreyDeviation;
	int nTargetGreyMeanValue;

	int nTargetGreyMeanValueSide;
	int nCalibTolerenceSide;

	int nInputAlongX;
	int nInputAlongY;
	double dResolutionAlongXInMicronPerPixel;
	double dResolutionAlongYInMicronPerPixel;

	int nInputMinRadiusDotTargetPixel;

	HImage hCalibSumImage;
	HImage hCalibAvgImage;
	HTuple hResolutionYInput;

	BOOL bEnableLightCorrection;
	HImage hCalibMaxImage;
	HTuple hLightCoefficient;
	HTuple hLightRows;
	HTuple hLightCols;
};

class DECLSPEC_LS3_IF CTrainingData
{
public:
	CTrainingData();

	//int nLensRadius;
	//int nCuvetteRadiusRa;
	//int nCuvetteRadiusRb;
	//int nCriticalOpticsZoneRadius;

	//void CopyParam(CTrainingData m_TrainingData);
// Device Location //
	BOOL bEnableDLModelInspect;
	BOOL bDeviceLocationEnable;
	int	nNoOfLocationTeachRegion;
	int	nMinDeviceContrast;
	double dDeviceLocWidth;
	double dDeviceLocHeight;
	double dMaskSize;
	BOOL bSobelAmp;
	//use to determine the reference point (from devicelocation detected by Blob Detection Algo)used for compute the shifted/rotated between teach and inspect image
	ReferencePointType nDeviceReferencePointType;

	CRect m_rectDeviceLoc;		//// Max. rectangle draw for Device Loaction = 1
	CRect m_rectAutoFocusROI; //Use for Auto Focusing Sequence
	BOOL bEnableAutoFocusSequence;
	
	//Using the Edge Detection instead of Device location Algo
	BOOL bDeviceEdgeDetectionEnable;
	int nDeviceHorizontalContrast;
	DirectionType nDeviceHorizontalDirection;

	int nDeviceVerticalContrast;
	DirectionType nDeviceVerticalDirection;
	CRect m_rectDeviceHorizontalLineLoc;		//// Rectangle draw for Horizontal Search Line
	CRect m_rectDeviceVerticalLineLoc;		//// Rectangle draw for Vertical Search Line

	BOOL bUseSmoothingForEdgeDetection;


// Dattam/Pattern Search //
	BOOL bDattamSearchEnable;
	double dDattamSearchAreaX;
	double dDattamSearchAreaY;
	int nDattamSearchMaxRotAngle;
	int nDattamSearchNoOfDattams;
	double dDattamSearchMinAcceptanceScore;
	CRect m_rectDattamSearch[4];		//// Max. rectangle draw for Dattam/Pattern Search = 4

// Region Search //
	BOOL bRegionSearchEnable;
	int nRegionSearchNoOfRegions;	//// Max. rectangle draw for Region Search = 10
	int nNoOfPviArea;

// Mask Region Search //
//	BOOL bMaskRgnSearchEnable;
//	int nMaskRgnSearchNumbers;
//	CRect m_rectMaskRgnSearch[10];		//// Max. rectangle draw for Mask Region Search = 10

// Encap Region //
	BOOL bEnableEncap;
	int nEncapMinContrast;
	int nEncapMaxContrast;
	CRect m_rectEncap[3];//// Encap rectangles - 3; encap area, outer & inner rectangle
	int m_nRectWidth[3];
	int m_nRectHeight[3];

	//Masking for encap tracing
	int nEncapTracingMaskNumber;
	Areatype nEncapTracingMaskType[10];
	int nEncapTracingPolygonPointNumber[10];
	CPoint m_EncapTracingPolygonPointArea[10][20];
	CRect m_rectMaskEncapTracing[10];
	//Masking for encap surface inspection
	int nEncapSurfaceMaskNumber;
	CRect m_rectMaskEncapSurface[10];

// Cover Layer //
	BOOL bEnableCoverLayer;
	
	//BOOL bEnableCLDevLoc;
	/*int nCLAboveCloseDistance;
	int nCLAboveFarDistance;
	int nCLBelowCloseDistance;
	int nCLBelowFarDistance;
	int nCLSideCloseDistance;
	int nCLSideFarDistance;*/
	
	BOOL bEnableCLDatum;
	
	int nCLDatum;
	int nCLInspRegion;

	int nCLMaxRotationAngle;
	int nCLPatternSearchX;
	int nCLPatternSearchY;
	int nCLMinAcceptanceScore;
	/*int nCLDatumAboveCloseDistance;
	int nCLDatumAboveFarDistance;
	int nCLDatumBelowCloseDistance;
	int nCLDatumBelowFarDistance;
	int nCLDatumSideCloseDistance;
	int nCLDatumSideFarDistance;*/
	CRect m_rectCLDatum[2];
	CRect m_rectCoverLayer[5];//// Max. 5 rectangle

//// Cover Layer Alignment
	BOOL bEnableCLAlignmentPointer;
	int nPointer;
	int nCLAP_MaxRotationAngle;
	int nCLAP_PatternSearchX;
	int nCLAP_PatternSearchY;
	int nCLAP_MinAcceptanceScore;
	BOOL bEnableManualPointer;
	CRect m_rectPointer[6];
//Slot
	BOOL bEnableSlot;
	int nNumOfSlot;
	int nMinIntensitySlot;
	int nContrastEdgeSlot;
	CRect m_rectSlot[5];
	CRect m_rectSlotLimit[5];
	int nSlotMaskNumber;
	Areatype nSlotMaskType[10];
	int nSlotMaskPolygonPointCount[10];
	CPoint m_SlotMaskPolygonPoint[10][20];
	CRect m_rectMaskSlot[10];

	//Slot REGION Teach
	Areatype nSlotAreaType;
	int nSlotPolygonPointCount;
	CPoint m_SlotPolygonPoint[5][20];

	Areatype nSlotLimitAreaType;
	int nSlotLimitPolygonPointCount;
	CPoint m_SlotLimitPolygonPoint[5][20];

//// Tilt Teach ////
	CRect m_rectTilt;
	

	CArray<HTuple> hvec_TupleVectorLocation;
	CArray<HTuple> hvec_TupleVectorPVI[32];
	CArray<double> darrayPVI[32][10];
	CArray<HTuple> hvec_TupleVectorEncap;
	CArray<double> darrayEncap[15];
	CArray<HTuple> hvec_TupleVectorCoverLayer;
	CArray<double> darrayCoverLayer[15];
	CArray<HTuple> hvec_TupleVectorCoverLayerPointer;

	HObject hObjectDeviceLocation;
	HObject hObjectPointerLocation;
	HTuple hTupleProjectionParam;
	HTuple hTupleTeachDoc;
	HTuple hTupleEdgeTeachTuple;
	HTuple hTupleDatumTeachTuple;
	HTuple hTuplePointerModelIDs;

	//For Centralized PC
	BOOL bCentralizedVerificationMode;



	//////////Encap Magnus Teach parameter 

	BOOL bEnable_EncapManus;
	int nThreshMin_EncapManus;
	int nThreshMax_EncapManus;

	CRect hRect_DeviceLocationEncap_magnus;
	CRect hRect_EncapLocation_magnus;
	CRect hRect_CropSmoothEncap_magnus;
	CRect hRect_CropNoSmoothEncap_magnus;
	CRect hRect_CropRemoveBlackLine_magnus;

	int nCrop_ExpandLeft_magnus[2];
	int nCrop_ExpandRight_magnus[2];
	int nCrop_ExpandHeight_magnus[2];
	int nCrop_Smooth_EncapManus[2];
	int nCrop_RemoveBLLeft_magnus[2];
	int nCrop_RemoveBLRight_magnus[2];
	int nCrop_RemoveBLHeight_magnus[2];

	int nDilateX_EncapManus;
	int nDilateY_EncapManus;

	int nOpeningX_EncapManus;
	int nOpeningY_EncapManus;
	int nValue_OpeningCircle_magnus;
	int nValue_OpeningCircleCrop_magnus;
	int nValue_Smooth_EncapMagnus;
	int nArea_Object_EncapManus;
	int nHeight_Object_magnus;
	int nWidth_Object_magnus;

	int nThreshMin_Black_EncapManus;
	int nThreshMax_Black_EncapManus;
	int nThreshMin_White_EncapManus;
	int nThreshMax_White_EncapManus;
	///////////////
};


class DECLSPEC_LS3_IF CInspectinonData
{
public:
	CInspectinonData();

	HTuple hTiltHeight;

	HTuple hStoreTeachModelRowCentre;
	HTuple hStoreTeachModelColCentre;
	HTuple hStoreMatchModelCentreRow;
	HTuple hStoreMatchModelCentreCol;
};

struct DECLSPEC_LS3_IF CDefectData
{
	std::vector<CRect> arrayDefectRects;
	std::vector<CPoint> arrayDefectCenters;
	std::vector<std::vector<int>> arrayDefectX;
	std::vector<std::vector<int>> arrayDefectY;
	HImage	EncapProcessedBackGround;
};

typedef CArray<HRegion> HRegionArray;
typedef CArray<UINT> UINTArray;

class COverlayData
{
public:
	HImage hImage;
	HRegion region;
	UINT color;
	CString strDebugMessage;

	COverlayData()
	{
	}
	
	COverlayData(const COverlayData& src)
	{
		hImage = src.hImage;
		region = src.region;
		color = src.color;
		COverlayData::strDebugMessage = strDebugMessage;
	}

	COverlayData(HImage hImage, HRegion region, UINT color, CString strDebugMessage)
	{
		COverlayData::hImage = hImage;
		COverlayData::region = region;
		COverlayData::color = color;
		COverlayData::strDebugMessage = strDebugMessage;
	}

	~COverlayData()
	{
	}
};

class COverlayArray : public CArray<COverlayData>
{
public:
	/*HImage hImage;
*/
	void Add(HImage hImage, HRegion region, UINT color, CString strDebugMessage)
	{
		CArray::Add(COverlayData(hImage, region, color, strDebugMessage));
	}

	void Add(HRegion region, UINT color, CString strDebugMessage="")
	{
		CArray::Add(COverlayData(HRegion(), region, color, strDebugMessage));
	}
};

typedef struct {
	HImage hImageBF, hImageDF, hImageSS, hImageSSB;
	
	HRegion hMaskRegions,hOpticalRegion;
	HObject hPrintWithOpticalRegion;
	HRegion hRegionMarksDilate;

	HTuple  hWidth, hHeight;
	HTuple  hLensCenterRow, hLensCenterCol;
	
	HObject hPatternLayerRegion;
	//-----Len Location-------
	HImage hLensEdgeImage, hInnerLensRegion, hLensRegion;
	HTuple hEdgeThreshold, hInnerRegionOffset;
	HTuple hArea, hRow, hColumn, hRowPolar, hColPolar, hRadiusPolar, hCircularity;
	HTuple hInnerRadius, hOuterRadius, hMaxRadius, hDiameterDiff;
	HRegion hLensEdge, hLensEdgeRegion, hPatternRegion, hCuvetteRegion;

	HRegion hDarkRegionisPTInBF,hBrightRegionInDF;

	HRegion hImageLensRegionBF, hImageLensRegionDF, hImageSPZoom;
	HRegion hImageLensEdgeRegionBF, hImageLensEdgeRegionDF, hImageLensEdgeRegionSS;
	HRegion hImageLensRegion, hLensInnerRegion;
	HRegion hImageLensInnerRegionBF, hImageLensInnerRegionDF, hImageLensInnerRegionSS;
	HObject hLocationMarkRegion;
	//Inner Region
	HRegion hLensTrueInnerRegion, hLensEdgeTrueRegionBF, hLensEdgeTrueRegionDF;
	HImage hLensTrueInnerRegionDF,hLensTrueInnerRegionBF;

	HImage hImageLensTrueInnerRegionDF,hImageLensTrueInnerRegionBF;
	HImage hLensEdgeRegionPT, hImageLensEdgePT;
	HObject hImageLensEdgePT_DF;

	HObject hAllObjectForRoughness,hBrightRegionInDFRoughness,hAllObjectForRoughnessPolar;
	HRegion hLensEdgeRegionInBF;

	int nStepDebug;
	BOOL bInspectAll;
	HTuple  hOverlapAnglePolar,hStartAnglePolar, hEndAnglePolar, hOverlapXY;

	HTuple  hLensRadius, hLensRadiusOffset;
	HTuple  hCuvetteRa, hCuvetteRb;
	HTuple  hCuvetteRow, hCuvetteCol, hCuvetteRadius;
	HTuple hRadiusStartPolar, hRadiusEndPolar, hWidthPolar, hHeightPolar;
	
	BOOL bIsPattern;
	BOOL bIsLocationMark;

	HTuple hNumberOfLayer;
	HRegion hPolarTransDarkPatternRegion, hPolarTransBrightPatternRegion;
	HRegion hPatternInspRegion, hDarkPatternRegionUnion;

	HObject hLensTrueInnerRegionSS,hLensEdgeTrueRegionSS,hPatternRegionForSurfaceInspSS;

	HRegion hImageCuvetteRegionBF, hImageCuvetteRegionDF, hImageCuvetteRegionSS, hImageCuvetteRegionSSB;

} SHARED_PARM;

typedef struct {
	HANDLE m_hThread;
	HANDLE m_hRun;
	HANDLE m_hDone;
	
	long lDebugFlag;
	HTuple bPass;

} THREAD_PARM;

class DECLSPEC_LS3_IF CInspectionCore
{
public:
	CInspectionCore();
	~CInspectionCore();

	CCoverLayer m_CoverLayerParm;
	CCoverLayerAlignment m_CoverLayerAlignmentParm;
	CEncap m_EncapParm;
	CSlot m_SlotParm;
	CTiltInsp m_TiltInspParm;
	CDieEdge m_DieEdgeParm;
	CEncapMagnus m_EncapMagnusPam;

	SHARED_PARM *m_pShare;
	BOOL m_bParallelism;
	THREAD_PARM m_ThreadParms[THREAD_COUNT];
	HANDLE m_hDoneEvents[THREAD_COUNT];
	HANDLE handlesDone[THREAD_COUNT];

	int m_nThreadIndex;
	BOOL m_bKillInspectThreads;
	BOOL m_bInitiateClearCache;
	BOOL m_bClearThreadCache;

	CParameterCore* m_InspParms[ERROR_CODES];

	CTrainingData m_TrainingData;
	CTeachInspectData m_TeachInspectData;
	CCalibrationData m_CalibrationData;

	CInspectinonData m_InspectionData;
	CPVIInspectionSetup m_pPviInspectionSetup[10];
	BOOL m_bIsPatternLens;
	double m_dPixelSize;
	BOOL m_bStepDebugSelection[THREAD_COUNT];

	CDefectData m_DefectData[6];//#define MAX_DOCS 6 in TrackHandler.h
	CDefectData m_CentralizedDefectData[3][5][6];//#define MAX_DIES 5, #define MAX_DOCS 6 in TrackHandler.h, #define NO_OF_PORT_CENTRALIZED 3 in CentralizedCommInterface.h

	void CleanDefectData(int nDoc);
	void CleanCentralizedDefectData(int nStationId, int nDie, int nDoc);

	virtual int Initialize(BOOL bParallel);
	virtual int InitializeInspection();
	int Close();
	//int Reset();
	/*virtual*/ int Teach(HImage hImage,
					  BOOL bRegionTeach,
					  BOOL *bEnable,
					  int nTeachStep,
					  int nCurTrack,
					  int nDoc,
					  int nFOV,
					  int nTotalFOV,
					  CTrainingData* pTrainingData,
					  CCalibrationData* pCalibData,
					  BOOL bEncapLoc);

	int AutoTeach(HImage hImage,
				BOOL bRegionTeach,
				int nCurTrack,
				int nDoc,
				int nFOV, 
				int nTotalFov,
				CCalibrationData* pCalibData);

	virtual int Inspect(HImage hImage, 
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
						CStringArray& strArrayInspValues);

	int Inspect(int nThreadIndex, HTuple& bPass);
	int ClearTeachParam(CTrainingData* pTrainingData);
	int ClearInspParam(CInspectinonData* pInspData, BOOL bInspClearAll);
	int ClearCalibParam(CCalibrationData* pCalibData, BOOL bClearAvg, BOOL bClearInsp);
	int GetCalibDataForInsp(CCalibrationData* pCalibData);
	int ClearLightCorrectionParam(CCalibrationData* pCalibData);

	int LightCalibrationMode(HImage hImage, HImage* hImageCalibResult,
							BOOL bFirst, BOOL bRefImg, BOOL bAvgImg, BOOL bLightCorrection,
							CCalibrationData* pCalibData, CString & strErrorMsg);

	int LightCalibrationOffline(CString strCalibImagesPath, HImage* hMaxImageCalib, CString & strErrorMsg);

	int LightCorrection(HImage hImage, HTuple hCoefficient, HTuple hLightImageRows, HTuple hLightImageCols, HImage* hCorrectedImage);
	int LightCoefficientInspect(HImage hLightImage, HTuple* hCoefficient, HTuple* hLightImageRows, HTuple* hLightImageCols);

	int DotCalibrationMode(HImage hImage,CRect rectDotCalib,HTuple &hDotWidths,HTuple &hDotHeights,
										HTuple &hDotRows,HTuple &hDotColumns);

	COverlayArray m_arrayOverlay[2];
	COverlayArray m_arrayOverlayDotCalibration;
	COverlayArray m_arrayOverlayInspection;
	COverlayArray m_arrayOverlayTeach;
	COverlayArray m_arrayOverlayPVIInsp;
	CStringArray m_strInspectionResult[5][5];

	COverlayArray m_arrayOverlayDisplayLoc;

	void StepDebug(HImage hImage,HRegion hDebugRgn, UINT color, HTuple hDebugMsg,int nRegion);
	void ZoomImage(HImage hImgSrc, HImage hImgDst, int nWidth, int nHeight,int nDoc);
	void ReleaseMemory();
	void ClearCache();
	int PVIDefectsOverlay(HImage hPVISubImg, HRegion hPVIRgn, CString strPVIDefectName, COLORREF PVIcolor);//
};

UINT DECLSPEC_LS3_IF InspectionThread(LPVOID pParam);

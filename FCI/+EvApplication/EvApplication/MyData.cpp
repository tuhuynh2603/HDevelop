// MyData.cpp : implementation file
//

#include "stdafx.h"
#include "MyData.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemListDataType

BEGIN_LIST_ITEM_DATA_TYPE(CPropertyItemListAccessLevel)
	LPCTSTR_STRING_ITEM_DATA(_T("Operator")),
	LPCTSTR_STRING_ITEM_DATA(_T("Technician")),
	LPCTSTR_STRING_ITEM_DATA(_T("Administrator")),
//	LPCTSTR_STRING_ITEM_DATA(_T("Disabled"))
END_LIST_ITEM_DATA_TYPE(CPropertyItemListAccessLevel)

/////////////////////////////////////////////////////////////////////////////
// CMyData

CMyData::CMyData()
{
	ResetDefaults();
} 

void CMyData::ResetDefaults()
{

	m_nOpenLot						= 0;
	m_nCloseLot						= 0;
	m_nProdMode						= 0;
	m_nSaveOnlinePassImages			= 1;
	m_nSaveOnlineFailImages			= 1;
	m_nByPassResult					= 2;

	m_nSaveOnlineDLImages = 1;
	m_nSaveOnlineDLPassImages = 1;
	m_nSaveOnlineDLFailImages = 1;

	m_nTeach						= 1;
	m_nStepSelection				= 1;
	m_nStepDebug					= 1;
	m_nInspect						= 0;
	m_nInspectSavedImage			= 1;
	m_nSetFolder					= 0;
	m_nSaveOfflinePassImages		= 1;
	m_nSaveOfflineFailImages		= 1;
	m_nInspectLoop					= 1;
	m_nInspectCycle					= 2;
	m_nCamEnable					= 1;
	m_nInspectionSeq				= 1;
	m_nCalibSettings				= 2;
	m_nInspSeqLoop					= 1;
	m_nSaveInspSeqImages			= 1;

	m_nGrab							= 0;
	m_nLive							= 0;
	m_nGrabAll						= 0;
	m_nLightSet						= 2;

	m_nVisionParm					= 1;
	m_nTeachParm					= 1;
	m_nDefectParm					= 1;
	m_nManageCfg					= 2;
	m_nCreateConfig					= 2;
	m_nPviDefectParm				= 1;
	m_nCameraCfg					= 2;
	m_nIOTest						= 2;
	m_nDeviceCfg					= 2;
	m_nTeachInspLoc					= 2;
	m_nDefectPriority				= 2;

	m_nLoadImg						= 0;
	m_nLoadImgAll					= 0;
	m_nSaveImg						= 0;
	m_nSaveImgAll					= 0;
	m_nLoadTeachImg					= 0;
	m_nLoadTeachImgAll				= 0;
	m_nResetStatCounters			= 2;
	m_nShowDieCfg					= 0;
	m_nShowStats					= 0;
	m_nDefaultSettings				= 0;
	m_nPixelRuler					= 1;
	m_nMotionControl				= 1;	
	m_nOpticalPanel					= 1;
	m_nDryRun						= 0;
	m_nOutPutLog					= 0;

	m_nBinarize						= 0;
	m_nZoomIn						= 0;
	m_nZoomOut						= 0;
	m_nZoomFit						= 0;
	m_nZoom100						= 0;
	m_nZoomAll						= 0;
	m_nSetupGrid					= 0;
	m_nShowOverLay					= 0;
	m_nAuditTrail					= 0;
	m_nShowMaginifier				= 0;
	m_nSaveOnlineJpegImages			= 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMyStaticPropertyItemManger

CMyStaticPropertyItemManger::CMyStaticPropertyItemManger()
{
	//////////////////////////////////////////////////////////////////////////
	// Online	
	BEGIN_PROPERTY_TAB(_T("Online Mode"), true)
		PROPERTY_ITEM(ID_PD_ONLINE_OPENLOT, CPropertyItemListAccessLevel, _T("Open Lot"), true)
		PROPERTY_ITEM(ID_PD_ONLINE_CLOSELOT, CPropertyItemListAccessLevel, _T("Close Lot"), true)
		PROPERTY_ITEM(ID_PD_ONLINE_PRODMODE, CPropertyItemListAccessLevel, _T("OnLine Mode"), true)
		PROPERTY_ITEM(ID_PD_ONLINE_SAVEPASSIMG, CPropertyItemListAccessLevel, _T("Save Pass Images"), true)
		PROPERTY_ITEM(ID_PD_ONLINE_SAVEFAILIMG, CPropertyItemListAccessLevel, _T("Save Fail Images"), true)
		PROPERTY_ITEM(ID_PD_ONLINE_BYPASSRESULT, CPropertyItemListAccessLevel, _T("ByPass Result"), true)
		PROPERTY_ITEM(ID_PD_ONLINE_JPEGFORMAT, CPropertyItemListAccessLevel, _T("JPEG Format"), true)
		PROPERTY_ITEM(ID_PD_ONLINE_BMPFORMAT, CPropertyItemListAccessLevel, _T("BMP Format"), true)
		PROPERTY_ITEM(ID_PD_ONLINE_SAVEDLIMG, CPropertyItemListAccessLevel, _T("Save Deep Learning Images"), true)
		PROPERTY_ITEM(ID_PD_ONLINE_SAVEDLPASSIMG, CPropertyItemListAccessLevel, _T("Save Deep Learning Pass Images"), true)
		PROPERTY_ITEM(ID_PD_ONLINE_SAVEDLFAILIMG, CPropertyItemListAccessLevel, _T("Save Deep Learning Fail Images"), true)

	END_PROPERTY_TAB()

	//////////////////////////////////////////////////////////////////////////
	// Inspection
	BEGIN_PROPERTY_TAB(_T("Inspection"), true)
		PROPERTY_ITEM(ID_PD_INSPECTION_TEACH, CPropertyItemListAccessLevel, _T("Inspection Train"), true)
	    //PROPERTY_ITEM(ID_PD_INSPECTION_STEPSELECTION, CPropertyItemListAccessLevel, _T("Step Selection"), true)
		PROPERTY_ITEM(ID_PD_INSPECTION_STEP_INSPECT, CPropertyItemListAccessLevel, _T("Debug"), true)
		PROPERTY_ITEM(ID_PD_INSPECTION_INSPECT, CPropertyItemListAccessLevel, _T("Inspect"), true)
		PROPERTY_ITEM(ID_PD_INSPECTION_INSPSAVEIMG, CPropertyItemListAccessLevel, _T("Stored Images Inspection"), true)
		PROPERTY_ITEM(ID_PD_INSPECTION_SET_FOLDER, CPropertyItemListAccessLevel, _T("Auto Run"), true)
		PROPERTY_ITEM(ID_PD_INSPECTION_SAVEPASSIMG, CPropertyItemListAccessLevel, _T("Save Pass Images"), true)
		PROPERTY_ITEM(ID_PD_INSPECTION_SAVEFAILIMG, CPropertyItemListAccessLevel, _T("Save Fail Images"), true)
		PROPERTY_ITEM(ID_PD_INSPECTION_INSPLOOP, CPropertyItemListAccessLevel, _T("Inspect Loop"), true)
		PROPERTY_ITEM(ID_PD_INSPECTION_INSPECTCYCLE, CPropertyItemListAccessLevel, _T("Inspect Cycle"), true)
		//PROPERTY_ITEM(ID_PD_INSPECTION_CAMENABLE, CPropertyItemListAccessLevel, _T("Camera Active"), true)
		PROPERTY_ITEM(ID_PD_INSPECTION_INSPSEQ, CPropertyItemListAccessLevel, _T("Inspect Sequence"), true)
		PROPERTY_ITEM(ID_PD_INSPECTION_INSPSEQSAVEIMG, CPropertyItemListAccessLevel, _T("Save Inspect Sequence Images"), true)
		PROPERTY_ITEM(ID_PD_INSPECTION_INSPSEQLOOP, CPropertyItemListAccessLevel, _T("Inspect Sequence Loop"), true)
	END_PROPERTY_TAB()

	//////////////////////////////////////////////////////////////////////////
	// Camera and Lighting
	BEGIN_PROPERTY_TAB(_T("Camera and Lighting"), true)
		PROPERTY_ITEM(ID_PD_CAMLIGHT_SNAP, CPropertyItemListAccessLevel, _T("Snap Image"), true)
		PROPERTY_ITEM(ID_PD_CAMLIGHT_STREAM, CPropertyItemListAccessLevel, _T("Stream Image"), true)
		PROPERTY_ITEM(ID_PD_CAMLIGHT_LIGHTSET, CPropertyItemListAccessLevel, _T("Light Setting"), true)
		PROPERTY_ITEM(ID_PD_CAMLIGHT_SNAPALL, CPropertyItemListAccessLevel, _T("Snap All"), true)
	END_PROPERTY_TAB()

	//////////////////////////////////////////////////////////////////////////
	// Parameters
	BEGIN_PROPERTY_TAB(_T("Parameters"), true)
	
		PROPERTY_ITEM(ID_PD_PARM_VISION, CPropertyItemListAccessLevel, _T("Vision Parameters"), true)
		PROPERTY_ITEM(ID_PD_PARM_TEACH, CPropertyItemListAccessLevel, _T("Teach Parameters"), true)
		PROPERTY_ITEM(ID_PD_PARM_DEFECT, CPropertyItemListAccessLevel, _T("Defect Error Codes"), true)
		PROPERTY_ITEM(ID_PD_PARM_MANAGERECIPE, CPropertyItemListAccessLevel, _T("Manage Recipe"), true)
		PROPERTY_ITEM(ID_PD_PARM_CREATERECIPE, CPropertyItemListAccessLevel, _T("Create Recipe"), true)
		PROPERTY_ITEM(ID_PD_PARM_PVIDEFECT, CPropertyItemListAccessLevel, _T("Pvi Inspection Parameters"), true)
		PROPERTY_ITEM(ID_PD_PARM_OPT_DEVCONFG, CPropertyItemListAccessLevel, _T("Device Configuration"), true)
		PROPERTY_ITEM(ID_PD_PARM_OPT_TEACHCONFG, CPropertyItemListAccessLevel, _T("Sequence SetUp"), true)
		PROPERTY_ITEM(ID_PD_PARM_OPT_DEFECTCONFG, CPropertyItemListAccessLevel, _T("Defect Priority Selection"), true)
	
	END_PROPERTY_TAB()

	//////////////////////////////////////////////////////////////////////////
	// Tools 
	BEGIN_PROPERTY_TAB(_T("Tools"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_LOADIMG, CPropertyItemListAccessLevel, _T("Load Image"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_LOADIMG_ALL, CPropertyItemListAccessLevel, _T("Load All Images"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_LOADTEACHIMG, CPropertyItemListAccessLevel, _T("Load Teach Image"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_LOADTEACHIMG_ALL, CPropertyItemListAccessLevel, _T("Load All Teach Image"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_SAVEIMG, CPropertyItemListAccessLevel, _T("Save Image"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_SAVEIMG_ALL, CPropertyItemListAccessLevel, _T("Save All Image"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_RESETSTATS, CPropertyItemListAccessLevel, _T("Reset Stats"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_SHOWDIECONFIG, CPropertyItemListAccessLevel, _T("Show Die Mapping"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_SHOWSTATS, CPropertyItemListAccessLevel, _T("Show statistics"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_DEFAULTSETTINGS, CPropertyItemListAccessLevel, _T("Default Settings"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_PIXELRULER, CPropertyItemListAccessLevel, _T("Pixel Ruler"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_MOTION_CONTROL, CPropertyItemListAccessLevel, _T("Motion Control"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_OPTICS_PANEL, CPropertyItemListAccessLevel, _T("Focusing Tool"), true)
		PROPERTY_ITEM(ID_PD_INSPECTION_CALIBRATION, CPropertyItemListAccessLevel, _T("Calibration"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_HW_CAMERA, CPropertyItemListAccessLevel, _T("Camera Configuration"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_HW_IO, CPropertyItemListAccessLevel, _T("IO Configuration"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_DRYRUN, CPropertyItemListAccessLevel, _T("Dry Run"), true)
		PROPERTY_ITEM(ID_PD_TOOLS_OUTPUT_LOG, CPropertyItemListAccessLevel, _T("Output Log"), true)
	END_PROPERTY_TAB()

	BEGIN_PROPERTY_TAB(_T("View"), true)
		PROPERTY_ITEM(ID_PD_VIEW_BINARIZE, CPropertyItemListAccessLevel, _T("Binarize"), true)
		PROPERTY_ITEM(ID_PD_VIEW_ZOOM_IN, CPropertyItemListAccessLevel, _T("Zoom In"), true)
		PROPERTY_ITEM(ID_PD_VIEW_ZOOM_OUT, CPropertyItemListAccessLevel, _T("Zoom Out"), true)
		PROPERTY_ITEM(ID_PD_VIEW_ZOOM_FIT, CPropertyItemListAccessLevel, _T("Zoom Fit"), true)
		PROPERTY_ITEM(ID_PD_VIEW_ZOOM_100, CPropertyItemListAccessLevel, _T("Zoom 100%"), true)
		PROPERTY_ITEM(ID_PD_VIEW_ZOOM_ALL, CPropertyItemListAccessLevel, _T("Zoom All"), true)
		PROPERTY_ITEM(ID_PD_VIEW_SETUGRID, CPropertyItemListAccessLevel, _T("Setup Grid"), true)
		PROPERTY_ITEM(ID_PD_VIEW_SHOW_OVERLAY, CPropertyItemListAccessLevel, _T("Show Overlay"), true)
		PROPERTY_ITEM(ID_PD_VIEW_AUDIT_TRAIL, CPropertyItemListAccessLevel, _T("Audit Trail"), true)
		PROPERTY_ITEM(ID_PD_VIEW_SHOW_MAGINIFIER, CPropertyItemListAccessLevel, _T("Show Magnifier"), true)
		PROPERTY_ITEM(ID_PD_DUMMY, CPropertyItemString, _T(""), true)
		PROPERTY_ITEM(ID_PD_DUMMY, CPropertyItemString, _T(""), true)
		PROPERTY_ITEM(ID_PD_DUMMY, CPropertyItemString, _T(""), true)
		PROPERTY_ITEM(ID_PD_DUMMY, CPropertyItemString, _T(""), true)
	END_PROPERTY_TAB()
}

CMyStaticPropertyItemManger::~CMyStaticPropertyItemManger()
{
}

void CMyStaticPropertyItemManger::SetData(const CMyData& data)
{
	BEGIN_ITERATE_PROPERTY_ITEMS()

		SET_ITEM_LIST(ID_PD_ONLINE_OPENLOT, data.m_nOpenLot)
		SET_ITEM_LIST(ID_PD_ONLINE_CLOSELOT, data.m_nCloseLot)
		SET_ITEM_LIST(ID_PD_ONLINE_PRODMODE, data.m_nProdMode)
		SET_ITEM_LIST(ID_PD_ONLINE_SAVEPASSIMG, data.m_nSaveOnlinePassImages)
		SET_ITEM_LIST(ID_PD_ONLINE_SAVEFAILIMG, data.m_nSaveOnlineFailImages)
		SET_ITEM_LIST(ID_PD_ONLINE_BYPASSRESULT, data.m_nByPassResult)
		SET_ITEM_LIST(ID_PD_ONLINE_JPEGFORMAT, data.m_nSaveOnlineJpegImages)
		SET_ITEM_LIST(ID_PD_ONLINE_BMPFORMAT, data.m_nSaveOnlineBMPImages)

		SET_ITEM_LIST(ID_PD_ONLINE_SAVEDLIMG, data.m_nSaveOnlineDLImages)
		SET_ITEM_LIST(ID_PD_ONLINE_SAVEDLPASSIMG, data.m_nSaveOnlineDLPassImages)
		SET_ITEM_LIST(ID_PD_ONLINE_SAVEDLFAILIMG, data.m_nSaveOnlineDLFailImages)

		SET_ITEM_LIST(ID_PD_INSPECTION_TEACH, data.m_nTeach)
		SET_ITEM_LIST(ID_PD_INSPECTION_STEPSELECTION, data.m_nStepSelection)
		SET_ITEM_LIST(ID_PD_INSPECTION_STEP_INSPECT, data.m_nStepDebug)
		SET_ITEM_LIST(ID_PD_INSPECTION_INSPECT, data.m_nInspect)
		SET_ITEM_LIST(ID_PD_INSPECTION_INSPSAVEIMG, data.m_nInspectSavedImage)
		SET_ITEM_LIST(ID_PD_INSPECTION_SET_FOLDER, data.m_nSetFolder)
		SET_ITEM_LIST(ID_PD_INSPECTION_SAVEPASSIMG, data.m_nSaveOfflinePassImages)
		SET_ITEM_LIST(ID_PD_INSPECTION_SAVEFAILIMG, data.m_nSaveOfflineFailImages)
		SET_ITEM_LIST(ID_PD_INSPECTION_INSPLOOP, data.m_nInspectLoop)
		SET_ITEM_LIST(ID_PD_INSPECTION_INSPECTCYCLE, data.m_nInspectCycle)
		SET_ITEM_LIST(ID_PD_INSPECTION_CAMENABLE, data.m_nCamEnable)
		SET_ITEM_LIST(ID_PD_INSPECTION_INSPSEQ, data.m_nInspectionSeq)
		SET_ITEM_LIST(ID_PD_INSPECTION_CALIBRATION, data.m_nCalibSettings)
		SET_ITEM_LIST(ID_PD_INSPECTION_INSPSEQSAVEIMG, data.m_nInspSeqLoop)
		SET_ITEM_LIST(ID_PD_INSPECTION_INSPSEQLOOP, data.m_nSaveInspSeqImages)

		
		SET_ITEM_LIST(ID_PD_CAMLIGHT_SNAP, data.m_nGrab)
		SET_ITEM_LIST(ID_PD_CAMLIGHT_STREAM, data.m_nLive)
		SET_ITEM_LIST(ID_PD_CAMLIGHT_LIGHTSET, data.m_nLightSet)
		SET_ITEM_LIST(ID_PD_CAMLIGHT_SNAPALL, data.m_nGrabAll)

		SET_ITEM_LIST(ID_PD_PARM_VISION, data.m_nVisionParm)
		SET_ITEM_LIST(ID_PD_PARM_TEACH, data.m_nTeachParm)
		SET_ITEM_LIST(ID_PD_PARM_DEFECT, data.m_nDefectParm)
		SET_ITEM_LIST(ID_PD_PARM_MANAGERECIPE, data.m_nManageCfg)
		SET_ITEM_LIST(ID_PD_PARM_CREATERECIPE, data.m_nCreateConfig)
		SET_ITEM_LIST(ID_PD_PARM_PVIDEFECT, data.m_nLive)
		SET_ITEM_LIST(ID_PD_PARM_OPT_DEVCONFG, data.m_nDeviceCfg)
		SET_ITEM_LIST(ID_PD_PARM_OPT_TEACHCONFG, data.m_nTeachInspLoc)
		SET_ITEM_LIST(ID_PD_PARM_OPT_DEFECTCONFG, data.m_nDefectPriority)	

		SET_ITEM_LIST(ID_PD_TOOLS_LOADIMG, data.m_nLoadImg)
		SET_ITEM_LIST(ID_PD_TOOLS_LOADIMG_ALL, data.m_nLoadImgAll)
		SET_ITEM_LIST(ID_PD_TOOLS_LOADTEACHIMG, data.m_nLoadTeachImg)
		SET_ITEM_LIST(ID_PD_TOOLS_LOADTEACHIMG_ALL, data.m_nLoadTeachImgAll)
		SET_ITEM_LIST(ID_PD_TOOLS_SAVEIMG, data.m_nSaveImg)
		SET_ITEM_LIST(ID_PD_TOOLS_SAVEIMG_ALL, data.m_nSaveImgAll)
		SET_ITEM_LIST(ID_PD_TOOLS_RESETSTATS, data.m_nResetStatCounters)
		SET_ITEM_LIST(ID_PD_TOOLS_SHOWDIECONFIG, data.m_nShowDieCfg)
		SET_ITEM_LIST(ID_PD_TOOLS_SHOWSTATS, data.m_nShowStats)
		SET_ITEM_LIST(ID_PD_TOOLS_DEFAULTSETTINGS, data.m_nDefaultSettings)
		SET_ITEM_LIST(ID_PD_TOOLS_PIXELRULER, data.m_nPixelRuler)
		SET_ITEM_LIST(ID_PD_TOOLS_MOTION_CONTROL, data.m_nMotionControl)
		SET_ITEM_LIST(ID_PD_TOOLS_OPTICS_PANEL, data.m_nOpticalPanel)
		SET_ITEM_LIST(ID_PD_TOOLS_HW_CAMERA, data.m_nCameraCfg)
		SET_ITEM_LIST(ID_PD_TOOLS_HW_IO, data.m_nIOTest)
		SET_ITEM_LIST(ID_PD_TOOLS_DRYRUN, data.m_nDryRun)
		SET_ITEM_LIST(ID_PD_TOOLS_OUTPUT_LOG, data.m_nOutPutLog)

		SET_ITEM_LIST(ID_PD_VIEW_BINARIZE, data.m_nBinarize)
		SET_ITEM_LIST(ID_PD_VIEW_ZOOM_IN, data.m_nZoomIn)
		SET_ITEM_LIST(ID_PD_VIEW_ZOOM_OUT, data.m_nZoomOut)
		SET_ITEM_LIST(ID_PD_VIEW_ZOOM_FIT, data.m_nZoomFit)
		SET_ITEM_LIST(ID_PD_VIEW_ZOOM_100, data.m_nZoom100)
		SET_ITEM_LIST(ID_PD_VIEW_ZOOM_ALL, data.m_nZoomAll)
		SET_ITEM_LIST(ID_PD_VIEW_SETUGRID, data.m_nSetupGrid)
		SET_ITEM_LIST(ID_PD_VIEW_SHOW_OVERLAY, data.m_nShowOverLay)
		SET_ITEM_LIST(ID_PD_VIEW_AUDIT_TRAIL, data.m_nAuditTrail)
		SET_ITEM_LIST(ID_PD_VIEW_SHOW_MAGINIFIER, data.m_nShowMaginifier)
	END_ITERATE_PROPERTY_ITEMS()
}

void CMyStaticPropertyItemManger::GetData(CMyData& data) const
{
	BEGIN_ITERATE_PROPERTY_ITEMS()

		GET_ITEM_LIST(ID_PD_ONLINE_OPENLOT, data.m_nOpenLot)
		GET_ITEM_LIST(ID_PD_ONLINE_CLOSELOT, data.m_nCloseLot)
		GET_ITEM_LIST(ID_PD_ONLINE_PRODMODE, data.m_nProdMode)
		GET_ITEM_LIST(ID_PD_ONLINE_SAVEPASSIMG, data.m_nSaveOnlinePassImages)
		GET_ITEM_LIST(ID_PD_ONLINE_SAVEFAILIMG, data.m_nSaveOnlineFailImages)
		GET_ITEM_LIST(ID_PD_ONLINE_BYPASSRESULT, data.m_nByPassResult)
		GET_ITEM_LIST(ID_PD_ONLINE_JPEGFORMAT, data.m_nSaveOnlineJpegImages)
		GET_ITEM_LIST(ID_PD_ONLINE_BMPFORMAT, data.m_nSaveOnlineBMPImages)

		GET_ITEM_LIST(ID_PD_ONLINE_SAVEDLIMG, data.m_nSaveOnlineDLImages)
		GET_ITEM_LIST(ID_PD_ONLINE_SAVEDLPASSIMG, data.m_nSaveOnlineDLPassImages)
		GET_ITEM_LIST(ID_PD_ONLINE_SAVEDLFAILIMG, data.m_nSaveOnlineDLFailImages)


		GET_ITEM_LIST(ID_PD_INSPECTION_TEACH, data.m_nTeach)
		GET_ITEM_LIST(ID_PD_INSPECTION_STEPSELECTION, data.m_nStepSelection)
		GET_ITEM_LIST(ID_PD_INSPECTION_STEP_INSPECT, data.m_nStepDebug)
		GET_ITEM_LIST(ID_PD_INSPECTION_INSPECT, data.m_nInspect)
		GET_ITEM_LIST(ID_PD_INSPECTION_INSPSAVEIMG, data.m_nInspectSavedImage)
		GET_ITEM_LIST(ID_PD_INSPECTION_SET_FOLDER, data.m_nSetFolder)
		GET_ITEM_LIST(ID_PD_INSPECTION_SAVEPASSIMG, data.m_nSaveOfflinePassImages)
		GET_ITEM_LIST(ID_PD_INSPECTION_SAVEFAILIMG, data.m_nSaveOfflineFailImages)
		GET_ITEM_LIST(ID_PD_INSPECTION_INSPLOOP, data.m_nInspectLoop)
		GET_ITEM_LIST(ID_PD_INSPECTION_INSPECTCYCLE, data.m_nInspectCycle)
		GET_ITEM_LIST(ID_PD_INSPECTION_CAMENABLE, data.m_nCamEnable)
		GET_ITEM_LIST(ID_PD_INSPECTION_INSPSEQ, data.m_nInspectionSeq)
		GET_ITEM_LIST(ID_PD_INSPECTION_CALIBRATION, data.m_nCalibSettings)
		GET_ITEM_LIST(ID_PD_INSPECTION_INSPSEQSAVEIMG, data.m_nInspSeqLoop)
		GET_ITEM_LIST(ID_PD_INSPECTION_INSPSEQLOOP, data.m_nSaveInspSeqImages)
		
		GET_ITEM_LIST(ID_PD_CAMLIGHT_SNAP, data.m_nGrab)
		GET_ITEM_LIST(ID_PD_CAMLIGHT_STREAM, data.m_nLive)
		GET_ITEM_LIST(ID_PD_CAMLIGHT_LIGHTSET, data.m_nLightSet)
		GET_ITEM_LIST(ID_PD_CAMLIGHT_SNAPALL, data.m_nGrabAll)
		
		GET_ITEM_LIST(ID_PD_PARM_VISION, data.m_nVisionParm)
		GET_ITEM_LIST(ID_PD_PARM_TEACH, data.m_nTeachParm)
		GET_ITEM_LIST(ID_PD_PARM_DEFECT, data.m_nDefectParm)
		GET_ITEM_LIST(ID_PD_PARM_MANAGERECIPE, data.m_nManageCfg)
		GET_ITEM_LIST(ID_PD_PARM_CREATERECIPE, data.m_nCreateConfig)
		GET_ITEM_LIST(ID_PD_PARM_PVIDEFECT, data.m_nPviDefectParm)
		GET_ITEM_LIST(ID_PD_PARM_OPT_DEVCONFG, data.m_nDeviceCfg)
		GET_ITEM_LIST(ID_PD_PARM_OPT_TEACHCONFG, data.m_nTeachInspLoc)
		GET_ITEM_LIST(ID_PD_PARM_OPT_DEFECTCONFG, data.m_nDefectPriority)
		
		GET_ITEM_LIST(ID_PD_TOOLS_LOADIMG, data.m_nLoadImg)
		GET_ITEM_LIST(ID_PD_TOOLS_LOADIMG_ALL, data.m_nLoadImgAll)
		GET_ITEM_LIST(ID_PD_TOOLS_LOADTEACHIMG, data.m_nLoadTeachImg)
		GET_ITEM_LIST(ID_PD_TOOLS_LOADTEACHIMG_ALL, data.m_nLoadTeachImgAll)
		GET_ITEM_LIST(ID_PD_TOOLS_SAVEIMG, data.m_nSaveImg)
		GET_ITEM_LIST(ID_PD_TOOLS_SAVEIMG_ALL, data.m_nSaveImgAll)
		GET_ITEM_LIST(ID_PD_TOOLS_RESETSTATS, data.m_nResetStatCounters)
		GET_ITEM_LIST(ID_PD_TOOLS_SHOWDIECONFIG, data.m_nShowDieCfg)
		GET_ITEM_LIST(ID_PD_TOOLS_SHOWSTATS, data.m_nShowStats)
		GET_ITEM_LIST(ID_PD_TOOLS_DEFAULTSETTINGS, data.m_nDefaultSettings)
		GET_ITEM_LIST(ID_PD_TOOLS_PIXELRULER, data.m_nPixelRuler)
		GET_ITEM_LIST(ID_PD_TOOLS_MOTION_CONTROL, data.m_nMotionControl)
		GET_ITEM_LIST(ID_PD_TOOLS_OPTICS_PANEL, data.m_nOpticalPanel)
		GET_ITEM_LIST(ID_PD_TOOLS_HW_CAMERA, data.m_nCameraCfg)
		GET_ITEM_LIST(ID_PD_TOOLS_HW_IO, data.m_nIOTest)
		GET_ITEM_LIST(ID_PD_TOOLS_DRYRUN, data.m_nDryRun)
		GET_ITEM_LIST(ID_PD_TOOLS_OUTPUT_LOG, data.m_nOutPutLog)

		GET_ITEM_LIST(ID_PD_VIEW_BINARIZE, data.m_nBinarize)
		GET_ITEM_LIST(ID_PD_VIEW_ZOOM_IN, data.m_nZoomIn)
		GET_ITEM_LIST(ID_PD_VIEW_ZOOM_OUT, data.m_nZoomOut)
		GET_ITEM_LIST(ID_PD_VIEW_ZOOM_FIT, data.m_nZoomFit)
		GET_ITEM_LIST(ID_PD_VIEW_ZOOM_100, data.m_nZoom100)
		GET_ITEM_LIST(ID_PD_VIEW_ZOOM_ALL, data.m_nZoomAll)
		GET_ITEM_LIST(ID_PD_VIEW_SETUGRID, data.m_nSetupGrid)
		GET_ITEM_LIST(ID_PD_VIEW_SHOW_OVERLAY, data.m_nShowOverLay)
		GET_ITEM_LIST(ID_PD_VIEW_AUDIT_TRAIL, data.m_nAuditTrail)
		GET_ITEM_LIST(ID_PD_VIEW_SHOW_MAGINIFIER, data.m_nShowMaginifier)

	END_ITERATE_PROPERTY_ITEMS()
}

/////////////////////////////////////////////////////////////////////////////
// CMyAdaptablePropertyItemManager

CMyAdaptablePropertyItemManager::CMyAdaptablePropertyItemManager()
{
}

CMyAdaptablePropertyItemManager::~CMyAdaptablePropertyItemManager()
{
}

void CMyAdaptablePropertyItemManager::SetData(const CMyData& data)
{
}

void CMyAdaptablePropertyItemManager::GetData(CMyData& data) const
{
}

void CMyAdaptablePropertyItemManager::OnDataChanged(CPropertyItem* pPropertyItem, CPropertyListCtrl* pWndPropertyListCtrl, int nIndex)
{
}


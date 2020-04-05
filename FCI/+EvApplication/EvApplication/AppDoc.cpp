/// AppDoc.cpp : implementation of the CAppDoc class
/// Udupa; April'2012

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.

#include "Application.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include "AppView.h"
#include <direct.h>
#include "ImageQueueHandler.h"
#include "CameraCode.h"
#include "EvMainFrame.h"
#include "MainFrame.h"
#include "CameraSetupDialog.h"
#include "LightControlDlg.h"
#include "EdgeMatching.h"
#include "ColorConversion.h"
#include "SelectGoodBDlg.h"
#include "ImageSelectionDlg.h"
#include "StepSelection.h"
#include <propkey.h>
#include "StationMapping.h"
#include "CalibrationSettings.h"
#include "PVIInspectionParameters.h"
#include "MotionControlDialog.h"
#include "SelectFolder.h"
#include "namespace_utilities_function.h"

#include "CentralizedCommInterface.h"
#include "CentralizedCommManager.h"
#include "CentralizedCommMonitor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAppDoc

IMPLEMENT_DYNCREATE(CAppDoc, CEvImageDoc)

BEGIN_MESSAGE_MAP(CAppDoc, CEvImageDoc)
	ON_COMMAND(ID_INSPECTION_INSPECT, OnInspectionInspect)
	ON_UPDATE_COMMAND_UI(ID_INSPECTION_INSPECT, OnUpdateInspectionInspect)
	ON_COMMAND(ID_STATIC_LOADFROMFILE, OnToolsStaticimageLoadFromFile)
	ON_COMMAND(ID_STATIC_LOADTEACHIMAGE, OnToolsStaticimageLoadTeachImage)
	ON_COMMAND(ID_STATIC_SAVE, OnToolsStaticimageSave)
	ON_UPDATE_COMMAND_UI(ID_STATIC_SAVE, OnUpdateToolsStaticimageSave)
	ON_UPDATE_COMMAND_UI(ID_STATIC_LOADFROMFILE, OnUpdateToolsStaticimageLoadFromFile)
	ON_UPDATE_COMMAND_UI(ID_STATIC_LOADTEACHIMAGE, OnUpdateToolsStaticimageLoadTeachImage)
	ON_COMMAND(ID_TOOLS_STATICIMAGE, OnToolsStaticimage)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_STATICIMAGE, OnUpdateToolsStaticimage)
	ON_COMMAND(ID_INSPECTION_TEACH, OnInspectionTeach)
	ON_UPDATE_COMMAND_UI(ID_INSPECTION_TEACH, OnUpdateInspectionTeach)
	ON_COMMAND(ID_INSPECTION_TEACH_SINGLE_IMAGE, OnInspectionTeachSingleImage)
	ON_UPDATE_COMMAND_UI(ID_INSPECTION_TEACH_SINGLE_IMAGE, OnUpdateInspectionTeachSingleImage)
	ON_COMMAND(ID_INSPECTION_DEBUG, OnInspectionDebug)
	ON_UPDATE_COMMAND_UI(ID_INSPECTION_DEBUG, OnUpdateInspectionDebug)
	ON_UPDATE_COMMAND_UI(ID_INSPECTION_NEXT, OnUpdateInspectionNext)
	ON_COMMAND(ID_INSPECTION_NEXT, OnInspectionNext)
	ON_COMMAND(ID_INSPECTION_ABORT, OnInspectionAbort)
	ON_UPDATE_COMMAND_UI(ID_INSPECTION_ABORT, OnUpdateInspectionAbort)
	ON_COMMAND(ID_CAMERA_SNAP, OnCameraSnap)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_SNAP, OnUpdateCameraSnap)
	ON_COMMAND(ID_CAMERA_STREAM, OnCameraStream)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_STREAM, OnUpdateCameraStream)
	ON_COMMAND(ID_CAMERA_LIGHT, OnCameraLight)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_LIGHT, OnUpdateCameraLight)
	ON_COMMAND(ID_PARM_VISION, OnParmVision)
	ON_UPDATE_COMMAND_UI(ID_PARM_VISION, OnUpdateParmVision)
	ON_COMMAND(ID_PARM_TEACH, OnParmTeach)
	ON_UPDATE_COMMAND_UI(ID_PARM_TEACH, OnUpdateParmTeach)
//	ON_COMMAND(ID_PARM_DEFECT, OnParmDefect)
//	ON_UPDATE_COMMAND_UI(ID_PARM_DEFECT, OnUpdateParmDefect)
	ON_COMMAND(ID_INSPECTION_STOREDIMAGES, OnInspectionStoredimages)
	ON_UPDATE_COMMAND_UI(ID_INSPECTION_STOREDIMAGES, OnUpdateInspectionStoredimages)
	ON_COMMAND(ID_INSPECTION_CYCLE, OnInspectionCycle)
	ON_UPDATE_COMMAND_UI(ID_INSPECTION_CYCLE, OnUpdateInspectionCycle)
	ON_COMMAND(ID_INSPECTION_CAMERAACTIVE, OnInspectionCameraactive)
	ON_UPDATE_COMMAND_UI(ID_INSPECTION_CAMERAACTIVE, OnUpdateInspectionCameraactive)
	ON_COMMAND(ID_INSPECTION_CYCLEAUTOSTEPMODE, OnInspectCycleAutoStepMode)
	ON_UPDATE_COMMAND_UI(ID_INSPECTION_CYCLEAUTOSTEPMODE, OnUpdateInspectCycleAutoStepMode)
	ON_COMMAND(ID_INSPECTION_SETSTOREDIMAGEFOLDER, OnInspectStoredImagesPath)
	ON_UPDATE_COMMAND_UI(ID_INSPECTION_SETSTOREDIMAGEFOLDER, OnUpdateInspectStoredImagesPath)
	ON_COMMAND(ID_INSPECTION_SAVEPASSIMAGES, OnInspectSavePassImages)
	ON_UPDATE_COMMAND_UI(ID_INSPECTION_SAVEPASSIMAGES, OnUpdateInspectSavePassImages)
	ON_COMMAND(ID_INSPECTION_SAVEFAILIMAGES1, OnInspectSaveFailImages)
	ON_UPDATE_COMMAND_UI(ID_INSPECTION_SAVEFAILIMAGES1, OnUpdateInspectSaveFailImages)
	ON_COMMAND(ID_PARM_HARDWARE_CAMERA, OnCameraParmButton)
	ON_UPDATE_COMMAND_UI(ID_PARM_HARDWARE_CAMERA, OnUpdateCameraParmButton)
	ON_COMMAND(ID_VIEWOPTIONS_SETUPGRID, OnViewOptionsSetupGrid)
	ON_UPDATE_COMMAND_UI(ID_VIEWOPTIONS_SETUPGRID, OnUpdateViewOptionsSetupGrid)
	ON_COMMAND(ID_VIEWOPTIONS_SHOWOVERLAY, OnViewOptionsShowOverlay)
	ON_UPDATE_COMMAND_UI(ID_VIEWOPTIONS_SHOWOVERLAY, OnUpdateViewOptionsShowOverlay)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPTIONS, OnUpdateViewOptions)
	ON_COMMAND(ID_FOCUSING_TOOL, OnViewOpticsPanel)
	ON_UPDATE_COMMAND_UI(ID_FOCUSING_TOOL, OnUpdateViewOpticsPanel)
	ON_COMMAND(ID_VIEW_OPTICS_ROI, OnViewOpticsRoi)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPTICS_ROI, OnUpdateViewOpticsRoi)
	ON_COMMAND(ID_VIEWOPTIONS_SELECTGOODB, OnViewSelectGoodB)
	ON_UPDATE_COMMAND_UI(ID_VIEWOPTIONS_SELECTGOODB, OnUpdateViewSelectGoodB)
	ON_CBN_SELCHANGE(IDC_FRAMECOUNT_COMBO, OnSelchangeFramecountCombo)

	ON_COMMAND(ID_STEPDEBUG_SELECTION, OnInspectStepDebugSelection)
	ON_UPDATE_COMMAND_UI(ID_STEPDEBUG_SELECTION, OnUpdateInspectStepDebugSelection)

	ON_COMMAND(ID_PVI_DEFECT_SHOW, OnPviDefectShow)
	ON_UPDATE_COMMAND_UI(ID_PVI_DEFECT_SHOW, OnUpdatePviDefectShow)

	ON_COMMAND(ID_BUTTON_CALIBRATION_SETTINGS, OnCalibrationSettings)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_CALIBRATION_SETTINGS, OnUpdateCalibrationSettings)
	//ON_MESSAGE(WM_OPTICS_FRAMECHANGED, OnSelchangeFramecountCombo)

	ON_COMMAND(ID_STATIC_LOADTEACHIMAGE_SINGLE, OnToolsStaticimageLoadTeachImageSingle)
	ON_UPDATE_COMMAND_UI(ID_STATIC_LOADTEACHIMAGE_SINGLE, OnUpdateToolsStaticimageLoadTeachImageSingle)
	ON_COMMAND(ID_LOAD_ALL_IMG_FILES_BUTTON, OnToolsStaticimageLoadAllFromFile)
	ON_UPDATE_COMMAND_UI(ID_LOAD_ALL_IMG_FILES_BUTTON, OnUpdateToolsStaticimageLoadAllFromFile)
	ON_COMMAND(ID_SAVE_ALL_BUTTON, OnToolsStaticimageSaveAll)
	ON_UPDATE_COMMAND_UI(ID_SAVE_ALL_BUTTON, OnUpdateToolsStaticimageSaveAll)

	ON_COMMAND(ID_INSP_ALL_BUTTON, OnInspectionInspectAll)
	ON_UPDATE_COMMAND_UI(ID_INSP_ALL_BUTTON, OnUpdateInspectionInspectAll)

	ON_COMMAND(ID_CAMERA_SNAP_ALL_BUTTON, OnCameraSnapAll)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_SNAP_ALL_BUTTON, OnUpdateCameraSnapAll)

END_MESSAGE_MAP()


// CAppDoc construction/destruction

CAppDoc::CAppDoc()
{
	m_ImageViewInfo.sizeImage.cx = 1024;
	m_ImageViewInfo.sizeImage.cy = 1024;

	m_pApp = NULL;
	m_pOpticsDialog = NULL;
	
	m_strConfigDir.Empty();

	m_nInspCounter = 0;
	m_pTrack = NULL;
	m_bInspSavedImage = FALSE;
	m_bSavedImagesStep = FALSE;
	m_strLoadImage.Empty();

	///m_pContactLensInspWnd = NULL;
	m_bStoredImages = FALSE;
	m_bInspectCycle = FALSE;
	m_bCycleAutoStepMode = FALSE;
	m_bCameraActive = TRUE;
	m_bInspecting = FALSE;
	

	m_bSetupGrid = FALSE;
	m_pColorConvertor = NULL;
	m_nDefectCount = 0;
	m_pRemoteInspDoc = NULL;
	m_nCalibLightGreyValue = 0;
	bIsFirstLightCalibration = FALSE;
	m_CalibTolerence =	1;
	m_bLiveImageSingle = FALSE;
	m_bInspectionDebug = FALSE;
	m_bExitLogEnry = FALSE;

	m_nCalibStepXCount = 4;
	m_nCalibStepYCount = 4;
	m_dCalibStepXSize = 1;
	m_dCalibStepYSize = 1;

}

CAppDoc::~CAppDoc()
{

	//if(m_hTrackImageUpdate) {
	//	//SetEvent(m_EventImageUpdate);
	//	if(WaitForSingleObject(m_hTrackImageUpdate, 500)!=WAIT_OBJECT_0)
	//		TerminateThread(m_hTrackImageUpdate, 0);
	//	m_hTrackImageUpdate = NULL;
	//	CloseHandle(m_EventImageUpdate);
	//}

	if(m_pOpticsDialog != NULL) {
		if(IsWindow(m_pOpticsDialog->m_hWnd))
			DestroyWindow(m_pOpticsDialog->m_hWnd);
		delete m_pOpticsDialog;
	}

	if(m_pColorConvertor != NULL)
		delete m_pColorConvertor;
}

BOOL CAppDoc::OnNewDocument()
{
	if(!CEvImageDoc::OnNewDocument())
		return FALSE;

	m_pApp = (CApplication*) AfxGetApp();
	if(m_pApp == NULL) {
		ASSERT(!"Failed to get Application reference");
		return FALSE;
	}

	return TRUE;
}




// CAppDoc serialization

void CAppDoc::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CAppDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CAppDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CAppDoc::SetSearchContent(const CString& value)
{
	if(value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if(pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CAppDoc diagnostics

#ifdef _DEBUG
void CAppDoc::AssertValid() const
{
	CEvImageDoc::AssertValid();
}

void CAppDoc::Dump(CDumpContext& dc) const
{
	CEvImageDoc::Dump(dc);
}
#endif //_DEBUG


// CAppDoc commands


BOOL CAppDoc::SetParameters(CTrackHandler* pTrack, int nDoc)
{
	m_pTrack = pTrack;
	m_nDocIdx = nDoc;

	m_EventInspEnd = CreateEventA(NULL, FALSE, FALSE, NULL);

	return TRUE;
}

void CAppDoc::InitializeDocView()
{
	CSize szImgSize;
	int nTrigType;
	CRect RectROI;
	CString str;
	str.Format("DebugFlag C%d", m_pTrack->m_nTrack+1);
	//m_lDebugFlag = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", str, (int)m_lDebugFlag);
	m_lDebugFlag = DEBUG_DRAW|DEBUG_PRINT|DEBUG_TIME;
	str.Format("ThresholdLow %d_%d", m_pTrack->m_nTrack+1, m_nDocIdx+1);
	m_nThreshold.nLow = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", str, 128);
	str.Format("ThresholdHigh %d_%d", m_pTrack->m_nTrack + 1, m_nDocIdx + 1);
	m_nThreshold.nHigh = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", str, 255);
	//m_pApp->m_nUseCameraResolution = GetRegistryEntry(READ_REGPATH + "Preferences", "UseCameraResolution", m_pApp->m_nUseCameraResolution);
	
	//m_pTrack->LoadCameraParm();

	if(m_pTrack->m_FGResource.pCamModule) {
		GetResolution(&m_pTrack->m_FGResource, &szImgSize);
		//GetCameraAoi(&m_pTrack->m_FGResource,&RectROI);
		/*szImgSize = RectROI.Size();*/
		GetTrigMode(&m_pTrack->m_FGResource,(int*)&nTrigType);
		if(nTrigType==TRIG_NONE) {
			m_pTrack->m_bCamTrigAvail = FALSE;
			m_pTrack->m_bCamExtTrigEnable = FALSE;
		}
		else if(nTrigType==TRIG_SOFT) {
			m_pTrack->m_bCamTrigAvail = TRUE;
			m_pTrack->m_bCamExtTrigEnable = FALSE;
		}
		else if(nTrigType==TRIG_HARD) {
			m_pTrack->m_bCamTrigAvail = TRUE;
			m_pTrack->m_bCamExtTrigEnable = TRUE;
		}
		else {
			AfxMessageBox("Error in setting TrigMode");
		}
		SetTrigMode(&m_pTrack->m_FGResource, TRIG_SOFT);
	}
	else
		szImgSize = m_pTrack->m_rectAoi.Size();

	OutputDebugLog("Size x = %d  y = %d",szImgSize.cx,szImgSize.cy);

	m_bIsColor = 0;
	

	

	ResizeDoc(szImgSize);
	//SetDocSize(szImgSize);
	InitViewBuffer(&m_Buffer);///
	///LoadConfiguration();

	m_bufPVIBackup.Allocate(m_Buffer.GetImgSize());
	m_bufPVIBackup.ClrImg();

	if(m_bIsColor)
		m_pColorConvertor = new CConvertRawY8BGGR();

	m_bAutoResizeImage = m_pApp->m_bAutoResizeImage;
	m_bAutoResizeImageToMidRes = m_pApp->m_bAutoResizeImageToMidRes;

	CRect rect = CRect(CPoint(77,85), CSize(35,35));
	m_InspTickPos1[0] = rect.TopLeft();
	m_InspTickPos1[0].x += rect.Width()/2;
	m_InspTickPos1[0].y -= 35;
	m_InspTickPos2[0] = m_InspTickPos1[0];
	m_InspTickPos2[0].y += rect.Height() + 70;

	m_InspTickPos1[1] = rect.TopLeft();
	m_InspTickPos1[1].Offset(-20, -20);
	m_InspTickPos2[1] = rect.BottomRight();
	m_InspTickPos2[1].Offset(20, 20);
	int temp = m_InspTickPos2[1].x;
	m_InspTickPos2[1].x = m_InspTickPos1[1].x;
	m_InspTickPos1[1].x = temp;

	m_InspTickPos1[2] = rect.TopLeft();
	m_InspTickPos1[2].x -= 35;
	m_InspTickPos1[2].y += rect.Height()/2;
	m_InspTickPos2[2] = m_InspTickPos1[2];
	m_InspTickPos2[2].x += rect.Width() + 70;

	m_InspTickPos1[3] = rect.TopLeft();
	m_InspTickPos1[3].Offset(-20, -20);
	m_InspTickPos2[3] = rect.BottomRight();
	m_InspTickPos2[3].Offset(20, 20);


	//m_EventImageUpdate = CreateEventA(NULL, FALSE, FALSE, NULL);
	m_EventLogInspectionData = CreateEventA(NULL, TRUE, FALSE, NULL);
	//m_hTrackImageUpdate = AfxBeginThread(TrackImageUpdate, this,THREAD_PRIORITY_LOWEST);
	AfxBeginThread(LogInspectionData, this, THREAD_PRIORITY_LOWEST);
	m_EventKillLiveSingle = CreateEventA(NULL, FALSE, FALSE, NULL);

	int nBlockX, nBlockY;
	int nDiePerBlockX, nDiePerBlockY;

	m_pApp->motionParameters.GetMappingParameters(nBlockX, nBlockY, nDiePerBlockX, nDiePerBlockY);
	int nTotalFov = nDiePerBlockX* nDiePerBlockY;

	for(int nFOV=0; nFOV< nTotalFov; nFOV++) {
		//m_pTrack->m_nXAxisPosition[nFOV] = 0;
		//m_pTrack->m_nYAxisPosition[nFOV] = 0;
//		m_nZAxisPosition[nFOV] = 0;
		//if (nFOV < m_pApp->m_nNoOfFov) {
			m_TeachBuffer[nFOV].Allocate(m_Buffer.GetImgSize());
			m_TeachBuffer[nFOV].ClrImg();
			//m_InspectionHandler[nFOV].Initialize(m_pTrack->m_nTrack);
			if (m_pApp->m_bIsMasterApp) {
				CInspection* pInsp = &m_InspectionHandler[nFOV].m_Inspection;
				pInsp->Initialize(m_pTrack->m_nTrack, m_pApp->m_bInspectionParallelism);
			}
		//}
	}
	
	
	m_nZAxisPosition.resize(nDiePerBlockX * nDiePerBlockY);
	m_bEnableGrab.resize(nDiePerBlockX * nDiePerBlockY);
	std::fill(m_nZAxisPosition.begin(), m_nZAxisPosition.end(), 0);
	std::fill(m_bEnableGrab.begin(), m_bEnableGrab.end(), false);

//	m_InspectionHandler[m_pTrack->m_nFOVIndex].Initialize(m_pTrack->m_nTrack);
	int nTrack = -1;
//	CInspection* pInsp = &m_InspectionHandler[m_pTrack->m_nFOVIndex].m_Inspection;
//	pInsp->Initialize(m_pTrack->m_nTrack, m_pApp->m_bInspectionParallelism);

	m_bLightCalibration = FALSE;
	//m_pApp->m_ThreadManager.m_nInspThreadIndex = nTrack;
	//for(int i=0; i<THREAD_COUNT; i++)
	//	pInsp->m_ThreadParms[i].m_hThread = m_pApp->m_ThreadManager.CreateThread(InspectionThread, pInsp, INSPECTION_THREAD, nTrack);
}

void CAppDoc::ResetDoc()
{
	int nBlockX, nBlockY;
	int nDiePerBlockX, nDiePerBlockY;

	m_pApp->motionParameters.GetMappingParameters(nBlockX, nBlockY, nDiePerBlockX, nDiePerBlockY);
	int nTotalFov = nDiePerBlockX* nDiePerBlockY;
	m_nZAxisPosition.resize(nDiePerBlockX * nDiePerBlockY);
	m_bEnableGrab.resize(nDiePerBlockX * nDiePerBlockY);
	std::fill(m_nZAxisPosition.begin(), m_nZAxisPosition.end(), 0);
	std::fill(m_bEnableGrab.begin(), m_bEnableGrab.end(), false);

	int nNoOfDies = nBlockX * nBlockY;
	int nNoOfFov = nDiePerBlockX * nDiePerBlockY;
	if (!m_pApp->m_bIsMasterApp) {
		nNoOfDies = MAX_DIES;
		nNoOfFov = MAX_FOV;
	}

	CSize szImage = m_Buffer.GetImgSize();
	for (int i = 0; i < nNoOfDies; i++) {
		for (int j = 0; j < nNoOfFov; j++) {
			if (!m_BufferDie[i][j].IsAllocated()) {
				m_BufferDie[i][j].Allocate(szImage, false, m_bIsColor);
			}
			m_BufferDie[i][j].ClrImg();

			//Buffer Die for Centralized Reject PC
			if (m_pApp->m_CentralizedCommInterface.m_bIsTCPServer) {
				for (int iPort = 0; iPort < NO_OF_PORT_CENTRALIZED; iPort++) {
					if (!m_CentralizedBufferDie[iPort][i][j].IsAllocated()) {
						m_CentralizedBufferDie[iPort][i][j].Allocate(szImage, false, m_bIsColor);
					}
					m_CentralizedBufferDie[iPort][i][j].ClrImg();
				}
			}
		}
	}
}

void CAppDoc::ResetDocServer()
{
	int nBlockX, nBlockY;
	int nDiePerBlockX, nDiePerBlockY;

	m_pApp->motionParameters.GetMappingParameters(nBlockX, nBlockY, nDiePerBlockX, nDiePerBlockY);
	int nTotalFov = nDiePerBlockX * nDiePerBlockY;
	m_nZAxisPosition.resize(nDiePerBlockX * nDiePerBlockY);
	m_bEnableGrab.resize(nDiePerBlockX * nDiePerBlockY);
	std::fill(m_nZAxisPosition.begin(), m_nZAxisPosition.end(), 0);
	std::fill(m_bEnableGrab.begin(), m_bEnableGrab.end(), false);

	/*CSize szImage = m_Buffer.GetImgSize();
	for (int i = 0; i < nBlockX * nBlockY; i++) {
		for (int j = 0; j < nDiePerBlockX * nDiePerBlockY; j++) {
			if (!m_BufferDie[i][j].IsAllocated()) {
				m_BufferDie[i][j].Allocate(szImage, false, m_bIsColor);
				m_BufferDie[i][j].ClrImg();
			}
		}
	}*/
}

int CAppDoc::ResizeDoc(CSize szImage)
{
	if(!m_pbufImage || !m_pbufImage->IsAllocated() || (szImage.cx != m_pbufImage->GetImgSize().cx) || (szImage.cy != m_pbufImage->GetImgSize().cy)) {

		int nDevicePerX, nDevicePerY;
		int nFovPerX, nFovPerY;
		m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);


		m_Buffer.Allocate(szImage, true, m_bIsColor);
		m_Buffer.ClrImg();
		InitViewBuffer(&m_Buffer);

		int nNoOfDies = nDevicePerX * nDevicePerY;
		int nNoOfFov = nFovPerX * nFovPerY;
		if (!m_pApp->m_bIsMasterApp) {
			nNoOfDies = MAX_DIES;
			nNoOfFov = MAX_FOV;
		}

		for (int i = 0; i < nNoOfDies; i++) {
			for (int j = 0; j < nNoOfFov; j++) {
				m_BufferDie[i][j].Allocate(szImage, false, m_bIsColor);
				m_BufferDie[i][j].ClrImg();

				//Buffer Die for Centralized Reject PC
				for (int iPort = 0; iPort < NO_OF_PORT_CENTRALIZED; iPort++) {
					m_CentralizedBufferDie[iPort][i][j].Allocate(szImage, false, m_bIsColor);
					m_CentralizedBufferDie[iPort][i][j].ClrImg();
				}
			}
		}
		if(m_bIsColor) {
			m_BufSource.Allocate(szImage, false, TRUE);
			m_BufSource.ClrImg();
		}
		if(m_pApp->m_bAllocFailedImageBuffer){
			for(int i=0;i<25;i++) {
				m_FailStack[i].bufFailImage.Allocate(m_Buffer.GetImgSize(), false, m_bIsColor);
				m_FailStack[i].bufFailImage.ClrImg();
			}
			OutputDebugLog("FailedImageBuffer Allocated");
		}

		SetDocSize(szImage);
		ZoomView(ZOOMFIT);
	}

	return 0;
}

void CAppDoc::CameraLive()
{
	if(m_pTrack->m_bLiveImage) {
		SetEvent(m_pTrack->m_EventKillLive);		
		Sleep(100);
	}
	else {
		m_pTrack->m_bLiveImage = TRUE;
		AfxBeginThread(TrackLiveImage, this, THREAD_PRIORITY_LOWEST);
	}
}

void CAppDoc::CamLiveSingle()
{
	if (m_bLiveImageSingle) {
		SetEvent(m_EventKillLiveSingle);
		Sleep(100);
	}
	else {
		m_bLiveImageSingle = TRUE;
		AfxBeginThread(TrackLiveImageSingle, this, THREAD_PRIORITY_LOWEST);
	}
}

void CAppDoc::CameraLiveCalibration()
{
	if(m_pTrack->m_bLiveCalibration) {
		SetEvent(m_pTrack->m_EventKillLiveCalibration);
		Sleep(100);
	}
	else {
		m_pTrack->m_bLiveCalibration = TRUE;
		AfxBeginThread(TrackLightCalibrationLiveImage, this, THREAD_PRIORITY_LOWEST);
	}
}

int CAppDoc::CopyGrabbedBuffer(CImageBuffer* pGrabbedBuffer)
{
	//CPerformanceTimer timer;
	int nXSize = __min(m_pbufImage->GetImgSize().cx, pGrabbedBuffer->GetImgSize().cx);
	int nYSize = __min(m_pbufImage->GetImgSize().cy, pGrabbedBuffer->GetImgSize().cy);
	
	if(m_bIsColor) {
		CPerformanceTimer timer;
		timer.Reset();
		EnterCriticalSection(&m_csDisplayBuffer);
		CImageBuffer* pBuf = m_bBinaryMode ? &m_BufSource : &m_Buffer;
		m_pColorConvertor->ConvertRawY8BGGREx(
			nXSize, nYSize,
			pGrabbedBuffer->GetImgPtr(),
			pBuf->GetImgPtr());
		if(m_bBinaryMode) {
			CRect rcFOV = CRect(CPoint(0,0), m_Buffer.GetImgSize());
			ImgCopy(&m_BufSource, &rcFOV, &m_Buffer, &rcFOV);
		}
		LeaveCriticalSection(&m_csDisplayBuffer);
		OutputDebugLogTo(1, TRUE, "Color Conversion Time: %5.1fms", timer.GetElapsedMSec());
	}
	else {
		CRect rect = CRect(0, 0, nXSize, nYSize);
		ImgCopy(pGrabbedBuffer, &rect, m_pbufImage, &rect);
	}

	//OutputDebugLogTo(2+m_pTrack->m_nTrack/5, TRUE, "CopyGrabbedBuffer time: %5.1fms", timer.GetElapsedMSec());
	OutputDebugLogTo(3+m_pTrack->m_nTrack, TRUE, "[Image %d] CopyGrabbedBuffer", m_nDocIdx+1);

	return 0;
}

int CAppDoc::CameraGrab(BOOL bDisplay, BOOL bSilent)
{
	int nError;

	if(bDisplay)
		m_pTrack->m_nOpticsRecalcMode = 1;

	if(m_pApp->m_bLightCtrlDlgOpen || m_bDotCalibration)
	{
		nError = Grab(m_pTrack->m_pDoc[m_nDocIdx], bDisplay);
		if(nError) {
			m_pTrack->m_nOpticsRecalcMode = 0;
			if(!bSilent && !m_pApp->m_bOnline)
				OutputDebugLog("********* Image %d Snap failed", m_nDocIdx+1);
			return IMAGE_SNAP_FAIL;
		}
		else if(!m_pApp->m_bOnline)
			SetImageTitle(NULL, FALSE);
		m_pApp->m_bCalib = FALSE;
	}
	else
	{
		CPerformanceTimer timer;
		for(int k=0; k</*m_pApp->m_nTotalViews*/m_pTrack->m_nTotalDocs; k++){
			timer.Reset();
			m_pTrack->RefreshLight(k);
			OutputDebugLog("Light Refresh[%d] = %5.1fms",k,timer.GetElapsedMSec());	
			timer.Reset();
			nError = Grab(m_pTrack->m_pDoc[k], bDisplay);
			OutputDebugLog("Grab Refresh[%d] = %5.1fms",k,timer.GetElapsedMSec());	
			if(nError) {
				m_pTrack->m_nOpticsRecalcMode = 0;
				if(!bSilent && !m_pApp->m_bOnline)
					OutputDebugLog("********* Image %d Snap failed",k);
				return IMAGE_SNAP_FAIL;
			}

			else if(!m_pApp->m_bOnline)
				SetImageTitle(NULL, FALSE);
		}
	}
	return 0;
}

int CAppDoc::CameraGrabUsingExposure(BOOL bDisplay, BOOL bSilent,BOOL bMoveSequence, BOOL bGrabAllImages)
{
	int nError = 0;
	//CPerformanceTimer timer;

	int nDocCount = m_pApp->m_nTotalViewDocs[m_pTrack->m_nTrack];
	for(int nDoc=0; nDoc<nDocCount; nDoc++) {
		
		if (!m_pApp->motionParameters.IsGrabEnabled(m_pTrack->m_nTrack, nDoc, m_pTrack->m_nFOVIndex) && !bGrabAllImages)
			continue;

		if (bDisplay)
			m_pTrack->m_nOpticsRecalcMode = 1;
	
		//timer.Reset();
		DrwDel(&m_pTrack->m_pDoc[nDoc]->m_ImageViewInfo);
		DrwUpdate(&m_pTrack->m_pDoc[nDoc]->m_ImageViewInfo);

		if(!m_pApp->m_bLightCtrlDlgOpen) {
			if((m_pApp->m_bInspSequenceOffline || m_pApp->m_bOnline || bMoveSequence) && (m_pTrack->m_nTrack != STATION_SIDE)) {
				m_pTrack->SetZPositionForSequence(nDoc);
				nError = 0;//m_pApp->WaitForZPositionEnd(/*m_pTrack->m_nTrack*/);
			}
			m_pTrack->RefreshLight(nDoc);
		}

		OutputDebugLogTo(3+m_pTrack->m_nTrack, TRUE, "Light Refresh [T%d]", m_pTrack->m_nTrack+1);	
		nError |= m_pTrack->EnqueueAsyncGrab();
		if(!nError) {
			m_pTrack->UpdateGrabImage(nDoc);
			//if(!m_pTrack->m_nTrack)
			//	m_pTrack->WaitForEndOfExposure();
		}
		else
			m_pTrack->m_nOpticsRecalcMode = 0;
	}

	if(m_pApp->m_bSnap)
		m_pApp->m_bSnap = FALSE;

	if(nError) {
		if(!m_pApp->m_bOnline)
			OutputDebugLog("********* Image Snap failed");
		return IMAGE_SNAP_FAIL;
	}
	else if(!m_pApp->m_bOnline)
		SetImageTitle(NULL, FALSE);

	return 0;
}

int CAppDoc::CameraGrabSingle(BOOL bDisplay, BOOL bSilent)
{
	int nError = 0;
	if (bDisplay)
		m_pTrack->m_nOpticsRecalcMode = 1;

	if (!m_pApp->m_bLightCtrlDlgOpen) {
		if ((m_pApp->m_bLightCalibration || m_pApp->m_bDotCalibration ||m_pApp->m_bInspSequenceOffline || m_pApp->m_bOnline) && (m_pTrack->m_nTrack != STATION_SIDE)) 
			m_pTrack->SetZPositionForSequence(m_nDocIdx);
		
		//m_pTrack->RefreshLight(m_nDocIdx);
	}
	if (!m_pTrack->m_bOpticsRoi) {
		DrwDel(&m_ImageViewInfo);
		DrwUpdate(&m_ImageViewInfo);
	}
	
	nError |= m_pTrack->EnqueueAsyncGrab();
	if (!nError) {
		m_pTrack->UpdateGrabImage(m_nDocIdx);
	}
	else
		m_pTrack->m_nOpticsRecalcMode = 0;
	
	if (m_pApp->m_bSnap)
		m_pApp->m_bSnap = FALSE;

	if (nError) {
		if (!m_pApp->m_bOnline)
			OutputDebugLog("********* Image Snap failed");
		return IMAGE_SNAP_FAIL;
	}
	else if (!m_pApp->m_bOnline)
		SetImageTitle(NULL, FALSE);

	return 0;
}

int CAppDoc::CopyGrabToDisplay(BOOL bDisplayToBuffer)//JP
{
	//CSize szImg = m_Buffer.GetImgSize();
	//CRect rcImg = CRect(CPoint(0,0) , szImg);
	//if(bDisplayToBuffer)
	//	ImgCopy(&m_DisplayBuffer, &rcImg, &m_Buffer, &rcImg);	//offline mode
	//else
	//	ImgCopy(&m_Buffer, &rcImg, &m_DisplayBuffer, &rcImg);	//online mode

	///*SaveGrayScaleBMPFile("E:\\BufferImage.bmp",m_Buffer);
	//SaveGrayScaleBMPFile("E:\\DisplayImage.bmp",m_DisplayBuffer);*/
	return 0;
}

int CAppDoc::LoadConfigurationServer()
{
	m_strConfigDir.Format("%s\\Doc%d", m_pTrack->m_strConfigDir, m_nDocIdx + 1);
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;

	_mkdir(m_strConfigDir);
	int nFOVIndexBk = m_pTrack->m_nFOVIndex;

	int nLoadedDataPercentageForDocView = m_pApp->m_pProgressDlg->m_nCurrentPos;
	int nLoadedDataPercentageForEachDoc = ((100 - nLoadedDataPercentageForDocView) / theApp.m_nTotalTracks) / m_pApp->m_pTracks[0].m_nTotalDocs;

	int nLoadedDataPercentage = nLoadedDataPercentageForDocView + m_pTrack->m_nTrack * (100 - nLoadedDataPercentageForDocView) / theApp.m_nTotalTracks
		+ nLoadedDataPercentageForEachDoc * m_nDocIdx;
	double dLoadingRate = nLoadedDataPercentageForEachDoc / m_nNoOfFov * 1.0;


	for (int nFOV = 0; nFOV < m_nNoOfFov; nFOV++) {
		m_pTrack->m_nFOVIndex = nFOV;

		m_InspectionHandler[nFOV].GetPVIDefectParameters(m_strConfigDir, m_pTrack->m_nTrack, m_nDocIdx, nFOV);
		((CMainFrame*)m_pApp->m_pMainWnd)->InitializeProperties(this, nFOV);
		((CMainFrame*)m_pApp->m_pMainWnd)->InitializeTeachProperties(this, nFOV);

		if (m_pApp->m_pProgressDlg != NULL) {
			m_pApp->m_pProgressDlg->SetStatus("Loading Configuration....", nLoadedDataPercentage + (nFOV + 1) * dLoadingRate);
		}
	}
	m_pTrack->m_nFOVIndex = nFOVIndexBk;

	m_lDebugFlag = m_lDebugFlag | DEBUG_DRAW | DEBUG_PRINT | DEBUG_TIME;

	DrawView(FALSE);
	DrawView();

	return 0;

}

int CAppDoc::LoadConfiguration()
{
	m_strConfigDir.Format("%s\\Doc%d", m_pTrack->m_strConfigDir, m_nDocIdx+1);
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;

	//m_strConfigDir = m_pTrack->m_strConfigDir;

	//if(m_pTrack->m_nTrack==0 && m_nDocIdx==0) {
		_mkdir(m_strConfigDir);
		//OutputDebugLogTo(8, TRUE, "Doc Config Folder: %s", m_strConfigDir);
		CCalibrationData* pCalibData;

		int nFOVIndexBk = m_pTrack->m_nFOVIndex;
		/*for (int nFOV = 0; nFOV < m_nNoOfFov; nFOV++) {
			m_InspectionHandler[nFOV].CleanUp();
			m_InspectionHandler[nFOV].CleanUpTeach();
		}*/

		int nLoadedDataPercentageForDocView = m_pApp->m_pProgressDlg->m_nCurrentPos;
		int nLoadedDataPercentageForEachDoc = ((100 - nLoadedDataPercentageForDocView) / theApp.m_nTotalTracks) / m_pApp->m_pTracks[0].m_nTotalDocs;

		int nLoadedDataPercentage = nLoadedDataPercentageForDocView + m_pTrack->m_nTrack * (100 - nLoadedDataPercentageForDocView)/ theApp.m_nTotalTracks
			+ nLoadedDataPercentageForEachDoc * m_nDocIdx;
		double dLoadingRate = nLoadedDataPercentageForEachDoc/ m_nNoOfFov*1.0;

		m_pApp->m_LightHandler.LoadLightSettings(m_nDocIdx, m_pTrack->m_nTrack);
		
		for(int nFOV=0; nFOV< m_nNoOfFov; nFOV++) {
			m_pTrack->m_nFOVIndex = nFOV;


			if (!m_nDocIdx) {
				m_InspectionHandler[nFOV].LoadCalibrationParameters(m_pApp->m_strCalibDir, m_pApp->m_strConfigFile, m_pTrack->m_nTrack, nFOV);
				m_InspectionHandler[nFOV].LoadDotCalibrationParameters(m_pApp->m_strCalibDir, m_pApp->m_strConfigFile, m_pTrack->m_nTrack, nFOV);
			}

			pCalibData = &m_pApp->m_pTracks[m_pTrack->m_nTrack].m_pDoc[0]->m_InspectionHandler[0].m_Inspection.m_CalibrationData;

			m_InspectionHandler[nFOV].CreateParameterList(m_pTrack->m_nTrack, m_nDocIdx, nFOV);
			m_InspectionHandler[nFOV].CreateTeachParmList(/*m_pTrack->m_nTrack, m_nDocIdx, nFOV*/);
			//// clear teach param variables before reading values from recipe file ////
			m_InspectionHandler[nFOV].ClearTeachParam(NULL);
			m_InspectionHandler[nFOV].LoadRecipe(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
										&m_Buffer,
										m_strConfigDir,
										m_pApp->m_strConfigFile,
										m_pTrack->m_nTrack,
										m_pTrack->m_nTotalDocs,
										nFOV,m_nDocIdx);

			CAppDoc* pAppDoc = (CAppDoc*)this;
			m_pApp->m_pTracks[pAppDoc->m_pTrack->m_nTrack].m_pActiveDocTeach = pAppDoc;

			((CMainFrame*) m_pApp->m_pMainWnd)->InitializeProperties(this, nFOV);
			((CMainFrame*) m_pApp->m_pMainWnd)->InitializeTeachProperties(this, nFOV);
			
			int nError, nTeachStep = 2;//// 1-Device Loc & Dattam; 2-Region & Mask;
			CString strImgFullPath;
			strImgFullPath.Format("%s\\TeachTemplate_Pos%d.bmp", m_strConfigDir, nFOV+1);
			nError = LoadGrayScaleBMPFile(strImgFullPath, m_TeachBuffer[nFOV]);
			if(!nError) {
				for(int i=0; i<nTeachStep; i++) {
					m_InspectionHandler[nFOV].AutoTeach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
											&m_TeachBuffer[nFOV], 
											m_strConfigDir, m_pApp->m_strConfigFile,
											!i ? FALSE : TRUE,
											m_pTrack->m_nTrack, 
											m_nDocIdx, 
											nFOV+1, 
											m_nNoOfFov,
											pCalibData);
				}
			}
			else {
				OutputDebugLogTo(1,TRUE,"AutoTeach Load Image Error [Teach Image Not Avaiable]");
				OutputDebugLogTo(9,TRUE,"CAppDoc::LoadConfiguration AutoTeach Image Error [Track%d FOV%d Doc%d]", 
                    m_pTrack->m_nTrack+1, nFOV+1, m_nDocIdx+1); 
                CString str;
                str.Format("Teach Image Not Avaiable for Track%d Doc%d Position %d", m_pTrack->m_nTrack + 1, m_nDocIdx + 1, nFOV + 1);
                m_pApp->AddLogEvent(str);
			}

			if (m_pApp->m_pProgressDlg != NULL) {
				m_pApp->m_pProgressDlg->SetStatus("Loading Configuration....", nLoadedDataPercentage + (nFOV + 1) * dLoadingRate);
			}
		}
		m_pTrack->m_nFOVIndex = nFOVIndexBk;
	//}

	m_lDebugFlag  = m_lDebugFlag|DEBUG_DRAW|DEBUG_PRINT|DEBUG_TIME;
	
	DrawView(FALSE);
	DrawView();

	return 0;
	
}

int CAppDoc::LoadConfiguration(int nFov)
{
	m_strConfigDir.Format("%s\\Doc%d", m_pTrack->m_strConfigDir, m_nDocIdx + 1);
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;

	//m_strConfigDir = m_pTrack->m_strConfigDir;

	//if(m_pTrack->m_nTrack==0 && m_nDocIdx==0) {
	_mkdir(m_strConfigDir);
	//OutputDebugLogTo(8, TRUE, "Doc Config Folder: %s", m_strConfigDir);
	CCalibrationData* pCalibData;

	int nFOVIndexBk = m_pTrack->m_nFOVIndex;
	/*m_InspectionHandler[nFov].CleanUp();
	m_InspectionHandler[nFov].CleanUpTeach();*/

	m_pApp->m_LightHandler.LoadLightSettings(m_nDocIdx, m_pTrack->m_nTrack);

	m_pTrack->m_nFOVIndex = nFov;


	if (!m_nDocIdx) {
		m_InspectionHandler[nFov].LoadCalibrationParameters(m_pApp->m_strCalibDir, m_pApp->m_strConfigFile, m_pTrack->m_nTrack, nFov);
		m_InspectionHandler[nFov].LoadDotCalibrationParameters(m_pApp->m_strCalibDir, m_pApp->m_strConfigFile, m_pTrack->m_nTrack, nFov);
	}

	pCalibData = &m_pApp->m_pTracks[m_pTrack->m_nTrack].m_pDoc[0]->m_InspectionHandler[0].m_Inspection.m_CalibrationData;

	m_InspectionHandler[nFov].CreateParameterList(m_pTrack->m_nTrack, m_nDocIdx, nFov);
	m_InspectionHandler[nFov].CreateTeachParmList(/*m_pTrack->m_nTrack, m_nDocIdx, nFOV*/);
	//// clear teach param variables before reading values from recipe file ////
	m_InspectionHandler[nFov].ClearTeachParam(NULL);
	m_InspectionHandler[nFov].LoadRecipe(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
		&m_Buffer,
		m_strConfigDir,
		m_pApp->m_strConfigFile,
		m_pTrack->m_nTrack,
		m_pTrack->m_nTotalDocs,
		nFov, m_nDocIdx);

	CAppDoc* pAppDoc = (CAppDoc*)this;
	m_pApp->m_pTracks[pAppDoc->m_pTrack->m_nTrack].m_pActiveDocTeach = pAppDoc;

	((CMainFrame*)m_pApp->m_pMainWnd)->InitializeProperties(this, nFov);
	((CMainFrame*)m_pApp->m_pMainWnd)->InitializeTeachProperties(this, nFov);

	int nError, nTeachStep = 2;//// 1-Device Loc & Dattam; 2-Region & Mask;
	CString strImgFullPath;
	strImgFullPath.Format("%s\\TeachTemplate_Pos%d.bmp", m_strConfigDir, nFov + 1);
	nError = LoadGrayScaleBMPFile(strImgFullPath, m_TeachBuffer[nFov]);
	if (!nError) {
		for (int i = 0; i < nTeachStep; i++) {
			m_InspectionHandler[nFov].AutoTeach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
				&m_TeachBuffer[nFov],
				m_strConfigDir, m_pApp->m_strConfigFile,
				!i ? FALSE : TRUE,
				m_pTrack->m_nTrack,
				m_nDocIdx,
				nFov + 1,
				m_nNoOfFov,
				pCalibData);
		}
	}
	else {
		OutputDebugLogTo(1, TRUE, "AutoTeach Load Image Error [Teach Image Not Avaiable]");
		OutputDebugLogTo(9, TRUE, "CAppDoc::LoadConfiguration AutoTeach Image Error [Track%d FOV%d Doc%d]",
			m_pTrack->m_nTrack + 1, nFov + 1, m_nDocIdx + 1);
		CString str;
		str.Format("Teach Image Not Avaiable for Track%d Doc%d Position %d", m_pTrack->m_nTrack + 1, m_nDocIdx + 1, nFov + 1);
		m_pApp->AddLogEvent(str);
	}

	m_pTrack->m_nFOVIndex = nFOVIndexBk;
	//}

	m_lDebugFlag = m_lDebugFlag | DEBUG_DRAW | DEBUG_PRINT | DEBUG_TIME;

	DrawView(FALSE);
	DrawView();

	return 0;

}
//int CAppDoc::Teach()
//{
//	CTrainingData* pData;
//	CTrainingData* pDataFirst;
//	CInspectionHandler* pInspHandler;
//	CPVIInspectionSetup* pDataRegion;
//	
//	int nError, nTrack, nDocCount, nDocFrame, nTeachStep;
//	CString strLoadImgMsg, strTeachImgName, strStatusBarMsg, strInfoMsg;
//	BOOL bTeachOnline;
//	LOGPEN TextColorRed, TextColorGreen, TextColor, TextColorYellow;
//
//	TextColorRed.lopnColor = RGB(230,50,50);
//	TextColorRed.lopnStyle = PS_SOLID;
//	TextColorRed.lopnWidth.x = 1;
//
//	TextColorYellow.lopnColor = RGB(230,230,0);
//	TextColorYellow.lopnStyle = PS_SOLID;
//	TextColorYellow.lopnWidth.x = 1;
//
//	TextColorGreen.lopnColor = RGB(50,230,50);
//	TextColorGreen.lopnStyle = PS_SOLID;
//	TextColorGreen.lopnWidth.x = 1;
//
//	nTeachStep = 2;		//// Device Loc; Dattam/Pattern Search; Region Search;
//	nError = 0;
//	nTrack = m_pTrack->m_nTrack;
//	nDocCount = m_pApp->m_nTotalViewDocs[nTrack];
//	bTeachOnline = (m_pApp->m_pTracks[nTrack].m_bCamEnable && m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_bCameraActive) ? TRUE : FALSE;
//
//	m_bShowOverlay = TRUE;
//	m_pTrack->m_bOpticsRoi = FALSE;
//	m_pApp->AddLogEvent("Teach Process Started");
//	UpdateStatusBar(&CString(""));
//
//	if(m_bBinaryMode)
//		SwitchImg(&m_ImageViewInfo, FALSE);
//
//	m_pApp->m_pWndPVIDefectParameters->m_nTrackId = nTrack;
//	int nFOVIdxBk = m_pTrack->m_nFOVIndex;
//	//m_pApp->m_pWndPVIDefectParameters->ShowWindow(TRUE);
//	for(int nFOV=0; nFOV<m_pApp->m_nNoOfFov; nFOV++) {
//		m_pTrack->m_nFOVIndex = nFOV;
//		strLoadImgMsg.Format("Do You Like To Load Position%d Teach Image?", nFOV+1);
//		if(!bTeachOnline) {
//			if (AfxMessageBox(strLoadImgMsg, MB_YESNO) == IDYES) {
//				//m_pApp->m_pTracks[nTrack].m_pDoc[0]->OnToolsStaticimageLoadTeachImage();
//				CRect rectTeach = CRect(CPoint(0, 0), m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_Buffer.GetImgSize());
//				CString strImgFullPath;
//				for (int nTeachDoc = 0; nTeachDoc < nDocCount; nTeachDoc++) {
//					strImgFullPath.Format("%s\\TeachTemplate_Pos%d.bmp", m_pApp->m_pTracks[nTrack].m_pDoc[nTeachDoc]->m_strConfigDir, nFOV + 1);
//					nError = LoadGrayScaleBMPFile(strImgFullPath, m_pApp->m_pTracks[nTrack].m_pDoc[nTeachDoc]->m_Buffer);
//					if (nError) {
//						if (AfxMessageBox("Teach Image Not Available!!!\nDo You Like To Load Static Image?", MB_YESNO) == IDYES)
//							m_pApp->m_pTracks[nTrack].m_pDoc[0]->OnToolsStaticimageLoadFromFile();
//						break;
//					}
//				}
//			}
//			else {
//				//m_pApp->AddLogEvent("Teach Process Aborted");
//				//UpdateStatusBar(&CString("Teach Process Aborted"));
//				//return -1;
//				continue;
//			}
//		}
//		else {
//			//// not required ////
//		}
//
//		DrawView(FALSE);
//		nDocFrame = 0;
//		CRect rectTemp;
//		int nRectCountTemp;
//		int nRectTempIndexLoc[5] = {0};
//
//		pDataFirst = &m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[nFOV].m_Inspection.m_TrainingData;
//		m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[nFOV].ClearTeachParam(pDataFirst);
//		for(int nDoc=0; nDoc<nDocCount; nDoc++) {
//			strInfoMsg.Format("%s - Position%d [%s]", !nTrack ? "Top Camera" : (nTrack == 1 ? "Bottom Camera" : "Side Camera"),
//							nFOV+1,
//							!nTrack ? m_pApp->m_strTopCameraName.GetAt(nDoc) : (nTrack == 1 ? m_pApp->m_strBottomCameraName.GetAt(nDoc) : m_pApp->m_strSideCameraName.GetAt(nDoc)));
//			if (m_pApp->m_pInfoMsgDlg) {
//				if (!m_pApp->m_pInfoMsgDlg->IsWindowVisible())
//					m_pApp->m_pInfoMsgDlg->ShowWindow(TRUE);
//			}
//
//			CString* strTemp = new CString;
//			strTemp->Format("%d%d", nDoc, nFOV);
//			m_pApp->m_pMainWnd->SendMessageA(WM_SET_CUR_POS_PARAM_DISP, (WPARAM)nTrack, (LPARAM)strTemp);
//			delete strTemp;
//
//			pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFOV];
//			pData = &pInspHandler->m_Inspection.m_TrainingData;
//			//pDataRegion = pInspHandler->m_Inspection.m_pPviInspectionSetup;
//			nDocFrame = !nTrack ? nDoc : (nTrack==1 ? m_pApp->m_nTop+nDoc : m_pApp->m_nTop+m_pApp->m_nBottom+nDoc);			
//
//			BOOL bTempMsgLocYes = FALSE;
//			BOOL bEnableLoc[2] = {FALSE};
//			strLoadImgMsg.Format("Do You Like To Teach Location For Intensity%d Image", nDoc+1);
//			//if(AfxMessageBox(strLoadImgMsg, MB_YESNO) == IDYES) {
//			if(pData->bDeviceLocationEnable || pData->bDattamSearchEnable) {
//				ZoomSelectedDocView(nDocFrame, TRUE);
//				bTempMsgLocYes = TRUE;
//				for(int nStep=0; nStep<nTeachStep; nStep++) {
//					if(!nStep) nRectCountTemp = pData->bDeviceLocationEnable ? 1 : 0;
//					else if(nStep==1) nRectCountTemp = pData->bDattamSearchEnable ? pData->nDattamSearchNoOfDattams : 0;
//
//					for(int nRect=0; nRect<nRectCountTemp; nRect++) {
//						bEnableLoc[nStep] = TRUE;
//						rectTemp = CRect(100,100,400,400);
//						if(!nStep && pData->m_rectDeviceLoc.Width() != 0) rectTemp = pData->m_rectDeviceLoc;
//						else if(nStep==1 && pData->m_rectDattamSearch[nRect].Width() != 0) rectTemp = pData->m_rectDattamSearch[nRect];
//						
//						DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTemp);
//						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
//						
//						if(!nStep) strStatusBarMsg.Format("Draw ROI To Find The Location Of Device");
//						else if(nStep==1) strStatusBarMsg.Format("Draw ROI To Locate Datum/Pattern Search Area");						
//						//UpdateStatusBar(&strStatusBarMsg);
//						/*if(m_pApp->m_pInfoMsgDlg) {
//							if (!m_pApp->m_pInfoMsgDlg->IsWindowVisible())
//								m_pApp->m_pInfoMsgDlg->ShowWindow(TRUE);
//						}*/
//						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
//
//						if(!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
//							rectTemp = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexLoc[nDoc]++);
//						else {
//							m_pApp->AddLogEvent("Teach Process Aborted");
//							UpdateStatusBar(&CString("Teach Process Aborted"));
//							if (m_pApp->m_pInfoMsgDlg) {
//								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
//									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
//							}
//							return -1;
//						}
//						
//						if(!nStep) pData->m_rectDeviceLoc = rectTemp;
//						else if(nStep==1) pData->m_rectDattamSearch[nRect] = rectTemp;
//					}
//				}
//			}
//			else {
//				bTempMsgLocYes = FALSE;
//				bEnableLoc[0] = (pData->m_rectDeviceLoc.Width() != 0) ? TRUE : FALSE;
//				for(int nTemp=0; nTemp<pData->nDattamSearchNoOfDattams; nTemp++)
//					bEnableLoc[1] |= (pData->m_rectDattamSearch[nTemp].Width() != 0) ? TRUE : FALSE;
//			}
//
//			nError = pInspHandler->Teach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
//									&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
//									m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir,
//									m_pApp->m_strConfigFile,
//									0,
//									bEnableLoc,
//									FALSE,
//									nTrack,
//									nDoc,
//									nFOV+1,
//									m_pApp->m_nNoOfFov,
//									pDataFirst);
//
//			if(!nError /*&& (bEnableLoc[0] || bEnableLoc[1])*/) {
//				//if (bTempMsgLocYes && (bEnableLoc[0] || bEnableLoc[1])) {
//					//AfxMessageBox("Confirm to save teach data?", MB_YESNO | MB_ICONQUESTION);
//						strTeachImgName.Format("%s\\TeachTemplate_Pos%d.bmp", m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, nFOV + 1);
//						SaveGrayScaleBMPFile(strTeachImgName, *m_pTrack->m_SharedData.pProcBuffers[nDoc]);
//				//}
//				pInspHandler->SaveTeachParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, m_pApp->m_strConfigFile, nFOV);	
//			}
//			else if(nError) {
//				m_pApp->AddLogEvent("Teach Process Aborted");
//				UpdateStatusBar(&CString("Teach Process Aborted"));
//				ZoomSelectedDocView(nDocFrame);
//				DrawView();
//				if(m_pApp->m_pInfoMsgDlg) {
//					if(m_pApp->m_pInfoMsgDlg->IsWindowVisible())
//						m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
//				}
//				return -1;
//			}
//
//			if ((bEnableLoc[0] || bEnableLoc[1]) && bTempMsgLocYes) {
//				strStatusBarMsg.Format("Regions found");
//				m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
//				if (WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
//					m_pApp->AddLogEvent("Teach Process Aborted");
//					UpdateStatusBar(&CString("Teach Process Aborted"));
//					if (m_pApp->m_pInfoMsgDlg) {
//						if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
//							m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
//					}
//					return -1;
//				}
//			}
//		}
//		//ZoomSelectedDocView(nDocFrame);
//		DrawView();
//			
//		//// ---- Region Teach ---- ////
//		DrawView(FALSE);
//		int nRectTempIndexRgn[5] = {0};
//		for(int nDoc=0; nDoc<nDocCount; nDoc++) {
//			strInfoMsg.Format("%s - Position%d [%s]", !nTrack ? "Top Camera" : (nTrack == 1 ? "Bottom Camera" : "Side Camera"),
//				nFOV + 1,
//				!nTrack ? m_pApp->m_strTopCameraName.GetAt(nDoc) : (nTrack == 1 ? m_pApp->m_strBottomCameraName.GetAt(nDoc) : m_pApp->m_strSideCameraName.GetAt(nDoc)));
//			if (m_pApp->m_pInfoMsgDlg) {
//				if (!m_pApp->m_pInfoMsgDlg->IsWindowVisible())
//					m_pApp->m_pInfoMsgDlg->ShowWindow(TRUE);
//			}
//
//			pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFOV];
//			pData = &pInspHandler->m_Inspection.m_TrainingData;
//			pDataRegion = pInspHandler->m_Inspection.m_pPviInspectionSetup;
//			nDocFrame = !nTrack ? nDoc : (nTrack==1 ? m_pApp->m_nTop+nDoc : m_pApp->m_nTop+m_pApp->m_nBottom+nDoc);			
//			pInspHandler->ClearTeachParam(NULL);
//
//			BOOL bTempMsgRgnYes[2] = { FALSE };
//			BOOL bEnableRgn[3] = {FALSE};
//			strLoadImgMsg.Format("Do You Like To Teach Pvi Region For Intensity%d Image", nDoc+1);
//			//if(AfxMessageBox(strLoadImgMsg, MB_YESNO) == IDYES) {
//			if(pDataRegion->bEnable) {
//				ZoomSelectedDocView(nDocFrame, TRUE);
//				bTempMsgRgnYes[0] = TRUE;
//				nRectCountTemp = pDataRegion->bEnable ? pData->nNoOfPviArea: 0;
//				for(int nRect=0; nRect<nRectCountTemp; nRect++) {
//					bEnableRgn[0] = TRUE;
//					rectTemp = CRect(100,100,400,400);
//					if(pDataRegion[nRect].m_rectPviArea.Width() != 0)
//						rectTemp = pDataRegion[nRect].m_rectPviArea;
//
//					DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTemp);
//					DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
//					strStatusBarMsg.Format("Draw ROI To Locate Pvi Area %d",nRect+1);
//					//UpdateStatusBar(&strStatusBarMsg);
//					/*if (m_pApp->m_pInfoMsgDlg) {
//						if (!m_pApp->m_pInfoMsgDlg->IsWindowVisible())
//							m_pApp->m_pInfoMsgDlg->ShowWindow(TRUE);
//					}*/
//					m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
//
//					if(!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
//						rectTemp = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexRgn[nDoc]++);
//					else {
//						m_pApp->AddLogEvent("Teach Process Aborted");
//						UpdateStatusBar(&CString("Teach Process Aborted"));
//						if (m_pApp->m_pInfoMsgDlg) {
//							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
//								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
//						}
//						return -1;
//					}
//					//if(pInspHandler->m_Inspection.m_pPviInspectionSetup[nRect].nDeviceMaskNumber>0){
//						for(int nMaskRegions=0;nMaskRegions<pInspHandler->m_Inspection.m_pPviInspectionSetup[nRect].nDeviceMaskNumber;nMaskRegions++) {
//							bEnableRgn[1] = TRUE;	
//							CRect rectTempMask = CRect(100,100,400,400);
//							if(pDataRegion[nRect].m_rectPviMaskRegions[nMaskRegions].Width() != 0)
//								rectTempMask = pDataRegion[nRect].m_rectPviMaskRegions[nMaskRegions];
//							
//							DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempMask);
//							DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
//
//							strStatusBarMsg.Format("Draw ROI To Locate Pvi Mask Region %d For Pvi Area %d",nMaskRegions+1,nRect+1);
//							//UpdateStatusBar(&strStatusBarMsg);
//							m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
//
//							if(!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
//								rectTempMask = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexRgn[nDoc]++);
//							else {
//								m_pApp->AddLogEvent("Teach Process Aborted");
//								UpdateStatusBar(&CString("Teach Process Aborted"));
//								if (m_pApp->m_pInfoMsgDlg) {
//									if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
//										m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
//								}
//								return -1;
//							}
//							pDataRegion[nRect].m_rectPviMaskRegions[nMaskRegions] = rectTempMask;
//						}
//					//}
//					pDataRegion[nRect].m_rectPviArea = rectTemp;
//				}
//			}
//			else {
//				bTempMsgRgnYes[0] = FALSE;
//				for (int nTemp1 = 0; nTemp1 < pData->nNoOfPviArea; nTemp1++) {
//					bEnableRgn[0] |= (pDataRegion[nTemp1].m_rectPviArea.Width() != 0) ? TRUE : FALSE;
//					for (int nTemp2 = 0; nTemp2 < pInspHandler->m_Inspection.m_pPviInspectionSetup[nTemp1].nDeviceMaskNumber; nTemp2++)
//						bEnableRgn[1] |= (pDataRegion[nTemp1].m_rectPviMaskRegions[nTemp2].Width() != 0) ? TRUE : FALSE;
//				}
//			}
//			
//			nError = pInspHandler->Teach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
//				&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
//				m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir,
//				m_pApp->m_strConfigFile,
//				1,
//				bEnableRgn,
//				FALSE,
//				nTrack,
//				nDoc,
//				nFOV + 1,
//				m_pApp->m_nNoOfFov,
//				pDataFirst);
//
//			if (!nError /*&& (bEnableRgn[0] || bEnableRgn[1])*/) {
//				//if((bTempMsgRgnYes[0] || bTempMsgRgnYes[1]) && (bEnableRgn[0] || bEnableRgn[1]))
//				//AfxMessageBox("Confirm to save teach data?", MB_YESNO | MB_ICONQUESTION);
//				//if(AfxMessageBox("Confirm to save teach data?", MB_YESNO | MB_ICONQUESTION)==IDYES)
//				pInspHandler->SaveTeachParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, m_pApp->m_strConfigFile, nFOV);
//				pInspHandler->SetPVIDefectParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, nFOV);
//			}
//			else if (nError) {
//				m_pApp->AddLogEvent("Teach Process Aborted");
//				UpdateStatusBar(&CString("Teach Process Aborted"));
//				ZoomSelectedDocView(nDocFrame);
//				DrawView();
//				if (m_pApp->m_pInfoMsgDlg) {
//					if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
//						m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
//				}
//				return -1;
//			}
//
//			//// ---- encap teach ---- ////
//			if (pInspHandler->m_Inspection.m_TrainingData.bEnableEncap) {
//				int nRectTempIndexEncap = 0;
//				bTempMsgRgnYes[1] = TRUE;
//				bEnableRgn[2] = TRUE;
//				for (int nEncapRect = 0; nEncapRect < 2; nEncapRect++) {
//					CRect rectTempEncap = CRect(100, 100, 400, 400);
//					if (pInspHandler->m_Inspection.m_TrainingData.m_rectEncap[nEncapRect].Width() != 0)
//						rectTempEncap = pInspHandler->m_Inspection.m_TrainingData.m_rectEncap[nEncapRect];
//
//					DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempEncap);
//					DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
//
//					strStatusBarMsg.Format("Locate %s Encap Area", !nEncapRect ? "Outter" : "Inner");
//					//UpdateStatusBar(&strStatusBarMsg);
//					/*if (m_pApp->m_pInfoMsgDlg) {
//						if (!m_pApp->m_pInfoMsgDlg->IsWindowVisible())
//							m_pApp->m_pInfoMsgDlg->ShowWindow(TRUE);
//					}*/
//					m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
//
//					if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
//						rectTempEncap = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexEncap++);
//					else {
//						m_pApp->AddLogEvent("Teach Process Aborted");
//						UpdateStatusBar(&CString("Teach Process Aborted"));
//						if (m_pApp->m_pInfoMsgDlg) {
//							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
//								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
//						}
//						return -1;
//					}
//					pInspHandler->m_Inspection.m_TrainingData.m_rectEncap[nEncapRect] = rectTempEncap;
//				}
//			}
//			else {
//				bTempMsgRgnYes[1] = FALSE;
//				for (int nTemp3 = 0; nTemp3<2; nTemp3++)
//					bEnableRgn[2] |= (pInspHandler->m_Inspection.m_TrainingData.m_rectEncap[nTemp3].Width() != 0) ? TRUE : FALSE;
//			}
//
//			nError = pInspHandler->Teach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
//									&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
//									m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir,
//									m_pApp->m_strConfigFile,
//									2,
//									bEnableRgn,
//									FALSE,
//									nTrack,
//									nDoc,
//									nFOV+1,
//									m_pApp->m_nNoOfFov,
//									pDataFirst);
//
//			if(!nError /*&& (bEnableRgn[0] || bEnableRgn[1])*/) {
//				//if((bTempMsgRgnYes[0] || bTempMsgRgnYes[1]) && (bEnableRgn[0] || bEnableRgn[1]))
//					//AfxMessageBox("Confirm to save teach data?", MB_YESNO | MB_ICONQUESTION);
//					//if(AfxMessageBox("Confirm to save teach data?", MB_YESNO | MB_ICONQUESTION)==IDYES)
//				pInspHandler->SaveTeachParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, m_pApp->m_strConfigFile, nFOV);
//				pInspHandler->SetPVIDefectParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, nFOV);
//			}
//			else if(nError) {
//				m_pApp->AddLogEvent("Teach Process Aborted");
//				UpdateStatusBar(&CString("Teach Process Aborted"));
//				ZoomSelectedDocView(nDocFrame);
//				DrawView();
//				if (m_pApp->m_pInfoMsgDlg) {
//					if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
//						m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
//				}
//				return -1;
//			}
//
//			if((bEnableRgn[0] || bEnableRgn[1] || bEnableRgn[2]) && (bTempMsgRgnYes[0] || bTempMsgRgnYes[1])) {
//				strStatusBarMsg.Format("Regions found");
//				m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
//				if (WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
//					m_pApp->AddLogEvent("Teach Process Aborted");
//					UpdateStatusBar(&CString("Teach Process Aborted"));
//					if (m_pApp->m_pInfoMsgDlg) {
//						if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
//							m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
//					}
//					return -1;
//				}
//			}
//		}
//		ZoomSelectedDocView(nDocFrame);
//		DrawView();		
//		//// read all teach values w.r.to FOV(only one) & Doc(all) and I/P to Teach function	
//	}
//	m_pTrack->m_nFOVIndex = nFOVIdxBk;
//	m_pApp->AddLogEvent("Teach Process Completed");
//	UpdateStatusBar(&CString("Teach Process Completed"));
//	if (m_pApp->m_pInfoMsgDlg) {
//		if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
//			m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
//	}
//
//	return nError;
//}

int CAppDoc::Teach()
{
	CTrainingData* pData;
	CTrainingData* pDataFirst;
	CInspectionHandler* pInspHandler;
	CPVIInspectionSetup* pDataRegion;
	CCalibrationData* pCalibData;

	int nError, nTrack, nDocCount, nDocFrame, nTeachStep;
	CString strLoadImgMsg, strTeachImgName, strStatusBarMsg, strInfoMsg;

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	
	LOGPEN TextColorRed, TextColorGreen, TextColor, TextColorYellow;
	TextColorRed.lopnColor = RGB(230, 50, 50);
	TextColorRed.lopnStyle = PS_SOLID;
	TextColorRed.lopnWidth.x = 1;

	TextColorYellow.lopnColor = RGB(230, 230, 0);
	TextColorYellow.lopnStyle = PS_SOLID;
	TextColorYellow.lopnWidth.x = 1;

	TextColorGreen.lopnColor = RGB(50, 230, 50);
	TextColorGreen.lopnStyle = PS_SOLID;
	TextColorGreen.lopnWidth.x = 1;

	nTeachStep = 2;		//// Device Loc; Dattam/Pattern Search; Region Search;
	nError = 0;
	nTrack = m_pTrack->m_nTrack;
	nDocCount = m_pApp->m_nTotalViewDocs[nTrack];

	m_bShowOverlay = TRUE;
	m_pTrack->m_bOpticsRoi = FALSE;

	if (m_pApp->m_bPixelRuler)
	{
		m_pApp->m_pPixelRulerDlg->ShowWindow(SW_HIDE);
		m_pApp->m_bPixelRuler = FALSE;
	}

	m_pApp->AddLogEvent("Teach Process Started");
	UpdateStatusBar(&CString(""));

	if (m_bBinaryMode)
		SwitchImg(&m_ImageViewInfo, FALSE, NULL);

	m_pApp->m_pWndPVIDefectParameters->m_nTrackId = nTrack;
	int nFOVIdxBk = m_pTrack->m_nFOVIndex;
	//m_pApp->m_pWndPVIDefectParameters->ShowWindow(TRUE);

	BOOL bLoadTeachImage = FALSE;
	strLoadImgMsg.Format("Do You Like To Load Teach Images?");
	if (AfxMessageBox(strLoadImgMsg, MB_YESNO) == IDYES)
		bLoadTeachImage = TRUE;

	int nStartFOV = 0;
	int nCurrentFOV = m_nNoOfFov;
	int nCurrentDoc = 0;
	if (m_pApp->m_bTeachSingleImage) {
		nStartFOV = m_pTrack->m_nFOVIndex;
		nCurrentFOV = nStartFOV + 1;
		nCurrentDoc = m_pTrack->m_pActiveDocTeach->m_nDocIdx;
	}

	m_pApp->m_nFovCompletedTeach.clear();

	for (int nFOV = nStartFOV; nFOV< nCurrentFOV/*m_pApp->m_nNoOfFov*/; nFOV++) {
		m_pTrack->m_nFOVIndex = nFOV;

		if (!m_pApp->m_bTeachSingleImage) {
			strLoadImgMsg.Format("Do You Like To Teach For Position%d ?", nFOV + 1);
			if (AfxMessageBox(strLoadImgMsg, MB_YESNO) == IDNO)
				continue;
		}
		
		bool bContinueTeach = true;

		if (bLoadTeachImage) {
			for (int nDoc1 = 0; nDoc1 < nDocCount; nDoc1++) {
				if (!theApp.motionParameters.IsGrabEnabled(nTrack, nDoc1, m_pTrack->m_nFOVIndex))
					continue;

				CString strTemp;
				strTemp.Format("%s\\TeachTemplate_Pos%d.bmp", m_pApp->m_pTracks[nTrack].m_pDoc[nDoc1]->m_strConfigDir, nFOV + 1);
				nError = LoadGrayScaleBMPFile(strTemp, m_pApp->m_pTracks[nTrack].m_pDoc[nDoc1]->m_Buffer);
				if (nError && (!m_pApp->m_bTeachSingleImage || (m_pApp->m_bTeachSingleImage && nDoc1 == nCurrentDoc))) {
					CString strImagePath;
					if (AfxMessageBox("Teach Image Not Available At This Doc... Do you want to load another image to teach!", IDOK) == IDOK) {
						CSelectFolder dlg;
						dlg.SetPath(m_pApp->m_strLoadMultiImagesFolderPath);
						dlg.m_bFindImage = TRUE;
						if (dlg.DoModal() == IDOK)
						{
							CString strTemp = dlg.GetPath();
							int nLength = strTemp.GetLength();
							strImagePath = strTemp.Left(nLength - 1);
						}
						else
							strImagePath.Empty();
					}

					if (strImagePath != "") {
						LoadImagesToDoc(strImagePath);
						nDoc1 = nDocCount;
					}
					else {
						bContinueTeach = false;
						break;
					}
				}
				
			}
		}
		else {
			//m_pApp->m_pTracks[nTrack].m_pDoc[0]->OnToolsStaticimageLoadFromFile();
			CString strImagePath;// = LoadImage();
			CSelectFolder dlg;
			dlg.SetPath(m_pApp->m_strLoadMultiImagesFolderPath);
			dlg.m_bFindImage = TRUE;
			if (dlg.DoModal() == IDOK)
			{
				CString strTemp = dlg.GetPath();
				int nLength = strTemp.GetLength();
				strImagePath = strTemp.Left(nLength - 1);
			}
			else
				strImagePath.Empty();

			if (strImagePath != "")
				LoadImagesToDoc(strImagePath);
			else {
				m_pApp->AddLogEvent("Teach Process Aborted");
				UpdateStatusBar(&CString("Teach Process Aborted"));
				DrawView();
				if (m_pApp->m_pInfoMsgDlg) {
					if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
						m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
				}
				return -1;
			}
		}

		if (!bContinueTeach)
			continue;

		DrawView(FALSE);
		nDocFrame = 0;
		CRect rectTemp;
		int nRectCountTemp;
		int nRectTempIndexLoc[5] = { 0 };

		pDataFirst = &m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[nFOV].m_Inspection.m_TrainingData;
		pCalibData = &m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[0].m_Inspection.m_CalibrationData;

		m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[nFOV].ClearTeachParam(pDataFirst);
		for (int nDoc = 0; nDoc<nDocCount; nDoc++) {
			strInfoMsg.Format("%s - Position%d [%s]", !nTrack ? "Top Camera" : (nTrack == 1 ? "Bottom Camera" : "Side Camera"),
				nFOV + 1,
				!nTrack ? m_pApp->m_strTopCameraName.GetAt(nDoc) : (nTrack == 1 ? m_pApp->m_strBottomCameraName.GetAt(nDoc) : m_pApp->m_strSideCameraName.GetAt(nDoc)));
			if (m_pApp->m_pInfoMsgDlg) {
				if (!m_pApp->m_pInfoMsgDlg->IsWindowVisible())
					m_pApp->m_pInfoMsgDlg->ShowWindow(TRUE);
			}

			CString* strTemp = new CString;
			strTemp->Format("%d%d", nDoc, nFOV);
			m_pApp->m_pMainWnd->SendMessageA(WM_SET_CUR_POS_PARAM_DISP, (WPARAM)nTrack, (LPARAM)strTemp);
			delete strTemp;

			pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFOV];
			pData = &pInspHandler->m_Inspection.m_TrainingData;
			//pDataRegion = pInspHandler->m_Inspection.m_pPviInspectionSetup;
			nDocFrame = !nTrack ? nDoc : (nTrack == 1 ? m_pApp->m_nTop + nDoc : m_pApp->m_nTop + m_pApp->m_nBottom + nDoc);

			BOOL bTempMsgLocYes = FALSE;
			BOOL bEnableLoc[2] = { FALSE };

			//For Singler FOV Teaching
			if (m_pApp->m_bTeachSingleImage && nDoc != nCurrentDoc) {
				//To bypass previous teach
				/*TupleConcat(pDataFirst->hTupleDatumTeachTuple, 0, &pDataFirst->hTupleDatumTeachTuple);
				TupleConcat(pDataFirst->hTupleEdgeTeachTuple, 0, &pDataFirst->hTupleEdgeTeachTuple);*/

				HRegion hEmptyRegion;
				GenEmptyRegion(&hEmptyRegion);
				ConcatObj(pDataFirst->hObjectDeviceLocation, hEmptyRegion, &pDataFirst->hObjectDeviceLocation);
				TupleConcat(pDataFirst->hTupleProjectionParam, (HTuple(0).TupleConcat(0)).TupleConcat(0), &pDataFirst->hTupleProjectionParam);
				TupleConcat(pDataFirst->hTupleTeachDoc, 1, &pDataFirst->hTupleTeachDoc);
				TupleConcat(pDataFirst->hTupleDatumTeachTuple, pData->bDattamSearchEnable, &pDataFirst->hTupleDatumTeachTuple);
				if (pData->bDattamSearchEnable)
					TupleConcat(pDataFirst->hTupleEdgeTeachTuple, pData->bDattamSearchEnable, &pDataFirst->hTupleEdgeTeachTuple);
				else
					TupleConcat(pDataFirst->hTupleEdgeTeachTuple, pData->bDeviceLocationEnable, &pDataFirst->hTupleEdgeTeachTuple);


				//pInspHandler->AutoTeach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
				//	&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
				//	m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, m_pApp->m_strConfigFile,
				//	FALSE,
				//	m_pTrack->m_nTrack,
				//	nDoc,
				//	nFOV + 1,
				//	m_pApp->m_nNoOfFov,
				//	pCalibData);
				if (pData->bDattamSearchEnable)
					bEnableLoc[1] = TRUE;
				if (pData->bDeviceLocationEnable)
					bEnableLoc[0] = TRUE;


				nError = pInspHandler->Teach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
					&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
					m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir,
					m_pApp->m_strConfigFile,
					0,
					bEnableLoc,
					FALSE,
					nTrack,
					nDoc,
					nFOV + 1,
					m_nNoOfFov,
					pDataFirst,
					pCalibData,
					FALSE);

				if (nError) {
					strInfoMsg = strInfoMsg + " - Error: " + m_pApp->GetFailedStr(nError, false);
					
					AfxMessageBox("Teach Failed At " + strInfoMsg);
					m_pApp->AddLogEvent("Teach Process Aborted");
					UpdateStatusBar(&CString("Teach Process Aborted"));
					ZoomSelectedDocView(nDocFrame);
					DrawView();
					if (m_pApp->m_pInfoMsgDlg) {
						if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
							m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
					}
					return -1;
				}

				continue;
			}

			if (pData->bDattamSearchEnable)
			{
				ZoomSelectedDocView(nDocFrame, TRUE);

				TupleConcat(pDataFirst->hTupleDatumTeachTuple, 1, &pDataFirst->hTupleDatumTeachTuple);

				bEnableLoc[1] = TRUE;
				rectTemp = CRect(100, 100, 400, 400);
				nRectCountTemp = pData->nDattamSearchNoOfDattams;
				for (int nRect = 0; nRect < nRectCountTemp; nRect++)
				{
					if (pData->m_rectDattamSearch[nRect].Width() != 0)
						rectTemp = pData->m_rectDattamSearch[nRect];

					DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTemp);
					DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
					strStatusBarMsg.Format("Draw ROI To Locate Datum/Pattern %d Search Area", nRect+1);
					m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

					if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
						rectTemp = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexLoc[nDoc]++);
					else {
						m_pApp->AddLogEvent("Teach Process Aborted");
						UpdateStatusBar(&CString("Teach Process Aborted"));
						if (m_pApp->m_pInfoMsgDlg) {
							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
						}
						ZoomSelectedDocView(nDocFrame);
						return -1;
					}
					pData->m_rectDattamSearch[nRect] = rectTemp;
				}
			}
			else
				TupleConcat(pDataFirst->hTupleDatumTeachTuple, 0, &pDataFirst->hTupleDatumTeachTuple);

			if (pData->bDeviceLocationEnable) 
			{
				if(!pData->bDattamSearchEnable)
					ZoomSelectedDocView(nDocFrame, TRUE);

				TupleConcat(pDataFirst->hTupleEdgeTeachTuple, 1, &pDataFirst->hTupleEdgeTeachTuple);

				bTempMsgLocYes = TRUE;
				bEnableLoc[0] = TRUE;		
				rectTemp = CRect(100, 100, 400, 400);
				
				if (pData->m_rectDeviceLoc.Width() != 0)
					rectTemp = pData->m_rectDeviceLoc;
						
				DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTemp);		
				DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

				strStatusBarMsg.Format("Draw ROI To Find The Location Of Device");
				m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

				if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
					rectTemp = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexLoc[nDoc]++);
				else {
					m_pApp->AddLogEvent("Teach Process Aborted");
					UpdateStatusBar(&CString("Teach Process Aborted"));
					if (m_pApp->m_pInfoMsgDlg) {
						if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
							m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
					}
					ZoomSelectedDocView(nDocFrame);
					return -1;
				}
				pData->m_rectDeviceLoc = rectTemp;

				//Get the edge line region search
				CRect horizontalROIRect(rectTemp.left, rectTemp.top, rectTemp.right, rectTemp.bottom - (rectTemp.Height() / 2));
				CRect verticalROIRect(rectTemp.left, rectTemp.top, rectTemp.right - (rectTemp.Width() / 2), rectTemp.bottom);

				if (pData->bDeviceEdgeDetectionEnable) {
					DrawView(FALSE);
					nRectTempIndexLoc[nDoc] = 0;

					//1.	Horizontal Line
					if (pData->nDeviceHorizontalDirection != NONE) {
						if (pData->m_rectDeviceHorizontalLineLoc.Width() != 0)
							horizontalROIRect = pData->m_rectDeviceHorizontalLineLoc;

						DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &horizontalROIRect);
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

						strStatusBarMsg.Format("Draw ROI To Find The Horizontal Edge Line Of Device");
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
							horizontalROIRect = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexLoc[nDoc]++);
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
						pData->m_rectDeviceHorizontalLineLoc = horizontalROIRect;
					}
					

					//2.	Horizontal Line
					if (pData->nDeviceVerticalDirection != NONE) {
						if (pData->m_rectDeviceVerticalLineLoc.Width() != 0)
							verticalROIRect = pData->m_rectDeviceVerticalLineLoc;

						DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &verticalROIRect);
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

						strStatusBarMsg.Format("Draw ROI To Find The Vertical Edge Line Of Device");
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
							verticalROIRect = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexLoc[nDoc]++);
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
						pData->m_rectDeviceVerticalLineLoc = verticalROIRect;
					}
					
				}
			}
			else {
				TupleConcat(pDataFirst->hTupleEdgeTeachTuple, 0, &pDataFirst->hTupleEdgeTeachTuple);

				bTempMsgLocYes = FALSE;
				bEnableLoc[0] = FALSE;// (pData->m_rectDeviceLoc.Width() != 0) ? TRUE : FALSE;
				for (int nTemp = 0; nTemp < pData->nDattamSearchNoOfDattams; nTemp++)
					bEnableLoc[1] |= FALSE;// (pData->m_rectDattamSearch[nTemp].Width() != 0) ? TRUE : FALSE;
			}

			//Auto Focusing Sequence ROI
			if (nFOV == 0 && nDoc == 0 && m_pApp->m_bAutoFocusingSequence) {
				pData->bEnableAutoFocusSequence = TRUE;

				rectTemp = CRect(2048 / 2 - 250, 2048 / 2 - 250, 2048 / 2 + 250, 2048 / 2 + 250);


				if (pData->m_rectAutoFocusROI.Width() != 0)
					rectTemp = pData->m_rectAutoFocusROI;

				DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTemp);
				DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

				strStatusBarMsg.Format("Draw Auto Focusing Sequence ROI");
				m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

				if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
					rectTemp = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexLoc[nDoc]++);
				else {
					m_pApp->AddLogEvent("Teach Process Aborted");
					UpdateStatusBar(&CString("Teach Process Aborted"));
					if (m_pApp->m_pInfoMsgDlg) {
						if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
							m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
					}
					ZoomSelectedDocView(nDocFrame);
					return -1;
				}
				pData->m_rectAutoFocusROI = rectTemp;
			}

			nError = pInspHandler->Teach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
										&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
										m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir,
										m_pApp->m_strConfigFile,
										0,
										bEnableLoc,
										FALSE,
										nTrack,
										nDoc,
										nFOV + 1,
										m_nNoOfFov,
										pDataFirst,
										pCalibData,
										FALSE);

			pData->bEnableAutoFocusSequence = FALSE;

			if (!nError) {
				strTeachImgName.Format("%s\\TeachTemplate_Pos%d.bmp", m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, nFOV + 1);
				SaveGrayScaleBMPFile(strTeachImgName, *m_pTrack->m_SharedData.pProcBuffers[nDoc]);
				CRect rectTeach = CRect(CPoint(0, 0), m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_Buffer.GetImgSize());
				ImgCopy(m_pTrack->m_SharedData.pProcBuffers[nDoc], &rectTeach, &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_TeachBuffer[nFOV], &rectTeach);
				pInspHandler->SaveTeachParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, m_pApp->m_strConfigFile, nFOV);
			}
			else if (nError) {
				strInfoMsg = strInfoMsg + " - Error: " + m_pApp->GetFailedStr(nError, false);

				AfxMessageBox("Teach Failed At " + strInfoMsg);
				m_pApp->AddLogEvent("Teach Process Aborted");
				UpdateStatusBar(&CString("Teach Process Aborted"));
				ZoomSelectedDocView(nDocFrame);
				DrawView();
				if (m_pApp->m_pInfoMsgDlg) {
					if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
						m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
				}
				return -1;
			}

			if ((bEnableLoc[0] || bEnableLoc[1]) && bTempMsgLocYes) {
				strStatusBarMsg.Format("Regions found");
				m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
				if (WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
					m_pApp->AddLogEvent("Teach Process Aborted");
					UpdateStatusBar(&CString("Teach Process Aborted"));
					if (m_pApp->m_pInfoMsgDlg) {
						if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
							m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
					}
					ZoomSelectedDocView(nDocFrame);
					return -1;
				}
			}
		}
		DrawView();

		//// ---- Region Teach ---- ////
		DrawView(FALSE);
		m_pApp->m_nFovCompletedTeach.push_back(nFOV);
		int nRectTempIndexRgn[5] = { 0 };
		for (int nDoc = 0; nDoc<nDocCount; nDoc++) {
			strInfoMsg.Format("%s - Position%d [%s]", !nTrack ? "Top Camera" : (nTrack == 1 ? "Bottom Camera" : "Side Camera"),
				nFOV + 1,
				!nTrack ? m_pApp->m_strTopCameraName.GetAt(nDoc) : (nTrack == 1 ? m_pApp->m_strBottomCameraName.GetAt(nDoc) : m_pApp->m_strSideCameraName.GetAt(nDoc)));
			if (m_pApp->m_pInfoMsgDlg) {
				if (!m_pApp->m_pInfoMsgDlg->IsWindowVisible())
					m_pApp->m_pInfoMsgDlg->ShowWindow(TRUE);
			}

			pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFOV];
			pData = &pInspHandler->m_Inspection.m_TrainingData;
			pDataRegion = pInspHandler->m_Inspection.m_pPviInspectionSetup;
			nDocFrame = !nTrack ? nDoc : (nTrack == 1 ? m_pApp->m_nTop + nDoc : m_pApp->m_nTop + m_pApp->m_nBottom + nDoc);
			pInspHandler->ClearTeachParam(NULL);

			//For Singler FOV Teaching
			if (m_pApp->m_bTeachSingleImage && nDoc != nCurrentDoc) {
				//pInspHandler->AutoTeach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
				//	&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
				//	m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, m_pApp->m_strConfigFile,
				//	TRUE,
				//	m_pTrack->m_nTrack,
				//	nDoc,
				//	nFOV + 1,
				//	m_pApp->m_nNoOfFov,
				//	pCalibData);
				continue;
			}

			BOOL bTempMsgRgnYes[3] = { FALSE };
			BOOL bEnableRgn[4] = { FALSE };
			//if (pDataRegion->bEnable) {
			ZoomSelectedDocView(nDocFrame, TRUE);
			bTempMsgRgnYes[0] = TRUE;
			nRectCountTemp = pData->nNoOfPviArea;
			rectTemp = CRect(100, 100, 400, 400);
			for (int nRect = 0; nRect < nRectCountTemp; nRect++) {
				DrawView();
				//// ---- Area Teach ---- ////
				DrawView(FALSE);
				nRectTempIndexRgn[nDoc] = 0;
				pDataRegion = &pInspHandler->m_Inspection.m_pPviInspectionSetup[nRect];
				if(pDataRegion->bEnable || pDataRegion->bEdgeEnable){
					bEnableRgn[0] = TRUE;
					
					if (pDataRegion->nDeviceAreaType == RECTANGLE) {
						if (pDataRegion->m_rectPviArea.Width() != 0 && pDataRegion->m_rectPviArea.Height() != 0)
							rectTemp = pDataRegion->m_rectPviArea;

						DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTemp);
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
						strStatusBarMsg.Format("Draw ROI To Locate Pvi Area %d", nRect + 1);
						//UpdateStatusBar(&strStatusBarMsg);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
							rectTemp = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexRgn[nDoc]++);
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
					}
					else if (pDataRegion->nDeviceAreaType == ECLIPSE) {
						rectTemp = CRect(100, 100, 400, 400);
						if (pDataRegion->m_rectPviArea.Width() != 0 && pDataRegion->m_rectPviArea.Height() != 0)
							rectTemp = pDataRegion->m_rectPviArea;

						DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTemp, 1);
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
						strStatusBarMsg.Format("Draw ROI To Locate Pvi Area %d", nRect + 1);
						//UpdateStatusBar(&strStatusBarMsg);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
							rectTemp = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexRgn[nDoc]++);
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
					}
					else if (pDataRegion->nDeviceAreaType == POLYGON) {
						int nOffset = 10;
						for (int nPointId = 0; nPointId < pDataRegion->nPolygonPointNumber; nPointId++) {
							rectTemp = CRect(pDataRegion->m_polygonPviArea[nPointId].x - nOffset, pDataRegion->m_polygonPviArea[nPointId].y - nOffset,
								pDataRegion->m_polygonPviArea[nPointId].x + nOffset, pDataRegion->m_polygonPviArea[nPointId].y + nOffset);
							DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTemp, POLYGON);
						}
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
						strStatusBarMsg.Format("Draw Polygon ROI To Locate Pvi Area %d", nRect + 1);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
							for (int nPointId = 0; nPointId < pDataRegion->nPolygonPointNumber; nPointId++) {
								rectTemp = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexRgn[nDoc]++);
								pDataRegion->m_polygonPviArea[nPointId] = rectTemp.CenterPoint();
							}
						}
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
					}
					if (pDataRegion->nDeviceAreaType != POLYGON)
						pDataRegion->m_rectPviArea = rectTemp;

					pDataRegion->m_rectPviArea = rectTemp;

					//PVI Mask Teach
					CRect rectTempMask = CRect(100, 100, 400, 400);
					for (int nMaskRegions = 0; nMaskRegions < pInspHandler->m_Inspection.m_pPviInspectionSetup[nRect].nDeviceMaskNumber; nMaskRegions++) {
						bEnableRgn[1] = TRUE;
						if (pDataRegion->m_rectPviMaskRegions[nMaskRegions].Width() != 0)
							rectTempMask = pDataRegion->m_rectPviMaskRegions[nMaskRegions];

						DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempMask);
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

						strStatusBarMsg.Format("Draw ROI To Locate Pvi Mask Region %d For Pvi Area %d", nMaskRegions + 1, nRect + 1);
						//UpdateStatusBar(&strStatusBarMsg);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
							rectTempMask = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexRgn[nDoc]++);
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
						pDataRegion->m_rectPviMaskRegions[nMaskRegions] = rectTempMask;
					}
					
					//PVI Slot Checking TEACH
					if (pInspHandler->m_Inspection.m_pPviInspectionSetup[nRect].bPVISlotConnectionCheckEnable) {
						CRect rectTempSlot = CRect(100, 100, 400, 400);
						for (int nSlotRegions = 0; nSlotRegions < pInspHandler->m_Inspection.m_pPviInspectionSetup[nRect].nNumOfSlot; nSlotRegions++) {
							if (pDataRegion->m_rectPviSlot[nSlotRegions].Width() != 0 && pDataRegion->m_rectPviSlot[nSlotRegions].Height() != 0)
								rectTempSlot = pDataRegion->m_rectPviSlot[nSlotRegions];

							DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempSlot);
							DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

							strStatusBarMsg.Format("Draw ROI To Locate Pvi Slot Region %d For Pvi Area %d", nSlotRegions + 1, nRect + 1);
							//UpdateStatusBar(&strStatusBarMsg);
							m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

							if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
								rectTempSlot = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexRgn[nDoc]++);
							else {
								m_pApp->AddLogEvent("Teach Process Aborted");
								UpdateStatusBar(&CString("Teach Process Aborted"));
								if (m_pApp->m_pInfoMsgDlg) {
									if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
										m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
								}
								ZoomSelectedDocView(nDocFrame);
								return -1;
							}
							pDataRegion->m_rectPviSlot[nSlotRegions] = rectTempSlot;
						}
					}
					
				}
			}

			nError = pInspHandler->Teach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
				&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
				m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir,
				m_pApp->m_strConfigFile,
				1,
				bEnableRgn,
				FALSE,
				nTrack,
				nDoc,
				nFOV + 1,
				m_nNoOfFov,
				pDataFirst,
				pCalibData,
				FALSE);

			if (!nError) {
				pInspHandler->SetPVIDefectParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, nTrack, nDoc, nFOV);
				pInspHandler->SaveTeachParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, m_pApp->m_strConfigFile, nFOV);
			}
			else if (nError) {
				strInfoMsg = strInfoMsg + " - Error: " + m_pApp->GetFailedStr(nError, false);

				AfxMessageBox("Teach Failed At " + strInfoMsg);
				m_pApp->AddLogEvent("Teach Process Aborted");
				UpdateStatusBar(&CString("Teach Process Aborted"));
				ZoomSelectedDocView(nDocFrame);
				DrawView();
				if (m_pApp->m_pInfoMsgDlg) {
					if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
						m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
				}
				return -1;
			}
		//	}
			else {
				bTempMsgRgnYes[0] = FALSE;
				for (int nTemp1 = 0; nTemp1 < pData->nNoOfPviArea; nTemp1++) {
					bEnableRgn[0] |= FALSE;// (pDataRegion[nTemp1].m_rectPviArea.Width() != 0) ? TRUE : FALSE;
					for (int nTemp2 = 0; nTemp2 < pInspHandler->m_Inspection.m_pPviInspectionSetup[nTemp1].nDeviceMaskNumber; nTemp2++)
						bEnableRgn[1] |= FALSE;// (pDataRegion[nTemp1].m_rectPviMaskRegions[nTemp2].Width() != 0) ? TRUE : FALSE;
					for (int nTemp2 = 0; nTemp2 < pInspHandler->m_Inspection.m_pPviInspectionSetup[nTemp1].nNumOfSlot; nTemp2++)
						bEnableRgn[1] |= FALSE;
				}
			}



			/////////////
			/////////////Encap magnus begin
			/////////////////end encap magnus
			/////////////
			/////////////
			if (pInspHandler->m_Inspection.m_TrainingData.bEnable_EncapManus)
			{
				

				nError = pInspHandler->Teach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
					&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
					m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir,
					m_pApp->m_strConfigFile,
					14,
					bEnableRgn,
					FALSE,
					nTrack,
					nDoc,
					nFOV + 1,
					m_nNoOfFov,
					pDataFirst,
					pCalibData,
					FALSE);

				if (!nError) {
					//pInspHandler->SetPVIDefectParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, nFOV);
				//	pInspHandler->SaveTeachParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, m_pApp->m_strConfigFile, nFOV);
				}
				else if (nError) {
					strInfoMsg = strInfoMsg + " - Error: Encap Bottom Magnus device location";

					AfxMessageBox("Teach Failed At " + strInfoMsg);
					m_pApp->AddLogEvent("Teach Process Aborted");
					UpdateStatusBar(&CString("Teach Process Aborted"));
					ZoomSelectedDocView(nDocFrame);
					DrawView();
					if (m_pApp->m_pInfoMsgDlg) {
						if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
							m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
					}
					return -1;
				}

					ZoomSelectedDocView(nDocFrame);
					bTempMsgRgnYes[1] = TRUE;
					bEnableRgn[2] = TRUE;
					CRect rectTempEncap = CRect(100, 100, 400, 400);
					///////// Get crop encap magnus

					if (pInspHandler->m_Inspection.m_TrainingData.hRect_EncapLocation_magnus.Width() >0 && 
						pInspHandler->m_Inspection.m_TrainingData.hRect_EncapLocation_magnus.Height()>0)
						rectTempEncap = pInspHandler->m_Inspection.m_TrainingData.hRect_EncapLocation_magnus;

					ZoomSelectedDocView(nDocFrame, TRUE);
					DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempEncap);
					DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

					strStatusBarMsg.Format("Locate Encap Location Magnus ");
					//UpdateStatusBar(&strStatusBarMsg);
					m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

					if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
						rectTempEncap = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, 0);// nRectTempIndexRgn[nDoc]++);
					else {
						m_pApp->AddLogEvent("Teach Process Aborted");
						UpdateStatusBar(&CString("Teach Process Aborted"));
						if (m_pApp->m_pInfoMsgDlg) {
							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
						}
						ZoomSelectedDocView(nDocFrame);
						return -1;
					}


					///////////// ///////// Get crop smooth encap magnus
					//if (pInspHandler->m_Inspection.m_TrainingData.hRect_EncapLocation_magnus.Width() != 0)
					//	rectTempEncap = pInspHandler->m_Inspection.m_TrainingData.hRect_EncapLocation_magnus;
					CRect rectTempEncap2 = CRect(100, 100, 400, 400);
					if (pInspHandler->m_Inspection.m_TrainingData.hRect_CropSmoothEncap_magnus.Width() >0
						&& pInspHandler->m_Inspection.m_TrainingData.hRect_CropSmoothEncap_magnus.Height()>0)
						rectTempEncap2 = pInspHandler->m_Inspection.m_TrainingData.hRect_CropSmoothEncap_magnus;

					ZoomSelectedDocView(nDocFrame, TRUE);
					DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempEncap2);
					DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

					strStatusBarMsg.Format("Locate Encap Crop Smooth Location Magnus");
					//UpdateStatusBar(&strStatusBarMsg);
					m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

					if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
						rectTempEncap2 = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, 1);// nRectTempIndexRgn[nDoc]++);
					else {
						m_pApp->AddLogEvent("Teach Process Aborted");
						UpdateStatusBar(&CString("Teach Process Aborted"));
						if (m_pApp->m_pInfoMsgDlg) {
							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
						}
						ZoomSelectedDocView(nDocFrame);
						return -1;
					}

					///////////////
					///////////////
					CRect rectTempEncap3 = CRect(100, 100, 400, 400);
					if (pInspHandler->m_Inspection.m_TrainingData.hRect_CropSmoothEncap_magnus.Width() > 0
						&& pInspHandler->m_Inspection.m_TrainingData.hRect_CropRemoveBlackLine_magnus.Height() > 0)
						rectTempEncap3 = pInspHandler->m_Inspection.m_TrainingData.hRect_CropRemoveBlackLine_magnus;

					ZoomSelectedDocView(nDocFrame, TRUE);
					DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempEncap3);
					DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

					strStatusBarMsg.Format("Locate Remove Black Line Location Magnus");
					//UpdateStatusBar(&strStatusBarMsg);
					m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

					if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
						rectTempEncap3 = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, 2);// nRectTempIndexRgn[nDoc]++);
					else {
						m_pApp->AddLogEvent("Teach Process Aborted");
						UpdateStatusBar(&CString("Teach Process Aborted"));
						if (m_pApp->m_pInfoMsgDlg) {
							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
						}
						ZoomSelectedDocView(nDocFrame);
						return -1;
					}

					if (rectTempEncap.Height() > 0 && rectTempEncap2.Height() > 0 && rectTempEncap3.Height() > 0)
					{
						pInspHandler->m_Inspection.m_TrainingData.hRect_EncapLocation_magnus = rectTempEncap;
						pInspHandler->m_Inspection.m_TrainingData.hRect_CropSmoothEncap_magnus = rectTempEncap2;
						pInspHandler->m_Inspection.m_TrainingData.hRect_CropRemoveBlackLine_magnus = rectTempEncap3;
					}
					/// encap bottom tracing magnus
					nError = pInspHandler->Teach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
						&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
						m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir,
						m_pApp->m_strConfigFile,
						15,
						bEnableRgn,
						FALSE,
						nTrack,
						nDoc,
						nFOV + 1,
						m_nNoOfFov,
						pDataFirst,
						pCalibData,
						FALSE);
					if (!nError) {
						pInspHandler->SaveTeachParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, m_pApp->m_strConfigFile, nFOV);

						strStatusBarMsg.Format("Regions found");
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
						if (WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
					}
					else if (nError) {
						strInfoMsg = strInfoMsg + " - Error: Encap Location Teaching Magnus";

						AfxMessageBox("Teach Failed At " + strInfoMsg);
						m_pApp->AddLogEvent("Teach Process Aborted");
						UpdateStatusBar(&CString("Teach Process Aborted"));
						ZoomSelectedDocView(nDocFrame);
						DrawView();
						if (m_pApp->m_pInfoMsgDlg) {
							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
						}
						return -1;
					}


			}

			///////////////////////
			/////////////////////
			////////////////////////   Encap region

			if (pInspHandler->m_Inspection.m_TrainingData.bEnableEncap) 
			{
				ZoomSelectedDocView(nDocFrame);
				bTempMsgRgnYes[1] = TRUE;
				bEnableRgn[2] = TRUE;
				CRect rectTempEncap = CRect(100, 100, 400, 400);
				if (pInspHandler->m_Inspection.m_TrainingData.m_rectEncap[0].Width() != 0)
					rectTempEncap = pInspHandler->m_Inspection.m_TrainingData.m_rectEncap[0];

				ZoomSelectedDocView(nDocFrame, TRUE);
				DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempEncap);
				DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

				strStatusBarMsg.Format("Locate Encap Location");
				//UpdateStatusBar(&strStatusBarMsg);
				m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

				if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
					rectTempEncap = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, 0);// nRectTempIndexRgn[nDoc]++);
				else {
					m_pApp->AddLogEvent("Teach Process Aborted");
					UpdateStatusBar(&CString("Teach Process Aborted"));
					if (m_pApp->m_pInfoMsgDlg) {
						if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
							m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
					}
					ZoomSelectedDocView(nDocFrame);
					return -1;
				}
				int nRectTempIndexEncap = 0;
				//Encap Tracing Mask Teach

				CRect rectTempEncapMask = CRect(100, 100, 400, 400);
				for (int nMaskIdx = 0; nMaskIdx < pInspHandler->m_Inspection.m_TrainingData.nEncapTracingMaskNumber; nMaskIdx++) {
					nRectTempIndexEncap = 0;
					DrawView();
					DrawView(FALSE);
					if (pInspHandler->m_Inspection.m_TrainingData.nEncapTracingMaskType[nMaskIdx] == RECTANGLE) {
						if (pInspHandler->m_Inspection.m_TrainingData.m_rectMaskEncapTracing[nMaskIdx].Width() != 0 &&
							pInspHandler->m_Inspection.m_TrainingData.m_rectMaskEncapTracing[nMaskIdx].Height() != 0)
							rectTempEncapMask = pInspHandler->m_Inspection.m_TrainingData.m_rectMaskEncapTracing[nMaskIdx];

						DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempEncapMask);
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

						strStatusBarMsg.Format("Locate Rectangle Mask %d For Encap Tracing", nMaskIdx + 1);
						//UpdateStatusBar(&strStatusBarMsg);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
							rectTempEncapMask = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexEncap++);
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
					}
					else if (pInspHandler->m_Inspection.m_TrainingData.nEncapTracingMaskType[nMaskIdx] == ECLIPSE) {
						rectTempEncapMask = CRect(100, 100, 400, 400);
						if (pInspHandler->m_Inspection.m_TrainingData.m_rectMaskEncapTracing[nMaskIdx].Width() != 0 &&
							pInspHandler->m_Inspection.m_TrainingData.m_rectMaskEncapTracing[nMaskIdx].Height() != 0)
							rectTempEncapMask = pInspHandler->m_Inspection.m_TrainingData.m_rectMaskEncapTracing[nMaskIdx];
						DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempEncapMask, 1);
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
						strStatusBarMsg.Format("Locate Ellipse Mask %d For Encap Tracing", nMaskIdx + 1);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
							rectTempEncapMask = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexEncap++);
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
					}
					else if (pInspHandler->m_Inspection.m_TrainingData.nEncapTracingMaskType[nMaskIdx] == POLYGON) {
						int nOffset = 10;
						for (int nPointId = 0; nPointId < pInspHandler->m_Inspection.m_TrainingData.nEncapTracingPolygonPointNumber[nMaskIdx]; nPointId++) {
							CPoint PointTemp = pInspHandler->m_Inspection.m_TrainingData.m_EncapTracingPolygonPointArea[nMaskIdx][nPointId];
							rectTempEncapMask = CRect(PointTemp.x - nOffset, PointTemp.y - nOffset, PointTemp.x + nOffset, PointTemp.y + nOffset);
							DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempEncapMask, POLYGON);
						}
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
						strStatusBarMsg.Format("Locate Polygon Mask %d For Encap Tracing", nMaskIdx + 1);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
							for (int nPointId = 0; nPointId < pInspHandler->m_Inspection.m_TrainingData.nEncapTracingPolygonPointNumber[nMaskIdx]; nPointId++) {
								rectTempEncapMask = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexEncap++);
								pInspHandler->m_Inspection.m_TrainingData.m_EncapTracingPolygonPointArea[nMaskIdx][nPointId] = rectTempEncapMask.CenterPoint();
							}
						}
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
					}
					pInspHandler->m_Inspection.m_TrainingData.m_rectMaskEncapTracing[nMaskIdx] = rectTempEncapMask;
				}
				pInspHandler->m_Inspection.m_TrainingData.m_rectEncap[0] = rectTempEncap;

				nError = pInspHandler->Teach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
					&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
					m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir,
					m_pApp->m_strConfigFile,	
					2,
					bEnableRgn,
					FALSE,
					nTrack,
					nDoc,	
					nFOV + 1,
					m_nNoOfFov,
					pDataFirst,
					pCalibData,
					FALSE);

				if (!nError) {
					//pInspHandler->SetPVIDefectParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, nFOV);
					pInspHandler->SaveTeachParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, m_pApp->m_strConfigFile, nFOV);

					strStatusBarMsg.Format("Regions found");
					m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
					if (WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
						m_pApp->AddLogEvent("Teach Process Aborted");
						UpdateStatusBar(&CString("Teach Process Aborted"));
						if (m_pApp->m_pInfoMsgDlg) {
							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
						}
						ZoomSelectedDocView(nDocFrame);
						return -1;
					}		
				}
				else if (nError) {
					strInfoMsg = strInfoMsg + " - Error: Encap Location Teaching";

					AfxMessageBox("Teach Failed At " + strInfoMsg);
					m_pApp->AddLogEvent("Teach Process Aborted");
					UpdateStatusBar(&CString("Teach Process Aborted"));
					ZoomSelectedDocView(nDocFrame);
					DrawView();
					if (m_pApp->m_pInfoMsgDlg) {
						if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
							m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
					}
					return -1;
				}

				//// ---- encap teach ---- ////
				//DrawView(FALSE);

				BOOL bEnableEncapLoc = FALSE;
				if (pInspHandler->m_Inspection.m_TrainingData.bEnableEncap) {
					int nRectTempIndexEncap = 0;
					bTempMsgRgnYes[1] = TRUE;
					bEnableRgn[2] = TRUE;
					for (int nEncapRect = 1; nEncapRect < 3; nEncapRect++) {
						CRect rectTempEncap = CRect(100, 100, 400, 400);
						if (pInspHandler->m_Inspection.m_TrainingData.m_nRectWidth[nEncapRect] == 0 &&
							pInspHandler->m_Inspection.m_TrainingData.m_nRectHeight[nEncapRect] == 0)
						{
							if (pInspHandler->m_Inspection.m_TrainingData.m_rectEncap[nEncapRect].Width() != 0)
								rectTempEncap = pInspHandler->m_Inspection.m_TrainingData.m_rectEncap[nEncapRect];
						}
						else
						{
							CSize szTemp = CSize(pInspHandler->m_Inspection.m_TrainingData.m_nRectWidth[nEncapRect] / m_pTrack->m_dResolutionX,
								pInspHandler->m_Inspection.m_TrainingData.m_nRectHeight[nEncapRect] / m_pTrack->m_dResolutionY);
							CPoint ptCenter = CPoint(pInspHandler->m_Inspection.m_TrainingData.m_rectEncap[nEncapRect].CenterPoint().x,
								pInspHandler->m_Inspection.m_TrainingData.m_rectEncap[nEncapRect].CenterPoint().y);
							rectTempEncap = CRect(ptCenter.x - (szTemp.cx / 2), ptCenter.y - (szTemp.cy / 2),
								ptCenter.x + (szTemp.cx / 2), ptCenter.y + (szTemp.cy / 2));
						}

						DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempEncap);
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

						strStatusBarMsg.Format("Locate %s", !nEncapRect ? "Encap Location" : (nEncapRect == 1 ? "Outer Encap Area" : "Inner Encap Area"));
						//UpdateStatusBar(&strStatusBarMsg);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
							rectTempEncap = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexEncap++);
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
						pInspHandler->m_Inspection.m_TrainingData.m_rectEncap[nEncapRect] = rectTempEncap;
						pInspHandler->m_Inspection.m_TrainingData.m_nRectWidth[nEncapRect] = rectTempEncap.Width()*m_pTrack->m_dResolutionX;
						pInspHandler->m_Inspection.m_TrainingData.m_nRectHeight[nEncapRect] = rectTempEncap.Height()*m_pTrack->m_dResolutionY;
					}

					

					//Encap Surface Mask Teach
					rectTempEncapMask = CRect(100, 100, 400, 400);
					for (int nMaskIdx = 0; nMaskIdx < pInspHandler->m_Inspection.m_TrainingData.nEncapSurfaceMaskNumber; nMaskIdx++) {
						if (pInspHandler->m_Inspection.m_TrainingData.m_rectMaskEncapSurface[nMaskIdx].Width() != 0 &&
							pInspHandler->m_Inspection.m_TrainingData.m_rectMaskEncapSurface[nMaskIdx].Height() != 0)
							rectTempEncapMask = pInspHandler->m_Inspection.m_TrainingData.m_rectMaskEncapSurface[nMaskIdx];

						DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempEncapMask);
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

						strStatusBarMsg.Format("Locate Mask %d For Encap Surface Inspection", nMaskIdx + 1);
						//UpdateStatusBar(&strStatusBarMsg);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
							rectTempEncapMask = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexEncap++);
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
						pInspHandler->m_Inspection.m_TrainingData.m_rectMaskEncapSurface[nMaskIdx] = rectTempEncapMask;
					}


					nError = pInspHandler->Teach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
						&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
						m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir,
						m_pApp->m_strConfigFile,
						3,
						bEnableRgn,
						FALSE,
						nTrack,
						nDoc,
						nFOV + 1,
						m_nNoOfFov,
						pDataFirst,
						pCalibData,
						bEnableEncapLoc);

					if (!nError) {
						////pInspHandler->SetPVIDefectParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, nFOV);
						pInspHandler->SaveTeachParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, m_pApp->m_strConfigFile, nFOV);

						strStatusBarMsg.Format("Regions found");
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
						if (WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
					}
					else if (nError) {
						strInfoMsg = strInfoMsg + " - Error: Outer/Inner Encap Teaching";

						AfxMessageBox("Teach Failed At " + strInfoMsg);
						m_pApp->AddLogEvent("Teach Process Aborted");
						UpdateStatusBar(&CString("Teach Process Aborted"));
						ZoomSelectedDocView(nDocFrame);
						DrawView();
						if (m_pApp->m_pInfoMsgDlg) {
							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
						}
						ZoomSelectedDocView(nDocFrame);
						return -1;
					}
				}
				else {
					bTempMsgRgnYes[1] = FALSE;
					bEnableEncapLoc = FALSE;// (pInspHandler->m_Inspection.m_TrainingData.m_rectEncap[0].Width() != 0) ? TRUE : FALSE;
					for (int nTemp3 = 1; nTemp3 < 3; nTemp3++)
						bEnableRgn[2] |= FALSE;// (pInspHandler->m_Inspection.m_TrainingData.m_rectEncap[nTemp3].Width() != 0) ? TRUE : FALSE;
				}
			}

			//// cover layer ////
			//DrawView(FALSE);
			if (pInspHandler->m_Inspection.m_TrainingData.bEnableCoverLayer) {
				int nRectTempIndexCL = 0;
				bTempMsgRgnYes[2] = TRUE;
				bEnableRgn[3] = TRUE;
				ZoomSelectedDocView(nDocFrame, TRUE);

				//if(pInspHandler->m_Inspection.m_TrainingData.bEnableCLDevLoc || pInspHandler->m_Inspection.m_TrainingData.bEnableCLDatum)
				//	bEnableRgn[3] = TRUE;
				if (pInspHandler->m_Inspection.m_TrainingData.bEnableCLDatum) {
					CRect rectTempCL = CRect(100, 100, 400, 400);
					int nDatum = pInspHandler->m_Inspection.m_TrainingData.nCLDatum;
					for (int n = 0; n < nDatum; n++) {
						if (pInspHandler->m_Inspection.m_TrainingData.m_rectCLDatum[n].Width() != 0)
							rectTempCL = pInspHandler->m_Inspection.m_TrainingData.m_rectCLDatum[n];

						DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempCL);
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

						strStatusBarMsg.Format("Locate Flex Datum %d", n+1);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
							rectTempCL = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, n);
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
						pInspHandler->m_Inspection.m_TrainingData.m_rectCLDatum[n] = rectTempCL;
					}
				}

				int nCLRegion = pInspHandler->m_Inspection.m_TrainingData.nCLInspRegion;
				for (int n = 0; n < nCLRegion; n++) {
					CRect rectTempCL = CRect(100, 100, 400, 400);
					if (pInspHandler->m_Inspection.m_TrainingData.m_rectCoverLayer[n].Width() != 0)
						rectTempCL = pInspHandler->m_Inspection.m_TrainingData.m_rectCoverLayer[n];

					DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempCL);
					DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

					strStatusBarMsg.Format("Locate Cover Layer Region %d", n+1);
					//UpdateStatusBar(&strStatusBarMsg);
					m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

					if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
						rectTempCL = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, n);
					else {
						m_pApp->AddLogEvent("Teach Process Aborted");
						UpdateStatusBar(&CString("Teach Process Aborted"));
						if (m_pApp->m_pInfoMsgDlg) {
							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
						}
						ZoomSelectedDocView(nDocFrame);
						return -1;
					}
					pInspHandler->m_Inspection.m_TrainingData.m_rectCoverLayer[n] = rectTempCL;
				}
				nError = pInspHandler->Teach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
					&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
					m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir,
					m_pApp->m_strConfigFile,
					4,
					bEnableRgn,
					FALSE,
					nTrack,
					nDoc,
					nFOV + 1,
					m_nNoOfFov,
					pDataFirst,
					pCalibData,
					FALSE);

				if (!nError) {
					////pInspHandler->SetPVIDefectParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, nFOV);
					pInspHandler->SaveTeachParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, m_pApp->m_strConfigFile, nFOV);
					strStatusBarMsg.Format("Regions found");
					m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
					if (WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
						m_pApp->AddLogEvent("Teach Process Aborted");
						UpdateStatusBar(&CString("Teach Process Aborted"));
						if (m_pApp->m_pInfoMsgDlg) {
							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
						}
						ZoomSelectedDocView(nDocFrame);
						return -1;
					}
				}
				else if (nError) {
					strInfoMsg = strInfoMsg + " - Error: Cover Layer Teaching";

					AfxMessageBox("Teach Failed At " + strInfoMsg);
					m_pApp->AddLogEvent("Teach Process Aborted");
					UpdateStatusBar(&CString("Teach Process Aborted"));
					ZoomSelectedDocView(nDocFrame);
					DrawView();
					if (m_pApp->m_pInfoMsgDlg) {
						if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
							m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
					}
					return -1;
				}
			}
			else {
				bTempMsgRgnYes[2] = FALSE;
				bEnableRgn[3] |= FALSE;// pInspHandler->m_Inspection.m_TrainingData.bEnableCLDevLoc;
				bEnableRgn[3] |= FALSE;// (pInspHandler->m_Inspection.m_TrainingData.m_rectCoverLayer.Width() != 0) ? TRUE : FALSE;
			}

			if (pData->bEnableCLAlignmentPointer)
			{
				int nPointer = pData->nPointer;
				for (int nRect = 0; nRect < nPointer; nRect++)
				{
					rectTemp = CRect(100, 100, 400, 400);
					if (pData->m_rectPointer[nRect].Width() != 0)
						rectTemp = pData->m_rectPointer[nRect];

					DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTemp);
					DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
					strStatusBarMsg.Format("Draw Pointer %d", nRect+1);
					m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

					if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
						rectTemp = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRect);
					else {
						m_pApp->AddLogEvent("Teach Process Aborted");
						UpdateStatusBar(&CString("Teach Process Aborted"));
						if (m_pApp->m_pInfoMsgDlg) {
							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
						}
						ZoomSelectedDocView(nDocFrame);
						return -1;
					}
					pData->m_rectPointer[nRect] = rectTemp;
				}

				nError = pInspHandler->Teach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
					&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
					m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir,
					m_pApp->m_strConfigFile,
					6,
					bEnableRgn,
					FALSE,
					nTrack,
					nDoc,
					nFOV + 1,
					m_nNoOfFov,
					pDataFirst,
					pCalibData,
					FALSE);

				if (!nError) {
					////pInspHandler->SetPVIDefectParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, nFOV);
					pInspHandler->SaveTeachParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, m_pApp->m_strConfigFile, nFOV);
					strStatusBarMsg.Format("Pointers found");
					m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
					if (WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
						m_pApp->AddLogEvent("Teach Process Aborted");
						UpdateStatusBar(&CString("Teach Process Aborted"));
						if (m_pApp->m_pInfoMsgDlg) {
							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
						}
						ZoomSelectedDocView(nDocFrame);
						return -1;
					}
				}
				else if (nError) {
					strInfoMsg = strInfoMsg + " - Error: Cover Layer Alignment Teaching";

					AfxMessageBox("Teach Failed At " + strInfoMsg);
					m_pApp->AddLogEvent("Teach Process Aborted");
					UpdateStatusBar(&CString("Teach Process Aborted"));
					ZoomSelectedDocView(nDocFrame);
					DrawView();
					if (m_pApp->m_pInfoMsgDlg) {
						if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
							m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
					}
					ZoomSelectedDocView(nDocFrame);
					return -1;
				}

			}

			//// Tilt Teach ////
			if (nTrack < m_pApp->m_nTotalTracks - 1) {	//// only for Top & Bottom Camera ////
				if (pInspHandler->m_Inspection.m_TiltInspParm.bEnable) {
					CRect rectTempTilt = CRect(100, 100, 400, 400);
					if (pInspHandler->m_Inspection.m_TrainingData.m_rectTilt.Width() != 0)
						rectTempTilt = pInspHandler->m_Inspection.m_TrainingData.m_rectTilt;

					ZoomSelectedDocView(nDocFrame, TRUE);
					DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempTilt);
					DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

					strStatusBarMsg.Format("Locate Tilt Region");
					//UpdateStatusBar(&strStatusBarMsg);
					m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

					if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
						rectTempTilt = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, 0);
					else {
						m_pApp->AddLogEvent("Teach Process Aborted");
						UpdateStatusBar(&CString("Teach Process Aborted"));
						if (m_pApp->m_pInfoMsgDlg) {
							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
						}
						ZoomSelectedDocView(nDocFrame);
						return -1;
					}
					pInspHandler->m_Inspection.m_TrainingData.m_rectTilt = rectTempTilt;

					nError = pInspHandler->Teach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
						&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
						m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir,
						m_pApp->m_strConfigFile,
						5,
						bEnableRgn,
						FALSE,
						nTrack,
						nDoc,
						nFOV + 1,
						m_nNoOfFov,
						pDataFirst,
						pCalibData,
						FALSE);

					if (!nError) {
						////pInspHandler->SetPVIDefectParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, nFOV);
						pInspHandler->SaveTeachParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, m_pApp->m_strConfigFile, nFOV);
						strStatusBarMsg.Format("Regions found");
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
						if (WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
					}
					else if (nError) {
						strInfoMsg = strInfoMsg + " - Error: Tilt Teaching";

						AfxMessageBox("Teach Failed At " + strInfoMsg);
						m_pApp->AddLogEvent("Teach Process Aborted");
						UpdateStatusBar(&CString("Teach Process Aborted"));
						ZoomSelectedDocView(nDocFrame);
						DrawView();
						if (m_pApp->m_pInfoMsgDlg) {
							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
						}
						return -1;
					}
				}
			}

			if (pInspHandler->m_Inspection.m_TrainingData.bEnableSlot) {
				DrawView();
				DrawView(FALSE);
				for (int nRect = 0; nRect < pInspHandler->m_Inspection.m_TrainingData.nNumOfSlot; nRect++)
				{
					DrawView(FALSE);
					
					//TEACH SLOT REGION
					if (pInspHandler->m_Inspection.m_TrainingData.nSlotAreaType == RECTANGLE) {
						rectTemp = CRect(100, 100, 400, 400);
						if (pInspHandler->m_Inspection.m_TrainingData.m_rectSlot[nRect].Width() != 0)
							rectTemp = pInspHandler->m_Inspection.m_TrainingData.m_rectSlot[nRect];

						DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTemp);
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
						strStatusBarMsg.Format("Draw Slot %d", nRect + 1);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
							rectTemp = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, 0);
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
						pInspHandler->m_Inspection.m_TrainingData.m_rectSlot[nRect] = rectTemp;
					}
					//POLYGON TYPE
					else {
						int nRectTempIndexSlot = 0;
						int nOffset = 10;
						CRect rectTempSlotMask;
						for (int nPointId = 0; nPointId < pInspHandler->m_Inspection.m_TrainingData.nSlotPolygonPointCount; nPointId++) {
							CPoint PointTemp = pInspHandler->m_Inspection.m_TrainingData.m_SlotPolygonPoint[nRect][nPointId];
							rectTempSlotMask = CRect(PointTemp.x - nOffset, PointTemp.y - nOffset, PointTemp.x + nOffset, PointTemp.y + nOffset);
							DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempSlotMask, POLYGON);
						}
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
						strStatusBarMsg.Format("Draw Slot %d", nRect + 1);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);


						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
							for (int nPointId = 0; nPointId < pInspHandler->m_Inspection.m_TrainingData.nSlotPolygonPointCount; nPointId++) {
								rectTempSlotMask = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexSlot++);
								pInspHandler->m_Inspection.m_TrainingData.m_SlotPolygonPoint[nRect][nPointId] = rectTempSlotMask.CenterPoint();
							}
						}
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
					}
					
					//TEACH SLOT LIMIT REGION
					DrawView(FALSE);
					if (pInspHandler->m_Inspection.m_TrainingData.nSlotLimitAreaType == RECTANGLE) {
						rectTemp = CRect(100, 100, 400, 400);
						if (pInspHandler->m_Inspection.m_TrainingData.m_rectSlotLimit[nRect].Width() != 0)
							rectTemp = pInspHandler->m_Inspection.m_TrainingData.m_rectSlotLimit[nRect];

						DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTemp);
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
						strStatusBarMsg.Format("Draw Slot Limit %d", nRect + 1);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
							rectTemp = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, 0);
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
						pInspHandler->m_Inspection.m_TrainingData.m_rectSlotLimit[nRect] = rectTemp;

					}
					//POLYGON TYPE
					else {
						int nRectTempIndexSlot = 0;
						int nOffset = 10;
						CRect rectTempSlotMask;
						for (int nPointId = 0; nPointId < pInspHandler->m_Inspection.m_TrainingData.nSlotLimitPolygonPointCount; nPointId++) {
							CPoint PointTemp = pInspHandler->m_Inspection.m_TrainingData.m_SlotLimitPolygonPoint[nRect][nPointId];
							rectTempSlotMask = CRect(PointTemp.x - nOffset, PointTemp.y - nOffset, PointTemp.x + nOffset, PointTemp.y + nOffset);
							DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempSlotMask, POLYGON);
						}
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
						strStatusBarMsg.Format("Draw Slot Limit %d", nRect + 1);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);


						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
							for (int nPointId = 0; nPointId < pInspHandler->m_Inspection.m_TrainingData.nSlotLimitPolygonPointCount; nPointId++) {
								rectTempSlotMask = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexSlot++);
								pInspHandler->m_Inspection.m_TrainingData.m_SlotLimitPolygonPoint[nRect][nPointId] = rectTempSlotMask.CenterPoint();
							}
						}
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
					}
					

					nError = pInspHandler->Teach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
						&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
						m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir,
						m_pApp->m_strConfigFile,
						7 + nRect,
						bEnableRgn,
						FALSE,
						nTrack,
						nDoc,
						nFOV + 1,
						m_nNoOfFov,
						pDataFirst,
						pCalibData,
						FALSE);

					if (!nError) {
						////pInspHandler->SetPVIDefectParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, nFOV);
						pInspHandler->SaveTeachParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, m_pApp->m_strConfigFile, nFOV);
						strStatusBarMsg.Format("Slots found");
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
						if (WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
					}
					else if (nError) {
						strInfoMsg = strInfoMsg + " - Error: Slot Teaching";

						AfxMessageBox("Teach Failed At " + strInfoMsg);
						m_pApp->AddLogEvent("Teach Process Aborted");
						UpdateStatusBar(&CString("Teach Process Aborted"));
						ZoomSelectedDocView(nDocFrame);
						DrawView();
						if (m_pApp->m_pInfoMsgDlg) {
							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
						}
						ZoomSelectedDocView(nDocFrame);
						return -1;
					}
				}


				int nRectTempIndexSlot = 0;
				CRect rectTempSlotMask = CRect(100, 100, 400, 400);
				for (int nMaskIdx = 0; nMaskIdx < pInspHandler->m_Inspection.m_TrainingData.nSlotMaskNumber; nMaskIdx++) {
					nRectTempIndexSlot = 0;
					DrawView();
					DrawView(FALSE);
					if (pInspHandler->m_Inspection.m_TrainingData.nSlotMaskType[nMaskIdx] == RECTANGLE) {
						if (pInspHandler->m_Inspection.m_TrainingData.m_rectMaskSlot[nMaskIdx].Width() != 0 &&
							pInspHandler->m_Inspection.m_TrainingData.m_rectMaskSlot[nMaskIdx].Height() != 0)
							rectTempSlotMask = pInspHandler->m_Inspection.m_TrainingData.m_rectMaskSlot[nMaskIdx];

						DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempSlotMask);
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

						strStatusBarMsg.Format("Locate Rectangle Mask %d For Slot Surface", nMaskIdx + 1);
						//UpdateStatusBar(&strStatusBarMsg);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);

						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
							rectTempSlotMask = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexSlot++);
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
					}
					else if (pInspHandler->m_Inspection.m_TrainingData.nSlotMaskType[nMaskIdx] == ECLIPSE) {
						rectTempSlotMask = CRect(100, 100, 400, 400);
						if (pInspHandler->m_Inspection.m_TrainingData.m_rectMaskSlot[nMaskIdx].Width() != 0 &&
							pInspHandler->m_Inspection.m_TrainingData.m_rectMaskSlot[nMaskIdx].Height() != 0)
							rectTempSlotMask = pInspHandler->m_Inspection.m_TrainingData.m_rectMaskSlot[nMaskIdx];
						DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempSlotMask, 1);
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
						strStatusBarMsg.Format("Locate Ellipse Mask %d For Slot Surface", nMaskIdx + 1);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
							rectTempSlotMask = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexSlot++);
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
					}
					else if (pInspHandler->m_Inspection.m_TrainingData.nSlotMaskType[nMaskIdx] == POLYGON) {
						int nOffset = 10;
						for (int nPointId = 0; nPointId < pInspHandler->m_Inspection.m_TrainingData.nSlotMaskPolygonPointCount[nMaskIdx]; nPointId++) {
							CPoint PointTemp = pInspHandler->m_Inspection.m_TrainingData.m_SlotMaskPolygonPoint[nMaskIdx][nPointId];
							rectTempSlotMask = CRect(PointTemp.x - nOffset, PointTemp.y - nOffset, PointTemp.x + nOffset, PointTemp.y + nOffset);
							DrwTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rectTempSlotMask, POLYGON);
						}
						DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
						strStatusBarMsg.Format("Locate Polygon Mask %d For Slot Surface", nMaskIdx + 1);
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
						if (!WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
							for (int nPointId = 0; nPointId < pInspHandler->m_Inspection.m_TrainingData.nSlotMaskPolygonPointCount[nMaskIdx]; nPointId++) {
								rectTempSlotMask = GetTrackRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, nRectTempIndexSlot++);
								pInspHandler->m_Inspection.m_TrainingData.m_SlotMaskPolygonPoint[nMaskIdx][nPointId] = rectTempSlotMask.CenterPoint();
							}
						}
						else {
							m_pApp->AddLogEvent("Teach Process Aborted");
							UpdateStatusBar(&CString("Teach Process Aborted"));
							if (m_pApp->m_pInfoMsgDlg) {
								if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
									m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
							}
							ZoomSelectedDocView(nDocFrame);
							return -1;
						}
					}
					
					pInspHandler->m_Inspection.m_TrainingData.m_rectMaskSlot[nMaskIdx] = rectTempSlotMask;
				}
				nError = pInspHandler->Teach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
					&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
					m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir,
					m_pApp->m_strConfigFile,
					12,
					bEnableRgn,
					FALSE,
					nTrack,
					nDoc,
					nFOV + 1,
					m_nNoOfFov,
					pDataFirst,
					pCalibData,
					FALSE);

				if (!nError) {
					////pInspHandler->SetPVIDefectParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, nFOV);
					pInspHandler->SaveTeachParameters(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_strConfigDir, m_pApp->m_strConfigFile, nFOV);
					strStatusBarMsg.Format("Region found");
					m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
					if (WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
						m_pApp->AddLogEvent("Teach Process Aborted");
						UpdateStatusBar(&CString("Teach Process Aborted"));
						if (m_pApp->m_pInfoMsgDlg) {
							if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
								m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
						}
						ZoomSelectedDocView(nDocFrame);
						return -1;
					}
				}
				else if (nError) {
					strInfoMsg = strInfoMsg + " - Error: Slot Surface Masking";

					AfxMessageBox("Teach Failed At " + strInfoMsg);
					m_pApp->AddLogEvent("Teach Process Aborted");
					UpdateStatusBar(&CString("Teach Process Aborted"));
					ZoomSelectedDocView(nDocFrame);
					DrawView();
					if (m_pApp->m_pInfoMsgDlg) {
						if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
							m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
					}
					ZoomSelectedDocView(nDocFrame);
					return -1;
				}
			}
			//if ((bEnableRgn[0] || bEnableRgn[1] || bEnableRgn[2] || bEnableRgn[3]) && (bTempMsgRgnYes[0] || bTempMsgRgnYes[1] || bTempMsgRgnYes[2])) {
			//	strStatusBarMsg.Format("Regions found");
			//	m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
			//	if (WaitResponse(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo)) {
			//		m_pApp->AddLogEvent("Teach Process Aborted");
			//		UpdateStatusBar(&CString("Teach Process Aborted"));
			//		if (m_pApp->m_pInfoMsgDlg) {
			//			if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
			//				m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
			//		}
			//		return -1;
			//	}
			//}
		}
		
	}

	m_pApp->m_bTeachSingleImage = FALSE;

	m_pTrack->m_nFOVIndex = nFOVIdxBk;
	m_pApp->AddLogEvent("Teach Process Completed");
	UpdateStatusBar(&CString("Teach Process Completed"));
	if (m_pApp->m_pInfoMsgDlg) {
		if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
			m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
	}
	return nError;
}

void CAppDoc::ZoomSelectedDocView(int nDoc, BOOL bZoom)
{
	if(bZoom) {
		m_pApp->m_pFrames[nDoc]->MDIActivate();
		if(!m_pApp->m_pFrames[nDoc]->IsZoomed()){
			m_pApp->OnMaximize(FALSE);
			m_pApp->m_pFrames[nDoc]->MDIMaximize();
		}
	}
	else {
		if(m_pApp->m_pFrames[nDoc]->IsZoomed()){
			m_pApp->m_pFrames[nDoc]->MDIRestore();
			m_pApp->OnMaximize(TRUE);
		}
	}
}

void CAppDoc::DisplaySelectedDocView(int nDoc)
{
	if (!m_pApp->m_pFrames[nDoc]->IsZoomed()) {
		//m_pApp->m_pFrames[nDoc]->ActivateFrame();
		m_pApp->m_pFrames[nDoc]->MDIActivate();
	}
}

void CAppDoc::ComGrab(int nReserved) 
{
	CString strMsg;

	strMsg.Format("");
	UpdateStatusBar(&strMsg,2);
	UpdateStatusBar(&strMsg,3);

	m_pTrack->m_nCommand = COM_GRAB;
	m_pTrack->m_pActiveDoc = this;
	SetEvent(m_pTrack->m_EventRun);
}

void CAppDoc::ComInspect(int nDoc) 
{
	CString strMsg;

	strMsg.Format("");
	UpdateStatusBar(&strMsg,2);
	UpdateStatusBar(&strMsg,3);

	m_pTrack->m_nCommand = COM_INSPECT;
	m_pTrack->m_pActiveDoc = this;
	SetEvent(m_pTrack->m_EventRun);
}

void CAppDoc::DrawTrackStatus(BOOL bActive)
{
	DrwDel(&m_ImageViewInfo);

	if(!bActive) {
		LOGPEN TextColorRed;
		TextColorRed.lopnColor = RGB(255,0,0);
		TextColorRed.lopnStyle = PS_SOLID;
		TextColorRed.lopnWidth.x = 1;
		
		CString strMsg = "Inspection Disabled";
		
		DrwStr(&m_ImageViewInfo, &strMsg, &CPoint(20, 20), &TextColorRed, 120);
	}
	DrwUpdate(&m_ImageViewInfo);
}

int CAppDoc::Inspect(int nError)
{
	CPerformanceTimer timerInspect;
	timerInspect.Reset();
	
		if(m_bBinaryMode)
			SwitchImg(&m_ImageViewInfo, FALSE, NULL);

		m_pTrack->m_bOpticsRoi = FALSE;

		int nTrack, nDocCount;
		nTrack = m_pTrack->m_nTrack;
		nDocCount = m_pApp->m_nTotalViewDocs[nTrack];
		if(m_pApp->m_RuntimeDisplayEnable) {
			DrawView(FALSE);
			for(int z=0; z<nDocCount; z++)				
				m_pApp->m_pTracks[nTrack].m_pDoc[z]->m_bShowOverlay = TRUE;
		}

		if(!nError) {

			CTrainingData* pTrainingDataFirst = &m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[m_pTrack->m_nFOVIndex].m_Inspection.m_TrainingData;
			CInspectinonData* pInspData = &m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].m_Inspection.m_InspectionData;	
			
			CPerformanceTimer timer;
			nError = -DEVICE_LOCATION;/*m_InspectionHandler[m_pTrack->m_nFOVIndex].Inspect(&m_pTrack->m_SharedData,
														&m_pApp->m_pTracks[nTrack].m_pDoc[m_nDocIdx]->m_Buffer,
														m_lDebugFlag,
														FALSE,
														nTrack,
														m_nDocIdx,
														m_pTrack->m_nFOVIndex,
														m_pApp->m_nNoOfFov,
														pTrainingDataFirst,
														pInspData);*/

			double dTime = timer.GetElapsedMSec();
			OutputDebugLogTo(nTrack+1, TRUE,"[Image%d] RESULT: '%s'  (%3.2fms)", 
								 m_nDocIdx+1, m_InspectionHandler[m_pTrack->m_nFOVIndex].m_strErrorMsg, dTime);
	//		SetInspectionTime(&m_ImageViewInfo, dTime);
			DrawView();			
		}
		else {
			UpdateStatusBar(&CString("Image Snap Failure"),2);
			UpdateStatusBar(&CString("Fail Image Snap"),3);
		}

		if(m_lDebugFlag & DEBUG_STEP_MODE) {
			DrawView(FALSE);
			
			int nDocFrame = !nTrack?m_nDocIdx:m_pApp->m_nTotalViewDocs[nTrack-1]+0;
			if(!m_pApp->m_pFrames[nDocFrame]->IsZoomed()) {
				m_pApp->OnMaximize(FALSE);
				m_pApp->m_pFrames[nDocFrame]->MDIMaximize();
			}
			
			CImageBuffer bufBk[5];
			CRect rect = CRect(CPoint(0,0), m_Buffer.GetImgSize());
			for(int x=0; x<nDocCount; x++) {
				bufBk[x].Allocate(rect.Size());
				ImgCopy(&m_pApp->m_pTracks[nTrack].m_pDoc[x]->m_Buffer, &rect, &bufBk[x], &rect);
			}
			for(int x=0; x<nDocCount; x++) {
				COverlayArray* pArray = &m_InspectionHandler[m_pTrack->m_nFOVIndex].m_Inspection.m_arrayOverlay[0];
				int nCount = (int) pArray->GetCount();
				if(nCount) {			
					for(int nIndex=0; nIndex<nCount; nIndex++) {
						m_InspectionHandler[m_pTrack->m_nFOVIndex].m_Inspection.DebugStep(m_pApp->m_pTracks[nTrack].m_SharedData.pImageViews[x], 
																m_pApp->m_pTracks[nTrack].m_SharedData.pProcBuffers[x],//&m_Buffer, 
																nIndex,0);
						nDocFrame = !nTrack?m_nDocIdx:m_pApp->m_nTotalViewDocs[nTrack-1]+x;
						m_pApp->m_pFrames[nDocFrame]->MDIActivate();
						if(!m_pApp->m_pFrames[nDocFrame]->IsZoomed()) {
							m_pApp->OnMaximize(FALSE);
							m_pApp->m_pFrames[nDocFrame]->MDIMaximize();
						}

						if(!Navigate(nIndex,x)){
							x = nDocCount;
							break;
						}
					}		
				}
			}

			for(int x=0 ;x<nDocCount; x++) {
				ImgCopy(&bufBk[x], &rect, &m_pApp->m_pTracks[nTrack].m_pDoc[x]->m_Buffer, &rect);
				COverlayArray* pArray = &m_InspectionHandler[m_pTrack->m_nFOVIndex].m_Inspection.m_arrayOverlay[0];
				int nCount = (int) pArray->GetCount();
				/*for(int nIndex=0; nIndex<nCount; nIndex++) {
					if(!pArray->GetAt(nIndex).hImage.IsInitialized())
						DrawRegion(m_pApp->m_pTracks[nTrack].m_SharedData.pImageViews[x], pArray->GetAt(nIndex));
				}*/
				nDocFrame = !nTrack?m_nDocIdx:m_pApp->m_nTotalViewDocs[nTrack-1]+x;
				if(m_pApp->m_pFrames[nDocFrame]->IsZoomed()) {
					m_pApp->m_pFrames[nDocFrame]->MDIRestore();
					m_pApp->OnMaximize(TRUE);
				}
			}

			DrawView();
		}

		LOGPEN TextColorRed, TextColorGreen, TextColor, TextColorYellow;

		TextColorRed.lopnColor = RGB(230,50,50);
		TextColorRed.lopnStyle = PS_SOLID;
		TextColorRed.lopnWidth.x = 1;

		TextColorYellow.lopnColor = RGB(230,230,0);
		TextColorYellow.lopnStyle = PS_SOLID;
		TextColorYellow.lopnWidth.x = 1;

		TextColorGreen.lopnColor = RGB(50,230,50);
		TextColorGreen.lopnStyle = PS_SOLID;
		TextColorGreen.lopnWidth.x = 1;

		CString strDefectMsg;
		if(m_pApp->m_RuntimeDisplayEnable) {
		if(nError < GOOD_UNIT) {
			LOGPEN TextDefectColor;
			//int nCount=1;//// Initialized count to 1; for Process error
			//for(int nTrack=0; nTrack<m_pApp->m_nTotalTracks; nTrack++) {
				//CArray<int>* pArray = &pInsp->m_Inspection.nErrorCodeArray[nTrack];
				//nCount = pArray->GetCount();
				//for(int i=0; i<nCount; i++) {
					CString strDefectMsg;
					strDefectMsg.Format("%s FAIL",m_pApp->GetFailedStr(nError,FALSE));//m_pApp->GetFailedStr(nError1, false);
					//TextDefectColor = TextColorRed;//GetDefectcolor(pArray->GetAt(i));
					
					TextDefectColor = TextColorRed;//GetDefectcolor(pArray->GetAt(i));
					DrwStr(&m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_ImageViewInfo, &strDefectMsg, &CPoint(180, 20), &TextDefectColor, 100);
				//}
			//}
		}
		else {
			strDefectMsg = "PASS";//.Format(" PASS");
			TextColor = TextColorGreen;
			//for(int i=0; i<m_pApp->m_nTotalTracks; i++)
			DrwStr(&m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_ImageViewInfo, &strDefectMsg, &CPoint(180, 20), &TextColor, 120);
		}

		CRect rect = CRect(CPoint(80, 80), CSize(60, 60));
		BOOL bFill = m_nInspCounter % 2;
		if(bFill)
			rect.DeflateRect(6, 6);
		else
			TextColorYellow.lopnWidth.x = 10;
		//for(int i=0; i<m_pApp->m_nTotalTracks; i++) {
			DrwRect(&m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_ImageViewInfo, &rect, &TextColorYellow/*, bFill*/);
			DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_ImageViewInfo);
		//}
	}
	
	return nError;
}

int CAppDoc::InspectAllDocOnline(int nDeviceInsp, int nFovInsp)
{
	int nError = 0;
	CTrainingData* pTrainingDataFirst;
	CInspectinonData* pInspData;
	CCalibrationData* pCalibData;

	int nTrack, nErrInsp[5] = { NOT_INSPECTED,NOT_INSPECTED,NOT_INSPECTED,NOT_INSPECTED,NOT_INSPECTED };
	CStringArray strArrayInspValues;
	strArrayInspValues.RemoveAll();
	BOOL bStepDebug;

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;

	nTrack = m_pTrack->m_nTrack;
	pTrainingDataFirst = &m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[nFovInsp].m_Inspection.m_TrainingData;
	m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[nFovInsp].ClearTeachParam(pTrainingDataFirst);
	pInspData = &m_pApp->m_pTracks[0].m_pDoc[0]->m_InspectionHandler[0].m_Inspection.m_InspectionData;
	pCalibData = &m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[0].m_Inspection.m_CalibrationData;

	if (m_pApp->m_bPixelRuler)
	{
		m_pApp->m_pPixelRulerDlg->ShowWindow(SW_HIDE);
		m_pApp->m_bPixelRuler = FALSE;
	}
	if (m_lDebugFlag & DEBUG_STEP_MODE)
		bStepDebug = 1;

	DrawView(FALSE);
	if (m_bBinaryMode) {
		for (int nDoc = 0; nDoc < m_pApp->m_nTotalViewDocs[nTrack]; nDoc++)
			SwitchImg(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, FALSE, NULL);
	}

	m_pTrack->m_bOpticsRoi = FALSE;
	if (m_pApp->m_RuntimeDisplayEnable) {
		for (int nDoc = 0; nDoc < m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {			
			m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_bShowOverlay = TRUE;
		}
	}

	int nCurrentDoc = 0;
	if (m_pTrack->m_pActiveDocTeach != NULL)
		nCurrentDoc = m_pTrack->m_pActiveDocTeach->m_nDocIdx;

	pTrainingDataFirst->bCentralizedVerificationMode = m_pApp->m_bCentralizedVerificationMode;
	for (int nInsp = 0; nInsp < 2; nInsp++) {	//// 0-Device Loc Insp; 1-Rest All Insp [Region, Encap, Die, ...]; ////
		for (int nDoc = 0; nDoc < m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {

			if (!theApp.motionParameters.IsGrabEnabled(nTrack, nDoc, nFovInsp)) {
				HRegion hEmptyRegion;
				GenEmptyRegion(&hEmptyRegion);
				ConcatObj(pTrainingDataFirst->hObjectDeviceLocation, hEmptyRegion, &pTrainingDataFirst->hObjectDeviceLocation);
				TupleConcat(pTrainingDataFirst->hTupleProjectionParam, (HTuple(0).TupleConcat(0)).TupleConcat(0), &pTrainingDataFirst->hTupleProjectionParam);
				TupleConcat(pTrainingDataFirst->hTupleTeachDoc, 0, &pTrainingDataFirst->hTupleTeachDoc);
				TupleConcat(pTrainingDataFirst->hTupleDatumTeachTuple, 0, &pTrainingDataFirst->hTupleDatumTeachTuple);
				TupleConcat(pTrainingDataFirst->hTupleEdgeTeachTuple, 0, &pTrainingDataFirst->hTupleEdgeTeachTuple);

				nErrInsp[nDoc] = -NO_IMAGE;
				continue;
			}

			long lDebugFlagCurrent = 0;
			if (nDoc == nCurrentDoc)
				lDebugFlagCurrent = m_lDebugFlag;

			/*if (nDoc != m_pApp->m_pTracks[nTrack].m_pActiveDocTeach->m_nDocIdx)
				continue;*/
				//if (!nFovInsp && !nTrack && !nDoc && !nInsp) {
				//	m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*nFovInsp*/0].ClearInspParam(pInspData, FALSE);
				//	m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*nFovInsp*/0].ClearCalibParam(pCalibData, FALSE, TRUE);
				//}

			if (m_pApp->m_bFwdDirectionForTiltInsp) {
				if (!nFovInsp && !nTrack && !nDoc && !nInsp) {
					m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*nFovInsp*/0].ClearInspParam(pInspData, FALSE);
					m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*nFovInsp*/0].ClearCalibParam(pCalibData, FALSE, TRUE);
				}
			}
			else {
				if ((nFovInsp == m_nNoOfFov - 1) && !nTrack && !nDoc && !nInsp) {
					m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*nFovInsp*/0].ClearInspParam(pInspData, FALSE);
					m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*nFovInsp*/0].ClearCalibParam(pCalibData, FALSE, TRUE);
				}
			}

			if (!nDoc && !nInsp) {
				m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*nFovInsp*/0].ClearInspParam(pInspData, TRUE);
			}

			if ((!nFovInsp || nFovInsp == m_nNoOfFov - 1)) {
				if ((!nTrack && m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFovInsp].m_Inspection.m_TiltInspParm.bEnable && nInsp > 0) || //(nDoc==m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFovInsp].m_Inspection.m_TiltInspParm.nImageIndex - 1) && nInsp>0) ||
					(nTrack == m_pApp->m_nTotalTracks - 1 && !nDoc && nInsp > 0)) {
					m_pApp->m_pTracks[nTrack].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*nFovInsp*/0].GetCalibDataForInsp(pCalibData);
				}
			}



			CPerformanceTimer timer;
			CInspectionHandler* pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFovInsp];
			pInspHandler->m_Inspection.m_bEnableDLModelInspect = m_pApp->m_bEnableDLModelInspect;
			pInspHandler->m_Inspection.m_bUseDLModelResult = m_pApp->m_bUseDLModelResult;
			pInspHandler->m_Inspection.m_bRemoveBackGround = m_pApp->m_bRemoveBackGround;
			nErrInsp[nDoc] = pInspHandler->Inspect(&m_pTrack->m_SharedData,
				&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDeviceInsp][nFovInsp],
				&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_TeachBuffer[nFovInsp],
				lDebugFlagCurrent,
				!nInsp ? FALSE : TRUE,
				nTrack,
				nDoc,
				m_pApp->m_nTotalViewDocs[nTrack],
				nFovInsp + 1,
				m_nNoOfFov,
				pTrainingDataFirst,
				pInspData,
				pCalibData,
				m_pApp->m_bFwdDirectionForTiltInsp,
				&m_pApp->m_DeepLearningModule,
				strArrayInspValues);
			double dTime = timer.GetElapsedMSec();
			OutputDebugLogTo(nTrack + 1, TRUE, "[Image%d] RESULT: '%s'  (%3.2fms)", nDoc + 1, pInspHandler->m_strErrorMsg, dTime);	
		}
	}
	pTrainingDataFirst->bCentralizedVerificationMode = FALSE;

	for (int nCount = 0; nCount < strArrayInspValues.GetCount(); nCount++)
		m_strLogInspectionData.Add(strArrayInspValues.GetAt(nCount));
	SetEvent(m_EventLogInspectionData);
	




	nError = GOOD_UNIT;
	for (int nDoc = 0; nDoc < m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {

		CInspectionHandler* pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[m_pTrack->m_nFOVIndex];
		for (int j = 0; j < pInspHandler->m_Inspection.m_arrayOverlayInspection.GetCount(); j++) {
			if (!pInspHandler->m_Inspection.m_arrayOverlayInspection[j].hImage.IsInitialized() && nErrInsp[nDoc] != -NO_IMAGE)
			{
				DrawRegion(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo,
						pInspHandler->m_Inspection.m_arrayOverlayInspection[j]);
				LOGPEN TextColorGreen;
				TextColorGreen.lopnColor = RGB(50, 230, 50);
				TextColorGreen.lopnStyle = PS_SOLID;
				TextColorGreen.lopnWidth.x = 1;
				CString strDefectMsg = "PASS";//.Format(" PASS");
				//for(int i=0; i<m_pApp->m_nTotalTracks; i++)
				DrwStr(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &strDefectMsg, &CPoint(180, 20), &TextColorGreen, 120);
			}

		}

		if (nError == GOOD_UNIT) {
			if (nErrInsp[nDoc] < GOOD_UNIT)
				nError = nErrInsp[nDoc];
		}
		int nView;
		theApp.motionParameters.GetViewID(nTrack, nDoc, nView);
		if (nErrInsp[nDoc] == -NO_IMAGE)
			m_pApp->m_nErrorCode[nDeviceInsp][nFovInsp][nView] = NOT_INSPECTED;
		else
			m_pApp->m_nErrorCode[nDeviceInsp][nFovInsp][nView] = nErrInsp[nDoc];


		if (!theApp.motionParameters.IsGrabEnabled(nTrack, nDoc, nFovInsp)) {
			continue;
		}
	}

	m_pApp->UpdateStatisticsWindow();

	DrawView(TRUE);

	return nError;
}

int CAppDoc::InspectAllDoc(int nError)
{	
	CTrainingData* pTrainingDataFirst;
	CInspectinonData* pInspData;
	CCalibrationData* pCalibData;

	CPerformanceTimer timerInspect;
	LOGPEN TextColorRed, TextColorGreen, TextColor, TextColorYellow;
	int nTrack, nErrInsp[5] = {NOT_INSPECTED,NOT_INSPECTED,NOT_INSPECTED,NOT_INSPECTED,NOT_INSPECTED };
	CStringArray strArrayInspValues;
	strArrayInspValues.RemoveAll();
	BOOL bStepDebug;

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;

	TextColorRed.lopnColor = RGB(230,50,50);
	TextColorRed.lopnStyle = PS_SOLID;
	TextColorRed.lopnWidth.x = 1;
	TextColorYellow.lopnColor = RGB(230,230,0);
	TextColorYellow.lopnStyle = PS_SOLID;
	TextColorYellow.lopnWidth.x = 1;
	TextColorGreen.lopnColor = RGB(50,230,50);
	TextColorGreen.lopnStyle = PS_SOLID;
	TextColorGreen.lopnWidth.x = 1;

	nTrack = m_pTrack->m_nTrack;
	pTrainingDataFirst = &m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[m_pTrack->m_nFOVIndex].m_Inspection.m_TrainingData;
	m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[m_pTrack->m_nFOVIndex].ClearTeachParam(pTrainingDataFirst);
	pInspData = &m_pApp->m_pTracks[0].m_pDoc[0]->m_InspectionHandler[0].m_Inspection.m_InspectionData;
	pCalibData = &m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[0].m_Inspection.m_CalibrationData;

	if (m_pApp->m_bPixelRuler)
	{
		m_pApp->m_pPixelRulerDlg->ShowWindow(SW_HIDE);
		m_pApp->m_bPixelRuler = FALSE;
	}
	if (m_lDebugFlag & DEBUG_STEP_MODE)
		bStepDebug = 1;

	DrawView(FALSE);
	timerInspect.Reset();
	if(m_bBinaryMode) {
		for(int nDoc=0; nDoc<m_pApp->m_nTotalViewDocs[nTrack]; nDoc++)
			SwitchImg(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, FALSE, NULL);
	}

	m_pTrack->m_bOpticsRoi = FALSE;
	if(m_pApp->m_RuntimeDisplayEnable) {
		for(int nDoc=0; nDoc<m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
			//DrwDel(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);				
			m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_bShowOverlay = TRUE;
		}
	}

	int nCurrentDoc = 0;
	if(m_pTrack->m_pActiveDocTeach != NULL)
		nCurrentDoc = m_pTrack->m_pActiveDocTeach->m_nDocIdx;

	if(!nError) {
		for(int nInsp=0; nInsp<2; nInsp++) {	//// 0-Device Loc Insp; 1-Rest All Insp [Region, Encap, Die, ...]; ////
			for(int nDoc=0; nDoc<m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
				
				if (!theApp.motionParameters.IsGrabEnabled(nTrack, nDoc, m_pTrack->m_nFOVIndex)) {
					HRegion hEmptyRegion;
					GenEmptyRegion(&hEmptyRegion);
					ConcatObj(pTrainingDataFirst->hObjectDeviceLocation, hEmptyRegion, &pTrainingDataFirst->hObjectDeviceLocation);
					TupleConcat(pTrainingDataFirst->hTupleProjectionParam, (HTuple(0).TupleConcat(0)).TupleConcat(0), &pTrainingDataFirst->hTupleProjectionParam);
					TupleConcat(pTrainingDataFirst->hTupleTeachDoc, 0, &pTrainingDataFirst->hTupleTeachDoc);
					TupleConcat(pTrainingDataFirst->hTupleDatumTeachTuple, 0, &pTrainingDataFirst->hTupleDatumTeachTuple);
					TupleConcat(pTrainingDataFirst->hTupleEdgeTeachTuple, 0, &pTrainingDataFirst->hTupleEdgeTeachTuple);

					nErrInsp[nDoc] = -NO_IMAGE;
					continue;
				}

				long lDebugFlagCurrent = 0;
				if (nDoc == nCurrentDoc)
					lDebugFlagCurrent = m_lDebugFlag;

				/*if (nDoc != m_pApp->m_pTracks[nTrack].m_pActiveDocTeach->m_nDocIdx)
					continue;*/
				//if (!m_pTrack->m_nFOVIndex && !nTrack && !nDoc && !nInsp) {
				//	m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].ClearInspParam(pInspData, FALSE);
				//	m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].ClearCalibParam(pCalibData, FALSE, TRUE);
				//}

				if (m_pApp->m_bFwdDirectionForTiltInsp) {
					if (!m_pTrack->m_nFOVIndex && !nTrack && !nDoc && !nInsp) {
						m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].ClearInspParam(pInspData, FALSE);
						m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].ClearCalibParam(pCalibData, FALSE, TRUE);
					}
				}
				else {
					if ((m_pTrack->m_nFOVIndex == m_nNoOfFov - 1) && !nTrack && !nDoc && !nInsp) {
						m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].ClearInspParam(pInspData, FALSE);
						m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].ClearCalibParam(pCalibData, FALSE, TRUE);
					}
				}

				if (!nDoc && !nInsp) {
					m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].ClearInspParam(pInspData, TRUE);
				}

				if ((!m_pTrack->m_nFOVIndex || m_pTrack->m_nFOVIndex== m_nNoOfFov-1)) {
					if ((!nTrack && m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[m_pTrack->m_nFOVIndex].m_Inspection.m_TiltInspParm.bEnable && nInsp>0) || //(nDoc==m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[m_pTrack->m_nFOVIndex].m_Inspection.m_TiltInspParm.nImageIndex - 1) && nInsp>0) ||
						(nTrack==m_pApp->m_nTotalTracks-1 && !nDoc && nInsp>0)) {
							m_pApp->m_pTracks[nTrack].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].GetCalibDataForInsp(pCalibData);
					}
				}

				CPerformanceTimer timer;
				CInspectionHandler* pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[m_pTrack->m_nFOVIndex];
				pInspHandler->m_Inspection.m_bEnableDLModelInspect = m_pApp->m_bEnableDLModelInspect;
				pInspHandler->m_Inspection.m_bUseDLModelResult = m_pApp->m_bUseDLModelResult;
				pInspHandler->m_Inspection.m_bRemoveBackGround = m_pApp->m_bRemoveBackGround;
				nErrInsp[nDoc] = pInspHandler->Inspect(&m_pTrack->m_SharedData,
													&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
													&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_TeachBuffer[m_pTrack->m_nFOVIndex], 
													lDebugFlagCurrent, 
													!nInsp ? FALSE : TRUE,
													nTrack,
													nDoc,
													m_pApp->m_nTotalViewDocs[nTrack],
													m_pTrack->m_nFOVIndex+1,
													m_nNoOfFov,
													pTrainingDataFirst,
													pInspData,
													pCalibData,
													m_pApp->m_bFwdDirectionForTiltInsp,
													&m_pApp->m_DeepLearningModule,
													strArrayInspValues);
				
				DrawView(TRUE);
				double dTime = timer.GetElapsedMSec();
				OutputDebugLogTo(nTrack+1, TRUE,"[Image%d] RESULT: '%s'  (%3.2fms)", nDoc+1, pInspHandler->m_strErrorMsg, dTime);
		
//				SetInspectionTime(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, dTime);
				//DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
				
				//m_pApp->m_strLogInspectionData.Copy(strArrayInspValues);	
			}
		}
		for (int nCount = 0; nCount < strArrayInspValues.GetCount(); nCount++)
			m_strLogInspectionData.Add(strArrayInspValues.GetAt(nCount));
		SetEvent(m_EventLogInspectionData);
	}
	else {
		//UpdateStatusBar(&CString("Image Snap Failure"),2);
		//UpdateStatusBar(&CString("Fail Image Snap"),3);
		UpdateStatusBar(&CString("Inspection Error"),3);
	}
	if (m_lDebugFlag & DEBUG_STEP_MODE) {
		for (int nDoc = 0; nDoc<m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
			DrwDel(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
			DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
		}
		if (m_pApp->m_pInfoMsgDlg) {
			if (!m_pApp->m_pInfoMsgDlg->IsWindowVisible())
				m_pApp->m_pInfoMsgDlg->ShowWindow(TRUE);
		}
		BOOL bAborted = FALSE;

		int nDocFrame;
		theApp.motionParameters.GetViewID(nTrack, nCurrentDoc, nDocFrame);
	//	int nDocFrame = !nTrack ? nCurrentDoc : (nTrack == 1 ? m_pApp->m_nTop + nCurrentDoc : m_pApp->m_nTop + m_pApp->m_nBottom + nCurrentDoc);
		ZoomSelectedDocView(nDocFrame, TRUE);

		for (int nInsp = 0; nInsp < 2; nInsp++) {
			for (int nDoc = 0; nDoc < m_pApp->m_nTotalViewDocs[nTrack]; nDoc++)
			{
				/*if (nDoc != m_pApp->m_pTracks[nTrack].m_pActiveDocTeach->m_nDocIdx)
					continue;*/
				if (!theApp.motionParameters.IsGrabEnabled(nTrack, nDoc, m_pTrack->m_nFOVIndex)) {
					continue;
				}

				CString strInfoMsg;
				

				CInspectionHandler* pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[m_pTrack->m_nFOVIndex];
				COverlayArray* pArray = &pInspHandler->m_Inspection.m_arrayOverlay[nInsp];
				int nCount = (int)pArray->GetCount();
				CRect rect = CRect(CPoint(0, 0), m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer.GetImgSize());
				CImageBuffer bufBk;
				bufBk.Allocate(rect.Size());
				ImgCopy(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer, &rect, &bufBk, &rect);
				m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer.ClrImg();
				strInfoMsg.Format("%s - Position%d [%s]", !nTrack ? "Top Camera" : (nTrack == 1 ? "Bottom Camera" : "Side Camera"),
					m_pTrack->m_nFOVIndex + 1,
					!nTrack ? m_pApp->m_strTopCameraName.GetAt(nDoc) : (nTrack == 1 ? m_pApp->m_strBottomCameraName.GetAt(nDoc) : m_pApp->m_strSideCameraName.GetAt(nDoc)));

				if (nCount) {
					for (int nIndex = 0; nIndex < nCount; nIndex++) {
						COverlayData* pData = &pInspHandler->m_Inspection.m_arrayOverlay[nInsp].GetAt(nIndex);
						CString strStatusBarMsg;
						strStatusBarMsg = pData->strDebugMessage;
						m_pApp->UpdateInfoMsgDlg(strInfoMsg, strStatusBarMsg);
						m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer.ClrImg();
						pInspHandler->m_Inspection.DebugStep(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer, nIndex,nInsp);
						if (!Navigate(nIndex, nDoc)) {
							bAborted = TRUE;
							break;
						}
					}
				}
				ImgCopy(&bufBk, &rect, &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer, &rect);
				if (bAborted) {
					nDoc = m_pApp->m_nTotalViewDocs[nTrack];
					ZoomSelectedDocView(nDocFrame, FALSE);
					nInsp = 2;
					bAborted = FALSE;
				}
			}
		}
		
		ZoomSelectedDocView(nDocFrame, FALSE);

		if (m_pApp->m_pInfoMsgDlg) {
			if (m_pApp->m_pInfoMsgDlg->IsWindowVisible())
				m_pApp->m_pInfoMsgDlg->ShowWindow(FALSE);
		}
		for (int nDoc = 0; nDoc<m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
			DrwDel(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
			DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
		}
	}
	CString strDefectMsg;
	for(int nDoc=0; nDoc<m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {

		if (!theApp.motionParameters.IsGrabEnabled(nTrack, nDoc, m_pTrack->m_nFOVIndex)) {
			continue;
		}


		if(m_pApp->m_RuntimeDisplayEnable) {
			if (m_pApp->m_pWndPVIDefectParameters) {
				int nDocFrame;
	//			nDocFrame  = !nTrack ? nDoc : (nTrack == 1 ? m_pApp->m_nTop + nDoc : m_pApp->m_nTop + m_pApp->m_nBottom + nDoc);
				theApp.motionParameters.GetViewID(nTrack, nCurrentDoc, nDocFrame);
				if (m_pApp->m_pWndPVIDefectParameters->IsWindowVisible() && m_pApp->m_pWndPVIDefectParameters->m_bPVIDefectImgEnable[m_pTrack->m_nFOVIndex][nDocFrame] /*&& nErrInsp[nDoc]==-PVI_INSP*/) {
					m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[m_pTrack->m_nFOVIndex].m_Inspection.DisplayPVIDefectImage(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo,
																																		&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
																																		m_pApp->m_pWndPVIDefectParameters->m_ComboPviArea.GetCurSel());
				}
			}
			if(nErrInsp[nDoc] < GOOD_UNIT) {
				if (nErrInsp[nDoc] > -ERROR_CODES || nErrInsp[nDoc] == -PROCESS_ERROR) {
					strDefectMsg.Format("%s ", m_pApp->GetFailedStr(nErrInsp[nDoc], FALSE));
					DrwStr(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &strDefectMsg, &CPoint(240, 20), &TextColorRed, 80);
				}
				else {
					/*CInspectionHandler* pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[m_pTrack->m_nFOVIndex];
					int nTemp = (abs(nErrInsp[nDoc])) % 100;
					int nAreaTemp = (nTemp / 10) - 1;
					int nAreaDefectTemp = (nTemp % 10) - 1;
					CString strDefect = pInspHandler->m_Inspection.m_pPviInspectionSetup[nAreaTemp].m_pPviDefect[nAreaDefectTemp].strDefectName;
					CString strTemp;
					int nLength = strDefect.GetLength();
					strTemp = strDefect.Mid(strDefect.ReverseFind('_'), nLength);
					strTemp.Format("PVI_%s", strTemp);
					DrwStr(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &strTemp, &CPoint(240, 20), &TextColorRed, 80);*/
					CString strTemp("PVI Defect");
					if (m_pApp->m_mapPviDefectsStatsGeneral.find(nErrInsp[nDoc]) != m_pApp->m_mapPviDefectsStatsGeneral.end()) {
						strTemp.Format("PVI_%s", m_pApp->m_mapPviDefectsStatsGeneral[nErrInsp[nDoc]]);
					}
					DrwStr(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &strTemp, &CPoint(240, 20), &TextColorRed, 80);
				}
				
			}
			else {
				strDefectMsg = "PASS";
				DrwStr(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &strDefectMsg, &CPoint(240, 20), &TextColorGreen, 80);
			}

			CRect rect = CRect(CPoint(80, 80), CSize(60, 60));
			BOOL bFill = m_nInspCounter % 2;
			if(bFill)
				rect.DeflateRect(6, 6);
			else
				TextColorYellow.lopnWidth.x = 10;
			DrwRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rect, &TextColorYellow/*, bFill*/);
			//DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
		}
		Sleep(m_pApp->m_nInSpectionDelay);
	}

	nError = GOOD_UNIT;
	for(int nDoc=0; nDoc<m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
		if(m_pApp->m_pWndPVIDefectParameters) {
			if(m_pApp->m_pWndPVIDefectParameters->IsWindowVisible() && 
			   m_pApp->m_pWndPVIDefectParameters->m_bPVIDefectImgEnable[m_pTrack->m_nFOVIndex][nDoc] //&&
			   /*nErrInsp[nDoc] == -PVI_INSP*/)
					continue;
		}
		CInspectionHandler* pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[m_pTrack->m_nFOVIndex];
		for(int j=0; j<pInspHandler->m_Inspection.m_arrayOverlayInspection.GetCount(); j++) {
			if(!pInspHandler->m_Inspection.m_arrayOverlayInspection[j].hImage.IsInitialized() && nErrInsp[nDoc] != -NO_IMAGE)
				DrawRegion(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, 
                    pInspHandler->m_Inspection.m_arrayOverlayInspection[j]);
		}
		if (nError == GOOD_UNIT) {
			if (nErrInsp[nDoc] < GOOD_UNIT)
				nError = nErrInsp[nDoc];
		}
		//nErrInsp[nDoc] = -11111;
//		int nView = !nTrack ? nDoc : (nTrack == 1 ? m_pApp->m_nTop + nDoc : m_pApp->m_nTop + m_pApp->m_nBottom + nDoc);
		int nView;
		theApp.motionParameters.GetViewID(nTrack, nDoc, nView);
		if(nErrInsp[nDoc] ==-NO_IMAGE)
			m_pApp->m_nErrorCode[m_pTrack->m_nDieIndex][m_pTrack->m_nFOVIndex][nView] = NOT_INSPECTED;
		else
			m_pApp->m_nErrorCode[m_pTrack->m_nDieIndex][m_pTrack->m_nFOVIndex][nView] = nErrInsp[nDoc];


		if (!theApp.motionParameters.IsGrabEnabled(nTrack, nDoc, m_pTrack->m_nFOVIndex)) {
			continue;
		}

		/*if (!m_pApp->m_bOnline || m_pApp->m_bDisplayAllStatsOffline) {
			m_pApp->UpdateLotSummary(m_pTrack->m_nDieIndex, m_pTrack->m_nFOVIndex, nView, nErrInsp[nDoc], !m_pApp->m_bOnline);
		}*/
	}
	
	//if (!m_pApp->m_bOnline || m_pApp->m_bDisplayAllStatsOffline)
	//	m_pApp->UpdateStatisticsWindow();
	DrawView(TRUE);

	m_nInspCounter++;
	if (m_nInspCounter == 100)
		m_nInspCounter = 0;

	return nError;
}

void CAppDoc::InspectionResultAllDoc(int nStationId, int nDie)
{
	int nTrack = m_pTrack->m_nTrack;
	CString strDefectMsg;
	LOGPEN TextColorRed, TextColorGreen, TextColor, TextColorYellow, TextColorCyan;
	TextColorRed.lopnColor = RGB(230, 50, 50);
	TextColorRed.lopnStyle = PS_SOLID;
	TextColorRed.lopnWidth.x = 1;
	TextColorYellow.lopnColor = RGB(230, 230, 0);
	TextColorYellow.lopnStyle = PS_SOLID;
	TextColorYellow.lopnWidth.x = 1;
	TextColorGreen.lopnColor = RGB(50, 230, 50);
	TextColorGreen.lopnStyle = PS_SOLID;
	TextColorGreen.lopnWidth.x = 1;
	TextColorCyan.lopnColor = RGB(0, 255, 255);
	TextColorCyan.lopnStyle = PS_SOLID;
	TextColorCyan.lopnWidth.x = 4;

	m_pTrack->m_bOpticsRoi = FALSE;
	if (m_pApp->m_RuntimeDisplayEnable) {
		for (int nDoc = 0; nDoc < m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {	
			m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_bShowOverlay = TRUE;
		}
	}

	OutputDebugLogTo(1, TRUE, "InspectionResultAllDoc For Track %d, FOV %d", nTrack + 1, m_pTrack->m_nFOVIndex + 1);
	for (int nDoc = 0; nDoc < m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
		int nView;
		theApp.motionParameters.GetViewID(nTrack, nDoc, nView);
		int nErrorInsp = m_pApp->m_nErrorCode[m_pTrack->m_nDieIndex][m_pTrack->m_nFOVIndex][nView];

		OutputDebugLogTo(1, TRUE, "InspectionResultAllDoc Error Inspection For Doc %d: %d", nDoc, nErrorInsp);

		if (nErrorInsp < GOOD_UNIT) {
			if (nErrorInsp > -ERROR_CODES || nErrorInsp == -PROCESS_ERROR) {
				strDefectMsg.Format("%s ", m_pApp->GetFailedStr(nErrorInsp, FALSE));
				DrwStr(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &strDefectMsg, &CPoint(240, 20), &TextColorRed, 80);
			}
			else {
				CString strTemp("PVI Defect");
				if (m_pApp->m_mapPviDefectsStatsGeneral.find(nErrorInsp) != m_pApp->m_mapPviDefectsStatsGeneral.end()) {
					strTemp.Format("PVI_%s", m_pApp->m_mapPviDefectsStatsGeneral[nErrorInsp]);
				}
				DrwStr(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &strTemp, &CPoint(240, 20), &TextColorRed, 80);
			}

			//Defect center and bounding box
			CInspectionHandler* pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[m_pTrack->m_nFOVIndex];
			
			for (int i = 0; i < pInspHandler->m_Inspection.m_CentralizedDefectData[nStationId][nDie][nDoc].arrayDefectCenters.size(); i++) {
				BOOL bEncapOverlay = FALSE;
				if ((abs(nErrorInsp) == ENCAP_INSUFFICIENT || abs(nErrorInsp) == ENCAP_EXCESS ||
					abs(nErrorInsp) == ENCAP_CRACK || abs(nErrorInsp) == ENCAP_BLOWHOLE || abs(nErrorInsp) == ENCAP_CONTAMINATION)) {
					if (i < pInspHandler->m_Inspection.m_CentralizedDefectData[nStationId][nDie][nDoc].arrayDefectX.size()) {
						if (pInspHandler->m_Inspection.m_CentralizedDefectData[nStationId][nDie][nDoc].arrayDefectX[i].size() > 0 &&
							pInspHandler->m_Inspection.m_CentralizedDefectData[nStationId][nDie][nDoc].arrayDefectX[i][0] == -1) {
							bEncapOverlay = TRUE;
						}
					}
				}

				if (bEncapOverlay) {
					DrwRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &pInspHandler->m_Inspection.m_CentralizedDefectData[nStationId][nDie][nDoc].arrayDefectRects[i], &TextColorCyan);
				}
				else {
					DrwRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &pInspHandler->m_Inspection.m_CentralizedDefectData[nStationId][nDie][nDoc].arrayDefectRects[i], &TextColorRed);
					DrwCur(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &pInspHandler->m_Inspection.m_CentralizedDefectData[nStationId][nDie][nDoc].arrayDefectCenters[i], &TextColorYellow, 1);
				}
			}

			for (int i = 0; i < pInspHandler->m_Inspection.m_CentralizedDefectData[nStationId][nDie][nDoc].arrayDefectX.size(); i++) {
				DrawRegion(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, pInspHandler->m_Inspection.m_CentralizedDefectData[nStationId][nDie][nDoc].arrayDefectX[i],
					pInspHandler->m_Inspection.m_CentralizedDefectData[nStationId][nDie][nDoc].arrayDefectY[i]);
			}
		}
		else {
			/*CInspectionHandler* pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[m_pTrack->m_nFOVIndex];
			pInspHandler->m_Inspection.CleanDefectData(nDoc);*/
			strDefectMsg = "PASS";
			DrwStr(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &strDefectMsg, &CPoint(240, 20), &TextColorGreen, 80);
		}
		CRect rect = CRect(CPoint(80, 80), CSize(60, 60));
		BOOL bFill = m_nInspCounter % 2;
		if (bFill)
			rect.DeflateRect(6, 6);
		else
			TextColorYellow.lopnWidth.x = 10;
		DrwRect(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &rect, &TextColorYellow);
	}

	DrawView(TRUE);

	m_nInspCounter++;
	if (m_nInspCounter == 100)
		m_nInspCounter = 0;
}

void CAppDoc::UpdateImages(CString *strZoneDefect,CString strResult)
{
	//bufImageUpdate = m_pApp->m_SharedData[m_pApp->m_pTracks->m_nTrack].pProcBuffers;
	bufImageUpdate = m_pTrack->m_SharedData.pProcBuffers;
	m_strZoneDefect = strZoneDefect;
	m_strResult = strResult;
	//SetEvent(m_EventImageUpdate);
}

int CAppDoc::UpdateImageToOriginal()
{
	CRect rect = CRect(CPoint(0,0), m_Buffer.GetImgSize());
	int nTrack = m_pTrack->m_nTrack;
	for(int nDoc=0; nDoc<m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
		DrwDel(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
		ImgCopy(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_bufPVIBackup, &rect, &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer, &rect);

		////CString str;
		////str.Format("C:\\Rakshith\\TempDelete\\OrgFOV%dTrack%dDoc%d.bmp", m_pTrack->m_nFOVIndex+1, nTrack+1, nDoc+1);
		////SaveGrayScaleBMPFile(str, m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_bufPVIBackup);
		////m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_bufPVIBackup.ClrImg();
		
		CInspectionHandler* pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[m_pTrack->m_nFOVIndex];
		for(int j=0; j<pInspHandler->m_Inspection.m_arrayOverlayInspection.GetCount(); j++) {
			if(!pInspHandler->m_Inspection.m_arrayOverlayInspection[j].hImage.IsInitialized())
				DrawRegion(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, pInspHandler->m_Inspection.m_arrayOverlayInspection[j]);
		}
		DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
	}
	
	return 0;
}

int CAppDoc::SaveOfflineImages(int nError,CString strImagePath,CString strImageName,CString strFolderName)
{
	CString strFolder, str1, str2, str3, str4,strRecipeFolder,strTemp,strImageDesc,strFile;
	str4 = "";
	str1 = m_pApp->m_ImageSaveFolder + "\\Sorted StoredImages";
	_mkdir(str1);
	strRecipeFolder.Format("%s\\%s", str1,m_pApp->m_strConfigFile);
	_mkdir(strRecipeFolder);
	strTemp.Format("%s"/*\\%s*/, strRecipeFolder/*, m_pApp->m_strLotInfo*/);
	_mkdir(strTemp);
	if(nError){
			str3.Format("%s\\FAIL", strTemp);
			_mkdir(str3);
	}
	else {
			str3.Format("%s\\PASS", strTemp);
			_mkdir(str3);
	}
	str4.Format("%s%s", str3, strFolderName);
	_mkdir(str4);
	if(!str4.IsEmpty()) {
		strFolder.Format("%s\\%s", str4, strImageName);
		CopyFileA(strImagePath, strFolder, FALSE);
	}
	
	return 0;
}

void CAppDoc::OnCloseDocument() 
{
	m_Buffer.Free();
	m_bufPVIBackup.Free();

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;

	for (int nFOV = 0; nFOV < m_nNoOfFov; nFOV++)
		m_TeachBuffer[nFOV].Free();

	int nNoOfDies = nDevicePerX * nDevicePerY;
	if (!m_pApp->m_bIsMasterApp) {
		nNoOfDies = MAX_DIES;
		m_nNoOfFov = MAX_FOV;
	}

	for (int i = 0; i < nNoOfDies; i++) {
		for (int j = 0; j < m_nNoOfFov; j++) {
			m_BufferDie[i][j].Free();

			//Buffer Die for Centralized Reject PC
			for (int iPort = 0; iPort < NO_OF_PORT_CENTRALIZED; iPort++) {
				m_CentralizedBufferDie[iPort][i][j].Free();
			}
		}
	}

	if (m_bLiveImageSingle) {
		m_bLiveImageSingle = FALSE;
		SetEvent(m_EventKillLiveSingle);
	}

	if(m_pApp->m_bAllocFailedImageBuffer){
	  for(int i =0;i<25;i++)
		  m_FailStack[i].bufFailImage.Free();
	}

	if(m_pTrack != NULL) {
		m_pTrack->m_pDoc[m_nDocIdx] = NULL;

		if(m_pTrack->m_bLiveImage) {
			m_pTrack->m_bLiveImage = FALSE;
			SetEvent(m_pTrack->m_EventKillLive);
		}
		if(m_pTrack->m_bLiveCalibration) {
			m_pTrack->m_bLiveCalibration = FALSE;
			SetEvent(m_pTrack->m_EventKillLiveCalibration);
		}
		if(m_pTrack->m_bContInsp) {
			m_pTrack->m_bContInsp = FALSE;
			SetEvent(m_pTrack->m_EventKillInspCycle);
		}
		if(m_pTrack->m_bTrackContInsp) {
			m_pTrack->m_bTrackContInsp = FALSE;
			SetEvent(m_pTrack->m_EventKillInspCycle);
		}
		if(m_pTrack->m_bAutoRun) {
			m_pTrack->m_bAutoRun = FALSE;
			SetEvent(m_pTrack->m_EventKillAutoRun);
		}
		if(m_bInspSavedImage)
			InspectStoredImages(FALSE);
		CString str;
		str.Format("ThresholdLow %d_%d", m_pTrack->m_nTrack+1, m_nDocIdx+1);
		PutRegistryEntry(READ_REGPATH + "Preferences\\Persistence", str, m_nThreshold.nLow);
		str.Format("ThresholdHigh %d_%d", m_pTrack->m_nTrack + 1, m_nDocIdx + 1);
		PutRegistryEntry(READ_REGPATH + "Preferences\\Persistence", str, m_nThreshold.nHigh);
	}
	m_bExitLogEnry = TRUE;
	SetEvent(m_EventLogInspectionData);
	//m_InspectionHandler.CloseDocument();
	m_InspectionHandler[m_pTrack->m_nFOVIndex].m_Inspection.Close();

	CloseHandle(m_EventInspEnd);

	CEvImageDoc::OnCloseDocument();
}

int CAppDoc::ContinuousInspect()
{
	if(m_pTrack->m_bContInsp) {
		m_pTrack->m_bContInsp = FALSE;
		SetEvent(m_pTrack->m_EventKillInspCycle);
	}
	else {
		m_pTrack->m_bContInsp = TRUE;
		AfxBeginThread(DocInspectCycle, this, THREAD_PRIORITY_LOWEST);
	}
	return 0;
}

void CAppDoc::SetOpticsCharacteristics(BOOL bForceCalculation)
{

	if (m_pTrack->m_bViewOpticsPanel || bForceCalculation) {
		BOOL bChanged = FALSE;
		if (m_pTrack->m_bOpticsRoi) {
			if (m_bTrackRectangle) {
				for (int i = 0; i<m_pTrack->m_nOpticsFrames; i++) {
					CRect rect = GetTrackRect(&m_ImageViewInfo, i);
					if (m_pTrack->m_rectOptics[i] != rect) {
						m_pTrack->m_rectOptics[i] = rect;
						bChanged = TRUE;
					}
				}
			}
			else {
				m_pTrack->m_bOpticsRoi = FALSE;
				//m_pTrack->m_rectOptics[0] = GetTrackRect(&m_ImageViewInfo, 0);
			}
			if (bChanged)
				m_pTrack->m_nOpticsRecalcMode = 2;
			//OutputDebugLog("SET OPTICS PARAMETERS1");
		}

		///Udupa; TODO
		if (m_pTrack->m_nOpticsRecalcMode != 0) {
			COpticsDlg* pOpticsDlg = &((CMainFrame*)AfxGetMainWnd())->m_wndOpticsPanel;
			double dMean, dNoise, dFocus;
			OPTICS_PARM OpticsParm;

			//if(pOpticsDlg->m_nCircular){
			for (int i = 0; i<m_pTrack->m_nOpticsFrames; i++) {
				if (OpticsCharacteristics(&m_Buffer, &m_pTrack->m_rectOptics[i], dMean, dNoise, dFocus) != -1) {
					OpticsParm.nIndex = i;
					OpticsParm.dMean = dMean;
					OpticsParm.dSNR = dNoise;
					OpticsParm.dFocus = dFocus;
					OpticsParm.nCalcMode = m_pTrack->m_nOpticsRecalcMode;
					//					if(::IsWindow(pOpticsDlg->m_hWnd))
					pOpticsDlg->SendMessage(WM_OPTICS_SETPARM, WPARAM(&OpticsParm));
					//else
				}
				dNoise = pOpticsDlg->m_dOpticsSNR[0];
			}
			pOpticsDlg->SendMessage(WM_OPTICS_REFRESHGRAPH, WPARAM(/*1*/m_pTrack->m_nOpticsFrames));
			m_pTrack->m_nOpticsRecalcMode = 0;
		}
		if (bChanged) {
			COpticsDlg* pOpticsDlg = &((CMainFrame*)AfxGetMainWnd())->m_wndOpticsPanel;
			DelStr(&m_ImageViewInfo);
			DelRect(&m_ImageViewInfo);
			DrawOpticsROIs(FALSE);
			DrwUpdate(&m_ImageViewInfo);
		}
	}

}

void CAppDoc::DrawOpticsROIs(BOOL bDrawRect)
{
	///Udupa; TODO
	LOGPEN TextColorGreen;
	CString strMsg;
	CPoint strPosOuter;
	CPoint strPosInner;
	//	int m_dUpperGapWidth,m_dLowerGapWidth;

	////m_dUpperGapWidth=20;
	////m_dLowerGapWidth=20;
	TextColorGreen.lopnStyle = PS_DOT;
	TextColorGreen.lopnWidth.x = 1;

	COpticsDlg* pOpticsDlg = &((CMainFrame*)AfxGetMainWnd())->m_wndOpticsPanel;
	pOpticsDlg->m_bCheckROI = (m_pTrack->m_bOpticsRoi == TRUE);
	//if(pOpticsDlg->m_nCircular){
	for (int i = 0; i<m_pTrack->m_nOpticsFrames; i++) {
		if (bDrawRect)
			DrwTrackRect(&m_ImageViewInfo, &m_pTrack->m_rectOptics[i]);
		TextColorGreen.lopnColor = RGB(0, 255, 0);
		strMsg.Format("%d", i + 1);
		strPosOuter.x = m_pTrack->m_rectOptics[i].right - 16;
		strPosOuter.y = m_pTrack->m_rectOptics[i].top - 38;
		DrwStr(&m_ImageViewInfo, &strMsg, &strPosOuter, &TextColorGreen, 30);
		TextColorGreen.lopnColor = pOpticsDlg->m_clrROI[i];
		DrwRect(&m_ImageViewInfo, &CRect(CPoint(strPosOuter + CPoint(-10, 0)), CSize(35, 38)), &TextColorGreen);
	}
	if (bDrawRect) {
		SetTrackRectProperties(&m_ImageViewInfo, TRUE, FALSE, TRUE);
	}
}

void CAppDoc::SaveAllGrabbedImages(int nTrackIndex)
{
	CString strImageDir, strTemp;
	
	
	strImageDir.Format("%s\\IMAGES",m_pApp->m_LotSummaryRootDir);
	_mkdir(strImageDir);
	strTemp = strImageDir;
    strImageDir.Format("%s\\Track%d",strTemp, nTrackIndex+1);
	_mkdir(strImageDir);
	
    strTemp = strImageDir;
	
	//////////////////////////////////////////
	CString strYear, strMonth, strDay, strHr, strMin, strSec, strMilSec;
	CString strFile;
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	strYear.Format("%d",sysTime.wYear);
	if(sysTime.wMonth < 10)
		strMonth.Format("0%d", sysTime.wMonth);
	else
		strMonth.Format("%d", sysTime.wMonth);
	
	if(sysTime.wDay < 10)
		strDay.Format("0%d", sysTime.wDay);
	else
		strDay.Format("%d", sysTime.wDay);

	if(sysTime.wHour < 10)
		strHr.Format("0%d", sysTime.wHour);
	else
		strHr.Format("%d", sysTime.wHour);

	if(sysTime.wMinute < 10)
		strMin.Format("0%d", sysTime.wMinute);
	else
		strMin.Format("%d", sysTime.wMinute);
	
	if(sysTime.wSecond < 10)
		strSec.Format("0%d", sysTime.wSecond);
	else
		strSec.Format("%d", sysTime.wSecond);

	if(sysTime.wMilliseconds< 10)
		strMilSec.Format("00%d", sysTime.wMilliseconds);
	else
		if(sysTime.wMilliseconds < 100 && sysTime.wMilliseconds>= 10)
			strMilSec.Format("0%d", sysTime.wMilliseconds);
		else
			strMilSec.Format("%d", sysTime.wMilliseconds);

	strFile = strYear + strMonth + strDay + strHr + strMin + strSec + strMilSec + ".bmp";

	strImageDir.Format("%s\\%s",strTemp, strFile);
	SaveGrayScaleBMPFile(strImageDir, m_pTrack->m_pDoc[0]->m_Buffer);
	

	//////////////////////////////////////////
		
}

void CAppDoc::SaveDocDLImages(int nTrack, int nDoc, int nDie, int nFov, int nErrorCode)
{
	CInspectionHandler* pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov];
	m_pApp->m_pImageQueue->PushImageBufferDL(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDie][nFov],
		nTrack,
		nDoc,
		nDie,
		nFov,
		nErrorCode,
		pInspHandler->m_Inspection.m_arrDefectRects,
		pInspHandler->m_Inspection.m_arrDefectRegions,
		pInspHandler->m_Inspection.m_DefectData[nDoc].arrayDefectRects,
		pInspHandler->m_Inspection.m_DefectData[nDoc].arrayDefectCenters);

	//pInspHandler->m_Inspection.CleanDefectData(nDoc);
}

void CAppDoc::SaveDocImage(int nTrack,int nDoc,int nDie,int nFov, int nErrorCode)
{
	m_pApp->m_pImageQueue->PushImageBuffer(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDie][nFov], 
		nTrack,
		nDoc,
		nDie,
		nFov,
		nErrorCode,
		m_InspectionHandler[m_pTrack->m_nFOVIndex].m_Inspection.m_arrDefectRects,
		m_InspectionHandler[m_pTrack->m_nFOVIndex].m_Inspection.m_arrDefectRegions);
}

void CAppDoc::SaveDocImage(int nDie, int nFov) {

	CString strTemp;
	CString strFile, strTemp1, strImageDesc;
	CString m_strImageFolders;
	strImageDesc.Format("HP_FCI_THA%d_Pos%d_", nDie + 1, nFov + 1);
	//strImageDesc = m_pApp->GetFailedStr(pBuf->m_nErrorCode,FALSE);
	switch (m_pTrack->m_nTrack) {
	case 0:		strTemp.Format("T%d", m_nDocIdx + 1);
		strImageDesc.Append(strTemp);
		break;
	case 1: strTemp.Format("B%d", m_nDocIdx + 1);
		strImageDesc.Append(strTemp);
		break;
	case 2:	strTemp.Format("S%d", m_nDocIdx + 1);
		strImageDesc.Append(strTemp);
		break;
	}

	m_strImageFolders.Format("%s\\%s", m_pApp->m_strCentralizedFTPSharedDirectory, m_pApp->m_strMachineID);
//	_mkdir(m_strImageFolders);

	CString strFolder, strRecipeFolder, strImage;
	strRecipeFolder.Format("%s\\%s", m_strImageFolders, m_pApp->m_strConfigFile);

//	_mkdir(strRecipeFolder);
	//strTemp.Format("%s\\Lot1", strRecipeFolder);			//Test Lot1
	//_mkdir(strTemp);
	//CreateDirectoryTree(strTemp);
	///*BOOL bPass = !IS_FAIL(pBuf->m_nErrorCode);
	//if (!bPass)
	//	strFolder.Format("%s\\FAIL", strTemp);
	//else
	//	strFolder.Format("%s\\PASS", strTemp);*/
	//_mkdir(strFolder);

	strTemp1.Format("%s\\%s", strRecipeFolder, m_pApp->m_strPartFlex[nDie]);
//	_mkdir(strTemp1);

	if(nFov == 0 && m_pTrack->m_pDoc[0]->m_nDocIdx == 0)		//To save time
		CreateDirectoryTree(strTemp1);

	if (!m_bEnableGrab[nFov])
		return;

	strFile.Format("%s\\%s", strTemp1, strImageDesc);
//	if ((bPass && m_pApp->m_bSaveOnlinePassImages && m_pApp->m_bSaveOnlineBMPImages) || (!bPass && m_pApp->m_bSaveOnlineFailImages && m_pApp->m_bSaveOnlineBMPImages)) {
		SaveGrayScaleBMPFile(strFile + ".bmp", m_BufferDie[nDie][nFov]);
		OutputDebugLog("%s saved!", strFile);
//	}
	//if ((bPass && m_pApp->m_bSaveOnlinePassImages && m_pApp->m_bSaveOnlineJpegImages) || (!bPass && m_pApp->m_bSaveOnlineFailImages && m_pApp->m_bSaveOnlineJpegImages)) {
	//	DrawResultText(pBuf, &sysTime);
	//	SaveJPGFile(strFile + ".jpg", m_BufDefect, m_pApp->m_nSaveJpegQualityImages);
	//}
}

void CAppDoc::LoadImagesForVerfication(CString Path, int nDieNo)
{

	CString m_strImageFolders, strTemp;
	m_strImageFolders.Format("%s\\%s", Path, m_pApp->m_strMachineID);
	_mkdir(m_strImageFolders);

	CString strFolder, strRecipeFolder, strImage;
	strRecipeFolder.Format("%s\\%s", m_strImageFolders, m_pApp->m_strConfigFile);
//	strTemp.Format("%s\\%s", strRecipeFolder, m_pApp->m_strLotInfo);
	/*BOOL bPass = !IS_FAIL(pBuf->m_nErrorCode);
	if (!bPass)
	strFolder.Format("%s\\FAIL", strTemp);
	else
	strFolder.Format("%s\\PASS", strTemp);*/
//	_mkdir(strFolder);

	strImage.Format("%s\\%s", strRecipeFolder, m_pApp->m_strPartFlex[nDieNo]);

	CString strImageName, strPos;
	OutputDebugLogTo(3, TRUE, "Image Read Path: %s", strImage);

	GetImageFilePaths(strImage + "\\", m_pApp->m_strLoadMultiImagesArray);
	strImageName = m_pApp->m_strLoadMultiImagesArray.GetAt(0);
	int nTempLen, nTempSel;
	int nPos = 0;
	strPos = strImageName.Mid(strImageName.ReverseFind('P'), 4);
	nTempLen = strPos.GetLength();
	m_pTrack->m_charTempPosNameForInsp = strPos[nTempLen - 1];
	nTempSel = atoi(&m_pTrack->m_charTempPosNameForInsp);
	int nFovTemp = nTempSel - 1;

	CString strDevice = strImageName.Mid(strImageName.ReverseFind('T')); //JY
	nPos = strDevice.Find('_');
	strImageName = strDevice.Mid(0, nPos);
	m_pTrack->m_charTempPosNameForInsp = strDevice[nPos - 1];
	nTempSel = atoi(&m_pTrack->m_charTempPosNameForInsp);
	int nDeviceTemp = nTempSel - 1;
	((CMainFrame*)m_pApp->m_pMainWnd)->m_dlgPane->m_wndDlg.EnableDieRect(0, nDeviceTemp);

}


void CAppDoc::SaveInspSeqImages(int nTrack,int nDoc,int nDie,int nFov)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	CString strTemp;
	CString strFile, strImageDesc;
	CString m_strImageFolders;
	m_strImageFolders.Format("%s\\Sorted StoredImages", m_pApp->m_ImageSaveFolder);
	strImageDesc.Format("HP_FCI_THA%d_Pos%d_",nDie+1,nFov+1);
	//strImageDesc = m_pApp->GetFailedStr(pBuf->m_nErrorCode,FALSE);
	switch(nTrack){	
	case 0:	strTemp.Format("T%d",nDoc+1);
			strImageDesc.Append(strTemp);
			break;
	case 1: strTemp.Format("B%d",nDoc+1);
			strImageDesc.Append(strTemp);
			break;
	case 2:	strTemp.Format("S%d",nDoc+1);
			strImageDesc.Append(strTemp);
			break;
	}

	CString strFolder, strRecipeFolder, strImage;
	strRecipeFolder.Format("%s\\%s", m_strImageFolders,m_pApp->m_strConfigFile);
	_mkdir(strRecipeFolder);
	if(!m_pApp->m_strLotInfo.IsEmpty())
		strFolder.Format("%s\\%s", strRecipeFolder, m_pApp->m_strLotInfo);
	else
		strFolder.Format("%s\\DUMMY_LOT", strRecipeFolder);
	_mkdir(strFolder);
	strTemp.Format("%s\\Seq%d", strFolder, m_pApp->m_nSequenceCounter);
	_mkdir(strTemp);
	strFolder.Format("%s\\%s", strTemp, m_pApp->m_strPartFlex[nDie]);
	_mkdir(strFolder);
	CreateDirectoryTree(strFolder);
	strFile.Format("%s\\%s",strFolder,strImageDesc);
	SaveGrayScaleBMPFile(strFile + ".bmp", m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer);
	OutputDebugLog("%s saved!", strFile);

}



void CAppDoc::ZoomView(int nMode)
{
	POSITION pos = GetFirstViewPosition();
	CAppView* pView = (CAppView*)GetNextView(pos);
	if(nMode==ZOOMOUT)
		pView->ZoomOut();
	else if(nMode==ZOOMIN)
		pView->ZoomIn();
	else if(nMode==ZOOMFIT) {
		pView->ZoomFit();
		pView->ZoomFit();
	}
}


void CAppDoc::OnInspectionTeach()
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnInspectionTeach();
		return;
	}
	m_pTrack->m_nCommand = TEACH;
	m_pTrack->m_pActiveDoc = m_pApp->m_pTracks[m_pTrack->m_nTrack].m_pDoc[0];//this;
	SetEvent(m_pTrack->m_EventRun);
	//m_pApp->m_pTracks[0].m_nCommand = TEACH;
	//m_pApp->m_pTracks[0].m_pActiveDoc = m_pApp->m_pTracks[0].m_pDoc[0];//this;
	//SetEvent(m_pApp->m_pTracks[0].m_EventRun);
}


void CAppDoc::OnUpdateInspectionTeach(CCmdUI *pCmdUI)
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateInspectionTeach(pCmdUI);
		return;
	}
	
	pCmdUI->Enable(theApp.m_bIsMasterApp &&
		!m_pTrack->m_bLiveImage &&
					!m_bStoredImages &&
				   !m_pApp->m_bOnline &&
				   !m_pApp->m_bTeaching &&
				   !m_pApp->m_bInspecting &&
				   !m_pApp->m_bTrackInspSeq &&
				   !m_pTrack->m_bContInsp &&
				   !m_pTrack->m_bCamExtTrigEnable &&
				   !m_bInspSavedImage &&
				   (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nTeach));
}

void CAppDoc::OnInspectionTeachSingleImage()
{
	/*if(m_pRemoteInspDoc) {
	m_pRemoteInspDoc->OnInspectionTeach();
	return;
	}*/
	theApp.m_bTeachSingleImage = TRUE;
	m_pTrack->m_nCommand = TEACH;
	m_pTrack->m_pActiveDoc = m_pApp->m_pTracks[m_pTrack->m_nTrack].m_pDoc[0];//this;
	SetEvent(m_pTrack->m_EventRun);
	//m_pApp->m_pTracks[0].m_nCommand = TEACH;
	//m_pApp->m_pTracks[0].m_pActiveDoc = m_pApp->m_pTracks[0].m_pDoc[0];//this;
	//SetEvent(m_pApp->m_pTracks[0].m_EventRun);
}


void CAppDoc::OnUpdateInspectionTeachSingleImage(CCmdUI *pCmdUI)
{
	/*if(m_pRemoteInspDoc) {
	m_pRemoteInspDoc->OnUpdateInspectionTeach(pCmdUI);
	return;
	}*/

	pCmdUI->Enable(theApp.m_bIsMasterApp &&
		!m_pTrack->m_bLiveImage &&
		!m_bStoredImages &&
		!m_pApp->m_bOnline &&
		!m_pApp->m_bTeaching &&
		!m_pApp->m_bInspecting &&
		!m_pApp->m_bTrackInspSeq &&
		!m_pTrack->m_bContInsp &&
		!m_pTrack->m_bCamExtTrigEnable &&
		!m_bInspSavedImage &&
		(m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nTeach));
}


void CAppDoc::OnInspectionDebug()
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnInspectionDebug();
		return;
	}

	if(m_bStoredImages) {
		for (int nTrack = 0; nTrack < m_pApp->m_nTotalTracks; nTrack++){
			if (!m_pApp->m_bSetForAllTracks && m_pTrack->m_nTrack)
				continue;

			m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_lDebugFlag &= ~DEBUG_STEP_MODE;
		}
		
		InspectStoredImages(TRUE);
	}
	else {
	//	for (int nTrack = 0; nTrack < m_pApp->m_nTotalTracks; nTrack++) {
			/*m_pApp->m_pTracks[nTrack].m_pDoc[0]->*/m_bInspectionDebug = !/*m_pApp->m_pTracks[nTrack].m_pDoc[0]->*/m_bInspectionDebug;
	//	}
		
		if (m_bInspectionDebug) {
			m_lDebugFlag |= DEBUG_STEP_MODE;
		}
		else
		{
			m_lDebugFlag &= ~DEBUG_STEP_MODE;
		}
		/*m_lDebugFlag |= DEBUG_STEP_MODE;
		m_pTrack->m_nCommand = INSPECT;
		m_pTrack->m_pActiveDoc = this;
		SetEvent(m_pTrack->m_EventRun);*/
	}
}


void CAppDoc::OnUpdateInspectionDebug(CCmdUI *pCmdUI)
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateInspectionDebug(pCmdUI);
		return;
	}

	BOOL bChecked = (m_bInspSavedImage && m_bSavedImagesStep)|| m_bInspectionDebug;
	pCmdUI->Enable((theApp.m_bIsMasterApp &&
					!m_pTrack->m_bLiveImage &&
				   !m_pApp->m_bOnline &&
				   !m_pApp->m_bTeaching &&
				   !m_pApp->m_bInspecting &&
				   !m_pApp->m_bTrackInspSeq &&
				   !m_pTrack->m_bContInsp &&
				   !m_bInspSavedImage &&
				   !m_pTrack->m_bCamExtTrigEnable &&
				   (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nStepDebug))
				   || bChecked);// && m_pApp->m_bSuperUserMode);
	
	pCmdUI->SetCheck(bChecked);
}

void CAppDoc::OnInspectionInspect()
{
	if (m_ImageViewInfo.pImageView->m_bMagnifierView) 
		m_ImageViewInfo.pImageView->ToggleMagnifier();

	if(m_pRemoteInspDoc){
		m_pRemoteInspDoc->OnInspectionInspect();
		return;
	}
	//m_pApp->bIsLeftButtonClicked = FALSE;

	//m_lDebugFlag &= ~DEBUG_STEP_MODE;
	CTrackHandler* pTrack;
	UpdateStatusBar(&CString(""));
	
	if(GetKeyState(VK_SHIFT) & 0x8000) {
		for(int i=0; i<m_pApp->m_nTotalTracks; i++) {
			pTrack = &m_pApp->m_pTracks[i];
			if(pTrack->m_pDoc[m_nDocIdx]->m_bStoredImages)
				pTrack->m_pDoc[m_nDocIdx]->InspectStoredImages(FALSE);
			else if(pTrack->m_pDoc[m_nDocIdx]->m_bInspectCycle)
				pTrack->m_pDoc[m_nDocIdx]->ContinuousInspect();
			else {
				pTrack->m_nCommand = INSPECT;
				pTrack->m_pActiveDoc = pTrack->m_pDoc[m_nDocIdx];
				SetEvent(pTrack->m_EventRun);
			}
		}
	}
	else {
		if(m_bStoredImages)
			InspectStoredImages(FALSE);
		else if(m_bInspectCycle)
			ContinuousInspect();
		else {
			m_pTrack->m_nCommand = INSPECT;
			m_pTrack->m_pActiveDoc = m_pApp->m_pTracks[m_pTrack->m_nTrack].m_pDoc[0];//this;
			SetEvent(m_pTrack->m_EventRun);
		}
	}
}


void CAppDoc::OnUpdateInspectionInspect(CCmdUI *pCmdUI)
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateInspectionInspect(pCmdUI);
		return;
	}

	BOOL bChecked = m_pTrack->m_bContInsp || (m_bInspSavedImage && !m_bSavedImagesStep && !m_pApp->m_bInspectionAll);
	pCmdUI->Enable((theApp.m_bIsMasterApp && !m_pTrack->m_bLiveImage &&
				   !m_pApp->m_bOnline &&
				   !m_pApp->m_bTeaching &&
				   !m_pApp->m_bInspecting &&
				   !m_pApp->m_bTrackInspSeq &&
				   !m_pTrack->m_bContInsp &&
				   !m_bInspSavedImage &&
				   !m_pTrack->m_bCamExtTrigEnable &&
				   !m_pApp->m_bSetForAllTracks &&
				  // !m_pApp->m_bLotOpened&&
				   (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nInspect))
				   || bChecked);// && m_pApp->m_bSuperUserMode);
	
	pCmdUI->SetCheck(bChecked);
}


void CAppDoc::OnInspectionInspectAll()
{
	UpdateStatusBar(&CString(""));
	if (m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnInspectionInspectAll();
		return;
	}

	
	for (int i = 0; i < m_pApp->m_nTotalTracks; i++) {
		for (int j = 0; j < m_pApp->m_nTotalViewDocs[i]; j++) {

			if (m_bInspectionDebug) {
				m_pApp->m_pTracks[i].m_pDoc[j]->m_bInspectionDebug = TRUE;
				m_pApp->m_pTracks[i].m_pDoc[j]->m_lDebugFlag |= DEBUG_STEP_MODE;
			}
			else {
				m_pApp->m_pTracks[i].m_pDoc[j]->m_bInspectionDebug = FALSE;
				m_pApp->m_pTracks[i].m_pDoc[j]->m_lDebugFlag &= ~DEBUG_STEP_MODE;
			}
		}
	}
	m_pApp->m_bInspectionAll = !m_pApp->m_bInspectionAll;

	
	if (m_pApp->m_bSetForAllTracks)
		InspectStoredImages(FALSE);
	else {
		m_pTrack->m_nCommand = INSPECT_ALL;
		m_pTrack->m_pActiveDoc = m_pApp->m_pTracks[m_pTrack->m_nTrack].m_pDoc[0];//this;
		SetEvent(m_pTrack->m_EventRun);
	}
}


void CAppDoc::OnUpdateInspectionInspectAll(CCmdUI *pCmdUI)
{
	if (m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateInspectionInspectAll(pCmdUI);
		return;
	}
	BOOL bChecked = (m_bInspSavedImage && !m_bSavedImagesStep && !m_pApp->m_bInspectionAll);
	BOOL bInspectCycleAllTrack = FALSE;
	for (size_t i = 0; i < m_pApp->m_nTotalTracks; i++) {
		bInspectCycleAllTrack = bInspectCycleAllTrack || m_pApp->m_pTracks[i].m_pDoc[0]->m_bInspectCycle;
	}

	pCmdUI->Enable((theApp.m_bIsMasterApp && !m_pApp->m_bOnline &&
		!m_pApp->m_bTeaching &&
		!m_pApp->m_bInspecting &&
		!m_pApp->m_bTrackInspSeq &&
		!m_pTrack->m_bLiveImage &&
		/*(!m_pApp->m_pTracks[0].m_pDoc[0]->m_bInspectCycle && !m_pApp->m_pTracks[1].m_pDoc[0]->m_bInspectCycle && !m_pApp->m_pTracks[2].m_pDoc[0]->m_bInspectCycle)*/ 
		!bInspectCycleAllTrack &&
		(m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nInspect))&&!bChecked);

	pCmdUI->SetCheck(m_pApp->m_bInspectionAll);
}

void CAppDoc::OnInspectionNext()
{
	if(m_pTrack->m_pDoc[0]->m_pRemoteInspDoc) {
		m_pTrack->m_pDoc[0]->m_pRemoteInspDoc->Continue();
		return;
	}

	if (m_bSavedImagesStep && m_bStoredImages) {
		m_pApp->m_pTracks[m_pApp->m_nStepTrack].m_pDoc[m_pApp->m_nStepDoc]->Continue();
	}
	else
		Continue();
}

void CAppDoc::OnUpdateInspectionNext(CCmdUI *pCmdUI)
{
	if(m_pTrack->m_pDoc[0]->m_pRemoteInspDoc) {
		m_pTrack->m_pDoc[0]->m_pRemoteInspDoc->OnUpdateInspectionNext(pCmdUI);
		return;
	}

	pCmdUI->Enable(theApp.m_bIsMasterApp && CEvImageDoc::m_bWaiting && !m_pTrack->m_bAutoRun);
}



void CAppDoc::OnInspectionAbort()
{
	if(m_pTrack->m_pDoc[0]->m_pRemoteInspDoc) {
		m_pTrack->m_pDoc[0]->m_pRemoteInspDoc->OnInspectionAbort();
		return;
	}
	if (m_pApp->m_bSetForAllTracks && m_bSavedImagesStep) {
		for (int nTrack = 0; nTrack < m_pApp->m_nTotalTracks; nTrack++) {
			m_pApp->m_pTracks[nTrack].m_pDoc[0]->Abort();
		}
	}
	else
		Abort();
}


void CAppDoc::OnUpdateInspectionAbort(CCmdUI *pCmdUI)
{
	if(m_pTrack->m_pDoc[0]->m_pRemoteInspDoc) {
		m_pTrack->m_pDoc[0]->m_pRemoteInspDoc->OnUpdateInspectionAbort(pCmdUI);
		return;
	}

	pCmdUI->Enable(theApp.m_bIsMasterApp && CEvImageDoc::m_bWaiting && !m_pTrack->m_bAutoRun);
}


void CAppDoc::InspectStoredImages(BOOL bStep) 
{
	if (m_pApp->m_bSetForAllTracks) {
		for (int nTrack = 0; nTrack < m_pApp->m_nTotalTracks; nTrack++) {
			m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_bSavedImagesStep = bStep;
		}
	}
	else
		m_bSavedImagesStep = bStep;

	if(m_bInspSavedImage) {
		m_pApp->m_bInspecting = FALSE;
		Continue();
		SetEvent(m_pTrack->m_EventKillInspectImages);
	}
	else {
		if (m_pApp->m_bSetForAllTracks) {
			for (int nTrack = 0; nTrack < m_pApp->m_nTotalTracks; nTrack++) {
				m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_bInspSavedImage = TRUE;
			}
		}
		else
			m_bInspSavedImage = TRUE;
		m_pApp->m_ThreadManager.CreateThread(TrackInspectImages, this, INSPECTION_THREAD, -1);
	}
}

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	TCHAR szDir[MAX_PATH];
	switch(uMsg){
	case BFFM_INITIALIZED:
		if (lpData){
			strcpy_s(szDir, (char*) lpData);
			SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)szDir);
		}
		break;
	case BFFM_SELCHANGED: {
	   if (SHGetPathFromIDList((LPITEMIDLIST) lParam ,szDir)){
		  SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);
	   }
	   break;
	}
	default:
	   break;
	}
         
	return 0;
}

BOOL GetFolder(CString* strSelectedFolder, const char* strStartFolder, char* strTitle)
{
	//char pszDisplayName[MAX_PATH];
	//LPITEMIDLIST lpID;
	//BROWSEINFOA bi;
	//CWnd hwnd;
	//
	//bi.hwndOwner = hwnd.GetSafeHwnd();
	//bi.pidlRoot = NULL;
	//bi.pszDisplayName = pszDisplayName;
	//bi.lpszTitle = _T(strTitle);
	//bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
	//bi.lpfn = BrowseCallbackProc;

	//bi.lParam = (LPARAM) strStartFolder;
	//bi.iImage = NULL;
	//lpID = SHBrowseForFolderA(&bi);
	//if (lpID != NULL){
	//	BOOL b = SHGetPathFromIDList(lpID, pszDisplayName);
	//	if (b == TRUE){
	//		if(strlen(pszDisplayName)>3)
	//			strcat_s(pszDisplayName, "\\");
	//		strSelectedFolder->Format("%s",pszDisplayName);
	//		return TRUE;
	//	}
	//}else{
	//	strSelectedFolder->Empty();
	//}

	CSelectFolder dlg;
	dlg.SetPath(strStartFolder);
	if (dlg.DoModal() == IDOK)
	{
		strSelectedFolder->Format("%s", dlg.GetPath());
		return TRUE;
	}
	return FALSE;
}

void CAppDoc::OnCameraSnap()
{
	//m_pApp->m_bSynchronousGrabEnable ? m_pApp->SynchronousCameraSnap() : CameraGrab();
//	CameraGrabUsingExposure();
	
	m_pTrack->RefreshLight(m_nDocIdx);
	CameraGrabSingle();
}


void CAppDoc::OnUpdateCameraSnap(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	for (int i = 0; i < m_pApp->m_nTotalTracks; i++)
	{
		for (int j = 0; j < m_pApp->m_nTotalViewDocs[i]; j++) {
			if (m_pApp->m_pTracks[i].m_pDoc[j]->m_bLiveImageSingle)
				bEnable = TRUE;
		}
	}
	pCmdUI->Enable(m_pTrack->m_bCamEnable && 
					m_pTrack->m_bCamAvail &&
				   !bEnable &&
				   !m_pTrack->m_bLiveImage &&
				   !m_pApp->m_bOnline &&
				   !m_pApp->m_bTeaching &&
				   !m_pApp->m_bInspecting &&
				   !m_pTrack->m_bContInsp &&
				   !m_pApp->m_bTrackInspSeq &&
				   (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nGrab));
}


void CAppDoc::OnCameraStream()
{
	if(!m_pTrack->m_bCamAvail)
		return;

	m_pTrack->RefreshLight(m_nDocIdx);
	CamLiveSingle();
}


void CAppDoc::OnUpdateCameraStream(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	for (int i = 0; i < m_pApp->m_nTotalTracks; i++)
	{
		for (int j = 0; j < m_pApp->m_nTotalViewDocs[i]; j++) {
			if (m_pApp->m_pTracks[i].m_pDoc[j]->m_bLiveImageSingle && j!=m_nDocIdx)
				bEnable = TRUE;
		}
	}
	pCmdUI->Enable(m_pTrack->m_bCamEnable && 
					m_pTrack->m_bCamAvail &&
				   !m_pTrack->m_bLiveImage &&
				   !m_pApp->m_bOnline &&
				   !bEnable &&
				   !m_pApp->m_bTeaching &&
				   !m_pApp->m_bInspecting &&
				   !m_pTrack->m_bContInsp &&
				   !m_pApp->m_bTrackInspSeq &&
				   !m_pApp->m_bAllowStream &&
				   (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nLive));
	
	//pCmdUI->SetCheck(m_pTrack->m_bLiveImage);
	pCmdUI->SetCheck(m_bLiveImageSingle);
}


void CAppDoc::OnCameraLight()
{
	if(m_pApp->m_pLightCtrlDlg)
		delete m_pApp->m_pLightCtrlDlg;
	
	m_pApp->m_pLightCtrlDlg = new CLightControlDlg();
	m_pApp->m_pLightCtrlDlg->m_nTrackId = m_pTrack->m_nTrack;
	m_pApp->m_pLightCtrlDlg->m_nDocId = m_nDocIdx;
	m_pApp->m_pLightCtrlDlg->m_nFOV = m_pApp->m_pTracks[m_pTrack->m_nTrack].m_nFOVIndex;

	m_pApp->m_bLightCtrlDlgOpen = TRUE;
	if(!m_pApp->m_pLightCtrlDlg->Create(IDD_LIGHT_CONTROL_DLG, AfxGetMainWnd())) {
		delete m_pApp->m_pLightCtrlDlg;
		m_pApp->m_pLightCtrlDlg = NULL;
		m_pApp->m_bLightCtrlDlgOpen = FALSE;
		return;
	}

	CRect rect1, rect2;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rect1, 0);
	m_pApp->m_pLightCtrlDlg->GetWindowRect(&rect2);

	m_pApp->m_pLightCtrlDlg->SetWindowPos(NULL, rect1.Width() - rect2.Width(), rect1.Height()-rect2.Height()-20, 0, 0, SWP_NOSIZE);
	
	m_pApp->m_pLightCtrlDlg->ShowWindow(TRUE);
	m_pApp->StationMapDlgControls();
}


void CAppDoc::OnUpdateCameraLight(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	for (int i = 0; i < m_pApp->m_nTotalTracks; i++)
	{
		for (int j = 0; j < m_pApp->m_nTotalViewDocs[i]; j++) {
			if (m_pApp->m_pTracks[i].m_pDoc[j]->m_bLiveImageSingle)
				bEnable = TRUE;
		}
	}
	pCmdUI->Enable(theApp.m_bIsMasterApp && !m_pApp->m_bLightCtrlDlgOpen && !m_pApp->m_bStationMapDlgOpen && /*!m_pApp->m_bMotionCtrlDlgOpen &&*/
					!bEnable &&
					!m_pApp->m_bTeaching &&
				  (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nLightSet));// && m_pApp->m_bSuperUserMode);
}


void CAppDoc::OnParmVision()
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnParmVision();
		return;
	}

	((CMainFrame*) m_pApp->m_pMainWnd)->ShowProperties(this, m_pTrack->m_nFOVIndex);
}


void CAppDoc::OnUpdateParmVision(CCmdUI *pCmdUI)
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateParmVision(pCmdUI);
		return;
	}
	BOOL bChecked = ((CMainFrame*) m_pApp->m_pMainWnd)->bIsPropertiesVisiblePane;
	/*pCmdUI->SetCheck(bChecked);*/
	pCmdUI->Enable(theApp.m_bIsMasterApp && (m_pApp->m_bSuperUserMode ||
				   !m_pTrack->m_bContInsp &&
				   !m_pApp->m_bOnline &&
				   !m_pApp->m_bTeaching &&
				   !m_pApp->m_bInspecting &&
		           !m_bSavedImagesStep &&
				   !m_bInspSavedImage &&
				   !m_bInspectionDebug &&
				  // !bChecked &&
				   (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nVisionParm)));
}


void CAppDoc::OnParmTeach()
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnParmTeach();
		return;
	}

	((CMainFrame*) m_pApp->m_pMainWnd)->ShowTeachProperties(this, m_pTrack->m_nFOVIndex);
}


void CAppDoc::OnUpdateParmTeach(CCmdUI *pCmdUI)
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateParmTeach(pCmdUI);
		return;
	}
	BOOL bChecked = ((CMainFrame*) m_pApp->m_pMainWnd)->bIsPropertiesVisiblePaneTeach;
	/*pCmdUI->SetCheck(bChecked);*/
	pCmdUI->Enable(theApp.m_bIsMasterApp && ( m_pApp->m_bSuperUserMode ||
				    !m_pTrack->m_bContInsp &&
				    !m_pApp->m_bOnline &&
				   //!m_pApp->m_bTeaching &&
				    !m_pApp->m_bInspecting &&
					!m_bSavedImagesStep &&
					!m_bInspSavedImage &&
					!m_bInspectionDebug &&
				  // !bChecked &&
				   (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nTeachParm)));
}


//void CAppDoc::OnParmDefect()
//{
//	if(m_pRemoteInspDoc) {
//		m_pRemoteInspDoc->OnParmDefect();
//		return;
//	}
//	//((CMainFrame*)m_pApp->m_pMainWnd)->InitializeDefectProperties(this);
//	((CMainFrame*) m_pApp->m_pMainWnd)->ShowDefectProperties();
//}


//void CAppDoc::OnUpdateParmDefect(CCmdUI *pCmdUI)
//{
//	if(m_pRemoteInspDoc) {
//		m_pRemoteInspDoc->OnUpdateParmDefect(pCmdUI);
//		return;
//	}
//	BOOL bChecked = ((CMainFrame*) m_pApp->m_pMainWnd)->bIsPropertiesVisiblePaneDefect;
//	/*pCmdUI->SetCheck(bChecked);*/
//	pCmdUI->Enable(m_pApp->m_bSuperUserMode ||
//				    !m_pTrack->m_bContInsp &&
//				    !m_pApp->m_bOnline &&
//				    !m_pApp->m_bTeaching &&
//				    !m_pApp->m_bInspecting &&
//					!m_bSavedImagesStep &&
//					!m_bInspSavedImage &&
//					!m_bInspectionDebug &&
//				  // !bChecked &&
//				   (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nDefectParm));
//}


void CAppDoc::SetImageTitle(CString* str, BOOL bRefresh)
{
	m_strLoadImage = str ? *str : "";
	if(bRefresh)
		SetDocStatusText(&m_ImageViewInfo, NULL);
}

void CAppDoc::OnToolsStaticimageLoadFromFile()
{
	CString strImagePath;
	if(GetKeyState(VK_SHIFT) & 0x8000) {
		//m_pApp->m_bLoadMultiImagesFromFile = !m_pApp->m_bLoadMultiImagesFromFile;
		//if(m_pApp->m_bLoadMultiImagesFromFile && (AfxMessageBox("Confirm Load Multiple Images Path", MB_YESNO)==IDYES)) {
		//	CFolderDialog dlg("Stored Multiple Images", m_pApp->m_strLoadMultiImagesFolderPath);
		//	if(dlg.DoModal() == IDOK) {
		//		CString strTemp;
		//		strTemp = dlg.GetPathName();
		//		if(!strTemp.IsEmpty()) {
		//			m_pApp->m_strLoadMultiImagesFolderPath = strTemp;
		//			PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Folder: StoredMultiImages", m_pApp->m_strLoadMultiImagesFolderPath);
		//		}
		//	}
		//}
	
		//if(!m_pApp->m_bLoadMultiImagesFromFile){
		//	int nCurPos, nDevice, nPos, nSel;
		//	nCurPos = ((CMainFrame*) m_pApp->m_pMainWnd)->m_dlgPane->m_wndDlg.nPosNew;
		//	if(nCurPos < 0)
		//		return;
		//	nDevice = nCurPos/MAX_FOV;
		//	nSel = ((nDevice*MAX_FOV) + (nCurPos%m_pApp->m_nNoOfFov));
		//	((CMainFrame*) m_pApp->m_pMainWnd)->m_dlgPane->m_wndDlg.m_ctrlDieImages[nSel].m_bSelected = FALSE;
		//	((CMainFrame*) m_pApp->m_pMainWnd)->m_dlgPane->m_wndDlg.m_ctrlDieImages[nSel].Invalidate();

		//	((CMainFrame*) m_pApp->m_pMainWnd)->m_dlgPane->m_wndDlg.nPosNew = -1;
		//	((CMainFrame*) m_pApp->m_pMainWnd)->m_dlgPane->m_wndDlg.nPosOld = -1;
		//}
	}
	else {
		CSelectFolder dlg;
		dlg.SetPath(m_pApp->m_strLoadMultiImagesFolderPath);
		dlg.m_bFindImage = TRUE;
		if (dlg.DoModal() == IDOK)
		{
			CString strTemp = dlg.GetPath();
			int nLength = strTemp.GetLength();
			strImagePath = strTemp.Left(nLength - 1);
			m_pApp->m_strLoadMultiImagesFolderPath = strImagePath.Left(strImagePath.ReverseFind('\\') + 1);
		}
		else
			strImagePath.Empty();
		//strImagePath = LoadImage();
		if (!strImagePath.IsEmpty()) {
			LoadStaticImage(strImagePath);
			////m_pApp->LoadDocImages(strImagePath);
			//SaveStaticImagePath(strImagePath);
			//LoadImagesToDoc(strImagePath);
			//CString* strTemp = new CString;
			//strTemp->Format("%d%d", m_nDocIdx, m_pTrack->m_nFOVIndex);
			//m_pApp->m_pMainWnd->SendMessageA(WM_SET_CUR_POS_PARAM_DISP, (WPARAM)m_pTrack->m_nTrack, (LPARAM)strTemp);
			//delete strTemp;

			if(m_bStoredImages)
				m_bStoredImages = !m_bStoredImages;
		}

	}
}

void CAppDoc::OnUpdateToolsStaticimageLoadFromFile(CCmdUI *pCmdUI)
{
	if (m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateToolsStaticimageLoadFromFile(pCmdUI);
		return;
	}
	pCmdUI->Enable(!m_pTrack->m_bLiveImage &&
				   !m_pApp->m_bOnline &&
				   !m_pApp->m_bTeaching &&
				   !m_bInspecting &&
				   !m_pApp->m_bTrackInspSeq &&
					!m_bSavedImagesStep &&
					!m_bInspSavedImage &&
					!m_bInspectionDebug &&
		            !m_bStoredImages &&
				   (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nLoadImg));
}

void CAppDoc::OnToolsStaticimageLoadAllFromFile() 
{
	m_pApp->m_bFwdDirectionForTiltInsp = TRUE;
	m_pApp->m_bLoadMultiImagesFromFile = TRUE;
	m_pApp->m_strLoadMultiImagesArray.RemoveAll();
	if (m_pApp->m_bLoadMultiImagesFromFile) {
		OutputDebugLogTo(9, TRUE, "LoadImageAll 1");
		CSelectFolder dlg;
		dlg.SetPath(m_pApp->m_strLoadMultiImagesFolderPath);
	//	CFolderDialog dlg(" ", m_pApp->m_strLoadMultiImagesFolderPath);
		OutputDebugLogTo(9, TRUE, "LoadImageAll 2");
		if (dlg.DoModal() == IDOK) {
			CString strTemp;
			strTemp = dlg.GetPath();
			if (!strTemp.IsEmpty()) {
				m_pApp->m_strLoadMultiImagesFolderPath = strTemp;
				PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Folder: StoredMultiImages", m_pApp->m_strLoadMultiImagesFolderPath);
			}
		}
		else
			return;
	}

	CString strImageName,strPos;
	GetImageFilePaths(m_pApp->m_strLoadMultiImagesFolderPath + "\\", m_pApp->m_strLoadMultiImagesArray);
	strImageName = m_pApp->m_strLoadMultiImagesArray.GetAt(0);
	int nTempLen, nTempSel;
	int nPos = 0;
	strPos = strImageName.Mid(strImageName.ReverseFind('P'), 4);
	nTempLen = strPos.GetLength();
	m_pTrack->m_charTempPosNameForInsp = strPos[nTempLen - 1];
	nTempSel = atoi(&m_pTrack->m_charTempPosNameForInsp);
	int nFovTemp = nTempSel - 1;

	
	CString strDevice = strImageName.Mid(strImageName.ReverseFind('T')); //JY
	nPos = strDevice.Find('_');
	strImageName = strDevice.Mid(0, nPos);
	m_pTrack->m_charTempPosNameForInsp = strDevice[nPos - 1];
	nTempSel = atoi(&m_pTrack->m_charTempPosNameForInsp);
	int nDeviceTemp = nTempSel - 1;
	((CMainFrame*)m_pApp->m_pMainWnd)->m_dlgPane->m_wndDlg.EnableDieRect(0, nDeviceTemp);
}


void CAppDoc::OnUpdateToolsStaticimageLoadAllFromFile(CCmdUI *pCmdUI)
{
	if (m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateToolsStaticimageLoadAllFromFile(pCmdUI);
		return;
	}

	BOOL bLiveImageAllTrack = FALSE;
	for (size_t i = 0; i < m_pApp->m_nTotalTracks; i++) {
		bLiveImageAllTrack = bLiveImageAllTrack || m_pApp->m_pTracks[i].m_bLiveImage;
	}

	pCmdUI->Enable(/*!m_pApp->m_pTracks[0].m_bLiveImage && !m_pApp->m_pTracks[1].m_bLiveImage && !m_pApp->m_pTracks[2].m_bLiveImage*/
					!bLiveImageAllTrack &&
					!m_pApp->m_bOnline &&
					!m_pApp->m_bInspSequenceOffline &&
					!m_pApp->m_bTeaching &&
					!m_bInspecting &&
					!m_pApp->m_bTrackInspSeq &&
					!m_bSavedImagesStep &&
					!m_bInspSavedImage &&
					!m_bInspectionDebug &&
					!m_bStoredImages &&
					(m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nLoadImgAll));
}

void CAppDoc::OnToolsStaticimageLoadTeachImage()
{
	int nError = -1;
	/*if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnToolsStaticimageLoadTeachImage();
		return;
	}*/

	//CString strTeachImage;
	//strTeachImage.Format("%s\\%s\\Cam1\\TeachTemplate.bmp",m_pApp->m_strMainCfgDir,m_pApp->m_strConfigFile);

	//nError = LoadStaticImage(/*strTeachImage*/);
	int nTrack, nDocCount;
	nTrack = m_pTrack->m_nTrack;
	nDocCount = m_pApp->m_nTotalViewDocs[nTrack];
	CString strInfoMsg;
	for(int i=0; i<nDocCount; i++) {
		nError = m_pApp->m_pTracks[nTrack].m_pDoc[i]->LoadStaticImage();
		if(nError) {
			strInfoMsg.Format("Teach Image Not Available! at %s - %s!!\nDo You Like To Load Static Image?", !nTrack ? "Top Camera" : (nTrack == 1 ? "Bottom Camera" : "Side Camera"),
				!nTrack ? m_pApp->m_strTopCameraName.GetAt(i) : (nTrack == 1 ? m_pApp->m_strBottomCameraName.GetAt(i) : m_pApp->m_strSideCameraName.GetAt(i)));
			m_pApp->AddLogEvent(strInfoMsg);
			/*if(AfxMessageBox(strInfoMsg, MB_YESNO) == IDYES)
				m_pApp->m_pTracks[nTrack].m_pDoc[0]->OnToolsStaticimageLoadFromFile();*/

			LOGPEN TextColorRed, TextColorYellow;
			TextColorRed.lopnColor = RGB(230, 50, 50);
			TextColorRed.lopnStyle = PS_SOLID;
			TextColorRed.lopnWidth.x = 1;
			TextColorYellow.lopnColor = RGB(230, 230, 0);
			TextColorYellow.lopnStyle = PS_SOLID;
			TextColorYellow.lopnWidth.x = 1;

			strInfoMsg = "No Image";
			DrwStr(m_pApp->m_pTracks[nTrack].m_SharedData.pImageViews[i], &strInfoMsg, &CPoint(240, 20), &TextColorRed, 80);

			CRect rect = CRect(CPoint(80, 80), CSize(60, 60));
			BOOL bFill = m_nInspCounter % 2;
			if (bFill)
				rect.DeflateRect(6, 6);
			else
				TextColorYellow.lopnWidth.x = 10;
			DrwRect(&m_pApp->m_pTracks[nTrack].m_pDoc[i]->m_ImageViewInfo, &rect, &TextColorYellow);
			DrawView(TRUE);

			m_nInspCounter++;
			if (m_nInspCounter == 100)
				m_nInspCounter = 0;
		}
	}
}

int CAppDoc::LoadStaticImage(CString strImagePath, BOOL bDisplay, BOOL bSameBitCount)
{
	m_pTrack->m_nOpticsRecalcMode = 2;
	if(strImagePath.IsEmpty())
		//strImagePath.Format("%s\\TeachTemplate.bmp", m_strConfigDir);
		strImagePath.Format("%s\\TeachTemplate_Pos%d.bmp", m_strConfigDir, m_pTrack->m_nFOVIndex+1);

	std::vector<CRect> rectDefects;
	std::vector<CPoint> ptDefectCenters;

	CPerformanceTimer timerReset;
	timerReset.Reset();
	try {
		if(m_bIsColor) {
			EnterCriticalSection(&m_csDisplayBuffer);
			CImageBuffer* pBuf = m_bBinaryMode ? &m_BufSource : &m_Buffer;
			int nError = LoadGrayScaleBMPFile(strImagePath, *pBuf, bSameBitCount);
			if(nError) {
				LeaveCriticalSection(&m_csDisplayBuffer);
				return nError;
			}

			if(m_bBinaryMode) {
				CRect rcFOV = CRect(CPoint(0,0), m_Buffer.GetImgSize());
				ImgCopy(&m_BufSource, &rcFOV, &m_Buffer, &rcFOV);
			}
			LeaveCriticalSection(&m_csDisplayBuffer);
		}
		else {
			//int nError = LoadGrayScaleBMPFile(strImagePath, m_Buffer, bSameBitCount);
			int nError = LoadGrayScaleBMPEmbeddedFile(strImagePath, m_Buffer, rectDefects, ptDefectCenters, bSameBitCount);
			if(nError) {
				if(bDisplay /*&& m_pTrack->m_nTrack == m_pApp->m_nTotalTracks-1*/) {
					m_Buffer.ClrImg();
					DrwDel(&m_ImageViewInfo);
					DrwUpdate(&m_ImageViewInfo);
				}
				return nError;
			}
		}
		SetImageTitle(&strImagePath.Mid(strImagePath.ReverseFind('\\') + 1), bDisplay);
		
		if(bDisplay) {
			DrwDel(&m_ImageViewInfo);
			if (ptDefectCenters.size() > 0) {
				LOGPEN TextColorRed, TextColorYellow;
				TextColorRed.lopnColor = RGB(230, 50, 50);
				TextColorRed.lopnStyle = PS_SOLID;
				TextColorRed.lopnWidth.x = 4;
				TextColorYellow.lopnColor = RGB(230, 230, 0);
				TextColorYellow.lopnStyle = PS_SOLID;
				TextColorYellow.lopnWidth.x = 1;
				for (int i = 0; i < ptDefectCenters.size(); i++) {
					DrwRect(&m_ImageViewInfo, &rectDefects[i], &TextColorRed);
					DrwCur(&m_ImageViewInfo, &ptDefectCenters[i], &TextColorYellow, 1);
				}
			}
			DrwUpdate(&m_ImageViewInfo);
		}
	}
	catch(...) {
		return -1;
	}

	
	if(!m_pTrack->m_bCamEnable)
///		m_pApp->m_pImageSelectionDlg->CopyImagesToStats(m_nDocIdx);

	OutputDebugLogTo(2, TRUE, "[Doc%d] Total Load Time: %5.1fms", m_nDocIdx+1, timerReset.GetElapsedMSec());

	return 0;
}

void CAppDoc::OnUpdateToolsStaticimageLoadTeachImage(CCmdUI *pCmdUI)
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateToolsStaticimageLoadTeachImage(pCmdUI);
		return;
	}
	//pCmdUI->Enable(FALSE);
	pCmdUI->Enable(!m_pTrack->m_bLiveImage &&
				   !m_pApp->m_bOnline &&
				   !m_pApp->m_bTeaching &&
				   !m_bInspecting &&
				   !m_pApp->m_bTrackInspSeq &&
					!m_bSavedImagesStep &&
					!m_bInspSavedImage &&
					!m_bInspectionDebug &&
					!m_bStoredImages &&
				   (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nLoadTeachImgAll));
}

void CAppDoc::OnToolsStaticimageLoadTeachImageSingle()
{
	int nError = -1;
	nError = m_pApp->m_pTracks[m_pTrack->m_nTrack].m_pDoc[m_nDocIdx]->LoadStaticImage();
	if(nError) {
		AfxMessageBox("Teach Image Not Available!!!");
		return;
	}
}

void CAppDoc::OnUpdateToolsStaticimageLoadTeachImageSingle(CCmdUI *pCmdUI)
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateToolsStaticimageLoadTeachImageSingle(pCmdUI);
		return;
	}
	
	pCmdUI->Enable(!m_pTrack->m_bLiveImage &&
				   !m_pApp->m_bOnline &&
				   !m_pApp->m_bTeaching &&
				   !m_bInspecting &&
				   !m_pApp->m_bTrackInspSeq &&
					!m_bSavedImagesStep &&
					!m_bInspSavedImage &&
					!m_bInspectionDebug &&
					!m_bStoredImages &&
				   (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nLoadTeachImg));
}

void CAppDoc::OnToolsStaticimageSave()
{

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;


	BOOL bSaveOne = FALSE;
	CString strFile, strTemp, strFolder, strFileName;
	
	CString strPath;
	CSelectFolder dlg;
	dlg.m_bSave = TRUE;
	dlg.SetPath(m_pApp->m_strLoadMultiImagesFolderPath);
	if (dlg.DoModal() == IDOK)
	{
		CString strTemp = dlg.GetPath();
		strPath.Format("%s%s", strTemp, dlg.m_strFileName);
		//CString strTemp = dlg.GetPath();
		//int nLength = strTemp.GetLength();
		//strPath = strTemp.Left(nLength - 1);
	}
	else
		return;

	if(GetKeyState(VK_SHIFT) & 0x8000) {
		//CString strPath = SaveImage(bSaveOne);
		for (int nFOV = 0; nFOV < m_nNoOfFov; nFOV++) {
			for (int nTrack = 0; nTrack < m_pApp->m_nTotalTracks; nTrack++) {
				for (int nDoc = 0; nDoc < m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
					CString strName;
					int nPos = strPath.ReverseFind('\\');
					strFolder = strPath.Left(nPos);
					strFileName = strPath.Mid(nPos);
					strTemp.Format("%s\\%s", strFolder, m_pApp->m_strPartFlex[m_pApp->m_nSaveAllDieIndex]);
					mkdir(strTemp);
					switch (nTrack) {
					case 0:
						strName.Format("%s_THA%d_Pos%d_T%d", strFileName, m_pApp->m_nSaveAllDieIndex != -1 ? m_pApp->m_nSaveAllDieIndex + 1 : 1, nFOV + 1, nDoc + 1);
						break;
					case 1:
						strName.Format("%s_THA%d_Pos%d_B%d", strFileName, m_pApp->m_nSaveAllDieIndex != -1 ? m_pApp->m_nSaveAllDieIndex + 1 : 1, nFOV + 1, nDoc + 1);
						break;
					case 2:
						strName.Format("%s_THA%d_Pos%d_S%d", strFileName, m_pApp->m_nSaveAllDieIndex != -1 ? m_pApp->m_nSaveAllDieIndex + 1 : 1, nFOV + 1, nDoc + 1);
						break;
					}
					strFile.Format("%s%s.bmp", strTemp, strName);
					SaveGrayScaleBMPFile(strFile, m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[m_pTrack->m_nDieIndex][nFOV]);
				}
			}
		}
	}
	else{
		//CString strPath = SaveImage(bSaveOne);
		if (!strPath.IsEmpty()) {
			CString strName;
			switch (m_pTrack->m_nTrack)
			{
			case 0:
				strName.Format("Pos%d_T%d", m_pApp->m_pTracks[m_pTrack->m_nTrack].m_nFOVIndex + 1, /*nDoc+1*/m_nDocIdx + 1);
				break;
			case 1:
				strName.Format("Pos%d_B%d", m_pApp->m_pTracks[m_pTrack->m_nTrack].m_nFOVIndex + 1, /*nDoc+1*/m_nDocIdx + 1);
				break;
			case 2:
				strName.Format("Pos%d_S%d", m_pApp->m_pTracks[m_pTrack->m_nTrack].m_nFOVIndex + 1, /*nDoc+1*/m_nDocIdx + 1);
				break;
			}
			strFile.Format("%s_%s.bmp", strPath, strName);
			//SaveGrayScaleBMPFile(strFile, m_pApp->m_pTracks[m_pTrack->m_nTrack].m_pDoc[nDoc]->m_Buffer);
			SaveGrayScaleBMPFile(strFile, m_pApp->m_pTracks[m_pTrack->m_nTrack].m_pDoc[m_nDocIdx]->m_Buffer);
		}
		//for(int nDoc=0; nDoc<m_pApp->m_nTotalViewDocs[m_pTrack->m_nTrack]; nDoc++) {
			
		//}
	}

	//SaveStaticImages();
	//

	//while(TRUE) {
	//	CString strPath = SaveImage(bSaveOne);
	//	if(!bSaveOne && !strPath.IsEmpty()) {
	//		CString strFile;
	//		strFile.Format("%s_Color.bmp", strPath);
	//		if(PathFileExists(strFile)) {
	//			CString str;
	//			int strPos = strFile.ReverseFind('\\');
	//			if(strPos >= 0) {
	//				str.Format("%s already exists                                                 \n\nDo you wish to overwrite?", strFile.Mid(strPos+1));
	//				if(MessageBox(NULL, str, "Save All Images", MB_ICONEXCLAMATION | MB_YESNO)==IDNO)
	//					continue;
	//			}
	//		}
	//		for(int nDoc=0; nDoc<m_pApp->m_nTotalViews; nDoc++) {
	//			CString strName;
	//			switch(nDoc)
	//			{
	//			case 0:
	//				strName = "BF";
	//				break;
	//			case 1:
	//				strName = "DF";
	//				break;
	//			case 2:
	//				strName = "SS";
	//				break;
	//			case 3:
	//				strName = "SSB";////Single Spot Bright Image
	//				break;
	//			}
	//			strFile.Format("%s_%s.bmp", strPath, strName);
	//			SaveGrayScaleBMPFile(strFile, m_pApp->m_pTracks[0].m_pDoc[nDoc]->m_Buffer);
	//		}
	//	}
	//	break;
	//}

}


void CAppDoc::OnUpdateToolsStaticimageSave(CCmdUI *pCmdUI)
{
	if (m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateToolsStaticimageSave(pCmdUI);
		return;
	}
	pCmdUI->Enable(!m_pTrack->m_bLiveImage &&
				   !m_pApp->m_bOnline &&
				   !m_pApp->m_bTeaching &&
				   !m_bInspecting &&
				   !m_pApp->m_bTrackInspSeq &&
					!m_bInspSavedImage &&
					!m_bInspectionDebug &&
					!m_bStoredImages &&
				   (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nSaveImg));
}

void CAppDoc::OnToolsStaticimageSaveAll()
{
	BOOL bSaveOne = FALSE;
	CString strFile;

	CString strPath;
	CSelectFolder dlg;
	dlg.m_bSave = TRUE;
	dlg.SetPath(m_pApp->m_strLoadMultiImagesFolderPath);

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfDies = nDevicePerX * nDevicePerY;
	int m_nNoOfFov = nFovPerX * nFovPerY;

	if (dlg.DoModal() == IDOK)
	{
		CString strTemp = dlg.GetPath();
		strPath.Format("%s%s", strTemp, dlg.m_strFileName);
		//CString strTemp = dlg.GetPath();
		//int nLength = strTemp.GetLength();
		//strPath = strTemp.Left(nLength - 1);
	}
	else
		strPath.Empty();
	//CString strPath = SaveImage(bSaveOne);
	
	if (!strPath.IsEmpty()) {
		CString strTemp;
		for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
			for (int nFOV = 0; nFOV < m_nNoOfFov; nFOV++) {
				for (int nTrack = 0; nTrack < m_pApp->m_nTotalTracks; nTrack++) {
					for (int nDoc = 0; nDoc < m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
						CString strName;
						int nPos = strPath.ReverseFind('\\');
						CString strFolder = strPath.Left(nPos);
						CString strFileName = strPath.Mid(nPos);
						strTemp.Format("%s\\%s", strFolder, m_pApp->m_strPartFlex[nDie]);
						mkdir(strTemp);
						switch (nTrack) {
						case 0:
							strName.Format("%s_THA%d_Pos%d_T%d", strFileName, nDie + 1, nFOV + 1, nDoc + 1);
							break;
						case 1:
							strName.Format("%s_THA%d_Pos%d_B%d", strFileName, nDie + 1, nFOV + 1, nDoc + 1);
							break;
						case 2:
							strName.Format("%s_THA%d_Pos%d_S%d", strFileName, nDie + 1, nFOV + 1, nDoc + 1);
							break;
						}
						strFile.Format("%s%s.bmp", strTemp, strName);
						SaveGrayScaleBMPFile(strFile, m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDie][nFOV]);
					}
				}
			}
		}
	}
}


void CAppDoc::OnUpdateToolsStaticimageSaveAll(CCmdUI *pCmdUI)
{
	if (m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateToolsStaticimageSaveAll(pCmdUI);
		return;
	}
	pCmdUI->Enable(!m_pTrack->m_bLiveImage &&
		!m_pApp->m_bOnline &&
		!m_pApp->m_bTeaching &&
		!m_bInspecting &&
		!m_pApp->m_bTrackInspSeq &&
		!m_bInspSavedImage &&
		!m_bInspectionDebug &&
		!m_bStoredImages &&
		(m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nSaveImgAll));
}

void CAppDoc::SaveStaticImages(BOOL bTeachImg)
{
	CString strFile, strImageFinalPath;
	CString strType[] = {"T", "B", "S"};
	UpdateStatusBar(&CString(""));

	if(bTeachImg) {
		strFile.Format("%s\\TeachTemplate.bmp", m_strConfigDir);
		SaveGrayScaleBMPFile(strFile, m_Buffer);
	}
	else {
		strFile = SaveImage(FALSE);
		int nPos = strFile.GetLength();	
		if(nPos > 0) {
			strImageFinalPath.Format("%s_%s%d.bmp", strFile, strType[m_pTrack->m_nTrack], m_nDocIdx+1);
			SaveGrayScaleBMPFile(strImageFinalPath, m_Buffer);
		}	
	}
}

void CAppDoc::OnToolsStaticimage()
{
	OnToolsStaticimageLoadFromFile();
}


void CAppDoc::OnUpdateToolsStaticimage(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_pTrack->m_bLiveImage &&
				   !m_pApp->m_bOnline &&
				   !m_pApp->m_bTeaching &&
				   !m_bInspecting &&
				   !m_pApp->m_bTrackInspSeq &&
				   (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nLoadImg));
}


void CAppDoc::OnInspectionStoredimages()
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnInspectionStoredimages();
		return;
	}
	CString strFolderPath;
	CTrackHandler* pTrack;
	if (!m_bStoredImages) {
		if (GetFolder(&strFolderPath, m_pTrack->m_strStoredImagesPath, "Stored Images Folder")) {
			if (!strFolderPath.IsEmpty()) {
				CString strTemp;
				if (m_pInspectionSetting.DoModal() == IDOK) {
					m_pApp->m_bSetForAllTracks = m_pInspectionSetting.m_bCheckForAllTracks;
					m_pApp->m_bSetForSingleFov = m_pInspectionSetting.m_bCheckForSingleFov;
					m_pApp->m_nInSpectionDelay = m_pInspectionSetting.m_nInspectionDelay;

					if (m_pApp->m_bSetForAllTracks) {
						for (int i = 0; i < m_pApp->m_nTotalTracks; i++) {
							pTrack = &m_pApp->m_pTracks[i];
							pTrack->m_strStoredImagesPath = strFolderPath;
							strTemp.Format("Folder: StoredImagesT%02d", pTrack->m_nTrack + 1);
							PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", strTemp, strFolderPath);
						}
					}
					else
						m_pTrack->m_strStoredImagesPath = strFolderPath;
					strTemp.Format("Folder: StoredImagesT%02d", m_pTrack->m_nTrack + 1);
					PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", strTemp, strFolderPath);

					if (m_pApp->m_bSetForAllTracks) {
						BOOL bStoredImages = !m_bStoredImages;
						for (int i = 0; i < m_pApp->m_nTotalTracks; i++) {
							pTrack = &m_pApp->m_pTracks[i];
							pTrack->m_pDoc[m_nDocIdx]->m_bStoredImages = bStoredImages;
							if (bStoredImages)
								pTrack->m_pDoc[m_nDocIdx]->m_bInspectCycle = FALSE;
						}
					}
					else {
						m_bStoredImages = !m_bStoredImages;
						if (m_bStoredImages) {
							m_bInspectCycle = FALSE;
						}
					}
				}
			}
		}
	}
	else {
		m_bStoredImages = !m_bStoredImages;
		if (m_pApp->m_bSetForAllTracks) {
			BOOL bStoredImages = m_bStoredImages;
			for (int i = 0; i < m_pApp->m_nTotalTracks; i++) {
				pTrack = &m_pApp->m_pTracks[i];
				pTrack->m_pDoc[m_nDocIdx]->m_bStoredImages = bStoredImages;
			}
		}
		m_pApp->m_bSetForAllTracks = FALSE;
	}
	

	CString str;
	if(m_bStoredImages)
		str.Format("Stored Images Folder: %s", m_pTrack->m_strStoredImagesPath);
	else
		str.Format("");
	UpdateStatusBar(&str, 3);
}


void CAppDoc::OnUpdateInspectionStoredimages(CCmdUI *pCmdUI)
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateInspectionStoredimages(pCmdUI);
		return;
	}
	BOOL bTempContInsp = FALSE;
	for (int nTrack = 0; nTrack < m_pApp->m_nTotalTracks; nTrack++) {
		bTempContInsp |= m_pApp->m_pTracks[nTrack].m_bContInsp;
	}
	pCmdUI->Enable(theApp.m_bIsMasterApp && !m_pTrack->m_bLiveImage &&
				   !m_pApp->m_bOnline &&
				   !m_pApp->m_bTeaching &&
				   !m_pApp->m_bTrackInspSeq &&
				   !m_pTrack->m_bCamExtTrigEnable &&
				   !m_bInspSavedImage &&
				   !m_bInspectionDebug &&
				  // !m_pTrack->m_bContInsp &&
				   !bTempContInsp &&
				   (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nInspectSavedImage));// && m_pApp->m_bSuperUserMode);

	pCmdUI->SetCheck(m_bStoredImages);
}


void CAppDoc::OnInspectionCycle()
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnInspectionCycle();
		return;
	}

	if(GetKeyState(VK_SHIFT) & 0x8000) {
		BOOL bInspectCycle = !m_bInspectCycle;
		CTrackHandler* pTrack;
		for(int i=0; i<m_pApp->m_nTotalTracks; i++) {
			pTrack = &m_pApp->m_pTracks[i];
			pTrack->m_pDoc[m_nDocIdx]->m_bInspectCycle = bInspectCycle;
			if(bInspectCycle)
				pTrack->m_pDoc[m_nDocIdx]->m_bStoredImages = FALSE;
		}
	}
	else {
		m_bInspectCycle = !m_bInspectCycle;
		if(m_bInspectCycle) {
			m_bStoredImages = FALSE;
			OnInspectionInspect();
		}
	}
}


void CAppDoc::OnUpdateInspectionCycle(CCmdUI *pCmdUI)
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateInspectionCycle(pCmdUI);
		return;
	}

	pCmdUI->Enable(theApp.m_bIsMasterApp && !m_pTrack->m_bLiveImage &&
				   !m_pApp->m_bOnline &&
				   !m_pApp->m_bTeaching &&
				   !m_pApp->m_bTrackInspSeq &&
				   !m_pTrack->m_bCamExtTrigEnable &&
				   !m_bInspSavedImage && 
				   !m_pTrack->m_bContInsp &&
					!m_bStoredImages &&
		            !m_pApp->m_bInspectionAll &&
				   (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nInspectCycle));// && m_pApp->m_bSuperUserMode);

	pCmdUI->SetCheck(m_bInspectCycle);
}


void CAppDoc::OnInspectionCameraactive()
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnInspectionCameraactive();
		return;
	}

	BOOL bCameraActive = !m_bCameraActive;
	CTrackHandler* pTrack;
	for (int i = 0; i<m_pApp->m_nTotalTracks; i++) {
		pTrack = &m_pApp->m_pTracks[i];
		for (int j = 0; j<m_pApp->m_nTotalViewDocs[i]; j++)
			pTrack->m_pDoc[j]->m_bCameraActive = bCameraActive;
	}

	//if(GetKeyState(VK_SHIFT) & 0x8000) {
	//	BOOL bCameraActive = !m_bCameraActive;
	//	CTrackHandler* pTrack;
	//	for(int i=0; i<m_pApp->m_nTotalTracks; i++) {
	//		pTrack = &m_pApp->m_pTracks[i];
	//		pTrack->m_pDoc[m_nDocIdx]->m_bCameraActive = bCameraActive;
	//	}
	//}
	//else {
	//	m_bCameraActive = !m_bCameraActive;
	//}
}


void CAppDoc::OnUpdateInspectionCameraactive(CCmdUI *pCmdUI)
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateInspectionCameraactive(pCmdUI);
		return;
	}

	pCmdUI->Enable(!m_pApp->m_bOnline && 
		m_pTrack->m_bCamAvail && 
		(m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nCamEnable));
	pCmdUI->SetCheck(m_bCameraActive);
}

void CAppDoc::OnInspectCycleAutoStepMode() 
{
	m_bCycleAutoStepMode = !m_bCycleAutoStepMode;
}

void CAppDoc::OnUpdateInspectCycleAutoStepMode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_pTrack->m_bLiveImage &&
				   !m_pApp->m_bOnline &&
				   !m_pApp->m_bTeaching &&
				   !m_pApp->m_bTrackInspSeq &&
				   !m_pTrack->m_bCamExtTrigEnable &&
				   !m_pTrack->m_bContInsp &&
				   !m_bInspSavedImage);// && m_pApp->m_bSuperUserMode);
	
	pCmdUI->SetCheck(m_bCycleAutoStepMode);
}


void CAppDoc::OnInspectStoredImagesPath() 
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnInspectStoredImagesPath();
		return;
	}

	CString strFolderPath;
	BOOL bAll = GetKeyState(VK_SHIFT) & 0x8000;
	if(GetFolder(&strFolderPath, m_pTrack->m_strStoredImagesPath, "Stored Images Folder")){
		if(!strFolderPath.IsEmpty()){
			CString strTemp;
			if(bAll || GetKeyState(VK_SHIFT) & 0x8000) {
				CTrackHandler* pTrack;
				for(int i=0; i<m_pApp->m_nTotalTracks; i++) {
					pTrack = &m_pApp->m_pTracks[i];
					pTrack->m_strStoredImagesPath = strFolderPath;
					strTemp.Format("Folder: StoredImagesT%02d", pTrack->m_nTrack+1);
					PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", strTemp, strFolderPath);
				}
			}
			else {
				m_pTrack->m_strStoredImagesPath = strFolderPath;
				strTemp.Format("Folder: StoredImagesT%02d", m_pTrack->m_nTrack+1);
				PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", strTemp, strFolderPath);
			}

			//UpdateStatusBar(&CString("Stored Images Folder = " + strFolderPath), 3);
			m_bStoredImages = FALSE;
			OnInspectionStoredimages();
		}
	}
}

void CAppDoc::OnUpdateInspectStoredImagesPath(CCmdUI *pCmdUI)
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateInspectStoredImagesPath(pCmdUI);
		return;
	}

	pCmdUI->Enable(!m_pTrack->m_bLiveImage &&
				   !m_pApp->m_bOnline &&
				   !m_pApp->m_bTeaching &&
				   !m_pApp->m_bTrackInspSeq &&
				   !m_pTrack->m_bCamExtTrigEnable &&
				   !m_pTrack->m_bContInsp &&
				   !m_bInspSavedImage && m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nSetFolder);// && m_pApp->m_bSuperUserMode);
}

void CAppDoc::OnInspectSavePassImages() 
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnInspectSavePassImages();
		return;
	}

	if(m_lDebugFlag & DEBUG_SAVE_GOOD_IMAGE)
		m_lDebugFlag &= ~DEBUG_SAVE_GOOD_IMAGE;
	else
		m_lDebugFlag |= DEBUG_SAVE_GOOD_IMAGE;

	SaveDebugFlags();
}

void CAppDoc::OnUpdateInspectSavePassImages(CCmdUI *pCmdUI)
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateInspectSavePassImages(pCmdUI);
		return;
	}

	pCmdUI->Enable(theApp.m_bIsMasterApp && !m_pApp->m_bOnline && m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nSaveOfflinePassImages);
	pCmdUI->SetCheck((m_lDebugFlag & DEBUG_SAVE_GOOD_IMAGE) ? 1 : 0);
}

void CAppDoc::OnInspectSaveFailImages() 
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnInspectSaveFailImages();
		return;
	}

	if(m_lDebugFlag & DEBUG_SAVE_BAD_IMAGE)
		m_lDebugFlag &= ~DEBUG_SAVE_BAD_IMAGE;
	else
		m_lDebugFlag |= DEBUG_SAVE_BAD_IMAGE;

	SaveDebugFlags();
}

void CAppDoc::OnUpdateInspectSaveFailImages(CCmdUI *pCmdUI)
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateInspectSaveFailImages(pCmdUI);
		return;
	}

	pCmdUI->Enable(theApp.m_bIsMasterApp && !m_pApp->m_bOnline && m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nSaveOfflineFailImages);
	pCmdUI->SetCheck((m_lDebugFlag & DEBUG_SAVE_BAD_IMAGE) ? 1 : 0);
}

void CAppDoc::SaveDebugFlags()
{
	CString str;
	str.Format("DebugFlag C%d", m_pTrack->m_nTrack+1);
	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", str, (int)m_lDebugFlag);
}

void CAppDoc::OnCameraParmButton() 
{
	m_pTrack->m_pCamSetupDlg = new CCameraSetupDialog(m_pTrack);
	m_pTrack->m_pCamSetupDlg->m_nExposure = m_pTrack->m_nExposure;
	for(int i=0; i<3; i++)
		m_pTrack->m_pCamSetupDlg->m_nGains[i] = m_pTrack->m_nGain[i];
	m_pTrack->m_pCamSetupDlg->m_nBlackLevel = m_pTrack->m_nBlackLevel;

	CString str;
	str.Format("Camera Setup [Camera %d]", m_pTrack->m_nTrack+1);
	m_pTrack->m_pCamSetupDlg->m_strTitle = str;
	if(!m_pTrack->m_pCamSetupDlg->Create(IDD_CAMERA_SETUP_DIALOG, (CMainFrame*)AfxGetMainWnd())) {
		m_pTrack->m_pCamSetupDlg = NULL;
		return;
	}
	m_pTrack->m_bCamSetupDlgOpen = TRUE;
	CRect rectParent;
	CRect sizeCanvas;
	((m_pTrack->m_pCamSetupDlg)->GetParent())->GetClientRect(&rectParent);
	m_pTrack->m_pCamSetupDlg->GetClientRect(&sizeCanvas);	 
	m_pTrack->m_pCamSetupDlg->SetWindowPos(NULL, (rectParent.right-20)-sizeCanvas.Width(), rectParent.top+100, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_pTrack->m_pCamSetupDlg->ShowWindow(TRUE);
}

void CAppDoc::OnUpdateCameraParmButton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.m_bIsMasterApp && !m_pApp->m_bOnline &&
		!m_pTrack->m_bCamSetupDlgOpen &&
		(m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nCameraCfg));
}

void CAppDoc::OnViewOptionsSetupGrid()
{
	m_bSetupGrid = !m_bSetupGrid;
	DeleteAllDisplay(TRUE);
	if(!m_bSetupGrid)
		return;

	LOGPEN PenInfo, PenInfo1;
	CPoint point1,point2;
	CSize  szImgSize, szFraction;

	PenInfo.lopnColor = RGB(0,255,0);
	PenInfo.lopnStyle = PS_SOLID;
	PenInfo.lopnWidth.x = 1;

	PenInfo1.lopnColor = RGB(255,0,0);
	PenInfo1.lopnStyle = PS_SOLID;
	PenInfo1.lopnWidth.x = 5;

	szImgSize = m_Buffer.GetImgSize();
	///GetResolution(&m_pTrack->m_FGResource, &szImgSize);
	szFraction.cx = szImgSize.cx/8;
	szFraction.cy = szImgSize.cy/8;

	for(int i=1; i< 8; i++) {
		point1.x = szFraction.cx * i;
		point1.y = 0;
		point2.x = szFraction.cx * i;
		point2.y = szImgSize.cy;
		DrwLine(&m_ImageViewInfo,&point1,&point2,&PenInfo);

		point1.x = 0;
		point1.y = szFraction.cy * i;
		point2.x = szImgSize.cx;
		point2.y = szFraction.cy * i;
		DrwLine(&m_ImageViewInfo,&point1,&point2,&PenInfo);
	}

	point1.x = szImgSize.cx/2;
	point1.y = 0;
	point2.x = szImgSize.cx/2;
	point2.y = szImgSize.cy;
	DrwLine(&m_ImageViewInfo,&point1,&point2,&PenInfo1);

	point1.x = 0;
	point1.y = szImgSize.cy/2;
	point2.x = szImgSize.cx;
	point2.y = szImgSize.cy/2;
	DrwLine(&m_ImageViewInfo,&point1,&point2,&PenInfo1);

	DrwUpdate(&m_ImageViewInfo);
}

void CAppDoc::OnUpdateViewOptionsSetupGrid(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bSetupGrid);
	pCmdUI->Enable(!m_pApp->m_bOnline && m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nSetupGrid);
}

void CAppDoc::OnViewOpticsRoi()
{
	m_pTrack->m_bOpticsRoi = !m_pTrack->m_bOpticsRoi;
	if (m_pTrack->m_bOpticsRoi) {
		DrawOpticsROIs(TRUE);
		DrwUpdate(&m_ImageViewInfo);
		UpdateStatusBar(&CString("Optics Characteristics"), 2);
		UpdateStatusBar(&CString("Reposition the ROI"), 3);
	}
	else {
		for (int i = 0; i<m_pTrack->m_nOpticsFrames; i++)
			m_pTrack->m_rectOptics[i] = GetTrackRect(&m_ImageViewInfo, i);
		DrwDel(&m_ImageViewInfo);
		DrwUpdate(&m_ImageViewInfo);
		UpdateStatusBar(&CString(""), 2);
		UpdateStatusBar(&CString(""), 3);
	}
}

void CAppDoc::OnUpdateViewOpticsRoi(CCmdUI* pCmdUI)
{

	pCmdUI->SetCheck(m_pTrack->m_bOpticsRoi);
	pCmdUI->Enable(m_pTrack->m_bViewOpticsPanel &&
		!m_pApp->m_bTeaching &&
		!m_pTrack->m_bContInsp &&
		!m_pTrack->m_bTrackContInsp &&
		!m_pApp->m_bInspecting
		/*!m_pApp->m_bCalibrating &&*/
		/* !m_pApp->m_bCalibratingDevice &&*/
		&& !m_pApp->m_bOnline && (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >=0 /*m_pApp->m_AdvSecurityDlg.m_data.m_nOpticsROI*/));
	/* pCmdUI->Enable(!m_pApp->m_bOnline && (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nOpticalPanel));*/
}


void CAppDoc::OnSelchangeFramecountCombo()
{
	CString strFileName, strDiectoryName, strTrack;
	int nFrames = m_pTrack->m_nOpticsFrames;

	int nOffsetTop, nOffsetBottom;
	nOffsetTop = nOffsetBottom = 20;

	strFileName.Format("Camera%d", m_pTrack->m_nTrack + 1);
	strTrack.Format("Track%d", m_pTrack->m_nTrack + 1);
	strDiectoryName = m_pTrack->m_pApp->m_strMainCfgDir + "\\" + m_pApp->m_strConfigFile[0] + "\\" + strTrack;
	//((CMainFrame*)AfxGetMainWnd())->m_wndOpticsPanel.OnResizeFrames(m_pTrack->m_nOpticsFrames, m_pTrack->m_nOpticsAvgSize);
	((CMainFrame*)AfxGetMainWnd())->m_wndOpticsPanel.OnResizeFrames(m_pTrack->m_nOpticsFrames, m_pTrack->m_nOpticsAvgSize, nOffsetTop, nOffsetBottom);

	////	m_pTrack->SaveOpticsSettings(/*m_pTrack->*/m_strConfigDir[g_nConPosIdx], m_pContactLensBVApp->m_strConfigFile[m_pContactLensBVApp->m_nConFilePosIdx]);
	m_pTrack->SaveOpticsSettings(/*strDiectoryName, m_pApp->m_strConfigFile[0]*/);


	if (m_pTrack->m_bOpticsRoi && nFrames != m_pTrack->m_nOpticsFrames) {
		for (int i = 0; i<nFrames; i++)
			m_pTrack->m_rectOptics[i] = GetTrackRect(&m_ImageViewInfo, i);
		DrwDel(&m_ImageViewInfo);
		DrawOpticsROIs(TRUE);

		m_pTrack->m_nOpticsRecalcMode = 2;
		DrwUpdate(&m_ImageViewInfo);
		SetTrackRectProperties(&m_ImageViewInfo, TRUE, FALSE, TRUE);
	}
}

void CAppDoc::OnViewOpticsPanel() 
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	m_pTrack->m_bViewOpticsPanel = !(pMain->m_wndOpticsPanel.IsVisible());

	int nOffsetTop, nOffsetBottom;
	nOffsetTop = nOffsetBottom = 20;

	if (m_pTrack->m_bViewOpticsPanel) {
		pMain->m_wndOpticsPanel.m_nCurrentTrack = m_pTrack->m_nTrack;
		pMain->m_wndOpticsPanel.m_nCurrentDoc = m_nDocIdx;

		CString strFocusingToolName = "Focusing Tool - ";
		if (m_pTrack->m_nTrack == 0) {
			strFocusingToolName = strFocusingToolName + theApp.m_strTopCameraName.GetAt(m_nDocIdx);
		}
		else if (m_pTrack->m_nTrack == 1) {
			strFocusingToolName = strFocusingToolName + theApp.m_strBottomCameraName.GetAt(m_nDocIdx);
		}
		else {
			strFocusingToolName = strFocusingToolName + theApp.m_strSideCameraName.GetAt(m_nDocIdx);
		}
		pMain->m_wndOpticsPanel.SetWindowText(strFocusingToolName);

		//pMain->m_wndOpticsPanel.OnResizeFrames(m_pTrack->m_nOpticsFrames, m_pTrack->m_nOpticsAvgSize);
		pMain->DisplayOpticsPanel(TRUE);
		m_pTrack->m_nOpticsRecalcMode = 2;
		UpdateAllViews(FALSE);
		//pMain->m_wndOpticsPanel.SetStats(m_pTrack->m_nOpticsFrames, m_pTrack->m_nOpticsAvgSize);
		pMain->m_wndOpticsPanel.SetStats(m_pTrack->m_nOpticsFrames, m_pTrack->m_nOpticsAvgSize, nOffsetTop, nOffsetBottom);

		//Show Focusing ROI
		m_pTrack->m_bOpticsRoi = TRUE;
		DrawOpticsROIs(TRUE);
		DrwUpdate(&m_ImageViewInfo);
		UpdateStatusBar(&CString("Optics Characteristics"), 2);
		UpdateStatusBar(&CString("Reposition the ROI"), 3);
	}
	else {
		pMain->DisplayOpticsPanel(FALSE);
		m_pTrack->m_bOpticsRoi = FALSE;
		DrwDel(&m_ImageViewInfo);
		DrwUpdate(&m_ImageViewInfo);
		//pMain->m_wndOpticsPanel.GetStats(m_pTrack->m_nOpticsFrames, m_pTrack->m_nOpticsAvgSize);
		pMain->m_wndOpticsPanel.GetStats(m_pTrack->m_nOpticsFrames, m_pTrack->m_nOpticsAvgSize, nOffsetTop, nOffsetBottom);
		UpdateStatusBar(&CString(""), 2);
		UpdateStatusBar(&CString(""), 3);
	}

}

void CAppDoc::OnUpdateViewOpticsPanel(CCmdUI* pCmdUI) 
{	
	if (m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateViewOpticsPanel(pCmdUI);
		return;
	}

	pCmdUI->Enable(!m_pTrack->m_bLiveImage &&
		!m_pApp->m_bOnline &&
		!m_pApp->m_bTeaching &&
		!m_pApp->m_bInspecting &&
		!m_pApp->m_bTrackInspSeq &&
		!m_pTrack->m_bContInsp &&
		!m_pTrack->m_bCamExtTrigEnable &&
		!m_bInspSavedImage &&
		!m_bStoredImages &&
		(m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nOpticalPanel));
}

void CAppDoc::OnViewSelectGoodB() 
{
	CSelectGoodBDlg dlg;
	dlg.DoModal();
}

void CAppDoc::OnUpdateViewSelectGoodB(CCmdUI* pCmdUI) 
{	
	pCmdUI->Enable(!m_pApp->m_bOnline && 
		(m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= 1/*m_pApp->m_AdvSecurityDlg.m_data.m_nSelectGoodB*/));
}

void CAppDoc::OnViewOptionsShowOverlay()
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnViewOptionsShowOverlay();
		return;
	}

	for(int i=0; i<m_pTrack->m_nTotalDocs; i++)
	{
		m_pTrack->m_pDoc[i]->m_bShowOverlay = !m_pTrack->m_pDoc[i]->m_bShowOverlay;
		DrwUpdate(&m_pTrack->m_pDoc[i]->m_ImageViewInfo);
	}
}

void CAppDoc::OnUpdateViewOptionsShowOverlay(CCmdUI *pCmdUI)
{
	if(m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateViewOptionsShowOverlay(pCmdUI);
		return;
	}

	pCmdUI->SetCheck(m_bShowOverlay);
	pCmdUI->Enable(m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nShowOverLay);
}

void CAppDoc::OnUpdateViewOptions(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_pApp->m_bOnline);
}

int CAppDoc::ApplyParameters(int nFOV)
{
	//if(m_pTrack->m_nTrack==0 && m_nDocIdx==0) {
		//OutputDebugLogTo(8, TRUE, "Doc Config Folder: %s", m_pTrack->m_strConfigDir);
		//m_InspectionHandler[m_pTrack->m_nFOVIndex].LoadRecipe(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
		//								&m_Buffer,
		//								/*m_pTrack->*/m_strConfigDir,
		//								m_pApp->m_strConfigFile,
		//								m_pTrack->m_nTrack,
		//								m_pTrack->m_nFOVIndex,m_nDocIdx);
		m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/nFOV].LoadInspectParameters(m_strConfigDir,m_pApp->m_strConfigFile, nFOV);
		m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/nFOV].LoadTeachParameters(m_strConfigDir,m_pApp->m_strConfigFile, nFOV);
		//m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/nFOV].LoadCalibrationParameters(m_strConfigDir, m_pApp->m_strConfigFile, nFOV);
//	}
	return 0;
}

BOOL CAppDoc::Navigate(int& nIndex,int nDoc)
{
	while(TRUE) {
		if(WaitResponse(&m_pApp->m_pTracks[m_pTrack->m_nTrack].m_pDoc[nDoc]->m_ImageViewInfo))
			return FALSE;
		/*if(m_bPrevStep) {
			if(nIndex == 0)
				continue;
			nIndex-=2;
		}*/
		break;
	}

	return TRUE;
}




void CAppDoc::OnInspectStepDebugSelection()
{
	CStepSelection dlg;
	dlg.DoModal();
}


void CAppDoc::OnUpdateInspectStepDebugSelection(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
	//pCmdUI->Enable((!m_pTrack->m_bLiveImage &&
	//			   !m_pApp->m_bOnline &&
	//			   !m_pApp->m_bTeaching &&
	//			   !m_pApp->m_bInspecting &&
	//			   !m_pApp->m_bTrackInspSeq &&
	//			   !m_pApp->m_bLightCtrlDlgOpen &&
	//			   !m_pTrack->m_bContInsp &&
	//			   !m_bInspSavedImage &&
	//			   !m_pTrack->m_bCamExtTrigEnable &&
	//			   (m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nStepSelection)));// && m_pApp->m_bSuperUserMode);

}

int CAppDoc::LoadImagesToDoc(CString strPath)
{
	int nError, nTrack, nPos=-1, nCount=0;
	CString strImageName, strImageExt, strImageFinalPath, strPos;
	CString strType[] = {"T", "B", "S"};

	nTrack = m_pTrack->m_nTrack;
	nCount = m_pApp->m_nTotalViewDocs[nTrack];
	nPos = strPath.ReverseFind('_');

	UpdateStatusBar(&CString(""));
	if(nPos > 0) {
		strImageExt = strPath.Mid(strPath.ReverseFind('.'));
		strImageName = strPath.Mid(0, nPos);
		for(int nDoc=0; nDoc<nCount; nDoc++) {
			strImageFinalPath.Format("%s_%s%d%s", strImageName, strType[nTrack], nDoc+1, strImageExt);
			nError = m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->LoadStaticImage(strImageFinalPath);
			if(nError)
				UpdateStatusBar(&CString("Load Image Error"));

			CRect rectTemp = CRect(CPoint(0, 0), m_pTrack->m_SharedData.pProcBuffers[m_nDocIdx]->GetImgSize());
			ImgCopy(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer, &rectTemp, &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_bufPVIBackup, &rectTemp);
		}
	}
	else
		UpdateStatusBar(&CString("Load Image Error"));

	if(nTrack == m_pApp->m_nTotalTracks-1){
		m_pTrack->m_charTempPosNameForInsp = '0';
		m_pTrack->m_nFOVIndex = m_pApp->m_pTracks[nTrack-1].m_nFOVIndex;//0;
	}
	else {
		int nTempLen,nTempSel;
		strPos = strImageName.Mid(strImageName.ReverseFind('P'), 4);
		nTempLen = strPos.GetLength();
		m_pTrack->m_charTempPosNameForInsp = strPos[nTempLen-1];
		nTempSel = atoi(&m_pTrack->m_charTempPosNameForInsp);
		m_pTrack->m_nFOVIndex = nTempSel-1;

		//nPos = strImageName.ReverseFind('_');
	/*	CString strDevice = strImageName.Mid(strImageName.ReverseFind('D'));
		strImageName = strDevice.Mid(0, nPos);*/
		nPos = strImageName.ReverseFind('_');
		/*strPos = strImageName.Mid(strImageName.ReverseFind('P'), 4);
		nTempLen = strDevice.GetLength();*/
		m_pTrack->m_charTempPosNameForInsp = strImageName[nPos-1];
		nTempSel = atoi(&m_pTrack->m_charTempPosNameForInsp);
		m_pTrack->m_nDieIndex = nTempSel-1;
	}

	return nError;
}


int CAppDoc::LoadImagesToDocForInspection(CString strPath,int nFov)
{
	int nError = 0, nTrack, nPos = -1, nCount = 0;
	CString strImageName, strImageExt, strImageFinalPath, strPos;
	CString strType[] = { "T", "B", "S" };

	nTrack = m_pTrack->m_nTrack;
	nCount = m_pApp->m_nTotalViewDocs[nTrack];
	nPos = strPath.ReverseFind('_');

	UpdateStatusBar(&CString(""));
	if (nPos > 0) {
		strImageExt = strPath.Mid(strPath.ReverseFind('.'));
		strImageName = strPath.Mid(0, nPos);
		CString strTemp;
		int nPosEnd = strImageName.ReverseFind('_');
		strTemp = strImageName.Mid(0, nPosEnd);
		for (int nDoc = 0; nDoc<nCount; nDoc++) {

			/*if (!theApp.motionParameters.IsGrabEnabled(nTrack, nDoc, nFov))
				continue;*/

			strImageFinalPath.Format("%s_Pos%d_%s%d%s", strTemp,nFov+1, strType[nTrack], nDoc + 1, strImageExt);
			nError = m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->LoadStaticImage(strImageFinalPath);
			if (nError)
				UpdateStatusBar(&CString("Load Image Error"));
			else {
				int nTempLen, nTempSel;

				strPos = strImageName.Mid(strImageName.ReverseFind('P'), 4);
				nTempLen = strPos.GetLength();
				m_pTrack->m_charTempPosNameForInsp = strPos[nTempLen - 1];
				nTempSel = atoi(&m_pTrack->m_charTempPosNameForInsp);
				m_pTrack->m_nFOVIndex = nFov;

				//nPos = strImageName.ReverseFind('_');
				/*	CString strDevice = strImageName.Mid(strImageName.ReverseFind('D'));
				strImageName = strDevice.Mid(0, nPos);*/
				nPos = strImageName.ReverseFind('_');
				/*strPos = strImageName.Mid(strImageName.ReverseFind('P'), 4);
				nTempLen = strDevice.GetLength();*/
				m_pTrack->m_charTempPosNameForInsp = strImageName[nPos - 1];
				nTempSel = atoi(&m_pTrack->m_charTempPosNameForInsp);
				m_pTrack->m_nDieIndex = nTempSel - 1;
			}
			CRect rectTemp = CRect(CPoint(0, 0), m_pTrack->m_SharedData.pProcBuffers[m_nDocIdx]->GetImgSize());
			ImgCopy(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer, &rectTemp, &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_bufPVIBackup, &rectTemp);
			ImgCopy(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer, &rectTemp, &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[m_pTrack->m_nDieIndex][nFov], &rectTemp);

			//Send Image To Server for Centralized Reject Images
			if (m_pApp->m_bCentralizedVerificationMode && !m_pApp->m_CentralizedCommInterface.m_bIsTCPServer) {
				m_pApp->m_pImageQueue->PushImageBufferCentralized(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[m_pTrack->m_nDieIndex][nFov], nTrack, nDoc, m_pTrack->m_nDieIndex, nFov);
			}
		}
	}
	else
		UpdateStatusBar(&CString("Load Image Error"));

	//if (nTrack == m_pApp->m_nTotalTracks - 1) {
	//	m_pTrack->m_charTempPosNameForInsp = '0';
	//	m_pTrack->m_nFOVIndex = nFov; // 0;
	//}
	//else {
	//	
	//}

	return nError;
}

int CAppDoc::LoadMultipleImagesToDoc(CStringArray& strFileNames, int nPosition, int nDevice)
{
	DrawView(FALSE);
	UpdateStatusBar(&CString("")); 

	int nTrack, nDocCount;
	CString strPath, strDevice, strPos, strSubPath;
	CString strType[] = {"T", "B", "S"};

	nTrack = m_pTrack->m_nTrack;
	nDocCount = m_pApp->m_nTotalViewDocs[nTrack];
	m_pApp->m_pTracks[nTrack].m_nFOVIndex = nPosition;
	m_pApp->m_pTracks[nTrack].m_nDieIndex = nDevice;

	if(m_pApp->m_bLoadMultiImagesFromFile){
		strPath = m_pApp->m_strLoadMultiImagesFolderPath;
		strDevice.Format("THA%d", nDevice+1);
		strPos.Format("Pos%d", nPosition+1);

		strSubPath.Format("%s\\", strPath /*strFolderType[nTrack]*/);
		for(int nDoc=0; nDoc<nDocCount; nDoc++) {
			m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer.ClrImg();
		}
		int nError=-1, nDevicePos=-1, nPos=-1;
		CString strImgPath, strImgName, strTempName;
		for(int nCount=0; nCount<strFileNames.GetCount(); nCount++) {
			nDevicePos = strFileNames[nCount].Find(LPCSTR(strDevice));//ReverseFind(strFileNames[nCount], strDevice);
			nPos = strFileNames[nCount].Find(LPCSTR(strPos));//ReverseFind(strFileNames[nCount], strPos);
			if(nDevicePos > 0 && nPos > 0) {
				strImgPath =strFileNames[nCount];
				strTempName = strImgPath.Mid(0, nDevicePos);
				for(int nDoc=0; nDoc<nDocCount; nDoc++) {
					strImgName.Format("%s%s_%s_%s%d.bmp", strTempName, strDevice, strPos, strType[nTrack], nDoc+1);
					nError = m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->LoadStaticImage(strImgName);
					if (nError) {
						UpdateStatusBar(&CString("Load Image Error"));
					}
					else {
						CRect rect = CRect(CPoint(0, 0), m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer.GetImgSize());
						ImgCopy(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer, &rect, 
								&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDevice][nPosition], &rect);
					}
				}
				if(!nError) {
					if(nTrack == m_pApp->m_nTotalTracks-1)
						m_pTrack->m_charTempPosNameForInsp = '0';
					else {
						int nTempLen;
						strPos = strImgName.Mid(strImgName.ReverseFind('P'), 4);
						nTempLen = strPos.GetLength();
						m_pTrack->m_charTempPosNameForInsp = strPos[nTempLen-1];
					}
					break;
				}
			}
			else
				UpdateStatusBar(&CString("Load Image Error"));
		}
	}
	DrawView();

	return 0;
}


int CAppDoc::GetImageFilePaths(CString strStartFolder, CStringArray& arrayFilePaths)
{
	CString strFilePath;
	strFilePath.Format("%s%s", strStartFolder, "*.*");
	CString strFolder, strFile;

	CFileFind FileFind;
	BOOL bNextFile = FileFind.FindFile(strFilePath);
	while (bNextFile) {
		bNextFile = FileFind.FindNextFile();
		if (FileFind.IsDots())
			continue;

		strFile = FileFind.GetFileName();
		if (FileFind.IsDirectory()) {
			strFolder.Format("%s%s\\", strStartFolder, strFile);
			GetImageFilePaths(strFolder, arrayFilePaths);
		}
		else if (!strFile.Right(3).CompareNoCase("bmp"))
			arrayFilePaths.Add(strStartFolder + strFile);
	}
	return 0;
}

void CAppDoc::DrawView(BOOL bUpdate)
{
	int nTrack, nCount;

	nTrack = m_pTrack->m_nTrack;
	nCount = m_pApp->m_nTotalViewDocs[nTrack];

	for(int nView=0; nView<nCount; nView++) {
		if(bUpdate)
			DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nView]->m_ImageViewInfo);
		else
			DrwDel(&m_pApp->m_pTracks[nTrack].m_pDoc[nView]->m_ImageViewInfo);
	}
}

void CAppDoc::SaveStaticImagePath(CString strPath)
{
	CString strTemp;
	m_pTrack->m_strLoadStaticImgFolderPath = strPath;
	strTemp.Format("Folder: StaticImagesT%02d", m_pTrack->m_nTrack+1);
	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", strTemp, m_pTrack->m_strLoadStaticImgFolderPath);
}

void CAppDoc::OnPviDefectShow()
{
	CRect rect1, rect2;

	m_pApp->m_pWndPVIDefectParameters->m_nTrackId = m_pTrack->m_nTrack;
	m_pApp->m_pWndPVIDefectParameters->m_nDocId = m_nDocIdx;
	m_pApp->m_pWndPVIDefectParameters->Initialize(m_pApp->m_pTracks[m_pTrack->m_nTrack].m_nFOVIndex);

	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rect1, 0);
	m_pApp->m_pWndPVIDefectParameters->GetWindowRect(&rect2);

	m_pApp->m_pWndPVIDefectParameters->SetWindowPos(NULL, rect1.Width() - rect2.Width(), rect1.Height()-rect2.Height()-20, 0, 0, SWP_NOSIZE);

	m_pApp->m_pWndPVIDefectParameters->ShowWindow(TRUE);
}

void CAppDoc::OnUpdatePviDefectShow(CCmdUI *pCmdUI)
{
	if (m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateParmVision(pCmdUI);
		return;
	}
	pCmdUI->Enable(theApp.m_bIsMasterApp && !m_pTrack->m_bContInsp &&
					!m_pApp->m_bOnline &&
					!m_pApp->m_bTeaching &&
					!m_pApp->m_bInspecting &&
					!m_bSavedImagesStep &&
					!m_bInspSavedImage &&
					!m_bInspectionDebug &&
					// !bChecked &&
					(m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nPviDefectParm));
}



void CAppDoc::OnCalibrationSettings()
{
	m_pApp->m_bCalibrationOpen = TRUE;
	m_pApp->m_pCalibrationDlg->nTrack  = m_pTrack->m_nTrack;
	//m_pApp->m_pCalibrationDlg->nDoc = this->m_nDocIdx;

	m_pApp->m_pCalibrationDlg->Initialize(m_nDocIdx);

	CRect rect1, rect2;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rect1, 0);
	m_pApp->m_pCalibrationDlg->GetWindowRect(&rect2);

	m_pApp->m_pCalibrationDlg->SetWindowPos(NULL, (rect1.Width()/2)-(rect2.Width()/2), (rect1.Height()/2)-(rect2.Height()/2),rect2.Width(),rect2.Height(),SWP_NOSIZE);

	m_pApp->m_pCalibrationDlg->ShowWindow(TRUE);
	

}

void CAppDoc::OnUpdateCalibrationSettings(CCmdUI *pCmdUI)
{
	if (m_pRemoteInspDoc) {
		m_pRemoteInspDoc->OnUpdateViewOpticsPanel(pCmdUI);
		return;
	}
	pCmdUI->Enable(!m_pTrack->m_bContInsp &&
		!m_pApp->m_bOnline &&
		!m_pApp->m_bTeaching &&
		!m_pApp->m_bInspecting &&
		!m_bSavedImagesStep &&
		!m_bInspSavedImage &&
		!m_bInspectionDebug &&
		!m_pApp->m_bLightCalibration &&
		!m_pApp->m_bDotCalibration &&
		!m_bStoredImages &&
		(m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nCalibSettings));
}

int CAppDoc::SetLightCalibrationOffline()
{
	CString strStoredCalibImagessPath;
	GetFolder(&strStoredCalibImagessPath, "", "Calib Light Images Folder");

	CImageBuffer bufMaxImageCalib;
	bufMaxImageCalib.Allocate(m_Buffer.GetImgSize());
	CString strErrorMsg;
	int nError = m_InspectionHandler[0].m_Inspection.LightCalibrationOffline(strStoredCalibImagessPath, &bufMaxImageCalib, strErrorMsg);
	if (!nError) {
		if (strErrorMsg == "") {
			strErrorMsg = "Do you want to save Light Calibration Image?";
		}

		CRect rectTemp = CRect(CPoint(0, 0), bufMaxImageCalib.GetImgSize());
		ImgCopy(&bufMaxImageCalib, &rectTemp, &m_Buffer, &rectTemp);
		DrwUpdate(&m_ImageViewInfo);

		if (AfxMessageBox(strErrorMsg, MB_YESNO) == IDYES) {
			CString strLightCorrectionImage;
			CString str[] = { "Top", "Bottom", "Side" };
			strLightCorrectionImage.Format("%s\\%s_Light_Correction.bmp", m_pApp->m_strCalibDir, str[m_pTrack->m_nTrack]);
			SaveGrayScaleBMPFile(strLightCorrectionImage, bufMaxImageCalib);

			m_pApp->m_pTracks[m_pTrack->m_nTrack].m_pDoc[0]->m_InspectionHandler[0].LoadCalibrationParameters(m_pApp->m_strCalibDir, m_pApp->m_strConfigFile, m_pTrack->m_nTrack, 0);
			CCalibrationData* pCalibData = &m_pApp->m_pTracks[m_pTrack->m_nTrack].m_pDoc[0]->m_InspectionHandler[0].m_Inspection.m_CalibrationData;
			m_InspectionHandler[0].m_Inspection.ClearLightCorrectionParam(pCalibData);

			int nDevicePerX, nDevicePerY;
			int nFovPerX, nFovPerY;
			m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
			int m_nNoOfFov = nFovPerX * nFovPerY;
			for (int i = 0; i < m_nNoOfFov; i++) {
				m_pTrack->m_pDoc[0]->SaveTeachParmAutoTeach(i);
			}

			AfxMessageBox("Calibrated Image Saved.", IDOK);
		}
		else
			AfxMessageBox("Calibration Abort", IDOK);
	}
	else {
		AfxMessageBox(strErrorMsg, IDOK);
	}

	UpdateStatusBar(&CString("Calibration Done"));

	if (!m_pApp->m_pCalibrationDlg->IsWindowVisible())
		m_pApp->m_pCalibrationDlg->ShowWindow(TRUE);

	m_pApp->m_bLightCalibration = FALSE;

	return nError;
}

int CAppDoc::SetLightCalibration()
{
	if (m_bLightCalibration /*&& m_pApp->m_pLightCtrlDlg && m_pApp->m_bLightCtrlDlgOpen*/) {
		m_pApp->m_bLightCalibration = TRUE;
		DrwDel(&m_ImageViewInfo);
		UpdateStatusBar(&CString("Light Calibration Processing..."));
		
		CImageBuffer  bufCalibResult;
		bufCalibResult.Allocate(m_Buffer.GetImgSize());

		CPerformanceTimer timer;
		int nFOV, nReferenceImgCount, nAvgImgCount, nError, XAxis, YAxis;
		BOOL bFirst, bRefImg, bAvgImg, bLightCorrection;

		nFOV = 0;//// cabliration for first FOV
		nReferenceImgCount = 4;//// for motor movement (small)
		nAvgImgCount = 30;//// for avg image capture loop
		XAxis = 0;
		YAxis = 1;
		bFirst = bRefImg = bAvgImg = bLightCorrection = FALSE;
		
		CCalibrationData* pCalibData = &m_pApp->m_pTracks[m_pTrack->m_nTrack].m_pDoc[0]->m_InspectionHandler[nFOV].m_Inspection.m_CalibrationData;
		m_pApp->m_pTracks[m_pTrack->m_nTrack].m_pDoc[0]->m_InspectionHandler[nFOV].ClearCalibParam(pCalibData, TRUE);

		CString strErrorMsg;
		double resolution[2];
		int nAxis[2];
		int nCurrentPos[2];
		nAxis[0] = X_SERVO_MOTOR;
		nAxis[1] = Y_SERVO_MOTOR;
		theApp.motionParameters.GetMotorResolution(nAxis[0], resolution[0]);
		theApp.motionParameters.GetMotorResolution(nAxis[1], resolution[1]);
		const int relativeOffsetX = static_cast<int>(m_dCalibStepXSize * resolution[0]);
		const int relativeOffsetY = static_cast<int>(m_dCalibStepYSize * resolution[1]);

		theApp.motionParameters.GetCurrentMotorPosition(nAxis[0], nCurrentPos[0]);
		theApp.motionParameters.GetCurrentMotorPosition(nAxis[1], nCurrentPos[1]);

		for (size_t x = 0; x < m_nCalibStepXCount; ++x) {
			nError = m_pApp->motionParameters.RelativeMove(X_SERVO_MOTOR, relativeOffsetX, m_pApp->motionParameters.getMovingSpeed(X_SERVO_MOTOR));
			if (nError) {
				OutputDebugLogTo(0, TRUE, "----- ERROR : Cannot move X motor");
				return nError;
			}

			if (theApp.m_bExitApp)
				return 1;

			for (size_t y = 0; y < m_nCalibStepYCount; ++y) {
				nError = m_pApp->motionParameters.RelativeMove(Y_SERVO_MOTOR, x % 2 == 0 ? -relativeOffsetY : relativeOffsetY, 
																m_pApp->motionParameters.getMovingSpeed(Y_SERVO_MOTOR));

				if (nError) {
					OutputDebugLogTo(0, TRUE, "----- ERROR : Cannot move Y motor");
					return nError;
				}
				if (y == 0) {
					nError = m_pApp->motionParameters.WaitForMotionDone(X_SERVO_MOTOR, m_pApp->getMotionTimeOut());
					if (nError) {
						OutputDebugLogTo(0, TRUE, "----- ERROR : Cannot WaitForMotionDone X motor");
					}
				}
				nError = m_pApp->motionParameters.WaitForMotionDone(Y_SERVO_MOTOR, m_pApp->getMotionTimeOut());
				if (nError) {
					OutputDebugLogTo(0, TRUE, "----- ERROR : Cannot WaitForMotionDone Y motor");
				}

				//Grab Image Sequence
				for (int nInnerLoop = 0; nInnerLoop < nAvgImgCount; nInnerLoop++) {
					timer.Reset();
					OnCameraSnap();

					bRefImg = TRUE;
					bAvgImg = FALSE;
					bFirst = (!nInnerLoop) ? TRUE : FALSE;
					if (nInnerLoop == nAvgImgCount - 1)
						bAvgImg = TRUE;
					if ((x == m_nCalibStepXCount - 1 && y == m_nCalibStepYCount -1) && (nInnerLoop == nAvgImgCount - 1)) {
						bLightCorrection = TRUE;
						OutputDebugLogTo(6, TRUE, "[Light Calibration] Final");
					}

					m_InspectionHandler[nFOV].m_Inspection.LightCalibrationMode(&m_Buffer,
																				&bufCalibResult,
																				bFirst, bRefImg, bAvgImg, bLightCorrection,
																				pCalibData, strErrorMsg);

					OutputDebugLogTo(6, TRUE, "[Light Calibration] Time = %3.1fms", timer.GetElapsedMSec());

					bIsFirstLightCalibration = FALSE;
				}
			}
		}
		
		m_bLightCalibration = FALSE;
		m_pApp->m_bCalib = FALSE;

		CRect rectTemp = CRect(CPoint(0, 0), bufCalibResult.GetImgSize());
		ImgCopy(&bufCalibResult, &rectTemp, &m_Buffer, &rectTemp);
		DrwUpdate(&m_ImageViewInfo);

		if (strErrorMsg == "") {
			strErrorMsg = "Do you want to save Light Calibration Image?";
		}

		if (AfxMessageBox(strErrorMsg, MB_YESNO) == IDYES) {
			CString strLightCorrectionImage;
			CString str[] = { "Top", "Bottom", "Side" };
			strLightCorrectionImage.Format("%s\\%s_Light_Correction.bmp", m_pApp->m_strCalibDir, str[m_pTrack->m_nTrack]);
			SaveGrayScaleBMPFile(strLightCorrectionImage, bufCalibResult);

			
			m_InspectionHandler[nFOV].LoadCalibrationParameters(m_pApp->m_strCalibDir, m_pApp->m_strConfigFile, m_pTrack->m_nTrack, nFOV);
			m_InspectionHandler[nFOV].m_Inspection.ClearLightCorrectionParam(pCalibData);

			int nDevicePerX, nDevicePerY;
			int nFovPerX, nFovPerY;
			m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
			int m_nNoOfFov = nFovPerX * nFovPerY;
			for (int i = 0; i < m_nNoOfFov; i++) {
				m_pTrack->m_pDoc[0]->SaveTeachParmAutoTeach(i);
			}
			AfxMessageBox("Calibrated Image Saved.", IDOK);
		}
		else
			AfxMessageBox("Calibration Abort", IDOK);

		//Go back to previous position
		nError = m_pApp->motionParameters.AbsoluteMove(nAxis[0], nCurrentPos[0], m_pApp->motionParameters.getMovingSpeed(nAxis[0]));
		if (nError) {
			OutputDebugLogTo(0, TRUE, "----- ERROR : Cannot move X motor");
			return nError;
		}

		nError = m_pApp->motionParameters.AbsoluteMove(nAxis[1], nCurrentPos[1], m_pApp->motionParameters.getMovingSpeed(nAxis[1]));
		if (nError) {
			OutputDebugLogTo(0, TRUE, "----- ERROR : Cannot move Y motor");
			return nError;
		}

		nError = m_pApp->motionParameters.WaitForMotionDone(nAxis[0], m_pApp->getMotionTimeOut());
		if (nError) {
			OutputDebugLogTo(9, TRUE, "CMotionControlDialog::Movement[Absolute] Axis%d Error %d", nAxis[0], nError);
		}
		nError = m_pApp->motionParameters.WaitForMotionDone(nAxis[1], m_pApp->getMotionTimeOut());
		if (nError) {
			OutputDebugLogTo(9, TRUE, "CMotionControlDialog::Movement[Absolute] Axis%d Error %d", nAxis[1], nError);
		}

		UpdateStatusBar(&CString("Calibration Done"));

		if(!m_pApp->m_pCalibrationDlg->IsWindowVisible())
			m_pApp->m_pCalibrationDlg->ShowWindow(TRUE);

		m_pApp->m_bLightCalibration = FALSE;
	}

	return 0;
}



void CAppDoc::SaveAutoLightImages()
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	CString strImgName, strPath, strFolder;
	double dC1, dC2, dC3, dC4,dC5,dC6,dC7,dC8;
	
	strPath = m_pApp->m_ImageSaveFolder + "\\Sorted StoredImages";
	_mkdir(strPath);
	strFolder.Format("%s\\AutoLight", strPath);
	_mkdir(strFolder);

	dC1 = m_pApp->m_pLightCtrlDlg->m_nLightPulseChannels[0];
	dC2 = m_pApp->m_pLightCtrlDlg->m_nLightPulseChannels[1];
	dC3 = m_pApp->m_pLightCtrlDlg->m_nLightPulseChannels[2];
	dC4 = m_pApp->m_pLightCtrlDlg->m_nLightPulseChannels[3];
	dC5 = m_pApp->m_pLightCtrlDlg->m_nLightPulseChannels[4];
	dC6 = m_pApp->m_pLightCtrlDlg->m_nLightPulseChannels[5];
	dC7 = m_pApp->m_pLightCtrlDlg->m_nLightPulseChannels[6];
	dC8 = m_pApp->m_pLightCtrlDlg->m_nLightPulseChannels[7];
	strImgName.Format("%02d-%02d-%02d.%03d C1_%.2f C2_%.2f C3_%.2f C4_%.2f C5_%.2f C6_%.2f C7_%.2f C8_%.2f",
						sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds,
						dC1, dC2, dC3, dC4,dC5, dC6, dC7, dC8);

	SaveGrayScaleBMPFile(strFolder+"\\"+strImgName+".bmp", m_Buffer);
}


int CAppDoc::SetDotCalibration()
{
	int nError;
	if (m_bDotCalibration) {
		m_pApp->m_bDotCalibration = TRUE;
		BOOL bIsFound;
		int nErr = 0;
		int nFOV = 0;
		double dGreyValueOut;

		int nPositions[2];
		int nAxis[2];

		DrwDel(&m_ImageViewInfo);
		if (m_pApp->m_pMotionControl) {
			int nXValue, nYValue;
			nXValue = abs(m_pApp->m_nCalibXAxisPosition);
//			m_pApp->m_pMotionControl->m_nMoveTo[0] = -nXValue;
			nPositions[0] = -nXValue;
		
			nYValue = (abs(m_pApp->m_nCalibYAxisPosition));
//			m_pApp->m_pMotionControl->m_nMoveTo[1] = -nYValue;
			nPositions[1] = -nYValue;

			nAxis[0] = X_SERVO_MOTOR;
			nAxis[1] = Y_SERVO_MOTOR;

			if (m_pTrack->m_bCamAvail) {
	//			nError = m_pApp->m_pMotionControl->LinearMovement(TRUE);
				nError = m_pApp->motionParameters.AbsoluteLinearMove(2, nAxis, nPositions, m_pApp->motionParameters.getMovingSpeed(X_SERVO_MOTOR), false);

				nError = m_pApp->motionParameters.WaitForMotionDone(Y_SERVO_MOTOR, m_pApp->getMotionTimeOut());
				if (nError)
					OutputDebugLogTo(0, TRUE, "----- ERROR : Leaner Move for Y motor failed");

				nError = m_pApp->motionParameters.WaitForMotionDone(X_SERVO_MOTOR, m_pApp->getMotionTimeOut());
				if (nError)
					OutputDebugLogTo(0, TRUE, "----- ERROR : Leaner Move for X motor failed");
			}
		}
		CString strMsg = "Do you Want To Load Image";
		if (AfxMessageBox(strMsg, MB_YESNO) == IDYES) {
			CString strImagePath = LoadImage();
			int nError = LoadGrayScaleBMPFile(strImagePath, m_Buffer);
		}
		else {
			OnCameraSnap();
		}

		UpdateStatusBar(&CString("Dot Calibration Processing..."));

		int nDocFrame = !m_pTrack->m_nTrack ? m_nDocIdx : (m_pTrack->m_nTrack == 1 ? m_pApp->m_nTop + m_nDocIdx : m_pApp->m_nTop + m_pApp->m_nBottom + m_nDocIdx);
		ZoomSelectedDocView(nDocFrame, TRUE);

		DrwTrackRect(&m_ImageViewInfo, &m_InspectionHandler[nFOV].m_Inspection.m_CalibrationData.rectDotCalib);
		DrwUpdate(&m_ImageViewInfo);
		if (!WaitResponse(&m_ImageViewInfo))
			m_InspectionHandler[nFOV].m_Inspection.m_CalibrationData.rectDotCalib = GetTrackRect(&m_ImageViewInfo, 0);
		else {
			if (!m_pApp->m_pCalibrationDlg->IsWindowVisible())
				m_pApp->m_pCalibrationDlg->ShowWindow(TRUE);

			ZoomSelectedDocView(nDocFrame, FALSE);

			m_pApp->m_bDotCalibration = FALSE;
			UpdateStatusBar(&CString("Dot Calibration Abort"));
			return -1;
		}

		m_InspectionHandler[nFOV].m_Inspection.DotCalibrationMode(&m_pTrack->m_SharedData, &m_Buffer, m_pTrack->m_nTrack, m_nDocIdx);
		DrwUpdate(&m_ImageViewInfo);

		if (AfxMessageBox("Save Dot Calibration Data?", MB_YESNO) == IDYES) {
			CString str[] = { "T", "B", "S" };
			CString strPath;
			m_pApp->m_strCalibDir.Format("%s\\Calibration", m_pApp->m_strMainCfgDir);
			_mkdir(m_pApp->m_strCalibDir);
			strPath.Format("%s\\DotCalibration_Pos%d_%s%d.bmp", m_pApp->m_strCalibDir, /*m_pTrack->m_nFOVIndex+*/1, str[m_pTrack->m_nTrack], m_nDocIdx + 1);
			SaveGrayScaleBMPFile(strPath, m_Buffer);
			m_InspectionHandler[nFOV].SaveDotCalibrationParameters(m_pApp->m_strCalibDir, m_pApp->m_strConfigFile, m_pTrack->m_nTrack, nFOV);
			m_pTrack->LoadPixelData(m_pApp->m_strCalibDir, m_pApp->m_strConfigFile, m_pTrack->m_nTrack);
			AfxMessageBox("Calibration Data Saved!", IDOK);
		}
		else
			AfxMessageBox("Calibration Abort", IDOK);

		DrwDel(&m_ImageViewInfo);
		UpdateStatusBar(&CString("Dot Calibration Done"));

		bIsFirstLightCalibration = FALSE;
		/*if(nCalibRange==0){
		m_bLightCalibration = FALSE;
		if(m_pApp->m_pLightCtrlDlg && m_pApp->m_bLightCtrlDlgOpen)
		m_pApp->m_pLightCtrlDlg->OnOK();
		OutputDebugLogTo(6,TRUE,"[Light Calibration]sucess",nCalibRange);
		UpdateStatusBar(&CString("[Light Calibration]sucess"));
		m_pApp->m_bCalib = FALSE;
		m_pApp->m_pCalibrationDlg->ShowWindow(TRUE);
		}*/

		ZoomSelectedDocView(nDocFrame, FALSE);

		m_pApp->m_bDotCalibration = FALSE;
	}
	/*if(m_nIterationCalib==100)
	{
	m_nIterationCalib = 0;
	m_bLightCalibration = FALSE;
	if(m_pApp->m_pLightCtrlDlg && m_pApp->m_bLightCtrlDlgOpen)
	m_pApp->m_pLightCtrlDlg->OnCancel();
	UpdateStatusBar(&CString("[Light Calibration]Error"));
	m_pApp->m_bCalib = FALSE;
	m_pApp->m_pCalibrationDlg->ShowWindow(TRUE);
	}*/

	m_bLightCalibration = FALSE;
	m_pApp->m_bCalib = FALSE;

	if (!m_pApp->m_pCalibrationDlg->IsWindowVisible())
		m_pApp->m_pCalibrationDlg->ShowWindow(TRUE);

	return 0;
}



void CAppDoc::OnCameraSnapAll()
{
	m_pApp->SynchronousCameraSnap();
}

void CAppDoc::OnUpdateCameraSnapAll(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	for (int i = 0; i < m_pApp->m_nTotalTracks; i++)
	{
		for (int j = 0; j < m_pApp->m_nTotalViewDocs[i]; j++) {
			if (m_pApp->m_pTracks[i].m_pDoc[j]->m_bLiveImageSingle)
				bEnable = TRUE;
		}
	}
	pCmdUI->Enable(m_pTrack->m_bCamEnable &&
		m_pTrack->m_bCamAvail &&
		!bEnable &&
		!m_pTrack->m_bLiveImage &&
		!m_pApp->m_bOnline &&
		!m_pApp->m_bTeaching &&
		!m_pApp->m_bInspecting &&
		!m_pTrack->m_bContInsp &&
		!m_pApp->m_bTrackInspSeq &&
		(m_pApp->m_UserAccInfo.m_nCurrentAccessLevel >= m_pApp->m_AdvSecurityDlg.m_data.m_nGrabAll));
}



int CAppDoc::InspectQueueBuffers(int nTrack, int nDocTotal , int nFovIndex, int nDieIndex)
{
	CTrainingData* pTrainingDataFirst;
	CInspectinonData* pInspData;
	CCalibrationData* pCalibData;
	int nError = 0;
	CPerformanceTimer timerInspect;
	LOGPEN TextColorRed, TextColorGreen, TextColor, TextColorYellow;
	int nErrInsp[5];
	CStringArray strArrayInspValues;
	strArrayInspValues.RemoveAll();
	BOOL bStepDebug;

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;



	OutputDebugLogTo(8, TRUE, "m_nTrack = %d,nDocTotal = %d,nFovIndex = %d,nDieIndex = %d", nTrack, nDocTotal, nFovIndex, nDieIndex);
	TextColorRed.lopnColor = RGB(230, 50, 50);
	TextColorRed.lopnStyle = PS_SOLID;
	TextColorRed.lopnWidth.x = 1;
	TextColorYellow.lopnColor = RGB(230, 230, 0);
	TextColorYellow.lopnStyle = PS_SOLID;
	TextColorYellow.lopnWidth.x = 1;
	TextColorGreen.lopnColor = RGB(50, 230, 50);
	TextColorGreen.lopnStyle = PS_SOLID;
	TextColorGreen.lopnWidth.x = 1;

	pTrainingDataFirst = &m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[nFovIndex].m_Inspection.m_TrainingData;
	m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[nFovIndex].ClearTeachParam(pTrainingDataFirst);
	pInspData = &m_pApp->m_pTracks[0].m_pDoc[0]->m_InspectionHandler[0].m_Inspection.m_InspectionData;
	pCalibData = &m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[0].m_Inspection.m_CalibrationData;

	if (m_pApp->m_bPixelRuler)
	{
		m_pApp->m_pPixelRulerDlg->ShowWindow(SW_HIDE);
		m_pApp->m_bPixelRuler = FALSE;
	}
	if (m_lDebugFlag & DEBUG_STEP_MODE)
		bStepDebug = 1;

	DrawView(FALSE);
	timerInspect.Reset();
	if (m_bBinaryMode) {
		for (int nDoc = 0; nDoc<m_pApp->m_nTotalViewDocs[nTrack]; nDoc++)
			SwitchImg(m_pApp->m_pTracks[nTrack].m_SharedData.pImageViews[nDoc], FALSE, NULL);
	}

	m_pTrack->m_bOpticsRoi = FALSE;
	if (m_pApp->m_RuntimeDisplayEnable) {
		for (int nDoc = 0; nDoc<m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
			//DrwDel(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);				
			m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_bShowOverlay = TRUE;
		}
	}

	if (!nError) {
		for (int nInsp = 0; nInsp<2; nInsp++) {	//// 0-Device Loc Insp; 1-Rest All Insp [Region, Encap, Die, ...]; ////
			for (int nDoc = 0; nDoc<m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
				//if (!m_pTrack->m_nFOVIndex && !nTrack && !nDoc && !nInsp) {
				//	m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].ClearInspParam(pInspData, FALSE);
				//	m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].ClearCalibParam(pCalibData, FALSE, TRUE);
				//}

				if (m_pApp->m_bFwdDirectionForTiltInsp) {
					if (!nFovIndex && !nTrack && !nDoc && !nInsp) {
						m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].ClearInspParam(pInspData, FALSE);
						m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].ClearCalibParam(pCalibData, FALSE, TRUE);
					}
				}
				else {
					if ((nFovIndex == m_nNoOfFov - 1) && !nTrack && !nDoc && !nInsp) {
						m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].ClearInspParam(pInspData, FALSE);
						m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].ClearCalibParam(pCalibData, FALSE, TRUE);
					}
				}

				if (!nDoc && !nInsp) {
					m_pApp->m_pTracks[/*nTrack*/0].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].ClearInspParam(pInspData, TRUE);
				}

				if ((!nFovIndex || nFovIndex == m_nNoOfFov - 1)) {
					if ((!nTrack && m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFovIndex].m_Inspection.m_TiltInspParm.bEnable && nInsp>0) || //(nDoc==m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[m_pTrack->m_nFOVIndex].m_Inspection.m_TiltInspParm.nImageIndex - 1) && nInsp>0) ||
						(nTrack == m_pApp->m_nTotalTracks - 1 && !nDoc && nInsp>0)) {
						m_pApp->m_pTracks[nTrack].m_pDoc[/*nDoc*/0]->m_InspectionHandler[/*m_pTrack->m_nFOVIndex*/0].GetCalibDataForInsp(pCalibData);
					}
				}

				CPerformanceTimer timer;
				CInspectionHandler* pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFovIndex];
				nErrInsp[nDoc] = pInspHandler->Inspect(&m_pTrack->m_SharedData,
					m_pTrack->m_SharedData.pProcBuffers[nDoc],
					&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_TeachBuffer[nFovIndex],
					m_lDebugFlag,
					!nInsp ? FALSE : TRUE,
					nTrack,
					nDoc,
					m_pApp->m_nTotalViewDocs[nTrack],
					nFovIndex + 1,
					m_nNoOfFov,
					pTrainingDataFirst,
					pInspData,
					pCalibData,
					m_pApp->m_bFwdDirectionForTiltInsp,
					&m_pApp->m_DeepLearningModule,
					strArrayInspValues);

				double dTime = timer.GetElapsedMSec();
				OutputDebugLogTo(nTrack + 1, TRUE, "[Image%d] RESULT: '%s'  (%3.2fms)", nDoc + 1, pInspHandler->m_strErrorMsg, dTime);

//				SetInspectionTime(m_pApp->m_pTracks[nTrack].m_SharedData.pImageViews[nDoc], dTime);
				//DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
			/*	m_pApp->m_strLogInspectionData.RemoveAll();
				m_pApp->m_strLogInspectionData.Copy(strArrayInspValues);
				SetEvent(m_pApp->m_EventLogInspectionData);*/
			}
		}
	}
	else {
		//UpdateStatusBar(&CString("Image Snap Failure"),2);
		//UpdateStatusBar(&CString("Fail Image Snap"),3);
		UpdateStatusBar(&CString("Inspection Error"), 3);
	}
	CString strDefectMsg;
	for (int nDoc = 0; nDoc<m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
		if (m_pApp->m_RuntimeDisplayEnable) {
			if (m_pApp->m_pWndPVIDefectParameters) {
				int nDocFrame = !nTrack ? nDoc : (nTrack == 1 ? m_pApp->m_nTop + nDoc : m_pApp->m_nTop + m_pApp->m_nBottom + nDoc);
				if (m_pApp->m_pWndPVIDefectParameters->IsWindowVisible() && m_pApp->m_pWndPVIDefectParameters->m_bPVIDefectImgEnable[nFovIndex][nDocFrame] /*&& nErrInsp[nDoc]==-PVI_INSP*/) {
					m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFovIndex].m_Inspection.DisplayPVIDefectImage(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo,
						&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,
						m_pApp->m_pWndPVIDefectParameters->m_ComboPviArea.GetCurSel());
				}
			}
			if (nErrInsp[nDoc] < GOOD_UNIT) {
				if (nErrInsp[nDoc] > -ERROR_CODES || nErrInsp[nDoc] == -PROCESS_ERROR) {
					strDefectMsg.Format("%s ", m_pApp->GetFailedStr(nErrInsp[nDoc], FALSE));
					DrwStr(m_pApp->m_pTracks[nTrack].m_SharedData.pImageViews[nDoc], &strDefectMsg, &CPoint(240, 20), &TextColorRed, 80);
				}
				else {
					/*CInspectionHandler* pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFovIndex];
					int nTemp = (abs(nErrInsp[nDoc])) % 100;
					int nAreaTemp = (nTemp / 10) - 1;
					int nAreaDefectTemp = (nTemp % 10) - 1;
					CString strDefect = pInspHandler->m_Inspection.m_pPviInspectionSetup[nAreaTemp].m_pPviDefect[nAreaDefectTemp].strDefectName;
					CString strTemp;
					int nLength = strDefect.GetLength();
					strTemp = strDefect.Mid(strDefect.ReverseFind('_'), nLength);
					strTemp.Format("PVI_%s", strTemp);
					DrwStr(m_pApp->m_pTracks[nTrack].m_SharedData.pImageViews[nDoc], &strTemp, &CPoint(240, 20), &TextColorRed, 80);*/
					CString strTemp("PVI Defect");
					if (m_pApp->m_mapPviDefectsStatsGeneral.find(nErrInsp[nDoc]) != m_pApp->m_mapPviDefectsStatsGeneral.end()) {
						strTemp.Format("PVI_%s", m_pApp->m_mapPviDefectsStatsGeneral[nErrInsp[nDoc]]);
					}
					DrwStr(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo, &strTemp, &CPoint(240, 20), &TextColorRed, 80);
				}

			}
			else {
				strDefectMsg = "PASS";
				DrwStr(m_pApp->m_pTracks[nTrack].m_SharedData.pImageViews[nDoc], &strDefectMsg, &CPoint(240, 20), &TextColorGreen, 80);
			}

			CRect rect = CRect(CPoint(80, 80), CSize(60, 60));
			BOOL bFill = m_nInspCounter % 2;
			if (bFill)
				rect.DeflateRect(6, 6);
			else
				TextColorYellow.lopnWidth.x = 10;
			DrwRect(m_pApp->m_pTracks[nTrack].m_SharedData.pImageViews[nDoc], &rect, &TextColorYellow/*, bFill*/);
			//DrwUpdate(&m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);
		}
		Sleep(m_pApp->m_nInSpectionDelay);
	}

	nError = GOOD_UNIT;
	for (int nDoc = 0; nDoc<m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
		if (m_pApp->m_pWndPVIDefectParameters) {
			if (m_pApp->m_pWndPVIDefectParameters->IsWindowVisible() &&
				m_pApp->m_pWndPVIDefectParameters->m_bPVIDefectImgEnable[nFovIndex][nDoc] //&&
				/*nErrInsp[nDoc] == -PVI_INSP*/)
				continue;
		}
		CInspectionHandler* pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFovIndex];
		for (int j = 0; j<pInspHandler->m_Inspection.m_arrayOverlayInspection.GetCount(); j++) {
			if (!pInspHandler->m_Inspection.m_arrayOverlayInspection[j].hImage.IsInitialized())
				DrawRegion(m_pApp->m_pTracks[nTrack].m_SharedData.pImageViews[nDoc], pInspHandler->m_Inspection.m_arrayOverlayInspection[j]);
		}
		if (nError == GOOD_UNIT) {
			if (nErrInsp[nDoc] < GOOD_UNIT)
				nError = nErrInsp[nDoc];
		}
		//nErrInsp[nDoc] = -11111;

//		int nView = !nTrack ? nDoc : (nTrack == 1 ? m_pApp->m_nTop + nDoc : m_pApp->m_nTop + m_pApp->m_nBottom + nDoc);
		int nView;
		theApp.motionParameters.GetViewID(nTrack, nDoc, nView);
		if (nErrInsp[nDoc] == -NO_IMAGE)
			m_pApp->m_nErrorCode[nDieIndex][nFovIndex][nView] = NOT_INSPECTED;
		else
			m_pApp->m_nErrorCode[nDieIndex][nFovIndex][nView] = nErrInsp[nDoc];

		if (!m_pApp->m_bOnline || m_pApp->m_bDisplayAllStatsOffline) {
			m_pApp->UpdateLotSummary(nDieIndex, nFovIndex, nView, nErrInsp[nDoc], !m_pApp->m_bOnline);
		}
	}

	if (!m_pApp->m_bOnline || m_pApp->m_bDisplayAllStatsOffline)
		m_pApp->UpdateStatisticsWindow();
	DrawView(TRUE);

	return nError;
}

int CAppDoc::SaveTeachParmAutoTeach(int nFOV)
{

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;

	m_strConfigDir.Format("%s\\Doc%d", m_pTrack->m_strConfigDir, m_nDocIdx + 1);
	//m_strConfigDir = m_pTrack->m_strConfigDir;

	//if(m_pTrack->m_nTrack==0 && m_nDocIdx==0) {
	_mkdir(m_strConfigDir);
	//OutputDebugLogTo(8, TRUE, "Doc Config Folder: %s", m_strConfigDir);
	CCalibrationData* pCalibData;

	m_pTrack->m_nFOVIndex = nFOV;

	CInspectionHandler* pInspHandler = &m_pApp->m_pTracks[m_pTrack->m_nTrack].m_pDoc[m_nDocIdx]->m_InspectionHandler[nFOV];

	/*pCalibData = &m_pApp->m_pTracks[0].m_pDoc[0]->m_InspectionHandler[0].m_Inspection.m_CalibrationData;*/
	pCalibData = &m_pApp->m_pTracks[m_pTrack->m_nTrack].m_pDoc[0]->m_InspectionHandler[0].m_Inspection.m_CalibrationData;


	pInspHandler->ClearTeachParam(NULL);
	pInspHandler->LoadTeachParameters(m_strConfigDir, m_pApp->m_strConfigFile, nFOV);

	//m_InspectionHandler[nFOV].LoadRecipe(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
	//										&m_Buffer,
	//										m_strConfigDir,
	//										m_pApp->m_strConfigFile,
	//										m_pTrack->m_nTrack,
	//										m_pTrack->m_nTotalDocs,
	//										nFOV, m_nDocIdx);

	//Get total FOV for current station
	int nCurrentFOV = m_nNoOfFov;

	int nError, nTeachStep = 2;//// 1-Device Loc & Dattam; 2-Region & Mask;
	CString strImgFullPath;
	strImgFullPath.Format("%s\\TeachTemplate_Pos%d.bmp", m_strConfigDir, nFOV + 1);
	nError = LoadGrayScaleBMPFile(strImgFullPath, m_TeachBuffer[nFOV]);
	if (!nError) {
		for (int i = 0; i < nTeachStep; i++) {
			pInspHandler->AutoTeach(&m_pTrack->m_SharedData,//&m_ImageViewInfo,
				&m_TeachBuffer[nFOV],
				m_strConfigDir, m_pApp->m_strConfigFile,
				!i ? FALSE : TRUE,
				m_pTrack->m_nTrack,
				m_nDocIdx,
				nFOV + 1,
				/*m_pApp->m_nNoOfFov*/nCurrentFOV,
				pCalibData);
		}
	}
	else {
		OutputDebugLogTo(1, TRUE, "AutoTeach Load Image Error [Teach Image Not Avaiable]");
		OutputDebugLogTo(9, TRUE, "CAppDoc::LoadConfiguration AutoTeach Image Error [Track%d FOV%d Doc%d]",
			m_pTrack->m_nTrack + 1, nFOV + 1, m_nDocIdx + 1);
		CString str;
		str.Format("Teach Image Not Avaiable for Track%d Doc%d Position %d", m_pTrack->m_nTrack + 1, m_nDocIdx + 1, nFOV + 1);
		m_pApp->AddLogEvent(str);

	}

	m_lDebugFlag = m_lDebugFlag | DEBUG_DRAW | DEBUG_PRINT | DEBUG_TIME;

	DrawView(FALSE);
	DrawView();

	CString str;
	str.Format("AutoTeach Process Completed");
	m_pApp->AddLogEvent(str);

	return 0;

}

const CInspectionHandler & CAppDoc::inspection(unsigned char id) const
{
	return m_InspectionHandler[id];
}

CInspectionHandler & CAppDoc::inspection(unsigned char id)
{
	return m_InspectionHandler[id];
}
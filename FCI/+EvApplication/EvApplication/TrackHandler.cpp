/// TrackHandler.cpp: implementation of the CTrackHandler class.
/// Udupa; April'2012

#include "stdafx.h"

#include "Application.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include "StatisticsView.h"
#include "CameraCode.h"
#include "MainFrame.h"
#include <direct.h>
#include "IO_Interface.h"
#include "CameraSetupDialog.h"
#include "ImageSelectionDlg.h"
#include "MotionControlDialog.h"
#include "SequenceThread.h"

CTrackHandler::CTrackHandler()
{
	for(int nDoc=0; nDoc<MAX_DOCS; nDoc++)
		m_pDoc[nDoc] = NULL;

	m_nTotalDocs = 2;

	m_bLiveImage = FALSE;
	m_bLiveCalibration = FALSE;
	m_bContInsp = FALSE;
	m_bTrackContInsp = FALSE;
	m_bCamEnable = FALSE;
	m_bCamAvail = FALSE;
	m_bCamExtTrigEnable = FALSE;
	m_bCamTrigAvail = FALSE;
	m_bAutoRun = FALSE;
	m_nAutoRunDelay = 150;

	m_nDefImgCount = 0;

	m_nCommand = 0;

	m_pActiveDoc = NULL;
	m_pActiveUpdateDoc = NULL;
	m_bCamSetupDlgOpen = FALSE;

	m_nExposure = 10;
	m_nGain[0] = 100;
	m_nGain[1] = 10;
	m_nGain[2] = 10;
	m_nBlackLevel = 0;

	m_rectAoi = CRect(0, 0, 2048, 2048);
	m_pCamSetupDlg = NULL;

	m_bViewOpticsPanel = FALSE;
	m_nOpticsRecalcMode = 0;
	m_bOpticsRoi = FALSE;

	for(int i=0; i<8; i++)
		m_rectOptics[i] = CRect(CPoint(10+10*i, 10+10*i), CSize(100, 100));
	m_nOpticsFrames = 1;
	m_nOpticsAvgSize = 10;
	m_nOpticsTopOffset = 10;
	m_nOpticsBottomOffset= 10;
	m_charTempPosNameForInsp = '0';
	m_nFOVIndex = 0;
	m_nDieIndex = 0;
	m_pActiveDocTeach = NULL;

	m_dResolutionX = 5.5;
	m_dResolutionY = 5.5;
	InterlockedExchange(&m_nInspectionThreadId, 0);

	InitializeCriticalSection(&CriticalSectionScannedImage);
}

CTrackHandler::~CTrackHandler()
{
	if(m_pCamSetupDlg != NULL)
		delete m_pCamSetupDlg;	

	//if (m_hTrackUpdateImage) {
	//	m_pActiveUpdateDoc = NULL;
	//	SetEvent(m_hTrackEventUpdateImage);
	//	if (WaitForSingleObject(m_hTrackUpdateImage, 500) != WAIT_OBJECT_0)
	//		TerminateThread(m_hTrackUpdateImage, 0);
	//	m_hTrackUpdateImage = NULL;
	//	CloseHandle(m_hTrackEventUpdateImage);
	//}

	if(m_hTrackCommandThread) {
		m_nCommand = KILL_CMD_THREAD;
		m_pActiveDoc = NULL;
		SetEvent(m_EventRun);
		if(WaitForSingleObject(m_hTrackCommandThread, 500)!=WAIT_OBJECT_0)
			TerminateThread(m_hTrackCommandThread, 0);
		CloseHandle(m_EventRun);
		m_hTrackCommandThread = NULL;
	}
	/*if(m_hTrackUpdateThread) {
		m_nUpdateCommand = KILL_CMD_THREAD;
		m_pActiveUpdateDoc = NULL;
		SetEvent(m_EventUpdate);
		if(WaitForSingleObject(m_hTrackUpdateThread, 500)!=WAIT_OBJECT_0)
			TerminateThread(m_hTrackUpdateThread, 0);
		CloseHandle(m_EventUpdate);
		m_hTrackUpdateThread = NULL;
	}*/

	

	if(m_pActiveDocTeach != NULL)
		m_pActiveDocTeach = NULL;

	FreeSimulationBuffers();

	m_nLightControllerId.RemoveAll();
	//m_nLightStartSegmentNo.RemoveAll();
	AryStrNotInspectedFileName.RemoveAll();
	AryStrNotInspectedFileNameOnly.RemoveAll();
	AryStrNotInspectedFileNameOrg.RemoveAll();
	AryStrNotInspectedFileNameOnlyOrg.RemoveAll();
	ArynNotInspectedFileNameIndex.RemoveAll();
	ArynInspectedFileNameIndex.RemoveAll();

	//m_nStartSegmentsOfImage.RemoveAll();
	//for(int i=0; i<MAX_TRACKS; i++) {
	//	m_nSegmentsOfImage[i].RemoveAll();
	//	m_nSelectedSegmentsOfImage[i].RemoveAll();
	//}
	std::for_each(eventDoInspection.begin(), eventDoInspection.end(),
		[](HANDLE & handle) { CloseHandle(handle); });

	std::for_each(eventInspectionDone.begin(), eventInspectionDone.end(),
		[](HANDLE & handle) { CloseHandle(handle); });

	DeleteCriticalSection(&CriticalSectionScannedImage);
}

BOOL CTrackHandler::SetParameters(int nTrack)
{
	CString strSec, strTemp;

	m_pApp = (CApplication*) AfxGetApp();
	m_nTrack = nTrack;

	m_strStoredImagesPath = "void";
	strTemp.Format("Folder: StoredImagesT%02d", nTrack+1);
	m_strStoredImagesPath = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", strTemp, m_strStoredImagesPath);
	if (m_strStoredImagesPath.CompareNoCase("void")==0) {
		m_strStoredImagesPath = "C:\\";
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", strTemp, m_strStoredImagesPath);
	}

	m_strFGCard = "void";
	strSec.Format("Library: Track%d", nTrack+1);
	m_strFGCard = GetRegistryEntry(READ_REGPATH + "Camera", strSec, m_strFGCard);
	if (m_strFGCard.CompareNoCase("void")==0) {
		m_strFGCard = "C_PylonUsb";
		PutRegistryEntry(WRITE_REGPATH + "Camera", strSec, m_strFGCard);
	}
	if(m_strFGCard.CompareNoCase("none")!=0)
		m_strFGCard+=".dll";

	// Configure Camera File
	m_strCameraFile = "void";
	strSec.Format("Camera Id: Track%d", nTrack+1);
	m_strCameraFile = GetRegistryEntry(READ_REGPATH + "Camera", strSec, m_strCameraFile);
	if (m_strCameraFile.CompareNoCase("void")==0) {
		m_strCameraFile = "";
		PutRegistryEntry(WRITE_REGPATH + "Camera", strSec, m_strCameraFile);
	}

	strSec.Format("Max Exposure: Track%d", nTrack+1);
	m_nMaxExposure =  GetRegistryEntry(READ_REGPATH + "Camera", strSec, -1);
	if(m_nMaxExposure==-1) {
		m_nMaxExposure = 5000;
		PutRegistryEntry(WRITE_REGPATH + "Camera", strSec, m_nMaxExposure);
	}

	m_strFGCardFile = "void";
	//strSec.Format("CARDFile_Track%d FG", nTrack);
	//m_strFGCardFile = GetRegistryEntry(READ_REGPATH + "Hardware", strSec, m_strFGCardFile);
	//if (m_strFGCardFile.CompareNoCase("void")==0) {
	//	m_strFGCardFile = nTrack==0?"VCamDuoCamA.mpf":"VCamDuoCamB.mpf";
	//	PutRegistryEntry(WRITE_REGPATH + "Hardware", strSec, m_strFGCardFile);
	//}

	m_strLoadStaticImgFolderPath = "void";
	strTemp.Format("Folder: StaticImagesT%02d", nTrack+1);
	m_strLoadStaticImgFolderPath = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", strTemp, m_strLoadStaticImgFolderPath);
	if (m_strLoadStaticImgFolderPath.CompareNoCase("void")==0) {
		m_strLoadStaticImgFolderPath = "C:\\";
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", strTemp, m_strLoadStaticImgFolderPath);
	}

	return TRUE;
}

void CTrackHandler::InitializeDocView(CAppDoc** pDoc)
{
	m_bCamAvail = (m_FGResource.pCamModule != NULL) ? true : false;
	m_bCamEnable = m_bCamAvail; 
	for(int nDoc=0; nDoc<m_nTotalDocs; nDoc++) {
		m_pDoc[nDoc] = pDoc[nDoc];

		m_pDoc[nDoc]->SetParameters(this, nDoc);
		m_pDoc[nDoc]->InitializeDocView();
		/*m_pApp->m_SharedData[m_nTrack].Initialize(nDoc,
									&m_pDoc[nDoc]->m_Buffer,
									&m_pDoc[nDoc]->m_ImageViewInfo, 
									NULL,
									m_nTrack);*/
		m_SharedData.Initialize(nDoc,
								&m_pDoc[nDoc]->m_Buffer,
								&m_pDoc[nDoc]->m_ImageViewInfo, 
								NULL,
								m_nTrack);
	}

	///LoadOpticsSettings();

	//m_nSelectedSegmentsOfImage.resize(m_nTotalDocs);
	//m_nSegmentsOfImage.resize(m_nTotalDocs);

	m_EventGrabEnd = CreateEventA(NULL, FALSE, FALSE, NULL);

	m_EventRun = CreateEventA(NULL, FALSE, FALSE, NULL); 
	m_EventKillLive = CreateEventA(NULL, FALSE, FALSE, NULL);
	m_EventKillLiveCalibration = CreateEventA(NULL, FALSE, FALSE, NULL);
	m_EventKillInspCycle = CreateEventA(NULL, FALSE, FALSE, NULL);
	m_EventKillAutoRun = CreateEventA(NULL, FALSE, FALSE, NULL);

	m_EventKillInspectImages = CreateEventA(NULL, FALSE, FALSE, NULL);
	m_hInspectImagesKilled = CreateEventA(NULL, FALSE, FALSE, NULL);

	m_EventUpdate = CreateEventA(NULL, FALSE, FALSE, NULL);

	m_hTrackEventUpdateImage = CreateEventA(NULL, FALSE, FALSE, NULL);
	//m_hTrackUpdateImage = AfxBeginThread(TrackUpdateImage, this, THREAD_PRIORITY_LOWEST);

	//m_hTrackCommandThread = m_pApp->m_ThreadManager.CreateThread(TrackCommandThread, this, TRACK_THREAD, m_pApp->m_bInspectionAffinity ? m_nTrack : -1);
	if(!m_nTrack)
		m_EventPositionEnd = CreateEventA(NULL, FALSE, FALSE, NULL);

	//CWinThread* pUpdateThread = AfxBeginThread(TrackUpdateThread, this, THREAD_PRIORITY_NORMAL);
	//m_hTrackUpdateThread = pUpdateThread->m_hThread;

	if (m_nTrack == STATION_TOP) {
		grabbedImageId.resize(4);
		eventDoInspection.resize(4);
		eventInspectionDone.resize(4);
		grabbedDeviceId.resize(4);
	}

	if (m_nTrack == STATION_BOTTOM) {
		grabbedImageId.resize(4);
		eventDoInspection.resize(4);
		eventInspectionDone.resize(4);
		grabbedDeviceId.resize(4);
	}

	std::for_each(eventDoInspection.begin(), eventDoInspection.end(),
		[](HANDLE & handle) { handle = CreateEventA(NULL, TRUE, FALSE, NULL); });

	std::for_each(eventInspectionDone.begin(), eventInspectionDone.end(),
		[](HANDLE & handle) { handle = CreateEventA(NULL, FALSE, FALSE, NULL); });

#ifdef _DEBUG
	CWinThread* pThread = AfxBeginThread(TrackCommandThread, this, THREAD_PRIORITY_NORMAL);
#else
	CWinThread* pThread = AfxBeginThread(TrackCommandThread, this, THREAD_PRIORITY_TIME_CRITICAL);
#endif
	m_hTrackCommandThread = pThread->m_hThread;

	SetThreadAffinityMask(m_hTrackCommandThread, 0x2 << m_nTrack);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void CTrackHandler::UpdateWatchDog(int nData) 
{
	m_nUpdateCommand = UPDATE_WATCHDOG;
	m_pActiveUpdateDoc = m_pDoc[0];
	SetEvent(m_EventUpdate);
}

void CTrackHandler::UpdateLotChangedAck() 
{
	m_nUpdateCommand = UPDATE_LOTCHANGEDACK;
	m_pActiveUpdateDoc = m_pDoc[0];
	SetEvent(m_EventUpdate);
}

int CTrackHandler::EnqueueAsyncGrab()
{
	if(!m_bCamAvail) {
		OutputDebugLogTo(3+m_nTrack, TRUE, "[Track%d] EnqueueAsyncGrab: Camera not available", m_nTrack+1);
		return -1;
	}
	if(!m_FGResource.lpEnqueueAsyncGrab)
		return -1;

	//ResetEvent(m_eventGrabEnd);
//	RefreshLight();
	m_FGResource.lpEnqueueAsyncGrab(m_FGResource.pCamModule);
	
	OutputDebugLogTo(3+m_nTrack, TRUE, "[Track%d] TrackHandler:: EnqueueAsyncGrab done", m_nTrack+1);

	return 0;
}

int CTrackHandler::WaitForEndOfExposure()
{
	if(!m_bCamAvail) {
		OutputDebugLogTo(3+m_nTrack, TRUE, "[Track%d] WaitForEndOfExposure: Camera not available", m_nTrack+1);
		return -1;
	}

	CPerformanceTimer timer;
	timer.Reset();
	if(m_FGResource.lpWaitForExposure(m_FGResource.pCamModule, NULL)) {
		OutputDebugLogTo(3+m_nTrack, TRUE, "[Track%d] === Wait for End of Exposure Error {%5.1fms}", m_nTrack+1, timer.GetElapsedMSec());
		return -1;
	}

	//Sleep(__max(0, (20-(DWORD)floor(timer.GetElapsedMSec()))));

	return 0;
}

int CTrackHandler::CancelSnap()
{
	if(!m_bCamAvail) {
		OutputDebugLogTo(3+m_nTrack, TRUE, "[Track%d] CancelSnap: Camera not available", m_nTrack+1);
		return -1;
	}

	if(m_FGResource.lpCancelPendingGrab)
		m_FGResource.lpCancelPendingGrab(m_FGResource.pCamModule);

	return 0;
}

int CTrackHandler::LoadConfigurationServer()
{
	m_strConfigDir.Format("%s\\Cam%d", m_pApp->m_strConfigDir, m_nTrack + 1);
	_mkdir(m_strConfigDir);
	for (int nDoc = 0; nDoc < m_nTotalDocs; nDoc++){
		m_pDoc[nDoc]->LoadConfigurationServer();
	}
	return 0;
}


int CTrackHandler::LoadConfiguration()
{
	m_strConfigDir.Format("%s\\Cam%d", m_pApp->m_strConfigDir, m_nTrack+1);
	_mkdir(m_strConfigDir);

	LoadCameraParm();
	//m_pApp->m_LightHandler.LoadLightSettings(m_nTrack);
	for(int nDoc=0; nDoc<m_nTotalDocs; nDoc++)
	{
		m_pDoc[nDoc]->LoadConfiguration();
		//m_pApp->m_LightHandler.LoadLightSettings(nDoc, m_nTrack);

	}
	LoadPixelData(m_pApp->m_strCalibDir, m_pApp->m_strConfigFile, m_nTrack);

	return 0;
}

int CTrackHandler::LoadConfiguration(int nFov)
{
	m_strConfigDir.Format("%s\\Cam%d", m_pApp->m_strConfigDir, m_nTrack + 1);
	_mkdir(m_strConfigDir);

	LoadCameraParm();
	//m_pApp->m_LightHandler.LoadLightSettings(m_nTrack);
	for (int nDoc = 0; nDoc < m_nTotalDocs; nDoc++)
	{
		m_pDoc[nDoc]->LoadConfiguration(nFov);
		//m_pApp->m_LightHandler.LoadLightSettings(nDoc, m_nTrack);

	}
	LoadPixelData(m_pApp->m_strCalibDir, m_pApp->m_strConfigFile, m_nTrack);

	return 0;
}

int CTrackHandler::LoadCameraParm()
{
	CIniFile iniFile;
	CString str, strFile;
	strFile.Format("%s\\Camera.cam", m_strConfigDir);
	iniFile.SetPath(strFile);
	
	if (iniFile.ReadFile()) {
		m_rectAoi = iniFile.GetCRect("Camera Setting", "rectAoi", m_rectAoi);
		m_nExposure = iniFile.GetInteger("Camera Setting", "Exposure", m_nExposure);
		if(m_nExposure > m_nMaxExposure)
			m_nExposure = m_nMaxExposure;
		for(int i=0; i<3; i++) {
			str.Format("Gain%d", i+1);
			m_nGain[i] = iniFile.GetInteger("Camera Setting", str, m_nGain[i]);
		}
		m_nBlackLevel = iniFile.GetInteger("Camera Setting", "BlackLevel", m_nBlackLevel);
	}
	else {
		iniFile.SetPath(strFile);
		iniFile.SetCRect("Camera Setting", "rectAoi", m_rectAoi);
		iniFile.SetInteger("Camera Setting", "Exposure", m_nExposure);
		for(int i=0; i<3; i++) {
			str.Format("Gain%d", i+1);
			iniFile.SetInteger("Camera Setting", str, m_nGain[i]);
		}
		iniFile.SetInteger("Camera Setting", "BlackLevel", m_nBlackLevel);

		iniFile.WriteFile();
	}
	if(m_bCamAvail) {
		SetCameraValue(CAMERA_EXPOSURE, m_nExposure);
		for(int i=0; i<3; i++)
			SetCameraValue(CAMERA_GAIN, m_nGain[i], i);
		SetCameraValue(CAMERA_BLACKLEVEL, m_nBlackLevel);

		SetCameraAoi(&m_FGResource, m_rectAoi);
		OutputDebugLog("TrackHandler->LoadCameraParm->SetCamAOI");
		CRect rectAoi;
		GetCameraAoi(&m_FGResource, &rectAoi);
		OutputDebugLog("TrackHandler->LoadCameraParm->GetCamAOI");

		if(m_rectAoi != rectAoi) {
			m_rectAoi = rectAoi;

			iniFile.SetPath(strFile);
			iniFile.ReadFile();
			iniFile.SetCRect("Camera Setting", "rectAoi", rectAoi);
			iniFile.WriteFile();
		}
	}

//	if (m_nTrack == m_pApp->m_nTotalTracks-1) {
		for (int nDoc = 0; nDoc < m_nTotalDocs; nDoc++)
			m_pDoc[nDoc]->ResizeDoc(m_rectAoi.Size());
//	}

	return 0;
}

void CTrackHandler::RefreshLight(int nDoc)
{
	for(int i=0; i<m_nLightControllerId.GetCount(); i++)
		m_pApp->m_LightHandler.RefreshLight(m_nLightControllerId[i], nDoc, m_nTrack);
}

int CTrackHandler::SetCameraValue(int nFeature, int nValue, int nParm)
{
	if(nFeature == CAMERA_EXPOSURE) {
		SetCameraExposure(&m_FGResource, nValue);
		m_nExposure = nValue;
	}
	else if(nFeature == CAMERA_GAIN) {
		SetCameraGain(&m_FGResource, nValue, nParm);
		m_nGain[nParm] = nValue;
	}
	else if(nFeature == CAMERA_BLACKLEVEL) {
		SetCameraBlackLevel(&m_FGResource, nValue);
		m_nBlackLevel = nValue;
	}

	return 0;
}

int CTrackHandler::GetCameraValue(int nFeature, int& nValue, int nParm)
{
	if(nFeature == CAMERA_EXPOSURE)
		GetCameraExposure(&m_FGResource, &nValue);
	else if(nFeature == CAMERA_GAIN)
		GetCameraGain(&m_FGResource, &nValue, nParm);
	else if(nFeature == CAMERA_BLACKLEVEL)
		GetCameraBlackLevel(&m_FGResource, &nValue);

	return 0;
}

void CTrackHandler::SaveCameraParm() 
{
	CIniFile iniFile;
	CString strFile;
	strFile.Format("%s\\Camera.cam", m_strConfigDir);
	CString strMsg;
	iniFile.SetPath(strFile);
	iniFile.SetCRect("Camera Setting", "rectAoi", m_rectAoi);
	iniFile.SetInteger("Camera Setting", "Exposure", m_nExposure);
	CString str;
	for(int i=0; i<3; i++) {
		str.Format("Gain%d", i+1);
		iniFile.SetInteger("Camera Setting", str, m_nGain[i]);
	}
	iniFile.SetInteger("Camera Setting", "BlackLevel", m_nBlackLevel);

	iniFile.WriteFile();
}

void CTrackHandler::SetCamActive(BOOL bActive)
{
	for(int i=0; i<m_nTotalDocs; i++)
		m_pDoc[i]->m_bCameraActive = bActive;
}

void CTrackHandler::CopyRecipe(CString strRecipeSrcMode,CString strRecipeDesMode,CString strRecipeSrc, CString strRecipeDst)
{
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	theApp.motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);

	CString strTrackSrc, strTrackDst, strDocSrc, strDocDst, strFileSrc, strFileDst, strSrc, strDst;

	strTrackSrc.Format("%s\\%s\\%s\\Cam%d", m_pApp->m_strMainCfgDir,strRecipeSrcMode, strRecipeSrc, m_nTrack+1);
	strTrackDst.Format("%s\\%s\\%s\\Cam%d", m_pApp->m_strMainCfgDir,strRecipeDesMode, strRecipeDst, m_nTrack+1);
	_mkdir(strTrackDst);

	strFileSrc.Format("%s\\Camera.cam", strTrackSrc);
	strFileDst.Format("%s\\Camera.cam", strTrackDst);
	CopyFileA(strFileSrc, strFileDst, FALSE);

	strSrc.Format("%s\\%s.ltv", strTrackSrc, strRecipeSrc);
	strDst.Format("%s\\%s.ltv", strTrackDst, strRecipeDst);
	CopyFileA(strSrc, strDst, FALSE);

	/*strSrc.Format("%s\\Dot_%s.calib", strTrackSrc, strRecipeSrc);
	strDst.Format("%s\\Dot_%s.calib", strTrackDst, strRecipeDst);
	CopyFileA(strSrc, strDst, FALSE);*/

	HANDLE hFile;
	int nCount = 0;
	WIN32_FIND_DATA FindFileData;
	if((hFile=FindFirstFile(strTrackSrc+"\\*.", &FindFileData))!=INVALID_HANDLE_VALUE) {
		do {
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				CString str = FindFileData.cFileName;
				if(str.Find(".")==-1/* && str.CompareNoCase("_Common")!=0*/) {
					nCount++;
				}
			}
		} while (FindNextFile(hFile, &FindFileData));
	}
	FindClose(hFile);

	for(int nDoc=0; nDoc<nCount; nDoc++) {
		/*strFileSrc.Format("%s\\DotCalibration_Pos%d_%d.bmp", strTrackSrc, 1, nDoc+1);
		strFileDst.Format("%s\\DotCalibration_Pos%d_%d.bmp", strTrackDst, 1, nDoc + 1);
		CopyFileA(strFileSrc, strFileDst, FALSE);*/

		//strDocSrc = strTrackSrc;
		//strDocDst = strTrackDst;
		strDocSrc.Format("%s\\Doc%d", strTrackSrc, nDoc+1);
		strDocDst.Format("%s\\Doc%d", strTrackDst, nDoc+1);
		_mkdir(strDocDst);

		strFileSrc.Format("%s\\%s.dat", strDocSrc, strRecipeSrc);
		strFileDst.Format("%s\\%s.dat", strDocDst, strRecipeDst);
		CopyFileA(strFileSrc, strFileDst, FALSE);
////
		strFileSrc.Format("%s\\%s.cll", strDocSrc, strRecipeSrc);
		strFileDst.Format("%s\\%s.cll", strDocDst, strRecipeDst);
		CopyFileA(strFileSrc, strFileDst, FALSE);

		/*strFileSrc.Format("%s\\%s.calib", strDocSrc, strRecipeSrc);
		strFileDst.Format("%s\\%s.calib", strDocDst, strRecipeDst);
		CopyFileA(strFileSrc, strFileDst, FALSE);

		strFileSrc.Format("%s\\CalibImagePos.bmp", strDocSrc);
		strFileDst.Format("%s\\CalibImagePos.bmp", strDocDst);
		CopyFileA(strFileSrc, strFileDst, FALSE);

		strFileSrc.Format("%s\\CalibImageNeg.bmp", strDocSrc);
		strFileDst.Format("%s\\CalibImageNeg.bmp", strDocDst);
		CopyFileA(strFileSrc, strFileDst, FALSE);*/

		for(int i=0;i<nFovPerX * nFovPerY;i++){
			strFileSrc.Format("%s\\%s_%d.tch", strDocSrc, strRecipeSrc,i+1);
			strFileDst.Format("%s\\%s_%d.tch", strDocDst, strRecipeDst,i+1);
			CopyFileA(strFileSrc, strFileDst, FALSE);
   
			strFileSrc.Format("%s\\%s_%d.insp", strDocSrc, strRecipeSrc,i+1);
			strFileDst.Format("%s\\%s_%d.insp", strDocDst, strRecipeDst,i+1);
			CopyFileA(strFileSrc, strFileDst, FALSE);

			strFileSrc.Format("%s\\TeachTemplate_Pos%d.bmp",strDocSrc,i+1);
			strFileDst.Format("%s\\TeachTemplate_Pos%d.bmp",strDocDst,i+1);
			CopyFileA(strFileSrc, strFileDst, FALSE);

			strFileSrc.Format("%s\\PVIDefectParameters_%d.pvi", strDocSrc,i+1);
			strFileDst.Format("%s\\PVIDefectParameters_%d.pvi", strDocDst,i+1);
			CopyFileA(strFileSrc, strFileDst, FALSE);
		}
	}
}

void CTrackHandler::ComGrabInspect(int nDoc) 
{
	//CString strMsg;
	//strMsg.Format("");
	//UpdateStatusBar(&strMsg,2);
	//UpdateStatusBar(&strMsg,3);

	m_nCommand = COM_GRABINSP;
	m_pActiveDoc = m_pDoc[nDoc];
	SetEvent(m_EventRun);
}

void CTrackHandler::ComGrabInspectOnlineSimulation(int nDoc) 
{
	CString strMsg;

	strMsg.Format("");
	UpdateStatusBar(&strMsg,2);
	UpdateStatusBar(&strMsg,3);

	m_nCommand = COM_ONLINE_SIMULATION;
	m_pActiveDoc = m_pDoc[nDoc];
	SetEvent(m_EventRun);
}

void CTrackHandler::ComGrab(int nDoc) 
{
	CString strMsg;

	strMsg.Format("");
	UpdateStatusBar(&strMsg,2);
	UpdateStatusBar(&strMsg,3);

	m_nCommand = COM_GRAB;
	m_pActiveDoc = m_pDoc[nDoc];
	SetEvent(m_EventRun);
}

void CTrackHandler::ZoomAllViews(int nMode)
{
	for(int nDoc=0; nDoc<m_nTotalDocs; nDoc++) {
		if(m_pDoc[nDoc])
			m_pDoc[nDoc]->ZoomView(nMode);
	}
}

void CTrackHandler::UpdateLotDefectsList()
{
	////for(int nDoc=0; nDoc<m_nTotalDocs; nDoc++) {
	//	if(m_pDoc[0]) {
	//		CInspection   *pInspection;
	//		TRACKSUMMARY  *pDispSummary;

	//		pDispSummary = &m_pApp->m_LotSummary.m_DisplaySummary[m_nDieIndex][m_nFOVIndex][m_nTrack];
	//		pInspection  = &m_pDoc[0]->m_InspectionHandler[m_nFOVIndex].m_Inspection;

	//		/*pDispSummary->bLensLocEnable				 = TRUE;
	//		pDispSummary->bFilpLensEnable				 = pInspection->m_FilpLensParam.bEnable;
	//		pDispSummary->bMultipleLensEnable			 = pInspection->m_MultipleLensParm.bEnable;
	//		pDispSummary->bLensCrackEnable				 = pInspection->m_LensCrackParm.bEnable;
	//		pDispSummary->bTornEdgeEnable			     = pInspection->m_TornEdgeParam.bEnable;
	//		pDispSummary->bEdgeRoughnessEnable			 = pInspection->m_EdgeRoughnessParam.bEnable;
	//		pDispSummary->bEdgeTearEnable				 = pInspection->m_EdgeTearParm.bEnable;
	//		pDispSummary->bEdgeFiberDefectEnable			 = pInspection->m_EdgeFiberParm.bEnable;
	//		pDispSummary->bForeignParticleEnable		 = pInspection->m_ForeignParam.bEnable; 
	//		pDispSummary->bSurfaceScartchEnable			 = pInspection->m_SurfaceScratchParm.bEnable;
	//		pDispSummary->bMoldFlashEnable				 = pInspection->m_MoldFlashParm.bEnable;
	//		pDispSummary->bSurfaceDefectEnable			 = pInspection->m_SurfaceDefectParm.bEnable;
	//		pDispSummary->bBubbleDefectEnable			 = pInspection->m_BubbleDefectParam.bEnable;
	//		pDispSummary->bDarkSpotEnable				 = pInspection->m_DarkSpotClearParm.bEnable;
	//		pDispSummary->bPrintShiftEnable				 = pInspection->m_PrintShiftParm.bEnable;
	//		pDispSummary->bMissingPrintEnable            = pInspection->m_MissingPrintParm.bEnable;
	//		pDispSummary->bEdgeDefect					 = pInspection->m_EdgeDefectParm.bEnable;
	//		pDispSummary->bHoleDefect					 = pInspection->m_HolesDefectParm.bEnable;
	//		pDispSummary->bFiberDefectEnable			 = pInspection->m_FiberParm.bEnable;*/

	//		/*m_pApp->m_bStepSelectionEnable[THREAD_MULTIPLE_LENS] = pDispSummary->bMultipleLensEnable;
	//		m_pApp->m_bStepSelectionEnable[THREAD_LENS_CRACK] = pDispSummary->bLensCrackEnable;
	//		m_pApp->m_bStepSelectionEnable[THREAD_TORN_EDGE] = pDispSummary->bTornEdgeEnable;
	//		m_pApp->m_bStepSelectionEnable[THREAD_EDGE_ROUGHNESS] = pDispSummary->bEdgeRoughnessEnable;
	//		m_pApp->m_bStepSelectionEnable[THREAD_EDGE_TEAR] = pDispSummary->bEdgeTearEnable;
	//		m_pApp->m_bStepSelectionEnable[THREAD_EDGE_FIBER] = pDispSummary->bEdgeFiberDefectEnable;
	//		m_pApp->m_bStepSelectionEnable[THREAD_FOREIGN_PARTICLE] = pDispSummary->bForeignParticleEnable;
	//		m_pApp->m_bStepSelectionEnable[THREAD_SURFACE_SCRATCH] = pDispSummary->bSurfaceScartchEnable;
	//		m_pApp->m_bStepSelectionEnable[THREAD_MOLD_FLASH] = pDispSummary->bMoldFlashEnable;
	//		m_pApp->m_bStepSelectionEnable[THREAD_SURFACE_DEFECT] = pDispSummary->bSurfaceDefectEnable;
	//		m_pApp->m_bStepSelectionEnable[THREAD_BUBBLE] = pDispSummary->bBubbleDefectEnable;
	//		m_pApp->m_bStepSelectionEnable[THREAD_DARK_SPOT_CLEAR] = pDispSummary->bDarkSpotEnable;
	//		m_pApp->m_bStepSelectionEnable[THREAD_PRINT_SHIFT] = pDispSummary->bPrintShiftEnable;
	//		m_pApp->m_bStepSelectionEnable[THREAD_MISSING_PRINT] = pDispSummary->bMissingPrintEnable;
	//		m_pApp->m_bStepSelectionEnable[THREAD_EDGE_DEFECT] = pDispSummary->bEdgeDefect;
	//		m_pApp->m_bStepSelectionEnable[THREAD_HOLE_DEFECT] = pDispSummary->bHoleDefect;
	//		m_pApp->m_bStepSelectionEnable[THREAD_FIBER] = pDispSummary->bFiberDefectEnable;*/
	//	

	//	}
	////}

	////if(m_pApp->m_pStatsView && ::IsWindow(m_pApp->m_pStatsView->m_ctrlDefectsList.m_hWnd))
	//	//m_pApp->m_pStatsView->RefreshDefectsList();
	//m_pApp->RefreshDefectsList();
}

void CTrackHandler::LoadOpticsSettings()
{
	CIniFile iniFile;

	CString strFile;
	strFile.Format("%s\\Camera.opt", m_strConfigDir);
	iniFile.SetPath(strFile);

	CString strSec;
	if (iniFile.ReadFile()) {
		for(int i=0; i<8; i++) {
			strSec.Format("rectOptics%d", i);
			m_rectOptics[i] = iniFile.GetCRect("OpticsSetting", strSec, CRect(CPoint(10+10*i, 10+10*i), CSize(100, 100)));
			if(CheckRect(&m_rectOptics[i], &m_pDoc[0]->m_Buffer.GetImgSize())==-1 || m_rectOptics[i].Width()<3 || m_rectOptics[i].Height()<3)
				m_rectOptics[i] = CRect(CPoint(10+10*i, 10+10*i), CSize(100, 100));
			m_nOpticsFrames = iniFile.GetInteger("OpticsSetting", "nFrames", 1);
			m_nOpticsAvgSize = iniFile.GetInteger("OpticsSetting", "nAvgSize", 10);
			m_nOpticsTopOffset = iniFile.GetInteger("OpticsSetting", "nTopOffset", 20);
			m_nOpticsBottomOffset = iniFile.GetInteger("OpticsSetting", "nBottomOffset", 20);
		}
	}
	else {
		OutputDebugLog("%s not found. Default values assumed", strFile);
		//InitLightVal(nSet);
		iniFile.ReadFile();
		SetOpticsVal(&iniFile);
		iniFile.WriteFile();
	}
}

void CTrackHandler::SaveOpticsSettings()
{
	CIniFile iniFile;
	CString strFile;
	strFile.Format("%s\\Camera.opt", m_strConfigDir);
	iniFile.SetPath(strFile);

	iniFile.ReadFile();
	SetOpticsVal(&iniFile);
	iniFile.WriteFile();
}

void CTrackHandler::SetOpticsVal(CIniFile *ptrIniFile)
{
	CString strName, strSec;
	for(int i=0; i<8; i++) {
		strSec.Format("rectOptics%d", i);
		ptrIniFile->SetCRect("OpticsSetting", strSec, m_rectOptics[i]);
		ptrIniFile->SetInteger("OpticsSetting", "nFrames", m_nOpticsFrames);
		ptrIniFile->SetInteger("OpticsSetting", "nAvgSize", m_nOpticsAvgSize);
		ptrIniFile->SetInteger("OpticsSetting", "nTopOffset", m_nOpticsTopOffset);
		ptrIniFile->SetInteger("OpticsSetting", "nBottomOffset", m_nOpticsBottomOffset);
	}
}

int CTrackHandler::LoadingImagesOnline(int nTrack, int nIndex, CString *strImage, bool *bLoadComplete)
{
	CString strImagePath;
	CApplication* pApp = (CApplication*) AfxGetApp();

	
	
	int k = (int) AryStrNotInspectedFileName.GetSize();
	if(	nIndex <  AryStrNotInspectedFileName.GetSize()){	
		strImagePath = AryStrNotInspectedFileName.GetAt(nIndex);
		*strImage = strImagePath;
		OutputDebugLogTo(m_nTrack+1, TRUE, "###Image File Name : %s" , AryStrNotInspectedFileNameOnlyOrg.GetAt(nIndex));
	}
	else {
		if(!pApp->m_bContinuousLoop)
			*bLoadComplete = TRUE;
	}
	if(pApp->m_bContinuousLoop)
	{
		if((nIndex != 0) && (nIndex == AryStrNotInspectedFileName.GetSize())) {
			m_nImageCount = 0;
			bLoadComplete = FALSE;
		}
	}
	if(ArynInspectedFileNameIndex.GetSize() == AryStrNotInspectedFileNameOnlyOrg.GetSize())
		return -1;
	ArynNotInspectedFileNameIndex.RemoveAll();
	

	return 0;
}

int CTrackHandler::BrowseThruImagesForSimul(int nTrack)
{
	CFileFind FileFind;
	//BOOL bFound;
	int nError;
	BOOL bNextFile;
	CString strImageFileName, strImagePath, str, strTemp, strExt;
	CString strDestPath;
	//char strDst[256];
	CString strImageFileNameTemp;
	CString str0,strTrack;

	CApplication* pApp = (CApplication*) AfxGetApp();

	AryStrNotInspectedFileName.RemoveAll();
	AryStrNotInspectedFileNameOnly.RemoveAll();
	AryStrNotInspectedFileNameOrg.RemoveAll();
	AryStrNotInspectedFileNameOnlyOrg.RemoveAll();
	ArynNotInspectedFileNameIndex.RemoveAll();
	ArynInspectedFileNameIndex.RemoveAll();

	nError = 0;
	strTemp =  "*.*";
	strTrack.Format("Track %d",nTrack+1);
	strImageFileNameTemp = pApp->m_strSimulationImagesPath+ "\\"+ strTrack + "\\";
	str = strImageFileNameTemp;
	str = str + strTemp;
	bNextFile = FileFind.FindFile(str);
	strExt = "bmp";	
	while(bNextFile) {
		if (m_pDoc[0]->m_lDebugFlag&DEBUG_PRINT)
			OutputDebugLog("====================================================");

		bNextFile = FileFind.FindNextFile();

		strImagePath = FileFind.GetFilePath();
		str = strImagePath.Right(3);
		if(str.CompareNoCase(strExt))
			continue;

		strImageFileName= CString(FileFind.GetFileName());
		AryStrNotInspectedFileNameOrg.Add(strImagePath);
		AryStrNotInspectedFileNameOnlyOrg.Add(strImageFileName);
		AryStrNotInspectedFileName.Add(strImagePath);
		AryStrNotInspectedFileNameOnly.Add(strImageFileName);
	}

	for(int i=0;i<AryStrNotInspectedFileName.GetSize();i++)
	{
		ArynNotInspectedFileNameIndex.Add(i);
	}	
	return 0;
}


int CTrackHandler::LoadSimulationBuffers()
{
	FreeSimulationBuffers();

	CFileFind FileFind;
	CString strTrack, strImagePath;
	CImageBuffer* pBuf;
	strTrack.Format("Track%d", m_nTrack+1);
	BOOL bNextFile = FileFind.FindFile(m_pApp->m_strSimulationImagesPath + "\\" + strTrack + "\\*.*");
	int nCount = 0;
	while(bNextFile) {
		bNextFile = FileFind.FindNextFile();
		strImagePath = FileFind.GetFilePath();
		if(strImagePath.Right(3).CompareNoCase("bmp"))
			continue;

		pBuf = new CImageBuffer();

		LoadGrayScaleBMPFile(strImagePath, *pBuf);
		m_arrSimulationBuffers.Add(pBuf);
		m_arrSimulationFilenames.Add(FileFind.GetFileName());

		if(++nCount > m_pApp->m_nMaxSimulationBuffers)
			break;
	}
	m_nSimulationBufferIndex = 0;

	return 0;
}

int CTrackHandler::FreeSimulationBuffers()
{
	int nSize = (int) m_arrSimulationBuffers.GetSize();
	for(int i=0; i<nSize; i++)
		delete m_arrSimulationBuffers.GetAt(i);
	m_arrSimulationBuffers.RemoveAll();

	m_nSimulationBufferIndex = -1;

	return 0;
}

int CTrackHandler::GetNextSimulationBuffer(int nDocIndex)
{
	int nIndex = m_nSimulationBufferIndex % ((int) m_arrSimulationBuffers.GetSize());

	CImageBuffer* pBuf = m_arrSimulationBuffers.GetAt(nIndex);

	m_pDoc[nDocIndex]->CopyImageAutoFit(pBuf);

	///Udupa: TODO
	///m_pDoc[nDocIndex]->SetTitleImage(&m_arrSimulationFilenames.GetAt(nIndex));
	m_nSimulationBufferIndex = nIndex + 1;

	return 0;
}

int CTrackHandler::GetFailedImage(int nIndex, CFailImageStack*& pFailure)
{
	if(nIndex < 0 || nIndex >= 25 || !m_pApp->m_bAllocFailedImageBuffer)
		return -1;

	pFailure = &m_pDoc[0]->m_FailStack[nIndex];
	
	return 0;
}

int CTrackHandler::WaitForSnapCompletion(BOOL bDisplay, int nDoc, BOOL bCopyToDocBuffer)
{
	CApplication* pApp = (CApplication*) AfxGetApp();

	if(!m_bCamAvail) {
		OutputDebugLogTo(3+m_nTrack, TRUE, "[Track%d] WaitForSnapCompletion: Camera not available", m_nTrack+1);
		return -1;
	}

	CPerformanceTimer timer;
	timer.Reset();
	CImageBuffer* pGrabbedBuffer;
	if(m_FGResource.lpWaitForCompletion(m_FGResource.pCamModule, &pGrabbedBuffer)) {
		OutputDebugLogTo(3+m_nTrack, TRUE, "[Track%d] === Wait for Snap Completion Error {%5.1fms}", m_nTrack+1, timer.GetElapsedMSec());
		return -1;
	}
	OutputDebugLogTo(3+m_nTrack, TRUE, "[Track%d] === Wait for Snap Completion Complete {%5.1fms}", m_nTrack+1, timer.GetElapsedMSec());

	m_pDoc[nDoc]->CopyGrabbedBuffer(pGrabbedBuffer);
	
	if(bDisplay)
		DrwUpdate(&m_pDoc[nDoc]->m_ImageViewInfo);

	//Sleep(__max(0, (20-(DWORD)floor(timer.GetElapsedMSec()))));
	//m_nTrack == 0 ? pApp->m_pImageSelectionDlg->CopyBrightImages() : m_nTrack == 1 ? pApp->m_pImageSelectionDlg->CopyDarkImages() : pApp->m_pImageSelectionDlg->CopySingleSpotImages() ;


	return 0;
}

int CTrackHandler::ResetLightSteps()
{
	return m_pApp->m_LightHandler.ResetLightSteps(/*m_nFOVIndex*/);
}

void CTrackHandler::UpdateGrabImage(int nDoc) 
{
	m_pActiveUpdateDoc = m_pDoc[nDoc];
	//SetEvent(m_hTrackEventUpdateImage);
	//WaitForSnapCompletion(!m_pApp->m_bOnline, nDoc, TRUE);
	WaitForSnapCompletion(TRUE, nDoc, TRUE);
}


void CTrackHandler::SaveConfigurationAs(CString strOldFile, CString strNewFile)
{
	CString strTemp, strFileSrc, strFileDst, strDocCfgDir,strCamSrc,strCamDst;

	/*strTemp.Format("\\Track%d", m_nTrack+1);
	m_strConfigDir = m_pApp->m_strConfigDir + strTemp;
	_mkdir(m_strConfigDir);*/

	for (int i =0 ; i < NO_OF_POSITION; i++){
		m_strConfigDirNew[i].Format("%s\\%s\\Track%d", m_pApp->m_strMainCfgDir, strOldFile, m_nTrack+1);
		_mkdir(m_strConfigDirNew[i]);

	//	strCamSrc.Format("%s\\Camera%d.cam", m_strConfigDir[i], m_nTrack+1);
		m_strConfigDirNew[i].Format("%s\\%s\\Track%d", m_pApp->m_strMainCfgDir, strNewFile, m_nTrack+1);
		_mkdir(m_strConfigDirNew[i]);
	//	strCamDst.Format("%s\\Camera%d.cam", m_strConfigDir[i], m_nTrack+1);
		
		strFileSrc.Format("%s\\%s\\Track%d\\%s.opt", m_pApp->m_strMainCfgDir, strOldFile, m_nTrack+1, strOldFile);    
		strFileDst.Format("%s\\%s\\Track%d\\%s.opt", m_pApp->m_strMainCfgDir, strNewFile, m_nTrack+1, strNewFile);    
		CopyFileA(strFileSrc, strFileDst , FALSE);

		/*strCamSrc = m_pApp->m_strMainCfgDir + "\\" + 
					m_pApp->m_strConfigFile +
					strTemp + "\\" + "Camera.cam";
		strCamDst = m_strConfigDir + "\\" + "Camera.cam";*/

//		CopyFileA(strCamSrc, strCamDst , TRUE);         
		
		for(int nDoc=0; nDoc<m_nTotalDocs; nDoc++) {

			strTemp.Format("\\Doc%d", m_pDoc[nDoc]->m_nDocIdx+1);
			strDocCfgDir = m_strConfigDir[i] + strTemp;
			_mkdir(strDocCfgDir);

			strFileSrc = m_pDoc[nDoc]->m_strConfigDir[i] + "\\" + m_pApp->m_strConfigFile;
			strFileDst = strDocCfgDir + "\\" + strNewFile;
	
			CopyFileA(strFileSrc+".cll", strFileDst+".cll", FALSE);
			CopyFileA(strFileSrc+".tch", strFileDst+".tch", FALSE);
			CopyFileA(strFileSrc+".insp", strFileDst+".insp", FALSE);
//			CopyFileA(strLightSrc+".ltv", strLightDst+".ltv", TRUE);         
			/*CopyFileA(m_pDoc[nDoc]->m_strConfigDir[i] + "\\" + "ContactLensTeachTmpl.bmp", strDocCfgDir + "\\" + "ContactLensTeachTmpl.bmp", TRUE);
			CopyFileA(m_pDoc[nDoc]->m_strConfigDir[i] + "\\" + "LightHeadPositionImage.bmp", strDocCfgDir + "\\" + "LightHeadPositionImage.bmp", TRUE);
			CopyFileA(m_pDoc[nDoc]->m_strConfigDir[i] + "\\" + "NoSalineTeachImage.bmp", strDocCfgDir + "\\" + "NoSalineTeachImage.bmp", TRUE);*/

			m_pDoc[nDoc]->m_strConfigDir = strDocCfgDir;
		}
	}

}

void CTrackHandler::StepDebugNavigate(int nStep)
{
	if(m_pDoc[0]->m_bWaiting && !m_pDoc[0]->m_pTrack->m_bAutoRun) {
		if(nStep==(int)VK_ESCAPE) {
			m_pDoc[0]->OnInspectionAbort();
		}
		else {
			m_pDoc[0]->OnInspectionNext();
		}
	}
}

int CTrackHandler::NavigateStaticImage(int nKey)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;

	if(!pApp->m_bOnline && !pApp->m_bTeaching && !pApp->m_bTrackInspSeq && 
		!pApp->m_bInspecting && !m_bLiveImage && 
		(pApp->m_UserAccInfo.m_nCurrentAccessLevel >= pApp->m_AdvSecurityDlg.m_data.m_nLoadImg)) {
		
			BOOL bSelChange = FALSE;
			CString strPath, strName, strFullPath, strTemp;
			int nPos, nPosTemp = -1;

			strPath = m_strLoadStaticImgFolderPath.Mid(0, m_strLoadStaticImgFolderPath.ReverseFind('\\'));
			strName = m_strLoadStaticImgFolderPath.Mid(m_strLoadStaticImgFolderPath.ReverseFind('\\')+1);
			nPosTemp = strName.Find(LPCSTR("Pos"));

			if((nKey == (int)VK_LEFT || nKey == (int)VK_RIGHT) && nPosTemp > 0) {
				CString strPosOld, strPosNew;
				BOOL bKeyLeft = (nKey == (int)VK_LEFT);
				strTemp = strName.Mid(nPosTemp+3, 1);
				nPos = atoi(strTemp);
				strPosOld.Format("Pos%d", nPos);
				if(bKeyLeft) {
					if(nPos > 1) {
						nPos -= 1;
						bSelChange = TRUE;
					}
				}
				else {
					if(nPos < m_nNoOfFov) {
						nPos += 1;
						bSelChange = TRUE;
					}
				}
				strPosNew.Format("Pos%d", nPos);
				strName.Replace(strPosOld, strPosNew);
				if(bSelChange) {
					strFullPath.Format("%s\\%s", strPath, strName);
					m_pDoc[0]->LoadImagesToDoc(strFullPath);
					m_pDoc[0]->SaveStaticImagePath(strFullPath);
					
					m_nFOVIndex = nPos-1;
					CString* strTemp = new CString;
					strTemp->Format("%d%d", m_pActiveDocTeach->m_nDocIdx, m_nFOVIndex);
					if(m_pActiveDocTeach != NULL)
						m_pApp->m_pMainWnd->SendMessageA(WM_SET_CUR_POS_PARAM_DISP, (WPARAM)m_nTrack, (LPARAM)strTemp);
					delete strTemp;
				}
			}
		}
	return 0;
}

void CTrackHandler::SetZPositionForAutoFocusingSequence(int nDoc)
{
	if (nDoc < 0 && nDoc >= m_pApp->m_pTracks[m_nTrack].m_nTotalDocs)
		return;

	int nZValue = m_pApp->m_pTracks[m_nTrack].m_pDoc[nDoc]->m_nZAxisPosition[m_nFOVIndex];
	OutputDebugLogTo(8, TRUE, "SetZPositionForSequence Fov %d Track %d, Doc %d, Z Value=%d", m_nFOVIndex, m_nTrack, nDoc, nZValue);

	int nMotorAxiz = (m_nTrack == STATION_TOP) ? Z_SERVO_TOP_MOTOR : Z_SERVO_BOTTOM_MOTOR;
	double dMotorResolution = 0;
	theApp.motionParameters.GetMotorResolution(nMotorAxiz, dMotorResolution);

	std::map<int, double> mapZFocus;
	CRect rectAutoFocusRoi = m_pDoc[nDoc]->m_InspectionHandler[0].m_Inspection.m_TrainingData.m_rectAutoFocusROI;
	int nCurrentZPos = nZValue;


	//Display Auto Focusing ROI
	LOGPEN TextColorRed;
	TextColorRed.lopnColor = RGB(230, 50, 50);
	TextColorRed.lopnStyle = PS_SOLID;
	TextColorRed.lopnWidth.x = 1;
	DrwRect(&m_pDoc[nDoc]->m_ImageViewInfo, &rectAutoFocusRoi, &TextColorRed);

	CPoint pt1(rectAutoFocusRoi.left + rectAutoFocusRoi.Width() / 2, rectAutoFocusRoi.top);
	CPoint pt2(rectAutoFocusRoi.left + rectAutoFocusRoi.Width() / 2, rectAutoFocusRoi.bottom);
	DrwLine(&m_pDoc[nDoc]->m_ImageViewInfo, &pt1, &pt2, &TextColorRed);
	
	pt1 = CPoint(rectAutoFocusRoi.left, rectAutoFocusRoi.top + rectAutoFocusRoi.Height()/2);
	pt2 = CPoint(rectAutoFocusRoi.right, rectAutoFocusRoi.top + rectAutoFocusRoi.Height() / 2);
	DrwLine(&m_pDoc[nDoc]->m_ImageViewInfo, &pt1, &pt2, &TextColorRed);

	DrwUpdate(&m_pDoc[nDoc]->m_ImageViewInfo);

	//Get Focusing Value of current position
	int nError = m_pApp->motionParameters.AbsoluteMove(nMotorAxiz, nCurrentZPos, m_pApp->motionParameters.getMovingSpeed(nMotorAxiz));
	if (nError) {
		OutputDebugLogTo(8, TRUE, "Failed to move motor axis %d Error %d", nMotorAxiz);
	}

	nError = m_pApp->motionParameters.WaitForMotionDone(nMotorAxiz, m_pApp->getMotionTimeOut());
	if (nError) {
		OutputDebugLogTo(8, TRUE, "CMotionControlDialog::Movement[Absolute] Axis%d Error %d", nMotorAxiz, nError);
	}

	RefreshLight(nDoc);
	m_nOpticsRecalcMode = 1;
	nError = EnqueueAsyncGrab();

	if (!nError) {
		UpdateGrabImage(nDoc);

		double dMean, dNoise, dFocus;
		double dAvgFocus = 0;
		OpticsCharacteristics(&m_pDoc[nDoc]->m_Buffer, &rectAutoFocusRoi, dMean, dNoise, dFocus);
		
		OutputDebugLogTo(8, TRUE, "Auto Focusing - Z: %d, Focus: %.5f", nCurrentZPos, dFocus);
		mapZFocus.insert(std::pair<int, double>(nCurrentZPos, dFocus));
	}
	
	int nAutoFocusStepNo = 10;
	double dAutoFocusStepSizeMM = 0.05;

	//1. Move Up
	for (int nStep = 0; nStep < nAutoFocusStepNo; nStep++) {
		int nNextZPos = nCurrentZPos + (nStep + 1)*dAutoFocusStepSizeMM * dMotorResolution;

		//Validate Z Position before moving (without warning message box)
		if (theApp.motionParameters.IsSoftwareLimits(nMotorAxiz, nNextZPos)) {
			OutputDebugLogTo(8, TRUE, "Auto Focusing - Z Position %d is reach soft limit", nNextZPos);
			break;
		}

		nError = theApp.motionParameters.AbsoluteMove(nMotorAxiz, nNextZPos, theApp.motionParameters.getMovingSpeed(nMotorAxiz));
		if (nError) {
			OutputDebugLogTo(8, TRUE, "Auto Focusing - Failed to move motor axis %d Error %d", nMotorAxiz);
			break;
		}

		nError = theApp.motionParameters.WaitForMotionDone(nMotorAxiz, theApp.getMotionTimeOut());
		if (nError) {
			OutputDebugLogTo(8, TRUE, "Auto Focusing - CMotionControlDialog::Movement[Absolute] Axis%d Error %d", nMotorAxiz, nError);
			break;
		}

		//Get Focusing Value
		m_nOpticsRecalcMode = 1;
		nError = EnqueueAsyncGrab();
		if (!nError) {
			UpdateGrabImage(nDoc);

			double dMean, dNoise, dFocus;
			double dAvgFocus = 0;
			OpticsCharacteristics(&m_pDoc[nDoc]->m_Buffer, &rectAutoFocusRoi, dMean, dNoise, dFocus);
			OutputDebugLogTo(8, TRUE, "Auto Focusing - Z: %d, Focus: %.5f", nNextZPos, dFocus);
			mapZFocus.insert(std::pair<int, double>(nNextZPos, dFocus));
		}
	}

	//2. Move down
	for (int nStep = 0; nStep < nAutoFocusStepNo; nStep++) {
		int nNextZPos = nCurrentZPos - (nStep + 1)*dAutoFocusStepSizeMM * dMotorResolution;

		//Validate Z Position before moving (without warning message box)
		if (theApp.motionParameters.IsSoftwareLimits(nMotorAxiz, nNextZPos)) {
			OutputDebugLogTo(8, TRUE, "Auto Focusing - Z Position %d is reach soft limit", nNextZPos);
			break;
		}

		nError = theApp.motionParameters.AbsoluteMove(nMotorAxiz, nNextZPos, theApp.motionParameters.getMovingSpeed(nMotorAxiz));
		if (nError) {
			OutputDebugLogTo(8, TRUE, "Auto Focusing - Failed to move motor axis %d Error %d", nMotorAxiz);
			break;
		}

		nError = theApp.motionParameters.WaitForMotionDone(nMotorAxiz, theApp.getMotionTimeOut());
		if (nError) {
			OutputDebugLogTo(8, TRUE, "Auto Focusing - CMotionControlDialog::Movement[Absolute] Axis%d Error %d", nMotorAxiz, nError);
			break;
		}

		//Get Focusing Value
		m_nOpticsRecalcMode = 1;
		nError = EnqueueAsyncGrab();
		if (!nError) {
			UpdateGrabImage(nDoc);

			double dMean, dNoise, dFocus;
			double dAvgFocus = 0;
			OpticsCharacteristics(&m_pDoc[nDoc]->m_Buffer, &rectAutoFocusRoi, dMean, dNoise, dFocus);
			OutputDebugLogTo(8, TRUE, "Auto Focusing - Z: %d, Focus: %.5f", nNextZPos, dFocus);
			mapZFocus.insert(std::pair<int, double>(nNextZPos, dFocus));
		}
	}

	//3.Find the best Z focus value
	double dBestFocus = 0;
	int nBestZFocus = 0;
	for (std::map<int, double>::iterator it = mapZFocus.begin(); it != mapZFocus.end(); it++) {
		if (it->second > dBestFocus) {
			dBestFocus = it->second;
			nBestZFocus = it->first;
		}
	}

	nError = theApp.motionParameters.AbsoluteMove(nMotorAxiz, nBestZFocus, theApp.motionParameters.getMovingSpeed(nMotorAxiz));
	if (nError) {
		OutputDebugLogTo(8, TRUE, "Auto Focusing - Failed to move motor axis %d Error %d", nMotorAxiz);
	}
	nError = theApp.motionParameters.WaitForMotionDone(nMotorAxiz, theApp.getMotionTimeOut());
	if (nError) {
		OutputDebugLogTo(8, TRUE, "Auto Focusing - CMotionControlDialog::Movement[Absolute] Axis%d Error %d", nMotorAxiz, nError);
	}

	DrwDel(&m_pDoc[nDoc]->m_ImageViewInfo);
	DrwUpdate(&m_pDoc[nDoc]->m_ImageViewInfo);
}

void CTrackHandler::SetZPositionForSequence(int nDoc)
{
	if (nDoc < 0 && nDoc >= m_pApp->m_pTracks[m_nTrack].m_nTotalDocs)
		return;

	int nValue, nCommand, nError;

	if (!m_pApp->m_bLightCalibration && !m_pApp->m_bDotCalibration)
		nValue = m_pApp->m_pTracks[m_nTrack].m_pDoc[nDoc]->m_nZAxisPosition[m_nFOVIndex];
	else
		nValue = (m_nTrack == 1) ? m_pApp->m_nCalibZAxisPositionBottom : m_pApp->m_nCalibZAxisPositionTop;

	OutputDebugLogTo(9, TRUE, "SetZPositionForSequence Fov %d Track %d, Doc %d, Z Value=%d", m_nFOVIndex, m_nTrack, nDoc, nValue);

	int nMotorAxiz = (m_nTrack == STATION_TOP) ? Z_SERVO_TOP_MOTOR : Z_SERVO_BOTTOM_MOTOR;

	nError = m_pApp->motionParameters.AbsoluteMove(nMotorAxiz, nValue, m_pApp->motionParameters.getMovingSpeed(nMotorAxiz));
	if (nError) {
		OutputDebugLogTo(7, TRUE, "Failed to move motor axis %d Error %d", nMotorAxiz);
	}

	nError = m_pApp->motionParameters.WaitForMotionDone(nMotorAxiz, m_pApp->getMotionTimeOut());
	if (nError) {
		OutputDebugLogTo(9, TRUE, "CMotionControlDialog::Movement[Absolute] Axis%d Error %d", nMotorAxiz, nError);
	}
}

void CTrackHandler::LoadPixelData(CString strRecipeFolder, CString strRecipeFile, int nTrack)
{
	CIniFile iniFile;
	CString strCalibFileName, strSec;
	CString str[] = { "Top", "Bottom", "Side" };

	//strCalibFileName.Format("%s\\%s_%d.calib", strRecipeFolder, strRecipeFile, nFOV + 1);
	strCalibFileName.Format("%s\\Dot_%s.calib", strRecipeFolder, str[nTrack]);
	iniFile.SetPath(strCalibFileName);

	if (iniFile.ReadFile()) {
		strSec.Format("Dot Calibration Data");

		m_dResolutionX = iniFile.GetDouble(strSec, "Dot ResolutionAlongXInMPerP", m_dResolutionX);
		m_dResolutionY = iniFile.GetDouble(strSec, "Dot ResolutionAlongYInMPerP", m_dResolutionY);
	}

	return;
}

long CTrackHandler::getInspectionThreadId()
{
	return InterlockedIncrement(&m_nInspectionThreadId) - 1;
}

long CTrackHandler::getInspectionThreadCount()
{
	return InterlockedExchangeAdd(&m_nInspectionThreadId, 0);
}
#include "StdAfx.h"
#include "Hardware.h"
#include "Application.h"
#include "TrackHandler.h"
#include "AppView.h"
#include "AppDoc.h"
#include <direct.h>
#include <algorithm>
#include "StatisticsView.h"
#include "ScanBarcode.h"
#include "CameraCode.h"
#include "ImageSelectionDlg.h"
#include "MainFrame.h"
#include "StationMapping.h"
#include "MotionControlDialog.h"
#include "ImageQueueHandler.h"
#include "CentralizedCommInterface.h"
#include "CentralizedCommManager.h"

int Grab(CAppDoc *pDoc, BOOL bDisplay) 
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	CTrackHandler* pTrack = pDoc->m_pTrack;

	if(pTrack->m_bCamEnable) {
		CPerformanceTimer timer;
		if(pDoc->m_pApp->m_bLightCtrlDlgOpen)
			pTrack->RefreshLight(pDoc->m_nDocIdx);
		CImageBuffer* pGrabbedBuffer;
		if(pTrack->m_FGResource.lpImageGrab(pTrack->m_FGResource.pCamModule, &pGrabbedBuffer))
			return -1;
		pDoc->CopyGrabbedBuffer(pGrabbedBuffer);

	///if(pDoc->m_lDebugFlag&DEBUG_TIME_EXT||pDoc->m_lDebugFlag&DEBUG_TIME)
		OutputDebugLogTo(2+pTrack->m_nTrack/5, TRUE, "[Doc%d] Total Snap Time: %5.1fms", pDoc->m_nDocIdx+1, timer.GetElapsedMSec());
		if(bDisplay)
			pDoc->m_ImageViewInfo.pImageView->RedrawWindow();
	}
	else {
		OutputDebugLogTo(2+pTrack->m_nTrack/5, TRUE, "[Doc%d] Snap not available", pDoc->m_nDocIdx+1);
		//CImageBuffer buf;
		//CString str;
		//str.Format("C:\\Users\\JY\\Desktop\\NewBio\\Test\\a (%d).bmp", pDoc->m_nDocIdx+1);
		//LoadGrayScaleBMPFile(str, buf);
		//pDoc->CopyGrabbedBuffer(&buf);

		//if(bDisplay)
		//	pDoc->m_ImageViewInfo.pImageView->RedrawWindow();
	}
	
	return 0;
}

UINT TrackLiveAll(LPVOID pParam)
{
	CApplication* pApp = (CApplication*) pParam;
	int nError;

	while(WaitForSingleObject(pApp->m_pTracks[0].m_EventKillLive, 100) == WAIT_TIMEOUT) {
		
		if(!pApp->m_bStream) {
			pApp->m_bStream = TRUE;
			pApp->m_bScanning = TRUE;

			nError = pApp->InspectionScanSequenceOffline();
			
			if(nError) {
				OutputDebugLogTo(9,TRUE,"TrackLiveAll For InspectionScanSequenceOffline Error");
				pApp->m_bInspSequenceOffline = !pApp->m_bInspSequenceOffline;
				pApp->m_bScanning = FALSE;
				break;
			}
			pApp->m_bScanning = FALSE;
		}
	}

	for(int nTrack=0; nTrack<pApp->m_nTotalTracks; nTrack++)
		pApp->m_pTracks[nTrack].m_bLiveImage = FALSE;
	pApp->m_bStream = FALSE;

	return 0;
}

UINT TrackLiveImage(LPVOID pParam)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	int nError=-1;
	CAppDoc* pDoc = (CAppDoc*)pParam;
	CTrackHandler* pTrack = pDoc->m_pTrack;

	while(WaitForSingleObject(pDoc->m_pTrack->m_EventKillLive, 30) == WAIT_TIMEOUT) {
		//nError = pDoc->CameraGrab();
		if(!pApp->m_bSnap) {
			pApp->m_bSnap = TRUE;
			nError = pDoc->CameraGrabUsingExposure(TRUE, FALSE, FALSE, TRUE);

			if(nError=0) {
				pDoc->m_pTrack->m_bLiveImage = FALSE;
				break;
			}
		}
	}
	pTrack->m_bLiveImage = FALSE;
	
	return 0;
}

UINT TrackLiveImageSingle(LPVOID pParam)
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	int nError = -1;
	CAppDoc* pDoc = (CAppDoc*)pParam;
	CTrackHandler* pTrack = pDoc->m_pTrack;

	while (WaitForSingleObject(pDoc->m_EventKillLiveSingle, 30) == WAIT_TIMEOUT) {
		if (!pApp->m_bSnap) {
			pApp->m_bSnap = TRUE;
			nError = pDoc->CameraGrabSingle();

			if (nError = 0) {
				pDoc->m_bLiveImageSingle = FALSE;
				break;
			}
		}
	}

	pDoc->m_bLiveImageSingle = FALSE;
	if (pApp->m_bSnap)
		pApp->m_bSnap = FALSE;

	return 0;
}

UINT TrackLightCalibrationLiveImage(LPVOID pParam)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	int nError=-1;
	CPerformanceTimer m_Time,m_Time1;
	CAppDoc* pDoc = (CAppDoc*)pParam;
	int nDoc = pDoc->m_nDocIdx;
	CTrackHandler* pTrack = pDoc->m_pTrack;
	OutputDebugLogTo(7,TRUE,"TrackLightCalibrationLiveImage Started");
	while(WaitForSingleObject(pDoc->m_pTrack->m_EventKillLiveCalibration, 500) == WAIT_TIMEOUT) {
		//m_bCalib
		//nError = pDoc->CameraGrab();
		OutputDebugLogTo(7,TRUE,"TrackLightCalibrationLiveImage inside 1 bCalib = %d",pApp->m_bCalib);
		if(!pApp->m_bCalib) {
			pApp->m_bCalib = TRUE;
			OutputDebugLogTo(7,TRUE,"TrackLightCalibrationLiveImage inside 2 bCalib = %d",pApp->m_bCalib);
			nError = pDoc->CameraGrab();	
			OutputDebugLogTo(7,TRUE,"TrackLightCalibrationLiveImage inside 3 bCalib = %d",pApp->m_bCalib);
			if(nError) {
				pDoc->m_pTrack->m_bLiveCalibration = FALSE;
				//
				break;
			}
		}
	}
	pTrack->m_bLiveCalibration = FALSE;
	
	return 0;
}


UINT DocInspectCycle(LPVOID pParam)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	CAppDoc* pDoc = (CAppDoc*) pParam;

	int nTrackIndex = -1;
	if(GetKeyState(VK_SHIFT) & 0x8000) {
	//	pApp->m_LotSummary.InitDisplaySummary();
		nTrackIndex = pDoc->m_pTrack->m_nTrack;
	}

	int nError;
	BOOL bCameraOn = pDoc->m_pTrack->m_bCamEnable && pDoc->m_bCameraActive;

	while (WaitForSingleObject(pDoc->m_pTrack->m_EventKillInspCycle, 10) == WAIT_TIMEOUT) {
		if(bCameraOn) {
			//nError = pApp->m_bSynchronousGrabEnable ? pApp->SynchronousCameraSnap(FALSE) : 
			//	pDoc->CameraGrab(FALSE);
			nError = pDoc->CameraGrabUsingExposure();
			if(nError) {
				pDoc->m_pTrack->m_bContInsp = FALSE;
				break;
			}
		}
		
		//nError = pDoc->Inspect(0);
		nError = pDoc->InspectAllDoc(0);
	
		/*if(bCameraOn) {
			CAppDoc* pDoc1;
			for(int nTrack=0; nTrack<pApp->m_nTotalTracks; nTrack++) {
				pDoc1 = pApp->m_pTracks[0].m_pDoc[0];
				if(pDoc1->m_pRemoteInspDoc)
					DrwUpdate(&pDoc1->m_ImageViewInfo);
			}
		}*/

		if(nTrackIndex >= 0) {
			//pApp->UpdateLotSummary(nTrackIndex, nError);
			pApp->UpdateStatisticsWindow();
		}
	}

	return 0;
}

UINT TrackInspectCycle(LPVOID pParam)
{
	///Udupa; TODO
	//CPerformanceTimer m_Time,m_Time1;
	//CTrackHandler* pTrack = (CTrackHandler*) pParam;
	//CEvTabView* pTabView = (CEvTabView*) (((CApplication*)AfxGetApp())->m_pFrames[pTrack->m_nTrack]->GetActiveView());
	//int nError;

	//while (WaitForSingleObject(pTrack->m_EventKillInspCycle, 100) == WAIT_TIMEOUT) {
	//	for(int nDoc=0; nDoc<pTrack->m_nTotalDocs; nDoc++) {
	//		if(WaitForSingleObject(pTrack->m_EventKillInspCycle, 10)==WAIT_OBJECT_0)
	//			return 0;
	//		if(pTrack->m_pDoc[nDoc]->m_lDebugFlag&DEBUG_PRINT)
	//			OutputDebugLog("====================================================");
	//		nError = 0;
	//		if(pTrack->m_bCamEnable) {
	//			nError = pTrack->m_pDoc[nDoc]->CameraGrab(pTrack->m_pDoc[nDoc]->m_lDebugFlag!=0);
	//			if(nError) {
	//				pTrack->m_bTrackContInsp = FALSE;
	//				return 0;
	//			}
	//		}
	//		
	//		if(((CApplication*)AfxGetApp())->m_bTrackProcess)
	//			pTabView->ActivatePage(nDoc);
	//		nError = pTrack->m_pDoc[nDoc]->Inspect(nError);
	//		if(nError && !((CApplication*)AfxGetApp())->m_bTrackProcess)
	//			pTabView->ActivatePage(nDoc);
	//	}
	//}
	return 0;
}


UINT TrackAutoRunThread(LPVOID pParam)
{
	
	CTrackHandler* pTrack = (CTrackHandler*) pParam;
	CAppDoc* pDoc = (CAppDoc*) pParam;
	
	int nDoc;

	ResetEvent(pTrack->m_EventKillAutoRun);
	while (WaitForSingleObject(pDoc->m_pTrack->m_EventKillAutoRun, pDoc->m_pTrack->m_nAutoRunDelay) != WAIT_OBJECT_0) {
		for(nDoc=0; nDoc<pDoc->m_pTrack->m_nTotalDocs; nDoc++) {
			if(pDoc->m_pTrack->m_pDoc[nDoc]->m_bWaiting) {
				pDoc->m_pTrack->m_pDoc[nDoc]->Continue();
			}
		}
	}
	return 0;
}


UINT TrackCommandThread(LPVOID pParam)
{
	CTrackHandler* pTrack = (CTrackHandler*)pParam;
	CApplication* pApp = (CApplication*)AfxGetApp();
	CAppDoc* pDoc=NULL;
	int nError;

	while(1) {
		WaitForSingleObject(pTrack->m_EventRun, INFINITE);
		pDoc = (CAppDoc*)pTrack->m_pActiveDoc;
		if(!pDoc) {
			OutputDebugLog("TrackCommandThread closed");
			return 0;
		}

		if(pDoc->m_lDebugFlag&DEBUG_PRINT)
			OutputDebugLog("====================================================");

		switch (pTrack->m_nCommand) {
		case (TEACH) :
			nError = 0;
			pApp->m_bTeaching = TRUE;
			//if(pTrack->m_bCamEnable && pDoc->m_bCameraActive)
			//	nError = pApp->m_bSynchronousGrabEnable ? pApp->SynchronousCameraSnap(FALSE) : 
			//		pDoc->CameraGrab(FALSE);
			pDoc->Teach();
			pApp->m_bTeaching = FALSE;
			pApp->m_bTeachSingleImage = FALSE;

			//pApp->m_pMainWnd->SendMessage(WM_LOAD_RECIPE); // update teach value in teach params, safe?
			for (size_t i = 0; i < pApp->m_nFovCompletedTeach.size(); i++) {
				pApp->m_pMainWnd->SendMessage(WM_LOAD_SINGLE_FOV_RECIPE, (WPARAM)pTrack->m_nTrack, (LPARAM)pApp->m_nFovCompletedTeach[i]);
			}
			pApp->m_nFovCompletedTeach.clear();
			break;

		case (INSPECT) :
			nError = 0;
			pApp->m_bInspecting = TRUE;
			pDoc->m_bInspecting = TRUE;
			//if(pTrack->m_bCamEnable && pDoc->m_bCameraActive)
			//	nError = pApp->m_bSynchronousGrabEnable ? pApp->SynchronousCameraSnap(FALSE) : 
			//		pDoc->CameraGrab(FALSE);
			//pDoc->Inspect(nError);
			
			pApp->m_pTracks[pTrack->m_nTrack].m_pDoc[0]->InspectAllDoc(nError);
			pApp->m_bInspecting = FALSE;
			pDoc->m_bInspecting = FALSE;
			SetEvent(pDoc->m_EventInspEnd);
			break;

		case (INSPECT_ALL):
			nError = 0;
			pApp->m_bInspecting = TRUE;
			pDoc->m_bInspecting = TRUE;
			//for (int nTrack = 0; nTrack < pApp->m_nTotalTracks; nTrack++) {
			//	if (pApp->m_pTracks[nTrack].m_bCamEnable && pApp->m_pTracks[nTrack].m_pDoc[0]->m_bCameraActive)
			//		pApp->m_pTracks[nTrack].m_pDoc[0]->CameraGrabUsingExposure();
			//}
			
			for(int nTrack=0; nTrack<pApp->m_nTotalTracks; nTrack++)
				pApp->m_pTracks[nTrack].m_pDoc[0]->InspectAllDoc(nError);
			pApp->m_bInspecting = FALSE;
			pDoc->m_bInspecting = FALSE;
			pApp->m_bInspectionAll = !pApp->m_bInspectionAll;
			SetEvent(pDoc->m_EventInspEnd);
			break;
		case (COM_GRAB):
			nError = pApp->m_bSynchronousGrabEnable ? pApp->SynchronousCameraSnap(FALSE) : 
				pDoc->CameraGrab(FALSE);

			pDoc->m_ImageViewInfo.pImageView->RedrawWindow();
			SetEvent(pTrack->m_EventGrabEnd);
			break;
	
		case (COM_GRABINSP): {
			if(!pApp->m_bOnline)
				break;

			while(TRUE) {
				OutputDebugLogTo(pTrack->m_nTrack + 1,TRUE,"Track %d COM_GRABINSP START",pTrack->m_nTrack + 1);
				pTrack->TrackInspTime.Reset();

				CCameraInterface* pTrackFG = &pTrack->m_FGResource;
				CImageBuffer* pGrabbedBuffer;
				if(pApp->m_bIsCameraMonitorMode)
					pTrack->CancelSnap();
				pTrack->RefreshLight(pDoc->m_nDocIdx);

				nError = -1;
				if(pTrack->m_FGResource.lpImageGrab) {
					nError = pTrackFG->lpImageGrab(pTrackFG->pCamModule, &pGrabbedBuffer);
					//if(!nError)
					//	pDoc->CopyGrabbedBuffer(pGrabbedBuffer);
				}
				if(nError) {
					nError = IMAGE_SNAP_FAIL;
					if(pApp->m_bIsCameraMonitorMode)
						continue;
				}

				OutputDebugLogTo(pTrack->m_nTrack + 1, TRUE, "********** Track %d START OF INSPECTION **********",pTrack->m_nTrack + 1);

				pApp->SaveCrashImages(pTrack->m_nTrack);

				nError = pDoc->Inspect(nError);
				OutputDebugLogTo(pTrack->m_nTrack + 1, TRUE, "********** Track %d END OF INSPECTION **********",pTrack->m_nTrack + 1);

				break;
			}

			pApp->m_bInspecting = FALSE;

			if(pApp->m_bOnline || pApp->m_bTrackInspSeq)
				if(pApp->UpdateResults())//if(pApp->UpdateResult(pTrack->m_nTrack, nError))
					return -1;

			pDoc->m_bInspecting = FALSE;
			SetEvent(pDoc->m_EventInspEnd);
			
			break;
		}
	
		case (COM_ONLINE_SIMULATION): {
			if(pDoc->m_lDebugFlag & DEBUG_PRINT)
				OutputDebugLogTo(pTrack->m_nTrack + 1,TRUE,"Track %d START OF GRAB",pTrack->m_nTrack + 1);

			pTrack->TrackInspTime.Reset();

			nError = pApp->m_bSynchronousGrabEnable ? pApp->SynchronousCameraSnap(FALSE) : pDoc->CameraGrab(FALSE);

			if(pDoc->m_lDebugFlag&DEBUG_PRINT_EXT)
				OutputDebugLog("====================================================");
			
			if(pDoc->m_lDebugFlag & DEBUG_PRINT)
				OutputDebugLogTo(pTrack->m_nTrack + 1,TRUE,"Track %d START OF INSPECTION",pTrack->m_nTrack + 1);

			if(pApp->m_bEnableSimulationMode)
				pTrack->GetNextSimulationBuffer();

			nError = pDoc->Inspect(nError);

			pApp->m_bInspecting = FALSE;
			if(pApp->m_bOnline || pApp->m_bTrackInspSeq)
				if(pApp->UpdateResults())//if(pApp->UpdateResult(pTrack->m_nTrack, nError))
					return -1;
					
			
			pDoc->m_bInspecting = FALSE;
			SetEvent(pDoc->m_EventInspEnd);
			break;
		}
		case (CALIBRATE) :
			nError = 0;
			//if(pTrack->m_bCamAvail && pDoc->m_bCameraActive && pDoc->m_bDotCalibration)
			//	pDoc->OnCameraSnap();	//pDoc->CameraGrab(FALSE);
			
		/*	if((!pTrack->m_bCamAvail || !pDoc->m_bCameraActive) && pDoc->m_bDotCalibration) {
				CString strImagePath = pDoc->LoadImage();
				pDoc->LoadStaticImage(strImagePath);
			}		*/
			pDoc->SetDotCalibration();
			break;
		case (CALIBRATE_LIGHT) :
			if (pDoc->m_pTrack->m_bCamAvail) {
				pDoc->SetLightCalibration();
			}
			else {
				pDoc->SetLightCalibrationOffline();
			}
			
			break;
		case (KILL_CMD_THREAD) :
			return 0;

		}
	}
	return 0;
}

UINT InspectSeqThread(LPVOID pParam)
{
	CApplication *pApp = (CApplication*)AfxGetApp();
	if (pApp->m_lCOMMInCardNo<0 || pApp->m_lCOMMInPortID<0) {
		ASSERT(!"m_lCOMMInCardNo or m_lCOMMInPortID not initialised!!!"); 
		return -1;
	}

	int nError;
	long lInData = 0;
	pApp->m_CommModule.SetDIInterrupt(pApp->m_lCOMMOutCardNo, pApp->m_lCOMMOutPortID, 0x03, 0);

	OutputDebugLog("InspectSeqThread started");
	while(1) {
		WaitForSingleObject(pApp->m_eventOnline, INFINITE);
		if(pApp->m_bExitApp) {
			OutputDebugLog("InspectSeqThread closed");
			return 0;
		}
		
		if(!pApp->m_bStream && ( pApp->m_bReceivedStartCmd || pApp->m_bReScanSequence)) {
			OutputDebugLogTo(0, TRUE, "Received Start Cmd");
			pApp->m_bStream = TRUE;
			pApp->m_bScanning = TRUE;

			nError = pApp->InspectionScanSequenceOnline();

			if(nError) {
				OutputDebugLogTo(9,TRUE,"InspectSeqThread Error ");
			}

			pApp->m_bReceivedStartCmd = FALSE;
			pApp->m_bReScanSequence = FALSE;
			pApp->m_bScanning = FALSE;
		}
		Sleep(50);
	}
}

UINT VisionResetThread(LPVOID pParam) // Changed to End lot Thread
{
	CApplication *pApp = (CApplication*)AfxGetApp();
	if (pApp->m_lCOMMInCardNo<0 || pApp->m_lCOMMInPortID<0) {
		ASSERT(!"m_lCOMMInCardNo or m_lCOMMInPortID not initialised!!!"); 
		return -1;
	}

	while(1) {
		WaitForSingleObject(pApp->m_eventOnline, INFINITE);
		if(pApp->m_bExitApp) {
			OutputDebugLog("VisionResetThread (End Lot) closed");
			return 0;
		}
		if(pApp->m_strIOInterfaceCard != "none")
		{		
			if(pApp->m_CommModule.WaitForActiveDIInterrupt(pApp->m_lCOMMInCardNo, pApp->m_lCOMMInPortID, INFINITE, 1) == 0)
			{
				OutputDebugLog("VisionResetThread:ClearActiveDIInterrupt (End Lot) done");
				pApp->m_CommModule.ClearActiveDIInterrupt(1);
				
				// Record Lot Stats
				//pApp->RecordLotStats();

				// Reset Counter
				pApp->m_nGoodACounter = 0;
				pApp->m_nGoodBCounter = 0;
				pApp->m_nFailCounter = 0;

				//Reset Statistic
	//			pApp->m_LotSummary.InitDisplaySummary();
				pApp->UpdateStatisticsWindow();

			}
		}
	}
}


UINT PollingThread(LPVOID pParam)
{
	CApplication *pApp = (CApplication*)AfxGetApp();
	if (pApp->m_lCOMMInCardNo<0 || pApp->m_lCOMMInPortID<0) {
		ASSERT(!"m_lCOMMInCardNo or m_lCOMMInPortID not initialised!!!"); 
		return -1;
	}

	long lInData;

	while(1) {
		WaitForSingleObject(pApp->m_eventVisionReady, INFINITE);
		if(pApp->m_bExitApp) {
			OutputDebugLog("PollingThread closed");
			return 0;
		}
		if(pApp->m_strIOInterfaceCard != "none")
		{
			//Read I/O Status
			pApp->m_CommModule.InPortRead(pApp->m_lCOMMInCardNo, pApp->m_lCOMMInPortID, &lInData, 0);
			if(lInData & IN_MODE_SELECTION_READY)
			{
				if((lInData & IN_MODE_BIT1))
				{
					if((lInData & IN_MODE_BIT2))
					{
						OutputDebugLog("Mode 4 Selected");
						//Mode 4
						pApp->m_nOperationMode = 4;
						pApp->m_nOutData |= (OUT_MODE_BIT1 | OUT_MODE_BIT2);
					}
					else
					{
						OutputDebugLog("Mode 2 Selected");
						//Mode 2
						pApp->m_nOperationMode = 2;
						pApp->m_nOutData &= ~OUT_MODE_BIT1;
						pApp->m_nOutData |= OUT_MODE_BIT2;
					}
				}
				else
				{
					if((lInData & IN_MODE_BIT2))
					{
						OutputDebugLog("Mode 3 Selected");
						//Mode 3
						pApp->m_nOperationMode = 3;
						pApp->m_nOutData |= OUT_MODE_BIT1;
						pApp->m_nOutData &= ~OUT_MODE_BIT2;
					}
					else
					{
						OutputDebugLog("Mode 1 Selected");
						//Mode 1
						pApp->m_nOperationMode = 1;
						pApp->m_nOutData &= (~OUT_MODE_BIT1 & ~OUT_MODE_BIT2);
					}
				}
				pApp->SetRecipeName(pApp->m_strConfigFile);
				PutRegistryEntry(WRITE_REGPATH + "Preferences", "Operation Mode", pApp->m_nOperationMode);
				pApp->m_CommModule.OutPortWrite(pApp->m_lCOMMInCardNo, pApp->m_lCOMMOutPortID, pApp->m_nOutData, 0);
			}
			else if(lInData & IN_TRAY_IN_PLACE) 
			{
				if(!pApp->m_bBarcodeScanning)
				{
					CScanBarcode scanBarcodeDlg;
					pApp->m_nOutData &= ~OUT_BARCODE_STATUS;
					pApp->m_CommModule.OutPortWrite(pApp->m_lCOMMInCardNo, pApp->m_lCOMMOutPortID, pApp->m_nOutData, 0);

					pApp->m_nOutData |= OUT_BARCODE_READ;
					pApp->m_CommModule.OutPortWrite(pApp->m_lCOMMInCardNo, pApp->m_lCOMMOutPortID, pApp->m_nOutData, 0);

					pApp->m_bBarcodeScanning = TRUE;
					scanBarcodeDlg.DoModal();

					pApp->m_bRecipeFound ? pApp->m_nOutData |= OUT_BARCODE_STATUS : pApp->m_nOutData &= ~OUT_BARCODE_STATUS;
					pApp->m_CommModule.OutPortWrite(pApp->m_lCOMMInCardNo, pApp->m_lCOMMOutPortID, pApp->m_nOutData, 0);

					Sleep(50);

					pApp->m_nOutData &= ~OUT_BARCODE_READ;
					pApp->m_CommModule.OutPortWrite(pApp->m_lCOMMInCardNo, pApp->m_lCOMMOutPortID, pApp->m_nOutData, 0);

					Sleep(50);
					pApp->m_bBarcodeScanning = FALSE;
				}
			}
		}
	}
}

UINT TrackInspectSeqThread(LPVOID pParam)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	CTrackHandler* pTrack = (CTrackHandler*) pParam;
	CCameraInterface* pTrackFG = &pTrack->m_FGResource;
	BOOL bHome = FALSE;
	int nError;
	if (pApp->m_lCOMMInCardNo<0 || pApp->m_lCOMMInPortID<0) {
		ASSERT(!"m_lCOMMInCardNo or m_lCOMMInPortID not initialised!!!"); 
		return -1;
	}
	if(!pTrackFG->lpEnqueueAsyncGrab || !pTrackFG->lpCancelPendingGrab || !pTrackFG->lpWaitForExposure) {
		OutputDebugLog("[Track%d] Camera Enqueue/Cancel/Wait access error", pTrack->m_nTrack);
		return 0;
	}


	long lInData;
	while(1) {
		WaitForSingleObject(pApp->m_eventOnline, INFINITE);
		if(pApp->m_bExitApp) {
			OutputDebugLog("InspectSeqThread closed");
			return 0;
		}
		//if((pApp->m_cActiveTracks & (1 << pTrack->m_nTrack)) == 0) {
		//	Sleep(1000);
		//	continue;
		//}

		//if(pTrack->m_bCamAvail) {
		//	pTrack->EnqueueAsyncGrab();

		//	int nError = pTrack->WaitForEndOfExposure();
		//	if(nError != 0) {
		//		OutputDebugLog("[%d] Wait For Exposure error", pTrack->m_nTrack);
		//		continue;
		//	}

		//	if(!pApp->m_bOnline) {
		//		OutputDebugLog("[%d - ExposureEnd] Offline; Loop back", pTrack->m_nTrack);
		//		Sleep(300);
		//		continue;
		//	}

		//	lInData = 0;
		//	pApp->m_CommModule.InPortRead(pApp->m_lCOMMInCardNo, pApp->m_lCOMMInPortID, &lInData, 0);

		//	if(!pTrack->m_pDoc[0]->m_bInspecting) {
		//		pTrack->m_pDoc[0]->m_bInspecting = TRUE;
		//		pTrack->ComGrabInspect(0);
	
		//		//pApp->RecordLotStats();
		//	}
		//	else
		//		pTrack->CancelSnap();
	/*	}
		else
			Sleep(1000);*/
	}
}

///Udupa; New version
///Improvements:
/// 1. Sort the files in ascending order of the filename (earlier, it was arbitrary, as dictated by the SDK, depending on the hardware)
/// 2. Fix an issue of erratic behavior of inspection results when executing parallely for multiple tracks
/// 3. Enable parallel inspection of different folders in multiple tracks
/// 4. Got rid of lot of redundancies
//DWORD WINAPI TrackInspectImages(LPVOID pParam)
UINT TrackInspectImages(LPVOID pParam)
{
	double dTime = 0;
	double dTimeSum;
	CPerformanceTimer timer;
	CApplication* pApp = (CApplication*) AfxGetApp();
	CAppDoc* pDoc = (CAppDoc*) pParam;
	CTrackHandler* pTrack = (CTrackHandler*) pDoc->m_pTrack;
	int nDieStats;
	pApp->m_nStepTrack = pTrack->m_nTrack;
	pApp->m_nStepDoc = pDoc->m_nDocIdx;

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;

	CStringArray arrayFilePaths;
	CStringArray arrayFileFolders;
	CString strFolder, strFileName, strFilePath;
	CArray <CString, CString> strFileNames;
	CArray <CString, CString> strFilePaths;
	CString strType[] = { "T", "B", "S" };

	CFileFind FileFind;
	strFolder = pTrack->m_strStoredImagesPath.Trim();
	if(strFolder.Right(1) != "\\")
		strFolder.Append("\\");
	strFilePath = strFolder + "*.bmp";

	BOOL bNextFile = FileFind.FindFile(strFilePath.GetBuffer(0));
	strFilePath.ReleaseBuffer();


	if (!pApp->m_bSetForAllTracks) {
			GetImageFilePaths(strFolder, arrayFilePaths, arrayFileFolders, TRUE, strType[pTrack->m_nTrack]);
	}
	else
		GetImageFilePaths(strFolder, arrayFilePaths, arrayFileFolders, TRUE,"");

	int nFolderCount = arrayFileFolders.GetCount();
	///int nTotalCount = arrayFileNames.GetSize();
	INT_PTR nTotalCount = arrayFilePaths.GetSize();
	int nTrackIndex = pTrack->m_nTrack;
	
	OutputDebugLogTo(nTrackIndex+1, FALSE, " Stored Images Inspection"); 
	OutputDebugLogTo(nTrackIndex+1, FALSE, "=========================="); 
	OutputDebugLogTo(nTrackIndex+1, FALSE, "Number of files found for inspection: %d\n", nTotalCount); 	


	if(nTotalCount > 0) {
		CString* fileNames = new CString[nTotalCount];
		for(int k=0; k<nTotalCount; k++)
			fileNames[k] = arrayFilePaths.GetAt(k);
		arrayFilePaths.RemoveAll();

		CString strStatus;
		int nError = 0;

		pApp->InitializeLotStats();
	//	pApp->m_LotSummary.InitDisplaySummary(nTrackIndex);

	
		int i;
		BOOL bFovFail;
		BOOL bBreak = FALSE;
		BOOL waitInspection = FALSE;
		int nPos = -1;
		strFolder = "";

		pApp->m_bIsFailPart = FALSE;
		CPerformanceTimer timerFlushBuffer;
		pApp->m_pImageQueue->FlushQueueBufferCentralized();
		OutputDebugLogTo(5, TRUE, "FlushQueueBufferCentralized time: %.2fms", timerFlushBuffer.GetElapsedMSec());
		timerFlushBuffer.Reset();
		pApp->m_pImageQueue->FlushQueueDefectInfoCentralized();
		OutputDebugLogTo(5, TRUE, "FlushQueueDefectInfoCentralized time: %.2fms", timerFlushBuffer.GetElapsedMSec());


		pApp->ClearResults();
		timer.Reset();
		while(TRUE){
			for (i = 0; i < nTotalCount; i++) {
				pDoc->m_bAccurateResize = TRUE;
				nPos = fileNames[i].ReverseFind('\\') + 1;
				strFolder = fileNames[i].Left(nPos);
				strFileName = fileNames[i].Mid(nPos);
				strFilePath = fileNames[i];
				bFovFail = FALSE;

				//Update MID Folder 
				CString strMIDFolder;
				CString strCurrentMidFolder = fileNames[i].Left(nPos - 1);
				int nTempPos = strCurrentMidFolder.ReverseFind('\\') + 1;
				strCurrentMidFolder = strCurrentMidFolder.Mid(nTempPos);
				strMIDFolder = strCurrentMidFolder;
				strCurrentMidFolder = "Current MID Folder: " + strCurrentMidFolder;
				UpdateStatusBar(&strCurrentMidFolder);

				std::vector<int> nTrackInspectedId;
				if (pApp->m_bSetForAllTracks) {
					for (int nTrack = 0; nTrack < pApp->m_nTotalTracks; nTrack++) {
						nTrackInspectedId.push_back(nTrack);
					}
				}
				else {
					nTrackInspectedId.push_back(pTrack->m_nTrack);
				}

				std::vector<std::vector <BOOL>> waitEventInspection;
				waitEventInspection.resize(pApp->m_nTotalTracks);
				for (int i = 0; i < nTrackInspectedId.size(); i++) {
					int nTrack = nTrackInspectedId[i];
					waitEventInspection[nTrack].resize(pApp->m_pTracks[nTrack].getInspectionThreadCount(), FALSE);
				}

				CPerformanceTimer nDieTimer;
				for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
					if (!pApp->m_bSetForAllTracks && pApp->m_bSetForSingleFov && pApp->m_pTracks[nTrackIndex].m_nFOVIndex != nFov)
						continue;
					int threadId;
					try {
						for (int i = 0; i < nTrackInspectedId.size(); i++) {
							//Load image to doc for inspection
							int nTrack = nTrackInspectedId[i];

							if (pApp->m_bCentralizedVerificationMode && !pApp->m_CentralizedCommInterface.m_bIsTCPServer) {
								CString strMsg;
								strMsg.Format("Track %d Scanning Fov %d", nTrack, nFov);
								pApp->m_CentralizedCommInterface.m_pCommManager[0]->SendDataMessage(SEND_MESSAGE, strMsg);
							}

							EnterCriticalSection(&pApp->m_csFileAccess);
							nError = pApp->m_pTracks[nTrack].m_pDoc[0]->LoadImagesToDocForInspection(strFilePath, nFov);
							theApp.m_strPartFlex[pApp->m_pTracks[nTrack].m_nDieIndex] = strMIDFolder;
							UpdateStatusBar(&strCurrentMidFolder);
							LeaveCriticalSection(&pApp->m_csFileAccess);

							int nDevice = pApp->m_pTracks[nTrack].m_nDieIndex;

							if (pApp->m_bMultithreadInspectionMode && pApp->m_pTracks[nTrack].getInspectionThreadCount() > 0) {
								EnterCriticalSection(&pApp->m_pTracks[nTrack].CriticalSectionScannedImage);
								threadId = nFov % pApp->m_pTracks[nTrack].getInspectionThreadCount();
								pApp->m_pTracks[nTrack].grabbedImageId[threadId].push(nFov);
								pApp->m_pTracks[nTrack].grabbedDeviceId[threadId].push(nDevice);

								LeaveCriticalSection(&pApp->m_pTracks[nTrack].CriticalSectionScannedImage);

								SetEvent(pApp->m_pTracks[nTrack].eventDoInspection[threadId]);

								strFileName.ReleaseBuffer();
								strStatus.ReleaseBuffer();
								Sleep(pApp->m_nInSpectionDelay);
								waitInspection = TRUE;
								waitEventInspection[nTrack][threadId] = TRUE;
							}
							else {
								pApp->m_pTracks[nTrack].m_pDoc[0]->InspectAllDocOnline(nDevice, nFov);
							}
						}
						
					}
					catch (...) {
						OutputDebugLogTo(nTrackIndex + 1, FALSE, "Exception raised while loading image from hard disk; Skipping the file for inspection [%s]", strFileName);
						if (WaitForSingleObject(pTrack->m_EventKillInspectImages, 0) == WAIT_OBJECT_0) {
							bBreak = TRUE;
							break;
						}
						continue;

					}
					
					if (pApp->m_bSetForAllTracks)
					{
						for (int nTrack = 0; nTrack < pApp->m_nTotalTracks; nTrack++)
							i += pApp->m_nTotalViewDocs[nTrack];//i++;
					}
					else if (pApp->m_bSetForSingleFov)
						i = nTotalCount;
					else
						i += pApp->m_nTotalViewDocs[nTrackIndex];
				}
				if (!pApp->m_bSetForSingleFov) {
					if (waitInspection) {
						
						std::vector <HANDLE> eventInspection;
						for (int i = 0; i < nTrackInspectedId.size(); i++) {
							int nTrack = nTrackInspectedId[i];
							if (pApp->m_pTracks[nTrack].getInspectionThreadCount() > 0) {
								for (int nThreadId = 0; nThreadId < pApp->m_pTracks[nTrack].getInspectionThreadCount(); nThreadId++) {
									if (waitEventInspection[nTrack][nThreadId])
										eventInspection.push_back(pApp->m_pTracks[nTrack].eventInspectionDone[nThreadId]);
								}
							}
							else {
								DWORD dwError = WaitForSingleObject(pApp->m_pTracks[nTrack].eventInspectionDone[0], INFINITE);
								if (dwError == WAIT_FAILED || dwError == WAIT_TIMEOUT) {
									OutputDebugLogTo(7, TRUE, "-----ERROR: Image Inspection threads Waiting have been failed.");
									return 0;
								}
							}
						}

						DWORD dwError = WaitForMultipleObjects(eventInspection.size(), eventInspection.data(), TRUE, INFINITE);
						if (dwError == WAIT_FAILED || dwError == WAIT_TIMEOUT) {
							OutputDebugLogTo(7, TRUE, "-----ERROR: Image Inspection threads Waiting have been failed.");
							return 0;
						}
					}

					int nTempDieTimer = nDieTimer.GetElapsedMSec();
					pApp->UpdateUPH(nTempDieTimer, m_nNoOfDies);
					
					if (pApp->m_bSetForAllTracks) {
						/*pApp->m_bDisplayAllStatsOffline = TRUE;
						int  nVerificationModeBk = pApp->m_nVerificationMode;
						pApp->m_nVerificationMode = 1;*/

						pApp->UpdateResults();

						/*pApp->m_nVerificationMode = nVerificationModeBk;
						pApp->m_bDisplayAllStatsOffline = FALSE;*/
					}
					else {
						for (int nFOV = 0; nFOV < m_nNoOfFov; nFOV++) {
							pApp->m_bDisplayAllStatsOffline = TRUE;
							pApp->UpdateResultsOffline(pTrack->m_nDieIndex, nFOV, FALSE);
							pApp->m_bDisplayAllStatsOffline = FALSE;
						}
						nDieStats = pTrack->m_nDieIndex;
						pApp->m_bDisplayAllStatsOffline = TRUE;
						pApp->UpdateResultsOffline(nDieStats, -1, TRUE);
						pApp->m_bDisplayAllStatsOffline = FALSE;
					}
				}

				if (pDoc->m_bSavedImagesStep) {
					pApp->m_pTracks[nTrackIndex].m_pDoc[pApp->m_nActiveDocIdx]->m_bWaiting = TRUE;

					if (WaitResponse(&pApp->m_pTracks[nTrackIndex].m_pDoc[pApp->m_nActiveDocIdx]->m_ImageViewInfo)) {
						pApp->m_pTracks[nTrackIndex].m_pDoc[pApp->m_nActiveDocIdx]->m_bWaiting = FALSE;
						pApp->m_pTracks[nTrackIndex].m_pDoc[pApp->m_nActiveDocIdx]->m_bSavedImagesStep = FALSE;

						i = nTotalCount + 1;
						bBreak = TRUE;
						break;
					}	
				}
				if (WaitForSingleObject(pTrack->m_EventKillInspectImages, 0) == WAIT_OBJECT_0) {
					i = nTotalCount+1;
					bBreak = TRUE;
					pDoc->m_bSavedImagesStep = FALSE;
					pApp->m_pTracks[nTrackIndex].m_pDoc[pApp->m_nActiveDocIdx]->m_bSavedImagesStep = FALSE;
					break;
				}
					
				strFileNames.RemoveAll();
				strFilePaths.RemoveAll();

				if (((pApp->m_pTracks[nTrackIndex].m_pDoc[0]->m_lDebugFlag & DEBUG_SAVE_BAD_IMAGE) && bFovFail) ||
					((pApp->m_pTracks[nTrackIndex].m_pDoc[0]->m_lDebugFlag & DEBUG_SAVE_GOOD_IMAGE) && !bFovFail)) {
					for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
						if (!pApp->m_bSetForAllTracks && pApp->m_bSetForSingleFov && pApp->m_pTracks[nTrackIndex].m_nFOVIndex != nFov)
							continue;

						for (int nTrack = 0; nTrack < pApp->m_nTotalTracks; nTrack++) {
							if (!pApp->m_bSetForAllTracks && nTrack != nTrackIndex)
								continue;
							for (int k = 0; k < pApp->m_nTotalViewDocs[nTrack]; k++) {
								CString strImageName, strImageExt, strImageFinalPath, strPos;
								int nPos = -1;
								CString strPath;
								strPath = strFileName;
								if (strPath != "") {
									nPos = strPath.ReverseFind('_');
									if (nPos > 0) {
										strImageExt = strPath.Mid(strPath.ReverseFind('.'));
										strImageName = strPath.Mid(0, nPos);
										CString strTemp;
										int nPosEnd = strImageName.ReverseFind('_');
										strTemp = strImageName.Mid(0, nPosEnd);
										strImageFinalPath.Format("%s_Pos%d_%s%d%s", strTemp, nFov + 1, strType[nTrack], k + 1, strImageExt);
										strImageExt.Format("%s\\%s", strFolder, strImageFinalPath);
										CString strTemp1;
										int nLength = strFolder.GetLength();
										strTemp1 = strFolder.Mid(0, nLength - 1);
										nPosEnd = strTemp1.ReverseFind('\\');
										nLength = strTemp1.GetLength();
										strTemp = strTemp1.Mid(nPosEnd, nLength);

										pDoc->SaveOfflineImages(pApp->m_nPartErrorCode[0], strImageExt, strImageFinalPath, strTemp);
									}
								}
							}

						}
					}
				}
			}

			dTime = timer.GetElapsedMSec();
			OutputDebugLogTo(nTrackIndex+1, FALSE, "-------------------- Total Inspection Time: %3.1fms", dTime);

		if(bBreak || !pApp->m_bStoredImagesLoop)
			break;
		}
		delete[] fileNames;

		
		OutputDebugLogTo(nTrackIndex+1, FALSE, " =============== End of Stored Images Inspection ==============="); 

		//pApp->m_pStatsView->RecordLotStats();
	//	pApp->m_LotSummary.InitDisplaySummary(nTrackIndex);

	}
	if (pApp->m_bSetForAllTracks)
	{
		for (int nTrack = 0; nTrack < pApp->m_nTotalTracks; nTrack++) {
			for(int nDoc = 0;nDoc<pApp->m_nTotalViewDocs[nTrack];nDoc++)
				pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_bInspSavedImage = FALSE;
			pApp->m_bInspectionAll = FALSE;
		}
	}
	else {
		pDoc->m_bInspSavedImage = FALSE;
		for (int nDoc = 0; nDoc < pApp->m_nTotalViewDocs[nTrackIndex]; nDoc++)
			pApp->m_pTracks[nTrackIndex].m_pDoc[nDoc]->m_bSavedImagesStep = FALSE;
	}
	pApp->m_bInspecting = FALSE;

	//COLORREF m_colorTemp = RGB(125, 125, 120);
	//for (int nDIE = 0; nDIE < m_nNoOfDies; nDIE++) {
	//	for (int nFOV = 0; nFOV < m_nNoOfFov; nFOV++) {
	//	
	//		pApp->m_colorFovDieMapping[nDIE][nFOV].DeleteObject();
	//		pApp->m_colorFovDieMapping[nDIE][nFOV].CreateSolidBrush(m_colorTemp);

	//		pApp->m_bDieInspection[nDIE][nFOV] = FALSE;
	//		pApp->m_nFovPartErrorCode[nDIE][nFOV] = 98765;
	//	}
	//}
	//((CMainFrame*) pApp->m_pMainWnd)->m_dlgPane->m_wndDlg.Invalidate();

	ResetEvent(pTrack->m_EventKillInspectImages);
	SetEvent(pTrack->m_hInspectImagesKilled);

	return 0;
}

UINT TrackUpdateThread(LPVOID pParam)
{
	CTrackHandler* pTrack = (CTrackHandler*) pParam;
	CApplication* pApp = (CApplication*)AfxGetApp();
	CAppDoc* pDoc=NULL;
//	int nError;
	bool bLotChangedAck = true;
	while(1) {
		WaitForSingleObject(pTrack->m_EventUpdate, INFINITE);
		pDoc = (CAppDoc*)pTrack->m_pActiveUpdateDoc;
		if(!pDoc) {
			OutputDebugLog("TrackUpdateThread closed");
			return 0;
		}

		if (pDoc->m_lDebugFlag&DEBUG_PRINT)
			OutputDebugLog("====================================================");
	}
}

//UINT TrackImageUpdate(LPVOID pParam)
//{
//
//	CApplication* pApp = (CApplication*)AfxGetApp();
//	//CTrackHandler* pTrack = (CTrackHandler*) pParam;
//	CAppDoc* pDoc=(CAppDoc*) pParam;;
//	//CMainFrame *pMainFrame;
//	//int cstring float
//
//	while(TRUE)
//	{
//		WaitForSingleObject(pDoc->m_EventImageUpdate,INFINITE);
//		if (pApp->m_bExitApp) {
//			OutputDebugLog("TrackImageUpdate closed");
//			return 0;
//		}
//		OutputDebugLogTo(8, TRUE, "Copy time: Before");
//		
//		//((CMainFrame*) pApp->m_pMainWnd)->m_dlgPane->m_wndDlg.CopyImagesToStats(pDoc[0].bufImageUpdate);
//		OutputDebugLogTo(8, TRUE, "Copy time After");
//	}
//
//   return 0;
//}

UINT TrackUpdateImage(LPVOID pParam)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	CTrackHandler* pTrack = (CTrackHandler*) pParam;
	CAppDoc* pDoc = NULL;
	BOOL bDisplay;
	int nError[3];

	while(TRUE) {
		WaitForSingleObject(pTrack->m_hTrackEventUpdateImage, INFINITE);
		pDoc = (CAppDoc*) pTrack->m_pActiveUpdateDoc;
		
		if(!pDoc) {
			OutputDebugLog("TrackUpdateImage closed");
			return 0;
		}

		bDisplay = TRUE;
		if(pApp->m_bOnline)
			bDisplay = FALSE;

		nError[pTrack->m_nTrack] = pTrack->WaitForSnapCompletion(bDisplay, pDoc->m_nDocIdx, TRUE);
		SetEvent(pTrack->m_EventGrabEnd);
		OutputDebugLogTo(2+pTrack->m_nTrack, TRUE, "Track%d [Doc%d] GrabEnd Event Set", pTrack->m_nTrack+1, pDoc->m_nDocIdx+1);
		
		if(!pTrack->m_nTrack && pApp->m_bStream /*&& (pDoc->m_nDocIdx==pApp->m_nTotalViewDocs[pTrack->m_nTrack])*/)
			pApp->m_bStream = FALSE;
		if(pApp->m_bSnap /*&& (pDoc->m_nDocIdx==pApp->m_nTotalViewDocs[pTrack->m_nTrack])*/)
			pApp->m_bSnap = FALSE;

		if(nError[pTrack->m_nTrack])		//// return if WaitForSnapCompletion error
			pTrack->m_nOpticsRecalcMode = 0;
	}
	
	return 0;
}

UINT RefreshLightThread(LPVOID pParam)
{
	CApplication* pApp = (CApplication*) pParam;

	while(TRUE) {
		WaitForSingleObject(pApp->m_EventRefreshLight, INFINITE);

		if(pApp->m_bExitApp) {
			OutputDebugLog("RefreshLightThread closed");
			return 0;
		}

		pApp->m_LightHandler.RefreshLight();
		pApp->m_LightHandler.ResetLightSteps();

		ResetEvent(pApp->m_EventRefreshLight);//// Reset Event Once Refresh Light has done
	}

	return 0;
}


UINT RefreshMotionStop(LPVOID pParam)
{
	CApplication* pApp = (CApplication*) pParam;
	int nCardNo = 0;
	int nPortNo = 1;
	BOOL bPreviousEmergencyStatus = pApp->m_bEmergencyStop;

	WaitForSingleObject(pApp->m_EventMotionSensor, INFINITE);
	while(1) {
		if(pApp->m_bExitApp) {
			OutputDebugLogTo(7,TRUE,"RefreshMotionStop closed");
			return 0;
		}
		UINT pDataInCurtainSensor = 1, pDataInEmgStop;
		
		if(!pApp->m_bBypassCurtainSensor){
			pApp->m_MotorResource.InPortReadBit(nCardNo, nPortNo,IN_MOTOR_CURTAIN_SENSOR,&pDataInCurtainSensor);
			if(!pDataInCurtainSensor){
				pApp->m_bCurtainSensor = TRUE;
				OutputDebugLogTo(9, TRUE, "Curtain Sensor Triggered");
				CString strMsg;
				strMsg = "Curtain Sensor Triggered";
				if (pApp->m_bExitApp) {
					OutputDebugLogTo(7, TRUE, "RefreshMotionStop closed");
					return 0;
				}
				pApp->AddLogEvent(strMsg);

				if(pApp->m_bInspSequenceOffline)
					pApp->OnInspInspectionSequence();

				if(pApp->m_bOnline){
					pApp->m_bStream = TRUE;
					ResetEvent(pApp->m_eventOnline);
					OutputDebugLogTo(9,TRUE,"RefreshMotionStop ResetEvent(pApp->m_eventOnline) m_bCurtainSensor = %d m_bOnline=%d",pApp->m_bCurtainSensor,pApp->m_bOnline);
				}
					
				SetEvent(pApp->m_EventMotionSensorStart);
				ResetEvent(pApp->m_EventMotionSensor);
			}
		}

		long DataIn;
		for(int i=0;i<NO_OF_AXIS;i++){
			pApp->motionParameters.GetMotorIOStatus(i, DataIn);
		}
		
		pApp->m_bEmergencyStop = (DataIn >> 4) & 1;

		if(pApp->m_bEmergencyStop && !bPreviousEmergencyStatus){
			OutputDebugLogTo(9, TRUE, "Emergency Stop Triggered");

			pApp->motionParameters.SetMotorHomed(X_SERVO_MOTOR, FALSE);
			pApp->motionParameters.SetMotorHomed(Y_SERVO_MOTOR, FALSE);
			pApp->motionParameters.SetMotorHomed(Z_SERVO_TOP_MOTOR, FALSE);
			pApp->motionParameters.SetMotorHomed(Z_SERVO_BOTTOM_MOTOR, FALSE);

			pApp->EnableMachineResetOperation(TRUE);


			if (pApp->m_bExitApp) {
				OutputDebugLogTo(7, TRUE, "RefreshMotionStop closed");
				return 0;
			}

			CString strMsg;
			strMsg = "Emergency Stop Triggered";
			pApp->AddLogEvent(strMsg);
			pApp->ShowMessageDlgBox(strMsg, true);

			if(pApp->m_bInspSequenceOffline)
				pApp->OnInspInspectionSequence();

			if(pApp->m_bOnline){
				pApp->m_bStream = TRUE;
				ResetEvent(pApp->m_eventOnline);
				OutputDebugLogTo(9,TRUE,"RefreshMotionStop ResetEvent(pApp->m_eventOnline) m_bEmergencyStop = %d m_bOnline=%d",pApp->m_bEmergencyStop,pApp->m_bOnline);
			}
	
			/*SetEvent(pApp->m_EventMotionSensorStart);
			ResetEvent(pApp->m_EventMotionSensor);*/
		}
		else {
			pApp->ShowMessageDlgBox(FALSE);
		}
		bPreviousEmergencyStatus = pApp->m_bEmergencyStop;

		Sleep(pApp->m_nSignalScanFrequency);
	}

	return 0;
}

UINT RefreshMotionStart(LPVOID pParam)
{
	CApplication* pApp = (CApplication*) pParam;

	while(1) {
		WaitForSingleObject(pApp->m_EventMotionSensorStart, INFINITE);

		if(pApp->m_bExitApp) {
			OutputDebugLogTo(7,TRUE,"RefreshMotionStop closed");
			return 0;
		}
		UINT pDataInCurtainSensor=1, pDataInEmgStop;
		//OutputDebugLogTo(8,TRUE,"RefreshMotionStart Trigger 1");
		if(!pApp->m_bBypassCurtainSensor){
			
			if(pApp->m_bCurtainSensor){
			//	OutputDebugLogTo(9, TRUE, "RefreshMotionStart 1");// m_bCurtainSensor = %d pData = %d", pApp->m_bCurtainSensor, pDataInCurtainSensor);

				pApp->m_MotorResource.InPortReadBit(0,1, IN_MOTOR_CURTAIN_SENSOR,&pDataInCurtainSensor);
				//OutputDebugLogTo(9, TRUE, "RefreshMotionStart  m_bCurtainSensor = %d pData=%d", pApp->m_bCurtainSensor, pDataInCurtainSensor);

				if(pDataInCurtainSensor){
	//					OutputDebugLogTo(9,TRUE,"RefreshMotionStart  m_bCurtainSensor = %d pData=%d",pApp->m_bCurtainSensor, pDataInCurtainSensor);
						if(!pApp->m_bOnline){
							pApp->m_bCurtainSensor = FALSE;
							ResetEvent(pApp->m_EventMotionSensorStart);
						//	OutputDebugLogTo(9,TRUE,"RefreshMotionStart ResetEvent(pApp->m_eventOnline) m_bCurtainSensor = %d m_bOnline=%d",pApp->m_bCurtainSensor,pApp->m_bOnline);
						}

						if(pApp->m_bOnline){
							pApp->m_bCurtainSensor = FALSE;
							pApp->m_bStream = FALSE;
							pApp->m_bReceivedStartCmd = FALSE;
							SetEvent(pApp->m_eventOnline);
							SetEvent(pApp->m_EventMotionSensor);
							ResetEvent(pApp->m_EventMotionSensorStart);
							//OutputDebugLogTo(9,TRUE,"RefreshMotionStart ResetEvent(pApp->m_eventOnline) m_bCurtainSensor = %d m_bOnline=%d",pApp->m_bCurtainSensor,pApp->m_bOnline);
						}

//						pApp->ShowMessageDlgBox("", false);

						//return 0;
				}
			}
		}
		//OutputDebugLogTo(8,TRUE,"RefreshMotionStart Trigger 2");
		long DataIn;
		if(pApp->m_bEmergencyStop){
			for(int i=0;i<NO_OF_AXIS;i++){
//				pApp->m_MotorResource.GetIoStatus(pApp->m_nAxes[i],&pDataInEmgStop);
				pApp->motionParameters.GetMotorIOStatus(i, DataIn);

				//OutputDebugLogTo(7,TRUE,"GetIoStatus stop Axis=%d pDataOut = %d ",i,pDataOut);
			}

			//if (!(pDataInEmgStop & IN_MOTOR_EMERGENCY_STOP)) {
			//	pApp->m_bEmergencyStop = FALSE;
			//}
			pApp->m_bEmergencyStop = (pDataInEmgStop >> 4) & 1;

//			OutputDebugLogTo(9,TRUE,"RefreshMotionStart  pApp->m_bEmergencyStop = %d ",pApp->m_bEmergencyStop);
			if(!pApp->m_bEmergencyStop)
			{
				pApp->ShowMessageDlgBox("", false);
				if(pApp->m_bOnline){
					pApp->m_bStream = FALSE;
					pApp->m_bReceivedStartCmd = FALSE;
					SetEvent(pApp->m_EventMotionSensor);
					SetEvent(pApp->m_eventOnline);
					ResetEvent(pApp->m_EventMotionSensorStart);
					OutputDebugLogTo(9,TRUE,"RefreshMotionStop ResetEvent(pApp->m_eventOnline) m_bEmergencyStop = %d m_bOnline=%d",pApp->m_bEmergencyStop,pApp->m_bOnline);
				}
				
			}
		}
		//BOOL bEmergencyStatus = (pData >> 10) & 1;
		Sleep(pApp->m_nSignalScanFrequency);
	}
	return 0;
}

void GetImageFilePaths(CString strStartFolder, CStringArray& arrayFilePaths, CStringArray& arrayFileFolders, BOOL bParseSub,CString strChar)
{
	CString strFilePath;
	strFilePath.Format("%s%s", strStartFolder, bParseSub ? "*.*" : "*.bmp");
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
			arrayFileFolders.Add(strStartFolder);
			GetImageFilePaths(strFolder, arrayFilePaths, arrayFileFolders, bParseSub,strChar);
		}
		else if (!strFile.Right(3).CompareNoCase("bmp"))
		{
			if (!strChar.IsEmpty()) {
				CString strTemp = strFile.Right(6);
				CString strTemp1 = strTemp.Left(1);
				if(strTemp1.CompareNoCase(strChar)==0)
					arrayFilePaths.Add(strStartFolder + strFile);
			}
			else
				arrayFilePaths.Add(strStartFolder + strFile);

		}
	}
}

void GetImageFolderPaths(CString strStartFolder, CStringArray& arrayFileFolders)
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
			arrayFileFolders.Add(strFolder);
		}
	}
}

UINT LogInspectionData(LPVOID pParam)
{
	CApplication* pApp = (CApplication*)AfxGetApp();
	CAppDoc *pDoc = (CAppDoc*)pParam;;
	int nError;

	while (1) {
		WaitForSingleObject(pDoc->m_EventLogInspectionData, INFINITE);
		if (pDoc->m_bExitLogEnry) {
			OutputDebugLogTo(7, TRUE, "LogInspectionData closed");
			return 0;
		}
		for (int nCount = 0; nCount < pDoc->m_strLogInspectionData.GetCount(); nCount++) {
			pApp->AddLogEvent(pDoc->m_strLogInspectionData.GetAt(nCount), 2,FALSE);
		}
		pDoc->m_strLogInspectionData.RemoveAll();
		ResetEvent(pDoc->m_EventLogInspectionData);
	}
	return 0;
}

UINT ResetMotor(LPVOID pParam)
{
	int nError;

	while (1) {

		if (WaitForSingleObject(theApp.m_hResetReceived, INFINITE) != WAIT_OBJECT_0) {
			OutputDebugLogTo(7, TRUE, "------- ERROR IN RESET THREAD MANIPULATION------");
			return 0;
		}

		if (theApp.m_bExitApp)
			return 0;

		ResetEvent(theApp.m_hResetReceived);
		ResetEvent(theApp.m_hResetDone);

		//Reset button
		if (theApp.m_bReceivedResetCmd) {

			CProgressBarDlg *pProgressDlg;
			pProgressDlg = new CProgressBarDlg();

			if (!pProgressDlg->Create(IDD_PROGRESS_DLG, AfxGetMainWnd())) {
				delete pProgressDlg;
				pProgressDlg = NULL;
			}
			else {
				if (pProgressDlg != NULL) {
					pProgressDlg->ShowWindow(TRUE && !theApp.m_bProgressDlgOpen);
					pProgressDlg->CenterWindow();
					pProgressDlg->UpdateWindow();
					pProgressDlg->SetStatus("Machine Resetting....", 0);
				}
			}

			theApp.AddLogEvent("Machine Resetting");

			if (!theApp.motionParameters.IsMotorHomed(X_SERVO_MOTOR)) {

				nError = theApp.motionParameters.HomeMove(X_SERVO_MOTOR);
				if (nError) {
					OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
					if (pProgressDlg != NULL) {
						pProgressDlg->ShowWindow(FALSE);
						//		theApp.m_bProgressDlgOpen = FALSE;
						pProgressDlg->DestroyWindow();
						delete pProgressDlg;
						pProgressDlg = NULL;
					}
					theApp.m_bReceivedResetCmd = FALSE;
					theApp.m_bReceivedResetCmdPrevStatus = FALSE;
					continue;
				}
			}

			if (pProgressDlg != NULL) {
				pProgressDlg->SetStatus("Machine Resetting....", 35);
				pProgressDlg->ShowWindow(TRUE && !theApp.m_bProgressDlgOpen);
			}

			if (!theApp.motionParameters.IsMotorHomed(Y_SERVO_MOTOR)) {

				nError = theApp.motionParameters.HomeMove(Y_SERVO_MOTOR);
				if (nError) {
					OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
					if (pProgressDlg != NULL) {
						pProgressDlg->ShowWindow(FALSE);
						//		theApp.m_bProgressDlgOpen = FALSE;
						pProgressDlg->DestroyWindow();
						delete pProgressDlg;
						pProgressDlg = NULL;
					}
					theApp.m_bReceivedResetCmd = FALSE;
					theApp.m_bReceivedResetCmdPrevStatus = FALSE;
					continue;
				}
			}

			if (pProgressDlg != NULL) {
				pProgressDlg->SetStatus("Machine Resetting....", 45);
				pProgressDlg->ShowWindow(TRUE && !theApp.m_bProgressDlgOpen);
			}

			if (theApp.m_bExitApp) {
				if (pProgressDlg != NULL) {
					pProgressDlg->ShowWindow(FALSE);
					//		theApp.m_bProgressDlgOpen = FALSE;
					pProgressDlg->DestroyWindow();
					delete pProgressDlg;
					pProgressDlg = NULL;
				}
				theApp.m_bReceivedResetCmd = FALSE;
				theApp.m_bReceivedResetCmdPrevStatus = FALSE;
				return 0;
			}

			if (!theApp.motionParameters.IsMotorHomed(Z_SERVO_TOP_MOTOR)) {

				nError = theApp.motionParameters.HomeMove(Z_SERVO_TOP_MOTOR);
				if (nError) {
					OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
					if (pProgressDlg != NULL) {
						pProgressDlg->ShowWindow(FALSE);
						//		theApp.m_bProgressDlgOpen = FALSE;
						pProgressDlg->DestroyWindow();
						delete pProgressDlg;
						pProgressDlg = NULL;
					}
					theApp.m_bReceivedResetCmd = FALSE;
					theApp.m_bReceivedResetCmdPrevStatus = FALSE;
					continue;
				}
			}


			if (!theApp.motionParameters.IsMotorHomed(Z_SERVO_BOTTOM_MOTOR)) {

				nError = theApp.motionParameters.HomeMove(Z_SERVO_BOTTOM_MOTOR);
				if (nError) {
					OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
					if (pProgressDlg != NULL) {
						pProgressDlg->ShowWindow(FALSE);
						//		theApp.m_bProgressDlgOpen = FALSE;
						pProgressDlg->DestroyWindow();
						delete pProgressDlg;
						pProgressDlg = NULL;
					}
					theApp.m_bReceivedResetCmd = FALSE;
					theApp.m_bReceivedResetCmdPrevStatus = FALSE;
					continue;
				}
			}

			if (pProgressDlg != NULL) {
				pProgressDlg->SetStatus("Machine Resetting....", 55);
				pProgressDlg->ShowWindow(TRUE && !theApp.m_bProgressDlgOpen);
			}

			if (!theApp.motionParameters.IsMotorHomed(X_SERVO_MOTOR)) {

				nError = theApp.motionParameters.WaitForMotionDone(X_SERVO_MOTOR, theApp.getMotionTimeOut());
				if (nError) {
					OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
					if (pProgressDlg != NULL) {
						pProgressDlg->ShowWindow(FALSE);
						//		theApp.m_bProgressDlgOpen = FALSE;
						pProgressDlg->DestroyWindow();
						delete pProgressDlg;
						pProgressDlg = NULL;
					}
					theApp.m_bReceivedResetCmd = FALSE;
					theApp.m_bReceivedResetCmdPrevStatus = FALSE;
					continue;
				}

				if (theApp.m_bExitApp) break;

				if (pProgressDlg != NULL) {
					pProgressDlg->SetStatus("Machine Resetting....", 70);
					pProgressDlg->ShowWindow(TRUE && !theApp.m_bProgressDlgOpen);
				}

				nError = theApp.motionParameters.HomeDone(X_SERVO_MOTOR);
				if (nError) {
					OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
					if (pProgressDlg != NULL) {
						pProgressDlg->ShowWindow(FALSE);
						//		theApp.m_bProgressDlgOpen = FALSE;
						pProgressDlg->DestroyWindow();
						delete pProgressDlg;
						pProgressDlg = NULL;
					}
					theApp.m_bReceivedResetCmd = FALSE;
					theApp.m_bReceivedResetCmdPrevStatus = FALSE;
					continue;
				}

			}

			if (theApp.m_bExitApp) {
				if (pProgressDlg != NULL) {
					pProgressDlg->ShowWindow(FALSE);
					//		theApp.m_bProgressDlgOpen = FALSE;
					pProgressDlg->DestroyWindow();
					delete pProgressDlg;
					pProgressDlg = NULL;
				}
				theApp.m_bReceivedResetCmd = FALSE;
				theApp.m_bReceivedResetCmdPrevStatus = FALSE;
				return 0;
			}

			if (!theApp.motionParameters.IsMotorHomed(Y_SERVO_MOTOR)) {

				nError = theApp.motionParameters.WaitForMotionDone(Y_SERVO_MOTOR, theApp.getMotionTimeOut());
				if (nError) {
					OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
					if (pProgressDlg != NULL) {
						pProgressDlg->ShowWindow(FALSE);
						//		theApp.m_bProgressDlgOpen = FALSE;
						pProgressDlg->DestroyWindow();
						delete pProgressDlg;
						pProgressDlg = NULL;
					}
					theApp.m_bReceivedResetCmd = FALSE;
					theApp.m_bReceivedResetCmdPrevStatus = FALSE;
					continue;
				}

				if (theApp.m_bExitApp) break;

				if (pProgressDlg != NULL) {
					pProgressDlg->SetStatus("Machine Resetting....", 80);
					pProgressDlg->ShowWindow(TRUE && !theApp.m_bProgressDlgOpen);
				}

				nError = theApp.motionParameters.HomeDone(Y_SERVO_MOTOR);
				if (nError) {
					OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
					if (pProgressDlg != NULL) {
						pProgressDlg->ShowWindow(FALSE);
						//		theApp.m_bProgressDlgOpen = FALSE;
						pProgressDlg->DestroyWindow();
						delete pProgressDlg;
						pProgressDlg = NULL;
					}
					theApp.m_bReceivedResetCmd = FALSE;
					theApp.m_bReceivedResetCmdPrevStatus = FALSE;
					continue;
				}

			}

			if (!theApp.motionParameters.IsMotorHomed(Z_SERVO_TOP_MOTOR)) {

				nError = theApp.motionParameters.WaitForMotionDone(Z_SERVO_TOP_MOTOR, theApp.getMotionTimeOut());
				if (nError) {
					OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
					if (pProgressDlg != NULL) {
						pProgressDlg->ShowWindow(FALSE);
						//		theApp.m_bProgressDlgOpen = FALSE;
						pProgressDlg->DestroyWindow();
						delete pProgressDlg;
						pProgressDlg = NULL;
					}
					theApp.m_bReceivedResetCmd = FALSE;
					theApp.m_bReceivedResetCmdPrevStatus = FALSE;
					continue;
				}

				if (theApp.m_bExitApp) break;

				if (pProgressDlg != NULL) {
					pProgressDlg->SetStatus("Machine Resetting....", 90);
					pProgressDlg->ShowWindow(TRUE && !theApp.m_bProgressDlgOpen);
				}

				nError = theApp.motionParameters.HomeDone(Z_SERVO_TOP_MOTOR);
				if (nError) {
					OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
					if (pProgressDlg != NULL) {
						pProgressDlg->ShowWindow(FALSE);
						//		theApp.m_bProgressDlgOpen = FALSE;
						pProgressDlg->DestroyWindow();
						delete pProgressDlg;
						pProgressDlg = NULL;
					}
					theApp.m_bReceivedResetCmd = FALSE;
					theApp.m_bReceivedResetCmdPrevStatus = FALSE;
					continue;
				}

			}

			if (!theApp.motionParameters.IsMotorHomed(Z_SERVO_BOTTOM_MOTOR)) {

				nError = theApp.motionParameters.WaitForMotionDone(Z_SERVO_BOTTOM_MOTOR, theApp.getMotionTimeOut());
				if (nError) {
					OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
					if (pProgressDlg != NULL) {
						pProgressDlg->ShowWindow(FALSE);
						//		theApp.m_bProgressDlgOpen = FALSE;
						pProgressDlg->DestroyWindow();
						delete pProgressDlg;
						pProgressDlg = NULL;
					}
					theApp.m_bReceivedResetCmd = FALSE;
					theApp.m_bReceivedResetCmdPrevStatus = FALSE;
					continue;
				}

				if (theApp.m_bExitApp) break;

				if (pProgressDlg != NULL) {
					pProgressDlg->SetStatus("Machine Resetting....", 90);
					pProgressDlg->ShowWindow(TRUE && !theApp.m_bProgressDlgOpen);
				}

				nError = theApp.motionParameters.HomeDone(Z_SERVO_BOTTOM_MOTOR);
				if (nError) {
					OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
					if (pProgressDlg != NULL) {
						pProgressDlg->ShowWindow(FALSE);
						//		theApp.m_bProgressDlgOpen = FALSE;
						pProgressDlg->DestroyWindow();
						delete pProgressDlg;
						pProgressDlg = NULL;
					}
					theApp.m_bReceivedResetCmd = FALSE;
					theApp.m_bReceivedResetCmdPrevStatus = FALSE;
					continue;
				}

			}

			SetEvent(theApp.m_hResetDone);
			ResetEvent(theApp.m_hInitPositionDone);
			SetEvent(theApp.m_hInitPosition);
			ResetEvent(theApp.m_hResetReceived);
			theApp.EnableMachineResetOperation(FALSE);

			theApp.m_bReceivedResetCmd = FALSE;
			theApp.m_bReceivedResetCmdPrevStatus = FALSE;

			OutputDebugLogTo(7, TRUE, "----- Machine RESET Completed");
			theApp.AddLogEvent("Machine RESET Completed");

			if (pProgressDlg != NULL) {
				pProgressDlg->SetStatus("Machine RESET Completed...", 100);
				//		theApp.m_bProgressDlgOpen = FALSE;
				pProgressDlg->ShowWindow(FALSE);
				pProgressDlg->DestroyWindow();
				delete pProgressDlg;
				pProgressDlg = NULL;
			}

		}

	}

	return 0;
}

UINT StartScanPosition(LPVOID pParam)
{
	CPerformanceTimer timerStartScanPosition;
	while (1) {
		if (WaitForSingleObject(theApp.m_hInitPosition, INFINITE) != WAIT_OBJECT_0) {
			OutputDebugLogTo(7, TRUE, "------- ERROR IN RESET THREAD MANIPULATION------");
			return 0;
		}
		timerStartScanPosition.Reset();

		OutputDebugLogTo(7, TRUE, "------- Inital positions------");

		ResetEvent(theApp.m_hInitPosition);

		if (theApp.m_bExitApp)
			return 0;

		int nError = theApp.motionParameters.StopAxis(X_SERVO_MOTOR);
		if (nError) {
			OutputDebugLogTo(7, TRUE, "Failed to stop X motor");
			SetEvent(theApp.m_hInitPositionDone);
			continue;
		}

		nError = theApp.motionParameters.StopAxis(Y_SERVO_MOTOR);
		if (nError) {
			OutputDebugLogTo(7, TRUE, "Failed to stop Y motor");
			SetEvent(theApp.m_hInitPositionDone);
			continue;
		}

		nError = theApp.motionParameters.StopAxis(Z_SERVO_TOP_MOTOR);
		if (nError) {
			OutputDebugLogTo(7, TRUE, "Failed to stop Z Top motor");
			SetEvent(theApp.m_hInitPositionDone);
			continue;
		}

		nError = theApp.motionParameters.StopAxis(Z_SERVO_BOTTOM_MOTOR);
		if (nError) {
			OutputDebugLogTo(7, TRUE, "Failed to stop Z Bottom motor");
			SetEvent(theApp.m_hInitPositionDone);
			continue;
		}

		int nPosX, nPosY;
		int Axis[2]; 
		int nPosZ, nPosZB;;
		int nAxisZ[2];

		nError = theApp.motionParameters.getMotorStartScanPosition(X_SERVO_MOTOR, nPosX);
		if (nError) {
			OutputDebugLogTo(7, TRUE, "Failed to get scan position for X motor");
			SetEvent(theApp.m_hInitPositionDone);
			continue;
		}

		nError = theApp.motionParameters.AbsoluteMove(X_SERVO_MOTOR, nPosX, theApp.motionParameters.getScanningSpeed(X_SERVO_MOTOR), false);
		if (nError) {
			OutputDebugLogTo(7, TRUE, "Scanning movement failed for X motor %d", nPosX);
		//	continue;
		}

		nError = theApp.motionParameters.getMotorStartScanPosition(Y_SERVO_MOTOR, nPosY);
		if (nError) {
			OutputDebugLogTo(7, TRUE, "Failed to get scan position for Y motor %d", nPosY);
			SetEvent(theApp.m_hInitPositionDone);
			continue;
		}

		nError = theApp.motionParameters.AbsoluteMove(Y_SERVO_MOTOR, nPosY, theApp.motionParameters.getScanningSpeed(Y_SERVO_MOTOR), false);
		if (nError) {
			OutputDebugLogTo(7, TRUE, "Scanning movement failed for Y motor %d", nPosY);
		//	continue;
		}

		nError = theApp.motionParameters.getMotorStartScanPosition(Z_SERVO_TOP_MOTOR, nPosZ);
		if (nError) {
			OutputDebugLogTo(7, TRUE, "Failed to get scan position for Z Top motor");
			SetEvent(theApp.m_hInitPositionDone);
			continue;
		}

		nError = theApp.motionParameters.AbsoluteMove(Z_SERVO_TOP_MOTOR, nPosZ, theApp.motionParameters.getScanningSpeed(Z_SERVO_TOP_MOTOR), false);
		if (nError) {
			OutputDebugLogTo(7, TRUE, "Scanning movement failed for Z Top motor %d", nPosZ);
//			continue;
		}

		nError = theApp.motionParameters.getMotorStartScanPosition(Z_SERVO_BOTTOM_MOTOR, nPosZB);
		if (nError) {
			OutputDebugLogTo(7, TRUE, "Failed to get scan position for Z Bottom motor");
			SetEvent(theApp.m_hInitPositionDone);
			continue;
		}

		nError = theApp.motionParameters.AbsoluteMove(Z_SERVO_BOTTOM_MOTOR, nPosZB, theApp.motionParameters.getScanningSpeed(Z_SERVO_BOTTOM_MOTOR), false);
		if (nError) {
			OutputDebugLogTo(7, TRUE, "Scanning movement failed for Z Bottom motor %d", nPosZB);
//			continue;
		}

		OutputDebugLogTo(5, TRUE, "Start Scan Position Complete Time: %.3f", timerStartScanPosition.GetElapsedMSec());

		SetEvent(theApp.m_hInitPositionDone);
		ResetEvent(theApp.m_hInitPosition);
	}

	return 0;
}

UINT TrackAutoFocusing(LPVOID pParam)
{
	SetEvent(theApp.m_EventMotionSensor);

	int nLoadedDataPercentage = 0;
	if (theApp.m_pProgressDlg != NULL) {
		theApp.m_pProgressDlg->ShowWindow(SW_SHOWNA);
		theApp.m_bProgressDlgOpen = TRUE;
		theApp.m_pProgressDlg->SetStatus("Auto Focusing Process...", nLoadedDataPercentage);
	}

	int nTrack = 0, nDoc = 0;
	COpticsDlg* pFocusingToolDlg = (COpticsDlg*)pParam;
	nTrack = pFocusingToolDlg->m_nCurrentTrack;
	nDoc = pFocusingToolDlg->m_nCurrentDoc;
	int nMotorAxiz = (nTrack == STATION_TOP) ? Z_SERVO_TOP_MOTOR : Z_SERVO_BOTTOM_MOTOR;


	int nLoadedDataPercentageStepSize = 100 / (2 * pFocusingToolDlg->m_nAutoFocusStepNo);
	int nCurrentZPos = 0;
	theApp.motionParameters.GetCurrentMotorPosition(nMotorAxiz, nCurrentZPos);

	//Move up/down m_nAutoFocusStepNo times
	double dMotorResolution = 0;
	theApp.motionParameters.GetMotorResolution(nMotorAxiz, dMotorResolution);

	int nError = 0;
	std::map<int, double> mapZFocus;

	//Get Focusing Value of current position
	int nGrabCount = 1, nFocusRoiCount = 1;
	int nDummyT, nDummyB;
	pFocusingToolDlg->GetStats(nFocusRoiCount, nGrabCount, nDummyT, nDummyB);

	OutputDebugLogTo(8, TRUE, "TrackAutoFocusing For Track %d, Doc %d, nGrabCount %d, nFocusRoiCount %d", nTrack, nDoc, nGrabCount, nFocusRoiCount);

	if (theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_bLiveImageSingle) {
		theApp.m_pTracks[nTrack].m_pDoc[nDoc]->CamLiveSingle();
	}

	//Refresh Light For Single Step Light Mode
	theApp.m_pTracks[nTrack].RefreshLight(nDoc);

	int nGrabSuccessCount = 0;
	double dAverageFocusGrabbed = 0;
	for (int nGrab = 0; nGrab < nGrabCount; nGrab++) {
		//theApp.m_pTracks[nTrack].RefreshLight(nDoc);

		theApp.m_pTracks[nTrack].m_nOpticsRecalcMode = 1;
		nError = theApp.m_pTracks[nTrack].EnqueueAsyncGrab();
		if (!nError) {
			theApp.m_pTracks[nTrack].UpdateGrabImage(nDoc);

			double dMean, dNoise, dFocus;
			double dAvgFocus = 0;
			for (int nFocusRoi = 0; nFocusRoi < nFocusRoiCount; nFocusRoi++) {
				OpticsCharacteristics(&theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer, &theApp.m_pTracks[nTrack].m_rectOptics[nFocusRoi], dMean, dNoise, dFocus);
				dAvgFocus += dFocus;
			}
			dAvgFocus = dAvgFocus / nFocusRoiCount;

			dAverageFocusGrabbed += dAvgFocus;
			nGrabSuccessCount++;
		}
	}
	if (nGrabSuccessCount > 0)
		dAverageFocusGrabbed = dAverageFocusGrabbed / nGrabSuccessCount;

	dAverageFocusGrabbed = dAverageFocusGrabbed * 100;
	OutputDebugLogTo(8, TRUE, "Auto Focusing - Z: %d, Focus: %.5f - Optic Focus: %.5f", nCurrentZPos, dAverageFocusGrabbed, pFocusingToolDlg->m_dOpticsFocus[0]);
	mapZFocus.insert(std::pair<int, double>(nCurrentZPos, dAverageFocusGrabbed));

	//Move Up
	for (int nStep = 0; nStep < pFocusingToolDlg->m_nAutoFocusStepNo; nStep++) {
		int nNextZPos = nCurrentZPos + (nStep + 1)*pFocusingToolDlg->m_dAutoFocusStepSizeMM * dMotorResolution;

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
		theApp.motionParameters.WaitMotorSettlingTime(nMotorAxiz);

		//Get Focusing Value
		int nGrabSuccessCount = 0;
		double dAverageFocusGrabbed = 0;
		for (int nGrab = 0; nGrab < nGrabCount; nGrab++) {
			//theApp.m_pTracks[nTrack].RefreshLight(nDoc);

			theApp.m_pTracks[nTrack].m_nOpticsRecalcMode = 1;
			nError = theApp.m_pTracks[nTrack].EnqueueAsyncGrab();
			if (!nError) {
				theApp.m_pTracks[nTrack].UpdateGrabImage(nDoc);

				double dMean, dNoise, dFocus;
				double dAvgFocus = 0;
				for (int nFocusRoi = 0; nFocusRoi < nFocusRoiCount; nFocusRoi++) {
					OpticsCharacteristics(&theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer, &theApp.m_pTracks[nTrack].m_rectOptics[nFocusRoi], dMean, dNoise, dFocus);
					dAvgFocus += dFocus;
				}
				dAvgFocus = dAvgFocus / nFocusRoiCount;

				dAverageFocusGrabbed += dAvgFocus;
				nGrabSuccessCount++;
			}
		}
		if (nGrabSuccessCount > 0)
			dAverageFocusGrabbed = dAverageFocusGrabbed / nGrabSuccessCount;

		dAverageFocusGrabbed = dAverageFocusGrabbed * 100;
		OutputDebugLogTo(8, TRUE, "Auto Focusing - Z: %d, Focus: %.5f - Optic Focus: %.5f", nNextZPos, dAverageFocusGrabbed, pFocusingToolDlg->m_dOpticsFocus[0]);
		mapZFocus.insert(std::pair<int, double>(nNextZPos, dAverageFocusGrabbed));

		if (theApp.m_pProgressDlg != NULL) {
			nLoadedDataPercentage = nLoadedDataPercentage + nLoadedDataPercentageStepSize;

			theApp.m_pProgressDlg->ShowWindow(TRUE);
			theApp.m_bProgressDlgOpen = TRUE;
			theApp.m_pProgressDlg->UpdateWindow();
			theApp.m_pProgressDlg->SetStatus("Auto Focusing Process...", nLoadedDataPercentage);
		}
	}

	nLoadedDataPercentage = 50;

	//Move down
	for (int nStep = 0; nStep < pFocusingToolDlg->m_nAutoFocusStepNo; nStep++) {
		int nNextZPos = nCurrentZPos - (nStep + 1)*pFocusingToolDlg->m_dAutoFocusStepSizeMM * dMotorResolution;

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
		theApp.motionParameters.WaitMotorSettlingTime(nMotorAxiz);

		//Get Focusing Value
		int nGrabSuccessCount = 0;
		double dAverageFocusGrabbed = 0;
		for (int nGrab = 0; nGrab < nGrabCount; nGrab++) {
			theApp.m_pTracks[nTrack].m_nOpticsRecalcMode = 1;

			//theApp.m_pTracks[nTrack].RefreshLight(nDoc);

			nError = theApp.m_pTracks[nTrack].EnqueueAsyncGrab();
			if (!nError) {
				theApp.m_pTracks[nTrack].UpdateGrabImage(nDoc);

				double dMean, dNoise, dFocus;
				double dAvgFocus = 0;
				for (int nFocusRoi = 0; nFocusRoi < nFocusRoiCount; nFocusRoi++) {
					OpticsCharacteristics(&theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer, &theApp.m_pTracks[nTrack].m_rectOptics[nFocusRoi], dMean, dNoise, dFocus);
					dAvgFocus += dFocus;
				}
				dAvgFocus = dAvgFocus / nFocusRoiCount;

				dAverageFocusGrabbed += dAvgFocus;
				nGrabSuccessCount++;
			}
		}

		if (nGrabSuccessCount > 0)
			dAverageFocusGrabbed = dAverageFocusGrabbed / nGrabSuccessCount;

		dAverageFocusGrabbed = dAverageFocusGrabbed * 100;
		OutputDebugLogTo(8, TRUE, "Auto Focusing - Z: %d, Focus: %.5f - Optic Focus: %.5f", nNextZPos, dAverageFocusGrabbed, pFocusingToolDlg->m_dOpticsFocus[0]);
		mapZFocus.insert(std::pair<int, double>(nNextZPos, dAverageFocusGrabbed));

		if (theApp.m_pProgressDlg != NULL) {
			nLoadedDataPercentage = nLoadedDataPercentage + nLoadedDataPercentageStepSize;

			theApp.m_pProgressDlg->ShowWindow(TRUE);
			theApp.m_bProgressDlgOpen = TRUE;
			theApp.m_pProgressDlg->UpdateWindow();
			theApp.m_pProgressDlg->SetStatus("Auto Focusing Process...", nLoadedDataPercentage);
		}
	}

	//Find the best Z focus value
	double dBestFocus = 0;
	int nBestZFocus = 0;
	for (std::map<int, double>::iterator it = mapZFocus.begin(); it != mapZFocus.end(); it++) {
		if (it->second > dBestFocus) {
			dBestFocus = it->second;
			nBestZFocus = it->first;
		}
	}

	if (theApp.m_pProgressDlg != NULL) {
		theApp.m_pProgressDlg->SetStatus("Auto Focusing Process...", 100);
		theApp.m_pProgressDlg->ShowWindow(FALSE);
		theApp.m_bProgressDlgOpen = FALSE;
		theApp.m_pProgressDlg->m_nCurrentPos = 0;
	}

	bool bMoveToBestZFocusPosition = false;
	if (dBestFocus > 0) {
		CString strMsg;
		strMsg.Format("Best focus is at Z position %d (Focus value: %.2f).\nDo you want to move to best focus position?", nBestZFocus, dBestFocus);
		bMoveToBestZFocusPosition = AfxMessageBox(strMsg, MB_YESNO | MB_ICONQUESTION) == IDYES;
	}
	else {
		AfxMessageBox("Can not found the focusing area!");
	}

	if (bMoveToBestZFocusPosition) {
		nError = theApp.motionParameters.AbsoluteMove(nMotorAxiz, nBestZFocus, theApp.motionParameters.getMovingSpeed(nMotorAxiz));
		if (nError) {
			OutputDebugLogTo(8, TRUE, "Auto Focusing - Failed to move motor axis %d Error %d", nMotorAxiz);
		}

		if (!nError) {
			nError = theApp.motionParameters.WaitForMotionDone(nMotorAxiz, theApp.getMotionTimeOut());
			if (nError) {
				OutputDebugLogTo(8, TRUE, "Auto Focusing - CMotionControlDialog::Movement[Absolute] Axis%d Error %d", nMotorAxiz, nError);
			}
			else {
				theApp.motionParameters.WaitMotorSettlingTime(nMotorAxiz);
				//theApp.m_pTracks[nTrack].RefreshLight(nDoc);
				nError = theApp.m_pTracks[nTrack].EnqueueAsyncGrab();
				if (!nError) {
					theApp.m_pTracks[nTrack].UpdateGrabImage(nDoc);
				}
			}
		}
	}
	else {
		//Move back previous Z Position
		nError = theApp.motionParameters.AbsoluteMove(nMotorAxiz, nCurrentZPos, theApp.motionParameters.getMovingSpeed(nMotorAxiz));
		if (nError) {
			OutputDebugLogTo(8, TRUE, "Auto Focusing - Failed to move motor axis %d Error %d", nMotorAxiz);
		}

		nError = theApp.motionParameters.WaitForMotionDone(nMotorAxiz, theApp.getMotionTimeOut());
		if (nError) {
			OutputDebugLogTo(8, TRUE, "Auto Focusing - CMotionControlDialog::Movement[Absolute] Axis%d Error %d", nMotorAxiz, nError);
		}
	}

	////Refresh Light Step Mode Setting
	//SetEvent(theApp.m_EventRefreshLightStepMode);
	//WaitForSingleObject(theApp.m_EventRefreshLightStepModeDone, INFINITE);

	pFocusingToolDlg->m_bAutoFocusing = TRUE;

	ResetEvent(theApp.m_EventMotionSensor);

	return 0;
}
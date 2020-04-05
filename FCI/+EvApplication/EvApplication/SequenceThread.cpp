#include "stdafx.h"
#include <direct.h>
#include "SequenceThread.h"
#include "Application.h"
#include "DebugLog.h"
#include "TrackHandler.h"
#include "ImageSelectionDlg.h"
#include "AppDoc.h"
#include "CameraCode.h"
//#include "GlobalBackgroundThread.h"
#include "MathValidations.h"
//#include "ExternalFunctions.h"
#include "BitmapFunctions.h"
#include <numeric>
#include <queue>
#include "namespace_utilities_function.h"
#include "CalibrationSettings.h"
#include "PVIInspectionParameters.h"
#include "ImageQueueHandler.h"

UINT InspectOnlineThread(LPVOID pParam) {
	try {
		CApplication* pApp = (CApplication*)AfxGetApp();
		CTrackHandler* pTrack = (CTrackHandler*)pParam;
		CAppDoc* pDoc = pTrack->m_pDoc[0];
		int nTrackIndex = pTrack->m_nTrack;

		const unsigned char threadId = static_cast<unsigned char>(pTrack->getInspectionThreadId());

		int nFOV = -1;
		int nDevice = -1;
		while (1) {
			if (pApp->m_bExitApp)
				break;

			if (WaitForSingleObject(pTrack->eventDoInspection[threadId], INFINITE) != WAIT_OBJECT_0) {
				OutputDebugLogTo(0, TRUE, "----- ERROR : Thread manipulation, Inspection Thread has been closed");
				break;
			}

			if (pApp->m_bExitApp)
				break;

			ResetEvent(pTrack->eventInspectionDone[threadId]);

			bool bScannedQueueEmpty = false;

			while (!bScannedQueueEmpty) {
				if (pApp->m_bExitApp)
					break;

				EnterCriticalSection(&pTrack->CriticalSectionScannedImage);
				bScannedQueueEmpty = pTrack->grabbedImageId[threadId].empty();
				
				if (bScannedQueueEmpty) {
					SetEvent(pTrack->eventInspectionDone[threadId]);
					ResetEvent(pTrack->eventDoInspection[threadId]);

					//Clear memory
					CInspectionHandler* pInspHandler = &pApp->m_pTracks[0].m_pDoc[0]->inspection(0);
					pInspHandler->m_bClearThreadCache = TRUE;
					pInspHandler->ReleaseMemory();

					LeaveCriticalSection(&pTrack->CriticalSectionScannedImage);

					break;
				}
				 
				nFOV = pTrack->grabbedImageId[threadId].front();
				pTrack->grabbedImageId[threadId].pop();

				nDevice = pTrack->grabbedDeviceId[threadId].front();
				pTrack->grabbedDeviceId[threadId].pop();

				LeaveCriticalSection(&pTrack->CriticalSectionScannedImage);

				if (theApp.m_bExitApp)
					break;

				pTrack->m_pDoc[0]->InspectAllDocOnline(nDevice, nFOV);
				OutputDebugLogTo(0, TRUE, "[Die%d Track%d Fov%d] InspectAllDocOnline: Done at Thread Id: %d", nDevice, pTrack->m_nTrack, nFOV, threadId);

				//Send Defect Info
				if (theApp.m_bCentralizedVerificationMode && !theApp.m_CentralizedCommInterface.m_bIsTCPServer) {
					int nView;
					for (int nDoc = 0; nDoc < pApp->m_nTotalViewDocs[pTrack->m_nTrack]; nDoc++) {
						theApp.motionParameters.GetViewID(pTrack->m_nTrack, nDoc, nView);
						if (pApp->m_nErrorCode[nDevice][nFOV][nView] < GOOD_UNIT && pApp->m_nErrorCode[nDevice][nFOV][nView] != NOT_INSPECTED) {
							pApp->m_bIsFailPart = TRUE;
						}
					}

					for (int nDoc = 0; nDoc < pApp->m_nTotalViewDocs[pTrack->m_nTrack]; nDoc++) {
						if (!theApp.motionParameters.IsGrabEnabled(pTrack->m_nTrack, nDoc, nFOV))
							continue;

						CInspectionHandler* pInspHandler = &pApp->m_pTracks[pTrack->m_nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFOV];
						pApp->m_pImageQueue->PushDefectInfoCentralized(pTrack->m_nTrack, nDoc, nDevice, nFOV, pInspHandler->m_Inspection.m_DefectData[nDoc].arrayDefectCenters, pInspHandler->m_Inspection.m_DefectData[nDoc].arrayDefectRects,
							pInspHandler->m_Inspection.m_DefectData[nDoc].arrayDefectX, pInspHandler->m_Inspection.m_DefectData[nDoc].arrayDefectY);
					}
				}

				//SetEvent(pTrack->eventInspectionDone[threadId]);
			}

		}
		SetEvent(pTrack->eventInspectionDone[threadId]);
	}
	catch (...) {
		OutputDebugLogTo(0,FALSE, "-----ERROR: Unknown exception occured in image inspection thread");
	}

	return 0;
}
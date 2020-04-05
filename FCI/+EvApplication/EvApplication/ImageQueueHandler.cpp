/// Udupa; Image Queue Handler to manage and save images in a queue
/// Udupa; Dec'2013; Defect overlay queuing option

#include "stdafx.h"
#include "Application.h"
#include "ImageQueueHandler.h"
#include "DisplayOverlay.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include <direct.h>
#include "namespace_utilities_function.h"
#include "ImageSelectionDlg.h"
#include "Application.h"
#include "CentralizedCommInterface.h"
#include "CentralizedCommManager.h"
#include "CentralizedCommMonitor.h"

CImageQueueHandler::CImageQueueHandler()
{
}

CImageQueueHandler::~CImageQueueHandler()
{
	m_bActive = FALSE;
	SetEvent(m_hPushed);
	SetEvent(m_hPushedDL);
	SetEvent(m_hPushedImageCentralized);
	SetEvent(m_hPushedResultCentralized);
	SetEvent(m_hPushedDefectInfoCentralized);
	SetEvent(m_hPushedStationQueueCentralized);
	
	if(WaitForSingleObject(m_hImageSaveThread, 1000) != WAIT_OBJECT_0)
		TerminateThread(m_hImageSaveThread, 0);

	SetEvent(m_hPushedInspect);
	if (WaitForSingleObject(m_hImageInspectThread, 1000) != WAIT_OBJECT_0)
		TerminateThread(m_hImageInspectThread, 0);

	DeleteCriticalSection(&m_csQueue);
	DeleteCriticalSection(&m_csImageCentralizedQueue);
	DeleteCriticalSection(&m_csResultCentralizedQueue);
	DeleteCriticalSection(&m_csDefectInfoCentralizedQueue);
	//delete[] m_strImageFolders;
	CloseHandle(m_hPushed);
	CloseHandle(m_hPushedInspect);
	CloseHandle(m_hPushedDL);
	CloseHandle(m_hPushedImageCentralized);
	CloseHandle(m_hPushedResultCentralized);
	CloseHandle(m_hPushedDefectInfoCentralized);
	CloseHandle(m_hPushedStationQueueCentralized);
}

int CImageQueueHandler::Initialize(CSize szImage)
{
	m_pApp = (CApplication*) AfxGetApp();
	int nTracks = m_pApp->m_nTotalTracks;

	m_bActive = TRUE;
	m_hPushed = CreateEventA(NULL, FALSE, FALSE, NULL);
	m_hPushedDL = CreateEventA(NULL, FALSE, FALSE, NULL);
	m_hPushedImageCentralized = CreateEventA(NULL, FALSE, FALSE, NULL);
	m_hPushedResultCentralized = CreateEventA(NULL, FALSE, FALSE, NULL);
	m_hPushedDefectInfoCentralized = CreateEventA(NULL, FALSE, FALSE, NULL);
	m_hPushedStationQueueCentralized = CreateEventA(NULL, FALSE, FALSE, NULL);

	m_hPushedInspect = CreateEventA(NULL, FALSE, FALSE, NULL);
	InitializeCriticalSection(&m_csQueue);
	InitializeCriticalSection(&m_csImageCentralizedQueue);
	InitializeCriticalSection(&m_csResultCentralizedQueue);
	InitializeCriticalSection(&m_csDefectInfoCentralizedQueue);

//	m_hImageSaveThread = m_pApp->m_ThreadManager.CreateThread(ImageSaveThread, this, IMAGE_SAVE_THREAD);

	m_hImageSaveThread = AfxBeginThread(ImageSaveThread, this, THREAD_PRIORITY_LOWEST)->m_hThread;
	SetThreadAffinityMask(m_hImageSaveThread, 0x80); 

	m_hImageSaveThreadDL = AfxBeginThread(ImageSaveThreadDL, this, THREAD_PRIORITY_LOWEST)->m_hThread;
	SetThreadAffinityMask(m_hImageSaveThreadDL, 0x80);

	m_hImageInspectThread = AfxBeginThread(ImageInspectThread, this, THREAD_PRIORITY_LOWEST)->m_hThread;
	SetThreadAffinityMask(m_hImageInspectThread, 0x80);

	m_hImageCentralizedSendThread = AfxBeginThread(ImageCentralizedSendThread, this, THREAD_PRIORITY_LOWEST)->m_hThread;
	SetThreadAffinityMask(m_hImageCentralizedSendThread, 0x80);

	m_hResultCentralizedSendThread = AfxBeginThread(ResultCentralizedSendThread, this, THREAD_PRIORITY_LOWEST)->m_hThread;
	SetThreadAffinityMask(m_hResultCentralizedSendThread, 0x80);

	m_hDefectInfoCentralizedSendThread = AfxBeginThread(DefectInfoCentralizedSendThread, this, THREAD_PRIORITY_LOWEST)->m_hThread;
	SetThreadAffinityMask(m_hDefectInfoCentralizedSendThread, 0x80);

	m_hCentralizedQueueSystemThread = AfxBeginThread(CentralizedQueueHandlingThread, this, THREAD_PRIORITY_LOWEST)->m_hThread;
	SetThreadAffinityMask(m_hCentralizedQueueSystemThread, 0x80);

	//// create insp thread
	
	m_BufDefect.Allocate(szImage, true, TRUE);
	m_BufDefect.ClrImg();

	return 0;
}

int CImageQueueHandler::FlushQueueBuffers()
{
	EnterCriticalSection(&m_csQueue);
	while(!m_BackImageQueue.empty()) {
		CQueueBuffer* pBuf = m_BackImageQueue.front();
		m_BackImageQueue.pop();
		delete pBuf;
	}
	LeaveCriticalSection(&m_csQueue);

	return 0;
}

int CImageQueueHandler::FlushQueueResultCentralized()
{
	EnterCriticalSection(&m_csResultCentralizedQueue);
	while (!m_ResultQueueCentralized.empty()) {
		m_ResultQueueCentralized.front();
		m_ResultQueueCentralized.pop();
	}
	LeaveCriticalSection(&m_csResultCentralizedQueue);

	return 0;
}

int CImageQueueHandler::FlushQueueDefectInfoCentralized()
{
	EnterCriticalSection(&m_csDefectInfoCentralizedQueue);
	while (!m_BackupDefectInfoQueueCentralized.empty()) {
		CQueueDefectInfoCentralized* pBuf = m_BackupDefectInfoQueueCentralized.front();
		m_BackupDefectInfoQueueCentralized.pop();
		delete pBuf;
	}
	LeaveCriticalSection(&m_csDefectInfoCentralizedQueue);

	return 0;
}

int CImageQueueHandler::FlushQueueBufferCentralized()
{
	EnterCriticalSection(&m_csImageCentralizedQueue);
	while (!m_BackupImageQueueCentralized.empty()) {
		CQueueBufferCentralized* pBuf = m_BackupImageQueueCentralized.front();
		m_BackupImageQueueCentralized.pop();
		delete pBuf;
	}
	LeaveCriticalSection(&m_csImageCentralizedQueue);

	return 0;
}

int CImageQueueHandler::FlushQueueBufferDL()
{
	EnterCriticalSection(&m_csQueue);
	while (!m_ImageQueueDL.empty()) {
		CQueueBuffer* pBuf = m_ImageQueueDL.front();
		m_ImageQueueDL.pop();
		delete pBuf;
	}
	LeaveCriticalSection(&m_csQueue);

	return 0;
}

void CImageQueueHandler::TriggerSendCentralizedResult()
{
	SetEvent(m_hPushedResultCentralized);
}

void CImageQueueHandler::TriggerSendCentralizedImage()
{
	SetEvent(m_hPushedImageCentralized);
}

void CImageQueueHandler::TriggerSendCentralizedDefectInfo()
{
	SetEvent(m_hPushedDefectInfoCentralized);
}

BOOL CImageQueueHandler::ImageQueueCentralizedEmpty()
{
	BOOL isEmptyQueue = TRUE;
	EnterCriticalSection(&m_csImageCentralizedQueue);
	isEmptyQueue = m_ImageQueueCentralized.empty() && m_BackupImageQueueCentralized.empty();
	LeaveCriticalSection(&m_csImageCentralizedQueue);
	return isEmptyQueue;
}

BOOL CImageQueueHandler::ResultQueueCentralizedEmpty()
{
	BOOL isEmptyQueue = TRUE;
	EnterCriticalSection(&m_csResultCentralizedQueue);
	isEmptyQueue = m_ResultQueueCentralized.empty();
	LeaveCriticalSection(&m_csResultCentralizedQueue);
	return isEmptyQueue;
}

BOOL CImageQueueHandler::DefectInfoQueueCentralizedEmpty()
{
	BOOL isEmptyQueue = TRUE;
	EnterCriticalSection(&m_csDefectInfoCentralizedQueue);
	isEmptyQueue = m_DefectInfoQueueCentralized.empty() && m_BackupDefectInfoQueueCentralized.empty();
	LeaveCriticalSection(&m_csDefectInfoCentralizedQueue);
	return isEmptyQueue;
}

int CImageQueueHandler::PushResultCentralized()
{
	EnterCriticalSection(&m_csResultCentralizedQueue);
	m_ResultQueueCentralized.push(TRUE);
	SetEvent(m_hPushedResultCentralized);
	LeaveCriticalSection(&m_csResultCentralizedQueue);
	return 0;
}

int CImageQueueHandler::PushDefectInfoCentralized(int nTrackIndex, int nDocIndex, int nDieIndex, int nFovIndex, std::vector<CPoint>& defectCenters, 
				std::vector<CRect>& defectRects, std::vector<std::vector<int>> defectX, std::vector<std::vector<int>> defectY)
{

	CPerformanceTimer timer;
	timer.Reset();

	EnterCriticalSection(&m_csDefectInfoCentralizedQueue);
	CQueueDefectInfoCentralized* pBuf = new CQueueDefectInfoCentralized(nTrackIndex,
		nDocIndex,
		nDieIndex,
		nFovIndex,
		defectCenters,
		defectRects,
		defectX, defectY);
	m_BackupDefectInfoQueueCentralized.push(pBuf);
	SetEvent(m_hPushedDefectInfoCentralized);
	LeaveCriticalSection(&m_csDefectInfoCentralizedQueue);

	OutputDebugLogTo(6, TRUE, "[Track%d] Push Defect Info Centralized Time: %5.2fms", nTrackIndex, timer.GetElapsedMSec());

	return 0;
}

int CImageQueueHandler::PushImageBufferCentralized(CImageBuffer* pImageBuffer, int nTrackIndex, int nDocIndex, int nDieIndex, int nFovIndex, BOOL bMappingReady)
{

	CPerformanceTimer timer;
	timer.Reset();

	EnterCriticalSection(&m_csImageCentralizedQueue);
	CQueueBufferCentralized* pBuf = new CQueueBufferCentralized(
		pImageBuffer,
		nTrackIndex,
		nDocIndex,
		nDieIndex,
		nFovIndex, 
		bMappingReady);
	m_BackupImageQueueCentralized.push(pBuf);
	SetEvent(m_hPushedImageCentralized);
	LeaveCriticalSection(&m_csImageCentralizedQueue);

	OutputDebugLogTo(6, TRUE, "[Die%d Track%d Doc%d Fov%d] PushImageBufferCentralized Time: %5.2fms", nDieIndex, nTrackIndex, nDocIndex, nFovIndex, timer.GetElapsedMSec());

	return 0;
}

int CImageQueueHandler::PushImageBufferDL(CImageBuffer* pImageBuffer, int nTrackIndex, int nDocIndex, int nDieIndex, int nFovIndex, int nErrorCode, 
	CRectArray& rects, CRegionArray& regions, std::vector<CRect> pRects, std::vector<CPoint> pCenters)
{

	CPerformanceTimer timer;
	timer.Reset();

	EnterCriticalSection(&m_csQueue);
	CQueueBuffer* pBuf = new CQueueBuffer(
		pImageBuffer,
		nTrackIndex,
		nDocIndex,
		nDieIndex,
		nFovIndex,
		nErrorCode,
		rects,
		regions,
		pRects,
		pCenters);
	m_ImageQueueDL.push(pBuf);
	SetEvent(m_hPushedDL);
	LeaveCriticalSection(&m_csQueue);

	OutputDebugLogTo(6, TRUE, "PushImageBufferDL [Track%d] Image Push Time: %5.2fms", nTrackIndex, timer.GetElapsedMSec());

	return 0;
}

int CImageQueueHandler::PushImageBuffer(CImageBuffer* pImageBuffer, int nTrackIndex,int nDocIndex,int nDieIndex,int nFovIndex, int nErrorCode, CRectArray& rects, CRegionArray& regions)
{

	CPerformanceTimer timer;
	timer.Reset();
	std::vector<CRect> defectRects;
	std::vector<CPoint> defectCenters;

	EnterCriticalSection(&m_csQueue);
	CQueueBuffer* pBuf = new CQueueBuffer(
		pImageBuffer, 
		nTrackIndex, 
		nDocIndex,
		nDieIndex,
		nFovIndex,

		nErrorCode,
		rects,
		regions,
		defectRects,
		defectCenters);
	m_BackImageQueue.push(pBuf);
	SetEvent(m_hPushed);
	LeaveCriticalSection(&m_csQueue);

	OutputDebugLogTo(6, TRUE, "PushImageBuffer [Track%d] Image Push Time: %5.2fms", nTrackIndex, timer.GetElapsedMSec());

	return 0;
}

int CImageQueueHandler::PushInspectImageBuffer(CImageBuffer* pImageBuffer, int nTrackIndex, int nDocIndex, int nDieIndex, int nFovIndex)
{

	//CPerformanceTimer timer;
	//timer.Reset();

	//EnterCriticalSection(&m_csQueue);
	//CQueueBuffer* pBuf = new CQueueBuffer(
	//	pImageBuffer,
	//	nTrackIndex,
	//	nDocIndex,
	//	nDieIndex,
	//	nFovIndex);
	//m_BackImageQueue.push(pBuf);

	//SetEvent(m_hPushedInspect);
	//LeaveCriticalSection(&m_csQueue);

	//OutputDebugLogTo(6, TRUE, "[Track%d] Image Push Time: %5.2fms", nTrackIndex, timer.GetElapsedMSec());

	return 0;
}

int CImageQueueHandler::SendCentralizedResult()
{
	return theApp.m_CentralizedCommInterface.m_pCommManager[0]->SendMappingResult(MAPPING_RESULT);
}

int CImageQueueHandler::SendCentralizedImage(CQueueBufferCentralized * pBuf)
{
	if (pBuf->m_bMappingReady) {
		int nSuccess = theApp.m_CentralizedCommInterface.m_pCommManager[0]->m_Monitor.SendCommandMessage(MAPPING_RESULT_READY);
		return !nSuccess;
	}
	else {
		return theApp.m_CentralizedCommInterface.m_pCommManager[0]->SendImageBufferMessage(pBuf->m_nTrack, pBuf->m_nDoc, pBuf->m_nDie, pBuf->m_nFov,
			*pBuf->m_pImageBuffer);
	}
	
}

int CImageQueueHandler::SendCentralizedDefectInfo(CQueueDefectInfoCentralized * pBuf)
{
	return theApp.m_CentralizedCommInterface.m_pCommManager[0]->SendDefectInfoMessage(pBuf->m_nTrack, pBuf->m_nDoc, pBuf->m_nDie, pBuf->m_nFov,
		pBuf->defectCenters, pBuf->defectRects, pBuf->defectX, pBuf->defectY);
}

int CImageQueueHandler::SaveImageDL(CQueueBuffer* pBuf)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	CString strTemp;
	CString strFile, strImageDesc, strDocName;
	BOOL bPass = !IS_FAIL(pBuf->m_nErrorCode);
	switch (pBuf->m_nTrack) {
	case 0:	strDocName = m_pApp->m_strTopCameraName[pBuf->m_nDoc];
		break;
	case 1: strDocName = m_pApp->m_strBottomCameraName[pBuf->m_nDoc];
		break;
	case 2:	strDocName = m_pApp->m_strSideCameraName[pBuf->m_nDoc];
		break;
	}

	//RecipeName_FLexID_P/F(Pass/Fail)_Defect Code_Doc Name_Position
	strImageDesc.Format("%s_%s_%s_%s_%s_Pos%d", m_pApp->m_strConfigFile, m_pApp->m_strPartFlex[pBuf->m_nDie],
		bPass ? "P" : "F", m_pApp->m_pImageSelectionDlg->m_wndDlg.GetDefectName(pBuf->m_nErrorCode),
		strDocName, pBuf->m_nFov + 1);

	if(m_pApp->m_bOnline)
		m_strImageFolders.Format("%s\\Online Images", m_pApp->m_ImageSaveFolderDL);
	else
		m_strImageFolders.Format("%s\\Offline Images", m_pApp->m_ImageSaveFolderDL);

	_mkdir(m_strImageFolders);
	strFile.Format("%s\\%s", m_strImageFolders, strImageDesc);
	
	if (m_pApp->m_bSaveDLImages && ((bPass && m_pApp->m_bSaveDLPassImages ) || (!bPass && m_pApp->m_bSaveDLFailImages))) {
		SaveDefectsEmbeddedBMPFile(strFile + ".bmp", *pBuf->m_pImageBuffer, pBuf->defectRects, pBuf->defectCenters);
		OutputDebugLogTo(5, TRUE, "%s saved!", strFile);
	}

	return 0;
}

int CImageQueueHandler::SaveImage(CQueueBuffer* pBuf)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	CString strTemp;
	CString strFile, strImageDesc;
	strImageDesc.Format("HP_FCI_THA%d_Pos%d_",pBuf->m_nDie+1,pBuf->m_nFov+1);
	//strImageDesc = m_pApp->GetFailedStr(pBuf->m_nErrorCode,FALSE);
	switch(pBuf->m_nTrack){	
	case 0:		strTemp.Format("T%d",pBuf->m_nDoc+1);
			strImageDesc.Append(strTemp);
			break;
	case 1: strTemp.Format("B%d",pBuf->m_nDoc+1);
			strImageDesc.Append(strTemp);
			break;
	case 2:	strTemp.Format("S%d",pBuf->m_nDoc+1);
			strImageDesc.Append(strTemp);
			break;
	}
	m_strImageFolders.Format("%s\\Online Images", m_pApp->m_ImageSaveFolder);
	_mkdir(m_strImageFolders);

	CString strFolder, strRecipeFolder, strImage;
	strRecipeFolder.Format("%s\\%s", m_strImageFolders,m_pApp->m_strConfigFile);
	_mkdir(strRecipeFolder);
	strTemp.Format("%s\\%s", strRecipeFolder, m_pApp->m_strLotInfo);
	_mkdir(strTemp);
	BOOL bPass = !IS_FAIL(pBuf->m_nErrorCode);
	if(!bPass)
		strFolder.Format("%s\\FAIL", strTemp);
	else
		strFolder.Format("%s\\PASS", strTemp);
	_mkdir(strFolder);
	
	strTemp.Format("%s\\%s", strFolder, m_pApp->m_strPartFlex[pBuf->m_nDie]);
	_mkdir(strTemp);
	strFile.Format("%s\\%s",strTemp,strImageDesc);
	if ((bPass && m_pApp->m_bSaveOnlinePassImages && m_pApp->m_bSaveOnlineBMPImages) || (!bPass && m_pApp->m_bSaveOnlineFailImages && m_pApp->m_bSaveOnlineBMPImages)) {
		SaveGrayScaleBMPFile(strFile + ".bmp", *pBuf->m_pImageBuffer);
		OutputDebugLogTo(5, TRUE, "%s saved!", strFile);
	}
	if ((bPass && m_pApp->m_bSaveOnlinePassImages && m_pApp->m_bSaveOnlineJpegImages) || (!bPass && m_pApp->m_bSaveOnlineFailImages && m_pApp->m_bSaveOnlineJpegImages)) {
		DrawResultText(pBuf, &sysTime);
		SaveJPGFile(strFile + ".jpg", m_BufDefect, m_pApp->m_nSaveJpegQualityImages);
	}

	return 0;
}

void CImageQueueHandler::DrawResultText(CQueueBuffer* pBuf, SYSTEMTIME* pSysTime)
{
	LOGPEN TextColor;
	TextColor.lopnStyle = PS_SOLID;
	TextColor.lopnWidth.x = 10;
	TextColor.lopnColor = IS_FAIL(pBuf->m_nErrorCode) ? RGB(50,50,230) : RGB(50,230,50);

	CString strDate;
	strDate.Format("%02u/%02u/%04u  @%02u:%02u:%02u", 
		pSysTime->wMonth, pSysTime->wDay, pSysTime->wYear, 
		pSysTime->wHour, pSysTime->wMinute, pSysTime->wSecond);

	CSize szImage = m_BufDefect.GetImgSize();
	CRect rcROI = CRect(CPoint(), szImage);
	ImgCopy(pBuf->m_pImageBuffer, &rcROI, &m_BufDefect, &rcROI);
	CDC* pDC = m_BufDefect.GetDrawContext();
	if(pDC) {
		DrawOverlayText(pDC, szImage, m_pApp->GetFailedStr(pBuf->m_nErrorCode, FALSE), CPoint(50, 0), TextColor, 100);
		DrawOverlayText(pDC, szImage, strDate, CPoint(szImage.cx-1150, 0), TextColor, 100);
		//DrawOverlayText(pDC, szImage, strText, CPoint(50, m_Buffer.GetImgSize().cy - 150), TextColor, 100);
		DrawDefectRegions(pDC, szImage, pBuf->m_nErrorCode, pBuf->arrRects, pBuf->arrRegions);
		m_BufDefect.ReleaseDrawContext();
	}
}

UINT ResultCentralizedSendThread(LPVOID pParam)
{
	CPerformanceTimer timer;

	CImageQueueHandler* queueHandler = (CImageQueueHandler*)pParam;
	queue<BOOL>* pResultQueue = &queueHandler->m_ResultQueueCentralized;


	while (TRUE) {
		WaitForSingleObject(queueHandler->m_hPushedResultCentralized, INFINITE);
		if (!queueHandler->m_bActive)
			return 0;

		///Udupa; TODO
		//if(queueHandler->m_pApp->m_pResourceCheckDlg != NULL && !queueHandler->m_pApp->m_pResourceCheckDlg->m_bDiskOk) {
		//	OutputDebugLogTo(9, TRUE, "************* Low Disk space; Not saving image to disk *************");
		//	continue;
		//}

		if (theApp.m_CentralizedCommInterface.m_pCommManager[0]->m_Monitor.m_bConnected) {
			EnterCriticalSection(&queueHandler->m_csResultCentralizedQueue);

			while (!pResultQueue->empty()) {
				timer.Reset();
				int nError = queueHandler->SendCentralizedResult();
				if (nError)
					break;

				pResultQueue->pop();
				OutputDebugLogTo(6, TRUE, "**** Send Centralized Result Time: %5.2fms", timer.GetElapsedMSec());

			}

			LeaveCriticalSection(&queueHandler->m_csResultCentralizedQueue);

		}


		//		if(!queueHandler->m_pApp->m_pResourceCheckDlg->m_bDiskOk) {
		//			queueHandler->m_bActive = FALSE;
		//			OutputDebugLogTo(9, TRUE, "************* [%s] Low Disk Space (%d)", strLogDir, availableBytes);
		//			strDir.ReleaseBuffer();
		//			return -1;
		//		}
	}

	return 0;
}

UINT ImageCentralizedSendThread(LPVOID pParam)
{
	CPerformanceTimer timer;

	CImageQueueHandler* queueHandler = (CImageQueueHandler*)pParam;
	queue<CQueueBufferCentralized*>* pImageQueue = &queueHandler->m_ImageQueueCentralized;
	queue<CQueueBufferCentralized*>* pBackupImageQueue = &queueHandler->m_BackupImageQueueCentralized;

	while (TRUE) {
		WaitForSingleObject(queueHandler->m_hPushedImageCentralized, INFINITE);
		if (!queueHandler->m_bActive)
			return 0;

		if (theApp.m_CentralizedCommInterface.m_pCommManager[0]->m_Monitor.m_bConnected && theApp.m_bIsFailPart) {
			EnterCriticalSection(&queueHandler->m_csImageCentralizedQueue);
			while (!pBackupImageQueue->empty()) {
				pImageQueue->push(pBackupImageQueue->front());
				pBackupImageQueue->pop();
			}
			LeaveCriticalSection(&queueHandler->m_csImageCentralizedQueue);

			while (!pImageQueue->empty()) {

				timer.Reset();
				CQueueBufferCentralized* pBuf = pImageQueue->front();
				int nError = queueHandler->SendCentralizedImage(pBuf);
				
				//If can not send the data --> still keep the Image buffer in queue
				if (nError)
					break;
				pImageQueue->pop();
				if (!pBuf->m_bMappingReady) {
					OutputDebugLogTo(6, TRUE, "[Die%d Track%d Doc%d Fov%d] **** Send Centralized Image Time: %5.2fms", pBuf->m_nDie, pBuf->m_nTrack,
						pBuf->m_nDoc, pBuf->m_nFov, timer.GetElapsedMSec());
				}
				else {
					OutputDebugLogTo(6, TRUE, "[Die%d Track%d Doc%d Fov%d] **** Send Mapping Ready Time: %5.2fms", pBuf->m_nDie, pBuf->m_nTrack,
						pBuf->m_nDoc, pBuf->m_nFov, timer.GetElapsedMSec());
				}
				
				delete pBuf;

			}
		}
	}

	return 0;
}

UINT DefectInfoCentralizedSendThread(LPVOID pParam)
{
	CPerformanceTimer timer;

	CImageQueueHandler* queueHandler = (CImageQueueHandler*)pParam;
	queue<CQueueDefectInfoCentralized*>* pDefectInfoQueue = &queueHandler->m_DefectInfoQueueCentralized;
	queue<CQueueDefectInfoCentralized*>* pBackupDefectInfoQueue = &queueHandler->m_BackupDefectInfoQueueCentralized;

	while (TRUE) {
		WaitForSingleObject(queueHandler->m_hPushedDefectInfoCentralized, INFINITE);
		if (!queueHandler->m_bActive)
			return 0;

		///Udupa; TODO
		//if(queueHandler->m_pApp->m_pResourceCheckDlg != NULL && !queueHandler->m_pApp->m_pResourceCheckDlg->m_bDiskOk) {
		//	OutputDebugLogTo(9, TRUE, "************* Low Disk space; Not saving image to disk *************");
		//	continue;
		//}

		if (theApp.m_CentralizedCommInterface.m_pCommManager[0]->m_Monitor.m_bConnected && theApp.m_bIsFailPart) {

			EnterCriticalSection(&queueHandler->m_csDefectInfoCentralizedQueue);
			while (!pBackupDefectInfoQueue->empty()) {
				pDefectInfoQueue->push(pBackupDefectInfoQueue->front());
				pBackupDefectInfoQueue->pop();
			}
			LeaveCriticalSection(&queueHandler->m_csDefectInfoCentralizedQueue);

			while (!pDefectInfoQueue->empty()) {

				timer.Reset();
				CQueueDefectInfoCentralized* pBuf = pDefectInfoQueue->front();
				int nError = queueHandler->SendCentralizedDefectInfo(pBuf);
				if (nError)
					break;

				pDefectInfoQueue->pop();
				OutputDebugLogTo(6, TRUE, "[Track%d] **** Send Centralized Defect Info Time: %5.2fms", pBuf->m_nTrack, timer.GetElapsedMSec());
				delete pBuf;
			}
		}
	}

	return 0;
}

UINT ImageSaveThreadDL(LPVOID pParam)
{
	CPerformanceTimer timer;

	CImageQueueHandler* queueHandler = (CImageQueueHandler*)pParam;
	queue<CQueueBuffer*>* pImageQueue = &queueHandler->m_ImageQueueDL;

	while (TRUE) {
		WaitForSingleObject(queueHandler->m_hPushedDL, INFINITE);
		if (!queueHandler->m_bActive)
			return 0;

		///Udupa; TODO
		//if(queueHandler->m_pApp->m_pResourceCheckDlg != NULL && !queueHandler->m_pApp->m_pResourceCheckDlg->m_bDiskOk) {
		//	OutputDebugLogTo(9, TRUE, "************* Low Disk space; Not saving image to disk *************");
		//	continue;
		//}


		EnterCriticalSection(&queueHandler->m_csQueue);
		while (!pImageQueue->empty()) {

			timer.Reset();
			CQueueBuffer* pBuf = pImageQueue->front();
			queueHandler->SaveImageDL(pBuf);
			pImageQueue->pop();
			OutputDebugLogTo(6, TRUE, "ImageSaveThreadDL [Track%d] **** Image Save Time: %5.2fms", pBuf->m_nDoc, timer.GetElapsedMSec());
			delete pBuf;

		}
		LeaveCriticalSection(&queueHandler->m_csQueue);

		//		if(!queueHandler->m_pApp->m_pResourceCheckDlg->m_bDiskOk) {
		//			queueHandler->m_bActive = FALSE;
		//			OutputDebugLogTo(9, TRUE, "************* [%s] Low Disk Space (%d)", strLogDir, availableBytes);
		//			strDir.ReleaseBuffer();
		//			return -1;
		//		}
	}

	return 0;
}


UINT ImageSaveThread(LPVOID pParam)
{
	CPerformanceTimer timer;

	CImageQueueHandler* queueHandler = (CImageQueueHandler*) pParam;

	queue<CQueueBuffer*>* pImageQueue = &queueHandler->m_ImageQueue;
	queue<CQueueBuffer*>* pBackImageQueue = &queueHandler->m_BackImageQueue;

	while(TRUE) {
		WaitForSingleObject(queueHandler->m_hPushed, INFINITE);
		if(!queueHandler->m_bActive)
			return 0;

		///Udupa; TODO
		//if(queueHandler->m_pApp->m_pResourceCheckDlg != NULL && !queueHandler->m_pApp->m_pResourceCheckDlg->m_bDiskOk) {
		//	OutputDebugLogTo(9, TRUE, "************* Low Disk space; Not saving image to disk *************");
		//	continue;
		//}

		EnterCriticalSection(&queueHandler->m_csQueue);
		while(!pBackImageQueue->empty()) {
			pImageQueue->push(pBackImageQueue->front());
			pBackImageQueue->pop();
		}
		LeaveCriticalSection(&queueHandler->m_csQueue);

		while(!pImageQueue->empty()) {
			timer.Reset();
			CQueueBuffer* pBuf = pImageQueue->front();
			queueHandler->SaveImage(pBuf);
			pImageQueue->pop();
			OutputDebugLogTo(6, TRUE, "ImageSaveThread [Track%d] **** Image Save Time: %5.2fms", pBuf->m_nDoc, timer.GetElapsedMSec());
			delete pBuf;
		}

//		if(!queueHandler->m_pApp->m_pResourceCheckDlg->m_bDiskOk) {
//			queueHandler->m_bActive = FALSE;
//			OutputDebugLogTo(9, TRUE, "************* [%s] Low Disk Space (%d)", strLogDir, availableBytes);
//			strDir.ReleaseBuffer();
//			return -1;
//		}
	}

	return 0;
}


UINT ImageInspectThread(LPVOID pParam)
{
	CPerformanceTimer timer;

	CImageQueueHandler* queueHandler = (CImageQueueHandler*)pParam;

	queue<CQueueBuffer*>* pImageQueue = &queueHandler->m_ImageQueue;
	queue<CQueueBuffer*>* pBackImageQueue = &queueHandler->m_BackImageQueue;

	while (TRUE) {
		WaitForSingleObject(queueHandler->m_hPushedInspect, INFINITE);
		if (!queueHandler->m_bActive)
			return 0;

		///Udupa; TODO
		//if(queueHandler->m_pApp->m_pResourceCheckDlg != NULL && !queueHandler->m_pApp->m_pResourceCheckDlg->m_bDiskOk) {
		//	OutputDebugLogTo(9, TRUE, "************* Low Disk space; Not saving image to disk *************");
		//	continue;
		//}

		EnterCriticalSection(&queueHandler->m_csQueue);
		while (!pBackImageQueue->empty()) {
			pImageQueue->push(pBackImageQueue->front());
			pBackImageQueue->pop();
		}
		LeaveCriticalSection(&queueHandler->m_csQueue);

		while (!pImageQueue->empty()) {
			timer.Reset();
			CQueueBuffer* pBuf = pImageQueue->front();
			queueHandler->InspectImage(pBuf);
			pImageQueue->pop();
			OutputDebugLogTo(6, TRUE, "ImageInspectThread [Track%d] **** Image Save Time: %5.2fms", pBuf->m_nDoc, timer.GetElapsedMSec());
			delete pBuf;
		}

		//		if(!queueHandler->m_pApp->m_pResourceCheckDlg->m_bDiskOk) {
		//			queueHandler->m_bActive = FALSE;
		//			OutputDebugLogTo(9, TRUE, "************* [%s] Low Disk Space (%d)", strLogDir, availableBytes);
		//			strDir.ReleaseBuffer();
		//			return -1;
		//		}
	}

	return 0;
}


int CImageQueueHandler::InspectImage(CQueueBuffer* pBuf)
{
	//int nTrack;
	//int nError;

	//CAppDoc *pDoc;

	//nTrack = pBuf->m_nTrack;
	//pDoc = m_pApp->m_pTracks[nTrack].m_pDoc[0];
	//for (int nDoc = 0; nDoc < pBuf->m_nTotalDocs[nTrack]; nDoc++) {
	//	m_pApp->m_pTracks[nTrack].m_SharedData.pProcBuffers[nDoc]->Allocate(pBuf->m_pImageBufferInspect[nDoc]->GetImgSize());
	//	CRect rect = CRect(CPoint(0, 0), pBuf->m_pImageBufferInspect[nDoc]->GetImgSize());
	//	ImgCopy(pBuf->m_pImageBufferInspect[nDoc], &rect, m_pApp->m_pTracks[nTrack].m_SharedData.pProcBuffers[nDoc], &rect);
	//}
	//pDoc->InspectQueueBuffers(pBuf->m_nTrack,pBuf->m_nTotalDocs[nTrack],pBuf->m_nFov,pBuf->m_nDie);

	//if(pBuf->m_nTrack==m_pApp->m_nTotalTracks-1 && pBuf->m_nFov==m_pApp->m_nNoOfFovX-1 && pBuf->m_nDie==m_pApp->m_nNoOfDies-1)
	//	m_pApp->UpdateResults();

	return 0;
}

UINT CentralizedQueueHandlingThread(LPVOID pParam)
{
	CImageQueueHandler* queueHandler = (CImageQueueHandler*)pParam;

	while (TRUE) {
		WaitForSingleObject(queueHandler->m_hPushedStationQueueCentralized, INFINITE);
		if (!queueHandler->m_bActive)
			return 0;

		int nStationId = -1;
		EnterCriticalSection(&queueHandler->m_pApp->m_csCentralizedQueue);
		if (!queueHandler->m_pApp->m_CentralizedStationQueue.empty()) {
			nStationId = queueHandler->m_pApp->m_CentralizedStationQueue.front();
		}
		LeaveCriticalSection(&queueHandler->m_pApp->m_csCentralizedQueue);

		if (nStationId < 0 || nStationId >= NO_OF_PORT_CENTRALIZED)
			continue;

		if (theApp.m_CentralizedCommInterface.m_pCommManager[nStationId]->m_Monitor.m_bConnected) {
			OutputDebugLogTo(6, TRUE, "Trigger Verification for Machine #%d", nStationId);
			SetEvent(theApp.m_CentralizedCommInterface.m_pCommManager[nStationId]->m_eventVerification);

			theApp.UpdateCentralizedMachineState(nStationId, "Waiting for verification...");

			OutputDebugLogTo(6, TRUE, "Waiting for Verification of Machine #%d To Complete....", nStationId);
			if (WaitForSingleObject(theApp.m_CentralizedCommInterface.m_pCommManager[nStationId]->m_eventVerificationDone, INFINITE) != WAIT_OBJECT_0) {
				OutputDebugLogTo(0, TRUE, "-----ERROR : Waiting Verification Done Event has been failed");
				continue;
			}
			OutputDebugLogTo(6, TRUE, "Waiting for Verification of Machine #%d To Complete.... : Done", nStationId);

			theApp.UpdateCentralizedMachineState(nStationId, "Connected");

			queueHandler->m_pApp->m_CentralizedStationQueue.pop();
			queueHandler->m_pApp->m_mapCentralizedStationQueue.erase(nStationId);
		}
		else {
			CString strMsgError;
			strMsgError.Format("Error Connection With Machine: %s... Please Check!", theApp.m_CentralizedCommInterface.m_pCommManager[nStationId]->m_Monitor.GetClientAcceptedMachineID());
			AfxMessageBox(strMsgError);
		}
	}

	return 0;
}
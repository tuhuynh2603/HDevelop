/// Communication Manager
/// Udupa; Jan'11

#include "stdafx.h"
#include "Application.h"
#include "AppDoc.h"
#include "CentralizedCommManager.h"
#include "MainFrame.h"
#include "TrackHandler.h"
#include "StatisticsView.h"
#include "ImageSelectionDlg.h"
#include <iostream>
#include "CommMonitor.h"
#include "CommManager.h"
#include "ImageQueueHandler.h"


CENTRALIZEDSUMMARY::CENTRALIZEDSUMMARY()
{
	lTotalUnitsInspected = 0;
	lTotalUnitsPass = 0;
	lTotalUnitsFail = 0;
	lTotalUnitsFalseRejection = 0;

	dTotalYield = 0;

	strMachineState = "";
}

CCentralizedCommManager::CCentralizedCommManager(int nStation)
{
	m_nStation = nStation;
	//m_pDoc = NULL;
	m_pApp = (CApplication*) AfxGetApp(); 
	m_pDoc = m_pApp->m_pTracks[m_nStation].m_pDoc[0];
	m_eventResultCommand = CreateEventA(NULL, FALSE, FALSE, NULL);
	m_eventVResultCommand = CreateEventA(NULL, FALSE, FALSE, NULL);

	m_eventLastImageSent = CreateEventA(NULL, FALSE, FALSE, NULL);

	m_eventVerification = CreateEventA(NULL, FALSE, FALSE, NULL);
	m_eventVerificationDone = CreateEventA(NULL, FALSE, FALSE, NULL);

	m_nSendImageCount = 0;

	isValid			= FALSE;
	Initialize();
}

CCentralizedCommManager::~CCentralizedCommManager()
{
	SetEvent(m_eventResultCommand);
	SetEvent(m_eventVResultCommand);
	SetEvent(m_eventLastImageSent);
	SetEvent(m_eventVerification);
	SetEvent(m_eventVerificationDone);

	CloseHandle(m_eventResultCommand);
	CloseHandle(m_eventVResultCommand);
	CloseHandle(m_eventLastImageSent);
	CloseHandle(m_eventVerification);
	CloseHandle(m_eventVerificationDone);

}

void CCentralizedCommManager::Initialize()
{
	isValid	= m_Monitor.Create(&m_pApp->m_CentralizedCommInterface, m_nStation);

	

	if(!isValid)
		OutputDebugLog(" Controller:: Failed to open port");


	CWinThread* pThread = AfxBeginThread(VerificationResultsThread, this);
//	AfxBeginThread(CommSendThread, this);
	/*CWinThread* pThread = AfxBeginThread(MessageParser, this);
	m_hMessageParser	= pThread->m_hThread;

	pThread				= AfxBeginThread(MessageServer, this);
	m_hMessageServer	= pThread->m_hThread;*/
}

void CCentralizedCommManager::Close()
{
	m_Monitor.Close();
	isValid			= FALSE;
	/*SetEvent(m_Message.hMessageEvent);
	SetEvent(m_Message.hHandshakeEvent);*/
}

int CCentralizedCommManager::SendInfo(BYTE nCommand)
{
	try {
		int nHeadSize = sizeof(CentralizedHeader);
		int nCentralizedResultSize = theApp.m_strMachineID.GetLength() + theApp.m_strConfigFile.GetLength() + theApp.m_strMainCfgModeDir.GetLength();
		nCentralizedResultSize += 6;//each id need store length of string(3 factor x 2)

		//Flex Id
		int nDevicePerX, nDevicePerY;
		int nFovPerX, nFovPerY;
		theApp.motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
		int m_nNoOfDies = nDevicePerX * nDevicePerY;
		int m_nNoOfFov = nFovPerX * nFovPerY;
		for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
			nCentralizedResultSize += theApp.m_strPartFlex[nDie].GetLength() + 2;//string contents and its length
		}
		nCentralizedResultSize += 2;//no of dies

		//Device Config: Device Per X&Y, FOV Per X&Y,...
		int nDeviceConfigSize = sizeof(CentralizedDeviceConfigInfo);
		//Stats Info
		int nStatsInfoSize = sizeof(CentralizedStatsInfo);

		//Create header for buffer to send: contains command ID, buffer length and buffer contents
		int nBufSendSize = nHeadSize + nCentralizedResultSize + nDeviceConfigSize + nStatsInfoSize;
		uint8_t* pBufHead = new uint8_t[nHeadSize];
		CentralizedHeader* pHeader = reinterpret_cast<CentralizedHeader*>(pBufHead);
		pHeader->nCommand = nCommand;
		pHeader->nBufferLength = nBufSendSize;

		UnsignedShortByteUnion* unionConverter = NULL;
		unsigned short int uSize;

		//Buffer contents: machine ID, config file, config mode
		char* pBufferMachineInfo = new char[nCentralizedResultSize];
		char* pBufferMachineInfoOrigin = pBufferMachineInfo;

		//Machine ID and its length
		unionConverter = new UnsignedShortByteUnion;
		uSize = theApp.m_strMachineID.GetLength();
		unionConverter->asUnsignedShort = uSize;
		*pBufferMachineInfo = unionConverter->asBytes[0];
		pBufferMachineInfo++;
		*pBufferMachineInfo = unionConverter->asBytes[1];
		pBufferMachineInfo++;

		std::memcpy(pBufferMachineInfo, theApp.m_strMachineID.GetBuffer(), theApp.m_strMachineID.GetLength());
		pBufferMachineInfo += theApp.m_strMachineID.GetLength();

		//config file and its length
		unionConverter = new UnsignedShortByteUnion;
		uSize = theApp.m_strConfigFile.GetLength();
		unionConverter->asUnsignedShort = uSize;
		*pBufferMachineInfo = unionConverter->asBytes[0];
		pBufferMachineInfo++;
		*pBufferMachineInfo = unionConverter->asBytes[1];
		pBufferMachineInfo++;

		std::memcpy(pBufferMachineInfo, theApp.m_strConfigFile.GetBuffer(), theApp.m_strConfigFile.GetLength());
		pBufferMachineInfo += theApp.m_strConfigFile.GetLength();

		//config mode and its length
		unionConverter = new UnsignedShortByteUnion;
		uSize = theApp.m_strMainCfgModeDir.GetLength();
		unionConverter->asUnsignedShort = uSize;
		*pBufferMachineInfo = unionConverter->asBytes[0];
		pBufferMachineInfo++;
		*pBufferMachineInfo = unionConverter->asBytes[1];
		pBufferMachineInfo++;

		std::memcpy(pBufferMachineInfo, theApp.m_strMainCfgModeDir.GetBuffer(), theApp.m_strMainCfgModeDir.GetLength());
		pBufferMachineInfo += theApp.m_strMainCfgModeDir.GetLength();

		//No of Die, Flex Id and its length
		unionConverter = new UnsignedShortByteUnion;
		uSize = m_nNoOfDies;
		unionConverter->asUnsignedShort = uSize;
		*pBufferMachineInfo = unionConverter->asBytes[0];
		pBufferMachineInfo++;
		*pBufferMachineInfo = unionConverter->asBytes[1];
		pBufferMachineInfo++;

		for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
			unionConverter = new UnsignedShortByteUnion;
			uSize = theApp.m_strPartFlex[nDie].GetLength();
			unionConverter->asUnsignedShort = uSize;
			*pBufferMachineInfo = unionConverter->asBytes[0];
			pBufferMachineInfo++;
			*pBufferMachineInfo = unionConverter->asBytes[1];
			pBufferMachineInfo++;

			std::memcpy(pBufferMachineInfo, theApp.m_strPartFlex[nDie].GetBuffer(), theApp.m_strPartFlex[nDie].GetLength());
			pBufferMachineInfo += theApp.m_strPartFlex[nDie].GetLength();
		}

		//Device Config Details
		uint8_t* pBufDeviceConfig = new uint8_t[nDeviceConfigSize];
		CentralizedDeviceConfigInfo* pCSBufDeviceConfig = reinterpret_cast<CentralizedDeviceConfigInfo*>(pBufDeviceConfig);
		theApp.motionParameters.GetMappingParameters(pCSBufDeviceConfig->nDevicePerX, pCSBufDeviceConfig->nDevicePerY,
			pCSBufDeviceConfig->nFovPerX, pCSBufDeviceConfig->nFovPerY);
		theApp.motionParameters.getDeviceDirection(pCSBufDeviceConfig->bHorizontalDevice);

		//Stats Info
		uint8_t* pBufStatsInfo = new uint8_t[nStatsInfoSize];
		CentralizedStatsInfo* pCSBufStatsInfo = reinterpret_cast<CentralizedStatsInfo*>(pBufStatsInfo);
		for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
			pCSBufStatsInfo->lTotalUnitsInspected[nDie] = theApp.m_LotSummary.m_TotalDisplaySummary[nDie].lTotalUnitsInspected;
			pCSBufStatsInfo->lTotalUnitsPass[nDie] = theApp.m_LotSummary.m_TotalDisplaySummary[nDie].lTotalUnitsPass;
			pCSBufStatsInfo->lTotalUnitsFail[nDie] = theApp.m_LotSummary.m_TotalDisplaySummary[nDie].lTotalUnitsFail;
			pCSBufStatsInfo->lTotalFalseRejection[nDie] = theApp.m_LotSummary.m_TotalDisplaySummary[nDie].lUserModifiedFalseRejection;
			pCSBufStatsInfo->dTotalYield[nDie] = theApp.m_LotSummary.m_TotalDisplaySummary[nDie].dTotalYield;
		}


		//Copy header and contents into buffer to send
		char* pBufSend = new char[nBufSendSize];
		std::memcpy(pBufSend, reinterpret_cast<char*>(pBufHead), nHeadSize);
		pBufSend += nHeadSize;
		std::memcpy(pBufSend, pBufferMachineInfoOrigin, nCentralizedResultSize);
		pBufSend += nCentralizedResultSize;
		std::memcpy(pBufSend, reinterpret_cast<char*>(pBufDeviceConfig), nDeviceConfigSize);
		pBufSend += nDeviceConfigSize;
		std::memcpy(pBufSend, reinterpret_cast<char*>(pBufStatsInfo), nStatsInfoSize);

		pBufSend -= (nHeadSize + nCentralizedResultSize + nDeviceConfigSize);

		int nSuccess = m_Monitor.SendDataMessage(reinterpret_cast<char*>(pBufSend), nBufSendSize);

		delete unionConverter;
		delete[] pBufHead;
		delete[] pBufferMachineInfoOrigin;
		delete[] pBufDeviceConfig;
		delete[] pBufStatsInfo;
		delete[] pBufSend;

		if (!nSuccess)
			return -1;
	}
	catch (...) {
		OutputDebugLogTo(2, TRUE, "---ERROR: SendInfo");
		return -1;
	}

	return 0;
}

int CCentralizedCommManager::SendMappingResult(BYTE nCommand)
{
	try {
		int nHeadSize = sizeof(CentralizedHeader);
		int nCentralizedResultSize = theApp.m_strMachineID.GetLength() + theApp.m_strConfigFile.GetLength() + theApp.m_strMainCfgModeDir.GetLength();
		nCentralizedResultSize += 6;//each id need store length of string(3 factor x 2)

		//Flex Id
		int nDevicePerX, nDevicePerY;
		int nFovPerX, nFovPerY;
		theApp.motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
		int m_nNoOfDies = nDevicePerX * nDevicePerY;
		int m_nNoOfFov = nFovPerX * nFovPerY;
		for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
			nCentralizedResultSize += theApp.m_strPartFlex[nDie].GetLength() + 2;//string contents and its length
		}
		nCentralizedResultSize += 2;//no of dies

		int nMappingSize = sizeof(CentralizedResult);

		//Create header for buffer to send: contains command ID, buffer length and buffer contents
		int nBufSendSize = nHeadSize + nCentralizedResultSize + nMappingSize;
		uint8_t* pBufHead = new uint8_t[nHeadSize];
		CentralizedHeader* pHeader = reinterpret_cast<CentralizedHeader*>(pBufHead);
		pHeader->nCommand = nCommand;
		pHeader->nBufferLength = nBufSendSize;

		UnsignedShortByteUnion* unionConverter = NULL;
		unsigned short int uSize;

		//Buffer contents: machine ID, config file, config mode
		char* pBufferMachineInfo = new char[nCentralizedResultSize];
		char* pBufferMachineInfoOrigin = pBufferMachineInfo;

		//Machine ID and its length
		unionConverter = new UnsignedShortByteUnion;
		uSize = theApp.m_strMachineID.GetLength();
		unionConverter->asUnsignedShort = uSize;
		*pBufferMachineInfo = unionConverter->asBytes[0];
		pBufferMachineInfo++;
		*pBufferMachineInfo = unionConverter->asBytes[1];
		pBufferMachineInfo++;

		std::memcpy(pBufferMachineInfo, theApp.m_strMachineID.GetBuffer(), theApp.m_strMachineID.GetLength());
		pBufferMachineInfo += theApp.m_strMachineID.GetLength();

		//config file and its length
		unionConverter = new UnsignedShortByteUnion;
		uSize = theApp.m_strConfigFile.GetLength();
		unionConverter->asUnsignedShort = uSize;
		*pBufferMachineInfo = unionConverter->asBytes[0];
		pBufferMachineInfo++;
		*pBufferMachineInfo = unionConverter->asBytes[1];
		pBufferMachineInfo++;

		std::memcpy(pBufferMachineInfo, theApp.m_strConfigFile.GetBuffer(), theApp.m_strConfigFile.GetLength());
		pBufferMachineInfo += theApp.m_strConfigFile.GetLength();

		//config mode and its length
		unionConverter = new UnsignedShortByteUnion;
		uSize = theApp.m_strMainCfgModeDir.GetLength();
		unionConverter->asUnsignedShort = uSize;
		*pBufferMachineInfo = unionConverter->asBytes[0];
		pBufferMachineInfo++;
		*pBufferMachineInfo = unionConverter->asBytes[1];
		pBufferMachineInfo++;

		std::memcpy(pBufferMachineInfo, theApp.m_strMainCfgModeDir.GetBuffer(), theApp.m_strMainCfgModeDir.GetLength());
		pBufferMachineInfo += theApp.m_strMainCfgModeDir.GetLength();

		//No of Die, Flex Id and its length
		unionConverter = new UnsignedShortByteUnion;
		uSize = m_nNoOfDies;
		unionConverter->asUnsignedShort = uSize;
		*pBufferMachineInfo = unionConverter->asBytes[0];
		pBufferMachineInfo++;
		*pBufferMachineInfo = unionConverter->asBytes[1];
		pBufferMachineInfo++;

		for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
			unionConverter = new UnsignedShortByteUnion;
			uSize = theApp.m_strPartFlex[nDie].GetLength();
			unionConverter->asUnsignedShort = uSize;
			*pBufferMachineInfo = unionConverter->asBytes[0];
			pBufferMachineInfo++;
			*pBufferMachineInfo = unionConverter->asBytes[1];
			pBufferMachineInfo++;

			std::memcpy(pBufferMachineInfo, theApp.m_strPartFlex[nDie].GetBuffer(), theApp.m_strPartFlex[nDie].GetLength());
			pBufferMachineInfo += theApp.m_strPartFlex[nDie].GetLength();
		}


		//Mapping Error Code
		uint8_t* pBufMappingErrorCode = new uint8_t[nMappingSize];
		CentralizedResult* pCSBufMappingErrorCode = reinterpret_cast<CentralizedResult*>(pBufMappingErrorCode);

		std::copy(std::begin(theApp.m_nPartErrorCode), std::end(theApp.m_nPartErrorCode), std::begin(pCSBufMappingErrorCode->nPartErrorCode));
		std::copy(std::begin(theApp.m_nDieErrorCodeStats), std::end(theApp.m_nDieErrorCodeStats), std::begin(pCSBufMappingErrorCode->nDieErrorCodeStats));
		for (int nDie = 0; nDie < MAX_DIES; nDie++) {
			std::copy(std::begin(theApp.m_nFovPartErrorCode[nDie]), std::end(theApp.m_nFovPartErrorCode[nDie]), std::begin(pCSBufMappingErrorCode->nFovPartErrorCode[nDie]));
			for (int nFov = 0; nFov < MAX_FOV; nFov++) {
				std::copy(std::begin(theApp.m_nErrorCode[nDie][nFov]), std::end(theApp.m_nErrorCode[nDie][nFov]), std::begin(pCSBufMappingErrorCode->nErrorCode[nDie][nFov]));
			}
		}

		//Copy header and contents into buffer to send
		char* pBufSend = new char[nBufSendSize];
		std::memcpy(pBufSend, reinterpret_cast<char*>(pBufHead), nHeadSize);
		pBufSend += nHeadSize;
		std::memcpy(pBufSend, pBufferMachineInfoOrigin, nCentralizedResultSize);
		pBufSend += nCentralizedResultSize;
		std::memcpy(pBufSend, reinterpret_cast<char*>(pBufMappingErrorCode), nMappingSize);
		pBufSend -= (nHeadSize + nCentralizedResultSize);

		int nSuccess = m_Monitor.SendDataMessage(reinterpret_cast<char*>(pBufSend), nBufSendSize);

		delete unionConverter;
		delete[] pBufHead;
		delete[] pBufferMachineInfoOrigin;
		delete[] pBufMappingErrorCode;
		delete[] pBufSend;

		if (!nSuccess)
			return -1;
	}
	catch (...) {
		OutputDebugLogTo(2, TRUE, "---ERROR: SendMappingResult");
		return -1;
	}

	return 0;
}

int CCentralizedCommManager::DecodeMappingResult(char* pImageBufferMsg, int nImageBufferMsgLength)
{
	try {
		//Buffer contents: machine ID, config file, config mode, no of dies --> at least need 4 factor sore it's length buffer
		int nHeaderSize = 4 * 2;
		if (nImageBufferMsgLength < nHeaderSize)
			return -1;

		unsigned char* ucMessage = reinterpret_cast<unsigned char*>(pImageBufferMsg);
		unsigned char* ucMessageRun = ucMessage;

		int nCurrentMsgLength = nImageBufferMsgLength;
		//Starting extract information
		UnsignedShortByteUnion* unionConverter = NULL;
		//Decode machine ID, config file, config mode
		int nMachineIdLength, nConfigLength, nConfigModeLength;

		//---Machine ID
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessageRun[0];
		unionConverter->asBytes[1] = ucMessageRun[1];
		nMachineIdLength = unionConverter->asUnsignedShort;

		nCurrentMsgLength -= 2;
		if (nCurrentMsgLength < nMachineIdLength) {
			OutputDebugLogTo(2, TRUE, "DecodeMappingResult: Machine ID have not information!");
			return -1;
		}
		ucMessageRun += 2;

		char* strMachineId = new char[nMachineIdLength];
		std::memcpy(strMachineId, ucMessageRun, nMachineIdLength);
		CString csMachineId(strMachineId, nMachineIdLength);
		OutputDebugLogTo(2, TRUE, "DecodeMappingResult: machine ID %s", csMachineId);
		ucMessageRun += nMachineIdLength;
		nCurrentMsgLength -= nMachineIdLength;
		m_pApp->m_strClientMachineID[GetStationId()] = csMachineId;

		if (m_pApp->m_CentralizedCommInterface.m_bIsTCPServer)
			m_pApp->UpdateClientMachineIDPanel("Vision Machine ID: " + m_pApp->m_strClientMachineID[GetStationId()]);
		
		if (!m_pApp->m_bOnline)
			m_pApp->OnVisionOnline();

		m_Monitor.SetCurrentClientAcceptedMachineId(csMachineId);
		m_pApp->m_nCentralizedStationLatest = GetStationId();

		if (nCurrentMsgLength < 2) {
			OutputDebugLogTo(2, TRUE, "DecodeMappingResult: Config File have not buffer length information!");
			return -1;
		}

		//---Config File
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessageRun[0];
		unionConverter->asBytes[1] = ucMessageRun[1];
		nConfigLength = unionConverter->asUnsignedShort;
		nCurrentMsgLength -= 2;

		if (nCurrentMsgLength < nConfigLength) {
			OutputDebugLogTo(2, TRUE, "DecodeMappingResult: Config file have not information!");

			delete unionConverter;
			delete[] strMachineId;
			return -1;
		}
		ucMessageRun += 2;

		char* strConfigFile = new char[nConfigLength];
		std::memcpy(strConfigFile, ucMessageRun, nConfigLength);
		CString csConfigFile(strConfigFile, nConfigLength);
		OutputDebugLogTo(2, TRUE, "DecodeMappingResult: Config file %s", csConfigFile);
		ucMessageRun += nConfigLength;
		nCurrentMsgLength -= nConfigLength;

		if (nCurrentMsgLength < 2) {
			OutputDebugLogTo(2, TRUE, "DecodeMappingResult: Config mode have not buffer length information!");
			return -1;
		}

		//---Config Mode
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessageRun[0];
		unionConverter->asBytes[1] = ucMessageRun[1];
		nConfigModeLength = unionConverter->asUnsignedShort;
		nCurrentMsgLength -= 2;

		if (nCurrentMsgLength < nConfigModeLength) {
			OutputDebugLogTo(2, TRUE, "DecodeMappingResult: Config mode dir have not information!");

			delete unionConverter;
			delete[] strMachineId;
			delete[] strConfigFile;
			return -1;
		}
		ucMessageRun += 2;

		char* strConfigMode = new char[nConfigModeLength];
		std::memcpy(strConfigMode, ucMessageRun, nConfigModeLength);
		CString csConfigMode(strConfigMode, nConfigModeLength);
		OutputDebugLogTo(2, TRUE, "DecodeMappingResult: Config mode %s", csConfigMode);
		ucMessageRun += nConfigModeLength;
		nCurrentMsgLength -= nConfigModeLength;

		if (nCurrentMsgLength < 2) {
			OutputDebugLogTo(2, TRUE, "DecodeMappingResult: Flex ID have not number of die information!");
			return -1;
		}


		//---No of Die, Flex Id and its length
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessageRun[0];
		unionConverter->asBytes[1] = ucMessageRun[1];
		int nNoOfDies = unionConverter->asUnsignedShort;
		ucMessageRun += 2;
		nCurrentMsgLength -= 2;
		for (int nDie = 0; nDie < nNoOfDies; nDie++) {
			if (nCurrentMsgLength < 2) {
				OutputDebugLogTo(2, TRUE, "DecodeMappingResult: Flex ID have not buffer length information!");
				return -1;
			}

			unionConverter = new UnsignedShortByteUnion;
			unionConverter->asBytes[0] = ucMessageRun[0];
			unionConverter->asBytes[1] = ucMessageRun[1];
			int nFlexIdLength = unionConverter->asUnsignedShort;

			nCurrentMsgLength -= 2;
			if (nCurrentMsgLength < nFlexIdLength) {
				OutputDebugLogTo(2, TRUE, "DecodeMappingResult: Flex ID have not information!");

				delete unionConverter;
				return -1;
			}
			ucMessageRun += 2;

			char* strFlexId = new char[nFlexIdLength];
			std::memcpy(strFlexId, ucMessageRun, nFlexIdLength);
			CString csFlexId(strFlexId, nFlexIdLength);
			OutputDebugLogTo(2, TRUE, "DecodeMappingResult: Flex ID of die %d: %s", nDie + 1, csFlexId);
			
			if (theApp.m_CentralizedCommInterface.m_bIsTCPServer) {
				theApp.m_strPartFlex[nDie] = csFlexId;
			}

			ucMessageRun += nFlexIdLength;
			nCurrentMsgLength -= nFlexIdLength;

			delete[] strFlexId;
		}

		//Mapping Error Code Result
		int nMappingSize = sizeof(CentralizedResult);
		if (nCurrentMsgLength < nMappingSize) {
			OutputDebugLogTo(2, TRUE, "DecodeMappingResult: Mapping error code have not information!");

			delete unionConverter;
			delete[] strMachineId;
			delete[] strConfigFile;
			delete[] strConfigMode;
			return -1;
		}
		uint8_t* pMappingBuf = new uint8_t[nMappingSize];
		std::memcpy(pMappingBuf, ucMessageRun, nMappingSize);
		CentralizedResult *pCSMappingResult = reinterpret_cast<CentralizedResult*>(pMappingBuf);

		std::copy(std::begin(pCSMappingResult->nPartErrorCode), std::end(pCSMappingResult->nPartErrorCode), std::begin(theApp.m_nPartErrorCode));
		std::copy(std::begin(pCSMappingResult->nDieErrorCodeStats), std::end(pCSMappingResult->nDieErrorCodeStats), std::begin(theApp.m_nDieErrorCodeStats));
		for (int nDie = 0; nDie < MAX_DIES; nDie++) {
			std::copy(std::begin(pCSMappingResult->nFovPartErrorCode[nDie]), std::end(pCSMappingResult->nFovPartErrorCode[nDie]), std::begin(theApp.m_nFovPartErrorCode[nDie]));
			for (int nFov = 0; nFov < MAX_FOV; nFov++) {
				std::copy(std::begin(pCSMappingResult->nErrorCode[nDie][nFov]), std::end(pCSMappingResult->nErrorCode[nDie][nFov]), std::begin(theApp.m_nErrorCode[nDie][nFov]));
			}
		}

		std::copy(std::begin(pCSMappingResult->nPartErrorCode), std::end(pCSMappingResult->nPartErrorCode), std::begin(theApp.m_nCentralizedPartErrorCode[GetStationId()]));
		for (int nDie = 0; nDie < MAX_DIES; nDie++) {
			std::copy(std::begin(pCSMappingResult->nFovPartErrorCode[nDie]), std::end(pCSMappingResult->nFovPartErrorCode[nDie]), std::begin(theApp.m_nCentralizedFovPartErrorCode[GetStationId()][nDie]));
			for (int nFov = 0; nFov < MAX_FOV; nFov++) {
				std::copy(std::begin(pCSMappingResult->nErrorCode[nDie][nFov]), std::end(pCSMappingResult->nErrorCode[nDie][nFov]), std::begin(theApp.m_nCentralizedErrorCode[GetStationId()][nDie][nFov]));
			}
		}

		if(((CMainFrame*)theApp.m_pMainWnd)->m_dlgPane != NULL)
			((CMainFrame*)theApp.m_pMainWnd)->m_dlgPane->m_wndDlg.Invalidate();

		delete unionConverter;
		delete[] strMachineId;
		delete[] strConfigFile;
		delete[] strConfigMode;
		delete[] pMappingBuf;
	}
	catch (...) {
		OutputDebugLogTo(2, TRUE, "---ERROR: DecodeMappingResult");
		return -1;
	}
	
	return 0;
}

int CCentralizedCommManager::DecodeInfo(char* pImageBufferMsg, int nImageBufferMsgLength)
{
	try {
		//Buffer contents: machine ID, config file, config mode, no of dies --> at least need 4 factor sore it's length buffer
		int nHeaderSize = 4 * 2;
		if (nImageBufferMsgLength < nHeaderSize)
			return -1;

		unsigned char* ucMessage = reinterpret_cast<unsigned char*>(pImageBufferMsg);
		unsigned char* ucMessageRun = ucMessage;

		int nCurrentMsgLength = nImageBufferMsgLength;
		//Starting extract information
		UnsignedShortByteUnion* unionConverter = NULL;
		//Decode machine ID, config file, config mode
		int nMachineIdLength, nConfigLength, nConfigModeLength;

		//---Machine ID
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessageRun[0];
		unionConverter->asBytes[1] = ucMessageRun[1];
		nMachineIdLength = unionConverter->asUnsignedShort;

		nCurrentMsgLength -= 2;
		if (nCurrentMsgLength < nMachineIdLength) {
			OutputDebugLogTo(2, TRUE, "DecodeInfo: Machine ID have not information!");
			return -1;
		}
		ucMessageRun += 2;

		bool bIsClientChanged = false;
		char* strMachineId = new char[nMachineIdLength];
		std::memcpy(strMachineId, ucMessageRun, nMachineIdLength);
		CString csMachineId(strMachineId, nMachineIdLength);
		OutputDebugLogTo(2, TRUE, "DecodeInfo: machine ID %s", csMachineId);
		ucMessageRun += nMachineIdLength;
		nCurrentMsgLength -= nMachineIdLength;

		if (m_pApp->m_strClientMachineID[GetStationId()] != csMachineId)
			bIsClientChanged = true;
		m_pApp->m_strClientMachineID[GetStationId()] = csMachineId;

		m_Monitor.SetCurrentClientAcceptedMachineId(csMachineId);

		m_pApp->UpdateCentralizedMachineID(GetStationId());
		m_pApp->UpdateCentralizedMachineState(GetStationId(), "Connected");

		if (nCurrentMsgLength < 2) {
			OutputDebugLogTo(2, TRUE, "DecodeInfo: Config File have not buffer length information!");
			return -1;
		}

		//---Config File
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessageRun[0];
		unionConverter->asBytes[1] = ucMessageRun[1];
		nConfigLength = unionConverter->asUnsignedShort;
		nCurrentMsgLength -= 2;

		if (nCurrentMsgLength < nConfigLength) {
			OutputDebugLogTo(2, TRUE, "DecodeInfo: Config file have not information!");

			delete unionConverter;
			delete[] strMachineId;
			return -1;
		}
		ucMessageRun += 2;

		char* strConfigFile = new char[nConfigLength];
		std::memcpy(strConfigFile, ucMessageRun, nConfigLength);
		CString csConfigFile(strConfigFile, nConfigLength);
		OutputDebugLogTo(2, TRUE, "DecodeInfo: Config file %s", csConfigFile);
		ucMessageRun += nConfigLength;
		nCurrentMsgLength -= nConfigLength;

		if (nCurrentMsgLength < 2) {
			OutputDebugLogTo(2, TRUE, "DecodeInfo: Config mode have not buffer length information!");
			return -1;
		}

		//---Config Mode
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessageRun[0];
		unionConverter->asBytes[1] = ucMessageRun[1];
		nConfigModeLength = unionConverter->asUnsignedShort;
		nCurrentMsgLength -= 2;

		if (nCurrentMsgLength < nConfigModeLength) {
			OutputDebugLogTo(2, TRUE, "DecodeInfo: Config mode dir have not information!");

			delete unionConverter;
			delete[] strMachineId;
			delete[] strConfigFile;
			return -1;
		}
		ucMessageRun += 2;

		char* strConfigMode = new char[nConfigModeLength];
		std::memcpy(strConfigMode, ucMessageRun, nConfigModeLength);
		CString csConfigMode(strConfigMode, nConfigModeLength);
		OutputDebugLogTo(2, TRUE, "DecodeInfo: Config mode %s", csConfigMode);
		ucMessageRun += nConfigModeLength;
		nCurrentMsgLength -= nConfigModeLength;

		if (nCurrentMsgLength < 2) {
			OutputDebugLogTo(2, TRUE, "DecodeInfo: Flex ID have not number of die information!");
			return -1;
		}

		//---No of Die, Flex Id and its length
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessageRun[0];
		unionConverter->asBytes[1] = ucMessageRun[1];
		int nNoOfDies = unionConverter->asUnsignedShort;
		ucMessageRun += 2;
		nCurrentMsgLength -= 2;
		for (int nDie = 0; nDie < nNoOfDies; nDie++) {
			if (nCurrentMsgLength < 2) {
				OutputDebugLogTo(2, TRUE, "DecodeInfo: Flex ID have not buffer length information!");
				return -1;
			}

			unionConverter = new UnsignedShortByteUnion;
			unionConverter->asBytes[0] = ucMessageRun[0];
			unionConverter->asBytes[1] = ucMessageRun[1];
			int nFlexIdLength = unionConverter->asUnsignedShort;

			nCurrentMsgLength -= 2;
			if (nCurrentMsgLength < nFlexIdLength) {
				OutputDebugLogTo(2, TRUE, "DecodeInfo: Flex ID have not information!");

				delete unionConverter;
				return -1;
			}
			ucMessageRun += 2;
			

			char* strFlexId = new char[nFlexIdLength];
			std::memcpy(strFlexId, ucMessageRun, nFlexIdLength);
			CString csFlexId(strFlexId, nFlexIdLength);
			OutputDebugLogTo(2, TRUE, "DecodeInfo: Flex ID of die %d: %s", nDie + 1, csFlexId);
			if (theApp.m_CentralizedCommInterface.m_bIsTCPServer) {
				theApp.m_strPartFlex[nDie] = csFlexId;
			}

			ucMessageRun += nFlexIdLength;
			nCurrentMsgLength -= nFlexIdLength;

			delete[] strFlexId;
		}

		//Device Config Details
		int nDeviceConfigSize = sizeof(CentralizedDeviceConfigInfo);
		if (nCurrentMsgLength < nDeviceConfigSize) {
			OutputDebugLogTo(2, TRUE, "DecodeInfo: Device Config have not information!");

			delete unionConverter;
			delete[] strMachineId;
			delete[] strConfigFile;
			delete[] strConfigMode;
			return -1;
		}
		uint8_t* pDeviceConfigBuf = new uint8_t[nDeviceConfigSize];
		std::memcpy(pDeviceConfigBuf, ucMessageRun, nDeviceConfigSize);
		CentralizedDeviceConfigInfo *pCSDeviceConfig = reinterpret_cast<CentralizedDeviceConfigInfo*>(pDeviceConfigBuf);
		m_MachineDeviceConifg = *pCSDeviceConfig;

		nCurrentMsgLength -= nDeviceConfigSize;

		//Stats Info
		int nStatsInfoSize = sizeof(CentralizedStatsInfo);
		if (nCurrentMsgLength < nStatsInfoSize) {
			OutputDebugLogTo(2, TRUE, "DecodeInfo: Stats Info have not information!");

			delete unionConverter;
			delete[] strMachineId;
			delete[] strConfigFile;
			delete[] strConfigMode;
			delete[] pDeviceConfigBuf;
			return -1;
		}
		ucMessageRun += nDeviceConfigSize;

		uint8_t* pStatsInfoBuf = new uint8_t[nStatsInfoSize];
		std::memcpy(pStatsInfoBuf, ucMessageRun, nStatsInfoSize);
		CentralizedStatsInfo *pCSStatsInfo = reinterpret_cast<CentralizedStatsInfo*>(pStatsInfoBuf);
		
		m_CentralizedSummary.lTotalUnitsInspected = 0;
		m_CentralizedSummary.lTotalUnitsPass = 0;
		m_CentralizedSummary.lTotalUnitsFail = 0;
		m_CentralizedSummary.lTotalUnitsFalseRejection = 0;
		m_CentralizedSummary.dTotalYield = 0;
		for (int nDie = 0; nDie < nNoOfDies; nDie++) {
			m_CentralizedSummary.lTotalUnitsInspected += pCSStatsInfo->lTotalUnitsInspected[nDie];
			m_CentralizedSummary.lTotalUnitsPass += pCSStatsInfo->lTotalUnitsPass[nDie];
			m_CentralizedSummary.lTotalUnitsFail += pCSStatsInfo->lTotalUnitsFail[nDie];
			m_CentralizedSummary.lTotalUnitsFalseRejection += pCSStatsInfo->lTotalFalseRejection[nDie];
			m_CentralizedSummary.dTotalYield += pCSStatsInfo->dTotalYield[nDie];
		}
		theApp.UpdateCentralizedSummary(GetStationId());


		delete unionConverter;
		delete[] strMachineId;
		delete[] strConfigFile;
		delete[] strConfigMode;
		delete[] pDeviceConfigBuf;
		delete[] pStatsInfoBuf;
	}
	catch (...) {
		OutputDebugLogTo(2, TRUE, "---ERROR: DecodeInfo");
		return -1;
	}

	return 0;
}

int CCentralizedCommManager::SendAllImageBufferMessage()
{
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	theApp.motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfDies = nDevicePerX * nDevicePerY;
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int nNoImages = 0;
	DWORD dwTotalSizeImage = 0;
	for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
		for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
			for (int nTrack = 0; nTrack < theApp.m_nTotalTracks; nTrack++) {
				for (int nDoc = 0; nDoc < theApp.m_nTotalViewDocs[nTrack]; nDoc++) {
					if (!theApp.motionParameters.IsGrabEnabled(nTrack, nDoc, nFov))
						continue;
					nNoImages += 1;

					int nImageWidth = theApp.m_pTracks[0].m_pDoc[0]->m_BufferDie[0][0].GetImgSize().cx;
					int nImageHeight = theApp.m_pTracks[0].m_pDoc[0]->m_BufferDie[0][0].GetImgSize().cy;
					dwTotalSizeImage += (nImageWidth*nImageHeight);
				}
			}
		}
	}

	
	//Contains Track, Doc, Device and Fov index and image width and image height information and image buffer
	int nHeaderSize = 6 * 2;
	int nTotalHeaderSize = nHeaderSize *nNoImages;
	BYTE* lpMessageImageInfo = new BYTE[abs((int)(dwTotalSizeImage)) + nTotalHeaderSize + 2];
	BYTE* lpMessageImageInfoOrigin = lpMessageImageInfo;

	UnsignedShortByteUnion* unionConverter = NULL;
	unsigned short int uSize;

	//First header store number of images
	//Number of image to send
	unionConverter = new UnsignedShortByteUnion;
	uSize = nNoImages;
	unionConverter->asUnsignedShort = uSize;
	*lpMessageImageInfo = unionConverter->asBytes[0];
	lpMessageImageInfo++;
	*lpMessageImageInfo = unionConverter->asBytes[1];
	lpMessageImageInfo++;

	for (int nDevice = 0; nDevice < m_nNoOfDies; nDevice++) {
		for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
			for (int nTrack = 0; nTrack < theApp.m_nTotalTracks; nTrack++) {
				for (int nDoc = 0; nDoc < theApp.m_nTotalViewDocs[nTrack]; nDoc++) {
					if (!theApp.motionParameters.IsGrabEnabled(nTrack, nDoc, nFov))
						continue;

					int nImageWidth = theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDevice][nFov].GetImgSize().cx;
					int nImageHeight = theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDevice][nFov].GetImgSize().cy;

					//Track, Doc, Device and Fov index
					//Starting Index: 0
					unionConverter = new UnsignedShortByteUnion;
					uSize = nTrack;
					unionConverter->asUnsignedShort = uSize;
					*lpMessageImageInfo = unionConverter->asBytes[0];
					lpMessageImageInfo++;
					*lpMessageImageInfo = unionConverter->asBytes[1];
					lpMessageImageInfo++;

					//Starting Index: 2
					unionConverter = new UnsignedShortByteUnion;
					uSize = nDoc;
					unionConverter->asUnsignedShort = uSize;
					*lpMessageImageInfo = unionConverter->asBytes[0];
					lpMessageImageInfo++;
					*lpMessageImageInfo = unionConverter->asBytes[1];
					lpMessageImageInfo++;

					//Starting Index: 4
					unionConverter = new UnsignedShortByteUnion;
					uSize = nDevice;
					unionConverter->asUnsignedShort = uSize;
					*lpMessageImageInfo = unionConverter->asBytes[0];
					lpMessageImageInfo++;
					*lpMessageImageInfo = unionConverter->asBytes[1];
					lpMessageImageInfo++;

					//Starting Index: 6
					unionConverter = new UnsignedShortByteUnion;
					uSize = nFov;
					unionConverter->asUnsignedShort = uSize;
					*lpMessageImageInfo = unionConverter->asBytes[0];
					lpMessageImageInfo++;
					*lpMessageImageInfo = unionConverter->asBytes[1];
					lpMessageImageInfo++;

					//Starting Index: 8
					unionConverter = new UnsignedShortByteUnion;
					uSize = nImageWidth;
					unionConverter->asUnsignedShort = uSize;
					*lpMessageImageInfo = unionConverter->asBytes[0];
					lpMessageImageInfo++;
					*lpMessageImageInfo = unionConverter->asBytes[1];
					lpMessageImageInfo++;

					//Starting Index: 10
					unionConverter = new UnsignedShortByteUnion;
					uSize = nImageHeight;
					unionConverter->asUnsignedShort = uSize;
					*lpMessageImageInfo = unionConverter->asBytes[0];
					lpMessageImageInfo++;
					*lpMessageImageInfo = unionConverter->asBytes[1];
					lpMessageImageInfo++;


					//Buffer Image
					DWORD dwSizeImage = nImageWidth* nImageHeight;
					BYTE*	pImageArray = theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDevice][nFov].GetImgPtr();
					std::memcpy(lpMessageImageInfo, pImageArray, dwSizeImage);
					lpMessageImageInfo += dwSizeImage;
				}
			}
		}
	}

	
	//Create header for buffer to send: contains command ID, buffer length and buffer contents
	int nHeadSize = sizeof(CentralizedHeader);
	int nBufSendSize = nHeadSize + dwTotalSizeImage + nTotalHeaderSize;
	uint8_t* buf = new uint8_t[nHeadSize];
	CentralizedHeader* pHeader = reinterpret_cast<CentralizedHeader*>(buf);
	pHeader->nCommand = SEND_LIST_IMAGE_BUFFER;
	pHeader->nBufferLength = nBufSendSize;

	//Copy header and contents into buffer to send
	char* pBufSend = new char[nBufSendSize];
	std::memcpy(pBufSend, reinterpret_cast<char*>(buf), nHeadSize);
	pBufSend += nHeadSize;
	std::memcpy(pBufSend, lpMessageImageInfoOrigin, dwTotalSizeImage + nHeaderSize);
	pBufSend -= nHeadSize;

	int nSuccess = m_Monitor.SendDataMessage(reinterpret_cast<char*>(pBufSend), nBufSendSize);

	//int nSuccess = theApp.m_CentralizedCommInterface.m_pCommManager[0]->m_Monitor.SendDataMessage(reinterpret_cast<char*>(pBufSend), 2*dwSizeImage + nHeaderSize);

	delete unionConverter;
	delete[] buf;
	delete[] pBufSend;
	delete[] lpMessageImageInfoOrigin;

	if (!nSuccess)
		return -1;

	return 0;
}


int CCentralizedCommManager::SendDefectInfoMessage(int nTrack, int nDoc, int nDevice, int nFov, const std::vector<CPoint>& arrayDefectCenters, const std::vector<CRect>& arrayDefectRects,
 std::vector<std::vector<int>>& arrayDefectX, std::vector<std::vector<int>>& arrayDefectY)
{
	try {
		if (nTrack > MAX_TRACKS || nDoc > MAX_DOCS || nDevice > MAX_DIES || nFov > MAX_FOV || arrayDefectX.size() == 0 || arrayDefectCenters.size() == 0)
			return 0;
		//Calculate how many bytes needed for store defect information(unit store is 2-byte: unsigned short int)
		//Following this order: Number of defect, coordinate of center defect i(x, y), top left defect i(x, y), bottom right i(x, y): (1 + defect count * 6) * 2 bytes
		int nDefects = arrayDefectCenters.size();
		int nSizeDefectInfo = nDefects * sizeof(CentralizedDefectRegionInfo) + sizeof(int);

		if (arrayDefectX.size() < arrayDefectCenters.size()) {
			int nStartIdx = arrayDefectX.size();
			for (int i = nStartIdx; i < arrayDefectCenters.size(); i++) {
				arrayDefectX.push_back(std::vector<int>(1, 0));
				arrayDefectY.push_back(std::vector<int>(1, 0));
			}
		}

		for (int i = 0; i < arrayDefectX.size(); i++) {
			nSizeDefectInfo += 2 * (sizeof(int) * arrayDefectX[i].size());
		}
		


		//contains  Track, Doc, Device and Fov index information and defect information
		int nHeaderSize = 4 * 2;
		BYTE* lpMessageImageInfo = new BYTE[abs((int)nSizeDefectInfo) + nHeaderSize];
		BYTE* lpMessageImageInfoOrigin = lpMessageImageInfo;

		UnsignedShortByteUnion* unionConverter = NULL;
		unsigned short int uSize;

		//Track, Doc, Device and Fov index
		//Starting Index: 0
		unionConverter = new UnsignedShortByteUnion;
		uSize = nTrack;
		unionConverter->asUnsignedShort = uSize;
		*lpMessageImageInfo = unionConverter->asBytes[0];
		lpMessageImageInfo++;
		*lpMessageImageInfo = unionConverter->asBytes[1];
		lpMessageImageInfo++;

		//Starting Index: 2
		unionConverter = new UnsignedShortByteUnion;
		uSize = nDoc;
		unionConverter->asUnsignedShort = uSize;
		*lpMessageImageInfo = unionConverter->asBytes[0];
		lpMessageImageInfo++;
		*lpMessageImageInfo = unionConverter->asBytes[1];
		lpMessageImageInfo++;

		//Starting Index: 4
		unionConverter = new UnsignedShortByteUnion;
		uSize = nDevice;
		unionConverter->asUnsignedShort = uSize;
		*lpMessageImageInfo = unionConverter->asBytes[0];
		lpMessageImageInfo++;
		*lpMessageImageInfo = unionConverter->asBytes[1];
		lpMessageImageInfo++;

		//Starting Index: 6
		unionConverter = new UnsignedShortByteUnion;
		uSize = nFov;
		unionConverter->asUnsignedShort = uSize;
		*lpMessageImageInfo = unionConverter->asBytes[0];
		lpMessageImageInfo++;
		*lpMessageImageInfo = unionConverter->asBytes[1];
		lpMessageImageInfo++;

		//--- Save defect Info
		//Defect count, Starting Index: 8
		unionConverter = new UnsignedShortByteUnion;
		uSize = nDefects;
		unionConverter->asUnsignedShort = uSize;
		*lpMessageImageInfo = unionConverter->asBytes[0];
		lpMessageImageInfo++;
		*lpMessageImageInfo = unionConverter->asBytes[1];
		lpMessageImageInfo++;

		int nBufDefectSize = sizeof(CentralizedDefectRegionInfo);
		for (int i = 0; i < nDefects; i++) {
			uint8_t* pBufDefect = new uint8_t[nBufDefectSize];
			CentralizedDefectRegionInfo * pDefectInfo = reinterpret_cast<CentralizedDefectRegionInfo*>(pBufDefect);
			//Store defect center
			pDefectInfo->nCenterX = arrayDefectCenters[i].x;
			pDefectInfo->nCenterY = arrayDefectCenters[i].y;

			//Store Defect Bounding Box
			//Top Left
			pDefectInfo->nTop = arrayDefectRects[i].top;
			pDefectInfo->nLeft = arrayDefectRects[i].left;
			//Bottom Right
			pDefectInfo->nBottom = arrayDefectRects[i].bottom;
			pDefectInfo->nRight = arrayDefectRects[i].right;

			//Store Number of Region Points of the Defect
			pDefectInfo->nDefectPointCount = arrayDefectX[i].size();

			//Copy Center(X, Y), Rect & Region points count to message
			std::memcpy(lpMessageImageInfo, reinterpret_cast<BYTE*>(pBufDefect), nBufDefectSize);
			lpMessageImageInfo += nBufDefectSize;

			//Copy all coordinates of region to message
			int* pDefectX = new int[arrayDefectX[i].size()];
			int* pDefectY = new int[arrayDefectX[i].size()];

			std::copy(arrayDefectX[i].begin(), arrayDefectX[i].end(), pDefectX);
			std::copy(arrayDefectY[i].begin(), arrayDefectY[i].end(), pDefectY);

			//Copy defect points
			int nBufDefectPointSize = sizeof(int)*arrayDefectX[i].size();
			std::memcpy(lpMessageImageInfo, reinterpret_cast<BYTE*>(pDefectX), nBufDefectPointSize);
			lpMessageImageInfo += nBufDefectPointSize;

			std::memcpy(lpMessageImageInfo, reinterpret_cast<BYTE*>(pDefectY), nBufDefectPointSize);
			lpMessageImageInfo += nBufDefectPointSize;

			delete[] pDefectX;
			delete[] pDefectY;
			delete[] pBufDefect;
		}

		//Create header for buffer to send: contains command ID, buffer length and buffer contents
		int nHeadSize = sizeof(CentralizedHeader);
		int nBufSendSize = nHeadSize + nSizeDefectInfo + nHeaderSize;
		uint8_t* buf = new uint8_t[nHeadSize];
		CentralizedHeader* pHeader = reinterpret_cast<CentralizedHeader*>(buf);
		pHeader->nCommand = SEND_DEFECT_INFO;
		pHeader->nBufferLength = nBufSendSize;

		//Copy header and contents into buffer to send
		char* pBufSend = new char[nBufSendSize];
		std::memcpy(pBufSend, reinterpret_cast<char*>(buf), nHeadSize);
		pBufSend += nHeadSize;
		std::memcpy(pBufSend, lpMessageImageInfoOrigin, nSizeDefectInfo + nHeaderSize);
		pBufSend -= nHeadSize;

		int nSuccess = m_Monitor.SendDataMessage(reinterpret_cast<char*>(pBufSend), nBufSendSize);

		delete unionConverter;
		delete[] lpMessageImageInfoOrigin;
		delete[] buf;
		delete[] pBufSend;

		if (!nSuccess)
			return -1;
	}
	catch (...) {
		OutputDebugLogTo(2, TRUE, "---ERROR: SendDefectInfoMessage");
		return -1;
	}
	
	return 0;
}

int CCentralizedCommManager::SendDefectInfoMessage(int nTrack, int nDoc, int nDevice, int nFov, const std::vector<CPoint>& arrayDefectCenters, const std::vector<CRect>& arrayDefectRects)
{
	if (nTrack > MAX_TRACKS || nDoc > MAX_DOCS || nDevice > MAX_DIES || nFov > MAX_FOV || arrayDefectRects.size() == 0 || arrayDefectCenters.size() == 0)
		return 0;

	//Calculate how many bytes needed for store defect information(unit store is 2-byte: unsigned short int)
	//Following this order: Number of defect, coordinate of center defect i(x, y), top left defect i(x, y), bottom right i(x, y): (1 + defect count * 6) * 2 bytes
	int nDefects = arrayDefectCenters.size();
	int nSizeDefectInfo = nDefects * sizeof(CentralizedDefectInfo) + sizeof(int);

	//contains  Track, Doc, Device and Fov index information and defect information
	int nHeaderSize = 4 * 2;
	BYTE* lpMessageImageInfo = new BYTE[abs((int)nSizeDefectInfo) + nHeaderSize];
	BYTE* lpMessageImageInfoOrigin = lpMessageImageInfo;

	UnsignedShortByteUnion* unionConverter = NULL;
	unsigned short int uSize;

	//Track, Doc, Device and Fov index
	//Starting Index: 0
	unionConverter = new UnsignedShortByteUnion;
	uSize = nTrack;
	unionConverter->asUnsignedShort = uSize;
	*lpMessageImageInfo = unionConverter->asBytes[0];
	lpMessageImageInfo++;
	*lpMessageImageInfo = unionConverter->asBytes[1];
	lpMessageImageInfo++;

	//Starting Index: 2
	unionConverter = new UnsignedShortByteUnion;
	uSize = nDoc;
	unionConverter->asUnsignedShort = uSize;
	*lpMessageImageInfo = unionConverter->asBytes[0];
	lpMessageImageInfo++;
	*lpMessageImageInfo = unionConverter->asBytes[1];
	lpMessageImageInfo++;

	//Starting Index: 4
	unionConverter = new UnsignedShortByteUnion;
	uSize = nDevice;
	unionConverter->asUnsignedShort = uSize;
	*lpMessageImageInfo = unionConverter->asBytes[0];
	lpMessageImageInfo++;
	*lpMessageImageInfo = unionConverter->asBytes[1];
	lpMessageImageInfo++;

	//Starting Index: 6
	unionConverter = new UnsignedShortByteUnion;
	uSize = nFov;
	unionConverter->asUnsignedShort = uSize;
	*lpMessageImageInfo = unionConverter->asBytes[0];
	lpMessageImageInfo++;
	*lpMessageImageInfo = unionConverter->asBytes[1];
	lpMessageImageInfo++;

	//--- Save defect Info
	//Defect count, Starting Index: 8
	unionConverter = new UnsignedShortByteUnion;
	uSize = nDefects;
	unionConverter->asUnsignedShort = uSize;
	*lpMessageImageInfo = unionConverter->asBytes[0];
	lpMessageImageInfo++;
	*lpMessageImageInfo = unionConverter->asBytes[1];
	lpMessageImageInfo++;

	int nBufDefectSize = sizeof(CentralizedDefectInfo);
	for (int i = 0; i < nDefects; i++) {
		uint8_t* pBufDefect = new uint8_t[nBufDefectSize];
		CentralizedDefectInfo * pDefectInfo = reinterpret_cast<CentralizedDefectInfo*>(pBufDefect);
		//Store defect center
		pDefectInfo->nCenterX = arrayDefectCenters[i].x;
		pDefectInfo->nCenterY = arrayDefectCenters[i].y;

		//Store Defect Bounding Box
		//Top Left
		pDefectInfo->nTop = arrayDefectRects[i].top;
		pDefectInfo->nLeft = arrayDefectRects[i].left;
		//Bottom Right
		pDefectInfo->nBottom = arrayDefectRects[i].bottom;
		pDefectInfo->nRight = arrayDefectRects[i].right;

		std::memcpy(lpMessageImageInfo, reinterpret_cast<BYTE*>(pBufDefect), nBufDefectSize);
		lpMessageImageInfo += nBufDefectSize;

		delete[] pBufDefect;
	}

	//Create header for buffer to send: contains command ID, buffer length and buffer contents
	int nHeadSize = sizeof(CentralizedHeader);
	int nBufSendSize = nHeadSize + nSizeDefectInfo + nHeaderSize;
	uint8_t* buf = new uint8_t[nHeadSize];
	CentralizedHeader* pHeader = reinterpret_cast<CentralizedHeader*>(buf);
	pHeader->nCommand = SEND_DEFECT_INFO;
	pHeader->nBufferLength = nBufSendSize;

	//Copy header and contents into buffer to send
	char* pBufSend = new char[nBufSendSize];
	std::memcpy(pBufSend, reinterpret_cast<char*>(buf), nHeadSize);
	pBufSend += nHeadSize;
	std::memcpy(pBufSend, lpMessageImageInfoOrigin, nSizeDefectInfo + nHeaderSize);
	pBufSend -= nHeadSize;

	int nSuccess = m_Monitor.SendDataMessage(reinterpret_cast<char*>(pBufSend), nBufSendSize);

	delete unionConverter;
	delete[] lpMessageImageInfoOrigin;
	delete[] buf;
	delete[] pBufSend;

	if (!nSuccess)
		return -1;

	return 0;
}

int CCentralizedCommManager::DecodeDefectDetailsInfoMessage(char* pDefectInfoBufferMsg, int nDefectInfoBufferMsgLength)
{
	try {
		//contains Track, Doc, Device, Fov and defect countinformation 
		int nHeaderSize = 5 * 2;
		if (nDefectInfoBufferMsgLength < nHeaderSize)
			return -1;

		unsigned char* ucMessage = reinterpret_cast<unsigned char*>(pDefectInfoBufferMsg);

		//Starting extract information
		UnsignedShortByteUnion* unionConverter = NULL;
		//Decode Track, Doc, Device, Fov Index
		int nTrack, nDoc, nDevice, nFov;
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[0];
		unionConverter->asBytes[1] = ucMessage[1];
		nTrack = unionConverter->asUnsignedShort;

		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[2];
		unionConverter->asBytes[1] = ucMessage[3];
		nDoc = unionConverter->asUnsignedShort;

		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[4];
		unionConverter->asBytes[1] = ucMessage[5];
		nDevice = unionConverter->asUnsignedShort;

		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[6];
		unionConverter->asBytes[1] = ucMessage[7];
		nFov = unionConverter->asUnsignedShort;

		//Defect count
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[8];
		unionConverter->asBytes[1] = ucMessage[9];
		int nDefectCount = unionConverter->asUnsignedShort;

		int nBufDefectSize = sizeof(CentralizedDefectRegionInfo);
		if (nDefectCount <= 0 || (nHeaderSize + nDefectCount * nBufDefectSize) > nDefectInfoBufferMsgLength) {
			OutputDebugLogTo(2, TRUE, "DecodeDefectDetailsInfoMessage: defect count different with buffer length needed!!!");
			return -1;
		}
		ucMessage += nHeaderSize;

		if (nTrack > MAX_TRACKS || nDoc > MAX_DOCS || nDevice > MAX_DIES || nFov > MAX_FOV ||
			nTrack < 0 || nDoc < 0 || nDevice < 0 || nFov < 0) {
			OutputDebugLogTo(3, TRUE, "Track %d, Doc %d, Die %d, Fov %d - Defect Count %d: INVAILD Defect Info!!!", nTrack, nDoc, nDevice, nFov, nDefectCount);
			return -1;
		}

		CInspectionHandler* pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov];
		pInspHandler->m_Inspection.CleanCentralizedDefectData(GetStationId(), nDevice, nDoc);
		for (int i = 0; i < nDefectCount; i++) {
			uint8_t* pBufDefect = new uint8_t[nBufDefectSize];
			std::memcpy(pBufDefect, ucMessage, nBufDefectSize);
			CentralizedDefectRegionInfo* pDefectInfo = reinterpret_cast<CentralizedDefectRegionInfo*>(pBufDefect);
			OutputDebugLogTo(3, TRUE, "Track %d, Doc %d, Die %d, Fov %d - Defect Center (%d, %d), Region points count: %d)", nTrack, nDoc, nDevice, nFov,
								pDefectInfo->nCenterX, pDefectInfo->nCenterY, pDefectInfo->nDefectPointCount);

			pInspHandler->m_Inspection.m_CentralizedDefectData[GetStationId()][nDevice][nDoc].arrayDefectCenters.push_back(CPoint(pDefectInfo->nCenterX, pDefectInfo->nCenterY));
			pInspHandler->m_Inspection.m_CentralizedDefectData[GetStationId()][nDevice][nDoc].arrayDefectRects.push_back(CRect(pDefectInfo->nLeft, pDefectInfo->nTop,
				pDefectInfo->nRight, pDefectInfo->nBottom));

			ucMessage += nBufDefectSize;

			//X
			int nBufDefectPointSize = sizeof(int)*pDefectInfo->nDefectPointCount;
			uint8_t* pBufDefectPointX = new uint8_t[nBufDefectPointSize];
			std::memcpy(pBufDefectPointX, ucMessage, nBufDefectPointSize);
			int* pDefectX = reinterpret_cast<int*> (pBufDefectPointX);

			std::vector<int> currentDefectX(pDefectInfo->nDefectPointCount);
			std::copy(pDefectX, pDefectX + pDefectInfo->nDefectPointCount, currentDefectX.begin());
			pInspHandler->m_Inspection.m_CentralizedDefectData[GetStationId()][nDevice][nDoc].arrayDefectX.push_back(currentDefectX);

			ucMessage += nBufDefectPointSize;

			//Y
			uint8_t* pBufDefectPointY = new uint8_t[nBufDefectPointSize];
			std::memcpy(pBufDefectPointY, ucMessage, nBufDefectPointSize);
			int* pDefectY = reinterpret_cast<int*> (pBufDefectPointY);

			std::vector<int> currentDefectY(pDefectInfo->nDefectPointCount);
			std::copy(pDefectY, pDefectY + pDefectInfo->nDefectPointCount, currentDefectY.begin());
			pInspHandler->m_Inspection.m_CentralizedDefectData[GetStationId()][nDevice][nDoc].arrayDefectY.push_back(currentDefectY);
			ucMessage += nBufDefectPointSize;


			delete[] pBufDefect;
			delete[] pBufDefectPointX;
			delete[] pBufDefectPointY;
		}
	}
	catch (...) {
		OutputDebugLogTo(2, TRUE, "---ERROR: DecodeDefectDetailsInfoMessage");
		return -1;
	}

	return 0;
}

int CCentralizedCommManager::DecodeDefectInfoMessage(char* pDefectInfoBufferMsg, int nDefectInfoBufferMsgLength)
{
	try {
		//contains Track, Doc, Device, Fov and defect countinformation 
		int nHeaderSize = 5 * 2;
		if (nDefectInfoBufferMsgLength < nHeaderSize)
			return -1;

		unsigned char* ucMessage = reinterpret_cast<unsigned char*>(pDefectInfoBufferMsg);

		//Starting extract information
		UnsignedShortByteUnion* unionConverter = NULL;
		//Decode Track, Doc, Device, Fov Index
		int nTrack, nDoc, nDevice, nFov;
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[0];
		unionConverter->asBytes[1] = ucMessage[1];
		nTrack = unionConverter->asUnsignedShort;

		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[2];
		unionConverter->asBytes[1] = ucMessage[3];
		nDoc = unionConverter->asUnsignedShort;

		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[4];
		unionConverter->asBytes[1] = ucMessage[5];
		nDevice = unionConverter->asUnsignedShort;

		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[6];
		unionConverter->asBytes[1] = ucMessage[7];
		nFov = unionConverter->asUnsignedShort;

		//Defect count
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[8];
		unionConverter->asBytes[1] = ucMessage[9];
		int nDefectCount = unionConverter->asUnsignedShort;

		int nBufDefectSize = sizeof(CentralizedDefectInfo);
		if (nDefectCount <= 0 || (nHeaderSize + nDefectCount * nBufDefectSize) > nDefectInfoBufferMsgLength) {
			OutputDebugLogTo(2, TRUE, "DecodeDefectInfoMessage: defect count different with buffer length needed!!!");
			return -1;
		}
		ucMessage += nHeaderSize;

		if (nTrack > MAX_TRACKS || nDoc > MAX_DOCS || nDevice > MAX_DIES || nFov > MAX_FOV ||
			nTrack < 0 || nDoc < 0 || nDevice < 0 || nFov < 0) {
			OutputDebugLogTo(3, TRUE, "Track %d, Doc %d, Die %d, Fov %d - Defect Count %d: INVAILD Defect Info!!!", nTrack, nDoc, nDevice, nFov, nDefectCount);
			return -1;
		}

		CInspectionHandler* pInspHandler = &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov];
		pInspHandler->m_Inspection.CleanCentralizedDefectData(GetStationId(), nDevice, nDoc);
		for (int i = 0; i < nDefectCount; i++) {
			uint8_t* pBufDefect = new uint8_t[nBufDefectSize];
			std::memcpy(pBufDefect, ucMessage, nBufDefectSize);
			CentralizedDefectInfo* pDefectInfo = reinterpret_cast<CentralizedDefectInfo*>(pBufDefect);
			OutputDebugLogTo(3, TRUE, "Defect Center (%d, %d), Bounding box (%d, %d) - (%d, %d)", pDefectInfo->nCenterX, pDefectInfo->nCenterY,
				pDefectInfo->nLeft, pDefectInfo->nTop, pDefectInfo->nRight, pDefectInfo->nBottom);

			pInspHandler->m_Inspection.m_CentralizedDefectData[GetStationId()][nDevice][nDoc].arrayDefectCenters.push_back(CPoint(pDefectInfo->nCenterX, pDefectInfo->nCenterY));
			pInspHandler->m_Inspection.m_CentralizedDefectData[GetStationId()][nDevice][nDoc].arrayDefectRects.push_back(CRect(pDefectInfo->nLeft, pDefectInfo->nTop,
				pDefectInfo->nRight, pDefectInfo->nBottom));

			delete[] pBufDefect;
			ucMessage += nBufDefectSize;
		}
	}
	catch (...) {
		OutputDebugLogTo(2, TRUE, "---ERROR: DecodeDefectInfoMessage");
		return -1;
	}
	
	return 0;
}

int CCentralizedCommManager::SendImageBufferMessage(int nTrack, int nDoc, int nDevice, int nFov, CImageBuffer & imageBufferToSend)
{
	if (nTrack > MAX_TRACKS || nDoc > MAX_DOCS || nDevice > MAX_DIES || nFov > MAX_FOV)
		return -1;

	BYTE*	pImageArray = imageBufferToSend.GetImgPtr();

	DWORD dwSizeImage = imageBufferToSend.GetImgSize().cx * imageBufferToSend.GetImgSize().cy;
	//contains  Track, Doc, Device and Fov index and image width and image height information and image buffer
	int nHeaderSize = 6 * 2;
	BYTE* lpMessageImageInfo = new BYTE[abs((int)dwSizeImage) + nHeaderSize];
	BYTE* lpMessageImageInfoOrigin = lpMessageImageInfo;

	////Command
	//*lpMessageImageInfo = SEND_IMAGE_BUFFER;
	//lpMessageImageInfo++;
	//*lpMessageImageInfo = ',';
	//lpMessageImageInfo++;

	UnsignedShortByteUnion* unionConverter = NULL;
	unsigned short int uSize;

	//Track, Doc, Device and Fov index
	//Starting Index: 0
	unionConverter = new UnsignedShortByteUnion;
	uSize = nTrack;
	unionConverter->asUnsignedShort = uSize;
	*lpMessageImageInfo = unionConverter->asBytes[0];
	lpMessageImageInfo++;
	*lpMessageImageInfo = unionConverter->asBytes[1];
	lpMessageImageInfo++;

	//Starting Index: 2
	unionConverter = new UnsignedShortByteUnion;
	uSize = nDoc;
	unionConverter->asUnsignedShort = uSize;
	*lpMessageImageInfo = unionConverter->asBytes[0];
	lpMessageImageInfo++;
	*lpMessageImageInfo = unionConverter->asBytes[1];
	lpMessageImageInfo++;

	//Starting Index: 4
	unionConverter = new UnsignedShortByteUnion;
	uSize = nDevice;
	unionConverter->asUnsignedShort = uSize;
	*lpMessageImageInfo = unionConverter->asBytes[0];
	lpMessageImageInfo++;
	*lpMessageImageInfo = unionConverter->asBytes[1];
	lpMessageImageInfo++;

	//Starting Index: 6
	unionConverter = new UnsignedShortByteUnion;
	uSize = nFov;
	unionConverter->asUnsignedShort = uSize;
	*lpMessageImageInfo = unionConverter->asBytes[0];
	lpMessageImageInfo++;
	*lpMessageImageInfo = unionConverter->asBytes[1];
	lpMessageImageInfo++;

	//Starting Index: 8
	unionConverter = new UnsignedShortByteUnion;
	uSize = imageBufferToSend.GetImgSize().cx;
	unionConverter->asUnsignedShort = uSize;
	*lpMessageImageInfo = unionConverter->asBytes[0];
	lpMessageImageInfo++;
	*lpMessageImageInfo = unionConverter->asBytes[1];
	lpMessageImageInfo++;

	//Starting Index: 10
	unionConverter = new UnsignedShortByteUnion;
	uSize = imageBufferToSend.GetImgSize().cy;
	unionConverter->asUnsignedShort = uSize;
	*lpMessageImageInfo = unionConverter->asBytes[0];
	lpMessageImageInfo++;
	*lpMessageImageInfo = unionConverter->asBytes[1];
	lpMessageImageInfo++;

	std::memcpy(lpMessageImageInfo, pImageArray, dwSizeImage);

	//Create header for buffer to send: contains command ID, buffer length and buffer contents
	int nHeadSize = sizeof(CentralizedHeader);
	int nBufSendSize = nHeadSize + dwSizeImage + nHeaderSize;
	uint8_t* buf = new uint8_t[nHeadSize];
	CentralizedHeader* pHeader = reinterpret_cast<CentralizedHeader*>(buf);
	pHeader->nCommand = SEND_IMAGE_BUFFER;
	pHeader->nBufferLength = nBufSendSize;

	//Copy header and contents into buffer to send
	char* pBufSend = new char[nBufSendSize];
	std::memcpy(pBufSend, reinterpret_cast<char*>(buf), nHeadSize);
	pBufSend += nHeadSize;
	std::memcpy(pBufSend, lpMessageImageInfoOrigin, dwSizeImage + nHeaderSize);
	pBufSend -= nHeadSize;
	
	int nSuccess = m_Monitor.SendDataMessage(reinterpret_cast<char*>(pBufSend), nBufSendSize);


	delete unionConverter;
	delete[] lpMessageImageInfoOrigin;
	delete[] buf;
	delete[] pBufSend;

	if(!nSuccess)
		return -1;

	return 0;
}

int CCentralizedCommManager::DecodeAllImageBufferMessage(char* pImageBufferMsg, int nImageBufferMsgLength)
{
	//contains SEND_IMAGE_BUFFER command , Track, Doc, Device, Fov, image width, image height, image buffer information 
	int nHeaderSize = 2;//number of images
	if (nImageBufferMsgLength < nHeaderSize)
		return -1;

	unsigned char* ucMessage = reinterpret_cast<unsigned char*>(pImageBufferMsg);

	//Starting extract information
	UnsignedShortByteUnion* unionConverter = NULL;

	//Decode number of images
	unionConverter = new UnsignedShortByteUnion;
	unionConverter->asBytes[0] = ucMessage[0];
	unionConverter->asBytes[1] = ucMessage[1];
	int nNoImages = unionConverter->asUnsignedShort;
	int nCurrentImageBufferMsgLength = nImageBufferMsgLength - 2;

	//Decode Track, Doc, Device, Fov Index, image width and image height: 6 elements
	nHeaderSize = 6 * 2;
	int nTrack, nDoc, nDevice, nFov;
	for (int i = 0; i < nNoImages; i++) {
		if (nCurrentImageBufferMsgLength < nHeaderSize)
			return -1;

		//Decode Track, Doc, Device, Fov Index
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[2];
		unionConverter->asBytes[1] = ucMessage[3];
		nTrack = unionConverter->asUnsignedShort;

		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[4];
		unionConverter->asBytes[1] = ucMessage[5];
		nDoc = unionConverter->asUnsignedShort;

		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[6];
		unionConverter->asBytes[1] = ucMessage[7];
		nDevice = unionConverter->asUnsignedShort;

		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[8];
		unionConverter->asBytes[1] = ucMessage[9];
		nFov = unionConverter->asUnsignedShort;

		//Decode image size, starting Index: 10
		int nWidth, nHeight;
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[10];
		unionConverter->asBytes[1] = ucMessage[11];
		nWidth = unionConverter->asUnsignedShort;

		unionConverter->asBytes[0] = ucMessage[12];
		unionConverter->asBytes[1] = ucMessage[13];
		nHeight = unionConverter->asUnsignedShort;
		OutputDebugLogTo(2, TRUE, "DecodeImageBufferMessage: Track: %d, Doc: %d, Device: %d, Fov: %d, Image Size (%d, %d)", nTrack, nDoc, nDevice, nFov, nWidth, nHeight);

		nCurrentImageBufferMsgLength -= nHeaderSize;
		int nExpectedReceivedBuffer = nWidth * nHeight;
		if (nExpectedReceivedBuffer > nImageBufferMsgLength || nWidth <= 0 || nHeight <= 0) {
			OutputDebugLogTo(2, TRUE, "Received image buffer not met required length!!!");
			return -1;
		}

		ucMessage += nHeaderSize;
		CImageBuffer imageBuffer;
		imageBuffer.Allocate(CSize(nWidth, nHeight));
		BYTE*	pImageBufferReceived = imageBuffer.GetImgPtr();
		std::memcpy(pImageBufferReceived, ucMessage, nWidth * nHeight);
		ucMessage += (nWidth * nHeight);
		nCurrentImageBufferMsgLength -= (nWidth * nHeight);

		if (nTrack > MAX_TRACKS || nDoc > MAX_DOCS || nDevice > MAX_DIES || nFov > MAX_FOV)
			continue;

		int nXSize = m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDevice][nFov].GetImgSize().cx;
		int nYSize = m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDevice][nFov].GetImgSize().cy;
		if (nXSize == imageBuffer.GetImgSize().cx && nYSize == imageBuffer.GetImgSize().cy) {
			CRect rectTemp = CRect(0, 0, nXSize, nYSize);
			ImgCopy(&imageBuffer, &rectTemp, &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDevice][nFov], &rectTemp);
			OutputDebugLogTo(2, TRUE, "Successfully transfer image!!!");
		}
		else {
			OutputDebugLogTo(2, TRUE, "SEND_IMAGE_BUFFER: image size different!!!");
		}
	}
	
	
	return 0;
}

int CCentralizedCommManager::DecodeImageBufferMessage(char* pImageBufferMsg, int nImageBufferMsgLength)
{
	try {
		//contains Track, Doc, Device, Fov, image width, image height, image buffer information 
		int nHeaderSize = 6 * 2;
		if (nImageBufferMsgLength < nHeaderSize)
			return -1;

		unsigned char* ucMessage = reinterpret_cast<unsigned char*>(pImageBufferMsg);

		//Starting extract information
		UnsignedShortByteUnion* unionConverter = NULL;
		//Decode Track, Doc, Device, Fov Index
		int nTrack, nDoc, nDevice, nFov;
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[0];
		unionConverter->asBytes[1] = ucMessage[1];
		nTrack = unionConverter->asUnsignedShort;

		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[2];
		unionConverter->asBytes[1] = ucMessage[3];
		nDoc = unionConverter->asUnsignedShort;

		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[4];
		unionConverter->asBytes[1] = ucMessage[5];
		nDevice = unionConverter->asUnsignedShort;

		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[6];
		unionConverter->asBytes[1] = ucMessage[7];
		nFov = unionConverter->asUnsignedShort;

		//Decode image size, starting Index: 10
		int nWidth, nHeight;
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asBytes[0] = ucMessage[8];
		unionConverter->asBytes[1] = ucMessage[9];
		nWidth = unionConverter->asUnsignedShort;

		unionConverter->asBytes[0] = ucMessage[10];
		unionConverter->asBytes[1] = ucMessage[11];
		nHeight = unionConverter->asUnsignedShort;
		OutputDebugLogTo(2, TRUE, "DecodeImageBufferMessage: Track: %d, Doc: %d, Device: %d, Fov: %d, Image Size (%d, %d)", nTrack, nDoc, nDevice, nFov, nWidth, nHeight);

		int nExpectedReceivedBuffer = nHeaderSize + (nWidth * nHeight);
		if (nExpectedReceivedBuffer < nImageBufferMsgLength || nWidth <= 0 || nHeight <= 0) {
			OutputDebugLogTo(2, TRUE, "Received image buffer not met required length!!!");

			delete unionConverter;
			return -1;
		}

		if (nTrack > MAX_TRACKS || nDoc > MAX_DOCS || nDevice > MAX_DIES || nFov > MAX_FOV) {
			delete unionConverter;
			return -1;
		}


		CImageBuffer imageBufferReceived;
		imageBufferReceived.Allocate(CSize(nWidth, nHeight));
		BYTE*	pImageBufferReceived = imageBufferReceived.GetImgPtr();
		ucMessage += nHeaderSize;
		std::memcpy(pImageBufferReceived, ucMessage, nWidth * nHeight);

		int nXSize = m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDevice][nFov].GetImgSize().cx;
		int nYSize = m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDevice][nFov].GetImgSize().cy;
		if (nXSize == imageBufferReceived.GetImgSize().cx && nYSize == imageBufferReceived.GetImgSize().cy) {
			CRect rectTemp = CRect(0, 0, nXSize, nYSize);


			//ImgCopy(&imageBufferReceived, &rectTemp, &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDevice][nFov], &rectTemp);
			ImgCopy(&imageBufferReceived, &rectTemp, &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_CentralizedBufferDie[m_Monitor.GetStationId()][nDevice][nFov], &rectTemp);

			OutputDebugLogTo(2, TRUE, "Successfully transfer image!!!");

			////Display sent image
			//m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->DrawView(0);
			//ImgCopy(&imageBufferReceived, &rectTemp, &m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer, &rectTemp);
			//m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->DrawView(1);


			//Check if this last image sent from 
			//int m_nNoOfFov = m_MachineDeviceConifg.nFovPerX * m_MachineDeviceConifg.nFovPerY;
			//if (nFov == m_nNoOfFov - 1 && nTrack == 1 && nDoc == m_pApp->m_pTracks[nTrack].m_nTotalDocs - 1) {
			//	/*SetEvent(m_eventLastImageSent);*/

			//	//Asking for Mapping result from client
			//	m_Monitor.SendCommandMessage(MAPPING_RESULT_INFO);

			//	OutputDebugLogTo(0, TRUE, "Transfer Scanning Image From %s To Centralized Reject PC: %.3f s", m_Monitor.GetClientAcceptedMachineID(), timerTransferImages.GetElapsedMSec() / 1000.0);
			//}

			//First Image was sent
			if (nFov == 0 && nTrack == 0 && nDoc == 0) {
				ResetEvent(m_eventLastImageSent);
				timerTransferImages.Reset();
			}
		}
		else {
			OutputDebugLogTo(2, TRUE, "SEND_IMAGE_BUFFER: image size different!!!");
			return -1;
		}
	}
	catch (...) {
		OutputDebugLogTo(2, TRUE, "---ERROR: DecodeImageBufferMessage");
		return -1;
	}
	
	return 0;
}

BOOL CCentralizedCommManager::SendDataMessage(char nCommand,CString strMessage)
{
	char* pData;
	int i,j=1;
	char* sendData = new char[MAX_BUFFER_MSG_LENGTH];
	int length =  3;

	//sendData[0] = nCommand;
	/*sendData[1] = ASCII_CR;
	sendData[2] = ASCII_LF;*/
	int nLength = strMessage.GetLength();
	
	//sendData[j++] = ',';
	for(i=0;i<strMessage.GetLength()+1;i++)
	{
		sendData[i] = strMessage[i];
	}
	sendData[i-1] = ASCII_CR;

	pData = sendData;//  = sendData;

	//Create header for buffer to send: contains command ID, buffer length and buffer contents
	int nHeadSize = sizeof(CentralizedHeader);
	int nMsgContentsSize = sizeof(char)*i;
	int nBufSendSize = nHeadSize + nMsgContentsSize;

	uint8_t* buf = new uint8_t[nHeadSize];
	CentralizedHeader* pHeader = reinterpret_cast<CentralizedHeader*>(buf);
	pHeader->nCommand = nCommand;
	pHeader->nBufferLength = nBufSendSize;

	//Create buffer contents
	char* pMsgContents = new char[nMsgContentsSize];
	std::memcpy(pMsgContents, sendData, nMsgContentsSize);

	//Copy header and contents into buffer to send
	char* pBufSend = new char[nBufSendSize];
	std::memcpy(pBufSend, reinterpret_cast<char*>(buf), nHeadSize);
	pBufSend += nHeadSize;
	std::memcpy(pBufSend, pMsgContents, nMsgContentsSize);
	pBufSend -= nHeadSize;

	BOOL bError = m_Monitor.SendDataMessage(pBufSend, nBufSendSize);

	//BOOL bError = m_Monitor.SendDataMessage(pData, i);

	if(nCommand == SEND_MESSAGE)
		LogVisionMessage(strMessage, nCommand);
	
	delete[] sendData;
	delete[] buf;
	delete[] pMsgContents;
	delete[] pBufSend;

	return bError;
}

int CCentralizedCommManager::SendWarningMessage(CString strWarningMsg)
{
	char* pData;
	int i, j = 1;
	char* sendData = new char[MAX_BUFFER_MSG_LENGTH];
	int length = 3;

	int nLength = strWarningMsg.GetLength();

	for (i = 0; i < strWarningMsg.GetLength() + 1; i++)
	{
		sendData[i] = strWarningMsg[i];
	}
	sendData[i - 1] = ASCII_CR;

	pData = sendData;//  = sendData;

	//Create header for buffer to send: contains command ID, buffer length and buffer contents
	int nHeadSize = sizeof(CentralizedHeader);
	int nMsgContentsSize = sizeof(char)*i;
	int nBufSendSize = nHeadSize + nMsgContentsSize;

	uint8_t* buf = new uint8_t[nHeadSize];
	CentralizedHeader* pHeader = reinterpret_cast<CentralizedHeader*>(buf);
	pHeader->nCommand = WARNING;
	pHeader->nBufferLength = nBufSendSize;

	//Create buffer contents
	char* pMsgContents = new char[nMsgContentsSize];
	std::memcpy(pMsgContents, sendData, nMsgContentsSize);

	//Copy header and contents into buffer to send
	char* pBufSend = new char[nBufSendSize];
	std::memcpy(pBufSend, reinterpret_cast<char*>(buf), nHeadSize);
	pBufSend += nHeadSize;
	std::memcpy(pBufSend, pMsgContents, nMsgContentsSize);
	pBufSend -= nHeadSize;

	BOOL bSuccess = m_Monitor.SendDataMessage(pBufSend, nBufSendSize);

	
	LogVisionMessage(strWarningMsg, WARNING);

	delete[] sendData;
	delete[] buf;
	delete[] pMsgContents;
	delete[] pBufSend;

	return !bSuccess;
}

int CCentralizedCommManager::SendStopMachine()
{
	char* pData;
	int i = 1;
	char* sendData = new char[MAX_BUFFER_MSG_LENGTH];
	
	sendData[i - 1] = ASCII_CR;

	pData = sendData;

	//Create header for buffer to send: contains command ID, buffer length and buffer contents
	int nHeadSize = sizeof(CentralizedHeader);
	int nMsgContentsSize = sizeof(char)*i;
	int nBufSendSize = nHeadSize + nMsgContentsSize;

	uint8_t* buf = new uint8_t[nHeadSize];
	CentralizedHeader* pHeader = reinterpret_cast<CentralizedHeader*>(buf);
	pHeader->nCommand = STOPMACHINE;
	pHeader->nBufferLength = nBufSendSize;

	//Create buffer contents
	char* pMsgContents = new char[nMsgContentsSize];
	std::memcpy(pMsgContents, sendData, nMsgContentsSize);

	//Copy header and contents into buffer to send
	char* pBufSend = new char[nBufSendSize];
	std::memcpy(pBufSend, reinterpret_cast<char*>(buf), nHeadSize);
	pBufSend += nHeadSize;
	std::memcpy(pBufSend, pMsgContents, nMsgContentsSize);
	pBufSend -= nHeadSize;

	BOOL bSuccess = m_Monitor.SendDataMessage(pBufSend, nBufSendSize);

	LogVisionMessage("", STOPMACHINE);

	delete[] sendData;
	delete[] buf;
	delete[] pMsgContents;
	delete[] pBufSend;

	return !bSuccess;
}

//UINT MessageParser(LPVOID pParam)
//{
//	CCentralizedCommManager* pManager = (CCentralizedCommManager*) pParam;
//	CCommMonitor* pMonitor  = (CCommMonitor*) &pManager->m_Monitor;
//
//	while(1) {
//		/*if(WaitForMultipleObjects(2, handles, FALSE, INFINITE) == WAIT_OBJECT_0) {
//			OutputDebugLogTo(1, TRUE, "MessageParser thread closed");
//			return 0;
//		}*/
//
//		WaitForSingleObject(pMonitor->m_hMessage, INFINITE);
//
//		if(pManager->m_Monitor.m_bClosing)
//			break;
//
//		pMonitor->RemoveFromMessage();
//
//		if(pManager->m_Message.nDataLength < 1)
//			continue;
//
//		ResetEvent(pManager->m_Message.hHandshakeEvent);
//		SetEvent(pManager->m_Message.hMessageEvent);
//		WaitForSingleObject(pManager->m_Message.hHandshakeEvent, INFINITE);
//	}
//
//	return 0;
//}
//
//UINT MessageServer(LPVOID pParam)
//{
//	CCentralizedCommManager* pManager = (CCentralizedCommManager*) pParam;
//	while(1) {
//		WaitForSingleObject(pManager->m_Message.hMessageEvent, INFINITE);
//		if(pManager->m_Monitor.m_bClosing)
//			break;
//		 
//		pManager->ServiceMessage();
//		if(pManager->m_Monitor.m_bClosing)
//			break;
//
//		SetEvent(pManager->m_Message.hHandshakeEvent);
//	}
//	return -1;
//}


//UINT CommSendThread(LPVOID pParam)
//{
//	CCentralizedCommManager* pManager = (CCentralizedCommManager*) pParam;
//	while(1) {
//		WaitForSingleObject(pManager->m_pApp->m_EventCommSendMsg, INFINITE);
//		if(pManager->m_pApp->m_bExitApp)
//			break;
//		 
//		pManager->SendOkToIndexMsg();
//		ResetEvent(pManager->m_pApp->m_EventCommSendMsg);
//	}
//	return -1;
//}


void CCentralizedCommManager::LogMachineMessage(char Command, CString strShortMessage, CString strMessage, CString strData)
{
	CString strOutputMessage;
	strOutputMessage.Format("MACHINE::[%c]  %s %s", Command , strShortMessage, strData);
	//m_pApp->SendLogDisplay(strOutputMessage);

	OutputDebugLogTo(2, TRUE, "MACHINE->   %s    %s [%c] [%XH]", 
		strShortMessage,  
		strMessage, 
		Command,
		Command);
}

int CCentralizedCommManager::GetStationId()
{
	return m_nStation;
}


void CCentralizedCommManager::LogVisionMessage(CString strShortMessage, char nCommand)
{
	CString strOutputMessage;
	strOutputMessage.Format("VISION::[%c]  %s", nCommand , strShortMessage);
	//m_pApp->SendLogDisplay(strOutputMessage);

	OutputDebugLogTo(2, TRUE, "Vision------->:   %s   [%C] [%XH]", 
					strShortMessage, 
					nCommand,
					nCommand);
}


int CCentralizedCommManager::ServiceMessage()
{
	if (theApp.m_bExitApp)
		return 0;

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;

	CString strCommEventMsg;
	CString strCommand;
	CString strMessage;
	CString strAck;
	SYSTEMTIME sysTime;
	BOOL bUpdateStats	= FALSE;
	int nError			= 0;

	CString strRecipeType	= "";
	CString strCartonType	= "";
	CString strLotNo		= "";

	int nIndex			= 1;

	INT32 nCommand = m_Message.nCommand;
	CAtlString resToken;
	CString respone[50];
	int curPos = 0;
	int i=0;
	
	switch (m_Message.nCommand) {
	//Client: Server asked client for info
	case INFO: {
		SendInfo(INFOACK);
		OutputDebugLogTo(2, TRUE, "INFO");
	}			 
						 break;

	//Server: received mapping info from client
	case INFOACK: {
		CMessageContents messageContents;
		messageContents.pData = new char[m_Message.nDataLength];
		messageContents.nDataLength = m_Message.nDataLength;
		std::memcpy(messageContents.pData, m_Message.pData, m_Message.nDataLength);
		DecodeInfo(messageContents.pData, messageContents.nDataLength);
		delete[] messageContents.pData;

		OutputDebugLogTo(2, TRUE, "INFOACK");
	}

			   break;
	//Server: received image buffer from client
	case SEND_IMAGE_BUFFER: {

		if (m_nSendImageCount % 3 == 0) {
			m_pApp->UpdateCentralizedMachineState(GetStationId(), "Sending Images.");
		}
		else if (m_nSendImageCount % 3 == 1) {
			m_pApp->UpdateCentralizedMachineState(GetStationId(), "Sending Images..");
		}
		else {
			m_pApp->UpdateCentralizedMachineState(GetStationId(), "Sending Images...");
		}
		m_nSendImageCount++;

		if (m_nSendImageCount == 100)
			m_nSendImageCount = 0;

		int nTrackRecv, nDocRecv, nDeviceRecv, nFovRecv;
		m_pApp->m_bUpdatingMapping = TRUE;
		int nError = DecodeImageBufferMessage(m_Message.pData, m_Message.nDataLength);
		if (nError)
			OutputDebugLogTo(2, TRUE, "SEND_IMAGE_BUFFER : failed to decode image buffer!!!");
		
		m_pApp->m_bUpdatingMapping = FALSE;
	}
					 break;
	
	//Server: received a list of image buffer from client
	case SEND_LIST_IMAGE_BUFFER: {
		int nError = DecodeAllImageBufferMessage(m_Message.pData, m_Message.nDataLength);
		if (nError)
			OutputDebugLogTo(2, TRUE, "SEND_LIST_IMAGE_BUFFER : failed to decode image buffer!!!");
	}
	break;

	//Server: received mapping result ready from Client
	case MAPPING_RESULT_READY: {
		m_pApp->UpdateCentralizedMachineState(GetStationId(), "Inspection Completed");

		SetEvent(m_eventLastImageSent);

		int nError = !m_Monitor.SendCommandMessage(MAPPING_RESULT_INFO);
		if (nError)
			OutputDebugLogTo(2, TRUE, "MAPPING_RESULT_READY : failed to send MAPPING_RESULT_INFO!!!");
		else
			OutputDebugLogTo(2, TRUE, "MAPPING_RESULT_READY");
	}

							  break;

	//Client: received mapping result request for from Server
	case MAPPING_RESULT_INFO: {
		theApp.m_pImageQueue->PushResultCentralized();
		OutputDebugLogTo(2, TRUE, "MAPPING_RESULT_INFO");
	}

						 break;

	//Server: received mapping result from client
	case MAPPING_RESULT: {
		m_pApp->UpdateCentralizedMachineState(GetStationId(), "Connected");

		EnterCriticalSection(&m_pApp->m_csCentralizedQueue);
		//Check if Station ID is in queue or not
		if (m_pApp->m_mapCentralizedStationQueue.find(GetStationId()) != m_pApp->m_mapCentralizedStationQueue.end()) {
			LeaveCriticalSection(&m_pApp->m_csCentralizedQueue);
			break;
		}
		m_pApp->m_CentralizedStationQueue.push(GetStationId());
		m_pApp->m_mapCentralizedStationQueue.insert(std::pair<int, int>(GetStationId(), 0));
		LeaveCriticalSection(&m_pApp->m_csCentralizedQueue);

		theApp.UpdateCentralizedMachineState(GetStationId(), "In queue for verification...");

		ResetEvent(m_eventVerificationDone);
		SetEvent(m_pApp->m_pImageQueue->m_hPushedStationQueueCentralized);


		CMessageContents messageContents;
		messageContents.pData = new char[m_Message.nDataLength];
		messageContents.nDataLength = m_Message.nDataLength;
		std::memcpy(messageContents.pData, m_Message.pData, m_Message.nDataLength);
		resultCommandQueue.push(messageContents);
		SetEvent(m_eventResultCommand);

		strCommEventMsg.Format("Comm Machine ID %s: MAPPING_RESULT", m_Monitor.GetClientAcceptedMachineID());
		m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
	}
						
						 break;
	//Server: received defect info from client
	case SEND_DEFECT_INFO: {
		CMessageContents messageContents;
		//DecodeDefectInfoMessage(m_Message.pData, m_Message.nDataLength);
		DecodeDefectDetailsInfoMessage(m_Message.pData, m_Message.nDataLength);

		strCommEventMsg.Format("Comm Machine ID %s: SEND_DEFECT_INFO", m_Monitor.GetClientAcceptedMachineID());
		m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
	}
						   break;
	 //Client: received Verification result from server
	case VERIFICATION_RESULT: {	
		DecodeMappingResult(m_Message.pData, m_Message.nDataLength);
		SetEvent(m_pApp->m_EventCommSendMsg);

		if (m_pApp->m_bStream)
			m_pApp->m_bStream = FALSE;

		for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
			for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
				for (int nView = 0; nView < m_pApp->m_nTotalViews; nView++) {
					if (m_pApp->m_bPartInspect[nDie])
						m_pApp->UpdateLotSummary(nDie, nFov, nView, m_pApp->m_nErrorCode[nDie][nFov][nView]);
				}
			}
			if (m_pApp->m_bPartInspect[nDie])
				m_pApp->UpdateTotalLotSummary(nDie, m_pApp->m_nDieErrorCodeStats[nDie]);
		}
		m_pApp->UpdateStatisticsWindow();

		strCommEventMsg.Format("Comm Machine ID %s: VERIFICATION_RESULT", m_Monitor.GetClientAcceptedMachineID());
		m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);

		//Send back VERIFICATION_RESULT ACK to server 
		CString msgTemp = "VERIFICATION_RESULT ACK";
		SendDataMessage(VRESULTACK, msgTemp);
	}
							  break;

	//Server: received Verification result ACK from client
	case VRESULTACK: {
		//Asking Info again to update Lot Stats
		m_Monitor.SendCommandMessage(INFO);
		strCommEventMsg.Format("Comm Machine ID %s: VRESULTACK", m_Monitor.GetClientAcceptedMachineID());
		m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
	}
					 break;
	//Client: received re-scan command from server
	case RESCAN: {  
		if (m_pApp->m_bStream)
			m_pApp->m_bStream = FALSE;

		strCommEventMsg.Format("Comm Machine ID %s: RESCAN", m_Monitor.GetClientAcceptedMachineID());
		m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);

		if (m_pApp->m_bCurtainSensor || m_pApp->m_bEmergencyStop || m_pApp->m_bReceivedStopCmd)
		{
			CString strStatus, strTemp;
			if (m_pApp->m_bCurtainSensor)
				strStatus = "CURTAIN SENSOR TRIGGERED";
			if (m_pApp->m_bEmergencyStop)
				strStatus = "EMERGENCY STOP TRIGGERED";
			if (m_pApp->m_bReceivedStopCmd)
				strStatus = "RECEIVED STOP COMMAND";

			int nCmd = RESCANACK;
			SendDataMessage(nCmd, strStatus);

			strCommEventMsg.Format("Machine PC: %s", strStatus);
			m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
		}
		else
		{
			CString strStatus, strTemp;
			int nCmd = RESCANACK;
			strStatus = "RE-SACN STARTED";
			SendDataMessage(nCmd, strStatus);

			strCommEventMsg.Format("Machine PC: %s", strStatus);
			m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);

			m_pApp->m_bReScanSequence = TRUE;
		}
	}
				 break;

	//Server: received RESCANACK from client (included client status)
	case RESCANACK: {
		CString strStatus(m_Message.pData, m_Message.nDataLength);
		strCommEventMsg.Format("Comm Machine ID %s: RESCANACK %s", m_Monitor.GetClientAcceptedMachineID(),  strStatus);
		m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
	}
					break;
	//Received the message
	case SEND_MESSAGE: {
		CString strStatus(m_Message.pData, m_Message.nDataLength);
		m_pApp->UpdateCentralizedMachineState(GetStationId(), strStatus);

		strCommEventMsg.Format("Comm Machine ID %s: SEND_MESSAGE %s", m_Monitor.GetClientAcceptedMachineID(), strStatus);
		m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
	}
					   break;

	//Received the message
	case DISCONNECT: {
		strCommEventMsg.Format("Comm Machine ID %s: DISCONNECT", m_Monitor.GetClientAcceptedMachineID());
		m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
	}
					   break;

	//received Warning message from client
	case WARNING: {
		CString strStatus(m_Message.pData, m_Message.nDataLength);
		if (m_pApp->pErrorMsgDlg[GetStationId()] != NULL) {
			m_pApp->pErrorMsgDlg[GetStationId()]->SetWindowText("System Event");
			m_pApp->pErrorMsgDlg[GetStationId()]->PostModuleStatus(true, m_pApp->m_strClientMachineID[GetStationId()], strStatus);
		}

		strCommEventMsg.Format("Comm Machine ID %s: WARNING", m_Monitor.GetClientAcceptedMachineID());
		theApp.AddLogEvent(strCommEventMsg, COMM_LOG);
	}
					   break;

	//received Stop Machine From Centralized Reject PC (Server)
	case STOPMACHINE: {
		int nCmd = STOPMACHINE;
		CString strMsg;
		strMsg.Format("%d", nCmd);
		int nError = SendDataMessage(nCmd, strMsg);

		strCommEventMsg.Format("Comm Machine ID %s: STOPMACHINE", m_Monitor.GetClientAcceptedMachineID());
		theApp.AddLogEvent(strCommEventMsg, COMM_LOG);
	}
				  break;
	}

	return 0;
}

int CCentralizedCommManager::SetStation(int nTrack)
{
	m_nStation	= nTrack - 1;
	if(m_nStation < 0)
		m_nStation = 0;

	//m_nStation = 0; ///Always set to zero for 1 Station....
	m_pDoc = m_pApp->m_pTracks[m_nStation].m_pDoc[0];
	return 0;
}


void CCentralizedCommManager::LoadRecipe()
{
	m_pApp->m_pMainWnd->SendMessage(WM_LOAD_RECIPE_SERVER);
	OutputDebugLogTo(0, TRUE, "Loading Recipe from Controller");
}


void CCentralizedCommManager::SendOkToIndexMsg()
{

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	m_pApp->motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfDies = nDevicePerX * nDevicePerY;


	CString strCommEventMsg;
	CString strStatus = "READY";
	int nCmd = OKTOINDEX;
	CString strTemp,strTemp1;
	strTemp = m_pApp->m_strPartFlex[0];
	for(int k=1;k<m_nNoOfDies;k++)
	{
		strTemp1.Format("%s','%s",strTemp,m_pApp->m_strPartFlex[k]);
		strTemp = strTemp1;
	}
	strTemp.Format("%d,'%s','%s'",OKTOINDEX,strTemp,strStatus);
	SendDataMessage(nCmd,strTemp);
	strCommEventMsg.Format("Vision : %s", strTemp);
	m_pApp->AddLogEvent(strCommEventMsg, COMM_LOG);
}

UINT VerificationResultsThread(LPVOID lpData)
{
	CCentralizedCommManager *pCommManager = (CCentralizedCommManager*)lpData;
	CString strMessage;
	CString strCommEventMsg;

	while (1) {
		if (WaitForSingleObject(pCommManager->m_eventResultCommand, INFINITE) != WAIT_OBJECT_0) {
			OutputDebugLogTo(0, TRUE, "-----ERROR : Verificationn Result thread 01 synchronization has been failed. Closing thread...");
			break;
		}

		if (theApp.m_bExitApp)
			break;


		if (!pCommManager->resultCommandQueue.empty()) {
			CMessageContents messageContents = pCommManager->resultCommandQueue.front();
			pCommManager->resultCommandQueue.pop();

			/*if (WaitForSingleObject(pCommManager->m_eventLastImageSent, INFINITE) != WAIT_OBJECT_0) {
				OutputDebugLogTo(0, TRUE, "-----ERROR : Waiting Last Image Sent Event has been failed");
			}*/

			if (WaitForSingleObject(pCommManager->m_eventVerification, INFINITE) != WAIT_OBJECT_0) {
				OutputDebugLogTo(0, TRUE, "-----ERROR : Waiting Verification Event has been failed");
			}
			
			//Display Images
			int m_nNoOfDies = pCommManager->m_MachineDeviceConifg.nDevicePerX * pCommManager->m_MachineDeviceConifg.nDevicePerY;
			int m_nNoOfFov = pCommManager->m_MachineDeviceConifg.nFovPerX * pCommManager->m_MachineDeviceConifg.nFovPerY;
			for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
				for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
					for (int nTrack = 0; nTrack < theApp.m_nTotalTracks; nTrack++) {
						for (int nDoc = 0; nDoc < theApp.m_nTotalViewDocs[nTrack]; nDoc++) {
							//Display sent image
							int nXSize = theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDie][nFov].GetImgSize().cx;
							int nYSize = theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDie][nFov].GetImgSize().cy;
							CRect rectTemp = CRect(0, 0, nXSize, nYSize);

							theApp.m_pTracks[nTrack].m_pDoc[nDoc]->DrawView(0);
							ImgCopy(&theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_CentralizedBufferDie[pCommManager->GetStationId()][nDie][nFov], 
								&rectTemp, &theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer, &rectTemp);
							theApp.m_pTracks[nTrack].m_pDoc[nDoc]->DrawView(1);
						}
					}
				}
			}

			EnterCriticalSection(&theApp.m_csLoadConfig);

			theApp.m_pMainWnd->SendMessage(WM_RESIZE_DIE_MAPPING, (WPARAM)&pCommManager->m_MachineDeviceConifg, NULL);

			LeaveCriticalSection(&theApp.m_csLoadConfig);

			pCommManager->DecodeMappingResult(messageContents.pData, messageContents.nDataLength);
			delete[] messageContents.pData;

			theApp.m_pImageSelectionDlg->m_wndDlg.EnableVerification(TRUE);

		}
		ResetEvent(pCommManager->m_eventResultCommand);

		ResetEvent(pCommManager->m_eventLastImageSent);
		ResetEvent(pCommManager->m_eventVerification);
	}

	return 0;
}
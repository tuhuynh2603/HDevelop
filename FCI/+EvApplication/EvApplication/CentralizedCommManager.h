/// Communication Manager
/// Udupa; Jan'11

#pragma once

#include "CentralizedCommMonitor.h"
#include <queue>
#include "BitmapFunctions.h"
#include "PerformanceTimer.h"

//UINT MessageParser(LPVOID pParam);
//UINT MessageServer(LPVOID pParam);
//UINT CommSendThread(LPVOID pParam);
class CApplication;
class CAppDoc;


class CENTRALIZEDSUMMARY {

public:
	CENTRALIZEDSUMMARY();

	long lTotalUnitsInspected;
	long lTotalUnitsPass;
	long lTotalUnitsFail;
	long lTotalUnitsFalseRejection;
	double dTotalYield;

	CString strMachineState;
};


class CCentralizedCommManager
{
public:
	CCentralizedCommManager(int nStation);
	~CCentralizedCommManager();

	void Initialize();
	void Close();

	int ServiceMessage();

	int DecodeImageBufferMessage(char* pImageBufferMsg, int nImageBufferMsgLength);
	int DecodeAllImageBufferMessage(char* pImageBufferMsg, int nImageBufferMsgLength);
	int SendImageBufferMessage(int nTrack, int nDoc, int nDevice, int nFov, CImageBuffer& imageBufferToSend);
	int SendAllImageBufferMessage();
	int SendDefectInfoMessage(int nTrack, int nDoc, int nDevice, int nFov, const std::vector<CPoint>& arrayDefectCenters, const std::vector<CRect>& arrayDefectRects);
	int SendDefectInfoMessage(int nTrack, int nDoc, int nDevice, int nFov, const std::vector<CPoint>& arrayDefectCenters, const std::vector<CRect>& arrayDefectRects, std::vector<std::vector<int>>& arrayDefectX, std::vector<std::vector<int>>& arrayDefectY);
	int DecodeDefectInfoMessage(char* pDefectInfoBufferMsg, int nDefectInfoBufferMsgLength);
	int DecodeDefectDetailsInfoMessage(char* pDefectInfoBufferMsg, int nDefectInfoBufferMsgLength);
	int SendWarningMessage(CString strWarningMsg);
	int SendStopMachine();

	int SendInfo(BYTE nCommand);
	int SendMappingResult(BYTE nCommand);
	int DecodeMappingResult(char* pImageBufferMsg, int nImageBufferMsgLength);
	int DecodeInfo(char* pImageBufferMsg, int nImageBufferMsgLength);

	BOOL SendDataMessage(char nCommand,CString strMessage);

	void LogVisionMessage(CString strShortMessage, char nCommand);
	void LogMachineMessage(char Command, CString strShortMessage, CString strMessage, CString strData = "");

	void LoadRecipe();
	void SendOkToIndexMsg();
	std::queue <CMessageContents > resultCommandQueue;

	CApplication* m_pApp;
	CAppDoc* m_pDoc;

	int GetStationId();
	/*HANDLE m_hMessageParser;
	HANDLE m_hMessageServer;*/
	CCommMessage m_Message;

	CCentralizedCommMonitor m_Monitor;
	BOOL isValid;
	HANDLE m_eventResultCommand;
	HANDLE m_eventVResultCommand;

	//Event to inform that last image was sent from Client Machine
	HANDLE m_eventLastImageSent;

	HANDLE m_eventVerification;
	HANDLE m_eventVerificationDone;

	CentralizedDeviceConfigInfo m_MachineDeviceConifg;

	CPerformanceTimer timerTransferImages;

	CENTRALIZEDSUMMARY m_CentralizedSummary;
	
	int m_nSendImageCount;
private:	
	int SetStation(int nTrack);
	
	
protected:
	int m_nStation;
};

UINT VerificationResultsThread(LPVOID lpData);


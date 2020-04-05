/// Communication Manager
/// Udupa; Jan'11
/// Modified for 2D-Wire.. E Santosh Jul'12

#pragma once

#include "CommMonitor.h"

//UINT MessageParser(LPVOID pParam);
//UINT MessageServer(LPVOID pParam);
UINT CommSendThread(LPVOID pParam);
class CApplication;
class CAppDoc;

class CCommManager
{
public:
	CCommManager(int nStation);
	~CCommManager();

	void Initialize();
	void Close();

	int ServiceMessage();

	BOOL SendDataMessage(char nCommand,CString strMessage);

	void LogVisionMessage(CString strShortMessage, char nCommand);
	void LogMachineMessage(char Command, CString strShortMessage, CString strMessage, CString strData = "");

	void LoadRecipe();
	void SendOkToIndexMsg();

	CApplication* m_pApp;
	CAppDoc* m_pDoc;

	/*HANDLE m_hMessageParser;
	HANDLE m_hMessageServer;*/
	CCommMessage m_Message;

	CCommMonitor m_Monitor;
	BOOL isValid;

private:	
	int SetStation(int nTrack);
	
	
protected:
	int m_nStation;
};


#pragma once

#include <afxtempl.h>
#include <afxmt.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "CommInterface.h"

#define BUFLEN 255

UINT CommMonitorThread(LPVOID lpData);

class CApplication;

class CCommMonitor
{
public:
	CCommMonitor();
	BOOL Create(CCommInterface* pCommInterface, int nStation);
	BOOL Close();

	BOOL RegisterServer();
	BOOL RegisterClient();

	BOOL SendDataMessage(char* pData, int nLength);
	void AddToMessage(char* pData, int nLength);
	//void RemoveFromMessage();
	
	CCommInterface* m_pCommInterface;
	CString m_strPortNo;
	SOCKET m_Socket;

	//HANDLE m_hMessage;
	HANDLE m_hDisconnect;

	BOOL m_bClosing;

protected:
	CApplication* m_pApp;
	int m_nStation;

	HANDLE m_hTcpMonitorThread;

	CRITICAL_SECTION m_csMessageList;
};


#pragma once

#include <afxtempl.h>
#include <afxmt.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <map>
#include "CentralizedCommInterface.h"

//Default Image size is 2048x2048 : Maximum buffer lengith is more will be 2048x2048 ~ 4194304
#define MAX_BUFFER_MSG_LENGTH 4194304
//Default Image size is 2048x2048 : image need 4194304 --> assume maximum image is 50 * 4194304 + 255(For attached information like image size, bla bla) = 209715455
#define MAXBUFFERIMAGES 209715455

UINT CentralizedCommMonitorThread(LPVOID lpData);

class CApplication;

class CCentralizedCommMonitor
{
public:
	CCentralizedCommMonitor();
	BOOL Create(CCentralizedCommInterface* pCommInterface, int nStation);
	BOOL Close();

	BOOL RegisterServer();
	BOOL RegisterClient();

	BOOL SendCommandMessage(char nCommand);
	BOOL SendDataMessage(char* pData, int nLength);
	void AddToMessage(char* pData, int nLength);
	//void RemoveFromMessage();
	
	CCentralizedCommInterface* m_pCommInterface;
	CString m_strPortNo;
	SOCKET m_Socket;

	//HANDLE m_hMessage;
	HANDLE m_hDisconnect;

	BOOL m_bClosing;
	BOOL m_bConnected;

	
	CString m_CurrentClientIPv4Addr;
	void InsertClientAccepted(CString strIPv4Addr, BOOL bConnected);
	//Set Current Client Accepted Status
	void SetCurrentClientAcceptedStatus(BOOL bConnected);
	
	//Get the list client IPv4 Address and its status was accepted by server
	 void GetClientAcceptedInfo(CString &strMachineID, CString &strIPv4, BOOL &bConnectedStatus);

	 CString GetClientAcceptedMachineID();

	 //Set Current Client Accepted Machine ID
	 void SetCurrentClientAcceptedMachineId(CString strMachineId);

	 void DisconnectSocket();

	 int GetStationId();
protected:
	CApplication* m_pApp;
	int m_nStation;

	//contains list of IPv4 address of client was accepted and its status(currently connected to server or not)
	BOOL m_ClientAcceptedStatus;
	CString m_ClientMachineID;
	CString m_ClientMachineIPAddress;

	HANDLE m_hTcpMonitorThread;

	CRITICAL_SECTION m_csMessageList;
};

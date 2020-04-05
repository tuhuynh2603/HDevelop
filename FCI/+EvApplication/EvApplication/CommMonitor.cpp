/// TCP/IP monitor class implementation
/// Udupa; Jan'2011
/// Modified for 2D-Wire.. E Santosh Jul'12

#include "stdafx.h"
#include "Application.h"
#include "CommMonitor.h"
#include "CommManager.h"
#include "Resource.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>


CCommMonitor::CCommMonitor()
{
	m_hDisconnect = NULL;
	m_hTcpMonitorThread = 0;
	m_pApp = NULL;
	m_strPortNo = "";
}

BOOL CCommMonitor::Create(CCommInterface* pCommInterface, int nStation)
{
	m_bClosing = FALSE;

	m_pApp = (CApplication*) AfxGetApp();
	InitializeCriticalSection(&m_csMessageList);
	m_hDisconnect = CreateEventA(NULL, TRUE, FALSE, NULL);
	if(m_hDisconnect == INVALID_HANDLE_VALUE) {
		m_hDisconnect = NULL;
		::AfxMessageBox("Unable to create event");
		return FALSE;
	}
	/*if((m_hMessage = CreateEventA(NULL, FALSE, FALSE, NULL)) == INVALID_HANDLE_VALUE) {
		m_hMessage = NULL;
		::AfxMessageBox("Unable to create event");
		return FALSE;
	}*/

	m_pCommInterface = pCommInterface;
	m_nStation = nStation;
	m_Socket = INVALID_SOCKET;

	CWinThread* pThread = AfxBeginThread(CommMonitorThread, this);
	m_hTcpMonitorThread = pThread->m_hThread;
	if(m_hTcpMonitorThread == INVALID_HANDLE_VALUE) {
		SetEvent(m_hDisconnect);
		::AfxMessageBox("Failed to creaate TCP/IP thread");
		return FALSE;
	}

	return TRUE;
}

BOOL CCommMonitor::Close()
{
	m_bClosing = TRUE;
	if(m_hDisconnect != NULL) {
		SetEvent(m_hDisconnect);
		// E Santosh.. Debug Problem... 2012
		DeleteCriticalSection(&m_csMessageList);
	}

	return TRUE;
}

BOOL CCommMonitor::SendDataMessage(char* pData, int nLength)
{
	if(m_Socket == INVALID_SOCKET) {
		OutputDebugLogTo(1, TRUE, "No valid socket found");
		return FALSE;
	}

	if(pData != NULL && nLength > 0) {
		int iSendResult = send(m_Socket, pData, nLength, 0);
		if(iSendResult == SOCKET_ERROR) {
			OutputDebugLogTo(1, TRUE, "send failed with error: %d", WSAGetLastError());
			closesocket(m_Socket);
			return FALSE;
		}
		OutputDebugLogTo(1, TRUE, "Bytes sent: %d", iSendResult);
	}

	return TRUE;
}

//void CCommMonitor::AddToMessage(char* pData, int nLength)
//{
//
//	if(pData == NULL || nLength < 1) {
//		OutputDebugLogTo(1, TRUE, "Truncated Message received from Controller [%d]", nLength);
//		return;
//	}
//
//	CCommMessage* pMessage;
//	char* pDest = new char[nLength];
//
//	pMessage = &m_pCommInterface->m_pCommManager[0]->m_Message;
//	memcpy(pDest, pData, nLength);
//	 
//	EnterCriticalSection(&m_csMessageList);
//	pMessage->nDataLength = nLength;
//	pMessage->nCommand	  = pData[0];
//	for(int i=1; i<nLength-1; i++){
//		if(pData[i] == ASCII_CR && pData[i+1] == ASCII_LF)
//			pMessage->nDataLength = i+1;
//	}
//	pMessage->pData = &pDest[1]; 
//
//	delete[] pDest;
//	SetEvent(m_hMessage);
//	LeaveCriticalSection(&m_csMessageList);
//}


void CCommMonitor::AddToMessage(char* pData, int nLength)
{

	if(pData == NULL || nLength < 1) {
		OutputDebugLogTo(1, TRUE, "Truncated Message received from Controller [%d]", nLength);
		return;
	} 

	CCommMessage* pMessage;
	//char* pDest = new char[nLength];

	pMessage = &m_pCommInterface->m_pCommManager[0]->m_Message;
	//memcpy(pDest, pData, nLength);
	 
	EnterCriticalSection(&m_csMessageList);
	pMessage->nDataLength = nLength;
	pMessage->nCommand	  = pData[0];
	for(int i=1; i<nLength-1; i++){
		if(pData[i] == ASCII_CR || pData[i] == ASCII_LF){
			pMessage->nDataLength = i+1;
			break;
		}
	}
	pMessage->pData = &pData[0]; 
	m_pCommInterface->m_pCommManager[0]->ServiceMessage();
	//delete[] pDest;
 	//SetEvent(m_hMessage);
	LeaveCriticalSection(&m_csMessageList);
}

//void CCommMonitor::RemoveFromMessage()
//{
//	EnterCriticalSection(&m_csMessageList);
//
//	if(m_pCommInterface->m_pCommManager[0]->m_Message.nDataLength > 0)
//	SetEvent(m_hMessage);
//	LeaveCriticalSection(&m_csMessageList);
//}

UINT CommMonitorThread(LPVOID lpData)
{
	CCommMonitor *pTcpMonitor = (CCommMonitor *)lpData;
	ASSERT(pTcpMonitor);
	CCommInterface *pCommInterface = pTcpMonitor->m_pCommInterface;

	char recvbuf[BUFLEN];
	char recvbufTemp[BUFLEN];
	int nResult;

	for(int p=0; p<BUFLEN ; p++){
		recvbuf[p] = '\0';
		recvbufTemp[p] = '\0';
	}
	while(1) {
		if(pCommInterface->m_bIsTCPServer)
			pTcpMonitor->RegisterServer();
		else
			pTcpMonitor->RegisterClient();

		do {
			nResult = recv(pTcpMonitor->m_Socket, recvbuf, BUFLEN, 0);
			if(nResult > 0){
				CString strReceive;
				strReceive = CString(recvbuf);
				//strReceive  =",'3456789',3456789','3456789',1,1,1,'345uknn','receipe'";
				/*memset(recvbuf, 0, strReceive.GetLength());
				strcpy_s(recvbuf, strReceive.GetBuffer(0));*/
				OutputDebugLogTo(1, TRUE, "PLC Send Data : [%s]", strReceive);

				for(int p=0; p<BUFLEN ; p++)
					recvbufTemp[p] = '\0';

				for(int k=0; k<nResult; k++)
					recvbufTemp[k] = recvbuf[k];

				pTcpMonitor->AddToMessage(recvbufTemp, nResult);

				for(int p=0; p<BUFLEN ; p++)
					recvbuf[p] = '\0';
			}
			else if(nResult == 0)
				OutputDebugLogTo(1, TRUE, "Connection closed by Controller");
			else {
				closesocket(pTcpMonitor->m_Socket);
				break;
			}

		} while(nResult > 0);


		if(pCommInterface->m_bIsTCPServer){
			if(pTcpMonitor->m_bClosing) {
				OutputDebugLogTo(1, TRUE, "TCP/IP comm : Connection closed.");
				return 0;
			}

			nResult = shutdown(pTcpMonitor->m_Socket, SD_SEND);
			if(nResult == SOCKET_ERROR) {
				closesocket(pTcpMonitor->m_Socket);
				Sleep(1000);
			}
		}

		closesocket(pTcpMonitor->m_Socket);
	}

	return 0;
}

int CCommMonitor::RegisterServer()
{
	SOCKET ServerSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, hints;


	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	m_strPortNo = m_pCommInterface->m_strPortNumbers[m_nStation];
	int nResult = getaddrinfo(NULL, m_strPortNo, &hints, &result);
	if(nResult != 0) {
		//OutputDebugLogTo(1, TRUE, "getaddrinfo failed with error: %d", nResult);
		return -1;
	}

	// Create a SOCKET for connecting to server
	ServerSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if(ServerSocket == INVALID_SOCKET) {
		OutputDebugLogTo(1, TRUE, "socket failed with error: %ld", WSAGetLastError());
		freeaddrinfo(result);
		return -1;
	}

	// Setup the TCP listening socket
	nResult = bind(ServerSocket, result->ai_addr,(int)result->ai_addrlen);
	if(nResult == SOCKET_ERROR) {
		OutputDebugLogTo(1, TRUE, "bind failed with error: %d", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ServerSocket);
		return -1;
	}

	freeaddrinfo(result);

	nResult = listen(ServerSocket, SOMAXCONN);
	if(nResult == SOCKET_ERROR) {
		OutputDebugLogTo(1, TRUE, "listen failed with error: %d", WSAGetLastError());
		closesocket(ServerSocket);
		return -1;
	}

	// Accept a client socket
	m_Socket = accept(ServerSocket, NULL, NULL);
	if(m_Socket == INVALID_SOCKET) {
		OutputDebugLogTo(1, TRUE, "accept failed with error: %d", WSAGetLastError());  /// Disconnect Sockets successfully error ..
		return -1;
	}

	OutputDebugLogTo(1, TRUE, "TCP/IP comm [%s]: Registered Controller successfully", m_strPortNo);

	// No longer need server socket
	closesocket(ServerSocket);

	return 0;
}

int CCommMonitor::RegisterClient()
{
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	m_strPortNo = m_pCommInterface->m_strPortNumbers[m_nStation];
	int nResult = getaddrinfo(m_pCommInterface->m_strIpAddress, m_strPortNo, &hints, &result);
	if(nResult != 0) {
		OutputDebugLogTo(1, TRUE, "getaddrinfo failed with error: %d", nResult);
		return -1;
	}

	// Attempt to connect to an address until one succeeds
	while(1) {
		for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
			// Create a SOCKET for connecting to server
			m_Socket = socket(ptr->ai_family, ptr->ai_socktype, 
				ptr->ai_protocol);
			if(m_Socket == INVALID_SOCKET) {
				OutputDebugLogTo(1, TRUE, "socket failed with error: %ld", WSAGetLastError());
				return -1;
			}

			// Connect to server.
			nResult = connect(m_Socket, ptr->ai_addr,(int)ptr->ai_addrlen);
			if(nResult == SOCKET_ERROR) {
				closesocket(m_Socket);
				m_Socket = INVALID_SOCKET;
				continue;
			}
			break;
		}
		if(nResult != SOCKET_ERROR)
			break;
	}

	freeaddrinfo(result);

	if(m_Socket == INVALID_SOCKET) {
		OutputDebugLogTo(1, TRUE, "Unable to connect to server!");
		return -1;
	}

	return 0;
}

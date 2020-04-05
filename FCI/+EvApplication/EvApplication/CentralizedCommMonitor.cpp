/// TCP/IP monitor class implementation
/// Udupa; Jan'2011
/// Modified for 2D-Wire.. E Santosh Jul'12

#include "stdafx.h"
#include "Application.h"
#include "CentralizedCommMonitor.h"
#include "CentralizedCommManager.h"
#include "ImageQueueHandler.h"
#include "Resource.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>


CCentralizedCommMonitor::CCentralizedCommMonitor()
{
	m_hDisconnect = NULL;
	m_hTcpMonitorThread = 0;
	m_pApp = NULL;
	m_strPortNo = "";
	m_bConnected = FALSE;
	m_CurrentClientIPv4Addr = "";
	m_ClientMachineID = "";
	m_ClientMachineIPAddress = "";
}

BOOL CCentralizedCommMonitor::Create(CCentralizedCommInterface* pCommInterface, int nStation)
{
	m_bClosing = FALSE;
	m_bConnected = FALSE;

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

	CWinThread* pThread = AfxBeginThread(CentralizedCommMonitorThread, this);
	m_hTcpMonitorThread = pThread->m_hThread;
	if(m_hTcpMonitorThread == INVALID_HANDLE_VALUE) {
		SetEvent(m_hDisconnect);
		::AfxMessageBox("Failed to creaate TCP/IP thread");
		return FALSE;
	}

	return TRUE;
}

BOOL CCentralizedCommMonitor::Close()
{
	m_bClosing = TRUE;
	m_bConnected = FALSE;
	m_ClientAcceptedStatus = FALSE;

	if(m_hDisconnect != NULL) {
		SetEvent(m_hDisconnect);
		// E Santosh.. Debug Problem... 2012
		DeleteCriticalSection(&m_csMessageList);
	}

	return TRUE;
}

int CCentralizedCommMonitor::GetStationId()
{
	return m_nStation;
}

void CCentralizedCommMonitor::DisconnectSocket()
{
	m_bConnected = FALSE;
	closesocket(m_Socket);
	SetCurrentClientAcceptedStatus(FALSE);
}

BOOL CCentralizedCommMonitor::SendCommandMessage(char nCommand)
{
	CString strTemp = "Command Only!";
	//Create header for buffer to send: contains command ID, buffer length and buffer contents
	int nHeadSize = sizeof(CentralizedHeader);
	int nMsgContentsSize = strTemp.GetLength();
	int nBufSendSize = nHeadSize + nMsgContentsSize;

	uint8_t* buf = new uint8_t[nHeadSize];
	CentralizedHeader* pHeader = reinterpret_cast<CentralizedHeader*>(buf);
	pHeader->nCommand = nCommand;
	pHeader->nBufferLength = nBufSendSize;

	//Create buffer contents
	
	char* pMsgContents = new char[nMsgContentsSize];
	std::memcpy(pMsgContents, strTemp.GetBuffer(), nMsgContentsSize);

	//Copy header and contents into buffer to send
	char* pBufSend = new char[nBufSendSize];
	std::memcpy(pBufSend, reinterpret_cast<char*>(buf), nHeadSize);
	pBufSend += nHeadSize;
	std::memcpy(pBufSend, pMsgContents, nMsgContentsSize);
	pBufSend -= nHeadSize;

	BOOL bError = SendDataMessage(pBufSend, nBufSendSize);

	delete[] buf;
	delete[] pBufSend;

	return bError;
}

BOOL CCentralizedCommMonitor::SendDataMessage(char* pData, int nLength)
{
	if(m_Socket == INVALID_SOCKET) {
		OutputDebugLogTo(2, TRUE, "Centralized comm[%d]: No valid socket found", GetStationId());
		return FALSE;
	}

	if(pData != NULL && nLength > 0) {
		int nSendLeft = nLength;
		while (nSendLeft > 0) {
			int nSendResult = send(m_Socket, pData, nSendLeft, 0);
			if (nSendResult == SOCKET_ERROR) {
				OutputDebugLogTo(2, TRUE, "Centralized comm[%d]: send failed with error: %d", GetStationId(), WSAGetLastError());
				//closesocket(m_Socket);
				DisconnectSocket();
				return FALSE;
			}
			OutputDebugLogTo(2, TRUE, "Centralized comm[%d]: Bytes send: %d, Bytes sent: %d", GetStationId(), nSendLeft, nSendResult);

			nSendLeft -= nSendResult;
			pData += nSendResult;
		}

		/*int iSendResult = send(m_Socket, pData, nLength, 0);
		if(iSendResult == SOCKET_ERROR) {
			OutputDebugLogTo(2, TRUE, "Centralized comm: send failed with error: %d", WSAGetLastError());
			closesocket(m_Socket);
			return FALSE;
		}*/
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

void CCentralizedCommMonitor::SetCurrentClientAcceptedStatus(BOOL bConnected)
{
	m_ClientAcceptedStatus = bConnected;
}

void CCentralizedCommMonitor::SetCurrentClientAcceptedMachineId(CString strMachineId)
{
	m_ClientMachineID = strMachineId;
}

void CCentralizedCommMonitor::InsertClientAccepted(CString strIPv4Addr, BOOL bConnected)
{
	m_ClientAcceptedStatus = bConnected;
	m_ClientMachineIPAddress = strIPv4Addr;
}

void CCentralizedCommMonitor::GetClientAcceptedInfo(CString &strMachineID, CString &strIPv4, BOOL &bConnectedStatus)

{
	strMachineID = m_ClientMachineID;
	strIPv4 = m_ClientMachineIPAddress;
	bConnectedStatus = m_ClientAcceptedStatus;
}

CString CCentralizedCommMonitor::GetClientAcceptedMachineID()
{
	return m_ClientMachineID;
}

void CCentralizedCommMonitor::AddToMessage(char* pData, int nLength)
{

	if(pData == NULL || nLength < 1) {
		OutputDebugLogTo(2, TRUE, "Centralized comm: Truncated Message received from Controller [%d]", nLength);
		return;
	} 

	CCommMessage* pMessage;
	//char* pDest = new char[nLength];

	pMessage = &m_pCommInterface->m_pCommManager[m_nStation]->m_Message;
	//memcpy(pDest, pData, nLength);
	 
	EnterCriticalSection(&m_csMessageList);
	pMessage->pData = pData;
	pMessage->nDataLength = nLength;
	//pMessage->nCommand	  = pData[0];

	if (pMessage->nCommand == SEND_IMAGE_BUFFER || pMessage->nCommand == SEND_LIST_IMAGE_BUFFER) {
		OutputDebugLogTo(2, TRUE, "Centralized comm: SEND_IMAGE_BUFFER, received data: %d", nLength);
	}
	/*for (int i = 1; i < nLength - 1; i++) {
		if (pData[i] == ASCII_CR || pData[i] == ASCII_LF) {
			pMessage->nDataLength = i + 1;
			break;
		}
	}
	pMessage->pData = &pData[0];
	m_pApp->AddLogEvent(pMessage->pData, COMM_LOG);*/


	m_pCommInterface->m_pCommManager[m_nStation]->ServiceMessage();
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

UINT CentralizedCommMonitorThread(LPVOID lpData)
{
	CCentralizedCommMonitor *pTcpMonitor = (CCentralizedCommMonitor *)lpData;
	ASSERT(pTcpMonitor);
	CCentralizedCommInterface *pCommInterface = pTcpMonitor->m_pCommInterface;

	char* recvbuf = new char[MAXBUFFERIMAGES];
	char* recvbufTemp = new char[MAXBUFFERIMAGES];
	int nRecvResult = 0, nRecvLeft = 0;

	for(int p=0; p< MAXBUFFERIMAGES; p++){
		recvbuf[p] = '\0';
		recvbufTemp[p] = '\0';
	}
	while(1) {
		if (pCommInterface->m_bIsTCPServer) {
			pTcpMonitor->RegisterServer();
			pTcpMonitor->SendCommandMessage(INFO);
		}
		else
			pTcpMonitor->RegisterClient();

		do {
			int nHeadSize = sizeof(CentralizedHeader);
			nRecvResult = recv(pTcpMonitor->m_Socket, recvbuf, nHeadSize, 0);
			if(nRecvResult > 0){
				if (nRecvResult >= nHeadSize) {
					uint8_t* pHeaderBuf = new uint8_t[nHeadSize];
					memcpy(pHeaderBuf, recvbuf, nHeadSize);
					CentralizedHeader *pHeader = reinterpret_cast<CentralizedHeader*>(pHeaderBuf);
					theApp.m_CentralizedCommInterface.m_pCommManager[pTcpMonitor->GetStationId()]->m_Message.nCommand = pHeader->nCommand;

					OutputDebugLogTo(2, TRUE, "CentralizedHeader Contents: Receive command: %d, buffer length: %d, received length: %d", pHeader->nCommand, pHeader->nBufferLength, nRecvResult);
					//Get the remaining data
					if (pHeader->nBufferLength > 0 && pHeader->nBufferLength < MAXBUFFERIMAGES) {
						nRecvLeft = pHeader->nBufferLength - nRecvResult;
						char* pRecvBuf = recvbuf;
						int nActualRecv = nRecvResult;
						while (nRecvLeft > 0) {
							recvbuf += nRecvResult;
							nRecvResult = recv(pTcpMonitor->m_Socket, recvbuf, nRecvLeft, 0);
							if (nRecvResult == 0) {
								OutputDebugLogTo(2, TRUE, "Centralized comm[%d]: Connection closed by Controller", pTcpMonitor->GetStationId());
								break;
							}
							else if (nRecvResult < 0) {
								pTcpMonitor->DisconnectSocket();
								break;
							}

							OutputDebugLogTo(2, TRUE, "Centralized comm[%d] : getting the left data: %d, received data: %d", pTcpMonitor->GetStationId(), nRecvLeft, nRecvResult);

							nRecvLeft -= nRecvResult;
							nActualRecv += nRecvResult;
						}
						recvbuf = pRecvBuf;
						nRecvResult = nActualRecv;
					}
					else {
						OutputDebugLogTo(2, TRUE, "CentralizedHeader Contents: Buffer length is negative or too big!");
						continue;
					}

					//Remove the header contents and passing the contents message only!
					recvbuf += nHeadSize;
					nRecvResult -= nHeadSize;
				}
				else {
					OutputDebugLogTo(2, TRUE, "CentralizedHeader Contents: received data less than header size!");
					continue;
				}
				
				for(int p=0; p< MAXBUFFERIMAGES; p++)
					recvbufTemp[p] = '\0';

				for(int k=0; k< nRecvResult; k++)
					recvbufTemp[k] = recvbuf[k];

				pTcpMonitor->AddToMessage(recvbufTemp, nRecvResult);

				//Go back to the starting address
				recvbuf -= nHeadSize;
				for(int p=0; p< MAXBUFFERIMAGES; p++)
					recvbuf[p] = '\0';
			}
			else if (nRecvResult == 0) {
				OutputDebugLogTo(2, TRUE, "Centralized comm[%d]: Connection closed by Controller", pTcpMonitor->GetStationId());
				
				break;
			}
			else {
				OutputDebugLogTo(2, TRUE, "Centralized comm[%d]: recv() failed with error: %d closing the connection...", pTcpMonitor->GetStationId(), WSAGetLastError());
				//closesocket(pTcpMonitor->m_Socket);

				break;
			}

		} while(nRecvResult > 0 && theApp.m_CentralizedCommInterface.m_pCommManager[pTcpMonitor->GetStationId()]->m_Message.nCommand != DISCONNECT);//Client received disconnect request from client --> shutdown connection for a while
		

		if(pCommInterface->m_bIsTCPServer){
			if(pTcpMonitor->m_bClosing) {
				OutputDebugLogTo(2, TRUE, "TCP/IP Centralized comm[%d] : Connection closed.", pTcpMonitor->GetStationId());
				return 0;
			}

			nRecvResult = shutdown(pTcpMonitor->m_Socket, SD_SEND);
			if(nRecvResult == SOCKET_ERROR) {
				//closesocket(pTcpMonitor->m_Socket);
				Sleep(theApp.m_CentralizedCommInterface.m_nDelayTime);
			}

			theApp.UpdateCentralizedMachineState(pTcpMonitor->GetStationId(), "Disconnected", RGB(255, 0, 0));
		}
		//pTcpMonitor->m_bConnected = FALSE;
		pTcpMonitor->DisconnectSocket();
		//closesocket(pTcpMonitor->m_Socket);

		if (!pCommInterface->m_bIsTCPServer && (theApp.m_CentralizedCommInterface.m_pCommManager[pTcpMonitor->GetStationId()]->m_Message.nCommand == VERIFICATION_RESULT || theApp.m_CentralizedCommInterface.m_pCommManager[0]->m_Message.nCommand == DISCONNECT)) {
			Sleep(theApp.m_CentralizedCommInterface.m_nDelayTime);
		}
	}

	delete[] recvbuf;
	delete[] recvbufTemp;
	return 0;
}

int CCentralizedCommMonitor::RegisterServer()
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
		OutputDebugLogTo(2, TRUE, "Centralized comm[%d]: socket failed with error: %ld", GetStationId(), WSAGetLastError());
		freeaddrinfo(result);
		return -1;
	}

	int nRecvBufferSize = MAX_BUFFER_MSG_LENGTH;
	nResult = setsockopt(ServerSocket, SOL_SOCKET, SO_RCVBUF, (char *)&nRecvBufferSize, (int)sizeof(nRecvBufferSize));
	if (nResult == SOCKET_ERROR) {
		OutputDebugLogTo(2, TRUE, "Centralized comm[%d]: set Socket Received Buffer Length option failed with error: %d", GetStationId(), WSAGetLastError());
	}
	else {
		OutputDebugLogTo(2, TRUE, "Centralized comm[%d]: set Socket Received Buffer Length option successfully!!!", GetStationId());
	}

	// Setup the TCP listening socket
	nResult = bind(ServerSocket, result->ai_addr,(int)result->ai_addrlen);
	if(nResult == SOCKET_ERROR) {
		OutputDebugLogTo(2, TRUE, "Centralized comm[%d]: bind failed with error: %d", GetStationId(), WSAGetLastError());
		freeaddrinfo(result);
		DisconnectSocket();
		//closesocket(ServerSocket);
		return -1;
	}

	freeaddrinfo(result);

	nResult = listen(ServerSocket, SOMAXCONN);
	if(nResult == SOCKET_ERROR) {
		OutputDebugLogTo(2, TRUE, "Centralized comm[%d]: listen failed with error: %d", GetStationId(), WSAGetLastError());
		DisconnectSocket();
		//closesocket(ServerSocket);
		return -1;
	}

	//Client Ipv4 Address
	struct sockaddr_in clientAddr;
	socklen_t clientAddr_size = sizeof(clientAddr);

	OutputDebugLogTo(2, TRUE, "Centralized comm[%d]: waiting for clients...", GetStationId());
	// Accept a client socket
	m_Socket = accept(ServerSocket, (struct sockaddr *)&clientAddr, &clientAddr_size);
	if(m_Socket == INVALID_SOCKET) {
		OutputDebugLogTo(2, TRUE, "Centralized comm[%d]: accept failed with error: %d", GetStationId(), WSAGetLastError());  /// Disconnect Sockets successfully error ..
		return -1;
	}

	//Get Client IPV4 Address
	struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&clientAddr;
	struct in_addr ipAddr = pV4Addr->sin_addr;

	char strClientAddr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ipAddr, strClientAddr, INET_ADDRSTRLEN);

	OutputDebugLogTo(2, TRUE, "TCP/IP Centralized comm[%d] [%s]: Centralized PC accepted client (IPv4: %s)", GetStationId(), m_strPortNo, strClientAddr);

	//Insert/Update client accepted map
	memcpy(m_CurrentClientIPv4Addr.GetBuffer(), strClientAddr, clientAddr_size);
	InsertClientAccepted(m_CurrentClientIPv4Addr, TRUE);

	// No longer need server socket
	closesocket(ServerSocket);

	m_bConnected = TRUE;
	return 0;
}

int CCentralizedCommMonitor::RegisterClient()
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
		OutputDebugLogTo(2, TRUE, "Centralized comm: getaddrinfo failed with error: %d", nResult);
		return -1;
	}

	// Attempt to connect to an address until one succeeds
	while(1) {
		for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
			// Create a SOCKET for connecting to server
			m_Socket = socket(ptr->ai_family, ptr->ai_socktype, 
				ptr->ai_protocol);
			if(m_Socket == INVALID_SOCKET) {
				OutputDebugLogTo(2, TRUE, "Centralized comm: socket failed with error: %ld", WSAGetLastError());
				return -1;
			}

			// Connect to server.
			//OutputDebugLogTo(2, TRUE, "Centralized comm: Attempt to connect Centralized PC");
			nResult = connect(m_Socket, ptr->ai_addr,(int)ptr->ai_addrlen);
			if(nResult == SOCKET_ERROR) {
				//OutputDebugLogTo(2, TRUE, "Centralized comm: Attempt to connect Centralized PC: Failed!!!");
				DisconnectSocket();
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
		OutputDebugLogTo(2, TRUE, "Centralized comm: Unable to connect to server!");
		return -1;
	}

	OutputDebugLogTo(2, TRUE, "TCP/IP Centralized comm: Client Registered Centralized PC successfully");
	m_bConnected = TRUE;

	if (!m_pApp->m_pImageQueue->ImageQueueCentralizedEmpty()) {
		OutputDebugLogTo(2, TRUE, "TCP/IP Centralized comm: TriggerSendCentralizedImage");
		m_pApp->m_pImageQueue->TriggerSendCentralizedImage();
	}

	if (!m_pApp->m_pImageQueue->ResultQueueCentralizedEmpty()) {
		OutputDebugLogTo(2, TRUE, "TCP/IP Centralized comm: TriggerSendCentralizedResult");
		m_pApp->m_pImageQueue->TriggerSendCentralizedResult();
	}

	if (!m_pApp->m_pImageQueue->DefectInfoQueueCentralizedEmpty()) {
		OutputDebugLogTo(2, TRUE, "TCP/IP Centralized comm: TriggerSendCentralizedDefectInfo");
		m_pApp->m_pImageQueue->TriggerSendCentralizedDefectInfo();
	}

	return 0;
}

/// TCP/IP monitor class implementation
/// Udupa; Aug'2012

#include "stdafx.h"
#include "TcpMonitor.h"
#include "DebugLog.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>


CTcpMonitor::CTcpMonitor()
{
	m_hDisconnect = NULL;
	m_nSendLength = 0;
	m_nReceiveLength = 0;

	m_bIsTCPServer = TRUE;
	strcpy_s(m_strIpAddress, "");
	strcpy_s(m_strPortNo, "");
}

BOOL CTcpMonitor::Create(BOOL bIsTCPServer, char* strIpAddress, char* strPortNo)
{
	m_bClosing = FALSE;

	InitializeCriticalSection(&m_csTcpSender);
	m_hDisconnect = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(m_hDisconnect == INVALID_HANDLE_VALUE) {
		m_hDisconnect = NULL;
		::AfxMessageBox("Unable to create event");
		return FALSE;
	}

	m_bIsTCPServer = bIsTCPServer;
	strcpy_s(m_strIpAddress, strIpAddress);
	strcpy_s(m_strPortNo, strPortNo);
	m_Socket = INVALID_SOCKET;

	return TRUE;
}

BOOL CTcpMonitor::Close()
{
	m_bClosing = TRUE;
	if(m_hDisconnect != NULL) {
		SetEvent(m_hDisconnect);
		DeleteCriticalSection(&m_csTcpSender);
	}

	return TRUE;
}

BOOL CTcpMonitor::SendTcpMessage(int nCommand, void* pData1, int nLength1, void* pData2, int nLength2)
{
	int nTcpHeaderSize = sizeof(INT32)<<1;
	int nTotalLength = nLength1 + nLength2;
	if(nTotalLength > (COMM_BUFLEN-nTcpHeaderSize)) {
		OutputDebugLogTo(1, TRUE, "****** SendMessage: Data length (%d) exceeds limit", nTotalLength);
		return FALSE;
	}
	if(m_Socket == INVALID_SOCKET) {
		OutputDebugLogTo(0, TRUE, "No valid socket found");
		return FALSE;
	}

	char* pSendBuf = (char*) m_pSendBuf;

	EnterCriticalSection(&m_csTcpSender);
	{
		CTcpMessage message(nCommand, pData1, nTotalLength);
		memcpy(pSendBuf, &message, nTcpHeaderSize);
		TcpMessageSender(pSendBuf, nTcpHeaderSize);
	}
	if(nTotalLength > 0) {
		pSendBuf += nTcpHeaderSize;
		memcpy(pSendBuf, pData1, nLength1);
		if(nLength2 > 0)
			memcpy(pSendBuf+nLength1, pData2, nLength2);
		TcpMessageSender(pSendBuf, nTotalLength);
	}
	LeaveCriticalSection(&m_csTcpSender);
	

	return TRUE;
}

BOOL CTcpMonitor::TcpMessageSender(char* pData, int nLength)
{
	int nSendResult = send(m_Socket, pData, nLength, 0);
	if(nSendResult == SOCKET_ERROR) {
		OutputDebugLogTo(0, TRUE, "****** SendMessage: send failed with error (%d)", WSAGetLastError());
		closesocket(m_Socket);
		return FALSE;
	}
	OutputDebugLogTo(1, TRUE, "[Port%s] Message sent (%d)", m_strPortNo, nSendResult);

	return TRUE;
}

int CTcpMonitor::ReceiveMessage()
{
	int nRequiredLength = sizeof(INT32)<<1;
	int nReceivedLength;
	m_nReceiveLength = 0;
	BYTE* pReceiveBuf = m_pReceiveBuf;
	CTcpMessage* pMessage = NULL;

	while(1) {
		nReceivedLength = recv(m_Socket, (char*) pReceiveBuf, nRequiredLength, MSG_WAITALL);
		m_nReceiveLength += nReceivedLength;
		OutputDebugLogTo(1, TRUE, "Received length %d (Total:%d)", nReceivedLength, m_nReceiveLength);
		if(nReceivedLength > 0) {
			if(!pMessage) {
				pMessage = (CTcpMessage*) pReceiveBuf;
				if(pMessage->nDataLength > 0)
					nRequiredLength = pMessage->nDataLength;
				else
					break;
			}
			else
				nRequiredLength -= nReceivedLength;

			//OutputDebugLogTo(1, TRUE, "Expected length %d", nRequiredLength);
			if(nRequiredLength > 0) {
				pReceiveBuf += nReceivedLength;
				continue;
			}
			///OutputDebugLogTo(1, TRUE, "Bytes received: %d", m_nReceiveLength);
		}
		else if(!m_bClosing) {
			if(nReceivedLength == 0)
				OutputDebugLogTo(0, TRUE, "[Port%s] Connection closed", m_strPortNo);
			else
				OutputDebugLogTo(0, TRUE, "[Port%s] ERROR in receiving data (Length: %d) [Error: %d]", m_strPortNo, nReceivedLength, WSAGetLastError());
		}
		else
			OutputDebugLogTo(0, TRUE, "[Port%s] Socket closed", m_strPortNo, WSAGetLastError());

		break;
	}
	//m_nReceiveLength = nTotalReceived;

	return 0;
}

int CTcpMonitor::RegisterComm()
{
	int nResult;
	if(m_Socket != INVALID_SOCKET) {
		nResult = shutdown(m_Socket, SD_BOTH);
		//if(nResult == SOCKET_ERROR)
		//	OutputDebugLogTo(1, TRUE, "Connection Shutdown failed with error: %d", WSAGetLastError());
		closesocket(m_Socket);
	}

	if(m_bIsTCPServer) {
		nResult = RegisterServer();
	}
	else
		nResult = RegisterClient();

	if(nResult==0) {
		int nFlag = 1;
		int result = setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (char*) &nFlag, sizeof(int));
		if(result < 0)
			OutputDebugLogTo(0, TRUE, "[Port%s] Failed to set NoDelay", m_strPortNo);
	}

	return nResult;
}

int CTcpMonitor::RegisterServer()
{
	SOCKET ServerSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, hints;


	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	int nResult = getaddrinfo(NULL, m_strPortNo, &hints, &result);
	if(nResult != 0) {
		OutputDebugLogTo(0, TRUE, "getaddrinfo failed with error: %d", nResult);
		return -1;
	}

	ServerSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if(ServerSocket == INVALID_SOCKET) {
		OutputDebugLogTo(0, TRUE, "socket failed with error: %ld", WSAGetLastError());
		freeaddrinfo(result);
		return -1;
	}

	nResult = bind(ServerSocket, result->ai_addr,(int)result->ai_addrlen);
	if(nResult == SOCKET_ERROR) {
		OutputDebugLogTo(0, TRUE, "bind failed with error: %d", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ServerSocket);
		return -1;
	}

	freeaddrinfo(result);

	nResult = listen(ServerSocket, SOMAXCONN);
	if(nResult == SOCKET_ERROR) {
		OutputDebugLogTo(0, TRUE, "listen failed with error: %d", WSAGetLastError());
		closesocket(ServerSocket);
		return -1;
	}

	m_Socket = accept(ServerSocket, NULL, NULL);
	if(m_Socket == INVALID_SOCKET) {
		OutputDebugLogTo(0, TRUE, "accept failed with error: %d", WSAGetLastError());
		closesocket(ServerSocket);
		return -1;
	}

	OutputDebugLogTo(0, TRUE, "TCP/IP comm [%s]: Registered Server successfully", m_strPortNo);

	closesocket(ServerSocket);

	return 0;
}

int CTcpMonitor::RegisterClient()
{
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int nResult = getaddrinfo(m_strIpAddress, m_strPortNo, &hints, &result);
	if(nResult != 0) {
		OutputDebugLogTo(0, TRUE, "getaddrinfo failed with error: %d", nResult);
		return -1;
	}

	SOCKET socket1;
	while(!m_bClosing) {
		for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
			socket1 = socket(ptr->ai_family, ptr->ai_socktype, 
				ptr->ai_protocol);
			if(socket1 == INVALID_SOCKET) {
				OutputDebugLogTo(0, TRUE, "socket failed with error: %ld", WSAGetLastError());
				return -1;
			}

			nResult = connect(socket1, ptr->ai_addr,(int)ptr->ai_addrlen);
			if(nResult == SOCKET_ERROR) {
				closesocket(socket1);
				socket1 = INVALID_SOCKET;
				continue;
			}
			m_Socket = socket1;
			break;
		}
		if(nResult != SOCKET_ERROR)
			break;
		Sleep(300);
	}

	freeaddrinfo(result);
	if(m_Socket != INVALID_SOCKET) {
		OutputDebugLogTo(0, TRUE, "TCP/IP comm [%s, %s]: Registered Client successfully", m_strIpAddress, m_strPortNo);
		return 0;
	}

	return -1;
}

int CTcpMonitor::InitializeTcpIp()
{
	WSADATA wsaData;
	int nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(nResult != 0)
		AfxMessageBox("TCP/IP Initialization (WSAStartup) failed\n\nError: %d", nResult);

	return 0;
}

int CTcpMonitor::CloseTcpIp()
{
	WSACleanup();

	return 0;
}

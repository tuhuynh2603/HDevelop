/// Comm Interface
/// Udupa; Jan'2011

#include "stdafx.h"
#include "CentralizedCommInterface.h"
#include "Application.h"
#include <winsock2.h>
#include "CentralizedCommManager.h"

CCentralizedCommInterface::CCentralizedCommInterface()
{
	bInitialized = FALSE;
}

CCentralizedCommInterface::~CCentralizedCommInterface()
{
}

void CCentralizedCommInterface::Initialize()
{
	CString strComm;
	m_nControllerTimeout = 5000;
	m_nDelayTime = 1000;

	strComm.Format("Centralized Comm");
	
	m_nControllerTimeout = GetRegistryEntry(READ_REGPATH + strComm, "Comm::Controller Timeout", m_nControllerTimeout);
	PutRegistryEntry(WRITE_REGPATH + strComm, "Comm::Controller Timeout", m_nControllerTimeout);

	m_nDelayTime = GetRegistryEntry(READ_REGPATH + strComm, "Comm::Delay Time", m_nDelayTime);
	PutRegistryEntry(WRITE_REGPATH + strComm, "Comm::Delay Time", m_nDelayTime);

	m_bIsTCPServer = 1;
	m_bIsTCPServer = GetRegistryEntry(READ_REGPATH + strComm, "Comm::TCPServer", (int) m_bIsTCPServer);
	PutRegistryEntry(WRITE_REGPATH + strComm, "Comm::TCPServer", (int) m_bIsTCPServer);

	CString strVal = "192.168.0.200";
	strVal = GetRegistryEntry(READ_REGPATH + strComm, "Comm::IpAddress", strVal);
	PutRegistryEntry(WRITE_REGPATH + strComm, "Comm::IpAddress", strVal);
	memset(m_strIpAddress, 0, 16);
	strcpy_s(m_strIpAddress, strVal.GetBuffer(0));
	strVal.ReleaseBuffer();

	strVal = "255.255.0.0";
	strVal = GetRegistryEntry(READ_REGPATH + strComm, "Comm::SubnetMask", strVal);
	PutRegistryEntry(WRITE_REGPATH + strComm, "Comm::SubnetMask", strVal);
	memset(m_strSubnetMask, 0, 16);
	strcpy_s(m_strSubnetMask, strVal.GetBuffer(0));
	strVal.ReleaseBuffer();

	CString str;

	for(int i=0; i< NO_OF_PORT_CENTRALIZED; i++) {
		strVal.Format("%d", 3000+i);
		str.Format("Comm::PortNumber[%d]", i);
		strVal = GetRegistryEntry(READ_REGPATH + strComm, str, strVal);
		PutRegistryEntry(WRITE_REGPATH + strComm, str, strVal);
		strcpy_s(m_strPortNumbers[i], strVal.GetBuffer(0));
		m_strPortNumbers[i][4] = 0;
		strVal.ReleaseBuffer();
	}

	/*int nPortId = theApp.m_nAppInstanceID >= 0 ? theApp.m_nAppInstanceID : 0;
	strVal.Format("%d", 3000 + nPortId);
	str.Format("Comm::PortNumber[%d]", nPortId);
	strVal = GetRegistryEntry(READ_REGPATH + strComm, str, strVal);
	PutRegistryEntry(WRITE_REGPATH + strComm, str, strVal);
	strcpy_s(m_strPortNumbers[0], strVal.GetBuffer(0));
	m_strPortNumbers[0][4] = 0;
	strVal.ReleaseBuffer();*/


	WSADATA wsaData;
	int nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(nResult != 0) {
		AfxMessageBox("TCP/IP Initialization (WSAStartup) failed\n\nError: %d", nResult);
	}

	if (m_bIsTCPServer) {
		for (int i = 0; i < NO_OF_PORT_CENTRALIZED; i++) {
			m_pCommManager[i] = new CCentralizedCommManager(i);
			bInitialized = m_pCommManager[i]->isValid;
		}
	}
	else {
		m_pCommManager[0] = new CCentralizedCommManager(0);
		bInitialized = m_pCommManager[0]->isValid;
	}
}

void CCentralizedCommInterface::Close()
{
	for(int i=0; i< NO_OF_PORT_CENTRALIZED; i++)
		if(m_pCommManager[i] != NULL)
			m_pCommManager[i]->Close();

	WSACleanup();

	for(int i=0; i< NO_OF_PORT_CENTRALIZED; i++)
		if(m_pCommManager[i] != NULL)
			delete m_pCommManager[i];
}

void CCentralizedCommInterface::ResetConnection()
{
	WSACleanup();
	WSADATA wsaData;
	int nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(nResult != 0) {
		AfxMessageBox("TCP/IP Centralized comm Initialization (WSAStartup) failed\n\nError: %d", nResult);
	}

	OutputDebugLogTo(2, TRUE, "Centralized comm Connection RESET by VISION");
	for(int i=0; i< NO_OF_PORT_CENTRALIZED; i++){
		OutputDebugLogTo(2, TRUE, "TCP/IP Centralized comm [%s]: Connection closed.", m_pCommManager[i]->m_Monitor.m_strPortNo);
	}
}


void CCentralizedCommInterface::LoadRecipeFromCOMM()
{
	m_pCommManager[0]->LoadRecipe();
}
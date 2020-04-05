/// Comm Interface
/// Udupa; Jan'2011
/// Modified for 2D-Wire.. E Santosh Jul'12

#include "stdafx.h"
#include "CommInterface.h"
#include "Application.h"
#include <winsock2.h>
#include "CommManager.h"

CCommInterface::CCommInterface()
{
	bInitialized = FALSE;
}

CCommInterface::~CCommInterface()
{
}

void CCommInterface::Initialize()
{
	CString strComm;
	m_nControllerTimeout = 5000;

	strComm.Format("Comm");
	
	m_nControllerTimeout = GetRegistryEntry(READ_REGPATH + strComm, "Comm::Controller Timeout", m_nControllerTimeout);
	PutRegistryEntry(WRITE_REGPATH + strComm, "Comm::Controller Timeout", m_nControllerTimeout);

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
	for(int i=0; i<NO_OF_PORT; i++) {
		strVal.Format("%d", 2000+i);
		str.Format("Comm::PortNumber[%d]", i);
		strVal = GetRegistryEntry(READ_REGPATH + strComm, str, strVal);
		PutRegistryEntry(WRITE_REGPATH + strComm, str, strVal);
		strcpy_s(m_strPortNumbers[i], strVal.GetBuffer(0));
		m_strPortNumbers[i][4] = 0;
		strVal.ReleaseBuffer();
	}

	WSADATA wsaData;
	int nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(nResult != 0) {
		AfxMessageBox("TCP/IP Initialization (WSAStartup) failed\n\nError: %d", nResult);
	}

	m_pCommManager[0]	= new CCommManager(0);	
	bInitialized		= m_pCommManager[0]->isValid;
}

void CCommInterface::Close()
{
	for(int i=0; i<NO_OF_PORT; i++)
		if(m_pCommManager[i] != NULL)
			m_pCommManager[i]->Close();

	WSACleanup();

	for(int i=0; i<NO_OF_PORT; i++)
		if(m_pCommManager[i] != NULL)
			delete m_pCommManager[i];
}

void CCommInterface::ResetConnection()
{
	WSACleanup();
	WSADATA wsaData;
	int nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(nResult != 0) {
		AfxMessageBox("TCP/IP Initialization (WSAStartup) failed\n\nError: %d", nResult);
	}

	OutputDebugLogTo(1, TRUE, "Connection RESET by VISION");
	for(int i=0; i<NO_OF_PORT; i++){
		OutputDebugLogTo(1, TRUE, "TCP/IP comm [%s]: Connection closed.", m_pCommManager[i]->m_Monitor.m_strPortNo);
	}
}


void CCommInterface::LoadRecipeFromCOMM()
{
	m_pCommManager[0]->LoadRecipe();
}
/*   -----------------------------------------------------------------	//
//																		//
//	Copyright (c) 2009 Gardasoft Products Ltd							//
//	All rights reserved                   								//
//																		//
//	Gardasoft Vision Limited											//
//	Units 1 and 2, Castle Acres											//
//	Elsworth															//
//	Cambridge															//
//	CB23 4JQ															//
//	UK 																	//
//																		//
//	tel. +44 1954 200 343												//
//	fax. +44 1954 204 343												//
//																		//
//	Email	vision@gardasoft.com										//
//	WWW	http:\\www.gardasoft.com										//
//																		//
//	-----------------------------------------------------------------	//
//																		//
//	This file is supplied solely for the purpose of						//
//	supporting Gardasoft's products. 									//
//	It may not be used for any other purpose.							//
//																		//
//	-----------------------------------------------------------------	//
//																		//
//	AAsyncSocket.cpp : implementation file								//
//																		//
//	This class is based on the CAsyncSocket class						//
//	-----------------------------------------------------------------	//
*/

#include "stdafx.h"
#include "AAsyncSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*********************************************************************
  CAAsyncSocket::CAAsyncSocket()

  Description
  Simple constructor for CAAsyncSocket class. Initilises class data

  Parameters
  None
*********************************************************************/

CAAsyncSocket::CAAsyncSocket()
{
	ieSocketProtocol = AAS_PROTOCOL_NOTINIT;
	ieSocketState = AAS_STATE_NOTINIT;
	bConnected = false;
	sIPAddress = "";
	iPort = 0;
	pLogger = NULL;
}

/*********************************************************************
  CAAsyncSocket::~CAAsyncSocket()

  Description
  Simple destructor for CAAsyncSocket class. Destroys any residual
  communications objects.

  Parameters
  None
*********************************************************************/

CAAsyncSocket::~CAAsyncSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CAAsyncSocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CAAsyncSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CAAsyncSocket member functions


/*********************************************************************
  Routine
  CAAsyncSocket::OnClose

  Description
  Overloads the CAsyncSocket class so that we can capture when a socket
  really closes. Fires when the close event is detected.

  Parameters
  1 - int nErrorCode	- error return.

  Returns - void
*********************************************************************/

void CAAsyncSocket::OnClose(int nErrorCode )
{
	CAsyncSocket::OnClose(nErrorCode);

	try
	{
		char strMessage[200];

		if (nErrorCode != 0)
		{
			sprintf(strMessage, "Socket OnClose Error - %i", nErrorCode);
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		}

		ieSocketState = AAS_STATE_CLOSED;
		bConnected = false;
		LogMessage(CLogger::LGR_SEVERITY_INFO,"Socket Closed");
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - CAAsyncSocket::OnClose");
	}
}

/*********************************************************************
  Routine
  CAAsyncSocket::OnConnect

  Description
  Overloads the CAsyncSocket class so that we can capture when a TCP 
  socket connects. Fires when the connect event is detected.

  Parameters
  1 - int nErrorCode	- error return.

  Returns - void
*********************************************************************/

void CAAsyncSocket::OnConnect(int nErrorCode )
{
	CAsyncSocket::OnConnect(nErrorCode);

	try
	{
		char strMessage[200];

		if (ieSocketProtocol != AAS_PROTOCOL_TCP)
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CAAsyncSocket::OnConnect Fired for Non-TCP Protocol");
		else
		{
			if (nErrorCode != 0)
			{
				sprintf(strMessage, "TCP Socket Failed to Connect - %i", nErrorCode);
				LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
			}
			else
			{
				// successful TCP connection
				bConnected = true;
				ieSocketState = AAS_STATE_ACTIVE;
				LogMessage(CLogger::LGR_SEVERITY_INFO,"TCP Socket Connected Successfully");
			}
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - CAAsyncSocket::OnConnect");
	}
}

/*********************************************************************
  Routine
  CAAsyncSocket::OnReceive

  Description
  Overloads the CAsyncSocket class so that we can capture when a packet
  is received. Fires when the receive event is detected. This routine
  invokes the notify routine to pass the buffer back to any parties
  who have registered for receipt.

  Parameters
  1 - int nErrorCode	- error return.

  Returns - void
*********************************************************************/

void CAAsyncSocket::OnReceive(int nErrorCode) 
{
	CString sIPAddress;
	char strMessage[200];
	int iReturn;
	UINT iPort;

	TCHAR buff[4096];
	int nRead;

	try
	{
		CAsyncSocket::OnReceive(nErrorCode);

		if (nErrorCode != 0)
		{
			sprintf(strMessage, "Socket OnReceive Error - %i", nErrorCode);
			throw strMessage;
		}

		if (ieSocketState != AAS_STATE_ACTIVE)
		{
			sprintf(strMessage, "OnReceive - Data Returned in State - %i", ieSocketState);
			throw strMessage;
		}

		if (!bConnected && (ieSocketProtocol == AAS_PROTOCOL_TCP))
		{
			sprintf(strMessage, "OnReceive - Data Returned While Not Connected");
			throw strMessage;
		}

		if ((ieSocketProtocol != AAS_PROTOCOL_UDP) &&
			(ieSocketProtocol != AAS_PROTOCOL_TCP))
		{
			sprintf(strMessage, "CAAsyncSocket::OnReceive - no Protocol Selected");
			throw strMessage;
		}

		nRead = ReceiveFrom(buff, 4096, sIPAddress, iPort); 
		switch (nRead)
		{
		case 0:
			sprintf(strMessage, "CAAsyncSocket::OnReceive - Called For No Message");
			throw strMessage;

		case SOCKET_ERROR:
			iReturn = GetLastError();
			if (iReturn != WSAEWOULDBLOCK) 
			{
				sprintf(strMessage, "Socket Error Occurred - %i", iReturn);
				throw strMessage;
			}
			break;
		default:
			buff[nRead] = 0; //terminate the string
			CString szTemp(buff);

			// if TCP then need to pick address and Port from connection
			if (ieSocketProtocol == AAS_PROTOCOL_TCP)
				NotifyReceive(szTemp, this->sIPAddress, this->iPort);
			else
				NotifyReceive(szTemp, sIPAddress, iPort);
		}
	}
	catch (char* strMessage)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - CAAsyncSocket::OnReceive");
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - CAAsyncSocket::OnReceive");
	}
}

/*********************************************************************
  Routine
  CAAsyncSocket::NotifyReceive

  Description
  Called when there is a data buffer to pass back to any interested
  parties. Overloads the CAsyncSocket class so that we can capture when a packet
  is received. Fires when the receive event is detected. This routine
  invokes the OnSocketReceive routine to pass the buffer back to any parties
  who have registered for receipt.

  Parameters
  1 - CString& strBuff		- buffer containing returned data.
  2 - CString& sIPAddress	- buffer containing ascii IP address of sender
  3 - UINT& iPort			- port number of sender

  Returns - void
*********************************************************************/

void CAAsyncSocket::NotifyReceive(CString& strBuff, CString& sIPAddress, UINT& iPort)
{
	try
	{
		list<ISocketEvents*>::iterator it;
		for (it=m_clients.begin(); it!=m_clients.end(); it++) 
		{
			(*it)->OnSocketReceive(strBuff, sIPAddress, iPort);
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - CAAsyncSocket::NotifyReceive");
		throw;
	}
}

/*********************************************************************
  Routine
  CAAsyncSocket::setLoggerObject

  Description
  Set pointer to logger object. This routine sets a pointer to
  a CLogger object. The logger object should be instantiated by
  the code which instantiates this class. It should be derived from
  (or simply) the supplied CLogger class.

  Parameters
  1 - CLogger* cLog	- pointer to Clogger object.

  Returns - void
*********************************************************************/
void CAAsyncSocket::setLoggerObject(CLogger* cLog)
{
	pLogger = cLog;	// pointer to logger object
}

/*********************************************************************
  Routine
  CAAsyncSocket::LogMessage

  Description
  Log Message and severity. Note the implementation of AddLogMessage
  may be unknown.
  
  Parameters
  1 - CLogger::LGR_SEVERITY iSeverity	- (see enum LGR_SEVERITY)
  2 - char * strLogMsg					- ascii message to log

  Returns - void
*********************************************************************/

void CAAsyncSocket::LogMessage(CLogger::LGR_SEVERITY iSeverity, char * strLogMsg)
{
	try
	{
		if (pLogger != NULL)
			pLogger->AddLogMessage(iSeverity, strLogMsg);
	}
	catch(...)
	{}
}

/*********************************************************************
  Routine
  CAAsyncSocket::LogMessage

  Description
  Log Message and severity. Note the implementation of AddLogMessage
  may be unknown.
  
  Parameters
  1 - CLogger::LGR_SEVERITY iSeverity	- (see enum LGR_SEVERITY)
  2 - CString & strLogMessage			- ascii message to log

  Returns - void
*********************************************************************/

void CAAsyncSocket::LogMessage(CLogger::LGR_SEVERITY iSeverity, CString & strLogMessage)
{
	try
	{
		if (pLogger != NULL)
			pLogger->AddLogMessage(iSeverity, strLogMessage);
	}
	catch(...)
	{}
}

/*********************************************************************
  Routine
  CAAsyncSocket::getProtocol

  Description
  Get protocol. This routine returns the currently set protocol.
  
  Parameters
  None

  Returns - CAAsyncSocket::AAS_PROTOCOL
							AAS_PROTOCOL_TCP if TCP
							AAS_PROTOCOL_UDP if UDP
							AAS_PROTOCOL_NOTINIT if neither
*********************************************************************/

enum CAAsyncSocket::AAS_PROTOCOL CAAsyncSocket::getProtocol()
{
	return ieSocketProtocol;
}

/*********************************************************************
  Routine
  CAAsyncSocket::setProtocol

  Description
  Set protocol. This routine sets the required protocol.
  
  Parameters
  1 - CAAsyncSocket::AAS_PROTOCOL	- required protocol (Must be 
											AAS_PROTOCOL_TCP or
											AAS_PROTOCOL_UDP)

  Returns - void
*********************************************************************/

void CAAsyncSocket::setProtocol(enum AAS_PROTOCOL iProto)
{
	char strMessage[200];

	try
	{
		ieSocketProtocol = iProto;
	
		switch (iProto)
		{
		case AAS_PROTOCOL_NOTINIT:
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Protocol of Socket Set to NOT INITIALISED");
			break;
		case AAS_PROTOCOL_UDP:
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Protocol of Socket Set to UDP");
			break;
		case AAS_PROTOCOL_TCP:
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Protocol of Socket Set to TCP");
			break;
		default:
			sprintf(strMessage, "Error Set Protocol attempted with value - %i", iProto); 
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - CAAsyncSocket::setProtocol");
		throw;
	}
}

/*********************************************************************
  Routine
  CAAsyncSocket::getState

  Description
  Get state. This routine returns the current state of the socket.
  
  Parameters
  None

  Returns - CAAsyncSocket::AAS_STATE
			AAS_STATE_NOTINIT			initial state before activation
			AAS_STATE_ACTIVE			active state
			AAS_STATE_CLOSE_REQUESTED	close requested on this socket awaiting
			AAS_STATE_CLOSED			close confirmed awaiting destruction
*********************************************************************/

enum CAAsyncSocket::AAS_STATE CAAsyncSocket::getState()
{
	return ieSocketState;
}

/*********************************************************************
  Routine
  CAAsyncSocket::setState

  Description
  Set state. This routine sets the current state of the socket.
  
  Parameters
  1 - CAAsyncSocket::AAS_STATE	- required protocol (Must be 
											AAS_STATE_NOTINIT
											AAS_STATE_ACTIVE
											AAS_STATE_CLOSE_REQUESTED
											AAS_STATE_CLOSED)

  Returns - void
*********************************************************************/

void CAAsyncSocket::setState(enum AAS_STATE iState)
{
	char strMessage[200];

	try
	{
		ieSocketState = iState;
	
		switch (iState)
		{
		case AAS_STATE_NOTINIT:
			LogMessage(CLogger::LGR_SEVERITY_INFO,"State of Socket Set to NOT INITIALISED");
			break;
		case AAS_STATE_ACTIVE:
			LogMessage(CLogger::LGR_SEVERITY_INFO,"State of Socket Set to ACTIVE");
			break;
		case AAS_STATE_CLOSE_REQUESTED:
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Protocol of Socket Set to CLOSE_REQUESTED");
			break;
		case AAS_STATE_CLOSED:
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Protocol of Socket Set to CLOSED");
			break;
		default:
			sprintf(strMessage, "Error Set State attempted with value - %i", iState); 
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - CAAsyncSocket::setState");
		throw;
	}
}

/*********************************************************************
  Routine
  CAAsyncSocket::getConnected

  Description
  Get TCP connected state. This routine returns the whether or not the 
  TCP socket is connected.
  
  Parameters
  None

  Returns - bool -	true if connected otherwise false.
*********************************************************************/
// get TCP Connected setting

bool CAAsyncSocket::getConnected()
{
	return bConnected;
}

/*********************************************************************
  Routine
  CAAsyncSocket::setIPAddress

  Description
  Set IP address. This routine sets the IP address for the socket.
  
  Parameters
  1 - CString	- ascii formatted IP address

  Returns - void
*********************************************************************/

void CAAsyncSocket::setIPAddress(CString sIPAddr)
{
	sIPAddress = sIPAddr;
}

/*********************************************************************
  Routine
  CAAsyncSocket::getIPAddress

  Description
  Gets IP address. This routine gets the IP address for the socket.
  
  Parameters
  None

  Returns - CString	- ascii formatted IP address
*********************************************************************/

CString CAAsyncSocket::getIPAddress()
{
	return sIPAddress;
}

/*********************************************************************
  Routine
  CAAsyncSocket::setPort

  Description
  Set Port. This routine sets the Port number for the socket.
  
  Parameters
  1 - UINT	- port number

  Returns - void
*********************************************************************/

void CAAsyncSocket::setPort(UINT iP)
{
	iPort = iP;
}

/*********************************************************************
  Routine
  CAAsyncSocket::getPort

  Description
  Gets Port Number. This routine gets the Port Number for the socket.
  
  Parameters
  None

  Returns - UINT	- Port Number
*********************************************************************/

UINT CAAsyncSocket::getPort()
{
	return iPort;
}

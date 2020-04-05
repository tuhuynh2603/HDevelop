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
//	ControllerNW.cpp: implementation of the CControllerNW class			//
//																		//
//	-----------------------------------------------------------------	//
*/

#include "stdafx.h"
#include "ControllerNW.h"
#include "assert.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*********************************************************************
  CControllerNW::CControllerNW()

  Description
  Simple constructor for CControllerNW class. Initilises Windows Sockets
  and other class data

  Parameters
  None
*********************************************************************/

CControllerNW::CControllerNW()
{
	UINT	i;
	
	if (!AfxSocketInit())
	{
		AfxMessageBox("Failed to Initialise Sockets");	// attempt to let someone know
		throw "ERROR - CControllerNW::CControllerNW - Failed to Initialise Sockets";  
	}

	this->Controllers.SetSize(0);
	this->ComPortSerialRefresh.SetSize(0);
	pLogger = NULL;
	
	for (i = 0U; i < CNW_NUMBER_OF_ETHERNET_SOCKETS; i++)
	{
		apControlSockets [i] = NULL;
	} // *for*

	// Set up timeouts and Non-blocking for comms ports	
	// Specify time-out between character for receiving.
	comTimeOut.ReadIntervalTimeout = MAXDWORD;				// return immediate
	// Specify value that is multiplied 
	// by the requested number of bytes to be read. 
	comTimeOut.ReadTotalTimeoutMultiplier = 0;				// return immediate
	// Specify value is added to the product of the 
	// ReadTotalTimeoutMultiplier member
	comTimeOut.ReadTotalTimeoutConstant = 0;				// return immediate
	// Specify value that is multiplied 
	// by the requested number of bytes to be sent. 
	comTimeOut.WriteTotalTimeoutMultiplier = 0;				// not used
	// Specify value is added to the product of the 
	// WriteTotalTimeoutMultiplier member
	comTimeOut.WriteTotalTimeoutConstant = 0;				// not used

	uiTimerIdentMain = 0;			// process timer for Main State Machine
	uiTimerIdentTCPIPRefresh = 0;	// process timer for Socket Comms
	uiTimerIdentControlClose = 0;	// process timer for Closure Timeout for Control Socket
	uiTimerIdentSerialComms = 0;	// process timer for Serial Port Maintenance

	if (!this->SetUpEthernet())
	{
		AfxMessageBox("Unable to Set Up Ethernet Communications");	// attempt to let someone know
	}
	if (!this->SetUpSerial())
	{
		AfxMessageBox("Unable to Set Up Serial Communications");	// attempt to let someone know
	}
}

/*********************************************************************
  CControllerNW::~CControllerNW()

  Description
  Simple destructor for CControllerNW class. Destroys any residual
  communications objects.

  Parameters
  None
*********************************************************************/

CControllerNW::~CControllerNW()
{
	UINT i;
	
	for (i = 0U; i < CNW_NUMBER_OF_ETHERNET_SOCKETS; i++)
	{
		if (apControlSockets [i] != NULL)
		{
			delete apControlSockets [i];
			apControlSockets [i] = NULL;
		} // *if*
	} // *for*
	
	this->Controllers.RemoveAll();
	this->ComPortSerialRefresh.RemoveAll();
}

/*********************************************************************
  Routine
  CControllerNW::setLoggerObject

  Description
  Set pointer to logger object. This routine sets a pointer to
  a CLogger object. The logger object should be instantiated by
  the code which instantiates this class. It should be derived from
  (or simply) the supplied CLogger class.

  Parameters
  1 - CLogger* cLog	- pointer to Clogger object.

  Returns - void
*********************************************************************/

void CControllerNW::setLoggerObject(CLogger* cLog)
{
	pLogger = cLog;	// pointer to logger object
}

/*********************************************************************
  Routine
  CControllerNW::RefreshControllerList

  Description
  Client Request - Refresh Controller List. This routine clears any 
  current connections, removes all controllers from the current list 
  and broadcasts a message (both serial and ethernet) to request 
  controllers to repost their information.
  
  Parameters
  None.

  Returns - bool	- true if successful otherwise false
*********************************************************************/

bool CControllerNW::RefreshControllerList()
{
	bool bSuccessful= true;

	try
	{
		LogMessage(CLogger::LGR_SEVERITY_INFO,"Refreshing Controller List");

		RemoveAllControllers();	// tidy and remove all controllers

		iRefreshesRemaining_Ethernet	= CNW_REFRESH_ATTEMPTS;	// number of refresh attempts allowed
		iRefreshesRemaining_Serial		= CNW_REFRESH_ATTEMPTS;	// number of refresh attempts allowed

		///Udupa; Commented; 18Oct'12; Unnecessary and blocks serial ports
		///if (!SendRefreshMessage(CMS_COMMSTYPE_SERIAL))		bSuccessful = false;
		if (!SendRefreshMessage(CMS_COMMSTYPE_ETHERNET))	bSuccessful = false;

		return 	bSuccessful;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::RefreshControllerList - Error Detected");
		return false;
	}
}
	
/*********************************************************************
  Routine
  CControllerNW::SetUpEthernet

  Description
  Set Up Ethernet. We will get our ip address for comms from
  our hosts first entry in the ip list.
  
  Parameters
  None.

  Returns - bool	- true if successful otherwise false
*********************************************************************/

bool CControllerNW::SetUpEthernet()
{
	UINT	i;
	int     iCnt;
	int		iReturn;
	CString strMessage;

	try
	{
		LogMessage(CLogger::LGR_SEVERITY_INFO,"Setting Up Ethernet Communications");

		// destroy all the control sockets
		for (i = 0U; i < CNW_NUMBER_OF_ETHERNET_SOCKETS; i++)
		{
			if (apControlSockets [i] != NULL)
			{
				delete apControlSockets [i];
				apControlSockets [i] = NULL;
			} // *if*
		} // *for*

		// initialise ethernet interface
		char szHostName[128] = "";
		if(gethostname(szHostName, sizeof(szHostName)))
		{
			iReturn = GetLastError();
			strMessage.Format("Error in gethostname - %i", iReturn);
			throw strMessage;
		}

		// Get local IP addresses
		struct sockaddr_in SocketAddress;
		struct hostent     *pHost        = 0;

		pHost = gethostbyname(szHostName);
		if(!pHost)
		{
			iReturn = GetLastError();
			strMessage.Format("Error in gethostbyname - %i", iReturn);
			throw strMessage;
		}

		char aszIPAddresses[CNW_NUMBER_OF_ETHERNET_SOCKETS][20];		// one address per socket ...
		char aszLogMessage [128];

		uiNumberofEthernetPorts = 0U;

		for(iCnt = 0; ((pHost->h_addr_list[iCnt]) && (iCnt < CNW_NUMBER_OF_ETHERNET_SOCKETS)); ++iCnt)
		{
			memcpy(&SocketAddress.sin_addr, pHost->h_addr_list[iCnt], pHost->h_length);
			strcpy(aszIPAddresses[iCnt], inet_ntoa(SocketAddress.sin_addr));
			// ... create the interface to the socket ...
			astrLocalHostIP [iCnt] = aszIPAddresses[iCnt];
			// ... and then configure it.
			if (this->ConfigureUDPSocket (apControlSockets [iCnt], CMS_COMMSPORT_CLIENT_CONTROL, astrLocalHostIP [iCnt], true))
			{
				sprintf (aszLogMessage, "Ethernet socket [%u] successfully created for %s", iCnt + 1U, astrLocalHostIP [iCnt]);
				LogMessage(CLogger::LGR_SEVERITY_INFO, aszLogMessage);
				uiNumberofEthernetPorts++;
			}
			else
			{
				sprintf (aszLogMessage, "Unable to create Ethernet socket [%u] for %s", iCnt + 1U, astrLocalHostIP [iCnt]);
				LogMessage(CLogger::LGR_SEVERITY_ERROR, aszLogMessage);
				return false;
			}
		}

		return true;
	}
	catch (CString strMessage)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::SetUpEthernet - Error Detected");
		return false;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::SetUpEthernet - Error Detected");
		return false;
	}
}

/*********************************************************************
  Routine
  CControllerNW::SetUpSerial

  Description
  Set Up Serial Comms. Does little.
  
  Parameters
  None.

  Returns - bool -	True if Successful, otherwise False
*********************************************************************/

bool CControllerNW::SetUpSerial()
{
	try
	{
		LogMessage(CLogger::LGR_SEVERITY_INFO,"Setting Up Serial Communications");

		return true;
	}
	catch(...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - SetUpSerial");
		return false;
	}
}

/*********************************************************************
  Routine
  CControllerNW::OnSocketReceive

  Description
  This is a callback routine used by the CAAsnchSocket Class to return 
  messages which have arrived. This object must be registered with the
  CAAsnchSocket object for this mechanism to work. (and deregistered 
  finished with). Also the parameters must remain exactly as they are.
  This routine will process the message assuming it is an identification
  message. This routine will handle adding or modifying the controller.
  
  Parameters
  1 - CString& strBuff		- message received.
  2 - CString& sIPAddress	- ip address of sender
  3 - UINT& iPort			- port of sender

  Returns - void
*********************************************************************/

// sIPAddress = address of responding controller ...

void CControllerNW::OnSocketReceive(CString& strBuff, CString& sIPAddress, UINT& iPort) 
{
	CString strMessage;
	CController* newItem;
	CController* oldItem;

	// TODO: allow for multiple sockets ...
	
	try
	{
		// trace message
		strMessage = "Comms Host Control Port Message - " + strBuff;
		LogMessage(CLogger::LGR_SEVERITY_DEBUG,strMessage);

		newItem = new(CController);
		if (newItem != NULL)
		{
			newItem->initialise(strBuff);
			newItem->setControllerType(CController::CTR_TYPE_ETHERNET); // ethernet type
			newItem->setProtocol("UDP");								// default to UDP
			//JM-CARTER add workaround make sure pick correct ip
			__int64 diff= 0xffffffffffff;
			for(UINT eth_count=0;eth_count<uiNumberofEthernetPorts;eth_count++)
			{
				unsigned long deviceip= inet_addr(sIPAddress);
				unsigned long hostip= inet_addr(astrLocalHostIP[eth_count]);
				if(abs(long(deviceip-hostip)<diff)) {
					diff=abs(long(deviceip-hostip));
					newItem->setLocalHostIPAddress(astrLocalHostIP[eth_count]);//leecat
				}
			}
//			newItem->setLocalHostIPAddress(strzBroadcastIP);//leecat
			//JM-CARTER add workaround make sure pick correct ip
			newItem->setLoggerObject(pLogger);
			oldItem = FindFromSerial(newItem->getSerialNumber());
			if (oldItem == NULL)
			{
				this->Controllers.Add(*newItem);
				strMessage.Format("%7s - Added Controller Details - %s - %s - %s",
									newItem->getSerialNumber(), inet_ntoa(newItem->getIPAddress()),
									newItem->getModel(), newItem->getFirmware());
				LogMessage(CLogger::LGR_SEVERITY_INFO,strMessage);
			}
			else
			{
				if (oldItem->getControllerType() == CController::CTR_TYPE_ETHERNET)
				{
					oldItem->setIPAddress(newItem->getIPAddress().S_un.S_addr);
					oldItem->setModel(newItem->getModel());
					oldItem->setFirmware(newItem->getFirmware());
					strMessage.Format("%7s - Changed Controller Details - %s - %s - %s",
										oldItem->getSerialNumber(), inet_ntoa(oldItem->getIPAddress()),
										oldItem->getModel(), oldItem->getFirmware());
					LogMessage(CLogger::LGR_SEVERITY_INFO,strMessage);
				}
				else
					LogMessage(CLogger::LGR_SEVERITY_ERROR,
								"CControllerNW::OnSocketReceive - Attempt to Change Serial Controller to Ethernet");
			}
			delete newItem;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - OnSocketReceive");
	}
}	

/*********************************************************************
  Routine
  CControllerNW::NumberOfControllers

  Description
  Report on number of controllers in list.
  
  Parameters
  None.

  Returns - DWORD		Number of Controllers in Controller Array.
*********************************************************************/

int CControllerNW::NumberOfControllers()
{
	try
	{
		return (int) this->Controllers.GetSize();
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - NumberOfControllers");
		throw "Error in - NumberOfControllers";
	}
}

/*********************************************************************
  Routine
  CControllerNW::FindFromSerial

  Description
  Get the array index of the controller from its serial number.
  
  Parameters
  1 - CString* strSerialNumber	- Ascii Serial Number.

  Returns - CController* -	Pointer to the Controller Object
*********************************************************************/

CController* CControllerNW::FindFromSerial(const CString strSerialNumber)
{
	try
	{
		for ( int i = 0; i <= this->Controllers.GetUpperBound(); i++)
		{
			if ( strSerialNumber == this->Controllers[i].getSerialNumber())
				return &this->Controllers[i];
		}

		return NULL;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - FindFromSerial");
		throw "Error in - FindFromSerial";
	}
}

/*********************************************************************
  Routine
  CControllerNW::FindFromCommsPort

  Description
  Get the array index of the controller from its comms port.
  
  Parameters
  1 - CString* strCommsPort	- Ascii Comms Port.

  Returns - CController* -	Pointer to the Controller Object
*********************************************************************/

CController* CControllerNW::FindFromCommsPort(const CString strCommsPort)
{
	try
	{
		for ( int i = 0; i <= this->Controllers.GetUpperBound(); i++)
			if ( strCommsPort == this->Controllers[i].getComPort())
				return &this->Controllers[i];
		return NULL;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - FindFromCommsPort");
		throw "Error in - FindFromCommsPort";
	}
}

/*********************************************************************
  Routine
  CControllerNW::getSerialNumber

  Description
  Get Serial Number of Controller from array index.
  
  Parameters
  1 - const int i	- array index of required controller.

  Returns - CString -	Ascii Serial Number
*********************************************************************/

CString	CControllerNW::getSerialNumber(const int i)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			// give back value
			return this->Controllers[i].getSerialNumber();
		else
			return "";
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - getSerialNumber");
		throw "Error in - getSerialNumber";
	}
}

/*********************************************************************
  Routine
  CControllerNW::getCommsPort

  Description
  Get Comms Port of Controller from array index.
  
  Parameters
  1 - const int i	- array index of required controller.

  Returns - CString -	Ascii Comms Port
*********************************************************************/

CString	CControllerNW::getCommsPort(const int i)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			// give back value
			return this->Controllers[i].getComPort();
		else
			return "";
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - getCommsPort");
		throw "Error in - getCommsPort";
	}
}

/*********************************************************************
  Routine
  CControllerNW::getModel

  Description
  Get Model of Controller from array index.
  
  Parameters
  1 - const int i	- array index of required controller.

  Returns - CString -	Ascii Model Text
*********************************************************************/

CString	CControllerNW::getModel(const int i)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			// give back value
			return this->Controllers[i].getModel();
		else
			return "";
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - getModel");
		throw "Error in - getModel";
	}
}

/*********************************************************************
  Routine
  CControllerNW::getFirmware

  Description
  Get Firmware of Controller from array index.
  
  Parameters
  1 - const int i	- array index of required controller.

  Returns - CString -	Ascii FirmWare Text
*********************************************************************/

CString	CControllerNW::getFirmware(const int i)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			// give back value
			return this->Controllers[i].getFirmware();
		else
			return "";
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - getFirmware");
		throw "Error in - getFirmware";
	}
}

/*********************************************************************
  Routine
  CControllerNW::getIPAddress

  Description
  Get IP Address of Controller from array index.
  
  Parameters
  1 - const int i	- array index of required controller.

  Returns - in_addr -	in_addr structure holding IP Address of Controller
*********************************************************************/

in_addr CControllerNW::getIPAddress(const int i)
{
	in_addr	iaNone;
	try
	{
		iaNone.S_un.S_addr = inet_addr("0.0.0.0");
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			// give back value
			return this->Controllers[i].getIPAddress();
		else
			return iaNone;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - getIPAddress");
		throw "Error in - getIPAddress";
	}
}

CString CControllerNW::getIPAddressLocalHost (const int i)
{
	CString unknownHost;
	
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
		{
			// give back value
			return this->Controllers[i].getLocalHostIPAddress();
		}
		else
		{
			unknownHost = "0.0.0.0";
			return unknownHost;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - getIPAddressLocalHost");
		throw "Error in - getIPAddressLocalHost";
	}
}
	
/*********************************************************************
  Routine
  CControllerNW::getControllerType

  Description
  Get type of Controller. This routine returns the type of Controller.
  
  Parameters
  1 - const int i	- array index of required controller.

  Returns - int CTR_TYPE		Controller Type.
*********************************************************************/

int CControllerNW::getControllerType(const int i)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].getControllerType();
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::getControllerType - Controller Index Out of Range");
			return -1;
		}
	}
	catch (...)
	{
		// failed
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::getControllerType - Error Detected");
		return -1;
	}
}

/*********************************************************************
  Routine
  CControllerNW::getProtocol

  Description
  Get protocol. This routine returns the currently set protocol.
  
  Parameters
  1 - const int i	- array index of required controller.

  Returns - CString			"TCP" if TCP
							"UDP" if UDP
							"FAILED" if failure in routine
*********************************************************************/

CString CControllerNW::getProtocol(const int i)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].getProtocol();
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::getProtocol - Controller Index Out of Range");
			return "FAILED";
		}
	}
	catch (...)
	{
		// failed
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::getProtocol - Error Detected");
		return "FAILED";
	}
}

/*********************************************************************
  Routine
  CControllerNW::setProtocol

  Description
  Set protocol. This routine sets the required protocol.
  
  Parameters
  1 - const int i		- array index of required controller.
  2 - CString strProto	- required protocol (Must be "TCP" or "UDP").

  Returns - bool	- true if successful otherwise false
*********************************************************************/
bool CControllerNW::setProtocol(const int i, CString strProto)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].setProtocol(strProto);
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::setProtocol - Controller Index Out of Range");
			return false;
		}
	}
	catch (...)
	{
		// failed
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::setProtocol - Error Detected");
		return false;
	}
}

/*********************************************************************
  Routine
  CControllerNW::getControllerCMDStatusValues

  Description
  Get Message values for last SEND COMMAND for this Controller.
  
  Parameters
  1 - const int i							- array index of required controller.
  2 - CString	&strMsgState				- current message state description
  3 - CString	&strLastCMDMsgSent			- last user Command message sent
  4 - CString	&strLastCMDMsgReceived		- last message received from user Command
  5 - bool		bErrorInLastMessageReceived	- last received had errors or not

  Returns - bool -	True if Successful, otherwise False
*********************************************************************/

bool CControllerNW::getControllerCMDStatusValues(const int i,
												 CString &strMsgState,
												 CString &strLastCMDMsgSent,
												 CString &strLastCMDMsgReceived,
												 bool &bErrorInLastMessageReceived)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].getControllerCMDStatusValues(strMsgState, strLastCMDMsgSent,
																	              strLastCMDMsgReceived, bErrorInLastMessageReceived);
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::getControllerCMDStatusValues - Controller Index Out of Range");
			return false;
		}
	}
	catch (...)
	{
		// failed
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::getControllerCMDStatusValues - Error Detected");
		return false;
	}
}

/*********************************************************************
  Routine
  CControllerNW::RequestSendCommand

  Description
  Client Request - Send Command Message.
  
  Parameters
  1 - const int i			- controller to index
  2 - int &iMsgStatus		- messaging status on completion of send
								(see enum MSG_STATUS)
  3 - CString* strCommand	- ascii message to send
  4 - UINT uiTimeout		- timeout for response before send retry
								(milliseconds or zero for no retries)
  5 - UINT uiRetries		-  number of times to retry (or zero for 
								no retries)

  Returns - bool -	True if Successful, otherwise False
*********************************************************************/

bool CControllerNW::RequestSendCommand(const int i, 
									   int &iMsgStatus, 
									   CString * strCommand, 
									   UINT uiTimeout, 
									   UINT uiRetries)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].RequestSendCommand(iMsgStatus, strCommand, uiTimeout, uiRetries);
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::RequestSendCommand - Controller Index Out of Range");
			return false;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::RequestSendCommand - Error Detected");
		throw; // and rethrow
	}
}

/*********************************************************************
  Routine
  CControllerNW::RequestReceivedMessage

  Description
  Client Request - Receive Command Message. Try to retrieve a message
  that may have been sent by the controller.
  
  Parameters
  1 - const int i			- controller to index
  2 - int &iMsgStatus		- current messaging status
								(see enum MSG_STATUS)
  3 - CString* strCommand	- ascii message received
  4 - bool &bReceiveError	- true if message is ok else false.

  Returns - bool -	True if Successful, otherwise False
*********************************************************************/

bool CControllerNW::RequestReceivedMessage(const int i, int &iMsgStatus, CString &strMessage, bool &bReceiveError)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].RequestReceivedMessage(iMsgStatus, strMessage, bReceiveError);
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::RequestReceivedMessage - Controller Index Out of Range");
			return false;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::RequestReceivedMessage - Error Detected");
		throw; // and rethrow
	}
}

/*********************************************************************
  Routine
  CControllerNW::ProcessTimer

  Description
  Service Timer Routine. Used to catch timeouts which are passed here
  by the callback routine which is set up when a system timer is
  started. Will invoke the appropriate timer processing method.
  (ie the main state timeout process or the socket closure timeout
  process)
  
  Parameters
  1 - UINT idEvent	- timer identifier
  2 - DWORD dwTime	- current system time

  Returns - void.
*********************************************************************/

void CControllerNW::ProcessTimer (UINT_PTR idEvent, DWORD dwTime)
{
	CString strMessage;

	try
	{
		// if Serial Port Maintenance timer
		if (idEvent == uiTimerIdentSerialComms)
		{
			uiTimerIdentSerialComms = 0;
			ProcessSerialCommsTimer(idEvent, dwTime);
		}
		else
		{
			// if Refresh Socket Comms timer
			if (idEvent == uiTimerIdentTCPIPRefresh)
			{
				uiTimerIdentTCPIPRefresh = 0;
				ProcessRefreshSocketTimer(idEvent, dwTime);
			}
			else
			{
				// if Socket Closure Timeout then we can get rid of the offending socket
				// ProcessSocketClosureTimer should clear the timer ident for us.
				if (idEvent == uiTimerIdentControlClose)
					ProcessSocketClosureTimer(idEvent, dwTime);
				else
				{
					strMessage.Format("CControllerNW::ProcessTimer() Fired For Unknown Timer - %u", idEvent);
					LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
				}
			}
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in CControllerNW::ProcessTimer()");
	}
}

/*********************************************************************
  Routine
  CControllerNW::ProcessSocketClosureTimer

  Description
  Socket Closure Service Timer. Handle timeouts which are used by the
  socket closure mechanism to allow TCP sockets to close properly after
  being requested to do so. Once the timeout has occured we assume we
  can delete the socket object.
  
  Parameters
  1 - UINT idEvent	- timer identifier
  2 - DWORD dwTime	- current system time

  Returns - void.
*********************************************************************/

void CControllerNW::ProcessSocketClosureTimer(UINT_PTR idEvent, DWORD dwTime)
{
	CString strMessage;

	// TODO: insert additional code to process all sockets ...
	
	try
	{
		// we can now get rid of the offending socket
		if (idEvent == uiTimerIdentControlClose)
		{
			strMessage.Format("CControllerNW::ProcessSocketClosureTimer() Closing Control Socket After Timer - %u", idEvent);
			LogMessage(CLogger::LGR_SEVERITY_INFO,strMessage);

			uiTimerIdentControlClose = 0;
			if (apControlSockets [0] != NULL)
			{
				delete apControlSockets [0];
				apControlSockets [0] = NULL;
			}
			else
			{
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::ProcessSocketClosureTimer() Fired For NULL Socket");
			}
		}
		else
		{
			strMessage.Format("CControllerNW::ProcessSocketClosureTimer() Fired For Unknown Timer - %u", idEvent);
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
			return;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in CControllerNW::ProcessSocketClosureTimer()");
	}
}

/*********************************************************************
  Routine
  CControllerNW::ProcessSerialCommsTimer

  Description
  Serial Comms Service Timer. When Serial Comms is selected will 
  attempt to retrieve any messages and timeout comms ports once
  timeout attempts are expired. Will also process any outstanding
  refreshing controllers and add to the active list once processed
  
  Parameters
  1 - UINT idEvent	- timer identifier
  2 - DWORD dwTime	- current system time

  Returns - void.
*********************************************************************/

void CControllerNW::ProcessSerialCommsTimer(UINT_PTR idEvent, DWORD dwTime)
{
	CString		strMessage;
	CString		strSendCmdMessage;
	char		szInBuffer[201];
	DWORD		dwNumberOfBytesRead;
	int			iArrowPos;
	int			iSNrPos;
	int			i;
	DWORD		dwBytesWritten;
	bool		bCheckTimedOut;
	CString		strDummy;
	CString		strCommand;

	static bool	bLastMessageNull=false;		// flags if last message was null

	try
	{
		// clear until next timer
		uiTimerIdentSerialComms = 0;

		// for each coms port with refresh outstanding
		for (i=0; i<= this->ComPortSerialRefresh.GetUpperBound(); i++)
		{
			bCheckTimedOut = true;		// this controller merits timeout checks
			// read all available and if ">" present then complete recognition
			if (ReadFile(this->ComPortSerialRefresh[i].getFileDescriptor(),
			  szInBuffer,					// pointer to buffer that receives data
			  100,							// number of bytes to read
			  &dwNumberOfBytesRead,			// pointer to number of bytes read
			  NULL))						// pointer to structure for data
			{
				if (dwNumberOfBytesRead != 0)
				{
					szInBuffer[dwNumberOfBytesRead] = 0;		// zero terminated string
					this->ComPortSerialRefresh[i].appendInputBuffer(szInBuffer);	// append to buffer
					strMessage.Format("Com Buffer of Com Port - %s = %s", 
										this->ComPortSerialRefresh[i].getComPort(),
										this->ComPortSerialRefresh[i].getInputBuffer());
					LogMessage(CLogger::LGR_SEVERITY_INFO,strMessage);

					iArrowPos = this->ComPortSerialRefresh[i].getInputBuffer().Find(">");
					if (iArrowPos > -1)
					{
						switch (this->ComPortSerialRefresh[i].getControllerState())
						{
						case CController::CTR_STATE_REFRESH_SENT:	// "ES" sent
							iSNrPos = this->ComPortSerialRefresh[i].getInputBuffer().Find("Serial number ");
							if (iSNrPos > -1)
							{
								iSNrPos = iSNrPos + 14;
								if (iSNrPos < iArrowPos-6)
								{
									this->ComPortSerialRefresh[i].setSerialNumber(this->ComPortSerialRefresh[i].getInputBuffer().Mid(iSNrPos, 6));
								
									// send "VR" message...
									strSendCmdMessage = "VR\r\n";	// 
									if (WriteFile(this->ComPortSerialRefresh[i].getFileDescriptor(),// handle to file to write to
										strSendCmdMessage,			// pointer to data to write to file
										strSendCmdMessage.GetLength(),// number of bytes to write
										&dwBytesWritten,NULL) == 0)	// pointer to number of bytes written
									{
										strMessage.Format("WriteFile Error - %i on %s", GetLastError(), this->ComPortSerialRefresh[i].getComPort());
										LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
									}

									// set up new controller state...
									this->ComPortSerialRefresh[i].setControllerState(CController::CTR_STATE_VR_SENT); // awaiting identification message
									this->ComPortSerialRefresh[i].setInputBuffer("");	// empty buffer to start again
								}
							}

							break;

						case CController::CTR_STATE_VR_SENT:		//"VR" sent
							// set model ???
							this->ComPortSerialRefresh[i].setModel(this->ComPortSerialRefresh[i].getInputBuffer().Left(iArrowPos));// create new controller in controller list
							// create new controller
							this->Controllers.Add(this->ComPortSerialRefresh[i]);
							strMessage.Format("%7s - Added Controller Details - %s - %s", 
											this->Controllers[this->Controllers.GetUpperBound()].getSerialNumber(),
											this->Controllers[this->Controllers.GetUpperBound()].getModel(),
											this->Controllers[this->Controllers.GetUpperBound()].getComPort());
							LogMessage(CLogger::LGR_SEVERITY_INFO,strMessage);
							// delete refresh controller
							this->ComPortSerialRefresh.RemoveAt(i--);
							bCheckTimedOut = false;		// this controller no longer merits timeout checks
							// set up new controller state...
							this->Controllers[this->Controllers.GetUpperBound()].setControllerState(CController::CTR_STATE_OPEN); // open as we leave for later

						default: // do nothing
							;
						}
					}
				}
			}
			else
			{
				strMessage.Format("ReadFile Error - %i on %s", GetLastError(), this->ComPortSerialRefresh[i].getComPort());
				LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
			}

			if (bCheckTimedOut)		// if this controller merits timeout checks
			{
				if (this->ComPortSerialRefresh[i].decTimeOutCount() <= 0)
				{
					// time out so kill off
					strMessage.Format("Removing Com Port from Refresh - %s", this->ComPortSerialRefresh[i].getComPort());
					LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
					if (this->ComPortSerialRefresh[i].getFileDescriptor() != INVALID_HANDLE_VALUE)
						if (!CloseHandle(this->ComPortSerialRefresh[i].getFileDescriptor()))	// close file
						{
							strMessage.Format("CloseHandle Error - %i on %s", GetLastError(), this->ComPortSerialRefresh[i].getComPort());
							LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
						}
					this->ComPortSerialRefresh.RemoveAt(i--);

					// if last controller try to do refresh a second time if allowed
					if ((this->ComPortSerialRefresh.GetUpperBound () == -1) && (iRefreshesRemaining_Serial > 0))
					{
						if (!this->SendRefreshMessage(CMS_COMMSTYPE_SERIAL))	// do second refresh
						{
							LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::ProcessSerialCommsTimer - failed refresh");
						}
						// kill off serial comms timer started by refresh as will be started later this routine
						CTimer::KillTimer(&uiTimerIdentSerialComms);
						uiTimerIdentSerialComms = 0;
					}
				}
			}
		}

		CTimer::settimer(CNW_COM_PORTS_SERVICE_TIME, &uiTimerIdentSerialComms);	// try serial comms again in....

	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in CControllerNW::ProcessSerialCommsTimer()");
	}
}

/*********************************************************************
  Routine
  CControllerNW::ProcessRefreshSocketTimer

  Description
  Refresh Socket Timer. When this timer fires we should re-poll with
  the gardasoft search message...
  
  Parameters
  1 - UINT idEvent	- timer identifier
  2 - DWORD dwTime	- current system time

  Returns - void.
*********************************************************************/

void CControllerNW::ProcessRefreshSocketTimer(UINT_PTR idEvent, DWORD dwTime)
{
	try
	{
		// clear forever, to allow any straglers in..
		uiTimerIdentTCPIPRefresh = 0;

		// try to do refresh a second time if allowed
		if (iRefreshesRemaining_Ethernet > 0)
		{
			if (!this->SendRefreshMessage(CMS_COMMSTYPE_ETHERNET))	// do second refresh
			{
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::ProcessRefreshSocketTimer - Failed Refresh");
			}
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in CControllerNW::ProcessRefreshSocketTimer()");
	}
}

/*********************************************************************
  Routine
  CControllerNW::SendRefreshMessage

  Description
  Broadcasts a message to all controllers to update their identification information.
  
  Parameters
  1 - enum CMS_COMMSTYPE iCommsType	- flag ethernet/serial

  Returns - bool	- true if successful otherwise false
*********************************************************************/

bool CControllerNW::SendRefreshMessage(enum CMS_COMMSTYPE iCommsType)
{
	CString			strErrorMessage;
	CString			strSendCmdMessage;
	CString			strPortName;
	CString			strPortNameAlias;
	int				i;
	HANDLE			hComPort;
	DCB				dcbComPort = {0};
	DWORD			dwBytesWritten = 0;
	CController*	newItem;

	dcbComPort.DCBlength = sizeof(DCB);

	try
	{
		// refresh according to interface
		switch (iCommsType)
		{
		// no interface
		case CMS_COMMSTYPE_NOTINIT:
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::SendRefreshMessage - No Comms Type Requested");
			return false;
			
		// serial interface
		case CMS_COMMSTYPE_SERIAL:
			--iRefreshesRemaining_Serial;			// decrement allowed tries

			LogMessage(CLogger::LGR_SEVERITY_INFO,"Refreshing CMS_COMMSTYPE_SERIAL");
			
			// for com port = 1 to Max
			for (i=1; i<=CNW_NUMBER_OF_SERIAL_PORTS; i++)
			{
				strPortName.Format("COM%i", i);
				strPortNameAlias.Format("\\\\.\\COM%i", i);
				// only if not already there
				if (this->FindFromCommsPort(strPortName) == NULL)
				{
					hComPort = CreateFile(strPortNameAlias,			// port device
								GENERIC_READ | GENERIC_WRITE,	// Specify mode that open device.
								0,								// the device isn't shared.
								NULL,							// the object gets a default security.
								OPEN_EXISTING,					// Specify which action to take on file. 
								0,								// default.
								NULL);							// default.

					if (hComPort == INVALID_HANDLE_VALUE)
					{
						strErrorMessage.Format("CControllerNW::SendRefreshMessage - CreateFile Error - %i on %s", GetLastError(), strPortName);
						LogMessage(CLogger::LGR_SEVERITY_ERROR,strErrorMessage);
					}
					else
					{
						// set up baud etc...
						if (GetCommState(hComPort,&dcbComPort) == 0)
						{
							strErrorMessage.Format("GetCommState Error - %i on %s", GetLastError(), strPortName);
							LogMessage(CLogger::LGR_SEVERITY_ERROR,strErrorMessage);
						}
						dcbComPort.BaudRate = CBR_115200;
						dcbComPort.ByteSize = 8;
						dcbComPort.StopBits = ONESTOPBIT;
						dcbComPort.Parity	= NOPARITY;
						if (SetCommState(hComPort,&dcbComPort) == 0)
						{
							strErrorMessage.Format("SetCommState Error - %i on %s", GetLastError(), strPortName);
							LogMessage(CLogger::LGR_SEVERITY_ERROR,strErrorMessage);
						}

						// set the time-out parameters into device control.
						if (SetCommTimeouts(hComPort,&comTimeOut) == 0)
						{
							strErrorMessage.Format("SetCommTimeouts Error - %i on %s", GetLastError(), strPortName);
							LogMessage(CLogger::LGR_SEVERITY_ERROR,strErrorMessage);
						}

						strErrorMessage = "Refreshing - " + strPortName;
						LogMessage(CLogger::LGR_SEVERITY_INFO,strErrorMessage);

						// send "ES" message...
						strSendCmdMessage = "ES\r\n";	// 
						if (WriteFile(hComPort,			// handle to file to write to
							strSendCmdMessage,			// pointer to data to write to file
							strSendCmdMessage.GetLength(),// number of bytes to write
							&dwBytesWritten,NULL) == 0)	// pointer to number of bytes written
						{
							strErrorMessage.Format("CControllerNW::SendRefreshMessage - WriteFile Error - %i on %s", GetLastError(), strPortName);
							LogMessage(CLogger::LGR_SEVERITY_ERROR,strErrorMessage);
						}

						// set up controller into list...
						newItem = new(CController);
						if (newItem != NULL)
						{
							newItem->setFileDescriptor(hComPort);			// save FD handle
							newItem->setComPort(strPortName);				// save com port name
							newItem->setTimeOutCount(CNW_COM_PORTS_REFRESH_TIMEOUT);	// allow timeout
							newItem->setControllerState(CController::CTR_STATE_REFRESH_SENT); // awaiting identification message
							newItem->setControllerType(CController::CTR_TYPE_SERIAL); // serial type
							newItem->setLoggerObject(pLogger);				// initialise logger	
							newItem->setInputBuffer("");					// empty buffer to start
							this->ComPortSerialRefresh.Add(*newItem);

							delete newItem;
						}
					}
				}
			}

			// kick off serial timer if ports to process
			if (this->ComPortSerialRefresh.GetSize() > 0)
				CTimer::settimer(CNW_COM_PORTS_SERVICE_TIME, &uiTimerIdentSerialComms);	// try serial comms in....

			break;

		// ethernet interface
		case CMS_COMMSTYPE_ETHERNET:
			// TODO: process all ethernet sockets ...
			--iRefreshesRemaining_Ethernet;			// decrement allowed tries

			LogMessage(CLogger::LGR_SEVERITY_INFO,"Refreshing CMS_COMMSTYPE_ETHERNET");

			for (i = 0; i < (int) uiNumberofEthernetPorts; i++)
			{
				// copy sending broadcast address for use by receive function ...
				strzBroadcastIP = astrLocalHostIP [i];
				// set timer here as may bomb out after SendTo
				CTimer::settimer(CNW_SOCKET_REFRESH_TIMEOUT, &uiTimerIdentTCPIPRefresh);	// socket refresh interval
				// broadcast "search" message ...
				if (apControlSockets [i]->SendTo (CNW_SEARCH, sizeof (CNW_SEARCH), CMS_COMMSPORT_HOST_CONTROL, BCAST) == SOCKET_ERROR)
				{
					strErrorMessage = "CControllerNW::SendRefreshMessage - " + apControlSockets [i]->GetLastError();
					LogMessage (CLogger::LGR_SEVERITY_ERROR,strErrorMessage);
					return false;
				} // *if*
			} // *for*
			break;
	
		default:
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::SendRefreshMessage - Invalid Comms Type");
			return false;
		}

		return true;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::SendRefreshMessage - Error Detected");
		return false;
	}
}
	
/*********************************************************************
  Routine
  CControllerNW::LogMessage

  Description
  Log Message and severity. Note the implementation of AddLogMessage
  may be unknown.
  
  Parameters
  1 - CLogger::LGR_SEVERITY iSeverity	- (see enum LGR_SEVERITY)
  2 - char * strLogMsg					- ascii message to log

  Returns - void
*********************************************************************/

void CControllerNW::LogMessage(CLogger::LGR_SEVERITY iSeverity, char * strLogMsg)
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
  CControllerNW::LogMessage

  Description
  Log Message and severity. Note the implementation of AddLogMessage
  may be unknown.
  
  Parameters
  1 - CLogger::LGR_SEVERITY iSeverity	- (see enum LGR_SEVERITY)
  2 - CString & strLogMessage			- ascii message to log

  Returns - void
*********************************************************************/
void CControllerNW::LogMessage(CLogger::LGR_SEVERITY iSeverity, CString & strLogMessage)
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
  CControllerNW::ConfigureUDPSocket

  Description
  Set up or recreate a UDP socket. Will attempt to reuse a socket if
  appropriate to do so if not will recreate if allowed to do so as this
  routine may be called from situations where deletion of the socket 
  should under no circumstances be deleted.
  
  Parameters
  1 - CAAsyncSocket* &rpSocket			- address of socket pointer
  2 - UINT nLocalSocketPort				- local socket port number
  3 - LPCTSTR lpszLocalSocketAddress	- IP Address
  4 - boolean bAllowedToDelete			- true if allowed to delete 
											socket false otherwise

  Returns - bool -	True if Successful, otherwise False
*********************************************************************/

bool CControllerNW::ConfigureUDPSocket(CAAsyncSocket* &rpSocket, UINT nLocalSocketPort, LPCTSTR lpszLocalSocketAddress, boolean bAllowedToDelete)
{
	int iReturn;
	CString strMessage;
	bool bOptval = true;

	try
	{
		// must have a socket for the next bit
		if (rpSocket == NULL)
			rpSocket = new(CAAsyncSocket);

		// handle according to protocol of socket
		switch (rpSocket->getProtocol())
		{
		case CAAsyncSocket::AAS_PROTOCOL_NOTINIT:
			break; // new socket so will just go configure

		case CAAsyncSocket::AAS_PROTOCOL_UDP:
			LogMessage(CLogger::LGR_SEVERITY_INFO,"ConfigureUDPSocket - Reusing Socket - Already Set up as UDP");
			return true; // just reuse as UDP is used in broadcast mode

		default:
			strMessage.Format("Error Unknown Protocol Setting - %i", rpSocket->getProtocol()); 
			throw strMessage;
		}

		// if got to here then ok for UDP creation
		LogMessage(CLogger::LGR_SEVERITY_DEBUG,"Attempting to set up UDP Socket");

		rpSocket->setLoggerObject(pLogger);
		// initialise ethernet interface
		rpSocket->Register(this);         // register myself for socket events
		rpSocket->setProtocol(CAAsyncSocket::AAS_PROTOCOL_UDP);
	
		// Set up a UDP Socket
		if (!rpSocket->Socket(SOCK_DGRAM, FD_READ|FD_CLOSE))
		{
			iReturn = rpSocket->GetLastError();
			strMessage.Format("Socket Error - %i", iReturn);
			throw strMessage;
		}

		// set up as re use so we can reattach
		if (!rpSocket->SetSockOpt(SO_REUSEADDR , &bOptval, sizeof(bOptval),SOL_SOCKET ))
		{
			iReturn = rpSocket->GetLastError();
			strMessage.Format("SetSockOpt SO_REUSEADDR Error - %i", iReturn);
			throw strMessage;
		}

		// bind to this address
		if (!rpSocket->Bind(nLocalSocketPort,lpszLocalSocketAddress))
		{
			iReturn = rpSocket->GetLastError();
			strMessage.Format("Bind Error - %i", iReturn);
			throw strMessage;
		}

		// allow broadcasts from here
		if (!rpSocket->SetSockOpt(SO_BROADCAST , &bOptval, sizeof(bOptval),SOL_SOCKET ))
		{
			iReturn = rpSocket->GetLastError();
			strMessage.Format("SetSockOpt SO_BROADCAST Error - %i", iReturn);
			throw strMessage;
		}

		LogMessage(CLogger::LGR_SEVERITY_DEBUG,"UDP Socket Set Up");

		// came through ok
		rpSocket->setState(CAAsyncSocket::AAS_STATE_ACTIVE);
		return true;
	}
	catch (CString strMessage)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - ConfigureUDPSocket");

		// check allowed to delete
		if (bAllowedToDelete)
		{
			if (rpSocket != NULL)
			{
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Destroying Old Socket");
				delete rpSocket;
				rpSocket = NULL;
			}
		}
		else
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Not Allowed To Delete Socket From Here");

		return false;
	}

	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - ConfigureUDPSocket");

		// check allowed to delete
		if (bAllowedToDelete)
		{
			if (rpSocket != NULL)
			{
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Destroying Old Socket");
				delete rpSocket;
				rpSocket = NULL;
			}
		}
		else
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Not Allowed To Delete Socket From Here");

		return false;
	}
}

/*********************************************************************
  Routine
  CControllerNW::CloseOrRemoveSocket

  Description
  Close or Remove a socket. Will attempt to close a socket if ok to do 
  so. If TCP and not ok will request shutdown and kick off a timer to 
  get rid of once shutdown is completed or timed out. This routine may 
  be called from situations where deletion of the socket should under 
  no circumstances be deleted.
  
  Parameters
  1 - CAAsyncSocket* &rpSocket			- address of socket pointer
  2 - UINT* pidEvent					- pointer for event id of shutdown timer.
  3 - boolean bAllowedToDelete			- true if allowed to delete 
											socket false otherwise

  Returns - bool -	True if Successful, otherwise False
*********************************************************************/

bool CControllerNW::CloseOrRemoveSocket(CAAsyncSocket* &rpSocket, UINT_PTR* pidEvent, boolean bAllowedToDelete)
{
	CString strMessage;

	try
	{
		// must have a socket for the next bit
		if (rpSocket != NULL)
		{
			// handle according to protocol of socket
			switch (rpSocket->getProtocol())
			{

			case CAAsyncSocket::AAS_PROTOCOL_UDP:
			case CAAsyncSocket::AAS_PROTOCOL_NOTINIT:
			default:
			
				CTimer::KillTimer(pidEvent);
				rpSocket->setState(CAAsyncSocket::AAS_STATE_CLOSED);
				// check allowed to delete
				if (bAllowedToDelete)
				{
					// destroy the socket
					delete rpSocket;
					rpSocket = NULL;
					LogMessage(CLogger::LGR_SEVERITY_INFO,"CloseOrRemoveSocket - Socket Destroyed");
					break;
				}
				else
				{
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Not Allowed To Delete Socket From Here");
					return false;
				}
			}
			return true;
		}
		else
			return false;
	}
	catch (char* strMessage)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - CControllerNW::CloseOrRemoveSocket");
		return false;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - CControllerNW::CloseOrRemoveSocket");
		return false;
	}
}

/*********************************************************************
  Routine
  CControllerNW::RemoveAllControllers

  Description
  This routine removes all controllers from the controller list and 
  the refresh controller list. It simply performs any closures then
  doas a RemoveAll on the list.
  
  Parameters
  None.

  Returns - bool	- true if successful otherwise false
*********************************************************************/

bool CControllerNW::RemoveAllControllers()
{
	CString strMessage;
	int		i;

	try
	{
		// kill off serial comms timer
		CTimer::KillTimer(&uiTimerIdentSerialComms);

		// remove any refresh controllers
		// for each comms port with refresh outstanding
		for (i=0; i<= this->ComPortSerialRefresh.GetUpperBound(); i++)
		{
			// close connection
			this->ComPortSerialRefresh[i].DisconnectFromController();
		}

		// remove any initialised controllers
		// for initialised CONTROLLERS
		for (i=0; i<= this->Controllers.GetUpperBound(); i++)
		{
			// close connection
			this->Controllers[i].DisconnectFromController();
		}

		// get rid of all controllers
		this->Controllers.RemoveAll();
		this->ComPortSerialRefresh.RemoveAll();

		return true;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::RemoveAllControllers - Error Detected");
		return false;
	}
}

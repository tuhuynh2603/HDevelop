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
//	Controller.cpp: implementation of the CController class				//
//																		//
//	This class is simply an object representation of a Controller item	//
//	-----------------------------------------------------------------	//
*/

#include "stdafx.h"
#include "Controller.h"
#include <iostream>
#include <fstream>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*********************************************************************
  CController::CController()

  Description
  Simple constructor for CController class. Initilises class data

  Parameters
  None
*********************************************************************/
CController::CController()
{
	hFileDescriptor			= INVALID_HANDLE_VALUE;
	sSerialNumber			= "";
	sModel					= "";
	iaIPAddress.S_un.S_addr = 0;
	sComPort				= "";
	sFirmware				= "";
	boIsValid				= false;
	iTimeOutCount			= 0;
	pLogger					= NULL;

	iControllerType			= CTR_TYPE_NOTINIT;
	iControllerState		= CTR_STATE_NOTINIT;
	ieRequiredProtocol		= CAAsyncSocket::AAS_PROTOCOL_NOTINIT;

	iMainState				= MSM_MAINSTATE_IDLE;
	iMsgState				= MSG_STATUS_IDLE;
	iLastCMDMsgState		= MSG_STATUS_IDLE;

	pDataSocket				= NULL;
	strLastMessageSent		= "";
	strLastCMDMessageReceived	= "";
	bLastMessageReceivedInError = false;
	bLastMessageBroadcast	= true;

	strLocalHostIP			= "";

	uiTimerIdentMain		= 0;
	uiTimerIdentDataClose	= 0;
	uiTimerIdentSerialComms = 0;

	iChainCommandNumber		= CHN_NONE;		// no command yet command
	iChainNextIndex			= 0;			// index of next link in chain

	bUpgradeFirmwareActive	= false;		// upgrading firmware sequence
	bIPChangeActive			= false;		// Changing IP sequence
	bLastFirmwareUpgradeSuccessful	= false;// Result flag for last firmware upgrade
	bLastIPChangeSuccessful	= false;		// Result flag for last Changing IP sequence

	bIPRefreshRequired		= false;

	pfileFirmware			= NULL;
}

/*********************************************************************
  CController::CController()

  Description
  Initialisation constructor for CController class. Initilises class 
  data

  Parameters
  1 - HANDLE	-	handle of File Descriptor
  2	- CString	-	ascii serial number
  3 - CString	-	ascii model number
  4 - DWORD		-	in_addr representation of IP address
  5	- CString	-	Com port number "COMX"
  6 - CString	-	ascii firmware
  7	- int		-	Time Out Counter
*********************************************************************/

CController::CController(HANDLE hFD, CString sSN, CString sM, DWORD dA, CString sCP, CString sF, int iTOC)
{
	hFileDescriptor			= hFD;
	sSerialNumber			= sSN;
	sModel					= sM;
	iaIPAddress.S_un.S_addr = dA;
	sComPort				= sCP;
	sFirmware				= sF;
	boIsValid				= true;
	iTimeOutCount			= iTOC;
	pLogger					= NULL;

	iControllerType			= CTR_TYPE_NOTINIT;
	iControllerState		= CTR_STATE_NOTINIT;
	ieRequiredProtocol		= CAAsyncSocket::AAS_PROTOCOL_NOTINIT;

	iMainState				= MSM_MAINSTATE_IDLE;
	iMsgState				= MSG_STATUS_IDLE;
	iLastCMDMsgState		= MSG_STATUS_IDLE;

	pDataSocket				= NULL;
	strLastMessageSent		= "";
	strLastCMDMessageReceived	= "";
	bLastMessageReceivedInError = false;
	bLastMessageBroadcast	= true;

	strLocalHostIP			= "";

	uiTimerIdentMain		= 0;
	uiTimerIdentDataClose	= 0;
	uiTimerIdentSerialComms = 0;

	iChainCommandNumber		= CHN_NONE;		// no command yet command
	iChainNextIndex			= 0;			// index of next link in chain

	bUpgradeFirmwareActive	= false;		// upgrading firmware sequence
	bIPChangeActive			= false;		// Changing IP sequence
	bLastFirmwareUpgradeSuccessful	= false;// Result flag for last firmware upgrade
	bLastIPChangeSuccessful	= false;		// Result flag for last Changing IP sequence

	bIPRefreshRequired		= false;

	pfileFirmware			= NULL;
}

/*********************************************************************
  CController::CController()

  Description
  Copy Constructor for CController class. Initilises class 
  data from supplied controller item,

  Parameters
  1 - CController	-	controller to copy
*********************************************************************/

CController::CController(const CController &item)
{
	hFileDescriptor				= item.hFileDescriptor;
	sSerialNumber				= item.sSerialNumber;
	sModel						= item.sModel;
	iaIPAddress					= item.iaIPAddress;
	sComPort					= item.sComPort;
	sFirmware					= item.sFirmware;
	boIsValid					= true;
	iTimeOutCount				= item.iTimeOutCount;
	pLogger						= item.pLogger;

	iControllerType				= item.iControllerType;
	iControllerState			= item.iControllerState;
	ieRequiredProtocol			= item.ieRequiredProtocol;

	iMainState					= item.iMainState;
	iMsgState					= item.iMsgState;
	iLastCMDMsgState			= item.iLastCMDMsgState;

	pDataSocket					= item.pDataSocket;
	strLastMessageSent			= item.strLastMessageSent;
	strLastCMDMessageReceived	= item.strLastCMDMessageReceived;
	bLastMessageReceivedInError = item.bLastMessageReceivedInError;
	bLastMessageBroadcast		= item.bLastMessageBroadcast;

	strLocalHostIP				= item.strLocalHostIP;

	uiTimerIdentMain			= item.uiTimerIdentMain;
	uiTimerIdentDataClose		= item.uiTimerIdentDataClose;
	uiTimerIdentSerialComms		= item.uiTimerIdentSerialComms;

	iChainCommandNumber			= item.iChainCommandNumber;
	iChainNextIndex				= item.iChainNextIndex;

	bUpgradeFirmwareActive			= item.bUpgradeFirmwareActive;
	bIPChangeActive					= item.bIPChangeActive;
	bLastFirmwareUpgradeSuccessful	= item.bLastFirmwareUpgradeSuccessful;
	bLastIPChangeSuccessful			= item.bLastIPChangeSuccessful;

	bIPRefreshRequired			= item.bIPRefreshRequired;

	pfileFirmware				= item.pfileFirmware;
}
	
/*********************************************************************
  CController::CController()

  Description
  Assignment operator for CController class. Sets class 
  data from operand,

  Parameters
  None
*********************************************************************/

CController &CController::operator=(const CController& item)
{
	hFileDescriptor				= item.hFileDescriptor;
	sSerialNumber				= item.sSerialNumber;
	sModel						= item.sModel;
	iaIPAddress					= item.iaIPAddress;
	sComPort					= item.sComPort;
	sFirmware					= item.sFirmware;
	boIsValid					= true;
	iTimeOutCount				= item.iTimeOutCount;
	pLogger						= item.pLogger;

	iControllerType				= item.iControllerType;
	iControllerState			= item.iControllerState;
	ieRequiredProtocol			= item.ieRequiredProtocol;

	iMainState					= item.iMainState;
	iMsgState					= MSG_STATUS_IDLE;
	iLastCMDMsgState			= MSG_STATUS_IDLE;

	pDataSocket					= item.pDataSocket;
	strLastMessageSent			= item.strLastMessageSent;
	strLastCMDMessageReceived	= item.strLastCMDMessageReceived;
	bLastMessageReceivedInError = item.bLastMessageReceivedInError;
	bLastMessageBroadcast		= item.bLastMessageBroadcast;

	strLocalHostIP				= item.strLocalHostIP;
	
	uiTimerIdentMain			= item.uiTimerIdentMain;
	uiTimerIdentDataClose		= item.uiTimerIdentDataClose;
	uiTimerIdentSerialComms		= item.uiTimerIdentSerialComms;

	iChainCommandNumber			= item.iChainCommandNumber;
	iChainNextIndex				= item.iChainNextIndex;

	bUpgradeFirmwareActive			= item.bUpgradeFirmwareActive;
	bIPChangeActive					= item.bIPChangeActive;
	bLastFirmwareUpgradeSuccessful	= item.bLastFirmwareUpgradeSuccessful;
	bLastIPChangeSuccessful			= item.bLastIPChangeSuccessful;

	bIPRefreshRequired			= item.bIPRefreshRequired;

	pfileFirmware				= item.pfileFirmware;

	return *this;
}

/*********************************************************************
  CController::~CController()

  Description
  Simple destructor for CController class. Destroys any residual
  objects.

  Parameters
  None
*********************************************************************/

CController::~CController()
{
/*
MUST NOT CLOSE HERE AS WE DELETE TEMPORARY SERIAL CONTROLLER OBJECTS !!!

  if (hFileDescriptor != NULL)
		CloseHandle(hFileDescriptor);
*/

	if 	(pfileFirmware != NULL)
	{
		if (pfileFirmware->is_open())
		{
			pfileFirmware->close();
		}
		delete pfileFirmware;
		pfileFirmware = NULL;
	}

	if (pDataSocket != NULL)
	{
		delete pDataSocket;
		pDataSocket = NULL;
	}
}

/*********************************************************************
  Routine
  CController::initialise

  Description
  Initialise this controller. This routine initialises this controller
  using the supplied string which is expected to be in the format as
  returned by a controller in response to the search request.
  
  Parameters
  1 - CString& csSearchResponse	- string to initialise controller from.

  Returns - void
*********************************************************************/
// CController member functions

void CController::initialise(CString& csSearchResponse)
{
	CStringArray* pacsTokens;
	CString sWorkBuffer;
	CString strMessage;

	try
	{
		pacsTokens = ParseCString(csSearchResponse, ",");
	
		if (pacsTokens->GetSize() != 5)
		{
			strMessage = "Invalid Number of Tokens Received - " + csSearchResponse;
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		}
		else
		{
			sModel					= pacsTokens->GetAt(1); 
			sSerialNumber			= pacsTokens->GetAt(2);
			sFirmware				= pacsTokens->GetAt(3);
			iaIPAddress.S_un.S_addr = ntohl(FromAsciiHex(pacsTokens->GetAt(4)));
		}
		delete pacsTokens;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - initialise");
		throw;
	}
}

/*********************************************************************
  Routine
  CController::getProtocol

  Description
  Get protocol. This routine returns the currently set protocol.
  
  Parameters
  None

  Returns - CString			"TCP" if TCP
							"UDP" if UDP
							"UNKNOWN" if neither
*********************************************************************/

CString CController::getProtocol()
{
	CString strProto = "UNKNOWN";

	switch (ieRequiredProtocol)
	{
	case CAAsyncSocket::AAS_PROTOCOL_TCP:
		strProto = "TCP";
		break;
	case CAAsyncSocket::AAS_PROTOCOL_UDP:
		strProto = "UDP";
		break;
	default:
		break;
	}
	return strProto;
}

/*********************************************************************
  Routine
  CController::setProtocol

  Description
  Set protocol. This routine sets the required protocol.
  
  Parameters
  1 - CString strProto	- required protocol (Must be "TCP" or "UDP").

  Returns - bool	- true if successful otherwise false
*********************************************************************/

bool CController::setProtocol(CString strProto)
{
	if (strProto == "TCP")
	{
		LogMessage(CLogger::LGR_SEVERITY_DEBUG,"TCP Protocol Requested");
		ieRequiredProtocol = CAAsyncSocket::AAS_PROTOCOL_TCP;

		///Udupa; 13Sep2012
		CheckSocketConnected();
	}
	else if (strProto == "UDP")
	{
		LogMessage(CLogger::LGR_SEVERITY_DEBUG,"UDP Protocol Requested");
		ieRequiredProtocol = CAAsyncSocket::AAS_PROTOCOL_UDP;
	}
	else
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error - Invalid Protocol Requested");

	return true;
}

/*********************************************************************
  Routine
  CController::setLoggerObject

  Description
  Set pointer to logger object. This routine sets a pointer to
  a CLogger object. The logger object should be instantiated by
  the code which instantiates this class. It should be derived from
  (or simply) the supplied CLogger class.

  Parameters
  1 - CLogger* cLog	- pointer to Clogger object.

  Returns - void
*********************************************************************/

void CController::setLoggerObject(CLogger* cLog)
{
	pLogger = cLog;	// pointer to logger object
}

/*********************************************************************
  Routine
  CController::getFileDescriptor

  Description
  Get File Descriptor of Controller.
  
  Parameters
  None

  Returns - HANDLE -	File Descriptor Handle
*********************************************************************/

HANDLE CController::getFileDescriptor()
{
	return hFileDescriptor;
}

/*********************************************************************
  Routine
  CController::setFileDescriptor

  Description
  Set File Descriptor of Controller.
  
  Parameters
  1 - HANDLE hFD	- File Descriptor of this controller

  Returns - Void
*********************************************************************/
void CController::setFileDescriptor(HANDLE hFD)
{
	hFileDescriptor = hFD;
}

CString		getLocalHostIPAddress();
void		setLocalHostIPAddress(CString sIP);

/*********************************************************************
  Routine
  CController::getSerialNumber

  Description
  Get Serial Number of Controller.
  
  Parameters
  None

  Returns - CString -	Ascii Serial Number
*********************************************************************/

CString CController::getSerialNumber()
{
	return sSerialNumber;
}

/*********************************************************************
  Routine
  CController::setSerialNumber

  Description
  Set Serial Number of Controller.
  
  Parameters
  1 - CString sSN	- ascii serial number of this controller

  Returns - Void
*********************************************************************/

void CController::setSerialNumber(CString sSN)
{
	sSerialNumber = sSN;
}

/*********************************************************************
  Routine
  CController::getModel

  Description
  Get Model of Controller.
  
  Parameters
  None

  Returns - CString -	Ascii Model
*********************************************************************/

CString CController::getModel()
{
	return sModel;
}

/*********************************************************************
  Routine
  CController::setModel

  Description
  Set Model of Controller.
  
  Parameters
  1 - CString sM	- ascii model of this controller

  Returns - Void
*********************************************************************/

void CController::setModel(CString sM)
{
	sModel = sM; 
}

/*********************************************************************
  Routine
  CController::getComPort

  Description
  Get Com Port of Controller.
  
  Parameters
  None

  Returns - CString -	Ascii Com Port
*********************************************************************/

CString CController::getComPort()
{
	return sComPort;
}

/*********************************************************************
  Routine
  CController::setComPort

  Description
  Set Com Port of Controller.
  
  Parameters
  1 - CString sCP	- ascii Com Port of this controller

  Returns - Void
*********************************************************************/

void CController::setComPort(CString sCP)
{
	sComPort = sCP;
}

/*********************************************************************
  Routine
  CController::getFirmware

  Description
  Get Firmware Number of Controller.
  
  Parameters
  None

  Returns - CString -	Ascii Firmware
*********************************************************************/

CString CController::getFirmware()
{
	return sFirmware;
}

/*********************************************************************
  Routine
  CController::setFirmware

  Description
  Set Firmware of Controller.
  
  Parameters
  1 - CString sF	- ascii firmware of this controller

  Returns - Void
*********************************************************************/

void CController::setFirmware(CString sF)
{
	sFirmware = sF;
}

/*********************************************************************
  Routine
  CController::getLocalHostIPAddress

  Description
  Get Local Host IP Address of Controller.
  
  Parameters
  None

  Returns - CString -	Ascii IP Address
*********************************************************************/

CString CController::getLocalHostIPAddress()
{
	return strLocalHostIP;
}

/*********************************************************************
  Routine
  CController::setLocalHostIPAddress

  Description
  Set Local Host IP Address of Controller.
  
  Parameters
  1 - CString sIP	- ascii local host IP address for this controller

  Returns - Void
*********************************************************************/

void CController::setLocalHostIPAddress(CString sIP)
{
	strLocalHostIP = sIP;
}

/*********************************************************************
  Routine
  CController::getIPAddress

  Description
  Get IP Address of Controller.
  
  Parameters
  None

  Returns - in_addr -	in_addr IP Address
*********************************************************************/

in_addr CController::getIPAddress()
{
	return iaIPAddress;
}

/*********************************************************************
  Routine
  CController::setIPAddress

  Description
  Set IP Address of Controller.
  
  Parameters
  1 - DWORD dA	- in_addr formatted IP Address of this controller

  Returns - Void
*********************************************************************/

void CController::setIPAddress(DWORD dA)
{
	iaIPAddress.S_un.S_addr = dA;
}

/*********************************************************************
  Routine
  CController::setIPAddress

  Description
  Set IP Address of Controller.
  
  Parameters
  1 - DWORD dA	- in_addr formatted IP Address of this controller
  1 - char ba1	- 1st byte value of address
  2 - char ba2	- 2nd byte value of address
  3 - char ba3	- 3rd byte value of address
  4 - char ba4	- 4th byte value of address

  Returns - Void
*********************************************************************/

void CController::setIPAddress(char ba1, char ba2, char ba3, char ba4)
{
	iaIPAddress.S_un.S_un_b.s_b1 = ba1;
	iaIPAddress.S_un.S_un_b.s_b2 = ba2;
	iaIPAddress.S_un.S_un_b.s_b3 = ba3;
	iaIPAddress.S_un.S_un_b.s_b4 = ba4;
}

/*********************************************************************
  Routine
  CController::getTimeOutCount

  Description
  Get TimeOut Count of Controller.
  
  Parameters
  None

  Returns - int -	TimeOut Count
*********************************************************************/

int CController::getTimeOutCount()
{
	return iTimeOutCount;
}

/*********************************************************************
  Routine
  CController::setTimeOutCount

  Description
  Set TimeOut Count of Controller.
  
  Parameters
  1 - int iTOC	- int TimeOut Count of this controller

  Returns - Void
*********************************************************************/

void CController::setTimeOutCount(int iTOC)
{
	iTimeOutCount = iTOC;
}

/*********************************************************************
  Routine
  CController::decTimeOutCount

  Description
  Decrements TimeOut Count of Controller.
  
  Parameters
  None

  Returns - int -	New TimeOut Count
*********************************************************************/

int CController::decTimeOutCount()
{
	return --iTimeOutCount;
}

/*********************************************************************
  Routine
  CController::getControllerType

  Description
  Get Type of Controller.
  
  Parameters
  None

  Returns - int -	int CTR_TYPE
*********************************************************************/

int CController::getControllerType()
{
	return iControllerType;
}

/*********************************************************************
  Routine
  CController::setControllerType

  Description
  Set Type of Controller.
  
  Parameters
  1 - int iCT	- enum CTR_TYPE

  Returns - Void
*********************************************************************/

void CController::setControllerType(enum CController::CTR_TYPE iCT)
{
	iControllerType = iCT;
}

/*********************************************************************
  Routine
  CController::getFirmwareFileName

  Description
  Get Current Firmware Filename.
  
  Parameters
  None

  Returns - CString -	current firmware filename
*********************************************************************/

CString CController::getFirmwareFileName()
{
	return this->strFirmwareFileName;
}


/*********************************************************************
  Routine
  CController::setFirmwareFileName

  Description
  Set Firmware Filename.
  
  Parameters
  1 - CString &strFFileName	- File Name for Firmware Update

  Returns - Void
*********************************************************************/

void CController::setFirmwareFileName(const CString &strFFileName)
{
	CString strMessage;
	strFirmwareFileName = strFFileName;
	strMessage.Format("Firmware File Name Now - %s", strFirmwareFileName);
	LogMessage(CLogger::LGR_SEVERITY_INFO,strMessage);
}

/*********************************************************************
  Routine
  CController::getControllerState

  Description
  Get State of Controller.
  
  Parameters
  None

  Returns - int -	int CTR_STATE
*********************************************************************/

int CController::getControllerState()
{
	return iControllerState;
}

/*********************************************************************
  Routine
  CController::setControllerState

  Description
  Set State of Controller.
  
  Parameters
  1 - int iCT	- enum CTR_STATE

  Returns - Void
*********************************************************************/

void CController::setControllerState(enum CController::CTR_STATE iCS)
{
	try
	{
		iControllerState = iCS;

		KillTimer(&uiTimerIdentSerialComms);	// kill any old timers

		switch (iCS)
		{
		case CTR_STATE_OPEN:			// serial controller now connected and FD opened
			// kick off timer
			settimer(CNW_COM_PORTS_SERVICE_TIME, &uiTimerIdentSerialComms);	// try serial comms again in....
			break;

		case CTR_STATE_NOTINIT:
		case CTR_STATE_REFRESH_SENT:	// refresh message "ES" sent to serial 
										//	controller - awaiting response
		case CTR_STATE_VR_SENT:			// refresh message "VR" sent to serial 
										//	controller - awaiting response
		case CTR_STATE_INIT:			// serial controller now initialised
		default:
			;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_INFO,"CController::setControllerState - Error Detected");
	}
}

/*********************************************************************
  Routine
  CController::getControllerStatusValues

  Description
  Get Status values for Controller.
  
  Parameters
  1 - CString	&strMainState			current main state description
  2 - CString	&strSubState			current sub state description
  3 - CString	&strUpdateFileMessage	current file transfer message
  4 - int		&iPercentageComplete	current % complete for file transfer

  Returns - bool -	True if Successful, otherwise False
*********************************************************************/

bool CController::getControllerStatusValues(CString &strMainState,
											CString &strSubState,
											CString &strUpdateFileMessage,
											int &iPercentageComplete,
											bool &bComplexCmdActive)
{
	try
	{
		strMainState = "";
		strSubState = "";
		strUpdateFileMessage = "";
		iPercentageComplete = 0;

		bComplexCmdActive = (bUpgradeFirmwareActive || bIPChangeActive);

		switch (iMainState)
		{
		case MSM_MAINSTATE_IDLE:				// idle state
			strMainState = "Main State Idle";
			break;
		case MSM_MAINSTATE_SEND_CMD_DATA:		// sent command message
			strMainState = "Main State Command Send";
			switch (iMsgState)
			{
			case MSG_STATUS_IDLE:			// idle state
				strSubState = "Command Message State Idle";
				break;
			case MSG_STATUS_SENT:			// message sent state
				strSubState = "Command Message State Sent";
				break;
			case MSG_STATUS_SENT_TIMING:	// message sent timeout running state
				strSubState = "Command Message State Sent with Timeout";
				break;
			case MSG_STATUS_SENT_TIMEDOUT:	// message sent but timeout before response state
				strSubState = "Command Message State Timed Out";
				break;
			case MSG_STATUS_RECEIVED:		// message received state
				strSubState = "Command Message State Received";
				break;
			default:
				;
			}
			break;
		case MSM_MAINSTATE_BOOT_REQ:			// sent boot command
			strMainState = "Main State Boot Request Sent";
			break;
		case MSM_MAINSTATE_BOOT_PRE_REFRESH:	// boot PRE refresh delay
			strMainState = "Main State Boot Pre-Refresh Delay";
			break;
		case MSM_MAINSTATE_BOOT_IP_REFRESH:		// boot IP refresh
			strMainState = "Main State Boot Awaiting Controller IP Refresh";
			break;
		case MSM_MAINSTATE_BOOT_SOCKET_WAIT:	// delay to allow socket to close
			strMainState = "Main State Boot Awaiting Socket Closure";
			break;
		case MSM_MAINSTATE_BOOT_MODE:			// in boot mode
			strMainState = "Main State Boot Mode";
			break;
		case MSM_MAINSTATE_APP_VALID_REQ:		// sent app valid message
			strMainState = "Main State App Valid Request Sent";
			break;
		case MSM_MAINSTATE_RUN_APP_REQ:			// sent run app message
			strMainState = "Main State Run App Request Sent";
			break;
		case MSM_MAINSTATE_RUN_APP_REFRESH_DELAY:// run app refresh delay
			strMainState = "Main State Run App Pre-Refresh Delay";
			break;
		case MSM_MAINSTATE_RUN_APP_IP_REFRESH:	// run app IP refresh
			strMainState = "Main State Run App Awaiting Controller IP Refresh";
			break;
		case MSM_MAINSTATE_ERASE_REQ:			// sent erase message
			strMainState = "Main State Erase App Request Sent";
			break;
		case MSM_MAINSTATE_LOAD_APP_REQ:		// sent load request message
			strMainState = "Main State Load App Request Sent";
			break;
		case MSM_MAINSTATE_LOAD_APP_DATA:		// sent load data message
			strMainState = "Main State Loading App";
			strUpdateFileMessage.Format("Transferred %u bytes of %u", dwFirmwareFileBytesProcessed, dwFirmwareFileLength);
			iPercentageComplete = 0;
			if (dwFirmwareFileLength != 0)
			{
				iPercentageComplete = (int)((float)dwFirmwareFileBytesProcessed/(float)dwFirmwareFileLength * 100.0);
			}
			break;
		case MSM_MAINSTATE_CHECKSUM_REQ:		// sent get checksum message
			strMainState = "Main State Checksum Request Sent";
			break;
		case MSM_MAINSTATE_SEND_IP_CMD:			// sent IP command message
			strMainState = "Main State IP Change Request Sent";
			switch (iSubState)
			{
			case MSM_SUBSTATE_IDLE:					// idle state
				strSubState = "Sub State Idle";
				break;
			case MSM_SUBSTATE_SEND_IP_DYNAMIC_CMD:	// Sent IP Dynamic Command state
				strSubState = "Sub State Sent DHCP Command";
				break;
			case MSM_SUBSTATE_SEND_IP_EXECUTE_CMD:	// Sent IP Execute Command state
				strSubState = "Sub State Sent Execute Command";
				break;
			case MSM_SUBSTATE_SEND_IP_FIXED_CMD:		// Sent IP Fixed Command state
				strSubState = "Sub State Sent Fixed IP Command";
				break;
			case MSM_SUBSTATE_SEND_IP_ADDRESS:		// Sent IP Address state
				strSubState = "Sub State Sent IP Address";
				break;
			case MSM_SUBSTATE_SEND_IP_MASK:			// Sent IP Mask state
				strSubState = "Sub State Sent IP Mask";
				break;
			case MSM_SUBSTATE_SEND_IP_GATEWAY:		// Sent IP Gateway state
				strSubState = "Sub State Sent Gateway";
				break;
			case MSM_SUBSTATE_ALLOW_IP_REFRESH:		// Simple Timeout to allow update of ip
				strSubState = "Sub State Awaiting IP Refresh";
				break;
			case MSM_SUBSTATE_IP_FIXED_SOCKET_WAIT:	// delay to allow socket to close fixed ip
			case MSM_SUBSTATE_IP_DYNAMIC_SOCKET_WAIT:// delay to allow socket to close dynamic ip
				strSubState = "Sub State Awaiting Socket Closure";
				break;
			case MSM_SUBSTATE_IP_PRE_REFRESH:			// IP pre refresh delay
				strSubState = "Sub State Pre-Refresh Delay";
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		return true;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_INFO,"CController::getControllerStatusValues - Error Detected");
		return false;
	}
}

/*********************************************************************
  Routine
  CController::getControllerCMDStatusValues

  Description
  Get Message values for last SEND COMMAND for this Controller.
  
  Parameters
  1 - CString	&strMsgState				- current message state description
  2 - CString	&strLastCMDMsgSent			- last user Command message sent
  3 - CString	&strLastCMDMsgReceived		- last message received from user Command
  4 - bool		bErrorInLastMessageReceived	- last received had errors or not

  Returns - bool -	True if Successful, otherwise False
*********************************************************************/

bool CController::getControllerCMDStatusValues(CString &strMsgState,
											   CString &strLastCMDMsgSent, CString &strLastCMDMsgReceived,
											   bool &bErrorInLastMessageReceived)
{
	try
	{
		switch (iLastCMDMsgState)
		{
		case MSG_STATUS_IDLE:			// idle state
			strMsgState = "Command Message State Idle";
			break;
		case MSG_STATUS_SENT:			// message sent state
			strMsgState = "Command Message State Sent";
			break;
		case MSG_STATUS_SENT_TIMING:	// message sent timeout running state
			strMsgState = "Command Message State Sent with Timeout";
			break;
		case MSG_STATUS_SENT_TIMEDOUT:	// message sent but timeout before response state
			strMsgState = "Command Message State Timed Out";
			break;
		case MSG_STATUS_RECEIVED:		// message received state
			strMsgState = "Command Message State Received";
			break;
		default:
			;
		}
		strMsgState;
		strLastCMDMsgSent = strLastCMDMessageSent;
		strLastCMDMsgReceived = strLastCMDMessageReceived;
		bErrorInLastMessageReceived = bLastMessageReceivedInError;
		return true;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_INFO,"CController::getControllerCMDStatusValues - Error Detected");
		return false;
	}
}

/*********************************************************************
  Routine
  CController::getLastFirmwareUpgradeSuccessful

  Description
  Get Result for Last Firmware Upgrade Operation for this Controller.
  
  Parameters
  none

  Returns - bool -	Result Flag
*********************************************************************/

bool CController::getLastFirmwareUpgradeSuccessful()
{
	return bLastFirmwareUpgradeSuccessful;
}

/*********************************************************************
  Routine
  CController::getLastIPChangeSuccessful

  Description
  Get Result for Last IP Change Operation for this Controller.
  
  Parameters
  none

  Returns - bool -	Result Flag
*********************************************************************/

bool CController::getLastIPChangeSuccessful()
{
	return bLastIPChangeSuccessful;
}

/*********************************************************************
  Routine
  CController::getInputBuffer

  Description
  Get Current Input Buffer of Controller.
  
  Parameters
  None

  Returns - CString -	input buffer
*********************************************************************/

CString CController::getInputBuffer()
{
	return strInBuffer;
}

/*********************************************************************
  Routine
  CController::setInputBuffer

  Description
  Set Input Buffer of Controller.
  
  Parameters
  1 - CString sIB	- string for input buffer this controller

  Returns - Void
*********************************************************************/

void CController::setInputBuffer(CString sIB)
{
	strInBuffer = sIB;
}

/*********************************************************************
  Routine
  CController::appendInputBuffer

  Description
  Append to Input Buffer of Controller.
  
  Parameters
  1 - CString sIB	- string to append to input buffer of this 
						controller

  Returns - Void
*********************************************************************/

void CController::appendInputBuffer(const char * sIB)
{
	strInBuffer = strInBuffer + sIB ;
}

/*********************************************************************
  Routine
  CController::LogMessage

  Description
  Log Message and severity. Note the implementation of AddLogMessage
  may be unknown.
  
  Parameters
  1 - CLogger::LGR_SEVERITY iSeverity	- (see enum LGR_SEVERITY)
  2 - char * strLogMsg					- ascii message to log

  Returns - void
*********************************************************************/

void CController::LogMessage(CLogger::LGR_SEVERITY iSeverity, char *strLogMsg)
{
	CString strMessage;
	strMessage.Format("%7s - %s", sSerialNumber, strLogMsg);
	if (pLogger != NULL)
		pLogger->AddLogMessage(iSeverity, strMessage);
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

void CController::LogMessage(CLogger::LGR_SEVERITY iSeverity, CString &strLogMessage)
{
	CString strMessage;
	strMessage.Format("%7s - %s", sSerialNumber, strLogMessage);
	if (pLogger != NULL)
		pLogger->AddLogMessage(iSeverity, strMessage);
}

/*********************************************************************
  Routine
  CController::ParseCString

  Description
  Parse a string and return a set of tokens.
  
  Parameters
  1 - CString &csParseString	- string to parse into tokens.
  2 - CString csDelimiter		- delimiter for tokens.

  Returns - CStringArray * - array of fields parse
*********************************************************************/

CStringArray * CController::ParseCString(CString &csParseString, CString csDelimiter)
{
	int iReturn;

	try
	{
		CStringArray* pacsTokens = new CStringArray;
		pacsTokens->SetSize(0);
	
		TCHAR* sText = new char[csParseString.GetLength()+1];
		_tcscpy(sText,csParseString);
		TCHAR* sToken;

		/* Establish string and get the first token: */
		sToken = strtok( sText, "," );

		/* While there are tokens in "string" */
		while( sToken != NULL )
		{
			iReturn = (int) pacsTokens->Add(sToken);

			/* Get next token: */
			sToken = strtok( NULL, "," );
		}

		delete sText;
	
		return pacsTokens;
	}
	catch (...)
	{
		throw;
	}
}

/*********************************************************************
  Routine
  CController::FromAsciiHex

  Description
  Convert an ascii string of 8 characters to double word, most 
  significant.nibble first. Used to convert the Hex IP address to DWORD
  format.
  
  Parameters
  1 - CString &csParseString	- ascii hex string.

  Returns - DWORD	- IP Address in in_addr format.
*********************************************************************/

DWORD CController::FromAsciiHex(const CString csParseString)
{
	DWORD dHexValue;

	dHexValue = 0;

	int iPosition = 0;
	char bNibble;

	try
	{
		while ((iPosition < csParseString.GetLength()) && (iPosition < 8))
		{
			bNibble = csParseString.GetAt(iPosition);
			if (bNibble > 0x29 && bNibble < 0x40) // if 0 to 9 
				dHexValue = (dHexValue << 4) + (bNibble & 0x0F);
			else if (bNibble >= 'a' && bNibble <= 'f') // if a to f
				dHexValue = (dHexValue << 4) + (bNibble & 0x0F) + 9;
			else if (bNibble >= 'A' && bNibble <= 'F') // if A to F
				dHexValue = (dHexValue << 4) + (bNibble & 0x0F) + 9;
			else 
				break;
			iPosition++;
		}

		return dHexValue;
	}
	catch (...)
	{
		throw;
	}
}

/*********************************************************************
  Routine
  CController::OnSocketReceive

  Description
  This is a callback routine used by the CAAsnchSocket Class to return 
  messages which have arrived. This object must be registered with the
  CAAsnchSocket object for this mechanism to work. (and deregistered 
  finished with). Also the parameters must remain exactly as they are.
  This routine will process the message according to whether it is an
  identification message, or a data message. If it is an identification
  message from a controller then this routine will handle adding or 
  modifying the controller. If it is a data message then the data 
  message handling routine will be invoked.
  
  Parameters
  1 - CString& strBuff		- message received.
  2 - CString& sIPAddress	- ip address of sender
  3 - UINT& iPort			- port of sender

  Returns - void
*********************************************************************/

void CController::OnSocketReceive(CString& strBuff, CString& sIPAddress, UINT& iPort)
{
	CString strMessage;

	try
	{
		// data interface
		strMessage = "Comms Host Data Port Message - " + strBuff;
		LogMessage(CLogger::LGR_SEVERITY_DEBUG,strMessage);
		ProcessDataMessage(strBuff, sIPAddress, iPort);
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_INFO,"CController::OnSocketReceive - Error Detected");
	}
}	

/*********************************************************************
  Routine
  CController::DisconnectFromController

  Description
  Client Request - Disconnect From Controller.
  
  Parameters
  None.

  Returns - bool -	True if Successful, otherwise False
*********************************************************************/

bool CController::DisconnectFromController()
{
	CString			strMessage;

	try
	{
		KillTimer(&uiTimerIdentMain);		// kill any old timers

		iMainState = MSM_MAINSTATE_IDLE;	// reset state machine

		// connect depending on controller type
		switch (iControllerType)
		{
		case CMS_COMMSTYPE_ETHERNET:
			// close down data socket if present
			this->CloseOrRemoveSocket(pDataSocket, &uiTimerIdentDataClose, true);
			break;

		case CMS_COMMSTYPE_SERIAL:
			// check for connected already
			if (hFileDescriptor != INVALID_HANDLE_VALUE)
			{
				// close file as no longer in dialog
				if (!CloseHandle(hFileDescriptor))	// close file
				{
					strMessage.Format("CControllerNW::DisconnectFromController() - Error - %i on %s", GetLastError(), sComPort);
					LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
				}

				// set FD to invalid as no longer open
				hFileDescriptor = INVALID_HANDLE_VALUE;	// save FD handle

				// set controller state to INIT...
				iControllerState = CTR_STATE_INIT;		// initialised
			}
			break;
		default:
			;
		}

		return true;
	}
	catch (...)
	{
		// failed
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::DisconnectFromController() - Error Detected");
		return false;
	}
}

/*********************************************************************
  Routine
  CController::ConfigureUDPSocket

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
bool CController::ConfigureUDPSocket(CAAsyncSocket* &rpSocket, UINT nLocalSocketPort, 
										LPCTSTR lpszLocalSocketAddress, 
										boolean bAllowedToDelete)
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
		case CAAsyncSocket::AAS_PROTOCOL_TCP:
			// can't use the TCP socket so handle depending on Socket state
			// handle according to Socket State
			switch (rpSocket->getState())
			{
			case CAAsyncSocket::AAS_STATE_ACTIVE:
				if (!rpSocket->ShutDown(2))
				{
					iReturn = rpSocket->GetLastError();
					if (iReturn != WSAEWOULDBLOCK)
					{
						strMessage.Format("Connect Error - %i", iReturn);
						throw strMessage;
					}
				}
				rpSocket->setState(CAAsyncSocket::AAS_STATE_CLOSE_REQUESTED);
				settimer(CNW_SOCKET_CLOSURE_TIME, &uiTimerIdentDataClose);	// timeout for closure
				LogMessage(CLogger::LGR_SEVERITY_INFO,"UDP Configure Requested on Active TCP Socket - Awaiting Closure");
				return false;
			case CAAsyncSocket::AAS_STATE_CLOSE_REQUESTED:
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Close Already Requested on This Socket - Awaiting Closure");
				return false;
			case CAAsyncSocket::AAS_STATE_NOTINIT:
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Socket Should Not Be In AAS_STATE_NOTINIT State");
				// allow to fall through and treat as closed socket
			case CAAsyncSocket::AAS_STATE_CLOSED:
				KillTimer(&uiTimerIdentDataClose);
				// check allowed to delete
				if (bAllowedToDelete)
				{
					// regenerate the socket
					delete rpSocket;
					rpSocket = new(CAAsyncSocket);
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Old Socket Destroyed New instance of Socket Created");
					break; // and go through to UDP configure
				}
				else
				{
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Not Allowed To Delete Socket From Here");
					return false;
				}
			default:
				strMessage.Format("Socket With State Value - %i", rpSocket->getState()); 
				throw strMessage;
			}

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
		if (!rpSocket->Bind(0,lpszLocalSocketAddress))
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
  CController::ConfigureTCPSocket

  Description
  Set up or recreate a TCP socket. Will attempt to reuse a socket if
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
bool CController::ConfigureTCPSocket(CAAsyncSocket* &rpSocket, UINT nLocalSocketPort, 
										LPCTSTR lpszLocalSocketAddress, 
										boolean bAllowedToDelete)
{
	int iReturn;
	CString strMessage;
	bool bOptval;
	CString strHostAddress;

	try
	{
		// must have a socket for the next bit
		if (rpSocket == NULL)
			rpSocket = new(CAAsyncSocket);
		
		// handle according to protocol of socket
		switch (rpSocket->getProtocol())
		{
		case CAAsyncSocket::AAS_PROTOCOL_TCP:
			// handle according to Socket State
			switch (rpSocket->getState())
			{
			case CAAsyncSocket::AAS_STATE_ACTIVE:
				// if we're connected already
				strHostAddress = inet_ntoa(iaIPAddress);
				if ((rpSocket->getIPAddress() == strHostAddress) && (rpSocket->getPort() == CMS_COMMSPORT_HOST_DATA))
				{
					LogMessage(CLogger::LGR_SEVERITY_INFO,"ConfigureTCPSocket - Reusing Socket - Already Set up as TCP");
					return true;
				}
				else
				{
					if (!rpSocket->ShutDown(2))
					{
						iReturn = rpSocket->GetLastError();
						if (iReturn != WSAEWOULDBLOCK)
						{
							strMessage.Format("Connect Error - %i", iReturn);
							throw strMessage;
						}
					}
					rpSocket->setState(CAAsyncSocket::AAS_STATE_CLOSE_REQUESTED);
					settimer(CNW_SOCKET_CLOSURE_TIME, &uiTimerIdentDataClose);	// timeout for closure
					LogMessage(CLogger::LGR_SEVERITY_INFO,"TCP Configure Requested on Active TCP Socket - Awaiting Closure");
					return false;
					}
				case CAAsyncSocket::AAS_STATE_CLOSE_REQUESTED:
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Close Already Requested on This Socket - Awaiting Closure");
					return false;
				case CAAsyncSocket::AAS_STATE_NOTINIT:
					LogMessage(CLogger::LGR_SEVERITY_ERROR,"Socket Should Not Be In AAS_STATE_NOTINIT State");
					// allow to fall through and treat as closed socket
				case CAAsyncSocket::AAS_STATE_CLOSED:
					KillTimer(&uiTimerIdentDataClose);
					// check allowed to delete
					if (bAllowedToDelete)
					{
						// regenerate the socket
						delete rpSocket;
						rpSocket = new(CAAsyncSocket);
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Old Socket Destroyed New instance of Socket Created");
						break;
					}
					else
					{
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Not Allowed To Delete Socket From Here");
						return false;
					}
				default:
					strMessage.Format("Socket With State Value - %i", rpSocket->getState()); 
					throw strMessage;
				}
			// if got to here then we can go configure TCP Socket
			break;

		case CAAsyncSocket::AAS_PROTOCOL_NOTINIT:
			break; // new socket so will just go configure

		case CAAsyncSocket::AAS_PROTOCOL_UDP:

			KillTimer(&uiTimerIdentDataClose);
			// check allowed to delete
			if (bAllowedToDelete)
			{
				// regenerate the socket
				delete rpSocket;
				rpSocket = new(CAAsyncSocket);
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Old Socket Destroyed New instance of Socket Created");
				break;
			}
			else
			{
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Not Allowed To Delete Socket From Here");
				return false;
			}
		default:
			strMessage.Format("Error Unknown Protocol Setting - %i", rpSocket->getProtocol()); 
			throw strMessage;
		}

		// if got to here then ok for TCP creation
		LogMessage(CLogger::LGR_SEVERITY_DEBUG,"Attempting to set up TCP Socket");

		rpSocket->setLoggerObject(pLogger);
		// initialise ethernet interface
		rpSocket->Register(this);         // register myself for socket events
		rpSocket->setProtocol(CAAsyncSocket::AAS_PROTOCOL_TCP);
	
		// Set up a TCP Socket
		if (!rpSocket->Socket(SOCK_STREAM, FD_READ|FD_CONNECT|FD_CLOSE))
		{
			iReturn = rpSocket->GetLastError();
			strMessage.Format("Socket Error - %i", iReturn);
			throw strMessage;
		}

		// set up as re use so we can reattach
		bOptval = true;
		if (!rpSocket->SetSockOpt(SO_REUSEADDR , &bOptval, sizeof(bOptval), SOL_SOCKET))
		{
			iReturn = rpSocket->GetLastError();
			strMessage.Format("SetSockOpt SO_REUSEADDR Error - %i", iReturn);
			throw strMessage;
		}

		// allow set no delay to ensure no buffering up and hanging around
		bOptval = true;
		if (!rpSocket->SetSockOpt(TCP_NODELAY , &bOptval, sizeof(bOptval),IPPROTO_TCP))
		{
			iReturn = rpSocket->GetLastError();
			strMessage.Format("SetSockOpt TCP_NODELAY Error - %i", iReturn);
			throw strMessage;
		}

		// allow keep alive if there
		bOptval = false;
		if (!rpSocket->SetSockOpt(SO_KEEPALIVE , &bOptval, sizeof(bOptval),SOL_SOCKET))
		{
			iReturn = rpSocket->GetLastError();
			strMessage.Format("SetSockOpt SO_KEEPALIVE Error - %i", iReturn);
			throw strMessage;
		}

		// don't wait for any more on close
		bOptval = true;
		if (!rpSocket->SetSockOpt(SO_DONTLINGER , &bOptval, sizeof(bOptval),SOL_SOCKET))
		{
			iReturn = rpSocket->GetLastError();
			strMessage.Format("SetSockOpt SO_DONTLINGER Error - %i", iReturn);
			throw strMessage;
		}

		// finally connect the thing
		strHostAddress = inet_ntoa(iaIPAddress);
		rpSocket->setIPAddress(strHostAddress);
		rpSocket->setPort(CMS_COMMSPORT_HOST_DATA);

		if (!rpSocket->Connect(strHostAddress,CMS_COMMSPORT_HOST_DATA))
		{
			iReturn = rpSocket->GetLastError();
			if (iReturn != WSAEWOULDBLOCK)
			{
				strMessage.Format("Connect Error - %i", iReturn);
				throw strMessage;
			}
		}
				
		LogMessage(CLogger::LGR_SEVERITY_DEBUG,"TCP Socket Set Up");

		// came through ok
		return true;
	}
	catch (CString strMessage)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - ConfigureTCPSocket");
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
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - ConfigureTCPSocket");
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
  CController::CloseOrRemoveSocket

  Description
  Close or Remove a socket. Will attempt to close a socket if ok to do 
  so. If TCP and not ok will request shutdown and kick off a timer to 
  get rid of once shutdown is completed or timed out. This routine may 
  be called from situations where deletion of the socket should under 
  no circumstances be deleted.
  
  Parameters
  1 - CAAsyncSocket* &rpSocket			- address of socket pointer
  2 - UINT* pidEvent					- pointer for event id of 
											shutdown timer.
  3 - boolean bAllowedToDelete			- true if allowed to delete 
											socket false otherwise

  Returns - bool -	True if Successful, otherwise False
*********************************************************************/

bool CController::CloseOrRemoveSocket(CAAsyncSocket* &rpSocket, UINT_PTR* pidEvent, 
											boolean bAllowedToDelete)
{
	int iReturn;
	CString strMessage;

	try
	{
		// must have a socket for the next bit
		if (rpSocket != NULL)
		{
			// handle according to protocol of socket
			switch (rpSocket->getProtocol())
			{
			case CAAsyncSocket::AAS_PROTOCOL_TCP:
				// handle according to state of socket
				switch (rpSocket->getState())
				{
				case CAAsyncSocket::AAS_STATE_NOTINIT:
				case CAAsyncSocket::AAS_STATE_CLOSED:
					// check allowed to delete
					if (bAllowedToDelete)
					{
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
				case CAAsyncSocket::AAS_STATE_ACTIVE:
					settimer(CNW_SOCKET_CLOSURE_TIME, pidEvent);	// timeout for closure
					if (!rpSocket->ShutDown(2))
					{
						iReturn = rpSocket->GetLastError();
						if (iReturn != WSAEWOULDBLOCK)
						{
							strMessage.Format("Connect Error - %i", iReturn);
							throw strMessage;
						}
					}
					rpSocket->setState(CAAsyncSocket::AAS_STATE_CLOSE_REQUESTED);
					LogMessage(CLogger::LGR_SEVERITY_INFO,"CloseOrRemoveSocket - Socket Close Requested");
					break;
				case CAAsyncSocket::AAS_STATE_CLOSE_REQUESTED:
					LogMessage(CLogger::LGR_SEVERITY_INFO,"CloseOrRemoveSocket - Socket Close Already Requested");
					break;
				default:
					strMessage.Format("Error Socket With Invalid State - %i", rpSocket->getState()); 
					throw strMessage;
				}
				// came through ok
				break;

			case CAAsyncSocket::AAS_PROTOCOL_UDP:
			case CAAsyncSocket::AAS_PROTOCOL_NOTINIT:
			default:
			
				KillTimer(pidEvent);
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
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - CController::CloseOrRemoveSocket");
		return false;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - CController::CloseOrRemoveSocket");
		return false;
	}
}

/*********************************************************************
  Routine
  CController::ProcessTimer

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

void CController::ProcessTimer(	UINT_PTR idEvent, DWORD dwTime)
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
			// if main state timer
			if (idEvent == uiTimerIdentMain)
			{
				uiTimerIdentMain = 0;
				ProcessMainStateTimer(idEvent, dwTime);
			}
			else
			{
				// if Socket Closure Timeout then we can get rid of the offending socket
				// ProcessSocketClosureTimer should clear the timer ident for us.
				if (idEvent == uiTimerIdentDataClose)
					ProcessSocketClosureTimer(idEvent, dwTime);
				else
				{
					strMessage.Format("CController::ProcessTimer() Fired For Unknown Timer - %u", idEvent);
					LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
				}
			}
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::ProcessTimer() - Error Detected");
	}
}

/*********************************************************************
  Routine
  CControllerNW::ProcessMainStateTimer

  Description
  State Machine Service Timer. Handle all timeouts which are used by 
  the main state machine.
  
  Parameters
  1 - UINT idEvent	- timer identifier
  2 - DWORD dwTime	- current system time

  Returns - void.
*********************************************************************/

void CController::ProcessMainStateTimer(UINT_PTR idEvent, DWORD dwTime)
{
	CString strMessage;
	CString strIPMessage;

	try
	{
		switch (iMainState)
		{
		case MSM_MAINSTATE_BOOT_REQ:	// in boot mode requested
			// timed out so see if allowed to retry

			if (uiRetryCount > 0)
			{
				// resend message with 1 less retry
				--uiRetryCount;
				if (SendControllerDataMessage(strLastMessageSent, uiRetryTimeout, uiRetryCount, bLastMessageBroadcast))
					break;
			}
			else
			{
				//no retry allowed
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Retries Exhausted in Boot Mode Request State");
			}
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle State");

			if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
			{
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
				bUpgradeFirmwareActive = false;
				bLastFirmwareUpgradeSuccessful	= false;
			}

			iMainState = MSM_MAINSTATE_IDLE;
			break;
			
		case MSM_MAINSTATE_BOOT_SOCKET_WAIT:	// in boot socketwait
			// force socket to be UDP
			if (!this->ConfigureUDPSocket(pDataSocket, CMS_COMMSPORT_CLIENT_DATA_UDP, strLocalHostIP, true))
			{
				if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
				{
					LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
					bUpgradeFirmwareActive = false;
					bLastFirmwareUpgradeSuccessful	= false;
				}
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error - Boot Mode Failed to Set Data Socket to UDP");
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Idle State");
				iMainState = MSM_MAINSTATE_IDLE;	// in Idle
			}
			else
			{
				// send message
				strMessage = "DV" + sSerialNumber + ",0\r\n";
				if (!SendControllerDataMessage(strMessage, CNW_BOOT_TIMEOUT, CNW_BOOT_MSG_RETRY, true))
				{
					if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
					{
						LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
						bUpgradeFirmwareActive = false;
					}
					LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error - Failed to Send Boot Message");
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Idle State");
					iMainState = MSM_MAINSTATE_IDLE;
				}
				else
				{
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Boot Mode Request State");
					iMainState = MSM_MAINSTATE_BOOT_REQ;
				}
			}
			break;
		case MSM_MAINSTATE_BOOT_IP_REFRESH:	 // in BOOT_IP_REFRESH
			// assume got all needed by now so just move into boot mode
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Boot Mode State");
			iMainState = MSM_MAINSTATE_BOOT_MODE; // in boot mode
			HandleChainedCommand();	// activate next link if chained command
			break;
		case MSM_MAINSTATE_BOOT_PRE_REFRESH: // in PRE_IP_REFRESH delay
			// need to refresh controller IP addresses here due to DHCP dynamic
			bIPRefreshRequired = true;
			settimer(CNW_BOOT_REFRESH_DELAY, &uiTimerIdentMain);	// allow refresh timer to complete
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Boot IP Refresh State");
			iMainState = MSM_MAINSTATE_BOOT_IP_REFRESH; // in BOOT_IP_REFRESH
			break;
		case MSM_MAINSTATE_APP_VALID_REQ:	// in APP VALID REQUEST
			// timed out so see if allowed to retry
			if (uiRetryCount > 0)
			{
				// resend message with 1 less retry
				--uiRetryCount;
				if (SendControllerDataMessage(strLastMessageSent, uiRetryTimeout, uiRetryCount, bLastMessageBroadcast))
					break;
			}
			else
			{
				//no retry allowed
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Timeout in App Valid Request State");
			}
			if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
			{
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
				bUpgradeFirmwareActive = false;
				bLastFirmwareUpgradeSuccessful	= false;
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle Mode State");
				iMainState = MSM_MAINSTATE_IDLE; // in idle mode
			}
			else // come from boot mode
			{
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Boot Mode State");
				iMainState = MSM_MAINSTATE_BOOT_MODE; // in boot mode
			}
			break;
		case MSM_MAINSTATE_RUN_APP_REQ:		// in RUN_APP_REQ delay
			// timed out so see if allowed to retry
			if (uiRetryCount > 0)
			{
				// resend message with 1 less retry
				if (iControllerType == CTR_TYPE_ETHERNET)
				{
					if (SendControllerDataMessage(strLastMessageSent, 0, 0, bLastMessageBroadcast))
					{
						settimer(CNW_RUN_PRE_REFRESH_DELAY, &uiTimerIdentMain);	// time to delay before refresh
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Run App Refresh Delay State");
						iMainState = MSM_MAINSTATE_RUN_APP_REFRESH_DELAY; // run app refresh delay
						break;
					}
				}
				else
				{
					--uiRetryCount;
					if (SendControllerDataMessage(strLastMessageSent, uiRetryTimeout, uiRetryCount, bLastMessageBroadcast))
						break;
				}
			}
			else
			{
				//no retry allowed
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Retries Exhausted in Run App Request State");
			}
			if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
			{
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
				bUpgradeFirmwareActive = false;
				bLastFirmwareUpgradeSuccessful	= false;
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle Mode State");
				iMainState = MSM_MAINSTATE_IDLE; // in idle mode
			}
			else // come from boot mode
			{
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Boot Mode State");
				iMainState = MSM_MAINSTATE_BOOT_MODE; // in boot mode
			}
			break;
		case MSM_MAINSTATE_RUN_APP_REFRESH_DELAY:		// in RUN_APP_REFRESH_DELAY delay
			// need to refresh controller IP addresses here due to DHCP dynamic
			bIPRefreshRequired = true;
			settimer(CNW_RUN_APP_REFRESH_DELAY, &uiTimerIdentMain);	// allow refresh timer to complete
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Run App IP Refresh State");
			iMainState = MSM_MAINSTATE_RUN_APP_IP_REFRESH; // in RUN_APP_IP_REFRESH
			break;
		case MSM_MAINSTATE_RUN_APP_IP_REFRESH:	 // in BOOT_IP_REFRESH
			// assume got all needed by now so just move into idle mode
			if (bUpgradeFirmwareActive)			// if completed upgrade then flag it.
			{
				bUpgradeFirmwareActive = false;	
				bLastFirmwareUpgradeSuccessful	= true;
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Firmware Upgrade Completed Successfully");
			}
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle State");
			iMainState = MSM_MAINSTATE_IDLE; // in idle mode
			break;
		case MSM_MAINSTATE_ERASE_REQ:		// in ERASE REQUEST
			// timed out so see if allowed to retry
			if (uiRetryCount > 0)
			{
				// resend message with 1 less retry
				--uiRetryCount;
				if (SendControllerDataMessage(strLastMessageSent, uiRetryTimeout, uiRetryCount, bLastMessageBroadcast))
					break;
			}
			else
			{
				//no retry allowed
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Timeout in Erase App Request State");
			}
			if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
			{
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
				bUpgradeFirmwareActive = false;
				bLastFirmwareUpgradeSuccessful	= false;
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle Mode State");
				iMainState = MSM_MAINSTATE_IDLE; // in idle mode
			}
			else // come from boot mode
			{
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Boot Mode State");
				iMainState = MSM_MAINSTATE_BOOT_MODE; // in boot mode
			}
			break;
		case MSM_MAINSTATE_LOAD_APP_REQ:	// in LOAD REQUEST
			// timed out so see if allowed to retry
			if (uiRetryCount > 0)
			{
				// resend message with 1 less retry
				--uiRetryCount;
				if (SendControllerDataMessage(strLastMessageSent, uiRetryTimeout, uiRetryCount, bLastMessageBroadcast))
					break;
			}
			else
			{
				//no retry allowed
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Timeout in Load App Request State");
			}
			if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
			{
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
				bUpgradeFirmwareActive = false;
				bLastFirmwareUpgradeSuccessful	= false;
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle Mode State");
				iMainState = MSM_MAINSTATE_IDLE; // in idle mode
			}
			else // come from boot mode
			{
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Boot Mode State");
				iMainState = MSM_MAINSTATE_BOOT_MODE; // in boot mode
			}
			break;
		case MSM_MAINSTATE_LOAD_APP_DATA:	// in LOAD APP DATA
			// timed out so see if allowed to retry
			if (uiRetryCount > 0)
			{
				// resend message with 1 less retry
				--uiRetryCount;
				if (SendControllerDataMessage(strLastMessageSent, uiRetryTimeout, uiRetryCount, bLastMessageBroadcast))
					break;
			}
			else
			{
				//no retry allowed
				if 	(pfileFirmware != NULL)
				{
					if (pfileFirmware->is_open())
					{
						pfileFirmware->close();
					}
					delete pfileFirmware;
					pfileFirmware = NULL;
				}

				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Timeout in Load App Data State");
			}
			if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
			{
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
				bUpgradeFirmwareActive = false;
				bLastFirmwareUpgradeSuccessful	= false;
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle Mode State");
				iMainState = MSM_MAINSTATE_IDLE; // in idle mode
			}
			else // come from boot mode
			{
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Boot Mode State");
				iMainState = MSM_MAINSTATE_BOOT_MODE; // in boot mode
			}
			break;
		case MSM_MAINSTATE_CHECKSUM_REQ:	// in REQ CHECKSUM
			// timed out so see if allowed to retry
			if (uiRetryCount > 0)
			{
				// resend message with 1 less retry
				--uiRetryCount;
				if (SendControllerDataMessage(strLastMessageSent, uiRetryTimeout, uiRetryCount, bLastMessageBroadcast))
					break;
			}
			else
			{
				//no retry allowed
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Retries Exhausted in Request Checksum State");
			}
			if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
			{
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
				bUpgradeFirmwareActive = false;
				bLastFirmwareUpgradeSuccessful	= false;
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle Mode State");
				iMainState = MSM_MAINSTATE_IDLE; // in idle mode
			}
			else // come from boot mode
			{
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Boot Mode State");
				iMainState = MSM_MAINSTATE_BOOT_MODE; // in boot mode
			}
			break;
		case MSM_MAINSTATE_SEND_CMD_DATA:	// in SEND COMMAND
			// timed out so see if allowed to retry
			if (uiRetryCount > 0)
			{
				// resend message with 1 less retry
				--uiRetryCount;
				if (SendControllerDataMessage(strLastMessageSent, uiRetryTimeout, uiRetryCount, bLastMessageBroadcast))
					break;
			}
			else
			{
				//no retry allowed
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Retries Exhausted in Send Command State");
			}
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle State");
			iMainState = MSM_MAINSTATE_IDLE; // in idle
			if (iMsgState == MSG_STATUS_SENT_TIMING)
			{
				iMsgState = MSG_STATUS_SENT_TIMEDOUT;
				iLastCMDMsgState = iMsgState;
			}
			break;
		case MSM_MAINSTATE_SEND_IP_CMD:		// sent IP Dynamic command message
			switch (iSubState)
			{
			case MSM_SUBSTATE_IP_FIXED_SOCKET_WAIT:		// delay to allow socket to close fixed ip
			case MSM_SUBSTATE_IP_DYNAMIC_SOCKET_WAIT:	// delay to allow socket to close dynamic ip
				// force socket to be UDP
				if (!this->ConfigureUDPSocket(pDataSocket, CMS_COMMSPORT_CLIENT_DATA_UDP, strLocalHostIP, true))
				{
					LogMessage(CLogger::LGR_SEVERITY_ERROR,"Failed to Set Data Socket to UDP");
				}
				else
				{
					if (iSubState == MSM_SUBSTATE_IP_FIXED_SOCKET_WAIT)
					{
						iSubState = MSM_SUBSTATE_SEND_IP_FIXED_CMD;
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Send IP Fixed Request State");
						// send message
						strIPMessage = "ED" + sSerialNumber + ",0\r\n";
//						if (SendControllerDataMessage(strIPMessage, CNW_SEND_IP_CMD_TIMEOUT, CNW_SEND_IP_CMD_MSG_RETRY, true))
						if (SendControllerDataMessage(strIPMessage, 1000, 2, true))
							break;
					}
					else	// dynamic
					{
						iSubState = MSM_SUBSTATE_SEND_IP_DYNAMIC_CMD;
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Send IP Dynamic Request State");
						// send message
						strIPMessage = "ED" + sSerialNumber + ",1\r\n";
						if (SendControllerDataMessage(strIPMessage, CNW_SEND_IP_CMD_TIMEOUT, CNW_SEND_IP_CMD_MSG_RETRY, true))
							break;
					}
				}
				if (bIPChangeActive) bIPChangeActive = false;	// if completed change then flag it.
				bLastIPChangeSuccessful	= false;				// Result flag for last Changing IP 
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Idle State");
				iMainState = MSM_MAINSTATE_IDLE;
				break;
			case MSM_SUBSTATE_SEND_IP_FIXED_CMD:	// in Send Fixed IP Command
			case MSM_SUBSTATE_SEND_IP_DYNAMIC_CMD:	// in Send Dynamic IP Command
				if (uiRetryCount > 0)
				{
					// resend message with 1 less retry
					--uiRetryCount;
					if (SendControllerDataMessage(strLastMessageSent, uiRetryTimeout, uiRetryCount, bLastMessageBroadcast))
						break;
				}
				else
				{
#if 0				// timed out so see if allowed to retry
					//no retry allowed
					LogMessage(CLogger::LGR_SEVERITY_ERROR,"Retries Exhausted in Send IP Command State");
#else
					iSubState = MSM_SUBSTATE_SEND_IP_ADDRESS;
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Send IP Address State");
					// send message
					strIPMessage = "EP" + sSerialNumber + "," + strRequiredIPAddress.GetBuffer(0) + "\r\n";
					if (!SendControllerDataMessage(strIPMessage, CNW_SEND_IP_CMD_TIMEOUT, CNW_SEND_IP_CMD_MSG_RETRY, true))
					{
						if (bIPChangeActive) bIPChangeActive = false;	// if completed change then flag it.
						bLastIPChangeSuccessful	= false;				// Result flag for last Changing IP 
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Idle State");
						iMainState = MSM_MAINSTATE_IDLE;
					}
					break;
#endif
				}
				if (bIPChangeActive) bIPChangeActive = false;	// if completed change then flag it.
				bLastIPChangeSuccessful	= false;				// Result flag for last Changing IP 
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Idle State");
				iMainState = MSM_MAINSTATE_IDLE;
				break;
			case MSM_SUBSTATE_IP_PRE_REFRESH: // in PRE_IP_REFRESH delay
				// need to refresh controller IP addresses here due to DHCP dynamic
				bIPRefreshRequired = true;
				settimer(CNW_IP_REFRESH_DELAY, &uiTimerIdentMain);	// allow refresh timer to complete
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into IP Allow Refresh State");
				iSubState = MSM_SUBSTATE_ALLOW_IP_REFRESH; // in ALLOW_IP_REFRESH
				break;
			case MSM_SUBSTATE_ALLOW_IP_REFRESH:	// waiting for address refresh?
				// assume got all needed by now so just move into idle mode
				if (bIPChangeActive) bIPChangeActive = false;	// if completed change then flag it.
				bLastIPChangeSuccessful	= true;					// Result flag for last Changing IP 
				LogMessage(CLogger::LGR_SEVERITY_INFO,"IP Change Completed Successfully");
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle State");
				iMainState = MSM_MAINSTATE_IDLE; // in idle mode
				break;
			default:
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Timeout in Unsupported State");
				break;
			}
			break;

		default: // do nothing
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Timeout in Unsupported State");
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::ProcessMainStateTimer - Error Detected");
	}
}

/*********************************************************************
  Routine
  CController::ProcessSocketClosureTimer

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

void CController::ProcessSocketClosureTimer(UINT_PTR idEvent, DWORD dwTime)
{
	CString strMessage;

	try
	{
		// we can now get rid of the offending socket
		if (idEvent == uiTimerIdentDataClose)
		{
			strMessage.Format("CController::ProcessSocketClosureTimer() Closing Data Socket After Timer - %u", idEvent);
			LogMessage(CLogger::LGR_SEVERITY_INFO,strMessage);

			uiTimerIdentDataClose = 0;
			if (pDataSocket != NULL)
			{
				delete pDataSocket;
				pDataSocket = NULL;
			}
			else
			{
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::ProcessSocketClosureTimer() Fired For NULL Socket");
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
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::ProcessSocketClosureTimer() - Error Detected");
	}
}

/*********************************************************************
  Routine
  CController::ProcessSerialCommsTimer

  Description
  Serial Comms Service Timer. When Serial Comms is selected will 
  attempt to retrieve any messages and timeout comms ports once
  timeout attempts are expired.
  
  Parameters
  1 - UINT idEvent	- timer identifier
  2 - DWORD dwTime	- current system time

  Returns - void.
*********************************************************************/

void CController::ProcessSerialCommsTimer(UINT_PTR idEvent, DWORD dwTime)
{
	CString		strMessage;
	char		szInBuffer[201];
	DWORD		dwNumberOfBytesRead;
	int			iArrowPos;
	CString		strDummy;
	UINT		uiDummy;
	CString		strCommand;

	static bool	bLastMessageNull=false;		// flags if last message was null

	try
	{
		// clear until next timer
		uiTimerIdentSerialComms = 0;

		// read all available
		if (ReadFile(hFileDescriptor,	// Open FD for this controller
		  szInBuffer,					// pointer to buffer that receives data
		  200,							// number of bytes to read
		  &dwNumberOfBytesRead,			// pointer to number of bytes read
		  NULL))						// pointer to structure for data
		{
			if (dwNumberOfBytesRead != 0)
			{
				szInBuffer[dwNumberOfBytesRead] = 0;		// zero terminated string
				strInBuffer += szInBuffer;					// append to buffer
				strMessage.Format("Com Buffer of Com Port - %s = %s", sComPort, strInBuffer);
				LogMessage(CLogger::LGR_SEVERITY_INFO,strMessage);

				iArrowPos = strInBuffer.Find(">");
				while (iArrowPos > -1)
				{
					strCommand = strInBuffer.Left(iArrowPos+1);
					strInBuffer = strInBuffer.Mid(iArrowPos+1);

					if ((CheckMessageNotNull(strCommand)) || bLastMessageNull)
					{
						bLastMessageNull = false;
						strMessage.Format("Processing Command - %s", strCommand);
						LogMessage(CLogger::LGR_SEVERITY_DEBUG,strMessage);

						ProcessDataMessage(strCommand, strDummy, uiDummy);
					}
					else
						bLastMessageNull = true;

					iArrowPos = strInBuffer.Find(">");
				}
			}
		}
		else
		{
			strMessage.Format("ReadFile Error - %i on %s", GetLastError(), sComPort);
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		}

		settimer(CNW_COM_PORTS_SERVICE_TIME, &uiTimerIdentSerialComms);	// try serial comms again in....

	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::ProcessSerialCommsTimer() - Error Detected");
	}
}

/*********************************************************************
  Routine
  CController::SendControllerDataMessage

  Description
  Send message to controller. Also sets up timers for timeout.
  
  Parameters
  1 - CString &msg		- Ascii Message to Send.
  2 - UINT uiTimeout	- Timeout in Millisecs or zero for no timeout
  3 - UINT uiRetries	- Number of Retries or zero for none
  4 - bool bBroadcast	- true if message to be sent to 255.255.255.255
						  false if from to connected controllers IP address.

  Returns - bool	- true if successful otherwise false
*********************************************************************/
bool CController::SendControllerDataMessage(CString &msg, UINT uiTimeout, UINT uiRetries, bool bBroadcast)
{
	CString		strMessage;
	CString		strHostAddress;
	DWORD		dwBytesWritten;

	try
	{
		// kill any outstanding timers
		KillTimer(&uiTimerIdentMain);

		// send according to interface
		switch (iControllerType)
		{
		// no interface
		case CMS_COMMSTYPE_NOTINIT:
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error - No Comms Type Set Up");
			return false;
	
		// serial interface
		case CMS_COMMSTYPE_SERIAL:
			strLastMessageSent = msg;
			bLastMessageBroadcast = bBroadcast;

			if (WriteFile(hFileDescriptor,	// handle to file to write to
				msg,						// pointer to data to write to file
				msg.GetLength(),			// number of bytes to write
				&dwBytesWritten,NULL) == 0)	// pointer to number of bytes written
			{
				strMessage.Format("WriteFile Error - %i on %s", GetLastError(), sComPort);
				throw strMessage;
			}
			// set up new controller state...
			setInputBuffer("");				// empty buffer to start again

			strMessage = "Sent Message - " + msg;
			LogMessage(CLogger::LGR_SEVERITY_INFO,strMessage);

			if (iMsgState != MSG_STATUS_SENT_TIMING)
				iMsgState = MSG_STATUS_SENT;

			if ((uiTimeout != 0))
			{
				settimer(uiTimeout, &uiTimerIdentMain);	// time allowed for controller to respond
				uiRetryCount = uiRetries;				// retries allowed
				uiRetryTimeout = uiTimeout;				// retry timeout
				iMsgState = MSG_STATUS_SENT_TIMING;
			}

			if (iMainState == MSM_MAINSTATE_SEND_CMD_DATA)
				iLastCMDMsgState = iMsgState;

			break;
	
		// ethernet interface
		case CMS_COMMSTYPE_ETHERNET:
			strLastMessageSent = msg;
			bLastMessageBroadcast = bBroadcast;
			if (bBroadcast)
				strHostAddress = BCAST;
			else
				strHostAddress = inet_ntoa(iaIPAddress);

			if (pDataSocket->SendTo(msg,msg.GetLength(),CMS_COMMSPORT_HOST_DATA,strHostAddress) == SOCKET_ERROR)
			{
				strMessage.Format("Error SendControllerDataMessage - %i", pDataSocket->GetLastError());
				throw strMessage;
			}
			strMessage = "Sent Message - " + msg;
			LogMessage(CLogger::LGR_SEVERITY_INFO,strMessage);
			
			if (iMsgState != MSG_STATUS_SENT_TIMING)
				iMsgState = MSG_STATUS_SENT;

			if ((uiTimeout != 0))
			{
				settimer(uiTimeout, &uiTimerIdentMain);	// time allowed for controller to respond
				uiRetryCount = uiRetries;				// retries allowed
				uiRetryTimeout = uiTimeout;				// retry timeout
				iMsgState = MSG_STATUS_SENT_TIMING;
			}

			if (iMainState == MSM_MAINSTATE_SEND_CMD_DATA)
				iLastCMDMsgState = iMsgState;

			break;

		default:
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error - Invalid Comms Type");
			return false;
		}

		return true;
	}
	catch (CString)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - SendControllerDataMessage");
		return false;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - SendControllerDataMessage");
		return false;
	}
}

/*********************************************************************
  Routine
  CController::RequestSendCommand

  Description
  Client Request - Send Command Message.
  
  Parameters
  1 - int &iMsgStatus		- messaging status on completion of send (see enum MSG_STATUS)
  2 - CString* strCommand	- ascii message to send
  3 - UINT uiTimeout		- timeout for response before send retry (milliseconds or zero for no retries)
  4 - UINT uiRetries		-  number of times to retry (or zero for no retries)

  Returns - bool -	True if Successful, otherwise False
*********************************************************************/

bool CController::RequestSendCommand(int &iMsgStatus, CString * strCommand, UINT uiTimeout, UINT uiRetries)
{
	CString strSendCmdMessage;

	try
	{
		switch (iMainState)
		{
		case MSM_MAINSTATE_IDLE:
		case MSM_MAINSTATE_BOOT_MODE:
			strSendCmdMessage = strCommand->GetBuffer(0);
			strSendCmdMessage = strSendCmdMessage + "\r\n";
			if (iControllerType == CMS_COMMSTYPE_ETHERNET)
			{
				if (!CheckSocketConnected())
				{
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Idle State");
					iMainState = MSM_MAINSTATE_IDLE;
					iMsgStatus = iMsgState ;
					return false;
				}
			}
				
			iMainState = MSM_MAINSTATE_SEND_CMD_DATA;
			iMsgState = MSG_STATUS_IDLE;
			strLastCMDMessageSent = strCommand->GetBuffer(0);
			strLastCMDMessageReceived = "";
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Send Command Request State");
			if (!SendControllerDataMessage(strSendCmdMessage, uiTimeout, uiRetries, false))
			{
				// pretend we've sent something
				iMainState = MSM_MAINSTATE_SEND_CMD_DATA;
				iMsgState = MSG_STATUS_SENT_TIMING;
				strLastCMDMessageReceived = "";
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Send Command Timing State");
				strLastMessageSent = strSendCmdMessage;
				bLastMessageBroadcast = false;
				iMsgStatus = iMsgState ;
				settimer(uiTimeout, &uiTimerIdentMain);	// time allowed for controller to respond
				uiRetryCount = uiRetries+1;				// retries allowed make sure we do
				uiRetryTimeout = uiTimeout;				// retry timeout
				return true;
			}
				
			if (iMsgState != MSG_STATUS_SENT_TIMING)
			{
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Idle State");
				iMainState = MSM_MAINSTATE_IDLE;
			}
			iMsgStatus = iMsgState ;
			return true;

		default:
			iMsgStatus = iMsgState ;
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"Command Only Available From Idle State");
			return false;
		}
		return true;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - RequestSendCommand");
		throw; // and rethrow
	}
}

/*********************************************************************
  Routine
  CController::RequestReceivedMessage

  Description
  Client Request - Receive Command Message. Try to retrieve a message
  that may have been sent by the controller.
  
  Parameters
  1 - int &iMsgStatus		- current messaging status (see enum MSG_STATUS)
  2 - CString* strCommand	- ascii message received
  3 - bool &bReceiveError	- true if message is ok else false.

  Returns - bool -	True if Successful, otherwise False
*********************************************************************/

bool CController::RequestReceivedMessage(int &iMsgStatus, CString &strMessage, bool &bReceiveError)
{	
	CString strSendCmdMessage;

	try
	{
		strMessage = strLastCMDMessageReceived;
		bReceiveError = bLastMessageReceivedInError;
		iMsgStatus = iMsgState;
		return true;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - RequestReceiveMessage");
		throw; // and rethrow error
	}
}

/*********************************************************************
  Routine
  CController::RequestBootMode

  Description
  Client Request - Put us into boot mode.
  
  Parameters
  None

  Returns - bool -	True if Request Successful, otherwise False
							- true does not indicate boot mode is 
								achieved, only that the request is
								accepted.
*********************************************************************/

bool CController::RequestBootMode()
{
	iChainCommandNumber = CHN_NONE;		// no chaining from singleton
	iChainNextIndex = 0;				// index of next link in chain
	return (RequestBootModeChained());
}

bool CController::RequestBootModeChained()
{
	CString	strBootMessage;
	CString	strMessage;

	try
	{
		// get rid of any timers
		KillTimer(&uiTimerIdentMain);

		switch (iMainState)
		{
		// already in boot mode requested
		case MSM_MAINSTATE_BOOT_REQ:
			LogMessage(CLogger::LGR_SEVERITY_INFO,"CController::RequestBootMode - Already in Boot Mode Request State");
			return true;

		case MSM_MAINSTATE_BOOT_MODE:
			LogMessage(CLogger::LGR_SEVERITY_INFO,"CController::RequestBootMode - Already in Boot Mode State");
			return true;

		default:
			if (iControllerType == CTR_TYPE_ETHERNET)
			{
				// force socket to be UDP
				if (!this->ConfigureUDPSocket(pDataSocket, CMS_COMMSPORT_CLIENT_DATA_UDP, strLocalHostIP, true))
				{
					settimer(CNW_BOOT_SOCKET_WAIT, &uiTimerIdentMain);	// boot wait to try to reconnect socket delay
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Boot Socket Wait State");
					iMainState = MSM_MAINSTATE_BOOT_SOCKET_WAIT; // in BOOT_SOCKET_WAIT
					return true;
				}
			}

			iMainState = MSM_MAINSTATE_BOOT_REQ;
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Boot Mode Request State");

				// send message
			strBootMessage = "DV" + sSerialNumber + ",0\r\n";
			if (!SendControllerDataMessage(strBootMessage, CNW_BOOT_TIMEOUT, CNW_BOOT_MSG_RETRY, true))
			{
				if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
				{
					LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
					bUpgradeFirmwareActive = false;
					bLastFirmwareUpgradeSuccessful	= false;
				}
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Idle State");
				iMainState = MSM_MAINSTATE_IDLE;
				return false;
			}
			return true;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - CController::RequestBootMode");
		return false;
	}
}

/*********************************************************************
  Routine
  CController::RequestAppValid

  Description
  Client Request - Put us into app valid state.
  
  Parameters
  None

  Returns - bool -	True if Request Successful, otherwise False
							- true does not indicate action is 
								achieved, only that the request is
								initiated.
*********************************************************************/

bool CController::RequestAppValid()
{
	iChainCommandNumber = CHN_NONE;		// no chaining from singleton
	iChainNextIndex = 0;				// index of next link in chain
	return (RequestAppValidChained());
}

bool CController::RequestAppValidChained()
{
	CString strMessage;
	CString strAppValidMessage;

	try
	{
		switch (iMainState)
		{
		// in boot mode requested
		case MSM_MAINSTATE_BOOT_REQ:
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::RequestAppValid - Still in Boot Mode Request State");
			return false;

		case MSM_MAINSTATE_BOOT_MODE:
			strAppValidMessage = "DK1\r\n";

			iMainState = MSM_MAINSTATE_APP_VALID_REQ;
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into App Valid Request State");

			if (iControllerType == CTR_TYPE_ETHERNET)
			{
				if (!CheckSocketConnected())
				{
					if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
					{
						LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
						bUpgradeFirmwareActive = false;
						bLastFirmwareUpgradeSuccessful	= false;
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle Mode State");
						iMainState = MSM_MAINSTATE_IDLE; // in idle mode
					}
					else // come from boot mode
					{
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Boot Mode State");
						iMainState = MSM_MAINSTATE_BOOT_MODE;
					}
					return false;
				}
			}

			// send message
			if (!SendControllerDataMessage(strAppValidMessage, CNW_APP_VALID_TIMEOUT, CNW_APP_VALID_MSG_RETRY, false))
			{
				// assume blocking problem so go into retry loop
				iMsgState = MSG_STATUS_SENT_TIMING;
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Send Command Timing State");
				strLastMessageSent = strAppValidMessage;
				bLastMessageBroadcast = false;
				settimer(CNW_APP_VALID_TIMEOUT, &uiTimerIdentMain);		// time allowed for controller to respond
				uiRetryCount = CNW_APP_VALID_MSG_RETRY;			// retries allowed
				uiRetryTimeout = CNW_APP_VALID_TIMEOUT;			// retry timeout
			}
			break;

		default:
			LogMessage(CLogger::LGR_SEVERITY_ERROR, "CController::RequestAppValid - Command Only Available From Boot Mode State");
			return false;
		}
		return true;
	}
	catch (CString)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::RequestAppValid - Error Detected");
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		return false;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - RequestAppValid");
		return false;
	}
}

/*********************************************************************
  Routine
  CController::RequestEraseApp

  Description
  Client Request - Erase App Message
  
  Parameters
  None

  Returns - bool -	True if Request Successful, otherwise False
							- true does not indicate action is 
								achieved, only that the request is
								initiated.
*********************************************************************/

bool CController::RequestEraseApp()
{
	iChainCommandNumber = CHN_NONE;		// no chaining from singleton
	iChainNextIndex = 0;				// index of next link in chain
	return (RequestEraseAppChained());
}

bool CController::RequestEraseAppChained()
{
	CString	strMessage;
	CString strEraseAppMessage;

	try
	{
		switch (iMainState)
		{
		// in boot mode requested
		case MSM_MAINSTATE_BOOT_REQ:
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::RequestEraseApp - Still in Boot Mode Request State");
			return false;

		case MSM_MAINSTATE_BOOT_MODE:
			strEraseAppMessage = "DE\r\n";

			iMainState = MSM_MAINSTATE_ERASE_REQ;
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Erase App Request State");

			if (iControllerType == CTR_TYPE_ETHERNET)
			{
				if (!CheckSocketConnected())
				{
					if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
					{
						LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
						bUpgradeFirmwareActive = false;
						bLastFirmwareUpgradeSuccessful	= false;
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle Mode State");
						iMainState = MSM_MAINSTATE_IDLE; // in idle mode
					}
					else // come from boot mode
					{
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Boot Mode State");
						iMainState = MSM_MAINSTATE_BOOT_MODE;
					}
					return false;
				}
			}

			// send message
			if (!SendControllerDataMessage(strEraseAppMessage, CNW_ERASE_TIMEOUT, CNW_ERASE_MSG_RETRY, false))
			{
				// assume blocking problem so go into retry loop
				iMsgState = MSG_STATUS_SENT_TIMING;
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Send Command Timing State");
				strLastMessageSent = strEraseAppMessage;
				bLastMessageBroadcast = false;
				settimer(CNW_ERASE_TIMEOUT, &uiTimerIdentMain);	// time allowed for controller to respond
				uiRetryCount = CNW_ERASE_MSG_RETRY;				// retries allowed
				uiRetryTimeout = CNW_ERASE_TIMEOUT;				// retry timeout
			}
			break;

		default:
			LogMessage(CLogger::LGR_SEVERITY_ERROR, "CController::RequestEraseApp - Command Only Available From Boot Mode State");
			return false;
		}
		return true;
	}
	catch (CString)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::RequestEraseApp - Error Detected");
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		return false;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - RequestEraseApp");
		return false;
	}
}

/*********************************************************************
  Routine
  CController::RequestRunApp

  Description
  Client Request - Run App Message
  
  Parameters
  None

  Returns - bool -	True if Request Successful, otherwise False
							- true does not indicate action is 
								achieved, only that the request is
								initiated.
*********************************************************************/

bool CController::RequestRunApp()
{
	iChainCommandNumber = CHN_NONE;		// no chaining from singleton
	iChainNextIndex = 0;				// index of next link in chain
	return (RequestRunAppChained());
}

bool CController::RequestRunAppChained()
{
	CString	strMessage;
	CString strRunAppMessage;

	try
	{
		switch (iMainState)
		{
		// in boot mode requested
		case MSM_MAINSTATE_BOOT_REQ:
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::RequestRunApp - Still in Boot Mode Request State");
			return false;

		case MSM_MAINSTATE_BOOT_MODE:
			strRunAppMessage = CNW_RUN_APP_COMMAND;
			strRunAppMessage += "\r\n";

			iMainState = MSM_MAINSTATE_RUN_APP_REQ;
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Run App Request State");

			if (iControllerType == CTR_TYPE_ETHERNET)
			{
				if (!CheckSocketConnected())
				{
					if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
					{
						LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
						bUpgradeFirmwareActive = false;
						bLastFirmwareUpgradeSuccessful	= false;
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle Mode State");
						iMainState = MSM_MAINSTATE_IDLE; // in idle mode
					}
					else // come from boot mode
					{
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Boot Mode State");
						iMainState = MSM_MAINSTATE_BOOT_MODE;
					}
					return false;
				}
				else
				{
					// send message
					if (!SendControllerDataMessage(strRunAppMessage, 0, 0, false))
					{
						// assume blocking problem so go into retry loop
						iMsgState = MSG_STATUS_SENT_TIMING;
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Send Command Timing State");
						strLastMessageSent = strRunAppMessage;
						bLastMessageBroadcast = false;
						settimer(CNW_RUN_APP_TIMEOUT, &uiTimerIdentMain);	// time allowed for controller to respond
						uiRetryCount = CNW_RUN_APP_MSG_RETRY;				// retries allowed
						uiRetryTimeout = CNW_RUN_APP_TIMEOUT;				// retry timeout
					}
					else
					{
						// sent ok so go into wait to refresh
						iMainState = MSM_MAINSTATE_RUN_APP_REFRESH_DELAY;
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Delay Before Refresh State");
						settimer(CNW_RUN_PRE_REFRESH_DELAY, &uiTimerIdentMain);		// time allowed for controller to respond
					}
				}
			}
			else
			{
				// send message
				if (!SendControllerDataMessage(strRunAppMessage, CNW_RUN_APP_TIMEOUT, CNW_RUN_APP_MSG_RETRY, false))
				{
					if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
					{
						LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
						bUpgradeFirmwareActive = false;
						bLastFirmwareUpgradeSuccessful	= false;
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle Mode State");
						iMainState = MSM_MAINSTATE_IDLE; // in idle mode
					}
					else // come from boot mode
					{
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Boot Mode State");
						iMainState = MSM_MAINSTATE_BOOT_MODE;
					}
					return false;
				}
			}
			return true;

		default:
			LogMessage(CLogger::LGR_SEVERITY_ERROR, "CController::RequestAppValid - Command Only Available From Boot Mode State");
			return false;
		}
	}
	catch (CString)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::RequestAppValid - Error Detected");
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		return false;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - RequestAppValid");
		return false;
	}
}

bool RequestUpgradeFirmware();

/*********************************************************************
  Routine
  CController::RequestLoadApp

  Description
  Client Request - Load App Message
  
  Parameters
  None

  Returns - bool -	True if Request Successful, otherwise False
							- true does not indicate action is 
								achieved, only that the request is
								initiated.
*********************************************************************/

bool CController::RequestLoadApp()
{
	iChainCommandNumber = CHN_NONE;		// no chaining from singleton
	iChainNextIndex = 0;				// index of next link in chain
	return (RequestLoadAppChained());
}

bool CController::RequestLoadAppChained()
{
	CString	strMessage;
	CString strLoadAppMessage;

	try
	{
		// check for app
		if (strFirmwareFileName == "")
		{
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::RequestLoadApp - No App Selected");
			return false;
		}

		switch (iMainState)
		{
		// in boot mode requested
		case MSM_MAINSTATE_BOOT_REQ:
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::RequestLoadApp - Still in Boot Mode Request State");
			return false;

		case MSM_MAINSTATE_BOOT_MODE:
			iMainState = MSM_MAINSTATE_LOAD_APP_REQ;
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Load App Request State");
			strLoadAppMessage = "VR\r\n";

			if (iControllerType == CTR_TYPE_ETHERNET)
			{
				if (!CheckSocketConnected())
				{
					if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
					{
						LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
						bUpgradeFirmwareActive = false;
						bLastFirmwareUpgradeSuccessful	= false;
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle Mode State");
						iMainState = MSM_MAINSTATE_IDLE; // in idle mode
					}
					else // come from boot mode
					{
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Boot Mode State");
						iMainState = MSM_MAINSTATE_BOOT_MODE;
					}
					return false;
				}
			}

			if (!SendControllerDataMessage(strLoadAppMessage, CNW_LOAD_REQUEST_TIMEOUT, CNW_LOAD_REQUEST_MSG_RETRY, false))
			{
				// assume blocking problem so go into retry loop
				iMsgState = MSG_STATUS_SENT_TIMING;
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Send Command Timing State");
				strLastMessageSent = strLoadAppMessage;
				bLastMessageBroadcast = false;
				settimer(CNW_LOAD_REQUEST_TIMEOUT, &uiTimerIdentMain);	// time allowed for controller to respond
				uiRetryCount = CNW_LOAD_REQUEST_MSG_RETRY;				// retries allowed
				uiRetryTimeout = CNW_LOAD_REQUEST_TIMEOUT;				// retry timeout
			}
			break;

		default:
			LogMessage(CLogger::LGR_SEVERITY_ERROR, "CController::RequestLoadApp - Command Only Available From Boot Mode State");
			return false;
		}
		return true;
	}
	catch (CString)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::RequestLoadApp - Error Detected");
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		return false;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::RequestLoadApp - Error Detected");
		return false;
	}
}

/*********************************************************************
  Routine
  CController::RequestUpgradeFirmware

  Description
  Client Request - Upgrade Firmware (Chained command)
  
  Parameters
  None

  Returns - bool -	True if Request Successful, otherwise False
							- true does not indicate action is 
								achieved, only that the request is
								initiated.
*********************************************************************/

bool CController::RequestUpgradeFirmware()
{
	CString	strMessage;
	CString strLoadAppMessage;

	try
	{
		switch (iMainState)
		{
		// only available in idle state
		case MSM_MAINSTATE_IDLE:
			// set up command and chain link for first command
			iChainCommandNumber = CHN_UPGRADE_FIRMWARE;
			iChainNextIndex = 0;	// index of first link in chain
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Request Upgrade Firmware");
			bUpgradeFirmwareActive = true;
			HandleChainedCommand();	// Activate first link in chain
			return true;
		default:
			LogMessage(CLogger::LGR_SEVERITY_ERROR, "CController::RequestUpgradeFirmware - Command Only Available From Idle State");
			return false;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::RequestUpgradeFirmware - Error Detected");
		return false;
	}
}

/*********************************************************************
  Routine
  CController::RequestIPDynamicCommand

  Description
  Client Request - IP Dynamic Command Message
  
  Parameters
  None

  Returns - bool -	True if Request Successful, otherwise False
							- true does not indicate action is 
								achieved, only that the request is
								initiated.
*********************************************************************/

bool CController::RequestIPDynamicCommand()
{
	CString	strMessage;
	CString strIPMessage;

	try
	{
		if (iControllerType == CTR_TYPE_ETHERNET)
		{
			switch (iMainState)
			{
			case MSM_MAINSTATE_IDLE:
			case MSM_MAINSTATE_BOOT_MODE:

				bIPChangeActive = true;

				if (!this->ConfigureUDPSocket(pDataSocket, CMS_COMMSPORT_CLIENT_DATA_UDP, strLocalHostIP, true))
				{
					settimer(CNW_IP_CONFIG_SOCKET_WAIT, &uiTimerIdentMain);	// ip config wait to try to reconnect socket delay
					iMainState = MSM_MAINSTATE_SEND_IP_CMD;
					iSubState = MSM_SUBSTATE_IP_DYNAMIC_SOCKET_WAIT;
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into IP Config Socket Wait State");
					return true;
				}
				iMainState = MSM_MAINSTATE_SEND_IP_CMD;
				iSubState = MSM_SUBSTATE_SEND_IP_DYNAMIC_CMD;
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Send IP Dynamic Request State");
				// send message
				strIPMessage = "ED" + sSerialNumber + ",1\r\n";
				if (!SendControllerDataMessage(strIPMessage, CNW_SEND_IP_CMD_TIMEOUT, CNW_SEND_IP_CMD_MSG_RETRY, true))
				{
					if (bIPChangeActive) bIPChangeActive = false;	// if completed change then flag it.
					bLastIPChangeSuccessful	= false;				// Result flag for last Changing IP 
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Idle State");
					iMainState = MSM_MAINSTATE_IDLE;
				}
				break;
			default:
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Command Only Available From Boot Mode or Idle States");
				return false;
			}
		}
		else
		{
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::RequestIPDynamicCommand - Only Available for Serial Comms");
			return false;
		}
		return true;
	}
	catch (CString)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - RequestIPDynamicCommand");
		return false;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - RequestIPDynamicCommand");
		throw;
	}
}

/*********************************************************************
  Routine
  CController::RequestIPFixedCommand

  Description
  Client Request - IP Fixed Command Message
  
  Parameters
  None

  Returns - bool -	True if Request Successful, otherwise False
							- true does not indicate action is 
								achieved, only that the request is
								initiated.
*********************************************************************/

bool CController::RequestIPFixedCommand(CString *strIPAddress, CString * strIPMask, CString * strIPGateway )
{
	CString	strMessage;
	CString strIPMessage;

	try
	{
		if (iControllerType == CTR_TYPE_ETHERNET)
		{
			switch (iMainState)
			{
			case MSM_MAINSTATE_IDLE:
			case MSM_MAINSTATE_BOOT_MODE:
				// save required settings
				strRequiredIPAddress = strIPAddress->GetBuffer(0);
				strRequiredIPMask = strIPMask->GetBuffer(0);
				strRequiredIPGateway = strIPGateway->GetBuffer(0);

				// change to commas for Gardasoft
				strRequiredIPAddress.Replace(".",",");
				strRequiredIPMask.Replace(".",",");
				strRequiredIPGateway.Replace(".",",");

				bIPChangeActive = true;

				// IP changes always sent as UDP
				if (!this->ConfigureUDPSocket(pDataSocket, CMS_COMMSPORT_CLIENT_DATA_UDP, strLocalHostIP, true))
				{
					settimer(CNW_IP_CONFIG_SOCKET_WAIT, &uiTimerIdentMain);	// ip config wait to try to reconnect socket delay
					iMainState = MSM_MAINSTATE_SEND_IP_CMD;
					iSubState = MSM_SUBSTATE_IP_FIXED_SOCKET_WAIT;
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into IP Config Socket Wait State");
					return true;
				}

				iMainState = MSM_MAINSTATE_SEND_IP_CMD;
				iSubState = MSM_SUBSTATE_SEND_IP_FIXED_CMD;
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Send IP Fixed Request State");
				// send message
				strIPMessage = "ED" + sSerialNumber + ",0\r\n";
				if (!SendControllerDataMessage(strIPMessage, CNW_SEND_IP_CMD_TIMEOUT, CNW_SEND_IP_CMD_MSG_RETRY, true))
//				if (!SendControllerDataMessage(strIPMessage, 5000, 5, true))
				{
					if (bIPChangeActive) bIPChangeActive = false;	// if completed change then flag it.
					bLastIPChangeSuccessful	= false;				// Result flag for last Changing IP 
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Idle State");
					iMainState = MSM_MAINSTATE_IDLE;
				}
				break;
			default:
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Command Only Available From Boot Mode or Idle States");
				return false;
			}
		}
		else
		{
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::RequestIPFixedCommand - Only Available for Serial Comms");
			return false;
		}
		return true;
	}
	catch (CString)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - RequestIPFixedCommand");
		return false;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - RequestIPFixedCommand");
		throw;
	}
}

/*********************************************************************
  Routine
  CController::ProcessDataMessage

  Description
  State Machine Service Data Message. Used to process messages which 
  are passed here by the callback routine which is invoked when a
  message has been received. It will pass the message to the appropriate
  state handling routine.
  
  Parameters
  1 - CString& strBuff		- Full Message Received
  2 - CString& sIPAddress	- Ascii IP Address of Sender
  3 - UINT& iPort			- Port Number of Sender

  Returns - void.
*********************************************************************/

void CController::ProcessDataMessage(CString& strBuff, CString& sIPAddress, UINT& iPort)
{
	CString strMessage;

	try
	{
		switch (iMainState)
		{
		case MSM_MAINSTATE_BOOT_REQ:	// in boot mode requested
			ProcessMessageInBOOT_REQ(strBuff, sIPAddress, iPort);
			break;
		case MSM_MAINSTATE_APP_VALID_REQ:	// in APP VALID REQUEST
			ProcessMessageInAPP_VALID_REQ(strBuff, sIPAddress, iPort);
			break;
		case MSM_MAINSTATE_RUN_APP_REQ:		// in RUN APP REQUEST
			ProcessMessageInRUN_APP_REQ(strBuff, sIPAddress, iPort);
			break;
		case MSM_MAINSTATE_ERASE_REQ:		// in ERASE APP REQUEST
			ProcessMessageInERASE_APP_REQ(strBuff, sIPAddress, iPort);
			break;
		case MSM_MAINSTATE_LOAD_APP_REQ:		// in LOAD APP REQUEST
			ProcessMessageInLOAD_APP_REQ(strBuff, sIPAddress, iPort);
			break;
		case MSM_MAINSTATE_LOAD_APP_DATA:		// in LOAD APP REQUEST
			ProcessMessageInLOAD_APP_DATA(strBuff, sIPAddress, iPort);
			break;
		case MSM_MAINSTATE_CHECKSUM_REQ:		// in GET CHECKSUM REQUEST
			ProcessMessageInCHECKSUM_REQ(strBuff, sIPAddress, iPort);
			break;
		case MSM_MAINSTATE_SEND_CMD_DATA:		// in SEND CMD REQUEST
			ProcessMessageInSEND_CMD_DATA(strBuff, sIPAddress, iPort);
			break;
		case MSM_MAINSTATE_SEND_IP_CMD:			// in IP REQUEST
			ProcessMessageInSEND_IP_CMD(strBuff, sIPAddress, iPort);
			break;
		default: // do nothing
			// dodgy message so just allow timeout processing
			strMessage = "Error in Controller Message - " + strBuff;
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - ProcessTimer()");
	}
}

/*********************************************************************
  Routine
  CController::ProcessMessageInSEND_CMD_DATA

  Used to process messages which are received while in Send Command 
  Data State. It will perform simple checks on the message. Is 
  successful it will progress the main state to idle.
  
  Parameters
  1 - CString& strBuff		- Full Message Received
  2 - CString& sIPAddress	- Ascii IP Address of Sender
  3 - UINT& iPort			- Port Number of Sender

  Returns - void.
*********************************************************************/

void CController::ProcessMessageInSEND_CMD_DATA(CString& strBuff, CString& sIPAddress, UINT& iPort)
{
	int iPos;
	CString strLast;	// to be played with
	CString strRecv;	// to be played with
	CString strMessage;

	try
	{
		bLastMessageReceivedInError = true;	// message contained errors until..
		strLastCMDMessageReceived = strBuff;// in case we error out

		if (!CheckMessageFormat(iPos, strBuff))
		{
			if (iMsgState != MSG_STATUS_SENT_TIMING)	// if not in timeout mode
			{
				iMsgState = MSG_STATUS_RECEIVED;		// message received state
				iLastCMDMsgState = iMsgState;
			}
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle State");
			iMainState = MSM_MAINSTATE_IDLE;			// in idle
			return;
		}

		strLastCMDMessageReceived = strBuff;	// simply return message for now
		bLastMessageReceivedInError = false;	// message ok..

		// simply log response for now
		LogMessage(CLogger::LGR_SEVERITY_INFO,"Successful Message Receipt");
		LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle State");

		iMainState = MSM_MAINSTATE_IDLE;	// in idle
		iMsgState = MSG_STATUS_RECEIVED;	// message received state
		iLastCMDMsgState = iMsgState;
		KillTimer(&uiTimerIdentMain);	// kill any old timers
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::ProcessMessageInSEND_CMD_DATA - Error Detected()");
	}
}

/*********************************************************************
  Routine
  CController::ProcessMessageInBOOT_REQ

  Description
  Used to process messages which are received while in Boot Requested State. 
  
  Parameters
  1 - CString& strBuff		- Full Message Received
  2 - CString& sIPAddress	- Ascii IP Address of Sender
  3 - UINT& iPort			- Port Number of Sender

  Returns - void.
*********************************************************************/

void CController::ProcessMessageInBOOT_REQ(CString& strBuff, CString& sIPAddress, UINT& iPort)
{
	int iPos;
	CString strLast;	// to be played with
	CString strRecv;	// to be played with
	CString strMessage;

	try
	{
		if (!CheckMessageFormat(iPos, strBuff))
			return;

		strRecv = strBuff.Left(iPos);		// strip off ">...."

		// ditch <cr> and <lf>s
		iPos = strRecv.FindOneOf("\n\r");
		if (iPos != -1)
			strRecv = strRecv.Left(iPos);	// strip off "\n or \r"
			// copy last sent and ditch <cr> and <lf>s
		strLast = strLastMessageSent;
		iPos = strLast.FindOneOf("\n\r");
		if (iPos != -1)
			strLast = strLast.Left(iPos);	// strip off "\n or \r"

		// find mirror of sent message 
		iPos = strRecv.Find(strLast);
		if (iPos == -1)
		{
			// dodgy message so just allow timeout processing
			strMessage = "Error in Controller Message - " + strBuff;
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
			return;
		}
		else
		{
			strRecv = strRecv.Mid(strLast.GetLength());	// strip off sent message
			// if strings equal then courtesy
			if (strRecv.GetLength() == 0)
			{
				// courtesy reply so see if allowed to retry
				if (uiRetryCount == 0)
				{
					// kill timer
					KillTimer(&uiTimerIdentMain);
					LogMessage(CLogger::LGR_SEVERITY_ERROR,"Retries Exhausted in Boot Mode Request State");
					if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
					{
						LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
						bUpgradeFirmwareActive = false;
						bLastFirmwareUpgradeSuccessful	= false;
					}
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Idle State");
					iMainState = MSM_MAINSTATE_IDLE;
				}
				// else
				// don't resend but allow timer to fall through and resend there, otherwise too fast
			}
			else
				// if string equals "VL0" then BOOT MODE
				if (strRecv == CNW_BOOT_RESPONSE)
				{
					// kill timer
					KillTimer(&uiTimerIdentMain);

					if (iControllerType == CTR_TYPE_ETHERNET)
					{
						// need to refresh controller IP addresses here due to DHCP dynamic
						// but delay first to allow controller to refresh ip address
						settimer(CNW_BOOT_PRE_REFRESH_DELAY, &uiTimerIdentMain);	// pre ip refresh delay
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into boot PRE refresh delay State");
						iMainState = MSM_MAINSTATE_BOOT_PRE_REFRESH; // in BOOT_PRE_REFRESH
					}
					else
					{
						// serial comms so assume all ok and move straight into boot mode
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Boot Mode State");
						iMainState = MSM_MAINSTATE_BOOT_MODE; // in boot mode
						HandleChainedCommand();	// activate next link if chained command
					}
				}
				else
				{
					// dodgy message so just allow timeout processing
					strMessage = "Error in Controller Message - " + strBuff;
					LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
				}
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - ProcessMessageInBOOT_REQ");
	}
}

/*********************************************************************
  Routine
  CController::ProcessMessageInAPP_VALID_REQ

  Description
  Used to process messages which are received while in App Valid 
  Requested State. 
  
  Parameters
  1 - CString& strBuff		- Full Message Received
  2 - CString& sIPAddress	- Ascii IP Address of Sender
  3 - UINT& iPort			- Port Number of Sender

  Returns - void.
*********************************************************************/

void CController::ProcessMessageInAPP_VALID_REQ(CString& strBuff, CString& sIPAddress, UINT& iPort)
{
	int iPos;
	CString strLast;	// to be played with
	CString strRecv;	// to be played with
	CString strMessage;

	try
	{
		if (!CheckMessageFormat(iPos, strBuff))
			return;

		strRecv = strBuff.Left(iPos);		// strip off ">...."

		// ditch <cr> and <lf>s
		iPos = strRecv.FindOneOf("\n\r");
		if (iPos != -1)
			strRecv = strRecv.Left(iPos);	// strip off "\n or \r"
			// copy last sent and ditch <cr> and <lf>s
		strLast = strLastMessageSent;
		iPos = strLast.FindOneOf("\n\r");
		if (iPos != -1)
			strLast = strLast.Left(iPos);	// strip off "\n or \r"

		// find mirror of sent message 
		iPos = strRecv.Find(strLast);
		if (iPos != -1)
		{
			strRecv = strRecv.Mid(strLast.GetLength());	// strip off sent message
			if (strRecv == CNW_APP_VALID_RESPONSE)
			{
				// successful App Valid message so just move into boot mode
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Successful App Valid Message Response");
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Boot Mode State");
				iMainState = MSM_MAINSTATE_BOOT_MODE; // in boot mode
				KillTimer(&uiTimerIdentMain);	// kill any old timers
				HandleChainedCommand();	// activate next link if chained command
				return;
			}
		}
		// dodgy message so just allow timeout processing
		strMessage = "Error in Controller Message - " + strBuff;
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - ProcessMessageInAPP_VALID_REQ");
	}
}

/*********************************************************************
  Routine
  CController::ProcessMessageInRUN_APP_REQ

  Description
  Used to process messages which are received while in Run App 
  Requested State. 
  
  Parameters
  1 - CString& strBuff		- Full Message Received
  2 - CString& sIPAddress	- Ascii IP Address of Sender
  3 - UINT& iPort			- Port Number of Sender

  Returns - void.
*********************************************************************/

void CController::ProcessMessageInRUN_APP_REQ(CString& strBuff, CString& sIPAddress, UINT& iPort)
{
	int iPos;
	CString strLast;	// to be played with
	CString strRecv;	// to be played with
	CString strMessage;

	try
	{
		if (!CheckMessageFormat(iPos, strBuff))
			return;

		strRecv = strBuff.Left(iPos);		// strip off ">...."

		// ditch <cr> and <lf>s
		iPos = strRecv.FindOneOf("\n\r");
		if (iPos != -1)
			strRecv = strRecv.Left(iPos);	// strip off "\n or \r"
			// copy last sent and ditch <cr> and <lf>s
		strLast = strLastMessageSent;
		iPos = strLast.FindOneOf("\n\r");
		if (iPos != -1)
			strLast = strLast.Left(iPos);	// strip off "\n or \r"

		// find mirror of sent message, only look for match of command at position 0 here 
		iPos = strRecv.Find(CNW_RUN_APP_COMMAND);
		if (iPos != 0)
		{
			// dodgy message so just allow timeout processing
			strMessage = "Error in Controller Message - " + strBuff;
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
			return;
		}
		else
		{
			// assume all ok as lhs matches sent run command and err not detected
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Successful Run App Message Response");
			LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle State");
			if (bUpgradeFirmwareActive)		// if completed upgrade then flag it. (for serial comms)
			{
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Firmware Upgrade Completed Successfully");
				bUpgradeFirmwareActive = false;
				bLastFirmwareUpgradeSuccessful	= true;
			}
			iMainState = MSM_MAINSTATE_IDLE;	// in idle mode
			KillTimer(&uiTimerIdentMain);	// kill any old timers
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - ProcessMessageInRUN_APP_REQ");
	}
}

/*********************************************************************
  Routine
  CController::ProcessMessageInERASE_APP_REQ

  Description
  Used to process messages which are received while in Erase App 
  Requested State. 
  
  Parameters
  1 - CString& strBuff		- Full Message Received
  2 - CString& sIPAddress	- Ascii IP Address of Sender
  3 - UINT& iPort			- Port Number of Sender

  Returns - void.
*********************************************************************/

void CController::ProcessMessageInERASE_APP_REQ(CString& strBuff, CString& sIPAddress, UINT& iPort)
{
	int iPos;
	CString strLast;	// to be played with
	CString strRecv;	// to be played with
	CString strMessage;

	try
	{
		if (!CheckMessageFormat(iPos, strBuff))
			return;

		strRecv = strBuff.Left(iPos);		// strip off ">...."

		// ditch <cr> and <lf>s
		iPos = strRecv.FindOneOf("\n\r");
		if (iPos != -1)
			strRecv = strRecv.Left(iPos);	// strip off "\n or \r"
			// copy last sent and ditch <cr> and <lf>s
		strLast = strLastMessageSent;
		iPos = strLast.FindOneOf("\n\r");
		if (iPos != -1)
			strLast = strLast.Left(iPos);	// strip off "\n or \r"

		// find mirror of sent message 
		iPos = strRecv.Find(strLast);
		if (iPos != -1)
		{
			strRecv = strRecv.Mid(strLast.GetLength());	// strip off sent message
			// if strings equal then confimation of erase
			if (strRecv.GetLength() == 0)
			{
				// successful Erase App message so just move into boot mode
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Successful Erase App Message Response");
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Boot Mode State");
				iMainState = MSM_MAINSTATE_BOOT_MODE; // in boot mode
				KillTimer(&uiTimerIdentMain);	// kill any old timers
				HandleChainedCommand();	// activate next link if chained command
				return;
			}
		}
		// dodgy message so just allow timeout processing
		strMessage = "Error in Controller Message - " + strBuff;
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		return;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - ProcessMessageInERASE_APP_REQ");
	}
}

/*********************************************************************
  Routine
  CController::ProcessMessageInLOAD_APP_REQ

  Description
  Used to process messages which are received while in Load App 
  Requested State. 
  
  Parameters
  1 - CString& strBuff		- Full Message Received
  2 - CString& sIPAddress	- Ascii IP Address of Sender
  3 - UINT& iPort			- Port Number of Sender

  Returns - void.
*********************************************************************/

void CController::ProcessMessageInLOAD_APP_REQ(CString& strBuff, CString& sIPAddress, UINT& iPort)
{
	int iPos;
	CString strLast;	// to be played with
	CString strRecv;	// to be played with
	CString strMessage;
	bool	bFailed = false;

	try
	{
		if (!CheckMessageFormat(iPos, strBuff))
			return;

		strRecv = strBuff.Left(iPos);		// strip off ">...."

		// ditch <cr> and <lf>s
		iPos = strRecv.FindOneOf("\n\r");
		if (iPos != -1)
			strRecv = strRecv.Left(iPos);	// strip off "\n or \r"
			// copy last sent and ditch <cr> and <lf>s
		strLast = strLastMessageSent;
		iPos = strLast.FindOneOf("\n\r");
		if (iPos != -1)
			strLast = strLast.Left(iPos);	// strip off "\n or \r"

		// find mirror of sent message 
		iPos = strRecv.Find(strLast);
		if (iPos != -1)
		{
			KillTimer(&uiTimerIdentMain);		// kill any old timers

			if 	(pfileFirmware != NULL)
			{
				if (pfileFirmware->is_open())
				{
					pfileFirmware->close();
				}
			}
			else
			{
				pfileFirmware = new ifstream;
			}

			char str[201];					// buffer for file read
			CString strSendBuffer = "";		// buffer for compound message
			pfileFirmware->clear();			// reset eof etc
			
			pfileFirmware->open(strFirmwareFileName);
			
			// if not open then error else build message
			if (!pfileFirmware->is_open())
			{
				// failed to open so error and move into boot mode
				strMessage.Format("Error Failed To Open File - %s", strFirmwareFileName);
				bFailed = true;
			}
			else
			{
				// do an open just to get file length
				{
					CFile f;
					CFileException e;
					if( !f.Open( strFirmwareFileName, CFile::modeRead | CFile::shareDenyNone , &e ) )
					   {
							// failed to open so error and set file length to zero
							strMessage.Format("Error Failed To Get File Length - %s, m_cause - %i",
													strFirmwareFileName, e.m_cause);
							LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
					   }
					else
					{
						dwFirmwareFileLength = (DWORD)(f.GetLength());
						dwFirmwareFileBytesProcessed = 0;
					}
				}
				
				uiAccumulatedCheckSum = 0;	// initialise checksum
				uiAddressOffset = 0;		// initialise address offset

				while((!pfileFirmware->eof()) && (strSendBuffer.GetLength() <=(CNW_LOAD_MESSAGE_LIMIT-55)) &&
						(!bFailed))
				{
					pfileFirmware->getline(str,200);
					if ((pfileFirmware->bad() || pfileFirmware->fail()) && !pfileFirmware->eof())
					{
						strMessage.Format("File Read Error - %s", strFirmwareFileName);
						bFailed = true;
					}
					else
					{
						// add string plus 2 for crlf
						dwFirmwareFileBytesProcessed = dwFirmwareFileBytesProcessed + 2 + (DWORD) strlen(str);
						if (strlen(str) > 0)
						{
							if (!AccumulateChecksum(str))
							{
								// dodgy data so error and fail
								strMessage.Format("Data Error - %s", strFirmwareFileName);
								bFailed = true;
							}

							str[strlen(str)+1] = 0x00;
							str[strlen(str)] = ';';
							strSendBuffer = strSendBuffer + str;
						}
					}
				}

				if (!bFailed)
				{
					// check for something to send otherwise must be eof
					if (strSendBuffer.GetLength() == 0)
					{
						// empty file so error and move into boot mode
						strMessage.Format("File Empty - %s", strFirmwareFileName);
						dwFirmwareFileBytesProcessed = 0;
						bFailed = true;
					}
					else
					{
						strSendBuffer = strSendBuffer + "\r\n";		// append cr,lf and will always have something to send
						if (SendControllerDataMessage(strSendBuffer, CNW_LOAD_DATA_TIMEOUT, CNW_LOAD_DATA_MSG_RETRY, false))
						{
							// and move into data load state
							LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Load App Data State");
							iMainState = MSM_MAINSTATE_LOAD_APP_DATA; // in LOAD_APP_DATA
						}
						else
						{
							if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
							{
								LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
								bUpgradeFirmwareActive = false;
								bLastFirmwareUpgradeSuccessful	= false;
								LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle Mode State");
								iMainState = MSM_MAINSTATE_IDLE; // in idle mode
							}
							else // come from boot mode
							{
								LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Boot Mode State");
								iMainState = MSM_MAINSTATE_BOOT_MODE; // in boot mode
							}
						}
						return;
					}
				}
			}

			if 	(pfileFirmware != NULL)
			{
				if (pfileFirmware->is_open())
				{
					pfileFirmware->close();
				}
				delete pfileFirmware;
				pfileFirmware = NULL;
			}

			// and log outstanding error
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
			if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
			{
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
				bUpgradeFirmwareActive = false;
				bLastFirmwareUpgradeSuccessful	= false;
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle Mode State");
				iMainState = MSM_MAINSTATE_IDLE; // in idle mode
			}
			else // come from boot mode
			{
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Boot Mode State");
				iMainState = MSM_MAINSTATE_BOOT_MODE; // in boot mode
			}
			return;
		}
		// dodgy message so just allow timeout processing
		strMessage = "Error in Controller Message - " + strBuff;
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		return;
	}
	
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - ProcessMessageInLOAD_APP_REQ");
	}
}

/*********************************************************************
  Routine
  CController::ProcessMessageInLOAD_APP_DATA

  Description
  Used to process messages which are received while in Load App Data
  Requested State. 
  
  Parameters
  1 - CString& strBuff		- Full Message Received
  2 - CString& sIPAddress	- Ascii IP Address of Sender
  3 - UINT& iPort			- Port Number of Sender

  Returns - void.
*********************************************************************/

void CController::ProcessMessageInLOAD_APP_DATA(CString& strBuff, CString& sIPAddress, UINT& iPort)
{
	int iPos;
	CString strRecv;	// to be played with
	CString strMessage;
	CString strReqChecksumMessage;
	bool	bFailed = false;

	try
	{
		if (!CheckMessageFormat(iPos, strBuff))
			return;

		strRecv = strBuff.Left(iPos);		// strip off ">...."

		// ditch <cr> and <lf>s
		iPos = strRecv.FindOneOf("\n\r");
		if (iPos != -1)
			strRecv = strRecv.Left(iPos);	// strip off "\n or \r"

		// just expecting '>' or in 1st case 'MEMOFF....' otherwise problem message
		if ((strRecv.GetLength() != 0) && (strRecv.Left(6) != "MEMOFF"))
		{
			// dodgy message so just allow timeout processing
			strMessage = "Error in Controller Message - " + strBuff;
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
			return;
		}
		else
		{
			KillTimer(&uiTimerIdentMain);	// kill any old timers
			// if not open then error else build message
			if (pfileFirmware->is_open() == 0)
			{
				// not open so error and move into boot mode
				strMessage.Format("Error - File Not Open - %s", strFirmwareFileName);
				bFailed = true;
			}
			else
			{
				// build next message
				char str[201];					// buffer for file read
				CString strSendBuffer = "";		// buffer for compound message
				while(!pfileFirmware->eof() && (strSendBuffer.GetLength() <=(CNW_LOAD_MESSAGE_LIMIT-55)) &&
						(!bFailed))
				{
					pfileFirmware->getline(str,200);
					if ((pfileFirmware->bad() || pfileFirmware->fail()) && !pfileFirmware->eof())
					{
						// dodgy read so error and fail
						int iTest = pfileFirmware->rdstate();
						strMessage.Format("File Read Error - %s", strFirmwareFileName);
						bFailed = true;
					}
					else
					{
						// add string plus 2 for crlf
						dwFirmwareFileBytesProcessed = dwFirmwareFileBytesProcessed + 2 + (DWORD) strlen(str);
						if (strlen(str) > 0)
						{
							if (!AccumulateChecksum(str))
							{
								// dodgy data so error and move into boot mode
								strMessage.Format("Data Error - %s", strFirmwareFileName);
								bFailed = true;
							}

							str[strlen(str)+1] = 0x00;
							str[strlen(str)] = ';';
							strSendBuffer = strSendBuffer + str;
						}
					}
				}

				if (!bFailed)
				{
					// check for something to send otherwise must be eof
					if (strSendBuffer.GetLength() == 0)
					{
						dwFirmwareFileBytesProcessed -= 2; // tidy up count
						// close file here
						pfileFirmware->close();		// make sure old one is closed
						delete pfileFirmware;		// and destroyed
						pfileFirmware = NULL;

						// and get checksum stuff
						strReqChecksumMessage = "DK0\r\n";
						if (SendControllerDataMessage(strReqChecksumMessage, CNW_REQ_CHECKSUM_TIMEOUT, CNW_REQ_CHECKSUM_MSG_RETRY, false))
						{
							strMessage.Format("Completed Data Loading Phase %u bytes of %u",
												dwFirmwareFileBytesProcessed , dwFirmwareFileLength);
							LogMessage(CLogger::LGR_SEVERITY_INFO, strMessage);
							LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Request Checksum State");
							iMainState = MSM_MAINSTATE_CHECKSUM_REQ;
							return;
						}
						else
						{
							bFailed = true;
						}
					}
					else
					{
						strSendBuffer = strSendBuffer + "\r\n";		// append cr,lf
						if (SendControllerDataMessage(strSendBuffer, CNW_LOAD_DATA_TIMEOUT, CNW_LOAD_DATA_MSG_RETRY, false))
						{
							return;
						}
						else
						{
							bFailed = true;
						}
					}
				}
			}
			// if got here then failed as good paths are returned from.
			if 	(pfileFirmware != NULL)
			{
				if (pfileFirmware->is_open())
				{
					pfileFirmware->close();
				}
				delete pfileFirmware;
				pfileFirmware = NULL;
			}

			if (strMessage != "")
				LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
			if (bUpgradeFirmwareActive)		// if firmware upgrade then flag error
			{
				LogMessage(CLogger::LGR_SEVERITY_ERROR,"Firmware Upgrade Failed");
				bUpgradeFirmwareActive = false;
				bLastFirmwareUpgradeSuccessful	= false;
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Idle Mode State");
				iMainState = MSM_MAINSTATE_IDLE; // in idle mode
			}
			else // come from boot mode
			{
				LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Boot Mode State");
				iMainState = MSM_MAINSTATE_BOOT_MODE; // in boot mode
			}
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - ProcessMessageInLOAD_APP_DATA");
	}
}

/*********************************************************************
  Routine
  CController::ProcessMessageInCHECKSUM_REQ

  Description
  Used to process messages which are received while in Checksum
  Requested State. 
  
  Parameters
  1 - CString& strBuff		- Full Message Received
  2 - CString& sIPAddress	- Ascii IP Address of Sender
  3 - UINT& iPort			- Port Number of Sender

  Returns - void.
*********************************************************************/
void CController::ProcessMessageInCHECKSUM_REQ(CString& strBuff, CString& sIPAddress, UINT& iPort)
{
	int		iPos;
	UINT	uiControllerChecksum = 0;	// checksum as at controller;
	CString strLast;	// to be played with
	CString strRecv;	// to be played with
	CString strMessage;

	try
	{
		if (!CheckMessageFormat(iPos, strBuff))
			return;

		strRecv = strBuff.Left(iPos);		// strip off ">...."

		// ditch <cr> and <lf>s
		iPos = strRecv.FindOneOf("\n\r");
		if (iPos != -1)
			strRecv = strRecv.Left(iPos);	// strip off "\n or \r"
			// copy last sent and ditch <cr> and <lf>s
		strLast = strLastMessageSent;
		iPos = strLast.FindOneOf("\n\r");
		if (iPos != -1)
			strLast = strLast.Left(iPos);	// strip off "\n or \r"

		// find mirror of sent message 
		iPos = strRecv.Find(strLast);
		if (iPos != -1)
		{
			strRecv = strRecv.Mid(strLast.GetLength());	// strip off sent message
			iPos = strRecv.Find(CNW_REQ_CHECKSUM_RESPONSE);
			if ((iPos == 0) && (strRecv.GetLength() > (int)(strlen(CNW_REQ_CHECKSUM_RESPONSE))))
			{
				// get checksum
				if (sscanf(strRecv.Mid(2).GetBuffer(1),"%u", &uiControllerChecksum) == 1)
				{
					if (uiControllerChecksum == uiAccumulatedCheckSum)
					{
						// successful Check sum message so move into boot mode
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Successful Check Sum Match");
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Boot Mode State");
						iMainState = MSM_MAINSTATE_BOOT_MODE; // in boot mode
						KillTimer(&uiTimerIdentMain);	// kill any old timers
						HandleChainedCommand();	// activate next link if chained command
						return;
					}
				}
			}
		}

		// if got here then dodgy message so just allow timeout processing
		strMessage = "Error in Controller Message - " + strBuff;
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		return;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - ProcessMessageInCHECKSUM_REQ");
	}
}

/*********************************************************************
  Routine
  CController::ProcessMessageInSEND_IP_CMD

  Description
  Used to process messages which are received while in Send IP Command
  Requested State. 
  
  Parameters
  1 - CString& strBuff		- Full Message Received
  2 - CString& sIPAddress	- Ascii IP Address of Sender
  3 - UINT& iPort			- Port Number of Sender

  Returns - void.
*********************************************************************/

void CController::ProcessMessageInSEND_IP_CMD(CString& strBuff, CString& sIPAddress, UINT& iPort)
{
	int iPos;
	CString strLast;	// to be played with
	CString strRecv;	// to be played with
	CString strMessage;
	CString strIPMessage;

	try
	{
		if (!CheckMessageFormat(iPos, strBuff))
			return;

		strRecv = strBuff.Left(iPos);		// strip off ">...."

		// ditch <cr> and <lf>s
		iPos = strRecv.FindOneOf("\n\r");
		if (iPos != -1)
			strRecv = strRecv.Left(iPos);	// strip off "\n or \r"
			// copy last sent and ditch <cr> and <lf>s
		strLast = strLastMessageSent;
		iPos = strLast.FindOneOf("\n\r");
		if (iPos != -1)
			strLast = strLast.Left(iPos);	// strip off "\n or \r"

		// find mirror of sent message 
		iPos = strRecv.Find(strLast);
		if (iPos == -1)
		{
			// dodgy message so just allow timeout processing
			strMessage.Format ("Error in Controller Message - %s", strBuff);
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
			return;
		}
		else
		{
			strRecv = strRecv.Mid(strLast.GetLength());	// strip off sent message
			// if string equals "VL1" then SUCCESSFUL MESSAGE
			if (strRecv == CNW_IP_RESPONSE)
			{
				// kill timer
				KillTimer(&uiTimerIdentMain);

				// handle according to sub state
				switch (iSubState)
				{
				case MSM_SUBSTATE_SEND_IP_DYNAMIC_CMD:	// Sent IP Dynamic Command state
				case MSM_SUBSTATE_SEND_IP_GATEWAY:		// Sent IP Gateway state
					iSubState = MSM_SUBSTATE_SEND_IP_EXECUTE_CMD;
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into IP Execute Command State");
					// send message
					strIPMessage = "EX" + sSerialNumber + "\r\n";
					if (!SendControllerDataMessage(strIPMessage, CNW_SEND_IP_CMD_TIMEOUT, CNW_SEND_IP_CMD_MSG_RETRY, true))
					{
						if (bIPChangeActive) bIPChangeActive = false;	// if completed change then flag it.
						bLastIPChangeSuccessful	= false;				// Result flag for last Changing IP 
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Idle State");
						iMainState = MSM_MAINSTATE_IDLE;
					}
					break;
				case MSM_SUBSTATE_SEND_IP_EXECUTE_CMD:	// Sent IP Execute Command state
					iSubState = MSM_SUBSTATE_IP_PRE_REFRESH;
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Pre Refresh IP Delay State");
					settimer(CNW_IP_PRE_REFRESH_DELAY, &uiTimerIdentMain);	// ip pre refresh delay
					break;
				case MSM_SUBSTATE_SEND_IP_FIXED_CMD:	// Sent IP Fixed Command state
					iSubState = MSM_SUBSTATE_SEND_IP_ADDRESS;
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Send IP Address State");
					// send message
					strIPMessage = "EP" + sSerialNumber + "," + strRequiredIPAddress.GetBuffer(0) + "\r\n";
					if (!SendControllerDataMessage(strIPMessage, CNW_SEND_IP_CMD_TIMEOUT, CNW_SEND_IP_CMD_MSG_RETRY, true))
					{
						if (bIPChangeActive) bIPChangeActive = false;	// if completed change then flag it.
						bLastIPChangeSuccessful	= false;				// Result flag for last Changing IP 
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Idle State");
						iMainState = MSM_MAINSTATE_IDLE;
					}
					break;
				case MSM_SUBSTATE_SEND_IP_ADDRESS:	// Sent IP Address state
					iSubState = MSM_SUBSTATE_SEND_IP_MASK;
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Send IP Mask State");
					// send message
					strIPMessage = "EM" + sSerialNumber + "," + strRequiredIPMask.GetBuffer(0) + "\r\n";
					if (!SendControllerDataMessage(strIPMessage, CNW_SEND_IP_CMD_TIMEOUT, CNW_SEND_IP_CMD_MSG_RETRY, true))
					{
						if (bIPChangeActive) bIPChangeActive = false;	// if completed change then flag it.
						bLastIPChangeSuccessful	= false;				// Result flag for last Changing IP 
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Idle State");
						iMainState = MSM_MAINSTATE_IDLE;
					}
					break;
				case MSM_SUBSTATE_SEND_IP_MASK:		// Sent IP Mask state
					iSubState = MSM_SUBSTATE_SEND_IP_GATEWAY;
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Into Send IP Gateway State");
					// send message
					strIPMessage = "EG" + sSerialNumber + "," + strRequiredIPGateway.GetBuffer(0) + "\r\n";
					if (!SendControllerDataMessage(strIPMessage, CNW_SEND_IP_CMD_TIMEOUT, CNW_SEND_IP_CMD_MSG_RETRY, true))
					{
						if (bIPChangeActive) bIPChangeActive = false;	// if completed change then flag it.
						bLastIPChangeSuccessful	= false;				// Result flag for last Changing IP 
						LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Idle State");
						iMainState = MSM_MAINSTATE_IDLE;
					}
					break;
				default: // do nothing
					// dodgy state so drop back to Idle
					strMessage.Format("ProcessMessageInSEND_IP_CMD - Error In Substate - %i", iSubState);
					LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
					if (bIPChangeActive) bIPChangeActive = false;	// if completed change then flag it.
					bLastIPChangeSuccessful	= false;				// Result flag for last Changing IP 
					LogMessage(CLogger::LGR_SEVERITY_INFO,"Moving Back Into Idle State");
					iMainState = MSM_MAINSTATE_IDLE;
					return;
				}
			}
			else
			{
				// dodgy message so just allow timeout processing
				strMessage.Format("Error in Controller Message - %s", strBuff);
				LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
				return;
			}
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - ProcessMessageInSEND_IP_CMD");
	}
}

/*********************************************************************
  Routine
  CController::CheckSocketConnected

  Description
  Ensures that the correct socket is set up, otherwise throws an error
  message. Assumes that we are set to ethernet.
  
  Parameters
  1 - none

  Returns - none
*********************************************************************/

bool CController::CheckSocketConnected()
{
	CString	strMessage;

	try
	{
		if (ieRequiredProtocol == CAAsyncSocket::AAS_PROTOCOL_TCP)
		{
			if (!this->ConfigureTCPSocket(pDataSocket, 0, strLocalHostIP, true))
			{
				strMessage = "CController::CheckSocketConnected - Failed to Set Data Socket to TCP";
				throw strMessage;
			}
		}
		else
		{
			if (!this->ConfigureUDPSocket(pDataSocket, CMS_COMMSPORT_CLIENT_DATA_UDP, strLocalHostIP, true))
			{
				strMessage = "CController::CheckSocketConnected - Failed to Set Data Socket to UDP";
				throw strMessage;
			}
		}
		return true;
	}
	catch (CString)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::CheckSocketConnected - Error Detected");
		return false; //
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::CheckSocketConnected - Error Detected");
		throw; // and rethrow
	}
}

/*********************************************************************
  Routine
  CController::CheckMessageFormat

  Description
  Performs a simple check in the supplied message. Looks for Controller
  End Character and non existence of "ERR".
  
  Parameters
  1 - int &iPos				- iPos to return position of End Char
  2 - CString strMessage	- message to check

  Returns - bool	- true if message ok otherwise false
*********************************************************************/

bool CController::CheckMessageFormat(int &iPos, CString strMessage)
{
	CString strTest;			// to be played with
	CString strErrorMessage;

	try
	{
		strTest = strMessage;	// preserve strMessage in case of error
		strTest.MakeUpper();	// all upper for error check

		iPos = strMessage.Find(CNW_CONTROLLER_END_CHAR);
		if ((iPos == -1) || (strMessage.Find("ERR") != -1))
		{
			strErrorMessage = "Error in Controller Message - " + strMessage;
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strErrorMessage);
			return false;
		}
		return true;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CControllerNW::CheckMessageFormat - Error Detected");
		return false;
	}
}

/*********************************************************************
  Routine
  CController::CheckMessageNotNull

  Description
  Performs a simple check in the supplied message. Looks for Content
  other than End Character, \n and \r.
  
  Parameters
  1 - CString strMessage	- message to check

  Returns - bool	- true if message Not Null otherwise false
*********************************************************************/

bool CController::CheckMessageNotNull(CString strMessage)
{
	try
	{
		int		iArrowPos;
		int		iDoubleArrowPos;
		strMessage.Remove('\n');
		strMessage.Remove('\r');

		iArrowPos = strMessage.Find(">");
		iDoubleArrowPos = strMessage.Find(">>");

		if ((iArrowPos < iDoubleArrowPos) || (iDoubleArrowPos == -1))
			strMessage.Remove('>');

		if (strMessage.GetLength() <= 0)
			return false;
		else
			return true;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CController::CheckMessageNotNull - Error Detected");
		return false;
	}
}

/*********************************************************************
  Routine
  CController::AccumulateChecksum

  Description
  Handle accumulation of checksum.

  Each 32 bit data word is added to the current checksum, incremented 
  by 1 and 'ANDed' with 0xFFFFFF.
  
  Parameters
  1 - const char *sLine				- new load line to process.

  Returns - bool					- true if ok else false
*********************************************************************/

bool CController::AccumulateChecksum(const char *sLine)
{
	int				iCount;
	int				iPartPos = 0;
	CLoadDataLine*	pLoadDataLine = NULL;
	CString			csLine = sLine;
	CString			strMessage;

	try
	{
		if (!ParseLoadLine(&pLoadDataLine, csLine))
		{
			// failed to parse data load line
			strMessage.Format("CController::AccumulateChecksum - Failed To Parse Data Line - %s", csLine);
			throw strMessage;
		}

		switch (pLoadDataLine->uiRecordType)
		{
		case LDR_MEM_REC_DATA:
			// if memory address in range then
			if ((((pLoadDataLine->uiAddress + uiAddressOffset) + pLoadDataLine->uiNumberDataBytes) <= LDR_MAX_CHECK_ADDRESS) &&
				((pLoadDataLine->uiAddress + uiAddressOffset) >= LDR_MIN_CHECK_ADDRESS))
			{
				if (((pLoadDataLine->uiNumberDataBytes & 0x3) != 0) || ((pLoadDataLine->uiAddress & 0x3) != 0))
				{
					strMessage.Format("CController::AccumulateChecksum - Address and Count must be div 4 rem 0 - %s", csLine);
					throw strMessage;
				}
				else
				{
					// accumulate to checksum
					iCount = 0;
					while (iCount < (int)(pLoadDataLine->uiNumberDataBytes))
					{
						uiAccumulatedCheckSum += pLoadDataLine->ucData[iCount++];
						uiAccumulatedCheckSum += pLoadDataLine->ucData[iCount++] << 8;
						uiAccumulatedCheckSum += pLoadDataLine->ucData[iCount++] << 16;
						iCount++;
						uiAccumulatedCheckSum += 0x01;
						uiAccumulatedCheckSum &= 0x0FFFFFF;
					}
				}
			}
			break;
		case LDR_MEM_REC_EOF:		// ignore
			break;
		case LDR_MEM_REC_SECT:
			// if correct number of data bytes then
			if (pLoadDataLine->uiNumberDataBytes == 2)
			{
				// update address offset
				uiAddressOffset = pLoadDataLine->ucData[0] << 24; 
				uiAddressOffset |= pLoadDataLine->ucData[1] << 16; 
			}
			else
			{
				strMessage.Format("CController::AccumulateChecksum - Address Extension not 16 bits - %s", csLine);
				throw strMessage;
			}
			break;
		case LDR_MEM_REC_SEG:
		default:
			// not handled yet
			strMessage.Format("CController::AccumulateChecksum - Record Type Not Handled - %s", csLine);
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
			break;
		}

		if (pLoadDataLine != NULL)
		{
			delete pLoadDataLine;
			pLoadDataLine = NULL;
		}
		return true;
	}
	catch (CString)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
		if (pLoadDataLine != NULL)
		{
			delete pLoadDataLine;
			pLoadDataLine = NULL;
		}
		return false;
	}
	catch (...)
	{
		if (pLoadDataLine != NULL)
		{
			delete pLoadDataLine;
			pLoadDataLine = NULL;
		}
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in CController::AccumulateChecksum()");
		throw;
	}
}

/*********************************************************************
  Routine
  CController::ParseLoadLine

  Description
  Parse a string and return a set of tokens.
  
  Parameters
  1 - CLoadDataLine *pLoadDataLine	- pointer to new load line data.
  2 - CString csLoadLine			- load line to parse..

  Returns - bool					- true if ok else false
*********************************************************************/

bool CController::ParseLoadLine(CLoadDataLine **ppLoadDataLine, CString csLoadLine)
{
	CString strMessage;
	unsigned char ucValue;
	UINT uiCheckSum = 0;
	int	iDataPointer;
	int x;

	try
	{
		(*ppLoadDataLine) = new CLoadDataLine;

		// length must be large enough to hold minimum
		if (csLoadLine.GetLength() < 11)
		{
			strMessage.Format("CController::ParseLoadLine - processing invalid load line - %s", csLoadLine);
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
			return false;
		}

		// check ":"
		if (csLoadLine.Mid(0,1) != ":")
		{
			strMessage.Format("CController::ParseLoadLine - processing invalid load line - %s", csLoadLine);
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
			return false;
		}

		// get length of data
		ucValue = FromAsciiHex(&csLoadLine.GetBuffer(2)[1]);
		uiCheckSum += ucValue;
		(*ppLoadDataLine)->uiNumberDataBytes = ucValue;
		ucValue = (ucValue * 2) + 11;
		// length must be large enough to hold minimum plus data and we don't handle other than 16 or 32 bytes
		if ((csLoadLine.GetLength() != ucValue) || ((*ppLoadDataLine)->uiNumberDataBytes > 32))
		{
			strMessage.Format("CController::ParseLoadLine - processing invalid load line - %s", csLoadLine);
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
			return false;
		}

		// get address
		ucValue = FromAsciiHex(&csLoadLine.GetBuffer(2)[3]);
		uiCheckSum += ucValue;
		(*ppLoadDataLine)->uiAddress = ucValue << 8;
		ucValue = FromAsciiHex(&csLoadLine.GetBuffer(2)[5]);
		uiCheckSum += ucValue;
		(*ppLoadDataLine)->uiAddress += ucValue;

		// get record type
		ucValue = FromAsciiHex(&csLoadLine.GetBuffer(2)[7]);
		uiCheckSum += ucValue;
		(*ppLoadDataLine)->uiRecordType = ucValue;

		// get data payload
		iDataPointer = 9;
		for (x = 0; x < (int)((*ppLoadDataLine)->uiNumberDataBytes); x++)
		{
			ucValue = FromAsciiHex(&csLoadLine.GetBuffer(2)[iDataPointer]);
			uiCheckSum += ucValue;
			(*ppLoadDataLine)->ucData[x] = ucValue;
			iDataPointer += 2;
		}

		// check the checksum
		ucValue = FromAsciiHex(&csLoadLine.GetBuffer(2)[iDataPointer]);
		uiCheckSum &= 0xFF;
		uiCheckSum ^= 0xFF;
		uiCheckSum += 1;
		uiCheckSum &= 0xFF;
		if (uiCheckSum != ucValue)
		{
			strMessage.Format("CController::ParseLoadLine - processing invalid load line - %s", csLoadLine);
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
			return false;
		}
		return true;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in CController::ParseLoadLine()");
		throw;
	}
}

/*********************************************************************
  Routine
  CController::FromAsciiHex

  Description
  Convert an ascii string of 2 characters to UChar,
  
  Parameters
  1 - CString &csParseString	- ascii hex string.

  Returns - UChar	- IP Address in in_addr format.
*********************************************************************/

unsigned char CController::FromAsciiHex(char * sParseString)
{
	CString			strMessage;
	unsigned char	uChar;
	int				iPosition;

	uChar = 0;

	char bNibble;

	try
	{
		for (iPosition = 0; iPosition < 2; iPosition++)
		{
			bNibble = sParseString[iPosition];
			if (bNibble > 0x29 && bNibble < 0x40) // if 0 to 9 
				uChar = (uChar << 4) + (bNibble & 0x0F);
			else if (bNibble >= 'a' && bNibble <= 'f') // if a to f
				uChar = (uChar << 4) + (bNibble & 0x0F) + 9;
			else if (bNibble >= 'A' && bNibble <= 'F') // if A to F
				uChar = (uChar << 4) + (bNibble & 0x0F) + 9;
			else 
			{
				strMessage.Format("CController::FromAsciiHex - non hex character found");
				LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
				throw;
			}
		}

		return uChar;
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in CController::FromAsciiHex()");
		throw;
	}
}

/*********************************************************************
  Routine
  CController::HandleChainedCommand

  Description
  Activates the next link in a chained command. 
  
  Parameters
  1 - none.

  Returns - none.
*********************************************************************/

void CController::HandleChainedCommand()
{
	CString	strMessage;

	// Chain Command Number, and index along chain.
	static int	iaCommandChains[1][10] = {
		MSM_MAINSTATE_BOOT_MODE,			// in boot mode
		MSM_MAINSTATE_ERASE_REQ,			// sent erase message
		MSM_MAINSTATE_LOAD_APP_REQ,			// sent load request message
		MSM_MAINSTATE_APP_VALID_REQ,		// sent app valid message
		MSM_MAINSTATE_RUN_APP_REQ,			// sent run app message
		-1,									// CHAIN TERMINATOR
		0,0,0,0};

	int			iNextCommand;

	try
	{
		if (iChainCommandNumber == -1)
			return;

		iNextCommand = iaCommandChains[iChainCommandNumber][iChainNextIndex];
		
		if (iNextCommand == -1)
			return;

		iChainNextIndex += 1;			// get ready for next link

		switch (iNextCommand)
		{
		case MSM_MAINSTATE_BOOT_MODE:		// request boot mode
			RequestBootModeChained();
			return;
		case MSM_MAINSTATE_ERASE_REQ:		// sent erase message
			RequestEraseAppChained();
			return;
		case MSM_MAINSTATE_LOAD_APP_REQ:	// sent load request message
			RequestLoadAppChained();
			return;
		case MSM_MAINSTATE_APP_VALID_REQ:	// sent app valid message
			RequestAppValidChained();
			return;
		case MSM_MAINSTATE_RUN_APP_REQ:		// sent run app message
			RequestRunAppChained();
			return;
		default:
			strMessage = "CController::HandleChainedCommand - Unhandled Link.";
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
			return;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in CController::HandleChainedCommand()");
		return;
	}
}

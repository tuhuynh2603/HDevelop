#if !defined(AFX_ControllerNW_H__DBE3F7E0_664F_465A_97C0_78230DDBCFF8__INCLUDED_)
#define AFX_ControllerNW_H__DBE3F7E0_664F_465A_97C0_78230DDBCFF8__INCLUDED_

//   -----------------------------------------------------------------	//
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
//

/*********************************************************************
	ControllerNW.h: interface for the CControllerNW class.

	This interface provides client software an interface to a set
	of Gardasoft Lighting Controllers. The client software should be
	built with this class and the following dependencies, along with 
	any dependencies of those files. This class will build a list of
	controllers which will be held in a CARRAY list. The controllers
	properties will be accessible to the client through a series of 
	'get' methods.

	Typically the client would instantiate this class and then interact
	through the following methods.

	*********************************************************************
	  Routine
	  CControllerNW::setLoggerObject

	  Description
	  Set pointer to logger object. This routine sets a pointer to
	  a CLogger object. The logger object should be instantiated by
	  the code which instantiates this class. It should be derived from
	  (or simply) the supplied CLogger class. This routine should be 
	  executed before any of the others.

	  Parameters
	  1 - CLogger* cLog	- pointer to Clogger object.
	*********************************************************************


	*********************************************************************
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
	*********************************************************************


	*********************************************************************
	  Routine
	  CControllerNW::NumberOfControllers

	  Description
	  Report on number of controllers in list.
  
	  Parameters
	  None.

	  Returns - int		Number of Controllers in Controller Array.
	*********************************************************************


	*********************************************************************
	  Routine
	  CControllerNW::getSerialNumber

	  Description
	  Get Serial Number of Controller from array index.
  
	  Parameters
	  1 - const int i	- array index of required controller.

	  Returns - CString -	Ascii Serial Number
	*********************************************************************

  
	*********************************************************************
	  Routine
	  CControllerNW::getCommsPort

	  Description
	  Get Comms Port of Controller from array index.
  
	  Parameters
	  1 - const int i	- array index of required controller.

	  Returns - CString -	Ascii Comms Port
	*********************************************************************


	*********************************************************************
	  Routine
	  CControllerNW::getModel

	  Description
	  Get Model of Controller from array index.
  
	  Parameters
	  1 - const int i	- array index of required controller.

	  Returns - CString -	Ascii Model Text
	*********************************************************************


	*********************************************************************
	  Routine
	  CControllerNW::getFirmware

	  Description
	  Get Firmware of Controller from array index.
  
	  Parameters
	  1 - const int i	- array index of required controller.

	  Returns - CString -	Ascii FirmWare Text
	*********************************************************************


	*********************************************************************
	  Routine
	  CControllerNW::getIPAddress

	  Description
	  Get IP Address of Controller from array index.
  
	  Parameters
	  1 - const int i	- array index of required controller.

	  Returns - in_addr -	in_addr structure holding IP Address of Controller
	*********************************************************************


	*********************************************************************
	  Routine
	  CControllerNW::getControllerType

	  Description
	  Get type of Controller. This routine returns the type of Controller.
  
	  Parameters
	  1 - const int i	- array index of required controller.

	  Returns - int CTR_TYPE		Controller Type.
	*********************************************************************


	*********************************************************************
	  Routine
	  CControllerNW::getProtocol

	  Description
	  Get protocol. This routine returns the currently set protocol.
  
	  Parameters
	  1 - const int i	- array index of required controller.

	  Returns - CString			"TCP" if TCP
								"UDP" if UDP
								"FAILED" if failure in routine
	*********************************************************************


	*********************************************************************
	  Routine
	  CControllerNW::setProtocol

	  Description
	  Set protocol. This routine sets the required protocol.
  
	  Parameters
	  1 - const int i		- array index of required controller.
	  2 - CString strProto	- required protocol (Must be "TCP" or "UDP").

	  Returns - bool	- true if successful otherwise false
	*********************************************************************


	*********************************************************************
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
	*********************************************************************


	*********************************************************************
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
	*********************************************************************


*********************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Controller.h"
#include "AAsyncSocket.h"
#include "Logger.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CControllerNW interface

class CControllerNW : public ISocketEvents, public CTimer
{
public:
	CControllerNW();
	virtual ~CControllerNW();

	enum CMS_COMMSTYPE               // Declare enum type for comms
	{
		CMS_COMMSTYPE_NOTINIT,
		CMS_COMMSTYPE_SERIAL,
		CMS_COMMSTYPE_ETHERNET
	};

public:

	bool RefreshControllerList();
	virtual bool RequestSendCommand(const int i, int &iMsgStatus, CString * strCommand, UINT uiTimeout, UINT uiRetries);
	bool RequestReceivedMessage(const int i, int &iMsgStatus, CString &strMessage, bool &bReceiveError);

	int			NumberOfControllers();
	CString		getSerialNumber(const int i);
	CString		getCommsPort(const int i);
	CString		getModel(const int i);
	CString		getFirmware(const int i);
	in_addr		getIPAddress(const int i);
	CString		getIPAddressLocalHost (const int i);

	void		ProcessTimer(							// Service Timer Routine
							UINT_PTR idEvent,					// timer identifier
							DWORD dwTime);					// current system time

	void		setLoggerObject(CLogger* cLog);			// set pointer to logger object

	int			getControllerType(const int i);
	CString		getProtocol(const int i);							// get protocol
	bool		setProtocol(const int i, CString strProto);			// set protocol

	bool		getControllerCMDStatusValues(const int i,
											 CString &strMsgState,
											 CString &strLastCMDMessageSent,
											 CString &strLastCMDMessageReceived,
											 bool &bLastMessageReceivedInError);


protected:

#define		CNW_SOCKET_CLOSURE_TIME			5000		// 5000 Msecs to allow closure of sockets
#define		CNW_SEARCH						"Gardasoft Search"
#define		CNW_CONTROLLER_END_CHAR			">"			// terminating character for controller messages
#define		CNW_LOG_BUFFER_LENGTH			1000		// allow 1000 entries in log
#define		CNW_NUMBER_OF_SERIAL_PORTS		(32)			// refresh up to COM32
#define		CNW_COM_PORTS_SERVICE_TIME		50			// try for serial receive every 50MSecs
#define		CNW_COM_PORTS_BAUD				115200		// baud rate is 115200
#define		CNW_COM_PORTS_BITS				8			// bits per byte = 8
#define		CNW_COM_PORTS_STOP_BITS			1			// 1 stop bit
#define		CNW_COM_PORTS_REFRESH_TIMEOUT	100			// Allow 5 seconds to reply (tied to CNW_COM_PORTS_SERVICE_TIME)
#define		CNW_SOCKET_REFRESH_TIMEOUT		10000		// Allow 10 seconds to reply
#define		CNW_REFRESH_ATTEMPTS			2			// Allow 2 refresh attempts

#define		CNW_NUMBER_OF_ETHERNET_SOCKETS	(99)			// maximum number of ethernet sockets allowed ...

	enum CMS_COMMSPORT					// Declare enum type for comms ports
	{									// we are client Controller is Host
		CMS_COMMSPORT_CLIENT_CONTROL	= 30310,
		CMS_COMMSPORT_HOST_CONTROL		= 30311,
		CMS_COMMSPORT_CLIENT_DATA_UDP	= 30312,
		CMS_COMMSPORT_CLIENT_TCP_FIRST	= 30313,
		CMS_COMMSPORT_CLIENT_TCP_LAST	= 30399,
		CMS_COMMSPORT_HOST_DATA			= 30313
	};

	// Close or Remove a socket
	bool CloseOrRemoveSocket(CAAsyncSocket* &rpSocket, UINT_PTR* pidEvent,
								boolean bAllowedToDelete);

	// set up or recreate a UDP socket
	bool ConfigureUDPSocket(CAAsyncSocket* &pSocket, UINT nSocketPort, 
							LPCTSTR lpszSocketAddress, boolean bAllowedToDelete = false);

	// ISocketEvents handlers
	void OnSocketReceive(CString& strBuff, CString& sIPAddress, UINT& iPort); 

	// Socket Closure Service Timer
	void ProcessSocketClosureTimer( UINT_PTR idEvent,	// timer identifier
									DWORD dwTime);	// current system time

	// Serial Comms Service Timer
	void ProcessSerialCommsTimer( UINT_PTR idEvent,		// timer identifier
								  DWORD dwTime);	// current system time

	//Refresh Socket Service Timer
	void ProcessRefreshSocketTimer(UINT_PTR idEvent,	// timer identifier
								   DWORD dwTime);	// current system time

	// send refresh controller list message
	bool SendRefreshMessage(enum CMS_COMMSTYPE iCommsType);

	// remove all controllers from the lists
	bool RemoveAllControllers();

	// log message routines
	void LogMessage(CLogger::LGR_SEVERITY, CString & strLogMessage);
	void LogMessage(CLogger::LGR_SEVERITY, char *);

	CController* FindFromSerial(const CString strSerialNumber);
	CController* FindFromCommsPort(const CString strCommsPort);

	bool SetUpEthernet();
	bool SetUpSerial();

	// list of Controllers
	CArray<CController, CController&> Controllers;

	// list of Serial Ports for Refresh Message
	CArray<CController, CController&> ComPortSerialRefresh;

	// pointer to logger object
	CLogger* pLogger;

	// pointer to socket(s) to handle search for controllers
	CAAsyncSocket *apControlSockets [CNW_NUMBER_OF_ETHERNET_SOCKETS];
public : // a bit of a bodge here ...
	CString         astrLocalHostIP [CNW_NUMBER_OF_ETHERNET_SOCKETS];
	CString         strzBroadcastIP;
protected :
	// comms timeout values
	COMMTIMEOUTS	comTimeOut;                   

	CAAsyncSocket::AAS_PROTOCOL ieRequiredProtocol;

	int		iRefreshesRemaining_Ethernet;		// number of times allowed to retry
	int		iRefreshesRemaining_Serial;			// number of times allowed to retry

	UINT_PTR	uiTimerIdentMain;			// process timer for Main State Machine
	UINT_PTR	uiTimerIdentTCPIPRefresh;	// process timer for Socket Comms
	UINT_PTR	uiTimerIdentControlClose;	// process timer for Closure Timeout for Control Socket
	UINT_PTR	uiTimerIdentSerialComms;	// process timer for Serial Port Maintenance

	UINT	uiNumberofEthernetPorts;
};

#endif // !defined(AFX_ControllerNW_H__DBE3F7E0_664F_465A_97C0_78230DDBCFF8__INCLUDED_)

#if !defined(AFX_Controller_H__B9BFBAEC_28B9_4CEF_BA5F_A3D1B56509E4__INCLUDED_)
#define AFX_Controller_H__B9BFBAEC_28B9_4CEF_BA5F_A3D1B56509E4__INCLUDED_

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
//	Controller.h: interface for the CController class					//
//																		//
//	-----------------------------------------------------------------	//
*/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Timer.h"
#include "AAsyncSocket.h"
#include "Logger.h"

class CController	:	public ISocketEvents, CTimer
{
public:
	CController();
	CController(HANDLE hFD, CString sSN, CString sM, DWORD dA, CString sCP, CString sF, int iTOC);
	CController(const CController &item);
	CController &operator=(const CController& item);

	virtual ~CController();

	enum CTR_TYPE               // Declare enum type for controller
	{
		CTR_TYPE_NOTINIT,
		CTR_TYPE_SERIAL,
		CTR_TYPE_ETHERNET
	};

	enum CTR_STATE               // Declare enum type for controller state
	{
		CTR_STATE_NOTINIT,
		CTR_STATE_REFRESH_SENT,	// refresh message "ES" sent to serial 
								//	controller - awaiting response
		CTR_STATE_VR_SENT,		// refresh message "VR" sent to serial 
								//	controller - awaiting response
		CTR_STATE_INIT,			// serial controller now initialised
		CTR_STATE_OPEN			// serial controller now connected and FD opened
	};

	virtual bool RequestSendCommand(int &iMsgStatus, CString * strCommand, UINT uiTimeout, UINT uiRetries);
	bool RequestReceivedMessage(int &iMsgStatus, CString &strMessage, bool &bReceiveError);

	bool RequestBootMode();
	bool RequestAppValid();
	bool RequestRunApp();
	bool RequestEraseApp();
	bool RequestLoadApp();
	bool RequestUpgradeFirmware();
	bool RequestIPDynamicCommand();
	bool RequestIPFixedCommand(CString * strIPAddress, CString * strIPMask, CString * strIPGateway);

protected:

#define		CNW_SOCKET_CLOSURE_TIME			5000		// 5000 Msecs to allow closure of sockets
#define		CNW_SEARCH						"Gardasoft Search"
#define		CNW_CONTROLLER_END_CHAR			">"			// terminating character for controller messages
#define		CNW_COM_PORTS_SERVICE_TIME		50			// try for serial receive every 50MSecs
#define		CNW_COM_PORTS_BAUD				115200		// baud rate is 115200
#define		CNW_COM_PORTS_BITS				8			// bits per byte = 8
#define		CNW_COM_PORTS_STOP_BITS			1			// 1 stop bit

#define		CNW_BOOT_RESPONSE				"VL0"			// Sought Boot Response
#define		CNW_BOOT_TIMEOUT				500				// 500 msec timeout for boot response
#define		CNW_BOOT_SOCKET_WAIT			2000			// 2000 msec to allow socket to close
#define		CNW_BOOT_MSG_RETRY				20				// 20 attempts for boot response
#define		CNW_BOOT_PRE_REFRESH_DELAY		3000			// 3000 msec delay before requesting refresh
#define		CNW_BOOT_REFRESH_DELAY			3000			// 3000 msec delay to allow controller to refresh IP
#define		CNW_APP_VALID_RESPONSE			"VL1"			// App Valid Response
#define		CNW_APP_VALID_TIMEOUT			1500			// 1500 msec timeout for app valid response
#define		CNW_APP_VALID_MSG_RETRY			2				// 0 attempts for any response
#define		CNW_LOAD_REQUEST_TIMEOUT		500				// 500 msec timeout for load request response
#define		CNW_LOAD_REQUEST_MSG_RETRY		2				// 0 attempts for any response
#define		CNW_LOAD_REQUEST_VALID_RESPONSE	"B_PP4/5 V015"	// Load Request Valid Response
#define		CNW_LOAD_MESSAGE_LIMIT			200				// Max Number of Chars transmitted
#define		CNW_LOAD_DATA_TIMEOUT			500				// 500 msec timeout for load data response
#define		CNW_LOAD_DATA_MSG_RETRY			3				// 3 attempts for any response
#define		CNW_REQ_CHECKSUM_TIMEOUT		500				// 500 msec timeout for get checksum response
#define		CNW_REQ_CHECKSUM_MSG_RETRY		2				// 1 attempts for any response
#define		CNW_REQ_CHECKSUM_RESPONSE		"VL"			// Sought checksum response leader
#define		CNW_SEND_IP_CMD_TIMEOUT			500				// 500 msec timeout for send Command response
#define		CNW_SEND_IP_CMD_MSG_RETRY		0				// 0 attempts for any response
#define		CNW_IP_RESPONSE					"VL1"			// IP Valid Response
#define		CNW_IP_REFRESH_DELAY			3000			// 3000 msec delay to allow controller to refresh IP
#define		CNW_IP_PRE_REFRESH_DELAY		10000			// 10000 msec delay before requesting refresh
#define		CNW_IP_CONFIG_SOCKET_WAIT		2000			// 2000 msec to allow socket to close
#define		CNW_ERASE_TIMEOUT				5000			// 5000 msec timeout for erase response
#define		CNW_ERASE_MSG_RETRY				2				// 1 attempts for any response
#define		CNW_RUN_PRE_REFRESH_DELAY		3000			// 3000 msec delay before requesting refresh
#define		CNW_RUN_APP_MSG_RETRY			2				// 2 attempts for any response
#define		CNW_RUN_APP_REFRESH_DELAY		3000			// 3000 msec delay to allow controller to refresh IP
#define		CNW_RUN_APP_TIMEOUT				2000			// 2000 msec timeout for run app response
#define		CNW_RUN_APP_COMMAND				"DG"			// Command to run app, also response

#define     LDR_MEM_REC_DATA        (0x00)
#define     LDR_MEM_REC_EOF         (0x01)
#define     LDR_MEM_REC_SEG         (0x02)
#define     LDR_MEM_REC_SECT        (0x04)
#define     LDR_MIN_CHECK_ADDRESS   (0x010000)
#define     LDR_MAX_CHECK_ADDRESS   (0x0557dc)

	enum CMS_COMMSTYPE               // Declare enum type for comms
	{
		CMS_COMMSTYPE_NOTINIT,
		CMS_COMMSTYPE_SERIAL,
		CMS_COMMSTYPE_ETHERNET
	};

	enum CMS_COMMSPORT					// Declare enum type for comms ports
	{									// we are client Controller is Host
		CMS_COMMSPORT_CLIENT_CONTROL	= 30310,
		CMS_COMMSPORT_HOST_CONTROL		= 30311,
		CMS_COMMSPORT_CLIENT_DATA_UDP	= 30312,
		CMS_COMMSPORT_CLIENT_TCP_FIRST	= 30313,
		CMS_COMMSPORT_CLIENT_TCP_LAST	= 30399,
		CMS_COMMSPORT_HOST_DATA			= 30313
	};

	enum MSM_MAINSTATES			// Declare enum type for main state machine
	{
		MSM_MAINSTATE_IDLE = 0,				// idle state
		MSM_MAINSTATE_SEND_CMD_DATA,		// sent command message
		MSM_MAINSTATE_BOOT_REQ,				// sent boot command
		MSM_MAINSTATE_BOOT_PRE_REFRESH,		// boot PRE refresh delay
		MSM_MAINSTATE_BOOT_IP_REFRESH,		// boot IP refresh
		MSM_MAINSTATE_BOOT_SOCKET_WAIT,		// delay to allow socket to close
		MSM_MAINSTATE_BOOT_MODE,			// in boot mode
		MSM_MAINSTATE_APP_VALID_REQ,		// sent app valid message
		MSM_MAINSTATE_RUN_APP_REQ,			// sent run app message
		MSM_MAINSTATE_RUN_APP_REFRESH_DELAY,// run app refresh delay
		MSM_MAINSTATE_RUN_APP_IP_REFRESH,	// run app IP refresh
		MSM_MAINSTATE_ERASE_REQ,			// sent erase message
		MSM_MAINSTATE_LOAD_APP_REQ,			// sent load request message
		MSM_MAINSTATE_LOAD_APP_DATA,		// sent load data message
		MSM_MAINSTATE_CHECKSUM_REQ,			// sent get checksum message
		MSM_MAINSTATE_SEND_IP_CMD			// sent IP command message
	};

	enum MSM_SUBSTATES					// Declare enum type for sub state machine
	{
		MSM_SUBSTATE_IDLE = 0,				// idle state
		MSM_SUBSTATE_SEND_IP_DYNAMIC_CMD,	// Sent IP Dynamic Command state
		MSM_SUBSTATE_SEND_IP_EXECUTE_CMD,	// Sent IP Execute Command state
		MSM_SUBSTATE_SEND_IP_FIXED_CMD,		// Sent IP Fixed Command state
		MSM_SUBSTATE_SEND_IP_ADDRESS,		// Sent IP Address state
		MSM_SUBSTATE_SEND_IP_MASK,			// Sent IP Mask state
		MSM_SUBSTATE_SEND_IP_GATEWAY,		// Sent IP Gateway state
		MSM_SUBSTATE_ALLOW_IP_REFRESH,		// Simple Timeout to allow update of ip
		MSM_SUBSTATE_IP_FIXED_SOCKET_WAIT,	// delay to allow socket to close fixed ip
		MSM_SUBSTATE_IP_DYNAMIC_SOCKET_WAIT,// delay to allow socket to close dynamic ip
		MSM_SUBSTATE_IP_PRE_REFRESH			// IP pre refresh delay
	};

	enum MSG_STATUS						// Declare enum type for messaging status
	{
		MSG_STATUS_IDLE = 0,				// idle state
		MSG_STATUS_SENT,					// message sent state
		MSG_STATUS_SENT_TIMING,				// message sent timeout running state
		MSG_STATUS_SENT_TIMEDOUT,			// message sent but timeout before response state
		MSG_STATUS_RECEIVED					// message received state

	};

	enum CHN_COMMANDS					// Declare enum type for chained commands
										// actual links are defined in routine HandleChainedCommand
	{
		CHN_NONE =-1,						// no chaining
		CHN_UPGRADE_FIRMWARE				// Command to upgrade firmware
	};

	struct		CLoadDataLine
	{
		UINT	uiNumberDataBytes;		// Byte count, two hex digits, a number of bytes (hex digit pairs) in the data field. 16 (0x10) or 32 (0x20) bytes of data are the usual compromise values between line length and address overhead.
		UINT	uiAddress;				// Address, four hex digits, a 16-bit address of the beginning of the memory position for the data. Limited to 64 kilobytes. (Big Endian)
		UINT	uiRecordType;			// Record type, two hex digits, 00 to 05, defining the type of the data field.
		unsigned char ucData[32];		// Data, a sequence of n bytes of the data themselves. 2n hex digits.
	};

	// Close or Remove a socket
	bool CloseOrRemoveSocket(CAAsyncSocket* &rpSocket, UINT_PTR* pidEvent, boolean bAllowedToDelete);

	// set up or recreate a UDP socket
	bool ConfigureUDPSocket(CAAsyncSocket* &pSocket, UINT nSocketPort, LPCTSTR lpszSocketAddress, boolean bAllowedToDelete = false);

	// set up or recreate a TCP socket
	bool ConfigureTCPSocket(CAAsyncSocket* &pSocket, UINT nSocketPort, LPCTSTR lpszSocketAddress, boolean bAllowedToDelete = false);

	// ISocketEvents handlers
	void OnSocketReceive(CString& strBuff, CString& sIPAddress, UINT& iPort); 

	// State Machine Service Data Messages
	virtual void ProcessDataMessage(CString& strBuff, CString& sIPAddress, UINT& iPort); 
	void ProcessMessageInSEND_CMD_DATA(CString& strBuff, CString& sIPAddress, UINT& iPort);
	void ProcessMessageInBOOT_REQ(CString& strBuff, CString& sIPAddress, UINT& iPort);
	void ProcessMessageInAPP_VALID_REQ(CString& strBuff, CString& sIPAddress, UINT& iPort);
	void ProcessMessageInRUN_APP_REQ(CString& strBuff, CString& sIPAddress, UINT& iPort);
	void ProcessMessageInERASE_APP_REQ(CString& strBuff, CString& sIPAddress, UINT& iPort);
	void ProcessMessageInLOAD_APP_REQ(CString& strBuff, CString& sIPAddress, UINT& iPort);
	void ProcessMessageInLOAD_APP_DATA(CString& strBuff, CString& sIPAddress, UINT& iPort);
	void ProcessMessageInCHECKSUM_REQ(CString& strBuff, CString& sIPAddress, UINT& iPort);
	void ProcessMessageInSEND_IP_CMD(CString& strBuff, CString& sIPAddress, UINT& iPort);

	void HandleChainedCommand();
	bool RequestBootModeChained();
	bool RequestAppValidChained();
	bool RequestRunAppChained();
	bool RequestEraseAppChained();
	bool RequestLoadAppChained();

	bool CheckMessageFormat(int &iPos, CString strMessage);
	bool CheckMessageNotNull(CString strMessage);

	void ProcessTimer(							// Service Timer Routine
							UINT_PTR idEvent,					// timer identifier
							DWORD dwTime);					// current system time

	// State Machine Service Timer
	virtual void ProcessMainStateTimer(	UINT_PTR idEvent,		// timer identifier
										DWORD dwTime);		// current system time
	
	// Socket Closure Service Timer
	void ProcessSocketClosureTimer( UINT_PTR idEvent,	// timer identifier
									DWORD dwTime);	// current system time

	// Serial Comms Service Timer
	void ProcessSerialCommsTimer( UINT_PTR idEvent,		// timer identifier
								  DWORD dwTime);	// current system time

	// send message to controller
	bool SendControllerDataMessage(CString &msg, UINT uiTimeout, UINT uiRetries, bool bBroadcast);

	bool CheckSocketConnected();

	bool	SetAsEthernet();
	bool	SetAsSerial();

	bool AccumulateChecksum(const char *sLine);
	bool ParseLoadLine(CLoadDataLine **pLoadDataLine, CString csLoadLine);
	unsigned char FromAsciiHex(char * sParseString);

	CString		strRequiredIPAddress;
	CString		strRequiredIPMask;
	CString		strRequiredIPGateway;

	HANDLE		hFileDescriptor;
	CString		sSerialNumber;
	CString		sModel;
	in_addr		iaIPAddress;
	CString		sComPort;
	CString		sFirmware;
	bool		boIsValid;

	CString		strInBuffer;

	int			iTimeOutCount;	// number of times serial timer to fire before timed out
	CLogger*	pLogger;		// pointer to logger object

	// pointer to socket to handle ethernet dialog to controllers
	CAAsyncSocket* pDataSocket;

	// Expected Controller Type
	enum CTR_TYPE iControllerType;

	// Controller State (used only in serial comms)
	enum CTR_STATE iControllerState;

	// Main State
	int iMainState;

	// log message routines
	void LogMessage(CLogger::LGR_SEVERITY, CString &strLogMessage);
	void LogMessage(CLogger::LGR_SEVERITY, char *);

	// parsing routines
	CStringArray * ParseCString(CString &csParseString, CString csDelimiter);
	DWORD FromAsciiHex(const CString csParseString);

	CAAsyncSocket::AAS_PROTOCOL ieRequiredProtocol;

	enum MSG_STATUS	iMsgState;					// messaging status

	CString						strLastMessageSent;
	enum MSG_STATUS				iLastCMDMsgState;			// messaging status
	CString						strLastCMDMessageSent;		// last command message requested by user
	CString						strLastCMDMessageReceived;	// last message received in response to user command
	bool						bLastMessageReceivedInError;		// message contained errors
	bool						bLastMessageBroadcast;

	UINT						uiRetryCount;
	UINT						uiRetryTimeout;

	// comms timeout values
	COMMTIMEOUTS				comTimeOut;                   

	CString						strLocalHostIP;

	UINT_PTR						uiTimerIdentMain;		// ident of main state timer
	UINT_PTR						uiTimerIdentDataClose;	// ident of data socket close timer
	UINT_PTR						uiTimerIdentSerialComms;// process timer for Serial Port Maintenance

	// Sub State
	enum MSM_SUBSTATES iSubState;

	UINT		uiAccumulatedCheckSum;
	UINT		uiAddressOffset;

	// firmware file object
//	ifstream		fileFirmware;
	ifstream		*pfileFirmware;

	CString		strFirmwareFileName;
	DWORD		dwFirmwareFileLength;
	DWORD		dwFirmwareFileBytesProcessed;

	int	iChainCommandNumber;		// command number or -1 if none.
	int	iChainNextIndex;			// index of next link in chain

	bool bUpgradeFirmwareActive;			// upgrading firmware sequence
	bool bIPChangeActive;					// Changing IP sequence
	bool bLastFirmwareUpgradeSuccessful;	// Result flag for last firmware upgrade
	bool bLastIPChangeSuccessful;			// Result flag for last Changing IP sequence

public:
	HANDLE		getFileDescriptor();
	void		setFileDescriptor(HANDLE hFD);
	CString		getSerialNumber();
	void		setSerialNumber(CString sSN);
	CString		getModel();
	void		setModel(CString sM);
	CString		getComPort();
	void		setComPort(CString sCP);
	CString		getFirmware();
	void		setFirmware(CString sF);
	CString		getLocalHostIPAddress();
	void		setLocalHostIPAddress(CString sIP);
	in_addr		getIPAddress();
	void		setIPAddress(DWORD wAddr);
	void		setIPAddress(char ba1, char ba2, char ba3, char ba4);
	int			getTimeOutCount();
	void		setTimeOutCount(int iTOC);
	int			decTimeOutCount();
	int			getControllerType();
	void		setControllerType(enum CController::CTR_TYPE iCT);
	int			getControllerState();
	void		setControllerState(enum CController::CTR_STATE iCS);

	bool		getControllerStatusValues(CString &strMainState,
										  CString &strSubState,
										  CString &strUpdateFileMessage,
										  int &iPercentageComplete,
										  bool &bComplexCmdActive);

	bool		getControllerCMDStatusValues(CString &strMsgState,
											 CString &strLastCMDMsgSent,
											 CString &strLastCMDMsgReceived,
											 bool &bErrorInLastMessageReceived);

	bool		getLastFirmwareUpgradeSuccessful();
	bool		getLastIPChangeSuccessful();

	CString		getProtocol();							// get protocol
	bool		setProtocol(CString strProto);			// set protocol

	bool		ConnectToController();
	bool		DisconnectFromController();

	CString		getInputBuffer();
	void		setInputBuffer(CString sIB);
	void		appendInputBuffer(const char * sIB);

	void		initialise(CString& csSearchResponse);

	void		setLoggerObject(CLogger* cLog);				// set pointer to logger object

	void		setFirmwareFileName(const CString &strFFileName);
	CString		getFirmwareFileName();

	bool		bIPRefreshRequired;		// set to true to indicate that this controller requires an IP refresh
										// should be set to false once request has been serviced
};

#endif // !defined(AFX_Controller_H__B9BFBAEC_28B9_4CEF_BA5F_A3D1B56509E4__INCLUDED_)

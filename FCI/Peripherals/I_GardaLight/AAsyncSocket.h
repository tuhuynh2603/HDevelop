#if !defined(AFX_AASYNCSOCKET_H__DC449534_D3D1_49F7_B7F1_B8662624C1AA__INCLUDED_)
#define AFX_AASYNCSOCKET_H__DC449534_D3D1_49F7_B7F1_B8662624C1AA__INCLUDED_


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
//	AAsyncSocket.h : header file										//
//																		//
//	-----------------------------------------------------------------	//
*/


#define		BCAST	"255.255.255.255"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <afxsock.h>
#include "Logger.h"

using namespace std;



class ISocketEvents {
protected:
   ISocketEvents() { };
   virtual ~ISocketEvents() { };
public:
   virtual void OnSocketReceive(CString& strBuff, CString& sIPAddress, UINT& iPort) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// CAAsyncSocket command target

class CAAsyncSocket : public CAsyncSocket
{
// Attributes
public:

// Operations
public:

	enum AAS_PROTOCOL               // Declare enum type for socket protocol
	{
		AAS_PROTOCOL_NOTINIT,
		AAS_PROTOCOL_UDP,
		AAS_PROTOCOL_TCP
	};

	enum AAS_STATE					// Declare enum type for socket state
	{
		AAS_STATE_NOTINIT,			// initial state before activation
		AAS_STATE_ACTIVE,			// active state
		AAS_STATE_CLOSE_REQUESTED,	// close requested on this socket awaiting
		AAS_STATE_CLOSED			// close confirmed awaiting destruction
	};

	CAAsyncSocket();
	virtual ~CAAsyncSocket();

	// Register client to receive events
	void Register(ISocketEvents* client)
	{
	   m_clients.push_back(client);
	}

	// Unregister client from receiving events.
	void Unregister(ISocketEvents* client)
	{
	   m_clients.remove(client);
	}

	// set pointer to logger object
	void setLoggerObject(CLogger* cLog);

	void setProtocol(enum AAS_PROTOCOL iProto);
	enum AAS_PROTOCOL getProtocol();

	void setState(enum AAS_STATE iState);
	enum AAS_STATE getState();

	bool getConnected();
	void setIPAddress(CString sIPAddr);
	CString getIPAddress();
	void setPort(UINT iP);
	UINT getPort();

	// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAAsyncSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);

	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CAAsyncSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:

	list<ISocketEvents*> m_clients; // list of client objects to notify
   
	CString	m_strRecv;

	// Raise events
	void NotifyReceive(CString& strBuff, CString& sIPAddress, UINT& iPort);

	CLogger*	pLogger;		// pointer to logger object

	// log message routines
	void LogMessage(CLogger::LGR_SEVERITY, CString & strLogMessage);
	void LogMessage(CLogger::LGR_SEVERITY, char *);

	enum AAS_PROTOCOL ieSocketProtocol;		// protocol of socket
	enum AAS_STATE ieSocketState;			// state of socket

	// for TCP connections 
	bool bConnected;
	CString sIPAddress;						// remote ip address
	UINT iPort;								// remote port number

	// time when closure was requested
	SYSTEMTIME stClosureRequest;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AASYNCSOCKET_H__DC449534_D3D1_49F7_B7F1_B8662624C1AA__INCLUDED_)

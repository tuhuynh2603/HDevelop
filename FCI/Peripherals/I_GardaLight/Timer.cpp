/*   -----------------------------------------------------------------	//
//																		//
//	Copyright (c) 2009 Roslix Ltd										//
//	All rights reserved                   								//
//																		//
//	Roslix Limited														//
//	2 Foscote Cottages													//
//	Abthorpe															//
//	Towcester															//
//	Northants															//
//	NN12 8PB															//
//	UK 																	//
//																		//
//	tel. +44 7836 296253												//
//																		//
//	Email	enquiries@roslix.co.uk										//
//	WWW	http:\\www.roslix.co.uk											//
//																		//
//	-----------------------------------------------------------------	//
//																		//
//																		//
//	Timer.cpp: implementation of the CTimer class						//
//																		//
//	This class is simply an object representation of a Timer Class		//
//	-----------------------------------------------------------------	//
*/

#include "stdafx.h"
#include "Timer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*   -----------------------------------------------------------------	//
//																		//
//	System Timer Mechanism to pass timer events back to Timer			//
//	Objects. An array of callbacks will be maintained by the objects	//
//	and indexed by timer ident.											//
//																		//
//	-----------------------------------------------------------------	//
*/
struct CTimerEvent
{
public:
    CTimer	*pTimerClient;	// object which requested timer
	UINT_PTR	uiTimerIdent;	// timer ident
};

CArray<CTimerEvent, CTimerEvent&> TimerEvents;	// list of Live Timers
bool	bTimerIsInitialised = false;			// Inistialised Flag



/*********************************************************************
  Routine	OnTimerEnd

  Description
  Callback Function to handle timeouts and direct them to the correct
  object member.
  
  Parameters
  1 - HWND hwnd		- handle of window for timer messages
  2 - UINT uMsg		- WM_TIMER message
  3 - UINT idEvent	- timer identifier
  4 - DWORD dwTime	- current system time

  Returns - void
*********************************************************************/
void CALLBACK EXPORT OnTimerEnd(	HWND hwnd,     // handle of window for timer messages
									UINT uMsg,     // WM_TIMER message
									UINT_PTR idEvent,  // timer identifier
									DWORD dwTime)  // current system time
{
	int iReturn;
	CString strMessage;
	CTimer *pCTimer;

	if (!::KillTimer(NULL,idEvent))
		iReturn = AfxMessageBox("KillTimer Failed - " + GetLastError());

	for ( int i = 0; i <= TimerEvents.GetUpperBound(); i++)
	{
		if ( TimerEvents[i].uiTimerIdent == idEvent)
		{
			pCTimer = TimerEvents[i].pTimerClient;
			TimerEvents.RemoveAt(i);		// need to remove before call as called routine may remove lower members
			pCTimer->ProcessTimer(idEvent, dwTime);
			return;
		}
	}

	// not found
	strMessage.Format("Servicing Unidentified Timer - %u", idEvent);
	iReturn = AfxMessageBox(strMessage);
	return;

}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*********************************************************************
  CTimer::CTimer()

  Description
  Simple constructor for CTimer class.

  Parameters
  None
*********************************************************************/

CTimer::CTimer()
{
	try
	{
	    pLogger = NULL;
	
		if (bTimerIsInitialised == false)
		{
			// do initialisation
			TimerEvents.SetSize(0);
			bTimerIsInitialised = true;
		}
	}
	catch (...)
	{
		int iReturn = AfxMessageBox("Error in - Timer Object Initialisation");
	}
}

/*********************************************************************
  CTimer::~CTimer()

  Description
  Simple destructor for CTimer class. Destroys any residual objects.

  Parameters
  None
*********************************************************************/

CTimer::~CTimer()
{
}


/*********************************************************************
  Routine
  CTimer::setLoggerObject

  Description
  Set pointer to logger object. This routine sets a pointer to
  a CLogger object. The logger object should be instantiated by
  the code which instantiates this class. It should be derived from
  (or simply) the supplied CLogger class.

  Parameters
  1 - CLogger* cLog	- pointer to Clogger object.

  Returns - void
*********************************************************************/
void CTimer::setLoggerObject(CLogger* cLog)
{
	pLogger = cLog;	// pointer to logger object
}

/*********************************************************************
  Routine
  CTimer::ProcessTimer

  Description
  Service Timer Routine. Should not be called as should call derived 
  class version.
  
  Parameters
  1 - UINT_PTR idEvent	- timer identifier
  2 - DWORD dwTime	- current system time

  Returns - void.
*********************************************************************/
void CTimer::ProcessTimer(UINT_PTR idEvent,  // timer identifier
							DWORD dwTime)  

// current system time
{
	LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - ProcessTimer");
}


/*********************************************************************
  Routine
  CTimer::KillTimer

  Description
  Get rid of event timer.
  
  Parameters
  1 - UINT* pidEvent	- pointer to id of event.

  Returns - void.
*********************************************************************/
void CTimer::KillTimer(UINT_PTR* pidEvent)
{
	CString strErrorMessage;
	bool	bFound = false;

	if (*pidEvent != 0)
	{
		// kill timer
		if (!::KillTimer(NULL,*pidEvent))
		{
			strErrorMessage.Format("Error - KillTimer Failed - %i", GetLastError());
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strErrorMessage);
		}

		for ( int i = 0; i <= TimerEvents.GetUpperBound(); i++)
		{
			if ( TimerEvents[i].uiTimerIdent == *pidEvent)
			{
				TimerEvents.RemoveAt(i);
				bFound = true;
			}
		}
		
		// clear until next timer
		*pidEvent = 0;

		if (!bFound)
		{
			strErrorMessage.Format("CTimer::KillTimer - Failed to Find Timer in List - %i", *pidEvent);
			LogMessage(CLogger::LGR_SEVERITY_ERROR,strErrorMessage);
		}
	}
}


/*********************************************************************
  Routine
  CTimer::settimer

  Description
  Set up event timer. Sets up a timer and returns the event id to the 
  location passed in the call.
  
  Parameters
  1 - UINT uiMSECS		- number of milliseconds to time for
  2 - UINT_PTR* pidEvent	- pointer to id of event.

  Returns - void.
*********************************************************************/
void CTimer::settimer(UINT uiMSECS, UINT_PTR* pidEvent)
{
	CTimerEvent	timerEvent;

	try
	{
		this->KillTimer(pidEvent); // kill any old timer

		*pidEvent = ::SetTimer(NULL,0,uiMSECS, OnTimerEnd);

		if (*pidEvent == 0)
		{
			CString strErrorMessage;
			DWORD dwError;
			dwError = GetLastError();
			strErrorMessage.Format("CTimer::settimer() - Error returned from SetTimer - %u", dwError);
			LogMessage(CLogger::LGR_SEVERITY_ERROR, strErrorMessage);
		}
		else
		{
			// Register client to receive events
			timerEvent.pTimerClient = this;
			timerEvent.uiTimerIdent = *pidEvent;
			TimerEvents.Add(timerEvent);
		}


	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - settimer");
	}

}


/*********************************************************************
  Routine
  CTimer::LogMessage

  Description
  Log Message and severity. Note the implementation of AddLogMessage
  may be unknown.
  
  Parameters
  1 - CLogger::LGR_SEVERITY iSeverity	- (see enum LGR_SEVERITY)
  2 - char * strLogMsg					- ascii message 

to log

  Returns - void
*********************************************************************/
void CTimer::LogMessage(CLogger::LGR_SEVERITY iSeverity, char * strLogMsg)
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
  CTimer::LogMessage

  Description
  Log Message and severity. Note the implementation of AddLogMessage
  may be unknown.
  
  Parameters
  1 - CLogger::LGR_SEVERITY iSeverity	- (see enum LGR_SEVERITY)
  2 - CString & strLogMessage			- ascii message to log

  Returns - void
*********************************************************************/
void CTimer::LogMessage(CLogger::LGR_SEVERITY iSeverity, CString & strLogMessage)
{
	try
	{
		if (pLogger != NULL)
			pLogger->AddLogMessage(iSeverity, strLogMessage);
	}
	catch(...)
	{}
}




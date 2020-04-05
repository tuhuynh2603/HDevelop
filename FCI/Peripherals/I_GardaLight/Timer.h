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
//	Timer.h: interface for the CTimer class.								//
//																		//
//	-----------------------------------------------------------------	//
*/

#if !defined(AFX_TIMER_H__D593FA72_A197_4281_905E_D4C6392DAF87__INCLUDED_)
#define AFX_TIMER_H__D593FA72_A197_4281_905E_D4C6392DAF87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Logger.h"

class CTimer  
{
public:
	CTimer();
	virtual ~CTimer();

	virtual void ProcessTimer(UINT_PTR idEvent,  // timer identifier
							DWORD dwTime);


	
protected:
	virtual void KillTimer(UINT_PTR* pidEvent);

	virtual void settimer(UINT uiMSECS, UINT_PTR* pidEvent);

	virtual void LogMessage(CLogger::LGR_SEVERITY iSeverity, char * strLogMsg);

	virtual void LogMessage(CLogger::LGR_SEVERITY iSeverity, CString & strLogMessage);

	// set pointer to logger object
	virtual void setLoggerObject(CLogger* cLog);

	CLogger*	pLogger;		// pointer to logger object
};

#endif // !defined(AFX_TIMER_H__D593FA72_A197_4281_905E_D4C6392DAF87__INCLUDED_)





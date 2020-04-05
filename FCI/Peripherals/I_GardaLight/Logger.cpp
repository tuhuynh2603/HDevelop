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
//	Logger.cpp: implementation of the CLogger class						//
//																		//
//	-----------------------------------------------------------------	//
*/

#include "stdafx.h"
#include "Logger.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*********************************************************************
  CLogger::CLogger()

  Description
  Simple constructor for CLogger class. Initilises class data

  Parameters
  None
*********************************************************************/
CLogger::CLogger()
{
}


/*********************************************************************
  CLogger::CLogger()

  Description
  SetSize constructor for CLogger class. Sets size of CStringArray
  on instantiation.

  Parameters
  1 -	UINT uiMaximumMessages	- number of log messages to hold in 
									cyclic buffer.
*********************************************************************/
CLogger::CLogger(UINT uiMaximumMessages)
{
	CLogger::SetSize(uiMaximumMessages);
	iNumberOfLogMessages = 0;
	iNextElement = 0;

	// set up directory for dump log
	strDumpLogDirectory = "C:\\temp\\";
}


/*********************************************************************
  CLogger::~CLogger()

  Description
  Simple destructor for CLogger class. Destroys any residual
  objects.

  Parameters
  None
*********************************************************************/
CLogger::~CLogger()
{

}


/*********************************************************************
  Routine
  CLogger::AddLogMessage

  Description
  Adds a logging message to the CStringArray. The array is handled as 
  a cyclic buffer.

  Parameters
  1 - enum LGR_SEVERITY - severity for message tag
									LGR_SEVERITY_DEBUG,
									LGR_SEVERITY_INFO,
									LGR_SEVERITY_WARNING,
									LGR_SEVERITY_ERROR,
									LGR_SEVERITY_SEVERE,
									LGR_SEVERITY_FATAL
  2 - char * strLogMsg	- ascii log message

  Returns - void
*********************************************************************/
void CLogger::AddLogMessage(enum LGR_SEVERITY iSeverity, char * strLogMsg)
{
	CString strBuff = strLogMsg;
	this->AddLogMessage(iSeverity, strBuff);
}


/*********************************************************************
  Routine
  CLogger::AddLogMessage

  Description
  Adds a logging message to the CStringArray. The array is handled as 
  a cyclic buffer.

  Parameters
  1 - enum LGR_SEVERITY			- severity for message tag
									LGR_SEVERITY_DEBUG,
									LGR_SEVERITY_INFO,
									LGR_SEVERITY_WARNING,
									LGR_SEVERITY_ERROR,
									LGR_SEVERITY_SEVERE,
									LGR_SEVERITY_FATAL
  2 - CString & strLogMessage	- ascii log message

  Returns - void
*********************************************************************/
void CLogger::AddLogMessage(enum LGR_SEVERITY iSeverity, CString & strLogMessage)
{
	CString strBuff;
	char strTDBuff[50];
	char strSeverityBuff[10];

	SYSTEMTIME st;
    GetSystemTime(&st);
    sprintf(strTDBuff, "%02d-%02d-%04d %02d:%02d:%02d.%03d     " ,st.wDay,st.wMonth,st.wYear,st.wHour,st.wMinute,st.wSecond, st.wMilliseconds);

	switch( iSeverity )
	{
	case LGR_SEVERITY_DEBUG:	if (!LOG_DEBUG)		return; else	strcpy( strSeverityBuff, "DEBUG    "); break;
	case LGR_SEVERITY_INFO:		if (!LOG_INFO)		return; else	strcpy( strSeverityBuff, "INFO     "); break;
	case LGR_SEVERITY_WARNING:	if (!LOG_WARNING)	return; else	strcpy( strSeverityBuff, "WARNING  "); break;
	case LGR_SEVERITY_ERROR:	if (!LOG_ERROR)		return; else	strcpy( strSeverityBuff, "ERROR    "); break;
	case LGR_SEVERITY_SEVERE:	if (!LOG_SEVERE)	return; else	strcpy( strSeverityBuff, "SEVERE   "); break;
	case LGR_SEVERITY_FATAL:	if (!LOG_SEVERE)	return; else	strcpy( strSeverityBuff, "FATAL    "); break;
	default:														strcpy( strSeverityBuff, "UNKNOWN  "); 
	}

	strBuff = strLogMessage;
	strBuff = strSeverityBuff + strLogMessage;
	strBuff = strTDBuff + strBuff;

	this->SetAt(iNextElement++,strBuff);

	iNumberOfLogMessages++;

	if (iNumberOfLogMessages >= this->GetSize())
		iNumberOfLogMessages = (int) this->GetSize();

	// if cycled around then 
	if (iNextElement > this->GetUpperBound())
		iNextElement -= iNumberOfLogMessages;			// actual element offset

}


/*********************************************************************
  Routine
  CLogger::GetLogMessage

  Description
  Gets a logging message from the CStringArray. The array is handled as 
  a cyclic buffer. This class will handle indexing such that the client
  only needs to request the message number from the start of the buffer.

  Parameters
  1 - int iMessageNumber	- message offset from the start of the 
								cyclic buffer. (zero indexed)

  Returns - CString & strLogMessage	- ascii log message
*********************************************************************/
CString * CLogger::GetLogMessage(int iMessageNumber)
{
	// if cycled around the 
	if (iNumberOfLogMessages >= this->GetSize())
	{
		iMessageNumber += iNextElement;			// actual element offset
		if (iMessageNumber > this->GetUpperBound())
			iMessageNumber -= (int) this->GetSize();
	}
	
	// check for still above range and if so return NULL
	if ((iMessageNumber > this->GetUpperBound()) || 
		(iMessageNumber < 0))
		return NULL;
	else
		return &this->ElementAt(iMessageNumber);
}


/*********************************************************************
  Routine
  CLogger::NumberOfLogs

  Description
  Report on number of log messages in CStringArray.
  
  Parameters
  None.

  Returns - int		Number of Log Messages in String Array.
*********************************************************************/
int CLogger::NumberOfLogs()
{
	return iNumberOfLogMessages;
}



/*********************************************************************
  Routine
  CLogger::DumpLog

  Description
  Client Request - Dump the contents of the log to file.

  Parameters
  1 - CString strFile	- file name to dump to or "" if to use default
  2 - CString strPath	- path to dump to or "" if to use current

  Returns - bool	- true if successful otherwise false
*********************************************************************/
bool CLogger::DumpLog(CString strFile, const CString strPath)
{
	CString strMessage;
	CString strLogFile;
	SYSTEMTIME st;

	try
	{
		// set up log file name
		GetSystemTime(&st);
		if (strFile == "")
			strFile.Format("Log_%04d%02d%02d%02d%02d%02d%03d.log" , 
							st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		// set up the path
		if (strPath != "")
			strDumpLogDirectory= strPath;

		strLogFile.Format("%s\\%s" ,strDumpLogDirectory, strFile);
	
		if (fileDumpLog.is_open())		// need to open file here
			fileDumpLog.close();		// so first make sure old one is closed
		fileDumpLog.open(strLogFile);
		fileDumpLog.clear();			// and reset eof etc
		// if not open then error else build message
		if (fileDumpLog.is_open() == 0)
		{
			// failed to open so error
			strMessage.Format("Error Failed To Open File - %s",strLogFile);
			AddLogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
			return false;
		}
		else
		{
			for (int iLogIndex = (iNumberOfLogMessages-1) ; iLogIndex >= 0 ; iLogIndex--)
			{
				strMessage.Format("%s\r\n", this->GetLogMessage(iLogIndex)->GetBuffer(1));
				if (!fileDumpLog.write(strMessage.GetBuffer(1), strMessage.GetLength()))
				{
					// failed to write so error
					strMessage.Format("Error Failed To Write to File - %s",strLogFile);
					AddLogMessage(CLogger::LGR_SEVERITY_ERROR,strMessage);
					fileDumpLog.close();	// close file here
					return false;
				}
			}
						
			fileDumpLog.close();		// close file here
			return true;
		}
	}
	catch (...)
	{
		AddLogMessage(CLogger::LGR_SEVERITY_ERROR,"Error in - DumpLog");
		return false;
	}
}

	
/*********************************************************************
  Routine
  CLogger::getDumpDirectory

  Description
  Get Dump Directory.
  
  Parameters
  None

  Returns - CString -	Ascii Log Directory
*********************************************************************/
CString CLogger::getDumpDirectory()
{
	return strDumpLogDirectory;
}

/*********************************************************************
  Routine
  CLogger::setDumpDirectory

  Description
  Set Dump Log Directory.
  
  Parameters
  1 - CString sDD	- ascii Dump Log Directory

  Returns - Void
*********************************************************************/
void CLogger::setDumpDirectory(CString sDD)
{
	strDumpLogDirectory = sDD;
}




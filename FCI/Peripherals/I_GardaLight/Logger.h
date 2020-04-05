#if !defined(AFX_LOGGER_H__478A3449_F964_4B37_B8D7_EEDB2D3F29B3__INCLUDED_)
#define AFX_LOGGER_H__478A3449_F964_4B37_B8D7_EEDB2D3F29B3__INCLUDED_

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
//	Logger.h: interface for the Logger class							//
//																		//
//	-----------------------------------------------------------------	//
*/

#define		LOG_DEBUG				true		// log debug stuff
#define		LOG_INFO				true		// log info stuff
#define		LOG_WARNING				true		// log warning stuff
#define		LOG_ERROR				true		// log error stuff
#define		LOG_SEVERE				true		// log severe stuff
#define		LOG_FATAL				true		// log fatal stuff

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <fstream>
using namespace std;

class CLogger : public 	CStringArray
 
{
public:
	CLogger();
	CLogger(UINT uiMaximumMessages);
	virtual ~CLogger();

	enum LGR_SEVERITY			// Declare enum type for severity
	{
		LGR_SEVERITY_DEBUG = 0,	// Debug trace only
		LGR_SEVERITY_INFO,		// For Information
		LGR_SEVERITY_WARNING,	// Warning, unexpected behaviour
		LGR_SEVERITY_ERROR,		// Error in behaviour
		LGR_SEVERITY_SEVERE,	// Major error in behaviour
		LGR_SEVERITY_FATAL		// Fatal error in behaviour
	};

	void AddLogMessage(enum LGR_SEVERITY, CString & strLogMessage);
	void AddLogMessage(enum LGR_SEVERITY, char *);
	CString * GetLogMessage(int iLogNumber);

	bool DumpLog(const CString strFile, const CString strPath);
	
	CString	getDumpDirectory();
	void	setDumpDirectory(CString sDD);

	int NumberOfLogs();


protected:
//	void AddLogMessage(int iSeverity, CString & strLogMessage);

	int iNumberOfLogMessages;
	int iNextElement;

	CString		strDumpLogDirectory;// directory for log file
	ofstream	fileDumpLog;		// dump log file object

};

#endif // !defined(AFX_LOGGER_H__478A3449_F964_4B37_B8D7_EEDB2D3F29B3__INCLUDED_)

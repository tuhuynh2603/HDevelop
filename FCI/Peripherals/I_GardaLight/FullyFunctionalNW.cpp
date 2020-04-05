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
//	FullyFunctionalNW.cpp: implementation of the CFullyFunctionalNW class.
//																		//
//	This class is simply an object representation of a Controller item	//
//	-----------------------------------------------------------------	//
*/

#include "stdafx.h"
#include "FullyFunctionalNW.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFullyFunctionalNW::CFullyFunctionalNW()
{
	uiTimerIdentTCPIPRefreshRequired = 0;	// process timer for controller requested refresh

	// and start check for refresh request timer
	settimer(FFN_CONTRLLER_REFRESH_SERVICE_TIME, &uiTimerIdentTCPIPRefreshRequired);
}

CFullyFunctionalNW::~CFullyFunctionalNW()
{
}

/*********************************************************************
  Routine
  CFullyFunctionalNW::getFirmwareFileName

  Description
  Get Current Firmware Filename.
  
  Parameters
  1 - const int i	- array index of required controller.

  Returns - CString -	current firmware filename
*********************************************************************/

CString CFullyFunctionalNW::getFirmwareFileName(const int i)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].getFirmwareFileName();
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::getFirmwareFileName - Controller Index Out of Range");
			return "";
		}
	}
	catch (...)
	{
		// failed
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::getFirmwareFileName - Error Detected");
		return "";
	}
}

// set firmware file name
/*********************************************************************
  Routine
  CFullyFunctionalNW::setFirmwareFileName

  Description
  Set Firmware Filename.
  
  Parameters
  1 - HANDLE hFD	- File Descriptor of this controller

  Returns - bool -	True if Request Successful, otherwise False
*********************************************************************/

bool CFullyFunctionalNW::setFirmwareFileName(const int i, const CString &strFFileName)
{
	CString strMessage;
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
		{
			Controllers[i].setFirmwareFileName(strFFileName);
			return true;
		}
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::setFirmwareFileName - Controller Index Out of Range");
			return false;
		}
	}
	catch (...)
	{
		// failed
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::setFirmwareFileName - Error Detected");
		return false;
	}
}

/*********************************************************************
  Routine
  CFullyFunctionalNW::getControllerStatusValues

  Description
  Get Status values for Controller.
  
  Parameters
  1 - const int i						- array index of required controller.
  2 - CString	&strMainState			- current main state description
  3 - CString	&strSubState			- current sub state description
  4 - CString	&strUpdateFileMessage	- current file transfer message
  5 - int		&iPercentageComplete	- current % complete for file transfer

  Returns - bool -	True if Successful, otherwise False
*********************************************************************/

bool CFullyFunctionalNW::getControllerStatusValues(const int i,
											  CString &strMainState,
											  CString &strSubState,
											  CString &strUpdateFileMessage,
											  int &iPercentageComplete,
											  bool &bComplexCmdActive)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].getControllerStatusValues(strMainState,
																  strSubState,
																  strUpdateFileMessage,
																  iPercentageComplete,
																  bComplexCmdActive);
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::getControllerStatusValues - Controller Index Out of Range");
			return false;
		}
	}
	catch (...)
	{
		// failed
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::getControllerStatusValues - Error Detected");
		return false;
	}
}

/*********************************************************************
  Routine
  CFullyFunctionalNW::getLastFirmwareUpgradeSuccessful

  Description
  Get Result for Last Firmware Upgrade Operation for this Controller.
  
  Parameters
  none

  Parameters
  1 - const int i						- array index of required controller.

  Returns - bool -	Result Flag
*********************************************************************/

bool CFullyFunctionalNW::getLastFirmwareUpgradeSuccessful(const int i)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].getLastFirmwareUpgradeSuccessful();
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::getLastFirmwareUpgradeSuccessful - Controller Index Out of Range");
			return false;
		}
	}
	catch (...)
	{
		// failed
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::getLastFirmwareUpgradeSuccessful - Error Detected");
		return false;
	}
}

/*********************************************************************
  Routine
  CFullyFunctionalNW::getLastIPChangeSuccessful

  Description
  Get Result for Last IP Change Operation for this Controller.
  
  Parameters
  none

  Parameters
  1 - const int i						- array index of required controller.

  Returns - bool -	Result Flag
*********************************************************************/

bool CFullyFunctionalNW::getLastIPChangeSuccessful(const int i)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].getLastIPChangeSuccessful();
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::getLastIPChangeSuccessful - Controller Index Out of Range");
			return false;
		}
	}
	catch (...)
	{
		// failed
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::getLastIPChangeSuccessful - Error Detected");
		return false;
	}
}

/*********************************************************************
  Routine
  CFullyFunctionalNW::RequestBootMode

  Description
  Client Request - Put us into boot mode.
  
  Parameters
  1 - const int i			- controller to index

  Returns - bool -	True if Request Successful, otherwise False
							- true does not indicate boot mode is 
								achieved, only that the request is
								accepted.
*********************************************************************/

bool CFullyFunctionalNW::RequestBootMode(const int i)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].RequestBootMode();
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::RequestBootMode - Controller Index Out of Range");
			return false;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::RequestBootMode - Error Detected");
		throw; // and rethrow
	}
}

/*********************************************************************
  Routine
  CFullyFunctionalNW::RequestAppValid

  Description
  Client Request - Put us into app valid state.
  
  Parameters
  1 - const int i			- controller to index

  Returns - bool -	True if Request Successful, otherwise False
							- true does not indicate action is 
								achieved, only that the request is
								initiated.
*********************************************************************/

bool CFullyFunctionalNW::RequestAppValid(const int i)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].RequestAppValid();
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::RequestAppValid - Controller Index Out of Range");
			return false;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::RequestAppValid - Error Detected");
		throw; // and rethrow
	}
}

/*********************************************************************
  Routine
  CFullyFunctionalNW::RequestEraseApp

  Description
  Client Request - Erase App Message
  
  Parameters
  1 - const int i			- controller to index

  Returns - bool -	True if Request Successful, otherwise False
							- true does not indicate action is 
								achieved, only that the request is
								initiated.
*********************************************************************/

bool CFullyFunctionalNW::RequestEraseApp(const int i)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].RequestEraseApp();
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::RequestEraseApp - Controller Index Out of Range");
			return false;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::RequestEraseApp - Error Detected");
		throw; // and rethrow
	}
}

/*********************************************************************
  Routine
  CFullyFunctionalNW::RequestRunApp

  Description
  Client Request - Run App Message
  
  Parameters
  1 - const int i			- controller to index

  Returns - bool -	True if Request Successful, otherwise False
							- true does not indicate action is 
								achieved, only that the request is
								initiated.
*********************************************************************/

bool CFullyFunctionalNW::RequestRunApp(const int i)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].RequestRunApp();
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::RequestRunApp - Controller Index Out of Range");
			return false;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::RequestRunApp - Error Detected");
		throw; // and rethrow
	}
}

/*********************************************************************
  Routine
  CFullyFunctionalNW::RequestLoadApp

  Description
  Client Request - Load App Message
  
  Parameters
  1 - const int i			- controller to index

  Returns - bool -	True if Request Successful, otherwise False
							- true does not indicate action is 
								achieved, only that the request is
								initiated.
*********************************************************************/

bool CFullyFunctionalNW::RequestLoadApp(const int i)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].RequestLoadApp();
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::RequestLoadApp - Controller Index Out of Range");
			return false;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::RequestLoadApp - Error Detected");
		throw; // and rethrow
	}
}

/*********************************************************************
  Routine
  CFullyFunctionalNW::RequestUpgradeFirmware

  Description
  Client Request - Upgrade Firmware (Chained command)
  
  Parameters
  1 - const int i			- controller to index

  Returns - bool -	True if Request Successful, otherwise False
							- true does not indicate action is 
								achieved, only that the request is
								initiated.
*********************************************************************/

bool CFullyFunctionalNW::RequestUpgradeFirmware(const int i)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].RequestUpgradeFirmware();
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::RequestUpgradeFirmware - Controller Index Out of Range");
			return false;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::RequestUpgradeFirmware - Error Detected");
		throw; // and rethrow
	}
}

/*********************************************************************
  Routine
  CFullyFunctionalNW::RequestIPDynamicCommand

  Description
  Client Request - IP Dynamic Command Message
  
  Parameters
  1 - const int i			- controller to index

  Returns - bool -	True if Request Successful, otherwise False
							- true does not indicate action is 
								achieved, only that the request is
								initiated.
*********************************************************************/

bool CFullyFunctionalNW::RequestIPDynamicCommand(const int i)
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].RequestIPDynamicCommand();
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::RequestIPDynamicCommand - Controller Index Out of Range");
			return false;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::RequestIPDynamicCommand - Error Detected");
		throw; // and rethrow
	}
}

/*********************************************************************
  Routine
  CFullyFunctionalNW::RequestIPFixedCommand

  Description
  Client Request - IP Fixed Command Message
  
  Parameters
  1 - const int i			- controller to index

  Returns - bool -	True if Request Successful, otherwise False
							- true does not indicate action is 
								achieved, only that the request is
								initiated.
*********************************************************************/

bool CFullyFunctionalNW::RequestIPFixedCommand(const int i,
											   CString * strIPAddress,
											   CString * strIPMask, 
											   CString * strIPGateway )
{
	try
	{
		// check for in range
		if ((i >= 0) && (i <= this->Controllers.GetUpperBound()))
			return this->Controllers[i].RequestIPFixedCommand(strIPAddress,
											   strIPMask, strIPGateway);
		else
		{
			// failed
			LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::RequestIPFixedCommand - Controller Index Out of Range");
			return false;
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::RequestIPFixedCommand - Error Detected");
		throw; // and rethrow
	}
}

/*********************************************************************
  Routine
  CFullyFunctionalNW::ProcessTimer

  Description
  Service Timer Routine. Used to catch timeouts which are passed here
  by the callback routine which is set up when a system timer is
  started. Will check for requests from the controllers for refresh of
  IP addresses.
  
  Parameters
  1 - UINT idEvent	- timer identifier
  2 - DWORD dwTime	- current system time

  Returns - void.
*********************************************************************/

void CFullyFunctionalNW::ProcessTimer(	UINT_PTR idEvent,  // timer identifier
										DWORD dwTime)  // current system time
{
	CString strMessage;
	bool	bRefreshed=false;
	int		i;

	try
	{
		// if controller requested refresh timer
		if (idEvent == uiTimerIdentTCPIPRefreshRequired)
		{
			uiTimerIdentTCPIPRefreshRequired = 0;

			// for initialised CONTROLLERS
			for (i=0; i<= this->Controllers.GetUpperBound(); i++)
			{
				if (this->Controllers[i].bIPRefreshRequired)
				{
					if (!bRefreshed)
					{
						iRefreshesRemaining_Ethernet++;			// increment allowed tries so as not to disturb pending refreshes
						if (!this->SendRefreshMessage(CMS_COMMSTYPE_ETHERNET))	// do requested refresh
						{
							LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::ProcessTimer - Failed Refresh");
						}
					}

					this->Controllers[i].bIPRefreshRequired = false;
				}
			}

			// and restore timer
			settimer(FFN_CONTRLLER_REFRESH_SERVICE_TIME, &uiTimerIdentTCPIPRefreshRequired);

		}
		else
		{
			// invoke base class timer handler
			CControllerNW::ProcessTimer(idEvent, dwTime);
		}
	}
	catch (...)
	{
		LogMessage(CLogger::LGR_SEVERITY_ERROR,"CFullyFunctionalNW::ProcessTimer() - Error Detected");
	}
}

// FullyFunctionalNW.h: interface for the CFullyFunctionalNW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FULLYFUNCTIONALNW_H__33AE898A_1EC5_443F_858D_CAA27CF33E32__INCLUDED_)
#define AFX_FULLYFUNCTIONALNW_H__33AE898A_1EC5_443F_858D_CAA27CF33E32__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ControllerNW.h"

class CFullyFunctionalNW : public CControllerNW  
{
public:
	CFullyFunctionalNW();
	virtual ~CFullyFunctionalNW();

	bool		setFirmwareFileName(const int i, const CString &strFFileName);
	CString		getFirmwareFileName(const int i);

	bool		getControllerStatusValues(const int i,
										  CString &strMainState,
										  CString &strSubState,
										  CString &strUpdateFileMessage,
										  int &iPercentageComplete,
										  bool &bComplexCmdActive);

	bool		getLastFirmwareUpgradeSuccessful(const int i);
	bool		getLastIPChangeSuccessful(const int i);

	// Client Request - routines
	bool RequestBootMode(const int i);
	bool RequestAppValid(const int i);
	bool RequestRunApp(const int i);
	bool RequestEraseApp(const int i);
	bool RequestLoadApp(const int i);
	bool RequestUpgradeFirmware(const int i);
	bool RequestIPDynamicCommand(const int i);
	bool RequestIPFixedCommand(const int i, CString * strIPAddress, 
								CString * strIPMask, CString * strIPGateway );

	virtual void	ProcessTimer(									// Service Timer Routine
									UINT_PTR idEvent,					// timer identifier
									DWORD dwTime);					// current system time

protected:

#define		FFN_CONTRLLER_REFRESH_SERVICE_TIME		200			// retry time (MSecs) for serial refresh request

	UINT_PTR	uiTimerIdentTCPIPRefreshRequired;	// process timer for impromptu controller requested refresh

};

#endif // !defined(AFX_FULLYFUNCTIONALNW_H__33AE898A_1EC5_443F_858D_CAA27CF33E32__INCLUDED_)

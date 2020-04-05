/// GardaLight: GardaSoft Light Control interface
/// Udupa; May 2012;

#include "stdafx.h"
#include "GardaLight.h"
#include "IOCode.h"
#include "PerformanceTimer.h"
#include "DebugLog.h"

#include "FullyFunctionalNW.h"

int gControllerId[5];
CFullyFunctionalNW* pControllerNW = NULL;

int _stdcall InitIODLL(long lReserved)
{
	if(!pControllerNW)
		pControllerNW = new CFullyFunctionalNW();

	for(int i=0; i<5; i++)
		gControllerId[i] = -1;

	if(!pControllerNW->RefreshControllerList()) {
		OutputDebugLogTo(0, TRUE, "[GardaLight] Error - RefreshControllerList failed **********");
		return -1;
	}
	OutputDebugLogTo(0, TRUE, "[GardaLight] RefreshControllerList succeeded");

	return 0;
}

int _stdcall FreeIODLL(long lReserved)
{
	if(pControllerNW)
		delete pControllerNW;

	return 0;
}

int _stdcall RegIOCard(long lCardNo, long lReserved)
{

	return 0;
}

int _stdcall FreeIOCard(long lCardNo,long lReserved)
{

	return 0;
}

///Udupa; May'2012; Timer fix
int _stdcall ConfigIOPortLight(long lCardNo, CString lPortID, long lIOMode, long lReserved)
{
	MSG msg;
	CPerformanceTimer timer;
	int nTime, nLastTime = 0;
	int nTimeout = 2000;

	CString pstrSlNo = lPortID;
	OutputDebugLogTo(0, TRUE, "");
	OutputDebugLogTo(0, TRUE, "[GardaLight] Registering Controller (%s)", pstrSlNo);
	int nFoundControllers = 0;
	while(TRUE) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			if(msg.message) {
				GetMessage(&msg, NULL, 0, 0);
				DispatchMessage(&msg);
			}
		}
		else
			Sleep(10);

		nTime = (int) timer.GetElapsedMSec();
		if((nTime - nLastTime) > 50) {
			int nControllers = pControllerNW->NumberOfControllers();
			if(nControllers > nFoundControllers) {
				///OutputDebugLogTo(0, TRUE, "[GardaLight] No. of controllers: %d", nControllers);

				CString strAddress, strSlNo;
				for(int i=0; i<nControllers; i++) {
					CString strAddress = inet_ntoa(pControllerNW->getIPAddress(i));
					strSlNo = pControllerNW->getSerialNumber(i);
					///OutputDebugLogTo(0, TRUE, "[GardaLight] [%d] Serial No.: %s;  I.P.Address: %s", i, strSlNo, strAddress);
					if(strSlNo.CompareNoCase(pstrSlNo)==0) {
						gControllerId[lCardNo] = i;
						pControllerNW->setProtocol(i, "TCP");
						OutputDebugLogTo(0, TRUE, "[GardaLight] Index: %d;  Found Serial No.: %s", i, strSlNo);
						return 0;
					}
				}
				nFoundControllers = nControllers;
			}
			nLastTime = nTime;
		}

		if(nTime > nTimeout)
			break;
	}

	return -1;
}

int _stdcall FreeIOPort(long lCardNo,long lPortID,long lReserved)
{
	return 0;
}

int _stdcall OutPortWriteString(long lCardNo, long lPortID, CString* strData, long lReserved)
{
	int nController = gControllerId[lCardNo];
	if(nController==-1) {
		OutputDebugLogTo(4, TRUE, "[GardaLight] Error - OutPortWriteString(%d, %s) Invalid Controller-ID**********", 
			lCardNo, *strData);
		return -1;
	}

	try {
		int iMsgStatus;
		CString strMessage = *strData;
		if(!pControllerNW->RequestSendCommand(nController, iMsgStatus, &strMessage, 2000, 2)) {
			OutputDebugLogTo(4, TRUE, "[GardaLight] Error - OutPortWriteString->RequestSendCommand(%d, %s) failed (Status: %d)**********", 
				nController, strMessage, iMsgStatus);
			return -1;
		}
		else {
			bool bReceivedError;

			MSG msg;
			CPerformanceTimer timer;
			BOOL bSuccess = FALSE;
			int nTime, nLastTime = 0;

			while(TRUE) {
				if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
					if(msg.message) {
						GetMessage(&msg, NULL, 0, 0);
						DispatchMessage(&msg);
					}
				}
				else
					Sleep(10);

				nTime = (int) timer.GetElapsedMSec();
				if((nTime - nLastTime) > 50) {
					if(pControllerNW->RequestReceivedMessage(nController, iMsgStatus, *strData, bReceivedError)) {
						CString str = strMessage.Left(2);
						if((str.CompareNoCase("ST")==0) || (str.CompareNoCase("GR")==0) || (strData->Find(">") > 0)) {
							bSuccess = TRUE;
							break;
						}
					}
					else
						break;

					nLastTime = nTime;
				}

				if(nTime > 2000)
					break;
			}
			if(!bSuccess) {
				OutputDebugLogTo(4, TRUE, "[GardaLight] Error - Write->RequestReceivedMessage(%d, %s) failed (Message: %s Status: %d) **********", 
					nController, strMessage, *strData, iMsgStatus);
				return -1;
			}

			OutputDebugLogTo(4, TRUE, "[GardaLight] OutPortWriteString(%d, %s) succeeded", nController, strMessage);
		}
	}
	catch(...) {
		OutputDebugLogTo(4, TRUE, "[GardaLight] Error - Exception in OutPortWriteString **********");
	}

	return 0;
}

int _stdcall InPortRead(long lCardNo,long lPortID,long *plData,long lReserved)
{
	return 0;
}

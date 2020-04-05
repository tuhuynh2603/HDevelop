#include "stdafx.h"
#include "IO_Interface.h"
#include "IOCode.h"

#include "Utilities.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CIOInterface::CIOInterface()
{
	hDLL = 0;

	lpInitIODLL = 0;
	lpFreeIODLL = 0;

	lpRegisterCard = 0;
	lpFreeCard = 0;

	lpConfigIOPort = 0;
    lpConfigIOPortLight = 0;
	lpConfigCOMPort = 0;
	lpFreeIOPort = 0;
	
	lpOutPortWrite = 0;
	lpOutPortWriteBit = 0;
	lpOutPortWriteString = 0;
	lpOutPortRead = 0;
	
	lpSetDIInterrupt=0;
	lpWaitForActiveDIInterrupt=0;

	lpClearActiveDIInterrupt = 0;

	lpInPortRead = 0;
	lpInPortReadString = 0;
}

int CIOInterface::LoadIODll(CString strIOCard)
{
	DWORD dwErr=0;

	OutputDebugLog("Registering IO Card ...");

	if(strIOCard.CompareNoCase("NONE")!=0) {
		hDLL = AfxLoadLibrary(strIOCard);

		if(hDLL==NULL) {
			CString str;
			str.Format("Error Loading IO Card library %s", strIOCard);
			AfxMessageBox(str, MB_OK | MB_ICONERROR);
			return -1;
		}

		lpInitIODLL = (INIT_IO_DLL) GetProcAddress (hDLL,"InitIODLL");
		if(lpInitIODLL==NULL)
			OutputDebugLog("Fail to register IO Card InitIODLL() Function:%d", GetLastError());

		lpFreeIODLL = (FREE_IO_DLL) GetProcAddress (hDLL,"FreeIODLL");
		if(lpFreeIODLL==NULL)
			OutputDebugLog("Fail to register IO Card FreeIODLL() Function:%d", GetLastError());

		lpRegisterCard = (REG_IO_CARD) GetProcAddress (hDLL,"RegisterCard");
		if(lpRegisterCard==NULL)
			OutputDebugLog("Fail to register RegisterCard() Function:%d", GetLastError());

		lpFreeCard = (FREE_IO_CARD) GetProcAddress (hDLL,"FreeCard");
		if(lpFreeCard==NULL)
			OutputDebugLog("Fail to register FreeCard() Function:%d", GetLastError());

		lpConfigIOPort = (CONFIG_IO_PORT) GetProcAddress (hDLL,"ConfigIOPort");
		if(lpConfigIOPort==NULL)
			OutputDebugLog("Fail to register ConfigIOPort() Function:%d", GetLastError());

        lpConfigIOPortLight = (CONFIG_IO_PORT_LIGHT) GetProcAddress (hDLL,"ConfigIOPortLight");
		if(lpConfigIOPortLight==NULL)
			OutputDebugLog("Fail to register ConfigIOPort() Function:%d", GetLastError());

		lpConfigCOMPort = (CONFIG_COM_PORT) GetProcAddress (hDLL,"ConfigCOMPort");
		if(lpConfigCOMPort==NULL)
			OutputDebugLog("Fail to register ConfigCOMPort() Function:%d", GetLastError());

		lpFreeIOPort = (FREE_IO_PORT) GetProcAddress (hDLL,"FreeIOPort");
		if(lpFreeIOPort==NULL)
			OutputDebugLog("Fail to register FreeIOPort() Function:%d", GetLastError());

		lpOutPortWrite = (OUTPORT_WRITE) GetProcAddress (hDLL,"OutPortWrite");
		if(lpOutPortWrite==NULL)
			OutputDebugLog("Fail to register OutPortWrite() Function:%d", GetLastError());

		lpOutPortWriteBit = (OUTPORT_WRITE_BIT) GetProcAddress (hDLL,"OutPortWriteBit");
		if(lpOutPortWriteBit==NULL)
			OutputDebugLog("Fail to register OutPortWriteBit() Function:%d", GetLastError());

		lpOutPortWriteString = (OUTPORT_WRITE_STRING) GetProcAddress (hDLL,"OutPortWriteString");
		if(lpOutPortWriteString==NULL)
			OutputDebugLog("Fail to register OutPortWriteString() Function:%d", GetLastError());

		lpOutPortRead = (OUTPORT_READ) GetProcAddress (hDLL,"OutPortRead");
		if(lpOutPortRead==NULL)
			OutputDebugLog("Fail to register OutPortRead() Function:%d", GetLastError());


		lpInPortRead = (INPORT_READ) GetProcAddress (hDLL,"InPortRead");
		if(lpInPortRead==NULL)
			OutputDebugLog("Fail to register InPortRead() Function:%d", GetLastError());

		lpInPortReadString = (INPORT_READ_STRING) GetProcAddress (hDLL,"InPortReadString");
		if(lpInPortReadString==NULL)
			OutputDebugLog("Fail to register InPortReadString() Function:%d", GetLastError());

		lpInPortAbortRead = (INPORT_ABORT_READ) GetProcAddress (hDLL,"InPortAbortRead");
		if(lpInPortAbortRead==NULL)
			OutputDebugLog("Fail to register InPortAbortRead() Function:%d", GetLastError());

		lpSetDIInterrupt = (SET_DI_INTERRUPT) GetProcAddress (hDLL,"SetDIInterrupt");
		if(lpSetDIInterrupt==NULL)
			OutputDebugLog("Fail to register SetDIInterrupt() Function:%d", GetLastError());

		lpWaitForActiveDIInterrupt = (WAIT_FOR_ACTIVE_DI_INTERRUPT_IO) GetProcAddress (hDLL,"WaitForActiveDIInterrupt");
		if(lpWaitForActiveDIInterrupt==NULL)
			OutputDebugLog("Fail to register WaitForActiveDIInterrupt() Function:%d", GetLastError());

		lpClearActiveDIInterrupt = (CLEAR_ACTIVE_DI_INTERRUPT) GetProcAddress (hDLL, "ClearActiveDIInterrupt");
		if(lpClearActiveDIInterrupt==NULL)
			OutputDebugLog("Fail to register ClearActiveDIInterrupt() Function:%d", GetLastError());

		if(lpInitIODLL)
			lpInitIODLL(0);
	}
	else {
		OutputDebugLog("No IO Card");
	}
	return 0;
}

int CIOInterface::RegisterCard(long lCardNo, long lReserved)
{
	int nError;

	if(lpRegisterCard) {
		nError = lpRegisterCard(lCardNo,lReserved);
		if(nError) {
			AfxMessageBox("Fail to Register IO Card");
			return nError;
		}
		else
			OutputDebugLog("Registered IO Card");
	}
	else
		OutputDebugLog("No RegisterCard() Function");

	return 0;
}

int CIOInterface::FreeCard(long lCardNo, long lReserved)
{
	int nError;

	if(lpFreeCard) {
		nError = lpFreeCard(lCardNo,lReserved);
		if(nError) {
			///AfxMessageBox("Fail to Free IO Card");
			return nError;
		}
		else
			OutputDebugLog("Freed IO Card");
	}
	else 
		OutputDebugLog("No FreeCard() Function");

	return 0;
}

int CIOInterface::ConfigIOPort(long lCardNo, long lPortID, long lIOMode, long lReserved)
{
	int nError;
	CString strMsg;

	if(lpConfigIOPort) {
		nError = lpConfigIOPort(lCardNo,lPortID,lIOMode,lReserved);
		if(nError) {
			//strMsg.Format("Failed to Configure Card=%ld,PortID=%ld, lIOMode=%ld",
			//				lCardNo,lPortID,lIOMode);
			//AfxMessageBox(strMsg);
			return nError;
		}
		else {
			//strMsg.Format("Configured Card=%ld,PortID=%ld, lIOMode=%ld",
			//				lCardNo,lPortID,lIOMode);
			//OutputDebugLog(strMsg.GetBuffer(0));
		}
	}
	return 0;
}

int CIOInterface::ConfigIOPortLight(long lCardNo, CString lPortID, long lIOMode, long lReserved)
{
	int nError;
	CString strMsg;

	if(lpConfigIOPortLight) {
		nError = lpConfigIOPortLight(lCardNo,lPortID,lIOMode,lReserved);
		if(nError) {
			//strMsg.Format("Failed to Configure Card=%ld,PortID=%ld, lIOMode=%ld",
			//				lCardNo,lPortID,lIOMode);
			//AfxMessageBox(strMsg);
			return nError;
		}
		else {
			//strMsg.Format("Configured Card=%ld,PortID=%ld, lIOMode=%ld",
			//				lCardNo,lPortID,lIOMode);
			//OutputDebugLog(strMsg.GetBuffer(0));
		}
	}
	return 0;
}

int CIOInterface::ConfigCOMPort(long lCardNo, long lPortID, long lBaudRate, long lReserved)
{
	int nError;
	CString strMsg;

	if(lpConfigCOMPort) {
		nError = lpConfigCOMPort(lCardNo, lPortID, lBaudRate, lReserved);
		if(nError) {
			strMsg.Format("Failed to Configure Card=%ld,PortID=%ld, lBaudRate=%ld",
							lCardNo, lPortID, lBaudRate);
			AfxMessageBox(strMsg);
			return nError;
		}
		else {
			strMsg.Format("Configured Card=%ld,PortID=%ld, lBaudRate=%ld",
							lCardNo, lPortID, lBaudRate);
			OutputDebugLog(strMsg.GetBuffer(0));
		}
	}
	return 0;
}

int CIOInterface::FreeIOPort(long lCardNo, long lPortID, long lReserved)
{
	if(lpFreeIOPort)
		return lpFreeIOPort(lCardNo,lPortID,lReserved);

	return 0;
}

int CIOInterface::OutPortWrite(long lCardNo, long lPortID, long lData, long lReserved)
{
	if(lpOutPortWrite)
		return lpOutPortWrite(lCardNo,lPortID,lData,lReserved);

	return 0;
}

int CIOInterface::OutPortWriteBit(long lCardNo, long lPortID, long lBitPos, long lData)
{
	if(lpOutPortWriteBit)
		return lpOutPortWriteBit(lCardNo, lPortID, lBitPos, lData);

	return 0;
}

int CIOInterface::OutPortWriteString(long lCardNo, long lPortID, CString* pstrData, long lReserved)
{
	if(lpOutPortWriteString)
		return lpOutPortWriteString(lCardNo, lPortID, pstrData, lReserved);

	return 0;
}

int CIOInterface::OutPortRead(long lCardNo, long lPortID, long *plData, long lReserved)
{
	if(lpOutPortRead)
		return lpOutPortRead(lCardNo,lPortID,plData,lReserved);

	return 0;
}

int CIOInterface::InPortRead(long lCardNo, long lPortID, long *plData, long lReserved)
{
	if(lpInPortRead)
		return lpInPortRead(lCardNo,lPortID,plData,lReserved);

	return 0;
}

int CIOInterface::InPortReadString(long lCardNo, long lPortID, CString *strMessage, long lReserved)
{
	if(lpInPortReadString)
		return lpInPortReadString(lCardNo, lPortID, strMessage, lReserved);

	return 0;
}

int CIOInterface::SetDIInterrupt(long lCardNo, long lPortID, long lIntMask, long lReserved)
{
	if(lpSetDIInterrupt)
		return lpSetDIInterrupt(lCardNo,lPortID,lIntMask,lReserved);

	return 0;
}

int CIOInterface::WaitForActiveDIInterrupt(long lCardNo, long lPortID, long lData, long lBitPos)
{
	if(lpWaitForActiveDIInterrupt)
		return lpWaitForActiveDIInterrupt(lCardNo,lPortID,lData,lBitPos);

	return 0;
}

int CIOInterface::ClearActiveDIInterrupt(long lBitPos)
{
	if(lpClearActiveDIInterrupt)
		return lpClearActiveDIInterrupt(lBitPos);

	return 0;
}

long CIOInterface::GetIOPortID(CString strPortName) 
{
	if(strPortName.CompareNoCase("PORT_1A")==0) {
		return PORT_1A;
	}
	else if(strPortName.CompareNoCase("PORT_1B")==0) {
		return PORT_1B;
	}
	else if(strPortName.CompareNoCase("PORT_1C")==0) {
		return PORT_1C;
	}
	else if(strPortName.CompareNoCase("PORT_2A")==0) {
		return PORT_2A;
	}
	else if(strPortName.CompareNoCase("PORT_2B")==0) {
		return PORT_2B;
	}
	else if(strPortName.CompareNoCase("PORT_2C")==0) {
		return PORT_2C;
	}
	else if(strPortName.CompareNoCase("PORT_3A")==0) {
		return PORT_3A;
	}
	else if(strPortName.CompareNoCase("PORT_3B")==0) {
		return PORT_3B;
	}
	else if(strPortName.CompareNoCase("PORT_3C")==0) {
		return PORT_3C;
	}
	else if(strPortName.CompareNoCase("PORT_4A")==0) {
		return PORT_4A;
	}
	else if(strPortName.CompareNoCase("PORT_4B")==0) {
		return PORT_4B;
	}
	else if(strPortName.CompareNoCase("PORT_4C")==0) {
		return PORT_4C;
	}
	else if(strPortName.CompareNoCase("NONE")==0) {
		return 0;
	}
	else {
		return -1;
	}
}

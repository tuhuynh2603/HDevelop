#include "StdAfx.h"
#include "PCI7230.h"
#include "IOCode.h"
#include "PerformanceTimer.h"
#include "DebugLog.h"

int g_nCardID[5];
HANDLE g_hIntEvent[2];

int ConvertPortID(long lPortID)
{
	int nPortID;

	switch (lPortID) {
	case PORT_1A :
		nPortID = Channel_P1A;
		break;

	case PORT_1B :
		nPortID = Channel_P1B;
		break;

	case PORT_1C :
		nPortID = Channel_P1C;
		break;

	case PORT_2A :
		nPortID = Channel_P2A;
		break;

	case PORT_2B :
		nPortID = Channel_P2B;
		break;

	case PORT_2C :
		nPortID = Channel_P2C;
		break;
	
	case PORT_3A :
		nPortID = Channel_P3A;
		break;

	case PORT_3B :
		nPortID = Channel_P3B;
		break;

	case PORT_3C :
		nPortID = Channel_P3C;
		break;

	case PORT_4A :
		nPortID = Channel_P4A;
		break;

	case PORT_4B :
		nPortID = Channel_P4B;
		break;

	case PORT_4C :
		nPortID = Channel_P4C;
		break;

	default :
		nPortID = -1;
		break;
	}
	return nPortID;
}

int	ConvertIOMode(long lIOMode)
{
	int nPortDir;

	switch (lIOMode) {
	case IO_MODE_IN :
		nPortDir = INPUT_PORT;
		break;

	case IO_MODE_OUT :
		nPortDir = OUTPUT_PORT;
		break;

	default :
		nPortDir = -1;
		break;
	}
	return nPortDir;
}

int _stdcall InitIODLL(long lReserved)
{
	for(int i=0;i<5;i++)
		g_nCardID[i] = -1;

	OutputDebugLogTo(0,  TRUE, "Init I_7230 x64 DLL");
	return 0;
}

int _stdcall FreeIODLL(long lReserved)
{
	for(int i=0;i<5;i++)
		g_nCardID[i] = -1;

	return 0;
}

int _stdcall RegisterCard(long lCardNo, long lReserved)
{
	int nCardNo = lCardNo;

	int nError = Register_Card(PCI_7230, nCardNo);
	if(nError < 0)
		return -1;
	else {
		g_nCardID[nCardNo] = nError;
		return 0;
	}
}

int _stdcall FreeCard(long lCardNo, long lReserved)
{
	int nCardNo = g_nCardID[lCardNo];
	int nError = -1;
	if(nCardNo >= 0)
		nError = Release_Card(nCardNo);

	if(nError < 0)
		return -1;
	else {
		g_nCardID[lCardNo] = -1;
		return 0;
	}
}

int _stdcall ConfigIOPort(long lCardNo, long lPortID, long lIOMode, long lReserved)
{
	return 0;
}

int _stdcall FreeIOPort(long lCardNo, long lPortID, long lReserved)
{
	return 0;
}

int _stdcall OutPortWrite(long lCardNo, long lPortID, long lData, long lReserved)
{
	int nPortID;
	U32 u32Data;

	if(lCardNo > 1)
		lCardNo = 0;

	int nCardNo = g_nCardID[lCardNo];

	nPortID = ConvertPortID(lPortID);
	if(nPortID == -1)
		return -1;

	u32Data = (U32) lData;

	int nError = DO_WritePort(nCardNo, nPortID, u32Data);
	if(nError == NoError)
		return 0;
	else
		return -1;
}

int _stdcall OutPortWriteBit(long lCardNo, long lPortID, long lBitPos, long lData)
{
	int nPortID;
	if(lCardNo > 1)
		lCardNo = 0;

	int nCardNo = g_nCardID[lCardNo];

	nPortID = ConvertPortID(lPortID);
	if(nPortID == -1)
		return -1;

	int nError = DO_WriteLine(nCardNo, nPortID, (U16) lBitPos, (U16) lData);
	if(nError == NoError)
		return 0;
	else
		return -1;
}

int _stdcall InPortRead(long lCardNo, long lPortID, long *plData, long lReserved)
{
	int nPortID;

	int nCardNo = g_nCardID[lCardNo];
	nPortID = ConvertPortID(lPortID);
	if(nPortID == -1)
		return -1;

	U32 u32Data = 0;
	int nError = DI_ReadPort(nCardNo, nPortID, &u32Data);
	if(nError == NoError) {
		*plData = (long) u32Data;
		return 0;
	}
	else
		return -1;
}

int _stdcall SetDIInterrupt(long lCardNo, long lPortID, long lIntMask, long lReserved)
{
	int nCardNo = g_nCardID[(int)lCardNo];
	
	int nError = DIO_SetDualInterrupt(nCardNo, (lIntMask & 1) ? INT1_EXT_SIGNAL : INT1_DISABLE, (lIntMask & 2) ? INT2_EXT_SIGNAL : INT2_DISABLE, g_hIntEvent);
	if(nError == NoError) {
		OutputDebugLogTo(0, TRUE, "[PCI7230] Interrupt set to IDI-0x%x", lIntMask);
		return 0;
	}
	else
		return -1;
}

int _stdcall WaitForActiveDIInterrupt(long lCardNo, long lPortID, long lData, long lBitPos)
{
	unsigned short nPortID;
	CString strMsg;

	nPortID = ConvertPortID(lPortID);
	if(nPortID == -1)
		return -1;

	if(WaitForSingleObject(g_hIntEvent[lBitPos], (DWORD)lData) == WAIT_OBJECT_0) {
		ResetEvent(g_hIntEvent[lBitPos]);
		return 0;
	}

	return -1;
}

int _stdcall ClearActiveDIInterrupt(long lBitPos)
{
	CString strMsg;
	ResetEvent(g_hIntEvent[lBitPos]);

	return -1;
}

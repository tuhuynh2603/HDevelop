// ADSInterface.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "AppDoc.h"
#include "ADSInterface.h"
#include "MainFrame.h"
#include "StatisticsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CADSInterface
CADSInterface::IPI_In_Common  CADSInterface::s_IPI_In_Common = {0,0,false,false};
//														
//CADSInterface::IPI_In_Start  CADSInterface::s_IPI_In_Start = {false,false,false,false,false};

CADSInterface::IPI_In_First_Lens  CADSInterface::s_IPI_In_First_Lens = {false,false,false,false,false};

CADSInterface::IPI_In_Tool  CADSInterface::s_IPI_In_Tool = {0,0,0,0,0};

CADSInterface::IPI_In_Mold  CADSInterface::s_IPI_In_Mold = {0,0,0,0,0};

CADSInterface::IPI_In_Arm  CADSInterface::s_IPI_In_Arm = {0,0,0,0,0};

CADSInterface::IPI_In_Lot_Number1  CADSInterface::s_IPI_In_Lot_Number1 = {""};

CADSInterface::IPI_In_Lot_Number2  CADSInterface::s_IPI_In_Lot_Number2 = {""};

CADSInterface::IPI_In_Lot_Number3  CADSInterface::s_IPI_In_Lot_Number3 = {""};

CADSInterface::IPI_In_Lot_Number4  CADSInterface::s_IPI_In_Lot_Number4 = {""};

CADSInterface::IPI_In_Lot_Number5  CADSInterface::s_IPI_In_Lot_Number5 = {""};

CADSInterface::IPI_In_Color1  CADSInterface::s_IPI_In_Color1 = {""};

CADSInterface::IPI_In_Color2  CADSInterface::s_IPI_In_Color2 = {""};

CADSInterface::IPI_In_Color3  CADSInterface::s_IPI_In_Color3 = {""};

CADSInterface::IPI_In_Color4  CADSInterface::s_IPI_In_Color4 = {""};

CADSInterface::IPI_In_Color5  CADSInterface::s_IPI_In_Color5 = {""};

CADSInterface::IPI_In_Type1  CADSInterface::s_IPI_In_Type1 = {""};

CADSInterface::IPI_In_Type2  CADSInterface::s_IPI_In_Type2 = {""};

CADSInterface::IPI_In_Type3  CADSInterface::s_IPI_In_Type3 = {""};

CADSInterface::IPI_In_Type4  CADSInterface::s_IPI_In_Type4 = {""};

CADSInterface::IPI_In_Type5  CADSInterface::s_IPI_In_Type5 = {""};
CADSInterface::CADSInterface()
{
	m_bADSOpen = false;
	m_nADSPort = 0;

	m_bhIPI_Common = 0;
	m_bhChangeNotifyCommon = 0;

	//m_bhIPI_Start = 0;
	//m_bhChangeNotifyStart = 0;

	m_bhIPI_First_Lens = 0;
	m_bhChangeNotifyFirst_Lens = 0;

	m_bhIPI_Tool = 0;
	m_bhChangeNotifyTool = 0;

	m_bhIPI_Mold = 0;
	m_bhChangeNotifyMold = 0;

	m_bhIPI_Arm = 0;
	m_bhChangeNotifyArm = 0;

	m_bhIPI_Lot_Nr1 = 0;
	m_bhChangeNotifyLot_Nr1 = 0;

	m_bhIPI_Lot_Nr2 = 0;
	m_bhChangeNotifyLot_Nr2 = 0;

	m_bhIPI_Lot_Nr3 = 0;
	m_bhChangeNotifyLot_Nr3 = 0;

	m_bhIPI_Lot_Nr4 = 0;
	m_bhChangeNotifyLot_Nr4 = 0;

	m_bhIPI_Lot_Nr5 = 0;
	m_bhChangeNotifyLot_Nr5 = 0;

	nTrackIdx = 0;

	AdsSyncSetTimeout(5000);

	m_pCLApp = (CApplication*) AfxGetApp();
	
	m_bLotChanged = false;

	for(int i=0;i<5;i++)
		bAmsData[i] = false;

	m_bADSAutoOpen = FALSE;
	
	m_nPrevADSState = 5;

}

CADSInterface::~CADSInterface()
{
	CSingleLock(&m_csADS,TRUE);

	ClosePort();
	if(m_nADSPort !=0)
		AdsPortClose();

	m_bADSOpen = false;


}

////////////////////////////////////////////////////////////////////////////////
////////This function is to Update the BUSY Signal after getting the start signal from the PLC. 
////////////////////////////////////////////////////////////////////////////////

/*int CADSInterface::UpdateADSBusySignal(AmsAddr *pAddr, ULONG *plAmsHdlVar, int nTrack,int nStatus)
{

	CPerformanceTimer WriteTimer;
	WriteTimer.Reset();
	int nError;
	nError=0;
//	CSingleLock(&m_csADS,TRUE);


		bAmsData[nTrack] = true;
		nError = AdsSyncWriteReq(&m_PlcAddress, ADSIGRP_SYM_VALBYHND, lAmsHdlVar[nTrack], sizeof(bAmsData[nTrack]), &bAmsData[nTrack]); 
		if(nError)
			OutputDebugLogTo(8,TRUE,"Error in Writing Busy HIGH");
		else
			OutputDebugLogTo(8,TRUE,"Write Busy HIGH Successful");
	

	return 0;

}*/

void CADSInterface::SetTransTiming(int nMaxDelay, int nCycleTime, int nTimeout)
{	
	m_nMaxDelay  = nMaxDelay;
	m_nCycleTime = nCycleTime;
	AdsSyncSetTimeout(nTimeout);
}


BOOL CADSInterface::SetRemoteAddress(BYTE bNetID[6], unsigned int nPort)
{
	bool bConnected = m_bADSOpen;

	if(bConnected)
		ClosePort();

	AmsNetId NetID;

	for(int i=0;i<6;i++)
		NetID.b[i] = bNetID[i];

	m_nADSPort = AdsPortOpen();

	int nErr = AdsGetLocalAddress(&m_PlcAddress);
	  
	if(nErr)
		OutputDebugLogTo(8,TRUE,"Error: AdsGetLocalAddress code:%d", nErr);

	m_PlcAddress.port  = nPort;//AMSPORT_R0_PLC_RTS1;
	m_PlcAddress.netId = NetID;
	
	if(bConnected)
		return OpenPort();
	else
		return TRUE;
}

///////////////////////////////////////////////////////////////////////////
//// ADS Callback : called on Changes in the IPI_In_Common structure
//////////////////////////////////////////////////////////////////////////

void _stdcall CADSInterface::OnPlcNotifyCommon(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Common IPI_In_CommonOld = s_IPI_In_Common;
		// here we obtain the new values
		s_IPI_In_Common = *((IPI_In_Common*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Common(IPI_In_CommonOld);
	}
}
void _stdcall CADSInterface::OnPlcNotifyColor1(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Color1 IPI_In_Color1_Old = s_IPI_In_Color1;
		// here we obtain the new values
		s_IPI_In_Color1 = *((IPI_In_Color1*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Color1(IPI_In_Color1_Old);
	}
}

void _stdcall CADSInterface::OnPlcNotifyColor2(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Color2 IPI_In_Color2_Old = s_IPI_In_Color2;
		// here we obtain the new values
		s_IPI_In_Color2 = *((IPI_In_Color2*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Color2(IPI_In_Color2_Old);
	}
}

void _stdcall CADSInterface::OnPlcNotifyColor3(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Color3 IPI_In_Color3_Old = s_IPI_In_Color3;
		// here we obtain the new values
		s_IPI_In_Color3 = *((IPI_In_Color3*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Color3(IPI_In_Color3_Old);
	}
}

void _stdcall CADSInterface::OnPlcNotifyColor4(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Color4 IPI_In_Color4_Old = s_IPI_In_Color4;
		// here we obtain the new values
		s_IPI_In_Color4 = *((IPI_In_Color4*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Color4(IPI_In_Color4_Old);
	}
}

void _stdcall CADSInterface::OnPlcNotifyColor5(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Color5 IPI_In_Color5_Old = s_IPI_In_Color5;
		// here we obtain the new values
		s_IPI_In_Color5 = *((IPI_In_Color5*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Color5(IPI_In_Color5_Old);
	}
}

void _stdcall CADSInterface::OnPlcNotifyType1(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Type1 IPI_In_Type1_Old = s_IPI_In_Type1;
		// here we obtain the new values
		s_IPI_In_Type1 = *((IPI_In_Type1*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Type1(IPI_In_Type1_Old);
	}
}

void _stdcall CADSInterface::OnPlcNotifyType2(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Type2 IPI_In_Type2_Old = s_IPI_In_Type2;
		// here we obtain the new values
		s_IPI_In_Type2 = *((IPI_In_Type2*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Type2(IPI_In_Type2_Old);
	}
}

void _stdcall CADSInterface::OnPlcNotifyType3(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Type3 IPI_In_Type3_Old = s_IPI_In_Type3;
		// here we obtain the new values
		s_IPI_In_Type3 = *((IPI_In_Type3*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Type3(IPI_In_Type3_Old);
	}
}

void _stdcall CADSInterface::OnPlcNotifyType4(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Type4 IPI_In_Type4_Old = s_IPI_In_Type4;
		// here we obtain the new values
		s_IPI_In_Type4 = *((IPI_In_Type4*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Type4(IPI_In_Type4_Old);
	}
}

void _stdcall CADSInterface::OnPlcNotifyType5(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Type5 IPI_In_Type5_Old = s_IPI_In_Type5;
		// here we obtain the new values
		s_IPI_In_Type5 = *((IPI_In_Type5*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Type5(IPI_In_Type5_Old);
	}
}

///////////////////////////////////////////////////////////////////////////
//// ADS Callback : called on Changes in the IPI_In_Lot_Number1 structure
//////////////////////////////////////////////////////////////////////////

void _stdcall CADSInterface::OnPlcNotifyLot_Nr1(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Lot_Number1 IPI_In_Lot_Number1Old = s_IPI_In_Lot_Number1;
		// here we obtain the new values
		s_IPI_In_Lot_Number1 = *((IPI_In_Lot_Number1*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Lot_Nr1(IPI_In_Lot_Number1Old);
	}
}

///////////////////////////////////////////////////////////////////////////
//// ADS Callback : called on Changes in the IPI_In_Lot_Number2 structure
//////////////////////////////////////////////////////////////////////////

void _stdcall CADSInterface::OnPlcNotifyLot_Nr2(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Lot_Number2 IPI_In_Lot_Number2Old = s_IPI_In_Lot_Number2;
		// here we obtain the new values
		s_IPI_In_Lot_Number2 = *((IPI_In_Lot_Number2*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Lot_Nr2(IPI_In_Lot_Number2Old);
	}
}

///////////////////////////////////////////////////////////////////////////
//// ADS Callback : called on Changes in the IPI_In_Lot_Number3 structure
//////////////////////////////////////////////////////////////////////////

void _stdcall CADSInterface::OnPlcNotifyLot_Nr3(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Lot_Number3 IPI_In_Lot_Number3Old = s_IPI_In_Lot_Number3;
		// here we obtain the new values
		s_IPI_In_Lot_Number3 = *((IPI_In_Lot_Number3*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Lot_Nr3(IPI_In_Lot_Number3Old);
	}
}

///////////////////////////////////////////////////////////////////////////
//// ADS Callback : called on Changes in the IPI_In_Lot_Number4 structure
//////////////////////////////////////////////////////////////////////////

void _stdcall CADSInterface::OnPlcNotifyLot_Nr4(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Lot_Number4 IPI_In_Lot_Number4Old = s_IPI_In_Lot_Number4;
		// here we obtain the new values
		s_IPI_In_Lot_Number4 = *((IPI_In_Lot_Number4*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Lot_Nr4(IPI_In_Lot_Number4Old);
	}
}

///////////////////////////////////////////////////////////////////////////
//// ADS Callback : called on Changes in the IPI_In_Lot_Number5 structure
//////////////////////////////////////////////////////////////////////////

void _stdcall CADSInterface::OnPlcNotifyLot_Nr5(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Lot_Number5 IPI_In_Lot_Number5Old = s_IPI_In_Lot_Number5;
		// here we obtain the new values
		s_IPI_In_Lot_Number5 = *((IPI_In_Lot_Number5*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Lot_Nr5(IPI_In_Lot_Number5Old);
	}
}

///////////////////////////////////////////////////////////////////////////
//// ADS Callback : called on Changes in the IPI_In_Start structure
//////////////////////////////////////////////////////////////////////////

/*void _stdcall CADSInterface::OnPlcNotifyStart(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Start IPI_In_StartOld = s_IPI_In_Start;
		// here we obtain the new values
		s_IPI_In_Start = *((IPI_In_Start*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Start(IPI_In_StartOld);
	}
}*/


///////////////////////////////////////////////////////////////////////////
//// ADS Callback : called on Changes in the IPI_In_First_Lens structure
//////////////////////////////////////////////////////////////////////////

void _stdcall CADSInterface::OnPlcNotifyFirst_Lens(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_First_Lens IPI_In_First_LensOld = s_IPI_In_First_Lens;
		// here we obtain the new values
		s_IPI_In_First_Lens = *((IPI_In_First_Lens*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_First_Lens(IPI_In_First_LensOld);
	}
}

///////////////////////////////////////////////////////////////////////////
//// ADS Callback : called on Changes in the IPI_In_Tool structure
//////////////////////////////////////////////////////////////////////////

void _stdcall CADSInterface::OnPlcNotifyTool(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Tool IPI_In_ToolOld = s_IPI_In_Tool;
		// here we obtain the new values
		s_IPI_In_Tool = *((IPI_In_Tool*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Tool(IPI_In_ToolOld);
	}
}

///////////////////////////////////////////////////////////////////////////
//// ADS Callback : called on Changes in the IPI_In_Mold structure
//////////////////////////////////////////////////////////////////////////

void _stdcall CADSInterface::OnPlcNotifyMold(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Mold IPI_In_MoldOld = s_IPI_In_Mold;
		// here we obtain the new values
		s_IPI_In_Mold = *((IPI_In_Mold*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Mold(IPI_In_MoldOld);
	}
}


///////////////////////////////////////////////////////////////////////////
//// ADS Callback : called on Changes in the IPI_In_Arm structure
//////////////////////////////////////////////////////////////////////////

void _stdcall CADSInterface::OnPlcNotifyArm(AmsAddr* pAddr, AdsNotificationHeader* pNotification, unsigned long hUser)
{
	if(pNotification->cbSampleSize == 0)
	{
		//error: disconnected from PLC
		OutputDebugLogTo(8,TRUE,"Disconnected from PLC");
		
		return;
	}
	if(CADSInterface* pInterface = (CADSInterface*) hUser)
	{

		// we copy the old values of the structure here
		IPI_In_Arm IPI_In_ArmOld = s_IPI_In_Arm;
		// here we obtain the new values
		s_IPI_In_Arm = *((IPI_In_Arm*)pNotification->data);
		/// now we call our event handler with the old values as parameter
		pInterface->HandlePLC_Arm(IPI_In_ArmOld);
	}
}

void CADSInterface::HandlePLC_Common(IPI_In_Common IPI_In_CommonOld)
{
	CString strTemp1;
	/// Here we compare the different parts of the structure that may have changed
	if(m_pCLApp->m_bOnline){

		
		if(s_IPI_In_Common.w_lens_ipi_to_ipi_watchdog != IPI_In_CommonOld.w_lens_ipi_to_ipi_watchdog)
		{
					
			OutputDebugLogTo(8,TRUE,"WatchDog from Server %d",s_IPI_In_Common.w_lens_ipi_to_ipi_watchdog);
			nData = s_IPI_In_Common.w_lens_ipi_to_ipi_watchdog;
			m_pCLApp->UpdateWatchDog(nData);
			
		}
		if(s_IPI_In_Common.b_lens_ipi_lot_changed != IPI_In_CommonOld.b_lens_ipi_lot_changed)
		{
			if(s_IPI_In_Common.b_lens_ipi_lot_changed) {
				m_bLotChanged = TRUE;
				m_pCLApp->AutoLoadRecipes();
				m_pCLApp->UpdateLotChangedAck();

				OutputDebugLogTo(8,TRUE,"Lot Changed %d", s_IPI_In_Common.b_lens_ipi_lot_changed);
			}
		}
		
		if(s_IPI_In_Common.w_lens_ipi_inspection_setup != IPI_In_CommonOld.w_lens_ipi_inspection_setup)
		{
		
///			OutputDebugLogTo(8,TRUE,"Inspection Setup %d",s_IPI_In_Common.w_lens_ipi_inspection_setup);
		}

	}	
//	else
//		OutputDebugLogTo(8,TRUE,"Vision OffLine");

}


void CADSInterface::HandlePLC_Color1(IPI_In_Color1 IPI_Color1Old)
{
	CString strTemp;
	strTemp.Format(s_IPI_In_Color1.s_lens_color);
	OutputDebugLogTo(8,TRUE,"ColorInfo Track 1 = %s", strTemp);
	OutputDebugLogTo(8,TRUE,"  ");
	m_pCLApp->m_strConfigFile = strTemp.Trim();
}

void CADSInterface::HandlePLC_Color2(IPI_In_Color2 IPI_Color2Old)
{
	CString strTemp;
	strTemp.Format(s_IPI_In_Color2.s_lens_color);
	OutputDebugLogTo(8,TRUE,"ColorInfo Track 2 = %s", strTemp);
	OutputDebugLogTo(8,TRUE,"  ");
	m_pCLApp->m_strConfigFile = strTemp.Trim();
}

void CADSInterface::HandlePLC_Color3(IPI_In_Color3 IPI_Color3Old)
{
	CString strTemp;
	strTemp.Format(s_IPI_In_Color3.s_lens_color);
	OutputDebugLogTo(8,TRUE,"ColorInfo Track 3 = %s", strTemp);
	OutputDebugLogTo(8,TRUE,"  ");
	m_pCLApp->m_strConfigFile = strTemp.Trim();
}
void CADSInterface::HandlePLC_Color4(IPI_In_Color4 IPI_Color4Old)
{
	CString strTemp;
	strTemp.Format(s_IPI_In_Color4.s_lens_color);
	OutputDebugLogTo(8,TRUE,"ColorInfo Track 4 = %s", strTemp);
	OutputDebugLogTo(8,TRUE,"  ");
	m_pCLApp->m_strConfigFile = strTemp.Trim();
}

void CADSInterface::HandlePLC_Color5(IPI_In_Color5 IPI_Color5Old)
{
	CString strTemp;
	strTemp.Format(s_IPI_In_Color5.s_lens_color);
	OutputDebugLogTo(8,TRUE,"ColorInfo Track 5 = %s", strTemp);
	OutputDebugLogTo(8,TRUE,"  ");
	m_pCLApp->m_strConfigFile = strTemp.Trim();
}


void CADSInterface::HandlePLC_Type1(IPI_In_Type1 IPI_Type1Old)
{
	CString strTemp;
	
	/// Here we compare the different parts of the structure that may have changed

	
	if(m_pCLApp->m_bOnline /* && m_bLotChanged*/){

		
		strTemp.Format(s_IPI_In_Type1.s_lens_type);
			
		OutputDebugLogTo(8,TRUE,"TypeInfo Track 1 = %s", strTemp);
		OutputDebugLogTo(8,TRUE,"  ");

	}	
//	else
//		OutputDebugLogTo(8,TRUE,"Vision OffLine");

}

void CADSInterface::HandlePLC_Type2(IPI_In_Type2 IPI_Type2Old)
{
	CString strTemp;
	
	/// Here we compare the different parts of the structure that may have changed

	
	if(m_pCLApp->m_bOnline /*&& m_bLotChanged*/){

		
		strTemp.Format(s_IPI_In_Type2.s_lens_type);
			
		OutputDebugLogTo(8,TRUE,"TypeInfo Track 2 = %s", strTemp);
		OutputDebugLogTo(8,TRUE,"  ");
		
	}	
//	else
//		OutputDebugLogTo(8,TRUE,"Vision OffLine");

}

void CADSInterface::HandlePLC_Type3(IPI_In_Type3 IPI_Type3Old)
{
	CString strTemp;
	
	/// Here we compare the different parts of the structure that may have changed

	
	if(m_pCLApp->m_bOnline /*&& m_bLotChanged*/){

		
		strTemp.Format(s_IPI_In_Type3.s_lens_type);
			
		OutputDebugLogTo(8,TRUE,"TypeInfo Track 3 = %s", strTemp);
		OutputDebugLogTo(8,TRUE,"  ");

	}	
//	else
//		OutputDebugLogTo(8,TRUE,"Vision OffLine");

}

void CADSInterface::HandlePLC_Type4(IPI_In_Type4 IPI_Type4Old)
{
	CString strTemp;
	
	/// Here we compare the different parts of the structure that may have changed

	
	if(m_pCLApp->m_bOnline /*&& m_bLotChanged*/){

		
		strTemp.Format(s_IPI_In_Type4.s_lens_type);
			
		OutputDebugLogTo(8,TRUE,"TypeInfo Track 4 = %s", strTemp);
		OutputDebugLogTo(8,TRUE,"  ");

	}	
//	else
//		OutputDebugLogTo(8,TRUE,"Vision OffLine");

}

void CADSInterface::HandlePLC_Type5(IPI_In_Type5 IPI_Type5Old)
{
	CString strTemp;
	
	/// Here we compare the different parts of the structure that may have changed

	
	if(m_pCLApp->m_bOnline /*&& m_bLotChanged*/){

		
		strTemp.Format(s_IPI_In_Type5.s_lens_type);
			
		OutputDebugLogTo(8,TRUE,"TypeInfo Track 5 = %s", strTemp);
		OutputDebugLogTo(8,TRUE,"  ");

	}	
//	else
//		OutputDebugLogTo(8,TRUE,"Vision OffLine");

}
void CADSInterface::HandlePLC_Lot_Nr1(IPI_In_Lot_Number1 IPI_Lot_Number1Old)
{
	CString strTemp;
	/// Here we compare the different parts of the structure that may have changed
	if(m_pCLApp->m_bOnline && m_bLotChanged){

		
		strTemp.Format(s_IPI_In_Lot_Number1.sLotNumber);
		OutputDebugLogTo(8,TRUE,"LotNumber Track 1 = %s", strTemp);
		OutputDebugLogTo(8,TRUE,"  ");

	}	
//	else
//		OutputDebugLogTo(8,TRUE,"Vision OffLine");

}

void CADSInterface::HandlePLC_Lot_Nr2(IPI_In_Lot_Number2 IPI_Lot_Number2Old)
{
	CString strTemp;
	/// Here we compare the different parts of the structure that may have changed
	if(m_pCLApp->m_bOnline && m_bLotChanged){

		
		strTemp.Format(s_IPI_In_Lot_Number2.sLotNumber);
		OutputDebugLogTo(8,TRUE,"LotNumber Track 2 = %s", strTemp);
		OutputDebugLogTo(8,TRUE,"  ");

	}	
//	else
//		OutputDebugLogTo(8,TRUE,"Vision OffLine");

}
void CADSInterface::HandlePLC_Lot_Nr3(IPI_In_Lot_Number3 IPI_Lot_Number3Old)
{
	CString strTemp;
	/// Here we compare the different parts of the structure that may have changed
	if(m_pCLApp->m_bOnline && m_bLotChanged){

		
		strTemp.Format(s_IPI_In_Lot_Number3.sLotNumber);
		OutputDebugLogTo(8,TRUE,"LotNumber Track 3 = %s", strTemp);
		OutputDebugLogTo(8,TRUE,"  ");
	}	
//	else
//		OutputDebugLogTo(8,TRUE,"Vision OffLine");

}
void CADSInterface::HandlePLC_Lot_Nr4(IPI_In_Lot_Number4 IPI_Lot_Number4Old)
{
	CString strTemp;
	/// Here we compare the different parts of the structure that may have changed
	if(m_pCLApp->m_bOnline && m_bLotChanged){

		
		strTemp.Format(s_IPI_In_Lot_Number4.sLotNumber);
		OutputDebugLogTo(8,TRUE,"LotNumber Track 4 = %s", strTemp);
		OutputDebugLogTo(8,TRUE,"  ");
	}	
//	else
//		OutputDebugLogTo(8,TRUE,"Vision OffLine");

}
void CADSInterface::HandlePLC_Lot_Nr5(IPI_In_Lot_Number5 IPI_Lot_Number5Old)
{
	CString strTemp;
	/// Here we compare the different parts of the structure that may have changed
	if(m_pCLApp->m_bOnline && m_bLotChanged){

		
		strTemp.Format(s_IPI_In_Lot_Number5.sLotNumber);
		OutputDebugLogTo(8,TRUE,"LotNumber Track 5 = %s", strTemp);
		OutputDebugLogTo(8,TRUE,"  ");
	}	
//	else
//		OutputDebugLogTo(8,TRUE,"Vision OffLine");

}
/*void CADSInterface::HandlePLC_Start(IPI_In_Start IPI_In_StartOld)
{
	if(m_pCLApp->m_bOnline) {
//		if(s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[0] != IPI_In_StartOld.ab_lens_ipi_to_ipi_start_inspection_track[0])
//		{
			//declaration changed
//			OutputDebugLogTo(8,TRUE,"Recieved Start Inspection for Track[1] Start[%d] : Inspecting[%d] ;",s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[0],m_pCLApp->m_TrackManager[0].m_bInspecting);
			if((s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[0])&&(!m_pCLApp->m_TrackManager[0].m_bInspecting)){
				OutputDebugLogTo(1,TRUE,"Recieved Start Inspection for Track[1] Start[%d] : Inspecting[%d] ;",s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[0],m_pCLApp->m_TrackManager[0].m_bInspecting);
//			    Sleep(200);
				m_pCLApp->StartInspectSeq(0x01);
			}
			
//		}

//		if(s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[1] != IPI_In_StartOld.ab_lens_ipi_to_ipi_start_inspection_track[1])
//		{
			//declaration changed
//			OutputDebugLogTo(8,TRUE,"Recieved Start Inspection for Track[2] Start[%d] : Inspecting[%d] ;",s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[1],m_pCLApp->m_TrackManager[1].m_bInspecting);
			if((s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[1])&&(!m_pCLApp->m_TrackManager[1].m_bInspecting)){
				OutputDebugLogTo(2,TRUE,"Recieved Start Inspection for Track[2] Start[%d] : Inspecting[%d] ;",s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[0],m_pCLApp->m_TrackManager[0].m_bInspecting);
//			    Sleep(200);
				m_pCLApp->StartInspectSeq(0x02);
			}
			
//		}

//		if(s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[2] != IPI_In_StartOld.ab_lens_ipi_to_ipi_start_inspection_track[2])
//		{
			//declaration changed
 //			OutputDebugLogTo(8,TRUE,"Recieved Start Inspection for Track[3] Start[%d] : Inspecting[%d] ;",s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[2],m_pCLApp->m_TrackManager[2].m_bInspecting);
			if((s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[2])&&(!m_pCLApp->m_TrackManager[2].m_bInspecting)){
				OutputDebugLogTo(3,TRUE,"Recieved Start Inspection for Track[3] Start[%d] : Inspecting[%d] ;",s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[0],m_pCLApp->m_TrackManager[0].m_bInspecting);
//				Sleep(200);
				m_pCLApp->StartInspectSeq(0x04);
			}
			
//		}

//		if(s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[3] != IPI_In_StartOld.ab_lens_ipi_to_ipi_start_inspection_track[3])
//		{
			//declaration changed
//			OutputDebugLogTo(8,TRUE,"Recieved Start Inspection for Track[4] Start[%d] : Inspecting[%d] ;",s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[3],m_pCLApp->m_TrackManager[3].m_bInspecting);
			if((s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[3])&&(!m_pCLApp->m_TrackManager[3].m_bInspecting)){
				OutputDebugLogTo(4,TRUE,"Recieved Start Inspection for Track[4] Start[%d] : Inspecting[%d] ;",s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[0],m_pCLApp->m_TrackManager[0].m_bInspecting);
//				Sleep(200);
				m_pCLApp->StartInspectSeq(0x08);
			}
			
//		}

//		if(s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[4] != IPI_In_StartOld.ab_lens_ipi_to_ipi_start_inspection_track[4])
//		{
			//declaration changed
//			OutputDebugLogTo(8,TRUE,"Recieved Start Inspection for Track[5] Start[%d] : Inspecting[%d] ;",s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[4],m_pCLApp->m_TrackManager[4].m_bInspecting);
			if((s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[4])&&(!m_pCLApp->m_TrackManager[4].m_bInspecting)){
				OutputDebugLogTo(5,TRUE,"Recieved Start Inspection for Track[5] Start[%d] : Inspecting[%d] ;",s_IPI_In_Start.ab_lens_ipi_to_ipi_start_inspection_track[0],m_pCLApp->m_TrackManager[0].m_bInspecting);
//				Sleep(200);
				m_pCLApp->StartInspectSeq(0x10);
			}
		
//		}
	}

}*/


void CADSInterface::HandlePLC_First_Lens(IPI_In_First_Lens IPI_In_First_LensOld)
{

	/// Here we compare the different parts of the structure that may have changed
	if(m_pCLApp->m_bOnline){

		

		
		if(s_IPI_In_First_Lens.ab_lens_ipi_to_ipi_first_lens_per_CT[0] != IPI_In_First_LensOld.ab_lens_ipi_to_ipi_first_lens_per_CT[0])
		{
		
//			OutputDebugLogTo(8,TRUE,"FirstLens_CT_Track1 %d",s_IPI_In_First_Lens.ab_lens_ipi_to_ipi_first_lens_per_CT[0]);
		}

		if(s_IPI_In_First_Lens.ab_lens_ipi_to_ipi_first_lens_per_CT[1] != IPI_In_First_LensOld.ab_lens_ipi_to_ipi_first_lens_per_CT[1])
		{
		
//			OutputDebugLogTo(8,TRUE,"FirstLens_CT_Track2 %d",s_IPI_In_First_Lens.ab_lens_ipi_to_ipi_first_lens_per_CT[1]);
		}
		
		if(s_IPI_In_First_Lens.ab_lens_ipi_to_ipi_first_lens_per_CT[2] != IPI_In_First_LensOld.ab_lens_ipi_to_ipi_first_lens_per_CT[2])
		{
		
//			OutputDebugLogTo(8,TRUE,"FirstLens_CT_Track3 %d",s_IPI_In_First_Lens.ab_lens_ipi_to_ipi_first_lens_per_CT[2]);
		}

		if(s_IPI_In_First_Lens.ab_lens_ipi_to_ipi_first_lens_per_CT[3] != IPI_In_First_LensOld.ab_lens_ipi_to_ipi_first_lens_per_CT[3])
		{
		
//			OutputDebugLogTo(8,TRUE,"FirstLens_CT_Track4 %d",s_IPI_In_First_Lens.ab_lens_ipi_to_ipi_first_lens_per_CT[3]);
		}

		if(s_IPI_In_First_Lens.ab_lens_ipi_to_ipi_first_lens_per_CT[4] != IPI_In_First_LensOld.ab_lens_ipi_to_ipi_first_lens_per_CT[4])
		{
		
//			OutputDebugLogTo(8,TRUE,"FirstLens_CT_Track5 %d",s_IPI_In_First_Lens.ab_lens_ipi_to_ipi_first_lens_per_CT[4]);
		}

		
	}
}

void CADSInterface::HandlePLC_Tool(IPI_In_Tool IPI_In_ToolOld)
{

	/// Here we compare the different parts of the structure that may have changed
	if(m_pCLApp->m_bOnline){
		/////ToolNumber
		if(s_IPI_In_Tool.aw_lens_ipi_to_ipi_tool_number[0])// != IPI_In_ToolOld.aw_lens_ipi_to_ipi_tool_number[0])
		{
			m_pCLApp->m_nToolNo[0] = s_IPI_In_Tool.aw_lens_ipi_to_ipi_tool_number[0];
			OutputDebugLogTo(7,TRUE,"ToolNumber_Track1 %d",s_IPI_In_Tool.aw_lens_ipi_to_ipi_tool_number[0]);
		}

		if(s_IPI_In_Tool.aw_lens_ipi_to_ipi_tool_number[1])// != IPI_In_ToolOld.aw_lens_ipi_to_ipi_tool_number[1])
		{
			m_pCLApp->m_nToolNo[1] = s_IPI_In_Tool.aw_lens_ipi_to_ipi_tool_number[1];
			OutputDebugLogTo(7,TRUE,"ToolNumber_Track2 %d",s_IPI_In_Tool.aw_lens_ipi_to_ipi_tool_number[1]);
		}
		
		if(s_IPI_In_Tool.aw_lens_ipi_to_ipi_tool_number[2])// != IPI_In_ToolOld.aw_lens_ipi_to_ipi_tool_number[2])
		{
		
			m_pCLApp->m_nToolNo[2] = s_IPI_In_Tool.aw_lens_ipi_to_ipi_tool_number[2];
			OutputDebugLogTo(7,TRUE,"ToolNumber_Track3 %d",s_IPI_In_Tool.aw_lens_ipi_to_ipi_tool_number[2]);
		}

		if(s_IPI_In_Tool.aw_lens_ipi_to_ipi_tool_number[3])// != IPI_In_ToolOld.aw_lens_ipi_to_ipi_tool_number[3])
		{
			m_pCLApp->m_nToolNo[3] = s_IPI_In_Tool.aw_lens_ipi_to_ipi_tool_number[3];
			OutputDebugLogTo(7,TRUE,"ToolNumber_Track4 %d",s_IPI_In_Tool.aw_lens_ipi_to_ipi_tool_number[3]);
		}

		if(s_IPI_In_Tool.aw_lens_ipi_to_ipi_tool_number[4]) //!= IPI_In_ToolOld.aw_lens_ipi_to_ipi_tool_number[4])
		{
			m_pCLApp->m_nToolNo[4] = s_IPI_In_Tool.aw_lens_ipi_to_ipi_tool_number[4];
			OutputDebugLogTo(7,TRUE,"ToolNumber_Track5 %d",s_IPI_In_Tool.aw_lens_ipi_to_ipi_tool_number[4]);
		}

	}
}

void CADSInterface::HandlePLC_Mold(IPI_In_Mold IPI_In_MoldOld)
{

	/// Here we compare the different parts of the structure that may have changed
	if(m_pCLApp->m_bOnline){


		/////MoldNumber
		if(s_IPI_In_Mold.aw_lens_ipi_to_ipi_mold_number[0])// != IPI_In_MoldOld.aw_lens_ipi_to_ipi_mold_number[0])
		{
			m_pCLApp->m_nMoldNo[0] = s_IPI_In_Mold.aw_lens_ipi_to_ipi_mold_number[0];
			OutputDebugLogTo(6,TRUE,"MoldNumber_Track1 %d",s_IPI_In_Mold.aw_lens_ipi_to_ipi_mold_number[0]);
		}

		if(s_IPI_In_Mold.aw_lens_ipi_to_ipi_mold_number[1])// != IPI_In_MoldOld.aw_lens_ipi_to_ipi_mold_number[1])
		{
			m_pCLApp->m_nMoldNo[1] = s_IPI_In_Mold.aw_lens_ipi_to_ipi_mold_number[1];
			OutputDebugLogTo(6,TRUE,"MoldNumber_Track2 %d",s_IPI_In_Mold.aw_lens_ipi_to_ipi_mold_number[1]);
		}
		
		if(s_IPI_In_Mold.aw_lens_ipi_to_ipi_mold_number[2])// != IPI_In_MoldOld.aw_lens_ipi_to_ipi_mold_number[2])
		{
			m_pCLApp->m_nMoldNo[2] = s_IPI_In_Mold.aw_lens_ipi_to_ipi_mold_number[2];
			OutputDebugLogTo(6,TRUE,"MoldNumber_Track3 %d",s_IPI_In_Mold.aw_lens_ipi_to_ipi_mold_number[2]);
		}

		if(s_IPI_In_Mold.aw_lens_ipi_to_ipi_mold_number[3])// != IPI_In_MoldOld.aw_lens_ipi_to_ipi_mold_number[3])
		{
			m_pCLApp->m_nMoldNo[3] = s_IPI_In_Mold.aw_lens_ipi_to_ipi_mold_number[3];
			OutputDebugLogTo(6,TRUE,"MoldNumber_Track4 %d",s_IPI_In_Mold.aw_lens_ipi_to_ipi_mold_number[3]);
		}

		if(s_IPI_In_Mold.aw_lens_ipi_to_ipi_mold_number[4])// != IPI_In_MoldOld.aw_lens_ipi_to_ipi_mold_number[4])
		{
			m_pCLApp->m_nMoldNo[4] = s_IPI_In_Mold.aw_lens_ipi_to_ipi_mold_number[4];
			OutputDebugLogTo(6,TRUE,"MoldNumber_Track5 %d",s_IPI_In_Mold.aw_lens_ipi_to_ipi_mold_number[4]);
		}
	}
}

void CADSInterface::HandlePLC_Arm(IPI_In_Arm IPI_In_ArmOld)
{

	/// Here we compare the different parts of the structure that may have changed
	if(m_pCLApp->m_bOnline){

		/////GripperNumber
		if(s_IPI_In_Arm.aw_lens_ipi_to_ipi_gripper_arm_number[0])// != IPI_In_ArmOld.aw_lens_ipi_to_ipi_gripper_arm_number[0])
		{
			m_pCLApp->m_nGripperNo[0] = s_IPI_In_Arm.aw_lens_ipi_to_ipi_gripper_arm_number[0];
			OutputDebugLogTo(5,TRUE,"GripperArmNumber_Track1 %d",s_IPI_In_Arm.aw_lens_ipi_to_ipi_gripper_arm_number[0]);
		}

		if(s_IPI_In_Arm.aw_lens_ipi_to_ipi_gripper_arm_number[1])// != IPI_In_ArmOld.aw_lens_ipi_to_ipi_gripper_arm_number[1])
		{
			m_pCLApp->m_nGripperNo[1] = s_IPI_In_Arm.aw_lens_ipi_to_ipi_gripper_arm_number[1];
			OutputDebugLogTo(5,TRUE,"GripperArmNumber_Track2 %d",s_IPI_In_Arm.aw_lens_ipi_to_ipi_gripper_arm_number[1]);
		}
		
		if(s_IPI_In_Arm.aw_lens_ipi_to_ipi_gripper_arm_number[2])// != IPI_In_ArmOld.aw_lens_ipi_to_ipi_gripper_arm_number[2])
		{
			m_pCLApp->m_nGripperNo[2] = s_IPI_In_Arm.aw_lens_ipi_to_ipi_gripper_arm_number[2];
			OutputDebugLogTo(5,TRUE,"GripperArmNumber_Track3 %d",s_IPI_In_Arm.aw_lens_ipi_to_ipi_gripper_arm_number[2]);
		}

		if(s_IPI_In_Arm.aw_lens_ipi_to_ipi_gripper_arm_number[3])// != IPI_In_ArmOld.aw_lens_ipi_to_ipi_gripper_arm_number[3])
		{
			m_pCLApp->m_nGripperNo[3] = s_IPI_In_Arm.aw_lens_ipi_to_ipi_gripper_arm_number[3];
			OutputDebugLogTo(5,TRUE,"GripperArmNumber_Track4 %d",s_IPI_In_Arm.aw_lens_ipi_to_ipi_gripper_arm_number[3]);
		}

		if(s_IPI_In_Arm.aw_lens_ipi_to_ipi_gripper_arm_number[4])// != IPI_In_ArmOld.aw_lens_ipi_to_ipi_gripper_arm_number[4])
		{
			m_pCLApp->m_nGripperNo[4] = s_IPI_In_Arm.aw_lens_ipi_to_ipi_gripper_arm_number[4];
			OutputDebugLogTo(5,TRUE,"GripperArmNumber_Track5 %d",s_IPI_In_Arm.aw_lens_ipi_to_ipi_gripper_arm_number[4]);
		}

	}
}

BOOL CADSInterface::OpenPort()
{
	CPerformanceTimer ADSTimer;
	long nError,nErr;
/////////////////////////////Variables used for the BUSY Signals////////////////
	/*char m_strADSIs_Busy1[] = {".ab_lens_ipi_from_ipi_busy[1]"};
	char m_strADSIs_Busy2[] = {".ab_lens_ipi_from_ipi_busy[2]"};
	char m_strADSIs_Busy3[] = {".ab_lens_ipi_from_ipi_busy[3]"};
	char m_strADSIs_Busy4[] = {".ab_lens_ipi_from_ipi_busy[4]"};
	char m_strADSIs_Busy5[] = {".ab_lens_ipi_from_ipi_busy[5]"};*/
	char m_strADSIs_Online[] = {".b_lens_ip_from_ipi_is_online"};
	char m_strADSWatchDogToServer[] = {".w_lens_ipi_from_ipi_watchdog"};
	char m_strADSLotChangedAck[] = {".b_lens_ipi_lot_changed_acknowledge"};
	//////////// RESULTS
	/*char m_strADSIs_BadLens1[] = {".ab_lens_ipi_from_ipi_bad_lens[1]"};
	char m_strADSIs_BadLens2[] = {".ab_lens_ipi_from_ipi_bad_lens[2]"};
	char m_strADSIs_BadLens3[] = {".ab_lens_ipi_from_ipi_bad_lens[3]"};
	char m_strADSIs_BadLens4[] = {".ab_lens_ipi_from_ipi_bad_lens[4]"};
	char m_strADSIs_BadLens5[] = {".ab_lens_ipi_from_ipi_bad_lens[5]"};

	char m_strADSIs_GoodLens1[] = {".ab_lens_ipi_from_ipi_good_lens[1]"};
	char m_strADSIs_GoodLens2[] = {".ab_lens_ipi_from_ipi_good_lens[2]"};
	char m_strADSIs_GoodLens3[] = {".ab_lens_ipi_from_ipi_good_lens[3]"};
	char m_strADSIs_GoodLens4[] = {".ab_lens_ipi_from_ipi_good_lens[4]"};
	char m_strADSIs_GoodLens5[] = {".ab_lens_ipi_from_ipi_good_lens[5]"};*/

	char m_strADSIs_TearLens1[] = {".ab_lens_ipi_from_ipi_tear[1]"};
	char m_strADSIs_TearLens2[] = {".ab_lens_ipi_from_ipi_tear[2]"};
	char m_strADSIs_TearLens3[] = {".ab_lens_ipi_from_ipi_tear[3]"};
	char m_strADSIs_TearLens4[] = {".ab_lens_ipi_from_ipi_tear[4]"};
	char m_strADSIs_TearLens5[] = {".ab_lens_ipi_from_ipi_tear[5]"};

	char m_strADSIs_GapLens1[] = {".ab_lens_ipi_from_ipi_gap[1]"};
	char m_strADSIs_GapLens2[] = {".ab_lens_ipi_from_ipi_gap[2]"};
	char m_strADSIs_GapLens3[] = {".ab_lens_ipi_from_ipi_gap[3]"};
	char m_strADSIs_GapLens4[] = {".ab_lens_ipi_from_ipi_gap[4]"};
	char m_strADSIs_GapLens5[] = {".ab_lens_ipi_from_ipi_gap[5]"};

	char m_strADSIs_NoLens1[] = {".ab_lens_ipi_from_ipi_no_lens[1]"};
	char m_strADSIs_NoLens2[] = {".ab_lens_ipi_from_ipi_no_lens[2]"};
	char m_strADSIs_NoLens3[] = {".ab_lens_ipi_from_ipi_no_lens[3]"};
	char m_strADSIs_NoLens4[] = {".ab_lens_ipi_from_ipi_no_lens[4]"};
	char m_strADSIs_NoLens5[] = {".ab_lens_ipi_from_ipi_no_lens[5]"};

	char m_strADSIs_NonCirLens1[] = {".ab_lens_ipi_from_ipi_non_circular[1]"};
	char m_strADSIs_NonCirLens2[] = {".ab_lens_ipi_from_ipi_non_circular[2]"};
	char m_strADSIs_NonCirLens3[] = {".ab_lens_ipi_from_ipi_non_circular[3]"};
	char m_strADSIs_NonCirLens4[] = {".ab_lens_ipi_from_ipi_non_circular[4]"};
	char m_strADSIs_NonCirLens5[] = {".ab_lens_ipi_from_ipi_non_circular[5]"};

	char m_strADSImageNumber1[]   = {".ab_lens_ipi_from_ipi_image_number[1]"};
	char m_strADSImageNumber2[]   = {".ab_lens_ipi_from_ipi_image_number[2]"};
	char m_strADSImageNumber3[]   = {".ab_lens_ipi_from_ipi_image_number[3]"};
	char m_strADSImageNumber4[]   = {".ab_lens_ipi_from_ipi_image_number[4]"};
	char m_strADSImageNumber5[]   = {".ab_lens_ipi_from_ipi_image_number[5]"};
//	CSingleLock(&m_csADS,TRUE);


	//define handles to plc-structures
	CString strVar;

	// s_IPI_In_Common
	strVar = ".IPI_In_Common";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Common),&m_bhIPI_Common, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_Common"
	AdsNotificationAttrib adsNotify_Common;

	adsNotify_Common.cbLength		= sizeof(s_IPI_In_Common);
	adsNotify_Common.nTransMode	= ADSTRANS_SERVERONCHA;
	adsNotify_Common.nMaxDelay		= 2000000;
	adsNotify_Common.nCycleTime	= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Common, &adsNotify_Common, OnPlcNotifyCommon,(unsigned long)this, &m_bhChangeNotifyCommon);

	if(nError != 0)
		return -1;

	/*
	// s_IPI_In_Start
	strVar = ".IPI_In_Start";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Start),&m_bhIPI_Start, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_Start"
	AdsNotificationAttrib adsNotify_Start;

	adsNotify_Start.cbLength		= sizeof(s_IPI_In_Start);
	adsNotify_Start.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Start.nMaxDelay		= 2000000;
	adsNotify_Start.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Start, &adsNotify_Start, OnPlcNotifyStart,(unsigned long)this, &m_bhChangeNotifyStart);

	if(nError != 0)
		return -1;
	*/
	// s_IPI_In_First_Lens
	strVar = ".IPI_In_First_Lens";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_First_Lens),&m_bhIPI_First_Lens, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_First_Lens;

	adsNotify_First_Lens.cbLength		= sizeof(s_IPI_In_First_Lens);
	adsNotify_First_Lens.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_First_Lens.nMaxDelay		= 2000000;
	adsNotify_First_Lens.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_First_Lens, &adsNotify_First_Lens, OnPlcNotifyFirst_Lens,(unsigned long)this, &m_bhChangeNotifyFirst_Lens);

	if(nError != 0)
		return -1;

	// s_IPI_In_Tool
	strVar = ".IPI_In_Tool";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Tool),&m_bhIPI_Tool, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Tool;

	adsNotify_Tool.cbLength			= sizeof(s_IPI_In_Tool);
	adsNotify_Tool.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Tool.nMaxDelay		= 2000000;
	adsNotify_Tool.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Tool, &adsNotify_Tool, OnPlcNotifyTool,(unsigned long)this, &m_bhChangeNotifyTool);

	if(nError != 0)
		return -1;

	// s_IPI_In_Mold
	strVar = ".IPI_In_Mold";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Mold),&m_bhIPI_Mold, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Mold;

	adsNotify_Mold.cbLength			= sizeof(s_IPI_In_Mold);
	adsNotify_Mold.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Mold.nMaxDelay		= 2000000;
	adsNotify_Mold.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Mold, &adsNotify_Mold, OnPlcNotifyMold,(unsigned long)this, &m_bhChangeNotifyMold);

	if(nError != 0)
		return -1;


	// s_IPI_In_Arm
	strVar = ".IPI_In_Arm";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Arm),&m_bhIPI_Arm, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Arm;

	adsNotify_Arm.cbLength			= sizeof(s_IPI_In_Arm);
	adsNotify_Arm.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Arm.nMaxDelay			= 2000000;
	adsNotify_Arm.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Arm, &adsNotify_Arm, OnPlcNotifyArm,(unsigned long)this, &m_bhChangeNotifyArm);

	if(nError != 0)
		return -1;


	// s_IPI_In_Lot_Nr1
	strVar = ".IPI_In_Lot_Number1";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Lot_Nr1),&m_bhIPI_Lot_Nr1, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Lot_Nr1;

	adsNotify_Lot_Nr1.cbLength			= sizeof(s_IPI_In_Lot_Number1);
	adsNotify_Lot_Nr1.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Lot_Nr1.nMaxDelay			= 2000000;
	adsNotify_Lot_Nr1.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Lot_Nr1, &adsNotify_Lot_Nr1, OnPlcNotifyLot_Nr1,(unsigned long)this, &m_bhChangeNotifyLot_Nr1);

	if(nError != 0)
		return -1;

	// s_IPI_In_Lot_Nr2
	strVar = ".IPI_In_Lot_Number2";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Lot_Nr2),&m_bhIPI_Lot_Nr2, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Lot_Nr2;

	adsNotify_Lot_Nr2.cbLength			= sizeof(s_IPI_In_Lot_Number2);
	adsNotify_Lot_Nr2.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Lot_Nr2.nMaxDelay			= 2000000;
	adsNotify_Lot_Nr2.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Lot_Nr2, &adsNotify_Lot_Nr2, OnPlcNotifyLot_Nr2,(unsigned long)this, &m_bhChangeNotifyLot_Nr2);

	if(nError != 0)
		return -1;


	// s_IPI_In_Lot_Nr3
	strVar = ".IPI_In_Lot_Number3";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Lot_Nr3),&m_bhIPI_Lot_Nr3, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Lot_Nr3;

	adsNotify_Lot_Nr3.cbLength			= sizeof(s_IPI_In_Lot_Number3);
	adsNotify_Lot_Nr3.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Lot_Nr3.nMaxDelay			= 2000000;
	adsNotify_Lot_Nr3.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Lot_Nr3, &adsNotify_Lot_Nr3, OnPlcNotifyLot_Nr3,(unsigned long)this, &m_bhChangeNotifyLot_Nr3);

	if(nError != 0)
		return -1;

	// s_IPI_In_Lot_Nr4
	strVar = ".IPI_In_Lot_Number4";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Lot_Nr4),&m_bhIPI_Lot_Nr4, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Lot_Nr4;

	adsNotify_Lot_Nr4.cbLength			= sizeof(s_IPI_In_Lot_Number4);
	adsNotify_Lot_Nr4.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Lot_Nr4.nMaxDelay			= 2000000;
	adsNotify_Lot_Nr4.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Lot_Nr4, &adsNotify_Lot_Nr4, OnPlcNotifyLot_Nr4,(unsigned long)this, &m_bhChangeNotifyLot_Nr4);

	if(nError != 0)
		return -1;

	// s_IPI_In_Lot_Nr5
	strVar = ".IPI_In_Lot_Number5";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Lot_Nr5),&m_bhIPI_Lot_Nr5, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Lot_Nr5;

	adsNotify_Lot_Nr5.cbLength			= sizeof(s_IPI_In_Lot_Number5);
	adsNotify_Lot_Nr5.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Lot_Nr5.nMaxDelay			= 2000000;
	adsNotify_Lot_Nr5.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Lot_Nr5, &adsNotify_Lot_Nr5, OnPlcNotifyLot_Nr5,(unsigned long)this, &m_bhChangeNotifyLot_Nr5);

	if(nError != 0)
		return -1;

	
	///Added by Biju in Jun'2013
	// s_IPI_In_Type1
	strVar = ".IPI_In_Type1";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Type1),&m_bhIPI_Type1, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Type1;

	adsNotify_Type1.cbLength			= sizeof(s_IPI_In_Type1);
	adsNotify_Type1.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Type1.nMaxDelay			= 2000000;
	adsNotify_Type1.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Type1, &adsNotify_Type1, OnPlcNotifyType1,(unsigned long)this, &m_bhChangeNotifyType1);

	if(nError != 0)
		return -1;

	// s_IPI_In_Type2
	strVar = ".IPI_In_Type2";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Type2),&m_bhIPI_Type2, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Type2;

	adsNotify_Type2.cbLength			= sizeof(s_IPI_In_Type2);
	adsNotify_Type2.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Type2.nMaxDelay			= 2000000;
	adsNotify_Type2.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Type2, &adsNotify_Type2, OnPlcNotifyType2,(unsigned long)this, &m_bhChangeNotifyType2);

	if(nError != 0)
		return -1;

	// s_IPI_In_Type3
	strVar = ".IPI_In_Type3";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Type3),&m_bhIPI_Type3, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Type3;

	adsNotify_Type3.cbLength			= sizeof(s_IPI_In_Type3);
	adsNotify_Type3.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Type3.nMaxDelay			= 2000000;
	adsNotify_Type3.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Type3, &adsNotify_Type3, OnPlcNotifyType3,(unsigned long)this, &m_bhChangeNotifyType3);

	if(nError != 0)
		return -1;

	// s_IPI_In_Type4
	strVar = ".IPI_In_Type4";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Type4),&m_bhIPI_Type4, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Type4;

	adsNotify_Type4.cbLength			= sizeof(s_IPI_In_Type4);
	adsNotify_Type4.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Type4.nMaxDelay			= 2000000;
	adsNotify_Type4.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Type4, &adsNotify_Type4, OnPlcNotifyType4,(unsigned long)this, &m_bhChangeNotifyType4);

	if(nError != 0)
		return -1;

	// s_IPI_In_Type5
	strVar = ".IPI_In_Type5";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Type5),&m_bhIPI_Type5, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Type5;

	adsNotify_Type5.cbLength			= sizeof(s_IPI_In_Type5);
	adsNotify_Type5.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Type5.nMaxDelay			= 2000000;
	adsNotify_Type5.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Type5, &adsNotify_Type5, OnPlcNotifyType5,(unsigned long)this, &m_bhChangeNotifyType5);

	if(nError != 0)
		return -1;

	// s_IPI_In_Color1
	strVar = ".IPI_In_Color1";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Color1),&m_bhIPI_Color1, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Color1;

	adsNotify_Color1.cbLength			= sizeof(s_IPI_In_Color1);
	adsNotify_Color1.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Color1.nMaxDelay			= 2000000;
	adsNotify_Color1.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Color1, &adsNotify_Color1, OnPlcNotifyColor1,(unsigned long)this, &m_bhChangeNotifyColor1);

	if(nError != 0)
		return -1;

	// s_IPI_In_Color2
	strVar = ".IPI_In_Color2";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Color2),&m_bhIPI_Color2, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Color2;

	adsNotify_Color2.cbLength			= sizeof(s_IPI_In_Color2);
	adsNotify_Color2.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Color2.nMaxDelay			= 2000000;
	adsNotify_Color2.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Color2, &adsNotify_Color2, OnPlcNotifyColor2,(unsigned long)this, &m_bhChangeNotifyColor2);

	if(nError != 0)
		return -1;

	// s_IPI_In_Color3
	strVar = ".IPI_In_Color3";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Color3),&m_bhIPI_Color3, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Color3;

	adsNotify_Color3.cbLength			= sizeof(s_IPI_In_Color3);
	adsNotify_Color3.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Color3.nMaxDelay			= 2000000;
	adsNotify_Color3.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Color3, &adsNotify_Color3, OnPlcNotifyColor3,(unsigned long)this, &m_bhChangeNotifyColor3);

	if(nError != 0)
		return -1;

	// s_IPI_In_Color4
	strVar = ".IPI_In_Color4";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Color4),&m_bhIPI_Color4, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Color4;

	adsNotify_Color4.cbLength			= sizeof(s_IPI_In_Color4);
	adsNotify_Color4.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Color4.nMaxDelay			= 2000000;
	adsNotify_Color4.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Color4, &adsNotify_Color4, OnPlcNotifyColor4,(unsigned long)this, &m_bhChangeNotifyColor4);

	if(nError != 0)
		return -1;

	// s_IPI_In_Color5
	strVar = ".IPI_In_Color5";
	nError = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME,0x0, sizeof(m_bhIPI_Color5),&m_bhIPI_Color5, strVar.GetLength()+1,strVar.GetBuffer(0));

	if(nError != 0)
		return -1;

	// set callback-change-notification on struct "s_IPI_In_First_Lens"
	AdsNotificationAttrib adsNotify_Color5;

	adsNotify_Color5.cbLength			= sizeof(s_IPI_In_Color5);
	adsNotify_Color5.nTransMode		= ADSTRANS_SERVERONCHA;
	adsNotify_Color5.nMaxDelay			= 2000000;
	adsNotify_Color5.nCycleTime		= 1000000;
	nError = AdsSyncAddDeviceNotificationReq(&m_PlcAddress,ADSIGRP_SYM_VALBYHND, m_bhIPI_Color5, &adsNotify_Color5, OnPlcNotifyColor5,(unsigned long)this, &m_bhChangeNotifyColor5);

	if(nError != 0)
		return -1;

	/////////////////////////////////
	


	ADSTimer.Reset();

	// Fetch handle for an <szVar> PLC variable 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// BUSY //////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////

	/*nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[0]), &lAmsHdlVar[0], sizeof(m_strADSIs_Busy1), m_strADSIs_Busy1);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle [Busy1]: %d",lAmsHdlVar[0]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[1]), &lAmsHdlVar[1], sizeof(m_strADSIs_Busy2), m_strADSIs_Busy2);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle [Busy2]: %d",lAmsHdlVar[1]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[2]), &lAmsHdlVar[2], sizeof(m_strADSIs_Busy3), m_strADSIs_Busy3);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle [Busy3]: %d",lAmsHdlVar[2]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[3]), &lAmsHdlVar[3], sizeof(m_strADSIs_Busy4), m_strADSIs_Busy4);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle [Busy4]: %d",lAmsHdlVar[3]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[4]), &lAmsHdlVar[4], sizeof(m_strADSIs_Busy5), m_strADSIs_Busy5);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle [Busy5]: %d",lAmsHdlVar[4]);
	*/
	///////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// END OF BUSY //////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// ONLINE //////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////


	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[5]), &lAmsHdlVar[5], sizeof(m_strADSIs_Online), m_strADSIs_Online);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle Online : %d",lAmsHdlVar[5]);

	///////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// BADLENS //////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////

	 /*nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[6]), &lAmsHdlVar[6], sizeof(m_strADSIs_BadLens1), m_strADSIs_BadLens1);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle BadLens - Track[1] : %d",lAmsHdlVar[6]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[7]), &lAmsHdlVar[7], sizeof(m_strADSIs_BadLens2), m_strADSIs_BadLens2);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle BadLens - Track[2] : %d",lAmsHdlVar[7]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[8]), &lAmsHdlVar[8], sizeof(m_strADSIs_BadLens3), m_strADSIs_BadLens3);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle BadLens - Track[3] : %d",lAmsHdlVar[8]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[9]), &lAmsHdlVar[9], sizeof(m_strADSIs_BadLens4), m_strADSIs_BadLens4);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle BadLens - Track[4] : %d",lAmsHdlVar[9]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[10]), &lAmsHdlVar[10], sizeof(m_strADSIs_BadLens5), m_strADSIs_BadLens5);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle BadLens - Track[5] : %d",lAmsHdlVar[10]);

	 
	 ///////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// GOOD LENS //////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[11]), &lAmsHdlVar[11], sizeof(m_strADSIs_GoodLens1), m_strADSIs_GoodLens1);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle GoodLens - Track[1] : %d",lAmsHdlVar[11]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[12]), &lAmsHdlVar[12], sizeof(m_strADSIs_GoodLens2), m_strADSIs_GoodLens2);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle GoodLens - Track[2] : %d",lAmsHdlVar[12]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[13]), &lAmsHdlVar[13], sizeof(m_strADSIs_GoodLens3), m_strADSIs_GoodLens3);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle GoodLens - Track[3] : %d",lAmsHdlVar[13]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[14]), &lAmsHdlVar[14], sizeof(m_strADSIs_GoodLens4), m_strADSIs_GoodLens4);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle GoodLens - Track[4] : %d",lAmsHdlVar[14]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[15]), &lAmsHdlVar[15], sizeof(m_strADSIs_GoodLens5), m_strADSIs_GoodLens5);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle GoodLens - Track[5] : %d",lAmsHdlVar[15]);
	*/
	 ///////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// WatchDog ToServer //////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////


	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[16]), &lAmsHdlVar[16], sizeof(m_strADSWatchDogToServer), m_strADSWatchDogToServer);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle WatchDogToServer : %d",lAmsHdlVar[16]);


	 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// Tear LENS //////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[17]), &lAmsHdlVar[17], sizeof(m_strADSIs_TearLens1), m_strADSIs_TearLens1);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle TearLens - Track[1] : %d",lAmsHdlVar[17]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[18]), &lAmsHdlVar[18], sizeof(m_strADSIs_TearLens2), m_strADSIs_TearLens2);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle TearLens - Track[2] : %d",lAmsHdlVar[18]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[19]), &lAmsHdlVar[19], sizeof(m_strADSIs_TearLens3), m_strADSIs_TearLens3);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle TearLens - Track[3] : %d",lAmsHdlVar[19]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[20]), &lAmsHdlVar[20], sizeof(m_strADSIs_TearLens4), m_strADSIs_TearLens4);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle TearLens - Track[4] : %d",lAmsHdlVar[20]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[21]), &lAmsHdlVar[21], sizeof(m_strADSIs_TearLens5), m_strADSIs_TearLens5);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle TearLens - Track[5] : %d",lAmsHdlVar[21]);


	 ///////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// Gap LENS //////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[22]), &lAmsHdlVar[22], sizeof(m_strADSIs_GapLens1), m_strADSIs_GapLens1);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle GapLens - Track[1] : %d",lAmsHdlVar[22]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[23]), &lAmsHdlVar[23], sizeof(m_strADSIs_GapLens2), m_strADSIs_GapLens2);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle GapLens - Track[2] : %d",lAmsHdlVar[23]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[24]), &lAmsHdlVar[24], sizeof(m_strADSIs_GapLens3), m_strADSIs_GapLens3);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle GapLens - Track[3] : %d",lAmsHdlVar[24]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[25]), &lAmsHdlVar[25], sizeof(m_strADSIs_GapLens4), m_strADSIs_GapLens4);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle GapLens - Track[4] : %d",lAmsHdlVar[25]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[26]), &lAmsHdlVar[26], sizeof(m_strADSIs_GapLens5), m_strADSIs_GapLens5);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle GapLens - Track[5] : %d",lAmsHdlVar[26]);



	 ///////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// No LENS //////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[27]), &lAmsHdlVar[27], sizeof(m_strADSIs_NoLens1), m_strADSIs_NoLens1);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle NoLens - Track[1] : %d",lAmsHdlVar[27]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[28]), &lAmsHdlVar[28], sizeof(m_strADSIs_NoLens2), m_strADSIs_NoLens2);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle NoLens - Track[2] : %d",lAmsHdlVar[28]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[29]), &lAmsHdlVar[29], sizeof(m_strADSIs_NoLens3), m_strADSIs_NoLens3);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle NoLens - Track[3] : %d",lAmsHdlVar[29]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[30]), &lAmsHdlVar[30], sizeof(m_strADSIs_NoLens4), m_strADSIs_NoLens4);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle NoLens - Track[4] : %d",lAmsHdlVar[30]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[31]), &lAmsHdlVar[31], sizeof(m_strADSIs_NoLens5), m_strADSIs_NoLens5);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle NoLens - Track[5] : %d",lAmsHdlVar[31]);


	  ///////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// NonCir LENS //////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[32]), &lAmsHdlVar[32], sizeof(m_strADSIs_NonCirLens1), m_strADSIs_NonCirLens1);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle NonCirLens - Track[1] : %d",lAmsHdlVar[32]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[33]), &lAmsHdlVar[33], sizeof(m_strADSIs_NonCirLens2), m_strADSIs_NonCirLens2);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle NonCirLens - Track[2] : %d",lAmsHdlVar[33]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[34]), &lAmsHdlVar[34], sizeof(m_strADSIs_NonCirLens3), m_strADSIs_NonCirLens3);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle NonCirLens - Track[3] : %d",lAmsHdlVar[34]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[35]), &lAmsHdlVar[35], sizeof(m_strADSIs_NonCirLens4), m_strADSIs_NonCirLens4);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle NonCirLens - Track[4] : %d",lAmsHdlVar[35]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[36]), &lAmsHdlVar[36], sizeof(m_strADSIs_NonCirLens5), m_strADSIs_NonCirLens5);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle NonCirLens - Track[5] : %d",lAmsHdlVar[36]);

	 ///////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// ONLINE //////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////


	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[37]), &lAmsHdlVar[37], sizeof(m_strADSLotChangedAck), m_strADSLotChangedAck);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle LotChanged Acknowledge: %d",lAmsHdlVar[5]);

	
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// ImageNumber ToServer //////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////


	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[38]), &lAmsHdlVar[38], sizeof(m_strADSImageNumber1), m_strADSImageNumber1);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle ImageNumber Track1 : %d",lAmsHdlVar[38]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[39]), &lAmsHdlVar[39], sizeof(m_strADSImageNumber2), m_strADSImageNumber2);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle ImageNumber Track2 : %d",lAmsHdlVar[39]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[40]), &lAmsHdlVar[40], sizeof(m_strADSImageNumber3), m_strADSImageNumber3);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle ImageNumber Track3 : %d",lAmsHdlVar[40]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[41]), &lAmsHdlVar[41], sizeof(m_strADSImageNumber4), m_strADSImageNumber4);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle ImageNumber Track4 : %d",lAmsHdlVar[41]);

	 nErr = AdsSyncReadWriteReq(&m_PlcAddress, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lAmsHdlVar[42]), &lAmsHdlVar[42], sizeof(m_strADSImageNumber5), m_strADSImageNumber5);
	 if (nErr) 
		OutputDebugLogTo(8,TRUE,"Error: AdsSyncReadWriteReq: %d" ,nErr);
	 else
		OutputDebugLogTo(8,TRUE,"Handle ImageNumber Track5 : %d",lAmsHdlVar[42]);

	 
	 
	 OutputDebugLogTo(8,TRUE,"Handle Creation Time: %.3f",ADSTimer.GetElapsedMSec());

	if(nErr !=0)
		return -1;

	//signal plc-connection as active
	m_bADSOpen = true;

	OutputDebugLogTo(8,TRUE,"ADS Interface successful");

	bAmsData[5] = true;
	nErr = AdsSyncWriteReq(&m_PlcAddress, ADSIGRP_SYM_VALBYHND, lAmsHdlVar[5], sizeof(bAmsData[5]), &bAmsData[5]); 
	if(nErr){
		Sleep(500);
		nErr = AdsSyncWriteReq(&m_PlcAddress, ADSIGRP_SYM_VALBYHND, lAmsHdlVar[5], sizeof(bAmsData[5]), &bAmsData[5]); 
		if(nErr)
			OutputDebugLogTo(8,TRUE,"Error in Writing ONLINE");
		else
			OutputDebugLogTo(8,TRUE,"RETRY :: Write ONLINE Successful");
	}
	else
		OutputDebugLogTo(8,TRUE,"Write ONLINE Successful");



	return 0;

}

bool CADSInterface::ClosePort(bool bRemoveHandles)
{
	int nError;
	CSingleLock(&m_csADS, TRUE);
	if (bRemoveHandles)
	{
		if(m_bhIPI_Common != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Common,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Common - from ADS Server");
		}

		if(m_bhChangeNotifyCommon !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyCommon);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Common - from ADS Server");
		}

		/*if(m_bhIPI_Start != 0) {
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Start,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Start - from ADS Server");
		}
           
		if(m_bhChangeNotifyStart !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyStart);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Start - from ADS Server");
		}*/
		
		if(m_bhIPI_First_Lens != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_First_Lens,0,0);
            if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_First_Lens - from ADS Server");
		} 

		if(m_bhChangeNotifyFirst_Lens !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyFirst_Lens);
             if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_First_Lens - from ADS Server");
		} 

		if(m_bhIPI_Tool != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Tool,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Tool - from ADS Server");
		} 

		if(m_bhChangeNotifyTool !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyTool);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Tool - from ADS Server");
		} 

		if(m_bhIPI_Mold != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Mold,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Mold - from ADS Server");
		} 

		if(m_bhChangeNotifyMold !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyMold);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Mold - from ADS Server");
		} 

		if(m_bhIPI_Arm != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Arm,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Arm - from ADS Server");
		} 


		if(m_bhChangeNotifyArm !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyArm);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Arm - from ADS Server");
		} 

		if(m_bhIPI_Lot_Nr1 != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Lot_Nr1,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Lot_Nr1 - from ADS Server");
		} 

		if(m_bhChangeNotifyLot_Nr1 !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyLot_Nr1);
            if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Lot_Nr1 - from ADS Server");
		}  

		if(m_bhIPI_Lot_Nr2 != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Lot_Nr2,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Lot_Nr2 - from ADS Server");
		}  

		if(m_bhChangeNotifyLot_Nr2 !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyLot_Nr2);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Lot_Nr2 - from ADS Server");
		}  

		if(m_bhIPI_Lot_Nr3 != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Lot_Nr3,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Lot_Nr3 - from ADS Server");
		}  

		if(m_bhChangeNotifyLot_Nr3 !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyLot_Nr3);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Lot_Nr3 - from ADS Server");
		}  

		if(m_bhIPI_Lot_Nr4 != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Lot_Nr4,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Lot_Nr4 - from ADS Server");
		}  

		if(m_bhChangeNotifyLot_Nr4 !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyLot_Nr4);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Lot_Nr4 - from ADS Server");
		}  

		if(m_bhIPI_Lot_Nr5 != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Lot_Nr5,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Lot_Nr5 - from ADS Server");
		}  

		if(m_bhChangeNotifyLot_Nr5 !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyLot_Nr5);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Lot_Nr5 - from ADS Server");
		}  
		/////////////////////////

		if(m_bhChangeNotifyColor1 !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyColor1);
            if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Color1 - from ADS Server");
		}  

		if(m_bhIPI_Color2 != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Color2,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Color2 - from ADS Server");
		}  

		if(m_bhChangeNotifyColor2 !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyColor2);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Color2 - from ADS Server");
		}  

		if(m_bhIPI_Color3 != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Color3,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Color3 - from ADS Server");
		}  

		if(m_bhChangeNotifyColor3 !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyColor3);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Color3 - from ADS Server");
		}  

		if(m_bhIPI_Color4 != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Color4,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Color4 - from ADS Server");
		}  

		if(m_bhChangeNotifyColor4 !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyColor4);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Color4 - from ADS Server");
		}  

		if(m_bhIPI_Color5 != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Color5,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Color5 - from ADS Server");
		}  

		if(m_bhChangeNotifyColor5 !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyColor5);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Color5 - from ADS Server");
		}

		//////////////////////////////


		
		if(m_bhChangeNotifyType1 !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyType1);
            if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Type1 - from ADS Server");
		}  

		if(m_bhIPI_Type2 != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Type2,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Type2 - from ADS Server");
		}  

		if(m_bhChangeNotifyType2 !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyType2);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Type2 - from ADS Server");
		}  

		if(m_bhIPI_Type3 != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Type3,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Type3 - from ADS Server");
		}  

		if(m_bhChangeNotifyType3 !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyType3);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Type3 - from ADS Server");
		}  

		if(m_bhIPI_Type4 != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Type4,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Type4 - from ADS Server");
		}  

		if(m_bhChangeNotifyType4 !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyType4);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Type4 - from ADS Server");
		}  

		if(m_bhIPI_Type5 != 0){
			nError = AdsSyncWriteReq(&m_PlcAddress,ADSIGRP_SYM_RELEASEHND,m_bhIPI_Type5,0,0);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncWriteReq : IPI_Type5 - from ADS Server");
		}  

		if(m_bhChangeNotifyType5 !=0){
			nError = AdsSyncDelDeviceNotificationReq(&m_PlcAddress,m_bhChangeNotifyType5);
			if(nError)
				OutputDebugLogTo(0,TRUE, "Error in AdsSyncDelDeviceNotificationReq : IPI_Type5 - from ADS Server");
		}

		if(m_nADSPort != 0)
		{
			m_nADSPort = 0;
			AdsPortClose();
		}

	}

	m_bhIPI_Common			= 0;
	m_bhChangeNotifyCommon	= 0;;

	/*m_bhIPI_Start			= 0;
	m_bhChangeNotifyStart	= 0;*/

	m_bhIPI_First_Lens		= 0;
	m_bhChangeNotifyFirst_Lens = 0;;

	m_bhIPI_Tool			= 0;
	m_bhChangeNotifyTool	= 0;

	m_bhIPI_Mold			= 0;
	m_bhChangeNotifyMold	= 0;

	m_bhIPI_Arm				= 0;
	m_bhChangeNotifyArm		= 0;
	
	m_bhIPI_Lot_Nr1			= 0;
	m_bhChangeNotifyLot_Nr1	= 0;
	
	m_bhIPI_Lot_Nr2			= 0;
	m_bhChangeNotifyLot_Nr2	= 0;
	
	m_bhIPI_Lot_Nr3			= 0;
	m_bhChangeNotifyLot_Nr3	= 0;
	
	m_bhIPI_Lot_Nr4			= 0;
	m_bhChangeNotifyLot_Nr4	= 0;
	
	m_bhIPI_Lot_Nr5			= 0;
	m_bhChangeNotifyLot_Nr5	= 0;
	
	m_bhIPI_Color1			= 0;
	m_bhChangeNotifyColor1	= 0;
	
	m_bhIPI_Color2			= 0;
	m_bhChangeNotifyColor2	= 0;
	
	m_bhIPI_Color3			= 0;
	m_bhChangeNotifyColor3	= 0;
	
	m_bhIPI_Color4			= 0;
	m_bhChangeNotifyColor4	= 0;
	
	m_bhIPI_Color5			= 0;
	m_bhChangeNotifyColor5	= 0;
	
	m_bhIPI_Type1			= 0;
	m_bhChangeNotifyType1	= 0;
	
	m_bhIPI_Type2			= 0;
	m_bhChangeNotifyType2	= 0;
	
	m_bhIPI_Type3			= 0;
	m_bhChangeNotifyType3	= 0;
	
	m_bhIPI_Type4			= 0;
	m_bhChangeNotifyType4	= 0;
	
	m_bhIPI_Type5			= 0;
	m_bhChangeNotifyType5	= 0;
	
	
	m_bADSOpen = false;
	return true;

}

BOOL CADSInterface::CheckConnection()
{
	CSingleLock loc(&m_csADS,TRUE);
	AmsNetId NetID;
	BYTE bNetID[6];
	
	USHORT nAdsState;
	USHORT nDeviceState;

	
	int nErr = AdsSyncReadStateReq(&m_PlcAddress, &nAdsState, &nDeviceState);

	//OutputDebugLogTo(8,TRUE,"ADSState = %d;DeviceState = %d;",nAdsState,nDeviceState);

	if(m_nPrevADSState != nAdsState)
	{
		if(nAdsState != 5){
			m_nPrevADSState = nAdsState;
			m_bADSAutoOpen = TRUE;
		}
	}

	//OutputDebugLogTo(8,TRUE,"Previous ADS State = %d ; ADSAuto = %d",m_nPrevADSState,m_bADSAutoOpen);
	
	if((m_nPrevADSState != 5) && (nAdsState == 5) && (nDeviceState ==0))
	{
		if(!m_bADSAutoOpen)
			return false;
	
		ClosePort(TRUE);
		Sleep(100);

		bNetID[0] = m_pCLApp->m_byteNetID[0];
		bNetID[1] = m_pCLApp->m_byteNetID[1];
		bNetID[2] = m_pCLApp->m_byteNetID[2];
		bNetID[3] = m_pCLApp->m_byteNetID[3];
		bNetID[4] = m_pCLApp->m_byteNetID[4];
		bNetID[5] = m_pCLApp->m_byteNetID[5];

		for(int i=0;i<6;i++)
			NetID.b[i] = bNetID[i];

		m_nADSPort = AdsPortOpen();  

		int nErr = AdsGetLocalAddress(&m_PlcAddress);
		  
		if(nErr) 
			OutputDebugLogTo(8,TRUE,"Error: AdsGetLocalAddress");

		m_PlcAddress.port = AMSPORT_R0_PLC_RTS1;
		m_PlcAddress.netId = NetID;
		Sleep(100);

		nErr = OpenPort();
		if(nErr)
			OutputDebugLogTo(8,TRUE,"Auto Reconnection Failed : ADS Communication Failed with the Server");
		else
			OutputDebugLogTo(8,TRUE,"Auto Reconnection Successful");
		Sleep(1000);
		m_bADSAutoOpen = FALSE;
		m_nPrevADSState = 5;


		////		ResetEvent(m_pCLApp->m_eventWaitForDIInterrupt);

		
	}
/*	else 
	{

		if(m_pCLApp->m_bOnline)
		{
			m_pCLApp->m_nADSServerFailCount++;
			if(m_pCLApp->m_nADSServerFailCount == 5)
			{	
				Sleep(100);
				ClosePort(TRUE);
				Sleep(500);
				m_pCLApp->ReconnectADSServer();
				m_pCLApp->m_nADSServerFailCount=0;
			}
		}
	
	}*/
	return nErr;
}


#pragma once

// ADSInterface.h : header file
//

#include "TcAdsDef.h"
#include "TcAdsApi.h"


class CADSInterface
{
public:
	CADSInterface();
	bool m_bLotChanged;
	BOOL OpenPort();
	bool ClosePort(bool bRemoveHandles = true);
	void SetTransTiming(int nMaxDelay, int nCycleTime, int nTimeout);

	BOOL CheckConnection();
	BOOL SetRemoteAddress(BYTE bNetID[6], unsigned int nPort);
	bool IsConnected() {return m_bADSOpen;};
    bool m_bADSAutoOpen;
	int m_nPrevADSState;
	struct IPI_In_Common
	{
		WORD w_lens_ipi_to_ipi_watchdog;
		WORD w_lens_ipi_inspection_setup;
		bool b_lens_ipi_lot_changed;
		bool b_packing_color_lenses;
	};

	/*struct IPI_In_Start
	{
		bool ab_lens_ipi_to_ipi_start_inspection_track[6];
	};*/

	struct IPI_In_First_Lens
	{
		bool ab_lens_ipi_to_ipi_first_lens_per_CT[5];
	};

	struct IPI_In_Tool
	{
		WORD aw_lens_ipi_to_ipi_tool_number[5];
	};
	struct IPI_In_Mold
	{
		WORD aw_lens_ipi_to_ipi_mold_number[5];
	};

	struct IPI_In_Arm
	{
		WORD aw_lens_ipi_to_ipi_gripper_arm_number[5];
	};

	struct IPI_In_Color1
	{
		char s_lens_color[15];
		
	};

	struct IPI_In_Color2
	{
		char s_lens_color[15];
		
	};

	struct IPI_In_Color3
	{
		char s_lens_color[15];
		
	};

	struct IPI_In_Color4
	{
		char s_lens_color[15];
		
	};

	struct IPI_In_Color5
	{
		char s_lens_color[15];
		
	};

	struct IPI_In_Type1
	{
		char s_lens_type[10];

	};

	struct IPI_In_Type2
	{
		char s_lens_type[10];

	};

	struct IPI_In_Type3
	{
		char s_lens_type[10];

	};

	struct IPI_In_Type4
	{
		char s_lens_type[10];

	};

	struct IPI_In_Type5
	{
		char s_lens_type[10];

	};

	struct IPI_In_Lot_Number1
	{
		char sLotNumber[8];
	};

	struct IPI_In_Lot_Number2
	{
		char sLotNumber[8];
	};

	struct IPI_In_Lot_Number3
	{
		char sLotNumber[8];
	};

	struct IPI_In_Lot_Number4
	{
		char sLotNumber[8];
	};

	struct IPI_In_Lot_Number5
	{
		char sLotNumber[8];
	};
	static IPI_In_Common		s_IPI_In_Common;
	//static IPI_In_Start			s_IPI_In_Start;
	static IPI_In_First_Lens	s_IPI_In_First_Lens;
	static IPI_In_Mold			s_IPI_In_Mold;
	static IPI_In_Tool			s_IPI_In_Tool;
	static IPI_In_Arm			s_IPI_In_Arm;
	static IPI_In_Lot_Number1	s_IPI_In_Lot_Number1;
	static IPI_In_Lot_Number2	s_IPI_In_Lot_Number2;
	static IPI_In_Lot_Number3	s_IPI_In_Lot_Number3;
	static IPI_In_Lot_Number4	s_IPI_In_Lot_Number4;
	static IPI_In_Lot_Number5	s_IPI_In_Lot_Number5;

	static IPI_In_Color1	s_IPI_In_Color1;
	static IPI_In_Color2	s_IPI_In_Color2;
	static IPI_In_Color3	s_IPI_In_Color3;
	static IPI_In_Color4	s_IPI_In_Color4;
	static IPI_In_Color5	s_IPI_In_Color5;

	static IPI_In_Type1	s_IPI_In_Type1;
	static IPI_In_Type2	s_IPI_In_Type2;
	static IPI_In_Type3	s_IPI_In_Type3;
	static IPI_In_Type4	s_IPI_In_Type4;
	static IPI_In_Type5	s_IPI_In_Type5;

	//int UpdateADSBusySignal(AmsAddr *pAddr, ULONG *plAmsHdlVar,int nTrack,int nStatus);

	int nTrackIdx;
	
	long m_nADSPort;
	bool m_bADSOpen;
	AmsAddr m_PlcAddress;
	
	ULONG m_bhIPI_Common;
	ULONG m_bhChangeNotifyCommon;

	/*ULONG m_bhIPI_Start;
	ULONG m_bhChangeNotifyStart;*/

	ULONG m_bhIPI_First_Lens;
	ULONG m_bhChangeNotifyFirst_Lens;

	ULONG m_bhIPI_Tool;
	ULONG m_bhChangeNotifyTool;

	ULONG m_bhIPI_Mold;
	ULONG m_bhChangeNotifyMold;

	ULONG m_bhIPI_Arm;
	ULONG m_bhChangeNotifyArm;

	ULONG m_bhIPI_Lot_Nr1;
	ULONG m_bhChangeNotifyLot_Nr1;

	ULONG m_bhIPI_Lot_Nr2;
	ULONG m_bhChangeNotifyLot_Nr2;

	ULONG m_bhIPI_Lot_Nr3;
	ULONG m_bhChangeNotifyLot_Nr3;

	ULONG m_bhIPI_Lot_Nr4;
	ULONG m_bhChangeNotifyLot_Nr4;

	ULONG m_bhIPI_Lot_Nr5;
	ULONG m_bhChangeNotifyLot_Nr5;

	ULONG m_bhIPI_Color1;
	ULONG m_bhChangeNotifyColor1;

	ULONG m_bhIPI_Color2;
	ULONG m_bhChangeNotifyColor2;

	ULONG m_bhIPI_Color3;
	ULONG m_bhChangeNotifyColor3;

	ULONG m_bhIPI_Color4;
	ULONG m_bhChangeNotifyColor4;

	ULONG m_bhIPI_Color5;
	ULONG m_bhChangeNotifyColor5;

	ULONG m_bhIPI_Type1;
	ULONG m_bhChangeNotifyType1;

	ULONG m_bhIPI_Type2;
	ULONG m_bhChangeNotifyType2;

	ULONG m_bhIPI_Type3;
	ULONG m_bhChangeNotifyType3;

	ULONG m_bhIPI_Type4;
	ULONG m_bhChangeNotifyType4;

	ULONG m_bhIPI_Type5;
	ULONG m_bhChangeNotifyType5;


	ULONG     lAmsHdlVar[45]; 
	WORD nData;
	bool bAmsData[45];
	int nAmsData[5];

	int m_nMaxDelay;
	int m_nCycleTime;

	CApplication* m_pCLApp;
	
	CCriticalSection m_csADS;



private:
	static void _stdcall OnPlcNotifyCommon(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Common(IPI_In_Common IPI_In_Common_Old);

	//static void _stdcall OnPlcNotifyStart(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	//void HandlePLC_Start(IPI_In_Start IPI_In_Start_Old);

	
	static void _stdcall OnPlcNotifyFirst_Lens(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_First_Lens(IPI_In_First_Lens IPI_In_First_LensOld);

	
	static void _stdcall OnPlcNotifyTool(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Tool(IPI_In_Tool IPI_In_Tool_Old);

	static void _stdcall OnPlcNotifyMold(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Mold(IPI_In_Mold IPI_In_Mold_Old);

	static void _stdcall OnPlcNotifyArm(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Arm(IPI_In_Arm IPI_In_Arm_Old);

	static void _stdcall OnPlcNotifyLot_Nr1(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Lot_Nr1(IPI_In_Lot_Number1 IPI_In_Lot_Number1_Old);

	static void _stdcall OnPlcNotifyLot_Nr2(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Lot_Nr2(IPI_In_Lot_Number2 IPI_In_Lot_Number2_Old);

	static void _stdcall OnPlcNotifyLot_Nr3(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Lot_Nr3(IPI_In_Lot_Number3 IPI_In_Lot_Number3_Old);

	static void _stdcall OnPlcNotifyLot_Nr4(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Lot_Nr4(IPI_In_Lot_Number4 IPI_In_Lot_Number4_Old);

	static void _stdcall OnPlcNotifyLot_Nr5(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Lot_Nr5(IPI_In_Lot_Number5 IPI_In_Lot_Number5_Old);

	static void _stdcall OnPlcNotifyColor1(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Color1(IPI_In_Color1 IPI_In_Color1_Old);

	static void _stdcall OnPlcNotifyColor2(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Color2(IPI_In_Color2 IPI_In_Color2_Old);

	static void _stdcall OnPlcNotifyColor3(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Color3(IPI_In_Color3 IPI_In_Color3_Old);

	static void _stdcall OnPlcNotifyColor4(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Color4(IPI_In_Color4 IPI_In_Color4_Old);

	static void _stdcall OnPlcNotifyColor5(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Color5(IPI_In_Color5 IPI_In_Color5_Old);

	static void _stdcall OnPlcNotifyType1(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Type1(IPI_In_Type1 IPI_In_Type1_Old);

	static void _stdcall OnPlcNotifyType2(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Type2(IPI_In_Type2 IPI_In_Type2_Old);

	static void _stdcall OnPlcNotifyType3(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Type3(IPI_In_Type3 IPI_In_Type3_Old);

	static void _stdcall OnPlcNotifyType4(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Type4(IPI_In_Type4 IPI_In_Type4_Old);

	static void _stdcall OnPlcNotifyType5(AmsAddr *pAddr,AdsNotificationHeader* pNotification, unsigned long hUser);

	void HandlePLC_Type5(IPI_In_Type5 IPI_In_Type5_Old);

public:
	virtual ~CADSInterface();


};

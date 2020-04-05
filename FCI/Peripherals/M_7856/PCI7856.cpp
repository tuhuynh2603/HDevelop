/// 7856.cpp: 7856 Motion Control interface
/// Udupa; March 2013;


#include "StdAfx.h"
#include "PCI7856.h"
#include "IOCode.h"
#include "PerformanceTimer.h"
#include "DebugLog.h"
#include <math.h>

int gWaitTimeOut = 10000;

I32 gIntNo[8][4];
int gStartAxis;
int busNo = 1;

//////////////////////////////////////////////////////////////////////////////

int _stdcall InitMotionDLL(int nReserved)
{
	I32 DPAC_ID_Bits;
	I32 nError = APS_initial(&DPAC_ID_Bits, 0x0);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "M_7856 APS_initial FAILED (Error=%d) **********", nError);
		return -1;
	}

	APS_set_field_bus_param(0, busNo, PRF_TRANSFER_RATE, 3);
	OutputDebugLogTo(0,FALSE,"Init M_7856 DLL");
	return 0;
}

int _stdcall FreeMotionDLL(long lReserved)
{
	I32 nError = APS_close();

	return (nError==0) ? 0 : -1;
}

int _stdcall SetWaitTimeOut(int nWaitTimeOut)
{
	gWaitTimeOut = nWaitTimeOut;
	return 0;
}

int PrepareMove(int nAxis)
{
	nAxis -= gStartAxis;
	for(int i=0; i<4; i++)
		APS_reset_int(gIntNo[nAxis][i]);

	return 0;
}

extern "C" int _stdcall WaitForMotionDone(int nAxis, int nTimeOut, BOOL bIntMode)
{
	 /* @ Return Value
		0 Stop
		1 Reserved
		2 Wait CSTA (Synchronous start signal)
		3 Wait Internal sync. signal
		4 Wait for other axis
		5 Wait ERC finished
		6 Wait DIR Change
		7 Backlash compensating
		8 Wait PA/PB
		9 In home special speed motion
		10 In start velocity motion
		11 In acceleration
		12 In Max velocity motion
		13 In deceleration
		14 Wait INP
		15 Other axis is still moving */

	if(bIntMode) {
		I32 nError = APS_wait_multiple_int(4, gIntNo[nAxis-gStartAxis], FALSE, nTimeOut); 
		if(nError == ERR_TimeOut)
			return -3;
		if(nError < ERR_NoError) {
			OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] APS_wait_multiple_int FAILED (Error=%d) **********", nAxis, nError);
			return -1;
		}
		//OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] WaitForMotionDone Interrupt received", nAxis);

		if(nError != 0) {
			OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] WaitForMotionDone ERROR (Alarm/EL)", nAxis);
			return -2;
		}
	}
	else {
		CPerformanceTimer timer;
		while(TRUE) {
			I32 nIOStatus = APS_motion_status(nAxis);
			if(nIOStatus < ERR_NoError) {
				OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] GetIOStatus FAILED; (Error=%d) **********", nAxis, nIOStatus);
				return -1;
			}
			if(nIOStatus & 0xFD0021) { //0, 5, 16, 18, 19, 20, 21, 22
				if(nIOStatus & 0x01)
					break;
				else {
					OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] WaitForMotionDone ERROR (Status: 0x%X)", nAxis, nIOStatus);
					return -2;
				}
			}
			
			Sleep(10);
			if((nTimeOut != INFINITE) && (timer.GetElapsedMSec() > nTimeOut))
				return -3;
		}
	}
	
	OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] Motion Complete", nAxis);
	return 0;
}

extern "C" int _stdcall RegisterCard(int cardNo, int nStartAxis)
{
	OutputDebugLogTo(0, TRUE, "M_7856_Motion RegisterCard %d (%d)", cardNo, nStartAxis);

	/// Bus_No: 0-HSL; 1_MNet
	I32 nError = APS_start_field_bus(cardNo, busNo, nStartAxis);
	if(nError != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "M_7856_Motion *** APS_start_field_bus(MNET) FAILED (Error=%d) **********", nError);

	gStartAxis = nStartAxis;

	return 0;
}

extern "C" int _stdcall ReleaseCard(int cardNo)
{
	/// Bus_No: 0-HSL; 1_MNet
	I32 nError = APS_stop_field_bus(cardNo, 1);
	if(nError != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "M_7856_Motion *** APS_stop_field_bus(MNET) FAILED (Error=%d) **********", nError);

	return (nError==0) ? 0 : -1;
}

extern "C" int _stdcall LoadConfigFromFile(char* strParmFile)
{
	int nError = APS_load_param_from_file(strParmFile);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "M_7856 *** APS_load_param_from_file FAILED; File: %s (Error=%d) **********", strParmFile, nError);
		return -1;
	}

	OutputDebugLogTo(0, TRUE, "M_7856 APS_load_param_from_file done; File: %s", strParmFile);

	return 0;
}

extern "C" int _stdcall GetAxisCount()
{
	return AXIS_COUNT;
}

extern "C" int _stdcall GetIOStatus(int nAxis, U32* ioStatus)
{
	I32 nIOStatus = APS_motion_io_status(nAxis);
	if(nIOStatus < ERR_NoError) {
		OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] GetIOStatus FAILED; (Error=%d) **********", nAxis, nIOStatus);
		return -1;
	}

	*ioStatus = nIOStatus;
/*
	OutputDebugLogTo(7,TRUE,"GetIOStatus APS_motion_io_status %d ",*ioStatus);*/
	return 0;
}

extern "C" int _stdcall GetAlarmStatus(int nAxis, BOOL* bAlarm, BOOL* bEl)
{
	U32 ioStatus;
	if(GetIOStatus(nAxis, &ioStatus) != 0)
		return -1;

	*bAlarm = ioStatus & 0x01;
	*bEl = ioStatus & 0x06;

	return 0;
}

extern "C" int _stdcall EnableInterrupt(int cardNo, int nEnable)
{
	I32 nError = APS_int_enable(cardNo, nEnable);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "M_7856_Motion *** APS_int_enable FAILED (Error=%d) **********", nError);
		return -1;
	}

	OutputDebugLogTo(0, TRUE, "M_7856_Motion APS_int_enable done; Card No: %d", cardNo);
	return 0;
}

extern "C" int _stdcall ResetFieldBusInterrupt(int nAxis)
{
	I32 nError = APS_reset_field_bus_int_motion(nAxis);
	if(nError < ERR_NoError) {
		OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] APS_reset_field_bus_int_motion FAILED (Error=%d) **********", nAxis, nError);
		return -1;
	}

	OutputDebugLogTo(0, TRUE, "M_7856_Motion APS_reset_field_bus_int_motion done; Axis No: %d", nAxis);
	return 0;
}

extern "C" int _stdcall SetIntFactor(int nAxis, int nIndex, U32 intFactorNo, BOOL bIsMotionInt)
{
	I32 nError;
	if(bIsMotionInt)
		nError = APS_set_field_bus_int_factor_motion(nAxis, intFactorNo, 1);
	else
		nError = APS_set_field_bus_int_factor_error(nAxis, intFactorNo, 1);
	if(nError < ERR_NoError) {
		OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] APS_set_field_bus_int_factor FAILED (Error=%d) **********", nAxis, nError);
		return -1;
	}
	gIntNo[nAxis-gStartAxis][nIndex] = nError;

	OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] SetIntFactor done; intFactor=%d (Event=%d)", nAxis, intFactorNo, nError);
	return 0;
}

extern "C" int _stdcall SetServoOn(int nAxis, int servoOn)
{
	I32 nError = APS_set_servo_on(nAxis, servoOn);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "M_7856[Axis%d] SetServoOn FAILED [nErr=%d]**********", nAxis, nError);
		return -1;
	}

	OutputDebugLogTo(0, TRUE, "M_7856[Axis%d] SetServoOn done; servoOn=%d", nAxis, servoOn);
	return 0;
}

extern "C" int _stdcall InitializeAxis(int nAxis)
{
	SetIntFactor(nAxis, 0, 0, TRUE);
	SetIntFactor(nAxis, 1, 3, FALSE);
	SetIntFactor(nAxis, 2, 4, FALSE);
	SetIntFactor(nAxis, 3, 5, FALSE);

	//SetServoOn(nAxis, 1);

	I32 nError = APS_set_axis_param(nAxis, PRA_CURVE, 0); ///0:T-Curve 1:S-Curve
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "M_7856[Axis%d] Set S-Curve FAILED **********", nAxis);
		return -1;
	}

	//SetAcceleration(nAxis, 100000, 100000);

	return 0;
}

extern "C" int _stdcall FreeAxis(int nAxis)
{
	I32 nError = APS_stop_move(nAxis);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] APS_stop_move FAILED (Error=%d) **********", nAxis, nError);
		return -1;
	}

	return 0;
}

extern "C" int _stdcall SetHomeMode(int nAxis, int homeMode)
{
	//OutputDebugLogTo(0, TRUE, "M_7856[Axis%d] Entering home mode", nAxis);
	I32 nError1 = APS_set_axis_param(nAxis, PRA_HOME_MODE, homeMode);
	if(nError1 != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "M_7856[Axis%d] PRA_HOME_MODE FAILED (Error=%d) **********", nAxis, nError1);
	
	OutputDebugLogTo(0, TRUE, "M_7856[Axis%d] SetHomeMode done; homeMode=%d", //; orgLogic=%d; ezLogic=%d; moveRatio=1", 
		nAxis, homeMode); //, orgLogic, ezLogic);

	return 0;
}

extern "C" int _stdcall SetAcceleration(int nAxis, int nAcc, int nDecc)
{
	I32 nError = APS_set_axis_param(nAxis, PRA_ACC, nAcc);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] APS_set_axis_param(PRA_ACC) FAILED (Error=%d) **********", nAxis, nError);
		return -1;
	}
	
	nError = APS_set_axis_param(nAxis, PRA_DEC, nDecc);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] APS_set_axis_param(PRA_DEC) FAILED (Error=%d) **********", nAxis, nError);
		return -1;
	}
	
	OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] Acceleration set to %d; Decceleration set to %d (pulses/secsq)", nAxis, nAcc, nDecc);
	return 0;
}

extern "C" int _stdcall AbsoluteMove(int nAxis, int nPos, int nMaxVel, BOOL bWait)
{
	PrepareMove(nAxis);

	I32 nError = APS_absolute_move(nAxis, nPos, nMaxVel);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] AbsoluteMove FAILED (Error=%d) **********", nAxis, nError);
		return -1;
	}

	OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] AbsoluteMove done; Pos=%d; MaxVel=%d; Wait=%d", 
		nAxis, nPos, nMaxVel, bWait);
	if(bWait) {
		nError = WaitForMotionDone(nAxis, gWaitTimeOut, FALSE);
		if(nError) {
			OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] APS_absolute_move FAILED (Wait Error=%d) **********", nAxis, nError);
			return nError;
		}
	}

	return 0;
}

extern "C" int _stdcall RelativeMove(int nAxis, int nPos, int nMaxVel, BOOL bWait)
{
	PrepareMove(nAxis);

	I32 nError = APS_relative_move(nAxis, nPos, nMaxVel);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] RelativeMove FAILED (Error=%d) **********", nAxis, nError);
		return -1;
	}

	OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] RelativeMove done; Pos=%d; MaxVel=%d; Wait=%d", 
		nAxis, nPos, nMaxVel, bWait);

	if(bWait) {
		nError = WaitForMotionDone(nAxis, gWaitTimeOut, FALSE);
		if(nError) {
			OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] APS_relative_move FAILED (Wait Error=%d) **********", nAxis, nError);
			return nError;
		}
	}

	return 0;
}

extern "C" int _stdcall SetCurrentPosition(int nAxis, int nPosition)
{
	I32 nError = APS_set_position(nAxis, nPosition);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] APS_set_position FAILED; (Error=%d) **********", nAxis, nError);
		return -1;
	}
	nError = APS_set_command(nAxis, nPosition);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] APS_set_command FAILED; (Error=%d) **********", nAxis, nError);
		return -1;
	}
	//nError = _7856_reset_target_pos(nAxis, position); // set new value for the target position recorder (S\W) 

	OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] SetCurrentPosition done; position=%d", nAxis, nPosition);
	return 0;
}

extern "C" int _stdcall WaitForHomeDone(int nAxis, int nTimeOut)
{
	int nError = WaitForMotionDone(nAxis, (nTimeOut<0) ? INFINITE : nTimeOut, FALSE);
	if(nError) {
		OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] _7856_home_move FAILED (Wait Error=%d) **********", nAxis, nError);
		return nError;
	}

	Sleep(2000);
	SetCurrentPosition(nAxis, 0);
	OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] HomeMove done", nAxis);

	return 0;
}

extern "C" int _stdcall HomeMove(int nAxis, int nMode, int nDir, int nStartVel, int nMaxVel, BOOL bWait)
{
	OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] Start Home Move (Direction=%d, StartVel=%d, MaxVel=%d)", nAxis, nDir, nStartVel, nMaxVel);
	I32 nError;

	APS_set_axis_param(nAxis, PRA_HOME_MODE, nMode);
	APS_set_axis_param(nAxis, PRA_HOME_DIR, nDir);
	APS_set_axis_param(nAxis, PRA_VS, nStartVel);
	APS_set_axis_param(nAxis, PRA_HOME_VM, nMaxVel);
	APS_set_axis_param(nAxis, PRA_HOME_VO, __max(nStartVel/10, 100)); //Set homing FA velocity. *1
	APS_set_axis_param(nAxis, PRA_HOME_OFFSET, 100);
	
	PrepareMove(nAxis);

	nError = APS_home_move(nAxis);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] _7856_home_search FAILED (Error=%d) **********", nAxis, nError);
		return -1;
	}

	if(bWait)
		return WaitForHomeDone(nAxis, -1);

	return 0;
}

extern "C" int _stdcall Stop(int nAxis, int nDecc)
{
	I32 nError = APS_stop_move(nAxis);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] APS_stop_move FAILED (Error=%d) **********", nAxis, nError);
		return -1;
	}
	
	OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] Stop done", nAxis);
	return 0;
}

extern "C" int _stdcall GetCurrentPosition(int nAxis, int* nPosition)
{
	I32 nPos;
	I32 nError = APS_get_position(nAxis, &nPos);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] GetCurrentPosition FAILED (Error=%d) **********", nAxis, nError);
		return -1;
	}
	*nPosition = nPos;

	///OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] GetCurrentPosition done; position=%d", nAxis, nPos);
	return 0;
}

extern "C" int _stdcall EmergencyStop(int nAxis)
{
	I32 nError = APS_emg_stop(nAxis);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(1, TRUE, "----- M_7856[Axis%d] EmergencyStop FAILED (Error=%d) **********", nAxis, nError);
		return -1;
	}
	
	OutputDebugLogTo(1, TRUE, "M_7856[Axis%d] EmergencyStop done", nAxis);

	return 0;
}

extern "C" int _stdcall SetAlarm(int nAxis)
{
	I32 nError;

	nAxis -= gStartAxis;
	for(int i = 0; i < 4; i++) {
		nError = APS_set_int(gIntNo[nAxis][i]);
		if(nError != ERR_NoError) {
			OutputDebugLogTo(1, TRUE, "----- M_7856 SetAlarm FAILED (Error=%d) **********", nError);
			return -1;
		}
	}
	OutputDebugLogTo(1, TRUE, "M_7856 SetAlarm done");

	return 0;
}

extern "C" int _stdcall ResetAlarm(int nCardNo, int nPortNo, int nAxis)
{
	I32 ModuleNo;
	I32 DO_Value;
	I32 nError;

	ModuleNo = nPortNo;
	DO_Value = 0x0;

	nError = SetServoOn(nAxis, 0);
	if(nError) {
		return nError;	
	}

	nError = APS_field_bus_d_get_output( nCardNo, 1, ModuleNo, &DO_Value );
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- M_7856 ResetAlarm[get_output1] FAILED (Error=%d) **********", nError);
		return -1;
	}

	if(DO_Value == 0xff) {
		DO_Value = 0xef;
		//else if(DO_Value == 0xef) 
		//	DO_Value = 0xff;

		nError = APS_field_bus_d_set_output(nCardNo, 1, ModuleNo, DO_Value);
		if(nError != ERR_NoError) {
			OutputDebugLogTo(0, TRUE, "----- M_7856 ResetAlarm[set_output1] FAILED (Error=%d) **********", nError);
			return -1;
		}
	}
	Sleep(200);

	nError = SetServoOn(nAxis, 1);
	if(nError) {
		return nError;	
	}
	return 0;
}

extern "C" int _stdcall InPortRead(int nCardNo, int nPortNo, UINT* pData)
{
	I32 DO_Value;
	I32 nError = APS_field_bus_d_get_input(nCardNo, 1, nPortNo, &DO_Value);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- M_7856 InPortRead FAILED (Error=%d) **********",nError);
		return -1;
	}

	*pData = DO_Value;

	return 0;
}


int _stdcall InPortReadBit(int cardNo, int nPortNo, int Ch_No, UINT *pData)
{
	I32 DO_Value;
	
	I32 nError = APS_get_field_bus_d_channel_input(cardNo,1, nPortNo, Ch_No, &DO_Value);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_get_field_bus_d_channel_input() function with parameters 0, %d, %d failed with Error %d for Card %d", nPortNo, Ch_No, nError, cardNo);
		return -1;
	}
	*pData = DO_Value;
	return 0;
}


extern "C" int _stdcall OutPortWrite(int nCardNo, int nPortNo, UINT nData)
{
	I32 DO_Value;
	DO_Value = 0x0;

	if(nData==0)
		DO_Value = 0xef;	//// ON
	else if(nData==1)
		DO_Value = 0xff;	//// OFF

	I32 nError = APS_field_bus_d_set_output(nCardNo, 1, nPortNo, DO_Value);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- M_7856 OutPortWrite FAILED (Error=%d) **********", nError);
		return -1;
	}

	return 0;
}


extern "C" int _stdcall OutPortWriteBit(int nCardNo, int nPortNo,int nChannelNo, UINT nData)
{
	I32 DO_Value;
	DO_Value = 0x0;

	if(nData==0)
		DO_Value = 0xef;	//// ON
	else if(nData==1)
		DO_Value = 0xff;	//// OFF

	I32 nError = APS_set_field_bus_d_channel_output(nCardNo, 1, nPortNo,nChannelNo,DO_Value);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- M_7856 OutPortWrite FAILED (Error=%d) **********", nError);
		return -1;
	}

	return 0;
}

extern "C" int _stdcall OutPortReadBit(int cardNo, int nPortNo, int Ch_No, UINT *DO_Value)
{
	I32* p32Data = (I32*) DO_Value;
	I32 nError = APS_get_field_bus_d_channel_output(cardNo, 0, nPortNo, Ch_No, p32Data);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_get_field_bus_d_channel_output() function with parameters 0, %d, %d failed with Error %d for Card %d", nPortNo, Ch_No, nError, cardNo);
		return -1;
	}

	return 0;
}


extern "C" int _stdcall GetServoAlarmStatus(int nAxis, long* lAlarmNo, long* lAlarmDetail)
{
	I32 Alarm_No;
	I32 Alarm_Detail;

	I32 nError = APS_get_sscnet_servo_alarm(nAxis, &Alarm_No, &Alarm_Detail);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- M_7856 GetServoAlarmStatus FAILED (Error=%d) **********", nError);
		return -1;
	}

	*lAlarmNo = Alarm_No;
	*lAlarmDetail = Alarm_Detail;

	return 0;
}

extern "C" int _stdcall AbsoluteLinearMove(int nDimension, int* pAxisIDArray, int* pPositionArray, int nMaxLinearSpeed)
{
	//// Maximum of 4 axes linear interpolation ////
	//// Absolute Linear Move - 2 axes(X & Y) linear interpolation ////
	I32 Dimension, Max_Linear_Speed;
	I32 Axis_ID_Array[4], Position_Array[4];

	Dimension = nDimension;
	Max_Linear_Speed = nMaxLinearSpeed;
	for(int i=0; i<2; i++) {
		Axis_ID_Array[i] = pAxisIDArray[i];
		Position_Array[i] = pPositionArray[i];
	}

	I32 nError = APS_absolute_linear_move(Dimension, Axis_ID_Array, Position_Array, Max_Linear_Speed);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- M_7856 AbsoluteLinearMove FAILED (Error=%d) **********", nError);
		return -1;
	}
}

extern "C" int _stdcall RelativeLinearMove(int nDimension, int* pAxisIDArray, int* pDistanceArray, int nMaxLinearSpeed)
{
	//// Maximum of 4 axes linear interpolation ////
	//// Relative Linear Move - 2 axes(X & Y) linear interpolation ////
	I32 Dimension, Max_Linear_Speed;
	I32 Axis_ID_Array[4], Distance_Array[4];

	Dimension = nDimension;
	Max_Linear_Speed = nMaxLinearSpeed;
	for(int i=0; i<2; i++) {
		Axis_ID_Array[i] = pAxisIDArray[i];
		Distance_Array[i] = pDistanceArray[i];
	}

	I32 nError = APS_relative_linear_move(Dimension, Axis_ID_Array, Distance_Array, Max_Linear_Speed);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- M_7856 RelativeLinearMove FAILED (Error=%d) **********", nError);
		return -1;
	}
}




extern "C" int _stdcall SetSoftLimits(int nAxis, int nNegLimit, int nPosLimit)
{
	if(nNegLimit >= nPosLimit)
		return -1;

	I32 nError = APS_set_axis_param(nAxis, PRA_SPEL_EN, 2);
	if(nError != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_SPEL_EN, 2 failed with Error %d for Axis %d", nError, nAxis);

	nError = APS_set_axis_param(nAxis, PRA_SMEL_EN, 2);
	if(nError != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_SMEL_EN, 2 failed with Error %d for Axis %d", nError, nAxis);
	
	nError = APS_set_axis_param(nAxis, PRA_EFB_POS0, nPosLimit);
	if(nError != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_EFB_POS0, %d failed with Error %d for Axis %d", nPosLimit, nError, nAxis);

	nError = APS_set_axis_param(nAxis, PRA_EFB_POS1, nNegLimit);
	if(nError != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_EFB_POS1, %d failed with Error %d for Axis %d", nNegLimit, nError, nAxis);

	OutputDebugLogTo(1, TRUE, "PCI-7856 Soft limits [%d - %d] are set for Axis %d", nNegLimit, nPosLimit, nAxis);

	return 0;
}

extern "C" int _stdcall ResetSoftLimits(int nAxis)
{
	I32 nError = APS_set_axis_param(nAxis, PRA_SPEL_EN, 0);
	if(nError != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_SPEL_EN, 0 failed with Error %d for Axis %d", nError, nAxis);

	nError = APS_set_axis_param(nAxis, PRA_SMEL_EN, 0);
	if(nError != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_SMEL_EN, 0 failed with Error %d for Axis %d", nError, nAxis);

	OutputDebugLogTo(1, TRUE, "PCI-7856 Soft limits are reset for Axis %d", nAxis);

	return 0;
}
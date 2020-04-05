/// 7856.cpp: 7856 Motion Control interface for MNET-4XMO-C
/// Ihar; April 2014 - November 2015

#include "StdAfx.h"
#include "7856.h"
#include "DebugLog.h"
//#include "PerformanceTimer.h"

int gWaitTimeOut = 10000;

//The number of axis is 16 for PCI-7856
//MNET-4XMO-C
I32 gIntNo[16][7];
bool axisEnabled[16] = {false};
int gStartAxis = 0;

I32 gLatchEvent[16];

bool alarmSet = false;

//////////////////////////////////////////////////////////////////////////////

int _stdcall InitMotionDLL()
{
	I32 DPAC_ID_Bits;
	//0x40 PCI-7856 only to support motion interrupt
	I32 nError = APS_initial(&DPAC_ID_Bits, 0x40);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_initial() function failed with Error %d", nError);
		return nError;
	}

	OutputDebugLogTo(0, TRUE, "Initialization of PCI-7856 Library completed");
	return 0;
}

int _stdcall FreeMotionDLL()
{
	I32 nError = APS_close();
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_close() function failed with Error %d", nError);
		return nError;
	}

	OutputDebugLogTo(0, FALSE, "Closing PCI-7856 Library completed");

	return 0;
}

void _stdcall SetWaitTimeOut(int nWaitTimeOut)
{
	gWaitTimeOut = nWaitTimeOut;
}

int PrepareMove(int nAxis)
{
	I32 nError;
	nAxis -= gStartAxis;
	for(int i=0; i<7; i++) {
		nError = APS_reset_int(gIntNo[nAxis][i]);
		if(nError != ERR_NoError) {
			OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_reset_int() function failed with Error %d for axis %d with interrupt value %d", nError, nAxis + gStartAxis, gIntNo[nAxis][i]);
			return nError;
		}
	}

	return 0;
}

int _stdcall WaitForAnyMotionError(int nAxisNo, int * pnAxisID, int nTimeOut)
{
	if(nAxisNo < 1)
		return -1;

	I32 nError = ERR_NoError;

	I32 * pnErrorInterrupt = new I32 [nAxisNo];

	for(int i=0;i<nAxisNo;i++) {
		pnErrorInterrupt[i] = gIntNo[pnAxisID[i] - gStartAxis][5];
		nError = APS_reset_int(pnErrorInterrupt[i]);
		if(nError != ERR_NoError) {
			OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_reset_int() function failed with Error %d for axis %d with interrupt value %d", nError, pnAxisID[i], pnErrorInterrupt[i]);
			delete [] pnErrorInterrupt;
			return nError;
		}
	}

	nError = APS_wait_multiple_int(nAxisNo, pnErrorInterrupt, FALSE, nTimeOut);

	delete [] pnErrorInterrupt;

	if(alarmSet) {
		alarmSet = false;
		return 0;
	}

	if(nError == ERR_TimeOut)
		return 1;

	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_wait_multiple_int() function failed with Error %d", nError);
		return -1;
	}	

	return 0;
}

int _stdcall SetAlarm()
{
	alarmSet = true;

	I32 nError;

	for(int i = 0; i < 16; i++) {
		if(axisEnabled[i]) {
			nError = APS_set_int(gIntNo[i][5]);
			if(nError != ERR_NoError) {
				OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_int() function failed with Error %d", nError);
				return -1;
			}
		}
	}

	return 0;
}

int _stdcall WaitForMotionDone(int nAxis, int nTimeOut)
{
	I32 nError = APS_wait_multiple_int(7, gIntNo[nAxis-gStartAxis], FALSE, nTimeOut); 
	if(nError == ERR_TimeOut) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 Waiting time out for axis %d", nAxis);
		return -3;
	}

	if(nError < ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_wait_multiple_int() function failed with Error %d", nError);
		return -1;
	}

	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_wait_multiple_int() function failed with Alarm/EL");
		return -2;
	}
	
	OutputDebugLogTo(1, TRUE, "PCI-7856 Motion Completed for axis %d", nAxis);

	return 0;
}

int _stdcall LoadConfigFromFile(char* strParmFile)
{
	int nError = APS_load_param_from_file(strParmFile);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_load_param_from_file() function failed with Error %d trying to load file %s", nError, strParmFile);
		return -1;
	}

	OutputDebugLogTo(0, TRUE, "PCI-7856 Parameter file %s has been loaded successfully", strParmFile);

	return 0;
}

int _stdcall RegisterCard(int cardNo, int nStartAxis)
{
	//For PCI-7856 Bus No for Motion (Motionnet) is 1, for I/O (HSL) is 0
	OutputDebugLogTo(0, TRUE, "PCI-7856 Registering MNET Card %d with start axis %d", cardNo, nStartAxis);

	I32 nError = APS_start_field_bus(cardNo, 1, nStartAxis);
	if(nError != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_start_field_bus() function for MNET failed with Error %d", nError);

	gStartAxis = nStartAxis;

	return 0;
}

int _stdcall ReleaseCard(int cardNo)
{
	I32 nError = APS_stop_field_bus(cardNo, 1);
	if(nError != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_stop_field_bus() function for MNET failed with Error %d", nError);

	return (nError==0) ? 0 : -1;
}

int _stdcall RegisterIOCard(int cardNo)
{
	//For PCI-7856 Bus No for Motion (Motionnet) is 1, for I/O (HSL) is 0

	OutputDebugLogTo(0, TRUE, "PCI-7856 Registering HSL Card %d", cardNo);

	I32 nError;

	nError = APS_set_field_bus_param(cardNo, 0, PRF_COMMUNICATION_TYPE, 1); //duplex connection
	if(nError != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_param() function with parameters PRF_COMMUNICATION_TYPE, 1 for HSL failed with Error %d", nError);

    nError = APS_set_field_bus_param(cardNo, 0, PRF_TRANSFER_RATE, 2); //6Mb is for duplex
	if(nError != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_param() function with parameters PRF_TRANSFER_RATE, 2 for HSL failed with Error %d", nError);

    nError = APS_set_field_bus_param(cardNo, 0, PRF_HUB_NUMBER, 0);
	if(nError != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_param() function with parameters PRF_HUB_NUMBER, 0 for HSL failed with Error %d", nError);

	nError = APS_start_field_bus(cardNo, 0, 0);//16500
	if(nError != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_start_field_bus() function for HSL failed with Error %d", nError);

	return 0;
}

int _stdcall ReleaseIOCard(int cardNo)
{
	I32 nError = APS_stop_field_bus(cardNo, 0);
	if(nError != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_stop_field_bus() function for HSL failed with Error %d", nError);

	return (nError==0) ? 0 : -1;
}

int _stdcall GetIOStatus(int nAxis, long & ioStatus)
{
	ioStatus = APS_motion_io_status(nAxis);
	if(ioStatus < ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_motion_io_status() function failed with Error %d for Axis %d", ioStatus, nAxis);
		return -1;
	}

	return 0;
}

int _stdcall GetMotorStatus(int nAxis, long & nMotorStatus)
{
	nMotorStatus = APS_motion_status(nAxis);
	if(nMotorStatus < 0) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_motion_status() function failed with Error %d for Axis %d", nMotorStatus, nAxis);
		return -1;
	}

	//Bit table definition
	//  0: Command stopped
	//  1: At maximum velocity
	//  2: At acceleration
	//  3: At deceleration
	//  4: ------
	//  5: Normal stop(Motion done)
	//  6: In homing
	//  7: Single axis move( relative, absolute, velocity move)
	//  8: Linear interpolation
	//  9: Circular interpolation
	// 10: At start velocity
	// 11: ------
	// 12: ------
	// 13: ------
	// 14: ------
	// 15: ------
	// 16: Abnormal stop
	// 17: Servo off stopped
	// 18: EMG / SEMG stopped
	// 19: Alarm stop
	// 20: ------
	// 21: PEL stopped
	// 22: MEL stopped
	// 23: ------
	// 24: SPEL stopped
	// 25: SMEL stopped

	return 0;
}

int _stdcall GetAlarmStatus(int nAxis, BOOL* bAlarm, BOOL* bEl)
{
	long ioStatus;
	if(GetIOStatus(nAxis, ioStatus) != 0)
		return -1;

	*bAlarm = ioStatus & 0x01;
	*bEl = ioStatus & 0x06;

	return 0;
}

int _stdcall EnableInterrupt(int cardNo, int nEnable)
{
	I32 nError = APS_int_enable(cardNo, nEnable);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_int_enable() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}

	OutputDebugLogTo(1, TRUE, "PCI-7856  Interrupt is enabled for Card %d", cardNo);

	return 0;
}

int SetIntFactor(int nAxis, int nIndex, U32 intFactorNo, const bool bNoErrorInterrupt)
{
	I32 nInterruptNo;

	if(bNoErrorInterrupt) {

		nInterruptNo = APS_set_field_bus_int_factor_motion(nAxis, intFactorNo, 1);
		if(nInterruptNo < ERR_NoError) {
			OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_int_factor_motion() function failed with Error %d for Axis %d", nInterruptNo, nAxis);
			return -1;
		}

		gIntNo[nAxis-gStartAxis][nIndex] = nInterruptNo;
	}
	else {


		nInterruptNo = APS_set_field_bus_int_factor_error(nAxis, intFactorNo, 1);
		if(nInterruptNo < ERR_NoError) {
			OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_int_factor() function failed with Error %d for Axis %d", nInterruptNo, nAxis);
			return -1;
		}

		gIntNo[nAxis-gStartAxis][nIndex] = nInterruptNo;
	}
	
	
	return 0;
}

int _stdcall SetServoOn(int nAxis, int servoOn)
{
	I32 nError = APS_set_servo_on(nAxis, servoOn);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_servo_on() function failed with Error %d for Axis %d", nError, nAxis);
		return -1;
	}

	return 0;
}

int _stdcall InitializeAxis(int nAxis)
{
	//Interruption table for MNET-4XMO-C
	SetIntFactor(nAxis, 0, 0, true);	// Normal stop
	SetIntFactor(nAxis, 1, 0, false);	// +Soft limit is ON and axis is stopped
	SetIntFactor(nAxis, 2, 1, false);	// -Soft limit is ON and axis is stopped
	SetIntFactor(nAxis, 3, 5, false);	// +End limit is on and axis is stopped
	SetIntFactor(nAxis, 4, 6, false);	// -End limit is on and axis is stopped
	SetIntFactor(nAxis, 5, 7, false);	// ALM is happened and axis is stopped
	SetIntFactor(nAxis, 6, 9, false);	// EMG is on and axis is stopped

	axisEnabled[nAxis-gStartAxis] = true;

	/*I32 nError = APS_set_axis_param(nAxis, PRA_CURVE, 1);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "M_7856 [Axis%d] Set T-Curve FAILED **********", nAxis);
		return -1;
	}*/

	I32 nError = APS_reset_field_bus_int_motion(nAxis);
	if(nError < ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_reset_field_bus_int_motion() function failed with Error %d for Axis %d", nError, nAxis);
		return -1;
	}

	return 0;
}

int _stdcall FreeAxis(int nAxis)
{

	OutputDebugLogTo(7, TRUE, "Axis - %d, Before stop", nAxis);

	I32 nError = APS_stop_move(nAxis);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_stop_move() function failed with Error %d for Axis %d", nError, nAxis);
		return -1;
	}
	OutputDebugLogTo(7, TRUE, "Axis - %d, after stop", nAxis);

	nError = APS_set_servo_on(nAxis, 0);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_servo_on() function failed with Error %d for Axis %d", nError, nAxis);
		return -1;
	}

	OutputDebugLogTo(7, TRUE, "Axis - %d, after servo off", nAxis);

	return 0;
}

int _stdcall SetHomeMode(int nAxis, int homeMode)
{
	I32 nError = APS_set_axis_param(nAxis, PRA_HOME_MODE, homeMode);
	if(nError != ERR_NoError)
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_HOME_MODE, %d failed with Error %d for Axis %d", homeMode, nError, nAxis);
	
	OutputDebugLogTo(1, TRUE, "PCI-7856 Home Mode %d for Axis %d is set", homeMode, nAxis);

	return 0;
}

int _stdcall SetSoftLimits(int nAxis, int nNegLimit, int nPosLimit)
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

int _stdcall ResetSoftLimits(int nAxis)
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

int _stdcall SetAcceleration(int nAxis, int nAcc, int nDecc)
{

	I32 nError = APS_set_axis_param(nAxis, PRA_ACC, nAcc);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_ACC, %d failed with Error %d for Axis %d", nAcc, nError, nAxis);
		return -1;
	}

	nError = APS_set_axis_param(nAxis, PRA_DEC, nDecc);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_DEC, %d failed with Error %d for Axis %d", nDecc, nError, nAxis);
		return -1;
	}
	
	//OutputDebugLogTo(1, TRUE, "PCI-7856 Acceleration is set to %d (pulses/(sec*sec)), Decceleration is set to %d (pulses/(sec*sec)) for Axis %d", nAcc, nDecc, nAxis);

	return 0;
}

int _stdcall AbsoluteMove(int nAxis, int nPos, int nStartVel, int nMaxVel, int nAcc, int nDecc, bool bWait)
{
	PrepareMove(nAxis);


	if(nStartVel > nMaxVel)
		nStartVel = nMaxVel;

	I32 nError = APS_set_axis_param(nAxis, PRA_VS, nStartVel);
	if(nError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_VS, %d failed with Error %d for Axis %d", nStartVel, nError, nAxis);
		return nError;
	}

	nError = APS_absolute_move(nAxis, nPos, nMaxVel);
	//I32 nError = APS_absolute_move2(nAxis, nPos, nStartVel, nMaxVel, nStartVel, 0, 0);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_absolute_move() function with parameters %d, %d failed with Error %d for Axis %d", nPos, nMaxVel, nError, nAxis);
		return nError;
	}

	if(bWait)
		OutputDebugLogTo(1, TRUE, "PCI-7856 Start Absolute Movement for Axis %d to position %d with Start Velocity %d (pulses/sec) and Maximum Velolcity %d (pulses/sec) with waiting for completion", nAxis, nPos, nStartVel, nMaxVel);
	else
		OutputDebugLogTo(1, TRUE, "PCI-7856 Start Absolute Movement for Axis %d to position %d with Start Velocity %d (pulses/sec) and Maximum Velolcity %d (pulses/sec) without waiting for completion", nAxis, nPos, nStartVel, nMaxVel);

	if(bWait) {
		return WaitForMotionDone(nAxis, gWaitTimeOut);
	}

	return 0;
}

int _stdcall RelativeMove(int nAxis, int nPos, int nStartVel, int nMaxVel, bool bWait)
{
	PrepareMove(nAxis);

	I32 nError = APS_set_axis_param(nAxis, PRA_VS, nStartVel);
	if(nError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_VS, %d failed with Error %d for Axis %d", nStartVel, nError, nAxis);
		return nError;
	}

	nError = APS_relative_move(nAxis, nPos, nMaxVel);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_relative_move() function with parameters %d, %d failed with Error %d for Axis %d", nPos, nMaxVel, nError, nAxis);
		return nError;
	}

	if(bWait)
		OutputDebugLogTo(1, TRUE, "PCI-7856 Start Relative Movement for Axis %d to position %d with Maximum Velolcity %d (pulses/sec) with waiting for completion", nAxis, nPos, nMaxVel);
	else
		OutputDebugLogTo(1, TRUE, "PCI-7856 Start Relative Movement for Axis %d to position %d with Maximum Velolcity %d (pulses/sec) without waiting for completion", nAxis, nPos, nMaxVel);

	if(bWait) {
		return WaitForMotionDone(nAxis, gWaitTimeOut);
	}

	return 0;
}

int _stdcall SpeedOverride(int nAxis, int nNewVel)
{
	I32 nError = APS_speed_override(nAxis, nNewVel);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_speed_override() function with parameters %d failed with Error %d for Axis %d", nNewVel, nError, nAxis);
		return -1;
	}

	return 0;
}

int _stdcall SetCurrentPosition(int nAxis, int nPosition)
{
	I32 nError = APS_set_position(nAxis, nPosition);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_position() function with parameters %d failed with Error %d for Axis %d", nPosition, nError, nAxis);
		return -1;
	}
	nError = APS_set_command(nAxis, nPosition);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_command() function with parameters %d failed with Error %d for Axis %d", nPosition, nError, nAxis);
		return -1;
	}

	OutputDebugLogTo(1, TRUE, "PCI-7856 Current Position for Axis %d is set to %d", nAxis, nPosition);

	return 0;
}

int _stdcall HomeMove(int nAxis, int nDistance, int nStartVel, int nMaxVel, int nAcc, bool bWait)
{
	OutputDebugLogTo(1, TRUE, "PCI-7856 Start Homing for Axis %d with Distance %d, Start Velolcity %d and Maxium Velocity %d)", nAxis, nDistance, nStartVel, nMaxVel);
	
	I32 nError;

	nError = APS_set_axis_param(nAxis, PRA_HOME_DIR, (nDistance >= 0) ? 0 : 1);
	if(nError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_HOME_DIR, %d failed with Error %d for Axis %d", (nDistance >= 0) ? 0 : 1, nError, nAxis);
		return nError;
	}

	nError = APS_set_axis_param(nAxis, PRA_CURVE, 0);
	if(nError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_CURVE, 0 failed with Error %d for Axis %d", nError, nAxis);
		return nError;
	}

	nError = APS_set_axis_param(nAxis, PRA_VS, nStartVel);
	if(nError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_VS, %d failed with Error %d for Axis %d", nStartVel, nError, nAxis);
		return nError;
	}

	nError = APS_set_axis_param(nAxis, PRA_HOME_VM, nMaxVel);
	if(nError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_HOME_VM, %d failed with Error %d for Axis %d", nMaxVel, nError, nAxis);
		return nError;
	}

	nError = APS_set_axis_param(nAxis, PRA_HOME_VO, __max(nStartVel/10, 100));
	if(nError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_HOME_VO, %d failed with Error %d for Axis %d", __max(nStartVel/10, 100), nError, nAxis);
		return nError;
	}

	nError = APS_set_axis_param(nAxis, PRA_HOME_OFFSET, abs(nDistance));
	if(nError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_HOME_OFFSET, %d failed with Error %d for Axis %d", abs(nDistance), nError, nAxis);
		return nError;
	}
	
	PrepareMove(nAxis);

	nError = APS_home_move(nAxis);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_home_move() function failed with Error %d for Axis %d", abs(nDistance), nError, nAxis);
		return -1;
	}

	if(bWait) {
		int nError = WaitForMotionDone(nAxis, gWaitTimeOut*5);
		if(nError)
			return nError;

		Sleep(2000);

		nError = SetCurrentPosition(nAxis, 0);
		if(nError)
			return nError;

		OutputDebugLogTo(1, TRUE, "PCI-7856 Homing done for Axis %d", nAxis);
	}

	return 0;
}

int _stdcall ContinuousMove(int nAxis, int nStartVel, int nMaxVel, int nAcc)
{
	I32 nError;

	if(abs(nStartVel) > abs(nMaxVel))
		nStartVel = nMaxVel;

	nError = APS_set_axis_param(nAxis, PRA_VS, nStartVel);
	if(nError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_VS, %d failed with Error %d for Axis %d", nStartVel, nError, nAxis);
		return nError;
	}

	nError = APS_velocity_move(nAxis, nMaxVel);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_velocity_move() function with parameters %d failed with Error %d for Axis %d", nMaxVel, nError, nAxis);
		return -1;
	}

	OutputDebugLogTo(1, TRUE, "PCI-7856 Start Continuos Movement for Axis %d with Maximum Velocity %d", nAxis, nMaxVel);

	return 0;
}

int _stdcall Stop(int nAxis, int nDecc)
{
	//Set decceleration

	I32 nError = APS_stop_move(nAxis);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_stop_move() function failed with Error %d for Axis %d", nError, nAxis);
		return -1;
	}
	
	OutputDebugLogTo(1, TRUE, "PCI-7856 Movement stopped for Axis %d", nAxis);

	return 0;
}

int _stdcall EmergencyStop(int nAxis)
{
	I32 nError = APS_emg_stop(nAxis);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_emg_stop() function failed with Error %d for Axis %d", nError, nAxis);
		return -1;
	}
	
	OutputDebugLogTo(1, TRUE, "PCI-7856 Emergency movement stopped for Axis %d", nAxis);

	return 0;
}

int _stdcall GetCurrentPosition(int nAxis, int* nPosition)
{
	I32 nPos;
	I32 nError = APS_get_position(nAxis, &nPos);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_get_position() function failed with Error %d for Axis %d", nError, nAxis);
		return -1;
	}

	*nPosition = nPos;

	return 0;
}

int _stdcall OutPortRead(int cardNo, int nPortNo, UINT* pData)
{
	I32* p32Data = (I32*) pData;
	I32 nError = APS_field_bus_d_get_output(cardNo, 0, nPortNo, p32Data);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_field_bus_d_get_output() function with parameters 0, %d failed with Error %d for Card %d", nPortNo, nError, cardNo);
		return -1;
	}

	return 0;
}

int _stdcall OutPortWrite(int cardNo, int nPortNo, UINT data)
{
	I32 nError = APS_field_bus_d_set_output(cardNo, 1, nPortNo, data);	
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_field_bus_d_set_output() function with parameters 0, %d, %d failed with Error %d for Card %d", nPortNo, data, nError, cardNo);
		return -1;
	}

	return 0;
}

int _stdcall InPortRead(int cardNo, int nPortNo, UINT* pData)
{
	I32* p32Data = (I32*) pData;
	I32 nError = APS_field_bus_d_get_input(cardNo, 1, nPortNo, p32Data);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_field_bus_d_get_input() function with parameters 0, %d failed with Error %d for Card %d", nPortNo, nError, cardNo);
		return -1;
	}

	return 0;
}

int _stdcall OutPortReadBit(int cardNo, int nPortNo, int Ch_No, UINT *DO_Value)
{
	I32* p32Data = (I32*) DO_Value;
	I32 nError = APS_get_field_bus_d_channel_output(cardNo, 0, nPortNo, Ch_No, p32Data);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_get_field_bus_d_channel_output() function with parameters 0, %d, %d failed with Error %d for Card %d", nPortNo, Ch_No, nError, cardNo);
		return -1;
	}

	return 0;
}

int _stdcall OutPortWriteBit(int cardNo, int nPortNo, int Ch_No, UINT DO_Value)
{
	I32 nError = APS_set_field_bus_d_channel_output(cardNo, 1, nPortNo, Ch_No, DO_Value);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_d_channel_output() function with parameters 0, %d, %d, %u failed with Error %d for Card %d", nPortNo, Ch_No, DO_Value, nError, cardNo);
		return -1;
	}

	return 0;
}

int _stdcall InPortReadBit(int cardNo, int nPortNo, int Ch_No, UINT *DI_Value)
{
	I32* p32Data = (I32*) DI_Value;
	I32 nError = APS_get_field_bus_d_channel_input(cardNo, 1, nPortNo, Ch_No, p32Data);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_get_field_bus_d_channel_input() function with parameters 0, %d, %d failed with Error %d for Card %d", nPortNo, Ch_No, nError, cardNo);
		return -1;
	}

	return 0;
}

int _stdcall SetDIInterrupt(int cardNo, int nPortNo, int intMask)
{
	I32 nEventNo = APS_set_field_bus_int_factor_di(cardNo, 0, nPortNo, intMask);
	//APS_set_field_bus_int_factor_di return Values:
	// - positive value: I32 Interrupt event number.
	// - negative value: I32 error code. Refer to error code table.
	//   - negative value: I32 error code: Refer to error code table.
	if(nEventNo < 0) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_int_factor_di() function with parameters 0, %d, %d failed with Error %d for Card %d", nPortNo, intMask, nEventNo, cardNo);
		return -1;
	}

	OutputDebugLogTo(1, TRUE, "PCI-7856 Set Interrupt  Mask to 0x%x for Card %d", intMask, cardNo);

	return nEventNo;
}

int _stdcall WaitForActiveDIInterrupt(int InterruptNo, int timeout)
{
	I32 nError = APS_wait_single_int(InterruptNo, timeout); 
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_wait_single_int() function failed with Error %d for Interrupt %d", nError, InterruptNo);
		return -1;
	}

	return 0;
}

int _stdcall ClearActiveDIInterrupt(int cardNo, int nPortNo, int InterruptNo)
{
	OutputDebugLogTo(1, TRUE, "PCI-7856 Start Clearing DI Interrupt");

	I32 nError = APS_set_int(InterruptNo);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_int() function failed with Error %d for Interrupt %d", nError, InterruptNo);
		return -1;
	}

	nError = APS_set_field_bus_int_factor_di(cardNo, 0, nPortNo, 0);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_int_factor_di() function with parameters 0, %d failed with Error %d for Card %d", nPortNo, nError, cardNo);
		return -1;
	}

	nError = APS_wait_single_int(InterruptNo, 0); 
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_wait_single_int() function failed with Error %d for Interrupt %d", nError, InterruptNo);
		return -1;
	}

	nError = APS_reset_int(InterruptNo);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_reset_int() function failed with Error %d for Interrupt %d", nError, InterruptNo);
		return -1;
	}

	return 0;
}

int _stdcall InitializeFifoArray(int cardNo, int ModuleTrigNo, int TrigSourceNo, int ModuleCMPNo, int nCMPSource, int nPulseWidth, int nInversePulse)
{
	if(nPulseWidth < 0) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 Incorrect pulse width input parameter %d", nPulseWidth);
		return -1;
	}

	if(ModuleTrigNo < 0 || ModuleTrigNo > 3) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 Incorrect trigger module input parameter %d", ModuleTrigNo);
		return -1;
	}

	if(ModuleCMPNo < 0 || ModuleCMPNo > 3) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 Incorrect CMP module input parameter %d", ModuleCMPNo);
		return -1;
	}

	OutputDebugLogTo(1, TRUE, "PCI-7856 Start Initializing trigger points array for Module %d", ModuleTrigNo);

	I32 nError;

	//Bus_No is 1 for MotionNet
	nError = APS_set_field_bus_trigger_param(cardNo, 1, ModuleCMPNo, TG_CMP0_SRC + nCMPSource, 1); // 0: Command counter 1: Position counter
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_trigger_param() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}

	OutputDebugLogTo(1, TRUE, "PCI-7856 Set Compare source to module %d, channel %x for position counter", ModuleCMPNo, TG_CMP0_SRC + nCMPSource, 1);

	//0: Disable
	//Other: Enable.
	//1: data = cmp counter (regardless of counting direction)
	//2: data = cmp counter (while counting up)
	//3: data = cmp counter (while counting down)
	//4: data > cmp counter
	//5: data < cmp counter
	nError = APS_set_field_bus_trigger_param(cardNo, 1, ModuleCMPNo, TG_CMP0_EN + nCMPSource, 1);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_trigger_param() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}

	OutputDebugLogTo(1, TRUE, "PCI-7856 Enable Compare source on module %d, channel %x for (data = cmp counter) method", ModuleCMPNo, TG_CMP0_EN + nCMPSource);
	
	nError = APS_set_field_bus_trigger_param(cardNo, 1, ModuleCMPNo, TG_CMP0_TYPE + nCMPSource, 0); // 0: Table, 1: Linear
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_trigger_param() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}

	OutputDebugLogTo(1, TRUE, "PCI-7856 Set compare module %d, channel %x to table mode", ModuleCMPNo, TG_CMP0_TYPE + nCMPSource, 0);

	nError = APS_set_field_bus_trigger_param(cardNo, 1, ModuleTrigNo, TG_TRG0_PWD + TrigSourceNo, nPulseWidth); //Pulse Width = ( N+ 5) * 10 ns Value: 0x05 ~ 0x7fffffff The value smaller than 0x05 is treated as 0x05
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_trigger_param() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}

	OutputDebugLogTo(1, TRUE, "PCI-7856 Set trigger pulse width for module %d, channel %x to %d (us)", ModuleTrigNo, TG_TRG0_PWD + TrigSourceNo, nPulseWidth);

	//Bit 0: Pulse logic inverse. Not Inverse (0) / Inverse (1)
	//Bit 1~2: pulse (0) / toggle (1) / ByPass (2) / Disable (3)
	//Bit 3~31: Reserved (set 0)
	nError = APS_set_field_bus_trigger_param(cardNo, 1, ModuleTrigNo, TG_TRG0_CFG + TrigSourceNo, nInversePulse);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_trigger_param() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}

	OutputDebugLogTo(1, TRUE, "PCI-7856 Set trigger pulse on module %d, channel %x to %d", ModuleTrigNo, TG_TRG0_CFG + TrigSourceNo, nInversePulse);

	//  0: None
	//  1: LCMP0 (Default)
	//  2: LCMP1
	//  4: FCMP0
	//  8: FCMP1
	// 16: TMR
	nError = APS_set_field_bus_trigger_param(cardNo, 1, ModuleTrigNo, TG_TRG0_SRC + TrigSourceNo, (1 << nCMPSource));
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_trigger_param() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}

	OutputDebugLogTo(1, TRUE, "PCI-7856 Set trigger output source on module %d, channel %x to %d", ModuleTrigNo, TG_TRG0_SRC + TrigSourceNo, (1 << nCMPSource));

	return 0;
}

int _stdcall SetFifoArray(int cardNo, int ModuleNo, int nCMPSource, long * FifoData, long FifoSize)
{
	if(ModuleNo < 0 || ModuleNo > 3) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 Incorrect module input parameter %d", ModuleNo);
		return -1;
	}
	
	I32 nError = APS_set_field_bus_trigger_table(cardNo, 1, ModuleNo, nCMPSource, FifoData, FifoSize);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_trigger_table() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}

	return 0;
}

int _stdcall ManualTrigger(int cardNo, int ModuleNo, int TrigSourceNo)
{
	if(ModuleNo < 0 || ModuleNo > 3) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 Incorrect module input parameter %d", ModuleNo);
		return -1;
	}

	I32 nError = APS_set_field_bus_trigger_manual(cardNo, 1, ModuleNo, TrigSourceNo);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_trigger_manual() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}

	return 0;
}

int _stdcall GetTriggerCount(int cardNo, int ModuleNo, int TrigSourceNo, int nCMPSource, int * nTrigCount, int * nTrigRemainCount, int * nEncCount)
{
	if(ModuleNo < 0 || ModuleNo > 3) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 Incorrect module input parameter %d", ModuleNo);
		return -1;
	}

	I32 TrigCount;
	I32 TrigRemainCount;
	I32 EncCount;

	I32 nError = APS_get_field_bus_trigger_count(cardNo, 1, ModuleNo, TrigSourceNo, &TrigCount);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_get_field_bus_trigger_count() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}

	nError = APS_get_field_bus_table_cmp_remain_count(cardNo, 1, ModuleNo, nCMPSource, &TrigRemainCount);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_get_field_bus_table_cmp_remain_count() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}

	nError = APS_get_field_bus_encoder(cardNo, 1, ModuleNo, nCMPSource, &EncCount);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_get_field_bus_encoder() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}

	*nTrigCount = TrigCount;
	*nTrigRemainCount = TrigRemainCount;
	*nEncCount = EncCount;

	return 0;
}

int _stdcall ResetTriggerCount(int cardNo, int ModuleNo, int TrigSourceNo)
{
	if(ModuleNo < 0 || ModuleNo > 3) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 Incorrect module input parameter %d", ModuleNo);
		return -1;
	}

	I32 nError = APS_reset_field_bus_trigger_count(cardNo, 1, ModuleNo, TrigSourceNo);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_reset_field_bus_trigger_count() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}

	return 0;
}

//-- Latching

int _stdcall EnableLatch(int nAxis, int nEventID)
{
	if(nEventID < 0 || nEventID > 15) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 Incorrect event ID input parameter %d", nEventID);
		return -1;
	}

	// 11 - COMP4, General comparator is ON

	// 0: LTC pin, 1: ORG pin, 2: GCMP ON
	I32 nError = APS_set_axis_param(nAxis, PRA_LTC_SRC, 0);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_LTC_SRC, 0 failed with Error %d for Axis %d", nError, nAxis);
		return -1;
	}

	// 0: DI, 1: LTC, 2: SD
	nError = APS_set_axis_param(nAxis, PRA_GPDI_SEL, 1);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_GPDI_SEL, 1 failed with Error %d for Axis %d", nError, nAxis);
		return -1;
	}

	// 0: Command counter, 1: Position counter
	nError = APS_set_axis_param(nAxis, PRA_LTC_DEST, 1);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_LTC_DEST, 1 failed with Error %d for Axis %d", nError, nAxis);
		return -1;
	}

	// 0: Disable, Other: Enable
	// 1:data = cmp counter (regardless of counting direction), 2: data=cmp counter (while counting up), 3: data=cmp counter (while counting down), 4: data>cmp counter, 5: data<cmp counter
	nError = APS_set_axis_param(nAxis, PRA_GCMP_EN, 1);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_GCMP_EN, 1 failed with Error %d for Axis %d", nError, nAxis);
		return -1;
	}
	
	//// Pulse (28-bit signed)
	//nError = APS_set_axis_param(nAxis, PRA_GCMP_POS, 1000);
	//if(nError != ERR_NoError) {
	//	OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_GCMP_POS, 1000 failed with Error %d for Axis %d", nError, nAxis);
	//	return -1;
	//}
	
	// 0: Command counter, 1: Position counter
	nError = APS_set_axis_param(nAxis, PRA_GCMP_SRC, 1);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_GCMP_SRC, 1 failed with Error %d for Axis %d", nError, nAxis);
		return -1;
	}

	I32 nInterruptNo = APS_set_field_bus_int_factor_motion(nAxis, 11, 1);
	if(nInterruptNo < ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_field_bus_int_factor_motion() function with parameters 11, 1 failed with Error %d for Axis %d", nInterruptNo, nAxis);
		return -1;
	}

	gLatchEvent[nEventID] = nInterruptNo;

	//I32 nError = APS_reset_int(gLatchEvent);
	//if(nError != ERR_NoError) {
	//	OutputDebugLogTo(0, TRUE, "M_7856 APS_reset_int FAILED (Error=%d) **********", nError);
	//	return -1;
	//}


	OutputDebugLogTo(1, TRUE, "PCI-7856 Latch is enabled for Axis %d", nAxis);

	return 0;
}

int _stdcall GetLatchValue(int nAxis, long * pnLatchData, long nInitialLatchData, int nEventID)
{
	//CPerformanceTimer timer;

	if(nEventID < 0 || nEventID > 15) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 Incorrect event ID input parameter %d", nEventID);
		return -1;
	}

	I32 nError;
	//nError = APS_reset_int(gLatchEvent[nEventID]);
	//if(nError != ERR_NoError) {
	//	OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_reset_int() function with parameters 11, 1 failed with Error %d for Event %d", nError, nEventID);
	//	return -1;
	//}
	//
	//// Pulse (28-bit signed)
	//nError = APS_set_axis_param(nAxis, PRA_GCMP_POS, nInitialLatchData);
	//if(nError != ERR_NoError) {
	//	OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_GCMP_POS, %d failed with Error %d for Axis %d", nInitialLatchData, nError, nAxis);
	//	return -1;
	//}
	//
	//nError = APS_wait_single_int(gLatchEvent[nEventID], -1);
	//if(nError != ERR_NoError) {
	//	OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_wait_single_int() function failed with Error %d for Event %d", nError, nEventID);
	//	return -1;
	//}
	//
	//nError = APS_get_axis_param(nAxis, PRA_LTC_DATA, pnLatchData);
	//if(nError != ERR_NoError) {
	//	OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_set_axis_param() function with parameters PRA_LTC_DATA failed with Error %d for Axis %d", nError, nAxis);
	//	return -1;
	//}

	nError = APS_get_latch_data2(nAxis, 1, pnLatchData);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "M_7856 APS_get_latch_data2 FAILED (Error=%d) **********", nError);
		return -1;
	}

	//OutputDebugLogTo(9, TRUE, "APS_get_latch_data2 time %f", timer.GetElapsedMSec());

	return 0;
}

int _stdcall ManualLatch(int nAxis)
{
	I32 nError = APS_manual_latch2(nAxis);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_manual_latch2() function failed with Error %d for Axis %d", nError, nAxis);
		return -1;
	}

	return 0;
}

int _stdcall ResetAlarm(int cardNo, int nAxis)
{
	I32 ModuleNo = (nAxis - gStartAxis) / 4;
	I32 DO_Value;
	I32 nError;

	nError = SetServoOn(nAxis, 0);
	if(nError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 SetServoOn() function with parameters 0 failed with Error %d for Axis %d", nError, nAxis);
		return nError;	
	}

	DO_Value = 0x0;

	nError = APS_field_bus_d_get_output( cardNo, 1, ModuleNo, &DO_Value );
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_field_bus_d_get_output() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}

	int motorId = (nAxis - gStartAxis) % 4;

	DO_Value = DO_Value & (0xf - (1 << motorId));

	nError = APS_field_bus_d_set_output(cardNo, 1, ModuleNo, DO_Value);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_field_bus_d_set_output() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}

	Sleep(200);

	nError = APS_field_bus_d_get_output( cardNo, 1, ModuleNo, &DO_Value );
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_field_bus_d_get_output() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}
	
	DO_Value = DO_Value | ((1 << motorId));

	nError = APS_field_bus_d_set_output(cardNo, 1, ModuleNo, DO_Value);
	if(nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 APS_field_bus_d_set_output() function failed with Error %d for Card %d", nError, cardNo);
		return -1;
	}

	Sleep(200);

	nError = SetServoOn(nAxis, 1);
	if(nError) {
		OutputDebugLogTo(0, TRUE, "----- ERROR : PCI-7856 SetServoOn() function with parameters 1 failed with Error %d for Axis %d", nError, nAxis);
		return nError;	
	}

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
	for (int i = 0; i<2; i++) {
		Axis_ID_Array[i] = pAxisIDArray[i];
		Position_Array[i] = pPositionArray[i];
	}

	I32 nError = APS_absolute_linear_move(Dimension, Axis_ID_Array, Position_Array, Max_Linear_Speed);
	if (nError != ERR_NoError) {
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
	for (int i = 0; i<2; i++) {
		Axis_ID_Array[i] = pAxisIDArray[i];
		Distance_Array[i] = pDistanceArray[i];
	}

	I32 nError = APS_relative_linear_move(Dimension, Axis_ID_Array, Distance_Array, Max_Linear_Speed);
	if (nError != ERR_NoError) {
		OutputDebugLogTo(0, TRUE, "----- M_7856 RelativeLinearMove FAILED (Error=%d) **********", nError);
		return -1;
	}
}
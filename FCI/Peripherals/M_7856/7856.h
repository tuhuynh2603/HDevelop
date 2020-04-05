/// 7856.cpp: 7856 Motion Control interface

#pragma once

#undef _WIN32_WINNT

#include <APS168.h>
#include "APS_Define.h"
#include "ErrorCodeDef.h"

extern "C" int _stdcall InitMotionDLL();

extern "C" int _stdcall FreeMotionDLL();

extern "C" int _stdcall LoadConfigFromFile(char* strParmFile);

extern "C" int _stdcall RegisterCard(int cardNo, int nStartAxis);

extern "C" int _stdcall ReleaseCard(int cardNo);

extern "C" int _stdcall EnableInterrupt(int cardNo, int nEnable);

extern "C" int _stdcall RegisterIOCard(int cardNo);

extern "C" int _stdcall ReleaseIOCard(int cardNo);

extern "C" int _stdcall InitializeAxis(int nAxis);

extern "C" int _stdcall FreeAxis(int nAxis);

extern "C" void _stdcall SetWaitTimeOut(int nWaitTimeOut);

extern "C" int _stdcall GetIOStatus(int nAxis, long & ioStatus);

extern "C" int _stdcall GetMotorStatus(int nAxis, long & nMotorStatus);

extern "C" int _stdcall SetHomeMode(int nAxis, int homeMode);

extern "C" int _stdcall SetSoftLimits(int nAxis, int nNegLimit, int nPosLimit);

extern "C" int _stdcall ResetSoftLimits(int nAxis);

extern "C" int _stdcall SetAcceleration(int nAxis, int nAcc, int nDecc);

extern "C" int _stdcall SetServoOn(int nAxis, int servoOn);

extern "C" int _stdcall GetAlarmStatus(int nAxis, BOOL* bAlarm, BOOL* bEl);

extern "C" int _stdcall HomeMove(int nAxis, int nDistance, int nStartVel, int nMaxVel, int nAcc, bool bWait);

extern "C" int _stdcall AbsoluteMove(int nAxis, int nPos, int nStartVel, int nMaxVel, int nAcc, int nDecc, bool bWait);

extern "C" int _stdcall RelativeMove(int nAxis, int nPos, int nStartVel, int nMaxVel, bool bWait);

extern "C" int _stdcall ContinuousMove(int nAxis, int nStartVel, int nMaxVel, int nAcc);

extern "C" int _stdcall Stop(int nAxis, int nDecc);

extern "C" int _stdcall SpeedOverride(int nAxis, int nNewVel);

extern "C" int _stdcall EmergencyStop(int nAxis);

extern "C" int _stdcall SetCurrentPosition(int nAxis, int nPosition);

extern "C" int _stdcall GetCurrentPosition(int nAxis, int* nPosition);

extern "C" int _stdcall SetAlarm();

extern "C" int _stdcall ResetAlarm(int cardNo, int nAxis);

extern "C" int _stdcall WaitForMotionDone(int nAxis, int nTimeOut);

extern "C" int _stdcall WaitForAnyMotionError(int nAxisNo, int * pnAxisID, int nTimeOut);

extern "C" int _stdcall OutPortRead(int cardNo, int nPortNo, UINT* pData);

extern "C" int _stdcall OutPortWrite(int cardNo, int nPortNo, UINT data);

extern "C" int _stdcall InPortRead(int cardNo, int nPortNo, UINT* pData);

extern "C" int _stdcall OutPortReadBit(int cardNo, int nPortNo, int Ch_No, UINT *DO_Value);

extern "C" int _stdcall OutPortWriteBit(int cardNo, int nPortNo, int Ch_No, UINT DO_Value);

extern "C" int _stdcall InPortReadBit(int cardNo, int nPortNo, int Ch_No, UINT *DI_Value);

extern "C" int _stdcall SetDIInterrupt(int cardNo, int nPortNo, int intMask);

extern "C" int _stdcall WaitForActiveDIInterrupt(int InterruptNo, int timeout);

extern "C" int _stdcall ClearActiveDIInterrupt(int cardNo, int nPortNo, int InterruptNo);

extern "C" int _stdcall InitializeFifoArray(int cardNo, int ModuleTrigNo, int TrigSourceNo, int ModuleCMPNo, int nCMPSource, int nPulseWidth, int nInversePulse = 0);

extern "C" int _stdcall SetFifoArray(int cardNo, int ModuleNo, int nCMPSource, long * FifoData, long FifoSize);

extern "C" int _stdcall ManualTrigger(int cardNo, int ModuleNo, int TrigSourceNo);

extern "C" int _stdcall GetTriggerCount(int cardNo, int ModuleNo, int TrigSourceNo, int nCMPSource, int * nTrigCount, int * nTrigRemainCount, int * nEncCount);

extern "C" int _stdcall ResetTriggerCount(int cardNo, int ModuleNo, int TrigSourceNo);

//-- Latching

extern "C" int _stdcall EnableLatch(int nAxis, int nEventID);

extern "C" int _stdcall GetLatchValue(int nAxis, long * pnLatchData, long nInitialLatchData, int nEventID);

extern "C" int _stdcall ManualLatch(int nAxis);
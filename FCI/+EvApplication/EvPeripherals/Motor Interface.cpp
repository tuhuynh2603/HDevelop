#include "stdafx.h"
#include "Motor_Interface.h"

#include "DebugLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <vector>

MotorInterface::MotorInterface()
{
	_dll = NULL;

	_lpInitMotionDLL = NULL;
	_lpFreeMotionDLL = NULL;
	_lpLoadConfigFromFile = NULL;

	_lpRegisterMotorCard = NULL;
	_lpReleaseMotorCard = NULL;
	_lpEnableInterrupt = NULL;

	_lpRegisterIoCard = NULL;
	_lpReleaseIoCard = NULL;

	_lpInitializeAxis = NULL;
	_lpFreeAxis = NULL;
	
	_lpSetWaitTimeOut = NULL;

	_lpGetIOStatus = NULL;
	_lpGetMotorStatus = NULL;
	_lpSetHomeMode = NULL;
	_lpSetSoftLimits = NULL;
	_lpResetSoftLimits = NULL;
	_lpSetAcceleration = NULL;
	_lpSetServoOn = NULL;

	_lpGetAlarmStatus = NULL;

	_lpHomeMove = NULL;
	_lpAbsoluteMove = NULL;
	_lpRelativeMove = NULL;
	_lpContinuousMove = NULL;
	_lpAbsoluteLinearMove = NULL;
	_lpRelativeLinearMove = NULL;
	_lpStopAxis = NULL;
	_lpSpeedOverride = NULL;
	_lpEmergencyStopAxis = NULL;

	_lpGetCurrentPosition = NULL;
	_lpSetCurrentPosition = NULL;
	
	_lSetAlarm = NULL;
	_lResetAlarm = NULL;

	_lpWaitForMotionDone = NULL;
	_lpWaitForAnyMotionError = NULL;
	
	_lpOutPortRead = NULL;
	_lpOutPortWrite = NULL;
	_lpInPortRead = NULL;
	_lpOutPortReadBit = NULL;
	_lpOutPortWriteBit = NULL;
	_lpInPortReadBit = NULL;

	_lpSetDIInterrupt = NULL;
	_lpWaitForActiveDIInterrupt = NULL;
	_lpClearActiveDIInterrupt = NULL;

	_lpInitializeFifoArray = NULL;
	_lpSetFifoArray = NULL;

	_lpManualTrigger = NULL;
	_lpGetTriggerCount = NULL;
	_lpResetTriggerCount = NULL;

	_lpEnableLatch = NULL;
	_lpGetLatchValue = NULL;
	_lpManualLatch = NULL;

	//InitializeCriticalSection(&_motorSynchronization);
	//InitializeCriticalSection(&_ioSynchronization);
}

MotorInterface::~MotorInterface()
{
	//DeleteCriticalSection(&_motorSynchronization);
	//DeleteCriticalSection(&_ioSynchronization);
}

int MotorInterface::InitMotionDLL()
{
	if(_lpInitMotionDLL)
		return _lpInitMotionDLL();

	return 0;
}

int MotorInterface::FreeMotionDLL()
{
	if(_lpFreeMotionDLL)
		return _lpFreeMotionDLL();

	return 0;
}

int MotorInterface::LoadMotorDll(CString strMotionCard)
{
	if(strMotionCard.CompareNoCase("none") != 0) {
		_dll = AfxLoadLibrary(strMotionCard);

		if(_dll == NULL) {
			CString str;
			str.Format("Error Loading Motion DLL (%s)", strMotionCard);
			AfxMessageBox(str);
			return -1;
		}

		OutputDebugLogTo(0, TRUE, "Registering Motion Card %s ...", strMotionCard);

		_lpInitMotionDLL = (INIT_MOTION_DLL) GetProcAddress (_dll,"InitMotionDLL");
		if(!_lpInitMotionDLL)
			OutputDebugLogTo(0, TRUE, "Failed to register IO Card InitMotionDLL() Function");

		_lpFreeMotionDLL = (FREE_MOTION_DLL) GetProcAddress (_dll,"FreeMotionDLL");
		if(!_lpFreeMotionDLL)
			OutputDebugLogTo(0, TRUE, "Failed to register IO Card FreeMotionDLL() Function");

		_lpLoadConfigFromFile = (LOAD_MOTOR_CONFIG) GetProcAddress (_dll, "LoadConfigFromFile");
		if(!_lpLoadConfigFromFile)
			OutputDebugLogTo(0, TRUE, "Failed to register LoadConfigFromFile() Function");


		_lpRegisterMotorCard = (REGISTER_MOTOR_CARD) GetProcAddress (_dll, "RegisterCard");
		if(!_lpRegisterMotorCard)
			OutputDebugLogTo(0, TRUE, "Failed to register RegisterCard() Function");		

		_lpReleaseMotorCard = (RELEASE_MOTOR_CARD) GetProcAddress (_dll, "ReleaseCard");
		if(!_lpReleaseMotorCard)
			OutputDebugLogTo(0, TRUE, "Failed to register ReleaseCard() Function");

		_lpEnableInterrupt = (ENABLE_INTERRUPT) GetProcAddress (_dll, "EnableInterrupt");
		if(!_lpEnableInterrupt)
			OutputDebugLogTo(0, TRUE, "Failed to register EnableInterrupt() Function");


		_lpRegisterIoCard = (REGISTER_IO_CARD) GetProcAddress (_dll, "RegisterIOCard");
		if(!_lpRegisterIoCard)
			OutputDebugLogTo(0, TRUE, "Failed to register RegisterIOCard() Function");

		_lpReleaseIoCard = (RELEASE_IO_CARD) GetProcAddress (_dll, "ReleaseIOCard");
		if(!_lpReleaseIoCard)
			OutputDebugLogTo(0, TRUE, "Failed to register ReleaseIOCard() Function");


		_lpInitializeAxis = (INITIALIZE_AXIS) GetProcAddress (_dll, "InitializeAxis");
		if(!_lpInitializeAxis)
			OutputDebugLogTo(0, TRUE, "Failed to register InitializeAxis() Function");

		_lpFreeAxis = (FREE_AXIS) GetProcAddress (_dll, "FreeAxis");
		if(!_lpFreeAxis)
			OutputDebugLogTo(0, TRUE, "Failed to register FreeAxis() Function");


		_lpSetWaitTimeOut = (SET_WAIT_TIMEOUT) GetProcAddress (_dll, "SetWaitTimeOut");
		if(!_lpSetWaitTimeOut)
			OutputDebugLogTo(0, TRUE, "Failed to register SetWaitTimeOut() Function");


		_lpGetIOStatus = (GET_IO_STATUS) GetProcAddress (_dll, "GetIOStatus");
		if(!_lpGetIOStatus)
			OutputDebugLogTo(0, TRUE, "Failed to register GetIOStatus() Function");

		_lpGetMotorStatus = (GET_MOTOR_STATUS) GetProcAddress (_dll, "GetMotorStatus");
		if(!_lpGetMotorStatus)
			OutputDebugLogTo(0, TRUE, "Failed to register GetMotorStatus() Function");

		_lpSetHomeMode = (SET_HOME_MODE) GetProcAddress (_dll, "SetHomeMode");
		if(!_lpSetHomeMode)
			OutputDebugLogTo(0, TRUE, "Failed to register SetHomeMode() Function");

		_lpSetSoftLimits = (SET_SOFT_LIMITS) GetProcAddress (_dll, "SetSoftLimits");
		if(!_lpSetSoftLimits)
			OutputDebugLogTo(0, TRUE, "Failed to register SetSoftLimits() Function");

		_lpResetSoftLimits = (RESET_SOFT_LIMITS) GetProcAddress (_dll, "ResetSoftLimits");
		if(!_lpResetSoftLimits)
			OutputDebugLogTo(0, TRUE, "Failed to register ResetSoftLimits() Function");

		_lpSetAcceleration = (SET_ACCELERATION) GetProcAddress (_dll, "SetAcceleration");
		if(!_lpSetAcceleration)
			OutputDebugLogTo(0, TRUE, "Failed to register SetAcceleration() Function");

		_lpSetServoOn = (SET_SERVO_ON) GetProcAddress (_dll,"SetServoOn");
		if(!_lpSetServoOn)
			OutputDebugLogTo(0, TRUE, "Failed to register ServoOn() Function");

		_lpGetAlarmStatus = (GET_ALARM_STATUS) GetProcAddress (_dll, "GetAlarmStatus");
		if(!_lpGetAlarmStatus)
			OutputDebugLogTo(0, TRUE, "Failed to register GetAlarmStatus() Function");


		_lpHomeMove = (HOME_MOVE) GetProcAddress (_dll, "HomeMove");
		if(!_lpHomeMove)
			OutputDebugLogTo(0, TRUE, "Failed to register HomeMove() Function");

		_lpAbsoluteMove = (ABSOLUTE_MOVE) GetProcAddress (_dll, "AbsoluteMove");
		if(!_lpAbsoluteMove)
			OutputDebugLogTo(0, TRUE, "Failed to register AbsoluteMove() Function");

		_lpRelativeMove = (RELATIVE_MOVE) GetProcAddress (_dll, "RelativeMove");
		if(!_lpRelativeMove)
			OutputDebugLogTo(0, TRUE, "Failed to register RelativeMove() Function");

		_lpContinuousMove = (CONTINUOUS_MOVE) GetProcAddress (_dll, "ContinuousMove");
		if(!_lpContinuousMove)
			OutputDebugLogTo(0, TRUE, "Failed to register ContinuousMove() Function");

		_lpAbsoluteLinearMove = (ABSOLUTE_LINEAR_MOVE)GetProcAddress(_dll, "AbsoluteLinearMove");
		if (!_lpAbsoluteLinearMove)
			OutputDebugLog("Failed to register AbsoluteLinearMove() Function");

		_lpRelativeLinearMove = (RELATIVE_LINEAR_MOVE)GetProcAddress(_dll, "RelativeLinearMove");
		if (!_lpRelativeLinearMove)
			OutputDebugLog("Failed to register RelativeLinearMove() Function");

		_lpStopAxis = (STOP_AXIS) GetProcAddress (_dll, "Stop");
		if(!_lpStopAxis)
			OutputDebugLogTo(0, TRUE, "Failed to register Stop() Function");

		_lpSpeedOverride = (SPEED_OVERRIDE) GetProcAddress (_dll, "SpeedOverride");
		if(!_lpSpeedOverride)
			OutputDebugLogTo(0, TRUE, "Failed to register SpeedOverride() Function");

		_lpEmergencyStopAxis = (EMERGENCY_STOP) GetProcAddress (_dll, "EmergencyStop");
		if(!_lpEmergencyStopAxis)
			OutputDebugLogTo(0, TRUE, "Failed to register EmergencyStop() Function");

		
		_lpGetCurrentPosition = (GET_CURRENT_POSITION) GetProcAddress (_dll, "GetCurrentPosition");
		if(!_lpGetCurrentPosition)
			OutputDebugLogTo(0, TRUE, "Failed to register GetCurrentPosition() Function");

		_lpSetCurrentPosition = (SET_CURRENT_POSITION) GetProcAddress (_dll, "SetCurrentPosition");
		if(!_lpSetCurrentPosition)
			OutputDebugLogTo(0, TRUE, "Failed to register SetCurrentPosition() Function");


		_lSetAlarm = (SET_ALARM) GetProcAddress (_dll, "SetAlarm");
		if (!_lSetAlarm)
			OutputDebugLog("Failed to register SetAlarm() Function");

		_lResetAlarm = (RESET_ALARM) GetProcAddress (_dll, "ResetAlarm");
		if (!_lResetAlarm)
			OutputDebugLog("Failed to register ResetAlarm() Function");
		

		_lpWaitForMotionDone = (WAIT_FOR_MOTION_DONE) GetProcAddress (_dll, "WaitForMotionDone");
		if(!_lpWaitForMotionDone)
			OutputDebugLogTo(0, TRUE, "Failed to register WaitForMotionDone() Function");

		_lpWaitForAnyMotionError = (WAIT_FOR_ANY_MOTION_ERROR) GetProcAddress (_dll, "WaitForAnyMotionError");
		if(!_lpWaitForAnyMotionError)
			OutputDebugLogTo(0, TRUE, "Failed to register WaitForAnyMotionError() Function");



		_lpOutPortRead = (IO_OUTPORT_READ) GetProcAddress (_dll,"OutPortRead");
		if (!_lpOutPortRead)
			OutputDebugLog("Failed to register OutPortRead() Function");

		_lpOutPortWrite = (IO_OUTPORT_WRITE) GetProcAddress (_dll,"OutPortWrite");
		if (!_lpOutPortWrite)
			OutputDebugLog("Failed to register OutPortWrite() Function");
		
		_lpInPortRead = (IO_INPORT_READ) GetProcAddress (_dll,"InPortRead");
		if (!_lpInPortRead)
			OutputDebugLog("Failed to register InPortRead() Function");

		_lpOutPortReadBit = (IO_OUTPORT_READ_BIT) GetProcAddress (_dll,"OutPortReadBit");
		if (!_lpOutPortReadBit)
			OutputDebugLog("Failed to register OutPortReadBit() Function");

		_lpOutPortWriteBit = (IO_OUTPORT_WRITE_BIT) GetProcAddress (_dll,"OutPortWriteBit");
		if (!_lpOutPortWriteBit)
			OutputDebugLog("Failed to register OutPortWriteBit() Function");

		_lpInPortReadBit = (IO_INPORT_READ_BIT) GetProcAddress (_dll,"InPortReadBit");
		if (!_lpInPortReadBit)
			OutputDebugLog("Failed to register InPortReadBit() Function");


		_lpSetDIInterrupt = (SET_DI_INTERRUPT_PCI) GetProcAddress (_dll,"SetDIInterrupt");
		if (!_lpSetDIInterrupt)
			OutputDebugLog("Failed to register SetDIInterrupt() Function");

		_lpWaitForActiveDIInterrupt = (WAIT_FOR_ACTIVE_DI_INTERRUPT) GetProcAddress (_dll,"WaitForActiveDIInterrupt");
		if (!_lpWaitForActiveDIInterrupt)
			OutputDebugLog("Failed to register WaitForActiveDIInterrupt() Function");

		_lpClearActiveDIInterrupt = (CLEAR_ACTIVE_DI_INTERRUPT_PCI) GetProcAddress (_dll, "ClearActiveDIInterrupt");
		if (!_lpClearActiveDIInterrupt)
			OutputDebugLog("Failed to register ClearActiveDIInterrupt() Function");


		_lpInitializeFifoArray = (INITIALIZE_FIFO_ARRAY) GetProcAddress (_dll, "InitializeFifoArray");
		if (!_lpInitializeFifoArray)
			OutputDebugLog("Failed to register InitializeFifoArray() Function");

		_lpSetFifoArray = (SET_FIFO_ARRAY) GetProcAddress (_dll, "SetFifoArray");
		if (!_lpSetFifoArray)
			OutputDebugLog("Failed to register SetFifoArray() Function");


		_lpManualTrigger = (MANUAL_TRIGGER) GetProcAddress (_dll, "ManualTrigger");
		if (!_lpManualTrigger)
			OutputDebugLog("Failed to register ManualTrigger() Function");

		_lpGetTriggerCount = (GET_TRIGGER_COUNT) GetProcAddress (_dll, "GetTriggerCount");
		if (!_lpGetTriggerCount)
			OutputDebugLog("Failed to register GetTriggerCount() Function");

		_lpResetTriggerCount = (RESET_TRIGGER_COUNT) GetProcAddress (_dll, "ResetTriggerCount");
		if (!_lpResetTriggerCount)
			OutputDebugLog("Failed to register ResetTriggerCount() Function");


		_lpEnableLatch = (ENABLE_LATCH) GetProcAddress (_dll, "EnableLatch");
		if (!_lpEnableLatch)
			OutputDebugLog("Failed to register EnableLatch() Function");

		_lpGetLatchValue = (GET_LATCH_VALUE) GetProcAddress (_dll, "GetLatchValue");
		if (!_lpGetLatchValue)
			OutputDebugLog("Failed to register GetLatchValue() Function");

		_lpManualLatch = (MANUAL_LATCH) GetProcAddress (_dll, "ManualLatch");
		if (!_lpManualLatch)
			OutputDebugLog("Failed to register ManualLatch() Function");		

		InitMotionDLL();
	}
	else {
		OutputDebugLogTo(0, TRUE, "No Motion Card");
	}

	return 0;
}

int MotorInterface::RegisterCard(int nCardNo, int nStartAxis)
{
	if(_lpRegisterMotorCard) {
		int nError = _lpRegisterMotorCard(nCardNo, nStartAxis);
		if(nError) {
			AfxMessageBox("Failed to Register Motor Card");
			return nError;
		}
		else
			OutputDebugLogTo(0, TRUE, "Registered Motor Card");
	}
	else
		OutputDebugLogTo(0, TRUE, "No RegisterCard() Function");

	return 0;
}

int MotorInterface::InitializeCard(char* strParmFile)
{
	if(_lpLoadConfigFromFile) {
		int nError = _lpLoadConfigFromFile(strParmFile);
		if(nError) {
			AfxMessageBox("Failed to load motion config from file");
			return nError;
		}
		else
			OutputDebugLogTo(0, TRUE, "Loaded motion config from file");
	}
	else
		OutputDebugLogTo(0, TRUE, "No LoadConfigFromFile() Function");

	return 0;
}

int MotorInterface::ReleaseCard(int nCardNo)
{
	if(_lpReleaseMotorCard) {
		int nError = _lpReleaseMotorCard(nCardNo);
		if(nError) {
			AfxMessageBox("Failed to Release Motor Card");
			return nError;
		}
		else
			OutputDebugLogTo(0, TRUE, "Released Motor Card");
	}
	else 
		OutputDebugLogTo(0, TRUE, "No ReleaseCard() Function");

	return 0;
}

int MotorInterface::RegisterIOCard(int nCardNo)
{
	if(_lpRegisterIoCard) {
		int nError = _lpRegisterIoCard(nCardNo);
		if(nError) {
			AfxMessageBox("Failed to Register Motor I/O Card");
			return nError;
		}
		else
			OutputDebugLogTo(0, TRUE, "Registered Motor I/O Card");
	}
	else
		OutputDebugLogTo(0, TRUE, "No RegisterIOCard() Function");

	return 0;
}

int MotorInterface::ReleaseIOCard(int nCardNo)
{
	if(_lpReleaseIoCard) {
		int nError = _lpReleaseIoCard(nCardNo);
		if(nError) {
			AfxMessageBox("Failed to Release Motor I/O Card");
			return nError;
		}
		else
			OutputDebugLogTo(0, TRUE, "Released Motor I/O Card");
	}
	else 
		OutputDebugLogTo(0, TRUE, "No ReleaseIOCard() Function");

	return 0;
}

int MotorInterface::InitializeAxis(int nAxis, int nHomeMode)
{
	if(_lpInitializeAxis) {
		int nError = _lpInitializeAxis(nAxis);
		if(nError) {
			OutputDebugLogTo(0, TRUE, "Failed to Initialize Motor Axis %d", nAxis);
			return nError;
		}
		else
			OutputDebugLogTo(0, TRUE, "Initialized Motor Axis %d", nAxis);
	}
	else 
		OutputDebugLogTo(0, TRUE, "No InitializeAxis() Function");

	if(_lpSetHomeMode != NULL)
		_lpSetHomeMode(nAxis, nHomeMode);


	return 0;
}

int MotorInterface::FreeAxis(int nAxis)
{
	if(_lpFreeAxis) {
		int nError = _lpFreeAxis(nAxis);
		if(nError) {
			//AfxMessageBox("Failed to Free Motor Axis");
			return nError;
		}
		else
			OutputDebugLogTo(0, TRUE, "Freed Motor Axis %d", nAxis);
	}
	else 
		OutputDebugLogTo(0, TRUE, "No FreeAxis() Function");

	if(_lpSetServoOn != NULL)
		_lpSetServoOn(nAxis, 0);

	return 0;
}

int MotorInterface::AbsoluteMove(int nAxis, int nPos, int nStartVel, int nMaxVel, int nAcc, int nDecc, bool bWait)
{
	if(_lpAbsoluteMove) {
		return _lpAbsoluteMove(nAxis, nPos, nStartVel, nMaxVel, nAcc, nDecc, bWait);
	}

	return 0;
}

int MotorInterface::WaitForMotionDone(int nAxis, int nTimeOut)
{
	if(_lpWaitForMotionDone==NULL) {
		OutputDebugLogTo(1, TRUE, "[Axis%d] No WaitForMotionDone function", nAxis);
		return -1;
	}


	return _lpWaitForMotionDone(nAxis, nTimeOut);
}

int MotorInterface::WaitForAnyMotionError(int nAxisNo, int * pnAxisID, int nTimeOut)
{
	if(_lpWaitForAnyMotionError)
		return _lpWaitForAnyMotionError(nAxisNo, pnAxisID, nTimeOut);

	return 0;
}

int MotorInterface::HomeMove(int nAxis,  int nDistance, int nStartVelocity, int nMaxVelocity, int nAcceleration, int nDecceleration, bool bWait)
{
	if(_lpHomeMove==NULL)
		return -1;

	return _lpHomeMove(nAxis, nDistance, nStartVelocity, nMaxVelocity, nAcceleration, bWait);
}

int MotorInterface::SetServoOn(int nAxis, BOOL bServoOn)
{
	if(_lpSetServoOn != NULL)
		_lpSetServoOn(nAxis, bServoOn ? 1 : 0);

	return 0;
}

int MotorInterface::GetAlarmStatus(int nAxis, BOOL& bAlarm, BOOL& bEl)
{
	if(_lpGetAlarmStatus != NULL)
		_lpGetAlarmStatus(nAxis, &bAlarm, &bEl);

	return 0;
}

int MotorInterface::SetAcceleration(int nAxis, int nAcc, int nDecc)
{
	if(_lpSetAcceleration != NULL)
		_lpSetAcceleration(nAxis, nAcc, nDecc);

	return 0;
}

int MotorInterface::OutPortWrite(int nCardNo, int nPortNo, UINT nData)
{
	if(_lpOutPortWrite)
		return _lpOutPortWrite(nCardNo, nPortNo, nData);

	return 0;
}

int MotorInterface::OutPortRead(int nCardNo, int nPortNo, UINT *pnData)
{
	if(_lpOutPortRead)
		return _lpOutPortRead(nCardNo, nPortNo, pnData);
	
	return 0;
}

int MotorInterface::InPortRead(int nCardNo, int nPortNo, UINT *pnData)
{
	if(_lpInPortRead)
		return _lpInPortRead(nCardNo, nPortNo, pnData);
	
	return 0;
}

int MotorInterface::OutPortWriteBit(int nCardNo, int nPortNo, int Ch_No, UINT DO_Value)
{
	if(_lpOutPortWriteBit)
		return _lpOutPortWriteBit(nCardNo, nPortNo, Ch_No, DO_Value);
	
	return 0;
}

int MotorInterface::OutPortReadBit(int nCardNo, int nPortNo, int Ch_No, UINT *DO_Value)
{
	if (_lpOutPortReadBit)
		return _lpOutPortReadBit(nCardNo, nPortNo, Ch_No, DO_Value);
	
	return 0;
}

int MotorInterface::InPortReadBit(int nCardNo, int nPortNo, int Ch_No, UINT *DI_Value)
{
	if (_lpInPortReadBit)
		return _lpInPortReadBit(nCardNo, nPortNo, Ch_No, DI_Value);
	
	return 0;
}

int MotorInterface::SetDIInterrupt(int nCardNo, int nPortNo, int nIntMask)
{
	if (_lpSetDIInterrupt)
		return _lpSetDIInterrupt(nCardNo, nPortNo, nIntMask);
	
	return 0;
}

int MotorInterface::WaitForActiveDIInterrupt(int InterruptNo, int nTimeout)
{
	if (_lpWaitForActiveDIInterrupt)
		return _lpWaitForActiveDIInterrupt(InterruptNo, nTimeout);
	
	return 0;
}

int MotorInterface::ClearActiveDIInterrupt(int nCardNo, int nPortNo, int nBitPos)
{
	if (_lpClearActiveDIInterrupt)
		return _lpClearActiveDIInterrupt(nCardNo, nPortNo, nBitPos);

	return 0;
}

int MotorInterface::InitializeFifoArray(int nCardNo, int ModuleTrigNo, int TrigSourceNo, int ModuleCMPNo, int nCMPSource, int nPulseWidth, int nInversePulse)
{
	if(_lpInitializeFifoArray)
		return _lpInitializeFifoArray(nCardNo, ModuleTrigNo, TrigSourceNo, ModuleCMPNo, nCMPSource, nPulseWidth, nInversePulse);

	return 0;
}
int MotorInterface::SetFifoArray(int nCardNo, int ModuleNo, int TrigSourceNo, long * FifoData, long FifoSize)
{
	if(_lpSetFifoArray)
		return _lpSetFifoArray(nCardNo, ModuleNo, TrigSourceNo, FifoData, FifoSize);

	return 0;
}
int MotorInterface::ManualTrigger(int nCardNo, int ModuleNo, int TrigSourceNo)
{
	if(_lpManualTrigger)
		return _lpManualTrigger(nCardNo, ModuleNo, TrigSourceNo);

	return 0;
}

int MotorInterface::GetTriggerCount(int nCardNo, int ModuleNo, int TrigSourceNo, int nCMPSource, int * nTrigCount, int * nTrigRemainCount, int * nEncCount)
{
	if(_lpGetTriggerCount)
		return _lpGetTriggerCount(nCardNo,ModuleNo, TrigSourceNo, nCMPSource, nTrigCount, nTrigRemainCount, nEncCount);

	return 0;
}

int MotorInterface::ResetTriggerCount(int nCardNo, int ModuleNo, int TrigSourceNo)
{
	if(_lpResetTriggerCount)
		return _lpResetTriggerCount(nCardNo, ModuleNo, TrigSourceNo);

	return 0;
}

int MotorInterface::EnableLatch(int nAxis, int nEventID)
{
	if(_lpEnableLatch)
		return _lpEnableLatch(nAxis, nEventID);

	return 0;
}

int MotorInterface::GetLatchValue(int nAxis, long * pnLatchData, long nInitialLatchData, int nEventID)
{
	if(_lpGetLatchValue)
		return _lpGetLatchValue(nAxis, pnLatchData, nInitialLatchData, nEventID);

	return 0;
}

int MotorInterface::ManualLatch(int nAxis)
{
	if(_lpManualLatch)
		return _lpManualLatch(nAxis);

	return 0;
}

int MotorInterface::EnableInterrupt(int nCardNo, int nEnable)
{
	if(_lpEnableInterrupt)
		return _lpEnableInterrupt(nCardNo, nEnable);

	return 0;
}

int MotorInterface::GetIOStatus(int nAxis, long & ioStatus)
{
	if(_lpGetIOStatus)
		return _lpGetIOStatus(nAxis, ioStatus);

	return 0;
}

int MotorInterface::GetMotorStatus(int nAxis, long & nMotorStatus)
{
	if(_lpGetMotorStatus)
		return _lpGetMotorStatus(nAxis, nMotorStatus);

	return 0;
}

int MotorInterface::SetCurrentPosition(int nAxis, int nPosition)
{
	if(_lpSetCurrentPosition)
		return _lpSetCurrentPosition(nAxis, nPosition);

	return 0;
}

int MotorInterface::GetCurrentPosition(int nAxis, int * pnPosition)
{
	if(_lpGetCurrentPosition)
		return _lpGetCurrentPosition(nAxis, pnPosition);

	return 0;
}

int MotorInterface::StopAxis(int nAxis, int nDecc)
{
	if(_lpStopAxis)
		return _lpStopAxis(nAxis, nDecc);

	return 0;
}

int MotorInterface::EmergencyStopAxis(int nAxis)
{
	if(_lpEmergencyStopAxis)
		return _lpEmergencyStopAxis(nAxis);

	return 0;
}

int MotorInterface::RelativeMove(int nAxis, int nPos, int nStartVel, int nMaxVel, bool bWait)
{
	if(_lpRelativeMove)
		return _lpRelativeMove(nAxis, nPos, nStartVel, nMaxVel, bWait);

	return 0;
}

int MotorInterface::ContinuousMove(int nAxis, int nStartVel, int nMaxVel, int nAcc)
{
	if(_lpContinuousMove)
		return _lpContinuousMove(nAxis, nStartVel, nMaxVel, nAcc);

	return 0;
}

int MotorInterface::AbsoluteLinearMove(int nDimension, int* pAxisIDArray, int* pPositionArray, int nMaxLinearSpeed)
{
	if (_lpAbsoluteLinearMove != NULL)
		return _lpAbsoluteLinearMove(nDimension, pAxisIDArray, pPositionArray, nMaxLinearSpeed);

	return 0;
}

int MotorInterface::RelativeLinearMove(int nDimension, int* pAxisIDArray, int* pPositionArray, int nMaxLinearSpeed)
{
	if (_lpRelativeLinearMove != NULL)
		return _lpRelativeLinearMove(nDimension, pAxisIDArray, pPositionArray, nMaxLinearSpeed);

	return 0;
}

int MotorInterface::SpeedOverride(int nAxis, int nNewVel)
{
	if(_lpSpeedOverride)
		return _lpSpeedOverride(nAxis, nNewVel);

	return 0;
}

int MotorInterface::SetSoftLimits(int nAxis, int nNegLimit, int nPosLimit)
{
	if(_lpSetSoftLimits)
		return _lpSetSoftLimits(nAxis, nNegLimit, nPosLimit);

	return 0;
}

int MotorInterface::ResetSoftLimits(int nAxis)
{
	if(_lpResetSoftLimits)
		return _lpResetSoftLimits(nAxis);

	return 0;
}

int MotorInterface::ResetAlarm(int cardNo, int nAxis)
{
	if(_lResetAlarm)
		return _lResetAlarm(cardNo, nAxis);

	return 0;
}

int MotorInterface::SetAlarm()
{
	if(_lSetAlarm)
		return _lSetAlarm();

	return 0;
}

bool MotorInterface::isDllOpened() const
{
	return _dll != NULL;
}

void MotorInterface::closeDll()
{
	if(isDllOpened())
		AfxFreeLibrary(_dll);
}

void MotorInterface::setWaitTimeout(int timeOut)
{
	if(_lpSetWaitTimeOut)
		return _lpSetWaitTimeOut(timeOut);
}
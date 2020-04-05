#pragma once

typedef int (CALLBACK* INIT_MOTION_DLL)();

typedef int (CALLBACK* FREE_MOTION_DLL)();

typedef int (CALLBACK* LOAD_MOTOR_CONFIG)(char* strFile);

typedef int (CALLBACK* REGISTER_MOTOR_CARD)(int nCardNo, int nStartAxis);

typedef int (CALLBACK* RELEASE_MOTOR_CARD)(int nCardNo);

typedef int (CALLBACK* ENABLE_INTERRUPT)(int nCardNo, int nEnable);

typedef int (CALLBACK* REGISTER_IO_CARD)(int nCardNo);

typedef int (CALLBACK* RELEASE_IO_CARD)(int nCardNo);

typedef int (CALLBACK* INITIALIZE_AXIS)(int nAxis);

typedef int (CALLBACK* FREE_AXIS)(int nAxis);

typedef void (CALLBACK* SET_WAIT_TIMEOUT)(int nWaitTimeOut);

typedef int (CALLBACK* GET_IO_STATUS)(int nAxis, long & ioStatus);

typedef int (CALLBACK* GET_MOTOR_STATUS)(int nAxis, long & nMotorStatus);

typedef int (CALLBACK* SET_HOME_MODE)(int nAxis, int nHomeMode);

typedef int (CALLBACK* SET_SOFT_LIMITS)(int nAxis, int nNegLimit, int nPosLimit);

typedef int (CALLBACK* RESET_SOFT_LIMITS)(int nAxis);

typedef int (CALLBACK* SET_ACCELERATION)(int nAxis, int nAcc, int nDecc);

typedef int (CALLBACK* SET_SERVO_ON)(int nAxis, int nServoOn);

typedef int (CALLBACK* GET_ALARM_STATUS)(int nAxis, BOOL* bAlarm, BOOL* bEl);

typedef int (CALLBACK* HOME_MOVE)(int nAxis, int nDistance, int nStartVel, int nMaxVel, int nAcc, bool bWait);

typedef int (CALLBACK* ABSOLUTE_MOVE)(int nAxis, int nPos, int nStartVel, int nMaxVel, int nAcc, int nDecc, bool bWait);

typedef int (CALLBACK* RELATIVE_MOVE)(int nAxis, int nPos, int nStartVel, int nMaxVel, bool bWait);

typedef int (CALLBACK* CONTINUOUS_MOVE)(int nAxis, int nStartVel, int nMaxVel, int nAcc);

typedef int(CALLBACK* ABSOLUTE_LINEAR_MOVE)(int nDimension, int* pAxisIDArray, int* pPositionArray, int nMaxLinearSpeed);

typedef int (CALLBACK* RELATIVE_LINEAR_MOVE)(int nDimension, int* pAxisIDArray, int* pPositionArray, int nMaxLinearSpeed);

typedef int (CALLBACK* STOP_AXIS)(int nAxis, int nDecc);

typedef int (CALLBACK* SPEED_OVERRIDE)(int nAxis, int nNewVel);

typedef int (CALLBACK* EMERGENCY_STOP)(int nAxis);

typedef int (CALLBACK* SET_CURRENT_POSITION)(int nAxis, int nPosition);

typedef int (CALLBACK* GET_CURRENT_POSITION)(int nAxis, int* pnPosition);

typedef int (CALLBACK* SET_ALARM)();

typedef int (CALLBACK* RESET_ALARM)(int cardNo, int nAxis);

typedef int (CALLBACK* WAIT_FOR_MOTION_DONE)(int nAxis, int nTimeOut);

typedef int (CALLBACK* WAIT_FOR_ANY_MOTION_ERROR)(int nAxisNo, int * pnAxisID, int nTimeOut);

typedef int (CALLBACK* IO_OUTPORT_READ)(int nCardNo, int nPortNo, UINT* pnData);

typedef int (CALLBACK* IO_OUTPORT_WRITE)(int nCardNo, int nPortNo, UINT nData);

typedef int (CALLBACK* IO_INPORT_READ)(int nCardNo, int nPortNo, UINT* pnData);

typedef int (CALLBACK* IO_OUTPORT_READ_BIT)(int cardNo, int nPortNo, int Ch_No, UINT *DO_Value);

typedef int (CALLBACK* IO_OUTPORT_WRITE_BIT)(int cardNo, int nPortNo, int Ch_No, UINT DO_Value);

typedef int (CALLBACK* IO_INPORT_READ_BIT)(int cardNo, int nPortNo, int Ch_No, UINT *DI_Value);

typedef int (CALLBACK* SET_DI_INTERRUPT_PCI)(int nCardNo, int nPortNo, int nIntMask);

typedef int (CALLBACK* WAIT_FOR_ACTIVE_DI_INTERRUPT)(int InterruptNo, int nTimeout);

typedef int (CALLBACK* CLEAR_ACTIVE_DI_INTERRUPT_PCI)(int nCardNo, int nPortNo, int nBitPos);

typedef int (CALLBACK* INITIALIZE_FIFO_ARRAY)(int nCardNo, int ModuleTrigNo, int TrigSourceNo, int ModuleCMPNo, int nCMPSource, int nPulseWidth, int nInversePulse);

typedef int (CALLBACK* SET_FIFO_ARRAY)(int nCardNo, int ModuleNo, int nCMPSource, long * FifoData, long FifoSize);

typedef int (CALLBACK* MANUAL_TRIGGER)(int nCardNo, int ModuleNo, int TrigSourceNo);

typedef int (CALLBACK* GET_TRIGGER_COUNT)(int nCardNo, int ModuleNo, int TrigSourceNo, int nCMPSource, int * nTrigCount, int * nTrigRemainCount, int * nEncCount);

typedef int (CALLBACK* RESET_TRIGGER_COUNT)(int nCardNo, int ModuleNo, int TrigSourceNo);

typedef int (CALLBACK* ENABLE_LATCH)(int nAxis, int nEventID);

typedef int (CALLBACK* GET_LATCH_VALUE)(int nAxis, long * pnLatchData, long nInitialLatchData, int nEventID);

typedef int (CALLBACK* MANUAL_LATCH)(int nAxis);

class MotorInterface
{
public:
	MotorInterface();
	~MotorInterface();

	int InitMotionDLL();
	int FreeMotionDLL();

	int LoadMotorDll(CString strMotorCard);
	int RegisterCard(int nCardNo, int nStartAxis);
	int InitializeCard(char* strParmFile);
	int ReleaseCard(int nCardNo);
	int RegisterIOCard(int nCardNo);
	int ReleaseIOCard(int nCardNo);
	int InitializeAxis(int nAxis, int nHomeMode);
	int FreeAxis(int nAxis);
	int AbsoluteMove(int nAxis, int nPos, int nStartVel, int nMaxVel, int nAcc = 0, int nDecc = 0, bool bWait = false);
	int WaitForMotionDone(int nAxis, int nTimeOut);
	int WaitForAnyMotionError(int nAxisNo, int * pnAxisID, int nTimeOut);

	int HomeMove(int nAxis, int nDistance, int nStartVelocity, int nMaxVelocity, int nAcceleration, int nDecceleration, bool bWait = false);
	int SetServoOn(int nAxis, BOOL bServoOn);
	int GetAlarmStatus(int nAxis, BOOL& bAlarm, BOOL& bEl);
	int SetAcceleration(int nAxis, int nAcc, int nDecc);

	int OutPortWrite(int nCardNo, int nPortNo, UINT nData);
	int OutPortRead(int nCardNo, int nPortNo, UINT* pnData);
	int InPortRead(int nCardNo, int nPortNo, UINT *pnData);

	int OutPortWriteBit(int nCardNo, int nPortNo, int Ch_No, UINT DO_Value);
	int OutPortReadBit(int nCardNo, int nPortNo, int Ch_No, UINT *DO_Value);
	int InPortReadBit(int nCardNo, int nPortNo, int Ch_No, UINT *DI_Value);

	int SetDIInterrupt(int nCardNo, int nPortNo, int nIntMask);
	int WaitForActiveDIInterrupt(int InterruptNo, int nTimeout);
	int ClearActiveDIInterrupt(int nCardNo, int nPortNo, int nBitPos);

	int InitializeFifoArray(int nCardNo, int ModuleTrigNo, int TrigSourceNo, int ModuleCMPNo, int nCMPSource, int nPulseWidth, int nInversePulse);
	int SetFifoArray(int nCardNo, int ModuleNo, int nCMPSource, long * FifoData, long FifoSize);
	int ManualTrigger(int nCardNo, int ModuleNo, int TrigSourceNo);
	int GetTriggerCount(int nCardNo, int ModuleNo, int TrigSourceNo, int nCMPSource, int * nTrigCount, int * nTrigRemainCount, int * nEncCount);
	int ResetTriggerCount(int nCardNo, int ModuleNo, int TrigSourceNo);

	int EnableLatch(int nAxis, int nEventID = 0);
	int GetLatchValue(int nAxis, long * pnLatchData, long nInitialLatchData, int nEventID = 0);
	int ManualLatch(int nAxis);

	int EnableInterrupt(int nCardNo, int nEnable);

	int GetIOStatus(int nAxis, long & ioStatus);
	int GetMotorStatus(int nAxis, long & nMotorStatus);

	int SetCurrentPosition(int nAxis, int nPosition);
	int GetCurrentPosition(int nAxis, int* pnPosition);

	int StopAxis(int nAxis, int nDecc);
	int EmergencyStopAxis(int nAxis);

	int RelativeMove(int nAxis, int nPos, int nStartVel, int nMaxVel, bool bWait = false);

	int ContinuousMove(int nAxis, int nStartVel, int nMaxVel, int nAcc);
	int AbsoluteLinearMove(int nDimension, int* pAxisIDArray, int* pPositionArray, int nMaxLinearSpeed);
	int RelativeLinearMove(int nDimension, int* pAxisIDArray, int* pPositionArray, int nMaxLinearSpeed);

	int SpeedOverride(int nAxis, int nNewVel);

	int SetSoftLimits(int nAxis, int nNegLimit, int nPosLimit);
	int ResetSoftLimits(int nAxis);

	int ResetAlarm(int cardNo, int nAxis);

	int SetAlarm();

	bool isDllOpened() const;

	void closeDll();

	void setWaitTimeout(int timeOut);

private:
	HINSTANCE _dll;

	INIT_MOTION_DLL			_lpInitMotionDLL;
	FREE_MOTION_DLL			_lpFreeMotionDLL;
	LOAD_MOTOR_CONFIG		_lpLoadConfigFromFile;

	REGISTER_MOTOR_CARD		_lpRegisterMotorCard;
	RELEASE_MOTOR_CARD		_lpReleaseMotorCard;
	ENABLE_INTERRUPT		_lpEnableInterrupt;

	REGISTER_IO_CARD		_lpRegisterIoCard;
	RELEASE_IO_CARD			_lpReleaseIoCard;

	INITIALIZE_AXIS			_lpInitializeAxis;
	FREE_AXIS				_lpFreeAxis;
	
	SET_WAIT_TIMEOUT		_lpSetWaitTimeOut;

	GET_IO_STATUS			_lpGetIOStatus;
	GET_MOTOR_STATUS		_lpGetMotorStatus;
	SET_HOME_MODE			_lpSetHomeMode;
	SET_SOFT_LIMITS			_lpSetSoftLimits;
	RESET_SOFT_LIMITS		_lpResetSoftLimits;
	SET_ACCELERATION		_lpSetAcceleration;
	SET_SERVO_ON			_lpSetServoOn;

	GET_ALARM_STATUS		_lpGetAlarmStatus;

	HOME_MOVE				_lpHomeMove;
	ABSOLUTE_MOVE			_lpAbsoluteMove;
	RELATIVE_MOVE			_lpRelativeMove;
	CONTINUOUS_MOVE			_lpContinuousMove;
	ABSOLUTE_LINEAR_MOVE	_lpAbsoluteLinearMove;
	RELATIVE_LINEAR_MOVE	_lpRelativeLinearMove;
	STOP_AXIS				_lpStopAxis;
	SPEED_OVERRIDE			_lpSpeedOverride;
	EMERGENCY_STOP			_lpEmergencyStopAxis;

	GET_CURRENT_POSITION	_lpGetCurrentPosition;
	SET_CURRENT_POSITION	_lpSetCurrentPosition;
	
	SET_ALARM				_lSetAlarm;
	RESET_ALARM				_lResetAlarm;

	WAIT_FOR_MOTION_DONE	_lpWaitForMotionDone;
	WAIT_FOR_ANY_MOTION_ERROR _lpWaitForAnyMotionError;
	
	IO_OUTPORT_READ			_lpOutPortRead;
	IO_OUTPORT_WRITE		_lpOutPortWrite;
	IO_INPORT_READ			_lpInPortRead;
	IO_OUTPORT_READ_BIT		_lpOutPortReadBit;
	IO_OUTPORT_WRITE_BIT	_lpOutPortWriteBit;
	IO_INPORT_READ_BIT		_lpInPortReadBit;

	SET_DI_INTERRUPT_PCI				_lpSetDIInterrupt;
	WAIT_FOR_ACTIVE_DI_INTERRUPT	_lpWaitForActiveDIInterrupt;
	CLEAR_ACTIVE_DI_INTERRUPT_PCI		_lpClearActiveDIInterrupt;

	INITIALIZE_FIFO_ARRAY	_lpInitializeFifoArray;
	SET_FIFO_ARRAY			_lpSetFifoArray;

	MANUAL_TRIGGER			_lpManualTrigger;
	GET_TRIGGER_COUNT		_lpGetTriggerCount;
	RESET_TRIGGER_COUNT		_lpResetTriggerCount;

	ENABLE_LATCH			_lpEnableLatch;
	GET_LATCH_VALUE			_lpGetLatchValue;
	MANUAL_LATCH			_lpManualLatch;

	//CRITICAL_SECTION _motorSynchronization;
	//CRITICAL_SECTION _ioSynchronization;
};

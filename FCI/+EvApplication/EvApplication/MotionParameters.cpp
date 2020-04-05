#include "StdAfx.h"
#include "Application.h"
#include "MotionParameters.h"
#include "DebugLog.h"
#include "RegistryAccess.h"
#include "MsgCode.h"
#include "IniFile.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include "multivectorOperations.h"

//#include "operatorOverloading.h"
#include <numeric>
#include <map>

MotionParameters::MotionParameters() : motorResource( theApp.m_MotorResource )
{

	InitializeCriticalSection(&CriticalSectionCommands);
	pApp = (CApplication*) AfxGetApp();
	//Motor Names
	motorCount = maximumMotorCount;
	
	motorEngine[X_SERVO_MOTOR].setName("X Servo Motor");
	motorEngine[Y_SERVO_MOTOR].setName("Y Servo Motor");
	motorEngine[Z_SERVO_TOP_MOTOR].setName("Z Servo Top Motor");
	motorEngine[Z_SERVO_BOTTOM_MOTOR].setName("Z Servo Bottom Motor");

	// Motor variables
	motorEngine[X_SERVO_MOTOR].setAxisId(1500); // X Servo Motor
	motorEngine[Y_SERVO_MOTOR].setAxisId(1501); // Y Servo Motor
	motorEngine[Z_SERVO_TOP_MOTOR].setAxisId(1502); // Z Top Servo Motor
	motorEngine[Z_SERVO_BOTTOM_MOTOR].setAxisId(1503); // Z Bottom Servo Motor

	motorEngine[X_SERVO_MOTOR].setHomingMode(2);
	motorEngine[Y_SERVO_MOTOR].setHomingMode(2);
	motorEngine[Z_SERVO_TOP_MOTOR].setHomingMode(2);
	motorEngine[Z_SERVO_BOTTOM_MOTOR].setHomingMode(2);

	motorEngine[X_SERVO_MOTOR].setResolution(1638.4);
	motorEngine[Y_SERVO_MOTOR].setResolution(1638.4);
	motorEngine[Z_SERVO_TOP_MOTOR].setResolution(1365.33);
	motorEngine[Z_SERVO_BOTTOM_MOTOR].setResolution(1365.33);

	motorEngine[X_SERVO_MOTOR].setHomePosition(0);
	motorEngine[Y_SERVO_MOTOR].setHomePosition(0);
	motorEngine[Z_SERVO_TOP_MOTOR].setHomePosition(0);
	motorEngine[Z_SERVO_BOTTOM_MOTOR].setHomePosition(0);

	for(int i = 0; i < motorCount; i++) {
		_motorScanVelocity[i]._setPositionName("Scanning");
		_motorScanVelocityTHR[i]._setPositionName("Scanning_THR");
		_motorMoveVelocity[i]._setPositionName("Moving");

		_motorMoveVelocity[i].startSpeed = 1000;
		_motorScanVelocity[i].startSpeed = 1000;
		_motorScanVelocityTHR[i].startSpeed = 1000;
		_motorMoveVelocity[i].maximumSpeed = 10000;
		_motorScanVelocity[i].maximumSpeed = 10000;
		_motorScanVelocityTHR[i].maximumSpeed = 10000;
	}

//	nTriggerInversePulse = 1;
//	nTriggerPulseWidth = 75000;

	for(int i = 0; i < motorCount; i++)
		motorStartScanPos[i] = 0;

	scanningReady = false;
							   // Scanning mode 1 (wafer mapping)
	m_nNoOfBlock[0] = 1;
	m_nNoOfBlock[1] = 1;
	m_dBlockPitch[0] = 10;
	m_dBlockPitch[1] = 10;
	m_nDeviceNoPerBlock[0] = 3;
	m_nDeviceNoPerBlock[1] = 3;
	m_dDevicePitch[0] = 5;
	m_dDevicePitch[1] = 5;
	nDieColumn = 1;
	nDieRow = 1;

	// Device Size
	dDeviceSizeInXMM = 2;
	dDeviceSizeInYMM = 2;
	bHorizontalDevice = FALSE;
	
//	InitializeCriticalSection(&CriticalSectionData);

	nTotalImageFOV = 0;
	m_nTotalGrabNo = 0;
	m_nTotalNoOfFov = 1;
	m_nTotalNoDevice = 1;

	// Z focusing levels for bond station
	// Triggering and movement for 2-3D station
}

MotionParameters::MotionParameters(const MotionParameters & motionParameters) : motorResource(motionParameters.motorResource)
{
	InitializeCriticalSection(&CriticalSectionCommands);
	pApp = (CApplication*) AfxGetApp();

	motorEngine = motionParameters.motorEngine;

	_motorMoveVelocity = motionParameters._motorMoveVelocity;
	_motorScanVelocity = motionParameters._motorScanVelocity;
	_motorScanVelocityTHR = motionParameters._motorScanVelocityTHR;
	//Motor Names

//	nTriggerInversePulse = motionParameters.nTriggerInversePulse;
//	nTriggerPulseWidth = motionParameters.nTriggerPulseWidth;

	motorCount = motionParameters.motorCount;
	motorStartScanPos = motionParameters.motorStartScanPos;

	scanningReady = motionParameters.scanningReady;
	// Scanning mode 1 (wafer mapping)
	m_nNoOfBlock[0] = motionParameters.m_nNoOfBlock[0];
	m_nNoOfBlock[1] = motionParameters.m_nNoOfBlock[1];
	m_dBlockPitch[0] = motionParameters.m_dBlockPitch[0];
	m_dBlockPitch[1] = motionParameters.m_dBlockPitch[1];
	m_nDeviceNoPerBlock[0] = motionParameters.m_nDeviceNoPerBlock[0];
	m_nDeviceNoPerBlock[1] = motionParameters.m_nDeviceNoPerBlock[1];
	m_dDevicePitch[0] = motionParameters.m_dDevicePitch[0];
	m_dDevicePitch[1] = motionParameters.m_dDevicePitch[1];
	nDieColumn = motionParameters.nDieColumn;
	nDieRow = motionParameters.nDieRow;

	// Device Size
	dDeviceSizeInXMM = motionParameters.dDeviceSizeInXMM;
	dDeviceSizeInYMM = motionParameters.dDeviceSizeInYMM;
	bHorizontalDevice = motionParameters.bHorizontalDevice;


	//	InitializeCriticalSection(&CriticalSectionData);

	nTotalImageFOV = motionParameters.nTotalImageFOV;
	m_dInitialOffset = motionParameters.m_dInitialOffset;
	vDieFovNo = motionParameters.vDieFovNo;
	m_nTotalGrabNo = motionParameters.m_nTotalGrabNo;
	m_nTotalNoOfFov = motionParameters.m_nTotalGrabNo;
	bDeviceEnabled = motionParameters.bDeviceEnabled;
	bDieEnabled = motionParameters.bDieEnabled;
}

const MotionParameters & MotionParameters::operator=(const MotionParameters & motionParameters)
{
	if(this == &motionParameters)
		return *this;

	motorEngine = motionParameters.motorEngine;

	// Motor variables
	motorCount = motionParameters.motorCount;

	_motorMoveVelocity = motionParameters._motorMoveVelocity;
	_motorScanVelocity = motionParameters._motorScanVelocity;
	_motorScanVelocityTHR = motionParameters._motorScanVelocityTHR;

//	nTriggerInversePulse = motionParameters.nTriggerInversePulse;
//	nTriggerPulseWidth = motionParameters.nTriggerPulseWidth;

	motorStartScanPos = motionParameters.motorStartScanPos;
	motorDisplacement = motionParameters.motorDisplacement;

	// Scanning mode 1 (substrate mapping)
	m_nNoOfBlock[0] = motionParameters.m_nNoOfBlock[0];
	m_nNoOfBlock[1] = motionParameters.m_nNoOfBlock[1];
	m_dBlockPitch[0] = motionParameters.m_dBlockPitch[0];
	m_dBlockPitch[1] = motionParameters.m_dBlockPitch[1];
	m_nDeviceNoPerBlock[0] = motionParameters.m_nDeviceNoPerBlock[0];
	m_nDeviceNoPerBlock[1] = motionParameters.m_nDeviceNoPerBlock[1];
	m_dDevicePitch[0] = motionParameters.m_dDevicePitch[0];
	m_dDevicePitch[1] = motionParameters.m_dDevicePitch[1];
	nDieColumn = motionParameters.nDieColumn;
	nDieRow = motionParameters.nDieRow;

	// Merging for 2-3D station
//	InitializeCriticalSection(&CriticalSectionData);

// Device Size
	dDeviceSizeInXMM = motionParameters.dDeviceSizeInXMM;
	dDeviceSizeInYMM = motionParameters.dDeviceSizeInYMM;
	bHorizontalDevice = motionParameters.bHorizontalDevice;


	nTotalImageFOV = motionParameters.nTotalImageFOV;
	m_dInitialOffset = motionParameters.m_dInitialOffset;
	vDieFovNo = motionParameters.vDieFovNo;
	m_nTotalGrabNo = motionParameters.m_nTotalGrabNo;
	m_nTotalNoOfFov = motionParameters.m_nTotalGrabNo;
	bDeviceEnabled = motionParameters.bDeviceEnabled;
	bDieEnabled = motionParameters.bDieEnabled;

	return (*this);
}

MotionParameters::~MotionParameters()
{
	DeleteCriticalSection(&CriticalSectionCommands);
//	DeleteCriticalSection(&CriticalSectionData);
}

void MotionParameters::SetMotionMovementParameters(const MotionParameters & motionParameters)
{
	_motorMoveVelocity = motionParameters._motorMoveVelocity;
	_motorScanVelocity = motionParameters._motorScanVelocity;
	_motorScanVelocityTHR = motionParameters._motorScanVelocityTHR;

	for(int i = 0; i < motorCount; i++) {

		motorEngine[i].setForwardHoming(motionParameters.motorEngine[i].isForwardHoming());
		motorEngine[i].setForwardMovement(motionParameters.motorEngine[i].isForwardMovement());
		motorEngine[i].setHomeOriginOffset(motionParameters.motorEngine[i].getHomeOriginOffset());
		motorEngine[i].setSettlingTime(motionParameters.motorEngine[i].getSettlingTime());
		motorEngine[i].setHomingSpeed(motionParameters.motorEngine[i].getHomingSpeed());
	}
}

void MotionParameters::InitMotionCard()
{
	EnterCriticalSection(&CriticalSectionCommands);

	motorResource.EnableInterrupt(theApp.m_nMotorCardNo, 1);

	for(int i = 0; i < motorCount; i++)
		motorEngine[i].initialize();

	LeaveCriticalSection(&CriticalSectionCommands);
}

void MotionParameters::FreeMotionCard()
{
	EnterCriticalSection(&CriticalSectionCommands);

	for(int i = 0; i < motorCount; i++) {
		motorEngine[i].free();
	}

	LeaveCriticalSection(&CriticalSectionCommands);
}

// Single motor axis interraction
int MotionParameters::CheckSafety(int nAxisNo, bool bSafety, bool bHomeByPass)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	int nCurpos;

	CString strTemp;

	
	if(bSafety) {
		// if any, in future
	}

	return 0;
}

int MotionParameters::CheckSoftwareLimits(int nAxisNo, int nPos)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	return motorEngine[nAxisNo].validatePosition(nPos);
}

bool MotionParameters::IsSoftwareLimits(int nAxisNo, int nPos)
{
	if (nAxisNo < 0 || nAxisNo >= motorCount)
		return true;

	return motorEngine[nAxisNo].isSoftwareLimits(nPos);
}

int MotionParameters::SetServoOn(int nAxisNo)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	EnterCriticalSection(&CriticalSectionCommands);

	int nError = motorEngine[nAxisNo].servoOn();

	LeaveCriticalSection(&CriticalSectionCommands);

	return nError;
}


int MotionParameters::SetServoOff(int nAxisNo)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	EnterCriticalSection(&CriticalSectionCommands);

	int nError = motorEngine[nAxisNo].servoOff();

	LeaveCriticalSection(&CriticalSectionCommands);

	return nError;
}

int MotionParameters::StopAxis(int nAxisNo)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	EnterCriticalSection(&CriticalSectionCommands);

	int nError = motorEngine[nAxisNo].stop();

	LeaveCriticalSection(&CriticalSectionCommands);

	return nError;
}

int MotionParameters::EmergencyStop(int nAxisNo)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	EnterCriticalSection(&CriticalSectionCommands);

	int nError = motorEngine[nAxisNo].emergencyStop();

	LeaveCriticalSection(&CriticalSectionCommands);

	return nError;
}

int MotionParameters::ResetAlarm(int nAxisNo)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	int nError;

	EnterCriticalSection(&CriticalSectionCommands);

	nError = motorEngine[nAxisNo].resetAlarm();

	LeaveCriticalSection(&CriticalSectionCommands);

	return nError;
}

int MotionParameters::HomeMove(int nAxisNo, bool bSafety, bool bHomeByPass)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount || CheckSafety(nAxisNo, bSafety, bHomeByPass) || CheckSoftwareLimits(nAxisNo, 0))
		return -1;

	EnterCriticalSection(&CriticalSectionCommands);

	int nError = motorEngine[nAxisNo].startHoming();

	LeaveCriticalSection(&CriticalSectionCommands);

	return nError;
}

int MotionParameters::HomeDone(const int nAxisNo)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	Sleep(motorEngine[nAxisNo].getSettlingTime() * 10);

	int nError;

	EnterCriticalSection(&CriticalSectionCommands);

	nError = motorEngine[nAxisNo].completeHoming();
	if(nError) {
		LeaveCriticalSection(&CriticalSectionCommands);
		return nError;
	}

	LeaveCriticalSection(&CriticalSectionCommands);

	return 0;
}

int MotionParameters::AbsoluteMove(int nAxisNo, int nPos, Speed speed, bool bSafety, bool bHomeByPass)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount || speed.startSpeed > speed.maximumSpeed || CheckSafety(nAxisNo, bSafety, bHomeByPass) || CheckSoftwareLimits(nAxisNo, nPos))
		return -1;

	int nError;

	EnterCriticalSection(&CriticalSectionCommands);

	nError = motorEngine[nAxisNo].absoluteMove(nPos, speed.startSpeed, speed.maximumSpeed);

	LeaveCriticalSection(&CriticalSectionCommands);

	return nError;
}

int MotionParameters::AbsoluteLinearMove(int nDimension, int* pAxisIDs, int* pPositionArray, Speed speed, bool bSafety, bool bHomeByPass)
{
	if (nDimension < 0)
		return -1;

	int pAxisIDArray[2];

	pAxisIDArray[0] = motorEngine[pAxisIDs[0]]._getAxisId();
	pAxisIDArray[1] = motorEngine[pAxisIDs[1]]._getAxisId();

	int nError;
	EnterCriticalSection(&CriticalSectionCommands);

	return theApp.m_MotorResource.AbsoluteLinearMove(nDimension, pAxisIDArray, pPositionArray, speed.maximumSpeed);

	LeaveCriticalSection(&CriticalSectionCommands);

	return 0;
}

int MotionParameters::RelativeLinearMove(int nDimension, int* pAxisIDArray, int* pPositionArray, Speed speed, bool bSafety, bool bHomeByPass)
{
	if (nDimension < 0)
		return -1;

	int nError;
	EnterCriticalSection(&CriticalSectionCommands);

	return theApp.m_MotorResource.RelativeLinearMove(nDimension, pAxisIDArray, pPositionArray, speed.maximumSpeed);

	LeaveCriticalSection(&CriticalSectionCommands);

	return 0;
}

int MotionParameters::RelativeMove(int nAxisNo, int nPos, Speed speed, bool bSafety, bool bHomeByPass)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount || speed.startSpeed > speed.maximumSpeed || CheckSafety(nAxisNo, bSafety, bHomeByPass))
		return -1;

	int nError;

	EnterCriticalSection(&CriticalSectionCommands);

	nError = motorEngine[nAxisNo].relativeMove(nPos, speed.startSpeed, speed.maximumSpeed);

	LeaveCriticalSection(&CriticalSectionCommands);

	return nError;
}

int MotionParameters::ChangeSpeedOnFly(int nAxisNo, int nNewMaxVel)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	int nError;

	EnterCriticalSection(&CriticalSectionCommands);

	nError = motorEngine[nAxisNo].changeSpeed(nNewMaxVel);

	LeaveCriticalSection(&CriticalSectionCommands);

	return nError;
}

int MotionParameters::ContinuousMove(int nAxisNo, BOOL bForward, Speed speed, bool bSafety, bool bHomeByPass)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount || speed.startSpeed > speed.maximumSpeed || CheckSafety(nAxisNo, bSafety, bHomeByPass))
		return -1;

	int nError = StopAxis(nAxisNo);
	if(nError) return nError;

	EnterCriticalSection(&CriticalSectionCommands);

	nError = motorEngine[nAxisNo].continuousMove(abs(speed.startSpeed), abs(speed.maximumSpeed), bForward != FALSE);

	LeaveCriticalSection(&CriticalSectionCommands);

	return nError;
}

int MotionParameters::WaitForStartPositionDone(int nTimeOut)
{
	//First wait for event go to start position is completed
	DWORD dwError = WaitForSingleObject(pApp->m_hInitPositionDone, nTimeOut);
	if (dwError == WAIT_FAILED || dwError == WAIT_TIMEOUT) {
		OutputDebugLogTo(7, TRUE, "-----ERROR: Init Position Done Waiting have been failed.");
		return -1;
	}

	//Wait for all motion done
	int nError = theApp.motionParameters.WaitForMotionDone(X_SERVO_MOTOR, theApp.getMotionTimeOut());
	if (nError) {
		OutputDebugLogTo(7, TRUE, "-----ERROR: WaitForMotionDone For X Servo Motor failed");
		return -1;
	}

	nError = theApp.motionParameters.WaitForMotionDone(Y_SERVO_MOTOR, theApp.getMotionTimeOut());
	if (nError) {
		OutputDebugLogTo(7, TRUE, "-----ERROR: WaitForMotionDone For Y Servo Motor failed");
		return -1;
	}

	nError = theApp.motionParameters.WaitForMotionDone(Z_SERVO_TOP_MOTOR, theApp.getMotionTimeOut());
	if (nError) {
		OutputDebugLogTo(7, TRUE, "-----ERROR: WaitForMotionDone For Z Top Servo Motor failed");
		return -1;
	}

	nError = theApp.motionParameters.WaitForMotionDone(Z_SERVO_BOTTOM_MOTOR, theApp.getMotionTimeOut());
	if (nError) {
		OutputDebugLogTo(7, TRUE, "-----ERROR: WaitForMotionDone For Z Top Servo Motor failed");
		return -1;
	}

	return 0;
}

int MotionParameters::WaitForMotionDone(int nAxisNo, int nTimeOut)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	return motorEngine[nAxisNo].waitMotionCompletion(nTimeOut);
}

int MotionParameters::IsMotionDone(int nAxisNo, BOOL & bDone)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	bDone = motorEngine[nAxisNo].isMotionCompleted();

	return 0;
}

int MotionParameters::getMotorStatus(int axisId, long & status)
{
	if(axisId < 0 || axisId >= motorCount)
		return -1;

	return motorEngine[axisId].getMotorStatus(status);
}


int MotionParameters::GetCurrentMotorPosition(int nAxisNo, int & nPos)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	return motorEngine[nAxisNo].getCurrentPosition(nPos);
}

BOOL MotionParameters::IsMotorActive(int nAxisNo)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return FALSE;

	return motorEngine[nAxisNo].isActive();
}

BOOL MotionParameters::IsMotorHomed(int nAxisNo) const
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return FALSE;

	return motorEngine[nAxisNo].getHomedStatus();
}

BOOL MotionParameters::IsMotorLimit(int nAxisNo)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return TRUE;

	return motorEngine[nAxisNo].isLimit();
}

void MotionParameters::SetMotorHomed(int nAxisNo, BOOL bHomeStatus)
{
	if (nAxisNo < 0 || nAxisNo >= motorCount)
		return;

	motorEngine[nAxisNo].setHomedStatus(bHomeStatus);
}

int MotionParameters::GetMotorEZStatus(int nAxisNo, BOOL & bStatus)
{
	if (nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	//CString strIOStatus[9] = {
	//		"ALR", // Servo Alarm
	//		"PEL", // Positive End Limit
	//		"MEL", // Negative End Limit
	//		"ORG", // Original position sensor (home sensor)
	//		"EMG", // EMG sensor
	//		"EZ",  // EZ passed
	//		"INP", // In Position
	//		"SVON",// Servo-ON
	//		"RDY"  // Ready
	//};

	long currentIoStatus;
	motorEngine[nAxisNo].getIoStatus(currentIoStatus);
	UINT nBit = 1;
	for (int j = 0; j < 6; j++, nBit *= 2) {
	}
	bStatus = (currentIoStatus & nBit) != 0;

	return 0;
}

int MotionParameters::GetMotorAlarmStatus(int nAxisNo, BOOL &bOn)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	int nError = motorEngine[nAxisNo].getAlarmStatus(bOn);
	if(nError)
		return nError;

	return 0;
}

int MotionParameters::GetMotorIOStatus(int nAxisNo, long & nStatus)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	return motorEngine[nAxisNo].getIoStatus(nStatus);
}

int MotionParameters::GetMotorResolution(int nAxisNo, double & dResolution) const
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	dResolution = motorEngine[nAxisNo].getResolution();

	return 0;
}

int MotionParameters::SetMotorResolution(int nAxisNo, double dResolution)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	motorEngine[nAxisNo].setResolution(dResolution);

	return 0;
}

int MotionParameters::GetMotorSettlingTime(int nAxisNo, int & nTime) const
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;
	nTime = motorEngine[nAxisNo].getSettlingTime();

	return 0;
}

int MotionParameters::SetMotorSettlingTime(int nAxisNo, int nTime)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	motorEngine[nAxisNo].setSettlingTime(nTime);

	return 0;
}

Speed MotionParameters::getHomingSpeed(int axisId) const
{
	if(axisId < 0 || axisId >= motorCount)
		return Speed(0, 0);
	else
		return motorEngine[axisId].getHomingSpeed();
}

Speed MotionParameters::getScanningSpeed(int axisId) const
{
	if(axisId < 0 || axisId >= motorCount)
		return Speed(0, 0);
	else
		return _motorScanVelocity[axisId];
}

Speed MotionParameters::getScanningSpeedTHR(int axisId) const
{
	if(axisId < 0 || axisId >= motorCount)
		return Speed(0, 0);
	else
		return _motorScanVelocityTHR[axisId];
}


Speed MotionParameters::getMovingSpeed(int axisId) const
{
	if(axisId < 0 || axisId >= motorCount)
		return Speed(0, 0);
	else
		return _motorMoveVelocity[axisId];
}

int MotionParameters::setHomingSpeed(int axisId, Speed speed)
{
	if(axisId < 0 || axisId >= motorCount)
		return -1;

	motorEngine[axisId].setHomingSpeed(speed);

	return 0;
}

int MotionParameters::setScanningSpeed(int axisId, Speed speed)
{
	if(axisId < 0 || axisId >= motorCount)
		return -1;

	_motorScanVelocity[axisId] = speed;

	return 0;
}

//int MotionParameters::setScanningSpeedTHR(int axisId, Speed speed)
//{
//	if(axisId < 0 || axisId >= motorCount)
//		return -1;
//
//	_motorScanVelocityTHR[axisId] = speed;
//
//	return 0;
//}

int MotionParameters::setMovingSpeed(int axisId, Speed speed)
{
	if(axisId < 0 || axisId >= motorCount)
		return -1;

	_motorMoveVelocity[axisId] = speed;

	return 0;
}


bool MotionParameters::checkSoftLimitSetStatus()
{
	bool set = true;

	for(int i = 0; i < motorCount; i++) {
		if(!motorEngine[i].isSoftwareLimitSet()) {
			set = false;
			break;
		}
	}

	return set;
}

int MotionParameters::GetMotorHomeOriginOffset(int nAxisNo, int & nOffset) const
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	nOffset = motorEngine[nAxisNo].getHomeOriginOffset();

	return 0;
}

int MotionParameters::SetMotorHomeOriginOffset(int nAxisNo, int nOffset)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	motorEngine[nAxisNo].setHomeOriginOffset(nOffset);

	return 0;
}

int MotionParameters::GetMotorHomeDirection(int nAxisNo, BOOL & bForward) const
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	bForward = motorEngine[nAxisNo].isForwardHoming();

	return 0;
}

int MotionParameters::SetMotorHomeDirection(int nAxisNo, BOOL bForward)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	motorEngine[nAxisNo].setForwardHoming(bForward != FALSE);

	return 0;
}

//BOOL MotionParameters::GetMotorHomeCheckByPass()
//{
//	return bEnableMotorHomeByPass;
//}

//void MotionParameters::SetMotorHomeCheckByPass(BOOL bHomeCheckByPass)
//{
//	bEnableMotorHomeByPass = bHomeCheckByPass;
//}

int MotionParameters::GetMotorMovementDirection(int nAxisNo, BOOL & bForward) const
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	bForward = motorEngine[nAxisNo].isForwardMovement();

	return 0;
}

int MotionParameters::SetMotorMovementDirection(int nAxisNo, BOOL bForward)
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	motorEngine[nAxisNo].setForwardMovement(bForward != FALSE);

	return 0;
}

int MotionParameters::GetMotorMaximumSpeed(int nAxisNo, double & dSpeed) const
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return -1;

	dSpeed = motorEngine[nAxisNo].getSpeedLimit();

	return 0;
}

void MotionParameters::WaitMotorSettlingTime(int nAxisNo) const
{
	if(nAxisNo < 0 || nAxisNo >= motorCount)
		return;

	Sleep(motorEngine[nAxisNo].getSettlingTime());
}

int MotionParameters::GetMotorName(int nMotorID, CString & strName) const
{
	if(nMotorID < 0 || nMotorID >= motorCount)
		return -1;

	strName = motorEngine[nMotorID].getName().c_str();

	return 0;
}

// Multiple motor axis interraction
int MotionParameters::GetTotalMotorCount() const
{
	return motorCount;
}

int MotionParameters::SetServoOnAllAxis()
{
	int nGlobalError = 0;

	for(int i = 0; i<motorCount; i++) {

		int nError = motorEngine[i].servoOn();
		if(nError)
			nGlobalError = nError;

		Sleep(100);
	}

	return nGlobalError;
}

int MotionParameters::SetServoOffAllAxis()
{
	int nGlobalError = 0;

	for(int i = 0; i<motorCount; i++) {

		int nError = motorEngine[i].servoOff();
		if(nError)
			nGlobalError = nError;
	}

	return nGlobalError;
}

int MotionParameters::StopEveryAxis()
{
	int nGlobalError = 0;

	for(int i = 0; i<motorCount; i++) {
		int nError = motorEngine[i].stop();
		if(nError)
			nGlobalError = nError;
	}

	return nGlobalError;
}

int MotionParameters::EmergencyStopEveryAxis()
{
	int nErrorTotal = 0;

	for(int i = 0; i<motorCount; i++) {
		int nError = motorEngine[i].emergencyStop();
		if(nError)
			nErrorTotal = nError;

	}

	return nErrorTotal;
}

int MotionParameters::ResetAlarmAllAxis()
{
	int nErrorGlobal = 0;

	for(int i = 0; i<motorCount; i++) {

		if(i == motorCount - 1)
			Sleep(1000);

		int nError = motorEngine[i].resetAlarm();
		if(nError)
			nErrorGlobal = nError;

		Sleep(100);
	}

	return nErrorGlobal;
}

int MotionParameters::HomeMoveEveryAxis(bool bSafety)
{
	int nErrorGlobal = 0;

	for(int i = 0; i<motorCount; i++) {
		int nError = HomeMove(i, bSafety);
		if(nError)
			nErrorGlobal = nError;
	}

	return nErrorGlobal;
}

int MotionParameters::WaitForAnyMotionError(std::vector < int > axisId, int nTimeOut)
{
	if(axisId.empty())
		return -1;

	std::vector < int > realAxis(axisId.size());

	for(size_t i = 0; i < axisId.size(); ++i) {

		if(axisId[i] < 0 || axisId[i] >= motorCount)
			return -1;

		realAxis[i] = motorEngine[i]._getAxisId();
	}

	return motorResource.WaitForAnyMotionError(axisId.size(), realAxis.data(), nTimeOut);
}

void MotionParameters::WaitForEveryMotionDone(std::vector < int > error, int nTimeOut)
{
	error.resize(motorCount);

	for(int i = 0; i<motorCount; i++)
		error[i] = WaitForMotionDone(i, nTimeOut);
}

void MotionParameters::SetZTopStartScanPos()
{
	scanningReady = false;

	motorEngine[Z_SERVO_TOP_MOTOR].getCurrentPosition(motorStartScanPos[2]);

}

void MotionParameters::SetZBottomStartScanPos()
{
	scanningReady = false;

	motorEngine[Z_SERVO_BOTTOM_MOTOR].getCurrentPosition(motorStartScanPos[3]);

}

int MotionParameters::moveToZTopStartScanPosPos(bool bSafety)
{
	
	int nError = StopAxis(Z_SERVO_TOP_MOTOR);
	if(nError)
		return nError;

	if(!IsMotorHomed(Z_SERVO_TOP_MOTOR)) {
		nError = HomeMove(Z_SERVO_TOP_MOTOR, false);
		if(nError)
			return nError;
	}

	if(!IsMotorHomed(Z_SERVO_TOP_MOTOR)) {
		nError = WaitForMotionDone(Z_SERVO_TOP_MOTOR, theApp.getMotionTimeOut());
		if (nError)
			return nError;

		nError = HomeDone(Z_SERVO_TOP_MOTOR);
		if (nError)
			return nError;
	}

	nError = AbsoluteMove(Z_SERVO_TOP_MOTOR, motorStartScanPos[2], getMovingSpeed(Z_SERVO_TOP_MOTOR), false);
	if(nError)
		return nError;

	return 0;
}

int MotionParameters::moveToZBottomStartScanPosPos(bool bSafety)
{

	int nError = StopAxis(Z_SERVO_BOTTOM_MOTOR);
	if (nError)
		return nError;

	if (!IsMotorHomed(Z_SERVO_BOTTOM_MOTOR)) {
		nError = HomeMove(Z_SERVO_BOTTOM_MOTOR, false);
		if (nError)
			return nError;
	}

	if (!IsMotorHomed(Z_SERVO_BOTTOM_MOTOR)) {
		nError = WaitForMotionDone(Z_SERVO_BOTTOM_MOTOR, theApp.getMotionTimeOut());
		if (nError)
			return nError;

		nError = HomeDone(Z_SERVO_BOTTOM_MOTOR);
		if (nError)
			return nError;
	}

	nError = AbsoluteMove(Z_SERVO_BOTTOM_MOTOR, motorStartScanPos[3], getMovingSpeed(Z_SERVO_BOTTOM_MOTOR), false);
	if (nError)
		return nError;

	return 0;
}

void MotionParameters::SetStartScanPos()
{
	scanningReady = false;

	motorEngine[X_SERVO_MOTOR].getCurrentPosition(motorStartScanPos[0]);
	motorEngine[Y_SERVO_MOTOR].getCurrentPosition(motorStartScanPos[1]);

	OutputDebugLogTo(7, TRUE, "Start Pos, X Position %d, Y Position %d", motorStartScanPos[0], motorStartScanPos[1]);

}

int MotionParameters::MoveToStartScanPos(bool bSafety)
{
	int nError = StopAxis(X_SERVO_MOTOR);
	if(nError)
		return nError;

	nError = StopAxis(Y_SERVO_MOTOR);
	if(nError)
		return nError;

	/*nError = StopAxis(Z_SERVO_MOTOR);
	if(nError)
	return nError;*/

	//if(CheckSafety(X_SERVO_MOTOR, bSafety) || CheckSafety(Y_SERVO_MOTOR, bSafety)/* || CheckSafety(Z_SERVO_MOTOR, bSafety)*/)
	//	return -1;

	if(!IsMotorHomed(X_SERVO_MOTOR)) {
		nError = HomeMove(X_SERVO_MOTOR, false);
		if(nError)
			return nError;
	}

	if(!IsMotorHomed(Y_SERVO_MOTOR)) {
		nError = HomeMove(Y_SERVO_MOTOR, false);
		if(nError)
			return nError;
	}

	if(!IsMotorHomed(X_SERVO_MOTOR)) {

		nError = WaitForMotionDone(X_SERVO_MOTOR, theApp.getMotionTimeOut());
		if(nError)
			return nError;

		nError = HomeDone(X_SERVO_MOTOR);
		if(nError)
			return nError;
	}

	if(!IsMotorHomed(Y_SERVO_MOTOR)) {

		nError = WaitForMotionDone(Y_SERVO_MOTOR, theApp.getMotionTimeOut());
		if(nError)
			return nError;

		nError = HomeDone(Y_SERVO_MOTOR);
		if(nError)
			return nError;
	}

	nError = AbsoluteMove(X_SERVO_MOTOR, motorStartScanPos[0], getMovingSpeed(X_SERVO_MOTOR), false);
	if(nError)
		return nError;

	nError = AbsoluteMove(Y_SERVO_MOTOR, motorStartScanPos[1], getMovingSpeed(Y_SERVO_MOTOR), false);
	if(nError)
		return nError;

	return 0;
}

int MotionParameters::MoveXYToPosition(int nDevice, int nFOV, bool bSaftey)
{
	if (nDevice < 0 || nDevice > m_nNoOfBlock[0] * m_nNoOfBlock[1] || nFOV < 0 || nFOV > nColumnNoPerDie * nRowNoPerDie)
		return -1;

	int nDieRow;
	int nDieCol;

	int nDeviceRow;
	int nDeviceCol;

	int nError;
	BOOL nTempDieMap = TRUE;

	nTempDieMap = FALSE;
	nDieCol = nFOV % nColumnNoPerDie;
	nDieRow = nFOV / nColumnNoPerDie;

	nDeviceCol = nDevice % m_nNoOfBlock[0];
	nDeviceRow = nDevice / m_nNoOfBlock[0];

	BOOL bIsHomed = TRUE;
	if (!pApp->motionParameters.IsMotorHomed(X_SERVO_MOTOR) || !pApp->motionParameters.IsMotorHomed(Y_SERVO_MOTOR) ||
		!pApp->motionParameters.IsMotorHomed(Z_SERVO_TOP_MOTOR) || !pApp->motionParameters.IsMotorHomed(Z_SERVO_BOTTOM_MOTOR)) {
		/*if (AfxMessageBox("Motor is not homed. Do you want to home?", MB_YESNO) == IDNO)
			return 0;*/
		bIsHomed = FALSE;
	}

	CProgressBarDlg *pProgressDlg = NULL;
	if (!bIsHomed) {
		pProgressDlg = new CProgressBarDlg();
		if (!pProgressDlg->Create(IDD_PROGRESS_DLG, AfxGetMainWnd())) {
			delete pProgressDlg;
			pProgressDlg = NULL;
		}
		else {
			if (pProgressDlg != NULL) {
				pProgressDlg->ShowWindow(TRUE);
				pProgressDlg->CenterWindow();
				pProgressDlg->UpdateWindow();
				pProgressDlg->SetStatus("Machine Resetting....", 0);
			}
		}
	}

	if (pProgressDlg != NULL) {
		pProgressDlg->ShowWindow(TRUE);
		pProgressDlg->UpdateWindow();
		pProgressDlg->SetStatus("Machine Resetting....", 20);
	}

	if (!IsMotorHomed(Z_SERVO_TOP_MOTOR)) {

		nError = HomeMove(Z_SERVO_TOP_MOTOR);
		if (nError) {
			if (pProgressDlg != NULL) {
				pProgressDlg->ShowWindow(FALSE);
				pProgressDlg->DestroyWindow();
				delete pProgressDlg;
				pProgressDlg = NULL;
			}
			return -1;
		}
		
		nError = WaitForMotionDone(Z_SERVO_TOP_MOTOR, theApp.getMotionTimeOut() * 10);
		if (nError) {
			if (pProgressDlg != NULL) {
				pProgressDlg->ShowWindow(FALSE);
				pProgressDlg->DestroyWindow();
				delete pProgressDlg;
				pProgressDlg = NULL;
			}
			return -1;
		}

		nError = HomeDone(Z_SERVO_TOP_MOTOR);
		if (nError) {
			if (pProgressDlg != NULL) {
				pProgressDlg->ShowWindow(FALSE);
				pProgressDlg->DestroyWindow();
				delete pProgressDlg;
				pProgressDlg = NULL;
			}
			return -1;
		}
	}

	if (pProgressDlg != NULL) {
		pProgressDlg->ShowWindow(TRUE);
		pProgressDlg->UpdateWindow();
		pProgressDlg->SetStatus("Machine Resetting....", 25);
	}

	if (!IsMotorHomed(Z_SERVO_BOTTOM_MOTOR)) {

		nError = HomeMove(Z_SERVO_BOTTOM_MOTOR);
		if (nError) {
			if (pProgressDlg != NULL) {
				pProgressDlg->ShowWindow(FALSE);
				pProgressDlg->DestroyWindow();
				delete pProgressDlg;
				pProgressDlg = NULL;
			}
			return -1;
		}

		nError = WaitForMotionDone(Z_SERVO_BOTTOM_MOTOR, theApp.getMotionTimeOut() * 10);
		if (nError) {
			if (pProgressDlg != NULL) {
				pProgressDlg->ShowWindow(FALSE);
				pProgressDlg->DestroyWindow();
				delete pProgressDlg;
				pProgressDlg = NULL;
			}
			return -1;
		}

		nError = HomeDone(Z_SERVO_BOTTOM_MOTOR);
		if (nError) {
			if (pProgressDlg != NULL) {
				pProgressDlg->ShowWindow(FALSE);
				pProgressDlg->DestroyWindow();
				delete pProgressDlg;
				pProgressDlg = NULL;
			}
			return -1;
		}
	}

	if (pProgressDlg != NULL) {
		pProgressDlg->ShowWindow(TRUE);
		pProgressDlg->UpdateWindow();
		pProgressDlg->SetStatus("Machine Resetting....", 50);
	}

	// Do homing first if it's necessary
	if (!IsMotorHomed(X_SERVO_MOTOR)) {
		nError = HomeMove(X_SERVO_MOTOR, false);
		if (nError) {
			if (pProgressDlg != NULL) {
				pProgressDlg->ShowWindow(FALSE);
				pProgressDlg->DestroyWindow();
				delete pProgressDlg;
				pProgressDlg = NULL;
			}
			return -1;
		}
	}

	if (pProgressDlg != NULL) {
		pProgressDlg->ShowWindow(TRUE);
		pProgressDlg->UpdateWindow();
		pProgressDlg->SetStatus("Machine Resetting....", 55);
	}

	if (!IsMotorHomed(Y_SERVO_MOTOR)) {
		nError = HomeMove(Y_SERVO_MOTOR, false);
		if (nError) {
			if (pProgressDlg != NULL) {
				pProgressDlg->ShowWindow(FALSE);
				pProgressDlg->DestroyWindow();
				delete pProgressDlg;
				pProgressDlg = NULL;
			}
			return -1;
		}
	}

	if (pProgressDlg != NULL) {
		pProgressDlg->ShowWindow(TRUE);
		pProgressDlg->UpdateWindow();
		pProgressDlg->SetStatus("Machine Resetting....", 60);
	}

	if (!IsMotorHomed(X_SERVO_MOTOR)) {
		nError = WaitForMotionDone(X_SERVO_MOTOR, theApp.getMotionTimeOut() * 10);
		if (nError) {
			if (pProgressDlg != NULL) {
				pProgressDlg->ShowWindow(FALSE);
				pProgressDlg->DestroyWindow();
				delete pProgressDlg;
				pProgressDlg = NULL;
			}
			return -1;
		}

		nError = HomeDone(X_SERVO_MOTOR);
		if (nError) {
			if (pProgressDlg != NULL) {
				pProgressDlg->ShowWindow(FALSE);
				pProgressDlg->DestroyWindow();
				delete pProgressDlg;
				pProgressDlg = NULL;
			}
			return -1;
		}
	}

	if (pProgressDlg != NULL) {
		pProgressDlg->ShowWindow(TRUE);
		pProgressDlg->UpdateWindow();
		pProgressDlg->SetStatus("Machine Resetting....", 80);
	}

	if (!IsMotorHomed(Y_SERVO_MOTOR)) {
		nError = WaitForMotionDone(Y_SERVO_MOTOR, theApp.getMotionTimeOut() * 10);
		if (nError) {
			if (pProgressDlg != NULL) {
				pProgressDlg->ShowWindow(FALSE);
				pProgressDlg->DestroyWindow();
				delete pProgressDlg;
				pProgressDlg = NULL;
			}
			return -1;
		}

		nError = HomeDone(Y_SERVO_MOTOR);
		if (nError) {
			if (pProgressDlg != NULL) {
				pProgressDlg->ShowWindow(FALSE);
				pProgressDlg->DestroyWindow();
				delete pProgressDlg;
				pProgressDlg = NULL;
			}
			return -1;
		}
	}

	if (pProgressDlg != NULL) {
		pProgressDlg->ShowWindow(TRUE);
		pProgressDlg->UpdateWindow();
		pProgressDlg->SetStatus("Machine Resetting....", 100);
	}

	if (pProgressDlg != NULL) {
		pProgressDlg->SetStatus("Machine RESET Completed...", 100);;
		pProgressDlg->ShowWindow(FALSE);
		pProgressDlg->DestroyWindow();
		delete pProgressDlg;
		pProgressDlg = NULL;
	}

	// Get X and Y offset
	int nDieOffsetX = 0;
	int nDieOffsetY = 0;
	int nDeviceOffsetX = 0;
	int nDeviceOffsetY = 0;

	nError = GetDieXYOffset(nDieCol, nDieRow, nDieOffsetX, nDieOffsetY);
	if (nError) return -2;


	nError = GetDeviceXYOffset(nDeviceCol, nDeviceRow, nDeviceOffsetX, nDeviceOffsetY);
	if (nError)
		return -2;

	// Move X motor
	nError = AbsoluteMove(X_SERVO_MOTOR, motorStartScanPos[0] + nDieOffsetX + nDeviceOffsetX, pApp->motionParameters.getMovingSpeed(X_SERVO_MOTOR), false);
	if (nError) return -2;

	//Move Y motor
	nError = AbsoluteMove(Y_SERVO_MOTOR, motorStartScanPos[1] + nDieOffsetY + nDeviceOffsetY, pApp->motionParameters.getMovingSpeed(Y_SERVO_MOTOR), false);
	if (nError) return -2;

	//Waiting for both X&Y motor done
	nError = WaitForMotionDone(X_SERVO_MOTOR, theApp.getMotionTimeOut());
	if (nError) return -2;

	WaitMotorSettlingTime(X_SERVO_MOTOR);

	nError = WaitForMotionDone(Y_SERVO_MOTOR, theApp.getMotionTimeOut());
	if (nError) return -2;

	WaitMotorSettlingTime(Y_SERVO_MOTOR);

	return 0;
}


int MotionParameters::moveToReadyScanPos(bool bSafety)
{
	int nError = StopAxis(X_SERVO_MOTOR);
	if (nError)
		return nError;

	nError = StopAxis(Y_SERVO_MOTOR);
	if (nError)
		return nError;

	nError = StopAxis(Z_SERVO_TOP_MOTOR);
	if (nError)
		return nError;

	if (!IsMotorHomed(X_SERVO_MOTOR)) {
		nError = HomeMove(X_SERVO_MOTOR, false);
		if (nError)
			return nError;
	}

	if (!IsMotorHomed(Y_SERVO_MOTOR)) {
		nError = HomeMove(Y_SERVO_MOTOR, false);
		if (nError)
			return nError;
	}

	if (!IsMotorHomed(Z_SERVO_TOP_MOTOR)) {
		nError = HomeMove(Z_SERVO_TOP_MOTOR, false);
		if (nError)
			return nError;
	}

	if (!IsMotorHomed(X_SERVO_MOTOR)) {

		nError = WaitForMotionDone(X_SERVO_MOTOR, theApp.getMotionTimeOut());
		if (nError)
			return nError;

		nError = HomeDone(X_SERVO_MOTOR);
		if (nError)
			return nError;
	}

	if (!IsMotorHomed(Y_SERVO_MOTOR)) {

		nError = WaitForMotionDone(Y_SERVO_MOTOR, theApp.getMotionTimeOut());
		if (nError)
			return nError;

		nError = HomeDone(Y_SERVO_MOTOR);
		if (nError)
			return nError;
	}

	if (!IsMotorHomed(Z_SERVO_TOP_MOTOR)) {

		nError = WaitForMotionDone(Z_SERVO_TOP_MOTOR, theApp.getMotionTimeOut());
		if (nError)
			return nError;

		nError = HomeDone(Z_SERVO_TOP_MOTOR);
		if (nError)
			return nError;
	}

	//nError = AbsoluteMove(X_SERVO_MOTOR, motorInitScanPos[X_SERVO_MOTOR], getMovingSpeed(X_SERVO_MOTOR), false);
	//if (nError)
	//	return nError;

	//nError = AbsoluteMove(Y_SERVO_MOTOR, motorInitScanPos[Y_SERVO_MOTOR], getMovingSpeed(Y_SERVO_MOTOR), false);
	//if (nError)
	//	return nError;

	//nError = AbsoluteMove(Z_SERVO_TOP_MOTOR, motorInitScanPos[Z_SERVO_TOP_MOTOR], getMovingSpeed(Z_SERVO_MOTOR), false);
	//if (nError)
	//	return nError;

	return 0;
}


BOOL MotionParameters::IsMachineHomed() const
{
	bool bHomedOn = true;
	for(int i = 0; i<motorCount; i++)
		bHomedOn = bHomedOn & motorEngine[i].isHomed();

	return bHomedOn;
}

int MotionParameters::SetAlarm()
{
	return motorResource.SetAlarm();
}

void MotionParameters::getDeviceSize(double & dWidthMM, double & dHeightMM) const
{
	dWidthMM = dDeviceSizeInXMM;
	dHeightMM = dDeviceSizeInYMM;
}

void MotionParameters::setDeviceSize(double dWidthMM, double dHeightMM)
{
	dDeviceSizeInXMM = dWidthMM;
	dDeviceSizeInYMM = dHeightMM;
}

void MotionParameters::getDevicePitch(double & dWidthMM, double & dHeightMM) const
{
	dWidthMM = m_dDevicePitch[0];
	dHeightMM = m_dDevicePitch[1];
}

void MotionParameters::setDevicePitch(double dWidthMM, double dHeightMM)
{
	m_dDevicePitch[0] = dWidthMM;
	m_dDevicePitch[1] = dHeightMM;
}


void MotionParameters::getDeviceScanArea(double & dScanAreaX, double & dScanAreaY) const
{
	dScanAreaX = m_dDeviceScanArea[0];
	dScanAreaY = m_dDeviceScanArea[1];
}

void MotionParameters::setDeviceScanArea(double dScanAreaX, double dScanAreaY)
{
	m_dDeviceScanArea[0] = dScanAreaX;
	m_dDeviceScanArea[1] = dScanAreaY;
}

void MotionParameters::getDeviceOverlap(double & dScanOverlapX, double & dScanOverlapY) const
{
	dScanOverlapX = m_dDeviceOverlap[0];
	dScanOverlapY = m_dDeviceOverlap[1];
}

void MotionParameters::setDeviceOverlap(double dScanOverlapX, double dScanOverlapY)
{
	m_dDeviceOverlap[0] = dScanOverlapX;
	m_dDeviceOverlap[1] = dScanOverlapY;
}

void MotionParameters::setDeviceDirection(BOOL bHorizontal)
{
	bHorizontalDevice = bHorizontal;
}

void MotionParameters::getDeviceDirection(BOOL & bHorizontal) const
{
	bHorizontal = bHorizontalDevice;
}

void MotionParameters::GetMappingParameters(int & nBlockX, int & nBlockY, double & dBlockPitchX, double & dBlockPitchY, int & nDieX, int & nDieY,
	double & dDiePitchX, double & dDiePitchY, int & nDieFOVX, int & nDieFOVY) const
{
	nBlockX = m_nNoOfBlock[0];
	nBlockY = m_nNoOfBlock[1];
	dBlockPitchX = m_dBlockPitch[0];
	dBlockPitchY = m_dBlockPitch[1];
	nDieX = m_nDeviceNoPerBlock[0];
	nDieY = m_nDeviceNoPerBlock[1];
	dDiePitchX = m_dDevicePitch[0];
	dDiePitchY = m_dDevicePitch[1];
	nDieFOVX = nDieColumn;
	nDieFOVY = nDieRow;
}

int MotionParameters::SetSubstrateSelected(int nColumn, int nRow)
{
	m_nSubstrateDieColumn = nColumn;
	m_nSubstrateDieRow = nRow;
	return 0;
}

int MotionParameters::GetSubstrateSelected(int & nColumn, int & nRow)
{
	nColumn = m_nSubstrateDieColumn;
	nRow = m_nSubstrateDieRow;

	return 0;
}

void MotionParameters::GetMappingParameters(int & nBlockX, int & nBlockY, int & nDieX, int & nDieY) const
{
	nBlockX = m_nNoOfBlock[0];
	nBlockY = m_nNoOfBlock[1];
	nDieX = m_nDeviceNoPerBlock[0];
	nDieY = m_nDeviceNoPerBlock[1];
}

void MotionParameters::GetMappingParameters(int & nDieFOVX, int & nDieFOVY) const
{
	nDieFOVX = m_nDeviceNoPerBlock[0];
	nDieFOVY = m_nDeviceNoPerBlock[1];
}

int MotionParameters::SetMappingParameters(int nBlockX, int nBlockY, double dBlockPitchX, double dBlockPitchY, int nDieX, 
	int nDieY, double dDiePitchX, double dDiePitchY, int nDieFOVX, int nDieFOVY)
{
	if(nBlockX < 1 || nBlockY < 1 || dBlockPitchX < 0 || dBlockPitchY < 0 || nDieX < 1 || nDieY < 1 || dDiePitchX < 0 || dDiePitchY < 0 || nDieFOVX < 1 || nDieFOVY < 1 ||
		nDieFOVX > nDieX || nDieFOVY > nDieY /* ||((nDieX - 1) * dDiePitchX >= dBlockPitchX && nBlockX > 1) ||*/ /*((nDieY - 1) * dDiePitchY >= dBlockPitchY && nBlockY > 1)*/)
		return -1;

	m_nNoOfBlock[0] = nBlockX;
	m_nNoOfBlock[1] = nBlockY;
	m_dBlockPitch[0] = dBlockPitchX;
	m_dBlockPitch[1] = dBlockPitchY;
	m_nDeviceNoPerBlock[0] = nDieX;
	m_nDeviceNoPerBlock[1] = nDieY;
	m_dDevicePitch[0] = dDiePitchX;
	m_dDevicePitch[1] = dDiePitchY;
	nDieColumn = nDieFOVX;
	nDieRow = nDieFOVY;
//	m_dInitialOffset = dInitialOffset;

	return 0;
}

int MotionParameters::SetMappingParameters(int nBlockX, int nBlockY, int nDieX, int nDieY, BOOL bHorizontal)
{
	if (nBlockX < 1 || nBlockY < 1 || nDieX < 1 || nDieY < 1)
		return -1;

	m_nNoOfBlock[0] = nBlockX;
	m_nNoOfBlock[1] = nBlockY;
	m_nDeviceNoPerBlock[0] = nDieX;
	m_nDeviceNoPerBlock[1] = nDieY;
	bHorizontalDevice = bHorizontal;

	return 0;
}

// Merging interraction for 2-3D station
//int MotionParameters::GetTotalImageGrabs() const
//{
//	return nTotalImageFOV;
//}

//void MotionParameters::SaveScanningOptions()
//{
//	CIniFile iniFile;
//	CFileException e;
//
//	CString strFile, strKey, strTemp;
//
//	strFile.Format("%s\\%s\\%s\\Scanning Options.info", pApp->m_strMainCfgDir, pApp->m_strMainCfgModeDir, pApp->m_strConfigFile);
//	iniFile.SetPath(strFile);
//
//	strKey.Format("Scanning Options");
//
//	iniFile.SetBoolean(strKey, "Enable Barcode Read Status", m_bEnablebarcodeReader);
//	iniFile.SetBoolean(strKey, "Enable Clamping Sequence", m_bEnableClampingSequence);
//	iniFile.SetInteger(strKey, "Clamping Type", m_nClampingType);
//	iniFile.SetBoolean(strKey, "Enable Ionizer", m_bEnableIonizer);
//	iniFile.SetBoolean(strKey, "Enable Top Scanning", m_bEnableTopScanning);
//	iniFile.SetBoolean(strKey, "Enable THR Scanning", m_bEnableTHRScanning);
//
//	iniFile.WriteFile();
//}

//void MotionParameters::loadScanningOptions()
//{
//	CIniFile iniFile;
//	CFileException e;
//
//	CString strFile, strKey, strTemp;
//
//	int nInt;
//
//	strFile.Format("%s\\%s\\%s\\Scanning Options.info", pApp->m_strMainCfgDir, pApp->m_strMainCfgModeDir, pApp->m_strConfigFile);
//	iniFile.SetPath(strFile);
//
//	strKey.Format("Scanning Options");
//
//	if (iniFile.ReadFile()) {
//		m_bEnablebarcodeReader = iniFile.GetBoolean(strKey, "Enable Barcode Read Status", m_bEnablebarcodeReader);
//		m_bEnableClampingSequence = iniFile.GetBoolean(strKey, "Enable Clamping Sequence", m_bEnableClampingSequence);
//		m_nClampingType = iniFile.GetInteger(strKey, "Clamping Type", m_nClampingType);
//		m_bEnableIonizer = iniFile.GetBoolean(strKey, "Enable Ionizer", m_bEnableIonizer);
//		m_bEnableTopScanning = iniFile.GetBoolean(strKey, "Enable Top Scanning", m_bEnableTopScanning);
//		m_bEnableTHRScanning = iniFile.GetBoolean(strKey, "Enable THR Scanning", m_bEnableTHRScanning);
//	}
//}

void MotionParameters::SaveScanningParm(bool saveMappingData) const
{
	CIniFile iniFile;
	CFileException e;

	CString strFile, strKey, strTemp;

	strFile.Format("%s\\%s\\%s\\Scanning Parameters.info", pApp->m_strMainCfgDir, pApp->m_strMainCfgModeDir, pApp->m_strConfigFile);
	iniFile.SetPath(strFile);

	strKey.Format("Scanning Parameters");

	std::string str;

	// X-Motor
	str = motorEngine[X_SERVO_MOTOR].getName() + " Start Scanning Position (pps)";
	iniFile.SetInteger(strKey, str.c_str(), motorStartScanPos[0]);

	// Y-Motor
	str = motorEngine[Y_SERVO_MOTOR].getName() + " Start Scanning Position (pps)";
	iniFile.SetInteger(strKey, str.c_str(), motorStartScanPos[1]);

	// Z-Motor
	str = motorEngine[Z_SERVO_TOP_MOTOR].getName() + " Ready Position (pps)";
	iniFile.SetInteger(strKey, str.c_str(), motorStartScanPos[2]);
	
	str = motorEngine[Z_SERVO_BOTTOM_MOTOR].getName() + " Ready Position (pps)";
	iniFile.SetInteger(strKey, str.c_str(), motorStartScanPos[3]);


	for(int i = 0; i < motorCount; i++) {
		Speed ScanSpeed = getScanningSpeed(i);
		str = motorEngine[i].getName() + " Start Scanning Speed (pps/sec)";
		iniFile.SetInteger(strKey, str.c_str(), ScanSpeed.startSpeed);

		str = motorEngine[i].getName() + " Maximum Scanning Speed (pps/sec)";
		iniFile.SetInteger(strKey, str.c_str(), ScanSpeed.maximumSpeed);

		ScanSpeed = getScanningSpeedTHR(i);
		str = motorEngine[i].getName() + " Start Scanning Speed THR (pps/sec)";
		iniFile.SetInteger(strKey, str.c_str(), ScanSpeed.startSpeed);

		str = motorEngine[i].getName() + " Maximum Scanning Speed THR (pps/sec)";
		iniFile.SetInteger(strKey, str.c_str(), ScanSpeed.maximumSpeed);
	}

	for(int i = 0; i < motorCount; i++) {
		Speed ScanSpeed = getMovingSpeed(i);
		str = motorEngine[i].getName() + " Start Moving Speed (pps/sec)";
		iniFile.SetInteger(strKey, str.c_str(), ScanSpeed.startSpeed);

		str = motorEngine[i].getName() + " Maximum Moving Speed (pps/sec)";
		iniFile.SetInteger(strKey, str.c_str(), ScanSpeed.maximumSpeed);
	}

	for (int nFOV = 0; nFOV < m_nDeviceNoPerBlock[0] * m_nDeviceNoPerBlock[1]; nFOV++) {
		for (int nTrack = 0; nTrack < theApp.m_nTotalTracks; nTrack++) {
			for (int nDoc = 0; nDoc <theApp.m_nTotalViewDocs[nTrack]; nDoc++) {
				strTemp.Format("T%d FOV%d Doc%d %s", nTrack + 1, nFOV + 1, nDoc + 1, !nTrack ? "TopZPos" : "BotZPos");
				iniFile.SetInteger(strKey, strTemp, theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_nZAxisPosition[nFOV]);
				strTemp.Format("T%d FOV%d Doc%d %s", nTrack + 1, nFOV + 1, nDoc + 1, !nTrack ? "Top GrabEnable" : "Bottom GrabEnable");
				iniFile.SetBoolean(strKey, strTemp, theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_bEnableGrab[nFOV]);

			}
		}
	}

	iniFile.WriteFile();
}

void MotionParameters::LoadScanningParm()
{
	CIniFile iniFile;
	CFileException e;

	CString strFile, strKey, strTemp;

	int nInt;
	
	strFile.Format("%s\\%s\\%s\\Scanning Parameters.info", pApp->m_strMainCfgDir, pApp->m_strMainCfgModeDir, pApp->m_strConfigFile);
	iniFile.SetPath(strFile);

	strKey.Format("Scanning Parameters");

	if(iniFile.ReadFile()) {
		std::string str;

		//X-Motor
		str = motorEngine[X_SERVO_MOTOR].getName() + " Start Scanning Position (pps)";
		motorStartScanPos[0] = iniFile.GetInteger(strKey, str.c_str(), motorStartScanPos[0]);

		str = motorEngine[Y_SERVO_MOTOR].getName() + " Start Scanning Position (pps)";
		motorStartScanPos[1] = iniFile.GetInteger(strKey, str.c_str(), motorStartScanPos[1]);

		str = motorEngine[Z_SERVO_TOP_MOTOR].getName() + " Ready Position (pps)";
		motorStartScanPos[2] = iniFile.GetInteger(strKey, str.c_str(), motorStartScanPos[2]);

		str = motorEngine[Z_SERVO_BOTTOM_MOTOR].getName() + " Ready Position (pps)";
		motorStartScanPos[3] = iniFile.GetInteger(strKey, str.c_str(), motorStartScanPos[3]);

		for(int i = 0; i < motorCount; i++) {
			Speed ScanSpeed;
			str = motorEngine[i].getName() + " Start Scanning Speed (pps/sec)";
			ScanSpeed.startSpeed = iniFile.GetInteger(strKey, str.c_str(), ScanSpeed.startSpeed);

			str = motorEngine[i].getName() + " Maximum Scanning Speed (pps/sec)";
			ScanSpeed.maximumSpeed = iniFile.GetInteger(strKey, str.c_str(), ScanSpeed.maximumSpeed);

			setScanningSpeed(i, ScanSpeed);

			str = motorEngine[i].getName() + " Start Scanning Speed THR (pps/sec)";
			ScanSpeed.startSpeed = iniFile.GetInteger(strKey, str.c_str(), ScanSpeed.startSpeed);

			str = motorEngine[i].getName() + " Maximum Scanning Speed THR (pps/sec)";
			ScanSpeed.maximumSpeed = iniFile.GetInteger(strKey, str.c_str(), ScanSpeed.maximumSpeed);

//			setScanningSpeedTHR(i, ScanSpeed);
		}

		for(int i = 0; i < motorCount; i++) {
			Speed ScanSpeed;
			str = motorEngine[i].getName() + " Start Moving Speed (pps/sec)";
			ScanSpeed.startSpeed = iniFile.GetInteger(strKey, str.c_str(), ScanSpeed.startSpeed);

			str = motorEngine[i].getName() + " Maximum Moving Speed (pps/sec)";
			ScanSpeed.maximumSpeed = iniFile.GetInteger(strKey, str.c_str(), ScanSpeed.maximumSpeed);

			setMovingSpeed(i, ScanSpeed);
		} 


		for (int nFOV = 0; nFOV < m_nDeviceNoPerBlock[0] * m_nDeviceNoPerBlock[1]; nFOV++) {
			for (int nTrack = 0; nTrack < theApp.m_nTotalTracks; nTrack++) {
				for (int nDoc = 0; nDoc < theApp.m_nTotalViewDocs[nTrack]; nDoc++) {
					if (theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_nZAxisPosition.size() > nFOV) {
						strTemp.Format("T%d FOV%d Doc%d %s", nTrack + 1, nFOV + 1, nDoc + 1, !nTrack ? "TopZPos" : "BotZPos");
						theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_nZAxisPosition[nFOV] = iniFile.GetInteger(strKey, strTemp, theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_nZAxisPosition[nFOV]);
						strTemp.Format("T%d FOV%d Doc%d %s", nTrack + 1, nFOV + 1, nDoc + 1, !nTrack ? "Top GrabEnable" : "Bottom GrabEnable");
						theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_bEnableGrab[nFOV] = iniFile.GetBoolean(strKey, strTemp, theApp.m_pTracks[nTrack].m_pDoc[nDoc]->m_bEnableGrab[nFOV]);
					}
				}
			}
		}
	}
}

void MotionParameters::SaveDeviceParameters() const
{

	CIniFile iniFile;
	CFileException e;

	CString strFile, strKey, strTemp;

	//strFile.Format("%s\\DeviceConfiguration.dat", pApp->m_strConfigDir);
	strFile.Format("%s\\%s\\%s\\DeviceConfiguration.dat", pApp->m_strMainCfgDir, pApp->m_strMainCfgModeDir, pApp->m_strConfigFile);
	iniFile.SetPath(strFile);
	strKey.Format("Device Config");
	for(int b = 0; b<MAX_DIRECTION; b++) {
		strTemp.Format("No. Of Block[%c]", !b ? 'X' : 'Y');
		iniFile.SetInteger(strKey, strTemp, m_nNoOfBlock[b]);
		strTemp.Format("Block Pitch[%c]", !b ? 'X' : 'Y');
		iniFile.SetDouble(strKey, strTemp, m_dBlockPitch[b]);
		strTemp.Format("Device No. Per Block[%c]", !b ? 'X' : 'Y');
		iniFile.SetInteger(strKey, strTemp, m_nDeviceNoPerBlock[b]);
		strTemp.Format("Device Pitch[%c]", !b ? 'X' : 'Y');
		iniFile.SetDouble(strKey, strTemp, m_dDevicePitch[b]);
		strTemp.Format("Device Scan Area[%c]", !b ? 'X' : 'Y');
		iniFile.SetDouble(strKey, strTemp, m_dDeviceScanArea[b]);
		///strTemp.Format("Device Offset[%c]", !b ? 'X' : 'Y');
		//iniFile.SetDouble(strKey, strTemp, m_dDeviceOffset[b]);
		strTemp.Format("Device Overlap[%c]", !b ? 'X' : 'Y');
		iniFile.SetDouble(strKey, strTemp, m_dDeviceOverlap[b]);
		iniFile.SetDouble(strKey, "Device Initial Position", m_dInitialOffset);
	}
	iniFile.SetBoolean(strKey, "Horizontal Device", bHorizontalDevice);
	iniFile.WriteFile();
}


void MotionParameters::LoadDeviceParameters()
{
	CIniFile iniFile;
	CFileException e;

	CString strFile, strKey, strTemp;

	////strFile.Format("%s\\DeviceConfiguration.dat", pApp->m_strConfigDir);
	strFile.Format("%s\\%s\\%s\\DeviceConfiguration.dat", pApp->m_strMainCfgDir, pApp->m_strMainCfgModeDir, pApp->m_strConfigFile);
	iniFile.SetPath(strFile);

	strKey.Format("Device Config");
	if(iniFile.ReadFile()) {
		for(int a = 0; a<MAX_DIRECTION; a++) {
			strTemp.Format("No. Of Block[%c]", !a ? 'X' : 'Y');
			m_nNoOfBlock[a] = iniFile.GetInteger(strKey, strTemp, m_nNoOfBlock[a]);
			strTemp.Format("Block Pitch[%c]", !a ? 'X' : 'Y');
			m_dBlockPitch[a] = iniFile.GetDouble(strKey, strTemp, m_dBlockPitch[a]);
			strTemp.Format("Device No. Per Block[%c]", !a ? 'X' : 'Y');
			m_nDeviceNoPerBlock[a] = iniFile.GetInteger(strKey, strTemp, m_nDeviceNoPerBlock[a]);
			strTemp.Format("Device Pitch[%c]", !a ? 'X' : 'Y');
			m_dDevicePitch[a] = iniFile.GetDouble(strKey, strTemp, m_dDevicePitch[a]);
			strTemp.Format("Device Scan Area[%c]", !a ? 'X' : 'Y');
			m_dDeviceScanArea[a] = iniFile.GetDouble(strKey, strTemp, m_dDeviceScanArea[a]);
			//strTemp.Format("Device Offset[%c]", !a ? 'X' : 'Y');
			//m_dDeviceOffset[a] = iniFile.GetDouble(strKey, strTemp, m_dDeviceOffset[a]);
			strTemp.Format("Device Overlap[%c]", !a ? 'X' : 'Y');
			m_dDeviceOverlap[a] = iniFile.GetDouble(strKey, strTemp, m_dDeviceOverlap[a]);
			m_dInitialOffset = iniFile.GetDouble(strKey, "Device Initial Position", m_dInitialOffset);
		}
		bHorizontalDevice = iniFile.GetBoolean(strKey, "Horizontal Device", bHorizontalDevice);
	}
}

int MotionParameters::CreateScanPositions()
{
	// 3. Calculate the initial number of images
//	int nBondScanColumnNo = nDiePerBlockInX * nBlockPerXNo;
//	int nBondScanRowNo = nDiePerBlockInY * nBlockPerYNo;

	int nDieIDX = 0;
	int nDieIDY = 0;
	int nImageIDPerDieX = 0;
	int nImageIDPerDieY = 0;

	int nXMotorPosition = 0;
	int nYMotorPosition = 0;

	double dStepX = 0;
	double dStepY = 0;

	int nDirectionX = 1;
	int nDirectionYTop = -1;

	nFOVPerDieinX = m_nDeviceNoPerBlock[0];
	nFOVPerDieinY = m_nDeviceNoPerBlock[1];

	nColumnNoPerDie = m_nDeviceNoPerBlock[0];
	nRowNoPerDie = m_nDeviceNoPerBlock[1];

	nRowNoPerBlock = m_nDeviceNoPerBlock[1] * m_nNoOfBlock[1];


	double dDiePerXPitchMM = m_dBlockPitch[0];
	double dFOVPerXPitchMM = m_dDevicePitch[0];

	double dDiePerYPitchMM = m_dBlockPitch[1];
	double dFOVPerYPitchMM = m_dDevicePitch[1];
	
	int nDieNoX = m_nNoOfBlock[0];
	int nDieNoY = m_nNoOfBlock[1];	//Hardcode

	int nCameraGrabNoPerDevice = 1;  // In case of Multile Docs
	double dGrabOffsetMM = 2.0; 

	motorScanPositionX.resize(nDieNoX * nFOVPerDieinX);		//Remove
	motorScanPositionY.resize(nDieNoY * nFOVPerDieinY);

	vDieFovNo.resize(nRowNoPerDie * nColumnNoPerDie);
	vDieMapNo.resize(nRowNoPerDie * nColumnNoPerDie);
	vScanDieOrder.resize(nRowNoPerDie * nColumnNoPerDie);

	m_nTotalNoOfFov = nFOVPerDieinX * nFOVPerDieinY;
	m_nTotalNoDevice = m_nNoOfBlock[0] * m_nNoOfBlock[1];

	bDeviceEnabled.resize(m_nTotalNoDevice);
	bDieEnabled.resize(m_nTotalNoOfFov);

	std::fill(bDeviceEnabled.begin(), bDeviceEnabled.end(), TRUE);
	std::fill(bDieEnabled.begin(), bDieEnabled.end(), TRUE);

	ZLevelNoPerPosition.resize(m_nTotalNoOfFov, 0);
	RelativeXPosition.resize(m_nTotalNoOfFov, 0);
	RelativeYPosition.resize(m_nTotalNoOfFov, 0);

	RelativeXDevicePosition.resize(nDieNoX * nDieNoY);
	RelativeYDevicePosition.resize(nDieNoX * nDieNoY);

	m_nTotalGrabNo = m_nNoOfBlock[0] * m_nNoOfBlock[1] * m_nDeviceNoPerBlock[0] * m_nDeviceNoPerBlock[1];

	resize2dVector(ZLevelIDPerGrab, m_nTotalNoOfFov, 0);


	nDirectionX = -1;
	nDirectionYTop = -1;


	int nDiePos = 0;
	for (int i = 0; i < nDieNoY; i++) {
		for (int j = 0; j < nDieNoX; j++) {
			RelativeXDevicePosition[nDiePos] = Round(nDirectionX    * j * m_dBlockPitch[0] * motorEngine[X_SERVO_MOTOR].getResolution());
			RelativeYDevicePosition[nDiePos] = Round(nDirectionYTop    * i * m_dBlockPitch[1] * motorEngine[Y_SERVO_MOTOR].getResolution());
			nDiePos++;
		}
	}


	int nGrabNo = 0;
	for (int i = 0; i<nRowNoPerDie; i++) {

		for (int j = 0; j<nColumnNoPerDie; j++) {
			RelativeXPosition[nGrabNo] = Round(nDirectionX    * j * m_dDevicePitch[0] * motorEngine[X_SERVO_MOTOR].getResolution());
			RelativeYPosition[nGrabNo] = Round(nDirectionYTop * i * m_dDevicePitch[1] * motorEngine[Y_SERVO_MOTOR].getResolution());
			nGrabNo++;
		}
	}

	
	for (int i = 0; i < getTotalFovPerDevice(); i++) {
		if (bHorizontalDevice) 
			vDieFovNo[i] = i % nFOVPerDieinX + nFOVPerDieinX * (i / nFOVPerDieinX);
		else 
			vDieFovNo[i] = i / nFOVPerDieinX + nFOVPerDieinY * (i % nFOVPerDieinX);
		
		vDieMapNo[vDieFovNo[i]] = i;
	}

	int nFov = 0;
	if (bHorizontalDevice) {
		for (int i = 0; i < nFOVPerDieinY; i++) {
			for (int j = 0; j < nFOVPerDieinX; j++, nFov++) {

				if (i % 2 == 0)
					vScanDieOrder[i * nFOVPerDieinX + j] = nFov;
				else
					vScanDieOrder[i * nFOVPerDieinX + (nFOVPerDieinX - 1 - j)] = nFov;
			}
		}
	}
	else {
		for (int i = 0; i < nFOVPerDieinX; i++) {
			for (int j = 0; j < nFOVPerDieinY; j++, nFov++) {

				if (i % 2 == 0)
					vScanDieOrder[i * nFOVPerDieinY + j] = nFov;
				else
					vScanDieOrder[i * nFOVPerDieinY + (nFOVPerDieinY - 1 - j)] = nFov;
			}
		}
	}

	

	scanningReady = true;
//	motorStartScanPos

	return 0;
}

void MotionParameters::SaveMotionParm() const
{
	CString strFile;
	strFile.Format("%s\\Motor.info", theApp.m_strMainCfgDir);

	CIniFile iniFile;
	CIniFile iniMotionPara;
	CFileException e;

	CString strKey, strTemp;
	std::string str;
	//strFile.Format("%s\\DeviceConfiguration.dat", pApp->m_strConfigDir);
	iniFile.SetPath(strFile);

	for(int i = 0; i < motorCount; i++) {
		
		iniFile.SetInteger(motorEngine[i].getName().c_str(), "Axis ID", motorEngine[i]._getAxisId());
		iniFile.SetInteger(motorEngine[i].getName().c_str(), "Homing mode", motorEngine[i]._getHomingMode());
		iniFile.SetInteger(motorEngine[i].getName().c_str(), "Negative Soft Limit", motorEngine[i].getNegativeSoftLimit());
		iniFile.SetInteger(motorEngine[i].getName().c_str(), "Positive Soft Limit", motorEngine[i].getPositiveSoftLimit());
		iniFile.SetBoolean(motorEngine[i].getName().c_str(), "Forward movement", motorEngine[i].isForwardMovement());
		iniFile.SetBoolean(motorEngine[i].getName().c_str(), "Forward homing", motorEngine[i].isForwardHoming());
		iniFile.SetInteger(motorEngine[i].getName().c_str(), "Home origin offset (pps)", motorEngine[i].getHomeOriginOffset());
		iniFile.SetInteger(motorEngine[i].getName().c_str(), "Settling time (ms)", motorEngine[i].getSettlingTime());
		iniFile.SetInteger(motorEngine[i].getName().c_str(), "Speed limit (mm/sec)", motorEngine[i].getSpeedLimit());
		iniFile.SetInteger(motorEngine[i].getName().c_str(), "Start Homing speed (mm/sec)", motorEngine[i].getHomingSpeed().startSpeed);
		iniFile.SetInteger(motorEngine[i].getName().c_str(), "Maximum Homing speed (mm/sec)", motorEngine[i].getHomingSpeed().maximumSpeed);
	}	
	iniFile.WriteFile();

	//strFile.Format("%s\\Motion Parameters.info", theApp.m_strMainCfgDir);
	//strKey.Format("Motor predefined positions");
	//iniMotionPara.SetPath(strFile);

	//str = motorEngine[X_SERVO_MOTOR].getName() + " Initial Position (pps)";
	//iniMotionPara.SetInteger(strKey, str.c_str(), motorInitScanPos[X_SERVO_MOTOR]);

	//// Y-Motor
	//str = motorEngine[Y_SERVO_MOTOR].getName() + " Initial Position (pps)";
	//iniMotionPara.SetInteger(strKey, str.c_str(), motorInitScanPos[Y_SERVO_MOTOR]);

	//// Z-Motor
	//str = motorEngine[Z_SERVO_TOP_MOTOR].getName() + " Initial Position (pps)";
	//iniMotionPara.SetInteger(strKey, str.c_str(), motorInitScanPos[Z_SERVO_TOP_MOTOR]);

	//str = motorEngine[Z_SERVO_TOP_MOTOR].getName() + " Initial Position (pps)";
	//iniMotionPara.SetInteger(strKey, str.c_str(), motorInitScanPos[Z_SERVO_TOP_MOTOR]);

	//
	//str = motorEngine[X_SERVO_MOTOR].getName() + " Module Unload Position (pps)";
	//iniMotionPara.SetInteger(strKey, str.c_str(), motorModuleUnloadPos[X_SERVO_MOTOR]);

	//str = motorEngine[Y_SERVO_MOTOR].getName() + " Module Unload Position (pps)";
	//iniMotionPara.SetInteger(strKey, str.c_str(), motorModuleUnloadPos[Y_SERVO_MOTOR]);

	//str = motorEngine[Z_SERVO_MOTOR].getName() + " Module Unload Position (pps)";
	//iniMotionPara.SetInteger(strKey, str.c_str(), motorModuleUnloadPos[Z_SERVO_MOTOR]);

	//// X-Motor
	//strKey.Format("Motor predefined measurements");
	//str = motorEngine[X_SERVO_MOTOR].getName() + " Scanning Offset (mm)";
	//iniMotionPara.SetInteger(strKey, str.c_str(), dMotorXScanOffsetMM);

	//iniMotionPara.SetDouble(strKey, "Z Sensor to TOP Camera Distance (mm)", m_dDistanceZSensorToTop);
	//iniMotionPara.SetDouble(strKey, "TOP Camera to THR Camera Distance (mm)", m_dDistanceTopToTHR);

	//iniMotionPara.WriteFile();

}
void MotionParameters::LoadMotionParm()
{
	CIniFile iniFile;
	CIniFile iniMotionPara;

	CFileException e;

	CString strFile, strKey, strTemp;

	int nTemp = 0, nTemp2 = 0;
	bool bTemp = false;

	strFile.Format("%s\\Motor.info", theApp.m_strMainCfgDir);
	iniFile.SetPath(strFile);

	if(iniFile.ReadFile()) {
		for(int i = 0; i < motorCount; i++) {
			nTemp = iniFile.GetInteger(motorEngine[i].getName().c_str(), "Axis ID", nTemp);
			motorEngine[i].setAxisId(nTemp);

			nTemp = iniFile.GetInteger(motorEngine[i].getName().c_str(), "Homing mode", nTemp);
			motorEngine[i].setHomingMode(nTemp);

			nTemp = iniFile.GetInteger(motorEngine[i].getName().c_str(), "Negative Soft Limit", nTemp);
			motorEngine[i].setNegativeSoftLimit(nTemp);

			nTemp = iniFile.GetInteger(motorEngine[i].getName().c_str(), "Positive Soft Limit", nTemp);
			motorEngine[i].setPositiveSoftLimit(nTemp);

			bTemp = iniFile.GetBoolean(motorEngine[i].getName().c_str(), "Forward movement", bTemp);
			motorEngine[i].setForwardMovement(bTemp);

			bTemp = iniFile.GetBoolean(motorEngine[i].getName().c_str(), "Forward homing", bTemp);
			motorEngine[i].setForwardHoming(bTemp);

			nTemp = iniFile.GetInteger(motorEngine[i].getName().c_str(), "Home origin offset (pps)", nTemp);
			motorEngine[i].setHomeOriginOffset(nTemp);

			nTemp = iniFile.GetInteger(motorEngine[i].getName().c_str(), "Settling time (ms)", nTemp);
			motorEngine[i].setSettlingTime(nTemp);

			nTemp = iniFile.GetInteger(motorEngine[i].getName().c_str(), "Speed limit (mm/sec)", nTemp);
			motorEngine[i].setSpeedLimit(nTemp);

			nTemp = iniFile.GetInteger(motorEngine[i].getName().c_str(), "Start Homing speed (mm/sec)", nTemp);
			nTemp2 = iniFile.GetInteger(motorEngine[i].getName().c_str(), "Maximum Homing speed (mm/sec)", nTemp2);

			motorEngine[i].setHomingSpeed(nTemp, nTemp2);
		}
	}

	//std::string str;
	//strFile.Format("%s\\Motion Parameters.info", theApp.m_strMainCfgDir);
	//strKey.Format("Motor predefined positions");
	//iniMotionPara.SetPath(strFile);
	//if(iniMotionPara.ReadFile()) {
	//	//X-Motor
	//	str = motorEngine[X_SERVO_MOTOR].getName() + " Initial Position (pps)";
	//	motorInitScanPos[X_SERVO_MOTOR] = iniMotionPara.GetInteger(strKey, str.c_str(), motorInitScanPos[X_SERVO_MOTOR]);

	//	str = motorEngine[Y_SERVO_MOTOR].getName() + " Initial Position (pps)";
	//	motorInitScanPos[Y_SERVO_MOTOR] = iniMotionPara.GetInteger(strKey, str.c_str(), motorInitScanPos[Y_SERVO_MOTOR]);

	//	str = motorEngine[Z_SERVO_MOTOR].getName() + " Initial Position (pps)";
	//	motorInitScanPos[Z_SERVO_MOTOR] = iniMotionPara.GetInteger(strKey, str.c_str(), motorInitScanPos[Z_SERVO_MOTOR]);

	//	str = motorEngine[X_SERVO_MOTOR].getName() + " Module Unload Position (pps)";
	//	motorModuleUnloadPos[X_SERVO_MOTOR] = iniMotionPara.GetInteger(strKey, str.c_str(), motorModuleUnloadPos[X_SERVO_MOTOR]);
	//
	//	str = motorEngine[Y_SERVO_MOTOR].getName() + " Module Unload Position (pps)";
	//	motorModuleUnloadPos[Y_SERVO_MOTOR] = iniMotionPara.GetInteger(strKey, str.c_str(), motorModuleUnloadPos[Y_SERVO_MOTOR]);
	//
	//	str = motorEngine[Z_SERVO_MOTOR].getName() + " Module Unload Position (pps)";
	//	motorModuleUnloadPos[Z_SERVO_MOTOR] = iniMotionPara.GetInteger(strKey, str.c_str(), motorModuleUnloadPos[Z_SERVO_MOTOR]);

	//	//X-Motor
	//	strKey.Format("Motor predefined measurements");
	//	str = motorEngine[X_SERVO_MOTOR].getName() + " Scanning Offset (mm)";
	//	dMotorXScanOffsetMM = iniMotionPara.GetInteger(strKey, str.c_str(), dMotorXScanOffsetMM);

	//	m_dDistanceZSensorToTop = iniMotionPara.GetDouble(strKey, "Z Sensor to TOP Camera Distance (mm)", m_dDistanceZSensorToTop);
	//	m_dDistanceTopToTHR = iniMotionPara.GetDouble(strKey, "TOP Camera to THR Camera Distance (mm)", m_dDistanceTopToTHR);
	//}

}

int MotionParameters::getTotalDeviceCount() const
{
	return (m_nNoOfBlock[0] * m_nNoOfBlock[1]);
}

//int MotionParameters::GetTotalFOVPerDie()
//{
//	return nFOVPerDieinXTHR * nFOVPerDieinYTHR;
//}

int MotionParameters::getMotorStartScanPosition(int nAxis, int &nPos)
{
	if (nAxis < 0 || nAxis >= 4)
		return -1;

	nPos = motorStartScanPos[nAxis];

	return 0;
}

int MotionParameters::setStartScanFOVPosition(int nRowNo, int nColumnNo)
{
	int nDirectionX = -1;
	int nDirectionY = -1;

	motorStartScanPos[0] = motorStartScanPos[0] - Round(nDirectionX    * nColumnNo * m_dDevicePitch[0] * motorEngine[X_SERVO_MOTOR].getResolution());
	motorStartScanPos[1] = motorStartScanPos[1] - Round(nDirectionY    * nRowNo * m_dDevicePitch[1] * motorEngine[Y_SERVO_MOTOR].getResolution());

	OutputDebugLogTo(7, TRUE, "Start Pos Row %d, Col %d, X Position %d, Y Position %d", nRowNo, nColumnNo, motorStartScanPos[0], motorStartScanPos[1]);
	return 0;

}


int MotionParameters::getTotalFovPerDevice() const
{
	return (m_nDeviceNoPerBlock[0] * m_nDeviceNoPerBlock[1]);
}

//int MotionParameters::GetTotalFOVPerDie()
//{
//	return (m_nDeviceNoPerBlock[0] * m_nDeviceNoPerBlock[1]);
//}

int MotionParameters::getTotalFovPerBlock() const
{
	return (m_nNoOfBlock[0] * m_nNoOfBlock[1] * m_nDeviceNoPerBlock[0] * m_nDeviceNoPerBlock[1]);
}

int MotionParameters::getDieMappingFovNo(int nMapPos, int &nFovPos)
{
	if (nMapPos < 0 || nMapPos >= getTotalFovPerBlock())
		return -1;

	nFovPos = vDieFovNo[nMapPos];
	return 0;
}

int MotionParameters::getDieMappingPos(int nFovPos, int &nDiePos)
{
	if (nFovPos < 0 || nFovPos >= getTotalFovPerBlock())
		return -1;

	nDiePos = vDieMapNo[nFovPos];
	return 0;
}

int MotionParameters::GetDieXYOffset(int nColumnNo, int nRowNo, int & nXOffset, int & nYOffset) const
{
	if (nColumnNo < 0 || nColumnNo >= nColumnNoPerDie || nRowNo < 0 || nRowNo >= nRowNoPerDie)
		return -1;

	nXOffset = RelativeXPosition[nRowNo*nColumnNoPerDie + nColumnNo];
	nYOffset = RelativeYPosition[nRowNo*nColumnNoPerDie + nColumnNo];

	return 0;
}


int MotionParameters::GetDeviceXYOffset(int nColumnNo, int nRowNo, int & nXOffset, int & nYOffset) const
{
	if (nColumnNo < 0 || nColumnNo >= m_nNoOfBlock[0] || nRowNo < 0 || nRowNo >= m_nNoOfBlock[1])
		return -1;

	nXOffset = RelativeXDevicePosition[nRowNo*m_nNoOfBlock[0] + nColumnNo];
	nYOffset = RelativeYDevicePosition[nRowNo*m_nNoOfBlock[0] + nColumnNo];

	return 0;
}

void MotionParameters::GetDieGrabNumber(int & nColumnNo, int & nRowNo) const
{
	nColumnNo = nColumnNoPerDie;
	nRowNo = nRowNoPerDie;
}


int MotionParameters::GetColumnNoPerDevice() const
{
	return nColumnNoPerDie;
}

int MotionParameters::GetRowNoPerDevice() const
{
	return nRowNoPerDie;
}

int MotionParameters::GetColumnNoPerBlock() const
{
	return m_nNoOfBlock[0];
}

int MotionParameters::GetRowNoPerBlock() const
{
	return m_nNoOfBlock[1];
}

void MotionParameters::GetViewID(int nTrackId, int nDocId, int & nViewId)
{
	int nTempDoc = 0;
	for (int i = 0; i <= nTrackId; i++) {
		for (int nDoc = 0; nDoc < pApp->m_pTracks[i].m_nTotalDocs; nDoc++) {
			if (nDoc == nDocId && nTrackId == i)
				break;
			nTempDoc++;
		}
	}

	nViewId = nTempDoc;
}

int MotionParameters::EnableGrab(int nTrack, int nDoc, int nFov, bool bEnable)
{
	if (nFov < 0 || nFov >= nColumnNoPerDie * nRowNoPerDie || nDoc < 0 || nDoc >= pApp->m_pTracks[nTrack].m_nTotalDocs)
		return -1;

	pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_bEnableGrab[nFov] = bEnable;

	return 0;
}

void MotionParameters::ClearEnableGrab()
{
	for (int i = 0; i < theApp.m_nTotalTracks; i++) {
		for (int nDoc = 0; nDoc < theApp.m_pTracks[i].m_nTotalDocs; nDoc++)
			std::fill(pApp->m_pTracks[i].m_pDoc[nDoc]->m_bEnableGrab.begin(), pApp->m_pTracks[i].m_pDoc[nDoc]->m_bEnableGrab.end(), false);
	}
}

bool MotionParameters::IsGrabEnabled(int nTrack, int nDoc, int nFoV)
{
	if (nFoV < 0 || nFoV >= m_nTotalNoOfFov)
		return -1;

	return pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_bEnableGrab[nFoV];

	return 0;
}

int MotionParameters::getImageMappingDetails(int unitId, int &nRowId, int &nColId, int &nDeviceId, int &nFov)
{
	if (unitId < 0 || unitId >= m_nTotalGrabNo)
		return -1;

	int nRow, nCol;
	int nDeviceRow;
	int nFOV;


	nRow = unitId / (m_nDeviceNoPerBlock[0] * m_nNoOfBlock[0]);
	nCol = unitId % (m_nDeviceNoPerBlock[0] * m_nNoOfBlock[0]);

	nRowId = nRow % m_nDeviceNoPerBlock[1];
	nColId = nCol % m_nDeviceNoPerBlock[0];

	nFOV = nRowId * m_nDeviceNoPerBlock[0] + nColId;
	getDieMappingFovNo(nFOV, nFov);

	nDeviceId = nCol / m_nDeviceNoPerBlock[0] + (nRow / m_nDeviceNoPerBlock[1]) * m_nNoOfBlock[0];
	//	nDieId = nDeviceRow * m_nNoOfBlock[0] + nCol / nFOVPerDieinXTHR;
//	nDieId = (nDeviceRow % 2 == 0) ? (nCol / nFOVPerDieinXTHR * 2 + 1) : (nCol / nFOVPerDieinXTHR * 2);

	return 0;
}


int MotionParameters::getImageMappingDetails(int nDeviceId, int nFov, int & unitId)
{
	if (nDeviceId < 0 || nDeviceId >= m_nTotalNoDevice || nFov < 0 || nFov >= m_nTotalNoOfFov)
		return -1;

	int nDieID;
	getDieMappingPos(nFov, nDieID);

	int nDieCol = nDieID % nColumnNoPerDie;
	int nDieRow = nDieID / nColumnNoPerDie;

	int nDeviceCol = nDeviceId % m_nNoOfBlock[0];
	int nDeviceRow = nDeviceId / m_nNoOfBlock[0];

	int unitRow = nDeviceRow*m_nNoOfBlock[0] + nDeviceCol;

	unitId = (nDeviceRow *  /*nColumnNoPerDie **/ m_nDeviceNoPerBlock[0] * m_nDeviceNoPerBlock[1] ) +  nDieRow * m_nNoOfBlock[0] * m_nDeviceNoPerBlock[0] + nDeviceCol * m_nDeviceNoPerBlock[0] + nDieCol;

	return 0;

}


int MotionParameters::getDieScanOrder(int unitId, int & unitInOrder)
{
	if (unitId < 0 || unitId >= m_nTotalNoOfFov)
		return -1;

	unitInOrder = vScanDieOrder[unitId];

	return 0;
}

int MotionParameters::setDieStatus(int nDieId, BOOL bEnable)
{
	if (nDieId < 0 || nDieId >= m_nTotalNoOfFov)
		return -1;

	bDieEnabled[nDieId] = bEnable;


	return 0;
}

int MotionParameters::getDieStatus(int nDieId, BOOL & bEnable) const
{
	if (nDieId < 0 || nDieId >= m_nTotalNoOfFov)
		return -1;

	bEnable = bDieEnabled[nDieId];

	return 0;
}

int MotionParameters::setDeviceStatus(int nDeviceId, BOOL bEnable)
{
	if (nDeviceId < 0 || nDeviceId >= m_nTotalNoDevice)
		return -1;

	bDeviceEnabled[nDeviceId] = bEnable;
	
	return 0;
}

int MotionParameters::getDeviceStatus(int nDeviceId, BOOL & bEnable) const
{
	if (nDeviceId < 0 || nDeviceId >= m_nTotalNoDevice)
		return -1;

	bEnable = bDeviceEnabled[nDeviceId];
	return 0;
}


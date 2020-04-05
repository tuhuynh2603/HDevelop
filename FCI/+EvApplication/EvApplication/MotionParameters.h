#pragma once

#include <vector>
#include "boost\array.hpp"
//#include "Application.h"
#include "motion_information.h"

class MotorInterface;

#define MAX_DIRECTION 2
//------ USER DEFINITIONS FOR MOTORS
enum Motors
{
	X_SERVO_MOTOR,
	Y_SERVO_MOTOR,
	Z_SERVO_TOP_MOTOR,
	Z_SERVO_BOTTOM_MOTOR,
};

class MotionParameters
{

public:
	
	MotionParameters();
	MotionParameters(const MotionParameters &motionParameters);
	~MotionParameters();
	CApplication* pApp;
	//Pass necessary InspectionParameter to Motion Movement class
	void SetMotionMovementParameters(const MotionParameters & motionParameters);
	const MotionParameters & operator=(const MotionParameters & motionParameters);

	void InitMotionCard();
	void FreeMotionCard();

	int SetServoOn(int nAxisNo);
	int SetServoOff(int nAxisNo);
	int StopAxis(int nAxisNo);
	int EmergencyStop(int nAxisNo);
	int ResetAlarm(int nAxisNo);
	int HomeMove(int nAxisNo, bool bSafety = true, bool bHomeByPass = false);
	int HomeDone(int nAxisNo);
	int AbsoluteMove(int nAxisNo, int nPos, Speed speed, bool bSafety = true, bool bHomeByPass = false);
	int AbsoluteLinearMove(int nDimension, int* pAxisIDArray, int* pPositionArray, Speed speed, bool bSafety, bool bHomeByPass = false);
	int RelativeLinearMove(int nDimension, int* pAxisIDArray, int* pPositionArray, Speed speed, bool bSafety, bool bHomeByPass = false);
	int RelativeMove(int nAxisNo, int nPos, Speed speed, bool bSafety = true, bool bHomeByPass = false);
	int ChangeSpeedOnFly(int nAxisNo, int nNewMaxVel);
	int ContinuousMove(int nAxisNo, BOOL bForward, Speed speed, bool bSafety = true, bool bHomeByPass = false);

	int WaitForStartPositionDone(int nTimeOut);
	int WaitForMotionDone(int nAxisNo, int nTimeOut);

	int IsMotionDone(int nAxisNo, BOOL & bDone);

	int getMotorStatus(int axisId, long & status);

	int GetCurrentMotorPosition(int nAxisNo, int & nPos);
	BOOL IsMotorActive(int nAxisNo);
	BOOL IsMotorHomed(int nAxisNo) const;
	BOOL IsMotorLimit(int nAxisNo);
	int GetMotorAlarmStatus(int nAxisNo, BOOL &bOn);
	int GetMotorIOStatus(int nAxisNo, long & nStatus);

	void SetMotorHomed(int nAxisNo, BOOL bHomeStatus);
	int GetMotorEZStatus(int nAxisNo, BOOL & bStatus);

	int GetMotorResolution(int nAxisNo, double & dResolution) const;
	int SetMotorResolution(int nAxisNo, double dResolution);

	int GetMotorSettlingTime(int nAxisNo, int & nTime) const;
	int SetMotorSettlingTime(int nAxisNo, int nTime);

	Speed getHomingSpeed(int axisId) const;
	Speed getScanningSpeed(int axisId) const;
	Speed getScanningSpeedTHR(int axisId) const;
	Speed getMovingSpeed(int axisId) const;

	int setHomingSpeed(int axisId, Speed speed);
	int setScanningSpeed(int axisId, Speed speed);		//Top speed
//	int setScanningSpeedTHR(int axisId, Speed speed);
	int setMovingSpeed(int axisId, Speed speed);

	bool checkSoftLimitSetStatus();

	int GetMotorHomeOriginOffset(int nAxisNo, int & nOffset) const;
	int SetMotorHomeOriginOffset(int nAxisNo, int nOffset);

	int GetMotorHomeDirection(int nAxisNo, BOOL & bForward) const;
	int SetMotorHomeDirection(int nAxisNo, BOOL bForward);

	int GetMotorMovementDirection(int nAxisNo, BOOL & bForward) const;
	int SetMotorMovementDirection(int nAxisNo, BOOL bForward);

	int GetMotorMaximumSpeed(int nAxisNo, double & dSpeed) const;

	void WaitMotorSettlingTime(int nAxisNo) const;

	int  GetMotorName(int nMotorID, CString & strName) const;

	// Multiple motor axis interraction
	int GetTotalMotorCount() const;
	int SetServoOnAllAxis();
	int SetServoOffAllAxis();
	int StopEveryAxis();
	int EmergencyStopEveryAxis();
	int ResetAlarmAllAxis();
	int HomeMoveEveryAxis(bool bSafety = true);
	int WaitForAnyMotionError(std::vector < int > axisId, int nTimeOut);
	void WaitForEveryMotionDone(std::vector < int > error, int nTimeOut = 1000);

	int MoveToStartScanPos(bool bSafety = true);
	int MoveXYToPosition(int nDevice, int nFOV, bool bSaftey = true);
	int moveToZTopStartScanPosPos(bool bSafety = true);
	int moveToZBottomStartScanPosPos(bool bSafety = true);

	void SetStartScanPos();
	void SetZTopStartScanPos();
	void SetZBottomStartScanPos();

	int moveToReadyScanPos(bool bSafety = true);

	BOOL IsMachineHomed() const;

	int SetAlarm();

	void GetMappingParameters(int & nBlockX, int & nBlockY, double & dBlockPitchX, double & dBlockPitchY, int & nDieX, int & nDieY,
		double & dDiePitchX, double & dDiePitchY, int & nDieFOVX, int & nDieFOVY) const;

	void GetMappingParameters(int & nBlockX, int & nBlockY, int & nDieX, int & nDieY) const;

	void GetMappingParameters(int & nDieFOVX, int & nDieFOVY) const;

	int SetMappingParameters(int nBlockX, int nBlockY, double dBlockPitchX, double dBlockPitchY, int nDieX, int nDieY,
		double dDiePitchX, double dDiePitchY, int nDieFOVX, int nDieFOVY);

	int SetMappingParameters(int nBlockX, int nBlockY, int nDieX, int nDieY, BOOL bHorizontal);

	void getDeviceSize(double & dWidthMM, double & dHeightMM) const;
	void setDeviceSize(double dWidthMM, double dHeightMM);
	void getDevicePitch(double & dWidthMM, double & dHeightMM) const;
	void setDevicePitch(double dWidthMM, double dHeightMM);
	void getDeviceScanArea(double & dScanAreaX, double & dScanAreaY) const;
	void setDeviceScanArea(double dScanAreaX, double dScanAreaY);
	void getDeviceOverlap(double & dScanOverlapX, double & dScanOverlapY) const;
	void setDeviceOverlap(double dScanOverlapX, double dScanOverlapY);
	void setDeviceDirection(BOOL bHorizontal);
	void getDeviceDirection(BOOL &bHorizontal) const;

	int SetSubstrateSelected(int nColumn, int nRow);				//Girish
	int GetSubstrateSelected(int & nColumn, int & nRow);

	int m_nSubstrateDieColumn;						//Girish
	int m_nSubstrateDieRow;
	void SaveScanningOptions();
	void loadScanningOptions();

	void SaveScanningParm(bool saveMappingData = true) const;
	void LoadScanningParm();

	void SaveDeviceParameters() const;
	void LoadDeviceParameters();

	void SaveMotionParm() const;
	void LoadMotionParm();

	int CreateScanPositions();
	int getMotorStartScanPosition(int nAxis, int &nPos);
	int setStartScanFOVPosition(int nRowNo, int nColumnNo);

	int MoveZMotorToReadyPos(Speed speed = Speed(0, 0));

	int setInspectionEnable(int nCameraGrabId, bool bEnable);
	int getInspectionEnableStatus(int nCameraGrabId, bool &bEnable);

	int CheckSafety(int nAxisNo, bool bSafety, bool bHomeByPass = false);
	int CheckSoftwareLimits(int nAxisNo, int nPos);
	bool IsSoftwareLimits(int nAxisNo, int nPos);

//	int GetTotalFOVPerDie();
	int getTotalFovPerDevice() const;
	int getTotalFovPerBlock() const;
	int getTotalDeviceCount() const;
	int getDieMappingFovNo(int nMapPos, int &nFovPos);
	int getDieMappingPos(int nFovPos, int &nDieNo);

	int GetDieXYOffset(int nColumnNo, int nRowNo, int & nXOffset, int & nYOffset) const;
	int GetDeviceXYOffset(int nColumnNo, int nRowNo, int & nXOffset, int & nYOffset) const;
	void GetDieGrabNumber(int & nColumnNo, int & nRowNo) const;
	void GetViewID(int nTrackId, int nDocId, int & nViewId);
	int EnableGrab(int nTrack, int nDoc, int nFoV, bool bEnable);
	void ClearEnableGrab();

	int GetColumnNoPerDevice() const;
	int GetRowNoPerDevice() const;

	int GetColumnNoPerBlock() const;
	int GetRowNoPerBlock() const;

	bool IsGrabEnabled(int nTrack, int nDoc, int nFoV);
	int getImageMappingDetails(int unitID, int &nRowId, int &nColId, int &nDeviceId, int &nFov);
	int getImageMappingDetails(int nDeviceId, int nFov, int &unitID);
	int getDieScanOrder(int unit, int & unitInOrder);
	int setDieStatus(int nDieId, BOOL bEnable);
	int getDieStatus(int nDieId, BOOL & bEnable) const;

	int setDeviceStatus(int nDeviceId, BOOL bEnable);
	int getDeviceStatus(int nDeviceId, BOOL & bEnable) const;

private:

	const static size_t maximumMotorCount = 4;

	// Hardware variables
	CRITICAL_SECTION CriticalSectionCommands;

	MotorInterface & motorResource;

// Motor variables
	int motorCount;

	boost::array < MotorEngine, maximumMotorCount > motorEngine;

	boost::array < Speed, maximumMotorCount > _motorMoveVelocity;
	boost::array < Speed, maximumMotorCount > _motorScanVelocity;
	boost::array < Speed, maximumMotorCount > _motorScanVelocityTHR;

//	int nTriggerInversePulse;
//	int nTriggerPulseWidth;

	boost::array <int, 4		> motorStartScanPos;		

	bool scanningReady;
	boost::array <double, 2> motorDisplacement;

	std::vector <int > motorScanPositionX;
	std::vector <int > motorScanPositionY;

	int m_nTotalNoOfFov;
	int m_nTotalNoDevice;
	int m_nTotalGrabNo;
	
	int m_nNoOfBlock[MAX_DIRECTION];
	double m_dBlockPitch[MAX_DIRECTION];
	int m_nDeviceNoPerBlock[MAX_DIRECTION];
	int m_nDeviceOffset[MAX_DIRECTION];
	double m_dInitialOffset;
public:
	double m_dDevicePitch[MAX_DIRECTION];
	double m_dDeviceScanArea[MAX_DIRECTION];
	double m_dDeviceOverlap[MAX_DIRECTION];

//	int nNumberofDiesPerRow[MAX_DEVICE_ROW];
	int nFOVPerDieinX;
	int nFOVPerDieinY;

private:
	int nDieColumn;
	int nDieRow;

	// Bond station die mapping
	int nColumnNoPerDie;
	int nRowNoPerDie;
	int nGrabNoPerDie;

	int nRowNoPerBlock;
	int nColumnNoPerBlock;

	std::vector < int > ZLevelNoPerPosition;
	std::vector < int > RelativeXPosition;
	std::vector < int > RelativeYPosition;
	std::vector < std::vector< int > > ZLevelIDPerGrab;


	std::vector < int > RelativeXDevicePosition;
	std::vector < int > RelativeYDevicePosition;

	// Device Size
	double dDeviceSizeInXMM;
	double dDeviceSizeInYMM;
	BOOL bHorizontalDevice;

	std::vector <BOOL > bDeviceEnabled;
	std::vector <BOOL > bDieEnabled;
 
	// Merging for 2-3D station
//	CRITICAL_SECTION CriticalSectionData;

//	MotorInterface  & motorResource;

	int nTotalImageFOV;

	int nNumberofDie; 
	std::vector <int > vDieFovNo;
	std::vector <int > vDieMapNo;
	std::vector <int > vScanDieOrder;

};

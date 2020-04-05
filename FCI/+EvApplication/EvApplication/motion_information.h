
#pragma once

#include "position_info.h"
//#include "inspectionParameter.h"

class DeviceData
{
public:
	DeviceData();

	void setName(std::string aName);

	std::string getName() const;

	bool isHomed() const;

	bool getHomedStatus() const;
	void setHomedStatus(bool set);

	void setResolution(double res);
	double getResolution() const;

	void setHomePosition(int pos);
	int getHomePosition() const;

	/*void _loadParameters(InspectionParameter & parameter);
	void _saveParameters(InspectionParameter & parameter) const;*/

protected:

	void _setHomedStatus(bool set);

private:
	std::string _name;

	bool _homed;

	double _resolution;

	int _homePosition;
};

class MotorData : public DeviceData
{
public:
	MotorData();

	void setAxisId(int id);
	int _getAxisId() const;

	void setSettlingTime(int time);
	int getSettlingTime() const;

	void setHomeOriginOffset(int offset);
	int getHomeOriginOffset() const;

	void setSpeedLimit(int limit);
	int getSpeedLimit() const;

	void setForwardMovement(bool forward);
	bool isForwardMovement() const;

	void setForwardHoming(bool forward);
	bool isForwardHoming() const;

	void setHomingSpeed(int start, int maximum);
	void setHomingSpeed(Speed speed);
	void getHomingSpeed(int & start, int & maximum) const;
	Speed getHomingSpeed() const;

	int getNegativeSoftLimit() const;
	int getPositiveSoftLimit() const;

	void setNegativeSoftLimit(int negative);
	void setPositiveSoftLimit(int positive);

	bool isSoftwareLimitSet() const;

	void setHomingMode(int mode);

	/*void _loadParameters(InspectionParameter & parameter);
	void _saveParameters(InspectionParameter & parameter) const;
*/

	int _getHomingMode() const;

	void _setStartHomingSpeed(int speed);
	int _getStartHomingSpeed() const;

	void _setMaximumHomingSpeed(int speed);
	int _getMaximumHomingSpeed() const;

private:
	int _axisId;
	int _homingMode;

	int _softLimit[2];

	bool _forwardMovement;
	bool _forwardHoming;
	
	int _homeOriginOffset;

	int _settlingTime;

	int _speedLimit;

	Speed _homingSpeed;
};

class MotorEngine : public MotorData
{
public:
	MotorEngine();

	int initialize();

	int free();

	int servoOn();
	int servoOff();

	int stop();
	int emergencyStop();

	int resetAlarm();

	int startHoming();
	int completeHoming();

	int absoluteMove(int pos, int startSpeed, int maximumSpeed);
//	int absoluteMove(FullPosition pos);
	int relativeMove(int pos, int startSpeed, int maximumSpeed);
//	int relativeMove(FullPosition pos);
	int continuousMove(int startSpeed, int maximumSpeed, bool forward);
	int continuousMove(Speed speed, bool forward);

	int waitMotionCompletion(int time);

	bool isMotionCompleted();

	int getMotorStatus(long & status);

	int getCurrentPosition(int & pos);

	bool isActive();
	bool isLimit();

	int getAlarmStatus(BOOL & alarm);
	int getIoStatus(long & status);

	int changeSpeed(int speed);

	int validatePosition(int pos) const;
	bool isSoftwareLimits(int pos) const;

};

//class ScaleData : public DeviceData
//{
//public:
//	ScaleData();
//
//	void setAxisId(short id);
//	short _getAxisId() const;
//
//	//void _loadParameters(InspectionParameter & parameter);
//	//void _saveParameters(InspectionParameter & parameter) const;
//
//private:
//	short _axisId;
//};

//class ScaleEngine : public ScaleData
//{
//public:
//	ScaleEngine();
//
//	int initialize(short latchChannelId, bool triggerOutOnRisingEdge, int pulseWidthNs, short counterSource);
//
//	int enableLatching();
//
//	int resetFifo();
//
//	int getCurrentPosition(int & pos);
//
//	void startHoming();
//	int completeHoming();
//
//	int resetTriggerCount();
//
//	int setTriggerTable(long * data, short size);
//
//	int getLatchValue(long & value, bool waitForInterrupt);
//
//	int manualTrigger();
//};
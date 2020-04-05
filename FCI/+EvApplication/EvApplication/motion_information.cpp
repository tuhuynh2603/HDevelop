#include "stdafx.h"
#include "operatorOverloading.h"
#include "motion_information.h"
//#include "descriptionOutput.h"
#include "Application.h"


//template <class T>
//void get_value( InspectionParameter & parameter, std::string sectionName, std::string itemName, T & value )
//{
//	value = parameter.get( sectionName, itemName, value );
//};

DeviceData::DeviceData()
{
	_name = "Unknown device";

	_homed = false;

	_resolution = 100;

	_homePosition = 0;
}

void DeviceData::setName(std::string aName)
{
	_name = aName;
}

std::string DeviceData::getName() const
{
	return _name;
}

bool DeviceData::isHomed() const
{
	return _homed;
}

void DeviceData::_setHomedStatus(bool set)
{
	_homed = set;
}

bool DeviceData::getHomedStatus() const
{
	return _homed;
}

void DeviceData::setHomedStatus(bool set)
{
	_homed = set;
}

void DeviceData::setResolution(double res)
{
	if( res > 0 )
		_resolution = res;
}

double DeviceData::getResolution() const
{
	return _resolution;
}

void DeviceData::setHomePosition(int pos)
{
	_homePosition = pos;
}

int DeviceData::getHomePosition() const
{
	return _homePosition;
}

//void DeviceData::_loadParameters(InspectionParameter & parameter)
//{
//	get_value( parameter, _name, "Resolution (pps/mm)", _resolution );
//	get_value( parameter, _name, "Home position (pps)", _homePosition );
//}
//
//void DeviceData::_saveParameters(InspectionParameter & parameter) const
//{
//	parameter.set( _name, "Resolution (pps/mm)", _resolution );
//	parameter.set( _name, "Home position (pps)", _homePosition );
//}


MotorData::MotorData()
{
	_axisId = 0;
	_homingMode = 0;

	// 0 - negative, 1 - positive
	_softLimit[0] = _softLimit[1] = 0;

	

	_forwardMovement = true;
	_forwardHoming = true;

	
	_homeOriginOffset = 1000;
	
	_settlingTime = 100;

	_speedLimit = 100;

	_homingSpeed.startSpeed = 1;
	_homingSpeed.maximumSpeed = 1000;
}

void MotorData::setAxisId(int id)
{
	if( id >= 0 )
		_axisId = id;
}

void MotorData::setSettlingTime(int time)
{
	if( time >= 0 )
		_settlingTime = time;
}

int MotorData::getSettlingTime() const
{
	return _settlingTime;
}

void MotorData::setHomingSpeed(int start, int maximum)
{
	if( start > 0 && maximum > 0 && maximum >= start ) {
		_homingSpeed.startSpeed = start;
		_homingSpeed.maximumSpeed = maximum;
	}
}

void MotorData::setHomingSpeed(Speed speed)
{
	if( speed.startSpeed > 0 && speed.maximumSpeed > 0 && speed.maximumSpeed >= speed.startSpeed )
		_homingSpeed = speed;
}

void MotorData::getHomingSpeed(int & start, int & maximum) const
{
	start = _homingSpeed.startSpeed;
	maximum = _homingSpeed.maximumSpeed;
}

Speed MotorData::getHomingSpeed() const
{
	return _homingSpeed;
}

void MotorData::setHomeOriginOffset(int offset)
{
	if( offset != 0 )
		_homeOriginOffset = offset;
}

int MotorData::getHomeOriginOffset() const
{
	return _homeOriginOffset;
}

void MotorData::setSpeedLimit(int limit)
{
	if( limit > 0 )
		_speedLimit = limit;
}

int MotorData::getSpeedLimit() const
{
	return _speedLimit;
}

int MotorData::_getAxisId() const
{
	return _axisId;
}

int MotorData::getNegativeSoftLimit() const
{
	return _softLimit[0];
}

int MotorData::getPositiveSoftLimit() const
{
	return _softLimit[1];
}

bool MotorData::isSoftwareLimitSet() const
{
	return getNegativeSoftLimit() != getPositiveSoftLimit();
}

void MotorData::setNegativeSoftLimit(int negative)
{
	_softLimit[0] = negative;
}

void MotorData::setPositiveSoftLimit(int positive)
{
	_softLimit[1] = positive;
}

void MotorData::setHomingMode(int mode)
{
	_homingMode = mode;
}

int MotorData::_getHomingMode() const
{
	return _homingMode;
}

void MotorData::_setStartHomingSpeed(int speed)
{
	if( speed > 0 )
		_homingSpeed.startSpeed = speed;
}

int MotorData::_getStartHomingSpeed() const
{
	return _homingSpeed.startSpeed;
}

void MotorData::_setMaximumHomingSpeed(int speed)
{
	if( speed > 0 )
		_homingSpeed.maximumSpeed = speed;
}

int MotorData::_getMaximumHomingSpeed() const
{
	return _homingSpeed.maximumSpeed;
}

void MotorData::setForwardMovement(bool forward)
{
	_forwardMovement = forward;
}

bool MotorData::isForwardMovement() const
{
	return _forwardMovement;
}

void MotorData::setForwardHoming(bool forward)
{
	_forwardHoming = forward;
}

bool MotorData::isForwardHoming() const
{
	return _forwardHoming;
}

//void MotorData::_loadParameters(InspectionParameter & parameter)
//{
//	DeviceData::_loadParameters( parameter );
//
//	get_value( parameter, getName(), "Axis ID", _axisId );
//	get_value( parameter, getName(), "Homing mode", _homingMode );
//	get_value( parameter, getName(), "Negative Soft Limit", _softLimit[0] );
//	get_value( parameter, getName(), "Positive Soft Limit", _softLimit[1] );
//	get_value( parameter, getName(), "Forward movement", _forwardMovement );
//	get_value( parameter, getName(), "Forward homing", _forwardHoming );
//	get_value( parameter, getName(), "Home origin offset (pps)", _homeOriginOffset );
//	get_value( parameter, getName(), "Settling time (ms)", _settlingTime );
//	get_value( parameter, getName(), "Speed limit (mm/sec)", _speedLimit );
//	get_value( parameter, getName(), "Start Homing speed (mm/sec)", _homingSpeed.startSpeed );
//	get_value( parameter, getName(), "Maximum Homing speed (mm/sec)", _homingSpeed.maximumSpeed );
//}
//
//void MotorData::_saveParameters(InspectionParameter & parameter) const
//{
//	DeviceData::_saveParameters( parameter );
//
//	parameter.set( getName(), "Axis ID", _axisId );
//	parameter.set( getName(), "Homing mode", _homingMode );
//	parameter.set( getName(), "Negative Soft Limit", _softLimit[0] );
//	parameter.set( getName(), "Positive Soft Limit", _softLimit[1] );
//	parameter.set( getName(), "Forward movement", _forwardMovement );
//	parameter.set( getName(), "Forward homing", _forwardHoming );
//	parameter.set( getName(), "Home origin offset (pps)", _homeOriginOffset );
//	parameter.set( getName(), "Settling time (ms)", _settlingTime );
//	parameter.set( getName(), "Speed limit (mm/sec)", _speedLimit );
//	parameter.set( getName(), "Start Homing speed (mm/sec)", _homingSpeed.startSpeed );
//	parameter.set( getName(), "Maximum Homing speed (mm/sec)", _homingSpeed.maximumSpeed );
//}


MotorEngine::MotorEngine()
{

}

int MotorEngine::initialize()
{
	return theApp.m_MotorResource.InitializeAxis( _getAxisId(), _getHomingMode() );
}

int MotorEngine::free()
{
	int error = theApp.m_MotorResource.FreeAxis( _getAxisId() );
	if( error )
		return error;
	
	Sleep(100);

	return 0;
}

int MotorEngine::servoOn()
{
	_setHomedStatus(false);

	return theApp.m_MotorResource.SetServoOn( _getAxisId(), 1 );
}

int MotorEngine::servoOff()
{
	_setHomedStatus(false);

	return theApp.m_MotorResource.SetServoOn( _getAxisId(), 0 );
}

int MotorEngine::stop()
{
	return theApp.m_MotorResource.StopAxis( _getAxisId(), 0 );
}

int MotorEngine::emergencyStop()
{
	return theApp.m_MotorResource.EmergencyStopAxis( _getAxisId() );
}

int MotorEngine::resetAlarm()
{
	return theApp.m_MotorResource.ResetAlarm( /*theApp.m_nMotorCardNo*/ 0, _getAxisId() );
}

int MotorEngine::startHoming()
{
//	theApp.m_MotorResource.ResetSoftLimits( _getAxisId() );

	_setHomedStatus(false);

	if( _getStartHomingSpeed() > _getMaximumHomingSpeed() ) {
		int max = _getMaximumHomingSpeed();
		_setMaximumHomingSpeed( _getStartHomingSpeed() );
		_setStartHomingSpeed( max );
	}

	if( abs(_getStartHomingSpeed()) > static_cast<int>( getSpeedLimit() * getResolution() ) )
		_setStartHomingSpeed( (_getStartHomingSpeed() < 0 ? -1 : 1) * static_cast<int>( getSpeedLimit() * getResolution() ) );

	if( abs(_getMaximumHomingSpeed()) > static_cast<int>( getSpeedLimit() * getResolution() ) )
		_setMaximumHomingSpeed( (_getMaximumHomingSpeed() < 0 ? -1 : 1) * static_cast<int>( getSpeedLimit() * getResolution() ) );

	return theApp.m_MotorResource.HomeMove( _getAxisId(), isForwardHoming() ? getHomeOriginOffset() : -getHomeOriginOffset(), _getStartHomingSpeed(), _getMaximumHomingSpeed(), 0, 0 );
}

int MotorEngine::completeHoming()
{
	int error = theApp.m_MotorResource.SetCurrentPosition( _getAxisId(), getHomePosition() );
	if( error )
		return error;

	OutputDebugLogTo(7, TRUE, "Home Position %d", getHomePosition());

	//if( getNegativeSoftLimit() != getPositiveSoftLimit() ) {
	//	error = theApp.m_MotorResource.SetSoftLimits( _getAxisId(), getNegativeSoftLimit(), getPositiveSoftLimit() );
	//	if( error )
	//		return error;
	//}

	int pos;
	getCurrentPosition(pos);

	long ioStatus = 0;

	/*if(getIoStatus(ioStatus))
		return false;
*/

	if(pos > -5 || pos < 5)
		_setHomedStatus(true);
	else
		_setHomedStatus(false);

	if (_getAxisId() == 1500) {
		int error = theApp.m_MotorResource.SetCurrentPosition(_getAxisId() + 3, getHomePosition());
		if (error)
			return error;
	}

	return 0;
}

int MotorEngine::absoluteMove(int pos, int startSpeed, int maximumSpeed)
{
	if( startSpeed > maximumSpeed )
		return -1;

	if( validatePosition( pos ) )
		return -1;

	if( abs(startSpeed) > static_cast<int>( getSpeedLimit() * getResolution() ) )
		startSpeed = (startSpeed < 0 ? -1 : 1) * static_cast<int>( getSpeedLimit() * getResolution() );

	if( abs(maximumSpeed) > static_cast<int>( getSpeedLimit() * getResolution() ) )
		maximumSpeed = (maximumSpeed < 0 ? -1 : 1) * static_cast<int>( getSpeedLimit() * getResolution() );

	return theApp.m_MotorResource.AbsoluteMove( _getAxisId(), pos, startSpeed, maximumSpeed );
}

//int MotorEngine::absoluteMove(FullPosition pos)
//{
//	return absoluteMove( pos.position, pos.startSpeed, pos.maximumSpeed );
//}

int MotorEngine::relativeMove(int pos, int startSpeed, int maximumSpeed)
{
	if( startSpeed > maximumSpeed )
		return -1;

	pos = isForwardMovement() ? pos : -pos;

	if( abs(startSpeed) > static_cast<int>( getSpeedLimit() * getResolution() ) )
		startSpeed = (startSpeed < 0 ? -1 : 1) * static_cast<int>( getSpeedLimit() * getResolution() );

	if( abs(maximumSpeed) > static_cast<int>( getSpeedLimit() * getResolution() ) )
		maximumSpeed = (maximumSpeed < 0 ? -1 : 1) * static_cast<int>( getSpeedLimit() * getResolution() );

	return theApp.m_MotorResource.RelativeMove( _getAxisId(), pos, startSpeed, maximumSpeed );
}

//int MotorEngine::relativeMove(FullPosition pos)
//{
//	return relativeMove( pos.position, pos.startSpeed, pos.maximumSpeed );
//}

int MotorEngine::continuousMove(int startSpeed, int maximumSpeed, bool forward)
{
	if( startSpeed > maximumSpeed )
		return -1;

	int direction = (isForwardMovement() == forward) ? 1 : -1;

	if( abs(startSpeed) > static_cast<int>( getSpeedLimit() * getResolution() ) )
		startSpeed = (startSpeed < 0 ? -1 : 1) * static_cast<int>( getSpeedLimit() * getResolution() );

	if( abs(maximumSpeed) > static_cast<int>( getSpeedLimit() * getResolution() ) )
		maximumSpeed = (maximumSpeed < 0 ? -1 : 1) * static_cast<int>( getSpeedLimit() * getResolution() );

	return theApp.m_MotorResource.ContinuousMove( _getAxisId(), startSpeed * direction, maximumSpeed * direction, 0 );
}

int MotorEngine::continuousMove(Speed speed, bool forward)
{
	return continuousMove( speed.startSpeed, speed.maximumSpeed, forward );
}

int MotorEngine::waitMotionCompletion(int time)
{
	return theApp.m_MotorResource.WaitForMotionDone( _getAxisId(), time);
}

bool MotorEngine::isMotionCompleted()
{
	long status = 0;

	int error = getMotorStatus( status );
	if( error )
		return false;

	return (status & 0x20) != 0;
}

int MotorEngine::getMotorStatus(long & status)
{
	return theApp.m_MotorResource.GetMotorStatus( _getAxisId(), status );
}

int MotorEngine::getCurrentPosition(int & pos)
{
	return theApp.m_MotorResource.GetCurrentPosition( _getAxisId(), &pos );
}

bool MotorEngine::isActive()
{
	long ioStatus = 0;

	if( getIoStatus( ioStatus ) )
		return false;

	return (ioStatus & (1 << 7)) != 0;
}

bool MotorEngine::isLimit()
{
	long ioStatus = 0;

	if( getIoStatus( ioStatus ) )
		return false;

	return (ioStatus & (1 << 1)) != 0 || (ioStatus & (1 << 2)) != 0;
}

int MotorEngine::getAlarmStatus(BOOL & alarm)
{
//	BOOL ioStatus = 0;
	BOOL bEbl;

	/*int nError =  theApp.m_MotorResource.GetAlarmStatus(_getAxisId(), alarm, bEbl);
	if(nError)
		return nError;*/

	long ioStatus;

	int error = getIoStatus( ioStatus );
	if( error )
		return error;

	alarm = (ioStatus & 1) != 0;

	return 0;
}

int MotorEngine::getIoStatus(long & status)
{
	return theApp.m_MotorResource.GetIOStatus( _getAxisId(), status );
}

int MotorEngine::changeSpeed(int speed)
{
	return theApp.m_MotorResource.SpeedOverride( _getAxisId(), speed );
}

int MotorEngine::validatePosition(int pos) const
{
	if( getNegativeSoftLimit() != getPositiveSoftLimit() ) {

		if(pos < getNegativeSoftLimit() || pos > getPositiveSoftLimit() ) {
			
			CString message;

			message.Format("Motor %s is trying move out of limits {%d   %d} to position %d", getName().c_str(), getNegativeSoftLimit(), getPositiveSoftLimit(), pos );

//			PostError(message);

			message.Format("You are trying to move %s to position %d what is out of soft limits {%d   %d}.\nDo you want to move motor?", getName().c_str(), pos, getNegativeSoftLimit(), getPositiveSoftLimit() );

			if(AfxMessageBox(message, MB_YESNO | MB_ICONQUESTION) != IDYES)
				return -1;
		}

	}

	return 0;
}

bool MotorEngine::isSoftwareLimits(int pos) const
{
	if (getNegativeSoftLimit() != getPositiveSoftLimit()) {
		if (pos < getNegativeSoftLimit() || pos > getPositiveSoftLimit()) {
			return true;
		}
	}

	return false;
}

//ScaleData::ScaleData()
//{
//	_axisId = 0;
//}
//
//void ScaleData::setAxisId(short id)
//{
//	if( id >= 0 )
//		_axisId = id;
//}
//
//short ScaleData::_getAxisId() const
//{
//	return _axisId;
//}

//void ScaleData::_loadParameters(InspectionParameter & parameter)
//{
//	DeviceData::_loadParameters( parameter );
//
//	get_value( parameter, getName(), "Axis ID", _axisId );
//}
//
//void ScaleData::_saveParameters(InspectionParameter & parameter) const
//{
//	DeviceData::_saveParameters( parameter );
//
//	parameter.set( getName(), "Axis ID", _axisId );
//}


//ScaleEngine::ScaleEngine()
//{
//
//}
//
//int ScaleEngine::initialize(short latchChannelId, bool triggerOutOnRisingEdge, int pulseWidthNs, short counterSource)
//{
//	return theApp.m_ScaleResource.InitializeScaleChannel( 0, _getAxisId(), latchChannelId, triggerOutOnRisingEdge, pulseWidthNs, counterSource );
//}
//
//int ScaleEngine::enableLatching()
//{
//	return theApp.m_ScaleResource.SetScaleLatch(0, _getAxisId(), false);
//}
//
//int ScaleEngine::resetFifo()
//{
//	return theApp.m_ScaleResource.ResetScaleFifo( 0, _getAxisId() );
//}
//
//int ScaleEngine::getCurrentPosition(int & pos)
//{
//	long realPosition = 0;
//
//	int error = theApp.m_ScaleResource.GetScaleEncoderValue( 0, _getAxisId(), &realPosition );
//	if( error )
//		return error;
//
//	pos = realPosition;
//
//	return 0;
//}
//
//void ScaleEngine::startHoming()
//{
//	_setHomedStatus( false );
//}
//
//int ScaleEngine::completeHoming()
//{
//	int error = theApp.m_ScaleResource.SetScaleEncoderValue( 0, _getAxisId(), getHomePosition() );
//	if( error )
//		return error;
//
//	int pos;
//	getCurrentPosition(pos);
//
//	if (pos > -5 || pos < 5)
//		_setHomedStatus(true);
//	else
//		_setHomedStatus(false);
//
//	return 0;
//}
//
//int ScaleEngine::resetTriggerCount()
//{
//	return theApp.m_ScaleResource.ResetScaleFifo(0, _getAxisId());
//}
//
//int ScaleEngine::setTriggerTable(long * data, short size)
//{
//	return theApp.m_ScaleResource.SetScaleFifoArray(0, _getAxisId(), data, size);
//}
//
//int ScaleEngine::getLatchValue(long & value, bool waitForInterrupt)
//{
//	return theApp.m_ScaleResource.GetScaleLatchValue(0, _getAxisId(), &value, waitForInterrupt);
//}
//
//int ScaleEngine::manualTrigger()
//{
//	return theApp.m_ScaleResource.manualTrigger( 0, _getAxisId() );
//}
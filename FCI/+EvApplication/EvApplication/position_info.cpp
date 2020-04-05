#include "stdafx.h"
#include "position_info.h"
#include "operatorOverloading.h"

//template <class T>
//void get_value( InspectionParameter & parameter, std::string sectionName, std::string itemName, T & value )
//{
//	value = parameter.get( sectionName, itemName, value );
//};


//void Position::_loadParameters(InspectionParameter & parameter, const std::string & sectionName, const std::string & motorName)
//{
//	std::string temp = motorName + " " + _positionName;
//
//	get_value( parameter, sectionName, temp + " (pps)", position );
//}
//
//void Position::_saveParameters(InspectionParameter & parameter, const std::string & sectionName, const std::string & motorName) const
//{
//	std::string temp = motorName + " " + _positionName;
//
//	parameter.set( sectionName, temp + " (pps)", position );
//}

void Position::_setPositionName(std::string name)
{
	_positionName = name;
}

Position & Position::operator= (const Position & pos )
{
	position = pos.position;

	return (*this);
}


//void Speed::_loadParameters(InspectionParameter & parameter, const std::string & sectionName, const std::string & motorName)
//{
//	std::string temp = motorName + " " + _positionName;
//
//	get_value( parameter, sectionName, temp + " Start Speed (pps / sec)", startSpeed );
//	get_value( parameter, sectionName, temp + " Maximum Speed (pps / sec)", maximumSpeed );
//}
//
//void Speed::_saveParameters(InspectionParameter & parameter, const std::string & sectionName, const std::string & motorName) const
//{
//	std::string temp = motorName + " " + _positionName;
//
//	parameter.set( sectionName, temp + " Start Speed (pps / sec)", startSpeed );
//	parameter.set( sectionName, temp + " Maximum Speed (pps / sec)", maximumSpeed );
//}

void Speed::_setPositionName(std::string name)
{
	_positionName = name;
}

Speed & Speed::operator= (const Speed & speed )
{
	startSpeed   = speed.startSpeed;
	maximumSpeed = speed.maximumSpeed;

	return (*this);
}

//void SpeedLimit::_loadParameters(InspectionParameter & parameter, const std::string & sectionName, const std::string & motorName)
//{
//	std::string temp = motorName + " " + _positionName;
//
//	get_value( parameter, sectionName, temp + " Speed Limit (mm / sec)", limit );
//}
//
//void SpeedLimit::_saveParameters(InspectionParameter & parameter, const std::string & sectionName, const std::string & motorName) const
//{
//	std::string temp = motorName + " " + _positionName;
//
//	parameter.set( sectionName, temp + " Speed Limit (mm / sec)", limit );
//}

void SpeedLimit::_setPositionName(std::string name)
{
	_positionName = name;
}

SpeedLimit & SpeedLimit::operator= (const SpeedLimit & speed )
{
	limit   = speed.limit;

	return (*this);
}
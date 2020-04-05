#pragma once
#include <string>

struct Position
{
	int position;

	Position() : position(0) { _positionName = "Unknown position"; };
	Position(int pos) : position(pos) { _positionName = "Unknown position"; };

	Position & operator= (const Position & pos );

	void _set( int pos ) { position = pos; };
	void _set( std::string name, int pos) { position = pos; _positionName = name; }

//	void _loadParameters(InspectionParameter & parameter, const std::string & sectionName, const std::string & motorName);
//	void _saveParameters(InspectionParameter & parameter, const std::string & sectionName, const std::string & motorName) const;

	void _setPositionName(std::string name);

protected:
	std::string _positionName;
};

struct Speed
{
	int startSpeed;
	int maximumSpeed;

	Speed() : startSpeed(0), maximumSpeed(0) { _positionName = "Unknown position"; };
	Speed(int start, int max) : startSpeed(start), maximumSpeed(max) { _positionName = "Unknown position"; };

	Speed & operator= (const Speed & speed );

	void _set( int start, int max ) { startSpeed = start; maximumSpeed = max; };
	void _set( std::string name, int start, int max ) { startSpeed = start; maximumSpeed = max; _positionName = name; };

//	void _loadParameters(InspectionParameter & parameter, const std::string & sectionName, const std::string & motorName);
//	void _saveParameters(InspectionParameter & parameter, const std::string & sectionName, const std::string & motorName) const;

	void _setPositionName(std::string name);

protected:
	std::string _positionName;
};

struct EmptyPosition
{
	EmptyPosition() { };
	EmptyPosition(int) { };

	Position & operator= (const EmptyPosition & pos );

	void _set( int ) { };
	void _set( std::string, int) { }

//	void _loadParameters(InspectionParameter &, const std::string &, const std::string &) { };
//	void _saveParameters(InspectionParameter &, const std::string &, const std::string &) const { };

	void _setPositionName(std::string) { };
};

struct SpeedLimit
{
	double limit;

	SpeedLimit() : limit(1) { _positionName = "Unknown position"; };
	SpeedLimit(int lim, int) : limit(lim) { _positionName = "Unknown position"; };

	SpeedLimit & operator= (const SpeedLimit & speed );

	void _set( int lm, int) { limit = lm; };
	void _set( std::string name, int lm, int) { limit = lm; _positionName = name; };

//	void _loadParameters(InspectionParameter & parameter, const std::string & sectionName, const std::string & motorName);
//	void _saveParameters(InspectionParameter & parameter, const std::string & sectionName, const std::string & motorName) const;

	void _setPositionName(std::string name);

protected:
	std::string _positionName;
};

template < typename positionT, typename speedT >
struct TemplatePosition : positionT, speedT
{
	TemplatePosition() { };
	TemplatePosition(int pos, int start, int max) : positionT( pos ), speedT( start, max ) { };

	TemplatePosition & operator= (const TemplatePosition & position )
	{
		positionT::operator=( position );
		speedT::operator=( position );

		return (*this);
	};

	void _set(int pos, int start, int max)
	{
		positionT::_set(pos);
		speedT::_set(start, max);
	};

	void _set(std::string name, int pos, int start, int max)
	{
		positionT::_set(name, pos);
		speedT::_set(name, start, max);
	};

	//void _loadParameters(InspectionParameter & parameter, const std::string & sectionName, const std::string & motorName)
	//{
	//
	//	positionT::_loadParameters(parameter, sectionName, motorName);
	//	speedT::_loadParameters(parameter, sectionName, motorName);

	//};

	/*void _saveParameters(InspectionParameter & parameter, const std::string & sectionName, const std::string & motorName) const
	{
		positionT::_saveParameters(parameter, sectionName, motorName);
		speedT::_saveParameters(parameter, sectionName, motorName);
	};*/

	void _setPositionName(std::string name)
	{
		positionT::_setPositionName( name );
		speedT::_setPositionName( name );
	};
};

typedef TemplatePosition < Position, Speed > FullPosition;
typedef TemplatePosition < EmptyPosition, SpeedLimit > PositionLimit;
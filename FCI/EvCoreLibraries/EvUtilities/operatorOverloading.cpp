#include "stdafx.h"
#include "operatorOverloading.h"

std::stringstream & operator << (std::stringstream & input, const CRect & value)
{
	input << value.left << " " << value.top << " " << value.right << " " << value.bottom;

	return input;
}

std::stringstream & operator >> (std::stringstream & input, CRect & value)
{
	input >> value.left >> value.top >> value.right >> value.bottom;

	return input;
}

std::stringstream & operator << (std::stringstream & input, CPoint & value)
{
	input << value.x << " " << value.y;

	return input;
}

std::stringstream & operator >> (std::stringstream & input, CPoint & value)
{
	input >> value.x >> value.y;

	return input;
}

std::stringstream & operator << (std::stringstream & input, const CPoint2D & value)
{

	input.precision(std::numeric_limits < double > ::digits10);

	input << value.x << " " << value.y;

	return input;
}

std::stringstream & operator >> (std::stringstream & input, CPoint2D & value)
{
	input >> value.x >> value.y;

	return input;
}

std::stringstream & operator << (std::stringstream & input, CString & value)
{
	std::string temp(value);

	input << temp;

	return input;
}

std::stringstream & operator >> (std::stringstream & input, CString & value)
{
	std::string temp;

	temp = input.str();

	value = temp.c_str();

	return input;
}


std::stringstream & operator << (std::stringstream & input, bool & value)
{
	input << value;

	return input;
}

std::stringstream & operator >> (std::stringstream & input, bool & value)
{
	bool temp = value;

	input >> temp;

	value = temp;

	return input;
}

//std::stringstream & operator << (std::stringstream & input, const Polygon2d & value)
//{
//	input << value.get().size() << " ";
//
//	for( auto data = value.get().cbegin(); data != value.get().cend(); ++data )
//		input << " " << data->x << " " << data->y;
//
//	return input;
//}
//
//std::stringstream & operator >> (std::stringstream & input, Polygon2d & value)
//{
//	size_t newSize = 0;
//
//	input >> newSize;
//
//	std::vector < CPoint2D > v ( newSize );
//
//	for( auto data = v.begin(); data != v.end(); ++data )
//		input >> data->x >> data->y;
//
//	value.set( v );
//
//	return input;
//}

template<>
std::string unstringify<std::string>(const std::string & value, const std::string & init)
{
	if( value.empty() )
		return init;
	else
		return value;
};

template<>
std::string stringify<std::string>(const std::string  & value)
{
    return value;
};
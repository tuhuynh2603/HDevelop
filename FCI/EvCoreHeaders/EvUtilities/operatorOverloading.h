#pragma once

#include <iostream>
#include <sstream>

#include "MathAlgo.h"

//--------------------
// Operators >> and << overaloading for user-defined types

std::stringstream & operator << (std::stringstream & input, const CRect & value);

std::stringstream & operator >> (std::stringstream & input, CRect & value);

std::stringstream & operator << (std::stringstream & input, CPoint & value);

std::stringstream & operator >> (std::stringstream & input, CPoint & value);

std::stringstream & operator << (std::stringstream & input, const CPoint2D & value);

std::stringstream & operator >> (std::stringstream & input, CPoint2D & value);

std::stringstream & operator << (std::stringstream & input, CString & value);

std::stringstream & operator >> (std::stringstream & input, CString & value);

std::stringstream & operator << (std::stringstream & input, bool & value);

std::stringstream & operator >> (std::stringstream & input, bool & value);

//std::stringstream & operator << (std::stringstream & input, const Polygon2d & value);
//
//std::stringstream & operator >> (std::stringstream & input, Polygon2d & value);

template <typename T>
std::string stringify(const T & value)
{
    std::stringstream input;
    input << value;

    return input.str();
};

template<>
std::string stringify<std::string>(const std::string  & value);

template <typename T>
T unstringify(const std::string & value, const T & init)
{
	std::stringstream input;
	input << value;

	if( !value.empty() ) {
		T temp(init);

		input >> temp;

		return temp;
	}
	else {
		return init;
	}
};

template<>
std::string unstringify<std::string>(const std::string & value, const std::string & init);

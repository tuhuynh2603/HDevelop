#pragma once

#include <vector>

template <typename T >
void resize2dVector( std::vector < std::vector < T > > & data, size_t size1D, size_t size2D)
{
	data.resize( size1D );

	for( auto iter = data.begin(); iter != data.end(); ++iter )
		iter->resize( size2D );
}

template <typename T, typename S >
void resize2dVector( std::vector < std::vector < T > > & data, size_t size1D, size_t size2D, S initValue)
{
	data.resize( size1D );

	for( auto iter = data.begin(); iter != data.end(); ++iter )
		iter->resize( size2D, initValue );
}

template <typename T >
void resize3dVector( std::vector < std::vector < std::vector < T > > > & data, size_t size1D, size_t size2D, size_t size3D)
{
	data.resize( size1D );

	for( auto iter = data.begin(); iter != data.end(); ++iter )
		resize2dVector( *iter, size2D, size3D );
}

template <typename T, typename S >
void resize3dVector( std::vector < std::vector < std::vector < T > > > & data, size_t size1D, size_t size2D, size_t size3D, S initValue)
{
	data.resize( size1D );

	for( auto iter = data.begin(); iter != data.end(); ++iter )
		resize2dVector( *iter, size2D, size3D, initValue );
}

template <typename T >
void resize4dVector( std::vector < std::vector < std::vector < std::vector < T > > > > & data, size_t size1D, size_t size2D, size_t size3D, size_t size4D)
{
	data.resize( size1D );

	for( auto iter = data.begin(); iter != data.end(); ++iter )
		resize3dVector( *iter, size2D, size3D, size4D );
}

template <typename T, typename S >
void resize4dVector( std::vector < std::vector < std::vector < std::vector < T > > > > & data, size_t size1D, size_t size2D, size_t size3D, size_t size4D, S initValue)
{
	data.resize( size1D );

	for( auto iter = data.begin(); iter != data.end(); ++iter )
		resize3dVector( *iter, size2D, size3D, size4D, initValue );
}

template <typename T, typename S >
void fill2dVector( std::vector < std::vector < T > > & data, S value)
{
	for( auto iter = data.begin(); iter != data.end(); ++iter )
		std::fill( iter->begin(), iter->end(), value );
}

template <typename T, typename S >
void fill3dVector( std::vector < std::vector < std::vector < T > > > & data, S value)
{
	for( auto iter = data.begin(); iter != data.end(); ++iter )
		fill2dVector( *iter, value );
}

template <typename T, typename S >
void fill4dVector( std::vector < std::vector < std::vector < T > > > & data, S value)
{
	for( auto iter = data.begin(); iter != data.end(); ++iter )
		fill3dVector( *iter, value );
}
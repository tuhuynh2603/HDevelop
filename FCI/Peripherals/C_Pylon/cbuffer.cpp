//-----------------------------------------------------------------------------
//  (c) 2008 by Basler Vision Technologies
//  Section:  Vision Components
//  Project:  Pylon
//-----------------------------------------------------------------------------
/**
  \file     cbuffer.cpp
  \brief    Implementation of the CBuffer class.
*/

#include "stdafx.h"
#include "cbuffer.h"

/////////////////////////////////////////////////////////////////////////////
// CBuffer


// Constructor allocates the image buffer
CGrabBuffer::CGrabBuffer (const size_t ImageSize, const SImageFormat &pixelFormat)
	: m_pBuffer( NULL ), 
	  m_ImageSize (ImageSize),
	  m_PixelFormat (pixelFormat)
{
    m_pBuffer = new uchar8_t[m_ImageSize];
    if (NULL == m_pBuffer)
    {
        GenICam::GenericException e ("Not enough memory to allocate image buffer", __FILE__,__LINE__);
        throw e;
    }
}

// Freeing the memory
CGrabBuffer::~CGrabBuffer ()
{
    if  (NULL != m_pBuffer)
        delete[] m_pBuffer;
}

void CGrabBuffer::ResetLinePitch ()
{
	m_PixelFormat.LinePitch = 0;
}

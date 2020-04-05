//-----------------------------------------------------------------------------
//  (c) 2008 by Basler Vision Technologies
//  Section:  Vision Components
//  Project:  Pylon
//-----------------------------------------------------------------------------
/**
  \file     cbuffer.h
  \brief    Interface of simple image buffer class
*/

#if !defined (__CBUFFER_H__)
#define __CBUFFER_H__

//#include <PylonIncludes.h>
#include <stdlib.h>

//using namespace Pylon;

class CGrabBuffer
{
public:
    CGrabBuffer (const size_t ImageSize, const SImageFormat &pixelFormat);
    ~CGrabBuffer ();
    uint8_t *GetBufferPointer (void) { return m_pBuffer; }
    StreamBufferHandle GetBufferHandle (void) { return m_hBuffer; }
    void SetBufferHandle (StreamBufferHandle hBuffer) { m_hBuffer = hBuffer; };
	const size_t GetSize () { return m_ImageSize; }
	const SImageFormat &GetFormat () { return m_PixelFormat; }
	void SetPixelType (PixelType pixelType) { m_PixelFormat.PixelFormat = pixelType; }
	void ResetLinePitch ();

protected:
    uint8_t *m_pBuffer;
    StreamBufferHandle m_hBuffer;
	const size_t m_ImageSize;
	SImageFormat m_PixelFormat;
};




#endif // __CBUFFER_H__
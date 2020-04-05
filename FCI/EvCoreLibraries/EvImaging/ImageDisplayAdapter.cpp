#include "stdafx.h"
#include "ImageDisplayAdapter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDisplayAdapter::CDisplayAdapter()
{
	m_bBinaryMode = false;
	m_nBinaryThreshold.m_nBinaryThresholdMin = 128;
	m_nBinaryThreshold.m_nBinaryThresholdMax = 255;
    m_ppvBits = NULL;
	m_pRGB = NULL;
}

CDisplayAdapter::~CDisplayAdapter()
{

}

void CDisplayAdapter::InitializeColorTable(RGBQUAD *RGB)
{
	m_pRGB = RGB;
}
BOOL CDisplayAdapter::Initialize(int nImgWidth, int nImgHeight, BOOL bColor)
{
	BYTE*				pbyteImg;
	int					nBytes;
	int					nColorTableEntries=bColor?0:256;
	LPBITMAPINFOHEADER	lpBMIH;
	LPBITMAPINFO		lpBMI;
	LPVOID				lpvColorTable;
	BYTE				byteValue=0;
	int					nPaddedWidth;


	nPaddedWidth = nImgWidth % 4;
	if (nPaddedWidth)
		nPaddedWidth = 4 - nPaddedWidth;

	nPaddedWidth = nImgWidth+nPaddedWidth;

	nBytes = nPaddedWidth * nImgHeight;
	pbyteImg = new BYTE[abs(nBytes)];

	lpBMIH = (LPBITMAPINFOHEADER) new 
		char[sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * nColorTableEntries)];

	lpBMI = (LPBITMAPINFO) lpBMIH;

	lpvColorTable = (LPBYTE) lpBMIH + sizeof(BITMAPINFOHEADER);


	lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	lpBMIH->biWidth = nImgWidth;
#ifdef DIB_TOPDOWN
	lpBMIH->biHeight = (-1L)*nImgHeight;
#endif
#ifndef DIB_TOPDOWN
	lpBMIH->biHeight = nImgHeight;
#endif
	lpBMIH->biPlanes = 1;
	lpBMIH->biBitCount = bColor? 24 : 8;
	lpBMIH->biCompression = BI_RGB;
	lpBMIH->biSizeImage = BytesPerLine(nImgWidth, 8) * nImgHeight;
	lpBMIH->biXPelsPerMeter = 0;
	lpBMIH->biYPelsPerMeter = 0;
	lpBMIH->biClrUsed = 0;
	lpBMIH->biClrImportant = 0;

	memset(lpvColorTable, 0, sizeof(RGBQUAD) * nColorTableEntries);
//	memset(lpvColorTable, 0, nSzRGBQUAD * nColorTableEntries);

	LPRGBQUAD pDibQuad = (LPRGBQUAD) lpvColorTable;

//	LPVOID lpLimit = (LPBYTE)lpvColorTable + (nSzRGBQUAD*nColorTableEntries-1);

	for(int i = 0; i < nColorTableEntries; i++) {
		
		pDibQuad->rgbRed = byteValue;
		pDibQuad->rgbGreen = byteValue;
		pDibQuad->rgbBlue = byteValue;
		byteValue++;
		pDibQuad++;
	}

	SetBitmap(lpBMI, pbyteImg);

	delete [] pbyteImg;
	delete [] lpBMIH;

	return TRUE;
}

void CDisplayAdapter::ChangeToBinaryPalette(WPARAM nThreshold)
{
    RGBQUAD *pRGB = new RGBQUAD[256];
	BYTE	byteValue = 0;
	struct Threshold {
		int nLow;
		int nHigh;
	};
	Threshold* nThresholdValue = (Threshold*)nThreshold;

	for(int i = 0; i < 256; i++) {
		if ((i<nThresholdValue->nLow)||(i>nThresholdValue->nHigh)) {
			pRGB[i].rgbRed = 0;
			pRGB[i].rgbGreen = 0;
			pRGB[i].rgbBlue = 0;
		}
		else {
			pRGB[i].rgbRed = 255;
			pRGB[i].rgbGreen = 255;
			pRGB[i].rgbBlue = 255;
		} 
	}

	SetColorTable(256, pRGB);
	m_bBinaryMode = true;
	m_nBinaryThreshold.m_nBinaryThresholdMin = nThresholdValue->nLow;
	m_nBinaryThreshold.m_nBinaryThresholdMax = nThresholdValue->nLow;

	delete [] pRGB;
}

void CDisplayAdapter::ChangeToGrayPalette()
{
    RGBQUAD *pRGB = new RGBQUAD[256];
	BYTE	byteValue = 0;

	for(int i = 0; i < 256; i++) {
		pRGB[i].rgbRed = byteValue;
		pRGB[i].rgbGreen = byteValue;
		pRGB[i].rgbBlue = byteValue;
		byteValue++;
	}

	SetColorTable(256, pRGB);
	m_bBinaryMode =false;

	delete [] pRGB;
}

void CDisplayAdapter::ChangeToColorPalette()
{
   
	SetColorTable(256, m_pRGB);
}

/// Udupa; March'2013; Bitmap Overlay drawing context
CDC* CDisplayAdapter::GetDrawContext()
{
	return GetMemoryDC(NULL, FALSE);
}

void CDisplayAdapter::ReleaseDrawContext()
{
	ReleaseMemoryDC();
}

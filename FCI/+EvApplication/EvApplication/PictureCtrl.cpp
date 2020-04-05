
#include "stdafx.h"
#include "PictureCtrl.h"
#include <GdiPlus.h>
#include "ImageBuffer.h"
#include "DebugLog.h"
#include <string>
#include <iostream>

using namespace Gdiplus;

//Macro to release COM Components

#ifdef SAFE_RELEASE
#undef SAFE_RELEASE
#endif
#define SAFE_RELEASE(x) do{\
							if((x) != NULL)\
							{\
								while((x)->Release() != 0);\
								(x) = NULL;\
							}\
						}while(0)

CPictureCtrl::CPictureCtrl()
	:CStatic()
	, m_pStream(NULL)
	, m_bIsPicLoaded(FALSE)
	, m_gdiplusToken(0)
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	
	m_bSelected = FALSE;
	m_strErrCode = "";
	m_pPen = new Pen(Color::Yellow, 2);
	m_font = new Gdiplus::Font(&FontFamily(L"Arial"), 10);
	m_brush = new LinearGradientBrush(Rect(0,0,100,100), Color::Green, Color::White, LinearGradientModeHorizontal);
	
	m_lpBMIH = NULL;
	m_lImage = NULL;
}

CPictureCtrl::~CPictureCtrl()
{
	FreeData();
	ReleaseMemory();

	delete m_pPen;
	delete m_font;
	delete m_brush;

	GdiplusShutdown(m_gdiplusToken);
}

BOOL CPictureCtrl::ReleaseMemory()
{
	if(m_lpBMIH != NULL)
		delete [] m_lpBMIH;
	if(m_lImage != NULL)
		delete m_lImage;

	return 0;
}

BOOL CPictureCtrl::LoadFromStream(IStream *piStream)
{
	SetLastError(ERROR_SUCCESS);

	FreeData();

	if(piStream == NULL){
		SetLastError(ERROR_INVALID_ADDRESS);
		return FALSE;
	}

	DWORD dwResult = CreateStreamOnHGlobal(NULL, TRUE, &m_pStream);
	if(dwResult != S_OK){
		SetLastError(dwResult);
		return FALSE;
	}

	LARGE_INTEGER lInt;
	lInt.QuadPart = 0;
	piStream->Seek(lInt, STREAM_SEEK_SET, NULL);

	STATSTG statSTG;
	dwResult = piStream->Stat(&statSTG, STATFLAG_DEFAULT);
	if(dwResult != S_OK){
		SetLastError(dwResult);
		//SAFE_RELEASE(m_pStream);
		SafeRelease(m_pStream);
		return FALSE;
	}

	piStream->CopyTo(m_pStream, statSTG.cbSize, NULL, NULL);

	m_bIsPicLoaded = TRUE;

	Invalidate();
	RedrawWindow();

	return TRUE;
}

BOOL CPictureCtrl::LoadFromStream(BYTE* pData, size_t nSize)
{
	SetLastError(ERROR_SUCCESS);
	FreeData();

	DWORD dwResult = CreateStreamOnHGlobal(NULL, TRUE, &m_pStream);
	if(dwResult != S_OK){
		SetLastError(dwResult);
		return FALSE;
	}
	dwResult = m_pStream->Write(pData, (ULONG)nSize, NULL);
	if(dwResult != S_OK){
		SetLastError(dwResult);
		//SAFE_RELEASE(m_pStream);
		SafeRelease(m_pStream);
		return FALSE;
	}

	m_bIsPicLoaded = TRUE;

	Invalidate();
	return TRUE;//

	RedrawWindow();

	return TRUE;
}

BOOL CPictureCtrl::LoadFromFile(CString &szFilePath)
{
	SetLastError(ERROR_SUCCESS);
	FreeData();

	DWORD dwResult = CreateStreamOnHGlobal(NULL, TRUE, &m_pStream);
	if(dwResult != S_OK){
		SetLastError(dwResult);
		return FALSE;
	}

	CFile cFile;
	CFileException cFileException;
	if(!cFile.Open(szFilePath, CStdioFile::modeRead | CStdioFile::typeBinary, &cFileException)){
		SetLastError(cFileException.m_lOsError);
		//SAFE_RELEASE(m_pStream);
		SafeRelease(m_pStream);
		return FALSE;
	}

	BYTE pBuffer[1024] = {0};
	while(UINT dwRead = cFile.Read(pBuffer, 1024)){
		dwResult = m_pStream->Write(pBuffer, dwRead, NULL);
		if(dwResult != S_OK){
			SetLastError(dwResult);
			//SAFE_RELEASE(m_pStream);
			SafeRelease(m_pStream);
			cFile.Close();
			return FALSE;
		}
	}

	cFile.Close();
	m_bIsPicLoaded = TRUE;

	//Invalidate();
	RedrawWindow();

	return TRUE;
}

BOOL CPictureCtrl::Load(CString &szFilePath)
{
	return LoadFromFile(szFilePath);
}

BOOL CPictureCtrl::Load(IStream* piStream)
{
	return LoadFromStream(piStream);
}

BOOL CPictureCtrl::Load(BYTE* pData, size_t nSize)
{
	return LoadFromStream(pData, nSize);
}


void CPictureCtrl::FreeData()
{
	m_bIsPicLoaded = FALSE;
	//SAFE_RELEASE(m_pStream);
	SafeRelease(m_pStream);
}

void CPictureCtrl::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();
	ModifyStyle(0, SS_OWNERDRAW);
}

void CPictureCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if(m_bIsPicLoaded){
		RECT rc;
		this->GetClientRect(&rc);

		Graphics graphics(lpDrawItemStruct->hDC);
		Image image(m_pStream);

		graphics.DrawImage(&image, (INT)rc.left, (INT)rc.top, (INT)(rc.right-rc.left), (INT)(rc.bottom-rc.top));
		if(m_bSelected) {
			graphics.DrawRectangle(m_pPen, 2, 2, rc.right-rc.left-4, rc.bottom-rc.top-4);
			/*std::wstring wideusername;
			for(int i=0; i<m_strErrCode.GetLength(); ++i)
				wideusername += wchar_t(m_strErrCode[i]);
			const wchar_t* charBuf = wideusername.c_str();

			graphics.DrawString(charBuf, wcslen(charBuf), m_font, PointF(0, 0), m_brush);*/
		}
		graphics.ReleaseHDC(lpDrawItemStruct->hDC);
	}
}

BOOL CPictureCtrl::OnEraseBkgnd(CDC *pDC)
{
	if(m_bIsPicLoaded){
		RECT rc;
		this->GetClientRect(&rc);

		Graphics graphics(pDC->GetSafeHdc());
		LARGE_INTEGER liSeekPos;
		liSeekPos.QuadPart = 0;
		m_pStream->Seek(liSeekPos, STREAM_SEEK_SET, NULL);
		Image image(m_pStream);
		graphics.DrawImage(&image, (INT)rc.left, (INT)rc.top, (INT)(rc.right-rc.left), (INT)(rc.bottom-rc.top));
		return TRUE;
	}
	else
		return CStatic::OnEraseBkgnd(pDC);
}

BOOL CPictureCtrl::ImportBuffer(CImageBuffer& bufImage, int nIndex)
{
	int     nColorTableEntries;
	DWORD    dwSizeImage;
	LPVOID    lpvColorTable = NULL;
	int     nSize;
	BYTE*   pDst;
	BYTE*    pSrc;
	int     nPitch;
	int     nWidth, nHeight;

	BITMAPFILEHEADER m_bmfh;

	nColorTableEntries = 256;
	m_bmfh.bfType = 0x4d42;  // 'BM'
	int nSizeHdr = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * nColorTableEntries;
	m_bmfh.bfSize = 0;
	m_bmfh.bfReserved1 = m_bmfh.bfReserved2 = 0;
	m_bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
		sizeof(RGBQUAD) * nColorTableEntries; 
	nSize = m_bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);

	if(m_lpBMIH == NULL)
		m_lpBMIH = (LPBITMAPINFOHEADER) new char[abs(nSize)];
	lpvColorTable = (LPBYTE) m_lpBMIH + sizeof(BITMAPINFOHEADER);
	dwSizeImage = bufImage.GetImgSize().cy*bufImage.GetPitch();

	m_lpBMIH->biSizeImage = 0; // dwSizeImage;
	m_lpBMIH->biWidth = bufImage.GetImgSize().cx;
	m_lpBMIH->biHeight = bufImage.GetImgSize().cy;
	m_lpBMIH->biBitCount = bufImage.GetBitCount();
	m_lpBMIH->biPlanes = 1;
	m_lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	m_lpBMIH->biXPelsPerMeter = 2925;
	m_lpBMIH->biYPelsPerMeter = 2925;
	m_lpBMIH->biCompression = BI_RGB;
	m_lpBMIH->biClrImportant = 0;
	m_lpBMIH->biClrUsed = 256;

	if(bufImage.GetAdapter()!=NULL){
		RGBQUAD *RGB = bufImage.GetAdapter()->GetColorPallete();
		MakeColorTable(lpvColorTable,RGB);
	}
	else
		MakeGrayColorTable(lpvColorTable);

	long lHeader = sizeof(BITMAPFILEHEADER);
	m_nImgSize = lHeader+nSizeHdr+dwSizeImage;

	if(m_lImage == NULL)
		m_lImage = (LPBYTE) new char[abs((int) m_nImgSize)];
	nHeight = bufImage.GetImgSize().cy;
	nWidth = bufImage.GetPitch();

	pDst = m_lImage;
	nPitch = bufImage.GetPitch();
	pSrc = bufImage.GetImgPtr() + nPitch*(nHeight-1);

	memcpy(pDst, &m_bmfh, sizeof(BITMAPFILEHEADER));
	pDst+=sizeof(BITMAPFILEHEADER);
	memcpy(pDst, m_lpBMIH, nSizeHdr);
	pDst+=nSizeHdr;
	for (int i=0; i<nHeight; i++, pDst+=nPitch, pSrc-=nPitch) { // Copy and flip
		memcpy(pDst, pSrc, nWidth);
	}

	LoadFromStream(m_lImage, m_nImgSize);

	return 0;
}

BOOL CPictureCtrl::DisplayBuffer()
{
	return 0;

}

void CPictureCtrl::MakeGrayColorTable(LPVOID lpvColorTable)
{
	int  nColorTableEntries = 256;
	BYTE byteValue=0;

	LPRGBQUAD pDibQuad = (LPRGBQUAD) lpvColorTable;
	for(int i = 0; i < nColorTableEntries; i++) {
		pDibQuad->rgbRed = byteValue;
		pDibQuad->rgbGreen = byteValue;
		pDibQuad->rgbBlue = byteValue;
		byteValue++;
		pDibQuad++;
	}
}

void CPictureCtrl::MakeColorTable(LPVOID lpvColorTable,RGBQUAD *RGB)
{
	int  nColorTableEntries = 256;
	BYTE byteValue=0;
	BYTE *byte = (BYTE*)RGB;

	LPRGBQUAD pDibQuad = (LPRGBQUAD) lpvColorTable;
	for(int i = 0; i < nColorTableEntries; i++) {
		pDibQuad->rgbRed = *byte++;
		pDibQuad->rgbGreen = *byte++;
		pDibQuad->rgbBlue = *byte++;
		pDibQuad->rgbReserved = 0;
		byteValue++;
		pDibQuad++;
		*byte++;
	} 
}

void CPictureCtrl::SafeRelease(IStream* m_pStream)
{
	do{
		if((m_pStream) != NULL)
		{
			//while((m_pStream)->Release() != 0);
			m_pStream->Release();
			(m_pStream) = NULL;
		}
	}while(0);
}
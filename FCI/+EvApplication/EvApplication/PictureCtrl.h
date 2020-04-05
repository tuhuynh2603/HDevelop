
#pragma once
#include "afxwin.h"

class CImageBuffer;
class CPictureCtrl :
	public CStatic
{
public:
	CPictureCtrl();
	~CPictureCtrl();

public:
//	BITMAPFILEHEADER m_bmfh;
	LPBITMAPINFOHEADER m_lpBMIH;
	BYTE* m_lImage;
	int m_nImageIndex;
	ULONG m_nImgSize;

	BOOL m_bSelected;
	CString m_strErrCode;
	Gdiplus::LinearGradientBrush* m_brush;

	BOOL ReleaseMemory();
	BOOL LoadFromFile(CString &szFilePath);
	BOOL LoadFromStream(IStream* piStream);
	BOOL LoadFromStream(BYTE* pData, size_t nSize);

	BOOL Load(CString &szFilePath);
	BOOL Load(IStream* piStream);
	BOOL Load(BYTE* pData, size_t nSize);

	BOOL ImportBuffer(CImageBuffer& bufImage, int nIndex);
	BOOL DisplayBuffer();
	void MakeGrayColorTable(LPVOID lpvColorTable);
	void MakeColorTable(LPVOID lpvColorTable,RGBQUAD *RGB);
	void FreeData();
	void SafeRelease(IStream* m_pStream);

protected:
	virtual void PreSubclassWindow();

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnEraseBkgnd(CDC* pDC);

private:
	IStream* m_pStream;
	BOOL m_bIsPicLoaded;
	ULONG_PTR m_gdiplusToken;
	Gdiplus::Pen* m_pPen;
	Gdiplus::Font* m_font;
	//Gdiplus::LinearGradientBrush* m_brush;
};

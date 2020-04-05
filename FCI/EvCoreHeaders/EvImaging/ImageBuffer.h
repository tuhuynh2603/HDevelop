#pragma once

#include "stdafx.h"
#include "ImageDisplayAdapter.h"

CSize AbsSize(CSize csSize);

class CImageBuffer {
public :
	CDisplayAdapter* GetAdapter();

	CImageBuffer();
	~CImageBuffer();
	int Allocate(CSize ImgSize, bool bDisplayEnabled = false, BOOL bColor = FALSE);	// Allocate Memory for Image Buffer
	int AllocImgIndirect(CSize ImgSize, BYTE* pImg, BOOL bColor=FALSE);
	int GetPixelVal(int x, int y, BYTE& byteVal);
	int GetPixelVal(double x, double y, BYTE& byteVal);
	int GetPixelVal(int x, int y, short int & nVal);
	int GetPixelRGB(int x, int y, BYTE& byteR, BYTE& byteG, BYTE& byteB);
	int GetPixelVal(int x, int y, double & dVal);
	int SetPixelVal(int x, int y, short int nVal, int nReserved);
	int SetPixelVal(int x, int y, double dVal, int nReserved);
	int SetPixelVal(int x, int y, BYTE byteVal);
	int GetPitch();
	int Free();					// Free Allocated Memory of Image Buffer
	BOOL IsAllocated();				// Check if Image Buffer is Allocated
	CSize GetImgSize();				// Get the Size of the Allocated Image Buffer
	BYTE* GetImgPtr();				// Get the Pointer to the Image Buffer Class
	void ClrImg(BYTE byteVal=0);	// Clear Image Buffer
	CDC* GetDrawContext();
	void ReleaseDrawContext();

	inline int GetBitCount() { return m_nBitCount; }
	inline BOOL IsColor() { return (m_nBitCount==24); }

private :
	CSize m_ImgSize;				// Size of the Image
	BYTE *m_pImg;					// Pointer to the Image
	BOOL m_bAllocated;				// Status of Allocation
	bool m_bDisplayEnabled;
	int m_nBitCount;

protected:
	CDisplayAdapter* m_pAdapter;
	bool m_bIndirect;
	int m_nPitch;
};

class CImgIntBuf {
public :
	CImgIntBuf();
	~CImgIntBuf();
	int Allocate(CSize ImgSize);
	int GetPixelVal(int x, int y, int& nVal);
	int GetPixelVal(CPoint* pptPos, int& nVal);
	int SetPixelVal(int x, int y, int nVal);
	int SetPixelVal(CPoint* pptPos, int nVal);
	int GetPitch();
	int Free();				
	BOOL IsAllocated();		
	CSize GetImgSize();		
	int* GetImgPtr();			
	void ClrImg(int nVal=0);
	int Convert(CImageBuffer* pImgBufWrap);
	int ConvertNormalize(CImageBuffer* pImgBufNormalize);


private :
	CSize m_ImgSize;		
	int *m_pImg;				
	BOOL m_bAllocated;		
protected:
	int m_nPitch;
};

class CImgShortIntBuf {
public :
	CImgShortIntBuf();
	~CImgShortIntBuf();
	int Allocate(CSize ImgSize);
	int GetPixelVal(int x, int y, short int& nVal);
	int GetPixelVal(CPoint* pptPos, short int& nVal);
	int SetPixelVal(int x, int y, short int nVal);
	int SetPixelVal(CPoint* pptPos, short int nVal);
	int GetPitch();
	int Free();				
	BOOL IsAllocated();		
	CSize GetImgSize();		
	short int* GetImgPtr();			
	void ClrImg(short int nVal=0);
	int Convert(CImageBuffer* pImgBufWrap);
	int ConvertNormalize(CImageBuffer* pImgBufNormalize);


private :
	CSize m_ImgSize;		
	short int *m_pImg;				
	BOOL m_bAllocated;		
protected:
	int m_nPitch;
};

class CImgDoubleBuf {
public :
	CImgDoubleBuf();
	~CImgDoubleBuf();
	int Allocate(CSize ImgSize);
	int GetPixelVal(int x, int y, double& nVal);
	int GetPixelVal(CPoint* pptPos, double& nVal);
	int SetPixelVal(int x, int y, double nVal);
	int SetPixelVal(CPoint* pptPos, double nVal);
	int GetPitch();
	int Free();				
	BOOL IsAllocated();		
	CSize GetImgSize();		
	double* GetImgPtr();			
	void ClrImg(double nVal=0);
	int Convert(CImageBuffer* pImgBufWrap);
	int ConvertNormalize(CImageBuffer* pImgBufNormalize);


private :
	CSize m_ImgSize;		
	double *m_pImg;				
	BOOL m_bAllocated;		
protected:
	int m_nPitch;
};

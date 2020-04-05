#include "stdafx.h"
#include "ImageBuffer.h"
#include "MathDef.h"
#include "DebugLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 2k * 2k
#define MAX_MEM_SIZE 8000000

CSize AbsSize(CSize csSize)
{
	CSize csAbs;
	csAbs = CSize(abs(csSize.cx), abs(csSize.cy));
	return csAbs;
}

CImageBuffer::CImageBuffer()
{
	m_pImg = 0;
	m_bAllocated = FALSE;
	m_bDisplayEnabled = FALSE;
	m_ImgSize = CSize(0,0);
	m_nPitch = 0;
	m_bIndirect = false;
	m_pAdapter = NULL;
	m_nBitCount = 0;
}

CImageBuffer::~CImageBuffer()
{
	Free();
}

int CImageBuffer::Allocate(CSize ImgSize, bool bDisplayEnabled, BOOL bColor)
{
	CSize absvalImgSize;
	absvalImgSize = CSize(abs(ImgSize.cx), abs(ImgSize.cy));
	ImgSize =  absvalImgSize;

	if((ImgSize.cx * ImgSize.cy) > MAX_MEM_SIZE) { 
		if (m_bAllocated) 
			Free(); 
		OutputDebugLogTo(9,TRUE,"Exceed Max Mem Size");
		return -1; 
	}
    if((ImgSize.cx < 0) || (ImgSize.cy < 0)) { 
		if (m_bAllocated) 
			Free(); 
		return -1; 
    } 
	if (m_bAllocated) {
		if((ImgSize == m_ImgSize) && (m_bDisplayEnabled == bDisplayEnabled) && (bColor == (m_nBitCount==24)))
			return 0;
		else
			Free();
	}

	m_bDisplayEnabled = bDisplayEnabled;
	int	nBytes = ImgSize.cx;
	if(bColor)
		nBytes *= 3;
	nBytes = nBytes % 4;
	if (nBytes > 0)
		nBytes = 4 - nBytes;
	m_nPitch = bColor ? ImgSize.cx*3 : ImgSize.cx;
	m_nBitCount = bColor ? 24:8;
	m_nPitch +=  nBytes; 
	m_ImgSize = ImgSize;

	if (bDisplayEnabled) {
		m_pAdapter = new CDisplayAdapter;
		m_pAdapter->Initialize(m_ImgSize.cx, m_ImgSize.cy, bColor);
		m_pImg = (BYTE *) m_pAdapter->GetDIBits();
		m_bIndirect = true;
	}
	else {
		m_pImg = (BYTE*) malloc(abs(m_nPitch*ImgSize.cy));
		m_pAdapter = NULL;
		m_bIndirect = false;
	}
	m_bAllocated = TRUE;

	return 0;
}

int CImageBuffer::Free()
{
	if(m_bAllocated) {
		if (!m_bIndirect)
			free(m_pImg);
		if (m_pAdapter) {
			delete m_pAdapter;
			m_pAdapter = NULL;
		}
		m_pImg = 0;
		m_bAllocated = FALSE;
		m_bDisplayEnabled = false;
		m_ImgSize = CSize(0,0);
	}
	return 0;
}

BOOL CImageBuffer::IsAllocated()
{
	return m_bAllocated;
}

CSize CImageBuffer::GetImgSize()
{
	return m_ImgSize;
}

BYTE* CImageBuffer::GetImgPtr()
{
	return m_pImg;
}

void CImageBuffer::ClrImg(BYTE byteVal)
{
	memset((void*)m_pImg,byteVal,m_nPitch*m_ImgSize.cy);
}

int CImageBuffer::GetPitch()
{
	return m_nPitch;
}

int CImageBuffer::AllocImgIndirect(CSize ImgSize, BYTE *pImg, BOOL bColor)
{
	int nBitCount = bColor ? 24 : 8;
	if(!m_bAllocated || (ImgSize != m_ImgSize) || (m_nBitCount != nBitCount)) {
		int nBytes = ImgSize.cx;
		if(bColor)
			nBytes *= 3;
		nBytes = nBytes % 4;
		if(nBytes > 0)
			nBytes = 4 - nBytes;
		m_nPitch = bColor ? ImgSize.cx*3 : ImgSize.cx;
		m_nBitCount = nBitCount;
		m_nPitch +=  nBytes; 
		m_ImgSize = ImgSize;
	}
	if(!m_bIndirect && m_bAllocated)
		free(m_pImg);
	m_pImg = pImg;
	m_bAllocated = TRUE;
	m_bIndirect = true;

	return 0;
}

CDisplayAdapter* CImageBuffer::GetAdapter()
{
	return m_pAdapter;
}

int CImageBuffer::GetPixelRGB(int x, int y, BYTE& byteR, BYTE& byteG, BYTE& byteB)
{
	if(!m_bAllocated || m_nBitCount!=24) {
		//LogMessage(0, "[CImageBuffer::GetPixelRGB] Invalid Image Buffer");
		return -1;
	}
	
	int nError = ValidatePoint(&CPoint(x,y), &m_ImgSize);
	if(nError == -1) {
		//LogMessage(0, "[CImageBuffer::GetPixelRGB] Image Coordinate InValid");
		return -1;
	}

	x *= 3;
	byteR = m_pImg[y * m_nPitch + x + 2];
	byteG = m_pImg[y * m_nPitch + x + 1];
	byteB = m_pImg[y * m_nPitch + x];

	return 0;
}

int CImageBuffer::GetPixelVal(int x, int y, BYTE& byteVal)
{
	int nError;
	if (!m_bAllocated) {
//		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(&CPoint(x,y), &m_ImgSize);
	if(nError == -1) {
//		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	byteVal = m_pImg[y * m_nPitch + x];
	return 0;
}

int CImageBuffer::GetPixelVal(int x, int y, short int & nVal)
{
	int nError;
	if (!m_bAllocated) {
//		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(&CPoint(x,y), &m_ImgSize);
	if(nError == -1) {
//		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	nVal = m_pImg[y * m_nPitch + x];
	return 0;
}

int CImageBuffer::GetPixelVal(int x, int y, double & dVal)
{
	int nError;
	if (!m_bAllocated) {
//		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(&CPoint(x,y), &m_ImgSize);
	if(nError == -1) {
//		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	dVal = m_pImg[y * m_nPitch + x];
	return 0;
}

int CImageBuffer::GetPixelVal(double x, double y, BYTE& byteVal)
{
	int nError;
	BYTE byte00, byte01, byte10, byte11;
	int nVal0;
	int nVal1;
	int nVal;
	int nx, ny;
	int nxAdd1, nyAdd1;
	int nDifXTimes10;
	int nDifYTimes10;
	if (!m_bAllocated) {
//		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nx = int(x);
	ny = int(y);
	nError = ValidatePoint(&CPoint(nx,ny), &m_ImgSize);
	if(nError == -1) {
//		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	nxAdd1 = nx + 1;
	nyAdd1 = ny + 1;
	nError = ValidatePoint(&CPoint(nxAdd1,nyAdd1), &m_ImgSize);
	if(nError == -1) {
//		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	byte00 = m_pImg[ny * m_nPitch + nx];
	byte01 = m_pImg[ny * m_nPitch + nxAdd1];
	byte10 = m_pImg[nyAdd1 * m_nPitch + nx];
	byte11 = m_pImg[nyAdd1 * m_nPitch + nxAdd1];
	nDifXTimes10 = int((x - nx) * 10 + 0.5);
	nDifYTimes10 = int((y - ny) * 10 + 0.5);
	nVal0 = byte00 * 10 + (byte01 - byte00) * nDifXTimes10;
	nVal1 = byte10 * 10 + (byte11 - byte10) * nDifXTimes10;
	nVal = (nVal0 * 10 + (nVal1 - nVal0) * nDifYTimes10 + 50)/100;
	byteVal = nVal;
	if(nVal > 255)
		byteVal = 255;
	if(nVal < 0)
		byteVal = 0;
	return 0;
}

int CImageBuffer::SetPixelVal(int x, int y, BYTE byteVal)
{
	int nError;
	if (!m_bAllocated) {
//		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(&CPoint(x,y), &m_ImgSize);
	if(nError == -1) {
//		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	m_pImg[y * m_nPitch + x] = byteVal;
	return 0;
}

int CImageBuffer::SetPixelVal(int x, int y, short int nVal, int nReserved)
{
	int nError;
	if (!m_bAllocated) {
//		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(&CPoint(x,y), &m_ImgSize);
	if(nError == -1) {
//		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	m_pImg[y * m_nPitch + x] = (unsigned char)nVal;
	return 0;
}

int CImageBuffer::SetPixelVal(int x, int y, double dVal, int nReserved)
{
	int nError;
	if (!m_bAllocated) {
//		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(&CPoint(x,y), &m_ImgSize);
	if(nError == -1) {
//		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	m_pImg[y * m_nPitch + x] = (unsigned char)dVal;
	return 0;
}


/// Udupa; March'2013; Bitmap Overlay drawing context
CDC* CImageBuffer::GetDrawContext()
{
	if(m_pAdapter)
		return m_pAdapter->GetDrawContext();

	return NULL;
}

void CImageBuffer::ReleaseDrawContext()
{
	if(m_pAdapter)
		m_pAdapter->ReleaseDrawContext();
}






CImgIntBuf::CImgIntBuf()
{
	m_pImg = 0;
	m_bAllocated = FALSE;
	m_ImgSize = CSize(0,0);
	m_nPitch = 0;
}

CImgIntBuf::~CImgIntBuf()
{
	Free();
}

int CImgIntBuf::Allocate(CSize ImgSize)
{
    if((ImgSize.cx < 0) || (ImgSize.cy < 0)) { 
		if (m_bAllocated) 
			Free(); 
		return -1; 
    } 
	if (m_bAllocated) {
		Free();
	}
	int	nBytes;
	nBytes = ImgSize.cx;
	nBytes = nBytes % 4;
	if (nBytes > 0)
		nBytes = 4 - nBytes;
	m_nPitch = ImgSize.cx + nBytes; 
	m_ImgSize = ImgSize;
	m_pImg = (int*) malloc(abs(m_nPitch*ImgSize.cy) * sizeof(int));
	m_bAllocated = TRUE;
	return 0;
}

int CImgIntBuf::Free()
{
	if(m_bAllocated) {
		free(m_pImg);
		m_pImg = 0;
		m_bAllocated = FALSE;
		m_ImgSize = CSize(0,0);
	}
	return 0;
}

BOOL CImgIntBuf::IsAllocated()
{
	return m_bAllocated;
}

CSize CImgIntBuf::GetImgSize()
{
	return m_ImgSize;
}

int* CImgIntBuf::GetImgPtr()
{
	return m_pImg;
}

void CImgIntBuf::ClrImg(int nVal)
{
	memset((void*)m_pImg,nVal,m_nPitch*m_ImgSize.cy*sizeof(int));
}

int CImgIntBuf::GetPitch()
{
	return m_nPitch;
}

int CImgIntBuf::GetPixelVal(int x, int y, int& nVal)
{
	int nError;
	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(&CPoint(x,y), &m_ImgSize);
	if(nError == -1) {
		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	nVal = m_pImg[y * m_nPitch + x];
	return 0;
}

int CImgIntBuf::GetPixelVal(CPoint* pptPos, int& nVal)
{
	int nError;
	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(pptPos, &m_ImgSize);
	if(nError == -1) {
		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	nVal = m_pImg[pptPos->y * m_nPitch + pptPos->x];
	return 0;
}


int CImgIntBuf::SetPixelVal(int x, int y, int nVal)
{
	int nError;
	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(&CPoint(x,y), &m_ImgSize);
	if(nError == -1) {
		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	m_pImg[y * m_nPitch + x] = nVal;
	return 0;
}

int CImgIntBuf::SetPixelVal(CPoint* pptPos, int nVal)
{
	int nError;
	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(pptPos, &m_ImgSize);
	if(nError == -1) {
		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	m_pImg[pptPos->y * m_nPitch + pptPos->x] = nVal;
	return 0;
}

int CImgIntBuf::Convert(CImageBuffer* pImgBufWrap)
{
	int i, j;
	long lRowIndex;
	BYTE* pImgBufWrapPtr;

	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	pImgBufWrap->Allocate(AbsSize(m_ImgSize));
	pImgBufWrapPtr = pImgBufWrap->GetImgPtr();
	lRowIndex = 0;
	for(i=0;i<m_ImgSize.cy;i++) {
		for(j=0;j<m_ImgSize.cx;j++) {
			if(m_pImg[lRowIndex+j] < 0)
				pImgBufWrapPtr[lRowIndex+j] = 0;
			else {
				pImgBufWrapPtr[lRowIndex+j] = 255 - m_pImg[lRowIndex+j] % 255;
			}
		}
		lRowIndex += m_nPitch;
	}
	return 0;
}

int CImgIntBuf::ConvertNormalize(CImageBuffer* pImgBufNormalize)
{
	int i, j;
	long lRowIndex;
	BYTE* pImgBufNormalizePtr;
	int nMinVal;
	int nMaxVal;
	int nMaxMinDif;

	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	pImgBufNormalize->Allocate(AbsSize(m_ImgSize));
	pImgBufNormalizePtr = pImgBufNormalize->GetImgPtr();
	nMaxVal = INT_MIN;
	nMinVal = INT_MAX;
	lRowIndex = 0;
	for(i=0;i<m_ImgSize.cy;i++) {
		for(j=0;j<m_ImgSize.cx;j++) {
			if(nMaxVal < m_pImg[lRowIndex+j])
				nMaxVal = m_pImg[lRowIndex+j];
			if(nMinVal > m_pImg[lRowIndex+j])
				nMinVal = m_pImg[lRowIndex+j];
		}
		lRowIndex += m_nPitch;
	}
	nMaxMinDif = nMaxVal - nMinVal;
	if(nMaxMinDif == 0)
		pImgBufNormalize->ClrImg(0);
	else {
		lRowIndex = 0;
		for(i=0;i<m_ImgSize.cy;i++) {
			for(j=0;j<m_ImgSize.cx;j++) {
				pImgBufNormalizePtr[lRowIndex+j] = (m_pImg[lRowIndex+j] - nMinVal) * 255/nMaxMinDif;
			}
			lRowIndex += m_nPitch;
		}
	}
	return 0;
}



CImgShortIntBuf::CImgShortIntBuf()
{
	m_pImg = 0;
	m_bAllocated = FALSE;
	m_ImgSize = CSize(0,0);
	m_nPitch = 0;
}

CImgShortIntBuf::~CImgShortIntBuf()
{
	Free();
}

int CImgShortIntBuf::Allocate(CSize ImgSize)
{
    if((ImgSize.cx < 0) || (ImgSize.cy < 0)) { 
		if (m_bAllocated) 
			Free(); 
		return -1; 
    } 
	if (m_bAllocated) {
		Free();
	}
	int	nBytes;
	nBytes = ImgSize.cx;
	nBytes = nBytes % 4;
	if (nBytes > 0)
		nBytes = 4 - nBytes;
	m_nPitch = ImgSize.cx + nBytes; 
	m_ImgSize = ImgSize;
	m_pImg = (short*) malloc(abs(m_nPitch*ImgSize.cy) * sizeof(short));
	m_bAllocated = TRUE;
	return 0;
}

int CImgShortIntBuf::Free()
{
	if(m_bAllocated) {
		free(m_pImg);
		m_pImg = 0;
		m_bAllocated = FALSE;
		m_ImgSize = CSize(0,0);
	}
	return 0;
}

BOOL CImgShortIntBuf::IsAllocated()
{
	return m_bAllocated;
}

CSize CImgShortIntBuf::GetImgSize()
{
	return m_ImgSize;
}

short int* CImgShortIntBuf::GetImgPtr()
{
	return m_pImg;
}

void CImgShortIntBuf::ClrImg(short int nVal)
{
	memset((void*)m_pImg,nVal,m_nPitch*m_ImgSize.cy*sizeof(short int));
}

int CImgShortIntBuf::GetPitch()
{
	return m_nPitch;
}

int CImgShortIntBuf::GetPixelVal(int x, int y, short int& nVal)
{
	int nError;
	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(&CPoint(x,y), &m_ImgSize);
	if(nError == -1) {
		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	nVal = m_pImg[y * m_nPitch + x];
	return 0;
}

int CImgShortIntBuf::GetPixelVal(CPoint* pptPos, short int& nVal)
{
	int nError;
	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(pptPos, &m_ImgSize);
	if(nError == -1) {
		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	nVal = m_pImg[pptPos->y * m_nPitch + pptPos->x];
	return 0;
}


int CImgShortIntBuf::SetPixelVal(int x, int y, short int nVal)
{
	int nError;
	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(&CPoint(x,y), &m_ImgSize);
	if(nError == -1) {
		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	m_pImg[y * m_nPitch + x] = nVal;
	return 0;
}

int CImgShortIntBuf::SetPixelVal(CPoint* pptPos, short int nVal)
{
	int nError;
	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(pptPos, &m_ImgSize);
	if(nError == -1) {
		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	m_pImg[pptPos->y * m_nPitch + pptPos->x] = nVal;
	return 0;
}

int CImgShortIntBuf::Convert(CImageBuffer* pImgBufWrap)
{
	int i, j;
	long lRowIndex;
	BYTE* pImgBufWrapPtr;

	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	pImgBufWrap->Allocate(AbsSize(m_ImgSize));
	pImgBufWrapPtr = pImgBufWrap->GetImgPtr();
	lRowIndex = 0;
	for(i=0;i<m_ImgSize.cy;i++) {
		for(j=0;j<m_ImgSize.cx;j++) {
			if(m_pImg[lRowIndex+j] < 0)
				pImgBufWrapPtr[lRowIndex+j] = 0;
			else {
				pImgBufWrapPtr[lRowIndex+j] = 255 - m_pImg[lRowIndex+j] % 255;
			}
		}
		lRowIndex += m_nPitch;
	}
	return 0;
}

int CImgShortIntBuf::ConvertNormalize(CImageBuffer* pImgBufNormalize)
{
	int i, j;
	long lRowIndex;
	BYTE* pImgBufNormalizePtr;
	int nMinVal;
	int nMaxVal;
	int nMaxMinDif;

	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	pImgBufNormalize->Allocate(AbsSize(m_ImgSize));
	pImgBufNormalizePtr = pImgBufNormalize->GetImgPtr();
	nMaxVal = INT_MIN;
	nMinVal = INT_MAX;
	lRowIndex = 0;
	for(i=0;i<m_ImgSize.cy;i++) {
		for(j=0;j<m_ImgSize.cx;j++) {
			if(nMaxVal < m_pImg[lRowIndex+j])
				nMaxVal = m_pImg[lRowIndex+j];
			if(nMinVal > m_pImg[lRowIndex+j])
				nMinVal = m_pImg[lRowIndex+j];
		}
		lRowIndex += m_nPitch;
	}
	nMaxMinDif = nMaxVal - nMinVal;
	if(nMaxMinDif == 0)
		pImgBufNormalize->ClrImg(0);
	else {
		lRowIndex = 0;
		for(i=0;i<m_ImgSize.cy;i++) {
			for(j=0;j<m_ImgSize.cx;j++) {
				pImgBufNormalizePtr[lRowIndex+j] = (m_pImg[lRowIndex+j] - nMinVal) * 255/nMaxMinDif;
			}
			lRowIndex += m_nPitch;
		}
	}
	return 0;
}





/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

CImgDoubleBuf::CImgDoubleBuf()
{
	m_pImg = 0;
	m_bAllocated = FALSE;
	m_ImgSize = CSize(0,0);
	m_nPitch = 0;
}

CImgDoubleBuf::~CImgDoubleBuf()
{
	Free();
}

int CImgDoubleBuf::Allocate(CSize ImgSize)
{
    if((ImgSize.cx < 0) || (ImgSize.cy < 0)) { 
		if (m_bAllocated) 
			Free(); 
		return -1; 
    } 
	if (m_bAllocated) {
		Free();
	}
	int	nBytes;
	nBytes = ImgSize.cx;
	nBytes = nBytes % 4;
	if (nBytes > 0)
		nBytes = 4 - nBytes;
	m_nPitch = ImgSize.cx + nBytes; 
	m_ImgSize = ImgSize;
	m_pImg = (double*) malloc(abs(m_nPitch*ImgSize.cy) * sizeof(double));
	m_bAllocated = TRUE;
	return 0;
}

int CImgDoubleBuf::Free()
{
	if(m_bAllocated) {
		free(m_pImg);
		m_pImg = 0;
		m_bAllocated = FALSE;
		m_ImgSize = CSize(0,0);
	}
	return 0;
}

BOOL CImgDoubleBuf::IsAllocated()
{
	return m_bAllocated;
}

CSize CImgDoubleBuf::GetImgSize()
{
	return m_ImgSize;
}

double* CImgDoubleBuf::GetImgPtr()
{
	return m_pImg;
}

void CImgDoubleBuf::ClrImg(double dVal)
{
	memset((void*)m_pImg,(int) dVal,m_nPitch*m_ImgSize.cy*sizeof(double));
}

int CImgDoubleBuf::GetPitch()
{
	return m_nPitch;
}

int CImgDoubleBuf::GetPixelVal(int x, int y, double& dVal)
{
	int nError;
	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(&CPoint(x,y), &m_ImgSize);
	if(nError == -1) {
		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	dVal = m_pImg[y * m_nPitch + x];
	return 0;
}

int CImgDoubleBuf::GetPixelVal(CPoint* pptPos, double& dVal)
{
	int nError;
	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(pptPos, &m_ImgSize);
	if(nError == -1) {
		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	dVal = m_pImg[pptPos->y * m_nPitch + pptPos->x];
	return 0;
}


int CImgDoubleBuf::SetPixelVal(int x, int y, double dVal)
{
	int nError;
	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(&CPoint(x,y), &m_ImgSize);
	if(nError == -1) {
		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	m_pImg[y * m_nPitch + x] = dVal;
	return 0;
}

int CImgDoubleBuf::SetPixelVal(CPoint* pptPos, double dVal)
{
	int nError;
	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	nError = ValidatePoint(pptPos, &m_ImgSize);
	if(nError == -1) {
		ASSERT(!"Image Coordinate InValid!!");
		return -1;
	}
	m_pImg[pptPos->y * m_nPitch + pptPos->x] = dVal;
	return 0;
}


int CImgDoubleBuf::Convert(CImageBuffer* pImgBufWrap)
{
/*	int i, j;
	long lRowIndex;
	BYTE* pImgBufWrapPtr;

	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	pImgBufWrap->Allocate(m_ImgSize);
	pImgBufWrapPtr = pImgBufWrap->GetImgPtr();
	lRowIndex = 0;
	for(i=0;i<m_ImgSize.cy;i++) {
		for(j=0;j<m_ImgSize.cx;j++) {
			if(m_pImg[lRowIndex+j] < 0)
				pImgBufWrapPtr[lRowIndex+j] = 0;
			else {
				pImgBufWrapPtr[lRowIndex+j] = 255 - m_pImg[lRowIndex+j] % 255;
			}
		}
		lRowIndex += m_nPitch;
	}*/
	return 0;
}

int CImgDoubleBuf::ConvertNormalize(CImageBuffer* pImgBufNormalize)
{
	int i, j;
	long lRowIndex;
	BYTE* pImgBufNormalizePtr;
	int nMinVal;
	int nMaxVal;
	int nMaxMinDif;

	if (!m_bAllocated) {
		ASSERT(!"Image Buffer Not Allocated!!");
		return -1;
	}
	pImgBufNormalize->Allocate(AbsSize(m_ImgSize));
	pImgBufNormalizePtr = pImgBufNormalize->GetImgPtr();
	nMaxVal = INT_MIN;
	nMinVal = INT_MAX;
	lRowIndex = 0;
	for(i=0;i<m_ImgSize.cy;i++) {
		for(j=0;j<m_ImgSize.cx;j++) {
			if(nMaxVal < m_pImg[lRowIndex+j])
				nMaxVal = (int) m_pImg[lRowIndex+j];
			if(nMinVal > m_pImg[lRowIndex+j])
				nMinVal = (int) m_pImg[lRowIndex+j];
		}
		lRowIndex += m_nPitch;
	}
	nMaxMinDif = nMaxVal - nMinVal;
	if(nMaxMinDif == 0)
		pImgBufNormalize->ClrImg(0);
	else {
		lRowIndex = 0;
		for(i=0;i<m_ImgSize.cy;i++) {
			for(j=0;j<m_ImgSize.cx;j++) {
				pImgBufNormalizePtr[lRowIndex+j] = (BYTE) ((m_pImg[lRowIndex+j] - nMinVal) * 255/nMaxMinDif);
			}
			lRowIndex += m_nPitch;
		}
	}
	return 0;
}
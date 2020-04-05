// BMPFunctions.cpp: implementation of the CBMPFunctions class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BitmapFunctions.h"
#include "MathValidations.h"
#include "ImageDef.h"
#include "ImageFunction.h"
#include "DebugLog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW	
#endif

int LoadGrayScaleBMPFile(CString strFileName, CImageBuffer& bufImage, BOOL bSameBitCount)
{
	int nCount, nSize;
	CFile   file;

	enum Alloc {noAlloc, crtAlloc, heapAlloc};

	LPVOID lpvColorTable = NULL;
    LPBYTE lpImage = NULL;		// starting address of DIB bits
	LPBITMAPINFOHEADER lpBMIH;	//  buffer containing the BITMAPINFOHEADER
	Alloc nBmihAlloc;
	Alloc nImageAlloc;
	DWORD dwSizeImage;			// of bits -- not BITMAPINFOHEADER or BITMAPFILEHEADER
	BYTE*	pSrcBak;
	BYTE*	pSrc;
	BYTE*	pDst;
	BYTE*	pDstBak;
	BYTE* byteValue;
	RGBQUAD *pRGB;

	int nWidth;
	int nHeight;
	int nPitchSrc, nPitchDst;

	BITMAPFILEHEADER bmfh;

	lpBMIH = NULL;
	lpImage = NULL;
	pRGB = NULL;

	if(!file.Open(strFileName, CFile::modeRead))
		goto ErrorAbort;
	
	try {
		nCount = file.Read((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));
		if(nCount != sizeof(BITMAPFILEHEADER)) {
			throw new CMemoryException;
		}
		if(bmfh.bfType != 0x4d42) {
			throw new CMemoryException;
		}
		nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		lpBMIH = (LPBITMAPINFOHEADER) new char[abs(nSize)];
		nBmihAlloc = nImageAlloc = crtAlloc;
		nCount = file.Read(lpBMIH, nSize); // info hdr & color table

		if(bSameBitCount && (lpBMIH->biBitCount != bufImage.GetBitCount()))
			goto ErrorIgnore;

		// Compute the metrics
		if(lpBMIH->biSize != sizeof(BITMAPINFOHEADER)) {
			TRACE("Not a valid Windows bitmap -- probably an OS/2 bitmap\n");
			throw new CMemoryException;
		}
		dwSizeImage = lpBMIH->biSizeImage;
		if(dwSizeImage == 0) {
			DWORD dwBytes = ((DWORD) lpBMIH->biWidth * lpBMIH->biBitCount) / 32;
			if(((DWORD) lpBMIH->biWidth * lpBMIH->biBitCount) % 32) {
				dwBytes++;
			}
			dwBytes *= 4;
			dwSizeImage = dwBytes * lpBMIH->biHeight; // no compression
		}
		lpvColorTable = (LPBYTE) lpBMIH + sizeof(BITMAPINFOHEADER);


		lpImage = (LPBYTE) new char[abs((int) dwSizeImage)];
		nCount = file.Read(lpImage, dwSizeImage); // image only

		byteValue = (BYTE*)lpvColorTable;

		pRGB = new RGBQUAD[256];


		// Copy the image bytes
		nWidth = (DWORD) lpBMIH->biWidth;
		nHeight = (DWORD) lpBMIH->biHeight;
		nPitchSrc = dwSizeImage/nHeight;

		pSrc = lpImage + nPitchSrc*(nHeight-1);

		if(!bufImage.IsAllocated())
			bufImage.Allocate(CSize(nWidth, nHeight), false, lpBMIH->biBitCount==24);
		nPitchDst = bufImage.GetPitch();
		nWidth = min(nPitchSrc, bufImage.GetPitch());
		nHeight = min(nHeight, bufImage.GetImgSize().cy);
		pDst = bufImage.GetImgPtr(); //  + nPitchDst*(nHeight-1);

		///Udupa; Feb'2011; Generic to grascale to color buffer load and vice versa
		if(lpBMIH->biBitCount == bufImage.GetBitCount()) {
			for (int i=0; i<nHeight; i++) {
				memcpy(pDst, pSrc, nWidth);
				pDst+=nPitchDst;
				pSrc-=nPitchSrc;
			}
		}
		else if((lpBMIH->biBitCount==24) && !bufImage.IsColor()) {
			for (int i=0; i<nHeight; i++) {
				pSrcBak = pSrc;
				pDstBak = pDst;
				for(int j=0; j<nWidth; j++) {
					*pDst = (*pSrc + *(pSrc+1) + *(pSrc+2))/3;
					pDst++;
					pSrc += 3;
				}
				pSrc = pSrcBak;
				pDst = pDstBak;

				pDst+=nPitchDst;
				pSrc-=nPitchSrc;
			}
		}
		else if((lpBMIH->biBitCount==8) && bufImage.IsColor()) {
			for (int i=0; i<nHeight; i++) {
				pSrcBak = pSrc;
				pDstBak = pDst;
				for(int j=0; j<nWidth; j++) {
					*pDst = *pSrc;
					pDst++;
					*pDst = *pSrc;
					pDst++;
					*pDst = *pSrc;
					pDst++;
					pSrc++;
				}
				pSrc = pSrcBak;
				pDst = pDstBak;

				pDst+=nPitchDst;
				pSrc-=nPitchSrc;
			}
		}


		if(bufImage.GetAdapter() != NULL) {
			for(int i = 0; i < 256; i++) {
				pRGB[i].rgbBlue = i;
				pRGB[i].rgbGreen = i;
				pRGB[i].rgbRed = i;
			}
			bufImage.GetAdapter()->SetColorTable(256, pRGB);
		}
	}


	catch(CException* pe) {
		pe->Delete();
		goto ErrorAbort;
	}
	if(lpBMIH != NULL)
		delete [] lpBMIH;
	if(lpImage != NULL)
		delete [] lpImage;
	if(pRGB  != NULL)
		delete [] pRGB;
	file.Close();
	return 0;
ErrorAbort:
	if(lpBMIH != NULL)
		delete [] lpBMIH;
	if(lpImage != NULL)
		delete [] lpImage;
	if(pRGB  != NULL)
		delete [] pRGB;
	return -1;
ErrorIgnore:
	if(lpBMIH != NULL)
		delete [] lpBMIH;
	if(lpImage != NULL)
		delete [] lpImage;
	if(pRGB  != NULL)
		delete [] pRGB;
	return -2;
}

static void MakeGrayColorTable(LPVOID lpvColorTable)
{
	int		nColorTableEntries = 256;
	BYTE	byteValue=0;

	LPRGBQUAD pDibQuad = (LPRGBQUAD) lpvColorTable;
	for(int i = 0; i < nColorTableEntries; i++) {
		pDibQuad->rgbRed = byteValue;
		pDibQuad->rgbGreen = byteValue;
		pDibQuad->rgbBlue = byteValue;
		byteValue++;
		pDibQuad++;
	}
}

static void MakeColorTable(LPVOID lpvColorTable,RGBQUAD *RGB)
{
	int		nColorTableEntries = 256;
	BYTE	byteValue=0;
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

static void MakeDefectsColorTable(LPVOID lpvColorTable)
{
	int		nColorTableEntries = 256;
	BYTE	byteValue=0;

	LPRGBQUAD pDibQuad = (LPRGBQUAD) lpvColorTable;
	for(int i = 0; i < nColorTableEntries; i++) {
		if (i==0) {
			pDibQuad->rgbRed = 255;
			pDibQuad->rgbGreen = 0;
			pDibQuad->rgbBlue = 0;
		}
		else if(i==1){
			pDibQuad->rgbRed = 0;
			pDibQuad->rgbGreen = 0;
			pDibQuad->rgbBlue = 255;
		}
		else {
			pDibQuad->rgbRed = byteValue;
			pDibQuad->rgbGreen = byteValue;
			pDibQuad->rgbBlue = byteValue;
			pDibQuad->rgbReserved = 0;
		}
		byteValue++;
		pDibQuad++;
	}
}


int SaveGrayScaleBMPFile(CString strFileName, CImageBuffer& bufImage)
{
	CFile   file;

	BITMAPFILEHEADER	bmfh;
	LPBITMAPINFOHEADER	lpBMIH = NULL;	//  buffer containing the BITMAPINFOHEADER
	int					nColorTableEntries;
	BYTE*				lpImage = NULL;
	DWORD				dwSizeImage;
	LPVOID				lpvColorTable = NULL;
	int					nSize;
	BYTE*				pSrc;
	BYTE*				pDst;
	int					nPitch;
	int					nWidth, nHeight;

	lpBMIH = NULL;
	lpImage = NULL;
	nColorTableEntries = 256;
	bmfh.bfType = 0x4d42;  // 'BM'
	int nSizeHdr = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * nColorTableEntries;
	bmfh.bfSize = 0;
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * nColorTableEntries;	
	nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);

	lpBMIH = (LPBITMAPINFOHEADER) new char[abs(nSize)];
	lpvColorTable = (LPBYTE) lpBMIH + sizeof(BITMAPINFOHEADER);
	dwSizeImage = bufImage.GetImgSize().cy*bufImage.GetPitch();

	lpBMIH->biSizeImage = 0; // dwSizeImage;
	lpBMIH->biWidth = bufImage.GetImgSize().cx;
	lpBMIH->biHeight = bufImage.GetImgSize().cy;
	lpBMIH->biBitCount = bufImage.GetBitCount();
	lpBMIH->biPlanes = 1;
	lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	lpBMIH->biXPelsPerMeter = 2925;
	lpBMIH->biYPelsPerMeter = 2925;
	lpBMIH->biCompression = BI_RGB;
	lpBMIH->biClrImportant = 0;
	lpBMIH->biClrUsed = 256;

	if(bufImage.GetAdapter()!=NULL){
		RGBQUAD *RGB = bufImage.GetAdapter()->GetColorPallete();
		MakeColorTable(lpvColorTable,RGB);
	}
	else
		MakeGrayColorTable(lpvColorTable);
	
	lpImage = (LPBYTE) new char[abs((int) dwSizeImage)];

	nHeight = bufImage.GetImgSize().cy;
	nWidth = bufImage.GetPitch();
	pDst = lpImage;
	nPitch = bufImage.GetPitch();
	pSrc = bufImage.GetImgPtr() + nPitch*(nHeight-1);

	for (int i=0; i<nHeight; i++, pDst+=nPitch, pSrc-=nPitch) {	// Copy and flip
		memcpy(pDst, pSrc, nWidth);
	}

	if(strFileName.Right(3).CompareNoCase("bmp") != 0)
		strFileName.Append(".bmp");

	if (!file.Open(strFileName, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary))
		goto ErrorAbort;

	try {
		file.Write((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));
		file.Write((LPVOID) lpBMIH,  nSizeHdr);
		file.Write((LPVOID) lpImage, dwSizeImage);
	}
	catch(CException* pe) {
		pe->Delete();
		goto ErrorAbort;
	}
	if(lpBMIH != NULL)
		delete [] lpBMIH;
	if(lpImage != NULL)
		delete [] lpImage;
	file.Close();
	return 0;
ErrorAbort:
	if(lpBMIH != NULL)
		delete [] lpBMIH;
	if(lpImage != NULL)
		delete [] lpImage;
	return -1;
}


int SaveGrayScaleBMPFileROI(CString strFileName, CRect* prcROI, CImageBuffer& bufImage)
{
	CImageBuffer ImgROI;
	CRect rcROIRelative;
	int nError;

	rcROIRelative = CRect(CPoint(0,0), prcROI->Size());
	ImgROI.Allocate(AbsSize(rcROIRelative.Size()));
	nError = ImgCopy(&bufImage,
					 prcROI,
					 &ImgROI,
					 &rcROIRelative);
	if(nError)
		goto ErrorAbort;
	nError = SaveGrayScaleBMPFile(strFileName, ImgROI);
	if(nError)
		goto ErrorAbort;
	return 0;
ErrorAbort:
	return -1;
}


int SaveDaiIchiDefectsBMPFile(CString strFileName, 
						    CImageBuffer& bufImage, 
						    CRect* prcDefects, 
						    int nDefects,
							int nInnerRectOffset,
							int nMinInnerSize,
							int nOuterInnerDis,
							int nLineWidth)
{
	CRect* prcLocationDefects;
	int nLocationDefects;
	int nError;

	if(nLineWidth<1)
		nLineWidth = 1;
	nLocationDefects = 2* nDefects;
	prcLocationDefects = new CRect[abs(nLocationDefects)];
	nError = CreateDefectsLocationWindow(bufImage.GetImgSize(),
										  prcDefects, 
										  nDefects,
										  &prcLocationDefects[0],
										  nLocationDefects,
										  nInnerRectOffset,
										  nMinInnerSize,
										  nOuterInnerDis);
	if(nError)
		goto ErrorAbort;
	nError = SaveDefectsBMPFile(strFileName, 
							    bufImage, 
								&prcLocationDefects[0], 
								nLocationDefects,
								nLineWidth);
	if(nError)
		goto ErrorAbort;

	delete [] prcLocationDefects;
	return 0;
ErrorAbort:
	delete [] prcLocationDefects;
	return -1;
}

int CreateDefectsLocationWindow(CSize& sizeImage,
								 CRect* prcDefects, 
								 int nDefects,
								 CRect* prcLocationDefects,
								 int nLocationDefects,
								 int nInnerRectOffset,
								 int nMinInnerSize,
								 int nOuterInnerDis)
{
	int i;
	CRect rcInner;
	CRect rcOuter;
	CRect rcDefect;
	int nCount;
	if(nLocationDefects < (2*nDefects))
		goto ErrorAbort;
	nCount = 0;
	for(i=0;i<nDefects;i++) {
		rcDefect = prcDefects[i];
		rcDefect.InflateRect(nInnerRectOffset,nInnerRectOffset,nInnerRectOffset,nInnerRectOffset);
		if(nMinInnerSize > rcDefect.Width()) {
			rcInner.left = rcDefect.CenterPoint().x - nMinInnerSize/2;
			rcInner.right = rcDefect.CenterPoint().x + nMinInnerSize/2;
		}
		else {
			rcInner.left = rcDefect.left;
			rcInner.right = rcDefect.right;
		}
		if(nMinInnerSize > rcDefect.Height()) {
			rcInner.top = rcDefect.CenterPoint().y - nMinInnerSize/2;
			rcInner.bottom = rcDefect.CenterPoint().y + nMinInnerSize/2;
		}
		else {
			rcInner.top = rcDefect.top;
			rcInner.bottom = rcDefect.bottom;
		}
		if(CheckRect(&rcInner, &sizeImage)) 
			goto ErrorAbort;
		rcOuter = rcInner;
		rcOuter.InflateRect(nOuterInnerDis,nOuterInnerDis,nOuterInnerDis,nOuterInnerDis);
		if(CheckRect(&rcInner, &sizeImage)) 
			goto ErrorAbort;
		prcLocationDefects[nCount++] = rcInner;
		prcLocationDefects[nCount++] = rcOuter;
	}
	return 0;
ErrorAbort:
	return -1;
}



int SaveDefectsBMPFile(CString strFileName, 
					   CImageBuffer& bufImage, 
					   CRect* prcDefects, 
					   int nDefects,
					   int nLineWidth)
{
	CFile   file;
	int i, j, k;
	BITMAPFILEHEADER	bmfh;
	LPBITMAPINFOHEADER	lpBMIH = NULL;	//  buffer containing the BITMAPINFOHEADER
	int					nColorTableEntries;
	BYTE*				lpImage = NULL;
	DWORD				dwSizeImage;
	LPVOID				lpvColorTable = NULL;
	int					nSize;
	BYTE*				pSrc;
	BYTE*				pDst;
	BYTE*				pSrcCol;
	BYTE*				pDstCol;
	int					nPitch;
	int					nWidth, nHeight;
	CSize				csImg;
	CRect rcDefect;
	BYTE	*pImgTopLftPtr, *pImgProcTopLine, *pImgProcBotLine, *pImgProcLeftLine, *pImgProcRightLine;
	int	nDefectWidth;
	int	nDefectHeight;

	int nArrowSaved;

	if(nLineWidth < 1)
		nLineWidth = 1;

	lpBMIH = NULL;
	lpImage = NULL;
	nArrowSaved = nDefects;
	if(nArrowSaved > MAX_ARROW_SAVED)
		nArrowSaved = MAX_ARROW_SAVED;

	csImg = bufImage.GetImgSize();
	nColorTableEntries = 256;
	bmfh.bfType = 0x4d42;  // 'BM'
	int nSizeHdr = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * nColorTableEntries;
	bmfh.bfSize = 0;
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * nColorTableEntries;	
	nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);

	lpBMIH = (LPBITMAPINFOHEADER) new char[abs(nSize)];
	lpvColorTable = (LPBYTE) lpBMIH + sizeof(BITMAPINFOHEADER);
	dwSizeImage = bufImage.GetImgSize().cy*bufImage.GetPitch();

	lpBMIH->biSizeImage = 0; // dwSizeImage;
	lpBMIH->biWidth = bufImage.GetImgSize().cx;
	lpBMIH->biHeight = bufImage.GetImgSize().cy;
	lpBMIH->biBitCount = bufImage.GetBitCount();
	lpBMIH->biPlanes = 1;
	lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	lpBMIH->biXPelsPerMeter = 2925;
	lpBMIH->biYPelsPerMeter = 2925;
	lpBMIH->biCompression = BI_RGB;
	lpBMIH->biClrImportant = 0;
	lpBMIH->biClrUsed = 256;

	MakeDefectsColorTable(lpvColorTable);

	lpImage = (LPBYTE) new char[abs((int) dwSizeImage)];

	nHeight = bufImage.GetImgSize().cy;
	nWidth = bufImage.GetPitch();
	pDst = lpImage;
	nPitch = bufImage.GetPitch();
	pSrc = bufImage.GetImgPtr() + nPitch*(nHeight-1);

	for (i=0; i<nHeight; i++, pDst+=nPitch, pSrc-=nPitch) {
		for (j=0,pSrcCol=pSrc,pDstCol = pDst;j<nWidth;j++,pDstCol++,pSrcCol++) {
			if(*pSrcCol <= 1)
				*pDstCol = 2;
			else
				*pDstCol = *pSrcCol;
		}
	}

	for(i=0;i<nArrowSaved;i++) {
		if(ValidateRect(&prcDefects[i], &csImg))
			continue;
		rcDefect = CRect(prcDefects[i].left,
						 nHeight - prcDefects[i].bottom,
						 prcDefects[i].right,
						 nHeight - prcDefects[i].top);
		rcDefect.InflateRect(nLineWidth,nLineWidth,nLineWidth,nLineWidth);
		if(CheckRect(&rcDefect, &csImg)) 
			continue;
		for(k=0;k<nLineWidth;k++) {
			if(CheckRect(&rcDefect, &csImg))
				break;
			nDefectWidth = rcDefect.Width();
			nDefectHeight = rcDefect.Height();
			pImgTopLftPtr = lpImage + rcDefect.top * nPitch + rcDefect.left;
			pImgProcLeftLine=pImgTopLftPtr;
			pImgProcRightLine=pImgTopLftPtr + nDefectWidth - 1;
			for(j = 0; j < nDefectHeight; j++, pImgProcLeftLine += nPitch, pImgProcRightLine += nPitch) {
				*pImgProcLeftLine = 0;
				*pImgProcRightLine = 0;
			}
			pImgProcTopLine = pImgTopLftPtr;
			pImgProcBotLine = pImgTopLftPtr + nPitch * (nDefectHeight - 1);
			for(j = 0; j < nDefectWidth; j++) {
				*pImgProcTopLine++ = 0;
				*pImgProcBotLine++ = 0;
			}
			rcDefect.DeflateRect(1,1,1,1);
			if((rcDefect.left >= rcDefect.right) || (rcDefect.top >= rcDefect.bottom))
				break;
		}
	}

	if (!file.Open(strFileName, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary))
		goto ErrorAbort;

	try {
		file.Write((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));
		file.Write((LPVOID) lpBMIH,  nSizeHdr);
		file.Write((LPVOID) lpImage, dwSizeImage);
	}
	catch(CException* pe) {
		pe->Delete();
		goto ErrorAbort;
	}
	if(lpBMIH != NULL)
		delete [] lpBMIH;
	if(lpImage != NULL)
		delete [] lpImage;
	file.Close();
	return 0;
ErrorAbort:
	if(lpBMIH != NULL)
		delete [] lpBMIH;
	if(lpImage != NULL)
		delete [] lpImage;
	return -1;
}

int SaveDefectsBMPFile(CString strFileName, 
					   CImageBuffer& bufImage, 
					   CRect* prcDefects,
					   int nDefects,
					   int nArrowOffset,
					   CImageBuffer* pImgArrow)
{
	CFile   file;
	int i, j;
	int i1, j1;
	BITMAPFILEHEADER	bmfh;
	LPBITMAPINFOHEADER	lpBMIH = NULL;	//  buffer containing the BITMAPINFOHEADER
	int					nColorTableEntries;
	BYTE*				lpImage = NULL;
	DWORD				dwSizeImage;
	LPVOID				lpvColorTable = NULL;
	int					nSize;
	BYTE*				pSrc;
	BYTE*				pDst;
	BYTE*				pSrcCol;
	BYTE*				pDstCol;
	int					nPitch;
	int					nWidth, nHeight;
	CSize				csImg;
	BYTE	*pImgTopLftPtr, *pImgProcTopLine, *pImgProcBotLine, *pImgProcLeftLine, *pImgProcRightLine;
	int	nDefectWidth;
	int	nDefectHeight;
	CRect rcDefect;
	CRect rcArrowRelative;
	CPoint ptOffsetOfArrow;
	CRect rcSrc;
	CRect rcOverlap;
	CRect rcArrow;
	long lIndex, lIndexTemp;
	int nArrowPitch;
	BYTE* pImgArrowPtr;
	int nSideTemp;
	int nArrowSaved;

	int nArrowWidth;
	int nArrowLength;
	int* pnArrowSide;
	int nError;
	int nSizeHdr;


	lpBMIH = NULL;
	lpImage = NULL;
	pnArrowSide = NULL;

	if(pImgArrow[TOP].IsAllocated()) {
		ASSERT(!"pImgArrow is not Allocated");
		goto ErrorAbort;
	}
	if(pImgArrow[BOTTOM].IsAllocated()) {
		ASSERT(!"pImgArrow is not Allocated");
		goto ErrorAbort;
	}
	if(pImgArrow[LEFT].IsAllocated()) {
		ASSERT(!"pImgArrow is not Allocated");
		goto ErrorAbort;
	}
	if(pImgArrow[RIGHT].IsAllocated()) {
		ASSERT(!"pImgArrow is not Allocated");
		goto ErrorAbort;
	}
	pnArrowSide = new int[abs(nDefects)];
//	nArrowWidth = pImgArrow[TOP].GetImgSize().cx;
//	nArrowLength = pImgArrow[TOP].GetImgSize().cy;

	nArrowWidth = 20;
	nArrowLength = 60;
	nError = GetArrowDir(prcDefects,
						 nDefects,
						 nArrowOffset,
						 nArrowWidth,
						 nArrowLength,
						 &pnArrowSide[0]);
	if(nError)
		goto ErrorAbort;


	nArrowSaved = nDefects;
	if(nArrowSaved > MAX_ARROW_SAVED)
		nArrowSaved = MAX_ARROW_SAVED;
	
	csImg = bufImage.GetImgSize();
	nColorTableEntries = 256;
	bmfh.bfType = 0x4d42;  // 'BM'
	nSizeHdr = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * nColorTableEntries;
	bmfh.bfSize = 0;
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * nColorTableEntries;	
	nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);

	lpBMIH = (LPBITMAPINFOHEADER) new char[abs(nSize)];
	lpvColorTable = (LPBYTE) lpBMIH + sizeof(BITMAPINFOHEADER);
	dwSizeImage = bufImage.GetImgSize().cy*bufImage.GetPitch();

	lpBMIH->biSizeImage = 0; // dwSizeImage;
	lpBMIH->biWidth = bufImage.GetImgSize().cx;
	lpBMIH->biHeight = bufImage.GetImgSize().cy;
	lpBMIH->biBitCount = bufImage.GetBitCount();
	lpBMIH->biPlanes = 1;
	lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	lpBMIH->biXPelsPerMeter = 2925;
	lpBMIH->biYPelsPerMeter = 2925;
	lpBMIH->biCompression = BI_RGB;
	lpBMIH->biClrImportant = 0;
	lpBMIH->biClrUsed = 256;

	MakeDefectsColorTable(lpvColorTable);

	lpImage = (LPBYTE) new char[abs((int) dwSizeImage)];

	nHeight = bufImage.GetImgSize().cy;
	nWidth = bufImage.GetPitch();
	pDst = lpImage;
	nPitch = bufImage.GetPitch();
	pSrc = bufImage.GetImgPtr() + nPitch*(nHeight-1);

	for (i=0; i<nHeight; i++, pDst+=nPitch, pSrc-=nPitch) {
		for (j=0,pSrcCol=pSrc,pDstCol = pDst;j<nWidth;j++,pDstCol++,pSrcCol++) {
			if(*pSrcCol <= 1)
				*pDstCol = 2;
			else
				*pDstCol = *pSrcCol;
		}
	}

	for(i=0;i<nArrowSaved;i++) {
		if(ValidateRect(&prcDefects[i], &csImg))
			continue;
		rcDefect = CRect(prcDefects[i].left,
						 nHeight - prcDefects[i].bottom,
						 prcDefects[i].right,
						 nHeight - prcDefects[i].top);
		nDefectWidth = rcDefect.Width();
		nDefectHeight = rcDefect.Height();
		pImgTopLftPtr = lpImage + rcDefect.top * nPitch + rcDefect.left;
		pImgProcLeftLine=pImgTopLftPtr;
		pImgProcRightLine=pImgTopLftPtr + nDefectWidth - 1;
		for(j = 0; j < nDefectHeight; j++, pImgProcLeftLine += nPitch, pImgProcRightLine += nPitch) {
			*pImgProcLeftLine = 0;
			*pImgProcRightLine = 0;
		}
		pImgProcTopLine = pImgTopLftPtr;
		pImgProcBotLine = pImgTopLftPtr + nPitch * (nDefectHeight - 1);
		for(j = 0; j < nDefectWidth; j++) {
			*pImgProcTopLine++ = 0;
			*pImgProcBotLine++ = 0;
		}

		rcSrc = CRect(CPoint(0,0), bufImage.GetImgSize());
		nSideTemp = pnArrowSide[i];
		if(pnArrowSide[i] == TOP)
			nSideTemp = BOTTOM;
		if(pnArrowSide[i] == BOTTOM)
			nSideTemp = TOP;
		pnArrowSide[i] = nSideTemp;
		switch(pnArrowSide[i]) {
			case LEFT:
				ptOffsetOfArrow = CPoint(prcDefects[i].left - nArrowOffset - pImgArrow[LEFT].GetImgSize().cx,
										 nHeight - prcDefects[i].CenterPoint().y - pImgArrow[LEFT].GetImgSize().cy/2);
				rcArrowRelative = CRect(CPoint(0,0), pImgArrow[LEFT].GetImgSize());
				break;
			case RIGHT:
				ptOffsetOfArrow = CPoint(prcDefects[i].right + nArrowOffset,
										 nHeight - prcDefects[i].CenterPoint().y - pImgArrow[RIGHT].GetImgSize().cy/2);
				rcArrowRelative = CRect(CPoint(0,0), pImgArrow[RIGHT].GetImgSize());
				break;
			case TOP:
				ptOffsetOfArrow = CPoint(prcDefects[i].CenterPoint().x - pImgArrow[TOP].GetImgSize().cx/2,
										 nHeight - (prcDefects[i].bottom + nArrowOffset + pImgArrow[TOP].GetImgSize().cy));
				rcArrowRelative = CRect(CPoint(0,0), pImgArrow[TOP].GetImgSize());
				break;
			case BOTTOM:
				ptOffsetOfArrow = CPoint(prcDefects[i].CenterPoint().x - pImgArrow[BOTTOM].GetImgSize().cx/2,
										 nHeight - (prcDefects[i].top - nArrowOffset));
				rcArrowRelative = CRect(CPoint(0,0), pImgArrow[BOTTOM].GetImgSize());
				break;
		}
		pImgArrowPtr = pImgArrow[pnArrowSide[i]].GetImgPtr();
		nArrowPitch = pImgArrow[pnArrowSide[i]].GetPitch();

		rcArrow = CRect(rcArrowRelative.left + ptOffsetOfArrow.x,
						rcArrowRelative.top + ptOffsetOfArrow.y,
						rcArrowRelative.right + ptOffsetOfArrow.x,
						rcArrowRelative.bottom + ptOffsetOfArrow.y);
		rcOverlap = rcSrc;
		if(rcOverlap.left < rcArrow.left) rcOverlap.left = rcArrow.left;
		if(rcOverlap.top < rcArrow.top) rcOverlap.top = rcArrow.top;
		if(rcOverlap.right > rcArrow.right) rcOverlap.right = rcArrow.right;
		if(rcOverlap.bottom > rcArrow.bottom) rcOverlap.bottom = rcArrow.bottom;

		if (ValidateRect(&rcOverlap,&(bufImage.GetImgSize()))) {

			goto ErrorAbort;
		}


		lIndex = rcOverlap.top * nPitch;
		lIndexTemp = (rcOverlap.top - ptOffsetOfArrow.y) * nArrowPitch;
		for(i1 = rcOverlap.top; i1 <rcOverlap.bottom; i1++) {
			for(j1 = rcOverlap.left; j1 < rcOverlap.right; j1++) {
				if(pImgArrowPtr[lIndexTemp + j1 - ptOffsetOfArrow.x] == 255) 
					lpImage[lIndex + j1] = 1;
			}
			lIndex += nPitch;
			lIndexTemp += nArrowPitch;
		}

	}
	if (!file.Open(strFileName, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary))
		goto ErrorAbort;

	try {
		file.Write((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));
		file.Write((LPVOID) lpBMIH,  nSizeHdr);
		file.Write((LPVOID) lpImage, dwSizeImage);
	}
	catch(CException* pe) {
		pe->Delete();
		goto ErrorAbort;
	}
	if(lpBMIH != NULL)
		delete [] lpBMIH;
	if(lpImage != NULL)
		delete [] lpImage;
	if(pnArrowSide != NULL)
		delete [] pnArrowSide;
	file.Close();
	return 0;
ErrorAbort:
	if(lpBMIH != NULL)
		delete [] lpBMIH;
	if(lpImage != NULL)
		delete [] lpImage;
	if(pnArrowSide != NULL)
		delete [] pnArrowSide;
	return -1;
}

int GetArrowDir(CRect* prcFndBlob,
					  int nFndBlobNo,
					  double dOffset,
					  int nArrowWidth,
					  int nArrowLength,
					  int* pnSide)
{
	int i, j, k1, k2;
	CRect* prcArrow[4];
	CPoint ptBlobCtr;
	int nOverlapCount[4];
	int nArrowHalfWidth;
	int nMin;
	int nMinIndex;
	int nBlobHalfWidth;
	int nBlobHalfHeight;
	int nArrowShow;

	nArrowShow = nFndBlobNo;
	if(nArrowShow > MAX_ARROW_SAVED)
		nArrowShow = MAX_ARROW_SAVED;

	nArrowHalfWidth = nArrowWidth/2;
	if(nFndBlobNo == 0)
		return -1;
	if(nFndBlobNo == 1) {
		pnSide[0] = LEFT;
		return 0;
	}
	for(i=0;i<4;i++)
		prcArrow[i] = new CRect[abs(nArrowShow)];
	for(i=0;i<nArrowShow;i++) {
		ptBlobCtr = prcFndBlob[i].CenterPoint();
		nBlobHalfWidth = prcFndBlob[i].Width()/2;
		nBlobHalfHeight = prcFndBlob[i].Height()/2;
		prcArrow[0][i] = CRect(ptBlobCtr.x - nArrowHalfWidth,
						   Round(ptBlobCtr.y - nBlobHalfHeight - dOffset - nArrowLength),
						   ptBlobCtr.x + nArrowHalfWidth,
						   Round(ptBlobCtr.y - nBlobHalfHeight - dOffset));
		prcArrow[1][i] = CRect(ptBlobCtr.x - nArrowHalfWidth,
						   Round(ptBlobCtr.y + nBlobHalfHeight + dOffset),
						   ptBlobCtr.x + nArrowHalfWidth,
						   Round(ptBlobCtr.y + nBlobHalfHeight + dOffset + nArrowLength));
		prcArrow[2][i] = CRect(Round(ptBlobCtr.x - nBlobHalfWidth - dOffset - nArrowLength),
						   ptBlobCtr.y - nArrowHalfWidth,
						   Round(ptBlobCtr.x - nBlobHalfWidth - dOffset),
						   ptBlobCtr.y + nArrowHalfWidth);
		prcArrow[3][i] = CRect(Round(ptBlobCtr.x + nBlobHalfWidth + dOffset),
						   ptBlobCtr.y - nArrowHalfWidth,
						   Round(ptBlobCtr.x + nBlobHalfWidth + dOffset + nArrowLength),
						   ptBlobCtr.y + nArrowHalfWidth);
	}
	for(i=0;i<nArrowShow;i++) {
		for(j=0;j<4;j++) 
			nOverlapCount[j] = 0;
		for(k1=0;k1<4;k1++) {
			for(j=0;j<nArrowShow;j++) {
				if(i == j)
					continue;
				for(k2=0;k2<4;k2++) {
					if(RectOverlapRect(prcArrow[k1][i], prcFndBlob[j]) == 1)
						nOverlapCount[k1]+=1000;
					if(RectOverlapRect(prcArrow[k1][i], prcArrow[k2][j]) == 1)
						nOverlapCount[k1]++;
				}
			}
		}
		nMin = nOverlapCount[0];
		nMinIndex = 0;
		for(k1=1;k1<4;k1++) {
			if(nMin > nOverlapCount[k1]) {
				nMin = nOverlapCount[k1];
				nMinIndex = k1;
			}
		}
		pnSide[i] = nMinIndex;
		for(k1=0;k1<4;k1++) {
			prcArrow[k1][i] = prcArrow[nMinIndex][i];
		}
	}
	for(i=nArrowShow;i<nFndBlobNo;i++) 
		pnSide[i] = 0;

	for(i=0;i<4;i++)
		delete [] prcArrow[i];
	return 0;
}

int LoadDefectInfoBMPEmbeddedFile(CString strFileName, std::vector<CRect>& prcDefects, std::vector<CPoint>& pptDefectCenters)
{
	int nCount, nSize;
	CFile   file;
	LPBYTE lpImage = NULL;		// starting address of DIB bits
	LPBITMAPINFOHEADER lpBMIH;	//  buffer containing the BITMAPINFOHEADER
	DWORD dwSizeImage;			// of bits -- not BITMAPINFOHEADER or BITMAPFILEHEADER
	BYTE*	pSrc;

	BITMAPFILEHEADER bmfh;

	lpBMIH = NULL;
	lpImage = NULL;
	int nWidth;
	int nHeight;

	//For Reading Embedded Defect Info
	int nDefects = 0;
	LPBYTE lpDefectInfo = NULL;		// starting address of Defect bits
	LPBYTE lpDefectCount = NULL;
	UnsignedShortByteUnion* unionConverter = NULL;
	int nCurrentSize = 0;

	if (!file.Open(strFileName, CFile::modeRead))
		goto ErrorAbort;

	try {
		nCount = file.Read((LPVOID)&bmfh, sizeof(BITMAPFILEHEADER));
		if (nCount != sizeof(BITMAPFILEHEADER)) {
			throw new CMemoryException;
		}
		if (bmfh.bfType != 0x4d42) {
			throw new CMemoryException;
		}

		nCurrentSize += nCount;

		nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		lpBMIH = (LPBITMAPINFOHEADER) new char[abs(nSize)];
		nCount = file.Read(lpBMIH, nSize); // info hdr & color table
		nCurrentSize += nCount;

		// Compute the metrics
		if (lpBMIH->biSize != sizeof(BITMAPINFOHEADER)) {
			TRACE("Not a valid Windows bitmap -- probably an OS/2 bitmap\n");
			throw new CMemoryException;
		}
		dwSizeImage = lpBMIH->biSizeImage;
		if (dwSizeImage == 0) {
			DWORD dwBytes = ((DWORD)lpBMIH->biWidth * lpBMIH->biBitCount) / 32;
			if (((DWORD)lpBMIH->biWidth * lpBMIH->biBitCount) % 32) {
				dwBytes++;
			}
			dwBytes *= 4;
			dwSizeImage = dwBytes * lpBMIH->biHeight; // no compression
		}

		nWidth = (DWORD)lpBMIH->biWidth;
		nHeight = (DWORD)lpBMIH->biHeight;

		lpImage = (LPBYTE) new char[abs((int)dwSizeImage)];
		nCount = file.Read(lpImage, dwSizeImage); // image only
		nCurrentSize += nCount;

		nCurrentSize += 2;
		if (nCurrentSize < file.GetLength()) {
			//Calculate how many bytes needed for store defect information(unit store is 2-byte: unsigned short int)
			//Following this order: Number of defect, coordinate of center defect i(x, y), top left defect i(x, y), bottom right i(x, y): (1 + defect count * 6) * 2 bytes


			lpDefectCount = (LPBYTE) new char[2];
			nCount = file.Read(lpDefectCount, 2);
			pSrc = lpDefectCount;

			unionConverter = new UnsignedShortByteUnion;
			unionConverter->asBytes[0] = *pSrc;
			pSrc++;
			unionConverter->asBytes[1] = *pSrc;
			//OutputDebugLogTo(7, FALSE, "Current Size: %d, Length: %d, Defect Count is: %d", nCurrentSize, file.GetLength(), unionConverter->asUnsignedShort);
			DWORD dwSizeDefectInfo = unionConverter->asUnsignedShort * 6 * 2;
			if (nCurrentSize + dwSizeDefectInfo == file.GetLength()) {
				nDefects = unionConverter->asUnsignedShort;
				lpDefectInfo = (LPBYTE) new char[abs((int)dwSizeDefectInfo)];
				nCount = file.Read(lpDefectInfo, dwSizeDefectInfo); // defect info only

				prcDefects.resize(nDefects);
				pptDefectCenters.resize(nDefects);

				pSrc = lpDefectInfo;
				for (int i = 0; i < nDefects; i++) {
					prcDefects[i] = CRect();
					pptDefectCenters[i] = CPoint();

					//Get the defect center
					unionConverter = new UnsignedShortByteUnion;
					unionConverter->asBytes[0] = *pSrc;
					pSrc++;
					unionConverter->asBytes[1] = *pSrc;
					pSrc++;
					pptDefectCenters[i].x = unionConverter->asUnsignedShort;

					unionConverter = new UnsignedShortByteUnion;
					unionConverter->asBytes[0] = *pSrc;
					pSrc++;
					unionConverter->asBytes[1] = *pSrc;
					pSrc++;
					pptDefectCenters[i].y = unionConverter->asUnsignedShort;

					//Store Defect Bounding Box
						//Top Left
					unionConverter = new UnsignedShortByteUnion;
					unionConverter->asBytes[0] = *pSrc;
					pSrc++;
					unionConverter->asBytes[1] = *pSrc;
					pSrc++;
					prcDefects[i].left = unionConverter->asUnsignedShort;//X

					unionConverter = new UnsignedShortByteUnion;
					unionConverter->asBytes[0] = *pSrc;
					pSrc++;
					unionConverter->asBytes[1] = *pSrc;
					pSrc++;
					prcDefects[i].top = unionConverter->asUnsignedShort;//Y

					//Bottom Right
					unionConverter = new UnsignedShortByteUnion;
					unionConverter->asBytes[0] = *pSrc;
					pSrc++;
					unionConverter->asBytes[1] = *pSrc;
					pSrc++;
					prcDefects[i].right = unionConverter->asUnsignedShort;//X

					unionConverter = new UnsignedShortByteUnion;
					unionConverter->asBytes[0] = *pSrc;
					pSrc++;
					unionConverter->asBytes[1] = *pSrc;
					pSrc++;
					prcDefects[i].bottom = unionConverter->asUnsignedShort;//Y

					//Validate the data
					if (pptDefectCenters[i].x > nWidth || pptDefectCenters[i].y > nHeight ||
						prcDefects[i].left > nWidth || prcDefects[i].right > nWidth ||
						prcDefects[i].top > nHeight || prcDefects[i].bottom > nHeight) {
						prcDefects.clear();
						pptDefectCenters.clear();
						nDefects = 0;
						break;
					}
				}
			}
		}
	}


	catch (CException* pe) {
		pe->Delete();
		goto ErrorAbort;
	}
	if (lpBMIH != NULL)
		delete[] lpBMIH;
	if (lpImage != NULL)
		delete[] lpImage;

	if (lpDefectCount != NULL)
		delete[] lpDefectCount;
	if (lpDefectInfo != NULL)
		delete[] lpDefectInfo;
	if (unionConverter != NULL)
		delete unionConverter;

	file.Close();
	return 0;
ErrorAbort:
	if (lpBMIH != NULL)
		delete[] lpBMIH;
	if (lpImage != NULL)
		delete[] lpImage;

	nDefects = 0;
	if (lpDefectCount != NULL)
		delete[] lpDefectCount;
	if (lpDefectInfo != NULL)
		delete[] lpDefectInfo;
	if (unionConverter != NULL)
		delete unionConverter;

	return -1;
ErrorIgnore:
	if (lpBMIH != NULL)
		delete[] lpBMIH;
	if (lpImage != NULL)
		delete[] lpImage;

	nDefects = 0;
	if (lpDefectCount != NULL)
		delete[] lpDefectCount;
	if (lpDefectInfo != NULL)
		delete[] lpDefectInfo;
	if (unionConverter != NULL)
		delete unionConverter;

	return -2;
}

int LoadGrayScaleBMPEmbeddedFile(CString strFileName, CImageBuffer& bufImage, std::vector<CRect>& prcDefects, std::vector<CPoint>& pptDefectCenters, BOOL bSameBitCount)
{
	int nCount, nSize;
	CFile   file;

	enum Alloc { noAlloc, crtAlloc, heapAlloc };

	LPVOID lpvColorTable = NULL;
	LPBYTE lpImage = NULL;		// starting address of DIB bits
	LPBITMAPINFOHEADER lpBMIH;	//  buffer containing the BITMAPINFOHEADER
	Alloc nBmihAlloc;
	Alloc nImageAlloc;
	DWORD dwSizeImage;			// of bits -- not BITMAPINFOHEADER or BITMAPFILEHEADER
	BYTE*	pSrcBak;
	BYTE*	pSrc;
	BYTE*	pDst;
	BYTE*	pDstBak;
	BYTE* byteValue;
	RGBQUAD *pRGB;

	int nWidth;
	int nHeight;
	int nPitchSrc, nPitchDst;

	BITMAPFILEHEADER bmfh;

	lpBMIH = NULL;
	lpImage = NULL;
	pRGB = NULL;

	//For Reading Embedded Defect Info
	int nDefects = 0;
	LPBYTE lpDefectInfo = NULL;		// starting address of Defect bits
	LPBYTE lpDefectCount = NULL;
	UnsignedShortByteUnion* unionConverter = NULL;
	int nCurrentSize = 0;

	if (!file.Open(strFileName, CFile::modeRead))
		goto ErrorAbort;

	try {
		nCount = file.Read((LPVOID)&bmfh, sizeof(BITMAPFILEHEADER));
		if (nCount != sizeof(BITMAPFILEHEADER)) {
			throw new CMemoryException;
		}
		if (bmfh.bfType != 0x4d42) {
			throw new CMemoryException;
		}

		nCurrentSize += nCount;

		nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		lpBMIH = (LPBITMAPINFOHEADER) new char[abs(nSize)];
		nBmihAlloc = nImageAlloc = crtAlloc;
		nCount = file.Read(lpBMIH, nSize); // info hdr & color table

		nCurrentSize += nCount;

		if (bSameBitCount && (lpBMIH->biBitCount != bufImage.GetBitCount()))
			goto ErrorIgnore;

		// Compute the metrics
		if (lpBMIH->biSize != sizeof(BITMAPINFOHEADER)) {
			TRACE("Not a valid Windows bitmap -- probably an OS/2 bitmap\n");
			throw new CMemoryException;
		}
		dwSizeImage = lpBMIH->biSizeImage;
		if (dwSizeImage == 0) {
			DWORD dwBytes = ((DWORD)lpBMIH->biWidth * lpBMIH->biBitCount) / 32;
			if (((DWORD)lpBMIH->biWidth * lpBMIH->biBitCount) % 32) {
				dwBytes++;
			}
			dwBytes *= 4;
			dwSizeImage = dwBytes * lpBMIH->biHeight; // no compression
		}
		lpvColorTable = (LPBYTE)lpBMIH + sizeof(BITMAPINFOHEADER);

		lpImage = (LPBYTE) new char[abs((int)dwSizeImage)];
		nCount = file.Read(lpImage, dwSizeImage); // image only

		nCurrentSize += nCount;

		byteValue = (BYTE*)lpvColorTable;

		pRGB = new RGBQUAD[256];


		// Copy the image bytes
		nWidth = (DWORD)lpBMIH->biWidth;
		nHeight = (DWORD)lpBMIH->biHeight;
		nPitchSrc = dwSizeImage / nHeight;

		pSrc = lpImage + nPitchSrc * (nHeight - 1);

		if (!bufImage.IsAllocated())
			bufImage.Allocate(CSize(nWidth, nHeight), false, lpBMIH->biBitCount == 24);
		nPitchDst = bufImage.GetPitch();
		nWidth = min(nPitchSrc, bufImage.GetPitch());
		nHeight = min(nHeight, bufImage.GetImgSize().cy);
		pDst = bufImage.GetImgPtr(); //  + nPitchDst*(nHeight-1);

		///Udupa; Feb'2011; Generic to grascale to color buffer load and vice versa
		if (lpBMIH->biBitCount == bufImage.GetBitCount()) {
			for (int i = 0; i < nHeight; i++) {
				memcpy(pDst, pSrc, nWidth);
				pDst += nPitchDst;
				pSrc -= nPitchSrc;
			}

			nCurrentSize += 2;
			if (nCurrentSize < file.GetLength()) {
				//Calculate how many bytes needed for store defect information(unit store is 2-byte: unsigned short int)
				//Following this order: Number of defect, coordinate of center defect i(x, y), top left defect i(x, y), bottom right i(x, y): (1 + defect count * 6) * 2 bytes


				lpDefectCount = (LPBYTE) new char[2];
				nCount = file.Read(lpDefectCount, 2);
				pSrc = lpDefectCount;

				unionConverter = new UnsignedShortByteUnion;
				unionConverter->asBytes[0] = *pSrc;
				pSrc++;
				unionConverter->asBytes[1] = *pSrc;
				//OutputDebugLogTo(7, FALSE, "Current Size: %d, Length: %d, Defect Count is: %d", nCurrentSize, file.GetLength(), unionConverter->asUnsignedShort);
				DWORD dwSizeDefectInfo = unionConverter->asUnsignedShort * 6 * 2;
				if (nCurrentSize + dwSizeDefectInfo == file.GetLength()) {
					nDefects = unionConverter->asUnsignedShort;
					lpDefectInfo = (LPBYTE) new char[abs((int)dwSizeDefectInfo)];
					nCount = file.Read(lpDefectInfo, dwSizeDefectInfo); // defect info only

					prcDefects.resize(nDefects);
					pptDefectCenters.resize(nDefects);

					pSrc = lpDefectInfo;
					for (int i = 0; i < nDefects; i++) {
						prcDefects[i] = CRect();
						pptDefectCenters[i] = CPoint();

						//Get the defect center
						unionConverter = new UnsignedShortByteUnion;
						unionConverter->asBytes[0] = *pSrc;
						pSrc++;
						unionConverter->asBytes[1] = *pSrc;
						pSrc++;
						pptDefectCenters[i].x = unionConverter->asUnsignedShort;

						unionConverter = new UnsignedShortByteUnion;
						unionConverter->asBytes[0] = *pSrc;
						pSrc++;
						unionConverter->asBytes[1] = *pSrc;
						pSrc++;
						pptDefectCenters[i].y = unionConverter->asUnsignedShort;

						//Store Defect Bounding Box
							//Top Left
						unionConverter = new UnsignedShortByteUnion;
						unionConverter->asBytes[0] = *pSrc;
						pSrc++;
						unionConverter->asBytes[1] = *pSrc;
						pSrc++;
						prcDefects[i].left = unionConverter->asUnsignedShort;//X

						unionConverter = new UnsignedShortByteUnion;
						unionConverter->asBytes[0] = *pSrc;
						pSrc++;
						unionConverter->asBytes[1] = *pSrc;
						pSrc++;
						prcDefects[i].top = unionConverter->asUnsignedShort;//Y

						//Bottom Right
						unionConverter = new UnsignedShortByteUnion;
						unionConverter->asBytes[0] = *pSrc;
						pSrc++;
						unionConverter->asBytes[1] = *pSrc;
						pSrc++;
						prcDefects[i].right = unionConverter->asUnsignedShort;//X

						unionConverter = new UnsignedShortByteUnion;
						unionConverter->asBytes[0] = *pSrc;
						pSrc++;
						unionConverter->asBytes[1] = *pSrc;
						pSrc++;
						prcDefects[i].bottom = unionConverter->asUnsignedShort;//Y

						//Validate the data
						if (pptDefectCenters[i].x > nWidth || pptDefectCenters[i].y > nHeight ||
							prcDefects[i].left > nWidth || prcDefects[i].right > nWidth ||
							prcDefects[i].top > nHeight || prcDefects[i].bottom > nHeight) {
							prcDefects.clear();
							pptDefectCenters.clear();
							nDefects = 0;
							break;
						}
					}

				/*	for (int i = 0; i < nDefects; i++) {
						OutputDebugLogTo(6, FALSE, "Defect Center (%d, %d), Top Left (%d, %d), Bottom Right (%d, %d)",
							pptDefectCenters[i].x, pptDefectCenters[i].y, prcDefects[i].TopLeft().x, prcDefects[i].TopLeft().y,
							prcDefects[i].BottomRight().x, prcDefects[i].BottomRight().y);
					}*/
				}
			}



		}
		else if ((lpBMIH->biBitCount == 24) && !bufImage.IsColor()) {
			for (int i = 0; i < nHeight; i++) {
				pSrcBak = pSrc;
				pDstBak = pDst;
				for (int j = 0; j < nWidth; j++) {
					*pDst = (*pSrc + *(pSrc + 1) + *(pSrc + 2)) / 3;
					pDst++;
					pSrc += 3;
				}
				pSrc = pSrcBak;
				pDst = pDstBak;

				pDst += nPitchDst;
				pSrc -= nPitchSrc;
			}
		}
		else if ((lpBMIH->biBitCount == 8) && bufImage.IsColor()) {
			for (int i = 0; i < nHeight; i++) {
				pSrcBak = pSrc;
				pDstBak = pDst;
				for (int j = 0; j < nWidth; j++) {
					*pDst = *pSrc;
					pDst++;
					*pDst = *pSrc;
					pDst++;
					*pDst = *pSrc;
					pDst++;
					pSrc++;
				}
				pSrc = pSrcBak;
				pDst = pDstBak;

				pDst += nPitchDst;
				pSrc -= nPitchSrc;
			}
		}


		if (bufImage.GetAdapter() != NULL) {
			for (int i = 0; i < 256; i++) {
				pRGB[i].rgbBlue = i;
				pRGB[i].rgbGreen = i;
				pRGB[i].rgbRed = i;
			}
			bufImage.GetAdapter()->SetColorTable(256, pRGB);
		}
	}


	catch (CException* pe) {
		pe->Delete();
		goto ErrorAbort;
	}
	if (lpBMIH != NULL)
		delete[] lpBMIH;
	if (lpImage != NULL)
		delete[] lpImage;
	if (pRGB != NULL)
		delete[] pRGB;

	if (lpDefectCount != NULL)
		delete[] lpDefectCount;
	if (lpDefectInfo != NULL)
		delete[] lpDefectInfo;
	if (unionConverter != NULL)
		delete unionConverter;

	file.Close();
	return 0;
ErrorAbort:
	if (lpBMIH != NULL)
		delete[] lpBMIH;
	if (lpImage != NULL)
		delete[] lpImage;
	if (pRGB != NULL)
		delete[] pRGB;

	nDefects = 0;
	if (lpDefectCount != NULL)
		delete[] lpDefectCount;
	if (lpDefectInfo != NULL)
		delete[] lpDefectInfo;
	if (unionConverter != NULL)
		delete unionConverter;

	return -1;
ErrorIgnore:
	if (lpBMIH != NULL)
		delete[] lpBMIH;
	if (lpImage != NULL)
		delete[] lpImage;
	if (pRGB != NULL)
		delete[] pRGB;

	nDefects = 0;
	if (lpDefectCount != NULL)
		delete[] lpDefectCount;
	if (lpDefectInfo != NULL)
		delete[] lpDefectInfo;
	if (unionConverter != NULL)
		delete unionConverter;

	return -2;
}

 int SaveDefectsEmbeddedBMPFile(CString strFileName,
								CImageBuffer& bufImage, std::vector<CRect> prcDefects, std::vector<CPoint> pptDefectCenters)
{
	CFile   file;

	BITMAPFILEHEADER	bmfh;
	LPBITMAPINFOHEADER	lpBMIH = NULL;	//  buffer containing the BITMAPINFOHEADER
	int					nColorTableEntries;
	BYTE*				lpImage = NULL;
	DWORD				dwSizeImage;
	LPVOID				lpvColorTable = NULL;
	int					nSize;
	BYTE*				pSrc;
	BYTE*				pDst;
	int					nPitch;
	int					nWidth, nHeight;

	lpBMIH = NULL;
	lpImage = NULL;
	nColorTableEntries = 256;
	bmfh.bfType = 0x4d42;  // 'BM'
	int nSizeHdr = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * nColorTableEntries;
	bmfh.bfSize = 0;
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
		sizeof(RGBQUAD) * nColorTableEntries;
	nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);

	lpBMIH = (LPBITMAPINFOHEADER) new char[abs(nSize)];
	lpvColorTable = (LPBYTE)lpBMIH + sizeof(BITMAPINFOHEADER);
	dwSizeImage = bufImage.GetImgSize().cy*bufImage.GetPitch();

	lpBMIH->biSizeImage = 0; // dwSizeImage;
	lpBMIH->biWidth = bufImage.GetImgSize().cx;
	lpBMIH->biHeight = bufImage.GetImgSize().cy;
	lpBMIH->biBitCount = bufImage.GetBitCount();
	lpBMIH->biPlanes = 1;
	lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	lpBMIH->biXPelsPerMeter = 2925;
	lpBMIH->biYPelsPerMeter = 2925;
	lpBMIH->biCompression = BI_RGB;
	lpBMIH->biClrImportant = 0;
	lpBMIH->biClrUsed = 256;

	if (bufImage.GetAdapter() != NULL) {
		RGBQUAD *RGB = bufImage.GetAdapter()->GetColorPallete();
		MakeColorTable(lpvColorTable, RGB);
	}
	else
		MakeGrayColorTable(lpvColorTable);

	//Calculate how many bytes needed for store defect information(unit store is 2-byte: unsigned short int)
	//Following this order: Number of defect, coordinate of center defect i(x, y), top left defect i(x, y), bottom right i(x, y): (1 + defect count * 6) * 2 bytes
	int nDefects = pptDefectCenters.size();
	int nSizeDefectInfo = nDefects * 6 + 1;
	dwSizeImage += nSizeDefectInfo * 2;

	lpImage = (LPBYTE) new char[abs((int)dwSizeImage)];

	nHeight = bufImage.GetImgSize().cy;
	nWidth = bufImage.GetPitch();
	pDst = lpImage;
	nPitch = bufImage.GetPitch();
	pSrc = bufImage.GetImgPtr() + nPitch * (nHeight - 1);

	for (int i = 0; i < nHeight; i++, pDst += nPitch, pSrc -= nPitch) {	// Copy and flip
		memcpy(pDst, pSrc, nWidth);
	}

	//Save defect Info
	unsigned short int uDefectCount = nDefects;
	UnsignedShortByteUnion* unionConverter;
	unionConverter = new UnsignedShortByteUnion;
	unionConverter->asUnsignedShort = uDefectCount;
	*pDst = unionConverter->asBytes[0];
	pDst++;
	*pDst = unionConverter->asBytes[1];
	pDst++;
	for (int i = 0; i < nDefects; i++) {
		//Store defect center
		unsigned short int uPoint = pptDefectCenters[i].x;
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asUnsignedShort = uPoint;
		*pDst = unionConverter->asBytes[0];
		pDst++;
		*pDst = unionConverter->asBytes[1];
		pDst++;

		uPoint = pptDefectCenters[i].y;
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asUnsignedShort = uPoint;
		*pDst = unionConverter->asBytes[0];
		pDst++;
		*pDst = unionConverter->asBytes[1];
		pDst++;

		//Store Defect Bounding Box
			//Top Left
		uPoint = prcDefects[i].TopLeft().x;
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asUnsignedShort = uPoint;
		*pDst = unionConverter->asBytes[0];
		pDst++;
		*pDst = unionConverter->asBytes[1];
		pDst++;

		uPoint = prcDefects[i].TopLeft().y;
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asUnsignedShort = uPoint;
		*pDst = unionConverter->asBytes[0];
		pDst++;
		*pDst = unionConverter->asBytes[1];
		pDst++;

		//Bottom Right
		uPoint = prcDefects[i].BottomRight().x;
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asUnsignedShort = uPoint;
		*pDst = unionConverter->asBytes[0];
		pDst++;
		*pDst = unionConverter->asBytes[1];
		pDst++;

		uPoint = prcDefects[i].BottomRight().y;
		unionConverter = new UnsignedShortByteUnion;
		unionConverter->asUnsignedShort = uPoint;
		*pDst = unionConverter->asBytes[0];
		pDst++;
		*pDst = unionConverter->asBytes[1];
		pDst++;
	}


	if (strFileName.Right(3).CompareNoCase("bmp") != 0)
		strFileName.Append(".bmp");

	if (!file.Open(strFileName, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary))
		goto ErrorAbort;

	try {
		file.Write((LPVOID)&bmfh, sizeof(BITMAPFILEHEADER));
		file.Write((LPVOID)lpBMIH, nSizeHdr);
		file.Write((LPVOID)lpImage, dwSizeImage);
	}
	catch (CException* pe) {
		pe->Delete();
		goto ErrorAbort;
	}
	if (lpBMIH != NULL)
		delete[] lpBMIH;
	if (lpImage != NULL)
		delete[] lpImage;
	file.Close();
	return 0;
ErrorAbort:
	if (lpBMIH != NULL)
		delete[] lpBMIH;
	if (lpImage != NULL)
		delete[] lpImage;
	return -1;
}
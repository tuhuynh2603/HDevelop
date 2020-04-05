#include "stdafx.h"
#include "ImageFunction.h"
#include "ExternalFunctions.h"
#include "ImageStruct.h"

#include "Utilities.h"

#include "BitmapFunctions.h"
#include "CudaInterface.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void LogImage(int nTab, CImageBuffer& ImgBuf, LPCTSTR pMessageText, ...)
{
    char buf[1024];
    vsprintf_s(buf, pMessageText, (LPSTR)(&pMessageText+1));
	LogDebugMessage(buf, 
		ImgBuf.GetImgPtr(), 
		ImgBuf.GetImgSize().cx, 
		ImgBuf.GetImgSize().cy, 
		ImgBuf.GetPitch(), 
		ImgBuf.GetBitCount(), 
		nTab);
}

//void LogImage(int nTab, CImageBuffer* pImgBuf, LPCTSTR pMessageText, ...)
//{
//	LogImage(nTab, *pImgBuf, pMessageText);
//}


int AllocImageBuffer(CImageBuffer *ImgBuf, CSize ImgSize)
{
	return ImgBuf->Allocate(AbsSize(ImgSize));
}

int FreeImgBuf(CImageBuffer *ImgBuf)
{
	return ImgBuf->Free();
}


int ValidateImageAndRect(CImageBuffer *pImg,
						 CRect* prc)
{
	if (!pImg) {
		return -1;
	}
	if (!pImg->IsAllocated()) {
		return -1;
	}
	if (ValidateRect(prc,&(pImg->GetImgSize()))) {
		return -1;
	}
	return 0;
}

int GetImgXLine(CImageBuffer *pImg, CPoint* pptStart, int nSize, BYTE *pbyteX)
{
	int x;
	int nImageWidth,nImageHeight,nImagePitch;
	BYTE *pCol;
	BYTE *pbyteCrntIdx;
	CPoint ptEnd;

	if (!pImg) {
		ASSERT(!"pImg is a bad Pointer");
		return -1;
	}
	if (!pImg->IsAllocated()) {
		ASSERT(!"pImg is not Allocated");
		return -1;
	}
	if (nSize < 0) {
		ASSERT(!"Size of data is Negative");
		return -1;
	}
	if (CheckPoint(pptStart, &pImg->GetImgSize())) {
		ASSERT(!"Starting Point is not in the area of image");
		return -1;
	}
	ptEnd = CPoint(pptStart->x + nSize,pptStart->y);
	if (CheckPoint(&ptEnd, &pImg->GetImgSize())) {
		ASSERT(!"Line is out of the area of image");
		return -1;
	}
	nImageWidth=pImg->GetImgSize().cx;
	nImageHeight=pImg->GetImgSize().cy;
	nImagePitch=pImg->GetPitch();
	pCol=pImg->GetImgPtr()+nImagePitch*pptStart->y+pptStart->x;
	for (x=0,pbyteCrntIdx=pbyteX;x<nSize;x++,pbyteCrntIdx++,pCol++) {
		*pbyteCrntIdx = *pCol;
	}
	return 0;
}

int GetImgYLine(CImageBuffer *pImg, CPoint* pptStart, int nSize, BYTE *pbyteY)
{
	int y;
	int nImageWidth,nImageHeight,nImagePitch;
	BYTE *pRow;
	BYTE *pbyteCrntIdx;
	CPoint ptEnd;

	if (!pImg) {
		ASSERT(!"pImg is a bad Pointer");
		return -1;
	}
	if (!pImg->IsAllocated()) {
		ASSERT(!"pImg is not Allocated");
		return -1;
	}
	if (nSize < 0) {
		ASSERT(!"Size of data is Negative");
		return -1;
	}
	if (CheckPoint(pptStart, &pImg->GetImgSize())) {
		ASSERT(!"Starting Point is not in the area of image");
		return -1;
	}
	ptEnd = CPoint(pptStart->x, pptStart->y + nSize);
	if (CheckPoint(&ptEnd, &pImg->GetImgSize())) {
		ASSERT(!"Line is out of the area of image");
		return -1;
	}

	nImageWidth=pImg->GetImgSize().cx;
	nImageHeight=pImg->GetImgSize().cy;
	nImagePitch=pImg->GetPitch();

	pRow=pImg->GetImgPtr()+nImagePitch*pptStart->y+pptStart->x;
	for (y=0,pbyteCrntIdx=pbyteY;y<nSize;y++,pRow+=nImagePitch,pbyteCrntIdx++) {
		*pbyteCrntIdx = *pRow;
	}
	return 0;
}

int GetXGrayAccl(CImageBuffer *pImg, CRect *prectROI, long *plXAry)
{
	int x,y;
	int nImageWidth,nImageHeight,nImagePitch;
	int nProjWidth,nProjHeight;
	BYTE *pRow,*pCol;
	long *plCrntIdx;

	if (!pImg) {
		ASSERT(!"pImg is a bad Pointer");
		return -1;
	}
	if (!pImg->IsAllocated()) {
		ASSERT(!"pImg is not Allocated");
		return -1;
	}
	if (ValidateRect(prectROI,&(pImg->GetImgSize()))) {
		ASSERT(!"prectROI is Invalid");
		return -1;
	}

	nImageWidth=pImg->GetImgSize().cx;
	nImageHeight=pImg->GetImgSize().cy;
	nImagePitch=pImg->GetPitch();
	nProjWidth=prectROI->Width();
	nProjHeight=prectROI->Height();
	if(nProjHeight == 0) {
		memset((void *)plXAry,0,nProjWidth*sizeof(long));
		return 0;
	}
	if(nProjHeight != 1) {
		memset((void *)plXAry,0,nProjWidth*sizeof(long));
		pRow=pCol=pImg->GetImgPtr()+nImagePitch*prectROI->top+prectROI->left;
		for (y=0;y<nProjHeight;y++,pRow+=nImagePitch) {
			for (x=0,pCol=pRow,plCrntIdx=plXAry;x<nProjWidth;x++,plCrntIdx++,pCol++) {
				*plCrntIdx+=*pCol;
			}
		}
	}
	else
	{
		pRow=pCol=pImg->GetImgPtr()+nImagePitch*prectROI->top+prectROI->left;
		for (x=0,pCol=pRow,plCrntIdx=plXAry;x<nProjWidth;x++,plCrntIdx++,pCol++) {
			*plCrntIdx = *pCol;
		}
	}
	return 0;
}

//Assembly Code
int GetXGrayAcclASM(CImageBuffer *pImg, CRect *prectROI, long *plXAry)
{
	AfxMessageBox("GetXGrayAcclASM Not Supported");
	return -1;

/*	int x,y;
	int nImageWidth2, nImageHeight2,nImagePitch;
	int nProjWidth2,nProjHeight2; 
	BYTE *pRow2,*pCol2;
	long *plCrntIdx2;

	if (!pImg) {
		ASSERT(!"pImg is a bad Pointer");
		return -1;
	}
	if (!pImg->IsAllocated()) {
		ASSERT(!"pImg is not Allocated");
		return -1;
	}
	if (ValidateRect(prectROI,&(pImg->GetImgSize()))) {
		ASSERT(!"prectROI is Invalid");
		return -1;
	}

	nImageWidth2=pImg->GetImgSize().cx;
	nImageHeight2=pImg->GetImgSize().cy;
	nImagePitch=pImg->GetPitch();
	nProjWidth2=prectROI->Width();
	nProjHeight2=prectROI->Height();
	memset((void *)plXAry,0,nProjWidth2*sizeof(long));
	pRow2=pCol2=pImg->GetImgPtr()+nImagePitch*prectROI->top+prectROI->left;
	for (y=0;y<nProjHeight2;y++,pRow2+=nImagePitch) {
		for (x=0,pCol2=pRow2,plCrntIdx2=plXAry;x<nProjWidth2/2;x++,plCrntIdx2+=2,pCol2+=2) {
			_asm {
				mov eax, plCrntIdx2
				mov ebx,  pCol2
				movq mm0, [eax]
				movq mm1, [ebx]
				punpcklbw mm2, mm1
				punpcklbw mm3, mm2
				psrld mm3, 24
				paddd mm0, mm3
				movq [eax], mm0
			}
		}
	}
	_asm {
		emms
	}
	return 0;
	*/
}

int GetYGrayAccl(CImageBuffer *pImg, CRect *prectROI, long *plYAry)
{
	int x,y;
	int nImageWidth,nImageHeight,nImagePitch;
	int nProjWidth,nProjHeight;
	BYTE *pRow,*pCol;
	long *plCrntIdx;

	if (!pImg) {
		ASSERT(!"pImg is a bad Pointer");
		return -1;
	}
	if (!pImg->IsAllocated()) {
		ASSERT(!"pImg is not Allocated");
		return -1;
	}
	if (ValidateRect(prectROI,&(pImg->GetImgSize()))) {
		ASSERT(!"prectROI is Invalid");
		return -1;
	}
	nImageWidth=pImg->GetImgSize().cx;
	nImageHeight=pImg->GetImgSize().cy;
	nImagePitch=pImg->GetPitch();
	nProjWidth=prectROI->Width();
	nProjHeight=prectROI->Height();
	if(nProjWidth == 0) {
		memset((void *)plYAry,0,nProjHeight*sizeof(long));
		return 0;
	}
	if(nProjWidth != 1) {
		memset((void *)plYAry,0,nProjHeight*sizeof(long));
		pRow=pCol=pImg->GetImgPtr()+nImagePitch*prectROI->top+prectROI->left;
		for (y=0,plCrntIdx=plYAry;y<nProjHeight;y++,pRow+=nImagePitch,plCrntIdx++) {
			for (x=0,pCol=pRow;x<nProjWidth;x++,pCol++) {
				*plCrntIdx+=*pCol;
			}
		}
	}
	else
	{
		pRow=pImg->GetImgPtr()+nImagePitch*prectROI->top+prectROI->left;
		for (y=0,plCrntIdx=plYAry;y<nProjHeight;y++,pRow+=nImagePitch,plCrntIdx++) {
			*plCrntIdx = *pRow;
		}
	}
	return 0;
}

//Asembly Code
int GetYGrayAcclASM(CImageBuffer *pImg, CRect *prectROI, long *plYAry)
{
	AfxMessageBox("GetYGrayAcclASM Not Supported");
	return -1;

	/*int x,y;
	int nImageWidth2, nImageHeight2,nImagePitch;
	int nProjWidth2,nProjHeight2;
	BYTE *pRow2,*pCol2;
	long *plCrntIdx2;

	if (!pImg) {
		ASSERT(!"pImg is a bad Pointer");
		return -1;
	}
	if (!pImg->IsAllocated()) {
		ASSERT(!"pImg is not Allocated");
		return -1;
	}
	if (ValidateRect(prectROI,&(pImg->GetImgSize()))) {
		ASSERT(!"prectROI is Invalid");
		return -1;
	}

	nImageWidth2=pImg->GetImgSize().cx;
	nImageHeight2=pImg->GetImgSize().cy;
	nImagePitch=pImg->GetPitch();
	nProjWidth2=prectROI->Width();
	nProjHeight2=prectROI->Height();
	memset((void *)plYAry,0,nProjHeight2*sizeof(long));
	pRow2=pCol2=pImg->GetImgPtr()+nImagePitch*prectROI->left+prectROI->top;
	//Accumulating the data by Col
		for (y=0,plCrntIdx2=plYAry;y<nProjHeight2;y++,pRow2+=nImagePitch,plCrntIdx2++) {
			for (x=0,pCol2=pRow2;x<nProjWidth2;x++,pCol2++) {
			_asm {
				mov  eax, pCol2
				xor   ecx, ecx 
				movsx  cx, [eax]  //or mov cl, [eax] 
				mov  edx, plCrntIdx2
				add  ecx, [edx]
				mov  eax, plCrntIdx2
				mov  [eax], ecx 
			}
		}
	}
	return 0;
	*/
}

/*

int GetXYBinAccl(CImageBuffer *pImg, CRect *prectROI, int *pnXAry, int *pnYAry)
{
	AfxMessageBox("This function is under construction.\nPlease try aain later...");
	return -1;
}

*/

int GetHist(CImageBuffer *pImg, CRect *prectROI, long *plAry)
{
	int x,y;
	int nImageWidth,nImageHeight,nImagePitch;
	int nWidth,nHeight;
	BYTE *pRow,*pCol;

	if (!pImg) {
		ASSERT(!"pImg is a bad Pointer");
		return -1;
	}
	if (!pImg->IsAllocated()) {
		ASSERT(!"pImg is not Allocated");
		return -1;
	}
	if (ValidateRect(prectROI,&(pImg->GetImgSize()))) {
		ASSERT(!"prectROI is Invalid");
		return -1;
	}

	nImageWidth=pImg->GetImgSize().cx;
	nImageHeight=pImg->GetImgSize().cy;
	nImagePitch=pImg->GetPitch();
	nWidth=prectROI->Width();
	nHeight=prectROI->Height();
	pRow=pCol=pImg->GetImgPtr()+nImagePitch*prectROI->top+prectROI->left;
	memset((void *)plAry,0,256*sizeof(long));   //sizeof(long) = 4
	for (y=0;y<nHeight;y++,pRow+=nImagePitch) {
		for (x=0,pCol=pRow;x<nWidth;x++,pCol++) {
			(plAry[*pCol])++;
		}
	}
	return 0;
}

int GetMax(CImageBuffer *pImg, CRect *prectROI, BYTE* pbyteMax, CPoint2D* ppt2DMax)
{
	int x,y;
	int nImageWidth,nImageHeight,nImagePitch;
	int nWidth,nHeight;
	BYTE *pRow,*pCol;
	double dSumX;
	double dSumY;
	int nWeight;
	BYTE byteMax;

	*pbyteMax = 0;
	*ppt2DMax = CPoint2D(-1,-1);
	if (!pImg) {
		ASSERT(!"pImg is a bad Pointer");
		return -1;
	}
	if (!pImg->IsAllocated()) {
		ASSERT(!"pImg is not Allocated");
		return -1;
	}
	if (ValidateRect(prectROI,&(pImg->GetImgSize()))) {
		ASSERT(!"prectROI is Invalid");
		return -1;
	}

	nImageWidth=pImg->GetImgSize().cx;
	nImageHeight=pImg->GetImgSize().cy;
	nImagePitch=pImg->GetPitch();
	nWidth=prectROI->Width();
	nHeight=prectROI->Height();
	pRow=pCol=pImg->GetImgPtr()+nImagePitch*prectROI->top+prectROI->left;
	byteMax = 0;
	for (y=0;y<nHeight;y++,pRow+=nImagePitch) {
		for (x=0,pCol=pRow;x<nWidth;x++,pCol++) {
			if(byteMax < *pCol)
				byteMax = *pCol;
		}
	}
	dSumX = 0;
	dSumY = 0;
	nWeight = 0;
	pRow=pCol=pImg->GetImgPtr()+nImagePitch*prectROI->top+prectROI->left;
	for (y=0;y<nHeight;y++,pRow+=nImagePitch) {
		for (x=0,pCol=pRow;x<nWidth;x++,pCol++) {
			if(*pCol == byteMax)
			{
				dSumX = dSumX + *pCol * x;
				dSumY = dSumY + *pCol * y;
				nWeight = nWeight + *pCol;
			}
		}
	}
	if(nWeight > 0)
	{
		*ppt2DMax = CPoint2D(dSumX/nWeight + prectROI->left, dSumY/nWeight + prectROI->top);
	}
	*pbyteMax = byteMax;
	return 0;
}

int ImgClear(CImageBuffer *pImg, CRect *prectROI, BYTE byteVal)
{
	int x,y;
	int nImageWidth,nImageHeight,nImagePitch;
	int nWidth,nHeight;
	BYTE *pRow,*pCol;

	if (!pImg) {
		ASSERT(!"pImg is a bad Pointer");
		return -1;
	}
	if (!pImg->IsAllocated()) {
		ASSERT(!"pImg is not Allocated");
		return -1;
	}
	if (ValidateRect(prectROI,&(pImg->GetImgSize()))) {
		ASSERT(!"prectROI is Invalid");
		return -1;
	}

	nImageWidth=pImg->GetImgSize().cx;
	nImageHeight=pImg->GetImgSize().cy;
	nImagePitch=pImg->GetPitch();
	nWidth=prectROI->Width();
	nHeight=prectROI->Height();
	pRow=pCol=pImg->GetImgPtr()+nImagePitch*prectROI->top+prectROI->left;
	for (y=0;y<nHeight;y++,pRow+=nImagePitch) {
		for (x=0,pCol=pRow;x<nWidth;x++,pCol++) {
			*pCol = byteVal;
		}
	}
	return 0;
}


int GetSum(CImageBuffer *pImg, CRect *prectROI, long *plSum)
{
	int x,y;
	int nImageWidth,nImageHeight,nImagePitch;
	int nWidth,nHeight;
	BYTE *pRow,*pCol;
	long lSum;

	*plSum = 0;
	if (!pImg) {
		ASSERT(!"pImg is a bad Pointer");
		return -1;
	}
	if (!pImg->IsAllocated()) {
		ASSERT(!"pImg is not Allocated");
		return -1;
	}
	if (ValidateRect(prectROI,&(pImg->GetImgSize()))) {
		ASSERT(!"prectROI is Invalid");
		return -1;
	}

	nImageWidth=pImg->GetImgSize().cx;
	nImageHeight=pImg->GetImgSize().cy;
	nImagePitch=pImg->GetPitch();
	nWidth=prectROI->Width();
	nHeight=prectROI->Height();
	pRow=pCol=pImg->GetImgPtr()+nImagePitch*prectROI->top+prectROI->left;
	lSum = 0;
	for (y=0;y<nHeight;y++,pRow+=nImagePitch) {
		for (x=0,pCol=pRow;x<nWidth;x++,pCol++) {
			lSum += *pCol;
		}
	}
	*plSum = lSum;
	return 0;
}

long GetHistSum(long* plHist)
{
	int i;
	long lSum;

	lSum = 0;
	for(i=0;i<256;i++)
		lSum += plHist[i];
	return lSum;
}

//Asembly Code
int GetHistASM(CImageBuffer *pImg, CRect *prectROI, long *plAry)
{
	AfxMessageBox("GetHistASM not supported");
	return -1;

/*	int x,y;
	int nImageWidth2,nImageHeight2,nImagePitch;
	int nWidth2,nHeight2;
	BYTE *pRow2,*pCol2; 
	if (!pImg) {
		ASSERT(!"pImg is a bad Pointer");
		return -1;
	}
	if (!pImg->IsAllocated()) {
		ASSERT(!"pImg is not Allocated");
		return -1;
	}
	if (ValidateRect(prectROI,&(pImg->GetImgSize()))) {
		ASSERT(!"prectROI is Invalid");
		return -1;
	}

	nImageWidth2=pImg->GetImgSize().cx;
	nImageHeight2=pImg->GetImgSize().cy;
	nImagePitch=pImg->GetPitch();
	nWidth2=prectROI->Width();
	nHeight2=prectROI->Height();
	pRow2=pCol2=pImg->GetImgPtr()+nImagePitch*prectROI->top+prectROI->left;
	memset((void *)plAry,0,256*sizeof(long));
	for (y=0;y<nHeight2;y++,pRow2+=nImagePitch) {
		for (x=0,pCol2=pRow2;x<nWidth2;x++,pCol2++) {
			_asm {
				mov     eax, pCol2
				xor       ecx, ecx
				mov  cl, [eax]     //or mov cl, [eax]
				mov     edx, plAry 
				mov     eax, [edx+ecx*4]
				add     eax, 1
				mov     ecx, pCol2
				xor	      edx, edx
				mov  dl, [ecx]
				mov     ecx, plAry
				mov     [ecx+edx*4], eax 
			}
		}
	}
	return 0;
	*/
}

int Binarise(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 BYTE nThresVal)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if (prcSrc->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc Size different from prcDst Size");
		return -1;
	}

	if (nThresVal<0 && nThresVal>=256) {
		ASSERT(!"nThresVal Out Of Range");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;

	for (y=0; y<nHeightSrc; y++,pRowSrc+=nImgPitchSrc,pRowDst+=nImgPitchDst) {
		for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nWidthSrc; x++,pColSrc++,pColDst++) {
			if (*pColSrc>=nThresVal)
				*pColDst = 255;
			else
				*pColDst = 0;
		}
	}

	return 0;
}

int ImgTimes(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 int nTimes)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	int nVal;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if (prcSrc->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc Size different from prcDst Size");
		return -1;
	}

	if (nTimes<0 && nTimes>=256) {
		ASSERT(!"nTimes Out Of Range");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;

	for (y=0; y<nHeightSrc; y++,pRowSrc+=nImgPitchSrc,pRowDst+=nImgPitchDst) {
		for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nWidthSrc; x++,pColSrc++,pColDst++) {
			nVal = *pColSrc * nTimes;
			if(nVal > 255)
				*pColDst = 255;
			else
				*pColDst = nVal;
		}
	}

	return 0;
}

int Binarise(CImgIntBuf *pImgSrc,
			 CRect *prcSrc,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 int nThresVal)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	int *pRowSrc, *pColSrc;
	BYTE *pRowDst, *pColDst;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if (prcSrc->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc Size different from prcDst Size");
		return -1;
	}

	if (nThresVal<0 && nThresVal>=256) {
		ASSERT(!"nThresVal Out Of Range");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;

	for (y=0; y<nHeightSrc; y++,pRowSrc+=nImgPitchSrc,pRowDst+=nImgPitchDst) {
		for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nWidthSrc; x++,pColSrc++,pColDst++) {
			if (*pColSrc>=nThresVal)
				*pColDst = 255;
			else
				*pColDst = 0;
		}
	}

	return 0;
}


int Rot90Deg(CImageBuffer *pImgSrc,
			 CRect *prcSrc)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc/*,nWidthDst*/;
	BYTE *pSrc, *pDst;
	CImageBuffer ImgRot;
	CRect rcRot;
	CRect rcRotRelative;
	long lIndexRowSrc;
	long lIndexRowDst;
//	long lIndexDst;
	int nMinSize;

	nMinSize = __min(prcSrc->Width(), prcSrc->Height()) - 4;
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	rcRot = CRect(prcSrc->CenterPoint().x - nMinSize/2,
		prcSrc->CenterPoint().y - nMinSize/2,
		prcSrc->CenterPoint().x + nMinSize/2,
		prcSrc->CenterPoint().y + nMinSize/2);
	CheckRect(prcSrc, &rcRot);
	rcRotRelative = CRect(CPoint(0,0), rcRot.Size());
	ImgRot.Allocate(AbsSize(rcRotRelative.Size()));

				
	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = ImgRot.GetPitch();

	nWidthSrc = rcRotRelative.Width();
	nHeightSrc = rcRotRelative.Height();
	
	pSrc = pImgSrc->GetImgPtr();
	pDst = ImgRot.GetImgPtr();

	lIndexRowSrc = rcRot.top * nImgPitchSrc + rcRot.left;
	for (y=0; y<nHeightSrc; y++) {
		lIndexRowDst = nHeightSrc - y - 1;
		for (x=0; x<nWidthSrc; x++) {
			pDst[lIndexRowDst] = pSrc[lIndexRowSrc+x];
			lIndexRowDst+=nImgPitchDst;
		}
		lIndexRowSrc += nImgPitchSrc;
	}
	ImgCopy(&ImgRot,
				&rcRotRelative,
				pImgSrc,
				&rcRot);
	return 0;
}

int BinariseExt1(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 BYTE nThresVal,
		     int nInvert)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if (prcSrc->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc Size different from prcDst Size");
		return -1;
	}

	if (nThresVal<0 && nThresVal>=256) {
		ASSERT(!"nThresVal Out Of Range");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;

	if(nInvert)	{
		for (y=0; y<nHeightSrc; y++,pRowSrc+=nImgPitchSrc,pRowDst+=nImgPitchDst) {
			for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nWidthSrc; x++,pColSrc++,pColDst++) {
				if (*pColSrc<nThresVal)
					*pColDst = 255;
				else
					*pColDst = 0;
			}
		}
	}
	else {
		for (y=0; y<nHeightSrc; y++,pRowSrc+=nImgPitchSrc,pRowDst+=nImgPitchDst) {
			for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nWidthSrc; x++,pColSrc++,pColDst++) {
				if (*pColSrc>=nThresVal)
					*pColDst = 255;
				else
					*pColDst = 0;
			}
		}
	}
	return 0;
}


//Asembly code
int BinariseASM(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 BYTE nThresVal)
{
	AfxMessageBox("BinariseASM not supported");
	return -1;

/*	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc2,nHeightSrc2,nWidthDst2;
	BYTE *pRowSrc2, *pColSrc2, *pRowDst2, *pColDst2;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if (prcSrc->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc Size different from prcDst Size");
		return -1;
	}

	if (nThresVal<0 && nThresVal>=256) {
		ASSERT(!"nThresVal Out Of Range");
		return -1;
	}


	short bThresAry[8];
	short *pThresAry;
	for (x=0;x<8;x++) bThresAry[x] = nThresVal;
	pThresAry = &bThresAry[0];
	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc2 = prcSrc->Width();
	nHeightSrc2 = prcSrc->Height();
	nWidthDst2 = prcDst->Width();
	
	pRowSrc2 = pColSrc2 = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst2 = pColDst2 = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;

	for (y=0; y<nHeightSrc2; y++,pRowSrc2+=nImgPitchSrc,pRowDst2+=nImgPitchDst) {
		for (x=0,pColSrc2=pRowSrc2,pColDst2=pRowDst2; x<nWidthSrc2; x+=8,pColSrc2+=8,pColDst2+=8) {
			_asm {
				mov eax, pColSrc2
				mov ecx, pColDst2
				mov edx, pThresAry
				movq mm0, [eax]
				pxor   mm1,mm1
				movq  mm2,mm0
				movq mm3, [edx] 
				punpcklbw mm0, mm1   //pcmpgtb can't compare unsigned byte, so we need 
				punpckhbw mm2, mm1  //to unpack to word, then compare it. Before load into ColDst 
				pcmpgtw  mm0, mm3     //we need to comvert/pack it back by packsswb from signed
				pcmpgtw  mm2, mm3    //word to byte 
				packsswb mm0, mm2
				movq [ecx], mm0
				emms
			}
		}
	}

	return 0;
	*/
}

int BinariseExt(CImageBuffer *pImgSrc,
			    CRect *prcSrc,
			    CImageBuffer *pImgDst,
			    CRect *prcDst,
			    int nThresMin,
			    int nThresMax,
			    int nInvert)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if (prcSrc->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc Size different from prcDst Size");
		return -1;
	}

	if (nThresMin<0 && nThresMin>=256) {
		ASSERT(!"nThresMin Out Of Range");
		return -1;
	}

	if (nThresMax<0 && nThresMax>=256) {
		ASSERT(!"nThresMax Out Of Range");
		return -1;
	}

	if (nThresMin>nThresMax) {
		ASSERT(!"nThresMin > nThresMax");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;

	if (nInvert==0) {
		for (y=0; y<nHeightSrc; y++,pRowSrc+=nImgPitchSrc,pRowDst+=nImgPitchDst) {
			for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nWidthSrc; x++,pColSrc++,pColDst++) {
				if ((*pColSrc>=nThresMin) && (*pColSrc<=nThresMax))
					*pColDst = 255;
				else
					*pColDst = 0;
			}
		}
	}
	else {
		for (y=0; y<nHeightSrc; y++,pRowSrc+=nImgPitchSrc,pRowDst+=nImgPitchDst) {
			for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nWidthSrc; x++,pColSrc++,pColDst++) {
				if ((*pColSrc>=nThresMin) && (*pColSrc<=nThresMax))
					*pColDst = 0;
				else
					*pColDst = 255;
			}
		}
	}

	return 0;
}

int ImgInvert(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			CImageBuffer *pImgDst,
			CRect *prcDst)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}
	if (prcSrc->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc Size different from prcDst Size");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;

	for (y=0; y<nHeightSrc; y++,pRowSrc+=nImgPitchSrc,pRowDst+=nImgPitchDst) {
		for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nWidthSrc; x++,pColSrc++,pColDst++) {
			*pColDst = 255 - *pColSrc;
		}
	}

	return 0;
}

/*
int ImgCopy(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			CImageBuffer *pImgDst,
			CRect *prcDst)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}
	if (prcSrc->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc Size different from prcDst Size");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;

	for (y=0; y<nHeightSrc; y++,pRowSrc+=nImgPitchSrc,pRowDst+=nImgPitchDst) {
		for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nWidthSrc; x++,pColSrc++,pColDst++) {
			*pColDst = *pColSrc;
		}
	}

	return 0;
}
*/

///Udupa; 2012; Generic ImageCopy across color/gray image types
int ImgCopy(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			CImageBuffer *pImgDst,
			CRect *prcDst)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	int nIntensity;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}
	if (prcSrc->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc Size different from prcDst Size");
		return -1;
	}
	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	

	if(pImgSrc->GetBitCount() == pImgDst->GetBitCount()) {
		if(pImgSrc->GetBitCount() == 8) {
			pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
			pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;
			for(y=0; y<nHeightSrc; y++, pRowSrc+=nImgPitchSrc, pRowDst+=nImgPitchDst)
				memcpy(pRowDst,pRowSrc,nWidthSrc);
		}
		else {
			pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left * 3;
			pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left * 3;
			nWidthSrc *= 3;
			for(y=0; y<nHeightSrc; y++, pRowSrc+=nImgPitchSrc, pRowDst+=nImgPitchDst)
				memcpy(pRowDst, pRowSrc, nWidthSrc);
		}
	}
	else {
		if(pImgSrc->IsColor()) {
			pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left * 3;
			pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;
			for(y=0; y<nHeightSrc; y++, pRowSrc+=nImgPitchSrc, pRowDst+=nImgPitchDst) {
				for(x=0, pColSrc=pRowSrc, pColDst=pRowDst; x<nWidthSrc; x++, pColDst++) {
					nIntensity = *pColSrc;
					pColSrc++;
					nIntensity += *pColSrc;
					pColSrc++;
					nIntensity += *pColSrc;
					pColSrc++;
					*pColDst = nIntensity/3;
				}
			}
		}
		else {
			pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
			pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left * 3;
			for(y=0; y<nHeightSrc; y++, pRowSrc+=nImgPitchSrc, pRowDst+=nImgPitchDst) {
				for(x=0, pColSrc=pRowSrc, pColDst=pRowDst; x<nWidthSrc; x++,pColSrc++, pColDst+=3)
					memset(pColDst, *pColSrc, 3);
			}
		}
	}
	return 0;
}

//Asembly code
int ImgCopyASM(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			CImageBuffer *pImgDst,
			CRect *prcDst)
{
	AfxMessageBox("ImgCopyASM not supported");
	return -1;

/*	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc2,nHeightSrc2,nWidthDst2;
	BYTE *pRowSrc2, *pColSrc2, *pRowDst2, *pColDst2;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}
	if (prcSrc->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc Size different from prcDst Size");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc2 = prcSrc->Width();
	nHeightSrc2 = prcSrc->Height();
	nWidthDst2 = prcDst->Width();
	
	pRowSrc2 = pColSrc2 = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst2 = pColDst2 = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;

	for (y=0; y<nHeightSrc2; y++,pRowSrc2+=nImgPitchSrc,pRowDst2+=nImgPitchDst) {
		for (x=0,pColSrc2=pRowSrc2,pColDst2=pRowDst2; x<nWidthSrc2; x+=8,pColSrc2+=8,pColDst2+=8) {
			_asm
			{
				mov eax, pColDst2
				mov ecx, pColSrc2
				movq mm0, [ecx]
				movq [eax], mm0
				emms
			}
		}
	}

	return 0;
	*/
}


int ZoomImg(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			CImageBuffer *pImgDst,
			CPoint *pptTopLftDst,
			CSize* pcsZoom)
{
	int x,y, m, n;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if((pptTopLftDst->x < 0) || (pptTopLftDst->y < 0)) {
		ASSERT(!"pptTopLftDst is negative");
		return -1;
	}
	CRect rcZoom;
	rcZoom = CRect(pptTopLftDst->x,
				   pptTopLftDst->y,
				   pptTopLftDst->x + prcSrc->Width() * pcsZoom->cx,
				   pptTopLftDst->y + prcSrc->Height() * pcsZoom->cy);

	if (ValidateRect(&rcZoom,&(pImgDst->GetImgSize()))) {
		ASSERT(!"Dst Size is small");
		return -1;
	}

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * pptTopLftDst->y + pptTopLftDst->x;
	for (y=0; y<nHeightSrc; y++,pRowSrc+=nImgPitchSrc) {
		for(m=0; m < pcsZoom->cy; m++,pRowDst+=nImgPitchDst) {
			for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nWidthSrc; x++,pColSrc++) {
				for(n=0; n < pcsZoom->cx; n++,pColDst++) {
					*pColDst = *pColSrc;
				}
			}
		}
	}

	return 0;
}


int ClearImageROIEdge(CImageBuffer *pImgSrc,
				   CRect* prcROISrc,
				   BYTE byteLable)
{
	int x, y;
	BYTE *pImgPtr, *pImgTopLftPtr, *pImgProcTopLine, *pImgProcBotLine, *pImgProcLeftLine, *pImgProcRightLine;
	int nPitchSrc, nImgProcWidth, nImgProcHeight;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcROISrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"pParm->rcROISrc is Invalid");
		return -1;
	}

	pImgPtr = pImgSrc->GetImgPtr();
	nPitchSrc = pImgSrc->GetPitch();
	nImgProcWidth = prcROISrc->Width();
	nImgProcHeight = prcROISrc->Height();
	pImgTopLftPtr = pImgPtr + prcROISrc->top * nPitchSrc + prcROISrc->left;

	pImgProcLeftLine=pImgTopLftPtr;
	pImgProcRightLine=pImgTopLftPtr + nImgProcWidth - 1;
	for(y = 0; y < nImgProcHeight; y++, pImgProcLeftLine += nPitchSrc, pImgProcRightLine += nPitchSrc) {
		*pImgProcLeftLine = byteLable;
		*pImgProcRightLine = byteLable;
	}
	pImgProcTopLine = pImgTopLftPtr;
	pImgProcBotLine = pImgTopLftPtr + nPitchSrc * (nImgProcHeight - 1);
	for(x = 0; x < nImgProcWidth; x++) {
		*pImgProcTopLine++ = byteLable;
		*pImgProcBotLine++ = byteLable;
	}
	return 0;
}

int GetImgVal0Pos(CImageBuffer *pImg,
				  CRect *prc,
				  CPoint* ppt,
				  BOOL &bFound)
{
	int x,y;
	int nImgPitch;
	int nWidth,nHeight;
	BYTE *pRow, *pCol;
	bFound = FALSE;
	if (!pImg) {
		ASSERT(!"pImg is a bad Pointer");
		return -1;
	}
	if (!pImg->IsAllocated()) {
		ASSERT(!"pImg is not Allocated");
		return -1;
	}
	if (ValidateRect(prc,&(pImg->GetImgSize()))) {
		ASSERT(!"prc is Invalid");
		return -1;
	}
	nImgPitch = pImg->GetPitch();
	nWidth = prc->Width();
	nHeight = prc->Height();
	pRow = pCol = pImg->GetImgPtr() + nImgPitch * prc->top + prc->left;
	for (y=0; y<nHeight; y++,pRow+=nImgPitch) {
		for (x=0,pCol=pRow; x<nWidth; x++,pCol++) {
			if(*pCol == 0) {
				*ppt = CPoint(x,y);
				bFound = TRUE;
				return 0;
			}
		}
	}
	return 0;
}

int ImgMax(CImageBuffer *pImgSrc0,
		CRect *prcSrc0,
		CImageBuffer *pImgSrc1,
		CRect *prcSrc1,
		CImageBuffer *pImgDst,
		CRect *prcDst)
{
	int x,y;
	int nImgPitchSrc0,nImgHeightSrc0,nImgPitchSrc1,nImgHeightSrc1,nImgPitchDst,nImgHeightDst;
	int nWidthSrc0,nHeightSrc0,nWidthSrc1,nHeightSrc1,nWidthDst,nHeightDst;
	BYTE *pRowSrc0, *pColSrc0,*pRowSrc1, *pColSrc1, *pRowDst, *pColDst;

	if (!pImgSrc0) {
		ASSERT(!"pImgSrc0 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc0->IsAllocated()) {
		ASSERT(!"pImgSrc0 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc0,&(pImgSrc0->GetImgSize()))) {
		ASSERT(!"prcSrc0 is Invalid");
		return -1;
	}

	if (!pImgSrc1) {
		ASSERT(!"pImgSrc1 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc1->IsAllocated()) {
		ASSERT(!"pImgSrc1 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc1,&(pImgSrc1->GetImgSize()))) {
		ASSERT(!"prcSrc1 is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if ((prcSrc0->Size()!=prcDst->Size())||(prcSrc1->Size()!=prcDst->Size())) {
		ASSERT(!"prcSrc0 Size,prcSrc0 Size and prcDst Size have different Sizes");
		return -1;
	}

	nImgPitchSrc0 = pImgSrc0->GetPitch();
	nImgHeightSrc0 = pImgSrc0->GetImgSize().cy;
	nImgPitchSrc1 = pImgSrc1->GetPitch();
	nImgHeightSrc1 = pImgSrc1->GetImgSize().cy;
	nImgPitchDst = pImgDst->GetPitch();
	nImgHeightDst = pImgDst->GetImgSize().cy;

	nWidthSrc0 = prcSrc0->Width();
	nHeightSrc0 = prcSrc0->Height();
	nWidthSrc1 = prcSrc1->Width();
	nHeightSrc1 = prcSrc1->Height();
	nWidthDst = prcDst->Width();
	nHeightDst = prcDst->Height();
	
	pRowSrc0 = pColSrc0 = pImgSrc0->GetImgPtr() + nImgPitchSrc0 * prcSrc0->top + prcSrc0->left;
	pRowSrc1 = pColSrc1 = pImgSrc1->GetImgPtr() + nImgPitchSrc1 * prcSrc1->top + prcSrc1->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;
	
	for (y=0;y<nHeightDst;y++,pRowSrc0+=nImgPitchSrc0,pRowSrc1+=nImgPitchSrc1,pRowDst+=nImgPitchDst) 
	{
		for (x=0,pColSrc0=pRowSrc0,pColSrc1=pRowSrc1,pColDst=pRowDst;x<nWidthDst;x++,pColSrc0++,pColSrc1++,pColDst++) 
		{
			if(*pColSrc0 > *pColSrc1)
				*pColDst=*pColSrc0;
			else
				*pColDst=*pColSrc1;
		}
	}
	return 0;
}

int ImgMin(CImageBuffer *pImgSrc0,
		CRect *prcSrc0,
		CImageBuffer *pImgSrc1,
		CRect *prcSrc1,
		CImageBuffer *pImgDst,
		CRect *prcDst)
{
	int x,y;
	int nImgPitchSrc0,nImgHeightSrc0,nImgPitchSrc1,nImgHeightSrc1,nImgPitchDst,nImgHeightDst;
	int nWidthSrc0,nHeightSrc0,nWidthSrc1,nHeightSrc1,nWidthDst,nHeightDst;
	BYTE *pRowSrc0, *pColSrc0,*pRowSrc1, *pColSrc1, *pRowDst, *pColDst;

	if (!pImgSrc0) {
		ASSERT(!"pImgSrc0 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc0->IsAllocated()) {
		ASSERT(!"pImgSrc0 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc0,&(pImgSrc0->GetImgSize()))) {
		ASSERT(!"prcSrc0 is Invalid");
		return -1;
	}

	if (!pImgSrc1) {
		ASSERT(!"pImgSrc1 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc1->IsAllocated()) {
		ASSERT(!"pImgSrc1 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc1,&(pImgSrc1->GetImgSize()))) {
		ASSERT(!"prcSrc1 is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if ((prcSrc0->Size()!=prcDst->Size())||(prcSrc1->Size()!=prcDst->Size())) {
		ASSERT(!"prcSrc0 Size,prcSrc0 Size and prcDst Size have different Sizes");
		return -1;
	}

	nImgPitchSrc0 = pImgSrc0->GetPitch();
	nImgHeightSrc0 = pImgSrc0->GetImgSize().cy;
	nImgPitchSrc1 = pImgSrc1->GetPitch();
	nImgHeightSrc1 = pImgSrc1->GetImgSize().cy;
	nImgPitchDst = pImgDst->GetPitch();
	nImgHeightDst = pImgDst->GetImgSize().cy;

	nWidthSrc0 = prcSrc0->Width();
	nHeightSrc0 = prcSrc0->Height();
	nWidthSrc1 = prcSrc1->Width();
	nHeightSrc1 = prcSrc1->Height();
	nWidthDst = prcDst->Width();
	nHeightDst = prcDst->Height();
	
	pRowSrc0 = pColSrc0 = pImgSrc0->GetImgPtr() + nImgPitchSrc0 * prcSrc0->top + prcSrc0->left;
	pRowSrc1 = pColSrc1 = pImgSrc1->GetImgPtr() + nImgPitchSrc1 * prcSrc1->top + prcSrc1->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;
	
	for (y=0;y<nHeightDst;y++,pRowSrc0+=nImgPitchSrc0,pRowSrc1+=nImgPitchSrc1,pRowDst+=nImgPitchDst) 
	{
		for (x=0,pColSrc0=pRowSrc0,pColSrc1=pRowSrc1,pColDst=pRowDst;x<nWidthDst;x++,pColSrc0++,pColSrc1++,pColDst++) 
		{
			if(*pColSrc0 < *pColSrc1)
				*pColDst=*pColSrc0;
			else
				*pColDst=*pColSrc1;
		}
	}
	return 0;
}


int GetImgVal255Pos(CImageBuffer *pImg,
				  CRect *prc,
				  CPoint* ppt,
				  BOOL &bFound)
{
	int x,y;
	int nImgPitch;
	int nWidth,nHeight;
	BYTE *pRow, *pCol;
	bFound = FALSE;
	if (!pImg) {
		ASSERT(!"pImg is a bad Pointer");
		return -1;
	}
	if (!pImg->IsAllocated()) {
		ASSERT(!"pImg is not Allocated");
		return -1;
	}
	if (ValidateRect(prc,&(pImg->GetImgSize()))) {
		ASSERT(!"prc is Invalid");
		return -1;
	}
	nImgPitch = pImg->GetPitch();
	nWidth = prc->Width();
	nHeight = prc->Height();
	pRow = pCol = pImg->GetImgPtr() + nImgPitch * prc->top + prc->left;
	for (y=0; y<nHeight; y++,pRow+=nImgPitch) {
		for (x=0,pCol=pRow; x<nWidth; x++,pCol++) {
			if(*pCol == 255) {
				*ppt = CPoint(x,y);
				bFound = TRUE;
				return 0;
			}
		}
	}
	return 0;
}

int ResizeImageWithInterpolation_Slow(CImageBuffer *pImgSrc,
								CRect *prcSrc,
								CImageBuffer *pImgDst,
								CPoint *pptTopLftDst,
								CSize2D* pcsZoom)
{
	int x,y, m;
	int nImgPitchSrc, nImgPitchDst;
	int nWidthSrc, nHeightSrc, nWidthDst, nHeightDst;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	int nError;

	if(pImgSrc == NULL) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if(!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if(ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	if(pImgDst == NULL) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if(!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if((pptTopLftDst->x < 0) || (pptTopLftDst->y < 0)) {
		ASSERT(!"pptTopLftDst is negative");
		return -1;
	}
	CRect rcZoom = CRect(pptTopLftDst->x,
				   pptTopLftDst->y,
				   pptTopLftDst->x + (int) ceil(prcSrc->Width() * pcsZoom->cx),
				   pptTopLftDst->y + (int) ceil(prcSrc->Height() * pcsZoom->cy));

	if(ValidateRect(&rcZoom, &(pImgDst->GetImgSize()))) {
		ASSERT(!"Image Zoom With Interpolation: Destination image size is invalid");
		return -1;
	}

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();

	nWidthDst = rcZoom.Width();
	nHeightDst = rcZoom.Height();

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * pptTopLftDst->y + pptTopLftDst->x;

	CInterpolationBy2Pts interpolationBy2Pts;
	CPoint2D* pt2DEdge;

	// X Interpolation
	double dFactor = __min(((double) nWidthDst) / nWidthSrc, ((double) nHeightDst) / nHeightSrc);
	double dValue = 0;
	int nValue;
	pt2DEdge = new CPoint2D[nWidthSrc];
	double* pFactors = new double[nWidthSrc];
	for(x=0; x<nWidthSrc; x++)
		pFactors[x] = x * dFactor;

	for(y=0; y<nHeightSrc; y++, pRowSrc+=nImgPitchSrc, pRowDst+=nImgPitchDst) {
		for(x=0, pColSrc=pRowSrc; x<nWidthSrc; x++, pColSrc++)
			pt2DEdge[x] = CPoint2D(pFactors[x], *pColSrc);
		nError = interpolationBy2Pts.SetPoint2DAry(pt2DEdge, nWidthSrc);
		for(m=0, pColDst=pRowDst; m<nWidthDst; m++, pColDst++) {
			interpolationBy2Pts.GetY(m, &dValue);
			if(dValue < 0)
				nValue = 0;
			else if(dValue > 255)
				nValue = 255;
			else
				nValue = Round(dValue);
			*pColDst = nValue;
		}
	}
	delete[] pt2DEdge;
	delete[] pFactors;


	pRowSrc = pColSrc = pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * pptTopLftDst->y + pptTopLftDst->x;
///	dFactor = ((double) nHeightDst) / nHeightSrc;

	// Y Interpolation
	pt2DEdge = new CPoint2D[nHeightSrc];
	pFactors = new double[nHeightSrc];
	for(y=0; y<nHeightSrc; y++)
		pFactors[y] = y * dFactor;

	for(x=0; x<nWidthDst; x++, pColSrc++, pColDst++) {
		for(y=0, pRowSrc=pColSrc; y<nHeightSrc; y++, pRowSrc+=nImgPitchDst)
			pt2DEdge[y] = CPoint2D(pFactors[y], *pRowSrc);
		nError = interpolationBy2Pts.SetPoint2DAry(pt2DEdge, nHeightSrc);
		for(m=0, pRowDst=pColDst; m<nHeightDst; m++, pRowDst+=nImgPitchDst) {
			interpolationBy2Pts.GetY(m, &dValue);
			if(dValue < 0)
				nValue = 0;
			else if(dValue > 255)
				nValue = 255;
			else
				nValue = Round(dValue);
			*pRowDst = nValue;
		}
	}
	delete[] pt2DEdge;
	delete[] pFactors;

	return 0;
}

int ResizeImageWithInterpolation_Fast(CImageBuffer *pImgSrc,
								CRect *prcSrc,
								CImageBuffer *pImgDst,
								CPoint *pptTopLftDst,
								CSize2D* pcsZoom)
{
	int x,y;
	int nImgPitchSrc, nImgPitchDst;
	int nWidthSrc, nHeightSrc, nWidthDst, nHeightDst;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;

	if(pImgSrc == NULL) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if(!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if(ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	if(pImgDst == NULL) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if(!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if((pptTopLftDst->x < 0) || (pptTopLftDst->y < 0)) {
		ASSERT(!"pptTopLftDst is negative");
		return -1;
	}
	CRect rcZoom = CRect(pptTopLftDst->x,
				   pptTopLftDst->y,
				   pptTopLftDst->x + (int) ceil(prcSrc->Width() * pcsZoom->cx),
				   pptTopLftDst->y + (int) ceil(prcSrc->Height() * pcsZoom->cy));

	if(ValidateRect(&rcZoom, &(pImgDst->GetImgSize()))) {
		ASSERT(!"Image Zoom With Interpolation: Destination image size is invalid");
		return -1;
	}

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();

	nWidthDst = rcZoom.Width();
	nHeightDst = rcZoom.Height();

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	double dFactor = __min(((double) nWidthDst) / nWidthSrc, ((double) nHeightDst) / nHeightSrc);
	double dValue;
	int nValue;

	//X Interpolation
	int* pFactors = new int[nWidthDst];
	int* pPos1 = new int[nWidthDst];
	int* pPos2 = new int[nWidthDst];
	for(x=0; x<nWidthDst; x++) {
		dValue = x / dFactor;
		if(dValue > (nWidthSrc - 1))
			dValue = nWidthSrc - 1;
		nValue = ((int) dValue);
		pPos1[x] = nValue;
		if(nValue == (nWidthSrc - 1)) {
			pPos2[x] = nValue;
			pFactors[x] = 0;
		}
		else {
			pPos2[x] = nValue + 1;
			pFactors[x] = (int) ((dValue - nValue) * 1000000);
		}
	}

	pRowSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * pptTopLftDst->y + pptTopLftDst->x;

	for(y=0; y<nHeightSrc; y++, pRowSrc+=nImgPitchSrc, pRowDst+=nImgPitchDst) {
		for(x=0, pColDst=pRowDst; x<nWidthDst; x++, pColDst++) {
			//if(y==142 && x==1560)
			//	nValue=0;
			nValue = pRowSrc[pPos1[x]];
			*pColDst = (nValue + ((int) (((pRowSrc[pPos2[x]] - nValue) * pFactors[x])) / 1000000));
		}
	}
	delete[] pPos1;
	delete[] pPos2;
	delete[] pFactors;


	//Y Interpolation
	pFactors = new int[nHeightDst];
	pPos1 = new int[nHeightDst];
	pPos2 = new int[nHeightDst];
	for(y=0; y<nHeightDst; y++) {
		dValue = y / dFactor;

		if(dValue > (nHeightSrc - 1))
			dValue = nHeightSrc - 1;
		nValue = ((int) dValue);
		pPos1[y] = nValue;
		if(nValue == (nHeightSrc - 1)) {
			pPos2[y] = nValue;
			pFactors[y] = 0;
		}
		else {
			pPos2[y] = nValue + 1;
			pFactors[y] = (int) ((dValue - nValue) * 1000000);
		}
	}

	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * pptTopLftDst->y + pptTopLftDst->x;
	pColSrc = new BYTE[nHeightSrc];

	for(x=0; x<nWidthDst; x++, pColDst++) {
		for(y=0, pRowDst=pColDst; y<nHeightSrc; y++, pRowDst+=nImgPitchDst)
			pColSrc[y] = *pRowDst;

		for(y=0, pRowDst=pColDst; y<nHeightDst; y++, pRowDst+=nImgPitchDst) {
			nValue = pColSrc[pPos1[y]];
			*pRowDst = (nValue + ((((pColSrc[pPos2[y]] - nValue) * pFactors[y])) / 1000000));
		}
	}
	//OutputDebugLog("Fast: Round /10M");
	delete[] pColSrc;
	delete[] pPos1;
	delete[] pPos2;
	delete[] pFactors;

	return 0;
}

int ResizeImageWithInterpolation_Accurate(CImageBuffer *pImgSrc,
								CRect *prcSrc,
								CImageBuffer *pImgDst,
								CPoint *pptTopLftDst,
								CSize2D* pcsZoom)
{
	int x,y;
	int nImgPitchSrc, nImgPitchDst;
	int nWidthSrc, nHeightSrc, nWidthDst, nHeightDst;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;

	if(pImgSrc == NULL) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if(!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if(ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	if(pImgDst == NULL) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if(!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if((pptTopLftDst->x < 0) || (pptTopLftDst->y < 0)) {
		ASSERT(!"pptTopLftDst is negative");
		return -1;
	}
	CRect rcZoom = CRect(pptTopLftDst->x,
				   pptTopLftDst->y,
				   pptTopLftDst->x + (int) ceil(prcSrc->Width() * pcsZoom->cx),
				   pptTopLftDst->y + (int) ceil(prcSrc->Height() * pcsZoom->cy));

	if(ValidateRect(&rcZoom, &(pImgDst->GetImgSize()))) {
		ASSERT(!"Image Zoom With Interpolation: Destination image size is invalid");
		return -1;
	}

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();

	nWidthDst = rcZoom.Width();
	nHeightDst = rcZoom.Height();

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	double dFactor = __min(((double) nWidthDst) / nWidthSrc, ((double) nHeightDst) / nHeightSrc);
	double dValue;
	int nValue;

	//X Interpolation
	double* pFactors = new double[nWidthDst];
	int* pPos1 = new int[nWidthDst];
	int* pPos2 = new int[nWidthDst];
	for(x=0; x<nWidthDst; x++) {
		dValue = x / dFactor;
		if(dValue > (nWidthSrc - 1))
			dValue = nWidthSrc - 1;
		nValue = ((int) dValue);
		pPos1[x] = nValue;
		if(nValue == (nWidthSrc - 1)) {
			pPos2[x] = nValue;
			pFactors[x] = 0;
		}
		else {
			pPos2[x] = nValue + 1;
			pFactors[x] = dValue - nValue;
		}
	}

	CImageBuffer imgTemp;
	if(pImgSrc->GetImgSize().cx > pImgDst->GetImgSize().cx)
		imgTemp.Allocate(pImgSrc->GetImgSize());
	else
		imgTemp.Allocate(pImgDst->GetImgSize());
	int nImgPitchTemp = imgTemp.GetPitch();
	int nHeightTemp = imgTemp.GetImgSize().cy;

	pRowSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = imgTemp.GetImgPtr() + nImgPitchTemp * pptTopLftDst->y + pptTopLftDst->x;

	for(y=0; y<nHeightSrc; y++, pRowSrc+=nImgPitchSrc, pRowDst+=nImgPitchTemp) {
		for(x=0, pColDst=pRowDst; x<nWidthDst; x++, pColDst++) {
			nValue = pRowSrc[pPos1[x]];
			*pColDst = (int) (nValue + (pRowSrc[pPos2[x]] - nValue) * pFactors[x] + 0.5);
		}
	}
	delete[] pPos1;
	delete[] pPos2;
	delete[] pFactors;

	//Y Interpolation
	pFactors = new double[nHeightDst];
	pPos1 = new int[nHeightDst];
	pPos2 = new int[nHeightDst];
	for(y=0; y<nHeightDst; y++) {
		dValue = y / dFactor;

		if(dValue > (nHeightSrc - 1))
			dValue = nHeightSrc - 1;
		nValue = ((int) dValue);
		pPos1[y] = nValue;
		if(nValue == (nHeightSrc - 1)) {
			pPos2[y] = nValue;
			pFactors[y] = 0;
		}
		else {
			pPos2[y] = nValue + 1;
			pFactors[y] = dValue - nValue;
		}
	}

	pColSrc = imgTemp.GetImgPtr() + nImgPitchTemp * prcSrc->top + prcSrc->left;
	pColDst = pImgDst->GetImgPtr() + nImgPitchDst * pptTopLftDst->y + pptTopLftDst->x;
	BYTE* pCol = new BYTE[nHeightTemp];

	for(x=0; x<nWidthDst; x++, pColSrc++, pColDst++) {
		for(y=0, pRowSrc=pColSrc; y<nHeightTemp; y++, pRowSrc+=nImgPitchTemp)
			pCol[y] = *pRowSrc;

		for(y=0, pRowDst=pColDst; y<nHeightDst; y++, pRowDst+=nImgPitchDst) {
			nValue = pCol[pPos1[y]];
			*pRowDst = (int) (nValue + (pCol[pPos2[y]] - nValue) * pFactors[y] + 0.5);
		}
//			pColSrc[y] = *pRowDst;

//		for(y=0, pRowDst=pColDst; y<nHeightDst; y++, pRowDst+=nImgPitchDst) {
//			nValue = pColSrc[pPos1[y]];
//			*pRowDst = (int) (nValue + (pColSrc[pPos2[y]] - nValue) * pFactors[y] + 0.5);
//		}
	}
	//OutputDebugLog("Accurate: Rounding inline");
	delete[] pCol;
	delete[] pPos1;
	delete[] pPos2;
	delete[] pFactors;

	return 0;
}

int ResizeImage(CImageBuffer* pImgSrc, CImageBuffer* pImgDst, BOOL bAccurate)
{
	if(pImgSrc == NULL) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if(!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if(pImgDst == NULL) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if(!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}


	double dFactor = __min(((double) pImgDst->GetImgSize().cx) / pImgSrc->GetImgSize().cx, 
		((double) pImgDst->GetImgSize().cy) / pImgSrc->GetImgSize().cy);

	if(bAccurate)
		return ResizeImageWithInterpolation_Accurate(pImgSrc,
			&CRect(CPoint(0,0), pImgSrc->GetImgSize()),
			pImgDst,
			&CPoint(0,0),
			&CSize2D(dFactor, dFactor)
		);
	else
		return ResizeImageWithInterpolation_Fast(pImgSrc,
			&CRect(CPoint(0,0), pImgSrc->GetImgSize()),
			pImgDst,
			&CPoint(0,0),
			&CSize2D(dFactor, dFactor)
		);
}

int ImgConvertLUT(CImageBuffer *pImgSrc,
				  CRect *prcSrc,
			      CImageBuffer *pImgDst,
			      CRect *prcDst,
				  int *pLUT)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}
	if (prcSrc->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc Size different from prcDst Size");
		return -1;
	}

	for (x=0;x<256;x++) {
		if (pLUT[x]<0 && pLUT[x]>=256) {
			ASSERT(!"LUT Data Corrupted");
			return -1;
		}
	}
	
	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;

	for (y=0; y<nHeightSrc; y++,pRowSrc+=nImgPitchSrc,pRowDst+=nImgPitchDst) {
		for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nWidthSrc; x++,pColSrc++,pColDst++) {
			*pColDst = pLUT[*pColSrc];
		}
	}
	return 0;
}

int ImgSubtract(CImageBuffer *pImgSrc0,
				CRect *prcSrc0,
				CImageBuffer *pImgSrc1,
				CRect *prcSrc1,
				CImageBuffer *pImgDst,
				CRect *prcDst)
{
	int x,y;
	int nImgPitchSrc0,nImgHeightSrc0,nImgPitchSrc1,nImgHeightSrc1,nImgPitchDst,nImgHeightDst;
	int nWidthSrc0,nHeightSrc0,nWidthSrc1,nHeightSrc1,nWidthDst,nHeightDst;
	BYTE *pRowSrc0, *pColSrc0,*pRowSrc1, *pColSrc1, *pRowDst, *pColDst;

	if (!pImgSrc0) {
		ASSERT(!"pImgSrc0 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc0->IsAllocated()) {
		ASSERT(!"pImgSrc0 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc0,&(pImgSrc0->GetImgSize()))) {
		ASSERT(!"prcSrc0 is Invalid");
		return -1;
	}

	if (!pImgSrc1) {
		ASSERT(!"pImgSrc1 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc1->IsAllocated()) {
		ASSERT(!"pImgSrc1 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc1,&(pImgSrc1->GetImgSize()))) {
		ASSERT(!"prcSrc1 is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if ((prcSrc0->Size()!=prcDst->Size())||(prcSrc1->Size()!=prcDst->Size())) {
		ASSERT(!"prcSrc0 Size,prcSrc0 Size and prcDst Size have different Sizes");
		return -1;
	}

	nImgPitchSrc0 = pImgSrc0->GetPitch();
	nImgHeightSrc0 = pImgSrc0->GetImgSize().cy;
	nImgPitchSrc1 = pImgSrc1->GetPitch();
	nImgHeightSrc1 = pImgSrc1->GetImgSize().cy;
	nImgPitchDst = pImgDst->GetPitch();
	nImgHeightDst = pImgDst->GetImgSize().cy;

	nWidthSrc0 = prcSrc0->Width();
	nHeightSrc0 = prcSrc0->Height();
	nWidthSrc1 = prcSrc1->Width();
	nHeightSrc1 = prcSrc1->Height();
	nWidthDst = prcDst->Width();
	nHeightDst = prcDst->Height();
	
	pRowSrc0 = pColSrc0 = pImgSrc0->GetImgPtr() + nImgPitchSrc0 * prcSrc0->top + prcSrc0->left;
	pRowSrc1 = pColSrc1 = pImgSrc1->GetImgPtr() + nImgPitchSrc1 * prcSrc1->top + prcSrc1->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;
	
	for (y=0;y<nHeightDst;y++,pRowSrc0+=nImgPitchSrc0,pRowSrc1+=nImgPitchSrc1,pRowDst+=nImgPitchDst) 
	{
		for (x=0,pColSrc0=pRowSrc0,pColSrc1=pRowSrc1,pColDst=pRowDst;x<nWidthDst;x++,pColSrc0++,pColSrc1++,pColDst++) 
		{
			if(*pColSrc0>*pColSrc1) *pColDst=*pColSrc0-*pColSrc1;
			else *pColDst=0;
		}
	}
	return 0;
}

int ImgSubtractAbs(CImageBuffer *pImgSrc0,
				CRect *prcSrc0,
				CImageBuffer *pImgSrc1,
				CRect *prcSrc1,
				CImageBuffer *pImgDst,
				CRect *prcDst)
{
	int x,y;
	int nImgPitchSrc0,nImgHeightSrc0,nImgPitchSrc1,nImgHeightSrc1,nImgPitchDst,nImgHeightDst;
	int nWidthSrc0,nHeightSrc0,nWidthSrc1,nHeightSrc1,nWidthDst,nHeightDst;
	BYTE *pRowSrc0, *pColSrc0,*pRowSrc1, *pColSrc1, *pRowDst, *pColDst;

	if (!pImgSrc0) {
		ASSERT(!"pImgSrc0 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc0->IsAllocated()) {
		ASSERT(!"pImgSrc0 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc0,&(pImgSrc0->GetImgSize()))) {
		ASSERT(!"prcSrc0 is Invalid");
		return -1;
	}

	if (!pImgSrc1) {
		ASSERT(!"pImgSrc1 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc1->IsAllocated()) {
		ASSERT(!"pImgSrc1 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc1,&(pImgSrc1->GetImgSize()))) {
		ASSERT(!"prcSrc1 is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if ((prcSrc0->Size()!=prcDst->Size())||(prcSrc1->Size()!=prcDst->Size())) {
		ASSERT(!"prcSrc0 Size,prcSrc0 Size and prcDst Size have different Sizes");
		return -1;
	}

	nImgPitchSrc0 = pImgSrc0->GetPitch();
	nImgHeightSrc0 = pImgSrc0->GetImgSize().cy;
	nImgPitchSrc1 = pImgSrc1->GetPitch();
	nImgHeightSrc1 = pImgSrc1->GetImgSize().cy;
	nImgPitchDst = pImgDst->GetPitch();
	nImgHeightDst = pImgDst->GetImgSize().cy;

	nWidthSrc0 = prcSrc0->Width();
	nHeightSrc0 = prcSrc0->Height();
	nWidthSrc1 = prcSrc1->Width();
	nHeightSrc1 = prcSrc1->Height();
	nWidthDst = prcDst->Width();
	nHeightDst = prcDst->Height();
	
	pRowSrc0 = pColSrc0 = pImgSrc0->GetImgPtr() + nImgPitchSrc0 * prcSrc0->top + prcSrc0->left;
	pRowSrc1 = pColSrc1 = pImgSrc1->GetImgPtr() + nImgPitchSrc1 * prcSrc1->top + prcSrc1->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;
	
	for (y=0;y<nHeightDst;y++,pRowSrc0+=nImgPitchSrc0,pRowSrc1+=nImgPitchSrc1,pRowDst+=nImgPitchDst) 
	{
		for (x=0,pColSrc0=pRowSrc0,pColSrc1=pRowSrc1,pColDst=pRowDst;x<nWidthDst;x++,pColSrc0++,pColSrc1++,pColDst++) 
		{
			*pColDst=abs(*pColSrc0-*pColSrc1);
		}
	}
	return 0;
}

//Asembly code
int ImgSubtractASM(CImageBuffer *pImgSrc0,
				CRect *prcSrc0,
				CImageBuffer *pImgSrc1,
				CRect *prcSrc1,
				CImageBuffer *pImgDst,
				CRect *prcDst)
{
	AfxMessageBox("ImgSubtractASM not supported");
	return -1;

/*	int x,y;
	int nImgPitchSrc0MX,nImgHeightSrc0MX,nImgPitchSrc1MX,nImgHeightSrc1MX,nImgPitchDstMX,nImgHeightDstMX;
	int nWidthSrc0MX,nHeightSrc0MX,nWidthSrc1MX,nHeightSrc1MX,nWidthDstMX,nHeightDstMX;
	BYTE *pRowSrc0MX, *pColSrc0MX,*pRowSrc1MX, *pColSrc1MX, *pRowDstMX, *pColDstMX;

	if (!pImgSrc0) {
		ASSERT(!"pImgSrc0 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc0->IsAllocated()) {
		ASSERT(!"pImgSrc0 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc0,&(pImgSrc0->GetImgSize()))) {
		ASSERT(!"prcSrc0 is Invalid");
		return -1;
	}

	if (!pImgSrc1) {
		ASSERT(!"pImgSrc1 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc1->IsAllocated()) {
		ASSERT(!"pImgSrc1 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc1,&(pImgSrc1->GetImgSize()))) {
		ASSERT(!"prcSrc1 is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if ((prcSrc0->Size()!=prcDst->Size())||(prcSrc1->Size()!=prcDst->Size())) {
		ASSERT(!"prcSrc0 Size,prcSrc0 Size and prcDst Size have different Sizes");
		return -1;
	}

	nImgPitchSrc0MX = pImgSrc0->GetPitch();
	nImgHeightSrc0MX = pImgSrc0->GetImgSize().cy;
	nImgPitchSrc1MX = pImgSrc1->GetPitch();
	nImgHeightSrc1MX = pImgSrc1->GetImgSize().cy;
	nImgPitchDstMX = pImgDst->GetPitch();
	nImgHeightDstMX = pImgDst->GetImgSize().cy;

	nWidthSrc0MX = prcSrc0->Width();
	nHeightSrc0MX = prcSrc0->Height();
	nWidthSrc1MX = prcSrc1->Width();
	nHeightSrc1MX = prcSrc1->Height();
	nWidthDstMX = prcDst->Width();
	nHeightDstMX = prcDst->Height();
	
	pRowSrc0MX = pColSrc0MX = pImgSrc0->GetImgPtr() + nImgPitchSrc0MX * prcSrc0->top + prcSrc0->left;
	pRowSrc1MX = pColSrc1MX = pImgSrc1->GetImgPtr() + nImgPitchSrc1MX * prcSrc1->top + prcSrc1->left;
	pRowDstMX = pColDstMX = pImgDst->GetImgPtr() + nImgPitchDstMX * prcDst->top + prcDst->left;
	
	for (y=0;y<nHeightDstMX;y++,pRowSrc0MX+=nImgPitchSrc0MX,pRowSrc1MX+=nImgPitchSrc1MX,pRowDstMX+=nImgPitchDstMX) 
	{
		for (x=0,pColSrc0MX=pRowSrc0MX,pColSrc1MX=pRowSrc1MX,pColDstMX=pRowDstMX;x<nWidthDstMX;x=x+8,pColSrc0MX=pColSrc0MX+8,pColSrc1MX=pColSrc1MX+8,pColDstMX=pColDstMX+8) 
		{
			_asm {
//				mov eax, pColSrc0MX
//				mov ebx, pColSrc1MX
//				mov ecx, pColDstMX
//				movq mm0, [eax]
//				movq mm1, [ebx]
//				movq mm2, mm1
//				pcmpgtw mm1, mm0
//				pand mm0, mm1
//				psubusw mm0, mm2
//				movq [ecx], mm0
//				emms
//something wrong with the pcmpgtw, but haven't verify yet
				mov eax, pColSrc0MX
				mov ebx, pColSrc1MX
				mov ecx, pColDstMX
				movq mm0, [eax]
				movq mm1, [ebx]
				movq mm2, mm0
				movq mm3, mm1
				pxor mm4, mm4
				punpcklbw mm0, mm4
				punpckhbw mm2, mm4
				punpcklbw mm1, mm4
				punpckhbw mm3, mm4
				pcmpgtw mm0, mm1
				pcmpgtw mm2, mm3
				packsswb mm0, mm2
				movq mm2, [eax]
				movq mm1, [ebx] 
				pand mm2, mm0
				psubusb  mm2, mm1
				movq [ecx], mm2
				emms
			}
		}
	}
	return 0;
	*/
}

int ImgAND(CImageBuffer *pImgSrc0,
				CRect *prcSrc0,
				CImageBuffer *pImgSrc1,
				CRect *prcSrc1,
				CImageBuffer *pImgDst,
				CRect *prcDst)
{
	int x,y;
	int nImgPitchSrc0,nImgHeightSrc0,nImgPitchSrc1,nImgHeightSrc1,nImgPitchDst,nImgHeightDst;
	int nWidthSrc0,nHeightSrc0,nWidthSrc1,nHeightSrc1,nWidthDst,nHeightDst;
	BYTE *pRowSrc0, *pColSrc0,*pRowSrc1, *pColSrc1, *pRowDst, *pColDst;

	if (!pImgSrc0) {
		ASSERT(!"pImgSrc0 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc0->IsAllocated()) {
		ASSERT(!"pImgSrc0 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc0,&(pImgSrc0->GetImgSize()))) {
		ASSERT(!"prcSrc0 is Invalid");
		return -1;
	}

	if (!pImgSrc1) {
		ASSERT(!"pImgSrc1 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc1->IsAllocated()) {
		ASSERT(!"pImgSrc1 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc1,&(pImgSrc1->GetImgSize()))) {
		ASSERT(!"prcSrc1 is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if ((prcSrc0->Size()!=prcDst->Size())||(prcSrc1->Size()!=prcDst->Size())) {
		ASSERT(!"prcSrc0 Size,prcSrc0 Size and prcDst Size have different Sizes");
		return -1;
	}

	nImgPitchSrc0 = pImgSrc0->GetPitch();
	nImgHeightSrc0 = pImgSrc0->GetImgSize().cy;
	nImgPitchSrc1 = pImgSrc1->GetPitch();
	nImgHeightSrc1 = pImgSrc1->GetImgSize().cy;
	nImgPitchDst = pImgDst->GetPitch();
	nImgHeightDst = pImgDst->GetImgSize().cy;

	nWidthSrc0 = prcSrc0->Width();
	nHeightSrc0 = prcSrc0->Height();
	nWidthSrc1 = prcSrc1->Width();
	nHeightSrc1 = prcSrc1->Height();
	nWidthDst = prcDst->Width();
	nHeightDst = prcDst->Height();
	
	pRowSrc0 = pColSrc0 = pImgSrc0->GetImgPtr() + nImgPitchSrc0 * prcSrc0->top + prcSrc0->left;
	pRowSrc1 = pColSrc1 = pImgSrc1->GetImgPtr() + nImgPitchSrc1 * prcSrc1->top + prcSrc1->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;
	
	if(OptimizeCpu()) {
		CudaIf()->ImgAND(pRowSrc0, nImgPitchSrc0,
			pRowSrc1, nImgPitchSrc1,
			pRowDst, nImgPitchDst,
			nWidthDst,
			nHeightDst);
	}
	else {
		for (y=0;y<nHeightDst;y++,pRowSrc0+=nImgPitchSrc0,pRowSrc1+=nImgPitchSrc1,pRowDst+=nImgPitchDst) {
			for (x=0,pColSrc0=pRowSrc0,pColSrc1=pRowSrc1,pColDst=pRowDst;x<nWidthDst;x++,pColSrc0++,pColSrc1++,pColDst++) {
				*pColDst=*pColSrc0&*pColSrc1;
			}
		}
	}
	return 0;
}



int ImgOR(CImageBuffer *pImgSrc0,
				CRect *prcSrc0,
				CImageBuffer *pImgSrc1,
				CRect *prcSrc1,
				CImageBuffer *pImgDst,
				CRect *prcDst)
{
	int x,y;
	int nImgPitchSrc0,nImgHeightSrc0,nImgPitchSrc1,nImgHeightSrc1,nImgPitchDst,nImgHeightDst;
	int nWidthSrc0,nHeightSrc0,nWidthSrc1,nHeightSrc1,nWidthDst,nHeightDst;
	BYTE *pRowSrc0, *pColSrc0,*pRowSrc1, *pColSrc1, *pRowDst, *pColDst;

	if (!pImgSrc0) {
		ASSERT(!"pImgSrc0 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc0->IsAllocated()) {
		ASSERT(!"pImgSrc0 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc0,&(pImgSrc0->GetImgSize()))) {
		ASSERT(!"prcSrc0 is Invalid");
		return -1;
	}

	if (!pImgSrc1) {
		ASSERT(!"pImgSrc1 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc1->IsAllocated()) {
		ASSERT(!"pImgSrc1 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc1,&(pImgSrc1->GetImgSize()))) {
		ASSERT(!"prcSrc1 is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if ((prcSrc0->Size()!=prcDst->Size())||(prcSrc1->Size()!=prcDst->Size())) {
		ASSERT(!"prcSrc0 Size,prcSrc0 Size and prcDst Size have different Sizes");
		return -1;
	}

	nImgPitchSrc0 = pImgSrc0->GetPitch();
	nImgHeightSrc0 = pImgSrc0->GetImgSize().cy;
	nImgPitchSrc1 = pImgSrc1->GetPitch();
	nImgHeightSrc1 = pImgSrc1->GetImgSize().cy;
	nImgPitchDst = pImgDst->GetPitch();
	nImgHeightDst = pImgDst->GetImgSize().cy;

	nWidthSrc0 = prcSrc0->Width();
	nHeightSrc0 = prcSrc0->Height();
	nWidthSrc1 = prcSrc1->Width();
	nHeightSrc1 = prcSrc1->Height();
	nWidthDst = prcDst->Width();
	nHeightDst = prcDst->Height();
	
	pRowSrc0 = pColSrc0 = pImgSrc0->GetImgPtr() + nImgPitchSrc0 * prcSrc0->top + prcSrc0->left;
	pRowSrc1 = pColSrc1 = pImgSrc1->GetImgPtr() + nImgPitchSrc1 * prcSrc1->top + prcSrc1->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;
	
	///Udupa; Feb'2014
	if(OptimizeCpu()) {
		CudaIf()->ImgOR(pRowSrc0, nImgPitchSrc0,
			pRowSrc1, nImgPitchSrc1,
			pRowDst, nImgPitchDst,
			nWidthDst,
			nHeightDst);
	}
	else {
		for (y=0;y<nHeightDst;y++,pRowSrc0+=nImgPitchSrc0,pRowSrc1+=nImgPitchSrc1,pRowDst+=nImgPitchDst) {
			for (x=0,pColSrc0=pRowSrc0,pColSrc1=pRowSrc1,pColDst=pRowDst;x<nWidthDst;x++,pColSrc0++,pColSrc1++,pColDst++) 
				*pColDst=*pColSrc0|*pColSrc1;
		}
	}
	return 0;
}

int ImgXOR(CImageBuffer *pImgSrc0,
				CRect *prcSrc0,
				CImageBuffer *pImgSrc1,
				CRect *prcSrc1,
				CImageBuffer *pImgDst,
				CRect *prcDst)
{
	int x,y;
	int nImgPitchSrc0,nImgHeightSrc0,nImgPitchSrc1,nImgHeightSrc1,nImgPitchDst,nImgHeightDst;
	int nWidthSrc0,nHeightSrc0,nWidthSrc1,nHeightSrc1,nWidthDst,nHeightDst;
	BYTE *pRowSrc0, *pColSrc0,*pRowSrc1, *pColSrc1, *pRowDst, *pColDst;

	if (!pImgSrc0) {
		ASSERT(!"pImgSrc0 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc0->IsAllocated()) {
		ASSERT(!"pImgSrc0 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc0,&(pImgSrc0->GetImgSize()))) {
		ASSERT(!"prcSrc0 is Invalid");
		return -1;
	}

	if (!pImgSrc1) {
		ASSERT(!"pImgSrc1 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc1->IsAllocated()) {
		ASSERT(!"pImgSrc1 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc1,&(pImgSrc1->GetImgSize()))) {
		ASSERT(!"prcSrc1 is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if ((prcSrc0->Size()!=prcDst->Size())||(prcSrc1->Size()!=prcDst->Size())) {
		ASSERT(!"prcSrc0 Size,prcSrc0 Size and prcDst Size have different Sizes");
		return -1;
	}

	nImgPitchSrc0 = pImgSrc0->GetPitch();
	nImgHeightSrc0 = pImgSrc0->GetImgSize().cy;
	nImgPitchSrc1 = pImgSrc1->GetPitch();
	nImgHeightSrc1 = pImgSrc1->GetImgSize().cy;
	nImgPitchDst = pImgDst->GetPitch();
	nImgHeightDst = pImgDst->GetImgSize().cy;

	nWidthSrc0 = prcSrc0->Width();
	nHeightSrc0 = prcSrc0->Height();
	nWidthSrc1 = prcSrc1->Width();
	nHeightSrc1 = prcSrc1->Height();
	nWidthDst = prcDst->Width();
	nHeightDst = prcDst->Height();
	
	pRowSrc0 = pColSrc0 = pImgSrc0->GetImgPtr() + nImgPitchSrc0 * prcSrc0->top + prcSrc0->left;
	pRowSrc1 = pColSrc1 = pImgSrc1->GetImgPtr() + nImgPitchSrc1 * prcSrc1->top + prcSrc1->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;
	
	for (y=0;y<nHeightDst;y++,pRowSrc0+=nImgPitchSrc0,pRowSrc1+=nImgPitchSrc1,pRowDst+=nImgPitchDst) 
	{
		for (x=0,pColSrc0=pRowSrc0,pColSrc1=pRowSrc1,pColDst=pRowDst;x<nWidthDst;x++,pColSrc0++,pColSrc1++,pColDst++) 
		{
			if(*pColSrc0!=*pColSrc1)
				*pColDst=255;
			else
				*pColDst=0;
		}
	}
	return 0;
}


int ImgSum(CImageBuffer *pImgBufSrc,
		   CRect* prcSrc,
		   BYTE byteThreshold,
		   long* plIntensitySum,
		   long* plIntensityNo,
		   BOOL bBLACK)
{
	int x,y;
	int nImgPitchSrc;
	int nWidth, nHeight;
	BYTE *pRowSrc, *pColSrc;
	long lSum, lWeight;

	*plIntensitySum = 0;
	*plIntensityNo = 0;

	if (!pImgBufSrc) {
		ASSERT(!"pImgBufSrc is a bad Pointer");
		return -1;
	}
	if (!pImgBufSrc->IsAllocated()) {
		ASSERT(!"pImgBufSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgBufSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	nImgPitchSrc = pImgBufSrc->GetPitch();
	nWidth = prcSrc->Width();
	nHeight = prcSrc->Height();
	
	pRowSrc = pColSrc = pImgBufSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	lSum = 0;
	lWeight = 0;
	if(bBLACK) {
		for (y=0;y<nHeight;y++,pRowSrc+=nImgPitchSrc) {
			for (x=0,pColSrc=pRowSrc;x<nWidth;x++,pColSrc++) {
				if(*pColSrc <= byteThreshold) {
					lSum += *pColSrc;
					lWeight++;
				}
			}
		}
	}
	else {
		for (y=0;y<nHeight;y++,pRowSrc+=nImgPitchSrc) {
			for (x=0,pColSrc=pRowSrc;x<nWidth;x++,pColSrc++) {
				if(*pColSrc >= byteThreshold) {
					lSum += *pColSrc;
					lWeight++;
				}
			}
		}
	}
	*plIntensitySum = lSum;
	*plIntensityNo = lWeight;
	return 0;
}


int ImgADD(CImageBuffer *pImgSrc0,
				CRect *prcSrc0,
				CImageBuffer *pImgSrc1,
				CRect *prcSrc1,
				CImageBuffer *pImgDst,
				CRect *prcDst)
{
	int x,y;
	int nSum;
	int nImgPitchSrc0,nImgHeightSrc0,nImgPitchSrc1,nImgHeightSrc1,nImgPitchDst,nImgHeightDst;
	int nWidthSrc0,nHeightSrc0,nWidthSrc1,nHeightSrc1,nWidthDst,nHeightDst;
	BYTE *pRowSrc0, *pColSrc0,*pRowSrc1, *pColSrc1, *pRowDst, *pColDst;

	if (!pImgSrc0) {
		ASSERT(!"pImgSrc0 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc0->IsAllocated()) {
		ASSERT(!"pImgSrc0 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc0,&(pImgSrc0->GetImgSize()))) {
		ASSERT(!"prcSrc0 is Invalid");
		return -1;
	}

	if (!pImgSrc1) {
		ASSERT(!"pImgSrc1 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc1->IsAllocated()) {
		ASSERT(!"pImgSrc1 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc1,&(pImgSrc1->GetImgSize()))) {
		ASSERT(!"prcSrc1 is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if ((prcSrc0->Size()!=prcDst->Size())||(prcSrc1->Size()!=prcDst->Size())) {
		ASSERT(!"prcSrc0 Size,prcSrc0 Size and prcDst Size have different Sizes");
		return -1;
	}

	nImgPitchSrc0 = pImgSrc0->GetPitch();
	nImgHeightSrc0 = pImgSrc0->GetImgSize().cy;
	nImgPitchSrc1 = pImgSrc1->GetPitch();
	nImgHeightSrc1 = pImgSrc1->GetImgSize().cy;
	nImgPitchDst = pImgDst->GetPitch();
	nImgHeightDst = pImgDst->GetImgSize().cy;

	nWidthSrc0 = prcSrc0->Width();
	nHeightSrc0 = prcSrc0->Height();
	nWidthSrc1 = prcSrc1->Width();
	nHeightSrc1 = prcSrc1->Height();
	nWidthDst = prcDst->Width();
	nHeightDst = prcDst->Height();
	
	pRowSrc0 = pColSrc0 = pImgSrc0->GetImgPtr() + nImgPitchSrc0 * prcSrc0->top + prcSrc0->left;
	pRowSrc1 = pColSrc1 = pImgSrc1->GetImgPtr() + nImgPitchSrc1 * prcSrc1->top + prcSrc1->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;
	
	for (y=0;y<nHeightDst;y++,pRowSrc0+=nImgPitchSrc0,pRowSrc1+=nImgPitchSrc1,pRowDst+=nImgPitchDst) {
		for (x=0,pColSrc0=pRowSrc0,pColSrc1=pRowSrc1,pColDst=pRowDst;x<nWidthDst;x++,pColSrc0++,pColSrc1++,pColDst++) {
			nSum = *pColSrc0 + *pColSrc1;
			if(nSum > 255)
				*pColDst= 255;
			else
				*pColDst= nSum;
		}
	}
	return 0;
}

int ImgADD(CImageBuffer *pImgSrc0,
			CRect *prcSrc0,
			int nVal,
			CImageBuffer *pImgDst,
			CRect *prcDst)
{
	int x,y;
	int nSum;
	int nImgPitchSrc0,nImgHeightSrc0,nImgPitchDst,nImgHeightDst;
	int nWidthSrc0,nHeightSrc0,nWidthDst,nHeightDst;
	BYTE *pRowSrc0, *pColSrc0, *pRowDst, *pColDst;

	if (!pImgSrc0) {
		ASSERT(!"pImgSrc0 is a bad Pointer");
		return -1;
	}
	if (!pImgSrc0->IsAllocated()) {
		ASSERT(!"pImgSrc0 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc0,&(pImgSrc0->GetImgSize()))) {
		ASSERT(!"prcSrc0 is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if (prcSrc0->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc0 Size and prcDst Size have different Sizes");
		return -1;
	}

	nImgPitchSrc0 = pImgSrc0->GetPitch();
	nImgHeightSrc0 = pImgSrc0->GetImgSize().cy;
	nImgPitchDst = pImgDst->GetPitch();
	nImgHeightDst = pImgDst->GetImgSize().cy;

	nWidthSrc0 = prcSrc0->Width();
	nHeightSrc0 = prcSrc0->Height();
	nWidthDst = prcDst->Width();
	nHeightDst = prcDst->Height();
	
	pRowSrc0 = pColSrc0 = pImgSrc0->GetImgPtr() + nImgPitchSrc0 * prcSrc0->top + prcSrc0->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;
	if(nVal >= 0) {
		for (y=0;y<nHeightDst;y++,pRowSrc0+=nImgPitchSrc0,pRowDst+=nImgPitchDst) {
			for (x=0,pColSrc0=pRowSrc0,pColDst=pRowDst;x<nWidthDst;x++,pColSrc0++,pColDst++) {
				nSum = *pColSrc0 + nVal;
				if(nSum > 255)
					*pColDst= 255;
				else
					*pColDst= nSum;
			}
		}
	}
	else {
		for (y=0;y<nHeightDst;y++,pRowSrc0+=nImgPitchSrc0,pRowDst+=nImgPitchDst) {
			for (x=0,pColSrc0=pRowSrc0,pColDst=pRowDst;x<nWidthDst;x++,pColSrc0++,pColDst++) {
				nSum = *pColSrc0 + nVal;
				if(nSum < 0)
					*pColDst= 0;
				else
					*pColDst= nSum;
			}
		}
	}
	return 0;
}


int ImgRotate(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 CPoint2D* ppt2DRotCenterSrc,
			 double dAngleSrc,
			 BOOL bAccurate)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	int nCosAngleTimes10000, nSinAngleTimes10000;
	BYTE *pRowDst, *pColDst;
	CRect rcImgSrcDeflateBy1;
	CPoint2D pt2DRotCenterDst;
	BYTE byteVal0, byteVal1, byteVal2, byteVal3;
	BYTE byteWeightX,byteWeightY;
	BYTE byteWeightX1,byteWeightY1;
	double dRotX, dRotY;
	long lIndex;
	double dDstOffsetX, dDstOffsetY;
	BYTE* pImgSrcPtr;


	pt2DRotCenterDst.x = ppt2DRotCenterSrc->x - prcSrc->TopLeft().x + prcDst->TopLeft().x;
	pt2DRotCenterDst.y = ppt2DRotCenterSrc->y - prcSrc->TopLeft().y + prcDst->TopLeft().y;
	nCosAngleTimes10000 = (int)(10000 * cos(dAngleSrc));
	nSinAngleTimes10000 = (int)(10000 * sin(dAngleSrc));

	rcImgSrcDeflateBy1 = CRect(1, 1, pImgSrc->GetImgSize().cx - 1, pImgSrc->GetImgSize().cy -1);
	
	if(ValidateRect(&rcImgSrcDeflateBy1) == -1)
		return -1;
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if (prcSrc->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc Size different from prcDst Size");
		return -1;
	}

	pImgSrcPtr = pImgSrc->GetImgPtr();
	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;

bAccurate = true; // 
//bAccurate = false will be completed soon.
	if(bAccurate) {
		dDstOffsetX = prcDst->left - pt2DRotCenterDst.x;
		dDstOffsetY = prcDst->top - pt2DRotCenterDst.y;
		for (y=0; y<nHeightSrc; y++,pRowDst+=nImgPitchDst) {
			for (x=0,pColDst=pRowDst; x<nWidthSrc; x++,pColDst++) {
				dRotX = (nCosAngleTimes10000 * (x + dDstOffsetX) + nSinAngleTimes10000 * (y + dDstOffsetY))/10000.0 + ppt2DRotCenterSrc->x;
				dRotY = (-nSinAngleTimes10000 * (x + dDstOffsetX) + nCosAngleTimes10000 * (y + dDstOffsetY))/10000.0 + ppt2DRotCenterSrc->y;
				if(ValidatePoint(&CPoint2D(dRotX, dRotY), &rcImgSrcDeflateBy1) == 0) {
					lIndex = ((int)dRotY) * nImgPitchSrc + (int)(dRotX);	
					byteVal0 = pImgSrcPtr[lIndex];
					byteVal1 = pImgSrcPtr[lIndex + 1];
					byteVal2 = pImgSrcPtr[lIndex + nImgPitchSrc];
					byteVal3 = pImgSrcPtr[lIndex + nImgPitchSrc + 1];
					byteWeightX1 = (BYTE)((dRotX - (int)dRotX) * 100);
					byteWeightY1 = (BYTE)((dRotY - (int)dRotY) * 100);
					byteWeightX = 100 - byteWeightX1;
					byteWeightY = 100 - byteWeightY1;
					*pColDst = ((byteWeightX * byteVal0 + byteWeightX1 * byteVal1) * byteWeightY +
											(byteWeightX * byteVal2 + byteWeightX1 * byteVal3) * byteWeightY1)/10000;
				}
				else
					*pColDst = 0;
			}
		}
	}
	return 0;
}

int ImgSampling(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			CImageBuffer *pImgDst,
			CPoint* pptTopLftDst,
			CSize *pcsSampling)
{
	int x, y;
	int nImgPitchSrc, nImgPitchDst;
	int nWidthSrc, nHeightSrc;
	long lSkipPitch,lSkipY,lSkipX;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	CRect rcDst;
	CRect rcTol;
//	CPerformanceTimer Timer;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	
	if ((pcsSampling->cx<1)||((pcsSampling->cy<1))) {
		ASSERT(!"pcsSampling is Invalid");
		return -1;
	}

	rcDst=CRect(pptTopLftDst->x,
				pptTopLftDst->y ,
				pptTopLftDst->x + prcSrc->Width() / pcsSampling->cx,
				pptTopLftDst->y + prcSrc->Height() / pcsSampling->cy);

	if (ValidateRect(&rcDst,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"The ImgSize of pImgSource is smaller than required Or The ImgSize of pImgDestination is bigger than required");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();

	lSkipPitch=nImgPitchSrc*pcsSampling->cy;
	lSkipX=pcsSampling->cx;
	lSkipY=pcsSampling->cy;

	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * pptTopLftDst->y + pptTopLftDst->x;

	for (y=0; y< rcDst.Height(); y++,pRowSrc+=lSkipPitch,pRowDst+=nImgPitchDst) {
		for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x< rcDst.Width(); x++,pColSrc+=lSkipX,pColDst++) {
			*pColDst = *pColSrc;
		}
	}
//	OutputDebugLogTo(0,FALSE,"Host ImgCopy Looping Time=%3.3f",Timer.GetElapsedMSec());

	return 0;
}


int ImgSampling(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			CImageBuffer *pImgDst,
			CRect *prcDst,
			CSize *pcsSampling)
{
	int x, y;
	int nImgPitchSrc, nImgPitchDst;
	int nWidthSrc, nHeightSrc;
	long lSkipPitch,lSkipY,lSkipX;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	CRect rcTol;
//	CPerformanceTimer Timer;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}
	
	if ((pcsSampling->cx<1)||((pcsSampling->cy<1))) {
		ASSERT(!"pcsSampling is Invalid");
		return -1;
	}

	rcTol=CRect(prcSrc->left,
				prcSrc->top ,
				prcSrc->left + prcDst->Width() * pcsSampling->cx,
				prcSrc->top + prcDst->Height() * pcsSampling->cy);

	if (ValidateRect(&rcTol,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"The ImgSize of pImgSource is smaller than required Or The ImgSize of pImgDestination is bigger than required");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();

	lSkipPitch=nImgPitchSrc*pcsSampling->cy;
	lSkipX=pcsSampling->cx;
	lSkipY=pcsSampling->cy;

	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;

	for (y=0; y< prcDst->Height(); y++,pRowSrc+=lSkipPitch,pRowDst+=nImgPitchDst) {
		for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x< prcDst->Width(); x++,pColSrc+=lSkipX,pColDst++) {
			*pColDst = *pColSrc;
		}
	}
//	OutputDebugLogTo(0,FALSE,"Host ImgCopy Looping Time=%3.3f",Timer.GetElapsedMSec());

	return 0;
}

int ImgSampling(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			CImageBuffer *pImgDst,
			CPoint* pptTopLftDst,
			CSize *pcsSampling,
			int nProcMode)//Min:-1; Avg:0 Max:1
{
	int x, y, k;
	int nImgPitchSrc, nImgPitchDst;
	int nWidthSrc, nHeightSrc;
	long lSkipPitch,lSkipY,lSkipX;
	BYTE *pSrc, *pDst;
	int nRowSrcIndex;
	int nSrcIndex;
	int nRowDstIndex;
	CRect rcDst;
	CRect rcTol;
	int* pnTabIndex;
	int nTabIndexNo;
	BYTE byteMin;
	BYTE byteMax;

	pnTabIndex = NULL;
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	
	if ((pcsSampling->cx<1)||((pcsSampling->cy<1))) {
		ASSERT(!"pcsSampling is Invalid");
		return -1;
	}

	rcDst=CRect(pptTopLftDst->x,
				pptTopLftDst->y ,
				pptTopLftDst->x + prcSrc->Width() / pcsSampling->cx,
				pptTopLftDst->y + prcSrc->Height() / pcsSampling->cy);

	if (ValidateRect(&rcDst,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"The ImgSize of pImgSource is smaller than required Or The ImgSize of pImgDestination is bigger than required");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();

	lSkipPitch=nImgPitchSrc*pcsSampling->cy;
	lSkipX=pcsSampling->cx;
	lSkipY=pcsSampling->cy;

	nTabIndexNo = pcsSampling->cx * pcsSampling->cy;
	pnTabIndex = new int[abs(nTabIndexNo)];
	nTabIndexNo = 0;
	for(y=0;y<pcsSampling->cy;y++) {
		for(x=0;x<pcsSampling->cx;x++) {
			pnTabIndex[nTabIndexNo++] = y*nImgPitchSrc + x;
		}
	}

	pSrc = pImgSrc->GetImgPtr();
	pDst = pImgDst->GetImgPtr();


	nRowSrcIndex = nImgPitchSrc * prcSrc->top + prcSrc->left;
	nRowDstIndex = nImgPitchDst * pptTopLftDst->y + pptTopLftDst->x;
	switch (nProcMode) {
	case -1:
		for (y=0; y< rcDst.Height(); y++) {
			for (x=0; x< rcDst.Width(); x++) {
				nSrcIndex = nRowSrcIndex + x * lSkipX;
				byteMin = pSrc[nSrcIndex + pnTabIndex[0]];
				for(k=1;k<nTabIndexNo;k++) {
					if(byteMin > pSrc[nSrcIndex + pnTabIndex[k]])
						byteMin = pSrc[nSrcIndex + pnTabIndex[k]];
				}
				pDst[nRowDstIndex + x] = byteMin;
			}
			nRowSrcIndex += lSkipPitch;
			nRowDstIndex += nImgPitchDst;
		}
		break;
	case 0:
		int nSum;
		for (y=0; y< rcDst.Height(); y++) {
			for (x=0; x< rcDst.Width(); x++) {
				nSrcIndex = nRowSrcIndex + x * lSkipX;
				nSum = pSrc[nSrcIndex + pnTabIndex[0]];
				for(k=1;k<nTabIndexNo;k++) {
					nSum += pSrc[nSrcIndex + pnTabIndex[k]];
				}
				pDst[nRowDstIndex + x] = nSum/nTabIndexNo;
			}
			nRowSrcIndex += lSkipPitch;
			nRowDstIndex += nImgPitchDst;
		}
		break;
	case 1:
		for (y=0; y< rcDst.Height(); y++) {
			for (x=0; x< rcDst.Width(); x++) {
				nSrcIndex = nRowSrcIndex + x * lSkipX;
				byteMax = pSrc[nSrcIndex + pnTabIndex[0]];
				for(k=1;k<nTabIndexNo;k++) {
					if(byteMax < pSrc[nSrcIndex + pnTabIndex[k]])
						byteMax = pSrc[nSrcIndex + pnTabIndex[k]];
				}
				pDst[nRowDstIndex + x] = byteMax;
			}
			nRowSrcIndex += lSkipPitch;
			nRowDstIndex += nImgPitchDst;
		}
		break;
	}

	if(pnTabIndex != NULL)
		delete [] pnTabIndex;
	return 0;
}

int ImgAvgSampling(CImageBuffer *pImgSrc,
				CRect *prcSrc,
				CImageBuffer *pImgDst,
				CRect *prcDst,
				CSize *pcsSampling)
{
	int x, y, k;
	long lSum;
	int nImgPitchSrc, nImgPitchDst;
	int nWidthSrc, nHeightSrc;
	long lSkipPitch, lSkipY, lSkipX;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	CRect rcTol;
	long *lDir;
	int nDirection;
//	CPerformanceTimer Timer;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}
	
	if ((pcsSampling->cx<1)||((pcsSampling->cy<1))) {
		ASSERT(!"pcsSampling is Invalid");
		return -1;
	}

	rcTol=CRect(prcSrc->left,
				prcSrc->top ,
				prcSrc->left + prcDst->Width() * pcsSampling->cx,
				prcSrc->top + prcDst->Height() * pcsSampling->cy);

	if (ValidateRect(&rcTol,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"The ImgSize of pImgSource is smaller than required Or The ImgSize of pImgDestination is bigger than required");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();

	lSkipPitch=nImgPitchSrc*pcsSampling->cy;
	lSkipX=pcsSampling->cx;
	lSkipY=pcsSampling->cy;

	lDir = new long [abs(lSkipX * lSkipY)];
	nDirection=0;
	for(y=0;y<lSkipY;y++) {
		for(x=0;x<lSkipX;x++) {
			lDir[nDirection++]=y*nImgPitchSrc+x;
		}
	}

	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;

	for (y=0; y<prcDst->Height(); y++,pRowSrc+=lSkipPitch,pRowDst+=nImgPitchDst) {
		for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<prcDst->Width(); x++,pColSrc+=lSkipX,pColDst++) {
			lSum = 0;
			for(k = 0; k < nDirection; k++) {
				lSum += *(pColSrc + lDir[k]);
			}
			*pColDst = (BYTE) (lSum / nDirection);
		}
	}
//	OutputDebugLogTo(0,FALSE,"Host ImgCopy Looping Time=%3.3f",Timer.GetElapsedMSec());
	delete [] lDir;
	return 0;
}

BOOL Dilation1D(BYTE *pbyteSrc,
				int nSizeSrc,
				int nBlobType,
				BYTE *pbyteDst,
				int nSizeDst, 
				double *pdMask,
				int nMask, 	
				long lProcMode)
{
	int i,j;
	int nIndexStart, nIndexEnd;
	for(i = 0; i < nSizeDst; i++) pbyteDst[i] = pbyteSrc[i];
	if(nBlobType == WHITE) {
		if(lProcMode == 0)
		{
			for(i = 1; i < nSizeDst; i++)	{
				// find transition position
				if(pbyteSrc[i-1] != pbyteSrc[i]) {
					// do dilation forward
					if(pbyteSrc[i] == 0) {
						nIndexStart = i;
						nIndexEnd = i + nMask;
						if(nIndexEnd > nSizeDst) nIndexEnd = nSizeDst;
						for(j = nIndexStart; j < nIndexEnd; j++) pbyteDst[j] = 255;
					}
					else {
						nIndexEnd = i;
						nIndexStart = i - nMask;
						if(nIndexStart < 0) nIndexStart = 0;
						for(j = nIndexStart; j < nIndexEnd; j++) pbyteDst[j] = 255;
					}
				}
			}
		}
	}
	else {
		if(lProcMode==0) {
			for(i = 1; i < nSizeDst; i++)	{
				// find transition position
				if(pbyteSrc[i-1] != pbyteSrc[i]) {
					// do dilation forward
					if(pbyteSrc[i] == 0) {
						nIndexStart = i - nMask;
						nIndexEnd = i;
						if(nIndexStart < 0) nIndexStart = 0;
						for(j = nIndexStart; j < nIndexEnd; j++) pbyteDst[j] = 0;
					}
					else {
						nIndexEnd = i + nMask;
						nIndexStart = i;
						if(nIndexEnd > nSizeDst) nIndexEnd = nSizeDst;
						for(j = nIndexStart; j < nIndexEnd; j++) pbyteDst[j] = 0;
					}
				}
			}
		}
	}
	return 0;
}

BOOL Erosion1D(BYTE *pbyteSrc, 	
			   int nSizeSrc, 	
			   int nBlobType,
			   BYTE *pbyteDst,  
			   int nSizeDst,  	
			   double *pdMask,  
			   int nMask, 		
			   long lProcMode)	
{
	int i, j;
	int nIndexStart, nIndexEnd;
	if(nSizeSrc < (2 * nMask)) {
		if(nBlobType == WHITE) {
			for(i = 0; i < nSizeDst; i++) 
				pbyteDst[i] = 0;
		}
		else {
			for(i = 0; i < nSizeDst; i++) 
				pbyteDst[i] = 255;
		}
		return 0;
	}
	for(i = 0; i < nSizeDst; i++) pbyteDst[i] = pbyteSrc[i];
	if(nBlobType == WHITE) {
		if(lProcMode==0) {
			for(i = 1; i < nSizeDst; i++)	{
				// find transition position
				if(pbyteSrc[i-1] != pbyteSrc[i]) {
					// do dilation forward
					if(pbyteSrc[i] == 0) {
						nIndexStart = i - nMask;
						nIndexEnd = i;
						if(nIndexStart < 0) nIndexStart = 0;
						for(j = nIndexStart; j < nIndexEnd; j++) pbyteDst[j] = 0;
					}
					else {
						nIndexEnd = i + nMask;
						nIndexStart = i;
						if(nIndexEnd > nSizeDst) nIndexEnd = nSizeDst;
						for(j = nIndexStart; j < nIndexEnd; j++) pbyteDst[j] = 0;
					}
				}
			}
			for(i = 0; i < nMask; i++) {
				pbyteDst[i] = 0;
				pbyteDst[nSizeDst - i - 1] = 0;
			}
		}
	}
	else {
		if(lProcMode == 0)
		{
			for(i = 1; i < nSizeDst; i++)	{
				// find transition position
				if(pbyteSrc[i-1] != pbyteSrc[i]) {
					// do dilation forward
					if(pbyteSrc[i] == 0) {
						nIndexStart = i;
						nIndexEnd = i + nMask;
						if(nIndexEnd > nSizeDst) nIndexEnd = nSizeDst;
						for(j = nIndexStart; j < nIndexEnd; j++) pbyteDst[j] = 255;
					}
					else {
						nIndexEnd = i;
						nIndexStart = i - nMask;
						if(nIndexStart < 0) nIndexStart = 0;
						for(j = nIndexStart; j < nIndexEnd; j++) pbyteDst[j] = 255;
					}
				}
			}
			for(i = 0; i < nMask; i++) {
				pbyteDst[i] = 255;
				pbyteDst[nSizeDst - i - 1] = 255;
			}
		}
	}
	return 0;
}

BOOL ImgDilation(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
			 int nBlobType,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 double *pdMask,
			 CSize *pszMask,
			 long lProcMode)
{
	int i, j;
	BYTE *pbyteSrc;
	int nSizeSrc;
	BYTE *pbyteDst;
	int nSizeDst;
	int nMask;

	long lSrcRowIndex, lDstRowIndex;
	int nSrcStartX, nSrcStartY, nSrcEndX, nSrcEndY;
	int nDstStartX, nDstStartY, nDstEndX, nDstEndY;
	int nSrcPitch, nDstPitch;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}
	if (prcSrc->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc Size different from prcDst Size");
		return -1;
	}

	nSrcPitch = pImgSrc->GetPitch();
	nSrcStartX = prcSrc->left;
	nSrcStartY = prcSrc->top;
	nSrcEndX = prcSrc->right;
	nSrcEndY = prcSrc->bottom;

	nDstPitch = pImgDst->GetPitch();
	nDstStartX = prcDst->left;
	nDstStartY = prcDst->top;
	nDstEndX = prcDst->right;
	nDstEndY = prcDst->bottom;

	if(lProcMode == 0)
	{
		nMask = pszMask->cx;
		nSizeSrc = prcSrc->Width();
		nSizeDst = prcDst->Width();
		pbyteSrc = new BYTE[abs(nSizeSrc)];
		pbyteDst = new BYTE[abs(nSizeDst)];
		lSrcRowIndex = nSrcStartY*nSrcPitch+nSrcStartX;
		lDstRowIndex = nDstStartY*nDstPitch+nDstStartX;
		for(i = nSrcStartY; i < nSrcEndY; i++) {
			// get one line of image to process
			for(j = 0; j < nSizeSrc; j++)
				pbyteSrc[j] = pImgSrc->GetImgPtr()[lSrcRowIndex+j];
			Dilation1D(pbyteSrc, nSizeSrc, nBlobType, pbyteDst, nSizeDst, pdMask, nMask, lProcMode);
			for(j = 0; j < nSizeDst; j++)
				pImgDst->GetImgPtr()[lDstRowIndex+j] = pbyteDst[j];
			lSrcRowIndex += nSrcPitch;
			lDstRowIndex += nDstPitch;
		}
		delete [] pbyteSrc;
		delete [] pbyteDst;

		nMask = pszMask->cy;
		nSizeDst = prcDst->Height();
		nSizeSrc = nSizeDst;
		pbyteSrc = new BYTE[abs(nSizeSrc)];
		pbyteDst = new BYTE[abs(nSizeDst)];
		for(i = 0; i < prcDst->Width(); i++) {
			// get one line of image to process
			lDstRowIndex = nDstStartY * nDstPitch + nDstStartX + i;
			lSrcRowIndex = lDstRowIndex;
			for(j = 0; j < nSizeSrc; j++) {
				pbyteSrc[j] = pImgDst->GetImgPtr()[lSrcRowIndex];
				lSrcRowIndex += nDstPitch;
			}
			Dilation1D(pbyteSrc, nSizeSrc, nBlobType, pbyteDst, nSizeDst, pdMask, nMask, lProcMode);
			for(j = 0; j < nSizeDst; j++) {
				pImgDst->GetImgPtr()[lDstRowIndex] = pbyteDst[j];
				lDstRowIndex += nDstPitch;
			}
		}
		delete [] pbyteSrc;
		delete [] pbyteDst;

	}
	return 0;
}

BOOL ImgErosion(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
			 int nBlobType,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 double *pdMask,
			 CSize *pszMask,
			 long lProcMode)
{
	int i, j;
	BYTE *pbyteSrc;
	int nSizeSrc;
	BYTE *pbyteDst;
	int nSizeDst;
	int nMask;
	long lSrcRowIndex, lDstRowIndex;
	int nSrcStartX, nSrcStartY, nSrcEndX, nSrcEndY;
	int nDstStartX, nDstStartY, nDstEndX, nDstEndY;
	int nSrcPitch, nDstPitch;

	nSrcPitch = pImgSrc->GetPitch();
	nSrcStartX = prcSrc->left;
	nSrcStartY = prcSrc->top;
	nSrcEndX = prcSrc->right;
	nSrcEndY = prcSrc->bottom;

	nDstPitch = pImgDst->GetPitch();
	nDstStartX = prcDst->left;
	nDstStartY = prcDst->top;
	nDstEndX = prcDst->right;
	nDstEndY = prcDst->bottom;

	if(lProcMode == 0)
	{
		nMask = pszMask->cx;
		nSizeSrc = prcSrc->Width();
		nSizeDst = prcDst->Width();
		pbyteSrc = new BYTE[abs(nSizeSrc)];
		pbyteDst = new BYTE[abs(nSizeDst)];
		lSrcRowIndex = nSrcStartY * nSrcPitch + nSrcStartX;
		lDstRowIndex = nDstStartY * nDstPitch + nDstStartX;
		for(i = nSrcStartY; i < nSrcEndY; i++) {
			// get one line of image to process
			for(j = 0; j < nSizeSrc; j++)
				pbyteSrc[j] = pImgSrc->GetImgPtr()[lSrcRowIndex+j];
			Erosion1D(pbyteSrc, nSizeSrc, nBlobType, pbyteDst, nSizeDst, pdMask, nMask, lProcMode);
			for(j = 0; j < nSizeDst; j++)
				pImgDst->GetImgPtr()[lDstRowIndex + j] = pbyteDst[j];
			lSrcRowIndex += nSrcPitch;
			lDstRowIndex += nDstPitch;
		}
		delete [] pbyteSrc;
		delete [] pbyteDst;

		nMask = pszMask->cy;
		nSizeDst = prcDst->Height();
		nSizeSrc = nSizeDst;
		pbyteSrc = new BYTE[abs(nSizeSrc)];
		pbyteDst = new BYTE[abs(nSizeDst)];
		for(i = 0; i < prcDst->Width(); i++) {
			// get one line of image to process
			lDstRowIndex = nDstStartY * nDstPitch + nDstStartX + i;
			lSrcRowIndex = lDstRowIndex;
			for(j = 0; j < nSizeSrc; j++) {
				pbyteSrc[j] = pImgDst->GetImgPtr()[lSrcRowIndex];
				lSrcRowIndex += nDstPitch;
			}
			Erosion1D(pbyteSrc, nSizeSrc, nBlobType, pbyteDst, nSizeDst, pdMask, nMask, lProcMode);
			for(j = 0; j < nSizeDst; j++) {
				pImgDst->GetImgPtr()[lDstRowIndex] = pbyteDst[j];
				lDstRowIndex += nDstPitch;
			}
		}
		delete [] pbyteSrc;
		delete [] pbyteDst;
	}
	return 0;
}


int ImgBinDilation(CImageBuffer *pImgSrc,
				 CRect *prcSrc,
				 int nBlobType,
				 CImageBuffer *pImgDst,
				 CRect *prcDst,
				 CSize *pszMask)
{
	int i, j, m;
	long lSrcRowIndex, lDstRowIndex;
	long lDstIndex;
	int nSrcStartX, nSrcStartY, nSrcEndX, nSrcEndY;
	int nDstStartX, nDstStartY, nDstEndX, nDstEndY;
	int nSrcPitch, nDstPitch;
	int* pnStart;
	int* pnEnd;
	int nPair;
	BYTE* pImgSrcPtr;
	BYTE* pImgDstPtr;
	int nDilationX;
	int nDilationY;
	int nROIWidth;
	int nROIHeight;
	int nError = 0;

	pnStart = NULL;
	pnEnd = NULL;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}
	if (prcSrc->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc Size different from prcDst Size");
		return -1;
	}

	BOOL bBigImage = (prcSrc->Height()) >  1024;
	if(bBigImage && IsOptimized()) {
		nError = CudaIf()->ImageBinDilation(pImgSrc,
											prcSrc,
											nBlobType,
											pImgDst,
											prcDst,
											pszMask);

		goto CleanUp;
	}

	pImgSrcPtr = pImgSrc->GetImgPtr();
	nSrcPitch = pImgSrc->GetPitch();
	nSrcStartX = prcSrc->left;
	nSrcStartY = prcSrc->top;
	nSrcEndX = prcSrc->right;
	nSrcEndY = prcSrc->bottom;

	pImgDstPtr = pImgDst->GetImgPtr();
	nDstPitch = pImgDst->GetPitch();
	nDstStartX = prcDst->left;
	nDstStartY = prcDst->top;
	nDstEndX = prcDst->right;
	nDstEndY = prcDst->bottom;

	nROIWidth = prcSrc->Width();
	nROIHeight = prcSrc->Height();
	lSrcRowIndex = nSrcStartY*nSrcPitch+nSrcStartX;
	lDstRowIndex = nDstStartY*nDstPitch+nDstStartX;
	for(i = nSrcStartY; i < nSrcEndY; i++) {
		for(j = 0; j < nROIWidth; j++) 
			pImgDstPtr[lDstRowIndex+j] = pImgSrcPtr[lSrcRowIndex+j];
		lSrcRowIndex += nSrcPitch;
		lDstRowIndex += nDstPitch;
	}
	if(nBlobType == WHITE) {
		nDilationX = pszMask->cx;
		if(nDilationX>0) {
			if(pnStart != NULL)
				delete [] pnStart;
			pnStart = NULL;
			if(pnEnd != NULL)
				delete [] pnEnd;
			pnEnd = NULL;
			pnStart = new int[abs(nROIWidth)];
			pnEnd = new int[abs(nROIWidth)];
			lDstRowIndex = nDstStartY*nDstPitch+nDstStartX;
			for(i = 0; i < nROIHeight; i++) {
				nPair = 0;
				for(j = 1; j < nROIWidth; j++) {
					if(pImgDstPtr[lDstRowIndex+j-1] != pImgDstPtr[lDstRowIndex+j]) {
						if(pImgDstPtr[lDstRowIndex+j] == 0) {
							pnStart[nPair] = j;
							pnEnd[nPair] = j + nDilationX;
							if(pnEnd[nPair] > nROIWidth) pnEnd[nPair] = nROIWidth;
							nPair++;
						}
						else {
							pnEnd[nPair] = j;
							pnStart[nPair] = j - nDilationX;
							if(pnStart[nPair] < 0) pnStart[nPair] = 0;
							nPair++;
						}
					}
				}
				for(j=0;j<nPair;j++) 
					for(m=pnStart[j];m<pnEnd[j];m++) 
						pImgDstPtr[lDstRowIndex+m] = 255;
				lDstRowIndex += nDstPitch;
			}
		}
		nDilationY = pszMask->cy;
		if(nDilationY>0) {
			if(pnStart != NULL)
				delete [] pnStart;
			pnStart = NULL;
			if(pnEnd != NULL)
				delete [] pnEnd;
			pnEnd = NULL;
			pnStart = new int[abs(nROIHeight)];
			pnEnd = new int[abs(nROIHeight)];
			lDstRowIndex = nDstStartY*nDstPitch+nDstStartX;
			for(i = 0; i < nROIWidth; i++) {
				lDstIndex = lDstRowIndex + nDstPitch + i;
				nPair = 0;
				for(j = 1; j < nROIHeight; j++) {
					if(pImgDstPtr[lDstIndex - nSrcPitch] != pImgDstPtr[lDstIndex]) {
						if(pImgDstPtr[lDstIndex] == 0) {
							pnStart[nPair] = j;
							pnEnd[nPair] = j + nDilationY;
							if(pnEnd[nPair] > nROIHeight) pnEnd[nPair] = nROIHeight;
							nPair++;
						}
						else {
							pnEnd[nPair] = j;
							pnStart[nPair] = j - nDilationY;
							if(pnStart[nPair] < 0) pnStart[nPair] = 0;
							nPair++;
						}
					}
					lDstIndex += nDstPitch;
				}
				for(j=0;j<nPair;j++) {
					lDstIndex = lDstRowIndex + pnStart[j] * nDstPitch + i;
					for(m=pnStart[j];m<pnEnd[j];m++) {
						pImgDstPtr[lDstIndex] = 255;
						lDstIndex += nDstPitch;
					}
				}
			}
		}
	}
	else {
		nDilationX = pszMask->cx;
		if(nDilationX>0) {
			if(pnStart != NULL)
				delete [] pnStart;
			pnStart = NULL;
			if(pnEnd != NULL)
				delete [] pnEnd;
			pnEnd = NULL;
			pnStart = new int[abs(nROIWidth)];
			pnEnd = new int[abs(nROIWidth)];
			lDstRowIndex = nDstStartY*nDstPitch+nDstStartX;
			for(i = 0; i < nROIHeight; i++) {
				nPair = 0;
				for(j = 1; j < nROIWidth; j++) {
					if(pImgDstPtr[lDstRowIndex+j-1] != pImgDstPtr[lDstRowIndex+j]) {
						if(pImgDstPtr[lDstRowIndex+j] == 0) {
							pnEnd[nPair] = j;
							pnStart[nPair] = j - nDilationX;
							if(pnStart[nPair] < 0) pnStart[nPair] = 0;
							nPair++;
						}
						else {
							pnStart[nPair] = j;
							pnEnd[nPair] = j + nDilationX;
							if(pnEnd[nPair] > nROIWidth) pnEnd[nPair] = nROIWidth;
							nPair++;
						}
					}
				}
				for(j=0;j<nPair;j++) 
					for(m=pnStart[j];m<pnEnd[j];m++) 
						pImgDstPtr[lDstRowIndex+m] = 0;
				lDstRowIndex += nDstPitch;
			}
		}
		nDilationY = pszMask->cy;
		if(nDilationY>0) {
			if(pnStart != NULL)
				delete [] pnStart;
			pnStart = NULL;
			if(pnEnd != NULL)
				delete [] pnEnd;
			pnEnd = NULL;
			pnStart = new int[abs(nROIHeight)];
			pnEnd = new int[abs(nROIHeight)];
			lDstRowIndex = nDstStartY*nDstPitch+nDstStartX;
			for(i = 0; i < nROIWidth; i++) {
				lDstIndex = lDstRowIndex + nDstPitch + i;
				nPair = 0;
				for(j = 1; j < nROIHeight; j++) {
					if(pImgDstPtr[lDstIndex - nSrcPitch] != pImgDstPtr[lDstIndex]) {
						if(pImgDstPtr[lDstIndex] == 0) {
							pnEnd[nPair] = j;
							pnStart[nPair] = j - nDilationY;
							if(pnStart[nPair] < 0) pnStart[nPair] = 0;
							nPair++;
						}
						else {
							pnStart[nPair] = j;
							pnEnd[nPair] = j + nDilationY;
							if(pnEnd[nPair] > nROIHeight) pnEnd[nPair] = nROIHeight;
							nPair++;
						}
					}
					lDstIndex += nDstPitch;
				}
				for(j=0;j<nPair;j++) {
					lDstIndex = lDstRowIndex + pnStart[j] * nDstPitch + i;
					for(m=pnStart[j];m<pnEnd[j];m++) {
						pImgDstPtr[lDstIndex] = 0;
						lDstIndex += nDstPitch;
					}
				}
			}
		}
	}
	if(pnStart != NULL)
		delete [] pnStart;
	pnStart = NULL;
	if(pnEnd != NULL)
		delete [] pnEnd;
	pnEnd = NULL;

CleanUp:
	return nError;
}


int ImgBinDilation(CImageBuffer *pImgSrc,
				 CRect *prcSrc,
				 int nBlobType,
				 CImageBuffer *pImgDst,
				 CRect *prcDst,
				 CRect *prcMask)
{
	int i, j, m;
	long lSrcRowIndex, lDstRowIndex;
	long lDstIndex;
	int nSrcStartX, nSrcStartY, nSrcEndX, nSrcEndY;
	int nDstStartX, nDstStartY, nDstEndX, nDstEndY;
	int nSrcPitch, nDstPitch;
	int* pnStart;
	int* pnEnd;
	int nPair;
	BYTE* pImgSrcPtr;
	BYTE* pImgDstPtr;
	int nDilationL;
	int nDilationR;
	int nDilationT;
	int nDilationB;
	int nROIWidth;
	int nROIHeight;

	pnStart = NULL;
	pnEnd = NULL;

	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}
	if (prcSrc->Size()!=prcDst->Size()) {
		ASSERT(!"prcSrc Size different from prcDst Size");
		return -1;
	}

	pImgSrcPtr = pImgSrc->GetImgPtr();
	nSrcPitch = pImgSrc->GetPitch();
	nSrcStartX = prcSrc->left;
	nSrcStartY = prcSrc->top;
	nSrcEndX = prcSrc->right;
	nSrcEndY = prcSrc->bottom;

	pImgDstPtr = pImgDst->GetImgPtr();
	nDstPitch = pImgDst->GetPitch();
	nDstStartX = prcDst->left;
	nDstStartY = prcDst->top;
	nDstEndX = prcDst->right;
	nDstEndY = prcDst->bottom;

	nROIWidth = prcSrc->Width();
	nROIHeight = prcSrc->Height();
	lSrcRowIndex = nSrcStartY*nSrcPitch+nSrcStartX;
	lDstRowIndex = nDstStartY*nDstPitch+nDstStartX;
	for(i = nSrcStartY; i < nSrcEndY; i++) {
		for(j = 0; j < nROIWidth; j++) 
			pImgDstPtr[lDstRowIndex+j] = pImgSrcPtr[lSrcRowIndex+j];
		lSrcRowIndex += nSrcPitch;
		lDstRowIndex += nDstPitch;
	}
	if(nBlobType == WHITE) {
		nDilationL = prcMask->left;
		nDilationR = prcMask->right;
		if((nDilationL + nDilationR)>0) {
			if(pnStart != NULL)
				delete [] pnStart;
			pnStart = NULL;
			if(pnEnd != NULL)
				delete [] pnEnd;
			pnEnd = NULL;
			pnStart = new int[abs(nROIWidth)];
			pnEnd = new int[abs(nROIWidth)];
			lDstRowIndex = nDstStartY*nDstPitch+nDstStartX;
			for(i = 0; i < nROIHeight; i++) {
				nPair = 0;
				for(j = 1; j < nROIWidth; j++) {
					if(pImgDstPtr[lDstRowIndex+j-1] != pImgDstPtr[lDstRowIndex+j]) {
						if(pImgDstPtr[lDstRowIndex+j] == 0) {
							pnStart[nPair] = j;
							pnEnd[nPair] = j + nDilationR;
							if(pnEnd[nPair] > nROIWidth) pnEnd[nPair] = nROIWidth;
							nPair++;
						}
						else {
							pnEnd[nPair] = j;
							pnStart[nPair] = j - nDilationL;
							if(pnStart[nPair] < 0) pnStart[nPair] = 0;
							nPair++;
						}
					}
				}
				for(j=0;j<nPair;j++) 
					for(m=pnStart[j];m<pnEnd[j];m++) 
						pImgDstPtr[lDstRowIndex+m] = 255;
				lDstRowIndex += nDstPitch;
			}
		}
		nDilationT = prcMask->top;
		nDilationB = prcMask->bottom;
		if((nDilationT + nDilationB)>0) {
			if(pnStart != NULL)
				delete [] pnStart;
			pnStart = NULL;
			if(pnEnd != NULL)
				delete [] pnEnd;
			pnEnd = NULL;
			pnStart = new int[abs(nROIHeight)];
			pnEnd = new int[abs(nROIHeight)];
			lDstRowIndex = nDstStartY*nDstPitch+nDstStartX;
			for(i = 0; i < nROIWidth; i++) {
				lDstIndex = lDstRowIndex + nDstPitch + i;
				nPair = 0;
				for(j = 1; j < nROIHeight; j++) {
					if(pImgDstPtr[lDstIndex - nSrcPitch] != pImgDstPtr[lDstIndex]) {
						if(pImgDstPtr[lDstIndex] == 0) {
							pnStart[nPair] = j;
							pnEnd[nPair] = j + nDilationB;
							if(pnEnd[nPair] > nROIHeight) pnEnd[nPair] = nROIHeight;
							nPair++;
						}
						else {
							pnEnd[nPair] = j;
							pnStart[nPair] = j - nDilationT;
							if(pnStart[nPair] < 0) pnStart[nPair] = 0;
							nPair++;
						}
					}
					lDstIndex += nDstPitch;
				}
				for(j=0;j<nPair;j++) {
					lDstIndex = lDstRowIndex + pnStart[j] * nDstPitch + i;
					for(m=pnStart[j];m<pnEnd[j];m++) {
						pImgDstPtr[lDstIndex] = 255;
						lDstIndex += nDstPitch;
					}
				}
			}
		}
	}
	else {
		nDilationL = prcMask->left;
		nDilationR = prcMask->right;
		if((nDilationL + nDilationR) >0) {
			if(pnStart != NULL)
				delete [] pnStart;
			pnStart = NULL;
			if(pnEnd != NULL)
				delete [] pnEnd;
			pnEnd = NULL;
			pnStart = new int[abs(nROIWidth)];
			pnEnd = new int[abs(nROIWidth)];
			lDstRowIndex = nDstStartY*nDstPitch+nDstStartX;
			for(i = 0; i < nROIHeight; i++) {
				nPair = 0;
				for(j = 1; j < nROIWidth; j++) {
					if(pImgDstPtr[lDstRowIndex+j-1] != pImgDstPtr[lDstRowIndex+j]) {
						if(pImgDstPtr[lDstRowIndex+j] == 0) {
							pnEnd[nPair] = j;
							pnStart[nPair] = j - nDilationL;
							if(pnStart[nPair] < 0) pnStart[nPair] = 0;
							nPair++;
						}
						else {
							pnStart[nPair] = j;
							pnEnd[nPair] = j + nDilationR;
							if(pnEnd[nPair] > nROIWidth) pnEnd[nPair] = nROIWidth;
							nPair++;
						}
					}
				}
				for(j=0;j<nPair;j++) 
					for(m=pnStart[j];m<pnEnd[j];m++) 
						pImgDstPtr[lDstRowIndex+m] = 0;
				lDstRowIndex += nDstPitch;
			}
		}
		nDilationT = prcMask->top;
		nDilationB = prcMask->bottom;
		if((nDilationT + nDilationB) >0) {
			if(pnStart != NULL)
				delete [] pnStart;
			pnStart = NULL;
			if(pnEnd != NULL)
				delete [] pnEnd;
			pnEnd = NULL;
			pnStart = new int[abs(nROIHeight)];
			pnEnd = new int[abs(nROIHeight)];
			lDstRowIndex = nDstStartY*nDstPitch+nDstStartX;
			for(i = 0; i < nROIWidth; i++) {
				lDstIndex = lDstRowIndex + nDstPitch + i;
				nPair = 0;
				for(j = 1; j < nROIHeight; j++) {
					if(pImgDstPtr[lDstIndex - nSrcPitch] != pImgDstPtr[lDstIndex]) {
						if(pImgDstPtr[lDstIndex] == 0) {
							pnEnd[nPair] = j;
							pnStart[nPair] = j - nDilationT;
							if(pnStart[nPair] < 0) pnStart[nPair] = 0;
							nPair++;
						}
						else {
							pnStart[nPair] = j;
							pnEnd[nPair] = j + nDilationB;
							if(pnEnd[nPair] > nROIHeight) pnEnd[nPair] = nROIHeight;
							nPair++;
						}
					}
					lDstIndex += nDstPitch;
				}
				for(j=0;j<nPair;j++) {
					lDstIndex = lDstRowIndex + pnStart[j] * nDstPitch + i;
					for(m=pnStart[j];m<pnEnd[j];m++) {
						pImgDstPtr[lDstIndex] = 0;
						lDstIndex += nDstPitch;
					}
				}
			}
		}
	}
	if(pnStart != NULL)
		delete [] pnStart;
	pnStart = NULL;
	if(pnEnd != NULL)
		delete [] pnEnd;
	pnEnd = NULL;
	return 0;
}


int MaskOverlapOR(CImageBuffer* pImgMask,
				CRect* prcMask,
				CImageBuffer* pImgMask1,
				CRect* prcMask1,
				CPoint* pptOffsetOfMask1)
{
	int i, j;
	long lIndex, lIndexTemp;
	CRect rcOverlap;
	CRect rcMask1AddOffset;
	BYTE *pImgMaskPtr, *pImgMaskTempPtr;
	int nImgMaskPitch, nImgMaskTempPitch;

	if (!pImgMask) {
		ASSERT(!"pImgMask is a bad Pointer");
		return -1;
	}
	if (!pImgMask->IsAllocated()) {
		ASSERT(!"pImgMask is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMask,&(pImgMask->GetImgSize()))) {
		ASSERT(!"prcMask is Invalid");
		return -1;
	}

	if (!pImgMask1) {
		ASSERT(!"pImgMask1 is a bad Pointer");
		return -1;
	}
	if (!pImgMask1->IsAllocated()) {
		ASSERT(!"pImgMask1 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMask1,&(pImgMask1->GetImgSize()))) {
		ASSERT(!"prcMask1 is Invalid");
		return -1;
	}

	nImgMaskPitch = pImgMask->GetPitch();
	nImgMaskTempPitch = pImgMask1->GetPitch();
	pImgMaskPtr = pImgMask->GetImgPtr();
	pImgMaskTempPtr = pImgMask1->GetImgPtr();

	rcMask1AddOffset = CRect(prcMask1->left + pptOffsetOfMask1->x,
							 prcMask1->top + pptOffsetOfMask1->y,
							 prcMask1->right + pptOffsetOfMask1->x,
							 prcMask1->bottom + pptOffsetOfMask1->y);

	rcOverlap = *prcMask;
	if(rcOverlap.left < rcMask1AddOffset.left) rcOverlap.left = rcMask1AddOffset.left;
	if(rcOverlap.top < rcMask1AddOffset.top) rcOverlap.top = rcMask1AddOffset.top;
	if(rcOverlap.right > rcMask1AddOffset.right) rcOverlap.right = rcMask1AddOffset.right;
	if(rcOverlap.bottom > rcMask1AddOffset.bottom) rcOverlap.bottom = rcMask1AddOffset.bottom;

	if (ValidateRect(&rcOverlap,&(pImgMask->GetImgSize()))) {
		return 0;
	}

	lIndex = rcOverlap.top * nImgMaskPitch;
	lIndexTemp = (rcOverlap.top - pptOffsetOfMask1->y) * nImgMaskTempPitch;
	for(i = rcOverlap.top; i <rcOverlap.bottom; i++) {
		for(j = rcOverlap.left; j < rcOverlap.right; j++) {
			pImgMaskPtr[lIndex + j] = pImgMaskPtr[lIndex + j] | pImgMaskTempPtr[lIndexTemp + j - pptOffsetOfMask1->x];
		}
		lIndex += nImgMaskPitch;
		lIndexTemp += nImgMaskTempPitch;
	}
	return 0;
}


int MaskOverlap(CImageBuffer* pImgMask,
				CRect* prcMask,
				CImageBuffer* pImgMask1,
				CRect* prcMask1,
				CPoint* pptOffsetOfMask1)	
{
	int i, j;
	long lIndex, lIndexTemp;
	CRect rcOverlap;
	CRect rcMask1AddOffset;
	BYTE *pImgMaskPtr, *pImgMaskTempPtr;
	int nImgMaskPitch, nImgMaskTempPitch;

	if (!pImgMask) {
		ASSERT(!"pImgMask is a bad Pointer");
		return -1;
	}
	if (!pImgMask->IsAllocated()) {
		ASSERT(!"pImgMask is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMask,&(pImgMask->GetImgSize()))) {
		ASSERT(!"prcMask is Invalid");
		return -1;
	}

	if (!pImgMask1) {
		ASSERT(!"pImgMask1 is a bad Pointer");
		return -1;
	}
	if (!pImgMask1->IsAllocated()) {
		ASSERT(!"pImgMask1 is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMask1,&(pImgMask1->GetImgSize()))) {
		ASSERT(!"prcMask1 is Invalid");
		return -1;
	}

	nImgMaskPitch = pImgMask->GetPitch();
	nImgMaskTempPitch = pImgMask1->GetPitch();
	pImgMaskPtr = pImgMask->GetImgPtr();
	pImgMaskTempPtr = pImgMask1->GetImgPtr();

	rcMask1AddOffset = CRect(prcMask1->left + pptOffsetOfMask1->x,
							 prcMask1->top + pptOffsetOfMask1->y,
							 prcMask1->right + pptOffsetOfMask1->x,
							 prcMask1->bottom + pptOffsetOfMask1->y);

	rcOverlap = *prcMask;
	if(rcOverlap.left < rcMask1AddOffset.left) rcOverlap.left = rcMask1AddOffset.left;
	if(rcOverlap.top < rcMask1AddOffset.top) rcOverlap.top = rcMask1AddOffset.top;
	if(rcOverlap.right > rcMask1AddOffset.right) rcOverlap.right = rcMask1AddOffset.right;
	if(rcOverlap.bottom > rcMask1AddOffset.bottom) rcOverlap.bottom = rcMask1AddOffset.bottom;

	if (ValidateRect(&rcOverlap,&(pImgMask->GetImgSize()))) {
		return 0;
	}

	lIndex = rcOverlap.top * nImgMaskPitch;
	lIndexTemp = (rcOverlap.top - pptOffsetOfMask1->y) * nImgMaskTempPitch;
	for(i = rcOverlap.top; i <rcOverlap.bottom; i++) {
		for(j = rcOverlap.left; j < rcOverlap.right; j++) {
			if(pImgMaskTempPtr[lIndexTemp + j - pptOffsetOfMask1->x] == 0) 
				pImgMaskPtr[lIndex + j] = 0;
		}
		lIndex += nImgMaskPitch;
		lIndexTemp += nImgMaskTempPitch;
	}
	return 0;
}

int ImgDebugMasking(CImageBuffer *pImgNOTMasked,
					CRect *prcNOTMasked,
					CImageBuffer *pImgMasked,
					CRect *prcMasked,
					CImageBuffer *pImgMask,
					CRect *prcMask,
					CImageBuffer *pImgMerge,
					CRect *prcMerge)
{
	int nError;
	CImageBuffer ImgBufMaskInvert;
	CRect rcMaskInvert;
	CImageBuffer ImgBufMerge1;
	CImageBuffer ImgBufMerge2;
	CRect rcMerge;

	if (!pImgNOTMasked) {
		ASSERT(!"pImgNOTMasked is a bad Pointer");
		return -1;
	}
	if (!pImgNOTMasked->IsAllocated()) {
		ASSERT(!"pImgNOTMasked is not Allocated");
		return -1;
	}
	if (ValidateRect(prcNOTMasked,&(pImgNOTMasked->GetImgSize()))) {
		ASSERT(!"prcNOTMasked is Invalid");
		return -1;
	}
	if (!pImgMasked) {
		ASSERT(!"pImgMasked is a bad Pointer");
		return -1;
	}
	if (!pImgMasked->IsAllocated()) {
		ASSERT(!"pImgNOTMasked is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMasked,&(pImgMasked->GetImgSize()))) {
		ASSERT(!"prcMasked is Invalid");
		return -1;
	}
	if (!pImgMask) {
		ASSERT(!"pImgMask is a bad Pointer");
		return -1;
	}
	if (!pImgMask->IsAllocated()) {
		ASSERT(!"pImgMask is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMask,&(pImgMask->GetImgSize()))) {
		ASSERT(!"prcMask is Invalid");
		return -1;
	}
	if (!pImgMerge) {
		ASSERT(!"pImgMerge is a bad Pointer");
		return -1;
	}
	if (!pImgMerge->IsAllocated()) {
		ASSERT(!"pImgMerge is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMerge,&(pImgMerge->GetImgSize()))) {
		ASSERT(!"prcMerge is Invalid");
		return -1;
	}
	if (prcMerge->Size()!=prcNOTMasked->Size()) {
		ASSERT(!"prcMerge Size different from prcNOTMasked Size");
		return -1;
	}
	if (prcMerge->Size()!=prcMasked->Size()) {
		ASSERT(!"prcMerge Size different from prcMasked Size");
		return -1;
	}
	if (prcMerge->Size()!=prcMask->Size()) {
		ASSERT(!"prcMerge Size different from prcMask Size");
		return -1;
	}

	rcMaskInvert = CRect(CPoint(0,0),prcMask->Size());
	ImgBufMaskInvert.Allocate(AbsSize(rcMaskInvert.Size()));
	rcMerge = CRect(CPoint(0,0),prcMerge->Size());
	ImgBufMerge1.Allocate(AbsSize(rcMerge.Size()));
	ImgBufMerge2.Allocate(AbsSize(rcMerge.Size()));

	nError = ImgInvert(pImgMask,
					   prcMask,
					   &ImgBufMaskInvert,
					   &rcMaskInvert);
	if(nError)
		return -1;
	nError = ImgAND(pImgMasked,
					prcMasked,
					pImgMask,
					prcMask,
					&ImgBufMerge1,
					&rcMerge);
	if(nError)
		return -1;
	nError = ImgAND(pImgNOTMasked,
					prcNOTMasked,
					&ImgBufMaskInvert,
					&rcMaskInvert,
					&ImgBufMerge2,
					&rcMerge);
	if(nError)
		return -1;
	nError = ImgADD(&ImgBufMerge1,
					&rcMerge,
					&ImgBufMerge2,
					&rcMerge,
					pImgMerge,
					prcMerge);
	if(nError)
		return -1;
	return 0;
}

int OpticsCharacteristics(CImageBuffer* pBuffer, CRect* prectROI, double& dMean, double& dNoise, double& dFocus)
{
	if (!pBuffer) {
		ASSERT(!"pBuffer is a bad Pointer");
		return -1;
	}
	
	if (!pBuffer->IsAllocated()) {
		ASSERT(!"pBuffer is not Allocated");
		return -1;
	}
	
	if (ValidateRect(prectROI, &(pBuffer->GetImgSize()))) {
		ASSERT(!"prectROI is Invalid");
		return -1;
	}

	int nWidth = prectROI->Width();
	int nHeight = prectROI->Height();
	if(nWidth < 1 || nHeight < 1) {
		ASSERT(!"prectROI is Invalid");
		return -1;
	}

	int x, y;
	int nPitch = pBuffer->GetPitch();
	BYTE* pBuf;
	BYTE* pRoiBuf = pBuffer->GetImgPtr() + nPitch * prectROI->top + prectROI->left;

	//MEAN
	int nSum = 0;
	pBuf = pRoiBuf;
	for (y=0; y<nHeight; y++, pBuf+=nPitch)
		for (x=0; x<nWidth; x++)
			nSum += pBuf[x];
	dMean = ((double)nSum)/(nHeight * nWidth);

	
	//NOISE
	double dSum = 0;
	pBuf = pRoiBuf;
	for (y=0; y<nHeight; y++, pBuf+=nPitch)
		for (x=0; x<nWidth; x++)
			dSum += pow(pBuf[x] - dMean, 2);
	double dSD = sqrt(dSum/((nHeight-1)*(nWidth-1)));

	if(dSD > 0)
		dNoise = dMean / dSD;
	else
		//dNoise = pow(2,64)-1;
		dNoise = 0;


	//FOCUS
	nSum = 0;
	pBuf = pRoiBuf;
	int nRow = 0, nCol = 0;
	int nEdgeCount = 0;
	for (y=0; y<nHeight-1; y++, pBuf+=nPitch) {
		for (x=0; x<nWidth-1; x++) {
			nRow = pBuf[x+1] - pBuf[x];
			if(nRow < 10)
				nRow = 0;
			nCol = pBuf[x+nPitch] - pBuf[x];
			if(nCol < 10)
				nCol = 0;
			nSum += (nRow + nCol);

			if (nRow + nCol > 0)
				nEdgeCount++;
		}
	}
	//dFocus = ((double)nSum)/((nHeight-1) * (nWidth-1));
	dFocus = 0;
	if (nEdgeCount > 0) {
		dFocus = ((double)nSum) / nEdgeCount;
		//Convert to %(0-100)
		double dMaxFocus = 45;
		dFocus = dFocus / dMaxFocus;
	}

	return 0;
}

CTeachShiftParm::CTeachShiftParm()
{
	ptPkgShift = CPoint(0,0);
	szMinDisp = CSize(30,30);
	rcDefault = CRect(100,100,200,200);
	szFOV = CSize(1024, 1024);
}

CTeachShiftParm::~CTeachShiftParm()
{
}


CRect GetTeachRect(CRect* prcPreTeach,
				   CTeachShiftParm* pTeachShiftParm)
{
	CRect rcTeach;
	CRect rcTeachTemp;
	rcTeach = pTeachShiftParm->rcDefault;
	rcTeachTemp = *prcPreTeach + pTeachShiftParm->ptPkgShift;
	if(ValidateRect(&rcTeachTemp, &pTeachShiftParm->szFOV) ||
		((rcTeachTemp.Width() < pTeachShiftParm->szMinDisp.cx) && 
		(rcTeachTemp.Height() < pTeachShiftParm->szMinDisp.cy)))
		return rcTeach;
	else
		return rcTeachTemp;
}

CRect GetTeachRect(CRect* prcPreTeach,
				   CPoint* ppt2DPkgShift,
				   CSize* pszMinDisp,
				   CRect* prcDefault,
				   CSize* pszFOV)
{
	CRect rcTeach;
	CRect rcTeachTemp;
	rcTeach = *prcDefault;
	rcTeachTemp = *prcPreTeach + *ppt2DPkgShift;
	if(ValidateRect(&rcTeachTemp, pszFOV) ||
		((rcTeachTemp.Width() < pszMinDisp->cx) && 
		(rcTeachTemp.Height() < pszMinDisp->cy)))
		return rcTeach;
	else
		return rcTeachTemp;
}


int GetLinePtsInROI(CPoint2D* ppt2DOrg,
				   CPoint2D* ppt2D,
				   CPoint2D* ppt2DInROI,
				   CRect* prcROI)
{
	int i;
	CPoint2D pt2DIntercept[4];
	CLine2D ln2D;
	double dDisOrg;
	double dDisSegment1;
	double dDisSegment2;
	double dDisSegment[4];
	CPoint2D pt2DInROI[4];
	int nFound;
	double dMax;
	int nMaxIndex;
	int nError;

	nError = ValidatePoint(ppt2DOrg, prcROI);
	if(nError)
		goto ErrorAbort;
	nError = ValidatePoint(ppt2D, prcROI);
	if(nError == 0) {
		*ppt2DInROI = *ppt2D;
		goto PASS;
	}
	dDisOrg = ppt2DOrg->GetDistance(*ppt2D);
	if(dDisOrg < 1)
		goto ErrorAbort;
	ln2D = CLine2D(*ppt2DOrg, *ppt2D);
	pt2DIntercept[0].x = prcROI->left + 1;
	pt2DIntercept[1].y = prcROI->top + 1;
	pt2DIntercept[2].x = prcROI->right - 2;
	pt2DIntercept[3].y = prcROI->bottom - 2;
	nError = ln2D.GetY(pt2DIntercept[0].x, &pt2DIntercept[0].y);
	if(nError)
		goto ErrorAbort;
	nError = ln2D.GetX(pt2DIntercept[1].y, &pt2DIntercept[1].x);
	if(nError)
		goto ErrorAbort;
	nError = ln2D.GetY(pt2DIntercept[2].x, &pt2DIntercept[2].y);
	if(nError)
		goto ErrorAbort;
	nError = ln2D.GetX(pt2DIntercept[3].y, &pt2DIntercept[3].x);
	if(nError)
		goto ErrorAbort;
	nFound = 0;
	for(i=0;i<4;i++) {
		nError = ValidatePoint(&pt2DIntercept[i], prcROI);
		if(nError == 0) {
			dDisSegment1 = ppt2DOrg->GetDistance(pt2DIntercept[i]);
			dDisSegment2 = ppt2D->GetDistance(pt2DIntercept[i]);
			if((dDisSegment1 <= dDisOrg) && ((dDisSegment2 <= dDisOrg))) {
				dDisSegment[nFound] = dDisSegment1;
				pt2DInROI[nFound] = pt2DIntercept[i];
				nFound++;
			}
		}
		else if(nError)
			goto ErrorAbort;
	}
	if(nFound <= 0)
		goto ErrorAbort;
	nMaxIndex = 0;
	dMax = dDisSegment[0];
	for(i=1;i<nFound;i++) {
		if(dMax < dDisSegment[i]) {
			dMax = dDisSegment[i];
			nMaxIndex = i;
		}
	}
	*ppt2DInROI = pt2DInROI[nMaxIndex];
PASS:
	return 0;
ErrorAbort:
	return -1;
}

int ImgDrawLine(CImageBuffer* pImgBufBin,
			 CRect* prcBin,
			 CPoint2D* ppt2DStart,
			 CPoint2D* ppt2DEnd,
			 BYTE byteVale,
			 int nWidth)
{
	int i;
	int nLineLength;
	double dStepX;
	double dStepY;
	double x, y;
	long lIndex;
	BYTE* pImgPtr;
	CPoint2D pt2DStart;
	CPoint2D pt2DEnd;
	CPoint2D pt2DEndInROI;
	CPoint ptEndInROI;
	int nPitch;
	CPoint ptBak;
	CPoint ptTemp;
	int nHalfWidth;
	CPoint ptStart;
	CPoint ptEnd;
	CPoint* pptStart;
	CPoint* pptEnd;


	ptStart = CPoint(Round(ppt2DStart->x), Round(ppt2DStart->y));
	ptEnd = CPoint(Round(ppt2DEnd->x), Round(ppt2DEnd->y));
	pptStart = &ptStart;
	pptEnd = &ptEnd;

	nHalfWidth = nWidth/2;
	if(nHalfWidth < 0)
		nHalfWidth = 0;
	if (!pImgBufBin) {
		ASSERT(!"pImgBufBin is a bad Pointer");
		return -1;
	}
	if (!pImgBufBin->IsAllocated()) {
		ASSERT(!"pImgBufBin is not Allocated");
		return -1;
	}
	if (ValidateRect(prcBin,&(pImgBufBin->GetImgSize()))) {
		ASSERT(!"prcBin is Invalid");
		return -1;
	}
	if (ValidatePoint(pptStart, prcBin)) {
		ASSERT(!"pptStart is Invalid");
		return -1;
	}

	pt2DStart = *pptStart;
	pt2DEnd = *pptEnd;
	GetLinePtsInROI(&pt2DStart,
				    &pt2DEnd,
				    &pt2DEndInROI,
				    prcBin);
	ptEndInROI = CPoint(Round(pt2DEndInROI.x), Round(pt2DEndInROI.y));

	pImgPtr = pImgBufBin->GetImgPtr();
	nPitch = pImgBufBin->GetPitch();
	nLineLength = (int) sqrt((double) ((pptStart->x - ptEndInROI.x) * (pptStart->x - ptEndInROI.x) + 
							 (pptStart->y - ptEndInROI.y) * (pptStart->y - ptEndInROI.y)));
	if(nLineLength == 0) {
		pImgBufBin->SetPixelVal(pptStart->x, pptStart->y, byteVale);
		goto PASS;
	}

	dStepX = double(ptEndInROI.x - pptStart->x) / nLineLength;
	dStepY = double(ptEndInROI.y - pptStart->y) / nLineLength;
	y = pptStart->y;
	x = pptStart->x;
	if(ValidatePoint(pptStart, prcBin)||ValidatePoint(&ptEndInROI, prcBin)) {
		CPoint ptTemp;
		if(fabs(dStepY) > fabs(dStepX))	{
			do	{
				lIndex = int(y) * nPitch + int(x);
				ptTemp = CPoint(int(x), int(y));
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
					}
				}
				for(i=1;i<=nHalfWidth;i++) {
					ptBak = CPoint(int(x), int(y));
					ptTemp = CPoint(ptBak.x-i, ptBak.y);
					if(ValidatePoint(&ptTemp, prcBin) == 0) {
						lIndex = ptTemp.y * nPitch + ptTemp.x;
						if(pImgPtr[lIndex] != byteVale) {
							pImgPtr[lIndex] = byteVale;
						}
					}
					ptTemp = CPoint(ptBak.x+i, ptBak.y);
					if(ValidatePoint(&ptTemp, prcBin) == 0) {
						lIndex = ptTemp.y * nPitch + ptTemp.x;
						if(pImgPtr[lIndex] != byteVale) {
							pImgPtr[lIndex] = byteVale;
						}
					}
				}
				y += dStepY;
				x += dStepX;
			}while((int)y != ptEndInROI.y); 
			lIndex=int(y) * nPitch + int(x);
			ptTemp = CPoint(int(x), int(y));
			if(ValidatePoint(&ptTemp, prcBin) == 0) {
				if(pImgPtr[lIndex] != byteVale) {
					pImgPtr[lIndex] = byteVale;
				}
			}
			for(i=1;i<=nHalfWidth;i++) {
				ptBak = CPoint(int(x), int(y));
				ptTemp = CPoint(ptBak.x-i, ptBak.y);
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					lIndex = ptTemp.y * nPitch + ptTemp.x;
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
					}
				}
				ptTemp = CPoint(ptBak.x+i, ptBak.y);
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					lIndex = ptTemp.y * nPitch + ptTemp.x;
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
					}
				}
			}
		}
		else {
			do	{
				lIndex = int(y) * nPitch + int(x);
				ptTemp = CPoint(int(x), int(y));
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
					}
				}
				for(i=1;i<=nHalfWidth;i++) {
					ptBak = CPoint(int(x), int(y));
					ptTemp = CPoint(ptBak.x, ptBak.y - i);
					if(ValidatePoint(&ptTemp, prcBin) == 0) {
						lIndex = ptTemp.y * nPitch + ptTemp.x;
						if(pImgPtr[lIndex] != byteVale) {
							pImgPtr[lIndex] = byteVale;
						}
					}
					ptTemp = CPoint(ptBak.x, ptBak.y + i);
					if(ValidatePoint(&ptTemp, prcBin) == 0) {
						lIndex = ptTemp.y * nPitch + ptTemp.x;
						if(pImgPtr[lIndex] != byteVale) {
							pImgPtr[lIndex] = byteVale;
						}
					}
				}
				y += dStepY;
				x += dStepX;
			}while((int)x != ptEndInROI.x);
			lIndex = int(y) * nPitch + int(x);
			ptTemp = CPoint(int(x), int(y));
			if(ValidatePoint(&ptTemp, prcBin) == 0) {
				if(pImgPtr[lIndex] != byteVale) {
					pImgPtr[lIndex] = byteVale;
				}
			}
			for(i=1;i<=nHalfWidth;i++) {
				ptBak = CPoint(int(x), int(y));
				ptTemp = CPoint(ptBak.x, ptBak.y - i);
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					lIndex = ptTemp.y * nPitch + ptTemp.x;
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
					}
				}
				ptTemp = CPoint(ptBak.x, ptBak.y + i);
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					lIndex = ptTemp.y * nPitch + ptTemp.x;
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
					}
				}
			}
		}
	}
	else {
		if(fabs(dStepY) > fabs(dStepX))	{
			do	{
				lIndex = int (y) * nPitch + int(x);
				if(pImgPtr[lIndex] != byteVale) {
					pImgPtr[lIndex] = byteVale;
				}
				for(i=1;i<=nHalfWidth;i++) {
					ptBak = CPoint(int(x), int(y));
					ptTemp = CPoint(ptBak.x-i, ptBak.y);
					if(ValidatePoint(&ptTemp, prcBin) == 0) {
						lIndex = ptTemp.y * nPitch + ptTemp.x;
						if(pImgPtr[lIndex] != byteVale) {
							pImgPtr[lIndex] = byteVale;
						}
					}
					ptTemp = CPoint(ptBak.x+i, ptBak.y);
					if(ValidatePoint(&ptTemp, prcBin) == 0) {
						lIndex = ptTemp.y * nPitch + ptTemp.x;
						if(pImgPtr[lIndex] != byteVale) {
							pImgPtr[lIndex] = byteVale;
						}
					}
				}
				y += dStepY;
				x += dStepX;
			}while((int)y != ptEndInROI.y); 
			lIndex=int(y) * nPitch + int(x);
			if(pImgPtr[lIndex] != byteVale) {
				pImgPtr[lIndex] = byteVale;
			}
			for(i=1;i<=nHalfWidth;i++) {
				ptBak = CPoint(int(x), int(y));
				ptTemp = CPoint(ptBak.x-i, ptBak.y);
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					lIndex = ptTemp.y * nPitch + ptTemp.x;
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
					}
				}
				ptTemp = CPoint(ptBak.x+i, ptBak.y);
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					lIndex = ptTemp.y * nPitch + ptTemp.x;
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
					}
				}
			}
		}
		else {
			do	{
				lIndex = int(y) * nPitch + int(x);
				if(pImgPtr[lIndex] != byteVale) {
					pImgPtr[lIndex] = byteVale;
				}
				for(i=1;i<=nHalfWidth;i++) {
					ptBak = CPoint(int(x), int(y));
					ptTemp = CPoint(ptBak.x, ptBak.y - i);
					if(ValidatePoint(&ptTemp, prcBin) == 0) {
						lIndex = ptTemp.y * nPitch + ptTemp.x;
						if(pImgPtr[lIndex] != byteVale) {
							pImgPtr[lIndex] = byteVale;
						}
					}
					ptTemp = CPoint(ptBak.x, ptBak.y + i);
					if(ValidatePoint(&ptTemp, prcBin) == 0) {
						lIndex = ptTemp.y * nPitch + ptTemp.x;
						if(pImgPtr[lIndex] != byteVale) {
							pImgPtr[lIndex] = byteVale;
						}
					}
				}
				y += dStepY;
				x += dStepX;
			}while((int)x != ptEndInROI.x);
			lIndex = int(y) * nPitch + int(x);
			if(pImgPtr[lIndex] != byteVale) {
				pImgPtr[lIndex] = byteVale;
			}
			for(i=1;i<=nHalfWidth;i++) {
				ptBak = CPoint(int(x), int(y));
				ptTemp = CPoint(ptBak.x, ptBak.y - i);
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					lIndex = ptTemp.y * nPitch + ptTemp.x;
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
					}
				}
				ptTemp = CPoint(ptBak.x, ptBak.y + i);
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					lIndex = ptTemp.y * nPitch + ptTemp.x;
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
					}
				}
			}
		}
	}
PASS:
	return 0;
}

int ImgSlidingSum(CImageBuffer *pImgSrc,
			CRect* prcSrc,
			CSize* pcsProj,
			CImageBuffer* pImgDst,
			CRect* prcDst)
{
	int i, j;
	int nImgPitchSrc, nImgIntPitchV;
	int nWidthSrc, nHeightSrc;
	BYTE *pImgSrcPtr;
	int *pProjVPtr;
	CRect rcFirstProj;
	long lImgSrcLostRowIndex;
	long lImgSrcAddRowIndex;
	long lProjVRowIndex;
	CImgIntBuf ImgIntProjV;
	CRect rcProjV;
	long *plXAry;
	long *plYAry;
	long lProjRowIndex;
	int* pIntProjPtr;
	int nImgIntPitch;
	CSize csProjOrg;
	CImgIntBuf* pImgIntProj;
	CImgIntBuf ImgIntProj;
	CRect rcProj;
	CRect* prcProj;
	BYTE* pImgDstPtr;
	int nPitchDst;
	//long lIndexSrc;
	long lIndexDst;
	int nWeight;
	CRect rcCopySrc[4];
	CRect rcCopyDst[4];

	pImgDstPtr = pImgDst->GetImgPtr();
	nPitchDst = pImgDst->GetPitch();
	csProjOrg = *pcsProj;
	int nError;

	rcCopySrc[0] = CRect(prcSrc->left, prcSrc->top, prcSrc->right, prcSrc->top + pcsProj->cy/2+1); 
	rcCopySrc[1] = CRect(prcSrc->left,prcSrc->top,prcSrc->left + pcsProj->cx/2+1, prcSrc->bottom); 
	rcCopySrc[2] = CRect(prcSrc->left, prcSrc->bottom - pcsProj->cy/2-1, prcSrc->right, prcSrc->bottom); 
	rcCopySrc[3] = CRect(prcSrc->right - pcsProj->cx/2-1, prcSrc->top, prcSrc->right, prcSrc->bottom); 

	rcCopyDst[0] = CRect(prcDst->left, prcDst->top, prcDst->right, prcDst->top + pcsProj->cy/2+1); 
	rcCopyDst[1] = CRect(prcDst->left,prcDst->top,prcDst->left + pcsProj->cx/2+1, prcDst->bottom); 
	rcCopyDst[2] = CRect(prcDst->left, prcDst->bottom - pcsProj->cy/2-1, prcDst->right, prcDst->bottom); 
	rcCopyDst[3] = CRect(prcDst->right - pcsProj->cx/2-1, prcDst->top, prcDst->right, prcDst->bottom); 

	for(i=0;i<4;i++)
	{
		nError = ImgCopy(pImgSrc,
			&rcCopySrc[i],
			pImgDst,
			&rcCopyDst[i]);
	}
	nWeight = pcsProj->cx * pcsProj->cy;
	rcProj = CRect(CPoint(0,0), CSize(prcSrc->Size() - csProjOrg));
	ImgIntProj.Allocate(rcProj.Size());
	pImgIntProj = &ImgIntProj;
	prcProj = &rcProj;
	
	plXAry = NULL;
	plYAry = NULL;
	if(nWeight <= 0)
		goto ErrorAbort;
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if(pcsProj->cx >= prcSrc->Width())
		pcsProj->cx = prcSrc->Width() - 1;
	if(pcsProj->cy >= prcSrc->Height())
		pcsProj->cy = prcSrc->Height() - 1;

	rcProjV = CRect(CPoint(0,0), CSize(prcSrc->Width(), prcSrc->Height() - pcsProj->cy));
	ImgIntProjV.Allocate(rcProjV.Size());
	ImgIntProjV.ClrImg(0);

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgIntPitchV = ImgIntProjV.GetPitch();
	nWidthSrc = rcProjV.Width();
	nHeightSrc = rcProjV.Height();
	pImgSrcPtr = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pProjVPtr = ImgIntProjV.GetImgPtr();

	plXAry = new long[abs(prcSrc->Width())];
	rcFirstProj = CRect(prcSrc->TopLeft(), CSize(prcSrc->Width(), pcsProj->cy));
	nError = GetXGrayAccl(pImgSrc, &rcFirstProj, &plXAry[0]);
	if(nError)
		goto ErrorAbort;
	for(i=0;i<nWidthSrc;i++)
		pProjVPtr[i] = plXAry[i];

	lImgSrcLostRowIndex = 0;
	lImgSrcAddRowIndex = nImgPitchSrc * pcsProj->cy;
	lProjVRowIndex = nImgIntPitchV;
	for(i=1;i<nHeightSrc;i++) {
		for(j=0;j<nWidthSrc;j++) {
			pProjVPtr[lProjVRowIndex + j] = pProjVPtr[lProjVRowIndex - nImgIntPitchV + j] + 
					pImgSrcPtr[lImgSrcAddRowIndex + j] - pImgSrcPtr[lImgSrcLostRowIndex+j];	
		}
		lImgSrcLostRowIndex += nImgPitchSrc;
		lImgSrcAddRowIndex += nImgPitchSrc;
		lProjVRowIndex += nImgIntPitchV;
	}
	*prcProj = CRect(CPoint(0,0), CSize(prcSrc->Width() - pcsProj->cx, prcSrc->Height() - pcsProj->cy));
	pImgIntProj->Allocate(prcProj->Size());
	pImgIntProj->ClrImg(0);

	nImgIntPitch = pImgIntProj->GetPitch();
	nWidthSrc = prcProj->Width();
	nHeightSrc = prcProj->Height();

	pIntProjPtr = pImgIntProj->GetImgPtr();

	plYAry = new long[abs(nHeightSrc)];
	
	lProjRowIndex = 0;
	lProjVRowIndex = 0;
	for(i=0;i<nHeightSrc;i++) {
		plYAry[i] = 0;
		for(j=0;j<pcsProj->cx;j++) {
			plYAry[i] += pProjVPtr[lProjVRowIndex + j];
		}
		pIntProjPtr[lProjRowIndex] = plYAry[i];
		lProjVRowIndex += nImgIntPitchV;
		lProjRowIndex += nImgIntPitch;
	}
	lProjRowIndex = 0;
	lProjVRowIndex = 0;
	lIndexDst = (prcDst->top + pcsProj->cy/2) * nPitchDst + prcDst->left + pcsProj->cx/2;
	for(i=0;i<nHeightSrc;i++) {
		for(j=1;j<nWidthSrc;j++) {
			pIntProjPtr[lProjRowIndex+j] = pIntProjPtr[lProjRowIndex+j-1] + 
				pProjVPtr[lProjVRowIndex + j - 1 + pcsProj->cx ] - pProjVPtr[lProjVRowIndex + j -1];
			pImgDstPtr[lIndexDst + j] = pIntProjPtr[lProjRowIndex+j]/nWeight;
//			if(pImgDstPtr[lIndexDst + j] > pImgSrcPtr[lIndexDst + j])
//				pImgDstPtr[lIndexDst + j] = pImgSrcPtr[lIndexDst + j];
		}
		lProjRowIndex += nImgIntPitch;
		lProjVRowIndex += nImgIntPitchV;
		lIndexDst += nPitchDst;
	}
	if(plXAry != NULL)
		delete [] plXAry;
	if(plYAry != NULL)
		delete [] plYAry;
	*pcsProj = csProjOrg;
	return 0;
ErrorAbort:
	if(plXAry != NULL)
		delete [] plXAry;
	if(plYAry != NULL)
		delete [] plYAry;
	*pcsProj = csProjOrg;
	return -1;
}

int ImgSlidingSum(CImageBuffer *pImgSrc,
			CRect* prcSrc,
			CSize* pcsProj,
			CImgIntBuf* pImgIntProj,
			CRect* prcProj)
{
	int i, j;
	int nImgPitchSrc, nImgIntPitchV;
	int nWidthSrc, nHeightSrc;
	BYTE *pImgSrcPtr;
	int *pProjVPtr;
	CRect rcFirstProj;
	long lImgSrcLostRowIndex;
	long lImgSrcAddRowIndex;
	long lProjVRowIndex;
	CImgIntBuf ImgIntProjV;
	CRect rcProjV;
	long *plXAry;
	long *plYAry;
	long lProjRowIndex;
	int* pIntProjPtr;
	int nImgIntPitch;
	CSize csProjOrg;


try
{
	csProjOrg = *pcsProj;
	int nError;
	plXAry = NULL;
	plYAry = NULL;
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if(pcsProj->cx >= prcSrc->Width())
		pcsProj->cx = prcSrc->Width() - 1;
	if(pcsProj->cy >= prcSrc->Height())
		pcsProj->cy = prcSrc->Height() - 1;

	rcProjV = CRect(CPoint(0,0), CSize(prcSrc->Width(), prcSrc->Height() - pcsProj->cy));
	ImgIntProjV.Allocate(rcProjV.Size());
	ImgIntProjV.ClrImg(0);

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgIntPitchV = ImgIntProjV.GetPitch();
	nWidthSrc = rcProjV.Width();
	nHeightSrc = rcProjV.Height();
	pImgSrcPtr = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pProjVPtr = ImgIntProjV.GetImgPtr();

	plXAry = new long[prcSrc->Width()];
	rcFirstProj = CRect(prcSrc->TopLeft(), CSize(prcSrc->Width(), pcsProj->cy));
	nError = GetXGrayAccl(pImgSrc, &rcFirstProj, &plXAry[0]);
	for(i=0;i<nWidthSrc;i++)
		pProjVPtr[i] = plXAry[i];

	lImgSrcLostRowIndex = 0;
	lImgSrcAddRowIndex = nImgPitchSrc * (pcsProj->cy+1);
	lProjVRowIndex = nImgIntPitchV;
	for(i=1;i<nHeightSrc;i++) {
		for(j=0;j<nWidthSrc;j++) {
			pProjVPtr[lProjVRowIndex + j] = pProjVPtr[lProjVRowIndex - nImgIntPitchV + j] + 
					pImgSrcPtr[lImgSrcAddRowIndex + j] - pImgSrcPtr[lImgSrcLostRowIndex+j];	
		}
		lImgSrcLostRowIndex += nImgPitchSrc;
		lImgSrcAddRowIndex += nImgPitchSrc;
		lProjVRowIndex += nImgIntPitchV;
	}
	*prcProj = CRect(CPoint(0,0), CSize(prcSrc->Width() - pcsProj->cx, prcSrc->Height() - pcsProj->cy));
	pImgIntProj->Allocate(prcProj->Size());
	pImgIntProj->ClrImg(0);

	nImgIntPitch = pImgIntProj->GetPitch();
	nWidthSrc = prcProj->Width();
	nHeightSrc = prcProj->Height();

	pIntProjPtr = pImgIntProj->GetImgPtr();

	plYAry = new long[nHeightSrc];
	
	lProjRowIndex = 0;
	lProjVRowIndex = 0;
	for(i=0;i<nHeightSrc;i++) {
		plYAry[i] = 0;
		for(j=0;j<pcsProj->cx;j++) {
			plYAry[i] += pProjVPtr[lProjVRowIndex + j];
		}
		pIntProjPtr[lProjRowIndex] = plYAry[i];
		lProjVRowIndex += nImgIntPitchV;
		lProjRowIndex += nImgIntPitch;
	}

	lProjRowIndex = 0;
	lProjVRowIndex = 0;
	for(i=0;i<nHeightSrc;i++) {
		for(j=1;j<nWidthSrc;j++) {
			pIntProjPtr[lProjRowIndex+j] = pIntProjPtr[lProjRowIndex+j-1] + 
				pProjVPtr[lProjVRowIndex + j + pcsProj->cx] - pProjVPtr[lProjVRowIndex + j -1];
		}
		lProjRowIndex += nImgIntPitch;
		lProjVRowIndex += nImgIntPitchV;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgSlidingSum");
	goto ErrorAbort;
}
	if(plXAry != NULL)
		delete [] plXAry;
	if(plYAry != NULL)
		delete [] plYAry;
	*pcsProj = csProjOrg;
	return 0;
ErrorAbort:
	if(plXAry != NULL)
		delete [] plXAry;
	if(plYAry != NULL)
		delete [] plYAry;
	*pcsProj = csProjOrg;
	return -1;
}

BOOL ZOOM1D(BYTE *pbyteSrc,	int nSizeSrc,BYTE *pbyteDst,int nSizeDst)
{
	int i;
	int No;
	int nSizeSrcSub1;
	int nSizeDstSub1;
	int nMappingSrcTimes16;

	if(nSizeDst<=1)
		goto ErrorAbort;
	nSizeSrcSub1 = (nSizeSrc-1) << 4;
	nSizeDstSub1 = nSizeDst-1;

//	dRatio = (double) (nSizeSrc-1)/(nSizeDst-1);

	for(i = 0; i < nSizeDst; i++)
	{
		nMappingSrcTimes16 = i * nSizeSrcSub1/nSizeDstSub1;
		No = nMappingSrcTimes16 >> 4;
		if(No >= nSizeSrcSub1)
			pbyteDst[nSizeDstSub1]= pbyteSrc[nSizeSrcSub1];
		else
		pbyteDst[i]= pbyteSrc[No] + (pbyteSrc[No + 1] - pbyteSrc[No]) * (nMappingSrcTimes16%16)/16;

	}
	
	return 0;
ErrorAbort:
	return -1;
}

BOOL ZOOM1D(BYTE *pbyteSrc,	
			int nSizeSrc,
			int* pnPosInSrc,
			int* pnPosInterpolTimes10,
			BYTE *pbyteDst,
			int nSizeDst)
{
	int i;
	int No;

	int nSizeDstSub1;
	int nSizeSrcSub1;
	nSizeDstSub1 = nSizeDst-1;
	nSizeSrcSub1 = nSizeSrc-1;


	if(nSizeDst<=1)
		goto ErrorAbort;
	for(i = 0; i < nSizeDst; i++)
	{
		No = pnPosInSrc[i];
		if(No >= nSizeSrcSub1)
		pbyteDst[nSizeDstSub1]= pbyteSrc[nSizeSrcSub1];
		else
		pbyteDst[i]= pbyteSrc[No] + (pbyteSrc[No + 1] - pbyteSrc[No]) * pnPosInterpolTimes10[i]/10;

	}
	
	return 0;
ErrorAbort:
	return -1;
}
		
int ImageZooming(CImageBuffer *pImgSrc,					// Pointer to Source Image
				  CRect *prcSrc,						// ROI of the Source Image
				  CImageBuffer *pImgDst,					// Pointer to Destination Image
				  CRect *prcDst)

{

	
	int i, j;
	BYTE *pbyteSrc;
	int nSizeSrc;
	int nSizeSrcSub1;
	BYTE *pbyteDst;
	int nSizeDst;
	int nSizeDstSub1;
	CImageBuffer ImgTemp;
	BYTE* pImgTempPtr;

	long lSrcRowIndex, lDstRowIndex;
	int nSrcStartX, nSrcStartY, nSrcEndX, nSrcEndY;
	int nDstStartX, nDstStartY, nDstEndX, nDstEndY;
	int nSrcPitch, nDstPitch;

	int* pnPosInSrc;
	int* pnPosInterpolTimes10;
	

	pnPosInSrc = NULL;
	pnPosInterpolTimes10 = NULL;
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc,&(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgDst) {
		ASSERT(!"pImgDst is a bad Pointer");
		return -1;
	}
	if (!pImgDst->IsAllocated()) {
		ASSERT(!"pImgDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcDst,&(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}
	ImgTemp.Allocate(CSize(prcDst->Width(), prcSrc->Height()));
	pImgTempPtr = ImgTemp.GetImgPtr();
	
	pbyteSrc = NULL;
	pbyteDst = NULL;
	nSrcPitch = pImgSrc->GetPitch();
	nSrcStartX = prcSrc->left;
	nSrcStartY = prcSrc->top;
	nSrcEndX = prcSrc->right;
	nSrcEndY = prcSrc->bottom;

	nDstPitch = pImgDst->GetPitch();
	nDstStartX = prcDst->left;
	nDstStartY = prcDst->top;
	nDstEndX = prcDst->right;
	nDstEndY = prcDst->bottom;


	
	nSizeSrc = prcSrc->Width();
	nSizeDst = prcDst->Width();
	pbyteSrc = new BYTE[nSizeSrc];
	pbyteDst = new BYTE[nSizeDst];
	pnPosInSrc = new int[nSizeDst];
	pnPosInterpolTimes10 = new int[nSizeDst];
	lSrcRowIndex = nSrcStartY*nSrcPitch+nSrcStartX;
	lDstRowIndex = nDstStartY*nDstPitch+nDstStartX;
	nSizeSrcSub1 = nSizeSrc - 1;
	nSizeDstSub1 = nSizeDst - 1;
	for(i=0;i<nSizeDst;i++) {
		if(nSizeDstSub1 <= 0)
			pnPosInterpolTimes10[i] = 0;
		else
			pnPosInterpolTimes10[i] = i * nSizeSrcSub1 * 10/nSizeDstSub1;
		pnPosInSrc[i] = pnPosInterpolTimes10[i]/10;
		pnPosInterpolTimes10[i] = pnPosInterpolTimes10[i] % 10;
	}
	for(i = nSrcStartY; i < nSrcEndY; i++) {
		// get one line of image to process
		for(j = 0; j < nSizeSrc; j++)
			pbyteSrc[j] = pImgSrc->GetImgPtr()[lSrcRowIndex+j];
		ZOOM1D(pbyteSrc, nSizeSrc, pnPosInSrc, pnPosInterpolTimes10, pbyteDst, nSizeDst);
		for(j = 0; j < nSizeDst; j++)
			pImgTempPtr[lDstRowIndex+j] = pbyteDst[j];
		lSrcRowIndex += nSrcPitch;
		lDstRowIndex += nDstPitch;
	}
	if(pbyteSrc != NULL)
		delete [] pbyteSrc;
	if(pbyteDst != NULL)
		delete [] pbyteDst;
	pbyteSrc = NULL;
	pbyteDst = NULL;
	if(pnPosInSrc != NULL)
		delete [] pnPosInSrc;
	if(pnPosInterpolTimes10 != NULL)
		delete [] pnPosInterpolTimes10;
	pnPosInSrc = NULL;
	pnPosInterpolTimes10 = NULL;

			
	nSizeDst = prcDst->Height();
	nSizeSrc = prcSrc->Height();
	pnPosInSrc = new int[nSizeDst];
	pnPosInterpolTimes10 = new int[nSizeDst];
	nSizeSrcSub1 = nSizeSrc - 1;
	nSizeDstSub1 = nSizeDst - 1;
	for(i=0;i<nSizeDst;i++) {
		if(nSizeDstSub1 <= 0)
			pnPosInterpolTimes10[i] = 0;
		else
			pnPosInterpolTimes10[i] = i * nSizeSrcSub1 * 10/nSizeDstSub1;
		pnPosInSrc[i] = pnPosInterpolTimes10[i]/10;
		pnPosInterpolTimes10[i] = pnPosInterpolTimes10[i] % 10;
	}
	pbyteSrc = new BYTE[nSizeSrc];
	pbyteDst = new BYTE[nSizeDst];
	for(i = 0; i < prcDst->Width(); i++) {
		// get one line of image to process
		lDstRowIndex = nDstStartY * nDstPitch + nDstStartX + i;
		lSrcRowIndex = lDstRowIndex;
		for(j = 0; j < nSizeSrc; j++) {
			pbyteSrc[j] = pImgTempPtr[lSrcRowIndex];
			lSrcRowIndex += nDstPitch;
		}
		ZOOM1D(pbyteSrc, nSizeSrc, pnPosInSrc, pnPosInterpolTimes10, pbyteDst, nSizeDst);
		for(j = 0; j < nSizeDst; j++) {
			pImgDst->GetImgPtr()[lDstRowIndex] = pbyteDst[j];
			lDstRowIndex += nDstPitch;
		}
	}

	if(pbyteSrc != NULL)
		delete [] pbyteSrc;
	if(pbyteDst != NULL)
		delete [] pbyteDst;
	if(pnPosInSrc != NULL)
		delete [] pnPosInSrc;
	if(pnPosInterpolTimes10 != NULL)
		delete [] pnPosInterpolTimes10;
	return 0;
}

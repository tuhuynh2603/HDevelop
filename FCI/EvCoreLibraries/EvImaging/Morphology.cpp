// Morphology.cpp: implementation of the CMorphology class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Morphology.h"
#include "EdgeMatching.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMorphology::CMorphology()
{
	pnKernelAddress = NULL;
	nKernelAddress = 0;
}

CMorphology::~CMorphology()
{
	Clean();
}

void CMorphology::Clean()
{
	if(pnKernelAddress != NULL)
		delete [] pnKernelAddress;
	pnKernelAddress = NULL;
	nKernelAddress = 0;
}

int CMorphology::Process(CImageBuffer* pImgIn,
					CRect* prcIn,
					int nImgType,
					int nProcType,
					CSize* pcsKernel,
					CImageBuffer* pImgOut,
					CRect* prcOut)
{
	CImageBuffer ImgTemp;
	CRect rcRelative;
	int nError;

try
{
	rcRelative = CRect(CPoint(0,0), prcIn->Size());
	ImgTemp.Allocate(AbsSize(rcRelative.Size()));
	if(nProcType == DILATION) {
		ImgTemp.ClrImg(255);
		pImgOut->ClrImg(255);
	}
	else {
		ImgTemp.ClrImg(0);
		pImgOut->ClrImg(0);
	}
	if((pcsKernel->cx > 1) && (pcsKernel->cy > 1)) {
		nError = Morphology1D(pImgIn,
							prcIn,
							ROW,
							nProcType,
							pcsKernel->cx,
							&ImgTemp,
							&rcRelative);
		if(nError)
			goto ErrorAbort;
		nError = Morphology1D(&ImgTemp,
							&rcRelative,
							COLUMN,
							nProcType,
							pcsKernel->cy,
							pImgOut,
							prcOut);
		if(nError)
			goto ErrorAbort;
	}
	else {
		if(pcsKernel->cx > 1) {
			nError = Morphology1D(pImgIn,
							prcIn,
							ROW,
							nProcType,
							pcsKernel->cx,
							pImgOut,
							prcOut);
			if(nError)
				goto ErrorAbort;
		}
		else {
			nError = Morphology1D(pImgIn,
							prcIn,
							COLUMN,
							nProcType,
							pcsKernel->cy,
							pImgOut,
							prcOut);
		if(nError)
		goto ErrorAbort;
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::Process");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMorphology::Process(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgMask,
					CRect* prcMask,
					int nImgType,
					int nProcType,
					CSize* pcsKernel,
					CImageBuffer* pImgOut,
					CRect* prcOut)
{
	CImageBuffer ImgTemp;
	CRect rcRelative;
	int nError;

try
{
	rcRelative = CRect(CPoint(0,0), prcIn->Size());
	ImgTemp.Allocate(AbsSize(rcRelative.Size()));
	if(nProcType == DILATION) {
		ImgTemp.ClrImg(255);
		pImgOut->ClrImg(255);
	}
	else {
		ImgTemp.ClrImg(0);
		pImgOut->ClrImg(0);
	}
	if((pcsKernel->cx > 1) && (pcsKernel->cy > 1)) {
		nError = Morphology1D(pImgIn,
							prcIn,
							pImgMask,
							prcMask,
							ROW,
							nProcType,
							pcsKernel->cx,
							&ImgTemp,
							&rcRelative);
		if(nError)
		goto ErrorAbort;
		nError = Morphology1D(&ImgTemp,
							&rcRelative,
							pImgMask,
							prcMask,
							COLUMN,
							nProcType,
							pcsKernel->cy,
							pImgOut,
							prcOut);
		if(nError)
		goto ErrorAbort;
	}
	else {
		if(pcsKernel->cx > 1) {
			nError = Morphology1D(pImgIn,
							prcIn,
							pImgMask,
							prcMask,
							ROW,
							nProcType,
							pcsKernel->cx,
							pImgOut,
							prcOut);
			if(nError)
				goto ErrorAbort;
		}
		else {
			nError = Morphology1D(pImgIn,
							prcIn,
							pImgMask,
							prcMask,
							COLUMN,
							nProcType,
							pcsKernel->cy,
							pImgOut,
							prcOut);
			if(nError)
		goto ErrorAbort;
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::Process -1");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;

}

int CMorphology::Morphology1D(CImageBuffer* pImgSrc,
							  CRect* prcSrc,
							  int nDir,
							  int nProcMode,
							  int nWidth,
							  CImageBuffer* pImgDst,
							  CRect* prcDst)
{
	int x, y, k;
	BYTE* pImgSrcPtr;
	int nImgSrcPitch;
	long lSrcIndex;
	BYTE* pImgDstPtr;
	int nImgDstPitch;
	long lDstIndex;
	int nWidthSplit0;
	int nWidthSplit1;
	BYTE byteSelectVal;
	long lSrcIndexSlideCtr;
	long lSrcIndexSlide;
	long lDstIndexSlide;
	long lSrcIndexMax;
	int nWidthSplit0ADD1;
	int nError;

try
{
	nError = ValidateImageAndRect(pImgSrc, prcSrc);
	if(nError)
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgDst, prcDst);
	if(nError)
		goto ErrorAbort;
	if(prcSrc->Size() != prcDst->Size())
		goto ErrorAbort;
	pImgSrcPtr = pImgSrc->GetImgPtr();
	nImgSrcPitch = pImgSrc->GetPitch();
	pImgDstPtr = pImgDst->GetImgPtr();
	nImgDstPitch = pImgDst->GetPitch();
	nWidthSplit0 = nWidth/2;
	nWidthSplit1 = nWidth - nWidthSplit0 - 1;
	nError = EdgePartMorphology(pImgSrc,
					  prcSrc,
					  nDir,
					  nWidth,
					  pImgDst,
					  prcDst);
	if(nError)
		goto ErrorAbort;
	
	int nValue;
	if(nDir == ROW) {
		if(nWidth >= prcSrc->Width()) 
			goto PASS;
		if(nWidthSplit0 >= prcSrc->Width()) 
			nWidthSplit0 = prcSrc->Width() - 1;
		if(nWidthSplit1 >= prcSrc->Width()) 
			nWidthSplit1 = prcSrc->Width() - 1;
		nWidthSplit0ADD1 = nWidthSplit0 + 1;
		lSrcIndex = prcSrc->top * nImgSrcPitch + prcSrc->left;
		lDstIndex = prcDst->top * nImgDstPitch + prcDst->left;
		if(nProcMode == DILATION) {
			for(y=0;y<prcSrc->Height();y++) {
				byteSelectVal = 0;
				lSrcIndexSlideCtr = lSrcIndex;
				for(k=0;k<nWidth;k++) {
					nValue = pImgSrcPtr[lSrcIndexSlideCtr+k];
					if(byteSelectVal < nValue)
						byteSelectVal = nValue;
				}
				pImgDstPtr[lDstIndex + nWidthSplit0] = byteSelectVal;
				for(x=nWidthSplit0+1;x<(prcSrc->Width() - nWidthSplit1);x++) {
					lSrcIndexMax = lSrcIndex + x + nWidthSplit1;
					nValue = pImgSrcPtr[lSrcIndexMax];
					if(pImgDstPtr[lDstIndex+x-1] <= nValue)
						pImgDstPtr[lDstIndex+x] = nValue;
					else {
						nValue = pImgDstPtr[lDstIndex+x-1];
						if(nValue > pImgSrcPtr[lSrcIndex+x-nWidthSplit0ADD1])
							pImgDstPtr[lDstIndex+x] = nValue;
						else {
							byteSelectVal = 0;
							lSrcIndexSlideCtr = lSrcIndex + x - nWidthSplit0;
							for(k=0;k<nWidth;k++) {
								nValue = pImgSrcPtr[lSrcIndexSlideCtr+k];
								if(byteSelectVal < nValue)
									byteSelectVal = nValue;
							}
							pImgDstPtr[lDstIndex+x] = byteSelectVal;
						}
					}
				}
				lSrcIndex += nImgSrcPitch;
				lDstIndex += nImgDstPitch;
			}
		}
		else {
			for(y=0;y<prcSrc->Height();y++) {
				byteSelectVal = 255;
				lSrcIndexSlideCtr = lSrcIndex;
				for(k=0;k<nWidth;k++) {
					nValue = pImgSrcPtr[lSrcIndexSlideCtr+k];
					if(byteSelectVal > nValue)
						byteSelectVal = nValue;
				}
				pImgDstPtr[lDstIndex + nWidthSplit0] = byteSelectVal;
				for(x=nWidthSplit0+1;x<(prcSrc->Width() - nWidthSplit1);x++) {
					lSrcIndexMax = lSrcIndex + x + nWidthSplit1;
					nValue = pImgSrcPtr[lSrcIndexMax];
					if(pImgDstPtr[lDstIndex+x-1] >= nValue)
						pImgDstPtr[lDstIndex+x] = nValue;
					else {
						if(pImgDstPtr[lDstIndex+x-1] < pImgSrcPtr[lSrcIndex+x-nWidthSplit0ADD1])
							pImgDstPtr[lDstIndex+x] = pImgDstPtr[lDstIndex+x-1];
						else {
							byteSelectVal = 255;
							lSrcIndexSlideCtr = lSrcIndex + x - nWidthSplit0;
							for(k=0;k<nWidth;k++) {
								nValue = pImgSrcPtr[lSrcIndexSlideCtr+k];
								if(byteSelectVal > nValue)
									byteSelectVal = nValue;
							}
							pImgDstPtr[lDstIndex+x] = byteSelectVal;
						}
					}
				}
				lSrcIndex += nImgSrcPitch;
				lDstIndex += nImgDstPitch;
			}
		}
	}
	else {
		int nWidthTimesnImgSrcPitch;
		int nWidthSplit0Add1TimesnImgDstPitch;
		if(nWidth >= prcSrc->Height()) 
			goto PASS;
		if(nWidthSplit0 >= prcSrc->Height()) 
			nWidthSplit0 = prcSrc->Height() - 1;
		if(nWidthSplit1 >= prcSrc->Height()) 
			nWidthSplit1 = prcSrc->Height() - 1;
		nWidthSplit0ADD1 = nWidthSplit0 + 1;
		nWidthTimesnImgSrcPitch = nWidth * nImgSrcPitch;
		nWidthSplit0Add1TimesnImgDstPitch = (nWidthSplit0+1) * nImgDstPitch;
		if(nProcMode == DILATION) {
			for(x=0;x<prcSrc->Width();x++) {
				lSrcIndex = prcSrc->top * nImgSrcPitch + prcSrc->left + x;
				lDstIndex = prcDst->top * nImgDstPitch + prcDst->left + x;
				byteSelectVal = 0;
				lSrcIndexSlide = lSrcIndex;
				for(k=0;k<nWidth;k++) {
					nValue = pImgSrcPtr[lSrcIndexSlide];
					if(byteSelectVal < nValue)
						byteSelectVal = nValue;
					lSrcIndexSlide += nImgSrcPitch;
				}
				pImgDstPtr[lDstIndex + nWidthSplit0 * nImgDstPitch] = byteSelectVal;
				for(y=nWidthSplit0+1;y<(prcSrc->Height() - nWidthSplit1 - 1);y++) {
					lSrcIndexMax = lSrcIndex + nWidthTimesnImgSrcPitch;
					lDstIndexSlide = lDstIndex + nWidthSplit0Add1TimesnImgDstPitch;
					nValue = pImgSrcPtr[lSrcIndexMax];
					if(pImgDstPtr[lDstIndexSlide - nImgDstPitch] <= nValue)
						pImgDstPtr[lDstIndexSlide] = nValue;
					else {
						nValue = pImgDstPtr[lDstIndexSlide - nImgDstPitch];
						if(nValue > pImgSrcPtr[lSrcIndex])
							pImgDstPtr[lDstIndexSlide] = nValue;
						else {
							byteSelectVal = 0;
							lSrcIndexSlide = lSrcIndex + nImgSrcPitch;
							for(k=0;k<nWidth;k++) {
								nValue = pImgSrcPtr[lSrcIndexSlide];
								if(byteSelectVal < nValue)
									byteSelectVal = nValue;
								lSrcIndexSlide += nImgSrcPitch;
							}
							pImgDstPtr[lDstIndexSlide] = byteSelectVal;
						}
					}
					lSrcIndex += nImgSrcPitch;
					lDstIndex += nImgDstPitch;
				}
			}
		} 
		else {
			for(x=0;x<prcSrc->Width();x++) {
				lSrcIndex = prcSrc->top * nImgSrcPitch + prcSrc->left + x;
				lDstIndex = prcDst->top * nImgDstPitch + prcDst->left + x;
				byteSelectVal = 255;
				lSrcIndexSlide = lSrcIndex;
				for(k=0;k<nWidth;k++) {
					nValue = pImgSrcPtr[lSrcIndexSlide];
					if(byteSelectVal > nValue)
						byteSelectVal = nValue;
					lSrcIndexSlide += nImgSrcPitch;
				}
				pImgDstPtr[lDstIndex + nWidthSplit0 * nImgDstPitch] = byteSelectVal;
				for(y=nWidthSplit0+1;y<(prcSrc->Height() - nWidthSplit1 - 1);y++) {
					lSrcIndexMax = lSrcIndex + nWidthTimesnImgSrcPitch;
					lDstIndexSlide = lDstIndex + nWidthSplit0Add1TimesnImgDstPitch;
					nValue = pImgSrcPtr[lSrcIndexMax];
					if(pImgDstPtr[lDstIndexSlide - nImgDstPitch] >= nValue)
						pImgDstPtr[lDstIndexSlide] = nValue;
					else {
						nValue = pImgDstPtr[lDstIndexSlide - nImgDstPitch];
						if(nValue < pImgSrcPtr[lSrcIndex])
							pImgDstPtr[lDstIndexSlide] = nValue;
						else {
							byteSelectVal = 255;
							lSrcIndexSlide = lSrcIndex + nImgSrcPitch;
							for(k=0;k<nWidth;k++) {
								nValue = pImgSrcPtr[lSrcIndexSlide];
								if(byteSelectVal > nValue)
									byteSelectVal = nValue;
								lSrcIndexSlide += nImgSrcPitch;
							}
							pImgDstPtr[lDstIndexSlide] = byteSelectVal;
						}
					}
					lSrcIndex += nImgSrcPitch;
					lDstIndex += nImgDstPitch;
				}
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::Morphology1D");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

int CMorphology::EdgePartMorphology(CImageBuffer* pImgSrc,
							  CRect* prcSrc,
							  int nDir,
							  int nWidth,
							  CImageBuffer* pImgDst,
							  CRect* prcDst)
{
	int x, y;
	BYTE* pImgSrcPtr;
	int nImgSrcPitch;
	long lSrcIndex;
	BYTE* pImgDstPtr;
	int nImgDstPitch;
	long lDstIndex;
	int nWidthSplit0;
	int nWidthSplit1;
	BYTE byteMax;
	long lNewDataIndex;
	int nError;

try
{
	nError = ValidateImageAndRect(pImgSrc, prcSrc);
	if(nError)
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgDst, prcDst);
	if(nError)
		goto ErrorAbort;
	if(prcSrc->Size() != prcDst->Size())
		goto ErrorAbort;
	pImgSrcPtr = pImgSrc->GetImgPtr();
	nImgSrcPitch = pImgSrc->GetPitch();
	pImgDstPtr = pImgDst->GetImgPtr();
	nImgDstPitch = pImgDst->GetPitch();
	nWidthSplit0 = nWidth/2;
	nWidthSplit1 = nWidth - nWidthSplit0 - 1;
	if(nDir == ROW) {
		if(nWidthSplit0 >= prcSrc->Width()) 
			nWidthSplit0 = prcSrc->Width() - 1;
		if(nWidthSplit1 >= prcSrc->Width()) 
			nWidthSplit1 = prcSrc->Width() - 1;
		lSrcIndex = prcSrc->top * nImgSrcPitch + prcSrc->left;
		lDstIndex = prcDst->top * nImgDstPitch + prcDst->left;
		for(y=0;y<prcSrc->Height();y++) {
			byteMax = 0;
			for(x=0;x<=nWidthSplit0;x++) {
				if(byteMax < pImgSrcPtr[lSrcIndex+x])
					byteMax = pImgSrcPtr[lSrcIndex+x];
			}
			pImgDstPtr[lDstIndex] = byteMax;
			lNewDataIndex = lSrcIndex + nWidthSplit0;
			for(x=1;x<=nWidthSplit0;x++) {
				if(byteMax < pImgSrcPtr[lNewDataIndex + x])
					byteMax = pImgSrcPtr[lNewDataIndex + x];
				pImgDstPtr[lDstIndex+x] = byteMax;
			}
			lSrcIndex += nImgSrcPitch;
			lDstIndex += nImgDstPitch;
		}
		lSrcIndex = prcSrc->top * nImgSrcPitch + prcSrc->right-1;
		lDstIndex = prcDst->top * nImgDstPitch + prcDst->right-1;

		for(y=0;y<prcSrc->Height();y++) {
			byteMax = 0;
			for(x=0;x<=nWidthSplit1;x++) {
				if(byteMax < pImgSrcPtr[lSrcIndex-x])
					byteMax = pImgSrcPtr[lSrcIndex-x];
			}
			pImgDstPtr[lDstIndex] = byteMax;

			lNewDataIndex = lSrcIndex - nWidthSplit1;
			for(x=1;x<nWidthSplit1;x++) {
				if(byteMax < pImgSrcPtr[lNewDataIndex - x])
					byteMax = pImgSrcPtr[lNewDataIndex - x];
				pImgDstPtr[lDstIndex-x] = byteMax;
			}
			lSrcIndex += nImgSrcPitch;
			lDstIndex += nImgDstPitch;
		}
	}
	else {
		long lSrcIndexTopLft;
		long lDstIndexTopLft;
		if(nWidthSplit0 >= prcSrc->Height()) 
			nWidthSplit0 = prcSrc->Height() - 1;
		if(nWidthSplit1 >= prcSrc->Height()) 
			nWidthSplit1 = prcSrc->Height() - 1;
		lSrcIndexTopLft = prcSrc->top * nImgSrcPitch + prcSrc->left;
		lDstIndexTopLft = prcDst->top * nImgDstPitch + prcDst->left;
		for(x=0;x<prcSrc->Width();x++) {
			lSrcIndex = lSrcIndexTopLft + x;
			lDstIndex = lDstIndexTopLft + x;
			byteMax = 0;
			for(y=0;y<=nWidthSplit0;y++) {
				if(byteMax < pImgSrcPtr[lSrcIndex])
					byteMax = pImgSrcPtr[lSrcIndex];
				lSrcIndex += nImgSrcPitch;
			}
			pImgDstPtr[lDstIndex] = byteMax;
			lNewDataIndex = lSrcIndexTopLft + nWidthSplit0 * nImgSrcPitch + nImgSrcPitch + x;
			lDstIndex += nImgDstPitch;
			for(y=1;y<=nWidthSplit1;y++) {
				if(byteMax < pImgSrcPtr[lNewDataIndex])
					byteMax = pImgSrcPtr[lNewDataIndex];
				pImgDstPtr[lDstIndex] = byteMax;
				lNewDataIndex += nImgSrcPitch;
				lDstIndex += nImgDstPitch;
			}
		}
		lSrcIndexTopLft = (prcSrc->bottom-1) * nImgSrcPitch + prcSrc->left;
		lDstIndexTopLft = (prcDst->bottom-1) * nImgDstPitch + prcDst->left;
		for(x=0;x<prcSrc->Width();x++) {
			lSrcIndex = lSrcIndexTopLft + x;
			lDstIndex = lDstIndexTopLft + x;
			byteMax = 0;
			for(y=0;y<=nWidthSplit1;y++) {
				if(byteMax < pImgSrcPtr[lSrcIndex])
					byteMax = pImgSrcPtr[lSrcIndex];
				lSrcIndex -= nImgSrcPitch;
			}
			pImgDstPtr[lDstIndex] = byteMax;
			lNewDataIndex = lSrcIndexTopLft - nWidthSplit0 * nImgSrcPitch - nImgSrcPitch + x;
			lDstIndex -= nImgDstPitch;
			for(y=1;y<=nWidthSplit1;y++) {
				if(byteMax < pImgSrcPtr[lNewDataIndex])
					byteMax = pImgSrcPtr[lNewDataIndex];
				pImgDstPtr[lDstIndex] = byteMax;
				lNewDataIndex -= nImgSrcPitch;
				lDstIndex -= nImgDstPitch;
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::EdgePartMorphology");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}



int CMorphology::CopyEdgeNoMorphology(CImageBuffer* pImgSrc,
							  CRect* prcSrc,
							  int nDir,
							  int nWidth,
							  CImageBuffer* pImgDst,
							  CRect* prcDst)
{
	int x, y;
	BYTE* pImgSrcPtr;
	int nImgSrcPitch;
	long lSrcIndex;
	BYTE* pImgDstPtr;
	int nImgDstPitch;
	long lDstIndex;
	int nWidthSplit0;
	int nWidthSplit1;
	int nError;

try
{
	nError = ValidateImageAndRect(pImgSrc, prcSrc);
	if(nError)
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgDst, prcDst);
	if(nError)
		goto ErrorAbort;
	if(prcSrc->Size() != prcDst->Size())
		goto ErrorAbort;
	pImgSrcPtr = pImgSrc->GetImgPtr();
	nImgSrcPitch = pImgSrc->GetPitch();
	pImgDstPtr = pImgDst->GetImgPtr();
	nImgDstPitch = pImgDst->GetPitch();
	nWidthSplit0 = nWidth/2;
	nWidthSplit1 = nWidth/2 - 1;
	if(nDir == ROW) {
		if(nWidthSplit0 >= prcSrc->Width()) 
			nWidthSplit0 = prcSrc->Width() - 1;
		if(nWidthSplit1 >= prcSrc->Width()) 
			nWidthSplit1 = prcSrc->Width() - 1;
		lSrcIndex = prcSrc->top * nImgSrcPitch + prcSrc->left;
		lDstIndex = prcDst->top * nImgDstPitch + prcDst->left;
		for(y=0;y<prcSrc->Height();y++) {
			for(x=0;x<nWidthSplit0;x++) {
				pImgDstPtr[lDstIndex+x] = pImgSrcPtr[lSrcIndex+x];
			}
			lSrcIndex += nImgSrcPitch;
			lDstIndex += nImgDstPitch;
		}
		lSrcIndex = prcSrc->top * nImgSrcPitch + prcSrc->right-1;
		lDstIndex = prcDst->top * nImgDstPitch + prcDst->right-1;
		for(y=0;y<prcSrc->Height();y++) {
			for(x=0;x<nWidthSplit1;x++) {
				pImgDstPtr[lDstIndex-x] = pImgSrcPtr[lSrcIndex-x];
			}
			lSrcIndex += nImgSrcPitch;
			lDstIndex += nImgDstPitch;
		}
	}
	else {
		long lSrcIndexTopLft;
		long lDstIndexTopLft;
		if(nWidthSplit0 >= prcSrc->Height()) 
			nWidthSplit0 = prcSrc->Height() - 1;
		if(nWidthSplit1 >= prcSrc->Height()) 
			nWidthSplit1 = prcSrc->Height() - 1;
		lSrcIndexTopLft = prcSrc->top * nImgSrcPitch + prcSrc->left;
		lDstIndexTopLft = prcDst->top * nImgDstPitch + prcDst->left;
		for(x=0;x<prcSrc->Width();x++) {
			lSrcIndex = lSrcIndexTopLft + x;
			lDstIndex = lDstIndexTopLft + x;
			for(y=0;y<=nWidthSplit0;y++) {
				pImgDstPtr[lDstIndex] = pImgSrcPtr[lSrcIndex];
				lSrcIndex += nImgSrcPitch;
				lDstIndex += nImgDstPitch;
			}
		}
		lSrcIndexTopLft = (prcSrc->bottom-1) * nImgSrcPitch + prcSrc->left;
		lDstIndexTopLft = (prcDst->bottom-1) * nImgDstPitch + prcDst->left;
		for(x=0;x<prcSrc->Width();x++) {
			lSrcIndex = lSrcIndexTopLft + x;
			lDstIndex = lDstIndexTopLft + x;
			for(y=0;y<=nWidthSplit0;y++) {
				pImgDstPtr[lDstIndex] = pImgSrcPtr[lSrcIndex];
				lSrcIndex -= nImgSrcPitch;
				lDstIndex -= nImgDstPitch;
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::CopyEdgeNoMorphology");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMorphology::Morphology1D(CImageBuffer* pImgSrc,
							  CRect* prcSrc,
							  CImageBuffer* pImgMsk,
							  CRect* prcMsk,
							  int nDir,
							  int nProcMode,
							  int nWidth,
							  CImageBuffer* pImgDst,
							  CRect* prcDst)
{
	int x, y, k;
	BYTE* pImgSrcPtr;
	int nImgSrcPitch;
	long lSrcIndex;
	BYTE* pImgMskPtr;
	int nImgMskPitch;
	long lMskIndex;
	BYTE* pImgDstPtr;
	int nImgDstPitch;
	long lDstIndex;
	int nWidthSplit0;
	int nWidthSplit1;
	BYTE byteSelectVal;
	long lSrcIndexSlideCtr;
	long lSrcIndexSlide;
	long lDstIndexSlide;
	long lSrcIndexMax;
	int nWidthSplit0ADD1;
	long lMskIndexSlideCtr;
	long lMskIndexSlide;
	BOOL* pbMask;
	int nError;

try
{
	pbMask = NULL;
	nError = ValidateImageAndRect(pImgSrc, prcSrc);
	if(nError)
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgMsk, prcMsk);
	if(nError)
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgDst, prcDst);
	if(nError)
		goto ErrorAbort;
	if(prcSrc->Size() != prcMsk->Size())
		goto ErrorAbort;
	if(prcSrc->Size() != prcDst->Size())
		goto ErrorAbort;
	pImgSrcPtr = pImgSrc->GetImgPtr();
	nImgSrcPitch = pImgSrc->GetPitch();
	pImgMskPtr = pImgMsk->GetImgPtr();
	nImgMskPitch = pImgMsk->GetPitch();
	pImgDstPtr = pImgDst->GetImgPtr();
	nImgDstPitch = pImgDst->GetPitch();
	nWidthSplit0 = nWidth/2;
	nWidthSplit1 = nWidth - nWidthSplit0 - 1;
	nError = CopyEdgeNoMorphology(pImgSrc,
							  prcSrc,
							  nDir,
							  nWidth,
							  pImgDst,
							  prcDst);
	BYTE nValue;
	if(nDir == ROW) {
		if(nWidth >= prcSrc->Width()) 
			goto PASS;
		if(nWidthSplit0 >= prcSrc->Width()) 
			nWidthSplit0 = prcSrc->Width() - 1;
		if(nWidthSplit1 >= prcSrc->Width()) 
			nWidthSplit1 = prcSrc->Width() - 1;
		nWidthSplit0ADD1 = nWidthSplit0 + 1;
		lSrcIndex = prcSrc->top * nImgSrcPitch + prcSrc->left;
		lMskIndex = prcMsk->top * nImgMskPitch + prcMsk->left;
		lDstIndex = prcDst->top * nImgDstPitch + prcDst->left;
		pbMask = new BOOL[abs(prcSrc->Width())];
		if(nProcMode == DILATION) {
			for(y=0;y<prcSrc->Height();y++) {
				byteSelectVal = 0;
				lSrcIndexSlideCtr = lSrcIndex;
				lMskIndexSlideCtr = lMskIndex;
				for(k=0;k<nWidth;k++) {
					nValue = pImgSrcPtr[lSrcIndexSlideCtr+k];
					if((byteSelectVal < nValue) && (pImgMskPtr[lMskIndexSlideCtr+k] > 0))
						byteSelectVal = nValue;
				}
				pImgDstPtr[lDstIndex + nWidthSplit0] = byteSelectVal;
				for(x=0;x<nWidth;x++)
					pbMask[x] = FALSE;
				for(x=nWidth;x<prcSrc->Width();x++)
					pbMask[x] = TRUE;
				for(x=nWidthSplit0+1;x<(prcSrc->Width() - nWidthSplit1);x++) {
					if(pImgMskPtr[lMskIndex+x] == 0) {
						for(k=0;k<nWidthSplit1;k++)
							pbMask[x + k] = FALSE;
						continue;
					}
					lSrcIndexMax = lSrcIndex + x + nWidthSplit1;
					nValue = pImgSrcPtr[lSrcIndexMax];
					if((pImgDstPtr[lDstIndex+x-1] <= nValue) && (pImgMskPtr[lMskIndex+x+nWidthSplit1]))
						pImgDstPtr[lDstIndex+x] = nValue;
					else {
						nValue = pImgDstPtr[lDstIndex+x-1];
						if((nValue > pImgSrcPtr[lSrcIndex+x-nWidthSplit0ADD1]) && (nValue > 0) && (pbMask[x]))
							pImgDstPtr[lDstIndex+x] = nValue;
						else {
							byteSelectVal = 0;
							lSrcIndexSlideCtr = lSrcIndex + x - nWidthSplit0;
							lMskIndexSlideCtr = lMskIndex + x - nWidthSplit0;
							for(k=0;k<nWidth;k++) {
								nValue = pImgSrcPtr[lSrcIndexSlideCtr+k];
								if((byteSelectVal < nValue) && (pImgMskPtr[lMskIndexSlideCtr+k] > 0))
									byteSelectVal = nValue;
							}
							pImgDstPtr[lDstIndex+x] = byteSelectVal;
						}
					}
				}
				lSrcIndex += nImgSrcPitch;
				lMskIndex += nImgMskPitch;
				lDstIndex += nImgDstPitch;
			}
		}
		else {
			for(y=0;y<prcSrc->Height();y++) {
				byteSelectVal = 255;
				lSrcIndexSlideCtr = lSrcIndex;
				lMskIndexSlideCtr = lMskIndex;
				for(k=0;k<nWidth;k++) {
					nValue = pImgSrcPtr[lSrcIndexSlideCtr+k];
					if((byteSelectVal > nValue) && (pImgMskPtr[lMskIndexSlideCtr+k] > 0))
						byteSelectVal = nValue;
				}
				pImgDstPtr[lDstIndex + nWidthSplit0] = byteSelectVal;
				for(x=0;x<nWidth;x++)
					pbMask[x] = FALSE;
				for(x=nWidth;x<prcSrc->Width();x++)
					pbMask[x] = TRUE;
				for(x=nWidthSplit0+1;x<(prcSrc->Width() - nWidthSplit1);x++) {
					if(pImgMskPtr[lMskIndex+x] == 0) {
						for(k=0;k<nWidthSplit1;k++)
							pbMask[x + k] = FALSE;
						continue;
					}
					lSrcIndexMax = lSrcIndex + x + nWidthSplit1;
					nValue = pImgSrcPtr[lSrcIndexMax];
					if((pImgDstPtr[lDstIndex+x-1] >= nValue) && (pImgMskPtr[lMskIndex+x+nWidthSplit1]))
						pImgDstPtr[lDstIndex+x] = nValue;
					else {
						nValue = pImgDstPtr[lDstIndex+x-1];
						if((nValue < pImgSrcPtr[lSrcIndex+x-nWidthSplit0ADD1]) && (nValue > 0) && (pbMask[x]))
							pImgDstPtr[lDstIndex+x] = nValue;
						else {
							byteSelectVal = 255;
							lSrcIndexSlideCtr = lSrcIndex + x - nWidthSplit0;
							for(k=0;k<nWidth;k++) {
								nValue = pImgSrcPtr[lSrcIndexSlideCtr+k];
								if((byteSelectVal > nValue) && (pImgMskPtr[lMskIndexSlideCtr+k] > 0))
									byteSelectVal = nValue;
							}
							pImgDstPtr[lDstIndex+x] = byteSelectVal;
						}
					}
				}
				lSrcIndex += nImgSrcPitch;
				lMskIndex += nImgMskPitch;
				lDstIndex += nImgDstPitch;
			}
		}
	}
	else {
		int nWidthTimesnImgSrcPitch;
		int nWidthSplit0Add1TimesnImgDstPitch;
		if(nWidth >= prcSrc->Height()) 
			goto PASS;
		if(nWidthSplit0 >= prcSrc->Height()) 
			nWidthSplit0 = prcSrc->Height() - 1;
		if(nWidthSplit1 >= prcSrc->Height()) 
			nWidthSplit1 = prcSrc->Height() - 1;
		nWidthSplit0ADD1 = nWidthSplit0 + 1;
		nWidthTimesnImgSrcPitch = nWidth * nImgSrcPitch;
		nWidthSplit0Add1TimesnImgDstPitch = (nWidthSplit0+1) * nImgDstPitch;
		pbMask = new BOOL[abs(prcSrc->Height())];
		if(nProcMode == DILATION) {
			for(x=0;x<prcSrc->Width();x++) {
				lSrcIndex = prcSrc->top * nImgSrcPitch + prcSrc->left + x;
				lMskIndex = prcMsk->top * nImgMskPitch + prcMsk->left + x;
				lDstIndex = prcDst->top * nImgDstPitch + prcDst->left + x;
				byteSelectVal = 0;
				lSrcIndexSlide = lSrcIndex;
				lMskIndexSlide = lMskIndex;
				for(k=0;k<nWidth;k++) {
					nValue = pImgSrcPtr[lSrcIndexSlide];
					if((byteSelectVal < nValue) && (pImgMskPtr[lMskIndexSlide] > 0))
						byteSelectVal = nValue;
					lSrcIndexSlide += nImgSrcPitch;
					lMskIndexSlide += nImgMskPitch;
				}
				pImgDstPtr[lDstIndex + nWidthSplit0 * nImgDstPitch] = byteSelectVal;
				for(y=0;y<nWidth;y++)
					pbMask[y] = FALSE;
				for(y=nWidth;y<prcSrc->Height();y++)
					pbMask[y] = TRUE;
				lMskIndex = (prcMsk->top+nWidthSplit0+1) * nImgMskPitch + prcMsk->left + x;
				for(y=nWidthSplit0+1;y<(prcSrc->Height() - nWidthSplit1 - 1);y++) {
					if(pImgMskPtr[lMskIndex]) {
						lSrcIndexMax = lSrcIndex + nWidthTimesnImgSrcPitch;
						lDstIndexSlide = lDstIndex + nWidthSplit0Add1TimesnImgDstPitch;
						nValue = pImgSrcPtr[lSrcIndexMax];
						if((pImgDstPtr[lDstIndexSlide - nImgDstPitch] <= nValue) && (pImgMskPtr[lMskIndex+x+nWidthSplit1 * nImgMskPitch]))
							pImgDstPtr[lDstIndexSlide] = nValue;
						else {
							nValue = pImgDstPtr[lDstIndexSlide - nImgDstPitch];
							if((nValue > pImgSrcPtr[lSrcIndex]) && (nValue > 0) && (pbMask[y]))
								pImgDstPtr[lDstIndexSlide] = nValue;
							else {
								byteSelectVal = 0;
								lSrcIndexSlide = lSrcIndex + nImgSrcPitch;
								lMskIndexSlide = lMskIndex - (nWidthSplit0) * nImgMskPitch;
								for(k=0;k<nWidth;k++) {
									nValue = pImgSrcPtr[lSrcIndexSlide];
									if((byteSelectVal < nValue) && (pImgMskPtr[lMskIndexSlide]))
										byteSelectVal = nValue;
									lSrcIndexSlide += nImgSrcPitch;
									lMskIndexSlide += nImgMskPitch;
								}
								pImgDstPtr[lDstIndexSlide] = byteSelectVal;
							}
						}
					}
					else {
						for(k=0;k<nWidthSplit1;k++)
							pbMask[y + k] = FALSE;
					}
					lSrcIndex += nImgSrcPitch;
					lMskIndex += nImgMskPitch;
					lDstIndex += nImgDstPitch;
				}
			}
		} 
		else {
			for(x=0;x<prcSrc->Width();x++) {
				lSrcIndex = prcSrc->top * nImgSrcPitch + prcSrc->left + x;
				lMskIndex = prcMsk->top * nImgMskPitch + prcMsk->left + x;
				lDstIndex = prcDst->top * nImgDstPitch + prcDst->left + x;
				byteSelectVal = 255;
				lSrcIndexSlide = lSrcIndex;
				lMskIndexSlide = lMskIndex;
				for(k=0;k<nWidth;k++) {
					nValue = pImgSrcPtr[lSrcIndexSlide];
					if((byteSelectVal > nValue) && (pImgMskPtr[lMskIndexSlide] > 0))
						byteSelectVal = nValue;
					lSrcIndexSlide += nImgSrcPitch;
					lMskIndexSlide += nImgMskPitch;
				}
				pImgDstPtr[lDstIndex + nWidthSplit0 * nImgDstPitch] = byteSelectVal;
				for(y=0;y<nWidth;y++)
					pbMask[y] = FALSE;
				for(y=nWidth;y<prcSrc->Height();y++)
					pbMask[y] = TRUE;
				lMskIndex = (prcMsk->top+nWidthSplit0+1) * nImgDstPitch + prcMsk->left + x;
				for(y=nWidthSplit0+1;y<(prcSrc->Height() - nWidthSplit1 - 1);y++) {
					if(pImgMskPtr[lMskIndex]) {
						lSrcIndexMax = lSrcIndex + nWidthTimesnImgSrcPitch;
						lDstIndexSlide = lDstIndex + nWidthSplit0Add1TimesnImgDstPitch;
						nValue = pImgSrcPtr[lSrcIndexMax];
						if((pImgDstPtr[lDstIndexSlide - nImgDstPitch] >= nValue) && (pImgMskPtr[lMskIndex+x+nWidthSplit1 * nImgMskPitch]))
							pImgDstPtr[lDstIndexSlide] = nValue;
						else {
							nValue = pImgDstPtr[lDstIndexSlide - nImgDstPitch];
							if((nValue < pImgSrcPtr[lSrcIndex]) && (nValue > 0) && (pbMask[y]))
								pImgDstPtr[lDstIndexSlide] = nValue;
							else {
								byteSelectVal = 255;
								lSrcIndexSlide = lSrcIndex + nImgSrcPitch;
								lMskIndexSlide = lMskIndex - (nWidthSplit0) * nImgMskPitch;
								for(k=0;k<nWidth;k++) {
									nValue = pImgSrcPtr[lSrcIndexSlide];
									if((byteSelectVal > nValue) && (pImgMskPtr[lMskIndexSlide]))
										byteSelectVal = nValue;
									lSrcIndexSlide += nImgSrcPitch;
									lMskIndexSlide += nImgMskPitch;
								}
								pImgDstPtr[lDstIndexSlide] = byteSelectVal;
							}
						}
					}
					else {
						for(k=0;k<nWidthSplit1;k++)
							pbMask[y + k] = FALSE;
					}
					lSrcIndex += nImgSrcPitch;
					lMskIndex += nImgMskPitch;
					lDstIndex += nImgDstPitch;
				}
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::Morphology1D -1");
	goto ErrorAbort;
}
PASS:
	if(pbMask != NULL)
		delete [] pbMask;
	return 0;
ErrorAbort:
	if(pbMask != NULL)
		delete [] pbMask;
	return -1;
}

int CMorphology::ProcessBySampling(CImageBuffer* pImgIn,
					CRect* prcIn,
					int nImgType,
					int nProcType,
					CImageBuffer* pKernel,
					CRect* prcKernel,
					CImageBuffer* pImgMask,
					CImageBuffer* pImgOut,
					CRect* prcOut,
					CSize* pcsSampling,
					int nThreshold)
{
	int nError;
try
{
	Clean();
	nError = ValidCheck(pImgIn,
					prcIn,
					nImgType,
					nProcType,
					pKernel,
					prcKernel,
					pImgOut,
					prcOut);
	if(nError)
		goto ErrorAbort;

	if (pImgMask!=NULL && (!pImgMask->IsAllocated() || pImgMask->GetImgSize() != pImgOut->GetImgSize())) {
		ASSERT(!"ERROR in parameter pImgMask");
		goto ErrorAbort;
	}

	nError = KernelAnalysis(pImgIn,
							prcIn,
							pKernel,
							prcKernel);
	if(nError)
		goto ErrorAbort;

	if (nThreshold<0 || nThreshold>=256)
		nThreshold=-1;

	if(pImgMask !=  NULL) {
		switch (nProcType) {
			case DILATION:
				nError = GrayDilationProcess(pImgIn,
					prcIn,
					pImgMask,
					pImgOut,
					prcOut,
					nThreshold);
				if(nError)
					goto ErrorAbort;
				break;
			case EROSION:
				nError = GrayErosionProcessBySampling(pImgIn,
					prcIn,
					pImgMask,
					pImgOut,
					prcOut,
					pcsSampling,
					nThreshold);
				if(nError)
					goto ErrorAbort;
				break;
			case AVERAGING:
				nError = GrayAveragingProcess(pImgIn,
					prcIn,
					pImgMask,
					pImgOut,
					prcOut,
					nThreshold);
				if(nError)
					goto ErrorAbort;
				break;
		}
	}
	else {
		switch (nProcType) {
			case DILATION:
				nError = GrayDilationProcess(pImgIn,
					prcIn,
					pImgOut,
					prcOut,
					nThreshold);
				if(nError)
					goto ErrorAbort;
				break;
			case EROSION:
				nError = GrayErosionProcess(pImgIn,
					prcIn,
					pImgOut,
					prcOut,
					nThreshold);
				if(nError)
					goto ErrorAbort;
				break;
			case AVERAGING:
				nError = GrayAveragingProcess(pImgIn,
					prcIn,
					pImgOut,
					prcOut,
					nThreshold);
				if(nError)
					goto ErrorAbort;
				break;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::ProcessBySampling");
	goto ErrorAbort;
}	
	return 0;

ErrorAbort:
	return -1;
}

int CMorphology::ValidCheck(CImageBuffer* pImgIn,
					CRect* prcIn,
					int nImgType,
					int nProcType,
					CImageBuffer* pKernel,
					CRect* prcKernel,
					CImageBuffer* pImgOut,
					CRect* prcOut)
{
	
try
{
	if (!pImgIn) 
		goto ErrorAbort;
	if (!pImgIn->IsAllocated()) 		
		goto ErrorAbort;
	if (ValidateRect(prcIn,&(pImgIn->GetImgSize()))) 	
		goto ErrorAbort;
	if (!pKernel) 
		goto ErrorAbort;
	if (!pKernel->IsAllocated()) 
		goto ErrorAbort;
	if (ValidateRect(prcKernel,&(pKernel->GetImgSize()))) 
		goto ErrorAbort;
	if (!pImgOut) 
		goto ErrorAbort;
	if (!pImgOut->IsAllocated()) 
		goto ErrorAbort;
	if (ValidateRect(prcOut,&(pImgOut->GetImgSize()))) 
		goto ErrorAbort;
	if(prcIn->Size() != prcOut->Size())
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::ValidCheck");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMorphology::KernelAnalysis(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pKernel,
					CRect* prcKernel)
{
	int i, j;
	BYTE byteVal;
	int nPitchImgIn;
	int nError;

try
{
	nPitchImgIn = pImgIn->GetPitch();
	nKernelWidth = prcKernel->Width();
	if(nKernelWidth <= 0)
		nKernelWidth = 1;
	nKernelHeight = prcKernel->Height();
	if(nKernelHeight <= 0)
		nKernelHeight = 1;
	nKernelCtrX = nKernelWidth/2;
	nKernelCtrY = nKernelHeight/2;
	if(pnKernelAddress != NULL)
		delete [] pnKernelAddress;
	pnKernelAddress = new int[abs(nKernelWidth * nKernelHeight)];
	nKernelAddress = 0;
	for(i=0;i<nKernelWidth;i++) {
		for(j=0;j<nKernelHeight;j++) {
			nError = pKernel->GetPixelVal(i, j, byteVal);
			if(nError)
				goto ErrorAbort;
			if(byteVal > 0)
				pnKernelAddress[nKernelAddress++] = (j-nKernelCtrY)*nPitchImgIn + (i-nKernelCtrX);
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::KernelAnalysis");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CMorphology::GrayProcess(CImageBuffer* pImgIn,
					CRect* prcIn,
					int nImgType,
					int nProcType,
					CImageBuffer* pImgOut,
					CRect* prcOut)
{
	
	int nError;
try
{
	switch (nProcType) {
		case DILATION:
			nError = GrayDilationProcess(pImgIn,
							prcIn,
							pImgOut,
							prcOut);
			if(nError)
				goto ErrorAbort;
			break;
		case EROSION:
			nError = GrayErosionProcess(pImgIn,
							prcIn,
							pImgOut,
							prcOut);
			if(nError)
				goto ErrorAbort;
			break;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::GrayProcess");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMorphology::GrayDilationProcess(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgOut,
					CRect* prcOut)
{
	int i, j, k;
	CRect rcImgInProc;
	CRect rcImgOutProc;
	BYTE byteMaxVal;
	BYTE* pImgInPtr;
	int nImgInPitch;
	int nIndexImgInRow;
	int nIndexImgIn;
	BYTE* pImgOutPtr;
	int nImgOutPitch;
	int nIndexImgOutRow;
	CRect rcInEdge[4];
	CRect rcOutEdge[4];
	int nError;

try
{
	rcImgInProc = CRect(prcIn->left + nKernelCtrX, 
				   prcIn->top + nKernelCtrY,
				   prcIn->right - (nKernelWidth - nKernelCtrX),
				   prcIn->bottom - (nKernelHeight - nKernelCtrY));
	rcImgOutProc = CRect(prcOut->left + nKernelCtrX, 
				   prcOut->top + nKernelCtrY,
				   prcOut->right - (nKernelWidth - nKernelCtrX),
				   prcOut->bottom - (nKernelHeight - nKernelCtrY));
	pImgInPtr = pImgIn->GetImgPtr();
	nImgInPitch = pImgIn->GetPitch();
	pImgOutPtr = pImgOut->GetImgPtr();
	nImgOutPitch = pImgOut->GetPitch();
	if((rcImgInProc.Width() > 0) && (rcImgInProc.Height() > 0)) {
		nIndexImgInRow = rcImgInProc.top * nImgInPitch + rcImgInProc.left;
		nIndexImgOutRow = rcImgOutProc.top * nImgOutPitch + rcImgOutProc.left;
		for(i=0;i<rcImgInProc.Height();i++) {
			for(j=0;j<rcImgInProc.Width();j++) {
				byteMaxVal = 0;
				nIndexImgIn = nIndexImgInRow + j;
				for(k=0;k<nKernelAddress;k++) {
					if(pImgInPtr[nIndexImgIn + pnKernelAddress[k]] > byteMaxVal)
						byteMaxVal = pImgInPtr[nIndexImgIn + pnKernelAddress[k]];
				}
				pImgOutPtr[nIndexImgOutRow + j] = byteMaxVal;
			}
			nIndexImgInRow += nImgInPitch;
			nIndexImgOutRow += nImgOutPitch;
		}
	}
	rcInEdge[0] = CRect(prcIn->left, prcIn->top, prcIn->left + nKernelCtrX, prcIn->bottom);
	rcInEdge[1] = CRect(prcIn->left, prcIn->top, prcIn->right, prcIn->top + nKernelCtrY);
	rcInEdge[2] = CRect(prcIn->right - (nKernelWidth - nKernelCtrX), prcIn->top, prcIn->right, prcIn->bottom);
	rcInEdge[3] = CRect(prcIn->left, prcIn->bottom - (nKernelHeight - nKernelCtrY), prcIn->right, prcIn->bottom);
	rcOutEdge[0] = CRect(prcOut->left, prcOut->top, prcOut->left + nKernelCtrX, prcOut->bottom);
	rcOutEdge[1] = CRect(prcOut->left, prcOut->top, prcOut->right, prcOut->top + nKernelCtrY);
	rcOutEdge[2] = CRect(prcOut->right - (nKernelWidth - nKernelCtrX), prcOut->top, prcOut->right, prcOut->bottom);
	rcOutEdge[3] = CRect(prcOut->left, prcOut->bottom - (nKernelHeight - nKernelCtrY), prcOut->right, prcOut->bottom);
	for(i=0;i<4;i++) {
		nError = ImgCopy(pImgIn,
						 &rcInEdge[i],
						 pImgOut,
						 &rcOutEdge[i]);
		if(nError)
			goto ErrorAbort;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::GrayDilationProcess");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMorphology::GrayDilationProcess(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgOut,
					CRect* prcOut,
					int nThreshold)
{
	int i, j, k;
	CRect rcImgInProc;
	CRect rcImgOutProc;
	BYTE byteMaxVal;
	BYTE* pImgInPtr;
	int nImgInPitch;
	int nIndexImgInRow;
	int nIndexImgIn;
	BYTE* pImgOutPtr;
	int nImgOutPitch;
	int nIndexImgOutRow;
	//int nIndexImgOut;

try
{
	CRect rcInEdge[4];
	CRect rcOutEdge[4];
	int nError;

	rcImgInProc = CRect(prcIn->left + nKernelCtrX, 
				   prcIn->top + nKernelCtrY,
				   prcIn->right - (nKernelWidth - nKernelCtrX),
				   prcIn->bottom - (nKernelHeight - nKernelCtrY));
	rcImgOutProc = CRect(prcOut->left + nKernelCtrX, 
				   prcOut->top + nKernelCtrY,
				   prcOut->right - (nKernelWidth - nKernelCtrX),
				   prcOut->bottom - (nKernelHeight - nKernelCtrY));
	pImgInPtr = pImgIn->GetImgPtr();
	nImgInPitch = pImgIn->GetPitch();
	pImgOutPtr = pImgOut->GetImgPtr();
	nImgOutPitch = pImgOut->GetPitch();
	if((rcImgInProc.Width() > 0) && (rcImgInProc.Height() > 0)) {
		nIndexImgInRow = rcImgInProc.top * nImgInPitch + rcImgInProc.left;
		nIndexImgOutRow = rcImgOutProc.top * nImgOutPitch + rcImgOutProc.left;
		if(nThreshold==-1) {
			for(i=0;i<rcImgInProc.Height();i++) {
				for(j=0;j<rcImgInProc.Width();j++) {
					byteMaxVal = 0;
					nIndexImgIn = nIndexImgInRow + j;
					//nIndexImgOut = nIndexImgOutRow + j;
					for(k=0;k<nKernelAddress;k++) {
						if(pImgInPtr[nIndexImgIn + pnKernelAddress[k]] > byteMaxVal)
							byteMaxVal = pImgInPtr[nIndexImgIn + pnKernelAddress[k]];
					}
					pImgOutPtr[nIndexImgOutRow + j] = byteMaxVal;
				}
				nIndexImgInRow += nImgInPitch;
				nIndexImgOutRow += nImgOutPitch;
			}
		}
		else {
			pImgOut->ClrImg(0);
			for(i=0;i<rcImgInProc.Height();i++) {
				for(j=0;j<rcImgInProc.Width();j++) {
					byteMaxVal = 0;
					nIndexImgIn = nIndexImgInRow + j;
					//nIndexImgOut = nIndexImgOutRow + j;
					for(k=0;k<nKernelAddress;k++) {
						if(pImgInPtr[nIndexImgIn + pnKernelAddress[k]] > byteMaxVal)
							byteMaxVal = pImgInPtr[nIndexImgIn + pnKernelAddress[k]];
					}
					if((byteMaxVal > pImgInPtr[nIndexImgIn]) && ((byteMaxVal - pImgInPtr[nIndexImgIn]) >= nThreshold))
						pImgOutPtr[nIndexImgOutRow + j] = 255;
					//if((byteMaxVal > pImgInPtr[nIndexImgIn]))
					//	pImgOutPtr[nIndexImgOut] = byteMaxVal - pImgInPtr[nIndexImgIn];
					//pImgOutPtr[nIndexImgOut] = byteMaxVal;
					//else
					//	pImgOutPtr[nIndexImgOutRow + j] = 0;
				}
				nIndexImgInRow += nImgInPitch;
				nIndexImgOutRow += nImgOutPitch;
			}
		}
	}
	if(nThreshold==-1) {
		rcInEdge[0] = CRect(prcIn->left, prcIn->top, prcIn->left + nKernelCtrX, prcIn->bottom);
		rcInEdge[1] = CRect(prcIn->left, prcIn->top, prcIn->right, prcIn->top + nKernelCtrY);
		rcInEdge[2] = CRect(prcIn->right - (nKernelWidth - nKernelCtrX), prcIn->top, prcIn->right, prcIn->bottom);
		rcInEdge[3] = CRect(prcIn->left, prcIn->bottom - (nKernelHeight - nKernelCtrY), prcIn->right, prcIn->bottom);
		rcOutEdge[0] = CRect(prcOut->left, prcOut->top, prcOut->left + nKernelCtrX, prcOut->bottom);
		rcOutEdge[1] = CRect(prcOut->left, prcOut->top, prcOut->right, prcOut->top + nKernelCtrY);
		rcOutEdge[2] = CRect(prcOut->right - (nKernelWidth - nKernelCtrX), prcOut->top, prcOut->right, prcOut->bottom);
		rcOutEdge[3] = CRect(prcOut->left, prcOut->bottom - (nKernelHeight - nKernelCtrY), prcOut->right, prcOut->bottom);
		for(i=0;i<4;i++) {
			nError = ImgCopy(pImgIn,
				&rcInEdge[i],
				pImgOut,
				&rcOutEdge[i]);
			if(nError)
				goto ErrorAbort;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::GrayDilationProcess -1");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMorphology::GrayDilationProcess(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgMask,
					CImageBuffer* pImgOut,
					CRect* prcOut,
					int nThreshold)
{
	int i, j, k;
	CRect rcImgInProc;
	CRect rcImgOutProc;
	BYTE byteMaxVal;
	BYTE* pImgInPtr;
	int nImgInPitch;
	int nIndexImgInRow;
	int nIndexImgIn;
	BYTE* pImgMaskPtr;
	BYTE* pImgOutPtr;
	int nImgOutPitch;
	int nIndexImgOutRow;
	//int nIndexImgOut;

try
{
	CRect rcInEdge[4];
	CRect rcOutEdge[4];
	int nError;

	rcImgInProc = CRect(prcIn->left + nKernelCtrX, 
				   prcIn->top + nKernelCtrY,
				   prcIn->right - (nKernelWidth - nKernelCtrX),
				   prcIn->bottom - (nKernelHeight - nKernelCtrY));
	rcImgOutProc = CRect(prcOut->left + nKernelCtrX, 
				   prcOut->top + nKernelCtrY,
				   prcOut->right - (nKernelWidth - nKernelCtrX),
				   prcOut->bottom - (nKernelHeight - nKernelCtrY));
	pImgInPtr = pImgIn->GetImgPtr();
	nImgInPitch = pImgIn->GetPitch();
	pImgOutPtr = pImgOut->GetImgPtr();
	pImgMaskPtr = pImgMask->GetImgPtr();
	nImgOutPitch = pImgOut->GetPitch();
	if((rcImgInProc.Width() > 0) && (rcImgInProc.Height() > 0)) {
		nIndexImgInRow = rcImgInProc.top * nImgInPitch + rcImgInProc.left;
		nIndexImgOutRow = rcImgOutProc.top * nImgOutPitch + rcImgOutProc.left;
		if(nThreshold==-1) {
			for(i=0;i<rcImgInProc.Height();i++) {
				for(j=0;j<rcImgInProc.Width();j++) {
					if(pImgMaskPtr[nIndexImgOutRow + j] > 0) {
						byteMaxVal = 0;
						nIndexImgIn = nIndexImgInRow + j;
						//nIndexImgOut = nIndexImgOutRow + j;
						for(k=0;k<nKernelAddress;k++) {
							if(//(pImgMaskPtr[nIndexImgOut + pnKernelAddress[k]] > 0) &&
									(pImgInPtr[nIndexImgIn + pnKernelAddress[k]] > byteMaxVal))
								byteMaxVal = pImgInPtr[nIndexImgIn + pnKernelAddress[k]];
						}
						pImgOutPtr[nIndexImgOutRow + j] = byteMaxVal;
					}
				}
				nIndexImgInRow += nImgInPitch;
				nIndexImgOutRow += nImgOutPitch;
			}
		}
		else {
			pImgOut->ClrImg(0);
			for(i=0;i<rcImgInProc.Height();i++) {
				for(j=0;j<rcImgInProc.Width();j++) {
					if(pImgMaskPtr[nIndexImgOutRow + j] > 0) {
						byteMaxVal = 0;
						nIndexImgIn = nIndexImgInRow + j;
						//nIndexImgOut = nIndexImgOutRow + j;
						for(k=0;k<nKernelAddress;k++) {
							if(//(pImgMaskPtr[nIndexImgOut + pnKernelAddress[k]] > 0) &&
									(pImgInPtr[nIndexImgIn + pnKernelAddress[k]] > byteMaxVal))
								byteMaxVal = pImgInPtr[nIndexImgIn + pnKernelAddress[k]];
						}
						if((byteMaxVal > pImgInPtr[nIndexImgIn]) && ((byteMaxVal - pImgInPtr[nIndexImgIn]) >= nThreshold))
							pImgOutPtr[nIndexImgOutRow + j] = 255;
						//if((byteMaxVal > pImgInPtr[nIndexImgIn]))
						//	pImgOutPtr[nIndexImgOut] = byteMaxVal - pImgInPtr[nIndexImgIn];
						//pImgOutPtr[nIndexImgOut] = byteMaxVal;
						//else
						//	pImgOutPtr[nIndexImgOutRow + j] = 0;
					}
					//else
					//	pImgOutPtr[nIndexImgOutRow + j] = 0;
				}
				nIndexImgInRow += nImgInPitch;
				nIndexImgOutRow += nImgOutPitch;
			}
		}
	}
	if(nThreshold==-1) {
		rcInEdge[0] = CRect(prcIn->left, prcIn->top, prcIn->left + nKernelCtrX, prcIn->bottom);
		rcInEdge[1] = CRect(prcIn->left, prcIn->top, prcIn->right, prcIn->top + nKernelCtrY);
		rcInEdge[2] = CRect(prcIn->right - (nKernelWidth - nKernelCtrX), prcIn->top, prcIn->right, prcIn->bottom);
		rcInEdge[3] = CRect(prcIn->left, prcIn->bottom - (nKernelHeight - nKernelCtrY), prcIn->right, prcIn->bottom);
		rcOutEdge[0] = CRect(prcOut->left, prcOut->top, prcOut->left + nKernelCtrX, prcOut->bottom);
		rcOutEdge[1] = CRect(prcOut->left, prcOut->top, prcOut->right, prcOut->top + nKernelCtrY);
		rcOutEdge[2] = CRect(prcOut->right - (nKernelWidth - nKernelCtrX), prcOut->top, prcOut->right, prcOut->bottom);
		rcOutEdge[3] = CRect(prcOut->left, prcOut->bottom - (nKernelHeight - nKernelCtrY), prcOut->right, prcOut->bottom);
		for(i=0;i<4;i++) {
			nError = ImgCopy(pImgIn,
				&rcInEdge[i],
				pImgOut,
				&rcOutEdge[i]);
			if(nError)
				goto ErrorAbort;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::GrayDilationProcess -2");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMorphology::GrayErosionProcess(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgOut,
					CRect* prcOut)
{
	int i, j, k;
	CRect rcImgInProc;
	CRect rcImgOutProc;
	BYTE byteMinVal;
	BYTE* pImgInPtr;
	int nImgInPitch;
	int nIndexImgInRow;
	int nIndexImgIn;
	BYTE* pImgOutPtr;
	int nImgOutPitch;
	int nIndexImgOutRow;

try
{
	rcImgInProc = CRect(prcIn->left + nKernelCtrX, 
				   prcIn->top + nKernelCtrY,
				   prcIn->right - (nKernelWidth - nKernelCtrX),
				   prcIn->bottom - (nKernelHeight - nKernelCtrY));
	rcImgOutProc = CRect(prcOut->left + nKernelCtrX, 
				   prcOut->top + nKernelCtrY,
				   prcOut->right - (nKernelWidth - nKernelCtrX),
				   prcOut->bottom - (nKernelHeight - nKernelCtrY));
	pImgInPtr = pImgIn->GetImgPtr();
	nImgInPitch = pImgIn->GetPitch();
	pImgOutPtr = pImgOut->GetImgPtr();
	nImgOutPitch = pImgOut->GetPitch();
	if((rcImgInProc.Width() > 0) && (rcImgInProc.Height() > 0)) {
		nIndexImgInRow = rcImgInProc.top * nImgInPitch + rcImgInProc.left;
		nIndexImgOutRow = rcImgOutProc.top * nImgOutPitch + rcImgOutProc.left;
		for(i=0;i<rcImgInProc.Height();i++) {
			for(j=0;j<rcImgInProc.Width();j++) {
				byteMinVal = 255;
				nIndexImgIn = nIndexImgInRow + j;
				for(k=0;k<nKernelAddress;k++) {
					if(pImgInPtr[nIndexImgIn + pnKernelAddress[k]] < byteMinVal)
						byteMinVal = pImgInPtr[nIndexImgIn + pnKernelAddress[k]];
				}
				pImgOutPtr[nIndexImgOutRow + j] = byteMinVal;
			}
			nIndexImgInRow += nImgInPitch;
			nIndexImgOutRow += nImgOutPitch;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::GrayErosionProcess");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMorphology::GrayErosionProcess(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgOut,
					CRect* prcOut,
					int nThreshold)
{
	int i, j, k;
	CRect rcImgInProc;
	CRect rcImgOutProc;
	BYTE byteMinVal;
	BYTE* pImgInPtr;
	int nImgInPitch;
	int nIndexImgInRow;
	int nIndexImgIn;
	BYTE* pImgOutPtr;
	int nImgOutPitch;
	int nIndexImgOutRow;
//	int nIndexImgOut;

try
{
	rcImgInProc = CRect(prcIn->left + nKernelCtrX, 
				   prcIn->top + nKernelCtrY,
				   prcIn->right - (nKernelWidth - nKernelCtrX),
				   prcIn->bottom - (nKernelHeight - nKernelCtrY));
	rcImgOutProc = CRect(prcOut->left + nKernelCtrX, 
				   prcOut->top + nKernelCtrY,
				   prcOut->right - (nKernelWidth - nKernelCtrX),
				   prcOut->bottom - (nKernelHeight - nKernelCtrY));
	pImgInPtr = pImgIn->GetImgPtr();
	nImgInPitch = pImgIn->GetPitch();
	pImgOutPtr = pImgOut->GetImgPtr();
	nImgOutPitch = pImgOut->GetPitch();
	if((rcImgInProc.Width() > 0) && (rcImgInProc.Height() > 0)) {
		nIndexImgInRow = rcImgInProc.top * nImgInPitch + rcImgInProc.left;
		nIndexImgOutRow = rcImgOutProc.top * nImgOutPitch + rcImgOutProc.left;
		if(nThreshold==-1) {
			for(i=0;i<rcImgInProc.Height();i++) {
				for(j=0;j<rcImgInProc.Width();j++) {
					byteMinVal = 255;
					nIndexImgIn = nIndexImgInRow + j;
					//nIndexImgOut = nIndexImgOutRow + j;
					for(k=0;k<nKernelAddress;k++) {
						if(pImgInPtr[nIndexImgIn + pnKernelAddress[k]] < byteMinVal)
							byteMinVal = pImgInPtr[nIndexImgIn + pnKernelAddress[k]];
					}
					pImgOutPtr[nIndexImgOutRow + j] = byteMinVal;
				}
				nIndexImgInRow += nImgInPitch;
				nIndexImgOutRow += nImgOutPitch;
			}
		}
		else {
			pImgOut->ClrImg(0);
			for(i=0;i<rcImgInProc.Height();i++) {
				for(j=0;j<rcImgInProc.Width();j++) {
					byteMinVal = 255;
					nIndexImgIn = nIndexImgInRow + j;
					//nIndexImgOut = nIndexImgOutRow + j;
					for(k=0;k<nKernelAddress;k++) {
						if(pImgInPtr[nIndexImgIn + pnKernelAddress[k]] < byteMinVal)
							byteMinVal = pImgInPtr[nIndexImgIn + pnKernelAddress[k]];
					}
					if((pImgInPtr[nIndexImgIn] > byteMinVal) && ((pImgInPtr[nIndexImgIn] - byteMinVal) >= nThreshold))
						pImgOutPtr[nIndexImgOutRow + j] = 255;
				}
				nIndexImgInRow += nImgInPitch;
				nIndexImgOutRow += nImgOutPitch;
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::GrayErosionProcess -1");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMorphology::GrayErosionProcessBySampling(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgMask,
					CImageBuffer* pImgOut,
					CRect* prcOut,
					CSize* pcsSampling,
					int nThreshold)
{
	int i, j, k;
	CRect rcImgInProc;
	CRect rcImgOutProc;
	BYTE byteMinVal;
	BYTE* pImgInPtr;
	int nImgInPitch;
	int nIndexImgInRow;
	int nIndexImgIn;
	BYTE* pImgMaskPtr;
	BYTE* pImgOutPtr;
	int nImgOutPitch;
	int nIndexImgOutRow;
	int nSamplingX;
	int nSamplingY;
	int nImgInPitchSampling;
	int nImgOutPitchSampling;
	long* plSamplingOffsetAddressTab;
	int nSamplingOffsetNo;
	int nIndexImgOut;

try
{
	plSamplingOffsetAddressTab = NULL;
	nSamplingX = pcsSampling->cx;
	nSamplingY = pcsSampling->cy;
	rcImgInProc = CRect(prcIn->left + nKernelCtrX, 
				   prcIn->top + nKernelCtrY,
				   prcIn->right - (nKernelWidth - nKernelCtrX),
				   prcIn->bottom - (nKernelHeight - nKernelCtrY));
	rcImgOutProc = CRect(prcOut->left + nKernelCtrX, 
				   prcOut->top + nKernelCtrY,
				   prcOut->right - (nKernelWidth - nKernelCtrX),
				   prcOut->bottom - (nKernelHeight - nKernelCtrY));
	pImgInPtr = pImgIn->GetImgPtr();
	nImgInPitch = pImgIn->GetPitch();
	pImgMaskPtr = pImgMask->GetImgPtr();
	pImgOutPtr = pImgOut->GetImgPtr();
	nImgOutPitch = pImgOut->GetPitch();
	nImgInPitchSampling = nImgInPitch * nSamplingY;
	nImgOutPitchSampling = nImgOutPitch * nSamplingY;

	nSamplingOffsetNo = (2*nSamplingX+1) * (2*nSamplingY+1);
	plSamplingOffsetAddressTab = new long[abs(nSamplingOffsetNo)];
	nSamplingOffsetNo = 0;
	for(i=-nSamplingY;i<=nSamplingY;i++) {
		for(j=-nSamplingX;j<=nSamplingX;j++) {
			plSamplingOffsetAddressTab[nSamplingOffsetNo++] = i*nImgOutPitch + j;
		}
	}

	if((rcImgInProc.Width() > 0) && (rcImgInProc.Height() > 0)) {
		nIndexImgInRow = rcImgInProc.top * nImgInPitch + rcImgInProc.left;
		nIndexImgOutRow = rcImgOutProc.top * nImgOutPitch + rcImgOutProc.left;
		if(nThreshold==-1) {
			for(i=0;i<rcImgInProc.Height();i+=nSamplingY) {
				for(j=0;j<rcImgInProc.Width();j+=nSamplingX) {
					if(pImgMaskPtr[nIndexImgOutRow + j] > 0) {
						byteMinVal = 255;
						nIndexImgIn = nIndexImgInRow + j;
						//nIndexImgOut = nIndexImgOutRow + j;
						for(k=0;k<nKernelAddress;k++) {
							if(//(pImgMaskPtr[nIndexImgOut + pnKernelAddress[k]] > 0) &&
								(pImgInPtr[nIndexImgIn + pnKernelAddress[k]] < byteMinVal))
								byteMinVal = pImgInPtr[nIndexImgIn + pnKernelAddress[k]];
						}
						nIndexImgOut = nIndexImgOutRow + j;
						for(k=0;k<nSamplingOffsetNo;k++)
							pImgOutPtr[nIndexImgOut + plSamplingOffsetAddressTab[k]] = byteMinVal;
					}
				}
				nIndexImgInRow += nImgInPitchSampling;
				nIndexImgOutRow += nImgOutPitchSampling;
			}
		}
		else {
			pImgOut->ClrImg(0);
			for(i=0;i<rcImgInProc.Height();i++) {
				for(j=0;j<rcImgInProc.Width();j++) {
					if(pImgMaskPtr[nIndexImgOutRow + j] > 0) {
						byteMinVal = 255;
						nIndexImgIn = nIndexImgInRow + j;
						//nIndexImgOut = nIndexImgOutRow + j;
						for(k=0;k<nKernelAddress;k++) {
							if(//(pImgMaskPtr[nIndexImgOut + pnKernelAddress[k]] > 0) &&
								(pImgInPtr[nIndexImgIn + pnKernelAddress[k]] < byteMinVal))
								byteMinVal = pImgInPtr[nIndexImgIn + pnKernelAddress[k]];
						}
						if((pImgInPtr[nIndexImgIn] > byteMinVal) && ((pImgInPtr[nIndexImgIn] - byteMinVal) >= nThreshold))
							pImgOutPtr[nIndexImgOutRow + j] = 255;
					}
				}
				nIndexImgInRow += nImgInPitch;
				nIndexImgOutRow += nImgOutPitch;
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::GrayErosionProcessBySampling");
	goto ErrorAbort;
}
	if(plSamplingOffsetAddressTab != NULL)
		delete [] plSamplingOffsetAddressTab;
	return 0;
ErrorAbort:
	if(plSamplingOffsetAddressTab != NULL)
		delete [] plSamplingOffsetAddressTab;
	return -1;
}

int CMorphology::GrayAveragingProcess(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgMask,
					CImageBuffer* pImgOut,
					CRect* prcOut,
					int nThreshold)
{
	int i, j, k;
	CRect rcImgInProc;
	CRect rcImgOutProc;
	int nAvgValue;
	BYTE* pImgInPtr;
	int nImgInPitch;
	int nIndexImgInRow;
	int nIndexImgIn;
	BYTE* pImgMaskPtr;
	BYTE* pImgOutPtr;
	int nImgOutPitch;
	int nIndexImgOutRow;
	//int nIndexImgOut;
	
	CRect rcInEdge[4];
	CRect rcOutEdge[4];
	int nError;

try
{
	rcImgInProc = CRect(prcIn->left + nKernelCtrX, 
				   prcIn->top + nKernelCtrY,
				   prcIn->right - (nKernelWidth - nKernelCtrX),
				   prcIn->bottom - (nKernelHeight - nKernelCtrY));
	rcImgOutProc = CRect(prcOut->left + nKernelCtrX, 
				   prcOut->top + nKernelCtrY,
				   prcOut->right - (nKernelWidth - nKernelCtrX),
				   prcOut->bottom - (nKernelHeight - nKernelCtrY));
	pImgInPtr = pImgIn->GetImgPtr();
	nImgInPitch = pImgIn->GetPitch();
	pImgOutPtr = pImgOut->GetImgPtr();
	pImgMaskPtr = pImgMask->GetImgPtr();
	nImgOutPitch = pImgOut->GetPitch();
	if((rcImgInProc.Width() > 0) && (rcImgInProc.Height() > 0)) {
		nIndexImgInRow = rcImgInProc.top * nImgInPitch + rcImgInProc.left;
		nIndexImgOutRow = rcImgOutProc.top * nImgOutPitch + rcImgOutProc.left;
		if(nThreshold==-1) {
			pImgOut->ClrImg(0);
			for(i=0;i<rcImgInProc.Height();i++) {
				for(j=0;j<rcImgInProc.Width();j++) {
					if(pImgMaskPtr[nIndexImgOutRow + j] > 0) {
						nAvgValue = 0;
						nIndexImgIn = nIndexImgInRow + j;
						//nIndexImgOut = nIndexImgOutRow + j;
						for(k=0;k<nKernelAddress;k++) {
							//if(//(pImgMaskPtr[nIndexImgOut + pnKernelAddress[k]] > 0) &&
							//		(pImgInPtr[nIndexImgIn + pnKernelAddress[k]] > nAvgValue))
								nAvgValue += pImgInPtr[nIndexImgIn + pnKernelAddress[k]];
						}
						nAvgValue /= nKernelArea;
						if((pImgOutPtr[nIndexImgOutRow + j] - nAvgValue) < 20)
							pImgOutPtr[nIndexImgOutRow + j] = nAvgValue;
						else
							pImgOutPtr[nIndexImgOutRow + j] = pImgInPtr[nIndexImgIn];
					}
				}
				nIndexImgInRow += nImgInPitch;
				nIndexImgOutRow += nImgOutPitch;
			}
		}
		else {
			pImgOut->ClrImg(0);
			for(i=0;i<rcImgInProc.Height();i++) {
				for(j=0;j<rcImgInProc.Width();j++) {
					if(pImgMaskPtr[nIndexImgOutRow + j] > 0) {
						nAvgValue = 0;
						nIndexImgIn = nIndexImgInRow + j;
						//nIndexImgOut = nIndexImgOutRow + j;
						for(k=0;k<nKernelAddress;k++) {
							//if(//(pImgMaskPtr[nIndexImgOut + pnKernelAddress[k]] > 0) &&
							//		(pImgInPtr[nIndexImgIn + pnKernelAddress[k]] > nAvgValue))
								nAvgValue += pImgInPtr[nIndexImgIn + pnKernelAddress[k]];
						}
						nAvgValue /= nKernelArea;
						if((pImgOutPtr[nIndexImgOutRow + j] - nAvgValue) >= 20)
							nAvgValue = pImgInPtr[nIndexImgIn];
						pImgOutPtr[nIndexImgOutRow + j] = (nAvgValue >= nThreshold) ? 255 : 0;
					}
				}
				nIndexImgInRow += nImgInPitch;
				nIndexImgOutRow += nImgOutPitch;
			}
		}
	}
	rcInEdge[0] = CRect(prcIn->left, prcIn->top, prcIn->left + nKernelCtrX, prcIn->bottom);
	rcInEdge[1] = CRect(prcIn->left, prcIn->top, prcIn->right, prcIn->top + nKernelCtrY);
	rcInEdge[2] = CRect(prcIn->right - (nKernelWidth - nKernelCtrX), prcIn->top, prcIn->right, prcIn->bottom);
	rcInEdge[3] = CRect(prcIn->left, prcIn->bottom - (nKernelHeight - nKernelCtrY), prcIn->right, prcIn->bottom);
	rcOutEdge[0] = CRect(prcOut->left, prcOut->top, prcOut->left + nKernelCtrX, prcOut->bottom);
	rcOutEdge[1] = CRect(prcOut->left, prcOut->top, prcOut->right, prcOut->top + nKernelCtrY);
	rcOutEdge[2] = CRect(prcOut->right - (nKernelWidth - nKernelCtrX), prcOut->top, prcOut->right, prcOut->bottom);
	rcOutEdge[3] = CRect(prcOut->left, prcOut->bottom - (nKernelHeight - nKernelCtrY), prcOut->right, prcOut->bottom);
	for(i=0;i<4;i++) {
		nError = ImgCopy(pImgIn,
						 &rcInEdge[i],
						 pImgOut,
						 &rcOutEdge[i]);
		if(nError)
			goto ErrorAbort;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::GrayAveragingProcess");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMorphology::GrayAveragingProcess(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgOut,
					CRect* prcOut,
					int nThreshold)
{
	int i, j, k;
	CRect rcImgInProc;
	CRect rcImgOutProc;
	int nAvgValue;
	BYTE* pImgInPtr;
	int nImgInPitch;
	int nIndexImgInRow;
	int nIndexImgIn;
	BYTE* pImgOutPtr;
	int nImgOutPitch;
	int nIndexImgOutRow;
	//int nIndexImgOut;
	
	CRect rcInEdge[4];
	CRect rcOutEdge[4];
	int nError;

try
{
	rcImgInProc = CRect(prcIn->left + nKernelCtrX, 
				   prcIn->top + nKernelCtrY,
				   prcIn->right - (nKernelWidth - nKernelCtrX),
				   prcIn->bottom - (nKernelHeight - nKernelCtrY));
	rcImgOutProc = CRect(prcOut->left + nKernelCtrX, 
				   prcOut->top + nKernelCtrY,
				   prcOut->right - (nKernelWidth - nKernelCtrX),
				   prcOut->bottom - (nKernelHeight - nKernelCtrY));
	pImgInPtr = pImgIn->GetImgPtr();
	nImgInPitch = pImgIn->GetPitch();
	pImgOutPtr = pImgOut->GetImgPtr();
	nImgOutPitch = pImgOut->GetPitch();
	if((rcImgInProc.Width() > 0) && (rcImgInProc.Height() > 0)) {
		nIndexImgInRow = rcImgInProc.top * nImgInPitch + rcImgInProc.left;
		nIndexImgOutRow = rcImgOutProc.top * nImgOutPitch + rcImgOutProc.left;
		if(nThreshold==-1) {
			pImgOut->ClrImg(0);
			for(i=0;i<rcImgInProc.Height();i++) {
				for(j=0;j<rcImgInProc.Width();j++) {
					nAvgValue = 0;
					nIndexImgIn = nIndexImgInRow + j;
					//nIndexImgOut = nIndexImgOutRow + j;
					for(k=0;k<nKernelAddress;k++) {
						nAvgValue += pImgInPtr[nIndexImgIn + pnKernelAddress[k]];
					}
					nAvgValue /= nKernelArea;
					if((pImgOutPtr[nIndexImgOutRow + j] - nAvgValue) < 20)
						pImgOutPtr[nIndexImgOutRow + j] = nAvgValue;
					else
						pImgOutPtr[nIndexImgOutRow + j] = pImgInPtr[nIndexImgIn];
				}
				nIndexImgInRow += nImgInPitch;
				nIndexImgOutRow += nImgOutPitch;
			}
		}
		else {
			pImgOut->ClrImg(0);
			for(i=0;i<rcImgInProc.Height();i++) {
				for(j=0;j<rcImgInProc.Width();j++) {
					nAvgValue = 0;
					nIndexImgIn = nIndexImgInRow + j;
					//nIndexImgOut = nIndexImgOutRow + j;
					for(k=0;k<nKernelAddress;k++) {
						nAvgValue += pImgInPtr[nIndexImgIn + pnKernelAddress[k]];
					}
					nAvgValue /= nKernelArea;
					if((pImgOutPtr[nIndexImgOutRow + j] - nAvgValue) >= 20)
						nAvgValue = pImgInPtr[nIndexImgIn];
					pImgOutPtr[nIndexImgOutRow + j] = (nAvgValue >= nThreshold) ? 255 : 0;
				}
				nIndexImgInRow += nImgInPitch;
				nIndexImgOutRow += nImgOutPitch;
			}
		}
	}
	rcInEdge[0] = CRect(prcIn->left, prcIn->top, prcIn->left + nKernelCtrX, prcIn->bottom);
	rcInEdge[1] = CRect(prcIn->left, prcIn->top, prcIn->right, prcIn->top + nKernelCtrY);
	rcInEdge[2] = CRect(prcIn->right - (nKernelWidth - nKernelCtrX), prcIn->top, prcIn->right, prcIn->bottom);
	rcInEdge[3] = CRect(prcIn->left, prcIn->bottom - (nKernelHeight - nKernelCtrY), prcIn->right, prcIn->bottom);
	rcOutEdge[0] = CRect(prcOut->left, prcOut->top, prcOut->left + nKernelCtrX, prcOut->bottom);
	rcOutEdge[1] = CRect(prcOut->left, prcOut->top, prcOut->right, prcOut->top + nKernelCtrY);
	rcOutEdge[2] = CRect(prcOut->right - (nKernelWidth - nKernelCtrX), prcOut->top, prcOut->right, prcOut->bottom);
	rcOutEdge[3] = CRect(prcOut->left, prcOut->bottom - (nKernelHeight - nKernelCtrY), prcOut->right, prcOut->bottom);
	for(i=0;i<4;i++) {
		nError = ImgCopy(pImgIn,
						 &rcInEdge[i],
						 pImgOut,
						 &rcOutEdge[i]);
		if(nError)
			goto ErrorAbort;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::GrayAveragingProcess -1");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMorphology::GrayErosionProcess(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgMask,
					CImageBuffer* pImgOut,
					CRect* prcOut,
					int nThreshold)
{
	int i, j, k;
	CRect rcImgInProc;
	CRect rcImgOutProc;
	BYTE byteMinVal;
	BYTE* pImgInPtr;
	int nImgInPitch;
	int nIndexImgInRow;
	int nIndexImgIn;
	BYTE* pImgMaskPtr;
	BYTE* pImgOutPtr;
	int nImgOutPitch;
	int nIndexImgOutRow;
//	int nIndexImgOut;

try
{
	rcImgInProc = CRect(prcIn->left + nKernelCtrX, 
				   prcIn->top + nKernelCtrY,
				   prcIn->right - (nKernelWidth - nKernelCtrX),
				   prcIn->bottom - (nKernelHeight - nKernelCtrY));
	rcImgOutProc = CRect(prcOut->left + nKernelCtrX, 
				   prcOut->top + nKernelCtrY,
				   prcOut->right - (nKernelWidth - nKernelCtrX),
				   prcOut->bottom - (nKernelHeight - nKernelCtrY));
	pImgInPtr = pImgIn->GetImgPtr();
	nImgInPitch = pImgIn->GetPitch();
	pImgMaskPtr = pImgMask->GetImgPtr();
	pImgOutPtr = pImgOut->GetImgPtr();
	nImgOutPitch = pImgOut->GetPitch();
	if((rcImgInProc.Width() > 0) && (rcImgInProc.Height() > 0)) {
		nIndexImgInRow = rcImgInProc.top * nImgInPitch + rcImgInProc.left;
		nIndexImgOutRow = rcImgOutProc.top * nImgOutPitch + rcImgOutProc.left;
		if(nThreshold==-1) {
			for(i=0;i<rcImgInProc.Height();i++) {
				for(j=0;j<rcImgInProc.Width();j++) {
					if(pImgMaskPtr[nIndexImgOutRow + j] > 0) {
						byteMinVal = 255;
						nIndexImgIn = nIndexImgInRow + j;
						//nIndexImgOut = nIndexImgOutRow + j;
						for(k=0;k<nKernelAddress;k++) {
							if(//(pImgMaskPtr[nIndexImgOut + pnKernelAddress[k]] > 0) &&
								(pImgInPtr[nIndexImgIn + pnKernelAddress[k]] < byteMinVal))
								byteMinVal = pImgInPtr[nIndexImgIn + pnKernelAddress[k]];
						}
						pImgOutPtr[nIndexImgOutRow + j] = byteMinVal;
					}
				}
				nIndexImgInRow += nImgInPitch;
				nIndexImgOutRow += nImgOutPitch;
			}
		}
		else {
			pImgOut->ClrImg(0);
			for(i=0;i<rcImgInProc.Height();i++) {
				for(j=0;j<rcImgInProc.Width();j++) {
					if(pImgMaskPtr[nIndexImgOutRow + j] > 0) {
						byteMinVal = 255;
						nIndexImgIn = nIndexImgInRow + j;
						//nIndexImgOut = nIndexImgOutRow + j;
						for(k=0;k<nKernelAddress;k++) {
							if(//(pImgMaskPtr[nIndexImgOut + pnKernelAddress[k]] > 0) &&
								(pImgInPtr[nIndexImgIn + pnKernelAddress[k]] < byteMinVal))
								byteMinVal = pImgInPtr[nIndexImgIn + pnKernelAddress[k]];
						}
						if((pImgInPtr[nIndexImgIn] > byteMinVal) && ((pImgInPtr[nIndexImgIn] - byteMinVal) >= nThreshold))
							pImgOutPtr[nIndexImgOutRow + j] = 255;
					}
				}
				nIndexImgInRow += nImgInPitch;
				nIndexImgOutRow += nImgOutPitch;
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::GrayErosionProcess -2");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMorphology::BinaryProcess(CImageBuffer* pImgIn,
					CRect* prcIn,
					int nImgType,
					int nProcType,
					CImageBuffer* pImgOut,
					CRect* prcOut)
{
	int nError;
try
{
	switch (nProcType) {
		case DILATION:
			nError = BinaryDilationProcess(pImgIn,
							prcIn,
							pImgOut,
							prcOut);
			if(nError)
				goto ErrorAbort;
			break;
		case EROSION:
			nError = BinaryErosionProcess(pImgIn,
							prcIn,
							pImgOut,
							prcOut);
			if(nError)
				goto ErrorAbort;
			break;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CMorphology::BinaryProcess");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CMorphology::BinaryDilationProcess(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgOut,
					CRect* prcOut)
{
	return 0;
}

int CMorphology::BinaryErosionProcess(CImageBuffer* pImgIn,
					CRect* prcIn,
					CImageBuffer* pImgOut,
					CRect* prcOut)
{
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

int GrayMaskBinarise(CImageBuffer *pImgSrc,			// Pointer to Source Image
				   CRect *prcSrc,				// ROI of Source Image
				   int nBlobType,
				   CImageBuffer *pImgMsk, 			// Pointer to Mask Image
				   CRect *prcMsk, 				// ROI of Mask Image
				   BYTE nMaskValue,
				   BYTE byteThresVal, 
				   CImageBuffer *pImgDst,  			// Pointer to Destination Image
				   CRect *prcDst) 				// ROI of Destination Image
{
	int x,y;
	int nImgPitchSrc, nImgPitchDst;
	int nImgPitchMsk;
	int nWidthSrc, nHeightSrc;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	BYTE *pRowMsk, *pColMsk;

try
{
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}

	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
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
	if (ValidateRect(prcDst, &(pImgDst->GetImgSize()))) {
		ASSERT(!"prcDst is Invalid");
		return -1;
	}

	if (prcSrc->Size() != prcMsk->Size()) {
		ASSERT(!"prectSrc Size different from prcMsk Size");
		return -1;
	}

	if (prcSrc->Size() != prcDst->Size()) {
		ASSERT(!"prectSrc Size different from prectDst Size");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();
	nImgPitchMsk = pImgMsk->GetPitch();;

	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	
	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowDst = pColDst = pImgDst->GetImgPtr() + nImgPitchDst * prcDst->top + prcDst->left;
	pRowMsk = pColMsk = pImgMsk->GetImgPtr() + nImgPitchMsk * prcMsk->top + prcMsk->left;

 	if(nBlobType == WHITE) {
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, pRowDst += nImgPitchDst, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, pColDst = pRowDst, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, pColDst++, pColMsk++) {
				if(*pColMsk == nMaskValue) {
					if (*pColSrc < byteThresVal)
						*pColDst = 0; 
					else
						*pColDst = 255;
				}
				else
					*pColDst = 0; 
			}
		}
	}
	else {
		for (y = 0; y < nHeightSrc; y++, pRowSrc += nImgPitchSrc, pRowDst += nImgPitchDst, pRowMsk += nImgPitchMsk) {
			for (x = 0, pColSrc = pRowSrc, pColDst = pRowDst, pColMsk = pRowMsk; x < nWidthSrc; x++, pColSrc++, pColDst++, pColMsk++) {
				if(*pColMsk == nMaskValue) {
					if (*pColSrc < byteThresVal)
						*pColDst = 0; 
					else
						*pColDst = 255;
				}
				else
					*pColDst = 255; 
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GrayMaskBinarise");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int GrayMaskAdvHist(CImageBuffer *pImgSrc,			// Pointer to Source Image
					  CRect *prcSrc,			// ROI of the Source Image
					  CImageBuffer *pImgMsk,			// Pointer to Mask Image
					  CRect *prcMsk,			// ROI of the Mask Image
					  BYTE nMaskValue,
					  long *plAry)				// Histogram Data
{
	int x, y;
	int nImgPitchMsk, nImgPitchSrc;
	int nWidth, nHeight;

	BYTE *pRowSrc, *pColSrc;
	BYTE *pRowMsk, *pColMsk;

try
{
	if (!pImgSrc) {
		ASSERT(!"pImgSrc is a bad Pointer");
		return -1;
	}
	if (!pImgSrc->IsAllocated()) {
		ASSERT(!"pImgSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcSrc, &(pImgSrc->GetImgSize()))) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	if (!pImgMsk) {
		ASSERT(!"pImgMsk is a bad Pointer");
		return -1;
	}
	if (!pImgMsk->IsAllocated()) {
		ASSERT(!"pImgMsk is not Allocated");
		return -1;
	}
	if (ValidateRect(prcMsk, &(pImgMsk->GetImgSize()))) {
		ASSERT(!"prcMsk is Invalid");
		return -1;
	}
	if (prcSrc->Size() != prcMsk->Size()) {
		ASSERT(!"prectSrc Size different from prcMsk Size");
		return -1;
	}

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchMsk = pImgMsk->GetPitch();;

	nWidth = prcSrc->Width();
	nHeight = prcSrc->Height();

	pRowSrc = pColSrc = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pRowMsk = pColMsk = pImgMsk->GetImgPtr() + nImgPitchMsk * prcMsk->top + prcMsk->left;

	memset((void *)plAry, 0, 256 * sizeof(long));   //sizeof(long) = 4
	for (y = 0; y < nHeight; y++, pRowSrc += nImgPitchSrc, pRowMsk += nImgPitchMsk) {
		for (x = 0, pColSrc = pRowSrc, pColMsk = pRowMsk; x < nWidth; x++, pColSrc++, pColMsk++) {
			if(*pColMsk==nMaskValue)
				(plAry[*pColSrc])++;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GrayMaskAdvHist");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}
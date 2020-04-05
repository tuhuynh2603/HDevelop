
#include "stdAfx.h"
#include "EdgeMatching.h"
#include "Morphology.h"
#include "AppDef.h"
#include "ExternalFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL bCodeDebug;

/*CSize AbsSize(CSize csSize)
{
	CSize csAbs;
	csAbs = CSize(abs(csSize.cx), abs(csSize.cy));
	return csAbs;
}*/

int GetBlobEdge(CAdvBlobFind* pBlobFinder, 
				CRect* prcCheck, 
				int nBlobIndex, 
				int nSide,
				CArray<CPoint, CPoint>* pAryptEdge)
{
	int i;
	int* pnEdge;
	int nWidth, nHeight;
	int nOffsetX;
	int nOffsetY;
	int nx;
	int ny;

try
{
	pnEdge = NULL;
	pAryptEdge->RemoveAll();
	nWidth = prcCheck->Width();
	nHeight = prcCheck->Height();
	nOffsetX = prcCheck->left;
	nOffsetY = prcCheck->top;
	pnEdge = new int[nWidth + nHeight];
	switch(nSide)
	{
	case 0:
		for(i=0;i<nHeight;i++)
		{
			pnEdge[i] = prcCheck->right;
		}
		for(i=0;i<pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].nBlobContourSize;i++)
		{
			ny = pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].pptBlobContour[i].y - nOffsetY;
			if(pnEdge[ny] > pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].pptBlobContour[i].x)
				pnEdge[ny] = pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].pptBlobContour[i].x;
		}
		for(i=0;i<nHeight;i++)
		{
			if(pnEdge[i] == prcCheck->right)
				continue;
			pAryptEdge->Add(CPoint(pnEdge[i], i + nOffsetY));
		}
		break;
	case 1:
		for(i=0;i<nWidth;i++)
		{
			pnEdge[i] = prcCheck->bottom;
		}
		for(i=0;i<pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].nBlobContourSize;i++)
		{
			nx = pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].pptBlobContour[i].x - nOffsetX;
			if(pnEdge[nx] > pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].pptBlobContour[i].y)
				pnEdge[nx] = pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].pptBlobContour[i].y;
		}
		for(i=0;i<nWidth;i++)
		{
			if(pnEdge[i] == prcCheck->bottom)
				continue;
			pAryptEdge->Add(CPoint(i + nOffsetX, pnEdge[i]));
		}
		break;
	case 2:
		for(i=0;i<nHeight;i++)
		{
			pnEdge[i] = -1;
		}
		for(i=0;i<pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].nBlobContourSize;i++)
		{
			ny = pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].pptBlobContour[i].y - nOffsetY;
			if(pnEdge[ny] < pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].pptBlobContour[i].x)
				pnEdge[ny] = pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].pptBlobContour[i].x;
		}
		for(i=0;i<nHeight;i++)
		{
			if(pnEdge[i] < 0)
				continue;
			pAryptEdge->Add(CPoint(pnEdge[i], i + nOffsetY));
		}
		break;
	case 3:
		for(i=0;i<nWidth;i++)
		{
			pnEdge[i] = -1;
		}
		for(i=0;i<pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].nBlobContourSize;i++)
		{
			nx = pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].pptBlobContour[i].x - nOffsetX;
			if(pnEdge[nx] < pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].pptBlobContour[i].y)
				pnEdge[nx] = pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].pptBlobContour[i].y;
		}
		for(i=0;i<nWidth;i++)
		{
			if(pnEdge[i] < 0)
				continue;
			pAryptEdge->Add(CPoint(i + nOffsetX, pnEdge[i]));
		}
		break;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetBlobEdge");
	goto ErrorAbort;
}	
	if(pnEdge != NULL)
		delete [] pnEdge;
	return 0;
ErrorAbort:
	if(pnEdge != NULL)
		delete [] pnEdge;
	return -1;
}

int BinariseLevelGradImg(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
 			 int nColor,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 CRect* prcGradStep,
			 BYTE nThresVal)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pImgSrcPtr;
	BYTE *pImgDstPtr;
	long lIndexSrc;
	long lIndexSrcRow;
	long lIndexDstRow;
	long lStepOffsetTopY;
	long lStepOffsetBotY;
	long lBlobPixelNo;
	int nOffsetX;
	CRect rcOffset;
	int nError;

try {
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
	if (prcGradStep->left < 0 || 
		prcGradStep->top < 0 ||
		prcGradStep->right < 0 ||
		prcGradStep->bottom < 0) {
		ASSERT(!"prcGradStep Out Of Range");
		return -1;
	}
	if (prcGradStep->left == 0 && 
		prcGradStep->top == 0 &&
		prcGradStep->right == 0 &&
		prcGradStep->bottom == 0) {
		ASSERT(!"prcGradStep Out Of Range");
		return -1;
	}


	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	lStepOffsetTopY = prcGradStep->top * nImgPitchSrc;
	lStepOffsetBotY = prcGradStep->bottom * nImgPitchSrc;
	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	
	pImgSrcPtr = pImgSrc->GetImgPtr();
	pImgDstPtr = pImgDst->GetImgPtr();
	lBlobPixelNo = 0;
	if(nColor == WHITE)
	{
		nError = ImgClear(pImgDst,
						prcDst,
						0);
		if(prcGradStep->left > 0)
		{
			nOffsetX = prcGradStep->left;
			lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left;
			lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left;
			for(y=0;y<nHeightSrc;y++) {
				for(x=nOffsetX;x<nWidthSrc;x++) {
					lIndexSrc = lIndexSrcRow+x;
					if((pImgSrcPtr[lIndexSrc] - pImgSrcPtr[lIndexSrc-nOffsetX]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow+x] = 255;
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexDstRow += nImgPitchDst;
			}
		}
		if(prcGradStep->right > 0)
		{
			nOffsetX = prcGradStep->right;
			lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left;
			lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left;
			for(y=0;y<nHeightSrc;y++) {
				for(x=0;x<(nWidthSrc-nOffsetX);x++) {
					lIndexSrc = lIndexSrcRow+x;
					if((pImgSrcPtr[lIndexSrc] - pImgSrcPtr[lIndexSrc+nOffsetX]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow+x] = 255;
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexDstRow += nImgPitchDst;
			}
		}
		if(prcGradStep->top > 0)
		{
			for(x=0;x<nWidthSrc;x++) {
				lIndexSrcRow = (prcSrc->top + prcGradStep->top) * nImgPitchSrc + prcSrc->left + x;
				lIndexDstRow = (prcDst->top + prcGradStep->top) * nImgPitchDst + prcDst->left + x;
				for(y=prcGradStep->top;y<nHeightSrc;y++) {
					if((pImgSrcPtr[lIndexSrcRow] - pImgSrcPtr[lIndexSrcRow-lStepOffsetTopY]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow] = 255;
					}
					lIndexSrcRow += nImgPitchSrc;
					lIndexDstRow += nImgPitchDst;
				}
			}
		}
		if(prcGradStep->bottom > 0)
		{
			for(x=0;x<nWidthSrc;x++) {
				lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left + x;
				lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left + x;
				for(y=0;y<(nHeightSrc-prcGradStep->bottom);y++) {
					if((pImgSrcPtr[lIndexSrcRow] - pImgSrcPtr[lIndexSrcRow+lStepOffsetBotY]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow] = 255;
					}
					lIndexSrcRow += nImgPitchSrc;
					lIndexDstRow += nImgPitchDst;
				}
			}
		}

	}
	if(nColor == BLACK)
	{
		nError = ImgClear(pImgDst,
						prcDst,
						255);
		if(prcGradStep->left > 0)
		{
			nOffsetX = prcGradStep->left;
			lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left;
			lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left;
			for(y=0;y<nHeightSrc;y++) {
				for(x=nOffsetX;x<nWidthSrc;x++) {
					lIndexSrc = lIndexSrcRow+x;
					if((pImgSrcPtr[lIndexSrc-nOffsetX] - pImgSrcPtr[lIndexSrc]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow+x] = 0;
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexDstRow += nImgPitchDst;
			}
		}
		if(prcGradStep->right > 0)
		{
			nOffsetX = prcGradStep->right;
			lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left;
			lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left;
			for(y=0;y<nHeightSrc;y++) {
				for(x=0;x<(nWidthSrc-nOffsetX);x++) {
					lIndexSrc = lIndexSrcRow+x;
					if((pImgSrcPtr[lIndexSrc+nOffsetX] - pImgSrcPtr[lIndexSrc]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow+x] = 0;
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexDstRow += nImgPitchDst;
			}
		}
		if(prcGradStep->top > 0)
		{
			for(x=0;x<nWidthSrc;x++) {
				lIndexSrcRow = (prcSrc->top + prcGradStep->top) * nImgPitchSrc + prcSrc->left + x;
				lIndexDstRow = (prcDst->top + prcGradStep->top) * nImgPitchDst + prcDst->left + x;
				for(y=prcGradStep->top;y<nHeightSrc;y++) {
					if((pImgSrcPtr[lIndexSrcRow-lStepOffsetTopY] - pImgSrcPtr[lIndexSrcRow]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow] = 0;
					}
					lIndexSrcRow += nImgPitchSrc;
					lIndexDstRow += nImgPitchDst;
				}
			}
		}
		if(prcGradStep->bottom > 0)
		{
			for(x=0;x<nWidthSrc;x++) {
				lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left + x;
				lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left + x;
				for(y=0;y<(nHeightSrc-prcGradStep->bottom);y++) {
					if((pImgSrcPtr[lIndexSrcRow+lStepOffsetBotY] - pImgSrcPtr[lIndexSrcRow]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow] = 0;
					}
					lIndexSrcRow += nImgPitchSrc;
					lIndexDstRow += nImgPitchDst;
				}
			}
		}

	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"BinariseLevelGradImg");
	goto ErrorAbort;
}

	return 0;
ErrorAbort:
	return -1;
}

int BinariseLevelGradImg(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 CRect* prcGradStep,
			 BYTE nThresVal,
 			 int nMaxWhite)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pImgSrcPtr;
	BYTE *pImgDstPtr;
	long lIndexSrc;
	long lIndexSrcRow;
	long lIndexDstRow;
	long lStepOffsetTopY;
	long lStepOffsetBotY;
	long lBlobPixelNo;
	int nOffsetX;
	CRect rcOffset;
	int nError;

try {
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
	if (prcGradStep->left < 0 || 
		prcGradStep->top < 0 ||
		prcGradStep->right < 0 ||
		prcGradStep->bottom < 0) {
		ASSERT(!"prcGradStep Out Of Range");
		return -1;
	}
	if (prcGradStep->left == 0 && 
		prcGradStep->top == 0 &&
		prcGradStep->right == 0 &&
		prcGradStep->bottom == 0) {
		ASSERT(!"prcGradStep Out Of Range");
		return -1;
	}


	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	lStepOffsetTopY = prcGradStep->top * nImgPitchSrc;
	lStepOffsetBotY = prcGradStep->bottom * nImgPitchSrc;
	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	
	pImgSrcPtr = pImgSrc->GetImgPtr();
	pImgDstPtr = pImgDst->GetImgPtr();
	lBlobPixelNo = 0;

	nError = ImgClear(pImgDst,
					prcDst,
					255);
	if(prcGradStep->left > 0)
	{
		nOffsetX = prcGradStep->left;
		lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left;
		lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left;
		for(y=0;y<nHeightSrc;y++) {
			for(x=nOffsetX;x<nWidthSrc;x++) {
				lIndexSrc = lIndexSrcRow+x;
				if(((pImgSrcPtr[lIndexSrc-nOffsetX] - pImgSrcPtr[lIndexSrc]) > nThresVal) && (pImgSrcPtr[lIndexSrc-nOffsetX] < nMaxWhite))
				{
					pImgDstPtr[lIndexDstRow+x] = 0;
				}
			}
			lIndexSrcRow += nImgPitchSrc;
			lIndexDstRow += nImgPitchDst;
		}
	}
	if(prcGradStep->right > 0)
	{
		nOffsetX = prcGradStep->right;
		lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left;
		lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left;
		for(y=0;y<nHeightSrc;y++) {
			for(x=0;x<(nWidthSrc-nOffsetX);x++) {
				lIndexSrc = lIndexSrcRow+x;
				if(((pImgSrcPtr[lIndexSrc+nOffsetX] - pImgSrcPtr[lIndexSrc]) > nThresVal) && (pImgSrcPtr[lIndexSrc+nOffsetX] < nMaxWhite))
				{
					pImgDstPtr[lIndexDstRow+x] = 0;
				}
			}
			lIndexSrcRow += nImgPitchSrc;
			lIndexDstRow += nImgPitchDst;
		}
	}
	if(prcGradStep->top > 0)
	{
		for(x=0;x<nWidthSrc;x++) {
			lIndexSrcRow = (prcSrc->top + prcGradStep->top) * nImgPitchSrc + prcSrc->left + x;
			lIndexDstRow = (prcDst->top + prcGradStep->top) * nImgPitchDst + prcDst->left + x;
			for(y=prcGradStep->top;y<nHeightSrc;y++) {
				if(((pImgSrcPtr[lIndexSrcRow-lStepOffsetTopY] - pImgSrcPtr[lIndexSrcRow]) > nThresVal) && (pImgSrcPtr[lIndexSrcRow-lStepOffsetTopY] < nMaxWhite))
				{
					pImgDstPtr[lIndexDstRow] = 0;
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexDstRow += nImgPitchDst;
			}
		}
	}
	if(prcGradStep->bottom > 0)
	{
		for(x=0;x<nWidthSrc;x++) {
			lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left + x;
			lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left + x;
			for(y=0;y<(nHeightSrc-prcGradStep->bottom);y++) {
				if(((pImgSrcPtr[lIndexSrcRow+lStepOffsetBotY] - pImgSrcPtr[lIndexSrcRow]) > nThresVal) && (pImgSrcPtr[lIndexSrcRow+lStepOffsetBotY] < nMaxWhite))
				{
					pImgDstPtr[lIndexDstRow] = 0;
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexDstRow += nImgPitchDst;
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"BinariseLevelGradImg");
	goto ErrorAbort;
}

	return 0;
ErrorAbort:
	return -1;
}

int BinariseGradLineImg(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
  			 int nColor,
			 CImageBuffer *pImgMask,
			 CRect *prcMask,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 CSize* pcsGradStep,
			 BYTE nThresVal)
{
	int x,y;
	int nImgPitchSrc,nImgPitchMask,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pImgSrcPtr;
	BYTE *pImgMaskPtr;
	BYTE *pImgDstPtr;
	long lIndexSrc;
	long lIndexSrcRow;
	long lIndexDstRow;
	long lIndexMask;
	long lIndexMaskRow;
	long lStepOffsetX;
	long lStepOffsetY;
	long lMaskStepOffsetY;
	CRect rcOffset;

try {
	pImgDst->ClrImg(0);
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
	if (prcSrc->Size()!=prcMask->Size()) {
		ASSERT(!"prcSrc Size different from prcMask Size");
		return -1;
	}

	if (nThresVal<0 && nThresVal>=256) {
		ASSERT(!"nThresVal Out Of Range");
		return -1;
	}
	if (pcsGradStep->cx<0 || pcsGradStep->cy<0) {
		ASSERT(!"pcsGradStep Out Of Range");
		return -1;
	}


	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchMask = pImgMask->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	lStepOffsetX = pcsGradStep->cx;
	lStepOffsetY = pcsGradStep->cy * nImgPitchSrc;
	lMaskStepOffsetY = pcsGradStep->cy * nImgPitchMask;
	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	
	pImgSrcPtr = pImgSrc->GetImgPtr();
	pImgMaskPtr = pImgMask->GetImgPtr();
	pImgDstPtr = pImgDst->GetImgPtr();
	lIndexSrcRow = (prcSrc->top+pcsGradStep->cy) * nImgPitchSrc + prcSrc->left;
	lIndexMaskRow = (prcMask->top+pcsGradStep->cy) * nImgPitchMask + prcMask->left;
	lIndexDstRow = (prcDst->top+pcsGradStep->cy) * nImgPitchDst + prcDst->left;
	BYTE nValue;
	if(nColor == WHITE)
	{
		if(pcsGradStep->cx != 0)
		{
			for(y=pcsGradStep->cy;y<(nHeightSrc-pcsGradStep->cy);y++) {
				for(x=pcsGradStep->cx;x<(nWidthSrc-pcsGradStep->cx);x++) {
					lIndexSrc = lIndexSrcRow+x;
					lIndexMask = lIndexMaskRow+x;

					nValue = pImgSrcPtr[lIndexSrc];
					if((pImgMaskPtr[lIndexMask-lStepOffsetX] > 0) &&
						(pImgMaskPtr[lIndexMask] > 0) &&
						(pImgMaskPtr[lIndexMask+lStepOffsetX] > 0))
					{
						if(((nValue - pImgSrcPtr[lIndexSrc-lStepOffsetX]) > nThresVal) &&
							((nValue - pImgSrcPtr[lIndexSrc+lStepOffsetX]) > nThresVal)) {
								pImgDstPtr[lIndexDstRow+x] = 255;
								continue;
						}
					}
					if((pImgMaskPtr[lIndexMask-lMaskStepOffsetY] > 0) &&
						(pImgMaskPtr[lIndexMask] > 0) &&
						(pImgMaskPtr[lIndexMask+lMaskStepOffsetY] > 0))
					{
						if(((nValue - pImgSrcPtr[lIndexSrc-lStepOffsetY]) > nThresVal) &&
							((nValue - pImgSrcPtr[lIndexSrc+lStepOffsetY]) > nThresVal)) {
								pImgDstPtr[lIndexDstRow+x] = 255;
						}
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexMaskRow += nImgPitchMask;
				lIndexDstRow += nImgPitchDst;
			}
		}
		else
		{
			for(y=pcsGradStep->cy;y<(nHeightSrc-pcsGradStep->cy);y++) {
				for(x=pcsGradStep->cx;x<(nWidthSrc-pcsGradStep->cx);x++) {
					lIndexSrc = lIndexSrcRow+x;
					lIndexMask = lIndexMaskRow+x;

					nValue = pImgSrcPtr[lIndexSrc];
					if((pImgMaskPtr[lIndexMask-lMaskStepOffsetY] > 0) &&
						(pImgMaskPtr[lIndexMask] > 0) &&
						(pImgMaskPtr[lIndexMask+lMaskStepOffsetY] > 0))
					{
						if(((nValue - pImgSrcPtr[lIndexSrc-lStepOffsetY]) > nThresVal) &&
							((nValue - pImgSrcPtr[lIndexSrc+lStepOffsetY]) > nThresVal)) {
								pImgDstPtr[lIndexDstRow+x] = 255;
						}
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexMaskRow += nImgPitchMask;
				lIndexDstRow += nImgPitchDst;
			}
		}
	}
	else
	{
		if(pcsGradStep->cx != 0)
		{
			for(y=pcsGradStep->cy;y<(nHeightSrc-pcsGradStep->cy);y++) {
				for(x=pcsGradStep->cx;x<(nWidthSrc-pcsGradStep->cx);x++) {
					lIndexSrc = lIndexSrcRow+x;
					lIndexMask = lIndexMaskRow+x;
				
					nValue = pImgSrcPtr[lIndexSrc];
					if((pImgMaskPtr[lIndexMask-lStepOffsetX] > 0) &&
						(pImgMaskPtr[lIndexMask] > 0) &&
						(pImgMaskPtr[lIndexMask+lStepOffsetX] > 0))
					{
						if(((pImgSrcPtr[lIndexSrc-lStepOffsetX] - nValue) > nThresVal) &&
							((pImgSrcPtr[lIndexSrc+lStepOffsetX] - nValue) > nThresVal)) {
								pImgDstPtr[lIndexDstRow+x] = 255;
								continue;
						}
					}
					if((pImgMaskPtr[lIndexMask-lMaskStepOffsetY] > 0) &&
						(pImgMaskPtr[lIndexMask] > 0) &&
						(pImgMaskPtr[lIndexMask+lMaskStepOffsetY] > 0))
					{
						if(((pImgSrcPtr[lIndexSrc-lStepOffsetY] - nValue) > nThresVal) &&
							((pImgSrcPtr[lIndexSrc+lStepOffsetY] - nValue) > nThresVal)) {
								pImgDstPtr[lIndexDstRow+x] = 255;
						}
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexMaskRow += nImgPitchMask;
				lIndexDstRow += nImgPitchDst;
			}
		}
		else
		{
			for(y=pcsGradStep->cy;y<(nHeightSrc-pcsGradStep->cy);y++) {
				for(x=pcsGradStep->cx;x<(nWidthSrc-pcsGradStep->cx);x++) {
					lIndexSrc = lIndexSrcRow+x;
					lIndexMask = lIndexMaskRow+x;

					nValue = pImgSrcPtr[lIndexSrc];
					if((pImgMaskPtr[lIndexMask-lMaskStepOffsetY] > 0) &&
						(pImgMaskPtr[lIndexMask] > 0) &&
						(pImgMaskPtr[lIndexMask+lMaskStepOffsetY] > 0))
					{
						if(((pImgSrcPtr[lIndexSrc-lStepOffsetY] - nValue) > nThresVal) &&
							((pImgSrcPtr[lIndexSrc+lStepOffsetY] - nValue) > nThresVal)) {
								pImgDstPtr[lIndexDstRow+x] = 255;
						}
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexMaskRow += nImgPitchMask;
				lIndexDstRow += nImgPitchDst;
			}
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"BinariseGradLineImg");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int BinariseGradLineImg(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
  			 int nColor,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 CRect* prcGradStep,
			 BYTE nThresVal)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pImgSrcPtr;
	BYTE *pImgDstPtr;
	long lIndexSrc;
	long lIndexSrcRow;
	long lIndexDstRow;
	long lStepOffsetLftX, lStepOffsetRhtX;
	long lStepOffsetTopY, lStepOffsetBotY;
	long lBlobPixelNo;
	CRect rcOffset;

try {
	pImgDst->ClrImg(0);
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
	if ((prcGradStep->left<0 ||	prcGradStep->right<0) &&
		(prcGradStep->top<0 || prcGradStep->bottom<0)) {
		ASSERT(!"prcGradStep Out Of Range");
		return -1;
	}


	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	lStepOffsetLftX = prcGradStep->left;
	lStepOffsetRhtX = prcGradStep->right;
	lStepOffsetTopY = prcGradStep->top * nImgPitchSrc;
	lStepOffsetBotY = prcGradStep->bottom * nImgPitchSrc;
	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	
	pImgSrcPtr = pImgSrc->GetImgPtr();
	pImgDstPtr = pImgDst->GetImgPtr();
	lBlobPixelNo = 0;
	BYTE nValue;
	if(nColor == WHITE)
	{
		if((prcGradStep->left != 0) && (prcGradStep->right != 0))
		{
			lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left;
			lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left;
			for(y=0;y<nHeightSrc;y++) {
				for(x=prcGradStep->left;x<(nWidthSrc-prcGradStep->right);x++) {
					lIndexSrc = lIndexSrcRow+x;
					nValue = pImgSrcPtr[lIndexSrc];
					if(((nValue - pImgSrcPtr[lIndexSrc-lStepOffsetLftX]) > nThresVal) &&
						((nValue - pImgSrcPtr[lIndexSrc+lStepOffsetRhtX]) > nThresVal)) {
							pImgDstPtr[lIndexDstRow+x] = 255;
							continue;
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexDstRow += nImgPitchDst;
			}
		}
		if((prcGradStep->top != 0) && (prcGradStep->bottom != 0))
		{
			lIndexSrcRow = (prcSrc->top+prcGradStep->top) * nImgPitchSrc + prcSrc->left;
			lIndexDstRow = (prcDst->top+prcGradStep->top) * nImgPitchDst + prcDst->left;
			for(y=prcGradStep->top;y<(nHeightSrc-prcGradStep->bottom);y++) {
				for(x=0;x<nWidthSrc;x++) {
					lIndexSrc = lIndexSrcRow+x;
					nValue = pImgSrcPtr[lIndexSrc];
					if(((nValue - pImgSrcPtr[lIndexSrc-lStepOffsetTopY]) > nThresVal) &&
						((nValue - pImgSrcPtr[lIndexSrc+lStepOffsetBotY]) > nThresVal)) {
							pImgDstPtr[lIndexDstRow+x] = 255;
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexDstRow += nImgPitchDst;
			}
		}
	}
	else
	{
		if((prcGradStep->left != 0) && (prcGradStep->right != 0))
		{
			lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left;
			lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left;
			for(y=0;y<nHeightSrc;y++) {
				for(x=prcGradStep->left;x<(nWidthSrc-prcGradStep->right);x++) {
					lIndexSrc = lIndexSrcRow+x;
					nValue = pImgSrcPtr[lIndexSrc];
					if(((pImgSrcPtr[lIndexSrc-lStepOffsetLftX] - nValue) > nThresVal) &&
						((pImgSrcPtr[lIndexSrc+lStepOffsetRhtX] - nValue) > nThresVal)) {
							pImgDstPtr[lIndexDstRow+x] = 255;
							continue;
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexDstRow += nImgPitchDst;
			}
		}
		if((prcGradStep->top != 0) && (prcGradStep->bottom != 0))
		{
			lIndexSrcRow = (prcSrc->top + prcGradStep->top) * nImgPitchSrc + prcSrc->left;
			lIndexDstRow = (prcDst->top + prcGradStep->top) * nImgPitchDst + prcDst->left;
			for(y=prcGradStep->top;y<(nHeightSrc-prcGradStep->bottom);y++) {
				for(x=0;x<nWidthSrc;x++) {
					lIndexSrc = lIndexSrcRow+x;
					nValue = pImgSrcPtr[lIndexSrc];
					if(((pImgSrcPtr[lIndexSrc-lStepOffsetTopY] - nValue) > nThresVal) &&
						((pImgSrcPtr[lIndexSrc+lStepOffsetBotY] - nValue) > nThresVal)) {
							pImgDstPtr[lIndexDstRow+x] = 255;
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexDstRow += nImgPitchDst;
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"BinariseGradLineImg");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int ImgSetPixelVal(CImageBuffer* pImg,
			   CRect* prc,
			   CPoint* ppt,
			   int nNo,
			   CPoint* pptOffset,
			   BYTE byteVal)
{
	int i;
	int nIndex;
	int nPitch;
	BYTE* pImgPtr;
	CPoint pt;
try
{
	nPitch = pImg->GetPitch();
	pImgPtr = pImg->GetImgPtr();
	for(i=0;i<nNo;i++)
	{
		pt = ppt[i] - *pptOffset;
		if(ValidatePoint(&pt, prc)) 
		{
			continue;
		}
		nIndex = pt.y * nPitch + pt.x;
		pImgPtr[nIndex] = byteVal;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgSetPixelVal");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int ProjMaxXVal(CImageBuffer *pImgSrc,
			  CRect *prcSrc,	
			  CImageBuffer *pImgMsk,	
			  CRect *prcMsk,	
			  int *pnAry)		
{
	int x, y;
	int nImgPitchMsk, nImgPitchSrc;
	int nWidth, nHeight;
	BOOL bFoundMax;
	int nColMax;
	BYTE *pSrc;
	BYTE *pMsk;
	long lIndexSrc, lIndexMsk;

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
	if(pImgMsk != NULL)
	{
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

		pSrc = pImgSrc->GetImgPtr();
		pMsk = pImgMsk->GetImgPtr();
		for (x = 0; x < nWidth; x++) {
			bFoundMax = FALSE;
			nColMax = 0;
			lIndexSrc = nImgPitchSrc * prcSrc->top + prcSrc->left + x;
			lIndexMsk = nImgPitchMsk * prcMsk->top + prcMsk->left + x;
			for (y = 0; y < nHeight; y++) {
				if(pMsk[lIndexMsk]) {
					bFoundMax = TRUE;
					if(nColMax <pSrc[lIndexSrc])
						nColMax = pSrc[lIndexSrc];
				}
				lIndexSrc += nImgPitchSrc;
				lIndexMsk += nImgPitchMsk;
			}
			if(bFoundMax)
				pnAry[x] = nColMax;
			else
				pnAry[x] = 0;
		}
	}
	else
	{
		nImgPitchSrc = pImgSrc->GetPitch();

		nWidth = prcSrc->Width();
		nHeight = prcSrc->Height();

		pSrc = pImgSrc->GetImgPtr();
		for (x = 0; x < nWidth; x++) {
			bFoundMax = FALSE;
			nColMax = 0;
			lIndexSrc = nImgPitchSrc * prcSrc->top + prcSrc->left + x;
			for (y = 0; y < nHeight; y++) 
			{
				if(nColMax <pSrc[lIndexSrc])
				{
					nColMax = pSrc[lIndexSrc];
				}
				lIndexSrc += nImgPitchSrc;
			}
			pnAry[x] = nColMax;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ProjMaxXVal");
	goto ErrorAbort;
}
    return 0;
ErrorAbort:
    return -1;
}

int BinariseGradImg(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
 			 int nColor,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 CSize* pcsGradStep,
			 BYTE nThresVal)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pImgSrcPtr;
	BYTE *pImgDstPtr;
	long lIndexSrc;
	long lIndexSrcRow;
	long lIndexDstRow;
	long lStepOffsetX;
	long lStepOffsetY;
	long lBlobPixelNo;
	CRect rcOffset;

try {
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
	if (pcsGradStep->cx<0 || pcsGradStep->cy<0) {
		ASSERT(!"pcsGradStep Out Of Range");
		return -1;
	}
	if (pcsGradStep->cx == 0 && pcsGradStep->cy == 0) {
		ASSERT(!"pcsGradStep Out Of Range");
		return -1;
	}


	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	lStepOffsetX = pcsGradStep->cx;
	lStepOffsetY = pcsGradStep->cy * nImgPitchSrc;
	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	
	pImgSrcPtr = pImgSrc->GetImgPtr();
	pImgDstPtr = pImgDst->GetImgPtr();
	lBlobPixelNo = 0;
	if(nColor == WHITE)
	{
		pImgDst->ClrImg(0);
		if(pcsGradStep->cx > 0)
		{
			lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left;
			lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left;
			for(y=0;y<nHeightSrc;y++) {
				for(x=0;x<(nWidthSrc-pcsGradStep->cx);x++) {
					lIndexSrc = lIndexSrcRow+x;
					if((pImgSrcPtr[lIndexSrc] - pImgSrcPtr[lIndexSrc+lStepOffsetX]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow+x] = 255;
					}
				}
				for(x=pcsGradStep->cx;x<nWidthSrc;x++) {
					lIndexSrc = lIndexSrcRow+x;
					if((pImgSrcPtr[lIndexSrc] - pImgSrcPtr[lIndexSrc-lStepOffsetX]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow+x] = 255;
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexDstRow += nImgPitchDst;
			}
		}
		if(pcsGradStep->cy > 0)
		{
			for(x=0;x<nWidthSrc;x++) {
				lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left + x;
				lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left + x;
				for(y=0;y<(nHeightSrc-pcsGradStep->cy);y++) {
					if((pImgSrcPtr[lIndexSrcRow] - pImgSrcPtr[lIndexSrcRow+lStepOffsetY]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow] = 255;
					}
					lIndexSrcRow += nImgPitchSrc;
					lIndexDstRow += nImgPitchDst;
				}
				lIndexSrcRow = (prcSrc->top + pcsGradStep->cy) * nImgPitchSrc + prcSrc->left + x;
				lIndexDstRow = (prcDst->top + pcsGradStep->cy) * nImgPitchDst + prcDst->left + x;
				for(y=0;y<(nHeightSrc-pcsGradStep->cy);y++) {
					if((pImgSrcPtr[lIndexSrcRow] - pImgSrcPtr[lIndexSrcRow-lStepOffsetY]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow] = 255;
					}
					lIndexSrcRow += nImgPitchSrc;
					lIndexDstRow += nImgPitchDst;
				}
			}
		}
	}
	if(nColor == BLACK)
	{
		pImgDst->ClrImg(255);
		if(pcsGradStep->cx > 0)
		{
			lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left;
			lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left;
			for(y=0;y<nHeightSrc;y++) {
				for(x=0;x<(nWidthSrc-pcsGradStep->cx);x++) {
					lIndexSrc = lIndexSrcRow+x;
					if((pImgSrcPtr[lIndexSrc+lStepOffsetX] - pImgSrcPtr[lIndexSrc]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow+x] = 0;
					}
				}
				for(x=pcsGradStep->cx;x<nWidthSrc;x++) {
					lIndexSrc = lIndexSrcRow+x;
					if((pImgSrcPtr[lIndexSrc-lStepOffsetX] - pImgSrcPtr[lIndexSrc]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow+x] = 0;
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexDstRow += nImgPitchDst;
			}
		}
		if(pcsGradStep->cy > 0)
		{
			for(x=0;x<nWidthSrc;x++) {
				lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left + x;
				lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left + x;
				for(y=0;y<(nHeightSrc-pcsGradStep->cy);y++) {
					if((pImgSrcPtr[lIndexSrcRow+lStepOffsetY] - pImgSrcPtr[lIndexSrcRow]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow] = 0;
					}
					lIndexSrcRow += nImgPitchSrc;
					lIndexDstRow += nImgPitchDst;
				}
				lIndexSrcRow = (prcSrc->top + pcsGradStep->cy) * nImgPitchSrc + prcSrc->left + x;
				lIndexDstRow = (prcDst->top + pcsGradStep->cy) * nImgPitchDst + prcDst->left + x;
				for(y=pcsGradStep->cy;y<nHeightSrc;y++) {
					if((pImgSrcPtr[lIndexSrcRow-lStepOffsetY] - pImgSrcPtr[lIndexSrcRow]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow] = 0;
					}
					lIndexSrcRow += nImgPitchSrc;
					lIndexDstRow += nImgPitchDst;
				}
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"BinariseGradImg");
	goto ErrorAbort;
}

	return 0;
ErrorAbort:
	return -1;
}

int BinariseGradImg(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
			 CImageBuffer *pImgMask,
			 CRect *prcMask,
 			 int nColor,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 CSize* pcsGradStep,
			 BYTE nThresVal)
{
	int x,y;
	int nImgPitchSrc,nImgPitchMask, nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pImgSrcPtr;
	BYTE *pImgMaskPtr;
	BYTE *pImgDstPtr;
	long lIndexSrc;
	long lIndexSrcRow;

	long lIndexMaskRow;

	long lIndexDstRow;
	long lStepOffsetX;
	long lStepOffsetY;
	long lStepOffsetMaskY;
	long lBlobPixelNo;
	CRect rcOffset;

try {
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
	if (prcSrc->Size()!=prcMask->Size()) {
		ASSERT(!"prcSrc Size different from prcMask Size");
		return -1;
	}

	if (nThresVal<0 && nThresVal>=256) {
		ASSERT(!"nThresVal Out Of Range");
		return -1;
	}
	if (pcsGradStep->cx<0 || pcsGradStep->cy<0) {
		ASSERT(!"pcsGradStep Out Of Range");
		return -1;
	}
	if (pcsGradStep->cx == 0 && pcsGradStep->cy == 0) {
		ASSERT(!"pcsGradStep Out Of Range");
		return -1;
	}


	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchMask = pImgMask->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	lStepOffsetX = pcsGradStep->cx;
	lStepOffsetY = pcsGradStep->cy * nImgPitchSrc;
	lStepOffsetMaskY = pcsGradStep->cy * nImgPitchMask;
	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	
	pImgSrcPtr = pImgSrc->GetImgPtr();
	pImgMaskPtr = pImgMask->GetImgPtr();
	pImgDstPtr = pImgDst->GetImgPtr();
	lBlobPixelNo = 0;
	if(nColor == WHITE)
	{
		pImgDst->ClrImg(0);
		if(pcsGradStep->cx > 0)
		{
			lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left;
			lIndexMaskRow = prcMask->top * nImgPitchMask + prcMask->left;
			lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left;
			for(y=0;y<nHeightSrc;y++) {
				for(x=0;x<(nWidthSrc-pcsGradStep->cx);x++) {
					if((pImgMaskPtr[lIndexMaskRow+x] == 0) || (pImgMaskPtr[lIndexMaskRow+lStepOffsetX+x] == 0))
						continue;
					lIndexSrc = lIndexSrcRow+x;
					if((pImgSrcPtr[lIndexSrc] - pImgSrcPtr[lIndexSrc+lStepOffsetX]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow+x] = 255;
					}
				}
				for(x=pcsGradStep->cx;x<nWidthSrc;x++) {
					if((pImgMaskPtr[lIndexMaskRow+x] == 0) || (pImgMaskPtr[lIndexMaskRow-lStepOffsetX+x] == 0))
						continue;
					lIndexSrc = lIndexSrcRow+x;
					if((pImgSrcPtr[lIndexSrc] - pImgSrcPtr[lIndexSrc-lStepOffsetX]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow+x] = 255;
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexMaskRow += nImgPitchMask;
				lIndexDstRow += nImgPitchDst;
			}
		}
		if(pcsGradStep->cy > 0)
		{
			for(x=0;x<nWidthSrc;x++) {
				lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left + x;
				lIndexMaskRow = prcMask->top * nImgPitchMask + prcMask->left + x;
				lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left + x;
				for(y=0;y<(nHeightSrc-pcsGradStep->cy);y++) {
					if((pImgMaskPtr[lIndexMaskRow] == 255) &&(pImgMaskPtr[lIndexMaskRow + lStepOffsetMaskY] == 255))
					{
						if((pImgSrcPtr[lIndexSrcRow] - pImgSrcPtr[lIndexSrcRow+lStepOffsetY]) > nThresVal)
						{
							pImgDstPtr[lIndexDstRow] = 255;
						}
					}
					lIndexSrcRow += nImgPitchSrc;
					lIndexMaskRow += nImgPitchMask;
					lIndexDstRow += nImgPitchDst;
				}
				lIndexSrcRow = (prcSrc->top + pcsGradStep->cy) * nImgPitchSrc + prcSrc->left + x;
				lIndexMaskRow = (prcMask->top + pcsGradStep->cy) * nImgPitchMask + prcMask->left + x;
				lIndexDstRow = (prcDst->top + pcsGradStep->cy) * nImgPitchDst + prcDst->left + x;
				for(y=0;y<(nHeightSrc-pcsGradStep->cy);y++) {
					if((pImgMaskPtr[lIndexMaskRow] == 255) && (pImgMaskPtr[lIndexMaskRow-lStepOffsetMaskY] == 255))
					{
						if((pImgSrcPtr[lIndexSrcRow] - pImgSrcPtr[lIndexSrcRow-lStepOffsetY]) > nThresVal)
						{
							pImgDstPtr[lIndexDstRow] = 255;
						}
					}
					lIndexSrcRow += nImgPitchSrc;
					lIndexMaskRow += nImgPitchMask;
					lIndexDstRow += nImgPitchDst;
				}
			}
		}
	}
	if(nColor == BLACK)
	{
		pImgDst->ClrImg(255);
		if(pcsGradStep->cx > 0)
		{
			lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left;
			lIndexMaskRow = prcMask->top * nImgPitchMask + prcMask->left;
			lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left;
			for(y=0;y<nHeightSrc;y++) {
				for(x=0;x<(nWidthSrc-pcsGradStep->cx);x++) {
					if((pImgMaskPtr[lIndexMaskRow+x] == 0) || (pImgMaskPtr[lIndexMaskRow+lStepOffsetX+x] == 0))
						continue;
					lIndexSrc = lIndexSrcRow+x;
					if((pImgSrcPtr[lIndexSrc+lStepOffsetX] - pImgSrcPtr[lIndexSrc]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow+x] = 0;
					}
				}
				for(x=pcsGradStep->cx;x<nWidthSrc;x++) {
					if((pImgMaskPtr[lIndexMaskRow+x] == 0) || (pImgMaskPtr[lIndexMaskRow-lStepOffsetX+x] == 0))
						continue;
					lIndexSrc = lIndexSrcRow+x;
					if((pImgSrcPtr[lIndexSrc-lStepOffsetX] - pImgSrcPtr[lIndexSrc]) > nThresVal)
					{
						pImgDstPtr[lIndexDstRow+x] = 0;
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexMaskRow += nImgPitchMask;
				lIndexDstRow += nImgPitchDst;
			}
		}
		if(pcsGradStep->cy > 0)
		{
			for(x=0;x<nWidthSrc;x++) {
				lIndexSrcRow = prcSrc->top * nImgPitchSrc + prcSrc->left + x;
				lIndexMaskRow = prcMask->top * nImgPitchMask + prcMask->left + x;
				lIndexDstRow = prcDst->top * nImgPitchDst + prcDst->left + x;
				for(y=0;y<(nHeightSrc-pcsGradStep->cy);y++) {
					if((pImgMaskPtr[lIndexMaskRow] == 255) && (pImgMaskPtr[lIndexMaskRow+lStepOffsetMaskY] == 255))
					{
						if((pImgSrcPtr[lIndexSrcRow+lStepOffsetY] - pImgSrcPtr[lIndexSrcRow]) > nThresVal)
						{
							pImgDstPtr[lIndexDstRow] = 0;
						}
					}
					lIndexSrcRow += nImgPitchSrc;
					lIndexMaskRow += nImgPitchMask;
					lIndexDstRow += nImgPitchDst;
				}
				lIndexSrcRow = (prcSrc->top + pcsGradStep->cy) * nImgPitchSrc + prcSrc->left + x;
				lIndexMaskRow = (prcMask->top + pcsGradStep->cy) * nImgPitchMask + prcMask->left + x;
				lIndexDstRow = (prcDst->top + pcsGradStep->cy) * nImgPitchDst + prcDst->left + x;
				for(y=pcsGradStep->cy;y<nHeightSrc;y++) {
					if((pImgMaskPtr[lIndexMaskRow] == 255) && (pImgMaskPtr[lIndexMaskRow-lStepOffsetMaskY] == 255))
					{
						if((pImgSrcPtr[lIndexSrcRow-lStepOffsetY] - pImgSrcPtr[lIndexSrcRow]) > nThresVal)
						{
							pImgDstPtr[lIndexDstRow] = 0;
						}
					}
					lIndexSrcRow += nImgPitchSrc;
					lIndexMaskRow += nImgPitchMask;
					lIndexDstRow += nImgPitchDst;
				}
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"BinariseGradImg");
	goto ErrorAbort;
}

	return 0;
ErrorAbort:
	return -1;
}

int BinariseGradLineImg(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
  			 int nColor,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 CSize* pcsGradStep,
			 BYTE nThresVal)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pImgSrcPtr;
	BYTE *pImgDstPtr;
	long lIndexSrc;
	long lIndexSrcRow;
	long lIndexDstRow;
	long lStepOffsetX;
	long lStepOffsetY;
	long lBlobPixelNo;
	CRect rcOffset;

try {
	pImgDst->ClrImg(0);
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
	if (pcsGradStep->cx<0 || pcsGradStep->cy<0) {
		ASSERT(!"pcsGradStep Out Of Range");
		return -1;
	}


	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();

	lStepOffsetX = pcsGradStep->cx;
	lStepOffsetY = pcsGradStep->cy * nImgPitchSrc;
	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	
	pImgSrcPtr = pImgSrc->GetImgPtr();
	pImgDstPtr = pImgDst->GetImgPtr();
	lIndexSrcRow = (prcSrc->top+pcsGradStep->cy) * nImgPitchSrc + prcSrc->left;
	lIndexDstRow = (prcDst->top+pcsGradStep->cy) * nImgPitchDst + prcDst->left;
	lBlobPixelNo = 0;
	int nValue;
	if(nColor == WHITE)
	{
		if(pcsGradStep->cx != 0)
		{
			for(y=pcsGradStep->cy;y<(nHeightSrc-pcsGradStep->cy);y++) {
				for(x=pcsGradStep->cx;x<(nWidthSrc-pcsGradStep->cx);x++) {
					lIndexSrc = lIndexSrcRow+x;
					nValue = pImgSrcPtr[lIndexSrc];
					if(((nValue - pImgSrcPtr[lIndexSrc-lStepOffsetX]) > nThresVal) &&
						((nValue - pImgSrcPtr[lIndexSrc+lStepOffsetX]) > nThresVal)) {
							pImgDstPtr[lIndexDstRow+x] = 255;
							continue;
					}
					if(((nValue - pImgSrcPtr[lIndexSrc-lStepOffsetY]) > nThresVal) &&
						((nValue - pImgSrcPtr[lIndexSrc+lStepOffsetY]) > nThresVal)) {
							pImgDstPtr[lIndexDstRow+x] = 255;
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexDstRow += nImgPitchDst;
			}
		}
		else
		{
			for(y=pcsGradStep->cy;y<(nHeightSrc-pcsGradStep->cy);y++) {
				for(x=pcsGradStep->cx;x<(nWidthSrc-pcsGradStep->cx);x++) {
					lIndexSrc = lIndexSrcRow+x;
					nValue = pImgSrcPtr[lIndexSrc];
					if(((nValue - pImgSrcPtr[lIndexSrc-lStepOffsetY]) > nThresVal) &&
						((nValue - pImgSrcPtr[lIndexSrc+lStepOffsetY]) > nThresVal)) {
							pImgDstPtr[lIndexDstRow+x] = 255;
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexDstRow += nImgPitchDst;
			}
		}
	}
	else
	{
		if(pcsGradStep->cx != 0)
		{
			for(y=pcsGradStep->cy;y<(nHeightSrc-pcsGradStep->cy);y++) {
				for(x=pcsGradStep->cx;x<(nWidthSrc-pcsGradStep->cx);x++) {
					lIndexSrc = lIndexSrcRow+x;
					nValue = pImgSrcPtr[lIndexSrc];
					if(((pImgSrcPtr[lIndexSrc-lStepOffsetX] - nValue) > nThresVal) &&
						((pImgSrcPtr[lIndexSrc+lStepOffsetX] - nValue) > nThresVal)) {
							pImgDstPtr[lIndexDstRow+x] = 255;
							continue;
					}
					if(((pImgSrcPtr[lIndexSrc-lStepOffsetY] - nValue) > nThresVal) &&
						((pImgSrcPtr[lIndexSrc+lStepOffsetY] - nValue) > nThresVal)) {
							pImgDstPtr[lIndexDstRow+x] = 255;
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexDstRow += nImgPitchDst;
			}
		}
		else
		{
			for(y=pcsGradStep->cy;y<(nHeightSrc-pcsGradStep->cy);y++) {
				for(x=pcsGradStep->cx;x<(nWidthSrc-pcsGradStep->cx);x++) {
					lIndexSrc = lIndexSrcRow+x;
					nValue = pImgSrcPtr[lIndexSrc];
					if(((pImgSrcPtr[lIndexSrc-lStepOffsetY] - nValue) > nThresVal) &&
						((pImgSrcPtr[lIndexSrc+lStepOffsetY] - nValue) > nThresVal)) {
							pImgDstPtr[lIndexDstRow+x] = 255;
					}
				}
				lIndexSrcRow += nImgPitchSrc;
				lIndexDstRow += nImgPitchDst;
			}
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"BinariseGradLineImg");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int ProjAvgXVal(CImageBuffer *pImgSrc,
			  CRect *prcSrc,	
			  CImageBuffer *pImgMsk,	
			  CRect *prcMsk,	
			  int *pnAry)		
{
	int x, y;
	int nImgPitchMsk, nImgPitchSrc;
	int nWidth, nHeight;
	BYTE *pSrc;
	BYTE *pMsk;
	long lIndexSrc, lIndexMsk;
	long lSum;
	int nWeight;

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
	nImgPitchSrc = pImgSrc->GetPitch();
	nWidth = prcSrc->Width();
	nHeight = prcSrc->Height();
	pSrc = pImgSrc->GetImgPtr();
	if (pImgMsk != NULL) {
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
		nImgPitchMsk = pImgMsk->GetPitch();;
		pMsk = pImgMsk->GetImgPtr();
		for (x = 0; x < nWidth; x++) {
			lIndexSrc = nImgPitchSrc * prcSrc->top + prcSrc->left + x;
			lIndexMsk = nImgPitchMsk * prcMsk->top + prcMsk->left + x;
			lSum = 0;
			nWeight = 0;
			for (y = 0; y < nHeight; y++) {
				if(pMsk[lIndexMsk]) {
					lSum += pSrc[lIndexSrc];
					nWeight ++;
				}
				lIndexSrc += nImgPitchSrc;
				lIndexMsk += nImgPitchMsk;
			}
			if(nWeight > 0)
				pnAry[x] = lSum/nWeight;
			else
				pnAry[x] = 0;
		}
	}
	else
	{
		for (x = 0; x < nWidth; x++) {
			lIndexSrc = nImgPitchSrc * prcSrc->top + prcSrc->left + x;
			lSum = 0;
			for (y = 0; y < nHeight; y++) {
				lSum += pSrc[lIndexSrc];
				lIndexSrc += nImgPitchSrc;
			}
			if(nHeight > 0)
				pnAry[x] = lSum/nHeight;
			else
				pnAry[x] = 0;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ProjAvgXVal");
	goto ErrorAbort;
}
    return 0;
ErrorAbort:
    return -1;
}


int ImgGradY(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			int nStepY,
			CImageBuffer *pImgDst,
			CRect *prcDst)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pSrcPtr;
	BYTE *pDstPtr;
	long lIndexSrc;
	int nStepYIndex;
	long lIndexDst;
	int nError;

try
{
	nError = ValidateImageAndRect(pImgSrc, prcSrc);
	if(nError) 
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgDst, prcDst);
	if(nError) 
		goto ErrorAbort;
	if (prcSrc->Size()!=prcDst->Size()) 
		goto ErrorAbort;
	
	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();
	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	pSrcPtr = pImgSrc->GetImgPtr();
	pDstPtr = pImgDst->GetImgPtr();
	nStepYIndex = nImgPitchSrc * nStepY;

	for (x=0; x<nWidthSrc; x++) {
		lIndexSrc = prcSrc->top * nImgPitchSrc + prcSrc->left + x;
		lIndexDst = prcDst->top * nImgPitchDst + prcDst->left + x;
		for(y=0; y<nHeightSrc; y++) {
			if(y<nStepY) {
				pDstPtr[lIndexDst] = 0;
				continue;
			}
			else {
				if(pSrcPtr[lIndexSrc + nStepYIndex] > pSrcPtr[lIndexSrc])
					pDstPtr[lIndexDst] = pSrcPtr[lIndexSrc + nStepYIndex] - pSrcPtr[lIndexSrc];
				else
					pDstPtr[lIndexDst] = 255;
			}
			lIndexSrc += nImgPitchSrc;
			lIndexDst += nImgPitchDst;
		}
	}
 }
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgGradY");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int ImgGradYForLineEdgeType(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			int nBlobColor,
			int nStepY,
			CImageBuffer *pImgDst,
			CRect *prcDst)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pSrcPtr;
	BYTE *pDstPtr;
	long lIndexSrc;
	int nStepYIndex;
	long lIndexDst;
	int nError;

try
{
	nError = ValidateImageAndRect(pImgSrc, prcSrc);
	if(nError) 
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgDst, prcDst);
	if(nError) 
		goto ErrorAbort;
	if (prcSrc->Size()!=prcDst->Size()) 
		goto ErrorAbort;
	pImgDst->ClrImg(0);
	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();
	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	pSrcPtr = pImgSrc->GetImgPtr();
	pDstPtr = pImgDst->GetImgPtr();
	nStepYIndex = nImgPitchSrc * nStepY;

	switch(nBlobColor)
	{
		case WHITE:
			for (x=0; x<nWidthSrc; x++) {
				lIndexSrc = (prcSrc->top + nStepY) * nImgPitchSrc + prcSrc->left + x;
				lIndexDst = (prcDst->top + nStepY) * nImgPitchDst + prcDst->left + x;
				for(y=nStepY; y<(nHeightSrc-nStepY); y++) {
					if((pSrcPtr[lIndexSrc + nStepYIndex] < pSrcPtr[lIndexSrc]) && 
					(pSrcPtr[lIndexSrc - nStepYIndex] < pSrcPtr[lIndexSrc]))
					{
						pDstPtr[lIndexDst] = pSrcPtr[lIndexSrc] - __max(pSrcPtr[lIndexSrc + nStepYIndex], pSrcPtr[lIndexSrc - nStepYIndex]);
					}
					lIndexSrc += nImgPitchSrc;
					lIndexDst += nImgPitchDst;
				}
			}
		break;
		case BLACK:
			for (x=0; x<nWidthSrc; x++) {
				lIndexSrc = (prcSrc->top + nStepY) * nImgPitchSrc + prcSrc->left + x;
				lIndexDst = (prcDst->top + nStepY) * nImgPitchDst + prcDst->left + x;
				for(y=nStepY; y<(nHeightSrc-nStepY); y++) {
					if((pSrcPtr[lIndexSrc + nStepYIndex] > pSrcPtr[lIndexSrc]) && 
					(pSrcPtr[lIndexSrc - nStepYIndex] > pSrcPtr[lIndexSrc]))
					{
						pDstPtr[lIndexDst] = __min(pSrcPtr[lIndexSrc + nStepYIndex], pSrcPtr[lIndexSrc - nStepYIndex]) - pSrcPtr[lIndexSrc];
					}
					lIndexSrc += nImgPitchSrc;
					lIndexDst += nImgPitchDst;
				}
			}
		break;
	}	
 }
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgGradYForLineEdgeType1");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int ImgGradYForLineEdgeType(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			CImageBuffer *pImgMask,
			CRect *prcMask,
			int nStepY,
			CImageBuffer *pImgDst,
			CRect *prcDst)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst,nImgPitchMask;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pSrcPtr;
	BYTE *pDstPtr;
	BYTE *pMaskPtr;
	long lIndexSrc;
	int nStepYIndex;
	int nStepYIndexMask;
	long lIndexDst;
	long lIndexMask;
	int nError;

try
{
	nError = ValidateImageAndRect(pImgSrc, prcSrc);
	if(nError) 
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgDst, prcDst);
	if(nError) 
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgMask, prcMask);
	if(nError) 
		goto ErrorAbort;
	if (prcSrc->Size()!=prcDst->Size()) 
		goto ErrorAbort;
	if (prcSrc->Size()!=prcMask->Size()) 
		goto ErrorAbort;
	pImgDst->ClrImg(0);
	nImgPitchSrc = pImgSrc->GetPitch();
	nImgPitchDst = pImgDst->GetPitch();
	nImgPitchMask = pImgMask->GetPitch();
	nWidthSrc = prcSrc->Width();
	nHeightSrc = prcSrc->Height();
	nWidthDst = prcDst->Width();
	pSrcPtr = pImgSrc->GetImgPtr();
	pDstPtr = pImgDst->GetImgPtr();
	pMaskPtr = pImgMask->GetImgPtr();
	nStepYIndex = nImgPitchSrc * nStepY;
	nStepYIndexMask = nImgPitchMask * nStepY;

	for (x=0; x<nWidthSrc; x++) {
		lIndexSrc = (prcSrc->top + nStepY) * nImgPitchSrc + prcSrc->left + x;
		lIndexDst = (prcDst->top + nStepY) * nImgPitchDst + prcDst->left + x;
		lIndexMask = (prcMask->top + nStepY) * nImgPitchMask + prcMask->left + x;
		for(y=nStepY; y<(nHeightSrc-nStepY); y++) {
			if((pMaskPtr[lIndexMask + nStepYIndexMask] > 0) &&
				(pMaskPtr[lIndexMask - nStepYIndexMask] > 0) &&
				(pMaskPtr[lIndexMask - nStepYIndexMask] > 0))
			{
				if((pSrcPtr[lIndexSrc + nStepYIndex] > pSrcPtr[lIndexSrc]) && 
				(pSrcPtr[lIndexSrc - nStepYIndex] > pSrcPtr[lIndexSrc]))
				{
					pDstPtr[lIndexDst] = __min(pSrcPtr[lIndexSrc + nStepYIndex], pSrcPtr[lIndexSrc - nStepYIndex]) - pSrcPtr[lIndexSrc];
				}
			}
			lIndexSrc += nImgPitchSrc;
			lIndexDst += nImgPitchDst;
			lIndexMask += nImgPitchMask;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgGradYForLineEdgeType2");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CheckLoopTimeOut(CPerformanceTimer* pTimerInspTimeOutExt,
					 int nLoopIndex,
					 int nCheckPeriod,
					 double dInspTimeOut)
{
	if((nLoopIndex % nCheckPeriod) == 0) {
		if(pTimerInspTimeOutExt->GetElapsedMSec() > dInspTimeOut)
			return VISION_TIMEOUT;
	}
	return 0;
}

int ImgSegmentCopy(CImageBuffer* pImgSrc,
				CRect* prcPeriod,
				CRect* prcSegment,
				CImageBuffer* pImgSegment,
				CRect* prcSegmentRelative)
{
	int i;
	int nStartPos;
	int nEndPos;
	CRect rcCopy[2];
	CRect rcCopyRelative[2];
	int nCopyNo;
	int nPeriod;

try
{
	nPeriod = prcPeriod->Width();

	nStartPos = (prcSegment->left + nPeriod) % nPeriod;
	nEndPos = (prcSegment->right + nPeriod) % nPeriod;

	if(nStartPos < nEndPos)
	{
		nCopyNo = 1;
		rcCopy[0] = CRect(nStartPos, prcSegment->top, nEndPos, prcSegment->bottom);
		rcCopyRelative[0] = CRect(CPoint(0,0), rcCopy[0].Size());
	}
	else
	{
		nCopyNo = 2;
		rcCopy[0] = CRect(nStartPos, prcSegment->top, prcPeriod->right, prcSegment->bottom);
		rcCopyRelative[0] = CRect(CPoint(0,0), rcCopy[0].Size());

		rcCopy[1] = CRect(0, prcSegment->top, nEndPos, prcSegment->bottom);
		rcCopyRelative[1] = CRect(CPoint(rcCopy[0].Width(),0), rcCopy[1].Size());
	}
	for(i=0;i<nCopyNo;i++)
	{
		ImgCopy(pImgSrc,
				&rcCopy[i],
				pImgSegment,
				&rcCopyRelative[i]);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgSegmentCopy");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}
int GetHoughTransformLine(CArray<CPoint,CPoint> * pAryPtSrc,
							double dNominalAngleRad,
							double dTolAngleRad,
							double dAngleStep,
							double dOnLineTol,
							double* pdOnLinePercentage,
							CLine2D* pln2DHT)
{
	int i;
	CArray<CPoint2D,CPoint2D> AryPt2DSrc;
	int nError;
	AryPt2DSrc.RemoveAll();
	for(i=0;i<pAryPtSrc->GetSize();i++)
	{
		AryPt2DSrc.Add(pAryPtSrc->GetAt(i));
	}
	nError = GetHoughTransformLine(&AryPt2DSrc,
							dNominalAngleRad,
							dTolAngleRad,
							dAngleStep,
							dOnLineTol,
							pdOnLinePercentage,
							pln2DHT);
	AryPt2DSrc.RemoveAll();
	return nError;
}

int GetHoughTransformLine(CArray<CPoint2D,CPoint2D> * pAryPt2DSrc,
				double dNominalAngleRad,
				double dTolAngleRad,
				double dAngleStep,
				double dOnLineTol,
				int* pnOnLinePercentage,
				CLine2D* pln2DHT)
{
	double dOnLinePercentage;
	int nError;

	nError = GetHoughTransformLine(pAryPt2DSrc,
				dNominalAngleRad,
				dTolAngleRad,
				dAngleStep,
				dOnLineTol,
				&dOnLinePercentage,
				pln2DHT);
	*pnOnLinePercentage = Round(dOnLinePercentage);
	return nError;
}

int GetHoughTransformLine(CArray<CPoint2D,CPoint2D> * pAryPt2DSrc,
							double dNominalAngleRad,
							double dTolAngleRad,
							double dAngleStep,
							double dOnLineTol,
							double* pdOnLinePercentage,
							CLine2D* pln2DHT)
{
	int i;
	CPoint2D* ppt2DEdge;
	int nPt2DEdgeNo;
	CPoint2D *ppt2DOnLine;
	int nOnLineSize;
	CPoint2D *ppt2DNOTOnLine;
	int nNOTOnLineSize;
	CHoughTransform HoughTransform;
	int nError = 0;
try
{
	*pdOnLinePercentage = 0;
	ppt2DEdge = NULL;
	ppt2DOnLine = NULL;
	ppt2DNOTOnLine = NULL;
	nPt2DEdgeNo = (int) pAryPt2DSrc->GetSize();
	*pln2DHT = CLine2D();
	if(nPt2DEdgeNo < 3)
		goto PASS;
	createArray(ppt2DEdge, nPt2DEdgeNo);
	createArray(ppt2DOnLine, nPt2DEdgeNo);
	createArray(ppt2DNOTOnLine, nPt2DEdgeNo);
	//ppt2DEdge = new CPoint2D[nPt2DEdgeNo];
	//ppt2DOnLine = new CPoint2D[nPt2DEdgeNo];
	//ppt2DNOTOnLine = new CPoint2D[nPt2DEdgeNo];
	for(i=0;i<nPt2DEdgeNo;i++)
		ppt2DEdge[i] = pAryPt2DSrc->GetAt(i);
	nError = HoughTransform.HoughTransformLine(&ppt2DEdge[0],
										nPt2DEdgeNo,
										dNominalAngleRad,
										dTolAngleRad,	
										dAngleStep,		
										dOnLineTol,
										&ppt2DOnLine[0],
										nOnLineSize,
										&ppt2DNOTOnLine[0],
										nNOTOnLineSize);
	if(nError) 
		goto ErrorAbort;
	if(nPt2DEdgeNo > 0)
		*pdOnLinePercentage = 100 * nOnLineSize/nPt2DEdgeNo;
	if(nOnLineSize <= 1) {
		*pdOnLinePercentage = 0;
		goto ErrorAbort;
	}

	nError=pln2DHT->GetBestFittingLine(&ppt2DOnLine[0], nOnLineSize);
	if(nError==-1){
		OutputDebugLogTo(9,TRUE,"GetHoughTransformLine");
		goto ErrorAbort;
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetHoughTransformLine");
	goto ErrorAbort;
}
PASS:
	deleteArray(ppt2DEdge);
	deleteArray(ppt2DOnLine);
	deleteArray(ppt2DNOTOnLine);
	//if(ppt2DEdge != NULL)
	//	delete [] ppt2DEdge;
	//if(ppt2DOnLine != NULL)
	//	delete [] ppt2DOnLine;
	//if(ppt2DNOTOnLine != NULL)
	//	delete [] ppt2DNOTOnLine;
	return 0;
ErrorAbort:
	deleteArray(ppt2DEdge);
	deleteArray(ppt2DOnLine);
	deleteArray(ppt2DNOTOnLine);
	//if(ppt2DEdge != NULL)
	//	delete [] ppt2DEdge;
	//if(ppt2DOnLine != NULL)
	//	delete [] ppt2DOnLine;
	//if(ppt2DNOTOnLine != NULL)
	//	delete [] ppt2DNOTOnLine;
	return -1;
}

int ImgFiltering(IMAGEVIEW_INFO *pView,
			 CImageBuffer *pImgBin,
			 CRect* prcUnwrap,
			 int nBlobColor,
			 int nMinArea,
			 int nMinX,
			 int nMinY,
			 double nMinLength,
			 double nMinElongation,
			 CImageBuffer* pImgBinFiltering)
{
	int nPkgDiagonalDist;
	int nMaxArea;
	ADV_BLOB_FIND_PARM AdvBlobParm;
	CAdvBlobFind BlobFinder;
	long lProcMode;
	int nError;

	nPkgDiagonalDist = prcUnwrap->Width() + prcUnwrap->Height();
	nMaxArea = prcUnwrap->Width() * prcUnwrap->Height();
	AdvBlobParm.nBlobType = nBlobColor;
	AdvBlobParm.nMinBlobArea = nMinArea; 
	AdvBlobParm.nMaxBlobArea = nMaxArea;
	AdvBlobParm.nMaxYBlobSize = nPkgDiagonalDist;
	AdvBlobParm.nMinYBlobSize = nMinY;
	AdvBlobParm.nMaxXBlobSize = nPkgDiagonalDist;
	AdvBlobParm.nMinXBlobSize = nMinX;
	AdvBlobParm.dMaxBlobLength = nPkgDiagonalDist;
	AdvBlobParm.dMinBlobLength = nMinLength;
	AdvBlobParm.dMaxBlobElongation = nPkgDiagonalDist;
	AdvBlobParm.dMinBlobElongation = nMinElongation;
	AdvBlobParm.rcROISrc = *prcUnwrap;
	AdvBlobParm.rcROIDst = *prcUnwrap;
	lProcMode = FIND_BLOB_RECT | FIND_BLOB_AREA | FIND_BLOB_LENGTH | FIND_BLOB_ELONGATION;
	nError = BlobFinder.GetAdvBlobFiltering(pView,
					pImgBin,
					pImgBinFiltering,
					&AdvBlobParm,
					lProcMode,
					0);
	return 0;
}

int ImgFiltering(IMAGEVIEW_INFO *pView,
			 CImageBuffer *pImgBin,
			 CRect* prcUnwrap,
			 int nBlobColor,
			 int nMinArea,
			 int nMinX,
			 int nMinY,
			 CImageBuffer* pImgBinFiltering)
{
	int nPkgDiagonalDist;
	int nMaxArea;
	ADV_BLOB_FIND_PARM AdvBlobParm;
	CAdvBlobFind BlobFinder;
	long lProcMode;
	int nError;

	nPkgDiagonalDist = prcUnwrap->Width() + prcUnwrap->Height();
	nMaxArea = prcUnwrap->Width() * prcUnwrap->Height();
	AdvBlobParm.nBlobType = nBlobColor;
	AdvBlobParm.nMinBlobArea = nMinArea; 
	AdvBlobParm.nMaxBlobArea = nMaxArea;
	AdvBlobParm.nMaxYBlobSize = nPkgDiagonalDist;
	AdvBlobParm.nMinYBlobSize = nMinY;
	AdvBlobParm.nMaxXBlobSize = nPkgDiagonalDist;
	AdvBlobParm.nMinXBlobSize = nMinX;
	AdvBlobParm.rcROISrc = *prcUnwrap;
	AdvBlobParm.rcROIDst = *prcUnwrap;
	lProcMode = 0;

	nError = BlobFinder.GetAdvBlobFiltering(pView,
					pImgBin,
					pImgBinFiltering,
					&AdvBlobParm,
					lProcMode,
					0);
	return 0;
}


int ImgFiltering(IMAGEVIEW_INFO *pView,
			 CImageBuffer *pImgBin,
			 CRect* prcUnwrap,
			 int nBlobColor,
			 CSize* pcsArea,
			 CSize* pcsX,
			 CSize* pcsY,
			 CImageBuffer* pImgBinFiltering)
{
	int nPkgDiagonalDist;
	int nMaxArea;
	ADV_BLOB_FIND_PARM AdvBlobParm;
	CAdvBlobFind BlobFinder;
	long lProcMode;
	int nError;

	nPkgDiagonalDist = prcUnwrap->Width() + prcUnwrap->Height();
	nMaxArea = prcUnwrap->Width() * prcUnwrap->Height();
	AdvBlobParm.nBlobType = nBlobColor;
	AdvBlobParm.nMinBlobArea = pcsArea->cx; 
	AdvBlobParm.nMaxBlobArea = pcsArea->cy;
	AdvBlobParm.nMaxYBlobSize = pcsY->cy;
	AdvBlobParm.nMinYBlobSize = pcsY->cx;
	AdvBlobParm.nMaxXBlobSize = pcsX->cy;
	AdvBlobParm.nMinXBlobSize = pcsX->cx;
	AdvBlobParm.rcROISrc = *prcUnwrap;
	AdvBlobParm.rcROIDst = *prcUnwrap;
	lProcMode = 0;

	nError = BlobFinder.GetAdvBlobFiltering(pView,
					pImgBin,
					pImgBinFiltering,
					&AdvBlobParm,
					lProcMode,
					0);
	return 0;
}

int BOOLAryDilation1D(BOOL* pbVal,
					int nValNo,
					BOOL bBlobType,
					BOOL* pbDilationVal,
					int nDilationValNo,
					int nDilationSize)
{
	int i,j;
	int nIndexStart, nIndexEnd;
try
{
	if(nValNo != nDilationValNo)
		goto ErrorAbort;
	for(i = 0; i < nValNo; i++) pbDilationVal[i] = pbVal[i];
	if(bBlobType == TRUE) {
		for(i = 1; i < nValNo; i++)	{
			// find transition position
			if(pbVal[i-1] != pbVal[i]) {
				// do dilation forward
				if(pbVal[i] == FALSE) {
					nIndexStart = i;
					nIndexEnd = i + nDilationSize;
					if(nIndexEnd > nValNo) nIndexEnd = nValNo;
					for(j = nIndexStart; j < nIndexEnd; j++) pbDilationVal[j] = TRUE;
				}
				else {
					nIndexEnd = i;
					nIndexStart = i - nDilationSize;
					if(nIndexStart < 0) nIndexStart = 0;
					for(j = nIndexStart; j < nIndexEnd; j++) pbDilationVal[j] = TRUE;
				}
			}
		}
	}
	else {
		for(i = 1; i < nValNo; i++)	{
			// find transition position
			if(pbVal[i-1] != pbVal[i]) {
				// do dilation forward
				if(pbVal[i] == 0) {
					nIndexStart = i - nDilationSize;
					nIndexEnd = i;
					if(nIndexStart < 0) nIndexStart = 0;
					for(j = nIndexStart; j < nIndexEnd; j++) pbDilationVal[j] = FALSE;
				}
				else {
					nIndexEnd = i + nDilationSize;
					nIndexStart = i;
					if(nIndexEnd > nValNo) nIndexEnd = nValNo;
					for(j = nIndexStart; j < nIndexEnd; j++) pbDilationVal[j] = FALSE;
				}
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"BOOLAryDilation1D");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

CSlidingAverage::CSlidingAverage()
{
}

CSlidingAverage::~CSlidingAverage()
{
}

void CSlidingAverage::Clean()
{
}

int CSlidingAverage::ImgSlidingAverageForEdge(CImageBuffer *pImgSrc,
				CRect* prcSrc,
				CImageBuffer* pImgMask,
				CRect* prcMask,
				CSize* pcsProj,
				CImageBuffer* pImgDst,
				CRect* prcDst)
{
	int i;
	int nSide[4];
	int nError;
	nSide[0] = LEFT;
	nSide[1] = TOP;
	nSide[2] = RIGHT;
	nSide[3] = BOTTOM;
	for(i=0;i<4;i++) {
		nError = ImgSlidingAverageForEdge(pImgSrc,
					prcSrc,
					pImgMask,
					prcMask,
					pcsProj,
					nSide[i],
					pImgDst,
					prcDst);
	}
	return 0;
}

int CSlidingAverage::ImgSlidingAverageForEdge(CImageBuffer *pImgSrc,
				CRect* prcSrc,
				CImageBuffer* pImgMask,
				CRect* prcMask,
				CSize* pcsProj,
				int nSide,
				CImageBuffer* pImgDst,
				CRect* prcDst)
{
	int i, j;
	int nStepX0;
	int nStepX1;
	int nStepY0;
	int nStepY1;
	BYTE* pImgSrcPtr;
	int nImgSrcPitch;
	long lIndexSrc;
	BYTE* pImgDstPtr;
	int nImgDstPitch;
	long lIndexDst;
	CImgIntBuf ImgIntProj;
	CImgIntBuf ImgIntWeight;
	CRect rcEdgeRelative;
	CRect rcSrcEdge;
	CRect rcSrcProj;
	long* plProj;
	long* plWeight;
	int nWidth;
	int nHeight;
	int* pIntProjPtr;
	int nImgIntPitch;
	int* pIntWeightPtr;
	long lIntProjIndex;
	long lIntProjIndexAdd;
	long lIntProjIndexSub;
	int nWeight0;
	int nWeight;
	int nSum;
	CRect rcMaskProj;
	BYTE* pImgMaskPtr;
	int nImgMaskPitch;
	long lIxdexMask;
	int nSumWeight;
	int nError;

	plProj = NULL;
	plWeight = NULL;
	nStepX0 = pcsProj->cx/2;
	nStepX1 = pcsProj->cx - nStepX0;
	nStepY0 = pcsProj->cy/2;
	nStepY1 = pcsProj->cy - nStepY0;
	pImgSrcPtr = pImgSrc->GetImgPtr();
	nImgSrcPitch = pImgSrc->GetPitch();
	pImgDstPtr = pImgDst->GetImgPtr();
	nImgDstPitch = pImgDst->GetPitch();
	nWidth = prcSrc->Width();
	nHeight = prcSrc->Height();

	pImgMaskPtr = pImgMask->GetImgPtr();
	nImgMaskPitch = pImgMask->GetPitch();

	switch(nSide) {
	case LEFT:
		plProj = new long[nHeight];
		plWeight = new long[nHeight];
		rcSrcProj = CRect(prcSrc->left,
						  prcSrc->top,
						  prcSrc->left + nStepX1,
						  prcSrc->bottom);
		rcMaskProj = rcSrcProj + (prcMask->TopLeft() - prcSrc->TopLeft());
		rcEdgeRelative = CRect(CPoint(0,0), CSize(nStepX0, nHeight));
		ImgIntProj.Allocate(rcEdgeRelative.Size());
		ImgIntProj.ClrImg(0);
		ImgIntWeight.Allocate(rcEdgeRelative.Size());
		ImgIntWeight.ClrImg(0);
		pIntProjPtr = ImgIntProj.GetImgPtr();
		nImgIntPitch = ImgIntProj.GetPitch(); 
		pIntWeightPtr = ImgIntWeight.GetImgPtr();
		nError = GetYGrayAcclWithMask(pImgSrc, 
						&rcSrcProj,
						pImgMask,
						&rcMaskProj,
						plProj,
						plWeight);

		lIndexSrc = rcSrcProj.top * nImgSrcPitch + rcSrcProj.left + nStepX1 - 1;
		lIntProjIndex = 0;
		lIxdexMask = rcMaskProj.top * nImgMaskPitch + rcMaskProj.left + nStepX1 - 1;
		for(i=0;i<nHeight;i++) {
			pIntProjPtr[lIntProjIndex] = plProj[i];
			pIntWeightPtr[lIntProjIndex] = plWeight[i];
			for(j=1;j<nStepX0;j++) {
				if(pImgMaskPtr[lIxdexMask + j]) {
					pIntProjPtr[lIntProjIndex+j] = pIntProjPtr[lIntProjIndex+j-1] + pImgSrcPtr[lIndexSrc + j];
					pIntWeightPtr[lIntProjIndex+j] = pIntWeightPtr[lIntProjIndex+j-1] + 1;
				}
				else {
					pIntProjPtr[lIntProjIndex+j] = pIntProjPtr[lIntProjIndex+j-1];
					pIntWeightPtr[lIntProjIndex+j] = pIntWeightPtr[lIntProjIndex+j-1];
				}
			}
			lIntProjIndex += nImgIntPitch;
			lIndexSrc += nImgSrcPitch;
			lIxdexMask += nImgMaskPitch;
		}
		for(i=0;i<nStepX0;i++) {
			nWeight0 = nStepX1+i;
			nSum = 0;
			nWeight = 0;
			lIntProjIndex = i;
			nSumWeight = 0; 
			for(j=0;j<nStepY1;j++) {
				nSum += pIntProjPtr[lIntProjIndex];
				lIntProjIndex += nImgIntPitch;
				nSumWeight += pIntWeightPtr[lIntProjIndex];
			}
			lIndexDst = i;
			lIxdexMask = rcMaskProj.top * nImgMaskPitch + rcMaskProj.left + i;
			if(nSumWeight > 0) {
				if(pImgMaskPtr[lIxdexMask]) {
					pImgDstPtr[lIndexDst] = nSum/nSumWeight;
				}
			}
			lIndexDst += nImgDstPitch;
			lIntProjIndexAdd = nImgIntPitch * nStepY1 + i;
			lIxdexMask += nImgMaskPitch;
			for(j=1;j<=nStepY0;j++) {
				nSum += pIntProjPtr[lIntProjIndexAdd];
				nSumWeight += pIntWeightPtr[lIntProjIndexAdd];
				if(nSumWeight > 0) {
					if(pImgMaskPtr[lIxdexMask]) {
						pImgDstPtr[lIndexDst] = nSum/nSumWeight;
					}
				}
				lIxdexMask += nImgMaskPitch;
				lIntProjIndexAdd += nImgIntPitch;
				lIndexDst += nImgDstPitch;
			}
			lIntProjIndexSub = i;
			lIntProjIndexAdd = nImgIntPitch * pcsProj->cy + i;
			for(j=nStepY0+1;j<(nHeight - nStepY1);j++) {
				nSum += (pIntProjPtr[lIntProjIndexAdd] - pIntProjPtr[lIntProjIndexSub]);

				nSumWeight += (pIntWeightPtr[lIntProjIndexAdd] - pIntWeightPtr[lIntProjIndexSub]);
				if(nSumWeight > 0) {
					if(pImgMaskPtr[lIxdexMask]) 
					{
						pImgDstPtr[lIndexDst] = nSum/nSumWeight;
					}
				}
				lIxdexMask += nImgMaskPitch;
				lIntProjIndexSub += nImgIntPitch;
				lIntProjIndexAdd += nImgIntPitch;
				lIndexDst += nImgDstPitch;
			}

			for(j=nHeight - nStepY1;j<nHeight;j++) {
				nSum -= pIntProjPtr[lIntProjIndexSub];
				nSumWeight -= pIntWeightPtr[lIntProjIndexSub];
				if(nSumWeight > 0) {
					if(pImgMaskPtr[lIxdexMask]) 
						pImgDstPtr[lIndexDst] = nSum/nSumWeight;
				}
				lIxdexMask += nImgMaskPitch;
				lIntProjIndexSub += nImgIntPitch;
				lIndexDst += nImgDstPitch;
			}
		}
		break;
	case TOP:
		plProj = new long[nWidth];
		plWeight = new long[nWidth];
		rcSrcProj = CRect(prcSrc->left,
						  prcSrc->top,
						  prcSrc->right,
						  prcSrc->top + nStepY1);

		rcMaskProj = rcSrcProj + (prcMask->TopLeft() - prcSrc->TopLeft());
		rcEdgeRelative = CRect(CPoint(0,0), CSize(nWidth, nStepY0));
		ImgIntProj.Allocate(rcEdgeRelative.Size());
		ImgIntProj.ClrImg(0);
		ImgIntWeight.Allocate(rcEdgeRelative.Size());
		ImgIntWeight.ClrImg(0);
		pIntProjPtr = ImgIntProj.GetImgPtr();
		nImgIntPitch = ImgIntProj.GetPitch(); 
		pIntWeightPtr = ImgIntWeight.GetImgPtr();
		nError = GetXGrayAcclWithMask(pImgSrc, 
						&rcSrcProj,
						pImgMask,
						&rcMaskProj,
						plProj,
						plWeight);

		for(i=0;i<nWidth;i++) {
			pIntProjPtr[i] = plProj[i];
			pIntWeightPtr[i] = plWeight[i];

			lIntProjIndex = nImgIntPitch + i;
			lIndexSrc = (rcSrcProj.top + nStepY1) * nImgSrcPitch + rcSrcProj.left + i;
			lIxdexMask = (rcMaskProj.top + nStepY1) * nImgMaskPitch + rcMaskProj.left + i;
			for(j=1;j<nStepY0;j++) {
				if(pImgMaskPtr[lIxdexMask]) {
					pIntProjPtr[lIntProjIndex] = pIntProjPtr[lIntProjIndex-nImgIntPitch] + pImgSrcPtr[lIndexSrc];
					pIntWeightPtr[lIntProjIndex] = pIntWeightPtr[lIntProjIndex-nImgMaskPitch] + 1;
				}
				else {
					pIntProjPtr[lIntProjIndex] = pIntProjPtr[lIntProjIndex-nImgIntPitch];
					pIntWeightPtr[lIntProjIndex] = pIntWeightPtr[lIntProjIndex-nImgMaskPitch];
				}
				lIntProjIndex += nImgIntPitch;
				lIndexSrc += nImgSrcPitch;
				lIxdexMask += nImgMaskPitch;
			}
		}

		for(i=0;i<nStepY0;i++) {
			nWeight0 = nStepY1+i;
			nSum = 0;
			nSumWeight = 0;
			lIntProjIndex = i * nImgIntPitch;
			for(j=0;j<nStepX1;j++) {
				nSum += pIntProjPtr[lIntProjIndex + j];
				nSumWeight += pIntWeightPtr[lIntProjIndex + j];
			}
			lIndexDst = i * nImgDstPitch;
			lIxdexMask = (rcMaskProj.top + i) * nImgMaskPitch + rcMaskProj.left;
			if(nSumWeight > 0) {
				if(pImgMaskPtr[lIxdexMask]) {
					pImgDstPtr[lIndexDst] = nSum/nSumWeight;
				}
			}
			lIntProjIndexAdd = lIntProjIndex + nStepX1;
			for(j=1;j<=nStepX0;j++) {
				nSum += pIntProjPtr[lIntProjIndexAdd + j];
				nSumWeight += pIntWeightPtr[lIntProjIndexAdd + j];
				if(nSumWeight > 0) {
					if(pImgMaskPtr[lIxdexMask+j]) {
						pImgDstPtr[lIndexDst + j] = nSum/nSumWeight;
					}
				}
			}

			lIntProjIndexSub = i * nImgIntPitch;
			lIntProjIndexAdd = lIntProjIndexSub + pcsProj->cx;
			
			for(j=0;j<=(nWidth - pcsProj->cx - 1);j++) {
				nSum += (pIntProjPtr[lIntProjIndexAdd+j] - pIntProjPtr[lIntProjIndexSub+j]);
				nSumWeight += (pIntWeightPtr[lIntProjIndexAdd+j] - pIntWeightPtr[lIntProjIndexSub+j]);
				if(pImgMaskPtr[lIxdexMask+j+nStepX0+1]) {
					pImgDstPtr[lIndexDst+nStepX0+j + 1] = nSum/nSumWeight;
				}
			}

			for(j=nWidth - pcsProj->cx;j<(nWidth - nStepX0 - 1);j++) {
				nSum -= pIntProjPtr[lIntProjIndexSub + j];
				nSumWeight -= pIntWeightPtr[lIntProjIndexSub+j];
				if(nSumWeight > 0) {
					if(pImgMaskPtr[lIxdexMask+j+nStepX0+1]) {
						pImgDstPtr[lIndexDst+nStepX0+j + 1] = nSum/nSumWeight;
					}
				}
			}

		}
		break;
	case RIGHT:
		int m;
		plProj = new long[nHeight];
		plWeight = new long[nHeight];
		rcSrcProj = CRect(prcSrc->right - nStepX0,
						  prcSrc->top,
						  prcSrc->right,
						  prcSrc->bottom);
		rcMaskProj = rcSrcProj + (prcMask->TopLeft() - prcSrc->TopLeft());
		rcEdgeRelative = CRect(CPoint(0,0), CSize(nStepX1, nHeight));
		ImgIntProj.Allocate(rcEdgeRelative.Size());
		ImgIntProj.Allocate(rcEdgeRelative.Size());
		ImgIntProj.ClrImg(0);
		ImgIntWeight.Allocate(rcEdgeRelative.Size());
		ImgIntWeight.ClrImg(0);
		pIntProjPtr = ImgIntProj.GetImgPtr();
		nImgIntPitch = ImgIntProj.GetPitch(); 
		pIntWeightPtr = ImgIntWeight.GetImgPtr();
		nError = GetYGrayAcclWithMask(pImgSrc, 
						&rcSrcProj,
						pImgMask,
						&rcMaskProj,
						plProj,
						plWeight);
		lIndexSrc = rcSrcProj.top * nImgSrcPitch + rcSrcProj.right - nStepX0;
		lIntProjIndex = 0;
		lIxdexMask = rcMaskProj.top * nImgMaskPitch + rcMaskProj.right - nStepX0;
		for(i=0;i<nHeight;i++) {
			pIntProjPtr[lIntProjIndex + nStepX0 - 1] = plProj[i];
			pIntWeightPtr[lIntProjIndex + nStepX0 - 1] = plWeight[i];
			m = 1;
			for(j=nStepX0 - 2;j>=0;j--) {
				if(pImgMaskPtr[lIxdexMask - m]) {
					pIntProjPtr[lIntProjIndex+j] = pIntProjPtr[lIntProjIndex+j+1] + pImgSrcPtr[lIndexSrc - m];
					pIntWeightPtr[lIntProjIndex+j] = pIntWeightPtr[lIntProjIndex+j+1] + 1;
				}
				else {
					pIntProjPtr[lIntProjIndex+j] = pIntProjPtr[lIntProjIndex+j+1];
					pIntWeightPtr[lIntProjIndex+j] = pIntWeightPtr[lIntProjIndex+j+1];
				}
				m++;
			}
			lIntProjIndex += nImgIntPitch;
			lIndexSrc += nImgSrcPitch;
			lIxdexMask += nImgMaskPitch;
		}
		for(i=0;i<nStepX1;i++) {
			nWeight0 = nStepX0+nStepX1-i-1;
			nSum = 0;
			nSumWeight = 0;
			lIntProjIndex = i;
			for(j=0;j<nStepY1;j++) {
				nSum += pIntProjPtr[lIntProjIndex];
				nSumWeight += pIntWeightPtr[lIntProjIndex];
				lIntProjIndex += nImgIntPitch;
			}
			lIndexDst = nWidth - nStepX1 + i;
			lIxdexMask = rcMaskProj.top * nImgMaskPitch + rcMaskProj.right - nStepX1 + i;
			if(nSumWeight > 0) {
				if(pImgMaskPtr[lIxdexMask]) {
					pImgDstPtr[lIndexDst] = nSum/nSumWeight;
				}
			}
			lIndexDst += nImgDstPitch;
			lIntProjIndexAdd = nImgIntPitch * nStepY1 + i;
			lIxdexMask += nImgMaskPitch;
			for(j=1;j<=nStepY0;j++) {
				nSum += pIntProjPtr[lIntProjIndexAdd];
				nSumWeight += pIntWeightPtr[lIntProjIndexAdd];
				if(nSumWeight > 0) {
					if(pImgMaskPtr[lIxdexMask]) {
						pImgDstPtr[lIndexDst] = nSum/nSumWeight;
					}
				}
				lIntProjIndexAdd += nImgIntPitch;
				lIndexDst += nImgDstPitch;
				lIxdexMask += nImgMaskPitch;
			}
			lIntProjIndexSub = i;
			lIntProjIndexAdd = nImgIntPitch * pcsProj->cy + i;
			for(j=nStepY0+1;j<(nHeight - nStepY1);j++) {
				nSum += (pIntProjPtr[lIntProjIndexAdd] - pIntProjPtr[lIntProjIndexSub]);
				nSumWeight += (pIntWeightPtr[lIntProjIndexAdd] - pIntWeightPtr[lIntProjIndexSub]);
				if(nSumWeight > 0) {
					if(pImgMaskPtr[lIxdexMask]) {
						pImgDstPtr[lIndexDst] = nSum/nSumWeight;
					}
				}

				lIntProjIndexSub += nImgIntPitch;
				lIntProjIndexAdd += nImgIntPitch;
				lIndexDst += nImgDstPitch;
				lIxdexMask += nImgMaskPitch;
			}

			for(j=nHeight - nStepY1;j<nHeight;j++) {
				nSum -= pIntProjPtr[lIntProjIndexSub];
				nSumWeight -= pIntWeightPtr[lIntProjIndexSub];
				if(nSumWeight > 0) {
					if(pImgMaskPtr[lIxdexMask]) {
						pImgDstPtr[lIndexDst] = nSum/nSumWeight;
					}
				}
				lIntProjIndexSub += nImgIntPitch;
				lIndexDst += nImgDstPitch;
				lIxdexMask += nImgMaskPitch;
			}

		}
		break;
	case BOTTOM:
		plProj = new long[nWidth];
		plWeight = new long[nWidth];
		rcSrcProj = CRect(prcSrc->left,
						  prcSrc->bottom - nStepY0,
						  prcSrc->right,
						  prcSrc->bottom);

		rcMaskProj = rcSrcProj + (prcMask->TopLeft() - prcSrc->TopLeft());
		rcEdgeRelative = CRect(CPoint(0,0), CSize(nWidth, nStepY1));
		ImgIntProj.Allocate(rcEdgeRelative.Size());
		ImgIntProj.ClrImg(0);
		ImgIntWeight.Allocate(rcEdgeRelative.Size());
		ImgIntWeight.ClrImg(0);
		pIntProjPtr = ImgIntProj.GetImgPtr();
		nImgIntPitch = ImgIntProj.GetPitch(); 
		pIntWeightPtr = ImgIntWeight.GetImgPtr();
		nError = GetXGrayAcclWithMask(pImgSrc, 
						&rcSrcProj,
						pImgMask,
						&rcMaskProj,
						plProj,
						plWeight);

		for(i=0;i<nWidth;i++) {
			lIntProjIndex = (nStepY1-1) * nImgIntPitch + i;
			pIntProjPtr[lIntProjIndex] = plProj[i];
			pIntWeightPtr[lIntProjIndex] = plWeight[i];
			lIndexSrc = (rcSrcProj.bottom - nStepY1 - 1) * nImgSrcPitch + rcSrcProj.left + i;
			lIntProjIndex -= nImgIntPitch;;
			lIxdexMask = (rcMaskProj.bottom - nStepY1 - 1) * nImgMaskPitch + rcMaskProj.left + i;
			for(j=1;j<nStepY1;j++) {
				if(pImgMaskPtr[lIxdexMask]) {
					pIntProjPtr[lIntProjIndex] = pIntProjPtr[lIntProjIndex+nImgIntPitch] + pImgSrcPtr[lIndexSrc];
					pIntWeightPtr[lIntProjIndex] = pIntWeightPtr[lIntProjIndex+nImgMaskPitch] + 1;
				}
				else {
					pIntProjPtr[lIntProjIndex] = pIntProjPtr[lIntProjIndex+nImgIntPitch];
					pIntWeightPtr[lIntProjIndex] = pIntWeightPtr[lIntProjIndex+nImgMaskPitch];
				}
				lIntProjIndex -= nImgIntPitch;
				lIndexSrc -= nImgSrcPitch;
				lIxdexMask -= nImgMaskPitch;
			}
		}

		for(i=0;i<nStepY1;i++) {
			nWeight0 = nStepY0+i;
			nSum = 0;
			nSumWeight = 0;
			lIntProjIndex = (nStepY1 - 1 - i) * nImgIntPitch;
			for(j=0;j<nStepX1;j++) {
				nSum += pIntProjPtr[lIntProjIndex + j];
				nSumWeight += pIntWeightPtr[lIntProjIndex + j];
			}
			lIndexDst = (nHeight - 1 - i) * nImgDstPitch;
			lIxdexMask = (rcMaskProj.bottom -1 - i) * nImgMaskPitch + rcMaskProj.left;
			if(nSumWeight > 0) {
				if(pImgMaskPtr[lIxdexMask]) {
					pImgDstPtr[lIndexDst] = nSum/nSumWeight;
				}
			}
			lIntProjIndexAdd = lIntProjIndex + nStepX1;
			for(j=1;j<=nStepX0;j++) {
				nSum += pIntProjPtr[lIntProjIndexAdd + j];
				nSumWeight += pIntWeightPtr[lIntProjIndexAdd + j];
				if(nSumWeight > 0) {
					if(pImgMaskPtr[lIxdexMask + j]) {
						pImgDstPtr[lIndexDst + j] = nSum/nSumWeight;
					}
				}
			}

			lIntProjIndexSub = (nStepY1 - 1 - i) * nImgIntPitch;
			lIntProjIndexAdd = lIntProjIndexSub + pcsProj->cx;
			
			for(j=0;j<=(nWidth - pcsProj->cx - 1);j++) {
				nSum += (pIntProjPtr[lIntProjIndexAdd+j] - pIntProjPtr[lIntProjIndexSub+j]);
				nSumWeight += (pIntWeightPtr[lIntProjIndexAdd+j] - pIntWeightPtr[lIntProjIndexSub+j]);
				if(nSumWeight > 0) {
					if(pImgMaskPtr[lIxdexMask+nStepX0+j + 1]) {
						pImgDstPtr[lIndexDst+nStepX0+j + 1] = nSum/nSumWeight;
					}
				}
			}

			for(j=nWidth - pcsProj->cx;j<(nWidth - nStepX0 - 1);j++) {
				nSum -= pIntProjPtr[lIntProjIndexSub + j];
				nSumWeight -= pIntWeightPtr[lIntProjIndexSub+j];
				if(nSumWeight > 0) {
					if(pImgMaskPtr[lIxdexMask+nStepX0+j + 1]) {
						pImgDstPtr[lIndexDst+nStepX0+j + 1] = nSum/nSumWeight;
					}
				}
			}

		}
		break;
	}
	if(plProj != NULL)
		delete [] plProj;
	if(plWeight != NULL)
		delete [] plWeight;
	return 0;
}

int CSlidingAverage::ImgSlidingAverageForEdge(CImageBuffer *pImgSrc,
				CRect* prcSrc,
				CSize* pcsProj,
				CImageBuffer* pImgDst,
				CRect* prcDst)
{
	int i;
	int nSide[4];
	int nError;
	nSide[0] = LEFT;
	nSide[1] = TOP;
	nSide[2] = RIGHT;
	nSide[3] = BOTTOM;
	for(i=0;i<4;i++) {
		nError = ImgSlidingAverageForEdge(pImgSrc,
					prcSrc,
					pcsProj,
					nSide[i],
					pImgDst,
					prcDst);
	}
	return 0;
}

int CSlidingAverage::ImgSlidingAverageForEdge(CImageBuffer *pImgSrc,
				CRect* prcSrc,
				CSize* pcsProj,
				int nSide,
				CImageBuffer* pImgDst,
				CRect* prcDst)
{
	int i, j;
	int nStepX0;
	int nStepX1;
	int nStepY0;
	int nStepY1;
	BYTE* pImgSrcPtr;
	int nImgSrcPitch;
	long lIndexSrc;
	BYTE* pImgDstPtr;
	int nImgDstPitch;
	long lIndexDst;
	CImgIntBuf ImgIntProj;
	CRect rcEdgeRelative;
	CRect rcSrcEdge;
	CRect rcSrcProj;
	long* plProj;
	int nWidth;
	int nHeight;
	int* pIntProjPtr;
	int nImgIntPitch;
	long lIntProjIndex;
	long lIntProjIndexAdd;
	long lIntProjIndexSub;
	int nWeight0;
	int nWeight;
	int nSum;
	int nError;

	plProj = NULL;
	nStepX0 = pcsProj->cx/2;
	nStepX1 = pcsProj->cx - nStepX0;
	nStepY0 = pcsProj->cy/2;
	nStepY1 = pcsProj->cy - nStepY0;
	pImgSrcPtr = pImgSrc->GetImgPtr();
	nImgSrcPitch = pImgSrc->GetPitch();
	pImgDstPtr = pImgDst->GetImgPtr();
	nImgDstPitch = pImgDst->GetPitch();
	nWidth = prcSrc->Width();
	nHeight = prcSrc->Height();
	switch(nSide) {
	case LEFT:
		plProj = new long[nHeight];
		rcSrcProj = CRect(prcSrc->left,
						  prcSrc->top,
						  prcSrc->left + nStepX1,
						  prcSrc->bottom);
		rcEdgeRelative = CRect(CPoint(0,0), CSize(nStepX0, nHeight));
		ImgIntProj.Allocate(rcEdgeRelative.Size());
		ImgIntProj.ClrImg(0);
		pIntProjPtr = ImgIntProj.GetImgPtr();
		nImgIntPitch = ImgIntProj.GetPitch(); 
		nError = GetYGrayAccl(pImgSrc, &rcSrcProj, plProj);
		lIndexSrc = rcSrcProj.top * nImgSrcPitch + rcSrcProj.left + nStepX1 - 1;
		lIntProjIndex = 0;
		for(i=0;i<nHeight;i++) {
			pIntProjPtr[lIntProjIndex] = plProj[i];
			for(j=1;j<nStepX0;j++) {
				pIntProjPtr[lIntProjIndex+j] = pIntProjPtr[lIntProjIndex+j-1] + pImgSrcPtr[lIndexSrc + j];
			}
			lIntProjIndex += nImgIntPitch;
			lIndexSrc += nImgSrcPitch;
		}
		for(i=0;i<nStepX0;i++) {
			nWeight0 = nStepX1+i;
			nSum = 0;
			nWeight = 0;
			lIntProjIndex = i;
			for(j=0;j<nStepY1;j++) {
				nSum += pIntProjPtr[lIntProjIndex];
				lIntProjIndex += nImgIntPitch;
				nWeight += nWeight0;
			}
			lIndexDst = i;
			if(nWeight > 0)
				pImgDstPtr[lIndexDst] = nSum/nWeight;
			lIndexDst += nImgDstPitch;
			lIntProjIndexAdd = nImgIntPitch * nStepY1 + i;
			for(j=1;j<=nStepY0;j++) {
				nSum += pIntProjPtr[lIntProjIndexAdd];
				nWeight += nWeight0;
				if(nWeight > 0)
					pImgDstPtr[lIndexDst] = nSum/nWeight;
				lIntProjIndexAdd += nImgIntPitch;
				lIndexDst += nImgDstPitch;
			}
			lIntProjIndexSub = i;
			lIntProjIndexAdd = nImgIntPitch * pcsProj->cy + i;
			for(j=nStepY0+1;j<(nHeight - nStepY1);j++) {
				nSum += (pIntProjPtr[lIntProjIndexAdd] - pIntProjPtr[lIntProjIndexSub]);
				pImgDstPtr[lIndexDst] = nSum/nWeight;

				lIntProjIndexSub += nImgIntPitch;
				lIntProjIndexAdd += nImgIntPitch;
				lIndexDst += nImgDstPitch;
			}
			for(j=nHeight - nStepY1;j<nHeight;j++) {
				nSum -= pIntProjPtr[lIntProjIndexSub];
				nWeight -= nWeight0;
				if(nWeight > 0)
					pImgDstPtr[lIndexDst] = nSum/nWeight;
				lIntProjIndexSub += nImgIntPitch;
				lIndexDst += nImgDstPitch;
			}
		}
		break;
	case TOP:
		plProj = new long[nWidth];
		rcSrcProj = CRect(prcSrc->left,
						  prcSrc->top,
						  prcSrc->right,
						  prcSrc->top + nStepY1);

		rcEdgeRelative = CRect(CPoint(0,0), CSize(nWidth, nStepY0));
		ImgIntProj.Allocate(rcEdgeRelative.Size());
		ImgIntProj.ClrImg(0);
		pIntProjPtr = ImgIntProj.GetImgPtr();
		nImgIntPitch = ImgIntProj.GetPitch(); 
		nError = GetXGrayAccl(pImgSrc, &rcSrcProj, plProj);

		for(i=0;i<nWidth;i++) {
			pIntProjPtr[i] = plProj[i];
			lIntProjIndex = nImgIntPitch + i;
			lIndexSrc = (rcSrcProj.top + nStepY1) * nImgSrcPitch + rcSrcProj.left + i;
			for(j=1;j<nStepY0;j++) {
				pIntProjPtr[lIntProjIndex] = pIntProjPtr[lIntProjIndex-nImgIntPitch] + pImgSrcPtr[lIndexSrc];
				lIntProjIndex += nImgIntPitch;
				lIndexSrc += nImgSrcPitch;
			}
		}

		for(i=0;i<nStepY0;i++) {
			nWeight0 = nStepY1+i;
			nSum = 0;
			nWeight = 0;
			lIntProjIndex = i * nImgIntPitch;
			for(j=0;j<nStepX1;j++) {
				nSum += pIntProjPtr[lIntProjIndex + j];
				nWeight += nWeight0;
			}
			lIndexDst = i * nImgDstPitch;
			if(nWeight > 0)
				pImgDstPtr[lIndexDst] = nSum/nWeight;
			lIntProjIndexAdd = lIntProjIndex + nStepX1;
			for(j=1;j<=nStepX0;j++) {
				nSum += pIntProjPtr[lIntProjIndexAdd + j];
				nWeight += nWeight0;
				if(nWeight > 0)
					pImgDstPtr[lIndexDst + j] = nSum/nWeight;
			}

			lIntProjIndexSub = i * nImgIntPitch;
			lIntProjIndexAdd = lIntProjIndexSub + pcsProj->cx;
			
			for(j=0;j<=(nWidth - pcsProj->cx - 1);j++) {
				nSum += (pIntProjPtr[lIntProjIndexAdd+j] - pIntProjPtr[lIntProjIndexSub+j]);
				pImgDstPtr[lIndexDst+nStepX0+j + 1] = nSum/nWeight;
			}
			for(j=nWidth - pcsProj->cx;j<(nWidth - nStepX0 - 1);j++) {
				nSum -= pIntProjPtr[lIntProjIndexSub + j];
				nWeight -= nWeight0;
				if(nWeight > 0)
					pImgDstPtr[lIndexDst+nStepX0+j + 1] = nSum/nWeight;
			}
		}

		break;
	case RIGHT:
		int m;
		plProj = new long[nHeight];
		rcSrcProj = CRect(prcSrc->right - nStepX0,
						  prcSrc->top,
						  prcSrc->right,
						  prcSrc->bottom);
		rcEdgeRelative = CRect(CPoint(0,0), CSize(nStepX1, nHeight));
		ImgIntProj.Allocate(rcEdgeRelative.Size());
		ImgIntProj.Allocate(rcEdgeRelative.Size());
		ImgIntProj.ClrImg(0);
		pIntProjPtr = ImgIntProj.GetImgPtr();
		nImgIntPitch = ImgIntProj.GetPitch(); 
		nError = GetYGrayAccl(pImgSrc, &rcSrcProj, plProj);
		lIndexSrc = rcSrcProj.top * nImgSrcPitch + rcSrcProj.right - nStepX1;
		lIntProjIndex = 0;
		for(i=0;i<nHeight;i++) {
			pIntProjPtr[lIntProjIndex + nStepX1 - 1] = plProj[i];
			m = 2;
			for(j=nStepX1 - 2;j>=0;j--) {
				pIntProjPtr[lIntProjIndex+j] = pIntProjPtr[lIntProjIndex+j+1] + pImgSrcPtr[lIndexSrc - m];
				m++;
			}
			lIntProjIndex += nImgIntPitch;
			lIndexSrc += nImgSrcPitch;
		}
		for(i=0;i<nStepX1;i++) {
			nWeight0 = nStepX0+nStepX1-i-1;
			nSum = 0;
			nWeight = 0;
			lIntProjIndex = i;
			for(j=0;j<nStepY1;j++) {
				nSum += pIntProjPtr[lIntProjIndex];
				lIntProjIndex += nImgIntPitch;
				nWeight += nWeight0;
			}
			lIndexDst = nWidth - nStepX1 + i;
			if(nWeight > 0)
				pImgDstPtr[lIndexDst] = nSum/nWeight;
			lIndexDst += nImgDstPitch;
			lIntProjIndexAdd = nImgIntPitch * nStepY1 + i;
			for(j=1;j<=nStepY0;j++) {
				nSum += pIntProjPtr[lIntProjIndexAdd];
				nWeight += nWeight0;
				if(nWeight > 0)
					pImgDstPtr[lIndexDst] = nSum/nWeight;
				lIntProjIndexAdd += nImgIntPitch;
				lIndexDst += nImgDstPitch;
			}
			lIntProjIndexSub = i;
			lIntProjIndexAdd = nImgIntPitch * pcsProj->cy + i;
			for(j=nStepY0+1;j<(nHeight - nStepY1);j++) {
				nSum += (pIntProjPtr[lIntProjIndexAdd] - pIntProjPtr[lIntProjIndexSub]);
				pImgDstPtr[lIndexDst] = nSum/nWeight;

				lIntProjIndexSub += nImgIntPitch;
				lIntProjIndexAdd += nImgIntPitch;
				lIndexDst += nImgDstPitch;
			}
			for(j=nHeight - nStepY1;j<nHeight;j++) {
				nSum -= pIntProjPtr[lIntProjIndexSub];
				nWeight -= nWeight0;
				if(nWeight > 0)
					pImgDstPtr[lIndexDst] = nSum/nWeight;
				lIntProjIndexSub += nImgIntPitch;
				lIndexDst += nImgDstPitch;
			}
		}
		break;
	case BOTTOM:
		plProj = new long[nWidth];
		rcSrcProj = CRect(prcSrc->left,
						  prcSrc->bottom - nStepY0,
						  prcSrc->right,
						  prcSrc->bottom);

		rcEdgeRelative = CRect(CPoint(0,0), CSize(nWidth, nStepY1));
		ImgIntProj.Allocate(rcEdgeRelative.Size());
		ImgIntProj.ClrImg(0);
		pIntProjPtr = ImgIntProj.GetImgPtr();
		nImgIntPitch = ImgIntProj.GetPitch(); 
		nError = GetXGrayAccl(pImgSrc, &rcSrcProj, plProj);

		for(i=0;i<nWidth;i++) {
			lIntProjIndex = (nStepY1-1) * nImgIntPitch + i;
			pIntProjPtr[lIntProjIndex] = plProj[i];
			lIndexSrc = (rcSrcProj.bottom - nStepY1 - 1) * nImgSrcPitch + rcSrcProj.left + i;
			lIntProjIndex -= nImgIntPitch;;
			for(j=1;j<nStepY1;j++) {
				pIntProjPtr[lIntProjIndex] = pIntProjPtr[lIntProjIndex+nImgIntPitch] + pImgSrcPtr[lIndexSrc];
				lIntProjIndex -= nImgIntPitch;
				lIndexSrc -= nImgSrcPitch;
			}
		}

		for(i=0;i<nStepY1;i++) {
			nWeight0 = nStepY0+i;
			nSum = 0;
			nWeight = 0;
			lIntProjIndex = (nStepY1 - 1 - i) * nImgIntPitch;
			for(j=0;j<nStepX1;j++) {
				nSum += pIntProjPtr[lIntProjIndex + j];
				nWeight += nWeight0;
			}
			lIndexDst = (nHeight - 1 - i) * nImgDstPitch;
			if(nWeight > 0)
				pImgDstPtr[lIndexDst] = nSum/nWeight;
			lIntProjIndexAdd = lIntProjIndex + nStepX1;
			for(j=1;j<=nStepX0;j++) {
				nSum += pIntProjPtr[lIntProjIndexAdd + j];
				nWeight += nWeight0;
				if(nWeight > 0)
					pImgDstPtr[lIndexDst + j] = nSum/nWeight;
			}

			lIntProjIndexSub = (nStepY1 - 1 - i) * nImgIntPitch;
			lIntProjIndexAdd = lIntProjIndexSub + pcsProj->cx;
			
			for(j=0;j<=(nWidth - pcsProj->cx - 1);j++) {
				nSum += (pIntProjPtr[lIntProjIndexAdd+j] - pIntProjPtr[lIntProjIndexSub+j]);
				pImgDstPtr[lIndexDst+nStepX0+j + 1] = nSum/nWeight;
			}
			for(j=nWidth - pcsProj->cx;j<(nWidth - nStepX0 - 1);j++) {
				nSum -= pIntProjPtr[lIntProjIndexSub + j];
				nWeight -= nWeight0;
				if(nWeight > 0)
					pImgDstPtr[lIndexDst+nStepX0+j + 1] = nSum/nWeight;
			}
		}
		break;
	}
	if(plProj != NULL)
		delete [] plProj;
	return 0;
}


int CSlidingAverage::ImgSlidingAverage(CImageBuffer *pImgSrc,
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
	int nError;
try 
	{

	nError = ImgClear(pImgDst,
					  prcDst,
					  0);
	pImgDstPtr = pImgDst->GetImgPtr();
	nPitchDst = pImgDst->GetPitch();
	csProjOrg = *pcsProj;
	int nError;

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

	nError = ImgSlidingAverageForEdge(pImgSrc,
				prcSrc,
				pcsProj,
				pImgDst,
				prcDst);

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
	lIndexDst = (prcDst->top + pcsProj->cy/2) * nPitchDst + prcDst->left + pcsProj->cx/2;
	for(i=0;i<nHeightSrc;i++) {
		pImgDstPtr[lIndexDst] = pIntProjPtr[lProjRowIndex]/nWeight;
		for(j=1;j<nWidthSrc;j++) {
			pIntProjPtr[lProjRowIndex+j] = pIntProjPtr[lProjRowIndex+j-1] + 
				pProjVPtr[lProjVRowIndex + j - 1 + pcsProj->cx ] - pProjVPtr[lProjVRowIndex + j -1];
			pImgDstPtr[lIndexDst + j] = pIntProjPtr[lProjRowIndex+j]/nWeight;
		}
		lProjRowIndex += nImgIntPitch;
		lProjVRowIndex += nImgIntPitchV;
		lIndexDst += nPitchDst;
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

int CSlidingAverage::GetXGrayAcclWithMask(CImageBuffer* pImgSrc, 
							CRect* prcSrc, 
							CImageBuffer* pImgMask, 
							CRect* prcMask, 
							long* plXAry,
							long* plWeightAry)
{
	int i, j;
	BYTE* pImgSrcPtr;
	int nPitchSrc;
	long lIndexSrc;
	BYTE* pImgMaskPtr;
	int nPitchMask;
	long lIndexMask;
	long lSum;
	int nWeight;
	int nError;

	nError = ValidateImageAndRect(pImgSrc, prcSrc);
	if(nError)
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgMask, prcMask);
	if(nError)
		goto ErrorAbort;
	if(prcSrc->Size() != prcMask->Size())
		goto ErrorAbort;

	pImgSrcPtr = pImgSrc->GetImgPtr();
	nPitchSrc = pImgSrc->GetPitch();
	pImgMaskPtr = pImgMask->GetImgPtr();
	nPitchMask = pImgMask->GetPitch();

	for(i=0;i<prcSrc->Width();i++) {
		lSum = 0;
		nWeight = 0;
		lIndexSrc = prcSrc->top * nPitchSrc + prcSrc->left + i;
		lIndexMask = prcMask->top * nPitchMask + prcMask->left + i;
		for(j=0;j<prcSrc->Height();j++, lIndexSrc += nPitchSrc, lIndexMask += nPitchMask) {
			if(pImgMaskPtr[lIndexMask] == 0)
				continue;
			lSum += pImgSrcPtr[lIndexSrc];
			nWeight++;
		}
		plXAry[i] = lSum;
		plWeightAry[i] = nWeight;
	}

	return 0;
ErrorAbort:
	return -1;
}

int CSlidingAverage::GetYGrayAcclWithMask(CImageBuffer* pImgSrc, 
							CRect* prcSrc, 
							CImageBuffer* pImgMask, 
							CRect* prcMask, 
							long* plYAry,
							long* plWeightAry)
{
	int i, j;
	BYTE* pImgSrcPtr;
	int nPitchSrc;
	long lIndexSrc;
	BYTE* pImgMaskPtr;
	int nPitchMask;
	long lIndexMask;
	long lSum;
	int nWeight;
	int nError;

	nError = ValidateImageAndRect(pImgSrc, prcSrc);
	if(nError)
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgMask, prcMask);
	if(nError)
		goto ErrorAbort;
	if(prcSrc->Size() != prcMask->Size())
		goto ErrorAbort;

	pImgSrcPtr = pImgSrc->GetImgPtr();
	nPitchSrc = pImgSrc->GetPitch();
	pImgMaskPtr = pImgMask->GetImgPtr();
	nPitchMask = pImgMask->GetPitch();

	lIndexSrc = prcSrc->top * nPitchSrc + prcSrc->left;
	lIndexMask = prcMask->top * nPitchMask + prcMask->left;
	for(i=0;i<prcSrc->Height();i++) {
		lSum = 0;
		nWeight = 0;
		for(j=0;j<prcSrc->Width();j++) {
			if(pImgMaskPtr[lIndexMask+j] == 0)
				continue;
			lSum += pImgSrcPtr[lIndexSrc+j];
			nWeight++;
		}
		lIndexSrc += nPitchSrc;
		lIndexMask += nPitchMask;
		
		plYAry[i] = lSum;
		plWeightAry[i] = nWeight;
	}

	return 0;
ErrorAbort:
	return -1;
}

int CSlidingAverage::ImgSlidingAverage(CImageBuffer *pImgSrc,
				CRect* prcSrc,
				CImageBuffer* pImgMask,
				CRect* prcMask,
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
	CImgIntBuf ImgIntProjWV;
	int* pIntProjWVPtr;
	CImgIntBuf ImgIntProjW;
	int* pIntProjWPtr;
	BYTE* pImgMaskPtr;
	int nPitchMask;
	long lImgMaskLostRowIndex;
	long lImgMaskAddRowIndex;
	long *plWeightXAry;
	long *plWeightYAry;
	CRect rcMaskFirstProj;
	CImageBuffer ImgMaskErosion;
	CRect rcRelative;
	BYTE* pImgMaskErosionPtr;
	int nImgMaskErosionPitch;
	long lImgMaskErosionIndex;
	int nAvgX;
	int nAvgY;
	long lProjVRowIndexADDj;
	long lImgMaskIndex;
	int nError;
try 
	{
	plXAry = NULL;
	plYAry = NULL;
	plWeightXAry = NULL;
	plWeightYAry = NULL;

	nAvgX = pcsProj->cx;
	nAvgY = pcsProj->cy;
	rcRelative = CRect(CPoint(0,0), prcMask->Size());
	ImgMaskErosion.Allocate(rcRelative.Size());
	pImgMaskErosionPtr = ImgMaskErosion.GetImgPtr();
	nImgMaskErosionPitch = ImgMaskErosion.GetPitch();

	nError = ImgClear(pImgDst,
					  prcDst,
					  0);
	nError = ImgSlidingAverageForEdge(pImgSrc,
				prcSrc,
				pImgMask,
				prcMask,
				pcsProj,
				pImgDst,
				prcDst);

	nError = ImgBinDilation(pImgMask,
					  prcMask,
					  BLACK,
					  &ImgMaskErosion,
					  &rcRelative,
					  pcsProj);
	if(nError)
		goto ErrorAbort;
	pImgDstPtr = pImgDst->GetImgPtr();
	nPitchDst = pImgDst->GetPitch();
	csProjOrg = *pcsProj;

	nWeight = pcsProj->cx * pcsProj->cy;
	rcProj = CRect(CPoint(0,0), CSize(prcSrc->Size() - csProjOrg));
	ImgIntProj.Allocate(rcProj.Size());
	pImgIntProj = &ImgIntProj;
	prcProj = &rcProj;

	pImgMaskPtr = pImgMask->GetImgPtr();
	nPitchMask = pImgMask->GetPitch();

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

	ImgIntProjWV.Allocate(rcProjV.Size());
	ImgIntProjWV.ClrImg(0);
	pIntProjWVPtr = ImgIntProjWV.GetImgPtr();

	nImgPitchSrc = pImgSrc->GetPitch();
	nImgIntPitchV = ImgIntProjV.GetPitch();
	nWidthSrc = rcProjV.Width();
	nHeightSrc = rcProjV.Height();
	pImgSrcPtr = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;
	pProjVPtr = ImgIntProjV.GetImgPtr();

	plXAry = new long[prcSrc->Width()];
	plWeightXAry  = new long[prcSrc->Width()];
	rcFirstProj = CRect(prcSrc->TopLeft(), CSize(prcSrc->Width(), pcsProj->cy));
	rcMaskFirstProj = CRect(prcMask->TopLeft(), CSize(prcSrc->Width(), pcsProj->cy));

	nError = GetXGrayAcclWithMask(pImgSrc, 
							&rcFirstProj, 
							pImgMask, 
							&rcMaskFirstProj,
							&plXAry[0],
							&plWeightXAry[0]);

	for(i=0;i<nWidthSrc;i++) {
		pProjVPtr[i] = plXAry[i];
		pIntProjWVPtr[i] = plWeightXAry[i];
	}

	lImgSrcLostRowIndex = 0;
	lImgSrcAddRowIndex = nImgPitchSrc * pcsProj->cy;
	lProjVRowIndex = nImgIntPitchV;

	lImgMaskLostRowIndex = prcMask->top * nPitchMask;
	lImgMaskAddRowIndex = (prcMask->top + pcsProj->cy) * nPitchMask;

	lImgMaskErosionIndex = nImgMaskErosionPitch;
	for(i=1;i<nHeightSrc;i++) {
		for(j=0;j<nWidthSrc;j++) {
			lProjVRowIndexADDj = lProjVRowIndex + j;
			if(pImgMaskErosionPtr[lImgMaskErosionIndex + j]) {
				pProjVPtr[lProjVRowIndexADDj] = pProjVPtr[lProjVRowIndexADDj - nImgIntPitchV] + 
					pImgSrcPtr[lImgSrcAddRowIndex + j] - pImgSrcPtr[lImgSrcLostRowIndex+j];
				pIntProjWVPtr[lProjVRowIndexADDj] = nAvgY;
			}
			else {
				pProjVPtr[lProjVRowIndexADDj] = pProjVPtr[lProjVRowIndexADDj - nImgIntPitchV];
				pIntProjWVPtr[lProjVRowIndexADDj] = pIntProjWVPtr[lProjVRowIndexADDj - nImgIntPitchV];
				if(pImgMaskPtr[lImgMaskAddRowIndex + j]) {
					pProjVPtr[lProjVRowIndexADDj] += pImgSrcPtr[lImgSrcAddRowIndex + j];
					pIntProjWVPtr[lProjVRowIndexADDj]++;
				}
				if(pImgMaskPtr[lImgMaskLostRowIndex + j]) {
					pProjVPtr[lProjVRowIndexADDj] -= pImgSrcPtr[lImgSrcLostRowIndex+j];
					pIntProjWVPtr[lProjVRowIndexADDj]--;
				}
			}
		}
		lImgSrcLostRowIndex += nImgPitchSrc;
		lImgSrcAddRowIndex += nImgPitchSrc;
		lProjVRowIndex += nImgIntPitchV;
		lImgMaskLostRowIndex += nPitchMask;
		lImgMaskAddRowIndex += nPitchMask;
		lImgMaskErosionIndex += nImgMaskErosionPitch;
	}
	*prcProj = CRect(CPoint(0,0), CSize(prcSrc->Width() - pcsProj->cx, prcSrc->Height() - pcsProj->cy));
	pImgIntProj->Allocate(prcProj->Size());
	pImgIntProj->ClrImg(0);
	nImgIntPitch = pImgIntProj->GetPitch();
	nWidthSrc = prcProj->Width();
	nHeightSrc = prcProj->Height();

	pIntProjPtr = pImgIntProj->GetImgPtr();

	ImgIntProjW.Allocate(prcProj->Size());
	ImgIntProjW.ClrImg(0);
	pIntProjWPtr = ImgIntProjW.GetImgPtr();


	plYAry = new long[nHeightSrc];
	plWeightYAry = new long[nHeightSrc];
	
	lProjRowIndex = 0;
	lProjVRowIndex = 0;
	for(i=0;i<nHeightSrc;i++) {
		plYAry[i] = 0;
		plWeightYAry[i] = 0;
		for(j=0;j<pcsProj->cx;j++) {
			plYAry[i] += pProjVPtr[lProjVRowIndex + j];
			plWeightYAry[i] += pIntProjWVPtr[lProjVRowIndex + j];
		}
		pIntProjPtr[lProjRowIndex] = plYAry[i];
		pIntProjWPtr[lProjRowIndex] = plWeightYAry[i];

		lProjVRowIndex += nImgIntPitchV;
		lProjRowIndex += nImgIntPitch;
	}
	lProjRowIndex = 0;
	lProjVRowIndex = 0;
	lIndexDst = (prcDst->top + pcsProj->cy/2) * nPitchDst + prcDst->left + pcsProj->cx/2;
	lImgMaskErosionIndex = 0;
	lImgMaskIndex = (prcMask->top + pcsProj->cy/2) * nPitchMask + prcMask->left + pcsProj->cx/2;

	for(i=0;i<nHeightSrc;i++) {
		if(pImgMaskPtr[lImgMaskIndex]) {
			if(pIntProjWPtr[lProjRowIndex] > 0)
				pImgDstPtr[lIndexDst] = pIntProjPtr[lProjRowIndex]/pIntProjWPtr[lProjRowIndex];
		}
		for(j=1;j<nWidthSrc;j++) {
			pIntProjPtr[lProjRowIndex+j] = pIntProjPtr[lProjRowIndex+j-1] + 
				pProjVPtr[lProjVRowIndex + j - 1 + nAvgX ] - pProjVPtr[lProjVRowIndex + j -1];
			if(pImgMaskErosionPtr[lImgMaskErosionIndex + j]) {
				pImgDstPtr[lIndexDst + j] = pIntProjPtr[lProjRowIndex+j]/nWeight;
				pIntProjWPtr[lProjRowIndex+j] = nWeight;
			}
			else {
				pIntProjWPtr[lProjRowIndex+j] = pIntProjWPtr[lProjRowIndex+j-1] + 
					pIntProjWVPtr[lProjVRowIndex + j - 1 + nAvgX] - pIntProjWVPtr[lProjVRowIndex + j -1];
				if(pIntProjWPtr[lProjRowIndex+j] > 0)
					pImgDstPtr[lIndexDst + j] = pIntProjPtr[lProjRowIndex+j]/pIntProjWPtr[lProjRowIndex+j];
				else
					pImgDstPtr[lIndexDst + j] = 0;
			}
			if(pImgMaskPtr[lImgMaskIndex+j] == FALSE) 
				pImgDstPtr[lIndexDst + j] = 0;
		}
		lProjRowIndex += nImgIntPitch;
		lProjVRowIndex += nImgIntPitchV;
		lIndexDst += nPitchDst;
		lImgMaskErosionIndex += nImgMaskErosionPitch;
		lImgMaskIndex += nPitchMask;
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
	if(plWeightXAry != NULL)
		delete [] plWeightXAry;
	if(plWeightYAry != NULL)
		delete [] plWeightYAry;
	*pcsProj = csProjOrg;
	return 0;
ErrorAbort:
	if(plXAry != NULL)
		delete [] plXAry;
	if(plYAry != NULL)
		delete [] plYAry;
	if(plWeightXAry != NULL)
		delete [] plWeightXAry;
	if(plWeightYAry != NULL)
		delete [] plWeightYAry;
	*pcsProj = csProjOrg;
	return -1;
}



CBlob1D::CBlob1D()
{
}

CBlob1D::~CBlob1D()
{
	Clean();
}
	
void CBlob1D::Clean()
{
	AryPtBlobRange.RemoveAll();
}

int CBlob1D::Inspect(CArray<BOOL, BOOL>* pArybVal,
				BOOL bBlobVal,
				int nMinBlobLength,
				int nMaxBlobLength)
{
	int i;
	BOOL* pbVal;
	int nValNo;
	int nError;
try {
	pbVal = NULL;
	if(pArybVal->GetSize() < 0)
		goto ErrorAbort;
	nValNo = (int) pArybVal->GetSize();
	pbVal = new BOOL[abs(nValNo)];
	for(i=0;i<nValNo;i++)
		pbVal[i] = pArybVal->GetAt(i);

	nError = Inspect(pbVal,
				nValNo,
				bBlobVal,
				nMinBlobLength,
				nMaxBlobLength);
	if(nError)
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CBlob1D::Inspect");
	goto ErrorAbort;
}
	if(pbVal != NULL)
		delete [] pbVal;
	return 0;
ErrorAbort:
	if(pbVal != NULL)
		delete [] pbVal;
	return -1;
}

int CBlob1D::Inspect(BOOL* pbVal,
				int nValNo,
				BOOL bBlobVal,
				int nMinBlobLength,
				int nMaxBlobLength)
{
	int i, j;
	CArray<CPoint,CPoint> AryPtBlobRangeTemp;
	CPoint ptRange;
	int nDis;

try
{
	Clean();
	AryPtBlobRangeTemp.RemoveAll();

	for(i=0;i<nValNo;i++) {
		if(pbVal[i] != bBlobVal)
			continue;
		for(j=i;j<nValNo;j++) {
			if(pbVal[j] != bBlobVal)
				break;
		}
		AryPtBlobRangeTemp.Add(CPoint(i,j));
		i=j;
	}
	for(i=0;i<AryPtBlobRangeTemp.GetSize();i++) {
		ptRange = AryPtBlobRangeTemp.GetAt(i);
		nDis = ptRange.y - ptRange.x;
		if((nDis >=nMinBlobLength) && (nDis <=nMaxBlobLength))
			AryPtBlobRange.Add(ptRange);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CBlob1D::Inspect");
	goto ErrorAbort;
}
	AryPtBlobRangeTemp.RemoveAll();
	return 0;
ErrorAbort:
	AryPtBlobRangeTemp.RemoveAll();
	return -1;

}

int CBlob1D::AND(BOOL* pbValSrc0,
				BOOL* pbValSrc1,
				BOOL* pbValDst,
				int nValNo)
{
	int i;
	for(i=0;i<nValNo;i++) {
		pbValDst[i] = pbValSrc0[i] && pbValSrc1[i];
	}
	return 0;
}


int CBlob1D::GetBlobRangebySeed(BOOL* pbVal,
				int nValNo,
				int nPos,
				CPoint* pptRange)
{
	int i;
	int nLeft;
	int nRight;
	BOOL bColor;

	if((nPos < 0) || (nPos >= nValNo))
		goto ErrorAbort;
	bColor = pbVal[nPos];
	nLeft = nPos;
	nRight = nPos;
	for(i=nPos;i>=0;i--) {
		if(pbVal[i] != bColor) {
			break;
		}
	}
	nLeft = i+1;
	for(i=nPos;i<nValNo;i++) {
		if(pbVal[i] != bColor) {
			nRight = i;
			break;
		}
	}
	nRight = __min(i+1, nValNo);
	*pptRange = CPoint(nLeft, nRight);
	return 0;
ErrorAbort:
	return -1;
}

int CBlob1D::Dilation(CArray<BOOL, BOOL>* pArybVal,
				int nDilationSize)
{
	int i;
	BOOL* pbVal;
	int nValNo;
	int nError;

	pbVal = NULL;
	nValNo = (int) pArybVal->GetSize();
	if(nValNo <= 0)
		goto ErrorAbort;
	pbVal = new BOOL[nValNo];
	for(i=0;i<nValNo;i++)
	{
		pbVal[i] = pArybVal->GetAt(i);
	}
	nError = Dilation(pbVal,
				nValNo,
				nDilationSize);
	pArybVal->RemoveAll();
	for(i=0;i<nValNo;i++)
	{
		pArybVal->Add(pbVal[i]);
	}
	if(pbVal != NULL)
		delete [] pbVal;
	return 0;
ErrorAbort:
	if(pbVal != NULL)
		delete [] pbVal;
	return -1;
}


int CBlob1D::Dilation(BOOL* pbVal,
				int nValNo,
				int nDilationSize)
{
	int i, j;
	int nStartPos;
	int nEndPos;
	for(i=1;i<nValNo;i++) {
		if(pbVal[i-1] != pbVal[i]) {
			if(pbVal[i]) {
				nStartPos = i - nDilationSize;
				if(nStartPos < 0)
					nStartPos = 0;
				for(j=nStartPos;j<i;j++)
					pbVal[j] = TRUE;
			}
			else {
				nEndPos = i + nDilationSize;
				if(nEndPos > nValNo)
					nEndPos = nValNo;
				for(j=i;j<nEndPos;j++)
					pbVal[j] = TRUE;
				i = nEndPos;
			}
		}
	}
	return 0;
}


int CBlob1D::Erosion(CArray<BOOL, BOOL>* pArybVal,
				int nDilationSize)
{
	int i;
	BOOL* pbVal;
	int nValNo;
	int nError;

	pbVal = NULL;
	nValNo = (int) pArybVal->GetSize();
	if(nValNo <= 0)
		goto ErrorAbort;
	pbVal = new BOOL[nValNo];
	for(i=0;i<nValNo;i++)
	{
		pbVal[i] = pArybVal->GetAt(i);
	}
	nError = Erosion(pbVal,
				nValNo,
				nDilationSize);
	pArybVal->RemoveAll();
	for(i=0;i<nValNo;i++)
	{
		pArybVal->Add(pbVal[i]);
	}
	if(pbVal != NULL)
		delete [] pbVal;
	return 0;
ErrorAbort:
	if(pbVal != NULL)
		delete [] pbVal;
	return -1;
}


int CBlob1D::Erosion(BOOL* pbVal,
				int nValNo,
				int nErosionSize)
{
	int i, j;
	int nStartPos;
	int nEndPos;
	for(i=1;i<nValNo;i++) {
		if(pbVal[i-1] != pbVal[i]) {
			if(pbVal[i]) {
				nEndPos = i + nErosionSize;
				if(nEndPos > nValNo)
					nEndPos = nValNo;
				for(j=i;j<nEndPos;j++)
					pbVal[j] = FALSE;
				i = nEndPos;
			}
			else {
				nStartPos = i - nErosionSize;
				if(nStartPos < 0)
					nStartPos = 0;
				for(j=nStartPos;j<i;j++)
					pbVal[j] = FALSE;
			}
		}
	}
	return 0;
}

CPointArrayMatching::CPointArrayMatching()
{
}

CPointArrayMatching::~CPointArrayMatching()
{
}

void CPointArrayMatching::Clean()
{
}

int CPointArrayMatching::Inspect(CArray<CPoint2D, CPoint2D>* pAryTempl,
				CArray<CPoint2D, CPoint2D>* pAryInsp,
				CArray<CPoint, CPoint>* pAryMatchingIndex,
				CArray<CPoint2D, CPoint2D>* pAryTemplMappingToInsp,
				CSize* pcsMatchingTol,
				CSize* pcsShiftTol,
				CPoint2D* ppt2DFoundBestShift)
{
	int i, j;
	CArray<CPoint2D, CPoint2D> AryTemplNotMatched1;
	CArray<CPoint2D, CPoint2D> AryInspNotMatched1;
	CArray<CPoint2D, CPoint2D> AryTemplNotMatched2;
	CArray<CPoint2D, CPoint2D> AryInspNotMatched2;
	CPoint2D pt2DFoundBestShift0;
	CPoint2D pt2DFoundBestShift1;
	CPoint2D pt2DInsp;
	CPoint2D pt2DTempl;
	int nMatchedNo0;
	int nMatchedNo1;
	int nDis;
	int nMinDis;
	int nMinDisIndex;
	CArray<CPoint2D, CPoint2D> AryPosXAndDifX;
	CArray<CPoint2D, CPoint2D> AryPosYAndDifY;
	double dNominalAngleRad;
	double dTolAngleRad;
	double dAngleStep;
	double dOnLineTol;
	double dOnLinePercentage;
	double dOffsetX;
	double dOffsetY;
	int nError;

	pAryMatchingIndex->RemoveAll();
	AryTemplNotMatched1.RemoveAll();
	AryInspNotMatched1.RemoveAll();
	AryTemplNotMatched2.RemoveAll();
	AryInspNotMatched2.RemoveAll();
	AryPosXAndDifX.RemoveAll();
	AryPosYAndDifY.RemoveAll();
	nError = InspectByRegion(pAryTempl,
							pAryInsp,
							pAryMatchingIndex,
							pcsMatchingTol,
							pcsShiftTol,
							&pt2DFoundBestShift0,
							&AryTemplNotMatched1,
							&AryInspNotMatched1);
	*ppt2DFoundBestShift = pt2DFoundBestShift0;
	nMatchedNo0 = (int) pAryMatchingIndex->GetSize();
	if(AryTemplNotMatched1.GetSize() > (pAryTempl->GetSize() * 0.4)) {
		nError = InspectByRegion(&AryTemplNotMatched1,
								&AryInspNotMatched1,
								pAryMatchingIndex,
								pcsMatchingTol,
								pcsShiftTol,
								&pt2DFoundBestShift1,
								&AryTemplNotMatched2,
								&AryInspNotMatched2);
		nMatchedNo1 = (int) pAryMatchingIndex->GetSize() - nMatchedNo0;
		if((nMatchedNo0 + nMatchedNo1) > 0) {
			*ppt2DFoundBestShift = CPoint2D((pt2DFoundBestShift0.x * nMatchedNo0 + pt2DFoundBestShift1.x * nMatchedNo1)/(nMatchedNo0 + nMatchedNo1),
											(pt2DFoundBestShift0.y * nMatchedNo0 + pt2DFoundBestShift1.y * nMatchedNo1)/(nMatchedNo0 + nMatchedNo1));
		}
	}
	pAryMatchingIndex->RemoveAll();
	for(i=0;i<pAryTempl->GetSize();i++) {
		pt2DTempl = pAryTempl->GetAt(i) + *ppt2DFoundBestShift;
		nMinDis = INT_MAX;
		nMinDisIndex = 0;
		for(j=0;j<pAryInsp->GetSize();j++) {
			pt2DInsp = pAryInsp->GetAt(j);
			nDis = (int) (fabs((double) (pt2DInsp.x - pt2DTempl.x)) + fabs((double) (pt2DInsp.y - pt2DTempl.y)));
			if(nMinDis > nDis) {
				nMinDis = nDis;
				nMinDisIndex = j;
			}
		}
		pAryMatchingIndex->Add(CPoint(i, nMinDisIndex));
		AryPosXAndDifX.Add(CPoint2D(pt2DTempl.x, pAryInsp->GetAt(nMinDisIndex).x - pt2DTempl.x));
		AryPosYAndDifY.Add(CPoint2D(pt2DTempl.y, pAryInsp->GetAt(nMinDisIndex).y - pt2DTempl.y));
	}

	dNominalAngleRad = 0;
	dTolAngleRad = 5 * PI/180;
	dAngleStep = 0.1 * PI/180;
	dOnLineTol = 4;
	nError = GetHoughTransformLine(&AryPosXAndDifX,
							dNominalAngleRad,
							dTolAngleRad,
							dAngleStep,
							dOnLineTol,
							&dOnLinePercentage,
							&ln2DZoomX);

	nError = GetHoughTransformLine(&AryPosYAndDifY,
							dNominalAngleRad,
							dTolAngleRad,
							dAngleStep,
							dOnLineTol,
							&dOnLinePercentage,
							&ln2DZoomY);

	for(i=0;i<pAryTempl->GetSize();i++) {
		pt2DTempl = pAryTempl->GetAt(i) + *ppt2DFoundBestShift;
		ln2DZoomX.GetY(pt2DTempl.x, &dOffsetX);
		ln2DZoomY.GetY(pt2DTempl.y, &dOffsetY);
		pt2DTempl = CPoint2D(pt2DTempl.x + dOffsetX, pt2DTempl.y + dOffsetY);
		pAryTemplMappingToInsp->Add(pt2DTempl);
	}


	AryTemplNotMatched1.RemoveAll();
	AryInspNotMatched1.RemoveAll();
	AryTemplNotMatched2.RemoveAll();
	AryInspNotMatched2.RemoveAll();
	AryPosXAndDifX.RemoveAll();
	AryPosYAndDifY.RemoveAll();
	return 0;
}


int CPointArrayMatching::Inspect(CArray<CPoint2D, CPoint2D>* pAryTempl,
				CArray<CPoint2D, CPoint2D>* pAryInsp,
				CArray<CPoint, CPoint>* pAryMatchingIndex,
				CArray<CPoint2D, CPoint2D>* pAryTemplMappingToInsp,
				int nMinMatchingPercentageTol,
				CSize* pcsMatchingTol,
				CSize* pcsShiftTol,
				CPoint2D* ppt2DFoundBestShift)
{
	int i, j;
	CArray<CPoint2D, CPoint2D> AryTemplNotMatched1;
	CArray<CPoint2D, CPoint2D> AryInspNotMatched1;
	CArray<CPoint2D, CPoint2D> AryTemplNotMatched2;
	CArray<CPoint2D, CPoint2D> AryInspNotMatched2;
	CPoint2D pt2DFoundBestShift0;
	CPoint2D pt2DFoundBestShift1;
	CPoint2D pt2DInsp;
	CPoint2D pt2DTempl;
	int nMatchedNo0;
	int nMatchedNo1;
	int nDis;
	int nMinDis;
	int nMinDisIndex;
	CArray<CPoint2D, CPoint2D> AryPosXAndDifX;
	CArray<CPoint2D, CPoint2D> AryPosYAndDifY;
	double dNominalAngleRad;
	double dTolAngleRad;
	double dAngleStep;
	double dOnLineTol;
	double dOnLinePercentage;
	double dOffsetX;
	double dOffsetY;
	int nError;
try
{
	pAryMatchingIndex->RemoveAll();
	AryTemplNotMatched1.RemoveAll();
	AryInspNotMatched1.RemoveAll();
	AryTemplNotMatched2.RemoveAll();
	AryInspNotMatched2.RemoveAll();
	AryPosXAndDifX.RemoveAll();
	AryPosYAndDifY.RemoveAll();
	nError = InspectByRegion(pAryTempl,
							pAryInsp,
							pAryMatchingIndex,
							pcsMatchingTol,
							pcsShiftTol,
							&pt2DFoundBestShift0,
							&AryTemplNotMatched1,
							&AryInspNotMatched1);
	*ppt2DFoundBestShift = pt2DFoundBestShift0;
	nMatchedNo0 = (int) pAryMatchingIndex->GetSize();
	if(AryTemplNotMatched1.GetSize() > (pAryTempl->GetSize() * 0.4)) {
		nError = InspectByRegion(&AryTemplNotMatched1,
								&AryInspNotMatched1,
								pAryMatchingIndex,
								pcsMatchingTol,
								pcsShiftTol,
								&pt2DFoundBestShift1,
								&AryTemplNotMatched2,
								&AryInspNotMatched2);
		nMatchedNo1 = (int) pAryMatchingIndex->GetSize() - nMatchedNo0;
		if((nMatchedNo0 + nMatchedNo1) > 0) {
			*ppt2DFoundBestShift = CPoint2D((pt2DFoundBestShift0.x * nMatchedNo0 + pt2DFoundBestShift1.x * nMatchedNo1)/(nMatchedNo0 + nMatchedNo1),
											(pt2DFoundBestShift0.y * nMatchedNo0 + pt2DFoundBestShift1.y * nMatchedNo1)/(nMatchedNo0 + nMatchedNo1));
		}
	}
	pAryMatchingIndex->RemoveAll();
	for(i=0;i<pAryTempl->GetSize();i++) {
		pt2DTempl = pAryTempl->GetAt(i) + *ppt2DFoundBestShift;
		nMinDis = INT_MAX;
		nMinDisIndex = 0;
		for(j=0;j<pAryInsp->GetSize();j++) {
			pt2DInsp = pAryInsp->GetAt(j);
			nDis = (int) (fabs((double) (pt2DInsp.x - pt2DTempl.x)) + fabs((double) (pt2DInsp.y - pt2DTempl.y)));
			if(nMinDis > nDis) {
				nMinDis = nDis;
				nMinDisIndex = j;
			}
		}
		pAryMatchingIndex->Add(CPoint(i, nMinDisIndex));
		AryPosXAndDifX.Add(CPoint2D(pt2DTempl.x, pAryInsp->GetAt(nMinDisIndex).x - pt2DTempl.x));
		AryPosYAndDifY.Add(CPoint2D(pt2DTempl.y, pAryInsp->GetAt(nMinDisIndex).y - pt2DTempl.y));
	}

	dNominalAngleRad = 0;
	dTolAngleRad = 5 * PI/180;
	dAngleStep = 0.1 * PI/180;
	dOnLineTol = 4;
	nError = GetHoughTransformLine(&AryPosXAndDifX,
							dNominalAngleRad,
							dTolAngleRad,
							dAngleStep,
							dOnLineTol,
							&dOnLinePercentage,
							&ln2DZoomX);
	
	
	OutputDebugLogTo(3,TRUE," ****DOT MATCH**** OnLine Perc = %.3f ; MinMatchScore = %d", dOnLinePercentage, nMinMatchingPercentageTol);
		
	if(dOnLinePercentage < nMinMatchingPercentageTol)
		goto ErrorAbort;

	nError = GetHoughTransformLine(&AryPosYAndDifY,
							dNominalAngleRad,
							dTolAngleRad,
							dAngleStep,
							dOnLineTol,
							&dOnLinePercentage,
							&ln2DZoomY);
	
	OutputDebugLogTo(4,TRUE," ****DOT MATCH**** OnLine Perc = %.3f ; MinMatchScore = %d", dOnLinePercentage, nMinMatchingPercentageTol);
	
	if(dOnLinePercentage < nMinMatchingPercentageTol)
		goto ErrorAbort;

	for(i=0;i<pAryTempl->GetSize();i++) {
		pt2DTempl = pAryTempl->GetAt(i) + *ppt2DFoundBestShift;
		ln2DZoomX.GetY(pt2DTempl.x, &dOffsetX);
		ln2DZoomY.GetY(pt2DTempl.y, &dOffsetY);
		pt2DTempl = CPoint2D(pt2DTempl.x + dOffsetX, pt2DTempl.y + dOffsetY);
		pAryTemplMappingToInsp->Add(pt2DTempl);
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CPointArrayMatching::Inspect");
	goto ErrorAbort;
}
	AryTemplNotMatched1.RemoveAll();
	AryInspNotMatched1.RemoveAll();
	AryTemplNotMatched2.RemoveAll();
	AryInspNotMatched2.RemoveAll();
	AryPosXAndDifX.RemoveAll();
	AryPosYAndDifY.RemoveAll();
	return 0;
ErrorAbort:
	AryTemplNotMatched1.RemoveAll();
	AryInspNotMatched1.RemoveAll();
	AryTemplNotMatched2.RemoveAll();
	AryInspNotMatched2.RemoveAll();
	AryPosXAndDifX.RemoveAll();
	AryPosYAndDifY.RemoveAll();
	return -1;
}


int CPointArrayMatching::InspectByRegion(CArray<CPoint2D, CPoint2D>* pAryTempl,
				CArray<CPoint2D, CPoint2D>* pAryInsp,
				CArray<CPoint, CPoint>* pAryMatchingIndex,
				CSize* pcsMatchingTol,
				CSize* pcsShiftTol,
				CPoint2D* ppt2DFoundBestShift,
				CArray<CPoint2D, CPoint2D>* pAryTemplNotMatched,
				CArray<CPoint2D, CPoint2D>* pAryInspNotMatched)
{
	int i, j;
	CRect rcRange;
	double* pdShiftX;
	double* pdShiftY;
	CPoint2D pt2DInsp;
	CPoint2D pt2DTempl;
	CPoint* pptTempl;
	int nShiftNo;
	CAdvHistogram AdvHistogram;
	int	nMaxHistX;
	double dAvgPosX;
	int	nMaxHistY;
	double dAvgPosY;
	BOOL* pbMatchedTempl;
	BOOL* pbMatchedInsp;
	int nError;

try
{
	pdShiftX = NULL;
	pdShiftY = NULL;
	pptTempl = NULL;
	pbMatchedTempl = NULL;
	pbMatchedInsp = NULL;
	*ppt2DFoundBestShift = CPoint2D(0,0);
	pptTempl = new CPoint[pAryTempl->GetSize()];
	for(i=0;i<pAryTempl->GetSize();i++) {
		pt2DTempl = pAryTempl->GetAt(i);
		pptTempl[i] = CPoint((int) pt2DTempl.x, (int) pt2DTempl.y);
	}
	pdShiftX = new double[pAryInsp->GetSize() * pAryTempl->GetSize()];
	pdShiftY = new double[pAryInsp->GetSize() * pAryTempl->GetSize()];
	nShiftNo = 0;
	for(i=0;i<pAryInsp->GetSize();i++) {
		pt2DInsp = pAryInsp->GetAt(i);
		rcRange = CRect(CPoint((int) pt2DInsp.x, (int) pt2DInsp.y), CSize(0,0));
		rcRange.InflateRect(pcsShiftTol->cx, pcsShiftTol->cy,pcsShiftTol->cx, pcsShiftTol->cy);
		for(j=0;j<pAryTempl->GetSize();j++) {
			if(ValidatePoint(&pptTempl[j], &rcRange))
				continue;
			pt2DTempl = pAryTempl->GetAt(j);
			pdShiftX[nShiftNo] = pt2DInsp.x - pt2DTempl.x;
			pdShiftY[nShiftNo] = pt2DInsp.y - pt2DTempl.y;
			nShiftNo++;
		}
	}

	nError = AdvHistogram.GetMaxConvHist1D(pdShiftX, 
									nShiftNo,
									pcsMatchingTol->cx,
									nMaxHistX,
									dAvgPosX);
	nError = AdvHistogram.GetMaxConvHist1D(pdShiftY, 
									nShiftNo,
									pcsMatchingTol->cy,
									nMaxHistY,
									dAvgPosY);
	*ppt2DFoundBestShift = CPoint2D(dAvgPosX,dAvgPosY);
	pbMatchedTempl = new BOOL[pAryTempl->GetSize()];
	pbMatchedInsp = new BOOL[pAryInsp->GetSize()];
	for(i=0;i<pAryTempl->GetSize();i++) 
		pbMatchedTempl[i] = FALSE;
	for(i=0;i<pAryInsp->GetSize();i++) 
		pbMatchedInsp[i] = FALSE;
	for(i=0;i<pAryInsp->GetSize();i++) {
		pt2DInsp = pAryInsp->GetAt(i);
		for(j=0;j<pAryTempl->GetSize();j++) {
			pt2DTempl = pAryTempl->GetAt(j) + *ppt2DFoundBestShift;
			if((fabs(pt2DInsp.x - pt2DTempl.x) < pcsMatchingTol->cx) &&
				(fabs(pt2DInsp.y - pt2DTempl.y) < pcsMatchingTol->cy)) {
				pbMatchedInsp[i] = TRUE;
				pbMatchedTempl[j] = TRUE;
				pAryMatchingIndex->Add(CPoint(i,j));
			}
		}
	}

	for(i=0;i<pAryTempl->GetSize();i++) {
		if(pbMatchedTempl[i])
			continue;
		pAryTemplNotMatched->Add(pAryTempl->GetAt(i));
	}

	for(i=0;i<pAryInsp->GetSize();i++) {
		if(pbMatchedInsp[i])
			continue;
		pAryInspNotMatched->Add(pAryInsp->GetAt(i));
	}
}

catch(...) {
	OutputDebugLogTo(9,TRUE,"CPointArrayMatching::InspectByRegion");
	goto ErrorAbort;
}

	if(pdShiftX != NULL)
		delete [] pdShiftX;
	if(pdShiftY != NULL)
		delete [] pdShiftY;
	if(pptTempl != NULL)
		delete [] pptTempl;
	if(pbMatchedTempl != NULL)
		delete [] pbMatchedTempl;
	if(pbMatchedInsp != NULL)
		delete [] pbMatchedInsp;
	return 0;
ErrorAbort:
	if(pdShiftX != NULL)
		delete [] pdShiftX;
	if(pdShiftY != NULL)
		delete [] pdShiftY;
	if(pptTempl != NULL)
		delete [] pptTempl;
	if(pbMatchedTempl != NULL)
		delete [] pbMatchedTempl;
	if(pbMatchedInsp != NULL)
		delete [] pbMatchedInsp;
	return -1;

}



CSinCosTabAry::CSinCosTabAry()
{
	SinCosTabAry.RemoveAll();
	SinCosTab360Ary.RemoveAll();
	bSinCosTabCreated = FALSE;
}

CSinCosTabAry::~CSinCosTabAry()
{
	Clean();
}

void CSinCosTabAry::Clean()
{
	SinCosTabAry.RemoveAll();
	SinCosTab360Ary.RemoveAll();
	bSinCosTabCreated = FALSE;
}

int CSinCosTabAry::Create(double dAngleDegStep)
{
	int i;
	CPoint2D pt2DSinCosVal;
	int nTabSize;
	double dAngle;

try
{
	Clean();
	for(i=0;i<360;i++) {
		pt2DSinCosVal = CPoint2D(cos(i*PI/180),sin(i*PI/180));
		SinCosTab360Ary.Add(pt2DSinCosVal);
	}
	if(dAngleDegStep <= 0.0001)
		goto ErrorAbort;
	nTabSize = Round(360/dAngleDegStep+1);
	dAngle = 0;
	for(i=0;i<nTabSize;i++) {
		pt2DSinCosVal = CPoint2D(cos(dAngle*PI/180),sin(dAngle*PI/180));
		SinCosTabAry.Add(pt2DSinCosVal);
		dAngle += dAngleDegStep;
	}
	bSinCosTabCreated = TRUE;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CSinCosTabAry::Create");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

CImageUnwrapInfo::CImageUnwrapInfo()
{
	ImgUnwrap.Free();
	rcUnwrap = CRect(0,0,0,0);
	ppptMappingTab = NULL;
	nUnwrapWidth = 0;
	nUnwrapHeight = 0;
}

CImageUnwrapInfo::~CImageUnwrapInfo()
{
}

void CImageUnwrapInfo::Clean()
{
	if(ppptMappingTab != NULL) {
		for(int i=0;i<nUnwrapWidth;i++) {
			if(ppptMappingTab[i] != NULL)
				delete [] ppptMappingTab[i];
			ppptMappingTab[i] = NULL;
		}
		delete [] ppptMappingTab;
		ppptMappingTab = NULL;
	}
	ImgUnwrap.Free();
	rcUnwrap = CRect(0,0,0,0);
	nUnwrapWidth = 0;
	nUnwrapHeight = 0;
}
int CImageUnwrapInfo::WrapPosToUnwrapPos(CPoint* pptWrap,
									 CPoint* pptUnWrap)
{
	double dAngle;
	dAngleStep;
	double dDisToCtr;
	if(csSampling.cy <= 0)
		goto ErrroAbort;
	dAngle = atan2(pptWrap->y - pt2DCenterOrg.y, pptWrap->x - pt2DCenterOrg.x);
	if(dAngle < 0)
		dAngle += 2*PI;
	pptUnWrap->x = int(dAngle/dAngleStepSampling);
	if(pptUnWrap->x < 0)
		pptUnWrap->x = 0;
	if(pptUnWrap->x >= rcUnwrap.right)
		pptUnWrap->x = rcUnwrap.right - 1;
	dDisToCtr = sqrt((pptWrap->x - pt2DCenterOrg.x)*(pptWrap->x - pt2DCenterOrg.x) + (pptWrap->y - pt2DCenterOrg.y) * (pptWrap->y - pt2DCenterOrg.y));
	pptUnWrap->y = int(nOuterRadiusOrg - dDisToCtr)/csSampling.cy;
	return 0;
ErrroAbort:
	return -1;
}

int CImageUnwrapInfo::UnwrapRectToWrapRect(CRect* prcUnwrap, CRect* prcWrap)
{
	int i;
	CPoint ptUnwrapCorner[4];
	CPoint ptWrapCorner[4];
	int nError;
try
{
	ptUnwrapCorner[0] = CPoint(prcUnwrap->left, prcUnwrap->top); 
	ptUnwrapCorner[1] = CPoint(prcUnwrap->right, prcUnwrap->top); 
	ptUnwrapCorner[2] = CPoint(prcUnwrap->right, prcUnwrap->bottom); 
	ptUnwrapCorner[3] = CPoint(prcUnwrap->left, prcUnwrap->bottom); 
	for(i=0;i<4;i++) {
		nError = UnwrapPosToWrapPos(&ptUnwrapCorner[i], &ptWrapCorner[i]);
		if(nError)
			goto ErrorAbort;
	}
	nError = GetPointsBoundingRect(&ptWrapCorner[0], 
						  4,
						  prcWrap);
	if(nError)
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImageUnwrapInfo::UnwrapRectToWrapRect");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CImageUnwrapInfo::UnwrapPosToWrapPos(CPoint* pptUnWrap,
						 CPoint* pptWrap)
{
	double dAngle;
	dAngleStep;
	double dRadius;
	dAngle = pptUnWrap->x * dAngleStepSampling;
	dRadius = nOuterRadiusOrg - pptUnWrap->y * csSampling.cy;
	pptWrap->x = int(pt2DCenterOrg.x + dRadius * cos(dAngle));
	pptWrap->y = int(pt2DCenterOrg.y + dRadius * sin(dAngle));
	return 0;
}

int CImageUnwrapInfo::UnwrapPosToWrapPos(CPoint2D* ppt2DUnWrap,
						 CPoint2D* ppt2DWrap)
{
	double dAngle;
	dAngleStep;
	double dRadius;
	dAngle = ppt2DUnWrap->x * dAngleStepSampling;
	dRadius = nOuterRadiusOrg - ppt2DUnWrap->y * csSampling.cy;
	ppt2DWrap->x = pt2DCenterOrg.x + dRadius * cos(dAngle);
	ppt2DWrap->y = pt2DCenterOrg.y + dRadius * sin(dAngle);
	return 0;
}

int CImageUnwrapInfo::ImgWrap(CImageBuffer* pImgBufUnwrap,
							  CImageBuffer* pImgBufWrap)
{
	int i, j;
	CRect rcWrapBounding;
	int nWrapNotInImg;
	BYTE* pImgBufUnwrapPtr;
	BYTE* pImgBufWrapPtr;
	int nUnwrapPitch;
	int nWrapPitch;
	double dAngle;
	int nCenterX, nCenterY;
	int nRadiusInROITol;;
	int nSinAngleTimes10000;
	int nCosAngleTimes10000;
	long lAddressOffset;
	long lJumpX, lJumpY;
	long lxTimes10000, lyTimes10000;
	int nRadius;
	long lIndex;
	int nSamplingX, nSamplingY;
	int nImgWrapWidth, nImgWrapHeight;
	long lIndexWrap;
	CImageBuffer ImgMask;
	BYTE* pImgMaskPtr;
	int nx, ny;
try
{
	ImgMask.Allocate(AbsSize(pImgBufWrap->GetImgSize()));
	ImgMask.ClrImg(0);
	pImgMaskPtr = ImgMask.GetImgPtr();
	nImgWrapWidth = pImgBufWrap->GetImgSize().cx;
	nImgWrapHeight = pImgBufWrap->GetImgSize().cy;
	nSamplingX = csSampling.cx;
	nSamplingY = csSampling.cy;
	rcWrapBounding = CRect(Round(pt2DCenterOrg.x - nOuterRadiusOrg),
						Round(pt2DCenterOrg.y - nOuterRadiusOrg),
						Round(pt2DCenterOrg.x + nOuterRadiusOrg),
						Round(pt2DCenterOrg.y + nOuterRadiusOrg));
	rcWrapBounding.InflateRect(2,2);
	nWrapNotInImg = ValidateRect(&rcWrapBounding, &pImgBufWrap->GetImgSize());
	pImgBufUnwrapPtr = pImgBufUnwrap->GetImgPtr();
	pImgBufWrapPtr = pImgBufWrap->GetImgPtr();
	nUnwrapPitch = pImgBufUnwrap->GetPitch();
	nWrapPitch = pImgBufWrap->GetPitch();
	dAngle = 0;
	nCenterX = Round(pt2DCenterOrg.x);
	nCenterY = Round(pt2DCenterOrg.y);
	nRadiusInROITol = __min(nCenterX, nCenterY);
	nRadiusInROITol = __min(pImgBufWrap->GetImgSize().cx - nCenterX, nRadiusInROITol);
	nRadiusInROITol = __min(pImgBufWrap->GetImgSize().cy - nCenterY, nRadiusInROITol);
	nRadiusInROITol = nRadiusInROITol - 2;
	if(nWrapNotInImg == 0) {
		lAddressOffset = nCenterY * nWrapPitch + nCenterX;
		for(i=0;i<nUnwrapWidth;i++) {
			nSinAngleTimes10000 = Round(sin(dAngle) * 8192);
			nCosAngleTimes10000 = Round(cos(dAngle) * 8192);
			nRadius = nOuterRadiusOrg;
			lIndex = i; 
			lxTimes10000 = nRadius * nCosAngleTimes10000;
			lyTimes10000 = nRadius * nSinAngleTimes10000;
			lJumpX = nSamplingY * nCosAngleTimes10000;
			lJumpY = nSamplingY * nSinAngleTimes10000;
			for(j=0;j<nUnwrapHeight;j++) {
				lIndexWrap = (lyTimes10000>>13) * nWrapPitch + (lxTimes10000>>13) + lAddressOffset;
				pImgMaskPtr[lIndexWrap] = 255;
				pImgBufWrapPtr[lIndexWrap] = pImgBufUnwrapPtr[lIndex];
				lxTimes10000 -= lJumpX;
				lyTimes10000 -= lJumpY;
				lIndex += nUnwrapPitch;
			}
			dAngle += dAngleStepSampling;
		}
	}
	else {
		lAddressOffset = nCenterY * nWrapPitch + nCenterX;
		if((nRadiusInROITol + nUnwrapHeight) > nOuterRadiusOrg) {
			for(i=0;i<nUnwrapWidth;i++) {
				nSinAngleTimes10000 = Round(sin(dAngle) * 8192);
				nCosAngleTimes10000 = Round(cos(dAngle) * 8192);
				nRadius = nOuterRadiusOrg;
				lIndex = i; 
				for(j=0;j<(nOuterRadiusOrg-nRadiusInROITol);j++) {
					nx = nCenterX + (nRadius * nCosAngleTimes10000)/8192;
					ny = nCenterY + (nRadius * nSinAngleTimes10000)/8192;
					if((nx >= 0) && (nx < nImgWrapWidth) &&
					   (ny >= 0) && (ny < nImgWrapHeight)) {
						lIndexWrap = ny * nWrapPitch + nx;
						pImgMaskPtr[lIndexWrap] = 255;
						pImgBufWrapPtr[lIndexWrap] = pImgBufUnwrapPtr[lIndex];
					}
					nRadius = nRadius - nSamplingY;
					lIndex += nUnwrapPitch;
				}
				lxTimes10000 = nRadius * nCosAngleTimes10000;
				lyTimes10000 = nRadius * nSinAngleTimes10000;
				lJumpX = nSamplingY * nCosAngleTimes10000;
				lJumpY = nSamplingY * nSinAngleTimes10000;
				for(j=nOuterRadiusOrg-nRadiusInROITol;j<nUnwrapHeight;j++) {
					lIndexWrap = (lyTimes10000>>13) * nWrapPitch + (lxTimes10000>>13) + lAddressOffset;
					pImgMaskPtr[lIndexWrap] = 255;
					pImgBufWrapPtr[lIndexWrap] = pImgBufUnwrapPtr[lIndex];
					lxTimes10000 -= lJumpX;
					lyTimes10000 -= lJumpY;
					lIndex += nUnwrapPitch;
				}
				dAngle += dAngleStepSampling;
			}
		}
		else {
			for(i=0;i<nUnwrapWidth;i++) {
				nSinAngleTimes10000 = Round(sin(dAngle) * 8192);
				nCosAngleTimes10000 = Round(cos(dAngle) * 8192);
				nRadius = nOuterRadiusOrg;
				lIndex = i; 
				for(j=0;j<nUnwrapHeight;j++) {
					nx = nCenterX + (nRadius * nCosAngleTimes10000)/8192;
					ny = nCenterY + (nRadius * nSinAngleTimes10000)/8192;
					if((nx >= 0) && (nx < nImgWrapWidth) &&
					   (ny >= 0) && (ny < nImgWrapHeight)) {
						lIndexWrap = ny * nWrapPitch + nx;
						pImgMaskPtr[lIndexWrap] = 255;
						pImgBufWrapPtr[ny * nWrapPitch + nx] = pImgBufUnwrapPtr[lIndex];
					}
					lIndex += nUnwrapPitch;
					nRadius = nRadius - nSamplingY;
				}
				dAngle += dAngleStepSampling;
			}
		}
	}
	lIndex = 0;
	for(i=0;i<pImgBufWrap->GetImgSize().cy;i++) {
		for(j=1;j<pImgBufWrap->GetImgSize().cx;j++) {
			if((pImgMaskPtr[lIndex+j-1] == 255) && (pImgMaskPtr[lIndex+j] == 0))
				pImgBufWrapPtr[lIndex+j] = pImgBufWrapPtr[lIndex+j-1];
		}
		lIndex += nWrapPitch;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImageUnwrapInfo::ImgWrap");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}



int CImageUnwrapInfo::ImgBinUnwrap(IMAGEVIEW_INFO* pView, 
					 CImageBuffer* pImgBufSrc,
					 CPoint2D* ppt2DCenter,
					 int nInnerRadius,
					 int nOuterRadius,
					 int nBlobColor,
					 CSize* pcsSampling,
					 long lDebugFlag,
					 int nProcMode)
{
	int nError;
try
{
	if(nProcMode == 0) {
		nError = ImgBinUnwrapWithInterpolation(pView, 
						 pImgBufSrc,
						 ppt2DCenter,
						 nInnerRadius,
						 nOuterRadius,
						 nBlobColor,
						 pcsSampling,
						 lDebugFlag);
		if(nError)
			goto ErrorAbort;
	}
	else {
		nError = ImgBinUnwrapWithoutInterpolation(pView, 
						 pImgBufSrc,
						 ppt2DCenter,
						 nInnerRadius,
						 nOuterRadius,
						 nBlobColor,
						 pcsSampling,
						 lDebugFlag);
		if(nError)
			goto ErrorAbort;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImageUnwrapInfo::ImgBinUnwrap");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CImageUnwrapInfo::ImgBinUnwrapWithoutInterpolation(IMAGEVIEW_INFO* pView, 
					 CImageBuffer* pImgBufSrc,
					 CPoint2D* ppt2DCenter,
					 int nInnerRadius,
					 int nOuterRadius,
					 int nBlobColor,
					 CSize* pcsSampling,
					 long lDebugFlag)
{

	int i, j;
	double dAngle;
	BYTE* pImgUnwrapPtr;
	BYTE* pImgBufSrcPtr;
	int nSrcPitch;
	int nCenterX, nCenterY;
	int nSinAngleTimes10000, nCosAngleTimes10000;
	int nx, ny;
	int nUnWrapPitch;
	int nRadius;
	int nWrapNotInImg;
	CRect rcWrapBounding;
	int nImgWrapWidth, nImgWrapHeight;
	int nSamplingX;
	int nSamplingY;
	long lAddressOffset;
	int nRadiusInROITol;
	long lxTimes10000, lyTimes10000;
	long lJumpX;
	long lJumpY;
	long lIndex;
try
{
	Clean();
	csSampling = *pcsSampling;
	nSamplingX = csSampling.cx;
	nSamplingY = csSampling.cy;
	if((pcsSampling->cx <= 0) || (pcsSampling->cy <= 0))
		goto ErrorAbort;
	if((nInnerRadius < 0) || (nInnerRadius > nOuterRadius))
		goto ErrorAbort;
	pt2DCenterOrg = *ppt2DCenter;
	nInnerRadiusOrg = nInnerRadius;
	nOuterRadiusOrg = nOuterRadius;

	nImgWrapWidth = pImgBufSrc->GetImgSize().cx;
	nImgWrapHeight = pImgBufSrc->GetImgSize().cy;
	rcWrapBounding = CRect(Round(ppt2DCenter->x - nOuterRadius),
						Round(ppt2DCenter->y - nOuterRadius),
						Round(ppt2DCenter->x + nOuterRadius),
						Round(ppt2DCenter->y + nOuterRadius));
	rcWrapBounding.InflateRect(2,2);
	nWrapNotInImg = ValidateRect(&rcWrapBounding, &pImgBufSrc->GetImgSize());
	nUnwrapHeight = (nOuterRadius - nInnerRadius + 1)/csSampling.cy;
	nUnwrapWidth = (Round(2*PI*nOuterRadius) + 1)/csSampling.cx;
	dAngleStep = 1.0/nOuterRadius;
	dAngleStepSampling = dAngleStep * nSamplingX;
	rcUnwrap = CRect(CPoint(0,0), CSize(nUnwrapWidth, nUnwrapHeight));
	ImgUnwrap.Allocate(AbsSize(rcUnwrap.Size()));
	//ImgUnwrap.ClrImg(0);
	pImgUnwrapPtr = ImgUnwrap.GetImgPtr();
	pImgBufSrcPtr = pImgBufSrc->GetImgPtr();
	nSrcPitch = pImgBufSrc->GetPitch();
	nUnWrapPitch = ImgUnwrap.GetPitch();
	dAngle = 0;
	nCenterX = Round(ppt2DCenter->x);
	nCenterY = Round(ppt2DCenter->y);

	nRadiusInROITol = __min(nCenterX, nCenterY);
	nRadiusInROITol = __min(pImgBufSrc->GetImgSize().cx - nCenterX, nRadiusInROITol);
	nRadiusInROITol = __min(pImgBufSrc->GetImgSize().cy - nCenterY, nRadiusInROITol);
	nRadiusInROITol = nRadiusInROITol - 2;

	if(nBlobColor == WHITE) 
		ImgUnwrap.ClrImg(0);
	else
		ImgUnwrap.ClrImg(255);
	if(nWrapNotInImg == 0) {
		lAddressOffset = nCenterY * nSrcPitch + nCenterX;
		for(i=0;i<nUnwrapWidth;i++) {
			nSinAngleTimes10000 = Round(sin(dAngle) * 8192);
			nCosAngleTimes10000 = Round(cos(dAngle) * 8192);
			nRadius = nOuterRadius;
			lIndex = i; 
			lxTimes10000 = nOuterRadius * nCosAngleTimes10000;
			lyTimes10000 = nOuterRadius * nSinAngleTimes10000;
			lJumpX = nSamplingY * nCosAngleTimes10000;
			lJumpY = nSamplingY * nSinAngleTimes10000;
			for(j=0;j<nUnwrapHeight;j++) {
				pImgUnwrapPtr[lIndex] = pImgBufSrcPtr[(lyTimes10000>>13) * nSrcPitch + (lxTimes10000>>13) + lAddressOffset];
				lxTimes10000 -= lJumpX;
				lyTimes10000 -= lJumpY;
				lIndex += nUnWrapPitch;
			}
			dAngle += dAngleStepSampling;
		}
	}
	else {
		//if(nBlobColor == WHITE) {
			lAddressOffset = nCenterY * nSrcPitch + nCenterX;
			if((nRadiusInROITol + nUnwrapHeight) > nOuterRadius) {
				for(i=0;i<nUnwrapWidth;i++) {
					nSinAngleTimes10000 = Round(sin(dAngle) * 8192);
					nCosAngleTimes10000 = Round(cos(dAngle) * 8192);
					nRadius = nOuterRadius;
					lIndex = i; 
					for(j=0;j<(nOuterRadius-nRadiusInROITol);j++) {
						nx = nCenterX + (nRadius * nCosAngleTimes10000)/8192;
						ny = nCenterY + (nRadius * nSinAngleTimes10000)/8192;
						if((nx >= 0) && (ny >= 0) && (nx < nImgWrapWidth) && (ny < nImgWrapHeight))
							pImgUnwrapPtr[lIndex] = pImgBufSrcPtr[ny * nSrcPitch + nx];
						nRadius = nRadius - nSamplingY;
						lIndex += nUnWrapPitch;
					}
					lxTimes10000 = nRadius * nCosAngleTimes10000;
					lyTimes10000 = nRadius * nSinAngleTimes10000;
					lJumpX = nSamplingY * nCosAngleTimes10000;
					lJumpY = nSamplingY * nSinAngleTimes10000;
					for(j=nOuterRadius-nRadiusInROITol;j<nUnwrapHeight;j++) {
						pImgUnwrapPtr[lIndex] = pImgBufSrcPtr[(lyTimes10000>>13) * nSrcPitch + (lxTimes10000>>13) + lAddressOffset];
						lxTimes10000 -= lJumpX;
						lyTimes10000 -= lJumpY;
						lIndex += nUnWrapPitch;
					}
					dAngle += dAngleStepSampling;
				}
			}
			else {
				for(i=0;i<nUnwrapWidth;i++) {
					nSinAngleTimes10000 = Round(sin(dAngle) * 8192);
					nCosAngleTimes10000 = Round(cos(dAngle) * 8192);
					nRadius = nOuterRadius;
					for(j=0;j<nUnwrapHeight;j++) {
						nx = nCenterX + (nRadius * nCosAngleTimes10000)/8192;
						ny = nCenterY + (nRadius * nSinAngleTimes10000)/8192;
					if((nx >= 0) && (ny >= 0) && (nx < nImgWrapWidth) && (ny < nImgWrapHeight))
							pImgUnwrapPtr[j* nUnWrapPitch + i] = pImgBufSrcPtr[ny * nSrcPitch + nx];
						nRadius = nRadius - nSamplingY;
					}
					dAngle += dAngleStepSampling;
				//}
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImageUnwrapInfo::ImgBinUnwrapWithoutInterpolation");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CImageUnwrapInfo::ImgBinUnwrapWithInterpolation(IMAGEVIEW_INFO* pView, 
					 CImageBuffer* pImgBufSrc,
					 CPoint2D* ppt2DCenter,
					 int nInnerRadius,
					 int nOuterRadius,
					 int nBlobColor,
					 CSize* pcsSampling,
					 long lDebugFlag)
{
	int i, j;
	double dAngle;
	BYTE* pImgUnwrapPtr;
	BYTE* pImgBufSrcPtr;
	int nSrcPitch;
	int nCenterX, nCenterY;
	int nSinAngleTimes10000, nCosAngleTimes10000;
	int nx, ny;
	int nUnWrapPitch;
	int nRadius;
	int nWrapNotInImg;
	CRect rcWrapBounding;
	int nImgWrapWidth, nImgWrapHeight;
	int nSamplingX;
	int nSamplingY;
	int nRadiusInROITol;
	long lxTimes10000, lyTimes10000;
	long lJumpX;
	long lJumpY;
	long lIndex;
	long lIndex0;
	int nX0, nY0;
	int nVal0, nVal1;
	int nCenterY10000;
	int nCenterX10000;
	int nImgWrapWidthSub1, nImgWrapHeightSub1;
try
{
	Clean();
	csSampling = *pcsSampling;
	nSamplingX = csSampling.cx;
	nSamplingY = csSampling.cy;
	if((pcsSampling->cx <= 0) || (pcsSampling->cy <= 0))
		goto ErrorAbort;
	if((nInnerRadius < 0) || (nInnerRadius > nOuterRadius))
		goto ErrorAbort;
	pt2DCenterOrg = *ppt2DCenter;
	nInnerRadiusOrg = nInnerRadius;
	nOuterRadiusOrg = nOuterRadius;

	nImgWrapWidth = pImgBufSrc->GetImgSize().cx;
	nImgWrapHeight = pImgBufSrc->GetImgSize().cy;
	rcWrapBounding = CRect(Round(ppt2DCenter->x - nOuterRadius),
						Round(ppt2DCenter->y - nOuterRadius),
						Round(ppt2DCenter->x + nOuterRadius),
						Round(ppt2DCenter->y + nOuterRadius));
	rcWrapBounding.InflateRect(3,3);
	nWrapNotInImg = ValidateRect(&rcWrapBounding, &pImgBufSrc->GetImgSize());
	nUnwrapHeight = (nOuterRadius - nInnerRadius + 1)/csSampling.cy;
	nUnwrapWidth = (Round(2*PI*nOuterRadius) + 1)/csSampling.cx;
	dAngleStep = 1.0/nOuterRadius;
	dAngleStepSampling = dAngleStep * nSamplingX;
	rcUnwrap = CRect(CPoint(0,0), CSize(nUnwrapWidth, nUnwrapHeight));
	ImgUnwrap.Allocate(AbsSize(rcUnwrap.Size()));
	ImgUnwrap.ClrImg(0);
	pImgUnwrapPtr = ImgUnwrap.GetImgPtr();
	pImgBufSrcPtr = pImgBufSrc->GetImgPtr();
	nSrcPitch = pImgBufSrc->GetPitch();
	nUnWrapPitch = ImgUnwrap.GetPitch();
	dAngle = 0;
	nCenterX = Round(ppt2DCenter->x);
	nCenterY = Round(ppt2DCenter->y);

	nRadiusInROITol = __min(nCenterX, nCenterY);
	nRadiusInROITol = __min(pImgBufSrc->GetImgSize().cx - nCenterX, nRadiusInROITol);
	nRadiusInROITol = __min(pImgBufSrc->GetImgSize().cy - nCenterY, nRadiusInROITol);
	nRadiusInROITol = nRadiusInROITol - 2;
	nCenterX10000 = nCenterX * 8192;
	nCenterY10000 = nCenterY * 8192;
	nImgWrapWidthSub1 = nImgWrapWidth - 1;
	nImgWrapHeightSub1 = nImgWrapHeight - 1;
	if(nWrapNotInImg == 0) {
		for(i=0;i<nUnwrapWidth;i++) {
			nSinAngleTimes10000 = Round(sin(dAngle) * 8192);
			nCosAngleTimes10000 = Round(cos(dAngle) * 8192);
			nRadius = nOuterRadius;
			lIndex = i; 
			lxTimes10000 = nOuterRadius * nCosAngleTimes10000 + nCenterX10000;
			lyTimes10000 = nOuterRadius * nSinAngleTimes10000 + nCenterY10000;
			lJumpX = nSamplingX * nCosAngleTimes10000;
			lJumpY = nSamplingY * nSinAngleTimes10000;
			for(j=0;j<nUnwrapHeight;j++) {
				nX0 = (lxTimes10000>>10)%8;
				nY0 = (lyTimes10000>>10)%8;
				lIndex0 = (lyTimes10000>>13) * nSrcPitch + (lxTimes10000>>13);
				nVal0 = pImgBufSrcPtr[lIndex0] + (((pImgBufSrcPtr[lIndex0+1] - pImgBufSrcPtr[lIndex0])*nX0)>>3);
				nVal1 = pImgBufSrcPtr[lIndex0+nSrcPitch] + (((pImgBufSrcPtr[lIndex0+nSrcPitch+1] - pImgBufSrcPtr[lIndex0+nSrcPitch])*nX0)>>3);
				pImgUnwrapPtr[lIndex] = nVal0 +(((nVal1 - nVal0) * nY0)>>3);
				lxTimes10000 -= lJumpX;
				lyTimes10000 -= lJumpY;
				lIndex += nUnWrapPitch;
			}
			dAngle += dAngleStepSampling;
		}
	}
	else {
		if(nBlobColor == WHITE) {
			if((nRadiusInROITol + nUnwrapHeight) > nOuterRadius) {
				for(i=0;i<nUnwrapWidth;i++) {
					nSinAngleTimes10000 = Round(sin(dAngle) * 8192);
					nCosAngleTimes10000 = Round(cos(dAngle) * 8192);
					nRadius = nOuterRadius;
					lIndex = i; 
					lxTimes10000 = nOuterRadius * nCosAngleTimes10000 + nCenterX10000;
					lyTimes10000 = nOuterRadius * nSinAngleTimes10000 + nCenterY10000;
					lJumpX = nSamplingX * nCosAngleTimes10000;
					lJumpY = nSamplingY * nSinAngleTimes10000;
					for(j=0;j<nUnwrapHeight;j++) {
						nX0 = (lxTimes10000>>10)%8;
						nY0 = (lyTimes10000>>10)%8;
						nx = (lxTimes10000>>13);
						ny = (lyTimes10000>>13);
						if((nx < 0) || (ny < 0) || (nx >= nImgWrapWidthSub1) || (ny >= nImgWrapHeightSub1))
							pImgUnwrapPtr[lIndex] = 0;
						else {
							lIndex0 = ny * nSrcPitch + nx;
							nVal0 = pImgBufSrcPtr[lIndex0] + (((pImgBufSrcPtr[lIndex0+1] - pImgBufSrcPtr[lIndex0])*nX0)>>3);
							nVal1 = pImgBufSrcPtr[lIndex0+nSrcPitch] + (((pImgBufSrcPtr[lIndex0+nSrcPitch+1] - pImgBufSrcPtr[lIndex0+nSrcPitch])*nX0)>>3);
							pImgUnwrapPtr[lIndex] = nVal0 +(((nVal1 - nVal0) * nY0)>>3);
						}
						lxTimes10000 -= lJumpX;
						lyTimes10000 -= lJumpY;
						lIndex += nUnWrapPitch;
					}
					dAngle += dAngleStepSampling;
				}
			}
			else {
				for(i=0;i<nUnwrapWidth;i++) {
					nSinAngleTimes10000 = Round(sin(dAngle) * 8192);
					nCosAngleTimes10000 = Round(cos(dAngle) * 8192);
					nRadius = nOuterRadius;
					lIndex = i; 
					lxTimes10000 = nOuterRadius * nCosAngleTimes10000 + nCenterX10000;
					lyTimes10000 = nOuterRadius * nSinAngleTimes10000 + nCenterY10000;
					lJumpX = nSamplingX * nCosAngleTimes10000;
					lJumpY = nSamplingY * nSinAngleTimes10000;
					for(j=0;j<nUnwrapHeight;j++) {
						nX0 = (lxTimes10000>>10)%8;
						nY0 = (lyTimes10000>>10)%8;
						nx = (lxTimes10000>>13);
						ny = (lyTimes10000>>13);
						if((nx < 0) || (ny < 0) || (nx >= nImgWrapWidthSub1) || (ny >= nImgWrapHeightSub1))
							pImgUnwrapPtr[lIndex] = 255;
						else {
							lIndex0 = ny * nSrcPitch + nx;
							nVal0 = pImgBufSrcPtr[lIndex0] + (((pImgBufSrcPtr[lIndex0+1] - pImgBufSrcPtr[lIndex0])*nX0)>>3);
							nVal1 = pImgBufSrcPtr[lIndex0+nSrcPitch] + (((pImgBufSrcPtr[lIndex0+nSrcPitch+1] - pImgBufSrcPtr[lIndex0+nSrcPitch])*nX0)>>3);
							pImgUnwrapPtr[lIndex] = nVal0 +(((nVal1 - nVal0) * nY0)>>3);
						}
						lxTimes10000 -= lJumpX;
						lyTimes10000 -= lJumpY;
						lIndex += nUnWrapPitch;
					}
					dAngle += dAngleStepSampling;
				}
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImageUnwrapInfo::ImgBinUnwrapWithInterpolation");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


CImageEdgeEnhance::CImageEdgeEnhance()
{
	m_nContrastMaxTol = 30;
	m_nContrastMinTol = 3;
	m_nMagnifyTimes = 3;
}


CImageEdgeEnhance::~CImageEdgeEnhance()
{
}

int CImageEdgeEnhance::Inspect(IMAGEVIEW_INFO *pView,
				 CImageBuffer *pImgBufSrc,
				 CRect* prcSrc,
				 CImageBuffer *pImgBufDst,
				 CRect* prcDst,
				 long lDebugFlag)
{
	CImageBuffer ImgContrast;
	CImageBuffer ImgContrastAdj;
    CSize csKernel;
	long lHist[256];
	CHistogram Histogram;
	double dPercentageStart;
	double dPercentageEnd;
	int nAvgBkgContrast;
	CImageBuffer ImgMaxContrastMask;
	CImageBuffer ImgMaxContrastMaskDilation;
	CImageBuffer ImgAdjContrastMask;
	int nDilation;
	CSize csDilation;
	int nError;
try
{	
	nDilation = 7;
	csKernel = CSize(3,3);
	ImgContrast.Allocate(AbsSize(pImgBufSrc->GetImgSize()));
	ImgMaxContrastMask.Allocate(AbsSize(pImgBufSrc->GetImgSize()));
	ImgMaxContrastMaskDilation.Allocate(AbsSize(pImgBufSrc->GetImgSize()));
	ImgAdjContrastMask.Allocate(AbsSize(pImgBufSrc->GetImgSize()));
	ImgContrastAdj.Allocate(AbsSize(pImgBufSrc->GetImgSize()));
	nError = GetContrastImg(pImgBufSrc,
				   prcSrc,
				   &csKernel,
				   EROSION,
				   &ImgContrast,
				   prcDst);
	if(nError)
		goto ErrorAbort;
	nError = GetHist(&ImgContrast, prcDst, &lHist[0]);
	if(nError)
		goto ErrorAbort;
	dPercentageStart = 10;
	dPercentageEnd = 80;
	nError = Histogram.GetAvgIntensity(dPercentageStart,
								dPercentageEnd,
								&lHist[0], 
								&nAvgBkgContrast); 
	if(nError)
		goto ErrorAbort;
	nMaxThreshold = m_nContrastMaxTol + nAvgBkgContrast;
	nMinThreshold = m_nContrastMinTol + nAvgBkgContrast;
	nError = Binarise(&ImgContrast,
				   prcDst,
				   &ImgMaxContrastMask,
				   prcDst,
				   nMaxThreshold);
	if(nError)
		goto ErrorAbort;
	csDilation = CSize(nDilation, nDilation);
	nError = ImgBinDilation(&ImgMaxContrastMask,
				 prcDst,
				 WHITE,
				 &ImgMaxContrastMaskDilation,
				 prcDst,
				 &csDilation);
	if(nError)
		goto ErrorAbort;
	nError = Binarise(&ImgContrast,
				   prcDst,
				   &ImgAdjContrastMask,
				   prcDst,
				   nMinThreshold);
	if(nError)
		goto ErrorAbort;
	nError = ImgSubtract(&ImgAdjContrastMask,
				   prcDst,
				   &ImgMaxContrastMaskDilation,
				   prcDst,
				   &ImgAdjContrastMask,
				   prcDst);
	if(nError)
		goto ErrorAbort;
	nError = ImgAND(&ImgAdjContrastMask,
				   prcDst,
				   &ImgContrast,
				   prcDst,
				   &ImgContrastAdj,
				   prcDst);
	if(nError)
		goto ErrorAbort;
	nError = ImgMagnify(&ImgContrastAdj,
				   prcDst,
				   &ImgContrastAdj,
				   prcDst,
				   m_nMagnifyTimes,
				   nMaxThreshold);
	if(nError)
		goto ErrorAbort;

	nError = ImgADD(&ImgContrastAdj,
				   prcDst,
				   pImgBufSrc,
				   prcSrc,
				   pImgBufDst,
				   prcDst);
	if(nError)
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImageEdgeEnhance::Inspect");
	goto ErrorAbort;
}
	return 0;
ErrorAbort : 
	return -1;
}

int CImageEdgeEnhance::ImgMagnify(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			CImageBuffer *pImgDst,
			CRect *prcDst,
			int nTimes,
			int nMaxThreshold)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	int nVal;
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
			if(*pColSrc > 0) {
				nVal = *pColSrc * nTimes;
				if(nVal > nMaxThreshold)
					*pColDst = nMaxThreshold;
				else
					*pColDst = nVal;
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImageEdgeEnhance::ImgMagnify");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}
CCircleInfo::CCircleInfo()
{
	pnRadiusSqr = NULL;
	nRadiusNo = 0;
}

CCircleInfo::~CCircleInfo()
{
	Clean();
}

void CCircleInfo::Clean()
{
	if(pnRadiusSqr != NULL)
		delete [] pnRadiusSqr;
	pnRadiusSqr = NULL;
	nRadiusNo = 0;
}


int CCircleInfo::Inspect(CPoint* pptContour,
				int nPtContourNo,
				int nProcMode,
				int nAngleMin,
				int nAngleMax)
{
	int i;
	int nHistMax;
	int nHist;
	int nCenterIndex;
	int nrcBoundingCenterX;
	int nrcBoundingCenterY;
	CPoint2D pt2DRoughCenter;
	CPoint2D pt2DCenterRelative;
	double dAngle;
	double dx, dy;
	double dRatio;
	CBlob1D Blob1D;
	dRatio = 180/PI;
	int nError;

try {
	nAngleDifInsp = nAngleMin;
	if(nPtContourNo < 3)
		goto ErrorAbort;
	nError = GetPointsBoundingRect(&pptContour[0], 
						  nPtContourNo,
						  &rcBounding);
	if(nError)
				goto ErrorAbort;
	pt2DCenter = rcBounding.CenterPoint();
	dRadius = (rcBounding.Width() + rcBounding.Height())/4.0;
	pt2DRoughCenter = CPoint2D((rcBounding.left + rcBounding.right)/2.0,
							   (rcBounding.top + rcBounding.bottom)/2.0);
	nEstimatedRadius = (rcBounding.Width() + rcBounding.Height())/4;
	nEdgeProcMode = nProcMode;
	rcBoundingRefCenter = rcBounding - rcBounding.CenterPoint();
	nError = SegmentEdge(&pptContour[0],
					nPtContourNo,
					&pt2DRoughCenter);
	if(nError)
				goto ErrorAbort;
	nError = FindCircleCenter4(nAngleMin, nAngleMax);
	if(nError)
		 goto ErrorAbort;

	nHistMax = 0;
	nCenterIndex = 0;
	if(nCenterAryNo <= 0)
		goto ErrorAbort;
	for(i=0;i<nCenterAryNo;i++) {
		nError = FindCircleRadius(&pt2DCenterAry[i],
							&nHist);
		if(nError)
			goto ErrorAbort;
		if(nHistMax < nHist) {
			nHistMax = nHist;
			nCenterIndex = i;
		}
	}
	pt2DCenter = pt2DCenterAry[nCenterIndex];
	if(nCenterAryNo > 1) {
		nError = FindCircleRadius(&pt2DCenter,
							&nHist);
		if(nError)
				goto ErrorAbort;
	}

	nrcBoundingCenterX = rcBounding.CenterPoint().x;
	nrcBoundingCenterY = rcBounding.CenterPoint().y;
	//pt2DRoughCenter = CPoint2D(pt2DCenter.x + nrcBoundingCenterX, pt2DCenter.y + nrcBoundingCenterY);
	//nError = SegmentEdge(&pptContour[0],
	//				nPtContourNo,
	//				&pt2DRoughCenter);
	//if(nError)
	//	goto ErrorAbort;
	nError = EdgeInterpolationForInvalidPoint();
	if(nError)
		goto ErrorAbort;
	pt2DCenterRelative = pt2DCenter;
	pt2DCenter = CPoint2D(pt2DCenter.x + nrcBoundingCenterX, pt2DCenter.y + nrcBoundingCenterY);
	for(i=0;i<360;i++) {
		if(pbSegmentEdge[i] == FALSE) {
			dAngle = i*PI/180;
			ptSegmentEdge[i] = CPoint(Round(dSegmentRadius[i]*cos(dAngle) + pt2DCenterRelative.x), Round(dSegmentRadius[i]*sin(dAngle)+pt2DCenterRelative.y)); 
		}
		
		else {
			dx = ptSegmentEdge[i].x - pt2DCenterRelative.x;
			dy = ptSegmentEdge[i].y - pt2DCenterRelative.y;
		}
		ptSegmentEdge[i] = CPoint(ptSegmentEdge[i].x + nrcBoundingCenterX, 
			ptSegmentEdge[i].y + nrcBoundingCenterY);
	}
	nError = Blob1D.Dilation(&pbSegmentEdge[0],
					360,
					1);
	nError = Blob1D.Erosion(&pbSegmentEdge[0],
					360,
					1);

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::Inspect");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CCircleInfo::Inspect2(CPoint* pptContour,
				int nPtContourNo,
				CPoint* pptEstimatedCenter,
				double dEstimatedRadius,
				int nProcMode,
				int nAngleMin,
				int nAngleMax)
{
	int i;
	int nHistMax;
	int nHist;
	int nCenterIndex;
	int nrcBoundingCenterX;
	int nrcBoundingCenterY;
	CPoint2D pt2DRoughCenter;
	CPoint2D pt2DCenterRelative;
	double dAngle;
	CBlob1D Blob1D;
	int nError;
try
{
	nAngleDifInsp = nAngleMin;
	if(nPtContourNo < 3)
		goto ErrorAbort;
	nError = GetPointsBoundingRect(&pptContour[0], 
						  nPtContourNo,
						  &rcBounding);
	pt2DCenter = *pptEstimatedCenter;
	dRadius = dEstimatedRadius;
	pt2DRoughCenter = pt2DCenter;
	nEstimatedRadius = (int) dEstimatedRadius;
	nEdgeProcMode = nProcMode;
	rcBoundingRefCenter = rcBounding - rcBounding.CenterPoint();
	nError = SegmentEdge(&pptContour[0],
					nPtContourNo,
					&pt2DRoughCenter);
 
	nError = FindCircleCenter4(nAngleMin, nAngleMax);

	nHistMax = 0;
	nCenterIndex = 0;
	for(i=0;i<nCenterAryNo;i++) {
		nError = FindCircleRadius(&pt2DCenterAry[i],
							&nHist);
		if(nHistMax < nHist) {
			nHistMax = nHist;
			nCenterIndex = i;
		}
	}
	pt2DCenter = pt2DCenterAry[nCenterIndex];
	if(nCenterAryNo > 1) {
		nError = FindCircleRadius(&pt2DCenter,
							&nHist);
	}

	nrcBoundingCenterX = (int) pt2DRoughCenter.x;
	nrcBoundingCenterY = (int) pt2DRoughCenter.y;
	//pt2DRoughCenter = CPoint2D(pt2DCenter.x + nrcBoundingCenterX, pt2DCenter.y + nrcBoundingCenterY);
	//nError = SegmentEdge(&pptContour[0],
	//				nPtContourNo,
	//				&pt2DRoughCenter);
	//if(nError)
	//	goto ErrorAbort;
	nError = EdgeInterpolationForInvalidPoint();
	pt2DCenterRelative = pt2DCenter;
	pt2DCenter = CPoint2D(pt2DCenter.x + nrcBoundingCenterX, pt2DCenter.y + nrcBoundingCenterY);
	for(i=0;i<360;i++) {
		if(pbSegmentEdge[i] == FALSE) {
			dAngle = i*PI/180;
			ptSegmentEdge[i] = CPoint(Round(dSegmentRadius[i]*cos(dAngle) + pt2DCenterRelative.x), Round(dSegmentRadius[i]*sin(dAngle)+pt2DCenterRelative.y)); 
		}
		ptSegmentEdge[i] = CPoint(ptSegmentEdge[i].x + nrcBoundingCenterX, 
								ptSegmentEdge[i].y + nrcBoundingCenterY);
	}
	nError = Blob1D.Dilation(&pbSegmentEdge[0],
					360,
					1);
	nError = Blob1D.Erosion(&pbSegmentEdge[0],
					360,
					1);

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::Inspect2");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CCircleInfo::GetSegmentPtsInRing(double dRadiusDifPercentage,
								CPoint2D* ppt2DMask)
{
	int i;
	double dMinRadius;
	double dMaxRadius;
	double dAngle;
	double dRadius0; 

	try {

	dMinRadius = dRadius * (100-dRadiusDifPercentage)/100.0;
	dMaxRadius = dRadius * (100+dRadiusDifPercentage)/100.0;

	for(i=0;i<360;i++)
	{
		dRadius0 = dSegmentRadius[i];
		if((dRadius0 > dMinRadius) && (dRadius0 < dMaxRadius))
		{
			ppt2DMask[i] = ptSegmentEdge[i];
			continue;
		}
		if(dRadius0 > dMaxRadius)
		{
			dRadius0 = dMaxRadius;
		}
		if(dRadius0 < dMinRadius)
		{
			dRadius0 = dMinRadius;
		}
		dAngle = i * PI/180;
		ppt2DMask[i].x = dRadius0 * cos(dAngle) + pt2DCenter.x;
		ppt2DMask[i].y = dRadius0 * cos(dAngle) + pt2DCenter.y;
	}
	}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::GetSegmentPtsInRing");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CCircleInfo::GetRadiusvar(CPoint* pptContour,
				int nPtContourNo,
				double dStartPercentage,
				double dEndPercentage,
				double* pdRadiusMaxVar)
{
	int i;
	double* pdVar;
	double dRadiusInsp;
	int nStartIndex;
	int nEndIndex;
	double dSum;
	int nWeight;

try {
	pdVar = NULL;
	*pdRadiusMaxVar = 0;
	if(nPtContourNo < 3)
		goto ErrorAbort;
	pdVar = new double[abs(nPtContourNo)];
	for(i=0;i<nPtContourNo;i++) {
		dRadiusInsp = sqrt((pptContour[i].x - pt2DCenter.x) * (pptContour[i].x - pt2DCenter.x) + 
						   (pptContour[i].y - pt2DCenter.y) * (pptContour[i].y - pt2DCenter.y));
		pdVar[i] = fabs(dRadiusInsp - dRadius);
	}
	Sorting(pdVar, nPtContourNo, TRUE);
	if((dStartPercentage <= -0.00001) || (dStartPercentage > 100.00001) ||
	   (dEndPercentage <= -0.00001) || (dEndPercentage > 100.00001) ||
	   (dStartPercentage >=  dEndPercentage))
	   goto ErrorAbort;
	nStartIndex = Round(nPtContourNo * dStartPercentage/100);
	nEndIndex = Round(nPtContourNo * dEndPercentage/100);
	if(nStartIndex >= nPtContourNo) { 
		nStartIndex = nPtContourNo - 1;
		nEndIndex = nPtContourNo;
	}
	else {
		if(nStartIndex >= nEndIndex) {
			nEndIndex = nStartIndex + 1;
		}
	}
	dSum = 0;
	nWeight = 0;
	for(i=nStartIndex;i<nEndIndex;i++) {
		dSum += pdVar[i];
		nWeight++;
	}
	if(nWeight > 0) 
		*pdRadiusMaxVar = dSum/nWeight;

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::GetRadiusvar");
	goto ErrorAbort;
}
	if(pdVar != NULL)
		delete [] pdVar;
	return 0;
ErrorAbort:
	if(pdVar != NULL)
		delete [] pdVar;
	return -1;
}


int CCircleInfo::Inspect(CPoint* pptContour,
				int nPtContourNo,
				CPoint* pptEstimatedCenter,
				double dEstimatedRadius,
				int nProcMode,
				int nAngleMin,
				int nAngleMax)
{
	int i;
	int nHistMax;
	int nHist;
	int nCenterIndex;
	int nrcBoundingCenterX;
	int nrcBoundingCenterY;
	CPoint2D pt2DRoughCenter;
	CPoint2D pt2DCenterRelative;
	double dAngle;
	CBlob1D Blob1D;
	int nError;

try {
	nAngleDifInsp = nAngleMin;
	if(nPtContourNo < 3)
		goto ErrorAbort;
	nError = GetPointsBoundingRect(&pptContour[0], 
						  nPtContourNo,
						  &rcBounding);
	if(nError)
		goto ErrorAbort;
	pt2DCenter = *pptEstimatedCenter;
	dRadius = dEstimatedRadius;
	pt2DRoughCenter = pt2DCenter;
	nEstimatedRadius = (int) dEstimatedRadius;
	nEdgeProcMode = nProcMode;
	rcBoundingRefCenter = rcBounding - rcBounding.CenterPoint();
	nError = SegmentEdge(&pptContour[0],
					nPtContourNo,
					&pt2DRoughCenter);
	if(nError)
		goto ErrorAbort;
 
	nError = FindCircleCenter4(nAngleMin, nAngleMax);
	if(nError)
		goto ErrorAbort;

	nHistMax = 0;
	nCenterIndex = 0;
	for(i=0;i<nCenterAryNo;i++) {
		nError = FindCircleRadius(&pt2DCenterAry[i],
							&nHist);
		if(nError)
				continue;
		if(nHistMax < nHist) {
			nHistMax = nHist;
			nCenterIndex = i;
		}
	}
	pt2DCenter = pt2DCenterAry[nCenterIndex];
	if(nCenterAryNo > 1) {
		nError = FindCircleRadius(&pt2DCenter,
							&nHist);
		if(nError)
				goto ErrorAbort;
	}

	nrcBoundingCenterX = (int) pt2DRoughCenter.x;
	nrcBoundingCenterY = (int) pt2DRoughCenter.y;
	//pt2DRoughCenter = CPoint2D(pt2DCenter.x + nrcBoundingCenterX, pt2DCenter.y + nrcBoundingCenterY);
	//nError = SegmentEdge(&pptContour[0],
	//				nPtContourNo,
	//				&pt2DRoughCenter);
	//if(nError)
	//	goto ErrorAbort;
	nError = EdgeInterpolationForInvalidPoint();
	if(nError)
		goto ErrorAbort;
	pt2DCenterRelative = pt2DCenter;
	pt2DCenter = CPoint2D(pt2DCenter.x + nrcBoundingCenterX, pt2DCenter.y + nrcBoundingCenterY);
	for(i=0;i<360;i++) {
		if(pbSegmentEdge[i] == FALSE) {
			dAngle = i*PI/180;
			ptSegmentEdge[i] = CPoint(Round(dSegmentRadius[i]*cos(dAngle) + pt2DCenterRelative.x), Round(dSegmentRadius[i]*sin(dAngle)+pt2DCenterRelative.y)); 
		}
		ptSegmentEdge[i] = CPoint(ptSegmentEdge[i].x + nrcBoundingCenterX, 
								ptSegmentEdge[i].y + nrcBoundingCenterY);
	}
	nError = Blob1D.Dilation(&pbSegmentEdge[0],
					360,
					1);
	nError = Blob1D.Erosion(&pbSegmentEdge[0],
					360,
					1);
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::Inspect");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CCircleInfo::InitResult()
{
	int i;
	pt2DCenter = CPoint2D(0,0);
	dRadius = 0;
	for(i=0;i<360;i++) {
		ptSegmentEdge[i] = CPoint(0,0);
		pbSegmentEdge[i] = FALSE;
		dSegmentRadius[i] = 0;
	}
	return 0;
}


int CCircleInfo::EdgeInterpolationForInvalidPoint()
{
	int i;
	CBlob1D Blob1D;
	CPoint2D pt2DEdge[362];
	int nValidEdgeNo;
	CInterpolationBy2Pts InterpolationBy2Pts;
	double dAngle;
	double dRatio;
	int nAngle;
	double dSegmentRadiusModified[360];
	int nError;
try {
	dRatio = 180/PI;
	for(i=0;i<360;i++) {
		pbSegmentEdge[i] = FALSE; 
		dSegmentRadiusModified[i] = -1;
	}
	/*for(i=0;i<360;i++) {
		dSegmentRadius[i] = -1;
		if(pbSegmentEdge[i] == FALSE)
			continue;
		dSegmentRadius[i] = sqrt((ptSegmentEdge[i].x - pt2DCenter.x) * (ptSegmentEdge[i].x - pt2DCenter.x) +
					(ptSegmentEdge[i].y - pt2DCenter.y) * (ptSegmentEdge[i].y - pt2DCenter.y));
	}*/
	for(i=0;i<360;i++) {
		if(dSegmentRadius[i] > 0) { 
			dAngle = atan2(ptSegmentEdge[i].y - pt2DCenter.y, ptSegmentEdge[i].x - pt2DCenter.x) * dRatio;
			if(dAngle < 0)
				dAngle += 360;
			if(dAngle >= 360)
				dAngle -= 360;
			nAngle = Round(dAngle);
			if((nAngle < 0) || (nAngle >= 360))
				continue;
			pbSegmentEdge[nAngle] = TRUE; 
			ptSegmentEdge[nAngle] = ptSegmentEdge[i];
			dSegmentRadiusModified[nAngle] = dSegmentRadius[i];
		}
	}
	for(i=0;i<360;i++) {
		dSegmentRadius[i] = dSegmentRadiusModified[i];
	}
	nValidEdgeNo = 0;
	for(i=0;i<360;i++) {
		if(dSegmentRadius[i] > 0) {
			pt2DEdge[nValidEdgeNo++] = CPoint2D(i,dSegmentRadius[i]);
		}
	}


	if(nValidEdgeNo > 1) {
		pt2DEdge[0] = CPoint2D(pt2DEdge[nValidEdgeNo-1].x - 360,pt2DEdge[nValidEdgeNo-1].y);
		pt2DEdge[nValidEdgeNo++] = CPoint2D(pt2DEdge[1].x + 360,pt2DEdge[1].y);
	}
	nError = InterpolationBy2Pts.SetPoint2DAry(&pt2DEdge[0], nValidEdgeNo);
	if(nError)
		goto ErrorAbort;
	for(i=0;i<360;i++) {
		if(pbSegmentEdge[i]) 
			continue;
		nError = InterpolationBy2Pts.GetY(i, &dSegmentRadius[i]);
		if(nError)
			continue;
	}
	//nError = Blob1D.Dilation(&pbSegmentEdge[0],
	//				360,
	//				1);
	//nError = Blob1D.Erosion(&pbSegmentEdge[0],
	//				360,
	//				1);

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::EdgeInterpolationForInvalidPoint");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CCircleInfo::SetInvalidEdge(double* pdRadius360,
						double dRadiusDifTol)
{
	int i;

	for(i=0;i<360;i++)
	{
		if(fabs(dSegmentRadius[i] - pdRadius360[i]) > dRadiusDifTol)
			pbSegmentEdge[i] = FALSE;
	}
	return 0;
}

int CCircleInfo::PointSortingByAngle(CPoint* pptContour,
				int nPtContourNo,
				CPoint2D* ppt2DCtr,
				int nProcMode,
				int nAngleDif)
{
	int i;
	CPoint2D pt2DRoughCenter;
	CPoint ptCtr;
	int nError;
try {
	nAngleDifInsp = nAngleDif;

	pt2DRoughCenter = *ppt2DCtr;
	ptCtr = CPoint(Round(ppt2DCtr->x), Round(ppt2DCtr->y));
	nEdgeProcMode = nProcMode;
	rcBoundingRefCenter = rcBounding - rcBounding.CenterPoint();
	nError = SegmentEdgeNoSampling(&pptContour[0],
					nPtContourNo,
					&pt2DRoughCenter);
	if(nError) 
		goto ErrorAbort;
	for(i=0;i<360;i++)
	{
		if(pbSegmentEdge[i])
		{
			ptSegmentEdge[i] = ptSegmentEdge[i] + ptCtr;
			dSegmentRadius[i] = pt2DRoughCenter.GetDistance(CPoint2D(ptSegmentEdge[i]));
		}
		else
			dSegmentRadius[i] = -1;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::EdgeInterpolationForInvalidPoint");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}
int CCircleInfo::PointSortingByAngle(CPoint* pptContour,
				int nPtContourNo,
				CPoint2D* ppt2DCtr,
				int nProcMode,
				CArray<CPoint,CPoint>* pAryPtContourSortingByAngle,
				int nAngleDif)
{
	int i;
	CPoint2D pt2DRoughCenter;
	CPoint ptCtr;
	int nError;
try {
	nAngleDifInsp = nAngleDif;

	pt2DRoughCenter = *ppt2DCtr;
	ptCtr = CPoint(Round(ppt2DCtr->x), Round(ppt2DCtr->y));
	nEdgeProcMode = nProcMode;
	rcBoundingRefCenter = rcBounding - rcBounding.CenterPoint();
	nError = SegmentEdgeNoSampling(&pptContour[0],
					nPtContourNo,
					&pt2DRoughCenter,
					pAryPtContourSortingByAngle);
	if(nError) 
		goto ErrorAbort;
	for(i=0;i<360;i++)
	{
		if(pbSegmentEdge[i])
		{
			ptSegmentEdge[i] = ptSegmentEdge[i] + ptCtr;
			dSegmentRadius[i] = pt2DRoughCenter.GetDistance(CPoint2D(ptSegmentEdge[i]));
		}
		else
			dSegmentRadius[i] = -1;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::EdgeInterpolationForInvalidPoint");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CCircleInfo::InspectNoSampling(CPoint* pptContour,
				int nPtContourNo,
				int nProcMode,
				CArray<CPoint,CPoint>* pAryPtContourSortingByAngle,
				int nAngleDif)
{
	int i;
	int nHistMax;
	int nHist;
	int nCenterIndex;
	int nrcBoundingCenterX;
	int nrcBoundingCenterY;
	CPoint2D pt2DRoughCenter;
	int nError;
try {
	pt2DCenter = CPoint2D(INT_MAX, INT_MAX);
	dRadius = INT_MAX;
	nAngleDifInsp = nAngleDif;

	nError = GetPointsBoundingRect(&pptContour[0], 
						  nPtContourNo,
						  &rcBounding);
	if(nError)
		goto ErrorAbort;
	pt2DRoughCenter = CPoint2D((rcBounding.left + rcBounding.right)/2.0,
							   (rcBounding.top + rcBounding.bottom)/2.0);
	nEstimatedRadius = (rcBounding.Width() + rcBounding.Height())/4;
	nEdgeProcMode = nProcMode;
	rcBoundingRefCenter = rcBounding - rcBounding.CenterPoint();
	nError = SegmentEdgeNoSampling(&pptContour[0],
					nPtContourNo,
					&pt2DRoughCenter,
					pAryPtContourSortingByAngle);
	if(nError)
		goto ErrorAbort;

	nError = EdgeInterpolation();
	if(nError)
		goto ErrorAbort;
 
//	nError = FindCircleCenter2(nAngleDif);
//	if(nCenterAryNo == 0) {
		nError = FindCircleCenter3(nAngleDif);
		if(nError)
			goto ErrorAbort;
//	}

	nHistMax = 0;
	nCenterIndex = 0;
	for(i=0;i<nCenterAryNo;i++) {
		nError = FindCircleRadius(&pt2DCenterAry[i],
							&nHist);
		if(nError)
				goto ErrorAbort;
		if(nHistMax < nHist) {
			nHistMax = nHist;
			nCenterIndex = i;
		}
	}
	pt2DCenter = pt2DCenterAry[nCenterIndex];
	if(nCenterAryNo > 1) {
		nError = FindCircleRadius(&pt2DCenter,
							&nHist);
		if(nError)
				goto ErrorAbort;
	}

	nrcBoundingCenterX = rcBounding.CenterPoint().x;
	nrcBoundingCenterY = rcBounding.CenterPoint().y;
	pt2DCenter = CPoint2D(pt2DCenter.x + nrcBoundingCenterX, pt2DCenter.y + nrcBoundingCenterY);
	for(i=0;i<360;i++) {
		ptSegmentEdge[i] = CPoint(ptSegmentEdge[i].x + nrcBoundingCenterX, 
								ptSegmentEdge[i].y + nrcBoundingCenterY);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::InspectNoSampling");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CCircleInfo::Deflate(double dDeflateRadius,
						 CArray<CPoint, CPoint>* pAryPtDeflatePos)
{
	int i;
	CPoint ptDeflate;
	double dRadiusAfterDelate, dRadiusRatio;
try {
	pAryPtDeflatePos->RemoveAll();
	if(dDeflateRadius < 0.00001) {
		for(i=0; i<360; i++) 
			pAryPtDeflatePos->Add(ptSegmentEdge[i]);
		goto PASS;
	}
	for(i=0; i<360; i++) {
		dRadiusAfterDelate = dSegmentRadius[i] - dDeflateRadius;
		if(dRadiusAfterDelate <= 0)
			ptDeflate = CPoint(Round(pt2DCenter.x), Round(pt2DCenter.y));
		else  {
			dRadiusRatio = dRadiusAfterDelate/dSegmentRadius[i];
			ptDeflate = CPoint(Round(pt2DCenter.x + (ptSegmentEdge[i].x - pt2DCenter.x) * dRadiusRatio),
				Round(pt2DCenter.y + (ptSegmentEdge[i].y - pt2DCenter.y) * dRadiusRatio));
		}
		pAryPtDeflatePos->Add(ptDeflate);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::Deflate");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}



int CCircleInfo::Inflate(double dInflateRadius,
						 CArray<CPoint, CPoint>* pAryPtInflatePos)
{
	int i;
	CPoint ptInflate;
	double dRadiusAfterDelate, dRadiusRatio;
try {
	pAryPtInflatePos->RemoveAll();
	if(dInflateRadius < 0.00001) {
		for(i=0; i<360; i++) 
			pAryPtInflatePos->Add(ptSegmentEdge[i]);
		goto PASS;
	}
	for(i=0; i<360; i++) {
		dRadiusAfterDelate = dSegmentRadius[i] + dInflateRadius;
		if(dRadiusAfterDelate <= 0)
			ptInflate = CPoint(Round(pt2DCenter.x), Round(pt2DCenter.y));
		else  {
			dRadiusRatio = dRadiusAfterDelate/dSegmentRadius[i];
			ptInflate = CPoint(Round(pt2DCenter.x + (ptSegmentEdge[i].x - pt2DCenter.x) * dRadiusRatio),
				Round(pt2DCenter.y + (ptSegmentEdge[i].y - pt2DCenter.y) * dRadiusRatio));
		}
		pAryPtInflatePos->Add(ptInflate);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::Inflate");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

int CCircleInfo::OffsetPos(CPoint* pptOffset)
{
	int i;
	for(i=0;i<360;i++)
	{
		ptSegmentEdge[i] = ptSegmentEdge[i] + *pptOffset;
	}
	pt2DCenter = pt2DCenter + *pptOffset;
	ptFOVRef = ptFOVRef + *pptOffset;
	return 0;
}


int CCircleInfo::Draw(IMAGEVIEW_INFO *pView, CPoint ptOffset, BOOL bSegments, LOGPEN* pPenInfo, int nSampling)
{
try {
	if(bSegments) {
		for(int i=1;i<360;i++) {
			if(pbSegmentEdge[i-1] && pbSegmentEdge[i])
				DrwLine(pView, &CPoint(ptSegmentEdge[i-1].x*nSampling+ptOffset.x, ptSegmentEdge[i-1].y*nSampling+ptOffset.y), &CPoint(ptSegmentEdge[i].x*nSampling+ptOffset.x, ptSegmentEdge[i].y*nSampling+ptOffset.y), pPenInfo);
		}
		if(pbSegmentEdge[359] && pbSegmentEdge[0])
			DrwLine(pView, &CPoint(ptSegmentEdge[359].x*nSampling+ptOffset.x, ptSegmentEdge[359].y*nSampling+ptOffset.y), &CPoint(ptSegmentEdge[0].x*nSampling+ptOffset.x, ptSegmentEdge[0].y*nSampling+ptOffset.y), pPenInfo);
	}
	else {
		DrwEllipse(pView, &CPoint2D(pt2DCenter.x*nSampling+ptOffset.x, pt2DCenter.y*nSampling+ptOffset.y), dRadius * nSampling, dRadius * nSampling, pPenInfo);
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::Draw");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}



int CCircleInfo::SegmentEdgeForMerge(CCircleInfo* pCircleInfo,
									 int* pnFndOverlapDeg)
{
	int i;
	int nCenterX, nCenterY;
	int nx, ny;
	int nAngle;
	double dRatio;
	CPoint ptSegmentEdgeForMerge0[360];
	BOOL pbSegmentEdgeForMerge0[360];
	CPoint ptSegmentEdgeForMerge1[360];
	BOOL pbSegmentEdgeForMerge1[360];
	int nValidNo;
	int nFndOverlapDeg;
try {
	*pnFndOverlapDeg = 0;
	dRatio = 180/PI;
	nCenterX = Round(pt2DCenter.x);
	nCenterY = Round(pt2DCenter.y);
	for(i=0;i<360;i++) {
		pbSegmentEdgeForMerge0[i] = FALSE;
		ptSegmentEdgeForMerge0[i] = CPoint(-1,-1);
	}
	nValidNo = 0;
	for(i=0;i<360;i++) {
		if(pCircleInfo->pbSegmentEdge[i] == FALSE)
			continue;
		nx = pCircleInfo->ptSegmentEdge[i].x - nCenterX;
		ny = pCircleInfo->ptSegmentEdge[i].y - nCenterY;
		nAngle = Round(atan2((double) ny, (double) nx) * dRatio);
		if(nAngle < 0)
			nAngle += 360;
		if(nAngle >= 360)
			nAngle -= 360;
		if((nAngle < 0) || (nAngle >= 360))
			continue;
		ptSegmentEdgeForMerge0[nAngle] = CPoint(nx, ny);
		pbSegmentEdgeForMerge0[nAngle] = TRUE;
		nValidNo++;
	}
	if(nValidNo <= 0)
		goto ErrorAbort;
	for(i=0;i<360;i++) {
		pbSegmentEdgeForMerge1[i] = FALSE;
		ptSegmentEdgeForMerge1[i] = CPoint(-1,-1);
	}
	for(i=0;i<360;i++) {
		if(pbSegmentEdge[i] == FALSE)
			continue;
		nx = ptSegmentEdge[i].x - nCenterX;
		ny = ptSegmentEdge[i].y - nCenterY;
		nAngle = Round(atan2((double) ny, (double) nx) * dRatio);
		if(nAngle < 0)
			nAngle += 360;
		if(nAngle >= 360)
			nAngle -= 360;
		if((nAngle < 0) || (nAngle >= 360))
			continue;
		ptSegmentEdgeForMerge1[nAngle] = CPoint(nx, ny);
		pbSegmentEdgeForMerge1[nAngle] = TRUE;
	}
	nFndOverlapDeg = 0;
	for(i=0;i<360;i++) {
		ptSegmentEdge[i] = ptSegmentEdgeForMerge0[i];
		pbSegmentEdge[i] = pbSegmentEdgeForMerge0[i];
		if(pbSegmentEdgeForMerge1[i]) {
			ptSegmentEdge[i] = ptSegmentEdgeForMerge1[i];
			pbSegmentEdge[i] = pbSegmentEdgeForMerge1[i];
		}
		if(pbSegmentEdgeForMerge0[i]&&pbSegmentEdgeForMerge1[i]) 
			nFndOverlapDeg++;
	}
	*pnFndOverlapDeg = nFndOverlapDeg;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::SegmentEdgeForMerge");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

CCircleInfo & CCircleInfo::operator =(CCircleInfo CircleInfo)
{
	int i;

	for(i=0;i<360;i++) {
		ptSegmentEdge[i] = CircleInfo.ptSegmentEdge[i];
		pbSegmentEdge[i] = CircleInfo.pbSegmentEdge[i];
		dSegmentRadius[i] = CircleInfo.dSegmentRadius[i];
	}
	pt2DCenter = CircleInfo.pt2DCenter;
	dRadius = CircleInfo.dRadius;
	nRadiusNo = CircleInfo.nRadiusNo;
	rcBounding = CircleInfo.rcBounding;
	nCircleMissingPercentage = CircleInfo.nCircleMissingPercentage;
	return *this;
}

int CCircleInfo::Backup(CCircleInfo* pCircleInfo)
{
	int i;

	for(i=0;i<360;i++) {
		pCircleInfo->ptSegmentEdge[i] = ptSegmentEdge[i];
		pCircleInfo->pbSegmentEdge[i] = pbSegmentEdge[i];
		pCircleInfo->dSegmentRadius[i] = dSegmentRadius[i];
	}
	pCircleInfo->pt2DCenter = pt2DCenter;
	pCircleInfo->dRadius = dRadius;
	pCircleInfo->nRadiusNo = nRadiusNo;
	pCircleInfo->rcBounding = rcBounding;
	pCircleInfo->nCircleMissingPercentage = nCircleMissingPercentage;
	return 0;
}



int CCircleInfo::GetValidRadiusNo(double dRadiusDifTol,
								  int* pnValidRadiusNo)
{
	int i;
	int nValidRadiusNo;
	double dMinRadius;
	double dMaxRadius;
try {
	*pnValidRadiusNo = 0;
	dMinRadius = dRadius - dRadiusDifTol;
	dMaxRadius = dRadius + dRadiusDifTol;
	nValidRadiusNo = 0;
	for(i=0;i<360;i++) {
		if(pbSegmentEdge[i] == FALSE)
			continue;
		if((dSegmentRadius[i] > dMinRadius) && (dSegmentRadius[i] <dMaxRadius))
			nValidRadiusNo++;
	}
	*pnValidRadiusNo = nValidRadiusNo;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::GetValidRadiusNo");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}



int CCircleInfo::GetPercentageByLineFitting(double dRadiusDifTol,
							   int nClosingSize,
							   int* pnConnectedPercentage,
							   int* pnPercentageAll)
{
	int i, j;
	CPoint2D pt2DRadius[720];
	int nIndex;
	int nRadiusNo;
	int nSegmentStep;
	int nSegmentNo;
	CArray<CPoint2D, CPoint2D> Arypt2DRadiusSegment;
	int nSegmentRadiusNo;
	int nStartNo;
	int nEndNo;
	CLine2D ln2DEdge;
	double dNominalAngleRad;
	double dTolAngleRad;
	double dAngleStep;
	double dOnLineTol;
	int nOnLineScore;
	CPoint2D pt2DFittingRadius[720];
	BOOL bEdgeMatching[720];
	int nFittingRadiusNo;
	double dFittingY;
	CBlob1D Blob1D;
	int nlength;
	int nMaxLength;
	int nAllLength;
	CInterpolationBy3Pts InterpolationBy3Pts;
	int nError;

try {
	Arypt2DRadiusSegment.RemoveAll();
	nSegmentStep = 20;
	nSegmentNo = 720/nSegmentStep;
	nRadiusNo = 0;

	*pnConnectedPercentage = 0;
	*pnPercentageAll = 0;
	dNominalAngleRad = 0;
	dTolAngleRad = 10 * PI/180;
	dAngleStep = PI/180;
	dOnLineTol = 3;

	nFittingRadiusNo = 0;
	for(i=0;i<nSegmentNo;i++) {
		nStartNo = i * nSegmentStep;
		nEndNo = nStartNo + nSegmentStep;
		if(nEndNo > 720)
			continue;
		Arypt2DRadiusSegment.RemoveAll();
		nSegmentRadiusNo = 0;
		for(j=nStartNo;j<nEndNo;j++) {
			nIndex = j;
			if(nIndex >= 360)
				nIndex -= 360;
			if(pbSegmentEdge[nIndex] == FALSE) 
				continue;
			Arypt2DRadiusSegment.Add(CPoint2D(double(j), dSegmentRadius[nIndex]));
		}
		if(Arypt2DRadiusSegment.GetSize() < (nSegmentStep/2))
			continue;
		nError = GetHoughTransformLine(&Arypt2DRadiusSegment,
							dNominalAngleRad,
							dTolAngleRad,
							dAngleStep,
							dOnLineTol,
							&nOnLineScore,
							&ln2DEdge);
		if(nError != 0)
			continue;
		if(nOnLineScore < 50)
			continue;
		pt2DFittingRadius[nFittingRadiusNo++] = ln2DEdge.GetPosition();
	}
	if(nFittingRadiusNo < 3)
		goto PASS;
	nError = InterpolationBy3Pts.SetPoint2DAry(&pt2DFittingRadius[0],
								nFittingRadiusNo);
	for(i=0;i<720;i++) {
		bEdgeMatching[i] = FALSE;
		nIndex = i;
		if(nIndex >= 360)
			nIndex -= 360;
		if(pbSegmentEdge[nIndex] == FALSE) 
			continue;
		InterpolationBy3Pts.GetY(i, &dFittingY);
		if(fabs(dFittingY - dSegmentRadius[nIndex]) < dRadiusDifTol)
			bEdgeMatching[i] = TRUE;
	}
	nError = Blob1D.Inspect(&bEdgeMatching[0],
							720,
							TRUE,
							1,
							720);
	nError = Blob1D.Dilation(&bEdgeMatching[0],
							720,
							5);
	nError = Blob1D.Erosion(&bEdgeMatching[0],
							720,
							5);
	nError = Blob1D.Inspect(&bEdgeMatching[0],
							720,
							TRUE,
							1,
							720);

	nMaxLength = 0;
	nAllLength = 0;
	for(i=0;i<Blob1D.AryPtBlobRange.GetSize();i++) {
		nlength = Blob1D.AryPtBlobRange.GetAt(i).y - Blob1D.AryPtBlobRange.GetAt(i).x;
		if(nMaxLength < nlength)
			nMaxLength = nlength;
		nAllLength += nlength;
	}
	*pnConnectedPercentage = nMaxLength * 100/360;
	*pnPercentageAll = nAllLength * 100/720;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::GetPercentageByLineFitting");
	goto ErrorAbort;
}
PASS:
	Arypt2DRadiusSegment.RemoveAll();
	return 0;
ErrorAbort:
	Arypt2DRadiusSegment.RemoveAll();
	return -1;
}


int CCircleInfo::GetPercentage(double dRadiusDifTol,
							   int* pnPercentage)
{
	int nError;
try {
	nError = GetValidRadiusNo(dRadiusDifTol, pnPercentage);
	if(nError)
		*pnPercentage = 0;
	*pnPercentage = (*pnPercentage * 100)/360;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::GetPercentage");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CCircleInfo::GetPercentageByValidEdge(double dRadiusDifTol,
							   int* pnPercentage)
{
	int i;
	int nValidRadiusNo;
	int nError;
try {
	nError = GetValidRadiusNo(dRadiusDifTol, pnPercentage);
	if(nError)
		*pnPercentage = 0;
	nValidRadiusNo = 0;
	for(i=0;i<360;i++) {
		if(pbSegmentEdge[i] == FALSE)
			continue;
		nValidRadiusNo++;
	}
	if(nValidRadiusNo > 60)
	{
		*pnPercentage = (*pnPercentage * 100)/nValidRadiusNo;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::GetPercentage");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}
int CCircleInfo::Merge(CCircleInfo* pCircleInfo,
			  int nMaxOverlapAngleDeg,
			  double dRadiusDifTol,
			  BOOL* pbMergedOK)
{
	int i;
	int nHistMax; 
	int nHist;
	int nCenterIndex;
	CPoint2D pt2DRoughCenter;
	CCircleInfo CircleInfoBackup;
	int nValidRadiusNoOrg;
	int nValidRadiusNoMerged;
	int nFndOverlapDeg;
	int nError;
try {
	*pbMergedOK = FALSE;
	nError = GetValidRadiusNo(dRadiusDifTol, &nValidRadiusNoOrg);
	if(nError)
		goto ErrorAbort;
	Backup(&CircleInfoBackup);
	nError = SegmentEdgeForMerge(pCircleInfo, &nFndOverlapDeg);
	if(nError)
		goto ErrorAbort;
	if(nFndOverlapDeg >= nMaxOverlapAngleDeg)
		goto ErrorAbort;
//	nError = EdgeInterpolation();

	nError = FindCircleCenter4(nAngleDifInsp, 180);
	if(nError)
		goto ErrorAbort;
	nHistMax = 0;
	nCenterIndex = 0;
	for(i=0;i<nCenterAryNo;i++) {
		nError = FindCircleRadius(&pt2DCenterAry[i],
							&nHist);
		if(nError)
			goto ErrorAbort;
		if(nHistMax < nHist) {
			nHistMax = nHist;
			nCenterIndex = i;
		}
	}
	pt2DCenter = pt2DCenterAry[nCenterIndex];
	if(nCenterAryNo >= 1) {
		nError = FindCircleRadius(&pt2DCenter,
							&nHist);
		if(nError)
				goto ErrorAbort;
	}
	else {
		goto ErrorAbort;
	}
	pt2DCenter = CPoint2D(pt2DCenter.x + CircleInfoBackup.pt2DCenter.x, 
						  pt2DCenter.y + CircleInfoBackup.pt2DCenter.y);
	for(i=0;i<360;i++) {
		ptSegmentEdge[i] = CPoint(Round(ptSegmentEdge[i].x + CircleInfoBackup.pt2DCenter.x), 
								Round(ptSegmentEdge[i].y + CircleInfoBackup.pt2DCenter.y));
	}
	nError = GetValidRadiusNo(dRadiusDifTol, &nValidRadiusNoMerged);
	if(nError)
		goto ErrorAbort;
	if((nValidRadiusNoMerged < nValidRadiusNoOrg) ||
		(fabs(dRadius - CircleInfoBackup.dRadius) > dRadiusDifTol)) 
		goto ErrorAbort;
	*pbMergedOK = TRUE;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::Merge");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	nError = Restore(&CircleInfoBackup);
	return -1;
}

int CCircleInfo::Restore(CCircleInfo* pCircleInfo)
{
	int i;
try {
	for(i=0;i<360;i++) {
		ptSegmentEdge[i] = pCircleInfo->ptSegmentEdge[i];
		pbSegmentEdge[i] = pCircleInfo->pbSegmentEdge[i];
		dSegmentRadius[i] = pCircleInfo->dSegmentRadius[i];
	}
	pt2DCenter = pCircleInfo->pt2DCenter;
	dRadius = pCircleInfo->dRadius;
	nRadiusNo = pCircleInfo->nRadiusNo;
	rcBounding = pCircleInfo->rcBounding;
	nCircleMissingPercentage = pCircleInfo->nCircleMissingPercentage;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::Restore");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CCircleInfo::FindCircleRadius(CPoint2D* ppt2DCenter,
								  int* pnMaxHist)
{
	int i;
	double dCenterX;
	double dCenterY;
	CAdvHistogram AdvHistogram;
	double dHistConv;
	int nMaxHist;
	int nValidRadius;
	double dSegmentRadiusHist[360];
	double dSegmentRadiusHistSecond[360];
	int nSegmentRadiusHistSecondNo;
	double dHistConvHalf;
	double dRadiusSecond;
	int nError;
try {
	dHistConv = 20;
	dHistConvHalf = dHistConv/2;
	dCenterX = ppt2DCenter->x;
	dCenterY = ppt2DCenter->y;
	nValidRadius = 0;
	for(i=0;i<360;i++) {
		dSegmentRadius[i] = -1;
		if(pbSegmentEdge[i] == FALSE)
			continue;
		dSegmentRadius[i] = sqrt((ptSegmentEdge[i].x - dCenterX) * (ptSegmentEdge[i].x - dCenterX) +
					(ptSegmentEdge[i].y - dCenterY) * (ptSegmentEdge[i].y - dCenterY));
		dSegmentRadiusHist[nValidRadius++] = dSegmentRadius[i];
	}
	nError = AdvHistogram.GetMaxConvHist1D(&dSegmentRadiusHist[0], 
									nValidRadius,
									dHistConv,
									nMaxHist,
									dRadius);
	if(nError)
		goto ErrorAbort;
	*pnMaxHist = nMaxHist;
	nSegmentRadiusHistSecondNo = 0;
	for(i=0;i<nValidRadius;i++) {
		if(fabs(dSegmentRadiusHist[i] - dRadius) > dHistConvHalf)
			dSegmentRadiusHistSecond[nSegmentRadiusHistSecondNo++] = dSegmentRadiusHist[i];
	}

	if(nSegmentRadiusHistSecondNo > 0)
	{
		nError = AdvHistogram.GetMaxConvHist1D(&dSegmentRadiusHistSecond[0], 
										nSegmentRadiusHistSecondNo,
										dHistConv,
										nMaxHist,
										dRadiusSecond);
		if(nError)
			goto ErrorAbort;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::FindCircleRadius");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}



int CCircleInfo::GetAvgRadius(CPoint2D* ppt2DCtr,
					 int nStartPercentage,
					 int nEndPercentage,
					 double* pdAvgRadius)
{
	int i;
	int nStartIndex;
	int nEndIndex;
	double dRadiusSorting[360];
	int nValidRadiusNo;
	double dSum;
	double dx, dy;
	int nWeight;
try {
	*pdAvgRadius = 0;
	if((nStartPercentage < 0) || (nStartPercentage >= 100) ||
		(nEndPercentage < 0) || (nEndPercentage > 100) ||
		(nStartPercentage>nEndPercentage))
		goto ErrorAbort;
	nValidRadiusNo = 0;
	for(i=0;i<360;i++) {
		if(pbSegmentEdge[i] == FALSE)
			continue;
		dx = ptSegmentEdge[i].x - ppt2DCtr->x;
		dy = ptSegmentEdge[i].y - ppt2DCtr->y;
		dRadiusSorting[nValidRadiusNo++] = sqrt(dx * dx + dy * dy);
	}
	Sorting(&dRadiusSorting[0], nValidRadiusNo, TRUE);
	nStartIndex = nStartPercentage * nValidRadiusNo/100;
	nEndIndex = nEndPercentage * nValidRadiusNo/100;
	dSum = 0;
	nWeight = 0;
	for(i=nStartIndex;i<nEndIndex;i++) {
		dSum += dRadiusSorting[i];
		nWeight++;
	}
	if(nWeight <= 0)
		goto ErrorAbort;
	*pdAvgRadius = dSum/nWeight;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::GetAvgRadius");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}




int CCircleInfo::GetAvgRadius(int nStartPercentage,
					 int nEndPercentage,
					 double* pdAvgRadius)
{
	int i;
	int nStartIndex;
	int nEndIndex;
	double dRadiusSorting[360];
	int nValidRadiusNo;
	double dSum;
	int nWeight;
try {
	*pdAvgRadius = 0;
	if((nStartPercentage < 0) || (nStartPercentage >= 100) ||
		(nEndPercentage < 0) || (nEndPercentage > 100) ||
		(nStartPercentage>nEndPercentage))
		goto ErrorAbort;
	nValidRadiusNo = 0;
	for(i=0;i<360;i++) {
		if(pbSegmentEdge[i] == FALSE)
			continue;
		dRadiusSorting[nValidRadiusNo++] = dSegmentRadius[i];
	}
	Sorting(&dRadiusSorting[0], nValidRadiusNo, TRUE);
	nStartIndex = nStartPercentage * nValidRadiusNo/100;
	nEndIndex = nEndPercentage * nValidRadiusNo/100;
	dSum = 0;
	nWeight = 0;
	for(i=nStartIndex;i<nEndIndex;i++) {
		dSum += dRadiusSorting[i];
		nWeight++;
	}
	if(nWeight <= 0)
		goto ErrorAbort;
	*pdAvgRadius = dSum/nWeight;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::GetAvgRadius");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CCircleInfo::GetAvgRadiusPercentage(double dMinRadius,
					 double dMaxRadius,
					 double* pdRadiusPercentage)
{
	int i;
	int nSum;
	int nWeight;
try {
	*pdRadiusPercentage = 0;
	nSum = 0;
	nWeight = 0;
	for(i=0;i<360;i++) {
		if(pbSegmentEdge[i] == FALSE)
			continue;
		nWeight++;
		if((dSegmentRadius[i]>=dMinRadius) && (dSegmentRadius[i]<=dMaxRadius))
		{
			nSum++;
		}
	}
	if(nWeight <= 0)
		goto ErrorAbort;
	*pdRadiusPercentage = nSum * 100.0/nWeight;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::GetAvgRadiusPercentage");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CCircleInfo::GetMaxBlobRadiusPercentage(double dMinRadius,
					 double dMaxRadius,
					 double* pdRadiusPercentage)
{
	int i;
	BOOL bGoodRadius[720];
	CBlob1D Blob1D;
	int nMax;
	CPoint ptRange;
	int nError;
try {
	*pdRadiusPercentage = 0;
	for(i=0;i<360;i++) {
		bGoodRadius[i] = FALSE;
		if(pbSegmentEdge[i] == FALSE)
			continue;
		if((dSegmentRadius[i]>=dMinRadius) && (dSegmentRadius[i]<=dMaxRadius))
		{
			bGoodRadius[i] = TRUE;
		}
	}
	for(i=0;i<360;i++)
	{
		bGoodRadius[i+360] = bGoodRadius[i];
	}
	nError = Blob1D.Inspect(&bGoodRadius[0],
						720,
						TRUE,
						1,
						720);
	nMax = 0;
	for(i=0;i<Blob1D.AryPtBlobRange.GetSize();i++)
	{
		ptRange = Blob1D.AryPtBlobRange.GetAt(i);
		if(nMax < (ptRange.y - ptRange.x))
		{
			nMax = ptRange.y - ptRange.x;
		}
	}
	*pdRadiusPercentage = nMax * 100.0/360;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::GetMaxBlobRadiusPercentage");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CCircleInfo::FindCircleCenter()
{
	int i;
	double dCenterX[180];
	double dCenterY[180];
	double dCenterXHist[180];
	int nXHistNo;
	double dCenterYHist[180];
	int nYHistNo;
	CAdvHistogram AdvHistogram;
	double dHistConv;
	int	nMaxHist;
	double dAvgPosX;
	double dAvgPosY;
	int nError;
try {
	dHistConv = 20;
	for(i= 0;i<180;i++) {
		dCenterX[i] = (ptSegmentEdge[i].x + ptSegmentEdge[i+180].x)/2;
		dCenterY[i] = (ptSegmentEdge[i].y + ptSegmentEdge[i+180].y)/2;
	}

	nXHistNo = 0;
	for(i=0;i<45;i++) 
		dCenterXHist[nXHistNo++] = dCenterX[i];
	for(i=135;i<180;i++) 
		dCenterXHist[nXHistNo++] = dCenterX[i];
	nYHistNo = 0;
	for(i=45;i<135;i++) 
		dCenterXHist[nYHistNo++] = dCenterY[i];

	nError = AdvHistogram.GetMaxConvHist1D(&dCenterXHist[0], 
									nXHistNo,
									dHistConv,
									nMaxHist,
									dAvgPosX);

	if(nError)
		goto ErrorAbort;

	nError = AdvHistogram.GetMaxConvHist1D(&dCenterYHist[0], 
									nYHistNo,
									dHistConv,
									nMaxHist,
									dAvgPosY);
	if(nError)
		goto ErrorAbort;
	pt2DCenter = CPoint2D(dAvgPosX, dAvgPosY);

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::FindCircleCenter");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}



int CCircleInfo::FindCircleCenter2(int nAngleDif)
{
	int i, j;
	int nStartIndex;
	int nEncIndex;
	CPoint2D pt2DLineCtr[360];
	CPoint2D pt2DCircleCtr[180];
	double dCenterXHist[180];
	int nCenterXHist;
	double dCenterYHist[180];
	int nCenterYHist;
	BOOL bCenterInValid[180];
	CLine2D ln2DCenterPerpendicular[360];
	CLine2D ln2DSegment;
	CAdvHistogram AdvHistogram;
	double dHistConv;
	int	nMaxHist;
	double dAvgPosX[2];
	int nPosX;
	double dAvgPosY[2];
	int nPosY;
	double dMin;
	double dMax;
	int nValidCenterNo;
	int nError;
try {
	dHistConv = 20;
	for(i= 0;i<360;i++) {
		ln2DCenterPerpendicular[i] = CLine2D();
		nStartIndex = i;
		nEncIndex = 180 + i;
		if(nEncIndex >= 360)
			nEncIndex = nEncIndex -360;
		if((pbSegmentEdge[nStartIndex] == FALSE) || (pbSegmentEdge[nEncIndex] == FALSE))
			continue;
		if(ptSegmentEdge[nStartIndex] == ptSegmentEdge[nEncIndex])
			continue;
		ln2DSegment = CLine2D(ptSegmentEdge[nStartIndex], ptSegmentEdge[nEncIndex]);
		pt2DLineCtr[i] = CPoint2D((ptSegmentEdge[nStartIndex].x + ptSegmentEdge[nEncIndex].x)/2,
								(ptSegmentEdge[nStartIndex].y + ptSegmentEdge[nEncIndex].y)/2);
		ln2DSegment.GetPerpendicularLine(&pt2DLineCtr[i],
								  &ln2DCenterPerpendicular[i]);

	}
	for(i= 0;i<180;i++) {
		bCenterInValid[i] = TRUE;
		if((ln2DCenterPerpendicular[i].IsInValid()) || (ln2DCenterPerpendicular[i+nAngleDif].IsInValid()))
			continue;
		nError = ln2DCenterPerpendicular[i].GetIntercept(&ln2DCenterPerpendicular[i+nAngleDif], 
						  &pt2DCircleCtr[i]);
		if(nError)
			goto ErrorAbort;
		bCenterInValid[i] = FALSE;
	}

	nValidCenterNo = 0;
	for(i=0;i<180;i++) { 
		if(bCenterInValid[i]) 
			continue;
		dCenterXHist[nValidCenterNo] = pt2DCircleCtr[i].x;
		dCenterYHist[nValidCenterNo] = pt2DCircleCtr[i].y;
		nValidCenterNo++;
	}
	nPosX = 0;
	nError = AdvHistogram.GetMaxConvHist1D(&dCenterXHist[0], 
									nValidCenterNo,
									dHistConv,
									nMaxHist,
									dAvgPosX[nPosX]);
	if(nError)
		goto ErrorAbort;
	if((dAvgPosX[nPosX] > rcBoundingRefCenter.left) && (dAvgPosX[nPosX] < rcBoundingRefCenter.right)) 
		nPosX++;
	if(nMaxHist < 120) {
		nCenterXHist = 0;
		dMin = dAvgPosX[0] - dHistConv/2;
		dMax = dAvgPosX[0] + dHistConv/2;
		for(i=0;i<nValidCenterNo;i++) {
			if((pt2DCircleCtr[i].x < dMin) || (pt2DCircleCtr[i].x > dMax))
				dCenterXHist[nCenterXHist++] = pt2DCircleCtr[i].x;
		}
		nError = AdvHistogram.GetMaxConvHist1D(&dCenterXHist[0], 
										nCenterXHist,
										dHistConv,
										nMaxHist,
										dAvgPosX[nPosX]);
		if(nError)
				goto ErrorAbort;
		if((dAvgPosX[nPosX] > rcBoundingRefCenter.left) && (dAvgPosX[nPosX] < rcBoundingRefCenter.right)) 
			nPosX++;
	}

	nPosY = 0;
	nError = AdvHistogram.GetMaxConvHist1D(&dCenterYHist[0], 
									nValidCenterNo,
									dHistConv,
									nMaxHist,
									dAvgPosY[nPosY]);
	if(nError)
		goto ErrorAbort;
	if((dAvgPosY[nPosY] > rcBoundingRefCenter.top) && (dAvgPosY[nPosY] < rcBoundingRefCenter.bottom)) 
		nPosY++;
	if(nMaxHist < 120) {
		nCenterYHist = 0;
		dMin = dAvgPosY[0] - dHistConv/2;
		dMax = dAvgPosY[0] + dHistConv/2;
		for(i=0;i<nValidCenterNo;i++) {
			if((pt2DCircleCtr[i].y < dMin) || (pt2DCircleCtr[i].y > dMax))
				dCenterYHist[nCenterYHist++] = pt2DCircleCtr[i].y;
		}
		nError = AdvHistogram.GetMaxConvHist1D(&dCenterYHist[0], 
										nCenterYHist,
										dHistConv,
										nMaxHist,
										dAvgPosY[nPosY]);
		if(nError)
			goto ErrorAbort;
		if((dAvgPosY[nPosY] > rcBoundingRefCenter.top) && (dAvgPosY[nPosY] < rcBoundingRefCenter.bottom)) 
			nPosY++;
	}
	nCenterAryNo = 0;
	for(i=0;i<nPosX;i++) {
		for(j=0;j<nPosY;j++) {
			pt2DCenterAry[nCenterAryNo++] = CPoint2D(dAvgPosX[i], dAvgPosY[j]);
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::FindCircleCenter2");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CCircleInfo::FindCircleCenter3(int nAngleDif)
{
	int i, j;
	int nStartIndex;
	int nEncIndex;
	CPoint2D pt2DLineCtr[360];
	CPoint2D pt2DCircleCtr[360];
	double dCenterXHist[360];
	int nCenterXHist;
	double dCenterYHist[360];
	int nCenterYHist;
	BOOL bCenterInValid[360];
	CLine2D ln2DCenterPerpendicular[360];
	CLine2D ln2DSegment;
	CAdvHistogram AdvHistogram;
	double dHistConv;
	int	nMaxHist;
	double dAvgPosX[2];
	int nPosX;
	double dAvgPosY[2];
	int nPosY;
	double dMin;
	double dMax;
	int nValidCenterNo;
	int nEndLineIndex[360];
	int nError;

try {
	dHistConv = 20;
	for(i= 0;i<360;i++) {
		ln2DCenterPerpendicular[i] = CLine2D();
		nStartIndex = i;
		nEncIndex = nAngleDif + i;
		if(nEncIndex >= 360)
			nEncIndex = nEncIndex -360;
		nEndLineIndex[i] = nEncIndex;
		if((pbSegmentEdge[nStartIndex] == FALSE) || (pbSegmentEdge[nEncIndex] == FALSE))
			continue;
		if(ptSegmentEdge[nStartIndex] == ptSegmentEdge[nEncIndex])
			continue;
		ln2DSegment = CLine2D(ptSegmentEdge[nStartIndex], ptSegmentEdge[nEncIndex]);
		pt2DLineCtr[i] = CPoint2D((ptSegmentEdge[nStartIndex].x + ptSegmentEdge[nEncIndex].x)/2,
								(ptSegmentEdge[nStartIndex].y + ptSegmentEdge[nEncIndex].y)/2);
		ln2DSegment.GetPerpendicularLine(&pt2DLineCtr[i],
								  &ln2DCenterPerpendicular[i]);

	}
	for(i= 0;i<360;i++) {
		bCenterInValid[i] = TRUE;
		if((ln2DCenterPerpendicular[i].IsInValid()) || (ln2DCenterPerpendicular[nEndLineIndex[i]].IsInValid()))
			continue;
		nError = ln2DCenterPerpendicular[i].GetIntercept(&ln2DCenterPerpendicular[nEndLineIndex[i]], 
						  &pt2DCircleCtr[i]);
		if(nError)
			goto ErrorAbort;
		bCenterInValid[i] = FALSE;
	}

	nValidCenterNo = 0;
	for(i=0;i<360;i++) { 
		if(bCenterInValid[i]) 
			continue;
		dCenterXHist[nValidCenterNo] = pt2DCircleCtr[i].x;
		dCenterYHist[nValidCenterNo] = pt2DCircleCtr[i].y;
		nValidCenterNo++;
	}
	nPosX = 0;
	nError = AdvHistogram.GetMaxConvHist1D(&dCenterXHist[0], 
									nValidCenterNo,
									dHistConv,
									nMaxHist,
									dAvgPosX[nPosX]);
	if(nError)
			goto ErrorAbort;
	if((dAvgPosX[nPosX] > rcBoundingRefCenter.left) && (dAvgPosX[nPosX] < rcBoundingRefCenter.right)) 
		nPosX++;
	if(nMaxHist < 120) {
		nCenterXHist = 0;
		dMin = dAvgPosX[0] - dHistConv/2;
		dMax = dAvgPosX[0] + dHistConv/2;
		for(i=0;i<nValidCenterNo;i++) {
			if((pt2DCircleCtr[i].x < dMin) || (pt2DCircleCtr[i].x > dMax))
				dCenterXHist[nCenterXHist++] = pt2DCircleCtr[i].x;
		}
		nError = AdvHistogram.GetMaxConvHist1D(&dCenterXHist[0], 
										nCenterXHist,
										dHistConv,
										nMaxHist,
										dAvgPosX[nPosX]);

		if(nError)
			goto ErrorAbort;

		if((dAvgPosX[nPosX] > rcBoundingRefCenter.left) && (dAvgPosX[nPosX] < rcBoundingRefCenter.right)) 
			nPosX++;
	}

	nPosY = 0;
	nError = AdvHistogram.GetMaxConvHist1D(&dCenterYHist[0], 
									nValidCenterNo,
									dHistConv,
									nMaxHist,
									dAvgPosY[nPosY]);
	if(nError)
			goto ErrorAbort;
	
	if((dAvgPosY[nPosY] > rcBoundingRefCenter.top) && (dAvgPosY[nPosY] < rcBoundingRefCenter.bottom)) 
		nPosY++;
	if(nMaxHist < 120) {
		nCenterYHist = 0;
		dMin = dAvgPosY[0] - dHistConv/2;
		dMax = dAvgPosY[0] + dHistConv/2;
		for(i=0;i<nValidCenterNo;i++) {
			if((pt2DCircleCtr[i].y < dMin) || (pt2DCircleCtr[i].y > dMax))
				dCenterYHist[nCenterYHist++] = pt2DCircleCtr[i].y;
		}
		nError = AdvHistogram.GetMaxConvHist1D(&dCenterYHist[0], 
										nCenterYHist,
										dHistConv,
										nMaxHist,
										dAvgPosY[nPosY]);
		if(nError)
			goto ErrorAbort;
		if((dAvgPosY[nPosY] > rcBoundingRefCenter.top) && (dAvgPosY[nPosY] < rcBoundingRefCenter.bottom)) 
			nPosY++;
	}
	nCenterAryNo = 0;
	for(i=0;i<nPosX;i++) {
		for(j=0;j<nPosY;j++) {
			pt2DCenterAry[nCenterAryNo++] = CPoint2D(dAvgPosX[i], dAvgPosY[j]);
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::FindCircleCenter3");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CCircleInfo::FindCircleCenter4(int nAngleMin, int nAngleMax)
{
	int i, j;
	int nStartIndex;
	int nEncIndex;
	CPoint2D pt2DLineCtr[360];
	CPoint2D pt2DCircleCtr[360];
	double dCenterXHist[360];
	double dCenterXHist1[360];
	int nCenterXHist1;
	double dCenterYHist[360];
	double dCenterYHist1[360];
	int nCenterYHist1;
	BOOL bCenterInValid[360];
	CLine2D ln2DCenterPerpendicular[360];
	CLine2D ln2DSegment;
	CAdvHistogram AdvHistogram;
	double dHistConv;
	int	nMaxHist;
	int	nMaxHist1;
	double dAvgPosX[2];
	int nPosX;
	double dAvgPosY[2];
	int nPosY;
	double dMin;
	double dMax;
	int nValidCenterNo;
	BOOL bFoundPair;
	CPoint ptLinePair[360];
	int nLinePairStatus[360];
	int nLinePairAngleDif[360];
	int nCtrPos;
	int nAngleDif;
	BOOL bCheckCtr[4];
	int nAnglePos[4];
	int nAngleVar;
	BOOL bFoundCtr;
	int nError;

try {
	nAngleVar = 20;
	nAnglePos[0] = 180;
	nAnglePos[1] = 270;
	nAnglePos[2] = 0;
	nAnglePos[3] = 90;
	for(i=0;i<4;i++) {
		bCheckCtr[i] = FALSE;
/*		for(j= nAnglePos[i] - nAngleVar;j<(nAnglePos[i] + nAngleVar);j++) {
			nIndex = j;
			if(nIndex < 0)
				nIndex += 360;
			if(nIndex >= 360)
				nIndex -= 360;
			if(pbSegmentEdge[nIndex]) {
				bCheckCtr[i] = TRUE;
				break;
			}
		}
*/
	}

	dHistConv = 20;
	for(i= 0;i<360;i++) {
		ptLinePair[i] = CPoint(-1,-1);
		nLinePairStatus[i] = -1;
		nLinePairAngleDif[i] = 0;
	}
	for(i= 0;i<360;i++) {
		nStartIndex = i;
		if(pbSegmentEdge[nStartIndex] == FALSE) {
			continue;
		}
		bFoundPair = FALSE;
		for(j=nAngleMax;j>=nAngleMin;j--) {
			nEncIndex = j + i;
			if(nEncIndex >= 360)
				nEncIndex = nEncIndex -360;
			if(nEncIndex >= 360)
				continue;
			if(pbSegmentEdge[nEncIndex]) {
				nCtrPos = i+j/2;
				if(nCtrPos >= 360)
					nCtrPos = nCtrPos -360;
				if(nCtrPos >= 360)
					continue;
				if(bFoundPair == FALSE) {
					if(nLinePairStatus[nCtrPos] == 1)
						continue;
					nLinePairStatus[nCtrPos] = 1;
					ptLinePair[nCtrPos] = CPoint(nStartIndex, nEncIndex);
					bFoundPair = TRUE;
				}
				else {
					nAngleDif = __min(abs(nEncIndex - nStartIndex), 360 - abs(nEncIndex - nStartIndex));
					if(nLinePairStatus[nCtrPos] == 0) {
						if(nAngleDif > nLinePairAngleDif[nCtrPos]) {
							nLinePairStatus[nCtrPos] = 0;
							ptLinePair[nCtrPos] = CPoint(nStartIndex, nEncIndex);
							nLinePairAngleDif[nCtrPos] = nAngleDif;
						}
					}
					if(nLinePairStatus[nCtrPos] < 0) {
						nLinePairStatus[nCtrPos] = 0;
						ptLinePair[nCtrPos] = CPoint(nStartIndex, nEncIndex);
						nLinePairAngleDif[nCtrPos] = nAngleDif;
					}
				}
			}
		}
	}


	for(i= 0;i<360;i++) {
		ln2DCenterPerpendicular[i] = CLine2D();
		if(nLinePairStatus[i] < 0)
			continue;
		ln2DSegment = CLine2D(ptSegmentEdge[ptLinePair[i].x], ptSegmentEdge[ptLinePair[i].y]);
		pt2DLineCtr[i] = CPoint2D((ptSegmentEdge[ptLinePair[i].x].x + ptSegmentEdge[ptLinePair[i].y].x)/2,
								(ptSegmentEdge[ptLinePair[i].x].y + ptSegmentEdge[ptLinePair[i].y].y)/2);
		ln2DSegment.GetPerpendicularLine(&pt2DLineCtr[i],
								  &ln2DCenterPerpendicular[i]);

	}
	for(i= 0;i<360;i++) {
		bCenterInValid[i] = TRUE;
		if(ln2DCenterPerpendicular[i].IsInValid()) 
			continue;
		bFoundPair = FALSE;
		for(j=i+90;j>=(i+nAngleMin);j--) {
			nEncIndex = j;
			if(nEncIndex >= 360)
				nEncIndex = nEncIndex -360;
			if(nEncIndex >= 360)
				continue;
			if(ln2DCenterPerpendicular[nEncIndex].IsInValid() == FALSE) {
				bFoundPair = TRUE;
				break;
			}
		}
		if(bFoundPair == FALSE)
			continue;
		nError = ln2DCenterPerpendicular[i].GetIntercept(&ln2DCenterPerpendicular[nEncIndex], 
						  &pt2DCircleCtr[i]);
		if(nError)
			continue;
		bCenterInValid[i] = FALSE;
	}

	nValidCenterNo = 0;
	for(i=0;i<360;i++) { 
		if(bCenterInValid[i]) 
			continue;
		dCenterXHist[nValidCenterNo] = pt2DCircleCtr[i].x;
		dCenterYHist[nValidCenterNo] = pt2DCircleCtr[i].y;
		nValidCenterNo++;
	}
	nPosX = 0;
	nError = AdvHistogram.GetMaxConvHist1D(&dCenterXHist[0], 
									nValidCenterNo,
									dHistConv,
									nMaxHist,
									dAvgPosX[nPosX]);
	if(nError)
		bFoundCtr = FALSE;
	else
		bFoundCtr = TRUE;
	if(bCheckCtr[0]) {
		if(dAvgPosX[nPosX] < rcBoundingRefCenter.left) 
			bFoundCtr = FALSE;
	}
	if(bCheckCtr[2]) {
		if(dAvgPosX[nPosX] > rcBoundingRefCenter.right)
			bFoundCtr = FALSE;
	}
	if(bFoundCtr)
		nPosX++;

	if(nMaxHist < 120) {
		nCenterXHist1 = 0;
		dMin = dAvgPosX[0] - dHistConv/2;
		dMax = dAvgPosX[0] + dHistConv/2;
		for(i=0;i<nValidCenterNo;i++) {
			if((dCenterXHist[i] < dMin) || (dCenterXHist[i] > dMax))
				dCenterXHist1[nCenterXHist1++] = dCenterXHist[i];
		}
		if(nCenterXHist1 > __max(1,nValidCenterNo/10)) {
			nError = AdvHistogram.GetMaxConvHist1D(&dCenterXHist1[0], 
											nCenterXHist1,
											dHistConv,
											nMaxHist1,
											dAvgPosX[nPosX]);
			if(nError)
				bFoundCtr = FALSE;
			else {
				if(nMaxHist1 > (nMaxHist/3)) {
					bFoundCtr = TRUE;
					if(bCheckCtr[0]) {
						if(dAvgPosX[nPosX] < rcBoundingRefCenter.left) 
							bFoundCtr = FALSE;
					}
					if(bCheckCtr[2]) {
						if(dAvgPosX[nPosX] > rcBoundingRefCenter.right)
							bFoundCtr = FALSE;
					}
				}
				if(bFoundCtr)
					nPosX++;
			}
		}
	}

	nPosY = 0;
	nError = AdvHistogram.GetMaxConvHist1D(&dCenterYHist[0], 
									nValidCenterNo,
									dHistConv,
									nMaxHist,
									dAvgPosY[nPosY]);
	if(nError)
		bFoundCtr = FALSE;
	else
		bFoundCtr = TRUE;
	if(bCheckCtr[1]) {
		if(dAvgPosY[nPosY] < rcBoundingRefCenter.top) 
			bFoundCtr = FALSE;
	}
	if(bCheckCtr[3]) {
		if(dAvgPosY[nPosY] > rcBoundingRefCenter.bottom)
			bFoundCtr = FALSE;
	}
	if(bFoundCtr)
		nPosY++;
	if(nMaxHist < 120) {
		dMin = dAvgPosY[0] - dHistConv/2;
		dMax = dAvgPosY[0] + dHistConv/2;
		nCenterYHist1 = 0;
		for(i=0;i<nValidCenterNo;i++) {
			if((dCenterYHist[i] < dMin) || (dCenterYHist[i] > dMax))
				dCenterYHist1[nCenterYHist1++] = dCenterYHist[i];
		}
		if(nCenterYHist1 > __max(1,nValidCenterNo/10)) {
			nError = AdvHistogram.GetMaxConvHist1D(&dCenterYHist1[0], 
											nCenterYHist1,
											dHistConv,
											nMaxHist1,
											dAvgPosY[nPosY]);
			if(nError)
				bFoundCtr = FALSE;
			else
			{
				if(nMaxHist1 > (nMaxHist/3)) {
					bFoundCtr = TRUE;
					if(bCheckCtr[1]) {
						if(dAvgPosY[nPosY] < rcBoundingRefCenter.top) 
							bFoundCtr = FALSE;
					}
					if(bCheckCtr[3]) {
						if(dAvgPosY[nPosY] > rcBoundingRefCenter.bottom)
							bFoundCtr = FALSE;
					}
					if(bFoundCtr)
						nPosY++;
				}
			}
		}
	}
	nCenterAryNo = 0;
	for(i=0;i<nPosX;i++) {
		for(j=0;j<nPosY;j++) {
			pt2DCenterAry[nCenterAryNo++] = CPoint2D(dAvgPosX[i], dAvgPosY[j]);
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::FindCircleCenter4");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CCircleInfo::ModifySegmentPt(int nRadiusDifTol)
{
	int i;
try {
	for(i=0;i<360;i++) {
		if(fabs(dSegmentRadius[i] - dRadius) > nRadiusDifTol) 
		{
			ptSegmentEdge[i] = CPoint(Round(pt2DCenter.x + dRadius * cos(i*PI/180)),
				Round(pt2DCenter.y + dRadius * sin(i*PI/180)));
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::ModifySegmentPt");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CCircleInfo::EdgeInterpolation()
{
	int i, j;
	CPoint ptInterpol[720];
	int nFirstIndex;
	int nlastIndex;
	int nInterpol[720];
	int nUnwrapnFirstIndex;
	int nStart;
	int nEnd;
	CPoint ptStart;
	CPoint ptEnd;
try {
	nFirstIndex = -1;
	for(i=0;i<360;i++) {
		if(pbSegmentEdge[i]) {
			nFirstIndex = i;
			break;
		}
	}
	nlastIndex = -1;
	for(i=359;i>=0;i--) {
		if(pbSegmentEdge[i]) {
			nlastIndex = i;
			break;
		}
	}
	if((nFirstIndex < 0) || (nlastIndex < 0))
		goto ErrorAbort;
	for(i=0;i<720;i++) 
		nInterpol[i] = 0;
	for(i=0;i<360;i++) {
		if(pbSegmentEdge[i])
			nInterpol[i] = 1;
		ptInterpol[i] = ptSegmentEdge[i];
	}
	nUnwrapnFirstIndex = 360 + nFirstIndex + 1;
	nInterpol[nUnwrapnFirstIndex-1] = 1;
	ptInterpol[nUnwrapnFirstIndex-1] = ptSegmentEdge[nFirstIndex];
	for(i=nFirstIndex;i<nUnwrapnFirstIndex;i++) {
		if(nInterpol[i] == 1)
			continue;
		nStart = i;
		nEnd = i;
		for(j=nStart;j<nUnwrapnFirstIndex;j++) {
			if(nInterpol[j] == 1) {
				nEnd = j;
				break;
			}
		}
		ptStart = ptInterpol[nStart-1];
		ptEnd = ptInterpol[nEnd];
		if(nStart == nEnd) 
			continue;
		if((nEnd - nStart) > 5) {
			for(j=nStart;j<nEnd;j++) 
				nInterpol[j] = -1;
			i = nEnd;
			continue;
		}
		if(nEnd <= 360) {
			for(j=nStart;j<nEnd;j++) {
				nInterpol[j] = 1;
				ptInterpol[j] = CPoint(ptStart.x + (ptEnd.x - ptStart.x) * (j-nStart+1)/(nEnd - nStart + 1),
										  ptStart.y + (ptEnd.y - ptStart.y) * (j-nStart+1)/(nEnd - nStart + 1));
			}
		}
		else {
			for(j=nStart;j<nEnd;j++) {
				if(j < 360)
					nInterpol[j] = 1;
				else
					nInterpol[j] = 2;
				ptInterpol[j] = CPoint(ptStart.x + (ptEnd.x - ptStart.x) * (j-nStart+1)/(nEnd - nStart + 1),
										  ptStart.y + (ptEnd.y - ptStart.y) * (j-nStart+1)/(nEnd - nStart + 1));
			}
		}
		i = nEnd;
	}
	for(i=0;i<360;i++) {
		if(nInterpol[i] == 1) { 
			pbSegmentEdge[i] = TRUE;
			ptSegmentEdge[i] = ptInterpol[i];
		}
		if(nInterpol[i] == 2) { 
			pbSegmentEdge[i] = TRUE;
			ptSegmentEdge[i] = ptInterpol[i+360];
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::EdgeInterpolation");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}



int CCircleInfo::SegmentEdgeNoSampling(CPoint* pptContour,
				int nPtContourNo,
				CPoint2D* ppt2DRoughCenter,
				CArray<CPoint,CPoint>* pAryPtContourSortingByAngle)
{
	int i;
	int nAngle;
	int nx, ny;
	double dRatio;
	int nCenterX;
	int nCenterY;
	int nAccumulationSegment;
	int nCircleMissingAngle;
//	int nStepNo;
	int nRadiusSqr[360];
	int nNewRadiusSqr;
	double* pdAngle;
	int* pnSorting;

try {
	pdAngle = NULL;
	pnSorting = NULL;
	if(nPtContourNo <= 0)
		goto ErrorAbort;
	pdAngle = new double[abs(nPtContourNo)];
	pnSorting = new int[abs(nPtContourNo)];
	nCenterX = Round(ppt2DRoughCenter->x);
	nCenterY = Round(ppt2DRoughCenter->y);
	dRatio = 180/PI;
	if(nEdgeProcMode > 0)
	{
		for(i=0;i<360;i++) {
			ptSegmentEdge[i] = CPoint(-10000,-10000);
			pbSegmentEdge[i] = FALSE;
			nRadiusSqr[i] = -1;
		}
		for(i=0;i<nPtContourNo;i++) {
			nx = pptContour[i].x - nCenterX;
			ny = pptContour[i].y - nCenterY;
			pdAngle[i] = atan2((double) ny, (double) nx);
			nAngle = Round(pdAngle[i] * dRatio);
			if(nAngle < 0)
				nAngle += 360;
			if(nAngle >= 360)
				nAngle -= 360;

			if((nAngle < 0) || (nAngle >= 360))
				continue;
				nNewRadiusSqr = nx * nx + ny * ny;
				if(nRadiusSqr[nAngle] < nNewRadiusSqr) {
					nRadiusSqr[nAngle] = nNewRadiusSqr;
					ptSegmentEdge[nAngle] = CPoint(nx, ny);
				}
			/*	break;
			}*/
			pbSegmentEdge[nAngle] = TRUE;
		}
	}
	else
	{
		for(i=0;i<360;i++) {
			ptSegmentEdge[i] = CPoint(-10000,-10000);
			pbSegmentEdge[i] = FALSE;
			nRadiusSqr[i] = 10000000;
		}
		for(i=0;i<nPtContourNo;i++) {
			nx = pptContour[i].x - nCenterX;
			ny = pptContour[i].y - nCenterY;
			pdAngle[i] = atan2((double) ny, (double) nx);
			nAngle = Round(pdAngle[i] * dRatio);
			if(nAngle < 0)
				nAngle += 360;
			if(nAngle >= 360)
				nAngle -= 360;

			if((nAngle < 0) || (nAngle >= 360))
				continue;
				nNewRadiusSqr = nx * nx + ny * ny;
				if(nRadiusSqr[nAngle] > nNewRadiusSqr) {
					nRadiusSqr[nAngle] = nNewRadiusSqr;
					ptSegmentEdge[nAngle] = CPoint(nx, ny);
				}
			/*	break;
			}*/
			pbSegmentEdge[nAngle] = TRUE;
		}
	}
	Sorting(&pdAngle[0],nPtContourNo,&pnSorting[0],TRUE);
	pAryPtContourSortingByAngle->RemoveAll();
	for(i=0;i<nPtContourNo;i++) {
		pAryPtContourSortingByAngle->Add(pptContour[pnSorting[i]]);
	}
	nCircleMissingAngle = 0;
	nAccumulationSegment = 0;
	for(i=0;i<360;i++) {
		if(pbSegmentEdge[i]) {
			if(nAccumulationSegment > 5)
				nCircleMissingAngle += nAccumulationSegment;
			nAccumulationSegment = 0;
		}
		else {
			nAccumulationSegment++;
		}
	}
	if(nAccumulationSegment > 5)
		nCircleMissingAngle += nAccumulationSegment;
	nCircleMissingPercentage = nCircleMissingAngle * 100/360;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::SegmentEdgeNoSampling");
	goto ErrorAbort;
}
	if(pdAngle != NULL)
		delete [] pdAngle;
	if(pnSorting != NULL)
		delete [] pnSorting;
	return 0;
ErrorAbort:
	if(pdAngle != NULL)
		delete [] pdAngle;
	if(pnSorting != NULL)
		delete [] pnSorting;
	return -1;
}


int CCircleInfo::SegmentEdgeNoSampling(CPoint* pptContour,
				int nPtContourNo,
				CPoint2D* ppt2DRoughCenter)
{
	int i;
	int nAngle;
	int nx, ny;
	double dRatio;
	int nCenterX;
	int nCenterY;
	int nAccumulationSegment;
	int nCircleMissingAngle;
//	int nStepNo;
	int nRadiusSqr[360];
	int nNewRadiusSqr;
	double* pdAngle;
	int* pnSorting;

try {
	pdAngle = NULL;
	pnSorting = NULL;
	if(nPtContourNo <= 0)
		goto ErrorAbort;
	pdAngle = new double[abs(nPtContourNo)];
	pnSorting = new int[abs(nPtContourNo)];
	nCenterX = Round(ppt2DRoughCenter->x);
	nCenterY = Round(ppt2DRoughCenter->y);
	dRatio = 180/PI;
	if(nEdgeProcMode > 0)
	{
		for(i=0;i<360;i++) {
			ptSegmentEdge[i] = CPoint(-10000,-10000);
			pbSegmentEdge[i] = FALSE;
			nRadiusSqr[i] = -1;
		}
		for(i=0;i<nPtContourNo;i++) {
			nx = pptContour[i].x - nCenterX;
			ny = pptContour[i].y - nCenterY;
			pdAngle[i] = atan2((double) ny, (double) nx);
			nAngle = Round(pdAngle[i] * dRatio);
			if(nAngle < 0)
				nAngle += 360;
			if(nAngle >= 360)
				nAngle -= 360;

			if((nAngle < 0) || (nAngle >= 360))
				continue;
				nNewRadiusSqr = nx * nx + ny * ny;
				if(nRadiusSqr[nAngle] < nNewRadiusSqr) {
					nRadiusSqr[nAngle] = nNewRadiusSqr;
					ptSegmentEdge[nAngle] = CPoint(nx, ny);
				}
			/*	break;
			}*/
			pbSegmentEdge[nAngle] = TRUE;
		}
	}
	else
	{
		for(i=0;i<360;i++) {
			ptSegmentEdge[i] = CPoint(-10000,-10000);
			pbSegmentEdge[i] = FALSE;
			nRadiusSqr[i] = 10000000;
		}
		for(i=0;i<nPtContourNo;i++) {
			nx = pptContour[i].x - nCenterX;
			ny = pptContour[i].y - nCenterY;
			pdAngle[i] = atan2((double) ny, (double) nx);
			nAngle = Round(pdAngle[i] * dRatio);
			if(nAngle < 0)
				nAngle += 360;
			if(nAngle >= 360)
				nAngle -= 360;

			if((nAngle < 0) || (nAngle >= 360))
				continue;
				nNewRadiusSqr = nx * nx + ny * ny;
				if(nRadiusSqr[nAngle] > nNewRadiusSqr) {
					nRadiusSqr[nAngle] = nNewRadiusSqr;
					ptSegmentEdge[nAngle] = CPoint(nx, ny);
				}
			/*	break;
			}*/
			pbSegmentEdge[nAngle] = TRUE;
		}
	}
	nCircleMissingAngle = 0;
	nAccumulationSegment = 0;
	for(i=0;i<360;i++) {
		if(pbSegmentEdge[i]) {
			if(nAccumulationSegment > 5)
				nCircleMissingAngle += nAccumulationSegment;
			nAccumulationSegment = 0;
		}
		else {
			nAccumulationSegment++;
		}
	}
	if(nAccumulationSegment > 5)
		nCircleMissingAngle += nAccumulationSegment;
	nCircleMissingPercentage = nCircleMissingAngle * 100/360;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::SegmentEdgeNoSampling");
	goto ErrorAbort;
}
	if(pdAngle != NULL)
		delete [] pdAngle;
	if(pnSorting != NULL)
		delete [] pnSorting;
	return 0;
ErrorAbort:
	if(pdAngle != NULL)
		delete [] pdAngle;
	if(pnSorting != NULL)
		delete [] pnSorting;
	return -1;
}


int CCircleInfo::SegmentEdge(CPoint* pptContour,
				int nPtContourNo,
				CPoint2D* ppt2DRoughCenter)
{
	int i, j;
	int nStart, nEnd;
	int nStep;
	double dStep;
	int lSumX;
	int lSumY;
	int nAngle;
	int nx, ny;
	double dRatio;
	int nCenterX;
	int nCenterY;
	int nAccumulationSegment;
	int nCircleMissingAngle;
	int nStepNo;
	int nRadiusSqr[360];
	int nRadiusSqrDifMin[360];
	int nNewRadiusSqr;
	BOOL bContourConnected;
	int nNominalRadiusSqr;
	int nRadiusSqrDif;
	int nError;

try {
	nCenterX = Round(ppt2DRoughCenter->x);
	nCenterY = Round(ppt2DRoughCenter->y);
	dRatio = 180/PI;
	nError = CheckPtConnection(pptContour,
						nPtContourNo,
						&bContourConnected);
	if(nError)
			goto ErrorAbort;
	if(bContourConnected) {
		dStep = 0.5 * (2 * PI * nEstimatedRadius) / 360;
		if(dStep < 1)
			dStep = 1;
	}
	else 
		dStep = 1;
	nNominalRadiusSqr = nEstimatedRadius * nEstimatedRadius;
	nStep = Round(dStep);
	nStepNo = Round(nPtContourNo/dStep);
	for(i=0;i<360;i++) {
		ptSegmentEdge[i] = CPoint(-10000,-10000);
		pbSegmentEdge[i] = FALSE;
		nRadiusSqr[i] = -1;
		nRadiusSqrDifMin[i] = INT_MAX;
	}
	if(nEdgeProcMode == -1) 
		for(i=0;i<360;i++) 
			nRadiusSqr[i] =  INT_MAX;
	else
		for(i=0;i<360;i++) 
			nRadiusSqr[i] =  -1;

	for(i=0;i<nStepNo;i++) {
		nStart = Round(i * dStep);
		nEnd = nStart + nStep;
		if(nEnd >= nPtContourNo) {
			nEnd = nPtContourNo;
			nStart = nPtContourNo - nStep;
		}
		lSumX = 0;
		lSumY = 0;
		for(j=nStart;j<nEnd;j++) {
			lSumX += pptContour[j].x;
			lSumY += pptContour[j].y;
		}
		nx = lSumX/nStep - nCenterX;
		ny = lSumY/nStep - nCenterY;
		nAngle = Round(atan2((double) ny, (double) nx) * dRatio);
		if(nAngle < 0)
			nAngle += 360;
		if(nAngle >= 360)
			nAngle -= 360;

		if((nAngle < 0) || (nAngle >= 360))
			continue;
		nNewRadiusSqr = nx * nx + ny * ny;
		switch (nEdgeProcMode) {
		case -1:
			if(nRadiusSqr[nAngle] > nNewRadiusSqr) {
				nRadiusSqr[nAngle] = nNewRadiusSqr;
				ptSegmentEdge[nAngle] = CPoint(nx, ny);
			}
			break;
		case 0:
			nRadiusSqrDif = abs(nNewRadiusSqr - nNominalRadiusSqr);
			if(nRadiusSqrDifMin[nAngle] > nRadiusSqrDif) {
				nRadiusSqrDifMin[nAngle] = nRadiusSqrDif;
				ptSegmentEdge[nAngle] = CPoint(nx, ny);
			}
			break;
		case 1:
			if(nRadiusSqr[nAngle] < nNewRadiusSqr) {
				nRadiusSqr[nAngle] = nNewRadiusSqr;
				ptSegmentEdge[nAngle] = CPoint(nx, ny);
			}
			break;
		}
		pbSegmentEdge[nAngle] = TRUE;
	}

	nCircleMissingAngle = 0;
	nAccumulationSegment = 0;
	for(i=0;i<360;i++) {
		if(pbSegmentEdge[i]) {
			if(nAccumulationSegment > 5)
				nCircleMissingAngle += nAccumulationSegment;
			nAccumulationSegment = 0;
		}
		else {
			nAccumulationSegment++;
		}
	}
	if(nAccumulationSegment > 5)
		nCircleMissingAngle += nAccumulationSegment;
	nCircleMissingPercentage = nCircleMissingAngle * 100/360;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::SegmentEdge");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}



int CCircleInfo::CheckPtConnection(CPoint* pptContour,
					int nPtContourNo,
					BOOL* pbContourConnected)
{
	int i;
	int nCtrX, nCtrY;
	int nLft, nRht;
	int nTop, nBot;
	*pbContourConnected = TRUE;
try {
	nCtrX = rcBounding.CenterPoint().x;
	nCtrY = rcBounding.CenterPoint().y;
	nLft = rcBounding.right;
	nRht = -1;
	nTop =  rcBounding.bottom;
	nBot = -1;
	if(pptContour[0].x == nCtrX) {
		nTop =  pptContour[0].y;
		nBot = pptContour[0].y;
	}
	if(pptContour[0].y == nCtrY) {
		nLft =  pptContour[0].x;
		nRht = pptContour[0].x;
	}
	for(i=1;i<nPtContourNo;i++) {
		if((abs(pptContour[i].x - pptContour[i-1].x) + abs(pptContour[i].y - pptContour[i-1].y)) > 2) {
			*pbContourConnected = FALSE;
			break;
		}
		if(pptContour[i].x == nCtrX) {
			if(nTop >  pptContour[i].y)
				nTop =  pptContour[i].y;
			if(nBot < pptContour[i].y)
				nBot = pptContour[i].y;
		}
		if(pptContour[i].y == nCtrY) {
			if(nLft >  pptContour[i].x)
				nLft =  pptContour[i].x;
			if(nRht < pptContour[i].x)
				nRht = pptContour[i].x;
		}
	}
	if((nRht < 0) && (nBot < 0))
		*pbContourConnected = FALSE;
	if(nRht > 0) {
		if((nRht - nLft) < rcBounding.Width()/2) 
			*pbContourConnected = FALSE;
	}
	if(nBot > 0) {
		if((nBot - nTop) < rcBounding.Height()/2) 
			*pbContourConnected = FALSE;
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::CheckPtConnection");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}



int CCircleInfo::FindCenterByHorAndVerLine(CPoint* pptContour,
				int nPtContourNo)
{
	int i;
	int* pnSortingX;
	int* pnSortingY;
	int* pnSortingXIndex;
	int* pnSortingYIndex;
	CPoint2D* ppt2DLineCtr;
	int nStart;
	int nEnd;
	int nPos;
	int nPt2DLineCtrNo;
	int nLengthTol;
	CHoughTransform HoughTransform;
	double dNominalAngleRad;
	double dTolAngleRad;	
	double dAngleStep;	
	double dOnLineTol;
	CPoint2D *ppt2DOnLine;
	int nOnLineSize;
	CPoint2D *ppt2DNOTOnLine;
	int nNOTOnLineSize;
	CLine2D ln2DHor;
	CLine2D ln2DVer;
	int nCenterX;
	int nCenterY;
	long lRadiusSqrSum;
	int nError;
try {
	Clean();
	pnSortingX = NULL;
	pnSortingY = NULL;
	pnSortingXIndex = NULL;
	pnSortingYIndex = NULL;
	ppt2DLineCtr = NULL;
	ppt2DOnLine = NULL;
	ppt2DNOTOnLine = NULL;
	if(nPtContourNo <= 0)
		goto PASS;
	nRadiusNo = nPtContourNo;
	if(pnRadiusSqr != NULL)
		delete [] pnRadiusSqr;
	pnRadiusSqr = NULL;
	pnRadiusSqr = new int[abs(nPtContourNo)];
	pnSortingX = new int[abs(nPtContourNo)];
	pnSortingY = new int[abs(nPtContourNo)];
	pnSortingXIndex = new int[abs(nPtContourNo)];
	pnSortingYIndex = new int[abs(nPtContourNo)];
	ppt2DLineCtr = new CPoint2D[abs(nPtContourNo)];
	ppt2DOnLine = new CPoint2D[abs(nPtContourNo)];
	ppt2DNOTOnLine = new CPoint2D[abs(nPtContourNo)];
	for(i=0; i<nPtContourNo;i++) 
		pnSortingX[i] = pptContour[i].x;
	Sorting(&pnSortingX[0], nPtContourNo, &pnSortingXIndex[0], TRUE);
	nLengthTol = (pnSortingX[nPtContourNo-1] - pnSortingX[0])/3;
	for(i=0; i<nPtContourNo;i++) 
		pnSortingY[i] = pptContour[pnSortingXIndex[i]].y;
	nStart = pnSortingY[0];
	nEnd = pnSortingY[0];
	nPos = pnSortingX[0];
	nPt2DLineCtrNo = 0;
	for(i=1; i<nPtContourNo;i++) {
		if(pnSortingX[i] == nPos) {
			if(nStart > pnSortingY[i])
				nStart = pnSortingY[i];
			if(nEnd < pnSortingY[i])
				nEnd = pnSortingY[i];
		}
		else {
			if((nEnd - nStart) > nLengthTol) {
				ppt2DLineCtr[nPt2DLineCtrNo++] = CPoint2D(nPos, (nStart + nEnd)/2.0);
			}
			nPos = pnSortingX[i];
		}
	}	
	
	dNominalAngleRad = 0;
	dTolAngleRad = 5 * PI/180;	
	dAngleStep = 1 * PI/180;		
	dOnLineTol = 5;
	nError = HoughTransform.HoughTransformLine(&ppt2DLineCtr[0],
										nPt2DLineCtrNo,
										dNominalAngleRad,
										dTolAngleRad,	
										dAngleStep,		
										dOnLineTol,
										&ppt2DOnLine[0],
										nOnLineSize,
										&ppt2DNOTOnLine[0],
										nNOTOnLineSize);
	if(nError)
		goto ErrorAbort;
	ln2DHor.GetBestFittingLine(&ppt2DOnLine[0],
							   nOnLineSize);

	Sorting(&pnSortingY[0], nPtContourNo, &pnSortingYIndex[0], TRUE);
	for(i=0; i<nPtContourNo;i++) 
		pnSortingX[i] = pptContour[pnSortingYIndex[i]].x;
	nStart = pnSortingX[0];
	nEnd = pnSortingX[0];
	nPos = pnSortingX[0];
	nPt2DLineCtrNo = 0;
	for(i=1; i<nPtContourNo;i++) {
		if(pnSortingY[i] == nPos) {
			if(nStart > pnSortingX[i])
				nStart = pnSortingX[i];
			if(nEnd < pnSortingX[i])
				nEnd = pnSortingX[i];
		}
		else {
			if((nEnd - nStart) > nLengthTol) {
				ppt2DLineCtr[nPt2DLineCtrNo++] = CPoint2D((nStart + nEnd)/2.0, nPos);
			}
			nPos = pnSortingY[i];
		}
	}	
	
	dNominalAngleRad = 90 * PI/180;
	dTolAngleRad = 5 * PI/180;	
	dAngleStep = 1 * PI/180;		
	dOnLineTol = 5;
	nError = HoughTransform.HoughTransformLine(&ppt2DLineCtr[0],
										nPt2DLineCtrNo,
										dNominalAngleRad,
										dTolAngleRad,	
										dAngleStep,		
										dOnLineTol,
										&ppt2DOnLine[0],
										nOnLineSize,
										&ppt2DNOTOnLine[0],
										nNOTOnLineSize);
	if(nError)
		goto ErrorAbort;
	ln2DVer.GetBestFittingLine(&ppt2DOnLine[0],
							   nOnLineSize);
	ln2DHor.GetIntercept(&ln2DVer, &pt2DCenter);
	nCenterX = Round(pt2DCenter.x);
	nCenterY = Round(pt2DCenter.y);
	lRadiusSqrSum = 0;
	for(i=0;i<nPtContourNo;i++) {
		pnRadiusSqr[i] = (pnSortingX[i] - nCenterX) * (pnSortingX[i] - nCenterX) + 
						 (pnSortingY[i] - nCenterY) * (pnSortingY[i] - nCenterY);
		lRadiusSqrSum += pnRadiusSqr[i];
	}
	dRadius = sqrt((double) (lRadiusSqrSum/nPtContourNo));

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleInfo::FindCenterByHorAndVerLine");
	goto ErrorAbort;
}
PASS:
	if(pnSortingX != NULL)
		delete [] pnSortingX;
	if(pnSortingY != NULL)
		delete [] pnSortingY;
	if(pnSortingXIndex != NULL)
		delete [] pnSortingXIndex;
	if(pnSortingYIndex != NULL)
		delete [] pnSortingYIndex;
	if(ppt2DLineCtr != NULL)
		delete [] ppt2DLineCtr;
	if(ppt2DOnLine != NULL)
		delete [] ppt2DOnLine;
	if(ppt2DNOTOnLine != NULL)
		delete [] ppt2DNOTOnLine;
	return 0;
ErrorAbort:
	Clean();
	if(pnSortingX != NULL)
		delete [] pnSortingX;
	if(pnSortingY != NULL)
		delete [] pnSortingY;
	if(pnSortingXIndex != NULL)
		delete [] pnSortingXIndex;
	if(pnSortingYIndex != NULL)
		delete [] pnSortingYIndex;
	if(ppt2DLineCtr != NULL)
		delete [] ppt2DLineCtr;
	if(ppt2DOnLine != NULL)
		delete [] ppt2DOnLine;
	if(ppt2DNOTOnLine != NULL)
		delete [] ppt2DNOTOnLine;
	return -1;
}

CScanColCenter::CScanColCenter()
{
	nColNo = 0;
}

CScanColCenter::~CScanColCenter()
{
	Clean();
}

void CScanColCenter::Clean()
{
	PtEdgeTopAry.RemoveAll();
	PtEdgeBotAry.RemoveAll();
	Pt2DEdgeCtrAry.RemoveAll();
	PtEdgeColIndexAry.RemoveAll();
	nColNo = 0;
}

int CScanColCenter::InspectWrap(CImageBuffer *pImg,
					CRect *prc,
					CRect* prcROI,
					BOOL bEdgeColor)
{
	int nError;
try
{	
	Clean();
	nColNo = prcROI->Width();
	if(nColNo <= 0)
		goto ErrorAbort;
	nError = GetHorLineCenterPoints(pImg,
							  prc,
							  prcROI,
							  bEdgeColor);
	if(nError)
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CScanColCenter::InspectWrap");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	Clean();
	return -1;
}

int CScanColCenter::GetColNumber()
{
	return nColNo;
}

int CScanColCenter::FindCtrByMinDistance(CPoint2D* ppt2DFittingCtr,
						 int* pnSelectedIndex,
						 BOOL* pbSelectCtr)
{
	int i;
	int nPosX;
	double dDis;
	double dMinDis;
	int nMinDisIndex;
	int nPtIndex;
	int nEdgeCtrColNo;

try
{
	*pbSelectCtr = FALSE;
	nPosX = Round(ppt2DFittingCtr->x);
	nPtIndex = PtEdgeColIndexAry[nPosX].x;
	nEdgeCtrColNo = PtEdgeColIndexAry[nPosX].y - nPtIndex;
	if((nPosX < 0) || (nPosX > nColNo))
		goto PASS;
	if(nEdgeCtrColNo <= 0)
		goto PASS;
	*pbSelectCtr = TRUE;
	dMinDis = fabs(ppt2DFittingCtr->y - Pt2DEdgeCtrAry[nPtIndex].y);
	nMinDisIndex = 0;
	for(i=1;i<nEdgeCtrColNo;i++) {
		dDis = fabs(ppt2DFittingCtr->y - Pt2DEdgeCtrAry[nPtIndex+i].y);
		if(dMinDis > dDis) {
			dMinDis = dDis;
			nMinDisIndex = i;
		}
	}
	*pnSelectedIndex = nMinDisIndex;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CScanColCenter::FindCtrByMinDistance");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

int CScanColCenter::FindCtrByMaxDistance(CPoint2D* ppt2DFittingCtr,
						 int* pnSelectedIndex,
						 BOOL* pbSelectCtr)
{
	int i;
	int nPosX;
	double dDis;
	double dMaxDis;
	int nMaxDisIndex;
	int nPtIndex;
	int nEdgeCtrColNo;

try
{
	*pbSelectCtr = FALSE;
	nPosX = Round(ppt2DFittingCtr->x);
	nPtIndex = PtEdgeColIndexAry[nPosX].x;
	nEdgeCtrColNo = PtEdgeColIndexAry[nPosX].y - nPtIndex;
	if((nPosX < 0) || (nPosX > nColNo))
		goto PASS;
	if(nEdgeCtrColNo <= 0)
		goto PASS;
	*pbSelectCtr = TRUE;
	dMaxDis = fabs(ppt2DFittingCtr->y - Pt2DEdgeCtrAry[nPtIndex].y);
	nMaxDisIndex = 0;
	for(i=1;i<nEdgeCtrColNo;i++) {
		dDis = fabs(ppt2DFittingCtr->y - Pt2DEdgeCtrAry[nPtIndex+i].y);
		if(dMaxDis < dDis) {
			dMaxDis = dDis;
			nMaxDisIndex = i;
		}
	}
	*pnSelectedIndex = nMaxDisIndex;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CScanColCenter::FindCtrByMaxDistance");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

int CScanColCenter::GetHorLineCenterPoints(CImageBuffer* pImg,
						  CRect* prc,
						  CRect* prcROI,
						  BOOL bEdgeColor)
{
	int x,y;
	int nx;
	int nImgPitch;
	int nWidth,nHeight;
	BYTE *pImgPtr;
	int nStart;
	int nEnd;
	BOOL bFound;
	int nHeightSub1;
	long lIndex;
	int nStartPtNo;
	int nEndPtNo;

try
{
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
	pImgPtr = pImg->GetImgPtr();
	nHeightSub1 = nHeight - 1;
	if(bEdgeColor == WHITE) {
		nStartPtNo = 0;
		nEndPtNo = 0;
		for (x=prcROI->left; x<prcROI->right; x++) {
			nx = x;
			if(nx < 0)
				nx = nx + nWidth;
			if(nx >= nWidth)
				nx = nx - nWidth;
			lIndex = nImgPitch * prc->top + prc->left + nx;
			bFound = FALSE;
			for (y=0; y<nHeight; y++) {
				if(pImgPtr[lIndex] == 255) {
					if(bFound == FALSE) { 
						nStart = y;
						bFound = TRUE;
					}
					nEnd = y;
					if((bFound == TRUE) && (y == nHeightSub1)) {
						PtEdgeTopAry.Add(CPoint(nx, nStart));
						PtEdgeBotAry.Add(CPoint(nx, nEnd));
						Pt2DEdgeCtrAry.Add(CPoint2D(nx,(nStart + nEnd)/2.0+prc->top));
						nEndPtNo++;
						bFound = FALSE;
					}
				}
				else {
					if(bFound == TRUE) {
						PtEdgeTopAry.Add(CPoint(nx, nStart));
						PtEdgeBotAry.Add(CPoint(nx, nEnd));
						Pt2DEdgeCtrAry.Add(CPoint2D(nx,(nStart + nEnd)/2.0+prc->top));
						nEndPtNo++;
						bFound = FALSE;
					}
				}
				lIndex += nImgPitch;
			}
			PtEdgeColIndexAry.Add(CPoint(nStartPtNo,nEndPtNo));
			nStartPtNo = nEndPtNo;
		}
	}
	else {
		for (x=prcROI->left; x<prcROI->right; x++) {
			nx = x;
			if(nx < 0)
				nx = nx + nWidth;
			if(nx >= nWidth)
				nx = nx - nWidth;
			lIndex = nImgPitch * prc->top + prc->left + nx;
			bFound = FALSE;
			for (y=0; y<nHeight; y++) {
				if(pImgPtr[lIndex] == 0) {
					if(bFound == FALSE) { 
						nStart = y;
						bFound = TRUE;
					}
					nEnd = y;
					if((bFound == TRUE) && (y == nHeightSub1)) {
						PtEdgeTopAry.Add(CPoint(nx, nStart));
						PtEdgeBotAry.Add(CPoint(nx, nEnd));
						Pt2DEdgeCtrAry.Add(CPoint2D(nx,(nStart + nEnd)/2.0+prc->top));
						nEndPtNo++;
						bFound = FALSE;
					}
				}
				else {
					if(bFound == TRUE) {
						PtEdgeTopAry.Add(CPoint(nx, nStart));
						PtEdgeBotAry.Add(CPoint(nx, nEnd));
						Pt2DEdgeCtrAry.Add(CPoint2D(nx,(nStart + nEnd)/2.0+prc->top));
						nEndPtNo++;
						bFound = FALSE;
					}
				}
				lIndex += nImgPitch;
			}
			PtEdgeColIndexAry.Add(CPoint(nStartPtNo,nEndPtNo));
			nStartPtNo = nEndPtNo;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CScanColCenter::GetHorLineCenterPoints");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CScanColCenter::GetCenter(int nCol, 
					  int nIndex,
					  CPoint2D* ppt2DCenter)
{
	if((nCol < 0) || (nCol >= nColNo))
		goto ErrorAbort;
	if((nIndex < 0) || (nIndex >= (PtEdgeColIndexAry[nCol].y - PtEdgeColIndexAry[nCol].x)))
		goto ErrorAbort;
	*ppt2DCenter = Pt2DEdgeCtrAry[PtEdgeColIndexAry[nCol].x + nIndex];
	return 0;
ErrorAbort:
	return -1;
}


int CScanColCenter::GetTopPoint(int nCol, 
					  int nIndex,
					  CPoint* pptTop)
{
	if((nCol < 0) || (nCol >= nColNo))
		goto ErrorAbort;
	if((nIndex < 0) || (nIndex >= (PtEdgeColIndexAry[nCol].y - PtEdgeColIndexAry[nCol].x)))
		goto ErrorAbort;
	*pptTop = PtEdgeTopAry[PtEdgeColIndexAry[nCol].x + nIndex];
	return 0;
ErrorAbort:
	return -1;
}

int CScanColCenter::GetBotPoint(int nCol, 
					  int nIndex,
					  CPoint* pptBot)
{
	if((nCol < 0) || (nCol >= nColNo))
		goto ErrorAbort;
	if((nIndex < 0) || (nIndex >= (PtEdgeColIndexAry[nCol].y - PtEdgeColIndexAry[nCol].x)))
		goto ErrorAbort;
	*pptBot = PtEdgeBotAry[PtEdgeColIndexAry[nCol].x + nIndex];
	return 0;
ErrorAbort:
	return -1;
}


int CScanColCenter::GetTopMostPoint(int nCol, 
					  CPoint* pptTopMost)
{
	if((nCol < 0) || (nCol >= nColNo))
		goto ErrorAbort;
	if((PtEdgeColIndexAry[nCol].y - PtEdgeColIndexAry[nCol].x) <= 0)
		goto ErrorAbort;
	*pptTopMost = PtEdgeTopAry[PtEdgeColIndexAry[nCol].x];
	return 0;
ErrorAbort:
	return -1;
}

int CScanColCenter::GetBotMostPoint(int nCol, 
					  CPoint* pptBotMost)
{
	if((nCol < 0) || (nCol >= nColNo))
		goto ErrorAbort;
	if((PtEdgeColIndexAry[nCol].y - PtEdgeColIndexAry[nCol].x) <= 0)
		goto ErrorAbort;
	*pptBotMost = PtEdgeBotAry[PtEdgeColIndexAry[nCol].y - 1];
	return 0;
ErrorAbort:
	return -1;
}

int CScanColCenter::GetEdgeNumber(int nCol, int* pnColEdgeNo)
{
	*pnColEdgeNo = 0;
	if((nCol < 0) || (nCol >= nColNo))
		goto ErrorAbort;
	*pnColEdgeNo = PtEdgeColIndexAry[nCol].y - PtEdgeColIndexAry[nCol].x;
	return 0;
ErrorAbort:
	return -1;
}


/*
CScanRowCenter::CScanRowCenter()
{
	pppt2DEdgeCtr = NULL;
	pnEdgeCtrRowNo = NULL;
	nRowNo = 0;
}

CScanRowCenter::~CScanRowCenter()
{
	Clean();
}

void CScanRowCenter::Clean()
{
	int i;
	if(pppt2DEdgeCtr != NULL) {
		for(i=0;i<nRowNo;i++) {
			if(pppt2DEdgeCtr[i] != NULL)
				delete [] pppt2DEdgeCtr[i];
			pppt2DEdgeCtr[i] = NULL;
		}
		delete [] pppt2DEdgeCtr;
	}
	pppt2DEdgeCtr = NULL;
	if(pnEdgeCtrRowNo != NULL)
		delete [] pnEdgeCtrRowNo;
	pnEdgeCtrRowNo = NULL;
	nRowNo = 0;
}

int CScanRowCenter::Inspect(CImageBuffer *pImg,
					CRect *prc)
{
	int i;
	int nError;
try
{	
	Clean();
	nRowNo = prc->Height();
	if(nRowNo <= 0)
		goto ErrorAbort;
	pnEdgeCtrRowNo = new int[abs(nRowNo)];
	pppt2DEdgeCtr = new CPoint2D*[abs(nRowNo)];
	for(i=0;i<nRowNo;i++)
		pppt2DEdgeCtr[i] = NULL;
	for(i=0;i<nRowNo;i++)
		pppt2DEdgeCtr[i] = new CPoint2D[abs(prc->Width())];
	nError = GetVerLineCenterPoints2(pImg,
							  prc,
							  pppt2DEdgeCtr,
							  pnEdgeCtrRowNo);
	if(nError)
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CScanRowCenter::Inspect");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	Clean();
	return -1;
}

int CScanRowCenter::GetCenter(int nRow, 
					  int nIndex,
					  CPoint2D* ppt2DCenter)
{
	if((nRow < 0) || (nRow >= nRowNo))
		goto ErrorAbort;
	if((nIndex < 0) || (nIndex >= pnEdgeCtrRowNo[nRow]))
		goto ErrorAbort;
	*ppt2DCenter = pppt2DEdgeCtr[nRow][nIndex];
	return 0;
ErrorAbort:
	return -1;
}

int CScanRowCenter::GetEdgeNumber(int nRow, int* pnRowEdgeNo)
{
	*pnRowEdgeNo = 0;
	if((nRow < 0) || (nRow >= nRowNo))
		goto ErrorAbort;
	*pnRowEdgeNo = pnEdgeCtrRowNo[nRow];
	return 0;
ErrorAbort:
	return -1;
}
*/

CScanRowCenter::CScanRowCenter()
{
	nRowNo = 0;
}

CScanRowCenter::~CScanRowCenter()
{
	Clean();
}

void CScanRowCenter::Clean()
{
	PtEdgeTopAry.RemoveAll();
	PtEdgeBotAry.RemoveAll();
	Pt2DEdgeCtrAry.RemoveAll();
	PtEdgeRowIndexAry.RemoveAll();
	nRowNo = 0;
}

int CScanRowCenter::InspectWrap(CImageBuffer *pImg,
					CRect *prc,
					CRect* prcROI,
					BOOL bEdgeColor)
{
	int nError;
	
	Clean();
	nRowNo = prcROI->Height();
	if(nRowNo <= 0)
		goto ErrorAbort;
	nError = GetVerLineCenterPoints(pImg,
							  prc,
							  prcROI,
							  bEdgeColor);
	return 0;
ErrorAbort:
	Clean();
	return -1;
}

int CScanRowCenter::Inspect(CImageBuffer *pImg,
					CRect *prc)
{
	int nError;
	nError = InspectWrap(pImg,
					prc,
					prc);
	if(nError)
		goto ErrorAbort;
	return 0;
ErrorAbort:
	return -1;
}
int CScanRowCenter::GetRowNumber()
{
	return nRowNo;
}

int CScanRowCenter::FindRightByMinDistance(CPoint* pptFitting,
						 int* pnSelectedIndex,
						 BOOL* pbSelectIndex)
{
	int i;
	int nPosY;
	double dDis;
	double dMinDis;
	int nMinDisIndex;
	int nPtIndex;
	int nEdgeCtrRowNo;

	*pbSelectIndex = FALSE;
	nPosY = Round(pptFitting->y);
	nPtIndex = PtEdgeRowIndexAry[nPosY].x;
	nEdgeCtrRowNo = PtEdgeRowIndexAry[nPosY].y - nPtIndex;
	if((nPosY < 0) || (nPosY > nRowNo))
		goto PASS;
	if(nEdgeCtrRowNo <= 0) 
		goto PASS;
	*pbSelectIndex = TRUE;
	dMinDis = fabs((double) (pptFitting->x - PtEdgeBotAry[nPtIndex].x));
	nMinDisIndex = 0;
	for(i=1;i<nEdgeCtrRowNo;i++) {
		dDis = fabs((double) (pptFitting->x - PtEdgeBotAry[nPtIndex+i].x));
		if(dMinDis > dDis) {
			dMinDis = dDis;
			nMinDisIndex = i;
		}
	}
	*pnSelectedIndex = nMinDisIndex;
PASS:
	return 0;
}

int CScanRowCenter::FindLeftByMinDistance(CPoint* pptFitting,
						 int* pnSelectedIndex,
						 BOOL* pbSelectIndex)
{
	int i;
	int nPosY;
	double dDis;
	double dMinDis;
	int nMinDisIndex;
	int nPtIndex;
	int nEdgeCtrRowNo;

	*pbSelectIndex = FALSE;
	nPosY = Round(pptFitting->y);
	nPtIndex = PtEdgeRowIndexAry[nPosY].x;
	nEdgeCtrRowNo = PtEdgeRowIndexAry[nPosY].y - nPtIndex;
	if((nPosY < 0) || (nPosY > nRowNo))
		goto PASS;
	if(nEdgeCtrRowNo <= 0)
		goto PASS;
	*pbSelectIndex = TRUE;
	dMinDis = fabs((double) (pptFitting->x - PtEdgeTopAry[nPtIndex].x));
	nMinDisIndex = 0;
	for(i=1;i<nEdgeCtrRowNo;i++) {
		dDis = fabs((double) (pptFitting->x - PtEdgeTopAry[nPtIndex+i].x));
		if(dMinDis > dDis) {
			dMinDis = dDis;
			nMinDisIndex = i;
		}
	}
	*pnSelectedIndex = nMinDisIndex;
PASS:
	return 0;
}

int CScanRowCenter::FindCtrByMinDistance(CPoint2D* ppt2DFittingCtr,
						 int* pnSelectedIndex,
						 BOOL* pbSelectCtr)
{
	int i;
	int nPosY;
	double dDis;
	double dMinDis;
	int nMinDisIndex;
	int nPtIndex;
	int nEdgeCtrRowNo;

	*pbSelectCtr = FALSE;
	nPosY = Round(ppt2DFittingCtr->y);
	nPtIndex = PtEdgeRowIndexAry[nPosY].y;
	nEdgeCtrRowNo = PtEdgeRowIndexAry[nPosY].y - nPtIndex;
	if((nPosY < 0) || (nPosY > nRowNo))
		goto PASS;
	if(nEdgeCtrRowNo <= 0)
		goto PASS;
	*pbSelectCtr = TRUE;
	dMinDis = fabs(ppt2DFittingCtr->x - Pt2DEdgeCtrAry[nPtIndex].x);
	nMinDisIndex = 0;
	for(i=1;i<nEdgeCtrRowNo;i++) {
		dDis = fabs(ppt2DFittingCtr->x - Pt2DEdgeCtrAry[nPtIndex+i].x);
		if(dMinDis > dDis) {
			dMinDis = dDis;
			nMinDisIndex = i;
		}
	}
	*pnSelectedIndex = nMinDisIndex;
PASS:
	return 0;
}

int CScanRowCenter::FindCtrByMaxDistance(CPoint2D* ppt2DFittingCtr,
						 int* pnSelectedIndex,
						 BOOL* pbSelectCtr)
{
	int i;
	int nPosX;
	double dDis;
	double dMaxDis;
	int nMaxDisIndex;
	int nPtIndex;
	int nEdgeCtrRowNo;

	*pbSelectCtr = FALSE;
	nPosX = Round(ppt2DFittingCtr->x);
	nPtIndex = PtEdgeRowIndexAry[nPosX].x;
	nEdgeCtrRowNo = PtEdgeRowIndexAry[nPosX].y - nPtIndex;
	if((nPosX < 0) || (nPosX > nRowNo))
		goto PASS;
	if(nEdgeCtrRowNo <= 0)
		goto PASS;
	*pbSelectCtr = TRUE;
	dMaxDis = fabs(ppt2DFittingCtr->y - Pt2DEdgeCtrAry[nPtIndex].y);
	nMaxDisIndex = 0;
	for(i=1;i<nEdgeCtrRowNo;i++) {
		dDis = fabs(ppt2DFittingCtr->y - Pt2DEdgeCtrAry[nPtIndex+i].y);
		if(dMaxDis < dDis) {
			dMaxDis = dDis;
			nMaxDisIndex = i;
		}
	}
	*pnSelectedIndex = nMaxDisIndex;
PASS:
	return 0;
}

int CScanRowCenter::GetVerLineCenterPoints(CImageBuffer* pImg,
						  CRect* prc,
						  CRect* prcROI,
						  BOOL bEdgeColor)
{
	int x,y;
	int ny;
	int nImgPitch;
	int nWidth,nHeight;
	BYTE *pImgPtr;
	int nStart;
	int nEnd;
	BOOL bFound;
	int nWidthSub1;
	long lIndex;
	int nStartPtNo;
	int nEndPtNo;

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
	pImgPtr = pImg->GetImgPtr();
	nWidthSub1 = nWidth - 1;
	if(bEdgeColor == WHITE) {
		nStartPtNo = 0;
		nEndPtNo = 0;
		lIndex = nImgPitch * prc->top + prc->left;
		for (y=prc->top; y<prc->bottom; y++) {
			ny = y;
			if(ny < 0)
				ny = ny + nHeight;
			if(ny >= nHeight)
				ny = ny - nHeight;
			bFound = FALSE;
			for (x=0; x<nWidth; x++) {
				if(pImgPtr[lIndex + x] == 255) {
					if(bFound == FALSE) { 
						nStart = x;
						bFound = TRUE;
					}
					nEnd = x;
					if((bFound == TRUE) && (x == nWidthSub1)) {
						PtEdgeTopAry.Add(CPoint(nStart, ny));
						PtEdgeBotAry.Add(CPoint(nEnd, ny));
						Pt2DEdgeCtrAry.Add(CPoint2D((nStart + nEnd)/2.0+prc->left,ny));
						nEndPtNo++;
						bFound = FALSE;
					}
				}
				else {
					if(bFound == TRUE) {
						PtEdgeTopAry.Add(CPoint(nStart, ny));
						PtEdgeBotAry.Add(CPoint(nEnd, ny));
						Pt2DEdgeCtrAry.Add(CPoint2D((nStart + nEnd)/2.0+prc->left, ny));
						nEndPtNo++;
						bFound = FALSE;
					}
				}
			}
			PtEdgeRowIndexAry.Add(CPoint(nStartPtNo,nEndPtNo));
			nStartPtNo = nEndPtNo;
			lIndex += nImgPitch;
		}
	}
	else {
		nStartPtNo = 0;
		nEndPtNo = 0;
		lIndex = nImgPitch * prc->top + prc->left;
		for (y=prc->top; y<prc->bottom; y++) {
			ny = y;
			if(ny < 0)
				ny = ny + nHeight;
			if(ny >= nHeight)
				ny = ny - nHeight;
			bFound = FALSE;
			for (x=0; x<nWidth; x++) {
				if(pImgPtr[lIndex + x] == 0) {
					if(bFound == FALSE) { 
						nStart = x;
						bFound = TRUE;
					}
					nEnd = x;
					if((bFound == TRUE) && (x == nWidthSub1)) {
						PtEdgeTopAry.Add(CPoint(nStart, ny));
						PtEdgeBotAry.Add(CPoint(nEnd, ny));
						Pt2DEdgeCtrAry.Add(CPoint2D((nStart + nEnd)/2.0+prc->left,ny));
						nEndPtNo++;
						bFound = FALSE;
					}
				}
				else {
					if(bFound == TRUE) {
						PtEdgeTopAry.Add(CPoint(nStart, ny));
						PtEdgeBotAry.Add(CPoint(nEnd, ny));
						Pt2DEdgeCtrAry.Add(CPoint2D((nStart + nEnd)/2.0+prc->left, ny));
						nEndPtNo++;
						bFound = FALSE;
					}
				}
			}
			PtEdgeRowIndexAry.Add(CPoint(nStartPtNo,nEndPtNo));
			nStartPtNo = nEndPtNo;
			lIndex += nImgPitch;
		}
	}

	return 0;
}


int CScanRowCenter::GetCenter(int nRow, 
					  int nIndex,
					  CPoint2D* ppt2DCenter)
{
	if((nRow < 0) || (nRow >= nRowNo))
		goto ErrorAbort;
	if((nIndex < 0) || (nIndex >= (PtEdgeRowIndexAry[nRow].y - PtEdgeRowIndexAry[nRow].x)))
		goto ErrorAbort;
	*ppt2DCenter = Pt2DEdgeCtrAry[PtEdgeRowIndexAry[nRow].x + nIndex];
	return 0;
ErrorAbort:
	return -1;
}


int CScanRowCenter::GetLeftPoint(int nRow, 
					  int nIndex,
					  CPoint* pptTop)
{
	if((nRow < 0) || (nRow >= nRowNo))
		goto ErrorAbort;
	if((nIndex < 0) || (nIndex >= (PtEdgeRowIndexAry[nRow].y - PtEdgeRowIndexAry[nRow].x)))
		goto ErrorAbort;
	*pptTop = PtEdgeTopAry[PtEdgeRowIndexAry[nRow].x + nIndex];
	return 0;
ErrorAbort:
	return -1;
}

int CScanRowCenter::GetRightPoint(int nRow, 
					  int nIndex,
					  CPoint* pptBot)
{
	if((nRow < 0) || (nRow >= nRowNo))
		goto ErrorAbort;
	if((nIndex < 0) || (nIndex >= (PtEdgeRowIndexAry[nRow].y - PtEdgeRowIndexAry[nRow].x)))
		goto ErrorAbort;
	*pptBot = PtEdgeBotAry[PtEdgeRowIndexAry[nRow].x + nIndex];
	return 0;
ErrorAbort:
	return -1;
}


int CScanRowCenter::GetLeftMostPoint(int nRow, 
					  CPoint* pptTopMost)
{
	if((nRow < 0) || (nRow >= nRowNo))
		goto ErrorAbort;
	if((PtEdgeRowIndexAry[nRow].y - PtEdgeRowIndexAry[nRow].x) <= 0)
		goto ErrorAbort;
	*pptTopMost = PtEdgeTopAry[PtEdgeRowIndexAry[nRow].x];
	return 0;
ErrorAbort:
	return -1;
}

int CScanRowCenter::GetRightMostPoint(int nRow, 
					  CPoint* pptBotMost)
{
	if((nRow < 0) || (nRow >= nRowNo))
		goto ErrorAbort;
	if((PtEdgeRowIndexAry[nRow].y - PtEdgeRowIndexAry[nRow].x) <= 0)
		goto ErrorAbort;
	*pptBotMost = PtEdgeBotAry[PtEdgeRowIndexAry[nRow].y - 1];
	return 0;
ErrorAbort:
	return -1;
}

int CScanRowCenter::GetEdgeNumber(int nRow, int* pnRowEdgeNo)
{
	*pnRowEdgeNo = 0;
	if((nRow < 0) || (nRow >= nRowNo))
		goto ErrorAbort;
	*pnRowEdgeNo = PtEdgeRowIndexAry[nRow].y - PtEdgeRowIndexAry[nRow].x;
	return 0;
ErrorAbort:
	return -1;
}



CCurvePointStatistics::CCurvePointStatistics()
{
	ppt2DHough = NULL;
	pbHough = NULL;
	nPtNoHough = 0;

}

CCurvePointStatistics::~CCurvePointStatistics()
{
	Clean();
}

void CCurvePointStatistics::Clean()
{
	if(ppt2DHough != NULL)
		delete [] ppt2DHough;
	ppt2DHough = NULL;
	if(pbHough != NULL)
		delete [] pbHough;
	pbHough = NULL;
	nPtNoHough = 0;
}


int CCurvePointStatistics::Inspect(CPoint2D* ppt2D,
					int* pnPercentage,
					int nPercentageTol,
					double dPosTol,
					int nPtNo)
{
	int i;
	int nError;
try {
	Clean();
	if(nPtNo < 0)
		goto ErrorAbort;
	nPtNoHough = nPtNo;
	ppt2DHough = new CPoint2D[abs(nPtNoHough)];
	pbHough = new BOOL[abs(nPtNoHough)];
	for(i=0;i<nPtNoHough;i++)
		pbHough[i] = FALSE;
	if(nPtNo < 4)
		goto ErrorAbort;
	nError = CalculateInterpolation(&ppt2D[0],
					&pnPercentage[0],
					nPercentageTol,
					dPosTol,
					nPtNo);
	if(nError)
		goto ErrorAbort;
	nError = EdgePtAndBadPtInterpolation(&ppt2D[0],
					&pnPercentage[0],
					nPercentageTol,
					dPosTol,
					nPtNo);
	if(nError)
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurvePointStatistics::Inspect");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CCurvePointStatistics::Inspect1(CPoint2D* ppt2D,
					int* pnPercentage,
					int nPercentageTol,
					CPoint2D* ppt2DDefectAvg,
					double dPosTol,
					int nPtNo)
{
	int i;
	CPoint2D* ppt2DGood;
	int nGoodNo;
	CInterpolationBy3Pts InterpolationBy3Pts;
	double dEstimatedY;
	int nLeftIndex;
	int nRightIndex;
	int nError;

try {
	Clean();
	ppt2DGood = NULL;
	if(nPtNo < 0)
		goto ErrorAbort;
	nPtNoHough = nPtNo;
	ppt2DHough = new CPoint2D[abs(nPtNoHough)];
	pbHough = new BOOL[abs(nPtNoHough)];
	for(i=0;i<nPtNoHough;i++)
		pbHough[i] = FALSE;
	if(nPtNo < 4)
		goto ErrorAbort;

	ppt2DGood = new CPoint2D[abs(nPtNoHough + 1)];
	nGoodNo = 0;
	for(i=0;i<nPtNoHough;i++) {
		if(pnPercentage[i] < nPercentageTol) 
			continue;
		ppt2DGood[nGoodNo++] = ppt2D[i];
	}

	nError = InterpolationBy3Pts.SetPoint2DAry(&ppt2DGood[0],
					  nGoodNo);
	if(nError)
		goto ErrorAbort;
	if(ppt2DDefectAvg->x > 0) {
		nLeftIndex = -1;
		for(i=0;i<nGoodNo;i++) {
			if(ppt2DGood[i].x > ppt2DDefectAvg->x)
				break;
			nLeftIndex = i;
		}
		nRightIndex = -1;
		for(i=nGoodNo-1;i>=0;i--) {
			if(ppt2DGood[i].x < ppt2DDefectAvg->x)
				break;
			nRightIndex = i;
		}
		if((nLeftIndex >= 0) && (nRightIndex>=0)) {
			InterpolationBy3Pts.GetY(ppt2DDefectAvg->x, &dEstimatedY);
			if((dEstimatedY < ppt2DDefectAvg->y) && 
			   (ppt2DGood[nLeftIndex].y < ppt2DDefectAvg->y) &&
			   (ppt2DGood[nRightIndex].y < ppt2DDefectAvg->y)) {
				ppt2DGood[nGoodNo++] = *ppt2DDefectAvg;
				nError = InterpolationBy3Pts.SetPoint2DAry(&ppt2DGood[0],
								  nGoodNo);
				if(nError)
					goto ErrorAbort;
			}
		}
	}
	for(i=0;i<nPtNoHough;i++) {
		if(pnPercentage[i] > nPercentageTol) 
			ppt2DHough[i] = ppt2D[i];
		else {
			ppt2DHough[i] = ppt2D[i];
			InterpolationBy3Pts.GetY(ppt2DHough[i].x, &(ppt2DHough[i].y));
		}
	}
/*	nError = CalculateInterpolation(&ppt2D[0],
					&pnPercentage[0],
					nPercentageTol,
					dPosTol,
					nPtNo);
	if(nError)
		goto ErrorAbort;
*/
/*	nError = EdgePtAndBadPtInterpolation(&ppt2D[0],
					&pnPercentage[0],
					nPercentageTol,
					dPosTol,
					nPtNo);
	if(nError)
		goto ErrorAbort;
*/
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurvePointStatistics::Inspect1");
	goto ErrorAbort;
}
	if(ppt2DGood != NULL)
		delete [] ppt2DGood;
	return 0;
ErrorAbort:
	if(ppt2DGood != NULL)
		delete [] ppt2DGood;
	return -1;
}



int CCurvePointStatistics::EdgePtAndBadPtInterpolation(CPoint2D* ppt2D,
					int* pnPercentage,
					int nPercentageTol,
					double dPosTol,
					int nPtNo)
{
	int i, j;
	int nSumPtNoHough;
	int* pnIndexLft;
	int nIndexLft;
	int* pnIndexRht;
	int nIndexRht;
	CPoint2DAndInt Point2DAndInt0;
	CPoint2DAndInt Point2DAndInt1;
	CPoint2DAndInt Point2DAndInt2;
	CPoint2DAndInt Point2DAndIntInsp;
	BOOL bFound;
	int nError;

try {
	pnIndexLft = NULL;
	pnIndexRht = NULL;
	pnIndexLft = new int[abs(nPtNoHough)];
	pnIndexRht = new int[abs(nPtNoHough)];
	nSumPtNoHough = 0;
	for(i=0;i<nPtNoHough;i++) {
		if(pbHough[i])
			nSumPtNoHough++;
	}
	if(nSumPtNoHough < 3) {
		nSumPtNoHough = 0;
		for(i=0;i<nPtNoHough;i++) {
			if(pnPercentage[i] > nPercentageTol) {
				pbHough[i] = TRUE;
				nSumPtNoHough++;
			}
		}
		if(nSumPtNoHough < 3)
			goto ErrorAbort;
	}
	for(i=0;i<nPtNoHough;i++) {
		if(pbHough[i])
			continue;
		Point2DAndIntInsp.pt2DPos = ppt2D[i];
		nIndexLft = 0;
		nIndexRht = 0;
		bFound = FALSE;
		for(j=i-1;j>=0;j--) {
			if(pbHough[j]) {
				pnIndexLft[nIndexLft++] = j;
			}
		}
		for(j=i+1;j<nPtNoHough;j++) {
			if(pbHough[j]) {
				pnIndexRht[nIndexRht++] = j;
			}
		}
		if((nIndexLft == 0) && (nIndexRht >= 3)) {
			Point2DAndInt0.pt2DPos = ppt2D[pnIndexRht[0]];
			Point2DAndInt1.pt2DPos = ppt2D[pnIndexRht[1]];
			Point2DAndInt2.pt2DPos = ppt2D[pnIndexRht[2]];
			bFound = TRUE;
		}
		if((nIndexLft >= 0) && (nIndexRht == 0)) {
			Point2DAndInt0.pt2DPos = ppt2D[pnIndexLft[0]];
			Point2DAndInt1.pt2DPos = ppt2D[pnIndexLft[1]];
			Point2DAndInt2.pt2DPos = ppt2D[pnIndexLft[2]];
			bFound = TRUE;
		}
		if((nIndexLft >= 1) && (nIndexRht >= 1)) {
			Point2DAndInt0.pt2DPos = ppt2D[pnIndexLft[0]];
			Point2DAndInt1.pt2DPos = ppt2D[pnIndexRht[0]];
			if((nIndexLft == 1)&& (nIndexRht >= 2))
				Point2DAndInt2.pt2DPos = ppt2D[pnIndexRht[1]];
			if((nIndexLft >= 2)&& (nIndexRht == 1))
				Point2DAndInt2.pt2DPos = ppt2D[pnIndexLft[1]];
			if((nIndexLft >= 2)&& (nIndexRht >= 2)) {
				if(fabs(ppt2D[pnIndexLft[1]].x - Point2DAndIntInsp.pt2DPos.x) > 
				   fabs(ppt2D[pnIndexRht[1]].x - Point2DAndIntInsp.pt2DPos.x)) 
					Point2DAndInt2.pt2DPos = ppt2D[pnIndexRht[1]];
				else
				Point2DAndInt2.pt2DPos = ppt2D[pnIndexLft[1]];
			}
			bFound = TRUE;
			if((nIndexLft == 1) && (nIndexRht == 1)) 
				bFound = FALSE;
		}
		if(bFound == FALSE)
			goto ErrorAbort;
		nError = InterpolationBy3Pts(&Point2DAndInt0,
						&Point2DAndInt1,
						&Point2DAndInt2,
						&Point2DAndIntInsp);
		if(nError)
			goto ErrorAbort;
		ppt2DHough[i] = Point2DAndIntInsp.pt2DPos;
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurvePointStatistics::EdgePtAndBadPtInterpolation");
	goto ErrorAbort;
}
	if(pnIndexLft != NULL)
		delete [] pnIndexLft;
	if(pnIndexRht != NULL)
		delete [] pnIndexRht;
	return 0;
ErrorAbort:
	if(pnIndexLft != NULL)
		delete [] pnIndexLft;
	if(pnIndexRht != NULL)
		delete [] pnIndexRht;
	return -1;
}



int CCurvePointStatistics::CalculateInterpolation(CPoint2D* ppt2D,
					int* pnPercentage,
					int nPercentageTol,
					double dPosTol,
					int nPtNo)
{
	int i, j, m, n;
	int* pnIndexLft;
	int nIndexLft;
	int* pnIndexRht;
	int nIndexRht;
	CPoint2DAndInt Point2DAndInt0;
	CPoint2DAndInt Point2DAndInt1;
	CPoint2DAndInt Point2DAndInt2;
	CPoint2DAndInt Point2DAndIntInsp;
	CAdvHistogram AdvHistogram;
	double* pdYPos;
	int nPos;
	int	nMaxHist;
	double dAvgYPos;
	int nSumPt;
	double dStartPosY;
	double dEndPosY;
	int nError;

try {
	pnIndexLft = NULL;
	pnIndexRht = NULL;
	pdYPos = NULL;
	pnIndexLft = new int[abs(nPtNoHough)];
	pnIndexRht = new int[abs(nPtNoHough)];
	pdYPos = new double[abs(nPtNoHough * nPtNoHough * nPtNoHough)];
	for(i=0;i<nPtNoHough;i++) {
		ppt2DHough[i] = ppt2D[i];
		if(pnPercentage[i] > nPercentageTol) 
			pbHough[i] = TRUE;
		else
			pbHough[i] = FALSE;
	}
	for(i=1;i<(nPtNoHough-1);i++) {
		pbHough[i] = FALSE;
		nIndexLft = 0;
		nIndexRht = 0;
		for(j=i-1;j>=0;j--) {
			if(pnPercentage[j] > nPercentageTol) {
				pnIndexLft[nIndexLft++] = j;
			}
		}
		for(j=i+1;j<nPtNoHough;j++) {
			if(pnPercentage[j] > nPercentageTol) {
				pnIndexRht[nIndexRht++] = j;
			}
		}
		if((nIndexLft == 0) ||
		   (nIndexRht == 0) ||
		   ((nIndexLft + nIndexRht) <= 2))
		   continue;
		Point2DAndIntInsp.pt2DPos = ppt2D[i];
		nPos = 0;
		if(pnPercentage[i] > nPercentageTol) 
			pdYPos[nPos++] = Point2DAndIntInsp.pt2DPos.y;
		for(j=0;j<nIndexLft;j++) {
			Point2DAndInt0.pt2DPos = ppt2D[pnIndexLft[j]];
			Point2DAndInt0.nVal = pnPercentage[pnIndexLft[j]];
			for(m=j+1;m<nIndexLft;m++) {
				Point2DAndInt1.pt2DPos = ppt2D[pnIndexLft[m]];
				Point2DAndInt1.nVal = pnPercentage[pnIndexLft[m]];
				for(n=0;n<nIndexRht;n++) {
					Point2DAndInt2.pt2DPos = ppt2D[pnIndexRht[n]];
					Point2DAndInt2.nVal = pnPercentage[pnIndexRht[n]];
					nError = InterpolationBy3Pts(&Point2DAndInt0,
									&Point2DAndInt1,
									&Point2DAndInt2,
									&Point2DAndIntInsp);
					if(nError)
						continue;
					pdYPos[nPos++] = Point2DAndIntInsp.pt2DPos.y;
				}
			}
		}

		for(j=0;j<nIndexRht;j++) {
			Point2DAndInt0.pt2DPos = ppt2D[pnIndexRht[j]];
			Point2DAndInt0.nVal = pnPercentage[pnIndexRht[j]];
			for(m=j+1;m<nIndexRht;m++) {
				Point2DAndInt1.pt2DPos = ppt2D[pnIndexRht[m]];
				Point2DAndInt1.nVal = pnPercentage[pnIndexRht[m]];
				for(n=0;n<nIndexLft;n++) {
					Point2DAndInt2.pt2DPos = ppt2D[pnIndexLft[n]];
					Point2DAndInt2.nVal = pnPercentage[pnIndexLft[n]];
					nError = InterpolationBy3Pts(&Point2DAndInt0,
									&Point2DAndInt1,
									&Point2DAndInt2,
									&Point2DAndIntInsp);
					if(nError)
						continue;
					pdYPos[nPos++] = Point2DAndIntInsp.pt2DPos.y;
				}
			}
		}
		dStartPosY = ppt2D[i].y - dPosTol/2;
		dEndPosY = ppt2D[i].y + dPosTol/2;
		nSumPt = 0;
		for(j=0;j<nPos;j++) {
			if((pdYPos[j] >= dStartPosY) && (pdYPos[j] <= dEndPosY))
				nSumPt++;
		} 
		nError = AdvHistogram.GetMaxConvHist1D(&pdYPos[0], 
									nPos,
									dPosTol,
									nMaxHist,
									dAvgYPos);
		if(nError == FALSE)
			pbHough[i] = TRUE;
		if(nSumPt > (nMaxHist/3))
			ppt2DHough[i] = ppt2D[i];
		else
			ppt2DHough[i] = CPoint2D(Point2DAndIntInsp.pt2DPos.x, dAvgYPos);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurvePointStatistics::CalculateInterpolation");
	goto ErrorAbort;
}
	if(pnIndexLft != NULL)
		delete [] pnIndexLft;
	if(pnIndexRht != NULL)
		delete [] pnIndexRht;
	if(pdYPos != NULL)
		delete [] pdYPos;
	return 0;
ErrorAbort:
	if(pnIndexLft != NULL)
		delete [] pnIndexLft;
	if(pnIndexRht != NULL)
		delete [] pnIndexRht;
	if(pdYPos != NULL)
		delete [] pdYPos;
	return -1;
}




int CCurvePointStatistics::InterpolationBy3Pts(CPoint2DAndInt* pPoint2DAndInt0,
									CPoint2DAndInt* pPoint2DAndInt1,
									CPoint2DAndInt* pPoint2DAndInt2,
									CPoint2DAndInt* pPoint2DAndIntInsp)
{
	double dx0, dy0;
	double dx1, dy1;
	double dx2, dy2;
	double dx, dy;

try {
	dx0 = pPoint2DAndInt0->pt2DPos.x;
	dy0 = pPoint2DAndInt0->pt2DPos.y;
	dx1 = pPoint2DAndInt1->pt2DPos.x;
	dy1 = pPoint2DAndInt1->pt2DPos.y;
	dx2 = pPoint2DAndInt2->pt2DPos.x;
	dy2 = pPoint2DAndInt2->pt2DPos.y;
	dx = pPoint2DAndIntInsp->pt2DPos.x;
	if((fabs(dx0-dx1) < 0.1) || 
	   (fabs(dx0-dx2) < 0.1) ||
	   (fabs(dx1-dx2) < 0.1))
	   goto ErrorAbort;
	dy = (dx - dx1)*(dx - dx2)/((dx0 - dx1) * (dx0 - dx2)) * dy0 +
		 (dx - dx0)*(dx - dx2)/((dx1 - dx0) * (dx1 - dx2)) * dy1 +
		 (dx - dx0)*(dx - dx1)/((dx2 - dx0) * (dx2 - dx1)) * dy2;
	pPoint2DAndIntInsp->pt2DPos.y = dy;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurvePointStatistics::InterpolationBy3Pts");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


CCurveFitting::CCurveFitting()
{
	ppt2DEdgeSorting = NULL;
	pnPt2DInSegment = NULL; 
	ppt2DSegmentCtr = NULL;
	nSegmentNo = 0;
	nSegmentCtrNo = 0;
	pln2DSegment = NULL;
	pnPercentageByLength = NULL;
	pnPercentageByAllPoints = NULL;
	prcSegment = NULL;
	bLineSegmentMethod = TRUE;
	dFOVZoomRatio = 1;
}

CCurveFitting::~CCurveFitting()
{
	Clean();
}

void CCurveFitting::Clean()
{
	int i;
	if(ppt2DEdgeSorting != NULL) {
		for(i=0;i<nSegmentNo;i++) {
			if(ppt2DEdgeSorting[i] != NULL)
				delete [] ppt2DEdgeSorting[i];
			ppt2DEdgeSorting[i] = NULL;
		}
		delete [] ppt2DEdgeSorting;
		if(pnPt2DInSegment != NULL)
			delete [] pnPt2DInSegment;
	}
	if(ppt2DSegmentCtr != NULL)
		delete [] ppt2DSegmentCtr;
	ppt2DEdgeSorting = NULL;
	pnPt2DInSegment = NULL;
	ppt2DSegmentCtr = NULL;
	nSegmentNo = 0;
	nSegmentCtrNo = 0;
	if(pln2DSegment != NULL)
		delete [] pln2DSegment;
	pln2DSegment = NULL;
	if(pnPercentageByLength != NULL)
		delete [] pnPercentageByLength;
	pnPercentageByLength = NULL;
	if(pnPercentageByAllPoints != NULL)
		delete [] pnPercentageByAllPoints;
	pnPercentageByAllPoints = NULL;
	if(prcSegment != NULL)
		delete [] prcSegment;
	prcSegment = NULL;
}



int CCurveFitting::GetMappingDebugImage(CImageBuffer* pImgSrc,
						CRect* prcSrc,
						CImageBuffer* pImgDst,
						CRect* prcDst)
{
	int i;
	double dy;
	int nError;
try {
	nError = ImgCopy(pImgSrc,
			prcSrc,
			pImgDst,
			prcDst);
	if(nError)
		goto ErrorAbort;
	for(i=prcSrc->left;i<prcSrc->right;i++) {
		nError = GetY(i, &dy);
		if(nError)
			goto ErrorAbort;
		pImgDst->SetPixelVal(i, Round(dy), 128);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurveFitting::GetMappingDebugImage");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CCurveFitting::CheckChipoff(CPoint2D* ppt2DEdgeAll,
							int nPt2DEdgeAllNo,
							CPoint2D* ppt2DEdgeDefect,
							int nPt2DEdgeDefectNo,
							int nOffset,
							double dChipoffTol,
							BOOL* pbFoundChipoff)
{
	int i, j;
	double dy;
//	int nIndex;
	double dLeftx;
	double dRightx;
	int nError;
try {
	*pbFoundChipoff = FALSE;
	for(i=0;i<nPt2DEdgeDefectNo;i++) {
		nError = GetY(ppt2DEdgeDefect[i].x, &dy);
		if(nError)
			goto ErrorAbort;
		if((dy > ppt2DEdgeDefect[i].y) || (fabs(dy - ppt2DEdgeDefect[i].y) > 1.5))
		{
			dLeftx = ppt2DEdgeDefect[i].x - nOffset - 0.1;
			dRightx = ppt2DEdgeDefect[i].x + nOffset + 0.1;
			for(j=0;j<nPt2DEdgeAllNo;j++)
			{
				if((ppt2DEdgeAll[j].x > dLeftx) && 
					(ppt2DEdgeAll[j].x < dRightx))
				{
					if(fabs(ppt2DEdgeDefect[i].y - ppt2DEdgeAll[j].y) > dChipoffTol)
					{
						*pbFoundChipoff = TRUE;
						break;
					}
				}
			}
			if(*pbFoundChipoff)
				break;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurveFitting::CheckChipoff");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CCurveFitting::CheckChipoff(CImageBuffer* pImgBin,
							CRect* prcBin,
							CPoint2D* ppt2DEdgeAll,
							int nPt2DEdgeAllNo,
							CPoint2D* ppt2DEdgeDefect,
							int nPt2DEdgeDefectNo,
							int nOffset,
							double dChipoffTol,
							BOOL* pbFoundChipoff)
{
	int i, j, k;
	double dy;
//	int nIndex;
	double dLeftx;
	double dRightx;
	int nx, ny;
	BYTE byteVal;
	BOOL bFoundEdge;
	int nError;
try {
	*pbFoundChipoff = FALSE;
	for(i=0;i<nPt2DEdgeDefectNo;i++) {
		nError = GetY(ppt2DEdgeDefect[i].x, &dy);
		if(nError)
			goto ErrorAbort;
		if((dy > ppt2DEdgeDefect[i].y) || (fabs(dy - ppt2DEdgeDefect[i].y) > 1.5))
		{
			dLeftx = ppt2DEdgeDefect[i].x - nOffset - 0.1;
			dRightx = ppt2DEdgeDefect[i].x + nOffset + 0.1;
			for(j=0;j<nPt2DEdgeAllNo;j++)
			{
				if((ppt2DEdgeAll[j].x > dLeftx) && 
					(ppt2DEdgeAll[j].x < dRightx))
				{
					if(fabs(ppt2DEdgeDefect[i].y - ppt2DEdgeAll[j].y) > dChipoffTol)
					{
						nx = (int) ppt2DEdgeAll[j].x;
						ny = (int) ppt2DEdgeAll[j].y;
						bFoundEdge = FALSE;
						for(k=-Round(dChipoffTol*0.8);k<=Round(dChipoffTol*0.8);k++)
						{
							nError = pImgBin->GetPixelVal(nx, ny + k, byteVal);
							if((nError == 0) && (byteVal == 255))
							{
								bFoundEdge = TRUE;
								break;
							}
						}
						if(bFoundEdge == FALSE)
						{
							*pbFoundChipoff = TRUE;
							break;
						}
					}
				}
			}
			if(*pbFoundChipoff)
				break;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurveFitting::CheckChipoff");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CCurveFitting::GetCurveFitting(CPoint2D* ppt2DEdge,
						int nPt2DEdgeNo,
						BOOL bHor,
						int nSegmentLength,
						int nSegmentPercentage)
{
	int nError;
try {
	Clean();
	m_nSegmentLength = nSegmentLength + 1;
	m_nSegmentPercentage = nSegmentPercentage;
	if(nSegmentLength < 3) {
		goto ErrorAbort;
	}
	bLineSegmentMethod = FALSE;
	nError = GetPointsBoundingRect(&ppt2DEdge[0], 
						  nPt2DEdgeNo,
						  &rcROI);
	if(nError)
		goto ErrorAbort;
	nError = SortingData(&ppt2DEdge[0],
						 nPt2DEdgeNo,
						 bHor);
	if(nError)
		goto ErrorAbort;
	nError = SegmentHoughTransform();
	if(nError)
		goto ErrorAbort;
	nError = CreateInterpolationBy3Pts();
	if(nError)
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurvePointStatistics::GetCurveFitting");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	Clean();
	return -1;
}


int CCurveFitting::GetLineSegmentFitting(CPoint2D* ppt2DEdge,
						int nPt2DEdgeNo,
						BOOL bHor,
						int nSegmentLength,
						int nSegmentPercentage)
{
	int nError;

try
{
	Clean();
	m_nSegmentLength = nSegmentLength + 1;
	m_nSegmentPercentage = nSegmentPercentage;
	if(nSegmentLength < 3) {
		goto ErrorAbort;
	}
	bLineSegmentMethod = TRUE;
	nError = GetPointsBoundingRect(&ppt2DEdge[0], 
						  nPt2DEdgeNo,
						  &rcROI);
	if(nError)
		goto ErrorAbort;
	nError = SortingData(&ppt2DEdge[0],
						 nPt2DEdgeNo,
						 bHor);
	if(nError)
		goto ErrorAbort;
	nError = SegmentHoughTransform();
	if(nError)
		goto ErrorAbort;
	nError = CreateInterpolationBy2Pts();
	if(nError)
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurveFitting::GetLineSegmentFitting");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	Clean();
	return -1;
}

int CCurveFitting::GetCurveFitting1(CPoint2D* ppt2DEdge,
						int nPt2DEdgeNo,
						CPoint2D* ppt2DDefectAvg,
						BOOL bHor,
						int nSegmentLength,
						int nSegmentPercentage)
{
	int nError;
try {
	Clean();
	m_nSegmentLength = nSegmentLength + 1;
	m_nSegmentPercentage = nSegmentPercentage;
	if(nSegmentLength < 3) {
		goto ErrorAbort;
	}
	bLineSegmentMethod = FALSE;
	nError = GetPointsBoundingRect(&ppt2DEdge[0], 
						  nPt2DEdgeNo,
						  &rcROI);
	/*if(nError)
		goto ErrorAbort;*/
	nError = SortingData(&ppt2DEdge[0],
						 nPt2DEdgeNo,
						 bHor);
	if(nError)
		goto ErrorAbort;
	nError = SegmentHoughTransform1(ppt2DDefectAvg);
	if(nError)
		goto ErrorAbort;
	nError = CreateInterpolationBy3Pts();
	if(nError)
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurveFitting::GetCurveFitting1");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	Clean();
	return -1;
}

int CCurveFitting::GetCurveFitting2(CPoint2D* ppt2DEdge,
						int nPt2DEdgeNo,
						CPoint2D* ppt2DDefectAvg,
						BOOL bHor,
						int nSegmentLength,
						int nSegmentPercentage,
						int nSegmentPercentageTol)
{
	int nError;
try {
	Clean();
	m_nSegmentLength = nSegmentLength + 1;
	m_nSegmentPercentage = nSegmentPercentage;
	if(nSegmentLength < 3) {
		goto ErrorAbort;
	}
	bLineSegmentMethod = FALSE;
	nError = GetPointsBoundingRect(&ppt2DEdge[0], 
						  nPt2DEdgeNo,
						  &rcROI);
	/*if(nError)
		goto ErrorAbort;*/
	nError = SortingData(&ppt2DEdge[0],
						 nPt2DEdgeNo,
						 bHor);
	if(nError)
		goto ErrorAbort;
	nError = SegmentHoughTransform2(ppt2DDefectAvg, nSegmentPercentageTol);
	if(nError)
		goto ErrorAbort;
	nError = CreateInterpolationBy3Pts2();
	if(nError)
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurveFitting::GetCurveFitting1");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	Clean();
	return -1;
}

int CCurveFitting::CheckFittingPercentage(CPoint2D* ppt2DEdge,
						int nPt2DEdgeNo,
						double dOffsetY,
						int* pnFittingPercentage)
{
	int i;
	double dy;
	int nMatchNo;
	int nError;

try
{
	*pnFittingPercentage = 0;
	nMatchNo = 0;
	for(i=0;i<nPt2DEdgeNo;i++)
	{
		nError = GetY(ppt2DEdge[i].x, &dy);
		if(fabs(ppt2DEdge[i].y - dy) < dOffsetY)
		{
			nMatchNo++;
		}
	}
	if(nPt2DEdgeNo > 0)
	{
		*pnFittingPercentage = nMatchNo * 100/nPt2DEdgeNo;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurveFitting::CheckFittingPercentage");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CCurveFitting::CheckChipoffPercentage(CPoint2D* ppt2DEdge,
						int nPt2DEdgeNo,
						double dOffsetY,
						double* pdChipoffPercentage)

{
	int i;
	double dy1;
	double dy2;
	int nChipoffNo;
	int nMaxChipoffNo;
	int nClearDisTol;
	int nClearDis;
	int nError;

try
{
	*pdChipoffPercentage = 0;
	nClearDisTol =3;
	nChipoffNo = 0;
	nMaxChipoffNo = 0;
	nClearDis = 0;
	for(i=0;i<nPt2DEdgeNo;i++)
	{
		nError = InterpolationBy2PtsSegment.GetY(ppt2DEdge[i].x, &dy1);
		nError = InterpolationBy3PtsSegment.GetY(ppt2DEdge[i].x, &dy2);
		if(((dy1 - ppt2DEdge[i].y) > dOffsetY) && ((dy2 - ppt2DEdge[i].y) > dOffsetY))
		{
			nChipoffNo++;
		}
		else
		{
			nClearDis++;
		}
		if(nClearDis >= nClearDisTol)
		{
			if(nMaxChipoffNo < nChipoffNo)
			{
				nMaxChipoffNo = nChipoffNo;
			}
			nClearDis = 0;
			nChipoffNo = 0;
		}
	}
	if(nPt2DEdgeNo > 0)
	{
		*pdChipoffPercentage = nMaxChipoffNo * 100.0/nPt2DEdgeNo;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurveFitting::CheckChipoffPercentage");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CCurveFitting::CreateInterpolationBy3Pts()
{
	int i;
	CPoint2D* ppt2DSegmentCtr;
	int nPt2DSegmentCtrNo;
	int nError;
try {
	ppt2DSegmentCtr = NULL;
	nPt2DSegmentCtrNo = nSegmentNo;
	if(nPt2DSegmentCtrNo <= 0)
		goto ErrorAbort;
	ppt2DSegmentCtr = new CPoint2D[abs(nPt2DSegmentCtrNo)];
	for(i=0;i<nPt2DSegmentCtrNo;i++)
		ppt2DSegmentCtr[i] = pln2DSegment[i].GetPosition();
	nError = InterpolationBy3PtsSegment.SetPoint2DAry(&ppt2DSegmentCtr[0],
					  nPt2DSegmentCtrNo);
	if(nError)
		goto ErrorAbort;
	nError = InterpolationBy2PtsSegment.SetPoint2DAry(&ppt2DSegmentCtr[0],
					  nPt2DSegmentCtrNo);
	if(nError)
		goto ErrorAbort;

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurvePointStatistics::CreateInterpolationBy3Pts");
	goto ErrorAbort;
}
	if(ppt2DSegmentCtr != NULL)
		delete [] ppt2DSegmentCtr;
	return 0;
ErrorAbort:
	if(ppt2DSegmentCtr != NULL)
		delete [] ppt2DSegmentCtr;
	return -1;
}

int CCurveFitting::CreateInterpolationBy3Pts2()
{
	int i;
	CPoint2D* ppt2DSegmentCtr;
	int nPt2DSegmentCtrNo;
	int nError;
try {
	ppt2DSegmentCtr = NULL;
	csRangeX = CSize(-1,-1);
	nPt2DSegmentCtrNo = nGoodLineSegmentNo;
	if(nPt2DSegmentCtrNo <= 0)
		goto ErrorAbort;
	ppt2DSegmentCtr = new CPoint2D[abs(nPt2DSegmentCtrNo)];
	for(i=0;i<nPt2DSegmentCtrNo;i++) 
	{
		ppt2DSegmentCtr[i] = pln2DSegment[i].GetPosition();
		if(i==0)
		{
			csRangeX = CSize(Round(ppt2DSegmentCtr[i].x), Round(ppt2DSegmentCtr[i].x));
		}
		else
		{
			if(csRangeX.cx > Round(ppt2DSegmentCtr[i].x))
				csRangeX.cx = Round(ppt2DSegmentCtr[i].x);
			if(csRangeX.cy < Round(ppt2DSegmentCtr[i].x))
				csRangeX.cy = Round(ppt2DSegmentCtr[i].x);
		}
	}
	nError = InterpolationBy3PtsSegment.SetPoint2DAry(&ppt2DSegmentCtr[0],
					  nPt2DSegmentCtrNo);
	if(nError)
		goto ErrorAbort;
	nError = InterpolationBy2PtsSegment.SetPoint2DAry(&ppt2DSegmentCtr[0],
					  nPt2DSegmentCtrNo);
	if(nError)
		goto ErrorAbort;

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurvePointStatistics::CreateInterpolationBy3Pts2");
	goto ErrorAbort;
}
	if(ppt2DSegmentCtr != NULL)
		delete [] ppt2DSegmentCtr;
	return 0;
ErrorAbort:
	if(ppt2DSegmentCtr != NULL)
		delete [] ppt2DSegmentCtr;
	return -1;
}


int CCurveFitting::CreateInterpolationBy2Pts()
{
	int i;
	CPoint2D* ppt2DSegmentCtr;
	int nPt2DSegmentCtrNo;
	int nError;
try
{
	ppt2DSegmentCtr = NULL;
	nPt2DSegmentCtrNo = nSegmentNo;
	if(nPt2DSegmentCtrNo <= 0)
		goto ErrorAbort;
	ppt2DSegmentCtr = new CPoint2D[abs(nPt2DSegmentCtrNo)];
	for(i=0;i<nPt2DSegmentCtrNo;i++)
		ppt2DSegmentCtr[i] = pln2DSegment[i].GetPosition();
	nError = InterpolationBy2PtsSegment.SetPoint2DAry(&ppt2DSegmentCtr[0],
					  nPt2DSegmentCtrNo);
	if(nError)
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurveFitting::CreateInterpolationBy2Pts");
	goto ErrorAbort;
}
	if(ppt2DSegmentCtr != NULL)
		delete [] ppt2DSegmentCtr;
	return 0;
ErrorAbort:
	if(ppt2DSegmentCtr != NULL)
		delete [] ppt2DSegmentCtr;
	return -1;
}


int CCurveFitting::GetY(double dx, double* pdy)
{
	int nError;
try {
	if(bLineSegmentMethod) 
		nError = InterpolationBy2PtsSegment.GetY(dx, pdy);
	else
		nError = InterpolationBy3PtsSegment.GetY(dx, pdy);
	if(nError)
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurvePointStatistics::GetY");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CCurveFitting::GetDistance(CPoint2D* ppt2D, double* pdDis)
{
	double dy;
	int nError;
try {
	if(bLineSegmentMethod) 
		nError = InterpolationBy2PtsSegment.GetY(ppt2D->x, &dy);
	else
		nError = InterpolationBy3PtsSegment.GetY(ppt2D->x, &dy);
	if(nError)
		goto ErrorAbort;
	*pdDis = fabs(ppt2D->y - dy);
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurvePointStatistics::GetDistance");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}



int CCurveFitting::SegmentHoughTransform()
{
	int i;
	CHoughTransform HoughTransform;
	double dNominalAngleRad;
	double dTolAngleRad;	
	double dAngleStep;	
	double dOnLineTol;
	CPoint2D *ppt2DOnLine;
	int nOnLineSize;
	CPoint2D *ppt2DNOTOnLine;
	int nNOTOnLineSize;
	CLine2D ln2DFitting;
	CLine2D* pln2DTemp;
	CPoint2D* ppt2DSegmentCtr;
	double dPosTol;
	CLine2D ln2DLft;
	CLine2D ln2DRht;
	int nError;
try {
	ppt2DOnLine = NULL;
	ppt2DNOTOnLine = NULL;
	pln2DTemp = NULL;
	ppt2DSegmentCtr = NULL;
	dNominalAngleRad = 0;
	dTolAngleRad = 20 * PI/180;	
	dAngleStep = 1 * PI/180;		
	dOnLineTol = 2;
	dPosTol = 2;

	if(pln2DSegment != NULL)
		delete[] pln2DSegment;
	pln2DSegment = NULL;
	if(pnPercentageByLength != NULL)
		delete [] pnPercentageByLength;
	pnPercentageByLength = NULL;
	if(pnPercentageByAllPoints != NULL)
		delete [] pnPercentageByAllPoints;
	pnPercentageByAllPoints = NULL;

	pln2DSegment = new CLine2D[abs(nSegmentNo)];
	pln2DTemp = new CLine2D[abs(nSegmentNo)];
	pnPercentageByLength = new int[abs(nSegmentNo)];
	pnPercentageByAllPoints = new int[abs(nSegmentNo)];
	ppt2DSegmentCtr = new CPoint2D[abs(nSegmentNo)];
	for(i=0;i<nSegmentNo;i++) {
		pnPercentageByLength[i] = 0;
		pnPercentageByAllPoints[i] = 0;
		ppt2DSegmentCtr[i] = CPoint2D(m_nSegmentLength * (i+0.5) + rcROI.left, 0);
	}
	for(i=0;i<nSegmentNo;i++) {
		if(pnPt2DInSegment[i] < 3)
			continue;
		if(ppt2DOnLine != NULL)
			delete [] ppt2DOnLine;
		if(ppt2DNOTOnLine != NULL)
			delete [] ppt2DNOTOnLine;
		ppt2DOnLine = new CPoint2D[abs(pnPt2DInSegment[i])];
		ppt2DNOTOnLine = new CPoint2D[abs(pnPt2DInSegment[i])];
		nError = HoughTransform.HoughTransformLine(&ppt2DEdgeSorting[i][0],
											pnPt2DInSegment[i],
											dNominalAngleRad,
											dTolAngleRad,	
											dAngleStep,		
											dOnLineTol,
											&ppt2DOnLine[0],
											nOnLineSize,
											&ppt2DNOTOnLine[0],
											nNOTOnLineSize);
		if(nError)
			goto ErrorAbort;
		if(nOnLineSize < 2)
			continue;
		pnPercentageByLength[i] = int(nOnLineSize * 100.0/m_nSegmentLength); 
		pnPercentageByAllPoints[i] = int(nOnLineSize * 100.0/pnPt2DInSegment[i]);
		ln2DFitting.GetBestFittingLine(&ppt2DOnLine[0],
									   nOnLineSize);
		ln2DFitting.GetY(ppt2DSegmentCtr[i].x, &ppt2DSegmentCtr[i].y);
		pln2DSegment[i] = ln2DFitting;
	}

	nError = CurvePointStatistics.Inspect(&ppt2DSegmentCtr[0],
							&pnPercentageByLength[0],
							m_nSegmentPercentage,
							dPosTol,
							nSegmentNo);
	if(nError) {
		for(i=0;i<nSegmentNo;i++) {
			CurvePointStatistics.ppt2DHough[i] = ppt2DSegmentCtr[i];
		}
	}
	else {
		for(i=0;i<nSegmentNo;i++) {
			if(pnPercentageByLength[i] > 80)
				CurvePointStatistics.ppt2DHough[i] = ppt2DSegmentCtr[i];
		}
	}
	pln2DSegment[0] = CLine2D(CurvePointStatistics.ppt2DHough[0],CurvePointStatistics.ppt2DHough[1]);
	pln2DSegment[nSegmentNo-1] = CLine2D(CurvePointStatistics.ppt2DHough[nSegmentNo-2],CurvePointStatistics.ppt2DHough[nSegmentNo-1]);
	for(i=1;i<(nSegmentNo-1);i++) { 
		ln2DLft = CLine2D(CurvePointStatistics.ppt2DHough[i-1],CurvePointStatistics.ppt2DHough[i]);
		ln2DRht = CLine2D(CurvePointStatistics.ppt2DHough[i],CurvePointStatistics.ppt2DHough[i+1]);
		pln2DSegment[i].GetBiSectLine(&ln2DLft, &ln2DRht); 
	}
	for(i=0;i<nSegmentNo;i++) {
		pln2DSegment[i].SetPosition(CurvePointStatistics.ppt2DHough[i]);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurvePointStatistics::SegmentHoughTransform");
	goto ErrorAbort;
}
	if(ppt2DOnLine != NULL)
		delete [] ppt2DOnLine;
	if(ppt2DNOTOnLine != NULL)
		delete [] ppt2DNOTOnLine;
	if(pln2DTemp != NULL)
		delete [] pln2DTemp;
	if(ppt2DSegmentCtr != NULL)
		delete [] ppt2DSegmentCtr;
	return 0;
ErrorAbort:
	if(ppt2DOnLine != NULL)
		delete [] ppt2DOnLine;
	if(ppt2DNOTOnLine != NULL)
		delete [] ppt2DNOTOnLine;
	if(pln2DTemp != NULL)
		delete [] pln2DTemp;
	if(ppt2DSegmentCtr != NULL)
		delete [] ppt2DSegmentCtr;
	return -1;
}


int CCurveFitting::SegmentHoughTransform1(CPoint2D* ppt2DDefectAvg)
{
	int i;
	CHoughTransform HoughTransform;
	double dNominalAngleRad;
	double dTolAngleRad;	
	double dAngleStep;	
	double dOnLineTol;
	CPoint2D *ppt2DOnLine;
	int nOnLineSize;
	CPoint2D *ppt2DNOTOnLine;
	int nNOTOnLineSize;
	CLine2D ln2DFitting;
	CLine2D* pln2DTemp;
	CPoint2D* ppt2DSegmentCtr;
	double dPosTol;
	CLine2D ln2DLft;
	CLine2D ln2DRht;
	int nError;

try {
	ppt2DOnLine = NULL;
	ppt2DNOTOnLine = NULL;
	pln2DTemp = NULL;
	ppt2DSegmentCtr = NULL;
	dNominalAngleRad = 0;
	dTolAngleRad = 30 * PI/180;	
	dAngleStep = 1 * PI/180;		
	dOnLineTol = 2 * dFOVZoomRatio;
	dPosTol = 2 * dFOVZoomRatio;

	if(pln2DSegment != NULL)
		delete[] pln2DSegment;
	pln2DSegment = NULL;
	if(pnPercentageByLength != NULL)
		delete [] pnPercentageByLength;
	pnPercentageByLength = NULL;
	if(pnPercentageByAllPoints != NULL)
		delete [] pnPercentageByAllPoints;
	pnPercentageByAllPoints = NULL;

	pln2DSegment = new CLine2D[abs(nSegmentNo)];
	pln2DTemp = new CLine2D[abs(nSegmentNo)];
	pnPercentageByLength = new int[abs(nSegmentNo)];
	pnPercentageByAllPoints = new int[abs(nSegmentNo)];
	ppt2DSegmentCtr = new CPoint2D[abs(nSegmentNo)];
	for(i=0;i<nSegmentNo;i++) {
		pnPercentageByLength[i] = 0;
		pnPercentageByAllPoints[i] = 0;
		ppt2DSegmentCtr[i] = CPoint2D(m_nSegmentLength * (i+0.5) + rcROI.left, 0);
	}
	for(i=0;i<nSegmentNo;i++) {
		if(pnPt2DInSegment[i] < 3)
			continue;
		if(ppt2DOnLine != NULL)
			delete [] ppt2DOnLine;
		if(ppt2DNOTOnLine != NULL)
			delete [] ppt2DNOTOnLine;
		ppt2DOnLine = new CPoint2D[abs(pnPt2DInSegment[i])];
		ppt2DNOTOnLine = new CPoint2D[abs(pnPt2DInSegment[i])];
		nError = HoughTransform.HoughTransformLine(&ppt2DEdgeSorting[i][0],
											pnPt2DInSegment[i],
											dNominalAngleRad,
											dTolAngleRad,	
											dAngleStep,		
											dOnLineTol,
											&ppt2DOnLine[0],
											nOnLineSize,
											&ppt2DNOTOnLine[0],
											nNOTOnLineSize);
		if(nError)
			goto ErrorAbort;
		if(nOnLineSize < 2)
			continue;
		pnPercentageByLength[i] = int(nOnLineSize * 100.0/m_nSegmentLength); 
		pnPercentageByAllPoints[i] = int(nOnLineSize * 100.0/pnPt2DInSegment[i]);
		ln2DFitting.GetBestFittingLine(&ppt2DOnLine[0],
									   nOnLineSize);
		ln2DFitting.GetY(ppt2DSegmentCtr[i].x, &ppt2DSegmentCtr[i].y);
		pln2DSegment[i] = ln2DFitting;
	}

	nError = CurvePointStatistics.Inspect1(&ppt2DSegmentCtr[0],
							&pnPercentageByLength[0],
							m_nSegmentPercentage,
							ppt2DDefectAvg,
							dPosTol,
							nSegmentNo);
	if(nError) {
		for(i=0;i<nSegmentNo;i++) {
			CurvePointStatistics.ppt2DHough[i] = ppt2DSegmentCtr[i];
		}
	}
	else {
		for(i=0;i<nSegmentNo;i++) {
			if(pnPercentageByLength[i] > 80)
				CurvePointStatistics.ppt2DHough[i] = ppt2DSegmentCtr[i];
		}
	}
	pln2DSegment[0] = CLine2D(CurvePointStatistics.ppt2DHough[0],CurvePointStatistics.ppt2DHough[1]);
	pln2DSegment[nSegmentNo-1] = CLine2D(CurvePointStatistics.ppt2DHough[nSegmentNo-2],CurvePointStatistics.ppt2DHough[nSegmentNo-1]);
	for(i=1;i<(nSegmentNo-1);i++) { 
		ln2DLft = CLine2D(CurvePointStatistics.ppt2DHough[i-1],CurvePointStatistics.ppt2DHough[i]);
		ln2DRht = CLine2D(CurvePointStatistics.ppt2DHough[i],CurvePointStatistics.ppt2DHough[i+1]);
		pln2DSegment[i].GetBiSectLine(&ln2DLft, &ln2DRht); 
	}
	for(i=0;i<nSegmentNo;i++) {
		pln2DSegment[i].SetPosition(CurvePointStatistics.ppt2DHough[i]);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurvePointStatistics::SegmentHoughTransform1");
	goto ErrorAbort;
}
	if(ppt2DOnLine != NULL)
		delete [] ppt2DOnLine;
	if(ppt2DNOTOnLine != NULL)
		delete [] ppt2DNOTOnLine;
	if(pln2DTemp != NULL)
		delete [] pln2DTemp;
	if(ppt2DSegmentCtr != NULL)
		delete [] ppt2DSegmentCtr;
	return 0;
ErrorAbort:
	if(ppt2DOnLine != NULL)
		delete [] ppt2DOnLine;
	if(ppt2DNOTOnLine != NULL)
		delete [] ppt2DNOTOnLine;
	if(pln2DTemp != NULL)
		delete [] pln2DTemp;
	if(ppt2DSegmentCtr != NULL)
		delete [] ppt2DSegmentCtr;
	return -1;
}

int CCurveFitting::SegmentHoughTransform2(CPoint2D* ppt2DDefectAvg, int nSegmentPercentageTol)
{
	int i;
	CHoughTransform HoughTransform;
	double dNominalAngleRad;
	double dTolAngleRad;	
	double dAngleStep;	
	double dOnLineTol;
	CPoint2D *ppt2DOnLine;
	int nOnLineSize;
	CPoint2D *ppt2DNOTOnLine;
	int nNOTOnLineSize;
	CLine2D ln2DFitting;
	CLine2D* pln2DTemp;
	CPoint2D* ppt2DSegmentCtr;
	double dPosTol;
	CLine2D ln2DLft;
	CLine2D ln2DRht;
	int nError;

try {
	ppt2DOnLine = NULL;
	ppt2DNOTOnLine = NULL;
	pln2DTemp = NULL;
	ppt2DSegmentCtr = NULL;
	dNominalAngleRad = 0;
	dTolAngleRad = 30 * PI/180;	
	dAngleStep = 1 * PI/180;		
	dOnLineTol = 2 * dFOVZoomRatio;
	dPosTol = 2 * dFOVZoomRatio;

	if(pln2DSegment != NULL)
		delete[] pln2DSegment;
	pln2DSegment = NULL;
	if(pnPercentageByLength != NULL)
		delete [] pnPercentageByLength;
	pnPercentageByLength = NULL;
	if(pnPercentageByAllPoints != NULL)
		delete [] pnPercentageByAllPoints;
	pnPercentageByAllPoints = NULL;

	pln2DSegment = new CLine2D[abs(nSegmentNo)];
	pln2DTemp = new CLine2D[abs(nSegmentNo)];
	pnPercentageByLength = new int[abs(nSegmentNo)];
	pnPercentageByAllPoints = new int[abs(nSegmentNo)];
	ppt2DSegmentCtr = new CPoint2D[abs(nSegmentNo)];
	for(i=0;i<nSegmentNo;i++) {
		pnPercentageByLength[i] = 0;
		pnPercentageByAllPoints[i] = 0;
		ppt2DSegmentCtr[i] = CPoint2D(m_nSegmentLength * (i+0.5) + rcROI.left, 0);
	}
	for(i=0;i<nSegmentNo;i++) {
		if(pnPt2DInSegment[i] < 3)
			continue;
		if(ppt2DOnLine != NULL)
			delete [] ppt2DOnLine;
		if(ppt2DNOTOnLine != NULL)
			delete [] ppt2DNOTOnLine;
		ppt2DOnLine = new CPoint2D[abs(pnPt2DInSegment[i])];
		ppt2DNOTOnLine = new CPoint2D[abs(pnPt2DInSegment[i])];
		nError = HoughTransform.HoughTransformLine(&ppt2DEdgeSorting[i][0],
											pnPt2DInSegment[i],
											dNominalAngleRad,
											dTolAngleRad,	
											dAngleStep,		
											dOnLineTol,
											&ppt2DOnLine[0],
											nOnLineSize,
											&ppt2DNOTOnLine[0],
											nNOTOnLineSize);
		if(nError)
			goto ErrorAbort;
		if(nOnLineSize < 2)
			continue;
		pnPercentageByLength[i] = int(nOnLineSize * 100.0/m_nSegmentLength); 
		pnPercentageByAllPoints[i] = int(nOnLineSize * 100.0/pnPt2DInSegment[i]);
		ln2DFitting.GetBestFittingLine(&ppt2DOnLine[0],
									   nOnLineSize);
		ln2DFitting.GetY(ppt2DSegmentCtr[i].x, &ppt2DSegmentCtr[i].y);
		pln2DSegment[i] = ln2DFitting;
	}

	nError = CurvePointStatistics.Inspect1(&ppt2DSegmentCtr[0],
							&pnPercentageByLength[0],
							m_nSegmentPercentage,
							ppt2DDefectAvg,
							dPosTol,
							nSegmentNo);
	if(nError) {
		nGoodLineSegmentNo = 0;
		for(i=0;i<nSegmentNo;i++) {
			if(pnPercentageByLength[i] > nSegmentPercentageTol)
				CurvePointStatistics.ppt2DHough[nGoodLineSegmentNo++] = ppt2DSegmentCtr[i];
		}
	}
	else {
		nGoodLineSegmentNo = 0;
		for(i=0;i<nSegmentNo;i++) {
			if(pnPercentageByLength[i] > nSegmentPercentageTol)
				CurvePointStatistics.ppt2DHough[nGoodLineSegmentNo++] = ppt2DSegmentCtr[i];
		}
	}
	if(nGoodLineSegmentNo < 2)
		goto ErrorAbort;
	pln2DSegment[0] = CLine2D(CurvePointStatistics.ppt2DHough[0],CurvePointStatistics.ppt2DHough[1]);
	pln2DSegment[nGoodLineSegmentNo-1] = CLine2D(CurvePointStatistics.ppt2DHough[nGoodLineSegmentNo-2],CurvePointStatistics.ppt2DHough[nGoodLineSegmentNo-1]);
	for(i=1;i<(nGoodLineSegmentNo-1);i++) { 
		ln2DLft = CLine2D(CurvePointStatistics.ppt2DHough[i-1],CurvePointStatistics.ppt2DHough[i]);
		ln2DRht = CLine2D(CurvePointStatistics.ppt2DHough[i],CurvePointStatistics.ppt2DHough[i+1]);
		pln2DSegment[i].GetBiSectLine(&ln2DLft, &ln2DRht); 
	}
	for(i=0;i<nGoodLineSegmentNo;i++) {
		pln2DSegment[i].SetPosition(CurvePointStatistics.ppt2DHough[i]);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurvePointStatistics::SegmentHoughTransform2");
	goto ErrorAbort;
}
	if(ppt2DOnLine != NULL)
		delete [] ppt2DOnLine;
	if(ppt2DNOTOnLine != NULL)
		delete [] ppt2DNOTOnLine;
	if(pln2DTemp != NULL)
		delete [] pln2DTemp;
	if(ppt2DSegmentCtr != NULL)
		delete [] ppt2DSegmentCtr;
	return 0;
ErrorAbort:
	if(ppt2DOnLine != NULL)
		delete [] ppt2DOnLine;
	if(ppt2DNOTOnLine != NULL)
		delete [] ppt2DNOTOnLine;
	if(pln2DTemp != NULL)
		delete [] pln2DTemp;
	if(ppt2DSegmentCtr != NULL)
		delete [] ppt2DSegmentCtr;
	return -1;
}





int CCurveFitting::SortingData(CPoint2D* ppt2DEdge,
						int nPt2DEdgeNo,
						BOOL bHor)
{
	int i;
	double dLft;
	int nSegmentIndex;
	int nLastSegmentLft;
	int n2ndLastSegmentRht;
	int nSegmentNoSub1;
try {	
	nSegmentNo = rcROI.Width()/m_nSegmentLength + 1;
	nLastSegmentLft = rcROI.right - m_nSegmentLength;
	n2ndLastSegmentRht = rcROI.left + (nSegmentNo - 1) * m_nSegmentLength;
	if(nSegmentNo < 3)
		goto ErrorAbort;

	if(ppt2DEdgeSorting != NULL) {
		for(i=0;i<nSegmentNo;i++) {
			if(ppt2DEdgeSorting[i] != NULL)
				delete [] ppt2DEdgeSorting[i];
			ppt2DEdgeSorting[i] = NULL;
		}
		delete [] ppt2DEdgeSorting;
		if(pnPt2DInSegment != NULL)
			delete [] pnPt2DInSegment;
	}
	if(ppt2DSegmentCtr != NULL)
		delete [] ppt2DSegmentCtr;
	ppt2DEdgeSorting = NULL;
	pnPt2DInSegment = NULL;
	ppt2DSegmentCtr = NULL;
	if(prcSegment != NULL)
		delete [] prcSegment;
	ppt2DEdgeSorting = new CPoint2D*[abs(nSegmentNo)];
	pnPt2DInSegment = new int[abs(nSegmentNo)];
	ppt2DSegmentCtr = new CPoint2D[abs(nSegmentNo)];
	prcSegment = new CRect[abs(nSegmentNo)];
	for(i=0;i<nSegmentNo;i++) {
		ppt2DEdgeSorting[i] = new CPoint2D[abs(nPt2DEdgeNo)];
		pnPt2DInSegment[i] = 0;
		prcSegment[i] = CRect(rcROI.left + i*m_nSegmentLength, 
							  rcROI.top,
							  rcROI.left + (i+1)*m_nSegmentLength,
							  rcROI.bottom);
		if(prcSegment[i].right > rcROI.right)
			prcSegment[i].right = rcROI.right;
	}
	nSegmentNoSub1 = nSegmentNo - 1;
	dLft = rcROI.left;
	for(i=0;i<nPt2DEdgeNo;i++) {
		nSegmentIndex = int(ppt2DEdge[i].x - dLft)/m_nSegmentLength;
		ppt2DEdgeSorting[nSegmentIndex][pnPt2DInSegment[nSegmentIndex]++] = ppt2DEdge[i];
		if((nSegmentIndex != nSegmentNoSub1) && (ppt2DEdge[i].x >= nLastSegmentLft) && (ppt2DEdge[i].x <= n2ndLastSegmentRht))
			ppt2DEdgeSorting[nSegmentNo-1][pnPt2DInSegment[nSegmentNo-1]++] = ppt2DEdge[i];
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCurvePointStatistics::SortingData");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}



CPointMatching::CPointMatching()
{
	csRange = CSize(0,0);
	lBestMatchingSum = -1;
	ptBestMatching = CPoint(0,0);
	pptTeachAry = NULL;
	nptTeachNo = 0;
	plAddress = NULL;
	nPrePitch = -1;
}

CPointMatching::~CPointMatching()
{
	CleanAll();
}

void CPointMatching::CleanAll()
{
	CleanInspection();
	CleanTeach();
}


void CPointMatching::CleanTeach()
{
	if(pptTeachAry != NULL)
		delete [] pptTeachAry;
	pptTeachAry = NULL;
	nptTeachNo = 0;
	if(plAddress != NULL)
		delete [] plAddress;
	plAddress = NULL;
}

void CPointMatching::CleanInspection()
{
	csRange = CSize(0,0);
}

int CPointMatching::SetTeachPointAry(CPoint* pptTeachAry1,
								 int nPtArySize1)
{
	int i;
	int nError;

try
{
	CleanTeach();
	if(nPtArySize1 <= 0)
		goto PASS;
	nptTeachNo = nPtArySize1;
	plAddress = new long[abs(nptTeachNo)];
	pptTeachAry = new CPoint[abs(nptTeachNo)];
	for(i=0;i<nptTeachNo;i++) 
		pptTeachAry[i] = pptTeachAry1[i];
	ptTeachCtr = pptTeachAry[nptTeachNo/2];
	nError = GetPointsBoundingRect(&pptTeachAry[0], nptTeachNo, &rcTeach);
	if(nError)
		goto ErrorAbort;
	ln2DTeach.GetBestFittingLine(&pptTeachAry[0], nptTeachNo);
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CPointMatching::SetTeachPointAry");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

int CPointMatching::GetPtNumberByContrast(IMAGEVIEW_INFO *pView,
								CImageBuffer *pImgBufSrc,
								CRect* prcInsp,
								CPoint* pptBestMatching,
								CPoint* pptShift,
								int nContrast,
								int nBlobType,
								int* pnPtNumberByContrast,
								long lDebugFlag)
{
	int i;
	BYTE* pImgBufSrcPtr;
	int nPitch;
	int nCount;
	long lBestMatchingOffset;
	long lOffset;
	CPoint ptAllShift;
	CRect rcAllShift;
	CRect rcBestMatchingShift;
try
{
	ptAllShift = *pptBestMatching + *pptShift;
	rcAllShift = rcTeach + ptAllShift;
	if(RectInRect(*prcInsp, rcAllShift) == 0)
		goto ErrorAbort;
	rcBestMatchingShift = rcTeach + *pptBestMatching;
	if(RectInRect(*prcInsp, rcBestMatchingShift) == 0)
		goto ErrorAbort;

	pImgBufSrcPtr = pImgBufSrc->GetImgPtr();
	nPitch = pImgBufSrc->GetPitch();
	lBestMatchingOffset = pptBestMatching->y * nPitch + pptBestMatching->x;
	lOffset = ptAllShift.y * nPitch + ptAllShift.x;
	nCount = 0;
	if(nBlobType == WHITE) {
		for(i=0;i<nptTeachNo;i++) {
			if((pImgBufSrcPtr[plAddress[i] + lBestMatchingOffset] - pImgBufSrcPtr[plAddress[i] + lOffset]) >= nContrast)
				nCount++;
		}
	}
	else {
		for(i=0;i<nptTeachNo;i++) {
			if((pImgBufSrcPtr[plAddress[i] + lOffset] - pImgBufSrcPtr[plAddress[i] + lBestMatchingOffset]) >= nContrast)
				nCount++;
		}
	}
	*pnPtNumberByContrast = nCount;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CPointMatching::GetPtNumberByContrast");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CPointMatching::GetPtNumberByContrast(IMAGEVIEW_INFO *pView,
								CImageBuffer *pImgBufSrc,
								CRect* prcInsp,
								CPoint* pptBestMatching,
								CPoint* pptShift0,
								CPoint* pptShift1,
								int nContrast,
								int nBlobType,
								int* pnPtNumberByContrast,
								long lDebugFlag)
{
	int i;
	BYTE* pImgBufSrcPtr;
	int nPitch;
	int nCount;
	long lBestMatchingOffset;
	long lOffset0;
	CPoint ptAllShift0;
	CRect rcAllShift0;
	long lOffset1;
	CPoint ptAllShift1;
	CRect rcAllShift1;
	CRect rcBestMatchingShift;

try
{
	ptAllShift0 = *pptBestMatching + *pptShift0;
	rcAllShift0 = rcTeach + ptAllShift0;
	if(RectInRect(*prcInsp, rcAllShift0) == 0)
		goto ErrorAbort;
	ptAllShift1 = *pptBestMatching + *pptShift1;
	rcAllShift1 = rcTeach + ptAllShift1;
	if(RectInRect(*prcInsp, rcAllShift1) == 0)
		goto ErrorAbort;
	rcBestMatchingShift = rcTeach + *pptBestMatching;
	if(RectInRect(*prcInsp, rcBestMatchingShift) == 0)
		goto ErrorAbort;

	pImgBufSrcPtr = pImgBufSrc->GetImgPtr();
	nPitch = pImgBufSrc->GetPitch();
	lBestMatchingOffset = pptBestMatching->y * nPitch + pptBestMatching->x;
	lOffset0 = ptAllShift0.y * nPitch + ptAllShift0.x;
	lOffset1 = ptAllShift1.y * nPitch + ptAllShift1.x;
	nCount = 0;
	if(nBlobType == WHITE) {
		for(i=0;i<nptTeachNo;i++) {
			if(((pImgBufSrcPtr[plAddress[i] + lBestMatchingOffset] - pImgBufSrcPtr[plAddress[i] + lOffset0]) >= nContrast) &&
			   ((pImgBufSrcPtr[plAddress[i] + lBestMatchingOffset] - pImgBufSrcPtr[plAddress[i] + lOffset1]) >= nContrast))
				nCount++;
		}
	}
	else {
		for(i=0;i<nptTeachNo;i++) {
			if(((pImgBufSrcPtr[plAddress[i] + lOffset0] - pImgBufSrcPtr[plAddress[i] + lBestMatchingOffset]) >= nContrast) &&
			   ((pImgBufSrcPtr[plAddress[i] + lOffset1] - pImgBufSrcPtr[plAddress[i] + lBestMatchingOffset]) >= nContrast))
				nCount++;
		}
	}
	*pnPtNumberByContrast = nCount;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CPointMatching::GetPtNumberByContrast-1");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CPointMatching::CreateAddress(int nPitch)
{
	int i;
	if(nPrePitch == nPitch)
		goto PASS;
	for(i=0;i<nptTeachNo;i++)
		plAddress[i] = pptTeachAry[i].y * nPitch + pptTeachAry[i].x;
	nPrePitch = nPitch;
PASS:
	return 0;
}

int CPointMatching::Inspection(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufSrc,
					   CRect* prcInsp,
					   int nBlobType,
					   CPoint* pptCircleShift,
					   CSize* pcsShift,
					   long lDebugFlag)
{
	int i, j, k;
	int nXIndex;
	int nYIndex;
	CRect rcTeachShift;
	BYTE* pImgBufSrcPtr;
	int nPitch;
	long lIndex;
	long lSum;
	long lOffset;
	int* pImgIntBufPtr;
	int nImgIntBufPitch;
	long nImgIntBufIndex;
	int nShiftXRangeTol, nShiftYRangeTol; 
	CRect rcTeachShiftOrg;

try
{
	if((pcsShift->cx <= 0) || (pcsShift->cy <= 0))
		goto ErrorAbort;
	nShiftXRangeTol = 2 * pcsShift->cx + 1;
	nShiftYRangeTol = 2 * pcsShift->cy + 1;
	ImgIntBuf.Allocate(AbsSize(CSize(nShiftXRangeTol, nShiftYRangeTol)));
	ImgIntBuf.ClrImg(0);
	pImgIntBufPtr = ImgIntBuf.GetImgPtr();
	nImgIntBufPitch = ImgIntBuf.GetPitch();

	lBestMatchingSum = -1;
	pImgBufSrcPtr = pImgBufSrc->GetImgPtr();
	nPitch = pImgBufSrc->GetPitch();
	CreateAddress(nPitch);
	ptBestMatching = CPoint(0,0);
	rcTeachShift = rcTeach + *pptCircleShift;
	rcTeachShiftOrg = rcTeachShift;
	rcTeachShift.InflateRect(*pcsShift);
	CheckRect(prcInsp, &rcTeachShift);
	nYIndex = rcTeachShift.top - rcTeachShiftOrg.top + pcsShift->cy;
	if(nBlobType == WHITE) {
		lBestMatchingSum = 0;
		for(i=rcTeachShift.top - rcTeach.top;i<=(rcTeachShift.bottom - rcTeach.bottom);i++) {
			nXIndex = rcTeachShift.left - rcTeachShiftOrg.left + pcsShift->cx;
			for(j=rcTeachShift.left - rcTeach.left;j<=(rcTeachShift.right - rcTeach.right);j++) {
				lOffset = i*nPitch + j;
				lSum = 0;
				for(k=0;k<nptTeachNo;k++) {
					lIndex = plAddress[k] + lOffset;
					lSum += pImgBufSrcPtr[lIndex];
				}
				nImgIntBufIndex = nYIndex * nImgIntBufPitch + nXIndex;
				pImgIntBufPtr[nImgIntBufIndex] = lSum;
				if(lBestMatchingSum < lSum) {
					lBestMatchingSum = lSum;
					ptBestMatching = CPoint(j, i);
				}
				nXIndex++;
			}
			nYIndex++;
		}
	}
	else {
		lBestMatchingSum = rcTeachShift.Width() * rcTeachShift.Height();
		for(i=rcTeachShift.top - rcTeach.top;i<=(rcTeachShift.bottom - rcTeach.bottom);i++) {
			nXIndex = rcTeachShift.left - rcTeachShiftOrg.left + pcsShift->cx;
			for(j=rcTeachShift.left - rcTeach.left;j<=(rcTeachShift.right - rcTeach.right);j++) {
				lOffset = i*nPitch + j;
				lSum = 0;
				for(k=0;k<nptTeachNo;k++) {
					lIndex = plAddress[k] + lOffset;
					lSum += pImgBufSrcPtr[lIndex];
				}
				nImgIntBufIndex = nYIndex * nImgIntBufPitch + nXIndex;
				pImgIntBufPtr[nImgIntBufIndex] = lSum;
				if(lBestMatchingSum > lSum) {
					lBestMatchingSum = lSum;
					ptBestMatching = CPoint(j, i);
				}
				nXIndex++;
			}
			nYIndex++;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CPointMatching::Inspection");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CPointMatching::InspectionByContrast(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufSrc,
					   CRect* prcInsp,
					   CPoint* pptBestMatching,
					   CPoint* pptShift0,
					   CPoint* pptShift1,
					   int nContrast,
					   int nBlobType,
					   CSize* pcsShift,
					   long lDebugFlag)
{
	int i, j, k;
	int nXIndex;
	int nYIndex;
	CRect rcTeachShift;
	BYTE* pImgBufSrcPtr;
	int nPitch;
	long lIndex;
	long lSum;
	long lOffset;
	int* pImgIntBufPtr;
	int nImgIntBufPitch;
	long nImgIntBufIndex;
	int nShiftXRangeTol, nShiftYRangeTol; 
	CRect rcTeachShiftOrg;
	long lOffset0;
	CPoint ptAllShift0;
	CRect rcAllShift0;
	long lOffset1;
	CPoint ptAllShift1;
	CRect rcAllShift1;
	CRect rcBestMatchingShift;
	CRect rcValidInsp;

try
{
	nPtNumberByContrast = 0;
	nPtPercentageByContrast = 0;
	ptAllShift0 = *pptBestMatching + *pptShift0;
	rcAllShift0 = rcTeach + ptAllShift0;
	if(RectInRect(*prcInsp, rcAllShift0) == 0)
		goto ErrorAbort;
	ptAllShift1 = *pptBestMatching + *pptShift1;
	rcAllShift1 = rcTeach + ptAllShift1;
	if(RectInRect(*prcInsp, rcAllShift1) == 0)
		goto ErrorAbort;
	if((pcsShift->cx <= 0) || (pcsShift->cy <= 0))
		goto ErrorAbort;
	rcTeachShift = rcTeach + *pptBestMatching;
	rcTeachShiftOrg = rcTeachShift;
	if(RectInRect(*prcInsp, rcTeachShift) == 0)
		goto ErrorAbort;

	rcValidInsp = *prcInsp;
	rcValidInsp.DeflateRect(*pcsShift);
	nShiftXRangeTol = 2 * pcsShift->cx + 1;
	nShiftYRangeTol = 2 * pcsShift->cy + 1;
	ImgIntBuf.Allocate(AbsSize(CSize(nShiftXRangeTol, nShiftYRangeTol)));
	ImgIntBuf.ClrImg(0);
	pImgIntBufPtr = ImgIntBuf.GetImgPtr();
	nImgIntBufPitch = ImgIntBuf.GetPitch();

	lBestMatchingSum = -1;
	pImgBufSrcPtr = pImgBufSrc->GetImgPtr();
	nPitch = pImgBufSrc->GetPitch();
	CreateAddress(nPitch);
	ptBestMatching = CPoint(0,0);
	rcTeachShift.InflateRect(*pcsShift);
	CheckRect(&rcValidInsp, &rcTeachShift);

	nYIndex = rcTeachShift.top - rcTeachShiftOrg.top + pcsShift->cy;
	lOffset0 = pptShift0->y * nPitch + pptShift0->x;
	lOffset1 = pptShift1->y * nPitch + pptShift1->x;
	if(nBlobType == WHITE) {
		lBestMatchingSum = 0;
		for(i=rcTeachShift.top - rcTeach.top;i<=(rcTeachShift.bottom - rcTeach.bottom);i++) {
			nXIndex = rcTeachShift.left - rcTeachShiftOrg.left + pcsShift->cx;
			for(j=rcTeachShift.left - rcTeach.left;j<=(rcTeachShift.right - rcTeach.right);j++) {
				lOffset = i*nPitch + j;
				lSum = 0;
				for(k=0;k<nptTeachNo;k++) {
					lIndex = plAddress[k] + lOffset;
					if(((pImgBufSrcPtr[lIndex] - pImgBufSrcPtr[lIndex+lOffset0]) >= nContrast) &&
						((pImgBufSrcPtr[lIndex] - pImgBufSrcPtr[lIndex+lOffset1]) >= nContrast))
						lSum++;
				}
				nImgIntBufIndex = nYIndex * nImgIntBufPitch + nXIndex;
				pImgIntBufPtr[nImgIntBufIndex] = lSum;
				if(lBestMatchingSum < lSum) {
					lBestMatchingSum = lSum;
					ptBestMatching = CPoint(j, i);
				}
				nXIndex++;
			}
			nYIndex++;
		}
	}
	else {
		lBestMatchingSum = 0;
		for(i=rcTeachShift.top - rcTeach.top;i<=(rcTeachShift.bottom - rcTeach.bottom);i++) {
			nXIndex = rcTeachShift.left - rcTeachShiftOrg.left + pcsShift->cx;
			for(j=rcTeachShift.left - rcTeach.left;j<=(rcTeachShift.right - rcTeach.right);j++) {
				lOffset = i*nPitch + j;
				lSum = 0;
				for(k=0;k<nptTeachNo;k++) {
					lIndex = plAddress[k] + lOffset;
					if(((pImgBufSrcPtr[lIndex+lOffset0] - pImgBufSrcPtr[lIndex]) >= nContrast) &&
					   ((pImgBufSrcPtr[lIndex+lOffset1] - pImgBufSrcPtr[lIndex]) >= nContrast))
						lSum++;
				}
				nImgIntBufIndex = nYIndex * nImgIntBufPitch + nXIndex;
				pImgIntBufPtr[nImgIntBufIndex] = lSum;
				if(lBestMatchingSum < lSum) {
					lBestMatchingSum = lSum;
					ptBestMatching = CPoint(j, i);
				}
				nXIndex++;
			}
			nYIndex++;
		}
	}
	nPtNumberByContrast = lBestMatchingSum;
	nPtPercentageByContrast = 100 * nPtNumberByContrast/nptTeachNo;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CPointMatching::InspectionByContrast");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CPointMatching::InspectionByContrastRough(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufSrc,
					   CRect* prcInsp,
					   CPoint* pptBestMatching,
					   CPoint* pptShift0,
					   CPoint* pptShift1,
					   int nContrast,
					   int nBlobType,
					   CSize* pcsShift,
					   long lDebugFlag)
{
	int i, j, k;
	CRect rcTeachShift;
	BYTE* pImgBufSrcPtr;
	int nPitch;
	long lIndex;
	long lSum;
	long lOffset;
	int nShiftXRangeTol, nShiftYRangeTol; 
	CRect rcTeachShiftOrg;
	long lOffset0;
	CPoint ptAllShift0;
	CRect rcAllShift0;
	long lOffset1;
	CPoint ptAllShift1;
	CRect rcAllShift1;
	CRect rcBestMatchingShift;
	CRect rcValidInsp;
	long lEdgePtSum;

try
{
	nPtNumberByContrast = 0;
	nPtPercentageByContrast = 0;
	ptAllShift0 = *pptBestMatching + *pptShift0;
	rcAllShift0 = rcTeach + ptAllShift0;
	if(RectInRect(*prcInsp, rcAllShift0) == 0)
		goto ErrorAbort;
	ptAllShift1 = *pptBestMatching + *pptShift1;
	rcAllShift1 = rcTeach + ptAllShift1;
	if(RectInRect(*prcInsp, rcAllShift1) == 0)
		goto ErrorAbort;
	if((pcsShift->cx <= 0) || (pcsShift->cy <= 0))
		goto ErrorAbort;
	rcTeachShift = rcTeach + *pptBestMatching;
	rcTeachShiftOrg = rcTeachShift;
	if(RectInRect(*prcInsp, rcTeachShift) == 0)
		goto ErrorAbort;

	rcValidInsp = *prcInsp;
	rcValidInsp.DeflateRect(*pcsShift);
	nShiftXRangeTol = 2 * pcsShift->cx + 1;
	nShiftYRangeTol = 2 * pcsShift->cy + 1;
	lBestMatchingSum = -1;
	pImgBufSrcPtr = pImgBufSrc->GetImgPtr();
	nPitch = pImgBufSrc->GetPitch();
	CreateAddress(nPitch);
	ptBestMatching = CPoint(0,0);
	rcTeachShift.InflateRect(*pcsShift);
	CheckRect(&rcValidInsp, &rcTeachShift);

	lOffset0 = pptShift0->y * nPitch + pptShift0->x;
	lOffset1 = pptShift1->y * nPitch + pptShift1->x;
	if(nBlobType == WHITE) {
		lBestMatchingSum = 0;
		for(i=rcTeachShift.top - rcTeach.top;i<=(rcTeachShift.bottom - rcTeach.bottom);i++) {
			for(j=rcTeachShift.left - rcTeach.left;j<=(rcTeachShift.right - rcTeach.right);j++) {
				lOffset = i*nPitch + j;
				lSum = 0;
				for(k=0;k<nptTeachNo;k++) {
					lIndex = plAddress[k] + lOffset;
					if(((pImgBufSrcPtr[lIndex] - pImgBufSrcPtr[lIndex+lOffset0]) >= nContrast) &&
					   ((pImgBufSrcPtr[lIndex] - pImgBufSrcPtr[lIndex+lOffset1]) >= nContrast))
						lSum++;
				}
				if(lBestMatchingSum < lSum) {
					lBestMatchingSum = lSum;
					ptBestMatching = CPoint(j, i);
				}
			}
		}
	}
	else {
		lBestMatchingSum = 0;
		lEdgePtSum = 0;
		for(i=rcTeachShift.top - rcTeach.top;i<=(rcTeachShift.bottom - rcTeach.bottom);i++) {
			for(j=rcTeachShift.left - rcTeach.left;j<=(rcTeachShift.right - rcTeach.right);j++) {
				lOffset = i*nPitch + j;
				lSum = 0;
				for(k=0;k<nptTeachNo;k++) {
					lIndex = plAddress[k] + lOffset;
					if(((pImgBufSrcPtr[lIndex+lOffset0] - pImgBufSrcPtr[lIndex]) >= nContrast) &&
					   ((pImgBufSrcPtr[lIndex+lOffset1] - pImgBufSrcPtr[lIndex]) >= nContrast))
						lSum++;
				}
				if(lBestMatchingSum < lSum) {
					lBestMatchingSum = lSum;
					ptBestMatching = CPoint(j, i);
				}
				lEdgePtSum += lSum;
			}
		}
	}
	if(lBestMatchingSum > nptTeachNo)
		lBestMatchingSum = nptTeachNo;
	nPtNumberByContrast = lBestMatchingSum;
	nPtPercentageByContrast = 100 * nPtNumberByContrast/nptTeachNo;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CPointMatching::InspectionByContrastRough");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CPointMatching::InspectionBinaryAccumulation(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufSrc,
					   CRect* prcInsp,
					   CPoint* pptBestMatching,
					   int nBlobType,
					   CSize* pcsShift,
					   long lDebugFlag)
{
	CRect rcTeachShift;
	BYTE* pImgBufSrcPtr;
	int nPitch;
	long lSum;
	int nShiftXRangeTol, nShiftYRangeTol; 
	CRect rcTeachShiftOrg;
	CRect rcBestMatchingShift;
	CRect rcValidInsp;
	int nError;
try
{
	nPtNumberByContrast = 0;
	nPtPercentageByContrast = 0;
	rcTeachShift = rcTeach + *pptBestMatching;
	rcTeachShiftOrg = rcTeachShift;
	if(RectInRect(*prcInsp, rcTeachShift) == 0)
		goto ErrorAbort;

	rcValidInsp = *prcInsp;
	rcValidInsp.DeflateRect(*pcsShift);
	nShiftXRangeTol = 2 * pcsShift->cx + 1;
	nShiftYRangeTol = 2 * pcsShift->cy + 1;
	lBestMatchingSum = -1;
	pImgBufSrcPtr = pImgBufSrc->GetImgPtr();
	nPitch = pImgBufSrc->GetPitch();
	CreateAddress(nPitch);
	ptBestMatching = CPoint(0,0);
	rcTeachShift.InflateRect(*pcsShift);
	CheckRect(&rcValidInsp, &rcTeachShift);

	nError = GetSum(pImgBufSrc, 
				   &rcTeachShift, 
				   &lSum);
	if(nError)
		goto ErrorAbort;
	if(nBlobType == WHITE) 
		lBestMatchingSum = lSum/255;
	else {
		lBestMatchingSum = rcTeachShift.Width() * rcTeachShift.Height() - lSum/255;
	}
	if(lBestMatchingSum > nptTeachNo)
		lBestMatchingSum = nptTeachNo;
	nPtNumberByContrast = lBestMatchingSum;
	nPtPercentageByContrast = 100 * nPtNumberByContrast/nptTeachNo;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CPointMatching::InspectionBinaryAccumulation");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CPointMatching::InspectionLabelByBinaryRough(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufSrc,
					   CRect* prcInsp,
					   CPoint* pptBestMatching,
					   int nBlobType,
					   CSize* pcsShift,
					   int nSegmentIndex,
					   CLensBlobLabeling* pLensBlobLabeling,
					   long lDebugFlag)
{
	int i, j, m, n;
	CRect rcTeachShift;
	BYTE* pImgBufSrcPtr;
	int nPitch;
	long lIndex;
	long lSum;
	long lOffset;
	int nShiftXRangeTol, nShiftYRangeTol; 
	CRect rcTeachShiftOrg;
	CRect rcBestMatchingShift;
	CRect rcValidInsp;
	int* pnLabel;
	int nLabel;
	int nCurrentLabel;
	BYTE* pImgLabelPtr;
	long lEdgePtSum;
	BOOL bNewLabel;
	CPoint ptBlobIndexAndLabel;

try
{
	pnLabel = NULL;
	nPtNumberByContrast = 0;
	nPtPercentageByContrast = 0;
	rcTeachShift = rcTeach + *pptBestMatching;
	rcTeachShiftOrg = rcTeachShift;
	if(RectInRect(*prcInsp, rcTeachShift) == 0)
		goto ErrorAbort;

	rcValidInsp = *prcInsp;
	rcValidInsp.DeflateRect(*pcsShift);
	nShiftXRangeTol = 2 * pcsShift->cx + 1;
	nShiftYRangeTol = 2 * pcsShift->cy + 1;
	lBestMatchingSum = -1;
	pImgBufSrcPtr = pImgBufSrc->GetImgPtr();
	nPitch = pImgBufSrc->GetPitch();
	CreateAddress(nPitch);
	ptBestMatching = CPoint(0,0);
	rcTeachShift.InflateRect(*pcsShift);
	CheckRect(&rcValidInsp, &rcTeachShift);

	pImgLabelPtr = pLensBlobLabeling->ImgLabel.GetImgPtr();
	pnLabel = new int[abs(pLensBlobLabeling->nLabelNo)];
	nLabel = 0;
	if(nBlobType == WHITE) {
		lBestMatchingSum = 0;
		for(i=rcTeachShift.top - rcTeach.top;i<=(rcTeachShift.bottom - rcTeach.bottom);i++) {
			for(j=rcTeachShift.left - rcTeach.left;j<=(rcTeachShift.right - rcTeach.right);j++) {
				lOffset = i*nPitch + j;
				lSum = 0;
				for(m=0;m<nptTeachNo;m++) {
					lIndex = plAddress[m] + lOffset;
					if(pImgBufSrcPtr[lIndex]) {
						bNewLabel = TRUE;
						nCurrentLabel = pImgLabelPtr[lIndex];
						for(n=0;n<nLabel;n++) {
							if(nCurrentLabel == pnLabel[n]) {
								bNewLabel = FALSE;
								break;
							}
						}
						if(bNewLabel) 
							pnLabel[nLabel++] = nCurrentLabel;
						lSum++;
					}
				}
				if(lBestMatchingSum < lSum) {
					lBestMatchingSum = lSum;
					ptBestMatching = CPoint(j, i);
				}
			}
		}
	}
	else {
		lBestMatchingSum = 0;
		lEdgePtSum = 0;
		for(i=rcTeachShift.top - rcTeach.top;i<=(rcTeachShift.bottom - rcTeach.bottom);i++) {
			for(j=rcTeachShift.left - rcTeach.left;j<=(rcTeachShift.right - rcTeach.right);j++) {
				lOffset = i*nPitch + j;
				lSum = 0;
				for(m=0;m<nptTeachNo;m++) {
					lIndex = plAddress[m] + lOffset;
					if(pImgBufSrcPtr[lIndex] == 0)
						lSum++;
				}
				if(lBestMatchingSum < lSum) {
					lBestMatchingSum = lSum;
					ptBestMatching = CPoint(j, i);
				}
				lEdgePtSum += lSum;
			}
		}
	}
	for(i=0;i<nLabel;i++) {
		ptBlobIndexAndLabel = CPoint(nSegmentIndex, pnLabel[i]);
		pLensBlobLabeling->SegmentIndexAndLabelAry.Add(ptBlobIndexAndLabel);
	}
	nPtNumberByContrast = lBestMatchingSum;
	nPtPercentageByContrast = 100 * nPtNumberByContrast/nptTeachNo;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CPointMatching::InspectionLabelByBinaryRough");
	goto ErrorAbort;
}
	if(pnLabel != NULL)
		delete [] pnLabel;
	return 0;
ErrorAbort:
	if(pnLabel != NULL)
		delete [] pnLabel;
	return -1;
}

int CPointMatching::InspectionByBinaryRough(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufSrc,
					   CRect* prcInsp,
					   CPoint* pptBestMatching,
					   int nBlobType,
					   CSize* pcsShift,
					   long lDebugFlag)
{
	int i, j, m;
	CRect rcTeachShift;
	int nPitch;
	long lIndex;
	long lSum;
	long lOffset;
	int nShiftXRangeTol, nShiftYRangeTol; 
	CRect rcTeachShiftOrg;
	CRect rcBestMatchingShift;
	CRect rcValidInsp;
	BYTE* pImgBufSrcPtr;
	long lEdgePtSum;

try
{
	nPtNumberByContrast = 0;
	nPtPercentageByContrast = 0;
	rcTeachShift = rcTeach + *pptBestMatching;
	rcTeachShiftOrg = rcTeachShift;
	if(RectInRect(*prcInsp, rcTeachShift) == 0)
		goto ErrorAbort;

	rcValidInsp = *prcInsp;
	rcValidInsp.DeflateRect(*pcsShift);
	nShiftXRangeTol = 2 * pcsShift->cx + 1;
	nShiftYRangeTol = 2 * pcsShift->cy + 1;
	lBestMatchingSum = -1;
	pImgBufSrcPtr = pImgBufSrc->GetImgPtr();
	nPitch = pImgBufSrc->GetPitch();
	CreateAddress(nPitch);
	ptBestMatching = CPoint(0,0);
	rcTeachShift.InflateRect(*pcsShift);
	CheckRect(&rcValidInsp, &rcTeachShift);

	if(nBlobType == WHITE) {
		lBestMatchingSum = 0;
		for(i=rcTeachShift.top - rcTeach.top;i<=(rcTeachShift.bottom - rcTeach.bottom);i++) {
			for(j=rcTeachShift.left - rcTeach.left;j<=(rcTeachShift.right - rcTeach.right);j++) {
				lOffset = i*nPitch + j;
				lSum = 0;
				for(m=0;m<nptTeachNo;m++) {
					lIndex = plAddress[m] + lOffset;
					if(pImgBufSrcPtr[lIndex]) 
						lSum++;
				}
				if(lBestMatchingSum < lSum) {
					lBestMatchingSum = lSum;
					ptBestMatching = CPoint(j, i);
				}
			}
		}
	}
	else {
		lBestMatchingSum = 0;
		lEdgePtSum = 0;
		for(i=rcTeachShift.top - rcTeach.top;i<=(rcTeachShift.bottom - rcTeach.bottom);i++) {
			for(j=rcTeachShift.left - rcTeach.left;j<=(rcTeachShift.right - rcTeach.right);j++) {
				lOffset = i*nPitch + j;
				lSum = 0;
				for(m=0;m<nptTeachNo;m++) {
					lIndex = plAddress[m] + lOffset;
					if(pImgBufSrcPtr[lIndex] == 0)
						lSum++;
				}
				if(lBestMatchingSum < lSum) {
					lBestMatchingSum = lSum;
					ptBestMatching = CPoint(j, i);
				}
				lEdgePtSum += lSum;
			}
		}
	}
	nPtNumberByContrast = lBestMatchingSum;
	nPtPercentageByContrast = 100 * nPtNumberByContrast/nptTeachNo;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CPointMatching::InspectionByBinaryRough");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


CEdgeMatching::CEdgeMatching()
{
	pPointMatching = NULL;
	nPointMatchingNo = 0;
	pbInspect = NULL;
	pbFoundMatching = NULL;
	nCircleWidthTeach = -1;
}

CEdgeMatching::~CEdgeMatching()
{
	CleanTeach();
	CleanInspect();
}
void CEdgeMatching::CleanTeach()
{
	if(pbInspect != NULL)
		delete [] pbInspect;
	pbInspect = NULL;
	nCircleWidthTeach = -1;
}
void CEdgeMatching::CleanInspect()
{
	int i;
	if(pbFoundMatching != NULL)
		delete [] pbFoundMatching;
	pbFoundMatching = NULL;
	if(pPointMatching != NULL) {
		for(i=0;i<nPointMatchingNo;i++)
			pPointMatching[i].CleanAll();
		delete [] pPointMatching;
	}
	pPointMatching = NULL;
	nPointMatchingNo = 0;
}

int CEdgeMatching::Teach(CPoint* pptTeach,
					  int nPtTeachNo,
					  int nPtArySize)
{
	int i, j;
	int nPointMatchingNoAllocMem;
	CPoint* pptTeachAry;
	int nIndex;

try
{
	CleanTeach();
	CleanInspect();
	pptTeachAry = NULL;
	if(nPtArySize <= 0)
		goto ErrorAbort;
	if(nPtTeachNo <= 0)
		goto PASS;
	nPointMatchingNoAllocMem = nPtTeachNo/nPtArySize + 1;
	pPointMatching = new CPointMatching[abs(nPointMatchingNoAllocMem)];
	pbInspect = new BOOL[abs(nPointMatchingNoAllocMem)];
	pbFoundMatching = new BOOL[abs(nPointMatchingNoAllocMem)];
	pptTeachAry = new CPoint[abs(nPtArySize)];
	nPointMatchingNo = 0;
	for(i=0;i<nPointMatchingNoAllocMem;i++) {
		pbInspect[i] = TRUE;
		nIndex = nPtArySize * i;
		if(nIndex >= nPtTeachNo)
			break;
		for(j=0;j<nPtArySize;j++) {
			if(nIndex >= nPtTeachNo)
				nIndex -= nPtTeachNo;
			pptTeachAry[j] = pptTeach[nIndex];
			nIndex++;
		}
		pPointMatching[i].SetTeachPointAry(&pptTeachAry[0], nPtArySize);
		nPointMatchingNo++;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CEdgeMatching::Teach");
	goto ErrorAbort;
}
PASS:	
	if(pptTeachAry != NULL)
		delete [] pptTeachAry;
	return 0;
ErrorAbort:
	if(pptTeachAry != NULL)
		delete [] pptTeachAry;
	return -1;
}

int CEdgeMatching::Inspection(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufSrc,
					   CRect* prcInsp,
					   int nBlobType,
					   CPoint* pptCircleShift,
					   CSize* pcsShift,
					   int nCircleWidth,
					   int nContrast,
					   long lDebugFlag)
{
	int i;
	CImageBuffer ImgBufTemp;
	CPoint2D pt2DOffset0;
	CPoint2D pt2DOffset1;
	CPoint2D pt2DBestMatching;
	int* pnPtNumberByContrast;
	CPoint2D pt2DCircleShift;
	int* pnSortingIndex;
	int nSelectSegmentPercentage;
	int nSelectSegmentNo;
	int nError;

try
{
	pnPtNumberByContrast = NULL;
	pnSortingIndex = NULL;
	nSelectSegmentPercentage = 80;
	nSumPtNumberByContrast = 0;
	nSumPtNumber = 0;
	pt2DCircleShift = CPoint2D(*pptCircleShift);
	if(nPointMatchingNo < 1)
		goto ErrorAbort;
	pnPtNumberByContrast = new int[abs(nPointMatchingNo)];
	pnSortingIndex = new int[abs(nPointMatchingNo)];
	if(nCircleWidth != nCircleWidthTeach) {
		for(i=0;i<nPointMatchingNo;i++) {
			nError = pPointMatching[i].ln2DTeach.GetPerpendicularPoints(&pt2DCircleShift, 
										nCircleWidth, 
										&pt2DOffset0,
										&pt2DOffset1);
			if(nError)
				goto ErrorAbort;
			pPointMatching[i].ptOffset0 = CPoint(Round(pt2DOffset0.x - pptCircleShift->x), Round(pt2DOffset0.y - pptCircleShift->y));
			pPointMatching[i].ptOffset1 = CPoint(Round(pt2DOffset1.x - pptCircleShift->x), Round(pt2DOffset1.y - pptCircleShift->y));
		}
		nCircleWidthTeach = nCircleWidth;
	}
	for(i=0;i<nPointMatchingNo;i++) {
		pnPtNumberByContrast[i] = 0;
		if(pbInspect[i] == FALSE)
			continue;
		nError = pPointMatching[i].InspectionByContrast(pView,
						   pImgBufSrc,
						   prcInsp,
						   pptCircleShift,
						   &pPointMatching[i].ptOffset0,
						   &pPointMatching[i].ptOffset1,
						   nContrast,
						   nBlobType,
						   pcsShift,
						   lDebugFlag);
		if(nError)
			goto ErrorAbort;
		pnPtNumberByContrast[i] = pPointMatching[i].nPtNumberByContrast;
		nSumPtNumberByContrast += pnPtNumberByContrast[i];
		nSumPtNumber += pPointMatching[i].nptTeachNo;
	}
	Sorting(&pnPtNumberByContrast[0], nPointMatchingNo, &pnSortingIndex[0], FALSE);
	nSelectSegmentNo = Round(nSelectSegmentPercentage * nPointMatchingNo/100);
	for(i=0;i<nPointMatchingNo;i++)
		pbFoundMatching[i] = FALSE;
	for(i=0;i<nSelectSegmentNo;i++)
		pbFoundMatching[pnSortingIndex[i]] = TRUE;

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CEdgeMatching::Inspection");
	goto ErrorAbort;
}
	if(pnPtNumberByContrast != NULL)
		delete [] pnPtNumberByContrast;
	if(pnSortingIndex != NULL)
		delete [] pnSortingIndex;
	return 0;
ErrorAbort:
	if(pnPtNumberByContrast != NULL)
		delete [] pnPtNumberByContrast;
	if(pnSortingIndex != NULL)
		delete [] pnSortingIndex;
	return -1;
}

int CEdgeMatching::InspectionRough(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufSrc,
					   CRect* prcInsp,
					   int nBlobType,
					   CPoint* pptCircleShift,
					   CSize* pcsShift,
					   int nCircleWidth,
					   int nContrast,
					   long lDebugFlag)
{
	int i;
	CImageBuffer ImgBufTemp;
	CPoint2D pt2DOffset0;
	CPoint2D pt2DOffset1;
	CPoint2D pt2DBestMatching;
	CPoint2D pt2DCircleShift;
	int nError;

try
{
	nSumPtNumberByContrast = 0;
	nSumPtNumber = 0;
	pt2DCircleShift = CPoint2D(*pptCircleShift);

	if(nCircleWidth != nCircleWidthTeach) {
		for(i=0;i<nPointMatchingNo;i++) {
			nError = pPointMatching[i].ln2DTeach.GetPerpendicularPoints(&pt2DCircleShift, 
										nCircleWidth, 
										&pt2DOffset0,
										&pt2DOffset1);
			if(nError)
				goto ErrorAbort;
			pPointMatching[i].ptOffset0 = CPoint(Round(pt2DOffset0.x - pptCircleShift->x), Round(pt2DOffset0.y - pptCircleShift->y));
			pPointMatching[i].ptOffset1 = CPoint(Round(pt2DOffset1.x - pptCircleShift->x), Round(pt2DOffset1.y - pptCircleShift->y));
		}
		nCircleWidthTeach = nCircleWidth;
	}
	for(i=0;i<nPointMatchingNo;i++) {
		if(pbInspect[i] == FALSE)
			continue;
		nError = pPointMatching[i].InspectionByContrastRough(pView,
						   pImgBufSrc,
						   prcInsp,
						   pptCircleShift,
						   &pPointMatching[i].ptOffset0,
						   &pPointMatching[i].ptOffset1,
						   nContrast,
						   nBlobType,
						   pcsShift,
						   lDebugFlag);
		if(nError)
			goto ErrorAbort;
		nSumPtNumberByContrast += pPointMatching[i].nPtNumberByContrast;
		nSumPtNumber += pPointMatching[i].nptTeachNo;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CEdgeMatching::InspectionRough");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CEdgeMatching::InspectionRough2(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufSrc,
					   CRect* prcInsp,
					   int nBlobType,
					   CPoint* pptCircleShift,
					   CSize* pcsShift,
					   int nCircleWidth,
					   int nContrast,
					   int nFoundMatchingScoreMax,
					   long lDebugFlag)
{
	int i;
	CImageBuffer ImgBufTemp;
	CPoint2D pt2DOffset0;
	CPoint2D pt2DOffset1;
	CPoint2D pt2DBestMatching;
	CPoint2D pt2DCircleShift;
	int nMatchingPtNoTol;
	int nPtUsedForMatching;
	int nError;

try
{
	nSumPtNumberByContrast = 0;
	nSumPtNumber = 0;
	pt2DCircleShift = CPoint2D(*pptCircleShift);

	if(nCircleWidth != nCircleWidthTeach) {
		for(i=0;i<nPointMatchingNo;i++) {
			nError = pPointMatching[i].ln2DTeach.GetPerpendicularPoints(&pt2DCircleShift, 
										nCircleWidth, 
										&pt2DOffset0,
										&pt2DOffset1);
			if(nError)
				goto ErrorAbort;
			pPointMatching[i].ptOffset0 = CPoint(Round(pt2DOffset0.x - pptCircleShift->x), Round(pt2DOffset0.y - pptCircleShift->y));
			pPointMatching[i].ptOffset1 = CPoint(Round(pt2DOffset1.x - pptCircleShift->x), Round(pt2DOffset1.y - pptCircleShift->y));
		}
		nCircleWidthTeach = nCircleWidth;
	}
	for(i=0;i<nPointMatchingNo;i++) {
		if(pbInspect[i] == FALSE)
			continue;
		nSumPtNumber += pPointMatching[i].nptTeachNo;
	}
	nMatchingPtNoTol = nSumPtNumber * nFoundMatchingScoreMax/100;
	nPtUsedForMatching = 0;
	for(i=0;i<nPointMatchingNo;i++) {
		if(pbInspect[i] == FALSE)
			continue;
		nError = pPointMatching[i].InspectionByContrastRough(pView,
						   pImgBufSrc,
						   prcInsp,
						   pptCircleShift,
						   &pPointMatching[i].ptOffset0,
						   &pPointMatching[i].ptOffset1,
						   nContrast,
						   nBlobType,
						   pcsShift,
						   lDebugFlag);
		if(nError)
			goto ErrorAbort;
		nSumPtNumberByContrast += pPointMatching[i].nPtNumberByContrast;
		nPtUsedForMatching += pPointMatching[i].nptTeachNo;
		if((nSumPtNumber - nPtUsedForMatching + nSumPtNumberByContrast) < nMatchingPtNoTol)
			break;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CEdgeMatching::InspectionRough2");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CEdgeMatching::InspectionLabelBinaryRough(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufBin,
					   CRect* prcBin,
					   int nBlobType,
					   CPoint* pptCircleShift,
					   CSize* pcsShift,
					   int nFoundMatchingScoreMax,
					   CLensBlobLabeling* pLensBlobLabeling,
					   long lDebugFlag)
{
	int i;
	CImageBuffer ImgBufTemp;
	CPoint2D pt2DBestMatching;
	CPoint2D pt2DCircleShift;
	int nMatchingPtNoTol;
	int nPtUsedForMatching;
	int nError;

try
{
	pLensBlobLabeling->CleanLabel();
	nSumPtNumberByContrast = 0;
	nSumPtNumber = 0;
	pt2DCircleShift = CPoint2D(*pptCircleShift);
	for(i=0;i<nPointMatchingNo;i++) {
		if(pbInspect[i] == FALSE)
			continue;
		nSumPtNumber += pPointMatching[i].nptTeachNo;
	}
	nMatchingPtNoTol = nSumPtNumber * nFoundMatchingScoreMax/100;
	nPtUsedForMatching = 0;
	for(i=0;i<nPointMatchingNo;i++) {
		if(pbInspect[i] == FALSE)
			continue;
		nError = pPointMatching[i].InspectionLabelByBinaryRough(pView,
						   pImgBufBin,
						   prcBin,
						   pptCircleShift,
						   nBlobType,
						   pcsShift,
						   i,
						   pLensBlobLabeling,
						   lDebugFlag);
		if(nError)
			goto ErrorAbort;
		nSumPtNumberByContrast += pPointMatching[i].nPtNumberByContrast;
		nPtUsedForMatching += pPointMatching[i].nptTeachNo;
		if((nSumPtNumber - nPtUsedForMatching + nSumPtNumberByContrast) < nMatchingPtNoTol)
			break;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CEdgeMatching::InspectionLabelBinaryRough");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
	
}


int CEdgeMatching::InspectionBinaryRough(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufBin,
					   CRect* prcBin,
					   int nBlobType,
					   CPoint* pptCircleShift,
					   CSize* pcsShift,
					   int nFoundMatchingScoreMax,
					   long lDebugFlag)
{
	int i;
	CImageBuffer ImgBufTemp;
	CPoint2D pt2DBestMatching;
	CPoint2D pt2DCircleShift;
	int nMatchingPtNoTol;
	int nPtUsedForMatching;
	int nError;

try
{
	nSumPtNumberByContrast = 0;
	nSumPtNumber = 0;
	pt2DCircleShift = CPoint2D(*pptCircleShift);
	for(i=0;i<nPointMatchingNo;i++) {
		if(pbInspect[i] == FALSE)
			continue;
		nSumPtNumber += pPointMatching[i].nptTeachNo;
	}
	nMatchingPtNoTol = nSumPtNumber * nFoundMatchingScoreMax/100;
	nPtUsedForMatching = 0;
	for(i=0;i<nPointMatchingNo;i++) {
		if(pbInspect[i] == FALSE)
			continue;
		nError = pPointMatching[i].InspectionByBinaryRough(pView,
						   pImgBufBin,
						   prcBin,
						   pptCircleShift,
						   nBlobType,
						   pcsShift,
						   lDebugFlag);
		if(nError)
			goto ErrorAbort;
		nSumPtNumberByContrast += pPointMatching[i].nPtNumberByContrast;
		nPtUsedForMatching += pPointMatching[i].nptTeachNo;
		if((nSumPtNumber - nPtUsedForMatching + nSumPtNumberByContrast) < nMatchingPtNoTol)
			break;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CEdgeMatching::InspectionBinaryRough");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CEdgeMatching::InspectionBinaryAccumulation(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufBin,
					   CRect* prcBin,
					   int nBlobType,
					   CPoint* pptCircleShift,
					   CSize* pcsShift,
					   long lDebugFlag)
{
	int i;
	CPoint2D pt2DBestMatching;
	CPoint2D pt2DCircleShift;
	int nError;

try
{
	pt2DCircleShift = CPoint2D(*pptCircleShift);

	nSumPtNumberByContrast = 0;
	nSumPtNumber = 0;
	for(i=0;i<nPointMatchingNo;i++) {
		if(pbInspect[i] == FALSE)
			continue;
		nSumPtNumber += pPointMatching[i].nptTeachNo;
		nError = pPointMatching[i].InspectionBinaryAccumulation(pView,
						   pImgBufBin,
						   prcBin,
						   pptCircleShift,
						   nBlobType,
						   pcsShift,
						   lDebugFlag);
		if(nError)
			goto ErrorAbort;
		nSumPtNumberByContrast += pPointMatching[i].nPtNumberByContrast;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CEdgeMatching::InspectionBinaryAccumulation");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

CCircleFitting::CCircleFitting()
{
	pt2DBestCtr = CPoint2D(0,0);
	dBestRadius = 0;
	dRadiusVar = 0;
}

CCircleFitting::~CCircleFitting()
{
	Clean();
}

void CCircleFitting::Clean()
{
	pt2DBestCtr = CPoint2D(0,0);
	dBestRadius = 0;
	dRadiusVar = 0;
}

int CCircleFitting::Inspect(CPoint* pptCircle,
							int nPtCircle,
							int nNominalRadius,
							CSize* pcsCenterShift,
							CRect* prcFOV)
{
	double dPercentage;
	int nError; 

try
{
	Clean();
	dPercentage = 0.6;
	if(nNominalRadius < 0) 
		goto ErrorAbort;
	nNominalRadiusVal = nNominalRadius;
	rcCtrRange = CRect(Round(nNominalRadius*dPercentage),
					   Round(nNominalRadius*dPercentage),
					   prcFOV->Width() - Round(nNominalRadius*dPercentage),
					   prcFOV->Height() - Round(nNominalRadius*dPercentage));
	if(nPtCircle <= 3)
		goto ErrorAbort;
	csCenterRange = CSize(2*pcsCenterShift->cx + 1, 2*pcsCenterShift->cy + 1);
//	nError = FindDistanceTol(&pptCircle[0],
//							 nPtCircle);
	nDisTol = nNominalRadius/2;

	nError = FindCircleCtr(&pptCircle[0],
							 nPtCircle,
							 &csCenterRange);
	if(nError)
		goto ErrorAbort;
	nError = CircleBestRadius(&pptCircle[0], 
							  nPtCircle,
							  &csCenterRange);
	if(nError)
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleFitting::Inspect");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CCircleFitting::FindCircleCtr(CPoint* pptCircle,
								  int nPtCircle,
								  CSize* pcsCenterRange)
{
	int i, j, m, n;
	CLine2D ln2DEdge;
	CPoint2D pt2DEdgeCtr;
	CPoint2D* ppt2DCtr;
	int nPt2DCtrNo;
	CLine2D* pln2D;
	int nLn2DNo;
	int nDis;
	double dAngle0;
	double dAngle1;
	int nAngleDif;
	CRect rcCircle;
	CRect rcCenter;
	CImgIntBuf ImgIntBufCtr;
	CRect rcCenterRelative;
	int* pImgIntBufCtrPtr;
	int nPitch;
	int nIndex;
	int nError;
	int nx, ny;
	int nWidthSub1, nHeightSub1;
	CImageBuffer ImgBufNorm;

try
{
	ppt2DCtr = NULL;
	pln2D = NULL;

	rcCircle = rcCtrRange; 
	if(rcCircle.left < 0)
		rcCircle.left = 0;
	if(rcCircle.top < 0)
		rcCircle.top = 0;
	if(rcCircle.right < 0)
		rcCircle.right = 0;
	if(rcCircle.bottom < 0)
		rcCircle.bottom = 0;
	ppt2DCtr = new CPoint2D[abs(nPtCircle * nPtCircle * nPtCircle)];
	nPt2DCtrNo = 0;
	for(i=0;i<nPtCircle;i++) {
		if(pln2D != NULL)
			delete [] pln2D;
		pln2D = NULL;
		pln2D = new CLine2D[abs(nPtCircle)];
		nLn2DNo = 0;
		for(j=0;j<nPtCircle;j++) {
			if(i==j)
				continue;
			nDis = abs(pptCircle[i].x - pptCircle[j].x) + abs(pptCircle[i].y - pptCircle[j].y);
			if(nDis < nDisTol)
				continue;
			pt2DEdgeCtr = CPoint2D((pptCircle[i].x + pptCircle[j].x)/2.0, (pptCircle[i].y + pptCircle[j].y)/2.0);
			ln2DEdge = CLine2D(pptCircle[i], pptCircle[j]);
			nError = ln2DEdge.GetPerpendicularLine(&pt2DEdgeCtr, &pln2D[nLn2DNo]);
			if(nError)
				goto ErrorAbort;
			if(nError == 0)
				nLn2DNo++;
		}
		if(nLn2DNo < 2)
			continue;
		for(m=0;m<nLn2DNo;m++) {
			for(n=0;n<nLn2DNo;n++) {
				if(m == n)
					continue;
				pln2D[m].GetAngleDeg(dAngle0);
				pln2D[n].GetAngleDeg(dAngle1);
				nAngleDif = abs(Round(dAngle1 - dAngle0));
				nAngleDif = nAngleDif % 90;
				if(nAngleDif < 30)
					continue;
				nError = pln2D[m].GetIntercept(&pln2D[n], &ppt2DCtr[nPt2DCtrNo]);	
				if(nError)
					goto ErrorAbort;
				if(nError == 0)
					nPt2DCtrNo++;
			}
		}
	}
	nError = GetPointsBoundingRect(&ppt2DCtr[0], 
						  nPt2DCtrNo,
						  &rcCenter);
	if(nError)
		goto ErrorAbort;
	rcCenter.InflateRect(pcsCenterRange->cx, pcsCenterRange->cy,pcsCenterRange->cx, pcsCenterRange->cy);
	nError = CheckRect(&rcCircle,&rcCenter);
	if(nError) {
		pt2DBestCtr = CPoint2D(rcCircle.CenterPoint());
		goto PASS;
	}
	rcCenterRelative = CRect(CPoint(0,0), rcCenter.Size());
	ImgIntBufCtr.Allocate(AbsSize(rcCenterRelative.Size()));
	ImgIntBufCtr.ClrImg(0);
	pImgIntBufCtrPtr = ImgIntBufCtr.GetImgPtr();
	nPitch = ImgIntBufCtr.GetPitch();
	nWidthSub1 = rcCenterRelative.Width() - 1;
	nHeightSub1 = rcCenterRelative.Height() - 1;
	for(i=0;i<nPt2DCtrNo;i++) {
		nx = int(ppt2DCtr[i].x) - rcCenter.left;
		ny = int(ppt2DCtr[i].y) - rcCenter.top;
		if((nx < 0) || (ny < 0) || (nx > nWidthSub1) || (ny > nHeightSub1))
			continue;
		nIndex = ny * nPitch + nx;
		pImgIntBufCtrPtr[nIndex]++;
	}
	nError = ImgIntBufCtr.ConvertNormalize(&ImgBufNorm);
	if(nError)
		goto ErrorAbort;
	nError = CircleBestCenter(&ImgBufNorm,
							  &rcCenterRelative,
							  pcsCenterRange,
							  WHITE,
							  &pt2DBestCtr);
	if(nError)
		goto ErrorAbort;
	pt2DBestCtr = CPoint2D(pt2DBestCtr.x + rcCenter.left + pcsCenterRange->cx/2.0,pt2DBestCtr.y + rcCenter.top + pcsCenterRange->cy/2.0);
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleFitting::FindCircleCtr");
	goto ErrorAbort;
}
PASS:
	if(pln2D != NULL)
		delete [] pln2D;
	if(ppt2DCtr != NULL)
		delete [] ppt2DCtr;
	return 0;
ErrorAbort:
	if(pln2D != NULL)
		delete [] pln2D;
	if(ppt2DCtr != NULL)
		delete [] ppt2DCtr;
	return -1;
}


int CCircleFitting::CircleBestRadius(CPoint* pptEdge, 
							  int nPtEdgeNo,
							  CSize* pcsCenterRange)
{
	int i;
	double* pdRadius;
	CAdvHistogram AdvHistogram;
	double dHistConv;
	int	nMaxHist;
	double dRadiusDif;
	double dRadiusDifMax;
	int nError;

try
{
	pdRadius = NULL;
	if(nPtEdgeNo <= 3)
		goto ErrorAbort;
	pdRadius = new double[abs(nPtEdgeNo)];
	for(i=0;i<nPtEdgeNo;i++) {
		pdRadius[i] = sqrt((pt2DBestCtr.x - pptEdge[i].x) * (pt2DBestCtr.x - pptEdge[i].x) + 
			(pt2DBestCtr.y - pptEdge[i].y) * (pt2DBestCtr.y - pptEdge[i].y));
	}
	dHistConv = __max(pcsCenterRange->cx * 2, pcsCenterRange->cy * 2);
	nError = AdvHistogram.GetMaxConvHist1D(&pdRadius[0], 
									nPtEdgeNo,
									dHistConv,
									nMaxHist,
									dBestRadius);
	if(nError)
		goto ErrorAbort;

	dRadiusDifMax = 0;
	for(i=0;i<nPtEdgeNo;i++) {
		dRadiusDif = fabs(pdRadius[i] - dBestRadius);
		if(dRadiusDifMax < dRadiusDif)
			dRadiusDifMax = dRadiusDif;
	}
	if(dRadiusDifMax > dHistConv)
		dRadiusDifMax = dHistConv;
	dRadiusVar = dRadiusDifMax;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleFitting::CircleBestRadius");
	goto ErrorAbort;
}
	if(pdRadius != NULL)
		delete [] pdRadius;
	return 0;
ErrorAbort:
	if(pdRadius != NULL)
		delete [] pdRadius;
	return -1;
}


int CCircleFitting::FindDistanceTol(CPoint* pptCircle,
									int nPtCircle)
{
	int i, j;
	long lSum;
	int nSumNo;
	int nLength;
	int nLengthMax;

try
{
	if(nPtCircle <= 3)
		goto ErrorAbort;
	lSum = 0;
	nSumNo = 0;
	nLengthMax = 0;
	for(i=0;i<nPtCircle;i++) {
		for(j=0;j<nPtCircle;j++) {
			if(i == j)
				continue;
			nLength = abs(pptCircle[i].x - pptCircle[j].x) + abs(pptCircle[i].y - pptCircle[j].y);
			lSum += nLength;
			nSumNo++;
			if(nLengthMax < nLength)
				nLengthMax = nLength;
		}
	}
	nDisTol = lSum/nSumNo;
	if(nDisTol > nLengthMax/8)
		nDisTol = nLengthMax/8;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleFitting::FindDistanceTol");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CCircleFitting::CircleBestCenter(CImageBuffer* pImgSrc,
									 CRect* prcSrc,
									 CSize* pcsProj,
									 int nBlobType,
									 CPoint2D* ppt2DBestCtr)

{

	int i, j;
	int nImgPitchSrc;
	int nWidthSrc, nHeightSrc;
	BYTE *pImgSrcPtr;
	int *pProjVPtr;
	CRect rcFirstProj;
	long lImgSrcLostRowIndex;
	long lImgSrcAddRowIndex;
	long lProjVRowIndex;
	CRect rcProjV;
	long *plXAry;
	long *plYAry;
	long lProjRowIndex;
	int nError;
	int nProjHeight,nProjWidth,nProjPitch;
	int nProjVWidth,nProjVPitch,nProjVHeight;
	int* pnProj;
	int *pIntPtr,*pRowptr,*pColptr;
	CRect* prcProj;
	CRect rcProj;
	int nMaxVal;
	CPoint ptMaxPos;
	long lSumX;
	long lSumY;
	long lWeight;
	int nx, ny;
	long lIndex;

try
{
	plXAry = NULL;
	plYAry = NULL;
	pnProj = NULL;
	pProjVPtr = NULL;
	rcProj  =CRect(prcSrc->left,prcSrc->top,prcSrc->right-pcsProj->cx,prcSrc->bottom-pcsProj->cy);
	prcProj = &rcProj;
	if((prcProj->Width() <= 0) || (prcProj->Height() <= 0)) {
		goto PASS;
	}
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
	if((pcsProj->cx<=0) || (pcsProj->cy<=0)) {
		return -1;
	}
	if(((prcSrc->Width() - pcsProj->cx) != prcProj->Width()) ||
	   ((prcSrc->Height() - pcsProj->cy) != prcProj->Height()))
	   return -1;

	nProjHeight = prcProj->Height(); 
	nProjWidth = prcProj->Width();
	nProjWidth = nProjWidth % 4;
	if (nProjWidth > 0)
		nProjWidth = 4 - nProjWidth;
	nProjPitch = prcProj->Width() + nProjWidth; 
	pnProj = new int[abs(nProjHeight*nProjPitch)];
	memset((void*)pnProj,0,nProjHeight*nProjPitch*sizeof(int));
	nProjWidth = prcProj->Width();	
	rcProjV = CRect(CPoint(0,0), CSize(prcSrc->Width(), prcSrc->Height() - pcsProj->cy));

	nProjVHeight = rcProjV.Height(); 
	nProjVWidth = rcProjV.Width();
	nProjVWidth = nProjVWidth % 4;
	if (nProjVWidth > 0)
		nProjVWidth = 4 - nProjVWidth;
	nProjVPitch = rcProjV.Width() + nProjVWidth; 
	pProjVPtr = new int[abs(nProjVHeight*nProjVPitch)];
	memset((void*)pProjVPtr,0,nProjVHeight*nProjVPitch*sizeof(int));

	nImgPitchSrc = pImgSrc->GetPitch();
	nWidthSrc = rcProjV.Width();
	nHeightSrc = rcProjV.Height();
	pImgSrcPtr = pImgSrc->GetImgPtr() + nImgPitchSrc * prcSrc->top + prcSrc->left;

	plXAry = new long[abs(prcSrc->Width())];
	rcFirstProj = CRect(prcSrc->TopLeft(), CSize(prcSrc->Width(), pcsProj->cy));
	nError = GetXGrayAccl(pImgSrc, &rcFirstProj, &plXAry[0]);
	for(i=0;i<nWidthSrc;i++)
		pProjVPtr[i] = plXAry[i];

	lImgSrcLostRowIndex = 0;
	lImgSrcAddRowIndex = nImgPitchSrc * (pcsProj->cy);
	lProjVRowIndex = nProjVPitch;
	for(i=1;i<nHeightSrc;i++) {
		for(j=0;j<nWidthSrc;j++) {
			pProjVPtr[lProjVRowIndex + j] = pProjVPtr[lProjVRowIndex - nProjVPitch + j] + 
					pImgSrcPtr[lImgSrcAddRowIndex + j] - pImgSrcPtr[lImgSrcLostRowIndex+j];	
		}
		lImgSrcLostRowIndex += nImgPitchSrc;
		lImgSrcAddRowIndex += nImgPitchSrc;
		lProjVRowIndex += nProjVPitch;
	}

	nWidthSrc = prcProj->Width();
	nHeightSrc = prcProj->Height();
	plYAry = new long[abs(nHeightSrc)];
	lProjRowIndex = 0;
	lProjVRowIndex = 0;
	for(i=0;i<nHeightSrc;i++) {
		plYAry[i] = 0;
		for(j=0;j<pcsProj->cx;j++) {
			plYAry[i] += pProjVPtr[lProjVRowIndex + j];
		}
		pnProj[lProjRowIndex] = plYAry[i];
		lProjVRowIndex += nProjVPitch;
		lProjRowIndex += nProjPitch;
	}
	lProjRowIndex = 0;
	lProjVRowIndex = 0;
	for(i=0;i<nHeightSrc;i++) {
		for(j=1;j<nWidthSrc;j++) {
			pnProj[lProjRowIndex+j] = pnProj[lProjRowIndex+j-1] + 
				pProjVPtr[lProjVRowIndex + j -1+ pcsProj->cx] - pProjVPtr[lProjVRowIndex + j -1];
		}
		lProjRowIndex += nProjPitch;
		lProjVRowIndex += nProjVPitch;
	}

	nMaxVal = 0;
	ptMaxPos = CPoint(0,0);
	pColptr = pRowptr = pIntPtr = pnProj;
	for(i = 0,pRowptr =pIntPtr;i<nProjHeight ;i++,pRowptr+=nProjPitch){
		for(j = 0, pColptr=pRowptr;j<nProjWidth;j++,pColptr++){
			if(nMaxVal < *pColptr){
				nMaxVal = *pColptr;
				ptMaxPos = CPoint(j, i);
			}
		}
	}
	lSumX = 0;
	lSumY = 0;
	lWeight = 0;
	for(i=-pcsProj->cy/2;i<=(pcsProj->cy/2);i++) {
		for(j=-pcsProj->cx/2;j<=(pcsProj->cx/2);j++) {
			nx = ptMaxPos.x + j;
			ny = ptMaxPos.y + i;
			if((nx < 0) || (ny < 0) ||
				(nx >= nWidthSrc) || (ny >= nHeightSrc))
				continue;
			lIndex = ny * nProjPitch + nx;
			lWeight += pnProj[lIndex];
			lSumX += pnProj[lIndex] * nx;
			lSumY += pnProj[lIndex] * ny;
		}
	}
	if(lWeight <= 0)
		goto ErrorAbort;
	ppt2DBestCtr->x = (double)lSumX/lWeight;
	ppt2DBestCtr->y = (double)lSumY/lWeight;

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CCircleFitting::CircleBestCenter");
	goto ErrorAbort;
}
PASS:
	if(plXAry != NULL)
		delete [] plXAry;
	if(plYAry != NULL)
		delete [] plYAry;
	if(pnProj!=NULL)
		delete [] pnProj;
	if(pProjVPtr!=NULL)
		delete [] pProjVPtr;
	return 0;
ErrorAbort:
	if(plXAry != NULL)
		delete [] plXAry;
	if(plYAry != NULL)
		delete [] plYAry;
	if(pnProj!=NULL)
		delete [] pnProj;
	if(pProjVPtr!=NULL)
		delete [] pProjVPtr;
	return -1;
}
int GetSeedsFillImage(CImageBuffer *pImgBinSrc,
					 CRect* prcBinSrc,
					 CImageBuffer *pImgBinSeed,
					 CRect* prcBinSeed,
					 CPoint* pptBinSeedTopLftInrcBinSrc,
					 int nBlobType,
					 CImageBuffer *pImgBinDst,
					 CRect* prcBinDstRelative,
					 CPoint* pptBinDstTopLftInrcBinSrc)
{
	int x, y;
	int k;
	long lm;
	long lIndex;
	long* plSeedsAddr1;
	long lSeedsNo1;
	long* plSeedsAddr2;
	long lSeedsNo2;
	BYTE* pbyteBinSrcPtr;
	int nBinSrcPitch;
	BYTE* pbyteBinSeedPtr;
	int nBinSeedPitch;
	BYTE* pbyteBinDstPtr;
	int nBinDstPitch;
	int nError;
	int nDir[8];
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	int nWidthSrc,nHeightSrc;
	int nx;
	int ny;
	int nLft;
	int nTop;
	int nRht;
	int nBot;
	CRect rcOverlap;
	CRect rcSeedInSrc;
	CRect rcOverlapInSeed;
	long lRowSeedIndex;
	long lRowSrcIndex;
	int nSrcTopLftInDstX;
	int nSrcTopLftInDstY;
	int nBlobWidth;
	int nBlobHeight;
	int nLftSrc;
	int nTopSrc;
	int nRhtSrc;
	int nBotSrc;

try
{
	plSeedsAddr1 = NULL;
	plSeedsAddr2 = NULL;
	nLftSrc = prcBinSrc->left;
	nTopSrc = prcBinSrc->top;
	nRhtSrc = prcBinSrc->right - 1;
	nBotSrc = prcBinSrc->bottom - 1;

	if (!pImgBinSrc) {
		ASSERT(!"pImgBinSrc is a bad Pointer");
		return -1;
	}
	if (!pImgBinSrc->IsAllocated()) {
		ASSERT(!"pImgBinSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcBinSrc,&(pImgBinSrc->GetImgSize()))) {
		ASSERT(!"prcBinSrc is Invalid");
		return -1;
	}
	if (!pImgBinSeed) {
		ASSERT(!"pImgBinSeed is a bad Pointer");
		return -1;
	}
	if (!pImgBinSeed->IsAllocated()) {
		ASSERT(!"pImgBinSeed is not Allocated");
		return -1;
	}
	if (ValidateRect(prcBinSeed,&(pImgBinSeed->GetImgSize()))) {
		ASSERT(!"prcBinSeed is Invalid");
		return -1;
	}

	pbyteBinSrcPtr = pImgBinSrc->GetImgPtr();
	nBinSrcPitch = pImgBinSrc->GetPitch();
	pbyteBinSeedPtr = pImgBinSeed->GetImgPtr();
	nBinSeedPitch = pImgBinSeed->GetPitch();
	pbyteBinDstPtr = pImgBinDst->GetImgPtr();
	nBinDstPitch = pImgBinDst->GetPitch();
	nWidthSrc = prcBinSrc->Width();
	nHeightSrc = prcBinSrc->Height();

	nDir[0] = 1;
	nDir[1] = -(nBinSrcPitch - 1);
	nDir[2] = -nBinSrcPitch;
	nDir[3] = -(nBinSrcPitch + 1);
	nDir[4] = -1;
	nDir[5] = nBinSrcPitch - 1;
	nDir[6] = nBinSrcPitch;
	nDir[7] = nBinSrcPitch + 1;

	rcSeedInSrc = CRect(*pptBinSeedTopLftInrcBinSrc, prcBinSeed->Size());
	if(RectOverlapRect(*prcBinSrc, rcSeedInSrc, &rcOverlap) == FALSE)
		goto ErrorAbort;
	rcOverlapInSeed = rcOverlap - *pptBinSeedTopLftInrcBinSrc;
	BYTE* pValue;
	if(nBlobType == WHITE) {
		nError = ClearImageROIEdge(pImgBinSrc,
								   prcBinSrc,
								   0);
		if(nError)
			goto ErrorAbort;
		plSeedsAddr1 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
		plSeedsAddr2 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
		lSeedsNo1 = 0;
		lRowSrcIndex = rcOverlap.top * nBinSrcPitch + rcOverlap.left;
		lRowSeedIndex = rcOverlapInSeed.top * nBinSeedPitch + rcOverlapInSeed.left;
		for(y = 0; y < rcOverlap.Height(); y++) {
			for(x = 0; x < rcOverlap.Width(); x++) {
				if((pbyteBinSrcPtr[lRowSrcIndex+x] == 255) && (pbyteBinSeedPtr[lRowSeedIndex+x] == 255)) 
					plSeedsAddr1[lSeedsNo1++] = lRowSrcIndex+x;
			}
			lRowSrcIndex += nBinSrcPitch;
			lRowSeedIndex += nBinSeedPitch;
		}
		if(lSeedsNo1 > 0) {
			nx = plSeedsAddr1[0] % nBinSrcPitch;
			ny = plSeedsAddr1[0] / nBinSrcPitch;
			nLft = nx;
			nTop = ny;
			nRht = nx;
			nBot = ny;
		}
		else {
			*pptBinDstTopLftInrcBinSrc = prcBinSrc->TopLeft();
			*prcBinDstRelative = CRect(0,0,1,1);
			pImgBinDst->Allocate(AbsSize(prcBinDstRelative->Size()));
			pImgBinDst->ClrImg(0);
			goto PASS;
		}
		while(lSeedsNo1 > 0) {
			lSeedsNo2 = 0;
			for(lm = 0; lm < lSeedsNo1; lm++) {
				lIndex = plSeedsAddr1[lm];
				pbyteBinSrcPtr[lIndex] = 200;
				nx = lIndex % nBinSrcPitch;
				ny = lIndex / nBinSrcPitch;
				if(nLft >= nx)
				{
					nLft = nx;
					if(nLftSrc >= nLft)
						continue;
				}
				if(nRht <= nx)
				{
					nRht = nx;
					if(nRhtSrc <= nRht)
						continue;
				}
				if(nTop >= ny)
				{
					nTop = ny;
					if(nTopSrc >= nTop)
						continue;
				}
				if(nBot <= ny)
				{
					nBot = ny;
					if(nBotSrc <= nBot)
						continue;
				}
				for(k = 0; k < 8; k++) {
					pValue = &pbyteBinSrcPtr[lIndex + nDir[k]];
					if(*pValue == 255) {
						plSeedsAddr2[lSeedsNo2++] = lIndex + nDir[k];
						*pValue = 200;
					}
				}
			}
			if(lSeedsNo2 == 0)
				break;
			lSeedsNo1 = 0;
			for(lm = 0; lm < lSeedsNo2; lm++) {
				lIndex = plSeedsAddr2[lm];
				pbyteBinSrcPtr[lIndex] = 200;
				nx = lIndex % nBinSrcPitch;
				ny = lIndex / nBinSrcPitch;
				if(nLft >= nx)
				{
					nLft = nx;
					if(nLftSrc >= nLft)
						continue;
				}
				if(nRht <= nx)
				{
					nRht = nx;
					if(nRhtSrc <= nRht)
						continue;
				}
				if(nTop >= ny)
				{
					nTop = ny;
					if(nTopSrc >= nTop)
						continue;
				}
				if(nBot <= ny)
				{
					nBot = ny;
					if(nBotSrc <= nBot)
						continue;
				}
				for(k = 0; k < 8; k++) {
					pValue = &pbyteBinSrcPtr[lIndex + nDir[k]];
					if(*pValue == 255) {
						plSeedsAddr1[lSeedsNo1++] = lIndex + nDir[k];
						*pValue = 200;
					}
				}
			}
		}
		nBlobWidth = nRht - nLft + 1;
		nBlobHeight = nBot - nTop + 1;
		if((nLft >= 1) && (nTop >= 1)) {
			*prcBinDstRelative = CRect(CPoint(0,0), CSize(nRht - nLft + 3, nBot - nTop + 3));
			*pptBinDstTopLftInrcBinSrc = CPoint(nLft-1, nTop-1);
			nSrcTopLftInDstX = 1;
			nSrcTopLftInDstY = 1;
		}
		else {
			*prcBinDstRelative = CRect(CPoint(0,0), CSize(nRht - nLft + 1, nBot - nTop + 1));
			*pptBinDstTopLftInrcBinSrc = CPoint(nLft, nTop);
			nSrcTopLftInDstX = 0;
			nSrcTopLftInDstY = 0;
		}

		pImgBinDst->Allocate(AbsSize(prcBinDstRelative->Size()));
		pImgBinDst->ClrImg(0);
		nBinDstPitch = pImgBinDst->GetPitch();
		pRowSrc = pColSrc = pImgBinSrc->GetImgPtr() + nBinSrcPitch * nTop + nLft;
		pRowDst = pColDst = pImgBinDst->GetImgPtr() + nBinDstPitch * nSrcTopLftInDstY + nSrcTopLftInDstX;
		for (y=0; y<nBlobHeight; y++,pRowSrc+=nBinSrcPitch,pRowDst+=nBinDstPitch) {
			for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nBlobWidth; x++,pColSrc++,pColDst++) {
				if(*pColSrc == 200) {
					*pColSrc = 255;
					*pColDst = 255;
				}
			}
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetSeedsFillImage");
	goto ErrorAbort;
}
PASS:
	if(plSeedsAddr1 != NULL)
		delete [] plSeedsAddr1;
	if(plSeedsAddr2 != NULL)
		delete [] plSeedsAddr2;
	return 0;
ErrorAbort:
	if(plSeedsAddr1 != NULL)
		delete [] plSeedsAddr1;
	if(plSeedsAddr2 != NULL)
		delete [] plSeedsAddr2;
	return -1;
}



int GetSeedsFillImage(CImageBuffer *pImgBinSrc,
					 CRect* prcBinSrc,
					 CPoint* pptSeed,
					 int nSeed,
					 int nBlobType,
					 CImageBuffer *pImgBinDst,
					 CRect* prcBinDstRelative,
					 CPoint* pptBinDstTopLftInrcBinSrc)
{
	int x, y;
	int m, k;
	long lm;
	long lIndex;
	long* plSeedsAddr1;
	long lSeedsNo1;
	long* plSeedsAddr2;
	long lSeedsNo2;
	BYTE* pbyteBinSrcPtr;
	int nBinSrcPitch;
	BYTE* pbyteBinDstPtr;
	int nBinDstPitch;
	int nError;
	int nDir[8];
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	int nWidthSrc,nHeightSrc;
	long lSeedIndex;
	int nx;
	int ny;
	int nLft;
	int nTop;
	int nRht;
	int nBot;
	int nSrcTopLftInDstX;
	int nSrcTopLftInDstY;
	int nBlobWidth;
	int nBlobHeight;
	int nLftSrc;
	int nTopSrc;
	int nRhtSrc;
	int nBotSrc;

try
{
	plSeedsAddr1 = NULL;
	plSeedsAddr2 = NULL;
	nLftSrc = prcBinSrc->left;
	nTopSrc = prcBinSrc->top;
	nRhtSrc = prcBinSrc->right - 1;
	nBotSrc = prcBinSrc->bottom - 1;
	if (!pImgBinSrc) {
		ASSERT(!"pImgBinSrc is a bad Pointer");

		return -1;
	}
	if (!pImgBinSrc->IsAllocated()) {
		ASSERT(!"pImgBinSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcBinSrc,&(pImgBinSrc->GetImgSize()))) {
		ASSERT(!"prcBinSrc is Invalid");
		return -1;
	}

	pbyteBinSrcPtr = pImgBinSrc->GetImgPtr();
	nBinSrcPitch = pImgBinSrc->GetPitch();
	pbyteBinDstPtr = pImgBinDst->GetImgPtr();
	nBinDstPitch = pImgBinDst->GetPitch();

	nWidthSrc = prcBinSrc->Width();
	nHeightSrc = prcBinSrc->Height();

	nDir[0] = 1;
	nDir[1] = -(nBinSrcPitch - 1);
	nDir[2] = -nBinSrcPitch;
	nDir[3] = -(nBinSrcPitch + 1);
	nDir[4] = -1;
	nDir[5] = nBinSrcPitch - 1;
	nDir[6] = nBinSrcPitch;
	nDir[7] = nBinSrcPitch + 1;

	BYTE* pValue;
	if(nBlobType == WHITE) {
		nError = ClearImageROIEdge(pImgBinSrc,
								   prcBinSrc,
								   0);
		if(nError)
			goto ErrorAbort;
		plSeedsAddr1 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
		plSeedsAddr2 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
		lSeedsNo1 = 0;
		for(m = 0; m < nSeed; m++) {
			if(ValidatePoint(&pptSeed[m], prcBinSrc)) 
				continue;
			lSeedIndex = pptSeed[m].y * nBinSrcPitch + pptSeed[m].x;
			pValue = &pbyteBinSrcPtr[lSeedIndex];
			if(*pValue == 255) {
				*pValue = 200;
				plSeedsAddr1[lSeedsNo1++] = lSeedIndex;
			}
		}
		for(m = 0; m < lSeedsNo1; m++) 
			pbyteBinSrcPtr[plSeedsAddr1[m]] = 255;
		if(lSeedsNo1 > 0) {
			nx = plSeedsAddr1[0] % nBinSrcPitch;
			ny = plSeedsAddr1[0] / nBinSrcPitch;
			nLft = nx;
			nTop = ny;
			nRht = nx;
			nBot = ny;
		}
		else {
			*pptBinDstTopLftInrcBinSrc = prcBinSrc->TopLeft();
			*prcBinDstRelative = CRect(0,0,1,1);
			pImgBinDst->Allocate(AbsSize(prcBinDstRelative->Size()));
			pImgBinDst->ClrImg(0);
			goto PASS;
		}
		while(lSeedsNo1 > 0) {
			lSeedsNo2 = 0;
			for(lm = 0; lm < lSeedsNo1; lm++) {
				lIndex = plSeedsAddr1[lm];
				pbyteBinSrcPtr[lIndex] = 200;
				nx = lIndex % nBinSrcPitch;
				ny = lIndex / nBinSrcPitch;
				if(nLft >= nx)
				{
					nLft = nx;
					if(nLftSrc >= nLft)
						continue;
				}
				if(nRht <= nx)
				{
					nRht = nx;
					if(nRhtSrc <= nRht)
						continue;
				}
				if(nTop >= ny)
				{
					nTop = ny;
					if(nTopSrc >= nTop)
						continue;
				}
				if(nBot <= ny)
				{
					nBot = ny;
					if(nBotSrc <= nBot)
						continue;
				}
				for(k = 0; k < 8; k++) {
					pValue = &pbyteBinSrcPtr[lIndex + nDir[k]];
					if(*pValue == 255) {
						plSeedsAddr2[lSeedsNo2++] = lIndex + nDir[k];
						*pValue = 200;
					}
				}
			}
			if(lSeedsNo2 == 0)
				break;
			lSeedsNo1 = 0;
			for(lm = 0; lm < lSeedsNo2; lm++) {
				lIndex = plSeedsAddr2[lm];
				pbyteBinSrcPtr[lIndex] = 200;
				nx = lIndex % nBinSrcPitch;
				ny = lIndex / nBinSrcPitch;
				if(nLft >= nx)
				{
					nLft = nx;
					if(nLftSrc >= nLft)
						continue;
				}
				if(nRht <= nx)
				{
					nRht = nx;
					if(nRhtSrc <= nRht)
						continue;
				}
				if(nTop >= ny)
				{
					nTop = ny;
					if(nTopSrc >= nTop)
						continue;
				}
				if(nBot <= ny)
				{
					nBot = ny;
					if(nBotSrc <= nBot)
						continue;
				}
				for(k = 0; k < 8; k++) {
					pValue = &pbyteBinSrcPtr[lIndex + nDir[k]];
					if(*pValue == 255) {
						plSeedsAddr1[lSeedsNo1++] = lIndex + nDir[k];
						*pValue = 200;
					}
				}
			}
		}
		nBlobWidth = nRht - nLft + 1;
		nBlobHeight = nBot - nTop + 1;
		if((nLft >= 1) && (nTop >= 1)) {
			*prcBinDstRelative = CRect(CPoint(0,0), CSize(nRht - nLft + 3, nBot - nTop + 3));
			*pptBinDstTopLftInrcBinSrc = CPoint(nLft-1, nTop-1);
			nSrcTopLftInDstX = 1;
			nSrcTopLftInDstY = 1;
		}
		else {
			*prcBinDstRelative = CRect(CPoint(0,0), CSize(nRht - nLft + 1, nBot - nTop + 1));
			*pptBinDstTopLftInrcBinSrc = CPoint(nLft, nTop);
			nSrcTopLftInDstX = 0;
			nSrcTopLftInDstY = 0;
		}
		pImgBinDst->Allocate(AbsSize(prcBinDstRelative->Size()));
		pImgBinDst->ClrImg(0);
		nBinDstPitch = pImgBinDst->GetPitch();
		pRowSrc = pColSrc = pImgBinSrc->GetImgPtr() + nBinSrcPitch * nTop + nLft;
		pRowDst = pColDst = pImgBinDst->GetImgPtr() + nBinDstPitch * nSrcTopLftInDstY + nSrcTopLftInDstX;
		for (y=0; y<nBlobHeight; y++,pRowSrc+=nBinSrcPitch,pRowDst+=nBinDstPitch) {
			for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nBlobWidth; x++,pColSrc++,pColDst++) {
				if(*pColSrc == 200) {
					*pColSrc = 255;
					*pColDst = 255;
				}
			}
		}
	}
	else {
		nError = ClearImageROIEdge(pImgBinSrc,
								   prcBinSrc,
								   255);
		if(nError)
			goto ErrorAbort;
		plSeedsAddr1 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];;
		plSeedsAddr2 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
		lSeedsNo1 = 0;
		for(m = 0; m < nSeed; m++) {
			if(ValidatePoint(&pptSeed[m], prcBinSrc)) 
				continue;
			lSeedIndex = pptSeed[m].y * nBinSrcPitch + pptSeed[m].x;
			pValue = &pbyteBinSrcPtr[lSeedIndex];
			if(*pValue == 0) {
				*pValue = 200;
				plSeedsAddr1[lSeedsNo1++] = lSeedIndex;
			}
		}
		for(m = 0; m < lSeedsNo1; m++) 
			pbyteBinSrcPtr[plSeedsAddr1[m]] = 0;
		if(lSeedsNo1 > 0) {
			nx = plSeedsAddr1[0] % nBinSrcPitch;
			ny = plSeedsAddr1[0] / nBinSrcPitch;
			nLft = nx;
			nTop = ny;
			nRht = nx;
			nBot = ny;
		}
		else {
			*pptBinDstTopLftInrcBinSrc = prcBinSrc->TopLeft();
			*prcBinDstRelative = CRect(0,0,1,1);
			pImgBinDst->Allocate(AbsSize(prcBinDstRelative->Size())); 
			pImgBinDst->ClrImg(0);
			goto PASS;
		}
		while(lSeedsNo1 > 0) {
			lSeedsNo2 = 0;
			for(lm = 0; lm < lSeedsNo1; lm++) {
				lIndex = plSeedsAddr1[lm];
				pbyteBinSrcPtr[lIndex] = 200;
				nx = lIndex % nBinSrcPitch;
				ny = lIndex / nBinSrcPitch;
				if(nLft >= nx)
				{
					nLft = nx;
					if(nLftSrc >= nLft)
						continue;
				}
				if(nRht <= nx)
				{
					nRht = nx;
					if(nRhtSrc <= nRht)
						continue;
				}
				if(nTop >= ny)
				{
					nTop = ny;
					if(nTopSrc >= nTop)
						continue;
				}
				if(nBot <= ny)
				{
					nBot = ny;
					if(nBotSrc <= nBot)
						continue;
				}

				for(k = 0; k < 8; k++) {
					pValue = &pbyteBinSrcPtr[lIndex + nDir[k]];
					if(*pValue == 0) {
						plSeedsAddr2[lSeedsNo2++] = lIndex + nDir[k];
						*pValue = 200;
					}
				}
			}
			if(lSeedsNo2 == 0)
				break;
			lSeedsNo1 = 0;
			for(lm = 0; lm < lSeedsNo2; lm++) {
				lIndex = plSeedsAddr2[lm];
				pbyteBinSrcPtr[lIndex] = 200;
				nx = lIndex % nBinSrcPitch;
				ny = lIndex / nBinSrcPitch;
				if(nLft >= nx)
				{
					nLft = nx;
					if(nLftSrc >= nLft)
						continue;
				}
				if(nRht <= nx)
				{
					nRht = nx;
					if(nRhtSrc <= nRht)
						continue;
				}
				if(nTop >= ny)
				{
					nTop = ny;
					if(nTopSrc >= nTop)
						continue;
				}
				if(nBot <= ny)
				{
					nBot = ny;
					if(nBotSrc <= nBot)
						continue;
				}
				for(k = 0; k < 8; k++) {
					pValue = &pbyteBinSrcPtr[lIndex + nDir[k]];
					if(*pValue == 0) {
						plSeedsAddr1[lSeedsNo1++] = lIndex + nDir[k];
						*pValue = 200;
					}
				}
			}
		}
		nBlobWidth = nRht - nLft + 1;
		nBlobHeight = nBot - nTop + 1;
		if((nLft >= 1) && (nTop >= 1)) {
			*prcBinDstRelative = CRect(CPoint(0,0), CSize(nRht - nLft + 3, nBot - nTop + 3));
			*pptBinDstTopLftInrcBinSrc = CPoint(nLft-1, nTop-1);
			nSrcTopLftInDstX = 1;
			nSrcTopLftInDstY = 1;
		}
		else {
			*prcBinDstRelative = CRect(CPoint(0,0), CSize(nRht - nLft + 1, nBot - nTop + 1));
			*pptBinDstTopLftInrcBinSrc = CPoint(nLft, nTop);
			nSrcTopLftInDstX = 0;
			nSrcTopLftInDstY = 0;
		}
		pImgBinDst->Allocate(AbsSize(prcBinDstRelative->Size()));
		pImgBinDst->ClrImg(255);
		nBinDstPitch = pImgBinDst->GetPitch();
		pRowSrc = pColSrc = pImgBinSrc->GetImgPtr() + nBinSrcPitch * nTop + nLft;
		pRowDst = pColDst = pImgBinDst->GetImgPtr() + nBinDstPitch * nSrcTopLftInDstY + nSrcTopLftInDstX;
		for (y=0; y<nBlobHeight; y++,pRowSrc+=nBinSrcPitch,pRowDst+=nBinDstPitch) {
			for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nBlobWidth; x++,pColSrc++,pColDst++) {
				if(*pColSrc == 200) {
					*pColSrc = 0;
					*pColDst = 0;
				}
			}
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetSeedsFillImage -1");
	goto ErrorAbort;
}
PASS:
	if(plSeedsAddr1 != NULL)
		delete [] plSeedsAddr1;
	if(plSeedsAddr2 != NULL)
		delete [] plSeedsAddr2;
	return 0;
ErrorAbort:
	if(plSeedsAddr1 != NULL)
		delete [] plSeedsAddr1;
	if(plSeedsAddr2 != NULL)
		delete [] plSeedsAddr2;
	return -1;
}

int GetSeedsFillImage(CImageBuffer *pImgBinSrc,
					 CRect* prcBinSrc,
					 CPoint* pptSeed,
					 int nSeed,
					 BYTE byteFilledVal)
{
	int m, k;
	long lm;
	long lIndex;
	long* plSeedsAddr1;
	long lSeedsNo1;
	long* plSeedsAddr2;
	long lSeedsNo2;
	BYTE* pbyteBinSrcPtr;
	int nBinSrcPitch;
	int nError;
	int nDir[8];
	int nWidthSrc,nHeightSrc;
	long lSeedIndex;
	int nx;
	int ny;
	int nLft;
	int nTop;
	int nRht;
	int nBot;
	int nLftSrc;
	int nTopSrc;
	int nRhtSrc;
	int nBotSrc;

try
{
	plSeedsAddr1 = NULL;
	plSeedsAddr2 = NULL;
	nLftSrc = prcBinSrc->left;
	nTopSrc = prcBinSrc->top;
	nRhtSrc = prcBinSrc->right - 1;
	nBotSrc = prcBinSrc->bottom - 1;
	if (!pImgBinSrc) {
		ASSERT(!"pImgBinSrc is a bad Pointer");
		return -1;
	}
	if (!pImgBinSrc->IsAllocated()) {
		ASSERT(!"pImgBinSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcBinSrc,&(pImgBinSrc->GetImgSize()))) {
		ASSERT(!"prcBinSrc is Invalid");
		return -1;
	}

	pbyteBinSrcPtr = pImgBinSrc->GetImgPtr();
	nBinSrcPitch = pImgBinSrc->GetPitch();

	nWidthSrc = prcBinSrc->Width();
	nHeightSrc = prcBinSrc->Height();

	nDir[0] = 1;
	nDir[1] = -(nBinSrcPitch - 1);
	nDir[2] = -nBinSrcPitch;
	nDir[3] = -(nBinSrcPitch + 1);
	nDir[4] = -1;
	nDir[5] = nBinSrcPitch - 1;
	nDir[6] = nBinSrcPitch;
	nDir[7] = nBinSrcPitch + 1;

	nError = ClearImageROIEdge(pImgBinSrc,
							   prcBinSrc,
							   0);
	if(nError)
		goto ErrorAbort;
	plSeedsAddr1 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
	plSeedsAddr2 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
	lSeedsNo1 = 0;

	BYTE* pValue;
	for(m = 0; m < nSeed; m++) {
		if(ValidatePoint(&pptSeed[m], prcBinSrc)) 
			continue;
		lSeedIndex = pptSeed[m].y * nBinSrcPitch + pptSeed[m].x;
		pValue = &pbyteBinSrcPtr[lSeedIndex];
		if(*pValue == 255) {
			*pValue = byteFilledVal;
			plSeedsAddr1[lSeedsNo1++] = lSeedIndex;
		}
	}
	for(m = 0; m < lSeedsNo1; m++) 
		pbyteBinSrcPtr[plSeedsAddr1[m]] = 255;
	if(lSeedsNo1 > 0) {
		nx = plSeedsAddr1[0] % nBinSrcPitch;
		ny = plSeedsAddr1[0] / nBinSrcPitch;
		nLft = nx;
		nTop = ny;
		nRht = nx;
		nBot = ny;
	}
	else 
		goto PASS;
	while(lSeedsNo1 > 0) {
		lSeedsNo2 = 0;
		for(lm = 0; lm < lSeedsNo1; lm++) {
			lIndex = plSeedsAddr1[lm];
			pbyteBinSrcPtr[lIndex] = byteFilledVal;
			nx = lIndex % nBinSrcPitch;
			ny = lIndex / nBinSrcPitch;
			if(nLft >= nx)
			{
				nLft = nx;
				if(nLftSrc >= nLft)
					continue;
			}
			if(nRht <= nx)
			{
				nRht = nx;
				if(nRhtSrc <= nRht)
					continue;
			}
			if(nTop >= ny)
			{
				nTop = ny;
				if(nTopSrc >= nTop)
					continue;
			}
			if(nBot <= ny)
			{
				nBot = ny;
				if(nBotSrc <= nBot)
					continue;
			}
			for(k = 0; k < 8; k++) {
				pValue = &pbyteBinSrcPtr[lIndex + nDir[k]];
				if(*pValue == 255) {
					plSeedsAddr2[lSeedsNo2++] = lIndex + nDir[k];
					*pValue = byteFilledVal;
				}
			}
		}
		if(lSeedsNo2 == 0)
			break;
		lSeedsNo1 = 0;
		for(lm = 0; lm < lSeedsNo2; lm++) {
			lIndex = plSeedsAddr2[lm];
			pbyteBinSrcPtr[lIndex] = byteFilledVal;
			nx = lIndex % nBinSrcPitch;
			ny = lIndex / nBinSrcPitch;
			if(nLft >= nx)
			{
				nLft = nx;
				if(nLftSrc >= nLft)
					continue;
			}
			if(nRht <= nx)
			{
				nRht = nx;
				if(nRhtSrc <= nRht)
					continue;
			}
			if(nTop >= ny)
			{
				nTop = ny;
				if(nTopSrc >= nTop)
					continue;
			}
			if(nBot <= ny)
			{
				nBot = ny;
				if(nBotSrc <= nBot)
					continue;
			}
			for(k = 0; k < 8; k++) {
				pValue = &pbyteBinSrcPtr[lIndex + nDir[k]];
				if(*pValue == 255) {
					plSeedsAddr1[lSeedsNo1++] = lIndex + nDir[k];
					*pValue = byteFilledVal;
				}
			}
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetSeedsFillImage -3");
	goto ErrorAbort;
}
PASS:
	if(plSeedsAddr1 != NULL)
		delete [] plSeedsAddr1;
	if(plSeedsAddr2 != NULL)
		delete [] plSeedsAddr2;
	return 0;
ErrorAbort:
	if(plSeedsAddr1 != NULL)
		delete [] plSeedsAddr1;
	if(plSeedsAddr2 != NULL)
		delete [] plSeedsAddr2;
	return -1;
}


int GetSeedsFillImageHole(CImageBuffer *pImgBinSrc,
					 CRect* prcBinSrc,
					 CPoint* pptSeed,
					 int nSeed,
					 int nBlobType,
					 CImageBuffer *pImgBinDst,
					 CRect* prcBinDstRelative,
					 CPoint* pptBinDstTopLftInrcBinSrc)
{
	int x, y;
	int m, k;
	long lm;
	long lIndex;
	long* plSeedsAddr1;
	long lSeedsNo1;
	long* plSeedsAddr2;
	long lSeedsNo2;
	BYTE* pbyteBinSrcPtr;
	int nBinSrcPitch;
	BYTE* pbyteBinDstPtr;
	int nBinDstPitch;
	int nError;
	int nDir[4];
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	int nWidthSrc,nHeightSrc;
	long lSeedIndex;
	int nx;
	int ny;
	int nLft;
	int nTop;
	int nRht;
	int nBot;
	int nSrcTopLftInDstX;
	int nSrcTopLftInDstY;
	int nBlobWidth;
	int nBlobHeight;
	int nLftSrc;
	int nTopSrc;
	int nRhtSrc;
	int nBotSrc;

try
{
	plSeedsAddr1 = NULL;
	plSeedsAddr2 = NULL;
	nLftSrc = prcBinSrc->left;
	nTopSrc = prcBinSrc->top;
	nRhtSrc = prcBinSrc->right - 1;
	nBotSrc = prcBinSrc->bottom - 1;
	if (!pImgBinSrc) {
		ASSERT(!"pImgBinSrc is a bad Pointer");
		return -1;
	}
	if (!pImgBinSrc->IsAllocated()) {
		ASSERT(!"pImgBinSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcBinSrc,&(pImgBinSrc->GetImgSize()))) {
		ASSERT(!"prcBinSrc is Invalid");
		return -1;
	}

	pbyteBinSrcPtr = pImgBinSrc->GetImgPtr();
	nBinSrcPitch = pImgBinSrc->GetPitch();
	pbyteBinDstPtr = pImgBinDst->GetImgPtr();
	nBinDstPitch = pImgBinDst->GetPitch();

	nWidthSrc = prcBinSrc->Width();
	nHeightSrc = prcBinSrc->Height();

	nDir[0] = 1;
	nDir[1] = -nBinSrcPitch;
	nDir[2] = -1;
	nDir[3] = nBinSrcPitch;

	BYTE* pValue;
	if(nBlobType == WHITE) {
		nError = ClearImageROIEdge(pImgBinSrc,
								   prcBinSrc,
								   0);
		if(nError)
			goto ErrorAbort;
		plSeedsAddr1 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
		plSeedsAddr2 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
		lSeedsNo1 = 0;
		for(m = 0; m < nSeed; m++) {
			if(ValidatePoint(&pptSeed[m], prcBinSrc)) 
				continue;
			lSeedIndex = pptSeed[m].y * nBinSrcPitch + pptSeed[m].x;
			pValue = &pbyteBinSrcPtr[lSeedIndex];
			if(*pValue == 255) {
				*pValue = 200;
				plSeedsAddr1[lSeedsNo1++] = lSeedIndex;
			}
		}
		for(m = 0; m < lSeedsNo1; m++) 
			pbyteBinSrcPtr[plSeedsAddr1[m]] = 255;
		if(lSeedsNo1 > 0) {
			nx = plSeedsAddr1[0] % nBinSrcPitch;
			ny = plSeedsAddr1[0] / nBinSrcPitch;
			nLft = nx;
			nTop = ny;
			nRht = nx;
			nBot = ny;
		}
		else {
			*pptBinDstTopLftInrcBinSrc = prcBinSrc->TopLeft();
			*prcBinDstRelative = CRect(0,0,1,1);
			pImgBinDst->Allocate(AbsSize(prcBinDstRelative->Size()));
			pImgBinDst->ClrImg(0);
			goto PASS;
		}
		while(lSeedsNo1 > 0) {
			lSeedsNo2 = 0;
			for(lm = 0; lm < lSeedsNo1; lm++) {
				lIndex = plSeedsAddr1[lm];
				pbyteBinSrcPtr[lIndex] = 200;
				nx = lIndex % nBinSrcPitch;
				ny = lIndex / nBinSrcPitch;
				if(nLft >= nx)
				{
					nLft = nx;
					if(nLftSrc >= nLft)
						continue;
				}
				if(nRht <= nx)
				{
					nRht = nx;
					if(nRhtSrc <= nRht)
						continue;
				}
				if(nTop >= ny)
				{
					nTop = ny;
					if(nTopSrc >= nTop)
						continue;
				}
				if(nBot <= ny)
				{
					nBot = ny;
					if(nBotSrc <= nBot)
						continue;
				}
				for(k = 0; k < 4; k++) {
					pValue = &pbyteBinSrcPtr[lIndex + nDir[k]];
					if(*pValue == 255) {
						plSeedsAddr2[lSeedsNo2++] = lIndex + nDir[k];
						*pValue = 200;
					}
				}
			}
			if(lSeedsNo2 == 0)
				break;
			lSeedsNo1 = 0;
			for(lm = 0; lm < lSeedsNo2; lm++) {
				lIndex = plSeedsAddr2[lm];
				pbyteBinSrcPtr[lIndex] = 200;
				nx = lIndex % nBinSrcPitch;
				ny = lIndex / nBinSrcPitch;
				if(nLft >= nx)
				{
					nLft = nx;
					if(nLftSrc >= nLft)
						continue;
				}
				if(nRht <= nx)
				{
					nRht = nx;
					if(nRhtSrc <= nRht)
						continue;
				}
				if(nTop >= ny)
				{
					nTop = ny;
					if(nTopSrc >= nTop)
						continue;
				}
				if(nBot <= ny)
				{
					nBot = ny;
					if(nBotSrc <= nBot)
						continue;
				}
				for(k = 0; k < 4; k++) {
					pValue = &pbyteBinSrcPtr[lIndex + nDir[k]];
					if(*pValue == 255) {
						plSeedsAddr1[lSeedsNo1++] = lIndex + nDir[k];
						*pValue = 200;
					}
				}
			}
		}
		nBlobWidth = nRht - nLft + 1;
		nBlobHeight = nBot - nTop + 1;
		if((nLft >= 1) && (nTop >= 1)) {
			*prcBinDstRelative = CRect(CPoint(0,0), CSize(nRht - nLft + 3, nBot - nTop + 3));
			*pptBinDstTopLftInrcBinSrc = CPoint(nLft-1, nTop-1);
			nSrcTopLftInDstX = 1;
			nSrcTopLftInDstY = 1;
		}
		else {
			*prcBinDstRelative = CRect(CPoint(0,0), CSize(nRht - nLft + 1, nBot - nTop + 1));
			*pptBinDstTopLftInrcBinSrc = CPoint(nLft, nTop);
			nSrcTopLftInDstX = 0;
			nSrcTopLftInDstY = 0;
		}
		pImgBinDst->Allocate(AbsSize(prcBinDstRelative->Size()));
		pImgBinDst->ClrImg(0);
		nBinDstPitch = pImgBinDst->GetPitch();
		pRowSrc = pColSrc = pImgBinSrc->GetImgPtr() + nBinSrcPitch * nTop + nLft;
		pRowDst = pColDst = pImgBinDst->GetImgPtr() + nBinDstPitch * nSrcTopLftInDstY + nSrcTopLftInDstX;
		for (y=0; y<nBlobHeight; y++,pRowSrc+=nBinSrcPitch,pRowDst+=nBinDstPitch) {
			for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nBlobWidth; x++,pColSrc++,pColDst++) {
				if(*pColSrc == 200) {
					*pColSrc = 255;
					*pColDst = 255;
				}
			}
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetSeedsFillImageHole");
	goto ErrorAbort;
}
PASS:
	if(plSeedsAddr1 != NULL)
		delete [] plSeedsAddr1;
	if(plSeedsAddr2 != NULL)
		delete [] plSeedsAddr2;
	return 0;
ErrorAbort:
	if(plSeedsAddr1 != NULL)
		delete [] plSeedsAddr1;
	if(plSeedsAddr2 != NULL)
		delete [] plSeedsAddr2;
	return -1;
}


int GetSeedsFillImage(CImageBuffer *pImgBinSrc,
					 CRect* prcBinSrc,
					 long* plSeedAddress,
					 int nSeedAddress,
					 int nBlobType,
					 CImageBuffer *pImgBinDst,
					 CRect* prcBinDstRelative,
					 CPoint* pptBinDstTopLftInrcBinSrc)
{
	int x, y;
	int m, k;
	long lm;
	long lIndex;
	long* plSeedsAddr1;
	long lSeedsNo1;
	long* plSeedsAddr2;
	long lSeedsNo2;
	BYTE* pbyteBinSrcPtr;
	int nBinSrcPitch;
	BYTE* pbyteBinDstPtr;
	int nBinDstPitch;
	int nError;
	int nDir[8];
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	int nWidthSrc,nHeightSrc;
	int nx;
	int ny;
	int nLft;
	int nTop;
	int nRht;
	int nBot;
	long lMaxAddress;
	int nSrcTopLftInDstX;
	int nSrcTopLftInDstY;
	int nBlobWidth;
	int nBlobHeight;
	int nLftSrc;
	int nTopSrc;
	int nRhtSrc;
	int nBotSrc;

try
{
	*prcBinDstRelative = CRect(0,0,0,0);
	plSeedsAddr1 = NULL;
	plSeedsAddr2 = NULL;
	nLftSrc = prcBinSrc->left;
	nTopSrc = prcBinSrc->top;
	nRhtSrc = prcBinSrc->right - 1;
	nBotSrc = prcBinSrc->bottom - 1;
	if (!pImgBinSrc) {
		ASSERT(!"pImgBinSrc is a bad Pointer");
		return -1;
	}
	if (!pImgBinSrc->IsAllocated()) {
		ASSERT(!"pImgBinSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcBinSrc,&(pImgBinSrc->GetImgSize()))) {
		ASSERT(!"prcBinSrc is Invalid");
		return -1;
	}

	pbyteBinSrcPtr = pImgBinSrc->GetImgPtr();
	nBinSrcPitch = pImgBinSrc->GetPitch();
	pbyteBinDstPtr = pImgBinDst->GetImgPtr();
	nBinDstPitch = pImgBinDst->GetPitch();

	nWidthSrc = prcBinSrc->Width();
	nHeightSrc = prcBinSrc->Height();

	nDir[0] = 1;
	nDir[1] = -(nBinSrcPitch - 1);
	nDir[2] = -nBinSrcPitch;
	nDir[3] = -(nBinSrcPitch + 1);
	nDir[4] = -1;
	nDir[5] = nBinSrcPitch - 1;
	nDir[6] = nBinSrcPitch;
	nDir[7] = nBinSrcPitch + 1;

	BYTE* pValue;
	if(nBlobType == WHITE) {
		nError = ClearImageROIEdge(pImgBinSrc,
								   prcBinSrc,
								   0);
		if(nError)
			goto ErrorAbort;
		plSeedsAddr1 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
		plSeedsAddr2 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
		lMaxAddress = prcBinSrc->Height() * nBinSrcPitch;
		lSeedsNo1 = 0;
		for(m = 0; m < nSeedAddress; m++) {
			if((plSeedAddress[m] >= 0) && (plSeedAddress[m] < lMaxAddress)) {
				pValue = &pbyteBinSrcPtr[plSeedAddress[m]];
				if(*pValue == 255) {
					*pValue = 200;
					plSeedsAddr1[lSeedsNo1++] = plSeedAddress[m];
				}
			}
		}
		for(m = 0; m < lSeedsNo1; m++) 
			pbyteBinSrcPtr[plSeedsAddr1[m]] = 255;

		if(lSeedsNo1 > 0) {
			nx = plSeedsAddr1[0] % nBinSrcPitch;
			ny = plSeedsAddr1[0] / nBinSrcPitch;
			nLft = nx;
			nTop = ny;
			nRht = nx;
			nBot = ny;
		}
		else {
			*pptBinDstTopLftInrcBinSrc = prcBinSrc->TopLeft();
			*prcBinDstRelative = CRect(0,0,1,1);
			pImgBinDst->Allocate(AbsSize(prcBinDstRelative->Size()));
			pImgBinDst->ClrImg(0);
			goto PASS;
		}
		while(lSeedsNo1 > 0) {
			lSeedsNo2 = 0;
			for(lm = 0; lm < lSeedsNo1; lm++) {
				lIndex = plSeedsAddr1[lm];
				pbyteBinSrcPtr[lIndex] = 200;
				nx = lIndex % nBinSrcPitch;
				ny = lIndex / nBinSrcPitch;
				if(nLft >= nx)
				{
					nLft = nx;
					if(nLftSrc >= nLft)
						continue;
				}
				if(nRht <= nx)
				{
					nRht = nx;
					if(nRhtSrc <= nRht)
						continue;
				}
				if(nTop >= ny)
				{
					nTop = ny;
					if(nTopSrc >= nTop)
						continue;
				}
				if(nBot <= ny)
				{
					nBot = ny;
					if(nBotSrc <= nBot)
						continue;
				}
				for(k = 0; k < 8; k++) {
					pValue = &pbyteBinSrcPtr[lIndex + nDir[k]];
					if(*pValue == 255) {
						plSeedsAddr2[lSeedsNo2++] = lIndex + nDir[k];
						*pValue = 200;
					}
				}
			}
			if(lSeedsNo2 == 0)
				break;
			lSeedsNo1 = 0;
			for(lm = 0; lm < lSeedsNo2; lm++) {
				lIndex = plSeedsAddr2[lm];
				pbyteBinSrcPtr[lIndex] = 200;
				nx = lIndex % nBinSrcPitch;
				ny = lIndex / nBinSrcPitch;
				if(nLft >= nx)
				{
					nLft = nx;
					if(nLftSrc >= nLft)
						continue;
				}
				if(nRht <= nx)
				{
					nRht = nx;
					if(nRhtSrc <= nRht)
						continue;
				}
				if(nTop >= ny)
				{
					nTop = ny;
					if(nTopSrc >= nTop)
						continue;
				}
				if(nBot <= ny)
				{
					nBot = ny;
					if(nBotSrc <= nBot)
						continue;
				}
				for(k = 0; k < 8; k++) {
					pValue = &pbyteBinSrcPtr[lIndex + nDir[k]];
					if(*pValue == 255) {
						plSeedsAddr1[lSeedsNo1++] = lIndex + nDir[k];
						*pValue = 200;
					}
				}
			}
		}
		nBlobWidth = nRht - nLft + 1;
		nBlobHeight = nBot - nTop + 1;
		if((nLft >= 1) && (nTop >= 1)) {
			*prcBinDstRelative = CRect(CPoint(0,0), CSize(nRht - nLft + 3, nBot - nTop + 3));
			*pptBinDstTopLftInrcBinSrc = CPoint(nLft-1, nTop-1);
			nSrcTopLftInDstX = 1;
			nSrcTopLftInDstY = 1;
		}
		else {
			*prcBinDstRelative = CRect(CPoint(0,0), CSize(nRht - nLft + 1, nBot - nTop + 1));
			*pptBinDstTopLftInrcBinSrc = CPoint(nLft, nTop);
			nSrcTopLftInDstX = 0;
			nSrcTopLftInDstY = 0;
		}
		pImgBinDst->Allocate(AbsSize(prcBinDstRelative->Size()));
		pImgBinDst->ClrImg(0);
		nBinDstPitch = pImgBinDst->GetPitch();
		pRowSrc = pColSrc = pImgBinSrc->GetImgPtr() + nBinSrcPitch * nTop + nLft;
		pRowDst = pColDst = pImgBinDst->GetImgPtr() + nBinDstPitch * nSrcTopLftInDstY + nSrcTopLftInDstX;
		for (y=0; y<nBlobHeight; y++,pRowSrc+=nBinSrcPitch,pRowDst+=nBinDstPitch) {
			for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nBlobWidth; x++,pColSrc++,pColDst++) {
				if(*pColSrc == 200) {
					*pColSrc = 255;
					*pColDst = 255;
				}
			}
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetSeedsFillImage -4");
	goto ErrorAbort;
}
PASS:
	if(plSeedsAddr1 != NULL)
		delete [] plSeedsAddr1;
	if(plSeedsAddr2 != NULL)
		delete [] plSeedsAddr2;
	return 0;
ErrorAbort:
	if(plSeedsAddr1 != NULL)
		delete [] plSeedsAddr1;
	if(plSeedsAddr2 != NULL)
		delete [] plSeedsAddr2;
	return -1;
}

int GetMaxOverlapSeedBlob(IMAGEVIEW_INFO *pView,
						 CImageBuffer *pImgBinSrc,
						 CRect* prcBinSrc,
						 CImageBuffer *pImgBinSeeds,
						 CRect* prcBinSeeds,
						 long lDebugFlag)
{
	int i, j;
	CPoint ptOffsetOfMask1;
	ADV_BLOB_FIND_PARM AdvBlobParm;
	long lProcMode;
	CAdvBlobFind BlobFinder;
	long lROIArea;
	int nPkgDiagonalDist;
	CPoint2D* ppt2DSeedsContour;
	int nppt2DSeedsContour;
	CPoint2D* ppt2DSeedsHole;
	int* pnSortingIndex;
	int nppt2DSeedsHole;
	CMask Mask;
	long lSortingMode;
	BOOL bAscend;
	int nError;

try
{
	ppt2DSeedsContour = NULL;
	ppt2DSeedsHole = NULL;
	pnSortingIndex = NULL;
	ptOffsetOfMask1 = CPoint(prcBinSeeds->left-prcBinSrc->left, prcBinSeeds->top-prcBinSrc->top);
	nError = MaskOverlap(pImgBinSeeds,
				prcBinSeeds,
				pImgBinSrc,
				prcBinSrc,
				&ptOffsetOfMask1);
	if(nError)
		goto ErrorAbort;
	lROIArea = prcBinSeeds->Width() * prcBinSeeds->Height();
	nPkgDiagonalDist = prcBinSeeds->Width() + prcBinSeeds->Height();
	AdvBlobParm.nBlobType = WHITE;
	AdvBlobParm.nMinBlobArea = 1;
	AdvBlobParm.nMaxBlobArea = lROIArea;
	AdvBlobParm.nMaxYBlobSize = nPkgDiagonalDist;
	AdvBlobParm.nMinYBlobSize = 1;
	AdvBlobParm.nMaxXBlobSize = nPkgDiagonalDist;
	AdvBlobParm.nMinXBlobSize = 1;
	AdvBlobParm.rcROISrc = *prcBinSeeds;
	AdvBlobParm.rcROIDst = *prcBinSeeds;
	lProcMode = FIND_BLOB_CONTOUR | FIND_BLOB_HOLE;
	nError = BlobFinder.GetAdvBlobHole(pView,pImgBinSeeds,&AdvBlobParm,lProcMode,lDebugFlag);
	if(nError) {
		*prcBinSeeds = CRect(0,0,0,0);
		goto ErrorAbort;
	}
	if(BlobFinder.m_AdvBlobFindRslt.nFndBlobs < 1) {
		*prcBinSeeds = CRect(0,0,0,0);
		goto PASS;
	}
	if(BlobFinder.m_AdvBlobFindRslt.nFndBlobs == 1) {
		*prcBinSeeds = BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[0].rcFndBlobPos;
		goto PASS;
	}
	pnSortingIndex = new int[abs(BlobFinder.m_AdvBlobFindRslt.nFndBlobs)];
	lSortingMode = SORTING_BLOB_BY_AREA;
	bAscend = FALSE;
	nError = BlobFinder.BlobSorting(lSortingMode, bAscend, &pnSortingIndex[0]);
	if(nError)
		goto ErrorAbort;
	pImgBinSeeds->ClrImg(0);
	if(ppt2DSeedsContour != NULL)
		delete [] ppt2DSeedsContour;
	ppt2DSeedsContour = NULL;
	nppt2DSeedsContour = BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[pnSortingIndex[0]].nBlobContourSize;
	ppt2DSeedsContour = new CPoint2D[abs(nppt2DSeedsContour)];
	for(i=0;i<nppt2DSeedsContour;i++) 
		ppt2DSeedsContour[i] = CPoint2D(BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[pnSortingIndex[0]].pptBlobContour[i]);
	nError = Mask.ApplyGrayMaskFreeHand(pImgBinSeeds,prcBinSeeds,&ppt2DSeedsContour[0], nppt2DSeedsContour, 255);
	if(nError == -1)
		goto ErrorAbort;
	for(i=0;i<BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[pnSortingIndex[0]].nBlobHole;i++) {
		nppt2DSeedsHole = BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[pnSortingIndex[0]].pnptBlobHoleContourSize[i];
		if(ppt2DSeedsHole != NULL)
			delete [] ppt2DSeedsHole;
		ppt2DSeedsHole = NULL;
		ppt2DSeedsHole = new CPoint2D[abs(nppt2DSeedsHole)];
		for(j=0;j<BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[pnSortingIndex[0]].pnptBlobHoleContourSize[i];j++) 
			ppt2DSeedsHole[j] = CPoint2D(BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[pnSortingIndex[0]].ppptBlobHoleContour[i][j]);
		nError = Mask.ApplyGrayMaskFreeHand(pImgBinSeeds,prcBinSeeds,&ppt2DSeedsHole[0], nppt2DSeedsHole, 0);
		if(nError == -1)
			goto ErrorAbort;
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetMaxOverlapSeedBlob");
	goto ErrorAbort;
}
PASS:
	if(ppt2DSeedsContour != NULL)
		delete [] ppt2DSeedsContour;
	if(ppt2DSeedsHole != NULL)
		delete [] ppt2DSeedsHole;
	if(pnSortingIndex != NULL)
		delete [] pnSortingIndex;
	return 0;
ErrorAbort:
	if(ppt2DSeedsContour != NULL)
		delete [] ppt2DSeedsContour;
	if(ppt2DSeedsHole != NULL)
		delete [] ppt2DSeedsHole;
	if(pnSortingIndex != NULL)
		delete [] pnSortingIndex;
	return -1;
}


int GetMaxSingleBlobSeedFillImage(IMAGEVIEW_INFO *pView,
								 CImageBuffer *pImgBinSrc,
								 CRect* prcBinSrc,
								 CImageBuffer *pImgBinSeeds,
								 CRect* prcBinSeeds,
								 CPoint ptBinSeedsTopLftInrcBinSrc,
								 int nBlobType,
								 CImageBuffer *pImgBinDst,
								 CRect* prcBinDstRelative,
								 CPoint* pptBinDstTopLftInrcBinSrc,
								 long lDebugFlag)
{
	int x, y;
	int m, n, k;
	long lm;
	long lIndex;
	long* plSeedsAddr1;
	long lSeedsNo1;
	long* plSeedsAddr2;
	long lSeedsNo2;
	BYTE* pbyteSeedsPtr;
	int nSeedsPitch;
	long lSeedsRowIndex;
	BYTE* pbyteBinSrcPtr;
	int nBinSrcPitch;
	int nError;
	int nDir[8];
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	int nWidthSrc,nHeightSrc;
	int nBinDstPitch;
	BYTE* pbyteBinDstPtr;

	int nx;
	int ny;
	int nLft;
	int nTop;
	int nRht;
	int nBot;
	CRect rcOverlapForSeeds;
	CRect rcSeedsInrcBinSrc;
	CRect rcSeedsAdj;

	CImageBuffer ImgBinMaxSingleBlobSeed;
	CRect rcRelative;
	int nLftSrc;
	int nTopSrc;
	int nRhtSrc;
	int nBotSrc;

try
{
	plSeedsAddr1 = NULL;
	plSeedsAddr2 = NULL;
	nLftSrc = prcBinSrc->left;
	nTopSrc = prcBinSrc->top;
	nRhtSrc = prcBinSrc->right - 1;
	nBotSrc = prcBinSrc->bottom - 1;
	rcSeedsInrcBinSrc = CRect(ptBinSeedsTopLftInrcBinSrc, prcBinSeeds->Size());
	nError = CheckRect(prcBinSrc, &rcSeedsInrcBinSrc);
	if(nError)
		goto ErrorAbort;
	rcSeedsAdj = rcSeedsInrcBinSrc - ptBinSeedsTopLftInrcBinSrc;

	if (!pImgBinSrc) {
		ASSERT(!"pImgBinSrc is a bad Pointer");
		return -1;
	}
	if (!pImgBinSrc->IsAllocated()) {
		ASSERT(!"pImgBinSrc is not Allocated");
		return -1;
	}
	if (ValidateRect(prcBinSrc,&(pImgBinSrc->GetImgSize()))) {
		ASSERT(!"prcBinSrc is Invalid");
		return -1;
	}
	if (!pImgBinSeeds) {
		ASSERT(!"pImgBinSeeds is a bad Pointer");
		return -1;
	}
	if (!pImgBinSeeds->IsAllocated()) {
		ASSERT(!"pImgBinSeeds is not Allocated");
		return -1;
	}
	if (ValidateRect(prcBinSeeds,&(pImgBinSeeds->GetImgSize()))) {
		ASSERT(!"prcBinSeeds is Invalid");
		return -1;
	}
	nError = ClearImageROIEdge(pImgBinSrc,
							   prcBinSrc,
							   0);
	if(nError)
		goto ErrorAbort;

	nError = GetMaxOverlapSeedBlob(pView,
							   pImgBinSrc,
							   &rcSeedsInrcBinSrc,
							   pImgBinSeeds,
							   &rcSeedsAdj,
							   lDebugFlag);
	if(nError)
		goto ErrorAbort;
	pbyteBinSrcPtr = pImgBinSrc->GetImgPtr();
	nBinSrcPitch = pImgBinSrc->GetPitch();
	pbyteSeedsPtr = pImgBinSeeds->GetImgPtr();
	nSeedsPitch = pImgBinSeeds->GetPitch();

	pbyteBinDstPtr = pImgBinDst->GetImgPtr();

	nWidthSrc = prcBinSrc->Width();
	nHeightSrc = prcBinSrc->Height();

	nDir[0] = 1;
	nDir[1] = -(nBinSrcPitch - 1);
	nDir[2] = -nBinSrcPitch;
	nDir[3] = -(nBinSrcPitch + 1);
	nDir[4] = -1;
	nDir[5] = nBinSrcPitch - 1;
	nDir[6] = nBinSrcPitch;
	nDir[7] = nBinSrcPitch + 1;

	lSeedsRowIndex = rcSeedsAdj.top * nSeedsPitch;
	plSeedsAddr1 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
	plSeedsAddr2 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
	lSeedsNo1 = 0;
	for(m = rcSeedsAdj.top; m < rcSeedsAdj.bottom; m++) {
		for(n = rcSeedsAdj.left;n < rcSeedsAdj.right; n++) {
			if(pbyteSeedsPtr[lSeedsRowIndex + n] == 255) 
				plSeedsAddr1[lSeedsNo1++] = (ptBinSeedsTopLftInrcBinSrc.y + m) * nBinSrcPitch + ptBinSeedsTopLftInrcBinSrc.x + n;
		}
		lSeedsRowIndex += nSeedsPitch;
	}
	if(lSeedsNo1 > 0) {
		nx = plSeedsAddr1[0] % nBinSrcPitch;
		ny = plSeedsAddr1[0] / nBinSrcPitch;
		nLft = nx;
		nTop = ny;
		nRht = nx;
		nBot = ny;
	}
	else {
		nLft = 0;
		nTop = 0;
		nRht = 0;
		nBot = 0;
	}
	while(lSeedsNo1 > 0) {
		lSeedsNo2 = 0;
		for(lm = 0; lm < lSeedsNo1; lm++) {
			lIndex = plSeedsAddr1[lm];
			pbyteBinSrcPtr[lIndex] = 200;
			nx = lIndex % nBinSrcPitch;
			ny = lIndex / nBinSrcPitch;
			if(nLft >= nx)
			{
				nLft = nx;
				if(nLftSrc >= nLft)
					continue;
			}
			if(nRht <= nx)
			{
				nRht = nx;
				if(nRhtSrc <= nRht)
					continue;
			}
			if(nTop >= ny)
			{
				nTop = ny;
				if(nTopSrc >= nTop)
					continue;
			}
			if(nBot <= ny)
			{
				nBot = ny;
				if(nBotSrc <= nBot)
					continue;
			}
			for(k = 0; k < 8; k++) {
				if(pbyteBinSrcPtr[lIndex + nDir[k]] == 255) {
					plSeedsAddr2[lSeedsNo2++] = lIndex + nDir[k];
					pbyteBinSrcPtr[lIndex + nDir[k]] = 200;
					nx = (lIndex + nDir[k]) % nBinSrcPitch;
					ny = (lIndex + nDir[k]) / nBinSrcPitch;
					if(nLft >= nx)
						nLft = nx;
					if(nRht <= nx)
						nRht = nx;
					if(nTop >= ny)
						nTop = ny;
					if(nBot <= ny)
						nBot = ny;
				}
			}
		}
		if(lSeedsNo2 == 0)
			break;
		lSeedsNo1 = 0;
		for(lm = 0; lm < lSeedsNo2; lm++) {
			lIndex = plSeedsAddr2[lm];
			pbyteBinSrcPtr[lIndex] = 200;
			nx = lIndex % nBinSrcPitch;
			ny = lIndex / nBinSrcPitch;
			if(nLft >= nx)
			{
				nLft = nx;
				if(nLftSrc >= nLft)
					continue;
			}
			if(nRht <= nx)
			{
				nRht = nx;
				if(nRhtSrc <= nRht)
					continue;
			}
			if(nTop >= ny)
			{
				nTop = ny;
				if(nTopSrc >= nTop)
					continue;
			}
			if(nBot <= ny)
			{
				nBot = ny;
				if(nBotSrc <= nBot)
					continue;
			}
			for(k = 0; k < 8; k++) {
				if(pbyteBinSrcPtr[lIndex + nDir[k]] == 255) {
					plSeedsAddr1[lSeedsNo1++] = lIndex + nDir[k];
					pbyteBinSrcPtr[lIndex + nDir[k]] = 200;
					nx = (lIndex + nDir[k]) % nBinSrcPitch;
					ny = (lIndex + nDir[k]) / nBinSrcPitch;
					if(nLft >= nx)
						nLft = nx;
					if(nRht <= nx)
						nRht = nx;
					if(nTop >= ny)
						nTop = ny;
					if(nBot <= ny)
						nBot = ny;
				}
			}
		}
	}

	if((nLft == 0) && (nTop == 0) && (nTop == 0) && (nTop == 0)) {
		*prcBinDstRelative = CRect(0,0,0,0);
	}
	else
		*prcBinDstRelative = CRect(CPoint(0,0), CSize(nRht - nLft + 1, nBot - nTop + 1));
	*pptBinDstTopLftInrcBinSrc = CPoint(nLft, nTop);
	pImgBinDst->Allocate(AbsSize(prcBinDstRelative->Size()));
	pImgBinDst->ClrImg(0);
	nBinDstPitch = pImgBinDst->GetPitch();
	pRowSrc = pColSrc = pImgBinSrc->GetImgPtr() + nBinSrcPitch * nTop + nLft;
	pRowDst = pColDst = pImgBinDst->GetImgPtr() + nBinDstPitch * prcBinDstRelative->top + prcBinDstRelative->left;
	for (y=0; y<prcBinDstRelative->Height(); y++,pRowSrc+=nBinSrcPitch,pRowDst+=nBinDstPitch) {
		for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<prcBinDstRelative->Width(); x++,pColSrc++,pColDst++) {
			if(*pColSrc == 200) {
				*pColSrc = 255;
				*pColDst = 255;
			}
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetMaxSingleBlobSeedFillImage");
	goto ErrorAbort;
}
	if(plSeedsAddr1 != NULL)
		delete [] plSeedsAddr1;
	if(plSeedsAddr2 != NULL)
		delete [] plSeedsAddr2;
	return 0;
ErrorAbort:
	if(plSeedsAddr1 != NULL)
		delete [] plSeedsAddr1;
	if(plSeedsAddr2 != NULL)
		delete [] plSeedsAddr2;
	return -1;
}

int GetMorphologyImg(CImageBuffer* pImgSrc,
				   CRect* prcSrc,
				   CSize* pcsKernel,
				   int nMorphologyType,
				   CImageBuffer* pImgMorphology,
				   CRect* prcMorphology)
{
	CMorphology Morphology;
	CRect rcSrcRelative;
	CImageBuffer Kernel;
	CRect rcKernel;
	int nImgType;
	int nProcType;
	int nError;

try
{
	nError = ValidateImageAndRect(pImgSrc, prcSrc);
	if(nError)
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgMorphology, prcMorphology);
	if(nError)
		goto ErrorAbort;
	if(prcSrc->Size() != prcMorphology->Size())
		goto ErrorAbort;

	rcSrcRelative = CRect(CPoint(0,0), prcSrc->Size());
	rcKernel = CRect(0,0,pcsKernel->cx,pcsKernel->cy);
	Kernel.Allocate(AbsSize(rcKernel.Size()));
	Kernel.ClrImg(1);
	nImgType = GRAY_SCALE_IMAGE;
	if(nMorphologyType == DILATION)
		nProcType = DILATION;
	else
		nProcType = EROSION;
	nError = Morphology.Process(pImgSrc,
							prcSrc,
							nImgType,
							nProcType,
							&rcKernel.Size(),
							pImgMorphology,
							prcMorphology);
	if(nError)
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetMorphologyImg");
	goto ErrorAbort;
}
	return 0;

ErrorAbort:
	return -1;
}

int GetContrastImg(CImageBuffer* pImgSrc,
				   CRect* prcSrc,
				   CSize* pcsKernel,
				   int nMorphologyType,
				   CImageBuffer* pImgContrast,
				   CRect* prcContrast)
{
	int i;
	CMorphology Morphology;
	CImageBuffer ImgProc;
	CRect rcSrcRelative;
	CImageBuffer Kernel;
	CRect rcKernel;
	int nImgType;
	int nProcType;
	CRect rcClear[4];
	int nError;

try
{
	nError = ValidateImageAndRect(pImgSrc, prcSrc);
	if(nError)
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgContrast, prcContrast);
	if(nError)
		goto ErrorAbort;
	if(prcSrc->Size() != prcContrast->Size())
		goto ErrorAbort;

	rcSrcRelative = CRect(CPoint(0,0), prcSrc->Size());
	ImgProc.Allocate(AbsSize(rcSrcRelative.Size()));
	rcKernel = CRect(0,0,pcsKernel->cx,pcsKernel->cy);
	Kernel.Allocate(AbsSize(rcKernel.Size()));
	Kernel.ClrImg(1);
	nImgType = GRAY_SCALE_IMAGE;
	if(nMorphologyType == DILATION)
		nProcType = DILATION;
	else
		nProcType = EROSION;
	nError = Morphology.Process(pImgSrc,
							prcSrc,
							nImgType,
							nProcType,
							&rcKernel.Size(),
							&ImgProc,
							&rcSrcRelative);
	if(nError)
		goto ErrorAbort;
	if(nProcType == DILATION)
		nError = ImgSubtract(&ImgProc,
					&rcSrcRelative,
					pImgSrc, 
					prcSrc, 
					pImgContrast,
					prcContrast);

	else
		nError = ImgSubtract(pImgSrc, 
					prcSrc, 
					&ImgProc,
					&rcSrcRelative,
					pImgContrast,
					prcContrast);

	if(nError)
		goto ErrorAbort;
	rcClear[0] = CRect(prcContrast->left,
					   prcContrast->top,
					   prcContrast->left + pcsKernel->cx/2+2,
					   prcContrast->bottom);
	rcClear[1] = CRect(prcContrast->left,
					   prcContrast->top,
					   prcContrast->right,
					   prcContrast->top + pcsKernel->cy/2+2);
	rcClear[2] = CRect(prcContrast->right - pcsKernel->cx/2-2,
					   prcContrast->top,
					   prcContrast->right,
					   prcContrast->bottom);
	rcClear[3] = CRect(prcContrast->left,
					   prcContrast->bottom - pcsKernel->cy/2-2,
					   prcContrast->right,
					   prcContrast->bottom);
	for(i=0;i<4;i++)
		nError = ImgClear(pImgContrast, 
				 &rcClear[i], 
				 0);
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetContrastImg");
	goto ErrorAbort;
}
	return 0;

ErrorAbort:
	return -1;
}


int GetContrastImg(CImageBuffer* pImgSrc,
				   CRect* prcSrc,
				   CSize* pcsKernel,
				   int nMorphologyType,
				   CImageBuffer* pImgMsk,
				   CImageBuffer* pImgContrast,
				   CRect* prcContrast)
{
	int i;
	CMorphology Morphology;
	CImageBuffer ImgProc;
	CRect rcSrcRelative;
	CImageBuffer Kernel;
	CRect rcKernel;
	int nImgType;
	int nProcType;
	CRect rcClear[4];
	int nError;
	CPerformanceTimer TimerExt;

try
{
	nError = ValidateImageAndRect(pImgSrc, prcSrc);
	if(nError)
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgMsk, prcContrast);
	if(nError)
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgContrast, prcContrast);
	if(nError)
		goto ErrorAbort;
	if(prcSrc->Size() != prcContrast->Size())
		goto ErrorAbort;

	rcSrcRelative = CRect(CPoint(0,0), prcSrc->Size());
	ImgProc.Allocate(AbsSize(rcSrcRelative.Size()));
	rcKernel = CRect(0,0,pcsKernel->cx,pcsKernel->cy);
	Kernel.Allocate(AbsSize(rcKernel.Size()));
	Kernel.ClrImg(1);
	nImgType = GRAY_SCALE_IMAGE;
	if(nMorphologyType == DILATION)
		nProcType = DILATION;
	else
		nProcType = EROSION;
	nError = Morphology.Process(pImgSrc,
					prcSrc,
					pImgMsk,
					&rcSrcRelative,
					nImgType,
					nProcType,
					&(rcKernel.Size()),
					&ImgProc,
					&rcSrcRelative);
	if(nError)
		goto ErrorAbort;
	if(nProcType == DILATION)
		nError = ImgSubtract(&ImgProc,
					&rcSrcRelative,
					pImgSrc, 
					prcSrc, 
					pImgContrast,
					prcContrast);
	else
		nError = ImgSubtract(pImgSrc, 
					prcSrc, 
					&ImgProc,
					&rcSrcRelative,
					pImgContrast,
					prcContrast);

	if(nError)
		goto ErrorAbort;
	rcClear[0] = CRect(prcContrast->left,
					   prcContrast->top,
					   prcContrast->left + pcsKernel->cx/2+2,
					   prcContrast->bottom);
	rcClear[1] = CRect(prcContrast->left,
					   prcContrast->top,
					   prcContrast->right,
					   prcContrast->top + pcsKernel->cy/2+2);
	rcClear[2] = CRect(prcContrast->right - pcsKernel->cx/2-2,
					   prcContrast->top,
					   prcContrast->right,
					   prcContrast->bottom);
	rcClear[3] = CRect(prcContrast->left,
					   prcContrast->bottom - pcsKernel->cy/2-2,
					   prcContrast->right,
					   prcContrast->bottom);
	for(i=0;i<4;i++)
	{
		nError = ImgClear(pImgContrast, 
				 &rcClear[i], 
				 0);

			if(nError)
				goto ErrorAbort;
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetContrastImg -1");
	goto ErrorAbort;
}
	return 0;

ErrorAbort:
	return -1;
}

int GetContrastImgBySampling(CImageBuffer* pImgSrc,
				   CRect* prcSrc,
				   CSize* pcsKernel,
				   int nMorphologyType,
				   CImageBuffer* pImgMsk,
				   CImageBuffer* pImgContrast,
				   CRect* prcContrast,
				   CSize* pcsSampling)
{
	int i;
	CMorphology Morphology;
	CImageBuffer ImgProc;
	CRect rcSrcRelative;
	CImageBuffer Kernel;
	CRect rcKernel;
	int nImgType;
	int nProcType;
	CRect rcClear[4];
	int nError;
    CPerformanceTimer TimerExt;

try
{

	nError = ValidateImageAndRect(pImgSrc, prcSrc);
	if(nError)
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgMsk, prcContrast);
	if(nError)
		goto ErrorAbort;
	nError = ValidateImageAndRect(pImgContrast, prcContrast);
	if(nError)
		goto ErrorAbort;
	if(prcSrc->Size() != prcContrast->Size())
		goto ErrorAbort;

	rcSrcRelative = CRect(CPoint(0,0), prcSrc->Size());
	ImgProc.Allocate(AbsSize(rcSrcRelative.Size()));
	ImgProc.ClrImg(0);
	rcKernel = CRect(0,0,pcsKernel->cx,pcsKernel->cy);
	Kernel.Allocate(AbsSize(rcKernel.Size()));
	Kernel.ClrImg(1);
	nImgType = GRAY_SCALE_IMAGE;
	if(nMorphologyType == DILATION)
		nProcType = DILATION;
	else
		nProcType = EROSION;
	nError = Morphology.ProcessBySampling(pImgSrc,
							prcSrc,
							nImgType,
							nProcType,
							&Kernel,
							&rcKernel,
							pImgMsk,
							&ImgProc,
							&rcSrcRelative,
							pcsSampling);
	if(nError)
		goto ErrorAbort;
	if(nProcType == DILATION)
	{
		nError = ImgSubtract(&ImgProc,
					&rcSrcRelative,
					pImgSrc, 
					prcSrc, 
					pImgContrast,
					prcContrast);
	}
	else
	{
		nError = ImgSubtract(pImgSrc, 
					prcSrc, 
					&ImgProc,
					&rcSrcRelative,
					pImgContrast,
					prcContrast);
	}
	nError = ImgAND(pImgContrast,
					prcContrast,
					pImgMsk,
					prcContrast,
					pImgContrast,
					prcContrast);
	if(nError)
		goto ErrorAbort;
	rcClear[0] = CRect(prcContrast->left,
					   prcContrast->top,
					   prcContrast->left + pcsKernel->cx/2+2,
					   prcContrast->bottom);
	rcClear[1] = CRect(prcContrast->left,
					   prcContrast->top,
					   prcContrast->right,
					   prcContrast->top + pcsKernel->cy/2+2);
	rcClear[2] = CRect(prcContrast->right - pcsKernel->cx/2-2,
					   prcContrast->top,
					   prcContrast->right,
					   prcContrast->bottom);
	rcClear[3] = CRect(prcContrast->left,
					   prcContrast->bottom - pcsKernel->cy/2-2,
					   prcContrast->right,
					   prcContrast->bottom);
	for(i=0;i<4;i++)
	{
		nError = ImgClear(pImgContrast, 
				 &rcClear[i], 
				 0);
		if(nError)
			goto ErrorAbort;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetContrastImgBySampling");
	goto ErrorAbort;
}
	return 0;

ErrorAbort:
	return -1;
}



CLensBlobLabeling::CLensBlobLabeling()
{
	pnLabelToBlobIndex = NULL;
	nLabelNo = 0;
	pnBlobIndexToLabel = NULL;
	nBlobNo = 0;
	pSingleBlobInfo = NULL;
}

CLensBlobLabeling::~CLensBlobLabeling()
{
	Clean();
}

void CLensBlobLabeling::Clean()
{
	if(pnLabelToBlobIndex != NULL)
		delete [] pnLabelToBlobIndex;
	pnLabelToBlobIndex = NULL;
	nLabelNo = 0;
	if(pnBlobIndexToLabel != NULL)
		delete [] pnBlobIndexToLabel;
	pnBlobIndexToLabel = NULL;
	nBlobNo = 0;
	if(pSingleBlobInfo != NULL)
		delete [] pSingleBlobInfo;
	pSingleBlobInfo = NULL;
	CleanLabel();
}

void CLensBlobLabeling::CleanLabel()
{
	SegmentIndexAndLabelAry.RemoveAll();
}


int CLensBlobLabeling::GetLabelingImg(CImageBuffer *pImgSrc,
			 CRect *prcSrc,
			 CImageBuffer *pImgDst,
			 CRect *prcDst,
			 int nLabelIndex)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
try {
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

	if (nLabelIndex<0 && nLabelIndex>=256) {
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
			if (*pColSrc == nLabelIndex)
				*pColDst = 255;
			else
				*pColDst = 0;
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CLensBlobLabeling::GetLabelingImg");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CLensBlobLabeling::BlobDetection(IMAGEVIEW_INFO *pView,
							CImageBuffer* pImgGradBinFiltering,
							CRect* prcSearchRelative,
							long lDebugFlag)
{
	int nPkgDiagonalDist;
	int nMaxArea;
	ADV_BLOB_FIND_PARM AdvBlobParm;
	long lProcMode;
	int nError;

try
{
	nPkgDiagonalDist = prcSearchRelative->Width() + prcSearchRelative->Height();
	nMaxArea = prcSearchRelative->Width() * prcSearchRelative->Height();
	AdvBlobParm.nBlobType = WHITE;
	AdvBlobParm.nMinBlobArea = 1;
	AdvBlobParm.nMaxBlobArea = nMaxArea;
	AdvBlobParm.nMaxYBlobSize = nPkgDiagonalDist;
	AdvBlobParm.nMinYBlobSize = 1;
	AdvBlobParm.nMaxXBlobSize = nPkgDiagonalDist;
	AdvBlobParm.nMinXBlobSize = 1;
	AdvBlobParm.dMaxBlobLength = nPkgDiagonalDist;
	AdvBlobParm.dMinBlobLength = 1;
	AdvBlobParm.dMaxBlobElongation = nPkgDiagonalDist;
	AdvBlobParm.dMinBlobElongation = 0;
	AdvBlobParm.rcROISrc = *prcSearchRelative;
	AdvBlobParm.rcROIDst = *prcSearchRelative;
	lProcMode = FIND_BLOB_CONTOUR | FIND_BLOB_LENGTH | FIND_BLOB_ELONGATION;
	nError = BlobFinder.GetAdvBlob3(pView,
					pImgGradBinFiltering,
					&AdvBlobParm,
					lProcMode,
					lDebugFlag);
	if(nError)
		goto ErrorAbort;
	pSegmentBlobFinder = &BlobFinder;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CLensBlobLabeling::BlobDetection");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CLensBlobLabeling::GetBlobLabelImg(IMAGEVIEW_INFO *pView,
							CImageBuffer* pImgGradBinFiltering,
							CRect* prcSearchRelative,
							long lDebugFlag)
{
	int i;
	int nDrwLabel;
	int nDrawBlobIndex;
	int nError;
try {
	Clean();
	bBlobWrapConnection = FALSE;
	if(pSegmentBlobFinder == NULL) {
		nError = BlobDetection(pView,
							pImgGradBinFiltering,
							prcSearchRelative,
							lDebugFlag);
		if(nError)
			goto ErrorAbort;
	}
	if(pSegmentBlobFinder->m_AdvBlobFindRslt.nFndBlobs <= 0)
		goto PASS;
	rcLabel = *prcSearchRelative;
	ImgLabel.Allocate(rcLabel.Size());
	nError = ImgCopy(pImgGradBinFiltering,
					 prcSearchRelative,
					 &ImgLabel,
					 &rcLabel);
	if(nError)
			goto ErrorAbort;
	nError = GetMappingIndex();
	if(nError)
			goto ErrorAbort;
	if(bBlobWrapConnection) {
		for(i=1;i<nLabelNo;i++) {
			nDrawBlobIndex = pnLabelToBlobIndex[i];
			nDrwLabel = i;
			if((nDrawBlobIndex == nMostLeftBlobIndex) || (nDrawBlobIndex == nMostRightBlobIndex))
				nDrwLabel = nShareLabel;
			nError = GetSeedsFillImage(&ImgLabel,
						 &rcLabel,
						 &pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nDrawBlobIndex].pptBlobContour[0],
						 pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nDrawBlobIndex].nBlobContourSize,
						 nDrwLabel);
			if(nError)
				goto ErrorAbort;
		}
	}
	else {
		for(i=1;i<nLabelNo;i++) {
			nDrawBlobIndex = pnLabelToBlobIndex[i];
			nError = GetSeedsFillImage(&ImgLabel,
						 &rcLabel,
						 &pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nDrawBlobIndex].pptBlobContour[0],
						 pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nDrawBlobIndex].nBlobContourSize,
						 i);
			if(nError)
				goto ErrorAbort;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CLensBlobLabeling::GetBlobLabelImg");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}


int CLensBlobLabeling::CheckLensSegment(CPoint2D* ppt2DLensRoughCenter,
										int nLensNominalRadius)
{
	int i, j, m;
	int* pnLabelHist;
	int* pnLabelSortingIndex;
	int nStartAngleDeg;
	int nEndAngleDeg;
	long lSumX;
	long lSumY;
	int nWeightX;
	int nWeightY;
	int nEdgePointSum;
	CPoint ptCenterAry[180];
	BOOL bCenterForX[180];
	BOOL bCenterForY[180];
	double dEdgeRadius;
	double dEdgeRadiusDifPercentage;
	int nError;
try {	
	nNominalRadius = nLensNominalRadius;
	pnLabelHist = NULL;
	pnLabelSortingIndex = NULL;
	if(nLensNominalRadius <= 0)
		goto ErrorAbort;
	pnLabelHist = new int[abs(nLabelNo)];
	pnLabelSortingIndex = new int[abs(nLabelNo)];

	for(i=0;i<nLabelNo;i++)
		pnLabelHist[i] = 0;
	for(i=0;i<SegmentIndexAndLabelAry.GetSize();i++) {
		if((SegmentIndexAndLabelAry[i].y > 0) &&
		   (SegmentIndexAndLabelAry[i].y) < nLabelNo)
			pnLabelHist[SegmentIndexAndLabelAry[i].y]++;
	}
	Sorting(&pnLabelHist[0], nLabelNo, &pnLabelSortingIndex[0], FALSE);
	if(pSingleBlobInfo != NULL)
		delete [] pSingleBlobInfo;
	pSingleBlobInfo = NULL;
	pSingleBlobInfo = new CSingleBlobInfo[abs(nLabelNo)];
	nSingleBlobInfoNo = 0;
	for(i=0;i<nLabelNo;i++) {
		if(pnLabelHist[i] <= 0) 
			break;
		pSingleBlobInfo[nSingleBlobInfoNo].Inspect(pSegmentBlobFinder,
							pnLabelToBlobIndex[pnLabelSortingIndex[i]],
							ppt2DLensRoughCenter,
							nLensNominalRadius);
		nSingleBlobInfoNo++;
	}
	for(i=0;i<360;i++) {
		bAngleFilled[i] = FALSE;
		ptEdge[i] = CPoint(-1,-1);
	}
	for(i=0;i<nSingleBlobInfoNo;i++) {
		pSingleBlobInfo[i].bSelected = TRUE;
		for(j=0;j<pSingleBlobInfo[i].nAngleRange;j++) {
			nStartAngleDeg = pSingleBlobInfo[i].nStartAngleDeg[j];
			nEndAngleDeg = pSingleBlobInfo[i].nEndAngleDeg[j];
			if(bAngleFilled[nStartAngleDeg] || bAngleFilled[nEndAngleDeg]) {
				pSingleBlobInfo[i].bSelected = FALSE;
			}
		}
		if(pSingleBlobInfo[i].bSelected == FALSE)
			continue;
		pSingleBlobInfo[i].bSelected = TRUE;
		for(j=0;j<pSingleBlobInfo[i].nAngleRange;j++) {
			nStartAngleDeg = pSingleBlobInfo[i].nStartAngleDeg[j];
			nEndAngleDeg = pSingleBlobInfo[i].nEndAngleDeg[j];
			for(m=nStartAngleDeg;m<nEndAngleDeg;m++) 
				bAngleFilled[m] = TRUE;
		}
	}
	rcLensBounding.SetRectEmpty();
	for(i=0;i<nSingleBlobInfoNo;i++) {
		if(pSingleBlobInfo[i].bSelected == FALSE)
			continue;
		nError = pSingleBlobInfo[i].InterpolationContour();
		if(nError)
			goto ErrorAbort;
		for(j=0;j<360;j++) {
			if(pSingleBlobInfo[i].ptEdge[j].x > 0) {
				ptEdge[j] = pSingleBlobInfo[i].ptEdge[j];
			}
		}
		rcLensBounding.UnionRect(rcLensBounding,pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[pSingleBlobInfo[i].nBlobIndex].rcFndBlobPos);
	}
	for(i=0;i<180;i++) {
		bCenterForX[i] = FALSE;
		bCenterForY[i] = FALSE;
		if((ptEdge[i].x > 0) && (ptEdge[i].y > 0) &&
			(ptEdge[i+180].x > 0) && (ptEdge[i+180].y > 0)) 
			ptCenterAry[i] = CPoint((ptEdge[i].x + ptEdge[i+180].x)/2,
									(ptEdge[i].y + ptEdge[i+180].y)/2);
		else
			ptCenterAry[i] = CPoint(-1,-1);

	}
	for(i=0;i<45;i++) 
		bCenterForX[i] = TRUE;
	for(i=135;i<180;i++) 
		bCenterForX[i] = TRUE;
	for(i=45;i<135;i++) 
		bCenterForY[i] = TRUE;

	lSumX = 0;
	lSumY = 0;
	nWeightX = 0;
	nWeightY = 0;
	for(i=0;i<180;i++) {
		if(bCenterForX[i] && (ptCenterAry[i].x > 0)) {
			lSumX = lSumX + ptEdge[i].x + ptEdge[i+180].x;
			nWeightX+=2;
		}
		if(bCenterForY[i] && (ptCenterAry[i].x > 0)) {
			lSumY = lSumY + ptEdge[i].y + ptEdge[i+180].y;
			nWeightY+=2;
		}
	}
	if((nWeightX > 0) && (nWeightY > 0))
		ptCenter = CPoint(lSumX/nWeightX, lSumY/nWeightY);
	else
		ptCenter = rcLensBounding.CenterPoint();
	
	nError = GetSegmentRadius();
	if(nError)
			goto ErrorAbort;
	nEdgePointSum = 0;
	for(i=0;i<360;i++) {
		if(bAngleFilled[i] == FALSE)
			continue;
		dEdgeRadius = sqrt((double) ((ptEdge[i].x - ptCenter.x) * (ptEdge[i].x - ptCenter.x) + 
			(ptEdge[i].y - ptCenter.y) * (ptEdge[i].y - ptCenter.y)));
		dEdgeRadiusDifPercentage = fabs((dEdgeRadius - nLensNominalRadius) * 100.0/nLensNominalRadius);
		if(dEdgeRadiusDifPercentage > 25)
			continue;
		nEdgePointSum++;
	}
	nMatchingPercentage = 100 * nEdgePointSum/360;

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CLensBlobLabeling::CheckLensSegment");
	goto ErrorAbort;
}
	if(pnLabelHist != NULL)
		delete [] pnLabelHist;
	if(pnLabelSortingIndex != NULL)
		delete []pnLabelSortingIndex;
	return 0;
ErrorAbort:
	if(pnLabelHist != NULL)
		delete [] pnLabelHist;
	if(pnLabelSortingIndex != NULL)
		delete []pnLabelSortingIndex;
	return -1;
}



int CLensBlobLabeling::GetSegmentRadius()
{
	int i, j;
	int nIndex;
	long lSumX;
	long lSumY;
	int nWeight;
	CPoint ptSegmentAvg[36];
	int nMaxDif;

	int nDif;
try {
	bBroken = FALSE;
	nIndex = 0;
	for(i=0;i<36;i++) {
		lSumX = 0;
		lSumY = 0;
		nWeight = 0;
		for(j=0;j<10;j++) {
			if((ptEdge[nIndex].x > 0) && (ptEdge[nIndex].y > 0)) {
				lSumX += ptEdge[nIndex].x;
				lSumY += ptEdge[nIndex].y;
				nWeight++;
			}
			nIndex++;
		}
		if(nWeight > 0) {
			ptSegmentAvg[i] = CPoint(lSumX/nWeight, lSumY/nWeight);
			nRadius[i] = int(sqrt((double) ((ptCenter.x - ptSegmentAvg[i].x) * (ptCenter.x - ptSegmentAvg[i].x) +
							  (ptCenter.y - ptSegmentAvg[i].y) * (ptCenter.y - ptSegmentAvg[i].y))));
		}
		else {
			ptSegmentAvg[i] = CPoint(-1,-1);
			nRadius[i] = -1;
		}
	}
	nMaxDif = 0;
	for(i=0;i<36;i++) {
		if(nRadius[i] > 0) {
			nDif = abs(nNominalRadius - nRadius[i]);
			if(nMaxDif < nDif)
				nMaxDif = nDif;
		}
		else 
			bBroken = TRUE;
	}
	nRadiusMaxVar = nMaxDif;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CLensBlobLabeling::GetSegmentRadius");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CLensBlobLabeling::DrawLensEdge(IMAGEVIEW_INFO *pView,
									LOGPEN* pPen,
									CPoint ptOffset,
									long lDebugFlag)
{
	int i;
	CSize csFOV;

try {
	csFOV = pView->sizeImage;
	for(i=1;i<360;i++) {
		if((ValidatePoint(&ptEdge[i-1], &csFOV)) ||
		   (ValidatePoint(&ptEdge[i], &csFOV)))
		   continue;
		DrwLine(pView, 
				&(ptEdge[i-1] + ptOffset),
				&(ptEdge[i] + ptOffset),
				pPen);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CLensBlobLabeling::DrawLensEdge");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CLensBlobLabeling::GetMappingIndex()
{
	int m, n, i, j;
	int* pnBlobLengthSorting;
	int* pnSortingIndex;
	int nLeftMostPos;
	int nRightMostPos;
	CArray<int, int> ArynLeftMostYPos;
	CArray<int, int> ArynLeftMostBlobIndex;
	CArray<int, int> ArynRightMostBlobIndex;
try {
	pnBlobLengthSorting = NULL;
	pnSortingIndex = NULL;
	ArynLeftMostYPos.RemoveAll();
	ArynLeftMostBlobIndex.RemoveAll();
	ArynRightMostBlobIndex.RemoveAll();
	nBlobNo = pSegmentBlobFinder->m_AdvBlobFindRslt.nFndBlobs;
	if(pnBlobIndexToLabel != NULL)
		delete [] pnBlobIndexToLabel;
	pnBlobIndexToLabel = NULL;
	if(pnLabelToBlobIndex != NULL)
		delete [] pnLabelToBlobIndex;
	pnLabelToBlobIndex = NULL;
	pnBlobLengthSorting = new int[abs(nBlobNo)];
	pnBlobIndexToLabel = new int[abs(nBlobNo)];
	pnSortingIndex = new int[abs(nBlobNo)];
	nLabelNo = nBlobNo + 1;
	if(nBlobNo <= 0)
		goto PASS;
	nMostLeftBlobIndex = 0;
	nLeftMostPos = pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[0].rcFndBlobPos.left;
	nMostRightBlobIndex = 0;
	nRightMostPos = pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[0].rcFndBlobPos.right;
	for(i=1;i<nBlobNo;i++) {
		if(nLeftMostPos > pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[i].rcFndBlobPos.left) {
			nLeftMostPos = pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[i].rcFndBlobPos.left;
			nMostLeftBlobIndex = i;
		}
		if(nRightMostPos < pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[i].rcFndBlobPos.right) {
			nRightMostPos = pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[i].rcFndBlobPos.right;
			nMostRightBlobIndex = i;
		}
	}
	if((nLeftMostPos < 3) &&
		(nRightMostPos > (prcSegmentBlobFinder->right - 3))) {
		nLeftMostPos++;
		nRightMostPos--;
		for(i=0;i<nBlobNo;i++) {
			if(pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[i].rcFndBlobPos.left <= nLeftMostPos) {
				ArynLeftMostBlobIndex.Add(i);
			}
			if(pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[i].rcFndBlobPos.right >= nRightMostPos) {
				ArynRightMostBlobIndex.Add(i);
			}
		}
		
		for(m=0;m<ArynLeftMostBlobIndex.GetSize();m++) {
			nMostLeftBlobIndex = ArynLeftMostBlobIndex.GetAt(m);
			if(ArynLeftMostBlobIndex.GetSize() > 1) {
				if(pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nMostLeftBlobIndex].rcFndBlobPos.Width() < 10)
					continue;
			}
			for(n=0;n<ArynRightMostBlobIndex.GetSize();n++) {
				nMostRightBlobIndex = ArynRightMostBlobIndex.GetAt(n);
				if(ArynRightMostBlobIndex.GetSize() > 1) {
					if(pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nMostRightBlobIndex].rcFndBlobPos.Width() < 10)
						continue;
				}
				for(i=0;i<pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nMostLeftBlobIndex].nBlobContourSize;i++) {
					if(pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nMostLeftBlobIndex].pptBlobContour[i].x <= nLeftMostPos) {
						ArynLeftMostYPos.Add(pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nMostLeftBlobIndex].pptBlobContour[i].y);
					}
				}
				bBlobWrapConnection = FALSE;
				for(i=0;i<pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nMostRightBlobIndex].nBlobContourSize;i++) {
					if(pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nMostRightBlobIndex].pptBlobContour[i].x >= nRightMostPos) {
						for(j=0;j<ArynLeftMostYPos.GetSize();j++) {
							if(abs(pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nMostRightBlobIndex].pptBlobContour[i].y - ArynLeftMostYPos.GetAt(j)) < 2) {
								bBlobWrapConnection = TRUE;
								goto FoundOverlap;
							}
						}
					}
				}
			}
		}
	}
FoundOverlap:
	if(nLabelNo > 250)
		nLabelNo = 250;
	pnLabelToBlobIndex = new int[abs(nLabelNo)];
	for(i=0;i<nBlobNo;i++) 
		pnBlobLengthSorting[i] = int(pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[i].dFndBlobLength);
	if(bBlobWrapConnection) {
		pnBlobLengthSorting[nMostLeftBlobIndex] += pnBlobLengthSorting[nMostRightBlobIndex];
		pnBlobLengthSorting[nMostRightBlobIndex] = pnBlobLengthSorting[nMostLeftBlobIndex];
	}
	Sorting(&pnBlobLengthSorting[0], nBlobNo, &pnSortingIndex[0], FALSE);
	pnLabelToBlobIndex[0] = -1;
	for(i=0;i<nBlobNo;i++) 
		pnBlobIndexToLabel[i] = 0;
	for(i=1;i<nLabelNo;i++) {
		pnLabelToBlobIndex[i] = pnSortingIndex[i-1];
		pnBlobIndexToLabel[pnSortingIndex[i-1]] = i;
	}
	if(bBlobWrapConnection) {
		if((pnBlobIndexToLabel[nMostLeftBlobIndex] == 1) ||(pnBlobIndexToLabel[nMostRightBlobIndex] == 1)) {
			nShareLabel = 1;
		}
		else {
			if(pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nMostLeftBlobIndex].dFndBlobLength > pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nMostRightBlobIndex].dFndBlobLength) 
				nShareLabel = pnBlobIndexToLabel[nMostLeftBlobIndex];
			else
				nShareLabel = pnBlobIndexToLabel[nMostRightBlobIndex];
		}		
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CLensBlobLabeling::GetMappingIndex");
	goto ErrorAbort;
}
PASS:
	if(pnBlobLengthSorting != NULL)
		delete [] pnBlobLengthSorting;
	if(pnSortingIndex != NULL)
		delete [] pnSortingIndex;
	ArynLeftMostYPos.RemoveAll();
	return 0;
ErrorAbort:
	if(pnBlobLengthSorting != NULL)
		delete [] pnBlobLengthSorting;
	if(pnSortingIndex != NULL)
		delete [] pnSortingIndex;
	ArynLeftMostYPos.RemoveAll();
	return -1;
}
/*
CLensBlobLabeling::CLensBlobLabeling()
{
	pnLabelToBlobIndex = NULL;
	nLabelNo = 0;
	pnBlobIndexToLabel = NULL;
	nBlobNo = 0;
	pSingleBlobInfo = NULL;
}

CLensBlobLabeling::~CLensBlobLabeling()
{
	Clean();
}

void CLensBlobLabeling::Clean()
{
	if(pnLabelToBlobIndex != NULL)
		delete [] pnLabelToBlobIndex;
	pnLabelToBlobIndex = NULL;
	nLabelNo = 0;
	if(pnBlobIndexToLabel != NULL)
		delete [] pnBlobIndexToLabel;
	pnBlobIndexToLabel = NULL;
	nBlobNo = 0;
	if(pSingleBlobInfo != NULL)
		delete [] pSingleBlobInfo;
	pSingleBlobInfo = NULL;
	CleanLabel();
}

void CLensBlobLabeling::CleanLabel()
{
	pSegmentBlobFinder = NULL;
	SegmentIndexAndLabelAry.RemoveAll();
}

int CLensBlobLabeling::BlobDetection(IMAGEVIEW_INFO *pView,
							CImageBuffer* pImgGradBinFiltering,
							CRect* prcSearchRelative,
							long lDebugFlag)
{
	int nPkgDiagonalDist;
	int nMaxArea;
	ADV_BLOB_FIND_PARM AdvBlobParm;
	long lProcMode;
	int nError;

try
{
	nPkgDiagonalDist = prcSearchRelative->Width() + prcSearchRelative->Height();
	nMaxArea = prcSearchRelative->Width() * prcSearchRelative->Height();
	AdvBlobParm.nBlobType = WHITE;
	AdvBlobParm.nMinBlobArea = 1;
	AdvBlobParm.nMaxBlobArea = nMaxArea;
	AdvBlobParm.nMaxYBlobSize = nPkgDiagonalDist;
	AdvBlobParm.nMinYBlobSize = 1;
	AdvBlobParm.nMaxXBlobSize = nPkgDiagonalDist;
	AdvBlobParm.nMinXBlobSize = 1;
	AdvBlobParm.dMaxBlobLength = nPkgDiagonalDist;
	AdvBlobParm.dMinBlobLength = 1;
	AdvBlobParm.dMaxBlobElongation = nPkgDiagonalDist;
	AdvBlobParm.dMinBlobElongation = 0;
	AdvBlobParm.rcROISrc = *prcSearchRelative;
	AdvBlobParm.rcROIDst = *prcSearchRelative;
	lProcMode = FIND_BLOB_CONTOUR | FIND_BLOB_LENGTH | FIND_BLOB_ELONGATION;
	nError = BlobFinder.GetAdvBlob3(pView,
					pImgGradBinFiltering,
					&AdvBlobParm,
					lProcMode,
					lDebugFlag);
	if(nError)
		goto ErrorAbort;
	pSegmentBlobFinder = &BlobFinder;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CLensBlobLabeling::BlobDetection");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CLensBlobLabeling::GetBlobLabelImg(IMAGEVIEW_INFO *pView,
							CImageBuffer* pImgGradBinFiltering,
							CRect* prcSearchRelative,
							long lDebugFlag)
{
	int i;
	int nDrawBlobIndex;
	int nError;

try
{
	Clean();
	if(pSegmentBlobFinder == NULL) {
		nError = BlobDetection(pView,
							pImgGradBinFiltering,
							prcSearchRelative,
							lDebugFlag);
		if(nError)
			goto ErrorAbort;
	}
	if(pSegmentBlobFinder->m_AdvBlobFindRslt.nFndBlobs <= 0)
		goto PASS;
	rcLabel = *prcSearchRelative;
	ImgLabel.Allocate(AbsSize(rcLabel.Size()));
	nError = ImgCopy(pImgGradBinFiltering,
					 prcSearchRelative,
					 &ImgLabel,
					 &rcLabel);
	nError = GetMappingIndex();
	for(i=1;i<nLabelNo;i++) {
		nDrawBlobIndex = pnLabelToBlobIndex[i];
		nError = GetSeedsFillImage(&ImgLabel,
					 &rcLabel,
					 &pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nDrawBlobIndex].pptBlobContour[0],
					 pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nDrawBlobIndex].nBlobContourSize,
					 i);

		if(nError)
			goto ErrorAbort;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CLensBlobLabeling::GetBlobLabelImg");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

int CLensBlobLabeling::CheckLensSegment(CPoint2D* ppt2DLensRoughCenter,
										int nLensNominalRadius)
{
	int i, j, m;
	int* pnLabelHist;
	int* pnLabelSortingIndex;
	int nStartAngleDeg;
	int nEndAngleDeg;
	long lSumX;
	long lSumY;
	int nWeightX;
	int nWeightY;
	int nEdgePointSum;
	CPoint ptCenterAry[180];
	BOOL bCenterForX[180];
	BOOL bCenterForY[180];
	int nError;

try
{
	nNominalRadius = nLensNominalRadius;
	pnLabelHist = NULL;
	pnLabelSortingIndex = NULL;
	pnLabelHist = new int[abs(nLabelNo)];
	pnLabelSortingIndex = new int[abs(nLabelNo)];

	for(i=0;i<nLabelNo;i++)
		pnLabelHist[i] = 0;
	for(i=0;i<SegmentIndexAndLabelAry.GetSize();i++) {
		if((SegmentIndexAndLabelAry[i].y > 0) &&
		   (SegmentIndexAndLabelAry[i].y) < nLabelNo)
			pnLabelHist[SegmentIndexAndLabelAry[i].y]++;
	}
	Sorting(&pnLabelHist[0], nLabelNo, &pnLabelSortingIndex[0], FALSE);
	pSingleBlobInfo = new CSingleBlobInfo[abs(nLabelNo)];
	nSingleBlobInfoNo = 0;
	for(i=0;i<nLabelNo;i++) {
		if(pnLabelHist[i] <= 0) 
			break;
		pSingleBlobInfo[nSingleBlobInfoNo].Inspect(pSegmentBlobFinder,
							pnLabelToBlobIndex[pnLabelSortingIndex[i]],
							ppt2DLensRoughCenter,
							nLensNominalRadius);
		nSingleBlobInfoNo++;
	}
	for(i=0;i<360;i++) {
		bAngleFilled[i] = FALSE;
		ptEdge[i] = CPoint(-1,-1);
	}
	for(i=0;i<nSingleBlobInfoNo;i++) {
		pSingleBlobInfo[i].bSelected = TRUE;
		for(j=0;j<pSingleBlobInfo[i].nAngleRange;j++) {
			nStartAngleDeg = pSingleBlobInfo[i].nStartAngleDeg[j];
			nEndAngleDeg = pSingleBlobInfo[i].nEndAngleDeg[j];
			if(bAngleFilled[nStartAngleDeg] || bAngleFilled[nEndAngleDeg]) {
				pSingleBlobInfo[i].bSelected = FALSE;
			}
		}
		if(pSingleBlobInfo[i].bSelected == FALSE)
			continue;
		pSingleBlobInfo[i].bSelected = TRUE;
		for(j=0;j<pSingleBlobInfo[i].nAngleRange;j++) {
			nStartAngleDeg = pSingleBlobInfo[i].nStartAngleDeg[j];
			nEndAngleDeg = pSingleBlobInfo[i].nEndAngleDeg[j];
			for(m=nStartAngleDeg;m<nEndAngleDeg;m++) 
				bAngleFilled[m] = TRUE;
		}
	}
	rcLensBounding.SetRectEmpty();
	for(i=0;i<nSingleBlobInfoNo;i++) {
		if(pSingleBlobInfo[i].bSelected == FALSE)
			continue;
		nError = pSingleBlobInfo[i].InterpolationContour();
		if(nError)
			goto ErrorAbort;
		for(j=0;j<360;j++) {
			if(pSingleBlobInfo[i].ptEdge[j].x > 0) {
				ptEdge[j] = pSingleBlobInfo[i].ptEdge[j];
			}
		}
		rcLensBounding.UnionRect(rcLensBounding,pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[pSingleBlobInfo[i].nBlobIndex].rcFndBlobPos);
	}
	for(i=0;i<180;i++) {
		bCenterForX[i] = FALSE;
		bCenterForY[i] = FALSE;
		if((ptEdge[i].x > 0) && (ptEdge[i].y > 0) &&
			(ptEdge[i+180].x > 0) && (ptEdge[i+180].y > 0)) 
			ptCenterAry[i] = CPoint((ptEdge[i].x + ptEdge[i+180].x)/2,
									(ptEdge[i].y + ptEdge[i+180].y)/2);
		else
			ptCenterAry[i] = CPoint(-1,-1);

	}
	for(i=0;i<45;i++) 
		bCenterForX[i] = TRUE;
	for(i=135;i<180;i++) 
		bCenterForX[i] = TRUE;
	for(i=45;i<135;i++) 
		bCenterForY[i] = TRUE;

	lSumX = 0;
	lSumY = 0;
	nWeightX = 0;
	nWeightY = 0;
	for(i=0;i<180;i++) {
		if(bCenterForX[i] && (ptCenterAry[i].x > 0)) {
			lSumX = lSumX + ptEdge[i].x + ptEdge[i+180].x;
			nWeightX+=2;
		}
		if(bCenterForY[i] && (ptCenterAry[i].x > 0)) {
			lSumY = lSumY + ptEdge[i].y + ptEdge[i+180].y;
			nWeightY+=2;
		}
	}
	if((nWeightX > 0) && (nWeightY > 0))
		ptCenter = CPoint(lSumX/nWeightX, lSumY/nWeightY);
	else
		ptCenter = rcLensBounding.CenterPoint();
	
	nError = GetSegmentRadius();
	if(nError)
			goto ErrorAbort;
	nEdgePointSum = 0;
	for(i=0;i<360;i++)
		nEdgePointSum += bAngleFilled[i];
	nMatchingPercentage = 100 * nEdgePointSum/360;

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CLensBlobLabeling::CheckLensSegment");
	goto ErrorAbort;
}
	if(pnLabelHist != NULL)
		delete [] pnLabelHist;
	if(pnLabelSortingIndex != NULL)
		delete []pnLabelSortingIndex;
	return 0;
ErrorAbort:
	if(pnLabelHist != NULL)
		delete [] pnLabelHist;
	if(pnLabelSortingIndex != NULL)
		delete []pnLabelSortingIndex;
	return -1;
}

int CLensBlobLabeling::GetSegmentRadius()
{
	int i, j;
	int nIndex;
	long lSumX;
	long lSumY;
	int nWeight;
	CPoint ptSegmentAvg[36];
	int nMaxDif;

	int nDif;
try
{

	bBroken = FALSE;
	nIndex = 0;
	for(i=0;i<36;i++) {
		lSumX = 0;
		lSumY = 0;
		nWeight = 0;
		for(j=0;j<10;j++) {
			if((ptEdge[nIndex].x > 0) && (ptEdge[nIndex].y > 0)) {
				lSumX += ptEdge[nIndex].x;
				lSumY += ptEdge[nIndex].y;
				nWeight++;
			}
			nIndex++;
		}
		if(nWeight > 0) {
			ptSegmentAvg[i] = CPoint(lSumX/nWeight, lSumY/nWeight);
			nRadius[i] = int(sqrt((ptCenter.x - ptSegmentAvg[i].x) * (ptCenter.x - ptSegmentAvg[i].x) +
							  (ptCenter.y - ptSegmentAvg[i].y) * (ptCenter.y - ptSegmentAvg[i].y)));
		}
		else {
			ptSegmentAvg[i] = CPoint(-1,-1);
			nRadius[i] = -1;
		}
	}
	nMaxDif = 0;
	for(i=0;i<36;i++) {
		if(nRadius[i] > 0) {
			nDif = abs(nNominalRadius - nRadius[i]);
			if(nMaxDif < nDif)
				nMaxDif = nDif;
		}
		else 
			bBroken = TRUE;
	}
	nRadiusMaxVar = nMaxDif;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CLensBlobLabeling::GetSegmentRadius");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CLensBlobLabeling::DrawLensEdge(IMAGEVIEW_INFO *pView,
									LOGPEN* pPen,
									CPoint ptOffset,
									long lDebugFlag)
{
	int i;
	CSize csFOV;

try
{
	csFOV = pView->sizeImage;
	for(i=1;i<360;i++) {
		if((ValidatePoint(&ptEdge[i-1], &csFOV)) ||
		   (ValidatePoint(&ptEdge[i], &csFOV)))
		   continue;
		DrwLine(pView, 
				&(ptEdge[i-1] + ptOffset),
				&(ptEdge[i] + ptOffset),
				pPen);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CLensBlobLabeling::DrawLensEdge");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CLensBlobLabeling::GetLensEdgePoints(IMAGEVIEW_INFO *pView,
										CPoint ptOffset,
										CPoint *pptEdge)
{
	int i;
	CSize csFOV;

	csFOV = pView->sizeImage;
	for(i=0;i<360;i++) {
		pptEdge[i] = ptEdge[i] + ptOffset;
	}
	return 0;
}

int CLensBlobLabeling::GetMappingIndex()
{
	int i;
	int* pnBlobLengthSorting;
	int* pnSortingIndex;

try
{
	pnBlobLengthSorting = NULL;
	pnSortingIndex = NULL;
	nBlobNo = pSegmentBlobFinder->m_AdvBlobFindRslt.nFndBlobs;
	if(nBlobNo < 1)
		goto ErrorAbort;
	pnBlobLengthSorting = new int[abs(nBlobNo)];
	pnBlobIndexToLabel = new int[abs(nBlobNo)];
	pnSortingIndex = new int[abs(nBlobNo)];
	nLabelNo = nBlobNo + 1;
	if(nLabelNo > 250)
		nLabelNo = 250;
	pnLabelToBlobIndex = new int[abs(nLabelNo)];
	for(i=0;i<nBlobNo;i++) 
		pnBlobLengthSorting[i] = int(pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[i].dFndBlobLength);
	Sorting(&pnBlobLengthSorting[0], nBlobNo, &pnSortingIndex[0], FALSE);
	pnLabelToBlobIndex[0] = -1;
	for(i=0;i<nBlobNo;i++) 
		pnBlobIndexToLabel[i] = 0;
	for(i=1;i<nLabelNo;i++) {
		pnLabelToBlobIndex[i] = pnSortingIndex[i-1];
		pnBlobIndexToLabel[pnSortingIndex[i-1]] = i;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CLensBlobLabeling::GetMappingIndex");
	goto ErrorAbort;
}
	if(pnBlobLengthSorting != NULL)
		delete [] pnBlobLengthSorting;
	if(pnSortingIndex != NULL)
		delete [] pnSortingIndex;
	return 0;
ErrorAbort:
	if(pnBlobLengthSorting != NULL)
		delete [] pnBlobLengthSorting;
	if(pnSortingIndex != NULL)
		delete [] pnSortingIndex;
	return -1;
}
int CLensBlobLabeling::CheckLensSegment2(CPoint2D* ppt2DLensRoughCenter,
										int nBlobIndex,
										int nLensNominalRadius)
{
	int i, j, m;
	int nStartAngleDeg;
	int nEndAngleDeg;
	long lSumX;
	long lSumY;
	int nWeightX;
	int nWeightY;
	int nEdgePointSum;
	CPoint ptCenterAry[180];
	BOOL bCenterForX[180];
	BOOL bCenterForY[180];
	int nError;
try
{

	CSingleBlobInfo IrisSingleBlobInfo;
	nNominalRadius = nLensNominalRadius;
	nSingleBlobInfoNo = 0;

	IrisSingleBlobInfo.Inspect(pSegmentBlobFinder,
								nBlobIndex,
								ppt2DLensRoughCenter,
								nLensNominalRadius);
	
	for(i=0;i<360;i++) {
		bAngleFilled[i] = FALSE;
		ptEdge[i] = CPoint(-1,-1);
	}

	IrisSingleBlobInfo.bSelected = TRUE;
	
	for(j=0; j<IrisSingleBlobInfo.nAngleRange; j++) {
		nStartAngleDeg = IrisSingleBlobInfo.nStartAngleDeg[j];
		nEndAngleDeg = IrisSingleBlobInfo.nEndAngleDeg[j];
		if(bAngleFilled[nStartAngleDeg] || bAngleFilled[nEndAngleDeg]) {
			IrisSingleBlobInfo.bSelected = FALSE;
		}
	}
	
	if(IrisSingleBlobInfo.bSelected == FALSE)
			goto ReturnError;
		
	for(j=0; j<IrisSingleBlobInfo.nAngleRange; j++) {
		nStartAngleDeg = IrisSingleBlobInfo.nStartAngleDeg[j];
		nEndAngleDeg = IrisSingleBlobInfo.nEndAngleDeg[j];
		for(m=nStartAngleDeg;m<nEndAngleDeg;m++) 
			bAngleFilled[m] = TRUE;
	}

	rcLensBounding.SetRectEmpty();

	nError = IrisSingleBlobInfo.InterpolationContour();
	if(nError)
		goto ReturnError;

	for(j=0;j<360;j++) {
		if(IrisSingleBlobInfo.ptEdge[j].x > 0) {
			ptEdge[j] = IrisSingleBlobInfo.ptEdge[j];
		}
	}
	rcLensBounding.UnionRect(rcLensBounding,pSegmentBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[IrisSingleBlobInfo.nBlobIndex].rcFndBlobPos);
	
	for(i=0;i<180;i++) {
		bCenterForX[i] = FALSE;
		bCenterForY[i] = FALSE;
		if((ptEdge[i].x > 0) && (ptEdge[i].y > 0) &&
			(ptEdge[i+180].x > 0) && (ptEdge[i+180].y > 0)) 
			ptCenterAry[i] = CPoint((ptEdge[i].x + ptEdge[i+180].x)/2,
									(ptEdge[i].y + ptEdge[i+180].y)/2);
		else
			ptCenterAry[i] = CPoint(-1,-1);

	}
	for(i=0;i<45;i++) 
		bCenterForX[i] = TRUE;
	for(i=135;i<180;i++) 
		bCenterForX[i] = TRUE;
	for(i=45;i<135;i++) 
		bCenterForY[i] = TRUE;

	lSumX = 0;
	lSumY = 0;
	nWeightX = 0;
	nWeightY = 0;
	for(i=0;i<180;i++) {
		if(bCenterForX[i] && (ptCenterAry[i].x > 0)) {
			lSumX = lSumX + ptEdge[i].x + ptEdge[i+180].x;
			nWeightX+=2;
		}
		if(bCenterForY[i] && (ptCenterAry[i].x > 0)) {
			lSumY = lSumY + ptEdge[i].y + ptEdge[i+180].y;
			nWeightY+=2;
		}
	}
	if((nWeightX > 0) && (nWeightY > 0))
		ptCenter = CPoint(lSumX/nWeightX, lSumY/nWeightY);
	else
		ptCenter = rcLensBounding.CenterPoint();
	
	nError = GetSegmentRadius();
	if(nError)
		goto ErrorAbort;
	nEdgePointSum = 0;
	for(i=0;i<360;i++)
		nEdgePointSum += bAngleFilled[i];
	nMatchingPercentage = 100 * nEdgePointSum/360;

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CLensBlobLabeling::CheckLensSegment2");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
ReturnError:
	return -1;
}
*/
CSingleBlobInfo::CSingleBlobInfo()
{
}

CSingleBlobInfo::~CSingleBlobInfo()
{
	Clean();
}

void CSingleBlobInfo::Clean()
{
}

int CSingleBlobInfo::Inspect(CAdvBlobFind* pBlobFinder,
				int nSingleBlobIndex,
				CPoint2D* ppt2DLensRoughCenter,
				int nLensNominalRadius)
{
	int i;
	int* pnAngle;
	int nAngleNo;
	int nStep;
	double dAngleStep;
	int nAngleBrokenDegTol;
	double dx, dy;
	int nIndex;
	double dAngle;
	int nAngle;
	double PITimes2;
	BOOL bBroken;
	double dRadToDegRatio;

try
{
	pnAngle = NULL;
	dRadToDegRatio = 180/PI;
	dAngleStep = 6/dRadToDegRatio;
	nAngleBrokenDegTol = Round(2 * dAngleStep * dRadToDegRatio);
	nStep = int(nLensNominalRadius * dAngleStep);
	PITimes2 = 2 * PI;
	nBlobIndex = nSingleBlobIndex;
	for(i=0;i<360;i++) {
		ptEdge[i] = CPoint(-1,-1);
		bEdgePt[i] = FALSE; 
	}
	nAngleNo = pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].nBlobContourSize/nStep;
	if(nAngleNo < 1)
		goto ErrorAbort;
	pnAngle = new int[abs(nAngleNo)];
	for(i=0;i<nAngleNo;i++) {
		nIndex = i * nStep;
		dx = pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].pptBlobContour[nIndex].x - ppt2DLensRoughCenter->x;
		dy = pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].pptBlobContour[nIndex].y - ppt2DLensRoughCenter->y;
		dAngle = atan2(dy,dx);
		if(dAngle < 0)
			dAngle += PITimes2;
		nAngle = int(dAngle * dRadToDegRatio);
		if(nAngle >= 360)
			nAngle = 359;
		pnAngle[i] = nAngle;
		ptEdge[nAngle] = pBlobFinder->m_AdvBlobFindRslt.pAdvBlobInfo[nBlobIndex].pptBlobContour[nIndex];
		bEdgePt[nAngle] = TRUE; 
	}
	Sorting(&pnAngle[0], nAngleNo, TRUE);
	nAngleRange = 1;
	nStartAngleDeg[0] = pnAngle[0];
	bBroken = FALSE;
	for(i=1;i<nAngleNo;i++) {
		if((pnAngle[i] - pnAngle[i-1]) > nAngleBrokenDegTol) {
			nEndAngleDeg[0] = pnAngle[i-1];
			nStartAngleDeg[1] = pnAngle[i];
			bBroken = TRUE;
			break;
		}
	}
	if(bBroken == FALSE)
		nEndAngleDeg[0] = pnAngle[nAngleNo-1];
	else {
		nEndAngleDeg[1] = pnAngle[nAngleNo-1];
		nAngleRange = 2;
	}
	for(i=0;i<nAngleRange;i++) {
		if(nStartAngleDeg[i] < 0)
			nStartAngleDeg[i] = 0;
		if(nStartAngleDeg[i] > 360)
			nStartAngleDeg[i] = 360;
		if(nEndAngleDeg[i] < 0)
			nEndAngleDeg[i] = 0;
		if(nEndAngleDeg[i] > 360)
			nEndAngleDeg[i] = 360;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CSingleBlobInfo::Inspect");
	goto ErrorAbort;
}
	if(pnAngle != NULL)
		delete [] pnAngle;
	return 0;
ErrorAbort:
	if(pnAngle != NULL)
		delete [] pnAngle;
	return -1;
}

int CSingleBlobInfo::InterpolationContour()
{
	int i, j, m;
	int nStartIndex;
	int nEndIndex;

try
{
	for(i=0;i<nAngleRange;i++) {
		for(j=nStartAngleDeg[i]+1;j<nEndAngleDeg[i];j++) {
			nStartIndex = -1;
			nEndIndex = -1;
			for(m=j;m<nEndAngleDeg[i];m++) {
				if((bEdgePt[m-1] == TRUE) && (bEdgePt[m] == FALSE)) {
					nStartIndex = m-1;
					break;
				}
			}
			for(m=nStartIndex+1;m<nEndAngleDeg[i];m++) {
				if((bEdgePt[m-1] == FALSE) && (bEdgePt[m] == TRUE)) {
					nEndIndex = m;
					break;
				}
			}
			if((nStartIndex < 0) || 
			   (nEndIndex < 0) ||
			   (nStartIndex == nEndIndex))
				continue;
			for(m=nStartIndex+1;m<nEndIndex;m++) {
				ptEdge[m].x = ptEdge[nStartIndex].x + (ptEdge[nEndIndex].x - ptEdge[nStartIndex].x)* (m - nStartIndex) / (nEndIndex - nStartIndex);
				ptEdge[m].y = ptEdge[nStartIndex].y + (ptEdge[nEndIndex].y - ptEdge[nStartIndex].y)* (m - nStartIndex) / (nEndIndex - nStartIndex);
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CSingleBlobInfo::InterpolationContour");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

CAngleLookupTable::CAngleLookupTable()
{
	int i;
	for(i=0;i<10000;i++) {
		dTanTab[i] = atan2((double) i, (double) 10000);
	}
}
CAngleLookupTable::~CAngleLookupTable()
{
	Clean();
}

void CAngleLookupTable::Clean()
{
}
int CAngleLookupTable::GetAngle(CPoint* pptOrg,
							   CPoint* pptDst,
							   double* pdAngle)
{
	int nx;
	int ny;
	int nAbsx;
	int nAbsy;
	double dAngle;
	double dAngWrap;


	*pdAngle = 0;
	nx = pptDst->x - pptOrg->x;
	ny = pptDst->y - pptOrg->y;
	nAbsx = abs(nx);
	nAbsy = abs(ny);
	if((nx * ny) == 0) {
		if((nx == 0) && (ny == 0)) 
			dAngle = 0;
		else {
			if(nx == 0) {
				if(ny > 0)
					dAngle = PI/2;
				else
					dAngle = 3*PI/2;
			}
			if(ny == 0) {
				if(nx > 0)
					dAngle = 0;
				else
					dAngle = PI;
			}
		}
	}
	else {
		if(nAbsx > nAbsy)
			dAngWrap = dTanTab[nAbsy *10000/nAbsx];
		else {
			dAngWrap = PI/2 - dTanTab[nAbsx *10000/nAbsy];
		}
		if((nx > 0) && (ny > 0))
			dAngle = dAngWrap;
		if((nx > 0) && (ny < 0))
			dAngle = 2*PI - dAngWrap;
		if((nx < 0) && (ny > 0))
			dAngle = PI - dAngWrap;
		if((nx < 0) && (ny < 0))
			dAngle = PI + dAngWrap;
	}
	*pdAngle = dAngle;
	return 0;
}


CPOLYGON_POINT_INFO::CPOLYGON_POINT_INFO()
{
}
CPOLYGON_POINT_INFO::~CPOLYGON_POINT_INFO()
{
}

CPOLYGON_POINT_INFO & CPOLYGON_POINT_INFO::operator =(CPOLYGON_POINT_INFO PolygonPointInfo)
{
	ptEdge = PolygonPointInfo.ptEdge;
	dDis = PolygonPointInfo.dDis;
	return *this;
}
CPOLYGON_LINE_INFO::CPOLYGON_LINE_INFO()
{
	int i;
	pPolygonPointInfo = NULL;
	nPolygonPointInfo = 0;
	dAngleStep = PI/180;
	for(i=0;i<=1000;i++) {
		nTanTab[i] = Round(atan2((double) i, (double) 1000)/dAngleStep);
	}
}

CPOLYGON_LINE_INFO::~CPOLYGON_LINE_INFO()
{
	Clean();
}

void CPOLYGON_LINE_INFO::Clean()
{
	if(pPolygonPointInfo != NULL)
		delete [] pPolygonPointInfo;
	pPolygonPointInfo = NULL;
	nPolygonPointInfo = 0;
}
int CPOLYGON_LINE_INFO::GetAngle(CPoint* pptOrg,
							   CPoint* pptDst,
							   int* pnAngle)
{
	int nx;
	int ny;
	int nAbsx;
	int nAbsy;
	int nAngleOffset;
	int nAngle;
	int nAngWrap;

	*pnAngle = 0;
	nx = pptDst->x - pptOrg->x;
	ny = pptDst->y - pptOrg->y;
	nAbsx = abs(nx);
	nAbsy = abs(ny);
	nAngleOffset = 0;
	if((nx * ny) == 0) {
		if((nx == 0) && (ny == 0)) 
			nAngle = 0;
		else {
			if(nx == 0) {
				if(ny > 0)
					nAngle = 90;
				else
					nAngle = 270;
			}
			if(ny == 0) {
				if(nx > 0)
					nAngle = 0;
				else
					nAngle = 180;
			}
		}
	}
	else {
		if(nAbsx > nAbsy)
			nAngWrap = nTanTab[nAbsy *1000/nAbsx];
		else {
			nAngWrap = 90 - nTanTab[nAbsx *1000/nAbsy];
		}
		if((nx > 0) && (ny > 0))
			nAngle = nAngWrap;
		if((nx > 0) && (ny < 0))
			nAngle = 360 - nAngWrap;
		if((nx < 0) && (ny > 0))
			nAngle = 180 - nAngWrap;
		if((nx < 0) && (ny < 0))
			nAngle = 180 + nAngWrap;
	}
	*pnAngle = nAngle;
	return 0;
}

int CPOLYGON_LINE_INFO::GetPolygonPt(int nMaxPolygonLength,
									 CPoint* pptPolygon)
{
	int i;
	CPoint ptStart;
	int nLft;
	int nTop;
	int nRht;
	int nBot;
	CPOLYGON_POINT_INFO* pPolygonPointInfoInROI;
	int nPolygonPointInfoInROI;
	CRect rcPolygon;
	int nSelectedIndex;
	CPOLYGON_POINT_INFO* pPolygonPointInfoBackup;
	int nPolygonPointInfoBackup;
	double dSizeX, dSizeY, dLength, dLengthTol;
	double dDisMax;
	BOOL bFound;

try
{
	pPolygonPointInfoInROI = NULL;
	pPolygonPointInfoBackup = NULL;
	if(nPolygonPointInfo <= 0)
		goto ErrorAbort;
	if(nPolygonPointInfo < nMaxPolygonLength) {
		Clean();
		*pptPolygon = pPolygonPointInfo[nPolygonPointInfo-1].ptEdge;
		goto PASS;
	}
	ptStart = pPolygonPointInfo[0].ptEdge;
	nLft = ptStart.x - nMaxPolygonLength;
	nTop = ptStart.y - nMaxPolygonLength;
	nRht = ptStart.x + nMaxPolygonLength;
	nBot = ptStart.y + nMaxPolygonLength;
	pPolygonPointInfoInROI = new CPOLYGON_POINT_INFO[abs(nPolygonPointInfo)];
	nPolygonPointInfoInROI = 0;
	for(i=0;i<nPolygonPointInfo;i++) {
		if((pPolygonPointInfo[i].ptEdge.x >= nLft) &&
		   (pPolygonPointInfo[i].ptEdge.y >= nTop) &&
		   (pPolygonPointInfo[i].ptEdge.x < nRht) &&
		   (pPolygonPointInfo[i].ptEdge.y < nBot))
		   pPolygonPointInfoInROI[nPolygonPointInfoInROI++] = pPolygonPointInfo[i];
	}
	if(nPolygonPointInfoInROI == 0) {
		Clean();
		*pptPolygon = pPolygonPointInfo[nPolygonPointInfo-1].ptEdge;
		goto PASS;
	}

	nLft = pPolygonPointInfo[0].ptEdge.x;
	nTop = pPolygonPointInfo[0].ptEdge.y;
	nRht = pPolygonPointInfo[0].ptEdge.x;
	nBot = pPolygonPointInfo[0].ptEdge.y;
	for(i=1;i<nPolygonPointInfoInROI;i++) {
		if(nLft > pPolygonPointInfo[i].ptEdge.x) 
			nLft = pPolygonPointInfo[i].ptEdge.x;
		if(nRht < pPolygonPointInfo[i].ptEdge.x) 
			nRht = pPolygonPointInfo[i].ptEdge.x;
		if(nTop > pPolygonPointInfo[i].ptEdge.y) 
			nTop = pPolygonPointInfo[i].ptEdge.y;
		if(nBot < pPolygonPointInfo[i].ptEdge.y) 
			nBot = pPolygonPointInfo[i].ptEdge.y;
	}

	dSizeX = (nRht - nLft)/4.0;
	dSizeY = (nBot - nTop)/4.0;
	if(dSizeX > dSizeY)
		dLengthTol = dSizeX;
	else
		dLengthTol = dSizeY;
	if(dLengthTol < 1)
		dLengthTol = 1;
	bFound = FALSE;
	for(i=1;i<nPolygonPointInfoInROI;i++) {
		dLength = abs(pPolygonPointInfo[i].ptEdge.x - ptStart.x) + abs(pPolygonPointInfo[i].ptEdge.y - ptStart.y);
		if(dLength < dLengthTol)
			continue;
		if(bFound == FALSE) {
			dDisMax = pPolygonPointInfo[i].dDis;
			nSelectedIndex = i;
			bFound = TRUE;
		}
		else {
			if(dDisMax < pPolygonPointInfo[i].dDis) {
				dDisMax = pPolygonPointInfo[i].dDis;
				nSelectedIndex = i;
			}
		}
	}
	if(bFound) {
		*pptPolygon = pPolygonPointInfo[nSelectedIndex].ptEdge;
		pPolygonPointInfoBackup = new CPOLYGON_POINT_INFO[abs(nPolygonPointInfo)];
		nPolygonPointInfoBackup = 0;
		for(i=nSelectedIndex;i<nPolygonPointInfo;i++) 
			pPolygonPointInfoBackup[nPolygonPointInfoBackup++] = pPolygonPointInfo[i];
		Clean();
		nPolygonPointInfo = nPolygonPointInfoBackup;
		if(nPolygonPointInfo <= 0)
			goto PASS;
		pPolygonPointInfo = new CPOLYGON_POINT_INFO[abs(nPolygonPointInfo)];
		for(i=0;i<nPolygonPointInfo;i++) 
			pPolygonPointInfo[i] = pPolygonPointInfoBackup[i];
	}
	else {
		if(nPolygonPointInfo > 0)
			*pptPolygon = pPolygonPointInfo[nPolygonPointInfo-1].ptEdge;
		Clean();
		goto PASS;
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CPOLYGON_LINE_INFO::GetPolygonPt");
	goto ErrorAbort;
}
PASS:
	if(pPolygonPointInfoInROI != NULL)
		delete [] pPolygonPointInfoInROI;
	if(pPolygonPointInfoBackup != NULL)
		delete [] pPolygonPointInfoBackup;
	return 0;
ErrorAbort:
	if(pPolygonPointInfoInROI != NULL)
		delete [] pPolygonPointInfoInROI;
	if(pPolygonPointInfoBackup != NULL)
		delete [] pPolygonPointInfoBackup;
	return -1;
}


int CPOLYGON_LINE_INFO::GetPolygonPt2(int nPreAngle,
									  int nMaxPolygonLength,
									  CPoint* pptPolygon)
{
	int i;
	CPoint ptStart;
	int nLft;
	int nTop;
	int nRht;
	int nBot;
	CPOLYGON_POINT_INFO* pPolygonPointInfoInROI;
	int nPolygonPointInfoInROI;
	CRect rcPolygon;
	int nSelectedIndex;
	CPOLYGON_POINT_INFO* pPolygonPointInfoBackup;
	int nPolygonPointInfoBackup;
	double dSizeX, dSizeY, dLength, dLengthTol;
	BOOL bFound;
	int* pnAngle;
	int nAngDif;
	int nMinAngDif;
	int nMinAngPos;
	int nError;

try
{
	pPolygonPointInfoInROI = NULL;
	pPolygonPointInfoBackup = NULL;
	pnAngle = NULL;
	if(nPolygonPointInfo <= 0)
		goto ErrorAbort;
	if(nPolygonPointInfo < nMaxPolygonLength) {
		*pptPolygon = pPolygonPointInfo[nPolygonPointInfo-1].ptEdge;
		Clean();
		goto PASS;
	}
	ptStart = pPolygonPointInfo[0].ptEdge;
	nLft = ptStart.x - nMaxPolygonLength;
	nTop = ptStart.y - nMaxPolygonLength;
	nRht = ptStart.x + nMaxPolygonLength;
	nBot = ptStart.y + nMaxPolygonLength;
	pPolygonPointInfoInROI = new CPOLYGON_POINT_INFO[abs(nPolygonPointInfo)];
	pnAngle = new int[abs(nPolygonPointInfo)];
	nPolygonPointInfoInROI = 0;
	for(i=0;i<nPolygonPointInfo;i++) {
		if((pPolygonPointInfo[i].ptEdge.x >= nLft) &&
		   (pPolygonPointInfo[i].ptEdge.y >= nTop) &&
		   (pPolygonPointInfo[i].ptEdge.x < nRht) &&
		   (pPolygonPointInfo[i].ptEdge.y < nBot))
		   pPolygonPointInfoInROI[nPolygonPointInfoInROI++] = pPolygonPointInfo[i];
	}
	if(nPolygonPointInfoInROI == 0) {
		Clean();
		*pptPolygon = pPolygonPointInfo[nPolygonPointInfo-1].ptEdge;
		goto PASS;
	}

	nLft = pPolygonPointInfo[0].ptEdge.x;
	nTop = pPolygonPointInfo[0].ptEdge.y;
	nRht = pPolygonPointInfo[0].ptEdge.x;
	nBot = pPolygonPointInfo[0].ptEdge.y;
	for(i=1;i<nPolygonPointInfoInROI;i++) {
		if(nLft > pPolygonPointInfo[i].ptEdge.x) 
			nLft = pPolygonPointInfo[i].ptEdge.x;
		if(nRht < pPolygonPointInfo[i].ptEdge.x) 
			nRht = pPolygonPointInfo[i].ptEdge.x;
		if(nTop > pPolygonPointInfo[i].ptEdge.y) 
			nTop = pPolygonPointInfo[i].ptEdge.y;
		if(nBot < pPolygonPointInfo[i].ptEdge.y) 
			nBot = pPolygonPointInfo[i].ptEdge.y;
	}

	dSizeX = (nRht - nLft)/4.0;
	dSizeY = (nBot - nTop)/4.0;
	if(dSizeX > dSizeY)
		dLengthTol = dSizeX;
	else
		dLengthTol = dSizeY;
	if(dLengthTol < 1)
		dLengthTol = 1;


	pnAngle[0] = -1;
	for(i=1;i<nPolygonPointInfoInROI;i++) {
		dLength = abs(pPolygonPointInfo[i].ptEdge.x - ptStart.x) + abs(pPolygonPointInfo[i].ptEdge.y - ptStart.y);
		if(dLength < dLengthTol) {
			pnAngle[i] = -1;
			continue;
		}
		nError = GetAngle(&ptStart,
				   &pPolygonPointInfo[i].ptEdge,
				   &pnAngle[i]);
		if(nError)
			pnAngle[i] = -1;
	}

	nMinAngDif = 360;
	nMinAngPos = 0;
	bFound = FALSE;
	for(i=1;i<nPolygonPointInfoInROI;i++) {
		if(pnAngle[i] < 0)
			continue;
		bFound = TRUE;
		nAngDif = nPreAngle - pnAngle[i];
		if(nAngDif < 0)
			nAngDif += 360;
		if(nMinAngDif > nAngDif) {
			nMinAngDif = nAngDif;
			nMinAngPos = i;
		}
	}
	nSelectedIndex = nMinAngPos;
	if(bFound) {
		nFoundAngle = pnAngle[nMinAngPos];
		*pptPolygon = pPolygonPointInfo[nSelectedIndex].ptEdge;
		pPolygonPointInfoBackup = new CPOLYGON_POINT_INFO[abs(nPolygonPointInfo + nMaxPolygonLength)];
		if((nPolygonPointInfo - nSelectedIndex) > nMaxPolygonLength) {
			nPolygonPointInfoBackup = 0;
			for(i=nSelectedIndex;i<nPolygonPointInfo;i++) 
				pPolygonPointInfoBackup[nPolygonPointInfoBackup++] = pPolygonPointInfo[i];
		}
		else {
			for(i=0;i<nMaxPolygonLength;i++) 
				pPolygonPointInfoBackup[i] = pPolygonPointInfo[nPolygonPointInfo-1];
			nPolygonPointInfoBackup = 0;
			for(i=nSelectedIndex;i<nPolygonPointInfo;i++) 
				pPolygonPointInfoBackup[nPolygonPointInfoBackup++] = pPolygonPointInfo[i];
			nPolygonPointInfoBackup = nMaxPolygonLength;
		}
		Clean();
		nPolygonPointInfo = nPolygonPointInfoBackup;
		if(nPolygonPointInfo <= 0)
			goto PASS;
		pPolygonPointInfo = new CPOLYGON_POINT_INFO[abs(nPolygonPointInfo)];
		for(i=0;i<nPolygonPointInfo;i++) 
			pPolygonPointInfo[i] = pPolygonPointInfoBackup[i];
	}
	else {
		if(nPolygonPointInfo > 0)
			*pptPolygon = pPolygonPointInfo[nPolygonPointInfo-1].ptEdge;
		Clean();
		goto PASS;
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CPOLYGON_LINE_INFO::GetPolygonPt2");
	goto ErrorAbort;
}
PASS:
	if(pPolygonPointInfoInROI != NULL)
		delete [] pPolygonPointInfoInROI;
	if(pPolygonPointInfoBackup != NULL)
		delete [] pPolygonPointInfoBackup;
	if(pnAngle != NULL)
		delete [] pnAngle;
	return 0;
ErrorAbort:
	if(pPolygonPointInfoInROI != NULL)
		delete [] pPolygonPointInfoInROI;
	if(pPolygonPointInfoBackup != NULL)
		delete [] pPolygonPointInfoBackup;
	if(pnAngle != NULL)
		delete [] pnAngle;
	return -1;
}


int CPOLYGON_LINE_INFO::GetData(CPoint* pptEdge,
								int nEdgeNo,
								int nStartIndex,
								int nEndIndex,
								CPoint* pptReference)
{
	int i;
	int nIndex;
	CLine2D ln2DRef;
	double dRef;
try
{
	Clean();
	if((nStartIndex > nEdgeNo) ||
		(nEndIndex > nEdgeNo) ||
		(nStartIndex < 0) ||
		(nEndIndex < 0))
		goto ErrorAbort;

	ln2DRef = CLine2D(pptEdge[nStartIndex], pptEdge[nEndIndex]);
	if(ln2DRef.IsInValid()) {
		goto PASS;
	}
 	if(nEndIndex > nStartIndex) {
		nPolygonPointInfo = nEndIndex - nStartIndex;
	}
	else {
		nPolygonPointInfo = nEdgeNo - nStartIndex + nEndIndex;
	}
	if(nPolygonPointInfo < 0)
		goto ErrorAbort;
	pPolygonPointInfo = new CPOLYGON_POINT_INFO[abs(nPolygonPointInfo)];
	nIndex = nStartIndex;
	ln2DRef.GetSignedDistance(pptReference, &dRef);
	for(i=0;i<nPolygonPointInfo;i++) {
		if(nIndex >= nEdgeNo)
			nIndex -= nEdgeNo;
		if((nIndex < 0) || (nIndex >= nEdgeNo))
			goto ErrorAbort;
		pPolygonPointInfo[i].ptEdge = pptEdge[nIndex];
		ln2DRef.GetSignedDistance(&pptEdge[nIndex], &pPolygonPointInfo[i].dDis);
		if(dRef < 0) 
			pPolygonPointInfo[i].dDis = -pPolygonPointInfo[i].dDis;
		nIndex++;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CPOLYGON_LINE_INFO::GetData");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	Clean();
	return -1;
}


CPOLYGON_LINE::CPOLYGON_LINE()
{
}
CPOLYGON_LINE::~CPOLYGON_LINE()
{
}
CPOLYGON_LINE & CPOLYGON_LINE::operator =(CPOLYGON_LINE PolygonLine)
{
	pt0 = PolygonLine.pt0;
	pt1 = PolygonLine.pt1;
	return *this;
}


CPOLYGON::CPOLYGON()
{
	pPolygonLine = NULL;
	nPolygonLine = 0;
}
CPOLYGON::~CPOLYGON()
{
	Clean();
}
void CPOLYGON::Clean()
{
	if(pPolygonLine != NULL)
		delete [] pPolygonLine;
	pPolygonLine = NULL;
	nPolygonLine = 0;
	ptPolygonArray.RemoveAll();
}
int CPOLYGON::GetPolygonInfo(CPoint* pptEdge,
							 int nEdgeNo)
{
	int i;
	int nLft;
	int nTop;
	int nRht;
	int nBot;
	int nLftMostIndex;
	int nTopMostIndex;
	int nRhtMostIndex;
	int nBotMostIndex;
	int nStartIndex[4];
	int nEndIndex[4];
try
{
	if(nEdgeNo <= 3)
		goto PASS;
	nLft = pptEdge[0].x;
	nTop = pptEdge[0].y;
	nRht = pptEdge[0].x;
	nBot = pptEdge[0].y;
	ptLftMost = pptEdge[0];
	ptTopMost = pptEdge[0];
	ptRhtMost = pptEdge[0];
	ptBotMost = pptEdge[0];
	nLftMostIndex = 0;
	nTopMostIndex = 0;
	nRhtMostIndex = 0;
	nBotMostIndex = 0;
	for(i=1;i<nEdgeNo;i++) {
		if(nLft > pptEdge[i].x) {
			nLft = pptEdge[i].x;
			ptLftMost = pptEdge[i];
			nLftMostIndex = i;
		}
		if(nTop > pptEdge[i].y) {
			nTop = pptEdge[i].y;
			ptTopMost = pptEdge[i];
			nTopMostIndex = i;
		}
		if(nRht < pptEdge[i].x) {
			nRht = pptEdge[i].x;
			ptRhtMost = pptEdge[i];
			nRhtMostIndex = i;
		}
		if(nBot < pptEdge[i].y) {
			nBot = pptEdge[i].y;
			ptBotMost = pptEdge[i];
			nBotMostIndex = i;
		}
	}
	ptTopLft = CPoint(ptLftMost.x, ptTopMost.y);
	ptTopRht = CPoint(ptRhtMost.x, ptTopMost.y);
	ptBotLft = CPoint(ptLftMost.x, ptBotMost.y);
	ptBotRht = CPoint(ptRhtMost.x, ptBotMost.y);

	ptReference[0] = CPoint(nLft - 1, nTop - 1);
	ptReference[1] = CPoint(nLft - 1, nBot + 1);
	ptReference[2] = CPoint(nRht + 1, nBot + 1);
	ptReference[3] = CPoint(nRht + 1, nTop - 1);

	nStartIndex[0] = nTopMostIndex;
	nEndIndex[0] = nLftMostIndex;
	nStartIndex[1] = nLftMostIndex;
	nEndIndex[1] = nBotMostIndex;
	nStartIndex[2] = nBotMostIndex;
	nEndIndex[2] = nRhtMostIndex;
	nStartIndex[3] = nRhtMostIndex;
	nEndIndex[3] = nTopMostIndex;
	for(i=0;i<4;i++) {
		PolygonLineinfo[i].GetData(&pptEdge[0],
								   nEdgeNo,
								   nStartIndex[i],
								   nEndIndex[i],
								   &ptReference[i]);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CPOLYGON::GetPolygonInfo");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

int CPOLYGON::GetPolygonLine(CPoint* pptEdge,
							 int nEdgeNo,
							 int nMaxPolygonLength)
{
	int i;
	CPoint ptPolygon;
	int nError;
	int nPreAngle;

try
{
	ptPolygonArray.RemoveAll();
	nError = GetPolygonInfo(&pptEdge[0],
							nEdgeNo);
	if(nError < 0)
		goto ErrorAbort;
	nPreAngle = 270;
	for(i=0;i<4;i++) {
		if(PolygonLineinfo[i].nPolygonPointInfo > 0)
			ptPolygonArray.Add(PolygonLineinfo[i].pPolygonPointInfo[0].ptEdge);
		do {
			nError = PolygonLineinfo[i].GetPolygonPt2(nPreAngle,
											nMaxPolygonLength,
											&ptPolygon);
			if(nError)
				break;
			ptPolygonArray.Add(ptPolygon);
			nPreAngle = PolygonLineinfo[i].nFoundAngle + 180;
			if(nPreAngle > 360)
				nPreAngle -= 360;
		} while(PolygonLineinfo[i].nPolygonPointInfo >= nMaxPolygonLength);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CPOLYGON::GetPolygonLine");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int ImgWrapCopy(CImageBuffer *pImgSrc,
				CRect* prcSrc,
				CImageBuffer* pImgDst,
				CRect* prcFOVDst,
				CRect* prcDst)
{
	CRect rcOverlapSrc;
	CRect rcOverlapDst;
	CRect rcExtSrc;
	CRect rcExtDst;
	int nError;
	int nOverlap;

try
{
	if(RectInRect(*prcFOVDst, *prcDst)) {
		nError = ImgCopy(pImgSrc,
						prcSrc,
						pImgDst,
						prcDst);
		if(nError)
			goto ErrorAbort;
	}
	else {
		nOverlap = RectOverlapRect(*prcFOVDst, 
							*prcDst,
							&rcOverlapDst);
		if(nOverlap == 0)
			goto ErrorAbort;
		rcOverlapSrc = rcOverlapDst + prcSrc->TopLeft() - prcDst->TopLeft();
		nError = ImgCopy(pImgSrc,
						&rcOverlapSrc,
						pImgDst,
						&rcOverlapDst);
		if(nError)
			goto ErrorAbort;
		if(prcDst->left < prcFOVDst->left) {
			rcExtDst = CRect(prcFOVDst->right - abs(prcFOVDst->left - prcDst->left),
							 prcDst->top,
							 prcFOVDst->right,
							 prcDst->bottom);
			rcExtSrc = CRect(prcSrc->TopLeft(), rcExtDst.Size());
			nError = ImgCopy(pImgSrc,
							&rcExtSrc,
							pImgDst,
							&rcExtDst);
			if(nError)
				goto ErrorAbort;
		}
		if(prcDst->right > prcFOVDst->right) {
			rcExtDst = CRect(prcFOVDst->left,
							 prcDst->top,
							 prcFOVDst->left + abs(prcDst->right - prcFOVDst->right),
							 prcDst->bottom);
			rcExtSrc = CRect(CPoint(prcSrc->right - rcExtDst.Width() - 1, prcSrc->top), rcExtDst.Size());
			nError = ImgCopy(pImgSrc,
							&rcExtSrc,
							pImgDst,
							&rcExtDst);
			if(nError)
				goto ErrorAbort;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgWrapCopy");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int ImgWrapCopy(CImageBuffer *pImgSrc,
				CRect* prcFOVSrc,
				CRect* prcSrc,
				CImageBuffer* pImgDst,
				CRect* prcDst)
{
	CRect rcOverlapSrc;
	CRect rcOverlapDst;
	CRect rcExtSrc;
	CRect rcExtDst;
	int nError;
	int nOverlap;

try
{
	if(RectInRect(*prcFOVSrc, *prcSrc)) {
		nError = ImgCopy(pImgSrc,
						prcSrc,
						pImgDst,
						prcDst);
		if(nError)
			goto ErrorAbort;
	}
	else {
		nOverlap = RectOverlapRect(*prcFOVSrc, 
							*prcSrc,
							&rcOverlapSrc);
		if(nOverlap == 0)
			goto ErrorAbort;
		rcOverlapDst = rcOverlapSrc + prcDst->TopLeft() - prcSrc->TopLeft();
		nError = ImgCopy(pImgSrc,
						&rcOverlapSrc,
						pImgDst,
						&rcOverlapDst);
		if(nError)
			goto ErrorAbort;
		if(prcSrc->left < prcFOVSrc->left) {
			rcExtSrc = CRect(prcFOVSrc->right - abs(prcFOVSrc->left - prcSrc->left),
							 prcSrc->top,
							 prcFOVSrc->right,
							 prcSrc->bottom);
			rcExtDst = CRect(prcDst->TopLeft(), rcExtSrc.Size());
			nError = ImgCopy(pImgSrc,
							&rcExtSrc,
							pImgDst,
							&rcExtDst);
			if(nError)
				goto ErrorAbort;
		}
		if(prcSrc->right > prcFOVSrc->right) {
			rcExtSrc = CRect(prcFOVSrc->left,
							 prcSrc->top,
							 prcFOVSrc->left + abs(prcSrc->right - prcFOVSrc->right),
							 prcSrc->bottom);
			//rcExtDst = CRect(CPoint(prcDst->right - rcExtSrc.Width() - 1, prcDst->top), rcExtSrc.Size());
			rcExtDst = CRect(CPoint(prcDst->right - rcExtSrc.Width(), prcDst->top), rcExtSrc.Size());
			nError = ImgCopy(pImgSrc,
							&rcExtSrc,
							pImgDst,
							&rcExtDst);
			if(nError)
				goto ErrorAbort;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgWrapCopy");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int ImgSetPixelValAry(CImageBuffer* pImgSrc,
					CRect* prcSrc,
					CImageBuffer* pImgDst,
					CRect* prcDst,
					CPoint2D* ppt2DAry, 
					int nPtAryNo,
					BYTE byteVale)
{
	int i;
	int nx, ny;
	int nError;
try
{
	nError = ImgCopy(pImgSrc,
					prcSrc,
					pImgDst,
					prcDst);
	if(nError)
		goto ErrorAbort;
	for(i=0;i<nPtAryNo;i++) {
		nx = Round(ppt2DAry[i].x);
		ny = Round(ppt2DAry[i].y);
		pImgDst->SetPixelVal(nx, ny, byteVale);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgSetPixelValAry");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int ImgDebugADD(CImageBuffer* pImgSrc0,
				CRect* prcSrc0,
				BYTE byteDivVale0,
				CImageBuffer* pImgSrc1,
				CRect* prcSrc1,
				BYTE byteDivVale1,
				CImageBuffer* pImgDst,
				CRect* prcDst)
{
	CImageBuffer Img0;
	CImageBuffer Img1;
	int nError;
try
{
	Img0.Allocate(AbsSize(prcSrc0->Size()));
	Img1.Allocate(AbsSize(prcSrc0->Size()));
	nError = ImgCopy(pImgSrc0,
					 prcSrc0,
					 &Img0,
					 prcSrc0);
	if(nError)
		goto ErrorAbort;
	nError = ImgDIV(&Img0, prcSrc0, byteDivVale0);
	if(nError)
		goto ErrorAbort;
	nError = ImgCopy(pImgSrc1,
					 prcSrc1,
					 &Img1,
					 prcSrc1);
	if(nError)
		goto ErrorAbort;
	nError = ImgDIV(&Img1, prcSrc1, byteDivVale1);
	if(nError)
		goto ErrorAbort;

	nError = ImgADD(&Img0,
					prcSrc0,
					&Img1,
					prcSrc1,
					pImgDst,
					prcDst);
	if(nError)
		goto ErrorAbort;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgDebugADD");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int ImgDIV(CImageBuffer *pImg, CRect *prectROI, BYTE byteVal)
{
	int x,y;
	int nImageWidth,nImageHeight,nImagePitch;
	int nWidth,nHeight;
	BYTE *pRow,*pCol;

try
{
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
	if(byteVal <= 0)
		return -1;
	nImageWidth=pImg->GetImgSize().cx;
	nImageHeight=pImg->GetImgSize().cy;
	nImagePitch=pImg->GetPitch();
	nWidth=prectROI->Width();
	nHeight=prectROI->Height();
	pRow=pCol=pImg->GetImgPtr()+nImagePitch*prectROI->top+prectROI->left;
	for (y=0;y<nHeight;y++,pRow+=nImagePitch) {
		for (x=0,pCol=pRow;x<nWidth;x++,pCol++) {
			*pCol = *pCol/byteVal;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgDIV");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}



int GetHorLineCenterPoints(CImageBuffer* pImg,
						  CRect* prc,
						  CRect* prcROI,
						  CPoint2D** pppt2D,
						  int* pnNo)
{
	int x,y;
	int nx;
	int nRelativeX;
	int nImgPitch;
	int nWidth,nHeight;
	BYTE *pImgPtr;
	int nStart;
	int nEnd;
	BOOL bFound;
	int nHeightSub1;
	long lIndex;

try
{
	for (x=0; x<prcROI->Width(); x++) 
		pnNo[x] = 0;
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
	pImgPtr = pImg->GetImgPtr();
	nRelativeX = 0;
	nHeightSub1 = nHeight - 1;
	for (x=prcROI->left; x<prcROI->right; x++) {
		nx = x;
		if(nx < 0)
			nx = nx + nWidth;
		if(nx >= nWidth)
			nx = nx - nWidth;
		lIndex = nImgPitch * prc->top + prc->left + nx;
		bFound = FALSE;
		for (y=0; y<nHeight; y++) {
			if(pImgPtr[lIndex] == 255) {
				if(bFound == FALSE) { 
					nStart = y;
					bFound = TRUE;
				}
				nEnd = y;
				if((bFound == TRUE) && (y == nHeightSub1)) {
					pppt2D[nRelativeX][pnNo[nRelativeX]++] = CPoint2D(nx,(nStart + nEnd)/2.0+prc->top);
					bFound = FALSE;
				}
			}
			else {
				if(bFound == TRUE) {
					pppt2D[nRelativeX][pnNo[nRelativeX]++] = CPoint2D(nx,(nStart + nEnd)/2.0+prc->top);
					bFound = FALSE;
				}
			}
			lIndex += nImgPitch;
		}
		nRelativeX++;
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetHorLineCenterPoints");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int GetVerLineCenterPoints2(CImageBuffer *pImg,
						  CRect *prc,
						  CPoint2D** pppt2D,
						  int* pnNo)
{
	int x,y;
	int nx;
	int nRelativeX;
	int nImgPitch;
	int nWidth,nHeight;
	BYTE *pImgPtr;
	int nStart;
	int nEnd;
	BOOL bFound;
	long lIndex;

try
{
	for (y=0; y<prc->Height(); y++) 
		pnNo[y] = 0;
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
	pImgPtr = pImg->GetImgPtr();
	nRelativeX = 0;
	lIndex = nImgPitch * prc->top + prc->left;
	for (y=0; y<nHeight; y++) {
		bFound = FALSE;
		for (x=prc->left; x<prc->right; x++) {
			nx = x;
			if(pImgPtr[lIndex+nx] == 255) {
				if(bFound == FALSE) { 
					nStart = nx;
					bFound = TRUE;
				}
				nEnd = nx;
			}
			else {
				if(bFound == TRUE) {
					pppt2D[nRelativeX][pnNo[nRelativeX]++] = CPoint2D((nStart + nEnd)/2.0,y+prc->top);
					bFound = FALSE;
				}
			}
		}
		nRelativeX++;
		lIndex += nImgPitch;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetVerLineCenterPoints2");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int GetHorLineCenterPoints(CImageBuffer *pImg,
						  CRect *prc,
						  CPoint2D* ppt2D,
						  int* pnNo)
{
	int x,y;
	int nImgPitch;
	int nWidth,nHeight;
	BYTE *pImgPtr;
	int nStart;
	int nEnd;
	BOOL bFound;
	long lIndex;
	int nNo;

try
{
	*pnNo = 0;
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
	pImgPtr = pImg->GetImgPtr();
	nNo = 0;
	for (x=0; x<nWidth; x++) {
		lIndex = nImgPitch * prc->top + prc->left + x;
		bFound = FALSE;
		for (y=0; y<nHeight; y++) {
			if(pImgPtr[lIndex] == 255) {
				if(bFound == FALSE) { 
					nStart = y;
					bFound = TRUE;
				}
				nEnd = y;
			}
			else {
				if(bFound == TRUE) {
					ppt2D[nNo++] = CPoint2D(x+prc->left,(nStart + nEnd)/2.0+prc->top);
					bFound = FALSE;
				}
			}
			lIndex += nImgPitch;
		}
	}
	*pnNo = nNo;

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetHorLineCenterPoints");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int GetVerLineCenterPoints(CImageBuffer *pImg,
						  CRect *prc,
						  CPoint2D* ppt2D,
						  int* pnNo)
{
	int x,y;
	int nImgPitch;
	int nWidth,nHeight;
	BYTE *pImgPtr;
	int nStart;
	int nEnd;
	BOOL bFound;
	long lIndex;
	int nNo;

try
{
	*pnNo = 0;
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
	pImgPtr = pImg->GetImgPtr();
	nNo = 0;
	lIndex = nImgPitch * prc->top + prc->left;
	for (y=0; y<nHeight; y++) {
		bFound = FALSE;
		for (x=0; x<nWidth; x++) {
			if(pImgPtr[lIndex+x] == 255) {
				if(bFound == FALSE) { 
					nStart = x;
					bFound = TRUE;
				}
				nEnd = x;
			}
			else {
				if(bFound == TRUE) {
					ppt2D[nNo++] = CPoint2D((nStart + nEnd)/2.0+prc->left,y+prc->top);
					bFound = FALSE;
				}
			}
		}
		lIndex += nImgPitch;
	}
	*pnNo = nNo;

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"GetVerLineCenterPoints");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int ImgDIV(CImageBuffer *pImgSrc,
			CRect *prcSrc,
			int nDivVal,
			CImageBuffer *pImgDst,
			CRect *prcDst)
{
	int x,y;
	int nImgPitchSrc,nImgPitchDst;
	int nWidthSrc,nHeightSrc,nWidthDst;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;

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
	if(nDivVal <= 0) {
		ASSERT(!"nDivVal is Invalid");
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
			*pColDst = *pColSrc/nDivVal;
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgDIV");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int ImgLine(CImageBuffer* pImgBufBin,
			CRect* prcBin,
			CPoint* pptStart,
			CPoint* pptEnd,
			BYTE byteVale)
{
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
	int nChangedNo;
	CPoint ptBak;
	CPoint ptTemp;

try {	
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
//		ASSERT(!"pptStart is Invalid");
		return -1;
	}
	if (ValidatePoint(pptEnd, prcBin)) {
//		ASSERT(!"pptEnd is Invalid");
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
	nChangedNo = 0;
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
				ptBak = CPoint(int(x), int(y));
				ptTemp = CPoint(ptBak.x-1, ptBak.y);
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					lIndex = ptTemp.y * nPitch + ptTemp.x;
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
					}
				}
				ptTemp = CPoint(ptBak.x+1, ptBak.y);
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					lIndex = ptTemp.y * nPitch + ptTemp.x;
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
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
			ptBak = CPoint(int(x), int(y));
			ptTemp = CPoint(ptBak.x-1, ptBak.y);
			if(ValidatePoint(&ptTemp, prcBin) == 0) {
				lIndex = ptTemp.y * nPitch + ptTemp.x;
				if(pImgPtr[lIndex] != byteVale) {
					pImgPtr[lIndex] = byteVale;
				}
			}
			ptTemp = CPoint(ptBak.x+1, ptBak.y);
			if(ValidatePoint(&ptTemp, prcBin) == 0) {
				lIndex = ptTemp.y * nPitch + ptTemp.x;
				if(pImgPtr[lIndex] != byteVale) {
					pImgPtr[lIndex] = byteVale;
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
				ptBak = CPoint(int(x), int(y));
				ptTemp = CPoint(ptBak.x, ptBak.y - 1);
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					lIndex = ptTemp.y * nPitch + ptTemp.x;
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
					}
				}
				ptTemp = CPoint(ptBak.x, ptBak.y + 1);
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					lIndex = ptTemp.y * nPitch + ptTemp.x;
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
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
			ptBak = CPoint(int(x), int(y));
			ptTemp = CPoint(ptBak.x, ptBak.y - 1);
			if(ValidatePoint(&ptTemp, prcBin) == 0) {
				lIndex = ptTemp.y * nPitch + ptTemp.x;
				if(pImgPtr[lIndex] != byteVale) {
					pImgPtr[lIndex] = byteVale;
				}
			}
			ptTemp = CPoint(ptBak.x, ptBak.y + 1);
			if(ValidatePoint(&ptTemp, prcBin) == 0) {
				lIndex = ptTemp.y * nPitch + ptTemp.x;
				if(pImgPtr[lIndex] != byteVale) {
					pImgPtr[lIndex] = byteVale;
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
				ptBak = CPoint(int(x), int(y));
				ptTemp = CPoint(ptBak.x-1, ptBak.y);
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					lIndex = ptTemp.y * nPitch + ptTemp.x;
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
					}
				}
				ptTemp = CPoint(ptBak.x+1, ptBak.y);
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					lIndex = ptTemp.y * nPitch + ptTemp.x;
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
					}
				}
				y += dStepY;
				x += dStepX;
			}while((int)y != ptEndInROI.y); 
			lIndex=int(y) * nPitch + int(x);
			if(pImgPtr[lIndex] != byteVale) {
				pImgPtr[lIndex] = byteVale;
			}
			ptBak = CPoint(int(x), int(y));
			ptTemp = CPoint(ptBak.x-1, ptBak.y);
			if(ValidatePoint(&ptTemp, prcBin) == 0) {
				lIndex = ptTemp.y * nPitch + ptTemp.x;
				if(pImgPtr[lIndex] != byteVale) {
					pImgPtr[lIndex] = byteVale;
				}
			}
			ptTemp = CPoint(ptBak.x+1, ptBak.y);
			if(ValidatePoint(&ptTemp, prcBin) == 0) {
				lIndex = ptTemp.y * nPitch + ptTemp.x;
				if(pImgPtr[lIndex] != byteVale) {
					pImgPtr[lIndex] = byteVale;
				}
			}
		}
		else {
			do	{
				lIndex = int(y) * nPitch + int(x);
				if(pImgPtr[lIndex] != byteVale) {
					pImgPtr[lIndex] = byteVale;
				}
				ptBak = CPoint(int(x), int(y));
				ptTemp = CPoint(ptBak.x, ptBak.y - 1);
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					lIndex = ptTemp.y * nPitch + ptTemp.x;
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
					}
				}
				ptTemp = CPoint(ptBak.x, ptBak.y + 1);
				if(ValidatePoint(&ptTemp, prcBin) == 0) {
					lIndex = ptTemp.y * nPitch + ptTemp.x;
					if(pImgPtr[lIndex] != byteVale) {
						pImgPtr[lIndex] = byteVale;
					}
				}
				y += dStepY;
				x += dStepX;
			}while((int)x != ptEndInROI.x);
			lIndex = int(y) * nPitch + int(x);
			if(pImgPtr[lIndex] != byteVale) {
				pImgPtr[lIndex] = byteVale;
			}
			ptBak = CPoint(int(x), int(y));
			ptTemp = CPoint(ptBak.x, ptBak.y - 1);
			if(ValidatePoint(&ptTemp, prcBin) == 0) {
				lIndex = ptTemp.y * nPitch + ptTemp.x;
				if(pImgPtr[lIndex] != byteVale) {
					pImgPtr[lIndex] = byteVale;
				}
			}
			ptTemp = CPoint(ptBak.x, ptBak.y + 1);
			if(ValidatePoint(&ptTemp, prcBin) == 0) {
				lIndex = ptTemp.y * nPitch + ptTemp.x;
				if(pImgPtr[lIndex] != byteVale) {
					pImgPtr[lIndex] = byteVale;
				}
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgLine");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

CData1DEdgeDetect::CData1DEdgeDetect()
{
}

CData1DEdgeDetect::~CData1DEdgeDetect()
{
	Clean();
}

void CData1DEdgeDetect::Clean()
{
	m_EdgeFindRslt.nFndPOSEdge = 0;
	if(m_EdgeFindRslt.pdFndPOSEdge) {
		delete [] m_EdgeFindRslt.pdFndPOSEdge;
		m_EdgeFindRslt.pdFndPOSEdge = NULL;
	}
	m_EdgeFindRslt.nFndNEGEdge = 0;
	if(m_EdgeFindRslt.pdFndNEGEdge) {
		delete [] m_EdgeFindRslt.pdFndNEGEdge;
		m_EdgeFindRslt.pdFndNEGEdge = NULL;
	}

}

int CData1DEdgeDetect::GetEdge(int* pnDat,
			 int nDatNo,
			 int nEdgeType,
			 int nContrast,
			 int nGradient)
{
	int i;
	double* pdDat;
	int nError;

try
{
	pdDat = NULL;
	Clean();
	if(nDatNo <= 3)
		goto ErrorAbort;
	pdDat = new double[abs(nDatNo)];
	for(i=0;i<nDatNo;i++)
		pdDat[i] = pnDat[i];
	nError = GetEdge(&pdDat[0],
			 nDatNo,
			 nEdgeType,
			 nContrast,
			 nGradient);
	if(nError)
		goto ErrorAbort;

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CData1DEdgeDetect::GetEdge");
	goto ErrorAbort;
}
	if(pdDat != NULL)
		delete [] pdDat;
	return 0;
ErrorAbort:
	if(pdDat != NULL)
		delete [] pdDat;
	return -1;
}

int CData1DEdgeDetect::GetEdge(long* plDat,
			 int nDatNo,
			 int nEdgeType,
			 int nContrast,
			 int nGradient)
{
	int i;
	double* pdDat;
	int nError;

try
{
	pdDat = NULL;
	Clean();
	if(nDatNo <= 3)
		goto ErrorAbort;
	pdDat = new double[abs(nDatNo)];
	for(i=0;i<nDatNo;i++)
		pdDat[i] = plDat[i];
	nError = GetEdge(&pdDat[0],
			 nDatNo,
			 nEdgeType,
			 nContrast,
			 nGradient);
	if(nError)
		goto ErrorAbort;

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CData1DEdgeDetect::GetEdge");
	goto ErrorAbort;
}
	if(pdDat != NULL)
		delete [] pdDat;
	return 0;
ErrorAbort:
	if(pdDat != NULL)
		delete [] pdDat;
	return -1;
}


int CData1DEdgeDetect::GetEdge(double* pdDat,
						 int nDatNo,
						 int nEdgeType,
						 int nContrast,
						 int nGradient)
{
	PROJ_PARM ProjParm;		
	// Get Projection
	int i;
	int nProjSize,nError;
	PROJ_RSLT ProjRsltOrg;
	BOOL bEdgePositive;
	int nFndEdge;
	EDGE_CONTRAST_INFO* pEdgeContrastInfo;
	int nFndContrastEdge;
	long lProcContrastPOS, lProcContrastNEG;
	long* plFirstGradVal;
	long* plSecondGradVal;

try
{
	Clean();
	pEdgeContrastInfo = NULL;
	plFirstGradVal = NULL;
	plSecondGradVal = NULL;
	m_nEdgeType = nEdgeType;
	EdgeGeneralParm.nStartPos = 0;
	EdgeGeneralParm.nGradient = nGradient;
	EdgeGeneralParm.nDirection = FORWARD;
	EdgeGeneralParm.nWhiteCount = 1;
	EdgeGeneralParm.nBlackCount = 1;
	EdgeGeneralParm.nContrast = nContrast;
	nError = CreateProjRslt(&m_Projection.m_ProjRslt,
							&pdDat[0],
							nDatNo);
	if (nError)
		return -1;
	m_Projection.CopyProjRslt(&m_Projection.m_ProjRslt,&ProjRsltOrg);
	nProjSize = m_Projection.m_ProjRslt.nProjSize;
	if (nProjSize<=4) 
		return -1;
	pEdgeContrastInfo = new EDGE_CONTRAST_INFO[abs(m_Projection.m_ProjRslt.nProjSize)];
	plFirstGradVal = new long[abs(m_Projection.m_ProjRslt.nProjSize)];
	plSecondGradVal = new long[abs(m_Projection.m_ProjRslt.nProjSize)];
	lProcContrastPOS = nContrast * 10;
	lProcContrastNEG = -lProcContrastPOS;
	ScanEdgeContrast(&EdgeGeneralParm,
					 &m_Projection.m_ProjRslt,
					 plFirstGradVal,
					 plSecondGradVal,
					 lProcContrastPOS,
					 lProcContrastNEG,
					 pEdgeContrastInfo,
					 nFndContrastEdge);

	m_EdgeFindRslt.pdFndPOSEdge = new double [abs(nFndContrastEdge)];
	m_EdgeFindRslt.pdFndNEGEdge = new double [abs(nFndContrastEdge)];
	GetGradientEdge(&m_Projection.m_ProjRslt,
					pEdgeContrastInfo,
					nFndContrastEdge,
					plSecondGradVal,
					&EdgeGeneralParm);
	if (nGradient==POS||nGradient==ANY) {
		bEdgePositive = TRUE;
		nFndEdge = 0;
		for(i = 0; i < m_EdgeFindRslt.nFndPOSEdge; i++) {
			m_EdgeFindRslt.pdFndPOSEdge[nFndEdge++] = m_EdgeFindRslt.pdFndPOSEdge[i];
		}
		m_EdgeFindRslt.nFndPOSEdge = nFndEdge;
	}
	if (nGradient==NEG||nGradient==ANY) {
		bEdgePositive = FALSE;
		nFndEdge = 0;
		for(i = 0; i < m_EdgeFindRslt.nFndNEGEdge; i++) {
			m_EdgeFindRslt.pdFndNEGEdge[nFndEdge++] = m_EdgeFindRslt.pdFndNEGEdge[i];
		}
		m_EdgeFindRslt.nFndNEGEdge = nFndEdge;
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CData1DEdgeDetect::GetEdge");
	goto ErrorAbort;
}
	if(pEdgeContrastInfo != NULL)
		delete [] pEdgeContrastInfo;
	if(plFirstGradVal != NULL)
		delete [] plFirstGradVal;
	if(plSecondGradVal != NULL)
		delete [] plSecondGradVal;
	return 0;
ErrorAbort:
	if(pEdgeContrastInfo != NULL)
		delete [] pEdgeContrastInfo;
	if(plFirstGradVal != NULL)
		delete [] plFirstGradVal;
	if(plSecondGradVal != NULL)
		delete [] plSecondGradVal;
	return -1;
}

int CData1DEdgeDetect::CreateProjRslt(PROJ_RSLT* pProjRsltSrc,
							  double* pdDat,
							  int nDatNo)
{
	int i;
	long lMaxValue;
	long lMinValue;

try
{
	pProjRsltSrc->Clean();
	if(nDatNo < 3)
		goto ErrorAbort;
	pProjRsltSrc->nProjSize = nDatNo;
	pProjRsltSrc->plProjVal = new long [abs(nDatNo)];
	for(i = 0; i < pProjRsltSrc->nProjSize; i++)
		pProjRsltSrc->plProjVal[i] = Round(10 * pdDat[i]);

	lMaxValue = pProjRsltSrc->plProjVal[0];
	lMinValue = pProjRsltSrc->plProjVal[0];
	for(i=1;i<nDatNo;i++) {
		if(lMaxValue < pProjRsltSrc->plProjVal[i])
			lMaxValue = pProjRsltSrc->plProjVal[i];
		if(lMinValue > pProjRsltSrc->plProjVal[i])
			lMinValue = pProjRsltSrc->plProjVal[i];
	}
	pProjRsltSrc->lMaxValue = lMaxValue;
	pProjRsltSrc->lMinValue = lMinValue;
	pProjRsltSrc->nFirstId = 0;
	pProjRsltSrc->nLastId = pProjRsltSrc->nProjSize;
	pProjRsltSrc->dPOSEdgeShift = 0;
	pProjRsltSrc->dNEGEdgeShift = 0;
	pProjRsltSrc->lMagnify = 1;

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CData1DEdgeDetect::CreateProjRslt");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}




int CData1DEdgeDetect::GetGradientEdge(PROJ_RSLT* pProjRslt,
								 EDGE_CONTRAST_INFO* pEdgeContrastInfo,
								 int nFndContrastEdge,
								 long* plSecondGradVal,
								 EDGE_GENERAL_PARM *pParm)
{
	int i;
	int nStartPos;
	double dExactPos;

try
{
	nStartPos = pParm->nStartPos;
	m_EdgeFindRslt.nFndPOSEdge = 0;
	m_EdgeFindRslt.nFndNEGEdge = 0;
	for(i = 0; i < nFndContrastEdge; i++) {
		if(plSecondGradVal[pEdgeContrastInfo[i].nZeroCrossingLftPos] != plSecondGradVal[pEdgeContrastInfo[i].nZeroCrossingRhtPos])
			dExactPos= pEdgeContrastInfo[i].nZeroCrossingLftPos + (double)plSecondGradVal[pEdgeContrastInfo[i].nZeroCrossingLftPos]/
				(double)(plSecondGradVal[pEdgeContrastInfo[i].nZeroCrossingLftPos] - plSecondGradVal[pEdgeContrastInfo[i].nZeroCrossingRhtPos]);
		else 
			dExactPos= (pEdgeContrastInfo[i].nZeroCrossingLftPos + pEdgeContrastInfo[i].nZeroCrossingRhtPos) / 2.0;
		if(((pParm->nDirection==FORWARD)&&(pEdgeContrastInfo[i].bPOSEdge)) ||
			((pParm->nDirection==BACKWARD)&&(!pEdgeContrastInfo[i].bPOSEdge))) {
			if(pParm->nDirection==FORWARD)
				m_EdgeFindRslt.pdFndPOSEdge[m_EdgeFindRslt.nFndPOSEdge++] = dExactPos + nStartPos - pProjRslt->dPOSEdgeShift;
			else
				m_EdgeFindRslt.pdFndPOSEdge[m_EdgeFindRslt.nFndPOSEdge++] = dExactPos + nStartPos + pProjRslt->dPOSEdgeShift;
		}
		else {
			if(pParm->nDirection==FORWARD)
				m_EdgeFindRslt.pdFndNEGEdge[m_EdgeFindRslt.nFndNEGEdge++] = dExactPos + nStartPos - pProjRslt->dNEGEdgeShift;
			else
				m_EdgeFindRslt.pdFndNEGEdge[m_EdgeFindRslt.nFndNEGEdge++] = dExactPos + nStartPos + pProjRslt->dNEGEdgeShift;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CData1DEdgeDetect::GetGradientEdge");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int	CData1DEdgeDetect::ScanEdgeContrast(EDGE_GENERAL_PARM *pParm,
								  PROJ_RSLT* pProjRslt,
								  long* plFirstGradVal,
								  long* plSecondGradVal,
								  long lPOSContrast,
								  long lNEGContrast,
								  EDGE_CONTRAST_INFO* pEdgeContrastInfo,
								  int &nFndContrastEdge)
{
	int i, j;
	int plContrast;
	long lMaxGrad;
	long lMinGrad;
	long lHalfGrad;
	long lBaseGrad;
	int nFirstId;
	int nLastId;
	long lMaxIntensity;
	long lMinIntensity;

	long lPOSContrastDivBy5;
	long lNEGContrastDivBy5;
	long lSum;
	int nAvg;

try
{
	lPOSContrastDivBy5 = lPOSContrast/5;
	lNEGContrastDivBy5 = lNEGContrast/5;
	EDGE_CONTRAST_INFO EdgeContrastInfo;

	nFirstId = pProjRslt->nFirstId;
	nLastId = pProjRslt->nLastId;

	if(m_nEdgeType == LINE_EDGE_TYPE) {
		lSum = 0;
		for (i=nFirstId; i < nLastId; i++) 
			lSum += pProjRslt->plProjVal[i];
		if(nLastId > nFirstId)
			nAvg = lSum/(nLastId - nFirstId);
		else
			goto PASS;
		for (i=nFirstId; i < nLastId; i++) 
			plFirstGradVal[i]=pProjRslt->plProjVal[i] - nAvg;
		for (i=nFirstId+1; i < (nLastId - 1);i++) 
			plSecondGradVal[i]=plFirstGradVal[i]-plFirstGradVal[i-1];
	}
	else {
		for (i=nFirstId; i < (nLastId - 1); i++) 
			plFirstGradVal[i]=pProjRslt->plProjVal[i+1] - pProjRslt->plProjVal[i];
		for (i=nFirstId+1; i < (nLastId - 1);i++) 
			plSecondGradVal[i]=plFirstGradVal[i]-plFirstGradVal[i-1];
	}
	nFndContrastEdge = 0;
	if(((pParm->nDirection==FORWARD)&&(pParm->nGradient == POS)) ||
		((pParm->nDirection==BACKWARD)&&(pParm->nGradient == NEG)) ||
		(pParm->nGradient == ANY)) {
		for (i = nFirstId + 1; i < (nLastId - 2); i++) {
			if((plSecondGradVal[i] >= 0) && (plSecondGradVal[i+1] <= 0)) {
				EdgeContrastInfo.bPOSEdge = true;
				EdgeContrastInfo.nZeroCrossingLftPos = i;
				EdgeContrastInfo.nZeroCrossingRhtPos = i+1;
				if(plFirstGradVal[i] > plFirstGradVal[i+1]) 
					lMaxGrad = plFirstGradVal[i];
				else
					lMaxGrad = plFirstGradVal[i+1];
				if(lMaxGrad > lPOSContrastDivBy5) 
				{
					lHalfGrad = lMaxGrad / 2;
					lBaseGrad = lMaxGrad / 3;
					
					EdgeContrastInfo.lPOSContrast = lPOSContrast;
					EdgeContrastInfo.lNEGContrast = lNEGContrast;
					EdgeContrastInfo.lMaxContrast = lMaxGrad;
					EdgeContrastInfo.lMinContrast = 0;
					EdgeContrastInfo.HalfGradEdgeShapeInfo.lGrad = lHalfGrad;
					EdgeContrastInfo.BaseGradEdgeShapeInfo.lGrad = lBaseGrad;
					FndPOSEdgeLeft(&EdgeContrastInfo,plFirstGradVal,i,nFirstId,lMaxGrad);
					if(EdgeContrastInfo.HalfGradEdgeShapeInfo.bFndLftPos || 
						EdgeContrastInfo.BaseGradEdgeShapeInfo.bFndLftPos) 
						FndPOSEdgeRight(&EdgeContrastInfo,plFirstGradVal,i+1,nLastId,lMaxGrad);
					EdgeContrastInfo.bHalfGradEdgeShape = FALSE;
					EdgeContrastInfo.bBaseGradEdgeShape = FALSE;
					if(EdgeContrastInfo.HalfGradEdgeShapeInfo.bFndLftPos &&
						EdgeContrastInfo.HalfGradEdgeShapeInfo.bFndRhtPos) {
						lMaxIntensity = pProjRslt->plProjVal[EdgeContrastInfo.HalfGradEdgeShapeInfo.nFndLftPos];
						lMinIntensity = lMaxIntensity;
						for(j = EdgeContrastInfo.HalfGradEdgeShapeInfo.nFndLftPos + 1; j <= EdgeContrastInfo.HalfGradEdgeShapeInfo.nFndRhtPos; j++) {
							if(lMaxIntensity < pProjRslt->plProjVal[j])
								lMaxIntensity = pProjRslt->plProjVal[j];
							if(lMinIntensity > pProjRslt->plProjVal[j])
								lMinIntensity = pProjRslt->plProjVal[j];
						}
						plContrast = lMaxIntensity - lMinIntensity;
						if(plContrast > lPOSContrast)  
							EdgeContrastInfo.bHalfGradEdgeShape = true;
					}
					if(EdgeContrastInfo.BaseGradEdgeShapeInfo.bFndLftPos &&
						EdgeContrastInfo.BaseGradEdgeShapeInfo.bFndRhtPos) {
						lMaxIntensity = pProjRslt->plProjVal[EdgeContrastInfo.BaseGradEdgeShapeInfo.nFndLftPos];
						lMinIntensity = lMaxIntensity;
						for(j = EdgeContrastInfo.BaseGradEdgeShapeInfo.nFndLftPos + 1; j <= EdgeContrastInfo.BaseGradEdgeShapeInfo.nFndRhtPos; j++) {
							if(lMaxIntensity < pProjRslt->plProjVal[j])
								lMaxIntensity = pProjRslt->plProjVal[j];
							if(lMinIntensity > pProjRslt->plProjVal[j])
								lMinIntensity = pProjRslt->plProjVal[j];
						}
						plContrast = lMaxIntensity - lMinIntensity;
						if(plContrast > lPOSContrast) 
							EdgeContrastInfo.bBaseGradEdgeShape = true;
					}
					if((EdgeContrastInfo.bHalfGradEdgeShape) || (EdgeContrastInfo.bBaseGradEdgeShape))
						pEdgeContrastInfo[nFndContrastEdge++] = EdgeContrastInfo;
				}
			}
		}
	}
	if(((pParm->nDirection==FORWARD)&&(pParm->nGradient == NEG)) ||
		((pParm->nDirection==BACKWARD)&&(pParm->nGradient == POS)) ||
		(pParm->nGradient == ANY)) {
		for (i = nFirstId + 1; i < (nLastId - 2); i++) {
			if((plSecondGradVal[i] <= 0) && (plSecondGradVal[i+1] >= 0)) {
				EdgeContrastInfo.bPOSEdge = false;
				EdgeContrastInfo.nZeroCrossingLftPos = i;
				EdgeContrastInfo.nZeroCrossingRhtPos = i+1;
				if(plFirstGradVal[i] > plFirstGradVal[i+1]) 
					lMinGrad = plFirstGradVal[i+1];
				else
					lMinGrad = plFirstGradVal[i];
				if(lMinGrad < lNEGContrastDivBy5)
				{
					lHalfGrad = lMinGrad / 2;
					lBaseGrad = lMinGrad / 3;
					EdgeContrastInfo.lPOSContrast = lPOSContrast;
					EdgeContrastInfo.lNEGContrast = lNEGContrast;
					EdgeContrastInfo.lMaxContrast = 0;
					EdgeContrastInfo.lMinContrast = lMinGrad;
					EdgeContrastInfo.HalfGradEdgeShapeInfo.lGrad = lHalfGrad;
					EdgeContrastInfo.BaseGradEdgeShapeInfo.lGrad = lBaseGrad;
					FndNEGEdgeLeft(&EdgeContrastInfo,plFirstGradVal,i,nFirstId,lMinGrad);
					if(EdgeContrastInfo.HalfGradEdgeShapeInfo.bFndLftPos || 
						EdgeContrastInfo.BaseGradEdgeShapeInfo.bFndLftPos) 
						FndNEGEdgeRight(&EdgeContrastInfo,plFirstGradVal,i+1,nLastId,lMinGrad);
					EdgeContrastInfo.bHalfGradEdgeShape = FALSE;
					EdgeContrastInfo.bBaseGradEdgeShape = FALSE;
					if(EdgeContrastInfo.HalfGradEdgeShapeInfo.bFndLftPos &&
						EdgeContrastInfo.HalfGradEdgeShapeInfo.bFndRhtPos) {
						lMaxIntensity = pProjRslt->plProjVal[EdgeContrastInfo.HalfGradEdgeShapeInfo.nFndLftPos];
						lMinIntensity = lMaxIntensity;
						for(j = EdgeContrastInfo.HalfGradEdgeShapeInfo.nFndLftPos + 1; j <= EdgeContrastInfo.HalfGradEdgeShapeInfo.nFndRhtPos; j++) {
							if(lMaxIntensity < pProjRslt->plProjVal[j])
								lMaxIntensity = pProjRslt->plProjVal[j];
							if(lMinIntensity > pProjRslt->plProjVal[j])
								lMinIntensity = pProjRslt->plProjVal[j];
						}
						plContrast = lMinIntensity - lMaxIntensity;
						if(plContrast < lNEGContrast)
							EdgeContrastInfo.bHalfGradEdgeShape = true;
					}
					if(EdgeContrastInfo.BaseGradEdgeShapeInfo.bFndLftPos &&
						EdgeContrastInfo.BaseGradEdgeShapeInfo.bFndRhtPos) {
						lMaxIntensity = pProjRslt->plProjVal[EdgeContrastInfo.BaseGradEdgeShapeInfo.nFndLftPos];
						lMinIntensity = lMaxIntensity;
						for(j = EdgeContrastInfo.BaseGradEdgeShapeInfo.nFndLftPos + 1; j <= EdgeContrastInfo.BaseGradEdgeShapeInfo.nFndRhtPos; j++) {
							if(lMaxIntensity < pProjRslt->plProjVal[j])
								lMaxIntensity = pProjRslt->plProjVal[j];
							if(lMinIntensity > pProjRslt->plProjVal[j])
								lMinIntensity = pProjRslt->plProjVal[j];
						}
						plContrast = lMinIntensity - lMaxIntensity;
						if(plContrast < lNEGContrast) 
							EdgeContrastInfo.bBaseGradEdgeShape = true;
					}
					if((EdgeContrastInfo.bHalfGradEdgeShape) || (EdgeContrastInfo.bBaseGradEdgeShape))
						pEdgeContrastInfo[nFndContrastEdge++] = EdgeContrastInfo;
				}
			}
		}
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CData1DEdgeDetect::ScanEdgeContrast");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}
int	CData1DEdgeDetect::FndNEGEdgeLeft(EDGE_CONTRAST_INFO* pEdgeContrastInfo,
								long* plFirstGradVal,
								int nStartPos,
								int nFirstId,
								long lMinGrad)
{
	int j;
	pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndLftPos = FALSE;
	pEdgeContrastInfo->BaseGradEdgeShapeInfo.bFndLftPos = FALSE;
	for(j = nStartPos; j >= nFirstId; j--) {
		if((!pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndLftPos) &&
			(plFirstGradVal[j] > pEdgeContrastInfo->HalfGradEdgeShapeInfo.lGrad)) {
			pEdgeContrastInfo->HalfGradEdgeShapeInfo.nFndLftPos = j;
			pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndLftPos = TRUE;
		}
		if(plFirstGradVal[j] > pEdgeContrastInfo->BaseGradEdgeShapeInfo.lGrad) {
			pEdgeContrastInfo->BaseGradEdgeShapeInfo.nFndLftPos = j;
			pEdgeContrastInfo->BaseGradEdgeShapeInfo.bFndLftPos = TRUE;
			break;
		}
		if(plFirstGradVal[j] < lMinGrad)
			break;
	}
	return 0;
}

int	CData1DEdgeDetect::FndNEGEdgeRight(EDGE_CONTRAST_INFO* pEdgeContrastInfo,
								long* plFirstGradVal,
								int nStartPos,
								int nLastId,
								long lMinGrad)
{
	int j;
	pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndRhtPos = FALSE;
	pEdgeContrastInfo->BaseGradEdgeShapeInfo.bFndRhtPos = FALSE;
	for(j = nStartPos; j < nLastId; j++) {
		if((!pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndRhtPos) && 
			(plFirstGradVal[j] > pEdgeContrastInfo->HalfGradEdgeShapeInfo.lGrad)) {
			pEdgeContrastInfo->HalfGradEdgeShapeInfo.nFndRhtPos = j;
			pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndRhtPos = TRUE;
		}
		if(plFirstGradVal[j] > pEdgeContrastInfo->BaseGradEdgeShapeInfo.lGrad) {
			pEdgeContrastInfo->BaseGradEdgeShapeInfo.nFndRhtPos = j;
			pEdgeContrastInfo->BaseGradEdgeShapeInfo.bFndRhtPos = TRUE;
			break;
		}
		if(plFirstGradVal[j] < lMinGrad)
			break;
	}
	return 0;
}

int	CData1DEdgeDetect::FndPOSEdgeLeft(EDGE_CONTRAST_INFO* pEdgeContrastInfo,
								long* plFirstGradVal,
								int nStartPos,
								int nFirstId,
								long lMaxGrad)
{
	int j;
	pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndLftPos = FALSE;
	pEdgeContrastInfo->BaseGradEdgeShapeInfo.bFndLftPos = FALSE;
	for(j = nStartPos; j >= nFirstId; j--) {
		if((!pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndLftPos) &&
			(plFirstGradVal[j] < pEdgeContrastInfo->HalfGradEdgeShapeInfo.lGrad)) {
			pEdgeContrastInfo->HalfGradEdgeShapeInfo.nFndLftPos = j;
			pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndLftPos = TRUE;
		}
		if(plFirstGradVal[j] < pEdgeContrastInfo->BaseGradEdgeShapeInfo.lGrad) {
			pEdgeContrastInfo->BaseGradEdgeShapeInfo.nFndLftPos = j;
			pEdgeContrastInfo->BaseGradEdgeShapeInfo.bFndLftPos = TRUE;
			break;
		}
		if(plFirstGradVal[j] > lMaxGrad)
			break;
	}
	return 0;
}

int	CData1DEdgeDetect::FndPOSEdgeRight(EDGE_CONTRAST_INFO* pEdgeContrastInfo,
								long* plFirstGradVal,
								int nStartPos,
								int nLastId,
								long lMaxGrad)
{
	int j;
	pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndRhtPos = FALSE;
	pEdgeContrastInfo->BaseGradEdgeShapeInfo.bFndRhtPos = FALSE;
	for(j = nStartPos; j < nLastId; j++) {
		if((!pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndRhtPos) &&
			(plFirstGradVal[j] < pEdgeContrastInfo->HalfGradEdgeShapeInfo.lGrad)) {
			pEdgeContrastInfo->HalfGradEdgeShapeInfo.nFndRhtPos = j;
			pEdgeContrastInfo->HalfGradEdgeShapeInfo.bFndRhtPos = TRUE;
		}
		if(plFirstGradVal[j] < pEdgeContrastInfo->BaseGradEdgeShapeInfo.lGrad) {
			pEdgeContrastInfo->BaseGradEdgeShapeInfo.nFndRhtPos = j;
			pEdgeContrastInfo->BaseGradEdgeShapeInfo.bFndRhtPos = TRUE;
			break;
		}
		if(plFirstGradVal[j] > lMaxGrad)
			break;
	}
	return 0;
}
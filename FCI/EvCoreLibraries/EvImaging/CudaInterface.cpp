/// Cuda Functions Interface class
/// Udupa; June'2014

#include "StdAfx.h"
#include "CudaInterface.h"
#include "ImageBuffer.h"
#include "Imaging.h"

CCudaInterface::CCudaInterface()
{
	m_bOptimizeCpu = TRUE;
	m_bOptimizeGpu = FALSE;
	m_bGpuAvailable = FALSE;
}

CCudaInterface::~CCudaInterface()
{
	CloseCudaKernel();
}

void CCudaInterface::InitializeCudaKernel(CSize szImage)
{
		m_bOptimizeGpu = FALSE;
}

void CCudaInterface::CloseCudaKernel()
{
	DeleteCriticalSection(&m_csCuda);
}
 
int CCudaInterface::ImgAND(BYTE *pRowSrc0,
	int nImgPitchSrc0,
	BYTE *pRowSrc1,
	int nImgPitchSrc1,
	BYTE *pRowDst,
	int nImgPitchDst,
	int nWidthDst,
	int nHeightDst)
{
	if(OptimizeCpu()) {
		int x, y;
		UINT64 *pdColSrc0, *pdColSrc1, *pdColDst;
		BYTE *pColSrc0, *pColSrc1, *pColDst;
		int nWidthDstMin = nWidthDst - nWidthDst%8;

		for (y=0; y<nHeightDst; y++, pRowSrc0+=nImgPitchSrc0, pRowSrc1+=nImgPitchSrc1, pRowDst+=nImgPitchDst) {
			for(x=0,
				pdColSrc0 = (UINT64*) pRowSrc0,
				pdColSrc1 = (UINT64*) pRowSrc1,
				pdColDst  = (UINT64*) pRowDst;
				x < nWidthDstMin;
				x+=8, pdColSrc0++, pdColSrc1++, pdColDst++)
				{
					*pdColDst = *pdColSrc0 & *pdColSrc1;
				}
			for(x=nWidthDstMin,
				pColSrc0=(BYTE*) pdColSrc0,
				pColSrc1=(BYTE*) pdColSrc1,
				pColDst=(BYTE*) pdColDst;
				x < nWidthDst;
				x++, pColSrc0++, pColSrc1++, pColDst++)
				{
					*pColDst = *pColSrc0 & *pColSrc1;
				}
		}
	}

	return 0;
}

int CCudaInterface::ImgOR(BYTE *pRowSrc0,
	int nImgPitchSrc0,
	BYTE *pRowSrc1,
	int nImgPitchSrc1,
	BYTE *pRowDst,
	int nImgPitchDst,
	int nWidthDst,
	int nHeightDst)
{
	if(OptimizeCpu()) {
		int x, y;
		UINT64 *pdColSrc0, *pdColSrc1, *pdColDst;
		BYTE *pColSrc0, *pColSrc1, *pColDst;
		int nWidthDstMin = nWidthDst - nWidthDst%8;

		for (y=0; y<nHeightDst; y++, pRowSrc0+=nImgPitchSrc0, pRowSrc1+=nImgPitchSrc1, pRowDst+=nImgPitchDst) {
			for(x=0,
				pdColSrc0 = (UINT64*) pRowSrc0,
				pdColSrc1 = (UINT64*) pRowSrc1,
				pdColDst  = (UINT64*) pRowDst;
				x < nWidthDstMin;
				x+=8, pdColSrc0++, pdColSrc1++, pdColDst++)
				{
					*pdColDst = *pdColSrc0 | *pdColSrc1;
				}
			for(x=nWidthDstMin,
				pColSrc0=(BYTE*) pdColSrc0,
				pColSrc1=(BYTE*) pdColSrc1,
				pColDst=(BYTE*) pdColDst;
				x < nWidthDst;
				x++, pColSrc0++, pColSrc1++, pColDst++)
				{
					*pColDst = *pColSrc0 | *pColSrc1;
				}
		}
	}

	return 0;
}

int CCudaInterface::GetGradMaxImage(CImageBuffer *pImgSrc,
									CRect* prcSrc,
									int nColor,
									CImageBuffer *pImgDst,
									CRect* prcDst,
									CRect *prcGradStep)
{
	int nPitchSrc = pImgSrc->GetPitch();
	int nPitchDst = pImgDst->GetPitch();

	int nWidth = prcSrc->Width();
	int nHeight = prcSrc->Height();

	BYTE *pImgSrcPtr = pImgSrc->GetImgPtr() + prcSrc->top * nPitchSrc + prcSrc->left;
	BYTE *pImgDstPtr = pImgDst->GetImgPtr() + prcDst->top * nPitchDst + prcDst->left;

	int nError = 0;
	if(m_bGpuAvailable && m_bOptimizeGpu) {
	}
	else {
		int nStepLeft = prcGradStep->left;
		int nStepRight = prcGradStep->right;
		int nStepTop = prcGradStep->top * nPitchSrc;
		int nStepBottom = prcGradStep->bottom * nPitchSrc;
	
		int nLimitLeft = prcGradStep->left - 1;
		int nLimitRight = nWidth - prcGradStep->right;
		int nLimitTop = prcGradStep->top - 1;
		int nLimitBottom = nHeight - prcGradStep->bottom;

		int nIndexSrcRow=0, nIndexDstRow=0, nIndexSrcCol, nIndexDstCol;

		BYTE nVal;
		int nDiff, nMaxDiff;
		int x,y;

		if(nColor == WHITE) {
			for(y=0; y<nHeight; y++, nIndexSrcRow+=nPitchSrc, nIndexDstRow+=nPitchDst) {
				for(x=0, nIndexSrcCol=nIndexSrcRow, nIndexDstCol=nIndexDstRow; x<nWidth; x++, nIndexSrcCol++, nIndexDstCol++) {
					nVal = pImgSrcPtr[nIndexSrcCol];
					nMaxDiff = 0;

					if(x > nLimitLeft) {
						nDiff = nVal - pImgSrcPtr[nIndexSrcCol - nStepLeft];
						if(nDiff > 0)
							nMaxDiff = nDiff;
					}
					if(x < nLimitRight) {
						nDiff = nVal - pImgSrcPtr[nIndexSrcCol + nStepRight];
						if(nDiff > nMaxDiff)
							nMaxDiff = nDiff;
					}
					if(y > nLimitTop) {
						nDiff = nVal - pImgSrcPtr[nIndexSrcCol - nStepTop];
						if(nDiff > nMaxDiff)
							nMaxDiff = nDiff;
					}
					if(y < nLimitBottom) {
						nDiff = nVal - pImgSrcPtr[nIndexSrcCol + nStepBottom];
						if(nDiff > nMaxDiff)
							nMaxDiff = nDiff;
					}
					if(pImgDstPtr[nIndexDstCol] < nMaxDiff)
						pImgDstPtr[nIndexDstCol] = nMaxDiff;
				}
			}
		}
		else {
			for(y=0; y<nHeight; y++, nIndexSrcRow+=nPitchSrc, nIndexDstRow+=nPitchDst) {
				for(x=0, nIndexSrcCol=nIndexSrcRow, nIndexDstCol=nIndexDstRow; x<nWidth; x++, nIndexSrcCol++, nIndexDstCol++) {
					nVal = pImgSrcPtr[nIndexSrcCol];
					nMaxDiff = 0;

					if(x > nLimitLeft) {
						nDiff = pImgSrcPtr[nIndexSrcCol - nStepLeft] - nVal;
						if(nDiff > 0)
							nMaxDiff = nDiff;
					}
					if(x < nLimitRight) {
						nDiff = pImgSrcPtr[nIndexSrcCol + nStepRight] - nVal;
						if(nDiff > nMaxDiff)
							nMaxDiff = nDiff;
					}
					if(y > nLimitTop) {
						nDiff = pImgSrcPtr[nIndexSrcCol - nStepTop] - nVal;
						if(nDiff > nMaxDiff)
							nMaxDiff = nDiff;
					}
					if(y < nLimitBottom) {
						nDiff = pImgSrcPtr[nIndexSrcCol + nStepBottom] - nVal;
						if(nDiff > nMaxDiff)
							nMaxDiff = nDiff;
					}
					if(pImgDstPtr[nIndexDstCol] < nMaxDiff)
						pImgDstPtr[nIndexDstCol] = nMaxDiff;
				}
			}
		}
	}

	return nError;
}

int CCudaInterface::ImageBinDilation(CImageBuffer *pImgSrc, 
									CRect *prcSrc,
									int nBlobType,
									CImageBuffer *pImgDst,
									CRect *prcDst,
									CSize* pszMask)
{
	BYTE *pImgSrcPtr = pImgSrc->GetImgPtr() + prcSrc->top * pImgSrc->GetPitch() + prcSrc->left;
	BYTE *pImgDstPtr = pImgDst->GetImgPtr() + prcDst->top * pImgDst->GetPitch() + prcDst->left;
	int nSrcPitch = pImgSrc->GetPitch();
	int nDstPitch = pImgDst->GetPitch();
	int nWidth = prcSrc->Width();
	int nHeight = prcSrc->Height();

	int nDilationX = pszMask->cx;
	int nDilationY = pszMask->cy;

	int nError = 0;
	if(m_bGpuAvailable && m_bOptimizeGpu) {
	}
	else {
		BYTE nBlobColor = nBlobType==WHITE ? 255 : 0;

		int nSrcRow, nSrcCol, nDstRow, nDstCol;
		BYTE nVal;
		int x, y, nDstLastCol, nStart, nEnd;

		CImageBuffer ImgTemp;
		ImgTemp.Allocate(CSize(nWidth, nHeight));
		BYTE *pImgTempPtr = ImgTemp.GetImgPtr();
		int nTmpPitch = ImgTemp.GetPitch();
		CRect rcTmp = CRect(CPoint(0,0), ImgTemp.GetImgSize());

		BYTE *pSrc, *pDst;
		int nPitchSrc, nPitchDst;

		if(nDilationX > 0) {
			pSrc = pImgSrcPtr;
			nPitchSrc = nSrcPitch;
			if(nDilationY > 0) {
				pDst = pImgTempPtr;
				nPitchDst = nTmpPitch;
			}
			else {
				pDst = pImgDstPtr;
				nPitchDst = nDstPitch;
			}
			for(y=0, nSrcRow=0, nDstRow=0; y<nHeight; y++, nSrcRow+=nPitchSrc, nDstRow+=nPitchDst) {
				memcpy(pDst+nDstRow, pSrc+nSrcRow, nWidth);
				nDstLastCol = nDstRow + nWidth;
				for(x=1, nSrcCol=nSrcRow+1, nDstCol=nDstRow+1, nStart=0; x<nWidth; x++, nSrcCol++, nDstCol++) {
					nVal = pSrc[nSrcCol];
					if(pSrc[nSrcCol-1] != nVal) {
						if(nVal == nBlobColor) {
							nEnd = nDstCol;
							nStart = nDstCol - nDilationX;
							if(nStart < nDstRow)
								nStart = nDstRow;
						}
						else {
							nStart = nDstCol;
							nEnd = nDstCol + nDilationX;
							if(nEnd > nDstLastCol)
								nEnd = nDstLastCol;
						}
						memset(pDst+nStart, nBlobColor, nEnd-nStart);
					}
				}
			}

			pSrc = pImgTempPtr;
			nPitchSrc = nTmpPitch;
		}
		else {
			pSrc = pImgSrcPtr;
			nPitchSrc = nSrcPitch;
		}

		if(nDilationY > 0) {
			pDst = pImgDstPtr;
			nPitchDst = nDstPitch;

			int nFillY = nDilationY * nDstPitch;
			int nDstLastRow = nHeight * nDstPitch;

			for(nSrcRow=0, nDstRow=0; nSrcRow<nDstLastRow; nSrcRow+=nPitchSrc, nDstRow+=nPitchDst)
				memcpy(pDst+nDstRow, pSrc+nSrcRow, nWidth);

			for(nSrcCol=0, nDstCol=0; nDstCol<nWidth; nSrcCol++, nDstCol++) {
				for(nSrcRow=nSrcCol+nSrcPitch, nDstRow=nDstCol+nDstPitch; nDstRow<nDstLastRow; nSrcRow+=nSrcPitch, nDstRow+=nDstPitch) {
					nVal = pSrc[nSrcRow];
					if(pSrc[nSrcRow-nSrcPitch] != nVal) {
						if(nVal == nBlobColor) {
							nEnd = nDstRow;
							nStart = nDstRow - nFillY;
							if(nStart < nDstCol)
								nStart = nDstCol;
						}
						else {
							nStart = nDstRow;
							nEnd = nDstRow + nFillY;
							if(nEnd > nDstLastRow)
								nEnd = nDstLastRow;
						}
						for(y=nStart; y<nEnd; y+=nDstPitch) 
							pDst[y] = nBlobColor;
					}
				}
			}
		}
	}

	return nError;
}

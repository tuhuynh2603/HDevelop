
#include "stdAfx.h"
#include "FloodFill.h"
#include "AppDef.h"
#include "EdgeMatching.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int FillImageHole(IMAGEVIEW_INFO* pView,
				CImageBuffer* pImgBin,
				CRect* prcBin,
				int nMinArea,
				int nMinSize,
				long lDebugFlag)
{
	int i, j;
	ADV_BLOB_FIND_PARM AdvBlobParm;
	CAdvBlobFind BlobFinder;
	CPoint2D* ppt2DMask;
	int nMaskPt;
	CMask Mask;
	long lProcMode;
	int nError;

	ppt2DMask = NULL;
	AdvBlobParm.nBlobType = WHITE;
	AdvBlobParm.nMinBlobArea = nMinArea;
	AdvBlobParm.nMaxBlobArea = prcBin->Width() * prcBin->Height();
	AdvBlobParm.nMaxYBlobSize = prcBin->Height();
	AdvBlobParm.nMinYBlobSize = nMinSize;
	AdvBlobParm.nMaxXBlobSize = prcBin->Width();
	AdvBlobParm.nMinXBlobSize = nMinSize;
	AdvBlobParm.rcROISrc = *prcBin;
	AdvBlobParm.rcROIDst = *prcBin;
	lProcMode = FIND_BLOB_CONTOUR;
	nError = BlobFinder.GetAdvBlob3(pView,pImgBin,&AdvBlobParm,lProcMode,lDebugFlag);
	if(nError)
		goto ErrorAbort;
	pImgBin->ClrImg(0);


	for(i=0;i<BlobFinder.m_AdvBlobFindRslt.nFndBlobs;i++) {
		if(ppt2DMask != NULL)
			delete [] ppt2DMask;
		ppt2DMask = NULL;
		nMaskPt = BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].nBlobContourSize;
		ppt2DMask = new CPoint2D[nMaskPt];
		for(j=0;j<nMaskPt;j++)
			ppt2DMask[j] = BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].pptBlobContour[j];
		nError = Mask.ApplyGrayMaskFreeHand(pImgBin,
											prcBin,
											&ppt2DMask[0], 
											nMaskPt, 
											255);
		if(nError)
			goto ErrorAbort;
	}

	if(ppt2DMask != NULL)
		delete [] ppt2DMask;
	return 0;
ErrorAbort:
	if(ppt2DMask != NULL)
		delete [] ppt2DMask;
	return -1;
}

int FillImageHole(IMAGEVIEW_INFO* pView,
				CImageBuffer* pImgBin,
				CRect* prcBin,
				long lDebugFlag)
{
	int i, j;
	ADV_BLOB_FIND_PARM AdvBlobParm;
	CAdvBlobFind BlobFinder;
	CPoint2D* ppt2DMask;
	int nMaskPt;
	CMask Mask;
	long lProcMode;
	int nError;

	ppt2DMask = NULL;
	AdvBlobParm.nBlobType = WHITE;
	AdvBlobParm.nMinBlobArea = 5;
	AdvBlobParm.nMaxBlobArea = prcBin->Width() * prcBin->Height();
	AdvBlobParm.nMaxYBlobSize = prcBin->Height();
	AdvBlobParm.nMinYBlobSize = 3;
	AdvBlobParm.nMaxXBlobSize = prcBin->Width();
	AdvBlobParm.nMinXBlobSize = 3;
	AdvBlobParm.rcROISrc = *prcBin;
	AdvBlobParm.rcROIDst = *prcBin;
	lProcMode = FIND_BLOB_CONTOUR;
	nError = BlobFinder.GetAdvBlob3(pView,pImgBin,&AdvBlobParm,lProcMode,lDebugFlag);
	if(nError)
		goto ErrorAbort;
	pImgBin->ClrImg(0);
	for(i=0;i<BlobFinder.m_AdvBlobFindRslt.nFndBlobs;i++) {
		if(ppt2DMask != NULL)
			delete [] ppt2DMask;
		ppt2DMask = NULL;
		nMaskPt = BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].nBlobContourSize;
		ppt2DMask = new CPoint2D[nMaskPt];
		for(j=0;j<nMaskPt;j++)
			ppt2DMask[j] = BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].pptBlobContour[j];
		nError = Mask.ApplyGrayMaskFreeHand(pImgBin,
											prcBin,
											&ppt2DMask[0], 
											nMaskPt, 
											255);
		if(nError)
			goto ErrorAbort;
	}
	if(ppt2DMask != NULL)
		delete [] ppt2DMask;
	return 0;
ErrorAbort:
	if(ppt2DMask != NULL)
		delete [] ppt2DMask;
	return -1;
}

int FillImageHole(IMAGEVIEW_INFO* pView,
				CImageBuffer* pImgBin,
				CRect* prcBin,
				int nMaxDiameter,
				long lDebugFlag)
{
	int i, j;
	ADV_BLOB_FIND_PARM AdvBlobParm;
	CAdvBlobFind BlobFinder;
	CPoint2D* ppt2DMask;
	int nMaskPt;
	CMask Mask;
	long lProcMode;
	int nError;

	ppt2DMask = NULL;
	AdvBlobParm.nBlobType = WHITE;
	AdvBlobParm.nMinBlobArea = 5;
	AdvBlobParm.nMaxBlobArea = prcBin->Width() * prcBin->Height();
	AdvBlobParm.nMaxYBlobSize = prcBin->Height();
	AdvBlobParm.nMinYBlobSize = 3;
	AdvBlobParm.nMaxXBlobSize = prcBin->Width();
	AdvBlobParm.nMinXBlobSize = 3;
	AdvBlobParm.rcROISrc = *prcBin;
	AdvBlobParm.rcROIDst = *prcBin;
	lProcMode = FIND_BLOB_CONTOUR;
	nError = BlobFinder.GetAdvBlob3(pView,pImgBin,&AdvBlobParm,lProcMode,lDebugFlag);
	if(nError)
		goto ErrorAbort;
	pImgBin->ClrImg(0);
	for(i=0;i<BlobFinder.m_AdvBlobFindRslt.nFndBlobs;i++) {
		if((BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].rcFndBlobPos.Width() > nMaxDiameter) ||
			(BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].rcFndBlobPos.Height() > nMaxDiameter))
			continue;
		if(ppt2DMask != NULL)
			delete [] ppt2DMask;
		ppt2DMask = NULL;
		nMaskPt = BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].nBlobContourSize;
		ppt2DMask = new CPoint2D[nMaskPt];
		for(j=0;j<nMaskPt;j++)
			ppt2DMask[j] = BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].pptBlobContour[j];
		nError = Mask.ApplyGrayMaskFreeHand(pImgBin,
											prcBin,
											&ppt2DMask[0], 
											nMaskPt, 
											255);
		if(nError)
			goto ErrorAbort;
	}
	if(ppt2DMask != NULL)
		delete [] ppt2DMask;
	return 0;
ErrorAbort:
	if(ppt2DMask != NULL)
		delete [] ppt2DMask;
	return -1;
}


int FillImageHole(IMAGEVIEW_INFO* pView,
				CImageBuffer* pImgBin,
				CRect* prcBin,
				CSize* pcsMaxHole,
				long lDebugFlag)
{
	int i, j;
	ADV_BLOB_FIND_PARM AdvBlobParm;
	CAdvBlobFind BlobFinder;
	CPoint2D* ppt2DMask;
	CMask Mask;
	long lProcMode;
	CRect rcHole;
	int nError;
try {
	ppt2DMask = NULL;
	AdvBlobParm.nBlobType = WHITE;
	AdvBlobParm.nMinBlobArea = 1;
	AdvBlobParm.nMaxBlobArea = prcBin->Width() * prcBin->Height();
	AdvBlobParm.nMaxYBlobSize = prcBin->Height();
	AdvBlobParm.nMinYBlobSize = 1;
	AdvBlobParm.nMaxXBlobSize = prcBin->Width();
	AdvBlobParm.nMinXBlobSize = 1;
	AdvBlobParm.rcROISrc = *prcBin;
	AdvBlobParm.rcROIDst = *prcBin;
	lProcMode = FIND_BLOB_CONTOUR | FIND_BLOB_HOLE;
	nError = BlobFinder.GetAdvBlobHole(pView,pImgBin,&AdvBlobParm,lProcMode,lDebugFlag);
	if(nError)
		goto ErrorAbort;
//	pImgBin->ClrImg(0);
	for(i=0;i<BlobFinder.m_AdvBlobFindRslt.nFndBlobs;i++) {
		for(j=0;j<BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].nBlobHole;j++)
		{
			rcHole = BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].prcHole[j];
			if((rcHole.Width() > pcsMaxHole->cx) || (rcHole.Height() > pcsMaxHole->cy))
				continue;
			nError = Mask.ApplyGrayMaskFreeHand(pImgBin,
												prcBin,
												&BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].ppptBlobHoleContour[j][0], 
												BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].pnptBlobHoleContourSize[j],
												&(prcBin->TopLeft()),
												255);
			if(nError)
				goto ErrorAbort;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"FillImageHole");
	goto ErrorAbort;
}
	if(ppt2DMask != NULL)
		delete [] ppt2DMask;
	return 0;
ErrorAbort:
	if(ppt2DMask != NULL)
		delete [] ppt2DMask;
	return -1;
}

int FillImageSmallHole(IMAGEVIEW_INFO* pView,
				CImageBuffer* pImgBin,
				CRect* prcBin,
				CSize* pcsFillMaxHole,
				int nBlobColor,
				long lDebugFlag)
{
	int i, j;
	ADV_BLOB_FIND_PARM AdvBlobParm; 
	CAdvBlobFind BlobFinder;
	CMask Mask;
	long lProcMode;
	CRect rcHole;
	CPoint ptOffset;
	int nFillHoleColor;
	int nError;
	if(nBlobColor == WHITE)
		nFillHoleColor = 255;
	else {
		nFillHoleColor = 0;
	}
	AdvBlobParm.nBlobType = nBlobColor;
	AdvBlobParm.nMinBlobArea = 1;
	AdvBlobParm.nMaxBlobArea = prcBin->Width() * prcBin->Height();
	AdvBlobParm.nMaxYBlobSize = prcBin->Height();
	AdvBlobParm.nMinYBlobSize = 1;
	AdvBlobParm.nMaxXBlobSize = prcBin->Width();
	AdvBlobParm.nMinXBlobSize = 1;
	AdvBlobParm.rcROISrc = *prcBin;
	AdvBlobParm.rcROIDst = *prcBin;
	lProcMode = FIND_BLOB_HOLE|FIND_BLOB_CONTOUR;
	nError = BlobFinder.GetAdvBlobHole(pView,pImgBin,&AdvBlobParm,lProcMode,lDebugFlag);
	if(nError)       
		goto ErrorAbort; 
	ptOffset = CPoint(0,0);
	for(i=0;i<BlobFinder.m_AdvBlobFindRslt.nFndBlobs;i++) {
		for(j=0;j<BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].nBlobHole;j++) {
			rcHole = BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].prcHole[j];
			if((rcHole.Width() > pcsFillMaxHole->cx) && (rcHole.Height() > pcsFillMaxHole->cy))
				continue;
			nError = Mask.ApplyGrayMaskFreeHand(pImgBin,
												prcBin,
												&BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].ppptBlobHoleContour[j][0], 
												BlobFinder.m_AdvBlobFindRslt.pAdvBlobInfo[i].pnptBlobHoleContourSize[j],
												&ptOffset,
												nFillHoleColor);
			if(nError)
				goto ErrorAbort;
		} 
	}
	return 0;
ErrorAbort:
	return -1;
}

CImgFloodFill::CImgFloodFill()
{
	pLftEdge = NULL;
	pTopEdge = NULL;
	pRhtEdge = NULL;
	pBotEdge = NULL;
}

CImgFloodFill::~CImgFloodFill()
{
	Clean();
}
	
void CImgFloodFill::Clean()
{
	if(pLftEdge != NULL)
		delete pLftEdge;
	if(pTopEdge != NULL)
		delete pTopEdge;
	if(pRhtEdge != NULL)
		delete pRhtEdge;
	if(pBotEdge != NULL)
		delete pBotEdge;
	pLftEdge = NULL;
	pTopEdge = NULL;
	pRhtEdge = NULL;
	pBotEdge = NULL;
}

int CImgFloodFill::CheckEdgeConnection(CImageBuffer *pImgBinSrc,
						CRect *prcBinSrc,
						int nBlobType,
						CImageBuffer *pImgBinDst,
						CRect *prcBinDstRelative,
						CPoint *pptBinDstTopLftInrcBinSrc)
{
	CRect rcBinDst;
	CRect rcCopy;
	CPoint *pptSeed;
	int nSeedNo;
	CRect rcFilled;
	int nError;

try
{
	pptSeed = NULL;
	rcBinDst = CRect(*pptBinDstTopLftInrcBinSrc, prcBinDstRelative->Size());
	if(ValidateRect(prcBinSrc, &rcBinDst)) 
		goto PASS;
	rcBinSrcExt = *prcBinSrc;
	rcBinSrcExt.InflateRect(1,1,1,1);
	rcBinSrcExtRelative = CRect(CPoint(0,0), rcBinSrcExt.Size());
	rcCopy = CRect(CPoint(1,1), prcBinSrc->Size());
	ImgBinSrcExt.Allocate(AbsSize(rcBinSrcExtRelative.Size()));
	nError = ImgCopy(pImgBinSrc,
					 prcBinSrc,
					 &ImgBinSrcExt,
					 &rcCopy);
	if(nError)
		goto ErrorAbort;
	nError = ClearImageROIEdge(&ImgBinSrcExt,
							   &rcBinSrcExtRelative,
							   0);
	if(nError)
		goto ErrorAbort;

	pptSeed = new CPoint[abs(rcBinSrcExtRelative.Width() * 2 + rcBinSrcExtRelative.Height() * 2)];
	nError = GetEdgeSeed(&ImgBinSrcExt,
						 &rcBinSrcExtRelative,
						 &pptSeed[0],
						 &nSeedNo);
	if(nError)
		goto ErrorAbort;
	nError = GetFloodFillImageWithCleanBounding(&ImgBinSrcExt,
						&rcBinSrcExtRelative,
						&pptSeed[0],
						nSeedNo,
						nBlobType,
						&rcFilled);
	if(nError)
		goto ErrorAbort;
	*prcBinDstRelative = CRect(CPoint(0,0), rcFilled.Size());
	*pptBinDstTopLftInrcBinSrc = rcFilled.TopLeft();

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgFloodFill::CheckEdgeConnection");
	goto ErrorAbort;
}
PASS:
	if(pptSeed != NULL)
		delete [] pptSeed;
	return 0;
ErrorAbort :
	if(pptSeed != NULL)
		delete [] pptSeed;
	return -1;


}

int CImgFloodFill::GetEdgeSeed(CImageBuffer *pImgBinSrcExt,
						 CRect* prcBinSrcExtRelative,
						 CPoint* pptSeed,
						 int* pnSeedNo)
{
	int i;
	BYTE* pImgBinSrcExtPtr;
	int nPitch;
	int nSeedNo;
	int nx1, ny1;
	int nx2, ny2;
	int nIndex1;
	int nIndex2;

try
{
	*pnSeedNo = 0;
	if((prcBinSrcExtRelative->Width() < 6) || (prcBinSrcExtRelative->Height() < 6))
		goto PASS;
	pImgBinSrcExtPtr = pImgBinSrcExt->GetImgPtr();
	nPitch = pImgBinSrcExt->GetPitch();
	nSeedNo = 0;
	nx1 = 2;
	nIndex1 = nx1;
	nx2 = prcBinSrcExtRelative->Width() - nx1 - 1;
	nIndex2 = nx2;
	for(i=0;i<prcBinSrcExtRelative->Height();i++) {
		if(pImgBinSrcExtPtr[nIndex1] == 200) 
			pptSeed[nSeedNo++] = CPoint(nx1, i);
		if(pImgBinSrcExtPtr[nIndex2] == 200) 
			pptSeed[nSeedNo++] = CPoint(nx2, i);
		nIndex1 += nPitch;
		nIndex2 += nPitch;
	}
	ny1 = 2;
	nIndex1 = ny1 * nPitch;
	ny2 = prcBinSrcExtRelative->Height() - ny1 - 1;
	nIndex2 = ny2 * nPitch;
	for(i=0;i<prcBinSrcExtRelative->Width();i++) {
		if(pImgBinSrcExtPtr[nIndex1+i] == 200) 
			pptSeed[nSeedNo++] = CPoint(i, ny1);
		if(pImgBinSrcExtPtr[nIndex2+i] == 200) 
			pptSeed[nSeedNo++] = CPoint(i, ny2);
	}
	*pnSeedNo = nSeedNo;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgFloodFill::GetEdgeSeed");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;

}


int CImgFloodFill::ClearImageROIEdgeAndBackup(CImageBuffer *pImgSrc,
				   CRect* prcROISrc,
				   BYTE byteLable,
				   BYTE* pLftEdge,
				   BYTE* pTopEdge,
				   BYTE* pRhtEdge,
				   BYTE* pBotEdge)
{
	int x, y;
	BYTE *pImgPtr, *pImgTopLftPtr, *pImgProcTopLine, *pImgProcBotLine, *pImgProcLeftLine, *pImgProcRightLine;
	int nPitchSrc, nImgProcWidth, nImgProcHeight;
	int nIndex;

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
	nIndex = 0;
	for(y = 0; y < nImgProcHeight; y++, pImgProcLeftLine += nPitchSrc, pImgProcRightLine += nPitchSrc) {
		pLftEdge[nIndex] = *pImgProcLeftLine;
		pRhtEdge[nIndex] = *pImgProcRightLine;
		*pImgProcLeftLine = byteLable;
		*pImgProcRightLine = byteLable;
		nIndex++;
	}

	pImgProcTopLine = pImgTopLftPtr;
	pImgProcBotLine = pImgTopLftPtr + nPitchSrc * (nImgProcHeight - 1);
	nIndex = 0;
	for(x = 0; x < nImgProcWidth; x++) {
		pTopEdge[nIndex] = *pImgProcTopLine;
		pBotEdge[nIndex] = *pImgProcBotLine;
		*pImgProcTopLine = byteLable;
		*pImgProcBotLine = byteLable;
		pImgProcTopLine++;
		pImgProcBotLine++;
		nIndex++;
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgFloodFill::ClearImageROIEdgeAndBackup");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CImgFloodFill::RestoreImageROIEdge(CImageBuffer *pImgSrc,
				   CRect* prcROISrc,
				   BYTE* pLftEdge,
				   BYTE* pTopEdge,
				   BYTE* pRhtEdge,
				   BYTE* pBotEdge)
{
	int x, y;
	BYTE *pImgPtr, *pImgTopLftPtr, *pImgProcTopLine, *pImgProcBotLine, *pImgProcLeftLine, *pImgProcRightLine;
	int nPitchSrc, nImgProcWidth, nImgProcHeight;
	int nIndex;

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
	nIndex = 0;
	for(y = 0; y < nImgProcHeight; y++, pImgProcLeftLine += nPitchSrc, pImgProcRightLine += nPitchSrc) {
		*pImgProcLeftLine = pLftEdge[nIndex];
		*pImgProcRightLine = pRhtEdge[nIndex];
		nIndex++;
	}

	pImgProcTopLine = pImgTopLftPtr;
	pImgProcBotLine = pImgTopLftPtr + nPitchSrc * (nImgProcHeight - 1);
	nIndex = 0;
	for(x = 0; x < nImgProcWidth; x++) {
		*pImgProcTopLine = pTopEdge[nIndex];
		*pImgProcBotLine = pBotEdge[nIndex];
		nIndex++;
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgFloodFill::RestoreImageROIEdge");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CImgFloodFill::GetFloodFillImage(CImageBuffer *pImgBinSrc,
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
	CRect rcBinDst;
	int nWidthSrc;
	int nHeightSrc;
	CRect rcBinDstEdgeCheck;

try
{
	Clean();
	plSeedsAddr1 = NULL;
	plSeedsAddr2 = NULL;

	nLftSrc = prcBinSrc->left;
	nTopSrc = prcBinSrc->top;
	nRhtSrc = prcBinSrc->right - 1;
	nBotSrc = prcBinSrc->bottom - 1;
	*prcBinDstRelative = CRect(0,0,0,0);
	*pptBinDstTopLftInrcBinSrc = CPoint(0,0);
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
	if(nBlobType == WHITE) {
		plSeedsAddr1 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
		plSeedsAddr2 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
		pLftEdge = new BYTE[abs(prcBinSrc->Height())];
		pTopEdge = new BYTE[abs(prcBinSrc->Width())];
		pRhtEdge = new BYTE[abs(prcBinSrc->Height())];
		pBotEdge = new BYTE[abs(prcBinSrc->Width())];
		nError = ClearImageROIEdgeAndBackup(pImgBinSrc,
								   prcBinSrc,
								   0,
								   &pLftEdge[0],
								   &pTopEdge[0],
								   &pRhtEdge[0],
								   &pBotEdge[0]);
		if(nError)
			goto ErrorAbort;
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
					if(pbyteBinSrcPtr[lIndex + nDir[k]] == 255) {
						plSeedsAddr2[lSeedsNo2++] = lIndex + nDir[k];
						pbyteBinSrcPtr[lIndex + nDir[k]] = 200;
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
					}
				}
			}
		}
		rcBinDst = CRect(nLft,
						 nTop,
						 nRht + 1,
						 nBot + 1);
		rcBinDstEdgeCheck = rcBinDst;
		rcBinDstEdgeCheck.InflateRect(1,1,1,1);
		if(ValidateRect(prcBinSrc, &rcBinDstEdgeCheck)) 
			goto PASS;

		if((rcBinDstEdgeCheck.left == prcBinSrc->left) ||
		   (rcBinDstEdgeCheck.top  == prcBinSrc->top) ||
		   (rcBinDstEdgeCheck.right == prcBinSrc->right) ||
		   (rcBinDstEdgeCheck.bottom == prcBinSrc->bottom)) {
			CRect rcUnion;
			CRect rcBinDst2;
			CRect rcBinDstFill;
			CRect rcBinDstExt;
			CRect rcBinDstFillUnion;
			nError = RestoreImageROIEdge(pImgBinSrc,
									   prcBinSrc,
									   &pLftEdge[0],
									   &pTopEdge[0],
									   &pRhtEdge[0],
									   &pBotEdge[0]);
			if(nError)
				goto ErrorAbort;
			nError = CheckEdgeConnection(pImgBinSrc,
										 prcBinSrc,
										 nBlobType,
										 pImgBinDst,
										 prcBinDstRelative,
										 pptBinDstTopLftInrcBinSrc);
			if(nError)
				goto ErrorAbort;
			if((prcBinDstRelative->Width() > 0) && (prcBinDstRelative->Height() > 0)) {
				rcBinDstExt = rcBinDst + CPoint(1,1);
				rcBinDst2 = CRect(*pptBinDstTopLftInrcBinSrc, prcBinDstRelative->Size());
				rcUnion.UnionRect(rcBinDstExt, rcBinDst2); 
			}
			else
				rcUnion = rcBinDst;

			*prcBinDstRelative = CRect(CPoint(0,0), rcUnion.Size());
			*pptBinDstTopLftInrcBinSrc = CPoint(rcUnion.left - 1, rcUnion.top - 1);
			
			rcBinDstFill = CRect(CPoint(1,1), rcBinDst.Size());
			rcBinDstFillUnion.UnionRect(rcBinDstFill, *prcBinDstRelative);

			pImgBinDst->Allocate(AbsSize(rcBinDstFillUnion.Size()));
			pImgBinDst->ClrImg(0);

			nError = CreateFloodFillImage(pImgBinSrc,
										 &rcBinDst,
										 nBlobType,
										 pImgBinDst,
										 &rcBinDstFill);
			if(nError)
				goto ErrorAbort;
			nError = AddEdgeFillPts(&ImgBinSrcExt,
									&rcUnion,
									&rcBinSrcExtRelative,
									pImgBinDst,
									prcBinDstRelative);
			if(nError)
				goto ErrorAbort;
		}
		else {
			*prcBinDstRelative = CRect(CPoint(0,0), rcBinDst.Size());
			*pptBinDstTopLftInrcBinSrc = rcBinDst.TopLeft();
			pImgBinDst->Allocate(AbsSize(prcBinDstRelative->Size()));
			pImgBinDst->ClrImg(0);
			nError = CreateFloodFillImage(pImgBinSrc,
										 &rcBinDst,
										 nBlobType,
										 pImgBinDst,
										 prcBinDstRelative);
			if(nError)
				goto ErrorAbort;
		}
		nError = RestoreImageROIEdge(pImgBinSrc,
								   prcBinSrc,
								   &pLftEdge[0],
								   &pTopEdge[0],
								   &pRhtEdge[0],
								   &pBotEdge[0]);
		if(nError)
			goto ErrorAbort;
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgFloodFill::GetFloodFillImage");
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


int CImgFloodFill::GetFloodFillImage(CImageBuffer *pImgBinSrc,
					 CRect* prcBinSrc,
					 CImageBuffer *pImgBinSeed,
					 CRect* prcBinSeed,
					 CPoint* pptBinSeedTopLftInrcBinSrc,
					 int nBlobType,
					 CImageBuffer *pImgBinDst,
					 CRect* prcBinSrcRelative)
{
	CRect rcCopy;
	CRect rcFilled;
	CPoint ptBinSeedTopLftInrcBinSrcExt;
	int nError;

try
{
	rcBinSrcExt = *prcBinSrc;
	rcBinSrcExt.InflateRect(1,1,1,1);
	rcBinSrcExtRelative = CRect(CPoint(0,0), rcBinSrcExt.Size());
	ImgBinSrcExt.Allocate(AbsSize(rcBinSrcExtRelative.Size()));
	rcCopy = CRect(CPoint(1,1), prcBinSrc->Size());
	nError = ImgCopy(pImgBinSrc,
					 prcBinSrc,
					 &ImgBinSrcExt,
					 &rcCopy);
	if(nError)
		goto ErrorAbort;
	nError = ClearImageROIEdge(&ImgBinSrcExt,
							   &rcBinSrcExtRelative,
							   0);
	if(nError)
		goto ErrorAbort;
	ptBinSeedTopLftInrcBinSrcExt = *pptBinSeedTopLftInrcBinSrc + CPoint(1,1);
	nError = GetFloodFillImageWithCleanBounding(&ImgBinSrcExt,
						 &rcBinSrcExtRelative,
						 pImgBinSeed,
						 prcBinSeed,
						 &ptBinSeedTopLftInrcBinSrcExt,
						 nBlobType,
						 &rcFilled);
	if(nError)
		goto ErrorAbort;
	pImgBinDst->Allocate(AbsSize(prcBinSrcRelative->Size()));
	pImgBinDst->ClrImg(0);
	nError = CreateFloodFillImage(&ImgBinSrcExt,
								 &rcCopy,
								 nBlobType,
								 pImgBinDst,
								 prcBinSrcRelative);
	if(nError)
		goto ErrorAbort;

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgFloodFill::GetFloodFillImage -1");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CImgFloodFill::GetFloodFillImage(CImageBuffer *pImgBinSrc,
					 CRect* prcBinSrc,
					 CImageBuffer *pImgBinSeed,
					 CRect* prcBinSeed,
					 CPoint* pptBinSeedTopLftInrcBinSrc,
					 int nBlobType,
					 CImageBuffer *pImgBinDst,
					 CRect* prcBinSrcRelative,
					 CRect* prcBlob)
{
	CRect rcCopy;
	CRect rcFilled;
	CPoint ptBinSeedTopLftInrcBinSrcExt;
	int nError;

try
{
	rcBinSrcExt = *prcBinSrc;
	rcBinSrcExt.InflateRect(1,1,1,1);
	rcBinSrcExtRelative = CRect(CPoint(0,0), rcBinSrcExt.Size());
	ImgBinSrcExt.Allocate(AbsSize(rcBinSrcExtRelative.Size()));
	rcCopy = CRect(CPoint(1,1), prcBinSrc->Size());
	nError = ImgCopy(pImgBinSrc,
					 prcBinSrc,
					 &ImgBinSrcExt,
					 &rcCopy);
	if(nError)
		goto ErrorAbort;
	nError = ClearImageROIEdge(&ImgBinSrcExt,
							   &rcBinSrcExtRelative,
							   0);
	if(nError)
		goto ErrorAbort;
	ptBinSeedTopLftInrcBinSrcExt = *pptBinSeedTopLftInrcBinSrc + CPoint(1,1);
	nError = GetFloodFillImageWithCleanBounding(&ImgBinSrcExt,
						 &rcBinSrcExtRelative,
						 pImgBinSeed,
						 prcBinSeed,
						 &ptBinSeedTopLftInrcBinSrcExt,
						 nBlobType,
						 &rcFilled);
	if(nError)
		goto ErrorAbort;
	pImgBinDst->Allocate(AbsSize(prcBinSrcRelative->Size()));
	pImgBinDst->ClrImg(0);
	nError = CreateFloodFillImage(&ImgBinSrcExt,
								 &rcCopy,
								 nBlobType,
								 pImgBinDst,
								 prcBinSrcRelative,
								 prcBlob);
	if(nError)
		goto ErrorAbort;

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgFloodFill::GetFloodFillImage -1");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CImgFloodFill::AddEdgeFillPts(CImageBuffer* pImgSrc,
								 CRect* prcBlobROI,
								 CRect* prcSrc,
								 CImageBuffer* pImgDst,
								 CRect* prcBinDst)
{
	int x, y;
	BYTE *pImgSrcPtr, *pImgDstPtr;
	int nPitchSrc, nPitchDst;
	int nIndexSrc1, nIndexDst1;
	int nIndexSrc2, nIndexDst2;
try
{

	pImgSrcPtr = pImgSrc->GetImgPtr();
	nPitchSrc = pImgSrc->GetPitch();
	pImgDstPtr = pImgDst->GetImgPtr();
	nPitchDst = pImgDst->GetPitch();

	nIndexSrc1 = prcBlobROI->top * nPitchSrc + prcBlobROI->left;
	nIndexDst1 = prcBinDst->top * nPitchDst + prcBinDst->left;
	nIndexSrc2 = prcBlobROI->top * nPitchSrc + prcBlobROI->left + prcBlobROI->Width() - 1;
	nIndexDst2 = prcBinDst->top * nPitchDst + prcBinDst->left + prcBinDst->Width() - 1;
	for(y = 0; y < prcBlobROI->Height(); y++) {
		if(pImgSrcPtr[nIndexSrc1] == 200)
			pImgDstPtr[nIndexDst1] = 255;
		if(pImgSrcPtr[nIndexSrc2] == 200)
			pImgDstPtr[nIndexDst2] = 255;
		nIndexSrc1 += nPitchSrc;
		nIndexDst1 += nPitchDst;
		nIndexSrc2 += nPitchSrc;
		nIndexDst2 += nPitchDst;
	}

	nIndexSrc1 = prcBlobROI->top * nPitchSrc + prcBlobROI->left;
	nIndexDst1 = prcBinDst->top * nPitchDst + prcBinDst->left;
	nIndexSrc2 = prcBlobROI->top * nPitchSrc + prcBlobROI->left + (prcBlobROI->Height() - 1) * nPitchSrc;
	nIndexDst2 = prcBinDst->top * nPitchDst + prcBinDst->left + (prcBlobROI->Height() - 1) * nPitchDst;
	for(x = 0; x < prcBlobROI->Width(); x++) {
		if(pImgSrcPtr[nIndexSrc1+x] == 200)
			pImgDstPtr[nIndexDst1+x] = 255;
		if(pImgSrcPtr[nIndexSrc2+x] == 200)
			pImgDstPtr[nIndexDst2+x] = 255;
	}

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgFloodFill::AddEdgeFillPts");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CImgFloodFill::CreateFloodFillImage(CImageBuffer *pImgBinSrc,
									  CRect* prcBinSrc,
									  int nBlobType,
									  CImageBuffer* pImgBinDst,
									  CRect* prcBinDst)
{
	int x, y;
	int nBlobWidth, nBlobHeight;
	int nBinSrcPitch;
	int nBinDstPitch;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;

try
{
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

	if (!pImgBinDst) {
		ASSERT(!"pImgBinDst is a bad Pointer");
		return -1;
	}
	if (!pImgBinDst->IsAllocated()) {
		ASSERT(!"pImgBinDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcBinDst,&(pImgBinDst->GetImgSize()))) {
		ASSERT(!"prcBinDst is Invalid");
		return -1;
	}
	if (prcBinSrc->Size()!=prcBinDst->Size()) {
		ASSERT(!"prcBinSrc Size different from prcBinDst Size");
		return -1;
	}

	nBlobWidth = prcBinDst->Width();
	nBlobHeight = prcBinDst->Height();
	nBinSrcPitch = pImgBinSrc->GetPitch();
	nBinDstPitch = pImgBinDst->GetPitch();
	pRowSrc = pColSrc = pImgBinSrc->GetImgPtr() + nBinSrcPitch * prcBinSrc->top + prcBinSrc->left;
	pRowDst = pColDst = pImgBinDst->GetImgPtr() + nBinDstPitch * prcBinDst->top + prcBinDst->left;
	for (y=0; y<nBlobHeight; y++,pRowSrc+=nBinSrcPitch,pRowDst+=nBinDstPitch) {
		for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nBlobWidth; x++,pColSrc++,pColDst++) {
			if(*pColSrc == 200) {
				*pColDst = 255;
				*pColSrc = 255; 
			}
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgFloodFill::CreateFloodFillImage");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CImgFloodFill::CreateFloodFillImage(CImageBuffer *pImgBinSrc,
									  CRect* prcBinSrc,
									  int nBlobType,
									  CImageBuffer* pImgBinDst,
									  CRect* prcBinDst,
									  CRect* prcBlob)
{
	int x, y;
	int nBlobWidth, nBlobHeight;
	int nBinSrcPitch;
	int nBinDstPitch;
	BYTE *pRowSrc, *pColSrc, *pRowDst, *pColDst;
	int nLeft, nTop, nRight, nBottom;
	int nx, ny;

try
{
	*prcBlob = CRect(0,0,0,0);
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

	if (!pImgBinDst) {
		ASSERT(!"pImgBinDst is a bad Pointer");
		return -1;
	}
	if (!pImgBinDst->IsAllocated()) {
		ASSERT(!"pImgBinDst is not Allocated");
		return -1;
	}
	if (ValidateRect(prcBinDst,&(pImgBinDst->GetImgSize()))) {
		ASSERT(!"prcBinDst is Invalid");
		return -1;
	}
	if (prcBinSrc->Size()!=prcBinDst->Size()) {
		ASSERT(!"prcBinSrc Size different from prcBinDst Size");
		return -1;
	}

	nBlobWidth = prcBinDst->Width();
	nBlobHeight = prcBinDst->Height();
	nBinSrcPitch = pImgBinSrc->GetPitch();
	nBinDstPitch = pImgBinDst->GetPitch();
	pRowSrc = pColSrc = pImgBinSrc->GetImgPtr() + nBinSrcPitch * prcBinSrc->top + prcBinSrc->left;
	pRowDst = pColDst = pImgBinDst->GetImgPtr() + nBinDstPitch * prcBinDst->top + prcBinDst->left;
	nLeft = prcBinDst->right;
	nTop = prcBinDst->bottom;
	nRight = -1;
	nBottom = -1;
	for (y=0; y<nBlobHeight; y++,pRowSrc+=nBinSrcPitch,pRowDst+=nBinDstPitch) {
		for (x=0,pColSrc=pRowSrc,pColDst=pRowDst; x<nBlobWidth; x++,pColSrc++,pColDst++) {
			if(*pColSrc == 200) {
				*pColDst = 255;
				*pColSrc = 255; 
				nx = x + prcBinDst->left;
				ny = y + prcBinDst->top;
				if(nLeft > nx)
					nLeft = nx;
				if(nTop > ny)
					nTop = ny;
				if(nRight < nx)
					nRight = nx;
				if(nBottom < ny)
					nBottom = ny;
			}
		}
	}
	if(nBottom >= 0)
	{
		*prcBlob = CRect(nLeft, nTop, nRight + 1, nBottom + 1);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgFloodFill::CreateFloodFillImage");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}
int CImgFloodFill::GetFloodFillImageWithCleanBounding(CImageBuffer *pImgBinSrc,
					 CRect* prcBinSrc,
					 CPoint* pptSeed,
					 int nSeed,
					 int nBlobType,
					 CRect* prcFilled)
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

try
{
	plSeedsAddr1 = NULL;
	plSeedsAddr2 = NULL;
	nLftSrc = prcBinSrc->left;
	nTopSrc = prcBinSrc->top;
	nRhtSrc = prcBinSrc->right - 1;
	nBotSrc = prcBinSrc->bottom - 1;
	prcFilled->SetRectEmpty();
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
			if((pbyteBinSrcPtr[lSeedIndex] == 255) || (pbyteBinSrcPtr[lSeedIndex] == 200)) {
				pbyteBinSrcPtr[lSeedIndex] = 200;
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
					if(pbyteBinSrcPtr[lIndex + nDir[k]] == 255) {
						plSeedsAddr2[lSeedsNo2++] = lIndex + nDir[k];
						pbyteBinSrcPtr[lIndex + nDir[k]] = 200;
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
					}
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
		plSeedsAddr1 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
		plSeedsAddr2 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
		lSeedsNo1 = 0;
		for(m = 0; m < nSeed; m++) {
			if(ValidatePoint(&pptSeed[m], prcBinSrc)) 
				continue;
			lSeedIndex = pptSeed[m].y * nBinSrcPitch + pptSeed[m].x;
			if((pbyteBinSrcPtr[lSeedIndex] == 0) || (pbyteBinSrcPtr[lSeedIndex] == 200)) {
				pbyteBinSrcPtr[lSeedIndex] = 200;
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
					if(pbyteBinSrcPtr[lIndex + nDir[k]] == 0) {
						plSeedsAddr2[lSeedsNo2++] = lIndex + nDir[k];
						pbyteBinSrcPtr[lIndex + nDir[k]] = 200;
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
					if(pbyteBinSrcPtr[lIndex + nDir[k]] == 0) {
						plSeedsAddr1[lSeedsNo1++] = lIndex + nDir[k];
						pbyteBinSrcPtr[lIndex + nDir[k]] = 200;
					}
				}
			}
		}
	}
	*prcFilled = CRect(nLft, nTop, nRht + 1, nBot + 1);

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgFloodFill::GetFloodFillImageWithCleanBounding");
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

int CImgFloodFill::GetFloodFillImageWithCleanBounding(CImageBuffer *pImgBinSrc,
						 CRect* prcBinSrc,
						 CImageBuffer *pImgBinSeed,
						 CRect* prcBinSeed,
						 CPoint* pptBinSeedTopLftInrcBinSrc,
						 int nBlobType,
						 CRect* prcFilled)
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
	int nError;
	int nDir[8];
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

try
{
	plSeedsAddr1 = NULL;
	plSeedsAddr2 = NULL;
	nLftSrc = prcBinSrc->left;
	nTopSrc = prcBinSrc->top;
	nRhtSrc = prcBinSrc->right - 1;
	nBotSrc = prcBinSrc->bottom - 1;
	prcFilled->SetRectEmpty();
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
		else 
			goto PASS;
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
					}
				}
			}
		}
	}
	*prcFilled = CRect(nLft, nTop, nRht + 1, nBot + 1);
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgFloodFill::GetFloodFillImageWithCleanBounding -1");
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

int CImgFloodFill::GetFloodFillImage(CImageBuffer *pImgBinSrc,
					 CRect* prcBinSrc,
					 CPoint* pptSeed,
					 int nSeed,
					 int nBlobType,
					 BYTE byteFillVal,
					 CRect* prcFill)
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

try
{
	plSeedsAddr1 = NULL;
	plSeedsAddr2 = NULL;
	nLftSrc = prcBinSrc->left;
	nTopSrc = prcBinSrc->top;
	nRhtSrc = prcBinSrc->right - 1;
	nBotSrc = prcBinSrc->bottom - 1;
	*prcFill = CRect(0,0,0,0);
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
			if(pbyteBinSrcPtr[lSeedIndex] == 255) {
				pbyteBinSrcPtr[lSeedIndex] = byteFillVal;
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
			goto PASS;
		}
		while(lSeedsNo1 > 0) {
			lSeedsNo2 = 0;
			for(lm = 0; lm < lSeedsNo1; lm++) {
				lIndex = plSeedsAddr1[lm];
				pbyteBinSrcPtr[lIndex] = byteFillVal;
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
						pbyteBinSrcPtr[lIndex + nDir[k]] = byteFillVal;
					}
				}
			}
			if(lSeedsNo2 == 0)
				break;
			lSeedsNo1 = 0;
			for(lm = 0; lm < lSeedsNo2; lm++) {
				lIndex = plSeedsAddr2[lm];
				pbyteBinSrcPtr[lIndex] = byteFillVal;
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
						pbyteBinSrcPtr[lIndex + nDir[k]] = byteFillVal;
					}
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
		plSeedsAddr1 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
		plSeedsAddr2 = new long[abs(prcBinSrc->Width() * prcBinSrc->Height())];
		lSeedsNo1 = 0;
		for(m = 0; m < nSeed; m++) {
			if(ValidatePoint(&pptSeed[m], prcBinSrc)) 
				continue;
			lSeedIndex = pptSeed[m].y * nBinSrcPitch + pptSeed[m].x;
			if(pbyteBinSrcPtr[lSeedIndex] == 0) {
				pbyteBinSrcPtr[lSeedIndex] = byteFillVal;
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
			goto PASS;
		}
		while(lSeedsNo1 > 0) {
			lSeedsNo2 = 0;
			for(lm = 0; lm < lSeedsNo1; lm++) {
				lIndex = plSeedsAddr1[lm];
				pbyteBinSrcPtr[lIndex] = byteFillVal;
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
					if(pbyteBinSrcPtr[lIndex + nDir[k]] == 0) {
						plSeedsAddr2[lSeedsNo2++] = lIndex + nDir[k];
						pbyteBinSrcPtr[lIndex + nDir[k]] = byteFillVal;
					}
				}
			}
			if(lSeedsNo2 == 0)
				break;
			lSeedsNo1 = 0;
			for(lm = 0; lm < lSeedsNo2; lm++) {
				lIndex = plSeedsAddr2[lm];
				pbyteBinSrcPtr[lIndex] = byteFillVal;
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
					if(pbyteBinSrcPtr[lIndex + nDir[k]] == 0) {
						plSeedsAddr1[lSeedsNo1++] = lIndex + nDir[k];
						pbyteBinSrcPtr[lIndex + nDir[k]] = byteFillVal;
					}
				}
			}
		}
	}
	*prcFill = CRect(nLft,
					 nTop,
					 nRht + 1,
					 nBot + 1);
	prcFill->InflateRect(1,1,1,1);
	CheckRect(prcBinSrc, prcFill);

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgFloodFill::GetFloodFillImage -2");
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

#ifndef _FLOOD_FILL
#define _FLOOD_FILL

#include "ImageBuffer.h"
#include "Imaging.h"

int FillImageSmallHole(IMAGEVIEW_INFO* pView,
				CImageBuffer* pImgBin,
				CRect* prcBin,
				CSize* pcsFillMaxHole,
				int nBlobColor,
				long lDebugFlag);

int FillImageHole(IMAGEVIEW_INFO* pView,
				CImageBuffer* pImgBin,
				CRect* prcBin,
				int nMinArea,
				int nMinSize,
				long lDebugFlag);

int FillImageHole(IMAGEVIEW_INFO* pView,
				CImageBuffer* pImgBin,
				CRect* prcBin,
				long lDebugFlag);

int FillImageHole(IMAGEVIEW_INFO* pView,
				CImageBuffer* pImgBin,
				CRect* prcBin,
				int nMaxDiameter,
				long lDebugFlag);


int FillImageHole(IMAGEVIEW_INFO* pView,
				CImageBuffer* pImgBin,
				CRect* prcBin,
				CSize* pcsMaxHole,
				long lDebugFlag);


class CImgFloodFill
{
public:
	CImgFloodFill();
	~CImgFloodFill();
	void Clean();
	int GetFloodFillImage(CImageBuffer *pImgBinSrc,
						 CRect* prcBinSrc,
						 CImageBuffer *pImgBinSeed,
						 CRect* prcBinSeed,
						 CPoint* pptBinSeedTopLftInrcBinSrc,
						 int nBlobType,
						 CImageBuffer *pImgBinDst,
						 CRect* prcBinDstRelative,
						 CPoint* pptBinDstTopLftInrcBinSrc);

	int GetFloodFillImage(CImageBuffer *pImgBinSrc,
						 CRect* prcBinSrc,
						 CImageBuffer *pImgBinSeed,
						 CRect* prcBinSeed,
						 CPoint* pptBinSeedTopLftInrcBinSrc,
						 int nBlobType,
						 CImageBuffer *pImgBinDst,
						 CRect* prcBinSrcRelative);

	int GetFloodFillImage(CImageBuffer *pImgBinSrc,
						 CRect* prcBinSrc,
						 CImageBuffer *pImgBinSeed,
						 CRect* prcBinSeed,
						 CPoint* pptBinSeedTopLftInrcBinSrc,
						 int nBlobType,
						 CImageBuffer *pImgBinDst,
						 CRect* prcBinSrcRelative,
						 CRect* prcBlob);

	int GetFloodFillImage(CImageBuffer *pImgBinSrc,
					 CRect* prcBinSrc,
					 CPoint* pptSeed,
					 int nSeed,
					 int nBlobType,
					 BYTE byteFillVal,
					 CRect* prcFill);

private:
	BYTE *pLftEdge, *pTopEdge, *pRhtEdge, *pBotEdge;
	CImageBuffer ImgBinSrcExt;
	CRect rcBinSrcExtRelative;
	CRect rcBinSrcExt;
	int nLftSrc;
	int nTopSrc;
	int nRhtSrc;
	int nBotSrc;

private:
	int ClearImageROIEdgeAndBackup(CImageBuffer *pImgSrc,
					   CRect* prcROISrc,
					   BYTE byteLable,
					   BYTE* pLftEdge,
					   BYTE* pTopEdge,
					   BYTE* pRhtEdge,
					   BYTE* pBotEdge);

	int RestoreImageROIEdge(CImageBuffer *pImgSrc,
					   CRect* prcROISrc,
					   BYTE* pLftEdge,
					   BYTE* pTopEdge,
					   BYTE* pRhtEdge,
					   BYTE* pBotEdge);

	int CheckEdgeConnection(CImageBuffer *pImgBinSrc,
							CRect *prcBinSrc,
							int nBlobType,
							CImageBuffer *pImgBinDst,
							CRect *prcBinDstRelative,
							CPoint *pptBinDstTopLftInrcBinSrc);

	int GetFloodFillImageWithCleanBounding(CImageBuffer *pImgBinSrc,
					 CRect* prcBinSrc,
					 CPoint* pptSeed,
					 int nSeed,
					 int nBlobType,
					 CRect* prcFilled);

	int GetFloodFillImageWithCleanBounding(CImageBuffer *pImgBinSrc,
						 CRect* prcBinSrc,
						 CImageBuffer *pImgBinSeed,
						 CRect* prcBinSeed,
						 CPoint* pptBinSeedTopLftInrcBinSrc,
						 int nBlobType,
						 CRect* prcFilled);

	int GetEdgeSeed(CImageBuffer *pImgBinSrcExt,
					 CRect* prcBinSrcExtRelative,
					 CPoint* pptSeed,
					 int* pnSeedNo);

	int CreateFloodFillImage(CImageBuffer *pImgBinSrc,
						  CRect* prcBinSrc,
						  int nBlobType,
						  CImageBuffer* pImgBinDst,
						  CRect* prcBinDst);

	int CreateFloodFillImage(CImageBuffer *pImgBinSrc,
						  CRect* prcBinSrc,
						  int nBlobType,
						  CImageBuffer* pImgBinDst,
						  CRect* prcBinDst,
						  CRect* prcBlob);

	int AddEdgeFillPts(CImageBuffer* pImgBinSrcExt,
					 CRect* prcUnion,
					 CRect* prcBinSrcExtRelative,
					 CImageBuffer* pImgBinDst,
					 CRect* prcBinDstRelative);

};


#endif
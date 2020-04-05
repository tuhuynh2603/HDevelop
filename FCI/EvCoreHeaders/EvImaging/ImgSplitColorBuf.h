#pragma once

#include "Imaging.h"

typedef struct {
	HANDLE m_hRun;
	HANDLE m_hDone;

	CImageBuffer *pImgBufSrc;
	CRect rcSrc;
	CImageBuffer *pImgDst;
	CImageBuffer *pImgDstInvert;
	CRect rcDst;
	CImageBuffer *pImgMask;
	CRect rcMask;
	long lDebugFlag;
	int nReserved;

	int nError;
} INSP_MT_PARM;

UINT ImgSplitColorThread0(LPVOID pParam);
UINT ImgSplitColorThread1(LPVOID pParam);
UINT ImgSplitColorThread2(LPVOID pParam);
UINT ImgSplitColorThread3(LPVOID pParam);

UINT ImgIntensityThread0(LPVOID pParam);
UINT ImgIntensityThread1(LPVOID pParam);
UINT ImgIntensityThread2(LPVOID pParam);
UINT ImgIntensityThread3(LPVOID pParam);

UINT ImgComposeColorThread0(LPVOID pParam);
UINT ImgComposeColorThread1(LPVOID pParam);
UINT ImgComposeColorThread2(LPVOID pParam);
UINT ImgComposeColorThread3(LPVOID pParam);


class CImgSplitColorBuf
{
public:
	CImageBuffer Img0;
	CImageBuffer Img1;
	CImageBuffer Img2;
	CRect rcROI;


	BOOL bUseMultiThreading;
	int nMultiThreadingRow;
	int nMultiThreadingCol;
	BOOL m_bKillInspectThreads;
	INSP_MT_PARM m_InspMTIntensityParm[10];
	INSP_MT_PARM m_InspMTSplitParm[10];
	INSP_MT_PARM m_InspMTComposeParm[10];
	CRect rcSegmentSrc[10];
	CRect rcSegmentDst[10];
	int nSegmentNo;

public:
	CImgSplitColorBuf();
	~CImgSplitColorBuf();
	int GetIntensityImg(CImageBuffer* pImgColor, 
						CRect* prcColor,
						CImageBuffer* pImgIntensity, 
						CImageBuffer* pImgIntensityInvert, 
						CRect* prcRelative);
	int GetIntensityImgWithoutMT(CImageBuffer* pImgColor, 
						CRect* prcColor,
						CImageBuffer* pImgIntensity, 
						CImageBuffer* pImgIntensityInvert, 
						CRect* prcRelative);

	int ColorSplit(CImageBuffer* pImgColor, CRect* prcColor);
	int ColorSplitWithoutMT(CImageBuffer* pImgColor, CRect* prcSrc, CRect* prcDst);
	int ColorCompose(CImageBuffer* pImgColor, CRect* prcColor = NULL);
	int ColorComposeWithoutMT(CImageBuffer* pImgColor, CRect* prcSrc, CRect* prcDst);

private:
	BOOL m_bInitialized;
	void Initialize();

	int GetSplitRect(CRect* prcColor);
};

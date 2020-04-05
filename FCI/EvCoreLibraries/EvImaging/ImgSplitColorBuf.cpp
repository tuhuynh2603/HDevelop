#include "StdAfx.h"
#include "ImgSplitColorBuf.h"
#include "DebugLog.h"


UINT ImgComposeColorThread0(LPVOID pParam)
{
	

	CImgSplitColorBuf* pPVI = (CImgSplitColorBuf *)pParam;
	INSP_MT_PARM* parm = &pPVI->m_InspMTComposeParm[0];

try {
	while(1) {
		WaitForSingleObject(parm->m_hRun, INFINITE);
		if(pPVI->m_bKillInspectThreads)
			goto PASS;
		parm->nError = 0;
		parm->nReserved = 0;

		parm->nError = 
			pPVI->ColorComposeWithoutMT(parm->pImgBufSrc,
				&parm->rcSrc,
				&parm->rcDst);
		SetEvent(parm->m_hDone);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgComposeColorThread0");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}


UINT ImgComposeColorThread1(LPVOID pParam)
{
	

	CImgSplitColorBuf* pPVI = (CImgSplitColorBuf *)pParam;
	INSP_MT_PARM* parm = &pPVI->m_InspMTComposeParm[1];

try {
	while(1) {
		WaitForSingleObject(parm->m_hRun, INFINITE);
		if(pPVI->m_bKillInspectThreads)
			goto PASS;
		parm->nError = 0;
		parm->nReserved = 0;

		parm->nError = 
			pPVI->ColorComposeWithoutMT(parm->pImgBufSrc,
				&parm->rcSrc,
				&parm->rcDst);
		SetEvent(parm->m_hDone);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgComposeColorThread1");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

UINT ImgComposeColorThread2(LPVOID pParam)
{
	

	CImgSplitColorBuf* pPVI = (CImgSplitColorBuf *)pParam;
	INSP_MT_PARM* parm = &pPVI->m_InspMTComposeParm[2];

try {
	while(1) {
		WaitForSingleObject(parm->m_hRun, INFINITE);
		if(pPVI->m_bKillInspectThreads)
			goto PASS;
		parm->nError = 0;
		parm->nReserved = 0;

		parm->nError = 
			pPVI->ColorComposeWithoutMT(parm->pImgBufSrc,
				&parm->rcSrc,
				&parm->rcDst);
		SetEvent(parm->m_hDone);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgComposeColorThread2");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

UINT ImgComposeColorThread3(LPVOID pParam)
{
	

	CImgSplitColorBuf* pPVI = (CImgSplitColorBuf *)pParam;
	INSP_MT_PARM* parm = &pPVI->m_InspMTComposeParm[3];

try {
	while(1) {
		WaitForSingleObject(parm->m_hRun, INFINITE);
		if(pPVI->m_bKillInspectThreads)
			goto PASS;
		parm->nError = 0;
		parm->nReserved = 0;

		parm->nError = 
			pPVI->ColorComposeWithoutMT(parm->pImgBufSrc,
				&parm->rcSrc,
				&parm->rcDst);
		SetEvent(parm->m_hDone);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgComposeColorThread3");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

UINT ImgIntensityThread0(LPVOID pParam)
{
	CImgSplitColorBuf* pPVI = (CImgSplitColorBuf *)pParam;
	INSP_MT_PARM* parm = &pPVI->m_InspMTIntensityParm[0];

try {
	while(1) {
		WaitForSingleObject(parm->m_hRun, INFINITE);
		if(pPVI->m_bKillInspectThreads)
			goto PASS;
		parm->nError = 0;
		parm->nReserved = 0;

		parm->nError = 
			pPVI->GetIntensityImgWithoutMT(parm->pImgBufSrc,
				&parm->rcSrc,
				parm->pImgDst,
				parm->pImgDstInvert,
				&parm->rcDst);
		SetEvent(parm->m_hDone);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgIntensityThread0");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

UINT ImgIntensityThread1(LPVOID pParam)
{
	CImgSplitColorBuf* pPVI = (CImgSplitColorBuf *)pParam;
	INSP_MT_PARM* parm = &pPVI->m_InspMTIntensityParm[1];

try {
	while(1) {
		WaitForSingleObject(parm->m_hRun, INFINITE);
		if(pPVI->m_bKillInspectThreads)
			goto PASS;
		parm->nError = 0;
		parm->nReserved = 0;

		parm->nError = 
			pPVI->GetIntensityImgWithoutMT(parm->pImgBufSrc,
				&parm->rcSrc,
				parm->pImgDst,
				parm->pImgDstInvert,
				&parm->rcDst);
		SetEvent(parm->m_hDone);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgIntensityThread1");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

UINT ImgIntensityThread2(LPVOID pParam)
{
	CImgSplitColorBuf* pPVI = (CImgSplitColorBuf *)pParam;
	INSP_MT_PARM* parm = &pPVI->m_InspMTIntensityParm[2];

try {
	while(1) {
		WaitForSingleObject(parm->m_hRun, INFINITE);
		if(pPVI->m_bKillInspectThreads)
			goto PASS;
		parm->nError = 0;
		parm->nReserved = 0;

		parm->nError = 
			pPVI->GetIntensityImgWithoutMT(parm->pImgBufSrc,
				&parm->rcSrc,
				parm->pImgDst,
				parm->pImgDstInvert,
				&parm->rcDst);
		SetEvent(parm->m_hDone);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgIntensityThread2");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

UINT ImgIntensityThread3(LPVOID pParam)
{
	CImgSplitColorBuf* pPVI = (CImgSplitColorBuf *)pParam;
	INSP_MT_PARM* parm = &pPVI->m_InspMTIntensityParm[3];

try {
	while(1) {
		WaitForSingleObject(parm->m_hRun, INFINITE);
		if(pPVI->m_bKillInspectThreads)
			goto PASS;
		parm->nError = 0;
		parm->nReserved = 0;

		parm->nError = 
			pPVI->GetIntensityImgWithoutMT(parm->pImgBufSrc,
				&parm->rcSrc,
				parm->pImgDst,
				parm->pImgDstInvert,
				&parm->rcDst);
		SetEvent(parm->m_hDone);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgIntensityThread3");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

UINT ImgSplitColorThread0(LPVOID pParam)
{
	

	CImgSplitColorBuf* pPVI = (CImgSplitColorBuf *)pParam;
	INSP_MT_PARM* parm = &pPVI->m_InspMTSplitParm[0];

try {
	while(1) {
		WaitForSingleObject(parm->m_hRun, INFINITE);
		if(pPVI->m_bKillInspectThreads)
			goto PASS;
		parm->nError = 0;
		parm->nReserved = 0;

		parm->nError = 
			pPVI->ColorSplitWithoutMT(parm->pImgBufSrc,
				&parm->rcSrc,
				&parm->rcDst);
		SetEvent(parm->m_hDone);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgSplitColorThread0");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}


UINT ImgSplitColorThread1(LPVOID pParam)
{
	

	CImgSplitColorBuf* pPVI = (CImgSplitColorBuf *)pParam;
	INSP_MT_PARM* parm = &pPVI->m_InspMTSplitParm[1];

try {
	while(1) {
		WaitForSingleObject(parm->m_hRun, INFINITE);
		if(pPVI->m_bKillInspectThreads)
			goto PASS;
		parm->nError = 0;
		parm->nReserved = 0;

		parm->nError = 
			pPVI->ColorSplitWithoutMT(parm->pImgBufSrc,
				&parm->rcSrc,
				&parm->rcDst);
		SetEvent(parm->m_hDone);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgSplitColorThread1");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

UINT ImgSplitColorThread2(LPVOID pParam)
{
	

	CImgSplitColorBuf* pPVI = (CImgSplitColorBuf *)pParam;
	INSP_MT_PARM* parm = &pPVI->m_InspMTSplitParm[2];

try {
	while(1) {
		WaitForSingleObject(parm->m_hRun, INFINITE);
		if(pPVI->m_bKillInspectThreads)
			goto PASS;
		parm->nError = 0;
		parm->nReserved = 0;

		parm->nError = 
			pPVI->ColorSplitWithoutMT(parm->pImgBufSrc,
				&parm->rcSrc,
				&parm->rcDst);
		SetEvent(parm->m_hDone);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgSplitColorThread2");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

UINT ImgSplitColorThread3(LPVOID pParam)
{
	

	CImgSplitColorBuf* pPVI = (CImgSplitColorBuf *)pParam;
	INSP_MT_PARM* parm = &pPVI->m_InspMTSplitParm[3];

try {
	while(1) {
		WaitForSingleObject(parm->m_hRun, INFINITE);
		if(pPVI->m_bKillInspectThreads)
			goto PASS;
		parm->nError = 0;
		parm->nReserved = 0;

		parm->nError = 
			pPVI->ColorSplitWithoutMT(parm->pImgBufSrc,
				&parm->rcSrc,
				&parm->rcDst);
		SetEvent(parm->m_hDone);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ImgSplitColorThread3");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}


CImgSplitColorBuf::CImgSplitColorBuf()
{
	bUseMultiThreading = FALSE;
	m_bInitialized = FALSE;
}

///Udupa; Feb'11; Late thread binding
void CImgSplitColorBuf::Initialize()
{
	if(m_bInitialized)
		return;

	int i;

try
{
	nMultiThreadingRow = 2;
	nMultiThreadingCol = 2;

	for(i=0;i<4;i++) {
		m_InspMTSplitParm[i].m_hRun = CreateEventA(NULL, FALSE, FALSE, NULL); 
		m_InspMTSplitParm[i].m_hDone = CreateEventA(NULL, FALSE, FALSE, NULL); 
		m_InspMTSplitParm[i].nError = -1;
	}
	for(i=0;i<4;i++) {
		m_InspMTIntensityParm[i].m_hRun = CreateEventA(NULL, FALSE, FALSE, NULL); 
		m_InspMTIntensityParm[i].m_hDone = CreateEventA(NULL, FALSE, FALSE, NULL); 
		m_InspMTIntensityParm[i].nError = -1;
	}

	for(i=0;i<4;i++) {
		m_InspMTComposeParm[i].m_hRun = CreateEventA(NULL, FALSE, FALSE, NULL); 
		m_InspMTComposeParm[i].m_hDone = CreateEventA(NULL, FALSE, FALSE, NULL); 
		m_InspMTComposeParm[i].nError = -1;
	}


	m_bKillInspectThreads = FALSE;

	AfxBeginThread(ImgSplitColorThread0, this, THREAD_PRIORITY_NORMAL);
	AfxBeginThread(ImgSplitColorThread1, this, THREAD_PRIORITY_NORMAL);
	AfxBeginThread(ImgSplitColorThread2, this, THREAD_PRIORITY_NORMAL);
	AfxBeginThread(ImgSplitColorThread3, this, THREAD_PRIORITY_NORMAL);

	AfxBeginThread(ImgIntensityThread0, this, THREAD_PRIORITY_NORMAL);
	AfxBeginThread(ImgIntensityThread1, this, THREAD_PRIORITY_NORMAL);
	AfxBeginThread(ImgIntensityThread2, this, THREAD_PRIORITY_NORMAL);
	AfxBeginThread(ImgIntensityThread3, this, THREAD_PRIORITY_NORMAL);

	AfxBeginThread(ImgComposeColorThread0, this, THREAD_PRIORITY_NORMAL);
	AfxBeginThread(ImgComposeColorThread1, this, THREAD_PRIORITY_NORMAL);
	AfxBeginThread(ImgComposeColorThread2, this, THREAD_PRIORITY_NORMAL);
	AfxBeginThread(ImgComposeColorThread3, this, THREAD_PRIORITY_NORMAL);

	m_bInitialized = TRUE;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgSplitColorBuf::Initialize");
	goto ErrorAbort;
}
	return;
ErrorAbort:
	return;
}
	
CImgSplitColorBuf::~CImgSplitColorBuf()
{
	m_bKillInspectThreads = TRUE;
	SetEvent(m_InspMTSplitParm[0].m_hRun);
	SetEvent(m_InspMTSplitParm[1].m_hRun);
	SetEvent(m_InspMTSplitParm[2].m_hRun);
	SetEvent(m_InspMTSplitParm[3].m_hRun);

	SetEvent(m_InspMTIntensityParm[0].m_hRun);
	SetEvent(m_InspMTIntensityParm[1].m_hRun);
	SetEvent(m_InspMTIntensityParm[2].m_hRun);
	SetEvent(m_InspMTIntensityParm[3].m_hRun);

	SetEvent(m_InspMTComposeParm[0].m_hRun);
	SetEvent(m_InspMTComposeParm[1].m_hRun);
	SetEvent(m_InspMTComposeParm[2].m_hRun);
	SetEvent(m_InspMTComposeParm[3].m_hRun);

}

int CImgSplitColorBuf::GetSplitRect(CRect* prcColor)
{
	int i, j;
	double dStepX;
	double dStepY;
	double dStartPosX;
	double dStartPosY;
	double dEndPosX;
	double dEndPosY;

try
{
	dStepX = (double)prcColor->Width()/nMultiThreadingCol;
	dStepY = (double)prcColor->Height()/nMultiThreadingRow;
	nSegmentNo = 0;
	for(i=0;i<nMultiThreadingRow;i++)
	{
		dStartPosY = i * dStepY;
		dEndPosY = dStartPosY + dStepY;
		for(j=0;j<nMultiThreadingCol;j++)
		{
			dStartPosX = j * dStepX;
			dEndPosX = dStartPosX + dStepX;
			rcSegmentDst[nSegmentNo] = CRect(Round(dStartPosX),
										  Round(dStartPosY),
										  Round(dEndPosX),
										  Round(dEndPosY));
			rcSegmentSrc[nSegmentNo] = prcColor->TopLeft() + rcSegmentDst[nSegmentNo];
			nSegmentNo++;
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgSplitColorBuf::GetSplitRect");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CImgSplitColorBuf::GetIntensityImgWithoutMT(CImageBuffer* pImgColor, 
												CRect* prcColor,
												CImageBuffer* pImgIntensity, 
												CImageBuffer* pImgIntensityInvert, 
												CRect* prcRelative)
{
	int i, j;
	int nColorPitch;
	BYTE* pImgColorPtr;
	long lColorIndex;
	long lColorIndexRow;
	int nPitch;
	BYTE* pImgIntensityPtr;
	BYTE* pImgIntensityInvertPtr;
	long lIndex;
	CRect rcROI;

try
{
	nColorPitch = pImgColor->GetPitch();
	pImgColorPtr = pImgColor->GetImgPtr();
	nPitch = pImgIntensity->GetPitch();
	pImgIntensityPtr = pImgIntensity->GetImgPtr();
	pImgIntensityInvertPtr = pImgIntensityInvert->GetImgPtr();
	lIndex = prcRelative->top * nPitch + prcRelative->left;
	lColorIndexRow = prcColor->top * nColorPitch + prcColor->left * 3;
	for(i=0;i<prcRelative->Height();i++,lIndex += nPitch,lColorIndexRow += nColorPitch) {
		lColorIndex = lColorIndexRow;
		for(j=0;j<prcRelative->Width();j++) {
			pImgIntensityPtr[lIndex+j] = (pImgColorPtr[lColorIndex++] + pImgColorPtr[lColorIndex++] + pImgColorPtr[lColorIndex++])/3;
			pImgIntensityInvertPtr[lIndex+j] = 255 - pImgIntensityPtr[lIndex+j];
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgSplitColorBuf::ColorSplitWithoutMT");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CImgSplitColorBuf::GetIntensityImg(CImageBuffer* pImgColor, 
									   CRect* prcColor,
									   CImageBuffer* pImgIntensity, 
									   CImageBuffer* pImgIntensityInvert, 
									   CRect* prcRelative)
{
	int i, k;
	int nError;
try
{
	if(bUseMultiThreading == FALSE)
	{
		nError = GetIntensityImgWithoutMT(pImgColor, 
							prcColor, 
							pImgIntensity,
							pImgIntensityInvert,
							prcRelative);
		if(nError)
		goto ErrorAbort;
	}
	else
	{
		HANDLE handles[4];
		Initialize();
		nMultiThreadingRow = 8;
		nMultiThreadingCol = 1;
		nError = GetSplitRect(prcColor);

		for(k=0;k<2;k++)
		{
			m_bKillInspectThreads = FALSE;
			for(i=0;i<4;i++)
			{
				m_InspMTIntensityParm[i].pImgBufSrc = pImgColor;
				m_InspMTIntensityParm[i].rcSrc = rcSegmentSrc[2 * i + k];
				m_InspMTIntensityParm[i].pImgDst = pImgIntensity;
				m_InspMTIntensityParm[i].pImgDstInvert = pImgIntensityInvert;
				m_InspMTIntensityParm[i].rcDst = rcSegmentDst[2 * i + k];
	/*
				nError = GetIntensityImgWithoutMT(pImgColor, 
									&m_InspMTIntensityParm[i].rcSrc, 
									m_InspMTIntensityParm[i].pImgDst,
									m_InspMTIntensityParm[i].pImgDstInvert,
									&m_InspMTIntensityParm[i].rcDst);
	*/
			}
			for(i=0;i<4;i++)
			{
				ResetEvent(m_InspMTIntensityParm[i].m_hDone);
				SetEvent(m_InspMTIntensityParm[i].m_hRun);
			}

			for(i=0;i<4;i++)
			{
				handles[i] = m_InspMTIntensityParm[i].m_hDone;
			}
			WaitForMultipleObjects(4, handles, TRUE, INFINITE);
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgSplitColorBuf::GetIntensityImg");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}



int CImgSplitColorBuf::ColorSplit(CImageBuffer* pImgColor, CRect* prcColor)
{
	if(!pImgColor || !pImgColor->IsAllocated()) {
		ASSERT(!"CImgSplitColorBuf::ColorSplit Source buffer is invalid");
		return -1;
	}

	int i;
	int nError;

try
{
	rcROI = CRect(CPoint(0,0), prcColor->Size());
	CheckRect(&rcROI, &pImgColor->GetImgSize());
	Img0.Allocate(rcROI.Size());
	Img1.Allocate(rcROI.Size());
	Img2.Allocate(rcROI.Size());

	if(bUseMultiThreading == FALSE)
	{
		nError = ColorSplitWithoutMT(pImgColor, prcColor, &rcROI);
		if(nError)
			goto ErrorAbort;
	}
	else
	{
		int k;
		Initialize();
		nMultiThreadingRow = 8;
		nMultiThreadingCol = 1;
		nError = GetSplitRect(prcColor);

		for(k=0;k<2;k++)
		{
			m_bKillInspectThreads = FALSE;
			for(i=0;i<4;i++)
			{
				m_InspMTSplitParm[i].pImgBufSrc = pImgColor;
				m_InspMTSplitParm[i].rcSrc = rcSegmentSrc[i * 2 + k];
				m_InspMTSplitParm[i].rcDst = rcSegmentDst[i * 2 + k];
			}

			for(i=0;i<4;i++)
			{
				ResetEvent(m_InspMTSplitParm[i].m_hDone);
				SetEvent(m_InspMTSplitParm[i].m_hRun);
			}

			HANDLE handles[4];
			for(i=0;i<4;i++)
			{
				handles[i] = m_InspMTSplitParm[i].m_hDone;
			}
			WaitForMultipleObjects(4, handles, TRUE, INFINITE);
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgSplitColorBuf::ColorSplit");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CImgSplitColorBuf::ColorSplitWithoutMT(CImageBuffer* pImgColor, 
										   CRect* prcSrc,
										   CRect* prcDst)
{
	int i, j;
	int nColorPitch;
	BYTE* pImgColorPtr;
	long lColorIndex;
	long lColorIndexRow;
	int nPitch;
	BYTE* pImg0Ptr;
	BYTE* pImg1Ptr;
	BYTE* pImg2Ptr;
	long lIndex;
	long lIndexRow;
	CRect rcROI;

try
{
	nColorPitch = pImgColor->GetPitch();
	pImgColorPtr = pImgColor->GetImgPtr();
	rcROI = *prcDst;
	nPitch = Img0.GetPitch();
	pImg0Ptr = Img0.GetImgPtr();
	pImg1Ptr = Img1.GetImgPtr();
	pImg2Ptr = Img2.GetImgPtr();
	lIndexRow = rcROI.top * nPitch + rcROI.left;
	lColorIndexRow = prcSrc->top * nColorPitch + prcSrc->left * 3;
	for(i=0;i<rcROI.Height();i++,lIndexRow += nPitch,lColorIndexRow += nColorPitch) {
		lIndex = lIndexRow;
		lColorIndex = lColorIndexRow;
		for(j=0;j<rcROI.Width();j++) {
			pImg2Ptr[lIndex+j] = pImgColorPtr[lColorIndex++];
			pImg1Ptr[lIndex+j] = pImgColorPtr[lColorIndex++];
			pImg0Ptr[lIndex+j] = pImgColorPtr[lColorIndex++];
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgSplitColorBuf::ColorSplitWithoutMT");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}
int CImgSplitColorBuf::ColorCompose(CImageBuffer* pImgColor, CRect* prcColor)
{

	int i;
	CRect rcColorSrc;
	int nError;

try
{
	if(prcColor == NULL) {
		rcColorSrc = rcROI;
	}
	else
	{
		rcColorSrc = *prcColor;
	}
	if(pImgColor == NULL) {
		pImgColor->Allocate(rcColorSrc.Size(), false, TRUE);
		pImgColor->ClrImg(0);
	}

	if(bUseMultiThreading == FALSE)
	{
		CRect rcDst;
		rcDst = CRect(CPoint(0,0), prcColor->Size());
		nError = ColorComposeWithoutMT(pImgColor, prcColor, &rcDst);
		if(nError)
			goto ErrorAbort;
	}
	else
	{
		Initialize();
		nError = GetSplitRect(prcColor);

		for(i=0;i<4;i++)
		{
			m_InspMTComposeParm[i].pImgBufSrc = pImgColor;
			m_InspMTComposeParm[i].rcSrc = rcSegmentSrc[i];
			m_InspMTComposeParm[i].rcDst = rcSegmentDst[i];
		}
		m_bKillInspectThreads = FALSE;
		

		for(i=0;i<4;i++)
		{
			ResetEvent(m_InspMTComposeParm[i].m_hDone);
			SetEvent(m_InspMTComposeParm[i].m_hRun);
		}

		HANDLE handles[4];
		for(i=0;i<4;i++)
		{
			handles[i] = m_InspMTComposeParm[i].m_hDone;
		}
		WaitForMultipleObjects(4, handles, TRUE, INFINITE);

	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgSplitColorBuf::ColorCompose");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CImgSplitColorBuf::ColorComposeWithoutMT(CImageBuffer* pImgColor, CRect* prcSrc, CRect* prcDst)
{
	int i, j;
	int nColorPitch;
	BYTE* pImgColorPtr;
	long lColorIndex;
	long lColorIndexRow;
	int nPitch;
	BYTE* pImg0Ptr;
	BYTE* pImg1Ptr;
	BYTE* pImg2Ptr;
	long lIndex;
	CRect rcColorSrc;
	CRect rcColorDst;

try
{
	if(prcSrc == NULL) {
		rcColorSrc = rcROI;
		rcColorDst = rcROI;
 	}
	else
	{
		rcColorSrc = *prcSrc;
		rcColorDst = *prcDst;
	}
	if(rcColorSrc.Size() != rcColorDst.Size())
		goto ErrorAbort;
	nColorPitch = pImgColor->GetPitch();
	pImgColorPtr = pImgColor->GetImgPtr();
	nPitch = Img0.GetPitch();
	pImg0Ptr = Img0.GetImgPtr();
	pImg1Ptr = Img1.GetImgPtr();
	pImg2Ptr = Img2.GetImgPtr();
	lIndex = rcColorDst.top * nPitch + rcColorDst.left;
	lColorIndexRow = rcColorSrc.top * nColorPitch + rcColorSrc.left * 3;
	for(i=0;i<rcColorDst.Height();i++,lIndex += nPitch,lColorIndexRow += nColorPitch) {
		lColorIndex = lColorIndexRow;
		for(j=0;j<rcColorDst.Width();j++) {
			pImgColorPtr[lColorIndex++] = pImg2Ptr[lIndex+j];
			pImgColorPtr[lColorIndex++] = pImg1Ptr[lIndex+j];
			pImgColorPtr[lColorIndex++] = pImg0Ptr[lIndex+j];
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CImgSplitColorBuf::ColorComposeWithoutMT");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
} 

#include "stdafx.h"
#include "ColorConversion.h"
#include "DebugLog.h"

struct pdst {
	BYTE B;
	BYTE G;
	BYTE R;
};


UINT ConvertRawY8BGGRThread0(LPVOID pParam)
{
	

	CConvertRawY8BGGR* pPVI = (CConvertRawY8BGGR *)pParam;
	INSP_MT_RAWY8BGGR_PARM* parm = &pPVI->m_InspMTConvertRawY8BGGRParm[0];

try {
	while(1) {
		WaitForSingleObject(parm->m_hRun, INFINITE);
		if(pPVI->m_bKillInspectThreads)
			goto PASS;
		parm->nError = 0;
		parm->nReserved = 0;

		parm->nError = 
			pPVI->ConvertRawY8BGGRExMT(parm->XSize, 
						parm->YStart, 
						parm->YSize, 
						parm->pBufRow, 
						parm->pBGR);
		SetEvent(parm->m_hDone);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ConvertRawY8BGGRThread0");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

UINT ConvertRawY8BGGRThread1(LPVOID pParam)
{
	

	CConvertRawY8BGGR* pPVI = (CConvertRawY8BGGR *)pParam;
	INSP_MT_RAWY8BGGR_PARM* parm = &pPVI->m_InspMTConvertRawY8BGGRParm[1];

try {
	while(1) {
		WaitForSingleObject(parm->m_hRun, INFINITE);
		if(pPVI->m_bKillInspectThreads)
			goto PASS;
		parm->nError = 0;
		parm->nReserved = 0;

		parm->nError = 
			pPVI->ConvertRawY8BGGRExMT(parm->XSize, 
						parm->YStart, 
						parm->YSize, 
						parm->pBufRow, 
						parm->pBGR);
		SetEvent(parm->m_hDone);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ConvertRawY8BGGRThread1");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}


UINT ConvertRawY8BGGRThread2(LPVOID pParam)
{
	

	CConvertRawY8BGGR* pPVI = (CConvertRawY8BGGR *)pParam;
	INSP_MT_RAWY8BGGR_PARM* parm = &pPVI->m_InspMTConvertRawY8BGGRParm[2];

try {
	while(1) {
		WaitForSingleObject(parm->m_hRun, INFINITE);
		if(pPVI->m_bKillInspectThreads)
			goto PASS;
		parm->nError = 0;
		parm->nReserved = 0;

		parm->nError = 
			pPVI->ConvertRawY8BGGRExMT(parm->XSize, 
						parm->YStart, 
						parm->YSize, 
						parm->pBufRow, 
						parm->pBGR);
		SetEvent(parm->m_hDone);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ConvertRawY8BGGRThread2");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}


UINT ConvertRawY8BGGRThread3(LPVOID pParam)
{
	

	CConvertRawY8BGGR* pPVI = (CConvertRawY8BGGR *)pParam;
	INSP_MT_RAWY8BGGR_PARM* parm = &pPVI->m_InspMTConvertRawY8BGGRParm[3];

try {
	while(1) {
		WaitForSingleObject(parm->m_hRun, INFINITE);
		if(pPVI->m_bKillInspectThreads)
			goto PASS;
		parm->nError = 0;
		parm->nReserved = 0;

		parm->nError = 
			pPVI->ConvertRawY8BGGRExMT(parm->XSize, 
						parm->YStart, 
						parm->YSize, 
						parm->pBufRow, 
						parm->pBGR);
		SetEvent(parm->m_hDone);
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"ConvertRawY8BGGRThread3");
	goto ErrorAbort;
}
PASS:
	return 0;
ErrorAbort:
	return -1;
}

CConvertRawY8BGGR::CConvertRawY8BGGR()
{
	bUseMultiThreading = FALSE;
	m_bInitialized = FALSE;
}

///Udupa; Apr'11; Late thread binding
void CConvertRawY8BGGR::Initialize()
{
	if(m_bInitialized)
		return;

	int i;

try
{
	nSegmentNo = 4;
	for(i=0;i<4;i++) {
		m_InspMTConvertRawY8BGGRParm[i].m_hRun = CreateEvent(NULL, FALSE, FALSE, NULL); 
		m_InspMTConvertRawY8BGGRParm[i].m_hDone = CreateEvent(NULL, FALSE, FALSE, NULL); 
		m_InspMTConvertRawY8BGGRParm[i].nError = -1;
	}

	m_bKillInspectThreads = FALSE;

	m_hThread[0] = AfxBeginThread(ConvertRawY8BGGRThread0, this, THREAD_PRIORITY_NORMAL)->m_hThread;
	m_hThread[1] = AfxBeginThread(ConvertRawY8BGGRThread1, this, THREAD_PRIORITY_NORMAL)->m_hThread;
	m_hThread[2] = AfxBeginThread(ConvertRawY8BGGRThread2, this, THREAD_PRIORITY_NORMAL)->m_hThread;
	m_hThread[3] = AfxBeginThread(ConvertRawY8BGGRThread3, this, THREAD_PRIORITY_NORMAL)->m_hThread;

	m_bInitialized = TRUE;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CConvertRawY8BGGR::Initialize");
	goto ErrorAbort;
}
	return;
ErrorAbort:
	return;
}

CConvertRawY8BGGR::~CConvertRawY8BGGR()
{
	///Udupa; Feb'11
	m_bKillInspectThreads = TRUE;
	for(int i=0; i<4; i++) {
		SetEvent(m_InspMTConvertRawY8BGGRParm[i].m_hRun);
		WaitForSingleObject(m_hThread[i], 3000);
	}
}

void CConvertRawY8BGGR::Clean()
{
}

int CConvertRawY8BGGR::ConvertRawY8BGGREx(int XSize, 
						int YSize, 
						BYTE *pBufRow, 
						BYTE *pBGR, 
						BYTE ur, 
						BYTE ug, 
						BYTE ub)
{
	int i;
	BYTE *pRBuf; 
	BYTE *pGBuf; 
	BYTE *pBBuf;
	int nError;

try
{
	ImgSplitColorBuf.rcROI = CRect(0, 0, XSize, YSize);
	ImgSplitColorBuf.Img0.Allocate(ImgSplitColorBuf.rcROI.Size());
	ImgSplitColorBuf.Img1.Allocate(ImgSplitColorBuf.rcROI.Size());
	ImgSplitColorBuf.Img2.Allocate(ImgSplitColorBuf.rcROI.Size());
	pRBuf = ImgSplitColorBuf.Img0.GetImgPtr();  
	pGBuf = ImgSplitColorBuf.Img1.GetImgPtr(); 
	pBBuf = ImgSplitColorBuf.Img2.GetImgPtr();

	if(bUseMultiThreading == FALSE)
	{
		nError = ConvertRawY8BGGRExMT(XSize, 
						0, 
						YSize, 
						pBufRow, 
						pBGR, 
						ur, 
						ug, 
						ub, 
						pRBuf, 
						pGBuf, 
						pBBuf);
		if(nError)
			goto ErrorAbort;
	}
	else
	{
		Initialize();

		double dStep;
		dStep = double(YSize)/4.0;
		for(i=0;i<4;i++)
		{
			m_InspMTConvertRawY8BGGRParm[i].XSize = XSize;
			m_InspMTConvertRawY8BGGRParm[i].YStart = Round(i * dStep); 
			m_InspMTConvertRawY8BGGRParm[i].YSize = Round((i+1) * dStep) - m_InspMTConvertRawY8BGGRParm[i].YStart; 
			m_InspMTConvertRawY8BGGRParm[i].pBufRow = pBufRow; 
			m_InspMTConvertRawY8BGGRParm[i].pBGR = pBGR; 
			m_InspMTConvertRawY8BGGRParm[i].ur = ur; 
			m_InspMTConvertRawY8BGGRParm[i].ug = ug; 
			m_InspMTConvertRawY8BGGRParm[i].ub = ub; 
			m_InspMTConvertRawY8BGGRParm[i].pRBuf = pRBuf; 
			m_InspMTConvertRawY8BGGRParm[i].pGBuf = pGBuf; 
			m_InspMTConvertRawY8BGGRParm[i].pBBuf = pBBuf;

/*			nError = ConvertRawY8BGGRExMT(m_InspMTConvertRawY8BGGRParm[i].XSize, 
							m_InspMTConvertRawY8BGGRParm[i].YStart, 
							m_InspMTConvertRawY8BGGRParm[i].YSize, 
							m_InspMTConvertRawY8BGGRParm[i].pBufRow, 
							m_InspMTConvertRawY8BGGRParm[i].pBGR, 
							m_InspMTConvertRawY8BGGRParm[i].ur, 
							m_InspMTConvertRawY8BGGRParm[i].ug, 
							m_InspMTConvertRawY8BGGRParm[i].ub, 
							m_InspMTConvertRawY8BGGRParm[i].pRBuf, 
							m_InspMTConvertRawY8BGGRParm[i].pGBuf, 
							m_InspMTConvertRawY8BGGRParm[i].pBBuf);
			if(nError)
				goto ErrorAbort;
*/
		}
		m_bKillInspectThreads = FALSE;
		

		for(int k=0;k<2;k++)
		{
			for(i=k;i<4;i+=2)
			{
				ResetEvent(m_InspMTConvertRawY8BGGRParm[i].m_hDone);
				SetEvent(m_InspMTConvertRawY8BGGRParm[i].m_hRun);
			}

			HANDLE handles[4];
			int nIndex;
			nIndex = 0;
			for(i=k;i<4;i+=2)
			{
				handles[nIndex++] = m_InspMTConvertRawY8BGGRParm[i].m_hDone;
			}
			WaitForMultipleObjects(2, handles, TRUE, INFINITE);

		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CConvertRawY8BGGR::ConvertRawY8BGGREx");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CConvertRawY8BGGR::ConvertRawY8BGGREx(int XSize, 
						int YSize, 
						BYTE *pBufRow, 
						BYTE *pBGR)
{
	int i;
	int nError;

try
{
	if(bUseMultiThreading == FALSE)
	{
		nError = ConvertRawY8BGGRExMT(XSize, 
						0, 
						YSize, 
						pBufRow, 
						pBGR);
		if(nError)
			goto ErrorAbort;
	}
	else
	{
		Initialize();

		double dStep;
		dStep = double(YSize)/4.0;
		for(i=0;i<4;i++)
		{
			m_InspMTConvertRawY8BGGRParm[i].XSize = XSize;
			m_InspMTConvertRawY8BGGRParm[i].YStart = Round(i * dStep); 
			m_InspMTConvertRawY8BGGRParm[i].YSize = Round((i+1) * dStep) - m_InspMTConvertRawY8BGGRParm[i].YStart; 
			m_InspMTConvertRawY8BGGRParm[i].pBufRow = pBufRow; 
			m_InspMTConvertRawY8BGGRParm[i].pBGR = pBGR; 

/*			nError = ConvertRawY8BGGRExMT(m_InspMTConvertRawY8BGGRParm[i].XSize, 
							m_InspMTConvertRawY8BGGRParm[i].YStart, 
							m_InspMTConvertRawY8BGGRParm[i].YSize, 
							m_InspMTConvertRawY8BGGRParm[i].pBufRow, 
							m_InspMTConvertRawY8BGGRParm[i].pBGR);
			if(nError)
				goto ErrorAbort;
*/
		}
		m_bKillInspectThreads = FALSE;

		for(int k=0;k<2;k++)
		{
			for(i=k;i<4;i+=2)
			{
				ResetEvent(m_InspMTConvertRawY8BGGRParm[i].m_hDone);
				SetEvent(m_InspMTConvertRawY8BGGRParm[i].m_hRun);
			}

			HANDLE handles[4];
			int nIndex;
			nIndex = 0;
			for(i=k;i<4;i+=2)
			{
				handles[nIndex++] = m_InspMTConvertRawY8BGGRParm[i].m_hDone;
			}
			WaitForMultipleObjects(2, handles, TRUE, INFINITE);
		}
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CConvertRawY8BGGR::ConvertRawY8BGGREx");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CConvertRawY8BGGR::ConvertRawY8BGGRExMT(int XSize, 
						int YStart, 
						int YSize, 
						BYTE *pBufRow, 
						BYTE *pBGR, 
						BYTE ur, 
						BYTE ug, 
						BYTE ub, 
						BYTE *pRBuf, 
						BYTE *pGBuf, 
						BYTE *pBBuf)
{
	BYTE *pBuf;
	BYTE *pR, *pB, *pG0, *pG1;
	int i, j;
	BYTE uMax;
	int nMaxIndex;

try
{
	// Udupa; Feb'11; No need to multiply by 3
	//pdst* pDstCol = (struct pdst*) pBGR + YStart * 3 * XSize;
	pdst* pDstCol = (struct pdst*) pBGR + YStart * XSize;
	
	pdst* pDstRow;

	pBuf = pBufRow + YStart * XSize;
	pRBuf = pRBuf + YStart * XSize;
	pGBuf = pGBuf + YStart * XSize;
	pBBuf = pBBuf + YStart * XSize;
	int twoRows = XSize + XSize;
	
	uMax = __max(__max(ur, ug), ub);
	nMaxIndex = 0;
	if(uMax == ug)
	{
		nMaxIndex = 1;
	}
	if(uMax == ub)
	{
		nMaxIndex = 2;
	}
	ur = 255 * ur/ uMax;
	ug = 255 * ug/ uMax;
	ub = 255 * ub/ uMax;

	switch(nMaxIndex)
	{
	case 0:
		for(i=0;i<YSize-1;i+=2) 
		{
			pDstRow = pDstCol;

			pB = pBuf;
			pG0 = pB + 1;
			pG1 = pB + XSize;
			pR = pG1 + 1;
			for(j=0; j<XSize-1; j+=2) {
				pDstRow->B = (*pB * ub) >> 8;
				pDstRow->G = ((*pG0+*pG1) * ug) >> 9;
				pDstRow->R = *pR;

				*pRBuf = pDstRow->R;
				pRBuf++;
				*pGBuf = pDstRow->G;
				pGBuf++;
				*pBBuf = pDstRow->B;
				pBBuf++;

				pDstRow++;
				pB+=2;
				pG1+=2;
				
				pDstRow->B = (*pB * ub) >> 8;
				pDstRow->G = ((*pG0+*pG1) * ug) >> 9;
				pDstRow->R = *pR;

				*pRBuf = pDstRow->R;
				pRBuf++;
				*pGBuf = pDstRow->G;
				pGBuf++;
				*pBBuf = pDstRow->B;
				pBBuf++;

				pDstRow++;
				pR+=2;
				pG0+=2;
			}

			pG1 = pBuf + XSize;
			pR = pG1 + 1;
			pB = pG1 + XSize;
			pG0 = pB + 1;
			
			for(j=0; j<XSize-1; j+=2) {
				pDstRow->B = (*pB * ub) >> 8;
				pDstRow->G = ((*pG0+*pG1) * ug) >> 9;
				pDstRow->R = *pR;	
				*pRBuf = pDstRow->R;
				pRBuf++;
				*pGBuf = pDstRow->G;
				pGBuf++;
				*pBBuf = pDstRow->B;
				pBBuf++;

				pDstRow++;
				pB += 2;
				pG1 += 2;
				
				pDstRow->B = (*pB * ub) >> 8;
				pDstRow->G = ((*pG0+*pG1) * ug) >> 9;
				pDstRow->R = *pR;

				*pRBuf = pDstRow->R;
				pRBuf++;
				*pGBuf = pDstRow->G;
				pGBuf++;
				*pBBuf = pDstRow->B;
				pBBuf++;

				pDstRow++;
				pR += 2;
				pG0 += 2;
			}

			pBuf += twoRows;
			pDstCol += twoRows;
		}
		break;
	case 1:
		for(i=0;i<YSize-1;i+=2) {
			pDstRow = pDstCol;

			pB = pBuf;
			pG0 = pB + 1;
			pG1 = pB + XSize;
			pR = pG1 + 1;
			for(j=0; j<XSize-1; j+=2) {
				pDstRow->B = (*pB * ub) >> 8;
				pDstRow->G = (*pG0+*pG1) >> 1;
				pDstRow->R = (*pR * ur) >>8;

				*pRBuf = pDstRow->R;
				pRBuf++;
				*pGBuf = pDstRow->G;
				pGBuf++;
				*pBBuf = pDstRow->B;
				pBBuf++;

				pDstRow++;
				pB+=2;
				pG1+=2;
				
				pDstRow->B = (*pB * ub) >> 8;
				pDstRow->G = (*pG0+*pG1) >> 1;
				pDstRow->R = (*pR * ur) >> 8;

				*pRBuf = pDstRow->R;
				pRBuf++;
				*pGBuf = pDstRow->G;
				pGBuf++;
				*pBBuf = pDstRow->B;
				pBBuf++;

				pDstRow++;
				pR+=2;
				pG0+=2;
			}

			pG1 = pBuf + XSize;
			pR = pG1 + 1;
			pB = pG1 + XSize;
			pG0 = pB + 1;
			
			for(j=0; j<XSize-1; j+=2) {
				pDstRow->B = (*pB * ub) >> 8;
				pDstRow->G = (*pG0+*pG1) >> 1;
				pDstRow->R = (*pR * ur) >>8;	

				*pRBuf = pDstRow->R;
				pRBuf++;
				*pGBuf = pDstRow->G;
				pGBuf++;
				*pBBuf = pDstRow->B;
				pBBuf++;

				pDstRow++;
				pB += 2;
				pG1 += 2;
				
				pDstRow->B = (*pB * ub) >> 8;
				pDstRow->G = (*pG0+*pG1) >> 1;
				pDstRow->R = (*pR * ur) >>8;


				*pRBuf = pDstRow->R;
				pRBuf++;
				*pGBuf = pDstRow->G;
				pGBuf++;
				*pBBuf = pDstRow->B;
				pBBuf++;

				pDstRow++;
				pR += 2;
				pG0 += 2;
			}

			pBuf += twoRows;
			pDstCol += twoRows;
		}
		break;
	case 2:
		for(i=0;i<YSize-1;i+=2) {
			pDstRow = pDstCol;

			pB = pBuf;
			pG0 = pB + 1;
			pG1 = pB + XSize;
			pR = pG1 + 1;
			for(j=0; j<XSize-1; j+=2) {
				pDstRow->B = *pB;
				pDstRow->G = ((*pG0+*pG1) * ug) >> 9;
				pDstRow->R = (*pR * ur) >>8;

				*pRBuf = pDstRow->R;
				pRBuf++;
				*pGBuf = pDstRow->G;
				pGBuf++;
				*pBBuf = pDstRow->B;
				pBBuf++;

				pDstRow++;
				pB+=2;
				pG1+=2;
				
				pDstRow->B = *pB;
				pDstRow->G = ((*pG0+*pG1) * ug) >> 9;
				pDstRow->R = (*pR * ur) >> 8;

				*pRBuf = pDstRow->R;
				pRBuf++;
				*pGBuf = pDstRow->G;
				pGBuf++;
				*pBBuf = pDstRow->B;
				pBBuf++;

				pDstRow++;
				pR+=2;
				pG0+=2;
			}

			pG1 = pBuf + XSize;
			pR = pG1 + 1;
			pB = pG1 + XSize;
			pG0 = pB + 1;
			
			for(j=0; j<XSize-1; j+=2) {
				pDstRow->B = *pB;
				pDstRow->G = ((*pG0+*pG1) * ug) >> 9;
				pDstRow->R = (*pR * ur) >>8;	

				*pRBuf = pDstRow->R;
				pRBuf++;
				*pGBuf = pDstRow->G;
				pGBuf++;
				*pBBuf = pDstRow->B;
				pBBuf++;

				pDstRow++;
				pB += 2;
				pG1 += 2;
				
				pDstRow->B = *pB;
				pDstRow->G = ((*pG0+*pG1) * ug) >> 9;
				pDstRow->R = (*pR * ur) >>8;

				*pRBuf = pDstRow->R;
				pRBuf++;
				*pGBuf = pDstRow->G;
				pGBuf++;
				*pBBuf = pDstRow->B;
				pBBuf++;

				pDstRow++;
				pR += 2;
				pG0 += 2;
			}

			pBuf += twoRows;
			pDstCol += twoRows;
		}
		break;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CConvertRawY8BGGR::ConvertRawY8BGGRExMT");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CConvertRawY8BGGR::ConvertRawY8BGGRExMT(int XSize, 
						int YStart, 
						int YSize, 
						BYTE *pBufRow, 
						BYTE *pBGR)
{
	BYTE *pBuf;
	BYTE *pR, *pB, *pG0, *pG1;
	int i, j;

try
{
	// Udupa; Feb'11; No need to multiply by 3
	//pdst* pDstCol = (struct pdst*) pBGR + YStart * 3 * XSize;
	pdst* pDstCol = (struct pdst*) pBGR + YStart * XSize;

	pdst* pDstRow;
	pBuf = pBufRow + YStart * XSize;
	int twoRows = XSize + XSize;

	for(i=0;i<YSize-2;i+=2) 
	{
		pDstRow = pDstCol;

		pB = pBuf;
		pG0 = pB + 1;
		pG1 = pB + XSize;
		pR = pG1 + 1;
		for(j=0; j<XSize-1; j+=2) {
			pDstRow->B = *pB;
			pDstRow->G = (*pG0+*pG1) >> 1;
			pDstRow->R = *pR;
			pDstRow++;
			pB+=2;
			pG1+=2;
			
			pDstRow->B = *pB;
			pDstRow->G = (*pG0+*pG1) >> 1;
			pDstRow->R = *pR;

			pDstRow++;
			pR+=2;
			pG0+=2;
		}

		pG1 = pBuf + XSize;
		pR = pG1 + 1;
		pB = pG1 + XSize;
		pG0 = pB + 1;
		
		for(j=0; j<XSize-1; j+=2) {
			pDstRow->B = *pB;
			pDstRow->G = (*pG0+*pG1) >> 1;
			pDstRow->R = *pR;	

			pDstRow++;
			pB += 2;
			pG1 += 2;
			
			pDstRow->B = *pB;
			pDstRow->G = (*pG0+*pG1) >> 1;
			pDstRow->R = *pR;

			pDstRow++;
			pR += 2;
			pG0 += 2;
		}

		pBuf += twoRows;
		pDstCol += twoRows;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CConvertRawY8BGGR::ConvertRawY8BGGRExMT");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}
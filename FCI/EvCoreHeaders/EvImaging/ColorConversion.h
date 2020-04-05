#ifndef COLORCONVERSION_H
#define COLORCONVERSION_H

#pragma once

#include "Imaging.h"
#include "ImgSplitColorBuf.h"

typedef struct {
	HANDLE m_hRun;
	HANDLE m_hDone;

	int XSize;
	int YStart; 
	int YSize; 
	BYTE *pBufRow; 
	BYTE *pBGR; 
	BYTE ur; 
	BYTE ug; 
	BYTE ub; 
	BYTE *pRBuf; 
	BYTE *pGBuf; 
	BYTE *pBBuf;
	int nReserved;
	int nError;
} INSP_MT_RAWY8BGGR_PARM;


UINT ConvertRawY8BGGRThread0(LPVOID pParam);
UINT ConvertRawY8BGGRThread1(LPVOID pParam);
UINT ConvertRawY8BGGRThread2(LPVOID pParam);
UINT ConvertRawY8BGGRThread3(LPVOID pParam);

class CConvertRawY8BGGR
{
public:
	CConvertRawY8BGGR();
	~CConvertRawY8BGGR();
	void Clean();
	int ConvertRawY8BGGREx(int XSize, 
						int YSize, 
						BYTE *pBufRow, 
						BYTE *pBGR, 
						BYTE ur, 
						BYTE ug, 
						BYTE ub);

	int ConvertRawY8BGGREx(int XSize, 
						int YSize, 
						BYTE *pBufRow, 
						BYTE *pBGR);


	int ConvertRawY8BGGRExMT(int XSize, 
						int YStart, 
						int YSize, 
						BYTE *pBufRow, 
						BYTE *pBGR);


	int ConvertRawY8BGGRExMT(int XSize, 
						int YStart, 
						int YSize, 
						BYTE *pBufRow, 
						BYTE *pBGR, 
						BYTE ur, 
						BYTE ug, 
						BYTE ub, 
						BYTE *pRBuf, 
						BYTE *pGBuf, 
						BYTE *pBBuf);

	CImgSplitColorBuf ImgSplitColorBuf;
public:
	BOOL bUseMultiThreading;
	BOOL m_bKillInspectThreads;
	INSP_MT_RAWY8BGGR_PARM m_InspMTConvertRawY8BGGRParm[10];
	CRect rcSegmentSrc[10];
	CRect rcSegmentDst[10];
	int nSegmentNo;

protected:
	HANDLE m_hThread[4];

	BOOL m_bInitialized;
	void Initialize();
};

#endif //COLORCONVERSION_H
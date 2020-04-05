/// Udupa; Image Queue Handler to manage and save images in a queue
/// Udupa; Dec'2013; Defect overlay queuing option

#pragma once

#include <queue>
using namespace std;

class CQueueDefectInfoCentralized
{
public:
	int m_nTrack;
	int m_nDoc;
	int m_nDie;
	int m_nFov;
	std::vector<CRect> defectRects;
	std::vector<CPoint> defectCenters;
	std::vector<std::vector<int>> defectX;
	std::vector<std::vector<int>> defectY;

	CQueueDefectInfoCentralized()
	{
		m_nTrack = 0;
		m_nDoc = 0;
		m_nDie = 0;
		m_nFov = 0;
	}

	CQueueDefectInfoCentralized(int nTrack,
		int nDoc,
		int nDie,
		int nFov,
		std::vector<CPoint> arrayCenter,
		std::vector<CRect> arrayRect,
		std::vector<std::vector<int>> arrayX,
		std::vector<std::vector<int>> arrayY)
	{
		m_nTrack = nTrack;
		m_nDoc = nDoc;
		m_nDie = nDie;
		m_nFov = nFov;
		defectCenters = arrayCenter;
		defectRects = arrayRect;

		defectX = arrayX;
		defectY = arrayY;
	}

	~CQueueDefectInfoCentralized()
	{
	}
};

class CQueueBufferCentralized
{
public:
	CImageBuffer* m_pImageBuffer;
	//CImageBuffer* m_pImageBufferInspect[5];
	int m_nTrack;
	int m_nDoc;
	int m_nDie;
	int m_nFov;
	BOOL m_bMappingReady;

	CQueueBufferCentralized()
	{
		m_pImageBuffer = NULL;
		m_nTrack = 0;
		m_nDoc = 0;
		m_nDie = 0;
		m_nFov = 0;
		m_bMappingReady = FALSE;
	}

	CQueueBufferCentralized(CImageBuffer* pImageBuffer,
		int nTrack,
		int nDoc,
		int nDie,
		int nFov, 
		BOOL bMappingReady)
	{
		m_pImageBuffer = new CImageBuffer();
		m_pImageBuffer->Allocate(pImageBuffer->GetImgSize());
		CRect rect = CRect(CPoint(0, 0), pImageBuffer->GetImgSize());
		ImgCopy(pImageBuffer, &rect, m_pImageBuffer, &rect);

		m_nTrack = nTrack;
		m_nDoc = nDoc;
		m_nDie = nDie;
		m_nFov = nFov;
		m_bMappingReady = bMappingReady;
	}

	~CQueueBufferCentralized()
	{
		if (m_pImageBuffer != NULL) {
			m_pImageBuffer->Free();
			delete m_pImageBuffer;
		}
	}
};

class CQueueBuffer
{
public:
	CImageBuffer* m_pImageBuffer;
	//CImageBuffer* m_pImageBufferInspect[5];
	int m_nTrack;
	int m_nDoc;
	int m_nDie;
	int m_nFov;
	int m_nErrorCode;
	int m_nTotalDocs[5];
	CRectArray arrRects;
	CRegionArray arrRegions;
	std::vector<CRect> defectRects;
	std::vector<CPoint> defectCenters;

	CQueueBuffer()
	{
		m_pImageBuffer = NULL;
		m_nTrack = 0;
		m_nDoc = 0;
		m_nDie = 0;
		m_nFov = 0;
		m_nErrorCode = 0;
	}

	CQueueBuffer(CImageBuffer* pImageBuffer, 
		int nTrack, 
		int nDoc, 
		int nDie, 
		int nFov,
		int nErrorCode, 
		CRectArray& rects, 
		CRegionArray& regions, 
		std::vector<CRect> pRects,
		std::vector<CPoint> pCenters)
	{
		m_pImageBuffer = new CImageBuffer();
		m_pImageBuffer->Allocate(pImageBuffer->GetImgSize());
		CRect rect = CRect(CPoint(0, 0), pImageBuffer->GetImgSize());
		ImgCopy(pImageBuffer, &rect, m_pImageBuffer, &rect);

		m_nTrack = nTrack;
		m_nDoc = nDoc;
		m_nDie = nDie;
		m_nFov = nFov;
		m_nErrorCode = nErrorCode;

		for(int i=0; i<rects.GetCount(); i++)
			arrRects.Add(rects[i]);
		for(int i=0; i<regions.GetCount(); i++)
			arrRegions.Add(regions[i]);

		defectRects = pRects;
		defectCenters = pCenters;
	}

	//CQueueBuffer(CImageBuffer* pImageBuffer,
	//	int nTrack,
	//	int nDocTotal,
	//	int nDie,
	//	int nFov)
	//{
	//	m_nTrack = nTrack;
	//	m_nTotalDocs[nTrack] = nDocTotal;
	//	m_nDie = nDie;
	//	m_nFov = nFov;

	//	for (int nDoc = 0; nDoc < nDocTotal; nDoc++) {
	//		m_pImageBufferInspect[nDoc] = new CImageBuffer();
	//		m_pImageBufferInspect[nDoc]->Allocate(pImageBuffer[nDoc].GetImgSize());
	//		CRect rect = CRect(CPoint(0, 0), pImageBuffer[nDoc].GetImgSize());
	//		ImgCopy(&pImageBuffer[nDoc], &rect, m_pImageBufferInspect[nDoc], &rect);
	//	}
	//}

	~CQueueBuffer()
	{
		if(m_pImageBuffer != NULL) {
			m_pImageBuffer->Free();
			delete m_pImageBuffer;
		}

		//for (int i = 0; i < 5; i++) {
		//	if (m_pImageBufferInspect[i] != NULL) {
		//		m_pImageBufferInspect[i]->Free();
		//		delete m_pImageBufferInspect[i];
		//	}
		//}
	}
};

class CImageQueueHandler
{
public:
	CImageQueueHandler();
	~CImageQueueHandler();
	int Initialize(CSize szImage);

	int PushImageBuffer(CImageBuffer* pImageBuffer, int nTrackIndex, int nDocIndex, int nDieIndex, int nFovIndex, int nErrorCode, CRectArray& rects, CRegionArray& regions);
	int PushImageBufferDL(CImageBuffer* pImageBuffer, int nTrackIndex,int nDocIndex,int nDieIndex,int nFovIndex, int nErrorCode, CRectArray& rects, CRegionArray& regions,
		std::vector<CRect> pRects, std::vector<CPoint> pCenters);

	int PushInspectImageBuffer(CImageBuffer* pImageBuffer, int nTrackIndex, int nDocIndex, int nDieIndex, int nFovIndex);

	int SaveImage(CQueueBuffer* pBuf);
	int SaveImageDL(CQueueBuffer * pBuf);
	int InspectImage(CQueueBuffer* pBuf);
	int FlushQueueBuffers();
	int FlushQueueBufferDL();

	queue<CQueueBuffer*> m_BackImageQueue;
	queue<CQueueBuffer*> m_ImageQueue;
	queue<CQueueBuffer*> m_ImageQueueDL;

	//Centralized Reject System: Send Image and Mapping result and Defect Info
	queue<CQueueBufferCentralized*> m_BackupImageQueueCentralized;
	queue<CQueueBufferCentralized*> m_ImageQueueCentralized;

	queue<BOOL> m_ResultQueueCentralized;

	queue<CQueueDefectInfoCentralized*> m_BackupDefectInfoQueueCentralized;
	queue<CQueueDefectInfoCentralized*> m_DefectInfoQueueCentralized;

	int FlushQueueBufferCentralized();
	int FlushQueueResultCentralized();
	int FlushQueueDefectInfoCentralized();

	int PushImageBufferCentralized(CImageBuffer* pImageBuffer, int nTrackIndex, int nDocIndex, int nDieIndex, int nFovIndex, BOOL bMappingReady = FALSE);
	int PushResultCentralized();
	int PushDefectInfoCentralized(int nTrackIndex, int nDocIndex, int nDieIndex, int nFovIndex, std::vector<CPoint>& defectCenters, std::vector<CRect>& defectRects,
		std::vector<std::vector<int>> defectX, std::vector<std::vector<int>> defectY);

	int SendCentralizedImage(CQueueBufferCentralized * pBuf);
	int SendCentralizedResult();
	int SendCentralizedDefectInfo(CQueueDefectInfoCentralized * pBuf);

	void TriggerSendCentralizedImage();
	void TriggerSendCentralizedResult();
	void TriggerSendCentralizedDefectInfo();

	BOOL ImageQueueCentralizedEmpty();
	BOOL ResultQueueCentralizedEmpty();
	BOOL DefectInfoQueueCentralizedEmpty();


	void DrawResultText(CQueueBuffer* pBuf, SYSTEMTIME* pSysTime);
	CImageBuffer m_BufDefect;

	//Save Image
	CRITICAL_SECTION m_csQueue;
	HANDLE m_hPushed;
	HANDLE m_hImageSaveThread;
	
	HANDLE m_hPushedDL;
	HANDLE m_hImageSaveThreadDL;

	HANDLE m_hPushedInspect;
	HANDLE m_hImageInspectThread;

	//Send Image To Centralized Sever
	CRITICAL_SECTION m_csImageCentralizedQueue;
	HANDLE m_hPushedImageCentralized;
	HANDLE m_hImageCentralizedSendThread;

	//Send Mapping Result To Centralized Sever
	CRITICAL_SECTION m_csResultCentralizedQueue;
	HANDLE m_hPushedResultCentralized;
	HANDLE m_hResultCentralizedSendThread;

	//Send Defect Info To Centralized Sever
	CRITICAL_SECTION m_csDefectInfoCentralizedQueue;
	HANDLE m_hPushedDefectInfoCentralized;
	HANDLE m_hDefectInfoCentralizedSendThread;

	//Centralized Queue
	HANDLE m_hCentralizedQueueSystemThread;
	HANDLE m_hPushedStationQueueCentralized;


	CApplication* m_pApp;
	CString m_strImageFolders;

	BOOL m_bActive;
};

UINT ImageSaveThread(LPVOID pParam);
UINT ImageSaveThreadDL(LPVOID pParam);
UINT ImageInspectThread(LPVOID pParam);

UINT ImageCentralizedSendThread(LPVOID pParam);
UINT ResultCentralizedSendThread(LPVOID pParam);
UINT DefectInfoCentralizedSendThread(LPVOID pParam);

UINT CentralizedQueueHandlingThread(LPVOID pParam);


//// inspect thread

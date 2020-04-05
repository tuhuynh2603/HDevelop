#pragma once

#include "ImageBuffer.h"

typedef int (CALLBACK* INIT_DLL)();
typedef int (CALLBACK* FREE_DLL)();

typedef int (CALLBACK* REGISTER_CAMERA)(CString strCameraFile, CString strCardFile, void **pCamModule, BOOL bCreateEvent, BOOL bMonitorMode);
typedef int (CALLBACK* FREE_MODULE)(void **pModule);

typedef int (CALLBACK* IMAGE_SNAP)(void *pCamModule, CImageBuffer** pGrabbedImgBuf);
typedef int (CALLBACK* ENQUEUE_ASYNC_SNAP)(void *pCamModule);
typedef int (CALLBACK* WAIT_FOR_EXPOSURE)(void *pCamModule, HANDLE eventKill);
typedef int (CALLBACK* WAIT_FOR_COMPLETION)(void *pCamModule, CImageBuffer** pGrabbedImgBuf);
typedef int (CALLBACK* CANCEL_SNAP)(void *pCamModule);

typedef int (CALLBACK* GET_RESOLUTION)(void *pCamModule, CSize *pSize);
typedef int (CALLBACK* SET_GRAB_TIMEOUT)(void *pCamModule, int nTimeout);

typedef int (CALLBACK* SET_TRIGGER_MODE)(void *pCamModule, int nMode);
typedef int (CALLBACK* GET_TRIGGER_MODE)(void *pCamModule, int *pnMode);

typedef int (CALLBACK* GET_CAMERA_AOI)(void *pCamModule, CRect *pRect);
typedef int (CALLBACK* SET_CAMERA_AOI)(void *pCamModule, CRect rect);

typedef int (CALLBACK* GET_EXPOSURE)(void *pCamModule, int* pnExposure);
typedef int (CALLBACK* SET_EXPOSURE)(void *pCamModule, int nExposure);

typedef int (CALLBACK* GET_CAMERA_GAIN)(void *pCamModule, int* pnGain, int nGainMode);
typedef int (CALLBACK* SET_CAMERA_GAIN)(void *pCamModule, int nGain, int nGainMode);

typedef int (CALLBACK* GET_BLACKLEVEL)(void *pCamModule, int* pnBlackLevel);
typedef int (CALLBACK* SET_BLACKLEVEL)(void *pCamModule, int nBlackLevel);

typedef int (CALLBACK* SET_WHITEBALANCE)(void *pCamModule, int nValue, int nColorSelector);

typedef int (CALLBACK* GET_BYTES_PER_PKT)(void *pCamModule, int* pnBytesPerPkt, int* pnBppMin, int* pnBppMax, int* pnBppInc);
typedef int (CALLBACK* SET_BYTES_PER_PKT)(void *pCamModule, int nBytesPerPkt);

typedef int (CALLBACK* ENABLE_MULTICAST)(void *pCamModule, char* strIpAddress);
	           
class CCameraInterface
{
public :
	// Variables
	HINSTANCE			hDLL;

	INIT_DLL			lpInitDLL;
	FREE_DLL			lpFreeDLL;

	REGISTER_CAMERA		lpRegisterCamera;
	FREE_MODULE			lpReleaseCamera;

	IMAGE_SNAP			lpImageGrab;
	ENQUEUE_ASYNC_SNAP	lpEnqueueAsyncGrab;
	WAIT_FOR_COMPLETION lpWaitForCompletion;
	CANCEL_SNAP			lpCancelPendingGrab;

	WAIT_FOR_EXPOSURE   lpWaitForExposure;

	GET_RESOLUTION		lpGetResolution;
	SET_TRIGGER_MODE	lpSetTrigMode;
	GET_TRIGGER_MODE	lpGetTrigMode;
	SET_GRAB_TIMEOUT	lpSetGrabTimeout;

	GET_CAMERA_AOI		lpGetCameraAoi;
	SET_CAMERA_AOI		lpSetCameraAoi;
	GET_EXPOSURE		lpGetExposure;
	SET_EXPOSURE		lpSetExposure;
	GET_CAMERA_GAIN		lpGetCameraGain;
	SET_CAMERA_GAIN		lpSetCameraGain;
	GET_BLACKLEVEL		lpGetBlackLevel;
	SET_BLACKLEVEL		lpSetBlackLevel;
	SET_WHITEBALANCE	lpSetWhiteBalance;
	GET_BYTES_PER_PKT	lpGetBytesPerPkt;
	SET_BYTES_PER_PKT	lpSetBytesPerPkt;

	ENABLE_MULTICAST    lpEnableMulticast;
	
	void *pCamModule;

	CCameraInterface();
	void CopyFrom(CCameraInterface *pFG);
};


int RegisterCameraCard(CCameraInterface *pFGM, CString strCameraCard);
int ReleaseCameraCard(CCameraInterface *pFGM);
int RegisterCamera(CCameraInterface *pFGM, CString strCameraFile, CString strCardFile, BOOL bCreateEvent, BOOL bMonitorMode);
int ReleaseCamera(CCameraInterface *pFGM);

int GetResolution(CCameraInterface *pFGM, CSize *pSize);
int SetGrabTimeout(CCameraInterface *pFGM, int nTimeout);

int SetTrigMode(CCameraInterface *pFGM, int nMode);
int GetTrigMode(CCameraInterface *pFGM, int *pnMode);

int GetCameraAoi(CCameraInterface *pFGM, CRect *pRect);
int SetCameraAoi(CCameraInterface *pFGM, CRect rect);

int GetCameraExposure(CCameraInterface *pFGM, int* pnExposure);
int SetCameraExposure(CCameraInterface *pFGM, int nExposure);

int GetCameraGain(CCameraInterface *pFGM, int* pnGain, int nGainMode);
int SetCameraGain(CCameraInterface *pFGM, int nGain, int nGainMode);

int GetCameraBlackLevel(CCameraInterface *pFGM, int* pnBlackLevel);
int SetCameraBlackLevel(CCameraInterface *pFGM, int nBlackLevel);

int GetBytesPerPkt(CCameraInterface *pFGM, int* pnBytesPerPkt, int* pnBppMin, int* pnBppMax, int* pnBppInc);
int SetBytesPerPkt(CCameraInterface *pFGM, int nBytesPerPkt);

int SetWhiteBalance(CCameraInterface *pFGM, int nValue, int nColorSelector);

int EnableMulticast(CCameraInterface *pFGM, char* strIpAddress);

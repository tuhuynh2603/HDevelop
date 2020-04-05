#pragma once

#include "ImageBuffer.h"

#include "afxmt.h"

#include "PylonGenericInterface.h"
#include <pylon/PylonIncludes.h>

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

//#include <pylon/gige/BaslerGigEInstantCamera.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>
#include <pylon/AviCompressionOptions.h>
//#include <pylon/gige/BaslerGigECameraEventHandler.h>

//typedef Pylon::CBaslerGigEInstantCamera Camera_t;
typedef Pylon::CBaslerUsbInstantCamera Camera_t;
//typedef Pylon::CBaslerGigECameraEventHandler CameraEventHandler_t;
typedef Pylon::CBaslerUsbCameraEventHandler CameraEventHandler_t;

using namespace Basler_UsbCameraParams;

#define NUM_BUFFERS 2

enum MyEvents
{
    eMyExposureEndEvent  = 100
};

typedef void (CALLBACK* CALLBACKFUNCT)(int);

extern "C" int _stdcall InitDLL();

extern "C" int _stdcall FreeDLL();

extern "C" int _stdcall RegisterCamera(
						CString strCamFileName,
						CString strMPFFileName,
						void **pCamModule,
						 BOOL bCreateEvent,
						 BOOL bMonitorMode);

extern "C" int _stdcall ReleaseCamera(void **pCamModule);
extern "C" int _stdcall WaitForCompletion(void *pCamModule, CImageBuffer** pGrabbedImgBuf/*, BOOL bStreamMode = FALSE*/);
extern "C" int _stdcall CancelPendingGrab(void* pCamModule);
extern "C" int _stdcall EnqueueAsyncGrab(void *pCamModule);

extern "C" int _stdcall SetTriggerMode(void *pCamModule, int nMode);
extern "C" int _stdcall SetTriggerParms(void *pCamModule, int nTriggerPolarity, int nPreTriggerDelay, int nDebouncingTime);
extern "C" int _stdcall GetTriggerMode(void *pCamModule, int *pnMode);
extern "C" int _stdcall SetGrabTimeOut(void *pCamModule, int nTimeout);
extern "C" int _stdcall GetResolution(void *pCamModule, CSize *pSize);

extern "C" int _stdcall ImageGrab(void *pCamModule, CImageBuffer** pGrabbedImgBuf/*, BOOL bStreamMode = FALSE*/);
extern "C" int _stdcall EnqueueAsyncGrabNoTrigger(void *pCamModule);

extern "C" int _stdcall RegHookFunction(void *pCamModule, CALLBACKFUNCT UserFunct);

extern "C" int _stdcall WaitForExposure(void *pCamModule, HANDLE eventKill);

int WaitForGrabEnd(void* pCamera);

extern "C" int _stdcall GetCameraAoi(void *pCamModule, CRect *pRect);
extern "C" int _stdcall SetCameraAoi(void *pCamModule, CRect rect);
extern "C" int _stdcall GetCameraGain(void *pCamModule, int *pnGain, int nGainMode);
extern "C" int _stdcall SetCameraGain(void *pCamModule, int nGain, int nGainMode);
extern "C" int _stdcall GetExposure(void *pCamModule, int *pnExposure);
extern "C" int _stdcall SetExposure(void *pCamModule, int nExposure);
extern "C" int _stdcall GetBlackLevel(void *pCamModule, int *pnBlackLevel);
extern "C" int _stdcall SetBlackLevel(void *pCamModule, int nBlackLevel);
extern "C" int _stdcall GetBytesPerPkt(void *pCamModule, long *pnBytesPerPkt, long* pnBytesPerPktMin, long* pnBytesPerPktMax, long* pnBytesPerPktInc);
extern "C" int _stdcall SetBytesPerPkt(void *pCamModule, long nBytesPerPkt , int nType);

extern "C" int _stdcall GetInterPacketDelay(void *pCamModule, int *nPacketsDelay);
extern "C" int _stdcall SetInterPacketDelay(void *pCamModule, int nPacketsDelay);
extern "C" int _stdcall GetFrameTransDelay(void *pCamModule, long *nFrameTransDelay);
extern "C" int _stdcall SetFrameTransDelay(void *pCamModule, long nFrameTransDelay);
extern "C" int _stdcall GetBandWidth(void *pCamModule, int* nBandWidth);
extern "C" int _stdcall SetBandWidth(void *pCamModule, int nBandWidth);
extern "C" int _stdcall GetBandWidthAcc(void *pCamModule, int *nBandWidthAcc);
extern "C" int _stdcall SetBandWidthAcc(void *pCamModule, int nBandWidthAcc);

extern "C" int _stdcall SetOutputLine(void *pCamModule, BOOL flag);//Added by E Santosh Singh for sending  to the output Line- 26th Feb 09

extern "C" int _stdcall SetWhiteBalance(void *pCamModule, int nValue, int nColorSelect);
extern "C" int _stdcall EnableMulticast(void *pCamModule, char* strIpAddress);
extern "C" int _stdcall CameraStream(void *pCamModule, CString* strFileName, int nFrameRate);
extern "C" int _stdcall StopStream(void *pCamModule);


class CCamEventHandler : public CameraEventHandler_t
{
public:
	//CameraEventHandler_t m_CamEventHandler;

	CCamEventHandler();
	~CCamEventHandler();
	//void (_stdcall* m_pUserFunct)(int);
	HANDLE m_hCamEvent;

    virtual void OnCameraEvent(Camera_t& camera, intptr_t userProvidedId, GenApi::INode* /* pNode */);
};

class CCamera
{
public :

	long m_lCamNo;
	BOOL m_bIsColor;

	Camera_t m_Camera;
	CImageBuffer m_GrabBuffer;

	CString m_strCamName;
	int m_nGrabTimeout;
	
	CSize m_szResolution;

	CPoint m_posAoi;
	CSize m_szAoi;

	CRITICAL_SECTION m_csCamera;

	int m_nTriggerMode;
	BOOL m_bMonitorMode;

	CCamera();
	~CCamera();

	Pylon::CAviWriter aviWriter;

	void (_stdcall* m_pUserFunct)(int);
	int ReInitializeCamera(void *pCamModule, CString *strCamID);
	int ReSetTriggerMode(void *pCamModule, int nMode);

	CCamEventHandler* m_pCamEventHandler;
private :

};

void SetMulticastAddress(CCamera* pCamera, BOOL bEnable);

// greatest common divisor
int gcd(int num1, int num2)
{
	ASSERT(num1 != 0);
	int remainder = num2 % num1;
	
	if (remainder != 0)
		return gcd(remainder,num1);
	
	return num1;
}

// least common multiple
int lcm(int num1, int num2)
{
	return (num1 * num2) / gcd(num1, num2);
}

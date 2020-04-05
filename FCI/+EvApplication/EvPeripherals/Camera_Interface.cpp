#include "stdafx.h"
#include "Camera_Interface.h"

#include "Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CCameraInterface::CCameraInterface()
{
	hDLL				= 0;
	lpInitDLL			= 0;
	lpFreeDLL			= 0;

	lpRegisterCamera	= 0;
	lpReleaseCamera		= 0;

	lpGetResolution			= 0;
	lpSetGrabTimeout	= 0;

	lpSetTrigMode		= 0;
	lpGetTrigMode		= 0;

	lpImageGrab			= 0;
	lpEnqueueAsyncGrab	= 0;
	lpWaitForCompletion = 0;
	lpCancelPendingGrab = 0;
	lpWaitForExposure   = 0;

	lpGetCameraAoi		= 0;
	lpSetCameraAoi		= 0;
	lpGetExposure		= 0;
	lpSetExposure		= 0;
	lpGetCameraGain		= 0;
	lpSetCameraGain		= 0;
	lpGetBlackLevel		= 0;
	lpSetBlackLevel		= 0;

	lpSetWhiteBalance   = 0;
	lpEnableMulticast   = 0;

	pCamModule = 0;
}

void CCameraInterface::CopyFrom(CCameraInterface *pFG)
{
	hDLL = pFG->hDLL;

	lpInitDLL		= pFG->lpInitDLL;
	lpFreeDLL		= pFG->lpFreeDLL;

	lpRegisterCamera	= pFG->lpRegisterCamera;
	lpReleaseCamera	= pFG->lpReleaseCamera;

	lpGetResolution		= pFG->lpGetResolution;
	lpSetGrabTimeout= pFG->lpSetGrabTimeout;

	lpSetTrigMode	= pFG->lpSetTrigMode;
	lpGetTrigMode	= pFG->lpGetTrigMode;

	lpImageGrab		= pFG->lpImageGrab;
	lpEnqueueAsyncGrab = pFG->lpEnqueueAsyncGrab;
	lpWaitForCompletion = pFG->lpWaitForCompletion;
	lpCancelPendingGrab = pFG->lpCancelPendingGrab;
	lpWaitForExposure = pFG->lpWaitForExposure;

	lpGetCameraAoi		= pFG->lpGetCameraAoi;
	lpSetCameraAoi		= pFG->lpSetCameraAoi;
	lpGetExposure		= pFG->lpGetExposure;
	lpSetExposure		= pFG->lpSetExposure;
	lpGetCameraGain		= pFG->lpGetCameraGain;
	lpSetCameraGain		= pFG->lpSetCameraGain;
	lpGetBlackLevel		= pFG->lpGetBlackLevel;
	lpSetBlackLevel		= pFG->lpSetBlackLevel;
	lpSetWhiteBalance	= pFG->lpSetWhiteBalance;
	lpGetBytesPerPkt	= pFG->lpGetBytesPerPkt;
	lpSetBytesPerPkt	= pFG->lpSetBytesPerPkt;

	lpEnableMulticast   = pFG->lpEnableMulticast;

	pCamModule = pFG->pCamModule;
}


int RegisterCameraCard(CCameraInterface *pFGM, CString strCameraCard)
{
	if(strCameraCard.CompareNoCase("none")!=0) {
		pFGM->hDLL = AfxLoadLibrary(strCameraCard);

		if(pFGM->hDLL==NULL) {
			CString str;
			str.Format("Error Loading Camera DLL (%s)", strCameraCard.GetBuffer(0));
			AfxMessageBox(str);
			return -1;
		}
		OutputDebugLogTo(0,FALSE,"Registering Camera Card...");

		pFGM->lpInitDLL = (INIT_DLL) GetProcAddress (pFGM->hDLL,"InitDLL");
		if(pFGM->lpInitDLL==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register InitDLL()");

		pFGM->lpFreeDLL = (FREE_DLL) GetProcAddress (pFGM->hDLL,"FreeDLL");
		if(pFGM->lpFreeDLL==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register FreeDLL()");

		pFGM->lpRegisterCamera = (REGISTER_CAMERA) GetProcAddress (pFGM->hDLL,"RegisterCamera");
		if(pFGM->lpRegisterCamera==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register RegisterCamera()");

		pFGM->lpReleaseCamera = (FREE_MODULE) GetProcAddress (pFGM->hDLL,"ReleaseCamera");
		if(pFGM->lpReleaseCamera==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register ReleaseCamera()");

		pFGM->lpImageGrab = (IMAGE_SNAP) GetProcAddress (pFGM->hDLL,"ImageGrab");
		if(pFGM->lpImageGrab==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register ImageGrab()");
		
       	pFGM->lpEnqueueAsyncGrab = (ENQUEUE_ASYNC_SNAP) GetProcAddress (pFGM->hDLL,"EnqueueAsyncGrab");
		if(pFGM->lpEnqueueAsyncGrab==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register EnqueueAsyncGrab()");

		pFGM->lpWaitForCompletion = (WAIT_FOR_COMPLETION) GetProcAddress (pFGM->hDLL,"WaitForCompletion");
		if(pFGM->lpWaitForCompletion==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register WaitForCompletion()");

		pFGM->lpCancelPendingGrab = (CANCEL_SNAP) GetProcAddress (pFGM->hDLL,"CancelPendingGrab");
		if(pFGM->lpCancelPendingGrab==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register CancelPendingGrab()");

		pFGM->lpWaitForExposure = (WAIT_FOR_EXPOSURE) GetProcAddress (pFGM->hDLL,"WaitForExposure");
		if(pFGM->lpWaitForExposure==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register WaitForExposure()");

		pFGM->lpGetResolution = (GET_RESOLUTION) GetProcAddress (pFGM->hDLL,"GetResolution");
		if(pFGM->lpGetResolution==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register GetResolution()");

		pFGM->lpSetTrigMode = (SET_TRIGGER_MODE) GetProcAddress (pFGM->hDLL,"SetTriggerMode");
		if(pFGM->lpSetTrigMode==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register SetTriggerMode()");

		pFGM->lpGetTrigMode = (GET_TRIGGER_MODE) GetProcAddress (pFGM->hDLL,"GetTriggerMode");
		if(pFGM->lpGetTrigMode==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register GetTriggerMode()");

		pFGM->lpSetGrabTimeout = (SET_GRAB_TIMEOUT) GetProcAddress (pFGM->hDLL,"SetGrabTimeOut");
		if(pFGM->lpGetTrigMode==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register SetGrabTimeOutMode()");

		pFGM->lpGetCameraAoi = (GET_CAMERA_AOI) GetProcAddress (pFGM->hDLL,"GetCameraAoi");
		if(pFGM->lpGetCameraAoi==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register GetCameraAoi()");

		pFGM->lpSetCameraAoi = (SET_CAMERA_AOI) GetProcAddress (pFGM->hDLL,"SetCameraAoi");
		if(pFGM->lpSetCameraAoi==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register SetCameraAoi()");

		pFGM->lpGetExposure = (GET_EXPOSURE) GetProcAddress (pFGM->hDLL,"GetExposure");
		if(pFGM->lpGetExposure==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register GetExposure()");

		pFGM->lpSetExposure = (SET_EXPOSURE) GetProcAddress (pFGM->hDLL,"SetExposure");
		if(pFGM->lpSetExposure==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register SetExposure()");

		pFGM->lpGetCameraGain = (GET_CAMERA_GAIN) GetProcAddress (pFGM->hDLL,"GetCameraGain");
		if(pFGM->lpGetCameraGain==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register GetCameraGain()");

		pFGM->lpSetCameraGain = (SET_CAMERA_GAIN) GetProcAddress (pFGM->hDLL,"SetCameraGain");
		if(pFGM->lpSetCameraGain==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register SetCameraGain()");

		pFGM->lpGetBlackLevel = (GET_BLACKLEVEL) GetProcAddress (pFGM->hDLL,"GetBlackLevel");
		if(pFGM->lpGetBlackLevel==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register GetBlackLevel()");

		pFGM->lpSetBlackLevel = (SET_BLACKLEVEL) GetProcAddress (pFGM->hDLL,"SetBlackLevel");
		if(pFGM->lpSetBlackLevel==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register SetBlackLevel()");

		pFGM->lpSetWhiteBalance = (SET_WHITEBALANCE) GetProcAddress (pFGM->hDLL,"SetWhiteBalance");
		if(pFGM->lpSetWhiteBalance==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register SetWhiteBalance()");

		pFGM->lpGetBytesPerPkt = (GET_BYTES_PER_PKT) GetProcAddress (pFGM->hDLL,"GetBytesPerPkt");
		if(pFGM->lpGetBytesPerPkt==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register GetBytesPerPkt()");

		pFGM->lpSetBytesPerPkt = (SET_BYTES_PER_PKT) GetProcAddress (pFGM->hDLL,"SetBytesPerPkt");
		if(pFGM->lpSetBytesPerPkt==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register SetBytesPerPkt()");
		
		pFGM->lpEnableMulticast= (ENABLE_MULTICAST) GetProcAddress (pFGM->hDLL,"EnableMulticast");
		if(pFGM->lpEnableMulticast==NULL)
			OutputDebugLogTo(0,FALSE,"Fail to register EnableMulticast()");

		if(pFGM->lpInitDLL)
			pFGM->lpInitDLL();
	}
	else {
		OutputDebugLogTo(0,FALSE,"No Camera Card");
	}
	return 0;
}

int ReleaseCameraCard(CCameraInterface *pFGM)
{
	if(pFGM->lpFreeDLL) {
		int nError = pFGM->lpFreeDLL();
		if(nError) {
			AfxMessageBox("[Camera Interface] Failed to release card");
			return -1;
		}
	}

	if(pFGM->hDLL)
		AfxFreeLibrary(pFGM->hDLL);

	return 0;
}

int RegisterCamera(CCameraInterface *pFGM,
				   CString strCameraFile,
				   CString strCardFile,
				   BOOL bCreateEvent,
				   BOOL bMonitorMode)
{
	CString strMsg;
	int nError;

	if(pFGM->lpRegisterCamera) {
		if(pFGM->pCamModule) {
			AfxMessageBox("**ERROR** CamModule has been previously Allocated");
			return -1;
		}

		nError = pFGM->lpRegisterCamera(strCameraFile,
									 strCardFile,
									 &(pFGM->pCamModule),
									 bCreateEvent,
									 bMonitorMode);
		if(nError) {
			strMsg.Format("Failed to register [%s]", strCameraFile);
			AfxMessageBox(strMsg);
			return -1;
		}
		else {
			strMsg.Format("Registered [%s]", strCameraFile);
			OutputDebugLogTo(0,FALSE,strMsg.GetBuffer(0));
			return 0;
		}
	}
	else {
		OutputDebugLogTo(0,FALSE,"No Camera Module");
	}
	return 0;
}

int ReleaseCamera(CCameraInterface *pFGM)
{
	int nError;

	if(pFGM->lpReleaseCamera) {
		if(pFGM->pCamModule) {
			nError = pFGM->lpReleaseCamera(&(pFGM->pCamModule));
			if(nError) {
				AfxMessageBox("Fail to release Camera");
				return -1;
			}
		}
	}
	return 0;
}

int GetResolution(CCameraInterface *pFGM,CSize *pSize)
{
	int nError = 0;

	if(pFGM->lpGetResolution)
		nError = pFGM->lpGetResolution(pFGM->pCamModule,pSize);
		if(nError) {
			AfxMessageBox("Fail to Get Camera Resolution");
			return -1;
		}

	return nError;
}

int SetTrigMode(CCameraInterface *pFGM,int nMode)
{
	int nError = 0;

	if(pFGM->lpSetTrigMode)
		nError = pFGM->lpSetTrigMode(pFGM->pCamModule,nMode);
		if(nError) {
			AfxMessageBox("Fail to Set Camera Trigger Mode");
			return -1;
		}

	return nError;
}

int GetTrigMode(CCameraInterface *pFGM,int *pnMode)
{
	int nError= 0;

	if(pFGM->lpGetTrigMode)
		nError = pFGM->lpGetTrigMode(pFGM->pCamModule,pnMode);
		if(nError) {
			AfxMessageBox("Fail to Get Camera Trigger Mode");
			return -1;
		}

	return nError;
}

int SetGrabTimeout(CCameraInterface *pFGM, int nTimeout)
{
	int nError = 0;

	if(pFGM->lpSetGrabTimeout)
		nError = pFGM->lpSetGrabTimeout(pFGM->pCamModule,nTimeout);
		if(nError) {
			AfxMessageBox("Fail to Set Camera Grab Timeout");
			return -1;
		}

	return nError;
}


int GetCameraAoi(CCameraInterface *pFGM, CRect *pRect)
{
	int nError = 0;

	if(pFGM->lpGetCameraAoi)
		nError = pFGM->lpGetCameraAoi(pFGM->pCamModule, pRect);
		if(nError) {
			AfxMessageBox("Failed to get Camera Aoi");
			return -1;
		}

	return nError;
}

int SetCameraAoi(CCameraInterface *pFGM, CRect rect)
{
	int nError = 0;

	if(pFGM->lpSetCameraAoi)
		nError = pFGM->lpSetCameraAoi(pFGM->pCamModule, rect);
		if(nError) {
			AfxMessageBox("Failed to set Camera Aoi");
			return -1;
		}

	return nError;
}

int GetCameraExposure(CCameraInterface *pFGM, int* pnExposure)
{
	int nError = 0;

	if(pFGM->lpGetExposure)
		nError = pFGM->lpGetExposure(pFGM->pCamModule, pnExposure);
		if(nError) {
			AfxMessageBox("Failed to get Camera Exposure");
			return -1;
		}

	return nError;
}

int SetCameraExposure(CCameraInterface *pFGM, int nExposure)
{
	int nError = 0;

	if(pFGM->lpSetExposure)
		nError = pFGM->lpSetExposure(pFGM->pCamModule, nExposure);
		if(nError) {
			AfxMessageBox("Failed to set Camera Exposure");
			return -1;
		}

	return nError;
}

int GetCameraGain(CCameraInterface *pFGM, int* pnGain, int nGainMode)
{
	int nError = 0;

	if(pFGM->lpGetCameraGain)
		nError = pFGM->lpGetCameraGain(pFGM->pCamModule, pnGain, nGainMode);
		if(nError) {
			AfxMessageBox("Failed to get Camera Gain");
			return -1;
		}

	return nError;
}

int SetCameraGain(CCameraInterface *pFGM, int nGain, int nGainMode)
{
	int nError = 0;

	if(pFGM->lpSetCameraGain)
		nError = pFGM->lpSetCameraGain(pFGM->pCamModule, nGain, nGainMode);
		if(nError) {
			AfxMessageBox("Failed to set Camera Gain");
			return -1;
		}

	return nError;
}

int GetCameraBlackLevel(CCameraInterface *pFGM, int* pnBlackLevel)
{
	int nError = 0;

	if(pFGM->lpGetBlackLevel)
		nError = pFGM->lpGetBlackLevel(pFGM->pCamModule, pnBlackLevel);
		if(nError) {
			AfxMessageBox("Failed to get Camera BlackLevel");
			return -1;
		}

	return nError;
}

int SetCameraBlackLevel(CCameraInterface *pFGM, int nBlackLevel)
{
	int nError = 0;

	if(pFGM->lpSetBlackLevel)
		nError = pFGM->lpSetBlackLevel(pFGM->pCamModule, nBlackLevel);
		if(nError) {
			AfxMessageBox("Failed to set Camera BlackLevel");
			return -1;
		}

	return nError;
}


int SetWhiteBalance(CCameraInterface *pFGM, int nValue, int nColorSelector)
{
	int nError = 0;

	if(pFGM->lpSetWhiteBalance)
		nError = pFGM->lpSetWhiteBalance(pFGM->pCamModule, nValue, nColorSelector);
		if(nError) {
			AfxMessageBox("Failed to set Camera White Balanc");
			return -1;
		}

	return nError;
}

int GetBytesPerPkt(CCameraInterface *pFGM, int* pnBytesPerPkt, int* pnBppMin, int* pnBppMax, int* pnBppInc)
{
	int nError = 0;

	if(pFGM->lpGetBytesPerPkt)
		nError = pFGM->lpGetBytesPerPkt(pFGM->pCamModule, pnBytesPerPkt, pnBppMin, pnBppMax, pnBppInc);
		if(nError) {
			AfxMessageBox("Failed to get Camera BytesPerPkt");
			return -1;
		}

	return nError;
}

int SetBytesPerPkt(CCameraInterface *pFGM, int nBytesPerPkt)
{
	int nError = 0;

	if(pFGM->lpSetBytesPerPkt)
		nError = pFGM->lpSetBytesPerPkt(pFGM->pCamModule, nBytesPerPkt);
		if(nError) {
			AfxMessageBox("Failed to set Camera BytesPerPkt");
			return -1;
		}

	return nError;
}

int EnableMulticast(CCameraInterface *pFGM, char* strIpAddress)
{
	int nError = 0;

	if(pFGM->lpEnableMulticast)
		nError = pFGM->lpEnableMulticast(pFGM->pCamModule, strIpAddress);
		if(nError) {
			AfxMessageBox("Failed to set Camera EnableMulticast");
			return -1;
		}

	return nError;
}
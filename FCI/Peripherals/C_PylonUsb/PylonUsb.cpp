#include "StdAfx.h"
#include "PylonUsb.h"
#include "CameraCode.h"
#include "PerformanceTimer.h"
#include "DebugLog.h"
#include "Imaging.h"
#include "ImageFunction.h"
#include "ImageBuffer.h"


CCamEventHandler::CCamEventHandler()
{
}

CCamEventHandler::~CCamEventHandler()
{
}

CCamera::CCamera()
{
	m_nGrabTimeout		= 1000;
	m_szResolution		= CSize(0,0);
	m_nTriggerMode		= TRIG_SOFT;
	m_pUserFunct		= NULL;

	m_bIsColor = FALSE;
	m_bMonitorMode = FALSE;
}

CCamera::~CCamera()
{
}

int _stdcall InitDLL()
{
	PylonInitialize();
	OutputDebugLog("C_PylonUsb DLL Loaded");

	return 0;
}

int _stdcall FreeDLL()
{
	PylonTerminate();

	return 0;
}

int _stdcall RegisterCamera(CString strCamFileName,
						 CString strMPFFileName,
						 void **pCamModule,
						 BOOL bCreateEvent,
						 BOOL bMonitorMode)
{
	if(strCamFileName.CompareNoCase("none")==0)
		return 0;

	Pylon::CTlFactory& tlFactory = Pylon::CTlFactory::GetInstance();
	Pylon::CDeviceInfo di;
	di.SetSerialNumber(strCamFileName.GetBuffer());
	strCamFileName.ReleaseBuffer();
	di.SetDeviceClass(Pylon::BaslerUsbDeviceClass);

	Pylon::IPylonDevice* pDevice = NULL;
	try { pDevice = tlFactory.CreateDevice(di); } catch(...) {}
	if(!pDevice) {
		OutputDebugLog("Camera %s not found", strCamFileName);
		return -1;
	}
	CCamera* pCamera = new CCamera();
	pCamera->m_Camera.Attach(pDevice);

	if(bMonitorMode) {
		pCamera->m_Camera.RegisterConfiguration((Pylon::CConfigurationEventHandler*) NULL, Pylon::RegistrationMode_ReplaceAll, Pylon::Ownership_ExternalOwnership);
		pCamera->m_Camera.MonitorModeActive = true;
		pCamera->m_bMonitorMode = TRUE;
	}

	////register here for exposure event
	//pCamera->m_pCamEventHandler = new CCamEventHandler();
    //pCamera->m_Camera.GrabCameraEvents = true;
	//pCamera->m_Camera.RegisterCameraEventHandler(pCamera->m_pCamEventHandler, "ExposureEndEventData", eMyExposureEndEvent, Pylon::RegistrationMode_ReplaceAll, Pylon::Ownership_ExternalOwnership);
	//
	//pCamera->m_pCamEventHandler->m_hCamEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
	pCamera->m_Camera.Open();

	CString strModelName;
	//strModelName.Format("%s", pCamera->m_Camera.GetDeviceInfo().GetModelName());
	//pCamera->m_bIsColor = FALSE;//(strModelName.Right(1).MakeLower()=="c");
	strModelName.Format("%s", pCamera->m_Camera.DeviceModelName.GetValue().c_str());
	pCamera->m_bIsColor = (strModelName.Right(1).MakeLower()=="c");

	pCamera->m_strCamName.Format("%s", pCamera->m_Camera.GetDeviceInfo().GetUserDefinedName().c_str());

	OutputDebugLog("[%s] Camera %s opened", pCamera->m_strCamName, strCamFileName);
	InitializeCriticalSection(&pCamera->m_csCamera);

	if(GenApi::IsReadable(pCamera->m_Camera.WidthMax)) {
		try {pCamera->m_szResolution.cx = (int) pCamera->m_Camera.WidthMax.GetValue(); }
		catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::Get WidthMax Exception"); }
		try { pCamera->m_szResolution.cy = (int) pCamera->m_Camera.HeightMax.GetValue(); }
		catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::Get HeightMax Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to get Resolution X/Y", pCamera->m_strCamName);

	if(!bMonitorMode) {
		//if(GenApi::IsAvailable(pCamera->m_Camera.EventSelector)) {
		//	try { pCamera->m_Camera.EventSelector.SetValue(Basler_UsbCameraParams::EventSelector_ExposureEnd); }
		//	catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::EventSelector_ExposureEnd Exception"); }
		//	/*try { pCamera->m_Camera.EventNotification.SetValue(Basler_UsbCameraParams::EventNotification_GenICamEvent); }
		//	catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::EventNotification_GenICamEvent Exception"); }*/
		//	try { pCamera->m_Camera.EventNotification.SetValue(Basler_UsbCameraParams::EventNotification_On); }
		//	catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::EventNotification_On Exception"); }
		//}

		if(GenApi::IsWritable(pCamera->m_Camera.PixelFormat)) {
			try { pCamera->m_Camera.PixelFormat.SetValue(pCamera->m_bIsColor ? Basler_UsbCameraParams::PixelFormat_BayerRG8 : Basler_UsbCameraParams::PixelFormat_Mono8); } 
			catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::PixelFormat Exception"); }
			OutputDebugLog("[%s] PixelFormat(%s) set", pCamera->m_strCamName, pCamera->m_bIsColor ? "BayerBG8" : "Mono8");
		} 
		else
			OutputDebugLog("********** [%s] Failed to set PixelFormat=(%s)", pCamera->m_strCamName, pCamera->m_bIsColor ? "BayerBG8" : "Mono8");

		if(GenApi::IsReadable(pCamera->m_Camera.OffsetX)) {
			try { pCamera->m_posAoi.x = (int) pCamera->m_Camera.OffsetX.GetValue(); } 
			catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::Get OffsetX Exception"); }
			try { pCamera->m_posAoi.y = (int) pCamera->m_Camera.OffsetY.GetValue(); }
			catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::Get OffsetY Exception"); }
		}
		else
			OutputDebugLog("********** [%s] Failed to get Offset X/Y", pCamera->m_strCamName);

		pCamera->m_posAoi = CPoint(0, 0);
		pCamera->m_szAoi = pCamera->m_szResolution;
		//SetCameraAoi(pCamera, CRect(pCamera->m_posAoi, pCamera->m_szAoi));

		if(GenApi::IsWritable(pCamera->m_Camera.TestImageSelector)) {
			try { pCamera->m_Camera.TestImageSelector.SetValue(Basler_UsbCameraParams::TestImageSelector_Off); }
			catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::TestImageSelector_Off Exception"); }
		}
		else
			OutputDebugLog("********** [%s] Failed to set TestImageSelector", pCamera->m_strCamName);

		if(GenApi::IsAvailable(pCamera->m_Camera.TriggerSelector.GetEntry(Basler_UsbCameraParams::TriggerSelector_FrameStart))) {
			if(IsWritable(pCamera->m_Camera.TriggerSelector)) {
				try { pCamera->m_Camera.TriggerSelector.SetValue(Basler_UsbCameraParams::TriggerSelector_FrameStart); }
				catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::TriggerSelector_FrameStart Exception"); }
			}
			if(IsWritable(pCamera->m_Camera.TriggerMode)) {
				try { pCamera->m_Camera.TriggerMode.SetValue(Basler_UsbCameraParams::TriggerMode_Off); }
				catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::TriggerMode_Off Exception"); }
			}
			if(IsWritable(pCamera->m_Camera.ExposureMode)) {
				try { pCamera->m_Camera.ExposureMode.SetValue(Basler_UsbCameraParams::ExposureMode_Timed); }
				catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::ExposureMode_Timed Exception"); }
			}
			if(IsWritable(pCamera->m_Camera.ExposureAuto)) {
				try { pCamera->m_Camera.ExposureAuto.SetValue(Basler_UsbCameraParams::ExposureAuto_Off); }
				catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::ExposureAuto_Off Exception"); }
			}
		}

		if(GenApi::IsAvailable(pCamera->m_Camera.TriggerSelector.GetEntry(Basler_UsbCameraParams::TriggerSelector_FrameStart))) {
			if(IsWritable(pCamera->m_Camera.TriggerSelector)) {
				try { pCamera->m_Camera.TriggerSelector.SetValue(Basler_UsbCameraParams::TriggerSelector_FrameStart); }
				catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::TriggerSelector_AcquisitionStart Exception"); }
			}
			else
				OutputDebugLog("********** [%s] Failed to set TriggerSelector:AcquisitionStart", pCamera->m_strCamName);

			/// Commented for Mold Forming... 
			//SetTriggerMode(pCamera, TRIG_SOFT);

			if(IsWritable(pCamera->m_Camera.AcquisitionFrameRateEnable)) {
				try { pCamera->m_Camera.AcquisitionFrameRateEnable.SetValue(false); }
				catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::AcquisitionFrameRateEnable Exception"); }
			}
			else
				OutputDebugLog("********** [%s] Failed to set AcquisitioFrameRateEnable", pCamera->m_strCamName);
		}

		/// Commented for Mold Forming... 
		//SetOutputLine(pCamera, true);
		pCamera->m_Camera.MaxNumBuffer = NUM_BUFFERS;
		
		try { pCamera->m_Camera.StartGrabbing(); } 
		catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::StartGrabbing Exception"); }
	}
	else {
		/*try { pCamera->m_Camera.GetStreamGrabberParams().TransmissionType = Basler_GigEStreamParams::TransmissionType_UseCameraConfig; }
		catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::TransmissionType_UseCameraConfig Exception"); }*/
		///SetMulticastAddress(pCamera, TRUE);

		///try { pCamera->m_Camera.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera); }
		try { pCamera->m_Camera.StartGrabbing(); }
		catch(...) {
			OutputDebugLog("********** [%s] Monitor Mode: Failed to attach to camera", pCamera->m_strCamName);
			ReleaseCamera((void**) &pCamera);
			delete pCamera;
			return -1;
		}
	}

	*pCamModule = pCamera;

	return 0;
}

int _stdcall ReleaseCamera(void **pCamModule)
{
	CString strMsg;
		
	if(pCamModule) {
		CCamera* pCamera =(CCamera*) *pCamModule;
		
		pCamera->m_Camera.StopGrabbing();
		//if(!PylonExecCommand(pCamera->m_hPylon, CAM_FEATURE, "AcquisitionStop"))
		//	OutputDebugLog("********** [%s] Failed to execute AcquisitionStop", pCamera->m_strCamName);

		//if(!PylonCancelGrab(pCamera->m_hPylon))
		//	OutputDebugLog("********** [%s] Failed to CancelGrab", pCamera->m_strCamName);

		DeleteCriticalSection(&pCamera->m_csCamera);
		//
		//if(!PylonFinishGrab(pCamera->m_hPylon))
		//	OutputDebugLog("********** [%s] Failed to FinishGrab", pCamera->m_strCamName);
		//SetTriggerMode((void**) pCamModule, TRIG_NONE);

		if(!pCamera->m_bMonitorMode) {
			if(IsWritable(pCamera->m_Camera.TriggerMode)) {
				try { pCamera->m_Camera.TriggerMode.SetValue(Basler_UsbCameraParams::TriggerMode_Off); }
				catch(...) { OutputDebugLogTo(0, TRUE, "ReleaseCamera::TriggerMode_Off Exception"); }
			}
			else
				OutputDebugLog("********** [%s] Failed to set TriggerMode", pCamera->m_strCamName);
		
			/*if(GenApi::IsAvailable(pCamera->m_Camera.EventSelector)) {
				try { pCamera->m_Camera.EventSelector.SetValue(Basler_UsbCameraParams::EventSelector_ExposureEnd); }
				catch(...) { OutputDebugLogTo(0, TRUE, "ReleaseCamera::EventSelector_ExposureEnd Exception"); }
				try { pCamera->m_Camera.EventNotification.SetValue(Basler_UsbCameraParams::EventNotification_Off); }
				catch(...) { OutputDebugLogTo(0, TRUE, "ReleaseCamera::EventNotification_Off Exception"); }
			}*/
		}
		//CloseHandle(pCamera->m_pCamEventHandler->m_hCamEvent);

		try { pCamera->m_Camera.Close(); }
		//if(!PylonCamClose(pCamera->m_hPylon))
		catch(...) { OutputDebugLog("********** [%s] Failed to CloseCamera", pCamera->m_strCamName); }
			
		if(pCamera->m_pUserFunct)
			pCamera->m_pUserFunct = NULL;
		
		//if(pCamera->m_pCamEventHandler)
		//	delete pCamera->m_pCamEventHandler;
		delete pCamera;
		pCamera = NULL;
		*pCamModule = pCamera;
	}
	return 0;
}

int _stdcall RegHookFunction(void *pCamModule, CALLBACKFUNCT UserFunct)
{
	if(pCamModule) {
		CCamera *pCamera =(CCamera*) pCamModule;

		pCamera->m_pUserFunct = UserFunct;
	}
	return 0;
}

int _stdcall SetGrabTimeOut(void *pCamModule,int nTimeout)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;
	pCamera->m_nGrabTimeout = nTimeout;

	return 0;
}

int _stdcall ImageGrab(void *pCamModule, CImageBuffer **pGrabbedImgBuf/*, BOOL bStreamMode*/)
{
	if(EnqueueAsyncGrab(pCamModule) == 0)
		return WaitForCompletion(pCamModule, pGrabbedImgBuf/*, bStreamMode*/);

	return -1;
}

int _stdcall EnqueueAsyncGrab(void *pCamModule)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}

	CCamera *pCamera =(CCamera*) pCamModule;

	if(!pCamera->m_bMonitorMode) {
		if(!pCamera->m_Camera.IsGrabbing()) {
			OutputDebugLog("********** [%s] EnqueueSnap: Camera is not ready", pCamera->m_strCamName);
			return -1;
		}
		pCamera->m_Camera.ExecuteSoftwareTrigger();
	}
	else {
		if(!pCamera->m_Camera.WaitForFrameTriggerReady(1000, Pylon::TimeoutHandling_Return)) {
			OutputDebugLog("********** [%s] EnqueueSnap: WaitForFrameTriggerReady failed", pCamera->m_strCamName);
			return -1;
		}
	}

	return 0;
}

int _stdcall WaitForCompletion(void *pCamModule, CImageBuffer** pGrabbedImgBuf/*, BOOL bStreamMode*/)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera *pCamera =(CCamera*) pCamModule;

	Pylon::CGrabResultPtr pGrabResult;
	int nError = 0;
	
	if(pCamera->m_Camera.RetrieveResult(pCamera->m_nGrabTimeout, pGrabResult, Pylon::TimeoutHandling_Return)) {
		if(pGrabResult && pGrabResult->GrabSucceeded()) {
			pCamera->m_GrabBuffer.AllocImgIndirect(CSize(pGrabResult->GetWidth(), pGrabResult->GetHeight()), (BYTE*) pGrabResult->GetBuffer());
			*pGrabbedImgBuf = &pCamera->m_GrabBuffer;
			//if(bStreamMode){
			//	OutputDebugLogTo(0, TRUE, "Adding Frames to AVI");
			//	pCamera->aviWriter.Add(pGrabResult);
			//	//OutputDebugLog("Added without conversion [%d]", pCamera->aviWriter.CanAddWithoutConversion(pGrabResult));
			//	//pCamera->aviWriter.Add(pGrabResult);
			//}
		}
		else {
			OutputDebugLogTo(0, TRUE, "[%s] Grab Error; GrabStatus = %s (0x%X)", 
				pCamera->m_strCamName, pGrabResult->GetErrorDescription().c_str(), pGrabResult->GetErrorCode());
			nError = -2;
		}
	}
	else
		nError = -1;

	if(pCamera->m_pUserFunct)
		(*(pCamera->m_pUserFunct))(1000);

	return nError;
}

int _stdcall CancelPendingGrab(void* pCamModule)
{
	CCamera *pCamera =(CCamera*) pCamModule;

	pCamera->m_Camera.StopGrabbing();
	pCamera->m_Camera.StartGrabbing(); //Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);

	OutputDebugLog("[%s] Cancelled grab", pCamera->m_strCamName);

	return 0;
}

//create OnCameraEvent function
void CCamEventHandler::OnCameraEvent(Camera_t& camera, intptr_t userProvidedId, GenApi::INode* /* pNode */)
{
	//if(m_bByPassEvent)
	//	return;

    if(userProvidedId == eMyExposureEndEvent) { //// Exposure End event
		//OutputDebugLog("Exposure End event. FrameID:%d ; Timestamp:%d ", (int)camera.ExposureEndEventFrameID.GetValue(), (int)camera.ExposureEndEventTimestamp.GetValue());
		//OutputDebugLogTo(2,TRUE,"Exposure End event Set");
		SetEvent(m_hCamEvent);
    }
}

int _stdcall WaitForExposure(void *pCamModule, HANDLE eventKill)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;
	if(pCamera->m_bMonitorMode)
		return 0;

	////GenApi::INode* pNode;
	////pNode = NULL;
	////OutputDebugLogTo(2,TRUE,"Pylon::WaitForExposure Entered");	
	//DWORD dwObj;
//
	//dwObj = WaitForSingleObject(pCamera->m_pCamEventHandler->m_hCamEvent, /*INFINITE*/1000); ////which will be set in callback OnCameraEvent
	//if(dwObj == WAIT_TIMEOUT) {
	//	OutputDebugLogTo(2,TRUE,"WaitForExposure TimeOut");	
	//	//try { pCamera->m_pCamEventHandler->OnCameraEvent(pCamera->m_Camera, eMyExposureEndEvent, pNode); }
	//	//catch(...) { OutputDebugLog("OnCameraEvent Exception"); }
	//	return -1;
	//}
	////else
	////	OutputDebugLogTo(2,TRUE,"WaitForExposure Lasted");

	return 0;
}

int WaitForGrabEnd(void* pCam)
{
	return 0;
}

int _stdcall EnqueueAsyncGrabNoTrigger(void *pCamModule)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}

	CCamera *pCamera =(CCamera*) pCamModule;
	if(pCamera->m_bMonitorMode)
		return 0;

	BOOL bTrigHard = FALSE;
	if(pCamera->m_nTriggerMode == TRIG_HARD) {
		if(IsWritable(pCamera->m_Camera.TriggerSource)) {
			try { pCamera->m_Camera.TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSource_Software); }
			catch(...) { OutputDebugLogTo(0, TRUE, "SetTriggerMode::TriggerSource_Software Exception"); }
		}
		else
			OutputDebugLog("********** [%s] Failed to set TriggerSource", pCamera->m_strCamName);
		pCamera->m_nTriggerMode = TRIG_SOFT;
		bTrigHard = TRUE;
	}

	int nResult = EnqueueAsyncGrab(pCamModule);

	if(bTrigHard) {
		Sleep(10);
		if(IsWritable(pCamera->m_Camera.TriggerSource)) {
			try { pCamera->m_Camera.TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSource_Line1); }
			catch(...) { OutputDebugLogTo(0, TRUE, "SetTriggerMode::TriggerSource_Line1 Exception"); }
		}
		else
			OutputDebugLog("********** [%s] Failed to set TriggerSource", pCamera->m_strCamName);
		pCamera->m_nTriggerMode = TRIG_HARD;
	}

	return nResult;
}

int _stdcall SetTriggerMode(void *pCamModule, int nMode)
{
	CString strMsg;
	
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}

	CCamera* pCamera =(CCamera*) pCamModule;
	if(pCamera->m_bMonitorMode)
		return 0;

	switch(nMode) {
		case TRIG_NONE:
			if(IsWritable(pCamera->m_Camera.TriggerMode)) { 
				try { pCamera->m_Camera.TriggerMode.SetValue(Basler_UsbCameraParams::TriggerMode_Off); }
				catch(...) { OutputDebugLogTo(0, TRUE, "SetTriggerMode::TriggerMode_Off Exception"); }
			}
			else
				OutputDebugLog("********** [%s] Failed to set TriggerMode", pCamera->m_strCamName);

			break;

		case TRIG_SOFT:
			if(IsWritable(pCamera->m_Camera.TriggerMode)) { 
				try { pCamera->m_Camera.TriggerMode.SetValue(Basler_UsbCameraParams::TriggerMode_On); }
				catch(...) { OutputDebugLogTo(0, TRUE, "SetTriggerMode::TriggerMode_On Exception"); }
			}
			else
				OutputDebugLog("********** [%s] Failed to set TriggerMode", pCamera->m_strCamName);

			if(IsWritable(pCamera->m_Camera.TriggerSource)) {
				try { pCamera->m_Camera.TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSource_Software); }
				catch(...) { OutputDebugLogTo(0, TRUE, "SetTriggerMode::TriggerSource_Software Exception"); }
			}
			else
				OutputDebugLog("********** [%s] Failed to set TriggerSource", pCamera->m_strCamName);
			break;

		case TRIG_HARD:
			if(IsWritable(pCamera->m_Camera.TriggerMode)) { 
				try { pCamera->m_Camera.TriggerMode.SetValue(Basler_UsbCameraParams::TriggerMode_On); }
				catch(...) { OutputDebugLogTo(0, TRUE, "SetTriggerMode::TriggerMode_On Exception"); }
			}
			else
				OutputDebugLog("********** [%s] Failed to set TriggerMode", pCamera->m_strCamName);

			if(IsWritable(pCamera->m_Camera.TriggerSource)) {
				try { pCamera->m_Camera.TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSource_Line1); }
				catch(...) { OutputDebugLogTo(0, TRUE, "SetTriggerMode::TriggerSource_Line1 Exception"); }
			}
			else
				OutputDebugLog("********** [%s] Failed to set TriggerSource:Line1", pCamera->m_strCamName);

			break;
	}
	pCamera->m_nTriggerMode = nMode;

	return 0;
}

int _stdcall SetTriggerParms(void *pCamModule, int nTriggerPolarity, int nPreTriggerDelay, int nDebouncingTime)
{
	CString strMsg;
	
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}

	//CCamera* pCamera =(CCamera*) pCamModule;
	//if(pCamera->m_bMonitorMode)
	//	return 0;

	//if(!PylonSetEnumFeature(pCamera->m_hPylon, CAM_FEATURE, "LineSelector", "Line1"))
	//	OutputDebugLog("********** [%s] Failed to set LineSelector", pCamera->m_strCamName);

	//if(!PylonSetEnumFeature(pCamera->m_hPylon, CAM_FEATURE, "TriggerActivation", nTriggerPolarity == RISING_EDGE? "RisingEdge" : "FallingEdge"))
	//	OutputDebugLog("********** [%s] Failed to set TriggerActivation", pCamera->m_strCamName);
	//if(!PylonSetFloatFeature(pCamera->m_hPylon, CAM_FEATURE, "TriggerDelayAbs", nPreTriggerDelay*1000))
	//	OutputDebugLog("********** [%s] Failed to set TriggerDelayAbs", pCamera->m_strCamName);

	//double dVal = nDebouncingTime * 1000;
	//if(!PylonSetFloatFeature(pCamera->m_hPylon, CAM_FEATURE, "LineDebouncerTimeAbs", dVal))
	//	OutputDebugLog("********** [%s] Failed to set LineDebouncerTimeAbs", pCamera->m_strCamName);
	//
	//dVal = 0;
	//double dMin = 0, dMax = 0;
	//if(!PylonGetFloatFeature(pCamera->m_hPylon, CAM_FEATURE, "LineDebouncerTimeAbs", &dVal, &dMin, &dMax))
	//	OutputDebugLog("********** [%s] Failed to get LineDebouncerTimeAbs", pCamera->m_strCamName);
	//OutputDebugLog("LineDebouncerTimeAbs Read: Value=%5.1f,  Min=%5.1f,  Max=%5.1f", dVal, dMin, dMax);

	//return 0;
	
	return -1;
}

int _stdcall GetTriggerMode(void *pCamModule,int *pnMode)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;

	*pnMode = pCamera->m_nTriggerMode;

	return 0;

}

int _stdcall GetResolution(void *pCamModule,CSize *pSize)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;

	*pSize = pCamera->m_szResolution;
	return 0;
}

int _stdcall GetCameraAoi(void *pCamModule, CRect *pRect)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;

	if(GenApi::IsReadable(pCamera->m_Camera.OffsetX)) {
		try { pCamera->m_posAoi.x = (int) pCamera->m_Camera.OffsetX.GetValue(); }
		catch(...) { OutputDebugLogTo(0, TRUE, "GetCameraAoi::Get OffsetX Exception"); }
		try { pCamera->m_posAoi.y = (int) pCamera->m_Camera.OffsetY.GetValue(); }
		catch(...) { OutputDebugLogTo(0, TRUE, "GetCameraAoi::Get OffsetY Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to get Offset X/Y", pCamera->m_strCamName);
	
	if(GenApi::IsReadable(pCamera->m_Camera.Width)) {
		try { pCamera->m_szAoi.cx = (int) pCamera->m_Camera.Width.GetValue(); }
		catch(...) { OutputDebugLogTo(0, TRUE, "GetCameraAoi::Get Width Exception"); }
		try { pCamera->m_szAoi.cy = (int) pCamera->m_Camera.Height.GetValue(); }
		catch(...) { OutputDebugLogTo(0, TRUE, "GetCameraAoi::Get Height Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to get AOI X/Y", pCamera->m_strCamName);

	pRect->SetRect(pCamera->m_posAoi.x, pCamera->m_posAoi.y, pCamera->m_posAoi.x+pCamera->m_szAoi.cx, pCamera->m_posAoi.y+pCamera->m_szAoi.cy);

	return 0;
}

int _stdcall SetCameraAoi(void *pCamModule, CRect rect)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;
	if(pCamera->m_bMonitorMode)
		return 0;

	EnterCriticalSection(&pCamera->m_csCamera);

	try { pCamera->m_Camera.StopGrabbing(); } 
	catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::StopGrabbing Exception"); }

	pCamera->m_szAoi = rect.Size();
	pCamera->m_szAoi.cx &= ~3;
	pCamera->m_posAoi = rect.TopLeft();

	OutputDebugLog("[%s] SetCameraAOI: Offset(%d,%d) Size(%d,%d) Resolution(%d,%d)", pCamera->m_strCamName,
																					pCamera->m_posAoi.x, pCamera->m_posAoi.y,
																					pCamera->m_szAoi.cx, pCamera->m_szAoi.cy,
																					pCamera->m_szResolution.cx, pCamera->m_szResolution.cy);

	if(pCamera->m_posAoi.x == 0 && pCamera->m_posAoi.y == 0){
		if(GenApi::IsWritable(pCamera->m_Camera.OffsetX)) {
			try { pCamera->m_Camera.OffsetX.SetValue(pCamera->m_posAoi.x); }
			catch(...) { OutputDebugLogTo(0, TRUE, "SetCameraAoi::Set OffsetX Exception"); }
			try { pCamera->m_Camera.OffsetY.SetValue(pCamera->m_posAoi.y); }
			catch(...) { OutputDebugLogTo(0, TRUE, "SetCameraAoi::Set OffsetY Exception"); }
		}
		else
			OutputDebugLog("********** [%s] Failed to set Offset X/Y", pCamera->m_strCamName);
	}

	if(GenApi::IsWritable(pCamera->m_Camera.Width)) {
		try { pCamera->m_Camera.Width.SetValue(pCamera->m_szAoi.cx); }
		catch(...) { OutputDebugLogTo(0, TRUE, "SetCameraAoi::Set Width Exception"); }
		try { pCamera->m_Camera.Height.SetValue(pCamera->m_szAoi.cy); }
		catch(...) { OutputDebugLogTo(0, TRUE, "SetCameraAoi::Set Height Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to set AOI X/Y", pCamera->m_strCamName);

	if(pCamera->m_posAoi.x > 0 || pCamera->m_posAoi.y > 0){
		if(GenApi::IsWritable(pCamera->m_Camera.OffsetX)) {
			try { pCamera->m_Camera.OffsetX.SetValue(pCamera->m_posAoi.x); }
			catch(...) { OutputDebugLogTo(0, TRUE, "SetCameraAoi::Set OffsetX Exception"); }
			try { pCamera->m_Camera.OffsetY.SetValue(pCamera->m_posAoi.y); }
			catch(...) { OutputDebugLogTo(0, TRUE, "SetCameraAoi::Set OffsetY Exception"); }
		}
		else
			OutputDebugLog("********** [%s] Failed to set Offset X/Y", pCamera->m_strCamName);
	}

	try { pCamera->m_Camera.StartGrabbing(); } 
	catch(...) { OutputDebugLogTo(0, TRUE, "RegisterCamera::Start Exception"); }
	LeaveCriticalSection(&pCamera->m_csCamera);

	return 0;
}

int _stdcall GetExposure(void *pCamModule, int *pnExposure)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;

	if(GenApi::IsReadable(pCamera->m_Camera.ExposureTime)) {	//ExposureTime was previosuly ExposureTimeRaw
		try { *pnExposure = (int) pCamera->m_Camera.ExposureTime.GetValue(); }
		catch(...) { OutputDebugLogTo(0, TRUE, "GetExposure::ExposureTimeAbs Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to get ExposureTimeAbs", pCamera->m_strCamName);

	return 0;
}

int _stdcall SetExposure(void *pCamModule, int nExposure)
{

	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;
	if(pCamera->m_bMonitorMode)
		return 0;

	if(nExposure < 40)
		nExposure = 40;
	if(GenApi::IsWritable(pCamera->m_Camera.ExposureTime)) {		//ExposureTime was previously ExposureTimeAbs
		try { pCamera->m_Camera.ExposureTime.SetValue(nExposure); }
		catch(...) { OutputDebugLogTo(0, TRUE, "SetExposure::ExposureTimeAbs Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to set ExposureTimeAbs", pCamera->m_strCamName);


	return 0;
}

int _stdcall GetCameraGain(void *pCamModule, int *pnGain, int nGainMode)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;

	if(GenApi::IsWritable(pCamera->m_Camera.GainSelector)) {
		/*try { pCamera->m_Camera.GainSelector.SetValue(nGainMode==0 ? Basler_UsbCameraParams::GainSelector_All : 
			(nGainMode==1 ? Basler_UsbCameraParams::GainSelector_Tap1 : Basler_UsbCameraParams::GainSelector_Tap2)); }
		catch(...) { OutputDebugLogTo(0, TRUE, "GetCameraGain::GainSelector Exception"); }*/

		try { pCamera->m_Camera.GainSelector.SetValue(Basler_UsbCameraParams::GainSelector_All); }
		catch(...) { OutputDebugLogTo(0, TRUE, "GetCameraGain::GainSelector Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to set GainSelector", pCamera->m_strCamName);

	if(GenApi::IsReadable(pCamera->m_Camera.Gain)) {	//Gain was previously GainRaw
		try { *pnGain = (int) pCamera->m_Camera.Gain.GetValue(); }
		catch(...) { OutputDebugLogTo(0, TRUE, "GetCameraGain::GainRaw Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to get GainRaw", pCamera->m_strCamName);

	return 0;
}

int _stdcall SetCameraGain(void *pCamModule, int nGain, int nGainMode)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;
	if(pCamera->m_bMonitorMode)
		return 0;

	if(GenApi::IsWritable(pCamera->m_Camera.GainSelector)) {
		/*try { pCamera->m_Camera.GainSelector.SetValue(nGainMode==0 ? Basler_UsbCameraParams::GainSelector_All : 
			(nGainMode==1 ? Basler_UsbCameraParams::GainSelector_Tap1 : Basler_UsbCameraParams::GainSelector_Tap2)); }
		catch(...) { OutputDebugLogTo(0, TRUE, "SetCameraGain::GainSelector Exception"); }*/

		try { pCamera->m_Camera.GainSelector.SetValue(Basler_UsbCameraParams::GainSelector_All); }
		catch(...) { OutputDebugLogTo(0, TRUE, "SetCameraGain::GainSelector Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to set GainSelector", pCamera->m_strCamName);

	if(GenApi::IsWritable(pCamera->m_Camera.Gain)) {	//Gain was previously GainRaw
		try { pCamera->m_Camera.Gain.SetValue(nGain); }
		catch(...) { OutputDebugLogTo(0, TRUE, "SetCameraGain::GainRaw Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to set GainRaw", pCamera->m_strCamName);

	return 0;
}

int _stdcall SetWhiteBalance(void *pCamModule, int nValue, int nColorSelect)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;
	if(pCamera->m_bMonitorMode)
		return 0;

	if(GenApi::IsWritable(pCamera->m_Camera.BalanceRatioSelector)) {
		try { pCamera->m_Camera.BalanceRatioSelector.SetValue(nColorSelect==0 ? Basler_UsbCameraParams::BalanceRatioSelector_Red : 
			(nColorSelect==1 ? Basler_UsbCameraParams::BalanceRatioSelector_Green : Basler_UsbCameraParams::BalanceRatioSelector_Blue)); }
		catch(...) { OutputDebugLogTo(0, TRUE, "SetWhiteBalance::BalanceRatioSelector Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to set BalanceRatioSelector", pCamera->m_strCamName);

	if(GenApi::IsWritable(pCamera->m_Camera.BalanceRatio)) {	//BalanceRatio was previously BalanceRatioRaw
		try { pCamera->m_Camera.BalanceRatio.SetValue(nValue); }
		catch(...) { OutputDebugLogTo(0, TRUE, "SetWhiteBalance::BalanceRatioRaw Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to set BalanceRatioRaw", pCamera->m_strCamName);

	return 0;
}

int _stdcall GetBlackLevel(void *pCamModule, int *pnBlackLevel)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;

	if(GenApi::IsReadable(pCamera->m_Camera.BlackLevel)) {	//BlackLevel was previously BlackLevelRaw
		try { *pnBlackLevel = (int) pCamera->m_Camera.BlackLevel.GetValue(); }
		catch(...) { OutputDebugLogTo(0, TRUE, "GetBlackLevel::BlackLevelRaw Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to get BLackLevelRaw", pCamera->m_strCamName);

	return 0;
}

int _stdcall SetBlackLevel(void *pCamModule, int nBlackLevel)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;
	if(pCamera->m_bMonitorMode)
		return 0;

	if(GenApi::IsWritable(pCamera->m_Camera.BlackLevel)) {	//BlackLevel was previously BlackLevelRaw
		try { pCamera->m_Camera.BlackLevel.SetValue(nBlackLevel); }
		catch(...) { OutputDebugLogTo(0, TRUE, "SetBlackLevel::nBlackLevel Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to set BlackLevelRaw", pCamera->m_strCamName);

	return 0;
}


int _stdcall GetBytesPerPkt(void *pCamModule, long *pnBytesPerPkt, long* pnBppMin, long* pnBppMax, long* pnBppInc)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;

	//PylonGetIntFeature(pCamera->m_hPylon, CAM_FEATURE, "GevSCPSPacketSize", pnBytesPerPkt, pnBppMin, pnBppMax, pnBppInc);
	//return 0;

	return -1;
}

int _stdcall SetBytesPerPkt(void *pCamModule, long nBytesPerPkt,int TransLayerPos)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;
	if(pCamera->m_bMonitorMode)
		return 0;
	//CString StrType;
	//int i;

	//switch(TransLayerPos) {
	//	case 0:
	//		StrType.Format("GevSCPSPacketSize");
	//		
	//		PylonCancelGrab(pCamera->m_hPylon);
	//		PylonExecCommand(pCamera->m_hPylon, CAM_FEATURE, "AcquisitionStop");
	//		PylonFinishGrab(pCamera->m_hPylon);

	//	break;
	//case 1:
	//		StrType.Format("GevSCPD");
	//	break;
	//case 2:
	//		StrType.Format("GevSCFTD");
	//	break;
	//case 3:
	//		StrType.Format("GevSCBWR");
	//	break;
	//case 4:
	//		StrType.Format("GevSCBWRA");
	//	break;

	//}
	//PylonSetIntFeature(pCamera->m_hPylon, CAM_FEATURE, StrType, nBytesPerPkt/*350*/);

	//return 0;

	return -1;
}

int _stdcall SetOutputLine(void *pCamModule, BOOL flag)
{
	/*if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}

	CCamera *pCamera =(CCamera*) pCamModule;
	if(pCamera->m_bMonitorMode)
		return 0;

	if(IsWritable(pCamera->m_Camera.LineSelector)) { 
		try { pCamera->m_Camera.LineSelector.SetValue(BaslerUsbCamera::L); }
		catch(...) { OutputDebugLogTo(0, TRUE, "LineSelector::LineSelector_Out1 Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to set LineSelector", pCamera->m_strCamName);

	if(IsWritable(pCamera->m_Camera.LineMode)) { 
		try { pCamera->m_Camera.LineMode.SetValue(BaslerUsbCamera::LineMode_Output); }
		catch(...) { OutputDebugLogTo(0, TRUE, "LineMode::LineMode_Output Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to set LineMode", pCamera->m_strCamName);

	if(IsWritable(pCamera->m_Camera.LineSource)) { 
		try { pCamera->m_Camera.LineSource.SetValue(BaslerUsbCamera::LineSource_ExposureActive); }
		catch(...) { OutputDebugLogTo(0, TRUE, "LineSource::LineSource_ExposureActive Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to set LineSource", pCamera->m_strCamName);

	if(IsWritable(pCamera->m_Camera.UserOutputSelector)) { 
		try { pCamera->m_Camera.UserOutputSelector.SetValue(BaslerUsbCamera::UserOutputSelector_UserOutput1); }
		catch(...) { OutputDebugLogTo(0, TRUE, "UserOutputSelector::UserOutputSelector_UserOutput1 Exception"); }
	}
	else
		OutputDebugLog("********** [%s] Failed to set UserOutputSelector", pCamera->m_strCamName);*/

	return 0;
}

void SetMulticastAddress(CCamera* pCamera, char* strIpAddress)
{
	return;
	/*try { pCamera->m_Camera.GetStreamGrabberParams().TransmissionType = 
		(strIpAddress ? Basler_GigEStreamParams::TransmissionType_Multicast :
		Basler_GigEStreamParams::TransmissionType_Unicast); }
	catch(...) { OutputDebugLogTo(0, TRUE, "EnableMulticast::TransmissionType Exception"); }

	if(strIpAddress) {
		try { pCamera->m_Camera.GetStreamGrabberParams().DestinationAddr = strIpAddress; }
		catch(...) { OutputDebugLogTo(0, TRUE, "EnableMulticast::DestinationAddr Exception"); }

		try { pCamera->m_Camera.GetStreamGrabberParams().DestinationPort = 49152; }
		catch(...) { OutputDebugLogTo(0, TRUE, "EnableMulticast::DestinationAddr Exception"); }
	}

	OutputDebugLogTo(0, TRUE, "[%s] ------ Multicasting %s ------------", pCamera->m_strCamName, strIpAddress ? "ENABLED" : "DISABLED");*/
}

int _stdcall EnableMulticast(void *pCamModule, char* strIpAddress)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;
	if(pCamera->m_bMonitorMode)
		return 0;

	pCamera->m_Camera.StopGrabbing();

	SetMulticastAddress(pCamera, strIpAddress);

	pCamera->m_Camera.StartGrabbing(); //Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);

	return 0;
}


int _stdcall CameraStream(void *pCamModule, CString* strFileName, int nFrameRate)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}

	uint32_t nWidth;
	uint32_t nHeight;

	CCamera* pCamera =(CCamera*) pCamModule;
	Pylon::SAviCompressionOptions compressionOptions("MSVC", false);
	if(EnqueueAsyncGrab(pCamModule) == 0){
		Pylon::CGrabResultPtr pGrabResult;

		if(pCamera->m_Camera.RetrieveResult(pCamera->m_nGrabTimeout, pGrabResult, Pylon::TimeoutHandling_Return)) {
			if(pGrabResult && pGrabResult->GrabSucceeded()){
				nWidth	= pGrabResult->GetWidth();
				nHeight = pGrabResult->GetHeight();
			}
			else 
				goto FailedGrab;
		}
		else
			goto FailedGrab;
	}
	else
			goto FailedGrab;

	Pylon::SAviCompressionOptions* pCompressionOptions = NULL;
	
    //pCompressionOptions = &compressionOptions;

	OutputDebugLog("AVI Frame rate %d", nFrameRate);
	pCamera->aviWriter.Open(strFileName->GetBuffer(), 
					   nFrameRate,
					   Pylon::PixelType_Mono8, 
					   nWidth, 
					   nHeight, 
					   Pylon::ImageOrientation_BottomUp, // Some compression codecs will not work with top down oriented images.
					   pCompressionOptions);

	OutputDebugLog("AVI Frame rate Open Completed");
	strFileName->ReleaseBuffer();
	return 0;

FailedGrab:
	return -1;
}


int _stdcall StopStream(void *pCamModule)
{
	if(!pCamModule) {
		ASSERT(!"CamModule is not Initialised");
		return -1;
	}
	CCamera* pCamera =(CCamera*) pCamModule;
	pCamera->aviWriter.Close();
	OutputDebugLog("Done saving AVI file");
	return 0;
}
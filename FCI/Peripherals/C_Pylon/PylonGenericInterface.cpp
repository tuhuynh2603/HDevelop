

#include "stdafx.h"

#include "PylonGenericInterface.h"

#include <pylon/PylonIncludes.h>


#if defined USE_WTL
CAppModule _Module;
#endif

using namespace Pylon;
using namespace GenApi;

class CPylonColorConverter;

inline IPylonDevice *PylonCamFromHandle(PYLONCAMHANDLE hCamera)
{
	return reinterpret_cast<IPylonDevice *>(hCamera);
}

inline StreamBufferHandle *PylonBufferFromHandle(PYLONBUFFERHANDLE hBuffer)
{
	return reinterpret_cast<StreamBufferHandle *>(hBuffer);
}

inline CPylonColorConverter *PylonColorConverterFromHandle(PYLONCCHANDLE hColorConverter)
{
	return reinterpret_cast<CPylonColorConverter *>(hColorConverter);
}

inline CallbackHandleType PylonCallbackFromHandle(PYLONCALLBACKHANDLE hCallback)
{
	return reinterpret_cast<CallbackHandleType>(hCallback);
}

// stores most recent error
struct CPylonLastError
{
	//unsigned long ErrorCode;
	gcstring ErrorMsg;
} PylonLastError;


// -------------------------------------------------------------
//
// implementation helpers
//
// -------------------------------------------------------------

static void _stdcall PylonClearLastError(void)
{
	PylonLastError.ErrorMsg = "";
}

static void _stdcall PylonSetLastError(const gcstring strFunction, const GenICam::GenericException& e)
{
	PylonLastError.ErrorMsg = strFunction + ": " + e.GetDescription();
}

// ==============================================================
//
//    Public methods 
//
// ==============================================================

// -------------------------------------------------------------
// PylonGetLastErrorMsg()
// -------------------------------------------------------------

PYLONDLL_API void _stdcall PylonGetLastErrorMsg(char *ErrMessage)
{
	strcpy(ErrMessage, (const char *) PylonLastError.ErrorMsg);
}


// -------------------------------------------------------------
// Inititializing / Uninitializing
// -------------------------------------------------------------

PYLONDLL_API void _stdcall PylonInitialize()
{
	Pylon::PylonInitialize();
}

PYLONDLL_API void _stdcall PylonTerminate()
{
	Pylon::PylonTerminate();
}

// -------------------------------------------------------------
// PylonOpen()
// -------------------------------------------------------------

DeviceInfoList_t devices;

PYLONDLL_API BOOL _stdcall PylonGetNumCam(long *NumberOfCameras)
{
	try
	{
		if(devices.empty()) {
			CTlFactory *pTlF = &CTlFactory::GetInstance();
			pTlF->EnumerateDevices(devices);
		}
		*NumberOfCameras = (long) devices.size();
		//*NumberOfCameras = pTlF->EnumerateDevices(devices);
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetNumCam"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonGetSerialNumberByIndex(long Index, char *SerialNumber)
{
	try
	{
		// enumerate device names of present Pylon devices
		CTlFactory *pTlF = &CTlFactory::GetInstance();
		DeviceInfoList_t devices;
		devices.clear();
		pTlF->EnumerateDevices(devices);

		if(devices[Index].GetDeviceClass() == "Basler1394")
		{
			// we have to extract the S/N from the FullName
			char tmpBuffer [50];	
			memset(tmpBuffer, 0, sizeof(tmpBuffer));
			gcstring value = devices[Index].GetFullName();
			strncpy(tmpBuffer, (const char *) value, 49);
			strtok(tmpBuffer, "#");
			char *token = strtok(NULL, "#"); 
			if(token && !strcmp("533000", token + strlen(token) -6)) // it's a Basler camera!
			{
				char snBuffer [11];
				sprintf(snBuffer, "0x%c%c%c%c%c%c%c%c", 
					token [6], token [7], token [4], token [5], 
					token [2], token [3], token [0], token [1]);
				unsigned long SN = strtol(snBuffer, 0, 16);
				_itoa(SN, SerialNumber, 10);
			}
			else
			{
				strcpy(SerialNumber, (const char *) devices[Index].GetSerialNumber());
			}
		}
		else{
			//strcpy(SerialNumber, (const char *) devices[Index].GetSerialNumber());
			int k=0;
			for(DeviceInfoList_t::iterator it = devices.begin(); it != devices.end(); it++)
			{
				strcpy(SerialNumber, (const char *) devices[Index].GetSerialNumber());
				k++;
			}

		}
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetSerialNumberByIndex"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonGetCameraInfo(PYLONCAMHANDLE hCamera, SCameraInfo *info)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		memset(info, 0, sizeof(info));
		strncpy(info->FriendlyName, (const char *) pCamera->GetDeviceInfo().GetFriendlyName(), 49);
		strncpy(info->FullName, (const char *) pCamera->GetDeviceInfo().GetFullName(), 49);
		strncpy(info->ModelName, (const char *) pCamera->GetDeviceInfo().GetModelName(), 49);
		strncpy(info->SerialNumber, (const char *) pCamera->GetDeviceInfo().GetSerialNumber(), 49);
		strncpy(info->UserDefinedName, (const char *) pCamera->GetDeviceInfo().GetUserDefinedName(), 49);
		strncpy(info->VendorName, (const char *) pCamera->GetDeviceInfo().GetVendorName(), 49);
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetCameraInfo"), e);
		return FALSE;
	}
}

PYLONDLL_API PYLONCAMHANDLE _stdcall PylonCamOpen(long Index)
{
	try
	{
		// enumerate device names of present Pylon devices
		IPylonDevice *pCamera;
		CTlFactory *pTlF = &CTlFactory::GetInstance();

		DeviceInfoList_t devices;
		if(Index > pTlF->EnumerateDevices(devices))
			return NULL;

		// create device and open camera
		pCamera = pTlF->CreateDevice(devices[Index]);
		pCamera->Open();
		IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
		pStreamGrabber->Open();

		return reinterpret_cast<PYLONCAMHANDLE>(pCamera);

	} 
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonCamOpen"), e);
		return NULL;
	}
}


PYLONDLL_API PYLONCAMHANDLE _stdcall PylonCamOpenBySerialNumber(const char *SerialNumber, int& cameraType)
{
	try
	{
		if(devices.empty()) {
			CTlFactory *pTlF = &CTlFactory::GetInstance();
			devices.clear();
			pTlF->EnumerateDevices(devices);
		}
		int k=0;
		int index=-1;
		int camType = -1;

		char slno[256];
		for(DeviceInfoList_t::iterator it = devices.begin(); it != devices.end(); it++)
		{
			//printf("VendorName: %s\n", (const char *) devices[k].GetSerialNumber());
			//if(!strcmp(SerialNumber, (const char *) devices[k].GetSerialNumber()))
			//	index=k;

			if(devices[k].GetDeviceClass() == "Basler1394") {
				// we have to extract the S/N from the FullName
				char tmpBuffer [50];	
				memset(tmpBuffer, 0, sizeof(tmpBuffer));
				gcstring value = devices[k].GetFullName();
				strncpy(tmpBuffer, (const char *) value, 49);
				strtok(tmpBuffer, "#");
				char *token = strtok(NULL, "#"); 
				if(token && !strcmp("533000", token + strlen(token) -6)) { // it's a Basler camera!
					char snBuffer [11];
					sprintf(snBuffer, "0x%c%c%c%c%c%c%c%c", 
						token [6], token [7], token [4], token [5], 
						token [2], token [3], token [0], token [1]);
					unsigned long SN = strtol(snBuffer, 0, 16);
					_itoa(SN, slno, 10);
				}
				else {
					strcpy(slno, (const char *) devices[k].GetSerialNumber());
				}
				camType = 0;
			}
			else{
				strcpy(slno, (const char *) devices[k].GetSerialNumber());
				camType = 1;
			}

			if(!strcmp(SerialNumber, slno)) {
				index=k;
				cameraType = camType;
				break;
			}
			k++;
		}
		// create device and open camera
		if(index==-1)
			return NULL;

		IPylonDevice *pCamera;
		CTlFactory *pTlF = &CTlFactory::GetInstance();
		pCamera = pTlF->CreateDevice(devices[index]);
		pCamera->Open();
		IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
		pStreamGrabber->Open();

		return reinterpret_cast<PYLONCAMHANDLE>(pCamera);

	} 
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonCamOpenBySerialNumber"), e);
		return NULL;
	}

}

// -------------------------------------------------------------
// PylonClose()
// -------------------------------------------------------------

PYLONDLL_API BOOL _stdcall PylonFinishGrab(PYLONCAMHANDLE hCamera)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
		pStreamGrabber->FinishGrab();
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonFinishGrab"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonCancelGrab(PYLONCAMHANDLE hCamera)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
		pStreamGrabber->CancelGrab();
		for(GrabResult r; pStreamGrabber->RetrieveResult(r) ;);
		return TRUE;
	}
	catch(GenICam::GenericException &e)
	{
		PylonSetLastError(gcstring("PylonCancelGrab"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonDeregisterBuffer(PYLONCAMHANDLE hCamera, PYLONBUFFERHANDLE hBuffer)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
		StreamBufferHandle *pStreamBuffer = PylonBufferFromHandle(hBuffer);
		pStreamGrabber->DeregisterBuffer(pStreamBuffer);

		return TRUE;
	}
	catch(GenICam::GenericException &e)
	{
		PylonSetLastError(gcstring("PylonDeregisterBuffer"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonCamClose(PYLONCAMHANDLE hCamera)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
		CTlFactory *pTlF = &CTlFactory::GetInstance();

		pStreamGrabber->Close();
		pCamera->Close();
		pTlF->DestroyDevice(pCamera);

		return TRUE;
	}
	catch(GenICam::GenericException &e)
	{
		PylonSetLastError(gcstring("PylonCamClose"), e);
		return FALSE;
	}
}

// -------------------------------------------------------------
// Feature Setters
// -------------------------------------------------------------

PYLONDLL_API BOOL _stdcall PylonSetEnumFeature(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, const char *Entry)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();
		CEnumerationPtr ptr = pNodeMap->GetNode(FeatureName);
		ptr->FromString(Entry);
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonSetEnumFeature"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonSetEnumFeatureByIndex(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, long Index)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();
		CEnumerationPtr ptr = pNodeMap->GetNode(FeatureName);
		ptr->SetIntValue(Index);
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonSetEnumFeature"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonSetIntFeature(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, long Val)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();
		CIntegerPtr ptr = pNodeMap->GetNode(FeatureName);
		ptr->SetValue(Val);
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonSetIntFeature"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonSetStringFeature(/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, const char *Val)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();
		CStringPtr ptr = pNodeMap->GetNode(FeatureName);
		ptr->FromString(Val);
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonSetStringFeature"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonSetBoolFeature(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, BOOL Val)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();
		CBooleanPtr ptr = pNodeMap->GetNode(FeatureName);
		ptr->SetValue(Val==1);
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonSetBoolFeature"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonSetFloatFeature(/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, double Val)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();
		CFloatPtr ptr = pNodeMap->GetNode(FeatureName);
		ptr->SetValue(Val);
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonSetFloatFeature"), e);
		return FALSE;
	}
}


PYLONDLL_API BOOL _stdcall PylonExecCommand(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *Command)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();
		CCommandPtr ptr = pNodeMap->GetNode(Command);
		ptr->Execute();
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonExecCommand"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonGetCommandStatus(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *Command, BOOL *bDone)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();
		CCommandPtr ptr = pNodeMap->GetNode(Command);
		*bDone = ptr->IsDone() ?  TRUE : FALSE;
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetCommandStatus"), e);
		return FALSE;
	}
}

// -------------------------------------------------------------
// Feature Getters
// -------------------------------------------------------------

PYLONDLL_API BOOL _stdcall PylonGetEnumFeature(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, 
												char *Entry)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();
		CEnumerationPtr ptr = pNodeMap->GetNode(FeatureName);
		strcpy(Entry, ptr->ToString());
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetEnumFeature"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonGetEnumFeatureIndex(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, 
													 long *Index)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();
		CEnumerationPtr ptr = pNodeMap->GetNode(FeatureName);
		*Index =(long) ptr->GetIntValue();
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetEnumFeature"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonGetNumEnumEntries(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, 
												   long *Num)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();
		CEnumerationPtr ptr = pNodeMap->GetNode(FeatureName);
		NodeList_t entries;
		ptr->GetEntries(entries);
		*Num =(long) entries.size();
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetNumEnumEntries"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonGetEnumEntryByIndex(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, long Index, 
													 char *Entry, char *DisplayName, EPylonAccessMode *mode)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();
		CEnumerationPtr ptr = pNodeMap->GetNode(FeatureName);
		NodeList_t entries;
		ptr->GetEntries(entries);
		if(mode)
			*mode =(EPylonAccessMode) entries[Index]->GetAccessMode();
		CEnumEntryPtr ptrEntry = entries[Index];
		strcpy(Entry, (const char *) ptrEntry->GetSymbolic());
		if(DisplayName)
			strcpy(DisplayName, (const char *) ptrEntry->GetNode()->GetDisplayName());
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetEnumEntryByIndex"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonGetIntFeature(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, 
											   long *Val, long *Min, long *Max, long *Inc)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();
		CIntegerPtr ptr = pNodeMap->GetNode(FeatureName);
		*Val =(long) ptr->GetValue();
		if(Min)
			*Min =(long) ptr->GetMin(); 
		if(Max)
			*Max =(long) ptr->GetMax(); 
		if(Inc)
			*Inc =(long) ptr->GetInc(); 

		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetIntFeature"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonGetFloatFeature(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, 
												 double *Val, double *Min, double *Max)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();
		CFloatPtr ptr = pNodeMap->GetNode(FeatureName);
		*Val = ptr->GetValue();
		if(Min)
			*Min = ptr->GetMin(); 
		if(Max)
			*Max = ptr->GetMax(); 

		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetFloatFeature"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonGetStringFeature(/*In*/ PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, 
												  /*Out*/ char *Val)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();
		CStringPtr ptr = pNodeMap->GetNode(FeatureName);
		strcpy(Val, (const char *) ptr->ToString());
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetStringFeature"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonGetBoolFeature(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, BOOL *Val)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();
		CBooleanPtr ptr = pNodeMap->GetNode(FeatureName);
		*Val = ptr->GetValue();
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetBoolFeature"), e);
		return FALSE;
	}
}

// -------------------------------------------------------------
// Feature Tester
// -------------------------------------------------------------

PYLONDLL_API BOOL _stdcall PylonGetFeatureAvailability(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, 
														EPylonAccessMode *mode)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();

		if(pNodeMap->GetNode(FeatureName) && GenApi::IsAvailable(pNodeMap->GetNode(FeatureName)))
		{
			if(mode)
				*mode =(EPylonAccessMode) pNodeMap->GetNode(FeatureName)->GetAccessMode();		
			return TRUE;
		}
		else
		{
			if(mode)
				*mode =(EPylonAccessMode) GenApi::_UndefinedAccesMode;
			return FALSE;
		}
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetFeatureAvailability"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonGetEnumEntryAvailability(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *FeatureName, const char *EntryName, 
														  EPylonAccessMode *mode)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();

		CEnumerationPtr ptr = pNodeMap->GetNode(FeatureName);
		IEnumEntry *Entry = ptr->GetEntryByName(EntryName);
		if(Entry && GenApi::IsAvailable(Entry))
		{
			if(mode)
				*mode =(EPylonAccessMode) Entry->GetAccessMode();
			return TRUE;
		}
		else
		{
			if(mode)
				*mode =(EPylonAccessMode) GenApi::_UndefinedAccesMode;
			return FALSE;
		}
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetEnumEntryAvailability"), e);
		return FALSE;
	}
}


// -------------------------------------------------------------
// Nodemap 
// -------------------------------------------------------------

PYLONDLL_API BOOL _stdcall PylonGetNumChildNodes(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *NodeName, long *NumRootNodes)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();

		INode *pRootNode = pNodeMap->GetNode(NodeName);
		NodeList_t Nodes;
		pRootNode->GetChildren(Nodes);
		*NumRootNodes =(long) Nodes.size();
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetNumChildNodes"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonGetChildNodeByIndex(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *ParentNodeName, long Index, char *ChildNodeName, EPylonVisibility *Visibility)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();

		INode *pRootNode = pNodeMap->GetNode(ParentNodeName);
		NodeList_t Nodes;
		pRootNode->GetChildren(Nodes);
		gcstring value = Nodes[Index]->GetName();
		strcpy(ChildNodeName, (const char *) value);
		*Visibility =(EPylonVisibility) Nodes[Index]->GetVisibility();
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetChildNodeByIndex"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonGetNodeInfo(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *NodeName, EPylonInterface *PylonInterface, EPylonAccessMode *AccessMode, EPylonVisibility *Visibility, char *DisplayName, char *ToolTip)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();

		INode *pNode = pNodeMap->GetNode(NodeName);
		strcpy(DisplayName, (const char *) pNode->GetDisplayName());
		strcpy(ToolTip, (const char *) pNode->GetToolTip());

		if(Visibility)
			*Visibility =(EPylonVisibility) pNode->GetVisibility();
		if(AccessMode)
			*AccessMode =(EPylonAccessMode) pNode->GetAccessMode();

		if(PylonInterface)
		{
			IEnumeration *ptrEnum = dynamic_cast<IEnumeration *>(pNode);
			if(ptrEnum)
			{
				*PylonInterface = IPylonEnumeration;
				return true;
			}

			IInteger *ptrInt = dynamic_cast<IInteger *>(pNode);
			if(ptrInt)
			{
				*PylonInterface = IPylonInteger;
				return true;
			}

			IFloat *ptrFloat = dynamic_cast<IFloat *>(pNode);
			if(ptrFloat)
			{
				*PylonInterface = IPylonFloat;
				return true;
			}

			IString *ptrStr = dynamic_cast<IString *>(pNode);
			if(ptrStr)
			{
				*PylonInterface = IPylonString;
				return true;
			}

			IBoolean *ptrBool = dynamic_cast<IBoolean *>(pNode);
			if(ptrBool)
			{
				*PylonInterface = IPylonBoolean;
				return true;
			}

			GenApi::ICommand *ptrCmd = dynamic_cast<GenApi::ICommand *>(pNode);
			if(ptrCmd)
			{
				*PylonInterface = IPylonCommand;
				return true;
			}
			*PylonInterface = IPylonNode;
		}

		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetNodeInfo"), e);
		return FALSE;
	}
}



// -------------------------------------------------------------
// Grabbing
// -------------------------------------------------------------

PYLONDLL_API BOOL _stdcall PylonPrepareGrab(PYLONCAMHANDLE hCamera)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
		pStreamGrabber->PrepareGrab();
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonPrepareGrab"), e);
		return FALSE;
	}
}

PYLONDLL_API PYLONBUFFERHANDLE _stdcall PylonRegisterBuffer(PYLONCAMHANDLE hCamera, unsigned char *ImageBuffer, size_t BufferSize)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
		StreamBufferHandle hBuffer = pStreamGrabber->RegisterBuffer(ImageBuffer, BufferSize);
		//free(pCamera);
		//free(pStreamGrabber);
		return reinterpret_cast<PYLONBUFFERHANDLE>(hBuffer);
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonRegisterBuffer"), e);
		return NULL;
	}
}

PYLONDLL_API BOOL _stdcall PylonQueueBuffer(PYLONCAMHANDLE hCamera, PYLONBUFFERHANDLE hBuffer, long BufferIndex)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
		StreamBufferHandle *pStreamBuffer = PylonBufferFromHandle(hBuffer);
		pStreamGrabber->QueueBuffer(pStreamBuffer, (void *) BufferIndex);
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonQueueBuffer"), e);
		return FALSE;
	}
}

PYLONDLL_API PYLONBUFFERHANDLE _stdcall PylonGetBuffer(PYLONCAMHANDLE hCamera, long timeout, 
														long *BufferIndex, EPylonGrabStatus *status, char *ErrStr)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
		GrabResult Result;

		if(pStreamGrabber->GetWaitObject().Wait(timeout))
		{
			pStreamGrabber->RetrieveResult(Result);
			*status = (EPylonGrabStatus) Result.Status();
			//if(Pylon::Grabbed == Result.Status())
			*BufferIndex =(int) Result.Context();

			//else {
			if(ErrStr && Result.Status() == Pylon::Failed)
				strcpy(ErrStr, (const char *) Result.GetErrorDescription());
			//}

			return reinterpret_cast<PYLONBUFFERHANDLE>(Result.Handle());
		}
		//else {
		//	*status =(EPylonGrabStatus)Pylon::TimeOut;
		//}
		//return reinterpret_cast<PYLONBUFFERHANDLE>(Result.Handle());//NULL;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetBuffer"), e);
		return NULL;
	}
}

// -------------------------------------------------------------
// Color Conversion
// -------------------------------------------------------------

PixelType PatchBayer16Format(const PixelType &pixelType, const unsigned int dynamicRangeMax)
{
	if(dynamicRangeMax !=(1 << 16) - 1)
	{
		switch(dynamicRangeMax)
		{
		case(1 << 10) - 1:
			switch(pixelType)
			{
			case PixelType_BayerBG16:
				return PixelType_BayerBG10;
				break;
			case PixelType_BayerRG16:
				return PixelType_BayerRG10;
				break;
			case PixelType_BayerGR16:
				return PixelType_BayerGR10;
				break;
			case PixelType_BayerGB16:
				return PixelType_BayerGB10;
				break;
			case PixelType_Mono16:
				return PixelType_Mono10;
				break;
			}
			break;
		case(1 << 12) - 1:
			switch(pixelType)
			{
			case PixelType_BayerBG16:
				return PixelType_BayerBG12;
				break;
			case PixelType_BayerRG16:
				return PixelType_BayerRG12;
				break;
			case PixelType_BayerGR16:
				return PixelType_BayerGR12;
				break;
			case PixelType_BayerGB16:
				return PixelType_BayerGB12;
				break;
			case PixelType_Mono16:
				return PixelType_Mono12;
				break;
			}
			break;
		}
	}

	return PixelType_BayerBG10;
}

PixelType PixelFormatByString(const char *PixelFormat)
{
	if(!_stricmp(PixelFormat, "Mono8")) return PixelType_Mono8;
	if(!_stricmp(PixelFormat, "Mono8signed")) return PixelType_Mono8signed;
	if(!_stricmp(PixelFormat, "Mono10")) return PixelType_Mono10;
	if(!_stricmp(PixelFormat, "Mono10packed")) return PixelType_Mono10packed;
	if(!_stricmp(PixelFormat, "Mono12")) return PixelType_Mono12;
	if(!_stricmp(PixelFormat, "Mono12packed")) return PixelType_Mono12packed;
	if(!_stricmp(PixelFormat, "Mono16")) return PixelType_Mono16;
	if(!_stricmp(PixelFormat, "BayerGR8")) return PixelType_BayerGR8;
	if(!_stricmp(PixelFormat, "BayerRG8")) return PixelType_BayerRG8;
	if(!_stricmp(PixelFormat, "BayerGB8")) return PixelType_BayerGB8;
	if(!_stricmp(PixelFormat, "BayerBG8")) return PixelType_BayerBG8;
	if(!_stricmp(PixelFormat, "BayerGR10")) return PixelType_BayerGR10;
	if(!_stricmp(PixelFormat, "BayerRG10")) return PixelType_BayerRG10;
	if(!_stricmp(PixelFormat, "BayerGB10")) return PixelType_BayerGB10;
	if(!_stricmp(PixelFormat, "BayerBG10")) return PixelType_BayerBG10;
	if(!_stricmp(PixelFormat, "BayerGR12")) return PixelType_BayerGR12;
	if(!_stricmp(PixelFormat, "BayerRG12")) return PixelType_BayerRG12;
	if(!_stricmp(PixelFormat, "BayerGB12")) return PixelType_BayerGB12;
	if(!_stricmp(PixelFormat, "BayerBG12")) return PixelType_BayerBG12;
	if(!_stricmp(PixelFormat, "RGB8packed")) return PixelType_RGB8packed;
	if(!_stricmp(PixelFormat, "BGR8packed")) return PixelType_BGR8packed;
	if(!_stricmp(PixelFormat, "RGBA8packed")) return PixelType_RGBA8packed;
	if(!_stricmp(PixelFormat, "BGRA8packed")) return PixelType_BGRA8packed;
	if(!_stricmp(PixelFormat, "RGB10packed")) return PixelType_RGB10packed;
	if(!_stricmp(PixelFormat, "BGR10packed")) return PixelType_BGR10packed;
	if(!_stricmp(PixelFormat, "RGB12packed")) return PixelType_RGB12packed;
	if(!_stricmp(PixelFormat, "BGR12packed")) return PixelType_BGR12packed;
	if(!_stricmp(PixelFormat, "BGR10V1packed")) return PixelType_BGR10V1packed;
	if(!_stricmp(PixelFormat, "BGR10V2packed")) return PixelType_BGR10V2packed;
	if(!_stricmp(PixelFormat, "YUV411packed")) return PixelType_YUV411packed;
	if(!_stricmp(PixelFormat, "YUV422packed")) return PixelType_YUV422packed;
	if(!_stricmp(PixelFormat, "YUV444packed")) return PixelType_YUV444packed;
	if(!_stricmp(PixelFormat, "RGB8planar")) return PixelType_RGB8planar;
	if(!_stricmp(PixelFormat, "RGB10planar")) return PixelType_RGB10planar;
	if(!_stricmp(PixelFormat, "RGB12planar")) return PixelType_RGB12planar;
	if(!_stricmp(PixelFormat, "RGB16planar")) return PixelType_RGB16planar;
	if(!_stricmp(PixelFormat, "YUV422_YUYV_Packed")) return PixelType_YUV422_YUYV_Packed;
	if(!_stricmp(PixelFormat, "BayerGB12Packed")) return PixelType_BayerGB12Packed;
	if(!_stricmp(PixelFormat, "BayerGR12Packed")) return PixelType_BayerGR12Packed;
	if(!_stricmp(PixelFormat, "BayerRG12Packed")) return PixelType_BayerRG12Packed;
	if(!_stricmp(PixelFormat, "BayerBG12Packed")) return PixelType_BayerBG12Packed;
	if(!_stricmp(PixelFormat, "BayerGR16")) return PixelType_BayerGR16;
	if(!_stricmp(PixelFormat, "BayerRG16")) return PixelType_BayerRG16;
	if(!_stricmp(PixelFormat, "BayerGB16")) return PixelType_BayerGB16;
	if(!_stricmp(PixelFormat, "BayerBG16")) return PixelType_BayerBG16;
	if(!_stricmp(PixelFormat, "RGB12V1packed")) return PixelType_RGB12V1packed;
	if(!_stricmp(PixelFormat, "Double")) return PixelType_Double;
	return PixelType_Undefined;
}

bool IsColorFormat(const PixelType &pixelType)
{
	switch(pixelType)
	{
	case PixelType_BayerGR8:
	case PixelType_BayerRG8:
	case PixelType_BayerGB8:
	case PixelType_BayerBG8:
	case PixelType_BayerGR10:
	case PixelType_BayerRG10:
	case PixelType_BayerGB10:
	case PixelType_BayerBG10:
	case PixelType_BayerGR12:
	case PixelType_BayerRG12:
	case PixelType_BayerGB12:
	case PixelType_BayerBG12:
	case PixelType_RGB8packed:
	case PixelType_BGR8packed:
	case PixelType_RGBA8packed:
	case PixelType_BGRA8packed:
	case PixelType_RGB10packed:
	case PixelType_BGR10packed:
	case PixelType_RGB12packed:
	case PixelType_BGR12packed:
	case PixelType_BGR10V1packed:
	case PixelType_BGR10V2packed:
	case PixelType_YUV411packed:
	case PixelType_YUV422packed:
	case PixelType_YUV444packed:
	case PixelType_RGB8planar:
	case PixelType_RGB10planar:
	case PixelType_RGB12planar:
	case PixelType_RGB16planar:
	case PixelType_YUV422_YUYV_Packed:
	case PixelType_BayerGB12Packed:
	case PixelType_BayerGR12Packed:
	case PixelType_BayerRG12Packed:
	case PixelType_BayerBG12Packed:
	case PixelType_BayerGR16:
	case PixelType_BayerRG16:
	case PixelType_BayerGB16:
	case PixelType_BayerBG16:
	case PixelType_RGB12V1packed:
		return true;
		break;
	case PixelType_Mono8:
	case PixelType_Mono8signed:
	case PixelType_Mono10:
	case PixelType_Mono10packed:
	case PixelType_Mono12:
	case PixelType_Mono12packed:
	case PixelType_Mono16:
		return false;
		break;
	}

	return false;
}

class CPylonColorConverter
{
public:
	CPylonColorConverter(CPixelFormatConverter *converter)
	{
		m_pColorConverter = converter;
	}

	~CPylonColorConverter()
	{
		delete m_pColorConverter;
	}

	void Init(const struct SImageFormat &imageFormat)
	{
		m_pColorConverter->Init(imageFormat);
	}

	void Convert( void *pDest, unsigned int sizeDest, const void *pSrcRaw, unsigned int sizeRaw)
	{
		m_pColorConverter->Convert(pDest, sizeDest, pSrcRaw, sizeRaw, InFmt, OutFmt);
	}

private:
	CPixelFormatConverter *m_pColorConverter;

public:
	SImageFormat InFmt;
	SOutputImageFormat OutFmt;
};

//PYLONDLL_API PYLONCCHANDLE _stdcall PylonOpenColorConverter(PYLONCAMHANDLE hCamera, const char *PixelFormat, long Width, long Height, BOOL *bIsColorFormat)
//{
//	try
//	{
//		PixelType InType = PixelFormatByString(PixelFormat);
//
//		// If the camera reports Mono16 or Bayer16, the current dynamic range might be less than 2^16, so
//		// ask the camera for it's dynamic range and return the matching pixel format value.
//		if(   InType == PixelType_BayerBG16 
//			|| InType == PixelType_BayerGB16
//			|| InType == PixelType_BayerGR16
//			|| InType == PixelType_BayerRG16
//			|| InType == PixelType_Mono16)
//		{
//			IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
//			INodeMap *pNodeMap = pCamera->GetNodeMap();
//			CIntegerPtr ptrDynamicRange = pNodeMap->GetNode("PixelDynamicRangeMax");
//			InType =(PixelType) PatchBayer16Format(InType, (UINT) ptrDynamicRange->GetValue());
//		}
//
//		SImageFormat InFmt;
//		InFmt.Width = Width;
//		InFmt.Height = Height;
//		InFmt.PixelFormat = InType;
//		InFmt.LinePitch = Width *((InType&0x00FF0000)>>16)/8;
//
//		CPylonColorConverter *pColorConverter;
//		switch(InType)
//		{
//		case PixelType_BayerGR16:
//		case PixelType_BayerRG16:
//		case PixelType_BayerGB16:
//		case PixelType_BayerBG16:
//		case PixelType_BayerGR8:
//		case PixelType_BayerRG8:
//		case PixelType_BayerGB8:
//		case PixelType_BayerBG8:
//		case PixelType_BayerGR10:
//		case PixelType_BayerRG10:
//		case PixelType_BayerGB10:
//		case PixelType_BayerBG10:
//		case PixelType_BayerGR12:
//		case PixelType_BayerRG12:
//		case PixelType_BayerGB12:
//		case PixelType_BayerBG12:
//			pColorConverter = new CPylonColorConverter(new CPixelFormatConverterBayer());
//			break;
//		case PixelType_BayerGB12Packed:
//		case PixelType_BayerGR12Packed:
//		case PixelType_BayerRG12Packed:
//		case PixelType_BayerBG12Packed:
//			InFmt.LinePitch = 0; // Packet formats do not have Line Pitch
//			pColorConverter = new CPylonColorConverter(new CPixelFormatConverterBayer());
//			break;
//		case PixelType_RGB8packed:
//		case PixelType_BGR8packed:
//		case PixelType_RGBA8packed:
//		case PixelType_BGRA8packed:
//		case PixelType_RGB12packed:
//		case PixelType_BGR12packed:
//			pColorConverter = new CPylonColorConverter(new CPixelFormatConverterRGB());
//			break;
//		case PixelType_RGB12V1packed:
//			InFmt.LinePitch = 0; // Packet formats do not have Line Pitch
//			pColorConverter = new CPylonColorConverter(new CPixelFormatConverterRGB());
//			break;
//		case PixelType_YUV422packed:
//			pColorConverter = new CPylonColorConverter(new CPixelFormatConverterYUV422());
//			break;
//		case PixelType_YUV422_YUYV_Packed:
//			pColorConverter = new CPylonColorConverter(new CPixelFormatConverterYUV422YUYV());
//			break;
//		case PixelType_Mono10:
//		case PixelType_Mono12:
//		case PixelType_Mono16:
//			pColorConverter = new CPylonColorConverter(new CPixelFormatConverterTruncate());
//			break;
//		case PixelType_Mono10packed:
//		case PixelType_Mono12packed:
//			InFmt.LinePitch = 0; // Packet formats do not have Line Pitch
//			pColorConverter = new CPylonColorConverter(new CPixelFormatConverterTruncatePacked());
//			break;
//		default:
//			GenICam::GenericException e("Image Format not supported!", __FILE__,__LINE__);
//			throw(e);
//		}
//
//		SOutputImageFormat OutFmt;
//		if(! IsColorFormat(InType))
//		{
//			OutFmt.LinePitch = Width; // Mono8 always 1 Bytes / Pixel
//			OutFmt.PixelFormat = PixelType_Mono8;
//			*bIsColorFormat = FALSE;
//		}
//		else
//		{
//			OutFmt.LinePitch = Width * 4; // RGBA always 4 Bytes / Pixel
//			OutFmt.PixelFormat = PixelType_RGBA8packed;
//			*bIsColorFormat = TRUE;
//		}
//		pColorConverter->InFmt = InFmt;
//		pColorConverter->OutFmt = OutFmt;
//		pColorConverter->Init(InFmt);
//		return reinterpret_cast<PYLONCCHANDLE>(pColorConverter);
//	}
//	catch(GenICam::GenericException& e)
//	{
//		PylonSetLastError(gcstring("PylonOpenColorConverter"), e);
//		return NULL;
//	}
//}

PYLONDLL_API BOOL _stdcall PylonCloseColorConverter(PYLONCCHANDLE hColorConverter)
{
	CPylonColorConverter *pColorConverter = PylonColorConverterFromHandle(hColorConverter);
	delete pColorConverter;
	return TRUE;
}

PYLONDLL_API BOOL _stdcall PylonConvertImageFormat(PYLONCCHANDLE hColorConverter, unsigned char *InBuffer, long InBufSize, long OutBufSize, unsigned char *OutBuffer)
{
	try
	{
		CPylonColorConverter *pColorConverter = PylonColorConverterFromHandle(hColorConverter);
		pColorConverter->Convert((void *) OutBuffer, OutBufSize, (void *) InBuffer, InBufSize);
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonConvertImageFormat"), e);
		return FALSE;
	}
}

// -------------------------------------------------------------
// Parameter Changes Callback Handler
// -------------------------------------------------------------

typedef void(_stdcall *PtrToVBFunction)(void *pNode);

PYLONDLL_API PYLONCALLBACKHANDLE _stdcall PylonRegisterParamCB(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *NodeName, void *pFunc)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();

		INode *pNode = pNodeMap->GetNode(NodeName);
		if(pNode)
		{
			PtrToVBFunction pvbFunc =(PtrToVBFunction) pFunc;
			return reinterpret_cast<PYLONCALLBACKHANDLE>(GenApi::Register(pNode, pvbFunc));
		}
		else
		{
			gcstring errstr = gcstring(NodeName) + ": Feature is not available!";
			GenICam::GenericException e(errstr, __FILE__,__LINE__);
			throw(e);
		}
		return NULL;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonRegisterParamCB"), e);
		return NULL;
	}
}

PYLONDLL_API BOOL _stdcall PylonUnRegisterParamCB(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, PYLONCALLBACKHANDLE hParamCB, const char *NodeName)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();

		INode *pNode = pNodeMap->GetNode(NodeName);
		if(pNode)
		{
			pNode->DeregisterCallback(PylonCallbackFromHandle(hParamCB));		
		}
		else
		{
			gcstring errstr = gcstring(NodeName) + ": Feature is not available!";
			GenICam::GenericException e(errstr, __FILE__,__LINE__);
			throw(e);
		}
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonUnRegisterParamCB"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonGetFeatureByNode(void *pInNode, EPylonInterface *PylonInterface, char *FeatureName, char *CurrVal)
{
	try
	{
		INode *pNode =(INode *) pInNode;
		strcpy(FeatureName, (const char *) pNode->GetName());

		if( GenApi::IsReadable(pNode))
		{
			CValuePtr ptrValue(pNode);
			strcpy(CurrVal, (const char *) ptrValue->ToString());
		}

		IEnumeration *ptrEnum = dynamic_cast<IEnumeration *>(pNode);
		if(ptrEnum)
		{
			*PylonInterface = IPylonEnumeration;
			return true;
		}

		IInteger *ptrInt = dynamic_cast<IInteger *>(pNode);
		if(ptrInt)
		{
			*PylonInterface = IPylonInteger;
			return true;
		}

		IFloat *ptrFloat = dynamic_cast<IFloat *>(pNode);
		if(ptrFloat)
		{
			*PylonInterface = IPylonFloat;
			return true;
		}

		IString *ptrStr = dynamic_cast<IString *>(pNode);
		if(ptrStr)
		{
			*PylonInterface = IPylonString;
			return true;
		}

		IBoolean *ptrBool = dynamic_cast<IBoolean *>(pNode);
		if(ptrBool)
		{
			*PylonInterface = IPylonBoolean;
			return true;
		}

		GenApi::ICommand *ptrCmd = dynamic_cast<GenApi::ICommand *>(pNode);
		if(ptrCmd)
		{
			*PylonInterface = IPylonCommand;
			return true;
		}

		*PylonInterface = IPylonNode;
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetFeatureByNode"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonGetNodeAddress(PYLONCAMHANDLE hCamera, EPylonFeatureType ftype, const char *NodeName, long *Address)
{
	try
	{
		IPylonDevice *pCamera = PylonCamFromHandle(hCamera);
		INodeMap *pNodeMap = 0;
		if(ftype == STREAM_FEATURE)
		{
			IStreamGrabber *pStreamGrabber = pCamera->GetStreamGrabber(0);
			pNodeMap = pStreamGrabber->GetNodeMap();
		}
		else if(ftype == EVENT_FEATURE)
		{
			IEventGrabber *pEventGrabber = pCamera->GetEventGrabber();
			pNodeMap = pEventGrabber->GetNodeMap();
		}
		else
			pNodeMap = pCamera->GetNodeMap();

		INode *pNode = pNodeMap->GetNode(NodeName);
		if(pNode)
		{
			*Address =(long)pNode;
		}
		else
		{
			gcstring errstr = gcstring(NodeName) + ": Node not available!";
			GenICam::GenericException e(errstr, __FILE__,__LINE__);
			throw(e);
		}
		return TRUE;
	}
	catch(GenICam::GenericException& e)
	{
		PylonSetLastError(gcstring("PylonGetNodeAddress"), e);
		return FALSE;
	}
}

PYLONDLL_API BOOL _stdcall PylonGetNodeByNode(void *pInNode, long *Address)
{
	*Address =(long)(INode *) pInNode;
	return TRUE;
}
/// Pipe interface enhanced and cleaned up to suit IPC requirement with the win32 Surrogate app from x64 system;
/// Udupa;

#include "stdafx.h"
#include "PipeMonitor.h"
#include "IOCode.h"
#include "DebugLog.h"

using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

///Udupa; Jan'2013
BOOL LaunchProcess(CString strProcess, HANDLE* phProcess)
{
	PROCESS_INFORMATION processInformation;
	memset(&processInformation, 0, sizeof(processInformation));
	STARTUPINFO startupInfo;
	memset(&startupInfo, 0, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);

	BOOL bSuccess = CreateProcess(NULL, strProcess.GetBuffer(), 
		NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, 
		&startupInfo, &processInformation);
	strProcess.ReleaseBuffer();
	if(!bSuccess) {
		OutputDebugLogTo(1, TRUE, "Failed to launch [%s] application (%d)", strProcess, GetLastError());
		if(phProcess)
			*phProcess = 0;
		return FALSE;
	}

	if(phProcess)
		*phProcess = processInformation.hProcess;
	else {
		CloseHandle(processInformation.hProcess);
		CloseHandle(processInformation.hThread);
		OutputDebugLogTo(1, TRUE, "Launched remote application '%s'", strProcess);
	}

	return TRUE;
}

BOOL CoInitialize()
{
	return TRUE;

    HRESULT hres;

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    //hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 
    //if (FAILED(hres)) {
    //    OutputDebugLogTo(1, TRUE, "Failed to initialize COM library. Error code = 0x%X", hres);
    //    return FALSE;
    //}

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------
    // Note: If you are using Windows 2000, you must specify -
    // the default authentication credentials for a user by using
    // a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
    // parameter of CoInitializeSecurity ------------------------

    hres =  CoInitializeSecurity(
        NULL, 
        -1,                          // COM negotiates service
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );
    
	if(FAILED(hres)) {
        OutputDebugLogTo(1, TRUE, "Failed to initialize security. Error code = 0x%X", hres);
        CoUninitialize();
        return FALSE;
    }

	return TRUE;
}

///Udupa; Dec'2013; Added remote process launch feature
# pragma comment(lib, "wbemuuid.lib")
BOOL LaunchRemoteProcess(CString strIpAddress, CString strProcess)
{
	return TRUE;

    HRESULT hres;

     // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,             
		0, 
		CLSCTX_INPROC_SERVER, 
		IID_IWbemLocator, (LPVOID *) &pLoc);

	if(FAILED(hres)) {
		OutputDebugLogTo(1, TRUE, "Failed to create IWbemLocator object. Err code = 0x%X", hres);
		///CoUninitialize();
		return FALSE;
	}

    // Step 4: ---------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices *pSvc = NULL;
 
	CString strNode;
	strNode.Format("\\\\%s\\ROOT\\CIMV2", strIpAddress);

    // Connect to the local root\cimv2 namespace
    // and obtain pointer pSvc to make IWbemServices calls.
    hres = pLoc->ConnectServer(
        _bstr_t(strNode.GetBuffer()), 
        NULL,
        NULL, 
        0, 
        NULL, 
        0, 
        0, 
        &pSvc
    );
	strNode.ReleaseBuffer();
     
    if(FAILED(hres)) {
        OutputDebugLogTo(1, TRUE, "Could not connect. Error code = 0x%X" , hres);
        pLoc->Release();
        ///CoUninitialize();
        return FALSE;
    }

    OutputDebugLogTo(1, TRUE, "Connected to %s WMI namespace", strNode);


    // Step 5: --------------------------------------------------
    // Set security levels for the proxy ------------------------

    hres = CoSetProxyBlanket(
        pSvc,                        // Indicates the proxy to set
        RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx 
        RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx 
        NULL,                        // Server principal name 
        RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
        RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
        NULL,                        // client identity
        EOAC_NONE                    // proxy capabilities 
    );

    if(FAILED(hres)) {
        OutputDebugLogTo(1, TRUE, "Could not set proxy blanket. Error code = 0x%X", hres);
        pSvc->Release();
        pLoc->Release();     
        ///CoUninitialize();
        return FALSE;
    }
	
	
	// Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----

    // set up to call the Win32_Process::Create method
    BSTR MethodName = SysAllocString(L"Create");
    BSTR ClassName = SysAllocString(L"Win32_Process");
    BSTR ProcessName = strProcess.AllocSysString();

    IWbemClassObject* pClass = NULL;
    hres = pSvc->GetObject(ClassName, 0, NULL, &pClass, NULL);

    IWbemClassObject* pInParamsDefinition = NULL;
    hres = pClass->GetMethod(MethodName, 0, 
        &pInParamsDefinition, NULL);

    IWbemClassObject* pClassInstance = NULL;
    hres = pInParamsDefinition->SpawnInstance(0, &pClassInstance);

    // Create the values for the in parameters
    VARIANT varCommand;
    varCommand.vt = VT_BSTR;
    varCommand.bstrVal = ProcessName;

    // Store the value for the in parameters
    hres = pClassInstance->Put(L"CommandLine", 0,
        &varCommand, 0);
    wprintf(L"The command is: %s\n", V_BSTR(&varCommand));

    // Execute Method
    IWbemClassObject* pOutParams = NULL;
    hres = pSvc->ExecMethod(ClassName, MethodName, 0,
    NULL, pClassInstance, &pOutParams, NULL);

    if(FAILED(hres)) {
        OutputDebugLogTo(1, TRUE, "Could not execute method. Error code = 0x%X", hres);
        VariantClear(&varCommand);
		SysFreeString(ProcessName);
        SysFreeString(ClassName);
        SysFreeString(MethodName);
        pClass->Release();
        pInParamsDefinition->Release();
        pOutParams->Release();
        pSvc->Release();
        pLoc->Release();     
        ///CoUninitialize();
        return FALSE;               // Program has failed.
    }

    // To see what the method returned,
    // use the following code.  The return value will
    // be in &varReturnValue
    VARIANT varReturnValue;
    hres = pOutParams->Get(_bstr_t(L"ReturnValue"), 0, 
        &varReturnValue, NULL, 0);


	OutputDebugLogTo(1, TRUE, "Launched Remote Application '%s' (%d)", strProcess, varReturnValue.intVal);

    // Clean up
    //--------------------------
    VariantClear(&varCommand);
	SysFreeString(ProcessName);
    VariantClear(&varReturnValue);
    SysFreeString(ClassName);
    SysFreeString(MethodName);
    pClass->Release();
    pInParamsDefinition->Release();
    pOutParams->Release();
    pLoc->Release();
    pSvc->Release();
    ///CoUninitialize();
   
	return TRUE;
}

CPipeMonitor::CPipeMonitor()
{
	m_hPipe = 0;
	m_hDisconnect = 0;
	m_bServerApplication=FALSE;
	m_hPipeMonitorThread = 0;
	m_bConnected = FALSE;
}

BOOL CPipeMonitor::Create(CString strPipeName, BOOL bServerApplication)
{
	m_bServerApplication = bServerApplication;
	m_hClientProcess = NULL;
	CString strPipe;
	strPipe.Format("\\\\.\\pipe\\%s", strPipeName);
	if(bServerApplication)
		m_hPipe = CreateNamedPipe(strPipe.GetBuffer(0), PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED, PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT, 100, 1000, 1000, 5000, NULL);
	else
		m_hPipe = CreateFile(strPipe.GetBuffer(0), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

	if(m_hPipe == INVALID_HANDLE_VALUE) {
		DWORD dwError = GetLastError();
		switch(dwError) {
		case ERROR_ACCESS_DENIED:
			OutputDebugLogTo(0, TRUE, "[%s] Access Denied", bServerApplication ? "Server" : "Client");
			break;
		case ERROR_INVALID_PARAMETER:
			OutputDebugLogTo(0, TRUE, "[%s] Access Denied", bServerApplication ? "Server" : "Client");
			break;

		default:
			//::AfxMessageBox("Unable to create Pipe");
			OutputDebugLogTo(0, TRUE, "[%s] Unable to create Pipe", bServerApplication ? "Server" : "Client");
			break;
		}
		m_hPipe = 0;
		return FALSE;
	}
	
	if((m_hDisconnect = CreateEvent(NULL, TRUE, FALSE, NULL)) == INVALID_HANDLE_VALUE) {
		CloseHandle(m_hPipe);
		m_hPipe = 0;
		m_hDisconnect = 0;
		//::AfxMessageBox("Unable to create the disconnect event");
		OutputDebugLogTo(0, TRUE, "[%s] Unable to create the disconnect event", bServerApplication ? "Server" : "Client");
		return FALSE;
	}

	InitializeCriticalSection(&m_csMessageList);
	if((m_hMessage = CreateEvent(NULL, FALSE, FALSE, NULL)) == INVALID_HANDLE_VALUE) {
		m_hMessage = NULL;
		//::AfxMessageBox("Unable to create event");
		OutputDebugLogTo(0, TRUE, "[%s] Unable to create event", bServerApplication ? "Server" : "Client");
		return FALSE;
	}

	DWORD dwThreadId;
	if((m_hPipeMonitorThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PipeMonitor, this, 0, &dwThreadId)) == INVALID_HANDLE_VALUE) {
		CloseHandle(m_hPipe);
		CloseHandle(m_hDisconnect);
		m_hPipe = 0;
		m_hDisconnect = 0;
		//::AfxMessageBox("Unable to create pipe monitoring thread");
		OutputDebugLogTo(0, TRUE, "[%s] Unable to create pipe monitoring thread", bServerApplication ? "Server" : "Client");
		return FALSE;
	}

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

	if(bServerApplication) {
		CString strFile = "IOServer";
#ifdef _DEBUG
		strFile += "d";
#endif
		CString strCmd;
		strCmd.Format("IO Server\\%s %s", strFile, strPipeName);
		if(!LaunchProcess(strCmd)) {
			OutputDebugLogTo(0, TRUE, "[%s] Failed to launch win32Server - %s", 
				bServerApplication ? "Server" : "Client", strCmd);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CPipeMonitor::Close()
{
	if(m_bServerApplication) {
		int pData[1] = { CLOSE_IO };
		SendMessage((char*) pData, 4);

		DisconnectNamedPipe(m_hPipe);
	}
	SetEvent(m_hDisconnect);
	WaitForSingleObject(m_hPipeMonitorThread, 3000);
	CloseHandle(m_hDisconnect);
	CloseHandle(m_hMessage);
	CloseHandle(m_hPipeMonitorThread);
	CloseHandle(m_hPipe);
	
	DeleteCriticalSection(&m_csMessageList);

	return TRUE;
}

void CPipeMonitor::SendMessage(char* pData, int nLength)
{
	DWORD dwWritten;
	DWORD dwWaitResult,dwError;
	OVERLAPPED os;

	int nCount = 0;
	while(!m_bConnected) {
		if(WaitForSingleObject(m_hDisconnect, 100) == WAIT_OBJECT_0)
			return;
		if(nCount++ > 10)
			return;
	}
	
	if(pData != NULL) {
		os.hEvent = CreateEvent(0, FALSE, FALSE, NULL);
		if(os.hEvent == INVALID_HANDLE_VALUE) {
			//::AfxMessageBox("Unable to create an event for SendMessage");
			OutputDebugLogTo(0, TRUE, "[%s] Unable to create an event for SendMessage", m_bServerApplication ? "Server" : "Client");
		}
		os.Offset = 0;
		os.OffsetHigh = 0;
		if(!WriteFile(m_hPipe, pData, nLength, &dwWritten, &os)) {
			//TTH, force the file buffer to be processed 
			FlushFileBuffers(m_hPipe);
			Sleep(1);
			
			switch(GetLastError()) {
			case ERROR_IO_PENDING:
				if((dwWaitResult = WaitForSingleObject(os.hEvent, 5000)) == WAIT_FAILED) {
					switch(dwError = GetLastError()) {
					case ERROR_INVALID_HANDLE:
						//::AfxMessageBox("Invalid handle in SendMessage");
						OutputDebugLogTo(0, TRUE, "[%s] Invalid handle in SendMessage", m_bServerApplication ? "Server" : "Client");
						break;
					default:
						CloseHandle(os.hEvent);
						return;
					}
				}
				else if(dwWaitResult == WAIT_TIMEOUT) {
					//::AfxMessageBox("Time out in SendMessage");		
					OutputDebugLogTo(0, TRUE, "[%s] Time out in SendMessage", m_bServerApplication ? "Server" : "Client");
				}
				else {
					switch(dwWaitResult - WAIT_OBJECT_0) {
					case 0:
						break;

					default:
						//::AfxMessageBox("Unknown result in SendMessage");
						OutputDebugLogTo(0, TRUE, "[%s] Unknown result in SendMessage", m_bServerApplication ? "Server" : "Client");
						CloseHandle(os.hEvent);
						return;
					}
				}
				break;

			case ERROR_BROKEN_PIPE:
				//::AfxMessageBox("Broken pipe in SendMessage");
				OutputDebugLogTo(0, TRUE, "[%s] Broken pipe in SendMessage", m_bServerApplication ? "Server" : "Client");
				CloseHandle(os.hEvent);
				return;

			case ERROR_INSUFFICIENT_BUFFER:
				//::AfxMessageBox("Insufficient buffer in SendMessage");
				OutputDebugLogTo(0, TRUE, "[%s] Insufficient buffer in SendMessage", m_bServerApplication ? "Server" : "Client");
				CloseHandle(os.hEvent);
				return;

			case ERROR_INVALID_USER_BUFFER:
				//::AfxMessageBox("Invalid user buffer in SendMessage");
				OutputDebugLogTo(0, TRUE, "[%s] Invalid user buffer in SendMessage", m_bServerApplication ? "Server" : "Client");
				CloseHandle(os.hEvent);
				return;

			case ERROR_MORE_DATA:
				//::AfxMessageBox("More data error in SendMessage");
				OutputDebugLogTo(0, TRUE, "[%s] More data error in SendMessage", m_bServerApplication ? "Server" : "Client");
				CloseHandle(os.hEvent);
				return;

			case ERROR_NOT_ENOUGH_MEMORY:
				//::AfxMessageBox("Insufficient memory in SendMessage");
				OutputDebugLogTo(0, TRUE, "[%s] Insufficient memory in SendMessage", m_bServerApplication ? "Server" : "Client");
				CloseHandle(os.hEvent);
				return;

			default:
				CloseHandle(os.hEvent);
				return;
			}
		}
		CloseHandle(os.hEvent);
	}

	return;
}


void CPipeMonitor::AddToMessageList(CManagedArray* pSrc)
{
	if(pSrc == NULL || pSrc->nLength < 1) {
		OutputDebugLogTo(0, TRUE, "Invalid Pipe Message received [%d]", pSrc->nLength);
		return;
	}

	CManagedArray* pDest = new CManagedArray();
	pDest->pData = new char[pSrc->nLength];
	pDest->nLength = pSrc->nLength;
	memcpy(pDest->pData, pSrc->pData, pSrc->nLength);

	EnterCriticalSection(&m_csMessageList);
	m_MessageList.AddTail((CObject*) pDest);
	SetEvent(m_hMessage);
	LeaveCriticalSection(&m_csMessageList);
}

//void CPipeMonitor::RemoveFromMessageList(CManagedArray*& pDest)
//{
//	EnterCriticalSection(&m_csMessageList);
//	if(m_MessageList.GetCount() > 0)
//		pDest = (CManagedArray*) m_MessageList.RemoveHead();
//	else
//		pDest = NULL;
//	if(m_MessageList.GetCount() == 0)
//		ResetEvent(m_hMessage);
//	LeaveCriticalSection(&m_csMessageList);
//}
//
BOOL CPipeMonitor::WaitForMessage(CManagedArray*& pDest)
{
	pDest = NULL;

	HANDLE phMultipleObjects[2];
	phMultipleObjects[0] = m_hMessage;
	phMultipleObjects[1] = m_hDisconnect;
	DWORD dwResult = WaitForMultipleObjects(2, phMultipleObjects, FALSE, INFINITE);
	if(dwResult != WAIT_OBJECT_0)
		return FALSE;

	EnterCriticalSection(&m_csMessageList);
	if(m_MessageList.GetCount() > 0)
		pDest = (CManagedArray*) m_MessageList.RemoveHead();

	if(m_MessageList.GetCount() > 0)
		SetEvent(m_hMessage);
	LeaveCriticalSection(&m_csMessageList);

	return (pDest != NULL);
}

BOOL CPipeMonitor::WaitForResponse(INT32& nResponse)
{
	CManagedArray* pDest;
	if(!WaitForMessage(pDest)) {
		OutputDebugLogTo(0, TRUE, "[%s] Failed to read return code", m_bServerApplication ? "Server" : "Client");
		return FALSE;
	}
	if(pDest->nLength == 4) {
		memcpy(&nResponse, pDest->pData, 4);
		delete[] pDest->pData;
		delete pDest;
		return TRUE;
	}
	if(pDest != NULL) {
		delete[] pDest->pData;
		delete pDest;
	}

	return FALSE;
}

DWORD FAR PASCAL PipeMonitor(LPSTR lpData) 
{
	CPipeMonitor *pPipeMonitor = (CPipeMonitor *)lpData;
	ASSERT(pPipeMonitor);
	DWORD dwBytesRead;
	OVERLAPPED os;
	DWORD dwWaitResult;
	DWORD dwError;
	HANDLE hReadEvent;
	HANDLE hConnectionEvent;
	char buf[256];
	
	if(pPipeMonitor->m_bServerApplication) {
		hConnectionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if(hConnectionEvent == INVALID_HANDLE_VALUE) {
			//::AfxMessageBox("Unable to create a connection event for PipeMonitor");
			OutputDebugLogTo(0, TRUE, "[%s] Unable to create a connection event for PipeMonitor", pPipeMonitor->m_bServerApplication ? "Server" : "Client");
			return 1;
		}
		os.Offset = 0;
		os.OffsetHigh = 0;
		os.hEvent = hConnectionEvent;
	
		if(!ConnectNamedPipe(pPipeMonitor->m_hPipe, &os)) {
			switch(dwError = GetLastError()) {
			case ERROR_IO_PENDING: // pipe is waiting to be connected
				dwWaitResult = WaitForSingleObject(hConnectionEvent, INFINITE);
				if(dwWaitResult == WAIT_TIMEOUT) {
					//::AfxMessageBox("Time out in PipeMonitor");
					OutputDebugLogTo(0, TRUE, "[%s] Time out in PipeMonitor", pPipeMonitor->m_bServerApplication ? "Server" : "Client");
					CloseHandle(hConnectionEvent);
					///PostMessage(pPipeMonitor->m_hParentWnd, WM_CLOSE, 0, 0);
					return 1;
				}
				switch(dwWaitResult - WAIT_OBJECT_0) 
				{
				//The pipe has been connected
				case 0:
					break;
		
				//The main thread exits
				case 1:
					CloseHandle(hConnectionEvent);
					return 0;

					
				default:
					//::AfxMessageBox("Unknown eror in PipeMonitor");
					OutputDebugLogTo(0, TRUE, "[%s] Unknown eror in PipeMonitor", pPipeMonitor->m_bServerApplication ? "Server" : "Client");
					CloseHandle(hConnectionEvent);
					///PostMessage(pPipeMonitor->m_hParentWnd, WM_CLOSE, 0, 0);
					return 1;
				}
				break;
			
			case ERROR_PIPE_CONNECTED:
				break;

			default:
				//::AfxMessageBox("Unknown eror in PipeMonitor");
				OutputDebugLogTo(0, TRUE, "[%s] Unknown eror in PipeMonitor", pPipeMonitor->m_bServerApplication ? "Server" : "Client");
				CloseHandle(hConnectionEvent);
				return 1;
			}
		}

		CloseHandle(hConnectionEvent);
	}

	hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(hReadEvent == INVALID_HANDLE_VALUE) {
		//::AfxMessageBox("Unable to create an read event for PipeMonitor");
		OutputDebugLogTo(0, TRUE, "[%s] Unable to create an read event for PipeMonitor", pPipeMonitor->m_bServerApplication ? "Server" : "Client");
		return 1;
	}

	HANDLE phMultipleObjects[2];
	phMultipleObjects[0] = hReadEvent;
	phMultipleObjects[1] = pPipeMonitor->m_hDisconnect;

	pPipeMonitor->m_bConnected = TRUE;

	while(TRUE) {
		//Read the pipe
		os.Offset = 0;
		os.OffsetHigh = 0;
		os.hEvent=hReadEvent;
		if(!ReadFile(pPipeMonitor->m_hPipe, buf, 256, &dwBytesRead, &os)) {

			// If there is no data in the pipe...
			switch(dwError = GetLastError()) {
			
			// If the pipe is empty
			case ERROR_IO_PENDING:
				// Wait until either:(a) the pipe gets some data
				//                   (b) the mainthread exits
				//                   (c) The pipe gets disconnected
				if((dwWaitResult = WaitForMultipleObjects(2, phMultipleObjects, FALSE, INFINITE)) == WAIT_FAILED) {
					//::AfxMessageBox("Wait failed");
					OutputDebugLogTo(0, TRUE, "[%s] Wait failed", pPipeMonitor->m_bServerApplication ? "Server" : "Client");
					CloseHandle(hReadEvent);
					SetEvent(pPipeMonitor->m_hDisconnect);
					///PostMessage(pPipeMonitor->m_hParentWnd, WM_CLOSE, 0, 0);
					return 1;
				}
				else {
					switch(dwWaitResult-WAIT_OBJECT_0) {
					// If the pipe gets some data
					case 0:
						os.Offset = 0;
						os.OffsetHigh = 0;
						os.hEvent=hReadEvent;
						GetOverlappedResult(pPipeMonitor->m_hPipe, &os, &dwBytesRead, TRUE);
						break;

					// If the pipe gets disconnected
					case 1:
						//::AfxMessageBox("Pipe disconnected");
						OutputDebugLogTo(0, TRUE, "[%s] Pipe disconnected", pPipeMonitor->m_bServerApplication ? "Server" : "Client");
						CloseHandle(hReadEvent);
						///PostMessage(pPipeMonitor->m_hParentWnd, WM_CLOSE, 0, 0);
						return 0;
				
					default:
						CloseHandle(hReadEvent);
						SetEvent(pPipeMonitor->m_hDisconnect);
						///PostMessage(pPipeMonitor->m_hParentWnd, WM_CLOSE, 0, 0);
						return 1;
					}
				}
				break;

			// If the pipe is broken
			case ERROR_BROKEN_PIPE:
				OutputDebugLogTo(0, TRUE, "[%s] Pipe broken", pPipeMonitor->m_bServerApplication ? "Server" : "Client");
				CloseHandle(hReadEvent);
				SetEvent(pPipeMonitor->m_hDisconnect);
				///PostMessage(pPipeMonitor->m_hParentWnd, WM_CLOSE, 0, 0);
				return 0;
				
			default:
				CloseHandle(hReadEvent);
				SetEvent(pPipeMonitor->m_hDisconnect);
				///PostMessage(pPipeMonitor->m_hParentWnd, WM_CLOSE, 0, 0);
				return 0;
			}
		}

		if(dwBytesRead > 0) {
			// if some data is read from the pipe
			buf[dwBytesRead] = 0;
			CManagedArray arr;
			arr.pData = buf;
			arr.nLength = dwBytesRead;
			pPipeMonitor->AddToMessageList(&arr);
		}
	}
	
	CloseHandle(hReadEvent);
	return 0;
}


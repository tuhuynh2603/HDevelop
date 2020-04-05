#include "stdafx.h"
#include "DebugLog.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OutputDebugLogString(LPCTSTR lpOutputString)
{
	///Udupa; July'2012
	LogMessage(0, lpOutputString);
	return;

	/* before we do anything else, make sure DebugLog process is open and waiting */
    HANDLE heventDebugLog = OpenEvent(EVENT_ALL_ACCESS, FALSE, "DEBUGLOG_BUFFER_READY");
    if ( !heventDebugLog )
    {
        //MessageBox(NULL, "DEBUGLOG_BUFFER_READY nonexistent", NULL, MB_OK);
        return;            
    }

	/* add time stamp to front of lpOutputString */
    SYSTEMTIME st;
    GetLocalTime(&st);
    char achBuffer[4096];
    sprintf_s(achBuffer, "%02u:%02u:%02u.%03u  %s\n", 
                            st.wHour, 
                            st.wMinute, 
                            st.wSecond, 
                            st.wMilliseconds,
                            lpOutputString);


    /* get a handle to the data synch object */
    HANDLE heventData = OpenEvent(EVENT_MODIFY_STATE, FALSE, "DEBUGLOG_DATA_READY");
    if ( !heventData )
    {
        // MessageBox(NULL, "DEBUGLOG_DATA_READY nonexistent", NULL, MB_OK);
        CloseHandle(heventDebugLog);
        return;            
    }
    
    HANDLE hSharedFile = CreateFileMapping((HANDLE)-1, NULL, PAGE_READWRITE, 0, 4096, "DEBUGLOG_BUFFER");
    if (!hSharedFile) 
    {
        //MessageBox(NULL, "OutputDebugLogString: Unable to create file mapping object DEBUGLOG_BUFFER", "Error", MB_OK);
        CloseHandle(heventDebugLog);
        CloseHandle(heventData);
        return;
    }

    LPSTR lpszSharedMem = (LPSTR)MapViewOfFile(hSharedFile, FILE_MAP_WRITE, 0, 0, 0);
    if (!lpszSharedMem) 
    {
        //MessageBox(NULL, "OutputDebugLogString: Unable to map shared memory", "Error", MB_OK);
        CloseHandle(hSharedFile);
        CloseHandle(heventDebugLog);
        CloseHandle(heventData);
        return;
    }

    /* wait for buffer event */
    WaitForSingleObject(heventDebugLog, INFINITE);

    /* write it to the shared memory */
    *((LPDWORD)lpszSharedMem) = _getpid();
    wsprintf(lpszSharedMem + sizeof(DWORD), "%s", achBuffer);

    /* signal data ready event */
    SetEvent(heventData);

    /* clean up handles */
    UnmapViewOfFile(lpszSharedMem);
    CloseHandle(hSharedFile);
    CloseHandle(heventData);
    CloseHandle(heventDebugLog);

    return;
}

void OutputDebugLogRect(LPCTSTR lpOutputString, CRect &rect)
{
	char buf[1024];
	wsprintf(buf," %s  SX=%d SY=%d EX=%d EY=%d Width=%d Height=%d",
					lpOutputString,
					rect.TopLeft().x,
					rect.TopLeft().y,
					rect.BottomRight().x,
					rect.BottomRight().y,
					rect.Width(),
					rect.Height());

	OutputDebugLogString(buf);
}

void OutputDebugLogPoint(LPCTSTR lpOutputString, CPoint &point)
{
	char buf[1024];
	wsprintf(buf," %s  X=%d Y=%d",
					lpOutputString,
					point.x,
					point.y);

	OutputDebugLogString(buf);
}

void OutputDebugLog(LPSTR lpFormat, ...) 
{
    char buf[1024];

    // format the string
    vsprintf_s(buf, lpFormat, (LPSTR)(&lpFormat+1));

	OutputDebugLogString(buf);
}

void OutputDebugLogStringTo(LPCTSTR lpOutputString,BOOL bTimeStamp=TRUE, int nIdx=0)
{
	LogMessage(nIdx, lpOutputString);
	return;

    /* before we do anything else, make sure DebugLog process is open and waiting */
    HANDLE heventDebugLog = OpenEvent(EVENT_ALL_ACCESS, FALSE, "DEBUGLOG_BUFFER_READY");
    if ( !heventDebugLog )
    {
        //MessageBox(NULL, "DEBUGLOG_BUFFER_READY nonexistent", NULL, MB_OK);
        return;            
    }
	
	char achBuffer[4096];
	if (bTimeStamp) {
		/* add time stamp to front of lpOutputString */
		SYSTEMTIME st;
		GetLocalTime(&st);
		sprintf_s(achBuffer, "%1d%02u:%02u:%02u.%03u  %s", 
							nIdx,
							st.wHour, 
							st.wMinute, 
							st.wSecond, 
							st.wMilliseconds,
							lpOutputString);
	}
	else {
		sprintf_s(achBuffer, "%1d%s",nIdx,lpOutputString);
	}

    /* get a handle to the data synch object */
    HANDLE heventData = OpenEvent(EVENT_MODIFY_STATE, FALSE, "NEW_DEBUGLOG_DATA_READY");
    if ( !heventData )
    {
        // MessageBox(NULL, "DEBUGLOG_DATA_READY nonexistent", NULL, MB_OK);
        CloseHandle(heventDebugLog);
        return;            
    }
    
    HANDLE hSharedFile = CreateFileMapping((HANDLE)-1, NULL, PAGE_READWRITE, 0, 4096, "DEBUGLOG_BUFFER");
    if (!hSharedFile) 
    {
        //MessageBox(NULL, "OutputDebugLogString: Unable to create file mapping object DEBUGLOG_BUFFER", "Error", MB_OK);
        CloseHandle(heventDebugLog);
        CloseHandle(heventData);
        return;
    }

    LPSTR lpszSharedMem = (LPSTR)MapViewOfFile(hSharedFile, FILE_MAP_WRITE, 0, 0, 0);
    if (!lpszSharedMem) 
    {
        //MessageBox(NULL, "OutputDebugLogString: Unable to map shared memory", "Error", MB_OK);
        CloseHandle(hSharedFile);
        CloseHandle(heventDebugLog);
        CloseHandle(heventData);
        return;
    }

    /* wait for buffer event */
    WaitForSingleObject(heventDebugLog, INFINITE);

    /* write it to the shared memory */
    *((LPDWORD)lpszSharedMem) = _getpid();
    wsprintf(lpszSharedMem + sizeof(DWORD), "%s", achBuffer);

    /* signal data ready event */
    SetEvent(heventData);

    /* clean up handles */
    UnmapViewOfFile(lpszSharedMem);
    CloseHandle(hSharedFile);
    CloseHandle(heventData);
    CloseHandle(heventDebugLog);

    return;
}


void OutputDebugLogRectTo(int nIdx,BOOL bTimeStamp,LPCTSTR lpOutputString, CRect &rect)
{
	char buf[1024];
	wsprintf(buf," %s  SX=%d SY=%d EX=%d EY=%d Width=%d Height=%d",
					lpOutputString,
					rect.TopLeft().x,
					rect.TopLeft().y,
					rect.BottomRight().x,
					rect.BottomRight().y,
					rect.Width(),
					rect.Height());

	OutputDebugLogStringTo(buf,bTimeStamp,nIdx);
}

void OutputDebugLogPointTo(int nIdx,BOOL bTimeStamp,LPCTSTR lpOutputString, CPoint &point)
{
	char buf[1024];
	wsprintf(buf," %s  X=%d Y=%d",
					lpOutputString,
					point.x,
					point.y);

	OutputDebugLogStringTo(buf,bTimeStamp,nIdx);
}

void OutputDebugLogTo(int nIdx,BOOL bTimeStamp,LPSTR lpFormat, ...) 
{
    char buf[1024];
	
    // format the string
    vsprintf_s(buf, lpFormat, (LPSTR)(&lpFormat+1));
	OutputDebugLogStringTo(buf,bTimeStamp,nIdx);
	//OutputDebugLogString(buf);
}

void OutputDebugLogClear(int nIdx)
{
    /* before we do anything else, make sure DebugLog process is open and waiting */
    HANDLE heventDebugLog = OpenEvent(EVENT_ALL_ACCESS, FALSE, "DEBUGLOG_BUFFER_READY");
    if ( !heventDebugLog )
    {
        return;            
    }
	
	char achBuffer[4096];
	sprintf_s(achBuffer, "%1d",nIdx);

    /* get a handle to the data synch object */
    HANDLE heventData = OpenEvent(EVENT_MODIFY_STATE, FALSE, "CLEAR_LOG_EVENT");
    if ( !heventData )
    {
        CloseHandle(heventDebugLog);
        return;            
    }
    
    HANDLE hSharedFile = CreateFileMapping((HANDLE)-1, NULL, PAGE_READWRITE, 0, 4096, "DEBUGLOG_BUFFER");
    if (!hSharedFile) 
    {
        CloseHandle(heventDebugLog);
        CloseHandle(heventData);
        return;
    }

    LPSTR lpszSharedMem = (LPSTR)MapViewOfFile(hSharedFile, FILE_MAP_WRITE, 0, 0, 0);
    if (!lpszSharedMem) 
    {
        CloseHandle(hSharedFile);
        CloseHandle(heventDebugLog);
        CloseHandle(heventData);
        return;
    }

    /* wait for buffer event */
    WaitForSingleObject(heventDebugLog, INFINITE);

    /* write it to the shared memory */
    *((LPDWORD)lpszSharedMem) = _getpid();
    wsprintf(lpszSharedMem + sizeof(DWORD), "%s", achBuffer);

    /* signal data ready event */
    SetEvent(heventData);

    /* clean up handles */
    UnmapViewOfFile(lpszSharedMem);
    CloseHandle(hSharedFile);
    CloseHandle(heventData);
    CloseHandle(heventDebugLog);
    return;
}

void OutputDebugLogClearAll()
{
    /* get a handle to the data synch object */
    HANDLE heventData = OpenEvent(EVENT_MODIFY_STATE, FALSE, "CLEAR_ALL_LOG_EVENT");
    if ( !heventData )
    {
        return;            
    }
    /* signal data ready event */
    SetEvent(heventData);
    CloseHandle(heventData);
}

void OutputDebugLogToFile(BOOL bState)
{
    /* before we do anything else, make sure DebugLog process is open and waiting */
    HANDLE heventDebugLog = OpenEvent(EVENT_ALL_ACCESS, FALSE, "DEBUGLOG_BUFFER_READY");
    if ( !heventDebugLog )
    {
        return;            
    }
	
	char achBuffer[4096];
	sprintf_s(achBuffer, "%1d",(int)bState);

    /* get a handle to the data synch object */
    HANDLE heventData = OpenEvent(EVENT_MODIFY_STATE, FALSE, "DEBUG_LOG_EVENT");
    if ( !heventData )
    {
        CloseHandle(heventDebugLog);
        return;            
    }
    
    HANDLE hSharedFile = CreateFileMapping((HANDLE)-1, NULL, PAGE_READWRITE, 0, 4096, "DEBUGLOG_BUFFER");
    if (!hSharedFile) 
    {
        CloseHandle(heventDebugLog);
        CloseHandle(heventData);
        return;
    }

    LPSTR lpszSharedMem = (LPSTR)MapViewOfFile(hSharedFile, FILE_MAP_WRITE, 0, 0, 0);
    if (!lpszSharedMem) 
    {
        CloseHandle(hSharedFile);
        CloseHandle(heventDebugLog);
        CloseHandle(heventData);
        return;
    }

    /* wait for buffer event */
    WaitForSingleObject(heventDebugLog, INFINITE);

    /* write it to the shared memory */
    *((LPDWORD)lpszSharedMem) = _getpid();
    wsprintf(lpszSharedMem + sizeof(DWORD), "%s", achBuffer);

    /* signal data ready event */
    SetEvent(heventData);

    /* clean up handles */
    UnmapViewOfFile(lpszSharedMem);
    CloseHandle(hSharedFile);
    CloseHandle(heventData);
    CloseHandle(heventDebugLog);
    return;
}


struct CMessageDef
{
	BYTE nTabNo;
	BYTE nInfoType;
	SYSTEMTIME time;
	DWORD nProcId;
	int nTextLength;
	int nWidth;
	int nPitch;
	int nHeight;
	BYTE nBitCount;
	void* pInfo;
};

///Udupa; July'2012
void LogMessage(int nTab, LPCTSTR pMessageText, ...)
{
    char buf[1024];
    
	vsprintf_s(buf, pMessageText, (LPSTR)(&pMessageText+1));
	LogDebugMessage(buf, NULL, 0, 0, 0, 0, nTab, 0);
}

///Udupa; July'2012
void LogDebugMessage(LPCTSTR pMessageText, BYTE* pBitmap, int nWidth, int nHeight, int nPitch, int nBitCount, int nTab, int nInfoType)
{
    HANDLE eventMessageDone = OpenEvent(EVENT_ALL_ACCESS, FALSE, "EvDebugMessageDone");
    if(eventMessageDone==NULL)
        return;            

	SYSTEMTIME st;
	GetLocalTime(&st);

    HANDLE eventMessageReady = OpenEvent(EVENT_MODIFY_STATE, FALSE, "EvDebugMessageReady");
    if(eventMessageReady == NULL) {
        CloseHandle(eventMessageDone);
        return;
    }

	HANDLE hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		"EvDebugViewerMessage");
    if(hMapFile == NULL) {
        CloseHandle(eventMessageDone);
        CloseHandle(eventMessageReady);
        return;
    }

    BYTE* pSharedMemory = (BYTE*) MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0);
    if(pSharedMemory == NULL) {
        CloseHandle(hMapFile);
        CloseHandle(eventMessageDone);
        CloseHandle(eventMessageReady);
        return;
    }

	while(1) {
	    if(WaitForSingleObject(eventMessageDone, 1000)==WAIT_OBJECT_0)
			break;
		CloseHandle(eventMessageDone);
		eventMessageDone = OpenEvent(EVENT_ALL_ACCESS, FALSE, "EvDebugMessageDone");
		if(eventMessageDone==NULL)
			return;            
	}

	CMessageDef* pMessage = (CMessageDef*) pSharedMemory;
	pMessage->nTabNo = nTab;
	pMessage->nInfoType = nInfoType;
	pMessage->time = st;
	pMessage->nProcId = GetProcessId(GetCurrentProcess()); //_getpid();

	pMessage->nTextLength = (int) strlen(pMessageText);
	BYTE* pInfo = (BYTE*) &pMessage->pInfo;
	memcpy(pInfo, pMessageText, pMessage->nTextLength);
	pInfo += pMessage->nTextLength;
	memset(pInfo++, 0, 1);

	if(nPitch < 0)
		nPitch = nWidth;
	pMessage->nWidth = nWidth;
	pMessage->nPitch = nPitch;

	if(nHeight > 0) {
		int nMaxSize = 6000000 - (pMessage->nTextLength + 1 + sizeof(CMessageDef));
		if((nHeight*nPitch) > nMaxSize)
			nHeight = nMaxSize / nPitch;
		pMessage->nHeight = nHeight;
		pMessage->nBitCount = nBitCount;
		memcpy(pInfo, pBitmap, nPitch * nHeight * nBitCount/8);
	}
	else
		pMessage->nHeight = 0;

	SetEvent(eventMessageReady);

	UnmapViewOfFile(pSharedMemory);
	CloseHandle(hMapFile);
	CloseHandle(eventMessageReady);
	CloseHandle(eventMessageDone);

	return;
}

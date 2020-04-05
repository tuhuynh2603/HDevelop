///Udupa; Thread Manager to control all threads initiated in the project


#pragma once
#include "AppDef.h"

class CThreadManager
{
public:
	CThreadManager(void);
	~CThreadManager(void);

	int Initialize();
	HANDLE CThreadManager::CreateThread(AFX_THREADPROC threadProc, LPVOID parm, ThreadTypes threadType, int nTrack = -1);

	//CArray<HANDLE, HANDLE> m_pThreads;
	int m_nNoOfProcessors;

	int m_nInspThreadIndex;
};

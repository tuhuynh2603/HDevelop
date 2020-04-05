///Udupa; Thread Manager to control all threads initiated in the project

#include "stdafx.h"
#include "Application.h"
#include "ThreadManager.h"


CThreadManager::CThreadManager()
{
}

CThreadManager::~CThreadManager()
{
}

int CThreadManager::Initialize()
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	m_nNoOfProcessors = systemInfo.dwNumberOfProcessors;
	OutputDebugLog("Number of Processing cores found: %d", m_nNoOfProcessors);
	if(m_nNoOfProcessors < 2)
		m_nNoOfProcessors = 2;

	m_nInspThreadIndex = 0;

	return 0;
}

HANDLE CThreadManager::CreateThread(AFX_THREADPROC threadProc, LPVOID parm, ThreadTypes threadType, int nTrack)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	UINT nAffinity = 0;
	UINT nPriority = THREAD_PRIORITY_NORMAL;
	if(threadType == TRACK_THREAD) {
		if(nTrack >= 0) {
			int nProcShare = (m_nNoOfProcessors-1) / pApp->m_nTotalTracks;
			nAffinity = (((int) pow(2.0, nProcShare)-1) << (nTrack*nProcShare+1));
		}
#ifndef _DEBUG
		nPriority = THREAD_PRIORITY_HIGHEST;
#endif
	}
	if(threadType == INSPECTION_THREAD) {
		if(nTrack >= 0) {
			nAffinity = (1 << ((++m_nInspThreadIndex) % m_nNoOfProcessors));
			if(nAffinity == 1)
				nAffinity = (1 << ((++m_nInspThreadIndex) % m_nNoOfProcessors));
		}
		nPriority = THREAD_PRIORITY_HIGHEST;
	}
	else if(threadType == SEQUENCE_THREAD) {
		nAffinity = 1;
		nPriority = THREAD_PRIORITY_TIME_CRITICAL;
	}
	else if(threadType == IMAGE_SAVE_THREAD) {
		nAffinity = 1;
		nPriority = THREAD_PRIORITY_LOWEST;
	}

	CWinThread* pThread = AfxBeginThread(threadProc, parm, nPriority);
	HANDLE hThread = pThread->m_hThread;
	SetThreadAffinityMask(hThread, nAffinity);

	if(nAffinity > 1)
		OutputDebugLog("Track%d: Affinity = 0x%X", nTrack, nAffinity);

	//m_pThreads.Add(hThread);


	return hThread;
}

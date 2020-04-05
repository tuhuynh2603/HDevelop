#pragma once

#include <sys/timeb.h>
#include <time.h>

int DisplayWin32Error(DWORD dwErrorCode = 0,
					  LPCTSTR lpstrTextPriorToErrorMessage = NULL,
                      UINT nType = MB_OK, 
                      UINT nIDHelp = 0);

class CPerformanceTimer
{
protected:
    LARGE_INTEGER m_liCounterFreq; 
    LARGE_INTEGER m_liStartCounts; 
    LARGE_INTEGER m_liStopCounts; 
    LONGLONG m_llElapsedCounts;    

public:
    CPerformanceTimer();
    void Reset();
    double GetElapsedMSec();
    double GetElapsedHours();
};

inline CPerformanceTimer::CPerformanceTimer()
{
    if (!::QueryPerformanceFrequency(&m_liCounterFreq))
    {
        DisplayWin32Error();
		ASSERT(FALSE);
    }
	Reset();
}

inline void CPerformanceTimer::Reset()
{
    VERIFY(::QueryPerformanceCounter(&m_liStartCounts));
	m_liStopCounts = m_liStartCounts;
}

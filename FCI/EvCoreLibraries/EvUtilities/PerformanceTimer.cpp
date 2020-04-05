#include "stdafx.h"
#include "PerformanceTimer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int DisplayWin32Error(DWORD dwErrorCode,
					  LPCTSTR lpstrTextPriorToErrorMessage,
					  UINT nType, 
					  UINT nIDHelp)
{
    LPVOID lpMsgBuf;
	DWORD dwLastError = ((dwErrorCode == 0) ? GetLastError() : dwErrorCode);
    int nReturn;

    ::FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dwLastError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL 
        );

    // Display the string.
    if (lpstrTextPriorToErrorMessage)
    {
        CString str(lpstrTextPriorToErrorMessage);
        str += _T("\n");
        str += (LPTSTR)lpMsgBuf;
        nReturn = AfxMessageBox(str, nType, nIDHelp);
    }
    else
    {
        nReturn = AfxMessageBox((LPTSTR)lpMsgBuf, nType, nIDHelp);
    }

    // Free the buffer.
    ::LocalFree( lpMsgBuf );

    return nReturn;
};


double CPerformanceTimer::GetElapsedMSec()
{
    VERIFY(QueryPerformanceCounter(&m_liStopCounts));
    m_llElapsedCounts = m_liStopCounts.QuadPart - m_liStartCounts.QuadPart;
    double dElapsedTime = ((double)m_llElapsedCounts / 
						   (double)m_liCounterFreq.QuadPart) * 1000;
    return dElapsedTime;
}

double CPerformanceTimer::GetElapsedHours()
{
    VERIFY(QueryPerformanceCounter(&m_liStopCounts));
    m_llElapsedCounts = m_liStopCounts.QuadPart - m_liStartCounts.QuadPart;
    double dElapsedTime = (double)m_llElapsedCounts / 
						  ((double)m_liCounterFreq.QuadPart * 3600);
    return dElapsedTime;
}

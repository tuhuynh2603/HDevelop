#pragma once

void OutputDebugLogString(LPCTSTR lpOutputString);

void OutputDebugLogRect(LPCTSTR lpOutputString, CRect &rect);

void OutputDebugLogPoint(LPCTSTR lpOutputString, CPoint &point);

void OutputDebugLog(LPSTR lpFormat, ...);

void OutputDebugLogStringTo(LPCTSTR lpOutputString,BOOL bTimeStamp,int nIdx);
void OutputDebugLogRectTo(int nIdx,BOOL bTimeStamp,LPCTSTR lpOutputString, CRect &rect);
void OutputDebugLogPointTo(int nIdx,BOOL bTimeStamp,LPCTSTR lpOutputString, CPoint &point);
void OutputDebugLogTo(int nIdx,BOOL bTimeStamp,LPSTR lpFormat, ...);

void OutputDebugLogClear(int nIdx);
void OutputDebugLogClearAll();
void OutputDebugLogToFile(BOOL bState);
void SaveDebugLogList(int nIdx,CString strFileName);
void SaveAllDebugLogList(CString strFileNameName);

void LogMessage(int nTab, LPCTSTR pMessageText, ...);
void LogDebugMessage(LPCTSTR pMessageText, BYTE* pBitmap, int nWidth, int nHeight, int nPitch, int nBitCount=8, int nTab=0, int nInfoType=1);

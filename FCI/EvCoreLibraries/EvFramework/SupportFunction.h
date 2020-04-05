#pragma once

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif


#include "ImageStruct.h"

DECLSPEC_WINDOW_IF void SaveDebugImage(CString strFile, CImageBuffer& buf);
DECLSPEC_WINDOW_IF int DrwCur(IMAGEVIEW_INFO *pImageViewInfo, CPoint *pt, LOGPEN *pPenInfo, int nExt, char charCursor = 'x');
DECLSPEC_WINDOW_IF int DrwRect(IMAGEVIEW_INFO *pImageViewInfo, CRect *rect, LOGPEN *pPenInfo);
DECLSPEC_WINDOW_IF int DrwCircle(IMAGEVIEW_INFO *pImageViewInfo, CRect *rect, LOGPEN *pPenInfo);
DECLSPEC_WINDOW_IF int DrwLine(IMAGEVIEW_INFO *pImageViewInfo, CPoint *pt1, CPoint *pt2, LOGPEN *pPenInfo);
DECLSPEC_WINDOW_IF int DrwUpdate(IMAGEVIEW_INFO *pImageViewInfo);
DECLSPEC_WINDOW_IF int DrwDel(IMAGEVIEW_INFO *pImageViewInfo);
DECLSPEC_WINDOW_IF int DrwString(IMAGEVIEW_INFO *pImageViewInfo,CString *string,CPoint *pt, 
							   LOGPEN *pBkColor,LOGPEN *pTextColor, int nSize);
DECLSPEC_WINDOW_IF int DrwStr(IMAGEVIEW_INFO *pImageViewInfo,CString *string,CPoint *pt, 
							   LOGPEN *pTextColor, int FontHeight);
DECLSPEC_WINDOW_IF int DelStr(IMAGEVIEW_INFO *pImageViewInfo);

DECLSPEC_WINDOW_IF int DelCur(IMAGEVIEW_INFO *pImageViewInfo);
DECLSPEC_WINDOW_IF int DelLine(IMAGEVIEW_INFO *pImageViewInfo, int nNoOfLines=-1);
DECLSPEC_WINDOW_IF int DelRect(IMAGEVIEW_INFO *pImageViewInfo);
DECLSPEC_WINDOW_IF int DelCircle(IMAGEVIEW_INFO *pImageViewInfo);

DECLSPEC_WINDOW_IF int DrwTrackCircle(IMAGEVIEW_INFO *pImageViewInfo, CRect *rect);
DECLSPEC_WINDOW_IF int DrwTrackCross(IMAGEVIEW_INFO *pImageViewInfo, CRect *rect);
DECLSPEC_WINDOW_IF int GetTrackRectGroupIdx(IMAGEVIEW_INFO *pImageViewInfo,int Idx);

DECLSPEC_WINDOW_IF int DrwTrackRect(IMAGEVIEW_INFO *pImageViewInfo, CRect *rect, int nAreaType = 0);
DECLSPEC_WINDOW_IF int GetTrackRectCount(IMAGEVIEW_INFO *pImageViewInfo);
DECLSPEC_WINDOW_IF CRect GetTrackRect(IMAGEVIEW_INFO *pImageViewInfo, int nIdx);
DECLSPEC_WINDOW_IF void HideCrntTrackRect(IMAGEVIEW_INFO *pImageViewInfo,BOOL bUpdate);
DECLSPEC_WINDOW_IF int GetTrackRectCrntIdx(IMAGEVIEW_INFO *pImageViewInfo);
DECLSPEC_WINDOW_IF void SetTrackRectProperties(IMAGEVIEW_INFO *pImageViewInfo, BOOL bLockAddDel, BOOL bLockMoveResize,BOOL bUpdate);
DECLSPEC_WINDOW_IF void LockTrackRectSelect(IMAGEVIEW_INFO *pImageViewInfo, BOOL bState);

DECLSPEC_WINDOW_IF int DrwImageHost(IMAGEVIEW_INFO *pImageViewInfo,CImageBuffer *Img);
DECLSPEC_WINDOW_IF int RestorePrevImg(IMAGEVIEW_INFO *pImageViewInfo);

DECLSPEC_WINDOW_IF int WaitResponse(IMAGEVIEW_INFO *pImageViewInfo);
DECLSPEC_WINDOW_IF void ContTask(IMAGEVIEW_INFO *PVisSysInfo);
DECLSPEC_WINDOW_IF void AbortTask(IMAGEVIEW_INFO *pImageViewInfo);

DECLSPEC_WINDOW_IF void SwitchImg(IMAGEVIEW_INFO *pImageViewInfo, BOOL bBinaryMode, LPARAM lParam);
DECLSPEC_WINDOW_IF void EditImage(IMAGEVIEW_INFO *pImageViewInfo, BOOL bEditMode);


DECLSPEC_WINDOW_IF int GetImgThreshold(IMAGEVIEW_INFO *pImageViewInfo);

DECLSPEC_WINDOW_IF void UpdateStatusBar(CString* pstrMsg, int nPane=3);

DECLSPEC_WINDOW_IF LPVOID CreateImgRotDlg(IMAGEVIEW_INFO *pView);
DECLSPEC_WINDOW_IF int DeleteImgRotDlg(IMAGEVIEW_INFO *pView, LPVOID pDlg);

DECLSPEC_WINDOW_IF int SetDocStatusText(IMAGEVIEW_INFO *pView, CString* pstrStatus);
DECLSPEC_WINDOW_IF int SetInspectionTime(IMAGEVIEW_INFO *pView, double dTime);

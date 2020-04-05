#pragma once

#include "ImageStruct.h"
#include "ImageBuffer.h"

extern void SaveDebugImage(CString strFile, CImageBuffer& buf);
extern int DrwDebugImage(IMAGEVIEW_INFO* pImageViewInfo,
						CImageBuffer* pImgBufDispFOV,
						CImageBuffer* pImgBufProc,
						CRect* prCImageBufferProcRelative,
						CPoint* pptImgBufProcTopLftInFOV,
						CString* pstrDebug,
						BOOL bColorDisplay);

extern int DrwDebugImage(IMAGEVIEW_INFO* pView,
				  CImageBuffer* pImgBufDispFOV,
				  CImageBuffer* pImgBufProc,
				  CRect* prCImageBufferProcRelative,
				  CImageBuffer* pImgBufMask,
				  CRect* prCImageBufferMaskRelative,
				  CPoint* pptImgBufProcTopLftInFOV,
				  CString* pstrDebug);

extern int DrwImageHost(IMAGEVIEW_INFO *pImageViewInfo,CImageBuffer *Img);
extern int RestorePrevImg(IMAGEVIEW_INFO *pImageViewInfo);

// Graphics Drawing Controls
extern int DrwCur(IMAGEVIEW_INFO *pImageViewInfo, CPoint *pt, LOGPEN *pPenInfo, int nExt, char charCursor = 'x');
extern int DrwCur2D(IMAGEVIEW_INFO *pImageViewInfo, CPoint2D *pt2D, LOGPEN *pPenInfo, int nExt);
extern int DrwRect(IMAGEVIEW_INFO *pImageViewInfo, CRect *rect, LOGPEN *pPenInfo);
extern int DrwRectangle2D(IMAGEVIEW_INFO *pImageViewInfo, CRectangle2D *prect2D, LOGPEN *pPenInfo);
extern int DrwEllipse2D(IMAGEVIEW_INFO *pImageViewInfo, CRectangle2D *prect2D, LOGPEN *pPenInfo);
extern int DrwEllipse(IMAGEVIEW_INFO *pImageViewInfo, CPoint2D *ppt2DCtr, double dRadiusX, double dRadiusY, LOGPEN *pPenInfo);
extern int DrwString(IMAGEVIEW_INFO *pImageViewInfo,CString *string,CPoint *pt,LOGPEN *pBkColor, LOGPEN *pTextColor,int nSize);
extern int DrwStr(IMAGEVIEW_INFO *pImageViewInfo,CString *string,CPoint *pt, LOGPEN *pTextColor,int nFontHeight);
extern int DelStr(IMAGEVIEW_INFO *pImageViewInfo);

extern int DelCur(IMAGEVIEW_INFO *pImageViewInfo);
extern int DelLine(IMAGEVIEW_INFO *pImageViewInfo, int nNoOfLines=-1);
extern int DelRect(IMAGEVIEW_INFO *pImageViewInfo);
extern int DelCircle(IMAGEVIEW_INFO *pImageViewInfo);

extern int DrwContour(IMAGEVIEW_INFO *pView, CPoint2D *pt2DContour, int nContour, LOGPEN *pPenInfo);

extern int DrwLine(IMAGEVIEW_INFO *pImageViewInfo, CPoint *pt1, CPoint *pt2, LOGPEN *pPenInfo);
extern int DrwLine2D(IMAGEVIEW_INFO *pImageViewInfo, CPoint2D *pt2D1, CPoint2D *pt2D2, LOGPEN *pPenInfo);
extern int DrwUpdate(IMAGEVIEW_INFO *pImageViewInfo);
extern int DrwDel(IMAGEVIEW_INFO *pImageViewInfo);

// Track Rectangle Controls
extern  int DrwTrackRect(IMAGEVIEW_INFO *pImageViewInfo, CRect *rect);
extern  int DrwTrackCircle(IMAGEVIEW_INFO *pImageViewInfo, CRect *rect);
extern  int DrwTrackCross(IMAGEVIEW_INFO *pImageViewInfo, CRect *rect);
extern int GetTrackRectCount(IMAGEVIEW_INFO *pImageViewInfo);
extern CRect GetTrackRect(IMAGEVIEW_INFO *pImageViewInfo, int nIdx);
extern int GetTrackRectCrntIdx(IMAGEVIEW_INFO *pImageViewInfo);
extern void HideCrntTrackRect(IMAGEVIEW_INFO *pImageViewInfo,BOOL bUpdate);
extern void SetTrackRectProperties(IMAGEVIEW_INFO *pImageViewInfo, BOOL bLockAddDel, BOOL bLockMoveResize,BOOL bUpdate);
extern void LockTrackRectSelect(IMAGEVIEW_INFO *pImageViewInfo, BOOL bState);

// User Interface Controls
extern int WaitResponse(IMAGEVIEW_INFO *pImageViewInfo);
extern void ContTask(IMAGEVIEW_INFO *PVisSysInfo);
extern void AbortTask(IMAGEVIEW_INFO *pImageViewInfo);

// Information Status Controls
extern void UpdateStatusBar(CString* pstrMsg, int nPane=3);
extern void SetAOIZoomRect(IMAGEVIEW_INFO *pImageViewInfo,CRect* prectZoom);

// Image Mode
extern void SwitchImg(IMAGEVIEW_INFO *pImageViewInfo, BOOL bBinaryMode, LPARAM lParam);
extern void EditImage(IMAGEVIEW_INFO *pImageViewInfo, BOOL bEditMode);
extern int GetImgThreshold(IMAGEVIEW_INFO *pImageViewInfo);

// Dialog Box Controls
extern int CustSelMarkSetDlg(BOOL *pbTaught, int nTotalMarkSet);

extern LPVOID CreateImgRotDlg(IMAGEVIEW_INFO *pView);
extern int DeleteImgRotDlg(IMAGEVIEW_INFO *pView, LPVOID pDlg);

extern int SetDocStatusText(IMAGEVIEW_INFO *pView, CString* pstrStatus);
extern int SetInspectionTime(IMAGEVIEW_INFO *pView, double dTime);

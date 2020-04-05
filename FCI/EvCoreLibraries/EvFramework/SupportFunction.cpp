#include "stdafx.h"
#include "MsgCode.h"
#include "EvImageView.h"
#include "EvMainFrame.h"
#include "SupportFunction.h"
#include "ImageRotationDlg.h"

void SaveDebugImage(CString strFile, CImageBuffer& buf)
{
#ifdef _DEBUG
//	SaveGrayScaleBMPFile(strFile, buf);
#endif
}

int DrwTrackRect(IMAGEVIEW_INFO *pImageViewInfo, CRect *rect, int nAreaType)
{
	CRect rectTrack;

	rectTrack = *rect;

	if (CheckRect(&rectTrack,&(pImageViewInfo->sizeImage)))
		return -1;
	
	(pImageViewInfo->pImageView)->SendMessage(WM_DRW_TRACK_RECT,(WPARAM)&rectTrack, nAreaType);
	return 0;
}

int DrwTrackCircle(IMAGEVIEW_INFO *pImageViewInfo, CRect *rect)
{
	CRect rectTrack;

	rectTrack = *rect;
	if(CheckRect(&rectTrack,&(pImageViewInfo->sizeImage)))
		return -1;
	
	(pImageViewInfo->pImageView)->SendMessage(WM_DRW_TRACK_RECT,(WPARAM)&rectTrack, TRUE);
	return 0;
}

int DrwTrackCross(IMAGEVIEW_INFO *pImageViewInfo, CRect *rect)
{
	CRect rectTrack = *rect;
	if(CheckRect(&rectTrack, &pImageViewInfo->sizeImage))
		return -1;
	
	pImageViewInfo->pImageView->SendMessage(WM_DRW_TRACK_CROSS, (WPARAM) &rectTrack);
	return 0;
}

int GetTrackRectCount(IMAGEVIEW_INFO *pImageViewInfo)
{
	return (int) (pImageViewInfo->pImageView->m_RectTrackAry.GetSize());
}

CRect GetTrackRect(IMAGEVIEW_INFO *pImageViewInfo, int nIdx)
{
	if (nIdx<pImageViewInfo->pImageView->m_RectTrackAry.GetSize())
		return pImageViewInfo->pImageView->m_RectTrackAry[nIdx].rect;//.ElementAt(nIdx);
	else {
		OutputDebugLog("Index Out of Track Rect Range");
		return CRect(0,0,0,0);
	}
}

int GetTrackRectGroupIdx(IMAGEVIEW_INFO *pImageViewInfo,int nIdx)
{
	int nGroupId = -1;

	if (nIdx<pImageViewInfo->pImageView->m_RectTrackAry.GetSize()){
		nGroupId = pImageViewInfo->pImageView->m_RectTrackAry[nIdx].nTrackerType;
	}
	return nGroupId;
}


void HideCrntTrackRect(IMAGEVIEW_INFO *pImageViewInfo,BOOL bUpdate)
{
	CEvImageDoc *pDoc = pImageViewInfo->pImageView->GetDocument();
	pDoc->HideTrackRect(bUpdate);
}

int GetTrackRectCrntIdx(IMAGEVIEW_INFO *pImageViewInfo)
{
	CEvImageDoc *pDoc = pImageViewInfo->pImageView->GetDocument();
	return pDoc->m_nTrackerIdx;
}

void SetTrackRectProperties(IMAGEVIEW_INFO *pImageViewInfo,
							BOOL bLockAddDel,
							BOOL bLockMoveResize,
							BOOL bUpdate)
{
	CEvImageDoc *pDoc = pImageViewInfo->pImageView->GetDocument();
	pDoc->m_bLockAddDelRect = bLockAddDel;
	pDoc->m_bLockMoveResizeRect = bLockMoveResize;

	if(!bLockAddDel)
		pImageViewInfo->pImageView->m_Tracker.m_nHandleSize = 5;
}

void LockTrackRectSelect(IMAGEVIEW_INFO *pImageViewInfo, BOOL bState)
{
	CEvImageDoc *pDoc = pImageViewInfo->pImageView->GetDocument();
	pDoc->m_bLockSelectRect = bState;
}

int DrwCur(IMAGEVIEW_INFO *pImageViewInfo,
		   CPoint *pt,
		   LOGPEN *pPenInfo,
		   int nExt,
		   char charCursor)
{
	CUR_INFO CurInfo;

	CurInfo.PenInfo.lopnColor = pPenInfo->lopnColor;
	CurInfo.PenInfo.lopnStyle = pPenInfo->lopnStyle;
	CurInfo.PenInfo.lopnWidth = pPenInfo->lopnWidth;
	CurInfo.ptCursor = *pt;
	CurInfo.nExt = nExt;
	CurInfo.charCursor = charCursor;

	CheckPoint(&(CurInfo.ptCursor),&(pImageViewInfo->sizeImage));

	(pImageViewInfo->pImageView)->SendMessage(WM_DRW_CUR,(WPARAM)&CurInfo,0);
	return 0;
}

int DelCur(IMAGEVIEW_INFO *pImageViewInfo)
{
	(pImageViewInfo->pImageView)->SendMessage(WM_CUR_DEL,0,0);
	return 0;
}

int DrwRect(IMAGEVIEW_INFO *pImageViewInfo,
			CRect *rect,
			LOGPEN *pPenInfo)
{
	RECT_INFO RectInfo;

	RectInfo.PenInfo.lopnColor = pPenInfo->lopnColor;
	RectInfo.PenInfo.lopnStyle = pPenInfo->lopnStyle;
	RectInfo.PenInfo.lopnWidth = pPenInfo->lopnWidth;
	RectInfo.rect = *rect;

	if(CheckRect(&(RectInfo.rect),&(pImageViewInfo->sizeImage)))
		return -1;

	(pImageViewInfo->pImageView)->SendMessage(WM_DRW_RECT,(WPARAM)&RectInfo,0);
	return 0;
}

int DelRect(IMAGEVIEW_INFO *pImageViewInfo)
{
	(pImageViewInfo->pImageView)->SendMessage(WM_RECT_DEL,0,0);
	return 0;
}


int DrwCircle(IMAGEVIEW_INFO *pImageViewInfo,
			CRect *rect,
			LOGPEN *pPenInfo)
{
	CIRCLE_INFO RectInfo;

	RectInfo.PenInfo.lopnColor = pPenInfo->lopnColor;
	RectInfo.PenInfo.lopnStyle = pPenInfo->lopnStyle;
	RectInfo.PenInfo.lopnWidth = pPenInfo->lopnWidth;
	RectInfo.rect = *rect;

	if(CheckRect(&(RectInfo.rect),&(pImageViewInfo->sizeImage)))
		return -1;

	(pImageViewInfo->pImageView)->SendMessage(WM_DRW_CIRCLE,(WPARAM)&RectInfo,0);
	return 0;
}

int DelCircle(IMAGEVIEW_INFO *pImageViewInfo)
{
	(pImageViewInfo->pImageView)->SendMessage(WM_CIRCLE_DEL,0,0);
	return 0;
}

int DrwLine(IMAGEVIEW_INFO *pImageViewInfo,
			CPoint *pt1,
			CPoint *pt2,
			LOGPEN *pPenInfo)
{
	LINE_INFO LineInfo;

	LineInfo.PenInfo.lopnColor = pPenInfo->lopnColor;
	LineInfo.PenInfo.lopnStyle = pPenInfo->lopnStyle;
	LineInfo.PenInfo.lopnWidth = pPenInfo->lopnWidth;
	LineInfo.pt1 = *pt1;
	LineInfo.pt2 = *pt2;

	CheckPoint(&(LineInfo.pt1),&(pImageViewInfo->sizeImage));
	CheckPoint(&(LineInfo.pt2),&(pImageViewInfo->sizeImage));

	(pImageViewInfo->pImageView)->ViewDrwLine((WPARAM)&LineInfo);
	return 0;
}

int DelLine(IMAGEVIEW_INFO *pImageViewInfo, int nNoOfLines)
{
	(pImageViewInfo->pImageView)->SendMessage(WM_LINE_DEL,(WPARAM)&nNoOfLines,0);
	return 0;
}


int DrwUpdate(IMAGEVIEW_INFO *pImageViewInfo)
{
	(pImageViewInfo->pImageView)->SendMessage(WM_DRW_UPD,0,0);
	return 0;
}

int DrwDel(IMAGEVIEW_INFO *pImageViewInfo)
{
	
	(pImageViewInfo->pImageView)->SendMessage(WM_DRW_DEL,0,0);
	return 0;
}

int DrwString(IMAGEVIEW_INFO *pImageViewInfo,
			    CString *string,
			    CPoint *pt,
			    LOGPEN *pBkColor,
			    LOGPEN *pTextColor,
			    int nSize)
{

	STR_INFO StrInfo;

	StrInfo.C_Interval = 0;
	StrInfo.L_Interval = 0;
	StrInfo.m_String_Size = nSize;
	
	StrInfo.str.Format("%s",*string);
	
	StrInfo.x = pt->x;
	StrInfo.y = pt->y;

	StrInfo.BkColor.lopnColor = pBkColor->lopnColor;
	StrInfo.TextColor.lopnColor = pTextColor->lopnColor;
	(pImageViewInfo->pImageView)->SendMessage(WM_DRW_STR,(WPARAM)&StrInfo,0);

	return 0;
}
int DrwStr(IMAGEVIEW_INFO *pImageViewInfo,
			    CString *string,
			    CPoint *pt,
			    LOGPEN *pTextColor,
			    int FontHeight)
{

	STR_ATTR StrAttr;

	StrAttr.Color = pTextColor->lopnColor;
	StrAttr.FontHeight = FontHeight;
	
	StrAttr.strMsg.Format("%s",*string);
	
	StrAttr.pt.x = pt->x;
	StrAttr.pt.y = pt->y;

	(pImageViewInfo->pImageView)->SendMessage(WM_DRW_STRING,(WPARAM)&StrAttr,0);

	return 0;
}

int DelStr(IMAGEVIEW_INFO *pImageViewInfo)
{
	(pImageViewInfo->pImageView)->SendMessage(WM_STR_DEL,0,0);
	return 0;
}

int DrwImageHost(IMAGEVIEW_INFO *pImageViewInfo, CImageBuffer *Img)
{
	(pImageViewInfo->pImageView)->SendMessage(WM_UPDATE_IMG,(WPARAM)Img,0);
	return 0;
}

int RestorePrevImg(IMAGEVIEW_INFO *pImageViewInfo)
{
	(pImageViewInfo->pImageView)->SendMessage(WM_RESTORE_IMG,0,0);
	return 0;

}

int WaitResponse(IMAGEVIEW_INFO *pImageViewInfo)
{
	CEvImageDoc *pDoc = pImageViewInfo->pImageView->GetDocument();
	return pDoc->Wait();
}

void ContTask(IMAGEVIEW_INFO *pImageViewInfo)
{
	CEvImageDoc *pDoc = pImageViewInfo->pImageView->GetDocument();
	pDoc->Continue();
}

void AbortTask(IMAGEVIEW_INFO *pImageViewInfo)
{
	CEvImageDoc *pDoc = pImageViewInfo->pImageView->GetDocument();
	pDoc->Abort();
}

void SwitchImg(IMAGEVIEW_INFO *pImageViewInfo, BOOL bBinaryMode, LPARAM lParam)
{
		(pImageViewInfo->pImageView)->SendMessage(WM_BINARY_MODE,(WPARAM)bBinaryMode,(LPARAM)lParam);
}

int GetImgThreshold(IMAGEVIEW_INFO *pImageViewInfo)
{
	CEvImageDoc *pDoc = pImageViewInfo->pImageView->GetDocument();
	return pDoc->m_nThreshold.nLow;
}

void UpdateStatusBar(CString* pstrMsg, int nPane)
{
	CWinApp *pApp = AfxGetApp();
	
	((CEvMainFrame*)(pApp->m_pMainWnd))->SendMessage(WM_UPD_STATUSBAR,(WPARAM)nPane,(LPARAM)pstrMsg);
}

LPVOID CreateImgRotDlg(IMAGEVIEW_INFO *pView)
{
	CImageRotationDlg Dlg;

	Dlg.m_pView = (CView*)pView->pImageView;
	Dlg.DoModal();

	return 0;
}

int DeleteImgRotDlg(IMAGEVIEW_INFO *pView,LPVOID pDlg)
{
	if (pDlg) {
		((CImageRotationDlg*)pDlg)->DestroyWindow();
	}
	return 0;
}

int SetDocStatusText(IMAGEVIEW_INFO *pView, CString* pstrStatus)
{
	try {
		pView->pImageView->GetParent()->GetParent()->GetParent()->SendMessage(WM_SET_INSPSTATUS, (WPARAM) pstrStatus, 0);
	}
	catch(...) {
		OutputDebugLog("Failed to set inspection status");
	}

	return 0;
}

int SetInspectionTime(IMAGEVIEW_INFO *pView, double dTime)
{
	try {
		CString strInsptime;
		strInsptime.Format("Insp Time: %dms", (int) dTime);
		pView->pImageView->GetParent()->GetParent()->GetParent()->SendMessage(WM_SET_INSPSTATUS, (WPARAM) &strInsptime, 0);
	}
	catch(...) {
		OutputDebugLog("Failed to set inspection status");
	}

	return 0;
}


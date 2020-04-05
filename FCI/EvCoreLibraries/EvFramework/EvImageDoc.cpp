// VisionDoc.cpp : implementation of the CEvImageDoc class
//

#include "stdafx.h"
#include "EvImageDoc.h"
#include "EvImageView.h"
#include "ImageBuffer.h"
#include <windowsx.h>
#include "BitmapFunctions.h"
#include "JpegFunctions.h"
#include "SupportFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEvImageDoc

IMPLEMENT_DYNCREATE(CEvImageDoc, CDocument)

BEGIN_MESSAGE_MAP(CEvImageDoc, CDocument)
	//{{AFX_MSG_MAP(CEvImageDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEvImageDoc construction/destruction

CEvImageDoc::CEvImageDoc()
{
	m_ImageViewInfo.pImageView = 0;
	m_ImageViewInfo.sizeImage.cx = 1024;
	m_ImageViewInfo.sizeImage.cy = 1024;

	m_nThreshold.nLow = 128;
	m_nThreshold.nHigh = 255;
	m_bBinaryMode = FALSE;

	m_bTrackRectangle = FALSE;
	m_nTrackerIdx = 0;
	m_bLockAddDelRect = FALSE;
	m_bLockMoveResizeRect = FALSE;
	m_bLockSelectRect = FALSE;

	m_lDebugFlag = 1;
	m_nJpegFineQuality = 95;
	m_nJpegDraftQuality = 80;

	m_EventCont.ResetEvent();
	m_EventAbort.ResetEvent();
	m_EventThres.ResetEvent();
	m_bWaiting = FALSE;

	m_pbufImage = NULL;
	m_pPreviousBuf = NULL;

	m_bInTeachProcess = FALSE;
	
	m_bImageResized = FALSE;

	m_bShowOverlay = TRUE;
	InitializeCriticalSection(&m_csDisplayBuffer);
}

CEvImageDoc::~CEvImageDoc()
{
	DeleteCriticalSection(&m_csDisplayBuffer);
}

BOOL CEvImageDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

int CEvImageDoc::InitViewBuffer(CImageBuffer* pbufImg)
{
	if(m_pbufImage != pbufImg) {
		m_pPreviousBuf = m_pbufImage;
		m_pbufImage = pbufImg;
		m_pbufImage->GetAdapter()->ChangeToGrayPalette();
		m_ImageViewInfo.pImageView->m_MagnifierWindow.AllocateBuffer(m_pbufImage->GetBitCount() == 24);
	}

	return 0;
}

int CEvImageDoc::RestorePreviousViewBuffer()
{
	InitViewBuffer(m_pPreviousBuf);
	return 0;
}



/////////////////////////////////////////////////////////////////////////////
// CEvImageDoc serialization

void CEvImageDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEvImageDoc diagnostics

#ifdef _DEBUG
void CEvImageDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEvImageDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEvImageDoc commands

CString CEvImageDoc::SaveImage(BOOL bSave) 
{
	if (m_pbufImage) {
		CFileDialog   dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Image Files (*.jpg;*.bmp)|*.jpg;*.bmp|All Files (*.*)|*.*||");
		
		CString strFileName;
		
		if((m_pbufImage==0) && (!m_pbufImage->IsAllocated())) {
			ASSERT(!"Image buffer does not exist or not allocated !!!");
			return "";
		}
		
		if (dlg.DoModal() == IDOK) {
			strFileName=dlg.GetPathName();
			CString strExt = dlg.GetFileExt();
			if(strExt.IsEmpty() || (strExt.CompareNoCase("bmp") && strExt.CompareNoCase("jpg"))) {
				strExt="bmp";
				strFileName+="."+strExt;
			}
			else
				strExt.MakeLower();

			if(bSave) {
				if(strExt=="bmp")
					SaveGrayScaleBMPFile(strFileName, *m_pbufImage);
				else if(strExt=="jpg")
					SaveJPGFile(strFileName, *m_pbufImage, m_nJpegFineQuality);
			}
			return dlg.GetPathName();
		}
	}

	return "";
}

CString CEvImageDoc::LoadImage() 
{
	if((m_pbufImage==0) && (!m_pbufImage->IsAllocated())) {
		ASSERT(!"Image buffer does not exist or not allocated !!!");
		return "";
	}

	CFileDialog   dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Image Files (*.jpg;*.bmp)|*.jpg;*.bmp|All Files (*.*)|*.*||");

	CString strFileName;

	if (dlg.DoModal() == IDOK) {
		strFileName=dlg.GetPathName();
		CString strExt = dlg.GetFileExt();
		strExt.MakeLower();

		//if(strExt=="bmp" || strExt.IsEmpty()) {
		//	LoadGrayScaleBMPFile(strFileName, *m_pbufImage);
		//	//LoadBmpImageAutoFit(strFileName);
		//}
		//else if(strExt=="jpg")
		//	LoadJPGFile(strFileName, *m_pbufImage);

		////strFileName = dlg.GetFileName();
		//DrwDel(&m_ImageViewInfo);
		//UpdateAllViews(NULL);
	}

	
	return strFileName;
}


int CEvImageDoc::LoadBmpImageAutoFit(CString strFileName)
{
	CImageBuffer buf;
	int nError = LoadGrayScaleBMPFile(strFileName, buf);
	if(nError)
		return nError;

	CopyImageAutoFit(&buf);

	return 0;
}

int CEvImageDoc::CopyImageAutoFit(CImageBuffer* pBuf)
{
	CSize newSize = pBuf->GetImgSize();
	CSize oldSize = m_pbufImage->GetImgSize();
	if(abs(newSize.cx - oldSize.cx) > 128) {
		ResizeDoc(newSize);
	}
	else {
		newSize.cx = min(oldSize.cx, newSize.cx);
		newSize.cy = min(oldSize.cy, newSize.cy);
	}
	m_pbufImage->GetAdapter()->ChangeToGrayPalette();

	CRect rect = CRect(CPoint(0,0), newSize);
	ImgCopy(pBuf, &rect, m_pbufImage, &rect);

	return 0;
}

void CEvImageDoc::LoadThumbnailImage(CString* strFileName)			// Load From Thumbnail
{
	if((m_pbufImage==0) && (!m_pbufImage->IsAllocated())) {
		ASSERT(!"Image buffer does not exist or not allocated !!!");
		//return "";
	}	
	LoadGrayScaleBMPFile(*strFileName, *m_pbufImage);
	m_pbufImage->GetAdapter()->ChangeToGrayPalette();	
	UpdateAllViews(NULL);
}

int CEvImageDoc::ResizeDoc(CSize szImage)
{

	return 0;
}

void CEvImageDoc::BinaryMode(BOOL binary)
{
	if((binary && !m_pbufImage->IsColor()) || (!binary && !m_BufSource.IsColor()))
		return;

	EnterCriticalSection(&m_csDisplayBuffer);
	CRect rcImgDisp = CRect(CPoint(0,0), m_pbufImage->GetImgSize());
	if(binary) {
		if(!m_BufSource.IsAllocated() || m_BufSource.GetImgSize() != rcImgDisp.Size())
			m_BufSource.Allocate(rcImgDisp.Size(), false, TRUE);
		ImgCopy(m_pbufImage, &rcImgDisp, &m_BufSource, &rcImgDisp);
		m_pbufImage->Allocate(rcImgDisp.Size(), true);
		ImgCopy(&m_BufSource, &rcImgDisp, m_pbufImage, &rcImgDisp);
	}
	else {
		if(m_BufSource.IsAllocated()) {
			m_pbufImage->Allocate(rcImgDisp.Size(), true, TRUE);
			ImgCopy(&m_BufSource, &rcImgDisp, m_pbufImage, &rcImgDisp);
			//m_BufSource.FreeImg();
		}
	}
	LeaveCriticalSection(&m_csDisplayBuffer);
}

int CEvImageDoc::SetDocSize(CSize szImage)
{
	m_ImageViewInfo.sizeImage = szImage;

	if (!m_ImageViewInfo.pImageView) {
		ASSERT(!"m_ImageViewInfo.pImageView not initialised");
		return -1;
	}

	m_ImageViewInfo.pImageView->SendMessage(WM_SET_GRAYPALLETTE);

	
//	fZoom = GetZoomLevel();
//	sizeTotal = pDoc->GetDocSize();

///	m_ImageViewInfo.pImageView->SetScrollSizes(MM_TEXT, szImage,CSize(20,20),CSize(5,5));
	m_ImageViewInfo.pImageView->SendMessage(WM_SETSCROLLSIZE, szImage.cx, szImage.cy);

//	SetZoomMinMax(0.125, 8);
//	SetZoomMode(SEC_ZOOMNORMAL);
//	SetZoomLevel(fZoom);


	return 0;
}

void CEvImageDoc::AddTrackRect(CRect rect, int nType)
{
	if (m_bLockAddDelRect)
		return ;
	
	TRACK_INFO Tracker;
	Tracker.rect = rect;
	Tracker.nTrackerType = nType;

	m_ImageViewInfo.pImageView->m_RectTrackAry.Add(Tracker);
	m_bTrackRectangle = TRUE;
	m_nTrackerIdx = (int) (m_ImageViewInfo.pImageView->m_RectTrackAry.GetSize() - 1);

		m_ImageViewInfo.pImageView->RedrawWindow();
}

void CEvImageDoc::DeleteAllDisplay(BOOL bDisplay)
{
	if (m_bLockAddDelRect)
		return ;

	m_ImageViewInfo.pImageView->m_DrwCurAry.RemoveAll();
	m_ImageViewInfo.pImageView->m_DrwRectAry.RemoveAll();
	m_ImageViewInfo.pImageView->m_DrwCircleAry.RemoveAll();
	m_ImageViewInfo.pImageView->m_DrwLineAry.RemoveAll();
	m_ImageViewInfo.pImageView->m_RectTrackAry.RemoveAll();
	m_ImageViewInfo.pImageView->m_DrwStrAry.RemoveAll();

	m_nTrackerIdx = 0;
	m_bTrackRectangle = FALSE;

	if(bDisplay)
		m_ImageViewInfo.pImageView->RedrawWindow();
}

void CEvImageDoc::DeleteTrackRect(BOOL bDisplay)
{
	if (m_bLockAddDelRect)
		return ;

	if (m_ImageViewInfo.pImageView->m_RectTrackAry.GetSize()) {
		if (m_nTrackerIdx >=m_ImageViewInfo.pImageView->m_RectTrackAry.GetSize()) {
			AfxMessageBox("Error ::Unknown Track Idx to Delete");
			return;
		}
		m_ImageViewInfo.pImageView->m_RectTrackAry.RemoveAt(m_nTrackerIdx);
		m_nTrackerIdx--;
		if (m_ImageViewInfo.pImageView->m_RectTrackAry.GetSize()==0) {
			m_ImageViewInfo.pImageView->m_RectTrackAry.FreeExtra();
			m_nTrackerIdx = 0;
			m_bTrackRectangle = FALSE;
		}
		else if (m_nTrackerIdx<0)
			m_nTrackerIdx = (int) (m_ImageViewInfo.pImageView->m_RectTrackAry.GetSize() - 1);
		
		if (bDisplay)
			m_ImageViewInfo.pImageView->RedrawWindow();
	}
}

void CEvImageDoc::HideTrackRect(BOOL bDisplay)
{
	CRect rc;
	if (m_bLockAddDelRect)
		return ;

	if (m_ImageViewInfo.pImageView->m_RectTrackAry.GetSize()) {
		if (m_nTrackerIdx >=m_ImageViewInfo.pImageView->m_RectTrackAry.GetSize()) {
			AfxMessageBox("Error ::Unknown Track Idx to Hide");
			return;
		}
		m_ImageViewInfo.pImageView->m_RectTrackAry[m_nTrackerIdx].rect = CRect(0,0,0,0);
		m_nTrackerIdx--;
		if (m_ImageViewInfo.pImageView->m_RectTrackAry.GetSize()==1) {
			m_ImageViewInfo.pImageView->m_RectTrackAry.RemoveAt(m_nTrackerIdx);
			m_ImageViewInfo.pImageView->m_RectTrackAry.FreeExtra();
			m_nTrackerIdx = 0;
			m_bTrackRectangle = FALSE;
		}
		else if (m_ImageViewInfo.pImageView->m_RectTrackAry.GetSize()==0) {
			m_ImageViewInfo.pImageView->m_RectTrackAry.FreeExtra();
			m_nTrackerIdx = 0;
			m_bTrackRectangle = FALSE;
		}
		else if (m_nTrackerIdx<0)
			m_nTrackerIdx = (int) (m_ImageViewInfo.pImageView->m_RectTrackAry.GetSize() - 1);
		
		if (bDisplay)
			m_ImageViewInfo.pImageView->RedrawWindow();
	}
}

void CEvImageDoc::SelectNextTrackRect(BOOL bDisplay)
{
	if (m_ImageViewInfo.pImageView->m_RectTrackAry.GetSize()) {
		m_nTrackerIdx++;
		if (m_nTrackerIdx>=m_ImageViewInfo.pImageView->m_RectTrackAry.GetSize())
			m_nTrackerIdx=0;

		while (1) {
			if (m_ImageViewInfo.pImageView->m_RectTrackAry[m_nTrackerIdx].rect == CRect(0,0,0,0)) {
				m_nTrackerIdx++;
				if (m_nTrackerIdx>=m_ImageViewInfo.pImageView->m_RectTrackAry.GetSize())
					m_nTrackerIdx=0;
			}
			else
				break;
		}

		if (bDisplay)
			m_ImageViewInfo.pImageView->RedrawWindow();
	}
}

void CEvImageDoc::Continue()
{
	m_bWaiting = FALSE;
	m_EventCont.SetEvent();

}

void CEvImageDoc::Abort()
{
	m_bWaiting = FALSE;
	m_EventAbort.SetEvent();

}
void CEvImageDoc::Threshold()
{
	m_bWaiting = FALSE;
	m_EventThres.SetEvent();
}


int CEvImageDoc::Wait()
{
	DWORD dwObj;
	int nIdx;
	HANDLE Handle[3];

	m_EventCont.ResetEvent();
	m_EventAbort.ResetEvent();
	m_EventThres.ResetEvent();

	Handle[0] = (HANDLE)m_EventCont;
	Handle[1] = (HANDLE)m_EventAbort;
	Handle[2] = (HANDLE)m_EventThres;

	m_bWaiting = TRUE;
	dwObj = WaitForMultipleObjects(3,&Handle[0],FALSE, INFINITE);
	nIdx = dwObj - WAIT_OBJECT_0;
	m_EventCont.ResetEvent();
	m_EventAbort.ResetEvent();
	m_EventThres.ResetEvent();
	if (nIdx==0) {
		//User wants to continue
		return 0;
	}
	else {
		//User wants to Abort
		DeleteAllDisplay(TRUE);
		return nIdx;
	}
}

void CEvImageDoc::ZoomFit()
{
	POSITION pos = GetFirstViewPosition();
	CEvImageView* pView = (CEvImageView*) GetNextView(pos);
	pView->PostMessage(WM_ZOOM_FIT);
}
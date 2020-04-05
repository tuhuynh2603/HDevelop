#pragma once

#include "afxmt.h"
#include "ImageDisplayAdapter.h"

#ifdef _WINDOW_IF
	#define	DECLSPEC_WINDOW_IF _declspec(dllexport)
#else
	#define DECLSPEC_WINDOW_IF _declspec(dllimport)
#endif

class CImageBuffer;

///Udupa; 03Dec'2012
struct CFailImageStack
{
public:
	CImageBuffer bufFailImage;
	CString strFailImage;
	CString strFailTime;

	int nToolNo;
	int nMoldNo;
	int nGripperNo;

	CFailImageStack()
	{
		strFailImage = "";
		strFailTime = "";

		nToolNo = 0;
		nMoldNo = 0;
		nGripperNo = 0;
	}

	void Initialize(CString* strFailure, CString* strTime, int nTool, int nMold, int nGripper)
	{
		strFailImage = *strFailure;
		strFailTime = *strTime;

		nToolNo = nTool;
		nMoldNo = nMold;
		nGripperNo = nGripper;
	}
};

class DECLSPEC_WINDOW_IF CEvImageDoc : public CDocument
{
protected: // create from serialization only
	CEvImageDoc();
	DECLARE_DYNCREATE(CEvImageDoc)

// Attributes
public:
	IMAGEVIEW_INFO m_ImageViewInfo;	// Information about Calibration and Vision System

	long m_lDebugFlag;			// Flag for Storing Debug Options
	BOOL m_bInTeachProcess;
	BYTE m_nJpegFineQuality;
	BYTE m_nJpegDraftQuality;

	struct Threshold
	{
		int nLow;
		int nHigh;
	} m_nThreshold;
			// Current Threshold(For Teach/Binary Mode Use)
	BOOL m_bBinaryMode;			// TRUE if View is in Binary Mode()


	int m_nTrackerIdx;			// Current Rectangle in the Array of Track Rect which is in focus
	BOOL m_bTrackRectangle;		// TRUE if Doc need to Track Rectangle
	BOOL m_bLockAddDelRect;		// To Restrict Add/Delete of Track Rectangles
	BOOL m_bLockMoveResizeRect;	// To Restrict Move/Resize of Track Rectangles
	BOOL m_bLockSelectRect;		// To Restrict Re-Selection of Track Rectangles

	CEvent m_EventCont,			// Event to determine if User wants to continue to Next Step when the Operation is Paused
		   m_EventAbort,
		   m_EventThres;		// Event to determine if User want to abort operation
	BOOL m_bWaiting;			// TRUE if Inspect/Calibration/Teach Process Needs to Pause to wait for User Response

	BOOL m_bAutoResizeImage;
	BOOL m_bAutoResizeImageToMidRes;
	BOOL m_bAccurateResize;
	BOOL m_bImageResized;

	BOOL m_bShowOverlay;

	// Operations
public:
	CString SaveImage(BOOL bSave=TRUE);
	CString LoadImage();

	int LoadBmpImageAutoFit(CString strFileName);
	int CopyImageAutoFit(CImageBuffer* buf);

	int InitViewBuffer(CImageBuffer* pbufImg);			// Initialise View Image buffer
	int RestorePreviousViewBuffer();				// Restore Previous View Image buffer
	void LoadThumbnailImage(CString* strFileName);			// Load From Thumbnail
	
	void AddTrackRect(CRect rect, int nType);	// Add a Track Rect to the Array
	void DeleteAllDisplay(BOOL bDisplay);			// Delete All Display Stuff
	void DeleteTrackRect(BOOL bDisplay);			// Delete the Selected Track Rect
	void SelectNextTrackRect(BOOL bDisplay);		// Select Next Track Rect
	void HideTrackRect(BOOL bDisplay);				// Only Hide the Rect and not delete it so that it does not screw up the Array Idx
	

	void Continue();								// User wants to continue to Next Step after Pausing
	void Abort();									// User wants to abort current operation
	int Wait();
	void Threshold();

	void ZoomFit();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEvImageDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	CImageBuffer* m_pbufImage;
	CImageBuffer m_BufSource;
	CRITICAL_SECTION m_csDisplayBuffer;
	CSize GetDocSize()  //called by ImageView to get size info.
    {return m_ImageViewInfo.sizeImage; }
	
	virtual int ResizeDoc(CSize szImage);

	int SetDocSize(CSize szImage);
	virtual ~CEvImageDoc();
	void BinaryMode(BOOL binary);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CEvImageDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CImageBuffer* m_pPreviousBuf;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

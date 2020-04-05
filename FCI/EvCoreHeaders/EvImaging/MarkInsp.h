#ifndef _MARK_INSP
#define _MARK_INSP

#include "MarkStruct.h"
#include "PkgLocStruct.h"
#include "AppDef.h"

#include "Utilities.h"
#include "Imaging.h"

class CMarkInspect {

public :

//Public Methods
	CMarkInspect();
	~CMarkInspect();
	static void InitMarkInspParm(MARK_INSP_PARM *pMarkInspParm);

	static void InitMarkTchInfo(MARK_TCH_INFO *pMarkTchInfo);

	int InspectMark(IMAGEVIEW_INFO *pView,						// System Information
					CImageBuffer *pImgBufSrc,				    // Source Image Buffer
					CImageBuffer *pImgDispBufSrc,
					PKG_LOC_RESULT *pPkgLocRslt,			// Found Pkg Location Information
					MARK_INSP_PARM *pMarkInspParm,			// Mark Inspection Parameters
					MARK_TCH_INFO *pMarkTchInfo,			// Mark Teach Information
					MARK_INSP_RESULT *pMarkInspResult,		// Mark Inspection Result
					CSize* pcsZoomFactor,
					long lDebugFlag);

	int InspectRotMark(IMAGEVIEW_INFO *pView,
					  CImageBuffer *pImgBufSrc,
					  CImageBuffer *pImgDispBufSrc,
					  PKG_LOC_RESULT *pPkgLocRslt,
					  PKG_LOC_RESULT *pPkgLocDispRslt,
					  MARK_INSP_PARM *pMarkInspParm,
					  MARK_TCH_INFO *pMarkTchInfo,
					  MARK_TCH_INFO* pMarkRotTchInfo,
					  int* pnSelectedRotTchInfo,
					  double dMarkRotAngleStep,
					  int nMarkRotTchInfoNo,
					  MARK_INSP_RESULT *pMarkInspResult,
					  CSize* pcsZoomFactor,
					  long lDebugFlag);

	int InspectRotMark(IMAGEVIEW_INFO *pView,
					  CImageBuffer *pImgBufSrc,
					  MARK_INSP_PARM *pMarkInspParm,
					  MARK_TCH_INFO *pMarkTchInfo,
					  MARK_TCH_INFO* pMarkRotTchInfo,
					  int* pnSelectedRotTchInfo,
					  int nAngleDeg,
					  int nMarkRotTchInfoNo,
					  MARK_INSP_RESULT *pMarkInspResult,
					  long lDebugFlag);


	int HostCheckSymbolSet(IMAGEVIEW_INFO *pView,				// System Information
						   CImageBuffer *pImgBufSrc,				// Source Image Buffer
						   CImageBuffer *pRefImgBufH,			// Pointer to Reference Image buffer(for Comparison)
						   CImageBuffer *pRefTmplBufH,			// Pointer to Template Image buffer(for Correlation)
						   PKG_LOC_RESULT *pPkgLocRslt,		// Found Pkg Location Information
						   SYM_INSP_PARM_SET *pSymParm,		// Symbol Set Inspection Parameters(Subset of Mark Inspect Parameters)
						   SYM_TCH_INFO_SET *pSymTch,		// Symbol Teach Information(Subset of Mark Teach Information)
						   SYM_INSP_RSLT_SET *pSymRslt,		// Symbol Inspection Result(Subset of Mark Inspect Result)
						   long lDebugFlag);

	int HostCheckRotSymbolSet(IMAGEVIEW_INFO *pView,				// System Information
						   CImageBuffer *pImgBufSrc,				// Source Image Buffer
						   CImageBuffer *pImgDispBufSrc,
						   CImageBuffer *pRefImgBufH,			// Pointer to Reference Image buffer(for Comparison)
						   CImageBuffer *pRefTmplBufH,			// Pointer to Template Image buffer(for Correlation)
						   CPoint2D* ppt2DTchPkgCenter,
						   double dTchPkgAngle,
						   PKG_LOC_RESULT *pPkgLocRslt,		// Found Pkg Location Information
						   PKG_LOC_RESULT *pPkgLocDispRslt,
						   SYM_INSP_PARM_SET *pSymParm,		// Symbol Set Inspection Parameters(Subset of Mark Inspect Parameters)
						   SYM_TCH_INFO_SET *pSymTch,		// Symbol Teach Information(Subset of Mark Teach Information)
						   SYM_INSP_RSLT_SET *pSymRslt,		// Symbol Inspection Result(Subset of Mark Inspect Result)
						   CSize* pcsZoomFactor,
						   long lDebugFlag);

	int HostCheckRotSymbolSet(IMAGEVIEW_INFO *pView,				// System Information
						   CImageBuffer *pImgBufSrc,				// Source Image Buffer
						   CImageBuffer *pRefImgBufH,			// Pointer to Reference Image buffer(for Comparison)
						   CImageBuffer *pRefTmplBufH,			// Pointer to Template Image buffer(for Correlation)
						   CPoint2D* ppt2DTchPkgCenter,
						   double dTchPkgAngle,
						   SYM_INSP_PARM_SET *pSymParm,		// Symbol Set Inspection Parameters(Subset of Mark Inspect Parameters)
						   SYM_TCH_INFO_SET *pSymTch,		// Symbol Teach Information(Subset of Mark Teach Information)
						   SYM_INSP_RSLT_SET *pSymRslt,		// Symbol Inspection Result(Subset of Mark Inspect Result)
						   long lDebugFlag);


	int HostInspTmpl(IMAGEVIEW_INFO *pView,					// Vision System Info
						 CImageBuffer *pImgBufSrc,				// Source Image Buffer
						 CImageBuffer *pImgDispBufSrc,
						 CImageBuffer *pRefImg,					// Reference Img
					     CImageBuffer *pRefTmpl,					// Reference Tmpl
						 int nIdx,							// Ref Corr Template Idx
						 TMPL_INSP_PARM *pTmplInspParm,		// Template Inspect Parameters
						 TMPL_TCH_INFO *pTmplTchInfo,		// Template Teach Info
						 CRect *prectSearch,				// Search Rect Size
						 int nTchThres,						// Taught Threshold
						 int nTmplContrastTol,				// Template Contrast Tol
						 int nMarkColor,					// White or Black Marking
						 TMPL_INSP_RSLT *pTmplInspRslt,		// ptr to Inspected Result for Storing Info
						 CSize* pcsZoomFactor,
						 long lDebugFlag);

	int HostInspTmpl(IMAGEVIEW_INFO *pView,					// Vision System Info
					 CImageBuffer *pImgBufSrc,				// Source Image Buffer
					 CImageBuffer *pRefImg,					// Reference Img
					 CImageBuffer *pRefTmpl,					// Reference Tmpl
					 int nIdx,							// Ref Corr Template Idx
					 TMPL_INSP_PARM *pTmplInspParm,		// Template Inspect Parameters
					 TMPL_TCH_INFO *pTmplTchInfo,		// Template Teach Info
					 CRect *prectSearch,				// Search Rect Size
					 int nTchThres,						// Taught Threshold
					 int nTmplContrastTol,				// Template Contrast Tol
					 int nMarkColor,					// White or Black Marking
					 TMPL_INSP_RSLT *pTmplInspRslt,		// ptr to Inspected Result for Storing Info
					 long lDebugFlag);

	int HostInspRotTmpl(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufSrc,
					   CImageBuffer *pImgDispBufSrc,
					   CImageBuffer *pRefImg,
					   CImageBuffer *pRefTmpl,
					   int nIdx,
					   TMPL_INSP_PARM *pTmplInspParm,
					   TMPL_TCH_INFO *pTmplTchInfo,
					   CRect *prectSearch,
					   int nTchThres,
					   int nTmplContrastTol,
					   int nMarkColor,
					   TMPL_INSP_RSLT *pTmplInspRslt,
					   CSize* pcsZoomFactor,
					   long lDebugFlag);

	int HostInspRotTmpl(IMAGEVIEW_INFO *pView,
					   CImageBuffer *pImgBufSrc,
					   CImageBuffer *pRefImg,
					   CImageBuffer *pRefTmpl,
					   int nIdx,
					   TMPL_INSP_PARM *pTmplInspParm,
					   TMPL_TCH_INFO *pTmplTchInfo,
					   CRect *prectSearch,
					   int nTchThres,
					   int nTmplContrastTol,
					   int nMarkColor,
					   TMPL_INSP_RSLT *pTmplInspRslt,
					   long lDebugFlag);


	int ConvTchImg(IMAGEVIEW_INFO* pView,
				   CImageBuffer* pSrcImg,
				   CImageBuffer* pDstImg,
				   CRect* prectROI,
				   int nThres);

	static int SaveMarkInspParm(CString strDirectoryName,
					   CString strFileName,
					   MARK_INSP_PARM *pMarkInspParm);

	static int LoadMarkInspParm(CString strDirectoryName,
					   CString strFileName,
					   MARK_INSP_PARM *pMarkInspParm);

	int SaveMarkTchInfo(CString strDirectoryName,
					   CString strFileName,
					   MARK_TCH_INFO *pMarkTchInfo);

	int LoadMarkTchInfo(CString strDirectoryName,
					   CString strFileName,
					   MARK_TCH_INFO *pMarkTchInfo);

	int ScanForTmpl(IMAGEVIEW_INFO *pView,
					CImageBuffer *pImgBufSrc,
					CRect rect,
				    int nMarkColor,
					int nMinSize,
					int nThres,
				    int nWinExtX,
				    int nWinExtY,
					int nMaxRect,
					int *nFndBlob,
					CRect *rectFnd);

	int ScanForMarkTmpl(IMAGEVIEW_INFO *pView,	// Vision System Info
					CImageBuffer *pImg,		// Source Image Buffer
					CRect *prect,		// Pointer to Rect ROI
					int nMarkColor,		// Type of Marking(WHITE or BLACK)
					int nMinSize,		// Minimum Mark Size(Blob Size)
					int nThres,			// Teach Threshold
					int nWinExtX,		// Template Extension Size
					int nWinExtY,		// Template Extension Size
					int nMaxRect,		// Maximum Allowable Rects
					int *nFndBlob,		// Pointer to Found Template(Blobs) Count
					CRect *rectFnd);	// Pointer to Found Template(Blobs) Rect Pos	


	int GetTeachMarkAngle(IMAGEVIEW_INFO *pView,
						  CRect *rcFirst,
						  CRect *rcLast,
						  double *pdAngle);

	int GetTmplTchPixCnt(IMAGEVIEW_INFO *pView,
						 CImageBuffer *pImgBufSrc,
						 CRect *rect,
						 int nMarkColor,
						 int nThres,
						 long *lPixCount);

public :
	int LoadMarkTemplates(IMAGEVIEW_INFO *pView,
						  CString strDirectoryName,
						  CString strFileName,
						  MARK_INSP_PARM *pMarkInspParm,
						  MARK_TCH_INFO *pMarkTchInfo);

	int SaveMarkTemplate(IMAGEVIEW_INFO *pView,
						 CString strDirectoryName,
						 CString strFileName,
					     MARK_TCH_INFO *pMarkTchInfo,
					     int nIdx);

	int CreateMarkTemplate(IMAGEVIEW_INFO *pView,
						   MARK_INSP_PARM *pMarkInspParm,
						   MARK_TCH_INFO *pMarkTchInfo);


	int CreateRotMarkTemplate(IMAGEVIEW_INFO *pView,
							  MARK_INSP_PARM *pMarkInspParm,
							  MARK_TCH_INFO *pMarkTchInfo,
							  MARK_TCH_INFO* pMarkRotTchInfo,
							  double dMarkZoomRotAngleStep,
							  double dMarkRotAngleStep,
							  int nMarkRotTchInfoNo,
							  CSize* pcsZoomFactor);

	CString m_strErrorMsg;

private :
	int m_nMarkTemplateSet;
	int m_nLastTmplErrorID;		// The Template that Failed Inspection
	int m_nLastMarkSetErrorID;	// The MarkSet that Failed Inspection
	int m_nLastSymSetErrorID;	// The SymbolSet that Failed Inspection
	int m_nFailErrorCode;		// The Lastest Failed Error Code
	CString m_strSymSetErrMsg;
	static int GetMarkInspParm(CIniFile *ptrIniFile,MARK_INSP_PARM *pMarkInspParm);
	static int SetMarkInspParm(CIniFile *ptrIniFile,MARK_INSP_PARM *pMarkInspParm);
	int GetMarkTchInfo(CIniFile *ptrIniFile,MARK_TCH_INFO *pMarkTchInfo);
	int SetMarkTchInfo(CIniFile *ptrIniFile,MARK_TCH_INFO *pMarkTchInfo);
	CGrayScaleCorr m_GrayScaleCorr;
};

#endif
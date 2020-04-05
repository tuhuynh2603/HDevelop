#ifndef _APP_DEF
#define _APP_DEF

//typedef struct {
//	double			dAngle;
//	CRect			rcSrcROI;
//	CRect			rcDstROI;
//	CPoint2D		pt2DRotPoint;
//	CImageBuffer	*pSrCImageBuffer;
//	CImageBuffer	*pDstImgBuf;
//	BOOL			bAccurate;
//} IMG_ROT_PARM;

#define BRIGHT_FIELD			  0
#define DARK_FIELD                1
#define SINGLE_SPOT				  2
#define SINGLE_SPOT_BRIGHT		  2
#define MAX_BUFFERS				  5
#define PROCESS_ERROR			 99
#define VISION_TIMEOUT			 -2
#define IMAGE_SNAP_FAIL			 -3
#define USER_ABORT				-20
#define MARK_NOT_TAUGHT		   -100
//#define EDGE_DEFECT			   -101
#define OPTICAL_DEFECT		   -102
#define PATTERN_DEFECT		   -103

#define INSP_PASS				  0

#define IS_FAIL(nError) (nError != INSP_PASS)

// Mark Inspection
#define INSP_FAIL_MARK_CORR		 11
#define INSP_FAIL_MARK_EXCESS	 12
#define INSP_FAIL_MARK_MISSING	 13
#define INSP_FAIL_MARK_TMPL_FIND 14

#define INSP_FAIL_NO_DEVICE		 51
#define INSP_FAIL_NON_CIRCULAR	 52
#define INSP_FAIL_LENS_CRACK	 53
#define INSP_FAIL_DARK_SPOT 	 54
#define INSP_FAIL_HOLE			 55
#define INSP_FAIL_PATTERN_SHIFT	 56
#define INSP_FAIL_EDGE_CHIP		 57
#define INSP_FAIL_EDGE_FLASH	 58
#define INSP_FAIL_FIBER_SCRATCH	 59
#define INSP_FAIL_PATTERN		 60
#define INSP_FAIL_CONTAMINATION  61
#define INSP_FAIL_DOUBLE_LENS	 62
#define INSP_FAIL_TOP_FIBER_SCRATCH	 63

enum ThreadTypes
{
	TRACK_THREAD,
	INSPECTION_THREAD,
	IMAGE_SAVE_THREAD,
	SEQUENCE_THREAD,
	DISPLAY_THREAD,
	VISION_RESET_THREAD,
	POLLING_THREAD
};

#endif
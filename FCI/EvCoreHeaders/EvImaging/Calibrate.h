#ifndef _CALIBRATE
#define _CALIBRATE

#include "Imaging.h"
#include "ExternalFunctions.h"
#include "Utilities.h"

///////////////////////////////////////////////////////////////////////////////
class CCalibrate
{
public :
// Public Members
	CString m_strErrorMsg;

// Public Methods
	CCalibrate();
	~CCalibrate();

	int TapeCalibrate(IMAGEVIEW_INFO *pView,
					  CImageBuffer *pImgBufSrc,
					  long lDebugFlag);

	int DeviceCalibrate(IMAGEVIEW_INFO *pView,
					  CImageBuffer *pImgBufSrc,
					  long lDebugFlag);

	int LoadCalData(CString strPath,
				CString strFileName,
				PIX_CAL_INFO *pPixCalInfo);

	int SaveCalData(CString strPath,
				CString strFileName,
				PIX_CAL_INFO *pPixCalInfo);

	int DrawTapeCalibrate(IMAGEVIEW_INFO *pView,
						  CLine2D *TopLine,
						  CLine2D *BotLine,
						  CLine2D *LftLine,
						  CLine2D *RhtLine);

private :
	int GetCalData(CIniFile *ptrIniFile,			// Pointer to the Ini File for Read/Write
				  PIX_CAL_INFO *pPixCalInfo);	// Pixel Calibration Information

	int SetCalData(CIniFile *ptrIniFile,			// Pointer to the Ini File for Read/Write
				  PIX_CAL_INFO *pPixCalInfo);	// Pixel Calibration Information

};
#endif



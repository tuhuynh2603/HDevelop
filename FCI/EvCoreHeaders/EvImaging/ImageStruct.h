#pragma once

#include "ImageDef.h"
#include "ImageBuffer.h"

#include "stdafx.h"
#include "Afxmt.h"

#include "MathDef.h"


class CEvImageView;

typedef struct {
	int nCalUnitType;					// Calibration Unit Type (mm,mil,micron)
	double dXCalBlockSize;				// Calibration Block X Size
	double dYCalBlockSize;				// Calibration Block Y Size
	double dXCalDotPitch;				// Calibration Ball Pattern X Pitch
	double dYCalDotPitch;				// Calibration Ball Pattern Y Pitch
	int nTableXSize;					// Calibration Dot X Number
	int nTableYSize;					// Calibration Dot Y Number
	double dXPixSize;					// Pixel Size Ratio on X-Axis
	double dYPixSize;					// Pixel Size Ratio on Y-Axis
	CRect rectTeachCalBlock;			// Teach Window Pos and Size
	int nCalType;
} PIX_CAL_INFO;

typedef struct {
	CEvImageView* pImageView;
	CSize sizeImage;
	PIX_CAL_INFO PixCalInfo;
} IMAGEVIEW_INFO;



typedef struct {
	CPoint pt1,pt2;						// Start and End Position of Line
	LOGPEN PenInfo;						// Line Style
} LINE_INFO;

typedef struct {
	CRect rect;							// Rectangle Position
	LOGPEN PenInfo;						// Rectangle Style
} RECT_INFO;

typedef struct {
	CPoint ptCursor;					// Cursor Center Point Position
	int nExt;							// Cursor Tail Extension
	LOGPEN PenInfo;						// Cursor Style
	char charCursor;
} CUR_INFO;

typedef struct{
	POINT pt;
	CString strMsg;
	int FontHeight;
	COLORREF Color;
} STR_ATTR;

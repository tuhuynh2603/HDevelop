#pragma once
#include "ImageStruct.h"

class CBlockArc
{
public:
	CBlockArc();
	~CBlockArc();
	CBlockArc & operator =(CBlockArc BlockArc);
	int Draw(IMAGEVIEW_INFO *pView,
			LOGPEN* pPen);
	int Draw(CDC* pDC, CSize& sizeMax, LOGPEN& pen);
	double dRadiusOuter;
	double dRadiusInner;
	double dAngleStart;
	double dAngleEnd;
	CPoint2D pt2DCenter;
	int nOffset;

	int nPerimeter;
	int nStartIndex;
	int nEndIndex;
	int nRectInnerPos;
	int nRectOuterPos;
	int nFailType;
};


/// Udupa; January'2013
class CVisionResult
{
public:
	CVisionResult();
	~CVisionResult();
	static int GetSize();
	int Create(BYTE* pSource, int nResultArg=0);

	int nResult;
	int nCursors;
	int nDefectRects;
	int nDefectRegions;
	CPoint2D* pCursors;
	CRect* pDefectRects;
	CBlockArc* pDefectRegions;
	BYTE* pData;
	BOOL bCreated;
};

typedef CArray<CRect, CRect> CRectArray;
typedef CArray<CBlockArc, CBlockArc> CRegionArray;

int DrawDefectRegions(CDC* pDC, CSize& sizeMax, int nResult, CRectArray& rects, CRegionArray& regions);

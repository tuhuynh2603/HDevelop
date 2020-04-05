#pragma once

#include "stdafx.h"
#include "MathAlgo.h"

#define INT_AND_DBL_EQ_TOL			0.001

int CheckRect(CRect *Rect,
			  CSize *ImgSize);

int CheckRect(CRect *RectSrc, 
			  CRect *RectDst);

int ValidateRect(CRect *Rect,
				 CSize *ImgSize);

int ValidateRect(CRect *RectSrc,
				 CRect *RectCheck);

int ValidateRect(CRect *Rect);

int CheckPoint(CPoint *pt,
			   CSize *ImgSize);

int CheckPoint(CPoint2D *pt2D, 
			   CSize *ImgSize);

int CheckPoint(CPoint *pt, 
			   CRect* prcROI);

int CheckPoint(CPoint2D *pt2D, CRect* prcROI);

int ValidatePoint(CPoint *pt,
				  CSize *ImgSize);

int ValidatePoint(CPoint *pt,
				  CRect* prcROI);

int ValidatePoint(CPoint2D *pt2D,
				  CSize *ImgSize);

int ValidatePoint(CPoint2D *pt2D, 
			   CRect* prcROI);

int RectInRect(CRect rect1,				// Bigger Rect
			   CRect rect2);			// Smaller Rect

int RectOverlapRect(CRect rect1, 
					CRect rect2);

int RectOverlapRect(CRect rect1, 
					CRect rect2,
					CRect* prcOverlap);

int PtInRect(CPoint* ppt, 
			 CRect* prc);

int PtInRect(CPoint2D* ppt2D, 
			 CRect* prc);

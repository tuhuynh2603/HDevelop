
#include "stdafx.h"
#include "MathValidations.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int CheckRect(CRect *Rect, CSize *ImgSize) {

	if (Rect->top<0)
		Rect->top=0;

	if (Rect->left<0)
		Rect->left=0;

	if (Rect->bottom >= ImgSize->cy)
		Rect->bottom  = ImgSize->cy;

	if (Rect->right >= ImgSize->cx)
		Rect->right  = ImgSize->cx;

	if ((Rect->left > Rect->right) || (Rect->top > Rect->bottom))
		return -1;

	return 0;
}


int CheckRect(CRect *RectSrc, CRect *RectDst) {

	if (RectDst->top < RectSrc->top)
		RectDst->top = RectSrc->top;
	if (RectDst->left < RectSrc->left)
		RectDst->left = RectSrc->left;
	if (RectDst->bottom >= RectSrc->bottom)
		RectDst->bottom  = RectSrc->bottom;
	if (RectDst->right >= RectSrc->right)
		RectDst->right  = RectSrc->right;

	if ((RectDst->left > RectDst->right) || (RectDst->top > RectDst->bottom))
		return -1;

	return 0;
}

int ValidateRect(CRect *Rect, CSize *ImgSize) {

	if (Rect->top<0)
		return -1;

	if (Rect->left<0)
		return -1;

	if (Rect->bottom > ImgSize->cy)
		return -1;

	if (Rect->right > ImgSize->cx)
		return -1;

	if ((Rect->left > Rect->right) || (Rect->top > Rect->bottom))
		return -1;

	return 0;
}

int ValidateRect(CRect *RectSrc,
				 CRect *RectCheck) {

	if (RectCheck->top < RectSrc->top)
		return -1;

	if (RectCheck->left < RectSrc->left)
		return -1;

	if (RectCheck->bottom > RectSrc->bottom)
		return -1;

	if (RectCheck->right > RectSrc->right)
		return -1;

	if ((RectCheck->left > RectCheck->right) || (RectCheck->top > RectCheck->bottom))
		return -1;

	return 0;
}


int ValidateRect(CRect *Rect) {
	if (Rect->top<0)
		return -1;

	if (Rect->left<0)
		return -1;

	if ((Rect->left > Rect->right) || (Rect->top > Rect->bottom))
		return -1;
	return 0;
}

int CheckPoint(CPoint *pt, CSize *ImgSize) {

	if (pt->x<0)
		pt->x=0;

	if (pt->y<0)
		pt->y=0;

	if (pt->x >= ImgSize->cx)
		pt->x  = ImgSize->cx-1;

	if (pt->y >= ImgSize->cy)
		pt->y  = ImgSize->cy-1;

	return 0;
}

int CheckPoint(CPoint2D *pt2D, CSize *ImgSize) {

	if (pt2D->x<0)
		pt2D->x = INT_AND_DBL_EQ_TOL;

	if (pt2D->y<0)
		pt2D->y = INT_AND_DBL_EQ_TOL;

	if (pt2D->x >= ImgSize->cx)
		pt2D->x  = ImgSize->cx - 1 - INT_AND_DBL_EQ_TOL;

	if (pt2D->y >= ImgSize->cy)
		pt2D->y  = ImgSize->cy - 1 - INT_AND_DBL_EQ_TOL;

	return 0;
}

int CheckPoint(CPoint2D *pt2D, CRect* prcROI) {
	if (pt2D->x < prcROI->left) 
		pt2D->x = prcROI->left + INT_AND_DBL_EQ_TOL;
	if (pt2D->x >= (prcROI->right - 1))
		pt2D->x = prcROI->right - 1 - INT_AND_DBL_EQ_TOL;
	if (pt2D->y < prcROI->top) 
		pt2D->y = prcROI->top + INT_AND_DBL_EQ_TOL;
	if (pt2D->y >= (prcROI->bottom - 1)) 
		pt2D->y = prcROI->bottom - 1 - INT_AND_DBL_EQ_TOL;
	return 0;
}

int CheckPoint(CPoint *pt, CRect* prcROI) {
	if (pt->x < prcROI->left) 
		pt->x = prcROI->left;
	if (pt->x > (prcROI->right - 1))
		pt->x = prcROI->right - 1;
	if (pt->y < prcROI->top) 
		pt->y = prcROI->top;
	if (pt->y > (prcROI->bottom - 1)) 
		pt->y = prcROI->bottom - 1;
	return 0;
}

int ValidatePoint(CPoint *pt, CRect* prcROI) {

	if (pt->x < prcROI->left) 
		return 1;

	if (pt->x >= prcROI->right) 
		return 1;

	if (pt->y < prcROI->top) 
		return 1;

	if (pt->y >= prcROI->bottom) 
		return 1;
	return 0;
}

int ValidatePoint(CPoint2D *pt2D, CRect* prcROI) {
// INT_AND_DBL_EQ_TOL is used to fix the computer accuracy problem.
// For computer, 1.00000 may be greater than 1.

	if (pt2D->x < (prcROI->left - INT_AND_DBL_EQ_TOL)) 
		return -1;
	if (pt2D->x < prcROI->left) 
		pt2D->x = prcROI->left + INT_AND_DBL_EQ_TOL;
	if (pt2D->x > (prcROI->right - 1 + INT_AND_DBL_EQ_TOL)) 
		return -1;
	if (pt2D->x > (prcROI->right - 1)) 
		pt2D->x = prcROI->right - 1 - INT_AND_DBL_EQ_TOL;
	if (pt2D->y < (prcROI->top - INT_AND_DBL_EQ_TOL)) 
		return -1;
	if (pt2D->y < prcROI->top) 
		pt2D->y = prcROI->top + INT_AND_DBL_EQ_TOL; 
	if (pt2D->y > (prcROI->bottom - 1 + INT_AND_DBL_EQ_TOL)) 
		return -1;
	if (pt2D->y > (prcROI->bottom - 1)) 
		pt2D->y = prcROI->bottom - 1 - INT_AND_DBL_EQ_TOL; 
	return 0;
}

int ValidatePoint(CPoint *pt, CSize *ImgSize) {

	if (pt->x<0)
		return -1;

	if (pt->y<0)
		return -1;

	if (pt->x >= ImgSize->cx)
		return -1;

	if (pt->y >= ImgSize->cy)
		return -1;

	return 0;
}

int ValidatePoint(CPoint2D *pt2D, CSize *ImgSize) {

	if(pt2D->x<-INT_AND_DBL_EQ_TOL)
		return -1;
	if(pt2D->x<0)
		pt2D->x = INT_AND_DBL_EQ_TOL;
	if (pt2D->y<-INT_AND_DBL_EQ_TOL)
		return -1;
	if(pt2D->y<0)
		pt2D->y = INT_AND_DBL_EQ_TOL;
	if (pt2D->x > (ImgSize->cx - 1 + INT_AND_DBL_EQ_TOL))
		return -1;
	if (pt2D->x > (ImgSize->cx - 1))
		pt2D->x = ImgSize->cx - 1 - INT_AND_DBL_EQ_TOL;
	if (pt2D->y > (ImgSize->cy - 1 + INT_AND_DBL_EQ_TOL))
		return -1;
	if (pt2D->y > (ImgSize->cy - 1))
		pt2D->y = ImgSize->cy - 1 - INT_AND_DBL_EQ_TOL;

	return 0;
}

int RectInRect(CRect rect1, CRect rect2)
{
	CPoint ptTL,
		   ptTR,
		   ptBL,
		   ptBR;

	BOOL bTLInRect,
		 bTRInRect,
		 bBLInRect,
		 bBRInRect;

	bTLInRect=bTRInRect=bBLInRect=bBRInRect=FALSE;

	ptTL = CPoint(rect2.left, rect2.top);
	ptTR = CPoint(rect2.right - 1, rect2.top);
	ptBL = CPoint(rect2.left, rect2.bottom - 1);
	ptBR = CPoint(rect2.right - 1, rect2.bottom - 1);

	if (rect1.PtInRect(ptTL))
		bTLInRect = TRUE;
	if (rect1.PtInRect(ptTR))
		bTRInRect = TRUE;
	if (rect1.PtInRect(ptBL))
		bBLInRect = TRUE;
	if (rect1.PtInRect(ptBR))
		bBRInRect = TRUE;

	if (bTLInRect&&bTRInRect&&bBLInRect&&bBRInRect) {
		return 1;
	}
	else {
		return 0;
	}
}

int RectOverlapRect(CRect rect1, CRect rect2)
{
	CRect rcOverlap;
	if (rect1.left < rect2.left)
		rcOverlap.left = rect2.left;
	else
		rcOverlap.left = rect1.left;
	if (rect1.top < rect2.top)
		rcOverlap.top = rect2.top;
	else
		rcOverlap.top = rect1.top;
	if (rect1.right < rect2.right)
		rcOverlap.right = rect1.right;
	else
		rcOverlap.right = rect2.right;
	if (rect1.bottom < rect2.bottom)
		rcOverlap.bottom = rect1.bottom;
	else
		rcOverlap.bottom = rect2.bottom;
	if(rcOverlap.left > rcOverlap.right)
		return 0;
	if(rcOverlap.top > rcOverlap.bottom)
		return 0;
	return 1;
}

int RectOverlapRect(CRect rect1, 
					CRect rect2,
					CRect* prcOverlap)
{
	CRect rcOverlap;
	if (rect1.left < rect2.left)
		rcOverlap.left = rect2.left;
	else
		rcOverlap.left = rect1.left;
	if (rect1.top < rect2.top)
		rcOverlap.top = rect2.top;
	else
		rcOverlap.top = rect1.top;
	if (rect1.right < rect2.right)
		rcOverlap.right = rect1.right;
	else
		rcOverlap.right = rect2.right;
	if (rect1.bottom < rect2.bottom)
		rcOverlap.bottom = rect1.bottom;
	else
		rcOverlap.bottom = rect2.bottom;
	if(rcOverlap.left > rcOverlap.right)
	{
		*prcOverlap = CRect(0,0,0,0);
		return 0;
	}
	if(rcOverlap.top > rcOverlap.bottom)
	{
		*prcOverlap = CRect(0,0,0,0);
		return 0;
	}
	*prcOverlap = rcOverlap;
	return 1;
}

int PtInRect(CPoint* ppt, CRect* prc) {
	if((prc->left <= ppt->x) && (ppt->x < prc->right) &&
	   (prc->top <= ppt->y) && (ppt->y < prc->bottom))
	   return 1;
	else
	   return 0;
}

int PtInRect(CPoint2D* ppt2D, CRect* prc) {
	if((prc->left <= ppt2D->x) && (ppt2D->x < prc->right) &&
	   (prc->top <= ppt2D->y) && (ppt2D->y < prc->bottom))
	   return 1;
	else
	   return 0;
}

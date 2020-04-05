#include "stdafx.h"
#include <math.h>
#include "MathAlgo.h"
#include "MathValidations.h"
#include "Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CPoint2D RotatePoint2D(CPoint2D ptOrg,
					   CPoint2D ptRef,
					   double dAngle) {

	CPoint2D ptNewPoint,
			 ptDelta;
	
	double dDistance,
		   dOldAngle,
		   dNewAngle;
	
	ptDelta = ptOrg - ptRef;
	dDistance = sqrt(ptDelta.x * ptDelta.x + ptDelta.y * ptDelta.y);
	dOldAngle = atan2(ptDelta.y,ptDelta.x);
	dNewAngle = dOldAngle + dAngle;
	ptNewPoint.x = dDistance * cos(dNewAngle);
	ptNewPoint.y = dDistance * sin(dNewAngle);
	ptNewPoint.x += ptRef.x;
	ptNewPoint.y += ptRef.y;

	return ptNewPoint;
}

CPoint2D RotatePoint2DOfRigidObject(CPoint2D ptOrg_PC,
						 CPoint2D ptOrg_OC,
						 CPoint2D ptRef_PC,
						 double dAngle) 
{
	CPoint2D ptNewPoint,
			 ptDelta;
	
	double dDistance,
		   dOldAngle,
		   dNewAngle;
	
	// get the invariant vector in rigid object coordinate system
	ptDelta = ptOrg_OC - ptOrg_PC;
	// get the length of this invariant vector
	dDistance = sqrt(ptDelta.x * ptDelta.x + ptDelta.y * ptDelta.y);
	// get the angle of this vector in object coordinate system
	dOldAngle = atan2(ptDelta.y,ptDelta.x);
	// get the new angle
	dNewAngle = dOldAngle + dAngle;
	// get the coordinate after rotation
	ptNewPoint.x = dDistance * cos(dNewAngle);
	ptNewPoint.y = dDistance * sin(dNewAngle);
	ptNewPoint.x += ptRef_PC.x;
	ptNewPoint.y += ptRef_PC.y;

	return ptNewPoint;
}

CRect RotateRecWindow2D(CPoint2D ptOrg_PC,
						 CRect rcOrg_OC,
						 CPoint2D ptRef_PC,
						 double dAngle) 
{

	CPoint2D ptNewPoint,
			 ptDelta;
	
	CRect rcNew;
	int nStartX, nStartY, nEndX, nEndY;

	// Compute the center of template window after rotation
	ptNewPoint=RotatePoint2DOfRigidObject(ptOrg_PC,rcOrg_OC.CenterPoint(),ptRef_PC,dAngle);
	
	nStartX = (int)ptNewPoint.x - rcOrg_OC.Width()/2;
	nStartY = (int)ptNewPoint.y - rcOrg_OC.Height()/2;
	nEndX = (int)ptNewPoint.x + rcOrg_OC.Width()/2;
	nEndY = (int)ptNewPoint.y + rcOrg_OC.Height()/2;

	rcNew.SetRect(nStartX, nStartY, nEndX, nEndY); 
	return rcNew;
}

CPoint2D::CPoint2D() {
/* Initializes coordinates to (0,0) */
	x=y=0;
}

CPoint2D::CPoint2D(CPoint p)
{
	x=p.x;y=p.y;
}

CPoint2D::CPoint2D(int x0, int y0)
{
	x=x0;y=y0;
}

CPoint2D::CPoint2D(double x0, double y0)
{
	x=x0;y=y0;
}

CPoint2D::CPoint2D(int x0, double y0)
{
	x=x0;y=y0;
}
CPoint2D::CPoint2D(double x0, int y0) 
{
	x=x0;y=y0;
}

CPoint2D::CPoint2D(const CPoint2D &p)
{
	x=p.x;y=p.y;
}

double CPoint2D::GetDistance(CPoint2D pt2D) {
	return sqrt(pow(x-pt2D.x,2)+pow(y-pt2D.y,2));
}

double CPoint2D::GetDistance(CPoint2D* ppt2D) {
	return sqrt(pow(x-ppt2D->x,2)+pow(y-ppt2D->y,2));
}

CPoint2D CPoint2D::GetInterpol(CPoint2D* ppt2DStart,
							   CPoint2D* ppt2DEnd,
							   double dRatio) {
	x = dRatio * ppt2DEnd->x + (1 - dRatio) * ppt2DStart->x;
	y = dRatio * ppt2DEnd->y + (1 - dRatio) * ppt2DStart->y;
	return *this;
}

BOOL CPoint2D::operator == (CPoint2D pt2D) {
	return (pt2D.x==x && pt2D.y==y);
}

CPoint2D & CPoint2D::operator += (const CPoint2D &pt2dAdd) 
{
    x += pt2dAdd.x;
    y += pt2dAdd.y;
	return *this;
}

CPoint2D & CPoint2D::operator -= (const CPoint2D &v) 
{
    x -= v.x;
    y -= v.y;
	return *this;
}

CPoint2D & CPoint2D::operator /= (const double d) 
{
	if( d != 0) {
	    x = x / d;
		y = y / d;
	}
	else {
		if(x < 0) 
			x = -1.0E+50;
		else
			x = 1.0E+50;
		if(y < 0) 
			y = -1.0E+50;
		else
			y = 1.0E+50;
	}
	return *this;
}

CPoint2D & CPoint2D::operator / (const double d) 
{
	if( d != 0) {
	    x = x / d;
		y = y / d;
	}
	else {
		if(x < 0) 
			x = -1.0E+50;
		else
			x = 1.0E+50;
		if(y < 0) 
			y = -1.0E+50;
		else
			y = 1.0E+50;
	}
	return *this;
}

void CPoint2D::SetPoint(const CPoint2D &pt2D) 
{
	x = pt2D.x;
	y = pt2D.y;
}

void CPoint2D::RotateAbout(CPoint2D pt2D,double dAngle) {
	double dx,dy;
	double dOldAngle,dNewAngle;
	double dDistance;
	CString strMsg;
	dx=x-pt2D.x;
	dy=y-pt2D.y;
	
	dDistance=sqrt(dx*dx+dy*dy);
	dOldAngle=atan2(dy,dx);
	dNewAngle=dOldAngle+dAngle;
	dx=dDistance*cos(dNewAngle);
	dy=dDistance*sin(dNewAngle);
	x=pt2D.x+dx;
	y=pt2D.y+dy;
}

CPoint2D CPoint2D::operator +(CPoint2D pt2dAdd)
{
	CPoint2D	pt;
	pt.x = x + pt2dAdd.x;
	pt.y = y + pt2dAdd.y;
	return pt;
}

CPoint2D & CPoint2D::operator =(CPoint2D pt2d)
{
	x = pt2d.x;
	y = pt2d.y;
	return *this;
}

CPoint2D CPoint2D::operator -(CPoint2D pt2dSub)
{
	CPoint2D	pt;
	pt.x = x - pt2dSub.x;
	pt.y = y - pt2dSub.y;
	return pt;
}



CSize2D::CSize2D() {
	cx=cy=0;
}

CSize2D::CSize2D(CSize cs) {
	cx=cs.cx;
	cy=cs.cy;
}

CSize2D::CSize2D(int x0, int y0) {
	cx=x0;
	cy=y0;
}

CSize2D::CSize2D(double x0, double y0) {
	cx=x0;
	cy=y0;
}

CSize2D::CSize2D(const CSize2D &cs) {
	cx=cs.cx;
	cy=cs.cy;
}

CSize2D & CSize2D::operator =(CSize2D cs2d)
{
	cx = cs2d.cx;
	cy = cs2d.cy;
	return *this;
}

CSize2D & CSize2D::operator =(CSize cs)
{
	cx = cs.cx;
	cy = cs.cy;
	return *this;
}

BOOL CSize2D::operator == (CSize2D cs2D) {
	return (cs2D.cx==cx && cs2D.cy==cy);
}

CPoint2DArray::CPoint2DArray()
{
}
CPoint2DArray::~CPoint2DArray()
{
	RemoveAll();
}
CPoint2DArray::CPoint2DArray(CPoint2DArray &Array)
{
	RemoveAll();
	for (int i=0;i<Array.GetSize();i++) {
		Add(Array[i]);
	}
}

CPoint2DArray::CPoint2DArray(CPoint* ppt, int nptNo)
{
	RemoveAll();
	for (int i=0;i<nptNo;i++) {
		Add(CPoint2D(ppt[i]));
	}
}

CPoint2DArray::CPoint2DArray(CPoint2D* ppt2D, int npt2DNo)
{
	RemoveAll();
	for (int i=0;i<npt2DNo;i++) {
		Add(ppt2D[i]);
	}
}

void CPoint2DArray::Zoom(CPoint2D* ppt2DCenter,
						double dRatio)
{
	int i;
	for(i=0;i<GetSize();i++) {
		(*this)[i].x = ppt2DCenter->x + ((*this)[i].x-ppt2DCenter->x) * dRatio;
		(*this)[i].y = ppt2DCenter->y + ((*this)[i].y-ppt2DCenter->y) * dRatio;
	}
}

int CPoint2DArray::GetGravityCenter(CPoint2D* ppt2DGravityCenter)
{
	int i;
	double dSumX;
	double dSumY;
	
	if(GetSize() <=0)
		goto ErrorAbort;
	dSumX = 0;
	dSumY = 0;
	for(i=0;i<GetSize();i++) {
		dSumX += (*this)[i].x;
		dSumY += (*this)[i].y;
	}
	ppt2DGravityCenter->x = dSumX/GetSize();
	ppt2DGravityCenter->y = dSumY/GetSize();
	return 0;
ErrorAbort:
	return -1;
}


CPoint2DArray & CPoint2DArray::operator =(CPoint2DArray &Array)
{
	RemoveAll();
	for (int i=0;i<Array.GetSize();i++) {
		Add(Array[i]);
	}
	return *this;
}

CVector2D::CVector2D()
{
}

CVector2D::CVector2D(const CVector2D &vt2D):CPoint2D(vt2D)
{
}

CVector2D::CVector2D(CPoint ptDst,CPoint ptSrc)
{
	x=ptDst.x-ptSrc.x;
	y=ptDst.y-ptSrc.y;
}

CVector2D::CVector2D(CPoint2D pt2DDst,CPoint2D pt2DSrc)
{
	x=pt2DDst.x-pt2DSrc.x;
	y=pt2DDst.y-pt2DSrc.y;
}

CVector2D::CVector2D(int nX0,int nY0):CPoint2D(nX0,nY0)
{
}

CVector2D::CVector2D(double dX0,double dY0):CPoint2D(dX0,dY0)
{
}

int CVector2D::Normalize()
{
	double dSize=GetSize();
	if(dSize) {
		x /= dSize;
		y /= dSize;
		return 0;
	}
	else 
		return -1;
}

double CVector2D::GetSize()
{
	return sqrt(x*x+y*y);
}

CVector2D & CVector2D::operator =(CVector2D vt2D)
{
	x=vt2D.x;
	y=vt2D.y;
	return *this;
}

int CVector2D::GetUnit(CVector2D* pvt2D)
{
	double dSize=GetSize();
	if(dSize) {
		*pvt2D = CVector2D(x/dSize,y/dSize);
		return 0;
	}
	else 
		return -1;
}

double CVector2D::operator ^ (CVector2D vt2D)
{
	return x*vt2D.x+y*vt2D.y;
}

BOOL CVector2D::IsValid()
{
	return (x!=0 || y!=0);
}

int CVector2D::GetAngleRad(CVector2D vt2D, 
						   double& dAngleRad)
{
	double dot,mag;
	if (!vt2D.IsValid()&&!IsValid()) {
		dot=*this^vt2D;
		mag=GetSize()*vt2D.GetSize();
		dAngleRad=acos(dot/mag);
		return 0;
	}
	else {
		return -1;
	}
}


CLine2D::CLine2D()
{
	m_bInValid = TRUE;
}

CLine2D::CLine2D(CPoint2D pt2D,CVector2D vt2D)
{
	Position = pt2D;
	Direction = vt2D;
	Direction.Normalize();
	m_bInValid = !Direction.IsValid();
}

CLine2D::CLine2D(CPoint2D pt2D,double dAngle)
{
	Position = pt2D;
	Direction = CPoint2D(cos(dAngle),sin(dAngle));
	m_bInValid = !Direction.IsValid();
}

CLine2D::CLine2D(CPoint2D pt2D0,CPoint2D pt2D1)
{
	Position = pt2D0;
	Direction = CPoint2D(pt2D1.x - pt2D0.x, pt2D1.y - pt2D0.y);
	Direction.Normalize();
	m_bInValid = !Direction.IsValid();
}

CLine2D::CLine2D(CPoint pt0, CPoint pt1)
{
	Position = pt0;
	Direction = CPoint2D(pt1.x - pt0.x,pt1.y - pt0.y);
	Direction.Normalize();
	m_bInValid = !Direction.IsValid();
}

CLine2D::~CLine2D()
{
}

CLine2D & CLine2D::operator =(CLine2D ln2D)
{
	Position = ln2D.Position;
	Direction = ln2D.Direction;
	m_bInValid = !Direction.IsValid();
	return *this;
}

void CLine2D::SetPosition(CPoint2D pt2D)
{
	Position = pt2D;
}

void CLine2D::SetPosition(double dPosX, double dPosY)
{
	Position.x = dPosX;
	Position.y = dPosY;
}
	
int CLine2D::SetDirection(CVector2D vt2D)
{
	Direction = vt2D;
	m_bInValid = !Direction.IsValid();
	if(m_bInValid) {
		//ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	Direction.Normalize();
	return 0;
}

int CLine2D::SetDirection(double dAngle)
{
	Direction.x = cos(dAngle);
	Direction.y = sin(dAngle);
	m_bInValid = !Direction.IsValid();
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	return 0;
}


int CLine2D::SetDirection(double dVt2DX, double dVt2DY)
{
	Direction.x = dVt2DX;
	Direction.y = dVt2DY;
	m_bInValid = !Direction.IsValid();
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	Direction.Normalize();
	return 0;
}


CPoint2D CLine2D::GetPosition()
{
	return Position;
}

CVector2D CLine2D::GetDirection()
{
	return Direction;
}


int CLine2D::GetParallelPoints(CPoint2D* ppt2DSrc, 
							   double dAbsDis, 
							   CPoint2D* ppt2DDst0,
							   CPoint2D* ppt2DDst1)
{
	double dX, dY;
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	dX = dAbsDis * Direction.x;
	dY = dAbsDis * Direction.y;
	*ppt2DDst0 = CPoint2D(ppt2DSrc->x - dX, ppt2DSrc->y - dY);
	*ppt2DDst1 = CPoint2D(ppt2DSrc->x + dX, ppt2DSrc->y + dY);
	return 0;
}

int CLine2D::GetParallelPoint(CPoint2D* ppt2DSrc, 
							  double dAbsDis, 
							  int nXCheckFlag,
							  int nYCheckFlag,
							  CPoint2D* ppt2DDst)
{
	double dX, dY;
	CPoint2D pt2D0, pt2D1;
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	if(((nXCheckFlag == 0) && (nYCheckFlag != 0)) ||
		((nXCheckFlag != 0) && (nYCheckFlag == 0))) {
		dX = dAbsDis * Direction.x;
		dY = dAbsDis * Direction.y;
		pt2D0 = CPoint2D(ppt2DSrc->x - dX, ppt2DSrc->y - dY);
		pt2D1 = CPoint2D(ppt2DSrc->x + dX, ppt2DSrc->y + dY);
		if(nXCheckFlag != 0) {
			if(nXCheckFlag > 0) {
				if(pt2D0.x > pt2D1.x)
					*ppt2DDst = pt2D0;
				else
					*ppt2DDst = pt2D1;
			}
			else {
				if(pt2D0.x > pt2D1.x)
					*ppt2DDst = pt2D1;
				else
					*ppt2DDst = pt2D0;
			}
			return 0;
		}
		if(nYCheckFlag != 0) {
			if(nYCheckFlag > 0) {
				if(pt2D0.y > pt2D1.y)
					*ppt2DDst = pt2D0;
				else
					*ppt2DDst = pt2D1;
			}
			else {
				if(pt2D0.y > pt2D1.y)
					*ppt2DDst = pt2D1;
				else
					*ppt2DDst = pt2D0;
			}
			return 0;
		}
	}
	else {
		ASSERT(!"nXCheckFlag and nYCheckFlag are Invalid");
		return -1;
	}
	return -1;
}


int CLine2D::GetPerpendicularPoints(CPoint2D* ppt2DSrc, 
									double dAbsDis, 
									CPoint2D* ppt2DDst0,
									CPoint2D* ppt2DDst1)
{
	double dX, dY;
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	dX = -dAbsDis * Direction.y;
	dY = dAbsDis * Direction.x;
	*ppt2DDst0 = CPoint2D(ppt2DSrc->x - dX, ppt2DSrc->y - dY);
	*ppt2DDst1 = CPoint2D(ppt2DSrc->x + dX, ppt2DSrc->y + dY);
	return 0;
}

int CLine2D::GetPerpendicularPoint(CPoint2D* ppt2DSrc, 
								   double dAbsDis, 
								   int nXCheckFlag,
								   int nYCheckFlag,
								   CPoint2D* ppt2DDst)
{
	double dX, dY;
	CPoint2D pt2D0, pt2D1;
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	if(((nXCheckFlag == 0) && (nYCheckFlag != 0)) ||
		((nXCheckFlag != 0) && (nYCheckFlag == 0))) {
		dX = -dAbsDis * Direction.y;
		dY = dAbsDis * Direction.x;
		pt2D0 = CPoint2D(ppt2DSrc->x - dX, ppt2DSrc->y - dY);
		pt2D1 = CPoint2D(ppt2DSrc->x + dX, ppt2DSrc->y + dY);
		if(nXCheckFlag != 0) {
			if(nXCheckFlag > 0) {
				if(pt2D0.x > pt2D1.x)
					*ppt2DDst = pt2D0;
				else
					*ppt2DDst = pt2D1;
			}
			else {
				if(pt2D0.x > pt2D1.x)
					*ppt2DDst = pt2D1;
				else
					*ppt2DDst = pt2D0;
			}
			return 0;
		}
		if(nYCheckFlag != 0) {
			if(nYCheckFlag > 0) {
				if(pt2D0.y > pt2D1.y)
					*ppt2DDst = pt2D0;
				else
					*ppt2DDst = pt2D1;
			}
			else {
				if(pt2D0.y > pt2D1.y)
					*ppt2DDst = pt2D1;
				else
					*ppt2DDst = pt2D0;
			}
			return 0;
		}
	}
	else {
		ASSERT(!"nXCheckFlag and nYCheckFlag are Invalid");
		return -1;
	}
	return -1;
}



int  CLine2D::GetDiagonalPoints(CPoint2D* ppt2DSrc, 
								double dAbsParallelDis, 
								double dAbsPerpendicularDis, 
								CPoint2D* ppt2DParallelPair0Pt0,
								CPoint2D* ppt2DParallelPair0Pt1,
								CPoint2D* ppt2DParallelPair1Pt0,
								CPoint2D* ppt2DParallelPair1Pt1)
{
	double dParX, dParY;
	double dPerX, dPerY;
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	dParX = dAbsParallelDis * Direction.x;
	dParY = dAbsParallelDis * Direction.y;
	dPerX = -dAbsPerpendicularDis * Direction.y;
	dPerY = dAbsPerpendicularDis * Direction.x;
	*ppt2DParallelPair0Pt0 = CPoint2D(ppt2DSrc->x - dParX - dPerX, ppt2DSrc->y - dParY - dPerY);
	*ppt2DParallelPair0Pt1 = CPoint2D(ppt2DSrc->x + dParX - dPerX, ppt2DSrc->y + dParY - dPerY);
	*ppt2DParallelPair1Pt0 = CPoint2D(ppt2DSrc->x - dParX + dPerX, ppt2DSrc->y - dParY + dPerY);
	*ppt2DParallelPair1Pt1 = CPoint2D(ppt2DSrc->x + dParX + dPerX, ppt2DSrc->y + dParY + dPerY);
	return 0;
}


void CLine2D::Reset()
{
	m_bInValid = TRUE;
}

BOOL CLine2D::IsInValid()
{
	return m_bInValid;
}

int CLine2D::IsOnLine(CPoint2D* ppt2DSrc,
					  BOOL* pbOnLine)
{
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	*pbOnLine = (Direction.y * (ppt2DSrc->x - Position.x)- Direction.x * (ppt2DSrc->y - Position.y) == 0);
	return 0;
}

int CLine2D::GetDistance(CPoint2D* ppt2DSrc,
						 double* pdDis)
{
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	*pdDis = fabs(ppt2DSrc->x * Direction.y - ppt2DSrc->y * Direction.x - Position.x * Direction.y + Position.y * Direction.x);
	return 0;
}

int CLine2D::GetDistance(CPoint* pptSrc,
						 double* pdDis)
{
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	*pdDis = fabs(pptSrc->x * Direction.y - pptSrc->y * Direction.x - Position.x * Direction.y + Position.y * Direction.x);
	return 0;
}

int CLine2D::GetSignedDistance(CPoint2D* ppt2DSrc, 
							   double* pdDis)
{
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	if(fabs(Direction.x) > fabs(Direction.y)) {
		if(Direction.x > 0) {
			Direction.x = -Direction.x;
			Direction.y = -Direction.y;
		}
	}
	else {
		if(Direction.y < 0) {
			Direction.x = -Direction.x;
			Direction.y = -Direction.y;
		}
	}
	*pdDis = (ppt2DSrc->x * Direction.y - ppt2DSrc->y * Direction.x - Position.x * Direction.y + Position.y * Direction.x);
	return 0;
}

int CLine2D::GetSignedDistance(CPoint* pptSrc, 
							   double* pdDis)
{
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	if(fabs(Direction.x) > fabs(Direction.y)) {
		if(Direction.x > 0) {
			Direction.x = -Direction.x;
			Direction.y = -Direction.y;
		}
	}
	else {
		if(Direction.y < 0) {
			Direction.x = -Direction.x;
			Direction.y = -Direction.y;
		}
	}
	*pdDis = (pptSrc->x * Direction.y - pptSrc->y * Direction.x - Position.x * Direction.y + Position.y * Direction.x);
	return 0;
}

int CLine2D::GetDistanceX(CPoint2D* ppt2DSrc, 
						  double* pdDis)
{
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	*pdDis = fabs(ppt2DSrc->x * Direction.y - ppt2DSrc->y * Direction.x - Position.x * Direction.y + Position.y * Direction.x) * Direction.y;	 
	return 0;
}

int CLine2D::GetDistanceY(CPoint2D* ppt2DSrc, 
						  double* pdDis)
{
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	*pdDis = fabs(ppt2DSrc->x * Direction.y - ppt2DSrc->y * Direction.x - Position.x * Direction.y + Position.y * Direction.x) * Direction.x;	 
	return 0;
}

int CLine2D::GetProjectionX(CPoint2D* pptSrc, 
							CPoint2D* pptProj)
{
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	if (Direction.x==0)	{
		*pptProj = CPoint2D(pptSrc->x, 1.0E+50);
		return -1;
	}
	else 
		*pptProj = CPoint2D(pptSrc->x, Position.y + (pptSrc->x - Position.x) * Direction.y / Direction.x);	 
	return 0;
}

int CLine2D::GetProjectionY(CPoint2D* pptSrc, 
							CPoint2D* pptProj)
{
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	if (Direction.y==0) {
		*pptProj = CPoint2D(1.0E+50, pptSrc->y);
		return -1;
	}
	else 
		*pptProj = CPoint2D(Position.x + (pptSrc->y - Position.y) * Direction.x / Direction.y, pptSrc->y);
	return 0;
}

int CLine2D::GetProjection(CPoint2D* ppt2dSrc, 
						   CPoint2D* ppt2DProj)
{
	CLine2D ln2DPerpendicular;
	int nError;
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	GetPerpendicularLine(ppt2dSrc,
						 &ln2DPerpendicular);
	nError = GetIntercept(&ln2DPerpendicular, 
						  ppt2DProj);
	if(nError)
		goto ErrorAbort;
	return 0;
ErrorAbort:
	return -1;
}

int CLine2D::IsOnSameSide(CPoint2D* pptSrc1,
						  CPoint2D* pptSrc2, 
						  BOOL* pbOnSameSide)
{
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	*pbOnSameSide = ((Direction.y * (pptSrc1->x - Position.x)- Direction.x * (pptSrc1->y - Position.y)) *
					(Direction.y * (pptSrc2->x - Position.x)- Direction.x * (pptSrc2->y - Position.y)) > 0);
	return 0;
}

int CLine2D::GetBestFittingLine(CPoint2D* ppt2D,
								int nCount)
{
	int i;
	double sum_x,sum_y,sum_xx,sum_yy,sum_xy, x, y, dAbs;
	double dSigmaX, dSigmaY;

	m_bInValid = TRUE;
	sum_x=sum_y=sum_xx=sum_yy=sum_xy=0;
	if (nCount<=1) {
		ASSERT(!"nCount is less than or equal 1");
		return -1;
	}
	for (i=0;i<nCount;i++) {
		x = ppt2D[i].x;
		y = ppt2D[i].y;
		sum_x		=  sum_x    + x;
		sum_xx      =  sum_xx   + x*x;
		sum_y       =  sum_y    + y;
		sum_yy      =  sum_yy   + y*y;
		sum_xy      =  sum_xy   + x*y;
	}
	sum_x=sum_x/nCount;
	sum_y=sum_y/nCount;
	sum_xx=sum_xx/nCount;
	sum_yy=sum_yy/nCount;
	sum_xy=sum_xy/nCount;
	Position.x=sum_x;
	Position.y=sum_y;
	dSigmaX = sum_xx - sum_x * sum_x;
	dSigmaY = sum_yy - sum_y * sum_y;
	if (dSigmaX > dSigmaY) {
		Direction.y=(sum_xy-sum_x*sum_y);
		Direction.x=(sum_xx-sum_x*sum_x);
	}
	else {
		Direction.x=(sum_xy-sum_y*sum_x);
		Direction.y=(sum_yy-sum_y*sum_y);
	}
	dAbs = sqrt(Direction.x*Direction.x + Direction.y*Direction.y);
	if(dAbs == 0) {
		return -1;
	}
	else {
		Direction.x /= dAbs;
		Direction.y /= dAbs;
		m_bInValid = FALSE;
		return 0;
	}
}


int CLine2D::GetBestFittingLine(CPoint* ppt,
								int nCount)
{
	int i;
	double sum_x,sum_y,sum_xx,sum_yy,sum_xy, x, y, dAbs;
	double dSigmaX, dSigmaY;

	m_bInValid = TRUE;
	sum_x=sum_y=sum_xx=sum_yy=sum_xy=0;
	if (nCount<=1) {
		ASSERT(!"nCount is less than or equal 1");
		return -1;
	}
	for (i=0;i<nCount;i++) {
		x = ppt[i].x;
		y = ppt[i].y;
		sum_x		=  sum_x    + x;
		sum_xx      =  sum_xx   + x*x;
		sum_y       =  sum_y    + y;
		sum_yy      =  sum_yy   + y*y;
		sum_xy      =  sum_xy   + x*y;
	}
	sum_x=sum_x/nCount;
	sum_y=sum_y/nCount;
	sum_xx=sum_xx/nCount;
	sum_yy=sum_yy/nCount;
	sum_xy=sum_xy/nCount;
	Position.x=sum_x;
	Position.y=sum_y;
	dSigmaX = sum_xx - sum_x * sum_x;
	dSigmaY = sum_yy - sum_y * sum_y;
	if (dSigmaX > dSigmaY) {
		Direction.y=(sum_xy-sum_x*sum_y);
		Direction.x=(sum_xx-sum_x*sum_x);
	}
	else {
		Direction.x=(sum_xy-sum_y*sum_x);
		Direction.y=(sum_yy-sum_y*sum_y);
	}
	dAbs = sqrt(Direction.x*Direction.x + Direction.y*Direction.y);
	if(dAbs == 0) {
		return -1;
	}
	else {
		Direction.x /= dAbs;
		Direction.y /= dAbs;
		m_bInValid = FALSE;
		return 0;
	}
}

int CLine2D::GetIntercept(CLine2D* pln2D, 
						  CPoint2D* ppt2DIntercept)
{
	double lambda, d;
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	if(pln2D->m_bInValid) {
		ASSERT(!"pln2D is Invalid");
		return -1;
	}
	d = Direction.x*pln2D->Direction.y-pln2D->Direction.x*Direction.y;
	if( d == 0) {
		if(Direction.x == 0) {
			*ppt2DIntercept = CPoint2D((Position.x + pln2D->Position.x) / 2, 1.0E+50);
			return -1;
		}
		if(Direction.y == 0) {
			*ppt2DIntercept = CPoint2D(1.0E+50, (Position.y + pln2D->Position.y) / 2);
			return -1;
		}
		*ppt2DIntercept = CPoint2D(1.0E+50, 1.0E+50);
		return -1;
	}
	else {
		lambda=(Direction.y*(pln2D->Position.x-Position.x)-Direction.x*(pln2D->Position.y-Position.y))/d;
		*ppt2DIntercept = CPoint2D(lambda * pln2D->Direction.x + pln2D->Position.x, lambda * pln2D->Direction.y + pln2D->Position.y);
	}
	return 0;
}

int CLine2D::GetAngleDeg(double& dAngleDeg)
{
	int nError;
	nError = GetAngleRad(dAngleDeg);
	dAngleDeg = dAngleDeg * 180 / PI;
	return nError;
}

int CLine2D::GetAngleRad(double& dAngleRad)
{
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	dAngleRad = atan2(Direction.y, Direction.x);
	if(dAngleRad < (-PI / 2)) dAngleRad += PI;
	if(dAngleRad >= (PI / 2)) dAngleRad -= PI;
	return 0;
}

int CLine2D::GetX(double dY,
				  double* dX)
{
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	if (Direction.y == 0) {
		*dX = 1.0E+50;
		return -1;
	}
	else 
		*dX = Position.x + (dY - Position.y)*Direction.x/Direction.y;
	return 0;
}

int CLine2D::GetY(double dX, 
				  double* dY)
{
	if(m_bInValid) {
//		ASSERT(!"CLine2D is Invalid");
		return -1;
	}
	if (Direction.x == 0) {
		*dY = 1.0E+50;
		return -1;
	}
	else 
		*dY = Position.y + (dX - Position.x)*Direction.y/Direction.x;
	return 0;
}

int CLine2D::GetBiSectLine(CLine2D *pln2DLine1, 
						   CLine2D *pln2DLine2) 
{
	CPoint2D pt2D;
	CLine2D lnPerpendicular;
	double dAngle1;
	double dAngle2;
	double dAngleDif;
	double dAnglePerpendicular;
	double dAngle;
	CPoint2D pt2DIntercept;
	m_bInValid = TRUE;
	if(pln2DLine1->m_bInValid) {
//		ASSERT(!"pln2DLine1 is Invalid");
		return -1;
	}
	if(pln2DLine2->m_bInValid) {
//		ASSERT(!"pln2DLine2 is Invalid");
		return -1;
	}
	pln2DLine1->GetAngleRad(dAngle1);
	pln2DLine2->GetAngleRad(dAngle2);
	dAngleDif = fabs(dAngle2 - dAngle1);
	dAngle = (dAngle2 + dAngle1)/2;
	if(dAngleDif > PI/2) {
		if(dAngle > 0)
			dAngle = dAngle - PI/2;
		else
			dAngle = dAngle + PI/2;
	}
	dAnglePerpendicular = PI/2 + dAngle;
	if(dAnglePerpendicular > PI/2)
		dAnglePerpendicular = dAnglePerpendicular - PI;
	lnPerpendicular.Direction = CPoint2D(cos(dAnglePerpendicular),sin(dAnglePerpendicular));
	lnPerpendicular.Position = pln2DLine1->Position;
	lnPerpendicular.m_bInValid = FALSE;
	lnPerpendicular.GetIntercept(pln2DLine2,&pt2DIntercept);
	Position = CPoint2D((lnPerpendicular.Position.x + pt2DIntercept.x)/2,(lnPerpendicular.Position.y + pt2DIntercept.y)/2);
	Direction = CVector2D(cos(dAngle),sin(dAngle));
	m_bInValid = FALSE;
	return 0;
}





int CLine2D::GetAverageLine(CLine2D *pln2D, 
							int nln2D) 
{
	int i, j;
	CPoint2D pt2D;
	CLine2D lnPerpendicular;
	double* pdAngle;
	double dAngleDifOrg;
	double dAngleDif;
	double dMaxAngleDif;
	double dAnglePerpendicular;
	double dAngleAvg, dAngleCenter;
	double dAngleMax, dAngleMin;
	double dAngleTemp;
	double dDoubleCompareAccuracy;
	int nAvg;
	int nError;
	CPoint2D pt2DIntercept;
	m_bInValid = TRUE;
	dDoubleCompareAccuracy = 0.0000001;
	pdAngle = new double [abs(nln2D)];
	for(i = 0; i < nln2D; i++) {
		if(pln2D[i].m_bInValid) {
			ASSERT(!"pln2D[i] is Invalid");
			delete [] pdAngle;
			return -1;
		}
		pln2D[i].GetAngleRad(pdAngle[i]);
	}
	dMaxAngleDif = 0;
	for( i = 0; i < nln2D; i++) {
		for( j = i + 1; j < nln2D; j++) {
			dAngleDifOrg = fabs(pdAngle[i] - pdAngle[j]);
			if(dAngleDifOrg > PI/2)
				dAngleDif = PI - dAngleDifOrg;
			else 
				dAngleDif = dAngleDifOrg;
			if(dMaxAngleDif < dAngleDif) {
				dMaxAngleDif = dAngleDif;
				dAngleCenter = (pdAngle[i] + pdAngle[j])/2;
				if(dAngleDifOrg > PI/2) {
					if(dAngleCenter > 0)
						dAngleCenter -= PI/2;
					else
						dAngleCenter += PI/2;
				}
			}
		}
	}

	dAngleMax = dAngleCenter + dMaxAngleDif/2 + dDoubleCompareAccuracy;
	dAngleMin = dAngleCenter - dMaxAngleDif/2 - dDoubleCompareAccuracy;
	dAngleAvg = 0;
	for( i = 0; i < nln2D; i++) {
		dAngleTemp = pdAngle[i] + PI;
		if((dAngleTemp >= dAngleMin) && (dAngleTemp <= dAngleMax))
			dAngleAvg += dAngleTemp;
		else {
			dAngleTemp = pdAngle[i] - PI;
			if((dAngleTemp >= dAngleMin) && (dAngleTemp <= dAngleMax))
				dAngleAvg += dAngleTemp;
			else {
				dAngleAvg += pdAngle[i];
			}
		}
	}
	dAngleAvg = dAngleAvg /  nln2D;
	if(dAngleAvg > PI /2)
		dAngleAvg = dAngleAvg - PI;
	if(dAngleAvg < (-PI/2))
		dAngleAvg = dAngleAvg + PI;
	dAnglePerpendicular = PI/2 + dAngleAvg;
	if(dAnglePerpendicular > PI/2)
		dAnglePerpendicular = dAnglePerpendicular - PI;
	lnPerpendicular.Direction = CPoint2D(cos(dAnglePerpendicular),sin(dAnglePerpendicular));
	lnPerpendicular.Position = pln2D[0].Position;
	lnPerpendicular.m_bInValid = FALSE;
	nAvg = 0;
	Position = CPoint2D(0,0);
	for( i = 0; i < nln2D; i++) {
		nError = lnPerpendicular.GetIntercept(&pln2D[i],&pt2DIntercept);
		if(nError == 0) {
			Position = CPoint2D(Position.x + pt2DIntercept.x, Position.y + pt2DIntercept.y);
			nAvg++;
		}
	}
	if(nAvg == 0) {
		delete [] pdAngle;
		return -1;
	}
	Position = CPoint2D(Position.x / nAvg, Position.y / nAvg);
	Direction = CVector2D(cos(dAngleAvg),sin(dAngleAvg));
	m_bInValid = FALSE;
	delete [] pdAngle;
	return 0;
}


int CLine2D::GetPerpendicularLine(CPoint2D* ppt2D,
								  CLine2D* pln2DLine)
{
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		pln2DLine->m_bInValid = TRUE;
		return -1;
	}
	pln2DLine->Direction = CVector2D(-Direction.y, Direction.x);
	pln2DLine->Position = *ppt2D;
	pln2DLine->m_bInValid = FALSE;
	return 0;
}

int CLine2D::GetParallelLine(CPoint2D* ppt2D, 
							 CLine2D* pln2DLine) 
{
	if(m_bInValid) {
		ASSERT(!"CLine2D is Invalid");
		pln2DLine->m_bInValid = TRUE;
		return -1;
	}
	pln2DLine->Direction = Direction;
	pln2DLine->Position = *ppt2D;
	pln2DLine->m_bInValid = FALSE;
	return 0;
}

int IsCollinear(CPoint2D* ppt2d, int nSize, double dDelta, BOOL* pbCollinear)
{
	double	dDet, dX, dY;
	CLine2D ln2DBestFitting;
	int i, nError;

	if(nSize<3) {
		ASSERT(!"Number of point is Invalid");
		return -1;
	}
	nError = ln2DBestFitting.GetBestFittingLine(ppt2d, nSize);
	if(nError <0) {
		*pbCollinear = TRUE;
		return 0;
	}
	else {
		dDet = 0;
		if(fabs(ln2DBestFitting.GetDirection().x) > fabs(ln2DBestFitting.GetDirection().y)) {
			for(i=0; i < nSize; i++) {
				ln2DBestFitting.GetY(ppt2d[i].x, &dY);	
				dDet = dDet + fabs(ppt2d[i].y - dY);
			}
		}
		else {
			for(i=0; i < nSize; i++) {
				ln2DBestFitting.GetX(ppt2d[i].y, &dX);	
				dDet = dDet + fabs(ppt2d[i].x - dX);
			}
		}
	}
	dDet = dDet / nSize;
	if (dDet > dDelta)
		*pbCollinear = FALSE;
	else *pbCollinear = TRUE;
	return 0;
}

CPointPatternCorrelation::CPointPatternCorrelation()
{

}

int CPointPatternCorrelation::GetLUTSize(int nTeachPatternNo,
									int nSearchAngle,
									double dAngleStep,
									long& lLUTSize)
{
	if(dAngleStep == 0) {
		ASSERT(!"dAngleStep is 0");
		return -1;
	}
	lLUTSize = (long)(nTeachPatternNo*(2*nSearchAngle/dAngleStep+1)/dAngleStep);
	return 0;
}

int CPointPatternCorrelation::InitLUT(CPoint *pptTeachPattern,
								int nTeachPatternNo,
								int nSearchAngle,
								double dAngleStep,
								int *pnLUTX,
								int *pnLUTY)
{
	int i,j;
	double angle;
	long index;
	int nAngleRange;
	
	if(dAngleStep == 0) {
		ASSERT(!"dAngleStep is 0");
		return -1;
	}
	nAngleRange=(int)(2*nSearchAngle/dAngleStep+1);
	for(i=0;i<nAngleRange;i++)	{
		angle=(i*dAngleStep-nSearchAngle)*PI/180;
		index=i*nTeachPatternNo;
		for(j=0;j<nTeachPatternNo;j++) {
			pnLUTX[index+j]=(int)(pptTeachPattern[j].x*cos(angle)+pptTeachPattern[j].y*sin(angle));
			pnLUTY[index+j]=(int)(-pptTeachPattern[j].x*sin(angle)+pptTeachPattern[j].y*cos(angle));
		}
	}
	return 0;
}

int CPointPatternCorrelation::FindPatternPos(CPoint *pptTeachPattern,
									   int nTeachPatternNo,
									   CPoint *pptInspectPattern,
									   int nInspectPatternNo,
									   int nSearchAngle,
									   double dAngleStep,
									   int nXShift,
									   int nYShift,
									   int nCenterUncertainty,
									   int *pnLUTX,
									   int *pnLUTY,
									   double &dFndAngle,
									   double &dFndXShift,
									   double &dFndYShift)
{
	long i,j,m,n,k;
	long MappingSize;
	BYTE *Mapping;
	long sumAngle;
	long sumShiftX;
	long sumShiftY;
	double sumNo;
	int minX,maxX,minY,maxY,minA,maxA;
	int MappingMax;
	int MaxPos;
	int pointShiftX,pointShiftY;
	int convolutionXShift,convolutionYShift;
	long index0,index,TableIndex;
	int nAngleRange;

	nAngleRange=(int)(2*nSearchAngle/dAngleStep+1);

	MappingSize=nAngleRange*nYShift*nXShift;
	Mapping=new BYTE[abs(MappingSize)];

	for(i=0;i<MappingSize;i++) {
		Mapping[i]=0;
	}

	for(k=0;k<nAngleRange;k++) {
		TableIndex=k*nTeachPatternNo;
		index0=k*nYShift*nXShift;
		for(i=0;i<nTeachPatternNo;i++) {
			for(j=0;j<nInspectPatternNo;j++) {
				pointShiftX=pptInspectPattern[j].x-pnLUTX[TableIndex+i]+nXShift/2;
				pointShiftY=pptInspectPattern[j].y-pnLUTY[TableIndex+i]+nYShift/2;
				if((pointShiftX<nXShift)&&(pointShiftY<nYShift)&&(pointShiftX>0)&&(pointShiftY>0)) {
					for(m=-nCenterUncertainty;m<=nCenterUncertainty;m++) {
						convolutionYShift=pointShiftY+m;
						if((convolutionYShift>=0)&&(convolutionYShift<nYShift)) {
							index=index0+convolutionYShift*nXShift;
							for(n=-nCenterUncertainty;n<=nCenterUncertainty;n++) {
								convolutionXShift=pointShiftX+n;
								if((convolutionXShift>=0)&&(convolutionXShift<nXShift)) {
									Mapping[index+convolutionXShift]=Mapping[index+convolutionXShift]+1;
								}
							}
						}
					}
				}
			}
		}
	}

	MappingMax=0;
	MaxPos=0;
	for(i=0;i<MappingSize;i++) {
		if(Mapping[i]>MappingMax) {
			MappingMax=Mapping[i];
		}
	}

	sumNo=0;
	sumAngle=0;
	sumShiftX=0;
	sumShiftY=0;
	minX=minY=minA=2000;
	maxX=maxY=maxA=0;
	for(k=0;k<nAngleRange;k++) {
		index0=k*nYShift*nXShift;
		for(i=0;i<nYShift;i++) {
			index=index0+i*nXShift;
			for(j=0;j<nXShift;j++) {
				if(Mapping[index+j]==MappingMax) {
					sumAngle=sumAngle+k;
					sumShiftX=sumShiftX+j;
					sumShiftY=sumShiftY+i;
					if(minX>j) minX=j;
					if(minY>i) minY=i;
					if(minA>k) minA=k;
					if(maxX<j) maxX=j;
					if(maxY<i) maxY=i;
					if(maxA<k) maxA=k;
					sumNo=sumNo+1;
				}
			}
		}
	}
	delete [] Mapping;
	if(sumNo>0) {
		dFndAngle=(sumAngle/sumNo*dAngleStep-nSearchAngle)*PI/180;
		dFndXShift=sumShiftX/sumNo-nXShift/2;
		dFndYShift=sumShiftY/sumNo-nYShift/2;
		return 0;
	}
	else
		return -1;
}



int CPointPatternCorrelation::FindPattern1D(double *pdTeachPos,
											int nTeachPosNo,
											double *pdInspPos,
											int nInspPosNo,
											double dTeachNInspOffsetNominal,
											double dTeachNInspOffsetTol,
											double dPosVarTol,
											int *pnMatchingPosIndex,
											int &nMatchingPosNo,
											int *pnNotMatchingPosIndex,
											int &nNotMatchingPosNo,
											double *pdCorrPos,
											BOOL *pbCorrStatus,
											bool bAccurate)
{
	int i, j;
	long *plTeach, *plInsp;
	long *plDis;
	int nDisNo;
	int nTimes;
	CAdvHistogram AdvHistogram;
	int *pnSortIndex;
	
	bool *pbTeachNotMatching;
	bool *pbInspNotMatching;
	int nMax;
	double dDisMin;
	int nMinPos;
	int nDifNo;
	long lOffset;
	long lTeachPos;
	long lDis;
	long lDif;
	long lTeachNInspDifMin;
	long lTeachNInspDifMax;

	if(nTeachPosNo <= 0)
		return -1;
	if(nInspPosNo <= 0)
		return -1;

	if(dPosVarTol < 0.01) {
		ASSERT(!"dPosVarTol is less than 0.01");
		return -1;
	}
	nTimes =1;
	while(dPosVarTol < 5) {
		nTimes *= 10;
		dPosVarTol *= 10;
		if(nTimes > 100) {
			nTimes = 100;
			if(dPosVarTol < 1)
				dPosVarTol = 1.0;
			break;
		}
	};

	lTeachNInspDifMin = Round((dTeachNInspOffsetNominal - dTeachNInspOffsetTol)*nTimes);
	lTeachNInspDifMax = Round((dTeachNInspOffsetNominal + dTeachNInspOffsetTol)*nTimes);

	nDisNo = nTeachPosNo * nInspPosNo;
	plTeach = new long[abs(nTeachPosNo)];
	plInsp = new long[abs(nInspPosNo)];
	plDis = new long[abs(nDisNo)];
	pbTeachNotMatching = new bool [abs(nTeachPosNo)];
	pbInspNotMatching = new bool [abs(nInspPosNo)];
	pnSortIndex = new int[abs(nInspPosNo)];
	for(i = 0; i < nTeachPosNo; i++) {
		plTeach[i] = (long)(pdTeachPos[i] * nTimes);
		pbTeachNotMatching[i] = true;
	}
	for(i = 0; i < nInspPosNo; i++) {
		plInsp[i] = (long)(pdInspPos[i] * nTimes);
		pbInspNotMatching[i] = true;
	}
	nDifNo = 0;
	for(i = 0; i < nTeachPosNo; i++) {
		for(j = 0; j < nInspPosNo; j++) {
			lDif = plTeach[i] - plInsp[j];
			if((lDif>=lTeachNInspDifMin) && (lDif<=lTeachNInspDifMax)) {
				plDis[nDifNo++] = lDif;
			}
		}
	}

	AdvHistogram.GetMaxConvHist1D(plDis, 
								  nDifNo,
								  (int)dPosVarTol,
								  nMax,
								  lOffset);
	nMatchingPosNo = 0;
	if(!bAccurate) {
		for(i = 0; i < nTeachPosNo; i++) {
			lTeachPos = plTeach[i] - lOffset;
			for(j = 0; j < nInspPosNo; j++) {
				if(pbInspNotMatching[j])	{
					lDis = abs(plInsp[j] - lTeachPos);
					if(lDis<dPosVarTol) {
						pbTeachNotMatching[i] = false;
						pbInspNotMatching[j] = false;
						pnMatchingPosIndex[nMatchingPosNo++] = j;
						pdCorrPos[i] = pdInspPos[j];
						pbCorrStatus[i] = TRUE;
						break;
					}
				}
			}
			if(pbTeachNotMatching[i]) {
				pdCorrPos[i] = pdTeachPos[i] - (double)lOffset/nTimes;
				pbCorrStatus[i] = FALSE;
			}
		}
	}
	else {
		for(i = 0; i < nTeachPosNo; i++) {
			lTeachPos = plTeach[i] + lOffset;
			dDisMin = abs(plInsp[0] - lTeachPos);
			nMinPos = i;
 			for(j = 0; j < nInspPosNo; j++) {
				if(pbInspNotMatching[j])	{
					lDis = abs(plInsp[j] - lTeachPos);
					if(dDisMin > lDis) {
						dDisMin = lDis;
						nMinPos = j;
					}
				}
			}
			if(dDisMin<dPosVarTol) {
				pbTeachNotMatching[i] = false;
				pbInspNotMatching[nMinPos] = false;
				pnMatchingPosIndex[nMatchingPosNo++] = nMinPos;
				pdCorrPos[i] = pdInspPos[nMinPos];
				pbCorrStatus[i] = TRUE;
			}
			else {
				pdCorrPos[i] = pdTeachPos[i] - (double)lOffset/nTimes;
				pbCorrStatus[i] = FALSE;
			}
		}
	}
	nNotMatchingPosNo = 0;
	for(i = 0; i < nInspPosNo; i++) {
		if(pbInspNotMatching[i]) 
			pnNotMatchingPosIndex[nNotMatchingPosNo++] = i;
	}

	delete [] plTeach;
	delete [] plInsp;
	delete [] plDis;
	delete [] pbTeachNotMatching;
	delete [] pbInspNotMatching;
	delete [] pnSortIndex;
	return 0;
}


int CPointPatternCorrelation::FindPeriodicPattern1D(double *pdTeachPos,
													int nTeachPosNo,
													double dTeachPitch,
													double *pdInspPos,
													int nInspPosNo,
													double dPosVarTol,
													int *pnMatchingTeachPosIndex,
													int *pnMatchingInspPosIndex,
													int &nMatchingPosNo,
													int *pnNotMatchingInspPosIndex,
													int &nNotMatchingInspPosNo,
													double *pdMissingInspPos,
													int &nMissingInspPosNo,
													bool bAccurate)
{
	int i, j;
	long *plTeach, *plInsp;
	long *plDis;
	long *plPitch;
	int nDisNo;
	int nIndex;
	int nTimes;
	CAdvHistogram AdvHistogram;
	int *pnSortIndex;
	bool *pbTeackNotMatching;
	bool *pbInspNotMatching;
	int nMax;
	double dDisMin;
	int nMinPos;
	long lOffset;
	long lTeachPos;
	long lDis;
	bool bAscend = 1;
	int nPitchNo;
	double dZoom;
	long lBestPitch;
	double dBestPitch;
	long lMinPitch;
	long lMaxPitch;
	long lSum;
	int nPosVarTol;
	int nBestPitchNo;

	if(dPosVarTol < 0.01) {
		ASSERT(!"dPosVarTol is less than 0.01");
		return -1;
	}
	nTimes =1;
	while(dPosVarTol < 5) {
		nTimes *= 10;
		dPosVarTol *= 10;
		if(nTimes > 100) {
			nTimes = 100;
			if(dPosVarTol < 1)
				dPosVarTol = 1.0;
			break;
		}
	};

	nDisNo = nTeachPosNo * nInspPosNo;
	plTeach = new long[abs(nTeachPosNo)];
	plInsp = new long[abs(nInspPosNo)];
	plDis = new long[abs(nDisNo)];
	pbTeackNotMatching = new bool [abs(nTeachPosNo)];
	pbInspNotMatching = new bool [abs(nInspPosNo)];
	pnSortIndex = new int[abs(nInspPosNo)];
	plPitch = new long[abs(2 * nInspPosNo)];
	for(i = 0; i < nTeachPosNo; i++) {
		plTeach[i] = (long)(pdTeachPos[i] * nTimes);
		pbTeackNotMatching[i] = true;
	}
	for(i = 0; i < nInspPosNo; i++) {
		plInsp[i] = (long)(pdInspPos[i] * nTimes);
		pbInspNotMatching[i] = true;
	}


	nPosVarTol = (int)dPosVarTol;
	Sorting(plInsp, nInspPosNo, bAscend);

	nIndex = 0;
	for(i = 1; i < (nInspPosNo - 1); i++) {
		plPitch[nIndex++] = plInsp[i] - plInsp[i - 1];
		plPitch[nIndex++] = plInsp[i + 1] - plInsp[i];
	}
	nPitchNo = nIndex;
	AdvHistogram.GetMaxConvHist1D(plPitch, 
								  nPitchNo,
								  nPosVarTol,
								  nMax,
								  lBestPitch);

	lMinPitch = lBestPitch - nPosVarTol;
	lMaxPitch = lBestPitch + nPosVarTol;

	nBestPitchNo = 0;
	lSum = 0;
	for(i = 0; i < nPitchNo; i++) {
		if((plPitch[i] > lMinPitch) && (plPitch[i] < lMaxPitch)) {
			lSum += plPitch[i];
			nBestPitchNo++;
		}
	}
	if(nBestPitchNo)
		dBestPitch = (double)lSum / nBestPitchNo;
	else
		dBestPitch = lBestPitch;

	dZoom = (dTeachPitch * nTimes * nTimes) / dBestPitch;
	for(i = 0; i < nInspPosNo; i++) 
		plInsp[i] = (long)(pdInspPos[i] * dZoom);

	nIndex = 0;
	for(i = 0; i < nTeachPosNo; i++)
		for(j = 0; j < nInspPosNo; j++)
			plDis[nIndex++] = plInsp[j] - plTeach[i];

	AdvHistogram.GetMaxConvHist1D(plDis, 
								  nDisNo,
								  (int)dPosVarTol,
								  nMax,
								  lOffset);
	nMatchingPosNo = 0;
	nMissingInspPosNo = 0;
	if(!bAccurate) {
		for(i = 0; i < nTeachPosNo; i++) {
			lTeachPos = plTeach[i] + lOffset;
			for(j = 0; j < nInspPosNo; j++) {
				if(pbInspNotMatching[j])	{
					lDis = plInsp[j] - lTeachPos;
					if(lDis > dPosVarTol)
						break;
					if(abs(lDis)<dPosVarTol) {
						pbTeackNotMatching[i] = false;
						pbInspNotMatching[j] = false;
						pnMatchingTeachPosIndex[nMatchingPosNo] = i;
						pnMatchingInspPosIndex[nMatchingPosNo] = j;
						nMatchingPosNo++;
						break;
					}
				}
			}
			if(pbTeackNotMatching[i])
				pdMissingInspPos[nMissingInspPosNo++] = pdTeachPos[i] + ((double)lOffset)/nTimes;
		}
	}
	else {
		for(i = 0; i < nTeachPosNo; i++) {
			lTeachPos = plTeach[i] + lOffset;
			dDisMin = abs(plInsp[0] - lTeachPos);
			nMinPos = i;
 			for(j = 0; j < nInspPosNo; j++) {
				if(pbInspNotMatching[j])	{
					lDis = plInsp[j] - lTeachPos;
					if(lDis > dPosVarTol)
						break;
					lDis = abs(lDis);
					if(dDisMin > lDis) {
						dDisMin = lDis;
						nMinPos = j;
					}
				}
			}
			if(dDisMin<dPosVarTol) {
				pbTeackNotMatching[i] = false;
				pbInspNotMatching[nMinPos] = false;
				pnMatchingTeachPosIndex[nMatchingPosNo] = i;
				pnMatchingInspPosIndex[nMatchingPosNo] = nMinPos;
				nMatchingPosNo++;
			}
			else
				pdMissingInspPos[nMissingInspPosNo++] = pdTeachPos[i] + ((double)lOffset)/nTimes;
		}
	}
	nNotMatchingInspPosNo = 0;
	for(i = 0; i < nInspPosNo; i++) {
		if(pbInspNotMatching[i]) 
			pnNotMatchingInspPosIndex[nNotMatchingInspPosNo++] = i;
	}

	delete [] plPitch;
	delete [] plTeach;
	delete [] plInsp;
	delete [] plDis;
	delete [] pbTeackNotMatching;
	delete [] pbInspNotMatching;
	delete [] pnSortIndex;
	return 0;
}

void CPointPatternCorrelation::PointSorting(CPoint *pptTeachPattern,
								 int nTeachPatternNo,
								 CPoint *pptInspectPattern,
								 BOOL *pbInspectPattern,
								 int nInspectPatternNo,
								 int nCenterUncertainty,
								 double &dFndAngle,
								 double &dFndXShift,
								 double &dFndYShift)
{
	CPoint EstimatedBallCenter;
	int i,j;
	int offsetXY;
	bool pbBallChecked[2000];
	int sumDeltaX,sumDeltaY,sumNo;
	int nCenterUncertaintyDiameter;
	nCenterUncertaintyDiameter=2*nCenterUncertainty+1;
	CPoint *pInspectionPatternTemp;
	pInspectionPatternTemp=new CPoint[abs(nTeachPatternNo)];

	sumDeltaX=0;
	sumDeltaY=0;
	sumNo=0;
	for(i=0;i<nInspectPatternNo;i++) {
		pbBallChecked[i]=FALSE;
	}

	for(i=0;i<nTeachPatternNo;i++) {
		pbInspectPattern[i] = 0;
		EstimatedBallCenter.x=(long)(pptTeachPattern[i].x*cos(dFndAngle)+pptTeachPattern[i].y*sin(dFndAngle)+dFndXShift);
		EstimatedBallCenter.y=(long)(-pptTeachPattern[i].x*sin(dFndAngle)+pptTeachPattern[i].y*cos(dFndAngle)+dFndYShift);
		for(j=0;j<nInspectPatternNo;j++) {
			if(pbBallChecked[j]==FALSE)	{
				offsetXY=abs(EstimatedBallCenter.x-pptInspectPattern[j].x)+abs(EstimatedBallCenter.y-pptInspectPattern[j].y);
				if(offsetXY<nCenterUncertainty) {
					pInspectionPatternTemp[i].x=pptInspectPattern[j].x;
					pInspectionPatternTemp[i].y=pptInspectPattern[j].y;
					pbInspectPattern[i] = TRUE;
					pbBallChecked[j]=TRUE;
					sumDeltaX=sumDeltaX+pInspectionPatternTemp[i].x-EstimatedBallCenter.x;
					sumDeltaY=sumDeltaY+pInspectionPatternTemp[i].y-EstimatedBallCenter.y;
					sumNo=sumNo+1;
					break;
				}
			}
		}
		if(pbInspectPattern[i]==FALSE) {
			pInspectionPatternTemp[i].x=EstimatedBallCenter.x;
			pInspectionPatternTemp[i].y=EstimatedBallCenter.y;
		}
	}
	int AverageOffsetX,AverageOffsetY;
	if(sumNo < 1) {
		AverageOffsetX=0;
		AverageOffsetY=0;
	}
	else {
		AverageOffsetX=sumDeltaX/sumNo;
		AverageOffsetY=sumDeltaY/sumNo;
	}

	for(i=0;i<nTeachPatternNo;i++) {
		if(pbInspectPattern[i]==FALSE) {
			pInspectionPatternTemp[i].x=pInspectionPatternTemp[i].x+AverageOffsetX;
			pInspectionPatternTemp[i].y=pInspectionPatternTemp[i].y+AverageOffsetY;
		}
		pptInspectPattern[i]=pInspectionPatternTemp[i];
	}
	dFndXShift=dFndXShift+AverageOffsetX;
	dFndYShift=dFndYShift+AverageOffsetY;
	delete [] pInspectionPatternTemp;
}


CHoughTransform::CHoughTransform()
{
	m_nLUTWidth = 0;
	m_nLUTHeight = 0;
	m_dTableNominalAngle = 0;
	m_dTableAngleTol = 0;
	m_dTableAngleStep = 1;
	m_bLUTStatus = 0;
	m_dAngleMin = -PI / 2;
	m_dAngleMax = PI / 2;
	m_bLUTChangeStatus = 0;
}

int CHoughTransform::GetLineLUTSize(CRect* prcROI,
								double dTolAngleRad,
								double dAngleStep,
								long& lLUTSize)
{
	if(dAngleStep == 0) {
		ASSERT(!"dAngleStep is 0");
		return -1;
	}
	lLUTSize = prcROI->Height() * prcROI->Width() * (int)((2 * dTolAngleRad) / dAngleStep + 0.5 + 1);
	return 0;
}

int CHoughTransform::GetLineLUTStatus(CRect* prcROI,
							   double dNominalAngleRad,
							   double dTolAngleRad,
							   double dAngleStep,
							   bool& bLUTChangeStatus,
							   bool& bLUTStatus)
{
	if(dAngleStep == 0) {
		m_bLUTStatus = 0;
		ASSERT(!"dAngleStep is 0");
		return -1;
	}
	if((m_nLUTWidth != prcROI->Width()) || (m_nLUTHeight != prcROI->Height()) ||
		(m_dTableNominalAngle != dNominalAngleRad) || (m_dTableAngleTol != dTolAngleRad) ||
		(m_dTableAngleStep != dAngleStep))
		m_bLUTChangeStatus = 1;
	else 
		m_bLUTChangeStatus = 0;
	bLUTChangeStatus = m_bLUTChangeStatus;
	bLUTStatus = m_bLUTStatus;
	return 0;
}

int CHoughTransform::InitLineLUT(CRect* prcROI,
							 double dNominalAngleRad,
							 double dTolAngleRad,
							 double dAngleStep,
							 short int * plLUT)
{
	int i,j,k;
	double dAngle;
	long lIndex;
	bool bLUTChangeStatus;
	bool bLUTStatus;
	
	if(dAngleStep == 0) {
		m_bLUTStatus = 0;
		ASSERT(!"dAngleStep is 0");
		return -1;
	}
	if(dNominalAngleRad <= m_dAngleMin) {
		m_bLUTStatus = 0;
		ASSERT(!"dNominalAngleRad is out of range (-PI/2,PI/2]");
		return -1;
	}
	if(dNominalAngleRad > m_dAngleMax) {
		m_bLUTStatus = 0;
		ASSERT(!"dNominalAngleRad is out of range (-PI/2,PI/2]");
		return -1;
	}
	if((dTolAngleRad <= 0) || (dTolAngleRad > m_dAngleMax)) {
		m_bLUTStatus = 0;
		ASSERT(!"dNominalAngleRad is out of range (0,PI/2]");
		return -1;
	}
	GetLineLUTStatus(prcROI,
			  dNominalAngleRad,
			  dTolAngleRad,
			  dAngleStep,
			  bLUTChangeStatus,
			  bLUTStatus);

	if((bLUTChangeStatus) || (m_bLUTStatus == 0)) {
		m_nLUTWidth = prcROI->Width();
		m_nLUTHeight = prcROI->Height();
		m_dTableNominalAngle = dNominalAngleRad;
		m_dTableAngleTol = dTolAngleRad;
		m_dTableAngleStep = dAngleStep;
		m_nTableAngleSize = (int)((2 * dTolAngleRad) / m_dTableAngleStep + 0.5) + 1;
		m_lROISize = m_nLUTWidth * m_nLUTHeight;
		m_lLUTSize = m_lROISize * m_nTableAngleSize;

		dAngle = dNominalAngleRad - dTolAngleRad;
		lIndex = 0;
		for(k = 0; k < m_nTableAngleSize; k++) {
			if(dAngle > m_dAngleMax) {
				dAngle = dAngle - PI;
			}
			for(i = 0; i < m_nLUTHeight; i++)	{
				for(j = 0; j < m_nLUTWidth; j++) {
					plLUT[lIndex++]= (short int)(((i - m_nLUTHeight / 2.0) * cos(dAngle) + (j - m_nLUTWidth / 2.0) * sin(dAngle)));
				}
			}
			dAngle += dAngleStep;
		}
		m_bLUTStatus = 1;
	}
	return 0;
}

int CHoughTransform::LineLUTHoughTransform(CPoint2D *ppt2DInsp,
										   int nInspNo,
										   short int* pnLUT,
										   long lLUTSize,
										   int nOnLineTol,
										   CPoint2D *ppt2DOnLine,
										   int &nOnLineSize,
										   CPoint2D *ppt2DNOTOnLine,
										   int &nNOTOnLineSize)
{
	int* pnPtHoughIndex;
	long lPtHoughIndexSize;
	long lPtHoughIndex;
	int i, j;
	long lIndex0, lIndex;
	long lIndexHough;
	int* pnPtDistance;
	int nAngleHoughMax, nAngleHoughMaxDis;
	long lHoughMax, lHoughMaxDis;
	long lDisDif;
	CPoint* pptInsp;

	CPoint* pptInspOffset;
	CPoint ptOffset;

	nOnLineSize = 0;
	nNOTOnLineSize = 0;
	if(nOnLineTol < 1) {
		ASSERT(!"nOnLineTol is less than 1");
		return -1;
	}
	if(nInspNo < 2) {
		ASSERT(!"Number of points is less than 2");
		return -1;
	}
	if(m_bLUTStatus == 0) {
		ASSERT(!"LUT is not created");
		return -1;
	}
	pptInsp= new CPoint[abs(nInspNo)];
	for(i = 0; i < nInspNo; i++) {
		pptInsp[i].x = (long)ppt2DInsp[i].x; 
		pptInsp[i].y = (long)ppt2DInsp[i].y; 
	}
	GetBoundingRect(pptInsp, nInspNo, &m_rcBounding);
	if(m_rcBounding.Width() > m_nLUTWidth) {
		ASSERT(!"Inspection point is out of LUT range");
		delete [] pptInsp;
		return -1;
	}
	if(m_rcBounding.Height() > m_nLUTHeight) {
		ASSERT(!"Inspection point is out of LUT range");
		delete [] pptInsp;
		return -1;
	}
	lPtHoughIndexSize = nInspNo * m_nTableAngleSize;
	pnPtHoughIndex = new int[abs(lPtHoughIndexSize)];
	pnPtDistance = new int[abs(nInspNo)];
	pptInspOffset = new CPoint[abs(nInspNo)];
	
	ptOffset = CPoint(m_rcBounding.left, m_rcBounding.top);
	GetOffsetPoints(pptInsp, nInspNo, pptInspOffset, &ptOffset);
	
	lIndexHough = 0;
	lIndex0 = 0;
	lPtHoughIndex = 0;
	lHoughMax = 0;
	lHoughMaxDis = 0;
	m_nAngleFound = 0;
	for(i = 0; i < m_nTableAngleSize; i++)	{
		for(j = 0; j < nInspNo; j++) {
			lIndex = lIndex0 + pptInspOffset[j].y * m_nLUTWidth + pptInspOffset[j].x;
			pnPtDistance[j] = pnLUT[lIndex]; 
			pnPtHoughIndex [lPtHoughIndex + j] = pnPtDistance[j];
		}
//		AdvHistogram.GetAccHist(pnPtDistance, nInspNo, pnHistCoordinate, pnHistAcc, nHistNo);
//		AdvHistogram.GetMaxConvHist1D(pnHistCoordinate,pnHistAcc,nHistNo,nOnLineTol,nAngleHoughMax, nAngleHoughMaxDis);
		AdvHistogram.GetMaxConvHist1D(pnPtDistance, 
									  nInspNo,
									  nOnLineTol,
									  nAngleHoughMax, 
									  nAngleHoughMaxDis);
		if(nAngleHoughMax > lHoughMax) {
			lHoughMax = nAngleHoughMax;
			lHoughMaxDis = nAngleHoughMaxDis;
			m_nAngleFound = i;
		}
		lIndex0 += m_lROISize;
		lPtHoughIndex += nInspNo;
	}
	lPtHoughIndex = m_nAngleFound * nInspNo;
	for(i = 0; i < nInspNo; i++) {
		lDisDif = abs(pnPtHoughIndex[lPtHoughIndex + i] - lHoughMaxDis);
		if(lDisDif <= nOnLineTol) {
			ppt2DOnLine[nOnLineSize++] = ppt2DInsp[i];
		}
		else {
			ppt2DNOTOnLine[nNOTOnLineSize++] = ppt2DInsp[i];
		}
	}
	delete [] pptInsp;
	delete [] pnPtDistance;
	delete [] pnPtHoughIndex;
	delete [] pptInspOffset;

	return 0;
}

int CHoughTransform::LineHoughTransform(CPoint2D *ppt2DInsp,
										int nInspNo,
										double dNominalAngleRad,
										double dTolAngleRad,	
										double dAngleStep,		
										double dOnLineTol,
										CPoint2D *ppt2DOnLine,
										int &nOnLineSize,
										CPoint2D *ppt2DNOTOnLine,
										int &nNOTOnLineSize)
{
	int* pnPtHoughIndex;
	long lPtHoughIndexSize;
	long lPtHoughIndex;
	int i, j;
	long lIndex0;
	long lIndexHough;
	int* pnPtDistance;
	int nAngleHoughMax, nAngleHoughMaxDis;
	int nHoughMax, nHoughMaxDis;
	int nDisDif;
	double dCosAngle, dSinAngle;
	double dAngle;
	int nAngleSize;
	int nTimes;
	int nOnLineTol;

	if(dOnLineTol < 0.01) {
		ASSERT(!"dOnLineTol is less than 0.01");
		return -1;
	}
	nTimes =1;
	while(dOnLineTol < 5) {
		nTimes *= 10;
		dOnLineTol *= 10;
		if(nTimes > 100) {
			nTimes = 100;
			if(dOnLineTol < 1)
				dOnLineTol = 1.0;
			break;
		}
	};

	nOnLineTol = (int)dOnLineTol;

	nOnLineSize = 0;
	nNOTOnLineSize = 0;
	if(nInspNo < 2) {
		ASSERT(!"Number of points is less than 2");
		return -1;
	}
	nAngleSize = (int)((2 * dTolAngleRad) / dAngleStep + 0.5) + 1;
	lPtHoughIndexSize = nInspNo * nAngleSize;
	pnPtHoughIndex = new int[abs(lPtHoughIndexSize)];
	pnPtDistance = new int[abs(nInspNo)];

	lIndexHough = 0;
	lIndex0 = 0;
	lPtHoughIndex = 0;
	nHoughMax = 0;
	nHoughMaxDis = 0;
	m_nAngleFound = 0;

	dAngle = dNominalAngleRad - dTolAngleRad;
	if(dAngle < m_dAngleMin) {
		dAngle = dAngle + PI;
	}
	for(i = 0; i < nAngleSize; i++)	{
		if(dAngle > m_dAngleMax) {
			dAngle = dAngle - PI;
		}
		dCosAngle=cos(dAngle) * nTimes;
		dSinAngle=sin(dAngle) * nTimes;
		for(j = 0; j < nInspNo; j++) {
			pnPtDistance[j] = int(ppt2DInsp[j].x * dSinAngle + ppt2DInsp[j].y * dCosAngle + 0.5);
			pnPtHoughIndex [lPtHoughIndex + j] = pnPtDistance[j];
		}
//		AdvHistogram.GetAccHist(pnPtDistance, nInspNo, pnHistCoordinate, pnHistAcc, nHistNo);
//		AdvHistogram.GetMaxConvHist1D(pnHistCoordinate,pnHistAcc,nHistNo,dOnLineTol,nAngleHoughMax, nAngleHoughMaxDis);
		AdvHistogram.GetMaxConvHist1D(pnPtDistance, 
									  nInspNo,
									  nOnLineTol,
									  nAngleHoughMax,
									  nAngleHoughMaxDis);

		if(nAngleHoughMax > nHoughMax) {
			nHoughMax = nAngleHoughMax;
			nHoughMaxDis = nAngleHoughMaxDis;
			m_nAngleFound = i;
		}
		lIndex0 += m_lROISize;
		lPtHoughIndex += nInspNo;
		dAngle += dAngleStep;
	}
	int nNEGOnLineTol;
	int nPOSOnLineTol;
	nPOSOnLineTol = (int)dOnLineTol;
	nNEGOnLineTol = (int)(-dOnLineTol);
	lPtHoughIndex = m_nAngleFound * nInspNo;
	for(i = 0; i < nInspNo; i++) {
		nDisDif = pnPtHoughIndex[lPtHoughIndex + i] - nHoughMaxDis;
		if((nDisDif >= nNEGOnLineTol)&&(nDisDif <= nPOSOnLineTol)) {
			ppt2DOnLine[nOnLineSize++] = ppt2DInsp[i];
		}
		else {
			ppt2DNOTOnLine[nNOTOnLineSize++] = ppt2DInsp[i];
		}
	}
	delete [] pnPtDistance;
	delete [] pnPtHoughIndex;

	return 0;
}

int CHoughTransform::HoughTransformLine(CPoint2D *ppt2DInsp,
										int nInspNo,
										double dNominalAngleRad,
										double dTolAngleRad,	
										double dAngleStep,		
										double dOnLineTol,
										CPoint2D *ppt2DOnLine,
										int &nOnLineSize,
										CPoint2D *ppt2DNOTOnLine,
										int &nNOTOnLineSize,
										int nMinOnLinePercentageForFastSearch)
{
	int* pnPtHoughIndex;
	long lPtHoughIndexSize;
	long lPtHoughIndex;
	int i, j;
	long lIndexHough;
	int* pnPtDistance;
	int nAngleHoughMax, nAngleHoughMaxDis;
	int nHoughMax, nHoughMaxDis;
	int nDisDif;
//	double dCosAngle, dSinAngle;
	int nCosAngleTimes16Bits, nSinAngleTimes16Bits;
	double dAngle;
	int nAngleSize;
//	int nTimes;
	int nOnLineTol;
	double* pdSortingAngle;
	int* pnSortingAngleIndex;
	int* pnAngleCheckedPercentage;
	int nIndexTemp;
	int nStepNoByHalf;
	double dAngle0;
	double dAngle1;
	int nPercentageMatchedIndex;
	int nBestIndex;
	int nNEGOnLineTol;
	int nPOSOnLineTol;
	int* pnXTimes128;
	int* pnYTimes128;
	BOOL bCheckMore;
	int nMaxAngleCheckedPercentage;

	pdSortingAngle = NULL;
	pnAngleCheckedPercentage = NULL;
	pnSortingAngleIndex = NULL;
	pnXTimes128 = NULL;
	pnYTimes128 = NULL;

	nOnLineTol = (int)(dOnLineTol * 128);

	nOnLineSize = 0;
	nNOTOnLineSize = 0;
	if(nInspNo < 2) {
 		ASSERT(!"Number of points is less than 2");
		return -1;
	}
	nStepNoByHalf = Round(dTolAngleRad / dAngleStep);
	nAngleSize = 2 * nStepNoByHalf + 1;
	pdSortingAngle = new double[abs(nAngleSize)];
	pnAngleCheckedPercentage = new int[abs(nAngleSize)];
	pnSortingAngleIndex = new int[abs(nAngleSize)];
	for(i=0;i<nAngleSize;i++)
		pnAngleCheckedPercentage[i] = -1;
	nIndexTemp = 0;
	pnSortingAngleIndex[nIndexTemp] = nStepNoByHalf;
	pdSortingAngle[nIndexTemp] = dNominalAngleRad;
	nIndexTemp++;
	bCheckMore = FALSE;
	for(i=1;i<=nStepNoByHalf;i++) {
		dAngle0 = dNominalAngleRad - i * dAngleStep;
		if(dAngle0 < m_dAngleMin) 
			dAngle0 = dAngle0 + PI;
		dAngle1 = dNominalAngleRad + i * dAngleStep;
		if(dAngle1 > m_dAngleMax) 
			dAngle1 = dAngle1 + PI;
		pnSortingAngleIndex[nIndexTemp] = nStepNoByHalf-i;
		pdSortingAngle[nIndexTemp] = dAngle0;
		nIndexTemp++;
		pnSortingAngleIndex[nIndexTemp] = nStepNoByHalf+i;
		pdSortingAngle[nIndexTemp] = dAngle1;
		nIndexTemp++;
	}


	lPtHoughIndexSize = nInspNo * nAngleSize;
	pnPtHoughIndex = new int[abs(lPtHoughIndexSize)];
	pnPtDistance = new int[abs(nInspNo)];
	pnXTimes128 = new int[abs(nInspNo)];
	pnYTimes128 = new int[abs(nInspNo)];
	for(i = 0; i < nInspNo; i++) {
		pnXTimes128[i] = int(ppt2DInsp[i].x * 128);
		pnYTimes128[i] = int(ppt2DInsp[i].y * 128);
	}
	lIndexHough = 0;
	lPtHoughIndex = 0;
	nHoughMax = 0;
	nHoughMaxDis = 0;
	m_nAngleFound = 0;
	nPercentageMatchedIndex = -1;
	nBestIndex = -1;
	for(i = 0; i < nAngleSize; i++)	{
		dAngle = pdSortingAngle[i];
		nCosAngleTimes16Bits = (int)(cos(dAngle) * 65536);
		nSinAngleTimes16Bits = (int)(sin(dAngle) * 65536);
		lPtHoughIndex = pnSortingAngleIndex[i] * nInspNo;
		for(j = 0; j < nInspNo; j++) {
			pnPtDistance[j] = (pnXTimes128[j] * nSinAngleTimes16Bits - pnYTimes128[j] * nCosAngleTimes16Bits)>>16;
			pnPtHoughIndex [lPtHoughIndex + j] = pnPtDistance[j];
		}
		AdvHistogram.GetMaxConvHist1D(pnPtDistance, 
									  nInspNo,
									  nOnLineTol,
									  nAngleHoughMax,
									  nAngleHoughMaxDis);
		if(nAngleHoughMax > nHoughMax) {
			nHoughMax = nAngleHoughMax;
			nHoughMaxDis = nAngleHoughMaxDis;
			m_nAngleFound = pnSortingAngleIndex[i];
			nBestIndex = m_nAngleFound;
		}
		if(nHoughMax == nInspNo)
			goto PASS;
		pnAngleCheckedPercentage[pnSortingAngleIndex[i]] = 100 * nAngleHoughMax/nInspNo;
		if(pnAngleCheckedPercentage[pnSortingAngleIndex[i]] >= nMinOnLinePercentageForFastSearch) {
			nPercentageMatchedIndex = pnSortingAngleIndex[i];
			break;
		}
	}
	if(nPercentageMatchedIndex >= 0) {
		for(i = nPercentageMatchedIndex - 1; i >= 0; i--)	{
			if(pnAngleCheckedPercentage[i] >= 0) {
				if(pnAngleCheckedPercentage[i] < pnAngleCheckedPercentage[i+1]) {
					nMaxAngleCheckedPercentage = 100 * nHoughMax/nInspNo;
					if(nMaxAngleCheckedPercentage <= (pnAngleCheckedPercentage[nPercentageMatchedIndex] + __max(3,100/nInspNo))) {
						bCheckMore = TRUE;
					}
					break;
				}
				else {
					nBestIndex = i;
					continue;
				}
			}
			dAngle = dNominalAngleRad - dTolAngleRad + i * dAngleStep;
			nCosAngleTimes16Bits = (int)(cos(dAngle) * 65536);
			nSinAngleTimes16Bits = (int)(sin(dAngle) * 65536);
			lPtHoughIndex = i * nInspNo;
			for(j = 0; j < nInspNo; j++) {
				pnPtDistance[j] = (pnXTimes128[j] * nSinAngleTimes16Bits - pnYTimes128[j] * nCosAngleTimes16Bits)>>16;
				pnPtHoughIndex [lPtHoughIndex + j] = pnPtDistance[j];
			}
			AdvHistogram.GetMaxConvHist1D(pnPtDistance, 
										  nInspNo,
										  nOnLineTol,
										  nAngleHoughMax,
										  nAngleHoughMaxDis);
			if(nAngleHoughMax > nHoughMax) {
				nHoughMax = nAngleHoughMax;
				nHoughMaxDis = nAngleHoughMaxDis;
				m_nAngleFound = i;
				nBestIndex = m_nAngleFound;
			}
			if(nHoughMax == nInspNo)
				goto PASS;
			pnAngleCheckedPercentage[i] = 100 * nAngleHoughMax/nInspNo;
			if(pnAngleCheckedPercentage[i] < pnAngleCheckedPercentage[i+1]) {
				nMaxAngleCheckedPercentage = 100 * nHoughMax/nInspNo;
				if(nMaxAngleCheckedPercentage <= (pnAngleCheckedPercentage[nPercentageMatchedIndex] + __max(3,100/nInspNo))) {
					bCheckMore = TRUE;
				}
				break;
			}
			else {
				nBestIndex = i;
				m_nAngleFound = i;
				nHoughMax = nAngleHoughMax;
				nHoughMaxDis = nAngleHoughMaxDis;
				if(pnAngleCheckedPercentage[i] == 100)
					break;
				continue;
			}
		}	
		if((bCheckMore == FALSE) && (nBestIndex >= 0)) {
			
		}
		else {
			nBestIndex = nPercentageMatchedIndex;
			for(i = nPercentageMatchedIndex + 1; i <nAngleSize; i++)	{
				if(pnAngleCheckedPercentage[i] >= 0) {
					if(pnAngleCheckedPercentage[i] < pnAngleCheckedPercentage[i-1])
						break;
					else {
						nBestIndex = i;
						continue;
					}
				}
				dAngle = dNominalAngleRad - dTolAngleRad + i * dAngleStep;
				nCosAngleTimes16Bits = (int)(cos(dAngle) * 65536);
				nSinAngleTimes16Bits = (int)(sin(dAngle) * 65536);
				lPtHoughIndex = i * nInspNo;
				for(j = 0; j < nInspNo; j++) {
					pnPtDistance[j] = (pnXTimes128[j] * nSinAngleTimes16Bits - pnYTimes128[j] * nCosAngleTimes16Bits)>>16;
					pnPtHoughIndex [lPtHoughIndex + j] = pnPtDistance[j];
				}
				AdvHistogram.GetMaxConvHist1D(pnPtDistance, 
											  nInspNo,
											  nOnLineTol,
											  nAngleHoughMax,
											  nAngleHoughMaxDis);
				if(nAngleHoughMax > nHoughMax) {
					nHoughMax = nAngleHoughMax;
					nHoughMaxDis = nAngleHoughMaxDis;
					m_nAngleFound = i;
					nBestIndex = i;
				}
				if(nHoughMax == nInspNo)
					goto PASS;
				pnAngleCheckedPercentage[i] = 100 * nAngleHoughMax/nInspNo;
				if(pnAngleCheckedPercentage[i] < pnAngleCheckedPercentage[i-1])
					break;
				else {
					nBestIndex = i;
					m_nAngleFound = i;
					nHoughMax = nAngleHoughMax;
					nHoughMaxDis = nAngleHoughMaxDis;
					if(pnAngleCheckedPercentage[i] == 100)
						break;
					continue;
				}
			}	
		}
//		if(nBestIndex >= 0)
//			m_nAngleFound = nBestIndex;
	}


PASS:

	nPOSOnLineTol = nOnLineTol;
	nNEGOnLineTol = -nOnLineTol;
	lPtHoughIndex = m_nAngleFound * nInspNo;
	for(i = 0; i < nInspNo; i++) {
		nDisDif = pnPtHoughIndex[lPtHoughIndex + i] - nHoughMaxDis;
		if((nDisDif >= nNEGOnLineTol)&&(nDisDif <= nPOSOnLineTol)) {
			ppt2DOnLine[nOnLineSize++] = ppt2DInsp[i];
		}
		else {
			ppt2DNOTOnLine[nNOTOnLineSize++] = ppt2DInsp[i];
		}
	}
	delete [] pnPtDistance;
	delete [] pnPtHoughIndex;
	if(pdSortingAngle != NULL)
		delete [] pdSortingAngle;
	if(pnAngleCheckedPercentage != NULL)
		delete [] pnAngleCheckedPercentage;
	if(pnSortingAngleIndex != NULL)
		delete [] pnSortingAngleIndex;
	if(pnXTimes128 != NULL)
		delete [] pnXTimes128;
	if(pnYTimes128 != NULL)
		delete [] pnYTimes128;
	return 0;
}


void CHoughTransform::GetOffsetPoints(CPoint* pptSrc,
									 int nSrc,
									 CPoint* pptDst,
									 CPoint* pptOffset)
{
	int i;
	for( i = 0; i < nSrc; i++) {
		pptDst[i] = pptSrc[i] - *pptOffset;
	}
}


void CHoughTransform::GetBoundingRect(CPoint *pptInsp,
									  int nInspNo,
									  CRect* prcBounding)
{
	int i;
	int nTopMost, nBotMost, nLftMost, nRhtMost;
	nTopMost = (int)pptInsp[0].y;
	nBotMost = (int)pptInsp[0].y;
	nLftMost = (int)pptInsp[0].x;
	nRhtMost = (int)pptInsp[0].x;
	for(i=1; i<nInspNo; i++) {
		if(nTopMost > pptInsp[i].y) 
			nTopMost = (int)pptInsp[i].y;
		if(nBotMost < pptInsp[i].y) 
			nBotMost = (int)pptInsp[i].y;
		if(nLftMost > pptInsp[i].x) 
			nLftMost = (int)pptInsp[i].x;
		if(nRhtMost < pptInsp[i].x) 
			nRhtMost = (int)pptInsp[i].x;
	}
	*prcBounding = CRect(nLftMost, nTopMost, nRhtMost, nBotMost);
}



PROJ_1D_X::PROJ_1D_X()
{	
	pnY = NULL;
	nYSize = 0;
}

PROJ_1D_X::~PROJ_1D_X()
{
	if(pnY)
		delete [] pnY;
}

PROJ_1D_Y::PROJ_1D_Y()
{	
	pnX = NULL;
	nXSize = 0;
}

PROJ_1D_Y::~PROJ_1D_Y()
{
	if(pnX)
		delete [] pnX;
}

PROJ_2D_X::PROJ_2D_X()
{	
	nptSize = 0;
	nHistSize = 0;
	nHistConv = 0;
	nMaxCovHist = 0;
	nMaxCovHistPos = 0;
	pnSortX = NULL;
	pnY = NULL;
	pnSortXIndex = NULL;
	pnHistPosSort = NULL;
	pnHistConvSort = NULL;
	pnHistConvSortIndex = NULL;
	pPROJX = NULL;
	nMemLoc = 0;
}

void PROJ_2D_X::MemLocate(int nInspNo)
{
	int i;
	Clean();
	nMemLoc = nInspNo;
	pnHistConvSort = new int[abs(nInspNo)];
	pnHistPosSort = new int[abs(nInspNo)];
	pnSortX = new int[abs(nInspNo)];
	pnSortXIndex = new int[abs(nInspNo)];
	pnHistConvSortIndex = new int[abs(nInspNo)];
	pnY = new int[abs(nInspNo)];
	pPROJX = new PROJ_1D_X[abs(nInspNo)];
	for(i = 0; i < nInspNo; i++)
		pPROJX[i].pnY = new int[abs(nInspNo)];
}

void PROJ_2D_X::Clean()
{
	int i;
	if(pnHistConvSort) {
		delete [] pnHistConvSort;
		pnHistConvSort = NULL;
	}
	if(pnHistPosSort) {
		delete [] pnHistPosSort;
		pnHistPosSort = NULL;
	}
	if(pnSortX) {
		delete [] pnSortX;
		pnSortX = NULL;
	}
	if(pnSortXIndex) {
		delete [] pnSortXIndex;
		pnSortXIndex = NULL;
	}
	if(pnHistConvSortIndex) {
		delete [] pnHistConvSortIndex;
		pnHistConvSortIndex = NULL;
	}
	if(pnY) {
		delete [] pnY;
		pnY = NULL;
	}
	if(pPROJX) {
		for(i = 0; i < nMemLoc; i++) {
			if(pPROJX[i].pnY) {
				delete [] pPROJX[i].pnY;
				pPROJX[i].pnY = NULL;
			}
		}
		nMemLoc = 0;
		delete [] pPROJX;
		pPROJX = NULL;
	}
}


PROJ_2D_X::~PROJ_2D_X()
{
	Clean();
}


PROJ_2D_Y::PROJ_2D_Y()
{	
	nptSize = 0;
	nHistSize = 0;
	nHistConv = 0;
	nMaxCovHist = 0;
	nMaxCovHistPos = 0;
	pnSortY = NULL;
	pnX = NULL;
	pnSortYIndex = NULL;
	pnHistPosSort = NULL;
	pnHistConvSort = NULL;
	pnHistConvSortIndex = NULL;
	pPROJY = NULL;
	nMemLoc = 0;
}

void PROJ_2D_Y::MemLocate(int nInspNo)
{
	int i;
	Clean();
	nMemLoc = nInspNo;
	pnHistConvSort = new int[abs(nInspNo)];
	pnHistPosSort = new int[abs(nInspNo)];
	pnSortY = new int[abs(nInspNo)];
	pnSortYIndex = new int[abs(nInspNo)];
	pnHistConvSortIndex = new int[abs(nInspNo)];
	pnX = new int[abs(nInspNo)];
	pPROJY = new PROJ_1D_Y[abs(nInspNo)];
	for(i = 0; i < nInspNo; i++)
		pPROJY[i].pnX = new int[abs(nInspNo)];
}

void PROJ_2D_Y::Clean()
{
	int i;
	if(pnHistConvSort) {
		delete [] pnHistConvSort;
		pnHistConvSort = NULL;
	}
	if(pnHistPosSort) {
		delete [] pnHistPosSort;
		pnHistPosSort = NULL;
	}
	if(pnSortY) {
		delete [] pnSortY;
		pnSortY = NULL;
	}
	if(pnSortYIndex) {
		delete [] pnSortYIndex;
		pnSortYIndex = NULL;
	}
	if(pnHistConvSortIndex) {
		delete [] pnHistConvSortIndex;
		pnHistConvSortIndex = NULL;
	}
	if(pnX) {
		delete [] pnX;
		pnX = NULL;
	}
	if(pPROJY) {
		for(i = 0; i < nMemLoc; i++) {
			if(pPROJY[i].pnX) {
				delete [] pPROJY[i].pnX;
				pPROJY[i].pnX = NULL;
			}
		}
		nMemLoc = 0;
		delete [] pPROJY;
		pPROJY = NULL;
	}
}

PROJ_2D_Y::~PROJ_2D_Y()
{
	Clean();
}


PROJ_2D_Search::PROJ_2D_Search()
{	
	nptSize = 0;
	nHistSize = 0;
	nHistConv = 0;
	nMaxCovHist = 0;
	nMaxCovHistPos = 0;
	pnData = NULL;
	pnHistPosSort = NULL;
	pnHistConvSort = NULL;
	pnHistConvSortIndex = NULL;
}

void PROJ_2D_Search::MemLocate(int nInspNo)
{
	Clean();
	pnData = new int[abs(nInspNo)];
	pnHistPosSort = new int[abs(nInspNo)];
	pnHistConvSort = new int[abs(nInspNo)];
	pnHistConvSortIndex = new int[abs(nInspNo)];
}

void PROJ_2D_Search::Clean()
{
	if(pnData) {
		delete [] pnData;
		pnData = NULL;
	}
	if(pnHistPosSort) {
		delete [] pnHistPosSort;
		pnHistPosSort = NULL;
	}
	if(pnHistConvSort) {
		delete [] pnHistConvSort;
		pnHistConvSort = NULL;
	}
	if(pnHistConvSortIndex) {
		delete [] pnHistConvSortIndex;
		pnHistConvSortIndex = NULL;
	}
}


PROJ_2D_Search::~PROJ_2D_Search()
{
	Clean();
}



ADV_HIST_1D_RSLT::ADV_HIST_1D_RSLT()
{	
	nHistNo = 0;
	pnHist = NULL;
	plPos = NULL; 
}

void ADV_HIST_1D_RSLT::MemLocate(int nNo)
{
	Clean();
	pnHist = new int[abs(nNo)];
	plPos = new long[abs(nNo)];

}

void ADV_HIST_1D_RSLT::Clean()
{
	nHistNo = 0;
	if(pnHist) {
		delete [] pnHist;
		pnHist = NULL;
	}
	if(plPos) {
		delete [] plPos;
		plPos = NULL;
	}
}

ADV_HIST_1D_RSLT::~ADV_HIST_1D_RSLT()
{
	Clean();
}





ADV_CONV_HIST_1D_RSLT::ADV_CONV_HIST_1D_RSLT()
{	
	nHistNo = 0;
	pnHist = NULL;
	plPosStart = NULL; 
	plPosEnd = NULL; 
}

void ADV_CONV_HIST_1D_RSLT::MemLocate(int nNo)
{
	Clean();
	pnHist = new int[abs(nNo)];
	plPosStart = new long[abs(nNo)];
	plPosEnd = new long[abs(nNo)];
}

void ADV_CONV_HIST_1D_RSLT::Clean()
{
	nHistNo = 0;
	if(pnHist) {
		delete [] pnHist;
		pnHist = NULL;
	}
	if(plPosStart) {
		delete [] plPosStart;
		plPosStart = NULL;
	}
	if(plPosEnd) {
		delete [] plPosEnd;
		plPosEnd = NULL;
	}
}

ADV_CONV_HIST_1D_RSLT::~ADV_CONV_HIST_1D_RSLT()
{
	Clean();
}


int CAdvHistogram::GetHist(int *pnData,
						   int nDataNo,
						   ADV_HIST_1D_RSLT* pAdvHist1DRslt)
{
	int i;
	int nCurrentDis;
	int nPtHist;
	int nHistNo;
	bool bAscend = TRUE;
	if(nDataNo <= 0) {
		ASSERT(!"nDataNo is less than or equal to 0");
		pAdvHist1DRslt->nHistNo = 0;
		return -1;
	}
	pAdvHist1DRslt->MemLocate(nDataNo);
	Sorting(pnData,nDataNo,bAscend);
	nCurrentDis = pnData[0];
	nPtHist = 0;
	pAdvHist1DRslt->plPos[0] = nCurrentDis;
	nHistNo = 0;
	for(i=0;i<nDataNo;i++) {
		if(pnData[i] == nCurrentDis) {
			nPtHist++;
		}
		else {
			pAdvHist1DRslt->pnHist[nHistNo] = nPtHist;
			pAdvHist1DRslt->plPos[nHistNo] = nCurrentDis;
			nPtHist = 0;
			nCurrentDis = pnData[i];
			nHistNo++;
			pAdvHist1DRslt->pnHist[nHistNo] = 0;
			i--;
		}
	}
	pAdvHist1DRslt->pnHist[nHistNo] = nPtHist;
	pAdvHist1DRslt->plPos[nHistNo] = nCurrentDis;
	nHistNo++;
	if(nHistNo == 0)
		pAdvHist1DRslt->pnHist[0] = nPtHist;
	pAdvHist1DRslt->nHistNo = nHistNo;
	return 0;
}

int CAdvHistogram::GetAccHist(int *pnData,
							  int nDataNo,
							  ADV_HIST_1D_RSLT* pAdvHist1DRslt)
{
	int i;
	int nCurrentDis;
	int nPtHist;
	bool bAscend = TRUE;
	int nHistNo;
	if(nDataNo <= 0) {
		ASSERT(!"nDataNo is less than or equal to 0");
		pAdvHist1DRslt->nHistNo = 0;
		return -1;
	}
	pAdvHist1DRslt->MemLocate(nDataNo);
	Sorting(pnData,nDataNo,bAscend);
	nHistNo = 0;
	nCurrentDis = pnData[0];
	nPtHist = 0;
	for(i=0;i<nDataNo;i++) {
		if(pnData[i] == nCurrentDis) 
			nPtHist++;
		else {
			pAdvHist1DRslt->plPos[nHistNo] = nCurrentDis;
			nCurrentDis = pnData[i];
			pAdvHist1DRslt->pnHist[nHistNo] = nPtHist; 
			nHistNo++;
			i--;
		}
	}
	pAdvHist1DRslt->plPos[nHistNo] = nCurrentDis;
	pAdvHist1DRslt->pnHist[nHistNo] = nPtHist; 
	nHistNo++;
	if(nHistNo == 0)
		pAdvHist1DRslt->pnHist[0] = nPtHist;
	pAdvHist1DRslt->nHistNo = nHistNo;
	return 0;
}


int CAdvHistogram::GetAccHist(long *plData,
							  int nDataNo,
							  ADV_HIST_1D_RSLT* pAdvHist1DRslt)
{
	int i;
	long lCurrentDis;
	int nPtHist;
	int nHistNo;
	bool bAscend = TRUE;
	if(nDataNo <= 0) {
		ASSERT(!"nDataNo is less than or equal to 0");
		pAdvHist1DRslt->nHistNo = 0;
		return -1;
	}
	pAdvHist1DRslt->MemLocate(nDataNo);
	Sorting(plData,nDataNo,bAscend);
	nHistNo = 0;
	lCurrentDis = plData[0];
	nPtHist = 0;
	for(i=0;i<nDataNo;i++) {
		if(plData[i] == lCurrentDis)
			nPtHist++;
		else {
			pAdvHist1DRslt->plPos[nHistNo] = lCurrentDis;
			lCurrentDis = plData[i];
			pAdvHist1DRslt->pnHist[nHistNo] = nPtHist; 
			nHistNo++;
			i--;
		}
	}

	pAdvHist1DRslt->plPos[nHistNo] = lCurrentDis;
//	lCurrentDis = plData[i];
	pAdvHist1DRslt->pnHist[nHistNo] = nPtHist; 
	nHistNo++;
	if(nHistNo == 0)
		pAdvHist1DRslt->pnHist[0] = nPtHist;
	pAdvHist1DRslt->nHistNo = nHistNo;
	return 0;
}


int CAdvHistogram::GetMaxConvHist1D(int *pnData, 
									int nDataNo,
									int nHistConv,
									int	&nMaxHist,
									int &nMaxHistPos)
{
	int i, j;
	int nHoughVal;

	int nCurrentCoor;
	int nEndIndex;
	int nEndCoor;
	int nEndIndexBak;
	int nHistNo;
	ADV_HIST_1D_RSLT AdvHist1DRslt;

	if(nDataNo <= 0) {
		ASSERT(!"nDataNo is less than or equal to 0");
		AdvHist1DRslt.nHistNo = 0;
		return -1;
	}
	AdvHist1DRslt.MemLocate(nDataNo);
	GetAccHist(pnData,nDataNo,&AdvHist1DRslt);
	nHistNo = AdvHist1DRslt.nHistNo;
	nMaxHist = 0;
	nMaxHistPos = 0;
	nEndCoor = AdvHist1DRslt.plPos[0] + nHistConv;
	for(i = 0;i < nHistNo; i++ ) {
		if(AdvHist1DRslt.plPos[i] > nEndCoor) 
			break;
	}

	nEndIndex = i-1;
	nMaxHist = AdvHist1DRslt.pnHist[nEndIndex];
	nMaxHistPos = (AdvHist1DRslt.plPos[0] + AdvHist1DRslt.plPos[nEndIndex]) / 2;
	nEndIndexBak = nEndIndex;
	for(i = 1; i < nHistNo; i++) {
		nCurrentCoor = AdvHist1DRslt.plPos[i];
		nEndCoor = nCurrentCoor + nHistConv;
		for(j = nEndIndexBak; j < nHistNo; j++) {
			if(AdvHist1DRslt.plPos[j] > nEndCoor)
				break;
		}
		nEndIndex = j-1;
		nEndIndexBak = nEndIndex;
		nHoughVal = AdvHist1DRslt.pnHist[nEndIndex] - AdvHist1DRslt.pnHist[i-1];
		if(nHoughVal > nMaxHist) {
			nMaxHist = nHoughVal;
			nMaxHistPos = (AdvHist1DRslt.plPos[i] + AdvHist1DRslt.plPos[nEndIndex]) / 2;
		}
		if(j == nHistNo)
			break;
	}
	return 0;
}


int CAdvHistogram::GetMaxConvHist1D(int *pnData, 
									int nDataNo,
									int nHistConv,
									int	&nMaxHist,
									int &nMaxHistPos,
									double &dAvgPos)
{
	int i, j;
	int nHoughVal;

	int nCurrentCoor;
	int nEndIndex;
	int nEndCoor;
	int nEndIndexBak;
	int nHistNo;
	double dSum;
	int nSumNo;
	int nStartBak;
	int nEndBak;
	ADV_HIST_1D_RSLT AdvHist1DRslt;

	if(nDataNo <= 0) {
		ASSERT(!"nDataNo is less than or equal to 0");
		AdvHist1DRslt.nHistNo = 0;
		return -1;
	}
	AdvHist1DRslt.MemLocate(nDataNo);
	GetAccHist(pnData,nDataNo,&AdvHist1DRslt);
	nHistNo = AdvHist1DRslt.nHistNo;
	nMaxHist = 0;
	nMaxHistPos = 0;
	nEndCoor = AdvHist1DRslt.plPos[0] + nHistConv;
	for(i = 0;i < nHistNo; i++ ) {
		if(AdvHist1DRslt.plPos[i] > nEndCoor) 
			break;
	}

	nEndIndex = i-1;

	nStartBak = 0;
	nEndBak = nEndIndex;
	nMaxHist = AdvHist1DRslt.pnHist[nEndIndex];
	nMaxHistPos = (AdvHist1DRslt.plPos[0] + AdvHist1DRslt.plPos[nEndIndex]) / 2;
	nEndIndexBak = nEndIndex;
	for(i = 1; i < nHistNo; i++) {
		nCurrentCoor = AdvHist1DRslt.plPos[i];
		nEndCoor = nCurrentCoor + nHistConv;
		for(j = nEndIndexBak; j < nHistNo; j++) {
			if(AdvHist1DRslt.plPos[j] > nEndCoor)
				break;
		}
		nEndIndex = j-1;
		nEndIndexBak = nEndIndex;
		nHoughVal = AdvHist1DRslt.pnHist[nEndIndex] - AdvHist1DRslt.pnHist[i-1];
		if(nHoughVal > nMaxHist) {
			nMaxHist = nHoughVal;
			nMaxHistPos = (AdvHist1DRslt.plPos[i] + AdvHist1DRslt.plPos[nEndIndex]) / 2;
			nStartBak = i;
			nEndBak = nEndIndex;
		}
		if(j == nHistNo)
			break;
	}
	dSum = 0;
	nSumNo = 0;
	for(i = nStartBak; i <= nEndBak; i++) {
		if(i==0) 
			dSum += AdvHist1DRslt.pnHist[0] * AdvHist1DRslt.plPos[0];
		else
			dSum += (AdvHist1DRslt.pnHist[i] - AdvHist1DRslt.pnHist[i-1]) * AdvHist1DRslt.plPos[i];
	}
	if(nStartBak == 0)
		nSumNo = AdvHist1DRslt.pnHist[nEndBak];
	else
		nSumNo = AdvHist1DRslt.pnHist[nEndBak] - AdvHist1DRslt.pnHist[nStartBak-1];
	if(nSumNo > 0)
		dAvgPos = dSum/nSumNo;
	return 0;
}

int CAdvHistogram::GetMaxConvHist1D(double *pdData, 
									int nDataNo,
									double dHistConv,
									int	&nMaxHist,
									double &dAvgPos)
{
	int i, j;
	int nHoughVal;

	int nCurrentCoor;
	int nEndIndex;
	int nEndCoor;
	int nEndIndexBak;
	int nHistNo;
	double dSum;
	int nSumNo;
	int nStartBak;
	int nEndBak;
	ADV_HIST_1D_RSLT AdvHist1DRslt;
	int *pnData;
	int nHistConv;
	int nTimes;


	if(nDataNo <= 0) {
//		ASSERT(!"nDataNo is less than or equal to 0");
		AdvHist1DRslt.nHistNo = 0;
		return -1;
	}
	if(dHistConv < 0.01) {
//		ASSERT(!"dHistConv is less than 0.01");
		return -1;
	}
	pnData = NULL;
	pnData = new int[abs(nDataNo)];

	nTimes =1;
	while(dHistConv < 5) {
		nTimes *= 10;
		dHistConv *= 10;
		if(nTimes > 100) {
			nTimes = 100;
			if(dHistConv < 1)
				dHistConv = 1.0;
			break;
		}
	};
	nHistConv = (int) dHistConv;
	for(i=0;i<nDataNo;i++) 	
		pnData[i] = (int)(pdData[i] * nTimes);
	AdvHist1DRslt.MemLocate(nDataNo);
	GetAccHist(pnData,nDataNo,&AdvHist1DRslt);
	nHistNo = AdvHist1DRslt.nHistNo;
	nMaxHist = 0;
	nEndCoor = AdvHist1DRslt.plPos[0] + nHistConv;
	for(i = 0;i < nHistNo; i++ ) {
		if(AdvHist1DRslt.plPos[i] > nEndCoor) 
			break;
	}

	nEndIndex = i-1;
	nStartBak = 0;
	nEndBak = nEndIndex;
	nMaxHist = AdvHist1DRslt.pnHist[nEndIndex];
	nEndIndexBak = nEndIndex;
	for(i = 1; i < nHistNo; i++) {
		nCurrentCoor = AdvHist1DRslt.plPos[i];
		nEndCoor = nCurrentCoor + nHistConv;
		for(j = nEndIndexBak; j < nHistNo; j++) {
			if(AdvHist1DRslt.plPos[j] > nEndCoor)
				break;
		}
		nEndIndex = j-1;
		nEndIndexBak = nEndIndex;
		nHoughVal = AdvHist1DRslt.pnHist[nEndIndex] - AdvHist1DRslt.pnHist[i-1];
		if(nHoughVal > nMaxHist) {
			nMaxHist = nHoughVal;
			nStartBak = i;
			nEndBak = nEndIndex;
		}
		if(j == nHistNo)
			break;
	}
	dSum = 0;
	nSumNo = 0;
	for(i = nStartBak; i <= nEndBak; i++) {
		if(i==0) 
			dSum += AdvHist1DRslt.pnHist[0] * AdvHist1DRslt.plPos[0];
		else
			dSum += (AdvHist1DRslt.pnHist[i] - AdvHist1DRslt.pnHist[i-1]) * AdvHist1DRslt.plPos[i];
	}
	if(nStartBak == 0)
		nSumNo = AdvHist1DRslt.pnHist[nEndBak];
	else
		nSumNo = AdvHist1DRslt.pnHist[nEndBak] - AdvHist1DRslt.pnHist[nStartBak-1];
	if(nSumNo > 0)
		dAvgPos = dSum/(nSumNo*nTimes);
	if(pnData != NULL)
		delete [] pnData;
	return 0;
}
// Commented Temporary.. Based on difference between IPIHR and IPI99.. May 05 2010..
/*int CAdvHistogram::GetMaxConvHist1D(double *pdData, 
									int nDataNo,
									double dHistConv,
									int	&nMaxHist,
									double &dAvgPos)
{
	int i, j, m;
	int nHoughVal;

	int nCurrentCoor;
	int nEndIndex;
	int nEndCoor;
	int nEndIndexBak;
	int nHistNo;
	double dSum;
	double dSumSqr;
	int nSumNo;
	int nStartBak;
	int nEndBak;
	ADV_HIST_1D_RSLT AdvHist1DRslt;
	int *pnData;
	int nHistConv;
	int nTimes;
	int* pnStartAry;
	int* pnEndAry;
	int* pnHoughVal;
	double dMinDev;
	double dDev;
	int nMinDevPos;
	double dBestAvg;
	int nAryNo;


	pnData = NULL;
	pnStartAry = NULL;
	pnEndAry = NULL;
	pnHoughVal = NULL;
	if(nDataNo <= 0) {
//		ASSERT(!"nDataNo is less than or equal to 0");
		AdvHist1DRslt.nHistNo = 0;
		goto ErrorAbort;
	}
	if(dHistConv < 0.01) {
		ASSERT(!"dHistConv is less than 0.01");
		goto ErrorAbort;
	}
	pnData = new int[abs(nDataNo)];
	pnStartAry = new int[abs(nDataNo)];
	pnEndAry = new int[abs(nDataNo)];
	pnHoughVal = new int[abs(nDataNo)];

	nTimes =1;
	while(dHistConv < 5) {
		nTimes *= 10;
		dHistConv *= 10;
		if(nTimes > 100) {
			nTimes = 100;
			if(dHistConv < 1)
				dHistConv = 1.0;
			break;
		}
	};
	nHistConv = (int) dHistConv;
	for(i=0;i<nDataNo;i++) 	
		pnData[i] = (int)(pdData[i] * nTimes);
	AdvHist1DRslt.MemLocate(nDataNo);
	GetAccHist(pnData,nDataNo,&AdvHist1DRslt);
	nHistNo = AdvHist1DRslt.nHistNo;
	nMaxHist = 0;
	nEndCoor = AdvHist1DRslt.plPos[0] + nHistConv;
	for(i = 0;i < nHistNo; i++ ) {
		if(AdvHist1DRslt.plPos[i] > nEndCoor) 
			break;
	}

	nAryNo = 0;
	nEndIndex = i-1;
	nStartBak = 0;
	nEndBak = nEndIndex;
	nMaxHist = AdvHist1DRslt.pnHist[nEndIndex];
	nEndIndexBak = nEndIndex;
	pnStartAry[nAryNo] = 0;
	pnEndAry[nAryNo] = nEndIndex;
	pnHoughVal[nAryNo] = nMaxHist;
	nAryNo++;
	for(i = 1; i < nHistNo; i++) {
		nCurrentCoor = AdvHist1DRslt.plPos[i];
		nEndCoor = nCurrentCoor + nHistConv;
		for(j = nEndIndexBak; j < nHistNo; j++) {
			if(AdvHist1DRslt.plPos[j] > nEndCoor)
				break;
		}
		nEndIndex = j-1;
		nEndIndexBak = nEndIndex;
		nHoughVal = AdvHist1DRslt.pnHist[nEndIndex] - AdvHist1DRslt.pnHist[i-1];
		if(nHoughVal >= nMaxHist) {
			nMaxHist = nHoughVal;
			pnStartAry[nAryNo] = i;
			pnEndAry[nAryNo] = nEndIndex;
			pnHoughVal[nAryNo] = nHoughVal;
			nAryNo++;
		}
		if(j == nHistNo)
			break;
	}


	dMinDev = INT_MAX;
	nMinDevPos = 0;
	dBestAvg = 0;
	for(m=0;m<nAryNo;m++) {
		if(pnHoughVal[m] < nMaxHist)
			continue;
		dSum = 0;
		nSumNo = 0;
		dSumSqr = 0;
		nStartBak = pnStartAry[m];
		nEndBak = pnEndAry[m];
		for(i = nStartBak; i <= nEndBak; i++) {
			if(i==0) { 
				dSum += AdvHist1DRslt.pnHist[0] * AdvHist1DRslt.plPos[0];
				dSumSqr += AdvHist1DRslt.pnHist[0] * AdvHist1DRslt.plPos[0] * AdvHist1DRslt.plPos[0];
			}
			else {
				dSum += (AdvHist1DRslt.pnHist[i] - AdvHist1DRslt.pnHist[i-1]) * AdvHist1DRslt.plPos[i];
				dSumSqr += (AdvHist1DRslt.pnHist[i] - AdvHist1DRslt.pnHist[i-1]) * AdvHist1DRslt.plPos[i] * AdvHist1DRslt.plPos[i];
			}
		}
		if(nStartBak == 0)
			nSumNo = AdvHist1DRslt.pnHist[nEndBak];
		else
			nSumNo = AdvHist1DRslt.pnHist[nEndBak] - AdvHist1DRslt.pnHist[nStartBak-1];
		if(nSumNo > 0)
			dAvgPos = dSum/(nSumNo*nTimes);
		dDev = dSumSqr - nSumNo * dAvgPos *nTimes* dAvgPos*nTimes;
		if(dMinDev > dDev) {
			dMinDev = dDev;
			nMinDevPos = m;
			dBestAvg = dAvgPos;
		}
	}
	dAvgPos = dBestAvg;
	if(pnData != NULL)
		delete [] pnData;
	if(pnStartAry != NULL)
		delete [] pnStartAry;
	if(pnEndAry != NULL)
		delete [] pnEndAry;
	if(pnHoughVal != NULL)
		delete [] pnHoughVal;
	return 0;
ErrorAbort:
	if(pnData != NULL)
		delete [] pnData;
	if(pnStartAry != NULL)
		delete [] pnStartAry;
	if(pnEndAry != NULL)
		delete [] pnEndAry;
	if(pnHoughVal != NULL)
		delete [] pnHoughVal;
	return -1;
}
*/
int CAdvHistogram::GetFastMaxConvHist1D(int *pnData, 
									int nDataNo,
									int nHistConv,
									int nHistMinTol,
									int	&nMaxHist,
									int &nMaxHistPos)
{
	int i, j;
	int nHoughVal;

	int nCurrentCoor;
	int nStartIndex;
	int nStartCoor;
	int nEndIndex;
	int nEndCoor;
	int nEndIndexBak;
	int nHistNo;
	int nLeftHistNo;
	ADV_HIST_1D_RSLT AdvHist1DRslt;

	if(nDataNo <= 0) {
		ASSERT(!"nDataNo is less than or equal to 0");
		AdvHist1DRslt.nHistNo = 0;
		return -1;
	}
	if(nHistMinTol == 0) {
		AdvHist1DRslt.MemLocate(nDataNo);
		GetAccHist(pnData,nDataNo,&AdvHist1DRslt);
		nHistNo = AdvHist1DRslt.nHistNo;
		nMaxHist = 0;
		nMaxHistPos = 0;
		nEndCoor = AdvHist1DRslt.plPos[0] + nHistConv;
		for(i = 0;i < nHistNo; i++ ) {
			if(AdvHist1DRslt.plPos[i] > nEndCoor) 
				break;
		}

		nEndIndex = i-1;
		nMaxHist = AdvHist1DRslt.pnHist[nEndIndex];
		nMaxHistPos = (AdvHist1DRslt.plPos[0] + AdvHist1DRslt.plPos[nEndIndex]) / 2;
		nEndIndexBak = nEndIndex;
		nLeftHistNo = nHistNo;
		for(i = 1; i < nHistNo; i++) {
			nCurrentCoor = AdvHist1DRslt.plPos[i];
			nEndCoor = nCurrentCoor + nHistConv;
			for(j = nEndIndexBak; j < nHistNo; j++) {
				if(AdvHist1DRslt.plPos[j] > nEndCoor)
					break;
			}
			nEndIndex = j-1;
			nEndIndexBak = nEndIndex;
			nHoughVal = AdvHist1DRslt.pnHist[nEndIndex] - AdvHist1DRslt.pnHist[i-1];
			if(nHoughVal > nMaxHist) {
				nMaxHist = nHoughVal;
				nMaxHistPos = (AdvHist1DRslt.plPos[i] + AdvHist1DRslt.plPos[nEndIndex]) / 2;
			}
			nLeftHistNo = nHistNo - AdvHist1DRslt.pnHist[i];
			if(nMaxHist > nLeftHistNo)
				break;
			if(j == nHistNo)
				break;
		}
	}
	else {
		AdvHist1DRslt.MemLocate(nDataNo);
		GetAccHist(pnData,nDataNo,&AdvHist1DRslt);
		nHistNo = AdvHist1DRslt.nHistNo;
		nMaxHist = 0;
		nMaxHistPos = 0;
		for(i = 0;i < nHistNo; i++ ) {
			if(AdvHist1DRslt.pnHist[i] > nHistMinTol) 
				break;
		}
		if(i == 0) {
			nStartIndex = 0;
			nEndCoor = AdvHist1DRslt.plPos[0] + nHistConv;
			for(i = 0;i < nHistNo; i++ ) {
				if(AdvHist1DRslt.plPos[i] > nEndCoor) 
					break;
			}
			nEndIndex = i-1;
		}
		else {
			nEndIndex = i-1;
			nStartCoor = AdvHist1DRslt.plPos[nEndIndex] - nHistConv;
			for(i = nEndIndex;i >=0; i-- ) {
				if(AdvHist1DRslt.plPos[i] < nStartCoor) 
					break;
			}
			nStartIndex = i+1;
		}

		nMaxHist = AdvHist1DRslt.pnHist[nEndIndex] - AdvHist1DRslt.pnHist[nStartIndex];
		nMaxHistPos = (AdvHist1DRslt.plPos[nStartIndex] + AdvHist1DRslt.plPos[nEndIndex]) / 2;
		nEndIndexBak = nEndIndex;
		nLeftHistNo = nHistNo - AdvHist1DRslt.pnHist[nStartIndex];

		for(i = nStartIndex + 1; i < nHistNo; i++) {
			nCurrentCoor = AdvHist1DRslt.plPos[i];
			nEndCoor = nCurrentCoor + nHistConv;
			for(j = nEndIndexBak; j < nHistNo; j++) {
				if(AdvHist1DRslt.plPos[j] > nEndCoor)
					break;
			}
			nEndIndex = j-1;
			nEndIndexBak = nEndIndex;
			nHoughVal = AdvHist1DRslt.pnHist[nEndIndex] - AdvHist1DRslt.pnHist[i-1];
			if(nHoughVal > nMaxHist) {
				nMaxHist = nHoughVal;
				nMaxHistPos = (AdvHist1DRslt.plPos[i] + AdvHist1DRslt.plPos[nEndIndex]) / 2;
			}
			nLeftHistNo = nHistNo - AdvHist1DRslt.pnHist[i];
			if((nMaxHist >= nLeftHistNo) || ((nHistMinTol >= nLeftHistNo)))
				break;
			if(j == nHistNo)
				break;
		}
	}
	return 0;
}


int CAdvHistogram::GetMaxConvHist1D(long *plData, 
									int nDataNo,
									int nHistConv,
									int &nMaxHist,
									long &lMaxHistPos)
{
	int i, j;
	int nHoughVal;
	long lCurrentCoor;
	int nEndIndex;
	long lEndCoor;
	int nEndIndexBak;
	int nHistNo;
	ADV_HIST_1D_RSLT AdvHist1DRslt;

	if(nDataNo <= 0) {
		ASSERT(!"nDataNo is less than or equal to 0");
		AdvHist1DRslt.nHistNo = 0;
		return -1;
	}
	AdvHist1DRslt.MemLocate(nDataNo);
	GetAccHist(plData,nDataNo,&AdvHist1DRslt);
	nHistNo = AdvHist1DRslt.nHistNo;
	nMaxHist = 0;
	lMaxHistPos = 0;
	lEndCoor = AdvHist1DRslt.plPos[0] + nHistConv;
	for(i = 0;i < nHistNo; i++ ) {
		if(AdvHist1DRslt.plPos[i] > lEndCoor) 
			break;
	}
	nEndIndex = i-1;
	nMaxHist = AdvHist1DRslt.pnHist[nEndIndex];
	lMaxHistPos = (AdvHist1DRslt.plPos[0] + AdvHist1DRslt.plPos[nEndIndex]) / 2;
	nEndIndexBak = nEndIndex;
	for(i = 1; i < nHistNo; i++) {
		lCurrentCoor = AdvHist1DRslt.plPos[i];
		lEndCoor = lCurrentCoor + nHistConv;
		for(j = nEndIndexBak; j < nHistNo; j++) {
			if(AdvHist1DRslt.plPos[j] > lEndCoor)
				break;
		}
		nEndIndex = j-1;
		nEndIndexBak = nEndIndex;
		nHoughVal = AdvHist1DRslt.pnHist[nEndIndex] - AdvHist1DRslt.pnHist[i-1];
		if(nHoughVal > nMaxHist) {
			nMaxHist = nHoughVal;
			lMaxHistPos = (AdvHist1DRslt.plPos[i] + AdvHist1DRslt.plPos[nEndIndex]) / 2;
		}
		if(j == nHistNo)
			break;
	}
	return 0;
}



int CAdvHistogram::GetConvHist(int *pnData, 
							   int nDataNo,
							   int nHistConv,
							   ADV_CONV_HIST_1D_RSLT* pAdvConvHist1DRslt)
{
	ADV_HIST_1D_RSLT AdvHist1DRslt;
	if(nDataNo <= 0) {
		ASSERT(!"nDataNo is less than or equal to 0");
		AdvHist1DRslt.nHistNo = 0;
		return -1;
	}
	GetAccHist(pnData,nDataNo,&AdvHist1DRslt);
	GetConvHist(&AdvHist1DRslt,
				nHistConv,
				pAdvConvHist1DRslt);
	return 0;
}

int CAdvHistogram::GetConvHist(long *plData, 
							   int nDataNo,
							   int nHistConv,
							   ADV_CONV_HIST_1D_RSLT* pAdvConvHist1DRslt)
{
	ADV_HIST_1D_RSLT AdvHist1DRslt;
	if(nDataNo <= 0) {
		ASSERT(!"nDataNo is less than or equal to 0");
		AdvHist1DRslt.nHistNo = 0;
		return -1;
	}
	GetAccHist(plData,nDataNo,&AdvHist1DRslt);
	GetConvHist(&AdvHist1DRslt,
				nHistConv,
				pAdvConvHist1DRslt);

	return 0;
}


int CAdvHistogram::GetConvHist(ADV_HIST_1D_RSLT* pAdvHist1DRslt,
							   int nHistConv,
							   ADV_CONV_HIST_1D_RSLT* pAdvConvHist1DRslt)
{
	int i,j;
	int nStartCoor;
	int nStartIndex;
	int nStartIndexBak;
	int nEndCoor;
	int nEndIndex;
	int nEndIndexBak;
	int nHistNo;
	int nConvPosTempNo;
	int nConvPosNo;
	int nCurrentPos;
	bool bAscend;
	int nConvHistNo;
	int nPOSHistConv;
	int nNEGHistConv;
	int nPosDif;
	bool bEndFound;
	int nStartHist;
	int nEndHist;
	int nMaxConvNo;
	long* plConvPosTemp;
	long* plConvPos;


	if(pAdvHist1DRslt->nHistNo <= 0) {
		ASSERT(!"pAdvHist1DRslt->nHistNo is less than or equal to 0");
		pAdvConvHist1DRslt->nHistNo = 0;
		return -1;
	}

	nPOSHistConv = nHistConv;
	nNEGHistConv = -nHistConv;
	nHistNo = pAdvHist1DRslt->nHistNo;
	nConvPosTempNo = 0;
	nMaxConvNo = 4 * nHistNo;
	pAdvConvHist1DRslt->MemLocate(nMaxConvNo);
	plConvPosTemp = new long[abs(nMaxConvNo)];
	plConvPos = new long[abs(nMaxConvNo)];
	for(i = 0; i < nHistNo; i++) {
		plConvPosTemp[nConvPosTempNo++] = pAdvHist1DRslt->plPos[i] - nHistConv-1;
		plConvPosTemp[nConvPosTempNo++] = pAdvHist1DRslt->plPos[i] - nHistConv;
		plConvPosTemp[nConvPosTempNo++] = pAdvHist1DRslt->plPos[i] + nHistConv;
		plConvPosTemp[nConvPosTempNo++] = pAdvHist1DRslt->plPos[i] + nHistConv+1;
	}
	bAscend = true;
	Sorting(&plConvPosTemp[0],nConvPosTempNo,bAscend);
	nCurrentPos = plConvPosTemp[0];
	plConvPos[0] = nCurrentPos;
	nConvPosNo = 1;
	for(i = 1; i < nConvPosTempNo; i++) {
		if(plConvPosTemp[i] != nCurrentPos) {
			plConvPos[nConvPosNo++] = plConvPosTemp[i];
			nCurrentPos = plConvPosTemp[i];
		}
	}
	nStartIndexBak = 0;
	nEndIndexBak = 0;
	nConvHistNo = 0;
	for(i = 0; i < (nConvPosNo - 1); i++) {
		nStartCoor = plConvPos[i];
		for(j = nStartIndexBak; j < nHistNo; j++) {
			nPosDif = pAdvHist1DRslt->plPos[j] - nStartCoor;
			if(nPosDif <= nPOSHistConv) {
				nStartIndex = j;
				nStartIndexBak = j;
				if(nPosDif >= nNEGHistConv) {
					if(j>0) {
						nStartIndex = j-1;
						nStartIndexBak = j-1;
					}
					break;
				}
			}
			else {
				nStartIndex = nStartIndexBak;
				break;
			}
		}
		if(j==0)
			nStartHist = 0;
		else
			nStartHist = pAdvHist1DRslt->pnHist[nStartIndex];
		nEndCoor =plConvPos[i + 1];
		bEndFound = false;
		for(j = nEndIndexBak; j < nHistNo; j++) {
			nPosDif = pAdvHist1DRslt->plPos[j] - nEndCoor;
			if(nPosDif >= nPOSHistConv) {
				if(j>0) {
					nEndIndex = j-1;
					nEndIndexBak = j-1;
					nEndHist = pAdvHist1DRslt->pnHist[nEndIndex];
				}
				else {
					nEndHist = 0;
					nEndIndex = nEndIndexBak;
				}
				break;
			}
			else 
				nEndHist = pAdvHist1DRslt->pnHist[j];
		}
		if(nEndHist > nStartHist) {
			pAdvConvHist1DRslt->pnHist[nConvHistNo] = nEndHist - nStartHist;
			pAdvConvHist1DRslt->plPosStart[nConvHistNo] = nStartCoor;
			pAdvConvHist1DRslt->plPosEnd[nConvHistNo] = nEndCoor;
			nConvHistNo++;
		}
	}
	pAdvConvHist1DRslt->nHistNo = nConvHistNo;
	delete [] plConvPosTemp;
	delete [] plConvPos;
	return 0;
}

int CAdvHistogram::GetFastConvHist1D(ADV_HIST_1D_RSLT* pAdvHist1DRslt,
									 int nHistConv,
									 ADV_CONV_HIST_1D_RSLT* pAdvConvHist1DRslt)
{
	int i, j;
	int nHoughVal;
	int nConvHistNo;
	int nCurrentCoor;
	int nEndIndex;
	int nEndCoor;
	int nEndIndexBak;
	int nHistNo;
/////////////////////
// Not Completed
/////////////////////
	return -1;
	nEndIndexBak = 0;
///////////////////////

	nHistNo = pAdvHist1DRslt->nHistNo;
	nEndCoor = pAdvHist1DRslt->plPos[0] + nHistConv;
	for(i = 0;i < nHistNo; i++ ) {
		if(pAdvHist1DRslt->plPos[i] > nEndCoor) 
			break;
	}
	nConvHistNo = 0;
	nEndIndex = i-1;
	pAdvConvHist1DRslt->pnHist[nConvHistNo] = pAdvHist1DRslt->pnHist[nEndIndex];
	pAdvConvHist1DRslt->plPosStart[nConvHistNo] = 0;
	pAdvConvHist1DRslt->plPosEnd[nConvHistNo] = nEndIndex;
	nConvHistNo++;
	for(i = 1; i < nHistNo; i++) {
		nCurrentCoor = pAdvHist1DRslt->plPos[i];
		nEndCoor = nCurrentCoor + nHistConv;
		for(j = nEndIndexBak; j < nHistNo; j++) {
			if(pAdvHist1DRslt->plPos[j] > nEndCoor)
				break;
		}
		nEndIndex = j-1;
		nEndIndexBak = nEndIndex;
		nHoughVal = pAdvHist1DRslt->pnHist[nEndIndex] - pAdvHist1DRslt->pnHist[i-1];
		pAdvConvHist1DRslt->pnHist[nConvHistNo] = nHoughVal;
		pAdvConvHist1DRslt->plPosStart[nConvHistNo] = i-1;
		pAdvConvHist1DRslt->plPosEnd[nConvHistNo] = nEndIndex;
		nConvHistNo++;
		if(j == nHistNo)
			break;
	}
	pAdvConvHist1DRslt->nHistNo = nConvHistNo;
	return 0;
}


int CAdvHistogram::GetMaxConvHist2D(CPoint2D *ppt2DInsp,
									int nInspNo,
									CSize2D* pcs2DHistConv,
									CPoint2D *ppt2DSelected,
									int &nSelectedSize,
									CPoint2D *ppt2DNOTSelected,
									int &nNOTSelectedSize)
{
	int i;
	long *plSrcX;
	long *plSrcY;
	CPoint ptTest;
	int nTimesX;
	int nTimesY;
	int nHistSize;
	double dHistConvX;
	double dHistConvY;
	int nHistConvX;
	int nHistConvY;

	dHistConvX = pcs2DHistConv->cx;
	dHistConvY = pcs2DHistConv->cy;
	if(dHistConvX < 0.01) {
		ASSERT(!"dHistConvX is less than 0.01");
		return -1;
	}
	if(dHistConvY < 0.01) {
		ASSERT(!"dHistConvY is less than 0.01");
		return -1;
	}
	nTimesX =1;
	while(dHistConvX < 5) {
		nTimesX *= 10;
		dHistConvX *= 10;
		if(nTimesX > 100) {
			nTimesX = 100;
			if(dHistConvX < 1)
				dHistConvX = 1.0;
			break;
		}
	};
	nTimesY =1;
	while(dHistConvY < 5) {
		nTimesY *= 10;
		dHistConvY *= 10;
		if(nTimesY > 100) {
			nTimesY = 100;
			if(dHistConvY < 1)
				dHistConvY = 1.0;
			break;
		}
	};

	nHistConvX = (int)dHistConvX;
	nHistConvY = (int)dHistConvY;

	plSrcX = new long[abs(nInspNo)];
	plSrcY = new long[abs(nInspNo)];
	for(i = 0; i < nInspNo; i++) {
		plSrcX[i] = (long)(ppt2DInsp[i].x * nTimesX);
		plSrcY[i] = (long)(ppt2DInsp[i].y * nTimesY);
	}

	ProjX.nHistConv = nHistConvX;
	ProjY.nHistConv = nHistConvY;
	Proj2DX(plSrcX, plSrcY, nInspNo, nHistConvX);

//	Proj2DY(plSrcX, plSrcY, nInspNo, nHistConvY);
	ProjResult.nFoundMaxPt = 0;
	nHistSize = ProjX.nHistSize;
	if(nHistSize < ProjY.nHistSize)
		nHistSize = ProjY.nHistSize;
	for(i = 0; i < nHistSize; i++) {
		if(ProjResult.nFoundMaxPt > ProjX.pnHistConvSort[i])
			break;
		else {
			if(i < ProjX.nHistSize) {
				ProjSearch.nHistConv = ProjY.nHistConv;
				FindMaxProj2DX(i);
			}
		}
/*
		if(ProjResult.nFoundMaxPt > ProjY.pnHistConvSort[i])
			break;
		else {
			if(i < ProjY.nHistSize) {
				ProjSearch.dHistConv = ProjX.dHistConv;
				FindMaxProj2DY(i);
			}
		}
*/
	}
	nSelectedSize = 0;
	nNOTSelectedSize = 0;
	for(i = 0; i < nInspNo; i++) {
		ptTest = CPoint(plSrcX[i],plSrcY[i]);
		if(CheckPoint(&ptTest, &ProjResult.rcFound)) 
			ppt2DNOTSelected[nNOTSelectedSize++] = ppt2DInsp[i];
		else
			ppt2DSelected[nSelectedSize++] = ppt2DInsp[i];
	}
	delete [] plSrcX;
	delete [] plSrcY;
	return 0;
}

int CAdvHistogram::FindMaxProj2DX(int nProjIndex)
{
/*	int j,k;
	int nCurrentIndex, nEndIndex;
	int nCurrentPos, nEndPos;
	int nPtCount;
	int nInspNo;
	nInspNo = ProjX.nptSize;
	ProjSearch.MemLocate(nInspNo);

	nCurrentPos = ProjX.pnHistPosSort[nProjIndex];
	nEndPos = nCurrentPos + (int)ProjX.dHistConv;
	nCurrentIndex = ProjX.pnHistConvSortIndex[nProjIndex];
	for(j = nCurrentIndex; j < ProjX.nHistSize; j++) {
		if(ProjX.pnHistPos[j] > nEndPos)
			break;
	}
	nEndIndex = j;
	nPtCount = 0;
	for(j = nCurrentIndex; j < nEndIndex; j++) {
		for(k = 0; k < ProjX.pPROJX[j].nYSize; k++) {
			ProjSearch.pnData[nPtCount++] = ProjX.pPROJX[j].pnY[k];
		}
	}
	ProjSearch.nptSize = nPtCount;

	GetMaxConvHist1D(ProjSearch.pnData, 
					 ProjSearch.nptSize,
					 (int)ProjSearch.dHistConv,
					 ProjSearch.nMaxCovHist,
					 ProjSearch.nMaxCovHistPos);

	if(ProjResult.nFoundMaxPt < ProjSearch.nMaxCovHist) {
		ProjResult.nFoundMaxPt = ProjSearch.nMaxCovHist;
		ProjResult.nStartProjIndex = nCurrentIndex;
		ProjResult.nEndProjIndex = nEndIndex;
		ProjResult.rcFound = CRect(nCurrentPos,
								   ProjSearch.nMaxCovHistPos - int(ProjX.dHistConv/2 +0.5),
								   nEndPos + 1,
								   ProjSearch.nMaxCovHistPos + int(ProjX.dHistConv/2 +0.5) + 1);
		ProjResult.bFoundByProjX = 1;
	}
*/
	return 0;
}

int CAdvHistogram::FindMaxProj2DY(int nProjIndex)
{
/*	int j,k;
	int nCurrentIndex, nEndIndex;
	int nCurrentPos, nEndPos;
	int nPtCount;
	int nInspNo;

	nInspNo = ProjY.nptSize;
	ProjSearch.MemLocate(nInspNo);
	nCurrentPos = ProjY.pnHistPosSort[nProjIndex];
	nEndPos = nCurrentPos + (int)ProjY.dHistConv;
	nCurrentIndex = ProjY.pnHistConvSortIndex[nProjIndex];
	for(j = nCurrentIndex; j < ProjY.nHistSize; j++) {
		if(ProjY.pnHistPos[j] > nEndPos)
			break;
	}
	nEndIndex = j;
	nPtCount = 0;
	for(j = nCurrentIndex; j < nEndIndex; j++) {
		for(k = 0; k < ProjY.pPROJY[j].nXSize; k++) {
			ProjSearch.pnData[nPtCount++] = ProjY.pPROJY[j].pnX[k];
		}
	}
	ProjSearch.nptSize = nPtCount;
	GetMaxConvHist1D(ProjSearch.pnData, 
					 ProjSearch.nptSize,
					 (int)ProjSearch.dHistConv,
					 ProjSearch.nMaxCovHist,
					 ProjSearch.nMaxCovHistPos);
	if(ProjResult.nFoundMaxPt < ProjSearch.nMaxCovHist) {
		ProjResult.nFoundMaxPt = ProjSearch.nMaxCovHist;
		ProjResult.nStartProjIndex = nCurrentIndex;
		ProjResult.nEndProjIndex = nEndIndex;
		ProjResult.rcFound = CRect(ProjSearch.nMaxCovHistPos - int(ProjY.dHistConv/2 +0.5),
								   nCurrentPos,
								   ProjSearch.nMaxCovHistPos + int(ProjY.dHistConv/2 +0.5) + 1,
								   nEndPos + 1);
		ProjResult.bFoundByProjX = 0;
	}
*/
	return 0;
}


int CAdvHistogram::Proj2DX(long* plX, long* plY, int nInspNo, int nHistConv)
{
	int i, j;
	int* pnSort;
	int nStart, nEnd;
	bool bAscend;
	ADV_HIST_1D_RSLT AdvHist1DRslt;
	bAscend = TRUE;
	pnSort = new int[abs(nInspNo)];

	ProjX.MemLocate(nInspNo);
	for(i = 0; i < nInspNo; i++)
		ProjX.pnSortX[i] = plX[i]; 
	Sorting(ProjX.pnSortX,nInspNo,pnSort,bAscend);
	for(i = 0; i < nInspNo; i++)
		ProjX.pnY[i] = plY[pnSort[i]]; 
	ProjX.nptSize = nInspNo;
	ProjX.nHistConv = nHistConv;
	GetAccHist(ProjX.pnSortX,
			   ProjX.nptSize, 
			   &AdvHist1DRslt);
	nStart = 0;
	nEnd = 0;
	ProjX.nHistSize = AdvHist1DRslt.nHistNo;
	for(i = 0; i < ProjX.nHistSize; i++) {
		ProjX.pPROJX[i].nProjX = AdvHist1DRslt.plPos[i];
		if(i==0) {
			ProjX.pPROJX[i].nYSize = AdvHist1DRslt.pnHist[0];
			nEnd = AdvHist1DRslt.pnHist[0];
		}
		else {
			nStart = AdvHist1DRslt.pnHist[i-1];
			nEnd = AdvHist1DRslt.pnHist[i];
			ProjX.pPROJX[i].nYSize = nEnd - nStart;
		}
		for(j = nStart; j < nEnd; j++) {
			ProjX.pPROJX[i].pnY[j - nStart] = ProjX.pnY[j];
		}
	}
	GetFastConvHist1D(&AdvHist1DRslt,
					  nHistConv,
					  &AdvConvHist1DXRslt);

	ProjX.nHistSize = AdvConvHist1DXRslt.nHistNo;
	for(i = 0; i < ProjX.nHistSize; i++) {
		ProjX.pnHistConvSort[i] = AdvConvHist1DXRslt.pnHist[i]; 
	}
	bAscend = FALSE;
	Sorting(ProjX.pnHistConvSort,ProjX.nHistSize,pnSort,bAscend);
/*
	for(i = 0; i < ProjX.nHistSize; i++) {
		pnSort[i] = ProjX.pnHistPos[i]; 
	}
	for(i = 0; i < ProjX.nHistSize; i++) {
		ProjX.pnHistPosSort[i] = pnSort[ProjX.pnHistConvSortIndex[i]]; 
	}
*/
	delete [] pnSort;

	return 0;
}

int CAdvHistogram::Proj2DY(long* plX, long* plY, int nInspNo, int nHistConv)
{
/*	int i, j;

	long* plXBak;
	long* plYBak;
	int* pnSort;
	int nStart, nEnd;
	bool bAscend;
	bAscend = TRUE;
	plXBak = new long[nInspNo];
	plYBak = new long[nInspNo];
	pnSort = new int[nInspNo];
	ProjY.MemLocate(nInspNo);

	for(i = 0; i < nInspNo; i++) {
		plXBak[i] = plX[i]; 
		plYBak[i] = plY[i]; 
	}
	Sorting(plYBak,nInspNo,ProjY.pnSortYIndex,bAscend);

	for(i = 0; i < nInspNo; i++) {
		ProjY.pnX[i] = plXBak[ProjY.pnSortYIndex[i]]; 
		ProjY.pnSortY[i] = plYBak[i]; 
	}
	ProjY.nptSize = nInspNo;
	ProjY.dHistConv = dHistConv;
*/
/*
	GetAccHist(ProjY.pnSortY,
			   ProjY.nptSize, 
			   ProjY.pnHistPos, 
			   ProjY.pnAccHist,
			   ProjY.nHistSize);

	nStart = 0;
	nEnd = 0;
	for(i = 0; i < ProjY.nHistSize; i++) {
		ProjY.pPROJY[i].nProjY = ProjY.pnHistPos[i];
		if(i==0) {
			ProjY.pPROJY[i].nXSize = ProjY.pnAccHist[0];
			nEnd = ProjY.pnAccHist[0];
		}
		else {
			nStart = ProjY.pnAccHist[i-1];
			nEnd = ProjY.pnAccHist[i];
			ProjY.pPROJY[i].nXSize = nEnd - nStart;
		}
		for(j = nStart; j < nEnd; j++) {
			ProjY.pPROJY[i].pnX[j - nStart] = ProjY.pnX[j];
		}
	}

	GetConvHist(ProjY.pnHistPos, 
			    ProjY.pnAccHist,
			    ProjY.nHistSize,
				ProjY.dHistConv,
				ProjY.pnHistConv,
				ProjY.nMaxCovHist,
				ProjY.nMaxCovHistPos);
	for(i = 0; i < ProjY.nHistSize; i++) {
		ProjY.pnHistConvSort[i] = ProjY.pnHistConv[i]; 
	}
	bAscend = FALSE;
	Sorting(ProjY.pnHistConvSort,ProjY.nHistSize,ProjY.pnHistConvSortIndex,bAscend);
	for(i = 0; i < ProjY.nHistSize; i++) {
		pnSort[i] = ProjY.pnHistPos[i]; 
	}
	for(i = 0; i < ProjY.nHistSize; i++) {
		ProjY.pnHistPosSort[i] = pnSort[ProjY.pnHistConvSortIndex[i]]; 
	}

	delete [] plXBak;
	delete [] plYBak;
	delete [] pnSort;
*/
	return 0;
}

CRECTANGLE2D_FIND_PARM::CRECTANGLE2D_FIND_PARM(){
	cs2DRect = CSize2D(0,0);
	csRectVarTol = CSize2D(10,10);
	csEdgePtVarTol= CSize2D(3,3);
	nTopSampleStep = 1;
	nTopPercentage = 30;
	nBotSampleStep = 1;
	nBotPercentage = 30;
	nLftSampleStep = 1;
	nLftPercentage = 30;
	nRhtSampleStep = 1;
	nRhtPercentage = 30;
	dNominalAngle = 0;
	dMaxTiltAngle = 5 * PI/180;
	dAngleStep = PI/180;
}

CRECTANGLE2D_FIND_PARM::~CRECTANGLE2D_FIND_PARM(){
}


CRECTANGLE2D_FOUND_INFO::CRECTANGLE2D_FOUND_INFO(){
	rc2DRect = CRectangle2D(CRect(0,0,0,0));
	nTopPercentage = 0;
	nBotPercentage = 0;
	nLftPercentage = 0;
	nRhtPercentage = 0;
}

CRECTANGLE2D_FOUND_INFO::~CRECTANGLE2D_FOUND_INFO(){
}


CRECTANGLE2D_FOUND_RSLT::CRECTANGLE2D_FOUND_RSLT(){
	pRect2DFndInfo = NULL;
	nRect2DFndNo = 0;
}

CRECTANGLE2D_FOUND_RSLT::~CRECTANGLE2D_FOUND_RSLT(){
	if(	pRect2DFndInfo != NULL)
		delete [] pRect2DFndInfo;
}


CPackageEdgeSelectedPoints::CPackageEdgeSelectedPoints(){
	nLftScore = 0;
	nTopScore = 0;
	nRhtScore = 0;
	nBotScore = 0;
	nSumScore = 0;
	nLft = 0;
	nTop = 0;
	nRht = 0;
	nBot = 0;
	dAngle = 0;
	pbLftIndex = NULL;
	pbRhtIndex = NULL;
	pbTopIndex = NULL;
	pbBotIndex = NULL;
}
CPackageEdgeSelectedPoints::~CPackageEdgeSelectedPoints(){
	Clean();
}
void CPackageEdgeSelectedPoints::Clean()
{
	nLftScore = 0;
	nTopScore = 0;
	nRhtScore = 0;
	nBotScore = 0;
	nSumScore = 0;
	nLft = 0;
	nTop = 0;
	nRht = 0;
	nBot = 0;
	dAngle = 0;
	if(pbLftIndex != NULL) {
		delete [] pbLftIndex;
		pbLftIndex = NULL;
	}
	if(pbTopIndex != NULL) {
		delete [] pbTopIndex;
		pbTopIndex = NULL;
	}
	if(pbRhtIndex != NULL) {
		delete [] pbRhtIndex;
		pbRhtIndex = NULL;
	}
	if(pbBotIndex != NULL) {
		delete [] pbBotIndex;
		pbBotIndex = NULL;
	}
}

CPackageEdgeSelectedPoints & CPackageEdgeSelectedPoints::operator =(CPackageEdgeSelectedPoints PackageEdgeSelectedPoints)
{
	int i;
	Clean();
	nLftScore = PackageEdgeSelectedPoints.nLftScore;
	nTopScore = PackageEdgeSelectedPoints.nTopScore;
	nRhtScore = PackageEdgeSelectedPoints.nRhtScore;
	nBotScore = PackageEdgeSelectedPoints.nBotScore;
	nSumScore = PackageEdgeSelectedPoints.nSumScore;
	nLft = PackageEdgeSelectedPoints.nLft;
	nTop = PackageEdgeSelectedPoints.nTop;
	nRht = PackageEdgeSelectedPoints.nRht;
	nBot = PackageEdgeSelectedPoints.nBot;
	dAngle = PackageEdgeSelectedPoints.dAngle;
	pbLftIndex = new bool[abs(nLft)];
	pbRhtIndex = new bool[abs(nRht)];
	pbTopIndex = new bool[abs(nTop)];
	pbBotIndex = new bool[abs(nBot)];
	for(i=0; i<nLft; i++) 
		pbLftIndex[i] = PackageEdgeSelectedPoints.pbLftIndex[i];
	for(i=0; i<nTop; i++) 
		pbTopIndex[i] = PackageEdgeSelectedPoints.pbTopIndex[i];
	for(i=0; i<nRht; i++) 
		pbRhtIndex[i] = PackageEdgeSelectedPoints.pbRhtIndex[i];
	for(i=0; i<nBot; i++) 
		pbBotIndex[i] = PackageEdgeSelectedPoints.pbBotIndex[i];
	return *this;
}



CRectangle2DFind::CRectangle2DFind(){
}

CRectangle2DFind::~CRectangle2DFind(){
}


int CRectangle2DFind::GetRectangle2D(CPoint2D* ppt2DInspTop,
									 int nInspTopNo,
									 CPoint2D* ppt2DInspBot,
									 int nInspBotNo,
									 CPoint2D* ppt2DInspLft,
									 int nInspLftNo,
									 CPoint2D* ppt2DInspRht,
									 int nInspRhtNo,
									 CRECTANGLE2D_FIND_PARM* pRect2DFindParm)
{
	int i,j,k;
	long *plLftSrcX;
	long *plLftSrcY;
	long *plRhtSrcX;
	long *plRhtSrcY;
	long *plTopSrcX;
	long *plTopSrcY;
	long *plBotSrcX;
	long *plBotSrcY;
	long *plLftSrcXBak;
	long *plLftSrcYBak;
	long *plRhtSrcXBak;
	long *plRhtSrcYBak;
	long *plTopSrcXBak;
	long *plTopSrcYBak;
	long *plBotSrcXBak;
	long *plBotSrcYBak;
	CPoint ptTest;
	double dHistConvX;
	double dHistConvY;
	int nHistConvX;
	int nHistConvY;
	int nError;
	double dAngle;
	int nAngle;
	double dSinAngle;
	double dCosAngle;
	CRect rcLftEdge;
	CRect rcTopEdge;
	CRect rcRhtEdge;
	CRect rcBotEdge;
	int nLftHist;
	int nTopHist;
	int nRhtHist;
	int nBotHist;

	int nSinAngleTimesX;
	int nSinAngleTimesY;
	int nCosAngleTimesX;
	int nCosAngleTimesY;
	CLine2D ln2DTop, ln2DBot, ln2DLft, ln2DRht;
	CPoint2D pt2DTopLft, pt2DTopRht, pt2DBotLft, pt2DBotRht;
	CPoint2D pt2DPkgCenter;
	CPoint2D pt2DTopCtr, pt2DBotCtr, pt2DLftCtr, pt2DRhtCtr;
	double dPkgWidth, dPkgHeight;
	double dPkgAngle;
	CPoint2D* ppt2DEdge;
	int nEdgeNo;

	int* pnSumScore;
	int nSumNo;
	int nSumScoreMax;
	int nSumScoreMaxIndex;
	int nSumScoreMaxIndexX;
	int nSumScoreMaxIndexY;

	double dAngleStep;
	double dMAXAngle;
	int nTotalEdgePoints;
	CArray<CRECTANGLE2D_FOUND_INFO,CRECTANGLE2D_FOUND_INFO> rc2DInfoAry;
	CRECTANGLE2D_FOUND_INFO rc2DFndInfo;

	int* pnLeftEdgeStartPos;
	int* pnLeftEdgeEndPos;
	int* pnRightEdgeStartPos;
	int* pnRightEdgeEndPos;
	int nLeftRightEdgePair;
	int* pnTopEdgeStartPos;
	int* pnTopEdgeEndPos;
	int* pnBottomEdgeStartPos;
	int* pnBottomEdgeEndPos;
	int nTopBottomEdgePair;
	int nCenterX, nCenterY;
	int nLftScanLine, nTopScanLine, nRhtScanLine, nBotScanLine;
	CPoint2D *ppt2DOnLine;
	int nOnLineSize;	
	CPoint2D *ppt2DNOTOnLine;
	int nNOTOnLineSize;		
	CHoughTransform HoughTransform;
	double dNominalAngleRad;
	double dTolAngleRad;		
	int nTopScore, nBotScore, nLftScore, nRhtScore;
	double dAngleHor, dAngleVer;
	double dWidthHeightRatio;
	bool bPass;

	bPass = true;
	if(pRect2DFindParm->nLftSampleStep <= 0)
		pRect2DFindParm->nLftSampleStep = 1;
	if(pRect2DFindParm->nRhtSampleStep <= 0)
		pRect2DFindParm->nRhtSampleStep = 1;
	if(pRect2DFindParm->nTopSampleStep <= 0)
		pRect2DFindParm->nTopSampleStep = 1;
	if(pRect2DFindParm->nBotSampleStep <= 0)
		pRect2DFindParm->nBotSampleStep = 1;
	nLftScanLine = (int)(pRect2DFindParm->cs2DRect.cy / pRect2DFindParm->nLftSampleStep);
	nRhtScanLine = (int)(pRect2DFindParm->cs2DRect.cy / pRect2DFindParm->nRhtSampleStep);
	nTopScanLine = (int)(pRect2DFindParm->cs2DRect.cx / pRect2DFindParm->nTopSampleStep);
	nBotScanLine = (int)(pRect2DFindParm->cs2DRect.cx / pRect2DFindParm->nBotSampleStep);
	
	nTopPercentage = (int)(pRect2DFindParm->cs2DRect.cx * pRect2DFindParm->nTopPercentage) / (pRect2DFindParm->nTopSampleStep * 100);
	if(nTopPercentage < 3) nTopPercentage = 3;
	nBotPercentage = (int)(pRect2DFindParm->cs2DRect.cx * pRect2DFindParm->nBotPercentage) / (pRect2DFindParm->nBotSampleStep * 100);
	if(nBotPercentage < 3) nBotPercentage = 3;
	nLftPercentage = (int)(pRect2DFindParm->cs2DRect.cy * pRect2DFindParm->nLftPercentage) / (pRect2DFindParm->nLftSampleStep * 100);
	if(nLftPercentage < 3) nLftPercentage = 3;
	nRhtPercentage = (int)(pRect2DFindParm->cs2DRect.cy * pRect2DFindParm->nRhtPercentage) / (pRect2DFindParm->nRhtSampleStep * 100);
	if(nRhtPercentage < 3) nRhtPercentage = 3;

	rc2DInfoAry.RemoveAll();
	rc2DInfoAry.Add(rc2DFndInfo);

	dHistConvX = pRect2DFindParm->csEdgePtVarTol.cx;
	dHistConvY = pRect2DFindParm->csEdgePtVarTol.cy;

	if(dHistConvX < 0.01) {
		ASSERT(!"dHistConvX is less than 0.01");
		return -1;
	}
	if(dHistConvY < 0.01) {
		ASSERT(!"dHistConvY is less than 0.01");
		return -1;
	}
	nTimesX =1;
	while(dHistConvX < 5) {
		nTimesX *= 10;
		dHistConvX *= 10;
		if(nTimesX > 100) {
			nTimesX = 100;
			if(dHistConvX < 1)
				dHistConvX = 1.0;
			break;
		}
	};
	nTimesY =1;
	while(dHistConvY < 5) {
		nTimesY *= 10;
		dHistConvY *= 10;
		if(nTimesY > 100) {
			nTimesY = 100;
			if(dHistConvY < 1)
				dHistConvY = 1.0;
			break;
		}
	};

	if(pRect2DFindParm->dAngleStep <= 0)
		pRect2DFindParm->dAngleStep = 0.5;
	nAngle =Round(pRect2DFindParm->dMaxTiltAngle / pRect2DFindParm->dAngleStep);
	if(nAngle <= 1)
		nAngle = 1;
	int nAngleSearchTimes;
	nAngleSearchTimes = 2 * nAngle + 1;
	pPackageEdgeSelectedPoints = new CPackageEdgeSelectedPoints[abs(nAngleSearchTimes)];
	for(i=0;i<nAngleSearchTimes;i++) {
		pPackageEdgeSelectedPoints[i].nTop = nInspTopNo;
		pPackageEdgeSelectedPoints[i].pbTopIndex = new bool[abs(nInspTopNo)];
		pPackageEdgeSelectedPoints[i].nBot = nInspBotNo;
		pPackageEdgeSelectedPoints[i].pbBotIndex = new bool[abs(nInspBotNo)];
		pPackageEdgeSelectedPoints[i].nLft = nInspLftNo;
		pPackageEdgeSelectedPoints[i].pbLftIndex = new bool[abs(nInspLftNo)];
		pPackageEdgeSelectedPoints[i].nRht = nInspRhtNo;
		pPackageEdgeSelectedPoints[i].pbRhtIndex = new bool[abs(nInspRhtNo)];
	}

	csRect = CSize((int)(pRect2DFindParm->cs2DRect.cx * nTimesX), (int)(pRect2DFindParm->cs2DRect.cy * nTimesY));
	csRectVarTol = CSize((int)(pRect2DFindParm->csRectVarTol.cx * nTimesX), (int)(pRect2DFindParm->csRectVarTol.cy * nTimesY));
	csEdgePtVarTol = CSize((int)(pRect2DFindParm->csEdgePtVarTol.cx * nTimesX), (int)(pRect2DFindParm->csEdgePtVarTol.cy * nTimesY));

	nHistConvX = csEdgePtVarTol.cx;
	nHistConvY = csEdgePtVarTol.cx;

	plLftSrcX = new long[abs(nInspLftNo)];
	plLftSrcY = new long[abs(nInspLftNo)];
	plRhtSrcX = new long[abs(nInspRhtNo)];
	plRhtSrcY = new long[abs(nInspRhtNo)];
	plTopSrcX = new long[abs(nInspTopNo)];
	plTopSrcY = new long[abs(nInspTopNo)];
	plBotSrcX = new long[abs(nInspBotNo)];
	plBotSrcY = new long[abs(nInspBotNo)];


	plLftSrcXBak = new long[abs(nInspLftNo)];
	plLftSrcYBak = new long[abs(nInspLftNo)];
	plRhtSrcXBak = new long[abs(nInspRhtNo)];
	plRhtSrcYBak = new long[abs(nInspRhtNo)];
	plTopSrcXBak = new long[abs(nInspTopNo)];
	plTopSrcYBak = new long[abs(nInspTopNo)];
	plBotSrcXBak = new long[abs(nInspBotNo)];
	plBotSrcYBak = new long[abs(nInspBotNo)];


	pnLeftEdgeStartPos = new int[abs(nInspLftNo * nInspLftNo)];
	pnLeftEdgeEndPos = new int[abs(nInspLftNo * nInspLftNo)];
	pnRightEdgeStartPos = new int[abs(nInspRhtNo * nInspRhtNo)];
	pnRightEdgeEndPos = new int[abs(nInspRhtNo * nInspRhtNo)];

	pnTopEdgeStartPos = new int[abs(nInspTopNo * nInspTopNo)];
	pnTopEdgeEndPos = new int[abs(nInspTopNo * nInspTopNo)];
	pnBottomEdgeStartPos = new int[abs(nInspBotNo * nInspBotNo)];
	pnBottomEdgeEndPos = new int[abs(nInspBotNo * nInspBotNo)];


	nTotalEdgePoints = nInspLftNo + nInspRhtNo + nInspTopNo + nInspBotNo;

	dMAXAngle = pRect2DFindParm->dMaxTiltAngle * PI/180;
	dAngleStep = pRect2DFindParm->dAngleStep * PI/180;
	dAngle = -dMAXAngle;

	for(i = -nAngle; i <= nAngle; i++) {
		dAngle += dAngleStep;
		dSinAngle = sin(dAngle);
		dCosAngle = cos(dAngle);
		nSinAngleTimesX = (int)(nTimesX * dSinAngle * 1000);
		nSinAngleTimesY = (int)(nTimesY * dSinAngle * 1000);
		nCosAngleTimesX = (int)(nTimesX * dCosAngle * 1000);
		nCosAngleTimesY = (int)(nTimesY * dCosAngle * 1000);

		for(j = 0; j < nInspLftNo; j++) {
			plLftSrcX[j] = (long)(ppt2DInspLft[j].x * nCosAngleTimesX + ppt2DInspLft[j].y * nSinAngleTimesY)/1000;
			plLftSrcXBak[j] = plLftSrcX[j];
		}
		for(j = 0; j < nInspRhtNo; j++) {
			plRhtSrcX[j] = (long)(ppt2DInspRht[j].x * nCosAngleTimesX + ppt2DInspRht[j].y * nSinAngleTimesY)/1000;
			plRhtSrcXBak[j] = plRhtSrcX[j];
		}
		nError = DistanceMatching(plLftSrcX, 
								  nInspLftNo,
								  plRhtSrcX,
								  nInspRhtNo,
								  csRectVarTol.cx,
								  csRect.cx,
								  csEdgePtVarTol.cx,
								  nLftPercentage,
								  nRhtPercentage,
								  &pnLeftEdgeStartPos[0],
								  &pnLeftEdgeEndPos[0],
								  &pnRightEdgeStartPos[0],
								  &pnRightEdgeEndPos[0],
								  &nLeftRightEdgePair);

		if((nError == -1) || (nLeftRightEdgePair <= 0))
			continue;
		for(j = 0; j < nInspTopNo; j++) {
			plTopSrcY[j] = (long)(-ppt2DInspTop[j].x * nSinAngleTimesX + ppt2DInspTop[j].y * nCosAngleTimesY)/1000;
			plTopSrcYBak[j] = plTopSrcY[j];
		}
		for(j = 0; j < nInspBotNo; j++) {
			plBotSrcY[j] = (long)(-ppt2DInspBot[j].x * nSinAngleTimesX + ppt2DInspBot[j].y * nCosAngleTimesY)/1000;
			plBotSrcYBak[j] = plBotSrcY[j];
		}

		nError = DistanceMatching(plTopSrcY, 
								  nInspTopNo,
								  plBotSrcY,
								  nInspBotNo,
								  csRectVarTol.cy,
								  csRect.cy,
								  csEdgePtVarTol.cy,
								  nTopPercentage,
								  nBotPercentage,
								  &pnTopEdgeStartPos[0],
								  &pnTopEdgeEndPos[0],
								  &pnBottomEdgeStartPos[0],
								  &pnBottomEdgeEndPos[0],
								  &nTopBottomEdgePair);
		if((nError == -1) || (nTopBottomEdgePair <= 0))
			continue;
		for(j = 0; j < nInspLftNo; j++) {
			plLftSrcY[j] = (long)(-ppt2DInspLft[j].x * nSinAngleTimesX + ppt2DInspLft[j].y * nCosAngleTimesY)/1000;
			plLftSrcYBak[j] = plLftSrcY[j];
		}
		for(j = 0; j < nInspTopNo; j++) {
			plTopSrcX[j] = (long)( ppt2DInspTop[j].x * nCosAngleTimesX + ppt2DInspTop[j].y * nSinAngleTimesY)/1000;
			plTopSrcXBak[j] = plTopSrcX[j];
		}
		for(j = 0; j < nInspRhtNo; j++) {
			plRhtSrcY[j] = (long)(-ppt2DInspRht[j].x * nSinAngleTimesX + ppt2DInspRht[j].y * nCosAngleTimesY)/1000;
			plRhtSrcYBak[j] = plRhtSrcY[j];
		}
		for(j = 0; j < nInspBotNo; j++) {
			plBotSrcX[j] = (long)( ppt2DInspBot[j].x * nCosAngleTimesX + ppt2DInspBot[j].y * nSinAngleTimesY)/1000;
			plBotSrcXBak[j] = plBotSrcX[j];
		}

		pnSumScore = new int[abs(nLeftRightEdgePair * nTopBottomEdgePair)];
		nSumNo = nLeftRightEdgePair * nTopBottomEdgePair;
		for(j = 0; j < nLeftRightEdgePair; j++) {
			nCenterX = (pnLeftEdgeStartPos[j] + pnLeftEdgeEndPos[j] + pnRightEdgeStartPos[j] + pnRightEdgeEndPos[j])/4;
			for(k = 0; k < nTopBottomEdgePair; k++) {
				pnSumScore[j*nTopBottomEdgePair + k] = 0;
				nCenterY = (pnTopEdgeStartPos[k] + pnTopEdgeEndPos[k] + pnBottomEdgeStartPos[k] + pnBottomEdgeEndPos[k])/4;
				rcLftEdge = CRect(pnLeftEdgeStartPos[j],
								  nCenterY - csRect.cy/2,
								  pnLeftEdgeEndPos[j],
								  nCenterY + csRect.cy/2);
				nLftHist = GetVerticalEdgeHist(plLftSrcXBak, 
									   plLftSrcYBak, 
									   nInspLftNo,
									   csEdgePtVarTol.cx,
									   &rcLftEdge);
				if(nLftHist < nLftPercentage)
					continue;
				rcTopEdge = CRect(nCenterX - csRect.cx/2,
								  pnTopEdgeStartPos[k],
								  nCenterX + csRect.cx/2,
								  pnTopEdgeEndPos[k]);
				nTopHist = GetHorizontalEdgeHist(plTopSrcXBak, 
									   plTopSrcYBak, 
									   nInspTopNo,
									   csEdgePtVarTol.cy,
									   &rcTopEdge);
				if(nTopHist < nTopPercentage)
					continue;
				rcRhtEdge = CRect(pnRightEdgeStartPos[j],
								  nCenterY - csRect.cy/2,
								  pnRightEdgeEndPos[j],
								  nCenterY + csRect.cy/2);
				nRhtHist = GetVerticalEdgeHist(plRhtSrcXBak, 
									   plRhtSrcYBak, 
									   nInspRhtNo,
									   csEdgePtVarTol.cx,
									   &rcRhtEdge);
				if(nRhtHist < nRhtPercentage)
					continue;
				rcBotEdge = CRect(nCenterX - csRect.cx/2,
								  pnBottomEdgeStartPos[k],
								  nCenterX + csRect.cx/2,
								  pnBottomEdgeEndPos[k]);
				nBotHist = GetHorizontalEdgeHist(plBotSrcXBak, 
									   plBotSrcYBak, 
									   nInspBotNo,
									   csEdgePtVarTol.cy,
									   &rcBotEdge);
				if(nBotHist < nBotPercentage)
					continue;
				nTopScore = (nTopHist*100)/nTopScanLine;
				if(nTopScore > 100)	nTopScore = 100;
				nBotScore = (nBotHist*100)/nBotScanLine;
				if(nBotScore > 100)	nBotScore = 100;
				nLftScore = (nLftHist * 100)/nLftScanLine;
				if(nLftScore > 100)	nLftScore = 100;
				nRhtScore = (nRhtHist*100)/nRhtScanLine;
				if(nRhtScore > 100)	nRhtScore = 100;
				pnSumScore[j*nTopBottomEdgePair + k] = nTopScore + nBotScore + nLftScore + nRhtScore;
			}
		}
		nSumScoreMax = 0;
		nSumScoreMaxIndex = 0;
		nSumScoreMaxIndexX = 0;
		nSumScoreMaxIndexY = 0;
		for(j=0;j<nSumNo;j++) {
			if(nSumScoreMax < pnSumScore[j]) {
				nSumScoreMax = pnSumScore[j];
				nSumScoreMaxIndex = j;
			}
		}
		pPackageEdgeSelectedPoints[i+nAngle].nSumScore = nSumScoreMax;
		if(nSumScoreMax > 0) {
			nSumScoreMaxIndexX = nSumScoreMaxIndex / nTopBottomEdgePair;
			nSumScoreMaxIndexY = nSumScoreMaxIndex % nTopBottomEdgePair;
			nCenterX = (pnLeftEdgeStartPos[nSumScoreMaxIndexX] + pnLeftEdgeEndPos[nSumScoreMaxIndexX] + pnRightEdgeStartPos[nSumScoreMaxIndexX] + pnRightEdgeEndPos[nSumScoreMaxIndexX])/4;
			nCenterY = (pnTopEdgeStartPos[nSumScoreMaxIndexY] + pnTopEdgeEndPos[nSumScoreMaxIndexY] + pnBottomEdgeStartPos[nSumScoreMaxIndexY] + pnBottomEdgeEndPos[nSumScoreMaxIndexY])/4;
			rcLftEdge = CRect(pnLeftEdgeStartPos[nSumScoreMaxIndexX],
							  nCenterY - csRect.cy/2,
							  pnLeftEdgeEndPos[nSumScoreMaxIndexX],
							  nCenterY + csRect.cy/2);
			GetVerticalEdgeIndex(plLftSrcXBak, 
						 plLftSrcYBak, 
						 nInspLftNo,
						 &rcLftEdge,
						 csEdgePtVarTol.cx,
						 &pPackageEdgeSelectedPoints[i+nAngle].pbLftIndex[0]);
			rcTopEdge = CRect(nCenterX - csRect.cx/2,
							  pnTopEdgeStartPos[nSumScoreMaxIndexY],
							  nCenterX + csRect.cx/2,
							  pnTopEdgeEndPos[nSumScoreMaxIndexY]);
			GetHorizontalEdgeIndex(plTopSrcXBak, 
						 plTopSrcYBak, 
						 nInspTopNo,
						 &rcTopEdge,
						 csEdgePtVarTol.cy,
						 &pPackageEdgeSelectedPoints[i+nAngle].pbTopIndex[0]);
			rcRhtEdge = CRect(pnRightEdgeStartPos[nSumScoreMaxIndexX],
							  nCenterY - csRect.cy/2,
							  pnRightEdgeEndPos[nSumScoreMaxIndexX],
							  nCenterY + csRect.cy/2);
			GetVerticalEdgeIndex(plRhtSrcXBak, 
						 plRhtSrcYBak, 
						 nInspRhtNo,
						 &rcRhtEdge,
						 csEdgePtVarTol.cx,
						 &pPackageEdgeSelectedPoints[i+nAngle].pbRhtIndex[0]);
			rcBotEdge = CRect(nCenterX - csRect.cx/2,
							  pnBottomEdgeStartPos[nSumScoreMaxIndexY],
							  nCenterX + csRect.cx/2,
							  pnBottomEdgeEndPos[nSumScoreMaxIndexY]);
			GetHorizontalEdgeIndex(plBotSrcXBak, 
						 plBotSrcYBak, 
						 nInspBotNo,
						 &rcBotEdge,
						 csEdgePtVarTol.cy,
						 &pPackageEdgeSelectedPoints[i+nAngle].pbBotIndex[0]);
		}
		delete [] pnSumScore;
	}
	nSumScoreMax = 0;
	nSumScoreMaxIndex = 0;
	for(i=0; i<nAngleSearchTimes;i++) {
		if(nSumScoreMax < pPackageEdgeSelectedPoints[i].nSumScore) {
			nSumScoreMax = pPackageEdgeSelectedPoints[i].nSumScore;
			nSumScoreMaxIndex = i;
		}
	}
	if(nSumScoreMax > 0) {
		ppt2DEdge = new CPoint2D[abs(nInspTopNo)];
		ppt2DOnLine = new CPoint2D[abs(nInspTopNo)];
		ppt2DNOTOnLine = new CPoint2D[abs(nInspTopNo)];
		nEdgeNo = 0;
		for(i=0; i<nInspTopNo; i++) {
			if(pPackageEdgeSelectedPoints[nSumScoreMaxIndex].pbTopIndex[i]) 
				ppt2DEdge[nEdgeNo++] = ppt2DInspTop[i];
		}

		dNominalAngleRad = (nSumScoreMaxIndex - nAngle) * dAngleStep;
		dTolAngleRad = dAngleStep;
		HoughTransform.HoughTransformLine(&ppt2DEdge[0],
							nEdgeNo,				
							dNominalAngleRad,	
							dTolAngleRad,		
							dAngleStep,			
							pRect2DFindParm->csRectVarTol.cy,			
							ppt2DOnLine,		
							nOnLineSize,			
							ppt2DNOTOnLine,	
							nNOTOnLineSize);		
		if(nOnLineSize > 1) {
			ln2DTop.GetBestFittingLine(ppt2DOnLine,nOnLineSize);
			delete [] ppt2DEdge;
			delete [] ppt2DOnLine;
			delete [] ppt2DNOTOnLine;
		}
		else {
			bPass = false;
			delete [] ppt2DEdge;
			delete [] ppt2DOnLine;
			delete [] ppt2DNOTOnLine;
			goto ErrorAbort;
		}
		
		ppt2DEdge = new CPoint2D[abs(nInspBotNo)];
		ppt2DOnLine = new CPoint2D[abs(nInspBotNo)];
		ppt2DNOTOnLine = new CPoint2D[abs(nInspBotNo)];
		nEdgeNo = 0;
		for(i=0; i<nInspBotNo; i++) {
			if(pPackageEdgeSelectedPoints[nSumScoreMaxIndex].pbBotIndex[i]) 
				ppt2DEdge[nEdgeNo++] = ppt2DInspBot[i];
		}
		HoughTransform.HoughTransformLine(&ppt2DEdge[0],
							nEdgeNo,				
							dNominalAngleRad,	
							dTolAngleRad,		
							dAngleStep,			
							pRect2DFindParm->csRectVarTol.cy,			
							ppt2DOnLine,		
							nOnLineSize,			
							ppt2DNOTOnLine,	
							nNOTOnLineSize);		
		if(nOnLineSize > 1) {
			ln2DBot.GetBestFittingLine(ppt2DOnLine,nOnLineSize);
			delete [] ppt2DEdge;
			delete [] ppt2DOnLine;
			delete [] ppt2DNOTOnLine;
		}
		else {
			bPass = false;
			delete [] ppt2DEdge;
			delete [] ppt2DOnLine;
			delete [] ppt2DNOTOnLine;
			goto ErrorAbort;
		}
		
		dNominalAngleRad = (nSumScoreMaxIndex - nAngle) * dAngleStep + PI/2;

		ppt2DEdge = new CPoint2D[abs(nInspLftNo)];
		ppt2DOnLine = new CPoint2D[abs(nInspLftNo)];
		ppt2DNOTOnLine = new CPoint2D[abs(nInspLftNo)];
		nEdgeNo = 0;
		for(i=0; i<nInspLftNo; i++) {
			if(pPackageEdgeSelectedPoints[nSumScoreMaxIndex].pbLftIndex[i]) 
				ppt2DEdge[nEdgeNo++] = ppt2DInspLft[i];
		}
		HoughTransform.HoughTransformLine(&ppt2DEdge[0],
							nEdgeNo,				
							dNominalAngleRad,	
							dTolAngleRad,		
							dAngleStep,			
							pRect2DFindParm->csRectVarTol.cy,			
							ppt2DOnLine,		
							nOnLineSize,			
							ppt2DNOTOnLine,	
							nNOTOnLineSize);		
		if(nOnLineSize > 1) {
			ln2DLft.GetBestFittingLine(ppt2DOnLine,nOnLineSize);
			delete [] ppt2DEdge;
			delete [] ppt2DOnLine;
			delete [] ppt2DNOTOnLine;
		}
		else {
			bPass = false;
			delete [] ppt2DEdge;
			delete [] ppt2DOnLine;
			delete [] ppt2DNOTOnLine;
			goto ErrorAbort;
		}
		ppt2DEdge = new CPoint2D[abs(nInspRhtNo)];
		ppt2DOnLine = new CPoint2D[abs(nInspRhtNo)];
		ppt2DNOTOnLine = new CPoint2D[abs(nInspRhtNo)];
		nEdgeNo = 0;
		for(i=0; i<nInspRhtNo; i++) {
			if(pPackageEdgeSelectedPoints[nSumScoreMaxIndex].pbRhtIndex[i]) 
				ppt2DEdge[nEdgeNo++] = ppt2DInspRht[i];
		}
		HoughTransform.HoughTransformLine(&ppt2DEdge[0],
							nEdgeNo,				
							dNominalAngleRad,	
							dTolAngleRad,		
							dAngleStep,			
							pRect2DFindParm->csRectVarTol.cy,			
							ppt2DOnLine,		
							nOnLineSize,			
							ppt2DNOTOnLine,	
							nNOTOnLineSize);		
		if(nOnLineSize > 1) {
			ln2DRht.GetBestFittingLine(ppt2DOnLine,nOnLineSize);
			delete [] ppt2DEdge;
			delete [] ppt2DOnLine;
			delete [] ppt2DNOTOnLine;
		}
		else {
			bPass = false;
			delete [] ppt2DEdge;
			delete [] ppt2DOnLine;
			delete [] ppt2DNOTOnLine;
			goto ErrorAbort;
		}
		ln2DTop.GetIntercept(&ln2DLft, &pt2DTopLft);
		ln2DTop.GetIntercept(&ln2DRht, &pt2DTopRht);
		ln2DBot.GetIntercept(&ln2DLft, &pt2DBotLft);
		ln2DBot.GetIntercept(&ln2DRht, &pt2DBotRht);

		pt2DPkgCenter = (pt2DTopLft + pt2DTopRht + pt2DBotLft + pt2DBotRht)/4;
		dPkgWidth = (pt2DTopLft.GetDistance(pt2DTopRht) + pt2DBotLft.GetDistance(pt2DBotRht))/2;
		dPkgHeight = (pt2DTopLft.GetDistance(pt2DBotLft) + pt2DTopRht.GetDistance(pt2DBotRht))/2;
		pt2DTopCtr = (pt2DTopLft + pt2DTopRht)/2;
		pt2DBotCtr = (pt2DBotLft + pt2DBotRht)/2;
		pt2DLftCtr = (pt2DTopLft + pt2DBotLft)/2;
		pt2DRhtCtr = (pt2DTopRht + pt2DBotRht)/2;
		dAngleHor = atan2(pt2DRhtCtr.y - pt2DLftCtr.y, pt2DRhtCtr.x - pt2DLftCtr.x);
		dAngleVer = atan2(pt2DTopCtr.x - pt2DBotCtr.x, pt2DBotCtr.y - pt2DTopCtr.y);
		if(dPkgHeight == 0) {
			bPass = false;
			goto ErrorAbort;
		}
		dWidthHeightRatio = dPkgWidth / dPkgHeight;
		if((dWidthHeightRatio > 0.5) && (dWidthHeightRatio < 1.5))
			dPkgAngle = (dAngleHor + dAngleVer)/2;
		else {
			if(dWidthHeightRatio > 1)
				dPkgAngle = dAngleHor;
			else
				dPkgAngle = dAngleVer;
		}
		rc2DFound.SetSize(CSize2D(dPkgWidth, dPkgHeight));
		rc2DFound.ShiftTo(pt2DPkgCenter);
		rc2DFound.RotateTo(dPkgAngle);
	}
	else 
		bPass = false;

ErrorAbort: 
	delete [] plLftSrcX;
	delete [] plLftSrcY;
	delete [] plRhtSrcX;
	delete [] plRhtSrcY;
	delete [] plTopSrcX;
	delete [] plTopSrcY;
	delete [] plBotSrcX;
	delete [] plBotSrcY;
	delete [] plLftSrcXBak;
	delete [] plLftSrcYBak;
	delete [] plRhtSrcXBak;
	delete [] plRhtSrcYBak;
	delete [] plTopSrcXBak;
	delete [] plTopSrcYBak;
	delete [] plBotSrcXBak;
	delete [] plBotSrcYBak;

	delete [] pnLeftEdgeStartPos;
	delete [] pnLeftEdgeEndPos;
	delete [] pnRightEdgeStartPos;
	delete [] pnRightEdgeEndPos;
	delete [] pnTopEdgeStartPos;
	delete [] pnTopEdgeEndPos;
	delete [] pnBottomEdgeStartPos;
	delete [] pnBottomEdgeEndPos;
	for(i=0;i<nAngleSearchTimes;i++) {
		pPackageEdgeSelectedPoints[i].Clean();
	}
	delete [] pPackageEdgeSelectedPoints;
	if(bPass)
		return 0;
	else
		return -1;
}



int CRectangle2DFind::GetVerticalEdgeHist(long* plSrcX, 
										  long* plSrcY, 
										  int nInspNo,
										  int nEdgeVar,
										  CRect* prcEdge)
{
	int i;
	int nEdgeHist;
	CAdvHistogram AdvHistogram;
	int nMaxHist;
	long lMaxHistPos;
	long* plSrcXSelected;
	plSrcXSelected = new long[abs(nInspNo)];
	nEdgeHist = 0;
	for(i = 0; i < nInspNo; i++) {
		if((plSrcX[i] > prcEdge->left) && (plSrcX[i] < prcEdge->right)) {
			if((plSrcY[i] > prcEdge->top) && (plSrcY[i] < prcEdge->bottom)) {
				plSrcXSelected[nEdgeHist++] = plSrcX[i];
			}
		}
	}
	if(nEdgeHist > 0) {
		AdvHistogram.GetMaxConvHist1D(&plSrcXSelected[0], 
									  nEdgeHist,
									  nEdgeVar,
									  nMaxHist,
									  lMaxHistPos);
		delete [] plSrcXSelected;
		return nMaxHist;
	}
	else {
		delete [] plSrcXSelected;
		return 0;
	}
}



int CRectangle2DFind::GetHorizontalEdgeHist(long* plSrcX, 
										  long* plSrcY, 
										  int nInspNo,
										  int nEdgeVar,
										  CRect* prcEdge)
{
	int i;
	int nEdgeHist;
	CAdvHistogram AdvHistogram;
	int nMaxHist;
	long lMaxHistPos;
	long* plSrcYSelected;
	plSrcYSelected = new long[abs(nInspNo)];
	nEdgeHist = 0;
	for(i = 0; i < nInspNo; i++) {
		if((plSrcX[i] > prcEdge->left) && (plSrcX[i] < prcEdge->right)) {
			if((plSrcY[i] > prcEdge->top) && (plSrcY[i] < prcEdge->bottom)) {
				plSrcYSelected[nEdgeHist++] = plSrcY[i];
			}
		}
	}
	if(nEdgeHist > 0) {
		AdvHistogram.GetMaxConvHist1D(&plSrcYSelected[0], 
									  nEdgeHist,
									  nEdgeVar,
									  nMaxHist,
									  lMaxHistPos);
		delete [] plSrcYSelected;
		return nMaxHist;
	}
	else {
		delete [] plSrcYSelected;
		return 0;
	}
}



void CRectangle2DFind::GetVerticalEdgeIndex(long* plSrcX, 
								   long* plSrcY, 
								   int nInspNo,
								   CRect* prcEdge,
								   int nEdgeVar,
								   bool* pbSelected)
{
	int i;
	int nEdgeHist;
	long* plSrcXSelected;
	int nEdgeLeft, nEdgeRight;
	CAdvHistogram AdvHistogram;
	int nMaxHist;
	long lMaxHistPos;
	plSrcXSelected = new long[abs(nInspNo)];
	nEdgeHist = 0;
	for(i = 0; i < nInspNo; i++) {
		pbSelected[i] = false;
		if((plSrcX[i] >= prcEdge->left) && (plSrcX[i] <= prcEdge->right)) {
			if((plSrcY[i] >= prcEdge->top) && (plSrcY[i] <= prcEdge->bottom)) {
				plSrcXSelected[nEdgeHist++] = plSrcX[i];
			}
		}
	}
	if(nEdgeHist > 0) {
		AdvHistogram.GetMaxConvHist1D(&plSrcXSelected[0], 
									  nEdgeHist,
									  nEdgeVar,
									  nMaxHist,
									  lMaxHistPos);
		delete [] plSrcXSelected;
		nEdgeLeft = lMaxHistPos - nEdgeVar;
		nEdgeRight = lMaxHistPos + nEdgeVar;
		for(i = 0; i < nInspNo; i++) {
			if((plSrcX[i] >= nEdgeLeft) && (plSrcX[i] <= nEdgeRight)) {
				if((plSrcY[i] >= prcEdge->top) && (plSrcY[i] <= prcEdge->bottom)) {
					pbSelected[i] = true;
				}
			}
		}
	}
	else {
		delete [] plSrcXSelected;
	}
}

void CRectangle2DFind::GetHorizontalEdgeIndex(long* plSrcX, 
								   long* plSrcY, 
								   int nInspNo,
								   CRect* prcEdge,
								   int nEdgeVar,
								   bool* pbSelected)
{
	int i;
	int nEdgeHist;
	long* plSrcYSelected;
	int nEdgeTop, nEdgeBottom;
	CAdvHistogram AdvHistogram;
	int nMaxHist;
	long lMaxHistPos;
	plSrcYSelected = new long[abs(nInspNo)];
	nEdgeHist = 0;
	for(i = 0; i < nInspNo; i++) {
		pbSelected[i] = false;
		if((plSrcX[i] >= prcEdge->left) && (plSrcX[i] <= prcEdge->right)) {
			if((plSrcY[i] >= prcEdge->top) && (plSrcY[i] <= prcEdge->bottom)) {
				plSrcYSelected[nEdgeHist++] = plSrcY[i];
			}
		}
	}
	if(nEdgeHist > 0) {
		AdvHistogram.GetMaxConvHist1D(&plSrcYSelected[0], 
									  nEdgeHist,
									  nEdgeVar,
									  nMaxHist,
									  lMaxHistPos);
		delete [] plSrcYSelected;
		nEdgeTop = lMaxHistPos - nEdgeVar;
		nEdgeBottom = lMaxHistPos + nEdgeVar;
		for(i = 0; i < nInspNo; i++) {
			if((plSrcX[i] >= prcEdge->left) && (plSrcX[i] <= prcEdge->right)) {
				if((plSrcY[i] >= nEdgeTop) && (plSrcY[i] <= nEdgeBottom)) {
					pbSelected[i] = true;
				}
			}
		}
	}
	else {
		delete [] plSrcYSelected;
	}
}


int CRectangle2DFind::RangeMerge(int* pnStartPos,
								 int* pnEndPos,
								 int* pnNo)
{
	int i;
	int nNo;
	int nNewNo;
	nNo = *pnNo;
	if(nNo <= 0)
		return -1;
	if(nNo == 1)
		return 0;
	nNewNo = 0;
	for(i=1; i<nNo; i++) {
		if(pnEndPos[i-1] != pnStartPos[i]) {
			pnEndPos[nNewNo] = pnEndPos[i-1];
			nNewNo++;
			pnStartPos[nNewNo] = pnStartPos[i];
		}
	}
	pnEndPos[nNewNo] = pnEndPos[nNo-1];
	*pnNo = nNewNo + 1;
	return 0;
}

int CRectangle2DFind::DistanceMatching(long* plFirstEdgePos, 
									   int nFirstEdgePosNo,
									   long* plLastEdgePos,
									   int nLastEdgePosNo,
									   int nDistVar,
									   int nDist,
									   int nEdgePtVar,
									   int nFirstEdgeHistThreshold,
									   int nLastEdgeHistThreshold,
									   int* pnFoundFirstEdgeStartPos,
									   int* pnFoundFirstEdgeEndPos,
									   int* pnFoundLastEdgeStartPos,
									   int* pnFoundLastEdgeEndPos,
									   int* pnFoundEdgePair)
{
	int i, j;
	ADV_CONV_HIST_1D_RSLT AdvConvHist1DRslt;
	int nLastEdgeStart;
	bool bFirstFound;
	int nEstimateLastStartEdge;
	int nEstimateLastEndEdge;

	int* pnFirstEdgeStartPos;
	int* pnFirstEdgeEndPos;
	int nFirstEdgePos;
	int* pnLastEdgeStartPos;
	int* pnLastEdgeEndPos;
	int nLastEdgePos;
	int nFoundEdgePair;
	int nLftEdgeStartPosOverlap;
	int nLftEdgeEndPosOverlap;
	int nRhtEdgeStartPosOverlap;
	int nRhtEdgeEndPosOverlap;


	CAdvHistogram AdvHistogram;

	pnFirstEdgeStartPos = NULL;
	pnFirstEdgeEndPos = NULL;
	pnLastEdgeStartPos = NULL;
	pnLastEdgeEndPos = NULL;

	nFirstEdgePos = 0;
	nLastEdgePos = 0;
	AdvHistogram.GetConvHist(plFirstEdgePos,
							 nFirstEdgePosNo,
							 nEdgePtVar, 
							 &AdvConvHist1DRslt);
	pnFirstEdgeStartPos = new int[abs(AdvConvHist1DRslt.nHistNo)];
	pnFirstEdgeEndPos = new int[abs(AdvConvHist1DRslt.nHistNo)];
	for(i = 0; i < AdvConvHist1DRslt.nHistNo; i++) {
		if(AdvConvHist1DRslt.pnHist[i] > nFirstEdgeHistThreshold) {
			pnFirstEdgeStartPos[nFirstEdgePos] = AdvConvHist1DRslt.plPosStart[i];
			pnFirstEdgeEndPos[nFirstEdgePos] = AdvConvHist1DRslt.plPosEnd[i];
			nFirstEdgePos++;
		}
	}
	if(nFirstEdgePos == 0) 
		goto Abort;

	RangeMerge(&pnFirstEdgeStartPos[0],
			   &pnFirstEdgeEndPos[0],
			   &nFirstEdgePos);

	AdvHistogram.GetConvHist(plLastEdgePos,
							 nLastEdgePosNo,
							 nEdgePtVar, 
							 &AdvConvHist1DRslt);
	pnLastEdgeStartPos = new int[abs(AdvConvHist1DRslt.nHistNo)];
	pnLastEdgeEndPos = new int[abs(AdvConvHist1DRslt.nHistNo)];
	for(i = 0; i < AdvConvHist1DRslt.nHistNo; i++) {
		if(AdvConvHist1DRslt.pnHist[i] > nLastEdgeHistThreshold) {
			pnLastEdgeStartPos[nLastEdgePos] = AdvConvHist1DRslt.plPosStart[i];
			pnLastEdgeEndPos[nLastEdgePos] = AdvConvHist1DRslt.plPosEnd[i];
			nLastEdgePos++;
		}
	}
	RangeMerge(&pnLastEdgeStartPos[0],
			   &pnLastEdgeEndPos[0],
			   &nLastEdgePos);

	if(nLastEdgePos == 0) 
		goto Abort;

	nFoundEdgePair = 0;
	nLastEdgeStart = 0;
	for(i = 0; i < nFirstEdgePos; i++) {
		bFirstFound = false;
		nEstimateLastStartEdge = pnFirstEdgeStartPos[i] + nDist - nDistVar;
		nEstimateLastEndEdge = pnFirstEdgeEndPos[i] + nDist + nDistVar;
		for(j = nLastEdgeStart; j < nLastEdgePos; j++) {
			if(nEstimateLastEndEdge < pnLastEdgeStartPos[j])
				break;
			if(nEstimateLastStartEdge > pnLastEdgeEndPos[j])
				continue;
			if(FindOverlap(pnFirstEdgeStartPos[i], 
						   pnFirstEdgeEndPos[i],
						   pnLastEdgeStartPos[j],
						   pnLastEdgeEndPos[j],
						   nDist,
						   nDistVar,
						   &nLftEdgeStartPosOverlap,
						   &nLftEdgeEndPosOverlap,
						   &nRhtEdgeStartPosOverlap,
						   &nRhtEdgeEndPosOverlap) == 0) {
				pnFoundFirstEdgeStartPos[nFoundEdgePair] = nLftEdgeStartPosOverlap;
				pnFoundFirstEdgeEndPos[nFoundEdgePair] = nLftEdgeEndPosOverlap;
				pnFoundLastEdgeStartPos[nFoundEdgePair] = nRhtEdgeStartPosOverlap;
				pnFoundLastEdgeEndPos[nFoundEdgePair] = nRhtEdgeEndPosOverlap;
				nFoundEdgePair++;
				if(!bFirstFound) {
					nLastEdgeStart = j;
					bFirstFound = true;
				}
			}
		}
	}

	*pnFoundEdgePair = nFoundEdgePair;
	if(pnFirstEdgeStartPos != NULL)
		delete [] pnFirstEdgeStartPos;
	if(pnFirstEdgeEndPos != NULL)
		delete [] pnFirstEdgeEndPos;
	if(pnLastEdgeStartPos != NULL)
		delete [] pnLastEdgeStartPos;
	if(pnLastEdgeEndPos != NULL)
		delete [] pnLastEdgeEndPos;
	return 0;
Abort:
	*pnFoundEdgePair = 0;
	if(pnFirstEdgeStartPos != NULL)
		delete [] pnFirstEdgeStartPos;
	if(pnFirstEdgeEndPos != NULL)
		delete [] pnFirstEdgeEndPos;
	if(pnLastEdgeStartPos != NULL)
		delete [] pnLastEdgeStartPos;
	if(pnLastEdgeEndPos != NULL)
		delete [] pnLastEdgeEndPos;
	return -1;
}

int CRectangle2DFind::FindOverlap(int nLftEdgeStartPosSrc,
								  int nLftEdgeEndPosSrc,
								  int nRhtEdgeStartPosSrc,
								  int nRhtEdgeEndPosSrc,
								  int nWidth,
								  int nWidthVar,
								  int* pnLftEdgeStartPosDst,
								  int* pnLftEdgeEndPosDst,
								  int* pnRhtEdgeStartPosDst,
								  int* pnRhtEdgeEndPosDst)
{
	int nLftEdgeStartCalPos;
	int nLftEdgeEndCalPos;
	int nRhtEdgeStartCalPos;
	int nRhtEdgeEndCalPos;
	nLftEdgeStartCalPos = nRhtEdgeStartPosSrc - nWidth - nWidthVar;
	nLftEdgeEndCalPos = nRhtEdgeEndPosSrc - nWidth + nWidthVar;
	nRhtEdgeStartCalPos = nLftEdgeStartPosSrc + nWidth - nWidthVar;
	nRhtEdgeEndCalPos = nLftEdgeEndPosSrc + nWidth + nWidthVar;
	if(nLftEdgeStartCalPos > nLftEdgeStartPosSrc)
		*pnLftEdgeStartPosDst = nLftEdgeStartCalPos;
	else
		*pnLftEdgeStartPosDst = nLftEdgeStartPosSrc;
	if(nLftEdgeEndCalPos > nLftEdgeEndPosSrc)
		*pnLftEdgeEndPosDst = nLftEdgeEndPosSrc;
	else
		*pnLftEdgeEndPosDst = nLftEdgeEndCalPos;
	if(*pnLftEdgeStartPosDst > *pnLftEdgeEndPosDst) 
		return -1;
	if(nRhtEdgeStartCalPos > nRhtEdgeStartPosSrc)
		*pnRhtEdgeStartPosDst = nRhtEdgeStartCalPos;
	else
		*pnRhtEdgeStartPosDst = nRhtEdgeStartPosSrc;
	if(nRhtEdgeEndCalPos > nRhtEdgeEndPosSrc)
		*pnRhtEdgeEndPosDst = nRhtEdgeEndPosSrc;
	else
		*pnRhtEdgeEndPosDst = nRhtEdgeEndCalPos;
	if(*pnRhtEdgeStartPosDst > *pnRhtEdgeEndPosDst) 
		return -1;
	return 0;
}

CPoint3D::CPoint3D(){
	x=y=z=0;
}

CPoint3D::CPoint3D(CPoint p,double z0) {
	x=p.x;y=p.y;z=z0;
}


CPoint3D::CPoint3D(double x0, double y0,double z0) {
	x=x0;y=y0;z=z0;
}


CPoint3D::CPoint3D(const CPoint3D &p) {
	x=p.x;y=p.y;z=p.z;
	
}

CPoint3D & CPoint3D::operator =(CPoint3D pt3DSrc) {
	x=pt3DSrc.x;y=pt3DSrc.y;z=pt3DSrc.z;
	return *this;
}

CPoint3D & CPoint3D::operator +=(CPoint3D pt3DSrc) {
	x+=pt3DSrc.x;y+=pt3DSrc.y;z+=pt3DSrc.z;
	return *this;
}

CPoint3D & CPoint3D::operator -=(CPoint3D pt3DSrc) {
	x-=pt3DSrc.x;y-=pt3DSrc.y;z-=pt3DSrc.z;
	return *this;
}

CPoint3D & CPoint3D::operator *=(double d) {
	x*=d;y*=d;z*=d;
	return *this;
}

CPoint3D & CPoint3D::operator /=(double d) {
	if( d != 0) {
	    x = x / d;
		y = y / d;
		z = z / d;
	}
	else {
		if(x < 0) 
			x = -1.0E+50;
		else
			x = 1.0E+50;
		if(y < 0) 
			y = -1.0E+50;
		else
			y = 1.0E+50;
		if(z < 0) 
			z = -1.0E+50;
		else
			z = 1.0E+50;
	}
	return *this;
}

CPoint3D CPoint3D::operator + (CPoint3D p) {
	return CPoint3D(x+p.x,y+p.y,z+p.z);
}

CPoint3D CPoint3D::operator - (CPoint3D p) {
	return CPoint3D(x-p.x,y-p.y,z-p.z);
}

CPoint3D CPoint3D::operator * (double d) {
	return CPoint3D(x*d,y*d,z*d);
}

CPoint3D CPoint3D::operator / (double d) {
	if( d != 0) {
		return CPoint3D(x/d,y/d,z/d);
	}
	else {
		double dX1, dY1, dZ1;
		if(x < 0) 
			dX1 = -1.0E+50;
		else
			dX1 = 1.0E+50;
		if(y < 0) 
			dY1 = -1.0E+50;
		else
			dY1 = 1.0E+50;
		if(z < 0) 
			dZ1 = -1.0E+50;
		else
			dZ1 = 1.0E+50;
		return CPoint3D(dX1, dY1, dZ1);
	}
}

double CPoint3D::GetDistance(CPoint3D pt3D) {
	return sqrt(pow(x-pt3D.x,2)+pow(y-pt3D.y,2)+pow(z-pt3D.z,2));
}

BOOL CPoint3D::operator == (CPoint3D pt3D) {
	return (pt3D.x==x && pt3D.y==y && pt3D.z==z);
}

CVector3D::CVector3D() {
}

CVector3D::CVector3D(const CVector3D &vt3D):CPoint3D(vt3D) {
}

CVector3D::CVector3D(CPoint3D vt3DDst,CPoint3D vt3DSrc) {
	x=vt3DDst.x-vt3DSrc.x;
	y=vt3DDst.y-vt3DSrc.y;
	z=vt3DDst.z-vt3DSrc.z;
}


CVector3D::CVector3D(double x0,double y0,double z0):CPoint3D(x0,y0,z0) {
}


int CVector3D::Normalize()
{
	double dSize=GetSize();
	if(dSize) {
		x /= dSize;
		y /= dSize;
		z /= dSize;
		return 0;
	}
	else 
		return -1;
}

double CVector3D::GetSize() {
	return sqrt(x*x+y*y+z*z);
}


CVector3D & CVector3D::operator =(CVector3D vt3D) {
	x=vt3D.x;
	y=vt3D.y;
	z=vt3D.z;
	return *this;
}

int CVector3D::GetUnit(CVector3D* pvct3D) {
	double dSize=GetSize();
	if(dSize) {
		*pvct3D = CVector3D(x/dSize,y/dSize,z/dSize);
		return 0;
	}
	else 
		return -1;
}

CVector3D CVector3D::operator * (CVector3D v) {
	return CVector3D(y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x);
}

CVector3D CVector3D::operator * (double s) {
	return CVector3D(x*s,y*s,z*s);
}


double CVector3D::operator ^ (CVector3D v) {
	return x*v.x+y*v.y+z*v.z;
}


BOOL CVector3D::IsValid() {
	return (x==0&&y==0&&z==0);
}


int CVector3D::GetAngleRad(CVector3D v,
						   double& dAngleRad) {
	double dot,mag;
	if (!v.IsValid()&&!IsValid()) {
		dot=*this^v;
		mag=GetSize()*v.GetSize();
		dAngleRad=acos(dot/mag);
		return 0;
	}
	else {
		return -1;
	}
}

CRectangle2D::CRectangle2D() {
	m_Width=m_Height=0;
	m_pointCenter=CPoint2D(0,0);
	m_Angle=0;
}


CRectangle2D::CRectangle2D(CRectangle2D &rect) {
	m_pointCenter=rect.m_pointCenter;
	m_Width=rect.m_Width;
	m_Height=rect.m_Height;
	m_Angle=rect.m_Angle;
}

CRectangle2D::CRectangle2D(CPoint2D* ppt2D0,
						   CPoint2D* ppt2D1,
						   CPoint2D* ppt2D2,
						   CPoint2D* ppt2D3) {

	int i, j;
	double dAngle[4];
	CPoint2D pt2DCorner[4];
	CLine2D ln2DHor1, ln2DHor2, ln2DHorAvg;
	CLine2D ln2DVer1, ln2DVer2, ln2DVerAvg;
	CLine2D ln2DVerToHor;
	CLine2D ln2DAvg;

	double dDis[4][4];
	double dMaxDis;
	int nMaxDisPos1, nMaxDisPos2;

	CPoint2D pt2DPairH1[2];
	CPoint2D pt2DPairH2[2];
	CPoint2D pt2DPairV1[2];
	CPoint2D pt2DPairV2[2];
	int nAngleCount;
	int nAngleIndex[4];

	pt2DCorner[0] = *ppt2D0; 
	pt2DCorner[1] = *ppt2D1; 
	pt2DCorner[2] = *ppt2D2; 
	pt2DCorner[3] = *ppt2D3; 
	m_pointCenter=(*ppt2D0 + *ppt2D1 + *ppt2D2 + *ppt2D3)/4;
	for(i=0;i<4;i++)  
		for(j=0;j<4;j++)  
			dDis[i][j] = pt2DCorner[i].GetDistance(pt2DCorner[j]);
	dMaxDis = dDis[0][0];
	nMaxDisPos1 = 0;
	nMaxDisPos2 = 0;
	for(i=0;i<4;i++) {  
		for(j=0;j<4;j++) {  
			if(dMaxDis < dDis[i][j]) {
				dMaxDis = dDis[i][j];
				nMaxDisPos1 = i;
				nMaxDisPos2 = j;
			}
		}
	}

	nAngleCount = 0;
	for(i=0;i<4;i++) {
		if((i != nMaxDisPos1) && (i != nMaxDisPos2)) {
			dAngle[nAngleCount] = atan2(fabs(pt2DCorner[nMaxDisPos1].y - pt2DCorner[i].y), 
						  fabs(pt2DCorner[nMaxDisPos1].x - pt2DCorner[i].x));
			nAngleIndex[nAngleCount] = i;
			nAngleCount++;
		}
	}
	pt2DPairH1[0] = pt2DCorner[nMaxDisPos1];
	pt2DPairH2[0] = pt2DCorner[nMaxDisPos2];
	pt2DPairV1[0] = pt2DCorner[nMaxDisPos1];
	pt2DPairV2[0] = pt2DCorner[nMaxDisPos2];
	if(dAngle[0] < dAngle[1]) {
		pt2DPairH1[1] = pt2DCorner[nAngleIndex[0]];
		pt2DPairH2[1] = pt2DCorner[nAngleIndex[1]];
		pt2DPairV1[1] = pt2DCorner[nAngleIndex[1]];
		pt2DPairV2[1] = pt2DCorner[nAngleIndex[0]];
	}
	else {
		pt2DPairH1[1] = pt2DCorner[nAngleIndex[1]];
		pt2DPairH2[1] = pt2DCorner[nAngleIndex[0]];
		pt2DPairV1[1] = pt2DCorner[nAngleIndex[0]];
		pt2DPairV2[1] = pt2DCorner[nAngleIndex[1]];
	}

	ln2DHor1 = CLine2D(pt2DPairH1[0], pt2DPairH1[1]);
	ln2DHor2 = CLine2D(pt2DPairH2[0], pt2DPairH2[1]);
	ln2DVer1 = CLine2D(pt2DPairV1[0], pt2DPairV1[1]);
	ln2DVer2 = CLine2D(pt2DPairV2[0], pt2DPairV2[1]);
	ln2DHorAvg.GetBiSectLine(&ln2DHor1, &ln2DHor2);
	ln2DVerAvg.GetBiSectLine(&ln2DVer1, &ln2DVer2);
	ln2DVerAvg.GetPerpendicularLine(&ln2DVerAvg.GetPosition(), &ln2DVerToHor);
	ln2DAvg.GetBiSectLine(&ln2DHorAvg, &ln2DVerToHor);
	ln2DAvg.GetAngleRad(m_Angle);
	m_Width = (pt2DPairH1[0].GetDistance(pt2DPairH1[1]) + pt2DPairH2[0].GetDistance(pt2DPairH2[1]))/2.0;
	m_Height = (pt2DPairV1[0].GetDistance(pt2DPairV1[1]) + pt2DPairV2[0].GetDistance(pt2DPairV2[1]))/2.0;
}



CRectangle2D::CRectangle2D(CPoint2D* ppt2D0,
						   CPoint2D* ppt2D1,
						   CPoint2D* ppt2D2,
						   CPoint2D* ppt2D3,
						   CRect* prcROI,
						   BOOL* pbPass) 
{
	int i, j;
	CPoint2D pt2DInROI[4];
	int npt2DInROI;
	CPoint2D pt2DNOTInROI[4];
	int npt2DNOTInROI;
	CRectangle2D rc2DMax;
	CRectangle2D rc2DFound[4];
	int nrc2DFound;
	double dArea;
	double dAreaMax;
	int nAreaMaxIndex;
	CPoint2D pt2DOrg;
	CPoint2D pt2DOthers[3];
	int npt2DOthers;
	int nError;

	*pbPass = FALSE;
	npt2DInROI = 0;
	npt2DNOTInROI = 0;
	nError = ValidatePoint(ppt2D0, prcROI);
	if(nError == 0) 
		pt2DInROI[npt2DInROI++] = *ppt2D0;
	else
		pt2DNOTInROI[npt2DNOTInROI++] = *ppt2D0;
	nError = ValidatePoint(ppt2D1, prcROI);
	if(nError == 0) 
		pt2DInROI[npt2DInROI++] = *ppt2D1;
	else
		pt2DNOTInROI[npt2DNOTInROI++] = *ppt2D1;
	nError = ValidatePoint(ppt2D2, prcROI);
	if(nError == 0) 
		pt2DInROI[npt2DInROI++] = *ppt2D2;
	else
		pt2DNOTInROI[npt2DNOTInROI++] = *ppt2D2;
	nError = ValidatePoint(ppt2D3, prcROI);
	if(nError == 0) 
		pt2DInROI[npt2DInROI++] = *ppt2D3;
	else
		pt2DNOTInROI[npt2DNOTInROI++] = *ppt2D3;
	
	if(npt2DInROI == 4)
		rc2DMax = CRectangle2D(ppt2D0,ppt2D1,ppt2D2,ppt2D3);
	if(npt2DInROI == 0)
		goto ErrorAbort;
	nrc2DFound = 0;
	dAreaMax = 0;
	nAreaMaxIndex = 0;
	for(i=0;i<npt2DInROI;i++) {
		pt2DOrg = pt2DInROI[i];
		npt2DOthers = 0;
		for(j=0;j<npt2DInROI;j++) {
			if(i!=j)
			pt2DOthers[npt2DOthers++] = pt2DInROI[j];
		}
		for(j=0;j<npt2DNOTInROI;j++) 
			pt2DOthers[npt2DOthers++] = pt2DNOTInROI[j];
		nError = GetRect2DByFixOnePt(&pt2DOrg,
						   &pt2DOthers[0],
						   prcROI,
						   &rc2DFound[nrc2DFound]);
		if(nError)
			continue;
		dArea = rc2DFound[nrc2DFound].GetWidth() * rc2DFound[nrc2DFound].GetHeight();
		if(dAreaMax < dArea) {
			dAreaMax = dArea;
			nAreaMaxIndex = nrc2DFound;
		}
		nrc2DFound++;
	}
	rc2DMax = rc2DFound[nAreaMaxIndex];
	m_Width = rc2DMax.GetWidth();
	m_Height = rc2DMax.GetHeight();
	m_Angle = rc2DMax.GetAngle();
	m_pointCenter = rc2DMax.GetCenterPoint();
	*pbPass = TRUE;
	return;
ErrorAbort:
	*pbPass = FALSE;
	return;
}

CRectangle2D::CRectangle2D(CRectangle2D* prc2D,
			   CRect* prcROI,
			   BOOL* pbPass)
{
	CPoint2D pt2D[4];
	pt2D[0] = prc2D->GetDevTopLeft();
	pt2D[1] = prc2D->GetDevTopRight();
	pt2D[2] = prc2D->GetDevBottomRight();
	pt2D[3] = prc2D->GetDevBottomLeft();
	CRectangle2D(&pt2D[0],
				 &pt2D[1],
				 &pt2D[2],
				 &pt2D[3],
				 prcROI,
				 pbPass);
	return;
}

int CRectangle2D::GetLinePtsInROI(CPoint2D* ppt2DOrg,
						   CPoint2D* ppt2D,
						   CPoint2D* ppt2DInROI,
						   CRect* prcROI)
{
	int i;
	CPoint2D pt2DIntercept[4];
	CLine2D ln2D;
	double dDisOrg;
	double dDisSegment1;
	double dDisSegment2;
	double dDisSegment[4];
	CPoint2D pt2DInROI[4];
	int nFound;
	double dMax;
	int nMaxIndex;
	int nError;

	nError = ValidatePoint(ppt2DOrg, prcROI);
	if(nError)
		goto ErrorAbort;
	nError = ValidatePoint(ppt2D, prcROI);
	if(nError == 0) {
		*ppt2DInROI = *ppt2D;
		goto PASS;
	}
	dDisOrg = ppt2DOrg->GetDistance(*ppt2D);
	if(dDisOrg < 1)
		goto ErrorAbort;
	ln2D = CLine2D(*ppt2DOrg, *ppt2D);
	pt2DIntercept[0].x = prcROI->left + 1;
	pt2DIntercept[1].y = prcROI->top + 1;
	pt2DIntercept[2].x = prcROI->right - 2;
	pt2DIntercept[3].y = prcROI->bottom - 2;
	nError = ln2D.GetY(pt2DIntercept[0].x, &pt2DIntercept[0].y);
	nError = ln2D.GetX(pt2DIntercept[1].y, &pt2DIntercept[1].x);
	nError = ln2D.GetY(pt2DIntercept[2].x, &pt2DIntercept[2].y);
	nError = ln2D.GetX(pt2DIntercept[3].y, &pt2DIntercept[3].x);
	nFound = 0;
	for(i=0;i<4;i++) {
		nError = ValidatePoint(&pt2DIntercept[i], prcROI);
		if(nError == 0) {
			dDisSegment1 = ppt2DOrg->GetDistance(pt2DIntercept[i]);
			dDisSegment2 = ppt2D->GetDistance(pt2DIntercept[i]);
			if((dDisSegment1 <= dDisOrg) && ((dDisSegment2 <= dDisOrg))) {
				dDisSegment[nFound] = dDisSegment1;
				pt2DInROI[nFound] = pt2DIntercept[i];
				nFound++;
			}
		}
	}
	if(nFound <= 0)
		goto ErrorAbort;
	nMaxIndex = 0;
	dMax = dDisSegment[0];
	for(i=1;i<nFound;i++) {
		if(dMax < dDisSegment[i]) {
			dMax = dDisSegment[i];
			nMaxIndex = i;
		}
	}
	*ppt2DInROI = pt2DInROI[nMaxIndex];
PASS:
	return 0;
ErrorAbort:
	return -1;
}

int CRectangle2D::GetRect2DByFixOnePt(CPoint2D* ppt2DOrg,
						   CPoint2D* ppt2DOthers,
						   CRect* prcROI,
						   CRectangle2D* prc2DMaxROI) 
{
	int i;
	CPoint2D pt2DIntercept;
	CPoint2D pt2DCorner[3];
	double dDisOrg[3];
	double dDis[3];
	double dRatio[3];
	double dMinRatio;
	double dDisMax;
	int nDiagonalIndex;
	CPoint2D pt2DSideCornerOrg[2];
	CPoint2D pt2DSide[2];
	CRectangle2D rc2D0;
	CRectangle2D rc2D1;
	int nptSide;
	int nError;

	dMinRatio = 1;
	for(i=0;i<3;i++) {
		nError = GetLinePtsInROI(ppt2DOrg,
								 &ppt2DOthers[i],
								 &pt2DIntercept,
								 prcROI);
		if(nError)
			goto ErrorAbort;
		dDisOrg[i] = ppt2DOrg->GetDistance(ppt2DOthers[i]);
		dDis[i] = ppt2DOrg->GetDistance(pt2DIntercept);
		if(dDisOrg[i] == 0)
			goto ErrorAbort;
		dRatio[i] = dDis[i]/dDisOrg[i];
		if(dMinRatio > dRatio[i])
			dMinRatio = dRatio[i];
	}
	
	for(i=0;i<3;i++) {
		pt2DCorner[i] = CPoint2D(ppt2DOrg->x + dMinRatio * (ppt2DOthers[i].x - ppt2DOrg->x),
								 ppt2DOrg->y + dMinRatio * (ppt2DOthers[i].y - ppt2DOrg->y));
		if(ValidatePoint(&pt2DCorner[i], prcROI))
			goto ErrorAbort;
	}

	for(i=0;i<3;i++) 
		dDis[i] = ppt2DOrg->GetDistance(pt2DCorner[i]);
	nDiagonalIndex = 0;
	dDisMax = dDis[0];
	for(i=1;i<3;i++) {
		if(dDisMax < dDis[i]) {
			dDisMax = dDis[i];
			nDiagonalIndex = i;
		}
	}
	nptSide = 0;
	for(i=0;i<3;i++) {
		if(i==nDiagonalIndex)
			continue;
		pt2DSide[nptSide] = pt2DCorner[i];
		pt2DSideCornerOrg[nptSide] = ppt2DOthers[i];
		nptSide++;
	}

	nError = GetExpandRect2D(ppt2DOrg, &pt2DSide[0], &pt2DSideCornerOrg[1], prcROI, &rc2D0);
	nError = GetExpandRect2D(ppt2DOrg, &pt2DSide[1], &pt2DSideCornerOrg[0], prcROI, &rc2D1);
	if((rc2D0.GetWidth() * rc2D0.GetHeight()) > (rc2D1.GetWidth() * rc2D1.GetHeight()))
		*prc2DMaxROI = rc2D0;
	else
		*prc2DMaxROI = rc2D1;
	return 0;
ErrorAbort:
	return -1;
}

CRectangle2D & CRectangle2D::operator =(CRectangle2D& rc2D)
{
	m_pointCenter=rc2D.m_pointCenter;
	m_Width=rc2D.m_Width;
	m_Height=rc2D.m_Height;
	m_Angle=rc2D.m_Angle;
	return *this;
}


CRectangle2D::CRectangle2D(CRect &rect) {
	m_pointCenter=rect.CenterPoint();
	m_Width=rect.Width();
	m_Height=rect.Height();
	m_Angle=0;
}
void CRectangle2D::SetSize(CSize cs) {
	m_Width=cs.cx;
	m_Height=cs.cy;
}
void CRectangle2D::SetSize(CSize2D cs2D) {
	m_Width=cs2D.cx;
	m_Height=cs2D.cy;
}

double CRectangle2D::GetWidth() {
	return m_Width;
}

double CRectangle2D::GetHeight(){
	return m_Height;
}

CPoint2D CRectangle2D::GetDevTopLeft() {
	CString strMsg;
	CPoint2D p(m_pointCenter.x-m_Width/2,m_pointCenter.y-m_Height/2);
	p.RotateAbout(m_pointCenter,m_Angle);
	return p;
}

CPoint2D CRectangle2D::GetDevTopRight() {
	CPoint2D p(m_pointCenter.x+m_Width/2,m_pointCenter.y-m_Height/2);
	p.RotateAbout(m_pointCenter,m_Angle);
	return p;
}

CPoint2D CRectangle2D::GetDevBottomLeft() {
	CPoint2D p(m_pointCenter.x-m_Width/2,m_pointCenter.y+m_Height/2);
	p.RotateAbout(m_pointCenter,m_Angle);
	return p;
}

CPoint2D CRectangle2D::GetDevBottomRight() {
	CPoint2D p(m_pointCenter.x+m_Width/2,m_pointCenter.y+m_Height/2);
	p.RotateAbout(m_pointCenter,m_Angle);
	return p;
}

CPoint2D CRectangle2D::GetDevTopMost() {
	if(m_Angle>0) {
		CPoint2D p(m_pointCenter.x-m_Width/2,m_pointCenter.y-m_Height/2);
		p.RotateAbout(m_pointCenter,m_Angle);
		return p;
	}
	else {
		CPoint2D p(m_pointCenter.x+m_Width/2,m_pointCenter.y-m_Height/2);
		p.RotateAbout(m_pointCenter,m_Angle);
		return p;
	}
}

CPoint2D CRectangle2D::GetDevLftMost() {
	if(m_Angle>0) {
		CPoint2D p(m_pointCenter.x-m_Width/2,m_pointCenter.y+m_Height/2);
		p.RotateAbout(m_pointCenter,m_Angle);
		return p;
	}
	else {
		CPoint2D p(m_pointCenter.x-m_Width/2,m_pointCenter.y-m_Height/2);
		p.RotateAbout(m_pointCenter,m_Angle);
		return p;
	}
}

CPoint2D CRectangle2D::GetDevRhtMost() {
	if(m_Angle>0) {
		CPoint2D p(m_pointCenter.x+m_Width/2,m_pointCenter.y-m_Height/2);
		p.RotateAbout(m_pointCenter,m_Angle);
		return p;
	}
	else {
		CPoint2D p(m_pointCenter.x+m_Width/2,m_pointCenter.y+m_Height/2);
		p.RotateAbout(m_pointCenter,m_Angle);
		return p;
	}
}

CPoint2D CRectangle2D::GetDevBotMost() {
	if(m_Angle>0) {
		CPoint2D p(m_pointCenter.x+m_Width/2,m_pointCenter.y+m_Height/2);
		p.RotateAbout(m_pointCenter,m_Angle);
		return p;
	}
	else {
		CPoint2D p(m_pointCenter.x-m_Width/2,m_pointCenter.y+m_Height/2);
		p.RotateAbout(m_pointCenter,m_Angle);
		return p;
	}
}

double CRectangle2D::GetAngle() {
	return m_Angle;
}

CPoint2D CRectangle2D::GetCenterPoint() {
	return m_pointCenter;
}


void CRectangle2D::Shift(CPoint2D pt2DOffset) {
	m_pointCenter.x+=pt2DOffset.x;
	m_pointCenter.y+=pt2DOffset.y;
}

void CRectangle2D::ShiftTo(CPoint2D pt2D) {
	m_pointCenter.x = pt2D.x;
	m_pointCenter.y = pt2D.y;
}


void CRectangle2D::RotateAbout(CPoint2D p,double Angle) {
	m_pointCenter.RotateAbout(p,Angle);
	m_Angle+=Angle;
}

void CRectangle2D::RotateBy(double Angle) {
	m_Angle+=Angle;
}

void CRectangle2D::RotateTo(double Angle) {
	m_Angle=Angle;
}


CRect CRectangle2D::GetBoundingRect() {
	CPoint2D pt2D[4];
	double dTopMost, dBotMost, dLftMost, dRhtMost;
	CRect rect;
	int i;

	pt2D[0] = GetDevTopLeft();
	pt2D[1] = GetDevTopRight();
	pt2D[2] = GetDevBottomLeft();
	pt2D[3] = GetDevBottomRight();
	dTopMost = pt2D[0].y;
	dBotMost = pt2D[0].y;
	dLftMost = pt2D[0].x;
	dRhtMost = pt2D[0].x;
	for(i = 1; i < 4; i++) {
		if(dTopMost > pt2D[i].y) dTopMost = pt2D[i].y;
		if(dBotMost < pt2D[i].y) dBotMost = pt2D[i].y;
		if(dLftMost > pt2D[i].x) dLftMost = pt2D[i].x;
		if(dRhtMost < pt2D[i].x) dRhtMost = pt2D[i].x;
	}
	rect = CRect(Round(floor(dLftMost)), Round(floor(dTopMost)), Round(ceil(dRhtMost)) + 1, Round(ceil(dBotMost))+1);
	return rect;
}


CRect CRectangle2D::GetBoundingRectWithoutInflate() {
	CPoint2D pt2D[4];
	double dTopMost, dBotMost, dLftMost, dRhtMost;
	CRect rect;
	int i;

	pt2D[0] = GetDevTopLeft();
	pt2D[1] = GetDevTopRight();
	pt2D[2] = GetDevBottomLeft();
	pt2D[3] = GetDevBottomRight();
	dTopMost = pt2D[0].y;
	dBotMost = pt2D[0].y;
	dLftMost = pt2D[0].x;
	dRhtMost = pt2D[0].x;
	for(i = 1; i < 4; i++) {
		if(dTopMost > pt2D[i].y) dTopMost = pt2D[i].y;
		if(dBotMost < pt2D[i].y) dBotMost = pt2D[i].y;
		if(dLftMost > pt2D[i].x) dLftMost = pt2D[i].x;
		if(dRhtMost < pt2D[i].x) dRhtMost = pt2D[i].x;
	}
	rect = CRect(Round(dLftMost), Round(dTopMost), Round(dRhtMost), Round(dBotMost));
	return rect;
}

int CRectangle2D::GetExpandRect2D(CPoint2D* ppt2DOrg, 
								 CPoint2D* ppt2DSide, 
								 CPoint2D* ppt2DCornerOrg, 
								 CRect* prcROI,
								 CRectangle2D* prc2D)
{
	CPoint2D pt2DIntercept0;
	double dDis0;
	CPoint2D pt2DIntercept1;
	double dDis1;
	CPoint2D pt2DSideTip;
	CLine2D ln2D;
	int nError;

	pt2DSideTip = CPoint2D(ppt2DSide->x + ppt2DCornerOrg->x - ppt2DOrg->x,
						   ppt2DSide->y + ppt2DCornerOrg->y - ppt2DOrg->y);

	nError = GetLinePtsInROI(ppt2DOrg,
							 ppt2DCornerOrg,
							 &pt2DIntercept0,
							 prcROI);
	if(nError)
		goto ErrorAbort;
	dDis0 = ppt2DOrg->GetDistance(pt2DIntercept0);
	nError = GetLinePtsInROI(ppt2DSide,
							 &pt2DSideTip,
							 &pt2DIntercept1,
							 prcROI);
	if(nError)
		goto ErrorAbort;
	dDis1 = ppt2DSide->GetDistance(pt2DIntercept1);
	if(dDis0 < dDis1) {
		ln2D = CLine2D(*ppt2DSide, pt2DSideTip);
		ln2D.GetProjection(&pt2DIntercept0, &pt2DIntercept1);
	}
	else {
		ln2D = CLine2D(*ppt2DOrg, *ppt2DCornerOrg);
		ln2D.GetProjection(&pt2DIntercept1, &pt2DIntercept0);
	}
	*prc2D = CRectangle2D(ppt2DOrg, ppt2DSide,&pt2DIntercept0,&pt2DIntercept1);
	return 0;
ErrorAbort:
	return -1;
}

int CRectangle2D::Rotate(CPoint2D* ppt2DRotCtr, 
						 double dAngle)
{
	CPoint2D pt2DTopLft;
	CPoint2D pt2DTopRht;
	CLine2D ln2DTop;
	int nError;
	m_pointCenter.RotateAbout(*ppt2DRotCtr, dAngle);
	pt2DTopLft = GetDevTopLeft();
	pt2DTopLft.RotateAbout(*ppt2DRotCtr, dAngle);
	pt2DTopRht = GetDevTopRight();
	pt2DTopRht.RotateAbout(*ppt2DRotCtr, dAngle);
	ln2DTop = CLine2D(pt2DTopLft, pt2DTopRht);
	nError = ln2DTop.GetAngleRad(m_Angle);
	if(nError)
		goto ErrorAbort;
	return 0;
ErrorAbort:
	return -1;
}

int CRectangle2D::DeflateRect2D(double dl, double dt, double dr, double db)
{
	CLine2D ln2DHor, ln2DVer;
	CPoint2D pt2D1, pt2D2;
	CPoint2D pt2DHorShift, pt2DVerShift;
	if((dl < -INT_AND_DBL_EQ_TOL) || (dt < -INT_AND_DBL_EQ_TOL) || (dr < -INT_AND_DBL_EQ_TOL) || (db < -INT_AND_DBL_EQ_TOL))
		return -1;
	if(	m_Width <= (dl + dr))
		return -1;
	if(	m_Height <= (dt + db))
		return -1;
	if(dl < 0)
		dl = 0;
	if(dt < 0)
		dt = 0;
	if(dr < 0)
		dr = 0;
	if(db < 0)
		db = 0;
	ln2DHor = CLine2D(m_pointCenter, m_Angle);
	ln2DVer = CLine2D(m_pointCenter, m_Angle + PI/2);
	ln2DHor.GetParallelPoints(&m_pointCenter, fabs(dr - dl)/2, &pt2D1, &pt2D2);
	if(dr > dl) {
		if(pt2D1.x < pt2D2.x)
			pt2DHorShift = pt2D1;
		else
			pt2DHorShift = pt2D2;
	}
	else {
		if(pt2D1.x > pt2D2.x)
			pt2DHorShift = pt2D1;
		else
			pt2DHorShift = pt2D2;
	}

	ln2DVer.GetParallelPoints(&m_pointCenter, fabs(db - dt)/2, &pt2D1, &pt2D2);
	if(db > dt) {
		if(pt2D1.y < pt2D2.y)
			pt2DVerShift = pt2D1;
		else
			pt2DVerShift = pt2D2;
	}
	else {
		if(pt2D1.y > pt2D2.y)
			pt2DVerShift = pt2D1;
		else
			pt2DVerShift = pt2D2;
	}

	m_pointCenter = pt2DHorShift + pt2DVerShift - m_pointCenter;
	m_Width = m_Width - dl - dr;
	m_Height = m_Height - dt - db;
	return 0;
}



int CRectangle2D::InflateRect2D(double dl, double dt, double dr, double db)
{
	CLine2D ln2DHor, ln2DVer;
	CPoint2D pt2D1, pt2D2;
	CPoint2D pt2DHorShift, pt2DVerShift;

	if((dl < -INT_AND_DBL_EQ_TOL) || (dt < -INT_AND_DBL_EQ_TOL) || (dr < -INT_AND_DBL_EQ_TOL) || (db < -INT_AND_DBL_EQ_TOL))
		return -1;
	if(dl < 0)
		dl = 0;
	if(dt < 0)
		dt = 0;
	if(dr < 0)
		dr = 0;
	if(db < 0)
		db = 0;
	ln2DHor = CLine2D(m_pointCenter, m_Angle);
	ln2DVer = CLine2D(m_pointCenter, m_Angle + PI/2);
	ln2DHor.GetParallelPoints(&m_pointCenter, fabs(dr - dl)/2, &pt2D1, &pt2D2);
	if(dr > dl) {
		if(pt2D1.x > pt2D2.x)
			pt2DHorShift = pt2D1;
		else
			pt2DHorShift = pt2D2;
	}
	else {
		if(pt2D1.x < pt2D2.x)
			pt2DHorShift = pt2D1;
		else
			pt2DHorShift = pt2D2;
	}

	ln2DVer.GetParallelPoints(&m_pointCenter, fabs(db - dt)/2, &pt2D1, &pt2D2);
	if(db > dt) {
		if(pt2D1.y > pt2D2.y)
			pt2DVerShift = pt2D1;
		else
			pt2DVerShift = pt2D2;
	}
	else {
		if(pt2D1.y < pt2D2.y)
			pt2DVerShift = pt2D1;
		else
			pt2DVerShift = pt2D2;
	}

	m_pointCenter = pt2DHorShift + pt2DVerShift - m_pointCenter;
	m_Width = m_Width + dl + dr;
	m_Height = m_Height + dt + db;
	return 0;
}


CPoint2D CRectangle2D::GetGeoTopMost() {
	CPoint2D pt2D[4], pt2DTopMost;
	double dTopMost;
	CRect rect;
	int i;
	pt2D[0] = GetDevTopLeft();
	pt2D[1] = GetDevTopRight();
	pt2D[2] = GetDevBottomLeft();
	pt2D[3] = GetDevBottomRight();
	pt2DTopMost = pt2D[0];
	dTopMost = pt2D[0].y;
	for(i = 1; i < 4; i++) {
		if(dTopMost > pt2D[i].y) {
			dTopMost = pt2D[i].y;
			pt2DTopMost = pt2D[i];
		}
	}
	return pt2DTopMost;
}


CPoint2D CRectangle2D::GetGeoBottomMost() {
	CPoint2D pt2D[4], pt2DBottomMost;
	double dBottomMost;
	CRect rect;
	int i;
	pt2D[0] = GetDevTopLeft();
	pt2D[1] = GetDevTopRight();
	pt2D[2] = GetDevBottomLeft();
	pt2D[3] = GetDevBottomRight();
	pt2DBottomMost = pt2D[0];
	dBottomMost = pt2D[0].y;
	for(i = 1; i < 4; i++) {
		if(dBottomMost < pt2D[i].y) {
			dBottomMost = pt2D[i].y;
			pt2DBottomMost = pt2D[i];
		}
	}
	return pt2DBottomMost;
}



CPoint2D CRectangle2D::GetGeoLeftMost() {
	CPoint2D pt2D[4], pt2DLeftMost;
	double dLeftMost;
	CRect rect;
	int i;
	pt2D[0] = GetDevTopLeft();
	pt2D[1] = GetDevTopRight();
	pt2D[2] = GetDevBottomLeft();
	pt2D[3] = GetDevBottomRight();
	pt2DLeftMost = pt2D[0];
	dLeftMost = pt2D[0].x;
	for(i = 1; i < 4; i++) {
		if(dLeftMost > pt2D[i].x) {
			dLeftMost = pt2D[i].x;
			pt2DLeftMost = pt2D[i];
		}
	}
	return pt2DLeftMost;
}

CPoint2D CRectangle2D::GetGeoRightMost() {
	CPoint2D pt2D[4], pt2DRightMost;
	double dRightMost;
	CRect rect;
	int i;
	pt2D[0] = GetDevTopLeft();
	pt2D[1] = GetDevTopRight();
	pt2D[2] = GetDevBottomLeft();
	pt2D[3] = GetDevBottomRight();
	pt2DRightMost = pt2D[0];
	dRightMost = pt2D[0].x;
	for(i = 1; i < 4; i++) {
		if(dRightMost < pt2D[i].x) {
			dRightMost = pt2D[i].x;
			pt2DRightMost = pt2D[i];
		}
	}
	return pt2DRightMost;
}

CPlane::CPlane() {
}
CPlane::CPlane(CPoint3D p,CVector3D n):Position(p),Normal(n) {
	Position=p;
	Normal=n;
}

CPlane::CPlane(CLine3D Line1,CLine3D Line2):Position(Line1.Position),Normal(Line1.Direction*Line2.Direction) {
}

CPlane::CPlane(CPoint3D Point1,CPoint3D Point2,CPoint3D Point3):Position(Point2),Normal(CVector3D(Point1,Point2)*CVector3D(Point3,Point2)) {
}

CPlane::CPlane(CPoint3D p,CLine3D l):Position(p),Normal(CVector3D(p,l.Position)*l.Direction){
}

BOOL CPlane::IsValid() {
	return Normal.IsValid();
}

int CPlane::GetDistance(CPoint3D pt3D, 
						double& dDis) {
	CVector3D UnitNormal;
	int nError;
	nError=Normal.GetUnit(&UnitNormal);

	dDis = fabs((CVector3D(pt3D)^UnitNormal)-(CVector3D(Position)^UnitNormal));
	return nError;
}

int CPlane::GetAngleRad(CVector3D v,
					 double &dAngleRad) {
	double dot,mag;
	if (!v.IsValid()&&!IsValid()) {
		dot=Normal^v;
		mag=Normal.GetSize()*v.GetSize();
		dAngleRad = asin(dot/mag);
		return 0;
	}
	else {
		return -1;
	}
}

int CPlane::GetAngleRad(CLine3D l,
					 double &dAngleRad) {
	return GetAngleRad(l.Direction, dAngleRad);
}

int CPlane::GetAngleRad(CPlane p, 
						double &dAngleRad) {
	return Normal.GetAngleRad(p.Normal, dAngleRad);
}

int CPlane::GetIntersection(CLine3D Line,CPoint3D *pPoint) {
	if(Normal.IsValid()==0) return -1;
	double d=Line.Direction ^ Normal;
	if (d==0) return -1;
	*pPoint = (Line.Position+Line.Direction*((CVector3D(Position,Line.Position)^Normal)/d));
	return 0;
}

int CPlane::GetProjection(CPoint3D Point,
						  CPoint3D& pt3DProj) {
	if(Normal.IsValid()==0) return -1;
	double d=Normal ^ Normal;
	if (d==0) return -1;
	pt3DProj = Point+Normal*((CVector3D(Position,Point)^Normal)/d);
	return 0;
}

CLine3D::CLine3D(){
	m_bValid = FALSE;
}

CLine3D::CLine3D(CPoint3D pt3D,CVector3D vt3D):Position(pt3D),Direction(vt3D) {
	m_bValid = Direction.IsValid();
}

CLine3D::CLine3D(CPoint3D pt3D0,CPoint3D pt3D1):Position(pt3D0),Direction(pt3D1,pt3D0) {
	m_bValid = Direction.IsValid();
}

BOOL CLine3D::IsValid() {
	return Direction.IsValid();
}

int CLine3D::IsOnLine(CPoint3D pt3D,
					  BOOL* pbOnLine) {
	double dDis;
	int nError;
	nError = GetDistance(pt3D, dDis);
	if(nError == -1) {
		ASSERT(!"CLine3D is Invalid");
		return -1;
	}
	*pbOnLine = (dDis==0);
	return 0;
}

int CLine3D::GetDistance(CPoint3D ppt3D,
						 double& dDis) {
	CVector3D vt3D(ppt3D,Position);
	CVector3D vt3DNormal;
	if(IsValid()==0) {
		ASSERT(!"CLine3D is Invalid");
		return -1;
	}
	vt3DNormal=(vt3D*Direction)*Direction;
	vt3DNormal.Normalize();
	dDis = fabs(vt3D^vt3DNormal);
	return 0;
}

CSize3D::CSize3D() {
	cx=cy=cz=0;
}


CSize3D::CSize3D(int x0, int y0, int z0) {
	cx=x0;
	cy=y0;
	cz=z0;
}

CSize3D::CSize3D(double x0, double y0, double z0) {
	cx=x0;
	cy=y0;
	cz=z0;
}

CSize3D::CSize3D(const CSize3D &cs) {
	cx=cs.cx;
	cy=cs.cy;
	cz=cs.cz;
}

CSize3D & CSize3D::operator =(CSize3D cs3d)
{
	cx = cs3d.cx;
	cy = cs3d.cy;
	cz = cs3d.cz;
	return *this;
}

CRectInfo::CRectInfo()
{
	rc = CRect(0,0,0,0);
	pnLabelX = NULL;
	nLabelXSize = 0;
	pnLabelY = NULL;
	nLabelYSize = 0;
	m_nLabel = 0;
	pnConnectedLabelHistX = NULL;
	nConnectedLabelHistX = 0;
	pnConnectedLabelHistY = NULL;
	nConnectedLabelHistY = 0;
	pnConnectedLabelHist = NULL;
	nConnectedLabelHist = 0;
	bMerged = false;
}

void CRectInfo::Clean()
{
	if(pnLabelX != NULL)
		delete [] pnLabelX;
	if(pnLabelY != NULL)
		delete [] pnLabelY;
	if(pnConnectedLabelHistX != NULL)
		delete [] pnConnectedLabelHistX;
	if(pnConnectedLabelHistY != NULL)
		delete [] pnConnectedLabelHistY;
	if(pnConnectedLabelHist != NULL)
		delete [] pnConnectedLabelHist;
	rc = CRect(0,0,0,0);
	pnLabelX = NULL;
	nLabelXSize = 0;
	pnLabelY = NULL;
	nLabelYSize = 0;
	m_nLabel = 0;
	pnConnectedLabelHistX = NULL;
	nConnectedLabelHistX = 0;
	pnConnectedLabelHistY = NULL;
	nConnectedLabelHistY = 0;
	pnConnectedLabelHist = NULL;
	nConnectedLabelHist = 0;
	bMerged = false;
}


CRectInfo::~CRectInfo()
{
	Clean();
}

int	CRectInfo::SetLabel(CRect* prcSrc, CSize* pcsFOV, int nLabel)
{
	int i;
	Clean();
	if (ValidateRect(prcSrc, pcsFOV)) {
		ASSERT(!"prcSrc is Invalid");
		return -1;
	}
	m_nLabel = nLabel;
	rc = *prcSrc;
	nLabelXSize = pcsFOV->cx;
	nLabelYSize = pcsFOV->cy;
	if(pnLabelX != NULL)
		delete [] pnLabelX;
	if(pnLabelY != NULL)
		delete [] pnLabelY;
	pnLabelX = new int[abs(nLabelXSize)];
	pnLabelY = new int[abs(nLabelYSize)];
	memset((void *)pnLabelX,0,nLabelXSize*sizeof(int));
	memset((void *)pnLabelY,0,nLabelYSize*sizeof(int));
	for(i = prcSrc->left; i < prcSrc->right; i++)
		pnLabelX[i] = nLabel;
	for(i = prcSrc->top; i < prcSrc->bottom; i++)
		pnLabelY[i] = nLabel;
	return 0;
}

CMergedRect::CMergedRect()
{
	rcMerged = CRect(0,0,0,0);
	pnMergedHist = NULL;
	pnMergedLabel = NULL;
	nMergedLabel = 0;
}

CMergedRect::~CMergedRect()
{
	Clean();
}
void CMergedRect::Clean()
{
	if(pnMergedLabel != NULL)
		delete [] pnMergedLabel;
	if(pnMergedHist != NULL)
		delete [] pnMergedHist;
	rcMerged = CRect(0,0,0,0);
	pnMergedLabel = NULL;
	pnMergedHist = NULL;
	nMergedLabel = 0;
}

CRectMerge::CRectMerge()
{
	prcMerge = NULL;
	nRectMergeNo = 0;
	nRectInfoNo = 0;
	pRectInfo = NULL;
	pMergedRect = NULL;
	npMergedRect = 0;
	prcFoundMerged = NULL;
	nFoundMerged = 0;

	plAccX = NULL;
	plAccY = NULL;
}

CRectMerge::~CRectMerge()
{
	Clean();
}
void CRectMerge::Clean()
{
	int i;
	if(pRectInfo != NULL) {
		for(i = 0; i < nRectInfoNo; i++) {
			pRectInfo[i].Clean();
		}
		delete [] pRectInfo;
		pRectInfo = NULL;
		nRectInfoNo = 0;
	}
	if(pMergedRect != NULL) {
		for(i = 0; i < npMergedRect; i++)
			pMergedRect[i].Clean();
		delete [] pMergedRect;
		pMergedRect = NULL;
		npMergedRect = 0;
	}
	if(prcMerge != NULL)
		delete [] prcMerge;
	nRectMergeNo = 0;
	prcMerge = NULL;
	if(prcFoundMerged != NULL)
		delete [] prcFoundMerged;
	prcFoundMerged = NULL;
	nFoundMerged = 0;
}


void CRectMerge::FndConnectLocalLabelX(int nRectIndex)
{
	int i, j;
	int nLabel;
	nLabel = pRectInfo[nRectIndex].m_nLabel;
	if(pRectInfo[nRectIndex].pnConnectedLabelHistX != NULL)
		delete [] pRectInfo[nRectIndex].pnConnectedLabelHistX;
	pRectInfo[nRectIndex].pnConnectedLabelHistX = new int[abs(nRectInfoNo)];
	pRectInfo[nRectIndex].nConnectedLabelHistX = nRectInfoNo;
	memset((void *)pRectInfo[nRectIndex].pnConnectedLabelHistX,0,pRectInfo[nRectIndex].nConnectedLabelHistX*sizeof(int));
	for(i = pRectInfo[nRectIndex].rc.left; i < pRectInfo[nRectIndex].rc.right; i++) {
		if(plAccX[i] != nLabel) {
			for(j = nLabelStart; j < nRectInfoNo; j++) {
				if(pRectInfo[j].pnLabelX[i]!=0) {
					pRectInfo[nRectIndex].pnConnectedLabelHistX[j] = pRectInfo[j].m_nLabel;
				}
			}
		}
	}
	pRectInfo[nRectIndex].pnConnectedLabelHistX[nRectIndex] = 0;
}


void CRectMerge::FndConnectLocalLabelY(int nRectIndex)
{
	int i, j;
	int nLabel;
	nLabel = pRectInfo[nRectIndex].m_nLabel;
	if(pRectInfo[nRectIndex].pnConnectedLabelHistY != NULL)
		delete [] pRectInfo[nRectIndex].pnConnectedLabelHistY;
	pRectInfo[nRectIndex].pnConnectedLabelHistY = new int[abs(nRectInfoNo)];
	pRectInfo[nRectIndex].nConnectedLabelHistY = nRectInfoNo;
	memset((void *)pRectInfo[nRectIndex].pnConnectedLabelHistY,0,pRectInfo[nRectIndex].nConnectedLabelHistY*sizeof(int));
	for(i = pRectInfo[nRectIndex].rc.top; i < pRectInfo[nRectIndex].rc.bottom; i++) {
		if(plAccY[i] != nLabel) {
			for(j = nLabelStart; j < nRectInfoNo; j++) {
				if(pRectInfo[j].pnLabelY[i]!=0) {
					pRectInfo[nRectIndex].pnConnectedLabelHistY[j] = pRectInfo[j].m_nLabel;
				}
			}
		}
	}
	pRectInfo[nRectIndex].pnConnectedLabelHistY[nRectIndex] = 0;
}

void CRectMerge::FndCommonLocalLabel(int nRectIndex)
{
	int i;
	int nConnectedLabelHist;
	if(pRectInfo[nRectIndex].pnConnectedLabelHist != NULL)
		delete [] pRectInfo[nRectIndex].pnConnectedLabelHist;
	pRectInfo[nRectIndex].pnConnectedLabelHist = new int[abs(nRectInfoNo)];
	memset((void *)pRectInfo[nRectIndex].pnConnectedLabelHist,0,nRectInfoNo*sizeof(int));
	nConnectedLabelHist = 0;
	for(i = nLabelStart; i < nRectInfoNo; i++) {
		if((pRectInfo[nRectIndex].pnConnectedLabelHistY[i] > 0) &&
		   (pRectInfo[nRectIndex].pnConnectedLabelHistY[i] == pRectInfo[nRectIndex].pnConnectedLabelHistX[i]))
			pRectInfo[nRectIndex].pnConnectedLabelHist[nConnectedLabelHist++] = pRectInfo[nRectIndex].pnConnectedLabelHistX[i];
	}
	pRectInfo[nRectIndex].nConnectedLabelHist = nConnectedLabelHist;
}


int CRectMerge::Merge(CRect* prcRect, int nRectNo, CSize* pcsFOV)
{
	int i, j, m;
	int nLabel;
	int *pnMergeLabel;
	int nMergeLabelNo;
	int *pnOldSeedLabels;
	int nOldSeedNo;
	int *pnNewSeedLabels;
	int nNewSeedNo;

	nLabelStart = 1;
	Clean();
	prcMerge = new CRect[abs(nRectNo + nLabelStart)];
	nRectInfoNo = nRectNo + nLabelStart;
	pRectInfo = new CRectInfo[abs(nRectInfoNo + nLabelStart)];
	nRectMergeNo = 0;
	pnOldSeedLabels = new int[abs(nRectInfoNo)];
	pnNewSeedLabels = new int[abs(nRectInfoNo)];
	pnMergeLabel = new int[abs(nRectInfoNo)];
	pMergedRect = new CMergedRect[abs(nRectNo)];
	npMergedRect = nRectNo;
	prcFoundMerged = new CRect[abs(nRectNo)];
	plAccX = new long[abs(pcsFOV->cx)];
	plAccY = new long[abs(pcsFOV->cy)];
	memset((void *)plAccX,0,pcsFOV->cx*sizeof(long));
	memset((void *)plAccY,0,pcsFOV->cy*sizeof(long));
	for(i = nLabelStart; i < nRectInfoNo; i++) {
		nLabel = i;
		pRectInfo[i].SetLabel(&prcRect[i-1], pcsFOV, nLabel);
		for(j = prcRect[i-nLabelStart].left; j < prcRect[i-nLabelStart].right; j++) 
			plAccX[j] += nLabel;
		for(j = prcRect[i-nLabelStart].top; j < prcRect[i-nLabelStart].bottom; j++) 
			plAccY[j] += nLabel;
	}
	for(i = nLabelStart; i < nRectInfoNo; i++) {
		FndConnectLocalLabelX(i);
		FndConnectLocalLabelY(i);
		FndCommonLocalLabel(i);
	}
	nMergeLabelNo = 0;
	nOldSeedNo = 0;
	nNewSeedNo = 0;

	nMergedRect = 0;
	for(i = nLabelStart; i < nRectInfoNo; i++) {
		if(!pRectInfo[i].bMerged) {
			pMergedRect[nMergedRect].pnMergedLabel = new int[abs(nRectInfoNo)];
			pMergedRect[nMergedRect].pnMergedHist = new int[abs(nRectInfoNo)];
			memset((void *)pMergedRect[nMergedRect].pnMergedHist,0,nRectInfoNo*sizeof(int));
			pMergedRect[nMergedRect].pnMergedHist[0] = pRectInfo[i].m_nLabel-1;
			pRectInfo[i].bMerged = true;
			if(pRectInfo[i].nConnectedLabelHist) {
				nOldSeedNo = 1;
				pnOldSeedLabels[0] = pRectInfo[i].m_nLabel;
				pMergedRect[nMergedRect].nMergedLabel = 0;
				pMergedRect[nMergedRect].pnMergedHist[pRectInfo[i].m_nLabel] = pRectInfo[i].m_nLabel;
				pMergedRect[nMergedRect].pnMergedLabel[pMergedRect[nMergedRect].nMergedLabel++] = pRectInfo[i].m_nLabel;
				do{
					nNewSeedNo = 0;
					for(j = 0; j < nOldSeedNo; j++) {
						for(m = 0; m < pRectInfo[pnOldSeedLabels[j]].nConnectedLabelHist; m++) {
							if(pMergedRect[nMergedRect].pnMergedHist[pRectInfo[pnOldSeedLabels[j]].pnConnectedLabelHist[m]] != pRectInfo[pnOldSeedLabels[j]].pnConnectedLabelHist[m]) {
								
								pMergedRect[nMergedRect].pnMergedHist[pRectInfo[pnOldSeedLabels[j]].pnConnectedLabelHist[m]] = pRectInfo[pnOldSeedLabels[j]].pnConnectedLabelHist[m];
								pMergedRect[nMergedRect].pnMergedLabel[pMergedRect[nMergedRect].nMergedLabel++] = pRectInfo[pnOldSeedLabels[j]].pnConnectedLabelHist[m];
								pRectInfo[pRectInfo[pnOldSeedLabels[j]].pnConnectedLabelHist[m]].bMerged = true;
								pnNewSeedLabels[nNewSeedNo] = pRectInfo[pnOldSeedLabels[j]].pnConnectedLabelHist[m];
								nNewSeedNo++;
							}
						}
					}
					nOldSeedNo = nNewSeedNo;
					for(j = 0; j < nNewSeedNo; j++) 
						pnOldSeedLabels[j] = pnNewSeedLabels[j];
				}while(nOldSeedNo > 0);
			}
			else {
				pMergedRect[nMergedRect].pnMergedLabel[0] = pRectInfo[i].m_nLabel;
				pMergedRect[nMergedRect].nMergedLabel = 1;
			}
			nMergedRect++;
		}
	}
	CRect rcMerge;
	for(i = 0; i < nMergedRect; i++) {
		rcMerge = CRect(0,0,0,0);
		for(j=0; j<pMergedRect[i].nMergedLabel;j++) {
			pMergedRect[i].pnMergedLabel[j]--;
			rcMerge.UnionRect(rcMerge, pRectInfo[pMergedRect[i].pnMergedLabel[j]+1].rc);
		}
		prcFoundMerged[i] = rcMerge;
	}
	nFoundMerged = nMergedRect;
	delete [] pnOldSeedLabels;
	delete [] pnNewSeedLabels;
	delete [] pnMergeLabel;
	delete [] plAccX;
	delete [] plAccY;
	plAccX = NULL;
	plAccY = NULL;

	return 0;
}




int GetCircleCenter(CPoint2D* ppt2DSrc1,
					CPoint2D* ppt2DSrc2,
					CPoint2D* ppt2DSrc3,
					CPoint2D* ppt2DCenter) {

	if(*ppt2DSrc1 == *ppt2DSrc2) {
//		ASSERT(!"Point1 and Point2 are Invalid");
		return -1;
	}
	if(*ppt2DSrc1 == *ppt2DSrc3) {
//		ASSERT(!"Point1 and Point3 are Invalid");
		return -1;
	}
	double da00, da01, db0;
	double da10, da11, db1;
	double dTemp;
	da00 = 2 * (ppt2DSrc1->x - ppt2DSrc2->x);
	da01 = 2 * (ppt2DSrc1->y - ppt2DSrc2->y);
	da10 = 2 * (ppt2DSrc1->x - ppt2DSrc3->x);
	da11 = 2 * (ppt2DSrc1->y - ppt2DSrc3->y);
	dTemp = ppt2DSrc1->x * ppt2DSrc1->x + ppt2DSrc1->y * ppt2DSrc1->y;
	db0 = dTemp - (ppt2DSrc2->x * ppt2DSrc2->x + ppt2DSrc2->y * ppt2DSrc2->y);
	db1 = dTemp - (ppt2DSrc3->x * ppt2DSrc3->x + ppt2DSrc3->y * ppt2DSrc3->y);
	dTemp = da00 * da11 - da01 * da10;
	if(dTemp == 0) {
//		ASSERT(!"Three Points are collinear");
		return -1;
	}
	ppt2DCenter->x = (db0 * da11 - db1 * da01) / dTemp;

	//there might be something wrong
	//ppt2DCenter->y = (db0 - da00 * ppt2DCenter->x) / da01;
	// the new one....
    ppt2DCenter->y = (db1 * da00 - db0 * da10) / dTemp;
	return 0;
}


void Sorting(int *pnData,
		     int nDataNo,
			 int *pnIndex, 
			 BOOL bAscend) 
{
	int i,j,k,m,n,t;
	int temp;
	if(nDataNo<=1) {
		pnIndex[0] = 0;
		return;
	}
	if(bAscend) {
		for(k=0;k<nDataNo;k++) pnIndex[k]=k;
		for(k=1;k<nDataNo;k++) 
		{
			i=k;
			j=((i+1)/2)-1;
			while((i!=0)&&(pnData[j]<=pnData[i])) 
			{
				temp=pnData[i];
				pnData[i]=pnData[j];
				pnData[j]=temp;
				t=pnIndex[j];
				pnIndex[j]=pnIndex[i];
				pnIndex[i]=t;
				i=j;
				j=((i+1)/2)-1;
			}
		}
		for(i=nDataNo-1;i>=1;i--) 
		{
			temp=pnData[i];
			pnData[i]=pnData[0];
			pnData[0]=temp;
			t=pnIndex[i];
			pnIndex[i]=pnIndex[0];
			pnIndex[0]=t;
			k = i-1;
			m = 0;
			n = 1;
			if((k>=2)&&(pnData[2]>pnData[1])) n=2;
			while((n<=k)&&(pnData[n]>pnData[m])) 
			{
				temp=pnData[m];
				pnData[m]=pnData[n];
				pnData[n]=temp;
				t=pnIndex[n];
				pnIndex[n]=pnIndex[m];
				pnIndex[m]=t;
				m=n;
				n=(2*(m+1))-1;
				if((n+1)<=k)
				if(pnData[n+1]>pnData[n]) n++;
			}
		}
	}
	else {
		for(k=0;k<nDataNo;k++) pnIndex[k]=k;
		for(k=1;k<nDataNo;k++) 
		{
			i=k;
			j=((i+1)/2)-1;
			while((i!=0)&&(pnData[j]>pnData[i])) 
			{
				temp=pnData[i];
				pnData[i]=pnData[j];
				pnData[j]=temp;
				t=pnIndex[j];
				pnIndex[j]=pnIndex[i];
				pnIndex[i]=t;
				i=j;
				j=((i+1)/2)-1;
			}
		}
		for(i=nDataNo-1;i>=1;i--) 
		{
			temp=pnData[i];
			pnData[i]=pnData[0];
			pnData[0]=temp;
			t=pnIndex[i];
			pnIndex[i]=pnIndex[0];
			pnIndex[0]=t;
			k = i-1;
			m = 0;
			n = 1;
			if((k>=2)&&(pnData[2]<pnData[1])) n=2;
			while((n<=k)&&(pnData[n]<pnData[m])) 
			{
				temp=pnData[m];
				pnData[m]=pnData[n];
				pnData[n]=temp;
				t=pnIndex[n];
				pnIndex[n]=pnIndex[m];
				pnIndex[m]=t;
				m=n;
				n=(2*(m+1))-1;
				if((n+1)<=k)
				if(pnData[n+1]<pnData[n]) n++;
			}
		}
	}
}


void Sorting(int *pnData,
		     int nDataNo,
			 BOOL bAscend) 
{
	int i,j,k,m,n;
	int temp;
	if(nDataNo<=1) return;
	if(bAscend) {
		for(k=1;k<nDataNo;k++) 
		{
			i=k;
			j=((i+1)/2)-1;
			while((i!=0)&&(pnData[j]<=pnData[i])) 
			{
				temp=pnData[i];
				pnData[i]=pnData[j];
				pnData[j]=temp;
				i=j;
				j=((i+1)/2)-1;
			}
		}
		for(i=nDataNo-1;i>=1;i--) 
		{
			temp=pnData[i];
			pnData[i]=pnData[0];
			pnData[0]=temp;
			k = i-1;
			m = 0;
			n = 1;
			if((k>=2)&&(pnData[2]>pnData[1])) n=2;
			while((n<=k)&&(pnData[n]>pnData[m])) 
			{
				temp=pnData[m];
				pnData[m]=pnData[n];
				pnData[n]=temp;
				m=n;
				n=(2*(m+1))-1;
				if((n+1)<=k)
				if(pnData[n+1]>pnData[n]) n++;
			}
		}
	}
	else {
		for(k=1;k<nDataNo;k++) 
		{
			i=k;
			j=((i+1)/2)-1;
			while((i!=0)&&(pnData[j]>pnData[i])) 
			{
				temp=pnData[i];
				pnData[i]=pnData[j];
				pnData[j]=temp;
				i=j;
				j=((i+1)/2)-1;
			}
		}
		for(i=nDataNo-1;i>=1;i--) 
		{
			temp=pnData[i];
			pnData[i]=pnData[0];
			pnData[0]=temp;
			k = i-1;
			m = 0;
			n = 1;
			if((k>=2)&&(pnData[2]<pnData[1])) n=2;
			while((n<=k)&&(pnData[n]<pnData[m])) 
			{
				temp=pnData[m];
				pnData[m]=pnData[n];
				pnData[n]=temp;
				m=n;
				n=(2*(m+1))-1;
				if((n+1)<=k)
				if(pnData[n+1]<pnData[n]) n++;
			}
		}
	}
}


void Sorting(long *plData,
		     int nDataNo,
			 int *pnIndex, 
			 BOOL bAscend) 
{
	int i,j,k,m,n,t;
	long temp;
	if(nDataNo<=1) {
		pnIndex[0] = 0;
		return;
	}
	if(bAscend) {
		for(k=0;k<nDataNo;k++) pnIndex[k]=k;
		for(k=1;k<nDataNo;k++) 
		{
			i=k;
			j=((i+1)/2)-1;
			while((i!=0)&&(plData[j]<=plData[i])) 
			{
				temp=plData[i];
				plData[i]=plData[j];
				plData[j]=temp;
				t=pnIndex[j];
				pnIndex[j]=pnIndex[i];
				pnIndex[i]=t;
				i=j;
				j=((i+1)/2)-1;
			}
		}
		for(i=nDataNo-1;i>=1;i--) 
		{
			temp=plData[i];
			plData[i]=plData[0];
			plData[0]=temp;
			t=pnIndex[i];
			pnIndex[i]=pnIndex[0];
			pnIndex[0]=t;
			k = i-1;
			m = 0;
			n = 1;
			if((k>=2)&&(plData[2]>plData[1])) n=2;
			while((n<=k)&&(plData[n]>plData[m])) 
			{
				temp=plData[m];
				plData[m]=plData[n];
				plData[n]=temp;
				t=pnIndex[n];
				pnIndex[n]=pnIndex[m];
				pnIndex[m]=t;
				m=n;
				n=(2*(m+1))-1;
				if((n+1)<=k)
				if(plData[n+1]>plData[n]) n++;
			}
		}
	}
	else {
		for(k=0;k<nDataNo;k++) pnIndex[k]=k;
		for(k=1;k<nDataNo;k++) 
		{
			i=k;
			j=((i+1)/2)-1;
			while((i!=0)&&(plData[j]>plData[i])) 
			{
				temp=plData[i];
				plData[i]=plData[j];
				plData[j]=temp;
				t=pnIndex[j];
				pnIndex[j]=pnIndex[i];
				pnIndex[i]=t;
				i=j;
				j=((i+1)/2)-1;
			}
		}
		for(i=nDataNo-1;i>=1;i--) 
		{
			temp=plData[i];
			plData[i]=plData[0];
			plData[0]=temp;
			t=pnIndex[i];
			pnIndex[i]=pnIndex[0];
			pnIndex[0]=t;
			k = i-1;
			m = 0;
			n = 1;
			if((k>=2)&&(plData[2]<plData[1])) n=2;
			while((n<=k)&&(plData[n]<plData[m])) 
			{
				temp=plData[m];
				plData[m]=plData[n];
				plData[n]=temp;
				t=pnIndex[n];
				pnIndex[n]=pnIndex[m];
				pnIndex[m]=t;
				m=n;
				n=(2*(m+1))-1;
				if((n+1)<=k)
				if(plData[n+1]<plData[n]) n++;
			}
		}
	}
}

void Sorting(long *plData,
		     int nDataNo,
			 BOOL bAscend) 
{
	int i,j,k,m,n;
	long temp;
	if(nDataNo<=1) return;
	if(bAscend) {
		for(k=1;k<nDataNo;k++) 
		{
			i=k;
			j=((i+1)/2)-1;
			while((i!=0)&&(plData[j]<=plData[i])) 
			{
				temp=plData[i];
				plData[i]=plData[j];
				plData[j]=temp;
				i=j;
				j=((i+1)/2)-1;
			}
		}
		for(i=nDataNo-1;i>=1;i--) 
		{
			temp=plData[i];
			plData[i]=plData[0];
			plData[0]=temp;
			k = i-1;
			m = 0;
			n = 1;
			if((k>=2)&&(plData[2]>plData[1])) n=2;
			while((n<=k)&&(plData[n]>plData[m])) 
			{
				temp=plData[m];
				plData[m]=plData[n];
				plData[n]=temp;
				m=n;
				n=(2*(m+1))-1;
				if((n+1)<=k)
				if(plData[n+1]>plData[n]) n++;
			}
		}
	}
	else {
		for(k=1;k<nDataNo;k++) 
		{
			i=k;
			j=((i+1)/2)-1;
			while((i!=0)&&(plData[j]>plData[i])) 
			{
				temp=plData[i];
				plData[i]=plData[j];
				plData[j]=temp;
				i=j;
				j=((i+1)/2)-1;
			}
		}
		for(i=nDataNo-1;i>=1;i--) 
		{
			temp=plData[i];
			plData[i]=plData[0];
			plData[0]=temp;
			k = i-1;
			m = 0;
			n = 1;
			if((k>=2)&&(plData[2]<plData[1])) n=2;
			while((n<=k)&&(plData[n]<plData[m])) 
			{
				temp=plData[m];
				plData[m]=plData[n];
				plData[n]=temp;
				m=n;
				n=(2*(m+1))-1;
				if((n+1)<=k)
				if(plData[n+1]<plData[n]) n++;
			}
		}
	}
}

void Sorting(double *pnData,
		     int nDataNo,
			 BOOL bAscend) 
{
	int i,j,k,m,n;
	double temp;
	if(nDataNo<=1) return;
	if(bAscend) {
		for(k=1;k<nDataNo;k++) 
		{
			i=k;
			j=((i+1)/2)-1;
			while((i!=0)&&(pnData[j]<=pnData[i])) 
			{
				temp=pnData[i];
				pnData[i]=pnData[j];
				pnData[j]=temp;
				i=j;
				j=((i+1)/2)-1;
			}
		}
		for(i=nDataNo-1;i>=1;i--) 
		{
			temp=pnData[i];
			pnData[i]=pnData[0];
			pnData[0]=temp;
			k = i-1;
			m = 0;
			n = 1;
			if((k>=2)&&(pnData[2]>pnData[1])) n=2;
			while((n<=k)&&(pnData[n]>pnData[m])) 
			{
				temp=pnData[m];
				pnData[m]=pnData[n];
				pnData[n]=temp;
				m=n;
				n=(2*(m+1))-1;
				if((n+1)<=k)
				if(pnData[n+1]>pnData[n]) n++;
			}
		}
	}
	else {
		for(k=1;k<nDataNo;k++) 
		{
			i=k;
			j=((i+1)/2)-1;
			while((i!=0)&&(pnData[j]>pnData[i])) 
			{
				temp=pnData[i];
				pnData[i]=pnData[j];
				pnData[j]=temp;
				i=j;
				j=((i+1)/2)-1;
			}
		}
		for(i=nDataNo-1;i>=1;i--) 
		{
			temp=pnData[i];
			pnData[i]=pnData[0];
			pnData[0]=temp;
			k = i-1;
			m = 0;
			n = 1;
			if((k>=2)&&(pnData[2]<pnData[1])) n=2;
			while((n<=k)&&(pnData[n]<pnData[m])) 
			{
				temp=pnData[m];
				pnData[m]=pnData[n];
				pnData[n]=temp;
				m=n;
				n=(2*(m+1))-1;
				if((n+1)<=k)
				if(pnData[n+1]<pnData[n]) n++;
			}
		}
	}
}


void Sorting(double *pnData,
		     int nDataNo,
			 int *pnIndex, 
			 BOOL bAscend) 
{
	int i,j,k,m,n,t;
	double temp;
	if(nDataNo<=1) {
		pnIndex[0] = 0;
		return;
	}
	if(bAscend) {
		for(k=0;k<nDataNo;k++) pnIndex[k]=k;
		for(k=1;k<nDataNo;k++) 
		{
			i=k;
			j=((i+1)/2)-1;
			while((i!=0)&&(pnData[j]<=pnData[i])) 
			{
				temp=pnData[i];
				pnData[i]=pnData[j];
				pnData[j]=temp;
				t=pnIndex[j];
				pnIndex[j]=pnIndex[i];
				pnIndex[i]=t;
				i=j;
				j=((i+1)/2)-1;
			}
		}
		for(i=nDataNo-1;i>=1;i--) 
		{
			temp=pnData[i];
			pnData[i]=pnData[0];
			pnData[0]=temp;
			t=pnIndex[i];
			pnIndex[i]=pnIndex[0];
			pnIndex[0]=t;
			k = i-1;
			m = 0;
			n = 1;
			if((k>=2)&&(pnData[2]>pnData[1])) n=2;
			while((n<=k)&&(pnData[n]>pnData[m])) 
			{
				temp=pnData[m];
				pnData[m]=pnData[n];
				pnData[n]=temp;
				t=pnIndex[n];
				pnIndex[n]=pnIndex[m];
				pnIndex[m]=t;
				m=n;
				n=(2*(m+1))-1;
				if((n+1)<=k)
				if(pnData[n+1]>pnData[n]) n++;
			}
		}
	}
	else {
		for(k=0;k<nDataNo;k++) pnIndex[k]=k;
		for(k=1;k<nDataNo;k++) 
		{
			i=k;
			j=((i+1)/2)-1;
			while((i!=0)&&(pnData[j]>pnData[i])) 
			{
				temp=pnData[i];
				pnData[i]=pnData[j];
				pnData[j]=temp;
				t=pnIndex[j];
				pnIndex[j]=pnIndex[i];
				pnIndex[i]=t;
				i=j;
				j=((i+1)/2)-1;
			}
		}
		for(i=nDataNo-1;i>=1;i--) 
		{
			temp=pnData[i];
			pnData[i]=pnData[0];
			pnData[0]=temp;
			t=pnIndex[i];
			pnIndex[i]=pnIndex[0];
			pnIndex[0]=t;
			k = i-1;
			m = 0;
			n = 1;
			if((k>=2)&&(pnData[2]<pnData[1])) n=2;
			while((n<=k)&&(pnData[n]<pnData[m])) 
			{
				temp=pnData[m];
				pnData[m]=pnData[n];
				pnData[n]=temp;
				t=pnIndex[n];
				pnIndex[n]=pnIndex[m];
				pnIndex[m]=t;
				m=n;
				n=(2*(m+1))-1;
				if((n+1)<=k)
				if(pnData[n+1]<pnData[n]) n++;
			}
		}
	}
}


int GetCombination(int nBase, 
				   int *pnOrder,
				   int& nValid)
{
    int i, j, k;
	int Valid=0;
	if(nBase < 3) {
		ASSERT(!"the combination for 3 out of nBase is calculated! nBase <3");
		return -1;
	}

	 for (i=0; i<nBase; i++) {
		for (j=0; j<nBase; j++) {
			for (k=0; k<nBase; k++) {
				if ((i!=j) & (i!=k) & (j!=k) & (k>j) & (k>i) & (j>i)) {
					    pnOrder[Valid * 3 + 0] = i;
						pnOrder[Valid * 3 + 1] = j;
						pnOrder[Valid * 3 + 2] = k;
						Valid++;					 
				}
			}
		}
	 }
     nValid = Valid;
	 return 0;
}

double InterpolX(CPoint2D ptBefore,CPoint2D ptAfter, double dThres) {
	if(ptAfter.y == ptBefore.y)
		return (ptAfter.x+ptBefore.x)/2;
	else
		return ptBefore.x + ((ptAfter.x-ptBefore.x)/(ptAfter.y-ptBefore.y))*(dThres-ptBefore.y);
}

int Round(double d)
{
	if(d>=0) 
		return (int)(d + 0.5); 
	else 
		return (int)(d - 0.5); 
}

int InterpolCPoint(CPoint* pptSrc1, 
				   BYTE byteVal1,
				   CPoint* pptSrc2, 
				   BYTE byteVal2, 
				   BYTE byteThreshold,
				   CPoint2D* ppt2DInterpol,
				   bool bThresholdCheck)
{
	if(bThresholdCheck) {
		if((byteVal1 > byteThreshold) && (byteVal2 > byteThreshold))
			return -1;
		if((byteVal1 < byteThreshold) && (byteVal2 < byteThreshold))
			return -1;
	}
	if(byteVal1 == byteVal2) {
		if(byteVal1 == byteThreshold) {
			ppt2DInterpol->x = (pptSrc1->x + pptSrc2->x) / 2.0;
			ppt2DInterpol->y = (pptSrc1->y + pptSrc2->y) / 2.0;
			return 0;
		}
		else
			return -1;
	}
	if(pptSrc1->x == pptSrc2->x)
		ppt2DInterpol->x = pptSrc1->x;
	else 
		ppt2DInterpol->x = pptSrc1->x + (pptSrc2->x - pptSrc1->x) * (byteThreshold - byteVal1) / double(byteVal2 - byteVal1);
	if(pptSrc1->y == pptSrc2->y)
		ppt2DInterpol->y = pptSrc1->y;
	else 
		ppt2DInterpol->y = pptSrc1->y + (pptSrc2->y - pptSrc1->y) * (byteThreshold - byteVal1) / double(byteVal2 - byteVal1);
	return 0;
}

int GetRoughBallCenter(CPoint2D* ppt2DBallEdge, 
					   int nBallEdge, 
					   CPoint2D* ppt2DEstimatedCenter,
					   double dBallRadiusTol,
					   double dCenterStep,
					   CSize2D* pcs2DShiftTol,
					   CPoint2D* ppt2DBallCenter,
					   double* pdRadiusInsp,
					   int* pnPercentage)
{
	int i, j, k;
	int nHistConv;
	int nMaxHist;
	int nMaxHistPos;
	int nXStart, nXEnd;
	int nYStart, nYEnd;
	CPoint2D pt2DShift, pt2DCenterTemp;
	CAdvHistogram AdvHistogram;
	int* pnEdgeRadiusSqr;
	int nMaxHistTemp;
	int nMaxHistPosTemp;
	double dX, dY;
	long lSum;


	if(nBallEdge == 0)
		return -1;
	if(dCenterStep<0.1)
		dCenterStep = 0.1;
	nXStart = -Round(pcs2DShiftTol->cx/dCenterStep);
	if(abs(nXStart)<1)
		nXStart = -1;
	nXEnd = -nXStart;
	nYStart = -Round(pcs2DShiftTol->cy/dCenterStep);
	if(abs(nYStart)<1)
		nYStart = -1;
	nYEnd = -nYStart;
	pnEdgeRadiusSqr = new int[abs(nBallEdge)];
	nMaxHist = 0;
	nMaxHistPos = 0;
	lSum = 0;
	for(k=0; k<nBallEdge; k++) {
		dX = ppt2DEstimatedCenter->x - ppt2DBallEdge[k].x;
		dY = ppt2DEstimatedCenter->y - ppt2DBallEdge[k].y;
		pnEdgeRadiusSqr[k] = int(dX * dX + dY * dY);
		lSum += pnEdgeRadiusSqr[k];
	}
	nHistConv = (int)(2 * dBallRadiusTol * sqrt((double ) (lSum/nBallEdge)));
	if(nHistConv < 1)
		nHistConv = 1;
	AdvHistogram.GetFastMaxConvHist1D(&pnEdgeRadiusSqr[0], 
								  nBallEdge,
								  nHistConv,
								  nMaxHist,
								  nMaxHistTemp,
								  nMaxHistPosTemp);
	nMaxHist = nMaxHistTemp;
	nMaxHistPos = nMaxHistPosTemp;
	*ppt2DBallCenter = *ppt2DEstimatedCenter;
	for(i=nXStart;i<=nXEnd;i++) {
		for(j=nYStart;j<=nYEnd;j++) {
			if((i==0)&&(j==0))
				continue;
			pt2DShift = CPoint2D(i*dCenterStep, j*dCenterStep);
			pt2DCenterTemp = *ppt2DEstimatedCenter + pt2DShift;
			for(k=0; k<nBallEdge; k++) {
				dX = pt2DCenterTemp.x - ppt2DBallEdge[k].x;
				dY = pt2DCenterTemp.y - ppt2DBallEdge[k].y;
				pnEdgeRadiusSqr[k] = int(dX * dX + dY * dY);
			}
			AdvHistogram.GetFastMaxConvHist1D(&pnEdgeRadiusSqr[0], 
										  nBallEdge,
										  nHistConv,
										  nMaxHist,
										  nMaxHistTemp,
										  nMaxHistPosTemp);
			if(nMaxHist < nMaxHistTemp) {
				nMaxHist = nMaxHistTemp;
				nMaxHistPos = nMaxHistPosTemp;
				*ppt2DBallCenter = *ppt2DEstimatedCenter + pt2DShift;
			}
		}
	}
	*pdRadiusInsp = sqrt((double) nMaxHistPos);
	*pnPercentage = (nMaxHist * 100)/nBallEdge;
	delete [] pnEdgeRadiusSqr;
	return 0;
}

int GetPointsBoundingRect(CArray<CPoint2D, CPoint2D>* pArypt2D, 
						  CRect* prcBounding)
{
	int i;
	double dTopMost, dBotMost, dLftMost, dRhtMost;
	*prcBounding = CRect(0,0,0,0);
	if(pArypt2D->GetSize() < 1)
		return -1;
	dTopMost = pArypt2D->GetAt(0).y;
	dBotMost = pArypt2D->GetAt(0).y;
	dLftMost = pArypt2D->GetAt(0).x;
	dRhtMost = pArypt2D->GetAt(0).x;
	for(i=0; i<pArypt2D->GetSize();i++) {
		if(dTopMost > pArypt2D->GetAt(i).y) dTopMost = pArypt2D->GetAt(i).y;
		if(dBotMost < pArypt2D->GetAt(i).y) dBotMost = pArypt2D->GetAt(i).y;
		if(dLftMost > pArypt2D->GetAt(i).x) dLftMost = pArypt2D->GetAt(i).x;
		if(dRhtMost < pArypt2D->GetAt(i).x) dRhtMost = pArypt2D->GetAt(i).x;
	}
	*prcBounding = CRect(int(dLftMost), int(dTopMost), int(dRhtMost) + 1, int(dBotMost)+1);

	if((dLftMost < 0) ||
		(dTopMost < 0) ||
		(dLftMost >= dRhtMost) ||
		(dTopMost >= dBotMost))
		goto ErrorAbort;
	return 0;
	ErrorAbort:
	return -1;
}

int GetPointsBoundingRect(CPoint2D* ppt2D, 
						  int nptNo,
						  CRect* prcBounding)
{
	int i;
	double dTopMost, dBotMost, dLftMost, dRhtMost;
	*prcBounding = CRect(0,0,0,0);
	if(nptNo < 1)
		return -1;
	dTopMost = ppt2D[0].y;
	dBotMost = ppt2D[0].y;
	dLftMost = ppt2D[0].x;
	dRhtMost = ppt2D[0].x;
	for(i=0; i<nptNo;i++) {
		if(dTopMost > ppt2D[i].y) dTopMost = ppt2D[i].y;
		if(dBotMost < ppt2D[i].y) dBotMost = ppt2D[i].y;
		if(dLftMost > ppt2D[i].x) dLftMost = ppt2D[i].x;
		if(dRhtMost < ppt2D[i].x) dRhtMost = ppt2D[i].x;
	}
	*prcBounding = CRect(int(dLftMost), int(dTopMost), int(dRhtMost) + 1, int(dBotMost)+1);

	if((dLftMost < 0) ||
		(dTopMost < 0) ||
		(dLftMost >= dRhtMost) ||
		(dTopMost >= dBotMost))
		goto ErrorAbort;
	return 0;
	ErrorAbort:
	return -1;
}

int GetPointsBoundingRect(CPoint* ppt, 
						  int nptNo,
						  CRect* prcBounding)
{
	int i;
	int nTopMost, nBotMost, nLftMost, nRhtMost;
	*prcBounding = CRect(0,0,0,0);
	if(nptNo < 1)
		return -1;
	nTopMost = ppt[0].y;
	nBotMost = ppt[0].y;
	nLftMost = ppt[0].x;
	nRhtMost = ppt[0].x;
	for(i=0; i<nptNo;i++) {
		if(nTopMost > ppt[i].y) nTopMost = ppt[i].y;
		if(nBotMost < ppt[i].y) nBotMost = ppt[i].y;
		if(nLftMost > ppt[i].x) nLftMost = ppt[i].x;
		if(nRhtMost < ppt[i].x) nRhtMost = ppt[i].x;
	}
	*prcBounding = CRect(int(nLftMost), int(nTopMost), int(nRhtMost) + 1, int(nBotMost)+1);

	if((nLftMost < 0) ||
		(nTopMost < 0) ||
		(nLftMost >= nRhtMost) ||
		(nTopMost >= nBotMost))
		goto ErrorAbort;
	return 0;
	ErrorAbort:
	return -1;
}

int GetPointsBoundingRect(CArray<CPoint, CPoint>* pArypt, 
						  CRect* prcBounding)
{
	int i;
	int nTopMost, nBotMost, nLftMost, nRhtMost;
	*prcBounding = CRect(0,0,0,0);
	if(pArypt->GetSize() < 1)
		return -1;
	nTopMost = pArypt->GetAt(0).y;
	nBotMost = pArypt->GetAt(0).y;
	nLftMost = pArypt->GetAt(0).x;
	nRhtMost = pArypt->GetAt(0).x;
	for(i=0; i<pArypt->GetSize();i++) {
		if(nTopMost > pArypt->GetAt(i).y) nTopMost = pArypt->GetAt(i).y;
		if(nBotMost < pArypt->GetAt(i).y) nBotMost = pArypt->GetAt(i).y;
		if(nLftMost > pArypt->GetAt(i).x) nLftMost = pArypt->GetAt(i).x;
		if(nRhtMost < pArypt->GetAt(i).x) nRhtMost = pArypt->GetAt(i).x;
	}
	*prcBounding = CRect(int(nLftMost), int(nTopMost), int(nRhtMost) + 1, int(nBotMost)+1);

	if((nLftMost < 0) ||
		(nTopMost < 0) ||
		(nLftMost >= nRhtMost) ||
		(nTopMost >= nBotMost))
		goto ErrorAbort;
	return 0;
	ErrorAbort:
	return -1;
}

int GetRoughBallCenter(CPoint* pptBallEdge, 
					   int nBallEdge, 
					   CPoint2D* ppt2DEstimatedCenter,
					   double dBallRadiusTol,
					   double dCenterStep,
					   CSize2D* pcs2DShiftTol,
					   CPoint2D* ppt2DBallCenter,
					   double* pdRadiusInsp,
					   int* pnPercentage)
{
	int i, j, k;
	int nHistConv;
	int nMaxHist;
	int nMaxHistPos;
	int nXStart, nXEnd;
	int nYStart, nYEnd;
	CPoint2D pt2DShift, pt2DCenterTemp;
	CAdvHistogram AdvHistogram;
	int* pnEdgeRadiusSqr;
	int nMaxHistTemp;
	int nMaxHistPosTemp;
	long lSum;

	int* pnBallEdgeX10;
	int* pnBallEdgeY10;
	int nEstimatedCenterX10;
	int nEstimatedCenterY10;
	int nTestCenterX10;
	int nTestCenterY10;
	int nCenterStep10;
	CSize csShiftTol;
	CPoint ptBallCenter;
	int nX, nY;


	if(nBallEdge == 0)
		goto ErrorAbort;

	pnBallEdgeX10 = new int[abs(nBallEdge)];
	pnBallEdgeY10 = new int[abs(nBallEdge)];
	for(i=0;i<nBallEdge;i++) {
		pnBallEdgeX10[i] = pptBallEdge[i].x * 10;
		pnBallEdgeY10[i] = pptBallEdge[i].y * 10;
	}
	if(dCenterStep<0.1)
		dCenterStep = 0.1;
	nCenterStep10 = Round(dCenterStep * 10);
	nEstimatedCenterX10 = Round(ppt2DEstimatedCenter->x * 10);
	nEstimatedCenterY10 = Round(ppt2DEstimatedCenter->y * 10);


	nXStart = -Round(pcs2DShiftTol->cx/dCenterStep);
	if(abs(nXStart)<1)
		nXStart = -1;
	nXEnd = -nXStart;
	nYStart = -Round(pcs2DShiftTol->cy/dCenterStep);
	if(abs(nYStart)<1)
		nYStart = -1;
	nYEnd = -nYStart;
	pnEdgeRadiusSqr = new int[abs(nBallEdge)];
	nMaxHist = 0;
	nMaxHistPos = 0;
	lSum = 0;
	for(k=0; k<nBallEdge; k++) {
		nX = nEstimatedCenterX10 - pnBallEdgeX10[k];
		nY = nEstimatedCenterY10 - pnBallEdgeY10[k];
		pnEdgeRadiusSqr[k] = nX * nX + nY * nY;
		lSum += pnEdgeRadiusSqr[k];
	}
	nHistConv = (int)(20 * dBallRadiusTol * sqrt((double) (lSum/nBallEdge)));
	if(nHistConv < 1)
		nHistConv = 1;
	AdvHistogram.GetFastMaxConvHist1D(&pnEdgeRadiusSqr[0], 
								  nBallEdge,
								  nHistConv,
								  nMaxHist,
								  nMaxHistTemp,
								  nMaxHistPosTemp);
	nMaxHist = nMaxHistTemp;
	nMaxHistPos = nMaxHistPosTemp;
	*ppt2DBallCenter = *ppt2DEstimatedCenter;
	for(i=nXStart;i<=nXEnd;i++) {
		for(j=nYStart;j<=nYEnd;j++) {
			if((i==0)&&(j==0))
				continue;
			pt2DShift = CPoint2D(i*dCenterStep, j*dCenterStep);
			pt2DCenterTemp = *ppt2DEstimatedCenter + pt2DShift;
			nTestCenterX10 = nEstimatedCenterX10 + Round(pt2DShift.x * 10);
			nTestCenterY10 = nEstimatedCenterY10 + Round(pt2DShift.y * 10);
			for(k=0; k<nBallEdge; k++) {
				nX = nTestCenterX10 - pnBallEdgeX10[k];
				nY = nTestCenterY10 - pnBallEdgeY10[k];
				pnEdgeRadiusSqr[k] = nX * nX + nY * nY;
			}
			AdvHistogram.GetFastMaxConvHist1D(&pnEdgeRadiusSqr[0], 
										  nBallEdge,
										  nHistConv,
										  nMaxHist,
										  nMaxHistTemp,
										  nMaxHistPosTemp);
			if(nMaxHist < nMaxHistTemp) {
				nMaxHist = nMaxHistTemp;
				nMaxHistPos = nMaxHistPosTemp;
				*ppt2DBallCenter = *ppt2DEstimatedCenter + pt2DShift;
			}
		}
	}
	*pdRadiusInsp = sqrt(nMaxHistPos/100.0);
	*pnPercentage = (nMaxHist * 100)/nBallEdge;
	delete [] pnEdgeRadiusSqr;
	delete [] pnBallEdgeX10;
	delete [] pnBallEdgeY10;
	return 0;
ErrorAbort:
	return -1;
}

CInterpolationBy2Pts::CInterpolationBy2Pts()
{
	ppt2DSorting = NULL;
	pnMappingIndex = NULL;
	pdx = NULL;
	pdy = NULL;
	nNo = 0;
}

CInterpolationBy2Pts::~CInterpolationBy2Pts()
{
	Clean();
}
	
void CInterpolationBy2Pts::Clean()
{
	if(ppt2DSorting != NULL)
		delete [] ppt2DSorting;
	ppt2DSorting = NULL;
	if(pnMappingIndex != NULL)
		delete [] pnMappingIndex;
	pnMappingIndex = NULL;
 	if(pdx != NULL)
		delete [] pdx;
	pdx = NULL;
	if(pdy != NULL)
		delete [] pdy;
	pdy = NULL;
	nNo = 0;
}


int CInterpolationBy2Pts::SetPointAry(CPoint* pPt,
					  int nPtNo)
{
	int i, j;
	int nStart;
	int nEnd;
	int* pnSortingIndex;
try {
	Clean();
	pnSortingIndex = NULL;
	if(nPtNo < 3)
		goto ErrorAbort;
	nNo = nPtNo;
	ppt2DSorting = new CPoint2D[abs(nNo)];
	pdx = new double[abs(nNo)];
	pdy = new double[abs(nNo)];
	pnSortingIndex = new int[abs(nNo)];
	for(i=0;i<nNo;i++)
		pdx[i] = pPt[i].x;
	Sorting(&pdx[0], nNo, pnSortingIndex, TRUE);
	for(i=0;i<nNo;i++) {
		pdy[i] = pPt[pnSortingIndex[i]].y;
		ppt2DSorting[i] = CPoint2D(pdx[i], pdy[i]);
	}
	nMappingNo = Round(pdx[nNo-1] - pdx[0] + 2);
	pnMappingIndex = new int[abs(nMappingNo)];
	for(i=0;i<(nNo-1);i++) {
		nStart = int(pdx[i] - pdx[0] - 1);
		if(nStart < 0)
			nStart = 0;
		nEnd = int(pdx[i+1] - pdx[0] + 1);
		if(nEnd > nMappingNo)
			nEnd = nMappingNo;
		for(j=nStart;j<nEnd;j++)
			pnMappingIndex[j] = i;
	}
	nStart = int(pdx[nNo-1] - pdx[0] - 1);
	if(nStart < 0)
		nStart = 0;
	if(nStart > nMappingNo)
		nStart = nMappingNo;
	nEnd = nMappingNo;
	for(j=nStart;j<nEnd;j++)
		pnMappingIndex[j] = nNo-1;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CInterpolationBy2Pts::SetPointAry");
	goto ErrorAbort;
}
	if(pnSortingIndex != NULL)
		delete [] pnSortingIndex;
	return 0;
ErrorAbort:
	Clean();
	if(pnSortingIndex != NULL)
		delete [] pnSortingIndex;
	return -1;
}


int CInterpolationBy2Pts::SetPoint2DAry(CPoint2D* pPt2D,
					  int nPt2DNo)
{
	int i, j;
	int nStart;
	int nEnd;
	int* pnSortingIndex;
try {
	Clean();
	pnSortingIndex = NULL;
	if(nPt2DNo < 3)
		goto ErrorAbort;
	nNo = nPt2DNo;
	ppt2DSorting = new CPoint2D[abs(nNo)];
	pdx = new double[abs(nNo)];
	pdy = new double[abs(nNo)];
	pnSortingIndex = new int[abs(nNo)];
	for(i=0;i<nNo;i++)
		pdx[i] = pPt2D[i].x;
	Sorting(&pdx[0], nNo, pnSortingIndex, TRUE);
	for(i=0;i<nNo;i++) {
		pdy[i] = pPt2D[pnSortingIndex[i]].y;
		ppt2DSorting[i] = CPoint2D(pdx[i], pdy[i]);
	}
	nMappingNo = Round(pdx[nNo-1] - pdx[0] + 2);
	pnMappingIndex = new int[abs(nMappingNo)];
	for(i=0;i<(nNo-1);i++) {
		nStart = int(pdx[i] - pdx[0]);
		if(nStart < 0)
			nStart = 0;
		nEnd = int(pdx[i+1] - pdx[0]);
		if(nEnd > nMappingNo)
			nEnd = nMappingNo;
		for(j=nStart;j<nEnd;j++)
			pnMappingIndex[j] = i;
	}
	nStart = int(pdx[nNo-1] - pdx[0]);
	if(nStart < 0)
		nStart = 0;
	if(nStart > nMappingNo)
		nStart = nMappingNo;
	nEnd = nMappingNo;
	for(j=nStart;j<nEnd;j++)
		pnMappingIndex[j] = nNo-1;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CInterpolationBy2Pts::SetPoint2DAry");
	goto ErrorAbort;
}
	if(pnSortingIndex != NULL)
		delete [] pnSortingIndex;
	return 0;
ErrorAbort:
	Clean();
	if(pnSortingIndex != NULL)
		delete [] pnSortingIndex;
	return -1;
}


int CInterpolationBy2Pts::GetIndex(double dx,
								   int* pnIndex)
{
	int nDisX;
try {
	if(nNo < 3)
		goto ErrorAbort;
	nDisX = Round(dx - pdx[0]);
	if((nDisX >= 0) && (nDisX < nMappingNo))
		*pnIndex = pnMappingIndex[nDisX];
	else {
		if(nDisX < 0)
			*pnIndex = 0;
		else
			*pnIndex = nNo - 1;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CInterpolationBy2Pts::GetIndex");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


	
int CInterpolationBy2Pts::GetY(double dx, double* pdInterpolationy)
{
	int i;
	int nLftIndex[2];
	int nLftPtNo;
	int nRhtIndex[2];
	int nRhtPtNo;
	CPoint2D pt2D0;
	CPoint2D pt2D1;
	double dSelectX[2];
	double dSelectY[2];
	int nDisX;
	int nSelectIndex;
try {
	if(nNo < 2)
		goto ErrorAbort;
	nDisX = int(dx - pdx[0]);
	if(nDisX < 0)
		nDisX = 0;
	if(nDisX >= nMappingNo)
		nDisX = nMappingNo - 1;
	nSelectIndex = pnMappingIndex[nDisX];
	nLftPtNo = 0;
	for(i=nSelectIndex;i>=0;i--) {
		nLftIndex[nLftPtNo++] = i;
		if(nLftPtNo >= 2)
			break;
	}
	nRhtPtNo = 0;
	for(i=nSelectIndex+1;i<nNo;i++) {
		nRhtIndex[nRhtPtNo++] = i;
		if(nRhtPtNo >= 2)
			break;
	}
	if((nLftPtNo + nRhtPtNo) < 2)
		goto ErrorAbort;
	if(nRhtPtNo == 0) 
		nLftPtNo = 2;
	if(nLftPtNo == 0) 
		nRhtPtNo = 2;
	if(nRhtPtNo == 1) 		
		nLftPtNo = 1;
	if(nLftPtNo == 1) 
		nRhtPtNo = 1;
	if((nLftPtNo >= 1) && (nRhtPtNo >= 1)) {
		nLftPtNo = 1;
		nRhtPtNo = 1;
	}

	nSelectIndex = 0;
	for(i=0;i<nLftPtNo;i++) {
		dSelectX[nSelectIndex] = pdx[nLftIndex[i]];
		dSelectY[nSelectIndex] = pdy[nLftIndex[i]];
		nSelectIndex++;
	}
	for(i=0;i<nRhtPtNo;i++) {
		dSelectX[nSelectIndex] = pdx[nRhtIndex[i]];
		dSelectY[nSelectIndex] = pdy[nRhtIndex[i]];
		nSelectIndex++;
	}

	if((fabs(dSelectX[0] - dSelectX[1]) < 0.1))
	   goto ErrorAbort;
	*pdInterpolationy = dSelectY[0] + (dSelectY[1] - dSelectY[0])*(dx - dSelectX[0])/(dSelectX[1] - dSelectX[0]);
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CInterpolationBy2Pts::GetY #1");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


int CInterpolationBy2Pts::GetYByMinDisPts(double dx, double* pdInterpolationy)
{
	int i;
	int nLftIndex[2];
	int nLftPtNo;
	int nRhtIndex[2];
	int nRhtPtNo;
	CPoint2D pt2D0;
	CPoint2D pt2D1;
	double dSelectX[2];
	double dSelectY[2];
	int nDisX;
	int nSelectIndex;
	double dDis[3];
	double dMinDis;
	int nMinDisIndex;
try {
	if(nNo < 2)
		goto ErrorAbort;
	nDisX = int(dx - pdx[0]);
	if(nDisX < 0)
		nDisX = 0;
	if(nDisX >= nMappingNo)
		nDisX = nMappingNo - 1;
	nSelectIndex = pnMappingIndex[nDisX];
	nLftPtNo = 0;
	for(i=nSelectIndex;i>=0;i--) {
		nLftIndex[nLftPtNo++] = i;
		if(nLftPtNo >= 2)
			break;
	}
	nRhtPtNo = 0;
	for(i=nSelectIndex+1;i<nNo;i++) {
		nRhtIndex[nRhtPtNo++] = i;
		if(nRhtPtNo >= 2)
			break;
	}
	dDis[0] = INT_MAX;
	dDis[1] = INT_MAX;
	dDis[2] = INT_MAX;
	if(nLftPtNo == 2) {
		dDis[0] = 2 * dx - pdx[nLftIndex[0]] - pdx[nLftIndex[1]];
	}
	if(nRhtPtNo == 2) {
		dDis[2] = pdx[nRhtIndex[0]] + pdx[nRhtIndex[1]] - 2 * dx;
	}
	if((nLftPtNo>=1)&&(nRhtPtNo >= 1)) {
		dDis[1] = pdx[nRhtIndex[0]] - pdx[nLftIndex[0]];
	}
	dMinDis = dDis[0];
	nMinDisIndex = 0;
	for(i=1;i<3;i++)
	{
		if(dMinDis > dDis[i])
		{
			dMinDis = dDis[i];
			nMinDisIndex = i;
		}
	}
	switch(nMinDisIndex)
	{
	case 0:
		nLftPtNo = 2;
		nRhtPtNo = 0;
		break;
	case 1:
		nLftPtNo = 1;
		nRhtPtNo = 1;
		break;
	case 2:
		nLftPtNo = 0;
		nRhtPtNo = 2;
		break;
	}

	if((nLftPtNo + nRhtPtNo) < 2)
		goto ErrorAbort;
	if(nRhtPtNo == 0) 
		nLftPtNo = 2;
	if(nLftPtNo == 0) 
		nRhtPtNo = 2;
	if(nRhtPtNo == 1) 		
		nLftPtNo = 1;
	if(nLftPtNo == 1) 
		nRhtPtNo = 1;
	if((nLftPtNo >= 1) && (nRhtPtNo >= 1)) {
		nLftPtNo = 1;
		nRhtPtNo = 1;
	}

	nSelectIndex = 0;
	for(i=0;i<nLftPtNo;i++) {
		dSelectX[nSelectIndex] = pdx[nLftIndex[i]];
		dSelectY[nSelectIndex] = pdy[nLftIndex[i]];
		nSelectIndex++;
	}
	for(i=0;i<nRhtPtNo;i++) {
		dSelectX[nSelectIndex] = pdx[nRhtIndex[i]];
		dSelectY[nSelectIndex] = pdy[nRhtIndex[i]];
		nSelectIndex++;
	}

	if((fabs(dSelectX[0] - dSelectX[1]) < 0.1))
	   goto ErrorAbort;
	*pdInterpolationy = dSelectY[0] + (dSelectY[1] - dSelectY[0])*(dx - dSelectX[0])/(dSelectX[1] - dSelectX[0]);
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CInterpolationBy2Pts::GetY #2");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

CInterpolationBy3Pts::CInterpolationBy3Pts()
{
	ppt2DSorting = NULL;
	pnMappingIndex = NULL;
	pdx = NULL;
	pdy = NULL;
	pdGrad = NULL;
	pParmA = NULL;
	pParmB = NULL;
	nNo = 0;
}

CInterpolationBy3Pts::~CInterpolationBy3Pts()
{
	Clean();
}
	
void CInterpolationBy3Pts::Clean()
{
	if(ppt2DSorting != NULL)
		delete [] ppt2DSorting;
	ppt2DSorting = NULL;
	if(pnMappingIndex != NULL)
		delete [] pnMappingIndex;
	pnMappingIndex = NULL;
 	if(pdx != NULL)
		delete [] pdx;
	pdx = NULL;
	if(pdy != NULL)
		delete [] pdy;
	pdy = NULL;
	if(pdGrad != NULL)
		delete [] pdGrad;
	pdGrad = NULL;
	if(pParmA != NULL)
		delete [] pParmA;
	pParmA = NULL;
	if(pParmB != NULL)
		delete [] pParmB;
	pParmB = NULL;
	nNo = 0;
}


int CInterpolationBy3Pts::CalculateGradVal()
{
	int i;
	double d0, d1, d2;
	double dGrad0;
try {
	pdGrad[0] = (pdy[1] - pdy[0])/(pdx[1] - pdx[0]);
	for(i=1;i<(nNo-1);i++) {
		d0 = (pdx[i-1] - pdx[i]) * (pdx[i-1] - pdx[i+1]);
		d1 = (pdx[i] - pdx[i-1]) * (pdx[i] - pdx[i+1]);
		d2 = (pdx[i+1] - pdx[i-1]) * (pdx[i+1] - pdx[i]);

		pdGrad[i] = (pdx[i] - pdx[i+1]) * pdy[i-1] / d0 +
					(2 * pdx[i] - pdx[i-1] - pdx[i+1]) * pdy[i] / d1 +
					(pdx[i] - pdx[i-1]) * pdy[i+1] / d2;
	}
	pdGrad[nNo-1] = (pdy[nNo-1] - pdy[nNo-2])/(pdx[nNo-1] - pdx[nNo-2]);
	for(i=0;i<(nNo-1);i++) {
		dGrad0 = (pdy[i+1] - pdy[i])/(pdx[i+1] - pdx[i]);
		pParmA[i] = (pdGrad[i] - dGrad0)/(pdx[i] - pdx[i+1]);
		pParmB[i] = (pdGrad[i+1] - dGrad0 - pParmA[i] * (pdx[i+1] - pdx[i]))/((pdx[i+1] - pdx[i]) * (pdx[i+1] - pdx[i]));
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CInterpolationBy3Pts::CalculateGradVal #1");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int CInterpolationBy3Pts::SetPoint2DAry(CPoint2D* pPt2D,
					  int nPt2DNo)
{
	int i, j;
	int nStart;
	int nEnd;
	int* pnSortingIndex;
	double dAccuracyTol;
	double dSumX;
	double dSumY;
	int nSumNo;
	int nIndex;
	int nError;
	
try {	
	Clean();
	dAccuracyTol = 0.00001;
	pnSortingIndex = NULL;
	if(nPt2DNo < 3)
		goto ErrorAbort;
	nNo = nPt2DNo;
	ppt2DSorting = new CPoint2D[abs(nNo)];
	pdx = new double[abs(nNo)];
	pdy = new double[abs(nNo)];
	pdGrad = new double[abs(nNo)];
	pParmA = new double[abs(nNo)];
	pParmB = new double[abs(nNo)];

	pnSortingIndex = new int[abs(nNo)];
	for(i=0;i<nNo;i++)
		pdx[i] = pPt2D[i].x;
	Sorting(&pdx[0], nNo, pnSortingIndex, TRUE);
	nIndex = 0;
	for(i=0;i<nNo;i++) {
		dSumX = pdx[i];
		dSumY = pPt2D[pnSortingIndex[i]].y;
		nSumNo = 1;
		for(j=i+1;j<nNo;j++) {
			if((pdx[j] - pdx[i]) > dAccuracyTol) {
				i = j - 1;
				break;
			}
			dSumX += pdx[j];
			dSumY += pPt2D[pnSortingIndex[j]].y;
			nSumNo++;
		}
		pdx[nIndex] = dSumX/nSumNo;
		pdy[nIndex] = dSumY/nSumNo;
		ppt2DSorting[nIndex] = CPoint2D(pdx[nIndex], pdy[nIndex]);
		nIndex++;
	}
	nNo = nIndex;
	nError = CalculateGradVal();
	nMappingNo = Round(pdx[nNo-1] - pdx[0] + 2);
	pnMappingIndex = new int[abs(nMappingNo)];
	for(i=0;i<(nNo-1);i++) {
		nStart = int(pdx[i] - pdx[0] - 1);
		if(nStart < 0)
			nStart = 0;
		nEnd = int(pdx[i+1] - pdx[0] + 1);
		if(nEnd > nMappingNo)
			nEnd = nMappingNo;
		for(j=nStart;j<nEnd;j++)
			pnMappingIndex[j] = i;
	}
	nStart = int(pdx[nNo-1] - pdx[0] - 1);
	if(nStart < 0)
		nStart = 0;
	if(nStart > nMappingNo)
		nStart = nMappingNo;
	nEnd = nMappingNo;
	for(j=nStart;j<nEnd;j++)
		pnMappingIndex[j] = nNo-2;
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CInterpolationBy3Pts::CalculateGradVal #2");
	goto ErrorAbort;
}
	if(pnSortingIndex != NULL)
		delete [] pnSortingIndex;
	return 0;
ErrorAbort:
	Clean();
	if(pnSortingIndex != NULL)
		delete [] pnSortingIndex;
	return -1;
}



int CInterpolationBy3Pts::ModifyPoint(double* pdDifY,
				 BOOL* pbMatch,
				 int nInterpolationNo,
				 int nStartX,
				 int nEndX,
				 int nSegmentMatchingPercentageTol,
				 double dDifYTol)
{
	int i, j;
	int nSegmentStartX;
	int nSegmentEndX;
	int nOverlapStartX;
	int nOverlapEndX;
	double* pdSegmentDifY;
	int nSegmentDifYNo;
	CAdvHistogram AdvHistogram;
	int	nMaxHist;
	double dAvgDifY;
	CPoint2D* pPt2D;
	int nPt2DNo;
	int nError;
try {
	pdSegmentDifY = NULL;
	pPt2D = NULL;
	if(nNo < 3)
		goto PASS;
	pdSegmentDifY = new double[abs(nInterpolationNo)];
	for(i=0;i<nNo;i++) {
		if((i> 0) && (i<(nNo-1))) {
			nSegmentStartX = Round((ppt2DSorting[i-1].x+ppt2DSorting[i].x)/2);
			nSegmentEndX = Round((ppt2DSorting[i].x+ppt2DSorting[i+1].x)/2);
		}
		else {
			if(i==0) {
				nSegmentStartX = min(nStartX, Round(ppt2DSorting[0].x));
				nSegmentEndX = Round((ppt2DSorting[0].x+ppt2DSorting[1].x)/2);
			}
			if(i==(nNo-1)) {
				nSegmentStartX = Round((ppt2DSorting[nNo-2].x + ppt2DSorting[nNo-1].x)/2);
				nSegmentEndX = max(nEndX, Round(ppt2DSorting[nNo-1].x));
			}
		}
		nOverlapStartX = __max(nStartX, nSegmentStartX);
		nOverlapEndX = __min(nEndX, nSegmentEndX);
		nSegmentDifYNo = 0;
		for(j=nOverlapStartX-nStartX;j<(nOverlapEndX - nStartX);j++) {
			if(pbMatch[j] == FALSE)
				continue;
			pdSegmentDifY[nSegmentDifYNo++] = pdDifY[j];
		}
		nError = AdvHistogram.GetMaxConvHist1D(&pdSegmentDifY[0], 
									nSegmentDifYNo,
									dDifYTol,
									nMaxHist,
									dAvgDifY);

		if(nError)
			goto ErrorAbort;
		if(nOverlapEndX == nOverlapStartX)
			continue;
		if(fabs(dAvgDifY) > dDifYTol)
			continue;
		if((nMaxHist * 100.0/(nOverlapEndX - nOverlapStartX)) > nSegmentMatchingPercentageTol) {
			ppt2DSorting[i] = CPoint2D(ppt2DSorting[i].x, ppt2DSorting[i].y + dAvgDifY);
		}
		pdx[i] = ppt2DSorting[i].x;
		pdy[i] = ppt2DSorting[i].y;
	}

	nPt2DNo = nNo;
	pPt2D = new CPoint2D[nPt2DNo];
	for(i=0;i<nPt2DNo;i++)
		pPt2D[i] = CPoint2D(pdx[i], pdy[i]);
	nError = SetPoint2DAry(pPt2D,
					  nPt2DNo);

}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CInterpolationBy3Pts::ModifyPoint");
	goto ErrorAbort;
}
PASS:
	if(pdSegmentDifY != NULL)
		delete [] pdSegmentDifY;
	if(pPt2D != NULL)
		delete [] pPt2D;
	return 0;
ErrorAbort:
	if(pdSegmentDifY != NULL)
		delete [] pdSegmentDifY;
	if(pPt2D != NULL)
		delete [] pPt2D;
	return -1;

}

int CInterpolationBy3Pts::GetIndex(double dx,
								   int* pnIndex)
{
	int nDisX;
try {
	if(nNo < 3)
		goto ErrorAbort;
	nDisX = Round(dx - pdx[0]);
	if((nDisX >= 0) && (nDisX < nMappingNo))
		*pnIndex = pnMappingIndex[nDisX];
	else {
		if(nDisX < 0)
			*pnIndex = 0;
		else
			*pnIndex = nNo - 1;
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CInterpolationBy3Pts::GetIndex");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


	
int CInterpolationBy3Pts::GetY(double dx, double* pdInterpolationy)
{
	int nDisX;
	int nSelectIndex;
try {
	if(nNo < 3)
		goto ErrorAbort;
	nDisX = int(dx - pdx[0]);
	if(nDisX < 0)
		nDisX = 0;
	if(nDisX >= nMappingNo)
		nDisX = nMappingNo - 1;
	nSelectIndex = pnMappingIndex[nDisX];
	*pdInterpolationy = pdy[nSelectIndex] + (pdy[nSelectIndex+1] - pdy[nSelectIndex])/(pdx[nSelectIndex+1] - pdx[nSelectIndex]) * (dx - pdx[nSelectIndex]) +
						pParmA[nSelectIndex] * (dx - pdx[nSelectIndex]) * (dx - pdx[nSelectIndex+1]) +
						pParmB[nSelectIndex] * (dx - pdx[nSelectIndex]) * (dx - pdx[nSelectIndex]) * (dx - pdx[nSelectIndex+1]);
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"CInterpolationBy3Pts::GetY");
	goto ErrorAbort;
}

	return 0;
ErrorAbort:
	return -1;
}

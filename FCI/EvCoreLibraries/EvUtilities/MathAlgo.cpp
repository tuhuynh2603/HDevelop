#include "stdafx.h"
#include <math.h>
#include "MathAlgo.h"
#include "MathValidations.h"
#include "Utilities.h"


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

int Round(double d)
{
	if(d>=0) 
		return (int)(d + 0.5); 
	else 
		return (int)(d - 0.5); 
}

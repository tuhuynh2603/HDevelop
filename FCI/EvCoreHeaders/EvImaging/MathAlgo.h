#pragma once

#include "stdafx.h"
#include "afxtempl.h"
#include <math.h>


#ifndef PI
#define PI 3.1415926535
#endif

class CPoint2D {
public:
	double x,y;
	CPoint2D();	
	CPoint2D(CPoint); 
	CPoint2D(int x0, int y0);
	CPoint2D(double x0, double y0);
	CPoint2D(int x0, double y0);
	CPoint2D(double x0, int y0);
	CPoint2D(const CPoint2D &);

	double GetDistance(CPoint2D);
	double GetDistance(CPoint2D* ppt2D);
	CPoint2D GetInterpol(CPoint2D* ppt2DStart,
						 CPoint2D* ppt2DEnd,
						 double dRatio);

	void SetPoint(const CPoint2D &pt2D);
	void RotateAbout(CPoint2D pt2D,
					 double Angle);

	CPoint2D & operator =(CPoint2D pt2d);
	BOOL operator ==(CPoint2D);
	CPoint2D  & operator += (const CPoint2D &v);
	CPoint2D  & operator -= (const CPoint2D &v);
	CPoint2D  & operator /= (const double d);
	CPoint2D  & operator / (const double d);
	CPoint2D operator -(CPoint2D pt2dSub);
	CPoint2D operator +(CPoint2D pt2dAdd);
};

class CSize2D {
public:
	double cx,cy;

	CSize2D();	
	CSize2D(CSize); 
	CSize2D(int x0, int y0);
	CSize2D(double x0, double y0);
	CSize2D(const CSize2D &);

	CSize2D & operator =(CSize2D cs2d);
	CSize2D & operator =(CSize cs);
	BOOL operator ==(CSize2D);
};

class CPoint2DArray : public CArray<CPoint2D,CPoint2D &>
{
public:
	CPoint2DArray();
	~CPoint2DArray();
	CPoint2DArray(CPoint* ppt, int nptNo);
	CPoint2DArray(CPoint2D* ppt2D, int npt2DNo);
	CPoint2DArray(CPoint2DArray &);
	CPoint2DArray & operator = (CPoint2DArray &);
	int GetGravityCenter(CPoint2D* ppt2DGravityCenter);
	void Zoom(CPoint2D* ppt2DCenter, double dRatio);
};

class CVector2D : public CPoint2D{
public:	
	CVector2D();
	CVector2D(CPoint ptDst,CPoint ptSrc);
	CVector2D(CPoint2D pt2DDst,CPoint2D ptSrc=CPoint2D(0,0));
	CVector2D(int nX0,int nY0);
	CVector2D(double dX0,double dY0);
	CVector2D(const CVector2D &);
	double GetSize();
	int Normalize();
	CVector2D & operator =(CVector2D);
	int GetUnit(CVector2D* pvctDst);				// Pointer to A 2-D Vector
	double operator ^ (CVector2D);					// A 2-D Vector
	BOOL IsValid();
	int GetAngleRad(CVector2D vt2D,					// A 2D Vector
					double& dAngleRad);				// Angle in Radians
};

class CLine2D {
public:
	CLine2D();
	CLine2D(CPoint2D pt2D,								// A Point on the Line			
			CVector2D vt2D);							// Line Vector
	
	CLine2D(CPoint2D pt2D,								// A Point on Line
			double dAngle);								// Angle of the Line

	CLine2D(CPoint2D pt2D0,								// A Point on the Line
			CPoint2D pt2D1);							// Another Point on the Line

	CLine2D(CPoint pt0, 								// A Point on the Line
			CPoint pt1);								// Another Point on the Line
	 ~CLine2D();

	CLine2D & operator =(CLine2D ln2D);

	BOOL IsInValid();
	void Reset();

	void SetPosition(double dPosX, double dPosY); 
	
	void SetPosition(CPoint2D pt2D); 

	int SetDirection(double dVt2DX, double dVt2DY);

	int SetDirection(double dAngle);

	int SetDirection(CVector2D vt2D);

	CPoint2D GetPosition(); 

	CVector2D GetDirection();

	int GetParallelPoints(CPoint2D* ppt2DSrc, 
						  double dAbsDis, 
						  CPoint2D* ppt2DDst0,
						  CPoint2D* ppt2DDst1);

	int GetParallelPoint(CPoint2D* ppt2DSrc, 
						 double dAbsDis, 
						 int nXCheckFlag,
						 int nYCheckFlag,
						 CPoint2D* ppt2DDst);

	int GetPerpendicularPoints(CPoint2D* ppt2DSrc, 
							   double dAbsDis, 
							   CPoint2D* ppt2DDst0,
							   CPoint2D* ppt2DDst1);

	int GetPerpendicularPoint(CPoint2D* ppt2DSrc, 
							  double dAbsDis, 
							  int nXCheckFlag,
							  int nYCheckFlag,
							  CPoint2D* ppt2DDst);

	int GetDiagonalPoints(CPoint2D* ppt2DSrc, 
						  double dAbsParallelDis, 
						  double dAbsPerpendicularDis, 
						  CPoint2D* ppt2DParallelPair0Pt0,
						  CPoint2D* ppt2DParallelPair0Pt1,
						  CPoint2D* ppt2DParallelPair1Pt0,
						  CPoint2D* ppt2DParallelPair1Pt1);

	int IsOnLine(CPoint2D* ppt2DSrc, 					// Pointer to A Point	 
				 BOOL* pbOnLine);						// Status whether the point is on the line: 1:On Line;  0: Not On Line

	int GetDistance(CPoint2D* ppt2DSrc, 				// Pointer to A Point	 
				    double* pdDis);						// Pointer to Absolute Distance

	int GetDistance(CPoint* pptSrc,
					double* pdDis);

	int GetSignedDistance(CPoint2D* ppt2DSrc, 			// Pointer to A Point	 
						  double* pdDis);				// Pointer to Signed Distance

	int GetSignedDistance(CPoint* pptSrc, 			// Pointer to A Point	 
						  double* pdDis);				// Pointer to Signed Distance

	int GetDistanceX(CPoint2D* ppt2DSrc,				// Pointer to A Point	 
					 double* pdDis);					// Pointer to Horizontal Distance

	int GetDistanceY(CPoint2D* ppt2DSrc,				// Pointer to A Point	
					 double* pdDis);					// Pointer to Vertical Distance

	int GetProjectionX(CPoint2D* pptSrc, 				// Pointer to A Point	
					   CPoint2D* pptProj);				// Pointer to Projection Point	

	int GetProjectionY(CPoint2D* pptSrc,  				// Pointer to A Point	
					   CPoint2D* pptProj);				// Pointer to Projection Point	

	int GetProjection(CPoint2D* ppt2dSrc, 				// Pointer to A Point	
					  CPoint2D* ppt2DProj);				// Pointer to Projection Point	

	int IsOnSameSide(CPoint2D* pptSrc1,					// Pointer to A Point 
					 CPoint2D* pptSrc2,					// Pointer to Another Point
					 BOOL* pbOnSameSide);				// Status whether two points are on the same side of the line

	int GetBiSectLine(CLine2D *pln2DLine1, 
					  CLine2D *pln2DLine2);

	int GetAverageLine(CLine2D *pln2D, 
					   int nln2D); 
	
	int GetBestFittingLine(CPoint2D* ppt2D,				// Pointer to a series of Points
						   int nCount);					// Number of Points

	int GetBestFittingLine(CPoint* ppt,
						   int nCount);
	int GetIntercept(CLine2D* pln2D,					// Pointer to A Line
					 CPoint2D* ppt2DIntercept);			// Pointer to Intersection Point

	int GetAngleDeg(double& pdAngle);					// Angle of This Line in Degree

	int GetAngleRad(double& pdAngle);					// Angle of This Line in Radians

	int GetX(double dY,									// Y Coordinate of a Point
			double* dX);								// Pointer to X Coordinate of a Point (that is on this line)
			 

	int GetY(double dX,									// X Coordinate of a Point
			 double* dY);								// Pointer to Y Coordinate of a Point (that is on this line)

	int GetPerpendicularLine(CPoint2D* ppt2D,			// Pointer to A Point on Perpendicular Line	
							 CLine2D* pln2DLine);		// Perpendicular Line

	int GetParallelLine(CPoint2D* ppt2D, 				// Pointer to A Point on Parallel Line	
						CLine2D* pln2DLine);			// Parallel Line

protected:
	CPoint2D Position; //The line passes through the point Position
	CVector2D Direction; //The direction vector of the line
	BOOL m_bInValid;									// Status of This Line :  1: Invalid;  0: Valid
private:
};

class PROJ_1D_X {
public:
	PROJ_1D_X();
	~PROJ_1D_X();
	int nProjX;
	int* pnY;
	int nYSize;
}; 

class PROJ_1D_Y {
public:
	PROJ_1D_Y();
	~PROJ_1D_Y();
	int nProjY;
	int* pnX;
	int nXSize;
}; 


class PROJ_2D_X {
public:
	PROJ_2D_X();
	~PROJ_2D_X();
	void MemLocate(int nInspNo);
	void Clean();
	int* pnSortX;
	int* pnY;
	int* pnSortXIndex;
	int nptSize;
	PROJ_1D_X* pPROJX;
	int* pnHistPosSort;
	int* pnHistConvSort;
	int* pnHistConvSortIndex;
	int nHistConvNo;
	int nHistSize;
	int nHistConv;
	int nMaxCovHist;
	int nMaxCovHistPos;
private:
	int nMemLoc;
};

class PROJ_2D_Y{
public:
	PROJ_2D_Y();
	~PROJ_2D_Y();
	void MemLocate(int nInspNo);
	void Clean();
	int* pnSortY;
	int* pnX;
	int* pnSortYIndex;
	int nptSize;
	PROJ_1D_Y* pPROJY;
	int* pnHistPosSort;
	int* pnHistConvSort;
	int* pnHistConvSortIndex;
	int nHistSize;
	int nHistConv;
	int nMaxCovHist;
	int nMaxCovHistPos;
private:
	int nMemLoc;
};

class  PROJ_2D_Search{
public:
	PROJ_2D_Search();
	~PROJ_2D_Search();
	void MemLocate(int nInspNo);
	void Clean();
	int* pnData;
	int nptSize;
	int* pnHistPosSort;
	int* pnHistConvSort;
	int* pnHistConvSortIndex;
	int nHistSize;
	int nHistConv;
	int nMaxCovHist;
	int nMaxCovHistPos;
};


class ADV_HIST_1D_RSLT {
public:
	ADV_HIST_1D_RSLT();
	~ADV_HIST_1D_RSLT();
	void Clean();
	void MemLocate(int nNo);
	int* pnHist;
	long* plPos; 
	int nHistNo;
};

class ADV_CONV_HIST_1D_RSLT {
public:
	ADV_CONV_HIST_1D_RSLT();
	~ADV_CONV_HIST_1D_RSLT();
	void Clean();
	void MemLocate(int nNo);
	int* pnHist;
	long* plPosStart; 
	long* plPosEnd; 
	int nHistNo;
};


class CAdvHistogram{
protected:
	typedef struct {
		int nFoundMaxPt;
		int nStartProjIndex;
		int nEndProjIndex;
		CRect rcFound;
		bool bFoundByProjX;
	} PROJ_2D_Result;
	PROJ_2D_X ProjX;
	PROJ_2D_Y ProjY;
	PROJ_2D_Result ProjResult;
	ADV_CONV_HIST_1D_RSLT AdvConvHist1DXRslt;
	ADV_CONV_HIST_1D_RSLT AdvConvHist1DYRslt;

public:
	
	int GetHist(int *pnData,
				int nDataNo,
				ADV_HIST_1D_RSLT* pAdvHist1DRslt);

	int GetAccHist(int *pnData,
				   int nDataNo,
				   ADV_HIST_1D_RSLT* pAdvHist1DRslt);

	int GetAccHist(long *plData,
				   int nDataNo,
				   ADV_HIST_1D_RSLT* pAdvHist1DRslt);

	int GetConvHist(int *pnData,
				    int nDataNo,
					int nHistConv,
					ADV_CONV_HIST_1D_RSLT* pAdvConvHist1DRslt);

	int GetConvHist(long *plData,
				    int nDataNo,
					int nHistConv,
					ADV_CONV_HIST_1D_RSLT* pAdvConvHist1DRslt);

	int GetConvHist(ADV_HIST_1D_RSLT* pAdvHist1DRslt,
					int nHistConv,
					ADV_CONV_HIST_1D_RSLT* pAdvConvHist1DRslt);
	
	int GetMaxConvHist1D(int *pnData, 
						 int nDataNo,
						 int nHistConv,
						 int &nMaxHist,
						 int &nMaxHistPos);

	int GetFastMaxConvHist1D(int *pnData, 
							int nDataNo,
							int nHistConv,
							int nHistMinTol,
							int	&nMaxHist,
							int &nMaxHistPos);

	int GetMaxConvHist1D(long *plData, 
						 int nDataNo,
						 int nHistConv,
						 int &nMaxHist,
						 long &lMaxHistPos);

	int GetMaxConvHist2D(CPoint2D *ppt2DInsp,
						 int nInspNo,
						 CSize2D* pcs2DHistConv,
						 CPoint2D *ppt2DSelected,
						 int &nSelectedSize,
						 CPoint2D *ppt2DNOTSelected,
						 int &nNOTSelectedSize);


	int GetMaxConvHist1D(int *pnData, 
						int nDataNo,
						int nHistConv,
						int	&nMaxHist,
						int &nMaxHistPos,
						double &dAvgPos);


	int GetMaxConvHist1D(double *pdData, 
						int nDataNo,
						double dHistConv,
						int	&nMaxHist,
						double &dAvgPos);


protected:
	int Proj2DX(long* plX, long* plY, int nInspNo, int nHistConv);
	int Proj2DY(long* plX, long* plY, int nInspNo, int nHistConv);
	int FindMaxProj2DX(int nProjIndex);
	int FindMaxProj2DY(int nProjIndex);
	int GetFastConvHist1D(ADV_HIST_1D_RSLT* pAdvHist1DRslt,
						  int nHistConv,
						  ADV_CONV_HIST_1D_RSLT* pAdvConvHist1DRslt);
	int GetMaxConvHist1DCmpFoundMax(int *pnData, 
									int nDataNo,
									int nHistConv,
									int nFndMaxHist,
									int	&nMaxHist,
									int &nMaxHistPos);

	PROJ_2D_Search ProjSearch;
};

class CPointPatternCorrelation {
public:
	CPointPatternCorrelation();
	int GetLUTSize(int nTeachPatternNo,				// Number of the Points of the 2D Teach Pattern
				   int nSearchAngle,				// Search Angle
				   double dAngleStep,				// Step of Search Angle
				   long& lLUTSize);					// Size of LUT

	int InitLUT(CPoint *pptTeachPattern,			// 2D Point Array of A Teach Pattern
				int nTeachPatternNo,				// Number of the Points of the 2D Teach Pattern
				int nSearchAngle,					// Search Angle
				double dAngleStep,					// Step of Search Angle
				int *pnLUTX,						// LookUpTable of X Coordinates
				int *pnLUTY);						// LookUpTable of Y Coordinates

	int FindPattern1D(double *pdTeachPos,
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
					  bool bAccurate);

	int FindPeriodicPattern1D(double *pdTeachPos,
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
							  int &nMissingPosInspNo,
							  bool bAccurate);

	int FindPatternPos(CPoint *pptTeachPattern,		// 2D Point Array of A Teach Pattern
					   int m_nTeachPatternNo,		// Number of the Points of the 2D Teach Pattern
					   CPoint *pptInspectPattern,	// 2D Point Array of A Inspection Pattern
					   int nInspectPatternNo,		// Number of the Points of the 2D Inspection Pattern
					   int nSearchAngle,			// Search Angle
					   double dAngleStep,			// Step of Search Angle
					   int nXShift,					// X Shift
					   int nYShift,					// Y Shift
					   int nCenterUncertainty,		// Tolerance of Point Matching
					   int *pnLUTX,					// LookUpTable of X Coordinates
					   int *pnLUTY,					// LookUpTable of Y Coordinates
					   double &dFndAngle,			// Found Rotation Angle
					   double &dFndXShift,			// Found X Shift
					   double &dFndYShift);			// Found Y Shift

	void PointSorting(CPoint *pptTeachPattern,				// 2D Point Array of A Teach Pattern
						 int nTeachPatternNo,			// Number of the Points of the 2D Teach Pattern
						 CPoint *pptInspectPattern,		// 2D Point Array of A Inspection Pattern
						 BOOL *pbInspectPattern,		// Status of Matching: 1: Matching; 0: Not Matching
						 int nInspectPatternNo,			// Number of the Points of the 2D Inspection Pattern
						 int nCenterUncertainty,		// Tolerance of Point Matching
						 double &dFndAngle,				// Found Rotation Angle
						 double &dFndXShift,			// Found X Shift
						 double &dFndYShift);			// Found Y Shift
};


class CHoughTransform {
public:

	CHoughTransform();
	int GetLineLUTStatus(CRect* prcROI,					// Pointer to ROI
				  double dNominalAngleRad,			// Nominal Angle 
				  double dTolAngleRad,				// Angle Tolerance
				  double dAngleStep,				// Step of Angle
				  bool& bLUTChangeStatus,			// Status of Change of LUT: 1: Change LUT. 0: Not Change LUT
				  bool& bLUTStatus);				// Status of LUT:			1: LUT Generated. 0: LUT Not Generated

	int GetLineLUTSize(CRect* prcROI,					// Pointer to ROI
				   double dTolAngleRad,				// Angle Tolerance
				   double dAngleStep,				// Step of Angle
				   long& lLUTSize);					// Size of LUT

	int InitLineLUT(CRect* prcROI,						// Pointer to ROI
				double dNominalAngleRad,			// Nominal Angle 
				double dTolAngleRad,				// Angle Tolerance
				double dAngleStep,					// Step of Angle
				short int* pnLUT);					// Pointer to LUT

	int LineLUTHoughTransform(CPoint2D *ppt2DInsp,		// Pointer to Inspected Points
						int nInspNo,				// Number of Inspected points
						short int* pnLUT,			// Pointer to LUT
						long lLUTSize,				// Size of LUT
						int nOnLineTol,				// Tolerance of Distance between A Point and Hough Transform Line
						CPoint2D *ppt2DOnLine,		// Pointer to the Points On Hough Transform Line
						int &nOnLineSize,			// Number of Points On Hough Transform Line
						CPoint2D *ppt2DNOTOnLine,	// Pointer to the Points NOT On Hough Transform Line
						int &nNOTOnLineSize);		// Number of Points NOT On Hough Transform Line
//////////////////////////////////////
//This function has a bug and will be removed.
//Please use the function "HoughTransformLine"
	int LineHoughTransform(CPoint2D *ppt2DInsp,		// Pointer to Inspected Points
						int nInspNo,				// Number of Inspected points
						double dNominalAngleRad,	// Nominal Angle 
						double dTolAngleRad,		// Angle Tolerance
						double dAngleStep,			// Step of Angle
						double dOnLineTol,			// Tolerance of Distance between A Point and Hough Transform Line
						CPoint2D *ppt2DOnLine,		// Pointer to the Points On Hough Transform Line
						int &nOnLineSize,			// Number of Points On Hough Transform Line
						CPoint2D *ppt2DNOTOnLine,	// Pointer to the Points NOT On Hough Transform Line
						int &nNOTOnLineSize);		// Number of Points NOT On Hough Transform Line
///////////////////////////////////////////////////

	int HoughTransformLine(CPoint2D *ppt2DInsp,		// Pointer to Inspected Points
						int nInspNo,				// Number of Inspected points
						double dNominalAngleRad,	// Nominal Angle 
						double dTolAngleRad,		// Angle Tolerance
						double dAngleStep,			// Step of Angle
						double dOnLineTol,			// Tolerance of Distance between A Point and Hough Transform Line
						CPoint2D *ppt2DOnLine,		// Pointer to the Points On Hough Transform Line
						int &nOnLineSize,			// Number of Points On Hough Transform Line
						CPoint2D *ppt2DNOTOnLine,	// Pointer to the Points NOT On Hough Transform Line
						int &nNOTOnLineSize, 		// Number of Points NOT On Hough Transform Line
						int nMinOnLinePercentageForFastSearch = 60);

private:
	CAdvHistogram AdvHistogram;
	int m_nAngleFound;
	bool m_bLUTStatus;
	bool m_bLUTChangeStatus;
	int m_nLUTWidth;
	int m_nLUTHeight;
	double m_dTableNominalAngle;
	double m_dTableAngleTol;
	double m_dTableAngleStep;
	int m_nTableAngleSize;
	long m_lROISize;
	long m_lLUTSize;
	long m_lHoughSize;
	CRect m_rcBounding;
	double m_dAngleMin;
	double m_dAngleMax;

	void GetBoundingRect(CPoint *pptInsp,
						 int nInspNo,
						 CRect* prcBounding);
	void GetOffsetPoints(CPoint* pptSrc,
						 int nSrc,
						 CPoint* pptDst,
						 CPoint* pptOffset);
};

class CPoint3D {
public:
	double x,y,z;
	CPoint3D();
	CPoint3D(CPoint,double dZ0=0);
	CPoint3D(double dX0,double dY0,double dZ0=0);
	CPoint3D(const CPoint3D &);
	CPoint3D & operator =(CPoint3D);
	CPoint3D operator +(CPoint3D);
	CPoint3D operator -(CPoint3D);
	CPoint3D operator *(double);
	CPoint3D operator /(double);
	CPoint3D &operator +=(CPoint3D);
	CPoint3D &operator -=(CPoint3D);
	CPoint3D &operator *=(double);
	CPoint3D &operator /=(double);
	BOOL operator == (CPoint3D);
	double GetDistance(CPoint3D);						// A 3-D Point
};

class  CVector3D : public CPoint3D{
public:	
	CVector3D();
	CVector3D(CPoint3D pt3DDst,CPoint3D pt3DSrc=CPoint3D(0,0,0));
	CVector3D(double dX0,double dY0,double dZ0=0);
	CVector3D(const CVector3D &);
	//CVector3D(const CVector2D &);
	double GetSize();  
	int Normalize();
	CVector3D & operator =(CVector3D) ;
	CVector3D operator *(CVector3D);
	CVector3D operator *(double d);
	int GetUnit(CVector3D* pvct3D);
	double operator ^ (CVector3D);
	BOOL IsValid();
	int GetAngleRad(CVector3D vt3D,							// A 3-D Vector
					double& dAngleRad);						// Angle between the Vector and This Vector
};


class  CLine3D {
public:
	CPoint3D Position; //The line passes through the point Position
	CVector3D Direction; //The direction vector of the line
public:
	CLine3D();
	CLine3D(CPoint3D pt3D,CVector3D vt3D);
	CLine3D(CPoint3D pt3D0,CPoint3D pt3D1);
	BOOL IsValid();
	int IsOnLine(CPoint3D,							// A 3-D Point
				 BOOL* pbOnLine);					// Status of Point-On This Line : 1: Point On This Line; 0: Point Not On This Line
	int GetDistance(CPoint3D,						// A 3-D Point
					double& dDis);					// Distance between The Point and This Line
private:
	BOOL m_bValid;									// Status of This Line
};

class CSize3D {
public:
// Member Variables
	double cx,cy,cz;
// Methods
	CSize3D();	
	CSize3D(CSize); 
	CSize3D(int x0, int y0, int z0);
	CSize3D(double x0, double y0, double z0);
	CSize3D(const CSize3D &);

	CSize3D & operator =(CSize3D cs3d);
};

class  CPlane {
public:
	CVector3D Normal;									//A 3D vector normal to the plane
	CPoint3D Position;									// A point which lies on the plane
public:
	CPlane();
	CPlane(CPoint3D p,CVector3D n);
	CPlane(CLine3D l0,CLine3D l1);
	CPlane(CPoint3D p,CLine3D l);
	CPlane(CPoint3D p0,CPoint3D p1,CPoint3D p2);
	BOOL IsValid();
	int GetDistance(CPoint3D p1,					// A 3-D Point
					double& dDis);					// Distance between the Point and This Plane

	int GetAngleRad(CVector3D v,						// A 3-D Vector
					double &dAngleRad);					// Angle between the Vector and This Plane
	int GetAngleRad(CLine3D l,							// A 3-D Line
					double &dAngleRad);					// Angle between the Line and This Plane
	int GetAngleRad(CPlane p,							// A Plane
					double &dAngleRad);					// Angle between the Plane and This Plane

	int GetIntersection(CLine3D,						// A 3-D Line
						CPoint3D *);					// A Pointer to A 3-D Point
	int GetProjection(CPoint3D pt3DSrc,					// A Source Point
					  CPoint3D& pt3DProj);				// The Projection Point
};

class CRectangle2D {
	CPoint2D m_pointCenter;								// Center of The Rectangle
	double m_Width;										// Width of The Rectangle
	double m_Height;									// Height of The Rectangle
	double m_Angle;										// Angle of The Rectangle
public:
	CRectangle2D();	
	CRectangle2D(CRectangle2D &); 
	CRectangle2D(CRect &);
	CRectangle2D(CPoint2D* ppt2D0,
				 CPoint2D* ppt2D1,
				 CPoint2D* ppt2D2,
				 CPoint2D* ppt2D3);
	CRectangle2D(CPoint2D* ppt2D0,
			   CPoint2D* ppt2D1,
			   CPoint2D* ppt2D2,
			   CPoint2D* ppt2D3,
			   CRect* prcROI,
			   BOOL* pbPass);
	CRectangle2D(CRectangle2D* prc2D,
			   CRect* prcROI,
			   BOOL* pbPass);
	CRectangle2D & operator =(CRectangle2D& rc2D);
	double GetWidth();
	double GetHeight();
	void SetSize(CSize cs);
	void SetSize(CSize2D cs2D);
	CPoint2D GetDevTopLeft();
	CPoint2D GetDevTopRight();
	CPoint2D GetDevBottomLeft();
	CPoint2D GetDevBottomRight();

	CPoint2D GetDevLftMost();							// Get the Device Left Most Point
	CPoint2D GetDevTopMost();							// Get the Device Top Most Point
	CPoint2D GetDevRhtMost();							// Get the Device Right Most Point
	CPoint2D GetDevBotMost();							// Get the Device Bottom Most Point

	CPoint2D GetGeoTopMost();
	CPoint2D GetGeoBottomMost();
	CPoint2D GetGeoLeftMost();
	CPoint2D GetGeoRightMost();

	int DeflateRect2D(double dl, double dt, double dr, double db);
	int InflateRect2D(double dl, double dt, double dr, double db);
	double GetAngle();
	CPoint2D GetCenterPoint();
	void Shift(CPoint2D);								// Offset 
	void ShiftTo(CPoint2D pt2D);						// Center of The Rectangle
	void RotateAbout(CPoint2D p,						// Center of The Rectangle
					 double Angle);						// Angle of Rotation
	void RotateBy(double Angle);						// Angle of Rotation
	void RotateTo(double Angle);						// Angle of The Rectangle
	int Rotate(CPoint2D* ppt2DRotCtr, 
			 double dAngle);
	CRect GetBoundingRect();
	CRect GetBoundingRectWithoutInflate();

private:
	int GetRect2DByFixOnePt(CPoint2D* ppt2DOrg,
					   CPoint2D* ppt2DOthers,
					   CRect* prcROI,
					   CRectangle2D* prc2DMaxROI);

	int GetExpandRect2D(CPoint2D* ppt2DOrg, 
					 CPoint2D* ppt2DSide, 
					 CPoint2D* ppt2DCornerOrg, 
					 CRect* prcROI,
					 CRectangle2D* prc2D);
	int GetLinePtsInROI(CPoint2D* ppt2DOrg,
				   CPoint2D* ppt2D,
				   CPoint2D* ppt2DInROI,
				   CRect* prcROI);

};

class CRECTANGLE2D_FIND_PARM {
public:
	CRECTANGLE2D_FIND_PARM();
	~CRECTANGLE2D_FIND_PARM();
	CSize2D cs2DRect;
	CSize2D csRectVarTol;
	CSize2D csEdgePtVarTol;
	int nTopSampleStep;
	int nTopPercentage;
	int nBotSampleStep;
	int nBotPercentage;
	int nLftSampleStep;
	int nLftPercentage;
	int nRhtSampleStep;
	int nRhtPercentage;
	double dNominalAngle;
	double dMaxTiltAngle;
	double dAngleStep;
};

class CRECTANGLE2D_FOUND_INFO {
public:
	CRECTANGLE2D_FOUND_INFO();
	~CRECTANGLE2D_FOUND_INFO();
	CRectangle2D rc2DRect;
	int nTopPercentage;
	int nBotPercentage;
	int nLftPercentage;
	int nRhtPercentage;
};

class CRECTANGLE2D_FOUND_RSLT {
public:
	CRECTANGLE2D_FOUND_RSLT();
	~CRECTANGLE2D_FOUND_RSLT();
	CRECTANGLE2D_FOUND_INFO* pRect2DFndInfo;
	int nRect2DFndNo;
};


class CPackageEdgeSelectedPoints {
public:
	CPackageEdgeSelectedPoints();
	~CPackageEdgeSelectedPoints();
	void Clean();
	CPackageEdgeSelectedPoints & operator =(CPackageEdgeSelectedPoints PackageEdgeSelectedPoints);
	bool* pbLftIndex;
	bool* pbRhtIndex;
	bool* pbTopIndex;
	bool* pbBotIndex;
	int nLft;
	int nRht;
	int nTop;
	int nBot;
	int nLftScore;
	int nTopScore;
	int nRhtScore;
	int nBotScore;
	int nSumScore;
	double dAngle;
};

class CRectangle2DFind {
public:
	CRectangle2DFind();
	~CRectangle2DFind();
	CRectangle2D rc2DFound;

	CPackageEdgeSelectedPoints *pPackageEdgeSelectedPoints;

	int GetRectangle2D(CPoint2D* ppt2DInspTop,
					   int nInspTopNo,
					   CPoint2D* ppt2DInspBot,
					   int nInspBotNo,
					   CPoint2D* ppt2DInspLft,
					   int nInspLftNo,
					   CPoint2D* ppt2DInspRht,
					   int nInspRhtNo,
					   CRECTANGLE2D_FIND_PARM* pRect2DFindParm);

	int DistanceMatching(long* plFirstEdgePos, 
						 int nFirstEdgePosNo,
						 long* plLastEdgePos,
						 int nLastEdgePosNo,
						 int nDistVar,
						 int nDist,
						 int nEdgePtVar,
						 int nFirstEdgeHistThreshold,
						 int nLastEdgeHistThreshold,
						 int* pnFirstEdgeStartPos,
						 int* pnFirstEdgeEndPos,
						 int* pnLastEdgeStartPos,
						 int* pnLastEdgeEndPos,
						 int* pnEdgePair);

	CRECTANGLE2D_FOUND_RSLT Rect2DFndRslt;
private:
	typedef struct {
		int nSide0Percentage;
		int nSide0HistIndex;
		int nSide1Percentage;
		int nSide1HistIndex;
		int nCrossPercentage;
	} RECT_PROJ;
	int GetRectProj();
	
	int nTopPercentage;
	int nBotPercentage;
	int nLftPercentage;
	int nRhtPercentage;
	CSize csRect;
	CSize csRectVarTol;
	CSize csEdgePtVarTol;
	double dPkgAngle;
	CPoint ptPkgCtr;
	int nTimesX;
	int nTimesY;

	int GetVerticalEdgeHist(long* plSrcX, 
						  long* plSrcY, 
						  int nInspNo,
						  int nEdgeVar,
						  CRect* prcEdge);

	int GetHorizontalEdgeHist(long* plSrcX, 
						  long* plSrcY, 
						  int nInspNo,
						  int nEdgeVar,
						  CRect* prcEdge);

	void GetVerticalEdgeIndex(long* plSrcX, 
							   long* plSrcY, 
							   int nInspNo,
							   CRect* prcEdge,
							   int nEdgeVar,
							   bool* pbSelected);


	void GetHorizontalEdgeIndex(long* plSrcX, 
							   long* plSrcY, 
							   int nInspNo,
							   CRect* prcEdge,
							   int nEdgeVar,
							   bool* pbSelected);


	int FindOverlap(int nLftEdgeStartPos, 
					int nLftEdgeEndPos,
					int nRhtEdgeStartPos,
					int nRhtEdgeEndPos,
					int nWidth,
					int nWidthVar,
					int* pnLftEdgeStartPosDst,
					int* pnLftEdgeEndPosDst,
					int* pnRhtEdgeStartPosDst,
					int* pnRhtEdgeEndPosDst);

	int RangeMerge(int* pnStartPos,
				   int* pnEndPos,
				   int* pnNo);
};


class CRectInfo
{
public:
	CRectInfo();
	~CRectInfo();
	void Clean();
	int	SetLabel(CRect* prcSrc, CSize* pcsFOV, int nLabel);
	CRect rc;
	int m_nLabel;
	int* pnLabelX;
	int nLabelXSize;
	int* pnConnectedLabelHistX;
	int nConnectedLabelHistX;
	int* pnLabelY;
	int nLabelYSize;
	int* pnConnectedLabelHistY;
	int nConnectedLabelHistY;
	int* pnConnectedLabelHist;
	int nConnectedLabelHist;
	bool bMerged;
};


class CMergedRect
{
public:
	CMergedRect();
	~CMergedRect();
	void Clean();
	CRect rcMerged;
	int* pnMergedHist;
	int nMergedLabel;
	int* pnMergedLabel;
};


class CRectMerge
{
public:
	CRectMerge();
	~CRectMerge();
	void Clean();
	CMergedRect* pMergedRect;
	CRect* prcFoundMerged;
	int nFoundMerged;
	int nLabelStart;
	int Merge(CRect* prcRect, int nRectNo, CSize* pcsFOV);
private:
	long* plAccX;
	long* plAccY;
	CRect* prcMerge;
	CRectInfo* pRectInfo;
	int npMergedRect;
	int nRectInfoNo;
	int nMergedRect;
	int nRectMergeNo;
	void FndConnectLocalLabelX(int nRectIndex);
	void FndConnectLocalLabelY(int nRectIndex);
	void FndCommonLocalLabel(int nRectIndex);
};


CPoint2D RotatePoint2D(CPoint2D ptOrg,					// Point to Be Rotated
					   CPoint2D ptRef,					// Point of Rotation Center
					   double dAngle);					// Angle of Rotation

CPoint2D RotatePoint2DOfRigidObject(CPoint2D ptOrg_PC,	// center point of package window in teaching process
						 CPoint2D ptOrg_OC, //the CRect of template window in teaching process
						 CPoint2D ptRef_PC,// center point of package window in inspection process
						 double dAngle) ;	// the angle rotated around the the center of package in inspection process

CRect RotateRecWindow2D(CPoint2D ptOrg_PC, // center point of package window in teaching process
						 CRect rcOrg_OC, //the CRect of template window in teaching process
						 CPoint2D ptRef_PC, // center point of package window in inspection process
						 double dAngle); 	// the angle rotated around the the center of package in inspection process

//BOOL IsCollinear(double ptsX[], double ptsY[], double dDelta);

int IsCollinear(CPoint2D* ppt2d,						// Pointer to Points
				int nSize,								// Number of Points
				double dDelta,							// Tolerance of Collinear
				BOOL* pbCollinear);						// Result of Collinear
int GetCircleCenter(CPoint2D* ppt2DSrc1,
					CPoint2D* ppt2DSrc2,
					CPoint2D* ppt2DSrc3,
					CPoint2D* ppt2DCenter);

void Sorting(int *nData,
		     int nDataNo,
			 int *index, 
			 BOOL bAscend); 

void Sorting(int *pnData,
		     int nDataNo,
			 BOOL bAscend);

void Sorting(long *nData,
		     int nDataNo,
			 int *index, 
			 BOOL bAscend); 

void Sorting(long *pnData,
		     int nDataNo,
			 BOOL bAscend);

void Sorting(double *nData,
		     int nDataNo,
			 BOOL bAscend);

void Sorting(double *nData,
		     int nDataNo,
			 int *index, 
			 BOOL bAscend);

int Round(double d);

int GetCombination(int nBase, 
				   int *pnOrder, 
				   int& nValid);

double InterpolX(CPoint2D ptBefore,CPoint2D ptAfter, double dThres);

int InterpolCPoint(CPoint* pptSrc1, 
				   BYTE byteVal1,
				   CPoint* pptSrc2, 
				   BYTE byteVal2, 
				   BYTE byteThreshold,
				   CPoint2D* ppt2DInterpol,
				   bool bThresholdCheck);

int GetRoughBallCenter(CPoint2D* ppt2DBallEdge, 
					   int nBallEdge, 
					   CPoint2D* ppt2DEstimatedCenter,
					   double dBallRadiusTol,
					   double dCenterStep,
					   CSize2D* pcs2DShiftTol,
					   CPoint2D* ppt2DBallCenter,
					   double* pdRadiusInsp,
					   int* pnPercentage);

int GetPointsBoundingRect(CPoint2D* ppt2D, 
						  int nptNo,
						  CRect* prcBounding);

int GetPointsBoundingRect(CPoint* ppt, 
						  int nptNo,
						  CRect* prcBounding);

int GetPointsBoundingRect(CArray<CPoint2D, CPoint2D>* pArypt2D, 
						  CRect* prcBounding);

int GetPointsBoundingRect(CArray<CPoint, CPoint>* pArypt, 
						  CRect* prcBounding);

int GetRoughBallCenter(CPoint* pptBallEdge, 
					   int nBallEdge, 
					   CPoint2D* ppt2DEstimatedCenter,
					   double dBallRadiusTol,
					   double dCenterStep,
					   CSize2D* pcs2DShiftTol,
					   CPoint2D* ppt2DBallCenter,
					   double* pdRadiusInsp,
					   int* pnPercentage);

class CInterpolationBy2Pts
{
public:
	CInterpolationBy2Pts();
	~CInterpolationBy2Pts();
	void Clean();
	int SetPoint2DAry(CPoint2D* pPt2D,
					  int nPt2DNo);
	int SetPointAry(CPoint* pPt,
					  int nPtNo);
	int GetY(double dx, 
			 double* pdInterpolationy);

	int GetYByMinDisPts(double dx, 
			 double* pdInterpolationy);

public:
	int nNo;
	CPoint2D* ppt2DSorting;
private:
	int* pnMappingIndex;
	int nMappingNo;
	double* pdx;
	double* pdy;
	int GetIndex(double dx,
			 int* pnIndex);
};

class CInterpolationBy3Pts
{
public:
	CInterpolationBy3Pts();
	~CInterpolationBy3Pts();
	void Clean();
	int SetPoint2DAry(CPoint2D* pPt2D,
					  int nPt2DNo);

	int GetY(double dx, 
			 double* pdInterpolationy);

	int ModifyPoint(double* pdDifY,
				 BOOL* pbMatch,
				 int nInterpolationNo,
				 int nStartX,
				 int nEndX,
				 int nSegmentMatchingPercentageTol,
				 double dDifYTol);
public:
	int nNo;
	CPoint2D* ppt2DSorting;
private:
	int CalculateGradVal();

	int* pnMappingIndex;
	int nMappingNo;
	double* pdx;
	double* pdy;
	double* pdGrad;
	double* pParmA;
	double* pParmB;
	int GetIndex(double dx,
			 int* pnIndex);
};

#pragma once

#include "Imaging.h"

class COvalContour
{
public:
	COvalContour();
	~COvalContour();
	void Clean();
	int CreateOval(CPoint2D* ppt2DCtr,
				   double dRadiusH,
				   double dRadiusV);
	int CreateOval(CPoint2D* ppt2DCtr,
				 double dRadiusH,
				 double dRadiusV,
				 int nSamplingNo);
	CPoint* ppt;
	int nptNo;
};

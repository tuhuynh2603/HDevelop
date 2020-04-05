#include "StdAfx.h"
#include "OvalContour.h"


COvalContour::COvalContour()
{
	ppt = NULL;
	nptNo = 0;
}


COvalContour::~COvalContour()
{
	Clean();
}

void COvalContour::Clean()
{
	if(ppt != NULL)
		delete [] ppt;
	ppt = NULL;
	nptNo = 0;
}


int COvalContour::CreateOval(CPoint2D* ppt2DCtr,
							 double dRadiusH,
							 double dRadiusV)
{
	int i;
	double dAngle;
	double dMaxRadius;
try {
	Clean();
	if(dRadiusH > dRadiusV)
		dMaxRadius = dRadiusH;
	else
		dMaxRadius = dRadiusV;

	nptNo = Round(2 * PI * dMaxRadius) + 1;
	ppt = new CPoint[abs(nptNo)];
	for(i=0;i<nptNo;i++) {
		dAngle = i/dMaxRadius;
		ppt[i].x = Round(ppt2DCtr->x + dRadiusH * cos(dAngle));
		ppt[i].y = Round(ppt2DCtr->y + dRadiusV * sin(dAngle));
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"COvalContour::CreateOval");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}

int COvalContour::CreateOval(CPoint2D* ppt2DCtr,
							 double dRadiusH,
							 double dRadiusV,
							 int nSamplingNo)
{
	int i;
	double dAngle;
	double dMaxRadius;
	double dStep;
	double dPerimeter;
try {
	Clean();
	if(nSamplingNo <= 0)
		goto ErrorAbort;
	if(dRadiusH > dRadiusV)
		dMaxRadius = dRadiusH;
	else
		dMaxRadius = dRadiusV;
	dPerimeter = 2 * PI * dMaxRadius + 1;
	nptNo = nSamplingNo;
	dStep = dPerimeter/nSamplingNo;
	ppt = new CPoint[nSamplingNo];
	for(i=0;i<nSamplingNo;i++) {
		dAngle = (i * dStep)/dMaxRadius;
		ppt[i].x = Round(ppt2DCtr->x + dRadiusH * cos(dAngle));
		ppt[i].y = Round(ppt2DCtr->y + dRadiusV * sin(dAngle));
	}
}
catch(...) {
	OutputDebugLogTo(9,TRUE,"COvalContour::CreateOval");
	goto ErrorAbort;
}
	return 0;
ErrorAbort:
	return -1;
}


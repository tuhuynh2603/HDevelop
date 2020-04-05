#include "stdafx.h"
#include "TrigTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

double tcos(double dAngle)
{
	int	nAngle;

	if (dAngle < 0.0)
		dAngle *= -1.0;

	nAngle = (int) (dAngle / dKAngleStep + 0.00001);

	return ccos[nAngle];
}

double tsin(double dAngle)
{
	int	   nAngle;
	double dMult = 1.0;;

	if (dAngle < 0.0) {
		dAngle *= -1.0;
		dMult = -1.0;
	}

	nAngle = (int) (dAngle / dKAngleStep + 0.00001);

	return csin[nAngle]*dMult;
}

int GetAngleIndex(double& dAngle)
{
	ASSERT(dAngle >= 0.0);
	ASSERT(dAngle <= 180.0);
	
	return (int) ((dAngle - dKAngleStart)/dKAngleStep + 0.00001);
}

int GetSignedAngleIndex(double& dAngle)
{
	ASSERT(dAngle >= -90.0);
	ASSERT(dAngle <= 90.0);
	
	return (int) ((dAngle - dKSignedAngleStart)/dKAngleStep + 0.00001);
}

int GetAngleStep(double& dAngleStep)
{
	return (int) ((dAngleStep/dKAngleStep) + 0.000001);
}

double GetAngle(int nIndex) 
{
	return (dKAngleStart + ((double) nIndex)*dKAngleStep);
}

double GetSignedAngle(int nIndex)
{
	return (dKSignedAngleStart + ((double) nIndex)*dKAngleStep);
}

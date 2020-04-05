#include "stdAfx.h"
#include "PkgLocStruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



PKG_ADV_LOC_PARM::PKG_ADV_LOC_PARM()
{
	nXShiftTol = 40;
	nYShiftTol = 40;
}

PKG_ADV_LOC_PARM::~PKG_ADV_LOC_PARM()
{
}


PKG_LOC_EDGE_PARM::PKG_LOC_EDGE_PARM()
{
}

PKG_LOC_EDGE_PARM::~PKG_LOC_EDGE_PARM()
{

}

PKG_LOC_RESULT::PKG_LOC_RESULT()
{	
	ptPkgFoundCenter = CPoint2D(0,0);
	dPkgFoundAngle = 0;
	dFndXOffset = 0;
	dFndYOffset = 0;
};
PKG_LOC_RESULT::PKG_LOC_RESULT(CPoint2D* ppt2DTopLft,
				   CPoint2D* ppt2DTopRht,
				   CPoint2D* ppt2DBotRht,
				   CPoint2D* ppt2DBotLft)
{
	rc2DPkg = CRectangle2D(ppt2DTopLft, ppt2DTopRht, ppt2DBotRht, ppt2DBotLft);
	ptPkgFoundCenter = rc2DPkg.GetCenterPoint();
	dPkgFoundAngle = rc2DPkg.GetAngle();
	ptTopLft = rc2DPkg.GetDevTopLeft();
	ptTopRht = rc2DPkg.GetDevTopRight();
	ptBotLft = rc2DPkg.GetDevBottomLeft();
	ptBotRht = rc2DPkg.GetDevBottomRight();
	ptTopCntr = (ptTopLft + ptTopRht)/2;
	ptBotCntr = (ptBotLft + ptBotRht)/2;
	ptLftCntr = (ptTopLft + ptBotLft)/2;
	ptRhtCntr = (ptTopRht + ptBotRht)/2;
	LineTop = CLine2D(ptTopLft, ptTopRht);
	LineBottom = CLine2D(ptBotLft, ptBotRht);
	LineLeft = CLine2D(ptTopLft, ptBotLft);
	LineRight = CLine2D(ptTopRht, ptBotRht);
	lineVert.GetBiSectLine(&LineLeft, &LineRight);
	lineHorz.GetBiSectLine(&LineTop, &LineBottom);
}


void PKG_LOC_RESULT::Rotate(double dAngle, CPoint2D* ppt2DRotCtr)
{
	ptPkgFoundCenter = *ppt2DRotCtr;
	dPkgFoundAngle += dAngle;
	rc2DPkg.RotateBy(dAngle);
	rc2DPkg.ShiftTo(*ppt2DRotCtr);
	ptTopLft = rc2DPkg.GetDevTopLeft();
	ptTopRht = rc2DPkg.GetDevTopRight();
	ptBotLft = rc2DPkg.GetDevBottomLeft();
	ptBotRht = rc2DPkg.GetDevBottomRight();
	ptTopCntr = (ptTopLft + ptTopRht)/2;
	ptBotCntr = (ptBotLft + ptBotRht)/2;
	ptLftCntr = (ptTopLft + ptBotLft)/2;
	ptRhtCntr = (ptTopRht + ptBotRht)/2;
	LineTop = CLine2D(ptTopLft, ptTopRht);
	LineBottom = CLine2D(ptBotLft, ptBotRht);
	LineLeft = CLine2D(ptTopLft, ptBotLft);
	LineRight = CLine2D(ptTopRht, ptBotRht);
	lineVert.GetBiSectLine(&LineLeft, &LineRight);
	lineHorz.GetBiSectLine(&LineTop, &LineBottom);
}
int PKG_LOC_RESULT::GetRotROIRefPkg(CRectangle2D* prc2DPkgTeach,
								    CRect* prcTeachROI,
								    CRectangle2D* prc2DInspROI)
{
	CPoint2D pt2DTeachCtr;
	CPoint2D pt2DShift;
	double dRotAngle;
	pt2DShift = rc2DPkg.GetCenterPoint() - prc2DPkgTeach->GetCenterPoint();
	dRotAngle = rc2DPkg.GetAngle() - prc2DPkgTeach->GetAngle();

	pt2DTeachCtr = prc2DPkgTeach->GetCenterPoint();
	*prc2DInspROI = CRectangle2D(*prcTeachROI);

	prc2DInspROI->Rotate(&pt2DTeachCtr, 
						 dRotAngle);
	prc2DInspROI->Shift(pt2DShift);
	return 0;
}


PKG_LOC_RESULT::~PKG_LOC_RESULT()
{	
}

CMarkInspParm::CMarkInspParm()
{
	bEnableMarkInsp				= FALSE;		
	nAcceptScore				= 80;
	nRejectScore				= 40;
	nSymbolContrast				= 20;
	nMarkRotation				= 5;
	dOtherExcessArea			= 5;
	dOtherMissingArea			= 5;
	nMarkMatchPixInMisMatchSqr	= 3;
	nFirstTempShiftXTol			= 40;
	nFirstTempShiftYTol			= 40;
	nOtherTempShiftXTol			= 20;
	nOtherTempShiftYTol			= 20;
	nMinCharSize				= 80;
	nNoOfTemplates				= 4;
	dAngleStep					= 0.5;
	nPkgLocDilationX			= 0;
	nPkgLocDilationY			= 4;
	nSymbolHeight				= 100;
	nSymbolPitch				= 100;
	dMaxTiltAngle				= 10;
	nShitXTolerance				= 50;
	nShitYTolerance				= 50;
}

CMarkInspParm::~CMarkInspParm()
{

}
#include "stdafx.h"
#include "InspParms.h"

CPatternTeachParm::CPatternTeachParm()
{
	m_nPatternLayers = 3;
	for(int i=0; i<3; i++)
	{
		m_dLightOpeningRadius[i] = 7.5;
		m_dDarkOpeningRadius[i] = 7.5;
	}
}

CPatternTeachParm::~CPatternTeachParm()
{
}

CPatternInspParm::CPatternInspParm()
{
	for(int i=0; i<3; i++)
	{
		m_bEnableDarkPatternInsp[i] = TRUE;
		m_nDarkCircleTolerance[i] = 10;
		m_nDarkWidthHeightTolerance[i] = 10;
		m_bEnableLightPatternInsp[i] = TRUE;
		m_nLightCircleTolerance[i] = 10;
		m_nLightWidthHeightTolerance[i] = 10;
	}
	
	m_bCheckDarkPrintMissing = FALSE;
	m_dOuterMostLayerErosion = 10;
	m_dCircularityTolerance = 0.5;
	m_dLightAreaHolesRatio = 0.04;
}

CPatternInspParm::~CPatternInspParm()
{
}

CLocationParm::CLocationParm()
{

	m_nLensRadius = 700;
	m_nHysteresisLow = 15;
	m_nHysteresisHigh = 20;
	m_nHysteresisMaxLength = 10;
	m_dMinCircularity = 0.93;
	m_nEdgeOpeningWidth = 50;
	m_nEdgeClosingWidth = 200;
	m_nPTEdgeRegionDilation = 50;
}

CLocationParm::~CLocationParm()
{
}

CLensCrackParm::CLensCrackParm()
{
	m_bEnable = TRUE;
	m_nMinCrackArea = 150;
	m_nBumpHeight = 3;
}

CLensCrackParm::~CLensCrackParm()
{
}

CLensCharacterParm::CLensCharacterParm()
{
	m_bPatternLens = 1;
	m_bSolidPattern = 1;

	m_nPatternPolarCloseWidth = 100;
	m_nPatternPolarCloseHeight = 50;
	m_nPatternPolarOpenWidth = 100;
	m_nTopInnerMaskDilation = 15;
	m_nTopOuterMaskErosion = 15;
}

CLensCharacterParm::~CLensCharacterParm()
{
}


CLensEdgeInspParm::CLensEdgeInspParm()
{
	m_bEnable = TRUE;

	m_nEdgeInspErosion = 8;
	m_nMaxEdgeGrayValue = 168;
	m_nConnectingWidth = 200;
}

CLensEdgeInspParm::~CLensEdgeInspParm()
{
}


CLensPatternShiftParm::CLensPatternShiftParm()
{
	m_bEnable = TRUE;
	m_bMethod1Enable = TRUE;
	m_nMinLensEdgeToPatternDist = 28;
	m_bMethod2Enable = TRUE;
	m_nMaxOuterAndEdgeCenterDist = 20;
	m_bMethod3Enable = TRUE;
	m_nMaxInnerAndEdgeCenterDist = 20;
	m_bMethod4Enable = TRUE;
	m_nMaxInnerAndOuterCenterDist = 20;
}

CLensPatternShiftParm::~CLensPatternShiftParm()
{
}

CLensFlashParm::CLensFlashParm()
{
	m_bEnable = TRUE;
	m_nFlashThers = 130;
	m_nMinFlashContLength = 7;
	m_nMaxFlashContLength = 200;
	m_nLensEdgeOpeningRadius = 100;
	m_dEdgeOffsetDilation = 2.5;
	m_dInspDilationFromOffset = 10;
}

CLensFlashParm::~CLensFlashParm()
{
}

CDarkSpotInspParm::CDarkSpotInspParm()
{
	m_bEnableDarkSpotOpticalInsp = TRUE;	
	m_nMaxDarkSpotGrayValue = 140;
	m_nMinDarkSpotSize = 80;
	m_nDarkSpotInnerCircleRegionErosion = 20;

	m_bEnableDarkSpotGapInsp = TRUE;
	m_nGapMaxDarkSpotGrayValue = 140;
	m_nGapMinDarkSpotSize = 80;
}

CDarkSpotInspParm::~CDarkSpotInspParm()
{
}

CHoleInspParm::CHoleInspParm()
{
	m_bEnableHoleOpticalInsp = TRUE;
	m_nMinOpticalGrayValue = 190;
	m_nMinHoleBrightArea = 20;
	m_nHoleInnerCircleRegionErosion = 20;

	m_bEnableHoleOnPatternInsp = TRUE;
	m_nInnerPatternMinGrayValue = 240;
	m_nOuterPatternMinGrayValue = 230;
	m_nPatternMinHoleBrightArea = 20;

	m_bEnableHoleGapInsp = TRUE;
	m_nGapMinHollowGrayValue = 220;
	m_nGapMinHollowArea = 20;
}

CHoleInspParm::~CHoleInspParm()
{
}

CFiberInspParm::CFiberInspParm()
{
	m_bOpticalZoneEnable = TRUE;
	m_nFiberContrast = 30;
	m_nFiberWidth = 3;
	m_nFiberLength = 25;
	m_nFiberInnerCircleRegionErosion = 20;
	m_nFiberGapRegionErosion = 3;
	m_bThicknessCheck = TRUE;
	m_dMinRectThickness = 2;

	m_bPatternZoneEnable = TRUE;
	m_nPatternFiberWidth = 3;
	m_nPatternThreshold = 185;
	m_nPatternFiberLength = 11;
	m_dPatternFiberAnisometry = 3;
	m_nLowThres = 90;
	m_nHighThres = 100;
	m_dPatternFiberWidthTolerance = 0.5;

	m_nGapZoneEnable = TRUE;
	m_nGapFiberContrast = 30;
	m_nGapFiberWidth = 3;
	m_nGapFiberLength = 25;

	m_bAngleDifferentCheckEnable = TRUE;
	m_nMaxAngleDifferent = 5;
	m_dMaxFiberHeight = 10.0;
}

CFiberInspParm::~CFiberInspParm()
{
}

CTopUInspParm::CTopUInspParm()
{
	m_bEnable = TRUE;

	m_nLowThres = 30;
	m_nHighThres = 40;
	m_dMinScracthAnisometry = 2.0;
	m_nMinScratchLength = 20;
	m_nLowBrightParticleGrayValue = 200;
	m_nHighBrightParticleGrayValue = 225;
	m_dMinThickness = 2;
	m_nMinDarkSpotSize = 80;
	m_nMaxDarkSpotGrayValue = 128;
}

CTopUInspParm::~CTopUInspParm()
{
}

CTopLInspParm::CTopLInspParm()
{
	m_bEnable = TRUE;

	m_nLowThres = 30;
	m_nHighThres = 40;
	m_dMinScracthAnisometry = 2.0;
	m_nMinScratchLength = 20;
	m_nLowBrightParticleGrayValue = 200;
	m_nHighBrightParticleGrayValue = 225;
	m_nMinDarkSpotSize = 50;
	m_dMinThickness = 2;
	m_nMaxDarkSpotGrayValue = 80;
	m_nLightRingClosingRadius = 50;
}

CTopLInspParm::~CTopLInspParm()
{
}
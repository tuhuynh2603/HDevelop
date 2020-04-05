#pragma once

class CPatternTeachParm
{
public:
	CPatternTeachParm();
	~CPatternTeachParm();

	int m_nPatternLayers;
	double m_dLightOpeningRadius[3];
	double m_dDarkOpeningRadius[3];
};

class CPatternInspParm
{
public:
	CPatternInspParm();
	~CPatternInspParm();

	BOOL m_bEnableDarkPatternInsp[3];
	int m_nDarkCircleTolerance[3];
	int m_nDarkWidthHeightTolerance[3];

	BOOL m_bCheckDarkPrintMissing;
	BOOL m_bEnableLightPatternInsp[3];
	int m_nLightCircleTolerance[3];
	int m_nLightWidthHeightTolerance[3];

	double m_dOuterMostLayerErosion;
	double m_dLightAreaHolesRatio;
	double m_dCircularityTolerance;
};

class CLocationParm
{
public:
	CLocationParm();
	~CLocationParm();

	int m_nLensRadius;
	int m_nHysteresisLow;
	int m_nHysteresisHigh;
	int m_nHysteresisMaxLength;
	double m_dMinCircularity;
	int m_nEdgeOpeningWidth;
	int m_nEdgeClosingWidth;

	int m_nPTEdgeRegionDilation;
};

class CLensCrackParm
{
public:
	CLensCrackParm();
	~CLensCrackParm();

	BOOL m_bEnable;
	int m_nMinCrackArea;
	int m_nBumpHeight;
};

class CLensCharacterParm
{
public:
	CLensCharacterParm();
	~CLensCharacterParm();

	BOOL m_bPatternLens;
	BOOL m_bSolidPattern;

	int m_nPatternPolarCloseWidth;
	int m_nPatternPolarCloseHeight;
	int m_nPatternPolarOpenWidth;
	int m_nTopInnerMaskDilation;
	int m_nTopOuterMaskErosion;
};

class CLensEdgeInspParm
{
public:
	CLensEdgeInspParm();
	~CLensEdgeInspParm();

	BOOL m_bEnable;

	int m_nEdgeInspErosion;
	int m_nMaxEdgeGrayValue;
	int m_nConnectingWidth;
};

class CLensPatternShiftParm
{
public:
	CLensPatternShiftParm();
	~CLensPatternShiftParm();

	BOOL m_bEnable;
	
	BOOL m_bMethod1Enable;
	int m_nMinLensEdgeToPatternDist;
	BOOL m_bMethod2Enable;
	int m_nMaxOuterAndEdgeCenterDist;
	BOOL m_bMethod3Enable;
	int m_nMaxInnerAndEdgeCenterDist;
	BOOL m_bMethod4Enable;
	int m_nMaxInnerAndOuterCenterDist;
};

class CLensFlashParm
{
public:
	CLensFlashParm();
	~CLensFlashParm();

	BOOL m_bEnable;
	int m_nFlashThers;
	int m_nMinFlashContLength;
	int m_nMaxFlashContLength;
	int m_nLensEdgeOpeningRadius;
	double m_dEdgeOffsetDilation;
	double m_dInspDilationFromOffset;
};

class CDarkSpotInspParm
{
public:
	CDarkSpotInspParm();
	~CDarkSpotInspParm();

	BOOL m_bEnableDarkSpotOpticalInsp;
	int m_nMaxDarkSpotGrayValue;
	int m_nMinDarkSpotSize;
	
	BOOL m_bEnableDarkSpotGapInsp;	
	int m_nGapMaxDarkSpotGrayValue;
	int m_nGapMinDarkSpotSize;

	int m_nDarkSpotInnerCircleRegionErosion;
};

class CHoleInspParm
{
public:
	CHoleInspParm();
	~CHoleInspParm();

	BOOL m_bEnableHoleOpticalInsp;
	int m_nMinOpticalGrayValue;
	int m_nMinHoleBrightArea;
	int m_nHoleInnerCircleRegionErosion;

	BOOL m_bEnableHoleOnPatternInsp;
	int m_nInnerPatternMinGrayValue;
	int m_nOuterPatternMinGrayValue;
	int m_nPatternMinHoleBrightArea;

	BOOL m_bEnableHoleGapInsp;	
	int m_nGapMinHollowGrayValue;
	int m_nGapMinHollowArea;
};

class CFiberInspParm
{
public:
	CFiberInspParm();
	~CFiberInspParm();

	BOOL m_bOpticalZoneEnable;
	int m_nFiberContrast;
	int m_nFiberWidth;
	int m_nFiberLength;
	int m_nFiberGapRegionErosion;
	int m_nFiberInnerCircleRegionErosion;
	BOOL m_bThicknessCheck;
	double m_dMinRectThickness;

	BOOL m_bPatternZoneEnable;
	int m_nPatternFiberWidth;
	int m_nPatternThreshold;
	int m_nPatternFiberLength;
	double m_dPatternFiberAnisometry;
	int m_nLowThres;
	int m_nHighThres;
	double m_dPatternFiberWidthTolerance;

	BOOL m_nGapZoneEnable;
	int m_nGapFiberContrast;
	int m_nGapFiberWidth;
	int m_nGapFiberLength;

	BOOL m_bAngleDifferentCheckEnable;
	int m_nMaxAngleDifferent;
	double m_dMaxFiberHeight;
};

class CTopUInspParm
{
public:
	CTopUInspParm();
	~CTopUInspParm();

	BOOL m_bEnable;
	
	//int m_nDualMinSize;
	int m_nLowThres;
	int m_nHighThres;
	double m_dMinScracthAnisometry;
	int m_nMinScratchLength;
	int m_nLowBrightParticleGrayValue;
	int m_nHighBrightParticleGrayValue;
	double m_dMinThickness;
	int m_nMinDarkSpotSize;
	int m_nMaxDarkSpotGrayValue;
	
};

class CTopLInspParm
{
public:
	CTopLInspParm();
	~CTopLInspParm();

	BOOL m_bEnable;
	
	//int m_nDualMinSize;
	int m_nLowThres;
	int m_nHighThres;
	double m_dMinScracthAnisometry;
	int m_nMinScratchLength;
	int m_nLowBrightParticleGrayValue;
	int m_nHighBrightParticleGrayValue;
	double m_dMinThickness;
	int m_nMinDarkSpotSize;
	int m_nMaxDarkSpotGrayValue;
	int m_nLightRingClosingRadius;
};
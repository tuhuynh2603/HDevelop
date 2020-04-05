/// LightControl: Light Control base class
/// Udupa; May 2012;
/// Udupa; Jan'2013; Individual bit Pulse Width

#pragma once

#include "IniFile.h"

#define NO_OF_SETS  3
#define MAX_LIGHT_VALUE  100

#define PP420     0
#define PP820     1
#define PP820S15  2

class CLightControl 
{
public:
	CLightControl();
	virtual ~CLightControl();

	int LoadLightSetting(int nController, CString strDirectoryName, CString strFileName, int nSet = 0, BOOL bLightDlgOpen = FALSE, BOOL *bTrackSegments = 0);
	int SaveLightSetting(int nController, CString strDirectoryName, CString strFileName, int nSet=0);

	void Configure(int nControllerNo,
					int nNoOfSegments, 
					float* pMaxCurrents, 
					int* pMaxPulseWidth,//int nMaxPulseWidth,
					int nSteps=1,
					int nControllerType=PP820);

	int SetLight(int nChannel, int nCurrent, BOOL bSend=TRUE);
	int SetAllLights(int nCurrent = -1);
	int RefreshLight(int nSet=-1, BOOL *bTrackSegments = 0);
	int SetPulseWidth(int nChannel, int nPulseWidth, BOOL bSend=TRUE);
	int SetAllPulseWidth();

	int GetLight(int nChannel);
	int GetPulseWidth(int nChannel);
	int ResetLightSteps();
	int ReverseSteps(BOOL bReverse);
	int m_nController;
	//void SendRTValues(int nChannel, int nPulseWidth);
	void UpdateLightStep(int nStep);
	int m_nLightStep;
protected:
	virtual int SendString(CString* pstrData) { return -1; }
	virtual int SendMaxLightValues(BOOL *bTrackSegments) { return 0; };
	virtual int SendMaxLightValue(int nChannel, int nPulseWidth) { return 0; };
	virtual int SendLightValue(int Channel, int nCurrent) { return 0; };
	virtual int SendLightValues(int nStep, int nSet, BOOL *bTrackSegments) { return 0; };
	virtual int SendStepCount(int nSteps);
	virtual int SaveInController() { return 0; };
	virtual int ResetSequenceStep() { return 0; };

	int GetLightVal(CIniFile *ptrIniFile, int nController, int nSet);
	int SetLightVal(CIniFile *ptrIniFile, int nController, int nSet);
	void InitLightVal();

	BOOL m_bInitialized;
	int Initialize();

	int m_nNoOfSegments;
	float* m_pMaxCurrents;
	//int m_nMaxPulseWidth;
	int* m_pMaxPulseWidth;

	BYTE** m_pLightVal;
	//short* m_pPulseVal;
	short** m_pPulseVal;


	int m_nControllerType;
	int m_nStepCount;
	int m_nCurrentStepCount;

	BOOL m_bStepsReversed;

	CRITICAL_SECTION m_CriticalSection;
};

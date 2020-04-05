/// LightControl: Light Control base class
/// Udupa; May 2012;
/// Udupa; Jan'2013; Individual bit Pulse Width

#include "stdafx.h"
#include "LightControl.h"
#include "DebugLog.h"
#include "PerformanceTimer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightControl::CLightControl()
{
	m_nController = 0;
	m_nNoOfSegments = 0;
	m_pMaxCurrents = NULL;
	//m_nMaxPulseWidth = 0;
	m_pMaxPulseWidth = NULL;

	m_nStepCount = 0;
	m_nCurrentStepCount = 0;

	m_pLightVal = NULL;
	m_pPulseVal = NULL;
	m_nLightStep = -2;

	m_nControllerType = PP820;
	m_bStepsReversed = FALSE;

	m_bInitialized = FALSE;
	
	InitializeCriticalSection(&m_CriticalSection);
}

CLightControl::~CLightControl()
{
	if(m_pLightVal != NULL) {
		for(int i=0; i<m_nStepCount/*NO_OF_SETS*/; i++)
			delete[] m_pLightVal[i];
		delete[] m_pLightVal;
	}
	/*if(m_pPulseVal != NULL)
		delete[] m_pPulseVal;*/
	if(m_pPulseVal != NULL)
		for(int j=0; j<m_nStepCount/*NO_OF_SETS*/; j++)
			delete[] m_pPulseVal[j];
		delete[] m_pPulseVal;

	DeleteCriticalSection(&m_CriticalSection);
}

void CLightControl::InitLightVal()
{
	if(m_pLightVal != NULL) {
		for(int i=0; i<m_nStepCount/*NO_OF_SETS*/; i++)
			delete[] m_pLightVal[i];
		delete[] m_pLightVal;
	}
	/*if(m_pPulseVal != NULL)
		delete[] m_pPulseVal;*/
	if(m_pPulseVal != NULL)
		for(int j=0; j<m_nStepCount/*NO_OF_SETS*/; j++)
			delete[] m_pPulseVal[j];
		delete[] m_pPulseVal;

	m_pLightVal = new BYTE*[m_nStepCount/*NO_OF_SETS*/];
	for(int i=0; i<m_nStepCount/*NO_OF_SETS*/; i++) {
		m_pLightVal[i] = new BYTE[m_nNoOfSegments];
		for(int j=0; j<m_nNoOfSegments; j++)
			m_pLightVal[i][j] = 0;
	}
	/*m_pPulseVal = new short[m_nNoOfSegments];
	for(int j=0; j<m_nNoOfSegments; j++)
		m_pPulseVal[j] = MAX_LIGHT_VALUE;*/
	m_pPulseVal = new short*[m_nStepCount/*NO_OF_SETS*/];
	for(int j=0; j<m_nStepCount/*NO_OF_SETS*/; j++) {
		m_pPulseVal[j] = new short[m_nNoOfSegments];
		for(int k=0; k<m_nNoOfSegments; k++)
			m_pPulseVal[j][k] = 0;
	}
}

int CLightControl::GetLightVal(CIniFile *ptrIniFile, int nController, int nSet)
{
	if(m_pLightVal == NULL)
		return -1;

	CString strSection, strName;
	strSection.Format("Controller%d", nController+1);
	for(int i=0; i<m_nNoOfSegments; i++) {
		strName.Format("LightData[%d][%d]", nSet, i);
		m_pLightVal[nSet][i] = ptrIniFile->GetInteger(strSection, strName, 0);
		/*strName.Format("PulseData[%d]", i);
		m_pPulseVal[i] = ptrIniFile->GetInteger(strSection, strName, MAX_LIGHT_VALUE);*/
		strName.Format("PulseData[%d][%d]", nSet, i);
		m_pPulseVal[nSet][i] = ptrIniFile->GetInteger(strSection, strName, 0);
		if(m_pLightVal[nSet][i] > MAX_LIGHT_VALUE)
			m_pLightVal[nSet][i] = MAX_LIGHT_VALUE;
	}

	return 0;
}

int CLightControl::SetLightVal(CIniFile *ptrIniFile, int nController, int nSet)
{
	if(m_pLightVal == NULL)
		return -1;

	CString strSection, strName;
	strSection.Format("Controller%d", nController+1);
	for(int i=0; i<m_nNoOfSegments; i++) {
		strName.Format("LightData[%d][%d]", nSet, i);
		ptrIniFile->SetInteger(strSection, strName, m_pLightVal[nSet][i]);
		/*strName.Format("PulseData[%d]", i);
		ptrIniFile->SetInteger(strSection, strName, m_pPulseVal[i]);*/
		strName.Format("PulseData[%d][%d]", nSet, i);
		ptrIniFile->SetInteger(strSection, strName, m_pPulseVal[nSet][i]);
		//OutputDebugLogTo(9,TRUE,"Light[%d][%d]=%d", nSet, i, m_pLightVal[nSet][i]);
	}

	return 0;
}

int CLightControl::LoadLightSetting(int nController, CString strDirectoryName, CString strFileName, int nSet, BOOL bLightDlgOpen, BOOL *bTrackSegments)
{
	if(m_pLightVal == NULL)
		return -1;

	CIniFile iniFile;
	CString strDataFileName;
	
	strDataFileName.Format("%s\\%s.ltv", strDirectoryName, strFileName);

	iniFile.SetPath(strDataFileName);

	if (iniFile.ReadFile()) {
		OutputDebugLogTo(4, TRUE, "[GetLightVal, nController%d] %s", nController, strDataFileName);
		GetLightVal(&iniFile, nController, nSet);
		OutputDebugLogTo(4, TRUE, "[GetLightVal, nController%d] Done", nController);
	}
	else {
		OutputDebugLog("%s not found. Default values assumed",strDataFileName);
		iniFile.ReadFile();
		SetLightVal(&iniFile, nController, nSet);
	/*	for(int i=0; i<3; i++)
			SetLightVal(&iniFile, i, nSet);*/
		iniFile.WriteFile();
	}
	
	m_nLightStep = -2;
	//if (m_nStepCount == 1&& !bLightDlgOpen)
	//{
	//	OutputDebugLogTo(4, TRUE, "[RefreshLight] %s", strDataFileName);
	//	RefreshLight(0/*0*//*nSet*/);
	//}
	//else
		
	RefreshLight(nSet, bTrackSegments);
		
	//SendMaxLightValues();
	//SendLightValues(nSet, nSet);
	OutputDebugLogTo(4, TRUE, "[LoadLightSetting] Done");

	return 0;
}

int CLightControl::SaveLightSetting(int nController, CString strDirectoryName, CString strFileName, int nSet)
{
	if(m_pLightVal == NULL)
		return -1;

	CIniFile iniFile;
	CString strDataFileName;
	strDataFileName.Format("%s\\%s.ltv", strDirectoryName, strFileName);
	//OutputDebugLogTo(9,TRUE,"%s", strDataFileName);
	iniFile.SetPath(strDataFileName);
	iniFile.ReadFile();
	SetLightVal(&iniFile, nController, nSet);
	iniFile.WriteFile();

	///RefreshLight(-1);
	///SaveInController();
	
	return 0;
}

void CLightControl::Configure(int nControllerNo,
							int nNoOfSegments,
							float* pMaxCurrents,
							int* pMaxPulseWidth,//int nMaxPulseWidth,
							int nSteps,
							int nControllerType)
{
	m_nController = nControllerNo;
	m_nNoOfSegments = nNoOfSegments;
	m_pMaxCurrents = pMaxCurrents;
	//m_nMaxPulseWidth = nMaxPulseWidth;
	m_pMaxPulseWidth = pMaxPulseWidth;

	m_nStepCount = nSteps;
	//if(nSteps > 1)
	//	m_nControllerType = PP820S15;
	//else
		m_nControllerType = nControllerType;

	//SendStepCount(nSteps);
	
	InitLightVal();

}

int CLightControl::Initialize()
{
	if(!m_bInitialized) {
		//if(SendString(&CString("CL")))
		//	return -1;
		for(int k=0; k<5; k++){
			if(SendString(&CString("CL"))==0)
				break;
		}
		//if(m_nControllerType==PP820S15) {
			if(SendString(&CString("FP1")))
				return -1;
		//}
		//else{
			if(m_nLightStep==-2)
				m_nLightStep=0;
		//}
		
		if(SendMaxLightValues(0))
			return -1;

		m_bInitialized = TRUE;
		return 0;
	}
	
	return -1;
}

int CLightControl::SetLight(int nChannel, int nCurrent, BOOL bSend)
{
	if(m_pLightVal == NULL || nChannel >= m_nNoOfSegments)
		return -1;

	int nStep = m_nLightStep;
	if(nStep < 0)
		nStep = 0;
	m_pLightVal[nStep][nChannel] = nCurrent;
	//OutputDebugLogTo(9,TRUE,"SetLight Step%d; Channel%d; Current%d", nStep, nChannel, nCurrent);//// for testing
	
	if(bSend) {
		if(m_nLightStep < 0) {
			if(RefreshLight(0))
				return -1;
		}
		SendLightValue(nChannel, nCurrent);
	}

	return 0;
}

int CLightControl::SetAllLights(int nCurrent)
{
	if(m_pLightVal == NULL)
		return -1;

	if(m_nLightStep < 0) {
		if(RefreshLight(0))
			return -1;
	}

	if(nCurrent >= 0) {
		for(int i=0; i<m_nNoOfSegments; i++)
			m_pLightVal[m_nLightStep][i] = nCurrent;
	}

	SendLightValues(__min(m_nLightStep, m_nCurrentStepCount-1), m_nLightStep,0);

	return 0;
}

int CLightControl::ReverseSteps(BOOL bReverse)
{
	if(m_bStepsReversed != bReverse) {
		for(int nSet=0; nSet<m_nStepCount; nSet++)
			SendLightValues(bReverse ? m_nStepCount - nSet - 1 : nSet, nSet,0);
		m_bStepsReversed = bReverse;
	}
	ResetSequenceStep();

	return 0;
}

int CLightControl::RefreshLight(int nSet, BOOL *bTrackSegments)
{
	if(m_nLightStep == nSet)
		return -1;

	if(m_pLightVal == NULL)
		return -1;

	Initialize();

	if(!m_bInitialized)
		return -1;
	
	//if(m_nControllerType==PP820S15) {
	//	m_nLightStep = nSet;
	//	if(nSet==-1) {
	//		SendStepCount(m_nStepCount);
	//		//SendMaxLightValues();
	//		for(nSet=0; nSet<m_nStepCount; nSet++)
	//			SendLightValues(nSet, nSet, bTrackSegments);
	//		//ResetSequenceStep();
	//		m_bStepsReversed = FALSE;
	//	}
	//	else {
	//		SendStepCount(1);
	//		SendLightValues(0, nSet, bTrackSegments);
	//	}
	//}
	//else {
		//m_nLightStep = 0;
		m_nLightStep = nSet;
		SendStepCount(1);
		SendMaxLightValues(0);
		if(SendLightValues(0, nSet, bTrackSegments)) {
			m_nLightStep = -2;
			return -1;
		}
	//}
	return 0;
}

int CLightControl::SetPulseWidth(int nChannel, int nPulseWidth, BOOL bSend)
{
	if(m_pPulseVal == NULL || nChannel >= m_nNoOfSegments)
		return -1;

	int nStep = m_nLightStep;
	if(nStep < 0)
		nStep = 0;
	//m_pPulseVal[nChannel] = nPulseWidth;
	m_pPulseVal[nStep][nChannel] = nPulseWidth;
	
	//OutputDebugLogTo(9,TRUE,"SetPulseWidth Step%d; Channel%d; PulseWidth%d", nStep, nChannel, nPulseWidth);//// for testing
	if(bSend) {
		SetAllPulseWidth();
		//SendMaxLightValues(__min(m_nLightStep, m_nCurrentStepCount-1), m_nLightStep);
	}
	return 0;
}

int CLightControl::SetAllPulseWidth()
{
	if(m_pLightVal == NULL)
		return -1;

	if(m_nLightStep < 0) {
		if(RefreshLight(0))
			return -1;
	}

	SendMaxLightValues(0);
	SendLightValues(__min(m_nLightStep, m_nCurrentStepCount-1), m_nLightStep,0);
	
	return 0;
}

int CLightControl::GetLight(int nChannel)
{
	if(m_pLightVal == NULL || nChannel >= m_nNoOfSegments)
		return -1;

	int nStep = m_nLightStep;
	if(nStep < 0)
		nStep = 0;

	return m_pLightVal[nStep][nChannel];
}

int CLightControl::GetPulseWidth(int nChannel)
{
	if(m_pPulseVal == NULL || nChannel >= m_nNoOfSegments)
		return -1;

	int nStep = m_nLightStep;
	if(nStep < 0)
		nStep = 0;

	//return m_pPulseVal[nChannel];
	return m_pPulseVal[nStep][nChannel];
}

int CLightControl::SendStepCount(int nSteps)
{
	if(nSteps < 0 || nSteps > m_nStepCount || nSteps==m_nCurrentStepCount)
		return -1;

	m_nCurrentStepCount = nSteps;

	return 0;
}

int CLightControl::ResetLightSteps()
{
	return ResetSequenceStep();
}

//void CLightControl::SendRTValues(int nChannel, int nPulseWidth)
//{
//	SetPulseWidth(nChannel, nPulseWidth);
//}

void CLightControl::UpdateLightStep(int nStep)
{
	m_nLightStep = nStep;
}
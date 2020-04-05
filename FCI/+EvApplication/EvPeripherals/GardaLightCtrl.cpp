/// CGardaLightCtrl: GardaSoft Light Control class derived from CLightControl
/// Udupa; May 2012
/// Udupa; Jan'2013; Individual bit Pulse Width

#include "StdAfx.h"
#include "LightControl.h"
#include "GardaLightCtrl.h"
#include "DebugLog.h"
#include "extIOfunct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGardaLightCtrl::CGardaLightCtrl()
{
}

CGardaLightCtrl::~CGardaLightCtrl()
{
}

int CGardaLightCtrl::SendString(CString* pstrData)
{
	EnterCriticalSection(&m_CriticalSection);
	int nError = OutPortSendString(m_nController, pstrData);
	LeaveCriticalSection(&m_CriticalSection);

	return nError;
}

int CGardaLightCtrl::SendMaxLightValues(BOOL *bTrackSegments)
{
	//OutputDebugLog("SendMaxLightValues::m_nLigthStep=%d",m_nLightStep);
	CString str1, str2 = "";
	for(int i=0; i<m_nNoOfSegments; i++) {
		//if(m_nControllerType==PP420)
		//	str1.Format("RR%d,%3.2f;", i+1, m_pMaxCurrents[i]);
		//else if(m_nControllerType==PP820)
		//	//str1.Format("RT%d,%d,0,%3.2f;", i, m_pPulseVal[i], m_pLightVal[0][i] * m_pMaxCurrents[i] / 100.0);
		//	str1.Format("RT%d,%d,0,%3.2f;", i, m_pPulseVal[m_nLightStep][i], m_pLightVal[m_nLightStep][i] * m_pMaxCurrents[i] / 100.0);
		//else
		//	//str1.Format("RT%d,%d,0,%3.2f;", i, m_pPulseVal[i], m_pMaxCurrents[i]);

		if (!bTrackSegments || (bTrackSegments[i] > 0)) {
			str1.Format("RT%d,%d,0,%3.2f;", i, m_pPulseVal[m_nLightStep][i], m_pMaxCurrents[i]);
			str2.Append(str1);	
		}
	}
	str2.Delete(str2.GetLength()-1);
	str2.Append("\r");

	return SendString(&str2);
}

int CGardaLightCtrl::SendMaxLightValue(int nChannel, int nPulseWidth)
{

	CString str;
	str.Format("RT%d,%d,0,%3.2f;", nChannel, nPulseWidth, m_pMaxCurrents[nChannel]);

	return SendString(&str);
}

int CGardaLightCtrl::SendLightValue(int nChannel, int nCurrent)
{
	//Initialize();
	////OutputDebugLog("SendLightValue::m_nLigthStep=%d",m_nLightStep);

	CString str;
	//if(m_nControllerType==PP420)
	//	//str.Format("RT%d,%dus,0,%d\r", nChannel+1, m_pPulseVal[nChannel], nCurrent);
	//	str.Format("RT%d,%dus,0,%d\r", nChannel+1, m_pPulseVal[m_nLightStep][nChannel], nCurrent);
	//else if(m_nControllerType==PP820)
	//	//str.Format("RT%d,%d,0,%3.2f\r", nChannel, m_pPulseVal[nChannel], nCurrent * m_pMaxCurrents[nChannel] / 100.0);
	//	str.Format("RT%d,%d,0,%3.2f\r", nChannel, m_pPulseVal[m_nLightStep][nChannel], nCurrent * m_pMaxCurrents[nChannel] / 100.0);
	//else
		str.Format("XS%d,%d,%d\r", nChannel, __min(m_nLightStep, m_nCurrentStepCount-1), nCurrent);
	
	return SendString(&str);
}

int CGardaLightCtrl::SendLightValues(int nStep, int nSet, BOOL *bTrackSegments)
{
	//Initialize();
	////OutputDebugLog("SendLightValues::m_nLigthStep=%d",m_nLightStep);

	CString str1, str2 = "";
	for(int i=0; i<m_nNoOfSegments; i++) {
		//if(m_nControllerType==PP420)
		//	//str1.Format("RT%d,%dus,0,%d;", i+1, m_pPulseVal[i], m_pLightVal[0][i]);
		//	str1.Format("RT%d,%dus,0,%d;", i+1, m_pPulseVal[m_nLightStep][i], m_pLightVal[m_nLightStep][i]);
		//else if(m_nControllerType==PP820)
		//	//str1.Format("RT%d,%d,0,%3.2f;", i, m_pPulseVal[i], m_pLightVal[0][i] * m_pMaxCurrents[i] / 100.0);
		//	str1.Format("RT%d,%d,0,%3.2f;", i, m_pPulseVal[m_nLightStep][i], m_pLightVal[m_nLightStep][i] * m_pMaxCurrents[i] / 100.0);
		//else
		if (!bTrackSegments || bTrackSegments[i] > 0)
		{
			str1.Format("XS%d,%d,%d\r", i, nStep, m_pLightVal[nSet][i]);
			str2.Append(str1);
		}

		//OutputDebugLogTo(9,TRUE,"%s", str1);//// for testing
	}
	str2.Delete(str2.GetLength()-1);
	str2.Append("\r");

	return SendString(&str2);
}

int CGardaLightCtrl::SendStepCount(int nSteps)
{
	/*if(m_nControllerType != PP820S15)
		return 0;*/

	if(CLightControl::SendStepCount(nSteps) == -1)
		return -1;

	CString str;
	str.Format("XC%d\r", nSteps);
	
	return SendString(&str);
}

int CGardaLightCtrl::SaveInController()
{
	return SendString(&CString("AW\r"));
}

int CGardaLightCtrl::ResetSequenceStep()
{
	if(m_nControllerType != PP820S15)
		return 0;

	return SendString(&CString("XR\r"));
}

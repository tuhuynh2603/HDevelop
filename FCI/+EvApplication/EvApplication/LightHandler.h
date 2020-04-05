/// Light Handler: Controls all light controllers within one station
/// Udupa; Sept'2012

#pragma once

class CLightControl;
class CTrackHandler;

class CLightHandler
{
public:
	CLightHandler();
	~CLightHandler();

	CApplication* m_pApp;
	void SetParameters(CTrackHandler* pTracks, int nTracks);
	int ConfigureControllers();
	int LoadLightSettings(int nDoc, int nImage = -1, BOOL bLightDlgOpen = FALSE);
	int SaveLightSettings(int nDoc, int nImage=-1);
	void RefreshLight(int nController, int nDoc, int nImage);

	int m_nControllerCount;
	//CLightControl* m_pControllers;
	CLightControl* m_pController;

	int m_nImageCount;
	int m_nControllerSegCount;
	CString* m_pstrSerialNos;
	int m_nSegmentsPerTrack;
	int m_nLightNoOfSegments[12/*MAX_DOC*//*8*/];

	float* m_pMaxCurrents;
	//int m_nMaxPulseWidth;
	int* m_pMaxPulseWidth;
	BOOL m_bShareAllChannel;
	int SetLight(int nController, int nChannel, int nCurrent, BOOL bSend=TRUE);
	int SetAllLights(int nController, int nFOV, int nCurrent = -1);
	int GetLight(int nController, int nChannel);
	int SetPulseWidth(int nController, int nChannel, int nPulseWidth, BOOL bSend=TRUE);
	int GetPulseWidth(int nController, int nChannel);
	int SendLightCommandString(int nController, CString* pstrLightCommand);
	int ResetLightSteps();
	int SetAllPulseWidth(int nController);
	//void SendRTValues();
	void SortingElements(CString strSrc, int nSrcCount, CString &strDst);
	void UpdateLightStep(int nController, int nStep);
	int RefreshLight();
	BOOL  bTrackSegments[8];

protected:
	void GetController(int& nController, int& nChannel);
};

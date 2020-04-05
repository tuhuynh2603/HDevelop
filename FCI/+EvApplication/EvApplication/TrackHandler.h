/// TrackHandler.h : interface of the CTrackHandler class
/// Udupa; April'2012

#pragma once

#include <queue>
#include "Camera_Interface.h"
#include "EvTrackHandler.h"

class CApplication;
class CAppDoc;
class CCameraSetupDialog;

#define MAX_DOCS 6//4
#define MAX_DIES 5
#define NO_OF_SEGMENTS	19
#define NO_OF_POSITION 1

class CTrackHandler : public CEvTrackHandler
{
public:
	void InitializeDocView(CAppDoc** pDoc);
	BOOL SetParameters(int nTrack);

	int LoadConfiguration();
	int LoadConfigurationServer();
	int LoadConfiguration(int nFov);

	void CopyRecipe(CString strRecipeSrcMode,CString strRecipeDesMode,CString strRecipeSrc, CString strRecipeDst);
	void ComGrab(int nDoc);
	void ComGrabInspect(int nDoc);
	void ComGrabInspectOnlineSimulation(int nDoc);

	void SetOpticsVal(CIniFile *ptrIniFile);
	void SaveOpticsSettings();
	void LoadOpticsSettings();
	
	int SetCameraValue(int nFeature, int nValue, int nParm=0);
	int GetCameraValue(int nFeature, int& nValue, int nParm=0);

	void SetCamActive(BOOL bActive);

	void RefreshLight(int nDoc=0);
	int ResetLightSteps();
	long getInspectionThreadId();
	long getInspectionThreadCount();

	CRITICAL_SECTION CriticalSectionScannedImage;

	std::vector <HANDLE> eventDoInspection;
	std::vector <HANDLE> eventInspectionDone;

	CArray<int, int> m_nLightControllerId;
	//CArray<int, int> m_nLightStartSegmentNo;

	CUIntArray m_nSegmentsOfImage[MAX_DOCS];
	CUIntArray m_nSelectedSegmentsOfImage[MAX_DOCS];

	//std::vector <UINT >  m_nSegmentsOfImage;
	//std::vector <UINT > m_nSelectedSegmentsOfImage;

	CTrackHandler();
	virtual ~CTrackHandler();

	CApplication* m_pApp;

	CAppDoc* m_pDoc[MAX_DOCS];
	int m_nTotalDocs;

	CAppDoc* m_pActiveDoc;
	CAppDoc* m_pActiveUpdateDoc;
	CAppDoc* m_pActiveDocTeach;
	
	int m_nAutoRunDelay;

	double m_dResolutionX;
	double m_dResolutionY;

	bool m_bContInsp;
	bool m_bTrackContInsp;
	bool m_bLiveImage;
	bool m_bLiveCalibration;
	bool m_bCamEnable;
	bool m_bCamAvail;
	bool m_bCamExtTrigEnable;
	bool m_bCamTrigAvail;
	BOOL m_bAutoRun;

	HANDLE m_hTrackCommandThread;
	//HANDLE m_hTrackUpdateThread;

	HANDLE m_EventGrabEnd;
	HANDLE m_EventUpdate;

	int EnqueueAsyncGrab();
	int WaitForEndOfExposure();
	int CancelSnap();

	HANDLE m_EventRun;
	HANDLE m_EventKillLive;
	HANDLE m_EventKillInspCycle;
	HANDLE m_EventKillAutoRun;

	HANDLE m_EventKillInspectImages;
	HANDLE m_hInspectImagesKilled;
	HANDLE m_EventKillLiveCalibration;
	CString m_strStoredImagesPath;


	int m_nCommand;
	
	int m_nTrackInspResult;
	int m_nStripInspResult;
	CPerformanceTimer TrackInspTime;

	CString m_strConfigDir;

	CString	m_strFGCard;
	CString	m_strFGCardFile;
	CString	m_strCameraFile;

	CCameraInterface m_FGResource;

	int m_nDefImgCount;

	BOOL m_bOpticsRoi;
	BOOL m_bViewOpticsPanel;

	CRect m_rectOptics[8];
	CPoint m_pCenter;
	int m_nOuterMajorRadius_x;
	int m_nOuterMinorRadius_y;
	int m_nInnerMajorRadius_x;
	int m_nInnerMinorRadius_y;
	int m_nOpticsFrames;
	int m_nOpticsAvgSize;
	int m_nOpticsTopOffset;
	int m_nOpticsBottomOffset;
	BYTE m_nOpticsRecalcMode;
	CString m_strConfigDirNew[7];
	CCameraSetupDialog* m_pCamSetupDlg;
	BOOL m_bCamSetupDlgOpen;
	CRect m_rectAoi;
	int m_nExposure;
	int m_nMaxExposure;
	int m_nGain[3];
	int m_nBlackLevel;

	void ZoomAllViews(int nMode);
	void UpdateLotDefectsList();
	int LoadCameraParm();
	void SaveCameraParm();
	void SaveConfigurationAs(CString strOldFile, CString strNewFile);
	int LoadingImagesOnline(int nTrack, int nIndex, CString *strImage, bool *bLoadComplete);
	int BrowseThruImagesForSimul(int nTrack);
	int m_nImageCount;
	bool bLoadComplete;

	CArray<CString, CString> AryStrNotInspectedFileNameOrg;
	CArray<CString, CString> AryStrNotInspectedFileNameOnlyOrg;

	CArray<CString, CString> AryStrNotInspectedFileName;
	CArray<CString, CString> AryStrNotInspectedFileNameOnly;

	CArray<int, int> ArynNotInspectedFileNameIndex;
	CArray<int, int> ArynInspectedFileNameIndex;

	int LoadSimulationBuffers();
	int FreeSimulationBuffers();
	int GetNextSimulationBuffer(int nDocIndex = 0);
	CArray<CImageBuffer*, CImageBuffer*> m_arrSimulationBuffers;
	CArray<CString, CString> m_arrSimulationFilenames;
	int m_nSimulationBufferIndex;

	int m_nUpdateCommand;
	void UpdateWatchDog(int nData);
	void UpdateLotChangedAck();

	int GetFailedImage(int nIndex, CFailImageStack*& pFailure);
	int WaitForSnapCompletion(BOOL bDisplay=TRUE, int nDoc=0, BOOL bCopyToDocBuffer=TRUE);

	CSharedData m_SharedData;

	//HANDLE m_hTrackUpdateImage;
	HANDLE m_hTrackEventUpdateImage;
	void UpdateGrabImage(int nDoc);
	char m_charTempPosNameForInsp;
	void StepDebugNavigate(int nStep);
	CString m_strLoadStaticImgFolderPath;
	int NavigateStaticImage(int nKey);
	int m_nFOVIndex;
	int m_nDieIndex;
	void SetZPositionForSequence(int nDoc);
	void SetZPositionForAutoFocusingSequence(int nDoc);
	HANDLE m_EventPositionEnd;
	void LoadPixelData(CString strRecipeFolder, CString strRecipeFile, int nTrack);
	volatile unsigned long m_nInspectionThreadId;//Use to determine FOV 
	std::vector < std::queue < long  >>grabbedImageId;
	std::vector < std::queue < long  >> grabbedDeviceId;//Use to determine Die/Device
};

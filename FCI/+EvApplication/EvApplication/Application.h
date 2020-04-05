/// Application.h : main header file for the Ev_InCuvette application
/// Udupa; April'2012

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#define MAX_VIEWS			8
#define MAX_TRACKS			3
#define AXES_COUNT			4
#define MAX_DIRECTION		2
#define MAX_FOV				20
#define MAX_DIES			5

#include "InspectionHandler.h"
#include "ApplicationDef.h"
#include "Hardware.h"
#include "ThreadManager.h"
#include "LotStruct.h"
#include "UserLoginAccount.h"
#include "SystemLogger.h"
#include "IO_Interface.h"
#include "Motor_Interface.h"
#include "AdvSecurityDlg.h"
#include "LightHandler.h"
#include "CommInterface.h"
#include "CentralizedCommInterface.h"
#include "MessageDlg.h"
#include "PixelRuler.h"
#include "InfoMsgDlg.h"
#include "DisplayOnlinePanel.h"
#include "ProgressBarDlg.h"
#include "MotionParameters.h"
#include "CentralizedFTPSettings.h"
#include <map>
#include "CDeepLearningInterface.h"
#include "ErrorDlg.h"
#include <queue>

enum
{
	STATION_TOP,
	STATION_BOTTOM,
	STATION_SIDE,
	TOTAL_STATIONS,
};

class CStatisticsView;
class CImageQueueHandler;
class CTrackHandler;
class CLightControlDlg;
class CScanBarcode;
class CImageSelectionDlg;
class CMotionControlDialog;
class CStationMapping;
class CPVIInspectionParameters;
class CCalibrationSettings;
class CCentralizedMonitorDlg;

class CApplication : public CWinAppEx
{
public:
	CApplication();

	int nTop,nBottom,nSide;
	int m_nTotalTracks;
	int m_nTotalViews;
	int m_nTotalViewDocs[MAX_TRACKS];
	int m_nMaxLineOutputLog;
	//CArray <int, int> m_nStartSegmentsOfImage;
	CTrackHandler* m_pTracks;
	//CSharedData m_SharedData[MAX_TRACKS];
	int m_nFirstTrackNo;
	int m_nOperationMode;
	CMDIChildWndEx* m_pFrames[MAX_VIEWS];
	CThreadManager m_ThreadManager;
	MotionParameters motionParameters;		//Girish
	int m_nActiveTrackIdx;
	int m_nActiveDocIdx;
	BOOL m_bEnableGroupingDocView;

	//CStatisticsView* m_pStatsView;
	CDialogPane* m_pImageSelectionDlg; //JP
	BOOL m_bInspectionParallelism;
	BOOL m_bStepSelection[THREAD_COUNT];//JP for Selecting defect for the step debug
	BOOL m_bStepSelectionEnable[THREAD_COUNT];
	BOOL m_bPixelRuler;
	BOOL bIsLeftButtonClicked;
	BOOL m_bIsMasterApp;
	BOOL m_bIsCameraMonitorMode;

	CErrorDlg * pErrorMsgDlg[NO_OF_PORT_CENTRALIZED];

	BOOL m_bImagesViewInitialised;
	BOOL m_bSetForAllTracks;
	BOOL m_bSetForSingleFov;
	int m_nInSpectionDelay;
//	int m_nNoOfDies;
//	int m_nNoOfFov;
//	int m_nNoOfFovX;
//	int m_nNoOfFovY;
	int m_nTrackCountGroup;
	int m_nFOVWidthCalculated;
	int m_nFOVHeightCalculated;
	CStringArray m_strTopCameraName;
	CStringArray m_strBottomCameraName;
	CStringArray m_strSideCameraName;
	
	CStringArray m_strTopCalibrationName;
	CStringArray m_strBottomCalibrationName;
	CStringArray m_strSideCalibrationName;
	
	int m_nImagesViewCX;
	int m_nImagesViewCY;
	int m_nTop;
	int m_nBottom;
	int m_nSide;

    CUserLoginAccount m_UserLog;
	UserAccountInfo m_UserAccInfo;
	BOOL m_bSuperUserMode;
	LOTSUMMARY m_LotSummary;
	
	int nNoOfResetPerDay;
    CString strLotResetTime1;
	CString strLotResetTime2;
	SYSTEMTIME tLotResetTime1;
	SYSTEMTIME tLotResetTime2;

	CString m_LotSummaryRootDir;
	CString m_ImageSaveFolder;
	CString m_LotSummaryLotIDDir;

	CString m_strConfigFile;
	CString m_strConfigDir;
	CString m_strMainCfgDir;
	CString m_strMainCfgModeDir;
	CString m_strAppId;

	CString m_strConfigModifiedDate;
	CString m_strSystemLoggingDir;

	CString m_strDefImageStore;

	int m_nGrabTimeout;

	CString m_strMachineID;
	CString m_strProductCode;
	CString m_strClientMachineID[NO_OF_PORT_CENTRALIZED];

	int m_nAppInstanceID;

	BOOL m_bOnline;

	BOOL m_bScanning; //for disable mapping Station 


	CRITICAL_SECTION m_CriticalSection;
	CRITICAL_SECTION m_csOnline;
	CRITICAL_SECTION m_csFileAccess;
	CRITICAL_SECTION m_csOutputData;

	CRITICAL_SECTION m_csLoadConfig;

	CRITICAL_SECTION m_csCentralizedQueue;
	std::queue < int  > m_CentralizedStationQueue;
	std::map<int, int> m_mapCentralizedStationQueue;
	int m_nCentralizedStationLatest;

	HANDLE m_eventOnline;
	HANDLE m_eventVisionReady;
	HANDLE m_eventOnlineSimulationContinue;
	HANDLE m_EventCommSendMsg;

	HANDLE m_hResetReceived;
	HANDLE m_hResetDone;
	HANDLE m_hInitPosition;
	HANDLE m_hInitPositionDone;

	HANDLE m_hInitMotionDone;

	CImageQueueHandler* m_pImageQueue;
	BOOL m_bAutoResizeImage;
	BOOL m_bAutoResizeImageToMidRes;
	BOOL m_bAutoSaveResizedImage;
	BOOL m_bCalibrationOpen;
	int m_nSystemRestartCounter;
	BOOL m_bSaveCrashImages;
	int m_nCurrentLightControllerIndex;

	CString	m_strLightLibrary;
	CString	m_strIOInterfaceCard;
	CString m_strMotorParmFile;
	CString m_strMotionCard;
	int m_nMotorCardNo;				//Girish
	int m_nMotionTimeout;
	MotorInterface m_MotorResource;
	CString m_strCentralizedFTPSharedDirectory;

	BOOL m_bEnableMachineResetOperation;


	BOOL m_bEmergencyStop;
	BOOL m_bCurtainSensor;
	BOOL m_bBypassCurtainSensor;
	BOOL m_bMachineOn;
	int m_nAxes[AXES_COUNT];
	CLightControlDlg* m_pLightCtrlDlg;
	CCalibrationSettings *m_pCalibrationDlg;
	CScanBarcode* m_pScanBarcodeDlg;
	BOOL m_bLightCtrlDlgOpen;

	int m_lIOCardAddr;
	int m_lIOInterfaceCardAddr;

	//CAuditTrailDlg* m_pAuditTrailDlg;
	CWnd* m_pAuditTrailDialog;
	CString m_strAuditLoggingDir;
	CString m_strParmFolder;
	//CCentralizedFTPSettings* m_pCentralizedFTPSettings;

	int m_lIOReverseLogic;

	CLightHandler m_LightHandler;

	CIOInterface m_LightModule;
	CIOInterface m_CommModule;

	//Deep Learning Dll 
	CString	m_strDeepLearningLibrary;
	CDeepLearningInterface m_DeepLearningModule;
	

	long m_lCOMMInCardNo;
	long m_lCOMMInPortID;
	long m_lCOMMOutCardNo;
	long m_lCOMMOutPortID;


	CCommInterface m_CommInterface;
	CCentralizedCommInterface m_CentralizedCommInterface;
	CPixelRuler *m_pPixelRulerDlg;

	int m_nCrashImageCounter[MAX_TRACKS];
	HANDLE m_hInspectSeqThread;
	//HANDLE m_hVisionResetThread;
	//HANDLE m_hPollingThread;

	int m_nFirstGrabDelay;
	int m_nSignalScanFrequency;
	int m_bEnableSimulationMode;
	CString m_strSimulationImagesPath;
	int m_nMaxSimulationBuffers;

	int m_nUnitIndex;

	int m_bContinuousLoop;

	std::vector< int > m_nFovCompletedTeach;
	BOOL m_bTeachPass;
	BOOL m_bTeaching;
	BOOL m_bTeachSingleImage;
	BOOL m_bInspecting;
	BOOL m_bBarcodeScanning;
	BOOL m_bRecipeFound;

	BOOL m_bTrackInspSeq;
	BOOL m_bCamExtTrig;

	int m_nCamLibraryTrackIndex;

	BOOL m_bExitApp;

	BYTE m_cActiveTracks;
	BOOL m_bByPassResult;
	BOOL m_RuntimeDisplayEnable;

	BOOL m_bClassPriorityRealtime;
	BOOL m_bAllocFailedImageBuffer;
	int m_nFailImageCounter[MAX_TRACKS];
	BOOL m_bSaveInspSeqImages;
	BOOL m_bInspSeqLoop;
	BOOL m_nSequenceCounter;
	CPerformanceTimer InspectSeqTime;

    int m_nDeviceCounter;
	int m_nOutData;

	int m_nBarcodeSize;
	int m_nProductCodeSize;

	BOOL m_bSaveOnlinePassImages;
	BOOL m_bSaveOnlineFailImages;
	BOOL m_bSaveOnlineJpegImages;
	BOOL m_bSaveOnlineBMPImages;

	BOOL m_bSaveDLImages;
	BOOL m_bSaveDLPassImages;
	BOOL m_bSaveDLFailImages;
	CString m_ImageSaveFolderDL;
	BOOL m_bOnlineContinueTopImageInsp;
	int m_nSaveJpegQualityImages;
	BOOL m_bStoredImagesLoop;
	int m_nMinimumDiskSpaceImageSaveGB;//Minimum disk space free for Image Saving (Gigabytes)
	BOOL m_bEnableDLModelInspect;
	BOOL m_bUseDLModelResult;
	BOOL m_bRemoveBackGround;
	BOOL m_bAutoFocusingSequence;//Auto Focusing While Running Sequence

	CString m_strCommOPC;
	//CADSInterface *m_pADSInterface;
	BYTE m_byteNetID[6];
	unsigned int m_nADSPortNr;
	int m_nCycleTime;
	int m_nTxDelay;
	int m_nMaxDelay;
	int m_nTimeout;
	void SaveADSTwinCATSetting();

	void UpdateWatchDog(int nData);
	void UpdateLotChangedAck();

	int m_nToolNo[5];
	int m_nMoldNo[5];
	int m_nGripperNo[5];
	int m_nFailNo[5];
	int m_nResultType;
	int m_nGoodACounter;
	int m_nGoodBCounter;
	int m_nFailCounter;

	BOOL m_bInspectionAffinity;
	BOOL m_bLoadComplete;
	BOOL m_bSynchronousGrabEnable;
	CAdvSecurityDlg m_AdvSecurityDlg;
	BOOL m_bZoomAll;
	BOOL m_bLotOpened;// Is Lot Information Entered
	CString m_strLotInfo;
	CString m_strLotConfigFile;
	BOOL m_bDisplayAllStatsOffline;
	//LOTSUMMARY m_LotSummary;
	BOOL m_bStream;
	BOOL m_bSnap;
	BOOL m_bCalib;
	CStationMapping* m_pStationMapDlg;
	BOOL m_bStationMapDlgOpen;
	BOOL m_bAllowStream;
	int m_nFOVHeight;
	int m_nFOVWidth;
	/*int m_nDeviceNoPerBlockX;
	int m_nDeviceNoPerBlockY;
	int m_nDevicePitchX;
	int m_nDevicePitchY;
	int m_nDeviceOffsetX;
	int m_nDeviceOffsetY;*/

	//int m_nNoOfBlock[MAX_DIRECTION];
	//double m_dBlockPitch[MAX_DIRECTION];
	//int m_nDeviceNoPerBlock[MAX_DIRECTION];
	//double m_dDevicePitch[MAX_DIRECTION];
	//double m_dDeviceScanArea[MAX_DIRECTION];
	//double m_dDeviceOffset[MAX_DIRECTION];//// commenting as per issue list
	//double m_dDeviceOverlap[MAX_DIRECTION];
//	void DeviceConfigFile(BOOL bSave, BOOL bCreate = FALSE);

	CCentralizedMonitorDlg* m_pCentralizedMonitorDlg;

	CMotionControlDialog* m_pMotionControl;
	BOOL bOpenedMotionDialog;

	BOOL m_bMotionCtrlDlgOpen;

	CProgressBarDlg* m_pProgressDlg;
	BOOL m_bProgressDlgOpen;

	BOOL m_bUpdatingMapping;

	BOOL m_bLoadMultiImagesFromFile;
	CString m_strLoadMultiImagesFolderPath;
	CStringArray  m_strLoadMultiImagesArray;
	CStringArray m_strLightChannelName;
	BOOL m_bInspSequenceOffline;
//	int m_nXAxisPosition[/*MAX_FOV*/8];
//	int m_nYAxisPosition[/*MAX_FOV*/8];
	BOOL m_bLightCalibration;
	BOOL m_bDotCalibration;
	int m_nCalibXAxisPosition;
	int m_nCalibYAxisPosition;
	int m_nCalibZAxisPositionTop;
	int m_nCalibZAxisPositionBottom;
	int m_nStepTrack;
	int m_nStepDoc;
	int m_nXAxisSettlingTime;
	int m_nYAxisSettlingTime;

	HANDLE m_EventRefreshLight;
	HANDLE m_EventMotionSensor;
	HANDLE m_EventMotionSensorStart;
	HANDLE m_EventLogInspectionData;

	CPVIInspectionParameters* m_pWndPVIDefectParameters;
	BOOL m_bAutoLightImageSave;//for Calibration;
	CMessageDlg *m_pDisplayMsgStatus;
	//for ethernet comm
	BOOL m_bIsFailPart;
	CString m_strPartFlex[MAX_DIES];
	int m_nPartErrorCode[MAX_DIES];
	int m_nCentralizedPartErrorCode[NO_OF_PORT_CENTRALIZED][MAX_DIES];
	int m_nDieErrorCodeStats[MAX_DIES];
	BOOL m_bPartInspect[MAX_DIES];
	int m_nFovPartErrorCode[MAX_DIES][MAX_FOV];
	int m_nCentralizedFovPartErrorCode[NO_OF_PORT_CENTRALIZED][MAX_DIES][MAX_FOV];
	CBrush m_colorFovDieMapping[MAX_DIES][MAX_FOV];
	BOOL m_bReceivedStartCmd;
	BOOL m_bModifierView;
	BOOL m_bReceivedResetCmd;	
	BOOL m_bReceivedResetCmdPrevStatus;

	BOOL m_bReceivedStopCmd;
	int m_nVerificationMode;
	BOOL m_bShowDieVerification;
	BOOL m_bReScanSequence;
	BOOL m_bInspectionAll;
	CDisplayOnlinePanel *m_pDispOnlinePanel;
	CInfoMsgDlg* m_pInfoMsgDlg;
	int UpdateInfoMsgDlg(CString strCamDocName, CString strInfoMsg);
	CArray<CParameters*> m_ParametersDefectCode;
	CArray<int> m_nDefectErrorCodesPriority;
	CArray<int> m_nDefectErrorCodesPriorityTemp;
	CStringArray m_strDefectErrorCodesPriority;
	CStringArray m_strDefectErrorCodesPriorityTemp;
	CDefectCodes *m_pDefectCode;

	CMFCRibbonComboBox* m_pComboDLMode;

	std::map<int, int> m_mapDefectErrorCodesPriority;

	int m_nNo_of_Defects;
	int m_nErrorCode[MAX_DIES][MAX_FOV][MAX_VIEWS];
	int m_nCentralizedErrorCode[NO_OF_PORT_CENTRALIZED][MAX_DIES][MAX_FOV][MAX_VIEWS];
	CString m_strCalibDir;
	BOOL m_bFwdDirectionForTiltInsp;
	int m_nPviNumberOfArea[MAX_FOV][MAX_VIEWS];
	int m_nPviNoOfDefects[MAX_FOV][MAX_VIEWS][10];
	int m_nSaveAllDieIndex;
	CStringArray m_strPviDefectsStats[MAX_FOV];
	std::map<int, CString> m_mapPviDefectsStatsGeneral;
	BOOL m_bCentralizedVerificationMode;
	BOOL m_bMultithreadInspectionMode;
// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void InitializeDocView();
	BOOL SetParameters();
	BOOL InitProcess();
	int LoadDefaults(int nMaster, int nTrackCount);

	int CheckPassword(int SecurityAccessNo=0);

	int CombinedComGrab();
	int CombinedComGrabInsp();
	int WaitForEndGrab(int nReserved=0);
	int WaitForEndTrackGrab(int nTrack);
	int WaitForEndTrackInspect(int nTrack);
	int CombinedWaitForEndInsp(int nReserved=0);
	int CheckForHalconnLicense();

	int CheckApplicationLicense();
	CString DecodeLicenseKey(CString strContent, CString strDecodeKey);
	CString EncodeKey(CString strContent, CString strKey);

	int SetVisOnLineState(BOOL bStatus);
	int UpdateResults();
	void ClearResults();
	int UpdateResultsOffline(int nDie, int nFov, BOOL bTotalSummary);
	void LoadImagesForVerfication(CString Path, int nDieNo);
	int GetImageFilePaths(CString strStartFolder, CStringArray& arrayFilePaths);
	BOOL m_bDieInspection[MAX_DIES][MAX_FOV];
	int m_nTotalDefectsCount;
	void LoadServerRecipes();
	void LoadRecipes();
	void LoadRecipes(int nTrack, int nFov);
	void AutoLoadRecipes();
	void SaveRecipeInfo();
	void CopyConfigFile(CString strSrcFile, CString strDstFile);
	void ShowMotionInitializing();
	int CopyRecipe(CString strRecipeSrcMode,CString strRecipeDesMode,CString strRecipeSrc, CString strRecipeDst);
	CString GetFailedStr(int err,bool abbrev);
	CString GetCuvetteFailedStr(int err,bool abbrev);

	void UpdateLotSummary(int nDevice, int nFov, int nView, int nError, BOOL allStats = FALSE);
	void UpdateTotalLotSummary(int nDevice,int nError, BOOL allStats = FALSE);
	void UpdateStatisticsWindow(BOOL bRefresh = FALSE);

	void UpdateCentralizedMachineID(int nStationId);
	void UpdateCentralizedMachineState(int nStationId, CString strMachineState, COLORREF colorState = RGB(0, 255, 0));
	void UpdateCentralizedSummary(int nStationId);

	void RecordLotStats();
	int AddLogEvent(CString strEvent,int nEventLog=0,BOOL m_bAuditTrail=TRUE);

	void SaveCrashImages(int nTrack);

	int InitializeLightLibrary();
	int InitializeCommLibrary();
	int InitializeMotionCard();
	int ConfigureLightControllers();

	int InitializeDeepLearningLibrary();
	int InitializeDeepLearningModel();

	int StartInspectSeq(long lInData);
	int StartInspectSeqOnlineSimulation(long lInData);

// Implementation
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	BOOL CheckDiskFreeSpace();//Check "Folder: Image Save" Disk Space
	BOOL CheckDLDiskFreeSpace();//Check "Folder:DL Image Save" Disk Space
	void ApplyDebugFlag(long lDebugFlag);

	void UpdateTrackStatus();
	int SavePresentationPassImages(int nTrack, int nError);

	void CheckConfiguration();
	void SetRecipeName(CString strRecipe);
	void SetUserName(CString strUser);
	void SetOnlineStatus();

	void BinarizeAllTracks(BOOL bBinaryMode);
	void StreamAllTracksToggle();
	int SynchronousCameraSnap(BOOL bDraw=TRUE);
	void SynchronousCameraStream();

	void EnableMachineResetOperation(BOOL enable);
	BOOL GetEnableMachineResetOperationStatus() const;

	void InitializeLotStats();
	void RefreshDefectsList();
	int LoadDocImages(CString strFileName);
	void StationMapDlgControls();
	void InitializePositionProperties();
	void ResizeDieSelectionDlg();
	void OnMaximize(BOOL bEnable);
	
	void StepDebugNavigate(int nStep);
	int NavigateDieImages(int nKey);
	int m_nNavigateStaticImgTrack;
	void SetCurPosParamDisplay(int nTrack, CString* str);
	void ScannedFOVSequenceData(BOOL bRead);
	int InspectionScanSequenceOffline();
	int InspectionScanSequenceOnline();
	int WaitForPositionToEnd();
	int CalculateFOVWidthAndHeight();
	int CalculateFOVAndDieNumber(BOOL bCreate = FALSE);
	int SequenceScanCameraSnap(BOOL bDraw=TRUE);
	int OpenLotDlg();
	void ShowMessageDlgBox(BOOL bTrue);
	void ShowMessageDlgBox(CString strMessage, BOOL bShow = FALSE);

	int OnReceiveResetCmd();
	void MoveToStartPos(BOOL bHome);
	
	void CreateDefectCodeParmList();
	int LoadDefectCodeParameters(CString strRecipeFolder, CString strRecipeFile);
	int SaveDefectCodeParameters(CString strRecipeFolder, CString strRecipeFile);
	
	void GetInformationFromErrorCode(int nErrorCode, CString strDefectName, int &nTrackIdx, int &nDocIdx);

	void InitializeDefectCode();

	int getMotionTimeOut() const;
	
	void UpdateClientMachineIDPanel(CString strClientMachineId);
	//BOOL m_bDryRunMachine;

	void UpdateStatistics(int nDie);


	//void LaunchWindowsExplorer();
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnVisionOnline();
	afx_msg void OnUpdateVisionOnline(CCmdUI *pCmdUI);
	afx_msg void OnGooffline();
	afx_msg void OnOnlineStatsResetMode();
	afx_msg void OnUpdateOnlineStatsResetMode(CCmdUI *pCmdUI);
	afx_msg void OnOnlineSavePassImages();
	afx_msg void OnUpdateOnlineSavePassImages(CCmdUI *pCmdUI);
	afx_msg void OnOnlineSaveFailImages();
	afx_msg void OnUpdateOnlineSaveFailImages(CCmdUI *pCmdUI);
	afx_msg void OnOnlineSaveJpegImages();
	afx_msg void OnUpdateOnlineSaveJpegImages(CCmdUI *pCmdUI);
	afx_msg void OnOnlineSaveBMPImages();
	afx_msg void OnUpdateOnlineSaveBMPImages(CCmdUI *pCmdUI);

	afx_msg void OnOnlineSaveDeepLearningImages();
	afx_msg void OnUpdateOnlineSaveDeepLearningImages(CCmdUI *pCmdUI);

	afx_msg void OnOnlineSaveDLPassImages();
	afx_msg void OnUpdateOnlineSaveDLPassImages(CCmdUI *pCmdUI);

	afx_msg void OnOnlineSaveDLFailImages();
	afx_msg void OnUpdateOnlineSaveDLFailImages(CCmdUI *pCmdUI);

	//Deep Learning Model Setting
	afx_msg void OnEnableDLModelInspect();
	afx_msg void OnUpdateEnableDLModelInspect(CCmdUI *pCmdUI);

	afx_msg void OnUseDLModelResult();
	afx_msg void OnUpdateUseDLModelResult(CCmdUI *pCmdUI);

	afx_msg void OnInspAutoFocusingSequence(); 
	afx_msg void OnUpdateInspAutoFocusingSequence(CCmdUI *pCmdUI);

	afx_msg void OnOnlineRemoveBackGround();
	afx_msg void OnUpdateOnlineRemoveBackGround(CCmdUI *pCmdUI);


	afx_msg void OnOnlineContinueTopImageInsp();
	afx_msg void OnUpdateOnlineContinueTopImageInsp(CCmdUI *pCmdUI);
	afx_msg void OnParmRecipeLoad();
	afx_msg void OnUpdateParmRecipeLoad(CCmdUI *pCmdUI);
	afx_msg void OnParmScanBarcode();
	afx_msg void OnUpdateParmScanBarcode(CCmdUI *pCmdUI);
	//afx_msg void OnParmRecipeSaveAs();
	//afx_msg void OnUpdateParmRecipeSaveAs(CCmdUI *pCmdUI);
	//afx_msg void OnParmRecipeDelete();
	//afx_msg void OnUpdateParmRecipeDelete(CCmdUI *pCmdUI);
	afx_msg void OnUpdateParmConfig(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRecipeButton(CCmdUI *pCmdUI);
	afx_msg void OnUpdateModeAndProductCodeButton(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHardwareButton(CCmdUI *pCmdUI);
	afx_msg void OnConfigurationAdstwincatsettings();
	afx_msg void OnUpdateConfigurationAdstwincatsettings(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOnlinemodeOptions(CCmdUI *pCmdUI);

	afx_msg void OnUpdateToolsSettings(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolsUser(CCmdUI *pCmdUI);
	afx_msg void OnSettingsUsers();
	afx_msg void OnUpdateSettingsUsers(CCmdUI* pCmdUI);
	afx_msg void OnSettingsAccessRights();
	afx_msg void OnUpdateSettingsAccessRights(CCmdUI* pCmdUI);
	afx_msg void OnUserLogIn();
	afx_msg void OnUpdateUserLogIn(CCmdUI* pCmdUI);
	afx_msg void OnUserLogOut();
	afx_msg void OnUpdateUserLogOut(CCmdUI* pCmdUI);
	afx_msg void OnUserChangePassword();
	afx_msg void OnUpdateUserChangePassword(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUserButton(CCmdUI *pCmdUI);
	afx_msg void OnUserButton();
	afx_msg void OnVisionstatusButton();
	afx_msg void OnUpdateVisionstatusButton(CCmdUI *pCmdUI);
	afx_msg void OnRecipeButton();
	afx_msg void OnHardwareIO();
	afx_msg void OnUpdateHardwareIO(CCmdUI *pCmdUI);
	afx_msg void OnOnlineBypassResult();
	afx_msg void OnUpdateOnlineBypassResult(CCmdUI *pCmdUI);
	afx_msg void OnCameraSynchronousGrab();
	afx_msg void OnUpdateCameraSynchronousGrab(CCmdUI *pCmdUI);
	afx_msg void OnViewOptionsAuditTrail();
	afx_msg void OnUpdateViewOptionsAuditTrail(CCmdUI *pCmdUI);
	afx_msg void OnViewZoomAll();
	afx_msg void OnUpdateViewZoomAll(CCmdUI *pCmdUI);
	afx_msg void OnOpenLot();
	afx_msg void OnUpdateOpenLot(CCmdUI *pCmdUI);
	afx_msg void OnCloseLot();
	afx_msg void OnUpdateCloseLot(CCmdUI *pCmdUI);
//	afx_msg void OnCameraStream();
//	afx_msg void OnUpdateCameraStream(CCmdUI *pCmdUI);

	afx_msg void OnInspectionLoop();
	afx_msg void OnUpdateInspectionLoop(CCmdUI *pCmdUI);
	
	afx_msg void OnDefaultSetting();
	afx_msg void OnUpdateDefaultSetting(CCmdUI *pCmdUI);

	afx_msg void OnParmOptionsTeachInspAndLoc();
	afx_msg void OnUpdateParmOptionsTeachInspAndLoc(CCmdUI *pCmdUI);
	afx_msg void OnParmOptionsDeviceConfiguration();
	afx_msg void OnUpdateParmOptionsDeviceConfiguration(CCmdUI *pCmdUI);
	afx_msg void OnParmOptionsDefectPrioritySelection();
	afx_msg void OnUpdateParmOptionsDefectPrioritySelection(CCmdUI *pCmdUI);
	afx_msg void OnMotionControl();
	afx_msg void OnUpdateMotionControl(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMotorInitializing(CCmdUI *pCmdUI);

	//afx_msg void OnToolsStaticimageLoadMultiImgFromFile();
	//afx_msg void OnUpdateToolsStaticimageLoadMultiImgFromFile(CCmdUI *pCmdUI);

	afx_msg void OnInspInspectionSequence();
	afx_msg void OnUpdateInspInspectionSequence(CCmdUI *pCmdUI);
	
	/*afx_msg void OnCameraSnapAll();
	afx_msg void OnUpdateCameraSnapAll(CCmdUI *pCmdUI);*/

	
	afx_msg void OnInspSequenceLoop();
	afx_msg void OnUpdateInspSequenceLoop(CCmdUI *pCmdUI);

	afx_msg void OnInspSaveSequenceImages();
	afx_msg void OnUpdateInspSaveSequenceImages(CCmdUI *pCmdUI);

	
	afx_msg void OnPixelRuler();
	afx_msg void OnUpdatePixelRuler(CCmdUI *pCmdUI);
	//afx_msg void OnToolsDryRun();
	//afx_msg void OnUpdateToolsDryRun(CCmdUI *pCmdUI);
	afx_msg void OnButtonDefectSelection();
	afx_msg void OnUpdateButtonDefectSelection(CCmdUI *pCmdUI);
	/*afx_msg void OnButtonFileTransferSettings();
	afx_msg void OnUpdateButtonFileTransferSettings(CCmdUI *pCmdUI);*/
	afx_msg void OnButtonCentralizedPc();
	afx_msg void OnUpdateButtonCentralizedPc(CCmdUI *pCmdUI);
	afx_msg void OnCheckEnableCentralizedVerification();
	afx_msg void OnUpdateCheckEnableCentralizedVerification(CCmdUI *pCmdUI);
	afx_msg void OnButtonCentralizedMonitor();
	afx_msg void OnUpdateButtonCentralizedMonitor(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDLMode(CCmdUI *pCmdUI);
	afx_msg void OnCbnSelchangeComboDLMode();
	void UpdateUPH(double dStripTime, int nDieNo);
};

int DeleteDir(LPCTSTR lpszName);

extern CApplication theApp;

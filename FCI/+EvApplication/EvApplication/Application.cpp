/// Application.cpp : Defines the class behaviors for the application.
/// Udupa; April'2012
#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Application.h"
#include "MainFrame.h"
#include <direct.h>

#include "EvTabDoc.h"
#include "AppTabView.h"
#include "EvChildFrame.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include "AppView.h"
#include "CameraCode.h"
#include "IOCode.h"
#include "StatisticsDoc.h"
#include "StatisticsView.h"
#include "SplashScreen.h"
#include "StatisticsResetDlg.h"
//#include "LoadRecipeDlg.h"
//#include "CopyRecipeDlg.h"
//#include "DeleteRecipeDlg.h"
#include "ModifyAccounts.h"
#include "LoginDlg.h"
#include "ChangePasswordDlg.h"
#include "AdvSecurityDlg.h"
#include "LightControlDlg.h"
#include "IOTestDialog.h"
#include "PasswordDialog.h"
#include "MultipleRecipeDialog.h"
#include "ScanBarcode.h"
#include "ImageQueueHandler.h"
#include"ImageSelectionDlg.h"
#include "LotInfoDlg.h"
#include "AuditTrailDlg.h"
#include "StationMapping.h"
#include "DeviceConfig.h"
#include "DefectPrioritySelDlg.h"
#include "MotionControlDialog.h"
#include "PVIInspectionParameters.h"
#include "CalibrationSettings.h"
#include "DefectCodesDlg.h"
#include "CentralizedCommunicationDlg.h"
#include "CentralizedCommInterface.h"
#include "CentralizedCommManager.h"
#include "CentralizedCommMonitor.h"
#include "CentralizedMonitorDlg.h"
#include "SequenceThread.h"
#include <winsock2.h>
#include <IPHlpApi.h>
#include <stdlib.h>
#include "CommMonitor.h"
#include "CommManager.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))


// CApplication

BEGIN_MESSAGE_MAP(CApplication, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	ON_COMMAND(ID_ONLINEMODE_VISIONONLINE, OnVisionOnline)
	ON_UPDATE_COMMAND_UI(ID_ONLINEMODE_VISIONONLINE, OnUpdateVisionOnline)
	ON_COMMAND(ID_GOOFFLINE, OnGooffline)
	ON_COMMAND(ID_ONLINE_STATSRESETMODE, OnOnlineStatsResetMode)
	ON_UPDATE_COMMAND_UI(ID_ONLINE_STATSRESETMODE, OnUpdateOnlineStatsResetMode)
	ON_COMMAND(ID_ONLINE_SAVEPASSIMAGES, OnOnlineSavePassImages)
	ON_UPDATE_COMMAND_UI(ID_ONLINE_SAVEPASSIMAGES, OnUpdateOnlineSavePassImages)
	ON_COMMAND(ID_ONLINE_SAVEFAILIMAGES, OnOnlineSaveFailImages)
	ON_UPDATE_COMMAND_UI(ID_ONLINE_SAVEFAILIMAGES, OnUpdateOnlineSaveFailImages)
	ON_COMMAND(ID_CHECK_SAVE_AS_JPEG, OnOnlineSaveJpegImages)
	ON_UPDATE_COMMAND_UI(ID_CHECK_SAVE_AS_JPEG, OnUpdateOnlineSaveJpegImages) 
	ON_COMMAND(ID_CHECK_BMP_FORMAT, OnOnlineSaveBMPImages)
	ON_UPDATE_COMMAND_UI(ID_CHECK_BMP_FORMAT, OnUpdateOnlineSaveBMPImages)

	ON_COMMAND(ID_ONLINE_SAVE_DL_IMAGES, OnOnlineSaveDeepLearningImages)
	ON_UPDATE_COMMAND_UI(ID_ONLINE_SAVE_DL_IMAGES, OnUpdateOnlineSaveDeepLearningImages)
	ON_COMMAND(ID_CHECK_SAVE_DL_PASS_IMAGE, OnOnlineSaveDLPassImages)
	ON_UPDATE_COMMAND_UI(ID_CHECK_SAVE_DL_PASS_IMAGE, OnUpdateOnlineSaveDLPassImages)
	ON_COMMAND(ID_CHECK_SAVE_DL_FAIL_IMAGE, OnOnlineSaveDLFailImages)
	ON_UPDATE_COMMAND_UI(ID_CHECK_SAVE_DL_FAIL_IMAGE, OnUpdateOnlineSaveDLFailImages)

	ON_COMMAND(ID_REMOVE_BACK_GROUND, OnOnlineRemoveBackGround)
	ON_UPDATE_COMMAND_UI(ID_REMOVE_BACK_GROUND, OnUpdateOnlineRemoveBackGround)
	
	ON_COMMAND(ID_ENABLE_DL_INSPECT, OnEnableDLModelInspect)
	ON_UPDATE_COMMAND_UI(ID_ENABLE_DL_INSPECT, OnUpdateEnableDLModelInspect)

	ON_COMMAND(ID_USE_DL_MODEL_RESULT, OnUseDLModelResult)
	ON_UPDATE_COMMAND_UI(ID_USE_DL_MODEL_RESULT, OnUpdateUseDLModelResult)

	ON_COMMAND(ID_ONLINE_CONTINUEINSPECTION, OnOnlineContinueTopImageInsp)
	ON_UPDATE_COMMAND_UI(ID_ONLINE_CONTINUEINSPECTION, OnUpdateOnlineContinueTopImageInsp)
	ON_COMMAND(ID_PARM_CONFIG, OnParmRecipeLoad)
	ON_UPDATE_COMMAND_UI(ID_PARM_CONFIG, OnUpdateParmRecipeLoad)
	ON_COMMAND(ID_PARM_SCAN_BARCODE, OnParmScanBarcode)
	ON_UPDATE_COMMAND_UI(ID_PARM_SCAN_BARCODE, OnUpdateParmScanBarcode)
	//ON_COMMAND(ID_PARM_CONFIG_SAVEAS, OnParmRecipeSaveAs)
	//ON_UPDATE_COMMAND_UI(ID_PARM_CONFIG_SAVEAS, OnUpdateParmRecipeSaveAs)
	//ON_COMMAND(ID_PARM_CONFIG_DELETE, OnParmRecipeDelete)
	//ON_UPDATE_COMMAND_UI(ID_PARM_CONFIG_DELETE, OnUpdateParmRecipeDelete)
	ON_UPDATE_COMMAND_UI(ID_PARM_CONFIG, OnUpdateParmConfig)
	ON_COMMAND(ID_RECIPE_BUTTON, OnRecipeButton)
	ON_UPDATE_COMMAND_UI(ID_RECIPE_BUTTON, OnUpdateRecipeButton)
	ON_UPDATE_COMMAND_UI(ID_PRODUCT_CODE_BUTTON, OnUpdateModeAndProductCodeButton)
	ON_UPDATE_COMMAND_UI(ID_MODE_BUTTON, OnUpdateModeAndProductCodeButton)
	ON_UPDATE_COMMAND_UI(ID_PARM_HARDWARE, OnUpdateHardwareButton)
	ON_COMMAND(ID_PARM_HARDWARE_ADSTWINCAT, OnConfigurationAdstwincatsettings)
	ON_UPDATE_COMMAND_UI(ID_PARM_HARDWARE_ADSTWINCAT, OnUpdateConfigurationAdstwincatsettings)
	ON_UPDATE_COMMAND_UI(ID_ONLINEMODE_OPTIONS, OnUpdateOnlinemodeOptions)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SETTINGS, OnUpdateToolsSettings)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_USER, OnUpdateToolsUser)
	ON_COMMAND(ID_SETTINGS_USERS, OnSettingsUsers)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS_USERS, OnUpdateSettingsUsers)
	ON_COMMAND(ID_SETTINGS_ACCESSRIGHTS, OnSettingsAccessRights)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS_ACCESSRIGHTS, OnUpdateSettingsAccessRights)
	ON_COMMAND(ID_USER_LOGIN, OnUserLogIn)
	ON_UPDATE_COMMAND_UI(ID_USER_LOGIN, OnUpdateUserLogIn)
	ON_COMMAND(ID_USER_LOGOUT, OnUserLogOut)
	ON_UPDATE_COMMAND_UI(ID_USER_LOGOUT, OnUpdateUserLogOut)
	ON_COMMAND(ID_USER_CHANGEPASSWORD, OnUserChangePassword)
	ON_UPDATE_COMMAND_UI(ID_USER_CHANGEPASSWORD, OnUpdateUserChangePassword)
	ON_UPDATE_COMMAND_UI(ID_USER_BUTTON, OnUpdateUserButton)
	ON_COMMAND(ID_USER_BUTTON, OnUserButton)
	ON_COMMAND(ID_VISIONSTATUS_BUTTON, OnVisionstatusButton)
	ON_UPDATE_COMMAND_UI(ID_VISIONSTATUS_BUTTON, OnUpdateVisionstatusButton)
	ON_COMMAND(ID_PARM_HARDEARE_IO, OnHardwareIO)
	ON_UPDATE_COMMAND_UI(ID_PARM_HARDEARE_IO, OnUpdateHardwareIO)
	ON_COMMAND(ID_ONLINE_BYPASSRESULT, OnOnlineBypassResult)
	ON_UPDATE_COMMAND_UI(ID_ONLINE_BYPASSRESULT, OnUpdateOnlineBypassResult)
	ON_COMMAND(ID_HARDWARE_SYNCHRONOUS_GRAB, OnCameraSynchronousGrab)
	ON_UPDATE_COMMAND_UI(ID_HARDWARE_SYNCHRONOUS_GRAB, OnUpdateCameraSynchronousGrab)
	ON_COMMAND(ID_VIEWOPTIONS_AUDITTRAIL, OnViewOptionsAuditTrail)
	ON_UPDATE_COMMAND_UI(ID_VIEWOPTIONS_AUDITTRAIL, OnUpdateViewOptionsAuditTrail)
	ON_COMMAND(ID_VIEW_ZOOM_ALL, OnViewZoomAll)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM_ALL, OnUpdateViewZoomAll)
	ON_COMMAND(ID_OPEN_LOT, OnOpenLot)
	ON_UPDATE_COMMAND_UI(ID_OPEN_LOT, OnUpdateOpenLot)
	ON_COMMAND(ID_CLOSE_LOT, OnCloseLot)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_LOT, OnUpdateCloseLot)
//	ON_COMMAND(ID_CAMERA_STREAM, &CApplication::OnCameraStream)
//	ON_UPDATE_COMMAND_UI(ID_CAMERA_STREAM, &CApplication::OnUpdateCameraStream)

	ON_COMMAND(ID_INSPECTION_LOOP, OnInspectionLoop) //JP
	ON_UPDATE_COMMAND_UI(ID_INSPECTION_LOOP, OnUpdateInspectionLoop)

	ON_COMMAND(ID_DEFAULT_SETTING, OnDefaultSetting) //JP
	ON_UPDATE_COMMAND_UI(ID_DEFAULT_SETTING, OnUpdateDefaultSetting)

	ON_COMMAND(ID_TOOLS_TEACH_INSP_LOC_BUTTON, OnParmOptionsTeachInspAndLoc)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_TEACH_INSP_LOC_BUTTON, OnUpdateParmOptionsTeachInspAndLoc)
	ON_COMMAND(ID_DEVICE_CONFIGURATION_BUTTON, OnParmOptionsDeviceConfiguration)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_CONFIGURATION_BUTTON, OnUpdateParmOptionsDeviceConfiguration)
	ON_COMMAND(ID_DEFECT_PRIPRITY_SEL_BUTTON, OnParmOptionsDefectPrioritySelection)
	ON_UPDATE_COMMAND_UI(ID_DEFECT_PRIPRITY_SEL_BUTTON, OnUpdateParmOptionsDefectPrioritySelection)
	
	ON_COMMAND(ID_CONFIG_MOTOR, OnMotionControl)
	ON_UPDATE_COMMAND_UI(ID_CONFIG_MOTOR, OnUpdateMotionControl)
	ON_UPDATE_COMMAND_UI(ID_INITIALIZING_BUTTON, OnUpdateMotorInitializing)

	/*ON_COMMAND(ID_STATIC_LOAD_MULTIIMG_FROMFILE, OnToolsStaticimageLoadMultiImgFromFile)
	ON_UPDATE_COMMAND_UI(ID_STATIC_LOAD_MULTIIMG_FROMFILE, OnUpdateToolsStaticimageLoadMultiImgFromFile)*/

	ON_COMMAND(ID_INSP_SEQ_BUTTON, OnInspInspectionSequence)
	ON_UPDATE_COMMAND_UI(ID_INSP_SEQ_BUTTON, OnUpdateInspInspectionSequence)

	ON_COMMAND(ID_INSP_SEQ_AUTO_FOCUSING, OnInspAutoFocusingSequence)
	ON_UPDATE_COMMAND_UI(ID_INSP_SEQ_AUTO_FOCUSING, OnUpdateInspAutoFocusingSequence)

	ON_COMMAND(ID_INSP_SAVE_SEQ_IMAGES, OnInspSaveSequenceImages)
	ON_UPDATE_COMMAND_UI(ID_INSP_SAVE_SEQ_IMAGES, OnUpdateInspSaveSequenceImages)


	
	ON_COMMAND(ID_INSP_SEQ_LOOP, OnInspSequenceLoop)
	ON_UPDATE_COMMAND_UI(ID_INSP_SEQ_LOOP, OnUpdateInspSequenceLoop)

	/*ON_COMMAND(ID_CAMERA_SNAP_ALL_BUTTON, OnCameraSnapAll)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_SNAP_ALL_BUTTON, OnUpdateCameraSnapAll)*/

	ON_COMMAND(ID_PIXEL_RULER, OnPixelRuler)
	ON_UPDATE_COMMAND_UI(ID_PIXEL_RULER, OnUpdatePixelRuler)
	//ON_COMMAND(ID_DRY_RUN_MACHINE, OnToolsDryRun)
	//ON_UPDATE_COMMAND_UI(ID_DRY_RUN_MACHINE, OnUpdateToolsDryRun)

		ON_COMMAND(ID_BUTTON_DEFECT_SELECTION, &CApplication::OnButtonDefectSelection)
		ON_UPDATE_COMMAND_UI(ID_BUTTON_DEFECT_SELECTION, &CApplication::OnUpdateButtonDefectSelection)
		/*ON_COMMAND(ID_BUTTON_FILE_TRANSFER_SETTINGS, &CApplication::OnButtonFileTransferSettings)
		ON_UPDATE_COMMAND_UI(ID_BUTTON_FILE_TRANSFER_SETTINGS, &CApplication::OnUpdateButtonFileTransferSettings)*/
		ON_COMMAND(ID_BUTTON_CENTRALIZED_PC, &CApplication::OnButtonCentralizedPc)
		ON_UPDATE_COMMAND_UI(ID_BUTTON_CENTRALIZED_PC, &CApplication::OnUpdateButtonCentralizedPc)
		ON_COMMAND(ID_CHECK_ENABLE_CENTRALIZED_VERIFICATION, &CApplication::OnCheckEnableCentralizedVerification)
		ON_UPDATE_COMMAND_UI(ID_CHECK_ENABLE_CENTRALIZED_VERIFICATION, &CApplication::OnUpdateCheckEnableCentralizedVerification)
		ON_COMMAND(ID_BUTTON_CENTRALIZED_MONITOR, &CApplication::OnButtonCentralizedMonitor)
		ON_UPDATE_COMMAND_UI(ID_BUTTON_CENTRALIZED_MONITOR, &CApplication::OnUpdateButtonCentralizedMonitor)

		ON_UPDATE_COMMAND_UI(ID_COMBO_DL_MODE, &CApplication::OnUpdateDLMode)
		ON_COMMAND(ID_COMBO_DL_MODE, &CApplication::OnCbnSelchangeComboDLMode)

		END_MESSAGE_MAP()
 

int OutPortSendString(int nController, CString* pstrData);


// CApplication construction

CApplication::CApplication()
{

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	//SetAppID(_T("EmageVision Pte Ltd.Ev_InCuvette.AppID.NoVersion"));
	SetAppID(_T("Emage Equipment Pte Ltd.FCI_4X.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	for(int i=0; i<MAX_VIEWS; i++)
		m_pFrames[i] = NULL;

	m_pTracks = NULL;
	m_nCamLibraryTrackIndex = -1;

	m_bImagesViewInitialised = FALSE;
	m_nImagesViewCX = 0;
	m_nImagesViewCY = 0;
	bIsLeftButtonClicked = FALSE;

	for(int i=0;i<THREAD_COUNT;i++)
		m_bStepSelection[i]=TRUE;

	nTop =0;
	nBottom =0;
	nSide = 0;
	m_bEmergencyStop = FALSE;
	m_bCurtainSensor = FALSE;
	m_bReceivedStopCmd = FALSE;

	for(int i=0;i<5;i++){
		CString strTemp;
		strTemp.Format("---Part Mid%d---",i+1);
		m_strPartFlex[i]=strTemp;
		m_nPartErrorCode[i] = 98765;
		m_nDieErrorCodeStats[i] = -98765;
		m_bPartInspect[i]  = TRUE;
	}

	m_bIsFailPart = FALSE;
	m_bSaveInspSeqImages = FALSE;
	m_bInspSeqLoop = FALSE;
	m_nSequenceCounter = 0;
	m_bBypassCurtainSensor = FALSE;
	m_bReceivedStartCmd = FALSE;
	m_bPixelRuler = FALSE;
	m_bReceivedResetCmd = FALSE;
	m_bReceivedResetCmdPrevStatus = m_bReceivedResetCmd;
	//m_bDryRunMachine = FALSE;

	m_nVerificationMode = 0;
	for (int nDie = 0; nDie < MAX_DIES; nDie++){
		for (int nFOV = 0; nFOV < MAX_FOV; nFOV++) {
			for (int nView = 0; nView < MAX_VIEWS; nView++) {
				m_nErrorCode[nDie][nFOV][nView] = 98765;
				
			}
			//m_colorFovDieMapping[nDie][nFOV].CreateSolidBrush(RGB(125, 125, 120));;
			m_bDieInspection[nDie][nFOV] = FALSE;
			m_nFovPartErrorCode[nDie][nFOV] = 98765;
		}
	}
	
	m_bFwdDirectionForTiltInsp = TRUE;
	m_nSaveAllDieIndex = -1;
	m_bSetForAllTracks = FALSE;
	m_bSetForSingleFov = FALSE;
	m_nInSpectionDelay = 0;
	m_bInspectionAll = FALSE;
	m_bProgressDlgOpen = FALSE;
	m_nActiveDocIdx = 0;
	m_nActiveTrackIdx = 0;
	m_bReScanSequence = FALSE;
	m_bCentralizedVerificationMode = FALSE;

	m_bMultithreadInspectionMode = FALSE;

	m_nCentralizedStationLatest = -1;

	m_pComboDLMode = NULL;
}

// The one and only CApplication object

CApplication theApp;


class CCustomCommandLineInfo : public CCommandLineInfo
{
public:
	int m_nArg[3];
	int m_nIndex;

	CCustomCommandLineInfo()
	{
		m_nArg[0] = -1;
		m_nArg[1] = 0;
		m_nArg[2] = 0;
		m_nIndex = 0;
	}

	virtual void ParseParam(const char* pszParam, BOOL bFlag, BOOL bLast)
	{
		sscanf_s(pszParam, "%d", &m_nArg[m_nIndex++]);
	}
};

// CApplication initialization
int CApplication::LoadDefaults(int nMaster, int nTrackCount)
{
	CString strSec;
	if(nMaster == -1) {
		strSec = "Application (0-Master; 1-Server; 2-Ask)";
		nMaster = GetRegistryEntry(READ_REGPATH + "Preferences", strSec, -1);
		if(nMaster == -1) {
			PutRegistryEntry(WRITE_REGPATH + "Preferences", strSec, 0);
			nMaster = 0;
		}
	}

	if(nMaster==2) {
		int nResult = AfxMessageBox("Open as MASTER Application?", MB_YESNOCANCEL|MB_ICONINFORMATION);
		if(nResult == IDYES)
			m_bIsMasterApp = TRUE;
		else if(nResult == IDCANCEL)
			return -1;
	}
	else
		m_bIsMasterApp = (nMaster==0);


	m_nAppInstanceID = 0;
	BOOL bIsValidRunningApp = FALSE;
	if (m_bIsMasterApp) {
		if (IsSingleInstance("EmageEquipment_FCI4X"))
			bIsValidRunningApp = TRUE;
	}
	else {
		for (int iPort = 0; iPort < MAX_CLIENT_MONITORING; iPort++) {
			CString strAppIDInstance;
			strAppIDInstance.Format("EmageEquipment_FCI4X_%d", iPort);
			if (IsSingleInstance(strAppIDInstance)) {
				bIsValidRunningApp = TRUE;
				m_nAppInstanceID = iPort;
				break;
			}
		}
	}
	if (!bIsValidRunningApp)
		return -1;
	

	//if(m_bIsMasterApp) {
	//	CString str, str1 = "";
	//	str1 = GetRegistryEntry(READ_REGPATH + "Preferences", "Application Path (Server)", str1);
	//	if(str1.IsEmpty()) {
	//		str1 = "D:\\";
	//		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Application Path (Server)", str1);
	//	}

	//	str.Format("%s\\Ev_DryLens.exe 1", str1);
	//	if(!LaunchProcess(str))
	//		OutputDebugLogTo(0, TRUE, "Failed to launch server application (%d)", GetLastError());
	//	else
	//		Sleep(1000);
	//}
	//else {
	//	if(!IsSingleInstance("Ev_DryLens Server"))
	//		return -1;
	//}
	
	int nValue = GetRegistryEntry(READ_REGPATH + "Preferences", "Camera (0-Controller; 1-Monitor; 2-Ask)", -1);
	if(nValue == -1) {
		nValue = 0;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Camera (0-Controller; 1-Monitor; 2-Ask)", nValue);
	}
	if(nValue==2) {
		int nResult = AfxMessageBox("Open as MASTER Camera Controller?", MB_YESNOCANCEL|MB_ICONINFORMATION);
		if(nResult == IDNO)
			m_bIsCameraMonitorMode = TRUE;
		else if(nResult == IDCANCEL)
			return -1;
	}
	else
		m_bIsCameraMonitorMode = (nValue==1);

	if(nTrackCount)
		m_nTotalTracks = nTrackCount;
	else
		m_nTotalTracks = GetRegistryEntry(READ_REGPATH + "Preferences", "Number of Tracks", -1);
	if((m_nTotalTracks < 1) || (m_nTotalTracks >  3/*MAX_TRACKS*/)) {
		m_nTotalTracks = 3;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Number of Tracks", m_nTotalTracks);
	}
	m_pTracks = new CTrackHandler[m_nTotalTracks];

	m_nTop = GetRegistryEntry(READ_REGPATH + "Preferences", "Number of Top Intensity", -1);
	if((m_nTop < 1) || (m_nTop >  5)) {
		m_nTop = 5;//4;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Number of Top Intensity", m_nTop);
	}
	m_nTotalViewDocs[0] = m_nTop;

	m_nBottom = GetRegistryEntry(READ_REGPATH + "Preferences", "Number of Bottom Intensity", -1);
	if((m_nBottom < 1) || (m_nBottom >  5)) {
		m_nBottom = 5;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Number of Bottom Intensity", m_nBottom);
	}
	if (m_nTotalTracks <= 1) {
		m_nBottom = 0;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Number of Bottom Intensity", m_nBottom);
	}
		
	m_nTotalViewDocs[1] = m_nBottom;

	m_nSide = GetRegistryEntry(READ_REGPATH + "Preferences", "Number of Side Intensity", -1);
	if((m_nSide < 0) || (m_nSide >  2)) {
		m_nSide = 2;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Number of Side Intensity", m_nSide);
	}
	if (m_nTotalTracks <= 2) {
		m_nSide = 0;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Number of Side Intensity", m_nSide);
	}
	m_nTotalViewDocs[2] = m_nSide;

	m_bEnableGroupingDocView = GetRegistryEntry(READ_REGPATH + "Preferences", "Enable Grouping Doc View", -1);
	if (m_bEnableGroupingDocView == -1) {
		m_bEnableGroupingDocView = 1;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Enable Grouping Doc View", m_bEnableGroupingDocView);
	}
	m_nTotalViews = 0;
	for (int i = 0; i < m_nTotalTracks; i++) {
		m_pTracks[i].m_nTotalDocs = m_nTotalViewDocs[i];
		m_nTotalViews += m_nTotalViewDocs[i];
	}

	//m_nTotalViews = (m_nTop+m_nBottom+m_nSide);

	//m_nNoOfDies = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Number of Dies", -1);
	//if((m_nNoOfDies < 1) || (m_nNoOfDies >  MAX_DIES)) {
	//	m_nNoOfDies = 5;//4;
	//	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Number of Dies", m_nNoOfDies);
	//}
	//m_nNoOfFovX = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Number of FOV X", -1);
	//if ((m_nNoOfFovX < 1)/* || (m_nNoOfFovX >  MAX_FOV*/) {
	//	m_nNoOfFovX = 4;//4;
	//	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Number of FOV X", m_nNoOfFovX);
	//}

	//m_nNoOfFovY = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Number of FOV Y", -1);
	//if ((m_nNoOfFovY < 1)/* || (m_nNoOfFovX >  MAX_FOV*/) {
	//m_nNoOfFovY = 1;//4;
	//PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Number of FOV Y", m_nNoOfFovY);
	//}

	//m_nNoOfFov = m_nNoOfFovX * m_nNoOfFovY;
	// GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Number of FOV", -1);
	//if((m_nNoOfFov < 1) || (m_nNoOfFov >  MAX_FOV)) {
	//	m_nNoOfFov = 8;//4;
	//	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Number of FOV", m_nNoOfFov);
	//}
	//int nBlockX, nBlockY;
	//motionParameters.GetMappingParameters(nBlockX, nBlockY, m_nNoOfFovX, m_nNoOfFovY);

	//m_nNoOfFov = m_nNoOfFovX * m_nNoOfFovY;

	m_nNo_of_Defects = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Number of Defects", -1);
	if (m_nNo_of_Defects < 1) {
		m_nNo_of_Defects = 100;//4;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Number of Defects", m_nNo_of_Defects);
	}

	m_nFOVHeight = 8;
	m_nFOVHeight = GetRegistryEntry(READ_REGPATH + "Preferences", "FOV Height (mm)", m_nFOVHeight);
	PutRegistryEntry(WRITE_REGPATH + "Preferences", "FOV Height (mm)", m_nFOVHeight);

	m_nFOVWidth = 11;
	m_nFOVWidth = GetRegistryEntry(READ_REGPATH + "Preferences", "FOV Width (mm)", m_nFOVWidth);
	PutRegistryEntry(WRITE_REGPATH + "Preferences", "FOV Width (mm)", m_nFOVWidth);

	m_nMaxLineOutputLog = GetRegistryEntry(READ_REGPATH + "Preferences", "MaxLines OutPutLog", -1);
	if (m_nMaxLineOutputLog < 1) {
		m_nMaxLineOutputLog = 1000;//4;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "MaxLines OutPutLog", m_nMaxLineOutputLog);
	}

	/*m_nDeviceNoPerBlockX = 4;
	m_nDeviceNoPerBlockX = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Device No Per Block X", m_nDeviceNoPerBlockX);
	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Device No Per Block X", m_nDeviceNoPerBlockX);

	m_nDeviceNoPerBlockY = 3;
	m_nDeviceNoPerBlockY = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Device No Per Block Y", m_nDeviceNoPerBlockY);
	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Device No Per Block Y", m_nDeviceNoPerBlockY);*/

	m_strSideCameraName.RemoveAll();
	m_strTopCameraName.RemoveAll();
	m_strBottomCameraName.RemoveAll();
	m_strSideCalibrationName.RemoveAll();
	m_strTopCalibrationName.RemoveAll();
	m_strBottomCalibrationName.RemoveAll();
	m_strLightChannelName.RemoveAll();


	CString strTempTName = "";
	strTempTName = GetRegistryEntry(READ_REGPATH + "Preferences", "Top Calibration Name", strTempTName);
	if (strTempTName.IsEmpty()) {
		strTempTName = "Top Die;Top EnCap;Top Slot;T4;T5";
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Top Calibration Name", strTempTName);
	}
	CString strTemp;
	int nPos = 0;
	do
	{
		strTemp = strTempTName.Tokenize(_T(";"), nPos);
		m_strTopCalibrationName.Add(strTemp);
	} while (strTemp != "");

	strTempTName = "";
	strTempTName = GetRegistryEntry(READ_REGPATH + "Preferences", "Bottom Calibration Name", strTempTName);
	if (strTempTName.IsEmpty()) {
		strTempTName = "Bottom Die;Bottom EnCap;Bottom Slot;Bottom Cover Layer;B5";
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Bottom Calibration Name", strTempTName);
	}
	strTemp = "";
	nPos = 0;
	do
	{
		strTemp = strTempTName.Tokenize(_T(";"), nPos);
		m_strBottomCalibrationName.Add(strTemp);
	} while (strTemp != "");

	strTempTName = "";
	strTempTName = GetRegistryEntry(READ_REGPATH + "Preferences", "Side Calibration Name", strTempTName);
	if (strTempTName.IsEmpty()) {
		strTempTName = "Side Left Tilt;Side Right Tilt";
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Side Calibration Name", strTempTName);
	}
	strTemp = "";
	nPos = 0;
	do
	{
		strTemp = strTempTName.Tokenize(_T(";"), nPos);
		m_strSideCalibrationName.Add(strTemp);
	} while (strTemp != "");


	strTempTName = "";
	strTempTName = GetRegistryEntry(READ_REGPATH + "Preferences", "Top Camera Name",strTempTName);
	if(strTempTName.IsEmpty()) {
		strTempTName = "Top Die;Top EnCap;Top Slot;T4;T5";
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Top Camera Name", strTempTName);
	}
	strTemp = "";
	nPos = 0;
	 do
	{
		strTemp = strTempTName.Tokenize(_T(";"), nPos);
		m_strTopCameraName.Add(strTemp);
	}while (strTemp != "");

	strTempTName = "";
	strTempTName = GetRegistryEntry(READ_REGPATH + "Preferences", "Bottom Camera Name",strTempTName);
	if(strTempTName.IsEmpty()) {
		strTempTName = "Bottom Die;Bottom EnCap;Bottom Slot;Bottom Cover Layer;B5";
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Bottom Camera Name", strTempTName);
	}
	strTemp = "";
	nPos = 0;
	 do
	{
		strTemp = strTempTName.Tokenize(_T(";"), nPos);
		m_strBottomCameraName.Add(strTemp);
	}while (strTemp != "");

	strTempTName = "";
	strTempTName = GetRegistryEntry(READ_REGPATH + "Preferences", "Side Camera Name",strTempTName);
	if(strTempTName.IsEmpty()) {
		strTempTName = "Side Left Tilt;Side Right Tilt";
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Side Camera Name", strTempTName);
	}
	strTemp = "";
	nPos = 0;
	 do
	{
		strTemp = strTempTName.Tokenize(_T(";"), nPos);
		m_strSideCameraName.Add(strTemp);
	}while (strTemp != "");


	strTempTName = "";
	strTempTName = GetRegistryEntry(READ_REGPATH + "Light", "Light Channel Name",strTempTName);
	if(strTempTName.IsEmpty()) {
		strTempTName = "T1;T2;T3;T4;T5;B1;B2;B3;B4;B5;S1;S2";
		PutRegistryEntry(WRITE_REGPATH + "Light", "Light Channel Name", strTempTName);
	}
	strTemp = "";
	nPos = 0;
	 do
	{
		strTemp = strTempTName.Tokenize(_T(";"), nPos);
		m_strLightChannelName.Add(strTemp);
	}while (strTemp != "");


	nValue = GetRegistryEntry(READ_REGPATH + "Preferences", "Active Tracks", -1);
	if(nValue == -1) {
		m_cActiveTracks = 0x1f;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Active Tracks", m_cActiveTracks);
	}
	else
		m_cActiveTracks = nValue & 0xff;

	m_bLotOpened = GetRegistryEntry(READ_REGPATH + "Preferences", "Lot Opened", -1);
	if (m_bLotOpened==-1) {
		m_bLotOpened = FALSE;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Lot Opened", m_bLotOpened);
	}

	if (m_bLotOpened) {
		m_strLotInfo = "void";
		m_strLotInfo = GetRegistryEntry(READ_REGPATH + "Preferences", "Lot Info", m_strLotInfo);
		if (m_strLotInfo.CompareNoCase("void") == 0) {
			m_strLotInfo = " ";
			PutRegistryEntry(WRITE_REGPATH + "Preferences", "Lot Info", m_strLotInfo);
		}
		m_strLotConfigFile = "void";
		m_strLotConfigFile = GetRegistryEntry(READ_REGPATH + "Preferences", "Lot Info Receipe", m_strLotConfigFile);
		if (m_strLotConfigFile.CompareNoCase("void") == 0) {
			m_strLotConfigFile = " ";
			PutRegistryEntry(WRITE_REGPATH + "Preferences", "Lot Info Receipe", m_strLotConfigFile);
		}
	}
	else
	{
		m_strLotInfo = "";
	}


	m_strMainCfgDir = GetRegistryEntry(READ_REGPATH + "Preferences", "Folder: Recipe", "void");
	if (m_strMainCfgDir.CompareNoCase("void")==0) {
		m_strMainCfgDir = "C:\\FCI Configs";
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Folder: Recipe", m_strMainCfgDir);
	}
	_mkdir(m_strMainCfgDir);
	CString strTempConfig = m_strMainCfgDir +"\\Engineering Config";
	_mkdir(strTempConfig);

	strTempConfig = m_strMainCfgDir +"\\Production Config";
	_mkdir(strTempConfig);

	m_strMainCfgModeDir = "void";
	m_strMainCfgModeDir = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Folder:Config Mode", "void");
	if (m_strMainCfgModeDir.CompareNoCase("void")==0) {
		m_strMainCfgModeDir = "Engineering Config";
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Folder:Config Mode", m_strMainCfgModeDir);
	}
	

	m_strConfigFile = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Recipe", "");
	if(m_strConfigFile.IsEmpty()){
		m_strConfigFile = "Default";
		PutRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Recipe", m_strConfigFile);
	}

	m_strCalibDir.Format("%s\\Calibration", m_strMainCfgDir);
	/*m_strCalibDir = "";
	m_strCalibDir = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Calibration", m_strCalibDir);
	if (m_strCalibDir.IsEmpty()) {
		
		PutRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Calibration", m_strCalibDir);
	}
	_mkdir(m_strCalibDir);*/

	m_strSystemLoggingDir = GetRegistryEntry(READ_REGPATH + "Preferences", "Folder: SystemLog", "");
	if(m_strSystemLoggingDir.IsEmpty()) {
		m_strSystemLoggingDir="C:\\SystemLog";
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Folder: SystemLog", m_strSystemLoggingDir);
	}
	_mkdir(m_strSystemLoggingDir); 

	m_strAuditLoggingDir = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Folder: AuditTrail", "");
	if(m_strAuditLoggingDir.IsEmpty()) {
		m_strAuditLoggingDir="C:\\AuditTrailLog";
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Folder: AuditTrail", m_strAuditLoggingDir);
	}
	_mkdir(m_strAuditLoggingDir); 

	m_LotSummaryRootDir = GetRegistryEntry(READ_REGPATH + "Preferences", "Folder: Statistics", "");
	if (m_LotSummaryRootDir.IsEmpty()) {
		m_LotSummaryRootDir = "C:\\FCI Stats";
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Folder: Statistics", m_LotSummaryRootDir);
	}
	_mkdir(m_LotSummaryRootDir);

	m_strParmFolder.Format("%s\\ParmDatabase", m_LotSummaryRootDir);
	_mkdir(m_strParmFolder);

	m_ImageSaveFolder = "void";
	m_ImageSaveFolder = GetRegistryEntry(READ_REGPATH + "Preferences", "Folder: Image Save", m_ImageSaveFolder);
	if (m_ImageSaveFolder.CompareNoCase("void")==0) {
		m_ImageSaveFolder = "C:\\FCI Images";
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Folder: Image Save", m_ImageSaveFolder);
	}
	_mkdir(m_ImageSaveFolder);

	m_ImageSaveFolderDL = "void";
	m_ImageSaveFolderDL = GetRegistryEntry(READ_REGPATH + "Preferences", "Folder:DL Image Save", m_ImageSaveFolderDL);
	if (m_ImageSaveFolderDL.CompareNoCase("void") == 0) {
		m_ImageSaveFolderDL = "C:\\DL Images";
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Folder:DL Image Save", m_ImageSaveFolderDL);
	}
	_mkdir(m_ImageSaveFolderDL);

	m_nSequenceCounter = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "InspSequenceCounter", -1);
	if(m_nSequenceCounter==-1) {
		m_nSequenceCounter = 1;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "InspSequenceCounter", m_nSequenceCounter);
	}

	m_bSaveInspSeqImages = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "SaveInspectSeqImages", -1);
	if(m_bSaveInspSeqImages==-1) {
		m_bSaveInspSeqImages = FALSE;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "SaveInspectSeqImages", m_bSaveInspSeqImages);
	}

	m_bSaveOnlineFailImages = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "SaveFailImages", -1);
	if(m_bSaveOnlineFailImages == -1) {
		m_bSaveOnlineFailImages = TRUE;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "SaveFailImages", m_bSaveOnlineFailImages);
	}
	m_bSaveOnlineJpegImages = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "SaveJpegImages", -1);
	if(m_bSaveOnlineJpegImages == -1) {
		m_bSaveOnlineJpegImages = FALSE;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "SaveJpegImages", m_bSaveOnlineJpegImages);
	}

	m_bSaveOnlineBMPImages = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "SaveBMPImages", -1);
	if (m_bSaveOnlineBMPImages == -1) {
		m_bSaveOnlineBMPImages = FALSE;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "SaveBMPImages", m_bSaveOnlineBMPImages);
	}

	m_bSaveOnlinePassImages = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "SavePassImages", -1);
	if(m_bSaveOnlinePassImages==-1) {
		m_bSaveOnlinePassImages = TRUE;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "SavePassImages", m_bSaveOnlinePassImages);
	}

	m_bEnableDLModelInspect = GetRegistryEntry(READ_REGPATH + "Deep Learning Engine", "Enable DL Model Inspect", -1);
	if (m_bEnableDLModelInspect == -1) {
		m_bEnableDLModelInspect = FALSE;
		PutRegistryEntry(WRITE_REGPATH + "Deep Learning Engine", "Enable DL Model Inspect", m_bEnableDLModelInspect);
	}

	m_bUseDLModelResult = GetRegistryEntry(READ_REGPATH + "Deep Learning Engine", "Use DL Model Result For Final Inspection", -1);
	if (m_bUseDLModelResult == -1) {
		m_bUseDLModelResult = FALSE;
		PutRegistryEntry(WRITE_REGPATH + "Deep Learning Engine", "Use DL Model Result For Final Inspection", m_bUseDLModelResult);
	}

	int nDLCropOffsetSize = GetRegistryEntry(READ_REGPATH + "Deep Learning Engine", "Offset Size To Crop Image For DL Inspection", -1);
	if (nDLCropOffsetSize == -1) {
		nDLCropOffsetSize = 10;
		PutRegistryEntry(WRITE_REGPATH + "Deep Learning Engine", "Offset Size To Crop Image For DL Inspection", nDLCropOffsetSize);
	}

	BOOL bForceToUseDLCropOffsetSize = GetRegistryEntry(READ_REGPATH + "Deep Learning Engine", "Force To Use Offset Size To Crop Image For DL Inspection", -1);
	if (bForceToUseDLCropOffsetSize == -1) {
		bForceToUseDLCropOffsetSize = FALSE;
		PutRegistryEntry(WRITE_REGPATH + "Deep Learning Engine", "Force To Use Offset Size To Crop Image For DL Inspection", bForceToUseDLCropOffsetSize);
	}

	BOOL bDLModelInspectFromFile = GetRegistryEntry(READ_REGPATH + "Deep Learning Engine", "DL Model Inspect From File", -1);
	if (bDLModelInspectFromFile == -1) {
		bDLModelInspectFromFile = FALSE;
		PutRegistryEntry(WRITE_REGPATH + "Deep Learning Engine", "DL Model Inspect From File", bDLModelInspectFromFile);
	}

	m_DeepLearningModule.m_nDeepLearningMode = GetRegistryEntry(READ_REGPATH + "Deep Learning Engine", "Deep Learning Mode", -1);
	if (m_DeepLearningModule.m_nDeepLearningMode == -1 || m_DeepLearningModule.m_nDeepLearningMode >= 2) {
		m_DeepLearningModule.m_nDeepLearningMode = 0;
		PutRegistryEntry(WRITE_REGPATH + "Deep Learning Engine", "Deep Learning Mode", m_DeepLearningModule.m_nDeepLearningMode);
	}

	m_bSaveDLImages = GetRegistryEntry(READ_REGPATH + "Preferences", "Save Deep Learning Images", -1);
	if (m_bSaveDLImages == -1) {
		m_bSaveDLImages = FALSE;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Save Deep Learning Images", m_bSaveDLImages);
	}

	m_bSaveDLPassImages = GetRegistryEntry(READ_REGPATH + "Preferences", "Save Deep Learning Pass Images", -1);
	if (m_bSaveDLPassImages == -1) {
		m_bSaveDLPassImages = FALSE;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Save Deep Learning Pass Images", m_bSaveDLPassImages);
	}

	m_bSaveDLFailImages = GetRegistryEntry(READ_REGPATH + "Preferences", "Save Deep Learning Fail Images", -1);
	if (m_bSaveDLFailImages == -1) {
		m_bSaveDLFailImages = TRUE;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Save Deep Learning Fail Images", m_bSaveDLFailImages);
	}

	m_nSaveJpegQualityImages = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "SaveJpegQuality", -1);
	if (m_nSaveJpegQualityImages == -1) {
		m_nSaveJpegQualityImages = 50;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "SaveJpegQuality", m_nSaveJpegQualityImages);
	}

	m_bOnlineContinueTopImageInsp = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "ContinueTopImageInsp", -1);
	if(m_bOnlineContinueTopImageInsp == -1) {
		m_bOnlineContinueTopImageInsp = FALSE;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "ContinueTopImageInsp", m_bOnlineContinueTopImageInsp);
	}

	m_nMinimumDiskSpaceImageSaveGB = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Minimum Disk Space For Image Saving (Gigabyte)", -1);
	if (m_nMinimumDiskSpaceImageSaveGB == -1) {
		m_nMinimumDiskSpaceImageSaveGB = 200;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Minimum Disk Space For Image Saving (Gigabyte)", m_nMinimumDiskSpaceImageSaveGB);
	}

	m_bAutoFocusingSequence = GetRegistryEntry(READ_REGPATH + "Preferences", "Auto Focusing Sequence", -1);
	if (m_bAutoFocusingSequence == -1) {
		m_bAutoFocusingSequence = FALSE;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Auto Focusing Sequence", m_bAutoFocusingSequence);
	}

	if(m_bSaveOnlinePassImages || m_bSaveOnlineFailImages || m_bSaveOnlineJpegImages) {
		if(!CheckDiskFreeSpace()) {
			m_bSaveOnlinePassImages = FALSE;
			m_bSaveOnlineFailImages = FALSE;
			m_bSaveOnlineJpegImages = FALSE;
		}
	}

	//Deep Learning Image Saving
	if (m_bSaveDLImages || m_bSaveDLPassImages || m_bSaveDLFailImages) {
		if (!CheckDLDiskFreeSpace()) {
			m_bSaveDLImages = FALSE;
			m_bSaveDLPassImages = FALSE;
			m_bSaveDLFailImages = FALSE;
		}
	}

	m_bInspectionParallelism = GetRegistryEntry(READ_REGPATH + "Preferences", "Inspection Parallelism: Enable", -1);
	if(m_bInspectionParallelism == -1) {
		m_bInspectionParallelism = 1;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Inspection Parallelism: Enable", m_bInspectionParallelism);
	}

	CString str1, str2 = "void";
	str1 = GetRegistryEntry(READ_REGPATH + "Preferences", "Stats: Display Reset @(hhmm)", str1);
	if (str1.CompareNoCase("void")==0) {
		str1 = "0600";
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Stats: Display Reset @(hhmm)", str1);
	}
	int nMinutes = __min(atoi(str1.Right(2).GetBuffer(0)), 59);
	int nHours = __min(atoi(str1.Mid(0, str1.GetLength()-2).GetBuffer(0)), 23);
	str2.Format("%02u%02u", nHours, nMinutes);
	if(str1 != str2)
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Stats: Display Reset @(hhmm)", str2);

	m_LotSummary.nDisplayStatsResetMin = nHours*60 + nMinutes;

	nValue = GetRegistryEntry(READ_REGPATH + "Preferences", "Stats: Recycle Lot After (days)", -1);
	if(nValue==-1) {
		nValue = 30;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Stats: Recycle Lot After (days)", nValue);
	}
	m_LotSummary.nLotRecycleDays = nValue;

	m_UserAccInfo.m_nLogoutTime = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "UserAutoLogoutMins", -1);
	if(m_UserAccInfo.m_nLogoutTime == -1) {
		m_UserAccInfo.m_nLogoutTime = 3;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "UserAutoLogoutMins", m_UserAccInfo.m_nLogoutTime);
	}
	
	//m_UserAccInfo.m_nLogTimeExt = 0;
	//m_UserAccInfo.m_nLogTimeExt = GetRegistryEntry(READ_REGPATH + "Preferences", "LogTimeExtInfo", m_UserAccInfo.m_nLogTimeExt);
	//PutRegistryEntry(WRITE_REGPATH + "Preferences","LogTimeExtInfo", m_UserAccInfo.m_nLogTimeExt);

	m_bEnableSimulationMode = GetRegistryEntry(READ_REGPATH + "Preferences", "SimulationMode: Enable", -1);
	if(m_bEnableSimulationMode == -1) {
		m_bEnableSimulationMode = 1;
		PutRegistryEntry(WRITE_REGPATH + "Preferences","SimulationMode: Enable", m_bEnableSimulationMode);
	}

	m_nMaxSimulationBuffers = 10;
	//m_nMaxSimulationBuffers = GetRegistryEntry(READ_REGPATH + "Preferences", "SimulationMode: MaxBuffers", m_nMaxSimulationBuffers);
	//PutRegistryEntry(WRITE_REGPATH + "Preferences", "SimulationMode: MaxBuffers", m_nMaxSimulationBuffers);

	//m_strSimulationImagesPath = "void";
	//m_strSimulationImagesPath = GetRegistryEntry(READ_REGPATH + "Preferences", "Folder: SimulationMode", m_strSimulationImagesPath);
	//if (m_strSimulationImagesPath.CompareNoCase("void")==0) {
	//	m_strSimulationImagesPath = "E:\\TestImagesOnline";
	//	PutRegistryEntry(WRITE_REGPATH + "Preferences", "Folder: SimulationMode", m_strSimulationImagesPath);
	//}

	//m_nUnitIndex = -1;
	//m_nUnitIndex = GetRegistryEntry(READ_REGPATH + "Preferences", "Unit (0-pixel; 1-micron; 2-mm)", m_nUnitIndex);
	//if(m_nUnitIndex==-1) {
	//	m_nUnitIndex = 1;
	//	PutRegistryEntry(WRITE_REGPATH + "Preferences", "Unit (0-pixel; 1-micron; 2-mm)", m_nUnitIndex);
	//}
	m_nUnitIndex = 0;
	m_nCurrentLightControllerIndex = 0;

	m_bClassPriorityRealtime = 1;
	//m_bClassPriorityRealtime = GetRegistryEntry(READ_REGPATH + "Preferences","Realtime Priority: Enable", m_bClassPriorityRealtime);
	//PutRegistryEntry(WRITE_REGPATH + "Preferences","Realtime Priority: Enable", m_bClassPriorityRealtime);

	m_nOperationMode = GetRegistryEntry(READ_REGPATH + "Preferences", "Operation Mode", -1);
	if(m_nOperationMode == -1) {
		m_nOperationMode = 1;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Operation Mode", m_nOperationMode);
	}

	m_bAllocFailedImageBuffer = GetRegistryEntry(READ_REGPATH + "Preferences", "Preserve FailedImages History", -1);
	if(m_bAllocFailedImageBuffer == -1) {
		m_bAllocFailedImageBuffer = 0;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Preserve FailedImages History", m_bAllocFailedImageBuffer);
	}

	m_bInspectionAffinity = GetRegistryEntry(READ_REGPATH + "Preferences", "Inspection Affinity: Enable", -1);
	if(m_bInspectionAffinity == -1) {
		m_bInspectionAffinity = 1;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Inspection Affinity: Enable", m_bInspectionAffinity);
	}

	m_nProductCodeSize = GetRegistryEntry(READ_REGPATH + "Preferences", "Product Code Size: ", -1);
	if(m_nProductCodeSize == -1) {
		m_nProductCodeSize = 5;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Product Code Size: ", m_nProductCodeSize);
	}

	m_nBarcodeSize = GetRegistryEntry(READ_REGPATH + "Preferences", "Barcode Size: ", -1);
	if(m_nBarcodeSize == -1) {
		m_nBarcodeSize = 17;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Barcode Size: ", m_nBarcodeSize);
	}

	m_bSynchronousGrabEnable = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "SynchronousGrab: Enable", -1);
	if(m_bSynchronousGrabEnable == -1) {
		m_bSynchronousGrabEnable = 0;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "SynchronousGrab: Enable", m_bSynchronousGrabEnable);
	}

	m_nSystemRestartCounter = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "SystemRestartCounter", 0);
	m_nSystemRestartCounter++;
	if(m_nSystemRestartCounter > 20)
		m_nSystemRestartCounter = 0;
	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "SystemRestartCounter",m_nSystemRestartCounter);

	m_bSaveCrashImages = GetRegistryEntry(READ_REGPATH + "Preferences","Save Crash Images", -1);
	if(m_bSaveCrashImages == -1) {
		m_bSaveCrashImages = FALSE;
		PutRegistryEntry(WRITE_REGPATH + "Preferences","Save Crash Images",m_bSaveCrashImages);
	}

	/////////////////////////////////////////////
	nNoOfResetPerDay = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "LotNoOfReset", -1);
	if(nNoOfResetPerDay == -1) {
		nNoOfResetPerDay = 0;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "LotNoOfReset", nNoOfResetPerDay);
	}

	nValue = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "LotResetTime1", -1);
    if(nValue == -1) {
        nValue = 0;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "LotResetTime1", nValue);
	}
    tLotResetTime1.wHour = nValue;

	nValue = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "LotResetTime2", -1);
    if(nValue == -1) {
        nValue = 0;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "LotResetTime2", nValue);
	}
    tLotResetTime2.wHour = nValue;
	
	m_strProductCode = "----NoBarcode----";
	m_strMachineID = "void";
	m_strMachineID = GetRegistryEntry(READ_REGPATH + "Preferences", "Machine ID", m_strMachineID);
	if (m_strMachineID.CompareNoCase("void")==0) {
		m_strMachineID = "FCI 1";
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Machine ID", m_strMachineID);
	}

	m_LotSummary.strMachineID = m_strMachineID;

	for(int i = 0; i < NO_OF_PORT_CENTRALIZED;i++)
		m_strClientMachineID[i] = "";

	m_bAutoResizeImage = GetRegistryEntry(READ_REGPATH + "Preferences", "Resize Images: Enable", TRUE);
	if(!m_bAutoResizeImage)
		m_bAutoResizeImageToMidRes = GetRegistryEntry(READ_REGPATH + "Preferences", "Resize Images ToMidRes: Enable", TRUE);
	m_bAutoSaveResizedImage = GetRegistryEntry(READ_REGPATH + "Preferences", "Resize Images: AutoSave", FALSE);

	m_nFirstGrabDelay = GetRegistryEntry(READ_REGPATH + "Preferences", "Sequence: First Grab Delay", -1);
	if(m_nFirstGrabDelay == -1) {
		m_nFirstGrabDelay = 200;
		PutRegistryEntry(READ_REGPATH + "Preferences", "Sequence: First Grab Delay", m_nFirstGrabDelay);
	}

	m_nSignalScanFrequency = GetRegistryEntry(READ_REGPATH + "Preferences", "Sequence: Signal Scan Frequency", -1);
	if(m_nSignalScanFrequency==-1) {
		m_nSignalScanFrequency = 10;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Sequence: Signal Scan Frequency", m_nSignalScanFrequency);
	}

	m_bStoredImagesLoop = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "StoredImagesLoop", -1);
	if(m_bStoredImagesLoop == -1) {
		m_bStoredImagesLoop = FALSE;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "StoredImagesLoop", m_bStoredImagesLoop);
	}

	m_bZoomAll = TRUE;
	m_bZoomAll = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Zoom All: Enable", 1);
	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Zoom All: Enable", m_bZoomAll);

	m_nXAxisSettlingTime = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "X-Axis Settling Time", -1);
	if(m_nXAxisSettlingTime==-1) {
		m_nXAxisSettlingTime = 1000;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "X-Axis Settling Time", m_nXAxisSettlingTime);
	}

	m_nYAxisSettlingTime = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Y-Axis Settling Time", -1);
	if(m_nYAxisSettlingTime==-1) {
		m_nYAxisSettlingTime = 1000;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Y-Axis Settling Time", m_nYAxisSettlingTime);
	}

	m_strLoadMultiImagesFolderPath = "void";
	strTemp.Format("Folder: StoredMultiImages");
	m_strLoadMultiImagesFolderPath = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", strTemp, m_strLoadMultiImagesFolderPath);
	if(m_strLoadMultiImagesFolderPath.CompareNoCase("void")==0) {
		m_strLoadMultiImagesFolderPath = "C:\\";
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", strTemp, m_strLoadMultiImagesFolderPath);
	}


	m_bBypassCurtainSensor = GetRegistryEntry(READ_REGPATH + "Preferences", "ByPass Curtain Sensor: Enable", -1);
	if(m_bBypassCurtainSensor == -1) {
		m_bBypassCurtainSensor = 0;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "ByPass Curtain Sensor: Enable", m_bBypassCurtainSensor);
	}

	/*m_bDryRunMachine = FALSE;
	m_bDryRunMachine = GetRegistryEntry(READ_REGPATH + "Preferences","Machine Dry Run: Enable", m_bDryRunMachine);
	PutRegistryEntry(WRITE_REGPATH + "Preferences", "Machine Dry Run: Enable", m_bDryRunMachine);*/

	m_bMultithreadInspectionMode = GetRegistryEntry(READ_REGPATH + "Preferences", "Enable Multi Threading Inspection", -1);
	if (m_bMultithreadInspectionMode == -1) {
		m_bMultithreadInspectionMode = 0;
		PutRegistryEntry(WRITE_REGPATH + "Preferences", "Enable Multi Threading Inspection", m_bMultithreadInspectionMode);
	}

	m_bLoadMultiImagesFromFile = FALSE;

	m_pCentralizedMonitorDlg = NULL;
	m_pDefectCode = NULL;
	m_pLightCtrlDlg = NULL;
	m_pMotionControl = NULL;
	m_pWndPVIDefectParameters = NULL;
	m_pScanBarcodeDlg = NULL;
	m_bLightCtrlDlgOpen = FALSE;
	m_bLoadComplete = FALSE;
	m_pCalibrationDlg = NULL;
	//m_pAuditTrailDlg = NULL;
	m_pDisplayMsgStatus = NULL;
	m_pInfoMsgDlg = NULL;

	m_bTeachPass = FALSE;
	m_bOnline = FALSE;
	m_bTeaching = FALSE;
	m_bInspecting = FALSE;
	m_bBarcodeScanning = FALSE;
	m_bRecipeFound = FALSE;
	m_bByPassResult = FALSE;
	m_bScanning = FALSE;

	for(int i=0;i<MAX_TRACKS;i++) {
		m_nFailImageCounter[i]=0;
		m_nCrashImageCounter[i]=0;
	}
	m_nDeviceCounter=0;
	m_RuntimeDisplayEnable=TRUE;
	m_bTrackInspSeq = FALSE; 
	m_bSuperUserMode = FALSE;
	m_nOutData = 0xff;

	m_UserAccInfo.UserAccount = NULL;
	m_UserAccInfo.strUserNames = NULL;

	m_strAppId = "EmageEquipment_FCI4X";

	m_UserAccInfo.m_strCurUserName = "Operator";	
	m_UserAccInfo.DefaultUser.strUsername = "engineer";
	CString strPswd = "MVFCI";
	m_UserAccInfo.DefaultUser.strPassword = m_UserLog.EncodePassWord(&strPswd);
	m_UserAccInfo.DefaultUser.strAccLevel = "Administrator";
	m_UserAccInfo.m_bAccessLevel = false;
	m_UserAccInfo.m_nCurrentAccessLevel = 0;

	m_LotSummaryLotIDDir = "";
	m_bStream = FALSE;
	m_bSnap = FALSE;
	m_bCalib = FALSE;
	m_pStationMapDlg = NULL;
	m_bStationMapDlgOpen = FALSE;
	m_bCalibrationOpen = FALSE;
	m_bAllowStream = FALSE;
	m_nNavigateStaticImgTrack = 0;
	m_bInspSequenceOffline = FALSE;
	m_bModifierView = FALSE;
	m_bUpdatingMapping = FALSE;

	return 0;
}

BOOL CApplication::InitInstance()
{
	///_CrtSetDbgFlag(_CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF);
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("EmageEquipment"));
	///LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	InitContextMenuManager();
	InitShellManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	CSplashScreen::ShowSplashScreen();

	// Parse command line for standard shell commands, DDE, file open
	CCustomCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if(LoadDefaults(cmdInfo.m_nArg[0], cmdInfo.m_nArg[1]))
		return FALSE;
	m_nFirstTrackNo = cmdInfo.m_nArg[2];

	/*if(!SetParameters())
		return FALSE;*/


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_EVAPPLICATIONTYPE,
		//RUNTIME_CLASS(CAppDoc),
		RUNTIME_CLASS(CEvTabDoc),
		RUNTIME_CLASS(CEvChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CAppTabView));
	if (!pDocTemplate)
		return FALSE;


	AddDocTemplate(pDocTemplate);
	
	CWinAppEx::m_bLoadWindowPlacement = FALSE;
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if(!pMainFrame)
		return FALSE;

	pMainFrame->LoadDefaults(READ_REGPATH);
	if(!pMainFrame->LoadFrame(IDR_MAINFRAME)) {
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	pMainFrame->m_strLotFolder = m_LotSummaryRootDir;
	
	/*((CMainFrame*) m_pMainWnd)->ShowStatsPane();	
	((CMainFrame*) m_pMainWnd)->ShowPreviousImages();
	((CMainFrame*) m_pMainWnd)->ShowDebugView();
*/
	if(!SetParameters())
		return FALSE;

	GetKeyboardManager()->ResetAll();
	
	//Application mode: Master --> need to check Halcon license for Inspection Module; Server: no need Inspection Module
	if (m_bIsMasterApp) {
		if (!CheckForHalconnLicense()) {
			return FALSE;
		}
	}

	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	CRect rectClient;

	GetClientRect(((CMDIFrameWndEx *)m_pMainWnd)->m_hWndMDIClient,&rectClient);
	int cx = rectClient.Width()*0.02;
	int cy = rectClient.Height() /0.9;
//////////////
	int arrx[3] = { 0, cx,  cx<<1};
	int arry[3] = { 0,  0,  0 };
	int arrcx[3] = { cx, cx, cx,};
	int arrcy[3] = { cy, cy, cy,};

//////////////
	((CMDIFrameWndEx *)m_pMainWnd)->MoveWindow(arrx[0], arry[0], arrcx[0], arrcy[0],TRUE);
	m_pFrames[0] = (CMDIChildWndEx *) ((CMDIFrameWndEx *)m_pMainWnd)->GetActiveFrame();

	for(int i=1; i<m_nTotalViews; i++) {
		if (pDocTemplate->OpenDocumentFile(NULL)==0)
			return FALSE;
		m_pFrames[i] = (CMDIChildWndEx *) ((CMDIFrameWndEx *)m_pMainWnd)->GetActiveFrame();
	
	}

	motionParameters.LoadDeviceParameters();

	InitializeDocView();
	motionParameters.LoadScanningParm();
	motionParameters.CreateScanPositions();

//////////////
	////Check app license
	//if(!CheckApplicationLicense())
	//	return FALSE;

	CMFCRibbonBar* pRibbon = &((CMainFrame*)m_pMainWnd)->m_wndRibbonBar;
	CMFCRibbonBaseElement* pElement = pRibbon->FindByID(ID_COMBO_DL_MODE, TRUE, TRUE);


	m_pComboDLMode = (CMFCRibbonComboBox*)(pElement);
	m_pComboDLMode->AddItem("Classification");
	m_pComboDLMode->AddItem("Recognition");
	m_pComboDLMode->SelectItem(m_DeepLearningModule.m_nDeepLearningMode);

	/*if (!ReloadWindowPlacement(((CMainFrame*) m_pMainWnd)))
	{
		((CMainFrame*) m_pMainWnd)->ShowWindow(m_nCmdShow);
		((CMainFrame*) m_pMainWnd)->UpdateWindow();
	}*/
	return InitProcess();
}

int CApplication::ExitInstance()
{
	m_bExitApp = TRUE;

	if(m_bOnline)
		SetVisOnLineState(FALSE);

	if (!m_bIsMasterApp) {
		motionParameters.LoadDeviceParameters();
	}

	//((CStatisticsView*)m_pFrames[2]->GetActiveView())->RecordLotStats(TRUE);
	SetEvent(m_eventOnline);
	SetEvent(m_EventMotionSensor); //Close Motion Thread
	SetEvent(m_EventMotionSensorStart);//Close Motion Thread
	SetEvent(m_EventRefreshLight);//// close thread
	SetEvent(m_EventCommSendMsg);//Close Send Comm Thread;
	if(WaitForSingleObject(m_hInspectSeqThread, 1000) != WAIT_OBJECT_0)
		TerminateThread(m_hInspectSeqThread, 0);

	SetEvent(m_hResetReceived);
	SetEvent(m_hResetDone);
	SetEvent(m_hInitPosition);
	SetEvent(m_hInitMotionDone);
	SetEvent(m_hInitPositionDone);

	for (int i = 0; i < m_nTotalTracks; i++) {
		std::for_each(m_pTracks[i].eventDoInspection.begin(), m_pTracks[i].eventDoInspection.end(),
			[](HANDLE & handle) {SetEvent(handle); });
		
		std::for_each(m_pTracks[i].eventInspectionDone.begin(), m_pTracks[i].eventInspectionDone.end(),
			[](HANDLE & handle) {SetEvent(handle); });

	}
	if(m_nCamLibraryTrackIndex >= 0) {
		for(int i=0; i<m_nTotalTracks; i++)
//				for(int i=0; i<m_nTotalTracks; i++)
			ReleaseCamera(&m_pTracks[i].m_FGResource);
		ReleaseCameraCard(&m_pTracks[m_nCamLibraryTrackIndex].m_FGResource);
	}

	OutputDebugLogTo(0, TRUE, "SetServoOn : Set To OFF For All Axis");

	for(int i=0; i<NO_OF_AXIS; i++) {
		m_MotorResource.SetServoOn(m_nAxes[i], FALSE);
	}

	OutputDebugLogTo(0, TRUE, "OutPortWrite(0,1,1) : OFF");

	m_MotorResource.OutPortWrite(0,1,1);	//// OFF

	OutputDebugLogTo(0, TRUE, "ReleaseCard()...");

	m_MotorResource.ReleaseCard(0);
	//m_MotorResource.ReleaseIOCard(0);
	
	if(m_LightModule.hDLL) {
		m_LightModule.lpFreeIODLL(0);
		AfxFreeLibrary(m_LightModule.hDLL);
	}

	if(m_CommModule.hDLL) {
		m_CommModule.lpFreeIODLL(0);
		AfxFreeLibrary(m_CommModule.hDLL);
	}

	if(m_MotorResource.isDllOpened()) {
		OutputDebugLogTo(0, TRUE, "FreeMotionCard()...");

		motionParameters.FreeMotionCard();
//		m_MotorResource.lpFreeMotionDLL(0);
//		AfxFreeLibrary(m_MotorResource._dll);
	}

	//Deep Learning Dll
	if (m_DeepLearningModule.hDLL) {
		m_DeepLearningModule.FreeDLModel(false);
		AfxFreeLibrary(m_DeepLearningModule.hDLL);
	}

	for (int i = 0; i < NO_OF_PORT_CENTRALIZED; i++) {
		if (pErrorMsgDlg[i] != NULL) {
			delete pErrorMsgDlg[i];
			pErrorMsgDlg[i] = NULL;
		}
	}
	

	if (m_pDispOnlinePanel != NULL) {
		if (IsWindow(m_pDispOnlinePanel->m_hWnd))
			m_pDispOnlinePanel->DestroyWindow();
		delete m_pDispOnlinePanel;
		m_pDispOnlinePanel = NULL;
	}

	delete m_pImageQueue;
	
	DeleteCriticalSection(&m_CriticalSection);
	DeleteCriticalSection(&m_csOnline);
	DeleteCriticalSection(&m_csFileAccess);
	DeleteCriticalSection(&m_csOutputData);

	DeleteCriticalSection(&m_csLoadConfig);
	DeleteCriticalSection(&m_csCentralizedQueue);

	//To Save Image Counter
	//CString strTemp;
	//for(nTrack = 0;nTrack<5;nTrack++){
		//strTemp.Format("Total Inspect Count Trk%d",nTrack);
		//PutRegistryEntry(WRITE_REGPATH + "Preferences",strTemp,m_pTracks[nTrack].m_nTotalInspCount);
	//}
	
//	KillDataBase();

	if(m_UserAccInfo.UserAccount != NULL)
		delete [] m_UserAccInfo.UserAccount;
	if(m_UserAccInfo.strUserNames != NULL)
		delete [] m_UserAccInfo.strUserNames;
	m_UserAccInfo.UserAccount = NULL;
	m_UserAccInfo.strUserNames = NULL;

	if(m_pLightCtrlDlg != NULL)
		delete m_pLightCtrlDlg;

	 if(m_pScanBarcodeDlg != NULL)
		 delete m_pScanBarcodeDlg;
	 //if(m_pAuditTrailDlg != NULL)
		//delete m_pAuditTrailDlg;
	
	  if(m_pStationMapDlg != NULL) {
		 if(IsWindow(m_pStationMapDlg->m_hWnd))
			 m_pStationMapDlg->DestroyWindow();
		delete m_pStationMapDlg;
		m_pStationMapDlg = NULL;
	  }

	if(m_pMotionControl != NULL) {
		 if(IsWindow(m_pMotionControl->m_hWnd))
			 m_pMotionControl->DestroyWindow();
		delete m_pMotionControl;
		m_pMotionControl = NULL;
	 }

	if (m_pCentralizedMonitorDlg != NULL) {
		if (IsWindow(m_pCentralizedMonitorDlg->m_hWnd))
			m_pCentralizedMonitorDlg->DestroyWindow();
		delete m_pCentralizedMonitorDlg;
		m_pCentralizedMonitorDlg = NULL;
	}

	 if(m_pWndPVIDefectParameters != NULL){
		if(IsWindow(m_pWndPVIDefectParameters->m_hWnd))
			m_pWndPVIDefectParameters->DestroyWindow();
		delete m_pWndPVIDefectParameters;
		m_pWndPVIDefectParameters = NULL;
	}


	if(m_pDisplayMsgStatus != NULL) {
		 if(IsWindow(m_pDisplayMsgStatus->m_hWnd))
			 m_pDisplayMsgStatus->DestroyWindow();
		delete m_pDisplayMsgStatus;
		m_pDisplayMsgStatus = NULL;
	}
	
	if (m_pInfoMsgDlg != NULL) {
		if (IsWindow(m_pInfoMsgDlg->m_hWnd))
			m_pInfoMsgDlg->DestroyWindow();
		delete m_pInfoMsgDlg;
		m_pInfoMsgDlg = NULL;
	}

	if(m_pCalibrationDlg != NULL){
			delete m_pCalibrationDlg;
	  m_pCalibrationDlg = NULL;
	}
	/*if (m_strCommOPC.CompareNoCase("TwinCAT")==0) {
		m_pADSInterface->ClosePort();
		delete m_pADSInterface;
	}*/
	 /*if(m_pImageSelectionDlg)
		delete m_pImageSelectionDlg;*/

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);

	for (int nDie = 0; nDie < nDevicePerX * nDevicePerY; nDie++) {
		for (int nFOV = 0; nFOV < nFovPerX * nFovPerY; nFOV++) {
			m_colorFovDieMapping[nDie][nFOV].DeleteObject();
		}
	}


	if (m_pDefectCode)
		delete[] m_pDefectCode;

	if(m_pTracks)
		delete[] m_pTracks;

	//m_nStartSegmentsOfImage.RemoveAll();

	AfxOleTerm(FALSE);
	OutputDebugLog("FCI_4X Vision Application closed");

	m_UserAccInfo.m_nCurrentAccessLevel = 2;
	m_strSideCameraName.RemoveAll();
	m_strTopCameraName.RemoveAll();
	m_strBottomCameraName.RemoveAll();
	m_strLightChannelName.RemoveAll();

	

	if (!m_ParametersDefectCode.IsEmpty()) {
		for (int i = 0; i<m_ParametersDefectCode.GetCount(); i++)
			delete m_ParametersDefectCode[i];
		m_ParametersDefectCode.RemoveAll();
	}

	if (m_pProgressDlg != NULL) {
		m_pProgressDlg->DestroyWindow();
		delete m_pProgressDlg;
		m_pProgressDlg = NULL;
	}

	/*if (m_pCentralizedFTPSettings != NULL) {
		m_pCentralizedFTPSettings->DestroyWindow();
		delete m_pCentralizedFTPSettings;
		m_pCentralizedFTPSettings = NULL;
	}*/

	//LaunchWindowsExplorer();
	return CWinAppEx::ExitInstance();
}

// CApplication message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_LBUTTONUP()
//	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//if(!(GetKeyState(VK_SHIFT) & 0x8000))
		//GetDlgItem(IDC_VERSION_STATIC)->SetWindowText(GetCurrentVersion("Ev_InCuvette"));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// App command to run the dialog
void CApplication::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CApplication customization load/save methods

void CApplication::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CApplication::LoadCustomState()
{
}

void CApplication::SaveCustomState()
{
}

// CApplication message handlers



BOOL CApplication::SetParameters()
{
	HANDLE procHandle = GetCurrentProcess();
	if (m_bClassPriorityRealtime)
		SetPriorityClass(procHandle, HIGH_PRIORITY_CLASS);
	else
		OutputDebugLogString ("Process Class - NOT HIGH_PRIORITY_CLASS");

	m_bCamExtTrig = FALSE;
	//m_bCamExtTrig = (BOOL)GetRegistryEntry(READ_REGPATH + "Camera","Enable External Trigger",(int)m_bCamExtTrig);
	//PutRegistryEntry(WRITE_REGPATH + "Camera","Enable External Trigger",(int)m_bCamExtTrig);

	m_nGrabTimeout = -1;
	m_nGrabTimeout = GetRegistryEntry(READ_REGPATH + "Camera", "Grab Timeout", m_nGrabTimeout);
	if (m_nGrabTimeout==-1) {
		m_nGrabTimeout=1000;
		PutRegistryEntry(WRITE_REGPATH + "Camera", "Grab Timeout", m_nGrabTimeout);
	}

	///////////////////////////////////////////////////////////////////////////
	CString strSec;
	m_strLightLibrary = "void";
	strSec = "IO Library";
	m_strLightLibrary = GetRegistryEntry(READ_REGPATH + "Light", strSec, m_strLightLibrary);
	if(m_strLightLibrary.CompareNoCase("void")==0) {
		m_strLightLibrary = "I_GardaLight";
		PutRegistryEntry(WRITE_REGPATH + "Light", strSec, m_strLightLibrary);
	}
	if(m_strLightLibrary.CompareNoCase("None")!=0)
		m_strLightLibrary += ".dll";

	//m_lIOCardAddr = -1;
	//m_lIOCardAddr = GetRegistryEntry(READ_REGPATH + "Light","IO Card Address",(int)m_lIOCardAddr);
	//if (m_lIOCardAddr==-1) {
	//	m_lIOCardAddr = 0;
	//	PutRegistryEntry(WRITE_REGPATH + "Light","IO Card Address",(int)m_lIOCardAddr);
	//}

	if(InitializeLightLibrary()) {
		CString str;
		str.Format("Failed to initialize Light library %s", m_strLightLibrary);
		AfxMessageBox(str);
	}



	m_strIOInterfaceCard = "void";
	m_strIOInterfaceCard = GetRegistryEntry(READ_REGPATH + "Comm","Library",m_strIOInterfaceCard);
	if (m_strIOInterfaceCard.CompareNoCase("void")==0) {
		m_strIOInterfaceCard = "I_7230";
		PutRegistryEntry(WRITE_REGPATH + "Comm","Library",m_strIOInterfaceCard);
	}
	if(m_strIOInterfaceCard.CompareNoCase("None")!=0)
		m_strIOInterfaceCard+=".dll";

	m_lIOInterfaceCardAddr = -1;
	m_lIOInterfaceCardAddr = GetRegistryEntry(READ_REGPATH + "Comm","Card Address",(int)m_lIOInterfaceCardAddr);
	if (m_lIOInterfaceCardAddr==-1) {
		m_lIOInterfaceCardAddr = 0;
		PutRegistryEntry(WRITE_REGPATH + "Comm","Card Address",(int)m_lIOInterfaceCardAddr);
	}

	m_lIOReverseLogic = 1;
	m_lIOReverseLogic = GetRegistryEntry(READ_REGPATH + "Comm","Reverse Logic",(int)m_lIOReverseLogic);
	PutRegistryEntry(WRITE_REGPATH + "Comm","Reverse Logic",(int)m_lIOReverseLogic);


	m_lCOMMInCardNo = -1;
	m_lCOMMInCardNo = GetRegistryEntry(READ_REGPATH + "Comm","In-Card No",(int)m_lCOMMInCardNo);
	if (m_lCOMMInCardNo==-1) {
		m_lCOMMInCardNo = 0;
		PutRegistryEntry(WRITE_REGPATH + "Comm","In-Card No",(int)m_lCOMMInCardNo);
	}

	CString strIOPort = "void";
	strIOPort = GetRegistryEntry(READ_REGPATH + "Comm","In-Port Id",strIOPort);
	if (strIOPort.CompareNoCase("void")==0) {
		strIOPort = "PORT_1A";
		PutRegistryEntry(WRITE_REGPATH + "Comm","In-Port Id",strIOPort);
	}
	m_lCOMMInPortID = m_CommModule.GetIOPortID(strIOPort);

	m_lCOMMOutCardNo = -1;
	m_lCOMMOutCardNo = GetRegistryEntry(READ_REGPATH + "Comm","Out-Card No",(int)m_lCOMMOutCardNo);
	if (m_lCOMMOutCardNo==-1) {
		m_lCOMMOutCardNo = 0;
		PutRegistryEntry(WRITE_REGPATH + "Comm","Out-Card No",(int)m_lCOMMOutCardNo);
	}

	strIOPort = "void";
	strIOPort = GetRegistryEntry(READ_REGPATH + "Comm","Out-Port Id",strIOPort);
	if (strIOPort.CompareNoCase("void")==0) {
		strIOPort = "PORT_1A";
		PutRegistryEntry(WRITE_REGPATH + "Comm","Out-Port Id",strIOPort);
	}
	m_lCOMMOutPortID = m_CommModule.GetIOPortID(strIOPort);

	if(InitializeCommLibrary()) {
		CString str;
		str.Format("Failed to initialize I/O card (%s)", m_strIOInterfaceCard);
		AfxMessageBox(str);
	}

	m_bCentralizedVerificationMode = 0;
	m_bCentralizedVerificationMode = GetRegistryEntry(READ_REGPATH + "Centralized Comm", "Centralized Verification Mode", m_bCentralizedVerificationMode);
	PutRegistryEntry(WRITE_REGPATH + "Centralized Comm", "Centralized Verification Mode", m_bCentralizedVerificationMode);

	////////////////////Deep Learning Library///////////////////////////
	m_strDeepLearningLibrary = "void";
	strSec = "Deep Learning Library";
	m_strDeepLearningLibrary = GetRegistryEntry(READ_REGPATH + "Deep Learning Engine", strSec, m_strDeepLearningLibrary);
	if (m_strDeepLearningLibrary.CompareNoCase("void") == 0) {
		m_strDeepLearningLibrary = "FCI_Integrate";
		PutRegistryEntry(WRITE_REGPATH + "Deep Learning Engine", strSec, m_strDeepLearningLibrary);
	}
	if (m_strDeepLearningLibrary.CompareNoCase("None") != 0)
		m_strDeepLearningLibrary += ".dll";

	
	if (InitializeDeepLearningLibrary()) {
		CString str;
		str.Format("Failed to initialize Deep Learning library %s", m_strDeepLearningLibrary);
		AfxMessageBox(str);
	}


	/*m_strCommOPC = GetRegistryEntry(READ_REGPATH + "ADSTwinCAT", "Communication_Protocol", "none");
	if(m_strCommOPC == "none")
		PutRegistryEntry(WRITE_REGPATH + "ADSTwinCAT", "Communication_Protocol", m_strCommOPC);
	 if(m_strCommOPC.CompareNoCase("TwinCAT")==0) {
		m_pADSInterface = new CADSInterface();
		m_nADSPortNr = 801;
		strSec.Format("ADS PortNr");
		m_nADSPortNr = GetRegistryEntry(READ_REGPATH + "ADSTwinCAT", strSec, m_nADSPortNr);
		PutRegistryEntry(WRITE_REGPATH + "ADSTwinCAT", strSec, m_nADSPortNr);

		m_nMaxDelay = 2000000;
		strSec.Format("ADS MaxDelay");
		m_nMaxDelay = GetRegistryEntry(READ_REGPATH + "ADSTwinCAT", strSec, m_nMaxDelay);
		PutRegistryEntry(WRITE_REGPATH + "ADSTwinCAT", strSec, m_nMaxDelay);

		m_nCycleTime = 1000000;
		strSec.Format("ADS CycleTime");
		m_nCycleTime = GetRegistryEntry(READ_REGPATH + "ADSTwinCAT", strSec, m_nCycleTime);
		PutRegistryEntry(WRITE_REGPATH + "ADSTwinCAT", strSec, m_nCycleTime);

		m_nTimeout = 1000;
		strSec.Format("ADS Timeout");
		m_nTimeout = GetRegistryEntry(READ_REGPATH + "ADSTwinCAT", strSec, m_nTimeout);
		PutRegistryEntry(WRITE_REGPATH + "ADSTwinCAT", strSec, m_nTimeout);

		m_nTxDelay = 5;
		strSec.Format("ADS TxDelay");
		m_nTxDelay = GetRegistryEntry(READ_REGPATH + "ADSTwinCAT", strSec, m_nTxDelay);
		PutRegistryEntry(WRITE_REGPATH + "ADSTwinCAT", strSec, m_nTxDelay);

		for(int i=0; i<6; i++) {
			m_byteNetID[i] = 0;
			strSec.Format("NetID%d", i);
			m_byteNetID[i] = GetRegistryEntry(READ_REGPATH + "ADSTwinCAT", strSec, m_byteNetID[i]);
			PutRegistryEntry(WRITE_REGPATH + "ADSTwinCAT", strSec, m_byteNetID[i]);
		}
	}*/	

	for(int nAxes=0; nAxes<AXES_COUNT; nAxes++) {
		strSec.Format("Motion Card ID:%d", nAxes+1);
		m_nAxes[nAxes] = GetRegistryEntry(READ_REGPATH + "Motion", strSec, m_nAxes[nAxes]);
		if(m_nAxes[nAxes]==0) {
			m_nAxes[nAxes] = 1500+nAxes;
			PutRegistryEntry(WRITE_REGPATH + "Motion", strSec, m_nAxes[nAxes]);
		}
		OutputDebugLogTo(0,TRUE,"App:Reg Read Axes=%d", m_nAxes[nAxes]);
	}

	m_strMotionCard = "Void";
	strSec = "Motion Library";
	m_strMotionCard = GetRegistryEntry(READ_REGPATH + "Motion", strSec, m_strMotionCard);
	if(m_strMotionCard.CompareNoCase("Void")==0) {
		m_strMotionCard = "M_7856";
		PutRegistryEntry(WRITE_REGPATH + "Motion", strSec, m_strMotionCard);
	}
	if(m_strMotionCard.CompareNoCase("None")!=0)
		m_strMotionCard += ".dll";
	m_bMachineOn = FALSE;

	m_strMotorParmFile = "void";
	m_strMotorParmFile = GetRegistryEntry(READ_REGPATH + "Motion", "Parameter File(.xml)", m_strMotorParmFile);
	if(m_strMotorParmFile.CompareNoCase("void")==0) {
		m_strMotorParmFile.Format("%s\\%s\\7856.xml", m_strMainCfgDir, m_strMainCfgModeDir);
		PutRegistryEntry(WRITE_REGPATH + "Motion", "Parameter File(.xml)", m_strMotorParmFile);
	}

	m_nMotorCardNo = -1;
	m_nMotorCardNo = GetRegistryEntry(READ_REGPATH + "Motion", "Card ID", m_nMotorCardNo);
	if (m_nMotorCardNo < 0) {
		m_nMotorCardNo = 0;
		PutRegistryEntry(WRITE_REGPATH + "Motion", "Card ID", m_nMotorCardNo);
	}

	m_nMotionTimeout = -1;
	m_nMotionTimeout = GetRegistryEntry(READ_REGPATH + "Motion", "Movement TimeOut (ms)", m_nMotionTimeout);
	if (m_nMotionTimeout < 1) {
		m_nMotionTimeout = 60000;
		PutRegistryEntry(WRITE_REGPATH + "Motion", "Movement TimeOut (ms)", m_nMotionTimeout);
	}

	if(InitializeMotionCard()) {
		CString str;
		str.Format("Failed to initialize Motion library %s", m_strMotionCard);
		AfxMessageBox(str);
	}

	motionParameters.LoadDeviceParameters();
	motionParameters.LoadMotionParm();
//	motionParameters.LoadScanningParm();
//	motionParameters.CreateScanPositions();

	for(int i=0; i<5; i++) {				//Unknown
		m_nToolNo[i]	= 0;
		m_nMoldNo[i]	= 0;
		m_nGripperNo[i]	= 0;
		m_nFailNo[i]	= 0;
	}

	m_nGoodACounter = 0;
	m_nGoodBCounter = 0;
	m_nFailCounter = 0;
	m_ThreadManager.Initialize();


	if(!(m_eventOnline = CreateEventA(NULL, TRUE, FALSE, NULL)))
		return FALSE;
	if(!(m_eventVisionReady = CreateEventA(NULL, TRUE, FALSE, NULL)))
		return FALSE;
	if(!(m_EventCommSendMsg = CreateEventA(NULL, TRUE, FALSE, NULL)))
		return FALSE;

	m_hResetReceived = CreateEventA(NULL, TRUE, FALSE, NULL);
	m_hInitPosition = CreateEventA(NULL, TRUE, FALSE, NULL);
	m_hInitPositionDone = CreateEventA(NULL, TRUE, FALSE, NULL);

	m_hResetDone = CreateEventA(NULL, TRUE, FALSE, NULL);
	m_hInitMotionDone = CreateEventA(NULL, TRUE, FALSE, NULL);

	for(int i=0; i<m_nTotalTracks; i++)
		if(!m_pTracks[i].SetParameters(i))
			return FALSE;

	m_LightHandler.SetParameters(m_pTracks, m_nTotalTracks);

	m_bExitApp = FALSE;
	int nError;
	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
		if(m_pTracks[nTrack].m_strFGCard.CompareNoCase("none")==0)
			continue;

		if(m_nCamLibraryTrackIndex < 0) {
			nError = RegisterCameraCard(&m_pTracks[nTrack].m_FGResource, m_pTracks[nTrack].m_strFGCard);
			if(nError) {
				if(AfxMessageBox("Failed to Initialize Camera Library.\nContinue in offline mode?", MB_YESNO|MB_ICONSTOP)==IDNO)
					return FALSE;
				break;
			}

			m_nCamLibraryTrackIndex = nTrack;
		}
		else {
			m_pTracks[nTrack].m_FGResource.CopyFrom(&m_pTracks[m_nCamLibraryTrackIndex].m_FGResource);
			m_pTracks[nTrack].m_FGResource.pCamModule = 0;
		}

		nError = RegisterCamera(&m_pTracks[nTrack].m_FGResource,
			m_pTracks[nTrack].m_strCameraFile,
			m_pTracks[nTrack].m_strFGCardFile,
			FALSE,
			m_bIsCameraMonitorMode);
		if(nError) {
			if(m_bIsCameraMonitorMode)
				AfxMessageBox("[Monitor Mode] Failed to attach to cameras\n\n\nPlease make sure that the MASTER application has\nregistered the cameras before opening in monitor mode", MB_OK|MB_ICONSTOP);
			///return -1;
			continue;
		}

		nError = SetGrabTimeout(&m_pTracks[nTrack].m_FGResource, m_nGrabTimeout);
		if (nError)
			return -1;
	}
	ConfigureLightControllers();	//// moved from main frame
	return TRUE;
}

#define SUPERUSER "neverfailtoadapt"

void CApplication::InitializeDocView()
{
	CEvTrackHandler** pTracks = new CEvTrackHandler*[m_nTotalTracks];
	for(int i=0; i<m_nTotalTracks; i++)
		pTracks[i] = (CEvTrackHandler*) &(m_pTracks[i]);
	((CMainFrame*) m_pMainWnd)->Initialize(pTracks, m_nTotalTracks);


 //	CAppDoc* pDoc[MAX_DOCS];
	//CEvSdiTemplate* pDocTemplate;
	//POSITION pos;
 //	for(int nViews=0; nViews<m_nTotalViews; nViews++) {
	//	CEvTabView* view = ((CEvTabView*) m_pFrames[nViews]->GetActiveView());
	//	for(int nDoc=0; nDoc<1; nDoc++) {
	//		pDocTemplate = view->m_SdiChildWnd[nDoc].m_pDocTemplate;
	//		pos = pDocTemplate->GetFirstDocPosition();
	//		pDoc[nViews] = (CAppDoc*) pDocTemplate->GetNextDoc(pos);
	//		//pDoc[nDoc] = view->GetDocument();
	//	}
	//	if(nViews<m_nTop)
	//		m_pTracks[0].InitializeDocView(pDoc);
	//	else if(nViews<m_nTop+m_nBottom)
	//		m_pTracks[1].InitializeDocView(pDoc);
	//	else 
	//		m_pTracks[2].InitializeDocView(pDoc);


	//}

	CAppDoc* pDoc[MAX_DOCS];
	CEvSdiTemplate* pDocTemplate;
	POSITION pos;
	int nView=0;
 	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
		
		for(int nDoc=0; nDoc<m_pTracks[nTrack].m_nTotalDocs; nDoc++) {
			CEvTabView* view = ((CEvTabView*) m_pFrames[nView]->GetActiveView());
			pDocTemplate = view->m_SdiChildWnd[0].m_pDocTemplate;
			pos = pDocTemplate->GetFirstDocPosition();
			pDoc[nDoc] = (CAppDoc*) pDocTemplate->GetNextDoc(pos);
			//pDoc[nDoc] = view->GetDocument();
			nView++;
		}
		m_pTracks[nTrack].InitializeDocView(pDoc);

		///view->SetTitle();
	}

	
	
	//if(m_nTotalViews > 1){
	//	//m_pTracks[0].m_pDoc[1]->m_pRemoteInspDoc = m_pTracks[0].m_pDoc[0];
	//	////m_pTracks[0].m_pDoc[2]->m_pRemoteInspDoc = m_pTracks[0].m_pDoc[0];
	//	for(int nView=1; nView<m_nTotalViews; nView++)//
	//		m_pTracks[0].m_pDoc[nView]->m_pRemoteInspDoc = m_pTracks[0].m_pDoc[0];//
	//}
	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
		for(int nDoc=1; nDoc<m_nTotalViewDocs[nTrack]; nDoc++) {
			m_pTracks[nTrack].m_pDoc[nDoc]->m_pRemoteInspDoc = m_pTracks[nTrack].m_pDoc[0];
		}
	}
	//CString str;
	//str.Format(" FOV: %d, %d", m_pTracks[0].m_szImgSize.cx, m_pTracks[0].m_rectAOI.Size().cy);
	//UpdateStatusBar(&str, 1);
	OutputDebugLog("Initialize com port");
	m_CommInterface.Initialize();
	//m_CentralizedCommInterface.Initialize();

	OutputDebugLog("Initialize com port Done");

}

void CApplication::BinarizeAllTracks(BOOL bBinaryMode)
{
	CEvImageDoc* pDoc;
 	for(int nViews=0; nViews<m_nTotalViews; nViews++) {
		pDoc = ((CEvTabView*) m_pFrames[nViews]->GetActiveView())->GetActiveDoc();
		SwitchImg(&pDoc->m_ImageViewInfo, bBinaryMode,(LPARAM)&pDoc->m_nThreshold);
	}
}

void CApplication::StreamAllTracksToggle()
{
	CAppDoc* pDoc;
 	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
		pDoc = (CAppDoc*) ((CEvTabView*) m_pFrames[nTrack]->GetActiveView())->GetActiveDoc();
		if(pDoc->m_pTrack->m_bCamAvail) {
			//pDoc->CameraLive();
			Sleep(20);
		}
	}
}

BOOL CApplication::InitProcess()
{
	CMainFrame *pMain;
	pMain = ((CMainFrame*)AfxGetMainWnd());
	//CWinThread* pThread;
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    _CrtSetBreakAlloc(681);
	//LaunchWindowsExplorer();

	InitializeCriticalSection(&m_CriticalSection);
	InitializeCriticalSection(&m_csOnline);
	InitializeCriticalSection(&m_csFileAccess);
	InitializeCriticalSection(&m_csOutputData);

	InitializeCriticalSection(&m_csLoadConfig);
	InitializeCriticalSection(&m_csCentralizedQueue);


	if(m_bIsMasterApp)
	{
		m_hInspectSeqThread = m_ThreadManager.CreateThread(InspectSeqThread, this, SEQUENCE_THREAD);
		//m_hVisionResetThread = m_ThreadManager.CreateThread(VisionResetThread, this, VISION_RESET_THREAD);
		//m_hPollingThread = m_ThreadManager.CreateThread(PollingThread, this, POLLING_THREAD);

		m_EventRefreshLight = CreateEventA(NULL, TRUE, FALSE, NULL);
		m_EventMotionSensor = CreateEventA(NULL, TRUE, FALSE, NULL);
		m_EventMotionSensorStart  = CreateEventA(NULL, TRUE, FALSE, NULL);
		
		AfxBeginThread(RefreshLightThread, this, THREAD_PRIORITY_LOWEST);

		AfxBeginThread(RefreshMotionStop, this, THREAD_PRIORITY_LOWEST);

		AfxBeginThread(RefreshMotionStart, this, THREAD_PRIORITY_LOWEST);

		AfxBeginThread(ResetMotor, this, THREAD_PRIORITY_LOWEST);
	
		AfxBeginThread(StartScanPosition, this, THREAD_PRIORITY_LOWEST);
		for (int nThread = 0; nThread < 4; nThread++) {
			if (AfxBeginThread(InspectOnlineThread, &m_pTracks[STATION_TOP], THREAD_PRIORITY_BELOW_NORMAL) == NULL) {
				OutputDebugLogTo(0, FALSE, "----- ERROR: Can't startup 2-3D Inspection Thread");
				return FALSE;
			}
		}

		for (int nThread = 0; nThread < 4; nThread++) {
			if (AfxBeginThread(InspectOnlineThread, &m_pTracks[STATION_BOTTOM], THREAD_PRIORITY_BELOW_NORMAL) == NULL) {
				OutputDebugLogTo(0, FALSE, "----- ERROR: Can't startup 2-3D Inspection Thread");
				return FALSE;
			}
		}
	}

	//Warning Message Dialog Position
	int iEvenCount = 0, iOddCount = 0;
	CRect rectErrorDlgLeft, rectErrorDlgRight;
	for (int i = 0; i < NO_OF_PORT_CENTRALIZED; i++) {
		pErrorMsgDlg[i] = new CErrorDlg();
		if (!pErrorMsgDlg[i]->Create(IDD_ERRORDLG, AfxGetMainWnd())) {
			delete pErrorMsgDlg[i];
			pErrorMsgDlg[i] = NULL;
		}
		else {
			CRect rectErrorDlg;
			pErrorMsgDlg[i]->GetWindowRect(&rectErrorDlg);
			if (i == 0) {
				rectErrorDlgLeft = CRect(rectErrorDlg.left, rectErrorDlg.top , rectErrorDlg.Width(), rectErrorDlg.Height());
				pErrorMsgDlg[i]->SetWindowPos(NULL, rectErrorDlgLeft.left, rectErrorDlgLeft.top , rectErrorDlgLeft.Width(), rectErrorDlgLeft.Height(), SWP_NOSIZE | SWP_NOZORDER);
				iEvenCount++;
			}
			else if(i == 1){
				rectErrorDlgRight = CRect(rectErrorDlg.left + rectErrorDlg.Width(), rectErrorDlg.top , rectErrorDlg.Width(), rectErrorDlg.Height());

				pErrorMsgDlg[i]->SetWindowPos(NULL, rectErrorDlgRight.left, rectErrorDlgRight.top , rectErrorDlgRight.Width(), rectErrorDlgRight.Height(), SWP_NOSIZE | SWP_NOZORDER);
				iOddCount++;
			}
			else {
				if (i % 2 == 0) {
					pErrorMsgDlg[i]->SetWindowPos(NULL, rectErrorDlgLeft.left, rectErrorDlgLeft.top + iEvenCount*rectErrorDlgLeft.Height(), rectErrorDlgLeft.Width(), rectErrorDlgLeft.Height(), SWP_NOSIZE | SWP_NOZORDER);
					iEvenCount++;
				}
				else {
					pErrorMsgDlg[i]->SetWindowPos(NULL, rectErrorDlgRight.left, rectErrorDlgRight.top + iOddCount*rectErrorDlgRight.Height(), rectErrorDlgRight.Width(), rectErrorDlgRight.Height(), SWP_NOSIZE | SWP_NOZORDER);
					iOddCount++;
				}
			}
			
		}
	}
	

	m_pProgressDlg = new CProgressBarDlg();
	if (!m_pProgressDlg->Create(IDD_PROGRESS_DLG, dynamic_cast<CMainFrame*>(AfxGetMainWnd()))) {
		delete m_pProgressDlg;
		m_pProgressDlg = NULL;
	}
	else {
		m_pProgressDlg->CenterWindow();
		m_pProgressDlg->SetFocus();
	}

	m_pImageQueue = new CImageQueueHandler();
	m_pImageQueue->Initialize(m_pTracks[0].m_pDoc[0]->m_ImageViewInfo.sizeImage);

	m_pMotionControl = new CMotionControlDialog(&motionParameters, m_pMainWnd);
	if(!m_pMotionControl->Create(IDD_MOTOR_DLG, m_pMainWnd)) {
		delete m_pMotionControl;
		m_pMotionControl = NULL;
	}

	m_pCentralizedMonitorDlg = new CCentralizedMonitorDlg();
	if (!m_pCentralizedMonitorDlg->Create(IDD_CENTRALIZED_MONITOR_DIALOG, m_pMainWnd)) {
		delete m_pCentralizedMonitorDlg;
		m_pCentralizedMonitorDlg = NULL;
	}

	m_pStationMapDlg = new CStationMapping(m_pMainWnd);
	if(!m_pStationMapDlg->Create(IDD_STATION_MAP_DLG, m_pMainWnd)) {
		delete m_pStationMapDlg;
		m_pStationMapDlg = NULL;
	}

	m_pWndPVIDefectParameters = new CPVIInspectionParameters(m_pMainWnd);
	if(!m_pWndPVIDefectParameters->Create(IDD_PVI_INSPECTION_PARM, m_pMainWnd)) {
		delete m_pWndPVIDefectParameters;
		m_pWndPVIDefectParameters = NULL;
		//return FALSE;
	}

	m_pCalibrationDlg = new CCalibrationSettings(m_pMainWnd);
	if(!m_pCalibrationDlg->Create(IDD_CALIBRATIONSETTINGS,m_pMainWnd)) {
		delete m_pCalibrationDlg;
		m_pCalibrationDlg = NULL;
	}

	m_pDispOnlinePanel = new CDisplayOnlinePanel(m_pMainWnd);
	if (!m_pDispOnlinePanel->Create(IDC_ONLINE_DISPLAY_PANEL, m_pMainWnd)) {
		delete m_pDispOnlinePanel;
		m_pDispOnlinePanel = NULL;
	}
	
	m_pDisplayMsgStatus = new CMessageDlg(m_pMainWnd);
	if (!m_pDisplayMsgStatus->Create(IDD_DIALOGMSG_DLG, m_pMainWnd)) {
		delete m_pDisplayMsgStatus;
		m_pDisplayMsgStatus = NULL;
	}
	else {
		m_pDisplayMsgStatus->ShowWindow(FALSE);
	}

	m_pInfoMsgDlg = new CInfoMsgDlg(m_pMainWnd);
	if (!m_pInfoMsgDlg->Create(IDD_INFO_MESSAGE_DLG, m_pMainWnd)) {
		delete m_pInfoMsgDlg;
		m_pInfoMsgDlg = NULL;
	}
	else {
		m_pInfoMsgDlg->ShowWindow(FALSE);
	}


	/*m_pCentralizedFTPSettings = new CCentralizedFTPSettings(m_pMainWnd);
	if (!m_pCentralizedFTPSettings->Create(IDD_CENTRALIZED_COMMUNICATION_DLG, m_pMainWnd)) {
		delete m_pCentralizedFTPSettings;
		m_pCentralizedFTPSettings = NULL;
	}
	else {
		m_pCentralizedFTPSettings->ShowWindow(FALSE);

		if(m_bCentralizedVerificationMode)
			m_pCentralizedFTPSettings->Connect();
	}*/


	m_pMainWnd->ShowWindow(SW_HIDE);
	//m_pMainWnd->ShowWindow(SW_HIDE);
	m_bLoadComplete = TRUE;
	//((CMainFrame*) m_pMainWnd)->RecalculateLayout();

	m_UserAccInfo.m_strCurUserName = "Operator";
	m_UserAccInfo.m_nCurrentAccessLevel = 0;
	m_UserAccInfo.m_strCurrentAccessLevel = "Operator";

	
	/*if(m_pFrames[m_nTotalViews]) {
		m_pStatsView = (CStatisticsView*)m_pFrames[m_nTotalViews]->GetActiveView();
		if(m_pStatsView && ::IsWindow(m_pStatsView->m_ctrlDefectsList.m_hWnd)) {
			m_pStatsView->Initialize();
			UpdateStatisticsWindow();
			AddLogEvent("FCI Vision Application started");
		}
	}
	else
		m_pStatsView = NULL*/;

	//m_pTracks[0].m_pDoc[0]->m_InspectionHandler.CreateParameterList();
	///LoadRecipes();

	m_pMainWnd->PostMessage(WM_LOAD_RECIPE);

	m_UserLog.LoadUserAccountData();
	m_AdvSecurityDlg.GetSecurityAccess();

	//m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);

	//if(m_AdvSecurityDlg.m_data.m_nLensInsp > 0)
	//	OnVisionOnline();

	for(int i=0; i<m_nTotalViews; i++)
		((CEvChildFrame*) m_pFrames[i])->EnableMaximizeBox();
	 
	CSplashScreen::HideSplashScreen();
	m_pMainWnd->SetFocus();

	if (m_pProgressDlg != NULL) {
		m_pProgressDlg->ShowWindow(SW_SHOWNA);
		m_bProgressDlgOpen = TRUE;
		m_pProgressDlg->SetStatus("Initialize The Doc View...", 0);
	}


	//if(!m_pStatsView/* && m_nTotalViews==1*/)
	{
		((CMainFrame*) m_pMainWnd)->MDIMaximize(m_pFrames[0]);
		((CEvTabView*) m_pFrames[0]->GetActiveView())->Invalidate();
	}
	//else
		//((CMainFrame*) m_pMainWnd)->MDIActivate(m_pFrames[0]);

	m_bDisplayAllStatsOffline = FALSE;//

	/*((CMainFrame*) m_pMainWnd)->ShowStatsPane();
	
	((CMainFrame*) m_pMainWnd)->ShowPreviousImages();

	((CMainFrame*) m_pMainWnd)->ShowDebugView();

	
	((CMainFrame*) m_pMainWnd)->RecalcLayout();*/
	int nLoadedDataPercentage = 0;
	double dStep = 3 * 1.0 / m_nTotalViews, dLoadedDataPercentage = nLoadedDataPercentage;

	for(int nView=0; nView<m_nTotalViews; nView++) {
		CEvTabView* view = ((CEvTabView*) m_pFrames[nView]->GetActiveView());
		view->SetTitle();

		if (m_pProgressDlg != NULL) {
			dLoadedDataPercentage += dStep;
			nLoadedDataPercentage = dLoadedDataPercentage;
			m_pProgressDlg->SetStatus("Initialize The Doc View...", nLoadedDataPercentage);
		}
	}


	OnDefaultSetting();

	//Init Deep Learning Model
	InitializeDeepLearningModel();

	Sleep(1000);

	m_bReceivedResetCmd = TRUE;
	m_bReceivedResetCmdPrevStatus = m_bReceivedResetCmd;
	SetEvent(m_hResetReceived);
	SetEvent(m_EventMotionSensor);

//	MoveToStartPos(TRUE);

	m_CentralizedCommInterface.Initialize();

	SetUserName(m_UserAccInfo.m_strCurUserName);	
	AddLogEvent("Logged in as Operator");
	//m_pMainWnd->SendMessageA(WM_SET_FULLSCREEN, TRUE, FALSE);
	return TRUE;
}


int CApplication::InitializeLightLibrary()
{
	int nError = m_LightModule.LoadIODll(m_strLightLibrary);
	if(nError) {
		OutputDebugLogTo(0, TRUE, "Error loading Light I/O Library %s", m_strLightLibrary);
		return 0;
	}
	else if(m_strLightLibrary.CompareNoCase("none")!=0)
		OutputDebugLog("Light Library '%s' loaded", m_strLightLibrary);

	//int nError = m_LightModule.RegisterCard(m_lIOCardAddr, 0);
	//if(nError)
	//	return nError;

	//nError = m_LightModule.ConfigIOPort(m_lIOCardAddr, m_pTracks[nTrack].m_lLightPortID[nPort], IO_MODE_OUT, 0);
	//if(nError)
	//	return nError;

	return 0;
}

int CApplication::InitializeDeepLearningLibrary()
{
	int nError = m_DeepLearningModule.LoadDeepLearningDll(m_strDeepLearningLibrary);
	if (nError) {
		OutputDebugLogTo(0, TRUE, "Error loading Deep Learning Library %s", m_strDeepLearningLibrary);
		return nError;
	}
	else if (m_strDeepLearningLibrary.CompareNoCase("none") != 0)
		OutputDebugLog("Deep Learning Library '%s' loaded", m_strDeepLearningLibrary);

	return 0;
}

int CApplication::InitializeDeepLearningModel()
{
	int nError = 0;
	int nLoadedDataPercentage = m_pProgressDlg->m_nCurrentPos;

	if (m_pProgressDlg != NULL) {
		m_pProgressDlg->ShowWindow(TRUE);
		m_bProgressDlgOpen = TRUE;
		m_pProgressDlg->UpdateWindow();
		m_pProgressDlg->SetStatus("Initialize Deep Learning Model...", nLoadedDataPercentage);
	}

	m_DeepLearningModule.m_bDLModelInspectFromFile = GetRegistryEntry("Software\\Vision\\FCI_4X\\Deep Learning Engine", "DL Model Inspect From File", 1);

	//CString strModelListDefault[] = { "FULL_TOP_ENCAP", "TOP_ENCAP" };
	//// Initialize vector with a string array
	//std::vector<CString> modelList(strModelListDefault, strModelListDefault + sizeof(strModelListDefault) / sizeof(CString));

	m_DeepLearningModule.m_DeepLearningModelList.clear();
	m_DeepLearningModule.GetModelList(m_DeepLearningModule.m_DeepLearningModelList, false);
	CPerformanceTimer timeDLModelInit;
	for (int i = 0; i < m_DeepLearningModule.m_DeepLearningModelList.size(); i++) {
		OutputDebugLogTo(9, TRUE, "InitializeDeepLearningModel: InitializeModel %s...", m_DeepLearningModule.m_DeepLearningModelList[i]);
		timeDLModelInit.Reset();
		int nErrorTemp = m_DeepLearningModule.InitializeModel(m_DeepLearningModule.m_DeepLearningModelList[i], true);
		OutputDebugLogTo(9, TRUE, "InitializeDeepLearningModel: InitializeModel %s Done - Execution time: %.3f ms", m_DeepLearningModule.m_DeepLearningModelList[i], timeDLModelInit.GetElapsedMSec());

		if (nErrorTemp) {
			OutputDebugLogTo(9, TRUE, "InitializeDeepLearningModel: failed at model %s, Error Code: %d", m_DeepLearningModule.m_DeepLearningModelList[i], nErrorTemp);
			nError = nErrorTemp;
		}
		else {
			//Inspect dummy buffer to speed-up inspect later(First time call DLInspect() from Deep Learning model is always slower)
			OutputDebugLogTo(9, TRUE, "InitializeDeepLearningModel: Model %s Inspecting Dummy buffer...", m_DeepLearningModule.m_DeepLearningModelList[i]);
			timeDLModelInit.Reset();
			BYTE* dummyBuffer = new BYTE[2500];
			memset(dummyBuffer, 0, (50 * 50)*sizeof(BYTE));
			CString strDefectName;
			m_DeepLearningModule.DLInspect(m_DeepLearningModule.m_DeepLearningModelList[i], 50, 50, dummyBuffer, strDefectName, true);
			OutputDebugLogTo(9, TRUE, "InitializeDeepLearningModel: Model %s Inspecting Dummy buffer Done - Execution time: %.3f ms", m_DeepLearningModule.m_DeepLearningModelList[i], timeDLModelInit.GetElapsedMSec());
		}
		m_pProgressDlg->SetStatus("Initialize Deep Learning Model...", nLoadedDataPercentage + 1);
	}

	return nError;
}


int CApplication::ConfigureLightControllers()
{
	m_LightHandler.ConfigureControllers();

	return 0;
}

int CApplication::InitializeCommLibrary()
{
	int nError = m_CommModule.LoadIODll(m_strIOInterfaceCard);
	if(nError) {
		OutputDebugLog("Error loading Comm Library %s", m_strIOInterfaceCard);
		return nError;
	}
	else if(m_strIOInterfaceCard.CompareNoCase("none")!=0)
		OutputDebugLog("Comm Library '%s' loaded", m_strIOInterfaceCard);

	nError = m_CommModule.RegisterCard(m_lCOMMOutCardNo, m_lIOInterfaceCardAddr);
	if(nError)
		return nError;

	nError = m_CommModule.ConfigIOPort(m_lCOMMOutCardNo, m_lCOMMOutPortID, IO_MODE_OUT, 0);
	if(nError)
		return nError;


	return 0;
}


int CApplication::InitializeMotionCard()
{
	int nError = m_MotorResource.LoadMotorDll(m_strMotionCard);
	if(nError || !m_strMotionCard.CompareNoCase("none"))
		m_bMachineOn = FALSE;
	if(nError) {
		OutputDebugLog("Error loading Motion DLL %s", m_strMotionCard);	
		return nError;
	}
	m_bMachineOn = TRUE;
	OutputDebugLog("Motion DLL %s loaded", m_strMotionCard);

	/*m_nAxes[0] = 1501;
	m_nAxes[1] = 1500;
	m_nAxes[2] = 1502;
	m_nAxes[3] = 1503;*/

	Sleep(1000);

	nError = m_MotorResource.RegisterCard(m_nMotorCardNo, m_nAxes[0]);
	if(nError)
		return nError;

	/*nError = m_MotorResource.RegisterIOCard(0);
	if(nError)
		return nError;*/
	Sleep(1000);


	char * pcFileName = NULL;
	pcFileName = (char*)LPCTSTR(m_strMotorParmFile);
	m_MotorResource.InitializeCard(/*m_strMotorParmFile.GetBuffer(0)*/pcFileName);
	//m_strMotorParmFile.ReleaseBuffer();

	Sleep(1000);

	motionParameters.InitMotionCard();

	Sleep(1000);

	//for(int i=0; i<AXES_COUNT; i++)
	//	m_MotorResource.InitializeAxis(m_nAxes[i]);

	m_MotorResource.OutPortWrite(0,1,0);	//// ON

	/*for(int i=0; i<AXES_COUNT; i++)
		m_MotorResource.SetServoOn(m_nAxes[i], TRUE);*/

	Sleep(1000);

	motionParameters.SetServoOnAllAxis();

	/*if(m_MotorResource.lpSetWaitTimeOut)
		m_MotorResource.lpSetWaitTimeOut(INFINITE);*/

	return 0;
}


int OutPortWriteApp(BOOL bLightModule, long lCardNo, long lPortID, long lData, long lReserved)
{
	if(bLightModule)
		return theApp.m_LightModule.OutPortWrite(lCardNo, lPortID, lData, lReserved);
	else
		return theApp.m_CommModule.OutPortWrite(lCardNo, lPortID, lData, lReserved);
}

int OutPortSendString(int nController, CString* pstrData)
{
	int nError = theApp.m_LightHandler.SendLightCommandString(nController, pstrData);

	return nError;
}

int OutPortReadApp(BOOL bLightModule, long lCardNo, long lPortID, long *plData, long lReserved)
{
	if(bLightModule)
		return theApp.m_LightModule.OutPortRead(lCardNo, lPortID, plData, lReserved);
	else
		return theApp.m_CommModule.OutPortRead(lCardNo, lPortID, plData, lReserved);
}

int InPortReadApp(BOOL bLightModule, long lCardNo, long lPortID, long *plData, long lReserved)
{
	if(bLightModule)
		return theApp.m_LightModule.InPortRead(lCardNo, lPortID, plData, lReserved);
	else
		return theApp.m_CommModule.InPortRead(lCardNo, lPortID, plData, lReserved);
}

int CApplication::CheckPassword(int SecurityAccessNo)
{
	CPasswordDialog dlg(m_pMainWnd);
	if(m_bSuperUserMode)
		return 0;

	if (dlg.DoModal()==IDOK) {
		if(dlg.m_strPassword == SUPERUSER) {
			//m_bSuperUserMode = TRUE;
			//((CMainFrame*)m_pMainWnd)->m_wndLogo.SwitchImage();
			return 0;
		}
		else {
			AfxMessageBox("Wrong Password", MB_OK | MB_ICONERROR);
			return -1;
		}
	}

	return -1;
}

int CApplication::CombinedComGrabInsp()
{
	CString strMsg = "";
	UpdateStatusBar(&strMsg,2);
	UpdateStatusBar(&strMsg,3);

//	CPerformanceTimer timer;
//	timer.Reset();
	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
		if(m_pTracks[nTrack].m_bCamEnable && (m_cActiveTracks & (1<<nTrack))){
			for(int nDoc=0; nDoc<m_pTracks[nTrack].m_nTotalDocs; nDoc++)
				m_pTracks[nTrack].ComGrabInspect(nDoc);
		}
	}

	//if(WaitForEndTrackGrab(0)==-1)
	//	return -1;

	if(CombinedWaitForEndInsp()==-1)
		return -1;
//	OutputDebugLog("Total ImageGrab Time (5): %5.2f ms", timer.GetElapsedMSec());

	return 0;
}

void CApplication::UpdateTrackStatus()
{
	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
		for(int nDoc=0; nDoc<m_pTracks[nTrack].m_nTotalDocs; nDoc++)
			m_pTracks[nTrack].m_pDoc[nDoc]->DrawTrackStatus(m_cActiveTracks & (1<<nTrack));
	}
}

int CApplication::CombinedComGrab() 
{
	CString strMsg;
	strMsg.Format("");
	UpdateStatusBar(&strMsg,2);
	UpdateStatusBar(&strMsg,3);

//	CPerformanceTimer timer;
//	timer.Reset();
	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
		if(m_pTracks[nTrack].m_bCamEnable){
			for(int nDoc=0; nDoc<m_pTracks[nTrack].m_nTotalDocs; nDoc++)
				m_pTracks[nTrack].ComGrab(nDoc);
		}
	}

	//if(WaitForEndTrackGrab(0)==-1)
	//	return -1;

//	if(WaitForEndGrab()==-1)
//		return -1;
//	OutputDebugLog("Total ImageGrab Time (5): %5.2f ms", timer.GetElapsedMSec());

	return 0;
}

int CApplication::WaitForEndTrackGrab(int nTrack)
{
	DWORD dwObj;
	int nError = 0;
	OutputDebugLogTo(9,TRUE,"CApplication::WaitForEndTrackGrab");//// for testing

	dwObj = WaitForSingleObject(m_pTracks[nTrack].m_EventGrabEnd, 10000);
	if(dwObj==WAIT_TIMEOUT){
		//OutputDebugLogTo (6, FALSE, "=== Wait for Track 1 End Grab Error-TIMEOUT {%4.2lf}", InspectSeqTime.GetElapsedMSec());
		OutputDebugLog("=== [Track %d] Wait for End Grab Error-TIMEOUT {%4.2lf}", nTrack, InspectSeqTime.GetElapsedMSec());
		return -1;
	}
	return 0;
}

int CApplication::WaitForEndGrab(int nReserved)
{
	DWORD dwObj;
	int nError = 0;

	HANDLE* Handle;
	Handle = new HANDLE[abs(m_nTotalTracks)];
	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++)
		Handle[nTrack] = m_pTracks[nTrack].m_EventGrabEnd;

	dwObj = WaitForMultipleObjects(m_nTotalTracks, &Handle[0], TRUE, 10000/*INFINITE*/);
	delete[] Handle;
	if(dwObj==WAIT_TIMEOUT){
		//OutputDebugLogTo (6, FALSE, "=== Wait for Track 1 End Grab Error-TIMEOUT {%4.2lf}", InspectSeqTime.GetElapsedMSec());
		OutputDebugLog("=== Wait for End Grab Error-TIMEOUT {%4.2lf}", InspectSeqTime.GetElapsedMSec());
		return -1;
	}
	return 0;

	switch (dwObj - WAIT_OBJECT_0) {
		case 0:
			return 0;	//Thread return correctly
		case 1:
			//OutputDebugLogTo (6, FALSE, "=== Wait for Track 1 End Grab Error- {%4.2lf} Main Thread exits", InspectSeqTime.GetElapsedMSec());
			OutputDebugLog("=== Wait for End Grab Error- {%4.2lf} Main Thread exits", InspectSeqTime.GetElapsedMSec());
			return -1;
		default:
			//OutputDebugLog(6, FALSE, "=== Wait for Track 1 End Grab Error-UNKNOWN {%4.2lf}", InspectSeqTime.GetElapsedMSec());
			OutputDebugLog("=== Wait for End Grab Error-UNKNOWN {%4.2lf}", InspectSeqTime.GetElapsedMSec());
			return -1;
	}
}

int CApplication::CombinedWaitForEndInsp(int nReserved)
{
	DWORD dwObj;
	int nError = 0;

	HANDLE* Handle;
	Handle = new HANDLE[abs(m_nTotalTracks)];
	int nActiveTracks = 0;
	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
		if(m_pTracks[nTrack].m_bCamEnable && (m_cActiveTracks & (1<<nTrack)))
			Handle[nActiveTracks++] = m_pTracks[nTrack].m_pDoc[0]->m_EventInspEnd;
	}

	dwObj = WaitForMultipleObjects(nActiveTracks, &Handle[0], TRUE, 3000);
	delete[] Handle;
	if(dwObj == WAIT_TIMEOUT) {
      //OutputDebugLogTo (6, FALSE, "=== Wait for Track 1 End Insp Error-TIMEOUT {%4.2lf}", InspectSeqTime.GetElapsedMSec());
      OutputDebugLog("=== Wait for End Insp Error-TIMEOUT {%4.2lf}", InspectSeqTime.GetElapsedMSec());
	  return -1;
	}
	return 0;

	switch (dwObj - WAIT_OBJECT_0) {
		case 0:
			return 0;		//Thread return correctly
		case 1:
			//OutputDebugLogTo (6, FALSE, "=== Wait for Track 1 End Insp Error- {%4.2lf}Main Thread exits", InspectSeqTime.GetElapsedMSec());
			OutputDebugLog("=== Wait for End Insp Error- {%4.2lf}Main Thread exits", InspectSeqTime.GetElapsedMSec());
			return -1;
		default:
			//OutputDebugLogTo (6, FALSE, "=== Wait for Track 1 End Insp Error-UNKNOWN {%4.2lf}", InspectSeqTime.GetElapsedMSec());
			OutputDebugLog("=== Wait for End Insp Error-UNKNOWN {%4.2lf}", InspectSeqTime.GetElapsedMSec());
			return -1;
	}
}

int CApplication::WaitForEndTrackInspect(int nTrack)
{
	DWORD dwObj;
	int nError = 0;

	dwObj = WaitForSingleObject(m_pTracks[nTrack].m_pDoc[0]->m_EventInspEnd, 10000);
	if(dwObj==WAIT_TIMEOUT){
		//OutputDebugLogTo (6, FALSE, "=== Wait for Track 1 End Grab Error-TIMEOUT {%4.2lf}", InspectSeqTime.GetElapsedMSec());
		OutputDebugLog("=== [Track %d] Wait for End Insp Error-TIMEOUT {%4.2lf}", nTrack+1, InspectSeqTime.GetElapsedMSec());
		return -1;
	}
	return 0;
}


CString CApplication::GetFailedStr (int err, bool abbrev)
{
	CString result;
	switch (abs(err)) {
		case PROCESS_ERROR :
		case IMAGE_SNAP_FAIL:
		case USER_ABORT :
			result = abbrev ? " ProcessError " : " Process Error";
			break;

		case VISION_TIMEOUT:
			result = abbrev ? " Timeout " : "Vision Timeout";
			break;
		case INSP_PASS:
			result = abbrev ? " Pass " : "Pass";
			break;
		case NO_IMAGE:
			result=abbrev ? " NoImage " : "No Image";
			break;
		//case INSP_FAIL_NO_DEVICE:
		//	result = abbrev ? " NoLens " : "Location";
		//	break;
		case DEVICE_LOCATION:
			result = abbrev ? "DeviceLocation":"Device Location";
			break;
		case DATUM_LOCATION:
			result = abbrev ? "DatumLocation" : "Datum Location";
			break;
		case TILT_INSP:
			result = abbrev ? "Tilt" : "Tilted";
			break;
		case COVER_LAYER_INSP:
			result = abbrev ? "CoverLayerAbsent" : "CoverLayer Absent";
			break;
		case COVER_LAYER_ALIGNMENT:
			result = abbrev ? "CoverLayerAlignment" : "CoverLayer Alignment";
			break;
		case ENCAP_INSUFFICIENT:
			result = abbrev ? "EncapInsufficient" : "Encap Insufficient";
			break;
		case ENCAP_EXCESS:
			result = abbrev ? "ExcessEncap" : "Excess Encap";
			break;
		case ENCAP_CRACK:
			result = abbrev ? "EncapCrack" : "Encap Crack";
			break;
		case ENCAP_BLOWHOLE:
			result = abbrev ? "EncapBlowHole" : "Blow Hole";
			break;
		case ENCAP_CONTAMINATION:
			result = abbrev ? "Encap Contamination" : "Contamination";
			break;
		case SLOT:
			result = abbrev ? "Slot" : "Slot";
			break;
		/*case DIE_INSP:
			result = abbrev ? "Die" : "Die";
			break;*/
		case DIE_EDGE:
			result = abbrev ? "PVIFail" : "PVI Fail";
			break;
		//-----Edge Zone------
		//case EDGE_ROUGHNESS:
		//	result = abbrev ? " EdgeRoughness " : "Edge Roughness";
		//	break;
		//case MULTIPLE_LENS:
		//	result = abbrev ? " MultipleLens " : "Multiple Lens";
		////	result = abbrev ? " SplitEdge " : "Split Edge";
		//	break;
		//case TORN_EDGE:
		//	result = abbrev ? " EdgeTorn " : "Edge Torn";
		//	break;
		//case FILP_LENS:
		//	//result = abbrev ? " FilpLens " : "Filp Lens";
		//	result = abbrev ? " DieSU8 " : "Die SU8";
		//	break;
		//case EDGE_BURR:
		//	result = abbrev ? " EdgeBurr " : "Edge Burr";
		//	break;
		//case EDGE_TIP:
		//	result = abbrev ? " EdgeTip " : "Edge Tip";
		//	break;
		//case LENS_CRACK:
		//	result = abbrev ? " LensCrack " : "Lens Crack";
		//	break;
		//case MISSING_PATTERN:
		//	result = abbrev ? " MissingPrint " : "Missing Print";
		//	break;
		//case PRINT_SMEAR:
		//	result = abbrev ? " PrintSmear " : "Print Smear";
		//	break;
		//case MOLD_FLASH:
		//	result = abbrev ? " MoldFlash" : "Mold Flash";
		//	break;
		//case HOLE_DEFECT:
		//	result = abbrev ? " HoleDefect " : "Hole Defect";
		//	break;	
		//case BUBBLE:
		//	result = abbrev ? "Bubble" : "Bubble";
		//	break;
		//case INLENS_TEAR:
		//	result = abbrev ? " InLensTear" : "InLens Tear";
		//	break;	
		//case SURFACE_SCRATCH:
		//	result = abbrev ? " SurfaceScratch" : "Surface Scratch";
		//	break;	
		////-----Edge Zone------
		//case PRINT_SHIFT://MISSING_PATTERN:
		//	//result = abbrev ? " MissingPattern " : "Missing Pattern";
		//	result = abbrev ? " PrintShift " : "Print Shift";
		//	break;
		//case CONTAMINATION:
		//	result = abbrev ? "Contamination" : "Contamination";
		//	break;
		//case FIBER:
		//	result = abbrev ? " Fiber " : "Fiber Defect";
		//	break;
		//case EDGE_FIBER:
		//	result = abbrev ? "EdgeFiber " : "Edge Fiber Defect";
		//	break;
		//case DARK_SPOT_CLEAR:
		//	result = abbrev ? " DarkSpotClear" :"Dark Spot Clear";
		//	break;
		//case SURFACE_DEFECT:
		//	result = abbrev ? " SurfaceDefect " :"Surface Defect";
		//	break;
		//case EDGE_TEAR:
		//	result = abbrev ? " EdgeTear " :"Edge Tear";
		//	break;
		//case EDGE_DEFECT:
		//	result = abbrev ? " EdgeDefect" :"Edge Defect";
		//	break;
		//case POLAR_TRANSFORMATION:
		//	result = abbrev ? " PolarTransformation " :"Polar Transformation";
		//	break;
		case UNKNOWN_DEFECT:
			result = abbrev ? " Unknown " :"Unknown";
			break;
	}
	return result;
}


CString CApplication::GetCuvetteFailedStr (int err, bool abbrev)
{
	CString result="";
	/*int nErrorCode = CUVETTE_DEFECT + err;
	switch (nErrorCode) {
		case 1 :
			result = abbrev ? " DarkScratches " : " Dark Scratches";
			break;

		case 2:
			result = abbrev ? " BigExcessParticles " : "Big Excess Particles";
			break;

		case 3 :
			result=abbrev ? " SandBeach " : "Sand Beach";
			break;

		case 4 :
			result = abbrev ? " DarkSpot " : " DarkSpot";
			break;

		case 5:
			result = abbrev ? " Contamination " : "Contamination";
			break;

		case 6 :
			result=abbrev ? " LightScratches " : "Light Scratches";
			break;

	}*/
	return result;
}

int CApplication::StartInspectSeq(long lInData)
{
	CIOInterface* pIoRes = &m_CommModule;
	int nTrack = 0;

	//RecordLotStats();
	OutputDebugLogTo(nTrack+1,TRUE,"------------Start of Sequence[Track%d] Before Busy: ---------------", nTrack+1);
	m_pTracks[nTrack].m_pDoc[0]->m_bInspecting = TRUE;

	ResetEvent(m_eventVisionReady);
		
	EnterCriticalSection(&m_csOutputData);
	m_nOutData |= OUT_BUSY;
	m_nOutData &= ~OUT_RESULT1;
	m_nOutData &= ~OUT_RESULT2;
	m_CommModule.OutPortWrite(m_lCOMMInCardNo, m_lCOMMOutPortID, m_nOutData, 0);
	LeaveCriticalSection(&m_csOutputData);

	m_pTracks[0].ComGrabInspect(0);
	WaitForSingleObject(m_pTracks[0].m_pDoc[0]->m_EventInspEnd, INFINITE);

	SetEvent(m_eventVisionReady);

	return 0;
}

int CApplication::StartInspectSeqOnlineSimulation(long lInData)
{
	int nError=0;
	CIOInterface* pIoRes = &m_CommModule;

	//RecordLotStats();

	//CString str, strTemp;
	//str = "Processing tracks:";
	BOOL bProcessed = FALSE;
	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
		if(lInData & (1<<nTrack)) {
			if( (m_cActiveTracks & (1<<nTrack)) && 
				!m_pTracks[nTrack].m_pDoc[0]->m_bInspecting)
			{
				if(m_pTracks[nTrack].m_pDoc[0]->m_lDebugFlag & DEBUG_PRINT)
					OutputDebugLogTo(nTrack + 1,TRUE,"------------Start of Sequence[Track%d] Before Busy: ---------------: %d", nTrack+1,m_nOutData);
				m_pTracks[nTrack].m_pDoc[0]->m_bInspecting = TRUE;

				EnterCriticalSection(&m_csOutputData);
				int byte;
				byte = 0x3ff;
				byte ^= (1 << (nTrack * 2 + 1)); //reset result
				m_nOutData &= byte;

				m_nOutData ^= (1 << (nTrack * 2)); //busy signal
				if(pIoRes->lpOutPortWrite) {
				//	OutputDebugLogTo(0,FALSE,"Track%d is busy. Capture + Process...", nTrack);
					nError = pIoRes->lpOutPortWrite(
						m_lCOMMOutCardNo, m_lCOMMOutPortID, 
						m_lIOReverseLogic ? m_nOutData : m_nOutData ^= 0x3ff,0);		//(1<<DATA_READY_BIT_POS)^0xff
					OutputDebugLogTo(nTrack + 1,TRUE,"Track %d Busy signal: [capture + process]: %d", nTrack+1,m_nOutData);
				}
				LeaveCriticalSection(&m_csOutputData);
				for(int nDoc=0; nDoc<m_pTracks[nTrack].m_nTotalDocs; nDoc++)
					m_pTracks[nTrack].ComGrabInspectOnlineSimulation(nDoc);
				//strTemp.Format(" %d", nTrack);
				//str += strTemp;

				bProcessed = TRUE;
			}
		}
	}
	if(!bProcessed)
		Sleep(300);
	//OutputDebugLogTo(m_nTotalTracks+2, TRUE, str.GetBuffer(0));
	//str.ReleaseBuffer();

	return 0;
}

int CApplication::SavePresentationPassImages(int nTrack, int nError)
{
	CString strFailedStr, strFailedFileName, strPassFileName;
	CString strTime,strYear,strDay,strMonth,strHr,strMin,strSec;

	char strCurTime[] = {"            "};

	SYSTEMTIME sysCurTime;
	GetLocalTime(&sysCurTime);	
	/////////////

	strYear.Format("%d",sysCurTime.wYear);
	strYear = strYear.Right(2);

	if(sysCurTime.wMonth < 10)
		strMonth.Format("0%d",sysCurTime.wMonth);
	else
		strMonth.Format("%d",sysCurTime.wMonth);

	if(sysCurTime.wDay < 10)
		strDay.Format("0%d",sysCurTime.wDay);
	else
		strDay.Format("%d",sysCurTime.wDay);

	if(sysCurTime.wHour < 10)
		strHr.Format("0%d", sysCurTime.wHour);
	else
		strHr.Format("%d", sysCurTime.wHour);

	if(sysCurTime.wMinute < 10)
		strMin.Format("0%d", sysCurTime.wMinute);
	else
		strMin.Format("%d", sysCurTime.wMinute);

	if(sysCurTime.wSecond < 10)
		strSec.Format("0%d", sysCurTime.wSecond);
	else
		strSec.Format("%d", sysCurTime.wSecond);
	
	sprintf_s(strCurTime,"%s%s%s%s%s%s",strYear,strMonth,strDay,strHr,strMin,strSec);

	///Udupa; TODO
	//if(m_bLogDefImages && m_nSaveImgSelection){
	//		
	//	strPassFileName.Format("%s\\%s_%s_1_0.bmp", m_strPresentPassDirTrk[nTrack],m_strMachineID,strCurTime);

	//	SaveGrayScaleBMPFile(strPassFileName, m_pTracks[nTrack].m_pDoc[0]->m_Buffer);
	//}

	return 0;
}

int CApplication::UpdateResults()
{
	OutputDebugLogTo(7, TRUE, "UpdateResults()");

	COLORREF m_colorTemp = RGB(125, 125, 120);
	BOOL bTempVar = FALSE;
	BOOL bTempVarFov = FALSE;

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfDies = nDevicePerX * nDevicePerY;
	int m_nNoOfFov = nFovPerX * nFovPerY;

	for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
		m_nPartErrorCode[nDie] = 98765;
	}
	for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
		for (int nFOV = 0; nFOV < m_nNoOfFov; nFOV++) {
			m_colorFovDieMapping[nDie][nFOV].DeleteObject();
		}
	}

	if (m_bByPassResult) {
		for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
			for (int nFOV = 0; nFOV < m_nNoOfFov; nFOV++) {
				for (int nView = 0; nView < m_nTotalViews; nView++) {
					m_nErrorCode[nDie][nFOV][nView] = GOOD_UNIT;
				}
			}
		}
	}


	for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
		for (int nFOV = 0; nFOV < m_nNoOfFov; nFOV++) {
			for (int nView = 0; nView < m_nTotalViews; nView++) {
				if(m_nErrorCode[nDie][nFOV][nView] != NOT_INSPECTED && ( m_nFovPartErrorCode[nDie][nFOV] == GOOD_UNIT || m_nFovPartErrorCode[nDie][nFOV] == NOT_INSPECTED))
					m_nFovPartErrorCode[nDie][nFOV] = -m_nErrorCode[nDie][nFOV][nView];
			}
		}
	}

	int nTempViewErrorCode=98765;
	int nTempDieErrorCodeStats = GOOD_UNIT;
	int nTempViewFovErrorCode = 98765;
	int nPriorityErrorCode = m_nDefectErrorCodesPriority.GetCount();
	int nFovPriorityErrorCode = m_nDefectErrorCodesPriority.GetCount();
	for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
		nTempViewErrorCode = 98765;
		bTempVar = FALSE;
		nPriorityErrorCode = m_nDefectErrorCodesPriority.GetCount();
		for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
			nTempViewFovErrorCode = 98765;
			bTempVarFov = FALSE;
			nFovPriorityErrorCode = m_nDefectErrorCodesPriority.GetCount();
			if (m_bPartInspect[nDie]) {
				for (int nView = 0; nView < m_nTotalViews; nView++) {

					if (m_nErrorCode[nDie][nFov][nView] < GOOD_UNIT && m_nErrorCode[nDie][nFov][nView] != NOT_INSPECTED) {
						if (m_nErrorCode[nDie][nFov][nView] != ERROR_PROCESS) {
							if (m_mapDefectErrorCodesPriority.find(m_nErrorCode[nDie][nFov][nView]) != m_mapDefectErrorCodesPriority.end()) {
								//Update defect code for FOV level
								if (m_mapDefectErrorCodesPriority[m_nErrorCode[nDie][nFov][nView]] < nFovPriorityErrorCode) {
									nFovPriorityErrorCode = m_mapDefectErrorCodesPriority[m_nErrorCode[nDie][nFov][nView]];
									nTempViewFovErrorCode = m_nErrorCode[nDie][nFov][nView];
									bTempVarFov = TRUE;
								}
								//Update defect code for Die level
								if (m_mapDefectErrorCodesPriority[m_nErrorCode[nDie][nFov][nView]] < nPriorityErrorCode) {
									nPriorityErrorCode = m_mapDefectErrorCodesPriority[m_nErrorCode[nDie][nFov][nView]];
									nTempDieErrorCodeStats = m_nErrorCode[nDie][nFov][nView];
									nTempViewErrorCode = m_nErrorCode[nDie][nFov][nView];
									bTempVar = TRUE;
								}
							}
						}
						else
						{
							/*m_colorTemp = m_pDefectCode[10].colorDefect;
							nTempViewErrorCode = m_pDefectCode[10].nErrorCode;
							nTempDieErrorCodeStats = m_nErrorCode[nDie][nFov][nView];
							nTempViewFovErrorCode = m_pDefectCode[10].nErrorCode*/;

							m_colorTemp = theApp.m_pImageSelectionDlg->m_wndDlg.GetDefectCode(UNKNOWN_DEFECT).color;
							nTempViewErrorCode = UNKNOWN_DEFECT;
							nTempDieErrorCodeStats = m_nErrorCode[nDie][nFov][nView];
							nTempViewFovErrorCode = UNKNOWN_DEFECT;
						}
					}
					else {
						if (m_nErrorCode[nDie][nFov][nView] != NOT_INSPECTED) {
							//Die Status level
							if (!bTempVar) {
								nTempViewErrorCode = GOOD_UNIT;/*m_pDefectCode[0].nErrorCode;*/
								nTempDieErrorCodeStats = m_nErrorCode[nDie][nFov][nView];
							}
							//Fov status level
							if (!bTempVarFov) {
								m_colorTemp = theApp.m_pImageSelectionDlg->m_wndDlg.GetDefectCode(GOOD_UNIT).color;/*m_pDefectCode[0].colorDefect;*/
								nTempViewFovErrorCode = GOOD_UNIT;/*m_pDefectCode[0].nErrorCode;*/
							}
						}
						else {
							//Fov status level
							if (!bTempVarFov) {
								m_colorTemp = theApp.m_pImageSelectionDlg->m_wndDlg.GetDefectCode(NOT_INSPECTED).color;
								nTempViewFovErrorCode = NOT_INSPECTED;
							}
							//Die Status level
							if (!bTempVar) {
								nTempViewErrorCode = NOT_INSPECTED;/*m_pDefectCode[0].nErrorCode;*/
								nTempDieErrorCodeStats = m_nErrorCode[nDie][nFov][nView];
							}
						}
					}
					//UpdateLotSummary(nDie, nFov, nView, m_nErrorCode[nDie][nFov][nView]);
				}
				m_nFovPartErrorCode[nDie][nFov] = nTempViewFovErrorCode;				//Girish, Need to check with Nguyen --> Nguyen: Ok checked haha
				m_colorFovDieMapping[nDie][nFov].CreateSolidBrush(m_colorTemp);
			}
			else {
//				m_nFovPartErrorCode[nDie][nFov] = nTempViewFovErrorCode;
				m_colorFovDieMapping[nDie][nFov].CreateSolidBrush(RGB(125, 125, 120));
			}
		}
		m_nPartErrorCode[nDie] = nTempViewErrorCode;
		m_nDieErrorCodeStats[nDie] = nTempDieErrorCodeStats;
	}

	OutputDebugLogTo(7, TRUE, "UpdateResults(): Updated die fail code based on priority defect");


	BOOL bInspectionError = FALSE;
	for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
		for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
			for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
				for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {

					if (!theApp.motionParameters.IsGrabEnabled(nTrack, nDoc, nFov))
						continue;

					int nView;
					theApp.motionParameters.GetViewID(nTrack, nDoc, nView);

					m_pTracks[nTrack].m_pDoc[nDoc]->SaveDocImage(nTrack, nDoc, nDie, nFov, m_nPartErrorCode[nDie]);
					m_pTracks[nTrack].m_pDoc[nDoc]->SaveDocDLImages(nTrack, nDoc, nDie, nFov, m_nErrorCode[nDie][nFov][nView]);

				}
			}
		}

		if (m_nPartErrorCode[nDie] != GOOD_UNIT && m_bPartInspect[nDie]) {
			bInspectionError = TRUE;
		}
	}

	OutputDebugLogTo(7, TRUE, "UpdateResults(): Completed push image for saving image thread");

	((CMainFrame*)m_pMainWnd)->m_dlgPane->m_wndDlg.Invalidate();

	OutputDebugLogTo(7, TRUE, "UpdateResults(): Completed update mapping result");

	if ((m_nVerificationMode == 1 && (bInspectionError || m_bReScanSequence)) || m_nVerificationMode == 2) {
		if (m_bCentralizedVerificationMode && !m_CentralizedCommInterface.m_bIsTCPServer) {
			//m_pImageQueue->PushResultCentralized();
			//Just waiting till Server received last image and asked for mapping result
		}
		else {
			m_bShowDieVerification = TRUE;
			((CMainFrame*)m_pMainWnd)->m_dlgPane->m_wndDlg.EnableVerification(m_bShowDieVerification);
		}
	}
	else {
		m_bShowDieVerification = FALSE;
		((CMainFrame*)m_pMainWnd)->m_dlgPane->m_wndDlg.EnableVerification(m_bShowDieVerification);
		for (int nDie = 0; nDie <m_nNoOfDies; nDie++) {
			for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
				for (int nView = 0; nView < m_nTotalViews; nView++) {

					if(m_bPartInspect[nDie])
						UpdateLotSummary(nDie, nFov, nView, m_nErrorCode[nDie][nFov][nView]);
				}
			}
			if (m_bPartInspect[nDie])
				UpdateTotalLotSummary(nDie, m_nDieErrorCodeStats[nDie]);
		}
		UpdateStatisticsWindow();

		if(m_bReceivedStartCmd)
			SetEvent(m_EventCommSendMsg);
	}
	UpdateStatistics(0);

	//Send Image To Server for Centralized Reject Images
	if (m_bCentralizedVerificationMode && !m_CentralizedCommInterface.m_bIsTCPServer) {
		BOOL bMappingReady = TRUE;
		m_pImageQueue->PushImageBufferCentralized(&m_pTracks[0].m_pDoc[0]->m_BufferDie[0][0], 0, 0, 0, 0, bMappingReady);

		if (!m_bIsFailPart) {
			//Send Info (Include Stats Information)
			m_CentralizedCommInterface.m_pCommManager[0]->SendInfo(INFOACK);
		}
	}

	if (m_bStream)
		m_bStream = FALSE;

	OutputDebugLogTo(7, TRUE, "UpdateResults(): Completed send result to host");

	return 0;
}

void CApplication::LoadImagesForVerfication(CString Path, int nDieNo)
{

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;

	CString m_strImageFolders, strTemp;
	m_strImageFolders.Format("%s\\%s", Path, m_strMachineID);
	_mkdir(m_strImageFolders);

	CString strFolder, strRecipeFolder, strImage;
	strRecipeFolder.Format("%s\\%s", m_strImageFolders, m_strConfigFile);
	//	strTemp.Format("%s\\%s", strRecipeFolder, m_pApp->m_strLotInfo);
	/*BOOL bPass = !IS_FAIL(pBuf->m_nErrorCode);
	if (!bPass)
	strFolder.Format("%s\\FAIL", strTemp);
	else
	strFolder.Format("%s\\PASS", strTemp);*/
	//	_mkdir(strFolder);

	strImage.Format("%s\\%s", strRecipeFolder, m_strPartFlex[nDieNo]);

	CString strImageName, strPos;
	OutputDebugLogTo(3, TRUE, "Image Read Path: %s", strImage);

	GetImageFilePaths(strImage + "\\", m_strLoadMultiImagesArray);
//	strImageName = m_strLoadMultiImagesArray.GetAt(0);
	
	INT_PTR nFileCount = m_strLoadMultiImagesArray.GetCount();
	CString strFilePath;

//	for (int i = 0; i < nFileCount; i++) {
	if (nFileCount > 0) {
		strFilePath = m_strLoadMultiImagesArray.GetAt(0);
		for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
			for (int y = 0; y < m_nTotalTracks; y++) {
				int nError = m_pTracks[y].m_pDoc[0]->LoadImagesToDocForInspection(strFilePath, nFov);
			}
		}
	}
//	}

	//
	//int nTempLen, nTempSel;
	//int nPos = 0;
	//strPos = strImageName.Mid(strImageName.ReverseFind('P'), 4);
	//nTempLen = strPos.GetLength();
	//m_pTrack->m_charTempPosNameForInsp = strPos[nTempLen - 1];
	//nTempSel = atoi(&m_pTrack->m_charTempPosNameForInsp);
	//int nFovTemp = nTempSel - 1;

	//CString strDevice = strImageName.Mid(strImageName.ReverseFind('T')); //JY
	//nPos = strDevice.Find('_');
	//strImageName = strDevice.Mid(0, nPos);
	//m_pTrack->m_charTempPosNameForInsp = strDevice[nPos - 1];
	//nTempSel = atoi(&m_pTrack->m_charTempPosNameForInsp);
	//int nDeviceTemp = nTempSel - 1;
	//((CMainFrame*)m_pApp->m_pMainWnd)->m_dlgPane->m_wndDlg.EnableDieRect(0, nDeviceTemp);

}

int CApplication::GetImageFilePaths(CString strStartFolder, CStringArray& arrayFilePaths)
{
	CString strFilePath;
	strFilePath.Format("%s%s", strStartFolder, "*.*");
	CString strFolder, strFile;

	CFileFind FileFind;
	BOOL bNextFile = FileFind.FindFile(strFilePath);
	while (bNextFile) {
		bNextFile = FileFind.FindNextFile();
		if (FileFind.IsDots())
			continue;

		strFile = FileFind.GetFileName();
		if (FileFind.IsDirectory()) {
			strFolder.Format("%s%s\\", strStartFolder, strFile);
			GetImageFilePaths(strFolder, arrayFilePaths);
		}
		else if (!strFile.Right(3).CompareNoCase("bmp"))
			arrayFilePaths.Add(strStartFolder + strFile);
	}
	return 0;
}

void CApplication::ClearResults()
{

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	theApp.motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;
	int m_nNoOfDies = nDevicePerX * nDevicePerY;

	int nTemp = 0;
	for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
		for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
			m_nFovPartErrorCode[nDie][nFov] = NOT_INSPECTED;
			for (int nView = 0; nView < m_nTotalViews; nView++) {
				m_nErrorCode[nDie][nFov][nView] = NOT_INSPECTED;
			}
		}
	}


	((CMainFrame*)m_pMainWnd)->m_dlgPane->m_wndDlg.Invalidate();
}

int CApplication::UpdateResultsOffline(int nDie,int nFov,BOOL bTotalSummary)
{
//	COLORREF m_colorTemp = RGB(125, 125, 120);
	BOOL bTempVar = FALSE;
	BOOL bTempVarFov = FALSE;

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfDies = nDevicePerX * nDevicePerY;
	int m_nNoOfFov = nFovPerX * nFovPerY;

	if (nFov >= 0) {
		m_bDieInspection[nDie][nFov] = FALSE;
		m_nFovPartErrorCode[nDie][nFov] = NOT_INSPECTED;
	}
		
	for (int nView = 0; nView < m_nTotalViews; nView++) {
		if (m_nErrorCode[nDie][nFov][nView] != NOT_INSPECTED && (m_nFovPartErrorCode[nDie][nFov] == GOOD_UNIT || m_nFovPartErrorCode[nDie][nFov] == NOT_INSPECTED))
			m_nFovPartErrorCode[nDie][nFov] = m_nErrorCode[nDie][nFov][nView];
	}
	
	
	int nTempViewErrorCode = 98765;
	int nTempDieErrorCodeStats = GOOD_UNIT;
	int nTempViewFovErrorCode = 98765;
	int nPriorityErrorCode = m_nDefectErrorCodesPriority.GetCount();
	int nFovPriorityErrorCode = m_nDefectErrorCodesPriority.GetCount();
	nTempViewErrorCode = 98765;
	bTempVar = FALSE;
	nPriorityErrorCode = m_nDefectErrorCodesPriority.GetCount();

	if (bTotalSummary && nFov==-1) {
		for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
			nTempViewFovErrorCode = 98765;
			bTempVarFov = FALSE;
			nFovPriorityErrorCode = m_nDefectErrorCodesPriority.GetCount();

			int nStartView = 0, nEndView = m_nTotalViews;
			if (!m_bSetForAllTracks) {
				if (m_nActiveTrackIdx == 0) {
					nStartView = 0;
					nEndView = m_nTop;
				}
				else if (m_nActiveTrackIdx == 1) {
					nStartView = m_nTop;
					nEndView = m_nTop + m_nBottom;
				}
				else {
					nStartView = m_nTop + m_nBottom;
					nEndView = m_nTotalViews;
				}
			}

			for (int nView = 0; nView < m_nTotalViews; nView++) {
				if (m_nErrorCode[nDie][nFov][nView] < GOOD_UNIT) {
					if (m_mapDefectErrorCodesPriority.find(m_nErrorCode[nDie][nFov][nView]) != m_mapDefectErrorCodesPriority.end()) {
						if (m_mapDefectErrorCodesPriority[m_nErrorCode[nDie][nFov][nView]] < nPriorityErrorCode) {
							nPriorityErrorCode = m_mapDefectErrorCodesPriority[m_nErrorCode[nDie][nFov][nView]];

							nTempDieErrorCodeStats = m_nErrorCode[nDie][nFov][nView];
							bTempVar = TRUE;
						}
					}
				}
				else {
					if (!bTempVar) {
						nTempDieErrorCodeStats = m_nErrorCode[nDie][nFov][nView];
					}
				}
			}
		}

		m_nPartErrorCode[nDie] = nTempDieErrorCodeStats;
		/*m_bShowDieVerification = TRUE;
		((CMainFrame*)m_pMainWnd)->m_dlgPane->m_wndDlg.EnableVerification(m_bShowDieVerification);*/

		if (m_bCentralizedVerificationMode && !m_CentralizedCommInterface.m_bIsTCPServer) {
			//m_pImageQueue->PushResultCentralized();
			//Just waiting till Server received last image and asked for mapping result
		}

			

		for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
			for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
				for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
					for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {

						if (!theApp.motionParameters.IsGrabEnabled(nTrack, nDoc, nFov))
							continue;

						int nView;
						theApp.motionParameters.GetViewID(nTrack, nDoc, nView);

						m_pTracks[nTrack].m_pDoc[nDoc]->SaveDocDLImages(nTrack, nDoc, nDie, nFov, m_nErrorCode[nDie][nFov][nView]);

					}
				}
			}
		}

		UpdateTotalLotSummary(nDie, nTempDieErrorCodeStats);
	}
	else {
		int nStartView = 0, nEndView = m_nTotalViews;
		if (!m_bSetForAllTracks) {
			if (m_nActiveTrackIdx == 0) {
				nStartView = 0;
				nEndView = m_nTop;
			}
			else if (m_nActiveTrackIdx == 1) {
				nStartView = m_nTop;
				nEndView = m_nTop + m_nBottom;
			}
			else {
				nStartView = m_nTop + m_nBottom;
				nEndView = m_nTotalViews;
			}
		}
		for (int nView = nStartView; nView < nEndView; nView++) {
			if (m_nErrorCode[nDie][nFov][nView] < GOOD_UNIT && m_nErrorCode[nDie][nFov][nView] != NOT_INSPECTED) {
				if (m_mapDefectErrorCodesPriority.find(m_nErrorCode[nDie][nFov][nView]) != m_mapDefectErrorCodesPriority.end()) {
					if (m_mapDefectErrorCodesPriority[m_nErrorCode[nDie][nFov][nView]] < nFovPriorityErrorCode) {
						nFovPriorityErrorCode = m_mapDefectErrorCodesPriority[m_nErrorCode[nDie][nFov][nView]];

						nTempViewFovErrorCode = m_nErrorCode[nDie][nFov][nView];
						bTempVarFov = TRUE;
					}
				}
			}
			else {
				if (!bTempVarFov) {
					if(m_nErrorCode[nDie][nFov][nView] != NOT_INSPECTED)
						nTempViewFovErrorCode = GOOD_UNIT;
					else
						nTempViewFovErrorCode = NOT_INSPECTED;
				}
			}
		}
		
		m_nFovPartErrorCode[nDie][nFov] = nTempViewFovErrorCode;
		/*m_colorFovDieMapping[nDie][nFov].DeleteObject();
		m_colorFovDieMapping[nDie][nFov].CreateSolidBrush(m_colorTemp);*/

//		for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
			for (int nView = 0; nView < m_nTotalViews; nView++) {
				UpdateLotSummary(nDie, nFov, nView, m_nErrorCode[nDie][nFov][nView]);
			}
//		}
	}
	((CMainFrame*)m_pMainWnd)->m_dlgPane->m_wndDlg.Invalidate();
	
	
	UpdateStatisticsWindow();
	return 0;
}

void CApplication::UpdateClientMachineIDPanel(CString strClientMachineId)
{
	if(m_pDispOnlinePanel != NULL)
		m_pDispOnlinePanel->GetDlgItem(IDC_ONLINE_LOT_ID_STATIC)->SetWindowTextA(strClientMachineId);
}

int CApplication::SetVisOnLineState(BOOL bStatus)
{
	if(!m_bExitApp){
		m_pMainWnd->SendMessageA(WM_SET_ONLINE_STATUS);
	}

	CString strMsg;
	int nError;
	CIOInterface* pIoRes = &m_CommModule;

	if (bStatus) {
		UpdateStatusBar(&CString(""), 2);
		UpdateStatusBar(&CString(""), 3);
		/*if(m_lIOReverseLogic)
			m_nOutData = 0x00;
		else
			m_nOutData = 0xFF;*/

		m_bLoadMultiImagesFromFile = FALSE;

		if(m_bIsCameraMonitorMode) {
			for(int i=0; i<m_nTotalTracks; i++) {
				m_pTracks[i].CancelSnap();
				SetGrabTimeout(&m_pTracks[i].m_FGResource, INFINITE);
			}
		}
		/*for(int i=0; i<m_nTotalTracks; i++)
			m_pTracks[i].m_pDoc[0]->m_strLoadImage = "";

		if (pIoRes->lpOutPortWrite) {
			nError = pIoRes->lpOutPortWrite(
			m_lCOMMOutCardNo, m_lCOMMOutPortID, m_nOutData, 0);
			if (nError)
				return -1;
		}
		*/		
		m_pDispOnlinePanel->GetDlgItem(IDC_ONLINE_MACHINE_ID_STATIC)->SetWindowTextA(m_strMachineID);

		if (m_CentralizedCommInterface.m_bIsTCPServer) {
		}
		else
			m_pDispOnlinePanel->GetDlgItem(IDC_ONLINE_LOT_ID_STATIC)->SetWindowTextA(m_strLotInfo);

		m_pDispOnlinePanel->GetDlgItem(IDC_ONLINE_RECEIPE_STATUS_STATIC)->SetWindowTextA(m_strLotConfigFile);
		//m_pDispOnlinePanel->m_strMachineId = m_strMachineID;
		//m_pDispOnlinePanel->m_strRecipe = m_strConfigFile;
		CRect rect1, rect2;
		SystemParametersInfo(SPI_GETWORKAREA, NULL, &rect1, 0);
		m_pDispOnlinePanel->GetWindowRect(&rect2);

		m_pDispOnlinePanel->SetWindowPos(NULL, 80,0, rect2.Width(), rect2.Height(), SWP_NOSIZE);

		m_pDispOnlinePanel->ShowWindow(TRUE);

		if(m_bTrackInspSeq)
			m_bTrackInspSeq = FALSE;
		else {
			m_bStream = FALSE;
			InitializeLotStats();
			if (m_bLotOpened)
			{
				//if (m_strLotConfigFile.Compare(m_strConfigFile) != 0) {
				//	m_strConfigFile = m_strLotConfigFile;
				//	LoadRecipes();
				//}
				AddLogEvent("Loading Lot Info");
				((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.LoadLotInfo();
				((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.LoadTotalLotInfo();
				UpdateStatisticsWindow();
				AddLogEvent("Info Loaded");
			}
			SetEvent(m_eventOnline);
			SetEvent(m_EventMotionSensor);
			m_pMainWnd->SendMessageA(WM_SET_FULLSCREEN, m_bOnline, TRUE);
			OutputDebugLog("SetEvent(m_eventOnline)");
			/*SetEvent(m_eventVisionReady);
			UpdateTrackStatus();*/
		}
		ApplyDebugFlag(DEBUG_DRAW|DEBUG_PRINT|DEBUG_TIME|DEBUG_SAVE_BAD_IMAGE);

		
		AddLogEvent("Vision ONLINE");
	}
	else {
		OutputDebugLog("ResetEvent(m_eventOnline)");
		ResetEvent(m_eventOnline);
		ResetEvent(m_EventMotionSensor);

		m_pDispOnlinePanel->ShowWindow(FALSE);
		
		if(m_lIOReverseLogic)
			m_nOutData = 0x00;
		else
			m_nOutData = 0xFF;
		if (pIoRes->lpOutPortWrite) {
			nError = pIoRes->lpOutPortWrite(
				m_lCOMMOutCardNo, m_lCOMMOutPortID,  m_nOutData,0);
			if (nError)
				return -1;
		}
		ApplyDebugFlag(DEBUG_DRAW|DEBUG_PRINT|DEBUG_TIME);
		
		
		if(m_bIsCameraMonitorMode) {
			for(int i=0; i<m_nTotalTracks; i++) {
				m_pTracks[i].CancelSnap();
				SetGrabTimeout(&m_pTracks[i].m_FGResource, m_nGrabTimeout);
			}
		}
		AddLogEvent("Vision OFFLINE");
	}

	if(!m_bExitApp) {
		m_pMainWnd->SendMessageA(WM_SET_FULLSCREEN,m_bOnline, TRUE);
		//for(int i=0; i<m_nTotalTracks; i++)
		//	m_pTracks[i].ZoomAllViews(ZOOMFIT);
	}
	if(m_bAllocFailedImageBuffer) {
		for(int i=0;i<MAX_TRACKS;i++) {
			if(m_nFailImageCounter[i] >= 25)
				m_nFailImageCounter[i] = 0;
		}
	}

	((CMainFrame*)m_pMainWnd)->m_dlgPane->EnableGripper(!bStatus);
	((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->EnableGripper(!bStatus);
	((CMainFrame*)m_pMainWnd)->m_wndOutputPane->EnableGripper(!bStatus);

	//SetTrigMode(&m_pTracks[0].m_FGResource, bStatus ? TRIG_HARD : TRIG_SOFT);
	//if(!bStatus)
	//	m_pTracks[0].CancelSnap();

	return 0;
}

BOOL SetPrivilege(HANDLE hToken,LPCTSTR lpszPrivilege,BOOL bChange)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;
	BOOL bReturnValue = FALSE;

	if (lpszPrivilege != NULL && !bChange)
	{
		if (LookupPrivilegeValue( 
			NULL,            // lookup privilege on local system
			lpszPrivilege,   // privilege to lookup 
			&luid )) 
		{      // receives LUID of privilege
			tp.PrivilegeCount = 1;
			tp.Privileges[0].Luid = luid;
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		}
	}

	AdjustTokenPrivileges(hToken,bChange,&tp,
		sizeof(TOKEN_PRIVILEGES), 
		(PTOKEN_PRIVILEGES) NULL,
		(PDWORD) NULL);  
	// Call GetLastError to determine whether the function succeeded.
	
	if (GetLastError() == ERROR_SUCCESS) 
	{ 
		bReturnValue = TRUE;
	} 
	 
	return bReturnValue;
} 

BOOL TakeOwnership(LPCTSTR lpszFile)
{
	int file[256];
	char error[256];
	DWORD description;
	SECURITY_DESCRIPTOR sd;
	SECURITY_INFORMATION info_owner=OWNER_SECURITY_INFORMATION;
		
	TOKEN_USER *owner = (TOKEN_USER*)file;
	HANDLE token;
	
	
	InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
	if(OpenProcessToken(GetCurrentProcess(),TOKEN_READ | TOKEN_ADJUST_PRIVILEGES,&token))
	{
		//To Get the Current Process Token & opening it to adjust the previleges
		if(SetPrivilege(token,SE_TAKE_OWNERSHIP_NAME,FALSE))
		{
			//Enabling the privilege
			if(GetTokenInformation(token,TokenUser,owner,sizeof(file),&description))
			{
				//Get the information on the opened token
				if(SetSecurityDescriptorOwner(&sd,owner->User.Sid,FALSE))
				{
					//replace any owner information present on the security descriptor
					if(SetFileSecurity(lpszFile,info_owner,&sd))
						return(TRUE);
					else
					{
						// Call GetLastError to determine whether the function succeeded.
						sprintf_s(error,"Error in SetFileSecurity Error No : %d",GetLastError());
						MessageBox(NULL,error,NULL,MB_OK);
					}//SetFileSecurity
				}
				else
				{
					sprintf_s(error,"Error in SetSecurityDescriptorOwner Error No : %d",GetLastError());
					MessageBox(NULL,error,NULL,MB_OK);
				}//SetSecurityDescriptorOwner
			}
			else
			{
				sprintf_s(error,"Error in GetTokenInformation Error No : %d",GetLastError());
				MessageBox(NULL,error,NULL,MB_OK);
			}//GetTokenInformation
		}
		else
		{
			sprintf_s(error,"Error in SetPrivilege No : %d",GetLastError());
			MessageBox(NULL,error,NULL,MB_OK);
		}//SetPrivilege
	}
	else
	{
		sprintf_s(error,"Error in OpenProcessToken No : %d",GetLastError());
		MessageBox(NULL,error,NULL,MB_OK);
	}//OpenProcessToken

	SetPrivilege(token,SE_TAKE_OWNERSHIP_NAME,TRUE);//Disabling the set previlege

	return(FALSE);
}




BOOL SetPermission(LPCTSTR lpszFile, LPCTSTR lpszAccess, DWORD dwAccessMask)
{
	int buff[512];
	char domain[512];
	char error[256];
	
	DWORD domain_size=sizeof(domain);
	DWORD acl_size;

	SECURITY_DESCRIPTOR sd1;
	SECURITY_INFORMATION info_dacl=DACL_SECURITY_INFORMATION;
	PSID sid = (PSID*)buff;
	ACL *acl;
	SID_NAME_USE sidname;
	DWORD sid_size=sizeof(buff);
	
	InitializeSecurityDescriptor(&sd1,SECURITY_DESCRIPTOR_REVISION);
	//to get the SID 
	if(LookupAccountName(NULL,lpszAccess,sid,&sid_size,domain,&domain_size,&sidname))
	{
		acl_size=sizeof(ACL)+sizeof(ACCESS_ALLOWED_ACE)-sizeof(DWORD)+GetLengthSid(buff);
		acl = (ACL *)malloc(acl_size);
		//To calculate the size of an ACL, 
		InitializeAcl(acl,acl_size,ACL_REVISION);

		if(AddAccessAllowedAce(acl,ACL_REVISION,dwAccessMask,sid))
		{
			if(SetSecurityDescriptorDacl(&sd1,TRUE,acl,FALSE))
			{
				if(SetFileSecurity(lpszFile,info_dacl,&sd1))
					return(TRUE);
				else
				{
					sprintf_s(error,"Error in SetFileSecurity Error No : %d",GetLastError());
					MessageBox(NULL,error,NULL,MB_OK);
				}//SetFileSecurity
			}
			else
			{
				sprintf_s(error,"Error in SetSecurityDescriptorDacl Error No : %d",GetLastError());
				MessageBox(NULL,error,NULL,MB_OK);
			}//SetSecurityDescriptorDacl
		}
		else
		{
			sprintf_s(error,"Error in AddAccessAllowedAce Error No : %d",GetLastError());
			MessageBox(NULL,error,NULL,MB_OK);
		}//AddAccessAllowedAce
	}
	else
	{
		sprintf_s(error,"Error in LookupAccountName No : %d",GetLastError());
		MessageBox(NULL,error,NULL,MB_OK);
	}//LookupAccountName

	free(acl);
	return(FALSE);
}

int DeleteDir(LPCTSTR lpszName)
{
	char lpszNameTemp[MAX_PATH];
	memset(lpszNameTemp, NULL, MAX_PATH);
	sprintf(lpszNameTemp, "%s", lpszName);

	SHFILEOPSTRUCT fileOpStruct;
	fileOpStruct.hwnd = theApp.m_pMainWnd->m_hWnd;
	fileOpStruct.wFunc = FO_DELETE;
	fileOpStruct.pFrom = lpszNameTemp;
	fileOpStruct.pTo = NULL;
	fileOpStruct.fFlags = FOF_NOCONFIRMATION  | FOF_SILENT;

	int nError = SHFileOperation(&fileOpStruct);

	return nError;
}

//	int ret = -1;
//	char name1[256];
//	WIN32_FIND_DATA info;
//    HANDLE hp;
////    int n;
//    char *cp;
//	
//	sprintf_s(name1, "%s\\*.*",lpszName);
//    hp = FindFirstFile(name1,&info);
//    if(!hp || hp==INVALID_HANDLE_VALUE)
//        return ret;
//    do
//    {
//		cp = info.cFileName;
//        if(cp[1]==0 && *cp=='.')
//            continue;
//        else if(cp[2]==0 && *cp=='.' && cp[1]=='.')
//            continue;
//        sprintf_s(name1,"%s\\%s",lpszName,info.cFileName);
//		if(info.dwFileAttributes&FILE_ATTRIBUTE_READONLY)
//		{
//			SetFileAttributes(name1,info.dwFileAttributes&~FILE_ATTRIBUTE_READONLY);
//		}
//		if(info.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
//		{
//			TakeOwnership(name1);
//			SetPermission(name1,"everyone",GENERIC_ALL);
//			DeleteDir(name1);
//		}
//		else
//		{
//			TakeOwnership(name1);
//			SetPermission(name1,"everyone",GENERIC_ALL);
//			DeleteFile(name1);
//		}
//    }
//    while(FindNextFile(hp,&info));
//	FindClose(hp);
//	if(info.dwFileAttributes&FILE_ATTRIBUTE_READONLY)
//	{
//		SetFileAttributes(lpszName,info.dwFileAttributes&~FILE_ATTRIBUTE_READONLY);
//	}
//	if(RemoveDirectory(lpszName))
//	{
//		OutputDebugLog("Delete Directory success");
//		ret = 0;
//	}	
//	else
//	{
//		OutputDebugLog("error %d\n",GetLastError());
//	}
//	return ret;
//}

void CApplication::UpdateCentralizedSummary(int nStationId)
{
	if (((CMainFrame*)m_pMainWnd)->m_dlgPaneStats != NULL) {
		((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.UpdateCentralizedSummary(nStationId);
	}
}

//void CApplication::UpdateCentralizedMachineState(int nStationId, CString strMachineState)
//{
//	if (((CMainFrame*)m_pMainWnd)->m_dlgPaneStats != NULL) {
//		m_CentralizedCommInterface.m_pCommManager[nStationId]->m_CentralizedSummary.strMachineState = strMachineState;
//		((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.UpdateCentralizedMachineState(nStationId);
//	}
//}

void CApplication::UpdateCentralizedMachineState(int nStationId, CString strMachineState, COLORREF colorState)
{
	if (((CMainFrame*)m_pMainWnd)->m_dlgPaneStats != NULL) {
		m_CentralizedCommInterface.m_pCommManager[nStationId]->m_CentralizedSummary.strMachineState = strMachineState;
		((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.UpdateCentralizedMachineState(nStationId, colorState);
		//((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.Invalidate();
	}
}

void CApplication::UpdateCentralizedMachineID(int nStationId)
{
	if(((CMainFrame*)m_pMainWnd)->m_dlgPaneStats != NULL)
		((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.UpdateCentralizedMachineID(nStationId);
}

void CApplication::UpdateStatisticsWindow(BOOL bRefresh)
{
	///Udupa; TODO
	//if(!m_bOnlineViewing)
	//{
	//	//if(m_pStatsView && ::IsWindow(m_pStatsView->m_ctrlDefectsList.m_hWnd))
	//		//m_pStatsView->UpdateContents();
	//}

	((CMainFrame*) m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.UpdateContents(FALSE);
	((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.UpdateTotalContents();
	((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.UpdateStatsInfo();
}

void CApplication::RecordLotStats()
{
	/*if(m_pStatsView)
		m_pStatsView->RecordLotStats();*/
	/*((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.RecordLotStats();
	((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.RecordLotStats(TRUE);*/
}

int CApplication::AddLogEvent(CString strEvent,int nEventLog,BOOL m_bAuditTrail)
{
	if (m_pMainWnd == NULL || ((CMainFrame*)m_pMainWnd)->m_dlgPaneStats == NULL)
		return -1;

	CString strEventLog;
	strEventLog.Format("     %s     %s      %s",m_UserAccInfo.m_strCurrentAccessLevel,m_UserAccInfo.m_strCurUserName, strEvent);
	m_pMainWnd->SendMessageA(WM_ADD_EVENT_LOG, (WPARAM)&strEventLog,nEventLog);
	
	if (nEventLog != COMM_LOG && m_bAuditTrail)
		return ((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.UpdateAuditTrail(strEvent, TRUE);
	else
		return 0;
	//return -1;
}



void CApplication::UpdateLotSummary(int nDevice,int nFov,int nView, int nError, BOOL allStats)
{
	/*TRACKSUMMARY* pDailyStats = &m_LotSummary.m_DailySummary[nDevice][nFov][nView];
	TRACKSUMMARY* pHourlyStats = &m_LotSummary.m_HourlySummary[nDevice][nFov][nView];*/

	if (nError == NOT_INSPECTED || nError == -NO_IMAGE)
		return;

	TRACKSUMMARY* pDisplayStats = &m_LotSummary.m_DisplaySummary[nDevice][nFov][nView];
	//if (m_bLotOpened && m_bOnline) // Update Lot Info Summary
	//if(allStats) {
	//	pDailyStats->lTotalUnitsInspected++;
	//	pHourlyStats->lTotalUnitsInspected++; 
	//}
	if(m_bDisplayAllStatsOffline || m_bOnline)
		pDisplayStats->lTotalUnitsInspected++; 

	if(nError==GOOD_UNIT){
	/*	if(allStats) {
			pDailyStats->lTotalUnitsPass++;
			pHourlyStats->lTotalUnitsPass++;
		}*/
		if(m_bDisplayAllStatsOffline || m_bOnline)
				pDisplayStats->lTotalUnitsPass++;
	}else { 
		/*if(allStats) {
			pDailyStats->lTotalUnitsFail++;
			pHourlyStats->lTotalUnitsFail++;
		}*/
		if(m_bDisplayAllStatsOffline || m_bOnline)
				pDisplayStats->lTotalUnitsFail++;
	}
	if(m_bDisplayAllStatsOffline || m_bOnline){
		pDisplayStats->dTotalYield = double(pDisplayStats->lTotalUnitsPass) /
										 double(pDisplayStats->lTotalUnitsInspected) * 100 + 0.04;
	}
	if(nError<GOOD_UNIT){
		switch (abs(nError)) {
			case IMAGE_SNAP_FAIL:
				break;
			case PROCESS_ERROR :
			case USER_ABORT :
			/*	if(allStats) {
					pDailyStats->lTotalUnitsInvalid++;
					pHourlyStats->lTotalUnitsInvalid++;
				}*/
				pDisplayStats->lTotalUnitsInvalid++;
				break;

			case INSP_FAIL_DOUBLE_LENS:
			case INSP_FAIL_NO_DEVICE:
			case DEVICE_LOCATION:
			/*	if(allStats) {
					pDailyStats->lDieLocation++;
					pHourlyStats->lDieLocation++;
				}*/
				pDisplayStats->lDieLocation++;
				break;

			case DATUM_LOCATION:
			/*	if(allStats){
					pDailyStats->lDatumLocation++;
					pHourlyStats->lDatumLocation++;
				}*/
				pDisplayStats->lDatumLocation++;
				break;

			case TILT_INSP:
				/*if(allStats) {
					pDailyStats->lTiltInsp++;
					pHourlyStats->lTiltInsp++;
				}*/
				pDisplayStats->lTiltInsp++;
				break;		

			case COVER_LAYER_INSP:
				/*if(allStats) {
					pDailyStats->lCoverLayerInspection++;
					pHourlyStats->lCoverLayerInspection++;
				}*/
				pDisplayStats->lCoverLayerInspection++;
				break;
			case COVER_LAYER_ALIGNMENT:
				/*if(allStats) {
				pDailyStats->lCoverLayerInspection++;
				pHourlyStats->lCoverLayerInspection++;
				}*/
				pDisplayStats->lCoverLayerAlignment++;
				break;
			case ENCAP_INSUFFICIENT://filp Lens
				/*if(allStats) {
					pDailyStats->lEncapInsufficient++;
					pHourlyStats->lEncapInsufficient++;
				}*/
				pDisplayStats->lEncapInsufficient++;
				break;
		
			case ENCAP_EXCESS:
				/*if(allStats) {
					pDailyStats->lEncapExcess++;
					pHourlyStats->lEncapExcess++;
				}*/
				pDisplayStats->lEncapExcess++;
				break;
			case ENCAP_CRACK:
				pDisplayStats->lEncapCrack++;
				break;
			case ENCAP_BLOWHOLE:
				/*if (allStats) {
					pDailyStats->lEncapBlowHole++;
					pHourlyStats->lEncapBlowHole++;
				}*/
				pDisplayStats->lEncapBlowHole++;
				break;

			case ENCAP_CONTAMINATION:
				/*if (allStats) {
					pDailyStats->lEncapContamination++;
					pHourlyStats->lEncapContamination++;
				}*/
				pDisplayStats->lEncapContamination++;
				break;
			case SLOT:
				/*if (allStats) {
				pDailyStats->lEncapContamination++;
				pHourlyStats->lEncapContamination++;
				}*/
				pDisplayStats->lSlot++;
				break;
			case DIE_EDGE:
				/*if(allStats) {
					pDailyStats->lDieEdge++;
					pHourlyStats->lDieEdge++;
				}*/
				pDisplayStats->lDieEdge++;
				break;
			case USER_MODIFIED:
				/*if (allStats) {
					pDailyStats->lUserModifiedFalseAcceptance++;
					pHourlyStats->lUserModifiedFalseAcceptance++;
				}*/
				pDisplayStats->lUserModifiedFalseAcceptance++;
				break;
			case USER_MODIFIED_FALSE_REJECTION:
				pDisplayStats->lUserModifiedFalseRejection++;
				break;
			default: {
				if (m_LotSummary.m_mapIndividualPviDefectCounter.find(nError) != m_LotSummary.m_mapIndividualPviDefectCounter.end()) {
					m_LotSummary.m_mapIndividualPviDefectCounter[nError]++;
				}
			}
				break;
		}
	}
}


void CApplication::UpdateTotalLotSummary(int nDevice, int nError, BOOL allStats)
{
	OutputDebugLogTo(9, TRUE, "UpdateTotalLotSummary 1 Device = %d,nError = %d",nDevice,nError);
	TRACKSUMMARY* pDisplayTotalStats = &m_LotSummary.m_TotalDisplaySummary[nDevice];
	//if (m_bLotOpened && m_bOnline)
	if (m_bDisplayAllStatsOffline || m_bOnline)
		pDisplayTotalStats->lTotalUnitsInspected++;

	if (nError == GOOD_UNIT) {
		if (m_bDisplayAllStatsOffline || m_bOnline)
			pDisplayTotalStats->lTotalUnitsPass++;
	}
	else {
		if (m_bDisplayAllStatsOffline || m_bOnline) {
			if (nError == -USER_MODIFIED_FALSE_REJECTION) {
				pDisplayTotalStats->lTotalUnitsPass++;
			}
			else {
				pDisplayTotalStats->lTotalUnitsFail++;
			}
		}
	}
	if (m_bDisplayAllStatsOffline || m_bOnline) {
		pDisplayTotalStats->dTotalYield = double(pDisplayTotalStats->lTotalUnitsPass) /
			double(pDisplayTotalStats->lTotalUnitsInspected) * 100 + 0.04;
	}


	if (nError<GOOD_UNIT) {
		switch (abs(nError)) {
			case IMAGE_SNAP_FAIL:
				break;
			case PROCESS_ERROR:
			case USER_ABORT:
				pDisplayTotalStats->lTotalUnitsInvalid++;
				break;

			case INSP_FAIL_DOUBLE_LENS:
			case INSP_FAIL_NO_DEVICE:
			case DEVICE_LOCATION:
				pDisplayTotalStats->lDieLocation++;
				break;

			case DATUM_LOCATION:
				pDisplayTotalStats->lDatumLocation++;
				break;

			case TILT_INSP:
				pDisplayTotalStats->lTiltInsp++;
				break;

			case COVER_LAYER_INSP:
				pDisplayTotalStats->lCoverLayerInspection++;
				break;
			case COVER_LAYER_ALIGNMENT:
				pDisplayTotalStats->lCoverLayerAlignment++;
				break;
			case ENCAP_INSUFFICIENT:
				pDisplayTotalStats->lEncapInsufficient++;
				break;

			case ENCAP_EXCESS:
				pDisplayTotalStats->lEncapExcess++;
				break;
			case ENCAP_CRACK:
				pDisplayTotalStats->lEncapCrack++;
				break;
			case ENCAP_BLOWHOLE:
				pDisplayTotalStats->lEncapBlowHole++;
				break;

			case ENCAP_CONTAMINATION:
				pDisplayTotalStats->lEncapContamination++;
				break;
			
			case SLOT:
				pDisplayTotalStats->lSlot++;
				break;

			case DIE_EDGE:
				pDisplayTotalStats->lDieEdge++;
				break;
			case USER_MODIFIED:
				pDisplayTotalStats->lUserModifiedFalseAcceptance++;
				break;
			case USER_MODIFIED_FALSE_REJECTION:
				pDisplayTotalStats->lUserModifiedFalseRejection++;
				break;
			default: {
				if (m_LotSummary.m_mapIndividualPviDefectCounter.find(nError) != m_LotSummary.m_mapIndividualPviDefectCounter.end()) {
					m_LotSummary.m_mapIndividualPviDefectCounter[nError]++;
				}
				break;
			}
		}
	}
}

void CApplication::LoadServerRecipes()
{
	EnterCriticalSection(&m_csLoadConfig);

	if (m_pImageSelectionDlg != NULL) {
		if (m_pProgressDlg != NULL) {
			int nLoadedDataPercentage = m_pProgressDlg->m_nCurrentPos;

			m_pProgressDlg->ShowWindow(TRUE);
			m_bProgressDlgOpen = TRUE;
			m_pProgressDlg->UpdateWindow();
			m_pProgressDlg->SetStatus("Loading Configuration....", nLoadedDataPercentage);
		}

		motionParameters.LoadDeviceParameters();
		int nDevicePerX, nDevicePerY;
		int nFovPerX, nFovPerY;
		motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
		int m_nNoOfFov = nFovPerX * nFovPerY;

		for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
			for (int nDoc = 0; nDoc < m_pTracks[nTrack].m_nTotalDocs; nDoc++) {
				m_pTracks[nTrack].m_pDoc[nDoc]->ResetDocServer();
			}
		}

		CalculateFOVWidthAndHeight();
		//	DeviceConfigFile(FALSE);
		motionParameters.LoadScanningParm();
		motionParameters.CreateScanPositions();
		//	ScannedFOVSequenceData(TRUE);
		int bLoadImageError = 0;
		CString strTemp = m_strMainCfgDir + "\\" + m_strMainCfgModeDir;
		_mkdir(strTemp);
		m_strConfigDir.Format("%s\\%s", strTemp, m_strConfigFile);
		_mkdir(m_strConfigDir);

		for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
			m_pTracks[nTrack].LoadConfigurationServer();
		}

		SetRecipeName(m_strConfigFile);

		/*if (m_pStationMapDlg)
			m_pStationMapDlg->StationMappingFile();*/

		//	MoveToStartPos(FALSE);

		InitializeDefectCode();

		ResizeDieSelectionDlg();
		ClearResults();

		if (m_pProgressDlg != NULL) {
			m_pProgressDlg->SetStatus("Configuration Loaded....", 100);
			m_pProgressDlg->ShowWindow(FALSE);
			m_bProgressDlgOpen = FALSE;
			m_pProgressDlg->m_nCurrentPos = 0;
		}

		m_pMainWnd->ShowWindow(SW_SHOW);
	}

	LeaveCriticalSection(&m_csLoadConfig);
}

void CApplication::LoadRecipes()
{
	if (m_pProgressDlg != NULL) {
		int nLoadedDataPercentage = m_pProgressDlg->m_nCurrentPos;

		m_pProgressDlg->ShowWindow(TRUE);
		m_bProgressDlgOpen = TRUE;
		m_pProgressDlg->UpdateWindow();
		m_pProgressDlg->SetStatus("Loading Configuration....", nLoadedDataPercentage);
	}

	motionParameters.LoadDeviceParameters();
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfFov = nFovPerX * nFovPerY;

	for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
		for (int nDoc = 0; nDoc < m_pTracks[nTrack].m_nTotalDocs; nDoc++) {
			m_pTracks[nTrack].m_pDoc[nDoc]->ResetDoc();
		}
	}

	CalculateFOVWidthAndHeight();
//	DeviceConfigFile(FALSE);
	motionParameters.LoadScanningParm();
	motionParameters.CreateScanPositions();
//	ScannedFOVSequenceData(TRUE);
    int bLoadImageError = 0;
	CString strTemp = m_strMainCfgDir +"\\"+m_strMainCfgModeDir;
	_mkdir(strTemp);
	m_strConfigDir.Format("%s\\%s", strTemp, m_strConfigFile);
	_mkdir(m_strConfigDir);

	if(m_bIsMasterApp){
		for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
			for (int nDoc = 0; nDoc < m_pTracks[nTrack].m_nTotalDocs; nDoc++) {
				for (int nFov = 0; nFov < nFovPerX * nFovPerY; nFov++) {
					m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.ClearAllNccModels();
				}
			}
		}
		for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
			m_pTracks[nTrack].LoadConfiguration();
		}
	}
	else {
		for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
			m_pTracks[nTrack].LoadConfigurationServer();
		}
	}

	SetEvent(m_EventRefreshLight);//// Set Event for Refresh Light


	SetRecipeName(m_strConfigFile);

	if(m_pStationMapDlg)
		m_pStationMapDlg->StationMappingFile();

//	MoveToStartPos(FALSE);

	InitializeDefectCode();
	/*CreateDefectCodeParmList();
	LoadDefectCodeParameters(m_strConfigDir, m_strConfigFile);
	((CMainFrame*)m_pMainWnd)->InitializeDefectProperties();*/
	
	ResizeDieSelectionDlg();
	ClearResults();

	if (m_pProgressDlg != NULL) {
		m_pProgressDlg->SetStatus("Configuration Loaded....", 100);
		m_pProgressDlg->ShowWindow(FALSE);
		m_bProgressDlgOpen = FALSE;
		m_pProgressDlg->m_nCurrentPos = 0;
	}

	//Update Recipe Infor To Centralized Reject PC
	if (m_bCentralizedVerificationMode && !m_CentralizedCommInterface.m_bIsTCPServer && m_CentralizedCommInterface.m_pCommManager[0]->m_Monitor.m_bConnected) {
		m_CentralizedCommInterface.m_pCommManager[0]->SendInfo(INFOACK);
	}

	m_pMainWnd->ShowWindow(SW_SHOW);
}

void CApplication::LoadRecipes(int nTrack, int nFov)
{
	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);


	CalculateFOVWidthAndHeight();
	motionParameters.LoadDeviceParameters();
	//	DeviceConfigFile(FALSE);
	motionParameters.LoadScanningParm();
	motionParameters.CreateScanPositions();
	//	ScannedFOVSequenceData(TRUE);
	int bLoadImageError = 0;
	CString strTemp = m_strMainCfgDir + "\\" + m_strMainCfgModeDir;
	_mkdir(strTemp);
	m_strConfigDir.Format("%s\\%s", strTemp, m_strConfigFile);
	_mkdir(m_strConfigDir);

	for (int nDoc = 0; nDoc < m_pTracks[nTrack].m_nTotalDocs; nDoc++) {
		m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.ClearAllNccModels();
	}

	m_pTracks[nTrack].LoadConfiguration(nFov);


	/*  if(bLoadImageError){
		  AfxMessageBox("Failed to Load Teach Image",MB_ICONEXCLAMATION);
	  }*/


	SetEvent(m_EventRefreshLight);//// Set Event for Refresh Light


	SetRecipeName(m_strConfigFile);

	if (m_pStationMapDlg)
		m_pStationMapDlg->StationMappingFile();

	//	MoveToStartPos(FALSE);

	InitializeDefectCode();
	/*CreateDefectCodeParmList();
	LoadDefectCodeParameters(m_strConfigDir, m_strConfigFile);
	((CMainFrame*)m_pMainWnd)->InitializeDefectProperties();*/

	/*ResizeDieSelectionDlg();*/
	//ClearResults();

	m_pMainWnd->ShowWindow(SW_SHOW);
}

void CApplication::AutoLoadRecipes()
{
	m_strConfigFile = GetRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Recipe", "");
	if(m_strConfigFile.IsEmpty()) {
		m_strConfigFile = "Default";
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Recipe", m_strConfigFile);
	}

	m_pMainWnd->SendMessage(WM_LOAD_RECIPE);
}

void CApplication::InitializePositionProperties()
{

}

void CApplication::SaveRecipeInfo()
{
	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Folder:Config Mode", m_strMainCfgModeDir);
	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Recipe", m_strConfigFile);
}

int CApplication::CopyRecipe(CString strRecipeSrcMode,CString strRecipeDesMode,CString strRecipeSrc, CString strRecipeDst)
{
	CString strSrc, strDest, strSrc1, strDest1;

	strSrc.Format("%s\\%s", m_strMainCfgDir, strRecipeSrcMode);
	strDest.Format("%s\\%s", m_strMainCfgDir, strRecipeDesMode);
	_mkdir(strDest);

	strSrc1.Format("%s\\%s", strSrc, strRecipeSrc);
	strDest1.Format("%s\\%s", strDest, strRecipeDst);
	_mkdir(strDest1);

	HANDLE hFile;
	int nCount = 0;
	WIN32_FIND_DATA FindFileData;
	int nFlag = 0;
	if ((hFile = FindFirstFile(strSrc1 + "\\*.", &FindFileData)) != INVALID_HANDLE_VALUE) {
		do {
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				CString str = FindFileData.cFileName;
				if (str.Find(".") == -1/* && str.CompareNoCase("_Common")!=0*/) {
					nFlag = 1;
					break;
				}
			}
		} while (FindNextFile(hFile, &FindFileData));
	}
	FindClose(hFile);
	if (nFlag == 1) {
		strSrc.Format("%s\\%s.mot", strSrc1, strRecipeSrc);
		strDest.Format("%s\\%s.mot", strDest1, strRecipeDst);
		CopyFileA(strSrc, strDest, FALSE);

		strSrc.Format("%s\\DefectPrioritySelection.dat", strSrc1);
		strDest.Format("%s\\DefectPrioritySelection.dat", strDest1);
		CopyFileA(strSrc, strDest, FALSE);

		strSrc.Format("%s\\DeviceConfiguration.dat", strSrc1);
		strDest.Format("%s\\DeviceConfiguration.dat", strDest1);
		CopyFileA(strSrc, strDest, FALSE);

		strSrc.Format("%s\\ScanningFOVSequenceParam.dat", strSrc1);
		strDest.Format("%s\\ScanningFOVSequenceParam.dat", strDest1);
		CopyFileA(strSrc, strDest, FALSE);

		strSrc.Format("%s\\ScanningSequenceMapping.dat", strSrc1);
		strDest.Format("%s\\ScanningSequenceMapping.dat", strDest1);
		CopyFileA(strSrc, strDest, FALSE);

		/*strSrc.Format("%s\\%s.dec", strSrc1, strRecipeSrc);
		strDest.Format("%s\\%s.dec", strDest1, strRecipeDst);*/
		strSrc.Format("%s\\Defect code.dat", strSrc1, strRecipeSrc);
		strDest.Format("%s\\Defect code.dat", strDest1, strRecipeDst);
		CopyFileA(strSrc, strDest, FALSE);

		strSrc.Format("%s\\Scanning Parameters.info", strSrc1, strRecipeSrc);
		strDest.Format("%s\\Scanning Parameters.info", strDest1, strRecipeDst);
		CopyFileA(strSrc, strDest, FALSE);




		for (int nTrack = 0; nTrack<m_nTotalTracks; nTrack++)
			m_pTracks[nTrack].CopyRecipe(strRecipeSrcMode, strRecipeDesMode, strRecipeSrc, strRecipeDst);
		return 0;
	}
	else {
		AfxMessageBox("Source Receipe is Empty");
		return -1;
	}

}

void CApplication::ApplyDebugFlag(long lDebugFlag)
{
	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++)
		for(int nDoc=0; nDoc<m_pTracks[nTrack].m_nTotalDocs; nDoc++)
			if(m_pTracks[nTrack].m_pDoc[nDoc])
				m_pTracks[nTrack].m_pDoc[nDoc]->m_lDebugFlag=lDebugFlag;
}


UINT SimulationImageLoadThread(LPVOID pParam)
{
	CApplication* pApp = (CApplication*) AfxGetApp();

	///Udupa; TODO
	//pApp->m_pResourceCheckDlg->m_strSimulationStatus = "*** Loading images to memory for SIMULATION ***";
	//pApp->m_pResourceCheckDlg->SendMessage(WM_SETSMSTATUS);

	for(int i=0; i<pApp->m_nTotalTracks; i++) {
		pApp->m_pTracks[i].LoadSimulationBuffers();
///				m_pTracks[i].bLoadComplete = FALSE;
///				m_pTracks[i].m_nImageCount = 0;
///				
///				m_pTracks[i].BrowseThruImagesForSimul(i);				
	}
	
	SetEvent(pApp->m_eventOnline);

//	pApp->m_pResourceCheckDlg->m_strSimulationStatus = "Images loaded from disk to memory";
//	pApp->m_pResourceCheckDlg->SendMessage(WM_SETSMSTATUS);
//	Sleep(1000);

	///Udupa; TODO
	//if(pApp->m_bSaveAllImages)
	//	pApp->m_pResourceCheckDlg->CheckResourceAvailability(TRUE);
	//else
	//	pApp->m_pResourceCheckDlg->m_strSimulationStatus = "Fast-Save Disabled";

	//pApp->m_pResourceCheckDlg->SendMessage(WM_SETSMSTATUS);

	return 0;
}

UINT SimulationImageFreeThread(LPVOID pParam)
{
	CApplication* pApp = (CApplication*) AfxGetApp();
	for(int i=0; i<pApp->m_nTotalTracks; i++) {
		if(pApp->m_pTracks[i].m_pDoc[0]->m_bInspecting) {
			pApp->WaitForEndTrackInspect(i);
			//	while(pApp->m_pTracks[i].m_pDoc[0]->m_bInspecting)
			//		Sleep(100);
			pApp->m_pTracks[i].m_pDoc[0]->m_bInspecting = FALSE;
		}
//		Sleep(100);
		if(pApp->m_bEnableSimulationMode)
			pApp->m_pTracks[i].FreeSimulationBuffers();
	}

	return 0;
}

void CApplication::SaveCrashImages(int nTrack)
{
	if(m_bSaveCrashImages) {
		ULARGE_INTEGER ulAvailable, ulTotal, ulFree;

		BOOL bDiskDrive = GetDiskFreeSpaceEx((LPCTSTR) "E:\\", &ulAvailable, &ulTotal, &ulFree);
		int nFreeSpace = (int)(ulFree.QuadPart/1000000);

		if(nFreeSpace >= 400 && bDiskDrive) {
			CString strFolder, strCrashFileName, strTemp;

			strFolder.Format("%s\\Crash Images Folder%d",m_ImageSaveFolder, m_nSystemRestartCounter);
			_mkdir(strFolder);

			strCrashFileName.Format("%s\\Track%d_No%d.bmp", strFolder, nTrack+1, m_nCrashImageCounter[nTrack]++);

			SaveGrayScaleBMPFile(strCrashFileName, m_pTracks[nTrack].m_pDoc[0]->m_Buffer);

			if(m_nCrashImageCounter[nTrack] >= 25)
				m_nCrashImageCounter[nTrack] = 0;
		}
	}
}

void CApplication::CheckConfiguration()
{
	//HANDLE hFile;
	//WIN32_FIND_DATA FindFileData;
	//hFile = FindFirstFile(m_strMainCfgDir + "\\" + m_strConfigFile,&FindFileData);
	//
	//if(hFile == INVALID_HANDLE_VALUE)
	//{ 
	//	CString strMsg;
	//	int nCreateDefaultConfig = 0;
	//	nCreateDefaultConfig = GetRegistryEntry(READ_REGPATH + "Preferences","Create Default Config", nCreateDefaultConfig);
	//	PutRegistryEntry(WRITE_REGPATH + "Preferences","Create Default Config", 0);//Reset to Zero Each Time
	//	strMsg.Format("The Configuration File \"%s\" is Not Found in Configuration Directory.",m_strConfigFile);
	//	if(nCreateDefaultConfig)
	//	{  
	//		if(AfxMessageBox(strMsg + "\n\n\tDo You Want To Create Default Configuration ? ",MB_YESNO|MB_ICONQUESTION) == IDYES)
	//		{
	//			m_strConfigFile = "Default";
	//			return;
	//		}
	//		else{
	//			
	//			AfxMessageBox("Application Need To Close. Please Manually Copy the Configuration File !",MB_OK|MB_ICONSTOP);
	//			exit(0);
	//		}
	//	}
	//	
	//	else
	//	{
	//		AfxMessageBox(strMsg + " Application Need To Close \n\n\t\tPlease Manually Copy the Configuration File !",MB_OK|MB_ICONSTOP);
	//		exit(0);
	//		
	//	}
	//}
	//else
	//	return;
	//
	//
}

BOOL CApplication::CheckDiskFreeSpace()
{
	ULARGE_INTEGER ulAvailable, ulTotal, ulFree;
	CString strImageDir, strMsg, strTemp;

	strImageDir.Format("%s", m_ImageSaveFolder);
	strTemp = strImageDir;
	
	BOOL bDiskDrive = GetDiskFreeSpaceEx((LPCTSTR)strImageDir, &ulAvailable, &ulTotal, &ulFree);
	double dFreeSpaceGB = ulFree.QuadPart / 1000000000.0;//convert to GB 1e-9
	
	if(!bDiskDrive){
		AfxMessageBox("Please Specify a valid Drive for Image saving");
		return FALSE;
	}
	
	if(dFreeSpaceGB < m_nMinimumDiskSpaceImageSaveGB) {
		strMsg.Format("Not Enough Space in Deep Learning Image Saving Directory (Minimum is %dGB): %s", m_nMinimumDiskSpaceImageSaveGB, strImageDir);
		AfxMessageBox(strMsg, MB_ICONSTOP);
		return FALSE;
	}
	else
		return TRUE;
	
}

BOOL CApplication::CheckDLDiskFreeSpace()
{
	ULARGE_INTEGER ulAvailable, ulTotal, ulFree;
	CString strImageDir, strMsg, strTemp;

	strImageDir.Format("%s", m_ImageSaveFolderDL);
	strTemp = strImageDir;

	BOOL bDiskDrive = GetDiskFreeSpaceEx((LPCTSTR)strImageDir, &ulAvailable, &ulTotal, &ulFree);
	double dFreeSpaceGB = ulFree.QuadPart / 1000000000.0;//convert to GB 1e-9

	if (!bDiskDrive) {
		AfxMessageBox("Please Specify a valid Drive for Deep Learning Image saving");
		return FALSE;
	}

	if (dFreeSpaceGB < m_nMinimumDiskSpaceImageSaveGB) {
		strMsg.Format("Not Enough Space in Deep Learning Image Saving Directory (Minimum is %dGB): %s", m_nMinimumDiskSpaceImageSaveGB, strImageDir);
		AfxMessageBox(strMsg, MB_ICONSTOP);
		return FALSE;
	}
	else
		return TRUE;

}

void CApplication::OnVisionOnline()
{
	/*if(!m_bOnline)
		if(m_nOperationMode == 3)
			if(AfxMessageBox("Current Operation Mode == 3. Proceed?\n(Please make sure GoodB list is correct.)\n", MB_OKCANCEL | MB_ICONQUESTION)==IDCANCEL)
				return;*/

 	if(m_bOnline) {
		if(m_bLotOpened){
			if(AfxMessageBox("Confirm to go OFFLINE", MB_OKCANCEL | MB_ICONQUESTION)==IDCANCEL) {
				((CMainFrame*) m_pMainWnd)->m_wndRibbonBar.RedrawWindow();
				return;
			}
		}
	}	

	//m_pImageSelectionDlg->SetSize();

	for(int i=0; i<m_nTotalTracks; i++) {
		if(m_pTracks[i].m_bContInsp) {
			m_pTracks[i].m_bContInsp = FALSE;
			SetEvent(m_pTracks[i].m_EventKillInspCycle);
		}
		if(m_pTracks[i].m_bCamAvail) {
			m_pTracks[i].m_bCamEnable = TRUE;
			if(m_pTracks[i].m_bLiveImage)
				m_pTracks[i].m_pDoc[0]->CameraLive();
		}
		for(int j=0; j<m_nTotalViewDocs[i]; j++)
			m_pTracks[i].m_pDoc[j]->SetImageTitle(NULL, !m_bOnline);
	}

	//if(m_pStatsView)
	InitializeLotStats();
	
	
	m_bOnline = !m_bOnline;
	/*if(!m_bLotOpened && m_bOnline){
		int nError = OpenLotDlg();
		if(nError==-1)
			return;
	}*/
	
	SetVisOnLineState(m_bOnline);
	m_bOnline ? m_nOutData |= OUT_VISION_AUTO : m_nOutData &= ~OUT_VISION_AUTO;		
	m_nOutData &= ~OUT_BUSY;
	m_nOutData &= ~OUT_RESULT1;

	m_CommModule.OutPortWrite(m_lCOMMInCardNo, m_lCOMMOutPortID, m_nOutData, 0);
	m_bLoadMultiImagesFromFile = FALSE;
	
	//if(!m_bReset)
	//	m_bOnline ? OnParmScanBarcode() : m_pScanBarcodeDlg->OnBnClickedQuit();
}

void CApplication::OnUpdateVisionOnline(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bOnline);
    bool bLive = FALSE;
    bool bStoredImageInsp = FALSE;
	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
		bLive |= m_pTracks[nTrack].m_bLiveImage;
		bStoredImageInsp |= m_pTracks[nTrack].m_pDoc[0] != NULL && m_pTracks[nTrack].m_pDoc[0]->m_bInspSavedImage;
	}

	pCmdUI->Enable(!bLive &&
				   !m_bTeaching &&
				   !m_bInspecting &&
				   !m_bTrackInspSeq &&
				   !bStoredImageInsp && 
				   (m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nProdMode));
}

void CApplication::OnGooffline()
{
	if(m_bOnline)
		OnVisionOnline();
}

void CApplication::OnOnlineStatsResetMode()
{
    CStatisticsResetDlg StaisticsReset;	
	StaisticsReset.DoModal();
}

void CApplication::OnUpdateOnlineStatsResetMode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nResetStatCounters);
}

void CApplication::OnEnableDLModelInspect()
{
	m_bEnableDLModelInspect = !m_bEnableDLModelInspect;

	for (int i = 0; i < m_nTotalTracks; i++){
		OutputDebugLogTo(i + 1, TRUE, m_bEnableDLModelInspect ? "*** Deep Learning Model Inspect enabled ***" : "*** Deep Learning Model Inspect disabled ***");
	}
	if (m_bEnableDLModelInspect == TRUE)
		AddLogEvent("Deep Learning Model Inspect enabled");
	else
		AddLogEvent("Deep Learning Model Inspect disabled");

	PutRegistryEntry(WRITE_REGPATH + "Deep Learning Engine", "Enable DL Model Inspect", m_bEnableDLModelInspect);
}

void CApplication::OnUpdateEnableDLModelInspect(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nSaveOnlineDLImages);
	pCmdUI->SetCheck(m_bEnableDLModelInspect);
}

void CApplication::OnUseDLModelResult()
{
	m_bUseDLModelResult = !m_bUseDLModelResult;

	for (int i = 0; i < m_nTotalTracks; i++) {
		OutputDebugLogTo(i + 1, TRUE, m_bUseDLModelResult ? "*** Use Deep Learning Model Result enabled ***" : "*** Use Deep Learning Model Result disabled ***");		
	}
	if (m_bUseDLModelResult == TRUE)
		AddLogEvent("Use Deep Learning Model Result enabled");
	else
		AddLogEvent("Use Deep Learning Model Result disabled");

	PutRegistryEntry(WRITE_REGPATH + "Deep Learning Engine", "Use DL Model Result For Final Inspection", m_bUseDLModelResult);
}

void CApplication::OnUpdateUseDLModelResult(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nSaveOnlineDLImages);
	pCmdUI->SetCheck(m_bUseDLModelResult);
}


void CApplication::OnOnlineSaveDeepLearningImages()
{
	m_bSaveDLImages = !m_bSaveDLImages;

	if (!CheckDLDiskFreeSpace())
		m_bSaveDLImages = FALSE;
	else {
		for (int i = 0; i < m_nTotalTracks + 1; i++)
			OutputDebugLogTo(i + 1, TRUE, m_bSaveDLImages ? "*** PASS-Deep Learning Image-Save enabled ***" : "*** PASS-Deep Learning Image-Save disabled ***");

		if (m_bSaveDLImages == TRUE)
			AddLogEvent("Online Save Pass Deep Learning images enabled");
		else
			AddLogEvent("Online Save Pass Deep Learning images disabled");
	}

	PutRegistryEntry(WRITE_REGPATH + "Preferences", "Save Deep Learning Images", m_bSaveDLImages);
	if (!m_bSaveDLImages && !m_bSaveDLPassImages && !m_bSaveDLFailImages)
		m_pImageQueue->FlushQueueBufferDL();
}

void CApplication::OnUpdateOnlineSaveDeepLearningImages(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nSaveOnlineDLImages);
	pCmdUI->SetCheck(m_bSaveDLImages);
}

void CApplication::OnOnlineSavePassImages()
{
	m_bSaveOnlinePassImages = !m_bSaveOnlinePassImages;

	if(!CheckDiskFreeSpace())
		m_bSaveOnlinePassImages = FALSE;
	else {
		for(int i=0; i<m_nTotalTracks+1; i++)
			OutputDebugLogTo(i+1, TRUE, m_bSaveOnlinePassImages ? "*** PASS-Image-Save enabled ***" : "*** PASS-Image-Save disabled ***");

		if(m_bSaveOnlinePassImages == TRUE)
			AddLogEvent("Online Save Pass images enabled");
		else 
			AddLogEvent("Online Save Pass images Disabled");
	}

	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "SavePassImages", m_bSaveOnlinePassImages);
	if(!m_bSaveOnlinePassImages && !m_bSaveOnlineFailImages && !m_bSaveOnlineJpegImages)
		m_pImageQueue->FlushQueueBuffers();
}

void CApplication::OnUpdateOnlineSavePassImages(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nSaveOnlinePassImages);
	pCmdUI->SetCheck(m_bSaveOnlinePassImages);
}

void CApplication::OnOnlineSaveFailImages()
{
	m_bSaveOnlineFailImages = !m_bSaveOnlineFailImages;

	if(!CheckDiskFreeSpace())
		m_bSaveOnlineFailImages = FALSE;
	else {
		for(int i=0; i<m_nTotalTracks+1; i++)
			OutputDebugLogTo(i+1, TRUE, m_bSaveOnlineFailImages ? "*** FAIL-Image-Save enabled ***" : "*** FAIL-Image-Save disabled ***");

		if(m_bSaveOnlineFailImages == TRUE)
			AddLogEvent("Online Save fail images enabled");
		else 
			AddLogEvent("Online Save fail images Disabled");
	}

	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "SaveFailImages", m_bSaveOnlineFailImages);
	if(!m_bSaveOnlinePassImages && !m_bSaveOnlineFailImages && !m_bSaveOnlineJpegImages)
		m_pImageQueue->FlushQueueBuffers();
}

void CApplication::OnUpdateOnlineSaveFailImages(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nSaveOnlineFailImages);
	pCmdUI->SetCheck(m_bSaveOnlineFailImages);
}

void CApplication::OnOnlineSaveJpegImages()
{
	m_bSaveOnlineJpegImages = !m_bSaveOnlineJpegImages;

	if(!CheckDiskFreeSpace())
		m_bSaveOnlineJpegImages = FALSE;
	else {
		for(int i=0; i<m_nTotalTracks+1; i++)
			OutputDebugLogTo(i+1, TRUE, m_bSaveOnlineJpegImages ? "*** Online Save Jpeg Format images enabled ***" : "*** Online Save Jpeg Format images Disabled ***");

		if(m_bSaveOnlineJpegImages == TRUE)
			AddLogEvent("Online Save Jpeg Format images enabled");
		else 
			AddLogEvent("Online Save Jpeg Format images Disabled");
	}

	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "SaveJpegImages", m_bSaveOnlineJpegImages);
	if(!m_bSaveOnlinePassImages && !m_bSaveOnlineFailImages && !m_bSaveOnlineJpegImages)
		m_pImageQueue->FlushQueueBuffers();
}

void CApplication::OnUpdateOnlineSaveJpegImages(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nSaveOnlineJpegImages);
	pCmdUI->SetCheck(m_bSaveOnlineJpegImages);
}

void CApplication::OnOnlineSaveBMPImages()
{
	m_bSaveOnlineBMPImages = !m_bSaveOnlineBMPImages;

	if (!CheckDiskFreeSpace())
		m_bSaveOnlineBMPImages = FALSE;
	else {
		for (int i = 0; i<m_nTotalTracks + 1; i++)
			OutputDebugLogTo(i + 1, TRUE, m_bSaveOnlineBMPImages ? "*** Online Save BMP Format images enabled ***" : "*** Online Save BMP Format images Disabled ***");

		if (m_bSaveOnlineBMPImages == TRUE)
			AddLogEvent("Online Save BMP Format images enabled");
		else
			AddLogEvent("Online Save BMP Format images Disabled");
	}

	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "SaveBMPImages", m_bSaveOnlineBMPImages);
	if (!m_bSaveOnlinePassImages && !m_bSaveOnlineFailImages && !m_bSaveOnlineJpegImages && !m_bSaveOnlineBMPImages)
		m_pImageQueue->FlushQueueBuffers();
}

void CApplication::OnUpdateOnlineSaveBMPImages(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nSaveOnlineJpegImages);
	pCmdUI->SetCheck(m_bSaveOnlineBMPImages);
}

void CApplication::OnInspAutoFocusingSequence()
{
	m_bAutoFocusingSequence = !m_bAutoFocusingSequence;

	if (m_bAutoFocusingSequence == TRUE)
		AddLogEvent("Auto Focusing Sequence Enabled");
	else
		AddLogEvent("Auto Focusing Sequence Disabled");

	PutRegistryEntry(WRITE_REGPATH + "Preferences", "Auto Focusing Sequence", m_bAutoFocusingSequence);
}

void CApplication::OnUpdateInspAutoFocusingSequence(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nSaveOnlineDLPassImages);
	pCmdUI->SetCheck(m_bAutoFocusingSequence);
}

void CApplication::OnOnlineSaveDLPassImages()
{
	m_bSaveDLPassImages = !m_bSaveDLPassImages;

	if (!CheckDLDiskFreeSpace())
		m_bSaveDLPassImages = FALSE;
	else {
		for (int i = 0; i < m_nTotalTracks + 1; i++)
			OutputDebugLogTo(i + 1, TRUE, m_bSaveDLPassImages ? "*** Online Save Deep Learning Pass images enabled ***" : "*** Online Save Deep Learning Pass images Disabled ***");

		if (m_bSaveDLPassImages == TRUE)
			AddLogEvent("Online Save Deep Learning Pass images enabled");
		else
			AddLogEvent("Online Save Deep Learning Pass images Disabled");
	}

	PutRegistryEntry(WRITE_REGPATH + "Preferences", "Save Deep Learning Pass Images", m_bSaveDLPassImages);
	if (!m_bSaveDLImages && !m_bSaveDLPassImages && !m_bSaveDLFailImages)
		m_pImageQueue->FlushQueueBufferDL();
}

void CApplication::OnUpdateOnlineSaveDLPassImages(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nSaveOnlineDLPassImages);
	pCmdUI->SetCheck(m_bSaveDLPassImages);
}

void CApplication::OnOnlineSaveDLFailImages()
{
	m_bSaveDLFailImages = !m_bSaveDLFailImages;

	if (!CheckDLDiskFreeSpace())
		m_bSaveDLFailImages = FALSE;
	else {
		for (int i = 0; i < m_nTotalTracks + 1; i++)
			OutputDebugLogTo(i + 1, TRUE, m_bSaveDLFailImages ? "*** Online Save Deep Learning Fail images enabled ***" : "*** Online Save Deep Learning Fail images Disabled ***");

		if (m_bSaveDLFailImages == TRUE)
			AddLogEvent("Online Save Deep Learning Fail images enabled");
		else
			AddLogEvent("Online Save Deep Learning Fail images Disabled");
	}

	PutRegistryEntry(WRITE_REGPATH + "Preferences", "Save Deep Learning Fail Images", m_bSaveDLFailImages);
	if (!m_bSaveDLImages && !m_bSaveDLPassImages && !m_bSaveDLFailImages)
		m_pImageQueue->FlushQueueBufferDL();
}

void CApplication::OnUpdateOnlineSaveDLFailImages(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nSaveOnlineDLFailImages);
	pCmdUI->SetCheck(m_bSaveDLFailImages);
}
void CApplication::OnOnlineRemoveBackGround() {
	m_bRemoveBackGround = !m_bRemoveBackGround;
}

void CApplication::OnUpdateOnlineRemoveBackGround(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nSaveOnlineDLImages);
	pCmdUI->SetCheck(m_bRemoveBackGround);
}
void CApplication::OnOnlineContinueTopImageInsp()
{
	m_bOnlineContinueTopImageInsp = !m_bOnlineContinueTopImageInsp;

	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "ContinueTopImageInsp", m_bOnlineContinueTopImageInsp);
}

void CApplication::OnUpdateOnlineContinueTopImageInsp(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nInspect);
	pCmdUI->SetCheck(m_bOnlineContinueTopImageInsp);
}

void CApplication::OnParmRecipeLoad()
{
	CMultipleRecipeDialog dlg;
	dlg.DoModal();
}

void CApplication::OnUpdateParmRecipeLoad(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bOnline &&
		(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nManageCfg) &&
		!m_pTracks[0].m_bLiveImage && (m_nTotalTracks<2 || !m_pTracks[1].m_bLiveImage));
}

void CApplication::OnParmScanBarcode()
{
	if(m_pScanBarcodeDlg)
		delete m_pScanBarcodeDlg;

	m_pScanBarcodeDlg = new CScanBarcode();
	if(!m_pScanBarcodeDlg->Create(IDD_SCAN_BARCODE_DIALOG, AfxGetMainWnd()))
	{
		delete m_pScanBarcodeDlg;
		m_pScanBarcodeDlg = NULL;
		return;
	}

	CRect rect1, rect2;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rect1, 0);
	m_pScanBarcodeDlg->GetWindowRect(&rect2);

	m_pScanBarcodeDlg->SetWindowPos(NULL, rect1.Width() - rect2.Width(), rect1.Height()-rect2.Height()-20, 0, 0, SWP_NOSIZE);
	m_pScanBarcodeDlg->ShowWindow(TRUE);
	m_pScanBarcodeDlg->DisableQuitButton(m_bOnline);
}

void CApplication::OnUpdateParmScanBarcode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bOnline && 
		(m_UserAccInfo.m_nCurrentAccessLevel >=1 /*m_AdvSecurityDlg.m_data.m_nScanBarcode*/));
}

//void CApplication::OnParmRecipeSaveAs()
//{
//	CCopyRecipeDlg dlg;
//	dlg.DoModal();
//}
//
//void CApplication::OnUpdateParmRecipeSaveAs(CCmdUI *pCmdUI)
//{
//	pCmdUI->Enable(!m_bOnline && 
//		(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nCreateConfig));
//}
//
//void CApplication::OnParmRecipeDelete()
//{
//	CDeleteRecipeDlg dlg;
//	dlg.DoModal();
//}
//
//void CApplication::OnUpdateParmRecipeDelete(CCmdUI *pCmdUI)
//{
//	pCmdUI->Enable(!m_bOnline && 
//		(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nCreateConfig));
//}

void CApplication::OnUpdateParmConfig(CCmdUI *pCmdUI)
{

	pCmdUI->Enable(!m_bOnline && (TRUE || m_bSuperUserMode));
}

void CApplication::SetRecipeName(CString strRecipe)
{
	m_pMainWnd->SendMessage(WM_SET_RECIPE_NAME,(WPARAM)&strRecipe, (LPARAM)&m_strMainCfgModeDir);
}

void CApplication::SetUserName(CString strUser)
{
	CMFCRibbonBar* pRibbon = &((CMainFrame*) m_pMainWnd)->m_wndRibbonBar;
	CMFCRibbonBaseElement* pElement = pRibbon->FindByID(ID_USER_BUTTON, TRUE, TRUE);
	pElement->SetText(strUser);
	pRibbon->ForceRecalcLayout();
}

void CApplication::SetOnlineStatus()
{
	CMFCRibbonBar* pRibbon = &((CMainFrame*) m_pMainWnd)->m_wndRibbonBar;
	CMFCRibbonBaseElement* pElement = pRibbon->FindByID(ID_VISIONSTATUS_BUTTON, TRUE, TRUE);
	pElement->SetText(m_bOnline ? "ONLINE" : "OFFLINE");
}

void CApplication::OnRecipeButton()
{
	OnParmRecipeLoad();
}

void CApplication::OnUpdateRecipeButton(CCmdUI *pCmdUI)
{
	BOOL bStepInspectionEnabled = FALSE;
	
	for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
		for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
			bStepInspectionEnabled |= m_pTracks[nTrack].m_pDoc[nDoc]->m_bInspSavedImage;
		}
	}
	pCmdUI->Enable(!m_bOnline &&
		!bStepInspectionEnabled &&
		!m_bTeaching &&
		!m_bInspecting &&
		!m_bInspectionAll &&
		!m_bInspSequenceOffline &&
		(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nManageCfg) &&
		!m_pTracks[0].m_bLiveImage && (m_nTotalTracks<2 || !m_pTracks[1].m_bLiveImage));
}

void CApplication::OnUpdateModeAndProductCodeButton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CApplication::OnUpdateHardwareButton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bOnline);
}

void CApplication::SaveADSTwinCATSetting()
{
	CString strSec;
	if (m_strCommOPC.CompareNoCase("TwinCAT")==0) {
		strSec.Format("ADS PortNr");
		PutRegistryEntry(WRITE_REGPATH + "ADSTwinCAT", strSec, m_nADSPortNr);
		strSec.Format("ADS CycleTime");
		PutRegistryEntry(WRITE_REGPATH + "ADSTwinCAT", strSec, m_nCycleTime);
		strSec.Format("ADS MaxDelay");
		PutRegistryEntry(WRITE_REGPATH + "ADSTwinCAT", strSec, m_nMaxDelay);
		strSec.Format("ADS Timeout");
		PutRegistryEntry(WRITE_REGPATH + "ADSTwinCAT", strSec, m_nTimeout);

		for(int i=0; i<6; i++) {
			//m_byteNetID[i] = 0;
			strSec.Format("NetID%d", i);
			PutRegistryEntry(WRITE_REGPATH + "ADSTwinCAT", strSec, m_byteNetID[i]);
		}
	}
}

void CApplication::OnConfigurationAdstwincatsettings() 
{
	//CADSTwinCATdlg dlg;

	//dlg.m_nNetID0 = m_byteNetID[0];
	//dlg.m_nNetID1 = m_byteNetID[1];
	//dlg.m_nNetID2 = m_byteNetID[2];
	//dlg.m_nNetID3 = m_byteNetID[3];
	//dlg.m_nNetID4 = m_byteNetID[4];
	//dlg.m_nNetID5 = m_byteNetID[5];

	//dlg.m_nADSPortNr = m_nADSPortNr;
	//dlg.m_nMaxDelay  = m_nMaxDelay;
	//dlg.m_nCycleTime = m_nCycleTime;
	//dlg.m_nTimeout   = m_nTimeout;


	//if (dlg.DoModal()==IDOK) {
	//	m_byteNetID[0] = dlg.m_nNetID0;
	//	m_byteNetID[1] = dlg.m_nNetID1;
	//	m_byteNetID[2] = dlg.m_nNetID2;
	//	m_byteNetID[3] = dlg.m_nNetID3;
	//	m_byteNetID[4] = dlg.m_nNetID4;
	//	m_byteNetID[5] = dlg.m_nNetID5;


	//	m_nMaxDelay	 = dlg.m_nMaxDelay;
	//	m_nCycleTime = dlg.m_nCycleTime;
	//	m_nTimeout   = dlg.m_nTimeout;
	//	m_nADSPortNr = dlg.m_nADSPortNr;

	//	SaveADSTwinCATSetting();
	//	AfxMessageBox("Please restart the application!",MB_OK);
	//}	
}

void CApplication::OnUpdateConfigurationAdstwincatsettings(CCmdUI* pCmdUI) 
{
	//pCmdUI->Enable(!m_bOnline && 
	//			   (m_strCommOPC.CompareNoCase("TwinCAT")==0) &&
	//			   m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nCameraCfg);
}

void CApplication::UpdateWatchDog(int nData)
{
	m_pTracks[0].UpdateWatchDog(nData);
}

void CApplication::UpdateLotChangedAck()
{
	m_pTracks[0].UpdateLotChangedAck();
}

void CApplication::OnUpdateOnlinemodeOptions(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}


void CApplication::OnUpdateToolsSettings(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bOnline);
}


void CApplication::OnUpdateToolsUser(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CApplication::OnSettingsUsers() 
{
	CModifyAccounts dlg;
	dlg.m_nLogoutTime = m_UserAccInfo.m_nLogoutTime;
	if(dlg.DoModal() == IDOK) {
		m_UserAccInfo.m_nLogoutTime = dlg.m_nLogoutTime;
		CString strAutoLogout;
		strAutoLogout.Format("Auto Logout time changed to %d minutes", m_UserAccInfo.m_nLogoutTime);
		AddLogEvent(strAutoLogout);
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "UserAutoLogoutMins", m_UserAccInfo.m_nLogoutTime);
	}	
}

void CApplication::OnUpdateSettingsUsers(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_UserAccInfo.m_nCurrentAccessLevel == 2);
}

void CApplication::OnSettingsAccessRights() 
{
	CAdvSecurityDlg dlg;
	if(dlg.DoModal() == IDOK)
		m_AdvSecurityDlg.GetSecurityAccess();
}

void CApplication::OnUpdateSettingsAccessRights(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_UserAccInfo.m_nCurrentAccessLevel == 2);
}

void CApplication::OnUserLogIn()
{
	int p;
	CLoginDlg dlg;
	SYSTEMTIME sysTime;
	CString strUsername, strUserpwd, strLevel;
	CString m_strAccessLevel, strStoredLevel;
	CString strStoredPswd, strPassword;
	bool nClickedOK = FALSE;
	CString strMsg, strHr, strMin, strSec;
		
	GetLocalTime(&sysTime);
	time(&m_UserAccInfo.tmStartLogoutTimeHr);

	m_UserAccInfo.m_strCurrentAccessLevel = "Operator";
	m_UserAccInfo.m_strCurUserName = "Operator";
	m_UserAccInfo.m_bAccessLevel = false;
	
	if(dlg.DoModal() == IDOK) {
		strUsername = dlg.m_strUsername;
		strUserpwd = dlg.m_strPassword;
		m_UserAccInfo.m_strCurrentAccessLevel = dlg.m_strUserLevel;
		nClickedOK = TRUE;
	}
	
	if(nClickedOK) {
		if(strUsername.CompareNoCase(m_UserAccInfo.DefaultUser.strUsername) == 0) {
			//strUserpwd += "ADMN";
			strStoredPswd = m_UserLog.DecodePassWord(&m_UserAccInfo.DefaultUser.strPassword);
			if(strUserpwd.Compare(strStoredPswd) == 0) {
				if(m_UserAccInfo.m_strCurrentAccessLevel.Compare(m_UserAccInfo.DefaultUser.strAccLevel) == 0) {
					m_strAccessLevel = "ADMN";
					m_UserAccInfo.m_bAccessLevel = true;
					m_UserAccInfo.m_strCurUserName = strUsername;
					m_UserAccInfo.m_nCurrentAccessLevel = 2;
					((CMainFrame*)m_pMainWnd)->ViewFullScreen(FALSE);
					AddLogEvent("Logged In");
				}
				else {
					m_UserAccInfo.m_nCurrentAccessLevel = 0;
					m_UserAccInfo.m_strCurrentAccessLevel = "Operator";
					AfxMessageBox("Please check your access level");
					AddLogEvent("Login incorrect");
				}
			}
			else {
				m_UserAccInfo.m_nCurrentAccessLevel = 0;
				m_UserAccInfo.m_strCurrentAccessLevel = "Operator";
				AfxMessageBox("Wrong Password");
				AddLogEvent("Login incorrect");
			}
		}
		else {
			if(m_UserAccInfo.m_strCurrentAccessLevel.CompareNoCase("Administrator") == 0) {
				m_strAccessLevel = "ADMN";
				m_UserAccInfo.m_nCurrentAccessLevel = 2;
			}
			else if(m_UserAccInfo.m_strCurrentAccessLevel.CompareNoCase("Technician") == 0){ 
				m_strAccessLevel = "TECH";
				m_UserAccInfo.m_nCurrentAccessLevel = 1;
			}
			else{
				m_strAccessLevel = "OPRT";
				m_UserAccInfo.m_nCurrentAccessLevel = 0;
			}
			strPassword = strUserpwd + m_strAccessLevel;
			if(m_UserAccInfo.nNumofUsers > 0) {
				for(p = 0; p < m_UserAccInfo.nNumofUsers; p++) {
					if(strUsername.CompareNoCase(m_UserAccInfo.UserAccount[p].strUsername) == 0) {
						strStoredPswd = m_UserAccInfo.UserAccount[p].strPassword;
						strStoredPswd = m_UserLog.DecodePassWord(&strStoredPswd);
						if(strStoredPswd.Compare(strPassword) == 0) {
							m_UserAccInfo.m_strCurUserName = strUsername;
							if((m_strAccessLevel == "ADMN") || (m_strAccessLevel == "TECH")){
								if(strStoredPswd == strPassword)
									AddLogEvent("Logged in");
								m_UserAccInfo.m_bAccessLevel = true;
								((CMainFrame*)m_pMainWnd)->ViewFullScreen(FALSE);
							}
							break;
							
						}
						else {
							m_strAccessLevel = "";
							m_UserAccInfo.m_nCurrentAccessLevel = 0;
							m_UserAccInfo.m_strCurrentAccessLevel = "Operator";
							AfxMessageBox("Wrong Password");
							AddLogEvent("Login incorrect");
							break;
						}
					}
					else
						continue;
				}
				if(p == m_UserAccInfo.nNumofUsers){
					m_strAccessLevel = "";
					m_UserAccInfo.m_strCurUserName = "Operator";
					m_UserAccInfo.m_nCurrentAccessLevel = 0;
					m_UserAccInfo.m_strCurrentAccessLevel = "Operator";
					AfxMessageBox("User Name doesn't exist");
					AddLogEvent("Login incorrect");
				}
			}
			else {
				m_strAccessLevel = "";
				m_UserAccInfo.m_strCurUserName = "Operator";
				m_UserAccInfo.m_nCurrentAccessLevel = 0;
				m_UserAccInfo.m_strCurrentAccessLevel = "Operator";
				AfxMessageBox("User Name doesn't exist");
				AddLogEvent("Login incorrect");
			}
		}
		if(m_UserAccInfo.m_strCurUserName.CompareNoCase("Operator")!=0){
			
			CSystemLoggingInfo* pSystemLoggingInfo;
			pSystemLoggingInfo = new CSystemLoggingInfo();
			
			pSystemLoggingInfo->SaveLoginDetails(&sysTime,
												 m_UserAccInfo.m_strCurUserName,
												 m_strAccessLevel);

			if(pSystemLoggingInfo)
				delete pSystemLoggingInfo;
			pSystemLoggingInfo = NULL;
			

        }

		//LaunchWindowsExplorer();
		SetUserName(m_UserAccInfo.m_strCurUserName);
		//AddLogEvent("Logged In");
	}	
}

void CApplication::OnUpdateUserLogIn(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_UserAccInfo.m_strCurUserName.CompareNoCase("Operator"));
	
}

void CApplication::OnUserLogOut()
{
	SYSTEMTIME sysTime;
	CString strMsg, strHr, strMin, strSec;
	
	m_UserAccInfo.m_bAccessLevel = false;
	GetLocalTime(&sysTime);

	CSystemLoggingInfo* pSystemLoggingInfo;
    pSystemLoggingInfo = new CSystemLoggingInfo();

	pSystemLoggingInfo->SaveLogoutDetails(&sysTime);

	AddLogEvent("Logged Out");

	//((CMainFrame*)m_pMainWnd)->ViewFullScreen(TRUE);
	m_UserAccInfo.m_strCurUserName = "Operator";
	m_UserAccInfo.m_nCurrentAccessLevel = 0;
	SetUserName(m_UserAccInfo.m_strCurUserName);
	m_UserAccInfo.m_strCurrentAccessLevel = "Operator";

	//LaunchWindowsExplorer();
	if(pSystemLoggingInfo) {
		delete pSystemLoggingInfo;
		pSystemLoggingInfo = NULL;
	}
}

void CApplication::OnUpdateUserLogOut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_UserAccInfo.m_strCurUserName.CompareNoCase("Operator"));
}


void CApplication::OnUserChangePassword() 
{
	CIniFile iniFile;
	CString strSec, strDataFileName;
	
	strDataFileName =  m_strMainCfgDir + "\\" + "LogAccount" + ".lgn";
	iniFile.SetPath(strDataFileName);
	iniFile.ReadFile();
	
	LoginInfo TempChangeNewPswd;
	CString strEncodePswd, strAccLevel, strPswd, strLevel;


	int p;
	LoginInfo* pInfo = NULL;
	if(m_UserAccInfo.m_strCurUserName.CompareNoCase("Engineer") == 0) {
		pInfo = &m_UserAccInfo.DefaultUser;
		strAccLevel = "Administrator";
		strLevel = "ADMN";
		p = -1;
	}
	else {
		for(p = 0; p < m_UserAccInfo.nNumofUsers; p++) {
			if(m_UserAccInfo.m_strCurUserName.CompareNoCase(m_UserAccInfo.UserAccount[p].strUsername) == 0) {
				strAccLevel = m_UserAccInfo.UserAccount[p].strAccLevel;
				if(strAccLevel.CompareNoCase("Administrator") == 0) 
					strLevel = "ADMN";
				else if(strAccLevel.CompareNoCase("Technician") == 0) 
					strLevel = "TECH";
				else if(strAccLevel.CompareNoCase("Operator") == 0) 
					strLevel = "OPRT";
				pInfo = &m_UserAccInfo.UserAccount[p];
				break;
			}
			else
				continue;
		}
	}
	if(!pInfo) {
		AfxMessageBox("User Name doesn't exist");
		return;
	}

	CChangePasswordDlg dlg;
	if(dlg.DoModal() == IDOK) {
		strPswd = m_UserAccInfo.m_strNewPassword + strLevel;
		strEncodePswd = m_UserLog.EncodePassWord(&strPswd);
		TempChangeNewPswd.strUsername = m_UserAccInfo.m_strCurUserName;
		TempChangeNewPswd.strAccLevel = strAccLevel;
		TempChangeNewPswd.strPassword = strEncodePswd;


		pInfo->strUsername = m_UserAccInfo.m_strCurUserName;
		pInfo->strAccLevel = strAccLevel;
		pInfo->strPassword = strEncodePswd;

		strSec.Format("User%d", p+1);
		iniFile.SetCString(strSec, "Name", m_UserAccInfo.m_strCurUserName);
		iniFile.SetCString(strSec, "Level", strAccLevel);
		iniFile.SetCString(strSec, "Pswd", strEncodePswd);
		iniFile.WriteFile();
	}
}

void CApplication::OnUpdateUserChangePassword(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_UserAccInfo.m_strCurUserName.CompareNoCase("Operator"));
}

void CApplication::OnUserButton()
{
	if(m_UserAccInfo.m_strCurUserName.CompareNoCase("Operator")) {
		CString str;
		str.Format("Log-out user '%s'?", m_UserAccInfo.m_strCurUserName);
		if(AfxMessageBox(str, MB_YESNO | MB_ICONQUESTION)==IDYES)
			OnUserLogOut();
	}
	else
		OnUserLogIn();
}

void CApplication::OnUpdateUserButton(CCmdUI *pCmdUI)
{
	///pCmdUI->Enable(TRUE);
	pCmdUI->Enable((!m_bOnline || m_bSuperUserMode));// && 
		           //!m_UserAccInfo.m_strCurUserName.CompareNoCase("Operator"));
}


void CApplication::OnVisionstatusButton()
{
	OnVisionOnline();
}

void CApplication::OnUpdateVisionstatusButton(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bOnline);
    
	bool bLive = FALSE;
    bool bStoredImageInsp = FALSE;
	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
		bLive |= m_pTracks[nTrack].m_bLiveImage;
		bStoredImageInsp |= ((m_pTracks[nTrack].m_pDoc[0] != NULL) && m_pTracks[nTrack].m_pDoc[0]->m_bInspSavedImage);
	}

	pCmdUI->Enable(!bLive &&
				   !m_bTeaching &&
				   !m_bInspecting &&
				   !m_bTrackInspSeq &&
				   !bStoredImageInsp && 
				   (m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nProdMode));
}

void CApplication::OnHardwareIO()
{
	CIOTestDialog dlg;
	dlg.DoModal();
}

void CApplication::OnUpdateHardwareIO(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bOnline && 
		(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nIOTest));
}

void CApplication::OnOnlineBypassResult()
{
	m_bByPassResult = !m_bByPassResult;
	if(m_bByPassResult == TRUE)
		AddLogEvent("Online Bypass Result is Enabled");
	else
		AddLogEvent("Online Bypass Result is Disabled");
}

void CApplication::OnUpdateOnlineBypassResult(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bOnline && 
		(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nByPassResult));
	pCmdUI->SetCheck(m_bByPassResult);
}

void CApplication::OnCameraSynchronousGrab()
{
	m_bSynchronousGrabEnable = !m_bSynchronousGrabEnable;
	PutRegistryEntry(WRITE_REGPATH + "Preferences", "Synchronous Grab: Enable", m_bSynchronousGrabEnable);
}

void CApplication::OnUpdateCameraSynchronousGrab(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bSynchronousGrabEnable);
	pCmdUI->Enable(!m_bOnline);
}

int CApplication::LoadDocImages(CString strFileName)
{
	int nError;
	int nIndex = -1;
	int nStrLength = strFileName.GetLength();
	CString strImageFinalPath;
	for(int nDoc=0; nDoc<m_nTotalViews; nDoc++){
		nIndex = strFileName.Find("SSB.bmp");
		if(nIndex > 0)
			strImageFinalPath = strFileName.Left(nStrLength-7);
		else
			strImageFinalPath = strFileName.Left(nStrLength-6);
		switch(nDoc){
			case 0:
				strImageFinalPath = strImageFinalPath + "T1.bmp";
				break;
			case 1:
				strImageFinalPath = strImageFinalPath + "T1.bmp";
				break;
			case 2:
				strImageFinalPath = strImageFinalPath + "T3.bmp";
				break;
			case 3:
				strImageFinalPath = strImageFinalPath + "T4.bmp";
				break;
			case 4:
				strImageFinalPath = strImageFinalPath + "T5.bmp";
				break;
			case 5:
				strImageFinalPath = strImageFinalPath + "B1.bmp";
				break;
			case 6:
				strImageFinalPath = strImageFinalPath + "B2.bmp";
				break;
			case 7:
				strImageFinalPath = strImageFinalPath + "B3.bmp";
				break;
			case 8:
				strImageFinalPath = strImageFinalPath + "B4.bmp";
				break;
			case 9:
				strImageFinalPath = strImageFinalPath + "B5.bmp";
				break;
			case 10:
				strImageFinalPath = strImageFinalPath + "S1.bmp";
				break;
			case 11:
				strImageFinalPath = strImageFinalPath + "S2.bmp";
				break;

		}
		nError = m_pTracks[0].m_pDoc[nDoc]->LoadStaticImage(strImageFinalPath);
	}
	return nError;
}

int CApplication::SynchronousCameraSnap(BOOL bDraw)
{
	CPerformanceTimer TimerTrack[3];
	int nError = 0;
	
	OutputDebugLogTo(9,TRUE,"CApplication::SynchronousCameraSnap Start");
	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
		m_pTracks[nTrack].m_pDoc[0]->CameraGrabUsingExposure();
	}
	OutputDebugLogTo(9,TRUE,"CApplication::SynchronousCameraSnap End");

	//for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++)
	//	SetEvent(m_pTracks[nTrack].m_EventGrabEnd);

	return 0;
}

void CApplication::SynchronousCameraStream()
{
	if(!m_pTracks[0].m_bCamAvail)
		return;

	BOOL bLive = m_pTracks[0].m_bLiveImage;
	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++)
		m_pTracks[nTrack].m_bLiveImage = !bLive;

	if(bLive) {
		SetEvent(m_pTracks[0].m_EventKillLive);
		ResetEvent(m_EventMotionSensor);
		Sleep(100);
	}
	else {
		SetEvent(m_EventMotionSensor);
		AfxBeginThread(TrackLiveAll, this, THREAD_PRIORITY_LOWEST);
	}
}

int CApplication::SequenceScanCameraSnap(BOOL bDraw)
{
	CPerformanceTimer TimerTrack[3];
	int nError[7]={0};

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	int m_nNoOfDies = nDevicePerX * nDevicePerY;
	int m_nNoOfFov = nFovPerX * nFovPerY;


	m_nInSpectionDelay = 0; //Used For Offline inspection /JP  
	int nTotalDocCount = 0;

	for(int nTrack=0; nTrack< m_nTotalTracks; nTrack++) {
		for(int nDoc=0; nDoc< m_pTracks[nTrack].m_nTotalDocs; nDoc++, nTotalDocCount++) {

			if (!motionParameters.IsGrabEnabled(nTrack, nDoc, m_pTracks[nTrack].m_nFOVIndex))
				continue;

			DrwDel(&m_pTracks[nTrack].m_pDoc[nDoc]->m_ImageViewInfo);

			int nXSize,nYSize;
			nXSize = m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer.GetImgSize().cx;
			nYSize = m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer.GetImgSize().cy;
			CRect rect = CRect(0, 0, nXSize, nYSize);

			if(m_bCurtainSensor || m_bEmergencyStop || m_bReceivedStopCmd)
				return -2;

			if(bDraw)
				m_pTracks[nTrack].m_nOpticsRecalcMode = 1;

			if (m_bAutoFocusingSequence && nDoc == 0) {
				m_pTracks[nTrack].SetZPositionForAutoFocusingSequence(nDoc);
			}
			else {
				m_pTracks[nTrack].SetZPositionForSequence(nDoc);
				m_pTracks[nTrack].RefreshLight(nDoc);
			}
			
			nError[nDoc] |= m_pTracks[nTrack].EnqueueAsyncGrab();
			if(!nError[nDoc]) {
				m_pTracks[nTrack].UpdateGrabImage(nDoc);

				if (m_bInspSequenceOffline && m_bSaveInspSeqImages) {
					m_pTracks[nTrack].m_pDoc[nDoc]->SaveInspSeqImages(nTrack, nDoc, m_pTracks[nTrack].m_nDieIndex, m_pTracks[nTrack].m_nFOVIndex);
				}
				CRect rect = CRect(CPoint(0, 0), m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer.GetImgSize());
				ImgCopy(&m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,&rect,&m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[m_pTracks[nTrack].m_nDieIndex][m_pTracks[nTrack].m_nFOVIndex],&rect);
			}
			else
				m_pTracks[nTrack].m_nOpticsRecalcMode = 0;
				
			OutputDebugLogTo(1, TRUE, "[Track%d][Doc%d] GrabTime :: [%3.2f]", nTrack+1, nDoc+1, TimerTrack[nTrack].GetElapsedMSec());
			TimerTrack[nTrack].Reset();
			if(m_bOnline){
				//m_pTracks[nTrack].m_pDoc[nDoc]->SaveDocImage(nTrack,nDoc,m_pTracks[nTrack].m_nDieIndex,m_pTracks[nTrack].m_nFOVIndex,GOOD_UNIT);
			}
			if(m_bInspSequenceOffline && m_bSaveInspSeqImages){
				m_pTracks[nTrack].m_pDoc[nDoc]->SaveInspSeqImages(nTrack,nDoc,m_pTracks[nTrack].m_nDieIndex,m_pTracks[nTrack].m_nFOVIndex);
			}

			ImgCopy(&m_pTracks[nTrack].m_pDoc[nDoc]->m_Buffer,&rect,&m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[m_pTracks[nTrack].m_nDieIndex][m_pTracks[nTrack].m_nFOVIndex],&rect);
			
			//Send grabbed image to Centralized System
			if (m_bCentralizedVerificationMode && !m_CentralizedCommInterface.m_bIsTCPServer) {
				m_pImageQueue->PushImageBufferCentralized(&m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[m_pTracks[nTrack].m_nDieIndex][m_pTracks[nTrack].m_nFOVIndex], 
					nTrack, nDoc, m_pTracks[nTrack].m_nDieIndex, m_pTracks[nTrack].m_nFOVIndex);
			}
		}
	}

	int nErrReturn = 0;
	for(int nDoc=0; nDoc< nTotalDocCount; nDoc++) {
		if(nError[nDoc]) {
			nErrReturn = nError[nDoc];
			break;
		}
	}
	return nErrReturn;
}

void CApplication::OnViewOptionsAuditTrail()
{
	// TODO: Add your command handler code here
	//if(m_pAuditTrailDlg)
	//	delete m_pAuditTrailDlg;

	//m_pAuditTrailDlg = new CAuditTrailDlg();
	//if(!m_pAuditTrailDlg->Create(IDD_AUDITTRAIL_DLG, AfxGetMainWnd())) {
	//	delete m_pAuditTrailDlg;
	//	m_pAuditTrailDlg = NULL;
	//	return;
	//}

	//CRect rect1, rect2;
	//SystemParametersInfo(SPI_GETWORKAREA, NULL, &rect1, 0);
	//m_pAuditTrailDlg->GetWindowRect(&rect2);

	//int r = rect1.Width();
	//int t = rect2.Width();
	//m_pAuditTrailDlg->CenterWindow();
	////m_pAuditTrailDlg->SetWindowPos(NULL, rect1.Width() - rect2.Width()-700, rect1.Height()-rect2.Height()-200, 0, 0, SWP_NOSIZE);
	//m_pAuditTrailDlg->ShowWindow(TRUE);

	CAuditTrailDlg dlg;
	dlg.DoModal();

}


void CApplication::OnUpdateViewOptionsAuditTrail(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	//pCmdUI->Enable(TRUE);
	pCmdUI->Enable(!m_bOnline && m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nAuditTrail);
}

//void CApplication::LaunchWindowsExplorer()
//{
//	#ifdef _DEBUG
// 
//	#else
//		if(m_UserAccInfo.m_nCurrentAccessLevel > 1){
//			STARTUPINFO si;
//			PROCESS_INFORMATION pi;
//
//			ZeroMemory( &si, sizeof( si ) );
//			si.cb = sizeof( si );
//			CreateProcess( NULL, "C:\\WINDOWS\\explorer.exe", NULL, NULL, false, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi );	
//		}else{
//			int retval = ::_tsystem( _T("taskkill /F /IM explorer.exe") );
//
//			//PROCESSENTRY32 entry;
//			//entry.dwFlags = sizeof( PROCESSENTRY32 );
//
//			//HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, NULL );
//
//			//if ( Process32First( snapshot, &entry ) == TRUE ) {
//			//  		while ( Process32Next( snapshot, &entry ) == TRUE ) {
//			//  			if ( stricmp( entry.szExeFile, "explorer.exe" ) == 0 ) {
//			//  				HANDLE hProcess = OpenProcess( /*PROCESS_QUERY_INFORMATION | PROCESS_VM_READ |*/ PROCESS_TERMINATE, FALSE, entry.th32ProcessID );
//			//			UINT exitCode = 0;
//			//  				TerminateProcess( hProcess, exitCode );
//
//			//  				CloseHandle( hProcess );
//
//			//  				break;
//			//  			}
//			//  		}
//			//}
//		}
//	#endif
//	
//}

CString CApplication::DecodeLicenseKey(CString strContent, CString strDecodeKey)
{
	CString strDecodedContent;
	int nPwdLength, nKeyLength;
	unsigned char* strCharContent;
	//unsigned char nameChar[1];

	nPwdLength = strContent.GetLength();
	strCharContent = new unsigned char[nPwdLength];
	nKeyLength = strDecodeKey.GetLength();

	for(int i=0, j=0; i<nPwdLength; i++, j++)
	{
		unsigned char charContent = strContent.GetAt(i);
		strCharContent[i] = charContent;
		if(j==nKeyLength)
			j=0;
		unsigned char charKey = strDecodeKey.GetAt(j);
		strCharContent[i] -= charKey;
		unsigned char charDecodedContent = strCharContent[i];
		if(charDecodedContent < 32 || charDecodedContent > 127)
		{
			charDecodedContent = charDecodedContent - 32 + 127;
			strCharContent[i] = charDecodedContent;
		}
		strDecodedContent += charDecodedContent;
	}
	delete[] strCharContent;

	int nTotal = strDecodedContent.GetLength();
	return strDecodedContent;
}

CString CApplication::EncodeKey(CString strContent, CString strKey)
{
	CString strEncodedContent;
	int nPwdLength, nKeyLength;
	unsigned char* strCharContent;
	//unsigned char nameChar[1];

	nPwdLength = strContent.GetLength();
	strCharContent = new unsigned char[nPwdLength];
	nKeyLength = strKey.GetLength();

	for(int i=0, j=0; i<nPwdLength; i++, j++)
	{
		unsigned char charContent = strContent.GetAt(i);
		strCharContent[i] = charContent;
		if(j==nKeyLength)
			j=0;
		unsigned char charKey = strKey.GetAt(j);
		strCharContent[i] += charKey;
		unsigned char charEncodedContent = strCharContent[i];
		if(charEncodedContent > 127)
		{
			charEncodedContent = charEncodedContent - 127 + 32;
			strCharContent[i] = charEncodedContent;
		}
		strEncodedContent += charEncodedContent;
	}
	delete[] strCharContent;

	int nTotal = strEncodedContent.GetLength();
	return strEncodedContent;
}

int CApplication::CheckApplicationLicense()
{
	
	BOOL bLicenseFound = FALSE;
	CStdioFile file;
	CFileException e;

	CString strLine, strLicenseDate, strDecodedKey;
	CString strFile = m_strMainCfgDir+"\\EmageEquipmentLicense.lic";
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
//	char str[256];

	//strDate.Format("%04d%02d%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	CString strCurrentDate, strPreviousDate, strTempCurrentDate;
	strPreviousDate = GetRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Domain Information", "");
	if(strPreviousDate.IsEmpty()){
		strPreviousDate = "";
		PutRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Domain Information", strPreviousDate);
	}

	if(strPreviousDate != "")
		strPreviousDate = DecodeLicenseKey(strPreviousDate, "NeverKnow");

	CTime CurrentDate, TempCurrentDate;
	CTimeSpan TimeSpan(31, 0, 0, 0);
	CurrentDate = CTime(sysTime.wYear, sysTime.wMonth, sysTime.wDay,0,0,0);
	TempCurrentDate = CurrentDate + TimeSpan;
	strCurrentDate.Format("%d%02d%02d",CurrentDate.GetYear(), CurrentDate.GetMonth(), CurrentDate.GetDay());
	int nCurrentDate = sysTime.wYear*10000 + sysTime.wMonth*100 + sysTime.wDay;
	
	if(!file.Open(strFile, CFile::modeRead, &e))
	{
		AfxMessageBox("No License Found");
		return FALSE;
	}
	else
	{
		int nLine = 0;
		while(file.ReadString(strLine))
		{
			if(nLine != 6)
				nLine++;
			else
			{
				CString strDecode1 = DecodeLicenseKey(strLine, "NeverKnow");
				strLicenseDate = strDecode1.Right(8);
				strTempCurrentDate.Format("%d%02d%02d",TempCurrentDate.GetYear(), TempCurrentDate.GetMonth(), TempCurrentDate.GetDay());
				if(strPreviousDate == "" && atoi(strLicenseDate) > atoi(strTempCurrentDate)) {
					AfxMessageBox("Invalid License");
					return FALSE;
				}
				CString strDecode2 = DecodeLicenseKey(strDecode1, strLicenseDate);
				int nIndex = strDecode2.GetLength();
				strDecodedKey = strDecode2.Left(nIndex-8);

				// Check License for which apps
				int nAppNameIndexEnd = strDecodedKey.Find("Application");
				CString strAppName = strDecodedKey.Left(nAppNameIndexEnd);
				strAppName = strAppName.Right(nAppNameIndexEnd-13);
				if(strAppName.Compare(m_strAppId))
				{
					AfxMessageBox("Incorrect License!");
					return FALSE;
				}

				int nPhyAddrStart = strDecodedKey.Find(":");
				strDecodedKey = strDecodedKey.Right(nIndex-8-nPhyAddrStart-1);
				strDecodedKey = strDecodedKey.Left(12);
				if(!strDecodedKey.Compare("xxxxxxxxxxxx"))
				{
					if(strPreviousDate != "" && atoi(strPreviousDate) > atoi(strCurrentDate)) {
						AfxMessageBox("Invalid License");
						return FALSE;
					}
					else {
						int nLicenseDate = atoi(strLicenseDate);
						int nTrialLeft = nLicenseDate - nCurrentDate;
						if(nTrialLeft <= 0)
						{
							AfxMessageBox("License Expired!");
							return FALSE;
						}
						bLicenseFound = TRUE;
					}
				}
				else
				{
					// Compare decoded Physical Address with Physical Address of PC

					DWORD dwSize = 0;
					DWORD dwRetVal = 0;
					ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

					ULONG family = AF_UNSPEC;

					LPVOID lpMsgBuf = NULL;
					PIP_ADAPTER_ADDRESSES pAddresses = NULL;
					ULONG outBufLen = 0;
					ULONG Iterations = 0;

					PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
					PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
					PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
					PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
					IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = NULL;
					IP_ADAPTER_PREFIX *pPrefix = NULL;
					outBufLen = 15000;

					do 
					{
						pAddresses = (IP_ADAPTER_ADDRESSES *) MALLOC(outBufLen);
						if (pAddresses == NULL) 
						{
							AfxMessageBox("Memory allocation failed for IP_ADAPTER_ADDRESSES struct");
							return FALSE;
						}

						dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

						if (dwRetVal == ERROR_BUFFER_OVERFLOW) 
						{
							FREE(pAddresses);
							pAddresses = NULL;
						} 
						else 
							break;
						Iterations++;

					} while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < 3));

					if (dwRetVal == NO_ERROR)
					{ 
						// If successful, output some information from the data we received
						pCurrAddresses = pAddresses;
						while (pCurrAddresses) 
						{
							CString strMacAddr, strTemp;
							if (pCurrAddresses->PhysicalAddressLength != 0) 
							{
								for (int i = 0; i < (int) pCurrAddresses->PhysicalAddressLength; i++) 
								{
									strTemp.Format("%.2X", (int) pCurrAddresses->PhysicalAddress[i]);
									strMacAddr = strMacAddr + strTemp;
								}
								if(!strMacAddr.Compare(strDecodedKey))
								{
									bLicenseFound = TRUE;
									break;
								}
							}
							pCurrAddresses = pCurrAddresses->Next;
						}
					}
					else
						AfxMessageBox("GetAdaptersAddresses failed");
				}
			}
		}
	}
	if(!bLicenseFound)
		AfxMessageBox("Incorrect License!");

	strCurrentDate = EncodeKey(strCurrentDate, "NeverKnow");
	PutRegistryEntry(READ_REGPATH + "Preferences\\Persistence", "Domain Information", strCurrentDate);

	return bLicenseFound;
}

void CApplication::OnViewZoomAll()
{
	// TODO: Add your command handler code here
	m_bZoomAll = !m_bZoomAll;
	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Zoom All: Enable", m_bZoomAll);
}


void CApplication::OnUpdateViewZoomAll(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(!m_bOnline  && m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nZoomAll);
	pCmdUI->SetCheck(!m_bOnline && m_bZoomAll);
}

void CApplication::OnOpenLot()
{
	 // TODO: Add your command handler code here
	int nError = OpenLotDlg();
	if(nError!=-1){
		OnVisionOnline();
	}
}


int CApplication::OpenLotDlg()
{
	CLotInfoDlg dlg;
	 CString strMsg;
	 SYSTEMTIME sysTime;
	 GetLocalTime(&sysTime);
	 dlg.m_strLotID.Format("%02u-%02u-%04u_%02u-%02u-%02u", 
		sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	 dlg.m_strOperator = m_UserAccInfo.m_strCurUserName;
	 dlg.m_strConfigFile = m_LotSummary.strPkgName;
	 dlg.m_strComments = m_LotSummary.strComments;
	 
	 if (dlg.DoModal()==IDCANCEL){
		return -1;
	 }

	// ((CMainFrame*)AfxGetMainWnd())->WriteToPipe("*" + m_strConfigFile + "\n");
	 m_LotSummary.strStartLotTime.Format("%02u:%02u:%02u", sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	 m_bLotOpened=TRUE;
	
	 UpdateStatisticsWindow(TRUE);
	/* m_strConfigFile = dlg.m_strConfigFile;
	 m_strLotInfo = dlg.m_strLotID;*/
	 LoadRecipes();
	/// m_LotSummary.strMachineID = dlg.m_strMachineID;
	 m_strLotInfo = dlg.m_strLotID;
	 m_LotSummary.strOperatorID = dlg.m_strOperator;
	 m_strLotConfigFile = dlg.m_strConfigFile;
	 m_LotSummary.strComments = dlg.m_strComments;
 
	 
	 PutRegistryEntry(WRITE_REGPATH + "Preferences", "Lot Opened", m_bLotOpened);
	 PutRegistryEntry(WRITE_REGPATH + "Preferences", "Lot Info", m_strLotInfo);
	 strMsg.Format("Lot Opened - Lot ID :: [%s] - Operator :: [%s] - Recipe :: [%s]",m_LotSummary.strLotID, m_LotSummary.strOperatorID, m_LotSummary.strPkgName);
	 AddLogEvent(strMsg);

	 return 0;

}

void CApplication::OnUpdateOpenLot(CCmdUI *pCmdUI)
{
	 // TODO: Add your command update UI handler code here
	 BOOL bBusy = FALSE;
	 for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++)
	  bBusy |= m_pTracks[nTrack].m_bContInsp ||
		 m_pTracks[nTrack].m_bTrackContInsp ||
		 m_pTracks[nTrack].m_bLiveImage;
	 pCmdUI->Enable(!bBusy &&
				   !m_bLotOpened &&
			//       !m_bOnline &&
				   !m_bTeaching &&
				   !m_bInspecting &&
				   (m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nOpenLot));

}


void CApplication::OnCloseLot()
{
 // TODO: Add your command handler code here

	 CFile   file;
	 CString strDataFileName;
	 CString strCrntFile,str, strMsg;

	 str.Format("Do you wish to close the lot - %s?", m_strLotInfo);
	 if(AfxMessageBox(str, MB_YESNO | MB_ICONQUESTION)==IDNO)
	  return;
 
	 strMsg.Format("Lot Closed - Lot ID :: [%s] - Operator :: [%s] - Recipe :: [%s]",m_LotSummary.strLotID, m_LotSummary.strOperatorID, m_LotSummary.strPkgName);
	 AddLogEvent(strMsg);

	 //m_bOnline=TRUE;
	 if(m_bOnline)
		OnVisionOnline();
 
	 SYSTEMTIME sysTime;
	 GetLocalTime(&sysTime);
	 m_LotSummary.strEndTimeDay.Format("%02u:%02u:%02u", sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	 ((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.UpdateLotInfo();
	 ((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.UpdateTotalLotInfo();
	 m_pMainWnd->ShowWindow(SW_MAXIMIZE);
	 m_bLotOpened = FALSE;
	 m_strLotInfo = "";
	 m_strLotConfigFile = "";
	 m_LotSummary.Init();

	 PutRegistryEntry(WRITE_REGPATH + "Preferences", "Lot Opened",m_bLotOpened);
	 PutRegistryEntry(WRITE_REGPATH + "Preferences", "Lot Info",m_strLotInfo);
	 PutRegistryEntry(WRITE_REGPATH + "Preferences", "Lot Info Receipe", m_strLotConfigFile);
	 UpdateStatisticsWindow(TRUE);

}


void CApplication::OnUpdateCloseLot(CCmdUI *pCmdUI)
{
 // TODO: Add your command update UI handler code here
 pCmdUI->Enable(m_bLotOpened &&
				(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nCloseLot));
}

void CApplication::OnInspectionLoop()
{
	m_bStoredImagesLoop = !m_bStoredImagesLoop;
	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "StoredImagesLoop", m_bStoredImagesLoop);
}

void CApplication::OnUpdateInspectionLoop(CCmdUI *pCmdUI)
{

	pCmdUI->Enable(theApp.m_bIsMasterApp && !m_bOnline && (m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nInspectLoop));
	pCmdUI->SetCheck(m_bStoredImagesLoop);
}


void CApplication::RefreshDefectsList()
{

	((CMainFrame*) m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.RefreshDefectsList();
}

void CApplication::InitializeLotStats()
{

	((CMainFrame*) m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.InitializeLotStats();
}





void CApplication::OnDefaultSetting()
{

	CPerformanceTimer timer;
	timer.Reset();
	//((CMainFrame*) m_pMainWnd)->ShowWindow(SW_HIDE);
	CRect rectClient;

	/*GetClientRect(((CMDIFrameWndEx *)m_pMainWnd)->m_hWndMDIClient,&rectClient);
	int cx = rectClient.Width()*0.02;
	int cy = rectClient.Height() /0.9;

	int arrx[3] = { 0, cx,  cx<<1};
	int arry[3] = { 0,  0,  0 };
	int arrcx[3] = { cx, cx, cx};
	int arrcy[3] = { cy, cy, cy};
	*/
	//((CMDIFrameWndEx *)m_pMainWnd)->MoveWindow(arrx[0], arry[0], arrcx[0], arrcy[0],TRUE);
	/*int i;
	for(i=0; i<m_nTotalViews-(m_nSide+m_nBottom); i++) {
		m_pFrames[i]->MDIActivate();
		m_pFrames[i]->GetActiveFrame();
	}
	((CMDIFrameWndEx *)m_pMainWnd)->MDITabNewGroup(TRUE);
	for(i=m_nTop; i<m_nTotalViews-m_nSide; i++) {
		m_pFrames[i]->MDIActivate();
		m_pFrames[i]->GetActiveFrame();
		((CMDIFrameWndEx *)m_pMainWnd)->MDITabMoveToNextGroup(TRUE);
	}

	((CMDIFrameWndEx *)m_pMainWnd)->MDITabNewGroup(TRUE);
	for(i=m_nTop+m_nBottom; i<m_nTotalViews; i++) {
		m_pFrames[i]->MDIActivate();
		m_pFrames[i]->GetActiveFrame();
		((CMDIFrameWndEx *)m_pMainWnd)->MDITabMoveToNextGroup(TRUE);
	}*/
	//m_pFrames[0] = (CMDIChildWndEx *) ((CMDIFrameWndEx *)m_pMainWnd)->GetActiveFrame();

	//for(int i=1; i<m_nTotalViews; i++) {
	//
	//	m_pFrames[i] = (CMDIChildWndEx *) ((CMDIFrameWndEx *)m_pMainWnd)->GetActiveFrame();
	///*	if(i ==m_nTop || i == m_nTop+m_nBottom)
	//		((CMDIFrameWndEx *)m_pMainWnd)->MDITabNewGroup();*/

	//	
	//
	//}
	//((CMDIFrameWndEx *)m_pMainWnd)->IsMDITabbedGroup();
	//((CMDIFrameWndEx *)m_pMainWnd)->LockWindowUpdate();

	//	const CObList& TabGrps = ((CMDIFrameWndEx *)m_pMainWnd)->GetMDITabGroups();
	//	int nTabGroupCount = TabGrps.GetCount();
	//	for (POSITION pos = TabGrps.GetHeadPosition(); pos != 0;)
	//	{
	//		CMFCTabCtrl* pNextWnd = DYNAMIC_DOWNCAST(CMFCTabCtrl, TabGrps.GetNext(pos));
	//		int nTabCount = pNextWnd->GetTabsNum();
	//		for(int j=0;j<nTabCount;j++){
	//			pNextWnd->MoveTab(j,0);//RemoveAllTabs();
	//		}
	//	}
	//POSITION pos = TabGrps.GetHeadPosition();
	//CMFCTabCtrl* pNextWnd = DYNAMIC_DOWNCAST(CMFCTabCtrl, TabGrps.GetNext(pos));
	int nLoadedDataPercentage = m_pProgressDlg->m_nCurrentPos;
	double dStep = 10 * 1.0 / m_nTotalViews, dLoadedDataPercentage = nLoadedDataPercentage;

	((CMDIFrameWndEx *)m_pMainWnd)->BeginWaitCursor();
	((CMDIFrameWndEx *)m_pMainWnd)->LockWindowUpdate();
	m_nTrackCountGroup = 0;
	/*for(int i=m_nTotalViews-1; i>=0; i--) {
		m_pFrames[i]->MDIActivate();
		((CMDIFrameWndEx *)m_pMainWnd)->MDITabMoveToNextGroup(FALSE);

		if (m_pProgressDlg != NULL) {
			dLoadedDataPercentage += dStep;
			nLoadedDataPercentage = dLoadedDataPercentage;
			m_pProgressDlg->SetStatus("Initialize The Doc View...", nLoadedDataPercentage);
		}
	}*/


	for (int i = 0; i <m_nTotalViews; i++) {
		m_pFrames[i]->MDIActivate();
		((CMDIFrameWndEx *)m_pMainWnd)->MDITabMoveToNextGroup(FALSE);

		if (m_pProgressDlg != NULL) {
			dLoadedDataPercentage += dStep;
			nLoadedDataPercentage = dLoadedDataPercentage;
			m_pProgressDlg->SetStatus("Initialize The Doc View...", nLoadedDataPercentage);
		}
	}


	m_nTrackCountGroup  = 1;
	//int nCont;
	//for(int i=m_nTotalViews-1; i>=m_nTop+m_nBottom; i--) {
	//	m_pFrames[i]->ActivateFrame();
	//	if(/*i==m_nTop || */i==m_nTop+m_nBottom){
	//		((CMDIFrameWndEx *)m_pMainWnd)->MDITabNewGroup(TRUE);
	//	}
	//}

	int nCont;

	
	if (m_bEnableGroupingDocView){
		for (int i = m_nTotalViews - 1; i >= m_nTop + m_nBottom; i--) {
			m_pFrames[i]->ActivateFrame();
			if (/*i==m_nTop || */i == m_nTop + m_nBottom) {
				((CMDIFrameWndEx *)m_pMainWnd)->MDITabNewGroup(TRUE);
			}
		}


		if (m_pProgressDlg != NULL) {
			nLoadedDataPercentage += 1;
			m_pProgressDlg->SetStatus("Initialize The Doc View...", nLoadedDataPercentage);
		}

		for (int i = m_nTotalViews - 1; i>m_nTop + m_nBottom; i--) {
			m_pFrames[i]->ActivateFrame();
			((CMDIFrameWndEx *)m_pMainWnd)->MDITabMoveToNextGroup(TRUE);
		}

		if (m_pProgressDlg != NULL) {
			nLoadedDataPercentage += 1;
			m_pProgressDlg->SetStatus("Initialize The Doc View...", nLoadedDataPercentage);
		}
		for (int i = m_nTotalViews - m_nSide - 1; i >= m_nTop; i--) {
			if (i == m_nTop) {
				m_pFrames[i]->ActivateFrame();
				((CMDIFrameWndEx *)m_pMainWnd)->MDITabNewGroup(TRUE);
			}
		}

		if (m_pProgressDlg != NULL) {
			nLoadedDataPercentage += 1;
			m_pProgressDlg->SetStatus("Initialize The Doc View...", nLoadedDataPercentage);
		}

		for (int i = m_nTop + 1; i < m_nTotalViews - m_nSide; i++) {
			m_pFrames[i]->ActivateFrame();
			((CMDIFrameWndEx *)m_pMainWnd)->MDITabMoveToNextGroup(TRUE);
		}

	}
	else {
		for (int i = m_nTotalViews - 1; i >= 0; i--) {
				m_pFrames[i]->ActivateFrame();
				((CMDIFrameWndEx *)m_pMainWnd)->MDITabNewGroup(TRUE);
		}
		m_pFrames[m_nTotalViews - 1]->ActivateFrame();
		((CMDIFrameWndEx *)m_pMainWnd)->MDITabNewGroup(TRUE);
		
		if (m_pProgressDlg != NULL) {
			nLoadedDataPercentage += 1;
			m_pProgressDlg->SetStatus("Initialize The Doc View...", nLoadedDataPercentage);
		}

	}

	
	if (m_pProgressDlg != NULL) {
		nLoadedDataPercentage += 1;
		m_pProgressDlg->SetStatus("Initialize The Doc View...", nLoadedDataPercentage);
	}

	((CMainFrame*) m_pMainWnd)->ShowStatsPane();
	
	((CMainFrame*) m_pMainWnd)->ShowPreviousImages();

	//((CMainFrame*)m_pMainWnd)->OnShowOutputLog();
	((CMainFrame*)m_pMainWnd)->InitOutputLog();

	((CMainFrame*) m_pMainWnd)->HideParmPanes();

	((CMainFrame*) m_pMainWnd)->RecalcLayout();
	((CMDIFrameWndEx *)m_pMainWnd)->UnlockWindowUpdate();
	((CMDIFrameWndEx *)m_pMainWnd)->EndWaitCursor();
	OutputDebugLogTo(0,TRUE,"Default view timing %3.2f", timer.GetElapsedMSec());
//	((CMainFrame*) m_pMainWnd)->ShowWindow(SW_SHOWMAXIMIZED);
}

void CApplication::OnUpdateDefaultSetting(CCmdUI *pCmdUI)
{
	BOOL nCheck=FALSE;
		const CObList& TabGrps = ((CMDIFrameWndEx *)m_pMainWnd)->GetMDITabGroups();
		int nTabGroupCount = TabGrps.GetCount();
		POSITION pos = TabGrps.GetHeadPosition();
		for (int i= 0;i<nTabGroupCount;i++)
		{
			CMFCTabCtrl* pNextWnd = DYNAMIC_DOWNCAST(CMFCTabCtrl, TabGrps.GetNext(pos));
			int nTabCount = pNextWnd->GetTabsNum();
			switch(i)
			{
				case 0:if(nTabCount>m_nTop || nTabCount<m_nTop)
							nCheck =  TRUE;
						break;
				case 1:if(nTabCount>m_nBottom || nTabCount<m_nBottom)
							nCheck =  TRUE;
						break;
				case 2:if(nTabCount>m_nSide || nTabCount<m_nSide)
							nCheck =  TRUE;
						break;
			}
			
		}
	pCmdUI->Enable(nCheck);
}

void CMDIClientAreaWnd::MDITabMoveToNextGroup(BOOL bNext)
{
	CApplication *m_pApp = (CApplication*) AfxGetApp();
	CMFCTabCtrl* pActiveWnd = FindActiveTabWndByActiveChild();
	
	if (pActiveWnd == NULL)
	{
		return;
	}
	ASSERT_VALID(pActiveWnd);
	POSITION pos;
	if(m_pApp->m_nTrackCountGroup == 0)
		pos = m_lstTabbedGroups.GetHeadPosition();
	else{
		pos = m_lstTabbedGroups.Find(pActiveWnd);
		bNext ? m_lstTabbedGroups.GetNext(pos) : m_lstTabbedGroups.GetPrev(pos);
	}

	if (pos == NULL)
	{
		return;
	}
	CMFCTabCtrl* pNextTabWnd = DYNAMIC_DOWNCAST(CMFCTabCtrl, m_lstTabbedGroups.GetAt(pos));
	ASSERT_VALID(pNextTabWnd);

	MoveWindowToTabGroup(pActiveWnd, pNextTabWnd);
}

void CMDIClientAreaWnd::MDITabNewGroup(BOOL bVert)
{
	// if m_groupAlignment has already been set (e.g. it's not GROUP_NO_ALIGN) the bVert parameter should be ignored and
	// the new group should be created according to the existing alignment (because we do not support nested tabbed groups)
	CApplication *m_pApp = (CApplication*) AfxGetApp();

	if (m_groupAlignment == GROUP_VERT_ALIGN)
	{
		bVert = TRUE;
	}
	else if (m_groupAlignment == GROUP_HORZ_ALIGN)
	{
		bVert = FALSE;
	}

	CMFCTabCtrl* pActiveWnd = FindActiveTabWndByActiveChild();
	if (pActiveWnd == NULL)
	{
		return;
	}
	ASSERT_VALID(pActiveWnd);

	CRect rect;
	GetClientRect(&rect);
	ScreenToClient(&rect);
	int cx;
	if (m_pApp->m_bEnableGroupingDocView) 
		cx = (int)rect.Width() / m_pApp->m_nTotalTracks;
	else
		cx = (int)rect.Width() / m_pApp->m_nTotalViews;
	int cy = (int) rect.Height();
//////////////
	int arrx[3] = { 0, cx,  cx<<1};
	int arry[3] = { 0,  0,  0 };
	int arrcx[3] = { cx, cx, cx,};
	int arrcy[3] = { cy, cy, cy,};
	/*if (bVert)
	{
		rect.left = rect.Width()/3.65;

	}
	else
	{
		rect.top += rect.Height() / 2;
	}
*/
	//CMFCTabCtrl* pNewTabWnd = CreateNewTabGroup(pActiveWnd, CRect(arrx[m_pApp->m_nTrackCountGroup], arry[m_pApp->m_nTrackCountGroup], arrcx[m_pApp->m_nTrackCountGroup], arrcy[m_pApp->m_nTrackCountGroup]), bVert);
	CMFCTabCtrl* pNewTabWnd = CreateNewTabGroup(pActiveWnd, CRect(arrx[0], arry[0], arrcx[0], arrcy[0]), bVert);
	
	MoveWindowToTabGroup(pActiveWnd, pNewTabWnd);
	
}

void CApplication::OnParmOptionsTeachInspAndLoc()
{
	//CStationMapping Dlg;
	//Dlg.DoModal();

	/*if(m_pStationMapDlg)
		delete m_pStationMapDlg;

	m_pStationMapDlg = new CStationMapping();
	if(!m_pStationMapDlg->Create(IDD_STATION_MAP_DLG, AfxGetMainWnd()))
	{
		delete m_pStationMapDlg;
		m_pStationMapDlg = NULL;
		m_bStationMapDlgOpen = FALSE;
		return;
	}*/

	if(m_pStationMapDlg != NULL) {
		CRect rect1, rect2;
		SystemParametersInfo(SPI_GETWORKAREA, NULL, &rect1, 0);
		m_pStationMapDlg->GetWindowRect(&rect2);

		//m_pStationMapDlg->SetWindowPos(NULL, rect1.Width() - rect2.Width(), rect1.Height()-rect2.Height()-20, 0, 0, SWP_NOSIZE);
		m_pStationMapDlg->SetWindowPos(NULL, rect1.Width()-rect2.Width()-rect2.Width()/2, rect1.Height()-rect2.Height()-20, 0, 0, SWP_NOSIZE);
		m_pStationMapDlg->ShowWindow(TRUE);
		m_bStationMapDlgOpen = TRUE;
		m_pStationMapDlg->Initialize();
		m_bAllowStream = TRUE;
	}
}

void CApplication::OnUpdateParmOptionsTeachInspAndLoc(CCmdUI *pCmdUI)
{
	BOOL bTrackLiveImg = FALSE;
	BOOL bDocInsp = FALSE;
	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
		bTrackLiveImg &= m_pTracks[nTrack].m_bLiveImage;
		bDocInsp &= m_pTracks[nTrack].m_pDoc[0]->m_bInspecting;
	}
	BOOL bStepInspectionEnabled = FALSE;
	BOOL bStoredInspectionEnabled = FALSE;
	for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
		for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
			bStepInspectionEnabled |= m_pTracks[nTrack].m_pDoc[nDoc]->m_bInspSavedImage;
			bStoredInspectionEnabled |= m_pTracks[nTrack].m_pDoc[nDoc]->m_bStoredImages;
		}
	}

	pCmdUI->Enable(!m_bOnline &&
		!bStepInspectionEnabled &&
		!m_bTeaching &&
		!m_bTrackInspSeq &&
		!m_bInspecting &&
		!bTrackLiveImg &&
		!bDocInsp &&
		!bStoredInspectionEnabled &&
	    !m_bStationMapDlgOpen && 
		!m_bLightCtrlDlgOpen && !m_bInspSequenceOffline &&
		(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nTeachInspLoc));
}


void CApplication::CopyConfigFile(CString strSrcFile, CString strDstFile)				
{
	CString strCamSrc, strCamDst, strLightSrc, strLightDst;
	CString strFileSrc = m_strConfigDir + "\\" + m_strConfigFile;

	m_strConfigDir = m_strMainCfgDir + "\\" + strDstFile;
	_mkdir(m_strConfigDir);

	CString strFileDst = m_strConfigDir + "\\" + strDstFile;
	CopyFileA(strFileSrc+".mot", strFileDst+".mot", FALSE);
	
	strFileSrc.Format("%s\\%s\\Camera%d.cam", m_strMainCfgDir, strSrcFile, 1);    
	strFileDst.Format("%s\\%s\\Camera%d.cam", m_strMainCfgDir, strDstFile, 1);  
	CopyFileA(strFileSrc, strFileDst, FALSE);

	for(int i=0;i<2;i++) {
		strFileSrc.Format("%s\\%s", m_strMainCfgDir, strSrcFile);
		strLightSrc.Format("%s\\%s_Light%d.ltv", strFileSrc, strSrcFile,i+1);

		strFileSrc.Format("%s\\%s", m_strMainCfgDir, strDstFile);
		strLightDst.Format("%s\\%s_Light%d.ltv", strFileSrc, strDstFile,i+1);		
		CopyFileA(strLightSrc, strLightDst , FALSE);
		}

	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++)
		m_pTracks[nTrack].SaveConfigurationAs(strSrcFile, strDstFile);

	m_strConfigFile = strDstFile;				

	SetRecipeName(m_strConfigFile);			

	UpdateStatisticsWindow();               
}


void CApplication::ShowMotionInitializing()
{
	CMFCRibbonBar* pRibbon = &((CMainFrame*) m_pMainWnd)->m_wndRibbonBar;
	CMFCRibbonBaseElement* pElement = pRibbon->FindByID(ID_INITIALIZING_BUTTON, TRUE, TRUE);
//	pElement->SetText((m_MotorResource.isDllOpened() && (/*!m_pMotionControl || */m_pMotionControl->m_bIsStarting)) ? "---   Initializing Motor. Please Wait   ---" : "");
	pRibbon->ForceRecalcLayout();
}



void CApplication::OnUpdateMotorInitializing(CCmdUI *pCmdUI)
{
	BOOL bEnable = m_MotorResource.isDllOpened();
	pCmdUI->SetCheck(bEnable);
	pCmdUI->Enable(bEnable);
}


void CApplication::OnMotionControl()
{
	if (m_pMotionControl == NULL) {
		m_pMotionControl = new CMotionControlDialog(&motionParameters);
		if (!m_pMotionControl->Create(IDD_MOTOR_DLG, m_pMainWnd)) {
			delete m_pMotionControl;
			m_pMotionControl = NULL;
			bOpenedMotionDialog = FALSE;
			return;
		}
	}

	if(m_pMotionControl != NULL) {
		bOpenedMotionDialog = TRUE;
		m_pMotionControl->bChecked = &bOpenedMotionDialog;
		m_pMotionControl->ResetValues();
		m_pMotionControl->DisplayPos();
		m_pMotionControl->ShowWindow(TRUE);
		StationMapDlgControls();
	}
}

void CApplication::OnUpdateMotionControl(CCmdUI *pCmdUI)
{
	BOOL bTrackLiveImg, bDocInsp;
	for (int nTrack = 0; nTrack<m_nTotalTracks; nTrack++) {
		bTrackLiveImg &= m_pTracks[nTrack].m_bLiveImage;
		bDocInsp &= m_pTracks[nTrack].m_pDoc[0]->m_bInspecting;


	}
	BOOL bStepInspectionEnabled = FALSE;
	BOOL bStoredInspectionEnabled = FALSE;
	for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
		for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
			bStepInspectionEnabled |= m_pTracks[nTrack].m_pDoc[nDoc]->m_bInspSavedImage;
			bStoredInspectionEnabled |= m_pTracks[nTrack].m_pDoc[nDoc]->m_bStoredImages;
		}
	}

	pCmdUI->Enable(!m_bOnline &&
		!bStepInspectionEnabled &&
		!m_bTeaching &&
		!m_bTrackInspSeq &&
		!m_bInspecting &&
		!bTrackLiveImg &&
		!bDocInsp &&
		!bStoredInspectionEnabled &&
		!m_bMotionCtrlDlgOpen && !m_bStationMapDlgOpen && !m_bLightCtrlDlgOpen &&
		(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nMotionControl));
}

void CApplication::OnParmOptionsDeviceConfiguration()
{
	//InitializeDefectCode();
//	CreateDefectCodeParmList();
	CDeviceConfig Dlg;
	Dlg.DoModal();
}

void CApplication::OnUpdateParmOptionsDeviceConfiguration(CCmdUI *pCmdUI)
{
	BOOL bTrackLiveImg, bDocInsp;
	for (int nTrack = 0; nTrack<m_nTotalTracks; nTrack++) {
		bTrackLiveImg &= m_pTracks[nTrack].m_bLiveImage;
		bDocInsp &= m_pTracks[nTrack].m_pDoc[0]->m_bInspecting;


	}
	BOOL bStepInspectionEnabled = FALSE;
	BOOL bStoredInspectionEnabled = FALSE;
	for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
		for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
			bStepInspectionEnabled |= m_pTracks[nTrack].m_pDoc[nDoc]->m_bInspSavedImage;
			bStoredInspectionEnabled |= m_pTracks[nTrack].m_pDoc[nDoc]->m_bStoredImages;
		}
	}

	pCmdUI->Enable(!m_bOnline &&
		!bStepInspectionEnabled &&
		!m_bTeaching &&
		!m_bTrackInspSeq &&
		!m_bInspecting &&
		!bTrackLiveImg &&
		!bDocInsp &&
		!bStoredInspectionEnabled &&
		(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nDeviceCfg));
}


void CApplication::OnParmOptionsDefectPrioritySelection()
{
	CDefectPrioritySelDlg Dlg;
	Dlg.DoModal();
}

void CApplication::OnUpdateParmOptionsDefectPrioritySelection(CCmdUI *pCmdUI)
{
	BOOL bTrackLiveImg, bDocInsp;
	for (int nTrack = 0; nTrack<m_nTotalTracks; nTrack++) {
		bTrackLiveImg &= m_pTracks[nTrack].m_bLiveImage;
		bDocInsp &= m_pTracks[nTrack].m_pDoc[0]->m_bInspecting;


	}
	BOOL bStepInspectionEnabled = FALSE;
	BOOL bStoredInspectionEnabled = FALSE;
	for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
		for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
			bStepInspectionEnabled |= m_pTracks[nTrack].m_pDoc[nDoc]->m_bInspSavedImage;
			bStoredInspectionEnabled |= m_pTracks[nTrack].m_pDoc[nDoc]->m_bStoredImages;
		}
	}

	pCmdUI->Enable(!m_bOnline &&
		!bStepInspectionEnabled &&
		!m_bTeaching &&
		!m_bTrackInspSeq &&
		!m_bInspecting &&
		!bTrackLiveImg &&
		!bDocInsp &&
		!bStoredInspectionEnabled &&
		(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nDefectPriority));
}

void CApplication::StationMapDlgControls()
{
	BOOL bEnable = (m_bStationMapDlgOpen && (m_bLightCtrlDlgOpen || m_bMotionCtrlDlgOpen)) ? FALSE : TRUE;
	if (m_pStationMapDlg && m_bStationMapDlgOpen) {
		m_pStationMapDlg->EnableWindow(bEnable);
		m_bAllowStream = !m_bAllowStream;
	}
}

void CApplication::ResizeDieSelectionDlg()
{
	((CMainFrame*) m_pMainWnd)->m_dlgPane->m_wndDlg.InitiDieControls();
	((CMainFrame*) m_pMainWnd)->m_dlgPane->m_wndDlg.SetSize(200,200);
	//((CMainFrame*) m_pMainWnd)->m_dlgPane->m_wndDlg.SetWindowPos(NULL,0,0,cx,cy,SWP_NOACTIVATE|SWP_NOZORDER);
	((CMainFrame*) m_pMainWnd)->m_dlgPane->m_wndDlg.Invalidate();
	((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->bRefreshPane = TRUE;
	((CMainFrame*)m_pMainWnd)->ShowStatsPane();
	//((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.Invalidate();
}


void CApplication::OnMaximize(BOOL bEnable)
{
	m_pMainWnd->SendMessage(WM_HIDE_PANES,bEnable);
}

//void CApplication::OnToolsStaticimageLoadMultiImgFromFile()
//{
//	
//}
//
//
//void CApplication::OnUpdateToolsStaticimageLoadMultiImgFromFile(CCmdUI *pCmdUI)
//{
//	BOOL bTrackLiveImg, bDocInsp;
//	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
//		bTrackLiveImg &= m_pTracks[nTrack].m_bLiveImage;
//		bDocInsp &= m_pTracks[nTrack].m_pDoc[0]->m_bInspecting;
//	}
//
//	pCmdUI->Enable(!m_bOnline &&
//				   !m_bTeaching &&
//				   !m_bTrackInspSeq &&
//				   !m_bInspecting &&
//				   !bTrackLiveImg &&
//				   !bDocInsp &&
//				   (m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nLoadImg));
//	pCmdUI->SetCheck(m_bLoadMultiImagesFromFile);
//}

void CApplication::StepDebugNavigate(int nStep)
{
	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++)
		m_pTracks[nTrack].StepDebugNavigate(nStep);
}

int CApplication::NavigateDieImages(int nKey)
{
	//// commented on customer request ////
	int nErr = -1;
	//if(m_bLoadMultiImagesFromFile) {
	//	nErr = ((CMainFrame*) m_pMainWnd)->m_dlgPane->m_wndDlg.NavigateDieImages(nKey);
	//}
	//else {
	//	nErr = m_pTracks[m_nNavigateStaticImgTrack].NavigateStaticImage(nKey);
	//}

	return nErr;
}

void CApplication::SetCurPosParamDisplay(int nTrack, CString* str)
{
	//CMainFrame* pFrame = ((CMainFrame*) m_pMainWnd);
	if(m_pMainWnd && ::IsWindow(m_pMainWnd->m_hWnd)) {
		char charDoc, charFOV;
		int nDoc, nFOV;
		charDoc = str->GetAt(0);
		charFOV = str->GetAt(1);
		//Not safe
		/*nDoc = atoi(&charDoc);
		nFOV = atoi(&charFOV);*/
		nDoc = charDoc - '0';
		nFOV = charFOV - '0';

		//Validate Index
		int nDevicePerX, nDevicePerY;
		int nFovPerX, nFovPerY;
		motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
		int m_nNoOfFov = nFovPerX * nFovPerY;
		if (nFOV >= 0 && nFOV < m_nNoOfFov && nTrack >= 0 && nTrack < m_nTotalTracks &&
			nDoc >= 0 && nDoc < MAX_DOCS) {
			//Teach Param Dialog
			if (((CMainFrame*)m_pMainWnd)->IsWindowVisible() &&
				(((CMainFrame*)m_pMainWnd)->m_pwndTeachProperties->IsDocked() ||
				((CMainFrame*)m_pMainWnd)->m_pwndTeachProperties->IsFloating())) {
				((CMainFrame*)m_pMainWnd)->m_pwndTeachProperties->InitPropList(m_pTracks[nTrack].m_pDoc[nDoc], nFOV);
			}
			//Vision Param Dialog
			if (((CMainFrame*)m_pMainWnd)->IsWindowVisible() &&
				(((CMainFrame*)m_pMainWnd)->m_pwndProperties->IsDocked() ||
				((CMainFrame*)m_pMainWnd)->m_pwndProperties->IsFloating())) {
				((CMainFrame*)m_pMainWnd)->m_pwndProperties->InitPropList(m_pTracks[nTrack].m_pDoc[nDoc], nFOV);
			}
		}
		
		
	}
}

void CApplication::OnInspInspectionSequence()
{
	m_bInspSequenceOffline = !m_bInspSequenceOffline;
	SynchronousCameraStream();
	if(!m_bInspSequenceOffline)
		 PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "InspSequenceCounter", m_nSequenceCounter);

	m_bLoadMultiImagesFromFile = FALSE;
}

void CApplication::OnUpdateInspInspectionSequence(CCmdUI *pCmdUI)
{
	BOOL bTrackLiveImg, bDocInsp;
	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
		bTrackLiveImg &= m_pTracks[nTrack].m_bLiveImage;
		bDocInsp &= m_pTracks[nTrack].m_pDoc[0]->m_bInspecting;
		

	}
	BOOL bStepInspectionEnabled = FALSE;
	BOOL bStoredInspectionEnabled = FALSE;
	for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
		for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
			bStepInspectionEnabled |= m_pTracks[nTrack].m_pDoc[nDoc]->m_bInspSavedImage;
			bStoredInspectionEnabled |= m_pTracks[nTrack].m_pDoc[nDoc]->m_bStoredImages;
		}
	}

	pCmdUI->Enable(theApp.m_bIsMasterApp && !m_bOnline &&
				   !bStepInspectionEnabled &&
				   !m_bTeaching &&
				   !m_bTrackInspSeq &&
				   !m_bInspecting &&
				   !bTrackLiveImg &&
				   !bDocInsp &&
				   !bStoredInspectionEnabled &&
				   (m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nInspectionSeq));
	pCmdUI->SetCheck(m_bInspSequenceOffline);
}

//void CApplication::OnCameraSnapAll()
//{
//	SynchronousCameraSnap();
//}
//
//void CApplication::OnUpdateCameraSnapAll(CCmdUI *pCmdUI)
//{
//	BOOL bTrackLiveImg, bTrackContInsp, bDocInsp,bLiveSingle;
//	for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
//		bTrackLiveImg &= m_pTracks[nTrack].m_bLiveImage;
//		bTrackContInsp &= m_pTracks[nTrack].m_bContInsp;
//		bDocInsp &= m_pTracks[nTrack].m_pDoc[0]->m_bInspecting;
//		for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
//			bLiveSingle &= m_pTracks[nTrack].m_pDoc[nDoc]->m_bLiveImageSingle;
//		}
//	}
//	
//	pCmdUI->Enable(m_pTracks[0].m_bCamAvail && m_pTracks[1].m_bCamAvail && m_pTracks[2].m_bCamAvail &&
//				   m_pTracks[0].m_bCamEnable && m_pTracks[1].m_bCamEnable && m_pTracks[2].m_bCamEnable &&
//				   !bTrackLiveImg &&
//				   !bTrackContInsp &&
//				   !bDocInsp &&
//				   !bLiveSingle &&
//				   !m_bOnline &&
//				   !m_bTeaching &&
//				   !m_bInspecting &&
//				   !m_bTrackInspSeq &&
//				   (m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nGrabAll));
//}


int CApplication::InspectionScanSequenceOffline()
{
	if (!m_bInspSequenceOffline)
		return -1;

	int nBlockX, nBlockY;
	int nDieNoPerBlockX, nDieNoPerBlockY;
	int nDeviceNo;
	int nFovNo;
	int nError = 0;
	int nErrResult[MAX_DIES];
	CPerformanceTimer nDieTimer;
	nDieTimer.Reset();
	motionParameters.GetMappingParameters(nBlockX, nBlockY, nDieNoPerBlockX, nDieNoPerBlockY);

	int m_nNoOfDies = nBlockX * nBlockY;
	int m_nNoOfFov = nDieNoPerBlockX * nDieNoPerBlockY;

//	GetDieGrabNumber()
	nDeviceNo = nBlockX * nBlockY;
	nFovNo = nDieNoPerBlockX * nDieNoPerBlockY;

	for (int nDie = 0; nDie < nDeviceNo; nDie++) {
		for (int nFov = 0; nFov < nFovNo; nFov++) {
			for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
				for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
					m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDie][nFov].ClrImg();
				}
			}
		}
	}

	//// ---- Die loop ---- ////
	BOOL waitInspection = FALSE;
	std::vector<std::vector <BOOL>> waitEventInspection;
	waitEventInspection.resize(m_nTotalTracks);
	for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
		waitEventInspection[nTrack].resize(m_pTracks[nTrack].getInspectionThreadCount(), FALSE);
	}

	for (int nDevice = 0; nDevice < nDeviceNo; nDevice++) {
		
		BOOL bDeviceInspect;
		nError = motionParameters.getDeviceStatus(nDevice, bDeviceInspect);

		if (nError) {
			OutputDebugLogTo(9, TRUE, "Error in reading Device Status %d", nDevice);
			continue;
		}
		OutputDebugLogTo(0, TRUE, "------- Start Scan Sequence Device %d, Device Inspect Status: %d -------", nDevice + 1, bDeviceInspect);

			
		if(!bDeviceInspect)
			continue;

		for (int nFov = 0; nFov < nFovNo; nFov++) {
			OutputDebugLogTo(0, TRUE, "Start Scan Sequence FOV %d", nFov + 1);

			int nDiePos;
			int nFovInSequence;

			if (m_bCurtainSensor || m_bEmergencyStop || m_bReceivedStopCmd) {
				OutputDebugLogTo(9, TRUE, "m_bCurtainSensor %d, m_bEmergencyStop %d, m_bReceivedStopCmd %d", m_bCurtainSensor, m_bEmergencyStop, m_bReceivedStopCmd);
				return -1;
			}

			nError = motionParameters.getDieScanOrder(nFov, nFovInSequence);
			if (nError) {
				OutputDebugLogTo(9, TRUE, "Error in reading Device Scan Order, FOV = %d, nFovInSequence = %d", nFov, nFovInSequence);
				continue;
			}

			if (!m_bInspSequenceOffline){
				OutputDebugLogTo(9, TRUE, "m_bInspSequenceOffline is FALSE");
				return -1;
			}

			if (m_bCurtainSensor) {
				OutputDebugLogTo(9, TRUE, "Curtain Sensor Triggered");
				return -1;
			}

			nError = motionParameters.getDieMappingPos(nFovInSequence, nDiePos);
			if (nError) {
				OutputDebugLogTo(9, TRUE, "Failed to get Die Mapping Pos for FOV %d", nFovInSequence);

			}
			
			nError = motionParameters.MoveXYToPosition(nDevice, nDiePos);
			if (nError) {
				CString strMessage;
				strMessage.Format("Failed to move X and Y motors to FOV Position %d", nFovInSequence);
				AfxMessageBox(strMessage);
				return -1;
			}

			for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
				m_pTracks[nTrack].m_nFOVIndex = nFovInSequence;
				m_pTracks[nTrack].m_nDieIndex = nDevice;
			}

			nError = SequenceScanCameraSnap();
			if (nError == -2) {
				OutputDebugLogTo(9, TRUE, "CApplication::InspectionScanSequenceOffline Sequence Stopped");//// for testing
				return -1;
			}
			else if (nError  && nError != -2) {
				OutputDebugLogTo(9, TRUE, "CApplication::InspectionScanSequenceOffline SynchronousCameraSnap Error");//// for testing
			}

			for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
				try {
					if (m_bMultithreadInspectionMode && m_pTracks[nTrack].getInspectionThreadCount() > 0) {
						EnterCriticalSection(&m_pTracks[nTrack].CriticalSectionScannedImage);
						int threadId = m_pTracks[nTrack].m_nFOVIndex % m_pTracks[nTrack].getInspectionThreadCount();
						m_pTracks[nTrack].grabbedImageId[threadId].push(m_pTracks[nTrack].m_nFOVIndex);
						m_pTracks[nTrack].grabbedDeviceId[threadId].push(m_pTracks[nTrack].m_nDieIndex);

						LeaveCriticalSection(&m_pTracks[nTrack].CriticalSectionScannedImage);

						SetEvent(m_pTracks[nTrack].eventDoInspection[threadId]);
						waitInspection = TRUE;
						waitEventInspection[nTrack][threadId] = TRUE;
					}
					else {
						nErrResult[nDevice] = m_pTracks[nTrack].m_pDoc[0]->InspectAllDocOnline(m_pTracks[nTrack].m_nDieIndex, m_pTracks[nTrack].m_nFOVIndex);

						//Send Defect Info
						if (theApp.m_bCentralizedVerificationMode && !theApp.m_CentralizedCommInterface.m_bIsTCPServer) {
							for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
								if (!theApp.motionParameters.IsGrabEnabled(nTrack, nDoc, m_pTracks[nTrack].m_nFOVIndex))
									continue;

								CInspectionHandler* pInspHandler = &m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[m_pTracks[nTrack].m_nFOVIndex];
								m_pImageQueue->PushDefectInfoCentralized(nTrack, nDoc, m_pTracks[nTrack].m_nDieIndex, m_pTracks[nTrack].m_nFOVIndex, pInspHandler->m_Inspection.m_DefectData[nDoc].arrayDefectCenters, pInspHandler->m_Inspection.m_DefectData[nDoc].arrayDefectRects,
									pInspHandler->m_Inspection.m_DefectData[nDoc].arrayDefectX, pInspHandler->m_Inspection.m_DefectData[nDoc].arrayDefectY);
							}
						}
					}
				}
				catch (...) {
					OutputDebugLogTo(0, FALSE, "Exception raised while inspecting at track %d", nTrack + 1);
					continue;
				}
			}
			OutputDebugLogTo(0, TRUE, "End Scan Sequence FOV %d", nFov + 1);
		}
		OutputDebugLogTo(0, TRUE, "------- End Scan Sequence Device %d -------", nDevice + 1);
	}
	
	//Go back to Start Scan Position
	ResetEvent(m_hInitPositionDone);
	SetEvent(m_hInitPosition);


	OutputDebugLogTo(0, TRUE, "Start Waiting Inspection Result");
	CPerformanceTimer timerInspStn1;
	if (waitInspection) {
		std::vector <HANDLE> eventInspection;
		for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
			if (m_pTracks[nTrack].getInspectionThreadCount() > 0) {
				for (int nThreadId = 0; nThreadId < m_pTracks[nTrack].getInspectionThreadCount(); nThreadId++) {
					if (waitEventInspection[nTrack][nThreadId]) {
						eventInspection.push_back(m_pTracks[nTrack].eventInspectionDone[nThreadId]);
					}
				}
			}
		}

		OutputDebugLogTo(0, TRUE, "InspectionScanSequenceOnline: waiting inspection, no threads: %d", eventInspection.size());
		DWORD dwError = WaitForMultipleObjects(eventInspection.size(), eventInspection.data(), TRUE, INFINITE);
		if (dwError == WAIT_FAILED || dwError == WAIT_TIMEOUT) {
			OutputDebugLogTo(7, TRUE, "-----ERROR: Image Inspection threads Waiting have been failed.");
			return 0;
		}
		eventInspection.clear();
	}
	OutputDebugLogTo(0, TRUE, "Inspection Station 1 Waiting Time: %.2fms", timerInspStn1.GetElapsedMSec());

	
	//Update UPH
	int nTempDieTimer = nDieTimer.GetElapsedMSec();
	UpdateUPH(nTempDieTimer, m_nNoOfDies);

	UpdateResults();

	if(m_bSaveInspSeqImages)
		m_nSequenceCounter++;

	if(!m_bInspSeqLoop){
		OnInspInspectionSequence();
	}
	else{
		//Wait till completely go to start scan position
		nError = theApp.motionParameters.WaitForStartPositionDone(5000);
		if (nError) {
			OutputDebugLogTo(7, TRUE, "-----ERROR: WaitForStartPositionDone failed");

			OnInspInspectionSequence();
		}
		else {
			if (m_bStream)
				m_bStream = FALSE;
		}
	}

	return 0;
}



int CApplication::InspectionScanSequenceOnline()
{
	if (!m_bReceivedStartCmd && !m_bInspSequenceOffline && !m_bReScanSequence)
		return -1;

	int nBlockX, nBlockY;
	int nDieNoPerBlockX, nDieNoPerBlockY;
	int nDeviceNo;
	int nFovNo;
	int nError = 0;
	int nErrResult[MAX_DIES];
	nErrResult[MAX_DIES] = { 98765 };
	CPerformanceTimer nDieTimer;
	nDieTimer.Reset();
	motionParameters.GetMappingParameters(nBlockX, nBlockY, nDieNoPerBlockX, nDieNoPerBlockY);

	int m_nNoOfDies = nBlockX * nBlockY;
	int m_nNoOfFov = nDieNoPerBlockX * nDieNoPerBlockY;

	//	GetDieGrabNumber()
	nDeviceNo = nBlockX * nBlockY;
	nFovNo = nDieNoPerBlockX * nDieNoPerBlockY;

	for (int nTrack = 0; nTrack<m_nTotalTracks; nTrack++)
		m_pTracks[nTrack].m_nFOVIndex = 0;

	if(!m_bOnline && (m_bReceivedStartCmd || m_bReScanSequence))
		return -1;

	ClearResults();

	for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
		for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
			for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
				for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
					m_pTracks[nTrack].m_pDoc[nDoc]->m_BufferDie[nDie][nFov].ClrImg();
				}
			}
		}
	}

	for(int nDie=0;nDie<m_nNoOfDies;nDie++){
		if((m_bReceivedStartCmd || m_bReScanSequence) && !m_bInspSequenceOffline) {
			motionParameters.setDeviceStatus(nDie, m_bPartInspect[nDie]);
			OutputDebugLogTo(9,TRUE,"CApplication::m_pStationMapDlg->m_bPosSelection[%d] = %d, m_bPartInspect[%d] = %d",nDie,m_pStationMapDlg->m_bPosSelection[nDie],nDie,m_bPartInspect[nDie]);//// for testing
		}
	}

	m_bIsFailPart = FALSE;

	if (m_bCentralizedVerificationMode && !m_CentralizedCommInterface.m_bIsTCPServer) {
		CPerformanceTimer timerFlushBuffer;
		m_pImageQueue->FlushQueueBufferCentralized();
		OutputDebugLogTo(5, TRUE, "FlushQueueBufferCentralized time: %.2fms", timerFlushBuffer.GetElapsedMSec());
		timerFlushBuffer.Reset();
		m_pImageQueue->FlushQueueDefectInfoCentralized();
		OutputDebugLogTo(5, TRUE, "FlushQueueDefectInfoCentralized time: %.2fms", timerFlushBuffer.GetElapsedMSec());
	}


	//// ---- Die loop ---- ////

	BOOL waitInspection = FALSE;
	std::vector<std::vector <BOOL>> waitEventInspection;
	waitEventInspection.resize(m_nTotalTracks);
	for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
		waitEventInspection[nTrack].resize(m_pTracks[nTrack].getInspectionThreadCount(), FALSE);
	}

	for (int nDevice = 0; nDevice < nDeviceNo; nDevice++) {

		BOOL bDeviceInspect;
		nError = motionParameters.getDeviceStatus(nDevice, bDeviceInspect);
		if (nError) {
			OutputDebugLogTo(9, TRUE, "Error in reading Device Status %d", nDevice);
			continue;
		}

		OutputDebugLogTo(0, TRUE, "------- Start Scan Sequence Device %d, Device Inspect Status: %d -------", nDevice + 1, bDeviceInspect);

		if (!bDeviceInspect)
			continue;

		if (m_bCentralizedVerificationMode && !m_CentralizedCommInterface.m_bIsTCPServer) {
			CString strMsg;
			strMsg.Format("Scanning THA %s", m_strPartFlex[nDevice]);
			m_CentralizedCommInterface.m_pCommManager[0]->SendDataMessage(SEND_MESSAGE, strMsg);
		}

		for (int nFov = 0; nFov < nFovNo; nFov++) {
			OutputDebugLogTo(0, TRUE, "Start Scan Sequence FOV %d", nFov + 1);

			if (m_bCurtainSensor || m_bEmergencyStop || m_bReceivedStopCmd) {
				OutputDebugLogTo(9, TRUE, "m_bCurtainSensor %d, m_bEmergencyStop %d, m_bReceivedStopCmd %d", m_bCurtainSensor, m_bEmergencyStop, m_bReceivedStopCmd);
				return -1;
			}

			int nDiePos;
			int nFovInSequence;

			nError = motionParameters.getDieScanOrder(nFov, nFovInSequence);
			if (nError) {
				OutputDebugLogTo(9, TRUE, "Error in reading Device Scan Order");
				continue;
			}

			if (!m_bInspSequenceOffline && !m_bReceivedStartCmd && !m_bReScanSequence) {
				OutputDebugLogTo(9, TRUE, "m_bInspSequenceOffline is FALSE");
				return -1;
			}

			if (m_bCurtainSensor) {
				OutputDebugLogTo(9, TRUE, "Curtain Sensor Triggered");
				return -1;
			}

			nError = motionParameters.getDieMappingPos(nFovInSequence, nDiePos);
			if (nError) {
				OutputDebugLogTo(9, TRUE, "Failed to get Die Mapping Pos for FOV %d", nFovInSequence);
				continue;
			}

			nError = motionParameters.MoveXYToPosition(nDevice, nDiePos);
			if (nError) {
				CString strMessage;
				strMessage.Format("Failed to move X and Y motors to FOV Position %d", nFovInSequence);
				AfxMessageBox(strMessage);
				return -1;
			}

			for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
				m_pTracks[nTrack].m_nFOVIndex = nFovInSequence;
				m_pTracks[nTrack].m_nDieIndex = nDevice;
			}

			nError = SequenceScanCameraSnap();
			if (nError == -2) {
				OutputDebugLogTo(9, TRUE, "CApplication::InspectionScanSequenceOnline Sequence Stopped");
				return -1;
			}
			else if (nError  && nError != -2) {
				OutputDebugLogTo(9, TRUE, "CApplication::InspectionScanSequenceOnline SynchronousCameraSnap Error");
			}

			for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
				try {
					if (m_bMultithreadInspectionMode && m_pTracks[nTrack].getInspectionThreadCount() > 0) {
						EnterCriticalSection(&m_pTracks[nTrack].CriticalSectionScannedImage);
						int threadId = m_pTracks[nTrack].m_nFOVIndex % m_pTracks[nTrack].getInspectionThreadCount();
						m_pTracks[nTrack].grabbedImageId[threadId].push(m_pTracks[nTrack].m_nFOVIndex);
						m_pTracks[nTrack].grabbedDeviceId[threadId].push(m_pTracks[nTrack].m_nDieIndex);

						LeaveCriticalSection(&m_pTracks[nTrack].CriticalSectionScannedImage);

						SetEvent(m_pTracks[nTrack].eventDoInspection[threadId]);
						waitInspection = TRUE;
						waitEventInspection[nTrack][threadId] = TRUE;
					}
					else {
						nErrResult[nDevice] = m_pTracks[nTrack].m_pDoc[0]->InspectAllDocOnline(m_pTracks[nTrack].m_nDieIndex, m_pTracks[nTrack].m_nFOVIndex);

						//Send Defect Info
						if (theApp.m_bCentralizedVerificationMode && !theApp.m_CentralizedCommInterface.m_bIsTCPServer) {
							for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
								if (!theApp.motionParameters.IsGrabEnabled(nTrack, nDoc, m_pTracks[nTrack].m_nFOVIndex))
									continue;

								CInspectionHandler* pInspHandler = &m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[m_pTracks[nTrack].m_nFOVIndex];
								m_pImageQueue->PushDefectInfoCentralized(nTrack, nDoc, m_pTracks[nTrack].m_nDieIndex, m_pTracks[nTrack].m_nFOVIndex, pInspHandler->m_Inspection.m_DefectData[nDoc].arrayDefectCenters, 
									pInspHandler->m_Inspection.m_DefectData[nDoc].arrayDefectRects, pInspHandler->m_Inspection.m_DefectData[nDoc].arrayDefectX, pInspHandler->m_Inspection.m_DefectData[nDoc].arrayDefectY);
							}
						}
					}
				}
				catch (...) {
					OutputDebugLogTo(0, FALSE, "Exception raised while inspecting at track %d", nTrack + 1);
					continue;
				}
			}

			m_bDieInspection[nDevice][nFovInSequence] = TRUE;
			OutputDebugLogTo(0, TRUE, "End Scan Sequence FOV %d", nFov + 1);
		}
		OutputDebugLogTo(0, TRUE, "------- End Scan Sequence Device %d -------", nDevice + 1);
	}

	//Go back to Start Scan Position
	ResetEvent(m_hInitPositionDone);
	SetEvent(m_hInitPosition);

	OutputDebugLogTo(0, TRUE, "Start Waiting Inspection Result");
	CPerformanceTimer timerInspStn1;
	if (waitInspection) {
		std::vector <HANDLE> eventInspection;
		for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
			if (m_pTracks[nTrack].getInspectionThreadCount() > 0) {
				for (int nThreadId = 0; nThreadId < m_pTracks[nTrack].getInspectionThreadCount(); nThreadId++) {
					if (waitEventInspection[nTrack][nThreadId]) {
						eventInspection.push_back(m_pTracks[nTrack].eventInspectionDone[nThreadId]);
					}
				}
			}
		}

		DWORD dwError = WaitForMultipleObjects(eventInspection.size(), eventInspection.data(), TRUE, INFINITE);
		if (dwError == WAIT_FAILED || dwError == WAIT_TIMEOUT) {
			OutputDebugLogTo(7, TRUE, "-----ERROR: Image Inspection threads Waiting have been failed.");
			return 0;
		}
		eventInspection.clear();
	}
	OutputDebugLogTo(0, TRUE, "Inspection Station 1 Waiting Time: %.2fms", timerInspStn1.GetElapsedMSec());

	//Update UPH
	int nTempDieTimer = nDieTimer.GetElapsedMSec();
	UpdateUPH(nTempDieTimer, m_nNoOfDies);

	UpdateResults();
	return 0;
}

void CApplication::MoveToStartPos(BOOL bHome)
{
	if (!m_pMotionControl) {
		OutputDebugLogTo(9, TRUE, "[CApplication::MoveToStartPos] Motion Control has not been Initialized!");
		return;
	}
	if(m_strMotionCard.CompareNoCase("none") == 0)
		return;

	if (bHome) {
		ShowMessageDlgBox(TRUE);

		int nError = motionParameters.StopEveryAxis();

		if (!motionParameters.IsMotorHomed(X_SERVO_MOTOR)) {

			nError = motionParameters.HomeMove(X_SERVO_MOTOR);
			if (nError) {
				OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
				return;
			}
		}

		if (!motionParameters.IsMotorHomed(Y_SERVO_MOTOR)) {
			nError = motionParameters.HomeMove(Y_SERVO_MOTOR);
			if (nError) {
				OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
				return;
			}
		}

		if (!motionParameters.IsMotorHomed(Z_SERVO_TOP_MOTOR)) {
			nError = motionParameters.HomeMove(Z_SERVO_TOP_MOTOR);
			if (nError) {
				OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
				return;
			}
		}

		if (!motionParameters.IsMotorHomed(Z_SERVO_BOTTOM_MOTOR)) {
			nError = motionParameters.HomeMove(Z_SERVO_BOTTOM_MOTOR);
			if (nError) {
				OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
				return;
			}
		}

		if (!motionParameters.IsMotorHomed(X_SERVO_MOTOR)) {

			nError = motionParameters.WaitForMotionDone(X_SERVO_MOTOR, getMotionTimeOut());
			if (nError) {
				OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
				return;
			}

			nError = motionParameters.HomeDone(X_SERVO_MOTOR);
			if (nError) {
				OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
				return;
			}

		}

		if (!motionParameters.IsMotorHomed(Y_SERVO_MOTOR)) {
			nError = motionParameters.WaitForMotionDone(Y_SERVO_MOTOR, getMotionTimeOut());
			if (nError) {
				OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
				return;
			}

			nError = motionParameters.HomeDone(Y_SERVO_MOTOR);
			if (nError) {
				OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
				return;
			}
		}

		if (!motionParameters.IsMotorHomed(Z_SERVO_TOP_MOTOR)) {
			nError = motionParameters.WaitForMotionDone(Z_SERVO_TOP_MOTOR, getMotionTimeOut()*10);
			if (nError) {
				OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
				return;
			}

			nError = motionParameters.HomeDone(Z_SERVO_TOP_MOTOR);
			if (nError) {
				OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
				return;
			}

		}

		if (!motionParameters.IsMotorHomed(Z_SERVO_BOTTOM_MOTOR)) {
			nError = motionParameters.WaitForMotionDone(Z_SERVO_BOTTOM_MOTOR, getMotionTimeOut()*10);
			if (nError) {
				OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
				return;
			}

			nError = motionParameters.HomeDone(Z_SERVO_BOTTOM_MOTOR);
			if (nError) {
				OutputDebugLogTo(0, TRUE, "----- ERROR : Home move for X motor failed");
				return;
			}
		}

//		m_pMotionControl->Stop(-1);////---- Stop all axes if running ---- ////
//		Sleep(100);

//		nError = motionParameters.HomeMoveEveryAxis();

//		nError = m_pMotionControl->Homing(-1);//// ---- home all axes ---- ////
		if (nError)
			OutputDebugLogTo(9, TRUE, "CApplication::MoveToStartPos Homing Error");//// for testing

		ShowMessageDlgBox(FALSE);
	}
}

int CApplication::WaitForPositionToEnd()
{
	DWORD dwObj;
	OutputDebugLogTo(9,TRUE,"CApplication::WaitForPositionToEnd Waiting...");//// for testing

	dwObj = WaitForSingleObject(m_pTracks[0].m_EventPositionEnd, INFINITE);
	if(dwObj==WAIT_TIMEOUT){
		OutputDebugLog("=== WaitForPositionToEnd Error");
		return -1;
	}
	OutputDebugLogTo(9, TRUE, "CApplication::WaitForPositionToEnd Success");//// for testing
	return 0;
}

int CApplication::CalculateFOVWidthAndHeight()
{
	double dWidthMM, dHeightMM;
	double dMicronsPerPixelX, dMicronsPerPixelY;
	CRect rcFOV; 
	CSize szImageSize;

	dMicronsPerPixelX = m_pTracks[0].m_dResolutionX;
	dMicronsPerPixelY = m_pTracks[0].m_dResolutionY;	//// 5.5 microns per pixels

	rcFOV = CRect(CPoint(0,0), m_pTracks[0].m_pDoc[0]->m_Buffer.GetImgSize());
	szImageSize = rcFOV.Size();

	dWidthMM = (dMicronsPerPixelX*szImageSize.cx)/1000.0;	//// converting to mm
	dHeightMM = (dMicronsPerPixelY*szImageSize.cy)/1000.0;	//// converting to mm

	m_nFOVWidthCalculated = (int)dWidthMM;
	m_nFOVHeightCalculated = (int)dHeightMM;
	
	return 0;
}

int CApplication::CalculateFOVAndDieNumber(BOOL bCreate)
{

	double dScanAreaX, dScanAreaY;
	double dOverlapX, dOverlapY;
	int DeviceNoPerX, DeviceNoPerY;
	int nFovPerX, nFovPerY;

	motionParameters.getDeviceScanArea(dScanAreaX, dScanAreaY);
	motionParameters.getDeviceOverlap(dOverlapX, dOverlapY);
	motionParameters.GetMappingParameters(DeviceNoPerX, DeviceNoPerY, nFovPerX, nFovPerY);

	int m_nNoOfFovX = nFovPerX;
	int m_nNoOfFovY = nFovPerY;
	double dTempFOVX, dTempFOVY, dTempDie;
	dTempFOVX = ceil((dScanAreaX / (m_nFOVWidth- dOverlapX)));
	dTempFOVY = ceil((dScanAreaY / (m_nFOVHeight - dOverlapY)));

	int m_nNoOfDies = DeviceNoPerX * DeviceNoPerX;
	int m_nNoOfFov = nFovPerX * nFovPerY;

	if(dTempFOVX > 0)
		m_nNoOfFovX = (int)(dTempFOVX)!= m_nNoOfFovX? (int)(dTempFOVX): m_nNoOfFovX;
	if (dTempFOVY > 0)
		m_nNoOfFovY = (int)(dTempFOVY)!= m_nNoOfFovY? (int)(dTempFOVY): m_nNoOfFovY;

	for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
		for (int nFOV = 0; nFOV < m_nNoOfFov; nFOV++) {
			m_colorFovDieMapping[nDie][nFOV].DeleteObject();
		}
	}

	m_nNoOfFov = m_nNoOfFovX*m_nNoOfFovY;
	if (m_nNoOfFov <.0 || m_nNoOfFov > 8.0) {
		m_nNoOfFov = 8;
	}

	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Number of FOV X", m_nNoOfFovX);
	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Number of FOV Y", m_nNoOfFovY);

	int nTotalFov;
	if (DeviceNoPerX > 0 && DeviceNoPerY > 0) {
		m_nNoOfDies = DeviceNoPerX * DeviceNoPerY;
		PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "Number of Dies", m_nNoOfDies);
	}
	for (int nDie = 0; nDie < m_nNoOfDies; nDie++) {
		for (int nFOV = 0; nFOV < m_nNoOfFov; nFOV++) {
			m_colorFovDieMapping[nDie][nFOV].CreateSolidBrush(RGB(125, 125, 120));
		}
	}

	ResizeDieSelectionDlg();
	
	/*if(bCreate)
	{
		((CMainFrame*)m_pMainWnd)->ReInitializePropertiesWindow();
	}*/

	return 0;
}

//void CApplication::ScannedFOVSequenceData(BOOL bRead)
//{
//	CIniFile iniFile;
//	CFileException e;
//
//	CString strKey, strFile, strTemp;
//	strFile.Format("%s\\%s\\%s\\ScanningFOVSequenceParam.dat", m_strMainCfgDir, m_strMainCfgModeDir, m_strConfigFile);
//	iniFile.SetPath(strFile);
//	
//	BOOL bFileRead = iniFile.ReadFile();
//	if(bRead && bFileRead) {
//		strKey.Format("ScanningFOVSequenceParam");
//		//for(int nTrack=0; nTrack<m_nTotalTracks-1; nTrack++) {
//			for(int nFOV=0; nFOV<m_nNoOfFov; nFOV++) {
//				/*strTemp.Format("T%d FOV%d %s", nTrack+1, nFOV+1, !nTrack?"TopXPos":"BotXPos");
//				m_pTracks[nTrack].m_nXAxisPosition[nFOV] = iniFile.GetInteger(strKey, strTemp, m_pTracks[nTrack].m_nXAxisPosition[nFOV]);
//				strTemp.Format("T%d FOV%d %s", nTrack+1, nFOV+1, !nTrack?"TopYPos":"BotYPos");
//				m_pTracks[nTrack].m_nYAxisPosition[nFOV] = iniFile.GetInteger(strKey, strTemp, m_pTracks[nTrack].m_nYAxisPosition[nFOV]);*/
//				strTemp.Format("FOV%d %s", nFOV+1, "XPos");
//				m_nXAxisPosition[nFOV] = iniFile.GetInteger(strKey, strTemp, m_nXAxisPosition[nFOV]);
//				strTemp.Format("FOV%d %s", nFOV+1, "YPos");
//				m_nYAxisPosition[nFOV] = iniFile.GetInteger(strKey, strTemp, m_nYAxisPosition[nFOV]);
//				for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
//					for(int nDoc=0; nDoc<m_nTotalViewDocs[nTrack]; nDoc++) {
//						strTemp.Format("T%d FOV%d Doc%d %s", nTrack+1, nFOV+1, nDoc+1, !nTrack?"TopZPos":"BotZPos");
//						m_pTracks[nTrack].m_pDoc[nDoc]->m_nZAxisPosition[nFOV] = iniFile.GetInteger(strKey, strTemp, m_pTracks[nTrack].m_pDoc[nDoc]->m_nZAxisPosition[nFOV]);
//					}
//				}
//			}
//	}
//	else /*if(!bRead)*/ {
//		strKey.Format("ScanningFOVSequenceParam");
//		//for(int nTrack=0; nTrack<m_nTotalTracks-1; nTrack++) {
//			for(int nFOV=0; nFOV<m_nNoOfFov; nFOV++) {
//				/*strTemp.Format("T%d FOV%d %s", nTrack+1, nFOV+1, !nTrack?"TopXPos":"BotXPos");
//				iniFile.SetInteger(strKey, strTemp, m_pTracks[nTrack].m_nXAxisPosition[nFOV]);
//				strTemp.Format("T%d FOV%d %s", nTrack+1, nFOV+1, !nTrack?"TopYPos":"BotYPos");
//				iniFile.SetInteger(strKey, strTemp, m_pTracks[nTrack].m_nYAxisPosition[nFOV]);*/
//				strTemp.Format("FOV%d %s", nFOV+1, "XPos");
//				iniFile.SetInteger(strKey, strTemp, m_nXAxisPosition[nFOV]);
//				strTemp.Format("FOV%d %s", nFOV+1, "YPos");
//				iniFile.SetInteger(strKey, strTemp, m_nYAxisPosition[nFOV]);
//				for(int nTrack=0; nTrack<m_nTotalTracks; nTrack++) {
//					for(int nDoc=0; nDoc<m_nTotalViewDocs[nTrack]; nDoc++) {
//						strTemp.Format("T%d FOV%d Doc%d %s", nTrack+1, nFOV+1, nDoc+1, !nTrack?"TopZPos":"BotZPos");
//						iniFile.SetInteger(strKey, strTemp, m_pTracks[nTrack].m_pDoc[nDoc]->m_nZAxisPosition[nFOV]);
//					}
//				}
//		}
//		iniFile.WriteFile();
//	}
//}



void CApplication::ShowMessageDlgBox(BOOL bTrue)
{
	if (!m_MotorResource.isDllOpened() || m_pDisplayMsgStatus == NULL)
		return;

	CRect rect1, rect2;
	//SystemParametersInfo(SPI_GETWORKAREA, NULL, &rect1, 0);
	m_pDisplayMsgStatus->GetWindowRect(&rect2);
	//m_pDisplayMsgStatus->SetWindowPos(NULL, (int)(rect1.Width()/2 - rect2.Width()/2), rect2.Height(), rect2.Width(), rect2.Height(), SWP_NOSIZE);
	CMFCRibbonBar* pRibbon = &((CMainFrame*)m_pMainWnd)->m_wndRibbonBar;
	CMFCRibbonBaseElement* pElement = pRibbon->FindByID(ID_INSP_SAVE_SEQ_IMAGES, TRUE, TRUE);
	rect1 = pElement->GetRect();
	m_pDisplayMsgStatus->SetWindowPos(NULL, (int)(rect1.right+20), (int)(rect1.top+10), rect2.Width(), rect2.Height(), SWP_NOZORDER);

	m_pDisplayMsgStatus->ShowWindow(bTrue);
	m_pDisplayMsgStatus->GetDlgItem(IDC_STATIC_MSG_INFO)->SetWindowTextA("Please Wait... All Motors Moving To Home");
}

void CApplication::ShowMessageDlgBox(CString strMessage, BOOL bShow)
{
	if (!m_MotorResource.isDllOpened())
		return;

	CRect rect1, rect2;
	//SystemParametersInfo(SPI_GETWORKAREA, NULL, &rect1, 0);
	m_pDisplayMsgStatus->GetWindowRect(&rect2);
	//m_pDisplayMsgStatus->SetWindowPos(NULL, (int)(rect1.Width()/2 - rect2.Width()/2), rect2.Height(), rect2.Width(), rect2.Height(), SWP_NOSIZE);
	CMFCRibbonBar* pRibbon = &((CMainFrame*)m_pMainWnd)->m_wndRibbonBar;
	CMFCRibbonBaseElement* pElement = pRibbon->FindByID(ID_INSP_SAVE_SEQ_IMAGES, TRUE, TRUE);
	rect1 = pElement->GetRect();
	m_pDisplayMsgStatus->SetWindowPos(NULL, (int)(rect1.right + 20), (int)(rect1.top + 10), rect2.Width(), rect2.Height(), SWP_NOZORDER);

	m_pDisplayMsgStatus->ShowWindow(bShow);
	m_pDisplayMsgStatus->GetDlgItem(IDC_STATIC_MSG_INFO)->SetWindowTextA(strMessage.GetString());
}


void CApplication::OnInspSequenceLoop()
{
	m_bInspSeqLoop = !m_bInspSeqLoop;
}

void CApplication::OnUpdateInspSequenceLoop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.m_bIsMasterApp && m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nInspSeqLoop);
	pCmdUI->SetCheck(m_bInspSeqLoop);
}


void CApplication::OnInspSaveSequenceImages()
{
	m_bSaveInspSeqImages = !m_bSaveInspSeqImages;

	if(!CheckDiskFreeSpace())
		m_bSaveInspSeqImages = FALSE;
	else {
		if(m_bSaveInspSeqImages == TRUE)
			AddLogEvent("Inspect Sequence Save Images enabled");
		else 
			AddLogEvent("Inspect Sequence Save Images Disabled");
	}

	PutRegistryEntry(WRITE_REGPATH + "Preferences\\Persistence", "SaveInspectSeqImages", m_bSaveInspSeqImages);
}

void CApplication::OnUpdateInspSaveSequenceImages(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.m_bIsMasterApp && m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nSaveInspSeqImages);
	pCmdUI->SetCheck(m_bSaveInspSeqImages);
}

void CApplication::OnPixelRuler()
{
	m_bPixelRuler = !m_bPixelRuler;
	if(m_bPixelRuler) {
		m_pPixelRulerDlg = new CPixelRuler();
		if(!m_pPixelRulerDlg->Create(IDD_DIALOG_PIXEL_RULER, m_pMainWnd)) {
			delete m_pPixelRulerDlg;
			m_pPixelRulerDlg = NULL;
			return ;
		}
		CRect rect1, rect2;
		m_pMainWnd->GetWindowRect(&rect1);
		m_pPixelRulerDlg->GetWindowRect(&rect2);

		m_pPixelRulerDlg->SetWindowPos(NULL, rect1.right - rect2.Width() - 130, rect1.top + 60, 0, 0, SWP_NOSIZE);
		m_pPixelRulerDlg->ShowWindow(TRUE);
		m_pMainWnd->SetFocus();
	}
	else {
		if(m_pPixelRulerDlg)
			delete m_pPixelRulerDlg;
		for(int i=0;i<m_nTotalTracks;i++){
			for(int j=0; j<m_nTotalViewDocs[i]; j++) {
				DrwDel(&m_pTracks[i].m_pDoc[j]->m_ImageViewInfo);
				DrwUpdate(&m_pTracks[i].m_pDoc[j]->m_ImageViewInfo);
				//m_StationHandler[i].m_pDoc[j]->UpdateAllViews(FALSE);	
			}
		}
	}
}

void CApplication::OnUpdatePixelRuler(CCmdUI *pCmdUI)
{
	BOOL bTrackLiveImg, bDocInsp;
	for (int nTrack = 0; nTrack<m_nTotalTracks; nTrack++) {
		bTrackLiveImg &= m_pTracks[nTrack].m_bLiveImage;
		bDocInsp &= m_pTracks[nTrack].m_pDoc[0]->m_bInspecting;


	}
	BOOL bStepInspectionEnabled = FALSE;
	BOOL bStoredInspectionEnabled = FALSE;
	for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
		for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
			bStepInspectionEnabled |= m_pTracks[nTrack].m_pDoc[nDoc]->m_bInspSavedImage;
			//bStoredInspectionEnabled |= m_pTracks[nTrack].m_pDoc[nDoc]->m_bStoredImages;
		}
	}

	pCmdUI->Enable(!m_bOnline &&
		!bStepInspectionEnabled &&
		!m_bTeaching &&
		!m_bTrackInspSeq &&
		!m_bInspecting &&
		!bTrackLiveImg &&
		!bDocInsp &&
		!bStoredInspectionEnabled &&
		(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nPixelRuler));
	pCmdUI->SetCheck(m_bPixelRuler);
}

void CApplication::EnableMachineResetOperation(BOOL enable)
{
	m_bEnableMachineResetOperation = enable;
}

BOOL CApplication::GetEnableMachineResetOperationStatus() const
{
	return m_bEnableMachineResetOperation;
}

int CApplication::OnReceiveResetCmd()
{
	if(m_bReceivedResetCmd){
		if(m_bEmergencyStop)
			return -1;
		else if(m_bCurtainSensor)
			return -2;
		else {
			if (GetEnableMachineResetOperationStatus()) {
				ShowMessageDlgBox(TRUE);
				
				ResetEvent(m_hResetDone);
				SetEvent(m_hResetReceived);

				if (WaitForSingleObject(m_hResetDone, getMotionTimeOut() * 2) != WAIT_OBJECT_0) {
					OutputDebugLogTo(7, true, "MOTORS FAILED TO RESET");
					m_bReceivedResetCmd = FALSE;
					m_bReceivedResetCmdPrevStatus = m_bReceivedResetCmd;
					ShowMessageDlgBox(FALSE);
					return -3;
				}
				
				ShowMessageDlgBox(FALSE);
				SetEvent(m_EventMotionSensor);
				SetEvent(m_eventOnline);
				ResetEvent(m_EventMotionSensorStart);
				ResetEvent(m_hInitPositionDone);
				SetEvent(m_hInitPosition);
			}
			else {
				if (m_bReceivedStopCmd) {
					ResetEvent(m_hInitPositionDone);
					SetEvent(m_hInitPosition);
				}
			}

			m_bStream = FALSE;
			m_bReceivedResetCmd = FALSE;
			m_bReceivedResetCmdPrevStatus = m_bReceivedResetCmd;
			m_bReceivedStopCmd = FALSE;
		}
	}
	return 0;
}

void CApplication::UpdateStatistics(int nDie)
{
	((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.UpdateLotInfoMID(nDie);
}

int CApplication::UpdateInfoMsgDlg(CString strCamDocName, CString strInfoMsg)
{
	if (m_pInfoMsgDlg != NULL) {
		if (m_pInfoMsgDlg->IsWindowVisible()) {
			m_pInfoMsgDlg->UpdateDlgItems(strCamDocName, strInfoMsg);
		}
	}

	return 0;
}

//void CApplication::OnToolsDryRun()
//{
//	m_bDryRunMachine = !m_bDryRunMachine;
//	PutRegistryEntry(WRITE_REGPATH + "Preferences", "Machine Dry Run: Enable", m_bDryRunMachine);
//
//	CString strEvent;
//	strEvent.Format("Dry Run Machine Sequence %s", m_bDryRunMachine ? "ENABLED" : "DISABLED");
//	AddLogEvent(strEvent);
//	OutputDebugLogTo(0, TRUE, "Dry Run Machine Sequence %s", m_bDryRunMachine ? "ENABLED" : "DISABLED");
//}

//void CApplication::OnUpdateToolsDryRun(CCmdUI *pCmdUI) {
//
//	pCmdUI->Enable(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nDryRun);
//	pCmdUI->SetCheck(m_bDryRunMachine);
//}


void CApplication::InitializeDefectCode()
{
	if (m_pDefectCode != NULL) {
		delete[] m_pDefectCode;
		m_pDefectCode = NULL;
	}

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);
	
	int m_nNoOfFov = nFovPerX * nFovPerY;

	int nView = -1;
	int nPviDefectCount = 0;
	for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
		m_strPviDefectsStats[nFov].RemoveAll();
		for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
			for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
				nView = !nTrack ? nDoc : (nTrack == 1 ? m_nTop + nDoc : m_nTop + m_nBottom + nDoc);
				int nAreaCount = m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.m_TrainingData.nNoOfPviArea;
				m_nPviNumberOfArea[nFov][nView] = nAreaCount;
				for (int nArea = 0; nArea < nAreaCount; nArea++) {
					int nAreaDefectCount = m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.m_pPviInspectionSetup[nArea].nDefectCount;
					m_nPviNoOfDefects[nFov][nView][nArea] = nAreaDefectCount;
					for (int nDefect = 0; nDefect < nAreaDefectCount; nDefect++) {
						nPviDefectCount++;
					}
				}
			}
		}
	}
	m_nTotalDefectsCount = 0;
	m_nNo_of_Defects = nPviDefectCount + 20;
//	m_pDefectCode =new CDefectCodes[m_nNo_of_Defects];
	int nGroup = 1;

	int nGroupId = 1;
	int nDefectGroupId = 1;
	m_pImageSelectionDlg->m_wndDlg.addGroup("Device Location", nGroupId++);
	m_pImageSelectionDlg->m_wndDlg.addDefect("Device Location", "Device Location", -DEVICE_LOCATION, nDefectGroupId++, RGB(0, 0, 255), false);

	m_pImageSelectionDlg->m_wndDlg.addGroup("Tilt Inspection", nGroupId++);
	m_pImageSelectionDlg->m_wndDlg.addDefect("Tilt Inspection", "Tilt Inspection", -TILT_INSP, nDefectGroupId++, RGB(19, 59, 69), false);

	m_pImageSelectionDlg->m_wndDlg.addGroup("Cover Layer Inspection", nGroupId++);
	m_pImageSelectionDlg->m_wndDlg.addDefect("Cover Layer Inspection", "Cover Layer Inspection", -COVER_LAYER_INSP, nDefectGroupId++, RGB(19, 59, 69), false);

	m_pImageSelectionDlg->m_wndDlg.addGroup("Cover Layer Alignment", nGroupId++);
	m_pImageSelectionDlg->m_wndDlg.addDefect("Cover Layer Alignment", "Cover Layer Alignment", -COVER_LAYER_ALIGNMENT, nDefectGroupId++, RGB(19, 59, 69), false);

	m_pImageSelectionDlg->m_wndDlg.addGroup("Encap Insufficient", nGroupId++);
	m_pImageSelectionDlg->m_wndDlg.addDefect("Encap Insufficient", "Encap Insufficient", -ENCAP_INSUFFICIENT, nDefectGroupId++, RGB(19, 59, 69), false);

	m_pImageSelectionDlg->m_wndDlg.addGroup("Encap Excess", nGroupId++);
	m_pImageSelectionDlg->m_wndDlg.addDefect("Encap Excess", "Encap Excess", -ENCAP_EXCESS, nDefectGroupId++, RGB(19, 59, 69), false);

	m_pImageSelectionDlg->m_wndDlg.addGroup("Encap Crack", nGroupId++);
	m_pImageSelectionDlg->m_wndDlg.addDefect("Encap Crack", "Encap Crack", -ENCAP_CRACK, nDefectGroupId++, RGB(19, 59, 69), false);

	m_pImageSelectionDlg->m_wndDlg.addGroup("Encap Blow Hole", nGroupId++);
	m_pImageSelectionDlg->m_wndDlg.addDefect("Encap Blow Hole", "Encap Blow Hole", -ENCAP_BLOWHOLE, nDefectGroupId++, RGB(19, 59, 69), false);

	m_pImageSelectionDlg->m_wndDlg.addGroup("Encap Contamination", nGroupId++);
	m_pImageSelectionDlg->m_wndDlg.addDefect("Encap Contamination", "Encap Contamination", -ENCAP_CONTAMINATION, nDefectGroupId++, RGB(19, 59, 69), false);

	m_pImageSelectionDlg->m_wndDlg.addGroup("Slot", nGroupId++);
	m_pImageSelectionDlg->m_wndDlg.addDefect("Slot", "Slot", -SLOT, nDefectGroupId++, RGB(19, 59, 69), false);

	m_pImageSelectionDlg->m_wndDlg.addGroup("Die Edge", nGroupId++);
	m_pImageSelectionDlg->m_wndDlg.addDefect("Die Edge", "Die Edge", -DIE_EDGE, nDefectGroupId++, RGB(19, 59, 69), false);


	/*int nIndex = 0;	
	m_pDefectCode[nIndex].m_nID				= GOOD_UNIT;
	m_pDefectCode[nIndex].strDefectName		= "Good Unit";
	m_pDefectCode[nIndex].nErrorCode			= 0;
	m_pDefectCode[nIndex].colorDefect = RGB(0, 0, 255);
	nIndex++;
	m_pDefectCode[nIndex].m_nID				= DEVICE_LOCATION;
	m_pDefectCode[nIndex].strDefectName		= "Device Location";
	m_pDefectCode[nIndex].nErrorCode			= 0;
	m_pDefectCode[nIndex].colorDefect		= RGB(255, 0, 0);
	nIndex++;
	m_pDefectCode[nIndex].m_nID				= DATUM_LOCATION;
	m_pDefectCode[nIndex].strDefectName		= "Datum Location";
	m_pDefectCode[nIndex].nErrorCode			= 0;
	m_pDefectCode[nIndex].colorDefect		= RGB(255, 0, 0);
	nIndex++;
	m_pDefectCode[nIndex].m_nID				= TILT_INSP;
	m_pDefectCode[nIndex].strDefectName		= "Tilt Inspection";
	m_pDefectCode[nIndex].nErrorCode			= 0;
	m_pDefectCode[nIndex].colorDefect		= RGB(255, 0, 0);
	nIndex++;
	m_pDefectCode[nIndex].m_nID				= COVER_LAYER_INSP;
	m_pDefectCode[nIndex].strDefectName		= "Cover Layer Inspection";
	m_pDefectCode[nIndex].nErrorCode			= 0;
	m_pDefectCode[nIndex].colorDefect		= RGB(255, 0, 0);
	nIndex++;
	m_pDefectCode[nIndex].m_nID = COVER_LAYER_ALIGNMENT;
	m_pDefectCode[nIndex].strDefectName = "Cover Layer Alignment";
	m_pDefectCode[nIndex].nErrorCode = 0;
	m_pDefectCode[nIndex].colorDefect = RGB(255, 0, 0);
	nIndex++;
	m_pDefectCode[nIndex].m_nID				= ENCAP_INSUFFICIENT;
	m_pDefectCode[nIndex].strDefectName		= "Encap Insufficient";
	m_pDefectCode[nIndex].nErrorCode			= 0;
	m_pDefectCode[nIndex].colorDefect		= RGB(255, 0, 0);
	nIndex++;
	m_pDefectCode[nIndex].m_nID				= ENCAP_EXCESS;
	m_pDefectCode[nIndex].strDefectName		= "Encap Excess";
	m_pDefectCode[nIndex].nErrorCode			= 0;
	m_pDefectCode[nIndex].colorDefect		= RGB(255, 0, 0);
	nIndex++;
	m_pDefectCode[nIndex].m_nID = ENCAP_CRACK;
	m_pDefectCode[nIndex].strDefectName = "Encap Crack";
	m_pDefectCode[nIndex].nErrorCode = 0;
	m_pDefectCode[nIndex].colorDefect = RGB(255, 0, 0);
	nIndex++;
	m_pDefectCode[nIndex].m_nID				= ENCAP_BLOWHOLE;
	m_pDefectCode[nIndex].strDefectName		= "Encap Blow Hole";
	m_pDefectCode[nIndex].nErrorCode		= 0;
	m_pDefectCode[nIndex].colorDefect		= RGB(255, 0, 0);
	nIndex++;
	m_pDefectCode[nIndex].m_nID				= ENCAP_CONTAMINATION;
	m_pDefectCode[nIndex].strDefectName		= "Encap Contamination";
	m_pDefectCode[nIndex].nErrorCode		= 0;
	m_pDefectCode[nIndex].colorDefect		= RGB(255, 0, 0);
	nIndex++;
	m_pDefectCode[nIndex].m_nID				= DIE_EDGE;
	m_pDefectCode[nIndex].strDefectName		= "Die Edge";
	m_pDefectCode[nIndex].nErrorCode		= 0;
	m_pDefectCode[nIndex].colorDefect		= RGB(255, 0, 0);
	nIndex++;
	m_pDefectCode[nIndex].m_nID = UNKNOWN_DEFECT;
	m_pDefectCode[nIndex].strDefectName = "Result Unknown";
	m_pDefectCode[nIndex].nErrorCode = 98765;
	m_pDefectCode[nIndex].colorDefect = RGB(0, 0, 255);
	nIndex++;
	m_pDefectCode[nIndex].m_nID = NOT_INSPECTED;
	m_pDefectCode[nIndex].strDefectName = "Not Inspected";
	m_pDefectCode[nIndex].nErrorCode = 2;
	m_pDefectCode[nIndex].colorDefect = RGB(92, 84, 84);*/
	
	nGroupId = nDefectGroupId;
	int nDefectCount = nDefectGroupId + 3;
	for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
		if(nTrack == STATION_TOP)
			m_pImageSelectionDlg->m_wndDlg.addGroup("PVI TOP", nDefectGroupId++);
		if(nTrack == STATION_BOTTOM)
			m_pImageSelectionDlg->m_wndDlg.addGroup("PVI BOTTOM", nDefectGroupId++);
		if (nTrack == STATION_SIDE)
			m_pImageSelectionDlg->m_wndDlg.addGroup("PVI SIDE", nDefectGroupId++);
	}


	nView = -1;
	
	m_mapPviDefectsStatsGeneral.clear();
	for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
		for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
			motionParameters.GetViewID(nTrack, nDoc, nView);
			for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
				m_strPviDefectsStats[nFov].RemoveAll();
				//	nView = !nTrack ? nDoc : (nTrack == 1 ? m_nTop + nDoc : m_nTop + m_nBottom + nDoc);
				int nAreaCount = m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.m_TrainingData.nNoOfPviArea;
				m_nPviNumberOfArea[nFov][nView] = nAreaCount;
				for (int nArea = 0; nArea < nAreaCount; nArea++) {
					int nAreaDefectCount = m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.m_pPviInspectionSetup[nArea].nDefectCount;
					m_nPviNoOfDefects[nFov][nView][nArea] = nAreaDefectCount;
					for (int nDefect = 0; nDefect < nAreaDefectCount; nDefect++) {
						//nIndex++;
						nDefectCount++;
						CString strDefectDisplay;
						CString strCam;
						strCam = !nTrack ? "Top" : nTrack == 2 ? "Side" : "Bottom";
						//strDefectDisplay.Format("Pos%d_%s%d_Area%d_Dno%d_%s",nFov+1, strCam,nDoc+1,nArea+1,nDefect+1, m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[nDefect].strDefectName);

						int nErrorCodeId = m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[nDefect].m_nErrorCodeID;
						strDefectDisplay = m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[nDefect].strDefectName;

						//int nErrorCodeId = m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[nDefect].m_nID;
						m_pImageSelectionDlg->m_wndDlg.addDefect(strDefectDisplay.GetString(), strDefectDisplay.GetString(), -nErrorCodeId, nGroupId + nTrack, RGB(19, 59, 69), false);
						
						//Update the Unique Defect Name
						m_mapPviDefectsStatsGeneral.insert(std::pair<int, CString>(-nErrorCodeId, strDefectDisplay));
						m_LotSummary.m_mapIndividualPviDefectCounter.insert(std::pair<int, long>(-nErrorCodeId, 0));
					}
				}
			}
		}
	}
	

	CString file;

	file.Format("%s\\Defect code.dat", m_strConfigDir, m_strConfigFile);
	m_pImageSelectionDlg->m_wndDlg.load(file.GetString());

	m_nTotalDefectsCount = nDefectCount +1;

	m_nDefectErrorCodesPriority.RemoveAll();
	int nIndex = 0;
	m_nDefectErrorCodesPriorityTemp.RemoveAll();
	m_nDefectErrorCodesPriorityTemp.Add(GOOD_UNIT);
	m_nDefectErrorCodesPriorityTemp.Add(-DEVICE_LOCATION);
	m_nDefectErrorCodesPriorityTemp.Add(-TILT_INSP);
	m_nDefectErrorCodesPriorityTemp.Add(-COVER_LAYER_INSP);
	m_nDefectErrorCodesPriorityTemp.Add(-COVER_LAYER_ALIGNMENT);
	m_nDefectErrorCodesPriorityTemp.Add(-ENCAP_INSUFFICIENT);
	m_nDefectErrorCodesPriorityTemp.Add(-ENCAP_EXCESS);
	m_nDefectErrorCodesPriorityTemp.Add(-ENCAP_CRACK);
	m_nDefectErrorCodesPriorityTemp.Add(-ENCAP_BLOWHOLE);
	m_nDefectErrorCodesPriorityTemp.Add(-ENCAP_CONTAMINATION);
	m_nDefectErrorCodesPriorityTemp.Add(-SLOT);
	m_nDefectErrorCodesPriorityTemp.Add(-DIE_EDGE);
	m_nDefectErrorCodesPriorityTemp.Add(-ENCAP_MAGNUS);
	m_nDefectErrorCodesPriorityTemp.Add(UNKNOWN_DEFECT);
	//m_nDefectErrorCodesPriorityTemp.Add(NOT_INSPECTED);
	//m_nDefectErrorCodesPriorityTemp.Add(ERROR_PROCESS);

	/*for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
		for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
			for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
				int nAreaCount = m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.m_TrainingData.nNoOfPviArea;
				for (int nArea = 0; nArea < nAreaCount; nArea++) {
					int nAreaDefectCount = m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.m_pPviInspectionSetup[nArea].nDefectCount;
					for (int nDefect = 0; nDefect < nAreaDefectCount; nDefect++) {
						m_nDefectErrorCodesPriorityTemp.Add(-m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[nDefect].m_nID);
					}
				}
			}
		}
	}*/

	m_strDefectErrorCodesPriorityTemp.RemoveAll();
	nIndex = 0;
	m_strDefectErrorCodesPriorityTemp.Add("Good Unit");
	m_strDefectErrorCodesPriorityTemp.Add("Device Location");
	m_strDefectErrorCodesPriorityTemp.Add("Tilt Inspection");
	m_strDefectErrorCodesPriorityTemp.Add("Cover Layer Inspection");
	m_strDefectErrorCodesPriorityTemp.Add("Cover Layer Alignment");
	m_strDefectErrorCodesPriorityTemp.Add("Encap Insufficient");
	m_strDefectErrorCodesPriorityTemp.Add("Encap Excess");
	m_strDefectErrorCodesPriorityTemp.Add("Encap Crack");
	m_strDefectErrorCodesPriorityTemp.Add("Encap BlowHole");
	m_strDefectErrorCodesPriorityTemp.Add("Encap Contamination");
	m_strDefectErrorCodesPriorityTemp.Add("Slot");
	m_strDefectErrorCodesPriorityTemp.Add("Die Edge");
	m_strDefectErrorCodesPriorityTemp.Add("Unknown Defect");
	//m_strDefectErrorCodesPriorityTemp.Add("Not Inspected");
	//m_strDefectErrorCodesPriorityTemp.Add("Process Error");
	nIndex = 6;

	/*for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
		for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
			for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
				int nAreaCount = m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.m_TrainingData.nNoOfPviArea;
				for (int nArea = 0; nArea < nAreaCount; nArea++) {
					int nAreaDefectCount = m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.m_pPviInspectionSetup[nArea].nDefectCount;
					for (int nDefect = 0; nDefect < nAreaDefectCount; nDefect++) {
						CString strDefectDisplay;
						CString strCam;
						strCam = !nTrack ? "Top" : nTrack == 2 ? "Side" : "Bottom";
						strDefectDisplay.Format("", nFov + 1, strCam, nDoc + 1, nArea+1,nDefect + 1, m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.m_pPviInspectionSetup[nArea].m_pPviDefect[nDefect].strDefectName);
						m_strDefectErrorCodesPriorityTemp.Add(strDefectDisplay);
						nIndex++;
					}
				}
			}
		}
	}*/

	//Update Defect Code Priority
	for (std::map<int, CString>::iterator it = m_mapPviDefectsStatsGeneral.begin(); it != m_mapPviDefectsStatsGeneral.end(); it++) {
		m_nDefectErrorCodesPriorityTemp.Add(it->first);
		int nTrackIdx, nDocIdx;
		CString strDefectDisplay = it->second;
		GetInformationFromErrorCode(it->first, it->second, nTrackIdx, nDocIdx);
		if (nTrackIdx != -1 && nDocIdx != -1) {
			CString strDoc;
			strDoc = nTrackIdx == 0 ? m_strTopCameraName[nDocIdx] : (nTrackIdx == 1 ? m_strBottomCameraName[nDocIdx] : m_strSideCameraName[nDocIdx]);
			strDefectDisplay.Format("%s_%s", strDoc, it->second);
		}
		m_strDefectErrorCodesPriorityTemp.Add(strDefectDisplay);
	}

	m_strDefectErrorCodesPriority.RemoveAll();
	CDefectPrioritySelDlg dlgPriorityList;
	dlgPriorityList.InitializePriorityList();


}

void CApplication::GetInformationFromErrorCode(int nErrorCode, CString strDefectName, int &nTrackIdx, int &nDocIdx)
{
	nTrackIdx = -1;
	nDocIdx = -1;
	for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
		for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
			CString strHashKey;
			strHashKey.Format("Track%d_Doc%d_%s", nTrack, nDoc, strDefectName);
			int nHasID = std::hash<std::string>()((LPCSTR)strHashKey);
			if (-abs(nHasID) == nErrorCode) {
				nTrackIdx = nTrack;
				nDocIdx = nDoc;
				break;
			}
		}
	}
}

void CApplication::CreateDefectCodeParmList(/*int nTrack, int nDoc, int nFOV*/)
{

	int nDevicePerX, nDevicePerY;
	int nFovPerX, nFovPerY;
	motionParameters.GetMappingParameters(nDevicePerX, nDevicePerY, nFovPerX, nFovPerY);

	int m_nNoOfFov = nFovPerX * nFovPerY;

	CParameters* pParm;
	int nGetParmCount = m_ParametersDefectCode.GetCount();
	for (int i = 0; i < nGetParmCount; i++) {
		pParm = m_ParametersDefectCode.GetAt(i);
		/*int nParmCount = pParm->arrParmInfo.GetCount();
		for (int j = 0; j<pParm->arrParmInfo.GetCount(); j++) {
			delete pParm->arrParmInfo[j];
		}
		pParm->Clean();*/
		delete pParm;
	}
	m_ParametersDefectCode.RemoveAll();
	int nIndex = 0;
	
	pParm = new CParameters();
	pParm->strInspection = m_pDefectCode[nIndex].strDefectName;
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].nErrorCode, "Error Code", TINT, "", 0, 98765, "", "Specifies the Error Code"));
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].colorDefect, "Color", TCOLOR, "", 0, 98765, "pixels", "Specifies the Color Code"));
	m_ParametersDefectCode.Add(pParm);
	nIndex++;
	pParm = new CParameters();
	pParm->strInspection = m_pDefectCode[nIndex].strDefectName;
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].nErrorCode, "Error Code", TINT, "", 1, 98765, "", "Specifies the Error Code"));
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].colorDefect, "Color", TCOLOR, "", 1, 98765, "pixels", "Specifies the Color Code"));
	m_ParametersDefectCode.Add(pParm);
	nIndex++;
	pParm = new CParameters();
	pParm->strInspection = m_pDefectCode[nIndex].strDefectName;
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].nErrorCode, "Error Code", TINT, "", 1, 98765, "", "Specifies the Error Code"));
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].colorDefect, "Color", TCOLOR, "", 1, 98765, "pixels", "Specifies the Color Code"));
	m_ParametersDefectCode.Add(pParm);
	nIndex++;
	pParm = new CParameters();
	pParm->strInspection = m_pDefectCode[nIndex].strDefectName;
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].nErrorCode, "Error Code", TINT, "", 1, 98765, "", "Specifies the Error Code"));
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].colorDefect, "Color", TCOLOR, "", 1, 98765, "pixels", "Specifies the Color Code"));
	m_ParametersDefectCode.Add(pParm);
	nIndex++;
	pParm = new CParameters();
	pParm->strInspection = m_pDefectCode[nIndex].strDefectName;
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].nErrorCode, "Error Code", TINT, "", 1, 98765, "", "Specifies the Error Code"));
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].colorDefect, "Color", TCOLOR, "", 1, 98765, "pixels", "Specifies the Color Code"));
	m_ParametersDefectCode.Add(pParm);
	nIndex++;
	pParm = new CParameters();
	pParm->strInspection = m_pDefectCode[nIndex].strDefectName;
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].nErrorCode, "Error Code", TINT, "", 1, 98765, "", "Specifies the Error Code"));
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].colorDefect, "Color", TCOLOR, "", 1, 98765, "pixels", "Specifies the Color Code"));
	m_ParametersDefectCode.Add(pParm);
	nIndex++;
	pParm = new CParameters();
	pParm->strInspection = m_pDefectCode[nIndex].strDefectName;
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].nErrorCode, "Error Code", TINT, "", 1, 98765, "", "Specifies the Error Code"));
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].colorDefect, "Color", TCOLOR, "", 1, 98765, "pixels", "Specifies the Color Code"));
	m_ParametersDefectCode.Add(pParm);
	nIndex++;
	/*pParm = new CParameters();
	pParm->strInspection = m_pDefectCode[7].strDefectName;
	pParm->Add(new CParmInfo(&m_pDefectCode[7].nErrorCode, "Error Code", TINT, "", 1, 98765, "", "Specifies the Error Code"));
	pParm->Add(new CParmInfo(&m_pDefectCode[7].colorDefect, "Color", TCOLOR, "", 1, 98765, "pixels", "Specifies the Color Code"));
	m_ParametersDefectCode.Add(pParm);*/

	pParm = new CParameters();
	pParm->strInspection = m_pDefectCode[nIndex].strDefectName;
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].nErrorCode, "Error Code", TINT, "", 1, 98765, "", "Specifies the Error Code"));
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].colorDefect, "Color", TCOLOR, "", 1, 98765, "pixels", "Specifies the Color Code"));
	m_ParametersDefectCode.Add(pParm);
	nIndex++;
	pParm = new CParameters();
	pParm->strInspection = m_pDefectCode[nIndex].strDefectName;
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].nErrorCode, "Error Code", TINT, "", 1, 98765, "", "Specifies the Error Code"));
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].colorDefect, "Color", TCOLOR, "", 1, 98765, "pixels", "Specifies the Color Code"));
	m_ParametersDefectCode.Add(pParm);
	nIndex++;
	pParm = new CParameters();
	pParm->strInspection = m_pDefectCode[nIndex].strDefectName;
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].nErrorCode, "Error Code", TINT, "", 1, 98765, "", "Specifies the Error Code"));
	pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].colorDefect, "Color", TCOLOR, "", 1, 98765, "pixels", "Specifies the Color Code"));
	m_ParametersDefectCode.Add(pParm);
	nIndex++;
	for (int nFov = 0; nFov < m_nNoOfFov; nFov++) {
		for (int nTrack = 0; nTrack < m_nTotalTracks; nTrack++) {
			for (int nDoc = 0; nDoc < m_nTotalViewDocs[nTrack]; nDoc++) {
				int nAreaCount = m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.m_TrainingData.nNoOfPviArea;
				for (int nArea = 0; nArea < nAreaCount; nArea++) {
					int nAreaDefectCount = m_pTracks[nTrack].m_pDoc[nDoc]->m_InspectionHandler[nFov].m_Inspection.m_pPviInspectionSetup[nArea].nDefectCount;
					for (int nDefect = 0; nDefect < nAreaDefectCount; nDefect++) {
						pParm = new CParameters();
						pParm->strInspection = m_pDefectCode[nIndex].strDefectName;
						pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].nErrorCode, "Error Code", TINT, "", 1, 98765, "", "Specifies the Error Code"));
						pParm->Add(new CParmInfo(&m_pDefectCode[nIndex].colorDefect, "Color", TCOLOR, "", 1, 98765, "pixels", "Specifies the Color Code"));
						m_ParametersDefectCode.Add(pParm);
						nIndex++;
					}
				}
			}
		}
	}
}


int CApplication::LoadDefectCodeParameters(CString strRecipeFolder, CString strRecipeFile) 
{
	CIniFile iniFile;
	CString strDataFileName;

	//strDataFileName = strDirectoryName + "\\" + strFileName + ".cll";
	strDataFileName.Format("%s\\%s.dec", strRecipeFolder, strRecipeFile);
	iniFile.SetPath(strDataFileName);

	if (iniFile.ReadFile()) {
		CString strSec, strVariable;
		strSec.Format("Defect Error Codes");

		int nCount = (int)m_ParametersDefectCode.GetSize();
		for (int i = 0; i<nCount; i++)
			(m_ParametersDefectCode.GetAt(i))->LoadParameters(&iniFile);
	}
	/*else {
	OutputDebugLog("%s not found. Default values assumed", strFileName);
	SaveInspectParameters(strDirectoryName, strFileName);
	}*/

	return 0;
}

int CApplication::SaveDefectCodeParameters(CString strRecipeFolder, CString strRecipeFile)
{
	CIniFile iniFile;
	CString strDataFileName;

	//strDataFileName = strDirectoryName + "\\" + strFileName + ".cll";
	strDataFileName.Format("%s\\%s.dec", strRecipeFolder, strRecipeFile);
	iniFile.SetPath(strDataFileName);

	CString strSec, strVariable;
	strSec.Format("Defect Error Codes");
	
	int nCount = (int)m_ParametersDefectCode.GetSize();
	for (int i = 0; i<nCount; i++)
		(m_ParametersDefectCode.GetAt(i))->SaveParameters(&iniFile);

	iniFile.WriteFile();

	return 0;
}

int CApplication::CheckForHalconnLicense()
{
	try {
		HTuple hSystem;
		GetSystem("temporary_mem_cache", &hSystem);
		if (hSystem == "true") {
			//SetSystem("temporary_mem_cache", "false");
		}
		else {
			SetSystem("temporary_mem_cache", "true");
		}

		GetSystem("reentrant", &hSystem);
		if (hSystem == "false") {
			SetSystem("reentrant", "true");
		}
		else {
			//SetSystem("reentrant", "false");
		}

		GetSystem("parallelize_operators", &hSystem);
		if (hSystem == "true") {
			SetSystem("parallelize_operators", "false");
		}
		else {
			//SetSystem("parallelize_operators", "true");
		}

		GetSystem("thread_pool", &hSystem);
		if (hSystem == "false") {
			SetSystem("thread_pool", "true ");
		}
		else {
			//SetSystem("thread_pool", "false ");
		}

		GetSystem("image_cache_capacity", &hSystem);
		if (hSystem != 4194304) {
			SetSystem("image_cache_capacity", 4194304);
		}
		else {
			//SetSystem("image_cache_capacity", 0);
		}

		GetSystem("global_mem_cache", &hSystem);
		if (hSystem != "exclusive") {
			SetSystem("global_mem_cache", "exclusive");
		}
		else {
			//SetSystem("global_mem_cache", "idle");
		}

		GetSystem("alloctmp_max_blocksize", &hSystem);
		if (hSystem != -1) {
			SetSystem("alloctmp_max_blocksize", -1);
		}
		else {
			//SetSystem("alloctmp_max_blocksize", 2);
		}
	}
	catch (HOperatorException ex) {
		CString str;
		str.Format("[Inspection::Initialize] %s", ex.ErrorMessage().Text());
		LogMessage(9, str);
		AfxMessageBox("Unregistered EmageEquipment_FCI4X Inspection module");
		return FALSE;
	}
	catch (...) {}

	return TRUE;
}

int CApplication::getMotionTimeOut() const
{
	return m_nMotionTimeout;
}


void CApplication::OnButtonDefectSelection()
{
	CDefectCodesDlg dlg;
	dlg.DoModal();
}


void CApplication::OnUpdateButtonDefectSelection(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bSuperUserMode ||
		!m_bOnline &&
		!m_bTeaching &&
		!m_bInspecting && 
		(m_UserAccInfo.m_nCurrentAccessLevel >= m_AdvSecurityDlg.m_data.m_nDefectParm));

}

void CApplication::OnButtonCentralizedPc()
{
	CentralizedCommunicationDlg dlg;
	dlg.DoModal();
}

void CApplication::OnUpdateButtonCentralizedPc(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_UserAccInfo.m_nCurrentAccessLevel >= 1) && m_bCentralizedVerificationMode);
	// TODO: Add your command update UI handler code here
}

void CApplication::OnButtonCentralizedMonitor()
{
	if (m_pCentralizedMonitorDlg == NULL) {
		m_pCentralizedMonitorDlg = new CCentralizedMonitorDlg();
		if (!m_pCentralizedMonitorDlg->Create(IDD_CENTRALIZED_MONITOR_DIALOG, m_pMainWnd)) {
			delete m_pCentralizedMonitorDlg;
			m_pCentralizedMonitorDlg = NULL;
			return;
		}
	}

	if (m_pCentralizedMonitorDlg != NULL) {
		m_pCentralizedMonitorDlg->ShowWindow(TRUE);
	}
}

void CApplication::OnUpdateButtonCentralizedMonitor(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_UserAccInfo.m_nCurrentAccessLevel >= 1) /*&& m_CentralizedCommInterface.m_bIsTCPServer*/ && m_bCentralizedVerificationMode);
	// TODO: Add your command update UI handler code here
}

void CApplication::OnUpdateDLMode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bOnline && m_UserAccInfo.m_nCurrentAccessLevel >= 1);
}

void CApplication::OnCbnSelchangeComboDLMode()
{
	if (m_pComboDLMode != NULL) {
		m_DeepLearningModule.m_nDeepLearningMode = m_pComboDLMode->GetCurSel();
		PutRegistryEntry(WRITE_REGPATH + "Deep Learning Engine", "Deep Learning Mode", m_DeepLearningModule.m_nDeepLearningMode);
	}
}

//void CApplication::OnButtonFileTransferSettings()
//{
//	if (m_pCentralizedFTPSettings == NULL) {
//		m_pCentralizedFTPSettings = new CCentralizedFTPSettings();
//		if (!m_pCentralizedFTPSettings->Create(IDD_CENTRALIZED_COMMUNICATION_DLG, m_pMainWnd)) {
//			delete m_pCentralizedFTPSettings;
//			m_pCentralizedFTPSettings = NULL;
//			return;
//		}
//	}
//
//	if (m_pCentralizedFTPSettings != NULL)
//		m_pCentralizedFTPSettings->ShowWindow(TRUE);
//
//}

//void CApplication::OnUpdateButtonFileTransferSettings(CCmdUI *pCmdUI)
//{
//	pCmdUI->Enable(TRUE);
//
//}


void CApplication::OnCheckEnableCentralizedVerification()
{
	m_bCentralizedVerificationMode = !m_bCentralizedVerificationMode;

	PutRegistryEntry(WRITE_REGPATH + "Centralized Comm", "Centralized Verification Mode", m_bCentralizedVerificationMode);	

}


void CApplication::OnUpdateCheckEnableCentralizedVerification(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_bCentralizedVerificationMode);
	
}
void CApplication::UpdateUPH(double dStripTime, int nDieNo)
{

	((CMainFrame*)m_pMainWnd)->m_dlgPaneStats->m_wndStatsDlg.UpdateUPH(dStripTime, nDieNo);
}
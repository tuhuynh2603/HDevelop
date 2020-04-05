// AdvSecurityDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "AdvSecurityDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSecurityDlg dialog


CAdvSecurityDlg::CAdvSecurityDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdvSecurityDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdvSecurityDlg)
	m_strTechPassword = _T("");
	m_strEngPassword = _T("");
	//}}AFX_DATA_INIT

	m_brush.CreateSolidBrush(RGB(30, 30, 30));
}

CAdvSecurityDlg::~CAdvSecurityDlg()
{
	m_brush.DeleteObject();
}

void CAdvSecurityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSecurityDlg)
	DDX_Control(pDX, IDC_OPTIONS_LIST, m_propertyControl);
	DDX_Text(pDX, IDC_TECH_PASSWORD, m_strTechPassword);
	DDX_Text(pDX, IDC_ENG_PASSWORD, m_strEngPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSecurityDlg, CDialog)
	//{{AFX_MSG_MAP(CAdvSecurityDlg)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvSecurityDlg message handlers

void CAdvSecurityDlg::OnOK() 
{
	// TODO: Add extra validation here
	m_propertyManager.GetData(m_data);
	UpdateData(TRUE);
	SetSecurityAccess();

	CDialog::OnOK();
}

void CAdvSecurityDlg::OnDefault() 
{
	// TODO: Add your control notification handler code here
	// 3 = (Disabled)
	// 2 = (Engineering)
	// 1 = (Setup)
	// 0 = (Operator)

	m_data.ResetDefaults();
	CApplication* pApp = (CApplication*)AfxGetApp();
	pApp->AddLogEvent("Access Rights have been reset");
	m_propertyManager.SetData(m_data);
	m_propertyControl.RedrawWindow();
}

BOOL CAdvSecurityDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if( GetSecurityAccess() ) {
		m_propertyManager.SetData(m_data);
		m_propertyControl.SetPropertyItemManger(&m_propertyManager);
		UpdateData(false);
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}
	else
		return FALSE;	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CAdvSecurityDlg::GetSecurityAccess()
{
	HKEY hKey;
	DWORD dwDisposition;
	DWORD dwType;
	DWORD dwLength;
	DWORD dwReturn;
	DWORD dwValue;
	//	char buf[256];



	dwType=REG_DWORD;
	dwLength=4;

	//Get Information from Registry
	if (RegCreateKeyEx(HKEY_CURRENT_USER,
		WRITE_REGPATH + "Preferences\\Persistence",
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&dwDisposition)!=ERROR_SUCCESS) {
			::AfxMessageBox("ERROR");
			return FALSE;
	}


	dwType=REG_SZ;
	dwLength=256;

	dwType=REG_DWORD;
	dwLength=4;

	//online 
	dwReturn = RegQueryValueEx(hKey, "rOpenLot", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 0(Operator)
		dwValue = 0;
	}
	m_data.m_nOpenLot = (int)dwValue;
	m_nOlddwvalue[0] = m_data.m_nOpenLot;

	// Get Access Level for Close Lot
	dwReturn = RegQueryValueEx(hKey, "rCloseLot", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 0(Operator)
		dwValue = 0;
	}
	m_data.m_nCloseLot = (int)dwValue;
	m_nOlddwvalue[1] = m_data.m_nCloseLot;

	dwReturn=RegQueryValueEx(hKey,"rOnlineMode",NULL,&dwType,(unsigned char *) &dwValue,&dwLength);
	if (dwReturn!=ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue=0;  
	}
	m_data.m_nProdMode = (int)dwValue;
	m_nOlddwvalue[2] = m_data.m_nProdMode;

	dwReturn=RegQueryValueEx(hKey,"rSavePassImages",NULL,&dwType,(unsigned char *) &dwValue,&dwLength);
	if (dwReturn!=ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue=1;  
	}
	m_data.m_nSaveOnlinePassImages = (int)dwValue;
	m_nOlddwvalue[3] = m_data.m_nSaveOnlinePassImages;

	dwReturn=RegQueryValueEx(hKey,"rSaveFailImages",NULL,&dwType,(unsigned char *) &dwValue,&dwLength);
	if (dwReturn!=ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue=1;  
	}
	m_data.m_nSaveOnlineFailImages = (int)dwValue;
	m_nOlddwvalue[4] = m_data.m_nSaveOnlineFailImages;

	dwReturn = RegQueryValueEx(hKey, "rByPassResult", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 2;
	}
	m_data.m_nByPassResult = (int)dwValue;
	m_nOlddwvalue[5] = m_data.m_nByPassResult;


	dwReturn = RegQueryValueEx(hKey, "rSaveJpegImages", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 1;
	}
	m_data.m_nSaveOnlineJpegImages = (int)dwValue;
	m_nOlddwvalue[58] = m_data.m_nSaveOnlineJpegImages;

	dwReturn = RegQueryValueEx(hKey, "rSaveBMPImages", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 1;
	}
	m_data.m_nSaveOnlineBMPImages = (int)dwValue;
	m_nOlddwvalue[59] = m_data.m_nSaveOnlineBMPImages;
	//inspection
	dwReturn = RegQueryValueEx(hKey, "rTrain", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 1;
	}
	m_data.m_nTeach = (int)dwValue;
	m_nOlddwvalue[6] = m_data.m_nTeach;

	dwReturn = RegQueryValueEx(hKey, "rStepSelection", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 0(Operator)
		dwValue = 1;
	}
	m_data.m_nStepSelection = (int)dwValue;
	m_nOlddwvalue[7] = m_data.m_nStepSelection;

	dwReturn = RegQueryValueEx(hKey, "rStepDebug", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 1;
	}
	m_data.m_nStepDebug = (int)dwValue;
	m_nOlddwvalue[8] = m_data.m_nStepDebug;

	dwReturn = RegQueryValueEx(hKey, "rInspect", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 0;
	}
	m_data.m_nInspect = (int)dwValue;
	m_nOlddwvalue[9] = m_data.m_nInspect;

	dwReturn = RegQueryValueEx(hKey, "rStoredImagesInsp", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Eng)
		dwValue = 1;
	}
	m_data.m_nInspectSavedImage = (int)dwValue;
	m_nOlddwvalue[10] = m_data.m_nInspectSavedImage;

	dwReturn = RegQueryValueEx(hKey, "rSetFolder", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Eng)
		dwValue = 0;
	}
	m_data.m_nSetFolder = (int)dwValue;
	m_nOlddwvalue[11] = m_data.m_nSetFolder;

	dwReturn = RegQueryValueEx(hKey, "rSavePassImages", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 1;
	}
	m_data.m_nSaveOfflinePassImages = (int)dwValue;
	m_nOlddwvalue[12] = m_data.m_nSaveOfflinePassImages;

	dwReturn = RegQueryValueEx(hKey, "rSaveFailImages", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 1;
	}
	m_data.m_nSaveOfflineFailImages = (int)dwValue;
	m_nOlddwvalue[13] = m_data.m_nSaveOfflineFailImages;

	dwReturn = RegQueryValueEx(hKey, "rInspectLoop", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 1;
	}
	m_data.m_nInspectLoop = (int)dwValue;
	m_nOlddwvalue[14] = m_data.m_nInspectLoop;

	dwReturn = RegQueryValueEx(hKey, "rInspectCycle", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 2;
	}
	m_data.m_nInspectCycle = (int)dwValue;
	m_nOlddwvalue[15] = m_data.m_nInspectCycle;

	dwReturn = RegQueryValueEx(hKey, "rCameraActive", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 2;
	}
	m_data.m_nCamEnable = (int)dwValue;
	m_nOlddwvalue[16] = m_data.m_nCamEnable;

	dwReturn = RegQueryValueEx(hKey, "rInspectSeq", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 1;
	}
	m_data.m_nInspectionSeq = (int)dwValue;
	m_nOlddwvalue[17] = m_data.m_nInspectionSeq;

	dwReturn = RegQueryValueEx(hKey, "rCalibrationSettings", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 2;
	}
	m_data.m_nCalibSettings = (int)dwValue;
	m_nOlddwvalue[18] = m_data.m_nCalibSettings;

	dwReturn = RegQueryValueEx(hKey, "rInspSeqLoop", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 1;
	}
	m_data.m_nInspSeqLoop = (int)dwValue;
	m_nOlddwvalue[19] = m_data.m_nInspSeqLoop;

	dwReturn = RegQueryValueEx(hKey, "rSaveInspSeqImages", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 1;
	}
	m_data.m_nSaveInspSeqImages = (int)dwValue;
	m_nOlddwvalue[20] = m_data.m_nSaveInspSeqImages;
	//End Inspection

	//Camera and Lighting
	dwReturn = RegQueryValueEx(hKey, "rSnap", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 0;
	}
	m_data.m_nGrab = (int)dwValue;
	m_nOlddwvalue[21] = m_data.m_nGrab;

	dwReturn = RegQueryValueEx(hKey, "rStream", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 0;
	}
	m_data.m_nLive = (int)dwValue;
	m_nOlddwvalue[22] = m_data.m_nLive;

	dwReturn = RegQueryValueEx(hKey, "rSnapAll", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 0;
	}
	m_data.m_nGrabAll = (int)dwValue;
	m_nOlddwvalue[23] = m_data.m_nGrabAll;

	dwReturn = RegQueryValueEx(hKey, "rLightSetting", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 2;
	}
	m_data.m_nLightSet = (int)dwValue;
	m_nOlddwvalue[24] = m_data.m_nLightSet;
	//end camera and lighting

	//Parameters
	dwReturn = RegQueryValueEx(hKey, "rVisionParm", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Eng)
		dwValue = 1;
	}
	m_data.m_nVisionParm = (int)dwValue;
	m_nOlddwvalue[25] = m_data.m_nVisionParm;

	dwReturn = RegQueryValueEx(hKey, "rTeachParm", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Eng)
		dwValue = 1;
	}
	m_data.m_nTeachParm = (int)dwValue;
	m_nOlddwvalue[26] = m_data.m_nTeachParm;

	dwReturn = RegQueryValueEx(hKey, "rDefectParm", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Eng)
		dwValue = 1;
	}
	m_data.m_nDefectParm = (int)dwValue;
	m_nOlddwvalue[27] = m_data.m_nDefectParm;

	dwReturn = RegQueryValueEx(hKey, "rLoadRecipe", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Eng)
		dwValue = 2;
	}
	m_data.m_nManageCfg = (int)dwValue;
	m_nOlddwvalue[28] = m_data.m_nManageCfg;

	dwReturn = RegQueryValueEx(hKey, "rCreateDeleteRecipe", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Eng)
		dwValue = 2;
	}
	m_data.m_nCreateConfig = (int)dwValue;
	m_nOlddwvalue[29] = m_data.m_nCreateConfig;

	dwReturn = RegQueryValueEx(hKey, "rPviDefectParm", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Eng)
		dwValue = 1;
	}
	m_data.m_nPviDefectParm = (int)dwValue;
	m_nOlddwvalue[30] = m_data.m_nPviDefectParm;

	

	dwReturn = RegQueryValueEx(hKey, "rDeviceConfig", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Eng)
		dwValue = 2;
	}
	m_data.m_nDeviceCfg = (int)dwValue;
	m_nOlddwvalue[31] = m_data.m_nDeviceCfg;

	dwReturn = RegQueryValueEx(hKey, "rTeachInspLoc", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Eng)
		dwValue = 2;
	}
	m_data.m_nTeachInspLoc = (int)dwValue;
	m_nOlddwvalue[32] = m_data.m_nTeachInspLoc;

	dwReturn = RegQueryValueEx(hKey, "rDefectPriority", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Eng)
		dwValue = 2;
	}
	m_data.m_nDefectPriority = (int)dwValue;
	m_nOlddwvalue[33] = m_data.m_nDefectPriority;
	//end parameters

	//Tools
	dwReturn = RegQueryValueEx(hKey, "rLoadImage", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 0;
	}
	m_data.m_nLoadImg = (int)dwValue;
	m_nOlddwvalue[34] = m_data.m_nLoadImg;

	dwReturn = RegQueryValueEx(hKey, "rLoadImageAll", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 0;
	}
	m_data.m_nLoadImgAll = (int)dwValue;
	m_nOlddwvalue[35] = m_data.m_nLoadImgAll;

	dwReturn = RegQueryValueEx(hKey, "rSaveImage", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 0;
	}
	m_data.m_nSaveImg = (int)dwValue;
	m_nOlddwvalue[36] = m_data.m_nSaveImg;

	dwReturn = RegQueryValueEx(hKey, "rSaveImageAll", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 0;
	}
	m_data.m_nSaveImgAll = (int)dwValue;
	m_nOlddwvalue[37] = m_data.m_nSaveImgAll;

	dwReturn = RegQueryValueEx(hKey, "rLoadTeachImage", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 1;
	}
	m_data.m_nLoadTeachImg = (int)dwValue;
	m_nOlddwvalue[38] = m_data.m_nLoadTeachImg;
	
	dwReturn = RegQueryValueEx(hKey, "rLoadTeachImageAll", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 1;
	}
	m_data.m_nLoadTeachImgAll = (int)dwValue;
	m_nOlddwvalue[39] = m_data.m_nLoadTeachImgAll;

	dwReturn=RegQueryValueEx(hKey,"rStatResetMode",NULL,&dwType,(unsigned char *) &dwValue,&dwLength);
	if (dwReturn!=ERROR_SUCCESS) {
		dwValue=2;  
	}
	m_data.m_nResetStatCounters = (int)dwValue;
	m_nOlddwvalue[40] = m_data.m_nResetStatCounters;

	dwReturn=RegQueryValueEx(hKey,"rShowDieCfg",NULL,&dwType,(unsigned char *) &dwValue,&dwLength);
	if (dwReturn!=ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue=0;  
	}
	m_data.m_nShowDieCfg = (int)dwValue;
	m_nOlddwvalue[41] = m_data.m_nShowDieCfg;


	dwReturn=RegQueryValueEx(hKey,"rShowStats",NULL,&dwType,(unsigned char *) &dwValue,&dwLength);
	if (dwReturn!=ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue=0;  
	}
	m_data.m_nShowStats = (int)dwValue;
	m_nOlddwvalue[42] = m_data.m_nShowStats;

	dwReturn=RegQueryValueEx(hKey,"rDefaultSettings",NULL,&dwType,(unsigned char *) &dwValue,&dwLength);
	if (dwReturn!=ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue=1;  
	}
	m_data.m_nDefaultSettings = (int)dwValue;
	m_nOlddwvalue[43] = m_data.m_nDefaultSettings;

	dwReturn = RegQueryValueEx(hKey, "rPixelRuler", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 1;
	}
	m_data.m_nPixelRuler = (int)dwValue;
	m_nOlddwvalue[44] = m_data.m_nPixelRuler;


	dwReturn=RegQueryValueEx(hKey,"rMotionCtrl",NULL,&dwType,(unsigned char *) &dwValue,&dwLength);
	if (dwReturn!=ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue=1;
	}
	m_data.m_nMotionControl = (int)dwValue;
	m_nOlddwvalue[45] = m_data.m_nMotionControl;

	dwReturn = RegQueryValueEx(hKey, "rFocusingTool", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 1;
	}
	m_data.m_nOpticalPanel = (int)dwValue;
	m_nOlddwvalue[46] = m_data.m_nOpticalPanel;


	dwReturn = RegQueryValueEx(hKey, "rCameraSetup", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 2;
	}
	m_data.m_nCameraCfg = (int)dwValue;
	m_nOlddwvalue[47] = m_data.m_nCameraCfg;

	dwReturn = RegQueryValueEx(hKey, "rIOTest", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Eng)
		dwValue = 2;
	}
	m_data.m_nIOTest = (int)dwValue;
	m_nOlddwvalue[48] = m_data.m_nIOTest;

	dwReturn = RegQueryValueEx(hKey, "rDryRun", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 1;
	}
	m_data.m_nDryRun = (int)dwValue;
	m_nOlddwvalue[49] = m_data.m_nDryRun;

	dwReturn = RegQueryValueEx(hKey, "rOutPutLog", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 0;
	}
	m_data.m_nOutPutLog = (int)dwValue;
	m_nOlddwvalue[50] = m_data.m_nOutPutLog;

	dwReturn = RegQueryValueEx(hKey, "rBinarize", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 0;
	}
	m_data.m_nBinarize = (int)dwValue;
	m_nOlddwvalue[51] = m_data.m_nBinarize;

	dwReturn = RegQueryValueEx(hKey, "rZoomIn", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 0;
	}
	m_data.m_nZoomIn = (int)dwValue;
	m_nOlddwvalue[52] = m_data.m_nZoomIn;

	dwReturn = RegQueryValueEx(hKey, "rZoomOut", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 0;
	}
	m_data.m_nZoomOut = (int)dwValue;
	m_nOlddwvalue[53] = m_data.m_nZoomOut;

	dwReturn = RegQueryValueEx(hKey, "rZoomFit", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 0;
	}
	m_data.m_nZoomFit = (int)dwValue;
	m_nOlddwvalue[54] = m_data.m_nZoomFit;

	dwReturn = RegQueryValueEx(hKey, "rZoom100", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 0;
	}
	m_data.m_nZoom100 = (int)dwValue;
	m_nOlddwvalue[55] = m_data.m_nZoom100;

	dwReturn = RegQueryValueEx(hKey, "rZoomAll", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 0;
	}
	m_data.m_nZoomAll = (int)dwValue;
	m_nOlddwvalue[56] = m_data.m_nZoomAll;

	dwReturn = RegQueryValueEx(hKey, "rSetupGrid", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 0;
	}
	m_data.m_nSetupGrid = (int)dwValue;
	m_nOlddwvalue[57] = m_data.m_nSetupGrid;

	dwReturn = RegQueryValueEx(hKey, "rShowOverLay", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 0;
	}
	m_data.m_nShowOverLay = (int)dwValue;
	m_nOlddwvalue[58] = m_data.m_nShowOverLay;

	dwReturn = RegQueryValueEx(hKey, "rAuditTrail", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 0;
	}
	m_data.m_nAuditTrail = (int)dwValue;
	m_nOlddwvalue[59] = m_data.m_nAuditTrail;

	dwReturn = RegQueryValueEx(hKey, "rShowMagnifier", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 2(Engineering)
		dwValue = 0;
	}
	m_data.m_nShowMaginifier = (int)dwValue;
	m_nOlddwvalue[60] = m_data.m_nShowMaginifier;

	//Deep Learning Image Saving
	dwReturn = RegQueryValueEx(hKey, "rSaveDeepLearningImages", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 1;
	}
	m_data.m_nSaveOnlineDLImages = (int)dwValue;
	m_nOlddwvalue[61] = m_data.m_nSaveOnlineDLImages;

	dwReturn = RegQueryValueEx(hKey, "rSaveDeepLearningPassImages", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 1;
	}
	m_data.m_nSaveOnlineDLPassImages = (int)dwValue;
	m_nOlddwvalue[62] = m_data.m_nSaveOnlineDLPassImages;

	dwReturn = RegQueryValueEx(hKey, "rSaveDeepLearningFailImages", NULL, &dwType, (unsigned char *)&dwValue, &dwLength);
	if (dwReturn != ERROR_SUCCESS) {
		// Default = 1(tech)
		dwValue = 1;
	}
	m_data.m_nSaveOnlineDLFailImages = (int)dwValue;
	m_nOlddwvalue[63] = m_data.m_nSaveOnlineDLFailImages;

	RegCloseKey(hKey);
	return TRUE;
}

bool CAdvSecurityDlg::SetSecurityAccess()
{
	HKEY hKey;
	DWORD dwDisposition;
	DWORD dwReturn;
	DWORD dwValue;

	//Set Information from Registry
	if (RegCreateKeyEx(HKEY_CURRENT_USER,
		WRITE_REGPATH + "Preferences\\Persistence",
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&dwDisposition)!=ERROR_SUCCESS) {
			::AfxMessageBox("ERROR");
			return FALSE;
	}
	/*
	//	buf = (char*)m_strEngPassword;
	if ((dwReturn=RegSetValueEx(hKey,
	"Engineering",
	NULL,
	REG_SZ,
	(unsigned char *)m_strEngPassword.GetBuffer(0),
	m_strEngPassword.GetLength()+1))!=ERROR_SUCCESS) {
	m_strEngPassword="Eng";
	}

	//	buf = m_strTechPassword;
	if ((dwReturn=RegSetValueEx(hKey,
	"Technician",
	NULL,
	REG_SZ,
	(unsigned char *)m_strTechPassword.GetBuffer(0),
	m_strTechPassword.GetLength()+1))!=ERROR_SUCCESS) {
	m_strTechPassword="tech";
	}*/

	CApplication* pApp = (CApplication*) AfxGetApp();
	CString strMsg;
	//online 
	dwValue = (DWORD)m_data.m_nOpenLot;
	dwReturn = RegSetValueEx(hKey, "rOpenLot", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[0] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "OpenLot", m_nOlddwvalue[0]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	// Get Access Level for Close Lot
	dwValue = (DWORD)m_data.m_nCloseLot;
	dwReturn = RegSetValueEx(hKey, "rCloseLot", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[1] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "CloseLot", m_nOlddwvalue[1]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nProdMode;
	dwReturn = RegSetValueEx(hKey, "rOnlineMode", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[2] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "OnlineMode", m_nOlddwvalue[2]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nSaveOnlinePassImages;
	dwReturn = RegSetValueEx(hKey, "rSavePassImages", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[3] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "SavePassImages", m_nOlddwvalue[3]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nSaveOnlineFailImages;
	dwReturn = RegSetValueEx(hKey, "rSaveFailImages", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[4] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "SaveFailImages", m_nOlddwvalue[4]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nByPassResult;
	dwReturn = RegSetValueEx(hKey, "rByPassResult", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[5] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "ByPassResult", m_nOlddwvalue[5]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nSaveOnlineJpegImages;
	dwReturn = RegSetValueEx(hKey, "rSaveJpegImages", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[58] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "SaveJpegImages", m_nOlddwvalue[58]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nSaveOnlineBMPImages;
	dwReturn = RegSetValueEx(hKey, "rSaveBMPImages", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[59] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "SaveBMPImages", m_nOlddwvalue[59]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	//inspection
	dwValue = (DWORD)m_data.m_nTeach;
	dwReturn = RegSetValueEx(hKey, "rTrain", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[6] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "Train", m_nOlddwvalue[6]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nStepSelection;
	dwReturn = RegSetValueEx(hKey, "rStepSelection", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[7] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "StepSelection", m_nOlddwvalue[7]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nStepDebug;
	dwReturn = RegSetValueEx(hKey, "rStepDebug", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[8] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "StepDebug", m_nOlddwvalue[8]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nInspect;
	dwReturn = RegSetValueEx(hKey, "rInspect", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[9] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "Inspect", m_nOlddwvalue[9]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nInspectSavedImage;
	dwReturn = RegSetValueEx(hKey, "rStoredImagesInsp", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[10] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "StoredImagesInsp", m_nOlddwvalue[10]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nSetFolder;
	dwReturn = RegSetValueEx(hKey, "rSetFolder", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[11] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "SetFolder", m_nOlddwvalue[11]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nSaveOfflinePassImages;
	dwReturn = RegSetValueEx(hKey, "rSavePassImages", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[12] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "SavePassImages", m_nOlddwvalue[12]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nSaveOfflineFailImages;
	dwReturn = RegSetValueEx(hKey, "rSaveFailImages", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[13] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "SaveFailImages", m_nOlddwvalue[13]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nInspectLoop;
	dwReturn = RegSetValueEx(hKey, "rInspectLoop", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[14] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "InspectLoop", m_nOlddwvalue[14]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nInspectCycle;
	dwReturn = RegSetValueEx(hKey, "rInspectCycle", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[15] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "InspectCycle", m_nOlddwvalue[15]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nCamEnable;
	dwReturn = RegSetValueEx(hKey, "rCameraActive", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[16] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "CameraActive", m_nOlddwvalue[16]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nInspectionSeq;
	dwReturn = RegSetValueEx(hKey, "rInspectSeq", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[17] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "InspectSeq", m_nOlddwvalue[17]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nCalibSettings;
	dwReturn = RegSetValueEx(hKey, "rCalibrationSettings", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[18] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "CalibrationSettings", m_nOlddwvalue[18]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nInspSeqLoop;
	dwReturn = RegSetValueEx(hKey, "rInspSeqLoop", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[19] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "InspSeqLoop", m_nOlddwvalue[19]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nSaveInspSeqImages;
	dwReturn = RegSetValueEx(hKey, "rSaveInspSeqImages", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[20] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "SaveInspSeqImages", m_nOlddwvalue[20]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}
	//End Inspection

	//Camera and Lighting
	dwValue = (DWORD)m_data.m_nGrab;
	dwReturn = RegSetValueEx(hKey, "rSnap", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[21] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "Snap", m_nOlddwvalue[21]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nLive;
	dwReturn = RegSetValueEx(hKey, "rStream", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[22] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "Stream", m_nOlddwvalue[22]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nGrabAll;
	dwReturn = RegSetValueEx(hKey, "rSnapAll", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[23] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "SnapAll", m_nOlddwvalue[23]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nLightSet;
	dwReturn = RegSetValueEx(hKey, "rLightSetting", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[24] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "LightSetting", m_nOlddwvalue[24]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}
	//end camera and lighting

	//Parameters
	dwValue = (DWORD)m_data.m_nVisionParm;
	dwReturn = RegSetValueEx(hKey, "rVisionParm", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[25] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "VisionParm", m_nOlddwvalue[25]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nTeachParm;
	dwReturn = RegSetValueEx(hKey, "rTeachParm", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[26] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "TeachParm", m_nOlddwvalue[26]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nDefectParm;
	dwReturn = RegSetValueEx(hKey, "rDefectParm", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[27] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "DefectParm", m_nOlddwvalue[27]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nManageCfg;
	dwReturn = RegSetValueEx(hKey, "rLoadRecipe", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[28] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "LoadRecipe", m_nOlddwvalue[28]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nCreateConfig;
	dwReturn = RegSetValueEx(hKey, "rCreateDeleteRecipe", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[29] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "CreateDeleteRecipe", m_nOlddwvalue[29]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nPviDefectParm;
	dwReturn = RegSetValueEx(hKey, "rPviDefectParm", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[30] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "PviDefectParm", m_nOlddwvalue[30]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nDeviceCfg;
	dwReturn = RegSetValueEx(hKey, "rDeviceConfig", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[31] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "DeviceConfig", m_nOlddwvalue[33]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nTeachInspLoc;
	dwReturn = RegSetValueEx(hKey, "rTeachInspLoc", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[32] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "TeachInspLoc", m_nOlddwvalue[34]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nDefectPriority;
	dwReturn = RegSetValueEx(hKey, "rDefectPriority", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[33] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "DefectPriority", m_nOlddwvalue[33]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}
	//end parameters

	//Tools
	dwValue = (DWORD)m_data.m_nLoadImg;
	dwReturn = RegSetValueEx(hKey, "rLoadImage", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[34] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "LoadImage", m_nOlddwvalue[34]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nLoadImgAll;
	dwReturn = RegSetValueEx(hKey, "rLoadImageAll", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[35] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "LoadImageAll", m_nOlddwvalue[35]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nSaveImg;
	dwReturn = RegSetValueEx(hKey, "rSaveImage", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[36] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "SaveImage", m_nOlddwvalue[36]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nSaveImgAll;
	dwReturn = RegSetValueEx(hKey, "rSaveImageAll", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[37] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "SaveImageAll", m_nOlddwvalue[37]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nLoadTeachImg;
	dwReturn = RegSetValueEx(hKey, "rLoadTeachImage", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[38] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "LoadTeachImage", m_nOlddwvalue[38]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nLoadTeachImgAll;
	dwReturn = RegSetValueEx(hKey, "rLoadTeachImageAll", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[39] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "LoadTeachImageAll", m_nOlddwvalue[39]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nResetStatCounters;
	dwReturn = RegSetValueEx(hKey, "rStatResetMode", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[40] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "StatResetMode", m_nOlddwvalue[40]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nShowDieCfg;
	dwReturn = RegSetValueEx(hKey, "rShowDieCfg", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[41] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "ShowDieCfg", m_nOlddwvalue[41]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nShowStats;
	dwReturn = RegSetValueEx(hKey, "rShowStats", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[42] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "ShowStats", m_nOlddwvalue[42]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nDefaultSettings;
	dwReturn = RegSetValueEx(hKey, "rDefaultSettings", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[43] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "DefaultSettings", m_nOlddwvalue[43]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nPixelRuler;
	dwReturn = RegSetValueEx(hKey, "rPixelRuler", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[44] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "PixelRuler", m_nOlddwvalue[44]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nMotionControl;
	dwReturn = RegSetValueEx(hKey, "rMotionCtrl", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[45] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "MotionCtrl", m_nOlddwvalue[45]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nOpticalPanel;
	dwReturn = RegSetValueEx(hKey, "rFocusingTool", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[46] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "FocusingTool", m_nOlddwvalue[46]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}


	dwValue = (DWORD)m_data.m_nCameraCfg;
	dwReturn = RegSetValueEx(hKey, "rCameraSetup", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[47] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "CameraSetup", m_nOlddwvalue[47]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nIOTest;
	dwReturn = RegSetValueEx(hKey, "rIOTest", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[48] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "IOTest", m_nOlddwvalue[48]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nDryRun;
	dwReturn = RegSetValueEx(hKey, "rDryRun", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[49] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "DryRun", m_nOlddwvalue[49]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nOutPutLog;
	dwReturn = RegSetValueEx(hKey, "rOutPutLog", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[50] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "OutPutLog", m_nOlddwvalue[50]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nBinarize;
	dwReturn = RegSetValueEx(hKey, "rBinarize", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[51] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "Binarize", m_nOlddwvalue[51]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nZoomIn;
	dwReturn = RegSetValueEx(hKey, "rZoomIn", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[52] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "ZoomIn", m_nOlddwvalue[52]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}
	
	dwValue = (DWORD)m_data.m_nZoomOut;
	dwReturn = RegSetValueEx(hKey, "rZoomOut", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[53] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "ZoomOut", m_nOlddwvalue[53]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nZoomFit;
	dwReturn = RegSetValueEx(hKey, "rZoomFit", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[54] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "ZoomFit", m_nOlddwvalue[54]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}
	
	dwValue = (DWORD)m_data.m_nZoom100;
	dwReturn = RegSetValueEx(hKey, "rZoom100", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[55] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "Zoom100", m_nOlddwvalue[55]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nZoomAll;
	dwReturn = RegSetValueEx(hKey, "rZoomAll", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[56] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "ZoomAll", m_nOlddwvalue[56]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nSetupGrid;
	dwReturn = RegSetValueEx(hKey, "rSetupGrid", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[57] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "SetupGrid", m_nOlddwvalue[57]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}
	
	dwValue = (DWORD)m_data.m_nShowOverLay;
	dwReturn = RegSetValueEx(hKey, "rShowOverLay", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[58] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "ShowOverLay", m_nOlddwvalue[58]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nAuditTrail;
	dwReturn = RegSetValueEx(hKey, "rAuditTrail", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[59] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "AuditTrail", m_nOlddwvalue[59]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}
	
	dwValue = (DWORD)m_data.m_nShowMaginifier;
	dwReturn = RegSetValueEx(hKey, "rShowMagnifier", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[60] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "ShowMagnifier", m_nOlddwvalue[60]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	//Deep Learning Image Saving
	dwValue = (DWORD)m_data.m_nSaveOnlineDLImages;
	dwReturn = RegSetValueEx(hKey, "rSaveDeepLearningImages", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[61] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "rSaveDeepLearningImages", m_nOlddwvalue[61]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nSaveOnlineDLPassImages;
	dwReturn = RegSetValueEx(hKey, "rSaveDeepLearningPassImages", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[62] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "rSaveDeepLearningPassImages", m_nOlddwvalue[62]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	dwValue = (DWORD)m_data.m_nSaveOnlineDLFailImages;
	dwReturn = RegSetValueEx(hKey, "m_nSaveOnlineDLFailImages", NULL, REG_DWORD, (unsigned char *)&dwValue, 4);
	if (m_nOlddwvalue[63] != dwValue) {
		strMsg = GetAccsLevelStatus(dwValue, "m_nSaveOnlineDLFailImages", m_nOlddwvalue[63]);
		pApp->AddLogEvent(strMsg);
	}
	if (dwReturn != ERROR_SUCCESS) {
		::AfxMessageBox("Unable To Update Value");
	}

	RegCloseKey(hKey);
	return TRUE;

}


HBRUSH CAdvSecurityDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if( pWnd->GetDlgCtrlID() == IDC_OPTIONS_LIST )
	{
		// Set color as red.
		pDC->SetTextColor( RGB( 235,235,255 ));
		//pDC->SetBkMode(RGB( 200,200,255) );
		pDC->SetBkColor(RGB( 30,30,30));
		///pDC->SetBkMode(RGB( 100,100,100) );
	}
	return m_brush;
}

CString CAdvSecurityDlg::GetAccsLevelStatus(DWORD dwValue,CString strParam,int nOldValue)
{
	CString strMsg,sOldName;

	if(nOldValue == 0)
		sOldName = "Operator";
	else if(nOldValue == 1)
		sOldName = "Technician";
	else
		sOldName = "Administrator";

	if(dwValue == 0)
		strMsg.Format("Access Rights for %s Mode is Set from  %s to Operator",strParam,sOldName);
	else if(dwValue == 1)
		strMsg.Format("Access Rights for %s Mode is Set from %s to Technician",strParam,sOldName);
	else 
		strMsg.Format("Access Rights for %s Mode is Set from %s to Administrator",strParam,sOldName);

	return strMsg;
}
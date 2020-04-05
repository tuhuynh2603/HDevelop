// CalibrationSettings.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include "CalibrationSettings.h"
#include "afxdialogex.h"
#include "LightControlDlg.h"
#include "MotionControlDialog.h"
#include "SaveCalibRationPosition.h"
#include <direct.h>
// CCalibrationSettings dialog

IMPLEMENT_DYNAMIC(CCalibrationSettings, CDialog)

CCalibrationSettings::CCalibrationSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CCalibrationSettings::IDD, pParent)
{
	
}

CCalibrationSettings::~CCalibrationSettings()
{
}

void CCalibrationSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	for (int i = 0; i < 5; i++) {
		DDX_Check(pDX, IDC_CHECK_ENABLE_LIGHT_CORRECTION_INTENSITY1 + i, m_bEnableLightCorrection[i]);
	}
}


BEGIN_MESSAGE_MAP(CCalibrationSettings, CDialog)
	ON_BN_CLICKED(IDC_CALIB_CLOSE, &CCalibrationSettings::OnBnClickedCalibClose)
	ON_BN_CLICKED(IDC_CALIB_INTENSITY_1, &CCalibrationSettings::OnBnClickedCalibIntensity1)
	ON_BN_CLICKED(IDC_CALIB_INTENSITY_2, &CCalibrationSettings::OnBnClickedCalibIntensity2)
	ON_BN_CLICKED(IDC_CALIB_INTENSITY_3, &CCalibrationSettings::OnBnClickedCalibIntensity3)
	ON_BN_CLICKED(IDC_CALIB_INTENSITY_4, &CCalibrationSettings::OnBnClickedCalibIntensity4)
	ON_BN_CLICKED(IDC_CALIB_INTENSITY_5, &CCalibrationSettings::OnBnClickedCalibIntensity5)
	ON_BN_CLICKED(IDC_CALIB_INTENSITY_6, &CCalibrationSettings::OnBnClickedCalibIntensity6)
	ON_BN_CLICKED(IDC_BUTTON1, &CCalibrationSettings::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CCalibrationSettings::OnBnClickedButtonApply)
END_MESSAGE_MAP()


// CCalibrationSettings message handlers


BOOL CCalibrationSettings::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_pApp = (CApplication*) AfxGetApp();

	for(int i = 0;i<5;i++)
	{
		GetDlgItem(IDC_CALIB_INTENSITY_1 + i)->ShowWindow(FALSE);
		if(i<4)
			GetDlgItem(IDC_CALIBDOWN_1+i)->ShowWindow(FALSE);
	}
	nSelDoc = 0;

	GetDlgItem(IDC_CHECK_ENABLE_LIGHT_CORRECTION_INTENSITY1)->ShowWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CCalibrationSettings::Initialize(int nDoc)
{

	for(int i = 0;i<5;i++)
	{
		GetDlgItem(IDC_CALIB_INTENSITY_1 + i)->ShowWindow(FALSE);
		GetDlgItem(IDC_CHECK_ENABLE_LIGHT_CORRECTION_INTENSITY1 +i)->ShowWindow(FALSE);
		if(i<4)
			GetDlgItem(IDC_CALIBDOWN_1+i)->ShowWindow(FALSE);
	}
	CString strTemp;
	CString strCameraName;
	int nLightIDx;
	int i;
	for(i = 0;i<m_pApp->m_nTotalViewDocs[nTrack];i++){
		GetDlgItem(IDC_CALIB_INTENSITY_1 + i)->ShowWindow(TRUE);

		if (i == 0) {
			GetDlgItem(IDC_CHECK_ENABLE_LIGHT_CORRECTION_INTENSITY1 + i)->ShowWindow(TRUE);
		}

		if(i<m_pApp->m_nTotalViewDocs[nTrack]-1)
			GetDlgItem(IDC_CALIBDOWN_1+i)->ShowWindow(TRUE);
		switch(nTrack)
		{
		case 0:strCameraName = m_pApp->m_strTopCameraName.GetAt(i);
				nLightIDx = i;
				break;
		case 1:strCameraName = m_pApp->m_strBottomCameraName.GetAt(i);
				nLightIDx = m_pApp->nTop+i;
				break;
		case 2:strCameraName = m_pApp->m_strSideCameraName.GetAt(i);
				nLightIDx = m_pApp->nTop+m_pApp->nBottom+i;
				break;
		}
		if(i==m_pApp->m_nTotalViewDocs[nTrack]){
		
		}
		else{
			strTemp.Format("%s / %s", m_pApp->m_strLightChannelName.GetAt(nLightIDx),strCameraName);
			GetDlgItem(IDC_CALIB_INTENSITY_1+i)->SetWindowTextA(strTemp);

			if (i == 0) {
				strTemp.Format("%s Light Correction", strCameraName);
				GetDlgItem(IDC_CHECK_ENABLE_LIGHT_CORRECTION_INTENSITY1 + i)->SetWindowTextA(strTemp);
			}
		}

		CCalibrationData* pCalibData = &m_pApp->m_pTracks[nTrack].m_pDoc[i]->m_InspectionHandler[0].m_Inspection.m_CalibrationData;
		m_bEnableLightCorrection[i] = pCalibData->bEnableLightCorrection;
	}
	

	CRect rect, rect1, rectGroup, rectGroup1;
	GetDlgItem(IDC_STATIC_GROUP_BOX1)->GetWindowRect(&rect);
	//GetDlgItem(IDC_STATIC_GROUP_BOX1)->ScreenToClient(&rect);
	GetDlgItem(IDC_CALIB_INTENSITY_1 + i-1)->GetWindowRect(&rect1);
	//GetDlgItem(IDC_STATIC_GROUP_BOX1)->ScreenToClient(&rect1);
	GetDlgItem(IDC_STATIC_GROUP_BOX1)->SetWindowPos(NULL, 0,0, rect.Width(), rect1.bottom + 10-rect.top, SWP_NOMOVE);

	GetDlgItem(IDC_STATIC_GROUP_BOX1)->GetWindowRect(&rectGroup);
	/*GetDlgItem(IDC_STATIC_GROUP_BOX1)->*/ScreenToClient(&rectGroup);
	GetDlgItem(IDC_STATIC_GROUP_BOX2)->GetClientRect(&rectGroup1);
	
	GetDlgItem(IDC_STATIC_GROUP_BOX2)->SetWindowPos(NULL, rectGroup.left, rectGroup.bottom+10, rectGroup1.Width(), rectGroup1.Height(), SWP_NOZORDER);
	strTemp.Format("Dot Calibration");
	GetDlgItem(IDC_CALIB_INTENSITY_6)->ShowWindow(TRUE);
	GetDlgItem(IDC_CALIB_INTENSITY_6)->SetWindowTextA(strTemp);
	GetDlgItem(IDC_CALIB_INTENSITY_6)->GetWindowRect(&rect);
	GetDlgItem(IDC_CALIB_INTENSITY_6)->SetWindowPos(NULL, rectGroup.left+30, rectGroup.bottom + 40, rect.Width(), rect.Height(), SWP_NOZORDER);
	nSelDoc = nDoc;

	m_nDotXAxisPosition= 0;
	m_nDotYAxisPosition = 0;
	m_nDotZAxisPositionTop = 0;
	m_nDotZAxisPositionBottom = 0;

	for (int nDoc = 0; nDoc < m_pApp->m_nTotalViewDocs[nTrack]; nDoc++) {
		m_nXAxisPosition[nTrack][nDoc] = 0;
		m_nYAxisPosition[nTrack][nDoc] = 0;
		m_nZAxisPositionTop[nTrack][nDoc] = 0;
		m_nZAxisPositionBottom[nTrack][nDoc] = 0;
	}
	
	m_pApp->m_strCalibDir.Format("%s\\Calibration", m_pApp->m_strMainCfgDir);
	_mkdir(m_pApp->m_strCalibDir);
	CString strPath;
	strPath.Format("%s\\LightCorrection_MotionSettings_Track%d.mot", m_pApp->m_strCalibDir, nTrack + 1);
	LoadMotionParameters(strPath, FALSE);

	m_pApp->m_strCalibDir.Format("%s\\Calibration", m_pApp->m_strMainCfgDir);
	_mkdir(m_pApp->m_strCalibDir);
	strPath.Format("%s\\DotCalibration_MotionSettings_Track%d.mot", m_pApp->m_strCalibDir, nTrack + 1);
	LoadMotionParameters(strPath, TRUE);

	UpdateData(FALSE);
}

void CCalibrationSettings::OnBnClickedCalibClose()
{
	OnOK();
}


void CCalibrationSettings::OnBnClickedCalibIntensity1()
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	BOOL bTemp;
	GetDlgItem(IDC_CALIB_INTENSITY_1)->GetWindowTextA(strTemp);
	strTemp.CompareNoCase("Dot Calibration")!=0?bTemp=FALSE:bTemp=TRUE;
	//nDoc = 0;
	dlgCalibInfo.m_bDotCalib = bTemp;
	if(!bTemp){
		CString str;
		switch (nTrack) {
		case 0: str = m_pApp->m_strTopCalibrationName.GetAt(0);
			break;
		case 1: str = m_pApp->m_strBottomCalibrationName.GetAt(0);
			break;
		case 2: str = m_pApp->m_strSideCalibrationName.GetAt(0);
			break;
		}

		dlgCalibInfo.m_nStepXCount = 4;
		dlgCalibInfo.m_nStepYCount = 4;
		dlgCalibInfo.m_dStepXSize = 1;
		dlgCalibInfo.m_dStepYSize = 1;

		dlgCalibInfo.strCalibrationName = str;
		if(dlgCalibInfo.DoModal()==IDOK)
		{
			nCalibGreyValue = dlgCalibInfo.m_nCalibGreyValue;
			GoToCalibration(0, FALSE);
		}
		else {
			m_pApp->m_bCalibrationOpen = FALSE;
			return;
		}
			
	}
	else
		GoToCalibration(0, TRUE);
}


void CCalibrationSettings::OnBnClickedCalibIntensity2()
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	BOOL bTemp;
	GetDlgItem(IDC_CALIB_INTENSITY_2)->GetWindowTextA(strTemp);
	strTemp.CompareNoCase("Dot Calibration")!=0?bTemp=FALSE:bTemp=TRUE;
	//nDoc = 1;
	dlgCalibInfo.m_bDotCalib = bTemp;
	if(!bTemp){
		CString str;
		switch (nTrack) {
		case 0: str = m_pApp->m_strTopCalibrationName.GetAt(1);
			break;
		case 1: str = m_pApp->m_strBottomCalibrationName.GetAt(1);
			break;
		case 2: str = m_pApp->m_strSideCalibrationName.GetAt(1);
			break;
		}

		dlgCalibInfo.m_nStepXCount = 4;
		dlgCalibInfo.m_nStepYCount = 4;
		dlgCalibInfo.m_dStepXSize = 1;
		dlgCalibInfo.m_dStepYSize = 1;

		dlgCalibInfo.strCalibrationName = str;
		if(dlgCalibInfo.DoModal()==IDOK)
		{
			nCalibGreyValue = dlgCalibInfo.m_nCalibGreyValue;
			GoToCalibration(1, FALSE);
		}
		else {
			m_pApp->m_bCalibrationOpen = FALSE;
			return;
		}
	}
	else
		GoToCalibration(0, TRUE);
}


void CCalibrationSettings::OnBnClickedCalibIntensity3()
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	BOOL bTemp;
	GetDlgItem(IDC_CALIB_INTENSITY_3)->GetWindowTextA(strTemp);
	strTemp.CompareNoCase("Dot Calibration")!=0?bTemp=FALSE:bTemp=TRUE;
	//nDoc = 2;
	dlgCalibInfo.m_bDotCalib = bTemp;
	if(!bTemp){
		CString str;
		switch (nTrack) {
		case 0: str = m_pApp->m_strTopCalibrationName.GetAt(2);
			break;
		case 1: str = m_pApp->m_strBottomCalibrationName.GetAt(2);
			break;
		case 2: str = m_pApp->m_strSideCalibrationName.GetAt(2);
			break;
		}

		dlgCalibInfo.m_nStepXCount = 4;
		dlgCalibInfo.m_nStepYCount = 4;
		dlgCalibInfo.m_dStepXSize = 1;
		dlgCalibInfo.m_dStepYSize = 1;

		dlgCalibInfo.strCalibrationName = str;
		if(dlgCalibInfo.DoModal()==IDOK)
		{
			nCalibGreyValue = dlgCalibInfo.m_nCalibGreyValue;
			GoToCalibration(2, FALSE);
		}
		else {
			m_pApp->m_bCalibrationOpen = FALSE;
			return;
		}
	}
	else
		GoToCalibration(0, TRUE);
}


void CCalibrationSettings::OnBnClickedCalibIntensity4()
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	BOOL bTemp;
	GetDlgItem(IDC_CALIB_INTENSITY_4)->GetWindowTextA(strTemp);
	strTemp.CompareNoCase("Dot Calibration")!=0?bTemp=FALSE:bTemp=TRUE;
	//nDoc = 3;
	dlgCalibInfo.m_bDotCalib = bTemp;
	if(!bTemp){
		CString str;
		switch (nTrack) {
		case 0: str = m_pApp->m_strTopCalibrationName.GetAt(3);
			break;
		case 1: str = m_pApp->m_strBottomCalibrationName.GetAt(3);
			break;
		case 2: str = m_pApp->m_strSideCalibrationName.GetAt(3);
			break;
		}

		dlgCalibInfo.m_nStepXCount = 4;
		dlgCalibInfo.m_nStepYCount = 4;
		dlgCalibInfo.m_dStepXSize = 1;
		dlgCalibInfo.m_dStepYSize = 1;

		dlgCalibInfo.strCalibrationName = str;
		if(dlgCalibInfo.DoModal()==IDOK)
		{
			nCalibGreyValue = dlgCalibInfo.m_nCalibGreyValue;
			GoToCalibration(3, FALSE);
		}
		else {
			m_pApp->m_bCalibrationOpen = FALSE;
			return;
		}
	}
	else
		GoToCalibration(0, TRUE);
}


void CCalibrationSettings::OnBnClickedCalibIntensity5()
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	BOOL bTemp;
	GetDlgItem(IDC_CALIB_INTENSITY_5)->GetWindowTextA(strTemp);
	strTemp.CompareNoCase("Dot Calibration")!=0?bTemp=FALSE:bTemp=TRUE;
	//nDoc = 4;
	dlgCalibInfo.m_bDotCalib = bTemp;
	if(!bTemp){
		CString str;
		switch (nTrack) {
		case 0: str = m_pApp->m_strTopCalibrationName.GetAt(4);
			break;
		case 1: str = m_pApp->m_strBottomCalibrationName.GetAt(4);
			break;
		case 2: str = m_pApp->m_strSideCalibrationName.GetAt(4);
			break;
		}

		dlgCalibInfo.m_nStepXCount = 4;
		dlgCalibInfo.m_nStepYCount = 4;
		dlgCalibInfo.m_dStepXSize = 1;
		dlgCalibInfo.m_dStepYSize = 1;

		dlgCalibInfo.strCalibrationName = str;
		if(dlgCalibInfo.DoModal()==IDOK)
		{
			nCalibGreyValue = dlgCalibInfo.m_nCalibGreyValue;
			GoToCalibration(4, FALSE);
		}
		else {
			m_pApp->m_bCalibrationOpen = FALSE;
			return;
		}
	}
	else
		GoToCalibration(0, TRUE);
}


void CCalibrationSettings::OnBnClickedCalibIntensity6()
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	BOOL bTemp;
	GetDlgItem(IDC_CALIB_INTENSITY_6)->GetWindowTextA(strTemp);
	strTemp.CompareNoCase("Dot Calibration")!=0?bTemp=FALSE:bTemp=TRUE;
	dlgCalibInfo.m_bDotCalib = bTemp;
	if(!bTemp){
		CString str;
		switch (nTrack) {
		case 0: str = m_pApp->m_strTopCalibrationName.GetAt(0);
			break;
		case 1: str = m_pApp->m_strBottomCalibrationName.GetAt(0);
			break;
		case 2: str = m_pApp->m_strSideCalibrationName.GetAt(0);
			break;
		}

		dlgCalibInfo.m_nStepXCount = 4;
		dlgCalibInfo.m_nStepYCount = 4;
		dlgCalibInfo.m_dStepXSize = 1;
		dlgCalibInfo.m_dStepYSize = 1;

		dlgCalibInfo.strCalibrationName = str;
		if(dlgCalibInfo.DoModal()==IDOK)
		{
			nCalibGreyValue = dlgCalibInfo.m_nCalibGreyValue;
			GoToCalibration(0, FALSE);
		}
		else {
			m_pApp->m_bCalibrationOpen = FALSE;
			return;
		}
	}
	else
		GoToCalibration(0, TRUE);
}


void CCalibrationSettings::GoToCalibration(int nDoc,BOOL bDotCalibration)
{
	if(!bDotCalibration){
		int nDocFrame = !nTrack ? nDoc : (nTrack == 1 ? m_pApp->m_nTop + nDoc : m_pApp->m_nTop + m_pApp->m_nBottom + nDoc);
		m_pApp->m_pFrames[nDocFrame]->MDIActivate();

		m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_bLightCalibration = TRUE;
		if(m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_bLightCalibration){
			
			m_pApp->m_nCalibXAxisPosition = m_nXAxisPosition[nTrack][nDoc];
			m_pApp->m_nCalibYAxisPosition = m_nYAxisPosition[nTrack][nDoc];
			m_pApp->m_nCalibZAxisPositionTop = m_nZAxisPositionTop[nTrack][nDoc];
			m_pApp->m_nCalibZAxisPositionBottom = m_nZAxisPositionBottom[nTrack][nDoc];
			
			OutputDebugLogTo(9, TRUE, "m_pApp->m_nCalibXAxisPosition = %d,m_pApp->m_nCalibYAxisPosition=%d,m_pApp->m_nCalibZAxisPositionTop = %d,m_pApp->m_nCalibZAxisPositionBottom = %d",
				m_pApp->m_nCalibXAxisPosition, m_pApp->m_nCalibYAxisPosition, m_pApp->m_nCalibZAxisPositionTop, m_pApp->m_nCalibZAxisPositionBottom);

			m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_nCalibLightGreyValue = nCalibGreyValue;
			
			m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_nCalibStepXCount = dlgCalibInfo.m_nStepXCount;
			m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_nCalibStepYCount = dlgCalibInfo.m_nStepYCount;
			m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_dCalibStepXSize = dlgCalibInfo.m_dStepXSize;
			m_pApp->m_pTracks[nTrack].m_pDoc[nDoc]->m_dCalibStepYSize = dlgCalibInfo.m_dStepYSize;

			dlgCalibInfo.m_nCalibGreyVal[bDotCalibration] = nCalibGreyValue;

			OutputDebugLogTo(7,TRUE,"Light Calibration Initated For Doc%d",nDoc);
			this->ShowWindow(FALSE);
			m_pApp->m_pTracks[nTrack].m_nCommand = CALIBRATE_LIGHT;
			m_pApp->m_pTracks[nTrack].m_pActiveDoc = m_pApp->m_pTracks[nTrack].m_pDoc[nDoc];
			SetEvent(m_pApp->m_pTracks[nTrack].m_EventRun);
		}
	}
	else {
		int nDocFrame = !nTrack ? nSelDoc : (nTrack == 1 ? m_pApp->m_nTop + nSelDoc : m_pApp->m_nTop + m_pApp->m_nBottom + nSelDoc);
		m_pApp->m_pFrames[nDocFrame]->MDIActivate();
		m_pApp->m_pTracks[nTrack].m_pDoc[nSelDoc]->m_InspectionHandler[0].LoadDotCalibrationParameters(m_pApp->m_strCalibDir, m_pApp->m_strConfigFile, nTrack, 0);
		dlgCalibInfo.m_nStepXCount = m_pApp->m_pTracks[nTrack].m_pDoc[nSelDoc]->m_InspectionHandler[0].m_Inspection.m_CalibrationData.nInputAlongX;
		dlgCalibInfo.m_nStepYCount = m_pApp->m_pTracks[nTrack].m_pDoc[nSelDoc]->m_InspectionHandler[0].m_Inspection.m_CalibrationData.nInputAlongY;
		dlgCalibInfo.m_nMinRadiusDotTarget = m_pApp->m_pTracks[nTrack].m_pDoc[nSelDoc]->m_InspectionHandler[0].m_Inspection.m_CalibrationData.nInputMinRadiusDotTargetPixel;

		dlgCalibInfo.strCalibrationName = "Dot Calibration";
		if (dlgCalibInfo.DoModal() == IDOK) {
			m_pApp->m_pTracks[nTrack].m_pDoc[nSelDoc]->m_InspectionHandler[0].m_Inspection.m_CalibrationData.nInputAlongX = dlgCalibInfo.m_nStepXCount;
			m_pApp->m_pTracks[nTrack].m_pDoc[nSelDoc]->m_InspectionHandler[0].m_Inspection.m_CalibrationData.nInputAlongY = dlgCalibInfo.m_nStepYCount;
			m_pApp->m_pTracks[nTrack].m_pDoc[nSelDoc]->m_InspectionHandler[0].m_Inspection.m_CalibrationData.nInputMinRadiusDotTargetPixel = dlgCalibInfo.m_nMinRadiusDotTarget;

			dlgCalibInfo.m_nCalibGreyVal[bDotCalibration] = dlgCalibInfo.m_nCalibGreyValue;
		}
		else {
			m_pApp->m_bCalibrationOpen = FALSE;
			return;
		}

		m_pApp->m_nCalibXAxisPosition = m_nDotXAxisPosition;
		m_pApp->m_nCalibYAxisPosition = m_nDotYAxisPosition;
		m_pApp->m_nCalibZAxisPositionTop = m_nDotZAxisPositionTop;
		m_pApp->m_nCalibZAxisPositionBottom = m_nDotZAxisPositionBottom;
		OutputDebugLogTo(9, TRUE, "m_pApp->m_nCalibXAxisPosition = %d,m_pApp->m_nCalibYAxisPosition=%d,m_pApp->m_nCalibZAxisPositionTop = %d,m_pApp->m_nCalibZAxisPositionBottom = %d",
			m_pApp->m_nCalibXAxisPosition, m_pApp->m_nCalibYAxisPosition, m_pApp->m_nCalibZAxisPositionTop, m_pApp->m_nCalibZAxisPositionBottom);
		m_pApp->m_pTracks[nTrack].m_pDoc[nSelDoc]->m_bDotCalibration = TRUE;
		this->ShowWindow(FALSE);
		m_pApp->m_pTracks[nTrack].m_nCommand = CALIBRATE;
		m_pApp->m_pTracks[nTrack].m_pActiveDoc = m_pApp->m_pTracks[nTrack].m_pDoc[nSelDoc];
		SetEvent(m_pApp->m_pTracks[nTrack].m_EventRun);
	}

}

void CCalibrationSettings::SaveStartPositionCalib(BOOL bSave)		//// Save Start position 
{
	// TODO: Add your control notification handler code here
	if (m_pApp->m_bCalibrationOpen)
		m_pApp->m_pCalibrationDlg->EnableWindow(TRUE);

	if (bSave) {
		CSaveCalibRationPosition dlgSavePosition;
		dlgSavePosition.nTrack = nTrack;
		if (dlgSavePosition.DoModal() == IDOK) {
			if (!dlgSavePosition.m_bSelectedDotCalibration) {
				if (m_pApp->m_pMotionControl) {
					m_pApp->motionParameters.GetCurrentMotorPosition(0, m_nDotXAxisPosition);
					m_pApp->motionParameters.GetCurrentMotorPosition(1, m_nDotYAxisPosition);
					m_pApp->motionParameters.GetCurrentMotorPosition(2, m_nDotZAxisPositionTop);
					m_pApp->motionParameters.GetCurrentMotorPosition(3, m_nDotZAxisPositionBottom);
					m_pApp->m_strCalibDir.Format("%s\\Calibration", m_pApp->m_strMainCfgDir);
					_mkdir(m_pApp->m_strCalibDir);
					CString strPath;
					strPath.Format("%s\\DotCalibration_MotionSettings_Track%d.mot", m_pApp->m_strCalibDir, nTrack + 1);
					SaveMotionParameters(strPath, TRUE);
				}
			}
			else {
				for (int i = 0; i < m_pApp->m_nTotalViewDocs[nTrack]; i++) {
					if (dlgSavePosition.m_bCheckedIntensity[i]) {
						if (m_pApp->m_pMotionControl) {
							m_pApp->motionParameters.GetCurrentMotorPosition(0, m_nXAxisPosition[nTrack][i]);
							m_pApp->motionParameters.GetCurrentMotorPosition(1, m_nYAxisPosition[nTrack][i]);
							m_pApp->motionParameters.GetCurrentMotorPosition(2, m_nZAxisPositionTop[nTrack][i]);
							m_pApp->motionParameters.GetCurrentMotorPosition(3, m_nZAxisPositionBottom[nTrack][i]);
						}
					}
				}
				m_pApp->m_strCalibDir.Format("%s\\Calibration", m_pApp->m_strMainCfgDir);
				_mkdir(m_pApp->m_strCalibDir);
				CString strPath;
				strPath.Format("%s\\LightCorrection_MotionSettings_Track%d.mot", m_pApp->m_strCalibDir, nTrack + 1);
				SaveMotionParameters(strPath, FALSE);
			}
		}
	}
	CString strPath;
	strPath.Format("%s\\DotCalibration_MotionSettings_Track%d.mot", m_pApp->m_strCalibDir, nTrack + 1);
	LoadMotionParameters(strPath, TRUE);
	strPath.Format("%s\\LightCorrection_MotionSettings_Track%d.mot", m_pApp->m_strCalibDir, nTrack + 1);
	LoadMotionParameters(strPath, FALSE);
}

void CCalibrationSettings::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	if (m_pApp->m_bCalibrationOpen)
		m_pApp->m_pCalibrationDlg->EnableWindow(FALSE);

	if (m_pApp->m_pMotionControl && !m_pApp->m_bMotionCtrlDlgOpen) {
		m_pApp->OnMotionControl();
	}
}


void CCalibrationSettings::SaveMotionParameters(CString strPath,BOOL bIsDotCalibration)
{
	// TODO: Add your control notification handler code here
	CIniFile iniFile;
	CFileException e;

	CString strKey, strFile, strTemp;
	iniFile.SetPath(strPath);

	if (bIsDotCalibration){
		strKey.Format("DotCalibration");
		strTemp.Format("XPos");
		iniFile.SetInteger(strKey, strTemp, m_nDotXAxisPosition);
		strTemp.Format("YPos");
		iniFile.SetInteger(strKey, strTemp, m_nDotYAxisPosition);
		strTemp.Format("Top ZPos");
		iniFile.SetInteger(strKey, strTemp, m_nDotZAxisPositionTop);
		strTemp.Format("Bottom ZPos");
		iniFile.SetInteger(strKey, strTemp, m_nDotZAxisPositionBottom);
	}
	else {
		strKey.Format("LightCorrection");
		for (int i = 0; i < m_pApp->m_nTotalViewDocs[nTrack]; i++) {
			strTemp.Format("Doc%d XPos",i+1);
			iniFile.SetInteger(strKey, strTemp, m_nXAxisPosition[nTrack][i]);
			strTemp.Format("Doc%d YPos", i + 1);
			iniFile.SetInteger(strKey, strTemp, m_nYAxisPosition[nTrack][i]);
			strTemp.Format("Doc%d Top ZPos", i + 1);
			iniFile.SetInteger(strKey, strTemp, m_nZAxisPositionTop[nTrack][i]);
			strTemp.Format("Doc%d Bottom ZPos", i + 1);
			iniFile.SetInteger(strKey, strTemp, m_nZAxisPositionBottom[nTrack][i]);
		}
		
	}
	iniFile.WriteFile();
}

void CCalibrationSettings::LoadMotionParameters(CString strPath, BOOL bIsDotCalibration)
{
	// TODO: Add your control notification handler code here
	CIniFile iniFile;
	CFileException e;

	CString strKey, strFile, strTemp;
	iniFile.SetPath(strPath);

	if (iniFile.ReadFile()) {
		if (bIsDotCalibration) {
			strKey.Format("DotCalibration");
			strTemp.Format("XPos");
			m_nDotXAxisPosition = iniFile.GetInteger(strKey, strTemp, m_nDotXAxisPosition);
			strTemp.Format("YPos");
			m_nDotYAxisPosition = iniFile.GetInteger(strKey, strTemp, m_nDotYAxisPosition);
			strTemp.Format("Top ZPos");
			m_nDotZAxisPositionTop = iniFile.GetInteger(strKey, strTemp, m_nDotZAxisPositionTop);
			strTemp.Format("Bottom ZPos");
			m_nDotZAxisPositionBottom = iniFile.GetInteger(strKey, strTemp, m_nDotZAxisPositionBottom);
		}
		else {
			strKey.Format("LightCorrection");
			for (int i = 0; i < m_pApp->m_nTotalViewDocs[nTrack]; i++) {
				strTemp.Format("Doc%d XPos", i + 1);
				m_nXAxisPosition[nTrack][i] = iniFile.GetInteger(strKey, strTemp, m_nXAxisPosition[nTrack][i]);
				strTemp.Format("Doc%d YPos", i + 1);
				m_nYAxisPosition[nTrack][i] = iniFile.GetInteger(strKey, strTemp, m_nYAxisPosition[nTrack][i]);
				strTemp.Format("Doc%d Top ZPos", i + 1);
				m_nZAxisPositionTop[nTrack][i] = iniFile.GetInteger(strKey, strTemp, m_nZAxisPositionTop[nTrack][i]);
				strTemp.Format("Doc%d Bottom ZPos", i + 1);
				m_nZAxisPositionBottom[nTrack][i] = iniFile.GetInteger(strKey, strTemp, m_nZAxisPositionBottom[nTrack][i]);
			}

		}
	}
}

void CCalibrationSettings::OnBnClickedButtonApply()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CCalibrationData* pCalibData = &m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[0].m_Inspection.m_CalibrationData;
	if (pCalibData->bEnableLightCorrection != m_bEnableLightCorrection[0]) {
		pCalibData->bEnableLightCorrection = m_bEnableLightCorrection[0];
		m_pApp->m_pTracks[nTrack].m_pDoc[0]->m_InspectionHandler[0].SaveCalibrationParameters(m_pApp->m_strCalibDir, m_pApp->m_strConfigFile, nTrack, 0);
		
		CString strMsg;
		switch (nTrack){
		case 0:
			strMsg.Format("Please Re-teach %s", m_pApp->m_strTopCameraName.GetAt(0));
			break;
		case 1:
			strMsg.Format("Please Re-teach %s", m_pApp->m_strBottomCameraName.GetAt(0));
			break;
		case 2:
			strMsg.Format("Please Re-teach %s", m_pApp->m_strSideCameraName.GetAt(0));
			break;
		}
		AfxMessageBox(strMsg, IDOK);
	}
}

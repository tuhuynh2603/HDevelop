/// MotionControlDialog.cpp : implementation file
/// Udupa; Dec'2016

#include "stdafx.h"
#include "Application.h"
#include "TrackHandler.h"
#include "AppDoc.h"
#include "MotionControlDialog.h"
#include "SavePositionDialog.h"
#include "afxdialogex.h"
#include "CalibrationSettings.h"
#include "MotionParameters.h"
#include "StationMapping.h"

static const UINT_PTR MOTION_TIMER = 777;

IMPLEMENT_DYNAMIC(CMotionControlDialog, CDialogEx)

CMotionControlDialog::CMotionControlDialog(MotionParameters * pMotionParmsInit, CWnd* pParent /*=NULL*/)
	: CDialog(CMotionControlDialog::IDD, pParent)
	, m_nHomeORGOffset(0)
	, m_bMotorOff(FALSE)
	, m_nMotorID(0)
	, m_nMotorIDPrev(0)
	, m_bForwardMove(FALSE)
	, m_bForwardHome(FALSE)
	, m_nMotorMovePos(0)
	, m_nMotorSettlingTime(0)
	, m_strMotorLimitAlarm(_T(""))
	, m_dHomeStartVelMM(0)
	, m_dHomeMaxVelMM(0)
	, m_dMoveStartVelMM(0)
	, m_dMoveMaxVelMM(0)
	, m_dScanStartVelMM(0)
	, m_dScanStartVelMMTHR(0)
	, m_dScanMaxVelMM(0)
	, m_dScanMaxVelMMTHR(0)
	, m_nMotorMoveRelativePos(0)
	, m_dHomeStartVelKM(0)
	, m_dMoveStartVelKM(0)
	, m_dScanStartVelKM(0)
	, m_dScanStartVelKMTHR(0)
	, m_dHomeMaxVelKM(0)
	, m_dMoveMaxVelKM(0)
	, m_dScanMaxVelKM(0)
	, m_dScanMaxVelKMTHR(0)
	, m_dMotorMovePosMM(0)
	, m_dMotorMoveRelativePosMM(0)
	, strMotorPos(_T(""))
	, _verificationMotorStepMm(0.01)
	//	, m_bHomeCheckByPass(FALSE)
{
	linearEncoderPositionCounter = 0;
	//	verifyLinearEncoderPosition = false;

	nTotalMotorNo = 0;
	//	nTotalScaleNo = 0;

	bChecked = NULL;
	bRunThread = false;
	//pApp = NULL;

	m_bContinuousMove = FALSE;

	bCreateHeader = false;
	if (pMotionParmsInit != NULL) {
		pMotionParms = new MotionParameters(*pMotionParmsInit);

		nTotalMotorNo = pMotionParmsInit->GetTotalMotorCount();
		//		nTotalScaleNo = pMotionParmsInit->GetTotalScaleCount();

		//		linearEncoderPosition.resize(nTotalScaleNo);

		//for(size_t i = 0; i < linearEncoderPosition.size(); i++)
		//	linearEncoderPosition[i].resize(1000);

		if (nTotalMotorNo > 0/* && nTotalScaleNo > 0*/) {

			motorPos.resize(nTotalMotorNo);
			//			scalePos.resize(nTotalScaleNo);
			previousMotorPos.resize(nTotalMotorNo);
			//			previousScalePos.resize(nTotalScaleNo);
			previousMotorHomed.resize(nTotalMotorNo);

			std::fill(motorPos.begin(), motorPos.end(), 0);
			//			std::fill(scalePos.begin(), scalePos.end(), 0);
			std::fill(previousMotorPos.begin(), previousMotorPos.end(), 0);
			//			std::fill(previousScalePos.begin(), previousScalePos.end(), 0);
			std::fill(previousMotorHomed.begin(), previousMotorHomed.end(), FALSE);
		}

		double dMotorResolution = 1;

		pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);

		homeSpeed = pMotionParms->getHomingSpeed(m_nMotorID);

		m_dHomeStartVelMM = Round(10 * homeSpeed.startSpeed / dMotorResolution) / 10.;
		m_dHomeMaxVelMM = Round(10 * homeSpeed.maximumSpeed / dMotorResolution) / 10.;
		m_dHomeStartVelKM = Round(m_dHomeStartVelMM * 36 / 100) / 10.;
		m_dHomeMaxVelKM = Round(m_dHomeMaxVelMM * 36 / 100) / 10.;

		moveSpeed = pMotionParms->getMovingSpeed(m_nMotorID);

		m_dMoveStartVelMM = Round(10 * moveSpeed.startSpeed / dMotorResolution) / 10.;
		m_dMoveMaxVelMM = Round(10 * moveSpeed.maximumSpeed / dMotorResolution) / 10.;
		m_dMoveStartVelKM = Round(m_dMoveStartVelMM * 36 / 100) / 10.;
		m_dMoveMaxVelKM = Round(m_dMoveMaxVelMM * 36 / 100) / 10.;

		scanSpeed = pMotionParms->getScanningSpeed(m_nMotorID);

		m_dScanStartVelMM = Round(10 * scanSpeed.startSpeed / dMotorResolution) / 10.;
		m_dScanMaxVelMM = Round(10 * scanSpeed.maximumSpeed / dMotorResolution) / 10.;
		m_dScanStartVelKM = Round(m_dScanStartVelMM * 36 / 100) / 10.;
		m_dScanMaxVelKM = Round(m_dScanMaxVelMM * 36 / 100) / 10.;

		scanSpeedTHR = pMotionParms->getScanningSpeedTHR(m_nMotorID);

		m_dScanStartVelMMTHR = Round(10 * scanSpeedTHR.startSpeed / dMotorResolution) / 10.;
		m_dScanMaxVelMMTHR = Round(10 * scanSpeedTHR.maximumSpeed / dMotorResolution) / 10.;
		m_dScanStartVelKMTHR = Round(m_dScanStartVelMMTHR * 36 / 100) / 10.;
		m_dScanMaxVelKMTHR = Round(m_dScanMaxVelMMTHR * 36 / 100) / 10.;

		pMotionParms->GetMotorHomeOriginOffset(m_nMotorID, m_nHomeORGOffset);
		pMotionParms->GetMotorMovementDirection(m_nMotorID, m_bForwardMove);
		pMotionParms->GetMotorHomeDirection(m_nMotorID, m_bForwardHome);
		pMotionParms->GetMotorSettlingTime(m_nMotorID, m_nMotorSettlingTime);

		//		m_bHomeCheckByPass = pMotionParmsInit->bEnableMotorHomeByPass;

		m_bMotorOff = !pMotionParms->IsMotorActive(m_nMotorID);

		for (int i = 0; i<nTotalMotorNo; i++)
			previousMotorHomed[i] = !pMotionParmsInit->IsMotorHomed(i);
	}
	else
		pMotionParms = NULL;

	pMotionParmsInitial = pMotionParmsInit;

	nMotionCommand = -1;

	pScanningTeachDialog = NULL;

	bMotorLimitAlarm = FALSE;

	bMotorServoStatus = TRUE;

	bActiveTimer = false;

	bEditBoxUpdate[0] = FALSE;
	bEditBoxUpdate[1] = FALSE;

	//	verifyEncoderVariation = false;
	//	displayEncoderVariation = true;

	linearEncoderVariationId = 0;

	moveMotorForVerification = false;
	currentVerificationMotorPositionId = 0;
	moveMotorToNextVerificationPosition = false;
	moveXMotorForVerification = true;

	motionIOGood[0] = false;
	motionIOGood[1] = false;
	motionIOGood[2] = false;
	motionIOGood[3] = true;
	motionIOGood[4] = false;
	motionIOGood[5] = true;
	motionIOGood[6] = true;
	motionIOGood[7] = true;
	motionIOGood[8] = false;

	clListControlWhite = RGB(255, 255, 224);
	clListControlGreen = RGB(0, 240, 10);
	clListControlRed = RGB(255, 10, 0);

}

CMotionControlDialog::~CMotionControlDialog()
{
	if (bChecked != NULL)
		*bChecked = FALSE;

	if (bRunThread)
		bRunThread = false;

	if (pMotionParms != NULL) {
		delete pMotionParms;
		pMotionParms = NULL;
	}
}

BOOL CMotionControlDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_PosMovement.LoadBitmaps(IDB_ARROWRIGHT, IDB_ARROWRIGHT_CLICKED);
	m_NegMovement.LoadBitmaps(IDB_ARROWLEFT, IDB_ARROWLEFT_CLICKED);

	m_PosRelativeMovement.LoadBitmaps(IDB_ARROWRIGHT_SMALL, IDB_ARROWRIGHT_SMALL_CLICKED);
	m_NegRelativeMovement.LoadBitmaps(IDB_ARROWLEFT_SMALL, IDB_ARROWLEFT_SMALL_CLICKED);


	//	UpdateData(TRUE);
	//	LoadMotorParm();
	//for(int nAxes=0; nAxes<NO_OF_AXIS; nAxes++)
	//	m_bServoDisable[nAxes] = FALSE;//TRUE;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CMotionControlDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MOTOR_POS_LIST, m_ctrlMotorPos);
	//	DDX_Control(pDX, IDC_LINEAR_ENCODER_POS_LIST, m_ctrlLinearEncoderPos);
	DDX_Control(pDX, IDC_MOTOR_ID_COMBO, m_cmbMotorID);

	DDX_Text(pDX, IDC_HOME_START_VEL, homeSpeed.startSpeed);
	DDX_Text(pDX, IDC_HOME_MAX_VEL, homeSpeed.maximumSpeed);
	DDX_Text(pDX, IDC_MOVE_START_VEL, moveSpeed.startSpeed);
	DDX_Text(pDX, IDC_MOVE_MAX_VEL, moveSpeed.maximumSpeed);
	DDX_Text(pDX, IDC_HOME_ORG_OFFSET, m_nHomeORGOffset);
	DDX_Check(pDX, IDC_MOTOR_OFF, m_bMotorOff);
	DDX_CBIndex(pDX, IDC_MOTOR_ID_COMBO, m_nMotorID);
	DDX_Text(pDX, IDC_SCAN_START_VEL, scanSpeed.startSpeed);
	DDX_Text(pDX, IDC_SCAN_MAX_VEL, scanSpeed.maximumSpeed);
//	DDX_Text(pDX, IDC_SCAN_START_VEL_THR, scanSpeedTHR.startSpeed);
//	DDX_Text(pDX, IDC_SCAN_MAX_VEL_THR, scanSpeedTHR.maximumSpeed);
	DDX_Check(pDX, IDC_FORWARD_MOVE, m_bForwardMove);
	DDX_Check(pDX, IDC_FORWARD_HOME, m_bForwardHome);
	DDX_Text(pDX, IDC_MOTOR_MOVE_POSITION, m_nMotorMovePos);
	DDX_Text(pDX, IDC_SETTLING_TIME, m_nMotorSettlingTime);
	//	DDX_Text(pDX, IDC_LIMIT_ALARM, m_strMotorLimitAlarm);
	DDX_Text(pDX, IDC_HOME_START_VEL_MM, m_dHomeStartVelMM);
	DDX_Text(pDX, IDC_HOME_MAX_VEL_MM, m_dHomeMaxVelMM);
	DDX_Text(pDX, IDC_MOVE_START_VEL_MM, m_dMoveStartVelMM);
	DDX_Text(pDX, IDC_MOVE_MAX_VEL_MM, m_dMoveMaxVelMM);
	DDX_Text(pDX, IDC_SCAN_START_VEL_MM, m_dScanStartVelMM);
//	DDX_Text(pDX, IDC_SCAN_START_VEL_MM_THR, m_dScanStartVelMMTHR);
	DDX_Text(pDX, IDC_SCAN_MAX_VEL_MM, m_dScanMaxVelMM);
//	DDX_Text(pDX, IDC_SCAN_MAX_VEL_MM_THR, m_dScanMaxVelMMTHR);
	DDX_Text(pDX, IDC_MOTOR_RELATIVE_MOVE_POSITION, m_nMotorMoveRelativePos);
	//	DDX_Text(pDX, IDC_HOME_START_VEL_KM, m_dHomeStartVelKM);
	//	DDX_Text(pDX, IDC_MOVE_START_VEL_KM, m_dMoveStartVelKM);
	//	DDX_Text(pDX, IDC_SCAN_START_VEL_KM, m_dScanStartVelKM);
	//	DDX_Text(pDX, IDC_HOME_MAX_VEL_KM, m_dHomeMaxVelKM);
	//	DDX_Text(pDX, IDC_MOVE_MAX_VEL_KM, m_dMoveMaxVelKM);
	//	DDX_Text(pDX, IDC_SCAN_MAX_VEL_KM, m_dScanMaxVelKM);
	DDX_Text(pDX, IDC_MOTOR_MOVE_POSITION_MM, m_dMotorMovePosMM);
	DDX_Text(pDX, IDC_MOTOR_RELATIVE_MOVE_POSITION_MM, m_dMotorMoveRelativePosMM);
	//	DDX_Control(pDX, IDC_MOTOR_POSITION, ctrlMotorPos);
	DDX_Control(pDX, IDC_NEG_MOVEMENT, m_NegMovement);
	DDX_Control(pDX, IDC_POS_MOVEMENT, m_PosMovement);

	DDX_Control(pDX, IDC_RELATIVE_MOVE, m_NegRelativeMovement);
	DDX_Control(pDX, IDC_RELATIVE_MOVE_NEGATIVE, m_PosRelativeMovement);
	DDX_Control(pDX, IDC_STATIC_MOTOR_RESOLUTION, m_ctrlMotorResolution);
	//	DDX_Text(pDX, IDC_VERIFICATION_STEP, _verificationMotorStepMm);
	//	DDX_Check(pDX, IDC_BYPASS_HOME_CHECK, m_bHomeCheckByPass);
}

BEGIN_MESSAGE_MAP(CMotionControlDialog, CDialog)
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDOK, &CMotionControlDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_HOME, &CMotionControlDialog::OnBnClickedHome)
	ON_BN_CLICKED(IDC_MOVE_TO_MOTOR_POS, &CMotionControlDialog::OnBnClickedMoveToMotorPos)
	ON_BN_CLICKED(IDC_POS_MOVEMENT, &CMotionControlDialog::OnBnClickedPosMovement)
	ON_BN_CLICKED(IDC_NEG_MOVEMENT, &CMotionControlDialog::OnBnClickedNegMovement)
	ON_CBN_SELCHANGE(IDC_MOTOR_ID_COMBO, &CMotionControlDialog::OnCbnSelchangeMotorIdCombo)
	ON_BN_CLICKED(IDC_MOTOR_OFF, &CMotionControlDialog::OnBnClickedMotorOff)
	ON_BN_CLICKED(IDC_STOP_MOTORS, &CMotionControlDialog::OnBnClickedStopMotors)
	ON_BN_CLICKED(IDC_HOME_ALL, &CMotionControlDialog::OnBnClickedHomeAll)
	ON_BN_CLICKED(IDC_APPLY_MOTION_PARAM, &CMotionControlDialog::OnBnClickedApplyParameters)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SETUP_SCAN, &CMotionControlDialog::OnBnClickedSetupScan)
	ON_BN_CLICKED(IDC_RELATIVE_MOVE, &CMotionControlDialog::OnBnClickedRelativeMove)
	ON_EN_CHANGE(IDC_HOME_START_VEL, &CMotionControlDialog::OnEnChangeHomeStartVel)
	ON_EN_CHANGE(IDC_HOME_MAX_VEL, &CMotionControlDialog::OnEnChangeHomeMaxVel)
	ON_EN_CHANGE(IDC_MOVE_START_VEL, &CMotionControlDialog::OnEnChangeMoveStartVel)
	ON_EN_CHANGE(IDC_SCAN_START_VEL, &CMotionControlDialog::OnEnChangeScanStartVel)
//	ON_EN_CHANGE(IDC_SCAN_START_VEL_THR, &CMotionControlDialog::OnEnChangeScanStartVelTHR)
	ON_EN_CHANGE(IDC_MOVE_MAX_VEL, &CMotionControlDialog::OnEnChangeMoveMaxVel)
	ON_EN_CHANGE(IDC_SCAN_MAX_VEL, &CMotionControlDialog::OnEnChangeScanMaxVel)
//	ON_EN_CHANGE(IDC_SCAN_MAX_VEL_THR, &CMotionControlDialog::OnEnChangeScanMaxVelTHR)
	ON_EN_CHANGE(IDC_HOME_START_VEL_MM, &CMotionControlDialog::OnEnChangeHomeStartVelMm)
	ON_EN_CHANGE(IDC_HOME_MAX_VEL_MM, &CMotionControlDialog::OnEnChangeHomeMaxVelMm)
	ON_EN_CHANGE(IDC_MOVE_START_VEL_MM, &CMotionControlDialog::OnEnChangeMoveStartVelMm)
	ON_EN_CHANGE(IDC_SCAN_START_VEL_MM, &CMotionControlDialog::OnEnChangeScanStartVelMm)
//	ON_EN_CHANGE(IDC_SCAN_START_VEL_MM_THR, &CMotionControlDialog::OnEnChangeScanStartVelMmTHR)
	ON_EN_CHANGE(IDC_MOVE_MAX_VEL_MM, &CMotionControlDialog::OnEnChangeMoveMaxVelMm)
	ON_EN_CHANGE(IDC_SCAN_MAX_VEL_MM, &CMotionControlDialog::OnEnChangeScanMaxVelMm)
//	ON_EN_CHANGE(IDC_SCAN_MAX_VEL_MM_THR, &CMotionControlDialog::OnEnChangeScanMaxVelMmTHR)
	//ON_WM_KILLFOCUS()
	ON_EN_CHANGE(IDC_SETTLING_TIME, &CMotionControlDialog::OnEnChangeSettlingTime)
	ON_BN_CLICKED(IDC_STOP_MOTOR, &CMotionControlDialog::OnBnClickedStopMotor)
	//	ON_BN_CLICKED(IDC_RELOAD_SETTINGS, &CMotionControlDialog::OnBnClickedReloadSettings)
	ON_BN_CLICKED(IDC_RESET_ALARM, &CMotionControlDialog::OnBnClickedResetAlarm)
	ON_EN_CHANGE(IDC_MOTOR_MOVE_POSITION, &CMotionControlDialog::OnEnChangeMotorMovePosition)
	ON_EN_CHANGE(IDC_MOTOR_RELATIVE_MOVE_POSITION, &CMotionControlDialog::OnEnChangeMotorRelativeMovePosition)
	ON_EN_CHANGE(IDC_MOTOR_MOVE_POSITION_MM, &CMotionControlDialog::OnEnChangeMotorMovePositionMm)
	ON_EN_CHANGE(IDC_MOTOR_RELATIVE_MOVE_POSITION_MM, &CMotionControlDialog::OnEnChangeMotorRelativeMovePositionMm)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RELATIVE_MOVE_NEGATIVE, &CMotionControlDialog::OnBnClickedRelativeMoveNegative)
	//	ON_BN_CLICKED(IDC_LINEAR_ENCODER_POSITION_VERIFICATION, &CMotionControlDialog::OnBnClickedLinearEncoderPositionVerification)
	//	ON_BN_CLICKED(IDC_LINEAR_ENCODER_VARIATION_VERIFICATION, &CMotionControlDialog::OnBnClickedLinearEncoderVariationVerification)
	//	ON_BN_CLICKED(IDC_VERIFY_MULTIPLE_POSITIONS, &CMotionControlDialog::OnBnClickedVerifyMultiplePositions)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_MOTOR_POS_LIST, &CMotionControlDialog::OnCustomdrawMotorPosList)
END_MESSAGE_MAP()

LRESULT CMotionControlDialog::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CDialog::OnNcHitTest(point);//HTCAPTION;
}

void CMotionControlDialog::OnBnClickedOk()
{
	_resetValidation();

	if (bChecked != NULL)
		*bChecked = FALSE;

	if (bRunThread) {
		bRunThread = false;

		KillTimer(MOTION_TIMER);
	}

	if (pScanningTeachDialog != NULL) {
		pScanningTeachDialog->DestroyWindow();
		delete pScanningTeachDialog;
		pScanningTeachDialog = NULL;
	}


	theApp.m_bMotionCtrlDlgOpen = FALSE;
	KillTimer(1);

	theApp.StationMapDlgControls();

	if (theApp.m_pStationMapDlg && theApp.m_bStationMapDlgOpen) {
		theApp.m_pStationMapDlg->OnBnClickedSmSetImgIdButton();
	}
	if (theApp.m_pCalibrationDlg && theApp.m_bCalibrationOpen) {
		theApp.m_pCalibrationDlg->SaveStartPositionCalib(TRUE);
	}

	ShowWindow(SW_HIDE);
}

int CMotionControlDialog::InitializeListControlHeaders()
{
	int i;

	m_ctrlMotorPos.SetRedraw(FALSE);
	//	m_ctrlLinearEncoderPos.SetRedraw(FALSE);

	if (!bCreateHeader) {
		LV_ITEM lvitem;

		int nElementWidth = 45;
		CString strIOStatus[9] = {
			"ALR", // Servo Alarm
			"PEL", // Positive End Limit
			"MEL", // Negative End Limit
			"ORG", // Original position sensor (home sensor)
			"EMG", // EMG sensor
			"EZ",  // EZ passed
			"INP", // In Position
			"SVON",// Servo-ON
			"RDY"  // Ready
		};

		m_ctrlMotorPos.DeleteAllItems();
		//		m_ctrlLinearEncoderPos.DeleteAllItems();

		m_FontList.CreateFont(15, 0, 0, 0, 100, FALSE, FALSE, FALSE,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "Arial");

		m_ctrlMotorPos.SetFont(&m_FontList);
		//		m_ctrlLinearEncoderPos.SetFont(&m_FontList);

		m_ctrlMotorPos.InsertColumn(0, "Motor Name", LVCFMT_CENTER, 100, 0);
		m_ctrlMotorPos.InsertColumn(1, "Pos, pulses", LVCFMT_CENTER, 80, 0);
		m_ctrlMotorPos.InsertColumn(2, "Pos, mm", LVCFMT_CENTER, 65, 0);
		m_ctrlMotorPos.InsertColumn(3, "Homed", LVCFMT_CENTER, 55, 0);
		for (i = 0; i<9; i++)
			m_ctrlMotorPos.InsertColumn(i + 4, strIOStatus[i], LVCFMT_CENTER, nElementWidth, 0);

		lvitem.iSubItem = 0;
		lvitem.pszText = "";
		lvitem.mask = LVIF_TEXT;

		CString strTemp = "";
		for (i = 0; i<nTotalMotorNo; i++) {
			lvitem.iItem = i + 1;
			m_ctrlMotorPos.InsertItem(&lvitem);
			pMotionParms->GetMotorName(i, strTemp);
			m_ctrlMotorPos.SetItemText(i, 0, strTemp);
			m_ctrlMotorPos.SetItemText(i, 1, "0");
			m_ctrlMotorPos.SetItemText(i, 2, "0");
			m_ctrlMotorPos.SetItemText(i, 3, "No");
			for (int j = 0; j<9; j++)
				m_ctrlMotorPos.SetItemText(i, j + 4, "");
		}

		bCreateHeader = true;
	}

	if (nTotalMotorNo > 0) {
		for (i = 0; i<nTotalMotorNo; i++) {
			m_ctrlMotorPos.SetItemText(i, 1, "0");
			m_ctrlMotorPos.SetItemText(i, 2, "0");
			m_ctrlMotorPos.SetItemText(i, 3, "No");
		}

		//Initialize combobox items
		m_cmbMotorID.ResetContent();
		CString strName;
		for (i = 0; i<nTotalMotorNo; i++) {
			pMotionParms->GetMotorName(i, strName);
			m_cmbMotorID.AddString(strName);
		}

		//Set default selection as first item
		m_cmbMotorID.SetCurSel(0);

		CString text;
		double motorResolution = 0;
		pMotionParms->GetMotorResolution(0, motorResolution);
		text.Format("Resolution: %.3f", motorResolution);
		m_ctrlMotorResolution.SetWindowText(text);

		//Reinitialize to default selection parameters
		OnCbnSelchangeMotorIdCombo();
	}

	// Allocating memory
	motionIoStatus.resize(nTotalMotorNo);
	prevIoStatus.resize(nTotalMotorNo);
	currentIoStatus.resize(nTotalMotorNo);

	std::fill(currentIoStatus.begin(), currentIoStatus.end(), 0);
	std::fill(prevIoStatus.begin(), prevIoStatus.end(), 0);

	for (i = 0; i<nTotalMotorNo; i++) {
		motionIoStatus[i].resize(9);

		std::fill(motionIoStatus[i].begin(), motionIoStatus[i].end(), false);

		for (int j = 0; j<9; j++) {
			m_ctrlMotorPos.SetItemText(i, j + 4, "");
		}
	}

	m_ctrlMotorPos.SetRedraw(TRUE);
	//	m_ctrlLinearEncoderPos.SetRedraw(TRUE);

	return 0;
}

void CMotionControlDialog::DisplayPos()
{
	if (!bRunThread) {
		bRunThread = true;

		InitializeListControlHeaders();

		SetTimer(MOTION_TIMER, 50, NULL); // 25 FPS

	}
}

void CMotionControlDialog::RedrawMotorPosStatus()
{
	if (!pMotionParms) return;

	CString strPos;

	if (nTotalMotorNo > 0) {

		for (int i = 0; i<nTotalMotorNo; i++)
			pMotionParms->GetCurrentMotorPosition(i, motorPos[i]);

		if (bMotorLimitAlarm != pMotionParms->IsMotorLimit(m_nMotorID)) {
			bMotorLimitAlarm = pMotionParms->IsMotorLimit(m_nMotorID);
			if (bMotorLimitAlarm)
				m_strMotorLimitAlarm.Format("WARNING!!! MOTOR LITMIT");
			else
				m_strMotorLimitAlarm.Empty();

		}

		double dMotorResolution = 1;


		for (int i = 0; i<nTotalMotorNo; i++) {

			if (previousMotorPos[i] != motorPos[i]) {

				previousMotorPos[i] = motorPos[i];

				strPos.Format("%d", motorPos[i]);

				m_ctrlMotorPos.SetItemText(i, 1, strPos);

				pMotionParms->GetMotorResolution(i, dMotorResolution);

				strPos.Format("%.3f", motorPos[i] / dMotorResolution);

				m_ctrlMotorPos.SetItemText(i, 2, strPos);
			}

			if (pMotionParmsInitial->IsMotorHomed(i) != previousMotorHomed[i]) {

				previousMotorHomed[i] = !previousMotorHomed[i];

				if (previousMotorHomed[i])
					m_ctrlMotorPos.SetItemText(i, 3, "Yes");
				else
					m_ctrlMotorPos.SetItemText(i, 3, "No");
			}

		}

		double dScaleResolution = 1;
		BOOL bMotorForwardMovement = TRUE;


		GetDlgItem(IDC_MOTOR_ID_COMBO)->UpdateData(TRUE);

		strMotorPos.Format("%d", motorPos[m_nMotorID]);

		//Check servo-motor status
		bMotorServoStatus = pMotionParmsInitial->IsMotorActive(m_nMotorID);

		GetDlgItem(IDC_MOTOR_OFF)->UpdateData(TRUE);

		if (m_bMotorOff == bMotorServoStatus) {
			m_bMotorOff = !bMotorServoStatus;
			GetDlgItem(IDC_MOTOR_OFF)->UpdateData(FALSE);
		}

		GetDlgItem(IDC_MOTOR_POS_LIST)->UpdateData(FALSE);
		//		ctrlMotorPos.SetWindowText(strMotorPos);

		if (!motionIoStatus.empty() && !prevIoStatus.empty() && !currentIoStatus.empty()) {

			int i, j;
			bool bRedraw = false;

			for (i = 0; i < nTotalMotorNo; i++)
				theApp.motionParameters.GetMotorIOStatus(i, currentIoStatus[i]);

			for (i = 0; i < nTotalMotorNo; i++) {
				if (currentIoStatus[i] != prevIoStatus[i])
					bRedraw = true;
			}

			if (bRedraw) {
				UINT nBit = 0x01;
				for (i = 0; i < nTotalMotorNo; i++) {
					nBit = 1;
					for (j = 0; j < 9; j++, nBit *= 2) {
						motionIoStatus[i][j] = (currentIoStatus[i] & nBit) != 0;
						m_ctrlMotorPos.SetItemText(i, j + 4, "");
					}
					prevIoStatus[i] = currentIoStatus[i];
				}
			}
		}

		//		ctrlMotorPos.UpdateData(FALSE);
	}
}

void CMotionControlDialog::OnBnClickedHome()
{
	// 1. Stop current movement
	// 2. Do homing

	UpdateParameters();
	MotorStopped();
	Sleep(50);

	int nError = pMotionParmsInitial->StopAxis(m_nMotorID);
	if (nError) return;


	nError = pMotionParmsInitial->HomeMove(m_nMotorID, true, false);
	if (nError) return;

	nMotionCommand = 0;
}

void CMotionControlDialog::OnBnClickedMoveToMotorPos()
{
	// 1. Stop current movement
	// 2. Move

	UpdateParameters();
	MotorStopped();
	Sleep(50);
	int nError = pMotionParmsInitial->StopAxis(m_nMotorID);
	if (nError) return;

	nError = pMotionParmsInitial->AbsoluteMove(m_nMotorID, m_nMotorMovePos, pMotionParmsInitial->getMovingSpeed(m_nMotorID), true, false);
	if (nError) return;
}

void CMotionControlDialog::OnBnClickedPosMovement()
{
	Move(m_nMotorID, 1);
}

void CMotionControlDialog::OnBnClickedNegMovement()
{
	Move(m_nMotorID, -1);
}


void CMotionControlDialog::OnCbnSelchangeMotorIdCombo()
{
	UpdateData(TRUE);

	//Change the Button captions to reflect selected motor.
	switch (m_nMotorID) {
	case X_SERVO_MOTOR: //23D X-axis servo motor.
		m_PosMovement.LoadBitmaps(IDB_ARROWRIGHT, IDB_ARROWRIGHT_CLICKED);
		m_NegMovement.LoadBitmaps(IDB_ARROWLEFT, IDB_ARROWLEFT_CLICKED);

		m_PosRelativeMovement.LoadBitmaps(IDB_ARROWRIGHT_SMALL, IDB_ARROWRIGHT_SMALL_CLICKED);
		m_NegRelativeMovement.LoadBitmaps(IDB_ARROWLEFT_SMALL, IDB_ARROWLEFT_SMALL_CLICKED);

		break;

	case Y_SERVO_MOTOR: //23D Y-axis servo motor(top) 
		m_PosMovement.LoadBitmaps(IDB_ARROWUP, IDB_ARROWUP_CLICKED);
		m_NegMovement.LoadBitmaps(IDB_ARROWDOWN, IDB_ARROWDOWN_CLICKED);

		m_PosRelativeMovement.LoadBitmaps(IDB_ARROWUP_SMALL, IDB_ARROWUP_SMALL_CLICKED);
		m_NegRelativeMovement.LoadBitmaps(IDB_ARROWDOWN_SMALL, IDB_ARROWDOWN_SMALL_CLICKED);
		break;


	case Z_SERVO_TOP_MOTOR: //Loader Z-axis servo motor
		m_PosMovement.LoadBitmaps(IDB_ARROWUP, IDB_ARROWUP_CLICKED);
		m_NegMovement.LoadBitmaps(IDB_ARROWDOWN, IDB_ARROWDOWN_CLICKED);

		m_PosRelativeMovement.LoadBitmaps(IDB_ARROWUP_SMALL, IDB_ARROWUP_SMALL_CLICKED);
		m_NegRelativeMovement.LoadBitmaps(IDB_ARROWDOWN_SMALL, IDB_ARROWDOWN_SMALL_CLICKED);
		break;

	case Z_SERVO_BOTTOM_MOTOR: //Loader Z-axis servo motor
		m_PosMovement.LoadBitmaps(IDB_ARROWUP, IDB_ARROWUP_CLICKED);
		m_NegMovement.LoadBitmaps(IDB_ARROWDOWN, IDB_ARROWDOWN_CLICKED);

		m_PosRelativeMovement.LoadBitmaps(IDB_ARROWUP_SMALL, IDB_ARROWUP_SMALL_CLICKED);
		m_NegRelativeMovement.LoadBitmaps(IDB_ARROWDOWN_SMALL, IDB_ARROWDOWN_SMALL_CLICKED);
		break;

	default:
		m_PosMovement.LoadBitmaps(IDB_ARROWUP);
	}
	CWnd::Invalidate();

	if (m_nMotorIDPrev != m_nMotorID) {

		pMotionParms->setHomingSpeed(m_nMotorIDPrev, homeSpeed);
		pMotionParms->setMovingSpeed(m_nMotorIDPrev, moveSpeed);
		pMotionParms->setScanningSpeed(m_nMotorIDPrev, scanSpeed);

		pMotionParms->SetMotorHomeOriginOffset(m_nMotorIDPrev, m_nHomeORGOffset);
		pMotionParms->SetMotorMovementDirection(m_nMotorIDPrev, m_bForwardMove);
		pMotionParms->SetMotorHomeDirection(m_nMotorIDPrev, m_bForwardHome);

		pMotionParms->SetMotorSettlingTime(m_nMotorIDPrev, m_nMotorSettlingTime);

		m_nMotorIDPrev = m_nMotorID;
	}

	double dMotorResolution = 0;

	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);

	CString text;
	text.Format("Resolution: %.3f", dMotorResolution);
	m_ctrlMotorResolution.SetWindowText(text);

	homeSpeed = pMotionParms->getHomingSpeed(m_nMotorID);


	m_dHomeStartVelMM = Round(10 * homeSpeed.startSpeed / dMotorResolution) / 10.;
	m_dHomeMaxVelMM = Round(10 * homeSpeed.maximumSpeed / dMotorResolution) / 10.;
	m_dHomeStartVelKM = Round(m_dHomeStartVelMM * 36 / 100) / 10.;
	m_dHomeMaxVelKM = Round(m_dHomeMaxVelMM * 36 / 100) / 10.;


	moveSpeed = pMotionParms->getMovingSpeed(m_nMotorID);

	m_dMoveStartVelMM = Round(10 * moveSpeed.startSpeed / dMotorResolution) / 10.;
	m_dMoveMaxVelMM = Round(10 * moveSpeed.maximumSpeed / dMotorResolution) / 10.;
	m_dMoveStartVelKM = Round(m_dMoveStartVelMM * 36 / 100) / 10.;
	m_dMoveMaxVelKM = Round(m_dMoveMaxVelMM * 36 / 100) / 10.;


	scanSpeed = pMotionParms->getScanningSpeed(m_nMotorID);

	m_dScanStartVelMM = Round(10 * scanSpeed.startSpeed / dMotorResolution) / 10.;
	m_dScanMaxVelMM = Round(10 * scanSpeed.maximumSpeed / dMotorResolution) / 10.;
	m_dScanStartVelKM = Round(m_dScanStartVelMM * 36 / 100) / 10.;
	m_dScanMaxVelKM = Round(m_dScanMaxVelMM * 36 / 100) / 10.;

	pMotionParms->GetMotorHomeOriginOffset(m_nMotorID, m_nHomeORGOffset);
	pMotionParms->GetMotorMovementDirection(m_nMotorID, m_bForwardMove);
	pMotionParms->GetMotorHomeDirection(m_nMotorID, m_bForwardHome);
	pMotionParms->GetMotorSettlingTime(m_nMotorID, m_nMotorSettlingTime);

	m_nMotorMovePos = Round(m_dMotorMovePosMM * dMotorResolution);
	m_nMotorMoveRelativePos = Round(m_dMotorMoveRelativePosMM * dMotorResolution);

	m_bMotorOff = !pMotionParmsInitial->IsMotorActive(m_nMotorID);

	nMotionCommand = -1;
	MotorStopped();

	UpdateData(FALSE);
}

void CMotionControlDialog::OnBnClickedMotorOff()
{
	_resetValidation();

	m_bMotorOff = !m_bMotorOff;

	if (m_bMotorOff)
		pMotionParmsInitial->SetServoOff(m_nMotorID);
	else
		pMotionParmsInitial->SetServoOn(m_nMotorID);

	bMotorServoStatus = pMotionParmsInitial->IsMotorActive(m_nMotorID);
	m_bMotorOff = !bMotorServoStatus;

	UpdateData(FALSE);
}

void CMotionControlDialog::EnableWindows(const BOOL bEnable)
{
	GetDlgItem(IDC_HOME)->EnableWindow(bEnable);
	GetDlgItem(IDC_MOVE_TO_MOTOR_POS)->EnableWindow(bEnable);
	GetDlgItem(IDC_RELATIVE_MOVE)->EnableWindow(bEnable);
	GetDlgItem(IDC_MOTOR_MOVE_POSITION)->EnableWindow(bEnable);
	GetDlgItem(IDC_MOTOR_RELATIVE_MOVE_POSITION)->EnableWindow(bEnable);
	GetDlgItem(IDC_HOME_ALL)->EnableWindow(bEnable);
	GetDlgItem(IDC_MOTOR_ID_COMBO)->EnableWindow(bEnable);
	GetDlgItem(IDC_MOTOR_MOVE_POSITION_MM)->EnableWindow(bEnable);
	GetDlgItem(IDC_MOTOR_RELATIVE_MOVE_POSITION_MM)->EnableWindow(bEnable);
}

void CMotionControlDialog::ResetValues()
{
	if (nTotalMotorNo > 0 /*&& nTotalScaleNo > 0*/) {

		std::fill(previousMotorPos.begin(), previousMotorPos.end(), 0);
		std::fill(previousMotorHomed.begin(), previousMotorHomed.end(), FALSE);
		//		std::fill(previousScalePos.begin(), previousScalePos.end(), 0);
	}
}

void CMotionControlDialog::Move(int nAxis, int nDirection)
{
	if (!m_bContinuousMove) {
		// 1. Stop current movement
		// 2. Move

		UpdateParameters();
		int nError = pMotionParmsInitial->StopAxis(nAxis);
		if (nError) return;

		MotorStopped();
		Sleep(50);

		nError = pMotionParmsInitial->ContinuousMove(nAxis, nDirection == 1 ? TRUE : FALSE, pMotionParmsInitial->getMovingSpeed(nAxis), true, false);
		if (nError) return;

		m_bContinuousMove = TRUE;
		EnableWindows(FALSE);

		//		GetDlgItem(IDC_LINEAR_ENCODER_VARIATION_VERIFICATION)->EnableWindow(TRUE);
	}
	else {
		// 1. Stop current movement
		int nError = pMotionParmsInitial->StopAxis(nAxis);
		if (nError) return;

		MotorStopped();
		Sleep(50);

		m_bContinuousMove = FALSE;
		EnableWindows(TRUE);

		//		GetDlgItem(IDC_LINEAR_ENCODER_VARIATION_VERIFICATION)->EnableWindow(FALSE);
	}
}

void CMotionControlDialog::UpdateParameters()
{
	UpdateData(TRUE);

	if (homeSpeed.startSpeed  < 1)
		homeSpeed.startSpeed = 1;

	if (homeSpeed.maximumSpeed < 1)
		homeSpeed.maximumSpeed = 1;

	if (moveSpeed.startSpeed   < 1)
		moveSpeed.startSpeed = 1;

	if (moveSpeed.maximumSpeed < 1)
		moveSpeed.maximumSpeed = 1;

	if (scanSpeed.startSpeed < 1)
		scanSpeed.startSpeed = 1;
	if (scanSpeed.maximumSpeed < 1)
		scanSpeed.maximumSpeed = 1;

	if (homeSpeed.startSpeed > homeSpeed.maximumSpeed)
		homeSpeed.startSpeed = homeSpeed.maximumSpeed;

	if (moveSpeed.startSpeed > moveSpeed.maximumSpeed)
		moveSpeed.startSpeed = moveSpeed.maximumSpeed;

	if (scanSpeed.startSpeed > scanSpeed.maximumSpeed)
		scanSpeed.startSpeed = scanSpeed.maximumSpeed;

	double dMotorResolution = 0;

	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);

	m_dHomeStartVelMM = Round(10 * homeSpeed.startSpeed / dMotorResolution) / 10.0;
	m_dHomeStartVelKM = Round(m_dHomeStartVelMM * 36 / 100) / 10.;

	m_dHomeMaxVelMM = Round(10 * homeSpeed.maximumSpeed / dMotorResolution) / 10.;
	m_dHomeMaxVelKM = Round(m_dHomeMaxVelMM * 36 / 100) / 10.;

	m_dMoveStartVelMM = Round(10 * moveSpeed.startSpeed / dMotorResolution) / 10.;
	m_dMoveStartVelKM = Round(m_dMoveStartVelMM * 36 / 100) / 10.;

	m_dMoveMaxVelMM = Round(10 * moveSpeed.maximumSpeed / dMotorResolution) / 10.;
	m_dMoveMaxVelKM = Round(m_dMoveMaxVelMM * 36 / 100) / 10.;

	m_dScanStartVelMM = Round(10 * scanSpeed.startSpeed / dMotorResolution) / 10.;
	m_dScanStartVelKM = Round(m_dScanStartVelMM * 36 / 100) / 10.;

	m_dScanMaxVelMM = Round(10 * scanSpeed.maximumSpeed / dMotorResolution) / 10.;
	m_dScanMaxVelKM = Round(m_dScanMaxVelMM * 36 / 100) / 10.;

	pMotionParms->setHomingSpeed(m_nMotorID, homeSpeed);
	pMotionParms->setMovingSpeed(m_nMotorID, moveSpeed);
	pMotionParms->setScanningSpeed(m_nMotorID, scanSpeed);

	pMotionParms->SetMotorHomeOriginOffset(m_nMotorID, m_nHomeORGOffset);
	pMotionParms->SetMotorMovementDirection(m_nMotorID, m_bForwardMove);
	pMotionParms->SetMotorHomeDirection(m_nMotorID, m_bForwardHome);

	pMotionParms->SetMotorSettlingTime(m_nMotorID, m_nMotorSettlingTime);
	//	pMotionParms->SetMotorHomeCheckByPass(m_bHomeCheckByPass);

	UpdateData(FALSE);
}

void CMotionControlDialog::StopEveryMotor()
{
	pMotionParmsInitial->StopEveryAxis();

	MotorStopped();

	EnableWindows(TRUE);
}

void CMotionControlDialog::OnBnClickedStopMotors()
{
	StopEveryMotor();
}


void CMotionControlDialog::OnBnClickedHomeAll()
{
	UpdateParameters();

	StopEveryMotor();

	pMotionParmsInitial->HomeMoveEveryAxis();

	nMotionCommand = 1;
}

void CMotionControlDialog::OnBnClickedApplyParameters()
{
	UpdateParameters();

	pMotionParms->SaveMotionParm();

	pMotionParms->SaveScanningParm(false);

	pMotionParmsInitial->LoadMotionParm();
	pMotionParmsInitial->LoadScanningParm();

	pMotionParmsInitial->SetMotionMovementParameters(*pMotionParms);

	//*pMotionParmsInitial = *pMotionParms;

	//pMotionParmsInitial->SaveMotionParm();
	//pMotionParmsInitial->SaveScanningParm();
}

void CMotionControlDialog::MotorStopped()
{
	_resetValidation();
	nMotionCommand = -1;
}

void CMotionControlDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == MOTION_TIMER && bRunThread) {

		if (!bActiveTimer) {

			bActiveTimer = true;

			RedrawMotorPosStatus();

			/*if(moveMotorToNextVerificationPosition) {

			moveVerificationMotor();

			linearEncoderPositionCounter = 0;
			verifyLinearEncoderPosition = true;
			displayEncoderVariation = false;

			moveMotorToNextVerificationPosition = false;
			}*/

			if (nMotionCommand > -1) {

				BOOL bMotionDone = FALSE;
				int nError;

				if (nMotionCommand == 0) {

					nError = pMotionParmsInitial->IsMotionDone(m_nMotorID, bMotionDone);

					if (!nError && bMotionDone) {
						pMotionParmsInitial->HomeDone(m_nMotorID);

						OutputDebugLogTo(0, TRUE, "Motor [%d] has done homing", m_nMotorID);

						MotorStopped();
					}

				}


				if (nMotionCommand == 1) {
					BOOL bGoodMove = TRUE;
					int i;

					for (i = 0; i<nTotalMotorNo; i++) {
						if (!pMotionParmsInitial->IsMotorHomed(i)) {

							nError = pMotionParmsInitial->IsMotionDone(i, bMotionDone);

							if (!nError && bMotionDone) {
								pMotionParmsInitial->HomeDone(i);

								OutputDebugLogTo(0, TRUE, "Motor [%d] has done homing", i);
							}

						}
					}

					for (i = 0; i<nTotalMotorNo; i++) {
						if (!pMotionParmsInitial->IsMotorHomed(i)) {
							bGoodMove = FALSE;
							break;
						}
					}

					if (bGoodMove) {
						OutputDebugLogTo(0, TRUE, "All motors have done homing");

						MotorStopped();
					}

				}

			}

			bActiveTimer = false;

		}

	}

	CDialog::OnTimer(nIDEvent);
}

void CMotionControlDialog::OnBnClickedSetupScan()
{

	if (pScanningTeachDialog == NULL) {

		pScanningTeachDialog = new ScanningTeachDialog();

		if (!pScanningTeachDialog->Create(IDD_SCANNING_TEACH_DIALOG, AfxGetMainWnd())) {
			delete pScanningTeachDialog;
			pScanningTeachDialog = NULL;
			return;
		}
	}

	if (pScanningTeachDialog != NULL) {
		pScanningTeachDialog->SetMotionParameters(pMotionParms, pMotionParmsInitial);
		pScanningTeachDialog->ShowWindow(TRUE);
	}
}


void CMotionControlDialog::OnBnClickedRelativeMove()
{
	// 1. Stop current movement
	// 2. Move

	UpdateParameters();
	MotorStopped();
	Sleep(50);
	int nError = pMotionParmsInitial->StopAxis(m_nMotorID);
	if (nError) return;

	nError = pMotionParmsInitial->RelativeMove(m_nMotorID, -m_nMotorMoveRelativePos, pMotionParmsInitial->getMovingSpeed(m_nMotorID), true, false);
	if (nError) return;
}


void CMotionControlDialog::OnBnClickedRelativeMoveNegative()
{
	// 1. Stop current movement
	// 2. Move

	UpdateParameters();
	MotorStopped();
	Sleep(50);
	int nError = pMotionParmsInitial->StopAxis(m_nMotorID);
	if (nError) return;

	nError = pMotionParmsInitial->RelativeMove(m_nMotorID, m_nMotorMoveRelativePos, pMotionParmsInitial->getMovingSpeed(m_nMotorID), true, false);
	if (nError) return;
}

void CMotionControlDialog::OnEnChangeHomeStartVel()
{
	CString strTemp;
	double dMotorResolution = 0;

	bEditBoxUpdate[0] = TRUE;
	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);
	GetDlgItem(IDC_HOME_START_VEL)->GetWindowTextA(strTemp);

	if (!strTemp.IsEmpty()) {
		if (bEditBoxUpdate[1] != TRUE) {
			homeSpeed.startSpeed = Round(atof(strTemp));
			m_dHomeStartVelMM = Round(10 * homeSpeed.startSpeed / dMotorResolution) / 10.0;
			strTemp.Format("%.3f", m_dHomeStartVelMM);
			GetDlgItem(IDC_HOME_START_VEL_MM)->SetWindowTextA(strTemp);
		}

		bEditBoxUpdate[0] = FALSE;
	}
}


void CMotionControlDialog::OnEnChangeHomeMaxVel()
{
	CString strTemp;
	double dMotorResolution = 0;

	bEditBoxUpdate[0] = TRUE;
	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);
	GetDlgItem(IDC_HOME_MAX_VEL)->GetWindowTextA(strTemp);

	if (!strTemp.IsEmpty()) {
		if (bEditBoxUpdate[1] != TRUE) {
			homeSpeed.maximumSpeed = Round(atof(strTemp));
			m_dHomeMaxVelMM = Round(10 * homeSpeed.maximumSpeed / dMotorResolution) / 10.;
			strTemp.Format("%.3f", m_dHomeMaxVelMM);
			GetDlgItem(IDC_HOME_MAX_VEL_MM)->SetWindowTextA(strTemp);
		}

		bEditBoxUpdate[0] = FALSE;
	}
}

void CMotionControlDialog::OnEnChangeMoveStartVel()
{
	CString strTemp;
	double dMotorResolution = 0;

	bEditBoxUpdate[0] = TRUE;
	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);
	GetDlgItem(IDC_MOVE_START_VEL)->GetWindowTextA(strTemp);

	if (!strTemp.IsEmpty()) {
		if (bEditBoxUpdate[1] != TRUE) {
			moveSpeed.startSpeed = Round(atof(strTemp));
			m_dMoveStartVelMM = Round(10 * moveSpeed.startSpeed / dMotorResolution) / 10.;
			strTemp.Format("%.3f", m_dMoveStartVelMM);
			GetDlgItem(IDC_MOVE_START_VEL_MM)->SetWindowTextA(strTemp);
		}

		bEditBoxUpdate[0] = FALSE;
	}
}

void CMotionControlDialog::OnEnChangeScanStartVel()
{
	CString strTemp;
	double dMotorResolution = 0;

	bEditBoxUpdate[0] = TRUE;
	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);
	GetDlgItem(IDC_SCAN_START_VEL)->GetWindowTextA(strTemp);

	if (!strTemp.IsEmpty()) {
		if (bEditBoxUpdate[1] != TRUE) {
			scanSpeed.startSpeed = Round(atof(strTemp));
			m_dScanStartVelMM = Round(10 * scanSpeed.startSpeed / dMotorResolution) / 10.;
			strTemp.Format("%.3f", m_dScanStartVelMM);
			GetDlgItem(IDC_SCAN_START_VEL_MM)->SetWindowTextA(strTemp);
		}

		bEditBoxUpdate[0] = FALSE;
	}
}

//void CMotionControlDialog::OnEnChangeScanStartVelTHR()
//{
//	CString strTemp;
//	double dMotorResolution = 0;
//
//	bEditBoxUpdate[0] = TRUE;
//	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);
//	GetDlgItem(IDC_SCAN_START_VEL_THR)->GetWindowTextA(strTemp);
//
//	if (!strTemp.IsEmpty()) {
//		if (bEditBoxUpdate[1] != TRUE) {
//			scanSpeedTHR.startSpeed = Round(atof(strTemp));
//			m_dScanStartVelMMTHR = Round(10 * scanSpeedTHR.startSpeed / dMotorResolution) / 10.;
//			strTemp.Format("%.3f", m_dScanStartVelMMTHR);
//			GetDlgItem(IDC_SCAN_START_VEL_MM_THR)->SetWindowTextA(strTemp);
//		}
//
//		bEditBoxUpdate[0] = FALSE;
//	}
//}

void CMotionControlDialog::OnEnChangeMoveMaxVel()
{
	CString strTemp;
	double dMotorResolution = 0;

	bEditBoxUpdate[0] = TRUE;
	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);
	GetDlgItem(IDC_MOVE_MAX_VEL)->GetWindowTextA(strTemp);

	if (!strTemp.IsEmpty()) {
		if (bEditBoxUpdate[1] != TRUE) {
			moveSpeed.maximumSpeed = Round(atof(strTemp));
			m_dMoveMaxVelMM = Round(10 * moveSpeed.maximumSpeed / dMotorResolution) / 10.;
			strTemp.Format("%.3f", m_dMoveMaxVelMM);
			GetDlgItem(IDC_MOVE_MAX_VEL_MM)->SetWindowTextA(strTemp);
		}

		bEditBoxUpdate[0] = FALSE;
	}
}

void CMotionControlDialog::OnEnChangeScanMaxVel()
{
	CString strTemp;
	double dMotorResolution = 0;

	bEditBoxUpdate[0] = TRUE;
	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);
	GetDlgItem(IDC_SCAN_MAX_VEL)->GetWindowTextA(strTemp);

	if (!strTemp.IsEmpty()) {
		if (bEditBoxUpdate[1] != TRUE) {
			scanSpeed.maximumSpeed = Round(atof(strTemp));
			m_dScanMaxVelMM = Round(10 * scanSpeed.maximumSpeed / dMotorResolution) / 10.;
			strTemp.Format("%.3f", m_dScanMaxVelMM);
			GetDlgItem(IDC_SCAN_MAX_VEL_MM)->SetWindowTextA(strTemp);
		}

		bEditBoxUpdate[0] = FALSE;
	}
}

//void CMotionControlDialog::OnEnChangeScanMaxVelTHR()
//{
//	CString strTemp;
//	double dMotorResolution = 0;
//
//	bEditBoxUpdate[0] = TRUE;
//	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);
//	GetDlgItem(IDC_SCAN_MAX_VEL_THR)->GetWindowTextA(strTemp);
//
//	if (!strTemp.IsEmpty()) {
//		if (bEditBoxUpdate[1] != TRUE) {
//			scanSpeedTHR.maximumSpeed = Round(atof(strTemp));
//			m_dScanMaxVelMMTHR = Round(10 * scanSpeedTHR.maximumSpeed / dMotorResolution) / 10.;
//			strTemp.Format("%.3f", m_dScanMaxVelMMTHR);
//			GetDlgItem(IDC_SCAN_MAX_VEL_MM_THR)->SetWindowTextA(strTemp);
//		}
//
//		bEditBoxUpdate[0] = FALSE;
//	}
//}

void CMotionControlDialog::OnEnChangeHomeStartVelMm()
{
	CString strTemp;
	double dMotorResolution = 0;

	bEditBoxUpdate[1] = TRUE;
	GetDlgItem(IDC_HOME_START_VEL_MM)->GetWindowTextA(strTemp);
	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);

	if (!strTemp.IsEmpty()) {
		if (bEditBoxUpdate[0] != TRUE) {

			m_dHomeStartVelMM = atof(strTemp);
			homeSpeed.startSpeed = Round(m_dHomeStartVelMM * dMotorResolution);
			strTemp.Format("%d", homeSpeed.startSpeed);
			GetDlgItem(IDC_HOME_START_VEL)->SetWindowTextA(strTemp);
		}

		bEditBoxUpdate[1] = FALSE;
	}
}

void CMotionControlDialog::OnEnChangeHomeMaxVelMm()
{
	CString strTemp;
	double dMotorResolution = 0;

	bEditBoxUpdate[1] = TRUE;
	GetDlgItem(IDC_HOME_MAX_VEL_MM)->GetWindowTextA(strTemp);
	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);

	if (!strTemp.IsEmpty()) {
		if (bEditBoxUpdate[0] != TRUE) {

			m_dHomeMaxVelMM = atof(strTemp);
			homeSpeed.maximumSpeed = Round(m_dHomeMaxVelMM * dMotorResolution);
			strTemp.Format("%d", homeSpeed.maximumSpeed);
			GetDlgItem(IDC_HOME_MAX_VEL)->SetWindowTextA(strTemp);
		}

		bEditBoxUpdate[1] = FALSE;
	}

}

void CMotionControlDialog::OnEnChangeMoveStartVelMm()
{
	CString strTemp;
	double dMotorResolution = 0;

	bEditBoxUpdate[1] = TRUE;
	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);
	GetDlgItem(IDC_MOVE_START_VEL_MM)->GetWindowTextA(strTemp);

	if (!strTemp.IsEmpty()) {
		if (bEditBoxUpdate[0] != TRUE) {

			m_dMoveStartVelMM = atof(strTemp);
			moveSpeed.startSpeed = Round(m_dMoveStartVelMM * dMotorResolution);
			strTemp.Format("%d", moveSpeed.startSpeed);
			GetDlgItem(IDC_MOVE_START_VEL)->SetWindowTextA(strTemp);
		}

		bEditBoxUpdate[1] = FALSE;
	}
}

void CMotionControlDialog::OnEnChangeScanStartVelMm()
{
	CString strTemp;
	double dMotorResolution = 0;

	bEditBoxUpdate[1] = TRUE;
	GetDlgItem(IDC_SCAN_START_VEL_MM)->GetWindowTextA(strTemp);
	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);

	if (!strTemp.IsEmpty()) {
		if (bEditBoxUpdate[0] != TRUE) {

			m_dScanStartVelMM = atof(strTemp);
			scanSpeed.startSpeed = Round(m_dScanStartVelMM * dMotorResolution);
			strTemp.Format("%d", scanSpeed.startSpeed);
			GetDlgItem(IDC_SCAN_START_VEL)->SetWindowTextA(strTemp);
		}

		bEditBoxUpdate[1] = FALSE;
	}
}

//void CMotionControlDialog::OnEnChangeScanStartVelMmTHR()
//{
//	CString strTemp;
//	double dMotorResolution = 0;
//
//	bEditBoxUpdate[1] = TRUE;
//	GetDlgItem(IDC_SCAN_START_VEL_MM_THR)->GetWindowTextA(strTemp);
//	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);
//
//	if (!strTemp.IsEmpty()) {
//		if (bEditBoxUpdate[0] != TRUE) {
//
//			m_dScanStartVelMMTHR = atof(strTemp);
//			scanSpeedTHR.startSpeed = Round(m_dScanStartVelMMTHR * dMotorResolution);
//			strTemp.Format("%d", scanSpeedTHR.startSpeed);
//			GetDlgItem(IDC_SCAN_START_VEL_THR)->SetWindowTextA(strTemp);
//		}
//
//		bEditBoxUpdate[1] = FALSE;
//	}
//}


void CMotionControlDialog::OnEnChangeMoveMaxVelMm()
{
	CString strTemp;
	double dMotorResolution = 0;

	bEditBoxUpdate[1] = TRUE;
	GetDlgItem(IDC_MOVE_MAX_VEL_MM)->GetWindowTextA(strTemp);
	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);

	if (!strTemp.IsEmpty()) {
		if (bEditBoxUpdate[0] != TRUE) {

			m_dMoveMaxVelMM = atof(strTemp);
			moveSpeed.maximumSpeed = Round(m_dMoveMaxVelMM * dMotorResolution);
			strTemp.Format("%d", moveSpeed.maximumSpeed);
			GetDlgItem(IDC_MOVE_MAX_VEL)->SetWindowTextA(strTemp);
		}

		bEditBoxUpdate[1] = FALSE;
	}
}

void CMotionControlDialog::OnEnChangeScanMaxVelMm()
{
	CString strTemp;
	double dMotorResolution = 0;

	bEditBoxUpdate[1] = TRUE;
	GetDlgItem(IDC_SCAN_MAX_VEL_MM)->GetWindowTextA(strTemp);
	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);

	if (!strTemp.IsEmpty()) {
		if (bEditBoxUpdate[0] != TRUE) {

			m_dScanMaxVelMM = atof(strTemp);
			scanSpeed.maximumSpeed = Round(m_dScanMaxVelMM * dMotorResolution);
			strTemp.Format("%d", scanSpeed.maximumSpeed);
			GetDlgItem(IDC_SCAN_MAX_VEL)->SetWindowTextA(strTemp);
		}

		bEditBoxUpdate[1] = FALSE;
	}
}

//void CMotionControlDialog::OnEnChangeScanMaxVelMmTHR()
//{
//	CString strTemp;
//	double dMotorResolution = 0;
//
//	bEditBoxUpdate[1] = TRUE;
//	GetDlgItem(IDC_SCAN_MAX_VEL_MM_THR)->GetWindowTextA(strTemp);
//	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);
//
//	if (!strTemp.IsEmpty()) {
//		if (bEditBoxUpdate[0] != TRUE) {
//
//			m_dScanMaxVelMMTHR = atof(strTemp);
//			scanSpeedTHR.maximumSpeed = Round(m_dScanMaxVelMMTHR * dMotorResolution);
//			strTemp.Format("%d", scanSpeedTHR.maximumSpeed);
//			GetDlgItem(IDC_SCAN_MAX_VEL_THR)->SetWindowTextA(strTemp);
//		}
//
//		bEditBoxUpdate[1] = FALSE;
//	}
//}

void CMotionControlDialog::moveVerificationMotor()
{
	if (moveXMotorForVerification) {
		int error = pMotionParmsInitial->StopAxis(X_SERVO_MOTOR);
		if (error)
			return;

		double resolution = 1;

		error = pMotionParms->GetMotorResolution(X_SERVO_MOTOR, resolution);
		if (error)
			return;

		error = pMotionParmsInitial->RelativeMove(X_SERVO_MOTOR, Round(_verificationMotorStepMm * resolution), pMotionParmsInitial->getMovingSpeed(X_SERVO_MOTOR), true);
		if (error)
			return;

		error = pMotionParmsInitial->WaitForMotionDone(X_SERVO_MOTOR, 10000);
		if (error)
			return;

		pMotionParmsInitial->WaitMotorSettlingTime(X_SERVO_MOTOR);
	}
	else {
		int error = pMotionParmsInitial->StopAxis(Y_SERVO_MOTOR);
		if (error)
			return;

		double resolution = 1;

		error = pMotionParms->GetMotorResolution(Y_SERVO_MOTOR, resolution);
		if (error)
			return;

		error = pMotionParmsInitial->RelativeMove(Y_SERVO_MOTOR, Round(0.01 * resolution), pMotionParmsInitial->getMovingSpeed(Y_SERVO_MOTOR), true, false);
		if (error)
			return;

		error = pMotionParmsInitial->WaitForMotionDone(Y_SERVO_MOTOR, 10000);
		if (error)
			return;

		pMotionParmsInitial->WaitMotorSettlingTime(Y_SERVO_MOTOR);
	}


	Sleep(2000);
}

void CMotionControlDialog::_resetValidation()
{
	//	verifyLinearEncoderPosition = false;
	//	verifyEncoderVariation = false;
	//	displayEncoderVariation = false;
	moveMotorForVerification = false;
	moveMotorToNextVerificationPosition = false;
	moveXMotorForVerification = false;
}

void CMotionControlDialog::OnClose()
{
	_resetValidation();

	if (bChecked != NULL)
		*bChecked = FALSE;

	if (bRunThread) {
		bRunThread = false;

		KillTimer(MOTION_TIMER);
	}

	if (pScanningTeachDialog != NULL) {
		pScanningTeachDialog->DestroyWindow();
		delete pScanningTeachDialog;
		pScanningTeachDialog = NULL;
	}

	ShowWindow(SW_HIDE);
}

void CMotionControlDialog::OnCancel()
{
	_resetValidation();

	if (bChecked != NULL)
		*bChecked = FALSE;

	if (bRunThread) {
		bRunThread = false;

		KillTimer(MOTION_TIMER);
	}

	if (pScanningTeachDialog != NULL) {
		pScanningTeachDialog->DestroyWindow();
		delete pScanningTeachDialog;
		pScanningTeachDialog = NULL;
	}

	ShowWindow(SW_HIDE);
}

//void CMotionControlDialog::OnKillFocus(CWnd* pNewWnd)
//{
//	if(bChecked != NULL)
//		*bChecked = FALSE;
//
//	if(bRunThread) {
//		bRunThread = false;
//
//		KillTimer(MOTION_TIMER);
//	}
//
//	if(pScanningTeachDialog != NULL) {
//		delete pScanningTeachDialog;
//		pScanningTeachDialog = NULL;
//	}
//
//
//
//	CDialog::OnKillFocus(pNewWnd);
//}

void CMotionControlDialog::OnEnChangeSettlingTime()
{

}

void CMotionControlDialog::OnBnClickedStopMotor()
{
	MotorStopped();
	Sleep(50);

	pMotionParmsInitial->StopAxis(m_nMotorID);

	EnableWindows(TRUE);
}

void CMotionControlDialog::OnBnClickedReloadSettings()
{
	StopEveryMotor();
	//	pMotionParms->LoadMotionParm();

	EnableWindows(TRUE);
}

void CMotionControlDialog::OnBnClickedResetAlarm()
{
	pMotionParmsInitial->ResetAlarm(m_nMotorID);
}

void CMotionControlDialog::OnEnChangeMotorMovePosition()
{
	CString strTemp;
	double dMotorResolution = 0;

	bEditBoxUpdate[0] = TRUE;
	GetDlgItem(IDC_MOTOR_MOVE_POSITION)->GetWindowTextA(strTemp);
	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);

	if (!strTemp.IsEmpty()) {
		if (bEditBoxUpdate[1] != TRUE) {

			m_nMotorMovePos = Round(atof(strTemp));
			m_dMotorMovePosMM = m_nMotorMovePos / dMotorResolution;
			strTemp.Format("%.3f", m_dMotorMovePosMM);
			GetDlgItem(IDC_MOTOR_MOVE_POSITION_MM)->SetWindowTextA(strTemp);
		}

		bEditBoxUpdate[0] = FALSE;
	}
}

void CMotionControlDialog::OnEnChangeMotorRelativeMovePosition()
{
	CString strTemp;
	double dMotorResolution = 0;

	bEditBoxUpdate[0] = TRUE;
	GetDlgItem(IDC_MOTOR_RELATIVE_MOVE_POSITION)->GetWindowTextA(strTemp);
	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);

	if (!strTemp.IsEmpty()) {
		if (bEditBoxUpdate[1] != TRUE) {

			m_nMotorMoveRelativePos = Round(atof(strTemp));
			m_dMotorMoveRelativePosMM = m_nMotorMoveRelativePos / dMotorResolution;
			strTemp.Format("%.3f", m_dMotorMoveRelativePosMM);
			GetDlgItem(IDC_MOTOR_RELATIVE_MOVE_POSITION_MM)->SetWindowTextA(strTemp);
		}

		bEditBoxUpdate[0] = FALSE;
	}
}

void CMotionControlDialog::OnEnChangeMotorMovePositionMm()
{
	CString strTemp;
	double dMotorResolution = 0;

	bEditBoxUpdate[1] = TRUE;
	GetDlgItem(IDC_MOTOR_MOVE_POSITION_MM)->GetWindowTextA(strTemp);
	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);

	if (!strTemp.IsEmpty()) {
		if (bEditBoxUpdate[0] != TRUE) {

			m_dMotorMovePosMM = atof(strTemp);
			m_nMotorMovePos = Round(m_dMotorMovePosMM * dMotorResolution);
			strTemp.Format("%d", m_nMotorMovePos);
			GetDlgItem(IDC_MOTOR_MOVE_POSITION)->SetWindowTextA(strTemp);
		}

		bEditBoxUpdate[1] = FALSE;
	}
}

void CMotionControlDialog::OnEnChangeMotorRelativeMovePositionMm()
{
	CString strTemp;
	double dMotorResolution = 0;

	bEditBoxUpdate[1] = TRUE;
	GetDlgItem(IDC_MOTOR_RELATIVE_MOVE_POSITION_MM)->GetWindowTextA(strTemp);
	pMotionParms->GetMotorResolution(m_nMotorID, dMotorResolution);

	if (!strTemp.IsEmpty()) {
		if (bEditBoxUpdate[0] != TRUE) {

			m_dMotorMoveRelativePosMM = atof(strTemp);
			m_nMotorMoveRelativePos = Round(m_dMotorMoveRelativePosMM * dMotorResolution);
			strTemp.Format("%d", m_nMotorMoveRelativePos);
			GetDlgItem(IDC_MOTOR_RELATIVE_MOVE_POSITION)->SetWindowTextA(strTemp);
		}

		bEditBoxUpdate[1] = FALSE;
	}
}



void CMotionControlDialog::OnCustomdrawMotorPosList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = CDRF_DODEFAULT;

	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage) {
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage) {
		// This is the notification message for an item.  We'll request
		// notifications before each subitem's prepaint stage.

		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage) {

		if (pLVCD->iSubItem > 3) {

			if (motionIoStatus[pLVCD->nmcd.dwItemSpec][pLVCD->iSubItem - 4])
				pLVCD->clrTextBk = motionIOGood[pLVCD->iSubItem - 4] ? clListControlGreen : clListControlRed;
			else
				pLVCD->clrTextBk = clListControlWhite;
		}


		// Tell Windows to paint the control itself.
		*pResult = CDRF_DODEFAULT;
	}


}

// CameraSetupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "CameraSetupDialog.h"
#include "afxdialogex.h"
#include "TrackHandler.h"


// CCameraSetupDialog dialog

IMPLEMENT_DYNAMIC(CCameraSetupDialog, CDialogEx)

CCameraSetupDialog::CCameraSetupDialog(CTrackHandler* pTrack) : CDialogEx(CCameraSetupDialog::IDD, NULL)
{
	m_pTrack = pTrack;
	m_nExposure = 0;
	m_nGains[0] = 0;
	m_nGains[1] = 0;
	m_nGains[2] = 0;
	m_nGain = 0;
	m_nBlackLevel = 0;
	m_nGainMode = 0;
}

CCameraSetupDialog::~CCameraSetupDialog()
{
}

void CCameraSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CAMERASETUP_EXPOSURE_EDIT, m_nExposure);
	DDV_MinMaxInt(pDX, m_nExposure, 0, m_pTrack->m_nMaxExposure);
	DDX_Text(pDX, IDC_CAMERASETUP_GAIN_EDIT, m_nGain);
	DDV_MinMaxInt(pDX, m_nGain, 0, 500);
	DDX_Text(pDX, IDC_CAMERASETUP_BLACKLEVEL_EDIT, m_nBlackLevel);
	DDV_MinMaxInt(pDX, m_nBlackLevel, 0, 600);
	DDX_Control(pDX, IDC_CAMERASETUP_EXPOSURE_SLIDER, m_sliderExposure);
	DDX_Control(pDX, IDC_CAMERASETUP_GAIN_SLIDER, m_sliderGain);
	DDX_Control(pDX, IDC_CAMERASETUP_BLACKLEVEL_SLIDER, m_sliderBlackLevel);
	DDX_Radio(pDX, IDC_CAMERASETUP_GAINALL_RADIO, m_nGainMode);
}


BEGIN_MESSAGE_MAP(CCameraSetupDialog, CDialogEx)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_CAMERASETUP_EXPOSURE_EDIT, OnEnChangeExposureEdit)
	ON_EN_CHANGE(IDC_CAMERASETUP_GAIN_EDIT, OnEnChangeGainEdit)
	ON_EN_CHANGE(IDC_CAMERASETUP_BLACKLEVEL_EDIT, OnEnChangeBlackLevelEdit)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedOk)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CAMERASETUP_GAINALL_RADIO, OnBnClickedCamerasetupGainRadio)
	ON_BN_CLICKED(IDC_CAMERASETUP_GAINTAP1_RADIO, OnBnClickedCamerasetupGainRadio)
	ON_BN_CLICKED(IDC_CAMERASETUP_GAINTAP2_RADIO, OnBnClickedCamerasetupGainRadio)
END_MESSAGE_MAP()


// CCameraSetupDialog message handlers

void CCameraSetupDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int nVal = ((CSliderCtrl*)pScrollBar)->GetPos();
	if(pScrollBar->m_hWnd == m_sliderExposure.m_hWnd) {
		m_nExposure = nVal;
		UpdateData(FALSE);
		OnEnChangeExposureEdit();
	}
	else if(pScrollBar->m_hWnd == m_sliderGain.m_hWnd) {
		m_nGain = nVal;
		UpdateData(FALSE);
		OnEnChangeGainEdit();
	}
	else if(pScrollBar->m_hWnd == m_sliderBlackLevel.m_hWnd) {
		m_nBlackLevel = nVal;
		UpdateData(FALSE);
		OnEnChangeBlackLevelEdit();
	}
	
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CCameraSetupDialog::OnEnChangeExposureEdit()
{
	UpdateData();
	CSliderCtrl* pSlider = (CSliderCtrl*) GetDlgItem(IDC_CAMERASETUP_EXPOSURE_SLIDER);
	if(m_nExposure < 4)
		m_nExposure = 4;
	pSlider->SetPos(m_nExposure);
	m_pTrack->SetCameraValue(CAMERA_EXPOSURE, m_nExposure);
}


void CCameraSetupDialog::OnEnChangeGainEdit()
{
	UpdateData();
	CSliderCtrl* pSlider = (CSliderCtrl*) GetDlgItem(IDC_CAMERASETUP_GAIN_SLIDER);
	pSlider->SetPos(m_nGain);
	m_nGains[m_nGainMode] = m_nGain;
	m_pTrack->SetCameraValue(CAMERA_GAIN, m_nGain, m_nGainMode);
 	CString strTemp;
	double dGainDB = log10(m_nGain / 32.0) * 20;
	strTemp.Format("Gain in dB = %3.5f", dGainDB);
	GetDlgItem(IDC_STATIC_GAIN_DB)->SetWindowTextA(strTemp);
}


void CCameraSetupDialog::OnEnChangeBlackLevelEdit()
{
	UpdateData();
	CSliderCtrl* pSlider = (CSliderCtrl*) GetDlgItem(IDC_CAMERASETUP_BLACKLEVEL_SLIDER);
	pSlider->SetPos(m_nBlackLevel);
	m_pTrack->SetCameraValue(CAMERA_BLACKLEVEL, m_nBlackLevel);
}


void CCameraSetupDialog::OnBnClickedOk()
{
	m_pTrack->m_nExposure = m_nExposure;
	for(int i=0; i<3; i++)
		m_pTrack->m_nGain[i] = m_nGains[i];
	m_pTrack->m_nBlackLevel = m_nBlackLevel;

	m_pTrack->SaveCameraParm();
	m_pTrack->m_bCamSetupDlgOpen = FALSE;

	CDialogEx::OnOK();
}


void CCameraSetupDialog::OnClose()
{
	OnBnClickedOk();

	CDialogEx::OnClose();
}


BOOL CCameraSetupDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_sliderExposure.SetRange(4, m_pTrack->m_nMaxExposure);
	m_sliderExposure.SetPageSize(10);
	m_sliderExposure.SetPos(m_nExposure);

	m_sliderGain.SetRange(0, 500);
	m_sliderGain.SetPageSize(10);
	OnBnClickedCamerasetupGainRadio();

	m_sliderBlackLevel.SetRange(0, 600);
	m_sliderBlackLevel.SetPageSize(10);
	m_sliderBlackLevel.SetPos(m_nBlackLevel);

	SetWindowText(m_strTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCameraSetupDialog::OnBnClickedCamerasetupGainRadio()
{
	UpdateData(TRUE);

	m_sliderGain.SetPos(m_nGains[m_nGainMode]);
	m_nGain = m_nGains[m_nGainMode];
	CString strTemp;
	double dGainDB = log10(m_nGain / 32.0) * 20;
	strTemp.Format("Gain in dB = %3.5f", dGainDB);
	GetDlgItem(IDC_STATIC_GAIN_DB)->SetWindowTextA(strTemp);
	UpdateData(FALSE);
}

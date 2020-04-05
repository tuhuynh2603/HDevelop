// SaveCalibRationPosition.cpp : implementation file
//

#include "stdafx.h"
#include "SaveCalibRationPosition.h"
#include "afxdialogex.h"
#include "Application.h"

// CSaveCalibRationPosition dialog

IMPLEMENT_DYNAMIC(CSaveCalibRationPosition, CDialogEx)

CSaveCalibRationPosition::CSaveCalibRationPosition(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_CALIB_MOTION_SAVE, pParent)
	, m_bSelectedDotCalibration(FALSE)
{
	for (int i = 0; i < 5; i++)
		m_bCheckedIntensity[i] = FALSE;

}

CSaveCalibRationPosition::~CSaveCalibRationPosition()
{
}

void CSaveCalibRationPosition::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_RADIO_DOT_CALIBRATION, m_bCheckedDotCalibration);
	for(int i=0;i<5;i++)
		DDX_Check(pDX, IDC_CHECK_SAVE_INTENSITY1+i, m_bCheckedIntensity[i]);

	DDX_Radio(pDX, IDC_RADIO_DOT_CALIBRATION, m_bSelectedDotCalibration);
}


BEGIN_MESSAGE_MAP(CSaveCalibRationPosition, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_DOT_CALIBRATION, &CSaveCalibRationPosition::OnBnClickedRadioDotCalibration)
	ON_BN_CLICKED(IDC_RADIO_LIGHT_CORRECTION, &CSaveCalibRationPosition::OnBnClickedRadioLightCorrection)
	ON_BN_CLICKED(IDOK, &CSaveCalibRationPosition::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSaveCalibRationPosition::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSaveCalibRationPosition message handlers


BOOL CSaveCalibRationPosition::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CApplication *pApp = (CApplication*)AfxGetApp();
	for (int i = 0; i < 5; i++)
		GetDlgItem(IDC_CHECK_SAVE_INTENSITY1 + i)->ShowWindow(FALSE);
	
	for (int i = 0; i < pApp->m_nTotalViewDocs[nTrack]; i++) {
		CString str;
		switch (nTrack) 
		{
		case 0: str = pApp->m_strTopCameraName.GetAt(i);
			break;
		case 1: str = pApp->m_strBottomCameraName.GetAt(i);
			break;
		case 2: str = pApp->m_strSideCameraName.GetAt(i);
			break;
		}
		GetDlgItem(IDC_CHECK_SAVE_INTENSITY1 + i)->SetWindowTextA(str);
		GetDlgItem(IDC_CHECK_SAVE_INTENSITY1 + i)->ShowWindow(TRUE);
		GetDlgItem(IDC_CHECK_SAVE_INTENSITY1 + i)->EnableWindow(FALSE);
	}
	GetDlgItem(IDC_STATIC_SAVE_CALIBRATION_POSITION)->EnableWindow(FALSE); 
	CheckDlgButton(IDC_RADIO_DOT_CALIBRATION,TRUE);
	// TODO:  Add extra initialization here
	OnBnClickedRadioDotCalibration();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CSaveCalibRationPosition::OnBnClickedRadioDotCalibration()
{
	// TODO: Add your control notification handler code here
	CApplication *pApp = (CApplication*)AfxGetApp();
	m_bSelectedDotCalibration = TRUE;

	for (int i = 0; i < pApp->m_nTotalViewDocs[nTrack]; i++) {
		GetDlgItem(IDC_CHECK_SAVE_INTENSITY1 + i)->EnableWindow(FALSE);
	}
	GetDlgItem(IDC_STATIC_SAVE_CALIBRATION_POSITION)->EnableWindow(FALSE);
	UpdateData();
}


void CSaveCalibRationPosition::OnBnClickedRadioLightCorrection()
{
	// TODO: Add your control notification handler code here
	CApplication *pApp = (CApplication*)AfxGetApp();
	m_bSelectedDotCalibration = FALSE;

	for (int i = 0; i < pApp->m_nTotalViewDocs[nTrack]; i++) {
		GetDlgItem(IDC_CHECK_SAVE_INTENSITY1 + i)->EnableWindow(TRUE);
	}
	CheckDlgButton(IDC_CHECK_SAVE_INTENSITY1, TRUE);
	GetDlgItem(IDC_STATIC_SAVE_CALIBRATION_POSITION)->EnableWindow(TRUE);
}


void CSaveCalibRationPosition::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	CDialogEx::OnOK();
}


void CSaveCalibRationPosition::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	
	CDialogEx::OnCancel();
}

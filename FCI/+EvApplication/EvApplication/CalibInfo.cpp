// CalibInfo.cpp : implementation file
//

#include "stdafx.h"
#include "CalibInfo.h"
#include "afxdialogex.h"
#include "Application.h"
#include "CalibrationSettings.h"
// CCalibInfo dialog

IMPLEMENT_DYNAMIC(CCalibInfo, CDialogEx)

CCalibInfo::CCalibInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCalibInfo::IDD, pParent)
{

	m_nCalibGreyValue = 0;
	m_nCalibValueY = 500;
	m_bDotCalib = FALSE;
	for (int i = 0; i < 2; i++)
		m_nCalibGreyVal[i] = 0;

	m_nMinRadiusDotTarget = 10;


	//Light Correction Step
	m_nStepXCount = 4;
	m_nStepYCount = 4;
	m_dStepXSize = 1;//mm
	m_dStepYSize = 1;
}

CCalibInfo::~CCalibInfo()
{
	m_fontHead.DeleteObject();
}

void CCalibInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CALIB_STEPX_COUNT, m_nStepXCount);
	DDX_Text(pDX, IDC_EDIT_CALIB_STEPY_COUNT, m_nStepYCount);
	DDX_Text(pDX, IDC_EDIT_CALIB_STEPX_SIZE, m_dStepXSize);
	DDX_Text(pDX, IDC_EDIT_CALIB_STEPY_SIZE, m_dStepYSize);
	DDX_Text(pDX, IDC_EDIT_CALIB_RADIUS_DOT_TARGET, m_nMinRadiusDotTarget);

	//DDV_MinMaxInt(pDX, m_nCalibGreyValue, 0, 255);
}


BEGIN_MESSAGE_MAP(CCalibInfo, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCalibInfo::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_START_POS, &CCalibInfo::OnBnClickedButtonStartPos)
END_MESSAGE_MAP()


// CCalibInfo message handlers


void CCalibInfo::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	CDialogEx::OnOK();
}


BOOL CCalibInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rectDesktop, rectWnd, rectFrame;
	//if(m_fontHead)
	m_fontHead.DeleteObject();
	m_fontHead.CreateFont((int)(rectDesktop.Width() / 1280.0 * 16), 0, 0, 0, 350, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "Segoe UI Semibold");

	// TODO:  Add extra initialization here
	CString str;
	str.Format("1)Please place %s on track", !m_bDotCalib ? "Target" : "calib plate");
	GetDlgItem(IDC_STATIC_MSG_1)->SetWindowTextA(str);
	str.Format("2)Please make sure %s covers whole FOV area", !m_bDotCalib ? "Target" : "calib plate");
	GetDlgItem(IDC_STATIC_MSG_2)->SetWindowTextA(str);
	str.Format("3)%s", !m_bDotCalib ? "Please enter step count and step size (mm) for Light Correction" : "Please enter pixel distance along X and Y axes(microns)");
	GetDlgItem(IDC_STATIC_MSG_3)->SetWindowTextA(str);
	

	GetDlgItem(IDC_STATIC_CALIBRATION_NAME)->SetFont(&m_fontHead);
	GetDlgItem(IDC_STATIC_CALIBRATION_NAME)->SetWindowText(strCalibrationName);

	GetDlgItem(IDC_EDIT_CALIB_STEPX_SIZE)->ShowWindow(!m_bDotCalib);
	GetDlgItem(IDC_EDIT_CALIB_STEPY_SIZE)->ShowWindow(!m_bDotCalib);
	GetDlgItem(IDC_STATIC_STEPX)->ShowWindow(!m_bDotCalib);
	GetDlgItem(IDC_STATIC_STEPY)->ShowWindow(!m_bDotCalib);

	//Minimum radius of dot target in pixel
	GetDlgItem(IDC_EDIT_CALIB_RADIUS_DOT_TARGET)->ShowWindow(m_bDotCalib);
	if (m_bDotCalib) {
		GetDlgItem(IDC_STATIC_MSG_4)->SetWindowTextA("4) Please enter minimum radius of the dot target(pixel)");
		GetDlgItem(IDC_STATIC_STEPX_COUNT)->SetWindowTextA("Distance X");
		GetDlgItem(IDC_STATIC_STEPY_COUNT)->SetWindowTextA("Distance Y");
	}
	else {
		GetDlgItem(IDC_STATIC_MSG_4)->SetWindowTextA("");
		GetDlgItem(IDC_STATIC_STEPX_COUNT)->SetWindowTextA("Step X Count");
		GetDlgItem(IDC_STATIC_STEPY_COUNT)->SetWindowTextA("Step Y Count");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CCalibInfo::OnBnClickedButtonStartPos()
{
	CApplication *pApp = (CApplication*)AfxGetApp();
	// TODO: Add your control notification handler code here
	
}

// DeviceConfig.cpp : implementation file
//

#include "stdafx.h"
#include "DeviceConfig.h"
#include "afxdialogex.h"
#include "Application.h"

// CDeviceConfig dialog

IMPLEMENT_DYNAMIC(CDeviceConfig, CDialogEx)

CDeviceConfig::CDeviceConfig(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDeviceConfig::IDD, pParent)
	, m_bHorizontalDevice(FALSE)
{
	pApp = (CApplication*) AfxGetApp();
	pMotionParms = &pApp->motionParameters;

	for(int i=0; i<MAX_DIRECTION; i++) {
		m_nNoOfBlock[i] = 1;
		m_dBlockPitch[i] = 10;
		m_nDeviceNoPerBlock[i] = 3;
		m_dDevicePitch[i] = 5;
		m_dDeviceScanArea[i] = 10;
		m_nDeviceOffset[i] = 4;
//		m_dInitialOffset = 0;
	}
}

CDeviceConfig::~CDeviceConfig()
{
}

void CDeviceConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	for (int i = 0; i<MAX_DIRECTION; i++) {
		DDX_Text(pDX, IDC_DC_NO_OF_BLOCK_X_EDIT + i, m_nNoOfBlock[i]);
		DDX_Text(pDX, IDC_DC_BLOCK_PITCH_X_EDIT + i, m_dBlockPitch[i]);
		DDX_Text(pDX, IDC_DC_DEVICE_NO_PER_BLOCK_X_EDIT + i, m_nDeviceNoPerBlock[i]);
		DDX_Text(pDX, IDC_DC_DEVICE_PITCH_X_EDIT + i, m_dDevicePitch[i]);
		DDX_Text(pDX, IDC_DC_DEVICE_SCAN_AREA_X_EDIT + i, m_dDeviceScanArea[i]);
		//DDX_Text(pDX, IDC_DC_DEVICE_OFFSET_X_EDIT+i, pApp->m_dDeviceOffset[i]);
		DDX_Text(pDX, IDC_DC_DEVICE_OVERLAP_X_EDIT + i, m_dDeviceOverlap[i]);
	}
	DDX_Check(pDX, IDC_CHECK_HORIZONTAL_DEVICE, m_bHorizontalDevice);
}


BEGIN_MESSAGE_MAP(CDeviceConfig, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDeviceConfig::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDeviceConfig::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_HORIZONTAL_DEVICE, &CDeviceConfig::OnBnClickedCheckHorizontalDevice)
END_MESSAGE_MAP()


// CDeviceConfig message handlers


BOOL CDeviceConfig::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	UpdateData(TRUE);
	
	LoadData();
	UpdateData(FALSE);

	//Disable for now, May need to add it's functionality, Girish
	for (int i = 0; i < 2; i++) {
		GetDlgItem(IDC_DC_DEVICE_SCAN_AREA_X_EDIT + i)->ShowWindow(FALSE);
		GetDlgItem(IDC_DC_DEVICE_OVERLAP_X_EDIT + i)->ShowWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDeviceConfig::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	double dTempFOVX, dTempFOVY, dTempDie;

	dTempFOVX = ceil((m_dDeviceScanArea[0] / (pApp->m_nFOVWidth - m_dDeviceOverlap[0])));
	dTempFOVY = ceil((m_dDeviceScanArea[1] / (pApp->m_nFOVHeight - m_dDeviceOverlap[1])));

	//double dTempNoOfFov = dTempFOVX * dTempFOVY;
	//if (dTempNoOfFov <.0 || dTempNoOfFov > 8.0) {
	//	AfxMessageBox("Maximum Position Per Device is 8");
	//	return;
	//}

	SaveData();
	pApp->LoadRecipes();
	UpdateData(FALSE);
	CDialogEx::OnOK();
}


void CDeviceConfig::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here

	CDialogEx::OnCancel();
}

void CDeviceConfig::LoadData()
{
	UpdateData(TRUE);
	pMotionParms->getDeviceSize(m_dBlockPitch[0], m_dBlockPitch[1]);
	pMotionParms->getDeviceDirection(m_bHorizontalDevice);
	pMotionParms->getDeviceScanArea(m_dDeviceScanArea[0], m_dDeviceScanArea[1]);
	pMotionParms->getDeviceOverlap(m_dDeviceOverlap[0], m_dDeviceOverlap[1]);

	int m_nDieXPerImage;
	int m_nDieYPerImage;

	pMotionParms->GetMappingParameters(m_nNoOfBlock[0], m_nNoOfBlock[1], m_dBlockPitch[0], m_dBlockPitch[1], m_nDeviceNoPerBlock[0], m_nDeviceNoPerBlock[1],
		m_dDevicePitch[0], m_dDevicePitch[1], m_nDieXPerImage, m_nDieYPerImage);


	UpdateData(FALSE);

}

void CDeviceConfig::SaveData()
{
	UpdateData(TRUE);
	int m_nDieXPerImage = 1;
	int m_nDieYPerImage = 1;

	pMotionParms->setDeviceSize(m_dBlockPitch[0], m_dBlockPitch[1]);
	pMotionParms->setDeviceDirection(m_bHorizontalDevice);
	pMotionParms->setDeviceScanArea(m_dDeviceScanArea[0], m_dDeviceScanArea[1]);
	pMotionParms->setDeviceOverlap(m_dDeviceOverlap[0], m_dDeviceOverlap[1]);

	pMotionParms->SetMappingParameters(m_nNoOfBlock[0], m_nNoOfBlock[1], m_dBlockPitch[0], m_dBlockPitch[1], m_nDeviceNoPerBlock[0], m_nDeviceNoPerBlock[1],
		m_dDevicePitch[0], m_dDevicePitch[1], m_nDieXPerImage, m_nDieYPerImage);

	pMotionParms->SaveDeviceParameters();

	//	UpdateData(FALSE);
	//	pMotionParms->SaveBondScanningMapping();

}

void CDeviceConfig::OnBnClickedCheckHorizontalDevice()
{
	UpdateData(TRUE);

	m_bHorizontalDevice != m_bHorizontalDevice;

	UpdateData(FALSE);
}

// ScanningTeachDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "Application.h"
#include "ScanningTeachDialog.h"
#include "MotionControlDialog.h"
#include "afxdialogex.h"


// ScanningTeachDialog dialog

IMPLEMENT_DYNAMIC(ScanningTeachDialog, CDialog)

ScanningTeachDialog::ScanningTeachDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SCANNING_TEACH_DIALOG, pParent)
{

}

ScanningTeachDialog::~ScanningTeachDialog()
{
}

void ScanningTeachDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ScanningTeachDialog, CDialog)
	ON_BN_CLICKED(IDC_SET_START_POS, &ScanningTeachDialog::OnBnClickedSetStartPos)
	ON_BN_CLICKED(IDC_MOVE_TO_START_POS, &ScanningTeachDialog::OnBnClickedMoveToStartPos)
	ON_BN_CLICKED(IDC_SET_ZREADY_POS, &ScanningTeachDialog::OnBnClickedSetZreadyPos)
	ON_BN_CLICKED(IDC_MOVE_TO_ZREADY_POS, &ScanningTeachDialog::OnBnClickedMoveToZreadyPos)
	ON_BN_CLICKED(IDC_SET_ZREADY_POS2, &ScanningTeachDialog::OnBnClickedSetZreadyPos2)
	ON_BN_CLICKED(IDC_MOVE_TO_ZREADY_POS2, &ScanningTeachDialog::OnBnClickedMoveToZreadyPos2)
	ON_BN_CLICKED(IDOK, &ScanningTeachDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// ScanningTeachDialog message handlers


void ScanningTeachDialog::OnBnClickedSetStartPos()
{
	if (IDYES == AfxMessageBox("Do you want to set current position as START Position?", MB_YESNO)) {

		if (pMotionParmsInitial != NULL) {
			pMotionParmsInitial->SetStartScanPos();
		}

		if (pMotionParms != NULL) {
			pMotionParms->SetStartScanPos();
		}
	}
}


void ScanningTeachDialog::OnBnClickedMoveToStartPos()
{
	if (theApp.m_pMotionControl != NULL) {
		pMotionParmsInitial->MoveToStartScanPos();
	}
}


void ScanningTeachDialog::OnBnClickedSetZreadyPos()
{
	if (IDYES == AfxMessageBox("Do you want to set current position as START Position?", MB_YESNO)) {

		if (pMotionParmsInitial != NULL) {
			pMotionParmsInitial->SetZTopStartScanPos();
		}

		if (pMotionParms != NULL) {
			pMotionParms->SetZTopStartScanPos();
		}
	}
}

void ScanningTeachDialog::SetMotionParameters(MotionParameters * pMotionParmsIn, MotionParameters * pMotionParmsInitialIn)
{
	if (pMotionParmsIn != NULL && pMotionParmsInitialIn != NULL) {
		pMotionParms = pMotionParmsIn;
		pMotionParmsInitial = pMotionParmsInitialIn;

		UpdateData(FALSE);
	}
}

void ScanningTeachDialog::OnBnClickedMoveToZreadyPos()
{
	if (theApp.m_pMotionControl != NULL) {
		pMotionParmsInitial->moveToZTopStartScanPosPos();
	}
}

void ScanningTeachDialog::OnBnClickedSetZreadyPos2()
{
	if (IDYES == AfxMessageBox("Do you want to set current position as START Position?", MB_YESNO)) {

		if (pMotionParmsInitial != NULL) {
			pMotionParmsInitial->SetZBottomStartScanPos();
		}

		if (pMotionParms != NULL) {
			pMotionParms->SetZBottomStartScanPos();
		}
	}
}


void ScanningTeachDialog::OnBnClickedMoveToZreadyPos2()
{
	if (theApp.m_pMotionControl != NULL) {
		pMotionParmsInitial->moveToZBottomStartScanPosPos();
	}
}


void ScanningTeachDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}

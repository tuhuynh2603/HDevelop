// ErrorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ErrorDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#include "Application.h"

static const UINT_PTR TIMER_ERROR_MESSAGE = 13;

// CErrorDlg dialog

IMPLEMENT_DYNAMIC(CErrorDlg, CDialog)

CErrorDlg::CErrorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_ERRORDLG, pParent)
	, m_strTransportErrorMsg(_T("Running"))
{
	clGoodStatus = RGB(0, 153, 0);
	clBadStatus = RGB(255, 0, 0);

	Font1.CreateFont(25, 0, 0, 0, 700, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "Arial");
	Font2.CreateFont(20, 0, 0, 0, 700, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "Arial");

	FontTitle.CreateFont(50, 0, 0, 0, 560, TRUE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "Verdana");

	bTransportErrorStatus = false;
	bErrorInSystem = false;
	bTransportShow = false;
	bTimerRun = false;

	bDoUpdate = true;
}

CErrorDlg::~CErrorDlg()
{
}

void CErrorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	/*DDX_Control(pDX, IDC_ERRORDLG_TRANSPORT_RETRY_BUTTON, m_ctrlTransportRetryButton);
	DDX_Control(pDX, IDC_ERRORDLG_TRANSPORT_STOP_BUTTON, m_ctrlTransportStopButton);*/
	DDX_Text(pDX, IDC_ERRORDLG_TRANSPORTMSG_LBL, m_strTransportErrorMsg);
	//DDX_Control(pDX, IDC_ERRORDLG_TRANSPORT_RESCAN_BUTTON, m_ctrlReScanButton);
}


BEGIN_MESSAGE_MAP(CErrorDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//ON_BN_CLICKED(IDC_ERRORDLG_TRANSPORT_RETRY_BUTTON, &CErrorDlg::OnBnClickedErrordlgTransportRetryButton)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, &CErrorDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CErrorDlg::OnBnClickedCancel)
	/*ON_STN_CLICKED(IDC_ERRORDLG_TRANSPORTMSG_LBL, &CErrorDlg::OnStnClickedErrordlgTransportmsgLbl)
	ON_BN_CLICKED(IDC_ERRORDLG_TRANSPORT_STOP_BUTTON, &CErrorDlg::OnBnClickedErrordlgTransportStopButton)
	ON_BN_CLICKED(IDC_ERRORDLG_TRANSPORT_RESCAN_BUTTON, &CErrorDlg::OnBnClickedErrordlgTransportRescanButton)*/
END_MESSAGE_MAP()



BOOL CErrorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rectPos(0, 0, 0, 0);


	GetDlgItem(IDC_ERRORDLG_PLACEHOLDER)->GetWindowRect(rectPos);
	ScreenToClient(rectPos);

	//	m_ctrlTransportRetryButton.LoadBitmaps(IDB_RETRYBUTTON, IDB_RETRYBUTTON_PRESSED, 0U, IDB_RETRY_DISABLED);

	/*m_ctrlTransportRetryButton.LoadBitmaps(IDB_CONTINUE_BUTTON, IDB_CONTINUE_BUTTON_PRESSED, 0U, IDB_RETRY_DISABLED);
	m_ctrlTransportStopButton.LoadBitmaps(IDB_STOP_BUTTON, IDB_STOP_BUTTON_PRESSED, 0U, IDB_RETRY_DISABLED);
	m_ctrlReScanButton.LoadBitmaps(IDB_BITMAP_RESCAN, IDB_BITMAP_RESCAN_PRESSED, 0U, IDB_RETRY_DISABLED);

	GetDlgItem(IDC_ERRORDLG_TRANSPORT_RESCAN_BUTTON)->EnableWindow(theApp.m_bEnableReScanOption);*/

	GetDlgItem(IDC_ERRORDLG_TRANSPORTMSG_LBL)->SetFont(&Font1);

	GetDlgItem(IDC_ERRORDLG_TITLE)->SetFont(&FontTitle);


	//	OnBnClickedErrordlgContractButton();

	return TRUE;
}

void CErrorDlg::PostModuleStatus(bool bError, CString strStatus)
{
	if (strStatus.IsEmpty())
		return;

	m_strTransportErrorMsg = (bError) ? CTime::GetCurrentTime().Format("%d %B at %H:%M:%S\n") + strStatus : strStatus;
	bTransportErrorStatus = bError;

	bErrorInSystem = bTransportErrorStatus;

	if (bErrorInSystem) {
		ShowWindow(SW_SHOW);
		if (!bTimerRun) {
			SetTimer(TIMER_ERROR_MESSAGE, 500, NULL);

			//if (theApp.pGlobalBackgroundThread->IsCurtainMuteActivated())
			//	SetTimer(TIMER_CURTAIN_MUTE, 240000, NULL);		//Activate curtain sensor on for each 4 mins. It auto truns of after each 5 mins 9 secs
			bTimerRun = true;
		}
		bDoUpdate = true;

	}
	else {
		if (bTimerRun) {
			KillTimer(TIMER_ERROR_MESSAGE);
			//KillTimer(TIMER_CURTAIN_MUTE);
			bTimerRun = false;
		}
		ShowWindow(SW_HIDE);
	}
}

void CErrorDlg::PostModuleStatus(bool bError, CString strTitle, CString strStatus)
{
	if (strStatus.IsEmpty() || strTitle.IsEmpty())
		return;

	GetDlgItem(IDC_ERRORDLG_TITLE)->SetWindowText(strTitle);

	m_strTransportErrorMsg = (bError) ? CTime::GetCurrentTime().Format("%d %B at %H:%M:%S\n") + strStatus : strStatus;
	bTransportErrorStatus = bError;

	bErrorInSystem = bTransportErrorStatus;

	if (bErrorInSystem) {
		ShowWindow(SW_SHOW);
		if (!bTimerRun) {
			SetTimer(TIMER_ERROR_MESSAGE, 500, NULL);

			//if (theApp.pGlobalBackgroundThread->IsCurtainMuteActivated())
			//	SetTimer(TIMER_CURTAIN_MUTE, 240000, NULL);		//Activate curtain sensor on for each 4 mins. It auto truns of after each 5 mins 9 secs
			bTimerRun = true;
		}
		bDoUpdate = true;

	}
	else {
		if (bTimerRun) {
			KillTimer(TIMER_ERROR_MESSAGE);
			//KillTimer(TIMER_CURTAIN_MUTE);
			bTimerRun = false;
		}
		ShowWindow(SW_HIDE);
	}
}

//void CErrorDlg::OnBnClickedErrordlgContractButton()
//{
//	CRect rectPos;
//	CRect rectWindow;
//	this->GetWindowRect(rectWindow);
//	GetDlgItem(IDC_ERRORDLG_EXPAND_BUTTON)->GetWindowRect(rectPos);
//	SetWindowPos(NULL, rectWindow.left, rectWindow.top, rectWindow.Width(), rectPos.bottom - rectWindow.top + 5, SWP_NOZORDER);
//
//	GetDlgItem(IDC_ERRORDLG_CONTRACT_BUTTON)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_ERRORDLG_EXPAND_BUTTON)->ShowWindow(SW_SHOW);
//	theApp.pNewIOControlDialog->ShowWindow(SW_HIDE);
//
//}


//void CErrorDlg::OnBnClickedErrordlgExpandButton()
//{
//	CRect rectPos;
//	CRect rectWindow;
//	this->GetWindowRect(rectWindow);
//	GetDlgItem(IDC_ERRORDLG_CONTRACT_BUTTON)->GetWindowRect(rectPos);
//	SetWindowPos(NULL, rectWindow.left, rectWindow.top, rectWindow.Width(), rectPos.bottom - rectWindow.top + 5, SWP_SHOWWINDOW);
//
//	GetDlgItem(IDC_ERRORDLG_EXPAND_BUTTON)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_ERRORDLG_CONTRACT_BUTTON)->ShowWindow(SW_SHOW);
//	theApp.pNewIOControlDialog->ShowWindow(SW_SHOW);
//
//}


HBRUSH CErrorDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	//_clGoodStatus = RGB(0,153,0);
	switch (pWnd->GetDlgCtrlID()) {

	case IDC_ERRORDLG_TRANSPORTMSG_LBL:
		pDC->SetTextColor(bTransportErrorStatus ? clBadStatus : clGoodStatus);
		break;
	}
	return hbr;
}


void CErrorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == TIMER_ERROR_MESSAGE && bTimerRun) {

		/*if (theApp.pGlobalBackgroundThread != NULL) {
			BOOL bSequenceRun = theApp.pGlobalBackgroundThread->IsRunning();

			if (!bSequenceRun) {
				bTransportErrorStatus = false;

				m_strTransportErrorMsg.Format("Sequence stopped");

				bTimerRun = false;

				bDoUpdate = TRUE;

				PostModuleStatus(false, "");
			}

			GetDlgItem(IDC_ERRORDLG_TRANSPORT_RESCAN_BUTTON)->EnableWindow(theApp.motionParameters.GetReScanStatus() && theApp.m_bEnableReScanOption);

		}*/


		if (bTransportErrorStatus) {
			bTransportShow = !bTransportShow;
			GetDlgItem(IDC_ERRORDLG_TRANSPORTMSG_LBL)->ShowWindow(SW_SHOW);
		}
		else {
			if (!bTransportShow) {
				GetDlgItem(IDC_ERRORDLG_TRANSPORTMSG_LBL)->ShowWindow(SW_SHOW);
				bTransportShow = true;
			}
		}

	}

	/*if (nIDEvent == TIMER_CURTAIN_MUTE && bTimerRun && theApp.pGlobalBackgroundThread->IsCurtainMuteActivated()) {


		int nError = theApp.ioControl.CurtainMuteOn(FALSE);

		Sleep(200);

		nError = theApp.ioControl.CurtainMuteOn(TRUE);

		if (nError) {
			Sleep(1000);
			nError = theApp.ioControl.CurtainMuteOn(TRUE);
			if (nError)
				OutputDebugLogTo(0, TRUE, "Failed to Curtain Mute On");

		}
	}*/

	if (bDoUpdate) {
		bDoUpdate = false;
		UpdateData(FALSE);
	}

	CDialog::OnTimer(nIDEvent);
}



void CErrorDlg::OnClose()
{
	ShowWindow(FALSE);
}

void CErrorDlg::OnBnClickedCancel()
{
	ShowWindow(FALSE);
}

void CErrorDlg::OnCancel()
{
	ShowWindow(FALSE);
}

void CErrorDlg::OnBnClickedOk()
{
	ShowWindow(FALSE);
}

void CErrorDlg::OnOK()
{
	ShowWindow(FALSE);
}

void CErrorDlg::ResetState(void)
{
	m_strTransportErrorMsg = _T("Running...");


	bTransportErrorStatus = false;
	bTimerRun = false;

	bTransportShow = true;

	bDoUpdate = TRUE;
}

//void CErrorDlg::OnBnClickedErrordlgTransportRetryButton()
//{
//	if (theApp.pGlobalBackgroundThread != NULL)
//		theApp.pGlobalBackgroundThread->Retry();
//}


void CErrorDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	//if(!bShow)
	//	OnBnClickedErrordlgContractButton();
}

// CErrorDlg message handlers




//void CErrorDlg::OnBnClickedErrordlgTransportStopButton()
//{
//	if (theApp.pGlobalBackgroundThread != NULL) {
//		theApp.pGlobalBackgroundThread->RemoveSequence();
//		theApp.pGlobalBackgroundThread->saveEndLotInformation();
//		ShowWindow(SW_HIDE);
//	}
//}


//void CErrorDlg::OnBnClickedErrordlgTransportRescanButton()
//{
//	if (theApp.pGlobalBackgroundThread != NULL)
//		theApp.pGlobalBackgroundThread->Retry(TRUE);
//}

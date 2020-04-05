// InfoMsgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "InfoMsgDlg.h"
#include "afxdialogex.h"
#include "Application.h"

// CInfoMsgDlg dialog

IMPLEMENT_DYNAMIC(CInfoMsgDlg, CDialogEx)

CInfoMsgDlg::CInfoMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_INFO_MESSAGE_DLG, pParent)
{

}

CInfoMsgDlg::~CInfoMsgDlg()
{
	m_fontHead.DeleteObject();
	m_fontContent.DeleteObject();
	m_Brush.DeleteObject();
}

void CInfoMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInfoMsgDlg, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CInfoMsgDlg message handlers


BOOL CInfoMsgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	CApplication* pApp = (CApplication*)AfxGetApp();
	CRect rectDesktop, rectWnd, rectFrame;
	
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rectDesktop, NULL);
	m_fontHead.CreateFont((int)(rectDesktop.Width() / 1280.0 * 16), 0, 0, 0, 350, FALSE, FALSE, FALSE,
						ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
						PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "Segoe UI Semibold");

	m_fontContent.CreateFont((int)(rectDesktop.Width() / 1280.0 * 14), 0, 0, 0, 350, FALSE, FALSE, FALSE,
							ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "Segoe UI");
	m_Brush.CreateSolidBrush(RGB(255, 255, 255));
	
	pApp->m_pFrames[0]->GetWindowRect(&rectFrame);
	GetWindowRect(&rectWnd);
	//SetWindowPos(NULL, (int)(rectDesktop.Width()/2), (int)(rectDesktop.Height()-rectWnd.Height()-10), rectWnd.Width(), rectWnd.Height(), /*SWP_NOZORDER | */SWP_NOREDRAW);
	SetWindowPos(NULL, (int)(rectFrame.right- rectWnd.Width()), (int)(rectFrame.Height()+10), rectWnd.Width(), rectWnd.Height(), /*SWP_NOZORDER | */SWP_NOREDRAW);

	BOOL bCloseEnable = FALSE;
	UINT menuEnable = bCloseEnable ? (MF_BYCOMMAND) : (MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if(pSysMenu) {
		pSysMenu->EnableMenuItem(SC_CLOSE, menuEnable);
	}

	GetDlgItem(IDC_MESSAGE_DLG_CAM_DOC_NAME)->SetFont(&m_fontHead);
	GetDlgItem(IDC_MESSAGE_DLG_INFO)->SetFont(&m_fontContent);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CInfoMsgDlg::UpdateDlgItems(CString strCamDocName, CString strInfoMsg)
{
	GetDlgItem(IDC_MESSAGE_DLG_CAM_DOC_NAME)->SetWindowTextA(strCamDocName);
	GetDlgItem(IDC_MESSAGE_DLG_INFO)->SetWindowTextA(strInfoMsg);
}

HBRUSH CInfoMsgDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (pWnd->GetDlgCtrlID() == IDC_MESSAGE_DLG_CAM_DOC_NAME) {
		pDC->SetTextColor(RGB(0, 64, 255));
		pDC->SetBkMode(TRANSPARENT);
	}
	else if (pWnd->GetDlgCtrlID() == IDC_MESSAGE_DLG_INFO) {
		pDC->SetTextColor(RGB(0, 0, 0));
		pDC->SetBkMode(TRANSPARENT);
	}
	// TODO:  Return a different brush if the default is not desired
	//return hbr;
	return m_Brush;
}

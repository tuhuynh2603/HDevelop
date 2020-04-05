// ChangePasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "ChangePasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangePasswordDlg dialog


CChangePasswordDlg::CChangePasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChangePasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangePasswordDlg)
	m_strNewPassword = _T("");
	m_strConfirmNewPassword = _T("");
	//}}AFX_DATA_INIT
}

CChangePasswordDlg::~CChangePasswordDlg()
{
	///m_brush.DeleteObject();
}


void CChangePasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangePasswordDlg)
	DDX_Text(pDX, IDC_EDIT_NEW_PASSWORD, m_strNewPassword);
	DDV_MaxChars(pDX, m_strNewPassword, 15);
	DDX_Text(pDX, IDC_EDIT_CONFIRM_PASSWORD, m_strConfirmNewPassword);
	DDV_MaxChars(pDX, m_strConfirmNewPassword, 15);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangePasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CChangePasswordDlg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangePasswordDlg message handlers

BOOL CChangePasswordDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//SetWindowText(m_title);
	CDialog::OnInitDialog();
	///m_brush.CreateSolidBrush(RGB(30, 30, 30));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChangePasswordDlg::OnOK() 
{
		
	CApplication* m_pApp = (CApplication*) AfxGetApp();
	CString strTemp;
	strTemp = m_title;
	UpdateData(TRUE);
	if(m_strNewPassword.Compare(m_strConfirmNewPassword) == 0){
		if(m_strNewPassword.GetLength()==0) {
			AfxMessageBox("Please enter a new password" ,MB_OK|MB_ICONSTOP|MB_ICONERROR);
			return;
		}
		else {
			m_pApp->m_UserAccInfo.m_strNewPassword = m_strNewPassword;
			AfxMessageBox("Password changed successfully");
			m_pApp->AddLogEvent("Password changed successfully");
			CDialog::OnOK();
			return;
		}

	}
	else {
		AfxMessageBox("Passwords do not match", MB_OK|MB_ICONSTOP|MB_ICONERROR);
		m_strNewPassword = _T("");
		m_strConfirmNewPassword = _T("");
		UpdateData(FALSE);
		return;
	}

	CDialog::OnCancel();
}

void CChangePasswordDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

HBRUSH CChangePasswordDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	//if(pWnd->GetDlgCtrlID() == IDC_STATIC_NEW_PASSWORD) {
	//    pDC->SetTextColor( RGB( 80,180,255 ));
	//    pDC->SetBkMode( TRANSPARENT );
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_STATIC_CONFIRM_PASSWORD) {
	//    pDC->SetTextColor( RGB( 80,180,255 ));
	//    pDC->SetBkMode( TRANSPARENT );
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_EDIT_NEW_PASSWORD) {
	//    pDC->SetTextColor( RGB( 255,255,255 ));
	//    pDC->SetBkColor( RGB(30,30,30) );
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_EDIT_CONFIRM_PASSWORD) {
	//    pDC->SetTextColor( RGB( 255,255,255 ));
	//    pDC->SetBkColor( RGB(30,30,30) );
	//}
	//
	//return m_brush;

	return hbr;
}

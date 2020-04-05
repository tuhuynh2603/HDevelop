// CreateNewUserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "CreateNewUserDlg.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreateNewUserDlg dialog


CCreateNewUserDlg::CCreateNewUserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateNewUserDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateNewUserDlg)
	m_strUsername = _T("");
	m_strPassword = _T("");
	m_strConfirmPassword = _T("");
	//}}AFX_DATA_INIT
	m_pUser = &((CApplication*)AfxGetApp())->m_UserAccInfo;
	bUserCreation = true;
}

CCreateNewUserDlg::~CCreateNewUserDlg()
{
	///m_brush.DeleteObject();
}

void CCreateNewUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateNewUserDlg)
	DDX_Control(pDX, IDC_ACCLEVEL_COMBO, m_ctrlAccessLevel);
	DDX_Text(pDX, IDC_EDIT_USER, m_strUsername);
	DDX_Text(pDX, IDC_EDIT_PSWD, m_strPassword);
	DDX_Text(pDX, IDC_EDIT_CONFIRM_PSWD, m_strConfirmPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateNewUserDlg, CDialog)
	//{{AFX_MSG_MAP(CCreateNewUserDlg)
	ON_CBN_SELCHANGE(IDC_ACCLEVEL_COMBO, OnSelchangeAccLevel)
	ON_EN_SETFOCUS(IDC_EDIT_USER, OnSetfocusEditUser)
//	ON_EN_SETFOCUS(IDC_EDIT_PSWD, OnSetfocusEditPswd)
//	ON_EN_SETFOCUS(IDC_EDIT_CONFIRM_PSWD, OnSetfocusEditConfirmPswd)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateNewUserDlg message handlers

void CCreateNewUserDlg::OnSelchangeAccLevel() 
{
	int nPos = m_ctrlAccessLevel.GetCurSel();
	m_ctrlAccessLevel.GetLBText(nPos, m_strAccessLevel);		
}

BOOL CCreateNewUserDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	///m_brush.CreateSolidBrush(RGB(30, 30, 30));

	
	FillComboBoxList();
	m_ctrlAccessLevel.SelectString(-1, m_strAccessLevel);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCreateNewUserDlg::FillComboBoxList()
{
	m_ctrlAccessLevel.ResetContent();

	m_ctrlAccessLevel.AddString("Administrator");
	m_ctrlAccessLevel.AddString("Technician");
	m_ctrlAccessLevel.AddString("Operator");
}


void CCreateNewUserDlg::OnOK() 
{
	CString strUsername, strUserpwd, strConfirmpwd, strLevel;
	CString strPswdLevel, strEncodePwd, strAccessLevel;
	LoginInfo NewUserAccount;

	CString strMsg, strHr, strMin, strSec;

	UpdateData(TRUE);
	int nNumOfUsers = m_pUser->nNumofUsers;

	strUsername = m_strUsername;
	strUserpwd = m_strPassword;
	strConfirmpwd = m_strConfirmPassword;
	strLevel = m_strAccessLevel;

	if(strUsername.CompareNoCase(m_pUser->DefaultUser.strUsername) == 0) {
		AfxMessageBox("The user name already exists. Please choose another user name");
		bUserCreation = false;
		return;
	}
	for(int k = 0; k < nNumOfUsers; k++) {
		if(strUsername.CompareNoCase(m_pUser->UserAccount[k].strUsername) == 0) {
			if(strLevel.CompareNoCase(m_pUser->UserAccount[k].strAccLevel) == 0) {
				AfxMessageBox("The user name already exists. Please choose another user name");
				bUserCreation = false;
				return;
			}
		}
		bUserCreation = true;
	}

	if(bUserCreation) {
		if(strUserpwd.Compare(strConfirmpwd) == 0){
			if(strUserpwd.GetLength()==0) {
				AfxMessageBox("Please enter a new password!",MB_OK|MB_ICONSTOP|MB_ICONERROR);
				return;
			}
		}
		else {
			AfxMessageBox("The passwords you typed do not match!",MB_OK|MB_ICONSTOP|MB_ICONERROR);
			strUserpwd = _T("");
			strConfirmpwd = _T("");
			UpdateData(FALSE);
			return;
		}
	}
	
	CDialog::OnOK();
}

void CCreateNewUserDlg::OnSetfocusEditUser() 
{
	
	//CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	//CApplication* m_pUser = (CApplication*)AfxGetApp();

	/*if(m_pUser->m_nTouchScreen) {
	GetDlgItem(IDC_EDIT_USER)->SetFocus();
	pMain->DisplayKeyBoard (TRUE);
	}*/
	
}

/*void CCreateNewUserDlg::OnSetfocusEditPswd() 
{
	
	//CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	//CApplication* m_pUser = (CApplication*)AfxGetApp();
	
	
	if(m_pUser->m_nTouchScreen) {
	GetDlgItem(IDC_EDIT_PSWD)->SetFocus();
	pMain->DisplayKeyBoard (TRUE);
	}

}*/

/*void CCreateNewUserDlg::OnSetfocusEditConfirmPswd() 
{
	
	//CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	//CApplication* m_pUser = (CApplication*)AfxGetApp();
	
	
	if(m_pUser->m_nTouchScreen) {
	GetDlgItem(IDC_EDIT_CONFIRM_PSWD)->SetFocus();
	pMain->DisplayKeyBoard (TRUE);
	}

}*/

HBRUSH CCreateNewUserDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	//if(pWnd->GetDlgCtrlID() == IDC_ACCLEVEL_COMBO) {
	//    pDC->SetTextColor(RGB(250,250,250));
	//    pDC->SetBkColor(RGB(30,30,30));
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_STATIC_USER_NAME) {
	//    pDC->SetTextColor( RGB(  80,180,255 ));
	//    pDC->SetBkMode( TRANSPARENT );
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_STATIC_PASSWORD) {
	//    pDC->SetTextColor( RGB(  80,180,255 ));
	//    pDC->SetBkMode( TRANSPARENT );
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_STATIC_ACCESS_LEVEL) {
	//    pDC->SetTextColor( RGB(  80,180,255 ));
	//    pDC->SetBkMode( TRANSPARENT );
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_STATIC_CONFIRM_PASSWORD) {
	//    pDC->SetTextColor( RGB(  80,180,255 ));
	//    pDC->SetBkMode( TRANSPARENT );
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_EDIT_USER) {
	//    pDC->SetTextColor( RGB( 255,255,255 ));
	//    pDC->SetBkColor( RGB( 30,30,30 ) );
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_EDIT_PSWD) {
	//    pDC->SetTextColor( RGB( 255,255,255 ));
	//    pDC->SetBkColor( RGB( 30,30,30 ) );
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_EDIT_CONFIRM_PSWD) {
	//    pDC->SetTextColor( RGB( 255,255,255 ));
	//    pDC->SetBkColor( RGB( 30,30,30 ) );
	//}
	//
	//return m_brush;

	return hbr;
}

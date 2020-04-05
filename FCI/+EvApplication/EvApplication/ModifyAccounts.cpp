// ModifyAccounts.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "ModifyAccounts.h"
#include "CreateNewUserDlg.h"
#include "DeleteUserDlg.h"
#include "StatisticsView.h"
#include "Mainframe.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModifyAccounts dialog

int nNumOfUsers = 0;

CModifyAccounts::CModifyAccounts(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyAccounts::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifyAccounts)
	m_nLogoutTime = 0;
	//}}AFX_DATA_INIT
	m_pApp = (CApplication*)AfxGetApp();
}

CModifyAccounts::~CModifyAccounts()
{
	///m_brush.DeleteObject();
}

void CModifyAccounts::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyAccounts)
	DDX_Text(pDX, IDC_LOGOUT_TIME, m_nLogoutTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModifyAccounts, CDialog)
	//{{AFX_MSG_MAP(CModifyAccounts)
	ON_BN_CLICKED(IDC_NEW_USER_BUTTON, OnCreateNewUser)
	ON_BN_CLICKED(IDC_DELETE_USER_BUTTON, OnDeleteUser)
	ON_EN_SETFOCUS(IDC_LOGOUT_TIME, OnSetfocusLogoutTime)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifyAccounts message handlers

void CModifyAccounts::OnCreateNewUser() 
{
	
	CString strUsername, strUserpwd, strConfirmpwd, strLevel;
	CString strPswdLevel, m_strAccessLevel, strEncodePwd;
	CCreateNewUserDlg dlg;
	LoginInfo NewUserAccount;

	nNumOfUsers = m_pApp->m_UserAccInfo.nNumofUsers;

	if(dlg.DoModal() == IDOK) {
		CString strMsg;
		if(dlg.bUserCreation) {
			nNumOfUsers++;
			m_pApp->m_UserAccInfo.nNumofUsers = nNumOfUsers;
			strUsername = dlg.m_strUsername;
			strUserpwd = dlg.m_strPassword;
			strConfirmpwd = dlg.m_strConfirmPassword;
			strLevel = dlg.m_strAccessLevel;

			if(strLevel.CompareNoCase("Administrator") == 0) 
				m_strAccessLevel = "ADMN";
			else if(strLevel.CompareNoCase("Technician") == 0)
				m_strAccessLevel = "TECH";
			else if(strLevel.CompareNoCase("Operator") == 0)
				m_strAccessLevel = "OPRT";

			strPswdLevel = strUserpwd + m_strAccessLevel;	
			
			strEncodePwd = m_pApp->m_UserLog.EncodePassWord(&strPswdLevel);

			NewUserAccount.strUsername = strUsername;
			NewUserAccount.strPassword = strEncodePwd;
			NewUserAccount.strAccLevel = strLevel;

			m_pApp->m_UserLog.SaveUserLoginInfo(m_pApp->m_strMainCfgDir, "LogAccount", &NewUserAccount, &nNumOfUsers);
			m_pApp->m_UserLog.LoadUserAccountData();

			strMsg.Format("New user [%s] created", strUsername); 
		}
		else
			strMsg = "Creating new user account failed";

		AfxMessageBox(strMsg, MB_OK | MB_ICONINFORMATION);
	
		m_pApp->AddLogEvent(strMsg);
	}
}

void CModifyAccounts::OnDeleteUser() 
{
	CDeleteUserDlg dlg;
	dlg.DoModal();
	///CDialog::EndDialog(0);
}

void CModifyAccounts::OnSetfocusLogoutTime() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CApplication* m_pApp = (CApplication*)AfxGetApp();
	
	
/*	if(m_pApp->m_nTouchScreen) {
	GetDlgItem(IDC_LOGOUT_TIME)->SetFocus();
	pMain->DisplayKeyBoard (TRUE);
	}
*/
}


BOOL CModifyAccounts::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	///m_brush.CreateSolidBrush(RGB(30, 30, 30));

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CModifyAccounts::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	//if(pWnd->GetDlgCtrlID() == IDC_STATIC_LOGOUT_TIME) {
	//    pDC->SetTextColor( RGB( 80,180,255 ));
	//    pDC->SetBkMode( TRANSPARENT );
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_LOGOUT_TIME) {
	//    pDC->SetTextColor( RGB( 255,255,255 ));
	//    pDC->SetBkColor( RGB( 30,30,30 ) );
	//}
	//else if(pWnd->GetDlgCtrlID() == IDC_STATIC_MINUTES) {
	//    pDC->SetTextColor( RGB( 80,180,255 ));
	//    pDC->SetBkMode( TRANSPARENT );
	//}
	//
	//return m_brush;

	return hbr;
}

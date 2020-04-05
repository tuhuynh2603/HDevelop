// DeleteUserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Application.h"
#include "DeleteUserDlg.h"
#include "StatisticsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteUserDlg dialog

int nNumUsers = 0;

CDeleteUserDlg::CDeleteUserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeleteUserDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeleteUserDlg)
	//}}AFX_DATA_INIT

	m_pApp = (CApplication*)AfxGetApp();
}

CDeleteUserDlg::~CDeleteUserDlg()
{
	///m_brush.DeleteObject();
}

void CDeleteUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteUserDlg)
	DDX_Control(pDX, IDC_USER_COMBO, m_ctrlUserName);
	//}}AFX_DATA_MAP 
}


BEGIN_MESSAGE_MAP(CDeleteUserDlg, CDialog)
	//{{AFX_MSG_MAP(CDeleteUserDlg)
	ON_BN_CLICKED(IDOK, OnDelete)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteUserDlg message handlers

BOOL CDeleteUserDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString m_strAccessLevel;
	///m_brush.CreateSolidBrush(RGB(30, 30, 30));

	FillComboBoxList();
	m_ctrlUserName.SelectString(-1, m_strAccessLevel);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeleteUserDlg::FillComboBoxList()
{
	int nUsers;

	nUsers = m_pApp->m_UserAccInfo.nNumofUsers;
	m_ctrlUserName.ResetContent();

	for(int i = 0; i < nUsers; i++) {
		m_ctrlUserName.InsertString(-1, m_pApp->m_UserAccInfo.strUserNames[i]);
	}

	m_ctrlUserName.SetCurSel(0);
//	m_ctrlUserName.AddString("Administrator");
}

void CDeleteUserDlg::OnDelete() 
{
	int k;
	CString strSel, strDeletedUser;
	LoginInfo UserAccount;

	CString strDataFileName;
	CIniFile iniFile;
	CString strFileName = "LogAccount";

	strDataFileName = m_pApp->m_strMainCfgDir + "\\" + strFileName + ".lgn";
	iniFile.SetPath(strDataFileName);
	int nSel = m_ctrlUserName.GetCurSel();

	if (m_pApp->m_UserAccInfo.UserAccount[nSel].strUsername.CompareNoCase(m_pApp->m_UserAccInfo.m_strCurUserName) == 0 &&
		m_pApp->m_UserAccInfo.UserAccount[nSel].strAccLevel.CompareNoCase(m_pApp->m_UserAccInfo.m_strCurrentAccessLevel) == 0) {
		CString str;
		str.Format("LogOut to Delete User [%s]", m_pApp->m_UserAccInfo.m_strCurUserName);
		MessageBox(str);
		return;
	}

	if(nSel >= 0) {
		m_ctrlUserName.GetWindowText(strDeletedUser);
		CString str;
		str.Format("Confirm to delete user [%s]", strDeletedUser);
		if(AfxMessageBox(str, MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
			m_ctrlUserName.DeleteString(nSel);

			m_ctrlUserName.SetCurSel(__min(nSel+1, m_ctrlUserName.GetCount()-1));

			//	BOOL bTest = iniFile.DeleteValue("NUM USER", "NoOfUsers");

			int n;
			for(n = nSel; n < m_pApp->m_UserAccInfo.nNumofUsers - 1; n++) {
				m_pApp->m_UserAccInfo.strUserNames[n] = m_pApp->m_UserAccInfo.strUserNames[n+1];
				m_pApp->m_UserAccInfo.UserAccount[n].strAccLevel = m_pApp->m_UserAccInfo.UserAccount[n+1].strAccLevel;
				m_pApp->m_UserAccInfo.UserAccount[n].strPassword = m_pApp->m_UserAccInfo.UserAccount[n+1].strPassword;
			}

			iniFile.WriteFile();
			//	iniFile.Reset();

			nNumUsers = 0;
			m_pApp->m_UserLog.SaveUserLoginInfo(m_pApp->m_strMainCfgDir, strFileName, &m_pApp->m_UserAccInfo.DefaultUser, &nNumUsers);

			for(n = 0; n < m_pApp->m_UserAccInfo.nNumofUsers - 1; n++) {
				if(n >= nSel)
					k = n + 1;
				else
					k = n;

				if(!(m_pApp->m_UserAccInfo.strUserNames[n].Compare(m_pApp->m_UserAccInfo.UserAccount[k].strUsername))) {
					nNumUsers++;
					UserAccount.strUsername = m_pApp->m_UserAccInfo.strUserNames[n];
					UserAccount.strAccLevel = m_pApp->m_UserAccInfo.UserAccount[n].strAccLevel;
					UserAccount.strPassword = m_pApp->m_UserAccInfo.UserAccount[n].strPassword;
					m_pApp->m_UserLog.SaveUserLoginInfo(m_pApp->m_strMainCfgDir, strFileName, &UserAccount, &nNumUsers);
				}
			}

			m_pApp->m_UserAccInfo.nNumofUsers -= 1; 


			if(m_pApp->m_UserAccInfo.UserAccount != NULL)
				delete [] m_pApp->m_UserAccInfo.UserAccount;
			m_pApp->m_UserAccInfo.UserAccount = NULL;
			m_pApp->m_UserAccInfo.UserAccount = new LoginInfo[m_pApp->m_UserAccInfo.nNumofUsers];


			for(k = 0; k < m_pApp->m_UserAccInfo.nNumofUsers; k++) {
				m_pApp->m_UserLog.LoadUserLoginInfo(m_pApp->m_strMainCfgDir, strFileName, &m_pApp->m_UserAccInfo.UserAccount[k], &k);
			}

			nNumUsers = 0;
			CString strMessage;
			strMessage.Format("User account [%s] Deleted", strDeletedUser); 
			AfxMessageBox(strMessage, MB_OK | MB_ICONINFORMATION);

			m_pApp->AddLogEvent(strMessage);
		}
	}

	CDialog::EndDialog(0);
}


HBRUSH CDeleteUserDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

	//if(pWnd->GetDlgCtrlID() == ID_STATIC_USER_NAME) {
	//	pDC->SetTextColor( RGB( 80,180,255 ));
	//	pDC->SetBkMode( TRANSPARENT );
	//}
	//if(pWnd->GetDlgCtrlID() == IDC_USER_COMBO) {
	//	pDC->SetTextColor(RGB(250,250,250));
	//	pDC->SetBkColor(RGB(30,30,30));
	//}

	//return m_brush;

	return hbr;
}
